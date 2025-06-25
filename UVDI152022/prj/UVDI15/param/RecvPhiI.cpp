#include "pch.h"
#include "RecvPhil.h"

#include "RecipeManager.h"

#include "..\DlgMain.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

class CDlgMain;

CRecvPhil::CRecvPhil()
{
	//m_stExpoLog.Init();
	//m_stPhilStatus.Init();

	m_hMainWnd = NULL;

}
CRecvPhil::~CRecvPhil()
{

}

VOID CRecvPhil::Init(HWND hWnd)
{
	m_hMainWnd = hWnd;
}


VOID CRecvPhil::PhilSendCreateRecipe(STG_PP_PACKET_RECV* stRecv)
{
	STG_PP_C2P_RCP_CREATE_ACK stCreateSend;
	STG_RJAF stRecipe = { NULL };
	stCreateSend.Reset();
	stRecipe.Init();
	CUniToChar csCnv;
	CString strRecipe;

	stCreateSend.ulUniqueID = stRecv->st_c2p_rcp_create.ulUniqueID;

	TCHAR szTemp[512] = { NULL };
	/*DlgPhilhmi ���� �޽��� Ȯ��*/
	strRecipe.Format(_T("%s"), csCnv.Ansi2Uni(stRecv->st_c2p_rcp_create.szRecipeName));
	swprintf_s(szTemp, 512, L"Reicpe Name : %s", strRecipe.GetString());
	//::PostMessage(m_hMainWnd, WM_MAIN_PHIL_MSG, WPARAM(szTemp), 0);

	memcpy(stRecipe.job_name, stRecv->st_c2p_rcp_create.szRecipeName, DEF_MAX_RECIPE_NAME_LENGTH);
	//memcpy(stRecipe.job_name,		stRecv->st_c2p_rcp_create.stVar[0].szParameterValue,		DEF_MAX_RECIPE_PARAM_VALUE_LENGTH);
	memcpy(stRecipe.gerber_path,	stRecv->st_c2p_rcp_create.stVar[1].szParameterValue,	DEF_MAX_RECIPE_PARAM_VALUE_LENGTH);
	memcpy(stRecipe.gerber_name,	stRecv->st_c2p_rcp_create.stVar[2].szParameterValue,	DEF_MAX_RECIPE_PARAM_VALUE_LENGTH);
	memcpy(stRecipe.align_recipe,	stRecv->st_c2p_rcp_create.stVar[3].szParameterValue,	DEF_MAX_RECIPE_PARAM_VALUE_LENGTH);
	memcpy(stRecipe.expo_recipe,	stRecv->st_c2p_rcp_create.stVar[4].szParameterValue,	DEF_MAX_RECIPE_PARAM_VALUE_LENGTH);
	stRecipe.material_thick =	(UINT32)atoi(stRecv->st_c2p_rcp_create.stVar[5].szParameterValue);
	stRecipe.expo_energy =		(FLOAT)atof(stRecv->st_c2p_rcp_create.stVar[6].szParameterValue);

	///*Recipe ���� ��û ����*/
	if (CRecipeManager::GetInstance()->CreateRecipe(stRecipe))
	{
		uvEng_Philhmi_Send_C2P_RCP_CREATE_ACK(stCreateSend);
	}
	/*Recipe ���� ��û ����*/
	else
	{
		stCreateSend.usErrorCode = ePHILHMI_ERR_RECIPE_CREATE;
		uvEng_Philhmi_Send_C2P_RCP_CREATE_ACK(stCreateSend);
	}
}

VOID CRecvPhil::PhilSendDelectRecipe(STG_PP_PACKET_RECV* stRecv)
{
	STG_PP_C2P_RCP_DELETE_ACK stDeleteSend;
	stDeleteSend.Reset();
	stDeleteSend.ulUniqueID = stRecv->st_c2p_rcp_delete.ulUniqueID;
	CUniToChar csCnv;
	CString strRecipe;

	strRecipe.Format(_T("%s"), csCnv.Ansi2Uni(stRecv->st_c2p_rcp_delete.szRecipeName));


	if (uvEng_JobRecipe_RecipeDelete(strRecipe.GetBuffer()))
	{
		uvEng_Philhmi_Send_C2P_RCP_DELETE_ACK(stDeleteSend);
	}
	/*Recipe ���� ��û ����*/
	else
	{
		stDeleteSend.usErrorCode = ePHILHMI_ERR_RECIPE_DELETE;
		uvEng_Philhmi_Send_C2P_RCP_DELETE_ACK(stDeleteSend);
	}


}

VOID CRecvPhil::PhilSendModifyRecipe(STG_PP_PACKET_RECV* stRecv)
{
	STG_PP_C2P_RCP_MODIFY_ACK stModifySend;
	STG_RJAF stRecipe = { NULL };
	stModifySend.Reset();
	stRecipe.Init();
	CUniToChar csCnv;
	stModifySend.ulUniqueID = stRecv->st_c2p_rcp_modify.ulUniqueID;


	/*�Ķ���� ����*/
	memcpy(stRecipe.job_name,		stRecv->st_c2p_rcp_create.stVar[0].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH);
	memcpy(stRecipe.gerber_path,	stRecv->st_c2p_rcp_create.stVar[1].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH);
	memcpy(stRecipe.gerber_name,	stRecv->st_c2p_rcp_create.stVar[2].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH);
	memcpy(stRecipe.align_recipe,	stRecv->st_c2p_rcp_create.stVar[3].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH);
	memcpy(stRecipe.expo_recipe,	stRecv->st_c2p_rcp_create.stVar[4].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH);
	stRecipe.material_thick =	(UINT32)atoi(stRecv->st_c2p_rcp_create.stVar[5].szParameterValue);
	stRecipe.expo_energy =		(FLOAT)atof(stRecv->st_c2p_rcp_create.stVar[6].szParameterValue);


	///*Recipe ���� ��û ����*/
	//if (CRecipeManager::GetInstance()->UpdateRecipe(stRecipe))
	if (uvEng_JobRecipe_RecipeModify(&stRecipe))
	{
		uvEng_Philhmi_Send_C2P_RCP_MODIFY_ACK(stModifySend);
	}
	/*Recipe ���� ��û ����*/
	else
	{
		stModifySend.usErrorCode = ePHILHMI_ERR_RECIPE_MODIFY;
		uvEng_Philhmi_Send_C2P_RCP_MODIFY_ACK(stModifySend);
	}
}

VOID CRecvPhil::PhilSendSelectRecipe(STG_PP_PACKET_RECV* stRecv, CDlgMain* callerInst, BOOL is_busy)
{
	STG_PP_C2P_RCP_SELECT_ACK stSelectSend;
	stSelectSend.Reset();
	stSelectSend.ulUniqueID = stRecv->st_c2p_rcp_select.ulUniqueID;
	CUniToChar csCnv;
	CString strRecipe;
	CString strExpo, strAlign;

	strRecipe.Format(_T("%s"), csCnv.Ansi2Uni(stRecv->st_c2p_rcp_select.szRecipeName));

	uvEng_Philhmi_Send_C2P_RCP_SELECT_ACK(stSelectSend);

	uvEng_JobRecipe_SetWhatLastSelectIsLocal(false); //<-���е� ���� ȣ��Ʈ �����Ƿ� �����ؾ���.
	
	/*���� �ó����� ���� ��*/
	if (is_busy)
	{
		stSelectSend.usErrorCode = ePHILHMI_ERR_STATUS_BUSY;
		uvEng_Philhmi_Send_C2P_RCP_SELECT_ACK(stSelectSend);
	}
	else
	{
		/*Recipe ���� ��û ����*/
		if (CRecipeManager::GetInstance()->SelectRecipe(strRecipe,EN_RECIPE_SELECT_TYPE::eRECIPE_MODE_SEL_FROM_HOST))
		{
			//Host���� ������ Recipe Name �����Ͽ� �� �뱤�� ���� Reicpe Name ���� Ȯ�� �۾� ����
			//memset(callerInst->m_stExpoLog.host_recipe_name, 0x00, DEF_MAX_RECIPE_NAME_LENGTH);
			//strcpy_s(callerInst->m_stExpoLog.host_recipe_name, stRecv->st_c2p_rcp_select.szRecipeName);


			UINT8 u8Offset = 0xff;
			callerInst->RunWorkJob(ENG_BWOK::en_gerb_load, PUINT64(&u8Offset));

			uvEng_Philhmi_Send_C2P_RCP_SELECT_ACK(stSelectSend);

			/*UI Recipe ��� ����*/
			strRecipe = CRecipeManager::GetInstance()->GetRecipeName();
			strExpo = CRecipeManager::GetInstance()->GetExpoRecipeName();
			strAlign = CRecipeManager::GetInstance()->GetAlignRecipeName();
			::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_MAIN_RECIPE_UPDATE, (WPARAM)0, (LPARAM)&strRecipe);
			::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_MAIN_RECIPE_UPDATE, (WPARAM)1, (LPARAM)&strExpo);
			::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_MAIN_RECIPE_UPDATE, (WPARAM)2, (LPARAM)&strAlign);
			/*Auto Dlg�� ����*/
			callerInst->CreateMenu(IDC_MAIN_BTN_AUTO);
		}
		/*Recipe ���� ��û ����*/
		else
		{
			stSelectSend.usErrorCode = ePHILHMI_ERR_RECIPE_LOAD;
			uvEng_Philhmi_Send_C2P_RCP_SELECT_ACK(stSelectSend);
		}

	}

}

VOID CRecvPhil::PhilSendListRecipe(STG_PP_PACKET_RECV* stRecv)
{
	STG_PP_C2P_RCP_LIST_ACK	stStatus;
	stStatus.Reset();
	stStatus.ulUniqueID = stRecv->st_c2p_rcp_list.ulUniqueID;

	int nCount;
	CUniToChar csCnv1;
	/*����� ������ ����*/
	nCount = uvEng_JobRecipe_GetCount();
	stStatus.usCount = nCount;


	// 20230824 mhbaek Add
	strcpy_s(stStatus.szSelectedRecipeName, DEF_MAX_RECIPE_NAME_LENGTH, csCnv1.Ansi2UTF(uvEng_JobRecipe_GetSelectRecipe(false)->job_name));

	for (int i = 0; i < nCount; i++)
	{
		LPG_RJAF pstRecipe = uvEng_JobRecipe_GetRecipeIndex(i);
		/*������ ����Ʈ �ۼ�*/
		strcpy_s(stStatus.szArrRecipeName[i], DEF_MAX_RECIPE_NAME_LENGTH, csCnv1.Ansi2UTF(pstRecipe->job_name));

		//if (m_pDlgMenu && m_pDlgMenu->GetDlgID() == ENG_CMDI::en_menu_philhmi)
		//{
		//	swprintf_s(szTemp, 512, L"Reicpe Name(%d) : %s", i, strRecipe.GetString());
		//	((CDlgPhilhmi*)m_pDlgMenu)->AddListBox(szTemp);
		//}
	}
	/*Philhmil�� Reicpe list ����*/
	uvEng_Philhmi_Send_C2P_RCP_LIST_ACK(stStatus);
}

VOID CRecvPhil::PhilSendInfoRecipe(STG_PP_PACKET_RECV* stRecv)
{
	STG_PP_C2P_RCP_INFO_ACK	stInfoSend;
	stInfoSend.Reset();
	stInfoSend.ulUniqueID = stRecv->st_c2p_rcp_info.ulUniqueID;

	memcpy(stInfoSend.szRecipeName, stRecv->st_c2p_rcp_info.szRecipeName, DEF_MAX_RECIPE_NAME_LENGTH);


	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe(false);

	EN_RECIPE_JOB::JOB_NAME;

	/*�뱤 ��� �Ķ���Ͱ�*/
	stInfoSend.usCount = 7;
	sprintf_s(stInfoSend.stVar[0].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stInfoSend.stVar[0].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Job Name");
	sprintf_s(stInfoSend.stVar[0].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", pstRecipe->job_name);

	sprintf_s(stInfoSend.stVar[1].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stInfoSend.stVar[1].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Gerber Path");
	sprintf_s(stInfoSend.stVar[1].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", pstRecipe->gerber_path);

	sprintf_s(stInfoSend.stVar[2].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stInfoSend.stVar[2].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Gerber Name");
	sprintf_s(stInfoSend.stVar[2].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", pstRecipe->gerber_name);

	sprintf_s(stInfoSend.stVar[3].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stInfoSend.stVar[3].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Align Recipe");
	sprintf_s(stInfoSend.stVar[3].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", pstRecipe->align_recipe);

	sprintf_s(stInfoSend.stVar[4].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stInfoSend.stVar[4].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Expo Recipe");
	sprintf_s(stInfoSend.stVar[4].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", pstRecipe->expo_recipe);

	sprintf_s(stInfoSend.stVar[5].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "INT");
	sprintf_s(stInfoSend.stVar[5].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Material Thick");
	sprintf_s(stInfoSend.stVar[5].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%d", pstRecipe->material_thick);

	sprintf_s(stInfoSend.stVar[6].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stInfoSend.stVar[6].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Expo Energy");
	sprintf_s(stInfoSend.stVar[6].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%.1f", pstRecipe->expo_energy);

	uvEng_Philhmi_Send_C2P_RCP_INFO_ACK(stInfoSend);
}

VOID CRecvPhil::PhilSendProcessExecute(STG_PP_PACKET_RECV* stRecv, CDlgMain* callerInst, BOOL is_busy)
{
	STG_PP_C2P_PROCESS_EXECUTE_ACK stProcessExecute;
	stProcessExecute.Reset();
	stProcessExecute.ulUniqueID = stRecv->st_c2p_process_execute.ulUniqueID;
	TCHAR tzMesg[512] = { NULL };

	callerInst->m_stExpoLog.Init();
	//callerInst->m_stExpoLog.phil_mode = 1;
	/*�뱤 ���� ����*/
	//m_stExpoLog.Init();
	memcpy(callerInst->m_stExpoLog.recipe_name, stRecv->st_c2p_process_execute.szRecipeName, DEF_MAX_RECIPE_NAME_LENGTH);
	memcpy(callerInst->m_stExpoLog.glassID, stRecv->st_c2p_process_execute.szGlassID, DEF_MAX_GLASS_NAME_LENGTH);
	/*���� �Ķ���Ͱ� ������*/
	memcpy(stProcessExecute.szRecipeName, stRecv->st_c2p_process_execute.szRecipeName, DEF_MAX_RECIPE_NAME_LENGTH);
	memcpy(stProcessExecute.szGlassID, stRecv->st_c2p_process_execute.szGlassID, DEF_MAX_GLASS_NAME_LENGTH);

	/*�������� ���� ����*/
	
	//
	uvEng_JobRecipe_SetWhatLastSelectIsLocal(false);
	LPG_RJAF selJob = uvEng_JobRecipe_GetSelectRecipe(false); //<- execute�� ������ host�����Ƿ� ����Ǿ���.
	//

	BOOL bSelect = selJob != nullptr;
	/*������ ��� ����*/
	BOOL bLoaded = uvCmn_Luria_IsJobNameLoaded();
	/*�����ǿ� ��ũ ��� ����*/
	BOOL bMarked = uvEng_Luria_IsMarkGlobal();


	if (is_busy)
	{

		swprintf_s(tzMesg, 512, L"execute failed, dimaster on busy.");
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);

		stProcessExecute.usErrorCode = ePHILHMI_ERR_STATUS_BUSY;
		uvEng_Philhmi_Send_C2P_PROCESS_EXECUTE_ACK(stProcessExecute);
		return;
	}

	//if(callerInst->m_stExpoLog.host_recipe_name[0] == '\0')
	//{
	//	swprintf_s(tzMesg, 512, L"No recipe selected. Select on Platform first.");
	//	LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);

	//	//   �� ����, ���� �Ұ�
	//	stProcessExecute.usErrorCode = ePHILHMI_ERR_STATUS_FAILED;
	//	uvEng_Philhmi_Send_C2P_PROCESS_EXECUTE_ACK(stProcessExecute);
	//	return;
	//}

	


	if (strcmp(selJob->job_name, callerInst->m_stExpoLog.recipe_name) != 0) //������ �ٸ����.
	{

		wchar_t seljob[128] = { 0 };
		wchar_t exeJob[128] = { 0 };

		MultiByteToWideChar(CP_ACP, 0, selJob->job_name, -1, seljob, 128);
		MultiByteToWideChar(CP_ACP, 0, callerInst->m_stExpoLog.recipe_name, -1, exeJob, 128);

		swprintf_s(
			tzMesg, 512,
			L"execute error! Selected =(%s) / Host Executed = (%s) recipe mismatching!!!!",
			seljob,
			exeJob);

		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
		stProcessExecute.usErrorCode = ePHILHMI_ERR_STATUS_FAILED;
	}
	else
	{

		char loadedJob[255] = { 0, };
		bool getLoadedJob = uvEng_Luria_GetLoadedJobName(loadedJob, 255);

		if (getLoadedJob && strcmp(selJob->gerber_name, loadedJob) != 0) //�ε尡 �ٸ���
		{


			wchar_t wloaded[128] = { 0 };
			wchar_t wexec[128] = { 0 };

			MultiByteToWideChar(CP_ACP, 0, selJob->gerber_name, -1, wexec, 128);
			MultiByteToWideChar(CP_ACP, 0, loadedJob, -1, wloaded, 128);

			swprintf_s(
				tzMesg, 512,
				L"execute error!loaded(% s) / execute(% s) recipe mismatching!!!!",
				wloaded,
				wexec);

			LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
			stProcessExecute.usErrorCode = ePHILHMI_ERR_STATUS_FAILED;
		}
		else
		{
			if (uvEng_GetConfig()->set_comn.use_auto_align)
			{
				/*�ڵ�ȭ ���۽� ����� ���� �� �뱤*/
				callerInst->RunWorkJob(ENG_BWOK::en_expo_align, PUINT64(&callerInst->m_stExpoLog));
			}
			else
			{
				/*�ڵ�ȭ ���۽� ����� ���� ���� �뱤*/
				callerInst->RunWorkJob(ENG_BWOK::en_expo_only, PUINT64(&callerInst->m_stExpoLog));
			}
		}
	}


	uvEng_Philhmi_Send_C2P_PROCESS_EXECUTE_ACK(stProcessExecute);
}

VOID CRecvPhil::PhilSendStatusValue(STG_PP_PACKET_RECV* stRecv)
{
	STG_PP_C2P_STATUS_VALUE_ACK		stStatusValue;
	stStatusValue.Reset();

	/*������� LED ���� �� LED �µ� ����*/
	//uvEng_Luria_ReqGetPhLedTempAll();
	UINT16(*pLed)[8] = uvEng_ShMem_GetLuria()->directph.light_source_driver_temp_led;
	UINT16(*pBoard)[8] = uvEng_ShMem_GetLuria()->directph.light_source_driver_temp_board;

	UINT8 u8Count = 0;

	TCHAR szTemp[512] = { NULL };
	/* ���а� LED / Board �µ� �� ���� */
	for (int ph = 0; ph < uvEng_GetConfig()->luria_svc.ph_count; ph++)
	{
		for (int led = 0x00; led < 0x03 + 1; led++)
		{
			// �Ҽ��� ��� �� �����÷����� "DOUBLE"�� ������ �մϴ�.
			sprintf_s(stStatusValue.stVar[u8Count].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "DOUBLE");
			sprintf_s(stStatusValue.stVar[u8Count].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Photohead %d_%d Led Temp", ph + 1, led);
			sprintf_s(stStatusValue.stVar[u8Count].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%.1f", pLed[ph][led] / 10.0f);
			u8Count++;
			sprintf_s(stStatusValue.stVar[u8Count].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "DOUBLE");
			sprintf_s(stStatusValue.stVar[u8Count].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Photohead %d_%d Board Temp", ph + 1, led);
			sprintf_s(stStatusValue.stVar[u8Count].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%.1f", pBoard[ph][led] / 10.0f);
			u8Count++;
		}

	}


	stStatusValue.ulUniqueID = stRecv->st_c2p_status_value.ulUniqueID;
	stStatusValue.usCount = u8Count;
	uvEng_Philhmi_Send_C2P_STATUS_VALUE_ACK(stStatusValue);
}

VOID CRecvPhil::PhilSendChageMode(STG_PP_PACKET_RECV* stRecv, CDlgMain* callerInst)
{
	STG_PP_C2P_MODE_CHANGE_ACK stModeChange;
	stModeChange.Reset();

	/*��� ����*/
	stModeChange.usMode = stRecv->st_c2p_mode_change.usMode;
	/*���� ����*/
	g_u8Romote = stRecv->st_c2p_mode_change.usMode;


	if (stRecv->st_c2p_mode_change.usMode == en_menu_phil_mode_auto)
	{
		callerInst->CreateMenu(IDC_MAIN_BTN_AUTO);
	}
	else if (stRecv->st_c2p_mode_change.usMode == en_menu_phil_mode_stop)
	{
		/*��� ���� ����*/
		callerInst->RunWorkJob(ENG_BWOK::en_work_stop);
	}
	else if (stRecv->st_c2p_mode_change.usMode == en_menu_phil_mode_pause)
	{
		/*��� ���� ����*/
		callerInst->RunWorkJob(ENG_BWOK::en_work_stop);
	}
	else if (stRecv->st_c2p_mode_change.usMode == en_menu_phil_mode_alarm)
	{

	}
	else if (stRecv->st_c2p_mode_change.usMode == en_menu_phil_mode_pm)
	{

	}


	stModeChange.ulUniqueID = stRecv->st_c2p_mode_change.ulUniqueID;
	uvEng_Philhmi_Send_C2P_MODE_CHANGE_ACK(stModeChange);
}

VOID CRecvPhil::PhilSendInitialExecute(STG_PP_PACKET_RECV* stRecv, CDlgMain* callerInst, BOOL is_busy)
{
	STG_PP_C2P_INITIAL_EXECUTE_ACK	stInitialExecuteAck;
	stInitialExecuteAck.Reset();

	stInitialExecuteAck.ulUniqueID = stRecv->st_c2p_initial_execute.ulUniqueID;

	if (!is_busy)
	{
		/*��� �ʱ�ȭ ����*/
		callerInst->RunWorkJob(ENG_BWOK::en_work_init);
	}
	else
	{
		stInitialExecuteAck.usErrorCode = ePHILHMI_ERR_STATUS_BUSY;
	}

	uvEng_Philhmi_Send_C2P_INITIAL_EXECUTE_ACK(stInitialExecuteAck);
}


VOID CRecvPhil::PhilSendTimeSync(STG_PP_PACKET_RECV* stRecv, CDlgMain* callerInst)
{
	STG_PP_C2P_TIME_SYNC_ACK		stTimeSync;
	stTimeSync.Reset();
	stTimeSync.ulUniqueID = stRecv->st_c2p_time_sync.ulUniqueID;

	/*���� �ð�*/
	SYSTEMTIME stTm = { NULL };
	GetLocalTime(&stTm);
	char szSyncTime[DEF_TIME_LENGTH + 1];
	sprintf_s(stRecv->st_c2p_time_sync.szSyncTime, DEF_TIME_LENGTH + 1, "%04d%02d%02d%02d%02d%02d",
		stTm.wYear, stTm.wMonth, stTm.wDay, stTm.wHour, stTm.wMinute, stTm.wSecond);
	memcpy(stRecv->st_c2p_time_sync.szSyncTime, szSyncTime, DEF_TIME_LENGTH);


	uvEng_Philhmi_Send_C2P_TIME_SYNC_ACK(stTimeSync);
}

VOID CRecvPhil::PhilSendInterruptStop(STG_PP_PACKET_RECV* stRecv, CDlgMain* callerInst)
{
	STG_PP_C2P_INTERRUPT_STOP_ACK	stInterruptStop;
	stInterruptStop.Reset();
	stInterruptStop.ulUniqueID = stRecv->st_c2p_interrupt_stop.ulUniqueID;

	/*��� ���� ����*/
	callerInst->RunWorkJob(ENG_BWOK::en_work_stop);

	uvEng_Philhmi_Send_C2P_INTERRUPT_STOP_ACK(stInterruptStop);
}