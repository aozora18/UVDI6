

#pragma once

#include "MC2Thread.h"

#ifdef _DEBUG
#define MAX_MC2_PACKET_COUNT	64	/* 최소한 32개 이상이어야 됨 */
#else
#define MAX_MC2_PACKET_COUNT	128
#endif
#define MAX_MC2_PACKET_SIZE		192	/* Drive Data (MAX:8 * 16 bytes) + Packet Header (ENG_DCG3) (Read:16 or Write:16 or ReadWrite:26 bytes) + UDP Header (10 bytes) */

class CMC2CThread : public CMC2Thread
{
// 생성자 & 파괴자
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


// 가상 함수
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();

/* 구조체 변수 */
protected:

#pragma pack (push, 1)
	typedef struct __stm_packet_send_data_size__
	{
		UINT8			mode;	/* 주기적으로 요청하는 경우 (0x00)인지, 외부에서 수동으로 요청하는 경우인지 (0x01) */
		UINT8			data[MAX_MC2_PACKET_SIZE];
		UINT32			size;

	}	STM_PSDS,	*LPM_PSDS;

#pragma pack (pop)

// 로컬 변수
protected:

	BOOL				m_bInitSocket;		/* UDP 소켓 초기화 되었는지 여부 */
	BOOL				m_bSendPeriodPkt;	/* MC2에게 주기적으로 패킷 데이터 요청할지 말지 결정 */

	PUINT8				m_pPktReadBuff;		/* Read Object Value Index 패킷 송신 버퍼 */
	PUINT8				m_pPktReqWrite;
	UINT16				m_u16ReqIndex;		/* 주기적으로 MC2에 데이터 요청하기 위한 순서 값 */
	UINT32				m_u32SerialNo;
	UINT32				m_u32IdleTime;

	UINT64				m_u64PeriodTime;	/* 주기적으로 호출하는 시간 간격 값 (단위: msec) */
	UINT64				m_u64SendTime;		/* 단위 : 밀리초. 주기적으로 송신하기 위한 시간 간격 */
	UINT64				m_u64RecvTime;		/* 단위 : 밀리초. 주기적으로 수신된 값 발생할 때마다 시간 저장 */

	LPG_CMSI			m_pstConfig;		/* MC2 환경 설정 정보 */
	LPG_PUCH			m_pstReqHead;		/* UDP Header Part */
	LPG_PQRD			m_pstReqRead;		/* Reading Object Part */

	/* 송신 패킷 저장 */
	CAtlList <STM_PSDS>	m_lstSendPkt;

	CMySection			m_syncPeriodPkt;	/* Luria Printing 여부 */


// 로컬 함수
protected:

	// 연결 관리
	BOOL				InitSocket(UINT64 tick);
	VOID				CloseSocket(SOCKET sd, UINT32 time_out=0);
	UINT8				PopSockEvent();			// Socket Event 감지 대기

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


// 공용 함수
public:

	/* 서버와의 연결 강제로 해제 */
	VOID				CloseClient()			{	CloseSocket(m_sdClient);	}
	/* PDOs (Process Data Objects) : PDO Reference Value Collective Object (address. 0x3801) */
	BOOL				WriteObjectValueByIndex(LPG_RVCH drv_head, LPG_RVCD drv_data);
	/* EN_STOP 모드 변경 */
	BOOL				WritePdoRefHead(ENG_PSOI obj_index, UINT64 data);
	/* 스테이지 이동 제어 */
	BOOL				WriteDevMoveRef(ENG_MMMM method, ENG_MMDI drv_id, ENG_MDMD direct,
										INT32 dist, INT32 velo);
	BOOL				WriteDevMoveAbs(ENG_MMDI drv_id, INT32 pos, UINT32 velo);
	/* 스테이지 벡터 이동 제어 */
	BOOL				WriteStageMoveVec(ENG_MMDI m_drv_id, ENG_MMDI s_drv_id,
										  INT32 m_drv_pos, INT32 s_drv_pos, INT32 velo=0);
	/* MC2 제어 */
	BOOL				WriteDevCtrl(ENG_MMDI drv_id, ENG_MCCN cmd);
	/* 모터 속도 제어 */
	BOOL				WriteDevGoVelo(ENG_MMDI drv_id, UINT32 velo);
	/* 트리거 모드 제어 */
	BOOL				WriteDevLuriaMode(ENG_MMDI drv_id, ENG_MTAE mode,
										  UINT64 pixel, INT32 begin, INT32 end);
	/* 모터 기본 속도 / 가속도 제어 (전역) */
	BOOL				WriteDevBaseVeloAcce(ENG_MMDI drv_id, UINT32 velo, UINT32 acce);
	/* Luria Printing 여부에 따라 */
	VOID				SetSendPeriodPkt(BOOL enable);
	BOOL				IsSendPeriodPkt()	{	return m_bSendPeriodPkt;	};
	BOOL				ReadReqPktRefAll();
	BOOL				ReadReqPktActAll();
};