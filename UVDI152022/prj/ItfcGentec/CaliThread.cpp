
/*
 desc : Gentec Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "CaliThread.h"

#include <numeric>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/* --------------------------------------------------------------------------------------------- */
/*                                    Array Queue <eXtended>                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Constructor
 parm : count	- [in]  버퍼 개수 (Max : 32)
 retn : None
*/
CArrQueEx::CArrQueEx()
{
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CArrQueEx::~CArrQueEx()
{
	m_vecValue.clear();
}

/*
 desc : Put the current value and then calculates the average and standard deviation
 parm : value	- [in]  Current value
 retn : None
*/
VOID CArrQueEx::PushQue(DOUBLE value)
{
	INT8 i8Sign	= 0;
	DOUBLE dbSum= 0.0f, dbMax = DBL_MIN, dbMin = DBL_MAX;
	vector<DOUBLE>::iterator itVec;

#if 1
	if (value == 0.0f)
	{
		TRACE(L"PushQue(value) : Power is Zero\n");
	}
#endif

	/* Put the current value (equal to insert()) */
	m_vecValue.emplace_back(value);

	/* Check if the queue is full */
	if (m_vecValue.size() > ILLUM_MEAS_COUNT)
	{
		m_vecValue.erase(m_vecValue.begin());
	}
	if (m_vecValue.size() > 1)
	{
#if 0	/* The sort function should not be used here */
		/* Sort in ascending order */
		sort(m_vecValue.begin(), m_vecValue.end());
#endif
		/* Calculate the sum */
		dbSum	= accumulate(m_vecValue.begin(), m_vecValue.end(), double(0.0f));
		/* Calculate the average */
		m_dbAvg	= dbSum/m_vecValue.size();
		/* If there is only one array... */
		if (m_vecValue.size() < 2)	m_dbStd	= sqrt(-1.0f);
		else
		{
			/* Calculate the standard deviation */
			itVec	= m_vecValue.begin();
			for (dbSum=0.0f; itVec != m_vecValue.end(); itVec++)
			{
				/* Set the min and max values */
				if (dbMax < *itVec)	dbMax = *itVec;
				if (dbMin > *itVec)	dbMin = *itVec;
				/* Calculate the sum (current - average) */
				dbSum	+= pow((*itVec - m_dbAvg), 2);
			}
			/* Calculates the standard deviation */
			m_dbStd	= sqrt(dbSum / DOUBLE(m_vecValue.size()));
		}
		/* Calculates the max - min (offset) value */
		m_dbDiff= dbMax - dbMin;
	}
	else
	{
		m_dbAvg	= value;
		m_dbStd	= value;
		m_dbDiff= value;
	}
}

/* --------------------------------------------------------------------------------------------- */
/*                                       Calibration Thread                                      */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : shmem	- [int]  Shared Memory
		type	- [in]  Measurement Type (0x01: Power, 0x02: Energy, 0x03: Frequency, 0x04: Density)
 retn : None
 참고 :	dev_set	- [out] Gentec Device Setup Information 정보가 저장될 구조체 포인터
		dev_get	- [out] Gentec Device Realtime Value 정보가 저장될 구조체 포인터
*/
CCaliThread::CCaliThread(UINT32 source_ip, UINT32 target_ip, UINT16 u16Port, LPG_PADV shmem, ENG_MDST type)
{
#if (DEF_COMMUNICATION_TYPE == DEF_SOCKET)
	char chLocalIP[32] = { 0, };
	char chRemoteIP[32] = { 0, };

	IN_ADDR stLocalAddr = { NULL };
	IN_ADDR stRemoteAddr = { NULL };

	memcpy(&stLocalAddr.s_addr, &source_ip, sizeof(UINT32));
	memcpy(&stRemoteAddr.s_addr, &target_ip, sizeof(UINT32));

	inet_ntop(AF_INET, (CONST VOID*) & stLocalAddr, chLocalIP, IPv4_LENGTH);
	inet_ntop(AF_INET, (CONST VOID*) & stRemoteAddr, chRemoteIP, IPv4_LENGTH);

	SetConfigLocalIPAddr(chLocalIP);
	SetConfigRemoteIPAddr(chRemoteIP);
	SetConfigPortNumber(u16Port);

	OpenDev();
	m_bIsOpened = TRUE;
#elif (DEF_COMMUNICATION_TYPE == DEF_SERIAL)
	ST_SERIAL_PARAM stParam;

	/* Manual 50 Page 참조 */
	stParam.SetPort((BYTE)u16Port);
	stParam.SetBaudRate(CBR_115200);
	stParam.SetDataBits(8);
	stParam.SetStopBits(ONESTOPBIT);
	stParam.SetParityBits(NOPARITY);

	SetSerialParam(stParam);
	SetUseThread(FALSE);

	m_bIsOpened = FALSE;
#endif

	m_bListen		= FALSE;
	m_bRunMeasure	= FALSE;

	/* Device 전체 정보 연결 */
	m_pstShMemGentec= shmem;
	m_enMeasType	= type;

	/* 임의 시간마다 연결을 하기 위해서 */
	m_u64ConnectTime= 0;

	/* 임의 개수만큼 평균을 내기 위한 큐 버퍼 생성 (초당 10 개 정도 적당함) */
	/* Gentec로부 명령 요청에 대한 응답시간이 아무리 빨라야 30 ~ 80 msec 임 */
	m_pArrQue		= new CArrQueEx();
	ASSERT(m_pArrQue);
}

/*
 desc : Destructor
 parm : None
*/
CCaliThread::~CCaliThread()
{
	/* 현재 연결된 Gentec 디바이스 닫기 */
	if (m_bIsOpened)	CloseDev();
	/* 큐 버퍼 메모리 해제 */
	if (m_pArrQue)	delete m_pArrQue;
}

#if (DEF_COMMUNICATION_TYPE == DEF_SOCKET)

long CCaliThread::RecvProcess(long lSize, BYTE* pbyData)
{
	if (0 >= lSize || FALSE == m_bListen)
		return -1;

	std::string strBuf = std::string(pbyData, pbyData + lSize);

	/* 시작 문자가 따로 없다... */
	m_strBuffer += strBuf;

	return 0;
}

BOOL CCaliThread::WaitReply(char* pszRecv, DWORD dwTimeout)
{
	DWORD dwTime = GetTickCount();

	while ((GetTickCount() - dwTime) < dwTimeout)
	{
		if (m_strBuffer.find(DEF_CMD_END) != std::string::npos)
		{
			if (0 != m_strBuffer.size())
			{
				if (NULL != pszRecv)
				{
					memcpy(pszRecv, m_strBuffer.data(), m_strBuffer.length());
				}

				m_strBuffer.clear();
				return TRUE;
			}
		}

		Sleep(10);
	}

	// Time out
	m_strBuffer.clear();
	return FALSE;
}

BOOL CCaliThread::ReadData(CString strSend, char* pchRead, DWORD dwTimeout)
{
	if (FALSE == ChkConnect())
		return FALSE;

	long lSize = strSend.GetLength();
	char* pchMsg = new char[lSize];

	memcpy(pchMsg, CStringA(strSend), lSize);

	m_bListen = TRUE;
	Send(lSize, (BYTE*)pchMsg);

	long lRet = WaitReply(pchRead, dwTimeout);

	m_bListen = FALSE;
	delete[] pchMsg;

	return lRet;
}

#elif (DEF_COMMUNICATION_TYPE == DEF_SERIAL)

BOOL CCaliThread::WaitResponse(char* pchRead, DWORD dwTimeout)
{
	BYTE byTemp[__SIZE_OF_COMM_BUFFER_] = { 0x00, };
	BYTE byETX[2] = { (BYTE)ASCII_CR, 0x00 };

	int nReadCnt = ReadComm(byTemp, byETX, __SIZE_OF_COMM_BUFFER_, dwTimeout);
	if (nReadCnt <= 0)
	{
		return FALSE;
	}

	memcpy(pchRead, byTemp, nReadCnt);

	return TRUE;
}

#endif

/*
 desc : 스레드 실행시 초기 한번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCaliThread::StartWork()
{

	return TRUE;
}

/*
 desc : 스레드 실행시 주기적 호출됨
 parm : None
 retn : None
*/
VOID CCaliThread::RunWork()
{
#if (DEF_COMMUNICATION_TYPE == DEF_SOCKET)
	/* 동기 진입 */
	if (m_syncData.Enter())
	{
		/* 주기적으로 값 요청 및 저장 */
		if (m_bRunMeasure)
		{
			QueryTotal();
		}

		/* 동기 해제*/
		m_syncData.Leave();
	}
#elif (DEF_COMMUNICATION_TYPE == DEF_SERIAL)
	/* 현재 Gentec와 연결되어 있는지 확인 */
	if (!m_bIsOpened)
	{
		BOOL bSucc	= FALSE;

		/* 기존 연결 수행 후 일정 시간이 경과 했는지 여부 */
		if (m_u64ConnectTime + 3000 > GetTickCount64())	return;
		/* 연결 시도한 시간 저장 */
		m_u64ConnectTime	= GetTickCount64();

		/* 일단 기존 연결된 디바이스 닫기 */
		CloseDev();
		/* 새로 연결 수행 */
		bSucc = OpenDev();
		if (bSucc)
		{
			/* 현재 장비에 설정된 정보 요청 */
			//bSucc	= GetDevSetData();
		}
		/* 연결 여부 최종 설정 */
		m_bIsOpened	= bSucc;
		return;
	}
	/* 동기 진입 */
	if (m_syncData.Enter())
	{
		/* 주기적으로 값 요청 및 저장 */
		if (m_bRunMeasure)
		{
			m_bIsOpened	= QueryTotal();
		}

		/* 동기 해제*/
		m_syncData.Leave();
	}
#endif
}

/*
 desc : 스레드 종료시 마지막 호출됨
 parm : None
 retn : None
*/
VOID CCaliThread::EndWork()
{
}

/*
 desc : 현재 장비에 설정된 정보 요청
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCaliThread::GetDevSetData()
{
	BOOL bSucc	= TRUE;

	/* 현재 연결된 디바이스 정보 얻기 */
	if (bSucc)	bSucc	= QueryDevID();
	
	return bSucc;
}

/*
 desc : Gentec Device에 연결 시도
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCaliThread::OpenDev()
{
#if (DEF_COMMUNICATION_TYPE == DEF_SOCKET)
	char pczLocalIP[32] = { 0, };
	char pczRemoteIP[32] = { 0, };
	unsigned short uczPort;

	GetConfigLocalIPAddr(pczLocalIP);
	GetConfigRemoteIPAddr(pczRemoteIP);
	uczPort = GetConfigPortNumber();

	return NetOpen(pczLocalIP, pczRemoteIP, uczPort);
#elif (DEF_COMMUNICATION_TYPE == DEF_SERIAL)
	return PortOpen();
#endif
}

/*
 desc : Gentec Device에 닫기
 parm : None
 retn : None
*/
VOID CCaliThread::CloseDev()
{
	if (TRUE == IsOpen())
	{
#if (DEF_COMMUNICATION_TYPE == DEF_SOCKET)
		NetClose();
#elif (DEF_COMMUNICATION_TYPE == DEF_SERIAL)
		PortClose();
#endif
	}
}



/*
 desc : 에러 정보 저정
 parm : err_cd	- [in]  Gentec에서 발생된 에러 코드 값
 retn : TRUE or FALSE
*/
BOOL CCaliThread::ErrosMessage(char *pszMesg)
{
	CString		strErrorMsg;
	CUniToChar	csCnv;

	/* Event Message */
	strErrorMsg.Format(_T("%s"), csCnv.Ansi2Uni(pszMesg));

	if (0 > strErrorMsg.Find(_T("Error")))
	{
		return TRUE;
	}

	LOG_ERROR(ENG_EDIC::en_gentec, strErrorMsg.GetBuffer());
	strErrorMsg.ReleaseBuffer();
	/* 에러가 발생 했다고 알림 */
	return FALSE;
}

/*
 desc : 현재 연결된 디바이스 ID 정보 얻기
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCaliThread::QueryDevID()
{
	char chReply[__SIZE_OF_COMM_BUFFER_ + 1] = { 0, };
	LPG_PDDI pstDevID		= &m_pstShMemGentec->set.dev_id;
	CUniToChar csCnv;

#if (DEF_COMMUNICATION_TYPE == DEF_SOCKET)
	if (FALSE == ReadData(_T("*HEA"), chReply))	return FALSE;
	wcscpy_s(pstDevID->m_name,	63, csCnv.Ansi2Uni(chReply));
	if (FALSE == ReadData(_T("*VER"), chReply))	return FALSE;
	wcscpy_s(pstDevID->s_firm,	63, csCnv.Ansi2Uni(chReply));
#elif (DEF_COMMUNICATION_TYPE == DEF_SERIAL)
	char chCmd[__SIZE_OF_COMM_BUFFER_ + 1] = { 0, };

	/* Get model name of the detector head. */
	sprintf_s(chCmd, __SIZE_OF_COMM_BUFFER_ + 1, "*HEA");
	memset(chReply, 0, sizeof(chReply));
	if (FALSE == WriteComm((PBYTE)chCmd, (long)strlen(chCmd)))	return FALSE;
	if (FALSE == WaitResponse(chReply, 1000))					return FALSE;
	if (FALSE == ErrosMessage(chReply))							return FALSE;
	wcscpy_s(pstDevID->m_name,	63, csCnv.Ansi2Uni(chReply));
	
	sprintf_s(chCmd, __SIZE_OF_COMM_BUFFER_ + 1, "*VER");
	memset(chReply, 0, sizeof(chReply));
	if (FALSE == WriteComm((PBYTE)chCmd, (long)strlen(chCmd)))	return FALSE;
	if (FALSE == WaitResponse(chReply, 1000))					return FALSE;
	if (FALSE == ErrosMessage(chReply))							return FALSE;
	wcscpy_s(pstDevID->s_firm,	63, csCnv.Ansi2Uni(chReply));
#endif

	return TRUE;
}


/*
 desc : 전체 장비의 실시간 데이터 요청
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCaliThread::QueryTotal()
{
	double		dValue= 0.0f;
	char		chReply[__SIZE_OF_COMM_BUFFER_ + 1] = { 0, };
	CUniToChar	csCnv;

	memset(chReply, 0, sizeof(chReply));

#if (DEF_COMMUNICATION_TYPE == DEF_SOCKET)
	if (FALSE == ReadData(_T("*CVU"), chReply))	return FALSE;
#elif (DEF_COMMUNICATION_TYPE == DEF_SERIAL)
	char chCmd[__SIZE_OF_COMM_BUFFER_ + 1] = { 0, };

	sprintf_s(chCmd, __SIZE_OF_COMM_BUFFER_ + 1, "*CVU");
	if (FALSE == WriteComm((PBYTE)chCmd, (long)strlen(chCmd)))	return FALSE;
	if (FALSE == WaitResponse(chReply, 1000))					return FALSE;
	if (FALSE == ErrosMessage(chReply))							return FALSE;
#endif

	/* If there is no measured value at all ... */
	if (!ErrosMessage(chReply))	return FALSE;

	dValue = _ttof(csCnv.Ansi2Uni(chReply));
	/* Push the current power */
	m_pArrQue->PushQue(dValue);
	/* Set the currently calcualted value in shared memory */
	m_pstShMemGentec->get.max_min	= m_pArrQue->GetDiff();
	m_pstShMemGentec->get.average	= m_pArrQue->GetAvg();
	m_pstShMemGentec->get.std_dev	= m_pArrQue->GetStd();
	m_pstShMemGentec->get.last_val	= dValue;

	return TRUE;
}

/*
 desc : 임의 시간 동안 측정한 값 초기화
 parm : None
 retn : None
*/
VOID CCaliThread::ResetValue()
{
	/* 동기 진입 */
	if (m_syncData.Enter())
	{
		TRACE(L"ResetValue\n");
		/* Empty the queue buffer */
		m_pArrQue->QueEmpty();
		/* 현재 구조체 값 초기화 */
		memset(&m_pstShMemGentec->get, 0x00, sizeof(STG_PADV));
		/* 동기 해제 */
		m_syncData.Leave();
	}
}

/*
 desc : 조도 데이터 측정 여부 결정
 parm : flag	- [in]  TRUE - 시작, FALSE - 중지
 retn : TRUE or FALSE
*/
BOOL CCaliThread::RunMeasure(BOOL flag)
{
	BOOL bFlag	= FALSE;
	/* 동기 진입 */
	if (m_syncData.Enter())
	{
		/* 기존 측정한 값들 초기화 */
		if (IsConnected())
		{
			bFlag	= TRUE;
			m_bRunMeasure = flag;
		}
		/* Gentec 로부터 실시간 값 요청 후 응답 받은데 걸리는 시간 : 35 ~ 80 msec */
		/* 1 초에 10 개 정도 수집하려면, 수집 요청 주기를 최소한 100 msec로 해야함 */
		SetCycleTime(100);
		/* 동기 해제 */
		m_syncData.Leave();
	}

	return bFlag;
}

/*
 desc : Set Wave Length
 parm : wave	- [in]  Wave Length (or Line Filter) (단위: nm)
						ex> 365, 385, 390, 405 (nm)
 retn : TRUE or FALSE
*/
BOOL CCaliThread::QueryWaveLength(INT16 wave)
{
	char		chReply[__SIZE_OF_COMM_BUFFER_ + 1] = { 0, };

	memset(chReply, 0, sizeof(chReply));

#if (DEF_COMMUNICATION_TYPE == DEF_SOCKET)
	CString strCmd;

	strCmd.Format(_T("*PWC%05d"), wave);
	if (FALSE == ReadData(strCmd, chReply))	return FALSE;
#elif (DEF_COMMUNICATION_TYPE == DEF_SERIAL)
	char		chCmd[__SIZE_OF_COMM_BUFFER_ + 1] = { 0, };

	sprintf_s(chCmd, __SIZE_OF_COMM_BUFFER_ + 1, "*PWC%05d", wave);
	if (FALSE == WriteComm((PBYTE)chCmd, (long)strlen(chCmd)))	return FALSE;
	if (FALSE == WaitResponse(chReply, 1000))					return FALSE;
	if (FALSE == ErrosMessage(chReply))							return FALSE;
#endif

	return TRUE;
}

/*
 desc : Get Wave Length
 parm : None
 retn : 양수 값 (음수 값인 경우 실패) (365, 385, 390, 405 (nm))
*/
INT16 CCaliThread::QueryWaveLength()
{
	INT16		wave = 0;
	char		chReply[__SIZE_OF_COMM_BUFFER_ + 1] = { 0, };
	CString		strReply;
	CUniToChar	csCnv;


	memset(chReply, 0, sizeof(chReply));

#if (DEF_COMMUNICATION_TYPE == DEF_SOCKET)
	if (FALSE == ReadData(_T("*GWL"), chReply))	return FALSE;
#elif (DEF_COMMUNICATION_TYPE == DEF_SERIAL)
	char chCmd[__SIZE_OF_COMM_BUFFER_ + 1] = { 0, };

	sprintf_s(chCmd, __SIZE_OF_COMM_BUFFER_ + 1, "*GWL");
	if (FALSE == WriteComm((PBYTE)chCmd, (long)strlen(chCmd)))	return FALSE;
	if (FALSE == WaitResponse(chReply, 1000))					return FALSE;
	if (FALSE == ErrosMessage(chReply))							return FALSE;
#endif

	strReply = csCnv.Ansi2Uni(chReply);
	strReply.Replace(_T("PWC") ,_T(""));
	strReply.Replace(_T(":"), _T(""));

	/* 진동수를 파장 값으로 변환 */
	return (INT16)_ttoi(strReply);
}

