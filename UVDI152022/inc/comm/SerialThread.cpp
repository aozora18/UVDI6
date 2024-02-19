
/*
 desc : 시리얼 통신 스레드
*/

#include "pch.h"
#include "SerialThread.h"


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
CSerialThread::CSerialThread(UINT8 port, UINT32 baud, UINT32 buff, ENG_SHOT shake)
{
	m_u8Port	= port;
	m_bIsReady	= FALSE;

	/* 전역 객체 생성 */
	m_pComm		= new CSerialComm(port, baud, buff, shake);
	ASSERT(m_pComm);
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CSerialThread::~CSerialThread()
{
	if (m_pComm)
	{
		/* 시리얼 통신 포트 해제 */
		m_pComm->Close();
		/* 시리얼 통신 객체 해제 */
		delete m_pComm;
		m_pComm	= NULL;
	}
}

/*
 desc : 스레드 시작할 때, 최초 한번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CSerialThread::StartWork()
{
	/* 통신 포트로부터 데이터를 송/수신할 준비가 되었다고 설정 */
	m_bIsReady	= TRUE;

	return TRUE;
}

/*
 desc : 스레드 동작 중, 지속적으로 호출됨
 parm : None
 retn : None
*/
VOID CSerialThread::RunWork()
{
	if (!m_bIsReady)	return;

	/* 현재 통신 포트가 열려져 있는지 여부를 확인 */
	/* 열려져 있지 않으면, 해제 후 다시 연결 시도 */
	if (!m_pComm->IsOpenPort())
	{
		/* 통신 포트 다시 열기 위해서, 일정 시간 대기 */
		Sleep(500);
		if (!m_pComm->Open())	m_pComm->Close();
		/* 통신 포트 다시 열기 때문에, 일정 시간 대기 */
		Sleep(500);
		return;
	}

	/* 주기적으로 데이터 수신이 있는지 읽기 시도 */
	if (m_syncComm.Enter())
	{
		m_pComm->ReadData();
		m_syncComm.Leave();
	}
}

/*
 desc : 스레드 중지(종료)될 때, 마지막으로 한 번 호출됨
 parm : None
 retn : None
*/
VOID CSerialThread::EndWork()
{
}

/*
 desc : 스레드 동작을 중지하고자 할 때,
		통신 이벤트 대기 상태를 탈출하도록 한 후, 중지 작업을 진행해야 한다.
 parm : None
 retn : None
*/
VOID CSerialThread::Stop()
{
	/* 데이터 송/수신 작업 중지하도록 플래그 강제로 설정       */
	m_bIsReady	= FALSE;
#if 0
	/* 반드시 통신 이벤트 감시 기능을 해제한 후, 중지 작업 수행 */
	if (m_pComm)	m_pComm->WaitCommEventBreak();
#endif
	// 스레드 동작을 중지 시킴
	CThinThread::Stop();
}

/*
 desc : 현재 시리얼 통신 포트가 열려져 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
BOOL CSerialThread::IsOpenPort()
{
	if (!m_pComm)	return FALSE;
	return m_pComm->IsOpenPort();
}

/*
 desc : 가장 최근에 발생된 에러 메시지 저장
 parm : msg	- [in]  에러 정보가 저장된 문자열 포인터
 retn : None
*/
VOID CSerialThread::PutErrMsg(TCHAR *msg)
{
	if (m_pComm)	m_pComm->PutErrMsg(msg);
}

/*
 desc : 가장 최근에 발생된 에러 메시지 반환
 parm : None
 retn : 에러 메시지가 저장된 메모리의 위치(포인터) 반환
*/
PTCHAR CSerialThread::GetErrMsg()
{
	if (!m_pComm)	return NULL;
	return m_pComm->GetErrMsg();
}

/*
 desc : 현재 통신 포트의 환경 설정 값 반환
 parm : data	- [out] 반환되어 저장될 구조체 참조
 retn : TRUE or FALSE
*/
BOOL CSerialThread::GetConfigPort(STG_SSCI &data)
{
	if (!m_pComm)	return FALSE;
	return m_pComm->GetConfigPort(data);
}

/*
 desc : 현재 통신 포트의 환결 설정 값 변경
 parm : data	- [in]  변경하고자 하는 값이 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
BOOL CSerialThread::SetConfigPort(LPG_SSCI data)
{
	if (!m_pComm)	return FALSE;
	return m_pComm->SetConfigPort(data);
}

/*
 desc : 현재 열려져 있는 통신 포트로 데이터 전송 작업 수행
 parm : data	- [in]  전송하려는 데이터가 저장된 버퍼 포인터
		size	- [out] 'data' 버퍼에 저장된 데이터의 크기 및 실제 전송된 크기 값 반환
 retn : TRUE (전송 성공) or FALSE (전송 실패)
*/
BOOL CSerialThread::WriteData(PUINT8 data, UINT32 &size)
{
	UINT32 u32Write		= 0;

	if (!m_pComm)	return FALSE;

	/* 현재 스레드가 동작 중인지 / 아닌지 확인 */
	if (!IsBusy())
	{
		m_pComm->PutErrMsg(L"The comm thread is stopped");
		return FALSE;
	}

	/* 동기 진입 */
	if (m_syncComm.Enter())
	{
		/* 현재 전송 중에 있다고 모드 설정 */
		m_bIsSend	= FALSE;
		/* 통신 포트로 데이터 전송 (size 크기만큼) */
		u32Write = m_pComm->WriteData(data, size);
		/* 전송 실패인 경우, 현재 전송 중에 있지 않다고 플래그 재설정 */
		if (u32Write < 1)
		{
			m_bIsSend = TRUE;	/* 즉, 전송 성공 했다고 처리 */
			m_pComm->Close();	/* 시리얼 통신 포트 닫기 */
		}
		/* 동기 해제 */
		m_syncComm.Leave();
	}
	return u32Write > 0 ? TRUE : FALSE;
}

/*
 desc : 주기적으로 수신된 데이터가 있는지 값 요청
 parm : port	- [in]  검색 대상의 통신 포트 번호
		data	- [in]  수신된 데이터가 저장될 버퍼 포인터
		size	- [out] 수신된 데이터가 저장될 'data' 버퍼의 크기
						실제로 수신된 데이터의 크기
 retn : TRUE or FALSE
*/
BOOL CSerialThread::PopReadData(PUINT8 data, UINT32 &size)
{
	if (!m_pComm)	return FALSE;

	/* 현재 스레드가 동작 중인지 / 아닌지 확인 */
	if (!IsBusy())
	{
		m_pComm->PutErrMsg(L"The comm thread is stopped");
		return FALSE;
	}

	/* 동기 진입 */
	if (m_syncComm.Enter())
	{
		/* 수신된 데이터 가져오고, 가져온 크기 저장 */
		size = m_pComm->PopReadData(data, size);
		/* 동기 해제 */
		m_syncComm.Leave();
	}

	return TRUE;
}

/*
 desc : 현재 수신된 버퍼에 저장된 데이터의 크기 반환
 parm : port	- [in]  검색 대상의 통신 포트 번호
 retn : 수신된 버퍼에 저장된 데이터의 크기(단위: Bytes) 값 반환
*/
UINT32 CSerialThread::GetReadSize()
{
	UINT32 u32Size	= 0;
	if (!m_pComm)	return u32Size;

	/* 동기 진입 */
	if (m_syncComm.Enter())
	{
		u32Size = m_pComm->GetReadSize();
		/* 동기 해제 */
		m_syncComm.Leave();
	}

	return u32Size;
}

/*
 desc : 수신된 데이터가 있는지 값 요청 (수신 데이터를 복사만 해서 가져감)
 parm : port	- [in]  검색 대상의 통신 포트 번호
		data	- [in]  수신된 데이터가 저장될 버퍼 포인터
		size	- [out] 수신된 데이터가 저장될 'data' 버퍼의 크기
						실제로 수신된 데이터의 크기
 retn : TRUE or FALSE
*/
BOOL CSerialThread::CopyReadData(PUINT8 data, UINT32 &size)
{
	if (!m_pComm)	return FALSE;
	/* 현재 스레드가 동작 중인지 / 아닌지 확인 */
	if (!IsBusy())
	{
		m_pComm->PutErrMsg(L"The comm thread is stopped");
		return FALSE;
	}

	/* 동기 진입 */
	if (m_syncComm.Enter())
	{
		/* 수신된 데이터 가져오고, 가져온 크기 저장 */
		size	= m_pComm->CopyReadData(data, size);
		m_syncComm.Leave();
	}

	return TRUE;
}

/*
 desc : 현재 통신 상태 값 반환
 parm : None
 retn : ENG_SCSC
*/
ENG_SCSC CSerialThread::GetCommStatus()
{
	return m_pComm->GetCommStatus();
}

/*
 desc : 현재 통신 연결 해제 처리
 parm : None
 retn : None
*/
VOID CSerialThread::CommClosed()
{
	if (m_pComm)	m_pComm->Close();
}