
#include "pch.h"
#include "MainApp.h"
#include "E30GEM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 desc : 생성자
 parm : logs	- [in]  Log Object Pointer
		share	- [in]  Shared Data Object Pointer
		conn_cnt- [in]  Connection Counts (CONNECTION 개수)
 retn : None
*/
CE30GEM::CE30GEM(CLogData *logs, CSharedData *share, LONG conn_cnt)
	: CEBase(logs, share)
{
	m_bIsInitConnected	= FALSE;
	m_bIsInitCompleted	= FALSE;
	m_bIsLoadProject	= FALSE;

	/* GUID 설정 */
	m_gdICxE			= IID_ICxClientApplication;

	/* EPJ 파일에 등록된 Connection 개수 */
	m_lConnectionCount	= conn_cnt;

	/* Tracking System에 등록된 Module (Location) Name 저장 */
	m_arrEquipLocName.Add(L"RobotArm");
	m_arrEquipLocName.Add(L"PreAligner");
	m_arrEquipLocName.Add(L"ProcessChamber");
	m_arrEquipLocName.Add(L"Buffer");
	m_arrEquipLocName.Add(L"LoadPort1");
	m_arrEquipLocName.Add(L"LoadPort2");

	/* Tracking System에 등록된 Processing Name 저장 */
	m_arrEquipProcName.Add(L"Need Processing");
	m_arrEquipProcName.Add(L"In Process");
	m_arrEquipProcName.Add(L"Processed");
	m_arrEquipProcName.Add(L"Aborted");
	m_arrEquipProcName.Add(L"Stopped");
	m_arrEquipProcName.Add(L"Rejected");
	m_arrEquipProcName.Add(L"Lost");
	m_arrEquipProcName.Add(L"Skipped");
	m_arrEquipProcName.Add(L"No State");
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CE30GEM::~CE30GEM()
{
	Close();

	/* 메모리 반환 */
	m_arrEquipLocName.RemoveAll();
	m_arrEquipProcName.RemoveAll();
}

/*
 desc : 초기화 (반드시 호출)
 parm : equip_id	- [in]  Equipment ID (0 or later. 환경 파일에 설정된 개수-1)
		conn_id		- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
 retn : TRUE or FALSE
*/
BOOL CE30GEM::Init(LONG equip_id, LONG conn_id)
{
	if (!CEBase::Init(equip_id, conn_id))	return FALSE;

	HRESULT hResult	= S_OK;

	if (!InitE30GEM(equip_id))
	{
		AfxMessageBox(L"Failed to initializ the CIMConnect Lib.", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* SECS-II Message Instance 생성 */
	hResult	= m_pICxSECSMesg.CreateInstance(CLSID_CxValueObject);
	if (m_pLogData->CheckResult(hResult, L"Init::SECS-II Message. CreateInstance"))	return FALSE;


	return TRUE;
}

/*
 desc : 해제
 parm : None
 retn : None
*/
VOID CE30GEM::Close()
{
	CloseE30GEM();

	/* SECS-II Message 해제 */
	if (m_pICxSECSMesg.GetInterfacePtr())	m_pICxSECSMesg.Release();

	CEBase::Close();
}

/*
 desc : 초기화 (반드시 호출)
 parm : equip_id	- [in]  Equipment ID (0 or later. 환경 파일에 설정된 개수-1)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::InitE30GEM(LONG equip_id)
{
	HRESULT hResult	= S_FALSE;
	LONG lWaitResult, lAppID;

	/* 플래그 설정 중요 */
	m_bIsInitCompleted	= FALSE;

	if (m_pICxEMService && m_pICxEMService.GetInterfacePtr())	return FALSE;

	/* Establish communication with CIMConnect (EMService) */
	m_pLogData->PushLogs(L"ICxEMService::CreateInstance");
	hResult = m_pICxEMService.CreateInstance(CLSID_CxEMService);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxEMService::CreateInstance")))	return FALSE;
	/* Wait for CIMConnect to be started up. */
	m_pLogData->PushLogs(L"ICxEMService::WaitOnEquipmentReady");
	hResult = m_pICxEMService->WaitOnEquipmentReady(equip_id, 5000, &lWaitResult);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxEMService::WaitOnEquipmentReady")))	return FALSE;
	if (lWaitResult == WAIT_TIMEOUT)
	{
		m_pLogData->PushLogs(L"ICxEMService::WaitOnEquipmentReady WAIT_TIMEOUT");
		return FALSE;
	}

	/* Establish a connection with CIMConnect */
	m_pLogData->PushLogs(L"ICxEMService::Connect");
	hResult = m_pICxEMService->Connect(equip_id, &m_pICxClientApp, &lAppID);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxEMService::Connect")))	return FALSE;

	/* Callback registration */
	m_pLogData->PushLogs(L"IEMSerivceCbkPtr::CreateInstance");
	hResult = m_pEMServiceCbk.CreateInstance(CLSID_EMSvcCbk);
	if (FAILED(m_pLogData->CheckResult(hResult, L"EMSerivceCbk::CreateInstance")))	return FALSE;
	/* Connect to Shared & Log Object */
	m_pLogData->PushLogs(L"IEMSerivceCbkPtr::SetSharedData && SetLogData");
	m_pEMServiceCbk->SetSharedData((LONG_PTR)m_pSharedData);
	m_pEMServiceCbk->SetLogData((LONG_PTR)m_pLogData);
	m_pLogData->PushLogs(L"IEMSerivceCbkPtr::Query Interface ICxEMAppCallbackPtr");

	/* Smart Pointer 연결 */
	m_pICxClientTool	= m_pICxClientApp;
	m_pICxClientData	= m_pICxClientApp;
	m_pICxHostEmulator	= m_pICxClientApp;
	m_pICxEMAppCbk2		= m_pEMServiceCbk;
	m_pICxEMAppCbk3		= m_pEMServiceCbk;
	m_pICxEMAppCbk5		= m_pEMServiceCbk;
	m_pICxEMAppCbk		= m_pEMServiceCbk;
	m_pICxEMMsgCbk		= m_pEMServiceCbk;
	m_pICxVersion		= m_pICxEMService;
	m_pICxDiagnostics	= m_pICxEMService;

	/* Register the global interface */
	m_pLogData->PushLogs(L"RegisterInterfaceInGlobal", m_tzGUID);
	hResult = m_pGIT->RegisterInterfaceInGlobal(m_pICxClientApp, m_gdICxE, &m_dwCookie);
	if (FAILED(m_pLogData->CheckResult(hResult, L"RegisterInterfaceInGlobal")))	return FALSE;
	m_pSharedData->GetGITCookie()->clientapp_cookie	= m_dwCookie;

	/* 일단 초기화 성공 */
	m_bIsInitConnected	= TRUE;

	return TRUE;
}

/*
 desc : 해제
 parm : None
 retn : None
*/
VOID CE30GEM::CloseE30GEM()
{
	LONG i	= 1;

	if (m_pICxClientApp && m_pICxClientApp.GetInterfacePtr())
	{
		/* Unregister */
		UnregisterValueChangeID();
		UnregisterGetValueID();
		UnregisterCommonID();
		UnregisterCommandID();
		UnregisterMessageID();
		/* Disable Communication */
		m_pLogData->PushLogs(L"ICxClientApplication::DisableComm");
		for (; i<=m_lConnectionCount; i++)	m_pICxClientApp->DisableComm(i);
		m_pLogData->PushLogs(L"ICxClientApplication::Release");
		m_pICxClientApp.Release();
	}
	m_pLogData->PushLogs(L"ICxClientTool::Release");
	if (m_pICxClientTool && m_pICxClientTool.GetInterfacePtr())		m_pICxClientTool.Release();
	m_pLogData->PushLogs(L"ICxClientData::Release");
	if (m_pICxClientData && m_pICxClientData.GetInterfacePtr())		m_pICxClientData.Release();
	m_pLogData->PushLogs(L"ICxEMAppCallback5Ptr::Release");
	if (m_pICxEMAppCbk5 && m_pICxEMAppCbk5.GetInterfacePtr())		m_pICxEMAppCbk5.Release();
	m_pLogData->PushLogs(L"ICxEMMsgCallbackPtr::Release");
	if (m_pICxEMMsgCbk && m_pICxEMMsgCbk.GetInterfacePtr())			m_pICxEMMsgCbk.Release();
	m_pLogData->PushLogs(L"ICxHostEmulatorPtr::Release");
	if (m_pICxHostEmulator && m_pICxHostEmulator.GetInterfacePtr())	m_pICxHostEmulator.Release();
#ifndef _DEBUG
	m_pLogData->PushLogs(L"ICxEMService::StopService");
	if (m_pICxEMService && m_pICxEMService.GetInterfacePtr())		m_pICxEMService->StopService();
#endif
	m_pLogData->PushLogs(L"ICxEMService::Release");
	if (m_pICxEMService && m_pICxEMService.GetInterfacePtr())		m_pICxEMService.Release();
	m_pLogData->PushLogs(L"ICxEMAppCallback::Release");
	if (m_pICxEMAppCbk && m_pICxEMAppCbk.GetInterfacePtr())			m_pICxEMAppCbk.Release();
	m_pLogData->PushLogs(L"ICxEMAppCallback::Release");
	if (m_pEMServiceCbk && m_pEMServiceCbk.GetInterfacePtr())		m_pEMServiceCbk.Release();
}

/*
 desc : 각종 초기화 플래그 값들 Reset 처리
 parm : None
 retn : None
*/
VOID CE30GEM::ResetInitFlag()
{
	/* 플래그 중요 */
	m_bIsInitConnected	= FALSE;
	m_bIsInitCompleted	= FALSE;
	m_bIsLoadProject	= FALSE;
}

/*
 desc : 현재 적재되어 있는 프로젝트 정보를 파일로 저장
 parm : epj_file	- [in]  저장하고자 하는 파일 이름 (전체 경로 포함)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SaveProjetsToEPJ(PTCHAR epj_file)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;

	if (!m_pICxClientData)	return FALSE;
	if (!m_bIsLoadProject)	return FALSE;
	
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SaveProjetsToEPJ::SaveProject (epj file = %s)", epj_file);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxClientTool->SaveProject(CComBSTR(epj_file));
	m_pLogData->CheckResult(hResult, tzMesg);

	return (S_OK == hResult);
}

/*
 desc : EMService가 동작 중인지 여부
 parm : None
 retn : TRUE or FALSE
*/
BOOL CE30GEM::IsServiceRun()
{
	HRESULT hResult	= S_FALSE;

	if (!m_pICxClientData)	return FALSE;
	hResult	= m_pICxClientData->Ping();

	return (S_OK == hResult);
}

/*
 desc : 연결이 초기화 되었는지 여부
 parm : logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
BOOL CE30GEM::IsInitConnected(BOOL logs)
{
	if (logs && !m_bIsInitConnected)
	{
		m_pLogData->PushLogs(L"The library is not initialized", ENG_GALC::en_warning);
	}
	return m_bIsInitConnected;
}

/*
 desc : 모두 초기화 되었는지 여부
 parm : logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
BOOL CE30GEM::IsInitedAll(BOOL logs)
{
	BOOL bIsInit	= m_bIsLoadProject && m_bIsInitCompleted && m_bIsInitConnected;
	if (logs && !bIsInit)
	{
		m_pLogData->PushLogs(L"The library is not initialized", ENG_GALC::en_warning);
	}
	return bIsInit;
}

/*
 desc : CIMConnect 초기화 수행 후 완료 작업 진행
 parm : equip_id	- [in]  Equipment ID (0 or later. 환경 파일에 설정된 개수-1)
		conn_id		- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
 retn : TRUE or FALSE
 note : Final initialization of the GEM interface. Until this is called, communication
		with the GEM Host is not permitted. This allows for equipment initialization to
		be completed as well as initialization of CIMConnect and this application. 
*/
BOOL CE30GEM::Finalized(LONG equip_id, LONG conn_id)
{
	INT32 i			= 1;
	LONG lID		= 0;
	HRESULT hResult	= S_OK;
	UNG_CVVT unValue= {NULL};

	for (; i<=m_lConnectionCount; i++)
	{
		/* Get/Set the communication state */
		if (!GetVarNameToID(i, L"DefaultCommState", lID, TRUE))	return FALSE;
		if (!GetValue(i, lID /*DefaultCommState*/, E30_GPVT::en_u4, &unValue, 0, 0))	return FALSE;
		/* Default Communication State 값이 Enable이라면, 초기화 이후 강제로 Enable 시키는 작업 수행 */
		if (256 == unValue.u32_value)
		{
			hResult	= m_pICxClientApp->EnableComm(i);
			if (FAILED(hResult))	m_pLogData->CheckResult(hResult, L"InitializeFinal::EnableComm");
		}

		/* Get/Set the control state */
		if (!GetVarNameToID(i, L"DefaultCtrlState", lID, TRUE))	return FALSE;
		if (!GetValue(i, lID /*DefaultCtrlState*/, E30_GPVT::en_u1, &unValue, 0, 0))	return FALSE;
		/* Default Control State 값이 Online이라면, 초기화 이후 강제로 Onlie-Remote 시키는 작업 수행 */
		if (0 == unValue.u8_value /* online */)
		{
			hResult	= m_pICxClientApp->GoRemote(i);
			if (FAILED(hResult))	m_pLogData->CheckResult(hResult, L"InitializeFinal::GetRemote");
		}
#if 0
		/* Set the spool state. Fixed Enable */
		hResult	= m_pICxClientApp->EnableSpooling(i);
		if (FAILED(hResult))	m_pLogData->CheckResult(hResult, L"InitializeFinal::EnableSpooling");
#endif
    }

	/* GEM Host Communication Status */
	if (!GetValue(conn_id, 2036/* CommState */, E30_GPVT::en_u4, &unValue, 0, 0))	return FALSE;
	m_pSharedData->E30_PushState(E30_SSMS::en_commstate, conn_id, LONG(unValue.u32_value));	/* 공유 메모리에 상태 값 설정  */
	/* GEM Host Control Status */
	if (!GetValue(conn_id, 2028/* CONTROLSTATE */, E30_GPVT::en_u1, &unValue, 0, 0))	return FALSE;
	m_pSharedData->E30_PushState(E30_SSMS::en_control, conn_id, LONG(unValue.u8_value));		/* 공유 메모리에 상태 값 설정  */

	if (!SetUpdateProcessState(E30_GPSV::en_idle))	return FALSE;	/* Idle */

	/* 초기화 완료 이벤트 수신 여부 */
	m_bIsInitCompleted	= TRUE;

	return TRUE;
}

/*
 desc : EPJ 파일 내용 확인 (검사)
 parm : epj_file	- [in]  EPJ 파일 (전체 경로)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::LoadProject(PTCHAR epj_file)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	INT32 i32LastBackslash			= 0;
	LONG lWaitResult				= 0;
	HRESULT hResult					= NULL;
	CString strPrjCurrent, strPrjDesired, strPrjDefault;
	CComBSTR bstrCurPrj, bstrTgtPrj (epj_file)	/* 변경될 Project File (EPJ) */;
	CUniToChar csCnv;

	if (!m_pICxClientTool)	return FALSE;
	/* Make sure the correct EPJ file is loaded */
	m_pLogData->PushLogs(L"ICxClientTool::GetCurrentProject");
	hResult = m_pICxClientTool->GetCurrentProject(&bstrCurPrj);	/* 현재 적재된 Project File (EPJ) 얻기 */
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxClientTool::GetCurrentProject")))	return FALSE;

	/* 현재 프로젝트 파일과 기존 프로젝트 파일이 동일하더라도, EPJ 내용이 바뀌면, 적재 작업 다시 수행 */
	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"ICxClientTool::LoadProject (%s)", epj_file);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientTool->LoadProject(bstrTgtPrj);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxClientTool::LoadProject")))	return FALSE;
	m_pLogData->PushLogs(L"ICxEMService::WaitOnEquipmentReady");
	hResult = m_pICxEMService->WaitOnEquipmentReady(0, 30000, &lWaitResult);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxEMService::WaitOnEquipmentReady")))	return FALSE;
	if (lWaitResult == WAIT_TIMEOUT)
	{
		m_pLogData->PushLogs(L"ICxEMService::WaitOnEquipmentReady WAIT_TIMEOUT");
		return FALSE;
	}

	/* Check the default EPJ file (기존 프로젝트와 새로 입력된 프로젝트가 다른 경우 인지 확인) */
	m_pLogData->PushLogs(L"ICxClientTool::GetDefaultProjectFile");
	hResult = m_pICxClientTool->GetDefaultProjectFile(&bstrCurPrj);
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxClientTool::GetDefaultProjectFile")))	return FALSE;
	if (bstrCurPrj != bstrTgtPrj)
	{
		swprintf_s(tzMesg, CIM_LOG_MESG_SIZE, L"ICxClientTool::SetDefaultProjectFile (%s)", epj_file);
		m_pLogData->PushLogs(tzMesg);
		hResult = m_pICxClientTool->SetDefaultProjectFile(bstrTgtPrj);
		if (FAILED(m_pLogData->CheckResult(hResult, L"ICxClientTool::SetDefaultProjectFile")))	return FALSE;
	}

	/* 이벤트 (콜백) ID 등록 */
	if (!RegisterCommonID())	return FALSE;
	if (!RegisterCommandID())	return FALSE;
	/* 프로젝트 적재 성공  */
	m_bIsLoadProject	= TRUE;

	return TRUE;
}

/*
 desc : 장치에 정의된 모든 데이터 항목들을 제거 (주의해서 사용해야 됨)
 parm : logs	- [in]  로그 출력 여부
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SetClearProject(BOOL logs)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetClearProject::ClearProject()");
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientTool->ClearProject();
	m_pLogData->CheckResult(hResult, tzMesg);

	return TRUE;
}

/*
 desc : 현재 작업 중에 있는지 여부... 즉, Busy 상태인지 여부
 parm : time_out	- [in]  최대 대기 시간 (단위: msec)
 retn : TRUE (Ready) or FALSE (BUSY)
*/
BOOL CE30GEM::WaitOnEMServiceIdle(UINT32 time_out)
{
	BOOL bIdled		= FALSE;
	UINT32 u32Tick	= (UINT32)GetTickCount();

	do {

		bIdled	= uvCmn_IsServiceStatus(L"EMService", 0x01);

	} while (!bIdled && ((UINT32)GetTickCount() < (u32Tick + time_out)));

	return bIdled;
}

/*
 desc : 콜백 함수 등록 (Value Change ID)
		EPJ 프로젝트 파일 내에 등록된 Event ID 중 값 변경에 따라, PCIMLib에게 알려줄 필요가 있는 경우
		해당 Event ID 값 등록해 줘야 됨
 parm : lst_val	- [in]  이벤트를 등록하려는 정보가 저장된 리스트 버퍼
 retn : TRUE or FALSE
*/
BOOL CE30GEM::RegisterValueChangeID(CAtlList <STG_TVCI> *lst_val)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE];
	UINT32 i	= 0;
	STG_TVCI stData;

	for (; i<lst_val->GetCount(); i++)
	{
		/* 이벤트 등록 정보 1개 추출 */
		stData	= lst_val->GetAt(lst_val->FindIndex(i));
		/* Log 설정  */
		wmemset(tzMesg, 0x00, CIM_CMD_DESC_SIZE);
		swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"RegisterValueChangedHandler (%s:%I32u)",
				   stData.var_name, stData.var_id);
		/* 이벤트 등록 */
		if (!RegisterValueChangeID(stData.conn_id, stData.var_id, tzMesg))	return FALSE;
	}

	return TRUE;
}

/*
 desc : 콜백 함수 등록 (Value Change ID)
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		var_id	- [in]  Variable ID
		mesg	- [in]  출력될 로그 메시지
 retn : TRUE or FALSE
*/
BOOL CE30GEM::RegisterValueChangeID(LONG conn_id, LONG var_id, PTCHAR mesg)
{
	HRESULT hResult		= S_OK;
	LPM_REHI pstEvent	= NULL;

	m_pLogData->PushLogs(mesg);
	hResult = m_pICxClientApp->RegisterValueChangedHandler(conn_id, var_id, m_pICxEMAppCbk);
	if (FAILED(m_pLogData->CheckResult(hResult, mesg)))	return FALSE;

	/* 등록된 이벤트 저장 */
	pstEvent	= (LPM_REHI)::Alloc(sizeof(STM_REHI));
	ASSERT(pstEvent);
	pstEvent->conn_id	= conn_id;
	pstEvent->evt_id	= var_id;
	m_lstEvtValC.AddTail(pstEvent);

	return (hResult == S_OK);
}

/*
 desc : 콜백 함수 해제 (Value Change ID)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CE30GEM::UnregisterValueChangeID()
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;
	POSITION pPos		= NULL;
	LPM_REHI pstEventID	= NULL;

	/* 라이브러리 내부 혹은 외부 (GUI)에서 등록된 Callback Event for ID 제거 */
	pPos	= m_lstEvtValC.GetHeadPosition();
	while (pPos)
	{
		wmemset(tzMesg, 0x00, CIM_CMD_DESC_SIZE);
		pstEventID	= m_lstEvtValC.GetNext(pPos);
		if (pstEventID)
		{
			swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"UnregisterValueChangeID::UnregisterValueChangedHandler [ conn_id=%d ] [ var_id=%d ]",
					   pstEventID->conn_id, pstEventID->evt_id);
			hResult = m_pICxClientApp->UnregisterValueChangedHandler(pstEventID->conn_id, pstEventID->evt_id);
			m_pLogData->CheckResult(hResult, tzMesg);
			::Free(pstEventID);
		}
	}
	m_lstEvtValC.RemoveAll();

	return (hResult == S_OK);
}

/*
 desc : 콜백 함수 등록 (Get Value ID)
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		var_id	- [in]  Variable ID
		mesg	- [in]  출력될 로그 메시지
 retn : TRUE or FALSE
*/
BOOL CE30GEM::RegisterGetValueID(LONG conn_id, LONG var_id, PTCHAR mesg)
{
	HRESULT hResult		= S_OK;
	LPM_REHI pstEvent	= NULL;

	m_pLogData->PushLogs(mesg);
	hResult = m_pICxClientApp->RegisterGetValueHandler(conn_id, var_id, m_pICxEMAppCbk);
	if (FAILED(m_pLogData->CheckResult(hResult, mesg)))	return FALSE;

	/* 등록된 이벤트 저장 */
	pstEvent	= (LPM_REHI)::Alloc(sizeof(STM_REHI));
	ASSERT(pstEvent);
	pstEvent->conn_id	= conn_id;
	pstEvent->evt_id	= var_id;
	m_lstEvtValG.AddTail(pstEvent);

	return (hResult == S_OK);
}

/*
 desc : 콜백 함수 해제 (Get Value ID)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CE30GEM::UnregisterGetValueID()
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;
	POSITION pPos		= NULL;
	LPM_REHI pstEventID	= NULL;

	/* 라이브러리 내부 혹은 외부 (GUI)에서 등록된 Callback Event for ID 제거 */
	pPos	= m_lstEvtValG.GetHeadPosition();
	while (pPos)
	{
		wmemset(tzMesg, 0x00, CIM_CMD_DESC_SIZE);
		pstEventID	= m_lstEvtValG.GetNext(pPos);
		if (pstEventID)
		{
			swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"UnregisterGetValueID::UnregisterGetValueHandler [ conn_id=%d ] [ var_id=%d ]",
					   pstEventID->conn_id, pstEventID->evt_id);
			hResult = m_pICxClientApp->UnregisterGetValueHandler(pstEventID->conn_id, pstEventID->evt_id);
			m_pLogData->CheckResult(hResult, tzMesg);
			::Free(pstEventID);
		}
	}
	m_lstEvtValG.RemoveAll();

	return (hResult == S_OK);
}

/*
 desc : 콜백 함수 등록 (Command)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CE30GEM::RegisterCommandID()
{
	/* ----------------------------------------------------------------------------------------- */
	/* Register for Remote Commands (Remote Command name의 총 길이가 20 string을 넘지 않아야 됨) */
	/* 참고로, 모든 원격 제어 (Host) 명령어는 대문자로 구성, Argument는 대부분 Zero 값으로 처리. */
	/* ----------------------------------------------------------------------------------------- */
	/* PPSELECT	: Select a recipe for execution                                                  */
	/*            PPID argument to identify the recipe ID                                        */
	if (!RegisterCommandID(L"PPSELECT",	L"Select a recipe for execution.",				L"RegisterCommandHandler PPSELECT"))		return FALSE;
	/* START	: Start processing the recipe                                                    */
	if (!RegisterCommandID(L"START",	L"Start processing. Only valid while IDLE",		L"RegisterCommandHandler START"))			return FALSE;
	/* STOP		: Finish the current processing and come to a safe stop,                         */
	/*            without damage to material or equipment                                        */
	if (!RegisterCommandID(L"STOP",		L"Stop processing. Only valid while EXECUTING",	L"RegisterCommandHandler STOP"))			return FALSE;
	/* PAUSE	: Safely come to a pause, without damage to material or equipment                */
	if (!RegisterCommandID(L"PAUSE",	L"Pause processing",							L"RegisterCommandHandler PAUSE"))			return FALSE;
	/* RESUME	:  */
	if (!RegisterCommandID(L"RESUME",	L"Resume after a PAUSE command",				L"RegisterCommandHandler RESUME"))			return FALSE;
	if (!RegisterCommandID(L"ABORT",	L"Abort processing",							L"RegisterCommandHandler ABORT"))			return FALSE;
	/* ----------------------------------------------------------------------------------------- */

	return TRUE;
}

/*
 desc : 콜백 함수 등록 (Command)
 parm : cmd		- [in]  등록될 명령 문자열
		desc	- [in]  등록될 메시지
		mesg	- [in]  출력될 로그 메시지
 retn : TRUE or FALSE
*/
BOOL CE30GEM::RegisterCommandID(CComBSTR cmd, CComBSTR desc, PTCHAR mesg)
{
	HRESULT hResult		= S_OK;
	LPM_RECI pstEvent	= NULL;
	_bstr_t bstrCmd		= (BSTR)cmd;

	m_pLogData->PushLogs(mesg);
	hResult = m_pICxClientApp->RegisterCommandHandler(cmd, &desc, m_pICxEMAppCbk);
	if (FAILED(m_pLogData->CheckResult(hResult, mesg)))	return FALSE;

	/* 등록된 이벤트 저장 */
	pstEvent	= (LPM_RECI)::Alloc(sizeof(STM_RECI));
	ASSERT(pstEvent);
	pstEvent->cmd	= (PTCHAR)::Alloc(sizeof(TCHAR) * CIM_CMD_NAME_SIZE+sizeof(TCHAR));
	wmemset(pstEvent->cmd, 0x00, CIM_CMD_NAME_SIZE+1);
	wcscpy_s(pstEvent->cmd, CIM_CMD_NAME_SIZE, bstrCmd);
	m_lstEvtCmds.AddTail(pstEvent);

	return (hResult == S_OK);
}

/*
 desc : 콜백 함수 해제 (Command)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CE30GEM::UnregisterCommandID()
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult				= S_OK;
	POSITION pPos				= NULL;
	LPM_RECI pstEventCmd		= NULL;
	CComBSTR bstrRemoteCmd;

	/* 라이브러리 내부 혹은 외부 (GUI)에서 등록된 Callback Event for ID 제거 */
	pPos	= m_lstEvtCmds.GetHeadPosition();
	while (pPos)
	{
		wmemset(tzMesg, 0x00, CIM_CMD_DESC_SIZE);
		pstEventCmd		= m_lstEvtCmds.GetNext(pPos);
		bstrRemoteCmd	= CComBSTR(pstEventCmd->cmd);
		swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"UnregisterCommandID::UnregisterCommandHandler [ cmd=%s ]", pstEventCmd->cmd);
		hResult = m_pICxClientApp->UnregisterCommandHandler(bstrRemoteCmd);
		m_pLogData->CheckResult(hResult, tzMesg);
		pstEventCmd->Free();
		::Free(pstEventCmd);
	}
	m_lstEvtCmds.RemoveAll();

	return (hResult == S_OK);
}

/*
 desc : 콜백 함수 등록 (Common or Others)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CE30GEM::RegisterCommonID()
{
	LONG i = 0;
	HRESULT hResult	= S_OK;
	CComBSTR bstrRemoteCmd, bstrRemoteCmdDesc;

	/* Register to Common Terminal Message Handler */
	hResult = m_pICxClientApp->RegisterTerminalMsgHandler(m_pICxEMAppCbk);
	if (FAILED(m_pLogData->CheckResult(hResult, L"RegisterCommonID::RegisterTerminalMsgHandler")))	return FALSE;

	/* 모든 알람의 상태 변경에 대한 이벤트 감지 등록 */
	hResult = m_pICxClientApp->RegisterAlarmChanged(-1, m_pICxEMAppCbk);
	if (FAILED(m_pLogData->CheckResult(hResult, L"RegisterCommonID::RegisterAlarmChanged")))	return FALSE;

	/* Register ICxEMAppCallback5 (Equipment Constant ID) callback handler */
	hResult	= m_pICxClientApp->RegisterECChanged(m_pICxEMAppCbk5);
	if (FAILED(m_pLogData->CheckResult(hResult, L"RegisterCommonID::RegisterECChanged")))	return FALSE;

	for (i=1; i<=m_lConnectionCount; i++)
	{
		/* Register to receive State Machine changes */
		hResult = m_pICxClientApp->RegisterStateMachineHandler(i, m_pICxEMAppCbk);
		if (m_pLogData->CheckResult(hResult, L"RegisterCommonID::RegisterStateMachineHandler"))	return FALSE;
		/* Register a message logger */
		hResult	= m_pICxClientApp->RegisterMsgLogger(i, m_pICxEMMsgCbk);
		if (FAILED(m_pLogData->CheckResult(hResult, L"RegisterCommonID::RegisterMsgLogger")))	return FALSE;
	}

	/* Register a recipe handler */
	hResult	= m_pICxClientApp->RegisterRecipeHandler(m_pICxEMAppCbk);
	if (FAILED(m_pLogData->CheckResult(hResult, L"RegisterCommonID::RegisterRecipeHandler")))	return FALSE;

	return TRUE;
}

/*
 desc : 콜백 함수 해제 (Common or Others)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CE30GEM::UnregisterCommonID()
{
	LONG i = 0;
	HRESULT hResult	= S_OK;

	/* Unregister the Global ICxEMAppCallback */
	hResult = m_pICxClientApp->UnregisterTerminalMsgHandler(m_pICxEMAppCbk);
	if (FAILED(m_pLogData->CheckResult(hResult, L"UnregisterCommonID::UnregisterTerminalMsgHandler")))	return FALSE;
	/* 모든 알람의 상태 변경에 대한 이벤트 감지 등록 */
	hResult = m_pICxClientApp->UnregisterAlarmChanged(-1, m_pICxEMAppCbk);
	if (FAILED(m_pLogData->CheckResult(hResult, L"UnregisterCommonID::RegisterAlarmChanged")))	return FALSE;
 
	/* Unregister ICxEMAppCallback5 (Equipment Constant ID) callback handler */
	hResult	= m_pICxClientApp->UnregisterECChanged(m_pICxEMAppCbk5);
	if (FAILED(m_pLogData->CheckResult(hResult, L"UnregisterCommonID::UnregisterECChanged")))	return FALSE;

	for (i=1; i<=m_lConnectionCount; i++)
	{
		/* Unregister the Global ICxEMAppCallback, for common*/
		hResult = m_pICxClientApp->UnregisterStateMachineHandler(i, m_pICxEMAppCbk);
		if (FAILED(m_pLogData->CheckResult(hResult, L"UnregisterCommonID::UnregisterStateMachineHandler")))	return FALSE;
		/* Register a message logger */
		hResult	= m_pICxClientApp->UnregisterMsgLogger(i, m_pICxEMMsgCbk);
		if (FAILED(m_pLogData->CheckResult(hResult, L"RegisterCommonID::UnregisterMsgLogger")))	return FALSE;
	}

	/* Un-register recipe handler */
	hResult	= m_pICxClientApp->UnregisterRecipeHandler(m_pICxEMAppCbk);
	if (FAILED(m_pLogData->CheckResult(hResult, L"UnregisterCommonID::UnregisterRecipeHandler")))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : SECS-II 메시지 등록 등록 (Message Handler & Message Sent)
 parm : lst_mesg	- [in]  SECS-II Message 정보가 저장된 리스트 포인터
 retn : TRUE or FALSE
*/
BOOL CE30GEM::RegisterMessageID(CAtlList <STG_TSMI> *lst_mesg)
{
	UINT32 i	= 0;
	STG_TSMI stData;

	for (; i<lst_mesg->GetCount(); i++)
	{
		/* 이벤트 등록 정보 1개 추출 */
		stData	= lst_mesg->GetAt(lst_mesg->FindIndex(i));
		/* 이벤트 등록 */
		if (!RegisterMessageID(stData.conn_id, stData.s_id, stData.f_id, stData.mesg_desc))
			return FALSE;
	}

	return TRUE;
}

/*
 desc : 콜백 함수 등록 (Message Handler & Message Sent)
 parm : conn_id	- [in]  연결된 장비 ID
		s_id	- [in]  Stream ID
		f_id	- [in]  Function ID
		mesg	- [in]  메시지 정보 (단지, 로그 출력 용)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::RegisterMessageID(LONG conn_id, UINT8 mesg_sid, UINT8 mesg_fid, PTCHAR mesg)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG i = 1, lMsgID	= 0x0000;
	HRESULT hResult		= S_OK;
	POSITION pPos		= NULL;
	LPM_REHI pstEvent	= NULL;

	/* 16진수 메시지 ID 얻기 */
	lMsgID	= SFtoMsgID(mesg_sid, mesg_fid); /* lMsgID = (s_id << 8 | f_id) */

	/* Part : RegisterMsgHandler */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"RegisterMsgEventID::RegisterMsgHandler (%s) "
										  L"(conn_id=%d, mesg_id=0x%04x)",
			   mesg, conn_id, lMsgID);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientApp->RegisterMsgHandler(conn_id, lMsgID, m_pICxEMAppCbk);
	if (FAILED(m_pLogData->CheckResult(hResult, tzMesg)))	return FALSE;
	/* Part : RegisterMsgSent */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"RegisterMsgEventID::RegisterMsgSent (%s) "
										  L"(conn_id=%d, mesg_id=0x%04x)",
			   mesg, conn_id, lMsgID);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientApp->RegisterMsgSent(conn_id, lMsgID, m_pICxEMMsgCbk);
	if (FAILED(m_pLogData->CheckResult(hResult, tzMesg)))	return FALSE;
	/* Part : RegisterMsgError */
	if (mesg_fid)	/* Function ID 값이 0 이 아닌 경우만 등록 가능 */
	{
		swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"RegisterMsgEventID::RegisterMsgError (%s) "
											  L"(conn_id=%d, mesg_id=0x%04x)",
				   mesg, conn_id, lMsgID);
		m_pLogData->PushLogs(tzMesg);
		hResult = m_pICxClientApp->RegisterMsgError(conn_id, lMsgID, m_pICxEMAppCbk);
		if (FAILED(m_pLogData->CheckResult(hResult, tzMesg)))	return FALSE;
	}
	/* 등록된 이벤트 저장 */
	pstEvent	= (LPM_REHI)::Alloc(sizeof(STM_REHI));
	ASSERT(pstEvent);
	pstEvent->conn_id	= conn_id;
	pstEvent->evt_id	= lMsgID;
	m_lstEvtMesg.AddTail(pstEvent);

	return (hResult == S_OK);
}

/*
 desc : 콜백 함수 해제 (Message Handler & Message Sent)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CE30GEM::UnregisterMessageID()
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG i = 1;
	HRESULT hResult				= S_OK;
	POSITION pPos				= NULL;
	LPM_REHI pstEventMsg		= NULL;

	/* Part : RegisterMsgSent */
	pPos	= m_lstEvtMesg.GetHeadPosition();
	while (pPos)
	{
		wmemset(tzMesg, 0x00, CIM_CMD_DESC_SIZE);
		pstEventMsg	= m_lstEvtMesg.GetNext(pPos);
		if (pstEventMsg)
		{
			/* Part : UnregisterMsgHandler */
			swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"UnregisterMessageID::UnregisterMsgHandler [ evt_id=0x%04x ]",
					   pstEventMsg->evt_id);
			hResult = m_pICxClientApp->UnregisterMsgHandler(pstEventMsg->conn_id, pstEventMsg->evt_id);
			m_pLogData->CheckResult(hResult, tzMesg);
			/* Part : UnregisterMsgHandler */
			swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"UnregisterMessageID::UnregisterMsgHandler [ evt_id=0x%04x ]",
					   pstEventMsg->evt_id);
			hResult = m_pICxClientApp->UnregisterMsgSent(pstEventMsg->conn_id, pstEventMsg->evt_id,
														 m_pICxEMMsgCbk);
			m_pLogData->CheckResult(hResult, tzMesg);
			/* Part : UnregisterMsgError */
			swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"UnregisterMessageID::UnregisterMsgError [ evt_id=0x%04x ]",
					   pstEventMsg->evt_id);
			hResult = m_pICxClientApp->UnregisterMsgError(pstEventMsg->conn_id, pstEventMsg->evt_id,
														  m_pICxEMAppCbk);
			m_pLogData->CheckResult(hResult, tzMesg);
			/* 메모리 해제 */
			::Free(pstEventMsg);
		}
	}
	m_lstEvtMesg.RemoveAll();

	return (hResult == S_OK);
}

/*
 desc : Update the GEM Processing State in CIMConnect. 
		Each equipment supplier should customzie the GEM Processing State Model
		to match the actual tool's processing states. This is just a  
		simple example with three states: IDLE, SETUP and EXECUTING. 
 parm : state	- [in]  동작 상태 (동작 상태 : 0 (Setup), 1 (Idle), 2 (executing))
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SetUpdateProcessState(E30_GPSV state)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	CComBSTR bstrStateName;
	CComBSTR bstrEventName;
	HRESULT hResult	= NULL;

	switch (state)
	{
	case E30_GPSV::en_setup		:
		bstrStateName	= CComBSTR(L"SETUP");
		bstrEventName	= CComBSTR(L"ProcessStateToSETUP");
		break;
	case E30_GPSV::en_idle		:
		bstrStateName	= CComBSTR(L"IDLE");
		bstrEventName	= CComBSTR(L"ProcessStateToIDLE");
		break;
	case E30_GPSV::en_execute	:
		bstrStateName	= CComBSTR(L"EXECUTING");
		bstrEventName	= CComBSTR(L"ProcessStateToEXECUTING");
		break;
	case E30_GPSV::en_complete	:
		bstrStateName	= CComBSTR(L"COMPLETE");
		bstrEventName	= CComBSTR(L"ProcessStateToCOMPLETE");
		break;
	default:	return FALSE;
	}

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SetUpdateProcessState::ProcessingStateChange (state (E30_GPSV)=%d)", state);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxClientApp->ProcessingStateChange((INT32)state, &bstrStateName, -1, &bstrEventName);
	m_pLogData->CheckResult(hResult, tzMesg);
	/* Update the Process State */
	m_pSharedData->SetProcessState(state);

	return (hResult == S_OK);
}

/*
 desc : Send a terminal service text message to the host.
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		term_id	- [in]  Terminal ID (0 or Later)
		mesg	- [in]  The text message
 retn : TRUE or FALSE
 참조 : This method requests that a terminal message to be sent to a connection
*/
BOOL CE30GEM::SendTerminalMessage(LONG conn_id, LONG term_id, PTCHAR mesg)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult		= NULL;
	CComBSTR bstrMesg	= mesg;

	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SendTerminalMessage::SendTerminalMsg (conn_id=%d, term_id =%d, mesg=%s)",
			   conn_id, term_id, mesg);
	m_pLogData->PushLogs(tzMesg);
    hResult = m_pICxClientApp->SendTerminalMsg(conn_id, term_id, bstrMesg);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Send a terminal service acknowledge event to the host. Send to host 1 by default.
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
 retn : TRUE or FALSE
 참조 : This method triggers an equipment well-known event
		Equipment events are momentary, not latched
		There is no method to reset events since they are a one-shot, non-latched event.
*/
BOOL CE30GEM::SendTerminalAcknowledge(LONG conn_id)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult		= NULL;
	CComBSTR bstrMesg	= L"MessageRecognition";

 	swprintf_s(tzMesg, CIM_LOG_MESG_SIZE,
			   L"SendTerminalAcknowledge::TriggerWellKnownEvent (conn_id=%d, value=MessageRecognition)",
			   conn_id);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientApp->TriggerWellKnownEvent(conn_id, bstrMesg);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Send S2,F17 to the host, to request the date and time. 
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
 retn : TRUE or FALSE
*/
BOOL CE30GEM::S2F17DateAndTimeRequest(LONG conn_id)
{
	HRESULT hResult;
	/* Note that SendMessage got renamed during the #import to __SendMessage */
	/* This is done by the "auto_rename" option */
	LONG TID = 0;
	/* The  "| 0x10000" is because there is no message body. */
	hResult = m_pICxClientApp->CxSendMessage(conn_id, SFtoMsgID(2, 17) | 0x10000 , NULL,
											 VARIANT_FALSE, VARIANT_TRUE, &TID, NULL);
	m_pLogData->CheckResult(hResult, L"S2F17DateAndTimeRequest::SendMessage");

	return (hResult == S_OK);
}

/*
 desc : Notify CIMConnect that an operator command has been issued.
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		name	- [in]  Name of the command
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID;1 or 2)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::OperatorCommand(LONG conn_id, PTCHAR name, LONG handle)
{
	CComSafeArray<BSTR> arrDataNames;
	ICxValueDispPtr pICxValueDisp;
	CComBSTR bRecipeName (name);

	arrDataNames.Add(CComBSTR("OperatorCommand"));
	pICxValueDisp.CreateInstance(CLSID_CxValueObject);
	pICxValueDisp->AppendValueAscii(handle, bRecipeName);
	return SendCollectionEventWithData(conn_id, arrDataNames, pICxValueDisp,
									   L"OperatorCommandIssued", handle);
}

/*
 desc : Variable ID에 해당되는 Value Type (E30_GPVT) 값 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		var_id	- [in]  Variable ID
		type	- [out] Value Type
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetValueType(LONG conn_id, LONG var_id, E30_GPVT &type)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG lCount = 0, lEventID;
	HRESULT hResult		= S_OK;
	VARIANT_BOOL varBoPrivate, varBoPersistent;
	VarType varType;
	ValueType valType;
	CComBSTR bstrName, bstrDesc, bstrUnit;
	ICxValueDispPtr pICxValueCur (CLSID_CxValueObject);
	ICxValueDispPtr pICxValueMin (CLSID_CxValueObject);
	ICxValueDispPtr pICxValueMax (CLSID_CxValueObject);
	ICxValueDispPtr pICxValueDef (CLSID_CxValueObject);

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetValueType::GetVarInfo");
//	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientTool->GetVarInfo(conn_id, &var_id, &bstrName, pICxValueCur, &varType,
										   &valType, &lEventID, &bstrDesc, &bstrUnit,
										   pICxValueMin, pICxValueMax, pICxValueDef,
										   &varBoPrivate, &varBoPersistent);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	type	= E30_GPVT(valType);
	return (hResult == S_OK);
}

/*
 desc : Variable ID에 해당되는 Variable Type (E30_GVTC) 값 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		var_id	- [in]  Variable ID
		type	- [out] Value Type
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetVarType(LONG conn_id, LONG var_id, E30_GVTC &type)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG lCount = 0, lEventID;
	HRESULT hResult		= S_OK;
	VARIANT_BOOL varBoPrivate, varBoPersistent;
	VarType varType;
	ValueType valType;
	CComBSTR bstrName, bstrDesc, bstrUnit;
	ICxValueDispPtr pICxValueCur (CLSID_CxValueObject);
	ICxValueDispPtr pICxValueMin (CLSID_CxValueObject);
	ICxValueDispPtr pICxValueMax (CLSID_CxValueObject);
	ICxValueDispPtr pICxValueDef (CLSID_CxValueObject);

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetVarType::GetVarInfo");
//	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientTool->GetVarInfo(conn_id, &var_id, &bstrName, pICxValueCur, &varType,
										   &valType, &lEventID, &bstrDesc, &bstrUnit,
										   pICxValueMin, pICxValueMax, pICxValueDef,
										   &varBoPrivate, &varBoPersistent);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	type	= E30_GVTC(varType);
	return (hResult == S_OK);
}

/*
 desc : 현재 Variable의 Value 값 얻기
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우 0 값
		var_id	- [in]  Variable ID와 매핑되는 Name
		value	- [out] Value 값이 저장되는 버퍼 포인터
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID; 1 or 2)
		index	- [in]	일반적으로 0 값이지만, 리스트의 경우 Slot ID (Wafer ID; 1 ~ Max)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetValue(LONG conn_id, LONG var_id, UNG_CVVT &value, LONG handle, LONG index)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;
	E30_GPVT enType;
	ICxValueDispPtr pCxValueDisp(CLSID_CxValueObject);

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetValue::GetVarType");
	if (!GetValueType(conn_id, var_id, enType))		return FALSE;
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return GetValue(conn_id, var_id, enType, &value, handle, index);
}

/*
 desc : 현재 Variable의 Value 값 얻기
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우 0 값
		var_id	- [in]  Variable ID
		val_type- [in]  Value Type
		value	- [out] Value 값이 저장되는 버퍼 포인터
		size	- [in]  'value' 버퍼 크기
		handle	- [in]  일반적으로 0 값이지만, 리스트의 경우 Carrier ID (FOUP ID; 1 or 2)
		index	- [in]	일반적으로 0 값이지만, 리스트의 경우 Slot ID (Wafer ID; 1 ~ Max)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetValue(LONG conn_id, LONG var_id, E30_GPVT type, LNG_CVVT value, LONG handle, LONG index)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]= {NULL};
	LONG lCount		= 0;
	HRESULT hResult	= S_OK;
	CComBSTR bstrName, bstrAscii;
	VariableResults varResults;
	ICxValueDispPtr pCxValueDisp (CLSID_CxValueObject);
	CComVariant vValue;

	if (type == E30_GPVT::en_w || type == E30_GPVT::en_list ||
		type == E30_GPVT::en_jis8 || type == E30_GPVT::en_array)
	{
		m_pLogData->PushLogs(L"Data type not currently supported", ENG_GALC::en_warning);
		return FALSE;
	}

	/* Value Object 얻기 */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetValue::GetValue ID (conn_id=%d, var_id=%d, val_type=%d)", conn_id, var_id, type);
#if 0
	hResult	= m_pICxClientApp->GetValue(conn_id, &var_id, &bstrName, pCxValueDisp, &varResults);
#else
	hResult = m_pICxClientApp->GetValueToByteBuffer(conn_id, &var_id, &bstrName, &vValue, &varResults);
#endif
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	if (varResults != VariableResults::vrSUCCESS)
	{
		switch (varResults)
		{
		case VariableResults::vrFAILURE		:	m_pLogData->PushLogs(L"Operation failed", ENG_GALC::en_warning);	break;
		case VariableResults::vrLIMITCROSSED:	m_pLogData->PushLogs(L"Operation succeeded and a limit defined by the host was crossed (SetValue only)", ENG_GALC::en_warning);	break;
		case VariableResults::vrINVALIDID	:	m_pLogData->PushLogs(L"variable does not exist", ENG_GALC::en_warning);	break;
		case VariableResults::vrINVALIDTYPE	:	m_pLogData->PushLogs(L"VarType does not match the specified variable", ENG_GALC::en_warning);	break;
		case VariableResults::vrNOCHANGE	:	m_pLogData->PushLogs(L"operation succeeded with no change in value (SetValue only)", ENG_GALC::en_warning);	break;
		}
		return FALSE;
	}

	/* 저장된 값 임시 버퍼에 가져다 놓기 */
	hResult	= pCxValueDisp->RestoreFromByteStream(0, 0, vValue);
	if (S_OK != m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* 현재 Handle에 해당되는 등록된 값의 개수가 있는지 여부 */
	hResult	= pCxValueDisp->ItemCount(handle, &lCount);
	if (S_OK != m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Value Type 에 따라 요청 */
	if (lCount)
	{
		switch (type)
		{
			case E30_GPVT::en_i1		:	hResult	= pCxValueDisp->GetValueI1(handle, index, (PINT16)&value->i8_value);			break;
			case E30_GPVT::en_i2		:	hResult	= pCxValueDisp->GetValueI2(handle, index, &value->i16_value);					break;
			case E30_GPVT::en_i4		:	hResult	= pCxValueDisp->GetValueI4(handle, index, (PLONG)&value->i32_value);			break;
			case E30_GPVT::en_i8		:	hResult	= pCxValueDisp->GetValueI8(handle, index, (PLONG)&value->i64_value);			break;
			case E30_GPVT::en_u1		:	hResult	= pCxValueDisp->GetValueU1(handle, index, (PUINT8)&value->u8_value);			break;
			case E30_GPVT::en_u2		:	hResult	= pCxValueDisp->GetValueU2(handle, index, (PLONG)&value->u16_value);			break;
			case E30_GPVT::en_u4		:	hResult	= pCxValueDisp->GetValueU4(handle, index, (PLONG)&value->u32_value);			break;
			case E30_GPVT::en_u8		:	hResult	= pCxValueDisp->GetValueU8(handle, index, (PLONG)&value->u64_value);			break;
			case E30_GPVT::en_f4		:	hResult	= pCxValueDisp->GetValueF4(handle, index, (PFLOAT)&value->f_value);				break;
			case E30_GPVT::en_f8		:	hResult	= pCxValueDisp->GetValueF8(handle, index, (DOUBLE*)&value->d_value);			break;
			case E30_GPVT::en_b			:	hResult	= pCxValueDisp->GetValueBoolean(handle, index, (VARIANT_BOOL*)&value->b_value);
											if (value->b_value == USHORT_MAX)	value->b_value = TRUE;	break;
			case E30_GPVT::en_bi		:	hResult	= pCxValueDisp->GetValueBinary(handle, index, (PUINT8)&value->bi_value);		break;
	#if 1
			case E30_GPVT::en_a			:	hResult	= pCxValueDisp->GetValueAscii(handle, index, &bstrAscii);
											swprintf_s(value->s_value, CIM_VAL_ASCII_SIZE, L"%s", (LPCTSTR)_bstr_t(bstrAscii));		break;
	#else
			case E30_GPVT::en_a			:	if (!GetVariableAscii(conn_id, var_id, value->s_value, CIM_VAL_ASCII_SIZE, handle, index))	hResult = S_FALSE;	break;
	#endif
		}
		swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetValue::GetValue ID (conn_id=%d, var_id=%d, val_type=%d)", conn_id, var_id, type);
		m_pLogData->CheckResult(hResult, tzMesg);
	}
	return (hResult == S_OK);
}

/*
 desc : 현재 Variable의 Value 값 얻기
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		var_id	- [in]  Variable ID
		value	- [out] Value 값이 저장되는 버퍼 포인터
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetValueI1(LONG conn_id, LONG var_id, PINT8 value, LONG handle, LONG index)
{
	UNG_CVVT unValue	= {NULL};
	if (!GetValue(conn_id, var_id, E30_GPVT::en_i1, &unValue, handle, index))	return FALSE;
	*value	= unValue.i8_value;
	return TRUE;
}
BOOL CE30GEM::GetValueI2(LONG conn_id, LONG var_id, PINT16 value, LONG handle, LONG index)
{
	UNG_CVVT unValue	= {NULL};
	if (!GetValue(conn_id, var_id, E30_GPVT::en_i2, &unValue, handle, index))	return FALSE;
	*value	= unValue.i16_value;
	return TRUE;
}
BOOL CE30GEM::GetValueI4(LONG conn_id, LONG var_id, PINT32 value, LONG handle, LONG index)
{
	UNG_CVVT unValue	= {NULL};
	if (!GetValue(conn_id, var_id, E30_GPVT::en_i4, &unValue, handle, index))	return FALSE;
	*value	= unValue.i32_value;
	return TRUE;
}
BOOL CE30GEM::GetValueI8(LONG conn_id, LONG var_id, PINT64 value, LONG handle, LONG index)
{
	UNG_CVVT unValue	= {NULL};
	if (!GetValue(conn_id, var_id, E30_GPVT::en_i8, &unValue, handle, index))	return FALSE;
	*value	= unValue.i64_value;
	return TRUE;
}
BOOL CE30GEM::GetValueU1(LONG conn_id, LONG var_id, PUINT8 value, LONG handle, LONG index)
{
	UNG_CVVT unValue	= {NULL};
	if (!GetValue(conn_id, var_id, E30_GPVT::en_u1, &unValue, handle, index))	return FALSE;
	*value	= unValue.u8_value;
	return TRUE;
}
BOOL CE30GEM::GetValueU2(LONG conn_id, LONG var_id, PUINT16 value, LONG handle, LONG index)
{
	UNG_CVVT unValue	= {NULL};
	if (!GetValue(conn_id, var_id, E30_GPVT::en_u2, &unValue, handle, index))	return FALSE;
	*value	= unValue.u16_value;
	return TRUE;
}
BOOL CE30GEM::GetValueU4(LONG conn_id, LONG var_id, PUINT32 value, LONG handle, LONG index)
{
	UNG_CVVT unValue	= {NULL};
	if (!GetValue(conn_id, var_id, E30_GPVT::en_u4, &unValue, handle, index))	return FALSE;
	*value	= unValue.u32_value;
	return TRUE;
}
BOOL CE30GEM::GetValueU8(LONG conn_id, LONG var_id, PUINT64 value, LONG handle, LONG index)
{
	UNG_CVVT unValue	= {NULL};
	if (!GetValue(conn_id, var_id, E30_GPVT::en_u8, &unValue, handle, index))	return FALSE;
	*value	= unValue.u64_value;
	return TRUE;
}
BOOL CE30GEM::GetValueF4(LONG conn_id, LONG var_id, PFLOAT value, LONG handle, LONG index)
{
	UNG_CVVT unValue	= {NULL};
	if (!GetValue(conn_id, var_id, E30_GPVT::en_f4, &unValue, handle, index))	return FALSE;
	*value	= unValue.f_value;
	return TRUE;
}
BOOL CE30GEM::GetValueF8(LONG conn_id, LONG var_id, DOUBLE *value, LONG handle, LONG index)
{
	UNG_CVVT unValue	= {NULL};
	if (!GetValue(conn_id, var_id, E30_GPVT::en_f8, &unValue, handle, index))	return FALSE;
	*value	= unValue.d_value;
	return TRUE;
}
BOOL CE30GEM::GetValueBo(LONG conn_id, LONG var_id, BOOL *value, LONG handle, LONG index)
{
	UNG_CVVT unValue	= {NULL};
	if (!GetValue(conn_id, var_id, E30_GPVT::en_b, &unValue, handle, index))	return FALSE;
	*value	= unValue.b_value;
	return TRUE;
}
BOOL CE30GEM::GetValueBi(LONG conn_id, LONG var_id, PUINT8 value, LONG handle, LONG index)
{
	UNG_CVVT unValue	= {NULL};
	if (!GetValue(conn_id, var_id, E30_GPVT::en_bi, &unValue, handle, index))	return FALSE;
	*value	= unValue.bi_value;
	return TRUE;
}

/*
 desc : 현재 통신 상태가 Enable인지 여부
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
BOOL CE30GEM::IsCommEnabled(LONG conn_id, BOOL logs)
{
	/* Get the comm enable switch */
	UINT32 u32Value	= 0;
	if (!GetValueU4(conn_id, 2036 /* CommState */, &u32Value))	return FALSE;
	if (0x00000100 != (0x00000100 & u32Value/*lCommEnabled*/))
	{
		if (logs)	m_pLogData->PushLogs(L"IsCommEnabled::The communication is not enabled", ENG_GALC::en_warning);
		return FALSE;
	}
	return TRUE;
}

/*
 desc : 현재 통신 상태가 Communicating인지 여부
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
BOOL CE30GEM::IsCommCommunicating(LONG conn_id, BOOL logs)
{
	/* Get the comm enable switch */
	UINT32 u32Value	= 0;
	if (!GetValueU4(conn_id, 2036 /* CommState */,  &u32Value))	return FALSE;
	if (0x00000100 != (0x00000100 & u32Value))
	{
		if (logs)	m_pLogData->PushLogs(L"IsCommEnabled::The communication is not communicating", ENG_GALC::en_warning);
		return FALSE;
	}
	return TRUE;
}

/*
 desc : 현재 제어 상태가 Online 모드인지 여부
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
BOOL CE30GEM::IsControlOnline(LONG conn_id, BOOL logs)
{
	/* Get the control state */
	UINT32 u32Value	= 0;
	if (!GetValueU4(conn_id, 2036 /* CommState */, &u32Value))	return FALSE;
	if (0x00000100 != (0x00000100 & u32Value))
	{
		if (logs)	m_pLogData->PushLogs(L"IsCommEnabled::The control state is not online", ENG_GALC::en_warning);
		return FALSE;
	}
	return TRUE;
}

/*
 desc : 현재 제어 상태가 Remote 모드인지 여부
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
BOOL CE30GEM::IsControlRemote(LONG conn_id, BOOL logs)
{
	/* Get the comm enable switch */
	UINT32 u32Value;
	if (!GetValueU4(conn_id, 2036 /* CommState */, &u32Value))	return FALSE;
	/* 4 : Local, 5 : Remote */
	if (5 != u32Value)
	{
		if (logs)	m_pLogData->PushLogs(L"IsCommEnabled::The control state is not remote", ENG_GALC::en_warning);
		return FALSE;
	}
	return TRUE;
}

/*
 desc : CIM Connection Enable or Disable
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		enable	- [in]  TRUE or FALSE
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SetCommunication(LONG conn_id, BOOL enable)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;

	/* 현재 통신 상태가 Enable인지 여부 */
	if (enable == IsCommEnabled(conn_id, FALSE))	return TRUE;

	/* 연결 장비 : Disable or Enable 처리 */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetCommunication::EnableComm or DisableComm : conn_id=%d, enable=%d",
			   conn_id, enable);
	m_pLogData->PushLogs(tzMesg);
	if (enable)	hResult	= m_pICxClientApp->EnableComm(conn_id);
	else		hResult	= m_pICxClientApp->DisableComm(conn_id);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Change the GEM Control state to ONLINE or OFFLINE
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		state	- [in]  if true, go ONLINE. if false, go OFFLINE
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SetControlOnline(LONG conn_id, BOOL state)
{	 
	TCHAR tzState[2][32]= { L"GoOffline", L"GoOnline" };
	HRESULT hResult;

	/* 현재 통신 상태가 Enable인지 여부 */
	if (!IsCommEnabled(conn_id))	return FALSE;
	/* Get the online state */
	UINT8 u8Value = 0;
	if (!GetValueU1(conn_id, 2028 /* CONTROLSTATE */, &u8Value))	return FALSE;
	/* 현재 Host 내지 Equipment가 Online 상태인지 / Offline 상태인지 확인 */
	if ((u8Value > 3 && state) || (u8Value < 4 && !state))
	{
		m_pLogData->PushLogs(L"SetControlOnline::It is the same the previously set command", ENG_GALC::en_warning);
		return FALSE;
	}

	m_pLogData->PushLogs(L"SetControlOnline", tzState[state]);
	if (state)	hResult	= m_pICxClientApp->GoOnline(conn_id);
	else		hResult	= m_pICxClientApp->GoOffline(conn_id);

	if (hResult != S_OK)
    {
		if (state)	m_pLogData->CheckResult(hResult, L"SetControlOnline::GoOnline");
		else		m_pLogData->CheckResult(hResult, L"SetControlOnline::GoOffline");
    }

	return (hResult == S_OK);
}

/*
 desc : Change the GEM Control State to REMOTE or LOCAL
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		state	- [in]  if true, go REMOTE. if false, go LOCAL
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SetControlRemote(LONG conn_id, BOOL state)
{
	TCHAR tzState[2][32]	= { L"GoLocal", L"GoRemote" };
	HRESULT hResult;

	/* 현재 통신 상태가 Enable인지 여부 */
	if (!IsCommEnabled(conn_id))	return FALSE;
	/* Get the online state */
	UINT8 u8Value = 0;
	if (!GetValueU1(conn_id, 2028 /* CONTROLSTATE */,  &u8Value))	return FALSE;
	/* 현재 Host가 접속되지 않은 상태라면, 아예 리턴 False 처리 */
	if (u8Value < 4)
	{
		switch (u8Value)
		{
		case 1	:	m_pLogData->PushLogs(L"SetControlRemote::The equipment is offline",			ENG_GALC::en_warning);	break;
		case 2	:	m_pLogData->PushLogs(L"SetControlRemote::The equipment is trying to Online",ENG_GALC::en_warning);	break;
		case 3	:	m_pLogData->PushLogs(L"SetControlRemote::The host is offline",				ENG_GALC::en_warning);	break;
		default	:	m_pLogData->PushLogs(L"SetControlRemote::Unknown Error",					ENG_GALC::en_warning);	break;
		}
		return FALSE;
	}
	/* 동일한 제어를 하게 되면, 하지 못하도록 막기 */
	if ((u8Value == 4 && !state) || (u8Value == 5 && state))
	{
		m_pLogData->PushLogs(L"SetControlStateRemote::It is the same the previously set command", ENG_GALC::en_warning);
		return FALSE;
	}

	/* 제어 */
	m_pLogData->PushLogs(L"SetControlRemote", tzState[state]);
	if (state)	hResult	= m_pICxClientApp->GoRemote(conn_id);
	else		hResult	= m_pICxClientApp->GoLocal(conn_id);
	/* 로그 출력 */
	if (state)	m_pLogData->CheckResult(hResult, L"SetControlRemote::GoRemote");
	else		m_pLogData->CheckResult(hResult, L"SetControlRemote::GoLocal");

	return (hResult == S_OK);
}

/*
 desc : Spooling State 제어
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		enable	- [in]  Enable (TRUE) or Disable (FALSE)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SetSpoolPurge(LONG conn_id)
{
	HRESULT hResult	= S_OK;
	if (conn_id < 1)	return -1L;

	/* 현재 통신 상태가 Enable인지 여부 */
	if (!IsCommEnabled(conn_id))	return FALSE;
	/* Run the Purge */
	hResult	= m_pICxHostEmulator->PurgeSpool(conn_id);
	m_pLogData->CheckResult(hResult, L"SetPurgeSpool::Set Purge");

	return (hResult == S_OK);
}

/*
 desc : Spooling State 제어
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		enable	- [in]  Enable (TRUE) or Disable (FALSE)
 retn : TRUE or FALSE
 note : Be aware that a connection may not support spooling
*/
BOOL CE30GEM::SetSpoolState(LONG conn_id, BOOL enable)
{
	HRESULT hResult	= S_OK;
	if (conn_id < 1)	return -1L;

	/* 현재 통신 상태가 Enable인지 여부 */
	if (!IsCommEnabled(conn_id))	return FALSE;
	/* 연결 장비 : Disable or Enable 처리 */
	if (enable)	hResult	= m_pICxClientApp->EnableSpooling(conn_id);
	else		hResult	= m_pICxClientApp->DisableSpooling(conn_id);

	m_pLogData->CheckResult(hResult, L"SetStateSpool::Enable/Disable Spooling");

	return TRUE;
}

/*
 desc : Spooling Overwrite 여부
 parm : conn_id		- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		overwrite	- [in]  Overwrite (TRUE) or Not overwrite (FALSE)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SetSpoolOverwrite(LONG conn_id, BOOL overwrite)
{
	HRESULT hResult;
	if (conn_id < 1)	return FALSE;

	/* 현재 통신 상태가 Enable인지 여부 */
	if (!IsCommEnabled(conn_id))	return FALSE;
	/* Run the SpoolOverwriting */
	hResult	= m_pICxClientApp->SpoolingOverwrite(conn_id, overwrite);
	if (FAILED(hResult))	m_pLogData->CheckResult(hResult, L"SetOverwriteSpool::SpoolingOverwrite");

	return (hResult == S_OK);
}

/*
 desc : Enables or Disables alarm reporting for a host for the specified alarms.
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		alarm_id- [in]  Connection Variables or Alarms의 Section에 나열된 variables identification number
		state	- [in]  TRUE (ENABLE) or FALSE (DISABLE)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SetAlarmsEnabled(LONG conn_id, LONG alarm_id, BOOL state)
{
	TCHAR tzState[2][16]= { L"Disabled", L"Enabled" };
	LONG lCtrlRemote	= 0;
	HRESULT hResult;

	/* 현재 통신 상태가 Enable인지 여부 */
	if (!IsCommEnabled(conn_id))	return FALSE;
	/* 제어 */
	m_pLogData->PushLogs(L"SetAlarmsEnabled", tzState[state]);
	if (state)	hResult	= m_pICxHostEmulator->EnableAlarm(conn_id, alarm_id);
	else		hResult	= m_pICxHostEmulator->DisableAlarm(conn_id, alarm_id);
	/* 로그 출력 */
	if (state)	m_pLogData->CheckResult(hResult, L"SetAlarmsEnabled::Enabled");
	else		m_pLogData->CheckResult(hResult, L"SetAlarmsEnabled::Disabled");

	return (hResult == S_OK);
}

/*
 desc : 현재 등록 (발생)된 알람 ID 값들 반환
 parm : id		- [out] 알람 리스트가 저장된 CAtlList <LONG> : 알람 ID
		state	- [out] 알람 리스트가 저장된 CAtlList <UINT8>: 알람 상태 (0 or 1)
		alarm	- [in]  알람이 발생된 정보만 반환 여부 (FLALSE:  모두, TRUE: 알람이 발생된 것만)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetAlarmList(CAtlList <LONG> &ids, CAtlList <LONG> &states, BOOL alarm)
{
	LONG i, lCount	= 0;
	LONG *pIDs		= NULL, *pStates = NULL;
	HRESULT hResult	= S_OK;
	VARIANT vtIDs, vtStates, vtTexts;
	CString strStates;
	CComSafeArray <LONG> saIDs, saStates;

	/* Initializes a variant */
	VariantInit(&vtIDs);
	VariantInit(&vtStates);
	VariantInit(&vtTexts);

	/* 현재 CIMConnect 내부에 발생된 알람 리스트 가져오기 */
	hResult	= m_pICxClientTool->ListAlarmsStates(&vtIDs, &vtStates, &vtTexts);
	if (m_pLogData->CheckResult(hResult, L"ClearAllAlarms::ListAlarmsStates"))	return FALSE;
	/* Get access to the list of IDs, States, Texts */
	if (SUCCEEDED(hResult))	hResult = saIDs.Attach(vtIDs.parray);
	if (SUCCEEDED(hResult))	hResult = saStates.Attach(vtStates.parray);
	if (m_pLogData->CheckResult(hResult, L"ClearAllAlarms::SafeArrary.Attach"))	return FALSE;

	/* 현재 발생(?)된 알람의 개수 구하기 */
	if ((lCount = GetSafeArrayCount(saIDs, 1)) > 0)
	{
		m_pLogData->PushLogs(L"The alarm was collected");
		/* get the id arrary */
		hResult	= SafeArrayLock(saIDs);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetAlarmList (AlarmIDs)"))
		{
			hResult	= SafeArrayAccessData(saIDs, (PVOID*)&pIDs);
			if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetAlarmList (AlarmIDs)"))
				hResult	= SafeArrayUnaccessData(saIDs);
			hResult	= SafeArrayUnlock(saIDs);
		}
		/* get the state array */
		hResult	= SafeArrayLock(saStates);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetAlarmList (AlarmStates)"))
		{
			hResult	= SafeArrayAccessData(saStates, (PVOID*)&pStates);
			if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetAlarmList (AlarmStates)"))
				hResult	= SafeArrayUnaccessData(saStates);
			hResult	= SafeArrayUnlock(saStates);
		}

		/* Alarm ID와 State 등록  */
		for (i=0;SUCCEEDED(hResult) && i<lCount;i++)
		{
			/* 알람 발생 여부에 따라 */
			if (alarm && 0 != pStates[i])
			{
				ids.AddTail(pIDs[i]);
				states.AddTail(pStates[i]);
			}
		}
	}

	/* SafeArray 해제 */
	saIDs.Detach();
	saStates.Detach();

	return TRUE;
}

/*
 desc : 개별 알람 상태 값 반환
 parm : id		- [in]  알람 ID
		state	- [out] 알람 상태 반환 (1 : Set, 0 : Clear)
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetAlarmState(LONG id, LONG &state, BOOL logs)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;
	CComBSTR bstrName;

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetAlarmState::GetAlarmState (alarm_id = %d)", id);
	if (logs)	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientApp->GetAlarmState(&id, &bstrName, &state);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : 현재 CIMConnect 내부에 발생된 알람 모두 해제
 parm : None
 retn : TRUE or FALSE
*/
BOOL CE30GEM::AlarmClearAll()
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG i, lCount;
	LONG *pIDs = NULL, *pStates = NULL;
	_bstr_t bstrStates;
	CComBSTR bstrEmpty;
	HRESULT hResult	= S_OK;
	VARIANT vtIDs, vtStates, vtTexts;
	CComSafeArray <LONG> saIDs, saStates;
	CComSafeArray <BSTR> saTexts;

	/* Initializes a variant */
	VariantInit(&vtIDs);
	VariantInit(&vtStates);
	VariantInit(&vtTexts);

	/* 현재 CIMConnect 내부에 발생된 알람 리스트 가져오기 */
	hResult	= m_pICxClientTool->ListAlarmsStates(&vtIDs, &vtStates, &vtTexts);
	if (FAILED(hResult))
	{
		m_pLogData->CheckResult(hResult, L"ClearAllAlarms::ListAlarmsStates");
		return FALSE;
	}
	/* Get access to the list of IDs, States, Texts */
	if (SUCCEEDED(hResult))	hResult = saIDs.Attach(vtIDs.parray);
	if (SUCCEEDED(hResult))	hResult = saStates.Attach(vtStates.parray);
	if (SUCCEEDED(hResult))	hResult = saTexts.Attach(vtTexts.parray);
	if (FAILED(hResult))
	{
		m_pLogData->CheckResult(hResult, L"ClearAllAlarms::SafeArrary.Attach");
		return FALSE;
	}

	/* 현재 발생(?)된 알람의 개수 구하기 */
	if ((lCount = GetSafeArrayCount(saIDs, 1)) > 0)
	{
		m_pLogData->PushLogs(L"The alarm was forcibly cleared");
		/* get the id arrary */
		hResult	= SafeArrayLock(saIDs);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on ClearAllAlarms (AlarmIDs)"))
		{
			hResult	= SafeArrayAccessData(saIDs, (PVOID*)&pIDs);
			if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on ClearAllAlarms (AlarmIDs)"))
				hResult	= SafeArrayUnaccessData(saIDs);
			hResult	= SafeArrayUnlock(saIDs);
		}
		/* get the state array */
		hResult	= SafeArrayLock(saStates);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on ClearAllAlarms (AlarmStates)"))
		{
			hResult	= SafeArrayAccessData(saStates, (PVOID*)&pStates);
			if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on ClearAllAlarms (AlarmStates)"))
				hResult	= SafeArrayUnaccessData(saStates);
			hResult	= SafeArrayUnlock(saStates);
		}
#if 0
		/* Get the text array */
		hResult	= SafeArrayLock(saTexts);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on ClearAllAlarms (AlarmTexts)"))
		{
			hResult	= SafeArrayAccessData(saTexts, (PVOID*)&pTexts);
			if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on ClearAllAlarms (AlarmTexts)"))
				hResult	= SafeArrayUnaccessData(saTexts);
			hResult	= SafeArrayUnlock(saTexts);
		}
#endif
		/* 값 가져오기  */
		for (i=0; SUCCEEDED(hResult) && i<lCount; i++)
		{
			/* 알람 이벤트 해제 */
			if (pStates[i])
			{
				bstrStates	= _bstr_t(pStates[i]);
				hResult	= m_pICxClientApp->ClearAlarm(&pIDs[i], &bstrEmpty);
				if (SUCCEEDED(hResult))	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"Succeeded to clear the alarm_id [%d][%s] on ClearAlarm", pIDs[i], (LPCTSTR)bstrStates);
				else					swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"Failed to clear the alarm_id [%d][%s] on ClearAlarm", pIDs[i], (LPCTSTR)bstrStates);
				m_pLogData->PushLogs(tzMesg);
				::SysFreeString(bstrStates);
			}
		}
	}

	/* SafeArray 해제 */
	saIDs.Detach();
	saStates.Detach();
	saTexts.Detach();

	return TRUE;
}

/*
 desc : Change the state of an alarm to CLEAR
 parm : var_id	- [in]  Alarm ID
 retn : TRUE or FALSE
*/
BOOL CE30GEM::AlarmClear(LONG var_id)
{
	HRESULT hResult;
	CComBSTR bstrAlarmName;

	m_pLogData->PushLogs(L"AlarmCLEAR::ClearAlarm", var_id);
	hResult = m_pICxClientApp->ClearAlarm(&var_id, &bstrAlarmName);
	m_pLogData->CheckResult(hResult, L"AlarmCLEAR::ClearAlarm");

	return (hResult == S_OK);
}

/*
 desc : CIMConnect 내부로 임의의 알람 발생
 parm : var_id	- [in]  Alarm ID 즉, EPJ Variables ID와 동일 함
 retn : TRUE or FALSE
*/
BOOL CE30GEM::AlarmSet(LONG var_id)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG lState					= 0;
	CComBSTR bstrEmpty;
	HRESULT hResult	= S_OK;

	/* 현재 해제하고자 하는 알람 상태 값 반환 */
	hResult	= m_pICxClientApp->GetAlarmState(&var_id, &bstrEmpty, &lState);
	if (FAILED(hResult))
	{
		m_pLogData->CheckResult(hResult, L"ClearAlarm::GetAlarmState");
		return FALSE;
	}
	/* 알람이 발생된 상태의 경우 */
	if (lState)
	{
		swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"Alarm setting failed. The alarm is triggered [var_id=%d]", var_id);
		m_pLogData->PushLogs(tzMesg);
		return FALSE;
	}

	hResult	= m_pICxClientApp->SetAlarm(&var_id, &bstrEmpty);
	if (SUCCEEDED(hResult))	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"Succeeded to set the alarm_id [%d] on SetAlarm", var_id);
	else					swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"Failed to set the alarm_id [%d] on SetAlarm", var_id);
	m_pLogData->PushLogs(tzMesg);

	return TRUE;
}

/*
 desc : Change an alarm to the SET (detected) state.
 retn : var_id	- [in]  Alarm ID
		text	- [in]  Text about the alarm. If empty, the default text for this alarm is passed to the host.
 retn : TRUE or FALSE
*/
BOOL CE30GEM::AlarmSet(LONG var_id, PTCHAR text)
{
	HRESULT hResult;
	CComBSTR bstrAlarmName;
	CComBSTR bstrText (text);

	if (text && wcslen(text))
	{
		m_pLogData->PushLogs(L"AlarmSET::SetAlarmAndText", var_id);
		hResult = m_pICxClientApp->SetAlarmAndText(var_id, bstrAlarmName, bstrText);
		m_pLogData->CheckResult(hResult, L"AlarmSET::SetAlarmAndText");
	}
	else
	{
		m_pLogData->PushLogs(L"AlarmSET::SetAlarm", var_id);
		hResult = m_pICxClientApp->SetAlarm(&var_id, &bstrAlarmName);
		m_pLogData->CheckResult(hResult, L"AlarmSET::SetAlarm");
	}

	return (hResult == S_OK);
}

/*
 desc : ICxValueDisp의 AppendValue 확장
 parm : disp	- [in]  ICxValueDisp
		value	- [in]  값 저장된 포인터 버퍼
		type	- [in]  'value' type (U1, U2, U4, Bo, I1, I3, ...)
 retn : TRUE or FALSE
 note : 단, list, jis_8_string 등은 제외
*/
BOOL CE30GEM::SetDispAppendValue(ICxValueDisp *disp, LNG_CVVT value, E30_GPVT type)
{
	HRESULT hResult		= S_OK;
	CComBSTR bstrValue(value->s_value);

	/* Value 등록 */
	switch (type)
	{
	case E30_GPVT::en_i1		: hResult = disp->AppendValueI1(0, value->i8_value);					break;
	case E30_GPVT::en_i2		: hResult = disp->AppendValueI2(0, value->i16_value);					break;
	case E30_GPVT::en_i4		: hResult = disp->AppendValueI4(0, value->i32_value);					break;
	case E30_GPVT::en_i8		: hResult = disp->AppendValueI8(0, INT32(value->i64_value));			break;

	case E30_GPVT::en_u1		: hResult = disp->AppendValueU1(0, value->u8_value);					break;
	case E30_GPVT::en_u2	: hResult = disp->AppendValueU2(0, INT16(value->u16_value));			break;
	case E30_GPVT::en_u4	: hResult = disp->AppendValueU4(0, INT32(value->u32_value));			break;
	case E30_GPVT::en_u8	: hResult = disp->AppendValueU8(0, INT32(value->u64_value));			break;

	case E30_GPVT::en_f4				: hResult = disp->AppendValueF4(0, FLOAT(value->f_value));				break;
	case E30_GPVT::en_f8				: hResult = disp->AppendValueF8(0, DOUBLE(value->d_value));				break;

	case E30_GPVT::en_b			: hResult = disp->AppendValueBoolean(0, VARIANT_BOOL(value->b_value));	break;

	case E30_GPVT::en_bi				: hResult = disp->AppendValueBinary(0, UINT8(value->bi_value));			break;
	case E30_GPVT::en_a				: hResult = disp->AppendValueAscii(0, bstrValue);						break;
	}

	return TRUE;
}

/*
 desc : Event 발생 (Method to trigger an event)
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		evt_id	- [in]  Collection Event ID
 retn : TRUE or FALSE
 note : This method triggers an equipment event
*/
BOOL CE30GEM::SetTrigEvent(LONG conn_id, LONG evt_id)
{
	HRESULT hResult;
	CComBSTR bstrName;
	m_pLogData->PushLogs(L"SetTriggerEvent::TriggerEvent", evt_id);
	hResult = m_pICxClientApp->TriggerEvent(conn_id, &evt_id, &bstrName);
	m_pLogData->CheckResult(hResult, L"SetTriggerEvent::TriggerEvent");

	return (hResult == S_OK);
}

/*
 desc : Trigger Event Data 발생 (부모에게 알림) - 1 개 값에 대해 설정
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		evt_id	- [in]  Event Index (In EPJ File)
		var_id	- [in]  Variable Index (DV) (In EPJ File)
		value	- [in]  값
		type	- [in]  'value' type (U1, U2, U4, Bo, I1, I3, ...)
 retn : TRUE or FALSE
 note : 단, list, jis_8_string 등은 제외
*/
BOOL CE30GEM::SetTrigEventValue(LONG conn_id, LONG evt_id, LONG var_id,
								LNG_CVVT value, E30_GPVT type)
{
	BOOL bSucc			= TRUE;
	HRESULT hResult		= S_OK;
	TCHAR tzVarName[CIM_CMD_NAME_SIZE]	= {NULL};
#if (USE_EPJ_LOAD_FILE)
	PTCHAR ptzEvtName	= NULL, ptzVarName = NULL;
#endif
	CComSafeArray <BSTR> arrDataNames;
	ICxValueDispPtr dispDataValues;
	/* Event && Variable ID에 해당되는 Name 얻기 */
#if (USE_EPJ_LOAD_FILE)
	ptzEvtName	= m_pEPJFile->GetID2Name(conn_id, evt_id, en_estc_events);
	if (!ptzEvtName)	return FALSE;
	wcscpy_s(tzEvtName, CIM_CMD_NAME_SIZE, ptzEvtName);
	ptzVarName	= m_pEPJFile->GetID2Name(conn_id, var_id, en_estc_variables);
	if (!ptzVarName)	return FALSE;
	wcscpy_s(tzVarName, CIM_CMD_NAME_SIZE, ptzVarName);
#else
	if (!GetVarIDToName(conn_id, var_id, tzVarName, CIM_CMD_NAME_SIZE))	return FALSE;
#endif
	/* Data Value 인스턴스 생성 */
	hResult	= dispDataValues.CreateInstance(CLSID_CxValueObject);
	if (S_OK == m_pLogData->CheckResult(hResult, L"SetTrigEventValue::dataValue.CreateInstance"))
	{
		if (SetDispAppendValue(dispDataValues, value, type))
		{
			/* Name 등록 */
			arrDataNames.Add(CComBSTR(tzVarName));
			/* Trigger Event 등록 */
			bSucc	= SendCollectionEventWithData(conn_id, arrDataNames, dispDataValues, evt_id, 0 /* fixed */);
			/* 기존 사용한 메모리 회수 */
			dispDataValues->Clear();
			arrDataNames.Destroy();
		}
	}
	/* Data Value 인스턴스 해제 */
	dispDataValues.Release();

	return bSucc;
}

/*
 desc : Trigger Event Data 발생 (부모에게 알림) - n 개 값에 대해 설정
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		evt_id	- [in]  Event Index (In EPJ File)
		var_ids	- [in]  Variable ID가 저장된 리스트
		values	- [in]  값이 저장된 리스트
		types	- [in]  'value' type이 저장된 리스트 (U1, U2, U4, Bo, I1, I3, ...)
 retn : TRUE or FALSE
 note : 단, list, jis_8_string 등은 제외
*/
BOOL CE30GEM::SetTrigEventValues(LONG conn_id, LONG evt_id,
								 CAtlList<LONG> &var_ids, CAtlList<UNG_CVVT> &values,
								 CAtlList<E30_GPVT> &types)
{
	BOOL bSucc			= TRUE;
	LONG i, lVarID;
	HRESULT hResult		= S_OK;
	E30_GPVT enType;
	TCHAR tzVarName[CIM_CMD_NAME_SIZE]	= {NULL};
	CComSafeArray <BSTR> arrDataNames;
	ICxValueDispPtr dispDataValues;
	/* Variable Casting */
	UNG_CVVT unValueV	= {NULL};

	/* Data Value 인스턴스 생성 */
	hResult	= dispDataValues.CreateInstance(CLSID_CxValueObject);
	if (S_OK != m_pLogData->CheckResult(hResult, L"SetTrigEventValues::dataValues.CreateInstance"))	return FALSE;
	/* Value 등록 */
	for (i=0;i<values.GetCount() && bSucc;i++)
	{
		/* Data Name */
		lVarID		= var_ids.GetAt(var_ids.FindIndex(i));
		if (!GetVarIDToName(conn_id, lVarID, tzVarName, CIM_CMD_NAME_SIZE))	return FALSE;
		/* Data Value */
		unValueV= values.GetAt(values.FindIndex(i));
		/* Data Type */
		enType	= types.GetAt(types.FindIndex(i));
		bSucc	= SetDispAppendValue(dispDataValues, &unValueV, enType);
		/* Name 등록 */
		if (bSucc)	arrDataNames.Add(CComBSTR(tzVarName));
	}
	/* Trigger Event 등록 */
	if (bSucc)	bSucc	= SendCollectionEventWithData(conn_id, arrDataNames, dispDataValues, evt_id, 0 /* fixed */);
	/* 기존 사용한 메모리 회수 */
	arrDataNames.Destroy();
	/* Data Value 인스턴스 해제 */
	dispDataValues->Clear();
	dispDataValues.Release();

	return bSucc;
}

/*
 desc : Trigger Event Data 발생 (부모에게 알림) - 여러 개 값에 대해 설정
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		evt_id	- [in]  Event Index (In EPJ File)
		var_id	- [in]  Variable Index (DV) (In EPJ File)
		values	- [in]  값이 저장된 리스트
		types	- [in]  'value' type이 저장된 리스트 (U1, U2, U4, Bo, I1, I3, ...)
 retn : TRUE or FALSE
 note : 단, jis_8_string 등은 제외
*/
BOOL CE30GEM::SetTrigEventList(LONG conn_id, LONG evt_id, LONG var_id,
								  CAtlList<UNG_CVVT> &values, CAtlList<E30_GPVT> &types)
{
	BOOL bSucc			= TRUE;
	LONG i, lHandle;
	HRESULT hResult		= S_OK;
	E30_GPVT enType;
	TCHAR tzVarName[CIM_CMD_NAME_SIZE]	= {NULL};
	CComSafeArray <BSTR> arrDataNames;
	ICxValueDispPtr dispDataValues;
	/* Variable Casting */
	UNG_CVVT unValueV	= {NULL};

	/* Event && Variable ID에 해당되는 Name 얻기 */
	if (!GetVarIDToName(conn_id, var_id, tzVarName, CIM_CMD_NAME_SIZE))	return FALSE;

	/* Data Value 인스턴스 생성 */
	hResult	= dispDataValues.CreateInstance(CLSID_CxValueObject);
	if (S_OK != m_pLogData->CheckResult(hResult, L"SetTrigEventValue::dataValues.CreateInstance"))	return FALSE;
	/* Get the handle */
	hResult	= dispDataValues->AppendList(0, &lHandle);
	if (S_OK == m_pLogData->CheckResult(hResult, L"SetTrigEventValue::dataValues->AppendList"))
	{
		/* Value 등록 */
		for (i=0;i<values.GetCount() && bSucc;i++)
		{
			/* Data Value */
			unValueV= values.GetAt(values.FindIndex(i));
			/* Data Type */
			enType	= types.GetAt(types.FindIndex(i));
			bSucc	= SetDispAppendValue(dispDataValues, &unValueV, enType);
		}
		/* Name 등록 */
		arrDataNames.Add(CComBSTR(tzVarName));
		/* Trigger Event 등록 */
		if (bSucc)	bSucc	= SendCollectionEventWithData(conn_id, arrDataNames,
														  dispDataValues, evt_id, 0 /* fixed */);
		/* 기존 사용한 메모리 회수 */
		arrDataNames.Destroy();
	}
	/* Data Value 인스턴스 해제 */
	dispDataValues->Clear();
	dispDataValues.Release();

	return bSucc;
}

/*
 desc : 시간 동기화
 parm : t_secs	- [in]  현재 컴퓨터 (장비)의 시간을 초로 환산한 값 (1970.01.01 이후부터 경과된 시간)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SetSynchronizeTime(UINT64 t_secs)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetSynchronizeTime::synchronizeTime time : [%I64u]", t_secs);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxClientData->SynchronizeTime(t_secs);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : 장비 내에 존재하는 알람 ID 제거
 parm : id	- [in]  제거하고자 하는 알람 ID
		logs- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SetDeleteAlarm(LONG id, BOOL logs)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetDeleteAlarm::DeleteAlarm (alarm_id = %d)", id);
	if (logs)	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientTool->DeleteAlarm(id);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : 모든 요청받은 데이터 (명령?)들 제거
 parm : logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SetDeleteAllRequests(BOOL logs)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;

	if (logs)
	{
		swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetDeleteAllRequests::DeleteAllRequests");
		m_pLogData->PushLogs(tzMesg);
	}
	hResult = m_pICxClientData->DeleteAllRequests();
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : 알람 이름에 해당되는 알람 ID 값 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		name	- [in]  알람 이름
		id		- [out] 알람 ID 반환
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetAlarmID(LONG conn_id, PTCHAR alarm_name, LONG &id, BOOL logs)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;
	CComBSTR bstrName (alarm_name);

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetAlarmID::GetAlarmID (conn_id = %d, alarm_name=%s)", conn_id, alarm_name);
	if (logs)	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientTool->GetAlarmID(conn_id, bstrName, &id);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : 통신 상태 (Low Level Communicating state) 값 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		state	- [out] 통신 여부
		substate- [out] HSMS (Ethernet) 상태 여부
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetCommState(LONG conn_id, E30_GCSS &state, E30_GCSH &substate, BOOL logs)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG lState, lSubState;
	HRESULT hResult		= S_OK;

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetCommState::GetCommState (conn_id=%d)", conn_id);
	if (logs)	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientApp->GetCommState(conn_id, &lState, &lSubState);
	m_pLogData->CheckResult(hResult, tzMesg);

	/* 값 변환 */
	state	= E30_GCSS(lState);
	substate= E30_GCSH(lSubState);

	return (hResult == S_OK);
}

/*
 desc : Host 연결에 대한 이름 즉, connection id에 매핑되는 이름 얻기
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		name	- [out] 연결된 Host Name 값 얻기
		size	- [in]  'name' 버퍼 크기
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetConnectionName(LONG conn_id, PTCHAR name, UINT32 size, BOOL logs)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;
	_bstr_t bstName;
	CComBSTR bstrName;

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetConnectionName::GetConnectionName (conn_id=%d)", conn_id);
	if (logs)	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientTool->GetConnectionName(conn_id, &bstrName);
	m_pLogData->CheckResult(hResult, tzMesg);

	/* 값 변환 */
	bstName	= _bstr_t(bstrName);

	/* 반환 버퍼 초기화 및 값 복사 */
	wmemset(name, 0x00, size);
	wmemcpy_s(name, size, (LPCTSTR)bstName, bstName.length());

	return (hResult == S_OK);
}

/*
 desc : 현재 적재된 Project Name 얻기
 parm : name	- [out] Project Name 값 얻기
		size	- [in]  'name' 버퍼 크기
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetCurrentProjectName(PTCHAR name, UINT32 size, BOOL logs)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;
	_bstr_t bstName;
	CComBSTR bstrName;

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetCurrentProjectName::GetCurrentProject");
	if (logs)	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientTool->GetCurrentProject(&bstrName);
	m_pLogData->CheckResult(hResult, tzMesg);

	/* 값 변환 */
	bstName	= _bstr_t(bstrName);

	/* 반환 버퍼 초기화 및 값 복사 */
	wmemset(name, 0x00, size);
	wmemcpy_s(name, size, (LPCTSTR)bstName, bstName.length());

	return (hResult == S_OK);
}

/*
 desc : 기본 설정된 Default Project Name 얻기
 parm : name	- [out] Project Name 값 얻기
		size	- [in]  'name' 버퍼 크기
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetDefaultProjectName(PTCHAR name, UINT32 size, BOOL logs)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;
	_bstr_t bstName;
	CComBSTR bstrName;

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetDefaultProjectName::GetDefaultProjectFile");
	if (logs)	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientTool->GetDefaultProjectFile(&bstrName);
	m_pLogData->CheckResult(hResult, tzMesg);

	/* 값 변환 */
	bstName	= _bstr_t(bstrName);

	/* 반환 버퍼 초기화 및 값 복사 */
	wmemset(name, 0x00, size);
	wmemcpy_s(name, size, (LPCTSTR)bstName, bstName.length());

	return (hResult == S_OK);
}

/*
 desc : 현재 연결된 장비 ID 값 반환
 parm : equip_id- [out] 매핑(관리)되는 장비 ID 값이 저장됨
		logs	- [in]  TRUE:로그 이력 남김, FALSE:로그 이력 남기지 않음
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetHostToEquipID(LONG &equip_id, BOOL logs)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetHostToEquipID::GetEquipmentID");
	if (logs)	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientApp->GetEquipmentID(&equip_id);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Variable ID에 해당되는 Min, Max 값 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		id		- [in]  Variable ID
		type	- [out] Value Type (E30_GPVT)이 저장됨
		min		- [out] Min 값이 저장됨
		max		- [out] Max 값이 저장됨
 retn : TRUE or FALSE
*/
#if 0
BOOL CE30GEM::GetVarMinMax(LONG conn_id, LONG id, E30_GPVT &type, PVOID min, PVOID max)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]		= {NULL};
	PUINT8 pu8ValMin		= PUINT8(min);
	PUINT8 pu8ValMax		= PUINT8(max);
	PINT16 pi16ValMin		= PINT16(min);
	PINT16 pi16ValMax		= PINT16(max);
	PUINT16 pu16ValMin		= PUINT16(min);
	PUINT16 pu16ValMax		= PUINT16(max);
	VARIANT_BOOL *pbValMin	= (VARIANT_BOOL*)min;
	VARIANT_BOOL *pbValMax	= (VARIANT_BOOL*)max;
	PLONG plValMin			= PLONG(min);
	PLONG plValMax			= PLONG(max);
	LONG lCount = 0, lEventID;
	HRESULT hResult			= S_OK, hResultMin, hResultMax;
	VARIANT_BOOL varBoPrivate, varBoPersistent;
	VarType varType;
	ValueType valType;
	CComBSTR bstrName, bstrDesc, bstrUnit;
	ICxValueDispPtr pICxValueCur (CLSID_CxValueObject);
	ICxValueDispPtr pICxValueMin (CLSID_CxValueObject);
	ICxValueDispPtr pICxValueMax (CLSID_CxValueObject);
	ICxValueDispPtr pICxValueDef (CLSID_CxValueObject);

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetVarMinMax::GetVarInfo");
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientTool->GetVarInfo(conn_id, &id, &bstrName, pICxValueCur, &varType,
										   &valType, &lEventID, &bstrDesc, &bstrUnit,
										   pICxValueMin, pICxValueMax, pICxValueDef,
										   &varBoPrivate, &varBoPersistent);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Get the value from the ICxValueDisp object, and put the value into the 'value' integer. 
	   This is the code that restricts this function to handle values of type integer. */
	switch (valType)
	{
	case Bi	:	hResultMin = pICxValueMin->GetValueBinary(0, 0, pu8ValMin);
				hResultMax = pICxValueMax->GetValueBinary(0, 0, pu8ValMax);		break;
	case Bo	:	hResultMin = pICxValueMin->GetValueBoolean(0, 0, pbValMin);
				hResultMax = pICxValueMax->GetValueBoolean(0, 0, pbValMax);		break;
	case I1	:	hResultMin = pICxValueMin->GetValueI1(0, 0, pi16ValMin);
				hResultMax = pICxValueMax->GetValueI1(0, 0, pi16ValMax);		break;
	case I2	:	hResultMin = pICxValueMin->GetValueI2(0, 0, pi16ValMin);
				hResultMax = pICxValueMax->GetValueI2(0, 0, pi16ValMax);		break;
	case I4	:	hResultMin = pICxValueMin->GetValueI4(0, 0, plValMin);
				hResultMax = pICxValueMax->GetValueI4(0, 0, plValMax);			break;
	case I8	:	hResultMin = pICxValueMin->GetValueI8(0, 0, plValMin);
				hResultMax = pICxValueMax->GetValueI8(0, 0, plValMax);			break;
	case U1	:	hResultMin = pICxValueMin->GetValueU1(0, 0, pu8ValMin);
				hResultMax = pICxValueMax->GetValueU1(0, 0, pu8ValMax);			break;
	case U2	:	hResultMin = pICxValueMin->GetValueU2(0, 0, plValMin);
				hResultMax = pICxValueMax->GetValueU2(0, 0, plValMax);			break;
	case U4	:	hResultMin = pICxValueMin->GetValueU4(0, 0, plValMin);
				hResultMax = pICxValueMax->GetValueU4(0, 0, plValMax);			break;
	case U8	:	hResultMin = pICxValueMin->GetValueU8(0, 0, plValMin);
				hResultMax = pICxValueMax->GetValueU8(0, 0, plValMax);			break;
	/* 문자열의 경우 NULL */
	case A	:	min = NULL; max = NULL;	break;
	case W	:	min = NULL; max = NULL;	break;
	default	:	m_pLogData->PushLogs(L"GetVariableValue : Unsupported value type");	return FALSE;
	}
	type	= E30_GPVT(valType);
	return (hResultMin == S_OK && hResultMax == S_OK);
}
#else
BOOL CE30GEM::GetVarMinMax(LONG conn_id, LONG id, E30_GPVT &type, UNG_CVVT &min, UNG_CVVT &max)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]		= {NULL};
	short i16Value;
	long lValue;
	double dbValue;
	VARIANT_BOOL bValMin;
	VARIANT_BOOL bValMax;
	LONG lCount = 0, lEventID;
	HRESULT hResult	= S_OK, hResultMin, hResultMax;
	VARIANT_BOOL varBoPrivate, varBoPersistent;
	VarType varType;
	ValueType valType;
	CComBSTR bstrName, bstrDesc, bstrUnit, bstrValue;
	ICxValueDispPtr pICxValueCur (CLSID_CxValueObject);
	ICxValueDispPtr pICxValueMin (CLSID_CxValueObject);
	ICxValueDispPtr pICxValueMax (CLSID_CxValueObject);
	ICxValueDispPtr pICxValueDef (CLSID_CxValueObject);

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetVarMinMax::GetVarInfo");
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientTool->GetVarInfo(conn_id, &id, &bstrName, pICxValueCur, &varType,
										   &valType, &lEventID, &bstrDesc, &bstrUnit,
										   pICxValueMin, pICxValueMax, pICxValueDef,
										   &varBoPrivate, &varBoPersistent);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Get the value from the ICxValueDisp object, and put the value into the 'value' integer. 
	   This is the code that restricts this function to handle values of type integer. */
	switch (valType)
	{
	case Bi	:	hResultMin = pICxValueMin->GetValueBinary(0, 0, &min.bi_value);
				hResultMax = pICxValueMax->GetValueBinary(0, 0, &max.bi_value);									break;
	case Bo	:	hResultMin = pICxValueMin->GetValueBoolean(0, 0, &bValMin);		min.b_value		= (bValMin == VARIANT_TRUE) ? TRUE : FALSE;
				hResultMax = pICxValueMax->GetValueBoolean(0, 0, &bValMax);		max.b_value		= (bValMax == VARIANT_TRUE) ? TRUE : FALSE;	break;
	case I1	:	hResultMin = pICxValueMin->GetValueI1(0, 0, &i16Value);			min.i8_value	= INT8(i16Value);
				hResultMax = pICxValueMax->GetValueI1(0, 0, &i16Value);			max.i8_value	= INT8(i16Value);	break;
	case I2	:	hResultMin = pICxValueMin->GetValueI2(0, 0, &i16Value);			min.i16_value	= i16Value;
				hResultMax = pICxValueMax->GetValueI2(0, 0, &i16Value);			max.i16_value	= i16Value;			break;
	case I4	:	hResultMin = pICxValueMin->GetValueI4(0, 0, &lValue);			min.i32_value	= lValue;
				hResultMax = pICxValueMax->GetValueI4(0, 0, &lValue);			max.i32_value	= lValue;			break;
	case I8	:	hResultMin = pICxValueMin->GetValueI8(0, 0, &lValue);			min.i64_value	= lValue;
				hResultMax = pICxValueMax->GetValueI8(0, 0, &lValue);			max.i64_value	= lValue;			break;
	case U1	:	hResultMin = pICxValueMin->GetValueU1(0, 0, &min.u8_value);
				hResultMax = pICxValueMax->GetValueU1(0, 0, &max.u8_value);											break;
	case U2	:	hResultMin = pICxValueMin->GetValueU2(0, 0, &lValue);			min.u16_value	= UINT16(lValue);
				hResultMax = pICxValueMax->GetValueU2(0, 0, &lValue);			max.u16_value	= UINT16(lValue);	break;
	case U4	:	hResultMin = pICxValueMin->GetValueU4(0, 0, &lValue);			min.u32_value	= UINT32(lValue);
				hResultMax = pICxValueMax->GetValueU4(0, 0, &lValue);			max.u32_value	= UINT32(lValue);	break;
	case U8	:	hResultMin = pICxValueMin->GetValueU8(0, 0, &lValue);			min.u64_value	= UINT64(lValue);
				hResultMax = pICxValueMax->GetValueU8(0, 0, &lValue);			max.u64_value	= UINT64(lValue);	break;
	case F4	:	hResultMin = pICxValueMin->GetValueFloat(0, 0, &dbValue);		min.f_value		= FLOAT(dbValue);
				hResultMax = pICxValueMax->GetValueFloat(0, 0, &dbValue);		max.f_value		= FLOAT(dbValue);	break;
	case F8	:	hResultMin = pICxValueMin->GetValueFloat(0, 0, &dbValue);		min.d_value		= DOUBLE(dbValue);
				hResultMax = pICxValueMax->GetValueFloat(0, 0, &dbValue);		max.d_value		= DOUBLE(dbValue);	break;
	case A	:	hResultMin = pICxValueMin->GetValueAscii(0, 0, &bstrValue);
				if (bstrValue)	wcscpy_s(min.s_value, CIM_VAL_ASCII_SIZE, (LPCTSTR)_bstr_t(bstrValue));
				hResultMax = pICxValueMax->GetValueAscii(0, 0, &bstrValue);
				if (bstrValue)	wcscpy_s(max.s_value, CIM_VAL_ASCII_SIZE, (LPCTSTR)_bstr_t(bstrValue));			break;
	/* 문자열의 경우 NULL */
	default	:	m_pLogData->PushLogs(L"GetVariableValue : Unsupported value type");	return FALSE;
	}
	type	= E30_GPVT(valType);
	return (hResultMin == S_OK && hResultMax == S_OK);
}
#endif
/*
 desc : 현재 연결되어 있는 Host ID 기준으로 변수들 정보 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		type	- [in]  변수 타입 (E30_GVTC)
		lst_data- [out] 변수들의 정보가 저장될 리스트 참조
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetHostVariables(LONG conn_id, E30_GVTC type, CAtlList <STG_CEVI> &lst_data)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	BOOL bSucc;
	UINT8 u8Value;
	LONG i, lHandle1, lHandle2, lCount1, lValue;
	HRESULT hResult		= S_OK;
	VARIANT varList;
	STG_CEVI stData;
#if 0
	POSITION pPos;
#endif
	CComBSTR bstrValue;
	ICxValueDispPtr pICxValueDisp (CLSID_CxValueObject);

	/* Variant 초기화 */
	VariantInit(&varList);

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetHostVariables::GetVariablesForConnection (conn_id=%d, type=%d)",
			   conn_id, type);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientTool->GetVariablesForConnection(conn_id, VarType(type), &varList);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 복사해 옴 */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetHostVariables::RestoreFromByteStream (conn_id=%d, type=%d)", 
			   conn_id, type);
	m_pLogData->PushLogs(tzMesg);
	pICxValueDisp->RestoreFromByteStream(0, 0, varList);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* List 정보 얻기 */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetHostVariables::ListAt (conn_id=%d, type=%d)",
			   conn_id, type);
	m_pLogData->PushLogs(tzMesg);
	hResult = pICxValueDisp->ListAt(0, 0, &lHandle1);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* List 개수 */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetHostVariables::ItemCount (conn_id=%d, type=%d, handle1=%d)",
			   conn_id, type, lHandle1);
	m_pLogData->PushLogs(tzMesg);
	hResult = pICxValueDisp->ItemCount(lHandle1, &lCount1);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* Variables 개수만큼 가져오기  */
	for (i=1,bSucc=FALSE; i<=lCount1; i++)
	{
		/* 등록 정보 초기화  */
		stData.Init();

		/* Variables 정보 얻기 */
		swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetHostVariables::ListAt (conn_id=%d, type=%d, handle1=%d)",
				   conn_id, type, lHandle1);
		m_pLogData->PushLogs(tzMesg);
		hResult = pICxValueDisp->ListAt(lHandle1, i, &lHandle2);
		m_pLogData->CheckResult(hResult, tzMesg);
		/* Variables ID 얻기 */
		if (hResult == S_OK)
		{
			swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetHostVariables::Variable ID (conn_id=%d, type=%d, "
												  L"handle1=%d, handle2=%d, index=%d)",
					   conn_id, type, lHandle1, lHandle2, i);
			m_pLogData->PushLogs(tzMesg);
			hResult = pICxValueDisp->GetValueU4(lHandle2, 1, &lValue);
			m_pLogData->CheckResult(hResult, tzMesg);
			stData.var_id	= lValue;
		}
		/* Variables Name 얻기 */
		if (hResult == S_OK)
		{
			bstrValue.Empty();
			swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetHostVariables::Variable Name (conn_id=%d, type=%d, "
												  L"handle1=%d, handle2=%d, index=%d, var_id=%d)",
					   conn_id, type, lHandle1, lHandle2, i, lValue);
			m_pLogData->PushLogs(tzMesg);
			hResult = pICxValueDisp->GetValueAscii(lHandle2, 2, &bstrValue);
			m_pLogData->CheckResult(hResult, tzMesg);
			swprintf_s(stData.var_name, CIM_CMD_NAME_SIZE, L"%s", (LPCTSTR)_bstr_t(bstrValue));
		}
		/* Variables Description 얻기 */
		if (hResult == S_OK)
		{
			bstrValue.Empty();
			swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetHostVariables::Variable Desc (conn_id=%d, type=%d, handle1=%d, handle2=%d, index=%d, var_id=%d)", conn_id, type, lHandle1, lHandle2, i, lValue);
			m_pLogData->PushLogs(tzMesg);
			hResult = pICxValueDisp->GetValueAscii(lHandle2, 3, &bstrValue);
			m_pLogData->CheckResult(hResult, tzMesg);
			swprintf_s(stData.var_desc, CIM_CMD_DESC_SIZE, L"%s", (LPCTSTR)_bstr_t(bstrValue));
		}
		/* Variables Type 얻기 */
		if (hResult == S_OK)
		{
			swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetHostVariables::Variable Type (conn_id=%d, type=%d, handle1=%d, handle2=%d, index=%d, var_id=%d)", conn_id, type, lHandle1, lHandle2, i, lValue);
			m_pLogData->PushLogs(tzMesg);
			hResult = pICxValueDisp->GetValueU1(lHandle2, 5, &u8Value);
			m_pLogData->CheckResult(hResult, tzMesg);
			stData.var_type	= u8Value;
		}
		/* Value Type 얻기 */
		if (hResult == S_OK)
		{
			swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetHostVariables::Value Type (conn_id=%d, type=%d, handle1=%d, handle2=%d, index=%d, var_id=%d)", conn_id, type, lHandle1, lHandle2, i, lValue);
			m_pLogData->PushLogs(tzMesg);
			hResult = pICxValueDisp->GetValueU4(lHandle2, 6, &lValue);
			m_pLogData->CheckResult(hResult, tzMesg);
			stData.val_type	= lValue;
		}
		/* Current Value 얻기 */
		if (hResult == S_OK)			bSucc = GetValueDispPtr(lHandle2, 10, 0x00, pICxValueDisp, &stData);
		/* Default Value 얻기 */
		if (hResult == S_OK && bSucc)	bSucc = GetValueDispPtr(lHandle2, 13, 0x01, pICxValueDisp, &stData);
		/* 최종 등록 */
#ifdef _DEBUG
		lst_data.AddTail(stData);
#else
		if (bSucc)	lst_data.AddTail(stData);
		else		stData.Delete();
#endif
	}
#if 0
	/* 등록 작업이 실패 했다면 */
	if (!bSucc)
	{
		/* 기존 등록된거 모두 제거 */
		pPos	= lst_data.GetHeadPosition();
		while (pPos)
		{
			stData	= lst_data.GetNext(pPos);
			stData.Delete();
		}
		lst_data.RemoveAll();
	}
#endif
	return (hResult == S_OK);
}

/*
 desc : ICxValueDisp에서 Value Type 별로 Value 값 반환
 parm : handle	- [in]  Handle Index
		index	- [in]  Index
		flag	- [in]  0x00: Current Value, 0x01: Default Value
		disp	- [in]  ICxValueDisp Pointer
		data	- [in]  기본 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetValueDispPtr(LONG handle, LONG index, UINT8 flag,
							  ICxValueDispPtr disp, LPG_CEVI data)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL}, tzType[2][16]	= { L"Current", L"Default" };
	HRESULT hResult	= S_FALSE;
	INT64 i64Value	= 0;
	CComBSTR bstrValue;

	switch (data->val_type)
	{
	case E30_GPVT::en_b		:
	case E30_GPVT::en_bi		:
	case E30_GPVT::en_i1		:
	case E30_GPVT::en_i2		:
	case E30_GPVT::en_i4		:
	case E30_GPVT::en_i8		:
	case E30_GPVT::en_u1		:
	case E30_GPVT::en_u2		:
	case E30_GPVT::en_u4		:
	case E30_GPVT::en_u8		:
	case E30_GPVT::en_f4		:
	case E30_GPVT::en_f8		:
		switch (data->val_type)
		{
		case E30_GPVT::en_b	:	hResult	= disp->GetValueBoolean(handle,	index,	(VARIANT_BOOL*)&i64Value);	break;
		case E30_GPVT::en_bi	:	hResult	= disp->GetValueBinary(handle,	index,	(PUINT8)&i64Value);			break;
		case E30_GPVT::en_i1	:	hResult	= disp->GetValueI1(handle,		index,	(PINT16)&i64Value);			break;
		case E30_GPVT::en_i2	:	hResult	= disp->GetValueI2(handle,		index,	(PINT16)&i64Value);			break;
		case E30_GPVT::en_i4	:	hResult	= disp->GetValueI4(handle,		index,	(PLONG)&i64Value);			break;
		case E30_GPVT::en_i8	:	hResult	= disp->GetValueI8(handle,		index,	(PLONG)&i64Value);			break;
		case E30_GPVT::en_u1	:	hResult	= disp->GetValueU1(handle,		index,	(PUINT8)&i64Value);			break;
		case E30_GPVT::en_u2	:	hResult	= disp->GetValueU2(handle,		index,	(PLONG)&i64Value);			break;
		case E30_GPVT::en_u4	:	hResult	= disp->GetValueU4(handle,		index,	(PLONG)&i64Value);			break;
		case E30_GPVT::en_u8	:	hResult	= disp->GetValueU8(handle,		index,	(PLONG)&i64Value);			break;
		case E30_GPVT::en_f4	:	hResult	= disp->GetValueF4(handle,		index,	(PFLOAT)&i64Value);			break;
		case E30_GPVT::en_f8	:	hResult	= disp->GetValueF8(handle,		index,	(DOUBLE*)&i64Value);		break;
		}
		if (hResult == S_OK)	data->SetValueNum(E30_GPVT(data->val_type), flag, (PVOID)&i64Value);
		break;

	case E30_GPVT::en_a		:	hResult	= disp->GetValueAscii(handle, index, &bstrValue);
							if (bstrValue)	data->SetValueStr(flag, (PTCHAR)_bstr_t(bstrValue));
							break;
	}
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetHostVariables::%s Value (type=%d, handle=%d, index=%d, var_id=%d)",
			   tzType[flag], data->val_type, handle, index, data->var_id);
	m_pLogData->PushLogs(tzMesg);
	if (index !=13 && m_pLogData->CheckResult(hResult, tzMesg))
	{
		return FALSE;
	}

	return TRUE;
}

/*
 desc : 로그 상태 (파일에 로그 파일이 저장 중인지 여부)
 parm : enable	- [out] 로그 파일 저장 여부 값 반환
 retn : TRUE or FALSE
*/
BOOL CE30GEM::IsLogging(BOOL &enable)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;

	hResult = m_pICxClientApp->get_IsLogging((VARIANT_BOOL*)&enable);
	if (hResult)
	{
		swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"IsLogging::get_IsLogging");
		m_pLogData->CheckResult(hResult, tzMesg);
	}
	return (hResult == S_OK);
}

/*
 desc : 로그 동작 유무 설정
 parm : enable	- [in]  로그 파일 동작 유무 설정 플래그
						enable=TRUE -> Start, enable=FALSE -> Stop
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SetLogEnable(BOOL enable)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	TCHAR tzRun[2][16]	= { L"StopLogging", L"StartLogging" };
	BOOL bStatus		= FALSE;
	HRESULT hResult		= S_OK;

	/* 현재 로그 동작 여부 */
	if (!IsLogging(bStatus))	return FALSE;
	if (bStatus == enable)		return TRUE;

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetLogEnable::%s", tzRun[enable]);
	if (enable)	hResult = m_pICxClientApp->StartLogging();
	else		hResult = m_pICxClientApp->StopLogging();
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : 현재 등록 (발생)된 명령 ID 값들 반환
 parm : id		- [out] 리스트가 저장될 CAtlList <LONG> : ID
		name	- [out] 리스트가 저장될 String Array    : Name
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetCommandList(CAtlList <LONG> &ids, CStringArray &names)
{
	LONG i, lCount	= 0;
	LONG *pIDs		= NULL;
	CComBSTR *pNames= NULL;
	HRESULT hResult	= S_OK;
	VARIANT vtIDs, vtNames, vtDescs;
	CComSafeArray <LONG> saIDs;
	CComSafeArray <BSTR> saNames;

	/* Initializes a variant */
	VariantInit(&vtIDs);
	VariantInit(&vtNames);
	VariantInit(&vtDescs);

	/* 현재 CIMConnect 내부에 발생된 알람 리스트 가져오기 */
	hResult	= m_pICxClientTool->ListCommands(&vtIDs, &vtNames, &vtDescs);
	if (FAILED(hResult))
	{
		m_pLogData->CheckResult(hResult, L"GetCommandList::ListCommands");
		return FALSE;
	}
	/* Get access to the list of IDs, States, Texts */
	if (SUCCEEDED(hResult))	hResult = saIDs.Attach(vtIDs.parray);
	if (SUCCEEDED(hResult))	hResult = saNames.Attach(vtNames.parray);
	if (m_pLogData->CheckResult(hResult, L"GetCommandList::SafeArrary.Attach"))	return FALSE;

	/* 현재 개수 구하기 */
	if ((lCount = GetSafeArrayCount(saIDs, 1)) > 0)
	{
		m_pLogData->PushLogs(L"The command was collected");
		/* get the id arrary */
		hResult	= SafeArrayLock(saIDs);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetCommandList (IDs)"))
		{
			hResult	= SafeArrayAccessData(saIDs, (PVOID*)&pIDs);
			if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetCommandList (IDs)"))
				hResult	= SafeArrayUnaccessData(saIDs);
			hResult	= SafeArrayUnlock(saIDs);
		}
		/* get the name array */
		hResult	= SafeArrayLock(saNames);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetCommandList (Names)"))
		{
			hResult	= SafeArrayAccessData(saNames, (PVOID*)&pNames);
			if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetCommandList (Names)"))
				hResult	= SafeArrayUnaccessData(saNames);
			hResult	= SafeArrayUnlock(saNames);
		}

		/* Command ID와 Names 등록  */
		for (i=0;SUCCEEDED(hResult) && i<lCount;i++)
		{
			ids.AddTail(pIDs[i]);
			names.Add(pNames[i]);
		}
	}

	/* SafeArray 해제 */
	saIDs.Detach();
	saNames.Detach();

	return TRUE;
}

/*
 desc : 현재 등록 (발생)된 EPJ 파일들 반환
 parm : files	- [out] 리스트가 저장될 String Array    : EPJ Files (Included Path)
		sizes	- [out] 리스트가 저장될 CAtlList <LONG> : EPJ File Size
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetEPJList(CStringArray &files, CAtlList <LONG> &sizes)
{
	LONG i, lCntFiles	= 0, lCntSizes = 0;
	LONG *pSizes		= NULL;
	CComBSTR *pFiles	= NULL;
	HRESULT hResult		= S_OK;
	VARIANT vtFiles, vtSizes;
	CComSafeArray <BSTR> saFiles;
	CComSafeArray <LONG> saSizes;

	/* Initializes a variant */
	VariantInit(&vtSizes);
	VariantInit(&vtFiles);

	/* 현재 CIMConnect 내부에 발생된 알람 리스트 가져오기 */
	hResult	= m_pICxClientTool->ListProjects(&vtFiles, &vtSizes);
	if (FAILED(hResult))
	{
		m_pLogData->CheckResult(hResult, L"GetEPJList::ListProjects");
		return FALSE;
	}
	/* Get access to the list of IDs, States, Texts */
	if (SUCCEEDED(hResult))	hResult = saFiles.Attach(vtFiles.parray);
	if (SUCCEEDED(hResult))	hResult = saSizes.Attach(vtSizes.parray);
	if (FAILED(hResult))
	{
		m_pLogData->CheckResult(hResult, L"GetEPJList::SafeArrary.Attach");
		return FALSE;
	}

	/* 현재 개수 구하기 */
	lCntSizes	= GetSafeArrayCount(saSizes, 1);
	if (lCntSizes > 0)
	{
		m_pLogData->PushLogs(L"The epj_file was collected");

		/* get the array */
		hResult	= SafeArrayLock(saSizes);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetEPJList (EPJ Sizes)"))
		{
			hResult	= SafeArrayAccessData(saSizes, (PVOID*)&pSizes);
			if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetEPJList (EPJ Sizes)"))
				hResult	= SafeArrayUnaccessData(saSizes);
			hResult	= SafeArrayUnlock(saSizes);
		}
		for (i=0;SUCCEEDED(hResult) && i<lCntSizes;i++)	sizes.AddTail(pSizes[i]);
	}
	lCntFiles	= GetSafeArrayCount(saFiles, 1);
	if (lCntFiles > 0)
	{
		/* get the arrary */
		hResult	= SafeArrayLock(saFiles);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetEPJList (EPJ Files)"))
		{
			hResult	= SafeArrayAccessData(saFiles, (PVOID*)&pFiles);
			if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetEPJList (EPJ Files)"))
				hResult	= SafeArrayUnaccessData(saFiles);
			hResult	= SafeArrayUnlock(saFiles);
		}

		/* Alarm ID와 State 등록  */
		for (i=0;SUCCEEDED(hResult) && i<lCntFiles;i++)	files.Add(pFiles[i]);
	}

	/* SafeArray 해제 */
	saFiles.Detach();
	saSizes.Detach();

	return TRUE;
}

/*
 desc : 현재 등록 (발생)된 Recipe Names 반환
 parm : names	- [out] 리스트가 저장될 String Array    : Recipe Names
		sizes	- [out] 리스트가 저장될 CAtlList <LONG> : Recip Sizes
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetRecipes(CStringArray &names, CAtlList <LONG> &sizes)
{
	LONG i, lCount	= 0;
	LONG *pSizes	= NULL;
	CComBSTR *pNames= NULL;
	HRESULT hResult	= S_OK;
	VARIANT vtNames, vtSizes;
	CComSafeArray <BSTR> saNames;
	CComSafeArray <LONG> saSizes;

	/* Initializes a variant */
	VariantInit(&vtSizes);
	VariantInit(&vtNames);

	/* 현재 CIMConnect 내부에 저장된 Recipe 리스트 가져오기 */
	hResult	= m_pICxClientTool->ListRecipes(&vtNames, &vtSizes);
	if (FAILED(hResult))
	{
		m_pLogData->CheckResult(hResult, L"GetRecipes::ListRecipes");
		return FALSE;
	}
	/* Get access to the list of IDs, States, Texts */
	if (SUCCEEDED(hResult))	hResult = saNames.Attach(vtNames.parray);
	if (SUCCEEDED(hResult))	hResult = saSizes.Attach(vtSizes.parray);
	if (FAILED(hResult))
	{
		m_pLogData->CheckResult(hResult, L"GetRecipes::SafeArrary.Attach");
		return FALSE;
	}

	/* 현재 개수 구하기 */
	if ((lCount = GetSafeArrayCount(saSizes, 1)) > 0)
	{
		m_pLogData->PushLogs(L"The epj_file was collected");
		/* get the arrary */
		hResult	= SafeArrayLock(saNames);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetRecipes (Recipe Names)"))
		{
			hResult	= SafeArrayAccessData(saNames, (PVOID*)&pNames);
			if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetRecipes (Recipe Names)"))
				hResult	= SafeArrayUnaccessData(saNames);
			hResult	= SafeArrayUnlock(saNames);
		}
		/* get the array */
		hResult	= SafeArrayLock(saSizes);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetRecipes (Recipe Sizes)"))
		{
			hResult	= SafeArrayAccessData(saSizes, (PVOID*)&pSizes);
			if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetRecipes (Recipe Sizes)"))
				hResult	= SafeArrayUnaccessData(saSizes);
			hResult	= SafeArrayUnlock(saSizes);
		}

		/* Alarm ID와 State 등록  */
		for (i=0;SUCCEEDED(hResult) && i<lCount;i++)
		{
			sizes.AddTail(pSizes[i]);
			names.Add(pNames[i]);
		}
	}

	/* SafeArray 해제 */
	saNames.Detach();
	saSizes.Detach();

	return TRUE;
}

/*
 desc : Host 쪽에 Process Program (Recipe)를 생성하겠다고 요청
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		name	- [in]  Recipe (Process Program) Name (확장자는 꼭 포함하지 않아도 됨)
		size	- [in]  Recipe (Process Program) Size (unit: bytes)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::QueryRecipeUpload(LONG conn_id, PTCHAR name, UINT32 size)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	CComBSTR bstrNames (name);
	HRESULT hResult		= S_OK;

	if (size < 1 || size > 0x00ffffff)
	{
		swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"QueryRecipeUpload::PPLoadInquire::Invalid File Size (%u bytes)", size);
		return FALSE;
	}
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"QueryRecipeUpload::PPLoadInquire (conn_id=%d, recipe=%s, size=%u bytes)",
			   conn_id, name, size);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxClientApp->PPLoadInquire(conn_id, bstrNames, size);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Host 쪽에 Process Program (Recipe)를 요청
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		name	- [in]  Recipe (Process Program) Name (확장자는 꼭 포함하지 않아도 됨)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::QueryRecipeRequest(LONG conn_id, PTCHAR name)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	CComBSTR bstrNames (name);
	HRESULT hResult		= S_OK;

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"QueryRecipeRequest::PPRequest (conn_id=%d, recipe=%s)",
			   conn_id, name);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxClientApp->PPRequest(conn_id, bstrNames);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : Host 쪽에 Process Program (Recipe)를 송신
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		name	- [in]  Recipe (Process Program) Name (확장자는 꼭 포함하지 않아도 됨)
 retn : TRUE or FALSE
 주의 : ICxValueDispPtr 부분 구현해야 되는데... 아직 이해를 못함
*/
BOOL CE30GEM::QueryRecipeSend(LONG conn_id, PTCHAR name)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	CComBSTR bstrNames (name);
	HRESULT hResult		= S_OK;
	ICxValueDispPtr pCxValueDisp(CLSID_CxValueObject);

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"QueryRecipeSend::PPSend (conn_id=%d, recipe=%s)", conn_id, name);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxClientApp->PPSend(conn_id, bstrNames, pCxValueDisp);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : 시스템에 정의된 Report 목록 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		ids		- [out] 리스트가 저장될 CAtlList <LONG> : Report Names
		names	- [out] 리스트가 저장될 String Array    : Report IDs
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetReports(LONG conn_id, CAtlList <LONG> &ids, CStringArray &names)
{
	LONG i, lCount	= 0;
	LONG *pIDs		= NULL;
	CComBSTR *pNames= NULL;
	HRESULT hResult	= S_OK;
	VARIANT vtIDs, vtNames;
	CComSafeArray <BSTR> saNames;
	CComSafeArray <LONG> saIDs;

	/* Initializes a variant */
	VariantInit(&vtIDs);
	VariantInit(&vtNames);

	/* 현재 CIMConnect 내부에 저장된 Report 리스트 가져오기 */
	hResult	= m_pICxClientTool->ListReports(conn_id, &vtIDs, &vtNames);
	if (FAILED(hResult))
	{
		m_pLogData->CheckResult(hResult, L"GetReports::ListReports");
		return FALSE;
	}
	/* Get access to the list of IDs, States, Texts */
	if (SUCCEEDED(hResult))	hResult = saIDs.Attach(vtIDs.parray);
	if (SUCCEEDED(hResult))	hResult = saNames.Attach(vtNames.parray);
	if (FAILED(hResult))
	{
		m_pLogData->CheckResult(hResult, L"GetRecipes::SafeArrary.Attach");
		return FALSE;
	}

	/* 현재 개수 구하기 */
	if ((lCount = GetSafeArrayCount(saIDs, 1)) > 0)
	{
		m_pLogData->PushLogs(L"The epj_file was collected");
		/* get the arrary */
		hResult	= SafeArrayLock(saNames);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetRecipes (Report Names)"))
		{
			hResult	= SafeArrayAccessData(saNames, (PVOID*)&pNames);
			if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetRecipes (Report Names)"))
				hResult	= SafeArrayUnaccessData(saNames);
			hResult	= SafeArrayUnlock(saNames);
		}
		/* get the array */
		hResult	= SafeArrayLock(saIDs);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetRecipes (Recipe IDs)"))
		{
			hResult	= SafeArrayAccessData(saIDs, (PVOID*)&pIDs);
			if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetRecipes (Recipe IDs)"))
				hResult	= SafeArrayUnaccessData(saIDs);
			hResult	= SafeArrayUnlock(saIDs);
		}

		/* Alarm ID와 State 등록  */
		for (i=0;SUCCEEDED(hResult) && i<lCount;i++)
		{
			ids.AddTail(pIDs[i]);
			names.Add(pNames[i]);
		}
	}

	/* SafeArray 해제 */
	saNames.Detach();
	saIDs.Detach();

	return TRUE;
}

/*
 desc : Report 및 관련된 Event의 리스트 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		r_ids	- [out] 리스트가 저장될 CAtlList <LONG> : Report IDs
		e_ids	- [out] 리스트가 저장될 CAtlList <LONG> : Event IDs
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetReportsEvents(LONG conn_id, CAtlList <LONG> &r_ids, CAtlList <LONG> &e_ids)
{
	LONG i, lCount	= 0;
	LONG *pRIDs		= NULL, *pEIDs = NULL;
	HRESULT hResult	= S_OK;
	VARIANT vtRIDs, vtEIDs;
	CComSafeArray <LONG> saRIDs;
	CComSafeArray <LONG> saEIDs;

	/* Initializes a variant */
	VariantInit(&vtRIDs);
	VariantInit(&vtEIDs);

	/* 현재 CIMConnect 내부에 저장된 Recipe 리스트 가져오기 */
	hResult	= m_pICxClientTool->ListReportsandEvents(conn_id, &vtRIDs, &vtEIDs);
	if (FAILED(hResult))
	{
		m_pLogData->CheckResult(hResult, L"GetReports::ListReports");
		return FALSE;
	}
	/* Get access to the list of IDs, States, Texts */
	if (SUCCEEDED(hResult))	hResult = saRIDs.Attach(vtRIDs.parray);
	if (SUCCEEDED(hResult))	hResult = saEIDs.Attach(vtEIDs.parray);
	if (FAILED(hResult))
	{
		m_pLogData->CheckResult(hResult, L"GetRecipes::SafeArrary.Attach");
		return FALSE;
	}

	/* 현재 개수 구하기 */
	if ((lCount = GetSafeArrayCount(saRIDs, 1)) > 0)
	{
		m_pLogData->PushLogs(L"The epj_file was collected");
		/* get the arrary */
		hResult	= SafeArrayLock(saRIDs);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetRecipes (Report Names)"))
		{
			hResult	= SafeArrayAccessData(saRIDs, (PVOID*)&pRIDs);
			if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetRecipes (Report Names)"))
				hResult	= SafeArrayUnaccessData(saRIDs);
			hResult	= SafeArrayUnlock(saRIDs);
		}
		/* get the array */
		hResult	= SafeArrayLock(saEIDs);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetRecipes (Recipe IDs)"))
		{
			hResult	= SafeArrayAccessData(saEIDs, (PVOID*)&pEIDs);
			if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetRecipes (Recipe IDs)"))
				hResult	= SafeArrayUnaccessData(saEIDs);
			hResult	= SafeArrayUnlock(saEIDs);
		}

		/* Alarm ID와 State 등록  */
		for (i=0;SUCCEEDED(hResult) && i<lCount;i++)
		{
			r_ids.AddTail(pRIDs[i]);
			e_ids.AddTail(pEIDs[i]);
		}
	}

	/* SafeArray 해제 */
	saRIDs.Detach();
	saEIDs.Detach();

	return TRUE;
}

/*
 desc : 발생된 Traces의 리스트 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		t_ids	- [out] 리스트가 저장될 CAtlList <LONG> : Report IDs
		t_names	- [out] 리스트가 저장될 String Array    : Trance Names
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetListTraces(LONG conn_id, CAtlList <LONG> &t_ids, CStringArray &t_names)
{
	LONG i, lCount	= 0;
	LONG *pTIDs		= NULL;
	HRESULT hResult	= S_OK;
	CComBSTR *pNames= NULL;
	VARIANT vtTIDs, vtNames;
	CComSafeArray <LONG> saTIDs;
	CComSafeArray <BSTR> saNames;

	/* Initializes a variant */
	VariantInit(&vtTIDs);
	VariantInit(&vtNames);

	/* 현재 CIMConnect 내부에 저장된 Trace 리스트 가져오기 */
	hResult	= m_pICxClientTool->ListTraces(conn_id, &vtTIDs, &vtNames);
	if (FAILED(hResult))
	{
		m_pLogData->CheckResult(hResult, L"GetListTraces::ListTraces");
		return FALSE;
	}
	/* Get access to the list of IDs, States, Texts */
	if (SUCCEEDED(hResult))	hResult = saTIDs.Attach(vtTIDs.parray);
	if (SUCCEEDED(hResult))	hResult = saNames.Attach(vtNames.parray);
	if (FAILED(hResult))
	{
		m_pLogData->CheckResult(hResult, L"GetRecipes::SafeArrary.Attach");
		return FALSE;
	}

	/* 현재 개수 구하기 */
	if ((lCount = GetSafeArrayCount(saTIDs, 1)) > 0)
	{
		m_pLogData->PushLogs(L"The epj_file was collected");
		/* get the arrary */
		hResult	= SafeArrayLock(saTIDs);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetListTraces (Trace ID)"))
		{
			hResult	= SafeArrayAccessData(saTIDs, (PVOID*)&pTIDs);
			if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetListTraces (Trace ID)"))
				hResult	= SafeArrayUnaccessData(saTIDs);
			hResult	= SafeArrayUnlock(saTIDs);
		}
		/* get the array */
		hResult	= SafeArrayLock(saNames);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetListTraces (Trace Name)"))
		{
			hResult	= SafeArrayAccessData(saNames, (PVOID*)&pNames);
			if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetListTraces (Trace Name)"))
				hResult	= SafeArrayUnaccessData(saNames);
			hResult	= SafeArrayUnlock(saNames);
		}

		/* Alarm ID와 State 등록  */
		for (i=0;SUCCEEDED(hResult) && i<lCount;i++)
		{
			t_ids.AddTail(pTIDs[i]);
			t_names.Add(pNames[i]);
		}
	}

	/* SafeArray 해제 */
	saTIDs.Detach();
	saNames.Detach();

	return TRUE;
}

/*
 desc : EPJ 파일에 등록된 variables 목록 반환
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		type	- [in]  Variable Type (E30_GVTC)
		ids		- [out] Variable IDs 반환
		names	- [out] Variable Names 반환
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetListVariables(LONG conn_id, E30_GVTC type, CAtlList <LONG> &ids, CStringArray &names)
{
	LONG i, lCount	= 0;
	LONG *pIDs		= NULL;
	HRESULT hResult	= S_OK;
	CComBSTR *pNames= NULL;
	VARIANT vtIDs, vtNames;
	CComSafeArray <LONG> saIDs;
	CComSafeArray <BSTR> saNames;

	/* Initializes a variant */
	VariantInit(&vtIDs);
	VariantInit(&vtNames);

	/* 현재 CIMConnect 내부에 저장된 Recipe 리스트 가져오기 */
	hResult	= m_pICxClientTool->ListVariables(conn_id, VarType(type), &vtIDs, &vtNames);
	if (FAILED(hResult))
	{
		m_pLogData->CheckResult(hResult, L"GetListVariables::ListVariables");
		return FALSE;
	}
	/* Get access to the list of IDs, States, Texts */
	if (SUCCEEDED(hResult))	hResult = saIDs.Attach(vtIDs.parray);
	if (SUCCEEDED(hResult))	hResult = saNames.Attach(vtNames.parray);
	if (FAILED(hResult))
	{
		m_pLogData->CheckResult(hResult, L"GetListVariables::SafeArrary.Attach");
		return FALSE;
	}

	/* 현재 개수 구하기 */
	if ((lCount = GetSafeArrayCount(saIDs, 1)) > 0)
	{
		m_pLogData->PushLogs(L"The epj_file was collected");
		/* get the arrary */
		hResult	= SafeArrayLock(saIDs);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetListVariables (ID)"))
		{
			hResult	= SafeArrayAccessData(saIDs, (PVOID*)&pIDs);
			if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetListVariables (ID)"))
				hResult	= SafeArrayUnaccessData(saIDs);
			hResult	= SafeArrayUnlock(saIDs);
		}
		/* get the array */
		hResult	= SafeArrayLock(saNames);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetListVariables (Name)"))
		{
			hResult	= SafeArrayAccessData(saNames, (PVOID*)&pNames);
			if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetListVariables (Name)"))
				hResult	= SafeArrayUnaccessData(saNames);
			hResult	= SafeArrayUnlock(saNames);
		}

		/* Alarm ID와 State 등록  */
		for (i=0; SUCCEEDED(hResult) && i<lCount; i++)
		{
			ids.AddTail(pIDs[i]);
			names.Add(pNames[i]);
		}
	}

	/* SafeArray 해제 */
	saIDs.Detach();
	saNames.Detach();

	return TRUE;
}

/*
 desc : List all messages (SECS-II 모든 메시지 반환)
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		ids		- [out] message IDs 반환
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetListMessageIDs(LONG conn_id, CAtlList <LONG> &ids)
{
	LONG i, lCount	= 0;
	LONG *pIDs		= NULL;
	HRESULT hResult	= S_OK;
	VARIANT vtIDs;
	CComSafeArray <LONG> saIDs;

	/* Initializes a variant */
	VariantInit(&vtIDs);

	/* 현재 CIMConnect 내부에 저장된 Recipe 리스트 가져오기 */
	hResult	= m_pICxClientTool->ListMessages(conn_id, &vtIDs);
	if (FAILED(hResult))
	{
		m_pLogData->CheckResult(hResult, L"GetListMessageIDs::ListMessages");
		return FALSE;
	}
	/* Get access to the list of IDs, States, Texts */
	if (SUCCEEDED(hResult))	hResult = saIDs.Attach(vtIDs.parray);
	if (FAILED(hResult))
	{
		m_pLogData->CheckResult(hResult, L"GetListMessageIDs::SafeArrary.Attach");
		return FALSE;
	}

	/* 현재 개수 구하기 */
	if ((lCount = GetSafeArrayCount(saIDs, 1)) > 0)
	{
		m_pLogData->PushLogs(L"The epj_file was collected");
		/* get the arrary */
		hResult	= SafeArrayLock(saIDs);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetListVariables (ID)"))
		{
			hResult	= SafeArrayAccessData(saIDs, (PVOID*)&pIDs);
			if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetListVariables (ID)"))
				hResult	= SafeArrayUnaccessData(saIDs);
			hResult	= SafeArrayUnlock(saIDs);
		}

		/* Message IDs 등록  */
		for (i=0; SUCCEEDED(hResult) && i<lCount; i++)	ids.AddTail(pIDs[i]);
	}

	/* SafeArray 해제 */
	saIDs.Detach();

	return TRUE;
}

/*
 desc : 연결된 통신 설정 정보 얻기
 parm : conn_id		- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		comm_pgid	- [out] The COM ProgID of the communications object
		size_pgid	- [in]  'comm_pgid'에 저장된 문자열 길이
		comm_sets	- [out] The communications settings
		size_sets	- [in]  'comm_sets'에 저장된 문자열 길이
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetCommSetupInfo(LONG conn_id, PTCHAR comm_pgid, UINT32 size_pgid,
							   PTCHAR comm_sets, UINT32 size_sets)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;
	CComBSTR bstrPgid, bstrSetting;

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetCommSetupInfo::GetCommIF (conn_id=%d)", conn_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxClientTool->GetCommIF(conn_id, &bstrPgid, &bstrSetting);
	/* 반환 버퍼에 복사 */
	swprintf_s(comm_pgid, size_pgid, L"%s", (LPCTSTR)_bstr_t(bstrPgid));
	swprintf_s(comm_sets, size_sets, L"%s", (LPCTSTR)_bstr_t(bstrSetting));

	return (hResult == S_OK);
}

/*
 desc : 연결될 통신 설정
 parm : conn_id		- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		comm_pgid	- [in]  The COM ProgID of the communications object
		comm_sets	- [in]  The communications settings
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SetCommSetupInfo(LONG conn_id, PTCHAR comm_pgid, PTCHAR comm_sets)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;
	CComBSTR bstrPgid, bstrSetting;

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetCommSetupInfo::SetCommIF (conn_id=%d)", conn_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxClientTool->SetCommIF(conn_id, CComBSTR(comm_pgid), CComBSTR(comm_sets));

	return (hResult == S_OK);
}

/*
 desc : 로그 저장 관련 환경 설정 값을 윈도 레지스트리에 저장할지 여부 설정
 parm : enable	- [in]  TRUE : 저장, FALSE : 저장 안함
		logs	- [in]  로그 저장 여부
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SetLoggingConfigSave(BOOL enable, BOOL logs)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetLoggingConfigSave::put_PersistLogging (enable=%d)", enable);
	if (logs)	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxClientTool->put_PersistLogging(VARIANT_BOOL(enable));
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : 로그 저장 관련 환경 설정 값의 저장 여부 값 반환
 parm : enable	- [out] TRUE : 저장하고 있음, FALSE : 저장 안하고 있음
		logs	- [in]  로그 저장 여부
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetLoggingConfigSave(BOOL &enable, BOOL logs)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;
	VARIANT_BOOL varEnable;

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetLoggingConfigSave::get_PersistLogging (enable=%d)", enable);
	if (logs)	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxClientTool->get_PersistLogging(&varEnable);
	m_pLogData->CheckResult(hResult, tzMesg);

	enable	= BOOL(varEnable);

	return (hResult == S_OK);
}

/*
 desc : SECS-II Message 송신 (전송) 즉, Primary Message 송신? 기능
 parm : conn_id	- [in]  1 or Later (공통 적용을 위해서는 0 값 사용)
		mesg_sid- [in]  SECS-II Message ID : Stream ID (0xssff -> 0x00xx ~ 0xffxx 까지)
		mesg_fid- [in]  SECS-II Message ID : Function ID (0xssff -> 0xxx00 ~ 0xxxFF 까지)
		term_id	- [in]  보통 0 값이나, 자신의 Terminal ID가 1 이상인 경우도 있다 (?)
		str_msg	- [in]  메시지 내용 (문자열), 만약 NULL이면 메시지 본문이 없는 경우임
		reply	- [in]  응답 기대 여부 (Host로부터 응답 받기를 원하는지 여부)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SendPrimaryMessage(LONG conn_id, UINT8 mesg_sid, UINT8 mesg_fid,
								 PTCHAR str_msg, UINT8 term_id, BOOL reply)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	VARIANT_BOOL varMsg	= VARIANT_FALSE;
	UINT16 u16MsgID		= 0x0000;
	LONG lTransID		= 0;
	HRESULT hResult		= S_OK;
	ICxValueDispPtr pICxValueDisp (CLSID_CxValueObject);

	/*
	S9F3	= Unknown Stream
	S9F5	= Unknown Function
	S9F7	= Illegal data
	S9F11	= Data too long
	S9F13	= Conversation timeout
	*/
	/* SECS-II Message ID 구성 */
	u16MsgID	= SFtoMsgID(mesg_sid, mesg_fid);
	if (u16MsgID == 0x0000)	return FALSE;

	/* Message Body 구성 */
	if (str_msg)
	{
		pICxValueDisp->AppendValueBinary(0, BYTE(term_id));
		pICxValueDisp->AppendValueAscii(0, CComBSTR (str_msg));
		varMsg	= VARIANT_TRUE;
	}

	/* Message 전달 */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE,
			   L"SendPrimaryMessage::StreamMsgID=%02d, FunctionMsgID=%02d", mesg_sid, mesg_fid);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxClientApp->CxSendMessage(conn_id, u16MsgID, pICxValueDisp,
											 varMsg, VARIANT_BOOL(reply), &lTransID, m_pICxEMAppCbk);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* 가장 최근에 송신한 Transaction ID 값 저장 */
	m_pSharedData->SetLastSECSMessageTransID(lTransID);

	return (hResult == S_OK);
}

/*
 desc : Application Name 반환
 parm : name	- [out] Application Name이 저장될 버퍼
		size	- [in]  'name' 버퍼 크기
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetAppName(PTCHAR name, UINT32 size)
{
	HRESULT hResult	= S_OK;
	CComBSTR bstrName;

	if (!m_pICxClientApp)	return FALSE;

	hResult	= m_pICxClientApp->get_appName(&bstrName);
	if (m_pLogData->CheckResult(hResult, L"GetAppName::get_appName"))	return FALSE;
	/* 값 반환 */
	swprintf_s(name, size, L"%s", (LPTSTR)_bstr_t(bstrName));

	return (hResult == S_OK);
}

/*
 desc : Application Name 설정
 parm : name	- [in]  Application Name이 저장된 버퍼
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SetAppName(PTCHAR name)
{
	HRESULT hResult	= S_OK;
	CComBSTR bstrName (name);

	if (!m_pICxClientApp)	return FALSE;

	hResult	= m_pICxClientApp->put_appName(bstrName);
	if (m_pLogData->CheckResult(hResult, L"GetAppName::put_appName"))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : 현재 프로젝트의 패키지가 빌드된 시점의 날짜 (시간 포함) 반환
 parm : date_time	- [out] 빌드된 시간 (날짜포함)이 저장될 버퍼
		size		- [in]  'data_time' 버퍼 크기
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetBuildDateTime(PTCHAR date_time, UINT32 size)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	DATE dtDateTime	= 0;
	COleDateTime oleDateTime;
	ICxEMServicePtr pEMSvc	= NULL;

	/* Variant 얻기 */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetBuildDateTime::GetBuildDateTime");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxVersion->GetBuildDateTime(&dtDateTime);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* MSDN DATE Type (https://docs.microsoft.com/en-us/cpp/atl-mfc-shared/date-type?view=vs-2019) */
	oleDateTime	= COleDateTime(dtDateTime);
	/* 시간 포맷 문자열로 반환 */
	swprintf_s(date_time, size, L"%4d-%02d-%02d %02d:%02d:%02d",
			   oleDateTime.GetYear(), oleDateTime.GetMonth(), oleDateTime.GetDay(),
			   oleDateTime.GetHour(), oleDateTime.GetMinute(), oleDateTime.GetSecond());

	return (hResult == S_OK);
}

/*
 desc : 현재 프로젝트의 패키지가 빌드된 시점의 Version 반환
 parm : version	- [out] 빌드된 시간 (날짜포함)이 저장될 버퍼
		size	- [in]  'version' 버퍼 크기
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetBuildVersion(PTCHAR version, UINT32 size)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrVer;
	ICxEMServicePtr pEMSvc	= NULL;

	/* Variant 얻기 */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetBuildVersion::GetVersionStr");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxVersion->GetVersionStr(&bstrVer);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 값 반환 */
	swprintf_s(version, size, L"%s", (LPTSTR)_bstr_t(bstrVer));

	return (hResult == S_OK);
}

/*
 desc : Get Package verion (for number)
 parm : major	- [out] Returned Major version revision number.
		minor	- [out] Returned Major version revision number.
		patch	- [out] Returned Patch number.
		build	- [out] Returned Build number.
 retn : TRUE or FALSE
 note : returns package version information
*/
BOOL CE30GEM::GetBuildVersion(LONG &major, LONG &minor, LONG &patch, LONG &build)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetVersion::GetVersion");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxVersion->GetVersion(&major, &minor, &patch, &build);
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : 임의 (특정) 장치에 연결되어 있는 응용 프로그램의 IDs와 이름들을 반환
 parm : equip_id- [in]  장비 ID (0, 1 or Later)
		ids		- [out] 응용 프로그램 IDs 반환 (ID 숫자 리스트로 반환)
		names	- [out] 응용 프로그램 Names 반환 (문자열 배열로 반환)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetListApplications(LONG equip_id, CAtlList <LONG> &ids, CStringArray &names)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG i, lCount	= 0, *pIDs	= NULL;
	HRESULT hResult	= S_OK;
	CComBSTR *pNames= NULL;
	VARIANT vtIDs;
	VARIANT vtNames;
	CComSafeArray <LONG> saIDs;
	CComSafeArray <BSTR> saNames;

	/* Variabt Init */
	VariantInit(&vtIDs);
	VariantInit(&vtNames);
	/* Variant 얻기 */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetListApplications::ListApplications (equip_id=%d)", equip_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxEMService->ListApplications(equip_id, &vtIDs, &vtNames);
	if (m_pLogData->CheckResult(hResult, L"GetListApplications::ListApplications"))	return FALSE;
	/* Get access to the list of IDs, States, Texts */
	if (SUCCEEDED(hResult))	hResult = saIDs.Attach(vtIDs.parray);
	if (SUCCEEDED(hResult))	hResult = saNames.Attach(vtNames.parray);
	if (m_pLogData->CheckResult(hResult, L"GetListApplications::SafeArrary.Attach"))	return FALSE;

	/* 개수 구하기 */
	lCount = GetSafeArrayCount(saIDs, 1);
	if (lCount < 1)	return FALSE;
	m_pLogData->PushLogs(L"The ListApplications was collected");
	/* get the id arrary */
	hResult	= SafeArrayLock(saIDs);
	if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetListApplications (IDs)"))
	{
		hResult	= SafeArrayAccessData(saIDs, (PVOID*)&pIDs);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetListApplications (IDs)"))
			hResult	= SafeArrayUnaccessData(saIDs);
		hResult	= SafeArrayUnlock(saIDs);
	}
	/* get the name array */
	hResult	= SafeArrayLock(saNames);
	if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetListApplications (Names)"))
	{
		hResult	= SafeArrayAccessData(saNames, (PVOID*)&pNames);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetListApplications (Names)"))
			hResult	= SafeArrayUnaccessData(saNames);
		hResult	= SafeArrayUnlock(saNames);
	}
	/* Application ID와 Names 등록  */
	for (i=0;SUCCEEDED(hResult) && i<lCount;i++)
	{
		ids.AddTail(pIDs[i]);
		names.Add(pNames[i]);
	}

	/* SafeArray 해제 */
	saIDs.Detach();
	saNames.Detach();

	return (hResult == S_OK);
}

/*
 desc : 지정된 장비에 구성된 호스트 연결 목록 반환
 parm : equip_id- [in]  장비 ID (0, 1 or Later)
		ids		- [out] HOST IDs 반환 (ID 숫자 리스트로 반환)
		names	- [out] HOST Names 반환 (문자열 배열로 반환)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetListConnections(LONG equip_id, CAtlList <LONG> &ids, CStringArray &names)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG i, lCount	= 0, *pIDs	= NULL;
	HRESULT hResult	= S_OK;
	CComBSTR *pNames= NULL;
	VARIANT vtIDs;
	VARIANT vtNames;
	CComSafeArray <LONG> saIDs;
	CComSafeArray <BSTR> saNames;

	/* Variabt Init */
	VariantInit(&vtIDs);
	VariantInit(&vtNames);
	/* Variant 얻기 */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetListConnections::ListConnections (equip_id=%d)", equip_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxEMService->ListConnections(equip_id, &vtIDs, &vtNames);
	if (m_pLogData->CheckResult(hResult, L"GetListConnections::ListConnections"))	return FALSE;
	/* Get access to the list of IDs, States, Texts */
	if (SUCCEEDED(hResult))	hResult = saIDs.Attach(vtIDs.parray);
	if (SUCCEEDED(hResult))	hResult = saNames.Attach(vtNames.parray);
	if (m_pLogData->CheckResult(hResult, L"GetListConnections::SafeArrary.Attach"))	return FALSE;

	/* 개수 구하기 */
	lCount = GetSafeArrayCount(saIDs, 1);
	if (lCount < 1)	return FALSE;
	m_pLogData->PushLogs(L"The ListConnections was collected");
	/* get the id arrary */
	hResult	= SafeArrayLock(saIDs);
	if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetListConnections (IDs)"))
	{
		hResult	= SafeArrayAccessData(saIDs, (PVOID*)&pIDs);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetListConnections (IDs)"))
			hResult	= SafeArrayUnaccessData(saIDs);
		hResult	= SafeArrayUnlock(saIDs);
	}
	/* get the name array */
	hResult	= SafeArrayLock(saNames);
	if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetListConnections (Names)"))
	{
		hResult	= SafeArrayAccessData(saNames, (PVOID*)&pNames);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetListConnections (Names)"))
			hResult	= SafeArrayUnaccessData(saNames);
		hResult	= SafeArrayUnlock(saNames);
	}
	/* Application ID와 Names 등록  */
	for (i=0;SUCCEEDED(hResult) && i<lCount;i++)
	{
		ids.AddTail(pIDs[i]);
		names.Add(pNames[i]);
	}

	/* SafeArray 해제 */
	saIDs.Detach();
	saNames.Detach();

	return (hResult == S_OK);
}

/*
 desc : EM Service에 연결되어 있는 Equipment의 IDs와 이름들을 반환
 parm : ids		- [out] Equipment IDs 반환 (ID 숫자 리스트로 반환)
		names	- [out] Equipment Names 반환 (문자열 배열로 반환)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetListEquipments(CAtlList <LONG> &ids, CStringArray &names)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG i, lCount	= 0, *pIDs	= NULL;
	HRESULT hResult	= S_OK;
	CComBSTR *pNames= NULL;
	VARIANT vtIDs;
	VARIANT vtNames;
	CComSafeArray <LONG> saIDs;
	CComSafeArray <BSTR> saNames;

	/* Variabt Init */
	VariantInit(&vtIDs);
	VariantInit(&vtNames);
	/* Variant 얻기 */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetListEquipments::ListEquipment");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxEMService->ListEquipment(&vtIDs, &vtNames);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* Get access to the list of IDs, States, Texts */
	if (SUCCEEDED(hResult))	hResult = saIDs.Attach(vtIDs.parray);
	if (SUCCEEDED(hResult))	hResult = saNames.Attach(vtNames.parray);
	if (m_pLogData->CheckResult(hResult, L"GetListEquipments::SafeArrary.Attach"))	return FALSE;

	/* 개수 구하기 */
	lCount = GetSafeArrayCount(saIDs, 1);
	if (lCount < 1)	return FALSE;
	m_pLogData->PushLogs(L"The ListEquipment was collected");
	/* get the id arrary */
	hResult	= SafeArrayLock(saIDs);
	if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetListEquipments (IDs)"))
	{
		hResult	= SafeArrayAccessData(saIDs, (PVOID*)&pIDs);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetListEquipments (IDs)"))
			hResult	= SafeArrayUnaccessData(saIDs);
		hResult	= SafeArrayUnlock(saIDs);
	}
	/* get the name array */
	hResult	= SafeArrayLock(saNames);
	if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayLock on GetListEquipments (Names)"))
	{
		hResult	= SafeArrayAccessData(saNames, (PVOID*)&pNames);
		if (!m_pLogData->CheckResult(hResult, L"Failed to call the SafeArrayAccessData on GetListEquipments (Names)"))
			hResult	= SafeArrayUnaccessData(saNames);
		hResult	= SafeArrayUnlock(saNames);
	}
	/* Application ID와 Names 등록  */
	for (i=0;SUCCEEDED(hResult) && i<lCount;i++)
	{
		ids.AddTail(pIDs[i]);
		names.Add(pNames[i]);
	}

	/* SafeArray 해제 */
	saIDs.Detach();
	saNames.Detach();

	return (hResult == S_OK);
}

/*
 desc : TS (Tracking System)에 등록된 Equipment Module (Location) Name 반환
 parm : index	- [in]  장비 명과 매핑된 인덱스 값 (1 based)
 retn : 이름 문자열 (실패의 경우 NULL or Empty String)
*/
PTCHAR CE30GEM::GetEquipLocationName(ENG_TSSI index)
{
	if (INT32(index) < 1 || m_arrEquipLocName.GetSize() < INT32(index))	return L"";
	return m_arrEquipLocName[INT32(index)-1].GetBuffer();
}

/*
 desc : TS (Tracking System)에 등록된 Processing Name 반환
 parm : index	- [in]  장비 명과 매핑된 인덱스 값 (!!! -1 based !!!)
 retn : 이름 문자열 (실패의 경우 NULL or Empty String)
*/
PTCHAR CE30GEM::GetEquipProcessingName(E90_SSPS index)
{
	INT32 i32Index	= INT32(index);
	if (i32Index < -1 || INT32(E90_SSPS::en_skipped) < i32Index)	return L"";
	if (i32Index == INT32(E90_SSPS::en_no_state))	i32Index	= INT32(E90_SSPS::en_skipped) + 1;
	return m_arrEquipProcName[INT32(i32Index)].GetBuffer();
}

/*
 desc : SECS-II Message 등록 헤재
 parm : None
 retn : TRUE or FALSE
*/
BOOL CE30GEM::ResetSecsMesgList()
{
	HRESULT hResult	= S_FALSE;

	/* 기존 등록된 메시지 모두 비우기 여부 */
	hResult	= m_pICxSECSMesg->Clear();

	/* Root Message 등록 */
	return (hResult == S_OK);
}

/*
 desc : SECS-II Message 등록 - Root Message
 parm : type	- [in]  Message Type
		value	- [in]  Message Type에 따른 값
		size	- [in]  'value' 버퍼 크기
 retn : TRUE or FALSE
*/
BOOL CE30GEM::AddSecsMesgRoot(E30_GPVT type, PVOID value, UINT32 size)
{
	/* 기존 등록된 메시지 모두 제거 */
	m_pICxSECSMesg->Clear();
	/* Root Message 등록 */
	return AddSecsMesgValue(0, type, value, size);
}

/*
 desc : SECS-II Message 등록 - Root Message
 parm : handle	- [in]  List가 추가될 상위 Parent ID
						맨 처음 추가될 List의 경우, 0 값
		clear	- [in]  Message 버퍼 모두 비우기 여부 (최초이면 TRUE, 중간이면 FALSE)
 retn : 새로 추가된 Handle 값 반환 (음수 값이면, 에러 처리)
*/
LONG CE30GEM::AddSecsMesgList(LONG handle, BOOL clear)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG lHandle	= -1;
	HRESULT hResult	= S_FALSE;

	/* 기존 등록된 메시지 모두 비우기 여부 */
	if (clear)	m_pICxSECSMesg->Clear();

	swprintf_s(tzMesg, L"AddSecsMesgList::handle = %d", handle);
	hResult	= m_pICxSECSMesg->AppendList(handle, &lHandle);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return -1;

	/* Root Message 등록 */
	return lHandle;
}


/*
 desc : 원격 장치 (Host?)에게 송신될 SECS-II Message 등록
 parm : handle	- [in]  Handle to a list within the object, or 0
		type	- [in]  'value' 값이 저장된 변수 형식
		value	- [in]  The value to append
		size	- [in]  'value' 버퍼에 저장된 데이터의 크기
 retn : TRUE or FALSE
*/
BOOL CE30GEM::AddSecsMesgValue(LONG handle, E30_GPVT type, PVOID value, UINT32 size)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult	= S_FALSE;

	/* 설정된 값의 속성 (Parameter Type)에 따라 등록 진행 */
	switch (type)
	{
	case E30_GPVT::en_i1	:
	case E30_GPVT::en_i2	:	hResult	= m_pICxSECSMesg->AppendValueI2(handle, *(PSHORT)value);				break;
	case E30_GPVT::en_i4	:
	case E30_GPVT::en_i8	:	hResult	= m_pICxSECSMesg->AppendValueI8(handle, *(PLONG)value);					break;

	case E30_GPVT::en_u1	:	hResult	= m_pICxSECSMesg->AppendValueU1(handle, *(PBYTE)value);					break;
	case E30_GPVT::en_u2	:	hResult	= m_pICxSECSMesg->AppendValueU2(handle, *(PSHORT)value);				break;
	case E30_GPVT::en_u4	:
	case E30_GPVT::en_u8	:	hResult	= m_pICxSECSMesg->AppendValueU8(handle, *(PLONG)value);					break;

	case E30_GPVT::en_f4	:	hResult	= m_pICxSECSMesg->AppendValueF4(handle, *(PFLOAT)value);				break;
	case E30_GPVT::en_f8	:	hResult	= m_pICxSECSMesg->AppendValueF8(handle, *(DOUBLE*)value);				break;

	case E30_GPVT::en_w		:
	case E30_GPVT::en_a		:	hResult	= m_pICxSECSMesg->AppendValueAscii(handle, CComBSTR((PTCHAR)value));	break;

	case E30_GPVT::en_b		:	hResult	= m_pICxSECSMesg->AppendValueBoolean(handle, *(VARIANT_BOOL*)value);	break;
	case E30_GPVT::en_bi	:	hResult	= m_pICxSECSMesg->AppendValueBinary(handle, *(PUINT8)value);			break;

	case E30_GPVT::en_any	:	break;	
	case E30_GPVT::en_list	:	break;
	case E30_GPVT::en_array	:	break;
	}

	/* 메시지 등록 성공 여부 */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"AddSecsMesgValue::AppendValue (mesg_type=%d)", type);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : 원격 장치 (Host?)에게 SECS-II Message 송신
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		mesg_sid- [in]  송신 대상의 SECS-II Message ID (Stream ID) (10진수)
		mesg_fid- [in]  송신 대상의 SECS-II Message ID (Function ID) (10진수)
		trans_id- [out] 송신 후 응답 받을 때, 동기화를 위해 필요한 ID (내가 보낸 물음에 응답임을 알기 위한 식별 ID) (0 값 초기화)
		is_reply- [in]  응답 (Ack) 요청 여부. TRUE - Host로부터 메시지 수신에 대한 응답 요청, FALSE - 응답 필요 없음
		is_body	- [in]  Message Body가 존재하는지 유무 즉, Message ID만 존재한다면 FALSE, 보낼 Message가 존재한다면 (TRUE)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SendSecsMessage(UINT8 conn_id, UINT8 mesg_sid, UINT8 mesg_fid,
							  BOOL is_reply, LONG &trans_id, BOOL is_body)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG lCount	= 0, lMesgID= SFtoMsgID(mesg_sid, mesg_fid);
	VARIANT_BOOL vbReply	= is_reply ? VARIANT_TRUE : VARIANT_FALSE;
	VARIANT_BOOL vbMessage	= VARIANT_FALSE;
	HRESULT hResult			= S_FALSE;
	CAtlList <STG_CLHC> lstItem;

	/* 만약 Stream 9 메시지 송신인 경우, 아래 조건에 맞지 않는 Function의 경우 에러 리턴 */
	if (mesg_sid == 9 && 
		mesg_fid != 3 && mesg_fid != 5 && mesg_fid != 7 && mesg_fid != 11 && mesg_fid != 13)
	{
		swprintf_s(tzMesg, CIM_CMD_DESC_SIZE,
				   L"SendSecsMessage::Invalid Function ID for Stream 9 "
				   "(conn_id=%d, mesg_id=%d,%d, is_reply=%d, transid=%d, is_body=%d)",
				   conn_id, mesg_sid, mesg_fid, is_reply, trans_id, is_body);
		m_pLogData->PushLogs(tzMesg, ENG_GALC::en_alarm);
		return FALSE;
	}
#if 0
	/* 재귀 함수 호출을 통해, 현재 수신된 SECS-II Message의 모든 리스트 정보 얻기 */
	ListAtItem(m_pICxSECSMesg, 0, 0, 0, lstItem);
#endif
	/* 본문 메시지가 없다면, 0x00010000 추가 */
	if (!is_body)	lMesgID	|= 0x10000;

	/* 본문 메시지 존재 여부에 따라 플래그 설정 */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE,
			   L"SendSecsMessage::ItemCount (conn_id=%d, mesg_id=%d,%d, is_reply=%d, transid=%d, is_body=%d)",
			   conn_id, mesg_sid, mesg_fid, is_reply, trans_id, is_body);
	hResult	= m_pICxSECSMesg->ItemCount(0, &lCount);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		return FALSE;
	}
	if (lCount > 0)	vbMessage	= VARIANT_TRUE;

	/* Transaction ID : 0 값 고정 */
	trans_id	= 0;

	/* 메시지 송신 (마지막 Callback은 반드시 NULL, RegisterMsgError 함수에 해당 관련 메시지 ID를 등록 했으므로) */
#if 1
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE,
			   L"SendSecsMessage::z (conn_id=%d, mesg_id=%d,%d, is_reply=%d, transid=%d, is_body=%d)",
			   conn_id, mesg_sid, mesg_fid, is_reply, trans_id, is_body);
	hResult= m_pICxClientApp->CxSendMessage(conn_id, lMesgID, m_pICxSECSMesg,
											vbMessage, vbReply, &trans_id, NULL);
#else
	LONG lReplyMesgID	= mesg_id+1, lErrorNumber, lExtra;
	ICxValueDispPtr pICxSECSReply(CLSID_CxValueObject);
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE,
			   L"SendSecsMessage::SyncSendMessage (conn_id=%d, mesg_id=%d,%d, is_reply=%d, transid=%d, is_body=%d)",
			   conn_id, mesg_sid, mesg_fid, is_reply, trans_id, is_body);
	hResult= m_pICxClientApp->SyncSendMessage(conn_id, lMesgID, m_pICxSECSMesg, vbMessage,
											  lReplyMesgID, &pICxSECSReply, &lErrorNumber, &lExtra);
#endif
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		return FALSE;
	}

	return (hResult == S_OK);
}

/*
 desc : ValueDisp에 저장된 List Data 가져오는 재귀 함수
 parm : mesg	- [in]  The message value object
		handle	- [in]  List Handle
		index	- [in]  List Index
		level	- [in]  Sub-Depth (Tab 깊이. XML 문법 연상하면 됨 <XML> ... </XML>)
		items	- [in]  List Data에 해당된 Handle과 Count 정보가 저장될 리스트 참조
 retn : TRUE or FALSE
*/
BOOL CE30GEM::ListAtItem(ICxValueDisp *mesg, LONG handle, LONG index, LONG level,
						 CAtlList <STG_CLHC> &item)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG l, lHandle	= -1L, lCount;
	HRESULT hResult	= S_OK;
	STG_CLHC stItem	= {NULL};

	/* List Handle 얻기 */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"CEMSvcCbk::ListAt (handle=%d)", handle);
	m_pLogData->PushLogs(tzMesg);
	hResult = mesg->ListAt(handle, index, &lHandle);
#if 0
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
#else
	if (lHandle < 0)	return FALSE;
#endif
	/* List Count 얻기 */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"CEMSvcCbk::ItemCount (lHandle=%d)", lHandle);
	m_pLogData->PushLogs(tzMesg);
	hResult = mesg->ItemCount(lHandle, &lCount);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* 만약 개수가 없다면 */
	if (lCount < 1)	return FALSE;

	/* 검색된 개수만큼, 다시 재귀함수 호출 (어렵다) */
	for (l=1; l<=lCount; l++)
	{
		/* 더 하위에 등록된 그룹 개수가 있으면, 리턴 해버림 */
		if (!ListAtItem(mesg, lHandle, l, level+1, item))
		{
			/* 더 이상 하위에 등록된 정보가 없다면, List에 현재 정보 등록 */
			stItem.handle	= lHandle;
			stItem.index	= l;
			stItem.level	= level;
			item.AddTail(stItem);
		}
	}

	return TRUE;
}