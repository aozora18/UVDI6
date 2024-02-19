

#pragma once

#include "MC2Thread.h"

#ifdef _DEBUG
#define MAX_MC2_PACKET_COUNT	64	/* �ּ��� 32�� �̻��̾�� �� */
#else
#define MAX_MC2_PACKET_COUNT	128
#endif
#define MAX_MC2_PACKET_SIZE		192	/* Drive Data (MAX:8 * 16 bytes) + Packet Header (ENG_DCG3) (Read:16 or Write:16 or ReadWrite:26 bytes) + UDP Header (10 bytes) */

class CMC2CThread : public CMC2Thread
{
// ������ & �ı���
public:

	CMC2CThread(LPG_CMSI config,
				LPG_MDSM shmem,
				UINT16 mc2_port,
				UINT16 cmps_port,
				UINT32 source_ip,
				UINT32 target_ip,
				UINT32 serial_no,
				UINT32 port_buff=1024*8,
				UINT32 recv_buff=1024*1024,
				UINT32 ring_buff=1024*1024*2,
				UINT32 idle_time=30000,
				UINT32 sock_time=30);
	virtual ~CMC2CThread();


// ���� �Լ�
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();

/* ����ü ���� */
protected:

#pragma pack (push, 1)
	typedef struct __stm_packet_send_data_size__
	{
		UINT8			mode;	/* �ֱ������� ��û�ϴ� ��� (0x00)����, �ܺο��� �������� ��û�ϴ� ������� (0x01) */
		UINT8			data[MAX_MC2_PACKET_SIZE];
		UINT32			size;

	}	STM_PSDS,	*LPM_PSDS;

#pragma pack (pop)

// ���� ����
protected:

	BOOL				m_bInitSocket;		/* UDP ���� �ʱ�ȭ �Ǿ����� ���� */
	BOOL				m_bSendPeriodPkt;	/* MC2���� �ֱ������� ��Ŷ ������ ��û���� ���� ���� */

	PUINT8				m_pPktReadBuff;		/* Read Object Value Index ��Ŷ �۽� ���� */
	PUINT8				m_pPktReqWrite;
	UINT16				m_u16ReqIndex;		/* �ֱ������� MC2�� ������ ��û�ϱ� ���� ���� �� */
	UINT32				m_u32SerialNo;
	UINT32				m_u32IdleTime;

	UINT64				m_u64PeriodTime;	/* �ֱ������� ȣ���ϴ� �ð� ���� �� (����: msec) */
	UINT64				m_u64SendTime;		/* ���� : �и���. �ֱ������� �۽��ϱ� ���� �ð� ���� */
	UINT64				m_u64RecvTime;		/* ���� : �и���. �ֱ������� ���ŵ� �� �߻��� ������ �ð� ���� */

	LPG_CMSI			m_pstConfig;		/* MC2 ȯ�� ���� ���� */
	LPG_PUCH			m_pstReqHead;		/* UDP Header Part */
	LPG_PQRD			m_pstReqRead;		/* Reading Object Part */

	/* �۽� ��Ŷ ���� */
	CAtlList <STM_PSDS>	m_lstSendPkt;

	CMySection			m_syncPeriodPkt;	/* Luria Printing ���� */


// ���� �Լ�
protected:

	// ���� ����
	BOOL				InitSocket(UINT64 tick);
	VOID				CloseSocket(SOCKET sd, UINT32 time_out=0);
	UINT8				PopSockEvent();			// Socket Event ���� ���

	INT32				GetDevStepMovePos(ENG_MMDI drv_id, ENG_MDMD direct, INT32 dist);
	INT32				GetDevJogMovePos(ENG_MMDI drv_id, ENG_MDMD direct);
	INT32				GetDevAbsMovePos(ENG_MMDI drv_id, INT32 pos);

	VOID				SendPeriodPkt(UINT64 tick);
	VOID				ReadReqPacket();
	VOID				ReadObjectValueByIndex(UINT8 sub_id,
											   UINT16 read_size,
											   UINT32 sub_index,
											   UINT32 index);
	BOOL				WriteObjectValueByIndex(UINT8 sub_id, UINT32 sub_index, UINT32 index);
	/* MC2 Control */
	BOOL				WriteSdoRefDrive(ENG_MMDI drv_id, ENG_MCCN cmd,
										 UINT8 param0, INT32 param1, INT32 param2, INT32 param3);
#if 1
	BOOL				AddPktData(PUINT8 data, UINT32 size, UINT8 mode);
#else
	BOOL				SendData(PUINT8 data, UINT32 size);
#endif

	VOID				UpdateLinkTime(UINT8 flag);


// ���� �Լ�
public:

	/* �������� ���� ������ ���� */
	VOID				CloseClient()			{	CloseSocket(m_sdClient);	}
	/* PDOs (Process Data Objects) : PDO Reference Value Collective Object (address. 0x3801) */
	BOOL				WriteObjectValueByIndex(LPG_RVCH drv_head, LPG_RVCD drv_data);
	/* EN_STOP ��� ���� */
	BOOL				WritePdoRefHead(ENG_PSOI obj_index, UINT64 data);
	/* �������� �̵� ���� */
	BOOL				WriteDevMoveRef(ENG_MMMM method, ENG_MMDI drv_id, ENG_MDMD direct,
										INT32 dist, INT32 velo);
	BOOL				WriteDevMoveAbs(ENG_MMDI drv_id, INT32 pos, UINT32 velo);
	/* �������� ���� �̵� ���� */
	BOOL				WriteStageMoveVec(ENG_MMDI m_drv_id, ENG_MMDI s_drv_id,
										  INT32 m_drv_pos, INT32 s_drv_pos, INT32 velo=0);
	/* MC2 ���� */
	BOOL				WriteDevCtrl(ENG_MMDI drv_id, ENG_MCCN cmd);
	/* ���� �ӵ� ���� */
	BOOL				WriteDevGoVelo(ENG_MMDI drv_id, UINT32 velo);
	/* Ʈ���� ��� ���� */
	BOOL				WriteDevLuriaMode(ENG_MMDI drv_id, ENG_MTAE mode,
										  UINT64 pixel, INT32 begin, INT32 end);
	/* ���� �⺻ �ӵ� / ���ӵ� ���� (����) */
	BOOL				WriteDevBaseVeloAcce(ENG_MMDI drv_id, UINT32 velo, UINT32 acce);
	/* Luria Printing ���ο� ���� */
	VOID				SetSendPeriodPkt(BOOL enable);
	BOOL				IsSendPeriodPkt()	{	return m_bSendPeriodPkt;	};
	BOOL				ReadReqPktRefAll();
	BOOL				ReadReqPktActAll();
};