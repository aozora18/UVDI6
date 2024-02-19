

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
#define MAX_LURIA_PACKET_COUNT	64	/* 최소한 32개 이상이어야 됨 */
#define	REQ_LURIA_PERIOD_TIME	250
#else
#define MAX_LURIA_PACKET_COUNT	128
#define	REQ_LURIA_PERIOD_TIME	100
#endif

class CLuriaCThread : public CLuriaThread
{
/* 생성자 & 파괴자 */
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


/* 가상 함수 */
protected:

	virtual VOID		RunWork();

/* 구조체 */
protected:

#pragma pack (push, 8)

	typedef struct __st_luria_packet_send_buff__
	{
		UINT32			pkt_size;
		UINT32			u32_reserved;
		PUINT8			pkt_buff;

	}	STM_LPSB,	*LPM_LPSB;

#pragma pack (pop)

// 로컬 변수
protected:

	UINT8				m_u8NextJob;			/* 주기적으로 요청할 때, 현재 요청하는 순서 값 */
#if 0	/* GUI 쪽에서 필요할 때마다 호출해서 진행하는 것으로 해야 될 듯... 여기서는 필요 없음 */
	UINT8				m_u8NextPh;				/* 주기적으로 요청할 때, 포토 헤드 번호 */
	UINT8				m_u8NextLed;			/* 주기적으로 요청하는 Led Number (0 ~ 5... 현재 4, 5번 LED는 사용하지 않음) */
#endif
	BOOL				m_bConnectInited;		// 연결된 후 초기화 되었는지 여부
	BOOL				m_bLuriaInited;			/* Luria 연결되고 난 이후 초기화가지 모두 완료되었는지 여부 */
	BOOL				m_bInitSend;			/* 접속 이후 초기화 명령을 송신할지 여부 */
	BOOL				m_bWorkBusy;			/* TRUE : Busy 상태, FALSE : Idle 상태*/

	UINT32				m_u32LinkTime;			// 재접속 주기 (단위: 밀리초)
	UINT32				m_u32IdleTime;			// Alive Check Time (단위: 밀리초)

	UINT64				m_u64RecvTime;			/* 장시간 수신된 데이터가 없는지 확인용 */
	UINT64				m_u64ReqLedOnTime;		/* Led On Time 부분은 주기적으로 요청하기 보다는 특정 시간마다 요청하기 위함 */
	UINT64				m_u64ReqPeriodTime;		/* 주기적으로 요청하는 시간 저장 */

	CAtlList <STM_LPSB>	m_lstPktSend;			/* 송신될 패킷이 저장된 리스트 */

	/* The objects of user data family */
	CMachineConfig		*m_pPktMC;
	CJobManagement		*m_pPktJM;
	CPanelPreparation	*m_pPktPP;
	CExposure			*m_pPktEP;
	CDirectPhComn		*m_pPktDP;
	CSystem				*m_pPktSS;
	CComManagement		*m_pPktCM;
	CPhZFocus			*m_pPktPF;


/* 로컬 함수 */
protected:

	BOOL				PktAnalysis(LPG_PCLR data);
	BOOL				Connected();

	// 연결 관리
	VOID				SetConnect();
	BOOL				IsLinkEvent();
	BOOL				DoConnect();
	BOOL				BindSock();
	VOID				Connected(SOCKET sd);	// 연결될 때, 초기 한번 호출됨
	UINT8				PopSockEvent();			// Socket Event 감지 대기

	VOID				PopPktData(UINT64 tick);

	VOID				CloseSocket(SOCKET sd, UINT32 time_out=0);

	BOOL				ReqPeriodSendPacket();	/* 주기적으로 요청하는 패킷 처리 */
	VOID				ReqGetPeriodPkt(UINT64 tick);
	VOID				SetRecvPacket(ENG_TPCS type, PUINT8 data);

	VOID				UpdateLinkTime(UINT8 flag);


/* 공용 함수 */
public:

	BOOL				IsConnectInited()		{	return m_bConnectInited;	}
	// 서버와의 연결 강제로 해제
	VOID				CloseClient()			{	CloseSocket(m_sdClient);	}
	/* 데이터 송신 */
	BOOL				AddPktSend(PUINT8 data, UINT32 size);
	/* 노광 시작 여부 */
	VOID				SetWorkBusy(BOOL flag)	{	m_bWorkBusy		= flag;		}
	BOOL				GetWorkBusy()			{	return m_bWorkBusy;			}
	/* LED On Time & Temp 모두 요청 */
	BOOL				ReqGetPhLedOnTimeAll();
	BOOL				ReqGetPhLedTempAll();
	BOOL				ReqGetPhLedTempFreqAll(UINT8 freq_no);

	VOID				RemoveAllSendPkt();
	/* Luria Service에 연결되고, 초기 설정 송/수신 작업이 모두 끝났는지 여부 */
	BOOL				IsLuriaInited()			{	return m_bLuriaInited;		}
	BOOL				IsSendBuffFull();
	FLOAT				GetSendBuffEmptyRate();
};