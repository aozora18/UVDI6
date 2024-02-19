
#include "pch.h"
#include "MainApp.h"
#include "E39OBJ.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : logs	- [in]  Log Object Pointer
		share	- [in]  Shared Data Object Pointer
		absl	- [in]  ICxAbsLayer Pointer
 retn : None
*/
CE39OBJ::CE39OBJ(CLogData *logs, CSharedData *share)
	: CEBase(logs, share)
{
	/* GUID 설정 */
	m_gdICxE	= IID_ICxE39;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CE39OBJ::~CE39OBJ()
{
	Close();
}

/*
 desc : 초기화 (반드시 호출)
 parm : equip_id	- [in]  Equipment ID (0 or later. 환경 파일에 설정된 개수-1)
		conn_id		- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
 retn : TRUE or FALSE
*/
BOOL CE39OBJ::Init(LONG equip_id, LONG conn_id)
{
	if (!CEBase::Init(equip_id, conn_id))	return FALSE;

	HRESULT hResult		= S_FALSE;
	CComBSTR bstrTemp	= L"top:top";

	ICxE39CBPtr pICxE39CB					= m_pICxEMAppCbk;
	ICxE39InfoCBPtr pICxE39InfoCB			= m_pICxEMAppCbk;
	ICxE39InfoCB2Ptr pICxE39InfoCB2			= m_pICxEMAppCbk;
#if 0
	ICxE39CreateObjCBPtr pICxE39CreateObjCB	= m_pICxEMAppCbk;
	ICxE39DeleteObjCBPtr pICxE39DeleteObjCB	= m_pICxEMAppCbk;
	ICxE39ObjActionCBPtr PICxE39ObjActionCB	= m_pICxEMAppCbk;
#endif
	if (m_pICxE39Ptr && m_pICxE39Ptr.GetInterfacePtr())				return FALSE;

	/* Create & Initialize */
	m_pLogData->PushLogs(L"ICxE39::CreateInstance");
	hResult = m_pICxE39Ptr.CreateInstance(CLSID_CxE39);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE39::CreateInstance CLSID_CxE39")))	return FALSE;
	m_pLogData->PushLogs(L"ICxE39::Init");
	hResult = m_pICxE39Ptr->Init(bstrTemp, m_pICxAbsLayerPtr);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE39::Init")))	return FALSE;
	m_pICxE39ObjPtr = m_pICxE39Ptr;

	/* Register callback function */
	hResult = m_pICxE39ObjPtr->RegisterE39Callback(pICxE39CB);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE39ObjPtr::RegisterE39Callback")))		return FALSE;
	hResult = m_pICxE39ObjPtr->RegisterE39InfoCallback(pICxE39InfoCB);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE39ObjPtr::RegisterE39InfoCallback")))	return FALSE;
	hResult = m_pICxE39ObjPtr->RegisterE39InfoCallback2(pICxE39InfoCB2);
#if 0
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE39ObjPtr::RegisterE39InfoCallback2")))	return FALSE;
	hResult = m_pICxE39ObjPtr->RegisterE39CreateObjCB(pICxE39CreateObjCB);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE39ObjPtr::RegisterE39CreateObjCB")))	return FALSE;
	hResult = m_pICxE39ObjPtr->RegisterE39DeleteObjCB(pICxE39DeleteObjCB);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE39ObjPtr::RegisterE39DeleteObjCB")))	return FALSE;
	hResult = m_pICxE39ObjPtr->RegisterObjectAction(PICxE39ObjActionCB);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE39ObjPtr::RegisterObjectAction")))		return FALSE;
#endif
	/* Register the global interface */
	m_pLogData->PushLogs(L"RegisterInterfaceInGlobal", m_tzGUID);
	hResult	= m_pGIT->RegisterInterfaceInGlobal(m_pICxE39ObjPtr, m_gdICxE, &m_dwCookie);
	if (FAILED(m_pLogData->CheckResult(hResult, L"RegisterInterfaceInGlobal")))	return FALSE;
	m_pSharedData->GetGITCookie()->e39oss_cookie	= m_dwCookie;

	return (hResult == S_OK);
}

/*
 desc : 해제
 parm : None
 retn : None
*/
VOID CE39OBJ::Close()
{
	HRESULT hResult	= S_FALSE;

	ICxE39CBPtr pICxE39CB					= m_pICxEMAppCbk;
	ICxE39InfoCBPtr pICxE39InfoCB			= m_pICxEMAppCbk;
	ICxE39InfoCB2Ptr pICxE39InfoCB2			= m_pICxEMAppCbk;
#if 0
	ICxE39CreateObjCBPtr pICxE39CreateObjCB	= m_pICxEMAppCbk;
	ICxE39DeleteObjCBPtr pICxE39DeleteObjCB	= m_pICxEMAppCbk;
#endif
	if (m_pICxE39Ptr && m_pICxE39Ptr.GetInterfacePtr() &&
		m_pICxE39ObjPtr && m_pICxE39ObjPtr.GetInterfacePtr())
	{
#if 0
		m_pLogData->PushLogs(L"ICxE39ObjPtr::ICxE39DeleteObjCBPtr");
		hResult = m_pICxE39ObjPtr->UnregisterE39DeleteObjCB(pICxE39DeleteObjCB);
		m_pLogData->CheckResult(hResult, L"ICxE39ObjPtr::ICxE39DeleteObjCBPtr");
		m_pLogData->PushLogs(L"ICxE39DeleteObjCBPtr::Release");
		pICxE39DeleteObjCB.Release();

		m_pLogData->PushLogs(L"ICxE39ObjPtr::ICxE39CreateObjCBPtr");
		hResult = m_pICxE39ObjPtr->UnregisterE39CreateObjCB(pICxE39CreateObjCB);
		m_pLogData->CheckResult(hResult, L"ICxE39ObjPtr::ICxE39CreateObjCBPtr");
		m_pLogData->PushLogs(L"ICxE39CreateObjCBPtr::Release");
		pICxE39CreateObjCB.Release();
#endif
		m_pLogData->PushLogs(L"ICxE39ObjPtr::ICxE39InfoCB2Ptr");
		hResult = m_pICxE39ObjPtr->UnregisterE39InfoCallback2(pICxE39InfoCB2);
		m_pLogData->CheckResult(hResult, L"ICxE39ObjPtr::ICxE39InfoCB2Ptr");
		m_pLogData->PushLogs(L"ICxE39InfoCB2Ptr::Release");
		pICxE39InfoCB2.Release();

		m_pLogData->PushLogs(L"ICxE39ObjPtr::ICxE39InfoCBPtr");
		hResult = m_pICxE39ObjPtr->UnregisterE39InfoCallback(pICxE39InfoCB);
		m_pLogData->CheckResult(hResult, L"ICxE39ObjPtr::ICxE39InfoCBPtr");
		m_pLogData->PushLogs(L"ICxE39InfoCBPtr::Release");
		pICxE39InfoCB.Release();

		m_pLogData->PushLogs(L"ICxE39ObjPtr::ICxE39CBPtr");
		hResult = m_pICxE39ObjPtr->UnregisterE39Callback(pICxE39CB);
		m_pLogData->CheckResult(hResult, L"ICxE39ObjPtr::ICxE39CBPtr");
		m_pLogData->PushLogs(L"ICxE39CBPtr::Release");
		pICxE39CB.Release();

		m_pLogData->PushLogs(L"ICxE39::Shutdown");
		hResult = m_pICxE39Ptr->Shutdown();
		m_pLogData->CheckResult(hResult, L"ICxE39::Shutdown");

		m_pLogData->PushLogs(L"ICxE39::Release");
		m_pICxE39Ptr.Release();
	}

	CEBase::Close();
}
