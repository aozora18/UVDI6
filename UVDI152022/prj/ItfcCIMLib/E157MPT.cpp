
#include "pch.h"
#include "MainApp.h"
#include "E157MPT.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                        Philoptics Area                                        */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : logs	- [in]  Log Object Pointer
		share	- [in]  Shared Data Object Pointer
 retn : None
*/
CE157MPT::CE157MPT(CLogData *logs, CSharedData *share)
	: CEBase(logs, share)
{
	/* GUID 설정 */
	m_gdICxE	= IID_ICxE157Object;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CE157MPT::~CE157MPT()
{
	Close();
}

/*
 desc : 초기화 (반드시 호출)
 parm : equip_id	- [in]  Equipment ID (0 or later. 환경 파일에 설정된 개수-1)
		conn_id		- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
 retn : TRUE or FALSE
*/
BOOL CE157MPT::Init(LONG equip_id, LONG conn_id)
{
	if (!CEBase::Init(equip_id, conn_id))	return FALSE;

	/* 아래 순서는 반드시 지킬 것 */
	if (!InitE115MPT())	return FALSE;
	if (!InitE115HW())	return FALSE;

	return TRUE;
}

/*
 desc : 해제
 parm : None
 retn : None
*/
VOID CE157MPT::Close()
{
	/* 아래 순서는 반드시 지킬 것 */
	CloseE115MPT();
	CloseE115HW();

	CEBase::Close();
}

/*
 desc : 초기화 (반드시 호출)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CE157MPT::InitE115MPT()
{
	HRESULT hResult	= S_OK;

	if (m_pICxE157ObjPtr && m_pICxE157ObjPtr.GetInterfacePtr())	return FALSE;

	/* Create & Initialize */
	m_pLogData->PushLogs(L"ICxE157Object::CreateInstance");
	hResult = m_pICxE157ObjPtr.CreateInstance(CLSID_CxE157Object);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE157Object::CreateInstance CLSID_CxE157Object")))	return FALSE;
	m_pLogData->PushLogs(L"ICxE157Object::Initialize"); 
	hResult = m_pICxE157ObjPtr->Initialize(m_pICxAbsLayerPtr);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE157Object::Initialize")))	return FALSE;

	/* Register the global interface */
	m_pLogData->PushLogs(L"RegisterInterfaceInGlobal", m_tzGUID);
	hResult	= m_pGIT->RegisterInterfaceInGlobal(m_pICxE157ObjPtr, m_gdICxE, &m_dwCookie);
	if (FAILED(m_pLogData->CheckResult(hResult, L"RegisterInterfaceInGlobal")))	return FALSE;
	m_pSharedData->GetGITCookie()->e116obj_cookie	= m_dwCookie;

	return TRUE;
}

/*
 desc : 해제
 parm : None
 retn : None
*/
VOID CE157MPT::CloseE115MPT()
{
	HRESULT hResult	= S_OK;

	if (m_pICxE157ObjPtr && m_pICxE157ObjPtr.GetInterfacePtr())
	{
		m_pLogData->PushLogs(L"ICxE157Object::Shutdown");
		hResult = m_pICxE157ObjPtr->Shutdown();
		m_pLogData->CheckResult(hResult, L"ICxE157Object::Shutdown");
		m_pLogData->PushLogs(L"ICxE157Object::Release");
		m_pICxE157ObjPtr.Release();
	}
}

/*
 desc : 초기화 (반드시 호출)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CE157MPT::InitE115HW()
{
	HRESULT hResult	= S_OK;
	CComVariant vErrors;

	/* Initialize E157 Module Process Tracking */
	m_pLogData->PushLogs(L"ICxE157Object::AddModule ProcessChamber 3");
	hResult = m_pICxE157ObjPtr->AddModule(CComBSTR("ProcessChamber"),
										  (INT32)ENG_TMPI::en_process_chamber, &vErrors);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE157Object::AddModule ProcessChamber 3")))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : 해제
 parm : None
 retn : None
*/
VOID CE157MPT::CloseE115HW()
{
}

/*
 desc : Module Process Tracking 실행 시작
 parm : mod_name- [in]  Name of the EPT module, use an epty string for the Equipemnt ETP state
						ex> ProcessChamber, RobotArm, PreAligner, Buffer, etc
		pj_id	- [in]  Process Job Name ID (String; Ascii)
		rp_id	- [in]  Recipe ID
		st_id	- [in]  Substrate ID (Normally, Destination ID)
 retn : TRUE or FALSE
*/
BOOL CE157MPT::MPTExecutionStarted(PTCHAR mod_name, PTCHAR pj_id, PTCHAR rp_id, PTCHAR st_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	HRESULT hResult	= S_OK;
	CComVariant vErrors;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"MPTExecutionStarted::MPTExecutionStarted : "
										  L"mod_name=%s, pj_id=%s, rp_id=%s, st_id=%s",
										  mod_name, pj_id, rp_id, st_id);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxE157ObjPtr->MPTExecutionStarted(CComBSTR(mod_name), CComBSTR(pj_id),
												   CComBSTR(rp_id), CComBSTR(rp_id), CComBSTR(st_id),
												   NULL, &vErrors); 
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Module Process Tracking 실행 완료 (레시피 실행 완료)
 parm : mod_name- [in]  Name of the EPT module, use an epty string for the Equipemnt ETP state
						ex> ProcessChamber, RobotArm, PreAligner, Buffer, etc
		exec_ok	- [in]  TRUE : Execution success, FALSE : Execution failed
 retn : TRUE or FALSE
*/
BOOL CE157MPT::MPTExecutionCompleted(PTCHAR mod_name, BOOL exec_ok)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	HRESULT hResult	= S_OK;
	VARIANT_BOOL vbExecOk	= exec_ok ? VARIANT_TRUE : VARIANT_FALSE;
	CComVariant vErrors;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"MPTExecutionStarted::MPTExecutionStarted : "
										  L"mod_name=%s, exec_ok=%d", mod_name, exec_ok);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxE157ObjPtr->MPTExecutionCompleted(CComBSTR(mod_name), vbExecOk, &vErrors); 
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Module Process Tracking 단계 시작
 parm : mod_name- [in]  Name of the EPT module, use an epty string for the Equipemnt ETP state
						ex> ProcessChamber, RobotArm, PreAligner, Buffer, etc
		step_id	- [in]  Step ID (Name)
						ex> LithographStep1 ~ LithographStep??
 retn : TRUE or FALSE
*/
BOOL CE157MPT::MPTStepStarted(PTCHAR mod_name, PTCHAR step_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	HRESULT hResult	= S_OK;
	CComVariant vErrors;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetMPTStepStarted::MPTStepStarted : "
										  L"mod_name=%s, step_id=%s", mod_name, step_id);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxE157ObjPtr->MPTStepStarted(CComBSTR(mod_name), CComBSTR(step_id), &vErrors); 
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Module Process Tracking 단계 완료
 parm : mod_name- [in]  Name of the EPT module, use an epty string for the Equipemnt ETP state
						ex> ProcessChamber, RobotArm, PreAligner, Buffer, etc
		step_ok	- [in]  TRUE : Step success, FALSE : step failed
 retn : TRUE or FALSE
*/
BOOL CE157MPT::MPTStepCompleted(PTCHAR mod_name, BOOL step_ok)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	HRESULT hResult	= S_OK;
	VARIANT_BOOL vbStepOk	= step_ok ? VARIANT_TRUE : VARIANT_FALSE;
	CComVariant vErrors;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetMPTStepCompleted::MPTStepCompleted : mod_name=%s", mod_name);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxE157ObjPtr->MPTStepCompleted(CComBSTR(mod_name), vbStepOk, &vErrors); 
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}
