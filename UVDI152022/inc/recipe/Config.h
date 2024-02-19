
#pragma once

class CConfig
{

public:

	/*
	 desc : ������
	 parm : cfg_file	- [in]  ��ü ��ΰ� ���Ե� ���ϸ�
	 retn : None
	*/
	CConfig()
	{
		//m_tzFile[_MAX_PATH] = {NULL};
		wmemset(m_tzFile, 0x00, _MAX_PATH);
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

	virtual BOOL		LoadConfig() = 0;
	virtual BOOL		SaveConfig() = 0;
	virtual VOID		SetData(ENG_FRCK kind, PUINT8 data, UINT32 size=0) = 0;


/* ����ü �� ����ü */
protected:

#pragma pack(push, 8)
	
	typedef struct __st_luria_error_code_message__
	{
		UINT16			msg_len;	/* ���� �޽����� ���� */
		UINT16			u32_reserved[3];
		PCHAR			err_msg;

	}	STM_LECM,	*LPM_LECM;

#pragma pack(pop)	/* 8 bytes ordering */


// ���� ����
protected:

	TCHAR				m_tzFile[_MAX_PATH];

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
