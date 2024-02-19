

#pragma once

#include "LuriaThread.h"

/* The objects of user data family */
class CMachineConfig;
class CJobManagement;
class CPanelPreparation;
class CExposure;
class CDirectPhComn;
class CSystem;
class CComManagement;
class CPhZFocus;

#ifdef _DEBUG
#define MAX_LURIA_PACKET_COUNT	64	/* �ּ��� 32�� �̻��̾�� �� */
#define	REQ_LURIA_PERIOD_TIME	250
#else
#define MAX_LURIA_PACKET_COUNT	128
#define	REQ_LURIA_PERIOD_TIME	100
#endif

class CLuriaCThread : public CLuriaThread
{
/* ������ & �ı��� */
public:

	CLuriaCThread(LPG_CTCS luria,
				  LPG_CLSI conf,
				  LPG_LDSM shmem,
				  CMachineConfig *machine,
				  CJobManagement *job,
				  CPanelPreparation *panel,
				  CExposure *exposure,
				  CDirectPhComn *direct,
				  CSystem *system,
				  CComManagement *commgr,
				  CPhZFocus *focus,
				  BOOL init_send);
	virtual ~CLuriaCThread();


/* ���� �Լ� */
protected:

	virtual VOID		RunWork();

/* ����ü */
protected:

#pragma pack (push, 8)

	typedef struct __st_luria_packet_send_buff__
	{
		UINT32			pkt_size;
		UINT32			u32_reserved;
		PUINT8			pkt_buff;

	}	STM_LPSB,	*LPM_LPSB;

#pragma pack (pop)

// ���� ����
protected:

	UINT8				m_u8NextJob;			/* �ֱ������� ��û�� ��, ���� ��û�ϴ� ���� �� */
#if 0	/* GUI �ʿ��� �ʿ��� ������ ȣ���ؼ� �����ϴ� ������ �ؾ� �� ��... ���⼭�� �ʿ� ���� */
	UINT8				m_u8NextPh;				/* �ֱ������� ��û�� ��, ���� ��� ��ȣ */
	UINT8				m_u8NextLed;			/* �ֱ������� ��û�ϴ� Led Number (0 ~ 5... ���� 4, 5�� LED�� ������� ����) */
#endif
	BOOL				m_bConnectInited;		// ����� �� �ʱ�ȭ �Ǿ����� ����
	BOOL				m_bLuriaInited;			/* Luria ����ǰ� �� ���� �ʱ�ȭ���� ��� �Ϸ�Ǿ����� ���� */
	BOOL				m_bInitSend;			/* ���� ���� �ʱ�ȭ ����� �۽����� ���� */
	BOOL				m_bWorkBusy;			/* TRUE : Busy ����, FALSE : Idle ����*/

	UINT32				m_u32LinkTime;			// ������ �ֱ� (����: �и���)
	UINT32				m_u32IdleTime;			// Alive Check Time (����: �и���)

	UINT64				m_u64RecvTime;			/* ��ð� ���ŵ� �����Ͱ� ������ Ȯ�ο� */
	UINT64				m_u64ReqLedOnTime;		/* Led On Time �κ��� �ֱ������� ��û�ϱ� ���ٴ� Ư�� �ð����� ��û�ϱ� ���� */
	UINT64				m_u64ReqPeriodTime;		/* �ֱ������� ��û�ϴ� �ð� ���� */

	CAtlList <STM_LPSB>	m_lstPktSend;			/* �۽ŵ� ��Ŷ�� ����� ����Ʈ */

	/* The objects of user data family */
	CMachineConfig		*m_pPktMC;
	CJobManagement		*m_pPktJM;
	CPanelPreparation	*m_pPktPP;
	CExposure			*m_pPktEP;
	CDirectPhComn		*m_pPktDP;
	CSystem				*m_pPktSS;
	CComManagement		*m_pPktCM;
	CPhZFocus			*m_pPktPF;


/* ���� �Լ� */
protected:

	BOOL				PktAnalysis(LPG_PCLR data);
	BOOL				Connected();

	// ���� ����
	VOID				SetConnect();
	BOOL				IsLinkEvent();
	BOOL				DoConnect();
	BOOL				BindSock();
	VOID				Connected(SOCKET sd);	// ����� ��, �ʱ� �ѹ� ȣ���
	UINT8				PopSockEvent();			// Socket Event ���� ���

	VOID				PopPktData(UINT64 tick);

	VOID				CloseSocket(SOCKET sd, UINT32 time_out=0);

	BOOL				ReqPeriodSendPacket();	/* �ֱ������� ��û�ϴ� ��Ŷ ó�� */
	VOID				ReqGetPeriodPkt(UINT64 tick);
	VOID				SetRecvPacket(ENG_TPCS type, PUINT8 data);

	VOID				UpdateLinkTime(UINT8 flag);


/* ���� �Լ� */
public:

	BOOL				IsConnectInited()		{	return m_bConnectInited;	}
	// �������� ���� ������ ����
	VOID				CloseClient()			{	CloseSocket(m_sdClient);	}
	/* ������ �۽� */
	BOOL				AddPktSend(PUINT8 data, UINT32 size);
	/* �뱤 ���� ���� */
	VOID				SetWorkBusy(BOOL flag)	{	m_bWorkBusy		= flag;		}
	BOOL				GetWorkBusy()			{	return m_bWorkBusy;			}
	/* LED On Time & Temp ��� ��û */
	BOOL				ReqGetPhLedOnTimeAll();
	BOOL				ReqGetPhLedTempAll();
	BOOL				ReqGetPhLedTempFreqAll(UINT8 freq_no);

	VOID				RemoveAllSendPkt();
	/* Luria Service�� ����ǰ�, �ʱ� ���� ��/���� �۾��� ��� �������� ���� */
	BOOL				IsLuriaInited()			{	return m_bLuriaInited;		}
	BOOL				IsSendBuffFull();
	FLOAT				GetSendBuffEmptyRate();
};