
/* ----------------------- */
/* desc : 통신 기본 스레드 */
/* ----------------------- */

#pragma once

#ifndef IPv4_LENGTH
#define LEN_IPV4				16
#endif // !IPv4_LENGTH

/* -------------------------------- */
/* 패킷 송신 시도 중에, 재송신 횟수 */
/* -------------------------------- */
#ifndef TCP_SEND_TRY_COUNT
#define TCP_SEND_TRY_COUNT		3
#endif // !TCP_SEND_TRY_COUNT

/* ---------------------------------------- */
/* 패킷 송신 시도 중에, 송신 대기 지연 시간 */
/* ---------------------------------------- */
#ifndef TCP_SEND_TRY_TIMEOUT
#define TCP_SEND_TRY_TIMEOUT	30	// 단위: 밀리초
#endif // !TCP_SEND_TRY_TIMEOUT

/* ---------------------------------------- */
/* 패킷 송신 시도 중에, 전체 대기 지연 시간 */
/* ---------------------------------------- */
#ifndef TCP_SEND_TOTAL_TIMEOUT
#define TCP_SEND_TOTAL_TIMEOUT	1000	// 단위: 밀리초
#endif // !TCP_SEND_TOTAL_TIMEOUT

/* ---------------------------------------- */
/* 패킷 수신 시도 중에, 전체 대기 지연 시간 */
/* ---------------------------------------- */
#ifndef TCP_RECV_TOTAL_TIMEOUT
#define TCP_RECV_TOTAL_TIMEOUT	3000	// 단위: 밀리초
#endif // !TCP_SEND_TOTAL_TIMEOUT

#define	PACKET_FAIL_PERIOD_TIME	300	/* 패킷 통신 에러 관련 메시지 빈번히 전달되는 것을 막기 위함 */

/* Log Macro */
#define LOG_TCPIP(app_id, level, mesg)	\
	do {	\
		uvLogs_SaveLogs(ENG_EDIC(app_id), ENG_LNWE(level), mesg, WFILE, WFUNC, WLINE);	\
	} while (0)
#define LOG_TCPIP2(app_id, level, mesg)	\
	do {	\
		uvLogs_SaveLogs2(ENG_EDIC(app_id), ENG_LNWE(level), mesg, WFILE, WFUNC, WLINE);	\
	} while (0)

/* --------------------------------------------------------------------------------------------- */
/*                                           Log Macro                                           */
/* --------------------------------------------------------------------------------------------- */

#include "../comn/ThinThread.h"
#include "../conf/tcpip.h"
#include "RingBuff.h"
#include "PktQue.h"

class CTcpThread : public CThinThread
{
// 생성자 & 파과자
public:
	CTcpThread(ENG_EDIC sys_id,					/* System ID */
			   UINT16 th_id,					// Thread ID
			   UINT16 tcp_port,					// TCP/IP Port
			   UINT32 source_ip,				// Local IPv4
			   UINT32 target_ip,				// Remote IPv4
			   UINT32 port_buff = 1024*8,		// TCP/IP Port Buffer Size (단위: KBytes, About 8 KBytes)
			   UINT32 recv_buff	= 1024*1024*1,	// Packet 버퍼의 최대 크기 (단위: Bytes. About 1 MBytes)
			   UINT32 ring_buff	= 1024*1024*2,	// Max Recv Packet Size (단위: Bytes, About 2 MBytes)
			   UINT32 sock_time	= 30,
			   UINT32 recv_count= 32);			// 기본 30 밀리초 동안 소켓 이벤트 감지를 위한 대기 (단위: 밀리초)
	virtual ~CTcpThread();

// 가상 함수
protected:

	// 순수 가상 함수
	virtual VOID		CloseSocket(SOCKET sd, UINT32 time_out=0);
	// 가상 함수 선언
	virtual BOOL		StartWork();
	virtual VOID		EndWork();
	virtual BOOL		RecvData()	= 0;


// 멤버 변수
protected:

	BOOL				m_bIsInited;		// 통신 스레드 초기화 여부
	BOOL				m_bIsLinked;		// 연결 (Connected) or 접속 (Accepted)되었는지 여부
	BOOL				m_bIsStopSend;		/* 현재 송신 중일 때, 중지 처리 */

	UINT16				m_u16ThreadID;		// Zero-based

	// 크기 단위: KBytes
	UINT32				m_u32PortBuff;		// 포트 버퍼 크기 (포트 내 송신 or 수신 부분 버퍼 크기)
	UINT32				m_u32RecvBuff;		// 수신 버퍼 크기 (송신 or 수신 패킷 중 가장 큰 패킷의 크기)
	UINT32				m_u32RingBuff;		// 수신 버퍼 크기 (수신된 패킷을 저장할 대용량 버퍼)
	INT32				m_i32SockError;		// 가장 최근에 발생된 소켓 에러
	INT32				m_i32LastSendPktSize;

	// 각종 시간 (단위: 밀리초)
	UINT32				m_u32SockTime;		// 소켓 이벤트 감지 대기 지연 시간 (단위: 밀리초)
	UINT64				m_u64WorkTime;		// 가장 최근에 통신한 시간 저장 (단위: 밀리초)

	UINT64				m_u64ConnectTime;

	SYSTEMTIME			m_stWorkTime;		// 가장 최근에 통신한 시간 저장 (단위: 년월일 시분초)
	SYSTEMTIME			m_stLinkTime;		// 원격 시스템 연결된 시간 저장 (단위: 년월일 시분초)

	// Socket Information
	TCHAR				m_tzTCPInfo[128];
	TCHAR				m_tzSourceIPv4[IPv4_LENGTH];
	TCHAR				m_tzTargetIPv4[IPv4_LENGTH];
	UINT16				m_u16TcpPort;		// TCP Port
	SOCKET				m_sdClient;			// Client Socket Descriptor
	WSAEVENT			m_wsaClient;		// Client Socket Event
	/* ---------------------------------------------------------------------------- */
	/* 패킷 수신 이벤트 발생할 때, 읽기 시도 횟수 (보통은 그냥 초기 값 0 으로 설정) */
	/* ---------------------------------------------------------------------------- */
	UINT32				m_u32RecvRepeat;	// 읽기 반복 시도 횟수
	UINT32				m_u32RecvSleep;		// 읽기 반복 시도 마다 대기하는 시간 (단위: 밀리초)

	// 가장 최근에 통신한 상태
	ENG_TPCS			m_enSocket;			// 가장 최근의 소켓 상태
	ENG_EDIC			m_enSysID;			/* System ID */

	// 수신 버퍼
	PUINT8				m_pPktRecvBuff;		// 한번 수신될 때, 저장될 임시 버퍼
	CRingBuff			*m_pPktRingBuff;	// 현재까지 수신된 데이터를 저장할 대용량 버퍼
	CPktQue				*m_pRecvQue;		// 임시 패킷 단위 별로 저장될 수신 버퍼


// 멤버 함수
protected:

	// 초기화 함수
	BOOL				InitData();
	BOOL				InitMemory();

	/* ------------------------------ */
	/* 연결된 소켓의 다양한 옵션 설정 */
	/* ------------------------------ */
	BOOL				SetSockOptBuffSize(UINT8 flag, SOCKET sd);
	BOOL				SetSockOptTimeout(UINT8 flag, SOCKET sd, INT32 time);
	BOOL				SetSockOptSpeed(INT32 optval, SOCKET sd);
	BOOL				SetSockOptAlive(INT32 optval, SOCKET sd);
	BOOL				SetSockOptRoute(INT32 optval, SOCKET sd);

	VOID				UpdateWorkTime();

	// 데이터 수신 처리
	INT32				ReceiveAllData(SOCKET sd);

	BOOL				IsTcpWorkIdle(UINT32 time_gap);
	BOOL				IsTcpWorkConnect(UINT32 time_gap);
	VOID				SetForceClose();	// 강제로 통신 종료

	// 패킷 데이터 송신/수신/해제
	BOOL				Sended(PUINT8 packet, UINT32 &size);
	BOOL				ReSended(PUINT8 packet,
								 UINT32 size,
								 UINT32 retry=TCP_SEND_TRY_COUNT,
								 UINT32 wait=TCP_SEND_TRY_TIMEOUT);
	BOOL				ReSended(PUINT8 packet,
								 UINT32 size,
								 UINT64 time_out=500,
								 UINT32 wait=TCP_SEND_TRY_TIMEOUT);
	BOOL				Received(SOCKET sd);

	// 임의 설정된 시간동안 통신이 없으면 주기적으로 Alive Check 정보 송신
	BOOL				SendAliveTime();

	VOID				SocketError(PTCHAR mesg);


// 공용 함수
public:


	BOOL				IsEmptyPktBuff();	// 수신 버퍼에 수신된 데이터가 있는지 조회

	/* ------------------------------------- */
	/* 단순한 현재 설정(저장)된 멤버 값 반환 */
	/* ------------------------------------- */
	UINT16				GetThreadID()				{	return m_u16ThreadID;	}
	BOOL				IsConnected()				{	return m_bIsLinked;		}
	UINT16				GetPort()					{	return m_u16TcpPort;	}
	UINT64				GetWorkTcpTick()			{	return m_u64WorkTime;	}
	LPSYSTEMTIME		GetWorkTcpTime()			{	return &m_stWorkTime;	}
	LPSYSTEMTIME		GetLinkTime()				{	return &m_stLinkTime;	}
	INT32				GetSockLastError()			{	return m_i32SockError;	}

	PTCHAR				GetSourceIPv4()				{	return m_tzSourceIPv4;	}
	PTCHAR				GetTargetIPv4()				{	return m_tzTargetIPv4;	}
};