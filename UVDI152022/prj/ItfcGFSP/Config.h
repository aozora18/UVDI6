
#pragma once

class CConfig
{

public:

	/*
	 desc : ������
	 parm : None
	 retn : None
	*/
	CConfig()
	{
	}
	/*
	 desc : �ı���
	 parm : None
	 retn : None
	*/
	virtual ~CConfig()
	{
	}

// ���� �Լ�
protected:
public:



/* ����ü �� ����ü */
protected:


// ���� ����
protected:

	TCHAR				m_tzFile[MAX_PATH_LEN];


// ���� �Լ�
protected:

	INT32				GetConfigInt(TCHAR *sub, TCHAR *key, INT32 def=0);
	UINT32				GetConfigIPv4(TCHAR *sub, TCHAR *key);
	VOID				GetConfigIPv4(TCHAR *sub, TCHAR *key, PUINT8 ipv4);
	DOUBLE				GetConfigDouble(TCHAR *sub, TCHAR *key);
	UINT8				GetConfigUint8(TCHAR *sub, TCHAR *key);
	UINT16				GetConfigUint16(TCHAR *sub, TCHAR *key);
	UINT32				GetConfigUint32(TCHAR *sub, TCHAR *key);
	INT32				GetConfigInt32(TCHAR *sub, TCHAR *key);
	UINT64				GetConfigUint64(TCHAR *sub, TCHAR *key);


	VOID				GetConfigStr(TCHAR *sub, TCHAR *key, TCHAR *data, UINT32 size, TCHAR *def=L"");
	VOID				SetConfigStr(TCHAR *sub, TCHAR *key, TCHAR *str);
	VOID				SetConfigInt32(TCHAR *sub, TCHAR *key, INT32 val);
	VOID				SetConfigDouble(TCHAR *sub, TCHAR *key, DOUBLE val, UINT8 pts=0);
	VOID				SetConfigUint32(TCHAR *sub, TCHAR *key, UINT32 val);
	VOID				SetConfigIPv4(TCHAR *sub, TCHAR *key, UINT32 ipv4);
	VOID				SetConfigIPv4(TCHAR *sub, TCHAR *key, PUINT8 ipv4);


// ���� �Լ�
public:

};
