
/*
 desc : Led Power Data 적재 및 관리
*/

#include "pch.h"
#include "UserPosition.h"


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
CUserPosition::CUserPosition(TCHAR *work_dir)
	: CBase(work_dir)
{
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CUserPosition::~CUserPosition()
{
	RemoveAll();
}

/*
 desc : 기존 등록된 User Position 모두 제거
 parm : None
 retn : None
*/
VOID CUserPosition::RemoveAll()
{
	POSITION pPos	= NULL;
	LPG_UPTP pPower	= NULL;

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
BOOL CUserPosition::LoadFile()
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
			   m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.user_position);
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

BOOL CUserPosition::SaveFile()
{
	CStdioFile sFile;
	CString strPath, strLine;

	strPath.Format(_T("%s\\%s\\recipe\\%s.dat"), m_tzWorkDir, CUSTOM_DATA_CONFIG, GetConfig()->file_dat.user_position);
	if (false == sFile.Open(strPath, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
	{
		return FALSE;
	}

	//1. 최상단 헤더 작성
	sFile.WriteString(_T(";Name, StageX, StageY, Cam1X, Cam2X(mm)\n"));
	//2. 리스트를 순회하며 데이터 작성
	POSITION pPos = m_lstRecipe.GetHeadPosition();
	while (pPos)
	{
		LPG_UPTP pstData = m_lstRecipe.GetNext(pPos);
		if (pstData)
		{
			strLine.Format(_T("%s,"), pstData->strName);
			sFile.WriteString(strLine);

			auto WriteCoordinate = [&](double dVal) {
				if (dVal < 0.0) sFile.WriteString(_T("-,"));
				else {
					CString strVal;
					strVal.Format(_T("%.4f,"), dVal);
					sFile.WriteString(strVal);

				}
			};

			WriteCoordinate(pstData->dPosX);
			WriteCoordinate(pstData->dPosY);
			WriteCoordinate(pstData->dCam1X);
			WriteCoordinate(pstData->dCam2X);
			//sFile.WriteString(strLine);
			sFile.WriteString(_T("\n"));
		}
	}
	sFile.Close();
	return TRUE;
}

/*
 desc : 한 줄의 문자열을 콤마 기준으로 분석하여 리스트에 등록
 parm : data	- [in]  읽어들인 ANSI 문자열 버퍼
		size	- [in]  'data' 버퍼에 저장된 데이터의 길이
 retn : TRUE or FALSE
*/
BOOL CUserPosition::ParseAppend(CHAR *data, UINT32 size)
{
	CString strLine(data);
	strLine.TrimRight(_T("\r\n"));

	int nCommaCount = 0;
	for (int i = 0;i < strLine.GetLength();i++)
	{
		if (strLine[i] == ',') nCommaCount++;
	}
	if (nCommaCount < 4)
	{
		AfxMessageBox(_T("Failed to analyse the value from user_position file (Invalid Format)"), MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}

	/*구조체 할당 및 초기화*/
	LPG_UPTP pstData = new STG_UPTP();
	ASSERT(pstData);
	pstData->Init();

	int nPos = 0;
	CString strToken;

	//값이 "-" 이면 DEF_IGNORE_POS(-1.0)할당
	auto ParseCoordinate = [](CString str)->double {
		str.Trim();
		if (str == _T("-")) return DEF_IGNORE_POS;
		return _ttof(str);
		};

 	strToken = strLine.Tokenize(_T(","), nPos);
	pstData->strName = strToken.Trim();
	if (nPos != -1) {
		strToken = strLine.Tokenize(_T(","), nPos);
		pstData->dPosX = ParseCoordinate(strToken);
	}
	if (nPos != -1) {
		strToken = strLine.Tokenize(_T(","), nPos);
		pstData->dPosY = ParseCoordinate(strToken);
	}
	if (nPos != -1) {
		strToken = strLine.Tokenize(_T(","), nPos);
		pstData->dCam1X = ParseCoordinate(strToken);
	}
	if (nPos != -1) {
		strToken = strLine.Tokenize(_T(","), nPos);
		pstData->dCam2X = ParseCoordinate(strToken);
	}

	m_lstRecipe.AddTail(pstData);

	return TRUE;
}

LPG_UPTP CUserPosition::GetUserPosData(int nIndex)
{
	//인덱스 범위 체크
	if (nIndex < 0 || nIndex >= m_lstRecipe.GetCount())
	{
		return NULL;
	} 

	//해당 인덱스의 POSITION 값을 찾아 데이터 변환
	POSITION pos = m_lstRecipe.FindIndex(nIndex);
	return m_lstRecipe.GetAt(pos);
}

VOID CUserPosition::SetUserPosData(int nIndex, LPCTSTR strName,
	double dPosX, double dPosY, double dCam1X, double dCam2X) 
{
	while ((int)m_lstRecipe.GetCount() <= nIndex)
	{
		LPG_UPTP pNewData = new STG_UPTP();
		pNewData->Init();
		m_lstRecipe.AddTail(pNewData);
	}

	POSITION pos = m_lstRecipe.FindIndex(nIndex);
	if (pos)
	{
		LPG_UPTP pData = m_lstRecipe.GetAt(pos);
		pData->strName = strName;
		pData->dPosX = dPosX;
		pData->dPosY = dPosY;
		pData->dCam1X = dCam1X;
		pData->dCam2X = dCam2X;
	}
}