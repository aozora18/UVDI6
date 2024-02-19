
/*
 desc : 환경 파일의 기본 객체
*/

#include "pch.h"
#include "Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 환경 파일에서 정수 값 읽어오기
 parm : sub	- [in]  Subject Names
		key	- [in]  Key Names
		def	- [in]  값이 존재하지 않을 경우, 설정되는 값
 retn : 정수 값 반환
*/
INT32 CConfig::GetConfigInt(TCHAR *sub, TCHAR *key, INT32 def)
{
	return GetPrivateProfileInt(sub, key, def, m_tzFile);
}

/*
 desc : 환경 파일에서 정수 값 읽어오기
 parm : sub	- [in]  Subject Names
		key	- [in]  Key Names
		data- [in]  읽어들인 값이 저장될 문자열 버퍼
		size- [in]  'data' 버퍼의 크기
		def	- [in]  검색된 문자열이 없는 경우, 반환될 기본 값
 retn : None
*/
VOID CConfig::GetConfigStr(TCHAR *sub, TCHAR *key, TCHAR *data, UINT32 size, TCHAR *def)
{
	GetPrivateProfileString(sub, key, def, data, size, m_tzFile);
}

/*
 desc : 환경 파일에서 IPv4 값 읽어들인 후 숫자 값으로 반환
 parm : sub	- [in]  Subject Names
		key	- [in]  Key Names
 retn : IPv4 문자열 값과 동일한 숫자 값 반환
*/
UINT32 CConfig::GetConfigIPv4(TCHAR *sub, TCHAR *key)
{
	TCHAR tzIPv4[IPv4_LENGTH]	= {NULL};

	GetPrivateProfileString(sub, key, L"", tzIPv4, IPv4_LENGTH, m_tzFile);
	/* 읽어들인 IPv4 정보가 없다면... */
	if (wcslen(tzIPv4) < 1)	return 0;

	return uvCmn_GetIPv4StrToUint32(tzIPv4);
}

/*
 desc : 환경 파일에서 IPv4 값 읽어들인 후 Bytes Array 값으로 반환
 parm : sub	- [in]  Subject Names
		key	- [in]  Key Names
		ipv4- [out] 반환될 Bytes Array 버퍼 포인터
 retn : None
*/
VOID CConfig::GetConfigIPv4(TCHAR *sub, TCHAR *key, PUINT8 ipv4)
{
	TCHAR tzIPv4[IPv4_LENGTH]	= {NULL};

	GetPrivateProfileString(sub, key, L"", tzIPv4, IPv4_LENGTH, m_tzFile);
	/* 읽어들인 IPv4 정보가 없다면... */
	if (wcslen(tzIPv4) < 1)	return;
	/* 문자열 IPv4 -> UINT32 IPv4 -> Bytes IPv4 */
	uvCmn_GetIPv4Uint32ToBytes(uvCmn_GetIPv4StrToUint32(tzIPv4), ipv4);
}

/*
 desc : 환경 파일에서 IPv4 값 저장
 parm : sub	- [in]  Subject Names
		key	- [in]  Key Names
		ipv4- [in]  IPv4 값
 retn : IPv4 문자열 값과 동일한 숫자 값 반환
*/
VOID CConfig::SetConfigIPv4(TCHAR *sub, TCHAR *key, UINT32 ipv4)
{
	TCHAR tzIPv4[IPv4_LENGTH]	= {NULL};
	uvCmn_GetIPv4Uint32ToStr(ipv4, tzIPv4, (UINT32)wcslen(tzIPv4));
	SetConfigStr(sub, key, tzIPv4);
}

/*
 desc : 환경 파일에서 IPv4 값 저장
 parm : sub	- [in]  Subject Names
		key	- [in]  Key Names
		ipv4- [in]  IPv4 값
 retn : IPv4 문자열 값과 동일한 숫자 값 반환
*/
VOID CConfig::SetConfigIPv4(TCHAR *sub, TCHAR *key, PUINT8 ipv4)
{
	TCHAR tzIPv4[IPv4_LENGTH]	= {NULL};
	swprintf_s(tzIPv4, IPv4_LENGTH, L"%d.%d.%d.%d", ipv4[0], ipv4[1], ipv4[2], ipv4[3]);
	SetConfigStr(sub, key, tzIPv4);
}

/*
 desc : 환경 파일에 문자열 값 저장
 parm : sub	- [in]  Subject Names
		key	- [in]  Key Names
		str	- [in]  저장될 문자열
  retn : None
*/
VOID CConfig::SetConfigStr(TCHAR *sub, TCHAR *key, TCHAR *str)
{
	WritePrivateProfileString(sub, key, str, m_tzFile);
}

/*
 desc : 환경 파일에서 DOUBLE 값 읽어오기
 parm : key	- [in]  Main Title
		sub	- [in]  Sub Title
 retn : DOUBLE 값
*/
DOUBLE CConfig::GetConfigDouble(TCHAR *sub, TCHAR *key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(sub, key, L"0.000000", tzData, 128, m_tzFile);
	return _wtof(tzData);
}

/*
 desc : 환경 파일에 DOUBLE 값 저장하기
 parm : key	- [in]  Main Title
		sub	- [in]  Sub Title
		val	- [in]  저장하고자 하는 DOUBLE 값
		pts	- [in]  소숫점 자리 개수
 retn : None
*/
VOID CConfig::SetConfigDouble(TCHAR *sub, TCHAR *key, DOUBLE val, UINT8 pts)
{
	TCHAR tzData[128]	= {NULL}, tzPoint[32] = {NULL};

	if (pts == 0)	swprintf_s(tzData, 128, L" %lf", val);
	else
	{
		swprintf_s(tzPoint, 32, L" %%.%df", pts);
		swprintf_s(tzData, 128, tzPoint, val);
	}
	WritePrivateProfileString(sub, key, tzData, m_tzFile);
}

/*
 desc : 환경 파일에서 UINT8 값 읽어오기
 parm : key	- [in]  Main Title
		sub	- [in]  Sub Title
 retn : DOUBLE 값
*/
UINT8 CConfig::GetConfigUint8(TCHAR *sub, TCHAR *key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(sub, key, L"0", tzData, 128, m_tzFile);
	return (UINT8)_wtol(tzData);
}

/*
 desc : 환경 파일에서 UINT32 값 읽어오기
 parm : key	- [in]  Main Title
		sub	- [in]  Sub Title
 retn : UINT32 값
*/
UINT16 CConfig::GetConfigUint16(TCHAR *sub, TCHAR *key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(sub, key, L"0", tzData, 128, m_tzFile);
	return (UINT16)_wtoi(tzData);
}

/*
 desc : 환경 파일에서 UINT32 값 읽어오기
 parm : key	- [in]  Main Title
		sub	- [in]  Sub Title
 retn : UINT32 값
*/
UINT32 CConfig::GetConfigUint32(TCHAR *sub, TCHAR *key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(sub, key, L"0", tzData, 128, m_tzFile);
	return (UINT32)_wtol(tzData);
}

/*
 desc : 환경 파일에서 UINT32 값 읽어오기
 parm : key	- [in]  Main Title
		sub	- [in]  Sub Title
 retn : INT32 값
*/
INT32 CConfig::GetConfigInt32(TCHAR *sub, TCHAR *key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(sub, key, L"0", tzData, 128, m_tzFile);
	return (INT32)_wtol(tzData);
}

/*
 desc : 환경 파일에서 UINT32 값 읽어오기
 parm : key	- [in]  Main Title
		sub	- [in]  Sub Title
 retn : UINT64 값
*/
UINT64 CConfig::GetConfigUint64(TCHAR *sub, TCHAR *key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(sub, key, L"0", tzData, 128, m_tzFile);
	return (UINT64)_wtoi64(tzData);
}

/*
 desc : 환경 파일에 UINT32 값 저장하기
 parm : key	- [in]  Main Title
		sub	- [in]  Sub Title
		val	- [in]  저장하고자 하는 UINT32 값
 retn : None
*/
VOID CConfig::SetConfigUint32(TCHAR *sub, TCHAR *key, UINT32 val)
{
	TCHAR tzData[128]	= {NULL};

	swprintf_s(tzData, 64, L" %u", val);
	WritePrivateProfileString(sub, key, tzData, m_tzFile);
}

/*
 desc : 환경 파일에 숫자 값 저장
 parm : sub	- [in]  Subject Names
		key	- [in]  Key Names
		val	- [in]  저장될 숫자 값
  retn : None
*/
VOID CConfig::SetConfigInt32(TCHAR *sub, TCHAR *key, INT32 val)
{
	TCHAR tzValue[128]	= {NULL};
	swprintf_s(tzValue, 128, L" %d", val);
	WritePrivateProfileString(sub, key, tzValue, m_tzFile);
}
