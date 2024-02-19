
/* desc : Client TCP Thread */

#pragma once

#include "TcpThread.h"


class CClientThread : public CTcpThread
{
/* 생성자 & 파괴자 */
public:

	CClientThread(ENG_EDIC sys_id,				/* System ID */
				  UINT16 th_id,					/* Thread ID */
				  UINT16 tcp_port,				// TCP/IP Port
				  UINT32 source_ip,				// Local IPv4
				  UINT32 target_ip,				// Remote IPv4
				  UINT32 port_buff=1024*8,		// TCP/IP Port Buffer Size (About 8 Kbytes)
				  UINT32 recv_buff=1024*1024,	// Packet 버퍼의 최대 크기 (단위: Bytes. About 1 MBytes)
				  UINT32 ring_buff=1024*1024*2,	// Max Recv Packet Size (단위: Bytes, About 1 MBytes)
				  UINT32 idle_time=0,			// 주어진 시간(단위: 밀리초)동안 아무린 통신 작업이 없을 경우, Alive Message 전송
												// 0 값으로 주어진 경우, Alive Check 수행하지 않음
				  UINT32 link_time=5*1000,		// 주어진 시간마다 서버에 재접속하는 값 (단위: 밀리초)
				  UINT32 sock_time=30,			// 기본 30 밀리초 동안 소켓 이벤트 감지를 위한 대기
				  UINT32 live_time=2000);
	virtual ~CClientThread();

/* 가상 함수 */
protected:

	/* 순수 가상 함수 */
	virtual BOOL		Connected()	= 0;
	virtual VOID		DoWork() = 0;
	/* 가상 함수 선언 */
	virtual VOID		RunWork();

/* 로컬 변수 */
protected:

	BOOL				m_bInitRecv;			/* 초기 1번 수신된 데이터가 존재하는지 여부  */

	UINT32				m_u32LinkTime;			/* 재접속 주기 (단위: 밀리초) */
	UINT32				m_u32IdleTime;			/* 일정 시간 통신 응답이 없는 경우, 통신 해제 시간 */
	UINT32				m_u32LiveTime;			/* Alive Check Time (단위: 밀리초) */

/* 로컬 함수 */
protected:

	BOOL				IsLinkEvent();
	BOOL				DoConnect();
	BOOL				BindSock();
	VOID				Connected(SOCKET sd);	/* 연결될 때, 초기 한번 호출됨 */
	VOID				CloseSocket(SOCKET sd, UINT32 time_out=0);
	UINT8				PopSockEvent();			/* Socket Event 감지 대기 */


/* 공용 함수 */
public:

	/* 서버와의 연결 강제로 해제 */
	VOID				CloseClient()			{	CloseSocket(m_sdClient);	}
};