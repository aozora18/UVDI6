
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
 desc : ������
 parm : None
 retn : None
*/
CUniToChar::CUniToChar()
{
	m_pcData = NULL;
	m_ptData = NULL;
}

/*
 desc : �ı���
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
 retn : Ansi ���ڿ� ��ȯ
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
			SetDisplayErrorMsg(_T("Str2Ansi() char ������ ������ �������� ����"));
			return NULL;
		}
		memset(m_pcData, NULL, sizeof(CHAR)*(i32Len+1));
		WideCharToMultiByte(CP_ACP, 0, data, -1, m_pcData, i32Len, NULL, NULL);
		m_pcData[i32Len] = '\0';
	}
	else
	{
		SetDisplayErrorMsg(_T("Str2Ansi() �Է� �����Ͱ� ����."));
		return NULL;
	}

	return m_pcData;
}

/*
 desc : Ansi String -> Unicode String
 parm : None
 retn : Unicode ���ڿ� ��ȯ
*/
CString CUniToChar::Ansi2Str(CHAR *data)
{
	// char -> TCHAR -> CString ��ȯ
	m_strData.Format(_T("%s"), Ansi2Uni(data));
	return m_strData;
}

/*
 desc : Ansi String -> TCHAR String
 parm : None
 retn : TCHAR ���ڿ� ��ȯ
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

	// �����Ͱ� �������� �ʴ� ���
	if (iLen < 0)	data = szData;

	// �����ڵ�� ��ȯ�ϱ⿡ �վ� ���� �װ��� �����ڵ忡���� ���̸� �˾ƾ� �Ѵ�.
	iLen = (int)MultiByteToWideChar(CP_ACP, 0, (LPCSTR)data, (int)strlen(data), NULL, NULL);
	m_ptData = SysAllocStringLen(NULL, iLen);
	if (m_ptData == NULL)
	{
		SetDisplayErrorMsg(_T("Ansi2Uni() TCHAR ������ ������ �������� ����"));
		return NULL;
	}
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)data, (int)strlen(data), m_ptData, iLen);
	m_ptData[iLen] = '\0';

	return m_ptData;
}

/*
 desc : TCHAR String -> Ansi String
 parm : None
 retn : Ansi ���ڿ� ��ȯ
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

	// �����Ͱ� �������� �ʴ� ���
	if (_tcslen(data) < 1)	data = tzData;

	// �����ڵ�� ��ȯ
	iLen = WideCharToMultiByte(CP_ACP, 0, data, -1, pcData, 0, NULL, NULL);

	m_pcData = new CHAR [iLen+1];
	if (m_pcData == NULL)
	{
		SetDisplayErrorMsg(_T("Uni2Ansi() char ������ ������ �������� ����"));
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
 retn : UTF8 ���ڿ� ��ȯ
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
			SetDisplayErrorMsg(_T("Ansi2UTF8() char ������ ������ �������� ����"));
			return NULL;
		}

		// Ansi -> Unicode
		tcData		= Ansi2Uni(data);
		iUniLen		=(int) _tcslen(tcData);
		iUTF8Len	= WideCharToMultiByte(CP_UTF8, 0, tcData, iUniLen, NULL, 0, NULL, NULL);
		m_pcData	= new CHAR[iUTF8Len+1];
		if (m_pcData == NULL)
		{
			SetDisplayErrorMsg(_T("Ansi2UTF8() char ������ ������ �������� ����"));
			return NULL;
		}
		memset(m_pcData, NULL, sizeof(char)*(iUTF8Len+1));
		// Unicode -> UTF
		iUTF8Len = WideCharToMultiByte(CP_UTF8, 0, tcData, iUniLen, m_pcData, iUTF8Len, NULL, NULL);

		m_pcData[iUTF8Len]='\0';
	}
	else
	{
		SetDisplayErrorMsg(_T("Ansi2UTF8() �Է� �����Ͱ� ����."));
		return NULL;
	}

	return m_pcData;
}

/*
 desc : UTF8 String -> Ansi String
 parm : None
 retn : Ansi ���ڿ� ��ȯ
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
			SetDisplayErrorMsg(_T("UTF2Ansi() char ������ ������ �������� ����"));
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
			SetDisplayErrorMsg(_T("UTF2Ansi() char ������ ������ �������� ����"));
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
		SetDisplayErrorMsg(_T("UTF2Ansi() �Է� �����Ͱ� ����."));
		return NULL;
	}

	return m_pcData;
}

/*
 desc : ���� �޽��� ���
 parm : msg	- ���� ������ ����� ��Ʈ�� ����
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
