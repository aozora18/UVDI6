
/*
 desc : Gerber Recipe Data ���� �� ����
*/

#include "pch.h"
#include "RecipeGen2i.h"
#include "LedPower.h"		/* Led Power Recipe */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : work_dir- [in]  ���� ������ ����Ǵ� ��� (��ü ���, '\' ����)
		config	- [in]  ��ü ȯ�� ������ ����� ����ü ������
 retn : None
*/
CRecipeGen2i::CRecipeGen2i(TCHAR *work_dir, LPG_CIEA config)
{
	m_pstSelRecipe	= NULL;
	m_pstCfg		= config;

	wmemset(m_tzWorkDir, 0x00, _MAX_PATH);
	wcscpy_s(m_tzWorkDir, _MAX_PATH, work_dir);
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CRecipeGen2i::~CRecipeGen2i()
{
	RecipeRemoveAll();
}

/*
 desc : ���� ��ϵ� ������ ��� ����
 parm : None
 retn : None
*/
VOID CRecipeGen2i::RecipeRemoveAll()
{
	POSITION pPos	= NULL;
	LPG_RIGP pRecipe= NULL;

	/* ���� ��ϵ� Recipe Data �޸� ���� */
	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		pRecipe	= m_lstRecipe.GetNext(pPos);
		if (pRecipe)
		{
			pRecipe->Close();	/* �޸� ���� */
			::Free(pRecipe);
		}
	}
	/* ����Ʈ ��� */
	m_lstRecipe.RemoveAll();
	/* ���� ���õ� ������ �ʱ�ȭ */
	m_pstSelRecipe	= NULL;
}

/*
desc : ���� ������ ����� Gerber Recipe ���ϵ��� ���� ��ü�� ����
parm : None
retn : TRUE or FALSE
*/
BOOL CRecipeGen2i::RecipeLoad()
{
	BOOL bFind	= FALSE;
	TCHAR tzDir[_MAX_PATH] = {NULL};
	CString strOnlyFileName;
	CFileFind csFF;	

	/* ���� ��ϵ� ������ ���� */
	RecipeRemoveAll();
		
	/* Recipe �˻� ����� Directory ���� */
	swprintf_s(tzDir, _MAX_PATH, L"%s\\data\\recipe\\*.rcp", g_tzWorkDir);
	bFind	= csFF.FindFile(tzDir);
	while (bFind)
	{
		/* ����/������ �����Ѵٸ� TRUE ��ȯ */
		bFind = csFF.FindNextFile();
		/* ���� �϶� */
		if (csFF.IsArchived())
		{
			/* ������ �̸� */
#if 0
			strOnlyFileName	= csFF.GetFileName();	/* Ȯ���� ���� */
#else
			strOnlyFileName	= csFF.GetFileTitle();	/* Ȯ���� ������ */
#endif
			/* Data parsing */
			if (!ParseAppend(strOnlyFileName.GetBuffer()))	return FALSE;
		}		
	}
	
	return TRUE;
}

/*
 desc : ������ ���� ���� �߸��� ������ ����Ǿ��ٰ� ���� �޽��� ���
 parm : file	- [in]  Recipe File
		item	- [in]  �˻� ��� Key ���ڿ� (�˻� ��� ���� �̸�)
 retn : FALSE (fixed)
*/
BOOL CRecipeGen2i::InvalidSetMesg(PTCHAR file, PTCHAR item)
{
	TCHAR tzMesg[256]	= {NULL};
	swprintf_s(tzMesg, 256,
			   L"The contents of the recipe file are set incorrectly [%s.rcp : %s]", file, item);
	AfxMessageBox(tzMesg, MB_ICONSTOP|MB_TOPMOST);

	return FALSE;	/* fixed */
}

/*
desc : Recipe �м� �� ��� ����
parm : file_name	- [in]  Recipe �̸� (��� �� Ȯ���� ������)
retn : TRUE or FALSE
*/
BOOL CRecipeGen2i::ParseAppend(PTCHAR recipe)
{
	TCHAR  tzFile[_MAX_PATH]= {NULL};
	TCHAR tzData[128]		= {NULL};
	DWORD dwReaded			= 0;
	LPG_RIGP pstData		= {NULL};
	CUniToChar csCnv;

	/* �޸� �Ҵ� �� �ʱ�ȭ */
	pstData = (LPG_RIGP)::Alloc(sizeof(STG_RIGP));
	ASSERT(pstData);
	pstData->Init();

	/* ������ ���� �б� ���� ��ü ���丮 ������ ���� ���� */
	swprintf_s(tzFile, _MAX_PATH, L"%s\\data\\recipe\\%s.rcp", g_tzWorkDir, recipe);

	/* 01.recipe name */
	dwReaded = GetPrivateProfileString(L"RECIPE_PARAMETER", L"RECIPE_NAME", L"", tzData, 128, tzFile);
	if (dwReaded < 1)	return InvalidSetMesg(recipe, L"RECIPE_NAME");
	if (dwReaded >= MAX_RECIPE_NAME_LEN)
	{
		AfxMessageBox(L"The length of the registered recipe name (128 bytes) has been exceeded",
					  MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	else	strcpy_s(pstData->recipe_name, MAX_RECIPE_NAME_LEN, csCnv.Uni2Ansi(tzData));
	/* 02.gerber name */
	dwReaded = GetPrivateProfileString(L"RECIPE_PARAMETER", L"IMAGE_FILE_PATH", L"", tzData, 128, tzFile);
	if (dwReaded >= MAX_GERBER_NAME)
	{
		AfxMessageBox(L"The length of the registered gerber name (200 bytes) has been exceeded",
					  MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	else	strcpy_s(pstData->gerber_name, MAX_GERBER_NAME, csCnv.Uni2Ansi(tzData));
	/* 03.get lot type */
	dwReaded = GetPrivateProfileString(L"RECIPE_PARAMETER", L"LOT_GET_MODE", L"0", tzData, 128, tzFile);
	if (dwReaded < 1)	return InvalidSetMesg(recipe, L"LOT_GET_MODE");
	pstData->get_lot = (UINT8)_wtol(tzData);
	/* 04.put lot type */
	dwReaded = GetPrivateProfileString(L"RECIPE_PARAMETER", L"LOT_PUT_MODE", L"0", tzData, 128, tzFile);
	if (dwReaded < 1)	return InvalidSetMesg(recipe, L"LOT_PUT_MODE");
	pstData->put_lot = (UINT8)_wtol(tzData);
	/* 05.mark1 pattern number */
	dwReaded = GetPrivateProfileString(L"RECIPE_PARAMETER", L"PATTERN_MARK1", L"0", tzData, 128, tzFile);
	if (dwReaded < 1)	return InvalidSetMesg(recipe, L"PATTERN_MARK1");
	pstData->mark1_pattern = (UINT8)_wtol(tzData);
	/* 06.mark2 pattern number */
	dwReaded =GetPrivateProfileString(L"RECIPE_PARAMETER", L"PATTERN_MARK2", L"0", tzData, 128, tzFile);
	if (dwReaded < 1)	return InvalidSetMesg(recipe, L"PATTERN_MARK2");
	pstData->mark2_pattern = (UINT8)_wtol(tzData);
	/* 07.wafer thickness */
	dwReaded = GetPrivateProfileString(L"RECIPE_PARAMETER", L"WAFER_THICK", L"0", tzData, 128, tzFile);
	if (dwReaded < 1)	return InvalidSetMesg(recipe, L"WAFER_THICK");
	pstData->material_thick = (UINT32)_wtol(tzData);
	/* 08.expose energy */
	dwReaded = GetPrivateProfileString(L"RECIPE_PARAMETER", L"EXPO_ENERGY", L"0.000000", tzData, 128, tzFile);
	if (dwReaded < 1)	return InvalidSetMesg(recipe, L"EXPO_ENERGY");
	pstData->expo_energy_avg = (FLOAT)_wtof(tzData);
	/* 09.step size */
	dwReaded = GetPrivateProfileString(L"RECIPE_PARAMETER", L"STEP_SIZE", L"0", tzData, 128, tzFile);
	if (dwReaded < 1)	return InvalidSetMesg(recipe, L"STEP_SIZE");
	pstData->step_size = (UINT8)_wtol(tzData);
	/* 10.duty cycle */
	dwReaded = GetPrivateProfileString(L"RECIPE_PARAMETER", L"LED_DUTY_CYCLE", L"0", tzData, 128, tzFile);
	if (dwReaded < 1)	return InvalidSetMesg(recipe, L"LED_DUTY_CYCLE");
	pstData->led_duty_cycle = (UINT8)_wtol(tzData);
	/* 11.frame rate */
	dwReaded = GetPrivateProfileString(L"RECIPE_PARAMETER", L"FRAME_RATE", L"0", tzData, 128, tzFile);
	if (dwReaded < 1)	return InvalidSetMesg(recipe, L"FRAME_RATE");
	pstData->frame_rate = (UINT16)_wtol(tzData);
	/* 12.expo led power 365 */
	dwReaded = GetPrivateProfileString(L"RECIPE_PARAMETER", L"LED_POWER_365", L"0.000000", tzData, 128, tzFile);
	if (dwReaded < 1)	return InvalidSetMesg(recipe, L"LED_POWER_365");
	pstData->led_365nm = (FLOAT)_wtof(tzData);
	/* 13.expo led power 385 */
	dwReaded = GetPrivateProfileString(L"RECIPE_PARAMETER", L"LED_POWER_385", L"0.000000", tzData, 128, tzFile);
	if (dwReaded < 1)	return InvalidSetMesg(recipe, L"LED_POWER_385");
	pstData->led_385nm = (FLOAT)_wtof(tzData);
	/* 14.expo led power 390 */
	dwReaded = GetPrivateProfileString(L"RECIPE_PARAMETER", L"LED_POWER_390", L"0.000000", tzData, 128, tzFile);
	if (dwReaded < 1)	return InvalidSetMesg(recipe, L"LED_POWER_390");
	pstData->led_390nm = (FLOAT)_wtof(tzData);
	/* 15.expo led power 405 */
	dwReaded = GetPrivateProfileString(L"RECIPE_PARAMETER", L"LED_POWER_405", L"0.000000", tzData, 128, tzFile);
	if (dwReaded < 1)	return InvalidSetMesg(recipe, L"LED_POWER_405");
	pstData->led_405nm = (FLOAT)_wtof(tzData);
	/* 16.rotation */
	dwReaded = GetPrivateProfileString(L"RECIPE_PARAMETER", L"ROTATION", L"0.000000", tzData, 128, tzFile);
	if (dwReaded < 1)	return InvalidSetMesg(recipe, L"ROTATION");
	pstData->rotation = (FLOAT)_wtof(tzData);
	/* 17.scale x */
	dwReaded = GetPrivateProfileString(L"RECIPE_PARAMETER", L"SCALE_X", L"0.000000", tzData, 128, tzFile);
	if (dwReaded < 1)	return InvalidSetMesg(recipe, L"SCALE_X");
	pstData->scale_x = (FLOAT)_wtof(tzData);
	/* 18.scale y */
	dwReaded = GetPrivateProfileString(L"RECIPE_PARAMETER", L"SCALE_Y", L"0.000000", tzData, 128, tzFile);
	if (dwReaded < 1)	return InvalidSetMesg(recipe, L"SCALE_Y");
	pstData->scale_y = (FLOAT)_wtof(tzData);
	/* 19.offset x */
	dwReaded = GetPrivateProfileString(L"RECIPE_PARAMETER", L"OFFSET_X", L"0.000000", tzData, 128, tzFile);
	if (dwReaded < 1)	return InvalidSetMesg(recipe, L"OFFSET_X");
	pstData->offset_x = (FLOAT)_wtof(tzData);
	/* 20.offset y */
	dwReaded = GetPrivateProfileString(L"RECIPE_PARAMETER", L"OFFSET_Y", L"0.000000", tzData, 128, tzFile);
	if (dwReaded < 1)	return InvalidSetMesg(recipe, L"OFFSET_Y");
	pstData->offset_y = (FLOAT)_wtof(tzData);

	/* �޸𸮿� �м��� ������ ������ ��� */
	m_lstRecipe.AddTail(pstData);

	return TRUE;
}

/*
 desc : �������� �̵� �ӵ� �� ���� �� ���
 parm : recipe	- [in]  ������ ������ ����� ����ü ������
 retn : None
*/
VOID CRecipeGen2i::CalcSpeedEnergy(LPG_RIGP recipe)
{
#if 0	/* Old */
	UINT8 i	= 0;
	DOUBLE dbEnergyTotal	= 0.0f;
	DOUBLE dbPixelSize		= (m_pstCfg->luria_svc.pixel_size / 1000000.0f);	/* unit: um */
 	DOUBLE dbFrameRate		= data->frame_rate / 1000.0f;						/* 0 ~ 950 */
	DOUBLE dbLedDutyCycle   = data->led_duty_cycle / 100.0f;					/* unit: percent */
	DOUBLE dbScrollRate		= m_pstCfg->luria_svc.scroll_rate * 1.0f;
	DOUBLE dbStepSize		= data->step_size * 1.0f;
	DOUBLE dbMirrCountX		= m_pstCfg->luria_svc.mirror_count_xy[0] * 1.0f;
	DOUBLE dbMirrCountY		= m_pstCfg->luria_svc.mirror_count_xy[1] * 1.0f;

	/* Exposure Stage Speed (unit : mm/sec) */
	data->expo_speed	= (FLOAT)ROUNDED(dbScrollRate *
										 dbPixelSize / 1000.0f *	/* unit : mm */
										 dbFrameRate *				/* range : 0.0 ~ 1.0 */
										 dbStepSize, 3);			/* range : 1 ~ 4 */

	/* Exposure Energy (mJ) */
	for (; i<m_pstCfg->luria_svc.ph_count; i++)
	{
		data->expo_energy[i]= (FLOAT)ROUNDED((dbPixelSize / 1000.0f /
											  ((dbScrollRate * dbPixelSize / 1000.0f * dbStepSize) * dbFrameRate) *
											  dbMirrCountY * dbLedDutyCycle) *
											 (data->led_power_watt[i][0] + data->led_power_watt[i][1] +
											  data->led_power_watt[i][2] + data->led_power_watt[i][3]) /
											 ((dbMirrCountX * dbPixelSize / 1000.0f * dbMirrCountY * dbPixelSize / 1000.0f) /
											  100.0f) * 1000.0f, 3);
		dbEnergyTotal		+= data->expo_energy[i];
	}
	/* ��ü ���а��� ��� �뱤�� ��� */
	data->expo_energy_avg	= (FLOAT)ROUNDED(dbEnergyTotal /m_pstCfg->luria_svc.ph_count, 3);
#else	/* New */
	UINT8 i, j;
	DOUBLE dbReqSpeed	= 0.0f, dbTimeOnePoint = 0.0f, dbExposeArea = 0.0f, dbPixelSize = 0.0f;
	DOUBLE dbPowerCm2	= 0.0f, dbSumEnergy = 0.0f, dbFactor = 0.0f;
	DOUBLE dbSensorArea	= 0.0f, dbImageArea = 0.0f;
	DOUBLE dbMaxSpeed	= 0.0f/*uvCmn_Luria_GetExposeMaxSpeed(recipe->step_size)*/;
	FLOAT **pfLedPower	= NULL;

	/* �ִ� �뱤 �ӵ� */
	dbMaxSpeed		= m_pstCfg->luria_svc.scroll_rate *
					  (GetConfig()->luria_svc.pixel_size /*+ MIRROR_PERIOD_UM*/)/1000.0f * recipe->step_size;

	/* Expose Speed */
	recipe->expo_speed	= FLOAT(dbMaxSpeed * (recipe->frame_rate / 1000.0f));
	/* Pixel Size (mm) */
	dbPixelSize		= GetConfig()->luria_svc.pixel_size / 1000.0f;
	/* Requirement Speed */
	dbReqSpeed		= dbMaxSpeed * (recipe->frame_rate / 1000.0f);
	/* Time on one point (R) */
	dbTimeOnePoint	= (dbPixelSize / dbReqSpeed) * 
					  (GetConfig()->luria_svc.mirror_count_xy[1] * (recipe->led_duty_cycle / 100.0f));
	/* Expose Area */
	dbExposeArea	= (GetConfig()->luria_svc.mirror_count_xy[0] * dbPixelSize) *
					  (GetConfig()->luria_svc.mirror_count_xy[1] * dbPixelSize);
	/* Image Area (cm^2) */
	dbImageArea		= ((GetConfig()->luria_svc.mirror_count_xy[0] * dbPixelSize) *
					   (GetConfig()->luria_svc.mirror_count_xy[1] * dbPixelSize)) / 100.0f;
	/* Illuminator Sensor Area */
	dbSensorArea	= (M_PI * pow((GetConfig()->luria_svc.sensor_diameter / 2.0f), 2)) / 100.0f;
	/* 5�� ���а��� Power (Watt) �� */
	for (i=0; i<GetConfig()->luria_svc.ph_count; i++)
	{
		/* Sum Factor */
		for (j=0,dbFactor=0.0f; j<4; j++)
		{
			dbFactor	+= ((pfLedPower[i][j] * GetConfig()->luria_svc.illum_filter_rate[j] * 
							GetConfig()->luria_svc.correction_factor * dbImageArea) / dbSensorArea) / 1000.0f;
		}
		/* Power of CM^2 (W/cm^2) */
		dbPowerCm2	= dbFactor / (dbExposeArea / 100.0f);
		/* Photohead ���� �뱤�� ��� */
		recipe->expo_energy[i]	= (FLOAT)(dbPowerCm2 * dbTimeOnePoint * 1000.0f);
		dbSumEnergy	+= recipe->expo_energy[i];
	}
	/* ��ü ���а��� ��� �뱤�� ��� */
	recipe->expo_energy_avg	= (FLOAT)ROUNDED(dbSumEnergy / GetConfig()->luria_svc.ph_count, 3);
#endif
}

/*
 desc : ������ �̸� �˻�
 parm : name	- [in]  �˻��ϰ��� �ϴ� ������ �̸�
 retn : TRUE (Finded) or FALSE (Not find)
*/
BOOL CRecipeGen2i::RecipeFind(PTCHAR recipe)
{
	POSITION pPos = NULL;
	CUniToChar csCnv;

	pPos = m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		if (0 == strcmp(csCnv.Uni2Ansi(recipe),
						m_lstRecipe.GetNext(pPos)->recipe_name))	return TRUE;
	}
	return FALSE;
}

/*
desc : ������ �̸� �˻��� ���� ����
parm : recipe	- [in]  �����ϰ��� �ϴ� ������ �̸�
retn : TRUE (Deleted) or FALSE (Not delete)
*/
BOOL CRecipeGen2i::RemoveRecipe(PTCHAR recipe)
{
	POSITION pPos		= NULL, pPrePos;
	LPG_RIGP pstRecipe	= NULL;
	CFileFind csFF;
	TCHAR  tzFile[_MAX_PATH] = { NULL };
	CUniToChar csCnv;

	pPos = m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		/* ��� ���� ã�� ��ġ ���� */
		pPrePos		= pPos;
		pstRecipe	= m_lstRecipe.GetNext(pPos);

		if (0 == strcmp(csCnv.Uni2Ansi(recipe), pstRecipe->recipe_name))
		{
			/* Recipe Directory ���� */
			swprintf_s(tzFile, _MAX_PATH, L"%s\\data\\recipe\\%s.rcp", g_tzWorkDir, recipe);
			if (csFF.FindFile(tzFile))
			{
				/* ������ ���� (���� ���� ����) */
				if (DeleteFile(tzFile) == TRUE)
				{
					pstRecipe->Close();
					::Free(pstRecipe);
					m_lstRecipe.RemoveAt(pPrePos);
					return TRUE;
				}				
			}	
		}
	}

	return FALSE;
}
/*
 desc : ������ �̸��� ���� ������ ��ȯ
 parm : recipe	- [in]  �˻��ϰ��� �ϴ� ������ �̸�
 retn : ��ȯ�� �����ǰ� ����� ����ü ������
*/
LPG_RIGP CRecipeGen2i::GetRecipeName(PTCHAR recipe)
{
	POSITION pPos		= NULL;
	LPG_RIGP pstRecipe	= NULL;
	CUniToChar csCnv;

	pPos = m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		pstRecipe	= m_lstRecipe.GetNext(pPos);
		if (0 == strcmp(csCnv.Uni2Ansi(recipe), pstRecipe->recipe_name))	return pstRecipe;
	}

	return NULL;
}

/*
 desc : ������ �̸��� ���� ������ ����
 parm : recipe	- [in]  �˻��ϰ��� �ϴ� ������ �̸�
 retn : TRUE (���� ����) or FALSE (���� ����)
*/
BOOL CRecipeGen2i::SetRecipeName(PTCHAR recipe)
{
	POSITION pPos		= NULL;
	LPG_RIGP pstRecipe	= NULL;
	CUniToChar csCnv;

	pPos = m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		pstRecipe	= m_lstRecipe.GetNext(pPos);
		if (0 == strcmp(csCnv.Uni2Ansi(recipe), pstRecipe->recipe_name))
		{
			m_pstSelRecipe	= pstRecipe;
			return TRUE;
		}
	}

	return FALSE;
}

/*
 desc : �����ǰ� ����� �ε��� ��ġ�� �ִ� ������ ��ȯ
 parm : index	- [in]  ���������� �ϴ� ������ �ε��� (Zero-based)
 retn : ��ȯ�� �����ǰ� ����� ����ü ������
*/
LPG_RIGP CRecipeGen2i::GetRecipeIndex(INT32 index)
{
	POSITION pPos		= NULL;

	if (index >= MAX_RECIPE_COUNT)	return NULL;

	/* �ش� ��ġ �� ��� */
	pPos	= m_lstRecipe.FindIndex(index);
	if (!pPos)	return NULL;

	/* ��ġ�� �ش�Ǵ� ��� �� ��ȯ */
	return m_lstRecipe.GetAt(pPos);
}

/*
 desc : ȯ�� ���Ͽ� DOUBLE �� �����ϱ�
 parm : subj- [in]  Main Subject
		key	- [in]  Main Key
		val	- [in]  �����ϰ��� �ϴ� DOUBLE ��
		pts	- [in]  �Ҽ��� �ڸ� ����
		file- [in]  ���� ��� ����
 retn : None
*/
VOID CRecipeGen2i::SetConfigDouble(PTCHAR subj, PTCHAR key, DOUBLE val, UINT8 pts, PTCHAR file)
{
	TCHAR tzData[128]	= {NULL}, tzPoint[32] = {NULL};

	if (pts == 0)	swprintf_s(tzData, 128, L" %lf", val);
	else
	{
		swprintf_s(tzPoint, 32, L" %%.%df", pts);
		swprintf_s(tzData, 128, tzPoint, val);
	}
	WritePrivateProfileString(subj, key, tzData, file);
}

/*
 desc : Recipe Save
 parm : recipe	- [in]  �����ϰ��� �ϴ� ������ �̸�
 retn : TRUE or FALSE
*/
BOOL CRecipeGen2i::RecipeSave(PTCHAR recipe)
{
	TCHAR tzFile[_MAX_PATH] = { NULL }, tzData[128] = { NULL };
	LPG_RIGP pstData = NULL;
	CFile csFile;
	CUniToChar csCnv;

	/* Recipe File ���� */
	swprintf_s(tzFile, _MAX_PATH, L"%s\\data\\recipe\\%s.rcp", g_tzWorkDir, recipe);
	if (!csFile.Open(tzFile, CFile::modeRead))
	{
		csFile.Open(tzFile, CFile::modeCreate | CFile::modeWrite);
		csFile.Write(L"", 0);
	}
	csFile.Close();

	/* Save �� ������ ���� ��� */
	pstData = GetRecipeName(recipe);
	/* 01.recipe name */
	WritePrivateProfileString(L"RECIPE_PARAMETER", L"RECIPE_NAME", csCnv.Ansi2Uni(pstData->recipe_name), tzFile);
	/* 02.gerber name */
	WritePrivateProfileString(L"RECIPE_PARAMETER", L"IMAGE_FILE_PATH", csCnv.Ansi2Uni(pstData->gerber_name), tzFile);
	/* 03.get lot type */
	swprintf_s(tzData, 8, L"%u", pstData->get_lot);
	WritePrivateProfileString(L"RECIPE_PARAMETER", L"LOT_GET_MODE", tzData, tzFile);
	/* 04.put lot type */
	swprintf_s(tzData, 8, L"%u", pstData->put_lot);
	WritePrivateProfileString(L"RECIPE_PARAMETER", L"LOT_PUT_MODE", tzData, tzFile);
	/* 05.mark1 pattern number */
	swprintf_s(tzData, 8, L"%u", pstData->mark1_pattern);
	WritePrivateProfileString(L"RECIPE_PARAMETER", L"PATTERN_MARK1", tzData, tzFile);
	/* 06.mark2 pattern number */
	swprintf_s(tzData, 8, L"%u", pstData->mark2_pattern);
	WritePrivateProfileString(L"RECIPE_PARAMETER", L"PATTERN_MARK2", tzData, tzFile);
	/* 07.wafer thickness */
	swprintf_s(tzData, 32, L"%u", pstData->material_thick);
	WritePrivateProfileString(L"RECIPE_PARAMETER", L"WAFER_THICK", tzData, tzFile);
	/* 08.expose energy */
	SetConfigDouble(L"RECIPE_PARAMETER", L"EXPO_ENERGY", pstData->expo_energy_avg, 2, tzFile);
	/* 09.step size */
	pstData->step_size = 1;
	swprintf_s(tzData, 8, L"%u", pstData->step_size);
	WritePrivateProfileString(L"RECIPE_PARAMETER", L"STEP_SIZE", tzData, tzFile);
	/* 10.duty cycle */
	swprintf_s(tzData, 8, L"%u", pstData->led_duty_cycle);
	WritePrivateProfileString(L"RECIPE_PARAMETER", L"LED_DUTY_CYCLE", tzData, tzFile);
	/* 11.frame rate */
	swprintf_s(tzData, 16, L"%u", pstData->frame_rate);
	WritePrivateProfileString(L"RECIPE_PARAMETER", L"FRAME_RATE", tzData, tzFile);
	/* 12.expo led power 365 */
	pstData->led_365nm = (FLOAT)1.00062;
	SetConfigDouble(L"RECIPE_PARAMETER", L"LED_POWER_365", pstData->led_365nm, 6, tzFile);
	/* 13.expo led power 385 */
	pstData->led_385nm = (FLOAT)1.00049;
	SetConfigDouble(L"RECIPE_PARAMETER", L"LED_POWER_385", pstData->led_385nm, 6, tzFile);
	/* 14.expo led power 390 */
	pstData->led_390nm = (FLOAT)0.90300;
	SetConfigDouble(L"RECIPE_PARAMETER", L"LED_POWER_390", pstData->led_390nm, 6, tzFile);
	/* 15.expo led power 405 */
	pstData->led_405nm = (FLOAT)1.00050;
	SetConfigDouble(L"RECIPE_PARAMETER", L"LED_POWER_405", pstData->led_405nm, 6, tzFile);
	/* 16.rotation */
	pstData->rotation = (FLOAT)0.000000;
	SetConfigDouble(L"RECIPE_PARAMETER", L"ROTATION", pstData->rotation, 2, tzFile);
	/* 17.scale x */
	pstData->scale_x = (FLOAT)0.000000;
	SetConfigDouble(L"RECIPE_PARAMETER", L"SCALE_X", pstData->scale_x, 6, tzFile);
	/* 18.scale y */
	pstData->scale_y = (FLOAT)0.000000;
	SetConfigDouble(L"RECIPE_PARAMETER", L"SCALE_Y", pstData->scale_y, 6, tzFile);
	/* 19.offset x */
	pstData->offset_x = (FLOAT)0.000000;
	SetConfigDouble(L"RECIPE_PARAMETER", L"OFFSET_X", pstData->offset_x, 6, tzFile);
	/* 20.offset y */
	pstData->offset_x = (FLOAT)0.000000;
	SetConfigDouble(L"RECIPE_PARAMETER", L"OFFSET_Y", pstData->offset_x, 6, tzFile);

	return TRUE;
}

/*
 desc : Recipe ���� ����ü �� ���� (����ü ���ο� �Ҵ�� �޸𸮰� �׸��� �����Ƿ�)
 parm : source	- [in]  ���� ���� ����� ��
		target	- [in]  ���ο� ���� ����(����)�� ��
 retn : None
*/
VOID CRecipeGen2i::CopyRecipe(LPG_RIGP source, LPG_RIGP target)
{
	target->led_duty_cycle	= source->led_duty_cycle;
	target->material_thick	= source->material_thick;
	target->frame_rate		= source->frame_rate;
	target->expo_energy_avg = source->expo_energy_avg;

	//target->recipe_num		= source->recipe_num;
	target->get_lot			= source->get_lot;
	target->put_lot			= source->put_lot;
	target->mark1_pattern	= source->mark1_pattern;
	target->mark2_pattern	= source->mark2_pattern;
	
	/* ����ü ���� �޸� �Ҵ�� �׸��� ������ ���� ���� */
	memcpy(target->recipe_name, source->recipe_name, strlen(source->recipe_name));
	memcpy(target->gerber_name, source->gerber_name, strlen(source->gerber_name));
	
	memcpy(target->led_power_index[0],	source->led_power_index[0],	sizeof(UINT16)*MAX_PH*MAX_LED);
	memcpy(target->led_power_watt[0],	source->led_power_watt[0],	sizeof(FLOAT)*MAX_PH*MAX_LED);
}

/*
desc : Recipe Append
parm : recipe	- [in]  Recipe ������ ����� ����ü ������
retn : TRUE or FALSE (���� �ʰ�. �ִ� 15��)
*/
BOOL CRecipeGen2i::RecipeAppend(LPG_RIGP recipe)
{
	LPG_RIGP pstRecipe = NULL;
	CUniToChar csCnv;

	if (m_lstRecipe.GetCount() >= MAX_RECIPE_COUNT)
	{
		AfxMessageBox(L"The registered recipe is full (MAX. 1000)", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* �ʼ��� �ԷµǴ� ���� �ִ��� ���� */
	if (!recipe->IsValid())
	{
		AfxMessageBox(L"The input data is invalid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ������ ��ϵ� �����ǰ� �ִ��� Ȯ�� */
	if (RecipeFind(csCnv.Ansi2Uni(recipe->recipe_name)))
	{
		AfxMessageBox(L"There is a name for the same recipe", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* �� ������ ��ġ�� ��� */
	pstRecipe	= (LPG_RIGP)::Alloc(sizeof(STG_RIGP));
	ASSERT(pstRecipe);
	pstRecipe->Init();	/* ����ü ���� ���� �޸� ���� */

	/* ���� ������ ����ü ������ ���� ������ �� ���� */
	CopyRecipe(recipe, pstRecipe);
	/* ���� �Ŀ� (�ε���)�� Frame Rate �׸��� Scroll Rate ��� ������ �������� �ӵ��� ���� ��� */
	CalcSpeedEnergy(pstRecipe);
	/* ��� */
	m_lstRecipe.AddTail(pstRecipe);

	/* ������� ��ϵ� ��� Recipe ������ ���Ϸ� ���� */
	return RecipeSave(csCnv.Ansi2Uni(pstRecipe->recipe_name));
}
/*
 desc : Recipe Delete
 parm : recipe	- [in]  Recipe ������ ����� ����ü ������
 retn : TRUE or FALSE
*/
BOOL CRecipeGen2i::RecipeModify(LPG_RIGP recipe)
{
	LPG_RIGP pstRecipe	= NULL;
	CUniToChar csCnv;

	/* �ʼ��� �ԷµǴ� ���� �ִ��� ���� */
	if (!recipe->IsValid())
	{
		AfxMessageBox(L"The update data is invalid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ������ ��ϵ� �����ǰ� �ִ��� Ȯ�� */
	pstRecipe = GetRecipeName(csCnv.Ansi2Uni(recipe->recipe_name));
	if (!pstRecipe)
	{
		AfxMessageBox(L"The target gerber file was not found", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ���� �Ŀ� (�ε���)�� Frame Rate �׸��� Scroll Rate ��� ������ �������� �ӵ��� ���� ��� */
	CalcSpeedEnergy(pstRecipe);

	/* ������� ��ϵ� ��� Recipe ������ ���Ϸ� ���� */
	return RecipeSave(csCnv.Ansi2Uni(pstRecipe->recipe_name));
}

/*
 desc : Recipe Modify
 parm : recipe	- [in]  �����Ϸ��� �������� �̸�
 retn : TRUE or FALSE
*/
BOOL CRecipeGen2i::RecipeDelete(PTCHAR recipe)
{
	/* ������ ��ϵ� �����ǰ� �ִ��� Ȯ�� */
	if (!GetRecipeName(recipe))
	{
		AfxMessageBox(L"The deleted gerber file was not found", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* Recipe ���� ���� */
	return RemoveRecipe(recipe);
}
