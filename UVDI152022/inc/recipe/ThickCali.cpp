
/*
 desc : 소재 두께에 따른 얼라인 카메라 2대의 Mark 2의 거버 좌표에 따른 모션 절대 위치 관리
*/

#include "pch.h"
#include "ThickCali.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : work_dir- [in]  실행 파일이 실행되는 경로 (전체 경로, '\' 제외)
 retn : None
*/
CThickCali::CThickCali(PTCHAR work_dir)
	: CBase(work_dir)
{
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CThickCali::~CThickCali()
{
	RemoveAll();
}

/*
 desc : 기존 등록된 레시피 모두 제거
 parm : None
 retn : None
*/
VOID CThickCali::RemoveAll()
{
	POSITION pPos	= NULL;
	LPG_MACP pRecipe= NULL;

	/* 기존 등록된 Recipe Data 메모리 해제 */
	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		pRecipe	= m_lstRecipe.GetNext(pPos);
		if (pRecipe)	::Free(pRecipe);
	}
	/* 리스트 비움 */
	m_lstRecipe.RemoveAll();
}

/*
 desc : Calibration Recipe 적재
 parm : None
 retn : TRUE or FALSE
*/
BOOL CThickCali::LoadFile()
{
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	CHAR szData[1024]	= {NULL};
	errno_t eRet		= 0;
	FILE *fp			= NULL;
	CUniToChar csCnv;

	/* 기존 등록된 레시피 제거 */
	RemoveAll();

	/* Recipe File 설정 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\cali\\%s.dat",
			   m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.thick_cali);
	/* 파일 열기 */
	
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "rt");
	if (0 != eRet)	return TRUE;
	while (!feof(fp))
	{
		
		/* 한줄 읽어오기 */
		memset(szData, 0x00, 1024);
		fgets(szData, 1024, fp);
		/* 문자열 길이가 특정 개수 이하인 경우인지 (만약 첫 글자가 주석인 세미콜론(;)이면 Skip) */
		if (strlen(szData) == 0 || ';' == szData[0]) 
			continue;

		ParseAppend(szData, (UINT32)strlen(szData));
		
	}

	/* 파일 핸들 닫기 */
	fclose(fp);

	return TRUE;
}

/*
 desc : Recipe 분석 및 등록 진행
 parm : data	- [in]  Recipe 정보가 저장된 문자열 버퍼
		size	- [in]  'data' 버퍼에 저장된 데이터의 길이
 retn : TRUE or FALSE
*/
BOOL CThickCali::ParseAppend(CHAR *data, UINT32 size)
{
	UINT8 u8Div			= 5;
	INT32 i32Value		= 0;
	UINT32 i, u32Find	= 0;
	CHAR *pData	= data, *pFind, szValue[64];
	LPG_MACP pstRecipe	= {NULL};

	/* 일단, 주어진 문자열 중에서 콤마(',') 개수가 xxx 개인지 확인 */
	for (i=0; i<size; i++)
	{
		if (',' == data[i])	u32Find++;
	}
	if (u32Find < u8Div)
	{
		AfxMessageBox(L"Failed to analyse the value from <thick_cali> file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 순서대로 읽어서 구조체에 저장 */
	pstRecipe	= (LPG_MACP)::Alloc(sizeof(STG_MACP));
	ASSERT(pstRecipe);

	for (i=0; i< u32Find;i++)
	{
		pFind	= strchr(pData, ',');
		if (!pFind)
		{
			::Free(pstRecipe);
			return FALSE;
		}

		memset(szValue, 0x00, 64);
		memcpy(szValue, pData, pFind - pData);
		pData	= ++pFind;

		switch (i)
		{
			case 0x00:	pstRecipe->film_thick_um	= (UINT32)atoi(szValue);break;
			case 0x01:	pstRecipe->mark2_stage_y[0]	= atof(szValue);		break;
			case 0x02:	pstRecipe->mark2_stage_y[1]	= atof(szValue);		break;
			case 0x03:	pstRecipe->mark2_acam_x[0]	= atof(szValue);		break;
			case 0x04:	pstRecipe->mark2_acam_x[1]	= atof(szValue);		break;
			case 0x05:	pstRecipe->mark2CentercamOffsetXY[0] = atof(szValue);		break;
			case 0x06:	pstRecipe->mark2CentercamOffsetXY[1] = atof(szValue);		break;
		}
	}

	/* 메모리에 분석된 레시피 데이터 등록 */
	m_lstRecipe.AddTail(pstRecipe);

	return TRUE;
}

/*
 desc : 레시피 속도 검색
 parm : thick	- [in]  검색하고자 하는 레시피 인덱스 (Thickness: um)
 retn : TRUE (Finded) or FALSE (Not find)
*/
BOOL CThickCali::RecipeFind(UINT32 thick)
{
	POSITION pPos		= NULL;
	LPG_MACP pstRecipe	= NULL;

	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		pstRecipe	= m_lstRecipe.GetNext(pPos);
		if (thick == pstRecipe->film_thick_um)	return TRUE;
	}
	return FALSE;
}

/*
 desc : 레시피 이름을 통한 레시피 반환
 parm : thick	- [in]  검색하고자 하는 레시피 인덱스 (Thickness: um)
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
LPG_MACP CThickCali::GetRecipe(UINT32 thick)
{
	POSITION pPos		= NULL;
	LPG_MACP pstRecipe	= NULL;

	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		pstRecipe	= m_lstRecipe.GetNext(pPos);
		if (thick == pstRecipe->film_thick_um)	return pstRecipe;
	}
	return NULL;
}

/*
 desc : 레시피가 저장된 인덱스 위치에 있는 레시피 반환
 parm : index	- [in]  가져오고자 하는 레시피 인덱스 (Zero-based)
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
LPG_MACP CThickCali::GetRecipeIndex(UINT32 index)
{
	POSITION pPos	= NULL;

	/* 해당 위치 값 얻기 */
	pPos	= m_lstRecipe.FindIndex(index);
	if (!pPos)	return NULL;

	/* 위치에 해당되는 결과 값 반환 */
	return m_lstRecipe.GetAt(pPos);
}



//221219 abh1000
/*
 desc : Calibration Recipe 저장
 parm : None
 retn : TRUE or FALSE
*/
BOOL CThickCali::SaveFile()
{
	UINT32 i = 0, j = 0;
	CHAR szData[256] = { NULL };
	TCHAR tzFile[MAX_PATH_LEN] = { NULL };
	errno_t eRet = 0;
	FILE* fp = NULL;
	CUniToChar csCnv;
	POSITION pPos = NULL;
	LPG_MACP pstRecipe = { NULL };

	/* Recipe File 설정 */
// 	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\data\\cali\\%s.dat",
// 		m_tzWorkDir, GetConfig()->file_dat.ph_step);
	/* Recipe File 설정 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\cali\\%s.dat",
		m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.thick_cali);
	/* 파일 열기 */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "wt");
	if (0 != eRet)	return TRUE;


	fputs("; film_thick (um),mark2_stage_y_1/2 (mm),mark2_acam_x_1/2 (mm),\r\n", fp);

	pPos = m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		/* 값 가져오기 */
		pstRecipe = (LPG_MACP)m_lstRecipe.GetNext(pPos);


		/* material_thick, calibration_thick */
		sprintf_s(szData, 256, "%05u,%.4f,%.4f,%.4f,%.4f,\n",
			pstRecipe->film_thick_um,
			pstRecipe->mark2_stage_y[0], pstRecipe->mark2_stage_y[1],
			pstRecipe->mark2_acam_x[0], pstRecipe->mark2_acam_x[1],
			pstRecipe->mark2CentercamOffsetXY[0], pstRecipe->mark2CentercamOffsetXY[1]);
		fputs(szData, fp);
	}

	fclose(fp);

	/* 다시 적재 수행 */
	return LoadFile();
}

/*
 desc : Recipe 수정
 parm : step	- [in]  Recipe 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
BOOL CThickCali::RecipeModify(LPG_MACP step)
{
	LPG_MACP pstData = NULL;

	/* 필수로 입력되는 값이 있는지 검토 */
	if (!step->IsValid())
	{
		AfxMessageBox(L"The update data is invalid", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}

	/* 기존에 등록된 레시피가 있는지 확인 */
	pstData = GetRecipe(step->film_thick_um);
	if (!pstData)
	{
		AfxMessageBox(L"The target file thick um was not found", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}

	/* 새로 생성된 구조체 영역에 기존 설정된 값 복사 */
	memcpy(pstData, step, sizeof(STG_MACP));

	/* 현재까지 등록된 모든 Recipe 내용을 파일로 저장 */
	return SaveFile();
}