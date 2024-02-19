#include "pch.h"
#include "RecvPhil.h"

#include "RecipeManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

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
	::PostMessage(m_hMainWnd, WM_MAIN_PHIL_MSG, WPARAM(szTemp), 0);

	memcpy(stRecipe.job_name,		stRecv->st_c2p_rcp_create.stVar[0].szParameterValue,		DEF_MAX_RECIPE_PARAM_VALUE_LENGTH);
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

VOID CRecvPhil::PhilSendSelectRecipe(STG_PP_PACKET_RECV* stRecv)
{
	STG_PP_C2P_RCP_SELECT_ACK stSelectSend;
	stSelectSend.Reset();
	stSelectSend.ulUniqueID = stRecv->st_c2p_rcp_select.ulUniqueID;
	CUniToChar csCnv;
	CString strRecipe;

	strRecipe.Format(_T("%s"), csCnv.Ansi2Uni(stRecv->st_c2p_rcp_select.szRecipeName));

	uvEng_Philhmi_Send_C2P_RCP_SELECT_ACK(stSelectSend);

	/*Recipe ���� ��û ����*/
	if (CRecipeManager::GetInstance()->SelectRecipe(strRecipe))
	{
		UINT8 u8Offset = 0xff;
		//RunWorkJob(ENG_BWOK::en_gerb_load, PUINT64(&u8Offset));

	}
	/*Recipe ���� ��û ����*/
	else
	{
		//stSelectSend.usErrorCode = ePHILHMI_ERR_RECIPE_LOAD;
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
	strcpy_s(stStatus.szSelectedRecipeName, DEF_MAX_RECIPE_NAME_LENGTH, csCnv1.Ansi2UTF(uvEng_JobRecipe_GetSelectRecipe()->job_name));

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

	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();

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


VOID CRecvPhil::PhilSendStatusValue(STG_PP_PACKET_RECV* stRecv)
{
	STG_PP_C2P_STATUS_VALUE_ACK		stStatusValue;
	stStatusValue.Reset();

	/*������� LED ���� �� LED �µ� ����*/
	uvEng_Luria_ReqGetPhLedTempAll();
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


		TCHAR szTemp[512] = { NULL };
		/*DlgPhilhmi ���� �޽��� Ȯ��*/
		for (int i = 0; i < u8Count; i++)
		{
				swprintf_s(szTemp, 512, L"Type:%S  Name:%S   Value%S",
					stStatusValue.stVar[i].szParameterType,
					stStatusValue.stVar[i].szParameterName,
					stStatusValue.stVar[i].szParameterValue);

				::PostMessage(m_hMainWnd, WM_MAIN_PHIL_MSG, WPARAM(szTemp), 0);
		}

	}


	stStatusValue.ulUniqueID = stRecv->st_c2p_status_value.ulUniqueID;
	stStatusValue.usCount = u8Count;
	uvEng_Philhmi_Send_C2P_STATUS_VALUE_ACK(stStatusValue);
}

VOID CRecvPhil::PhilSendChageMode(STG_PP_PACKET_RECV* stRecv)
{
	STG_PP_C2P_MODE_CHANGE_ACK stModeChange;
	stModeChange.Reset();

	/*��� ����*/
	stModeChange.usMode = stRecv->st_c2p_mode_change.usMode;
	/*���� ����*/
	g_u8Romote = stRecv->st_c2p_mode_change.usMode;

	/*���� �ʱ�ȭ ���� ����*/
	if (stRecv->st_c2p_mode_change.usMode == en_menu_phil_mode_stop)
	{
		/*��� �ʱ�ȭ ����*/
		//RunWorkJob(ENG_BWOK::en_work_init);
		::PostMessage(m_hMainWnd, WM_MAIN_THREAD, WPARAM(ENG_BWOK::en_work_init), 0);


	}
	/*���� �޴��� ���� ����*/
	else if (stRecv->st_c2p_mode_change.usMode == en_menu_phil_mode_stop)
	{
		//CreateMenu(IDC_MAIN_BTN_MANUAL);
		::PostMessage(m_hMainWnd, WM_MAIN_CHILD, WPARAM(ENG_CMDI::en_menu_manual), 0);
	}
	/*���� ���� ���� ����*/
	else if (stRecv->st_c2p_mode_change.usMode == en_menu_phil_mode_auto)
	{
		//CreateMenu(IDC_MAIN_BTN_AUTO);
		::PostMessage(m_hMainWnd, WM_MAIN_CHILD, WPARAM(ENG_CMDI::en_menu_auto), 0);
	}

	stModeChange.ulUniqueID = stRecv->st_c2p_mode_change.ulUniqueID;
	uvEng_Philhmi_Send_C2P_MODE_CHANGE_ACK(stModeChange);
}

