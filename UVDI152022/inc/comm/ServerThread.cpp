
/* desc : Server TCP/IP Thread */

#include "pch.h"
#include "ServerThread.h"


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
		sock_time	- [in]  소켓 이벤트 (송/수신) 감지 대기 시간 (단위: 밀리초)
							해당 주어진 값(시간)만큼만 소켓 이벤트 감지후 다음 단계로 루틴 이동
		idle_time	- [in]  주어진 시간 동안 연결이 없는, Listen 소켓을 재초기화 하는 시간 (단위: 밀리초)
 retn : None
*/
CServerThread::CServerThread(ENG_EDIC sys_id,
							 UINT16 th_id,
							 UINT16 tcp_port,
							 UINT32 source_ip,
							 UINT32 target_ip,							 
							 UINT32 port_buff,
							 UINT32 recv_buff,
							 UINT32 ring_buff,
							 UINT32 idle_time,
							 UINT32 sock_time)
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
	m_u64IdleTime	= idle_time;	// Listen Socket 주기적으로 초기화하는 시간 설정 (단위: 밀리초)
	/* Check whether it operates in demo mode */
	if (GetConfig()->IsRunDemo())	m_enSysID = ENG_EDIC::en_demo;
	// 소켓 초기화 및 이벤트 생성
	m_sdListen		= INVALID_SOCKET;
	m_wsaListen		= WSACreateEvent();
	if (WSA_INVALID_EVENT == m_wsaListen)
		AfxMessageBox(L"Failed to create the socket event <Server Listen>", MB_ICONSTOP|MB_TOPMOST);
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CServerThread::~CServerThread()
{
	// Release Socket 및 Event
	if (INVALID_SOCKET != m_sdListen)		CloseSocket(m_sdListen);
	if (WSA_INVALID_EVENT != m_wsaListen)	WSACloseEvent(m_wsaListen);
}

/*
 desc : The main function of the thread
 parm : None
 retn : None
*/
VOID CServerThread::RunWork()
{
	UINT8 u8PopRet	= 0x00;

	/* ------------------------------------------------------- */
	/* 연결된 Client 존재하지 않으면, Listen Socket 재초기화 */
	/* ------------------------------------------------------- */
#ifndef _DEBUG
	// 주어진 시간동안 전혀 통신이 이루어지지 않으면 서버 소켓 재초기화
	if ((m_sdListen == INVALID_SOCKET || m_u64IdleTime > 0) &&
		m_u64IdleTime < (GetTickCount64() - m_u64WorkTime))
	{
		if(!InitListen())
		{
			m_i32SockError	= WSAGetLastError();
			CloseSocket(m_sdClient);
			return;
		}
	}
#else
	if (m_sdListen == INVALID_SOCKET)
	{
		if (!InitListen())
		{
			m_i32SockError = WSAGetLastError();
			CloseSocket(m_sdClient);
			return;
		}
	}
#endif

	/* --------------------- */
	/* Socket Event 감지처리 */
	/* --------------------- */
	u8PopRet = PopSockEvent();
	if (u8PopRet == 1 || u8PopRet == 2)
	{
		TCHAR tzMesg[128]	= {NULL};
		if (0x01 == u8PopRet)	// 연결된 Client가 존재하는 경우
		{
			swprintf_s(tzMesg, 128, L"The access of <%s> has been successful", m_tzTCPInfo);
			LOG_MESG(ENG_EDIC::en_engine, tzMesg);
		}
		else					// 해제된 Client가 존재하는 경우
		{
			swprintf_s(tzMesg, 128, L"The access of <%s> has been failed", m_tzTCPInfo);
			SocketError(tzMesg);
		}
	}
	/* --------------------- */
	/* DoWork 가상 함수 호출 */
	/* --------------------- */
	DoWork();
}

/*
 desc : Release tcp/ip socket
 parm : sd		- [in]  Socket descriptor
		time_out- [in]  Please refer to the LINGER structure (= linger time)
						If the value is 0, I disconnected immediately without any delay
 retn : None
*/
VOID CServerThread::CloseSocket(SOCKET sd, UINT32 time_out)
{
	/* --------------------------------------------------------- */
	/* 부모 쪽의 CloseSocket 함수 호출하기 전에 수행할 업무 진행 */
	/* --------------------------------------------------------- */
	// 상위 부모 함수 호출
	CTcpThread::CloseSocket(sd, time_out);
	// Socket 종류에 따라 초기화 방법 다르게 처리
	if (sd == m_sdListen)	m_sdListen	= INVALID_SOCKET;
	if (sd == m_sdClient)	m_sdClient	= INVALID_SOCKET;
}

/*
 desc : 소켓 초기화 즉, Server의 Listen socket 초기화 작업 진행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CServerThread::InitListen()
{
	TCHAR tzMesg[128]	= {NULL};
	INT32 i32Option		= 1;
	BOOL bSucc		= TRUE;
	LONG32 l32Event		= FD_ACCEPT;
	DWORD dw32Error		= 0;
	SOCKADDR_IN stAddrIn= {NULL};

	// 기존 Listen 소켓이 초기화되지 않았다면 ...
	if (INVALID_SOCKET != m_sdListen)	CloseSocket(m_sdListen);
	// 기존 Client 소켓이 초기화되지 않았다면 ...
	if (INVALID_SOCKET != m_sdClient)	CloseSocket(m_sdClient);
#if 0
	/* -------------------------------------------------------------------- */
	/* Server or Client의 Listen or Client Socket 재초기화에 따른 시간 갱신 */
	/* -------------------------------------------------------------------- */
	m_u32ResetTime			= (UINT32)GetTickCount();
#endif
	// 소켓 설정
	stAddrIn.sin_family		= AF_INET;
	stAddrIn.sin_port		= htons(m_u16TcpPort);
	/* 물리적인 Land Card의 IPv4 주소 입력하면 되지만, 가상으로 부여한 IPv4는 실패 함 */
#if 0
	InetPtonW(AF_INET, m_tzSourceIPv4, &stAddrIn.sin_addr.s_addr);
#else
	stAddrIn.sin_addr.s_addr= htonl(INADDR_ANY);
#endif

	// 소켓 생성
#if 0
	bSucc = (INVALID_SOCKET != (m_sdListen = WSASocket(AF_INET,
													   SOCK_STREAM,
													   IPPROTO_TCP,
													   NULL,
													   NULL,
													   WSA_FLAG_OVERLAPPED)));
#else
	bSucc = (INVALID_SOCKET != (m_sdListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)));
#endif
	// 소켓 옵션 설정 - IP 주소 재사용
	if (bSucc)
	{
		bSucc = (SOCKET_ERROR != setsockopt(m_sdListen,
											SOL_SOCKET,
											SO_REUSEADDR,
											(CHAR *)&i32Option,
											sizeof(i32Option)));
	}
	// 소켓 및 설정된 주소 바인딩
	if (bSucc)
	{
		bSucc = (SOCKET_ERROR != ::bind(m_sdListen,
										(LPSOCKADDR)&stAddrIn,
										sizeof(SOCKADDR_IN)));
		dw32Error = WSAGetLastError();
	}
	// 서버 대기 설정. 동시 5 개의 연결 보장
	if (bSucc)
	{
		swprintf_s(tzMesg, 128, L"Waiting for listen the socket <%s>", m_tzTCPInfo);
		LOG_MESG(ENG_EDIC::en_engine, tzMesg);
		bSucc = (SOCKET_ERROR != listen(m_sdListen, 5));
		if (!bSucc)
		{
			swprintf_s(tzMesg, 128, L"Failed to listen the socket <%s>", m_tzTCPInfo);
			SocketError(tzMesg);
		}
	}
	// 소켓 이벤트 설정 (FD_ACCEPT만 설정!!!)
	if (bSucc)
	{
		bSucc = (SOCKET_ERROR != WSAEventSelect(m_sdListen, m_wsaListen, l32Event));
	}
	// 성공하건 실패하건, 서버 소켓 다시 Listen하기 위한 일정 대기 시간 설정을 위해
	UpdateWorkTime();
	// 현재 연결을 기다리고 있다고 설정
	m_enSocket	= ENG_TPCS::en_linking;

	// 부모에게 알림
	return bSucc;
}

/*
 desc : 소켓 이벤트 감지
 parm : None
 retn : 0x00 - 실패, 0x01 - 접속, 0x02 - 해제, 0x03 - 데이터 수신
*/
UINT8 CServerThread::PopSockEvent()
{
	UINT8 u8RetCode	= 0x00;
	INT32 i32WsaRet	= 0;
	UINT32 u32WsaRet= 0;
	SOCKET sdSock[2]	= {m_sdListen, m_sdClient};
	WSAEVENT wsaSock[2]	= {m_wsaListen, m_wsaClient};
	WSANETWORKEVENTS wsaNet;

	// Socket Event의 Signal 상태 감지하기
	u32WsaRet	= WSAWaitForMultipleEvents(2,				// 소켓 이벤트 감지 개수: 1개임
										   wsaSock,			// 소켓 이벤트 핸들 (Listen or Client)
										   FALSE,			// TRUE : 모든 소켓 이벤트 객체가 신호 상태, FALSE : 1개 이벤트 객체가 신호 상태이면 즉시 리턴
										   m_u32SockTime,	// 임의 초 이내에 응답이 없으면 리턴
										   FALSE);			// WSAEventSelect에서는 항상 FALSE
	if (u32WsaRet == WSA_WAIT_TIMEOUT || u32WsaRet == WSA_WAIT_FAILED)
	{
		if (u32WsaRet == WSA_WAIT_FAILED)
		{
			TCHAR tzMesg[256]	= {NULL};
			swprintf_s(tzMesg, 256, L"The return value of the WSAWaitForMultipleEvents failed (Server). "
									L"(time out) (tgt_ipv4=%s)", m_tzTargetIPv4);
			if (m_enSysID != ENG_EDIC::en_demo)	LOG_MESG(m_enSysID, tzMesg);
		}
		SetCycleTime(NORMAL_THREAD_SPEED);
		return 0x00;
	}
	/* ----------------------------------------------------------------------------------------- */
	/* 이벤트 객체가 신호 상태가 될 때까지 대기한다. WSAWaitForMultipleEvents() 함수의 반환 값은 */
	/* 신호 상태가 된 이벤트 객체의 배열 인덱스 + WSA_WAIT_EVENT_0 값이다. 그러므로, 실제 인덱스 */
	/* 값을 얻으려면 WSA_WAIT_EVENT_0 (WAIT_BOJECT_0) 값을 빼야 한다.                            */
	/* ----------------------------------------------------------------------------------------- */
	u32WsaRet	-= WSA_WAIT_EVENT_0;
	i32WsaRet	= WSAEnumNetworkEvents(sdSock[u32WsaRet],	// 대상 소켓
									   wsaSock[u32WsaRet],	// 대상 소켓과 짝을 이룬 객체 핸들
									   &wsaNet);			// 이벤트 정보가 저장될 구조체 포인터
	// 소켓이 에러인지 확인
	if (i32WsaRet == SOCKET_ERROR)	return 0x00;

	/* ------------------------------ */
	/* Socket Event 별 분리 처리 진행 */
	/* ------------------------------ */
	if (FD_READ == (FD_READ & wsaNet.lNetworkEvents))
	{
		// 통신 포트로부터 데이터 읽기 시도
		if (wsaNet.iErrorCode[FD_READ_BIT] == 0)
		{
			if (Received(sdSock[1]))	u8RetCode = 0x03;
		}
		// 데이터 읽기 실패인 경우, 기존 통신 포트의 버퍼 모두 읽어서 버리기
		else
		{
			ReceiveAllData(sdSock[1]);
		}
	}
	// 클라이언트로 연결이 들어온 상태인지 ...
	else if (FD_ACCEPT == (FD_ACCEPT & wsaNet.lNetworkEvents))
	{
		// 연결 에러가 발생 했는지 여부
		if (wsaNet.iErrorCode[FD_ACCEPT_BIT] != 0)
		{
			m_i32SockError = WSAGetLastError();
		}
		// 연결이 서버로부터 정상적으로 승인받은 경우
		else
		{
			// 접속되었다고 플래그 설정
			m_bIsLinked	= TRUE;
			// 가상함수 호출
			Accepted(m_sdListen);
			u8RetCode	= 0x01;
		}
	}
	// 서버와 연결이 해제인지
	else if (FD_CLOSE == (FD_CLOSE & wsaNet.lNetworkEvents))
	{
		CloseSocket(sdSock[1]);
		u8RetCode	= 0x02;
	}

	return u8RetCode;
}

/*
 desc : Client로부터 접속 시그널이 도착한 경우
 parm : sd	- Server Socket Descriptor (Listen Socket)
 retn : None
*/
VOID CServerThread::Accepted(SOCKET sd)
{
	TCHAR tzIPv4[IPv4_LENGTH]	= {NULL};
	INT32 i32AddrLen			= sizeof(SOCKADDR_IN);
	BOOL bSucc					= TRUE;
	LONG32 l32Event				= FD_READ|FD_CLOSE;
	SOCKET sdClient				= INVALID_SOCKET;
	IN_ADDR stInAddr			= {NULL};
	SOCKADDR_IN sdAddrIn		= {NULL};

	/* ----------------------------------------------------------- */
	/* 연결된 클라이언트가 있으므로, 일단 연결되었다고 플래그 설정 */
	/* ----------------------------------------------------------- */
	m_enSocket = ENG_TPCS::en_linked;
	/* ------------------------------------------------------- */
	/* 연결되었으므로, 소켓 초기화를 하지 못하도록 시간을 갱신 */
	/* ------------------------------------------------------- */
	UpdateWorkTime();

	// 새롭게 접속된 클라이언트에 대해 일단 접속 승인 처리
	sdClient	= WSAAccept(sd, (LPSOCKADDR)&sdAddrIn, &i32AddrLen, 0, 0);
	memcpy(&stInAddr.s_addr, &sdAddrIn.sin_addr.s_addr, sizeof(UINT32));
	// 접속 승인할 때, 에러가 발생 했는지 여부 확인
	if (INVALID_SOCKET == sdClient)
	{
		SocketError(L"Invalid the connected ip of client");
		m_i32SockError	= WSAGetLastError();
		bSucc	= FALSE;
	}
	/* -------------------------------------------------------------------------- */
	/* 접속된 클라이언트가 기존의 등록된 Client IPv4 주소가 아니면 접속 해제 처리 */
	/* -------------------------------------------------------------------------- */
	InetNtopW(AF_INET, (CONST VOID*)&stInAddr, tzIPv4, IPv4_LENGTH);
	if (bSucc && (0 != wcscmp(m_tzTargetIPv4, tzIPv4)))
	{
		// 현재 연결된 Client 접속 해체 처리
		CloseSocket(sdClient);
		bSucc	= FALSE;
		return;
	}
	/* ------------------------------------------------- */
	/* 만약 기존에 접속된 Client가 있다면 연결 해제 처리 */
	/* ------------------------------------------------- */
	if (INVALID_SOCKET != m_sdClient)
	{
		SocketError(L"Disconnect already the connected ip of client");
		CloseSocket(m_sdClient);
		m_sdClient	= INVALID_SOCKET;
	}
	/* --------------------------------------- */
	/* 현재 연결된 소켓의 다양한 옵션 설정하기 */
	/* --------------------------------------- */
	if (bSucc)	bSucc = SetSockOptBuffSize(0x11, sdClient);	// 8 KBytes가 표준임
//	if (bSucc)	bSucc = SetSockOptSpeed(1, sdClient);
	if (bSucc)	bSucc = SetSockOptRoute(1, sdClient);
//	if (bSucc)	bSucc = SetSockOptAlive(1, sdClient);
	if (bSucc)
	{
		// 소켓 이벤트 설정 (FD_CLOSE|FD_READ 설정!!!)
		bSucc = (SOCKET_ERROR != WSAEventSelect(sdClient, m_wsaClient, l32Event));
		if (!bSucc)
		{
			SocketError(L"Failed to setup the socket event");
		}
	}
	/* ------------------ */
	/* Ring Buffer 초기화 */
	/* ------------------ */
	m_pPktRingBuff->ResetBuff();
	// 통신 시간 갱신
	UpdateWorkTime();
	/* ------------------------------------ */
	/* 접속 허용이 성공적이면 가상함수 호출 */
	/* ------------------------------------ */
	if (!bSucc)	CloseSocket(sdClient);
	else
	{
		/* ------------------------------------------ */
		/* 가장 최근에 접속한 Client의 소켓 핸들 저장 */
		/* ------------------------------------------ */
		m_sdClient	= sdClient;	// 반드시 Accepted 함수 이전에 설정(저장)되어야 함
		// 접속 승인에 따른 초기화 작업 진행
		if (!Accepted())
		{
			CloseSocket(m_sdClient);
			m_sdClient	= INVALID_SOCKET;
		}
		else
		{
			// 클라이언트가 접속한 시간 저장
			GetLocalTime(&m_stLinkTime);
		}
	}
}

/*
 desc : 현재 연결된 Client Socket 연결 해제 처리
 parm : None
 retn : None
*/
VOID CServerThread::CloseClient()
{
	if (INVALID_SOCKET != m_sdClient)	CloseSocket(m_sdClient);
}

/*
 desc : 현재 Server Socket 해제 처리
 parm : None
 retn : None
*/
VOID CServerThread::CloseServer()
{
	if (INVALID_SOCKET != m_sdListen)	CloseSocket(m_sdListen);
	if (INVALID_SOCKET != m_sdClient)	CloseSocket(m_sdClient);
}
