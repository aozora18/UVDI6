
/*
 desc : Led Power Data 적재 및 관리
*/

#include "pch.h"
#include "LedPower.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : None
 retn : None
*/
CLedPower::CLedPower(TCHAR *work_dir)
	: CBase(work_dir)
{
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CLedPower::~CLedPower()
{
	RemoveAll();
}

/*
 desc : 기존 등록된 Led Power 모두 제거
 parm : None
 retn : None
*/
VOID CLedPower::RemoveAll()
{
	POSITION pPos	= NULL;
	LPG_PLPI pPower	= NULL;

	/* 기존 등록된 Recipe Data 메모리 해제 */
	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		pPower	= m_lstRecipe.GetNext(pPos);
		if (pPower)	delete pPower;
	}
	m_lstRecipe.RemoveAll();
}

/*
 desc : Led Power 적재
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLedPower::LoadFile()
{
	CHAR szData[1024]			= {NULL};
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	errno_t eRet				= 0;
	FILE *fp					= NULL;
	CUniToChar csCnv;

	/* 기존 데이터 모두 제거 */
	RemoveAll();

	/* Recipe File 설정 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\recipe\\%s.dat",
			   m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.led_power);
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
			ParseAppend(szData, (UINT32)strlen(szData));
		}
	}

	/* 파일 핸들 닫기 */
	fclose(fp);

	return TRUE;
}


/*
 desc : Led Power 저장
 parm : strData	- [in]  저장할 Led Power 정보가 저장된 문자열
 retn : TRUE or FALSE
*/
BOOL CLedPower::SaveFile(CString strData)
{
	/* 공백 제거 */
	strData.TrimLeft();
	strData.TrimRight();

	int nCommaCount = (MAX_PH * /*MAX_LED*/4) * 2 + 1;
	int nCount = (int)std::count((LPCTSTR)strData, (LPCTSTR)strData + strData.GetLength(), _T(','));

	/* 문장 검사 */
	if (nCommaCount != nCount)
	{
		/* 불완전 문장 */
		return FALSE;
	}

	else if (_T("\n") != strData.Right(2))
	{
		/* 마지막엔 반드시 줄바꿈 문자가 존재해야 한다. */
		strData += _T("\n");
	}

	CStdioFile sFile;
	CString strPath;

	strPath.Format(_T("%s\\%s\\recipe\\%s.dat"), m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.led_power);
	if (FALSE == sFile.Open(strPath, CFile::modeCreate | CFile::modeReadWrite | CFile::modeNoTruncate | CFile::typeText))
	{
		/* 파일을 열 수 없음 */
		return FALSE;
	}

	sFile.SeekToEnd();
	sFile.WriteString(strData);
	sFile.Close();

	return TRUE;
}


/*
 desc : Led Power 저장
 parm : strData	- [in]  저장할 Led Power 구조체
 retn : TRUE or FALSE
*/
BOOL CLedPower::SaveFile(LPG_PLPI pstData)
{
	CString strLine;
	CString strText;

	strLine = pstData->led_name;

	for (int nPH = 0; nPH < MAX_PH; nPH++)
	{
		for (int nLED = 0; nLED < /*MAX_LED*/4; nLED++)
		{
			if (0 == pstData->led_index[nPH][nLED])
			{
				strText = _T(",,");
			}
			else
			{
				strText.Format(_T(",%.3f,%d"), pstData->led_watt[nPH][nLED], pstData->led_index[nPH][nLED]);
			}

			strLine += strText; 
		}
	}

	strLine += _T(",");
	return SaveFile(strLine);
}


/*
 desc : Recipe 분석 및 등록 진행
 parm : data	- [in]  Led Power 정보가 저장된 문자열 버퍼
		size	- [in]  'data' 버퍼에 저장된 데이터의 길이
 retn : TRUE or FALSE
*/
BOOL CLedPower::ParseAppend(CHAR *data, UINT32 size)
{
	UINT32 i, j, k, u32Find = 0;
	CHAR *pData	= data, *pFind, szValue[256];
	LPG_PLPI pstData	= {NULL};

	/* 일단, 주어진 문자열 중에서 콤마(',') 개수가 9개인지 확인 */
	for (i=0; i<size; i++)
	{
		if (',' == data[i])	u32Find++;
	}
	if (u32Find != 65)
	{
		AfxMessageBox(L"Failed to analyse the value from led_power file", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 순서대로 읽어서 구조체에 저장 */
	pstData	= new STG_PLPI();
	ASSERT(pstData);
	/* 메모리 할당 및 초기화 */
	pstData->Init();
	/* 01.led power name */
	pFind	= strchr(pData, ',');
	if (pFind)	{	memcpy(pstData->led_name, pData, pFind - pData);	pData = ++pFind;	}

	for (i=0,j=0,k=0; i<64 && pFind; i++)
	{
		pFind	= strchr(pData, ',');
		memset(szValue, 0x00, 256);
		memcpy(szValue, pData, pFind - pData);
		pData	= ++pFind;
		/* Led Power (W) */
		if (0 == (i%2))
		{
			pstData->led_watt[k][j] = (FLOAT)atof(szValue);
		}
		else
		{
			pstData->led_index[k][j] = (UINT16)atoi(szValue);
			j++;
		}
		/* 4개씩 모두 처리 했으면 */
		if (4 == j)
		{
			j = 0;	/* Photohead 내 Led Number 증가 */
			k++;	/* Photohead Number 증가 */
		}
	}
	/* 분석된 데이터를 리스트 버퍼에 저장 */
	if (64 != i)
	{
		AfxMessageBox(L"Failed to register the recipe data in list memory", MB_ICONSTOP|MB_TOPMOST);
		delete pstData;
		return FALSE;
	}

	/* 메모리에 분석된 레시피 데이터 등록 */
	m_lstRecipe.AddTail(pstData);

	return TRUE;
}

/*
 desc : Led Power 이름 검색
 parm : name	- [in]  검색하고자 하는 레시피 이름
 retn : TRUE (Finded) or FALSE (Not find)
*/
BOOL CLedPower::FindLedPower(CHAR *name)
{
	POSITION pPos	= NULL;

	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		if (0 == strcmp(name, m_lstRecipe.GetNext(pPos)->led_name))	return TRUE;
	}
	return FALSE;
}

/*
 desc : Led Power 이름을 통해 몇 번째 저장되어 있는지
 parm : name	- [in]  검색하고자 하는 Led Power 이름
 retn : 리스트에 저장되어 있는 위치 반환 (음수이면 검색 실패)
*/
INT32 CLedPower::GetLedPowerPos(CHAR *name)
{
	UINT32 u32Index	= 0;
	POSITION pPos	= NULL;

	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		if (0 == strcmp(name, m_lstRecipe.GetNext(pPos)->led_name))
		{
			return u32Index;
		}
		u32Index++;
	}
	return -1;
}

/*
 desc : Led Power 이름을 통한 레시피 반환
 parm : name	- [in]  검색하고자 하는 Led Power 이름
 retn : 반환될 Led Power 저장된 구조체 포인터
*/
LPG_PLPI CLedPower::GetLedPowerName(CHAR *name)
{
	POSITION pPos	= NULL;
	LPG_PLPI pstData= NULL;

	pPos	= m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		pstData	= m_lstRecipe.GetNext(pPos);
		if (0 == strcmp(name, pstData->led_name))	return pstData;
	}
	return NULL;
}

/*
 desc : Led Power가 저장된 인덱스 위치에 있는 Led Power 반환
 parm : index	- [in]  가져오고자 하는 Led Power 인덱스 (Zero-based)
 retn : 반환될 Led Power가 저장된 구조체 포인터
*/
LPG_PLPI CLedPower::GetLedPowerIndex(UINT16 index)
{
	POSITION pPos	= NULL;

	/* 등록된 개수보다 가져오고자 하는 위치 값이 큰 경우 */
	if (index >= m_lstRecipe.GetCount())	return NULL;

	/* 해당 위치 값 얻기 */
	pPos	= m_lstRecipe.FindIndex(index);
	if (!pPos)	return NULL;

	/* 위치에 해당되는 결과 값 반환 */
	return m_lstRecipe.GetAt(pPos);
}
