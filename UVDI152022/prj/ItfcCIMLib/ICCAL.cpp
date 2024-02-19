
#include "pch.h"
#include "MainApp.h"
#include "ICCAL.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : logs	- [in]  Log Object Pointer
		share	- [in]  Shared Data Object Pointer
 retn : None
*/
CICCAL::CICCAL(CLogData *logs, CSharedData *share)
	: CEBase(logs, share)
{
	/* GUID 설정 */
	m_gdICxE	= GUID_NULL;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CICCAL::~CICCAL()
{
	Close();
}

/*
 desc : 초기화 (반드시 호출)
 parm : equip_id	- [in]  Equipment ID (0 or later. 환경 파일에 설정된 개수-1)
		conn_id		- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
 retn : TRUE or FALSE
*/
BOOL CICCAL::Init(LONG equip_id, LONG conn_id)
{
	if (!CEBase::Init(equip_id, conn_id))	return FALSE;

	HRESULT hResult	= S_OK;

	if (m_pICCAlObjectPtr && m_pICCAlObjectPtr.GetInterfacePtr())	return FALSE;
	if (m_pICxAbsLayerPtr && m_pICxAbsLayerPtr.GetInterfacePtr())	return FALSE;

	/* Abstraction Layer */
	m_pLogData->PushLogs(L"ICCAlObject::CreateInstance");
	hResult = m_pICCAlObjectPtr.CreateInstance(CLSID_CCAlObject);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICCAlObject::CreateInstance CLSID_CCAlObject")))	return FALSE;

	m_pLogData->PushLogs(L"ICCAlObject::Init 1");
	hResult = m_pICCAlObjectPtr->Init(m_pICxClientApp, 1);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICCAlObject::Init 1")))	return FALSE;
	m_pICxAbsLayerPtr = m_pICCAlObjectPtr;

	return TRUE;
}

/*
 desc : 해제
 parm : None
 retn : None
*/
VOID CICCAL::Close()
{
	HRESULT hResult	= S_OK;

	/* Abstraction Layer */
	if (m_pICxAbsLayerPtr && m_pICxAbsLayerPtr.GetInterfacePtr())
	{
		m_pLogData->PushLogs(L"ICxAbsLayer::Shutdown");
		m_pICxAbsLayerPtr->Shutdown();
		m_pLogData->CheckResult(hResult, L"ICxAbsLayer::Shutdown");
		m_pLogData->PushLogs(L"ICxAbsLayer::Release");
		m_pICxAbsLayerPtr.Release();
	}
	/* ICCAlObject Layer */
	if (m_pICCAlObjectPtr && m_pICCAlObjectPtr.GetInterfacePtr())
	{
		m_pLogData->PushLogs(L"ICCAlObject::Release");
		m_pICCAlObjectPtr.Release();
	}

	CEBase::Close();
}

/*
 desc : CIMConnect 내부에 로그 저장 여부
 parm : value	- [out] TRUE : Saved, FALSE : No save
 retn : TRUE or FALSE
*/
BOOL CICCAL::GetLogToCIMConnect(BOOL &value)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	VARIANT_BOOL vbValue;

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetLogToCIMConnect::get_LogToCIMConnect");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICCAlObjectPtr->get_LogToCIMConnect(&vbValue);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	value	= vbValue == VARIANT_TRUE;

	return (hResult == S_OK);
}

/*
 desc : CIMConnect 내부에 로그 저장 여부 설정
 parm : value	- [in]  TRUE : Saved, FALSE : No save
 retn : TRUE or FALSE
*/
BOOL CICCAL::SetLogToCIMConnect(BOOL value)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	VARIANT_BOOL vbValue = (value == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetLogToCIMConnect::put_LogToCIMConnect : value=%d", value);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICCAlObjectPtr->put_LogToCIMConnect(vbValue);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}
