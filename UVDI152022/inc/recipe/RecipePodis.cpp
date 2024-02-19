
/*
 desc : Gerber Recipe Data ���� �� ����
*/

#include "pch.h"
#include "RecipePodis.h"	/* Gerber Recipe */
#include "LedPower.h"		/* Led Power Recipe */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : work_dir	- [in]  ���� ������ ����Ǵ� ��� (��ü ���, '\' ����)
		led_power	- [in]  Led Power Recipe ���� ��ü ������
		mem_luria	- [in]  Luria ���� �޸� ����ü ������
 retn : None
*/
CRecipePodis::CRecipePodis(PTCHAR work_dir, CLedPower *led_power, LPG_LDSM mem_luria)
	: CBase(work_dir)
{
	m_pLedPower		= led_power;
	m_pstRecipe		= NULL;
	m_pstLuriaMem	= mem_luria;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CRecipePodis::~CRecipePodis()
{
	RemoveAll();
}

/*
 desc : ���� ���õ� ������ �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CRecipePodis::ResetSelectRecipe()
{
	m_pstRecipe	= NULL;
}

/*
 desc : ���� ���õ� ������ �⺻ ��ȯ
 parm : None
 retn : �����ǰ� ����� ����ü ������
*/
LPG_RBGF CRecipePodis::GetSelectRecipe()
{
	return m_pstRecipe;
}

/*
 desc : ���� ��ϵ� ������ ��� ����
 parm : None
 retn : None
*/
VOID CRecipePodis::RemoveAll()
{
	POSITION pPos	= NULL;
	LPG_RBGF pRecipe= NULL;

	/* ���� ��ϵ� Recipe Data �޸� ���� */
	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		pRecipe	= m_lstRecipe.GetNext(pPos);
		if (pRecipe)
		{
			pRecipe->Close();	/* �޸� ���� */
			delete pRecipe;
		}
	}
	/* ����Ʈ ��� */
	m_lstRecipe.RemoveAll();
	/* ���� ���õ� ������ �ʱ�ȭ */
	m_pstRecipe	= NULL;
}

/*
 desc : Calibration Recipe ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CRecipePodis::LoadFile()
{
	CHAR szData[1024]			= {NULL};
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	errno_t eRet	= 0;
	FILE *fp		= NULL;
	CUniToChar csCnv;

	/* ���� ��ϵ� ������ ���� */
	RemoveAll();

	/* Recipe Name ���� ��� */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\data\\recipe\\%s.dat",
			   m_tzWorkDir, GetConfig()->file_dat.gerb_recipe);
	/* ���� ���� */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "rt");
	if (0 != eRet)	return TRUE;	/* Recipe File�� ��� TRUE ��ȯ */
	while (!feof(fp))
	{
		/* ���� �о���� */
		memset(szData, 0x00, 1024);
		fgets(szData, 1024, fp);
		/* ���ڿ� ���̰� Ư�� ���� ������ ������� (���� ù ���ڰ� �ּ��� �����ݷ�(;)�̸� Skip) */
		if (strlen(szData) > 0 && ';' != szData[0] && '\n' != szData[0])
		{
			/* �м� �� ����ϱ� */
			ParseRecipe(szData, (UINT32)strlen(szData));
		}
	}
	/* ���� �ڵ� �ݱ� */
	fclose(fp);

	return TRUE;
}

/*
 desc : Recipe �⺻ �м� �� ��� ����
 parm : data	- [in]  Recipe ������ ����� ���ڿ� ����
		size	- [in]  'data' ���ۿ� ����� �������� ����
 retn : TRUE or FALSE
*/
BOOL CRecipePodis::ParseRecipe(PCHAR data, UINT32 size)
{
	UINT8 u8Div			= 36;
	UINT32 u32Find		= 0, i, j;
	BOOL bIsLoop		= TRUE;
	CHAR *pData			= data, *pFind, szValue[1024];
	LPG_RBGF pstRecipe	= NULL;

	/* �ϴ�, �־��� ���ڿ� �߿��� �޸�(',') ������ xxx ������ Ȯ�� */
	for (i=0; i<size; i++)
	{
		if (',' == data[i])	u32Find++;
	}
	if (u32Find != u8Div)
	{
		AfxMessageBox(L"Failed to analyse the value from <recipe base> file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ������ ��ü �ӽ� ���� */
	pstRecipe	= new STG_RBGF;
	ASSERT(pstRecipe);

	/* �޸� �Ҵ� �� �ʱ�ȭ */
	pstRecipe->Init();
	pstRecipe->ResetMemData();

	/* recipe name */
	pFind	= strchr(pData, ',');
	if (pFind)	{	memcpy(pstRecipe->recipe_name, pData, pFind - pData);	pData = ++pFind;	}
	/* gerber path */
	pFind	= strchr(pData, ',');
	if (pFind)	{	memcpy(pstRecipe->gerber_path, pData, pFind - pData);	pData = ++pFind;	}
	/* gerber name */
	pFind	= strchr(pData, ',');
	memcpy(pstRecipe->gerber_name, pData, pFind - pData);
	pData = ++pFind;
	/* led power name */
	pFind	= strchr(pData, ',');
	memcpy(pstRecipe->power_name, pData, pFind - pData);
	pData = ++pFind;
	/* Lot Date */
	pFind	= strchr(pData, ',');
	if (pFind)	{	memcpy(pstRecipe->lot_date, pData, pFind - pData);	pData = ++pFind;	}
	/* text string */
	pFind	= strchr(pData, ',');
	if (pFind)	{	memcpy(pstRecipe->text_string, pData, pFind - pData);	pData = ++pFind;	}
	/* serial/scale/text decode */
	for (i=0; i<3; i++)
	{
		pFind	= strchr(pData, ',');
		if (pFind)
		{
			memset(szValue, 0x00, 1024);
			memcpy(szValue, pData, pFind - pData);
			switch (i)
			{
			case 0x00	: pstRecipe->dcode_serial	= (UINT16)atoi(szValue);
			case 0x01	: pstRecipe->dcode_scale	= (UINT16)atoi(szValue);
			case 0x02	: pstRecipe->dcode_text		= (UINT16)atoi(szValue);
			}
			pData = ++pFind;
		}
	}
	/* flip series*/
	for (i=0; i<6; i++)
	{
		pFind	= strchr(pData, ',');
		memset(szValue, 0x00, 1024);
		memcpy(szValue, pData, pFind - pData);
		switch (i)
		{
		case 0x00	:	pstRecipe->serial_flip_h= (UINT8)atoi(szValue);	break;
		case 0x01	:	pstRecipe->serial_flip_v= (UINT8)atoi(szValue);	break;
		case 0x02	:	pstRecipe->scale_flip_h	= (UINT8)atoi(szValue);	break;
		case 0x03	:	pstRecipe->scale_flip_v	= (UINT8)atoi(szValue);	break;
		case 0x04	:	pstRecipe->text_flip_h	= (UINT8)atoi(szValue);	break;
		case 0x05	:	pstRecipe->text_flip_v	= (UINT8)atoi(szValue);	break;
		}
		pData = ++pFind;
	}
	/* mark distance diff */
	for (i=0; i<MAX_GLOBAL_MARK_DIST_CHECK_COUNT; i++)
	{
		pFind	= strchr(pData, ',');
		memset(szValue, 0x00, 1024);
		memcpy(szValue, pData, pFind - pData);
		pData	= ++pFind;
		pstRecipe->global_mark_dist[i]	= (UINT32)atoi(szValue);
	}

	/* �ݵ�� 0 ���� �ʱ�ȭ */
	for (i=0x00; i<0x0e; i++)
	{
		switch (i)
		{
		case 0x00 : case 0x01 : case 0x02 : case 0x03 : case 0x04 : case 0x05 :
		case 0x06 : case 0x07 : case 0x08 : case 0x09 : case 0x0a : case 0x0c : case 0x0d : 
			pFind	= strchr(pData, ',');
			memset(szValue, 0x00, 1024);
			memcpy(szValue, pData, pFind - pData);
			pData	= ++pFind;
			switch (i)
			{
			case 0x00 : pstRecipe->material_thick	= (UINT32)atoi(szValue);	break;
			case 0x01 : pstRecipe->cali_thick		= (UINT32)atoi(szValue);	break;
			case 0x02 : pstRecipe->step_size		= (UINT8)atoi(szValue);		break;
			case 0x03 : pstRecipe->led_duty_cycle	= (UINT8)atoi(szValue);		break;
			case 0x04 : pstRecipe->frame_rate		= (UINT16)atoi(szValue);	break;
			case 0x05 : pstRecipe->mark_score_accept= (DOUBLE)atof(szValue);	break;
			case 0x06 : pstRecipe->mark_scale_range	= (DOUBLE)atof(szValue);
						if (pstRecipe->mark_scale_range < 50.0f)
							pstRecipe->mark_scale_range = 50.0f;				break;
			case 0x07 : pstRecipe->align_type		= (UINT8)atoi(szValue);		break;
			case 0x08 : pstRecipe->mark_model		= (UINT8)atoi(szValue);		break;
			case 0x09 : pstRecipe->align_method		= (UINT8)atoi(szValue);		break;
			case 0x0a : pstRecipe->mark_count		= (UINT8)atoi(szValue);		break;
			case 0x0c : pstRecipe->gain_level[0]	= (UINT8)atoi(szValue);		break;
			case 0x0d : pstRecipe->gain_level[1]	= (UINT8)atoi(szValue);		break;
			}
			break;

		case 0x0b :
			for (j=0; j<2; j++)
			{
				pFind	= strchr(pData, ',');
				memset(szValue, 0x00, 1024);
				if ((pFind - pData) > 1)	memcpy(szValue, pData, pFind - pData);
				pData	= ++pFind;
				pstRecipe->mark_area[j]	= (UINT32)atoi(szValue);
			}
			break;
		}
	}

	/* ���� �Ŀ� (�ε���)�� Frame Rate �׸��� Scroll Rate ��� ������ �������� �ӵ��� ���� ��� */
	if (!CalcSpeedEnergy(pstRecipe))
	{
		delete pstRecipe;
		return FALSE;
	}

	/* �޸𸮿� �м��� ������ ������ ��� */
	m_lstRecipe.AddTail(pstRecipe);

	return TRUE;
}


/*
 desc : Recipe �⺻ ���� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CRecipePodis::SaveFile()
{
	UINT8 i;
	CHAR szData[256]	= {NULL};
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	errno_t eRet		= 0;
	FILE *fp			= NULL;
	POSITION pPos		= NULL;
	LPG_RBGF pstRecipe	= NULL;
	CUniToChar csCnv;

	/* Recipe File ���� */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\data\\recipe\\%s.dat",
			   m_tzWorkDir, GetConfig()->file_dat.gerb_recipe);
	/* ���� ���� */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "wt");
	if (0 != eRet)	return TRUE;

	fputs("; recipe_name,gerber_path,gerber_name,led_power_name,lot_date,text_string,serial_decode,"
		  "scale_decode,serial_flip_h,v,scale_flip_h,v,text_flip_h,v,global_mark_dist_1,2,3,4,5,6,"
		  "material_thick,cali_thick,step_size,led_duty_cycle,frame_rate,score_accept,scale_range,align_type,"
		  "mark_model,align_method,mark_count,mark_area_w,h\n", fp);

	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		/* �� �������� */
		pstRecipe	= (LPG_RBGF)m_lstRecipe.GetNext(pPos);
		/* recipe name*/
		sprintf_s(szData, 256,	"%s,", pstRecipe->recipe_name);
		fputs(szData, fp);
		/* gerber path */
		sprintf_s(szData, 256,	"%s,", pstRecipe->gerber_path);
		fputs(szData, fp);
		/* gerber_name */
		sprintf_s(szData, 256,	"%s,", pstRecipe->gerber_name);
		fputs(szData, fp);
		/* led_power_name, lot_date, text_string */
		sprintf_s(szData, 256,	"%s,%s,%s,",
				  pstRecipe->power_name, pstRecipe->lot_date, pstRecipe->text_string);
		fputs(szData, fp);
		/* recipe name, gerber path, gerber_name */
		sprintf_s(szData, 256,	"%u,%u,%u,%d,%d,%d,%d,%d,%d,",
				  pstRecipe->dcode_serial, pstRecipe->dcode_scale, pstRecipe->dcode_text,
				  pstRecipe->serial_flip_h, pstRecipe->serial_flip_v, pstRecipe->scale_flip_h,
				  pstRecipe->scale_flip_v, pstRecipe->text_flip_h, pstRecipe->text_flip_v);
		fputs(szData, fp);
		/* Mark Distance Difference */
		for (i=0; i<MAX_GLOBAL_MARK_DIST_CHECK_COUNT; i++)
		{
			sprintf_s(szData, 256, "%u,", pstRecipe->global_mark_dist[i]);	fputs(szData, fp);
		}
		/* material_thick, calibration_thick */
		sprintf_s(szData, 256, "%u,%u,%u,%u,%u,%.4f,%.4f,%u,%u,%u,%u,%u,%u,%u,%u,\n",
				  pstRecipe->material_thick, pstRecipe->cali_thick, pstRecipe->step_size,
				  pstRecipe->led_duty_cycle, pstRecipe->frame_rate, pstRecipe->mark_score_accept,
				  pstRecipe->mark_scale_range, pstRecipe->align_type, pstRecipe->mark_model,
				  pstRecipe->align_method,pstRecipe->mark_count, pstRecipe->mark_area[0],
				  pstRecipe->mark_area[1], pstRecipe->gain_level[0], pstRecipe->gain_level[1]);
		fputs(szData, fp);
	}
	/* ���� �ڵ� �ݱ� */
	fclose(fp);

	return TRUE;
}

/*
 desc : ���� ���õ� �������� �Ӽ� ������ ���� �������� �̵� �ӵ� �� ���� �� ���
 parm : recipe	- [out] �������� �̵� �ӵ� �� ���� ���� �� ��� �� ��ȯ (mm/sec)
 retn : TRUE or FALSE
*/
BOOL CRecipePodis::CalcSpeedEnergy(LPG_RBGF recipe)
{
	UINT8 i, j;
	DOUBLE dbReqSpeed	= 0.0f, dbTimeOnePoint = 0.0f, dbExposeArea = 0.0f, dbPixelSize = 0.0f;
	DOUBLE dbPowerCm2	= 0.0f, dbSumEnergy = 0.0f, dbFactor = 0.0f;
	DOUBLE dbSensorArea	= 0.0f, dbImageArea = 0.0f;
	DOUBLE dbMaxSpeed	= 0.0f/*uvCmn_Luria_GetExposeMaxSpeed(recipe->step_size)*/;
	FLOAT **pfLedPower	= NULL;

	/* �ִ� �뱤 �ӵ� */
	dbMaxSpeed		= m_pstLuriaMem->machine.scroll_rate *
					  (GetConfig()->luria_svc.pixel_size /*+ MIRROR_PERIOD_UM*/)/1000.0f * recipe->step_size;

	/* Power Name�� ���� Watt �� ��� */
	if (!m_pLedPower->GetLedPowerName(recipe->power_name))
	{
		AfxMessageBox(L"Failed to get the led_watt value corresponding to the led power name", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	pfLedPower		= m_pLedPower->GetLedPowerName(recipe->power_name)->led_watt;
	if (!pfLedPower)
	{
		AfxMessageBox(L"Failed to get the led_watt value corresponding to the led power name", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
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

	return TRUE;
}

/*
 desc : ������ �⺻ ���� ����
 parm : recipe	- [in]  �˻��ϰ��� �ϴ� ������ �̸�
 retn : TRUE (Finded) or FALSE (Not find)
*/
BOOL CRecipePodis::RecipeFind(PCHAR recipe)
{
	POSITION pPos	= NULL;

	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		if (0 == strcmp(recipe, m_lstRecipe.GetNext(pPos)->recipe_name))	return TRUE;
	}
	return FALSE;
}

/*
 desc : ������ �̸� �˻��� ���� ���� - �⺻ ���� ����
 parm : recipe	- [in]  �����ϰ��� �ϴ� ������ �̸�
 retn : TRUE (Deleted) or FALSE (Not delete)
*/
BOOL CRecipePodis::RemoveRecipe(PCHAR recipe)
{
	POSITION pPos	= NULL, pPrePos;
	LPG_RBGF pstData= NULL;

	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		/* ��� ���� ã�� ��ġ ���� */
		pPrePos	= pPos;
		pstData	= m_lstRecipe.GetNext(pPos);
		if (0 == strcmp(recipe, pstData->recipe_name))
		{
			pstData->Close();
			delete pstData;
			m_lstRecipe.RemoveAt(pPrePos);
			return TRUE;
		}
	}
	/* ���� ���õ� ������ ���� */
	ResetSelectRecipe();

	return FALSE;
}

/*
 desc : ������ �̸��� ���� ������ �⺻ ���� ��ȯ
 parm : recipe	- [in]  �˻��ϰ��� �ϴ� ������ �̸�
 retn : ��ȯ�� �����ǰ� ����� ����ü ������
*/
LPG_RBGF CRecipePodis::GetRecipeOnlyName(PCHAR recipe)
{
	POSITION pPos	= NULL;
	LPG_RBGF pstData= NULL;

	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		pstData	= m_lstRecipe.GetNext(pPos);
		if (0 == strcmp(recipe, pstData->recipe_name))	return pstData;
	}
	return NULL;
}

/*
 desc : ������ �̸� (��ü ��� ����)�� ���� ������ ��ȯ
 parm : path_name	- [in]  �˻��ϰ��� �ϴ� �Ź� �̸� (��� ����)
 retn : ��ȯ�� �����ǰ� ����� ����ü ������
*/
LPG_RBGF CRecipePodis::GetRecipePathName(PCHAR name)
{
	CHAR szGerberPath[MAX_GERBER_NAME];
	POSITION pPos	= NULL;
	LPG_RBGF pstData= NULL;

	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		pstData	= m_lstRecipe.GetNext(pPos);
		memset(szGerberPath, 0x00, MAX_GERBER_NAME);
		sprintf_s(szGerberPath, MAX_GERBER_NAME, "%s\\%s",
					pstData->gerber_path, pstData->gerber_name);
		if (0 == strcmp(name, szGerberPath))	return pstData;
	}

	return NULL;
}

/*
 desc : ������ �⺻ ������ ����� �ε��� ��ġ�� �ִ� ������ ��ȯ
 parm : index	- [in]  ���������� �ϴ� ������ �ε��� (Zero-based)
 retn : ��ȯ�� �����ǰ� ����� ����ü ������
*/
LPG_RBGF CRecipePodis::GetRecipeIndex(INT32 index)
{
	POSITION pPos	= NULL;

	/* �߸��� Index ���� ���Դ��� ���� */
	if (index < 0)	return NULL;
	/* �ش� ��ġ �� ��� */
	pPos	= m_lstRecipe.FindIndex(index);
	if (!pPos)	return NULL;

	/* ��ġ�� �ش�Ǵ� ��� �� ��ȯ */
	return m_lstRecipe.GetAt(pPos);
}

int CRecipePodis::GetSelectRecipeIndex()
{
	POSITION pPos = NULL;
	LPG_RBGF pstData = NULL;

	if (NULL == m_pstRecipe)
	{
		return -1;
	}

	pPos = m_lstRecipe.GetHeadPosition();
	int nIndex = 0;
	while (pPos)
	{
		pstData = m_lstRecipe.GetNext(pPos);
		if (0 == strcmp(m_pstRecipe->recipe_name, pstData->recipe_name))	return nIndex;
		nIndex++;
	}
	return NULL;
}

/*
 desc : ������ �⺻ �̸��� ���� ������ �⺻ ����
 parm : recipe	- [in]  �˻��ϰ��� �ϴ� ������ �⺻ �̸�
 retn : TRUE (���� ����) or FALSE (���� ����)
*/
BOOL CRecipePodis::SelRecipeOnlyName(PCHAR recipe)
{
	BOOL bSucc			= FALSE;
	POSITION pPos		= NULL;
	LPG_RBGF pstRecipe	= NULL;

	/* ������ �⺻ ���� �˻� �� ���� */
	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		pstRecipe	= m_lstRecipe.GetNext(pPos);
		if (0 == strcmp(recipe, pstRecipe->recipe_name))
		{
			m_pstRecipe	= pstRecipe;
			bSucc	= TRUE;
			break;
		}
	}

	/* �����ǿ� �⺻�� ����Ǵ� ������ �Ӽ� ������ ���ٸ�... */
	if (!bSucc)	m_pstRecipe	= NULL;

	return bSucc;
}

/*
 desc : ������ �⺻ �̸� (��ü ��� ����)�� ���� ������ �⺻ ����
 parm : recipe	- [in]  �˻��ϰ��� �ϴ� ������ �⺻ �̸� (��ü ��� ����)
 retn : TRUE (���� ����) or FALSE (���� ����)
*/
BOOL CRecipePodis::SelRecipePathName(PCHAR recipe)
{
	CHAR szGerberPath[MAX_GERBER_NAME];
	BOOL bSucc			= FALSE;
	POSITION pPos		= NULL;
	LPG_RBGF pstRecipe	= NULL;

	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos && !bSucc)
	{
		pstRecipe	= m_lstRecipe.GetNext(pPos);
		memset(szGerberPath, 0x00, MAX_GERBER_NAME);
		sprintf_s(szGerberPath, MAX_GERBER_NAME, "%s\\%s",
					pstRecipe->gerber_path, pstRecipe->gerber_name);
		if (0 == strcmp(recipe, szGerberPath))
		{
			m_pstRecipe	= pstRecipe;
			bSucc		= TRUE;
			break;
		}
	}
	if (!bSucc)	return FALSE;

	/* �����ǿ� �⺻�� ����Ǵ� ������ �Ӽ� ������ ���ٸ�... */
	if (!bSucc)	m_pstRecipe	= NULL;

	return FALSE;
}

/*
 desc : Recipe �⺻ ���� ���� ����ü �� ���� (����ü ���ο� �Ҵ�� �޸𸮰� �׸��� �����Ƿ�)
 parm : source	- [in]  ���� ���� ����� ��
		target	- [in]  ���ο� ���� ����(����)�� ��
 retn : TRUE or FALSE
*/
BOOL CRecipePodis::CopyRecipe(LPG_RBGF source, LPG_RBGF target)
{
	UINT8 i	= 0x00, j = 0;

	/* ���� �޸� ���� �ʱ�ȭ */
	target->ResetMemData();

	/* Source Energy ��� */
	if (!CalcSpeedEnergy(source))	return FALSE;

	/* �⺻ ����ü ��� ���� ���� */
	memcpy(target, source, sizeof(STG_RBGF) - sizeof(PUINT8) * 8);

	strcpy_s(target->gerber_path,	MAX_PATH_LEN,			source->gerber_path);
	strcpy_s(target->recipe_name,	RECIPE_NAME_LENGTH,		source->recipe_name);
	strcpy_s(target->gerber_name,	MAX_GERBER_NAME,		source->gerber_name);
	strcpy_s(target->lot_date,		MAX_LOT_DATE_LENGTH,	source->lot_date);
	strcpy_s(target->power_name,	LED_POWER_NAME_LENGTH,	source->power_name);
	strcpy_s(target->text_string,	MAX_PANEL_TEXT_STRING,	source->text_string);
	memcpy(target->expo_energy,	source->expo_energy, sizeof(FLOAT) * MAX_PH);
	memcpy(target->global_mark_dist,source->global_mark_dist,
		   sizeof(UINT32) * MAX_GLOBAL_MARK_DIST_CHECK_COUNT);

	return TRUE;
}

/*
 desc : Recipe �⺻ ���� �߰� (Append)
 parm : recipe	- [in]  Recipe ������ ����� ����ü ������
		check	- [in]  �� ��ȿ�� üũ ����
 retn : TRUE or FALSE (���� �ʰ�. �ִ� 15��)
*/
BOOL CRecipePodis::RecipeAppend(LPG_RBGF recipe)
{
	BOOL bSucc			= FALSE;
	LPG_RBGF pstRecipe	= NULL;

	if (m_lstRecipe.GetCount() >= MAX_REGIST_RECIPE_COUNT)
	{
		AfxMessageBox(L"The registered recipe (Base Info.) is full (MAX. 256)", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* �ʼ��� �ԷµǴ� ���� �ִ��� ���� */
	if (!recipe->IsValid())
	{
		AfxMessageBox(L"The input data (recipe base info) is invalid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ������ ��ϵ� �����ǰ� �ִ��� Ȯ�� */
	if (RecipeFind(recipe->recipe_name))
	{
		AfxMessageBox(L"The same recipe (Base Info.) exists", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* �� ������ ��ġ�� ��� */
	pstRecipe	= new STG_RBGF;
	ASSERT(pstRecipe);
	pstRecipe->Init();	/* ����ü ���� ���� �޸� ���� */

	/* ���� ������ ����ü ������ ���� ������ �� ���� */
	bSucc	= CopyRecipe(recipe, pstRecipe);
	/* ��� */
	if (bSucc)	m_lstRecipe.AddTail(pstRecipe);

	/* ������� ��ϵ� ��� Recipe ������ ���Ϸ� ���� */
	return bSucc && SaveFile();
}

/*
 desc : Recipe �⺻ ���� ����
 parm : recipe	- [in]  Recipe �⺻ ������ ����� ����ü ������
 retn : TRUE or FALSE
*/
BOOL CRecipePodis::RecipeModify(LPG_RBGF recipe)
{
	LPG_RBGF pstRecipe	= NULL;

	/* �ʼ��� �ԷµǴ� ���� �ִ��� ���� */
	if (!recipe->IsValid())
	{
		AfxMessageBox(L"The update data (Base Info.) is invalid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ������ ��ϵ� �����ǰ� �ִ��� Ȯ�� */
	pstRecipe = GetRecipeOnlyName(recipe->recipe_name);
	if (!pstRecipe)
	{
		AfxMessageBox(L"The target recipe (Base Info.) was not found", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ���� ������ ����ü ������ ���� ������ �� ���� */
	CopyRecipe(recipe, pstRecipe);

	/* ������� ��ϵ� ��� Recipe ������ ���Ϸ� ���� */
	return SaveFile();
}

/*
 desc : Recipe �⺻ ����
 parm : recipe	- [in]  �����Ϸ��� �������� �⺻ �̸�
 retn : TRUE or FALSE
*/
BOOL CRecipePodis::RecipeDelete(PCHAR recipe)
{
	/* ������ ��ϵ� �����ǰ� �ִ��� Ȯ�� */
	if (!GetRecipeOnlyName(recipe))
	{
		AfxMessageBox(L"The deleted recipe_name was not found", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* Recipe ���� ���� */
	RemoveRecipe(recipe);

	/* ������� ��ϵ� ��� Recipe ������ ���Ϸ� ���� */
	return SaveFile();
}

/*
 desc : ���� ���õ� Recipe�� Mark ���� ����� Shared Type���� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CRecipePodis::IsRecipeSharedType()
{
	if (!m_pstRecipe)	return FALSE;
	switch (m_pstRecipe->align_type)
	{
	case ENG_ATGL::en_global_4_local_0x0_n_point	:
	case ENG_ATGL::en_global_3_local_0x0_n_point	:
	case ENG_ATGL::en_global_2_local_0x0_n_point	:

	case ENG_ATGL::en_global_4_local_2x2_n_point	:
	case ENG_ATGL::en_global_4_local_3x2_n_point	:
	case ENG_ATGL::en_global_4_local_4x2_n_point	:
	case ENG_ATGL::en_global_4_local_5x2_n_point	:
		return FALSE;
	}
	return TRUE;
}

/*
 desc : ���� ���õ� Recipe�� Mark ���� ����� Local Mark�� �����ϴ��� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CRecipePodis::IsExistLocalMark()
{
	if (!m_pstRecipe)	return FALSE;
	switch (m_pstRecipe->align_type)
	{
	case ENG_ATGL::en_global_4_local_0x0_n_point	:
	case ENG_ATGL::en_global_4_local_2x2_n_point	:
	case ENG_ATGL::en_global_4_local_3x2_n_point	:
	case ENG_ATGL::en_global_4_local_4x2_n_point	:
	case ENG_ATGL::en_global_4_local_5x2_n_point	:
	case ENG_ATGL::en_global_4_local_2x2_s_point	:
	case ENG_ATGL::en_global_4_local_3x2_s_point	:
	case ENG_ATGL::en_global_4_local_4x2_s_point	:
	case ENG_ATGL::en_global_4_local_5x2_s_point	:
		return TRUE;
	}
	return FALSE;
}

/*
 desc : ���� ���õ� �����ǿ� ���Ե� ��ü ��ũ ����
 parm : None
 retn : ���� ��ȯ
*/
UINT8 CRecipePodis::GetSelectRecipeLocalMarkCount()
{
	if (!m_pstRecipe)	return 0x00;
	switch (m_pstRecipe->align_type)
	{
	case ENG_ATGL::en_global_4_local_0x0_n_point	:
	case ENG_ATGL::en_global_3_local_0x0_n_point	:
	case ENG_ATGL::en_global_2_local_0x0_n_point	:	return 0;

	case ENG_ATGL::en_global_4_local_2x2_n_point	:	return 16;
	case ENG_ATGL::en_global_4_local_3x2_n_point	:	return 24;
	case ENG_ATGL::en_global_4_local_4x2_n_point	:	return 32;
	case ENG_ATGL::en_global_4_local_5x2_n_point	:	return 40;

	case ENG_ATGL::en_global_4_local_2x2_s_point	:	return 9;
	case ENG_ATGL::en_global_4_local_3x2_s_point	:	return 12;
	case ENG_ATGL::en_global_4_local_4x2_s_point	:	return 15;
	case ENG_ATGL::en_global_4_local_5x2_s_point	:	return 18;
	}
	return 0x00;
}

/*
 desc : ���������� ��ġ�� Mark Index ���� Camera �� ���̰� �� ��° �̹��� �������� ��ȯ
 parm : mark	- [in]  Mark Index (Zero based)
		cam_id	- [out] Camera Number (1 or 2)
		img_id	- [out] Camera Grabbed Image Number (Zero based)
 retn : TRUE or FALSE
*/
BOOL CRecipePodis::GetLocalMarkToGrabNum(UINT8 mark, UINT8 &cam_id, UINT8 &img_id)
{
	UINT8 u8Count	= GetSelectRecipeLocalMarkCount();
	if (!m_pstRecipe || u8Count < 1)	return FALSE;

	/* �� �ʱ�ȭ (�ݵ�� Max ������ �ʱ�ȭ) */
	cam_id	= 0xff;
	img_id	= 0xff;

	switch (m_pstRecipe->align_type)
	{
	case ENG_ATGL::en_global_4_local_2x2_n_point	:
	case ENG_ATGL::en_global_4_local_3x2_n_point	:
	case ENG_ATGL::en_global_4_local_4x2_n_point	:
	case ENG_ATGL::en_global_4_local_5x2_n_point	:
		cam_id	= (mark < (u8Count / 2)) ? 0x01 : 0x02;;
		img_id	= (UINT8)ROUNDDN((mark % (u8Count / 2)), 0) + 2 /* Global Mark Number 0, 1�� �ֱ� ������ */;
		break;

	case ENG_ATGL::en_global_4_local_2x2_s_point	:
	case ENG_ATGL::en_global_4_local_3x2_s_point	:
	case ENG_ATGL::en_global_4_local_4x2_s_point	:
	case ENG_ATGL::en_global_4_local_5x2_s_point	:
		if (mark < (u8Count / 3))
		{
			cam_id	= 0x01;
			img_id	= mark + 2 /* Global Mark Number 0, 1�� �ֱ� ������ */;
		}
		else
		{
			cam_id	= 0x02;
			img_id	= (mark - ((m_pstRecipe->align_type & 0x0f) + 2)) + 2 /* Global Mark Number 0, 1�� �ֱ� ������ */;
		}
		break;

	default	:	return FALSE;
	}

	return TRUE;
}

/*
 desc : ���� Local Mark Index �� ���� �ش�� Scan ��ȣ �� ��ȯ
 parm : mark_id	- [in]  Local Mark Index �� (0 or Later)
 retn : Scan Number (Zero Based)
*/
UINT8 CRecipePodis::GetLocalMarkToScanNum(UINT8 mark_id)
{
	UINT8 u8Mark	= 0;
	if (!m_pstRecipe || mark_id < 4)	return 0;

	switch (m_pstRecipe->align_type)
	{
	case ENG_ATGL::en_global_4_local_2x2_n_point	:
	case ENG_ATGL::en_global_4_local_3x2_n_point	:
	case ENG_ATGL::en_global_4_local_4x2_n_point	:
	case ENG_ATGL::en_global_4_local_5x2_n_point	:
		u8Mark	= ((m_pstRecipe->align_type & 0x0f) + 1) * 4;
		break;

	case ENG_ATGL::en_global_4_local_2x2_s_point	:
	case ENG_ATGL::en_global_4_local_3x2_s_point	:
	case ENG_ATGL::en_global_4_local_4x2_s_point	:
	case ENG_ATGL::en_global_4_local_5x2_s_point	:
		u8Mark	= ((m_pstRecipe->align_type & 0x0f) + 1) * 3;
		break;
	}

	return (u8Mark > 0 && u8Mark <= mark_id) ? 0x01 : 0x00;
}

/*
 desc : ���� Camera Index�� Grabbed Image�� ������ �� ��° �ε����� ����Ǿ� �ִ��� ��ȯ
 parm : cam_id	- [in]  Camera Number (1 or 2)
		img_id	- [in]  Camera Grabbed Image Number (Zero based)
		mark	- [out] Mark Index (Zero based) ��ȯ
 retn : TRUE or FALSE
*/
BOOL CRecipePodis::GetGrabNumToLocalMark(UINT8 cam_id, UINT8 img_id, UINT8 &mark)
{
	UINT8 u8Count	= GetSelectRecipeLocalMarkCount();

	if (!m_pstRecipe || u8Count < 1)	return FALSE;
	switch (m_pstRecipe->align_type)
	{
	case ENG_ATGL::en_global_4_local_2x2_n_point	:
	case ENG_ATGL::en_global_4_local_3x2_n_point	:
	case ENG_ATGL::en_global_4_local_4x2_n_point	:
	case ENG_ATGL::en_global_4_local_5x2_n_point	:
		mark	= 4 * ((m_pstRecipe->align_type & 0x0f) + 1) * (cam_id - 1) + img_id;
		break;
	case ENG_ATGL::en_global_4_local_2x2_s_point	:
	case ENG_ATGL::en_global_4_local_3x2_s_point	:
	case ENG_ATGL::en_global_4_local_4x2_s_point	:
	case ENG_ATGL::en_global_4_local_5x2_s_point	:
#if 0
		mark	= 3 * ((m_pstRecipe->align_type & 0x0f) + 1) * (cam_id - 1) + img_id;
#else
		if (0x01 == cam_id)	mark	= img_id;
		else				mark	= (m_pstRecipe->align_type & 0x0f) + 2 + img_id;
#endif
		break;
	default	:	return FALSE;
	}
	return TRUE;
}

/*
 desc : ���� ī�޶󸶴� Grabbed Image ��ȣ�� �ش�Ǵ� ���� (�������� �̵� ����) ���� ��ȯ
 parm : img_id	- [in]  Camera Grabbed Image Number (Zero based)
 retn : TRUE (Normal : ������; ����) or FALSE (������; ����)
*/
BOOL CRecipePodis::GetImageToStageDirect(UINT8 img_id)
{
	if (!m_pstRecipe || img_id < 2)	return TRUE;	/* Global Mark�� ��� ������ ������ */
	switch (m_pstRecipe->align_type)
	{
	/* Normal Type */
	case ENG_ATGL::en_global_4_local_2x2_n_point	:	if (5 >= img_id)	return FALSE;	break;
	case ENG_ATGL::en_global_4_local_3x2_n_point	:	if (7 >= img_id)	return FALSE;	break;
	case ENG_ATGL::en_global_4_local_4x2_n_point	:	if (9 >= img_id)	return FALSE;	break;
	case ENG_ATGL::en_global_4_local_5x2_n_point	:	if (11 >= img_id)	return FALSE;	break;
	/* Shared Type */
	case ENG_ATGL::en_global_4_local_2x2_s_point	:	if (4 >= img_id)	return FALSE;	break;
	case ENG_ATGL::en_global_4_local_3x2_s_point	:	if (5 >= img_id)	return FALSE;	break;
	case ENG_ATGL::en_global_4_local_4x2_s_point	:	if (6 >= img_id)	return FALSE;	break;
	case ENG_ATGL::en_global_4_local_5x2_s_point	:	if (7 >= img_id)	return FALSE;	break;
	}

	return TRUE;
}

/*
 desc : ����� ��ũ �˻��� ���� 1 Scan �ϰ� �Ǹ�, �˻��Ǵ� ��ũ�� ���� ��,
		1 Scan �� ��ϵ� ��ũ�� ���� (���� ���õ� ������(�Ź�)�� ����)
 parm : type	- [in]  ��ũ ���� ����
 retn : 1 Scan �� ��ϵ� ��ũ�� ���� ��ȯ �ִ� 10 �� (����. 0x00)
*/
UINT8 CRecipePodis::GetScanLocalMarkCount()
{
	switch (m_pstRecipe->align_type)
	{
	/* Normal Type */
	case ENG_ATGL::en_global_4_local_2x2_n_point	:	return 0x04;
	case ENG_ATGL::en_global_4_local_3x2_n_point	:	return 0x06;
	case ENG_ATGL::en_global_4_local_4x2_n_point	:	return 0x08;
	case ENG_ATGL::en_global_4_local_5x2_n_point	:	return 0x0a;
	/* Shared Type */
	case ENG_ATGL::en_global_4_local_2x2_s_point	:	return 0x03;
	case ENG_ATGL::en_global_4_local_3x2_s_point	:	return 0x04;
	case ENG_ATGL::en_global_4_local_4x2_s_point	:	return 0x05;
	case ENG_ATGL::en_global_4_local_5x2_s_point	:	return 0x06;
	}

	return 0x00;
}