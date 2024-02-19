
#include "pch.h"
#include "LogData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : None
 retn : None
*/
CLogData::CLogData()
{
	/* 동기화 객체 생성 */
	if (!m_mtxData.Open(MUTEX_CIMLOG_DATA))
		AfxMessageBox(L"Failed to create the [Log Mutex]", MB_ICONSTOP|MB_TOPMOST);

	/* HRESULT Error Message 버퍼 할당 */
	m_ptzErrMsg	= (PTCHAR)Alloc(sizeof(TCHAR) * CIM_LOG_MESG_SIZE);
	ASSERT(m_ptzErrMsg);
	wmemset(m_ptzErrMsg, 0x00, CIM_LOG_MESG_SIZE);
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CLogData::~CLogData()
{
	/* 기존 로그 모두 제거 */
	RemoveAll();
	/* 동기화 객체 해제 */
	m_mtxData.Close();
	/* 버퍼 메모리 반환 */
	Free(m_ptzErrMsg);
	m_ptzErrMsg	= NULL;
}

/*
 desc : 모든 로그 항목 제거
 parm : None
 retn : None
*/
VOID CLogData::RemoveAll()
{
	POSITION pPos	= NULL;
	STG_CLLM stLog	= {NULL};

	/* 동기화 진입 */
	if (m_mtxData.Lock())
	{
		pPos	= m_lstLogMesg.GetHeadPosition();
		while (pPos)
		{
			stLog	= (STG_CLLM)m_lstLogMesg.GetNext(pPos);
			stLog.Release();
		}
		m_lstLogMesg.RemoveAll();
		/* 동기화 해제 */
		m_mtxData.Unlock();
	}
}

/*
 desc : 로그 메시지 임시 저장
 parm : mesg1/2	- [in]  메시지 저장 버퍼 포인터
		level	- [in]  로그 에러 (알람) 레벨 (0x00 - Normal, 0x01 - Warning, 0x02 - Alarm (Error), 0x03 - Unknown
 retn : None
*/
VOID CLogData::PushLogs(PTCHAR mesg, LONG value, ENG_GALC level)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"%s::%d", mesg, value);
	PushLogs(tzMesg, level);
}
VOID CLogData::PushLogs(PTCHAR mesg1, PTCHAR mesg2, ENG_GALC level)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"%s::%s", mesg1, mesg2);
	PushLogs(tzMesg, level);
}

/*
 desc : 로그 메시지 임시 저장
 parm : mesg	- [in]  메시지 저장 버퍼 포인터
		level	- [in]  로그 에러 (알람) 레벨 (0x00 - Normal, 0x01 - Warning, 0x02 - Alarm (Error), 0x03 - Unknown
 retn : None
*/
VOID CLogData::PushLogs(PTCHAR mesg, ENG_GALC level)
{
	STG_CLLM stLog	= {NULL};
	SYSTEMTIME stTm	= {NULL};

	if (!mesg)	return;
	if (!m_mtxData.GetHandle())	return;
#if 1
	/* 동기화 진입 */
	if (m_mtxData.Lock())
	{
		/* 현재 등록된 개수가 초과 되었으면 가장 오래된 항목 제거 */
		if (m_lstLogMesg.GetCount() > MAX_CIMLOG_COUNT)	m_lstLogMesg.RemoveHead();

		/* 현재 컴퓨터 시간 */
		GetLocalTime(&stTm);
		stLog.t_year	= stTm.wYear;
		stLog.t_month	= stTm.wMonth;
		stLog.t_day		= stTm.wDay;
		stLog.t_hour	= stTm.wHour;
		stLog.t_minute	= stTm.wMinute;
		stLog.t_second	= stTm.wSecond;
		stLog.t_msecond	= stTm.wMilliseconds;
		stLog.level		= UINT8(level) > 2 ? 0x03 : UINT8(level);
		/* 메모리 생성 */
		stLog.Init();
		/* 로그 데이터 저장 */
		wcscpy_s(stLog.mesg, CIM_LOG_MESG_SIZE, mesg);
		/* 로그 출력 */
#ifdef _DEBUG
		if (level != ENG_GALC::en_normal)
		{
			TRACE(L"[%04I64u-%02I64u-%02I64u %02I64u:%02I64u:%02I64u][%03I64u] %s\n",
				  stLog.t_year, stLog.t_month, stLog.t_day,
				  stLog.t_hour, stLog.t_minute, stLog.t_second, stLog.t_msecond, mesg);
		}
#endif
		/* 메모리에 등록 */
		m_lstLogMesg.AddTail(stLog);
		/* 동기화 해제 */
		m_mtxData.Unlock();
	}
#endif
}

/*
 desc : 로그 메시지 반환
 parm : mesg	- [in]  메시지가 저장되어 반환될 구조체 참조
 retn : TRUE or FALSE
*/
BOOL CLogData::PopLogs(STG_CLLM &mesg)
{
	BOOL bIsPoped	= FALSE;

	if (!m_mtxData.GetHandle())	return FALSE;

	/* 동기화 진입 */
	if (m_mtxData.Lock())
	{
		/* 현재 등록된 로그가 있는지 여부 */
		if (m_lstLogMesg.GetCount())
		{
			/* 맨 처음꺼 가져오고, 등록된 항목 제거 */
			mesg	= m_lstLogMesg.GetHead();
			m_lstLogMesg.RemoveHead();
			/* 반환될 데이터가 있다고 플래그 설정  */
			bIsPoped= TRUE;
		}
		/* 동기화 해제 */
		m_mtxData.Unlock();
	}

	return bIsPoped;
}

/*
 desc : COM 호출 결과 분석
 parm : hr	- [in]  HRESULT 값
		mesg- [in]  Error Message 문자열 버퍼
		logs- [in]  로그 출력 여부
 retn : HRESULT
*/
HRESULT CLogData::CheckResult(HRESULT hr, PTCHAR mesg)
{
	if (FAILED(hr))
	{
		PTCHAR ptzMesg	= (PTCHAR)::Alloc(sizeof(TCHAR) * CIM_LOG_MESG_SIZE);
		ASSERT(ptzMesg);
		/* CIMConnect Error Message 얻기 */
		if (!GetResultError(hr))
		{
			GetErrorInfo(hr);
			swprintf_s(ptzMesg, CIM_LOG_MESG_SIZE, L"%s : %s", mesg, m_ptzErrMsg);
		}
		else
		{
			swprintf_s(ptzMesg, CIM_LOG_MESG_SIZE, L"COM ERROR [ %s ] HRESULT 0x%x [%s]",
					   mesg, hr, m_ptzErrMsg);	/* Error Message 추가 */
		}
		PushLogs(ptzMesg, ENG_GALC::en_alarm);
		/* 메모리 반환 */
		::Free(ptzMesg);
	}

	return hr;
}

/*
 desc : 에러 코드 반환
 parm : hr	- [in]  HRESULT 값
 retn : TRUE (에러 검색된 경우) or FALSE (에러 찾지 못한 경우)
*/
BOOL CLogData::GetResultError(HRESULT hr)
{
	BOOL bFind	= TRUE;
	wmemset(m_ptzErrMsg, 0x00, CIM_LOG_MESG_SIZE);

	switch (hr)
	{
	case 0x80010006	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"Other connections are still valid (RPC_E_CONNECTION_TERMINATED)");			break;	/*  */
	case 0x80010007	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"The call may have executed (RPC_E_SERVER_DIED)");							break;	/*  */
	case 0x80010008	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"The caller (client) disappeared while the callee (server) was processing a call (RPC_E_CLIENT_DIED)");	break;	/*  */
	case 0x80010012	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"The call did not execute. (RPC_E_SERVER_DIED_DNE)");							break;	/*  */
	case 0x80010108	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"The object invoked has disconnected from its clients (RPC_E_DISCONNECTED)");	break;	/*  */
	case 0x800706BA	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"RPC Server Unavailable");													break;	/*  */
	case 0x800706BE	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"The remote procedure call failed");											break;	/*  */
	case 0x800706BF	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"The remote procedure call failed and did not execute");						break;	/*  */

	case 0x84000000	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"an item with the specified id already exists");								break;	/* E_IDEXISTS */
	case 0x84000001	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"the value is out of range");													break;	/* E_INVALIDID */
	case 0x84000002	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"the variable does not allow limits");										break;	/* E_OUTOFRANGE */
	case 0x84000003	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"there is no space to complete the operation");								break;	/* E_NOLIMITALLOWED */
	case 0x84000004	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"request rejected");															break;	/* E_NOSPACE */
	case 0x84000005	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"request rejected");															break;	/* E_REJECTED */
	case 0x84000006	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"the limit id is invalid");													break;	/* E_INVALIDLIMITID */
	case 0x84000007	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"the upper dead-band value is invalid");										break;	/* E_UPPERDB */
	case 0x84000008	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"the lower dead-band value is invalid");										break;	/* E_LOWERDB */
	case 0x84000009	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"the upper dead-band is less than the lower");								break;	/* E_UPLESSLOW */
	case 0x8400000a	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"the ASCII value is not a number");											break;	/* E_ASCIINUM */
	case 0x8400000b	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"the service is unavailable");												break;	/* E_UNAVAIL */
	case 0x8400000c	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"the service is unavailable");												break;	/* */
	case 0x8400000d	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"cannot perform that now");													break;	/* E_CANNOTPERFORM */
	case 0x8400000e	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"a parameter is invalid");													break;	/* E_INVALIDPARAM */
	case 0x8400000f	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"the array type is invalid");													break;	/* E_INVALIDARRAYTYPE */
	case 0x84000010	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"there are no equipment available");											break;	/* E_NOEQUIPMENT */
	case 0x84000011	:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"the connection ID is invalid");												break;	/* E_INVALIDCONNECTION */
	default			:	wcscpy_s(m_ptzErrMsg, CIM_LOG_MESG_SIZE, L"Unknown Error Message");	bFind = FALSE;									break;
	}

	return bFind;
}

/*
 desc : COM 일반적인 에러 정보 얻기
 parm : hr	- [in]  HRESULT 값
 retn : None
*/
VOID CLogData::GetErrorInfo(HRESULT hr)
{
	TCHAR szMesg[CIM_LOG_MESG_SIZE] = {NULL};

	wmemset(m_ptzErrMsg, 0x00, CIM_LOG_MESG_SIZE);
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
				  hr, 0, szMesg, sizeof(szMesg) / sizeof(TCHAR), NULL);
	wsprintf(m_ptzErrMsg, L"Error code 0x%08X: %s", hr, szMesg);
}

