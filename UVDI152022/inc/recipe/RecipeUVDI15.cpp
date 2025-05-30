
/*
 desc : Gerber Recipe Data 적재 및 관리
*/

#include "pch.h"
#include "RecipeUVDI15.h"	/* job, expo Recipe */


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*v
 desc : 생성자
 parm : work_dir	- [in]  실행 파일이 실행되는 경로 (전체 경로, '\' 제외)
		led_power	- [in]  Led Power Recipe 저장 객체 포인터
		mem_luria	- [in]  Luria 공유 메모리 구조체 포인터
 retn : None
*/
CRecipeUVDI15::CRecipeUVDI15(PTCHAR work_dir, CLedPower* led_power, LPG_LDSM mem_luria)
	: CBase(work_dir)
{
	m_pstJobRecipe = NULL;
	m_pstExpoRecipe = NULL;
	m_pLedPower = led_power;
	m_pstLuriaMem = mem_luria;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CRecipeUVDI15::~CRecipeUVDI15()
{
	RemoveAllJob();
	RemoveAllExpo();
}

/*
 desc : 기존 선택된 레시피 초기화
 parm : None
 retn : None
*/
VOID CRecipeUVDI15::ResetSelectJobRecipe()
{
	m_pstJobRecipe = NULL;
}
/*
 desc : 기존 선택된 레시피 초기화
 parm : None
 retn : None
*/
VOID CRecipeUVDI15::ResetSelectExpoRecipe()
{
	m_pstExpoRecipe = NULL;
}


/*
 desc : 현재 선택된 레시피 기본 반환
 parm : None
 retn : 레시피가 저장된 구조체 포인터
*/
LPG_RJAF CRecipeUVDI15::GetSelectJobRecipe()
{
	return m_pstJobRecipe;
}

/*
 desc : 현재 선택된 레시피 기본 반환
 parm : None
 retn : 레시피가 저장된 구조체 포인터
*/
LPG_REAF CRecipeUVDI15::GetSelectExpoRecipe()
{
	return m_pstExpoRecipe;
}

/*
 desc : 기존 등록된 Job 레시피 모두 제거
 parm : None
 retn : None
*/
VOID CRecipeUVDI15::RemoveAllJob()
{
	POSITION pPos	= NULL;
	LPG_RJAF pRecipe= NULL;

	/* 기존 등록된 Recipe Data 메모리 해제 */
	pPos	= m_lstJobRecipe.GetHeadPosition();
	while (pPos)
	{
		pRecipe	= m_lstJobRecipe.GetNext(pPos);
		if (pRecipe)
		{
			pRecipe->Close();	/* 메모리 해제 */
			delete pRecipe;
		}
	}
	/* 리스트 비움 */
	m_lstJobRecipe.RemoveAll();
	/* 기존 선택된 레시피 초기화 */
	m_pstJobRecipe = NULL;
}

/*
 desc : 기존 등록된 Expo 레시피 모두 제거
 parm : None
 retn : None
*/
VOID CRecipeUVDI15::RemoveAllExpo()
{
	POSITION pPos = NULL;
	LPG_REAF pRecipe = NULL;

	/* 기존 등록된 Recipe Data 메모리 해제 */
	pPos = m_lstExpoRecipe.GetHeadPosition();
	while (pPos)
	{
		pRecipe = m_lstExpoRecipe.GetNext(pPos);
		if (pRecipe)
		{
			pRecipe->Close();	/* 메모리 해제 */
			delete pRecipe;
		}
	}
	/* 리스트 비움 */
	m_lstExpoRecipe.RemoveAll();
	/* 기존 선택된 레시피 초기화 */
	m_pstExpoRecipe = NULL;
}

/*
 desc : Calibration Recipe 적재
 parm : None
 retn : TRUE or FALSE
*/
BOOL CRecipeUVDI15::LoadFile()
{
	CHAR szData[1024]			= {NULL};
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	errno_t eRet	= 0;
	FILE *fp		= NULL;
	CUniToChar csCnv;

	/* 기존 등록된 레시피 제거 */
	RemoveAllJob();
	RemoveAllExpo();


	/* ----------------------------------------------------------------------------------------- */
	/*                                     job recipe File 적재                                  */
	/* ----------------------------------------------------------------------------------------- */

	/* Job Recipe Name 정보 얻기 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\recipe\\%s.dat",
			   m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.job_recipe);
	/* 파일 열기 */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "rt");
	//if (0 != eRet)	return TRUE;	/* Recipe File이 없어도 TRUE 반환 */
	if (0 != eRet)
	{

	}
	else
	{
		while (!feof(fp))
		{
			/* 한줄 읽어오기 */
			memset(szData, 0x00, 1024);
			fgets(szData, 1024, fp);
			/* 문자열 길이가 특정 개수 이하인 경우인지 (만약 첫 글자가 주석인 세미콜론(;)이면 Skip) */
			if (strlen(szData) > 0 && ';' != szData[0] && '\n' != szData[0])
			{
				/* 분석 및 등록하기 */
				ParseJobRecipe(szData, (UINT32)strlen(szData));
			}
		}
		/* 파일 핸들 닫기 */
		fclose(fp);

		if (FALSE == SelJobRecipeOnlyName(csCnv.Uni2Ansi(GetConfig()->set_uvdi15.recipe_name)))
		{
			if (m_lstJobRecipe.GetCount())
			{
				SelJobRecipeOnlyName(m_lstJobRecipe.GetHead()->job_name);
			}
		}
	}


	/* ----------------------------------------------------------------------------------------- */
	/*                                     expo recipe File 적재                                  */
	/* ----------------------------------------------------------------------------------------- */

	/* Expo Recipe Name 정보 얻기 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\recipe\\%s.dat",
			   m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.expo_recipe);
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
			ParseExpoRecipe(szData, (UINT32)strlen(szData));
		}
	}
	/* 파일 핸들 닫기 */
	fclose(fp);

	return TRUE;
}

/*
 desc : Job Recipe 기본 분석 및 등록 진행
 parm : data	- [in]  Recipe 정보가 저장된 문자열 버퍼
		size	- [in]  'data' 버퍼에 저장된 데이터의 길이
 retn : TRUE or FALSE
*/
BOOL CRecipeUVDI15::ParseJobRecipe(PCHAR data, UINT32 size)
{

	UINT8 u8Div = 8; //최하 레시피 아이템 8개임. 더 늘어나는건 괜찮지만 이것보다 적으면 안됨 

	UINT32 u32Find = 0;
	UINT32 i;
	BOOL bIsLoop = TRUE;
	CHAR* pData = data, * pFind, szValue[1024];
	LPG_RJAF pstRecipe = NULL;

	/* 일단, 주어진 문자열 중에서 콤마(',') 개수가 xxx 개인지 확인 */
	for (i = 0; i < size; i++)
	{
		if (',' == data[i])	u32Find++;
	}
	if (u32Find < u8Div)
	{
		AfxMessageBox(L"Failed to analyse the value from <job recipe base> file", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}

	/* 레시피 객체 임시 생성 */
	pstRecipe = new STG_RJAF();
	ASSERT(pstRecipe);

	/* 메모리 할당 및 초기화 */
	pstRecipe->Init();
	pstRecipe->ResetMemData();

	/* job recipe name */
	pFind = strchr(pData, ',');
	if (pFind) { memcpy(pstRecipe->job_name, pData, pFind - pData);	pData = ++pFind; }
	/* gerber path */
	pFind = strchr(pData, ',');
	if (pFind) { memcpy(pstRecipe->gerber_path, pData, pFind - pData);	pData = ++pFind; }
	/* gerber name */
	pFind = strchr(pData, ',');
	if (pFind) { memcpy(pstRecipe->gerber_name, pData, pFind - pData);	pData = ++pFind; }

	pFind = strchr(pData, ',');
	if (pFind) {
		memset(szValue, 0x00, _countof(szValue));
		memcpy(szValue, pData, pFind - pData);	pData = ++pFind;
		pstRecipe->material_thick = (UINT32)atoi(szValue);
	}

	if (u32Find == u8Div)
	{
		pstRecipe->ldsThreshold = uvEng_GetConfig()->measure_flat.dLimitZPOS * 1000.0f;
	}
	else
	{
		pFind = strchr(pData, ',');
		if (pFind)
		{
			memset(szValue, 0x00, _countof(szValue));
			memcpy(szValue, pData, pFind - pData);	pData = ++pFind;
			pstRecipe->ldsThreshold = (UINT32)atoi(szValue);
		}
		
	}

	pFind = strchr(pData, ',');
	if (pFind) {
		memset(szValue, 0x00, _countof(szValue));
		memcpy(szValue, pData, pFind - pData);	pData = ++pFind;
		pstRecipe->expo_energy = (FLOAT)atof(szValue);
	}

	pFind = strchr(pData, ',');
	if (pFind) {
		memset(szValue, 0x00, _countof(szValue));
		memcpy(szValue, pData, pFind - pData);	pData = ++pFind;
		pstRecipe->expo_speed = (FLOAT)atof(szValue);
	}

	/* align recipe */
	pFind = strchr(pData, ',');
	if (pFind) { memcpy(pstRecipe->align_recipe, pData, pFind - pData);	pData = ++pFind; }
	/* expo recipe */
	pFind = strchr(pData, ',');
	if (pFind) { memcpy(pstRecipe->expo_recipe, pData, pFind - pData);	pData = ++pFind; }

	/* 조도 파워 (인덱스)와 Frame Rate 그리고 Scroll Rate 등등 값으로 스테이지 속도와 광량 계산 */
	if (!CalcSpeedEnergy(pstRecipe))
	{
		delete pstRecipe;
		return FALSE;
	}

	/* 메모리에 분석된 레시피 데이터 등록 */
	m_lstJobRecipe.AddTail(pstRecipe);

	return TRUE;
}


/*
 desc : Recipe 기본 분석 및 등록 진행
 parm : data	- [in]  Recipe 정보가 저장된 문자열 버퍼
		size	- [in]  'data' 버퍼에 저장된 데이터의 길이
 retn : TRUE or FALSE
*/
BOOL CRecipeUVDI15::ParseExpoRecipe(PCHAR data, UINT32 size)
{
	UINT8 u8Div = 24;
	UINT32 u32Find = 0, i;
	BOOL bIsLoop = TRUE;
	CHAR* pData = data, * pFind, szValue[1024];
	LPG_REAF pstRecipe = NULL;

	/* 일단, 주어진 문자열 중에서 콤마(',') 개수가 xxx 개인지 확인 */
	for (i = 0; i < size; i++)
	{
		if (',' == data[i])	u32Find++;
	}
	if (u32Find != u8Div)
	{
		AfxMessageBox(L"Failed to analyse the value from <recipe base> file", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}

	/* 레시피 객체 임시 생성 */
	pstRecipe = new STG_REAF;
	ASSERT(pstRecipe);

	/* 메모리 할당 및 초기화 */
	pstRecipe->Init();
	pstRecipe->ResetMemData();

	/* recipe name */
	pFind = strchr(pData, ',');
	if (pFind) { memcpy(pstRecipe->expo_name, pData, pFind - pData);	pData = ++pFind; }
	/* led power name */
	pFind = strchr(pData, ',');
	if (pFind) { memcpy(pstRecipe->power_name, pData, pFind - pData);	pData = ++pFind; }

	/* mark distance diff */
	for (i = 0; i < MAX_GLOBAL_MARK_DIST_CHECK_COUNT; i++)
	{
		pFind = strchr(pData, ',');
		memset(szValue, 0x00, 1024);
		memcpy(szValue, pData, pFind - pData);
		pData = ++pFind;
		pstRecipe->global_mark_dist[i] = (UINT32)atoi(szValue);
	}

	/* 반드시 0 으로 초기화 */
	for (i = 0x00; i < 0x05; i++)
	{
		switch (i)
		{
		case 0x00: case 0x01: case 0x02: case 0x03: case 0x04:
			pFind = strchr(pData, ',');
			memset(szValue, 0x00, 1024);
			memcpy(szValue, pData, pFind - pData);
			pData = ++pFind;
			switch (i)
			{
			case 0x00: pstRecipe->led_duty_cycle = (UINT8)atoi(szValue);		break;
			case 0x01: pstRecipe->mark_score_accept = (DOUBLE)atof(szValue);	break;
			case 0x02: pstRecipe->mark_scale_range = (DOUBLE)atof(szValue);
				if (pstRecipe->mark_scale_range < 50.0f)
					pstRecipe->mark_scale_range = 50.0f;				break;
			case 0x03: pstRecipe->real_scale_range		= (DOUBLE)atof(szValue);	break;
			case 0x04: pstRecipe->real_rotaion_range	= (DOUBLE)atof(szValue);	break;
			}
			break;
		}
	}

	/* serial/scale/text decode */
	for (i = 0; i < 3; i++)
	{
		pFind = strchr(pData, ',');
		if (pFind)
		{
			memset(szValue, 0x00, 1024);
			memcpy(szValue, pData, pFind - pData);
			switch (i)
			{
			case 0x00: pstRecipe->dcode_serial = (UINT16)atoi(szValue);
			case 0x01: pstRecipe->dcode_scale = (UINT16)atoi(szValue);
			case 0x02: pstRecipe->dcode_text = (UINT16)atoi(szValue);
			}
			pData = ++pFind;
		}
	}
	/* text string */
	pFind = strchr(pData, ',');
	if (pFind) { memcpy(pstRecipe->text_string, pData, pFind - pData);	pData = ++pFind; }

	/* flip series*/
	for (i = 0; i < 6; i++)
	{
		pFind = strchr(pData, ',');
		memset(szValue, 0x00, 1024);
		memcpy(szValue, pData, pFind - pData);
		switch (i)
		{
		case 0x00:	pstRecipe->serial_flip_h = (UINT8)atoi(szValue);	break;
		case 0x01:	pstRecipe->serial_flip_v = (UINT8)atoi(szValue);	break;
		case 0x02:	pstRecipe->scale_flip_h = (UINT8)atoi(szValue);	break;
		case 0x03:	pstRecipe->scale_flip_v = (UINT8)atoi(szValue);	break;
		case 0x04:	pstRecipe->text_flip_h = (UINT8)atoi(szValue);	break;
		case 0x05:	pstRecipe->text_flip_v = (UINT8)atoi(szValue);	break;
		}
		pData = ++pFind;
	}

	pFind = strchr(pData, ',');
	if (pFind)
	{
		memset(szValue, 0x00, 1024);
		memcpy(szValue, pData, pFind - pData);
		pData = ++pFind;
		pstRecipe->headOffset = (UINT8)atoi(szValue);
	}

	/* 메모리에 분석된 레시피 데이터 등록 */
	m_lstExpoRecipe.AddTail(pstRecipe);

	return TRUE;
}

/*
 desc : Job Recipe 기본 정보 저장
 parm : None
 retn : TRUE or FALSE
*/
BOOL CRecipeUVDI15::SaveJobFile()
{
	CHAR szData[MAX_PATH_LEN] = { NULL };
	TCHAR tzFile[MAX_PATH_LEN] = { NULL };
	errno_t eRet = 0;
	FILE* fp = NULL;
	POSITION pPos = NULL;
	LPG_RJAF pstRecipe = NULL;
	CUniToChar csCnv;

	/* Recipe File 설정 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\recipe\\%s.dat",
		m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.job_recipe);
	/* 파일 열기 */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "wt");
	if (0 != eRet)	return TRUE;

	// 	fputs("; recipe_name,gerber_path,gerber_name,led_power_name,lot_date,text_string,serial_decode,"
	// 		  "scale_decode,serial_flip_h,v,scale_flip_h,v,text_flip_h,v,global_mark_dist_1,2,3,4,5,6,"
	// 		  "material_thick,cali_thick,step_size,led_duty_cycle,frame_rate,score_accept,scale_range,align_type,"
	// 		  "mark_model,align_method,mark_count,mark_area_w,h\n", fp);
	fputs("; job_name,gerber_path,gerber_name,material_thick,expo_energy,expo_speed,align_recipe,expo_recipe\n", fp);

	pPos = m_lstJobRecipe.GetHeadPosition();
	while (pPos)
	{
		/* 값 가져오기 */
		pstRecipe = (LPG_RJAF)m_lstJobRecipe.GetNext(pPos);
		/* recipe name*/
		sprintf_s(szData, MAX_PATH_LEN, "%s,", pstRecipe->job_name);
		fputs(szData, fp);
		/* gerber path */
		sprintf_s(szData, MAX_PATH_LEN, "%s,", pstRecipe->gerber_path);
		fputs(szData, fp);
		/* gerber_name */
		sprintf_s(szData, MAX_PATH_LEN, "%s,", pstRecipe->gerber_name);
		fputs(szData, fp);

		/* material_thick, calibration_thick */
		sprintf_s(szData, MAX_PATH_LEN, "%u,%u,%.4f,%.4f,"
			, pstRecipe->material_thick
			, pstRecipe->ldsThreshold
			
			, pstRecipe->expo_energy
			, pstRecipe->expo_speed);
		fputs(szData, fp);



		/* align recipe */
		sprintf_s(szData, MAX_PATH_LEN, "%s,", pstRecipe->align_recipe);
		fputs(szData, fp);
		/* expo recipe */
		sprintf_s(szData, MAX_PATH_LEN, "%s,", pstRecipe->expo_recipe);
		fputs(szData, fp);

		fputs("\n", fp);
	}
	/* 파일 핸들 닫기 */
	fclose(fp);

	return TRUE;
}

/*
 desc : Recipe 기본 정보 저장
 parm : None
 retn : TRUE or FALSE
*/
BOOL CRecipeUVDI15::SaveExpoFile()
{
	UINT8 i;
	CHAR szData[MAX_PATH_LEN] = { NULL };
	TCHAR tzFile[MAX_PATH_LEN] = { NULL };
	errno_t eRet = 0;
	FILE* fp = NULL;
	POSITION pPos = NULL;
	LPG_REAF pstRecipe = NULL;
	CUniToChar csCnv;

	/* Recipe File 설정 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\recipe\\%s.dat",
		m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.expo_recipe);
	/* 파일 열기 */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "wt");
	if (0 != eRet)	return TRUE;

// 	fputs("; recipe_name,gerber_path,gerber_name,led_power_name,lot_date,text_string,serial_decode,"
// 		"scale_decode,serial_flip_h,v,scale_flip_h,v,text_flip_h,v,global_mark_dist_1,2,3,4,5,6,"
// 		"material_thick,cali_thick,step_size,led_duty_cycle,frame_rate,score_accept,scale_range,align_type,"
// 		"mark_model,align_method,mark_count,mark_area_w,h\n", fp);
	fputs("; expo_name,power_name,global_mark_dist_1,2,3,4,5,6,led_duty_cycle,score_accept,scale_range,real_scale_range,real_rotaion_range,"
		"serial_decode,scale_decode,text_decode,text_string,serial_flip_h,v,scale_flip_h,v,text_flip_h,v,\n", fp);

	pPos = m_lstExpoRecipe.GetHeadPosition();
	while (pPos)
	{
		/* 값 가져오기 */
		pstRecipe = (LPG_REAF)m_lstExpoRecipe.GetNext(pPos);
		/* recipe name*/
		sprintf_s(szData, MAX_PATH_LEN, "%s,", pstRecipe->expo_name);
		fputs(szData, fp);
		/* power name */
		sprintf_s(szData, MAX_PATH_LEN, "%s,", pstRecipe->power_name);
		fputs(szData, fp);

		/* Mark Distance Difference */
		for (i = 0; i < MAX_GLOBAL_MARK_DIST_CHECK_COUNT; i++)
		{
			sprintf_s(szData, MAX_PATH_LEN, "%u,", pstRecipe->global_mark_dist[i]);	fputs(szData, fp);
		}

		sprintf_s(szData, MAX_PATH_LEN, "%d,%.3f,%.3f,%.3f,%.3f,",
			pstRecipe->led_duty_cycle, pstRecipe->mark_score_accept, pstRecipe->mark_scale_range, pstRecipe->real_scale_range, pstRecipe->real_rotaion_range);
		fputs(szData, fp);

		/*  */
		sprintf_s(szData, MAX_PATH_LEN, "%u,%u,%u,",
			pstRecipe->dcode_serial, pstRecipe->dcode_scale, pstRecipe->dcode_text);
		fputs(szData, fp);

		/* text string */
		sprintf_s(szData, MAX_PATH_LEN, "%s,", pstRecipe->text_string);
		fputs(szData, fp);

		/*  */
		sprintf_s(szData, MAX_PATH_LEN, "%d,%d,%d,%d,%d,%d,%d,",
			pstRecipe->serial_flip_h, pstRecipe->serial_flip_v, pstRecipe->scale_flip_h,
			pstRecipe->scale_flip_v, pstRecipe->text_flip_h, pstRecipe->text_flip_v, pstRecipe->headOffset);
		fputs(szData, fp);

		fputs("\n", fp);
	}

	/* 파일 핸들 닫기 */
	fclose(fp);

	return TRUE;
}

/*
 desc : Job 레시피 이름 검색를 통한 삭제 - 기본 정보 삭제
 parm : recipe	- [in]  삭제하고자 하는 레시피 이름
 retn : TRUE (Deleted) or FALSE (Not delete)
*/
BOOL CRecipeUVDI15::RemoveJobRecipe(PCHAR recipe)
{
	POSITION pPos	= NULL, pPrePos;
	LPG_RJAF pstData= NULL;

	pPos	= m_lstJobRecipe.GetHeadPosition();
	while (pPos)
	{
		/* 방금 전에 찾은 위치 저장 */
		pPrePos	= pPos;
		pstData	= m_lstJobRecipe.GetNext(pPos);
		if (0 == strcmp(recipe, pstData->job_name))
		{
			pstData->Close();
			delete pstData;
			m_lstJobRecipe.RemoveAt(pPrePos);
			return TRUE;
		}
	}
	/* 기존 선택된 레시피 해제 */
	ResetSelectJobRecipe();

	return FALSE;
}

/*
 desc : 레시피 이름 검색를 통한 삭제 - 기본 정보 삭제
 parm : recipe	- [in]  삭제하고자 하는 레시피 이름
 retn : TRUE (Deleted) or FALSE (Not delete)
*/
BOOL CRecipeUVDI15::RemoveExpoRecipe(PCHAR recipe)
{
	POSITION pPos = NULL, pPrePos;
	LPG_REAF pstData = NULL;

	pPos = m_lstExpoRecipe.GetHeadPosition();
	while (pPos)
	{
		/* 방금 전에 찾은 위치 저장 */
		pPrePos = pPos;
		pstData = m_lstExpoRecipe.GetNext(pPos);
		if (0 == strcmp(recipe, pstData->expo_name))
		{
			pstData->Close();
			delete pstData;
			m_lstExpoRecipe.RemoveAt(pPrePos);
			return TRUE;
		}
	}
	/* 기존 선택된 레시피 해제 */
	ResetSelectExpoRecipe();

	return FALSE;
}

/*
 desc : 레시피 기본 이름을 통한 레시피 기본 선택
 parm : recipe	- [in]  검색하고자 하는 레시피 기본 이름
 retn : TRUE (선택 성공) or FALSE (선택 실패)
*/
BOOL CRecipeUVDI15::SelJobRecipeOnlyName(PCHAR recipe)
{
	BOOL bSucc			= FALSE;
	POSITION pPos		= NULL;
	LPG_RJAF pstRecipe	= NULL;

	/* Job 레시피 기본 정보 검색 및 선택 */
	pPos	= m_lstJobRecipe.GetHeadPosition();
	while (pPos)
	{
		pstRecipe	= m_lstJobRecipe.GetNext(pPos);
		if (0 == strcmp(recipe, pstRecipe->job_name))
		{
			m_pstJobRecipe = pstRecipe;
			bSucc	= TRUE;
			break;
		}
	}

	/* 레시피에 기본과 연결되는 레시피 속성 정보가 없다면... */
	if (!bSucc)
	{
		m_pstJobRecipe = NULL;
		return FALSE;
	}

	// by sysandj : 선택된 레시피 저장
	INT32 i32Len = (INT32)strlen(recipe);
	if (i32Len >= MAX_FILE_LEN)	i32Len = MAX_FILE_LEN - 1;
	CUniToChar csCnv;
	wcscpy_s(GetConfig()->set_uvdi15.recipe_name, MAX_FILE_LEN, csCnv.Ansi2Uni(recipe));

	/* 환경 파일 객체 생성 */
	CConfUvdi15* pConfUvdi15 = new CConfUvdi15(GetConfig());
	ASSERT(pConfUvdi15);
	/* 환경 파일 저장 */
	bSucc = pConfUvdi15->SaveConfigUvDI15Common();
	/* 메모리 해제 */
	delete pConfUvdi15;
	// by sysandj : 선택된 레시피 저장

	return bSucc;
}

/*
 desc : 레시피 기본 이름을 통한 레시피 기본 선택
 parm : recipe	- [in]  검색하고자 하는 레시피 기본 이름
 retn : TRUE (선택 성공) or FALSE (선택 실패)
*/
BOOL CRecipeUVDI15::SelExpoRecipeOnlyName(PCHAR recipe)
{
	BOOL bSucc = FALSE;
	POSITION pPos = NULL;
	LPG_REAF pstRecipe = NULL;

	/* 레시피 기본 정보 검색 및 선택 */
	pPos = m_lstExpoRecipe.GetHeadPosition();
	while (pPos)
	{
		pstRecipe = m_lstExpoRecipe.GetNext(pPos);
		if (0 == strcmp(recipe, pstRecipe->expo_name))
		{
			m_pstExpoRecipe = pstRecipe;
			bSucc = TRUE;
			break;
		}
	}

	/* 레시피에 기본과 연결되는 레시피 속성 정보가 없다면... */
	if (!bSucc)	m_pstExpoRecipe = NULL;

	return bSucc;
}


/*
 desc : 레시피 기본 이름 (전체 경로 포함)을 통한 레시피 기본 선택
 parm : recipe	- [in]  검색하고자 하는 레시피 기본 이름 (전체 경로 포함)
 retn : TRUE (선택 성공) or FALSE (선택 실패)
*/
BOOL CRecipeUVDI15::SelJobRecipePathName(PCHAR recipe)
{
	CHAR szGerberPath[MAX_GERBER_NAME];
	BOOL bSucc = FALSE;
	POSITION pPos = NULL;
	LPG_RJAF pstRecipe = NULL;

	pPos = m_lstJobRecipe.GetHeadPosition();
	while (pPos && !bSucc)
	{
		pstRecipe = m_lstJobRecipe.GetNext(pPos);
		memset(szGerberPath, 0x00, MAX_GERBER_NAME);
		sprintf_s(szGerberPath, MAX_GERBER_NAME, "%s\\%s",
			pstRecipe->gerber_path, pstRecipe->gerber_name);
		if (0 == strcmp(recipe, szGerberPath))
		{
			m_pstJobRecipe = pstRecipe;
			bSucc = TRUE;
			break;
		}
	}
	if (!bSucc)	return FALSE;

	/* 레시피에 기본과 연결되는 레시피 속성 정보가 없다면... */
	if (!bSucc)	m_pstJobRecipe = NULL;

	return FALSE;
}

/*
 desc : Recipe 기본 정보 간의 구조체 값 복사 (구조체 내부에 할당된 메모리가 항목이 많으므로)
 parm : source	- [in]  원래 값이 저장된 곳
		target	- [in]  새로운 값이 저장(복사)될 곳
 retn : TRUE or FALSE
*/
BOOL CRecipeUVDI15::CopyJobRecipe(LPG_RJAF source, LPG_RJAF target)
{
	UINT8 i = 0x00, j = 0;

	/* 기존 메모리 버퍼 초기화 */
	target->ResetMemData();

	/* Source Energy 계산 */
	//if (!CalcSpeedEnergy(source))	return FALSE;

	/* 기본 구조체 멤버 변수 복사 */
	//memcpy(target, source, sizeof(STG_RBGF) - sizeof(PUINT8) * 8);

	target->material_thick = source->material_thick;

	target->ldsThreshold = source->ldsThreshold;
	//target->ldsBaseHeight = source->ldsBaseHeight;


	target->expo_energy = source->expo_energy;
	target->expo_speed = source->expo_speed;
	target->step_size = source->step_size;
	target->frame_rate = source->frame_rate;
	
	strcpy_s(target->job_name, RECIPE_NAME_LENGTH, source->job_name);
	strcpy_s(target->gerber_path, MAX_PATH_LEN, source->gerber_path);
	strcpy_s(target->gerber_name, MAX_GERBER_NAME, source->gerber_name);
	strcpy_s(target->align_recipe, RECIPE_NAME_LENGTH, source->align_recipe);
	strcpy_s(target->expo_recipe, RECIPE_NAME_LENGTH, source->expo_recipe);


	return TRUE;
}



/*
 desc : Recipe 기본 정보 간의 구조체 값 복사 (구조체 내부에 할당된 메모리가 항목이 많으므로)
 parm : source	- [in]  원래 값이 저장된 곳
		target	- [in]  새로운 값이 저장(복사)될 곳
 retn : TRUE or FALSE
*/
BOOL CRecipeUVDI15::CopyExpoRecipe(LPG_REAF source, LPG_REAF target)
{
	UINT8 i = 0x00, j = 0;

	/* 기존 메모리 버퍼 초기화 */
	target->ResetMemData();

	/* Source Energy 계산 */
	//if (!CalcSpeedEnergy(source))	return FALSE;

	/* 기본 구조체 멤버 변수 복사 */
	//memcpy(target, source, sizeof(STG_RBGF) - sizeof(PUINT8) * 8);

	target->serial_flip_h	= source->serial_flip_h;
	target->serial_flip_v	= source->serial_flip_h;
	target->scale_flip_h	= source->scale_flip_h;
	target->scale_flip_v	= source->scale_flip_v;
	target->text_flip_h		= source->text_flip_h;
	target->text_flip_v		= source->text_flip_v;
	target->headOffset = source->headOffset;
	target->led_duty_cycle	= source->led_duty_cycle;

	target->dcode_serial	= source->dcode_serial;
	target->dcode_scale		= source->dcode_scale;
	target->dcode_text		= source->dcode_text;

	target->mark_score_accept = source->mark_score_accept;
	target->mark_scale_range = source->mark_scale_range;
	target->real_scale_range = source->real_scale_range;
	target->real_rotaion_range = source->real_rotaion_range;

	target->global_mark_dist_rate = source->global_mark_dist_rate;
	memcpy(target->global_mark_dist, source->global_mark_dist,
		sizeof(UINT32) * MAX_GLOBAL_MARK_DIST_CHECK_COUNT);

	strcpy_s(target->expo_name, RECIPE_NAME_LENGTH,		source->expo_name);
	strcpy_s(target->text_string, MAX_PANEL_TEXT_STRING, source->text_string);
	strcpy_s(target->power_name, LED_POWER_NAME_LENGTH, source->power_name);

	return TRUE;
}


/* ----------------------------------------------------------------------------------------- */
/*                                     job recipe 관리		                                  */
/* ----------------------------------------------------------------------------------------- */

/*
 desc : Job Recipe 기본 정보 추가 (Append)
 parm : recipe	- [in]  Recipe 정보가 저장된 구조체 포인터
		check	- [in]  값 유효성 체크 여부
 retn : TRUE or FALSE (개수 초과. 최대 15개)
*/
BOOL CRecipeUVDI15::JobRecipeAppend(LPG_RJAF recipe)
{
	BOOL bSucc			= FALSE;
	LPG_RJAF pstRecipe	= NULL;

	if (m_lstJobRecipe.GetCount() >= MAX_REGIST_RECIPE_COUNT)
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
	if (JobRecipeFind(recipe->job_name))
	{
		AfxMessageBox(L"The same recipe (Base Info.) exists", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/*입력된 거버가 해당 경로에 존재하는지 확인*/
	TCHAR tzPath[MAX_PATH_LEN] = { NULL };
	CUniToChar csCnv1, csCnv2;
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\%s",
		csCnv1.Ansi2Uni(recipe->gerber_path), csCnv2.Ansi2Uni(recipe->gerber_name));
	if (!uvCmn_FindPath(tzPath))
	{
		AfxMessageBox(L"TFailed to find the path of gerber file", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}

	/* 맨 마지막 위치에 등록 */
	pstRecipe	= new STG_RJAF;
	ASSERT(pstRecipe);
	pstRecipe->Init();	/* 구조체 내에 내부 메모리 생성 */

	/* 새로 생성된 구조체 영역에 기존 설정된 값 복사 */
	bSucc	= CopyJobRecipe(recipe, pstRecipe);
	/* 등록 */
	if (bSucc)	m_lstJobRecipe.AddTail(pstRecipe);

	/* 현재까지 등록된 모든 Recipe 내용을 파일로 저장 */
	return bSucc && SaveJobFile();
}

/*
 desc : Job Recipe 기본 정보 수정
 parm : recipe	- [in]  Recipe 기본 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
BOOL CRecipeUVDI15::JobRecipeModify(LPG_RJAF recipe)
{
	LPG_RJAF pstRecipe	= NULL;

	/* 필수로 입력되는 값이 있는지 검토 */
	if (!recipe->IsValid())
	{
		AfxMessageBox(L"The update job data (Base Info.) is invalid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 기존에 등록된 레시피가 있는지 확인 */
	pstRecipe = GetJobRecipeOnlyName(recipe->job_name);
	if (!pstRecipe)
	{
		AfxMessageBox(L"The target recipe (Base Info.) was not found", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 새로 생성된 구조체 영역에 기존 설정된 값 복사 */
	CopyJobRecipe(recipe, pstRecipe);

	/* 현재까지 등록된 모든 Recipe 내용을 파일로 저장 */
	return SaveJobFile();
}

/*
 desc : Job Recipe 기본 삭제
 parm : recipe	- [in]  제거하려는 레시피의 기본 이름
 retn : TRUE or FALSE
*/
BOOL CRecipeUVDI15::JobRecipeDelete(PCHAR recipe)
{
	/* 기존에 등록된 레시피가 있는지 확인 */
	if (!GetJobRecipeOnlyName(recipe))
	{
		AfxMessageBox(L"The deleted recipe_name was not found", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* Recipe 삭제 수행 */
	RemoveJobRecipe(recipe);

	/* 현재까지 등록된 모든 Recipe 내용을 파일로 저장 */
	return SaveJobFile();
}
/*
desc : Job 레시피 기본 존재 여부
parm : recipe	- [in]  검색하고자 하는 레시피 이름
retn : TRUE (Finded) or FALSE (Not find)
*/
BOOL CRecipeUVDI15::JobRecipeFind(PCHAR recipe)
{
	POSITION pPos = NULL;

	pPos = m_lstJobRecipe.GetHeadPosition();
	while (pPos)
	{
		if (0 == strcmp(recipe, m_lstJobRecipe.GetNext(pPos)->job_name))	return TRUE;
	}
	return FALSE;
}


/*
 desc : 레시피 이름을 통한 레시피 기본 정보 반환
 parm : recipe	- [in]  검색하고자 하는 레시피 이름
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
LPG_RJAF CRecipeUVDI15::GetJobRecipeOnlyName(PCHAR recipe)
{
	POSITION pPos = NULL;
	LPG_RJAF pstData = NULL;

	pPos = m_lstJobRecipe.GetHeadPosition();
	while (pPos)
	{
		pstData = m_lstJobRecipe.GetNext(pPos);
		if (0 == strcmp(recipe, pstData->job_name))	return pstData;
	}
	return NULL;
}

/*
 desc : 레시피 이름 (전체 경로 포함)을 통한 레시피 반환
 parm : path_name	- [in]  검색하고자 하는 거버 이름 (경로 포함)
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
LPG_RJAF CRecipeUVDI15::GetJobRecipePathName(PCHAR name)
{
	CHAR szGerberPath[MAX_GERBER_NAME];
	POSITION pPos = NULL;
	LPG_RJAF pstData = NULL;

	pPos = m_lstJobRecipe.GetHeadPosition();
	while (pPos)
	{
		pstData = m_lstJobRecipe.GetNext(pPos);
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
LPG_RJAF CRecipeUVDI15::GetJobRecipeIndex(INT32 index)
{
	POSITION pPos = NULL;

	/* 잘못된 Index 값이 들어왔는지 여부 */
	if (index < 0)	return NULL;
	/* 해당 위치 값 얻기 */
	pPos = m_lstJobRecipe.FindIndex(index);
	if (!pPos)	return NULL;

	/* 위치에 해당되는 결과 값 반환 */
	return m_lstJobRecipe.GetAt(pPos);
}

int CRecipeUVDI15::GetSelectJobRecipeIndex()
{
	POSITION pPos = NULL;
	LPG_RJAF pstData = NULL;
	int		nIndex = 0;

	if (NULL == m_pstJobRecipe)
	{
		return -1;
	}

	pPos = m_lstJobRecipe.GetHeadPosition();
	while (pPos)
	{
		pstData = m_lstJobRecipe.GetNext(pPos);
		if (0 == strcmp(m_pstJobRecipe->job_name, pstData->job_name))	return nIndex;
		nIndex++;
	}
	return -1;
}


/* ----------------------------------------------------------------------------------------- */
/*                                     Expo recipe 관리		                                  */
/* ----------------------------------------------------------------------------------------- */

/*
 desc : Expo Recipe 기본 정보 추가 (Append)
 parm : recipe	- [in]  Recipe 정보가 저장된 구조체 포인터
		check	- [in]  값 유효성 체크 여부
 retn : TRUE or FALSE (개수 초과. 최대 15개)
*/
BOOL CRecipeUVDI15::ExpoRecipeAppend(LPG_REAF recipe)
{
	BOOL bSucc = FALSE;
	LPG_REAF pstRecipe = NULL;

	if (m_lstExpoRecipe.GetCount() >= MAX_REGIST_RECIPE_COUNT)
	{
		AfxMessageBox(L"The registered recipe (Base Info.) is full (MAX. 256)", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}

	/* 필수로 입력되는 값이 있는지 검토 */
	if (!recipe->IsValid())
	{
		AfxMessageBox(L"The input data (recipe base info) is invalid", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}

	/* 기존에 등록된 레시피가 있는지 확인 */
	if (ExpoRecipeFind(recipe->expo_name))
	{
		AfxMessageBox(L"The same recipe (Base Info.) exists", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}

	/* 맨 마지막 위치에 등록 */
	pstRecipe = new STG_REAF;
	ASSERT(pstRecipe);
	pstRecipe->Init();	/* 구조체 내에 내부 메모리 생성 */

	/* 새로 생성된 구조체 영역에 기존 설정된 값 복사 */
	bSucc = CopyExpoRecipe(recipe, pstRecipe);
	/* 등록 */
	if (bSucc)	m_lstExpoRecipe.AddTail(pstRecipe);

	/* 현재까지 등록된 모든 Recipe 내용을 파일로 저장 */
	return bSucc && SaveExpoFile();
}

/*
 desc : Expo Recipe 기본 정보 수정
 parm : recipe	- [in]  Recipe 기본 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
BOOL CRecipeUVDI15::ExpoRecipeModify(LPG_REAF recipe)
{
	LPG_REAF pstRecipe = NULL;

	/* 필수로 입력되는 값이 있는지 검토 */
	if (!recipe->IsValid())
	{
		AfxMessageBox(L"The update expose data (Base Info.) is invalid", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}

	/* 기존에 등록된 레시피가 있는지 확인 */
	pstRecipe = GetExpoRecipeOnlyName(recipe->expo_name);
	if (!pstRecipe)
	{
		AfxMessageBox(L"The target recipe (Base Info.) was not found", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}

	/* 새로 생성된 구조체 영역에 기존 설정된 값 복사 */
	CopyExpoRecipe(recipe, pstRecipe);

	/* 현재까지 등록된 모든 Recipe 내용을 파일로 저장 */
	return SaveExpoFile();
}

/*
 desc : Expo Recipe 기본 삭제
 parm : recipe	- [in]  제거하려는 레시피의 기본 이름
 retn : TRUE or FALSE
*/
BOOL CRecipeUVDI15::ExpoRecipeDelete(PCHAR recipe)
{
	/* 기존에 등록된 레시피가 있는지 확인 */
	if (!GetExpoRecipeOnlyName(recipe))
	{
		AfxMessageBox(L"The deleted recipe_name was not found", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}
	/* Recipe 삭제 수행 */
	RemoveExpoRecipe(recipe);

	/* 현재까지 등록된 모든 Recipe 내용을 파일로 저장 */
	return SaveExpoFile();
}
/*
desc : Expo 레시피 기본 존재 여부
parm : recipe	- [in]  검색하고자 하는 레시피 이름
retn : TRUE (Finded) or FALSE (Not find)
*/
BOOL CRecipeUVDI15::ExpoRecipeFind(PCHAR recipe)
{
	POSITION pPos = NULL;

	pPos = m_lstExpoRecipe.GetHeadPosition();
	while (pPos)
	{
		if (0 == strcmp(recipe, m_lstExpoRecipe.GetNext(pPos)->expo_name))	return TRUE;
	}
	return FALSE;
}


/*
 desc : 레시피 이름을 통한 레시피 기본 정보 반환
 parm : recipe	- [in]  검색하고자 하는 레시피 이름
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
LPG_REAF CRecipeUVDI15::GetExpoRecipeOnlyName(PCHAR recipe)
{
	POSITION pPos = NULL;
	LPG_REAF pstData = NULL;

	pPos = m_lstExpoRecipe.GetHeadPosition();
	while (pPos)
	{
		pstData = m_lstExpoRecipe.GetNext(pPos);
		if (0 == strcmp(recipe, pstData->expo_name))	return pstData;
	}
	return NULL;
}

/*
 desc : 레시피 기본 정보가 저장된 인덱스 위치에 있는 레시피 반환
 parm : index	- [in]  가져오고자 하는 레시피 인덱스 (Zero-based)
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
LPG_REAF CRecipeUVDI15::GetExpoRecipeIndex(INT32 index)
{
	POSITION pPos = NULL;

	/* 잘못된 Index 값이 들어왔는지 여부 */
	if (index < 0)	return NULL;
	/* 해당 위치 값 얻기 */
	pPos = m_lstExpoRecipe.FindIndex(index);
	if (!pPos)	return NULL;

	/* 위치에 해당되는 결과 값 반환 */
	return m_lstExpoRecipe.GetAt(pPos);
}

int CRecipeUVDI15::GetSelectExpoRecipeIndex()
{
	POSITION pPos = NULL;
	LPG_REAF pstData = NULL;
	int		nIndex = 0;

	if (NULL == m_pstExpoRecipe)
	{
		return -1;
	}

	pPos = m_lstExpoRecipe.GetHeadPosition();
	while (pPos)
	{
		pstData = m_lstExpoRecipe.GetNext(pPos);
		if (0 == strcmp(m_pstExpoRecipe->expo_name, pstData->expo_name))	return nIndex;
		nIndex++;
	}

	return -1;
}

BOOL CRecipeUVDI15::CalcSpeedEnergy(LPG_RJAF recipe)
{
	// by sysandj : 변수없음
// 	UINT8 i, j;
// 	DOUBLE dbReqSpeed = 0.0f, dbTimeOnePoint = 0.0f, dbExposeArea = 0.0f, dbPixelSize = 0.0f;
// 	DOUBLE dbPowerCm2 = 0.0f, dbSumEnergy = 0.0f, dbFactor = 0.0f;
// 	DOUBLE dbSensorArea = 0.0f, dbImageArea = 0.0f;
// 	DOUBLE dbMaxSpeed = 0.0f/*uvCmn_Luria_GetExposeMaxSpeed(recipe->step_size)*/;
// 	FLOAT** pfLedPower = NULL;
// 
// 	LPG_REAF expo_recipe = GetExpoRecipeOnlyName(recipe->expo_recipe);
// 
// 	if (NULL == expo_recipe)
// 	{
// 		return FALSE;
// 	}
// 
// 	/* 최대 노광 속도 */
// 	dbMaxSpeed = m_pstLuriaMem->machine.scroll_rate *
// 		(GetConfig()->luria_svc.pixel_size /*+ MIRROR_PERIOD_UM*/) / 1000.0f * expo_recipe->step_size;
// 
// 	/* Power Name에 따른 Watt 값 얻기 */
// 	if (!m_pLedPower->GetLedPowerName(expo_recipe->power_name))
// 	{
// 		AfxMessageBox(L"Failed to get the led_watt value corresponding to the led power name", MB_ICONSTOP | MB_TOPMOST);
// 		return FALSE;
// 	}
// 	pfLedPower = m_pLedPower->GetLedPowerName(expo_recipe->power_name)->led_watt;
// 	if (!pfLedPower)
// 	{
// 		AfxMessageBox(L"Failed to get the led_watt value corresponding to the led power name", MB_ICONSTOP | MB_TOPMOST);
// 		return FALSE;
// 	}
// 	/* Expose Speed */
// 	recipe->expo_speed = FLOAT(dbMaxSpeed * (expo_recipe->frame_rate / 1000.0f));
// 	/* Pixel Size (mm) */
// 	dbPixelSize = GetConfig()->luria_svc.pixel_size / 1000.0f;
// 	/* Requirement Speed */
// 	dbReqSpeed = dbMaxSpeed * (expo_recipe->frame_rate / 1000.0f);
// 	/* Time on one point (R) */
// 	dbTimeOnePoint = (dbPixelSize / dbReqSpeed) *
// 		(GetConfig()->luria_svc.mirror_count_xy[1] * (expo_recipe->led_duty_cycle / 100.0f));
// 	/* Expose Area */
// 	dbExposeArea = (GetConfig()->luria_svc.mirror_count_xy[0] * dbPixelSize) *
// 		(GetConfig()->luria_svc.mirror_count_xy[1] * dbPixelSize);
// 	/* Image Area (cm^2) */
// 	dbImageArea = ((GetConfig()->luria_svc.mirror_count_xy[0] * dbPixelSize) *
// 		(GetConfig()->luria_svc.mirror_count_xy[1] * dbPixelSize)) / 100.0f;
// 	/* Illuminator Sensor Area */
// 	dbSensorArea = (M_PI * pow((GetConfig()->luria_svc.sensor_diameter / 2.0f), 2)) / 100.0f;
// 	/* 5개 광학계의 Power (Watt) 합 */
// 	for (i = 0; i < GetConfig()->luria_svc.ph_count; i++)
// 	{
// 		/* Sum Factor */
// 		for (j = 0, dbFactor = 0.0f; j < 4; j++)
// 		{
// 			dbFactor += ((pfLedPower[i][j] * GetConfig()->luria_svc.illum_filter_rate[j] *
// 				GetConfig()->luria_svc.correction_factor * dbImageArea) / dbSensorArea) / 1000.0f;
// 		}
// 		/* Power of CM^2 (W/cm^2) */
// 		dbPowerCm2 = dbFactor / (dbExposeArea / 100.0f);
// 		/* Photohead 별로 노광량 계산 */
// 		recipe->expo_energy = (FLOAT)(dbPowerCm2 * dbTimeOnePoint * 1000.0f);
// 		dbSumEnergy += recipe->expo_energy[i];
// 	}
// 	/* 전체 광학계의 평균 노광량 계산 */
// 	recipe->expo_energy_avg = (FLOAT)ROUNDED(dbSumEnergy / GetConfig()->luria_svc.ph_count, 3);

	return TRUE;
}