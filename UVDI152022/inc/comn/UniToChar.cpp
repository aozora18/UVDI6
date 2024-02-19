
#include "pch.h"
#include "UniToChar.h"
#ifdef _DEBUG
#include <locale.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 desc : 생성자
 parm : None
 retn : None
*/
CUniToChar::CUniToChar()
{
	m_pcData = NULL;
	m_ptData = NULL;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CUniToChar::~CUniToChar()
{
	if (m_pcData != NULL)
	{
		delete [] m_pcData;
		m_pcData = NULL;
	}
	if (m_ptData != NULL)
	{
		SysFreeString(m_ptData);
		m_ptData = NULL;
	}
	m_strData.Empty();
}

/*
 desc : Unicode String -> Ansi String
 parm : None
 retn : Ansi 문자열 반환
*/
CHAR* CUniToChar::Str2Ansi(CString data)
{
	INT32 i32Len;
	CHAR *pcData = NULL;

	if (data.GetLength() < 1)	return NULL;

	i32Len = WideCharToMultiByte(CP_ACP, 0, data, -1, pcData, 0, NULL, NULL);

	if (i32Len > 0)
	{
		if (m_pcData != NULL)
		{
			delete [] m_pcData;
			m_pcData = NULL;
		}
		m_pcData	= new CHAR [i32Len+1];
		if (m_pcData == NULL)
		{
			SetDisplayErrorMsg(_T("Str2Ansi() char 포인터 변수가 생성되지 않음"));
			return NULL;
		}
		memset(m_pcData, NULL, sizeof(CHAR)*(i32Len+1));
		WideCharToMultiByte(CP_ACP, 0, data, -1, m_pcData, i32Len, NULL, NULL);
		m_pcData[i32Len] = '\0';
	}
	else
	{
		SetDisplayErrorMsg(_T("Str2Ansi() 입력 데이터가 없음."));
		return NULL;
	}

	return m_pcData;
}

/*
 desc : Ansi String -> Unicode String
 parm : None
 retn : Unicode 문자열 반환
*/
CString CUniToChar::Ansi2Str(CHAR *data)
{
	// char -> TCHAR -> CString 변환
	m_strData.Format(_T("%s"), Ansi2Uni(data));
	return m_strData;
}

/*
 desc : Ansi String -> TCHAR String
 parm : None
 retn : TCHAR 문자열 반환
*/
TCHAR* CUniToChar::ConstAnsi2Uni(const char *data)
{
	char szData[4096]	= {NULL};
	sprintf_s(szData, 4096, "%s", data);
	return Ansi2Uni(szData);
}
TCHAR* CUniToChar::Ansi2Uni(char *data)
{
	if (!data)	return NULL;

	char szData[1024]	= "The input data does not exists.";
	int iLen = (int)strlen(data);

	if (m_ptData != NULL)
	{
		SysFreeString(m_ptData);
		m_ptData = NULL;
	}

	// 데이터가 존재하지 않는 경우
	if (iLen < 0)	data = szData;

	// 유니코드로 변환하기에 앞어 먼저 그것의 유니코드에서의 길이를 알아야 한다.
	iLen = (int)MultiByteToWideChar(CP_ACP, 0, (LPCSTR)data, (int)strlen(data), NULL, NULL);
	m_ptData = SysAllocStringLen(NULL, iLen);
	if (m_ptData == NULL)
	{
		SetDisplayErrorMsg(_T("Ansi2Uni() TCHAR 포인터 변수가 생성되지 않음"));
		return NULL;
	}
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)data, (int)strlen(data), m_ptData, iLen);
	m_ptData[iLen] = '\0';

	return m_ptData;
}

/*
 desc : TCHAR String -> Ansi String
 parm : None
 retn : Ansi 문자열 반환
*/
CHAR* CUniToChar::Uni2Ansi(TCHAR *data)
{
	int iLen;
	char *pcData = NULL;
	TCHAR tzData[1024]	= L"";

	if (!data)	return NULL;

	if (m_pcData != NULL)
	{
		delete [] m_pcData;
		m_pcData = NULL;
	}

	// 데이터가 존재하지 않는 경우
	if (_tcslen(data) < 1)	data = tzData;

	// 유니코드로 변환
	iLen = WideCharToMultiByte(CP_ACP, 0, data, -1, pcData, 0, NULL, NULL);

	m_pcData = new CHAR [iLen+1];
	if (m_pcData == NULL)
	{
		SetDisplayErrorMsg(_T("Uni2Ansi() char 포인터 변수가 생성되지 않음"));
		return NULL;
	}
	memset(m_pcData, NULL, sizeof(CHAR)*(iLen+1));
	WideCharToMultiByte(CP_ACP, 0, data, -1, m_pcData, iLen, NULL, NULL);
	m_pcData[iLen]='\0';

	return m_pcData;
}

/*
 desc : Ansi String -> UTF8 String
 parm : None
 retn : UTF8 문자열 반환
*/
CHAR* CUniToChar::Ansi2UTF(char *data)
{
	TCHAR* tcData;
	if (!data)	return NULL;

	int iUniLen = 0, iUTF8Len = 0;
	int iLen = (int)strlen(data);
	if (iLen > 0)
	{
		if (m_pcData != NULL)
		{
			delete[] m_pcData;
			m_pcData = NULL;
		}
		if (m_pcData != NULL)
		{
			SetDisplayErrorMsg(_T("Ansi2UTF8() char 포인터 변수가 해제되지 않음"));
			return NULL;
		}

		// Ansi -> Unicode
		tcData		= Ansi2Uni(data);
		iUniLen		=(int) _tcslen(tcData);
		iUTF8Len	= WideCharToMultiByte(CP_UTF8, 0, tcData, iUniLen, NULL, 0, NULL, NULL);
		m_pcData	= new CHAR[iUTF8Len+1];
		if (m_pcData == NULL)
		{
			SetDisplayErrorMsg(_T("Ansi2UTF8() char 포인터 변수가 생성되지 않음"));
			return NULL;
		}
		memset(m_pcData, NULL, sizeof(char)*(iUTF8Len+1));
		// Unicode -> UTF
		iUTF8Len = WideCharToMultiByte(CP_UTF8, 0, tcData, iUniLen, m_pcData, iUTF8Len, NULL, NULL);

		m_pcData[iUTF8Len]='\0';
	}
	else
	{
		SetDisplayErrorMsg(_T("Ansi2UTF8() 입력 데이터가 없음."));
		return NULL;
	}

	return m_pcData;
}

/*
 desc : UTF8 String -> Ansi String
 parm : None
 retn : Ansi 문자열 반환
*/
CHAR* CUniToChar::UTF2Ansi(char *data)
{
	TCHAR *tcData;
	if (!data)	return NULL;

	int iUniLen = 0, iAnsiLen = 0;
	int iLen = (int)strlen(data);
	if (iLen > 0)
	{
		if (m_pcData != NULL)
		{
		delete [] m_pcData;
			m_pcData = NULL;
		}
		if (m_pcData != NULL)
		{
			SetDisplayErrorMsg(_T("UTF2Ansi() char 포인터 변수가 해제되지 않음"));
			return NULL;
		}

		iUniLen		= MultiByteToWideChar(CP_UTF8, 0, data, iLen, NULL, 0);
		tcData		= new TCHAR[iUniLen+1];
		tcData[iUniLen] = '\0';
		iUniLen		= MultiByteToWideChar(CP_UTF8, 0, data, iLen, tcData, iUniLen);
		iAnsiLen	= WideCharToMultiByte(CP_ACP, 0, tcData, iUniLen, NULL, 0, NULL, NULL);

		m_pcData	= new CHAR[iAnsiLen+1];
		if (m_pcData == NULL)
		{
			SetDisplayErrorMsg(_T("UTF2Ansi() char 포인터 변수가 생성되지 않음"));
			return NULL;
		}
		memset(m_pcData, NULL, sizeof(char)*(iAnsiLen+1));	
		iAnsiLen	= WideCharToMultiByte(CP_ACP, 0, tcData, iUniLen, m_pcData, iAnsiLen, NULL, NULL);

		m_pcData[iAnsiLen]='\0';
		delete[] tcData;
		tcData = NULL;

	}
	else
	{
		SetDisplayErrorMsg(_T("UTF2Ansi() 입력 데이터가 없음."));
		return NULL;
	}

	return m_pcData;
}

/*
 desc : 에러 메시지 출력
 parm : msg	- 에러 정보가 저장된 스트링 버퍼
 retn : None
*/
VOID CUniToChar::SetDisplayErrorMsg(CString msg)
{
#ifdef _DEBUG
	TRACE(L"CUniToChar::SetDisplayErrorMsg - %s", msg.GetBuffer());
#else
//	OutputDebugString(msg);
#endif
}
