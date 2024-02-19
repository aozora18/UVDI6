
/*
 desc : ȯ�� ������ �⺻ ��ü
*/

#include "pch.h"
#include "Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : ȯ�� ���Ͽ��� ���� �� �о����
 parm : sub	- [in]  Subject Names
		key	- [in]  Key Names
		def	- [in]  ���� �������� ���� ���, �����Ǵ� ��
 retn : ���� �� ��ȯ
*/
INT32 CConfig::GetConfigInt(TCHAR *sub, TCHAR *key, INT32 def)
{
	return GetPrivateProfileInt(sub, key, def, m_tzFile);
}

/*
 desc : ȯ�� ���Ͽ��� ���� �� �о����
 parm : sub	- [in]  Subject Names
		key	- [in]  Key Names
		data- [in]  �о���� ���� ����� ���ڿ� ����
		size- [in]  'data' ������ ũ��
		def	- [in]  �˻��� ���ڿ��� ���� ���, ��ȯ�� �⺻ ��
 retn : None
*/
VOID CConfig::GetConfigStr(TCHAR *sub, TCHAR *key, TCHAR *data, UINT32 size, TCHAR *def)
{
	GetPrivateProfileString(sub, key, def, data, size, m_tzFile);
}

/*
 desc : ȯ�� ���Ͽ��� IPv4 �� �о���� �� ���� ������ ��ȯ
 parm : sub	- [in]  Subject Names
		key	- [in]  Key Names
 retn : IPv4 ���ڿ� ���� ������ ���� �� ��ȯ
*/
UINT32 CConfig::GetConfigIPv4(TCHAR *sub, TCHAR *key)
{
	TCHAR tzIPv4[IPv4_LENGTH]	= {NULL};

	GetPrivateProfileString(sub, key, L"", tzIPv4, IPv4_LENGTH, m_tzFile);
	/* �о���� IPv4 ������ ���ٸ�... */
	if (wcslen(tzIPv4) < 1)	return 0;

	return uvCmn_GetIPv4StrToUint32(tzIPv4);
}

/*
 desc : ȯ�� ���Ͽ��� IPv4 �� �о���� �� Bytes Array ������ ��ȯ
 parm : sub	- [in]  Subject Names
		key	- [in]  Key Names
		ipv4- [out] ��ȯ�� Bytes Array ���� ������
 retn : None
*/
VOID CConfig::GetConfigIPv4(TCHAR *sub, TCHAR *key, PUINT8 ipv4)
{
	TCHAR tzIPv4[IPv4_LENGTH]	= {NULL};

	GetPrivateProfileString(sub, key, L"", tzIPv4, IPv4_LENGTH, m_tzFile);
	/* �о���� IPv4 ������ ���ٸ�... */
	if (wcslen(tzIPv4) < 1)	return;
	/* ���ڿ� IPv4 -> UINT32 IPv4 -> Bytes IPv4 */
	uvCmn_GetIPv4Uint32ToBytes(uvCmn_GetIPv4StrToUint32(tzIPv4), ipv4);
}

/*
 desc : ȯ�� ���Ͽ��� IPv4 �� ����
 parm : sub	- [in]  Subject Names
		key	- [in]  Key Names
		ipv4- [in]  IPv4 ��
 retn : IPv4 ���ڿ� ���� ������ ���� �� ��ȯ
*/
VOID CConfig::SetConfigIPv4(TCHAR *sub, TCHAR *key, UINT32 ipv4)
{
	TCHAR tzIPv4[IPv4_LENGTH]	= {NULL};
	uvCmn_GetIPv4Uint32ToStr(ipv4, tzIPv4, (UINT32)wcslen(tzIPv4));
	SetConfigStr(sub, key, tzIPv4);
}

/*
 desc : ȯ�� ���Ͽ��� IPv4 �� ����
 parm : sub	- [in]  Subject Names
		key	- [in]  Key Names
		ipv4- [in]  IPv4 ��
 retn : IPv4 ���ڿ� ���� ������ ���� �� ��ȯ
*/
VOID CConfig::SetConfigIPv4(TCHAR *sub, TCHAR *key, PUINT8 ipv4)
{
	TCHAR tzIPv4[IPv4_LENGTH]	= {NULL};
	swprintf_s(tzIPv4, IPv4_LENGTH, L"%d.%d.%d.%d", ipv4[0], ipv4[1], ipv4[2], ipv4[3]);
	SetConfigStr(sub, key, tzIPv4);
}

/*
 desc : ȯ�� ���Ͽ� ���ڿ� �� ����
 parm : sub	- [in]  Subject Names
		key	- [in]  Key Names
		str	- [in]  ����� ���ڿ�
  retn : None
*/
VOID CConfig::SetConfigStr(TCHAR *sub, TCHAR *key, TCHAR *str)
{
	WritePrivateProfileString(sub, key, str, m_tzFile);
}

/*
 desc : ȯ�� ���Ͽ��� DOUBLE �� �о����
 parm : key	- [in]  Main Title
		sub	- [in]  Sub Title
 retn : DOUBLE ��
*/
DOUBLE CConfig::GetConfigDouble(TCHAR *sub, TCHAR *key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(sub, key, L"0.000000", tzData, 128, m_tzFile);
	return _wtof(tzData);
}

/*
 desc : ȯ�� ���Ͽ� DOUBLE �� �����ϱ�
 parm : key	- [in]  Main Title
		sub	- [in]  Sub Title
		val	- [in]  �����ϰ��� �ϴ� DOUBLE ��
		pts	- [in]  �Ҽ��� �ڸ� ����
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
 desc : ȯ�� ���Ͽ��� UINT8 �� �о����
 parm : key	- [in]  Main Title
		sub	- [in]  Sub Title
 retn : DOUBLE ��
*/
UINT8 CConfig::GetConfigUint8(TCHAR *sub, TCHAR *key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(sub, key, L"0", tzData, 128, m_tzFile);
	return (UINT8)_wtol(tzData);
}

/*
 desc : ȯ�� ���Ͽ��� UINT32 �� �о����
 parm : key	- [in]  Main Title
		sub	- [in]  Sub Title
 retn : UINT32 ��
*/
UINT16 CConfig::GetConfigUint16(TCHAR *sub, TCHAR *key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(sub, key, L"0", tzData, 128, m_tzFile);
	return (UINT16)_wtoi(tzData);
}

/*
 desc : ȯ�� ���Ͽ��� UINT32 �� �о����
 parm : key	- [in]  Main Title
		sub	- [in]  Sub Title
 retn : UINT32 ��
*/
UINT32 CConfig::GetConfigUint32(TCHAR *sub, TCHAR *key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(sub, key, L"0", tzData, 128, m_tzFile);
	return (UINT32)_wtol(tzData);
}

/*
 desc : ȯ�� ���Ͽ��� UINT32 �� �о����
 parm : key	- [in]  Main Title
		sub	- [in]  Sub Title
 retn : INT32 ��
*/
INT32 CConfig::GetConfigInt32(TCHAR *sub, TCHAR *key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(sub, key, L"0", tzData, 128, m_tzFile);
	return (INT32)_wtol(tzData);
}

/*
 desc : ȯ�� ���Ͽ��� UINT32 �� �о����
 parm : key	- [in]  Main Title
		sub	- [in]  Sub Title
 retn : UINT64 ��
*/
UINT64 CConfig::GetConfigUint64(TCHAR *sub, TCHAR *key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(sub, key, L"0", tzData, 128, m_tzFile);
	return (UINT64)_wtoi64(tzData);
}

/*
 desc : ȯ�� ���Ͽ� UINT32 �� �����ϱ�
 parm : key	- [in]  Main Title
		sub	- [in]  Sub Title
		val	- [in]  �����ϰ��� �ϴ� UINT32 ��
 retn : None
*/
VOID CConfig::SetConfigUint32(TCHAR *sub, TCHAR *key, UINT32 val)
{
	TCHAR tzData[128]	= {NULL};

	swprintf_s(tzData, 64, L" %u", val);
	WritePrivateProfileString(sub, key, tzData, m_tzFile);
}

/*
 desc : ȯ�� ���Ͽ� ���� �� ����
 parm : sub	- [in]  Subject Names
		key	- [in]  Key Names
		val	- [in]  ����� ���� ��
  retn : None
*/
VOID CConfig::SetConfigInt32(TCHAR *sub, TCHAR *key, INT32 val)
{
	TCHAR tzValue[128]	= {NULL};
	swprintf_s(tzValue, 128, L" %d", val);
	WritePrivateProfileString(sub, key, tzValue, m_tzFile);
}
