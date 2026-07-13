#include "pch.h"
#include "RecipeManager.h"
#include "../DlgMain.h"
#include "zip.h"
#include "unzip.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CRecipeManager::CRecipeManager()
{
	m_strRecipeName[eRECIPE_MODE_SEL] = _T("");
	m_strRecipeName[eRECIPE_MODE_VIEW] = _T("");	
	m_strExpoRecipeName[eRECIPE_MODE_SEL] = _T("");
	m_strExpoRecipeName[eRECIPE_MODE_VIEW] = _T("");
	m_strAlignRecipeName[eRECIPE_MODE_SEL] = _T("");
	m_strAlignRecipeName[eRECIPE_MODE_VIEW] = _T("");

	//m_strRecipePath = _T("\\data\\recipe\\");
	m_strRecipePath.Format(_T("\\%s\\recipe\\"), CUSTOM_DATA_CONFIG);
	m_hMainWnd = NULL;

	/*레시피 용량 관리 스레드*/
	m_pSyncThread = NULL;
	m_bStopThread = FALSE;
}


CRecipeManager::~CRecipeManager()
{

}

CRecipe* CRecipeManager::GetRecipe(EN_RECIPE_MODE eRecipeMode)
{
	return &m_clsRcp[eRecipeMode];
}

void CRecipeManager::Init(HWND hWnd, CDlgMain* maindlgPtr)
{
	m_hMainWnd = hWnd;
	this->mainDlgPtr = maindlgPtr;
	//레시피폼 생성
	LoadRecipeForm(eRECIPE_MODE_SEL);
	LoadRecipeForm(eRECIPE_MODE_VIEW);
	
	LoadRecipeList();

	PTCHAR lastHostRcp = uvEng_GetHostRecipeConfig();
	if (SelectRecipe(lastHostRcp, eRECIPE_MODE_SEL_FROM_INITIAL)) //최초 시작시엔 사고를 막기위해 무조건 호스트레시피로 동기화
	{
		uvEng_JobRecipe_SelRecipeOnlyName(lastHostRcp, true);;
		SetRecipeName(lastHostRcp, eRECIPE_MODE_LOCAL);
	}

	
	
//	PTCHAR lastLocalRcp = uvEng_GetLocalRecipeConfig();
//	uvEng_JobRecipe_SelRecipeOnlyName(lastLocalRcp,true); //local rcp도 같은레시피로 등록 , 초기화라서 가능한것.
//	SetRecipeName(lastLocalRcp,eRECIPE_MODE_LOCAL);
}

void CRecipeManager::Destroy()
{
}

CString CRecipeManager::GetRecipePath()
{
	return g_tzWorkDir + m_strRecipePath;
}

void CRecipeManager::LoadRecipeList()
{
	uvEng_JobRecipe_ReloadFile();
	int nCount = uvEng_JobRecipe_GetCount();
	m_strArrRecipeList.RemoveAll();

	/*philhmi plus*/
	/*Philhmil에 Reicpe list 구조체 선언*/
	//STG_PP_C2P_RCP_LIST_ACK	stStatus;
	//stStatus.Reset();
	//stStatus.usCount = nCount;
	//stStatus.szArrRecipeName;
	CUniToChar csCnv1;

	for (int i = 0; i < nCount; i++)
	{
		LPG_RJAF pstRecipe = uvEng_JobRecipe_GetRecipeIndex(i);

		m_strArrRecipeList.Add((CString)pstRecipe->job_name);

		/*레시피 리스트 작성*/
		//strcpy_s(stStatus.szArrRecipeName[i], DEF_MAX_RECIPE_NAME_LENGTH, csCnv1.Ansi2UTF(pstRecipe->job_name));

	}

	/*Philhmil에 Reicpe list 전송*/
	//uvEng_Philhmi_Send_C2P_RCP_LIST_ACK(stStatus);
}

void CRecipeManager::LoadRecipeForm(EN_RECIPE_MODE eRecipeMode)
{
	m_clsRcp[eRecipeMode].LoadRecipeForm();
}

BOOL CRecipeManager::CreateRecipe(CString strRecipeName)
{
	STG_RJAF	stRecipe = { NULL };
	CUniToChar	csCnv;
	stRecipe.Init();

	strcpy_s(stRecipe.job_name, RECIPE_NAME_LENGTH, csCnv.Uni2Ansi(strRecipeName.GetBuffer())); strRecipeName.ReleaseBuffer();
	UpdateRecipe(stRecipe, eRECIPE_MODE_VIEW);
	BOOL bSuccess = uvEng_JobRecipe_RecipeAppend(&stRecipe);
	
	if (bSuccess == TRUE)
	{
		/*Recipe 파라미터 변경시 Log 기록*/
		TCHAR tzMsg[256] = { NULL };
		swprintf_s(tzMsg, 256, L"Create Recipe Name: %S", stRecipe.job_name);
		LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
		swprintf_s(tzMsg, 256, L"Gerber Name: %S, Align %S, Expo: %S", stRecipe.gerber_name, stRecipe.align_recipe, stRecipe.expo_recipe);
		LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
		swprintf_s(tzMsg, 256, L"Energy: %.4f, Thick: %d", stRecipe.expo_energy, stRecipe.material_thick);
		LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
	}

	stRecipe.Close();

	LoadRecipeList();
	LoadRecipe(strRecipeName, eRECIPE_MODE_VIEW);

	::SendMessage(m_hMainWnd, WM_MAIN_RECIPE_CREATE, NULL, (LPARAM)&strRecipeName);

	return bSuccess;
}

BOOL CRecipeManager::CreateRecipe(STG_RJAF stRecipe)
{
	CString strRecipeName;
	CUniToChar	csCnv;
	//stRecipe.Init();

	strRecipeName.Format(_T("%s"), csCnv.Ansi2Uni(stRecipe.job_name));
	
	//UpdateRecipe(stRecipe, eRECIPE_MODE_VIEW);
	BOOL bSuccess = uvEng_JobRecipe_RecipeAppend(&stRecipe);

	if (!bSuccess) return bSuccess;
	//stRecipe.Close();

	LoadRecipeList();
	LoadRecipe(strRecipeName, eRECIPE_MODE_VIEW);

	::SendMessage(m_hMainWnd, WM_MAIN_RECIPE_CREATE, NULL, (LPARAM)&strRecipeName);

	return bSuccess;
}

BOOL CRecipeManager::CreateExpoRecipe(CString strRecipeName)
{
	STG_REAF	stRecipe = { NULL };
	CUniToChar	csCnv;
	stRecipe.Init();

	UpdateExpoRecipe(stRecipe, eRECIPE_MODE_VIEW);
	BOOL bSuccess = uvEng_ExpoRecipe_RecipeAppend(&stRecipe);
	stRecipe.Close();

	::SendMessage(m_hMainWnd, WM_MAIN_RECIPE_CREATE, NULL, (LPARAM)&strRecipeName);

	return bSuccess;
}

BOOL CRecipeManager::CreateAlignRecipe(CString strRecipeName)
{
	STG_RAAF	stRecipe = { NULL };
	CUniToChar	csCnv;
	stRecipe.Init(2);

	UpdateAlignRecipe(stRecipe, eRECIPE_MODE_VIEW);
	BOOL bSuccess = uvEng_Mark_AlignRecipeAppend(&stRecipe, 0x00);
	stRecipe.Close();

	::SendMessage(m_hMainWnd, WM_MAIN_RECIPE_CREATE, NULL, (LPARAM)&strRecipeName);

	return bSuccess;
}

BOOL CRecipeManager::DeleteRecipe(CString strRecipeName)
{
	BOOL bSuccess = uvEng_JobRecipe_RecipeDelete(strRecipeName.GetBuffer()); strRecipeName.ReleaseBuffer();

	if (bSuccess == TRUE)
	{
		/*Recipe 파라미터 변경시 Log 기록*/
		TCHAR tzMsg[256] = { NULL };
		swprintf_s(tzMsg, 256, L"Delete Recipe Name: %S", strRecipeName);
	}
	
	::SendMessage(m_hMainWnd, WM_MAIN_RECIPE_DELETE, NULL, (LPARAM)&strRecipeName);
	return bSuccess;
}


BOOL CRecipeManager::DeleteExpoRecipe(CString strRecipeName)
{
	BOOL bSuccess = uvEng_ExpoRecipe_RecipeDelete(strRecipeName.GetBuffer()); strRecipeName.ReleaseBuffer();

	::SendMessage(m_hMainWnd, WM_MAIN_RECIPE_DELETE, NULL, (LPARAM)&strRecipeName);

	return bSuccess;
}

BOOL CRecipeManager::DeleteAlignRecipe(CString strRecipeName)
{
	BOOL bSuccess = uvEng_Mark_AlignRecipeDelete(strRecipeName.GetBuffer()); strRecipeName.ReleaseBuffer();

	::SendMessage(m_hMainWnd, WM_MAIN_RECIPE_DELETE, NULL, (LPARAM)&strRecipeName);

	return bSuccess;
}

BOOL CRecipeManager::SelectRecipe(CString strRecipeName, EN_RECIPE_SELECT_TYPE selType)
{

	
	uvEng_JobRecipe_SetWhatLastSelectIsLocal(selType == eRECIPE_MODE_SEL_FROM_LOCAL);

	

	BOOL bSuccess = uvEng_JobRecipe_SelRecipeOnlyName((PTCHAR)strRecipeName.GetString(), selType == eRECIPE_MODE_SEL_FROM_LOCAL);
	if (!bSuccess) return FALSE;

	CUniToChar csCnv;

	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetRecipeOnlyName((PTCHAR)strRecipeName.GetString());
	if (!pstRecipe) return FALSE;

	LPG_RAAF pstAlignRecipe = uvEng_Mark_GetAlignRecipeName(csCnv.Ansi2Uni(pstRecipe->align_recipe));
	LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstRecipe->expo_recipe));
	if (!pstAlignRecipe || !pstExpoRecipe) return FALSE;

	CHAR szJob[MAX_PATH_LEN] = { 0 };
	sprintf_s(szJob, MAX_PATH_LEN, "%s\\%s", pstRecipe->gerber_path, pstRecipe->gerber_name);
	CHAR szJobZip[MAX_PATH_LEN] = { 0 };
	sprintf_s(szJobZip, MAX_PATH_LEN, "%s\\%s.zip", pstRecipe->gerber_path, pstRecipe->gerber_name);

	if (!uvCmn_FindFile(csCnv.Ansi2Uni(szJob)) && !uvCmn_FindFile(csCnv.Ansi2Uni(szJobZip)))
	{
		AfxMessageBox(L"The gerber file registered in the recipe does not exist", MB_ICONWARNING);
		return FALSE;
	}

	LPG_PLPI pstPowerI = uvEng_LedPower_GetLedPowerName(csCnv.Ansi2Uni(pstExpoRecipe->power_name));
	if (!pstPowerI)
	{
		AfxMessageBox(L"The LED Power file registered in the recipe does not exist", MB_ICONWARNING);
		return FALSE;
	}

	UINT8 i = 0, j = 0;
	DOUBLE dbTotal = 0.0, dbPowerWatt[MAX_LED] = { 0 }, dbSpeed = 0.0;

	int phcnt = uvEng_GetConfig()->luria_svc.ph_count;
	for (; i < uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		for (j = 0; j < MAX_LED; j++)
			dbPowerWatt[j] = pstPowerI->led_watt[i][j];

		dbTotal += uvCmn_Luria_GetEnergyToSpeed(
			pstRecipe->step_size,
			pstRecipe->expo_energy,
			pstExpoRecipe->led_duty_cycle,
			dbPowerWatt);
	}
	pstRecipe->frame_rate = (UINT16)(dbTotal / (DOUBLE)i);

	if (pstAlignRecipe->align_type != (UINT8)ENG_ATGL::en_global_0_local_0x0_n_point)
	{
		if (!uvEng_ACamCali_LoadFile(pstRecipe->cali_thick))
		{
			if (IDNO == AfxMessageBox(L"Failed to load the calibration data for align camera\nDo you want to continue?", MB_YESNO))
				return FALSE;
		}
	}

	for (int i = 0; i < uvEng_GetConfig()->set_cams.acam_count; i++)
	{
		try
		{
			uvEng_Camera_SetGainLevel(i + 1, pstAlignRecipe->gain_level[i]);
		}
		catch (...) {}
	}

	uvCmn_Luria_ResetRegisteredJob();

	if (pstAlignRecipe->search_count == 1)
		uvEng_Camera_SetMultiMarkArea();
	else
		uvEng_Camera_SetMultiMarkArea(pstAlignRecipe->mark_area[0], pstAlignRecipe->mark_area[1]);

	uvEng_Camera_SetRecipeMarkRate(pstExpoRecipe->mark_score_accept, pstExpoRecipe->mark_scale_range);

	uvEng_JobRecipe_SetWhatLastSelectIsLocal(selType == eRECIPE_MODE_SEL_FROM_LOCAL);
	LoadRecipe(strRecipeName, selType == eRECIPE_MODE_SEL_FROM_LOCAL ? eRECIPE_MODE_LOCAL : eRECIPE_MODE_SEL);

	bSuccess = uvEng_Mark_GetAlignRecipeName(csCnv.Ansi2Uni(pstRecipe->align_recipe)) != nullptr;
	if (!bSuccess) return FALSE;

	const int GLOBAL_MARK_NAME_INDEX = 0;
	for (int index = 0; index < 2; index++)
	{
		UINT8 u8Speed = (UINT8)uvEng_GetConfig()->mark_find.model_speed;
		UINT8 u8Level = (UINT8)uvEng_GetConfig()->mark_find.detail_level;
		DOUBLE dbSmooth = (DOUBLE)uvEng_GetConfig()->mark_find.model_smooth;
		DOUBLE dbScaleMin = 0.0, dbScaleMax = 0.0, dbScoreRate = pstExpoRecipe->mark_score_accept;
		CUniToChar csCnv1, csCnv2, csCnv3, csCnv4, csCnv5;

		BOOL IsFind = FALSE;
		bool bpatFile = false, bmmfFile = false;
		CFileFind finder;

		//ST_RECIPE_PARAM stMarkParam = CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_SEL)->GetParam(EN_RECIPE_TAB::ALIGN, EN_RECIPE_ALIGN::GLOBAL_MARK_NAME + index);

		LPG_CMPV pstMark = uvEng_Mark_GetModelName(csCnv.Ansi2Uni(pstAlignRecipe->m_name[GLOBAL_MARK_NAME_INDEX]));

		TCHAR tzMsg[256] = { NULL };
		swprintf_s(tzMsg, 256, L"mark Recipe Param - name %s count : %d , size(um)", pstMark->name, pstAlignRecipe->search_count , pstMark->param[1]);
		LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);

		if (pstMark)
		{
			if (ENG_MMDT(pstMark->type) != ENG_MMDT::en_image)
			{
				bmmfFile = true;
				for (int i = 0; i < uvEng_GetConfig()->set_cams.acam_count; i++)
					uvEng_Camera_SetModelDefine_tot(i + 1, u8Speed, u8Level, pstAlignRecipe->search_count, dbSmooth,
						pstMark, GLOBAL_MARK + index, csCnv2.Ansi2Uni(pstMark->file),
						dbScaleMin, dbScaleMax, dbScoreRate);
			}
			else
			{
				CHAR tmpfile[MARK_MODEL_NAME_LENGTH];
				sprintf_s(tmpfile, MARK_MODEL_NAME_LENGTH, "%s\\%s\\mmf\\%s.mmf",
					csCnv1.Uni2Ansi(g_tzWorkDir), CUSTOM_DATA_CONFIG2, pstMark->file);
				IsFind = finder.FindFile(csCnv3.Ansi2Uni(tmpfile));
				bmmfFile = IsFind;
				if (bmmfFile)
					for (int i = 0; i < uvEng_GetConfig()->set_cams.acam_count; i++)
						uvEng_Camera_SetModelDefine_tot(i + 1, u8Speed, u8Level, uvEng_GetConfig()->mark_find.max_mark_find, dbSmooth,
							pstMark, GLOBAL_MARK + index, csCnv2.Ansi2Uni(tmpfile),
							dbScaleMin, dbScaleMax, dbScoreRate);

				sprintf_s(tmpfile, MARK_MODEL_NAME_LENGTH, "%s\\%s\\pat\\%s.pat",
					csCnv1.Uni2Ansi(g_tzWorkDir), CUSTOM_DATA_CONFIG2, pstMark->file);
				IsFind = finder.FindFile(csCnv5.Ansi2Uni(tmpfile));
				bpatFile = IsFind;
				if (bpatFile)
					for (int i = 0; i < uvEng_GetConfig()->set_cams.acam_count; i++)
						uvEng_Camera_SetModelDefine_tot(i + 1, u8Speed, u8Level, uvEng_GetConfig()->mark_find.max_mark_find, dbSmooth,
							pstMark, GLOBAL_MARK + index, csCnv2.Ansi2Uni(tmpfile),
							dbScaleMin, dbScaleMax, dbScoreRate);

				if (!bpatFile && !bmmfFile)
					return FALSE;
			}
		}
	}

	memset(mainDlgPtr->m_stExpoLog.host_recipe_name, 0x00, DEF_MAX_RECIPE_NAME_LENGTH);
	strcpy_s(mainDlgPtr->m_stExpoLog.host_recipe_name, CT2A(strRecipeName.GetString()));
	::SendMessage(m_hMainWnd, WM_MAIN_RECIPE_CHANGE, NULL, (LPARAM)&strRecipeName);

	return TRUE;
}


BOOL CRecipeManager::LoadSelectRecipe()
{
	
	LPG_RJAF pstRecipe = nullptr;
	
	pstRecipe = uvEng_JobRecipe_GetSelectRecipe(false);
	
	if (pstRecipe != nullptr)
	{
		LoadRecipe((CString)pstRecipe->job_name, eRECIPE_MODE_VIEW);
		LoadRecipe((CString)pstRecipe->job_name, eRECIPE_MODE_SEL);
	
	}
	
	pstRecipe = uvEng_JobRecipe_GetSelectRecipe(true);
	if (pstRecipe != nullptr)
	{
		LoadRecipe((CString)pstRecipe->job_name, eRECIPE_MODE_LOCAL);
	}

	

	return TRUE;
}

void CRecipeManager::SetRecipeName(CString strRecipeName, EN_RECIPE_MODE eRecipeMode)
{
	m_strRecipeName[eRecipeMode] = strRecipeName;
}

CString CRecipeManager::GetRecipeName(EN_RECIPE_MODE eRecipeMode)
{
	return m_strRecipeName[eRecipeMode];
}

void CRecipeManager::SetExpoRecipeName(CString strRecipeName, EN_RECIPE_MODE eRecipeMode)
{
	m_strExpoRecipeName[eRecipeMode] = strRecipeName;
}

CString CRecipeManager::GetExpoRecipeName(EN_RECIPE_MODE eRecipeMode)
{
	return m_strExpoRecipeName[eRecipeMode];
}

void CRecipeManager::SetAlignRecipeName(CString strRecipeName, EN_RECIPE_MODE eRecipeMode)
{
	m_strAlignRecipeName[eRecipeMode] = strRecipeName;
}

CString CRecipeManager::GetAlignRecipeName(EN_RECIPE_MODE eRecipeMode)
{
	return m_strAlignRecipeName[eRecipeMode];
}

BOOL CRecipeManager::SaveRecipe(CString strName, EN_RECIPE_MODE eRecipeMode)
{
	CString strGroup;
	CString strParam;
	CString strValue;
	CUniToChar csCnv;
	STG_RJAF stRecipe = { NULL };
	STG_REAF stExpoRecipe = { NULL };
	STG_RAAF stAlignRecipe = { NULL };
	stRecipe.Init();
	stExpoRecipe.Init();
	stAlignRecipe.Init(2);

	int nIndexScan = 0;
	int nMaxTab = GetRecipe(eRecipeMode)->GetTabCount();
	for (int nCntTab = 0; nCntTab < nMaxTab; nCntTab++)
	{
		//사용하지 않는 탭은 건너 뛴다.
		if (FALSE == GetRecipe(eRecipeMode)->GetTabUse(nCntTab))
			continue;

		int nMaxParam = GetRecipe(eRecipeMode)->GetParamCount(nCntTab);
		strGroup = GetRecipe(eRecipeMode)->GetTabName(nCntTab);

		for (int nCntParam = 0; nCntParam < nMaxParam; nCntParam++)
		{
			//사용하지 않는 파라미터는 저장하지 않는다.
			if (FALSE == GetRecipe(eRecipeMode)->GetUse(nCntTab, nCntParam))
				continue;

			strParam = GetRecipe(eRecipeMode)->GetParamName(nCntTab, nCntParam);
			strValue = GetRecipe(eRecipeMode)->GetValue(nCntTab, nCntParam);

			/*Recipe 파라미터 변경시 Log 기록*/
			TCHAR tzMsg[256] = { NULL };
			swprintf_s(tzMsg, 256, L"Save Recipe Param %s Value %s", strParam, strValue);
			LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);

			switch (nCntTab)
			{
			case EN_RECIPE_TAB::JOB:
			{
				switch (nCntParam)
				{
				case EN_RECIPE_JOB::JOB_NAME:
					strcpy_s(stRecipe.job_name, RECIPE_NAME_LENGTH, csCnv.Uni2Ansi(strValue.GetBuffer())); strValue.ReleaseBuffer();
					break;
				case EN_RECIPE_JOB::GERBER_PATH:
					strcpy_s(stRecipe.gerber_path, MAX_PATH_LEN, csCnv.Uni2Ansi(strValue.GetBuffer())); strValue.ReleaseBuffer();
					break;
				case EN_RECIPE_JOB::GERBER_NAME:
					strcpy_s(stRecipe.gerber_name, MAX_GERBER_NAME, csCnv.Uni2Ansi(strValue.GetBuffer())); strValue.ReleaseBuffer();
					break;
				
				case EN_RECIPE_JOB::MATERIAL_THICK:
					stRecipe.material_thick = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;

				case EN_RECIPE_JOB::LDS_THRESHOLD:
					stRecipe.ldsThreshold = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;

				/*case EN_RECIPE_JOB::LDS_BASEHEIGHT:
					stRecipe.ldsBaseHeight = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;*/


				case EN_RECIPE_JOB::EXPO_ENERGY:
					stRecipe.expo_energy = (float)GetRecipe(eRecipeMode)->GetDouble(nCntTab, nCntParam);
					break;
				case EN_RECIPE_JOB::ALIGN_RECIPE:
					strcpy_s(stRecipe.align_recipe, RECIPE_NAME_LENGTH, csCnv.Uni2Ansi(strValue.GetBuffer())); strValue.ReleaseBuffer();
					break;
				case EN_RECIPE_JOB::EXPO_RECIPE:
					strcpy_s(stRecipe.expo_recipe, RECIPE_NAME_LENGTH, csCnv.Uni2Ansi(strValue.GetBuffer())); strValue.ReleaseBuffer();
					break;
				}
			}
			break;
			case EN_RECIPE_TAB::EXPOSE:
			{
				switch (nCntParam)
				{
				case EN_RECIPE_EXPOSE::EXPO_NAME:
					strcpy_s(stExpoRecipe.expo_name, RECIPE_NAME_LENGTH, csCnv.Uni2Ansi(strValue.GetBuffer())); strValue.ReleaseBuffer();
					break;
				case EN_RECIPE_EXPOSE::POWER_NAME:
					strcpy_s(stExpoRecipe.power_name, LED_POWER_NAME_LENGTH, csCnv.Uni2Ansi(strValue.GetBuffer())); strValue.ReleaseBuffer();
					break;
				case EN_RECIPE_EXPOSE::MARK_ERR_DIST_RATE:
					stExpoRecipe.global_mark_dist_rate = GetRecipe(eRecipeMode)->GetDouble(nCntTab, nCntParam);
					break;
				case EN_RECIPE_EXPOSE::MARK_ERR_DIST_TOP_HORZ:
				case EN_RECIPE_EXPOSE::MARK_ERR_DIST_BTM_HORZ:
				case EN_RECIPE_EXPOSE::MARK_ERR_DIST_TOP_VERT:
				case EN_RECIPE_EXPOSE::MARK_ERR_DIST_BTM_VERT:
				case EN_RECIPE_EXPOSE::MARK_ERR_DIST_LFT_DIAG:
				case EN_RECIPE_EXPOSE::MARK_ERR_DIST_RGT_DIAG:
					stExpoRecipe.global_mark_dist[nCntParam - EN_RECIPE_EXPOSE::MARK_ERR_DIST_TOP_HORZ] = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;

				case EN_RECIPE_EXPOSE::LED_DUTY_CYCLE:
					stExpoRecipe.led_duty_cycle = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;
				case EN_RECIPE_EXPOSE::SCORE_ACCEPT:
					stExpoRecipe.mark_score_accept = GetRecipe(eRecipeMode)->GetDouble(nCntTab, nCntParam);
					break;
				case EN_RECIPE_EXPOSE::SCALE_RANGE:
					stExpoRecipe.mark_scale_range = GetRecipe(eRecipeMode)->GetDouble(nCntTab, nCntParam);
					break;
				case EN_RECIPE_EXPOSE::SERIAL_DECODE:
					stExpoRecipe.dcode_serial = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;
				case EN_RECIPE_EXPOSE::SCALE_DECODE:
					stExpoRecipe.dcode_scale = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;
				case EN_RECIPE_EXPOSE::TEXT_DECODE:
					stExpoRecipe.dcode_text = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;
				case EN_RECIPE_EXPOSE::TEXT_STRING:
					strcpy_s(stExpoRecipe.text_string, MAX_PANEL_TEXT_STRING, csCnv.Uni2Ansi(strValue.GetBuffer())); strValue.ReleaseBuffer();
					break;
				case EN_RECIPE_EXPOSE::SERIAL_FLIP_HORZ:
					stExpoRecipe.serial_flip_h = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;
				case EN_RECIPE_EXPOSE::SERIAL_FLIP_VERT:
					stExpoRecipe.serial_flip_v = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;
				case EN_RECIPE_EXPOSE::SCALE_FLIP_HORZ:
					stExpoRecipe.scale_flip_h = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;
				case EN_RECIPE_EXPOSE::SCALE_FLIP_VERT:
					stExpoRecipe.scale_flip_v = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;
				case EN_RECIPE_EXPOSE::TEXT_FLIP_HORZ:
					stExpoRecipe.text_flip_h = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;
				case EN_RECIPE_EXPOSE::TEXT_FLIP_VERT:
					stExpoRecipe.text_flip_v = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;

				case EN_RECIPE_EXPOSE::MATERIAL_TYPE:
					stExpoRecipe.headOffset = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;
				}
			}
			break;
			case EN_RECIPE_TAB::ALIGN:
			{

				switch (nCntParam)
				{
				case EN_RECIPE_ALIGN::ALIGN_NAME:
					strcpy_s(stAlignRecipe.align_name, RECIPE_NAME_LENGTH, csCnv.Uni2Ansi(strValue.GetBuffer())); strValue.ReleaseBuffer();
					break;
				case EN_RECIPE_ALIGN::MARK_TYPE: 
					stAlignRecipe.mark_type = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;
				case EN_RECIPE_ALIGN::ALIGN_TYPE: 
					stAlignRecipe.align_type = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;

				case EN_RECIPE_ALIGN::ALIGN_MOTION:
				{
					stAlignRecipe.align_motion = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				}
				break;


				case EN_RECIPE_ALIGN::LAMP_TYPE: stAlignRecipe.lamp_type = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;
				case EN_RECIPE_ALIGN::LAMP_AMBER_CAM1:
				case EN_RECIPE_ALIGN::LAMP_AMBER_CAM2:
				case EN_RECIPE_ALIGN::LAMP_IR_CAM1:
				case EN_RECIPE_ALIGN::LAMP_IR_CAM2:
				case EN_RECIPE_ALIGN::LAMP_COAXIAL_CAM1:
				case EN_RECIPE_ALIGN::LAMP_COAXIAL_CAM2:
					stAlignRecipe.lamp_value[nCntParam - EN_RECIPE_ALIGN::LAMP_AMBER_CAM1] = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;
				case EN_RECIPE_ALIGN::GAIN_LEVEL_CAM1:
				case EN_RECIPE_ALIGN::GAIN_LEVEL_CAM2:
					stAlignRecipe.gain_level[nCntParam - EN_RECIPE_ALIGN::GAIN_LEVEL_CAM1] = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;

				case EN_RECIPE_ALIGN::SEARCH_TYPE: 
					
				break;

				case EN_RECIPE_ALIGN::SEARCH_COUNT: 

					stAlignRecipe.search_count = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					stAlignRecipe.search_type = stAlignRecipe.search_count == 1 ? 1 : 3; //3 은 멀티온리// GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;

				case EN_RECIPE_ALIGN::MARK_AREA_WIDTH: stAlignRecipe.mark_area[0] = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;
				case EN_RECIPE_ALIGN::MARK_AREA_HEIGHT: stAlignRecipe.mark_area[1] = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;
					
 				case EN_RECIPE_ALIGN::ALIGN_CAMERA_NUMBER1: stAlignRecipe.acam_num[0] = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
 					break;
				case EN_RECIPE_ALIGN::ALIGN_CAMERA_NUMBER2: stAlignRecipe.acam_num[1] = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;
 				case EN_RECIPE_ALIGN::GLOBAL_MARK_NAME: 
					strcpy_s(stAlignRecipe.m_name[0], MARK_MODEL_NAME_LENGTH, csCnv.Uni2Ansi(strValue.GetBuffer())); strValue.ReleaseBuffer();
					break;
				case EN_RECIPE_ALIGN::LOCAL_MARK_NAME: 
					strcpy_s(stAlignRecipe.m_name[1], MARK_MODEL_NAME_LENGTH, csCnv.Uni2Ansi(strValue.GetBuffer())); strValue.ReleaseBuffer();
					break;
				}
			}
			break;
			}
		}
	}
	/*Philhmi에 보고*/
	//PhilSendModifyRecipe(stRecipe);

	uvEng_JobRecipe_RecipeModify(&stRecipe);
	uvEng_ExpoRecipe_RecipeModify(&stExpoRecipe);
	uvEng_Mark_AlignRecipeModify(&stAlignRecipe);
	stRecipe.Close();
	stExpoRecipe.Close();
	stAlignRecipe.Close();
	return TRUE;
}
BOOL CRecipeManager::UpdateRecipe(STG_RJAF& stRecipe, EN_RECIPE_MODE eRecipeMode)
{
	CString strGroup;
	CString strParam;
	CString strValue;
	CUniToChar csCnv;

	int nIndexScan = 0;
	int nMaxTab = GetRecipe(eRecipeMode)->GetTabCount();
	for (int nCntTab = 0; nCntTab < nMaxTab; nCntTab++)
	{
		//사용하지 않는 탭은 건너 뛴다.
		if (FALSE == GetRecipe(eRecipeMode)->GetTabUse(nCntTab))
			continue;

		int nMaxParam = GetRecipe(eRecipeMode)->GetParamCount(nCntTab);
		strGroup = GetRecipe(eRecipeMode)->GetTabName(nCntTab);

		for (int nCntParam = 0; nCntParam < nMaxParam; nCntParam++)
		{
			//사용하지 않는 파라미터는 저장하지 않는다.
			if (FALSE == GetRecipe(eRecipeMode)->GetUse(nCntTab, nCntParam))
				continue;

			if (EN_RECIPE_TAB::JOB != nCntTab)
			{
				continue;
			}

			strParam = GetRecipe(eRecipeMode)->GetParamName(nCntTab, nCntParam);
			strValue = GetRecipe(eRecipeMode)->GetValue(nCntTab, nCntParam);

			switch (nCntParam)
			{
			case EN_RECIPE_JOB::GERBER_PATH:
				strcpy_s(stRecipe.gerber_path, MAX_PATH_LEN, csCnv.Uni2Ansi(strValue.GetBuffer())); strValue.ReleaseBuffer();
				break;
			case EN_RECIPE_JOB::GERBER_NAME:
				strcpy_s(stRecipe.gerber_name, MAX_GERBER_NAME, csCnv.Uni2Ansi(strValue.GetBuffer())); strValue.ReleaseBuffer();
				break;
			case EN_RECIPE_JOB::MATERIAL_THICK:
				stRecipe.material_thick = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;

			case EN_RECIPE_JOB::LDS_THRESHOLD:
				stRecipe.ldsThreshold = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;

			/*case EN_RECIPE_JOB::LDS_BASEHEIGHT:
				stRecipe.ldsBaseHeight = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;*/


			case EN_RECIPE_JOB::EXPO_ENERGY:
				stRecipe.expo_energy = (float)GetRecipe(eRecipeMode)->GetDouble(nCntTab, nCntParam);
				break;
			case EN_RECIPE_JOB::ALIGN_RECIPE:
				strcpy_s(stRecipe.align_recipe, RECIPE_NAME_LENGTH, csCnv.Uni2Ansi(strValue.GetBuffer())); strValue.ReleaseBuffer();
				break;
			case EN_RECIPE_JOB::EXPO_RECIPE:
				strcpy_s(stRecipe.expo_recipe, RECIPE_NAME_LENGTH, csCnv.Uni2Ansi(strValue.GetBuffer())); strValue.ReleaseBuffer();
				break;
			}
		}
	}

	return TRUE;
}


BOOL CRecipeManager::UpdateExpoRecipe(STG_REAF& stRecipe, EN_RECIPE_MODE eRecipeMode)
{
	CString strGroup;
	CString strParam;
	CString strValue;
	CUniToChar csCnv;

	int nIndexScan = 0;
	int nMaxTab = GetRecipe(eRecipeMode)->GetTabCount();
	for (int nCntTab = 0; nCntTab < nMaxTab; nCntTab++)
	{
		//사용하지 않는 탭은 건너 뛴다.
		if (FALSE == GetRecipe(eRecipeMode)->GetTabUse(nCntTab))
			continue;

		int nMaxParam = GetRecipe(eRecipeMode)->GetParamCount(nCntTab);
		strGroup = GetRecipe(eRecipeMode)->GetTabName(nCntTab);

		for (int nCntParam = 0; nCntParam < nMaxParam; nCntParam++)
		{
			//사용하지 않는 파라미터는 저장하지 않는다.
			if (FALSE == GetRecipe(eRecipeMode)->GetUse(nCntTab, nCntParam))
				continue;

			if (EN_RECIPE_TAB::EXPOSE != nCntTab)
			{
				continue;
			}

			strParam = GetRecipe(eRecipeMode)->GetParamName(nCntTab, nCntParam);
			strValue = GetRecipe(eRecipeMode)->GetValue(nCntTab, nCntParam);

			switch (nCntParam)
			{
			case EN_RECIPE_EXPOSE::EXPO_NAME:
				strcpy_s(stRecipe.expo_name, RECIPE_NAME_LENGTH, csCnv.Uni2Ansi(strValue.GetBuffer())); strValue.ReleaseBuffer();
				break;
			case EN_RECIPE_EXPOSE::POWER_NAME:
				strcpy_s(stRecipe.power_name, LED_POWER_NAME_LENGTH, csCnv.Uni2Ansi(strValue.GetBuffer())); strValue.ReleaseBuffer();
				break;
			case EN_RECIPE_EXPOSE::MARK_ERR_DIST_RATE:
				stRecipe.global_mark_dist_rate = GetRecipe(eRecipeMode)->GetDouble(nCntTab, nCntParam);
				break;
			case EN_RECIPE_EXPOSE::MARK_ERR_DIST_TOP_HORZ:
			case EN_RECIPE_EXPOSE::MARK_ERR_DIST_BTM_HORZ:
			case EN_RECIPE_EXPOSE::MARK_ERR_DIST_TOP_VERT:
			case EN_RECIPE_EXPOSE::MARK_ERR_DIST_BTM_VERT:
			case EN_RECIPE_EXPOSE::MARK_ERR_DIST_LFT_DIAG:
			case EN_RECIPE_EXPOSE::MARK_ERR_DIST_RGT_DIAG:
				stRecipe.global_mark_dist[nCntParam - EN_RECIPE_EXPOSE::MARK_ERR_DIST_TOP_HORZ] = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;

			case EN_RECIPE_EXPOSE::LED_DUTY_CYCLE:
				stRecipe.led_duty_cycle = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;
			case EN_RECIPE_EXPOSE::SCORE_ACCEPT:
				stRecipe.mark_score_accept = GetRecipe(eRecipeMode)->GetDouble(nCntTab, nCntParam);
				break;
			case EN_RECIPE_EXPOSE::SCALE_RANGE:
				stRecipe.mark_scale_range = GetRecipe(eRecipeMode)->GetDouble(nCntTab, nCntParam);
				break;
			case EN_RECIPE_EXPOSE::SERIAL_DECODE:
				stRecipe.dcode_serial = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;
			case EN_RECIPE_EXPOSE::SCALE_DECODE:
				stRecipe.dcode_scale = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;
			case EN_RECIPE_EXPOSE::TEXT_DECODE:
				stRecipe.dcode_text = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;
			case EN_RECIPE_EXPOSE::TEXT_STRING:
				strcpy_s(stRecipe.text_string, MAX_PANEL_TEXT_STRING, csCnv.Uni2Ansi(strValue.GetBuffer())); strValue.ReleaseBuffer();
				break;
			case EN_RECIPE_EXPOSE::SERIAL_FLIP_HORZ:
				stRecipe.serial_flip_h = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;
			case EN_RECIPE_EXPOSE::SERIAL_FLIP_VERT:
				stRecipe.serial_flip_v = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;
			case EN_RECIPE_EXPOSE::SCALE_FLIP_HORZ:
				stRecipe.scale_flip_h = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;
			case EN_RECIPE_EXPOSE::SCALE_FLIP_VERT:
				stRecipe.scale_flip_v = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;
			case EN_RECIPE_EXPOSE::TEXT_FLIP_HORZ:
				stRecipe.text_flip_h = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;
			case EN_RECIPE_EXPOSE::TEXT_FLIP_VERT:
				stRecipe.text_flip_v = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;

			case EN_RECIPE_EXPOSE::MATERIAL_TYPE:
				stRecipe.headOffset = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;
			}
		}
	}

	return TRUE;
}


BOOL CRecipeManager::UpdateAlignRecipe(STG_RAAF& stRecipe, EN_RECIPE_MODE eRecipeMode /*= eRECIPE_MODE_SEL*/)
{
	CString strGroup;
	CString strParam;
	CString strValue;
	CUniToChar csCnv;

	int nIndexScan = 0;
	int nMaxTab = GetRecipe(eRecipeMode)->GetTabCount();
	for (int nCntTab = 0; nCntTab < nMaxTab; nCntTab++)
	{
		//사용하지 않는 탭은 건너 뛴다.
		if (FALSE == GetRecipe(eRecipeMode)->GetTabUse(nCntTab))
			continue;

		int nMaxParam = GetRecipe(eRecipeMode)->GetParamCount(nCntTab);
		strGroup = GetRecipe(eRecipeMode)->GetTabName(nCntTab);

		for (int nCntParam = 0; nCntParam < nMaxParam; nCntParam++)
		{
			//사용하지 않는 파라미터는 저장하지 않는다.
			if (FALSE == GetRecipe(eRecipeMode)->GetUse(nCntTab, nCntParam))
				continue;

			if (EN_RECIPE_TAB::ALIGN != nCntTab)
			{
				continue;
			}

			strParam = GetRecipe(eRecipeMode)->GetParamName(nCntTab, nCntParam);
			strValue = GetRecipe(eRecipeMode)->GetValue(nCntTab, nCntParam);

			switch (nCntParam)
			{
			case EN_RECIPE_ALIGN::ALIGN_NAME:
				strcpy_s(stRecipe.align_name, RECIPE_NAME_LENGTH, csCnv.Uni2Ansi(strValue.GetBuffer())); strValue.ReleaseBuffer();
				break;
			case EN_RECIPE_ALIGN::MARK_TYPE: stRecipe.mark_type = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;
			case EN_RECIPE_ALIGN::ALIGN_TYPE: stRecipe.align_type = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;
			case EN_RECIPE_ALIGN::ALIGN_MOTION: stRecipe.align_motion = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;
			case EN_RECIPE_ALIGN::LAMP_TYPE: stRecipe.lamp_type = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;
			case EN_RECIPE_ALIGN::LAMP_AMBER_CAM1:
			case EN_RECIPE_ALIGN::LAMP_AMBER_CAM2:
			case EN_RECIPE_ALIGN::LAMP_IR_CAM1:
			case EN_RECIPE_ALIGN::LAMP_IR_CAM2:
			case EN_RECIPE_ALIGN::LAMP_COAXIAL_CAM1:
			case EN_RECIPE_ALIGN::LAMP_COAXIAL_CAM2:
				stRecipe.lamp_value[nCntParam - EN_RECIPE_ALIGN::LAMP_AMBER_CAM1] = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;
			case EN_RECIPE_ALIGN::GAIN_LEVEL_CAM1:
			case EN_RECIPE_ALIGN::GAIN_LEVEL_CAM2:
				stRecipe.gain_level[nCntParam - EN_RECIPE_ALIGN::GAIN_LEVEL_CAM1] = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;
			case EN_RECIPE_ALIGN::SEARCH_TYPE: stRecipe.search_type = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;
			case EN_RECIPE_ALIGN::SEARCH_COUNT: stRecipe.search_count = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;
			case EN_RECIPE_ALIGN::MARK_AREA_WIDTH: stRecipe.mark_area[0] = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;
			case EN_RECIPE_ALIGN::MARK_AREA_HEIGHT: stRecipe.mark_area[1] = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;
			case EN_RECIPE_ALIGN::ALIGN_CAMERA_NUMBER1: stRecipe.acam_num[0] = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;
			case EN_RECIPE_ALIGN::ALIGN_CAMERA_NUMBER2: stRecipe.acam_num[1] = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
				break;
			case EN_RECIPE_ALIGN::GLOBAL_MARK_NAME:
				strcpy_s(stRecipe.m_name[0], MARK_MODEL_NAME_LENGTH, csCnv.Uni2Ansi(strValue.GetBuffer())); strValue.ReleaseBuffer();
				break;
			case EN_RECIPE_ALIGN::LOCAL_MARK_NAME:
				strcpy_s(stRecipe.m_name[1], MARK_MODEL_NAME_LENGTH, csCnv.Uni2Ansi(strValue.GetBuffer())); strValue.ReleaseBuffer();
				break;
			}
		}
	}

	return TRUE;
}

BOOL CRecipeManager::LoadRecipe(CString strName, EN_RECIPE_MODE eRecipeMode)
{
	if (_T("") == strName)
		return FALSE;

	CUniToChar csCnv;
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetRecipeOnlyName(strName.GetBuffer()); strName.ReleaseBuffer();

	if (NULL == pstRecipe)
	{
		return FALSE;
	}

	LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstRecipe->expo_recipe));
	LPG_RAAF pstAlignRecipe = uvEng_Mark_GetAlignRecipeName(csCnv.Ansi2Uni(pstRecipe->align_recipe));

	if (NULL == pstRecipe || NULL == pstExpoRecipe || NULL == pstAlignRecipe)
	{
		return FALSE;
	}

	SetRecipeName(strName, eRecipeMode);
	SetExpoRecipeName(csCnv.Ansi2Uni(pstRecipe->expo_recipe), eRecipeMode);
	SetAlignRecipeName(csCnv.Ansi2Uni(pstRecipe->align_recipe), eRecipeMode);

	CString strValue;

	int nIndexScan = 0;
	int nMaxTab = GetRecipe(eRecipeMode)->GetTabCount();
	for (int nCntTab = 0; nCntTab < nMaxTab; nCntTab++)
	{
		ST_RECIPE_TAB stTab = GetRecipe(eRecipeMode)->GetTab(nCntTab);
		//사용하지 않는 탭은 건너 뛴다.
		if (FALSE == stTab.bUse)
			continue;

		for (int nCntParam = 0; nCntParam < stTab.GetParamCount(); nCntParam++)
		{
			ST_RECIPE_PARAM stParam = GetRecipe(eRecipeMode)->GetParam(nCntTab, nCntParam);

			//사용하지 않는 파라미터는 읽지 않는다.(LoadRecipeForm의 Value값이 Value,DefaultValue로 설정되어있는상태) 
			if (FALSE == stParam.bUse)
				continue;

			switch (nCntTab)
			{
			case EN_RECIPE_TAB::JOB:
			{
				switch (nCntParam)
				{
				case EN_RECIPE_JOB::JOB_NAME:
					stParam.SetValue(pstRecipe->job_name);
					break;
				case EN_RECIPE_JOB::GERBER_PATH:
					stParam.SetValue(pstRecipe->gerber_path);
					break;
				case EN_RECIPE_JOB::GERBER_NAME:
					stParam.SetValue(pstRecipe->gerber_name);
					break;
				case EN_RECIPE_JOB::MATERIAL_THICK:

					stParam.SetValue(pstRecipe->material_thick);
					break;

				case EN_RECIPE_JOB::LDS_THRESHOLD:
					stParam.SetValue(pstRecipe->ldsThreshold);
					break;

				/*case EN_RECIPE_JOB::LDS_BASEHEIGHT:
					stParam.SetValue(pstRecipe->ldsBaseHeight);
					break;*/

				case EN_RECIPE_JOB::EXPO_ENERGY:
					stParam.SetValue(pstRecipe->expo_energy);
					break;
				case EN_RECIPE_JOB::ALIGN_RECIPE:
					stParam.SetValue(pstRecipe->align_recipe);
					break;
				case EN_RECIPE_JOB::EXPO_RECIPE:
					stParam.SetValue(pstRecipe->expo_recipe);
					break;
				}
			}
			break;
			case EN_RECIPE_TAB::EXPOSE:
			{
				switch (nCntParam)
				{
				case EN_RECIPE_EXPOSE::EXPO_NAME:
					stParam.SetValue(pstExpoRecipe->expo_name);
					break;
				case EN_RECIPE_EXPOSE::POWER_NAME:
					stParam.SetValue(pstExpoRecipe->power_name);
					break;
				case EN_RECIPE_EXPOSE::MARK_ERR_DIST_RATE:
					stParam.SetValue(pstExpoRecipe->global_mark_dist_rate);
					break;
				case EN_RECIPE_EXPOSE::MARK_ERR_DIST_TOP_HORZ:
					stParam.SetValue(pstExpoRecipe->global_mark_dist[0]);
					break;
				case EN_RECIPE_EXPOSE::MARK_ERR_DIST_BTM_HORZ:
					stParam.SetValue(pstExpoRecipe->global_mark_dist[1]);
					break;
				case EN_RECIPE_EXPOSE::MARK_ERR_DIST_TOP_VERT:
					stParam.SetValue(pstExpoRecipe->global_mark_dist[2]);
					break;
				case EN_RECIPE_EXPOSE::MARK_ERR_DIST_BTM_VERT:
					stParam.SetValue(pstExpoRecipe->global_mark_dist[3]);
					break;
				case EN_RECIPE_EXPOSE::MARK_ERR_DIST_LFT_DIAG:
					stParam.SetValue(pstExpoRecipe->global_mark_dist[4]);
					break;
				case EN_RECIPE_EXPOSE::MARK_ERR_DIST_RGT_DIAG:
					stParam.SetValue(pstExpoRecipe->global_mark_dist[5]);
					break;
				case EN_RECIPE_EXPOSE::LED_DUTY_CYCLE:
					stParam.SetValue(pstExpoRecipe->led_duty_cycle);
					break;
				case EN_RECIPE_EXPOSE::SCORE_ACCEPT:
					stParam.SetValue(pstExpoRecipe->mark_score_accept);
					break;
				case EN_RECIPE_EXPOSE::SCALE_RANGE:
					stParam.SetValue(pstExpoRecipe->mark_scale_range);
					break;
				case EN_RECIPE_EXPOSE::SERIAL_DECODE:
					stParam.SetValue(pstExpoRecipe->dcode_serial);
					break;
				case EN_RECIPE_EXPOSE::SCALE_DECODE:
					stParam.SetValue(pstExpoRecipe->dcode_scale);
					break;
				case EN_RECIPE_EXPOSE::TEXT_DECODE:
					stParam.SetValue(pstExpoRecipe->dcode_text);
					break;
				case EN_RECIPE_EXPOSE::TEXT_STRING:
					stParam.SetValue(pstExpoRecipe->text_string);
					break;
				case EN_RECIPE_EXPOSE::SERIAL_FLIP_HORZ:
					stParam.SetValue(pstExpoRecipe->serial_flip_h);
					break;
				case EN_RECIPE_EXPOSE::SERIAL_FLIP_VERT:
					stParam.SetValue(pstExpoRecipe->serial_flip_v);
					break;
				case EN_RECIPE_EXPOSE::SCALE_FLIP_HORZ:
					stParam.SetValue(pstExpoRecipe->scale_flip_h);
					break;
				case EN_RECIPE_EXPOSE::SCALE_FLIP_VERT:
					stParam.SetValue(pstExpoRecipe->scale_flip_v);
					break;
				case EN_RECIPE_EXPOSE::TEXT_FLIP_HORZ:
					stParam.SetValue(pstExpoRecipe->text_flip_h);
					break;
				case EN_RECIPE_EXPOSE::TEXT_FLIP_VERT:
					stParam.SetValue(pstExpoRecipe->text_flip_v);
					break;

				case EN_RECIPE_EXPOSE::MATERIAL_TYPE:
					stParam.SetValue(pstExpoRecipe->headOffset);
					break;
				}
			}
			break;
			case EN_RECIPE_TAB::ALIGN:
			{

				switch (nCntParam)
				{
				case EN_RECIPE_ALIGN::ALIGN_NAME: stParam.SetValue(pstAlignRecipe->align_name);
					break;
				case EN_RECIPE_ALIGN::MARK_TYPE: stParam.SetValue(pstAlignRecipe->mark_type);
					break;
				case EN_RECIPE_ALIGN::ALIGN_TYPE: stParam.SetValue(pstAlignRecipe->align_type);
					break;
				case EN_RECIPE_ALIGN::LAMP_TYPE: stParam.SetValue(pstAlignRecipe->lamp_type);
					break;
				case EN_RECIPE_ALIGN::LAMP_AMBER_CAM1:
				case EN_RECIPE_ALIGN::LAMP_AMBER_CAM2:
				case EN_RECIPE_ALIGN::LAMP_IR_CAM1:
				case EN_RECIPE_ALIGN::LAMP_IR_CAM2:
				case EN_RECIPE_ALIGN::LAMP_COAXIAL_CAM1:
				case EN_RECIPE_ALIGN::LAMP_COAXIAL_CAM2:
					stParam.SetValue(pstAlignRecipe->lamp_value[nCntParam - EN_RECIPE_ALIGN::LAMP_AMBER_CAM1]);

				case EN_RECIPE_ALIGN::ALIGN_MOTION:
					stParam.SetValue(pstAlignRecipe->align_motion);
					break;

				case EN_RECIPE_ALIGN::GAIN_LEVEL_CAM1:
				case EN_RECIPE_ALIGN::GAIN_LEVEL_CAM2:
					stParam.SetValue(pstAlignRecipe->gain_level[nCntParam - EN_RECIPE_ALIGN::GAIN_LEVEL_CAM1]);
					break;
				case EN_RECIPE_ALIGN::SEARCH_TYPE: stParam.SetValue(pstAlignRecipe->search_type);
					break;
				case EN_RECIPE_ALIGN::SEARCH_COUNT: stParam.SetValue(pstAlignRecipe->search_count);
					break;
				case EN_RECIPE_ALIGN::MARK_AREA_WIDTH: stParam.SetValue(pstAlignRecipe->mark_area[0]);
					break;
				case EN_RECIPE_ALIGN::MARK_AREA_HEIGHT: stParam.SetValue(pstAlignRecipe->mark_area[1]);
					break;
				case EN_RECIPE_ALIGN::ALIGN_CAMERA_NUMBER1: stParam.SetValue(pstAlignRecipe->acam_num[0]);
					break;
				case EN_RECIPE_ALIGN::ALIGN_CAMERA_NUMBER2: stParam.SetValue(pstAlignRecipe->acam_num[1]);
					break;
				case EN_RECIPE_ALIGN::GLOBAL_MARK_NAME: 
					stParam.SetValue(pstAlignRecipe->m_name[0]);
					break;
				case EN_RECIPE_ALIGN::LOCAL_MARK_NAME: stParam.SetValue(pstAlignRecipe->m_name[1]);
					break;
				}
			}
			break;
			}

			GetRecipe(eRecipeMode)->SetParam(nCntTab, nCntParam, stParam);
		}
	}


	return TRUE;
}

BOOL CRecipeManager::LoadRecipe(CString strJobName, CString strExpoName, CString strAlignName, EN_RECIPE_MODE eRecipeMode)
{
	if (_T("") == strJobName || _T("") == strExpoName || _T("") == strAlignName)
		return FALSE;

	CUniToChar csCnv;
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetRecipeOnlyName(strJobName.GetBuffer()); strJobName.ReleaseBuffer();
	LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(strExpoName.GetBuffer()); strExpoName.ReleaseBuffer();
	LPG_RAAF pstAlignRecipe = uvEng_Mark_GetAlignRecipeName(strAlignName.GetBuffer()); strAlignName.ReleaseBuffer();

	if (NULL == pstRecipe || NULL == pstExpoRecipe || NULL == pstAlignRecipe)
	{
		return FALSE;
	}

	SetRecipeName(strJobName, eRecipeMode);
	SetExpoRecipeName(strExpoName, eRecipeMode);
	SetAlignRecipeName(strAlignName, eRecipeMode);

	CString strValue;

	int nIndexScan = 0;
	int nMaxTab = GetRecipe(eRecipeMode)->GetTabCount();
	for (int nCntTab = 0; nCntTab < nMaxTab; nCntTab++)
	{
		ST_RECIPE_TAB stTab = GetRecipe(eRecipeMode)->GetTab(nCntTab);
		//사용하지 않는 탭은 건너 뛴다.
		if (FALSE == stTab.bUse)
			continue;

		for (int nCntParam = 0; nCntParam < stTab.GetParamCount(); nCntParam++)
		{
			ST_RECIPE_PARAM stParam = GetRecipe(eRecipeMode)->GetParam(nCntTab, nCntParam);

			//사용하지 않는 파라미터는 읽지 않는다.(LoadRecipeForm의 Value값이 Value,DefaultValue로 설정되어있는상태) 
			if (FALSE == stParam.bUse)
				continue;

			switch (nCntTab)
			{
			case EN_RECIPE_TAB::JOB:
			{
				switch (nCntParam)
				{
				case EN_RECIPE_JOB::JOB_NAME:
					stParam.SetValue(pstRecipe->job_name);
					break;
				case EN_RECIPE_JOB::GERBER_PATH:
					stParam.SetValue(pstRecipe->gerber_path);
					break;
				case EN_RECIPE_JOB::GERBER_NAME:
					stParam.SetValue(pstRecipe->gerber_name);
					break;
				case EN_RECIPE_JOB::MATERIAL_THICK:
					stParam.SetValue(pstRecipe->material_thick);
					break;

				case EN_RECIPE_JOB::LDS_THRESHOLD:
					stParam.SetValue(pstRecipe->ldsThreshold);
					break;

				/*case EN_RECIPE_JOB::LDS_BASEHEIGHT:
					stParam.SetValue(pstRecipe->ldsBaseHeight);
					break;*/

				case EN_RECIPE_JOB::EXPO_ENERGY:
					stParam.SetValue(pstRecipe->expo_energy);
					break;
				case EN_RECIPE_JOB::ALIGN_RECIPE:
					stParam.SetValue(pstRecipe->align_recipe);
					break;
				case EN_RECIPE_JOB::EXPO_RECIPE:
					stParam.SetValue(pstRecipe->expo_recipe);
					break;
				}
			}
			break;
			case EN_RECIPE_TAB::EXPOSE:
			{
				switch (nCntParam)
				{
				case EN_RECIPE_EXPOSE::EXPO_NAME:
					stParam.SetValue(pstExpoRecipe->expo_name);
					break;
				case EN_RECIPE_EXPOSE::POWER_NAME:
					stParam.SetValue(pstExpoRecipe->power_name);
					break;
				case EN_RECIPE_EXPOSE::MARK_ERR_DIST_RATE:
					stParam.SetValue(pstExpoRecipe->global_mark_dist_rate);
					break;
				case EN_RECIPE_EXPOSE::MARK_ERR_DIST_TOP_HORZ:
					stParam.SetValue(pstExpoRecipe->global_mark_dist[0]);
					break;
				case EN_RECIPE_EXPOSE::MARK_ERR_DIST_BTM_HORZ:
					stParam.SetValue(pstExpoRecipe->global_mark_dist[1]);
					break;
				case EN_RECIPE_EXPOSE::MARK_ERR_DIST_TOP_VERT:
					stParam.SetValue(pstExpoRecipe->global_mark_dist[2]);
					break;
				case EN_RECIPE_EXPOSE::MARK_ERR_DIST_BTM_VERT:
					stParam.SetValue(pstExpoRecipe->global_mark_dist[3]);
					break;
				case EN_RECIPE_EXPOSE::MARK_ERR_DIST_LFT_DIAG:
					stParam.SetValue(pstExpoRecipe->global_mark_dist[4]);
					break;
				case EN_RECIPE_EXPOSE::MARK_ERR_DIST_RGT_DIAG:
					stParam.SetValue(pstExpoRecipe->global_mark_dist[5]);
					break;
				case EN_RECIPE_EXPOSE::LED_DUTY_CYCLE:
					stParam.SetValue(pstExpoRecipe->led_duty_cycle);
					break;
				case EN_RECIPE_EXPOSE::SCORE_ACCEPT:
					stParam.SetValue(pstExpoRecipe->mark_score_accept);
					break;
				case EN_RECIPE_EXPOSE::SCALE_RANGE:
					stParam.SetValue(pstExpoRecipe->mark_scale_range);
					break;
				case EN_RECIPE_EXPOSE::SERIAL_DECODE:
					stParam.SetValue(pstExpoRecipe->dcode_serial);
					break;
				case EN_RECIPE_EXPOSE::SCALE_DECODE:
					stParam.SetValue(pstExpoRecipe->dcode_scale);
					break;
				case EN_RECIPE_EXPOSE::TEXT_DECODE:
					stParam.SetValue(pstExpoRecipe->dcode_text);
					break;
				case EN_RECIPE_EXPOSE::TEXT_STRING:
					stParam.SetValue(pstExpoRecipe->text_string);
					break;
				case EN_RECIPE_EXPOSE::SERIAL_FLIP_HORZ:
					stParam.SetValue(pstExpoRecipe->serial_flip_h);
					break;
				case EN_RECIPE_EXPOSE::SERIAL_FLIP_VERT:
					stParam.SetValue(pstExpoRecipe->serial_flip_v);
					break;
				case EN_RECIPE_EXPOSE::SCALE_FLIP_HORZ:
					stParam.SetValue(pstExpoRecipe->scale_flip_h);
					break;
				case EN_RECIPE_EXPOSE::SCALE_FLIP_VERT:
					stParam.SetValue(pstExpoRecipe->scale_flip_v);
					break;
				case EN_RECIPE_EXPOSE::TEXT_FLIP_HORZ:
					stParam.SetValue(pstExpoRecipe->text_flip_h);
					break;
				case EN_RECIPE_EXPOSE::TEXT_FLIP_VERT:
					stParam.SetValue(pstExpoRecipe->text_flip_v);
					break;


				case EN_RECIPE_EXPOSE::MATERIAL_TYPE:
					stParam.SetValue(pstExpoRecipe->headOffset);
					break;
				}
			}
			break;
			case EN_RECIPE_TAB::ALIGN:
			{

				switch (nCntParam)
				{
				case EN_RECIPE_ALIGN::ALIGN_NAME: stParam.SetValue(pstAlignRecipe->align_name);
					break;
				case EN_RECIPE_ALIGN::MARK_TYPE: stParam.SetValue(pstAlignRecipe->mark_type);
					break;
				case EN_RECIPE_ALIGN::ALIGN_TYPE: stParam.SetValue(pstAlignRecipe->align_type);
					break;
				case EN_RECIPE_ALIGN::LAMP_TYPE: stParam.SetValue(pstAlignRecipe->lamp_type);
					break;
				case EN_RECIPE_ALIGN::LAMP_AMBER_CAM1:
				case EN_RECIPE_ALIGN::LAMP_AMBER_CAM2:
				case EN_RECIPE_ALIGN::LAMP_IR_CAM1:
				case EN_RECIPE_ALIGN::LAMP_IR_CAM2:
				case EN_RECIPE_ALIGN::LAMP_COAXIAL_CAM1:
				case EN_RECIPE_ALIGN::LAMP_COAXIAL_CAM2:
					stParam.SetValue(pstAlignRecipe->lamp_value[nCntParam - EN_RECIPE_ALIGN::LAMP_AMBER_CAM1]);
					break;
				case EN_RECIPE_ALIGN::ALIGN_MOTION: stParam.SetValue(pstAlignRecipe->align_motion);
					break;

				case EN_RECIPE_ALIGN::GAIN_LEVEL_CAM1:
				case EN_RECIPE_ALIGN::GAIN_LEVEL_CAM2:
					stParam.SetValue(pstAlignRecipe->gain_level[nCntParam - EN_RECIPE_ALIGN::GAIN_LEVEL_CAM1]);
					break;
				case EN_RECIPE_ALIGN::SEARCH_TYPE: stParam.SetValue(pstAlignRecipe->search_type);
					break;
				case EN_RECIPE_ALIGN::SEARCH_COUNT: stParam.SetValue(pstAlignRecipe->search_count);
					break;
				case EN_RECIPE_ALIGN::MARK_AREA_WIDTH: stParam.SetValue(pstAlignRecipe->mark_area[0]);
					break;
				case EN_RECIPE_ALIGN::MARK_AREA_HEIGHT: stParam.SetValue(pstAlignRecipe->mark_area[1]);
					break;
				case EN_RECIPE_ALIGN::ALIGN_CAMERA_NUMBER1: stParam.SetValue(pstAlignRecipe->acam_num[0]);
					break;
				case EN_RECIPE_ALIGN::ALIGN_CAMERA_NUMBER2: stParam.SetValue(pstAlignRecipe->acam_num[1]);
					break;
				case EN_RECIPE_ALIGN::GLOBAL_MARK_NAME: 
					stParam.SetValue(pstAlignRecipe->m_name[0]);
					break;
				case EN_RECIPE_ALIGN::LOCAL_MARK_NAME: stParam.SetValue(pstAlignRecipe->m_name[1]);
					break;
				}
			}
			break;
			}

			GetRecipe(eRecipeMode)->SetParam(nCntTab, nCntParam, stParam);
		}
	}


	return TRUE;
}

BOOL CRecipeManager::LoadExpoRecipe(CString strName, EN_RECIPE_MODE eRecipeMode)
{
	if (_T("") == strName)
		return FALSE;

	SetExpoRecipeName(strName, eRecipeMode);

	CUniToChar csCnv;
	LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(strName.GetBuffer()); strName.ReleaseBuffer();

	if (NULL == pstExpoRecipe)
	{
		return FALSE;
	}

	CString strValue;

	int nIndexScan = 0;
	int nMaxTab = GetRecipe(eRecipeMode)->GetTabCount();
	for (int nCntTab = 0; nCntTab < nMaxTab; nCntTab++)
	{
		ST_RECIPE_TAB stTab = GetRecipe(eRecipeMode)->GetTab(nCntTab);
		//사용하지 않는 탭은 건너 뛴다.
		if (FALSE == stTab.bUse)
			continue;

		for (int nCntParam = 0; nCntParam < stTab.GetParamCount(); nCntParam++)
		{
			ST_RECIPE_PARAM stParam = GetRecipe(eRecipeMode)->GetParam(nCntTab, nCntParam);

			//사용하지 않는 파라미터는 읽지 않는다.(LoadRecipeForm의 Value값이 Value,DefaultValue로 설정되어있는상태) 
			if (FALSE == stParam.bUse)
				continue;

			if (EN_RECIPE_TAB::EXPOSE != nCntTab)
			{
				continue;
			}

			switch (nCntParam)
			{
			case EN_RECIPE_EXPOSE::EXPO_NAME:
				stParam.SetValue(pstExpoRecipe->expo_name);
				break;
			case EN_RECIPE_EXPOSE::POWER_NAME:
				stParam.SetValue(pstExpoRecipe->power_name);
				break;
			case EN_RECIPE_EXPOSE::MARK_ERR_DIST_RATE:
				stParam.SetValue(pstExpoRecipe->global_mark_dist_rate);
				break;
			case EN_RECIPE_EXPOSE::MARK_ERR_DIST_TOP_HORZ:
				stParam.SetValue(pstExpoRecipe->global_mark_dist[0]);
				break;
			case EN_RECIPE_EXPOSE::MARK_ERR_DIST_BTM_HORZ:
				stParam.SetValue(pstExpoRecipe->global_mark_dist[1]);
				break;
			case EN_RECIPE_EXPOSE::MARK_ERR_DIST_TOP_VERT:
				stParam.SetValue(pstExpoRecipe->global_mark_dist[2]);
				break;
			case EN_RECIPE_EXPOSE::MARK_ERR_DIST_BTM_VERT:
				stParam.SetValue(pstExpoRecipe->global_mark_dist[3]);
				break;
			case EN_RECIPE_EXPOSE::MARK_ERR_DIST_LFT_DIAG:
				stParam.SetValue(pstExpoRecipe->global_mark_dist[4]);
				break;
			case EN_RECIPE_EXPOSE::MARK_ERR_DIST_RGT_DIAG:
				stParam.SetValue(pstExpoRecipe->global_mark_dist[5]);
				break;
			case EN_RECIPE_EXPOSE::LED_DUTY_CYCLE:
				stParam.SetValue(pstExpoRecipe->led_duty_cycle);
				break;
			case EN_RECIPE_EXPOSE::SCORE_ACCEPT:
				stParam.SetValue(pstExpoRecipe->mark_score_accept);
				break;
			case EN_RECIPE_EXPOSE::SCALE_RANGE:
				stParam.SetValue(pstExpoRecipe->mark_scale_range);
				break;
			case EN_RECIPE_EXPOSE::SERIAL_DECODE:
				stParam.SetValue(pstExpoRecipe->dcode_serial);
				break;
			case EN_RECIPE_EXPOSE::SCALE_DECODE:
				stParam.SetValue(pstExpoRecipe->dcode_scale);
				break;
			case EN_RECIPE_EXPOSE::TEXT_DECODE:
				stParam.SetValue(pstExpoRecipe->dcode_text);
				break;
			case EN_RECIPE_EXPOSE::TEXT_STRING:
				stParam.SetValue(pstExpoRecipe->text_string);
				break;
			case EN_RECIPE_EXPOSE::SERIAL_FLIP_HORZ:
				stParam.SetValue(pstExpoRecipe->serial_flip_h);
				break;
			case EN_RECIPE_EXPOSE::SERIAL_FLIP_VERT:
				stParam.SetValue(pstExpoRecipe->serial_flip_v);
				break;
			case EN_RECIPE_EXPOSE::SCALE_FLIP_HORZ:
				stParam.SetValue(pstExpoRecipe->scale_flip_h);
				break;
			case EN_RECIPE_EXPOSE::SCALE_FLIP_VERT:
				stParam.SetValue(pstExpoRecipe->scale_flip_v);
				break;
			case EN_RECIPE_EXPOSE::TEXT_FLIP_HORZ:
				stParam.SetValue(pstExpoRecipe->text_flip_h);
				break;
			case EN_RECIPE_EXPOSE::TEXT_FLIP_VERT:
				stParam.SetValue(pstExpoRecipe->text_flip_v);
				break;

			case EN_RECIPE_EXPOSE::MATERIAL_TYPE:
				stParam.SetValue(pstExpoRecipe->headOffset);
				break;
			}

			GetRecipe(eRecipeMode)->SetParam(nCntTab, nCntParam, stParam);
		}
	}


	return TRUE;
}

BOOL CRecipeManager::LoadAlignRecipe(CString strName, EN_RECIPE_MODE eRecipeMode)
{
	if (_T("") == strName)
		return FALSE;

	CUniToChar csCnv;
	LPG_RAAF pstAlignRecipe = uvEng_Mark_GetAlignRecipeName(strName.GetBuffer()); strName.ReleaseBuffer();

	if (NULL == pstAlignRecipe)
	{
		return FALSE;
	}

	CString strValue;

	int nIndexScan = 0;
	int nMaxTab = GetRecipe(eRecipeMode)->GetTabCount();
	for (int nCntTab = 0; nCntTab < nMaxTab; nCntTab++)
	{
		ST_RECIPE_TAB stTab = GetRecipe(eRecipeMode)->GetTab(nCntTab);
		//사용하지 않는 탭은 건너 뛴다.
		if (FALSE == stTab.bUse)
			continue;

		for (int nCntParam = 0; nCntParam < stTab.GetParamCount(); nCntParam++)
		{
			ST_RECIPE_PARAM stParam = GetRecipe(eRecipeMode)->GetParam(nCntTab, nCntParam);

			//사용하지 않는 파라미터는 읽지 않는다.(LoadRecipeForm의 Value값이 Value,DefaultValue로 설정되어있는상태) 
			if (FALSE == stParam.bUse)
				continue;

			if (EN_RECIPE_TAB::ALIGN != nCntTab)
			{
				continue;
			}
		
			switch (nCntParam)
			{
			case EN_RECIPE_ALIGN::ALIGN_NAME: stParam.SetValue(pstAlignRecipe->align_name);
				break;
			case EN_RECIPE_ALIGN::MARK_TYPE: stParam.SetValue(pstAlignRecipe->mark_type);
				break;
			case EN_RECIPE_ALIGN::ALIGN_TYPE: stParam.SetValue(pstAlignRecipe->align_type);
				break;

			case EN_RECIPE_ALIGN::ALIGN_MOTION: stParam.SetValue(pstAlignRecipe->align_motion);
				break;

			case EN_RECIPE_ALIGN::LAMP_TYPE: stParam.SetValue(pstAlignRecipe->lamp_type);
				break;
			case EN_RECIPE_ALIGN::LAMP_AMBER_CAM1:
			case EN_RECIPE_ALIGN::LAMP_AMBER_CAM2:
			case EN_RECIPE_ALIGN::LAMP_IR_CAM1:
			case EN_RECIPE_ALIGN::LAMP_IR_CAM2:
			case EN_RECIPE_ALIGN::LAMP_COAXIAL_CAM1:
			case EN_RECIPE_ALIGN::LAMP_COAXIAL_CAM2:
				stParam.SetValue(pstAlignRecipe->lamp_value[nCntParam - EN_RECIPE_ALIGN::LAMP_AMBER_CAM1]);
				break;
			case EN_RECIPE_ALIGN::GAIN_LEVEL_CAM1:
			case EN_RECIPE_ALIGN::GAIN_LEVEL_CAM2:
				stParam.SetValue(pstAlignRecipe->gain_level[nCntParam - EN_RECIPE_ALIGN::GAIN_LEVEL_CAM1]);
				break;
			case EN_RECIPE_ALIGN::SEARCH_TYPE: stParam.SetValue(pstAlignRecipe->search_type);
				break;
			case EN_RECIPE_ALIGN::SEARCH_COUNT: stParam.SetValue(pstAlignRecipe->search_count);
				break;
			case EN_RECIPE_ALIGN::MARK_AREA_WIDTH: stParam.SetValue(pstAlignRecipe->mark_area[0]);
				break;
			case EN_RECIPE_ALIGN::MARK_AREA_HEIGHT: stParam.SetValue(pstAlignRecipe->mark_area[1]);
				break;
			case EN_RECIPE_ALIGN::ALIGN_CAMERA_NUMBER1: stParam.SetValue(pstAlignRecipe->acam_num[0]);
				break;
			case EN_RECIPE_ALIGN::ALIGN_CAMERA_NUMBER2: stParam.SetValue(pstAlignRecipe->acam_num[1]);
				break;
			case EN_RECIPE_ALIGN::GLOBAL_MARK_NAME: 
				stParam.SetValue(pstAlignRecipe->m_name[0]);
				break;
			case EN_RECIPE_ALIGN::LOCAL_MARK_NAME: stParam.SetValue(pstAlignRecipe->m_name[1]);
				break;
			}
		
			GetRecipe(eRecipeMode)->SetParam(nCntTab, nCntParam, stParam);
		}
	}


	return TRUE;
}

int CRecipeManager::GetRecipeTabIndex(CString strTabName)
{
	int nRecipeTabIndex = -1;
	for (int i = 0; i < (int)EN_RECIPE_TAB::_size(); i++)
	{
		CString strName = (CString)EN_RECIPE_TAB::_from_index(i)._to_string();
		if (0 == strName.Compare(strTabName))
		{
			return i;
		}
	}

	return nRecipeTabIndex;
}

int CRecipeManager::GetRecipeIndex(CString strName)
{
	for (int i = 0; i < (int)m_strArrRecipeList.GetCount(); i++)
	{
		if (0 == strName.Compare(m_strArrRecipeList.GetAt(i)))
		{
			return i;
		}
	}

	return -1;
}

int CRecipeManager::GetSelectRecipeIndex(EN_RECIPE_MODE eRecipeMode)
{
	for (int i = 0; i < (int)m_strArrRecipeList.GetCount(); i++)
	{
		if (0 == GetRecipeName(eRecipeMode).Compare(m_strArrRecipeList.GetAt(i)))
		{
			return i;
		}
	}

	return -1;
}

CString CRecipeManager::GetRecipeTabName(int nIndexTab)
{
	if (0 > nIndexTab || (int)EN_RECIPE_TAB::_size() <= nIndexTab)
	{
		return _T("NOT EXIST TAB");
	}
	CString strName = (CString)EN_RECIPE_TAB::_from_index(nIndexTab)._to_string();

	return strName;
}

BOOL CRecipeManager::WriteRecipeName(CString strOldRecipeName, CString strNewRecipeName)
{
	CString OldName;
	OldName.Format(_T("%s%s.CSV"), GetRecipePath(), strOldRecipeName);
	CString NewName;
	NewName.Format(_T("%s%s.CSV"), GetRecipePath(), strNewRecipeName);

	CFile::Rename(OldName, NewName);

	return TRUE;
}

BOOL CRecipeManager::CompareRecipeDifference(CStringArray& strArrTab, CStringArray& strArrParam)
{	
	strArrTab.RemoveAll();
	strArrParam.RemoveAll();

	BOOL bIsDiffer = FALSE;
	int nMaxTab		= GetRecipe(eRECIPE_MODE_SEL)->GetTabCount();
	for (int nCntTab = 0; nCntTab < nMaxTab; nCntTab++)
	{
		int nMaxParam		= GetRecipe(eRECIPE_MODE_SEL)->GetParamCount(nCntTab);
		CString strTabName	= GetRecipe(eRECIPE_MODE_SEL)->GetTabName(nCntTab);

		for (int nCntParam = 0; nCntParam < nMaxParam; nCntParam++)
		{
			CString strParamName	= GetRecipe(eRECIPE_MODE_SEL)->GetParamName(nCntTab, nCntParam);
			CString strDataType		= GetRecipe(eRECIPE_MODE_SEL)->GetParamDataType(nCntTab, nCntParam);
			
			//CString으로만 비교하려했으나,
			//자리수등이 통일되지않는다면 같다고 볼 수 있는것도 다르다고 판단할 우려가 있어 각각 만듬
			if (strDataType == DEF_DATA_TYPE_BOOL || strDataType == DEF_DATA_TYPE_INT)
			{
				int nValueSel = GetRecipe(eRECIPE_MODE_SEL)->GetInt(nCntTab, nCntParam);
				int nValueView = GetRecipe(eRECIPE_MODE_VIEW)->GetInt(nCntTab, nCntParam);

				if (nValueSel != nValueView)
				{
					bIsDiffer = TRUE;
					strArrTab.Add(strTabName);
					CString strParam;
					strParam.Format(_T("%s : [%d] -> [%d]"), strParamName, nValueSel, nValueView);
					strArrParam.Add(strParam);
				}
			}
			else if (strDataType == DEF_DATA_TYPE_DOUBLE)
			{
				double dValueSel = GetRecipe(eRECIPE_MODE_SEL)->GetDouble(nCntTab, nCntParam);
				double dValueView = GetRecipe(eRECIPE_MODE_VIEW)->GetDouble(nCntTab, nCntParam);

				if (dValueSel != dValueView)
				{
					bIsDiffer = TRUE;
					strArrTab.Add(strTabName);
					CString strParam;
					strParam.Format(_T("%s : [%.3f] -> [%.3f]"), strParamName, dValueSel, dValueView);
					strArrParam.Add(strParam);
				}
			}
			else
			{
				CString strValueSel = GetRecipe(eRECIPE_MODE_SEL)->GetValue(nCntTab, nCntParam);
				CString strValueView = GetRecipe(eRECIPE_MODE_VIEW)->GetValue(nCntTab, nCntParam);

				if (0 != strValueSel.Compare(strValueView))
				{
					bIsDiffer = TRUE;
					strArrTab.Add(strTabName);
					CString strParam;
					strParam.Format(_T("%s : [%s] -> [%s]"), strParamName, strValueSel, strValueView);
					strArrParam.Add(strParam);
				}
			}			
		}
	}

	return bIsDiffer;
}

int CRecipeManager::GetRecipeList(CStringArray &strArrRecipeList)
{
	LoadRecipeList();
	
	for (int i = 0; i < m_strArrRecipeList.GetCount(); i++)
	{
		strArrRecipeList.Add(m_strArrRecipeList.GetAt(i));
	}

	return (int)strArrRecipeList.GetCount();
}

BOOL CRecipeManager::GetLEDFrameRate()
{
	/*CString strRecipeName = CRecipeManager::GetInstance()->GetRecipeName();

	bool isLocalRecipe =  uvEng_JobRecipe_WhatLastSelectIsLocal();

	BOOL bSuccess = uvEng_JobRecipe_SelRecipeOnlyName(strRecipeName.GetBuffer(), isLocalRecipe);
	
	strRecipeName.ReleaseBuffer();

	if (FALSE == bSuccess)
	{
		return FALSE;
	}*/
	//아니 뭐 다고쳐야되네 진짜. 

	bool isLocalRecipe = uvEng_JobRecipe_WhatLastSelectIsLocal();

	CHAR szJob[MAX_PATH_LEN] = { NULL };
	CUniToChar	csCnv;
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe(isLocalRecipe);// strRecipeName.GetBuffer());	strRecipeName.ReleaseBuffer();

	if (NULL == pstRecipe)
	{
		return FALSE;
	}

	LPG_RAAF pstAlignRecipe = uvEng_Mark_GetAlignRecipeName(csCnv.Ansi2Uni(pstRecipe->align_recipe));
	LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstRecipe->expo_recipe));

	/*각 레시피 조건에 따른 노광 속도 계산*/
	LPG_PLPI	pstPowerI = uvEng_LedPower_GetLedPowerName(csCnv.Ansi2Uni(pstExpoRecipe->power_name));

	if (NULL == pstPowerI)
	{
		AfxMessageBox(L"The LED Power file registered in the recipe does not exist", 0x01);
		return FALSE;
	}
	UINT8 i = 0, j = 0;
	DOUBLE dbTotal = 0.0f, dbPowerWatt[MAX_LED] = { NULL }, dbSpeed = 0.0f;
	/* 광량 계산 */
	for (; i < uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		for (j = 0; j < MAX_LED; j++)	dbPowerWatt[j] = pstPowerI->led_watt[i][j];
		dbTotal += uvCmn_Luria_GetEnergyToSpeed(pstRecipe->step_size, pstRecipe->expo_energy,
			pstExpoRecipe->led_duty_cycle, dbPowerWatt);

	}
	pstRecipe->frame_rate = (UINT16)(dbTotal / DOUBLE(i));


	return TRUE;
}



BOOL CRecipeManager::CalcMarkDist()
{
	CUniToChar	csCnv;
	UINT32 u32Dist[6] = { NULL };	/* um */
	DOUBLE dbRate = 0.0003 /* Medium (Middle) */, dbDist = 0.0f /* mm */;
	CAtlList <DOUBLE> lstX, lstY;
	TCHAR tzGerb[MAX_PATH_LEN] = { NULL };

	CString strRecipeName = CRecipeManager::GetInstance()->GetRecipeName();


	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetRecipeOnlyName(strRecipeName.GetBuffer());	strRecipeName.ReleaseBuffer();
	if (NULL == pstRecipe)
	{
		return FALSE;
	}


	LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstRecipe->expo_recipe));
	//* 현재 선택된 거버 파일 (전체 경로) 얻기 */
	swprintf_s(tzGerb, MAX_PATH_LEN, L"%S\\%S", pstRecipe->gerber_path, pstRecipe->gerber_name);

	/*파일 경로에 Gerber 데이터가 없을시 종료*/
	if (!uvCmn_FindFile(tzGerb))
	{
		return FALSE;
	}

	LPG_RAAF pstAlignRecipe = uvEng_Mark_GetAlignRecipeName(csCnv.Ansi2Uni(pstRecipe->align_recipe));

	/* 거버에 대한 마크 정보 얻기 */
	if (0x00 != uvEng_Luria_GetGlobalMarkJobName(tzGerb, lstX, lstY,
		(ENG_ATGL)pstAlignRecipe->align_type))
	{
		//dlgMesg.MyDoModal(L"Failed to get the mark info. of gerber file", 0x01);
	}
	else
	{
		//dbRate = 0.0001f;
		dbRate = pstExpoRecipe->global_mark_dist_rate;

		/* 만약 현재 등록된 마크가 없다면 */
		if (lstX.GetCount() >= 4 && lstY.GetCount() >= 4)
		{
			/* 1번 / 3번 마크 간의 수평 (거리) 길이 값에 오차 값 적용 */
			dbDist = abs(lstX.GetAt(lstX.FindIndex(0)) - lstX.GetAt(lstX.FindIndex(2)));
			u32Dist[0] = (UINT32)ROUNDDN(dbDist * 1000.0f * dbRate, 0);
			/* 2번 / 4번 마크 간의 수평 (거리) 길이 값에 오차 값 적용 */
			dbDist = abs(lstX.GetAt(lstX.FindIndex(1)) - lstX.GetAt(lstX.FindIndex(3)));
			u32Dist[1] = (UINT32)ROUNDDN(dbDist * 1000.0f * dbRate, 0);
			/* 1번 / 2번 마크 간의 수직 (거리) 길이 값에 오차 값 적용 */
			dbDist = abs(lstY.GetAt(lstY.FindIndex(0)) - lstY.GetAt(lstY.FindIndex(1)));
			u32Dist[2] = (UINT32)ROUNDDN(dbDist * 1000.0f * dbRate, 0);
			/* 3번 / 4번 마크 간의 수직 (거리) 길이 값에 오차 값 적용 */
			dbDist = abs(lstY.GetAt(lstY.FindIndex(2)) - lstY.GetAt(lstY.FindIndex(3)));
			u32Dist[3] = (UINT32)ROUNDDN(dbDist * 1000.0f * dbRate, 0);
			/* 1번 / 4번 마크 간의 대각 (거리) 길이 값에 오차 값 적용 */
			dbDist = sqrt(pow(lstY.GetAt(lstY.FindIndex(0)) - lstY.GetAt(lstY.FindIndex(1)), 2) +
				pow(lstX.GetAt(lstX.FindIndex(0)) - lstX.GetAt(lstX.FindIndex(3)), 2));
			u32Dist[4] = (UINT32)ROUNDDN(dbDist * 1000.0f * dbRate, 0);
			/* 2번 / 3번 마크 간의 대각 (거리) 길이 값에 오차 값 적용 */
			dbDist = sqrt(pow(lstY.GetAt(lstY.FindIndex(2)) - lstY.GetAt(lstY.FindIndex(3)), 2) +
				pow(lstX.GetAt(lstX.FindIndex(1)) - lstX.GetAt(lstX.FindIndex(2)), 2));
			u32Dist[5] = (UINT32)ROUNDDN(dbDist * 1000.0f * dbRate, 0);
		}
	}
	return TRUE;
}

VOID CRecipeManager::PhilSendCreateRecipe(LPG_RJAF stRecipe)
{
	STG_PP_P2C_RCP_CREATE			stCreate;
	STG_PP_P2C_RCP_CREATE_ACK		stCreateAck;
	stCreate.Reset();
	stCreateAck.Reset();

	memcpy(stCreate.szRecipeName, stRecipe->job_name, DEF_MAX_RECIPE_NAME_LENGTH);

	/*노광 결과 파라미터값*/
	stCreate.usCount = 7;
	sprintf_s(stCreate.stVar[0].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stCreate.stVar[0].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Job Name");
	sprintf_s(stCreate.stVar[0].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", stRecipe->job_name);

	sprintf_s(stCreate.stVar[1].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stCreate.stVar[1].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Gerber Path");
	sprintf_s(stCreate.stVar[1].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", stRecipe->gerber_path);

	sprintf_s(stCreate.stVar[2].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stCreate.stVar[2].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Gerber Name");
	sprintf_s(stCreate.stVar[2].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", stRecipe->gerber_name);

	sprintf_s(stCreate.stVar[3].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stCreate.stVar[3].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Align Recipe");
	sprintf_s(stCreate.stVar[3].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", stRecipe->align_recipe);

	sprintf_s(stCreate.stVar[4].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stCreate.stVar[4].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Expo Recipe");
	sprintf_s(stCreate.stVar[4].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", stRecipe->expo_recipe);

	sprintf_s(stCreate.stVar[5].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "INT");
	sprintf_s(stCreate.stVar[5].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Material Thick");
	sprintf_s(stCreate.stVar[5].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%d", stRecipe->material_thick);

	sprintf_s(stCreate.stVar[6].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stCreate.stVar[6].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Expo Energy");
	sprintf_s(stCreate.stVar[6].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%.1f", stRecipe->expo_energy);

	uvEng_Philhmi_Send_P2C_RCP_CREATE(stCreate, stCreateAck);
}

VOID CRecipeManager::PhilSendModifyRecipe(LPG_RJAF stRecipe)
{
	STG_PP_P2C_RCP_MODIFY			stModify;
	STG_PP_P2C_RCP_MODIFY_ACK		stModifyAck;

	stModify.Reset();
	stModifyAck.Reset();

	memcpy(stModify.szRecipeName, stRecipe->job_name, DEF_MAX_RECIPE_NAME_LENGTH);

	/*노광 결과 파라미터값*/
	stModify.usCount = 7;
	sprintf_s(stModify.stVar[0].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stModify.stVar[0].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Job Name");
	sprintf_s(stModify.stVar[0].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", stRecipe->job_name);

	sprintf_s(stModify.stVar[1].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stModify.stVar[1].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Gerber Path");
	sprintf_s(stModify.stVar[1].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", stRecipe->gerber_path);

	sprintf_s(stModify.stVar[2].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stModify.stVar[2].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Gerber Name");
	sprintf_s(stModify.stVar[2].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", stRecipe->gerber_name);

	sprintf_s(stModify.stVar[3].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stModify.stVar[3].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Align Recipe");
	sprintf_s(stModify.stVar[3].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", stRecipe->align_recipe);

	sprintf_s(stModify.stVar[4].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stModify.stVar[4].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Expo Recipe");
	sprintf_s(stModify.stVar[4].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", stRecipe->expo_recipe);

	sprintf_s(stModify.stVar[5].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "INT");
	sprintf_s(stModify.stVar[5].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Material Thick");
	sprintf_s(stModify.stVar[5].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%d", stRecipe->material_thick);

	sprintf_s(stModify.stVar[6].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stModify.stVar[6].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Expo Energy");
	sprintf_s(stModify.stVar[6].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%.1f", stRecipe->expo_energy);

	uvEng_Philhmi_Send_P2C_RCP_MODIFY(stModify, stModifyAck);
}


VOID CRecipeManager::PhilSendDeleteRecipe(CString strRecipeName)
{
	STG_PP_P2C_RCP_DELETE			stDelete;
	STG_PP_P2C_RCP_DELETE_ACK		stDeleteAck;
	stDelete.Reset();
	stDeleteAck.Reset();
	CUniToChar	csCnv;

	strcpy_s(stDelete.szRecipeName, DEF_MAX_RECIPE_NAME_LENGTH, csCnv.Uni2Ansi(strRecipeName.GetBuffer()));

	uvEng_Philhmi_Send_P2C_RCP_DELETE(stDelete, stDeleteAck);
}

VOID CRecipeManager::PhilSendSelectRecipe(CString strRecipeName)
{
	STG_PP_P2C_RCP_SELECT			stSelect;
	STG_PP_P2C_RCP_SELECT_ACK		stSelectAck;
	stSelect.Reset();
	stSelectAck.Reset();
	CUniToChar	csCnv;

	strcpy_s(stSelect.szRecipeName, DEF_MAX_RECIPE_NAME_LENGTH, csCnv.Uni2Ansi(strRecipeName.GetBuffer()));

	uvEng_Philhmi_Send_P2C_RCP_SELECT(stSelect, stSelectAck);
}

//설명: 날짜별 AlignExpo.csv 파싱 하여 use_recipe 파일 생성
VOID CRecipeManager::BuidRecipUsageList()
{
	//마지막 작업 날짜 확인
	//GetLastArchiveDay();

	m_mapGerberLastUsed.clear();
	m_strRecipeListPath.Format(L"%s\\%s\\recipe\\use_recipe.dat", g_tzWorkDir,
		CUSTOM_DATA_CONFIG);

	CFileFind finder;
	CTime clastTime;

	CString strSearcPath;
	strSearcPath.Format(L"%s\\logs\\expo\\*.*", g_tzWorkDir);
	BOOL bWorking = finder.FindFile(strSearcPath);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (finder.IsDots() || finder.IsDirectory()) continue;

		CString strFileName = finder.GetFileName();
		CString strFilePath = finder.GetFilePath();

		// 와일드카드 매칭 우회 필터링
		if (strFileName.Find(L"AlignExpo.csv") == -1)
			continue;

		if (strFileName.GetLength() < 10) continue;

		//파일명에서 YYYYY-MM-DD 추출
		int nYear = _ttoi(strFileName.Mid(0, 4));
		int nMonth = _ttoi((strFileName.Mid(5, 2)));
		int nDay = _ttoi((strFileName.Mid(8, 2)));

		//현재 파일 날짜 
		CTime cLogDay(nYear, nMonth, nDay, 0, 0, 0);

		//현재 날짜 보다 최종 기록 날짜가 작으면 기록 진행
		//if (cLogDay > m_tLastTime)
		//{
			CStdioFile file;
			CFileException ex;
			if (file.Open(finder.GetFilePath(), CFile::modeRead | CFile::typeText | CFile::shareDenyNone, &ex))
			{
				CString strLine;

				ULONGLONG nFileLength = file.GetLength();
				if (nFileLength > 0)
				{
					char* pBuffer = new char[(size_t)nFileLength + 1];
					memset(pBuffer, 0, (size_t)nFileLength + 1);

					// 2. 파일 전체 내용을 바이너리 형태로 한 번에 읽어옵니다.
					file.Read(pBuffer, (UINT)nFileLength);

					// ★ [수정] 원본 파일이 UTF-16 유니코드이므로 변환 없이 직접 맵핑합니다.
					// 유니코드 파일은 시작 2바이트가 BOM(0xFFFE 또는 0xFEFF) 마크일 수 있으므로 이를 체크합니다.
					int nStartOffset = 0;
					if (nFileLength >= 2 && ((unsigned char)pBuffer[0] == 0xFF && (unsigned char)pBuffer[1] == 0xFE))
					{
						nStartOffset = 2; // BOM 마크 2바이트 스킵
					}

					// 바이트 버퍼(char*)를 유니코드 wchar_t* 포인터로 안전하게 지정하여 CString을 생성합니다.
					wchar_t* pUnicodeData = (wchar_t*)(pBuffer + nStartOffset);

					// 남은 바이트 크기를 유니코드 문자 개수로 계산합니다.
					int nLengthInChars = ((int)nFileLength - nStartOffset) / sizeof(wchar_t);

					// 크기를 명시하여 CString 객체 생성 (문자열 중간에 NULL이나 공백이 있어도 안전함)
					CString strFullContent(pUnicodeData, nLengthInChars);

					// 사용이 끝난 임시 바이트 버퍼 해제
					delete[] pBuffer;

					int nCurPos = 0;
					CString strLine = strFullContent.Tokenize(_T("\n"), nCurPos);

					if (!strLine.IsEmpty())
					{
						strLine = strFullContent.Tokenize(_T("\n"), nCurPos);
					}

					while (!strLine.IsEmpty())
					{
						strLine.Trim();
						if (strLine.IsEmpty())
						{
							strLine = strFullContent.Tokenize(_T("\n"), nCurPos);
							continue;
						}
						CString strTimePart, strGerberName;
						AfxExtractSubString(strTimePart, strLine, 0, ',');   // 0번째: 시간 (time)
						AfxExtractSubString(strGerberName, strLine, 3, ','); // 3번째: 거버명 (gerber_name)

						strGerberName.Trim();
						strTimePart.Trim();

						if (strGerberName.IsEmpty() || strTimePart.GetLength() < 8)
						{
							strLine = strFullContent.Tokenize(_T("\n"), nCurPos);
							continue;
						}

						// 파일명에서 YYYY-MM-DD 추출
						int nHour = _ttoi(strTimePart.Mid(0, 2));
						int nMin = _ttoi(strTimePart.Mid(3, 2));
						int nSec = _ttoi(strTimePart.Mid(6, 2));

						CTime cLogTime(nYear, nMonth, nDay, nHour, nMin, nSec);

						// 맵 데이터 최신값 누적
						if (m_mapGerberLastUsed.find(strGerberName) != m_mapGerberLastUsed.end())
						{
							if (m_mapGerberLastUsed[strGerberName] < cLogTime)
							{
								m_mapGerberLastUsed[strGerberName] = cLogTime;
								//최종 시간 기록
								clastTime = cLogTime;
							}

						}
						else
						{
							m_mapGerberLastUsed[strGerberName] = cLogTime;
							//최종 시간 기록
							clastTime = cLogTime;
						}

						// 다음 줄 가져오기
						strLine = strFullContent.Tokenize(_T("\n"), nCurPos);
					}
				}
				file.Close();
			}
			else
			{
				TRACE(L"[File Skip] Open Failed: %s\n", strFileName);
				continue;
			}
		//}
	}
	finder.Close();


	//use_recipe.dat 저장
	CStdioFile outputFile;
	CString strLastArchiveDay;
	if (outputFile.Open(m_strRecipeListPath, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
	{
		
		outputFile.WriteString(_T("LastUsedDataTime, Path, GerberName\n"));
		//최종 사용 시간 기록
		strLastArchiveDay.Format(_T("%s\n"), clastTime.Format(_T("%Y-%m-%d %H:%M:%S")));
		outputFile.WriteString(strLastArchiveDay);

		for (auto const& [strName, cTime] : m_mapGerberLastUsed)
		{
			CString strOutLine;
			//JobRecipe에서 동일 거버 이름 검색 후 경로 확인
			GetGerberPath(strName);
			strOutLine.Format(_T("%s,%s,%s\n"),
				cTime.Format(_T("%Y-%m-%d %H:%M:%S")), m_strSameRecipePath, strName);
			outputFile.WriteString(strOutLine);
		}
		outputFile.Close();
	}
}

VOID CRecipeManager::GetGerberPath(CString strGerberName)
{
	CString strJobRecipe, strLine;

	/* Job Recipe Name 정보 얻기 */
	strJobRecipe.Format(L"%s\\%s\\recipe\\job_recipe.dat", g_tzWorkDir,
		CUSTOM_DATA_CONFIG);

	CFileFind finder;
	CStdioFile file;

	BOOL bWorking = finder.FindFile(strJobRecipe);

	if (bWorking)
	{
		if (file.Open(strJobRecipe, CFile::modeRead | CFile::typeText))
		{

			file.ReadString(strLine); 

			while (!strLine.IsEmpty())
			{
				CString strJobGerberName, strJobGerberPath;
				AfxExtractSubString(strJobGerberPath, strLine, 1, ',');   // 1번째: 경로 (Path)
				AfxExtractSubString(strJobGerberName, strLine, 2, ',');   // 2번째: 경로 (Name)
				
				//JobRecpe 이름과 동일한 거버 이름 확인
				if (strGerberName == strJobGerberName)
				{
					m_strSameRecipePath = strJobGerberPath;
					break;
				}
				//동일 거버 이름이 아니면 다음줄 이동
				else
				{
					file.ReadString(strLine);
				}
			}
		}
	}
	finder.Close();
}

VOID CRecipeManager::GetLastArchiveDay()
{
	//레시피 이력 관리 파일 
	m_strRecipeListPath.Format(L"%s\\%s\\recipe\\use_recipe.dat", g_tzWorkDir,
		CUSTOM_DATA_CONFIG);

	CString strLastArchiveDay;
	CString strLine;
	CFileFind finder;
	CStdioFile file;

	BOOL bWorking = finder.FindFile(m_strRecipeListPath);

	if (bWorking)
	{
		if (file.Open(m_strRecipeListPath, CFile::modeRead | CFile::typeText))
		{
			file.ReadString(strLine); // 헤더행(GerberName, LastUsedDataTime) 스킵
			file.ReadString(strLastArchiveDay); // 두번째 헤더행 최종 날짜
		}

		if (!strLastArchiveDay.IsEmpty())
		{
			//최종 날짜와 시간 계산해서 tLastTime에 저장
			CString strDay, strTime;
			strDay = strLastArchiveDay.Mid(0, 10);
			strTime = strLastArchiveDay.Mid(11, 20);

			//YYYY-MM-DD 날짜 추출
			int nYear = _ttoi(strDay.Mid(0, 4));
			int nMonth = _ttoi((strDay.Mid(5, 2)));
			int nDay = _ttoi((strDay.Mid(8, 2)));
			//HH:MM:SS 시간 추출
			int nHour = _ttoi(strTime.Mid(0, 2));
			int nMin = _ttoi(strTime.Mid(3, 2));
			int nSec = _ttoi(strTime.Mid(6, 2));

			CTime tLastTime(nYear, nMonth, nDay, nHour, nMin, nSec);
			m_tLastTime = tLastTime;
		}
	}

	finder.Close();
}




//설명: 조건 검색 후 압축 + 기존 폴더 파일 삭제 처리
VOID CRecipeManager::ArchiveOldRecipe() 
{
	//레시피 이력 관리 파일 
	m_strRecipeListPath.Format(L"%s\\%s\\recipe\\use_recipe.dat", g_tzWorkDir,
		CUSTOM_DATA_CONFIG);

	CStdioFile file;
	if (!file.Open(m_strRecipeListPath, CFile::modeRead | CFile::typeText)) return;

	CreateDirectory(m_strBackupFolder, NULL);

	CTime cCurrenTime = CTime::GetCurrentTime();
	CTimeSpan cLimitSpan(uvEng_GetConfig()->recipe_management.u16ArchiveLimitDays, 0, 0, 0);
	CTime cDeadlineTime = cCurrenTime - cLimitSpan;

	CString strLine;
	file.ReadString(strLine);
	TCHAR tzMesg[128] = { NULL };

	while (file.ReadString(strLine))
	{
		if (strLine.IsEmpty()) continue;

		CString strGerberName, strDataTime, strFilePath;
		AfxExtractSubString(strDataTime, strLine, 0, ',');
		AfxExtractSubString(strFilePath, strLine, 1, ',');
		AfxExtractSubString(strGerberName, strLine, 2, ',');
		strGerberName.Trim();
		strDataTime.Trim();

		int nYear = _ttoi(strDataTime.Mid(0, 4));
		int nMonth = _ttoi(strDataTime.Mid(5, 2));
		int nDay = _ttoi(strDataTime.Mid(8, 2));
		CTime cLastUsedTime(nYear, nMonth, nDay, 0, 0, 0);

		//고려사항 반영: 설정 기간을 넘긴 경우 진행
		if (cLastUsedTime < cDeadlineTime)
		{
			CString strTargetGerberPath = strFilePath + _T("\\") + strGerberName;
			CString strZipPath = strFilePath + _T("\\") + strGerberName + _T(".zip");

			//파일 혹은 폴더가 존재하는지 확인
			if (GetFileAttributes(strZipPath) != INVALID_FILE_ATTRIBUTES)
			{
				continue; // 이미 압축된 파일이므로 무한 재압축 대상에서 제외
			}

			CString strCmd;
			strCmd.Format(L"tar -C \"%s\" -a -cf \"%s\" \"%s\"", strFilePath, strZipPath, strGerberName);

			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;
			si.dwFlags = STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_HIDE; //검은참 숨김

			if (CreateProcess(NULL, strCmd.GetBuffer(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
			{
				WaitForSingleObject(pi.hProcess, INFINITE);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				strCmd.ReleaseBuffer();

				//파일 혹은 폴더가 존재하는지 확인
				if (GetFileAttributes(strTargetGerberPath) != INVALID_FILE_ATTRIBUTES)
				{
					swprintf_s(tzMesg, 128, L"[%s] File Compression Complete", strGerberName);
					LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMesg);

					//압축이 정상 완료 했다면 기존 폴더 삭제
					DelectDirectoryOrFile(strTargetGerberPath);
				}
			}
		}

	}
	file.Close();
}

//설명: 파일 또는 폴더 삭제 함수
VOID CRecipeManager::DelectDirectoryOrFile(CString strPath)
{

	if (strPath.IsEmpty()) return;
	//1. tar 프로세스가 파일 핸들을 완전히 놓을 수 있도록 일시 대기
	Sleep(150);

	CFileFind finder;
	//하위 모든 파일 탐색 경로 지정
	CString strSearchPAth = strPath + _T("\\*.*");
	BOOL bWorking = finder.FindFile(strSearchPAth);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots())
			continue;

		if (finder.IsDirectory())
		{
			DelectDirectoryOrFile(finder.GetFilePath());
		}
		else
		{
			//파일인 경우 읽기전용 속성이 걸려있어도 지울 수 있도록 속성 초기화 후 삭제
			::SetFileAttributes(finder.GetFilePath(), FILE_ATTRIBUTE_NORMAL);
			::DeleteFile(finder.GetFilePath());
		}
	}
	finder.Close();

	::SetFileAttributes(strPath, FILE_ATTRIBUTE_NORMAL);
	if (!::RemoveDirectory(strPath))
	{
		CString strCmd;
		strCmd.Format(L"cmd.exe /c rmdir /s /q \"%s\"", strPath);

		STARTUPINFO si = { sizeof(si) };
		PROCESS_INFORMATION pi;
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;

		if (CreateProcess(NULL, strCmd.GetBuffer(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
		{
			WaitForSingleObject(pi.hProcess, 1000);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
		strCmd.ReleaseBuffer();
	}
}

//설명: 해당 파일 압축 해제
BOOL CRecipeManager::LoadAndUnzipRecipe(CString strGerberPath, CString strGerberName)
{
	CString strUnZipGerberPath = strGerberPath + _T("\\") + strGerberName;
	CString strZipGerberPath = strGerberPath + _T("\\") + strGerberName + _T(".zip");
	TCHAR tzMesg[128] = { NULL };

	//백업 폴더에 해당 압축 파일이 있는지 확인
	if (GetFileAttributes(strZipGerberPath) == INVALID_FILE_ATTRIBUTES)
	{
		swprintf_s(tzMesg, 128, L"[%s] Can not Open file", strZipGerberPath);
		LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMesg);

		AfxMessageBox(L"Can not Open file", MB_OK);
		return FALSE;
	}

	CString strCmd;
	strCmd.Format(L"tar -xf \"%s\" -C \"%s\"", strZipGerberPath, strGerberPath);

	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	BOOL bUnzipSucess = FALSE;

	if (CreateProcess(NULL, strCmd.GetBuffer(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		strCmd.ReleaseBuffer();
	}
	else
	{
		strCmd.ReleaseBuffer();
	}

	//정상적으로 폴더/파일 복원되었는지 검증 단계
	if (GetFileAttributes(strUnZipGerberPath) != INVALID_FILE_ATTRIBUTES)
	{
		::DeleteFile(strZipGerberPath);

		swprintf_s(tzMesg, 128, L"[%s] Reicpe UnZip Complete", strZipGerberPath);
		LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMesg);
		
		AfxMessageBox(L"Reicpe UnZip Complete", MB_OK);
		return TRUE;
	}
	else
	{
		swprintf_s(tzMesg, 128, L"[%s] Reicpe UnZip False", strZipGerberPath);
		LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMesg);

		AfxMessageBox(L"Reicpe UnZip False", MB_OK);
		return FALSE;
	}
}


BOOL CRecipeManager::StartBackgroundSync()
{
	if (m_pSyncThread != NULL)
		return FALSE;

	m_bStopThread = FALSE;

	m_pSyncThread = AfxBeginThread(CRecipeManager::ReicpeSyncTreadProc, this, THREAD_PRIORITY_BELOW_NORMAL);

	return (m_pSyncThread != NULL);
}
VOID CRecipeManager::StopBackgroundSync()
{
	if (m_pSyncThread == NULL) return;
	m_bStopThread = TRUE;

	DWORD dwWait = ::WaitForSingleObject(m_pSyncThread->m_hThread, 3000);

	m_pSyncThread = NULL;
}

UINT __cdecl CRecipeManager::ReicpeSyncTreadProc(LPVOID pParam)
{
	CRecipeManager* pManager = reinterpret_cast<CRecipeManager*>(pParam);
	if (pManager == NULL) return 0;

	TCHAR tzMesg[128] = { NULL };

	while (!pManager->m_bStopThread)
	{
		/*1. 백그라운드에서 안전하게 로그 분석 및 매핑 테이블 빌드*/
		//pManager->BuidRecipUsageList();
		/*2. 분석 결과를 바탕으로 기간이 지난 올드레시피 압축 및 소거 진행*/
		//pManager->ArchiveOldRecipe();


		/*현재 시스템의 날짜와 시간 정보 획득*/
		CTime cCurrentTime = CTime::GetCurrentTime();
		int nHour		= cCurrentTime.GetHour();			//현재 시간(0~23)
		int nMin		= cCurrentTime.GetMinute();			//현재 분(0~60)
		int nDayOfWeek	= cCurrentTime.GetDayOfWeek();		//현재 요일(1:일요일, 2:월요일...7:토요일)

		/*스레드 동작 날짜 및 시간 설정*/
		BOOL bIsUseHoure	= (nHour >= 2 && nHour < 4);	//2시~4시
		//BOOL bIsUseDay	= (nDayOfWeek == 1);			//일요일
		//BOOL bIsUseMin	= (nMin > 53);

		/*설정한 날짜와 시간이 맞다면*/
		if(bIsUseHoure)
		{
			swprintf_s(tzMesg, 128, L"[Background Sync] Entering non-operational/night time - Staring recipe compression(Current: %d o'clock)", nHour);
			LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMesg);

			pManager->ArchiveOldRecipe();
		}
		else
		{
			swprintf_s(tzMesg, 128, L"[Background Sync] Skippiing recipe cleanup due to Daytime Mass Production Time (Current: %d o'clock)", nHour);
			LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMesg);
		}

		/*3. 주기적 실행을 위한 대기 루프 3600초==1시간*/
		for (int i = 0;i < 3600;++i)
		{
			if (pManager->m_bStopThread)
				break;
			/*1초씩 쪼개서 정지 플래그를 체크*/
			::Sleep(1000);
		}

	}
	return 0;
}