
#pragma once

#include "../../inc/comn/ThinThread.h"
#include "../../inc/comm/SerialComm.h"

class CMilaraThread : public CThinThread
{
/* ������ & �ı��� */
public:

	CMilaraThread(ENG_EDIC apps, UINT8 port, UINT32 baud, UINT32 buff);
	virtual ~ CMilaraThread();

/* �����Լ� �缱�� */
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();

	virtual VOID		RecvData() = 0;
	virtual BOOL		Initialized() = 0;


public:



/* ���� ���� */
protected:

	/* ���� ��Ŷ �� �ʿ� ���� ���� ���ڿ� Skip ���� (Bytes) */
	UINT8				m_u8ReplySkip;
	/* �ø��� ��� ��Ʈ (1 ~ 255) */
	UINT8				m_u8Port;
	/* �ʱ�ȭ �ܰ� ���� �ε��� �� */
	UINT8				m_u8InitStep;

	ENG_EDIC			m_enAppsID;			/* Prealigner or Robot */
	ENG_MSAS			m_enLastSendAck;	/* ���� �ֱٿ� �۽��� ��ɿ� ���� ���� �� (0x00: FAIL, 0x01: SUCC, 0xff: Not defined) */

	ENG_MRIM			m_enInfoMode;		/* 0x00 ~ 0x05 */

	BOOL				m_bIsInited;		/* ��� �۾��� �غ� �Ǿ� �ִ��� ���� */

	BOOL				m_bIsTechMode;		/* Teaching Mode���� ���� */
	BOOL				m_bIsSend;			/* ��� ��Ʈ�κ��� ������ �۽��� �����ߴ��� ���� */
	BOOL				m_bIsRecv;			/* ��� ��Ʈ�κ��� ������ ������ �����ߴ��� ���� */

	UINT32				m_u32RecvSize;		/* ���� �ֱٿ� ���ŵ� ��ɾ� ũ�� */

	UINT64				m_u64AckTimeWait;	/* ��� �۽� �� ������ �� ������ �ִ� ����ϴ� �ð� */

	PUINT8				m_pCmdSend;			/* ������ ���Ե� �۽� ���� */
	PUINT8				m_pCmdRecv;			/* ������ ���Ե� ���� ���� */

	/* ���� �ֱٿ� ���ŵ� ��Ŷ �ӽ� ���� */
	PCHAR				m_pszLastPkt;		/* �ӽ÷� ���� */

	CMySection			m_syncComm;

	CSerialComm			*m_pComm;			/* �ø��� ��� ��ü */


/* ���� �Լ� */
protected:

	/* ��� ������ ���� */
	BOOL				SendData(PCHAR cmd, UINT32 size);

	/* ���� ������ ��ȯ */
	BOOL				PopReadData(PUINT8 data, UINT32 &size);
	BOOL				CopyReadData(PUINT8 data, UINT32 &size);

	VOID				ParseAnalysis();
	VOID				CommClosed();	/* �ø��� ��� ���� */
	VOID				SaveErrorToSerial(UINT8 type, PCHAR buff);

	VOID				WaitTimeToRecv();
	VOID				PutErrMsg(TCHAR *msg);

	VOID				GetValues(vector <string> &vals);
	UINT32				RleaseCommaToNum();

	VOID				SetValues(DOUBLE *host_val, vector <string> &vals);
	VOID				SetValues(UINT32 *host_val, vector <string> &vals);
	VOID				SetValues(INT32 *host_val, vector <string> &vals);
	VOID				SetValues(UINT16 *host_val, vector <string> &vals);
	VOID				SetValues(UINT8 *host_val, vector <string> &vals);
	VOID				SetValues(INT8 *host_val, vector <string> &vals);
	VOID				SetValues(INT16 *host_val, vector <string> &vals);

	VOID				SetValue(DOUBLE &host_val, vector <string> &vals);
	VOID				SetValue(INT32 &host_val, vector <string> &vals);
	VOID				SetValue(UINT32 &host_val, vector <string> &vals);
	VOID				SetValue(UINT16 &host_val, vector <string> &vals);
	VOID				SetValue(INT16 &host_val, vector <string> &vals);
	VOID				SetValue(UINT8 &host_val, vector <string> &vals);
	VOID				SetValue(INT8 &host_val, vector <string> &vals);
	VOID				SetValue(CHAR &host_val, vector <string> &vals);

	VOID				SetValueHex(UINT32 &host_val, vector <string> &vals);
	VOID				SetValueHex(UINT16 &host_val, vector <string> &vals);

	VOID				SetInch01Value(DOUBLE &host_val, vector <string> &vals);
	VOID				SetInch001Value(DOUBLE &host_val, vector <string> &vals);
	VOID				SetInch0001Value(DOUBLE &host_val, vector <string> &vals);
	VOID				SetInch0001Values(DOUBLE *host_val, vector <string> &vals);

	DOUBLE				GetInchToUm()	{	return (2.54 /* 1 inch -> 2.54 cm */ * 10000 /* cm -> um */);	}
	DOUBLE				GetInchToPos(ENG_RITU p_type, INT32 value);


/* ���� �Լ� */
public:

	VOID				Stop();

	VOID				SetAckTimeout(UINT64 wait);

	UINT8				GetPort()		{	return m_u8Port;	}

	/* MPA�� �ʱ�ȭ �Ǿ����� ���� */
	BOOL				IsInitialized()	{	return m_bIsInited;	}
	/* �ø��� ��� ��Ʈ ȯ�� ���� �� ��� */
	BOOL				GetConfigPort(STG_SSCI &data);
	BOOL				SetConfigPort(LPG_SSCI data);

	BOOL				IsOpenPort();
	BOOL				IsSendData()	{	return m_bIsSend;	}
	BOOL				IsRecvData()	{	return m_bIsRecv;	}

	BOOL				SendData(PCHAR cmd);

	ENG_SCSC			GetCommStatus();	/* ���� ��� ���� ��ȯ */
	VOID				ResetCommData();	/* ���� �۽� �� ���� ���� ��� ������ �ʱ�ȭ */

	/* ���� �ֱٿ� �۽��� ��ɾ ���� ���� (����) �� */
	ENG_MSAS			GetLastSendAckStatus()	{	return m_enLastSendAck;	}
	BOOL				IsLastSendAckRecv()		{	return m_enLastSendAck != ENG_MSAS::none;	}
	BOOL				IsLastSendAckSucc()		{	return m_enLastSendAck == ENG_MSAS::succ;	}
	UINT32				GetRecvSize()			{	return m_u32RecvSize;	}

	/* ���� �ֱٿ� ���ŵ� ��Ŷ ��ȯ */
	PCHAR				GetLastRecvPkt()		{	return m_pszLastPkt;	}

	DOUBLE				GetAxisPosApp(ENG_RANC axis, INT32 value);
	INT32				GetPosToInch(ENG_RUTI p_type, DOUBLE value);
};