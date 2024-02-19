
#pragma once

#include "../../inc/comm/ClientThread.h"
#include "../../inc/comn/MySection.h"

class CSendQue;

class CEFEMThread : public CClientThread
{
/* ������/�ı��� */
public:

	CEFEMThread(LPG_EDSM shmem, UINT16 th_id, LPG_CTCS efem);
	virtual ~CEFEMThread();

/* ���� �Լ� */
protected:

	virtual BOOL		Connected();
	virtual VOID		DoWork();
	/* -------------------------------------------------------------------------- */
	/* ���� ��� ���� ����� �ƴϱ� ������, ������ �����Լ��� ���� �Լ� ���� ���� */
	/* -------------------------------------------------------------------------- */
	virtual BOOL		RecvData();


/* ���� ���� */
protected:

	UINT8				m_u8SendData[8];	/* ���ſ� ���� �ٷ� �۽��� ��, �ӽ÷� ��Ŷ ���� */
#if 0
	UINT8				m_u8LastEfemState;	/* ���� �ֱ��� EFEM ���� : Ready or Busy or Error */
	UINT8				m_u8LastWaferLoad;	/* ���� �ֱ��� Lot (Wafer)�� ������ ��ġ ���� (Combined) */
#endif
	UINT8				m_u8AliveCount;		/* Alive �۽� �õ� Ƚ��. ������ ������ ��� ������ */

	SYSTEMTIME			m_tmAliveTime;		/* Alive ���� ���� �ð� ���� */

	BOOL				m_bSendRecv;		/* DI_TRUE - Sended, DI_FALSE - Recved */
	BOOL				m_bSetAreaMode;		/* Ʈ���� ���尡 Area ��� �������� ���� */
	BOOL				m_bSetFoupChange;	/* EFEM���κ��� Attach �̺�Ʈ�� ���� ��� */
	BOOL				m_bIsExposeMode;	/* ���� �뱤 ������� ���� */

	UINT64				m_u64SendTime;		/* ������ ���� �ð� (����: �и���) */
	UINT64				m_u64PeriodTime;	/* ������ ������ ������ �ð� ���� */
	UINT64				m_u64StateTime;		/* EFEM State ��û�� �ð� ���� */
	UINT64				m_u64StateTick;		/* EFEM State ��û �ֱ� (����: msec) */

	PUINT8				m_pPktRecvData;

	LPG_EDSM			m_pstShMemEFEM;		/* ���� �޸� */

	CSendQue			*m_pSendQue;		/* �۽� ť ������ */
	CMySection			m_syncSend;			/* �۽� ����ȭ ��ü */


/* ���� �Լ� */
protected:

	BOOL				InitSendData();
	BOOL				PktAnalysis(LPG_PSRB data);
	BOOL				PopSendData();
	BOOL				IsSendData();
	BOOL				ReqRefreshData(UINT16 cmd, PUINT8 data);
	BOOL				ReqEfemState();

	VOID				CloseSocket(SOCKET sd, UINT32 time_out=0);
	VOID				SetAliveData();
	VOID				SetRecvPacket(LPG_PSRB data);

	VOID				SaveErrorLogs(UINT16 code);

	VOID				UpdateLinkTime(UINT8 flag);


/* ���� �Լ� */
public:

	BOOL				SendData(ENG_ECPC cmd, PUINT8 data=NULL, UINT16 size=0x0000);

	LPSYSTEMTIME		GetAliveTime()		{	return &m_tmAliveTime;	};

	VOID				SetExposeMode(BOOL enable);
	/* ó�� ������ �߻��� ���� �ڵ� �� ó�� */
	VOID				ResetErrorCode()			{	m_pstShMemEFEM->link.u16_error_code	= 0x0000;	}
	UINT16				GetErrorCode()				{	return m_pstShMemEFEM->link.u16_error_code;		}

	/* FOUP Lot List ��û */
	BOOL				ReqFoupLotList();
};