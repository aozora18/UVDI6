
#pragma once

#include "../comn/ThinThread.h"
#include "SerialComm.h"

class CSerialThread : public CThinThread
{
/* ������ & �ı��� */
public:

	CSerialThread(UINT8 port, UINT32 baud, UINT32 buff, ENG_SHOT shake);
	virtual ~ CSerialThread();

/* �����Լ� �缱�� */
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();


/* ���� ���� */
protected:

	/* �ø��� ��� ��Ʈ (1 ~ 255) */
	UINT8				m_u8Port;

	BOOL				m_bIsReady;		// ��� ��Ʈ�κ��� ������ ��/���� �۾� �غ� �Ǿ����� ����
	BOOL				m_bIsSend;		// ��� ��Ʈ�� ������ �۽��� �����ߴ��� ����

	CMySection			m_syncComm;

	CSerialComm			*m_pComm;		// �ø��� ��� ��ü

// ���� �Լ�
protected:



// ���� �Լ�
public:

	VOID				Stop();
	VOID				PutErrMsg(TCHAR *msg);
	PTCHAR				GetErrMsg();

	/* �ø��� ��� ��Ʈ ��ȣ ��ȯ */
	UINT8				GetPort()		{	return m_u8Port;	}

	/* �ø��� ��� ��Ʈ ȯ�� ���� �� ��� */
	BOOL				GetConfigPort(STG_SSCI &data);
	BOOL				SetConfigPort(LPG_SSCI data);

	/* ��� ���� ���� ��ȯ */
	BOOL				IsOpenPort();
	/* ���� ��Ŷ ���� ������ �������� ���� �� ��ȯ */
	BOOL				IsSendData()	{	return m_bIsSend;	}

	/* ��� ������ ���� */
	BOOL				WriteData(PUINT8 data, UINT32 &size);
	/* ���� ������ ��ȯ */
	UINT32				GetReadSize();
	BOOL				PopReadData(PUINT8 data, UINT32 &size);
	BOOL				CopyReadData(PUINT8 data, UINT32 &size);

	/* �ø��� ��� ���� */
	VOID				CommClosed();

	/* ���� ��� ���� ��ȯ */
	ENG_SCSC			GetCommStatus();
};