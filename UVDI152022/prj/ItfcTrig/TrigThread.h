
#pragma once

#include "../../inc/comm/ClientThread.h"

#define TRIG_RECV_OUTPUT	0	/* Trigger Board�κ��� ���ŵ� ������ ��� */

class CSendQue;

class CTrigThread : public CClientThread
{
/* ������/�ı��� */
public:

	CTrigThread(LPG_CTCS trig, LPG_CTSP conf, LPG_TPQR shmem,
				LPG_DLSM link, LPG_CCGS comn);
	virtual ~CTrigThread();

/* �����Լ� */
protected:
	virtual BOOL		Connected();
	virtual VOID		DoWork();
	virtual BOOL		RecvData();


/* ���� ����ü */
protected:


/* ���� ���� */
protected:

	BOOL				m_bInitRecv;			/* �ʱ� 1�� ���ŵ� �����Ͱ� �����ϴ��� ����  */
	BOOL				m_bInitUpdate;			/* Ʈ���� ���忡 ����ǰ� �� ���� �ʱ� �� ���� ����  */

	/* ����� ī�޶��� Ʈ���� ��� ��ġ ���� (���� �ֱ� ��. ���) */
	UINT32				m_u32SendTrigPos[MAX_TRIG_CHANNEL][MAX_REGIST_TRIG_POS_COUNT];	/* (����: 100 nm or 0.1 um) */

	UINT64				m_u64RecvTime;			/* ������ ���ŵ� �ð� ���� */
	UINT64				m_u64SendTime;			/* ª�� �ð� ���� ���� �����͸� �۽����� ���ϵ��� �ϱ� ���� */
#if (TRIG_RECV_OUTPUT)
	UINT64				m_u64TraceTime;
#endif

	LPG_CTSP			m_pstConfTrig;			/* Trigger Configuration */
	LPG_TPQR			m_pstShMemTrig;			/* Trigger Shared Memory */
	LPG_DLSM			m_pstShMemLink;			/* ��� ���� ���� */
	LPG_CCGS			m_pstConfComn;			/* ��ü ���� ȯ�� ���� */

	CMySection			m_syncSend;				/* �۽� ����ȭ ��ü */
	CSendQue			*m_pSendQue;			/* �۽� ť ������ */


/* ���� �Լ� */
protected:

	VOID				CloseSocket(SOCKET sd, UINT32 time_out=0);

	VOID				UpdateLinkTime(UINT8 flag);

	VOID				PopSendData();
	BOOL				IsPopEmptyData();

	VOID				SetRecvPacket(ENG_TPCS type, PUINT8 data, UINT32 size);
	VOID				SetTrigEvent(BOOL direct, LPG_TPTS trig_set);
	VOID				SetTrigPos(LPG_TPTS trig_set,
								   UINT8 start1, UINT8 count1, PUINT32 pos1,
								   UINT8 start2, UINT8 count2, PUINT32 pos2);


/* ���� �Լ� */
public:

	UINT8				GetTrigChNo(UINT8 cam_id);

	UINT16				GetQueCount();
	BOOL				ReqInitUpdate();
	BOOL				ReqEEPROMCtrl(UINT8 mode);
	BOOL				ReqReadSetup();
	BOOL				ResetTrigPosAll();
	BOOL				ReqWriteBoardReset();
	BOOL				ReqWriteIP4Addr(PUINT8 ip_addr);
	BOOL				ReqWriteEncoderOut(UINT32 enc_out);
	BOOL				ReqWriteTrigOutOne(UINT32 enc_out);
	BOOL				ReqWriteTriggerStrobe(BOOL enable);
	BOOL				ReqWriteAreaTrigPos(BOOL direct,
											UINT8 start1, UINT8 count1, PUINT32 pos1,
											UINT8 start2, UINT8 count2, PUINT32 pos2,
											ENG_TEED enable, BOOL clear);
	BOOL				ReqWriteAreaTrigPosCh(UINT8 cam_id, UINT8 start, UINT8 count, PUINT32 pos,
											  ENG_TEED enable, BOOL clear);
	BOOL				ReqResetTrigCount();

	BOOL				IsInitRecvData()	{	return m_bInitRecv;	};
	BOOL				IsTrigPosEqual(UINT8 cam_id, UINT8 index, UINT32 pos);
};