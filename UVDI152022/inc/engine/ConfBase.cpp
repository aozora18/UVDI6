
/*
 desc : Engine Configuration - Base
*/

#include "pch.h"
#include "ConfBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : cfg_name	- [in]  오로지 환경 파일 이름 (확장자 및 전체 경로 없어야 됨)
		config		- [in]  환경 정보
 retn : None
*/
CConfBase::CConfBase(PTCHAR cfg_name)
{
	TCHAR tzWorkDir[MAX_PATH_LEN]	= {NULL};

	/* 멤버 변수 초기화 */
	wmemset(m_tzSubj, 0x00, MAX_SUBJ_STRING);

	/* 현재 실행 파일 경로 얻고, 환경 파일 설정 */
	uvCmn_GetProcesssPath(tzWorkDir, MAX_PATH_LEN);
	swprintf_s(m_tzFile, MAX_PATH_LEN, L"%s\\info\\%s.inf", tzWorkDir, cfg_name);
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CConfBase::~CConfBase()
{
}

/*
 desc : 환경 파일에서 정수 값 읽어오기
 parm : key	- [in]  Key Names
		def	- [in]  값이 존재하지 않을 경우, 설정되는 값
 retn : 정수 값 반환
*/
INT32 CConfBase::GetConfigInt(PTCHAR key, INT32 def)
{
	return GetPrivateProfileInt(m_tzSubj, key, def, m_tzFile);
}

/*
 desc : 환경 파일에서 정수 값 읽어오기
 parm : key	- [in]  Key Names
		data- [in]  읽어들인 값이 저장될 문자열 버퍼
		size- [in]  'data' 버퍼의 크기
		def	- [in]  검색된 문자열이 없는 경우, 반환될 기본 값
 retn : None
*/
VOID CConfBase::GetConfigStr(PTCHAR key, PTCHAR data, UINT32 size, PTCHAR def)
{
	GetPrivateProfileString(m_tzSubj, key, def, data, size, m_tzFile);
}

/*
 desc : 환경 파일에서 IPv4 값 읽어들인 후 숫자 값으로 반환
 parm : key	- [in]  Key Names
 retn : IPv4 문자열 값과 동일한 숫자 값 반환
*/
UINT32 CConfBase::GetConfigIPv4(PTCHAR key)
{
	TCHAR tzIPv4[IPv4_LENGTH]	= {NULL};

	GetPrivateProfileString(m_tzSubj, key, L"", tzIPv4, IPv4_LENGTH, m_tzFile);
	/* 읽어들인 IPv4 정보가 없다면... */
	if (_tcslen(tzIPv4) < 1)	return 0;

	return uvCmn_GetIPv4StrToUint32(tzIPv4);
}

/*
 desc : 환경 파일에서 IPv4 값 읽어들인 후 Bytes Array 값으로 반환
 parm : key	- [in]  Key Names
		ipv4- [out] 반환될 Bytes Array 버퍼 포인터
 retn : None
*/
VOID CConfBase::GetConfigIPv4(PTCHAR key, PUINT8 ipv4)
{
	TCHAR tzIPv4[IPv4_LENGTH]	= {NULL};

	GetPrivateProfileString(m_tzSubj, key, L"", tzIPv4, IPv4_LENGTH, m_tzFile);
	/* 읽어들인 IPv4 정보가 없다면... */
	if (_tcslen(tzIPv4) < 1)	return;
	/* 문자열 IPv4 -> UINT32 IPv4 -> Bytes IPv4 */
	uvCmn_GetIPv4Uint32ToBytes(uvCmn_GetIPv4StrToUint32(tzIPv4), ipv4);
}

/*
 desc : 환경 파일에서 IPv4 값 저장
 parm : key	- [in]  Key Names
		ipv4- [in]  IPv4 값
 retn : IPv4 문자열 값과 동일한 숫자 값 반환
*/
VOID CConfBase::SetConfigIPv4(PTCHAR key, UINT32 ipv4)
{
	TCHAR tzIPv4[IPv4_LENGTH]	= {NULL};
	uvCmn_GetIPv4Uint32ToStr(ipv4, tzIPv4, IPv4_LENGTH);
	SetConfigStr(key, tzIPv4);
}

/*
 desc : 환경 파일에서 IPv4 값 저장
 parm : key	- [in]  Key Names
		ipv4- [in]  IPv4 값
 retn : IPv4 문자열 값과 동일한 숫자 값 반환
*/
VOID CConfBase::SetConfigIPv4(PTCHAR key, PUINT8 ipv4)
{
	TCHAR tzIPv4[IPv4_LENGTH]	= {NULL};
	swprintf_s(tzIPv4, IPv4_LENGTH, L"%d.%d.%d.%d", ipv4[0], ipv4[1], ipv4[2], ipv4[3]);
	SetConfigStr(key, tzIPv4);
}

/*
 desc : 환경 파일에 문자열 값 저장
 parm : key	- [in]  Key Names
		str	- [in]  저장될 문자열
  retn : None
*/
VOID CConfBase::SetConfigStr(PTCHAR key, PTCHAR str)
{
	PTCHAR ptzValue		= NULL;
	UINT32 u32BufLen	= 0;
	if (!str || _tcslen(str) < 1)
	{
		WritePrivateProfileString(m_tzSubj, key, L"", m_tzFile);
	}
	else
	{
		u32BufLen	= (UINT32)wcslen(str) + 4;
		ptzValue	= new TCHAR[u32BufLen];
		if (ptzValue)
		{
			swprintf_s(ptzValue, u32BufLen, L" %s", str);
			WritePrivateProfileString(m_tzSubj, key, ptzValue, m_tzFile);
			if (ptzValue)	delete [] ptzValue;
		}
	}
}

/*
 desc : 환경 파일에서 DOUBLE 값 읽어오기
 parm : key	- [in]  Main Title
 retn : DOUBLE 값
*/
DOUBLE CConfBase::GetConfigDouble(PTCHAR key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(m_tzSubj, key, L"0.000000", tzData, 128, m_tzFile);
	return _wtof(tzData);
}

/*
 desc : 환경 파일에 DOUBLE 값 저장하기
 parm : key	- [in]  Main Key
		val	- [in]  저장하고자 하는 DOUBLE 값
		pts	- [in]  소숫점 자리 개수
 retn : None
*/
VOID CConfBase::SetConfigDouble(PTCHAR key, DOUBLE val, UINT8 pts)
{
	TCHAR tzData[128]	= {NULL}, tzPoint[32] = {NULL};

	if (pts == 0)	swprintf_s(tzData, 128, L" %lf", val);
	else
	{
		swprintf_s(tzPoint, 32, L" %%.%df", pts);
		swprintf_s(tzData, 128, tzPoint, val);
	}
	WritePrivateProfileString(m_tzSubj, key, tzData, m_tzFile);
}

/*
 desc : 환경 파일에서 INT8 값 읽어오기
 parm : key	- [in]  Main Title
 retn : DOUBLE 값
*/
INT8 CConfBase::GetConfigInt8(PTCHAR key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(m_tzSubj, key, L"0", tzData, 128, m_tzFile);
	return (INT8)_wtol(tzData);
}

/*
 desc : 환경 파일에서 UINT8 값 읽어오기
 parm : key	- [in]  Main Title
 retn : DOUBLE 값
*/
UINT8 CConfBase::GetConfigUint8(PTCHAR key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(m_tzSubj, key, L"0", tzData, 128, m_tzFile);
	return (UINT8)_wtol(tzData);
}

/*
 desc : 환경 파일에서 UINT32 값 읽어오기
 parm : key	- [in]  Main Title
 retn : UINT32 값
*/
UINT16 CConfBase::GetConfigUint16(PTCHAR key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(m_tzSubj, key, L"0", tzData, 128, m_tzFile);
	return (UINT16)_wtoi(tzData);
}

/*
 desc : 환경 파일에서 UINT32 값 읽어오기
 parm : key	- [in]  Main Title
 retn : UINT32 값
*/
UINT32 CConfBase::GetConfigUint32(PTCHAR key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(m_tzSubj, key, L"0", tzData, 128, m_tzFile);
	return (UINT32)_wtoi64(tzData);
}

/*
 desc : 환경 파일에서 UINT32 값 읽어오기
 parm : key	- [in]  Main Title
 retn : INT32 값
*/
INT32 CConfBase::GetConfigInt32(PTCHAR key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(m_tzSubj, key, L"0", tzData, 128, m_tzFile);
	return (INT32)_wtol(tzData);
}

/*
 desc : 환경 파일에서 UINT32 값 읽어오기
 parm : key	- [in]  Main Title
 retn : UINT64 값
*/
UINT64 CConfBase::GetConfigUint64(PTCHAR key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(m_tzSubj, key, L"0", tzData, 128, m_tzFile);
	return (UINT64)_wtoi64(tzData);
}

/*
 desc : 환경 파일에 UINT32 값 저장하기
 parm : key	- [in]  Main Title
		sub	- [in]  Sub Title
		val	- [in]  저장하고자 하는 UINT32 값
 retn : None
*/
VOID CConfBase::SetConfigUint32(PTCHAR key, UINT32 val)
{
	TCHAR tzData[128]	= {NULL};

	swprintf_s(tzData, 128, L" %u", val);
	WritePrivateProfileString(m_tzSubj, key, tzData, m_tzFile);
}

/*
 desc : 환경 파일에 숫자 값 저장
 parm : key	- [in]  Key Names
		val	- [in]  저장될 숫자 값
  retn : None
*/
VOID CConfBase::SetConfigInt32(PTCHAR key, INT32 val)
{
	TCHAR tzValue[128]	= {NULL};
	swprintf_s(tzValue, 128, L" %d", val);
	WritePrivateProfileString(m_tzSubj, key, tzValue, m_tzFile);
}

/*
 desc : 환경 파일에 숫자 값 저장
 parm : key	- [in]  Key Names
		val	- [in]  저장될 숫자 값
  retn : None
*/
VOID CConfBase::SetConfigInt64(PTCHAR key, INT64 val)
{
	TCHAR tzValue[128]	= {NULL};
	swprintf_s(tzValue, 128, L" %lld", val);
	WritePrivateProfileString(m_tzSubj, key, tzValue, m_tzFile);
}

/*
 desc : 환경 파일에 숫자 값 저장
 parm : key	- [in]  Key Names
		val	- [in]  저장될 숫자 값
  retn : None
*/
VOID CConfBase::SetConfigUint64(PTCHAR key, UINT64 val)
{
	TCHAR tzValue[128]	= {NULL};
	swprintf_s(tzValue, 128, L" %llu", val);
	WritePrivateProfileString(m_tzSubj, key, tzValue, m_tzFile);
}

/*
 desc : 환경 파일에서 UINT16 Hex (16 진수) 값 읽어오기
 parm : key	- [in]  Main Title
 retn : UINT64 값
*/
UINT8 CConfBase::GetConfigHex8(PTCHAR key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(m_tzSubj, key, L"0", tzData, 128, m_tzFile);
	return (UINT8)wcstol(tzData, NULL, 16);
}

/*
 desc : 환경 파일에서 UINT16 Hex (16 진수) 값 읽어오기
 parm : key	- [in]  Main Title
 retn : UINT64 값
*/
UINT16 CConfBase::GetConfigHex16(PTCHAR key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(m_tzSubj, key, L"0", tzData, 128, m_tzFile);
	return (UINT16)wcstol(tzData, NULL, 16);
}

/*
 desc : 환경 파일에서 UINT16 Hex (16 진수) 값 읽어오기
 parm : key	- [in]  Main Title
 retn : UINT64 값
*/
UINT32 CConfBase::GetConfigHex32(PTCHAR key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(m_tzSubj, key, L"0", tzData, 128, m_tzFile);
	return (UINT32)wcstol(tzData, NULL, 16);
}
