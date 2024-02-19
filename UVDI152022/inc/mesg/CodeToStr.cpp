
/*
 desc : Code To String (에러 혹은 상태 코드에 대한 문자열 값 반환)
*/

#include "pch.h"
#include "CodeToStr.h"


#ifdef _DEBUG
#define new	DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : work	- [in]  작업 기본 경로 (Full path)
 retn : None
*/
CCodeToStr::CCodeToStr(TCHAR *work_dir)
{
	wmemset(m_tzWorkPath, 0x00, MAX_PATH_LEN);
	if (work_dir)	wcscpy_s(m_tzWorkPath, MAX_PATH_LEN, work_dir);

	/* Vector 크기 초기화 */
	m_vMC2Err.assign(512, L"");
	m_vLuriaReg.assign(128, L"");
	m_vLuriaSys.assign(128, L"");
	m_vLuriaPh.assign(128, L"");
	m_vLuriaSts1.assign(64, L"");
	m_vLuriaSts2.assign(64, L"");
	m_vPlcZErr.assign(3000, L"");
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CCodeToStr::~CCodeToStr()
{
	/* Error Messages 메모리 해제 */
	if (!m_vMC2Err.empty())		m_vMC2Err.clear();
	if (!m_vLuriaReg.empty())	m_vLuriaReg.clear();
	if (!m_vLuriaSys.empty())	m_vLuriaSys.clear();
	if (!m_vLuriaPh.empty())	m_vLuriaPh.clear();
	if (!m_vLuriaSts1.empty())	m_vLuriaSts1.clear();
	if (!m_vLuriaSts2.empty())	m_vLuriaSts2.clear();
	if (!m_vPlcZErr.empty())	m_vPlcZErr.clear();
}

/*
 desc : MC2 Error 정보 적재
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCodeToStr::LoadFile()
{
	BOOL bSucc			= TRUE;
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL}, tzMesg[128] = {NULL};
	TCHAR tzName[7][16]	= { L"mc2", L"luria_reg", L"luria_sys", L"luria_ph",
							L"luria_sts1", L"luria_sts2", L"plc_z_axis" };
	UINT16 u16Index	= 0, i, u16Len, u16Start[7] = { 0, 0, 10000, 10000, 0, 0, 0 };
	PCHAR pszData		= (PCHAR)::Alloc(1024);
	CHAR *pszNext		= pszData, *pszFind, szCode[8];
	errno_t eRet		= 0;
	FILE *fp			= NULL;
	CUniToChar csCnv;

	for (i=0; i<7; i++)
	{
		/* 읽어들이 내용이 저장된 파일명 설정 */
		//swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\data\\errc\\%s.err", m_tzWorkPath, tzName[i]);
		swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\%s\\errc\\%s.err", m_tzWorkPath, CUSTOM_DATA_CONFIG, tzName[i]);

		/* 파일 열기 */
		eRet = fopen_s(&fp, csCnv.Uni2Ansi(tzFile), "rt");
		if (0 != eRet)	return TRUE;
		while (!feof(fp))
		{
			/* 기존 버퍼 초기화 */
			memset(pszNext, 0x00, 1024);
			memset(szCode, 0x00, 8);
			/* 한줄 읽어오기 */
			fgets(pszNext, 1024, fp);
			/* 문자열 길이가 특정 개수 이하인 경우인지 (만약 첫 글자가 주석인 세미콜론(;)이면 Skip) */
			if (strlen(pszNext) > 0 && ';' != pszNext[0] && '\n' != pszNext[0])
			{
				/* 등호 ('=') 문자 검색 */
				pszFind	= strchr(pszNext, '=');
				if (!pszFind)
				{
					bSucc	= FALSE;
					break;
				}

				/* Error Code */
				memcpy(szCode, pszNext, pszFind - pszNext);	pszFind++;
				/* 에러 코드 위치에 해당되는 에러 문자열 추가 */
				u16Index	= (UINT16)atoi(szCode) - u16Start[i];
				if (u16Index < 353)
				{
					u16Len	= (UINT16)strlen(pszFind);
					pszFind[u16Len-1]	= NULL;
					switch (i)
					{
					case 0x00 : m_vMC2Err[u16Index]		= std::wstring(csCnv.Ansi2Uni(pszFind)); break;
					case 0x01 : m_vLuriaReg[u16Index]	= std::wstring(csCnv.Ansi2Uni(pszFind)); break;
					case 0x02 : m_vLuriaSys[u16Index]	= std::wstring(csCnv.Ansi2Uni(pszFind)); break;
					case 0x03 : m_vLuriaPh[u16Index]	= std::wstring(csCnv.Ansi2Uni(pszFind)); break;
					case 0x04 : m_vLuriaSts1[u16Index]	= std::wstring(csCnv.Ansi2Uni(pszFind)); break;
					case 0x05 : m_vLuriaSts2[u16Index]	= std::wstring(csCnv.Ansi2Uni(pszFind)); break;
					case 0x06 : m_vPlcZErr[u16Index]	= std::wstring(csCnv.Ansi2Uni(pszFind)); break;
					}
				}
			}
		}
		if (!bSucc)
		{
			swprintf_s(tzMesg, 128, L"Failed to load the error file for %s", tzName[i]);
			AfxMessageBox(tzMesg, MB_ICONSTOP|MB_TOPMOST);
			break;
		}
	}

	/* 파일 핸들 닫기 */
	fclose(fp);

	/* 메모리 해제 */
	if (pszData)	::Free(pszData);

	return bSucc;
}

/*
 desc : 코드에 대한 문자열 반환
 parm : type	- [in]  시스템 코드 (Type; ENG_SCTS)
		err_cd	- [in]  에러 코드 값
 retn : 문자열 반환
*/
PTCHAR CCodeToStr::GetMesg(ENG_SCTS type, UINT16 err_cd)
{
	PTCHAR ptzStr	= NULL;

	/* 코드 값이 유효 범위를 벗어난 경우라면 ... */
	switch (type)
	{
	case ENG_SCTS::en_mc2_error			:
		if (err_cd < m_vMC2Err.size())		ptzStr	= (PTCHAR)m_vMC2Err[err_cd].c_str();	break;
	case ENG_SCTS::en_luria_reg			:
		if (err_cd < m_vLuriaReg.size())	ptzStr	= (PTCHAR)m_vLuriaReg[err_cd].c_str();	break;
	case ENG_SCTS::en_luria_sys			:
		err_cd	-= 10000;
		if (err_cd < m_vLuriaSys.size())	ptzStr	= (PTCHAR)m_vLuriaSys[err_cd].c_str();	break;
	case ENG_SCTS::en_luria_ph			:
		err_cd	-= 10000;
		if (err_cd < m_vLuriaPh.size())		ptzStr	= (PTCHAR)m_vLuriaPh[err_cd].c_str();	break;
	case ENG_SCTS::en_luria_sts_1		:
		if (err_cd < m_vLuriaSts1.size())	ptzStr	= (PTCHAR)m_vLuriaSts1[err_cd].c_str();	break;
	case ENG_SCTS::en_luria_sts_2		:
		if (err_cd < m_vLuriaSts2.size())	ptzStr	= (PTCHAR)m_vLuriaSts2[err_cd].c_str();	break;
	case ENG_SCTS::en_melsec_axis_err	:
		if (err_cd < m_vPlcZErr.size())		ptzStr	= (PTCHAR)m_vPlcZErr[err_cd].c_str();break;
	}

	return ptzStr;
}