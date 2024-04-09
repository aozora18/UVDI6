
#pragma once

#include "../../inc/comn/ThinThread.h"
#include "Mvsenc.h"
#include "mvsencenum.h"
#define TRIG_RECV_OUTPUT	0	/* Trigger Board�κ��� ���ŵ� ������ ��� */

class CSendQue;

class CMvencThread : public CThinThread
{
/* ������/�ı��� */
public:

	CMvencThread(BYTE byPort, LPG_CTSP conf, LPG_TPQR shmem,
				LPG_DLSM link, LPG_CCGS comn);
	virtual ~CMvencThread();

	// �����Լ� ������
protected:
	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();

/* ���� ����ü */
protected:


/* ���� ���� */
protected:
	HANDLE					m_handle; 
	BYTE					m_byPort;
	MVS_ENC_HT_TRGCONTROL	m_stTrigCtrl[MAX_TRIG_CHANNEL];

	BOOL					m_bInitRecv;

	/* ����� ī�޶��� Ʈ���� ��� ��ġ ���� (���� �ֱ� ��. ���) */
	UINT32					m_u32SendTrigPos[MAX_TRIG_CHANNEL][MAX_REGIST_TRIG_POS_COUNT];	/* (����: 100 nm or 0.1 um) */
	UINT64					m_u64SendTime;			/* ª�� �ð� ���� ���� �����͸� �۽����� ���ϵ��� �ϱ� ���� */
#if (TRIG_RECV_OUTPUT)
	UINT64					m_u64TraceTime;
#endif

	LPG_CTSP				m_pstConfTrig;			/* Trigger Configuration */
	LPG_TPQR				m_pstShMemTrig;			/* Trigger Shared Memory */
	LPG_DLSM				m_pstShMemLink;			/* ��� ���� ���� */
	LPG_CCGS				m_pstConfComn;			/* ��ü ���� ȯ�� ���� */

	CMySection				m_syncSend;				/* �۽� ����ȭ ��ü */


/* ���� �Լ� */
protected:
	VOID				SetTrigEvent(BOOL direct, LPG_TPTS trig_set);
	VOID				SetTrigPos(LPG_TPTS trig_set,
								   UINT8 start1, UINT8 count1, PINT32 pos1,
								   UINT8 start2, UINT8 count2, PINT32 pos2);

/* ���� �Լ� */
public:
	BOOL				IsConnected();
	void				CloseHandle();

	UINT8				GetTrigChNo(UINT8 cam_id);
	BOOL				ReqInitUpdate();
	BOOL				ReqEEPROMCtrl(UINT8 mode);
	BOOL				ReqReadSetup();
	BOOL				ResetTrigPosAll();
	BOOL				ReqWriteBoardReset();
	BOOL				ReqWriteEncoderOut(UINT32 enc_out);
	BOOL				ReqWriteTrigOutOne(UINT32 enc_out);
	BOOL				ReqWriteTrigOutOne_(UINT32 channelBit, int trigTime=20);

	BOOL				ReqWriteTriggerStrobe(BOOL enable);
	BOOL				ReqWriteAreaTrigPos(BOOL direct,
											UINT8 start1, UINT8 count1, PINT32 pos1,
											UINT8 start2, UINT8 count2, PINT32 pos2,
											ENG_TEED enable, BOOL clear);
	BOOL				ReqWriteAreaTrigPosCh(UINT8 cam_id, UINT8 start, UINT8 count, PINT32 pos,
											  ENG_TEED enable, BOOL clear);
	BOOL				ReqResetTrigCount();
	BOOL				IsInitRecvData() { return m_bInitRecv; };
	BOOL				IsTrigPosEqual(UINT8 cam_id, UINT8 index, UINT32 pos);

	/*UVDI15*/
	BOOL				ReqWirteTrigLive(UINT8 cam_id, BOOL enable);
	BOOL				SetLiveMode();
	BOOL				SetPositionTrigMode(ENG_TEED enable);
};