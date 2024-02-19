#include "pch.h"
#include "RecipeManager.h"

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
}


CRecipeManager::~CRecipeManager()
{

}

CRecipe* CRecipeManager::GetRecipe(EN_RECIPE_MODE eRecipeMode)
{
	return &m_clsRcp[eRecipeMode];
}

void CRecipeManager::Init(HWND hWnd)
{
	m_hMainWnd = hWnd;
	//레시피폼 생성
	LoadRecipeForm(eRECIPE_MODE_SEL);
	LoadRecipeForm(eRECIPE_MODE_VIEW);
	
	LoadRecipeList();

	SelectRecipe(uvEng_GetRecipeConfig());
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
	
	/*Philhmi에 보고*/
	PhilSendCreateRecipe(stRecipe);

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
	stRecipe.Init();

	strRecipeName.Format(_T("%s"), csCnv.Ansi2Uni(stRecipe.job_name));
	
	//UpdateRecipe(stRecipe, eRECIPE_MODE_VIEW);
	BOOL bSuccess = uvEng_JobRecipe_RecipeAppend(&stRecipe);
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

	/*Philhmi에 보고*/
	PhilSendDeleteRecipe(strRecipeName);
	
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

BOOL CRecipeManager::SelectRecipe(CString strRecipeName)
{
	BOOL bSuccess = uvEng_JobRecipe_SelRecipeOnlyName(strRecipeName.GetBuffer()); strRecipeName.ReleaseBuffer();

	if (FALSE == bSuccess)
	{
		return FALSE;
	}

	CHAR szJob[MAX_PATH_LEN] = { NULL };
	CUniToChar	csCnv;
	LPG_RJAF pstRecipe		= uvEng_JobRecipe_GetRecipeOnlyName(strRecipeName.GetBuffer());	strRecipeName.ReleaseBuffer();

	if (NULL == pstRecipe)
	{
		return FALSE;
	}

	LPG_RAAF pstAlignRecipe = uvEng_Mark_GetAlignRecipeName(csCnv.Ansi2Uni(pstRecipe->align_recipe));
	LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstRecipe->expo_recipe));

	if (NULL == pstAlignRecipe || NULL == pstExpoRecipe)
	{
		return FALSE;
	}

	/* 거버 파일의 존재 여부 (거파 파일 검색) */
	sprintf_s(szJob, MAX_PATH_LEN, "%s\\%s", pstRecipe->gerber_path, pstRecipe->gerber_name);
	if (!uvCmn_FindFile(csCnv.Ansi2Uni(szJob)))
	{
		AfxMessageBox(L"The gerber file registered in the recipe does not exist", 0x01);
		return FALSE;
	}

#if 1
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
#endif

	/* 등록된 마크 정보가 있는지 여부 */
	if (pstAlignRecipe->align_type != UINT8(ENG_ATGL::en_global_0_local_0x0_n_point))
	{
		/* 얼라인 카메라의 2D 보정 파일 적재 (검색) */
		// by sysandj : 변수없음(수정)
		if (!uvEng_ACamCali_LoadFile(pstRecipe->cali_thick))
		{
			if (IDNO == AfxMessageBox(L"Failed to load the calibration data for align camera\n Do you want to continue", MB_YESNO))
			{
				return FALSE;
			}
		}
	}

	/* Align Camera의 Gain Level 값 설정 */
	if (!uvEng_Camera_SetGainLevel(0x01, pstAlignRecipe->gain_level[0]))
	{ 
		//AfxMessageBox(L"The gerber file registered in the recipe does not exist");
		//AfxMessageBox(L"Failed to set Gain Level value of Align Camera1");
		//return FALSE;
	}
	if (!uvEng_Camera_SetGainLevel(0x02, pstAlignRecipe->gain_level[1]))
	{
		//AfxMessageBox(L"The gerber file registered in the recipe does not exist");
		//AfxMessageBox(L"Failed to set Gain Level value of Align Camera2");
		//return FALSE;
	}


	/* 기존 적재된 Recipe 관련 거버 정보 초기화 */
	uvEng_MarkSelAlignRecipeReset();
	uvCmn_Luria_ResetRegisteredJob();


	/* Multi-Mark인 경우만 해당됨, 마크 검색 Area 크기 값 설정 */
	if (pstAlignRecipe->search_count == 1)	uvEng_Camera_SetMultiMarkArea();
	else									uvEng_Camera_SetMultiMarkArea(pstAlignRecipe->mark_area[0], pstAlignRecipe->mark_area[1]);


	if (NULL == pstExpoRecipe)
	{
		return FALSE;
	}

	/* 최정 검색된 마크의 검색 조건 값 설정 */
	uvEng_Camera_SetRecipeMarkRate(pstExpoRecipe->mark_score_accept, pstExpoRecipe->mark_scale_range);


	uvEng_SetRecipeConfig(strRecipeName.GetBuffer()); strRecipeName.ReleaseBuffer();
	LoadRecipe(strRecipeName, eRECIPE_MODE_SEL);
			
	bSuccess = uvEng_Mark_SelAlignRecipeName(csCnv.Ansi2Uni(pstRecipe->align_recipe)); // lk91 Mark Recipe Select 추가
	if (FALSE == bSuccess)
	{
		return FALSE;
	}

	for (int index = 0; index < 2; index++)
	{
		UINT8 u8Speed = (UINT8)uvEng_GetConfig()->mark_find.model_speed;
		UINT8 u8Level = (UINT8)uvEng_GetConfig()->mark_find.detail_level;
		DOUBLE dbSmooth = (DOUBLE)uvEng_GetConfig()->mark_find.model_smooth;
		DOUBLE dbScaleMin = 0.0f, dbScaleMax = 0.0f, dbScoreRate;
		CUniToChar csCnv1, csCnv2;
		bool bpatFile = false, bmmfFile = false;
		//dbScoreRate = uvEng_GetConfig()->mark_find.score_rate;
		dbScoreRate = pstExpoRecipe->mark_score_accept;

		BOOL IsFind = false;
		CFileFind	finder;
		CUniToChar csCnv3, csCnv4, csCnv5;


		//ST_RECIPE_PARAM stMarkParam = CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->GetParam(EN_RECIPE_TAB::ALIGN, EN_RECIPE_ALIGN::GLOBAL_MARK_NAME + index);
		ST_RECIPE_PARAM stMarkParam = CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_SEL)->GetParam(EN_RECIPE_TAB::ALIGN, EN_RECIPE_ALIGN::GLOBAL_MARK_NAME + index);

		LPG_CMPV pstMark = uvEng_Mark_GetModelName(stMarkParam.GetValue().GetBuffer()); stMarkParam.GetValue().ReleaseBuffer();

		//마크 정보가 없으면 패스
		if (pstMark)
		{
			if (ENG_MMDT(pstMark->type) != ENG_MMDT::en_image)
			{
				bmmfFile = true;
				bpatFile = false;
				for (int i = 0; i < 2; i++) {
					uvEng_Camera_SetModelDefine_tot(i + 1, u8Speed, u8Level, uvEng_GetConfig()->mark_find.max_mark_find, dbSmooth,
						pstMark, GLOBAL_MARK + index, csCnv2.Ansi2Uni(pstMark->file),
						dbScaleMin, dbScaleMax, dbScoreRate);
				}
			}
			else if (ENG_MMDT(pstMark->type) == ENG_MMDT::en_image) {
				CHAR tmpfile[MARK_MODEL_NAME_LENGTH];
				//sprintf_s(tmpfile, MARK_MODEL_NAME_LENGTH, "%s\\data\\mmf\\%s.mmf",
				//	csCnv1.Uni2Ansi(g_tzWorkDir), csCnv2.Uni2Ansi(pstMark->file));
				sprintf_s(tmpfile, MARK_MODEL_NAME_LENGTH, "%s\\%s\\mmf\\%s.mmf",
					csCnv1.Uni2Ansi(g_tzWorkDir), CUSTOM_DATA_CONFIG2, pstMark->file);
				IsFind = finder.FindFile(csCnv3.Ansi2Uni(tmpfile));
				if (IsFind)
					bmmfFile = true;
				else
					bmmfFile = false;
				if (bmmfFile) {
					for (int i = 0; i < 2; i++) {
						uvEng_Camera_SetModelDefine_tot(i + 1, u8Speed, u8Level, uvEng_GetConfig()->mark_find.max_mark_find, dbSmooth,
							pstMark, GLOBAL_MARK + index, csCnv2.Ansi2Uni(tmpfile),
							dbScaleMin, dbScaleMax, dbScoreRate);
					}
				}

				//sprintf_s(tmpfile, MARK_MODEL_NAME_LENGTH, "%s\\data\\pat\\%s.pat",
				//	csCnv1.Uni2Ansi(g_tzWorkDir), csCnv4.Uni2Ansi(pstMark->file));
				sprintf_s(tmpfile, MARK_MODEL_NAME_LENGTH, "%s\\%s\\pat\\%s.pat",
					csCnv1.Uni2Ansi(g_tzWorkDir), CUSTOM_DATA_CONFIG2, pstMark->file);
				IsFind = finder.FindFile(csCnv5.Ansi2Uni(tmpfile));
				if (IsFind)
					bpatFile = true;
				else
					bpatFile = false;
				if (bpatFile) {
					for (int i = 0; i < 2; i++) {
						uvEng_Camera_SetModelDefine_tot(i + 1, u8Speed, u8Level, uvEng_GetConfig()->mark_find.max_mark_find, dbSmooth,
							pstMark, GLOBAL_MARK + index, csCnv2.Ansi2Uni(tmpfile),
							dbScaleMin, dbScaleMax, dbScoreRate);
					}
				}

				if (!bpatFile && !bmmfFile) {
					//dlgMesg.MyDoModal(L"Failed to load mark file(MMF & PAT)", 0x01);
					return FALSE;
				}
			}
		}
		}


	/*Philhmi 에서 Select 보낸 요청한 메세지 아닐 경우만 보고*/
	if (g_u16PhilCommand |= ePHILHMI_C2P_RECIPE_SELECT)
	{
		/*Philhmi에 보고*/
		PhilSendSelectRecipe(strRecipeName);
	}

	::SendMessage(m_hMainWnd, WM_MAIN_RECIPE_CHANGE, NULL, (LPARAM)&strRecipeName);

	return TRUE;
}

BOOL CRecipeManager::LoadSelectRecipe()
{
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();

	if (NULL == pstRecipe)
	{
		return FALSE;
	}

	LoadRecipe((CString)pstRecipe->job_name, eRECIPE_MODE_VIEW);
	LoadRecipe((CString)pstRecipe->job_name, eRECIPE_MODE_SEL);

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
				case EN_RECIPE_ALIGN::MARK_TYPE: stAlignRecipe.mark_type = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;
				case EN_RECIPE_ALIGN::ALIGN_TYPE: stAlignRecipe.align_type = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;
				case EN_RECIPE_ALIGN::LAMP_TYPE: stAlignRecipe.lamp_type = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;
				case EN_RECIPE_ALIGN::GAIN_LEVEL_CAM1:
				case EN_RECIPE_ALIGN::GAIN_LEVEL_CAM2:
					stAlignRecipe.gain_level[nCntParam - EN_RECIPE_ALIGN::GAIN_LEVEL_CAM1] = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;
				case EN_RECIPE_ALIGN::SEARCH_TYPE: stAlignRecipe.search_type = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
					break;
				case EN_RECIPE_ALIGN::SEARCH_COUNT: stAlignRecipe.search_count = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
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
	PhilSendModifyRecipe(stRecipe);

	uvEng_JobRecipe_RecipeModify(&stRecipe);
	uvEng_ExpoRecipe_RecipeModify(&stExpoRecipe);
	uvEng_Mark_AlignRecipeModify(&stAlignRecipe);
	stRecipe.Close();
	stExpoRecipe.Close();
	stAlignRecipe.Close();
	return TRUE;
}

BOOL CRecipeManager::UpdateRecipe(STG_RJAF &stRecipe, EN_RECIPE_MODE eRecipeMode)
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
			case EN_RECIPE_ALIGN::LAMP_TYPE: stRecipe.lamp_type = GetRecipe(eRecipeMode)->GetInt(nCntTab, nCntParam);
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
				case EN_RECIPE_EXPOSE::LED_DUTY_CYCLE	:
					stParam.SetValue(pstExpoRecipe->led_duty_cycle);
					break;
				case EN_RECIPE_EXPOSE::SCORE_ACCEPT		:
					stParam.SetValue(pstExpoRecipe->mark_score_accept);
					break;
				case EN_RECIPE_EXPOSE::SCALE_RANGE		:
					stParam.SetValue(pstExpoRecipe->mark_scale_range);
					break;
				case EN_RECIPE_EXPOSE::SERIAL_DECODE		:
					stParam.SetValue(pstExpoRecipe->dcode_serial);
					break;
				case EN_RECIPE_EXPOSE::SCALE_DECODE		:
					stParam.SetValue(pstExpoRecipe->dcode_scale);
					break;
				case EN_RECIPE_EXPOSE::TEXT_DECODE		:
					stParam.SetValue(pstExpoRecipe->dcode_text);
					break;
				case EN_RECIPE_EXPOSE::TEXT_STRING		:
					stParam.SetValue(pstExpoRecipe->text_string);
					break;
				case EN_RECIPE_EXPOSE::SERIAL_FLIP_HORZ	:
					stParam.SetValue(pstExpoRecipe->serial_flip_h);
					break;
				case EN_RECIPE_EXPOSE::SERIAL_FLIP_VERT	:
					stParam.SetValue(pstExpoRecipe->serial_flip_v);
					break;
				case EN_RECIPE_EXPOSE::SCALE_FLIP_HORZ	:
					stParam.SetValue(pstExpoRecipe->scale_flip_h);
					break;
				case EN_RECIPE_EXPOSE::SCALE_FLIP_VERT	:
					stParam.SetValue(pstExpoRecipe->scale_flip_v);
					break;
				case EN_RECIPE_EXPOSE::TEXT_FLIP_HORZ	:
					stParam.SetValue(pstExpoRecipe->text_flip_h);
					break;
				case EN_RECIPE_EXPOSE::TEXT_FLIP_VERT	:
					stParam.SetValue(pstExpoRecipe->text_flip_v);
					break;
				}
			}
			break;
			case EN_RECIPE_TAB::ALIGN:
			{

				switch (nCntParam)
				{
				case EN_RECIPE_ALIGN::ALIGN_NAME			: stParam.SetValue(pstAlignRecipe->align_name);
					break;
				case EN_RECIPE_ALIGN::MARK_TYPE				: stParam.SetValue(pstAlignRecipe->mark_type);
					break;
				case EN_RECIPE_ALIGN::ALIGN_TYPE			: stParam.SetValue(pstAlignRecipe->align_type);
					break;
				case EN_RECIPE_ALIGN::LAMP_TYPE				: stParam.SetValue(pstAlignRecipe->lamp_type);
					break;
				case EN_RECIPE_ALIGN::GAIN_LEVEL_CAM1		:
				case EN_RECIPE_ALIGN::GAIN_LEVEL_CAM2		:	
					stParam.SetValue(pstAlignRecipe->gain_level[nCntParam - EN_RECIPE_ALIGN::GAIN_LEVEL_CAM1]);
					break;
				case EN_RECIPE_ALIGN::SEARCH_TYPE			: stParam.SetValue(pstAlignRecipe->search_type);
					break;
				case EN_RECIPE_ALIGN::SEARCH_COUNT			: stParam.SetValue(pstAlignRecipe->search_count);
					break;
				case EN_RECIPE_ALIGN::MARK_AREA_WIDTH		: stParam.SetValue(pstAlignRecipe->mark_area[0]);
					break;
				case EN_RECIPE_ALIGN::MARK_AREA_HEIGHT		: stParam.SetValue(pstAlignRecipe->mark_area[1]);
					break;
				case EN_RECIPE_ALIGN::ALIGN_CAMERA_NUMBER1	: stParam.SetValue(pstAlignRecipe->acam_num[0]);
					break;
				case EN_RECIPE_ALIGN::ALIGN_CAMERA_NUMBER2	: stParam.SetValue(pstAlignRecipe->acam_num[1]);
					break;
				case EN_RECIPE_ALIGN::GLOBAL_MARK_NAME		: stParam.SetValue(pstAlignRecipe->m_name[0]);
					break;
				case EN_RECIPE_ALIGN::LOCAL_MARK_NAME		: stParam.SetValue(pstAlignRecipe->m_name[1]);
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
				case EN_RECIPE_ALIGN::GLOBAL_MARK_NAME: stParam.SetValue(pstAlignRecipe->m_name[0]);
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
			case EN_RECIPE_ALIGN::LAMP_TYPE: stParam.SetValue(pstAlignRecipe->lamp_type);
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
			case EN_RECIPE_ALIGN::GLOBAL_MARK_NAME: stParam.SetValue(pstAlignRecipe->m_name[0]);
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

int CRecipeManager::GetSelectRecipeIndex()
{
	for (int i = 0; i < (int)m_strArrRecipeList.GetCount(); i++)
	{
		if (0 == GetRecipeName().Compare(m_strArrRecipeList.GetAt(i)))
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
	CString strRecipeName = CRecipeManager::GetInstance()->GetRecipeName();

	BOOL bSuccess = uvEng_JobRecipe_SelRecipeOnlyName(strRecipeName.GetBuffer()); strRecipeName.ReleaseBuffer();

	if (FALSE == bSuccess)
	{
		return FALSE;
	}

	CHAR szJob[MAX_PATH_LEN] = { NULL };
	CUniToChar	csCnv;
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetRecipeOnlyName(strRecipeName.GetBuffer());	strRecipeName.ReleaseBuffer();

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


	/* 거버에 대한 마크 정보 얻기 */
	if (0x00 != uvEng_Luria_GetGlobalMarkJobName(tzGerb, lstX, lstY,
		ENG_ATGL::en_global_4_local_0x0_n_point))
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

VOID CRecipeManager::PhilSendCreateRecipe(STG_RJAF stRecipe)
{
	STG_PP_P2C_RCP_CREATE			stCreate;
	STG_PP_P2C_RCP_CREATE_ACK		stCreateAck;
	stCreate.Reset();
	stCreateAck.Reset();

	memcpy(stCreate.szRecipeName, stRecipe.job_name, DEF_MAX_RECIPE_NAME_LENGTH);

	/*노광 결과 파라미터값*/
	stCreate.usCount = 7;
	sprintf_s(stCreate.stVar[0].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stCreate.stVar[0].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Job Name");
	sprintf_s(stCreate.stVar[0].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", stRecipe.job_name);

	sprintf_s(stCreate.stVar[1].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stCreate.stVar[1].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Gerber Path");
	sprintf_s(stCreate.stVar[1].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", stRecipe.gerber_path);

	sprintf_s(stCreate.stVar[2].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stCreate.stVar[2].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Gerber Name");
	sprintf_s(stCreate.stVar[2].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", stRecipe.gerber_name);

	sprintf_s(stCreate.stVar[3].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stCreate.stVar[3].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Align Recipe");
	sprintf_s(stCreate.stVar[3].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", stRecipe.align_recipe);

	sprintf_s(stCreate.stVar[4].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stCreate.stVar[4].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Expo Recipe");
	sprintf_s(stCreate.stVar[4].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", stRecipe.expo_recipe);

	sprintf_s(stCreate.stVar[5].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "INT");
	sprintf_s(stCreate.stVar[5].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Material Thick");
	sprintf_s(stCreate.stVar[5].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%d", stRecipe.material_thick);

	sprintf_s(stCreate.stVar[6].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stCreate.stVar[6].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Expo Energy");
	sprintf_s(stCreate.stVar[6].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%.1f", stRecipe.expo_energy);

	uvEng_Philhmi_Send_P2C_RCP_CREATE(stCreate, stCreateAck);
}

VOID CRecipeManager::PhilSendModifyRecipe(STG_RJAF stRecipe)
{
	STG_PP_P2C_RCP_MODIFY			stModify;
	STG_PP_P2C_RCP_MODIFY_ACK		stModifyAck;
 		
	stModify.Reset();
	stModifyAck.Reset();

	memcpy(stModify.szRecipeName, stRecipe.job_name, DEF_MAX_RECIPE_NAME_LENGTH);

	/*노광 결과 파라미터값*/
	stModify.usCount = 7;
	sprintf_s(stModify.stVar[0].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stModify.stVar[0].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Job Name");
	sprintf_s(stModify.stVar[0].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", stRecipe.job_name);
		
	sprintf_s(stModify.stVar[1].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stModify.stVar[1].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Gerber Path");
	sprintf_s(stModify.stVar[1].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", stRecipe.gerber_path);
			  
	sprintf_s(stModify.stVar[2].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stModify.stVar[2].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Gerber Name");
	sprintf_s(stModify.stVar[2].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", stRecipe.gerber_name);
			  
	sprintf_s(stModify.stVar[3].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stModify.stVar[3].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Align Recipe");
	sprintf_s(stModify.stVar[3].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", stRecipe.align_recipe);
			  
	sprintf_s(stModify.stVar[4].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stModify.stVar[4].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Expo Recipe");
	sprintf_s(stModify.stVar[4].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%s", stRecipe.expo_recipe);
			  
	sprintf_s(stModify.stVar[5].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "INT");
	sprintf_s(stModify.stVar[5].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Material Thick");
	sprintf_s(stModify.stVar[5].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%d", stRecipe.material_thick);
			  
	sprintf_s(stModify.stVar[6].szParameterType, DEF_MAX_RECIPE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stModify.stVar[6].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Expo Energy");
	sprintf_s(stModify.stVar[6].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "%.1f", stRecipe.expo_energy);
 		
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