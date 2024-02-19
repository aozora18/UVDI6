
/* desc : TCP Client 객체 */

#include "pch.h"
#include "ClientThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : sys_id		- [in]  System ID
		th_id		- [in]  Thread ID
		source_ip	- [in]  Local IPv4
		target_ip	- [in]  Remote IPv4
		tcp_port	- [in]  TCP/IP Port
		port_buff	- [in]  TCP/IP Port Buffer Size
		recv_buff	- [in]  Send or Recv의 패킷 중 가장 큰 패킷의 크기 (단위: Bytes)
		ring_buff	- [in]	Max Recv Packet Size
		idle_time	- [in]  원격 시스템과의 통신 유무 (Alive Check) 체크하기 위한 시간 (단위: 밀리초)
							즉, 이 시간 (단위: 초) 동안 아무런 통신이 없으면 Alive Check 송신
							이 값이 0 인 경우, 이 기능을 사용하지 않음
		link_time	- [in]  원격 시스템에 접속이 되지 않을 때, 주기적으로 접속하는 시간 (단위: 밀리초)
		sock_time	- [in]  소켓 이벤트 (송/수신) 감지 대기 시간 (단위: 밀리초)
							해당 주어진 값(시간)만큼만 소켓 이벤트 감지후 다음 단계로 루틴 이동
		live_time	- [in]  Client가 Server에게 주기적으로 Live Check를 송신하는 시간 (단위: 밀리초)
 retn : None
*/
CClientThread::CClientThread(ENG_EDIC sys_id,
							 UINT16 th_id,
							 UINT16 tcp_port,
							 UINT32 source_ip,
							 UINT32 target_ip,
							 UINT32 port_buff,
							 UINT32 recv_buff,
							 UINT32 ring_buff,
							 UINT32 idle_time,
							 UINT32 link_time,
							 UINT32 sock_time,
							 UINT32 live_time)
	: CTcpThread(sys_id,
				 th_id,
				 tcp_port,
				 source_ip,
				 target_ip,
				 port_buff,
				 recv_buff,
				 ring_buff,
				 sock_time)
{
	m_bInitRecv		= FALSE;		/* 초기 수신된 데이터가 존재하지 않음 */
	m_u32LinkTime	= link_time;	/* 재접속하는 주기 설정 (단위: 밀리초) */
	m_u32IdleTime	= idle_time;	/* 해당시간 동안 통신 작업이 없으면 기존 통신 연결 해제 */
	m_u32LiveTime	= live_time;	/* 해당시간 마다 Server에게 Alive Check 정보 송신 진행 */
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CClientThread::~CClientThread()
{
}

/*
 desc : 주기적으로 호출됨
 parm : None
 retn : None
*/
VOID CClientThread::RunWork()
{
	/* 현재 연결된 Client가 존재하지 않으면, 재접속 수행 */
	if (!m_bIsLinked)
	{
		if (m_enSocket == ENG_TPCS::en_closed || m_enSocket == ENG_TPCS::en_linking)
		{
			/* 임의의 시간마다 재접속을 수행하도록 하기 위함 */
			if (IsTcpWorkConnect(m_u32LinkTime))
			{
				/* 접속 작업 수행 (소켓 초기화 이후 접속 시도) */
				if (!DoConnect())	m_i32SockError = WSAGetLastError();
			}

			/* Socket Event 감지처리 (연결되었다면, 가상함수 호출) */
			if (m_enSocket == ENG_TPCS::en_linking)
			{
				if (!IsLinkEvent())	return;
				/* 정상적으로 연결되었을 때, 초기화 수행 */
				Connected(m_sdClient);
			}
		}
	}
	else
	{
		/* Socket Event 감지 확인 */
		if (0x02 == PopSockEvent())
		{
			TCHAR tzMesg[128]	= {NULL};
			CUniToChar csCnv;
			swprintf_s(tzMesg, 128,
					   L"Disconnected from the remote system (%s) / err_cd:%d)",
					   m_tzTCPInfo, m_i32SockError);
			LOG_ERROR(ENG_EDIC::en_bsa, tzMesg);
		}

		/* 임의 시간마다 Alive Time 정보 송신 */
		if (m_u32LiveTime && IsTcpWorkIdle(m_u32LiveTime))
		{
			/* EFEM Robot 장비인 경우만 해당됨 */
			if (ENG_EDIC::en_efem == m_enSysID)
			{
				SendAliveTime();
			}
		}

		/* DoWork 가상 함수 호출 */
		DoWork();
	}
}

/*
 desc : Release tcp/ip socket
 parm : sd		- [in]  Socket descriptor
		time_out- [in]  Please refer to the LINGER structure (= linger time)
						If the value is 0, I disconnected immediately without any delay
 retn : None
*/
VOID CClientThread::CloseSocket(SOCKET sd, UINT32 time_out)
{
	CTcpThread::CloseSocket(sd, time_out);

}

/*
 desc : Perform a bind operation for the client socket to connect to the remote server
 parm : None
 retn : TRUE or FALSE
*/
BOOL CClientThread::BindSock()
{
	SOCKADDR_IN stAddrIn= {NULL};

	if (INVALID_SOCKET == m_sdClient)	return FALSE;

	/* Setup the socket information */
	stAddrIn.sin_family	= AF_INET;
	stAddrIn.sin_port	= 0;	/* In this case, the port number is a value of 0 at all times */
	InetPtonW(AF_INET, m_tzSourceIPv4, &stAddrIn.sin_addr.s_addr);
	/* Socket binding ... */
	if (SOCKET_ERROR == ::bind(m_sdClient, (LPSOCKADDR)&stAddrIn, sizeof(SOCKADDR_IN)))
	{
		m_i32SockError = WSAGetLastError();
		return FALSE;
	}
	return TRUE;
}

/*
 desc : Attempt to connect to remote server to initialize client socket
 parm : None
 retn : TRUE or FALSE
*/
BOOL CClientThread::DoConnect()
{
	INT32 i32Option		= 1;
	BOOL bSucc			= FALSE;
	LONG32 l32Event		= FD_CONNECT;	/* FD_CLOSE */
	SOCKADDR_IN stAddrIn= {NULL};

	/* 초기 연결되고 난 이후 수신된 데이터가 없다고 설정 */
	m_bInitRecv			= FALSE;
	/* Server or Client의 Listen or Client Socket 재초기화에 따른 시간 갱신 */
	m_u64ConnectTime	= GetTickCount64();

	/* Ring Buffer 초기화 */
	m_pPktRingBuff->ResetBuff();

	/* Remove existing client socket exists */
	if (INVALID_SOCKET != m_sdClient)
	{
		if (!IsRunDemo())	LOG_WARN(ENG_EDIC::en_bsa, L"Release an exisiting connected socket");
		CloseSocket(m_sdClient);
	}

	/* 접속을 시도하므로 플래그 설정 */
	m_enSocket	= ENG_TPCS::en_linking;
	/* Setup the client socket information */
	memset(&stAddrIn, 0x00, sizeof(SOCKADDR_IN));
	stAddrIn.sin_family	= AF_INET;
	stAddrIn.sin_port	= htons(m_u16TcpPort);
	InetPtonW(AF_INET, m_tzTargetIPv4, &stAddrIn.sin_addr.s_addr);
	/* Create a client socket */
	bSucc = (INVALID_SOCKET != (m_sdClient = WSASocket(AF_INET,
													   SOCK_STREAM,
													   IPPROTO_TCP,
													   NULL,
													   NULL,
													   WSA_FLAG_OVERLAPPED)));
	/* Setup socketoption - reuse IPv4 */
	if (bSucc)
	{
		bSucc = (SOCKET_ERROR != setsockopt(m_sdClient,
											SOL_SOCKET,
											SO_REUSEADDR,
											(char *)&i32Option, sizeof(i32Option)));
	}
	/* Bind a client socket */
	if (bSucc)	bSucc = BindSock();
	/* Setup socket event (Only FD_CONNECT) */
	if (bSucc)
	{
		bSucc = (SOCKET_ERROR != WSAEventSelect(m_sdClient, m_wsaClient, l32Event));
	}
	/* Trying to connect!!! */
	if (bSucc)
	{
		TCHAR tzMesg[LOG_MESG_SIZE]	= {NULL};
		CUniToChar csCnv;
		swprintf_s(tzMesg, LOG_MESG_SIZE,
				   L"Attempting to connect to the remote system (%s)", m_tzTCPInfo);
		LOG_MESG(ENG_EDIC::en_engine, tzMesg);
		/* ------------------------------------------------------------------------------------- */
		bSucc = (SOCKET_ERROR != connect(m_sdClient, (LPSOCKADDR)&stAddrIn, sizeof(stAddrIn)));
		/* ------------------------------------------------------------------------------------- */
		if (!bSucc)
		{
			bSucc = (WSAGetLastError() == WSAEWOULDBLOCK);
			if (!bSucc)	SocketError(L"Failed to initialize the connection");
		}
	}

	return bSucc;
}

/*
 desc : 소켓 연결 이벤트가 있는지 확인
 parm : None
 retn : TRUE - 연결 이벤트 발생, FALSE - 연결 이벤트 없음
*/
BOOL CClientThread::IsLinkEvent()
{
	INT32 i32WsaRet		= 0;
	UINT32 u32WsaRet	= 0;
	WSANETWORKEVENTS wsaNet;

	/* Socket Event의 Signal 상태 감지하기 */
	u32WsaRet	= WSAWaitForMultipleEvents(1,				/* 소켓 이벤트 감지 개수: 1개임 */
										   &m_wsaClient,	/* 소켓 이벤트 핸들 */
										   FALSE,			/* TRUE : 모든 소켓 이벤트 객체가 신호 상태, FALSE : 1개 이벤트 객체가 신호 상태이면 즉시 리턴 */
										   m_u32SockTime,	/* 임의 시간 (밀리초) 이내에 응답이 없으면 리턴 */
										   FALSE);			/* WSAEventSelect에서는 항상 FALSE */
	if (u32WsaRet == WSA_WAIT_TIMEOUT || u32WsaRet == WSA_WAIT_FAILED)
	{
		if (u32WsaRet == WSA_WAIT_FAILED)
		{
			TCHAR tzMesg[256]	= {NULL};
			swprintf_s(tzMesg, 256, L"The return value of the WSAWaitForMultipleEvents failed (Client.Link). "
									L"(time out) (tgt_ipv4=%s)", m_tzTargetIPv4);
			if (m_enSysID != ENG_EDIC::en_demo)	LOG_WARN(m_enSysID, tzMesg);
		}
		return FALSE;
	}
	/* 구체적인 Network Event 알아내기 */
	i32WsaRet	= WSAEnumNetworkEvents(m_sdClient,			/* 대상 소켓 */
									   m_wsaClient,			/* 대상 소켓과 짝을 이룬 객체 핸들 */
									   &wsaNet);			/* 이벤트 정보가 저장될 구조체 포인터 */

	/* 현재 발생된 이벤트가 연결 이벤트가 아니면 연결해제 후 재접속 시도 */
	if (FD_CONNECT != (FD_CONNECT & wsaNet.lNetworkEvents))
	{
#if 0	/* 일단 아래 CloseSocket 함수 호출하지 말것 (재접속을 받아주지 않음) */
		CloseSocket(m_sdClient);
#endif
		return FALSE;
	}
	else
	{
		/* 연결 에러가 발생 했는지 여부 */
		if (wsaNet.iErrorCode[FD_CONNECT_BIT] != 0)
		{
			TCHAR tzMesg[LOG_MESG_SIZE]	= {NULL};
			CUniToChar csCnv;
			/* 에러 메시지 등록 */
			m_i32SockError = WSAGetLastError();
			if (m_i32SockError == 0)	m_i32SockError = wsaNet.iErrorCode[FD_CONNECT_BIT];
			swprintf_s(tzMesg, LOG_MESG_SIZE,
					   L"Connection denied by remote system (%s) / err_cd:%d",
					   m_tzSourceIPv4, m_i32SockError);
			/* 이전에 패킷 송신 실패한 시간보다 주기가 너무 짧으면 부모에게 알리지 않음 */
			LOG_ERROR(ENG_EDIC::en_engine, tzMesg);
			/* 기존 연결 소켓 해제처리 */
			CloseSocket(m_sdClient);

			return FALSE;
		}
	}

	/* 연결 이벤트라고 반환 */
	return TRUE;
}

/*
 desc : 소켓 이벤트 감지 대기
 parm : None
 retn : 0x00 - 실패, 0x01 - 접속, 0x02 - 해제, 0x03 - 데이터 수신
*/
UINT8 CClientThread::PopSockEvent()
{
	UINT8 u8RetCode	= 0x00;
	INT32 i32WsaRet	= 0;
	UINT32 u32WsaRet= 0;
	WSANETWORKEVENTS wsaNet;

	/* ----------------------------------------------------------------------------------------- */
	/* 이벤트 객체가 신호 상태가 될 때까지 대기한다. WSAWaitForMultipleEvents() 함수의 반환 값은 */
	/* 신호 상태가 된 이벤트 객체의 배열 인덱스 + WSA_WAIT_EVENT_0 값이다. 그러므로, 실제 인덱스 */
	/* 값을 얻으려면 WSA_WAIT_EVENT_0 (WAIT_BOJECT_0) 값을 빼야 한다.                            */
	/* ----------------------------------------------------------------------------------------- */
	u32WsaRet	= WSAWaitForMultipleEvents(1,				/* 소켓 이벤트 감지 개수: 1 개 */
										   &m_wsaClient,	/* 소켓 이벤트 핸들 */
										   FALSE,			/* TRUE : 모든 소켓 이벤트 객체가 신호 상태, FALSE : 1개 이벤트 객체가 신호 상태이면 즉시 리턴 */
										   m_u32SockTime,	/* 임의 시간 (밀리초) 이내에 응답이 없으면 리턴 */
										   FALSE);			/* WSAEventSelect에서는 항상 FALSE */
	if (u32WsaRet == WSA_WAIT_TIMEOUT || u32WsaRet == WSA_WAIT_FAILED)
	{
		if (u32WsaRet == WSA_WAIT_FAILED)
		{
			TCHAR tzMesg[256]	= {NULL};
			swprintf_s(tzMesg, 256, L"The return value of the WSAWaitForMultipleEvents failed (Client.Pop). "
									L"(time out) (tgt_ipv4=%s)", m_tzTargetIPv4);
			if (m_enSysID != ENG_EDIC::en_demo)	LOG_MESG(m_enSysID, tzMesg);
		}
		SetCycleTime(NORMAL_THREAD_SPEED);
		return 0x00;
	}
	/* 구체적인 Network Event 알아내기 */
	u32WsaRet	-= WSA_WAIT_EVENT_0;
	i32WsaRet	= WSAEnumNetworkEvents(m_sdClient,	/* 대상 소켓 */
									   m_wsaClient,	/* 대상 소켓과 짝을 이룬 객체 핸들 */
									   &wsaNet);	/* 이벤트 정보가 저장될 구조체 포인터 */

	/* 다음 스레드 동작 속도를 높이기 위한 처리 (데이터 수신모드일 경우) */
	SetCycleTime(0);

	/* Socket Event 별 분리 처리 진행 */
	if (FD_READ == (FD_READ & wsaNet.lNetworkEvents))
	{
		/* 통신 포트로부터 데이터 읽기 시도 */
		if (wsaNet.iErrorCode[FD_READ_BIT] == 0)
		{
			if (Received(m_sdClient))	u8RetCode = 0x03;
		}
		/* 데이터 읽기 실패인 경우, 기존 통신 포트의 버퍼 모두 읽어서 버리기 */
		else
		{
			ReceiveAllData(m_sdClient);
		}
	}
	/* 서버와 연결이 해제 혹은 기타 이벤트 (Send or Connected)인 경우, 강제 연결해제 처리 */
#if 1
	else if (FD_CLOSE == (FD_CLOSE & wsaNet.lNetworkEvents))
#else
	else
#endif
	{
#if 0
		if (!IsRunDemo())
		{
			LOG_WARN(ENG_EDIC::en_engine, L"The connection to the remote system has been disconnected (Client.Pop).");
		}
#endif
		CloseSocket(m_sdClient);
		u8RetCode	= 0x02;
	}

	return u8RetCode;
}

/*
 desc : Server와의 접속이 이루어진 경우, 초기 한번 호출됨
 parm : sd	- [in]  접속이 이루어진 Client Socket Descriptor
 retn : None
*/
VOID CClientThread::Connected(SOCKET sd)
{
	BOOL bSucc		= TRUE;
	LONG32 l32Event	= FD_READ|FD_CLOSE;
	CUniToChar csCnv;

	/* 일단 연결되었다는 시그널이 도착했으므로, DoWork의 연결시도를 막기 위함 */
	m_enSocket	= ENG_TPCS::en_linked;

	/* 연결되었으므로, 재접속을 하지 못하도록 시간을 갱신 */
	UpdateWorkTime();
	// 소켓 옵션 설정하기
	if (bSucc)	bSucc = SetSockOptBuffSize(0x11, sd);	/* 8 KBytes가 표준임 */
//	if (bSucc)	bSucc = SetSockOptSpeed(1, sd);
	if (bSucc)	bSucc = SetSockOptRoute(1, sd);
//	if (bSucc)	bSucc = SetSockOptAlive(1, sd);
	/* 소켓 옵션 설정 실패 */
	if (bSucc)
	{
		/* Setup socket event (Combination of FD_READ and FD_CLOSE) */
		bSucc = (SOCKET_ERROR != WSAEventSelect(sd, m_wsaClient, l32Event));
		/* 에러인 경우, 에러 코드 정보 저장 */
		if (!bSucc)	m_i32SockError	= WSAGetLastError();
	}

	/* 접속 허용이 성공적이면 가상함수 호출 */
	if (bSucc)	bSucc	= Connected();
	/* 서버와 접속된 시간 저장 */
	if (bSucc)	GetLocalTime(&m_stLinkTime);

	/* 에러인 경우, 에러 메시지 처리 */
	if (!bSucc)
	{
		CloseSocket(sd);	/* 이전에 패킷 송신 실패한 시간보다 주기가 너무 짧으면 부모에게 알리지 않음 */
	}
	else
	{
		/* 연결 되었다고 플래그 및 메시지 저장 */
		m_bIsLinked	= TRUE;
#ifndef _DEBUG
		LOG_MESG(ENG_EDIC::en_engine, L"The connection to the remote system has been connected");
#endif
	}
}

