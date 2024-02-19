
/*
 desc : Gerber Recipe Data 적재 및 관리
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
 desc : 생성자
 parm : work_dir	- [in]  실행 파일이 실행되는 경로 (전체 경로, '\' 제외)
		led_power	- [in]  Led Power Recipe 저장 객체 포인터
		mem_luria	- [in]  Luria 공유 메모리 구조체 포인터
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
 desc : 파괴자
 parm : None
 retn : None
*/
CRecipePodis::~CRecipePodis()
{
	RemoveAll();
}

/*
 desc : 기존 선택된 레시피 초기화
 parm : None
 retn : None
*/
VOID CRecipePodis::ResetSelectRecipe()
{
	m_pstRecipe	= NULL;
}

/*
 desc : 현재 선택된 레시피 기본 반환
 parm : None
 retn : 레시피가 저장된 구조체 포인터
*/
LPG_RBGF CRecipePodis::GetSelectRecipe()
{
	return m_pstRecipe;
}

/*
 desc : 기존 등록된 레시피 모두 제거
 parm : None
 retn : None
*/
VOID CRecipePodis::RemoveAll()
{
	POSITION pPos	= NULL;
	LPG_RBGF pRecipe= NULL;

	/* 기존 등록된 Recipe Data 메모리 해제 */
	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		pRecipe	= m_lstRecipe.GetNext(pPos);
		if (pRecipe)
		{
			pRecipe->Close();	/* 메모리 해제 */
			delete pRecipe;
		}
	}
	/* 리스트 비움 */
	m_lstRecipe.RemoveAll();
	/* 기존 선택된 레시피 초기화 */
	m_pstRecipe	= NULL;
}

/*
 desc : Calibration Recipe 적재
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

	/* 기존 등록된 레시피 제거 */
	RemoveAll();

	/* Recipe Name 정보 얻기 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\data\\recipe\\%s.dat",
			   m_tzWorkDir, GetConfig()->file_dat.gerb_recipe);
	/* 파일 열기 */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "rt");
	if (0 != eRet)	return TRUE;	/* Recipe File이 없어도 TRUE 반환 */
	while (!feof(fp))
	{
		/* 한줄 읽어오기 */
		memset(szData, 0x00, 1024);
		fgets(szData, 1024, fp);
		/* 문자열 길이가 특정 개수 이하인 경우인지 (만약 첫 글자가 주석인 세미콜론(;)이면 Skip) */
		if (strlen(szData) > 0 && ';' != szData[0] && '\n' != szData[0])
		{
			/* 분석 및 등록하기 */
			ParseRecipe(szData, (UINT32)strlen(szData));
		}
	}
	/* 파일 핸들 닫기 */
	fclose(fp);

	return TRUE;
}

/*
 desc : Recipe 기본 분석 및 등록 진행
 parm : data	- [in]  Recipe 정보가 저장된 문자열 버퍼
		size	- [in]  'data' 버퍼에 저장된 데이터의 길이
 retn : TRUE or FALSE
*/
BOOL CRecipePodis::ParseRecipe(PCHAR data, UINT32 size)
{
	UINT8 u8Div			= 36;
	UINT32 u32Find		= 0, i, j;
	BOOL bIsLoop		= TRUE;
	CHAR *pData			= data, *pFind, szValue[1024];
	LPG_RBGF pstRecipe	= NULL;

	/* 일단, 주어진 문자열 중에서 콤마(',') 개수가 xxx 개인지 확인 */
	for (i=0; i<size; i++)
	{
		if (',' == data[i])	u32Find++;
	}
	if (u32Find != u8Div)
	{
		AfxMessageBox(L"Failed to analyse the value from <recipe base> file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 레시피 객체 임시 생성 */
	pstRecipe	= new STG_RBGF;
	ASSERT(pstRecipe);

	/* 메모리 할당 및 초기화 */
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

	/* 반드시 0 으로 초기화 */
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

	/* 조도 파워 (인덱스)와 Frame Rate 그리고 Scroll Rate 등등 값으로 스테이지 속도와 광량 계산 */
	if (!CalcSpeedEnergy(pstRecipe))
	{
		delete pstRecipe;
		return FALSE;
	}

	/* 메모리에 분석된 레시피 데이터 등록 */
	m_lstRecipe.AddTail(pstRecipe);

	return TRUE;
}


/*
 desc : Recipe 기본 정보 저장
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

	/* Recipe File 설정 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\data\\recipe\\%s.dat",
			   m_tzWorkDir, GetConfig()->file_dat.gerb_recipe);
	/* 파일 열기 */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "wt");
	if (0 != eRet)	return TRUE;

	fputs("; recipe_name,gerber_path,gerber_name,led_power_name,lot_date,text_string,serial_decode,"
		  "scale_decode,serial_flip_h,v,scale_flip_h,v,text_flip_h,v,global_mark_dist_1,2,3,4,5,6,"
		  "material_thick,cali_thick,step_size,led_duty_cycle,frame_rate,score_accept,scale_range,align_type,"
		  "mark_model,align_method,mark_count,mark_area_w,h\n", fp);

	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		/* 값 가져오기 */
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
	/* 파일 핸들 닫기 */
	fclose(fp);

	return TRUE;
}

/*
 desc : 현재 선택된 레시피의 속성 정보를 통해 스테이지 이동 속도 및 광량 값 계산
 parm : recipe	- [out] 스테이지 이동 속도 및 광량 개별 및 평균 값 반환 (mm/sec)
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

	/* 최대 노광 속도 */
	dbMaxSpeed		= m_pstLuriaMem->machine.scroll_rate *
					  (GetConfig()->luria_svc.pixel_size /*+ MIRROR_PERIOD_UM*/)/1000.0f * recipe->step_size;

	/* Power Name에 따른 Watt 값 얻기 */
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
	/* 5개 광학계의 Power (Watt) 합 */
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
		/* Photohead 별로 노광량 계산 */
		recipe->expo_energy[i]	= (FLOAT)(dbPowerCm2 * dbTimeOnePoint * 1000.0f);
		dbSumEnergy	+= recipe->expo_energy[i];
	}
	/* 전체 광학계의 평균 노광량 계산 */
	recipe->expo_energy_avg	= (FLOAT)ROUNDED(dbSumEnergy / GetConfig()->luria_svc.ph_count, 3);

	return TRUE;
}

/*
 desc : 레시피 기본 존재 여부
 parm : recipe	- [in]  검색하고자 하는 레시피 이름
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
 desc : 레시피 이름 검색를 통한 삭제 - 기본 정보 삭제
 parm : recipe	- [in]  삭제하고자 하는 레시피 이름
 retn : TRUE (Deleted) or FALSE (Not delete)
*/
BOOL CRecipePodis::RemoveRecipe(PCHAR recipe)
{
	POSITION pPos	= NULL, pPrePos;
	LPG_RBGF pstData= NULL;

	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		/* 방금 전에 찾은 위치 저장 */
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
	/* 기존 선택된 레시피 해제 */
	ResetSelectRecipe();

	return FALSE;
}

/*
 desc : 레시피 이름을 통한 레시피 기본 정보 반환
 parm : recipe	- [in]  검색하고자 하는 레시피 이름
 retn : 반환될 레시피가 저장된 구조체 포인터
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
 desc : 레시피 이름 (전체 경로 포함)을 통한 레시피 반환
 parm : path_name	- [in]  검색하고자 하는 거버 이름 (경로 포함)
 retn : 반환될 레시피가 저장된 구조체 포인터
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
 desc : 레시피 기본 정보가 저장된 인덱스 위치에 있는 레시피 반환
 parm : index	- [in]  가져오고자 하는 레시피 인덱스 (Zero-based)
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
LPG_RBGF CRecipePodis::GetRecipeIndex(INT32 index)
{
	POSITION pPos	= NULL;

	/* 잘못된 Index 값이 들어왔는지 여부 */
	if (index < 0)	return NULL;
	/* 해당 위치 값 얻기 */
	pPos	= m_lstRecipe.FindIndex(index);
	if (!pPos)	return NULL;

	/* 위치에 해당되는 결과 값 반환 */
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
 desc : 레시피 기본 이름을 통한 레시피 기본 선택
 parm : recipe	- [in]  검색하고자 하는 레시피 기본 이름
 retn : TRUE (선택 성공) or FALSE (선택 실패)
*/
BOOL CRecipePodis::SelRecipeOnlyName(PCHAR recipe)
{
	BOOL bSucc			= FALSE;
	POSITION pPos		= NULL;
	LPG_RBGF pstRecipe	= NULL;

	/* 레시피 기본 정보 검색 및 선택 */
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

	/* 레시피에 기본과 연결되는 레시피 속성 정보가 없다면... */
	if (!bSucc)	m_pstRecipe	= NULL;

	return bSucc;
}

/*
 desc : 레시피 기본 이름 (전체 경로 포함)을 통한 레시피 기본 선택
 parm : recipe	- [in]  검색하고자 하는 레시피 기본 이름 (전체 경로 포함)
 retn : TRUE (선택 성공) or FALSE (선택 실패)
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

	/* 레시피에 기본과 연결되는 레시피 속성 정보가 없다면... */
	if (!bSucc)	m_pstRecipe	= NULL;

	return FALSE;
}

/*
 desc : Recipe 기본 정보 간의 구조체 값 복사 (구조체 내부에 할당된 메모리가 항목이 많으므로)
 parm : source	- [in]  원래 값이 저장된 곳
		target	- [in]  새로운 값이 저장(복사)될 곳
 retn : TRUE or FALSE
*/
BOOL CRecipePodis::CopyRecipe(LPG_RBGF source, LPG_RBGF target)
{
	UINT8 i	= 0x00, j = 0;

	/* 기존 메모리 버퍼 초기화 */
	target->ResetMemData();

	/* Source Energy 계산 */
	if (!CalcSpeedEnergy(source))	return FALSE;

	/* 기본 구조체 멤버 변수 복사 */
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
 desc : Recipe 기본 정보 추가 (Append)
 parm : recipe	- [in]  Recipe 정보가 저장된 구조체 포인터
		check	- [in]  값 유효성 체크 여부
 retn : TRUE or FALSE (개수 초과. 최대 15개)
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

	/* 필수로 입력되는 값이 있는지 검토 */
	if (!recipe->IsValid())
	{
		AfxMessageBox(L"The input data (recipe base info) is invalid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 기존에 등록된 레시피가 있는지 확인 */
	if (RecipeFind(recipe->recipe_name))
	{
		AfxMessageBox(L"The same recipe (Base Info.) exists", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 맨 마지막 위치에 등록 */
	pstRecipe	= new STG_RBGF;
	ASSERT(pstRecipe);
	pstRecipe->Init();	/* 구조체 내에 내부 메모리 생성 */

	/* 새로 생성된 구조체 영역에 기존 설정된 값 복사 */
	bSucc	= CopyRecipe(recipe, pstRecipe);
	/* 등록 */
	if (bSucc)	m_lstRecipe.AddTail(pstRecipe);

	/* 현재까지 등록된 모든 Recipe 내용을 파일로 저장 */
	return bSucc && SaveFile();
}

/*
 desc : Recipe 기본 정보 수정
 parm : recipe	- [in]  Recipe 기본 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
BOOL CRecipePodis::RecipeModify(LPG_RBGF recipe)
{
	LPG_RBGF pstRecipe	= NULL;

	/* 필수로 입력되는 값이 있는지 검토 */
	if (!recipe->IsValid())
	{
		AfxMessageBox(L"The update data (Base Info.) is invalid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 기존에 등록된 레시피가 있는지 확인 */
	pstRecipe = GetRecipeOnlyName(recipe->recipe_name);
	if (!pstRecipe)
	{
		AfxMessageBox(L"The target recipe (Base Info.) was not found", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 새로 생성된 구조체 영역에 기존 설정된 값 복사 */
	CopyRecipe(recipe, pstRecipe);

	/* 현재까지 등록된 모든 Recipe 내용을 파일로 저장 */
	return SaveFile();
}

/*
 desc : Recipe 기본 삭제
 parm : recipe	- [in]  제거하려는 레시피의 기본 이름
 retn : TRUE or FALSE
*/
BOOL CRecipePodis::RecipeDelete(PCHAR recipe)
{
	/* 기존에 등록된 레시피가 있는지 확인 */
	if (!GetRecipeOnlyName(recipe))
	{
		AfxMessageBox(L"The deleted recipe_name was not found", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* Recipe 삭제 수행 */
	RemoveRecipe(recipe);

	/* 현재까지 등록된 모든 Recipe 내용을 파일로 저장 */
	return SaveFile();
}

/*
 desc : 현재 선택된 Recipe의 Mark 구성 방식이 Shared Type인지 여부
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
 desc : 현재 선택된 Recipe의 Mark 구성 방식이 Local Mark가 존재하는지 여부
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
 desc : 현재 선택된 레시피에 포함된 전체 마크 개수
 parm : None
 retn : 개수 반환
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
 desc : 가져오려는 위치의 Mark Index 값이 Camera 몇 번이고 몇 번째 이미지 정보인지 반환
 parm : mark	- [in]  Mark Index (Zero based)
		cam_id	- [out] Camera Number (1 or 2)
		img_id	- [out] Camera Grabbed Image Number (Zero based)
 retn : TRUE or FALSE
*/
BOOL CRecipePodis::GetLocalMarkToGrabNum(UINT8 mark, UINT8 &cam_id, UINT8 &img_id)
{
	UINT8 u8Count	= GetSelectRecipeLocalMarkCount();
	if (!m_pstRecipe || u8Count < 1)	return FALSE;

	/* 값 초기화 (반드시 Max 값으로 초기화) */
	cam_id	= 0xff;
	img_id	= 0xff;

	switch (m_pstRecipe->align_type)
	{
	case ENG_ATGL::en_global_4_local_2x2_n_point	:
	case ENG_ATGL::en_global_4_local_3x2_n_point	:
	case ENG_ATGL::en_global_4_local_4x2_n_point	:
	case ENG_ATGL::en_global_4_local_5x2_n_point	:
		cam_id	= (mark < (u8Count / 2)) ? 0x01 : 0x02;;
		img_id	= (UINT8)ROUNDDN((mark % (u8Count / 2)), 0) + 2 /* Global Mark Number 0, 1이 있기 때문에 */;
		break;

	case ENG_ATGL::en_global_4_local_2x2_s_point	:
	case ENG_ATGL::en_global_4_local_3x2_s_point	:
	case ENG_ATGL::en_global_4_local_4x2_s_point	:
	case ENG_ATGL::en_global_4_local_5x2_s_point	:
		if (mark < (u8Count / 3))
		{
			cam_id	= 0x01;
			img_id	= mark + 2 /* Global Mark Number 0, 1이 있기 때문에 */;
		}
		else
		{
			cam_id	= 0x02;
			img_id	= (mark - ((m_pstRecipe->align_type & 0x0f) + 2)) + 2 /* Global Mark Number 0, 1이 있기 때문에 */;
		}
		break;

	default	:	return FALSE;
	}

	return TRUE;
}

/*
 desc : 현재 Local Mark Index 값 값에 해당된 Scan 번호 값 반환
 parm : mark_id	- [in]  Local Mark Index 값 (0 or Later)
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
 desc : 현재 Camera Index와 Grabbed Image를 가지고 몇 번째 인덱스에 저장되어 있는지 반환
 parm : cam_id	- [in]  Camera Number (1 or 2)
		img_id	- [in]  Camera Grabbed Image Number (Zero based)
		mark	- [out] Mark Index (Zero based) 반환
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
 desc : 현재 카메라마다 Grabbed Image 번호에 해당되는 방향 (스테이지 이동 방향) 정보 반환
 parm : img_id	- [in]  Camera Grabbed Image Number (Zero based)
 retn : TRUE (Normal : 정방향; 전진) or FALSE (역방향; 후진)
*/
BOOL CRecipePodis::GetImageToStageDirect(UINT8 img_id)
{
	if (!m_pstRecipe || img_id < 2)	return TRUE;	/* Global Mark인 경우 무조건 정방향 */
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
 desc : 얼라인 마크 검색을 위해 1 Scan 하게 되면, 검색되는 마크의 개수 즉,
		1 Scan 당 등록된 마크의 개수 (현재 선택된 레시피(거버)에 한함)
 parm : type	- [in]  마크 구성 형식
 retn : 1 Scan 당 등록된 마크의 개수 반환 최대 10 개 (실패. 0x00)
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