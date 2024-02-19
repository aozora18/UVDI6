
/*
 desc : Serial (RS-232C) 함수
*/

#include "pch.h"
#include "SerialComm.h"
/* 송/수신 버퍼 객체 */
#include "RingBuff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : port	- [in]  통신 포트 번호 (1 ~ 255)
		baud	- [in]  통신 포트 속도 (CBR_100 ~ CBR_CBR_256000 中 입력)
		buff	- [in]  통신 포트의 송/수신 버퍼 크기
		shake	- [in]  SerialComm - HandShae 방식 (0x00 ~ 0x03)
 retn : None
*/
CSerialComm::CSerialComm(UINT8 port, UINT32 baud, UINT32 buff, ENG_SHOT shake)
{
	m_bIsOpened			= FALSE;
	m_hComm				= INVALID_HANDLE_VALUE;
	m_enCommStatus		= ENG_SCSC::en_scsc_closed;

	m_u64OpenTryTime	= 0;	/* 초기 접속 시도할 때는 바로 수행하도록 반드시 0 값 설정 */
	m_pPortBuff			= NULL;	/* 임시 수신 버퍼 초기화 (SetupComm 함수에서 할당) */

	/* I/O Pending 상태 아님 */
	m_bIsWritePending	= FALSE;
	m_bIsReadPending	= FALSE;

	m_enHandShake		= shake;	/* 통신 흐름 제어 방법 (현재는 고정 시킴) */
	m_u32BaudRate		= baud;		/* 통신 포트 속도 */
	m_u32PortBuff		= buff;		/* 통신 송/수신 포트 버퍼 크기 */

	/* 포트가 10 이상이면 아래와 같이 수행 */
	wmemset(m_tzPort, 0x00, 64);
	if (port < 10)	swprintf_s(m_tzPort, 64, L"COM%d", port);
	else			swprintf_s(m_tzPort, 64, L"\\\\.\\COM%d", port);

	/* Error Message를 저장할 버퍼 생성 */
	m_pErrMsg			= (PTCHAR)::Alloc(1024 * sizeof(TCHAR));
	/* 패킷 데이터 송/수신 버퍼 생성 */
	m_pReadBuff			= new CRingBuff;
	ASSERT(m_pReadBuff);
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CSerialComm::~CSerialComm()
{
	m_bIsOpened		= FALSE;
	m_enCommStatus	= ENG_SCSC::en_scsc_closed;

	/* 기존 통신 연결 해제 */
	Close();

	if (m_pErrMsg)		::Free(m_pErrMsg);		/* 에러 메시지 버퍼 해제 */
	if (m_pReadBuff)	delete m_pReadBuff;		/* 수신 버퍼 메모리 해제 */
	if (m_pPortBuff)	::Free(m_pPortBuff);	/* 통신 포트의 임시 수신 버퍼 메모리 해제 */
}

/*
 desc : 에러 정보 설정
 parm : msg		- [in]  에러 문자열 정보가 저장된 버퍼
		flag	- [in]  에러 코드가 있는 경우-0x01 / 에러 코드 없이 그냥 문자열만인 경우-0x00
		msg_box	- [in]  메시지 박스 출력 여부 (디버그 모드일 경우만 동작)
 retn : 없음
 정보 : 작성자-강진수 / 생성-2007.09.06 / 수정-2007.09.06
*/
void CSerialComm::PutErrMsg(PTCHAR msg, UINT8 flag, BOOL msg_box)
{
	wmemset(m_pErrMsg, 0x00, 1024);
	if (flag == 0x01)	swprintf_s(m_pErrMsg, 1024, L"[%s] %s [ERR_CODE : %d]", m_tzPort, msg, GetLastError());
	else				swprintf_s(m_pErrMsg, 1024, L"[%s] %s", msg, m_tzPort);
#ifdef _DEBUG
	if (msg_box)	AfxMessageBox(m_pErrMsg, MB_ICONSTOP|MB_TOPMOST);
	TRACE(L"Serial Error : %s\n", m_pErrMsg);
#endif // _DEBUG
	LOG_ERROR(ENG_EDIC::en_engine, m_pErrMsg);
}

/*
 desc : 통신 포트 열기 (핸들 초기화 및 Port opened)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CSerialComm::OpenDefault()
{
	m_bIsOpened		= FALSE;			/* 포트가 초기화 되지 않았다고 설정 */
	m_u64OpenTryTime= GetTickCount64();	/* 시리얼 통신 포트를 열기 시작한 시간 저장 */

	/* Read / Write Event 생성 */
	if (!CreateEvent())	return FALSE;
	/* 기존에 열려져 있으면 열기 실패로 처리 */
	if (m_hComm != INVALID_HANDLE_VALUE)
	{
		PutErrMsg(L"The port number is already opened", TRUE);
		return FALSE;
	}
	/* 포트 명을 가지고 포트 열기 시도 */
	m_hComm = CreateFile(m_tzPort,									/* Port Name */
						 GENERIC_READ | GENERIC_WRITE,				/* Desired Access */
						 0,											/* Shared Mode */
						 NULL,										/* Security */
						 OPEN_EXISTING,								/* Creation Disposition */
						 FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
						 NULL);										/* Non Overlapped */
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		PutErrMsg(L"Failed to open the com port", TRUE);
		return FALSE;
	}
	/* 통신 포트 초기화 */
	if (!ConfigPort())	return FALSE;

	/* 포트가 열렸다고 설정 */
	m_bIsOpened			= TRUE;
	/* I/O Pending 상태 아니라고 설정 */
	m_bIsWritePending	= FALSE;
	m_bIsReadPending	= FALSE;
	m_enCommStatus		= ENG_SCSC::en_scsc_opened;

	return TRUE;
}

/*
 desc : 통신 포트 열기 (핸들 초기화 및 Port opened)
 parm : block_mode	- [in]  시리얼 포트 읽기 Block Mode 여부 (0x00, 0x01)
		is_binary	- [in]  송수신 데이터 값이 바이너리 모드인지 아닌지 (TRUE : binary, FALSE : text)
 retn : TRUE or FALSE
*/
BOOL CSerialComm::OpenExt(ENG_CRTB block_mode, BOOL is_binary)
{
	m_bIsOpened			= FALSE;			/* 포트가 초기화 되지 않았다고 설정 */
	m_u64OpenTryTime	= GetTickCount64();	/* 시리얼 통신 포트를 열기 시작한 시간 저장 */

	/* Read / Write Event 생성 */
	if (!CreateEvent())	return FALSE;
	/* 기존에 열려져 있으면 열기 실패로 처리 */
	if (m_hComm != INVALID_HANDLE_VALUE)
	{
		PutErrMsg(L"The port number is already opened", TRUE);
		return FALSE;
	}
	/* 포트 명을 가지고 포트 열기 시도 */
	m_hComm = CreateFile(m_tzPort,									/* Port Name */
						 GENERIC_READ | GENERIC_WRITE,				/* Desired Access */
						 0,											/* Shared Mode */
						 NULL,										/* Security */
						 OPEN_EXISTING,								/* Creation Disposition */
						 FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
						 NULL);										/* Non Overlapped */
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		PutErrMsg(L"Failed to open the com port", TRUE);
		return FALSE;
	}

	/* 통신 포트 송신/수신 버퍼 크기 설정 */
	if (!SetupComm(m_u32PortBuff))	return FALSE;
	/* 통신 포트 내의 에러 및 버퍼 초기화 */
	if (!PurgeClear())	return FALSE;
	/* 통신선로 상에서 한 바이트가 전송되고 난 후 또 한 바이트가 전송되기까지의 시간 */
	if (!SetTimeout(block_mode))	return FALSE;	/* 최소 50 msec 읽기 시간 제한 걸음!!! */
	/* DCB 설정 */
	if (!SetCommState(8, ONESTOPBIT, NOPARITY, is_binary))	return FALSE;
	/* 통신 이벤트 감시 대상 설정 */
	if (!SetCommMask(EV_RXCHAR))	return FALSE;


	/* 포트가 열렸다고 설정 */
	m_bIsOpened			= TRUE;
	/* I/O Pending 상태 아니라고 설정 */
	m_bIsWritePending	= FALSE;
	m_bIsReadPending	= FALSE;
	m_enCommStatus		= ENG_SCSC::en_scsc_opened;

	/* 포트 Open 열었다고 로그 저장 설정 */
	PutErrMsg(L"Successfully opened the communication port");

	return TRUE;
}

/*
 desc : 시리얼 포트 닫기
 parm : 없음
 retn : TRUE or FALSE
*/
BOOL CSerialComm::Close()
{
	DWORD dwFlag = PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR;

	/* 기존에 열려져 있지 않은 경우 */
	if (m_hComm == INVALID_HANDLE_VALUE)	return FALSE;

	/* 모든 Event Mask를 없앤다. */
	SetCommMask(0);
	/* 하지 않아도 되고, 굳이 해도 되고? */
	::EscapeCommFunction(m_hComm, CLRDTR);
	::EscapeCommFunction(m_hComm, CLRRTS) ;
	/* 통신 Queue들을 초기화 */
	::PurgeComm(m_hComm, dwFlag);
	/* Event 객체 해제 */
	if (m_ovRead.hEvent)	CloseHandle(m_ovRead.hEvent);
	if (m_ovWrite.hEvent)	CloseHandle(m_ovWrite.hEvent);
	m_ovRead.hEvent		= NULL;
	m_ovWrite.hEvent	= NULL;
	/* 통신 디바이스 핸들을 닫음 */
	::CloseHandle(m_hComm);
	m_hComm = INVALID_HANDLE_VALUE;

	/* 포트가 닫혔다고 설정 */
	m_bIsOpened			= FALSE;
	/* I/O Pending 상태 아니라고 설정 */
	m_bIsWritePending	= FALSE;
	m_bIsReadPending	= FALSE;
	m_enCommStatus		= ENG_SCSC::en_scsc_closed;
	
	PutErrMsg(L"The serial communication port is closed");

	return TRUE;
}

/*
 desc : ClearCommError 호출 후 얻어진 값들을 멤버 변수에 저장
 parm : None
 retn : TRUE or FALSE
*/
BOOL CSerialComm::SetClearCommError()
{
	DWORD dwError	= 0;
	COMSTAT stStat	= {NULL};

	/* 기존 에러 및 상태 정보 초기화 */
	m_u32CommError	= 0;
	if (m_hComm && !::ClearCommError(m_hComm, &dwError, &stStat))
	{
		PutErrMsg(L"Failed to invoke the <ClearCommError> function", 0x01);
		return FALSE;
	}

	/* 에러 정보 저장 */
	m_u32CommError	= dwError;
	switch (dwError)
	{
	case CE_BREAK		:	PutErrMsg(L"The hardware detected a break condition",	0x01);	break;	// 하드웨어 중단 조건을 감지
	case CE_FRAME		:	PutErrMsg(L"The hardware detected a framing error",		0x01);	break;	// 하드웨어 프레임 에러 감지
	case CE_OVERRUN		:	PutErrMsg(L"A character-buffer overrun has occurred",	0x01);	break;	// 문자 버퍼 오버런 발생됨, 다음 문자 손실
	case CE_RXOVER		:	PutErrMsg(L"An input buffer overflow has occurred",		0x01);	break;	// 입력 버퍼 오버플로우 발생됨 (입력 버퍼의 공간이 없던지, EOF 문자를 받은 후에 문자가 있을시 발생)
	case CE_RXPARITY	:	PutErrMsg(L"The hardware detected a parity error",		0x01);	break;	// 하드웨어 패리티 에러 감지
	}

	return TRUE;
}

/*
 desc : Read / Write Event 생성 및 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CSerialComm::CreateEvent()
{
	/* Overlapped Event 객체 초기화 */
	memset(&m_ovRead,	0x00, sizeof(OVERLAPPED));
	memset(&m_ovWrite,	0x00, sizeof(OVERLAPPED));

	/* ------------------------------------------------------------------------------------------ */
	/*                               Read & Write 이벤트 생성 및 초기화                           */
	/* ------------------------------------------------------------------------------------------ */
	/* lpEventAttributes                                                                          */
    /*                                                                                            */
	/*	SECURITY_ATTRIBUTES 구조체로 선언된 변수의 주소를 명시한다. 이 매개변수에 NULL을 명시하면 */ 
	/*	생성된 이벤트 핸들은 자식 프로세스에 상속되지 않는다. SECURITY_ATTRIBUTES 구조체의 항목인 */
	/*	lpSecurityDescriptor에는 새로운 이벤트의 보안 기술자(security descriptor)에 대하여 명시하 */
	/*	도록 되어있다. 만약, 이 매개변수에 NULL을 사용하여 보안 기술자에 대한 명시를 하지 않은 경 */
	/*	우에는, 새로 생성될 이벤트 객체에 일반적인 보안 기술자가 명시된걸로 간주한다.			  */
	/*	일반적인 보안 기술자의 ACL(Access Control List)은 현재 로그인되어 있는 사용자의 로그인 정 */
	/*	보에 있는 ACL을 이용하거나 이벤트 객체를 생성한 프로세스의 ACL을 가져와 사용한다.		  */
	/*                                                                                            */
	/* bManualReset                                                                               */
	/*                                                                                            */
    /*  이 매개변수에 TRUE 를 명시하면 발생된 이벤트 정보가 계속 유지되는 이벤트 객체를 생성한다. */ 
    /*  즉, 이벤트 객체에 SetEvent 함수를 이용하여 이벤트를 설정하면 ResetEvent 함수를 호출하여 해*/
	/*  제하기 전까지 계속 이벤트가 발생한 상태로 유지된다.                                       */
    /*  반대로 이 매개변수에 FALSE 를 명시하면 자동으로 이벤트가 해제되는 이벤트 객체를 생성한다. */
    /*  즉, 이벤트를 기다리는 스레드가 이벤트가 발생한 상황을 체크하게 되면 자동으로 이벤트 정보가*/
	/*	해제된다. 따라서 별도의 ResetEvent 함수를 호출할 필요가 없다.                             */
	/*                                                                                            */
	/* bInitialState
	/*                                                                                            */
    /*  이 매개변수가 TRUE라면 이벤트 객체는 이벤트가 발생한 상태로 생성되며 FALSE이면 이벤트가 발*/
	/*  생하지 않은 상태로 생성된다.                                                              */
	/*                                                                                            */
	/* lpName                                                                                     */
	/*                                                                                            */
    /*  이 매개변수에는 이벤트 객체에 사용할 이름을 명시한다. 여기에 사용되는 이름의 길이는 MAX_- */
	/*	PATH까지로 제한되며 대소문자를 구별(Tips, tips, tipS는 서로 다른걸로 간주)한다. 만약, 명시*/
	/*	한 이름이 이미 생성된 다른 이벤트 객체에 사용되고 있다면 이미 생성된 객체와 이벤트 발생정 */
	/*	보를 공유하게 된다. 따라서 먼저 만들어진 이벤트 객체의 정보가 우선시되기 때문에 현재 사용 */
	/*	한 bManualReset, bInitialState에 명시한 정보는 무시된다. 그리고 lpEventAttributes에 NULL이*/
	/*	아닌 보안정보가 명시된 경우, 먼저 생성된 이벤트 객체의 보안 기술자에 의해서 사용되어질것인*/
	/*	지 결정되며 사용가능하다고 판단되더라도 해당 구조체의 세부항목인 보안기술자 항목에 명시된 */
	/*	정보는 무시된다. 따라서 고유한 이벤트 객체를 생성하려면 이벤트 이름을 명시할때 주의 한다. */
	/*                                                                                            */
    /*  만약, 이 매개변수에 명시한 이름이 이미 생성된 Semaphore, Mutex, Waitable timer, Job 또는  */
	/*	File-mapping 객체에 사용되고 있다면 이 함수는 실패할것이고 정확한 오류체크를 위해 GetLast-*/
	/*	Error 함수를 호출하면 ERROR_INVALID_HANDLE 값이 반환될 것이다. 즉, 하나의 객체는 다른 형식*/
	/*	의 객체와 이름을 공유할수 없고 동일한 형식의 객체에서만 공유여부가 결정되어진다.          */
	/*                                                                                            */
    /*  이 매개변수에 NULL을 명시하면 이름없는 이벤트 객체를 생성한다.                            */
	/*                                                                                            */        
    /*  추가적으로 객체의 이름을 명시할때, 시스템 전역적으로 객체의 이름을 공유하고 싶다면 사용하 */
	/*	고자 하는 이름앞에 "Global\"이라는 접두어를 사용하고 현재 세션공간에서만 사용하고 싶을때에*/
	/*	는 "Local\"이라는 접두어를 사용할수 있다."                                                */
	/*                                                                                            */ 
    /*  "Global" 또는  "Local" 이라는 접두어를 사용할수 있는 커널 객체에 대해서 좀더 자세한 내용을*/
	/*	원하신다면 아래 내용을 참고하시기 바랍니다.                                               */
	/* ------------------------------------------------------------------------------------------ */
	m_ovRead.hEvent	= ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_ovWrite.hEvent= ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!m_ovRead.hEvent || !m_ovWrite.hEvent)
	{
		if (m_ovRead.hEvent)	CloseHandle(m_ovRead.hEvent);
		if (m_ovWrite.hEvent)	CloseHandle(m_ovWrite.hEvent);
		m_ovRead.hEvent		= NULL;
		m_ovWrite.hEvent	= NULL;

		return FALSE;
	}


	return TRUE;
}

/*
 desc : 통신 포트 환경 설정
 parm : None
 retn : TRUE-열기 성공 / FALSE - 열기 실패
*/
BOOL CSerialComm::ConfigPort()
{
	/* 통신 포트 송신/수신 버퍼 크기 설정 */
	if (!SetupComm(m_u32PortBuff))	return FALSE;
	/* 통신 포트 내의 에러 및 버퍼 초기화 */
	if (!PurgeClear())	return FALSE;
	/* 통신선로 상에서 한 바이트가 전송되고 난 후 또 한 바이트가 전송되기까지의 시간 */
	if (!SetTimeout(ENG_CRTB::en_crtb_read_timeout_nonblocking))	return FALSE;	/* 최소 50 msec 읽기 시간 제한 걸음!!! */
	/* DCB 설정 */
	if (!SetCommState())	return FALSE;
	/* 통신 이벤트 감시 대상 설정 */
	if (!SetCommMask(EV_RXCHAR))	return FALSE;

	return TRUE;
}

/*
 desc : 통신 포트의 에러 및 버퍼를 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CSerialComm::PurgeClear()
{
	DWORD dwError= 0;
	DWORD dwFlag = PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR;

	/* 통신 포트의 모든 에러를 리셋 시킴 */
	if (m_hComm && !::ClearCommError(m_hComm, &dwError, NULL))
	{
		PutErrMsg(L"Failed to invoke the <ClearCommError> function", 0x01);
	}
	/* 모든 TX/RX 동작을 제한하고, 또한 버퍼의 내용을 제거 */
	if (m_hComm && !::PurgeComm(m_hComm, dwFlag))
	{
		PutErrMsg(L"Failed to invoke the <PurgeComm> function", 0x01);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : 통신 포트 송신/수신 버퍼 크기 설정
 parm : port_buff	- 통신 포트 중 송/수신 버퍼 크기 (MAX : 65534)
 retn : TRUE or FALSE
*/
BOOL CSerialComm::SetupComm(UINT32 port_buff)
{
	/* 통신 포트 송신/수신 버퍼 크기 유효성 검사 */
	if (port_buff >= 65535)
	{
		PutErrMsg(L"The Send/Recv buffer of serial port has been exceeded");
		return FALSE;
	}
	/* 송/수신 버퍼 크기 설정 */
	if (m_hComm && !::SetupComm(m_hComm, port_buff, port_buff))
	{
		PutErrMsg(L"Failed to invoke the <SetupComm> function", 0x01);
		return FALSE;
	}

	/* 설정된 송/수신 버퍼 크기 저장 */
	m_u32PortBuff	= port_buff;
	/* 통신 포트로부터 수신되는 데이터를 저장할 임시 버퍼 (통신 포트 버퍼 크기만큼 할당) */
	if (m_pPortBuff)	Free(m_pPortBuff);
	m_pPortBuff	= (PUINT8)::Alloc(m_u32PortBuff+1);
	ASSERT(m_pPortBuff);
	memset(m_pPortBuff, 0x00, m_u32PortBuff+1);

	/* 기존 수신 버퍼 메모리 해제 및 다시 할당 */
	m_pReadBuff->Destroy();
	if (!m_pReadBuff->Create(m_u32PortBuff))
	{
		PutErrMsg(L"Failed to allocate the read buffer", 0x01);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : 통신 포트로부터 데이터를 읽고/쓰는데 최대 대기 시간 값 설정 (모든 단위는 msec)
 parm : rit	- [in]  Read Interval Timeout
					byte와 byte 사이에 도착 시간을 설정하는 것으로서 설정한 시간 안에 다음 byte가
					도착하지 않으면 타임 아웃이 발생한다.
					통상 MAXDWORD란 값을 설정한다. 만약 설정하고 싶지 않으면 0을 입력한다.
		rtm	- [in]  Read Total Timeout Multiplier
					한 캐릭터를 수신하는데 걸리는 시간이다. 수신버퍼를 4KB로 잡았다고 하면 총 수신
					시간은 (ReadTotalTimeoutMultiplier) x (수신버퍼사이즈 [4096]) 가 되겠다.
		rtc	- [in]  Read Total Timeout Constant
					총 수신 시간 이외의 잉여 시간으로서 해당 시간만큼 더 기다렸는데도 데이터가
					수신완료되지 않으면 타임 아웃이 발생한다. 따라서, 읽기동작에서 지정된 수신버퍼를
					채우기 위한 총 수신 시간은 (ReadTotalTimeoutMultiplier) x (수신버퍼사이즈) +
					ReadTotalTimeoutConstant 가 될 것이다.
		wtm	- [in]  Write Total Timeout Multiplier
					쓰기 작업을 위한 총 타임아웃 시간을 계산하기 위해 곱하기 계수 지정 (단위: 밀리초)
					각각의 쓰기 작업은 쓴 바이트 수만큼이 이 값이 곱이 됨
		wtc	- [in]  Write Total Timeout Constant
					쓰기 작업을 위한 토탈 타임 아웃 시간을 계산하기 위해 밀리초 단위의 정수 지정
					쓰기 작업은 쓴 바이트 수와 WriteTotalTimeoutMultiplier 맴버의 곱을 더한 값이 됨
					WriteTotalTimeoutMultiplier와 WriteTotalTimeoutConstant 맴버의 값을 0으로 설정
					하면 WriteFile에 타임 아웃을 설정하지 않음
 retn : TRUE or FALSE
*/
BOOL CSerialComm::SetTimeout(ENG_CRTB mode)
{
	/* 기존에 열려져 있지 않은 경우 */
	if (m_hComm == INVALID_HANDLE_VALUE)	return FALSE;

	/* 통신 포트에 대한 읽기/쓰기하는데 걸리는 대기 시간 정보 설정 */
	COMMTIMEOUTS stTimeout;
	if (!::GetCommTimeouts(m_hComm, &stTimeout))
	{
		PutErrMsg(L"Failed to invoke the <GetCommTimeouts> function");
		return FALSE;
	}

	/* ---------------------------------------------------------------------------------------- */
	/*                                    타이머의 시간 설정                                    */
	/* ---------------------------------------------------------------------------------------- */
	/* 구체적으로 어떻게 타이머의 시간을 설정하면 좋을까?										*/
	/* COMMTIMEOUTS 구조체의 변수를 보면 알 수 있는 것처럼 감시하는 대상은 2종류가 있다.		*/
	/* 하나는 인터벌 시간이고 다른 하나는 토탈 시간이다.										*/
	/* 인터벌 시간은 수신할 데이터 별로 타이머 감시를 하기 위한 것이기 때문에 				    */
	/* 예를들면 9600bps로 수신할 경우 1케릭터는 1ms정도이기 때문에 여유를 봐서				    */
	/* 10ms이면 충분하다. (단 상대가 연속해서 데이터를 송신하는 경우이다.)					    */
	/* 토털 시간은 이하의 계산식으로 계산된다. 이 두개의 감시시간을 넘으면 타임아웃이 발생한다. */
	/* 타임아웃의 결과 수신 동작이나 송신 동자은 도중에 중단되고 수신일 때는 					*/
	/* 타임아웃이 발생한 시점까지의 데이터를 읽는다.											*/
	/*																							*/
	/* 수신 																					*/
	/* 	ReadIntervalTimeout은 다음에 수신할 때 까지의 최대 대기 시간을 msec로 지정한다.			*/
	/* 	ReadTotalTimeoutMuliplier와 ReadTotalTimeoutConstant로 수신 토탈 시간을				    */
	/* 	이하의 식으로 계산한다.																    */
	/* 																							*/
	/* 	수신 인터벌 시간 = ReadIntervalTimeout(msec)											*/
	/* 	수신 토탈 시간 = 																		*/
	/* 	(ReadTotalTimeoutMuliplier * 수신할 Byte수) + ReadTotalTimeoutConstant(msec)			*/
	/* 																							*/
	/* 송신 																					*/
	/* 	송신에는 데이터 간격 타이머는 없다.														*/
	/* 	WriteTotalTimeoutMuliplier와 WriteTotalTimeoutConstant로 송신 토탈 시간을				*/
	/* 	이하의 식으로 계산한다.																	*/
	/* 																							*/
	/* 	송신 토탈 시간 = 																		*/
	/* 	(WriteTotalTimeoutMuliplier * 송신할 Byte수) + WriteTotalTimeoutConstant(msec)			*/
	/* ---------------------------------------------------------------------------------------- */

	/* ----------------------------------------------------------------------------------------------------------------------- */
	/* ReadTotalTimeoutMultiplier와 ReadTotalTimeoutConstant 함수 둘다 0일 경우 어떤 데이터도 받지 않았더라도 리턴한다.        */
	/* WriteTotalTimeoutMultiplier 와 WriteTotalTimeoutConstant 값이 둘다 0 이라면 어떤 데이터도 송신하지 않았더라도 리턴한다. */
	/* ----------------------------------------------------------------------------------------------------------------------- */
	switch (mode)
	{
	case ENG_CRTB::en_crtb_read_timeout_blocking		:
		stTimeout.ReadIntervalTimeout		= 0;
		break;

	case ENG_CRTB::en_crtb_read_timeout_nonblocking	:
		stTimeout.ReadIntervalTimeout		= UINT32_MAX;
		stTimeout.ReadTotalTimeoutMultiplier= 0;
		stTimeout.ReadTotalTimeoutConstant	= 0;
		break;
	}
	if (!::SetCommTimeouts(m_hComm, &stTimeout))
	{
		PutErrMsg(L"Failed to invoke the <SetCommTimeouts> function");
		return FALSE;
	}

	/* -------------------------------------------------------------------------------------------- */
	/* COM Port로부터 1 바이트 데이터 읽는 1 ms 초 정도 소요 된다.                                  */
	/* -------------------------------------------------------------------------------------------- */

	/* -------------------------------------------------------------------------------------------- */
	/* 타이머의 시간 설정																			*/
	/* -------------------------------------------------------------------------------------------- */
	/* ReadIntervalTimeout과 ReadTotalTimeoutMultilier를 MAXDWORD로 설정하고						*/
	/* MAXDWORD > ReadTotalTimeoutConstant > 0으로 설정하면											*/
	/* ReadFile함수가 사용될 때 아래와 같이 된다.													*/
 	/*																								*/
	/* 	- 입력 버퍼에 수신 데이터가 있으면 ReadFile은 바로 버퍼의 문자를 반환한다.					*/
	/* 	- 입력 버퍼에 수신 데이터가 없으면 ReadFile은 수신할 때까지 기다렸다가 반환한다.				*/
	/* 	- ReadTotalTimeoutConstant에 의해 지정된 시간 이내에 1바이트라도 수신하지 않으면				*/
	/*																								*/
	/* ReadFile은 타임아웃이 된다.																	*/
	/*																								*/
	/* -------------------------------------------------------------------------------------------- */
	/* 타이머의 시간 설정																			*/
	/* -------------------------------------------------------------------------------------------- */
	/* 	- 인터벌 시간																				*/
	/* 	- 토탈 시간																					*/
	/* 	- 이 두개의 감시시간을 넘으면 타임아웃이 발생한다.											*/
	/* 	- 타임 아웃이 발생하면 수신이나, 송신은 도중에 동작한다.										*/
	/* 	- 수신일 경우, 타임 아웃 발생 지점까지 데이터를 읽는다.										*/
	/* 	- 송신일 경우, 처음부터 다시 전송. 인터벌 시간이 없다.										*/
	/*																								*/
	/* 인터벌 시간은 수신할 데이터 별로 타이머 감시를 하기 위한 것이기 때문에 1byte는 1ms정도		*/
	/* 이므로 적당히 잡아 주면 된다.																*/
	/*																								*/
	/* -------------------------------------------------------------------------------------------- */
	/* 수신																							*/
	/* -------------------------------------------------------------------------------------------- */
	/*																								*/
	/* 	- ReadIntervalTimeout : 다음에 수신할 때 까지의 최대 대기 시간을 msec로 지정한다.			*/
	/* 	- ReadTotalTimeoutMuliplier와 ReadTotalTimeoutConstant										*/
 	/* 																								*/
	/* 	Read Interval Time = ReadIntervalTimeout													*/
	/* 	Read Total time = (ReadTotalTimeoutMuliplier * 수신할 Byte수) + ReadTotalTimeoutConstant	*/
 	/* 																								*/
	/* -------------------------------------------------------------------------------------------- */
	/* 송신																							*/
	/* -------------------------------------------------------------------------------------------- */
	/*																								*/
	/* 	- 송신에는 Interval Time이 없다.																*/
	/* 	- WriteTotalTimeoutMuliplier와 WriteTotalTimeoutConstant										*/
	/* 	Write Total time = (WriteTotalTimeoutMuliplier * 송신할 Byte수) + WriteTotalTimeoutConstant	*/
	/* -------------------------------------------------------------------------------------------- */

	/* --------- */
	/* 설정 예 1 */
	/* -------------------------------------------------------------------------------------------- */
	/* 새로운 데이터(패킷)가 도착한 경우, 읽는 동작을 대기시간 없이 즉시 수행하는 구조체 설정 방법  */
	/* -------------------------------------------------------------------------------------------- */
	/* timeouts.ReadIntervalTimeout			= MAXDWORD; 											*/
	/* timeouts.ReadTotalTimeoutMultiplier	= 0;													*/
	/* timeouts.ReadTotalTimeoutConstant	= 0;													*/
	/* timeouts.WriteTotalTimeoutMultiplier	= 0;													*/
	/* timeouts.WriteTotalTimeoutConstant	= 0;													*/
	/* -------------------------------------------------------------------------------------------- */

	/* --------- */
	/* 설정 예 2 */
	/* -------------------------------------------------------------------------------------------- */
	/* ReadTotalTimeoutConstant의 값만 Timeout 값으로 의미가 있음. 10 msec가 지나면 타임아웃 발생   */
	/* -------------------------------------------------------------------------------------------- */
	/* timeouts.ReadIntervalTimeout			= MAXDWORD; 											*/
	/* timeouts.ReadTotalTimeoutMultiplier	= 0;													*/
	/* timeouts.ReadTotalTimeoutConstant	= 10;													*/
	/* timeouts.WriteTotalTimeoutMultiplier	= 0;													*/
	/* timeouts.WriteTotalTimeoutConstant	= 0;													*/
	/* -------------------------------------------------------------------------------------------- */

	/* --------- */
	/* 설정 예 3 */
	/* -------------------------------------------------------------------------------------------- */
	/* 수신할 바이트를 수신할 때까지 반환하지 않는 설정 (CTRL+Alt+Del 키로만 빠져나갈 수 있음)      */
	/* -------------------------------------------------------------------------------------------- */
	/* timeouts.ReadIntervalTimeout			= 0; 													*/
	/* timeouts.ReadTotalTimeoutMultiplier	= 0;													*/
	/* timeouts.ReadTotalTimeoutConstant	= 0;													*/
	/* timeouts.WriteTotalTimeoutMultiplier	= 0;													*/
	/* timeouts.WriteTotalTimeoutConstant	= 0;													*/
	/* -------------------------------------------------------------------------------------------- */

	/* --------- */
	/* 설정 예 4 */
	/* -------------------------------------------------------------------------------------------- */
	/* 만약 20ms 마다 Serial data를 수신하여 처리하는 코드의 경우, 20ms가 지났는데도 수신된 data가  */
	/* 없을 경우에 timeout이 발생하도록 하고 싶다면 (즉, data가 수신될 때까지 계속 이상 기다리지 않 */
	/* 고 코드의 다음 라인을 진행하게 하고 싶다면)                                                  */ 
	/* -------------------------------------------------------------------------------------------- */
	/* timeouts.ReadIntervalTimeout			= MAXDWORD; 											*/
	/* timeouts.ReadTotalTimeoutMultiplier	= 0;													*/
	/* timeouts.ReadTotalTimeoutConstant	= 20; (혹은 Windows가 실시간성을 보장하지 않으므로 100) */
	/* timeouts.WriteTotalTimeoutMultiplier	= 0;													*/
	/* timeouts.WriteTotalTimeoutConstant	= 0;													*/
	/* -------------------------------------------------------------------------------------------- */

	return TRUE;
}

/*
 desc : DCB 설정
 parm : byte_size	- 송신/수신 바이트 크기 단위 (즉, 5 ~ 8 bit)
		stop_bit	- 정지 비트 값
		parity		- 패리티 비트 값
		is_binary	- 송수신 데이터 값이 바이너리 모드인지 아닌지
 retn : TRUE or FALSE
*/
BOOL CSerialComm::SetCommState(UINT8 byte_size, UINT8 stop_bit, UINT8 parity, BOOL is_binary)
{
	/* 현재 통신 포트에 설정된 값 가져오기 */
	DCB stDCB	= {NULL};
	stDCB.DCBlength	= sizeof(DCB);
	if (!::GetCommState(m_hComm, &stDCB))
	{
		PutErrMsg(L"Failed to invoke the <GetCommState> function", 0x01);
		return FALSE;
	}

	/* DCB 환경 정보 설정 (통신 포트에 대한 속도 설정) */
	stDCB.BaudRate	= m_u32BaudRate;
	stDCB.ByteSize	= byte_size;
	stDCB.Parity	= parity;
	stDCB.StopBits	= stop_bit;
	stDCB.fBinary	= is_binary;
	stDCB.fParity	= (parity != 0);
	/* 흐름제어를 사용하지 않으므로 */
	stDCB.fInX		= FALSE;
	stDCB.fOutX		= FALSE;
#if 0
	stDCB.XonChar	= 0;
	stDCB.XoffChar	= 0;
#endif
	/* 기본 값 설정 */
	stDCB.fOutxCtsFlow	= FALSE;				/* Disable CTS monitoring */
	stDCB.fOutxDsrFlow	= FALSE;				/* Disable DSR monitoring */
	stDCB.fDtrControl	= DTR_CONTROL_DISABLE;	/* Disable DTR monitoring */
	stDCB.fRtsControl	= RTS_CONTROL_DISABLE;	/* Disable RTS (Ready To Send) */
	stDCB.XonLim		= 2048;
	stDCB.XoffLim		= 1024;

	/* 통신 흐름 제어 값에 따라 */
	switch (m_enHandShake)
	{
	case ENG_SHOT::en_shot_led_can		:
	case ENG_SHOT::en_shot_strobe_camera	:
	case ENG_SHOT::en_shot_handshake_off	:
	case ENG_SHOT::en_shot_handshake_jted	:
		break;
	case ENG_SHOT::en_shot_handshake_hw	:
		stDCB.fOutxCtsFlow	= TRUE;			/* Enable CTS monitoring */
		stDCB.fOutxDsrFlow	= TRUE;			/* Enable DSR monitoring */
		break;
	case ENG_SHOT::en_shot_handshake_sw	:
		stDCB.fOutX			= TRUE;			/* Enable XON/XOFF for transmission */
		stDCB.fInX			= TRUE;			/* Enable XON/XOFF for receiving */
		stDCB.XonLim		= 2048;
		stDCB.XoffLim		= 1024;
		break;
	case ENG_SHOT::en_shot_img_opt_lens	:
		stDCB.StopBits		= 2;			/* 이런 경우는 또 무슨? (정말 가지가지 한다.) */
		break;
	}

	/* 통신 포트 상태 값 재설정 */
	if (!::SetCommState(m_hComm, &stDCB))
	{
		PutErrMsg(L"Failed to invoke the <SetCommState> function", 0x01);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : 통신 장치의 어떤 이벤트들을 감시할 것인지 지정
 parm : evt_mask	- [in]  활성화 할 이벤트 지정 즉, 감시할 대상 이벤트 설정
							0 값인 경우, 모든 이벤트를 비활성화 (즉, 감시 안함)
							여러 개의 값으로 OR 연산될 수 있음

							EV_BREAK	: 입력에서 브레이크 검색
							EV_CTS		: CTS (clear-to-send)신호 상태 전환
							EV_DSR		: DSR (data-set-ready)신호 상태 전환
							EV_ERR		: 라인상태 에러 발생
										  라인상태 에러 : CE_FRAME, CE_OVERRUN, and CE_RXPARITY
							EV_RING		: 전화벨 신호가 감지
							EV_RLSD		: RLSD(reveive-line-signal-detect 수신선 신호 검색됨)상태 가 변경
							EV_RXCHAR	: 새로운 문자가 수신되어 입력 버퍼에 있음
							EV_RXFLAG	: 이벤트 문자가 수신되어 입력 버퍼에 있음
							EV_TXEMPTY	: 송신버퍼에 마지막 문자가 전송 됨
										  즉, WriteFile 호출 후 실제로 모든 데이터가 원격으로 전송이
										  완료되었을 때, 이벤트를 발생시킴
 retn : TRUE or FALSE
*/
BOOL CSerialComm::SetCommMask(UINT32 evt_mask)
{
	if (m_hComm && !::SetCommMask(m_hComm, evt_mask))
	{
		PutErrMsg(L"Failed to invoke the <SetCommMask> function", 0x01);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : 원하는 이벤트가 발생 했는지 무한 대기 ...
 parm : None
 retn : 감지된 이벤트 코드 값 반환 (EV_~ 즉, EV_RXCHAR, EV_RXFLAG, EV_TXEMPTY, EV_BREAK, EV_ERR, ...)
		0x0000	- 발생된 이벤트 없다. (이벤트 감시 기능을 해제할 때 반환됨)
*/
UINT16 CSerialComm::WaitCommEvent()
{
	DWORD dwEventMask	= 0;

	/* 통신 포트로부터 수신되는 이벤트 대기 */
	if (!::WaitCommEvent(m_hComm, &dwEventMask, NULL))
	{
		/* 기타 에러가 발생한 경우 */
		return EV_ERR;
	}

	/* 포트로부터 수신된 데이터 시그널이 감지됨 */
	if (EV_RXCHAR == (dwEventMask & EV_RXCHAR))			return EV_RXCHAR;
	/* 포트로부터 송신 완료 시그널이 감지됨 */
	else if (EV_TXEMPTY == (dwEventMask & EV_TXEMPTY))	return EV_TXEMPTY;

	return 0x0000;
}

/*
 desc : WaitCommEvent를 강제로 깨운다. 즉, While loop 빠져나오기 위함
 parm : None
 retn : None
*/
VOID CSerialComm::WaitCommEventBreak()
{
	if (m_hComm && !::SetCommMask(m_hComm, 0))
	{
		PutErrMsg(L"Failed to invoke the <SetCommMask> function", 0x01);
	}
}

/*
 desc : 내부 버퍼에 저장된 데이터 크기 반환
 parm : None
 retn : 반환 가능한 데이터의 크기 반환 (단위: Bytes)
*/
UINT32 CSerialComm::GetReadSize()
{
	if (m_pReadBuff)	return (UINT32)m_pReadBuff->GetReadSize();
	return 0;
}

/*
 desc : 통신 포트에서 데이터 수신 처리
 parm : time_out	- [in]  통신 포트 버퍼로부터 읽어들이는데 총 대기 시간 (단위: msec)
							주어진 시간만큼 총 읽어들이는데 사용 됨
 retn : 읽어서 버퍼에 저장된 데이터 크기, 0 - 읽기 실패
*/
UINT32 CSerialComm::ReadData(UINT32 time_out)
{
	BOOL bReaded		= FALSE;
	DWORD dwQueBytes, dwReadBytes, dwReadTotal = 0, dwErrorFlags;
	UINT64 u64ReadTime	= GetTickCount64();
	PUINT8 pPortBuff	= m_pPortBuff;
	COMSTAT stComStat	= {NULL};

	m_enCommStatus	= ENG_SCSC::en_scsc_reading;

	/* 에러도 검출하지만, 수신된 데이터가 있는지 확인 */
	ClearCommError(m_hComm, &dwErrorFlags, &stComStat);
	/* 시스템 큐에는 있지만, 버퍼에는 데이터가 없다 ? */
	if (stComStat.cbInQue < 1)	return 0;

	/* 버퍼에 있는 모든 데이터를 읽어 버린다. */
	do {

		/* 시스템 큐에 수신된 데이터가 있는지 확인 (버퍼 크기가 작은 것 기준 읽기) */
		dwQueBytes	= min (m_u32PortBuff, stComStat.cbInQue);

		/* 버퍼에 일단 읽어들이는데, 만일 읽어들인 값이 없다면 */
		bReaded = ReadFile(m_hComm, pPortBuff, dwQueBytes, &dwReadBytes, &m_ovRead);
		if (!bReaded)
		{
			/* 읽을 거리가 남았다면 ... (읽기 대기가 걸린 경우, 약간 지연) */
			if (GetLastError() == ERROR_IO_PENDING)
			{
				/* time_out 정해준 시간만큼 기다려 준다 */
				while (!GetOverlappedResult(m_hComm, &m_ovRead, &dwReadBytes, TRUE))
				{
					m_u32CommError = GetLastError();
					if (m_u32CommError != ERROR_IO_INCOMPLETE)
					{
						dwReadBytes	= 0;
						ClearCommError(m_hComm, &dwErrorFlags, &stComStat);
						break;
					}
				}
			}
			else
			{
				dwReadBytes	= 0;
			}
		}

		if (dwReadBytes)
		{
			/* 현재까지 읽어들인 데이터의 크기 저장 */
			dwReadTotal	+= dwReadBytes;
			/* 그리고, 이어서 수신되는 데이터를 저장할 버퍼 포인터 이동 */
			pPortBuff	+= dwReadBytes;
			/* 혹시 더 읽어들일 데이터가 있는지 확인 */
			ClearCommError(m_hComm, &dwErrorFlags, &stComStat);
		}

		/* 읽어들이는데, 현재 설정된 시간보다 길면, 자동으로 루프 빠져나감 */
		if (GetTickCount64() > (u64ReadTime + time_out))
		{
			dwReadTotal	= 0;	/* 읽기 실패 했다고 설정 */
			break;
		}

	/* 더 읽어들일 데이터가 있는지 확인 or 더 이상 읽어서 저장할 버퍼 공간이 없는지 확인 */
	} while (dwReadBytes && ((dwReadTotal + stComStat.cbInQue) < m_u32PortBuff));

	/* IO Pending 플래그 초기화 */
	m_bIsReadPending	= FALSE;

	/* -------------------------------------------------------------------------------------- */
	/* 포트로부터 읽어들일 데이터의 크기가 수신 버퍼 (Ring Buffer) 의 여유보다 크지 않아야 함 */
	/* -------------------------------------------------------------------------------------- */
	if (dwReadTotal && dwReadTotal < m_pReadBuff->GetWriteSize())
	{
		/* 포트로부터 정상적으로 읽어들인 데이터를 수신 버퍼(Ring Buffer)에 저장 */
		m_pReadBuff->WriteBinary(m_pPortBuff, dwReadTotal);
	}

	/* 데이터 읽기 성공 */
	m_enCommStatus	= ENG_SCSC::en_scsc_readed;

	return dwReadTotal;
}

/*
 desc : 통신 포트로 데이터 송신 처리
 parm : data	- [in]  1 Byte 송신 데이터
		time_out- [in]  송신하는데, 총 소요되는 시간 설정 (단위: msec)
						이 시간동안 송신하지 않으면 실패 처리
 retn : 0 - 송신 실패, 양수 - 송신 크기
*/
UINT32 CSerialComm::WriteByte(UINT8 data, UINT32 time_out)
{
	BOOL bSendSucc	= TRUE;
	DWORD dwSentByte	= 0, dwErrorFlags	= 0;
	UINT64 u64WriteTime	= GetTickCount64();
	COMSTAT stComStat	= {NULL};

	/* 아직도 송신할 데이터가 남아 있는지 확인 (통신 포트의 모든 에러를 Reset 수행 */
	ClearCommError(m_hComm, &dwErrorFlags, &stComStat);
	
	do {

		/* 데이터 송신 실패할 경우, IO_PENDING인지 조사 */
		if (!WriteFile(m_hComm, &data, 1, &dwSentByte, &m_ovWrite))
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				/* 1초 동안 쓰기 이벤트 대기 후, 판단 */
				if (WaitForSingleObject(m_ovWrite.hEvent, 100) != WAIT_OBJECT_0)
				{
					bSendSucc	= FALSE;
					LOG_ERROR(ENG_EDIC::en_engine, L"Enter - WaitForSingleObject - wait_fail (send_fail)");
				}
				else
				{
					/* 송신 완료 데이터가 있는지 확인 */
					GetOverlappedResult(m_hComm, &m_ovWrite, &dwSentByte, FALSE);
					if (dwSentByte < 1)
					{
						bSendSucc	= FALSE;
						LOG_ERROR(ENG_EDIC::en_engine, L"Enter - WaitForSingleObject - wait_succ (send_fail)");
					}
				}
			}
			else
			{
				bSendSucc	= FALSE;
				TCHAR tzMesg[256] = {NULL};
				swprintf_s(tzMesg, 256, L"Enter - WaitForSingleObject - write_fail. err_cd=%d", GetLastError());
				LOG_ERROR(ENG_EDIC::en_engine, tzMesg);
			}
		}

		/* 송신하는데 시간이 초과되었는지 여부 */
		if (dwSentByte < 1 && GetTickCount64() > (u64WriteTime + time_out))
		{
			bSendSucc	= FALSE;
			dwSentByte	= 0;
			/* 무조건 루프 빠져 나감 */
			break;
		}

	/* 모두 전송될 때까지 루프 반복 (송신 실패이거나, 1 바이트 전송 완료될 때까지) */
	} while (!(bSendSucc && dwSentByte == 1));

	/* 다시 한번 통신 포트의 모든 에러를 Reset 함 */
	ClearCommError(m_hComm, &dwErrorFlags, &stComStat);

	/* IO Pending 플래그 초기화 */
	m_bIsWritePending	= FALSE;

	/* 송신 성공 */
	if (bSendSucc)	m_enCommStatus	= ENG_SCSC::en_scsc_writed;
	else			m_enCommStatus	= ENG_SCSC::en_scsc_write_fail;

	/* 송신이 성공적으로 이루어졌고, 모두 전송 했는지 여부 값 */
	return bSendSucc;
}

/*
 desc : 통신 포트로 데이터 송신 처리
 parm : buff		- [in]  송신 데이터가 저장된 버퍼
		size		- [both]송신 데이터의 크기 및 송신된 데이터의 크기 저장
		time_byte	- [in]  패킷 중 1 Byte 씩 보낼 때마다 약간의 Term을 주기 위한 시간 값 (단위: 밀리초)
		time_step	- [in]  현재 패킷 전체를 보내고 난 이후 바로 이어서 보낼 수 없도록 일정 시간 대기하도록 함
 retn : 0 - 송신 실패, 양수 - 송신 크기
*/
UINT32 CSerialComm::WriteData(PUINT8 buff, UINT32 size, UINT32 time_byte, UINT32 time_step)
{
	UINT32 u32Retry	= 0, i = 0,	u32SendBytes = size, u32SendedBytes = 0;
	PUINT8 pBuff	= buff;

	m_enCommStatus	= ENG_SCSC::en_scsc_writing;

	/* 데이터 크기만큼 통신 포트로 데이터 송신 시도 */
	for (; (i<u32SendBytes) && (u32Retry < 3); i++)
	{
		if (!WriteByte(pBuff[i]))	u32Retry++;
		else						u32SendedBytes++;
		/* 원래는 통신 속도이어야 하나, 그지같은 장비 때문에 별도로 설정을 해줘야 한다 */
		/* 1 Bytes 씩 송신할 때, 대기 시간 */
		if (time_byte)	WaitPeekMessageQueue(time_byte);
	}
	/* 1 개의 패킷 단위로 연속적으로 전송할 수 없도록 대기하기 위함 (JusTek Board의 문제 때문에... 정말 더럽다.) */
	if (time_step)	WaitPeekMessageQueue(time_step);
	/* 통신 포트로 정상적으로 송신되었다면, 송신된 데이터 크기 반환 */
	return u32SendedBytes;
}

/*
 desc : 내부적으로 Event Message는 처리하면서, 일정 시간동안 대기
 parm : wait	- [in]  대기 시간 (단위: 밀리초)
 retn : None
*/
VOID CSerialComm::WaitPeekMessageQueue(UINT64 wait)
{
	if (wait < 1)	return;
	UINT64 u64Tick	= GetTickCount64();
	MSG stMsg		= {NULL};  /* 정보를 받을 MSG 구조의 변수 선언 */

	do 	{

		/* PeekMessage 함수는 메시지 큐에 이벤트 정보가 있다면, */
		/* 해당 이벤트 메시지를 큐에서 가져와 정상적으로 처리함 */
		if (PeekMessage(&stMsg, NULL, 0, 0, PM_REMOVE) == TRUE)
		{
			/* PM_REMOVE = 처리후에 큐에서 메시지 제거 */
			TranslateMessage(&stMsg);
			DispatchMessage(&stMsg);
		}

		/* 1 밀리초 마다 Wait 시킴 */
		uSleep(1);
		/* 일정 시간 (밀리초)동안 대기를 했는지 검사 */
		if ((GetTickCount64() - u64Tick) > wait)	break;

	} while (1);
}

/*
 desc : micro 단위까지 wait 시킴
 parm : parm : usec	- [in]  대기(Sleep) 하고자 하는 시간 (단위: micro-second = 1 / 1000000 초)
 retn : None
*/
VOID CSerialComm::uSleep(UINT64 usec)
{
	/* Microsecond (1 / 1000000초)까지 측정하기 위한 변수 Type */
	LARGE_INTEGER perf, start, now;

	/* GetTickCount함수는 10 msec 이상 오차 발생, 이 보다 더 정확한 함수는 아래 함수 */
	if (!QueryPerformanceFrequency(&perf))	return Sleep((UINT32)usec);
	if (!QueryPerformanceCounter(&start))	return Sleep((UINT32)usec);

	do {

		QueryPerformanceCounter((LARGE_INTEGER*)&now);

	  /* 1 마이크로 초 (1 / 1000000) 동안 비교하여, 초과되었으면 루프 빠져 나감 */
	} while ((now.QuadPart - start.QuadPart) / (perf.QuadPart / 1000000.0f) <= usec);
}

/*
 desc : 현재 통신 포트의 환경 설정 값 반환
 parm : data	- [out] 반환되어 저장될 구조체 참조
 retn : TRUE or FALSE
*/
BOOL CSerialComm::GetConfigPort(STG_SSCI &data)
{
	DCB stDCB			= {NULL};
	COMMTIMEOUTS stCTS	= {NULL};

	/* DCB 값 얻기 */
	if (!::GetCommState(m_hComm, &stDCB))
	{
		PutErrMsg(L"Failed to invoke the <GetCommState> function", 0x01);
		return FALSE;
	}
	/* 통신 포트에 대한 읽기/쓰기하는데 걸리는 대기 시간 정보 얻기 */
	if (!::GetCommTimeouts(m_hComm, &stCTS))
	{
		PutErrMsg(L"Failed to invoke the <GetCommTimeouts> function");
		return FALSE;
	}

	/* 현재 설정된 값을 참조 구조체에 반환 */
	data.byte_size				= stDCB.ByteSize;
	data.stop_bits				= stDCB.StopBits;
	data.is_binary				= stDCB.fBinary;
	data.is_parity				= stDCB.fParity;
	data.parity					= stDCB.Parity;

	data.port_buff				= m_u32PortBuff;
	data.read_Interval_time		= stCTS.ReadIntervalTimeout;
	data.read_total_multiple	= stCTS.ReadTotalTimeoutMultiplier;
	data.read_const_time		= stCTS.ReadTotalTimeoutConstant;
	data.write_total_multiple	= stCTS.WriteTotalTimeoutMultiplier;
	data.write_const_time		= stCTS.WriteTotalTimeoutConstant;

	return TRUE;
}

/*
 desc : 현재 통신 포트의 환경 설정 값 변경
 parm : data	- [in]  변경된 설정 값이 저장된 구조체
 retn : TRUE or FALSE
*/
BOOL CSerialComm::SetConfigPort(LPG_SSCI data)
{
	BOOL bSucc	= TRUE;

	if (!m_bIsOpened)
	{
		PutErrMsg(L"Can't opened the serial port");
		return FALSE;
	}

	/* Setup Send & Recv Buffer size */
	if (bSucc)	bSucc = SetupComm(data->port_buff);
	/* Setup Comm timeouts */
#if 0
	if (bSucc)	bSucc = SetTimeout(data->read_Interval_time,
								   data->read_total_multiple, data->read_const_time,
								   data->write_total_multiple, data->write_const_time);
#else
	if (bSucc)	bSucc = SetTimeout(ENG_CRTB::en_crtb_read_timeout_nonblocking);
#endif
	/* Setup Comm status */
	if (bSucc)	bSucc = SetCommState(data->byte_size, data->stop_bits, data->parity, data->is_binary);

	return bSucc;
}

/*
 desc : 수신 버퍼의 데이터 반환 (값을 복사해서 반환하는 것이 아니라, 잘라내기 한 후 반환 한다)
		값을 반환한 후 수신 버퍼는 반환 이후의 데이터만 남아 있게 된다
 parm : data	- [out] 반환되어 저장될 버퍼
		size	- [in]  반환되는 데이터의 크기
 retn : 0 - 반환되는 데이터가 없다 (반환 실패), 양수 - 반환되는 데이터의 크기
*/
UINT32 CSerialComm::PopReadData(PUINT8 data, UINT32 size)
{
	UINT32 u32PopSize	= (UINT32)m_pReadBuff->GetReadSize();
	if (u32PopSize < 1)	return 0;

	/* 남아있는 데이터의 크기가 반환 요청된 데이터의 크기보다 큰 경우, 요청된 크기만큼만 반환하기 위함 */
	if (size < u32PopSize)	u32PopSize	= size;

	/* 데이터 복사 */
	if (!m_pReadBuff->ReadBinary(data, u32PopSize))
	{
		PutErrMsg(L"Failed to get the received data");
		return 0;
	}

	return u32PopSize;
}

/*
 desc : 수신 버퍼의 데이터 반환 (값을 복사해서 반환하는 것임, 수신 버퍼는 내용이 그대로 남아 있음)
		값을 반환한 후 수신 버퍼는 반환 이후의 데이터만 남아 있게 된다
 parm : data	- [out] 반환되어 저장될 버퍼
		size	- [in]  반환되는 데이터의 크기
 retn : FALSE - 반환 실패, TRUE - 요청한 크기만큼 반환 성공
*/
BOOL CSerialComm::CopyReadData(PUINT8 data, UINT32 size)
{
	UINT32 u32CopySize	= (UINT32)m_pReadBuff->GetReadSize();

	/* 만약 남아 있는 데이터 크기보다 요청한 데이터 크기가 크다면 실패 처리 */
	if (u32CopySize < size)	return FALSE;

	/* 데이터 복사 */
	if (!m_pReadBuff->CopyBinary(data, size))
	{
		PutErrMsg(L"Failed to copy the received data");
		return FALSE;
	}

	return TRUE;
}

/*
 desc : 시리얼 통신 포트로부터 수신된 버퍼의 내용을 모두 읽어서 버린다.
 parm : time_out	- [in]  일정 시간 동안 수신된 버퍼 데이터 모두 비우기 (단위: msec)
 retn : None
*/
VOID CSerialComm::ReadAllData(UINT32 time_out)
{
	UINT8 u8Read[2]	= {NULL};
	UINT32 u32Read	= 1;
	UINT64 u64Tick	= GetTickCount64();

	do 	{

		if (!PopReadData(u8Read, u32Read))	break;

		/* 무한 루프에 들어 갔는지 여부 확인 */
		if (GetTickCount64() > (u64Tick + time_out))	break;

	} while (u32Read);
}

/*
 desc : 검색 대상인 문자 1개가 버퍼 내에 있는지, 있다면 해당 위치를 반환 (zero-based)
 parm : find	- [in]  검색하고자 하는 문자
		pos		- [out] 검색(찾은)된 위치 (zero-based)
 retn : 0x00 - 검색 실패, 0x01 - 검색 성공
*/
BOOL CSerialComm::FindChar(UINT8 find, UINT32 &pos)
{
	return m_pReadBuff->FindChar(find, pos);
}

/*
 desc : 검색 대상인 문자 2 개가 버퍼 내에 있는지, 있다면 해당 위치를 반환 (zero-based)
 parm : find1	- [in]  첫 번째 검색하고자 하는 문자
		find2	- [in]  두 번째 검색하고자 하는 문자
		pos		- [out] 검색(찾은)된 위치 (zero-based) (find2의 위치 반환)
 retn : 0x00 - 검색 실패, 0x01 - 검색 성공
*/
BOOL CSerialComm::FindChar(UINT8 find1, UINT8 find2, UINT32 &pos)
{
	return m_pReadBuff->FindChar(find1, find2, pos);
}

/*
 desc : 맨 처음 (앞 부분)에 존재하는 1 byte 값 반환
 parm : None
 retn : 값 반환 (0x00 ~ 0xff)
*/
UINT8 CSerialComm::GetHeadData()
{
	UINT8 u8Data	= 0x00;
	m_pReadBuff->PeekChar(0, u8Data);

	return u8Data;
}

/*
 desc : 시리얼 통신 포트가 Open 되었는지 여부
 parm : None
 retn : TRUE or FALSE
*/
BOOL CSerialComm::IsOpenPort()
{
	if (!m_bIsOpened)
	{
		TCHAR tzMesg[128]	= {NULL};
		swprintf_s(tzMesg, 128, L"The serial port is not open [%s]", m_tzPort);
		LOG_WARN(ENG_EDIC::en_engine, tzMesg);
	}
	return m_bIsOpened;
}