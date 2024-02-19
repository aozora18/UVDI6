
/*
 desc : 광학계 동작 속도 즉, 스테이지 이동 속도에 따른 광학계들의 단차 값 (레시피) 관리
*/

#include "pch.h"
#include "CorrectY.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : config	- [in]  전역 환경 파일
		work	- [in]  작업 기본 경로 (Full path)
		shmem	- [in]  레시피가 저장될 메모리 시작 위치 (구조체 포인터)
 retn : None
*/
CCorrectY::CCorrectY(PTCHAR work_dir, LPG_OSCY shmem)
	: CBase(work_dir)
{
	m_pstSelRecipe	= NULL;
	m_pstShMemRecipe= shmem;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CCorrectY::~CCorrectY()
{
}

/*
 desc : 기존 등록된 레시피 모두 초기화
 parm : None
 retn : None
*/
VOID CCorrectY::ResetRecipeAll()
{
	/* 기존 등록된 Recipe Data 메모리 초기화 */
	memset(m_pstShMemRecipe, 0x00, sizeof(STG_OSCY) * MAX_PH_CORRECT_Y);
	/* 기존 선택된 레시피 초기화 */
	m_pstSelRecipe	= NULL;
}

/*
 desc : Calibration Recipe 적재
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCorrectY::LoadFile()
{
	CHAR szData[1024]			= {NULL};
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	UINT32 u32Index	= 0;	/* 레시피가 저장될 메모리 위치 즉, 구조체 인덱스 (Zero-based) */
	errno_t eRet	= 0;
	FILE *fp		= NULL;
	CUniToChar csCnv;

	/* 기존 등록된 레시피 제거 */
	ResetRecipeAll();

	/* Recipe File 설정 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\cali\\%s.dat",
			   m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.correct_y);
	/* 파일 열기 */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "rt");
	if (0 != eRet)	return TRUE;
	while (!feof(fp))
	{
		/* 한줄 읽어오기 */
		memset(szData, 0x00, 1024);
		fgets(szData, 1024, fp);
		/* 문자열 길이가 특정 개수 이하인 경우인지 (만약 첫 글자가 주석인 세미콜론(;)이면 Skip) */
		if (strlen(szData) > 0 && ';' != szData[0] && '\n' != szData[0])
		{
			/* 분석 및 등록하기 */
			if (!ParseAppend(u32Index, szData, (UINT32)strlen(szData)))
			{
				fclose(fp);
				return FALSE;
			}
		}
		/* 등록된 개수 이상으로 저장되어 있으면, 더 이상 등록작업 수행 안함 */
		if (++u32Index >= MAX_PH_CORRECT_Y)	break;
	}
	/* 파일 핸들 닫기 */
	fclose(fp);

	return TRUE;
}

/*
 desc : Recipe 분석 및 등록 진행
 parm : index	- [in]  레시피가 저장된 메모리 인덱스 (구조체 인덱스. Zero-based)
		data	- [in]  Recipe 정보가 저장된 문자열 버퍼
		size	- [in]  'data' 버퍼에 저장된 데이터의 길이
 retn : TRUE or FALSE
*/
BOOL CCorrectY::ParseAppend(UINT32 index, CHAR *data, UINT32 size)
{
	UINT8 u8Val		= 0x00;
	UINT32 u32Find	= 0, i = 0;
	CHAR *pData	= data, *pFind, szValue[128];
	LPG_OSCY pstData	= &m_pstShMemRecipe[index];

	/* 일단, 주어진 문자열 중에서 콤마(',') 개수가 9개인지 확인 */
	for (i=0; i<size; i++)
	{
		if (',' == data[i])	u32Find++;
	}

	/* 00.speed (frame) rate */
	pFind	= strchr(pData, ',');
	memset(szValue, 0x00, 128);
	memcpy(szValue, pData, pFind - pData);
	pData	= ++pFind;
	pstData->frame_rate	= (UINT32)atol(szValue);
	/* 01.Total Count */
	pFind	= strchr(pData, ',');
	memset(szValue, 0x00, 128);
	memcpy(szValue, pData, pFind - pData);
	pData	= ++pFind;
	pstData->stripe_count	= (UINT16)atol(szValue);

	if (u32Find != (pstData->stripe_count + 2))
	{
		AfxMessageBox(L"Failed to analyse the value from <correct_y> file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 전체 Stripe의 Y 단차 값 */
	for (i=0; i<pstData->stripe_count; i++)
	{
		pFind	= strchr(pData, ',');
		memset(szValue, 0x00, 128);
		memcpy(szValue, pData, pFind - pData);
		pData	= ++pFind;
		/* Stripe 단차 값 저장 (단위: nm) */
		pstData->adjust[i] = (INT32)ROUNDED(atof(szValue) * 1000.0f*-1.0f, 0);
	}

	return TRUE;
}

/*
 desc : 레시피 정보 검색
 parm : frame_rate	- [in]  검색하고자 하는 스테이지의 동작 속도 즉, 노광 속도
 retn : TRUE (Finded) or FALSE (Not find)
*/
BOOL CCorrectY::RecipeFind(UINT16 frame_rate)
{
	UINT16 i	= 0;

	for (; i<MAX_PH_CORRECT_Y; i++)
	{
		if (frame_rate == m_pstShMemRecipe[i].frame_rate)	return TRUE;
	}
	return FALSE;
}

/*
 desc : 레시피 이름 검색를 통한 제거 (메모리 제거는 아님)
 parm : frame_rate	- [in]  삭제하고자 하는 스테이지의 동작 속도 즉, 노광 속도
 retn : TRUE (Deleted) or FALSE (Not delete)
*/
BOOL CCorrectY::ResetRecipe(UINT16 frame_rate)
{
	UINT16 i	= 0;

	for (; i<MAX_PH_CORRECT_Y; i++)
	{
		if (frame_rate == m_pstShMemRecipe[i].frame_rate)
		{
			m_pstShMemRecipe[i].Reset();
			return TRUE;
		}
	}
	return FALSE;
}

/*
 desc : 레시피 이름을 통한 레시피 반환
 parm : frame_rate	- [in]  검색하고자 하는 스테이지의 동작 속도 즉, 노광 속도
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
LPG_OSCY CCorrectY::GetRecipeData(UINT16 frame_rate)
{
	UINT16 i	= 0;

	for (; i<MAX_PH_CORRECT_Y; i++)
	{
		if (frame_rate == m_pstShMemRecipe[i].frame_rate)	return &m_pstShMemRecipe[i];
	}
	return NULL;
}

/*
 desc : 레시피 이름을 통한 레시피 선택
 parm : frame_rate	- [in]  검색하고자 하는 스테이지의 동작 속도 즉, 노광 속도
 retn : TRUE (선택 성공) or FALSE (선택 실패)
*/
BOOL CCorrectY::SetRecipeData(UINT16 frame_rate)
{
	UINT16 i	= 0;

	for (; i<MAX_PH_CORRECT_Y; i++)
	{
		if (frame_rate == m_pstShMemRecipe[i].frame_rate)
		{
			m_pstSelRecipe	= &m_pstShMemRecipe[i];
			return TRUE;
		}
	}
	return FALSE;
}

/*
 desc : 레시피가 저장된 인덱스 위치에 있는 레시피 반환
 parm : index	- [in]  가져오고자 하는 레시피 인덱스 (Zero-based)
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
LPG_OSCY CCorrectY::GetRecipeIndex(UINT32 index)
{
	if (index >= MAX_PH_CORRECT_Y)	return NULL;

	/* 위치에 해당되는 결과 값 반환 */
	return &m_pstShMemRecipe[index];
}

/*
 desc : Calibration Recipe 저장
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCorrectY::SaveFile()
{
	UINT32 i = 0, j	= 0;
	CHAR szData[256]= {NULL};
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	errno_t eRet	= 0;
	FILE *fp		= NULL;
	CUniToChar csCnv;

	/* Recipe File 설정 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\cali\\%s.dat",
			   m_tzWorkDir, CUSTOM_DATA_CONFIG,GetConfig()->file_dat.correct_y);
	/* 파일 열기 */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "wt");
	if (0 != eRet)	return TRUE;
	fputs("; frame_rate,total,correction_y1 ~ n (max 200. unit:um)\r\n", fp);
	for (; i<MAX_PH_CORRECT_Y; i++)
	{
		if (m_pstShMemRecipe[i].frame_rate > 0)
		{
			/* 00.Frame Rate */
			sprintf_s(szData, 256,	"%04u,", m_pstShMemRecipe[i].frame_rate);
			fputs(szData, fp);
			/* 01.Stripe Count */
			sprintf_s(szData, 256,	"%03u,", m_pstShMemRecipe[i].stripe_count);
			fputs(szData, fp);
			/* 02. ~ ... Stripe Correct Y 값 */
			for (j=0; j<m_pstShMemRecipe[i].stripe_count; j++)
			{
				sprintf_s(szData, 256, "%.1f", (m_pstShMemRecipe[i].adjust[j] / 1000.0f) * -1.0f);
				fputs(szData, fp);
			}
			fputs("\n", fp);
		}
	}
	/* 파일 핸들 닫기 */
	fclose(fp);

	/* 다시 적재 수행 */
	return LoadFile();
}

/*
 desc : Recipe Append
 parm : adjust	- [in]  Recipe 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE (개수 초과. 최대 15개)
*/
BOOL CCorrectY::RecipeAppend(LPG_OSCY adjust)
{
	UINT32 i		= 0;
	BOOL bAppended	= FALSE;
	LPG_OSCY pstData= NULL;

	/* 필수로 입력되는 값이 있는지 검토 */
	if (!adjust->IsValid())
	{
		AfxMessageBox(L"The input data is invalid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 기존에 등록된 레시피가 있는지 확인 */
	if (RecipeFind(adjust->frame_rate))
	{
		AfxMessageBox(L"There is a speed_rate for the same recipe", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 비어 있는 영역에 값 설정 */
	for (; i<MAX_PH_CORRECT_Y; i++)
	{
		/* 비어 있는 공간에 값 복사*/
		if (m_pstShMemRecipe[i].frame_rate == 0)
		{
			/* 새로 생성된 구조체 영역에 기존 설정된 값 복사 */
			memcpy(&m_pstShMemRecipe[i], adjust, sizeof(STG_OSCY));
			bAppended	= TRUE;
			break;
		}
	}

	/* 현재까지 등록된 모든 Recipe 내용을 파일로 저장 */
	return SaveFile();
}

/*
 desc : Recipe Delete
 parm : adjust	- [in]  Recipe 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
BOOL CCorrectY::RecipeModify(LPG_OSCY adjust)
{
	LPG_OSCY pstData	= NULL;

	/* 필수로 입력되는 값이 있는지 검토 */
	if (!adjust->IsValid())
	{
		AfxMessageBox(L"The update data is invalid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 기존에 등록된 레시피가 있는지 확인 */
	pstData = GetRecipeData(adjust->frame_rate);
	if (!pstData)
	{
		AfxMessageBox(L"The target speed_rate was not found", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 새로 생성된 구조체 영역에 기존 설정된 값 복사 */
	memcpy(adjust, pstData, sizeof(STG_OSCY));

	/* 현재까지 등록된 모든 Recipe 내용을 파일로 저장 */
	return SaveFile();
}

/*
 desc : Recipe Delete
 parm : frame_rate	- [in]  제거하려는 스테이지의 이동 속도 즉, 노광 속도
 retn : TRUE or FALSE
*/
BOOL CCorrectY::RecipeDelete(UINT16 frame_rate)
{
	/* 기존에 등록된 레시피가 있는지 확인 */
	if (!GetRecipeData(frame_rate))
	{
		AfxMessageBox(L"The deleted speed_rate was not found", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* Recipe 삭제 수행 */
	ResetRecipe(frame_rate);

	/* 현재까지 등록된 모든 Recipe 내용을 파일로 저장 */
	return SaveFile();
}

/*
 desc : 현재 등록된 레시피 (포토헤드 단차) 개수 반환
 parm : None
 retn : 등록된 개수 반환
*/
UINT32 CCorrectY::GetRecipeCount()
{
	UINT32 i	= 0, u32Count = 0;

	for (; i<MAX_PH_CORRECT_Y; i++)
	{
		if (m_pstShMemRecipe[i].frame_rate != 0)	u32Count++;
	}
	return u32Count;
}

/*
 desc : 레시피를 가져오고자 하는 시작 위치 초기화
 parm : None
 retn : 레시피가 저장된 처음 위치 값 (Zero-based) 반환
		음수 값이면, 더 이상 등록된 레시피가 없음
*/
INT32 CCorrectY::GetRecipeStart()
{
	UINT32 i	= 0;
	for (; i<MAX_PH_CORRECT_Y; i++)
	{
		if (0 != m_pstShMemRecipe[i].frame_rate)
		{
			m_u32RecipeNext	= i;	/* 가장 최근에 조회한 레시피 위치 저장 */
			return i;
		}
	}

	return -1;
}

/*
 desc : 현재 위치에서 다음에 저장된 레시피의 저장 위치 반환
 parm : None
 retn : 기존 검색된 레시피가 저장된 위치 값 (Zero-based) 반환
		음수 값이면, 더 이상 등록된 레시피가 없음
*/
INT32 CCorrectY::GetRecipeNext()
{
	UINT32 i	= m_u32RecipeNext + 1;
	for (; i<MAX_PH_CORRECT_Y; i++)
	{
		m_u32RecipeNext	= i;	/* 가장 최근에 조회한 레시피 위치 저장 */
		if (0 != m_pstShMemRecipe[i].frame_rate)	return i;
	}

	return -1;
}