
#pragma once

#include "TcpThread.h"


class CServerThread : public CTcpThread
{
// 생성자 & 파괴자
public:

	CServerThread(ENG_EDIC sys_id,
				  UINT16 th_id,					// Thread ID
				  UINT16 tcp_port,				// TCP/IP Port
				  UINT32 source_ip,				// Local IPv4
				  UINT32 target_ip,				// Remote IPv4
				  UINT32 port_buff=1024*8,		// TCP/IP Port Buffer Size (About 8 Kbytes)
				  UINT32 recv_buff=1024*1024,	// Packet 버퍼의 최대 크기 (단위: Bytes. About 1 MBytes)
				  UINT32 ring_buff=1024*1024*2,	// Max Recv Packet Size (단위: Bytes, About 1 MBytes)
				  UINT32 idle_time=10,			// 주어진 시간 동안 응답이 없는 경우, 서버 소켓 재초기화
				  UINT32 sock_time=10);			// 기본 10 밀리초 동안 소켓 이벤트 감지를 위한 대기 (단위: 밀리초)
	virtual ~CServerThread();

// 가상 함수
protected:
	// 순수 가상 함수
	virtual BOOL		Accepted() = 0;
	virtual VOID		DoWork() = 0;
	// 가상 함수 선언
	virtual VOID		RunWork();


// 로컬 변수
protected:

	UINT64				m_u64IdleTime;		// Client로부터 주어진 시간동안 응답이 없는 경우, Listen Socket을 재초기화하는 주기 (단위: 밀리초)

	SOCKET				m_sdListen;			// Listen Socket Descriptor
	WSAEVENT			m_wsaListen;		// Listen Socket Event

// 로컬 함수
protected:

	BOOL				InitListen();		// 소켓 초기화
	UINT8				PopSockEvent();		// Socket Event 정보 추출
	VOID				Accepted(SOCKET sd);
	VOID				CloseSocket(SOCKET sd, UINT32 time_out=0);

// 공용 함수
public:

	VOID				CloseClient();		// 현재 연결된 Client Socket 연결 해제 처리
	VOID				CloseServer();		// 현재 Server Socket 연결 해제 처리
};
