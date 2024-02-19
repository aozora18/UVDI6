
#include "pch.h"
#include "MainApp.h"
#include "E30GEM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 desc : ������
 parm : logs	- [in]  Log Object Pointer
		share	- [in]  Shared Data Object Pointer
		conn_cnt- [in]  Connection Counts (CONNECTION ����)
 retn : None
*/
CE30GEM::CE30GEM(CLogData *logs, CSharedData *share, LONG conn_cnt)
	: CEBase(logs, share)
{
	m_bIsInitConnected	= FALSE;
	m_bIsInitCompleted	= FALSE;
	m_bIsLoadProject	= FALSE;

	/* GUID ���� */
	m_gdICxE			= IID_ICxClientApplication;

	/* EPJ ���Ͽ� ��ϵ� Connection ���� */
	m_lConnectionCount	= conn_cnt;

	/* Tracking System�� ��ϵ� Module (Location) Name ���� */
	m_arrEquipLocName.Add(L"RobotArm");
	m_arrEquipLocName.Add(L"PreAligner");
	m_arrEquipLocName.Add(L"ProcessChamber");
	m_arrEquipLocName.Add(L"Buffer");
	m_arrEquipLocName.Add(L"LoadPort1");
	m_arrEquipLocName.Add(L"LoadPort2");

	/* Tracking System�� ��ϵ� Processing Name ���� */
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
 desc : �ı���
 parm : None
 retn : None
*/
CE30GEM::~CE30GEM()
{
	Close();

	/* �޸� ��ȯ */
	m_arrEquipLocName.RemoveAll();
	m_arrEquipProcName.RemoveAll();
}

/*
 desc : �ʱ�ȭ (�ݵ�� ȣ��)
 parm : equip_id	- [in]  Equipment ID (0 or later. ȯ�� ���Ͽ� ������ ����-1)
		conn_id		- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
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

	/* SECS-II Message Instance ���� */
	hResult	= m_pICxSECSMesg.CreateInstance(CLSID_CxValueObject);
	if (m_pLogData->CheckResult(hResult, L"Init::SECS-II Message. CreateInstance"))	return FALSE;


	return TRUE;
}

/*
 desc : ����
 parm : None
 retn : None
*/
VOID CE30GEM::Close()
{
	CloseE30GEM();

	/* SECS-II Message ���� */
	if (m_pICxSECSMesg.GetInterfacePtr())	m_pICxSECSMesg.Release();

	CEBase::Close();
}

/*
 desc : �ʱ�ȭ (�ݵ�� ȣ��)
 parm : equip_id	- [in]  Equipment ID (0 or later. ȯ�� ���Ͽ� ������ ����-1)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::InitE30GEM(LONG equip_id)
{
	HRESULT hResult	= S_FALSE;
	LONG lWaitResult, lAppID;

	/* �÷��� ���� �߿� */
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

	/* Smart Pointer ���� */
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

	/* �ϴ� �ʱ�ȭ ���� */
	m_bIsInitConnected	= TRUE;

	return TRUE;
}

/*
 desc : ����
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
 desc : ���� �ʱ�ȭ �÷��� ���� Reset ó��
 parm : None
 retn : None
*/
VOID CE30GEM::ResetInitFlag()
{
	/* �÷��� �߿� */
	m_bIsInitConnected	= FALSE;
	m_bIsInitCompleted	= FALSE;
	m_bIsLoadProject	= FALSE;
}

/*
 desc : ���� ����Ǿ� �ִ� ������Ʈ ������ ���Ϸ� ����
 parm : epj_file	- [in]  �����ϰ��� �ϴ� ���� �̸� (��ü ��� ����)
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
 desc : EMService�� ���� ������ ����
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
 desc : ������ �ʱ�ȭ �Ǿ����� ����
 parm : logs	- [in]  �α� ��� ����
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
 desc : ��� �ʱ�ȭ �Ǿ����� ����
 parm : logs	- [in]  �α� ��� ����
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
 desc : CIMConnect �ʱ�ȭ ���� �� �Ϸ� �۾� ����
 parm : equip_id	- [in]  Equipment ID (0 or later. ȯ�� ���Ͽ� ������ ����-1)
		conn_id		- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
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
		/* Default Communication State ���� Enable�̶��, �ʱ�ȭ ���� ������ Enable ��Ű�� �۾� ���� */
		if (256 == unValue.u32_value)
		{
			hResult	= m_pICxClientApp->EnableComm(i);
			if (FAILED(hResult))	m_pLogData->CheckResult(hResult, L"InitializeFinal::EnableComm");
		}

		/* Get/Set the control state */
		if (!GetVarNameToID(i, L"DefaultCtrlState", lID, TRUE))	return FALSE;
		if (!GetValue(i, lID /*DefaultCtrlState*/, E30_GPVT::en_u1, &unValue, 0, 0))	return FALSE;
		/* Default Control State ���� Online�̶��, �ʱ�ȭ ���� ������ Onlie-Remote ��Ű�� �۾� ���� */
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
	m_pSharedData->E30_PushState(E30_SSMS::en_commstate, conn_id, LONG(unValue.u32_value));	/* ���� �޸𸮿� ���� �� ����  */
	/* GEM Host Control Status */
	if (!GetValue(conn_id, 2028/* CONTROLSTATE */, E30_GPVT::en_u1, &unValue, 0, 0))	return FALSE;
	m_pSharedData->E30_PushState(E30_SSMS::en_control, conn_id, LONG(unValue.u8_value));		/* ���� �޸𸮿� ���� �� ����  */

	if (!SetUpdateProcessState(E30_GPSV::en_idle))	return FALSE;	/* Idle */

	/* �ʱ�ȭ �Ϸ� �̺�Ʈ ���� ���� */
	m_bIsInitCompleted	= TRUE;

	return TRUE;
}

/*
 desc : EPJ ���� ���� Ȯ�� (�˻�)
 parm : epj_file	- [in]  EPJ ���� (��ü ���)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::LoadProject(PTCHAR epj_file)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	INT32 i32LastBackslash			= 0;
	LONG lWaitResult				= 0;
	HRESULT hResult					= NULL;
	CString strPrjCurrent, strPrjDesired, strPrjDefault;
	CComBSTR bstrCurPrj, bstrTgtPrj (epj_file)	/* ����� Project File (EPJ) */;
	CUniToChar csCnv;

	if (!m_pICxClientTool)	return FALSE;
	/* Make sure the correct EPJ file is loaded */
	m_pLogData->PushLogs(L"ICxClientTool::GetCurrentProject");
	hResult = m_pICxClientTool->GetCurrentProject(&bstrCurPrj);	/* ���� ����� Project File (EPJ) ��� */
	if (FAILED(m_pLogData->CheckResult(hResult, L"ICxClientTool::GetCurrentProject")))	return FALSE;

	/* ���� ������Ʈ ���ϰ� ���� ������Ʈ ������ �����ϴ���, EPJ ������ �ٲ��, ���� �۾� �ٽ� ���� */
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

	/* Check the default EPJ file (���� ������Ʈ�� ���� �Էµ� ������Ʈ�� �ٸ� ��� ���� Ȯ��) */
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

	/* �̺�Ʈ (�ݹ�) ID ��� */
	if (!RegisterCommonID())	return FALSE;
	if (!RegisterCommandID())	return FALSE;
	/* ������Ʈ ���� ����  */
	m_bIsLoadProject	= TRUE;

	return TRUE;
}

/*
 desc : ��ġ�� ���ǵ� ��� ������ �׸���� ���� (�����ؼ� ����ؾ� ��)
 parm : logs	- [in]  �α� ��� ����
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
 desc : ���� �۾� �߿� �ִ��� ����... ��, Busy �������� ����
 parm : time_out	- [in]  �ִ� ��� �ð� (����: msec)
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
 desc : �ݹ� �Լ� ��� (Value Change ID)
		EPJ ������Ʈ ���� ���� ��ϵ� Event ID �� �� ���濡 ����, PCIMLib���� �˷��� �ʿ䰡 �ִ� ���
		�ش� Event ID �� ����� ��� ��
 parm : lst_val	- [in]  �̺�Ʈ�� ����Ϸ��� ������ ����� ����Ʈ ����
 retn : TRUE or FALSE
*/
BOOL CE30GEM::RegisterValueChangeID(CAtlList <STG_TVCI> *lst_val)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE];
	UINT32 i	= 0;
	STG_TVCI stData;

	for (; i<lst_val->GetCount(); i++)
	{
		/* �̺�Ʈ ��� ���� 1�� ���� */
		stData	= lst_val->GetAt(lst_val->FindIndex(i));
		/* Log ����  */
		wmemset(tzMesg, 0x00, CIM_CMD_DESC_SIZE);
		swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"RegisterValueChangedHandler (%s:%I32u)",
				   stData.var_name, stData.var_id);
		/* �̺�Ʈ ��� */
		if (!RegisterValueChangeID(stData.conn_id, stData.var_id, tzMesg))	return FALSE;
	}

	return TRUE;
}

/*
 desc : �ݹ� �Լ� ��� (Value Change ID)
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		var_id	- [in]  Variable ID
		mesg	- [in]  ��µ� �α� �޽���
 retn : TRUE or FALSE
*/
BOOL CE30GEM::RegisterValueChangeID(LONG conn_id, LONG var_id, PTCHAR mesg)
{
	HRESULT hResult		= S_OK;
	LPM_REHI pstEvent	= NULL;

	m_pLogData->PushLogs(mesg);
	hResult = m_pICxClientApp->RegisterValueChangedHandler(conn_id, var_id, m_pICxEMAppCbk);
	if (FAILED(m_pLogData->CheckResult(hResult, mesg)))	return FALSE;

	/* ��ϵ� �̺�Ʈ ���� */
	pstEvent	= (LPM_REHI)::Alloc(sizeof(STM_REHI));
	ASSERT(pstEvent);
	pstEvent->conn_id	= conn_id;
	pstEvent->evt_id	= var_id;
	m_lstEvtValC.AddTail(pstEvent);

	return (hResult == S_OK);
}

/*
 desc : �ݹ� �Լ� ���� (Value Change ID)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CE30GEM::UnregisterValueChangeID()
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;
	POSITION pPos		= NULL;
	LPM_REHI pstEventID	= NULL;

	/* ���̺귯�� ���� Ȥ�� �ܺ� (GUI)���� ��ϵ� Callback Event for ID ���� */
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
 desc : �ݹ� �Լ� ��� (Get Value ID)
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		var_id	- [in]  Variable ID
		mesg	- [in]  ��µ� �α� �޽���
 retn : TRUE or FALSE
*/
BOOL CE30GEM::RegisterGetValueID(LONG conn_id, LONG var_id, PTCHAR mesg)
{
	HRESULT hResult		= S_OK;
	LPM_REHI pstEvent	= NULL;

	m_pLogData->PushLogs(mesg);
	hResult = m_pICxClientApp->RegisterGetValueHandler(conn_id, var_id, m_pICxEMAppCbk);
	if (FAILED(m_pLogData->CheckResult(hResult, mesg)))	return FALSE;

	/* ��ϵ� �̺�Ʈ ���� */
	pstEvent	= (LPM_REHI)::Alloc(sizeof(STM_REHI));
	ASSERT(pstEvent);
	pstEvent->conn_id	= conn_id;
	pstEvent->evt_id	= var_id;
	m_lstEvtValG.AddTail(pstEvent);

	return (hResult == S_OK);
}

/*
 desc : �ݹ� �Լ� ���� (Get Value ID)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CE30GEM::UnregisterGetValueID()
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult		= S_OK;
	POSITION pPos		= NULL;
	LPM_REHI pstEventID	= NULL;

	/* ���̺귯�� ���� Ȥ�� �ܺ� (GUI)���� ��ϵ� Callback Event for ID ���� */
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
 desc : �ݹ� �Լ� ��� (Command)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CE30GEM::RegisterCommandID()
{
	/* ----------------------------------------------------------------------------------------- */
	/* Register for Remote Commands (Remote Command name�� �� ���̰� 20 string�� ���� �ʾƾ� ��) */
	/* �����, ��� ���� ���� (Host) ��ɾ�� �빮�ڷ� ����, Argument�� ��κ� Zero ������ ó��. */
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
 desc : �ݹ� �Լ� ��� (Command)
 parm : cmd		- [in]  ��ϵ� ��� ���ڿ�
		desc	- [in]  ��ϵ� �޽���
		mesg	- [in]  ��µ� �α� �޽���
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

	/* ��ϵ� �̺�Ʈ ���� */
	pstEvent	= (LPM_RECI)::Alloc(sizeof(STM_RECI));
	ASSERT(pstEvent);
	pstEvent->cmd	= (PTCHAR)::Alloc(sizeof(TCHAR) * CIM_CMD_NAME_SIZE+sizeof(TCHAR));
	wmemset(pstEvent->cmd, 0x00, CIM_CMD_NAME_SIZE+1);
	wcscpy_s(pstEvent->cmd, CIM_CMD_NAME_SIZE, bstrCmd);
	m_lstEvtCmds.AddTail(pstEvent);

	return (hResult == S_OK);
}

/*
 desc : �ݹ� �Լ� ���� (Command)
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

	/* ���̺귯�� ���� Ȥ�� �ܺ� (GUI)���� ��ϵ� Callback Event for ID ���� */
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
 desc : �ݹ� �Լ� ��� (Common or Others)
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

	/* ��� �˶��� ���� ���濡 ���� �̺�Ʈ ���� ��� */
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
 desc : �ݹ� �Լ� ���� (Common or Others)
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
	/* ��� �˶��� ���� ���濡 ���� �̺�Ʈ ���� ��� */
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
 desc : SECS-II �޽��� ��� ��� (Message Handler & Message Sent)
 parm : lst_mesg	- [in]  SECS-II Message ������ ����� ����Ʈ ������
 retn : TRUE or FALSE
*/
BOOL CE30GEM::RegisterMessageID(CAtlList <STG_TSMI> *lst_mesg)
{
	UINT32 i	= 0;
	STG_TSMI stData;

	for (; i<lst_mesg->GetCount(); i++)
	{
		/* �̺�Ʈ ��� ���� 1�� ���� */
		stData	= lst_mesg->GetAt(lst_mesg->FindIndex(i));
		/* �̺�Ʈ ��� */
		if (!RegisterMessageID(stData.conn_id, stData.s_id, stData.f_id, stData.mesg_desc))
			return FALSE;
	}

	return TRUE;
}

/*
 desc : �ݹ� �Լ� ��� (Message Handler & Message Sent)
 parm : conn_id	- [in]  ����� ��� ID
		s_id	- [in]  Stream ID
		f_id	- [in]  Function ID
		mesg	- [in]  �޽��� ���� (����, �α� ��� ��)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::RegisterMessageID(LONG conn_id, UINT8 mesg_sid, UINT8 mesg_fid, PTCHAR mesg)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG i = 1, lMsgID	= 0x0000;
	HRESULT hResult		= S_OK;
	POSITION pPos		= NULL;
	LPM_REHI pstEvent	= NULL;

	/* 16���� �޽��� ID ��� */
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
	if (mesg_fid)	/* Function ID ���� 0 �� �ƴ� ��츸 ��� ���� */
	{
		swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"RegisterMsgEventID::RegisterMsgError (%s) "
											  L"(conn_id=%d, mesg_id=0x%04x)",
				   mesg, conn_id, lMsgID);
		m_pLogData->PushLogs(tzMesg);
		hResult = m_pICxClientApp->RegisterMsgError(conn_id, lMsgID, m_pICxEMAppCbk);
		if (FAILED(m_pLogData->CheckResult(hResult, tzMesg)))	return FALSE;
	}
	/* ��ϵ� �̺�Ʈ ���� */
	pstEvent	= (LPM_REHI)::Alloc(sizeof(STM_REHI));
	ASSERT(pstEvent);
	pstEvent->conn_id	= conn_id;
	pstEvent->evt_id	= lMsgID;
	m_lstEvtMesg.AddTail(pstEvent);

	return (hResult == S_OK);
}

/*
 desc : �ݹ� �Լ� ���� (Message Handler & Message Sent)
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
			/* �޸� ���� */
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
 parm : state	- [in]  ���� ���� (���� ���� : 0 (Setup), 1 (Idle), 2 (executing))
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
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		term_id	- [in]  Terminal ID (0 or Later)
		mesg	- [in]  The text message
 retn : TRUE or FALSE
 ���� : This method requests that a terminal message to be sent to a connection
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
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
 retn : TRUE or FALSE
 ���� : This method triggers an equipment well-known event
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
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
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
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		name	- [in]  Name of the command
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID;1 or 2)
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
 desc : Variable ID�� �ش�Ǵ� Value Type (E30_GPVT) �� ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
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
 desc : Variable ID�� �ش�Ǵ� Variable Type (E30_GVTC) �� ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
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
 desc : ���� Variable�� Value �� ���
 parm : conn_id	- [in]  ����� ��� ID, Common�� ��� 0 ��
		var_id	- [in]  Variable ID�� ���εǴ� Name
		value	- [out] Value ���� ����Ǵ� ���� ������
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID; 1 or 2)
		index	- [in]	�Ϲ������� 0 ��������, ����Ʈ�� ��� Slot ID (Wafer ID; 1 ~ Max)
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
 desc : ���� Variable�� Value �� ���
 parm : conn_id	- [in]  ����� ��� ID, Common�� ��� 0 ��
		var_id	- [in]  Variable ID
		val_type- [in]  Value Type
		value	- [out] Value ���� ����Ǵ� ���� ������
		size	- [in]  'value' ���� ũ��
		handle	- [in]  �Ϲ������� 0 ��������, ����Ʈ�� ��� Carrier ID (FOUP ID; 1 or 2)
		index	- [in]	�Ϲ������� 0 ��������, ����Ʈ�� ��� Slot ID (Wafer ID; 1 ~ Max)
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

	/* Value Object ��� */
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

	/* ����� �� �ӽ� ���ۿ� ������ ���� */
	hResult	= pCxValueDisp->RestoreFromByteStream(0, 0, vValue);
	if (S_OK != m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* ���� Handle�� �ش�Ǵ� ��ϵ� ���� ������ �ִ��� ���� */
	hResult	= pCxValueDisp->ItemCount(handle, &lCount);
	if (S_OK != m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* Value Type �� ���� ��û */
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
 desc : ���� Variable�� Value �� ���
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		var_id	- [in]  Variable ID
		value	- [out] Value ���� ����Ǵ� ���� ������
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
 desc : ���� ��� ���°� Enable���� ����
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
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
 desc : ���� ��� ���°� Communicating���� ����
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
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
 desc : ���� ���� ���°� Online ������� ����
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
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
 desc : ���� ���� ���°� Remote ������� ����
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
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
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		enable	- [in]  TRUE or FALSE
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SetCommunication(LONG conn_id, BOOL enable)
{
	TCHAR tzMesg[CIM_LOG_MESG_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;

	/* ���� ��� ���°� Enable���� ���� */
	if (enable == IsCommEnabled(conn_id, FALSE))	return TRUE;

	/* ���� ��� : Disable or Enable ó�� */
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
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		state	- [in]  if true, go ONLINE. if false, go OFFLINE
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SetControlOnline(LONG conn_id, BOOL state)
{	 
	TCHAR tzState[2][32]= { L"GoOffline", L"GoOnline" };
	HRESULT hResult;

	/* ���� ��� ���°� Enable���� ���� */
	if (!IsCommEnabled(conn_id))	return FALSE;
	/* Get the online state */
	UINT8 u8Value = 0;
	if (!GetValueU1(conn_id, 2028 /* CONTROLSTATE */, &u8Value))	return FALSE;
	/* ���� Host ���� Equipment�� Online �������� / Offline �������� Ȯ�� */
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
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		state	- [in]  if true, go REMOTE. if false, go LOCAL
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SetControlRemote(LONG conn_id, BOOL state)
{
	TCHAR tzState[2][32]	= { L"GoLocal", L"GoRemote" };
	HRESULT hResult;

	/* ���� ��� ���°� Enable���� ���� */
	if (!IsCommEnabled(conn_id))	return FALSE;
	/* Get the online state */
	UINT8 u8Value = 0;
	if (!GetValueU1(conn_id, 2028 /* CONTROLSTATE */,  &u8Value))	return FALSE;
	/* ���� Host�� ���ӵ��� ���� ���¶��, �ƿ� ���� False ó�� */
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
	/* ������ ��� �ϰ� �Ǹ�, ���� ���ϵ��� ���� */
	if ((u8Value == 4 && !state) || (u8Value == 5 && state))
	{
		m_pLogData->PushLogs(L"SetControlStateRemote::It is the same the previously set command", ENG_GALC::en_warning);
		return FALSE;
	}

	/* ���� */
	m_pLogData->PushLogs(L"SetControlRemote", tzState[state]);
	if (state)	hResult	= m_pICxClientApp->GoRemote(conn_id);
	else		hResult	= m_pICxClientApp->GoLocal(conn_id);
	/* �α� ��� */
	if (state)	m_pLogData->CheckResult(hResult, L"SetControlRemote::GoRemote");
	else		m_pLogData->CheckResult(hResult, L"SetControlRemote::GoLocal");

	return (hResult == S_OK);
}

/*
 desc : Spooling State ����
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		enable	- [in]  Enable (TRUE) or Disable (FALSE)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SetSpoolPurge(LONG conn_id)
{
	HRESULT hResult	= S_OK;
	if (conn_id < 1)	return -1L;

	/* ���� ��� ���°� Enable���� ���� */
	if (!IsCommEnabled(conn_id))	return FALSE;
	/* Run the Purge */
	hResult	= m_pICxHostEmulator->PurgeSpool(conn_id);
	m_pLogData->CheckResult(hResult, L"SetPurgeSpool::Set Purge");

	return (hResult == S_OK);
}

/*
 desc : Spooling State ����
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		enable	- [in]  Enable (TRUE) or Disable (FALSE)
 retn : TRUE or FALSE
 note : Be aware that a connection may not support spooling
*/
BOOL CE30GEM::SetSpoolState(LONG conn_id, BOOL enable)
{
	HRESULT hResult	= S_OK;
	if (conn_id < 1)	return -1L;

	/* ���� ��� ���°� Enable���� ���� */
	if (!IsCommEnabled(conn_id))	return FALSE;
	/* ���� ��� : Disable or Enable ó�� */
	if (enable)	hResult	= m_pICxClientApp->EnableSpooling(conn_id);
	else		hResult	= m_pICxClientApp->DisableSpooling(conn_id);

	m_pLogData->CheckResult(hResult, L"SetStateSpool::Enable/Disable Spooling");

	return TRUE;
}

/*
 desc : Spooling Overwrite ����
 parm : conn_id		- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		overwrite	- [in]  Overwrite (TRUE) or Not overwrite (FALSE)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SetSpoolOverwrite(LONG conn_id, BOOL overwrite)
{
	HRESULT hResult;
	if (conn_id < 1)	return FALSE;

	/* ���� ��� ���°� Enable���� ���� */
	if (!IsCommEnabled(conn_id))	return FALSE;
	/* Run the SpoolOverwriting */
	hResult	= m_pICxClientApp->SpoolingOverwrite(conn_id, overwrite);
	if (FAILED(hResult))	m_pLogData->CheckResult(hResult, L"SetOverwriteSpool::SpoolingOverwrite");

	return (hResult == S_OK);
}

/*
 desc : Enables or Disables alarm reporting for a host for the specified alarms.
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		alarm_id- [in]  Connection Variables or Alarms�� Section�� ������ variables identification number
		state	- [in]  TRUE (ENABLE) or FALSE (DISABLE)
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SetAlarmsEnabled(LONG conn_id, LONG alarm_id, BOOL state)
{
	TCHAR tzState[2][16]= { L"Disabled", L"Enabled" };
	LONG lCtrlRemote	= 0;
	HRESULT hResult;

	/* ���� ��� ���°� Enable���� ���� */
	if (!IsCommEnabled(conn_id))	return FALSE;
	/* ���� */
	m_pLogData->PushLogs(L"SetAlarmsEnabled", tzState[state]);
	if (state)	hResult	= m_pICxHostEmulator->EnableAlarm(conn_id, alarm_id);
	else		hResult	= m_pICxHostEmulator->DisableAlarm(conn_id, alarm_id);
	/* �α� ��� */
	if (state)	m_pLogData->CheckResult(hResult, L"SetAlarmsEnabled::Enabled");
	else		m_pLogData->CheckResult(hResult, L"SetAlarmsEnabled::Disabled");

	return (hResult == S_OK);
}

/*
 desc : ���� ��� (�߻�)�� �˶� ID ���� ��ȯ
 parm : id		- [out] �˶� ����Ʈ�� ����� CAtlList <LONG> : �˶� ID
		state	- [out] �˶� ����Ʈ�� ����� CAtlList <UINT8>: �˶� ���� (0 or 1)
		alarm	- [in]  �˶��� �߻��� ������ ��ȯ ���� (FLALSE:  ���, TRUE: �˶��� �߻��� �͸�)
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

	/* ���� CIMConnect ���ο� �߻��� �˶� ����Ʈ �������� */
	hResult	= m_pICxClientTool->ListAlarmsStates(&vtIDs, &vtStates, &vtTexts);
	if (m_pLogData->CheckResult(hResult, L"ClearAllAlarms::ListAlarmsStates"))	return FALSE;
	/* Get access to the list of IDs, States, Texts */
	if (SUCCEEDED(hResult))	hResult = saIDs.Attach(vtIDs.parray);
	if (SUCCEEDED(hResult))	hResult = saStates.Attach(vtStates.parray);
	if (m_pLogData->CheckResult(hResult, L"ClearAllAlarms::SafeArrary.Attach"))	return FALSE;

	/* ���� �߻�(?)�� �˶��� ���� ���ϱ� */
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

		/* Alarm ID�� State ���  */
		for (i=0;SUCCEEDED(hResult) && i<lCount;i++)
		{
			/* �˶� �߻� ���ο� ���� */
			if (alarm && 0 != pStates[i])
			{
				ids.AddTail(pIDs[i]);
				states.AddTail(pStates[i]);
			}
		}
	}

	/* SafeArray ���� */
	saIDs.Detach();
	saStates.Detach();

	return TRUE;
}

/*
 desc : ���� �˶� ���� �� ��ȯ
 parm : id		- [in]  �˶� ID
		state	- [out] �˶� ���� ��ȯ (1 : Set, 0 : Clear)
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
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
 desc : ���� CIMConnect ���ο� �߻��� �˶� ��� ����
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

	/* ���� CIMConnect ���ο� �߻��� �˶� ����Ʈ �������� */
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

	/* ���� �߻�(?)�� �˶��� ���� ���ϱ� */
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
		/* �� ��������  */
		for (i=0; SUCCEEDED(hResult) && i<lCount; i++)
		{
			/* �˶� �̺�Ʈ ���� */
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

	/* SafeArray ���� */
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
 desc : CIMConnect ���η� ������ �˶� �߻�
 parm : var_id	- [in]  Alarm ID ��, EPJ Variables ID�� ���� ��
 retn : TRUE or FALSE
*/
BOOL CE30GEM::AlarmSet(LONG var_id)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG lState					= 0;
	CComBSTR bstrEmpty;
	HRESULT hResult	= S_OK;

	/* ���� �����ϰ��� �ϴ� �˶� ���� �� ��ȯ */
	hResult	= m_pICxClientApp->GetAlarmState(&var_id, &bstrEmpty, &lState);
	if (FAILED(hResult))
	{
		m_pLogData->CheckResult(hResult, L"ClearAlarm::GetAlarmState");
		return FALSE;
	}
	/* �˶��� �߻��� ������ ��� */
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
 desc : ICxValueDisp�� AppendValue Ȯ��
 parm : disp	- [in]  ICxValueDisp
		value	- [in]  �� ����� ������ ����
		type	- [in]  'value' type (U1, U2, U4, Bo, I1, I3, ...)
 retn : TRUE or FALSE
 note : ��, list, jis_8_string ���� ����
*/
BOOL CE30GEM::SetDispAppendValue(ICxValueDisp *disp, LNG_CVVT value, E30_GPVT type)
{
	HRESULT hResult		= S_OK;
	CComBSTR bstrValue(value->s_value);

	/* Value ��� */
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
 desc : Event �߻� (Method to trigger an event)
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
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
 desc : Trigger Event Data �߻� (�θ𿡰� �˸�) - 1 �� ���� ���� ����
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		evt_id	- [in]  Event Index (In EPJ File)
		var_id	- [in]  Variable Index (DV) (In EPJ File)
		value	- [in]  ��
		type	- [in]  'value' type (U1, U2, U4, Bo, I1, I3, ...)
 retn : TRUE or FALSE
 note : ��, list, jis_8_string ���� ����
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
	/* Event && Variable ID�� �ش�Ǵ� Name ��� */
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
	/* Data Value �ν��Ͻ� ���� */
	hResult	= dispDataValues.CreateInstance(CLSID_CxValueObject);
	if (S_OK == m_pLogData->CheckResult(hResult, L"SetTrigEventValue::dataValue.CreateInstance"))
	{
		if (SetDispAppendValue(dispDataValues, value, type))
		{
			/* Name ��� */
			arrDataNames.Add(CComBSTR(tzVarName));
			/* Trigger Event ��� */
			bSucc	= SendCollectionEventWithData(conn_id, arrDataNames, dispDataValues, evt_id, 0 /* fixed */);
			/* ���� ����� �޸� ȸ�� */
			dispDataValues->Clear();
			arrDataNames.Destroy();
		}
	}
	/* Data Value �ν��Ͻ� ���� */
	dispDataValues.Release();

	return bSucc;
}

/*
 desc : Trigger Event Data �߻� (�θ𿡰� �˸�) - n �� ���� ���� ����
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		evt_id	- [in]  Event Index (In EPJ File)
		var_ids	- [in]  Variable ID�� ����� ����Ʈ
		values	- [in]  ���� ����� ����Ʈ
		types	- [in]  'value' type�� ����� ����Ʈ (U1, U2, U4, Bo, I1, I3, ...)
 retn : TRUE or FALSE
 note : ��, list, jis_8_string ���� ����
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

	/* Data Value �ν��Ͻ� ���� */
	hResult	= dispDataValues.CreateInstance(CLSID_CxValueObject);
	if (S_OK != m_pLogData->CheckResult(hResult, L"SetTrigEventValues::dataValues.CreateInstance"))	return FALSE;
	/* Value ��� */
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
		/* Name ��� */
		if (bSucc)	arrDataNames.Add(CComBSTR(tzVarName));
	}
	/* Trigger Event ��� */
	if (bSucc)	bSucc	= SendCollectionEventWithData(conn_id, arrDataNames, dispDataValues, evt_id, 0 /* fixed */);
	/* ���� ����� �޸� ȸ�� */
	arrDataNames.Destroy();
	/* Data Value �ν��Ͻ� ���� */
	dispDataValues->Clear();
	dispDataValues.Release();

	return bSucc;
}

/*
 desc : Trigger Event Data �߻� (�θ𿡰� �˸�) - ���� �� ���� ���� ����
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		evt_id	- [in]  Event Index (In EPJ File)
		var_id	- [in]  Variable Index (DV) (In EPJ File)
		values	- [in]  ���� ����� ����Ʈ
		types	- [in]  'value' type�� ����� ����Ʈ (U1, U2, U4, Bo, I1, I3, ...)
 retn : TRUE or FALSE
 note : ��, jis_8_string ���� ����
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

	/* Event && Variable ID�� �ش�Ǵ� Name ��� */
	if (!GetVarIDToName(conn_id, var_id, tzVarName, CIM_CMD_NAME_SIZE))	return FALSE;

	/* Data Value �ν��Ͻ� ���� */
	hResult	= dispDataValues.CreateInstance(CLSID_CxValueObject);
	if (S_OK != m_pLogData->CheckResult(hResult, L"SetTrigEventValue::dataValues.CreateInstance"))	return FALSE;
	/* Get the handle */
	hResult	= dispDataValues->AppendList(0, &lHandle);
	if (S_OK == m_pLogData->CheckResult(hResult, L"SetTrigEventValue::dataValues->AppendList"))
	{
		/* Value ��� */
		for (i=0;i<values.GetCount() && bSucc;i++)
		{
			/* Data Value */
			unValueV= values.GetAt(values.FindIndex(i));
			/* Data Type */
			enType	= types.GetAt(types.FindIndex(i));
			bSucc	= SetDispAppendValue(dispDataValues, &unValueV, enType);
		}
		/* Name ��� */
		arrDataNames.Add(CComBSTR(tzVarName));
		/* Trigger Event ��� */
		if (bSucc)	bSucc	= SendCollectionEventWithData(conn_id, arrDataNames,
														  dispDataValues, evt_id, 0 /* fixed */);
		/* ���� ����� �޸� ȸ�� */
		arrDataNames.Destroy();
	}
	/* Data Value �ν��Ͻ� ���� */
	dispDataValues->Clear();
	dispDataValues.Release();

	return bSucc;
}

/*
 desc : �ð� ����ȭ
 parm : t_secs	- [in]  ���� ��ǻ�� (���)�� �ð��� �ʷ� ȯ���� �� (1970.01.01 ���ĺ��� ����� �ð�)
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
 desc : ��� ���� �����ϴ� �˶� ID ����
 parm : id	- [in]  �����ϰ��� �ϴ� �˶� ID
		logs- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
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
 desc : ��� ��û���� ������ (���?)�� ����
 parm : logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
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
 desc : �˶� �̸��� �ش�Ǵ� �˶� ID �� ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		name	- [in]  �˶� �̸�
		id		- [out] �˶� ID ��ȯ
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
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
 desc : ��� ���� (Low Level Communicating state) �� ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		state	- [out] ��� ����
		substate- [out] HSMS (Ethernet) ���� ����
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
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

	/* �� ��ȯ */
	state	= E30_GCSS(lState);
	substate= E30_GCSH(lSubState);

	return (hResult == S_OK);
}

/*
 desc : Host ���ῡ ���� �̸� ��, connection id�� ���εǴ� �̸� ���
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		name	- [out] ����� Host Name �� ���
		size	- [in]  'name' ���� ũ��
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
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

	/* �� ��ȯ */
	bstName	= _bstr_t(bstrName);

	/* ��ȯ ���� �ʱ�ȭ �� �� ���� */
	wmemset(name, 0x00, size);
	wmemcpy_s(name, size, (LPCTSTR)bstName, bstName.length());

	return (hResult == S_OK);
}

/*
 desc : ���� ����� Project Name ���
 parm : name	- [out] Project Name �� ���
		size	- [in]  'name' ���� ũ��
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
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

	/* �� ��ȯ */
	bstName	= _bstr_t(bstrName);

	/* ��ȯ ���� �ʱ�ȭ �� �� ���� */
	wmemset(name, 0x00, size);
	wmemcpy_s(name, size, (LPCTSTR)bstName, bstName.length());

	return (hResult == S_OK);
}

/*
 desc : �⺻ ������ Default Project Name ���
 parm : name	- [out] Project Name �� ���
		size	- [in]  'name' ���� ũ��
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
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

	/* �� ��ȯ */
	bstName	= _bstr_t(bstrName);

	/* ��ȯ ���� �ʱ�ȭ �� �� ���� */
	wmemset(name, 0x00, size);
	wmemcpy_s(name, size, (LPCTSTR)bstName, bstName.length());

	return (hResult == S_OK);
}

/*
 desc : ���� ����� ��� ID �� ��ȯ
 parm : equip_id- [out] ����(����)�Ǵ� ��� ID ���� �����
		logs	- [in]  TRUE:�α� �̷� ����, FALSE:�α� �̷� ������ ����
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
 desc : Variable ID�� �ش�Ǵ� Min, Max �� ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		id		- [in]  Variable ID
		type	- [out] Value Type (E30_GPVT)�� �����
		min		- [out] Min ���� �����
		max		- [out] Max ���� �����
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
	/* ���ڿ��� ��� NULL */
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
	/* ���ڿ��� ��� NULL */
	default	:	m_pLogData->PushLogs(L"GetVariableValue : Unsupported value type");	return FALSE;
	}
	type	= E30_GPVT(valType);
	return (hResultMin == S_OK && hResultMax == S_OK);
}
#endif
/*
 desc : ���� ����Ǿ� �ִ� Host ID �������� ������ ���� ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		type	- [in]  ���� Ÿ�� (E30_GVTC)
		lst_data- [out] �������� ������ ����� ����Ʈ ����
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

	/* Variant �ʱ�ȭ */
	VariantInit(&varList);

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetHostVariables::GetVariablesForConnection (conn_id=%d, type=%d)",
			   conn_id, type);
	m_pLogData->PushLogs(tzMesg);
	hResult = m_pICxClientTool->GetVariablesForConnection(conn_id, VarType(type), &varList);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* �� ������ �� */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetHostVariables::RestoreFromByteStream (conn_id=%d, type=%d)", 
			   conn_id, type);
	m_pLogData->PushLogs(tzMesg);
	pICxValueDisp->RestoreFromByteStream(0, 0, varList);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* List ���� ��� */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetHostVariables::ListAt (conn_id=%d, type=%d)",
			   conn_id, type);
	m_pLogData->PushLogs(tzMesg);
	hResult = pICxValueDisp->ListAt(0, 0, &lHandle1);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* List ���� */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetHostVariables::ItemCount (conn_id=%d, type=%d, handle1=%d)",
			   conn_id, type, lHandle1);
	m_pLogData->PushLogs(tzMesg);
	hResult = pICxValueDisp->ItemCount(lHandle1, &lCount1);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* Variables ������ŭ ��������  */
	for (i=1,bSucc=FALSE; i<=lCount1; i++)
	{
		/* ��� ���� �ʱ�ȭ  */
		stData.Init();

		/* Variables ���� ��� */
		swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetHostVariables::ListAt (conn_id=%d, type=%d, handle1=%d)",
				   conn_id, type, lHandle1);
		m_pLogData->PushLogs(tzMesg);
		hResult = pICxValueDisp->ListAt(lHandle1, i, &lHandle2);
		m_pLogData->CheckResult(hResult, tzMesg);
		/* Variables ID ��� */
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
		/* Variables Name ��� */
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
		/* Variables Description ��� */
		if (hResult == S_OK)
		{
			bstrValue.Empty();
			swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetHostVariables::Variable Desc (conn_id=%d, type=%d, handle1=%d, handle2=%d, index=%d, var_id=%d)", conn_id, type, lHandle1, lHandle2, i, lValue);
			m_pLogData->PushLogs(tzMesg);
			hResult = pICxValueDisp->GetValueAscii(lHandle2, 3, &bstrValue);
			m_pLogData->CheckResult(hResult, tzMesg);
			swprintf_s(stData.var_desc, CIM_CMD_DESC_SIZE, L"%s", (LPCTSTR)_bstr_t(bstrValue));
		}
		/* Variables Type ��� */
		if (hResult == S_OK)
		{
			swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetHostVariables::Variable Type (conn_id=%d, type=%d, handle1=%d, handle2=%d, index=%d, var_id=%d)", conn_id, type, lHandle1, lHandle2, i, lValue);
			m_pLogData->PushLogs(tzMesg);
			hResult = pICxValueDisp->GetValueU1(lHandle2, 5, &u8Value);
			m_pLogData->CheckResult(hResult, tzMesg);
			stData.var_type	= u8Value;
		}
		/* Value Type ��� */
		if (hResult == S_OK)
		{
			swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetHostVariables::Value Type (conn_id=%d, type=%d, handle1=%d, handle2=%d, index=%d, var_id=%d)", conn_id, type, lHandle1, lHandle2, i, lValue);
			m_pLogData->PushLogs(tzMesg);
			hResult = pICxValueDisp->GetValueU4(lHandle2, 6, &lValue);
			m_pLogData->CheckResult(hResult, tzMesg);
			stData.val_type	= lValue;
		}
		/* Current Value ��� */
		if (hResult == S_OK)			bSucc = GetValueDispPtr(lHandle2, 10, 0x00, pICxValueDisp, &stData);
		/* Default Value ��� */
		if (hResult == S_OK && bSucc)	bSucc = GetValueDispPtr(lHandle2, 13, 0x01, pICxValueDisp, &stData);
		/* ���� ��� */
#ifdef _DEBUG
		lst_data.AddTail(stData);
#else
		if (bSucc)	lst_data.AddTail(stData);
		else		stData.Delete();
#endif
	}
#if 0
	/* ��� �۾��� ���� �ߴٸ� */
	if (!bSucc)
	{
		/* ���� ��ϵȰ� ��� ���� */
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
 desc : ICxValueDisp���� Value Type ���� Value �� ��ȯ
 parm : handle	- [in]  Handle Index
		index	- [in]  Index
		flag	- [in]  0x00: Current Value, 0x01: Default Value
		disp	- [in]  ICxValueDisp Pointer
		data	- [in]  �⺻ ������ ����� ����ü ������
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
 desc : �α� ���� (���Ͽ� �α� ������ ���� ������ ����)
 parm : enable	- [out] �α� ���� ���� ���� �� ��ȯ
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
 desc : �α� ���� ���� ����
 parm : enable	- [in]  �α� ���� ���� ���� ���� �÷���
						enable=TRUE -> Start, enable=FALSE -> Stop
 retn : TRUE or FALSE
*/
BOOL CE30GEM::SetLogEnable(BOOL enable)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	TCHAR tzRun[2][16]	= { L"StopLogging", L"StartLogging" };
	BOOL bStatus		= FALSE;
	HRESULT hResult		= S_OK;

	/* ���� �α� ���� ���� */
	if (!IsLogging(bStatus))	return FALSE;
	if (bStatus == enable)		return TRUE;

	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"SetLogEnable::%s", tzRun[enable]);
	if (enable)	hResult = m_pICxClientApp->StartLogging();
	else		hResult = m_pICxClientApp->StopLogging();
	m_pLogData->CheckResult(hResult, tzMesg);

	return (hResult == S_OK);
}

/*
 desc : ���� ��� (�߻�)�� ��� ID ���� ��ȯ
 parm : id		- [out] ����Ʈ�� ����� CAtlList <LONG> : ID
		name	- [out] ����Ʈ�� ����� String Array    : Name
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

	/* ���� CIMConnect ���ο� �߻��� �˶� ����Ʈ �������� */
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

	/* ���� ���� ���ϱ� */
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

		/* Command ID�� Names ���  */
		for (i=0;SUCCEEDED(hResult) && i<lCount;i++)
		{
			ids.AddTail(pIDs[i]);
			names.Add(pNames[i]);
		}
	}

	/* SafeArray ���� */
	saIDs.Detach();
	saNames.Detach();

	return TRUE;
}

/*
 desc : ���� ��� (�߻�)�� EPJ ���ϵ� ��ȯ
 parm : files	- [out] ����Ʈ�� ����� String Array    : EPJ Files (Included Path)
		sizes	- [out] ����Ʈ�� ����� CAtlList <LONG> : EPJ File Size
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

	/* ���� CIMConnect ���ο� �߻��� �˶� ����Ʈ �������� */
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

	/* ���� ���� ���ϱ� */
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

		/* Alarm ID�� State ���  */
		for (i=0;SUCCEEDED(hResult) && i<lCntFiles;i++)	files.Add(pFiles[i]);
	}

	/* SafeArray ���� */
	saFiles.Detach();
	saSizes.Detach();

	return TRUE;
}

/*
 desc : ���� ��� (�߻�)�� Recipe Names ��ȯ
 parm : names	- [out] ����Ʈ�� ����� String Array    : Recipe Names
		sizes	- [out] ����Ʈ�� ����� CAtlList <LONG> : Recip Sizes
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

	/* ���� CIMConnect ���ο� ����� Recipe ����Ʈ �������� */
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

	/* ���� ���� ���ϱ� */
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

		/* Alarm ID�� State ���  */
		for (i=0;SUCCEEDED(hResult) && i<lCount;i++)
		{
			sizes.AddTail(pSizes[i]);
			names.Add(pNames[i]);
		}
	}

	/* SafeArray ���� */
	saNames.Detach();
	saSizes.Detach();

	return TRUE;
}

/*
 desc : Host �ʿ� Process Program (Recipe)�� �����ϰڴٰ� ��û
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		name	- [in]  Recipe (Process Program) Name (Ȯ���ڴ� �� �������� �ʾƵ� ��)
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
 desc : Host �ʿ� Process Program (Recipe)�� ��û
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		name	- [in]  Recipe (Process Program) Name (Ȯ���ڴ� �� �������� �ʾƵ� ��)
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
 desc : Host �ʿ� Process Program (Recipe)�� �۽�
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		name	- [in]  Recipe (Process Program) Name (Ȯ���ڴ� �� �������� �ʾƵ� ��)
 retn : TRUE or FALSE
 ���� : ICxValueDispPtr �κ� �����ؾ� �Ǵµ�... ���� ���ظ� ����
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
 desc : �ý��ۿ� ���ǵ� Report ��� ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		ids		- [out] ����Ʈ�� ����� CAtlList <LONG> : Report Names
		names	- [out] ����Ʈ�� ����� String Array    : Report IDs
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

	/* ���� CIMConnect ���ο� ����� Report ����Ʈ �������� */
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

	/* ���� ���� ���ϱ� */
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

		/* Alarm ID�� State ���  */
		for (i=0;SUCCEEDED(hResult) && i<lCount;i++)
		{
			ids.AddTail(pIDs[i]);
			names.Add(pNames[i]);
		}
	}

	/* SafeArray ���� */
	saNames.Detach();
	saIDs.Detach();

	return TRUE;
}

/*
 desc : Report �� ���õ� Event�� ����Ʈ ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		r_ids	- [out] ����Ʈ�� ����� CAtlList <LONG> : Report IDs
		e_ids	- [out] ����Ʈ�� ����� CAtlList <LONG> : Event IDs
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

	/* ���� CIMConnect ���ο� ����� Recipe ����Ʈ �������� */
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

	/* ���� ���� ���ϱ� */
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

		/* Alarm ID�� State ���  */
		for (i=0;SUCCEEDED(hResult) && i<lCount;i++)
		{
			r_ids.AddTail(pRIDs[i]);
			e_ids.AddTail(pEIDs[i]);
		}
	}

	/* SafeArray ���� */
	saRIDs.Detach();
	saEIDs.Detach();

	return TRUE;
}

/*
 desc : �߻��� Traces�� ����Ʈ ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		t_ids	- [out] ����Ʈ�� ����� CAtlList <LONG> : Report IDs
		t_names	- [out] ����Ʈ�� ����� String Array    : Trance Names
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

	/* ���� CIMConnect ���ο� ����� Trace ����Ʈ �������� */
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

	/* ���� ���� ���ϱ� */
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

		/* Alarm ID�� State ���  */
		for (i=0;SUCCEEDED(hResult) && i<lCount;i++)
		{
			t_ids.AddTail(pTIDs[i]);
			t_names.Add(pNames[i]);
		}
	}

	/* SafeArray ���� */
	saTIDs.Detach();
	saNames.Detach();

	return TRUE;
}

/*
 desc : EPJ ���Ͽ� ��ϵ� variables ��� ��ȯ
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		type	- [in]  Variable Type (E30_GVTC)
		ids		- [out] Variable IDs ��ȯ
		names	- [out] Variable Names ��ȯ
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

	/* ���� CIMConnect ���ο� ����� Recipe ����Ʈ �������� */
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

	/* ���� ���� ���ϱ� */
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

		/* Alarm ID�� State ���  */
		for (i=0; SUCCEEDED(hResult) && i<lCount; i++)
		{
			ids.AddTail(pIDs[i]);
			names.Add(pNames[i]);
		}
	}

	/* SafeArray ���� */
	saIDs.Detach();
	saNames.Detach();

	return TRUE;
}

/*
 desc : List all messages (SECS-II ��� �޽��� ��ȯ)
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		ids		- [out] message IDs ��ȯ
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

	/* ���� CIMConnect ���ο� ����� Recipe ����Ʈ �������� */
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

	/* ���� ���� ���ϱ� */
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

		/* Message IDs ���  */
		for (i=0; SUCCEEDED(hResult) && i<lCount; i++)	ids.AddTail(pIDs[i]);
	}

	/* SafeArray ���� */
	saIDs.Detach();

	return TRUE;
}

/*
 desc : ����� ��� ���� ���� ���
 parm : conn_id		- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		comm_pgid	- [out] The COM ProgID of the communications object
		size_pgid	- [in]  'comm_pgid'�� ����� ���ڿ� ����
		comm_sets	- [out] The communications settings
		size_sets	- [in]  'comm_sets'�� ����� ���ڿ� ����
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
	/* ��ȯ ���ۿ� ���� */
	swprintf_s(comm_pgid, size_pgid, L"%s", (LPCTSTR)_bstr_t(bstrPgid));
	swprintf_s(comm_sets, size_sets, L"%s", (LPCTSTR)_bstr_t(bstrSetting));

	return (hResult == S_OK);
}

/*
 desc : ����� ��� ����
 parm : conn_id		- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
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
 desc : �α� ���� ���� ȯ�� ���� ���� ���� ������Ʈ���� �������� ���� ����
 parm : enable	- [in]  TRUE : ����, FALSE : ���� ����
		logs	- [in]  �α� ���� ����
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
 desc : �α� ���� ���� ȯ�� ���� ���� ���� ���� �� ��ȯ
 parm : enable	- [out] TRUE : �����ϰ� ����, FALSE : ���� ���ϰ� ����
		logs	- [in]  �α� ���� ����
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
 desc : SECS-II Message �۽� (����) ��, Primary Message �۽�? ���
 parm : conn_id	- [in]  1 or Later (���� ������ ���ؼ��� 0 �� ���)
		mesg_sid- [in]  SECS-II Message ID : Stream ID (0xssff -> 0x00xx ~ 0xffxx ����)
		mesg_fid- [in]  SECS-II Message ID : Function ID (0xssff -> 0xxx00 ~ 0xxxFF ����)
		term_id	- [in]  ���� 0 ���̳�, �ڽ��� Terminal ID�� 1 �̻��� ��쵵 �ִ� (?)
		str_msg	- [in]  �޽��� ���� (���ڿ�), ���� NULL�̸� �޽��� ������ ���� �����
		reply	- [in]  ���� ��� ���� (Host�κ��� ���� �ޱ⸦ ���ϴ��� ����)
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
	/* SECS-II Message ID ���� */
	u16MsgID	= SFtoMsgID(mesg_sid, mesg_fid);
	if (u16MsgID == 0x0000)	return FALSE;

	/* Message Body ���� */
	if (str_msg)
	{
		pICxValueDisp->AppendValueBinary(0, BYTE(term_id));
		pICxValueDisp->AppendValueAscii(0, CComBSTR (str_msg));
		varMsg	= VARIANT_TRUE;
	}

	/* Message ���� */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE,
			   L"SendPrimaryMessage::StreamMsgID=%02d, FunctionMsgID=%02d", mesg_sid, mesg_fid);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxClientApp->CxSendMessage(conn_id, u16MsgID, pICxValueDisp,
											 varMsg, VARIANT_BOOL(reply), &lTransID, m_pICxEMAppCbk);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* ���� �ֱٿ� �۽��� Transaction ID �� ���� */
	m_pSharedData->SetLastSECSMessageTransID(lTransID);

	return (hResult == S_OK);
}

/*
 desc : Application Name ��ȯ
 parm : name	- [out] Application Name�� ����� ����
		size	- [in]  'name' ���� ũ��
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetAppName(PTCHAR name, UINT32 size)
{
	HRESULT hResult	= S_OK;
	CComBSTR bstrName;

	if (!m_pICxClientApp)	return FALSE;

	hResult	= m_pICxClientApp->get_appName(&bstrName);
	if (m_pLogData->CheckResult(hResult, L"GetAppName::get_appName"))	return FALSE;
	/* �� ��ȯ */
	swprintf_s(name, size, L"%s", (LPTSTR)_bstr_t(bstrName));

	return (hResult == S_OK);
}

/*
 desc : Application Name ����
 parm : name	- [in]  Application Name�� ����� ����
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
 desc : ���� ������Ʈ�� ��Ű���� ����� ������ ��¥ (�ð� ����) ��ȯ
 parm : date_time	- [out] ����� �ð� (��¥����)�� ����� ����
		size		- [in]  'data_time' ���� ũ��
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetBuildDateTime(PTCHAR date_time, UINT32 size)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	DATE dtDateTime	= 0;
	COleDateTime oleDateTime;
	ICxEMServicePtr pEMSvc	= NULL;

	/* Variant ��� */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetBuildDateTime::GetBuildDateTime");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxVersion->GetBuildDateTime(&dtDateTime);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* MSDN DATE Type (https://docs.microsoft.com/en-us/cpp/atl-mfc-shared/date-type?view=vs-2019) */
	oleDateTime	= COleDateTime(dtDateTime);
	/* �ð� ���� ���ڿ��� ��ȯ */
	swprintf_s(date_time, size, L"%4d-%02d-%02d %02d:%02d:%02d",
			   oleDateTime.GetYear(), oleDateTime.GetMonth(), oleDateTime.GetDay(),
			   oleDateTime.GetHour(), oleDateTime.GetMinute(), oleDateTime.GetSecond());

	return (hResult == S_OK);
}

/*
 desc : ���� ������Ʈ�� ��Ű���� ����� ������ Version ��ȯ
 parm : version	- [out] ����� �ð� (��¥����)�� ����� ����
		size	- [in]  'version' ���� ũ��
 retn : TRUE or FALSE
*/
BOOL CE30GEM::GetBuildVersion(PTCHAR version, UINT32 size)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult	= S_OK;
	CComBSTR bstrVer;
	ICxEMServicePtr pEMSvc	= NULL;

	/* Variant ��� */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetBuildVersion::GetVersionStr");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxVersion->GetVersionStr(&bstrVer);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* �� ��ȯ */
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
 desc : ���� (Ư��) ��ġ�� ����Ǿ� �ִ� ���� ���α׷��� IDs�� �̸����� ��ȯ
 parm : equip_id- [in]  ��� ID (0, 1 or Later)
		ids		- [out] ���� ���α׷� IDs ��ȯ (ID ���� ����Ʈ�� ��ȯ)
		names	- [out] ���� ���α׷� Names ��ȯ (���ڿ� �迭�� ��ȯ)
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
	/* Variant ��� */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetListApplications::ListApplications (equip_id=%d)", equip_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxEMService->ListApplications(equip_id, &vtIDs, &vtNames);
	if (m_pLogData->CheckResult(hResult, L"GetListApplications::ListApplications"))	return FALSE;
	/* Get access to the list of IDs, States, Texts */
	if (SUCCEEDED(hResult))	hResult = saIDs.Attach(vtIDs.parray);
	if (SUCCEEDED(hResult))	hResult = saNames.Attach(vtNames.parray);
	if (m_pLogData->CheckResult(hResult, L"GetListApplications::SafeArrary.Attach"))	return FALSE;

	/* ���� ���ϱ� */
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
	/* Application ID�� Names ���  */
	for (i=0;SUCCEEDED(hResult) && i<lCount;i++)
	{
		ids.AddTail(pIDs[i]);
		names.Add(pNames[i]);
	}

	/* SafeArray ���� */
	saIDs.Detach();
	saNames.Detach();

	return (hResult == S_OK);
}

/*
 desc : ������ ��� ������ ȣ��Ʈ ���� ��� ��ȯ
 parm : equip_id- [in]  ��� ID (0, 1 or Later)
		ids		- [out] HOST IDs ��ȯ (ID ���� ����Ʈ�� ��ȯ)
		names	- [out] HOST Names ��ȯ (���ڿ� �迭�� ��ȯ)
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
	/* Variant ��� */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetListConnections::ListConnections (equip_id=%d)", equip_id);
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxEMService->ListConnections(equip_id, &vtIDs, &vtNames);
	if (m_pLogData->CheckResult(hResult, L"GetListConnections::ListConnections"))	return FALSE;
	/* Get access to the list of IDs, States, Texts */
	if (SUCCEEDED(hResult))	hResult = saIDs.Attach(vtIDs.parray);
	if (SUCCEEDED(hResult))	hResult = saNames.Attach(vtNames.parray);
	if (m_pLogData->CheckResult(hResult, L"GetListConnections::SafeArrary.Attach"))	return FALSE;

	/* ���� ���ϱ� */
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
	/* Application ID�� Names ���  */
	for (i=0;SUCCEEDED(hResult) && i<lCount;i++)
	{
		ids.AddTail(pIDs[i]);
		names.Add(pNames[i]);
	}

	/* SafeArray ���� */
	saIDs.Detach();
	saNames.Detach();

	return (hResult == S_OK);
}

/*
 desc : EM Service�� ����Ǿ� �ִ� Equipment�� IDs�� �̸����� ��ȯ
 parm : ids		- [out] Equipment IDs ��ȯ (ID ���� ����Ʈ�� ��ȯ)
		names	- [out] Equipment Names ��ȯ (���ڿ� �迭�� ��ȯ)
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
	/* Variant ��� */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"GetListEquipments::ListEquipment");
	m_pLogData->PushLogs(tzMesg);
	hResult	= m_pICxEMService->ListEquipment(&vtIDs, &vtNames);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
	/* Get access to the list of IDs, States, Texts */
	if (SUCCEEDED(hResult))	hResult = saIDs.Attach(vtIDs.parray);
	if (SUCCEEDED(hResult))	hResult = saNames.Attach(vtNames.parray);
	if (m_pLogData->CheckResult(hResult, L"GetListEquipments::SafeArrary.Attach"))	return FALSE;

	/* ���� ���ϱ� */
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
	/* Application ID�� Names ���  */
	for (i=0;SUCCEEDED(hResult) && i<lCount;i++)
	{
		ids.AddTail(pIDs[i]);
		names.Add(pNames[i]);
	}

	/* SafeArray ���� */
	saIDs.Detach();
	saNames.Detach();

	return (hResult == S_OK);
}

/*
 desc : TS (Tracking System)�� ��ϵ� Equipment Module (Location) Name ��ȯ
 parm : index	- [in]  ��� ��� ���ε� �ε��� �� (1 based)
 retn : �̸� ���ڿ� (������ ��� NULL or Empty String)
*/
PTCHAR CE30GEM::GetEquipLocationName(ENG_TSSI index)
{
	if (INT32(index) < 1 || m_arrEquipLocName.GetSize() < INT32(index))	return L"";
	return m_arrEquipLocName[INT32(index)-1].GetBuffer();
}

/*
 desc : TS (Tracking System)�� ��ϵ� Processing Name ��ȯ
 parm : index	- [in]  ��� ��� ���ε� �ε��� �� (!!! -1 based !!!)
 retn : �̸� ���ڿ� (������ ��� NULL or Empty String)
*/
PTCHAR CE30GEM::GetEquipProcessingName(E90_SSPS index)
{
	INT32 i32Index	= INT32(index);
	if (i32Index < -1 || INT32(E90_SSPS::en_skipped) < i32Index)	return L"";
	if (i32Index == INT32(E90_SSPS::en_no_state))	i32Index	= INT32(E90_SSPS::en_skipped) + 1;
	return m_arrEquipProcName[INT32(i32Index)].GetBuffer();
}

/*
 desc : SECS-II Message ��� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CE30GEM::ResetSecsMesgList()
{
	HRESULT hResult	= S_FALSE;

	/* ���� ��ϵ� �޽��� ��� ���� ���� */
	hResult	= m_pICxSECSMesg->Clear();

	/* Root Message ��� */
	return (hResult == S_OK);
}

/*
 desc : SECS-II Message ��� - Root Message
 parm : type	- [in]  Message Type
		value	- [in]  Message Type�� ���� ��
		size	- [in]  'value' ���� ũ��
 retn : TRUE or FALSE
*/
BOOL CE30GEM::AddSecsMesgRoot(E30_GPVT type, PVOID value, UINT32 size)
{
	/* ���� ��ϵ� �޽��� ��� ���� */
	m_pICxSECSMesg->Clear();
	/* Root Message ��� */
	return AddSecsMesgValue(0, type, value, size);
}

/*
 desc : SECS-II Message ��� - Root Message
 parm : handle	- [in]  List�� �߰��� ���� Parent ID
						�� ó�� �߰��� List�� ���, 0 ��
		clear	- [in]  Message ���� ��� ���� ���� (�����̸� TRUE, �߰��̸� FALSE)
 retn : ���� �߰��� Handle �� ��ȯ (���� ���̸�, ���� ó��)
*/
LONG CE30GEM::AddSecsMesgList(LONG handle, BOOL clear)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG lHandle	= -1;
	HRESULT hResult	= S_FALSE;

	/* ���� ��ϵ� �޽��� ��� ���� ���� */
	if (clear)	m_pICxSECSMesg->Clear();

	swprintf_s(tzMesg, L"AddSecsMesgList::handle = %d", handle);
	hResult	= m_pICxSECSMesg->AppendList(handle, &lHandle);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return -1;

	/* Root Message ��� */
	return lHandle;
}


/*
 desc : ���� ��ġ (Host?)���� �۽ŵ� SECS-II Message ���
 parm : handle	- [in]  Handle to a list within the object, or 0
		type	- [in]  'value' ���� ����� ���� ����
		value	- [in]  The value to append
		size	- [in]  'value' ���ۿ� ����� �������� ũ��
 retn : TRUE or FALSE
*/
BOOL CE30GEM::AddSecsMesgValue(LONG handle, E30_GPVT type, PVOID value, UINT32 size)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	HRESULT hResult	= S_FALSE;

	/* ������ ���� �Ӽ� (Parameter Type)�� ���� ��� ���� */
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

	/* �޽��� ��� ���� ���� */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"AddSecsMesgValue::AppendValue (mesg_type=%d)", type);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	return (hResult == S_OK);
}

/*
 desc : ���� ��ġ (Host?)���� SECS-II Message �۽�
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		mesg_sid- [in]  �۽� ����� SECS-II Message ID (Stream ID) (10����)
		mesg_fid- [in]  �۽� ����� SECS-II Message ID (Function ID) (10����)
		trans_id- [out] �۽� �� ���� ���� ��, ����ȭ�� ���� �ʿ��� ID (���� ���� ������ �������� �˱� ���� �ĺ� ID) (0 �� �ʱ�ȭ)
		is_reply- [in]  ���� (Ack) ��û ����. TRUE - Host�κ��� �޽��� ���ſ� ���� ���� ��û, FALSE - ���� �ʿ� ����
		is_body	- [in]  Message Body�� �����ϴ��� ���� ��, Message ID�� �����Ѵٸ� FALSE, ���� Message�� �����Ѵٸ� (TRUE)
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

	/* ���� Stream 9 �޽��� �۽��� ���, �Ʒ� ���ǿ� ���� �ʴ� Function�� ��� ���� ���� */
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
	/* ��� �Լ� ȣ���� ����, ���� ���ŵ� SECS-II Message�� ��� ����Ʈ ���� ��� */
	ListAtItem(m_pICxSECSMesg, 0, 0, 0, lstItem);
#endif
	/* ���� �޽����� ���ٸ�, 0x00010000 �߰� */
	if (!is_body)	lMesgID	|= 0x10000;

	/* ���� �޽��� ���� ���ο� ���� �÷��� ���� */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE,
			   L"SendSecsMessage::ItemCount (conn_id=%d, mesg_id=%d,%d, is_reply=%d, transid=%d, is_body=%d)",
			   conn_id, mesg_sid, mesg_fid, is_reply, trans_id, is_body);
	hResult	= m_pICxSECSMesg->ItemCount(0, &lCount);
	if (m_pLogData->CheckResult(hResult, tzMesg))
	{
		return FALSE;
	}
	if (lCount > 0)	vbMessage	= VARIANT_TRUE;

	/* Transaction ID : 0 �� ���� */
	trans_id	= 0;

	/* �޽��� �۽� (������ Callback�� �ݵ�� NULL, RegisterMsgError �Լ��� �ش� ���� �޽��� ID�� ��� �����Ƿ�) */
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
 desc : ValueDisp�� ����� List Data �������� ��� �Լ�
 parm : mesg	- [in]  The message value object
		handle	- [in]  List Handle
		index	- [in]  List Index
		level	- [in]  Sub-Depth (Tab ����. XML ���� �����ϸ� �� <XML> ... </XML>)
		items	- [in]  List Data�� �ش�� Handle�� Count ������ ����� ����Ʈ ����
 retn : TRUE or FALSE
*/
BOOL CE30GEM::ListAtItem(ICxValueDisp *mesg, LONG handle, LONG index, LONG level,
						 CAtlList <STG_CLHC> &item)
{
	TCHAR tzMesg[CIM_CMD_DESC_SIZE]	= {NULL};
	LONG l, lHandle	= -1L, lCount;
	HRESULT hResult	= S_OK;
	STG_CLHC stItem	= {NULL};

	/* List Handle ��� */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"CEMSvcCbk::ListAt (handle=%d)", handle);
	m_pLogData->PushLogs(tzMesg);
	hResult = mesg->ListAt(handle, index, &lHandle);
#if 0
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;
#else
	if (lHandle < 0)	return FALSE;
#endif
	/* List Count ��� */
	swprintf_s(tzMesg, CIM_CMD_DESC_SIZE, L"CEMSvcCbk::ItemCount (lHandle=%d)", lHandle);
	m_pLogData->PushLogs(tzMesg);
	hResult = mesg->ItemCount(lHandle, &lCount);
	if (m_pLogData->CheckResult(hResult, tzMesg))	return FALSE;

	/* ���� ������ ���ٸ� */
	if (lCount < 1)	return FALSE;

	/* �˻��� ������ŭ, �ٽ� ����Լ� ȣ�� (��ƴ�) */
	for (l=1; l<=lCount; l++)
	{
		/* �� ������ ��ϵ� �׷� ������ ������, ���� �ع��� */
		if (!ListAtItem(mesg, lHandle, l, level+1, item))
		{
			/* �� �̻� ������ ��ϵ� ������ ���ٸ�, List�� ���� ���� ��� */
			stItem.handle	= lHandle;
			stItem.index	= l;
			stItem.level	= level;
			item.AddTail(stItem);
		}
	}

	return TRUE;
}