
#include "pch.h"
#include "MainApp.h"
#include "E116EPT.h"


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
CE116EPT::CE116EPT(CLogData *logs, CSharedData *share)
	: CEBase(logs, share)
{
	/* GUID 설정 */
	m_gdICxE	= IID_ICxE116Object;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CE116EPT::~CE116EPT()
{
	Close();

}

/*
 desc : 초기화 (반드시 호출)
 parm : equip_id	- [in]  Equipment ID (0 or later. 환경 파일에 설정된 개수-1)
		conn_id		- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
 retn : TRUE or FALSE
*/
BOOL CE116EPT::Init(LONG equip_id, LONG conn_id)
{
	if (!CEBase::Init(equip_id, conn_id))	return FALSE;

	/* 아래 순서는 반드시 지킬 것 */
	if (!InitE116EPT())	return FALSE;
	if (!InitE116HW())	return FALSE;

	return TRUE;
}

/*
 desc : 해제
 parm : None
 retn : None
*/
VOID CE116EPT::Close()
{
	/* 아래 순서는 반드시 지킬 것 */
	CloseE116HW();
	CloseE116EPT();

	CEBase::Close();
}

/*
 desc : 초기화 (반드시 호출)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CE116EPT::InitE116EPT()
{
	HRESULT hResult	= S_OK;

	if (m_pICxE116ObjPtr && m_pICxE116ObjPtr.GetInterfacePtr())	return FALSE;

	/* Create & Initialize */
	m_pLogData->PushLogs(L"ICxE116Object::CreateInstance");
	hResult = m_pICxE116ObjPtr.CreateInstance(CLSID_CxE116Object);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE116Object::CreateInstance CLSID_CxE116Object")))	return FALSE;
	m_pLogData->PushLogs(L"ICxE116Object::Initialize");
	hResult = m_pICxE116ObjPtr->Initialize(m_pICxE39ObjPtr, m_pICxAbsLayerPtr, CComBSTR("CIM300Simulation"));
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE116Object::Initialize")))	return FALSE;

	/* Register the global interface */
	m_pLogData->PushLogs(L"RegisterInterfaceInGlobal", m_tzGUID);
	hResult	= m_pGIT->RegisterInterfaceInGlobal(m_pICxE116ObjPtr, m_gdICxE, &m_dwCookie);
	if (FAILED(m_pLogData->CheckResult(hResult, L"RegisterInterfaceInGlobal")))	return FALSE;
	m_pSharedData->GetGITCookie()->e116obj_cookie	= m_dwCookie;

	return TRUE;
}

/*
 desc : 해제
 parm : None
 retn : None
*/
VOID CE116EPT::CloseE116EPT()
{
	HRESULT hResult	= S_OK;

	if (m_pICxE116ObjPtr && m_pICxE116ObjPtr.GetInterfacePtr())
	{
		m_pLogData->PushLogs(L"ICxE116Object::Shutdown");
		hResult = m_pICxE116ObjPtr->Shutdown();
		m_pLogData->CheckResult(hResult, L"ICxE116Object::Shutdown");
		m_pLogData->PushLogs(L"ICxE116Object::Release");
		m_pICxE116ObjPtr.Release();
	}
}

/*
 desc : 초기화 (반드시 호출)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CE116EPT::InitE116HW()
{
	HRESULT hResult	= S_OK;

	if (!m_pICxE116ObjPtr)						return FALSE;
	if (!m_pICxE116ObjPtr.GetInterfacePtr())	return FALSE;

	/* Initialize E116 Equipment Performance Tracking */
	m_pLogData->PushLogs(L"ICxE116Object::AddEFEMModule2 RobotArm 1");
	hResult = m_pICxE116ObjPtr->AddEFEMModule2(CComBSTR("RobotArm"), (SHORT)ENG_TSSI::en_robot);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE116Object::AddEFEMModule2 RobotArm 1")))				return FALSE;
	m_pLogData->PushLogs(L"ICxE116Object::AddEFEMModule2 PreAligner 2");
	hResult = m_pICxE116ObjPtr->AddEFEMModule2(CComBSTR("PreAligner"),	(SHORT)ENG_TSSI::en_pre_aligner);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE116Object::AddEFEMModule2 PreAligner 2")))			return FALSE;
	m_pLogData->PushLogs(L"ICxE116Object::AddProductionModule2 ProcessChamber 3");
	hResult = m_pICxE116ObjPtr->AddProductionModule2( CComBSTR("ProcessChamber"),	(SHORT)ENG_TSSI::en_process_chamber);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE116Object::AddProductionModule2 ProcessChamber 3")))	return FALSE;
	m_pLogData->PushLogs(L"ICxE116Object::AddEFEMModule2 Buffer 4");
	hResult = m_pICxE116ObjPtr->AddEFEMModule2(CComBSTR("Buffer"),	(SHORT)ENG_TSSI::en_buffer);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE116Object::AddEFEMModule2 Buffer 4")))				return FALSE;
	m_pLogData->PushLogs(L"ICxE116Object::AddEFEMModule2 LoadPort1 5");
	hResult = m_pICxE116ObjPtr->AddEFEMModule2(CComBSTR("LoadPort1"),	(SHORT)ENG_TSSI::en_load_port_1);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE116Object::AddEFEMModule2 LoadPort1 5")))				return FALSE;
	m_pLogData->PushLogs(L"ICxE116Object::AddEFEMModule2 LoadPort2 6");
	hResult = m_pICxE116ObjPtr->AddEFEMModule2(CComBSTR("LoadPort2"),	(SHORT)ENG_TSSI::en_load_port_2);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxE116Object::AddEFEMModule2 LoadPort2 6")))				return FALSE;

	return (hResult == S_OK);
}

/*
 desc : 해제
 parm : None
 retn : None
*/
VOID CE116EPT::CloseE116HW()
{
}

/*
 desc : EPT (Equiptment Performance Tracking) - Busy 설정
 parm : mod_name	- [in]  Name of the EPT module, use an epty string for the Equipemnt ETP state
							ex> ProcessChamber, RobotArm, PreAligner, Buffer, etc
		task_name	- [in]  Name of the task
							ex> Lithography, Align, etc
		task_type	- [in]  Type of task (E116_ETBS)
 retn : TRUE or FALSE
 note : This method requests that an EPT module or the EPT equipment state machine transition to the BUSY state
		Note: Using this function to directly set Equipment state to BUSY is not compliant with E116-1104
			  (and later revisions).
*/
BOOL CE116EPT::SetBusy(PTCHAR mod_name, PTCHAR task_name, E116_ETBS task_type)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	HRESULT hResult	= S_OK;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetBusy::Busy : mod_name=%s, task_name=%s, task_type=%d",
			   mod_name, task_name, task_type);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxE116ObjPtr->Busy(CComBSTR(mod_name), CComBSTR(task_name), CxEPTTaskType(task_type)); 
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : EPT (Equiptment Performance Tracking) - Idle 설정
 parm : mod_name	- [in]  Name of the EPT module, use an epty string for the Equipemnt ETP state
							ex> ProcessChamber, RobotArm, PreAligner, Buffer, etc
 retn : TRUE or FALSE
 note : This method requests that an EPT module or the EPT equipment state machine transition to the IDLE state
		Note: Using this function to directly set Equipment state to IDLE is not compliant with E116-1104
			  (and later revisions)
			  Also use at startup for the NoState->Idle transition during initialization
*/
BOOL CE116EPT::SetIdle(PTCHAR mod_name)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE] = {NULL};
	HRESULT hResult	= S_OK;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"SetIdle::Idle : mod_name=%s", mod_name);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxE116ObjPtr->Idle(CComBSTR(mod_name)); 
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}
