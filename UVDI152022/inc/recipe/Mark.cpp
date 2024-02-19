
/*
 desc : Mark Model과 Recipe 적재 및 관리
*/

#include "pch.h"
#include "Mark.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

/*
 desc : 생성자
 parm : work	- [in]  작업 기본 경로 (Full path)
 retn : None
*/
CMark::CMark(PTCHAR work_dir)
	: CBase(work_dir)
{
	m_pstSelected	= NULL;
	m_pstROI = (LPG_CRD)::Alloc(sizeof(STG_CRD));
	for (int i = 0; i < 2; i++) {
		m_pstROI->roi_Left[i] = 5;
		m_pstROI->roi_Right[i] = ACA1920_SIZE_X - 5;
		m_pstROI->roi_Top[i] = 5;
		m_pstROI->roi_Bottom[i] = ACA1920_SIZE_Y - 5;
	}
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CMark::~CMark()
{
	RemoveModelAll();
	RemoveMarkAll();
}

/*
 desc : 기존 등록된 모델 모두 제거
 parm : None
 retn : None
*/
VOID CMark::RemoveModelAll()
{
	POSITION pPos	= NULL;
	LPG_CMPV pstData= NULL;

	/* 기존 등록된 Model Data 메모리 해제 */
	pPos	= m_lstModel.GetHeadPosition();
	while (pPos)
	{
		pstData	= m_lstModel.GetNext(pPos);
		if (pstData)	::Free(pstData);
	}
	/* 리스트 비움 */
	m_lstModel.RemoveAll();
}

/*
 desc : 기존 등록된 레시피 모두 제거
 parm : None
 retn : None
*/
VOID CMark::RemoveMarkAll()
{
	POSITION pPos	= NULL;
	LPG_CMRD pstData= NULL;

	/* 기존 등록된 Model Data 메모리 해제 */
	pPos	= m_lstMark.GetHeadPosition();
	while (pPos)
	{
		pstData	= m_lstMark.GetNext(pPos);
		if (pstData)
		{
			if (pstData->m_name)	pstData->Close();
			::Free(pstData);
		}
	}
	/* 리스트 비움 */
	m_lstMark.RemoveAll();
}

/*
 desc : Mark Model 및 Recipe 적재
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMark::LoadFile()
{
	CHAR szData[1024]	= {NULL};
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	errno_t eRet	= 0;
	FILE *fp		= NULL;
	CUniToChar csCnv;

	/* 기존 등록된 Model 제거 */
	RemoveModelAll();
	RemoveMarkAll();

	/* ----------------------------------------------------------------------------------------- */
	/*                                     Model File 적재                                       */
	/* ----------------------------------------------------------------------------------------- */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\data\\recipe\\%s.dat",
			   m_tzWorkDir, GetConfig()->file_dat.mark_model);
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
			ParseModel(szData, (UINT32)strlen(szData));
		}
	}
	/* 파일 핸들 닫기 */
	fclose(fp);

	/* ----------------------------------------------------------------------------------------- */
	/*                                    Recipe File 적재                                       */
	/* ----------------------------------------------------------------------------------------- */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\data\\recipe\\%s.dat",
			   m_tzWorkDir, GetConfig()->file_dat.mark_recipe);
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
			ParseMark(szData, (UINT32)strlen(szData));
		}
	}
	/* 파일 핸들 닫기 */
	fclose(fp);

/* ----------------------------------------------------------------------------------------- */
/*                                    ROI File 적재                                       	 */
/* ----------------------------------------------------------------------------------------- */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\data\\recipe\\%s.dat",
		m_tzWorkDir, GetConfig()->file_dat.mark_roi);
	/* 파일 열기 */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "rt");
	if (0 != eRet) {
		SaveROI();
		eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "rt");
	}
	int cnt = 0;
	while (!feof(fp))
	{
		/* 한줄 읽어오기 */
		memset(szData, 0x00, 1024);
		fgets(szData, 1024, fp);
		/* 문자열 길이가 특정 개수 이하인 경우인지 (만약 첫 글자가 주석인 세미콜론(;)이면 Skip) */
		if (strlen(szData) > 0 && ';' != szData[0] && '\n' != szData[0])
		{
			/* 분석 및 등록하기 */
			ParseSearchROI(szData, (UINT32)strlen(szData), cnt);
			cnt++;
		}
	}
	/* 파일 핸들 닫기 */
	fclose(fp);

	return TRUE;
}

/*
 desc : Model 분석 및 등록 진행
 parm : data	- [in]  Model 정보가 저장된 문자열 버퍼
		size	- [in]  'data' 버퍼에 저장된 데이터의 길이
 retn : TRUE or FALSE
*/
BOOL CMark::ParseModel(PCHAR data, UINT32 size)
{
	UINT32 u32Find	= 0, i = 0;
	CHAR *pData		= data, *pFind, szValue[256];
	LPG_CMPV pstData= {NULL};

	/* 일단, 주어진 문자열 중에서 콤마(',') 개수가 9개인지 확인 */
	for (; i<size; i++)
	{
		if (',' == data[i])	u32Find++;
	}
	if (u32Find != 8)
	{
		AfxMessageBox(L"Failed to analyse the value from model <mark_model> file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 순서대로 읽어서 구조체에 저장 */
	pstData	= (LPG_CMPV)::Alloc(sizeof(STG_CMPV));
	ASSERT(pstData);
	/* 메모리 할당 및 초기화 */
	pstData->Reset();
	/* 00.model name */
	pFind	= strchr(pData, ',');
	if (pFind)
	{
		memcpy(pstData->name, pData, pFind - pData);
		pData = ++pFind;
	}
	/* 01.model type */
	memset(szValue, 0x00, 256);
	pFind	= strchr(pData, ',');
	if (pFind)
	{
		memcpy(szValue, pData, pFind - pData);
		pData = ++pFind;
		pstData->type	= (UINT32)atoi(szValue);
	}

	/* 01. find type */
	memset(szValue, 0x00, 256);
	pFind = strchr(pData, ',');
	if (pFind)
	{
		memcpy(szValue, pData, pFind - pData);
		pData = ++pFind;
		pstData->find_type = (UINT32)atoi(szValue);
	}

	/* 모델 형식이 MMF 파일이 아닌 경우 */
	if (pstData->find_type == 0 && ENG_MMDT(pstData->type) != ENG_MMDT::en_image)
	//if (ENG_MMDT(pstData->type) != ENG_MMDT::en_image)
	{
		/* 02.param - 1 */
		memset(szValue, 0x00, 256);
		pFind	= strchr(pData, ',');
		if (pFind)
		{
			memcpy(szValue, pData, pFind - pData);
			pData = ++pFind;
			pstData->param[0]	= (DOUBLE)atof(szValue);
		}
		/* 03.param - 2 */
		memset(szValue, 0x00, 256);
		pFind	= strchr(pData, ',');
		if (pFind)
		{
			memcpy(szValue, pData, pFind - pData);
			pData = ++pFind;
			pstData->param[1]	= (DOUBLE)atof(szValue);
		}
		/* 04.param - 3 */
		memset(szValue, 0x00, 256);
		pFind	= strchr(pData, ',');
		if (pFind)
		{
			memcpy(szValue, pData, pFind - pData);
			pData = ++pFind;
			pstData->param[2]	= (DOUBLE)atof(szValue);
		}
		/* 05.param - 4 */
		memset(szValue, 0x00, 256);
		pFind	= strchr(pData, ',');
		if (pFind)
		{
			memcpy(szValue, pData, pFind - pData);
			pData = ++pFind;
			pstData->param[3]	= (DOUBLE)atof(szValue);
		}
		/* 06.param - 5 */
		memset(szValue, 0x00, 256);
		pFind	= strchr(pData, ',');
		if (pFind)
		{
			memcpy(szValue, pData, pFind - pData);
			pData = ++pFind;
			pstData->param[4]	= (DOUBLE)atof(szValue);
		}
	}
	else
	{
		/* 02.mmf,pat file name (확장자 제외) */
		memset(szValue, 0x00, 256);
		pFind	= strchr(pData, ',');
		if (pFind)
		{
			memcpy(pstData->file, pData, pFind - pData);
			pData = ++pFind;
		}
		/* 03.Mark Size x */
		memset(szValue, 0x00, 256);
		pFind = strchr(pData, ',');
		if (pFind)
		{
			memcpy(szValue, pData, pFind - pData);
			pData = ++pFind;
			pstData->iSizeP.x = (UINT32)atof(szValue);
		}
		/* 04.Mark Size y */
		memset(szValue, 0x00, 256);
		pFind = strchr(pData, ',');
		if (pFind)
		{
			memcpy(szValue, pData, pFind - pData);
			pData = ++pFind;
			pstData->iSizeP.y = (UINT32)atof(szValue);
		}
		/* 05.Mark Center x */
		memset(szValue, 0x00, 256);
		pFind = strchr(pData, ',');
		if (pFind)
		{
			memcpy(szValue, pData, pFind - pData);
			pData = ++pFind;
			pstData->iOffsetP.x = (UINT32)atof(szValue);
		}
		/* 06.Mark Center y */
		memset(szValue, 0x00, 256);
		pFind = strchr(pData, ',');
		if (pFind)
		{
			memcpy(szValue, pData, pFind - pData);
			pData = ++pFind;
			pstData->iOffsetP.y = (UINT32)atof(szValue);
		}
	}

	/* 메모리에 분석된 Model 데이터 등록 */
	m_lstModel.AddTail(pstData);

	return TRUE;
}

/*
 desc : Recipe 분석 및 등록 진행
 parm : data	- [in]  Model 정보가 저장된 문자열 버퍼
		size	- [in]  'data' 버퍼에 저장된 데이터의 길이
 retn : TRUE or FALSE
*/
BOOL CMark::ParseMark(PCHAR data, UINT32 size)
{
	UINT8 u8MarkType	= 0x00;
	UINT32 u32Find		= 0, i = 0;
	CHAR *pData			= data, *pFind, szValue[32], szName[MARK_MODEL_NAME_LENGTH] = {NULL};
	LPG_CMRD pstRecipe	= {NULL};

	/* 일단, 주어진 문자열 중에서 콤마(',') 개수가 9개인지 확인 */
	for (; i<size; i++)
	{
		if (',' == data[i])	u32Find++;
	}
	if (u32Find < 4)
	{
		AfxMessageBox(L"Failed to analyse the value from model <mark_recipe> file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 순서대로 읽어서 구조체에 저장 */
	pstRecipe	= (LPG_CMRD)::Alloc(sizeof(STG_CMRD));
	ASSERT(pstRecipe);
	/* 00.recipe name */
	pFind	= strchr(pData, ',');
	if (pFind)
	{
		memcpy(szName, pData, pFind - pData);
		pData = ++pFind;
	}
	/* 01.mark type */
	memset(szValue, 0x00, 32);
	pFind	= strchr(pData, ',');
	if (pFind)
	{
		memcpy(szValue, pData, pFind - pData);
		pData = ++pFind;
		u8MarkType	= (UINT8)atoi(szValue);
	}
	/* 02.mark count */
	memset(szValue, 0x00, 32);
	pFind	= strchr(pData, ',');
	if (pFind)
	{
		memcpy(szValue, pData, pFind - pData);
		pData = ++pFind;
		pstRecipe->count= (UINT8)atoi(szValue);
	}
	/* 모델 정보 읽어들이기 */
	if (pstRecipe->count)
	{
		/* 레시피 내부의 모델 마다 할당될 Align Camera Index 값 저장을 위한 버퍼 할당 */
		pstRecipe->Init(pstRecipe->count);
		strcpy_s(pstRecipe->r_name, MARK_MODEL_NAME_LENGTH, szName);
		pstRecipe->type	= u8MarkType;
		/* 레시피 이름을 저장할 전체 메모리 할당 */
		for (i=0; i<UINT16(pstRecipe->count); i++)
		{
			/* align camera number (1 or 2) */
			memset(szValue, 0x00, 32);
			pFind	= strchr(pData, ',');
			if (pFind)
			{
				memcpy(szValue, pData, pFind - pData);
				pData = ++pFind;
				pstRecipe->acam_num[i]	= (UINT8)atoi(szValue);
			}

			/* Model Name */
			pFind	= strchr(pData, ',');
			/* 마크 모델 이름 저장을 위한 버퍼 할당 및 초기화  */
			memcpy(pstRecipe->m_name[i], pData, pFind - pData);
			pData = ++pFind;
		}
	}

	/* 메모리에 분석된 Model 데이터 등록 */
	m_lstMark.AddTail(pstRecipe);

	return TRUE;
}

/*
 desc : Model 이름 검색
 parm : m_name	- [in]  검색하고자 하는 Model 이름
 retn : TRUE (Finded) or FALSE (Not find)
*/
BOOL CMark::ModelFind(PCHAR m_name)
{
	POSITION pPos	= NULL;

	pPos	= m_lstModel.GetHeadPosition();
	while (pPos)
	{
		if (0 == strcmp(m_name, m_lstModel.GetNext(pPos)->name))	return TRUE;
	}
	return FALSE;
}

/*
 desc : Recipe 이름 검색
 parm : r_name	- [in]  검색하고자 하는 Recipe 이름
 retn : TRUE (Finded) or FALSE (Not find)
*/
BOOL CMark::MarkFind(PCHAR r_name)
{
	POSITION pPos	= NULL;
	pPos	= m_lstMark.GetHeadPosition();
	while (pPos)
	{
		if (0 == strcmp(r_name, m_lstMark.GetNext(pPos)->r_name))	return TRUE;
	}
	return FALSE;
}

/*
 desc : Model 이름 검색를 통한 삭제
 parm : m_name	- [in]  삭제하고자 하는 Model 이름
 retn : TRUE (Deleted) or FALSE (Not delete)
*/
BOOL CMark::RemoveModel(PCHAR m_name)
{
	POSITION pPos	= NULL, pPrePos;
	LPG_CMPV pstData= NULL;

	pPos	= m_lstModel.GetHeadPosition();
	while (pPos)
	{
		/* 방금 전에 찾은 위치 저장 */
		pPrePos	= pPos;
		pstData	= m_lstModel.GetNext(pPos);
		if (pstData && 0 == strcmp(m_name, pstData->name))
		{
			::Free(pstData);
			m_lstModel.RemoveAt(pPrePos);
			return TRUE;
		}
	}
	return FALSE;
}

/*
 desc : Recipe 이름 검색를 통한 삭제
 parm : r_name	- [in]  삭제하고자 하는 Recipe 이름
 retn : TRUE (Deleted) or FALSE (Not delete)
*/
BOOL CMark::RemoveMark(PCHAR r_name)
{
	POSITION pPos	= NULL, pPrePos;
	LPG_CMRD pstData= NULL;

	/* 현재 삭제 대상인 레시피 이름과 선택된 레시피 이름 같은지 확인 */
	if (m_pstSelected && 0 == strcmp(r_name, m_pstSelected->r_name))
	{
		m_pstSelected	= NULL;	/* 선택된 레시피 해제 처리 */
	}

	pPos	= m_lstMark.GetHeadPosition();
	while (pPos)
	{
		/* 방금 전에 찾은 위치 저장 */
		pPrePos	= pPos;
		pstData	= m_lstMark.GetNext(pPos);
		if (0 == strcmp(r_name, pstData->r_name))
		{
			if (pstData->m_name)
			{
				::Free(pstData->m_name[0]);
				::Free(pstData->m_name);
			}
			::Free(pstData);
			m_lstMark.RemoveAt(pPrePos);
			return TRUE;
		}
	}
	return FALSE;
}

/*
 desc : Model 이름을 통한 Model 반환
 parm : m_name	- [in]  검색하고자 하는 Model 이름
 retn : 반환될 Model이 저장된 구조체 포인터
*/
LPG_CMPV CMark::GetModelName(PCHAR m_name)
{
	POSITION pPos	= NULL;
	LPG_CMPV pstData= NULL;

	pPos	= m_lstModel.GetHeadPosition();
	while (pPos)
	{
		pstData	= m_lstModel.GetNext(pPos);
		if (0 == strcmp(m_name, pstData->name))	return pstData;
	}
	return NULL;
}

/*
 desc : Model 이름을 통한 Model 반환
 parm : r_name	- [in]  검색하고자 하는 Recipe 이름
 retn : 반환될 Model가 저장된 구조체 포인터
*/
LPG_CMRD CMark::GetRecipeName(PCHAR r_name)
{
	POSITION pPos	= NULL;
	LPG_CMRD pstData= NULL;

	pPos	= m_lstMark.GetHeadPosition();
	while (pPos)
	{
		pstData	= m_lstMark.GetNext(pPos);
		if (0 == strcmp(r_name, pstData->r_name))	return pstData;
	}
	return NULL;
}

/*
 desc : Model가 저장된 인덱스 위치에 있는 Model 반환
 parm : index	- [in]  가져오고자 하는 Model 인덱스 (Zero-based)
 retn : 반환될 Model이 저장된 구조체 포인터
*/
LPG_CMPV CMark::GetModelIndex(UINT8 index)
{
	POSITION pPos	= NULL;

	if (index >= m_lstModel.GetCount())	return NULL;

	/* 해당 위치 값 얻기 */
	pPos	= m_lstModel.FindIndex(index);
	if (!pPos)	return NULL;

	/* 위치에 해당되는 결과 값 반환 */
	return m_lstModel.GetAt(pPos);
}

/*
 desc : Recipe가 저장된 인덱스 위치에 있는 Model 반환
 parm : index	- [in]  가져오고자 하는 Recipe 인덱스 (Zero-based)
 retn : 반환될 Model가 저장된 구조체 포인터
*/
LPG_CMRD CMark::GetRecipeIndex(UINT8 index)
{
	POSITION pPos	= NULL;

	if (index >= m_lstMark.GetCount())	return NULL;

	/* 해당 위치 값 얻기 */
	pPos	= m_lstMark.FindIndex(index);
	if (!pPos)	return NULL;

	/* 위치에 해당되는 결과 값 반환 */
	return m_lstMark.GetAt(pPos);
}

/*
 desc : Model가 저장된 인덱스 위치에 있는 Model 반환
 parm : index	- [in]  가져오고자 하는 Model 인덱스 (Zero-based)
 retn : 모델 이름 반환
*/
PCHAR CMark::GetModelName(UINT8 index)
{
	POSITION pPos	= NULL;

	if (index >= m_lstModel.GetCount())	return NULL;

	/* 해당 위치 값 얻기 */
	pPos	= m_lstModel.FindIndex(index);
	if (!pPos)	return NULL;

	/* 위치에 해당되는 결과 값 반환 */
	return m_lstModel.GetAt(pPos)->name;
}

/*
 desc : Recipe가 저장된 인덱스 위치에 있는 Model 반환
 parm : index	- [in]  가져오고자 하는 Model 인덱스 (Zero-based)
 retn : 모델 이름 반환
*/
PCHAR CMark::GetRecipeName(UINT8 index)
{
	POSITION pPos	= NULL;

	if (index >= m_lstMark.GetCount())	return NULL;

	/* 해당 위치 값 얻기 */
	pPos	= m_lstMark.FindIndex(index);
	if (!pPos)	return NULL;

	/* 위치에 해당되는 결과 값 반환 */
	return m_lstMark.GetAt(pPos)->r_name;
}

/*
 desc : Mark Model & Recipe 저장
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMark::SaveFile()
{
	return SaveModel() && SaveRecipe();
}

/*
 desc : Recipe Save
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMark::SaveRecipe()
{
	CHAR szData[256]	= {NULL};
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	UINT16 i;
	errno_t eRet		= 0;
	FILE *fp			= NULL;
	POSITION pPos		= NULL;
	LPG_CMRD pstRecipe	= NULL;
	CUniToChar csCnv;

	/* Model File 설정 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\data\\recipe\\%s.dat",
			   m_tzWorkDir, GetConfig()->file_dat.mark_recipe);
	/* 파일 열기 */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "wt");
	if (0 != eRet)	return TRUE;
	
	/* 주석 입력 */
	fputs("; Recipe Name,Mark Type (0:Geomatrix,1:Pattern Image),Mark Count,Align Camera Number (1 or 2),Mark Model Name (Pattern File),,,\n", fp);
	pPos	= m_lstMark.GetHeadPosition();
	while (pPos)
	{
		/* 값 가져오기 */
		pstRecipe	= m_lstMark.GetNext(pPos);
		/* 레시피 검증 */
		if (strlen(pstRecipe->r_name) > 0 && pstRecipe->count > 0)
		{
			/* 레시피 이름 저장 */
			fputs(pstRecipe->r_name, fp);
			fputs(",", fp);
			/* 마크 구분 (Type) 저장 */
			sprintf_s(szData, 256, "%u,", pstRecipe->type);
			fputs(szData, fp);
			/* 등록된 모델 개수 저장 */
			sprintf_s(szData, 256, "%u,", pstRecipe->count);
			fputs(szData, fp);
			/* 모델 정보 저장 */
			for (i=0; i<pstRecipe->count; i++)
			{
				sprintf_s(szData, 256,	"%u,", pstRecipe->acam_num[i]);	fputs(szData, fp);
				sprintf_s(szData, 256,	"%s,", pstRecipe->m_name[i]);	fputs(szData, fp);
			}
			fputs("\n", fp);
		}
	}
	/* 파일 핸들 닫기 */
	fclose(fp);

	return TRUE;
}

/*
 desc : Recipe 간의 구조체 값 복사 (구조체 내부에 할당된 메모리가 항목이 많으므로)
 parm : source	- [in]  원래 값이 저장된 곳
		target	- [in]  새로운 값이 저장(복사)될 곳
 retn : None
*/
VOID CMark::CopyRecipe(LPG_CMRD source, LPG_CMRD target)
{
	UINT8 i	= 0x00;

	/* 메모리 생성 */
	target->Init(source->count);

	/* 멤버 변수 값 복사 */
	target->type	= source->type;
	target->count	= source->count;
	memcpy(target->r_name, source->r_name, strlen(source->r_name));
	memcpy(target->acam_num, source->acam_num, source->count);
	for (; i<source->count; i++)
	{
		memcpy(target->m_name[i], source->m_name[i], strlen(source->m_name[i]));
	}
}

/*
 desc : Model Append
 parm : data	- [in]  Model 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE (개수 초과. 최대 15개)
*/
BOOL CMark::ModelAppend(LPG_CMPV value)
{
	LPG_CMPV pstValue	= NULL;

	if (m_lstModel.GetCount() >= MAX_REGIST_MODEL)
	{
		AfxMessageBox(L"The registered Model is full (MAX. 30)", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 필수로 입력되는 값이 있는지 검토 */
	if (!value->IsValid())
	{
		AfxMessageBox(L"The input data is invalid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 기존에 등록된 Model가 있는지 확인 */
	if (ModelFind(value->name))
	{
		AfxMessageBox(L"There is a name for the same model", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 맨 마지막 위치에 등록 */
	pstValue	= (LPG_CMPV)::Alloc(sizeof(STG_CMPV));
	ASSERT(pstValue);
	pstValue->Reset();
	memset(pstValue->name, 0x00, MARK_MODEL_NAME_LENGTH);
	/* 구조체 내에 메모리 할당된 항목은 별도로 복사 수행 */
	memcpy(pstValue, value, sizeof(STG_CMPV));
	/* 등록 */
	m_lstModel.AddTail(pstValue);

	/* 현재까지 등록된 모든 Model 내용을 파일로 저장 */
	return SaveModel();
}

/*
 desc : Recipe Append
 parm : recipe	- [in]  Model 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE (개수 초과. 최대 15개)
*/
BOOL CMark::MarkAppend(LPG_CMRD recipe)
{
	LPG_CMRD pstRecipe	= NULL;

	if (m_lstMark.GetCount() >= MAX_REGIST_RECIPE_COUNT)
	{
		AfxMessageBox(L"The registered Recipe is full", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 필수로 입력되는 값이 있는지 검토 */
	if (!recipe->IsValid())
	{
		AfxMessageBox(L"The input data is invalid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 기존에 등록된 Model가 있는지 확인 */
	if (MarkFind(recipe->r_name))
	{
		AfxMessageBox(L"There is a name for the same recipe", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 맨 마지막 위치에 등록 */
	pstRecipe	= (LPG_CMRD)::Alloc(sizeof(STG_CMRD));
	ASSERT(pstRecipe);
	pstRecipe->Init(recipe->count);
	if (!pstRecipe)	return FALSE;
	/* 새로 생성된 구조체 영역에 기존 설정된 값 복사 */
	CopyRecipe(recipe, pstRecipe);
	/* 등록 */
	m_lstMark.AddTail(pstRecipe);

	/* 현재까지 등록된 모든 Model 내용을 파일로 저장 */
	return SaveRecipe();
}

/*
 desc : Model Modify
 parm : value	- [in]  Model 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
BOOL CMark::ModelModify(LPG_CMPV value)
{
	LPG_CMPV pstValue	= NULL;

	/* 필수로 입력되는 값이 있는지 검토 */
	if (!value->IsValid())
	{
		AfxMessageBox(L"The update data is invalid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* Mark Recipe에 수정 대상의 Model이 등록되어 있는지 확인 */
	if (FindModelInMark(value->name))
	{
		AfxMessageBox(L"The model name exists inside the mark file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 기존에 등록된 Model가 있는지 확인 */
	pstValue = GetModelName(value->name);
	if (!pstValue)
	{
		AfxMessageBox(L"The model name was not found in the model file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 구조체 내에 메모리 할당된 항목은 별도로 복사 수행 */
	memcpy(pstValue, value, sizeof(STG_CMPV));

	/* 현재까지 등록된 모든 Model 내용을 파일로 저장 */
	return SaveModel();
}

/*
 desc : Model Modify
 parm : recipe	- [in]  Model 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
BOOL CMark::MarkModify(LPG_CMRD recipe)
{
	LPG_CMRD pstRecipe	= NULL;

	/* 필수로 입력되는 값이 있는지 검토 */
	if (!recipe->IsValid())
	{
		AfxMessageBox(L"The update data is invalid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 기존 수정 대상이 현재 선택된 레시피와 동일한지 여부 */
	if (m_pstSelected && 0 == strcmp(recipe->r_name, m_pstSelected->r_name))
	{
		AfxMessageBox(L"The currently selected recipe cannot be deleted", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 기존에 등록된 Model가 있는지 확인 */
	pstRecipe = GetRecipeName(recipe->r_name);
	if (!pstRecipe)
	{
		AfxMessageBox(L"The recipe name was not found in the mark file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 새로 생성된 구조체 영역에 기존 설정된 값 복사 */
	CopyRecipe(recipe, pstRecipe);

	/* 현재까지 등록된 모든 Model 내용을 파일로 저장 */
	return SaveRecipe();
}

/*
 desc : Model Modify
 parm : m_name	- [in]  제거하려는 Model의 이름
 retn : TRUE or FALSE
*/
BOOL CMark::ModelDelete(PCHAR m_name)
{
	/* 기존에 등록된 Model가 있는지 확인 */
	if (!GetModelName(m_name))
	{
		AfxMessageBox(L"The model name was not found in the model file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* Mark Recipe에 삭제 대상의 Model이 등록되어 있는지 확인 */
	if (FindModelInMark(m_name))
	{
		AfxMessageBox(L"The model name exists inside the mark file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* Model 삭제 수행 */
	RemoveModel(m_name);

	/* 현재까지 등록된 모든 Model 내용을 파일로 저장 */
	return SaveModel();
}

/*
 desc : Model Modify
 parm : r_name	- [in]  제거하려는 Recipe의 이름
 retn : TRUE or FALSE
*/
BOOL CMark::MarkDelete(PCHAR r_name)
{
	/* 삭제 대상이 현재 선택된 레시피와 동일한지 여부 */
	if (m_pstSelected && 0 == strcmp(r_name, m_pstSelected->r_name))
	{
		AfxMessageBox(L"The currently selected recipe cannot be deleted", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* 기존에 등록된 Model가 있는지 확인 */
	if (!GetRecipeName(r_name))
	{
		AfxMessageBox(L"The recipe name was not found in the mark file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* Model 삭제 수행 */
	RemoveMark(r_name);

	/* 현재까지 등록된 모든 Model 내용을 파일로 저장 */
	return SaveRecipe();
}

/*
 desc : 모델 타입에 따른 문자열 반환
 parm : type	- [in]  모델 타입 코드 값
 retn : 문자열 반환
*/
PTCHAR CMark::GetModelTypeToStr(UINT32 type)
{
	switch (type)
	{
	case ENG_MMDT::en_none		:	return L"None";
	case ENG_MMDT::en_circle	:	return L"Circle";
	case ENG_MMDT::en_ellipse	:	return L"Ellipse";
	case ENG_MMDT::en_square	:	return L"Square";
	case ENG_MMDT::en_rectangle	:	return L"Rect";
	case ENG_MMDT::en_ring		:	return L"Ring";
	case ENG_MMDT::en_cross		:	return L"Cross";
	case ENG_MMDT::en_diamond	:	return L"Diamond";
	case ENG_MMDT::en_triangle	:	return L"Triangle";
	case ENG_MMDT::en_image		:	return L"Image";
	}
	return L"";
}

/*
 desc : 레시피 이름을 통한 레시피 선택
 parm : r_name	- [in]  Mark 레시피 이름
 retn : TRUE or FALSE
*/
BOOL CMark::SelRecipeName(PCHAR r_name)
{
	LPG_CMRD pstRecipe	= GetRecipeName(r_name);
	if (!pstRecipe)	return FALSE;

	/* 레시피 선택 설정 */
	m_pstSelected	= pstRecipe;

	return TRUE;
}

/*
 desc : Mark Recipe 내에 검색 대상의 Model Name이 존재하는지 여부
 parm : m_name	- [in]  찾고자하는 Model Name
 retn : TRUE or FALSE
*/
BOOL CMark::FindModelInMark(PCHAR m_name)
{
	UINT8 i, j, u8Count	= (UINT8)m_lstMark.GetCount();
	LPG_CMRD pstRecipe	= NULL;

	for (i=0; i<u8Count; i++)
	{
		pstRecipe	= GetRecipeIndex(i);
		for (j=0; j<pstRecipe->count; j++)
		{
			if (0 == strcmp(pstRecipe->m_name[j], m_name))	return TRUE;
		}
	}

	return FALSE;
}
/* ----------------------------------------------------------------------------------------- */
/*                                 lk91 VISION 추가 함수                                     */
/* ----------------------------------------------------------------------------------------- */

/* desc : Mark 정보 file Save */
BOOL CMark::SaveModel() 
{
	CHAR szData[256] = { NULL };
	TCHAR tzFile[MAX_PATH_LEN] = { NULL };
	errno_t eRet = 0;
	FILE* fp = NULL;
	POSITION pPos = NULL;
	LPG_CMPV pstModel = NULL;
	CUniToChar csCnv;

	/* Model File 설정 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\data\\recipe\\%s.dat",
		m_tzWorkDir, GetConfig()->file_dat.mark_model);
	/* 파일 열기 */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "wt");
	if (0 != eRet)	return TRUE;

	/* 주석 입력 */
	fputs("; Model Name, Model Type, Find Type, File Name OR Param 1, Param 2, Param 3, Param 4, Param 5,\n", fp);

	pPos = m_lstModel.GetHeadPosition();
	while (pPos)
	{
		/* 값 가져오기 */
		pstModel = m_lstModel.GetNext(pPos);
		/* 저장 : 01 ~ 07 */
		if (pstModel->find_type == 1 || ENG_MMDT(pstModel->type) == ENG_MMDT::en_image)
		{
			sprintf_s(szData, 256, "%s,%u,%u,%s,%d,%d,%d,%d,",
				pstModel->name, pstModel->type, pstModel->find_type, pstModel->file,
				pstModel->iSizeP.x, pstModel->iSizeP.y, pstModel->iOffsetP.x, pstModel->iOffsetP.y);
		}
		else
		{
			sprintf_s(szData, 256, "%s,%u,%u,%.1f,%.4f,%.4f,%.4f,%.4f,",
				pstModel->name,
				pstModel->type,
				pstModel->find_type,
				pstModel->param[0],
				pstModel->param[1],
				pstModel->param[2],
				pstModel->param[3],
				pstModel->param[4]);
		}
		fputs(szData, fp);
		fputs("\n", fp);
	}
	/* 파일 핸들 닫기 */
	fclose(fp);

	return TRUE;
}

/* desc : Set Search ROI */
BOOL CMark::SetMarkROI(LPG_CRD data, UINT8 index)
{
	m_pstROI->roi_Left[index] = data->roi_Left[index];
	m_pstROI->roi_Right[index] = data->roi_Right[index];
	m_pstROI->roi_Top[index] = data->roi_Top[index];
	m_pstROI->roi_Bottom[index] = data->roi_Bottom[index];

	return TRUE;
}

/* desc : Get Search ROI */
LPG_CRD CMark::GetMarkROI()
{
	return m_pstROI;
}

/* desc : Search ROI Save */
BOOL CMark::SaveROI()
{
	CHAR szData[256] = { NULL };
	TCHAR tzFile[MAX_PATH_LEN] = { NULL };
	UINT16 i;
	errno_t eRet = 0;
	FILE* fp = NULL;
	POSITION pPos = NULL;
	CUniToChar csCnv;

	/* Model File 설정 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\data\\recipe\\%s.dat",
		m_tzWorkDir, GetConfig()->file_dat.mark_roi);
	/* 파일 열기 */
	eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "wt");
	if (0 != eRet)	return TRUE;

	/* 주석 입력 */
	fputs("; ROI_LEFT, ROI_RIGHT, ROI_TOP, ROI_BOTTOM,,,\n", fp);

	for (int i = 0; i < 2; i++) {
		sprintf_s(szData, 256, "%d,%d,%d,%d,\n",
			m_pstROI->roi_Left[i],
			m_pstROI->roi_Right[i],
			m_pstROI->roi_Top[i],
			m_pstROI->roi_Bottom[i]);

		fputs(szData, fp);
	}

	/* 파일 핸들 닫기 */
	fclose(fp);

	return TRUE;
}


/* desc : Search ROI 등록 진행 */
BOOL CMark::ParseSearchROI(PCHAR data, UINT32 size, UINT8 cnt)
{
	UINT8 u8MarkType = 0x00;
	UINT32 u32Find = 0, i = 0;
	CHAR* pData = data, * pFind, szValue[32], szName[MARK_MODEL_NAME_LENGTH] = { NULL };

	for (; i < size; i++)
	{
		if (',' == data[i])	u32Find++;
	}
	if (u32Find < 4)
	{
		AfxMessageBox(L"Failed to analyse the value from model <search_roi> file", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}

	if (!m_pstROI)
		m_pstROI = (LPG_CRD)::Alloc(sizeof(STG_CRD));
	ASSERT(m_pstROI);

	memset(szValue, 0x00, 32);
	pFind = strchr(pData, ',');
	if (pFind)
	{
		memcpy(szValue, pData, pFind - pData);
		pData = ++pFind;
		m_pstROI->roi_Left[cnt] = (INT32)atoi(szValue);
	}
	memset(szValue, 0x00, 32);
	pFind = strchr(pData, ',');
	if (pFind)
	{
		memcpy(szValue, pData, pFind - pData);
		pData = ++pFind;
		m_pstROI->roi_Right[cnt] = (INT32)atoi(szValue);
	}
	memset(szValue, 0x00, 32);
	pFind = strchr(pData, ',');
	if (pFind)
	{
		memcpy(szValue, pData, pFind - pData);
		pData = ++pFind;
		m_pstROI->roi_Top[cnt] = (INT32)atoi(szValue);
	}
	memset(szValue, 0x00, 32);
	pFind = strchr(pData, ',');
	if (pFind)
	{
		memcpy(szValue, pData, pFind - pData);
		pData = ++pFind;
		m_pstROI->roi_Bottom[cnt] = (INT32)atoi(szValue);
	}

	/* 메모리에 분석된 Model 데이터 등록 */
	return TRUE;
}
