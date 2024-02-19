
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
 desc : ������
 parm : cfg_name	- [in]  ������ ȯ�� ���� �̸� (Ȯ���� �� ��ü ��� ����� ��)
		config		- [in]  ȯ�� ����
 retn : None
*/
CConfBase::CConfBase(PTCHAR cfg_name)
{
	TCHAR tzWorkDir[MAX_PATH_LEN]	= {NULL};

	/* ��� ���� �ʱ�ȭ */
	wmemset(m_tzSubj, 0x00, MAX_SUBJ_STRING);

	/* ���� ���� ���� ��� ���, ȯ�� ���� ���� */
	uvCmn_GetProcesssPath(tzWorkDir, MAX_PATH_LEN);
	swprintf_s(m_tzFile, MAX_PATH_LEN, L"%s\\info\\%s.inf", tzWorkDir, cfg_name);
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CConfBase::~CConfBase()
{
}

/*
 desc : ȯ�� ���Ͽ��� ���� �� �о����
 parm : key	- [in]  Key Names
		def	- [in]  ���� �������� ���� ���, �����Ǵ� ��
 retn : ���� �� ��ȯ
*/
INT32 CConfBase::GetConfigInt(PTCHAR key, INT32 def)
{
	return GetPrivateProfileInt(m_tzSubj, key, def, m_tzFile);
}

/*
 desc : ȯ�� ���Ͽ��� ���� �� �о����
 parm : key	- [in]  Key Names
		data- [in]  �о���� ���� ����� ���ڿ� ����
		size- [in]  'data' ������ ũ��
		def	- [in]  �˻��� ���ڿ��� ���� ���, ��ȯ�� �⺻ ��
 retn : None
*/
VOID CConfBase::GetConfigStr(PTCHAR key, PTCHAR data, UINT32 size, PTCHAR def)
{
	GetPrivateProfileString(m_tzSubj, key, def, data, size, m_tzFile);
}

/*
 desc : ȯ�� ���Ͽ��� IPv4 �� �о���� �� ���� ������ ��ȯ
 parm : key	- [in]  Key Names
 retn : IPv4 ���ڿ� ���� ������ ���� �� ��ȯ
*/
UINT32 CConfBase::GetConfigIPv4(PTCHAR key)
{
	TCHAR tzIPv4[IPv4_LENGTH]	= {NULL};

	GetPrivateProfileString(m_tzSubj, key, L"", tzIPv4, IPv4_LENGTH, m_tzFile);
	/* �о���� IPv4 ������ ���ٸ�... */
	if (_tcslen(tzIPv4) < 1)	return 0;

	return uvCmn_GetIPv4StrToUint32(tzIPv4);
}

/*
 desc : ȯ�� ���Ͽ��� IPv4 �� �о���� �� Bytes Array ������ ��ȯ
 parm : key	- [in]  Key Names
		ipv4- [out] ��ȯ�� Bytes Array ���� ������
 retn : None
*/
VOID CConfBase::GetConfigIPv4(PTCHAR key, PUINT8 ipv4)
{
	TCHAR tzIPv4[IPv4_LENGTH]	= {NULL};

	GetPrivateProfileString(m_tzSubj, key, L"", tzIPv4, IPv4_LENGTH, m_tzFile);
	/* �о���� IPv4 ������ ���ٸ�... */
	if (_tcslen(tzIPv4) < 1)	return;
	/* ���ڿ� IPv4 -> UINT32 IPv4 -> Bytes IPv4 */
	uvCmn_GetIPv4Uint32ToBytes(uvCmn_GetIPv4StrToUint32(tzIPv4), ipv4);
}

/*
 desc : ȯ�� ���Ͽ��� IPv4 �� ����
 parm : key	- [in]  Key Names
		ipv4- [in]  IPv4 ��
 retn : IPv4 ���ڿ� ���� ������ ���� �� ��ȯ
*/
VOID CConfBase::SetConfigIPv4(PTCHAR key, UINT32 ipv4)
{
	TCHAR tzIPv4[IPv4_LENGTH]	= {NULL};
	uvCmn_GetIPv4Uint32ToStr(ipv4, tzIPv4, IPv4_LENGTH);
	SetConfigStr(key, tzIPv4);
}

/*
 desc : ȯ�� ���Ͽ��� IPv4 �� ����
 parm : key	- [in]  Key Names
		ipv4- [in]  IPv4 ��
 retn : IPv4 ���ڿ� ���� ������ ���� �� ��ȯ
*/
VOID CConfBase::SetConfigIPv4(PTCHAR key, PUINT8 ipv4)
{
	TCHAR tzIPv4[IPv4_LENGTH]	= {NULL};
	swprintf_s(tzIPv4, IPv4_LENGTH, L"%d.%d.%d.%d", ipv4[0], ipv4[1], ipv4[2], ipv4[3]);
	SetConfigStr(key, tzIPv4);
}

/*
 desc : ȯ�� ���Ͽ� ���ڿ� �� ����
 parm : key	- [in]  Key Names
		str	- [in]  ����� ���ڿ�
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
 desc : ȯ�� ���Ͽ��� DOUBLE �� �о����
 parm : key	- [in]  Main Title
 retn : DOUBLE ��
*/
DOUBLE CConfBase::GetConfigDouble(PTCHAR key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(m_tzSubj, key, L"0.000000", tzData, 128, m_tzFile);
	return _wtof(tzData);
}

/*
 desc : ȯ�� ���Ͽ� DOUBLE �� �����ϱ�
 parm : key	- [in]  Main Key
		val	- [in]  �����ϰ��� �ϴ� DOUBLE ��
		pts	- [in]  �Ҽ��� �ڸ� ����
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
 desc : ȯ�� ���Ͽ��� INT8 �� �о����
 parm : key	- [in]  Main Title
 retn : DOUBLE ��
*/
INT8 CConfBase::GetConfigInt8(PTCHAR key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(m_tzSubj, key, L"0", tzData, 128, m_tzFile);
	return (INT8)_wtol(tzData);
}

/*
 desc : ȯ�� ���Ͽ��� UINT8 �� �о����
 parm : key	- [in]  Main Title
 retn : DOUBLE ��
*/
UINT8 CConfBase::GetConfigUint8(PTCHAR key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(m_tzSubj, key, L"0", tzData, 128, m_tzFile);
	return (UINT8)_wtol(tzData);
}

/*
 desc : ȯ�� ���Ͽ��� UINT32 �� �о����
 parm : key	- [in]  Main Title
 retn : UINT32 ��
*/
UINT16 CConfBase::GetConfigUint16(PTCHAR key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(m_tzSubj, key, L"0", tzData, 128, m_tzFile);
	return (UINT16)_wtoi(tzData);
}

/*
 desc : ȯ�� ���Ͽ��� UINT32 �� �о����
 parm : key	- [in]  Main Title
 retn : UINT32 ��
*/
UINT32 CConfBase::GetConfigUint32(PTCHAR key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(m_tzSubj, key, L"0", tzData, 128, m_tzFile);
	return (UINT32)_wtoi64(tzData);
}

/*
 desc : ȯ�� ���Ͽ��� UINT32 �� �о����
 parm : key	- [in]  Main Title
 retn : INT32 ��
*/
INT32 CConfBase::GetConfigInt32(PTCHAR key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(m_tzSubj, key, L"0", tzData, 128, m_tzFile);
	return (INT32)_wtol(tzData);
}

/*
 desc : ȯ�� ���Ͽ��� UINT32 �� �о����
 parm : key	- [in]  Main Title
 retn : UINT64 ��
*/
UINT64 CConfBase::GetConfigUint64(PTCHAR key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(m_tzSubj, key, L"0", tzData, 128, m_tzFile);
	return (UINT64)_wtoi64(tzData);
}

/*
 desc : ȯ�� ���Ͽ� UINT32 �� �����ϱ�
 parm : key	- [in]  Main Title
		sub	- [in]  Sub Title
		val	- [in]  �����ϰ��� �ϴ� UINT32 ��
 retn : None
*/
VOID CConfBase::SetConfigUint32(PTCHAR key, UINT32 val)
{
	TCHAR tzData[128]	= {NULL};

	swprintf_s(tzData, 128, L" %u", val);
	WritePrivateProfileString(m_tzSubj, key, tzData, m_tzFile);
}

/*
 desc : ȯ�� ���Ͽ� ���� �� ����
 parm : key	- [in]  Key Names
		val	- [in]  ����� ���� ��
  retn : None
*/
VOID CConfBase::SetConfigInt32(PTCHAR key, INT32 val)
{
	TCHAR tzValue[128]	= {NULL};
	swprintf_s(tzValue, 128, L" %d", val);
	WritePrivateProfileString(m_tzSubj, key, tzValue, m_tzFile);
}

/*
 desc : ȯ�� ���Ͽ� ���� �� ����
 parm : key	- [in]  Key Names
		val	- [in]  ����� ���� ��
  retn : None
*/
VOID CConfBase::SetConfigInt64(PTCHAR key, INT64 val)
{
	TCHAR tzValue[128]	= {NULL};
	swprintf_s(tzValue, 128, L" %lld", val);
	WritePrivateProfileString(m_tzSubj, key, tzValue, m_tzFile);
}

/*
 desc : ȯ�� ���Ͽ� ���� �� ����
 parm : key	- [in]  Key Names
		val	- [in]  ����� ���� ��
  retn : None
*/
VOID CConfBase::SetConfigUint64(PTCHAR key, UINT64 val)
{
	TCHAR tzValue[128]	= {NULL};
	swprintf_s(tzValue, 128, L" %llu", val);
	WritePrivateProfileString(m_tzSubj, key, tzValue, m_tzFile);
}

/*
 desc : ȯ�� ���Ͽ��� UINT16 Hex (16 ����) �� �о����
 parm : key	- [in]  Main Title
 retn : UINT64 ��
*/
UINT8 CConfBase::GetConfigHex8(PTCHAR key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(m_tzSubj, key, L"0", tzData, 128, m_tzFile);
	return (UINT8)wcstol(tzData, NULL, 16);
}

/*
 desc : ȯ�� ���Ͽ��� UINT16 Hex (16 ����) �� �о����
 parm : key	- [in]  Main Title
 retn : UINT64 ��
*/
UINT16 CConfBase::GetConfigHex16(PTCHAR key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(m_tzSubj, key, L"0", tzData, 128, m_tzFile);
	return (UINT16)wcstol(tzData, NULL, 16);
}

/*
 desc : ȯ�� ���Ͽ��� UINT16 Hex (16 ����) �� �о����
 parm : key	- [in]  Main Title
 retn : UINT64 ��
*/
UINT32 CConfBase::GetConfigHex32(PTCHAR key)
{
	TCHAR tzData[128]	= {NULL};
	GetPrivateProfileString(m_tzSubj, key, L"0", tzData, 128, m_tzFile);
	return (UINT32)wcstol(tzData, NULL, 16);
}
