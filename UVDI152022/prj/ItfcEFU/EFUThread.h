
#pragma once

#include "../../inc/comn/ThinThread.h"
#include "../../inc/comm/SerialComm.h"

class CEFUThread : public CThinThread
{
/* ������ & �ı��� */
public:

	CEFUThread(UINT8 port, UINT32 baud, UINT32 buff, UINT8 efu_cnt, LPG_EPAS shmem);
	virtual ~ CEFUThread();

/* �����Լ� �缱�� */
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();

public:



/* ���� ���� */
protected:

	/* �ø��� ��� ��Ʈ (1 ~ 255) */
	UINT8				m_u8Port;
	UINT8				m_u8EFUCount;

	BOOL				m_bIsPktRecv;		/* Packet �۽ſ� ���� ���� ���� */
	BOOL				m_bAutoStateGet;	/* �ֱ������� EFU ���� ��û���� ���� */

	UINT64				m_u64SendPktTime;	/* ��� �ð� ���� (�۽�)*/
	UINT64				m_u64AckTimeWait;	/* ��� �۽� �� ������ �� ������ �ִ� ����ϴ� �ð� */

	STG_EBSS			m_stSpeed;			/* ���� �ֱٿ� ������ �ӵ� ���� */

	PUINT8				m_pRecvData;		/* ���ŵ����� �ӽ� ���� */
	LPG_EPAS			m_pstState;			/* Shared Memory (EFU ���� ����) */

	CMySection			m_syncComm;
	CSerialComm			*m_pComm;			/* �ø��� ��� ��ü */


/* ���� �Լ� */
protected:

	/* ��� ������ ���� */
	BOOL				SendData(PUINT8 cmd, UINT32 size);

	/* ���� ������ ��ȯ */
	BOOL				PopCommData(PUINT8 data, UINT32 &size);

	VOID				ParseAnalysis();
	VOID				SetRecvEnable();
	VOID				CommClosed();	/* �ø��� ��� ���� */

	VOID				SaveError(UINT8 type, PUINT8 buff);


/* ���� �Լ� */
public:

	VOID				Stop();

	VOID				SetAckTimeout(UINT64 wait);
	VOID				SetAutoEnable(BOOL enable)	{ m_bAutoStateGet = enable;	}
	BOOL				IsGetAutoState()			{	return m_bAutoStateGet;	}

	UINT8				GetPort()		{	return m_u8Port;	}

	/* �ø��� ��� ��Ʈ ȯ�� ���� �� ��� */
	BOOL				GetConfigPort(STG_SSCI &data);
	BOOL				SetConfigPort(LPG_SSCI data);

	BOOL				IsOpenPort();
	BOOL				IsSendIdle()	{	return m_bIsPktRecv;	}

	BOOL				ReqSetSpeed(UINT8 bl_id, UINT8 speed);
	BOOL				ReqGetState();

	ENG_SCSC			GetCommStatus();	/* ���� ��� ���� ��ȯ */
	VOID				ResetCommData();	/* ���� �۽� �� ���� ���� ��� ������ �ʱ�ȭ */

	LPG_EBSS			GetSpeedValue()	{	return &m_stSpeed;	}
};