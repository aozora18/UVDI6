
/* desc : 소켓 기본 클래스 */

#include "pch.h"
#include "TcpThread.h"

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
		port_size	- [in]  TCP/IP Port Buffer Size
		recv_size	- [in]  Send or Recv의 패킷 중 가장 큰 패킷의 크기 (단위: Bytes)
		ring_size	- [in]	Max Recv Packet Size
		wait_time	- [in]  기본 30 밀리초 동안 소켓 이벤트 감지를 위한 대기
		recv_count	- [in]  연속적으로 수신되는 데이터를 저장할 최소 버퍼 개수 할당
 retn : None
*/
CTcpThread::CTcpThread(ENG_EDIC sys_id,
					   UINT16 th_id,
					   UINT16 tcp_port,
					   UINT32 source_ip,
					   UINT32 target_ip,
					   UINT32 port_size,
					   UINT32 recv_size,	// Packet 버퍼의 최대 크기 (단위: Bytes)
					   UINT32 ring_size,	// Ring Buffer 크기 (단위: Bytes)
					   UINT32 sock_time,
					   UINT32 recv_count)
{
	IN_ADDR stInAddr	= {NULL};

	// 연결 or 접속 안되었다고 설정
	m_bIsLinked			= FALSE;
	m_bIsStopSend		= FALSE;
	m_enSysID			= sys_id;
	m_u16ThreadID		= th_id;
	m_u16TcpPort		= tcp_port;
	m_u32PortBuff		= port_size;
	m_u32RecvBuff		= recv_size;
	if (ring_size > recv_size)	m_u32RingBuff	= ring_size;
	else						m_u32RingBuff	= recv_size * 3;	/* 보통 2 배 이상. 안정적으로 3배 이상 */
	/* 최소 기다리는 시간이 0 값이면 기본 값으로 10을 넣어주면 됩니다.*/
	m_u32SockTime		= sock_time < 10 ? 10 : sock_time;	// 기본 10 밀리초 마다 소켓 이벤트 감지를 위한 대기
	/* ----------------------------------------------------------------------------- */
	/* Server or Client 쪽에서 Listen or Client Socket을 초기화한 시점의 시간을 저장 */
	/* 초기에는 바로 Listen or Connect를 수행해야 하기 때문에 0 값으로 초기화를 수행 */
	/* ----------------------------------------------------------------------------- */
	m_u64ConnectTime	= 0;
	/* ----------------------------------------------------------------------------- */
	/* 보통은 0 값으로 초기화 되나, 특정 장비로부터 수신 이벤트가 발생한 경우, 데이터*/
	/* 수신 작업 즉, Socket의 recv 함수 호출을 일정 기간 및 횟수를 반복적으로 수행하 */
	/* 여 더 이상 수신된 데이터가 없는지 확인한 후, 읽기 작업을 끝내야만 하는 경우가 */
	/* 있으며, 이럴 경우, 아래 두 개의 변수에 읽기 반복 횟수와 반복 읽기 이전에 대기 */
	/* 하는 시간 (단위: msec) 값을 설정해 줘야 한다. 이 부분은 꼭 필요할 때만 사용함 */
	/* ----------------------------------------------------------------------------- */
	m_u32RecvRepeat		= 0;	/* 만약 이 값이 3인 경우, Socket의 recv 함수를 3번 반복 호출 */
	m_u32RecvSleep		= 0;	/* 만약 이 값이 50인 경우, Socket의 recv 함수 호출 이전에 50 msec 대기함 */
	m_i32SockError		= 0;	/* 소켓 에러 초기화 */

	/* IP 주소 변환: DWORD -> String 변환 */
	wmemset(m_tzSourceIPv4, 0x00, IPv4_LENGTH);
	memcpy(&stInAddr.s_addr, &source_ip, sizeof(UINT32));
	InetNtopW(AF_INET, (CONST VOID*)&stInAddr, m_tzSourceIPv4, IPv4_LENGTH);

	wmemset(m_tzTargetIPv4, 0x00, IPv4_LENGTH);
	memcpy(&stInAddr.s_addr, &target_ip, sizeof(UINT32));
	InetNtopW(AF_INET, (CONST VOID*)&stInAddr, m_tzTargetIPv4, IPv4_LENGTH);

	/* TCP/IP 접속 정보 */
	swprintf_s(m_tzTCPInfo, 128, L"<src:%s> <tgt:%s> <port:%d> <th_id:%d>",
			   m_tzSourceIPv4, m_tzTargetIPv4, m_u16TcpPort, m_u16ThreadID);

	/* 수신된 패킷 데이터를 임시로 저장할 버퍼 */
	m_pRecvQue	= new CPktQue(recv_count);
	ASSERT(m_pRecvQue);
	/* 스레드 내 루프 속조 제어를 위한 정보 초기화 */
	m_bIsInited	= InitData();
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CTcpThread::~CTcpThread()
{
	/* Release Client Socket */
	if (INVALID_SOCKET != m_sdClient)		CloseSocket(m_sdClient);
	/* Socket Event 해제 */
	if (WSA_INVALID_EVENT != m_wsaClient)	WSACloseEvent(m_wsaClient);

	// 수신 버퍼 메모리 해제
	if (m_pPktRecvBuff)	delete [] m_pPktRecvBuff;
	if (m_pPktRingBuff)	delete m_pPktRingBuff;
	// 임시 패킷 버퍼 메모리 해제
	delete m_pRecvQue;
}

/*
 desc : 통신 스레드 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CTcpThread::InitData()
{
	/* 멤버 초기화 */
	m_sdClient	= INVALID_SOCKET;
	m_wsaClient	= WSA_INVALID_EVENT;
	m_enSocket	= ENG_TPCS::en_closed;

	/* initialize the member variable */
	m_u64WorkTime	= 0;
	memset(&m_stWorkTime, 0x00, sizeof(SYSTEMTIME));
	memset(&m_stLinkTime, 0x00, sizeof(SYSTEMTIME));

	/* 수신 버퍼 초기화 (CRingBuff 클래스 적용) */
	if (!InitMemory())	return TRUE;

	return TRUE;
}

/*
 desc : 송/수신 패킷 저장 관련 고정 메모리 할당 및 초기화
 parm : None
 retn : TRUE - 생성 성공, FALSE - 실패
*/
BOOL CTcpThread::InitMemory()
{
	m_pPktRingBuff	= new CRingBuff;
	ASSERT(m_pPktRingBuff);
	m_pPktRingBuff->Create(m_u32RingBuff+1);
	m_pPktRingBuff->ResetBuff();

	/* 한번 수신 받을 때, 저장할 수 있는 패킷 버퍼 할당 */
	m_pPktRecvBuff	= new UINT8 [m_u32RecvBuff+1];
	ASSERT(m_u32RecvBuff);
	memset(m_pPktRecvBuff, 0x00, m_u32RecvBuff+1);

	/* Client Socket Event 생성 */
	m_wsaClient	= WSACreateEvent();
	if (WSA_INVALID_EVENT == m_wsaClient)
	{
		AfxMessageBox(L"Failed to create the socket_event [TcpThread::InitMemory]", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : When thread is exceuted, the first is called only once
 parm : None
 retn : TRUE or FALSE
*/
BOOL CTcpThread::StartWork()
{
	if (!m_bIsInited)	return FALSE;

	/* TODO : */
	return TRUE;
}

/*
 desc : Called by only once at the end before the thread is terminated
 parm : None
 retn : None
*/
VOID CTcpThread::EndWork()
{
}

/*
 desc : 소켓 에러 관련 정보 설정
 parm : mesg	- [in]  에러 메시지
 retn : None
*/
VOID CTcpThread::SocketError(PTCHAR mesg)
{
	UINT32 u32Len	= 0;
	PTCHAR ptzMesg	= {NULL};
	
	if (!mesg)	return;
	u32Len	= (UINT32)_tcslen(mesg);
	u32Len	= (UINT32)uvCmn_GetMultiOf8(u32Len) + 48 /* Socket TCP/IP Setup Error */;

	/* 임시 메모리 할당 */
	ptzMesg	= (PTCHAR)::Alloc(u32Len * sizeof(TCHAR));
	memset(ptzMesg, 0x00, u32Len);

	/* 에러 메시지 설정 */
	m_i32SockError	= WSAGetLastError();
	swprintf_s(ptzMesg, u32Len, L"Socket TCP/IP Setup Error (%s) (code:0x%08x)",
			   mesg, m_i32SockError);

	/* 이전에 패킷 송신 실패한 시간보다 주기가 너무 짧으면 부모에게 알리지 않음 */
	if (!IsRunDemo())	LOG_ERROR(ENG_EDIC::en_engine, ptzMesg);

	/* 메모리 해제 */
	if (ptzMesg)	::Free(ptzMesg);
}

/*
 desc : 소켓 옵션 설정하기 - 송신/수신 버퍼 크기 설정
 parm :	flag	- [in]  0x01:Send Buffer, 0x10:Recv Buffer, 0x11:Send/Recv Buffer
		sd		- [in]  소켓 옵션을 설정한 소켓 기술자
		size	- [in]  Port Buffer Size
 retn : TRUE or FALSE
*/
BOOL CTcpThread::SetSockOptBuffSize(UINT8 flag, SOCKET sd)
{

	if (INVALID_SOCKET == sd)	return FALSE;
	INT32 i32Ret = 0, i32Size = sizeof(int);

	/* 송신 및 수신 버퍼 크기 설정 */
	if ((0 == i32Ret) && (0x01 == (0x01 & flag)))
		i32Ret = setsockopt(sd, SOL_SOCKET, SO_SNDBUF, (CHAR *)&m_u32PortBuff, i32Size);
	if ((0 == i32Ret) && (0x10 == (0x10 & flag)))
		i32Ret = setsockopt(sd, SOL_SOCKET, SO_RCVBUF, (CHAR *)&m_u32PortBuff, i32Size);

	if (i32Ret == SOCKET_ERROR)
	{
		SocketError(L"Socket Option : Buffer Size");
		return FALSE;
	}

	return TRUE;
}

/*
 desc : 소켓 옵션 설정하기 - 송신/수신 타임 아웃 설정
 parm :	flag	- [in]  0x01:Send, 0x10:Recv, 0x11:Send/Recv
		sd		- [in]  소켓 옵션을 설정한 소켓 기술자
		delay	- [in]  송신/수신 타임 아웃 값 설정 (단위: 밀리초)
 retn : TRUE or FALSE
*/
BOOL CTcpThread::SetSockOptTimeout(UINT8 flag, SOCKET sd, INT32 time)
{
	if (INVALID_SOCKET == sd)	return FALSE;
	INT32 i32Ret = 0, i32Size = sizeof(int);

	/* 소켓 송신/수신 타임아웃 값 설정 */
	if ((0 == i32Ret) && (0x01 == (0x01 & flag)))
		i32Ret = setsockopt(sd, SOL_SOCKET, SO_SNDTIMEO, (CHAR *)&time, i32Size);
	if ((0 == i32Ret) && (0x10 == (0x10 & flag)))
		i32Ret = setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (CHAR *)&time, i32Size);

	if (i32Ret == SOCKET_ERROR)
	{
		SocketError(L"Socket Option : Time-out");
		return FALSE;
	}

	return TRUE;
}

/*
 desc : 소켓 옵션 설정하기 - 송/수신 속도 설정 (가능하면 조금씩 여러번 보내지말고 한번에 많이 보내라(Effective TCP))
		WinSock 의경우 기본으로 Nagle 알고리즘이 적용이 되어있습니다. 하지만 몇몇 네트웍 관련 프로그램에서는
		네트웍 의 전송량이나 부하보다는 빠른 응답속도를 더 중요시 여기는 상황이 있습니다.
		그러한 때에는 TCP_NODELAY  라는 옵션을 사용하여 Nagle 알고리즘을 제거 할 수 있습니다.
 parm :	optval	- [in]  0 : Nagle 알고리즘 (가능하면 조금씩 여러번 보내지 말고 한번에 많이 보내라. Effective TCP)
						1 : No Delay 알고리즘 (네트워크의 전송량보다 패킷 전송 속도를 중시하는 경우.)
		sd		- [in]  소켓 옵션을 설정한 소켓 기술자
 retn : TRUE or FALSE
*/
BOOL CTcpThread::SetSockOptSpeed(INT32 optval, SOCKET sd)
{
	/* 소켓 데이터 속도 설정 */
	if (SOCKET_ERROR == setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (CHAR *)&optval, sizeof(optval)))
	{
		SocketError(L"Socket Option : Speed");
		return FALSE;
	}
	return TRUE;
}

/*
 desc : 일정 시간동안 해당 소켓을 통해 상대방의 소켓 연결 상태를 확인하기 위해 주고 받기를 설정한다.
		즉, 일정 시간동안 상호간의 응답이 없다면, 내부적으로 통산 2시간만에 한번 정도 ACK 시그널을 주고 받는다.
 parm :	optval	- [in]  0 : alive check 안함
				  [in]  1 : alive check 사용함
		sd		- [in]  소켓 옵션을 설정한 소켓 기술자
 retn : TRUE or FALSE
*/
BOOL CTcpThread::SetSockOptAlive(INT32 optval, SOCKET sd)
{
	/* 소켓 데이터 속도 설정 */
	if (SOCKET_ERROR == setsockopt(sd, SOL_SOCKET, SO_KEEPALIVE, (CHAR *)&optval, sizeof(optval)))
	{
		SocketError(L"Socket Option : Alive check");
		return FALSE;
	}		
	return TRUE;
}

/*
 desc : 라우팅 테이블을 사용할지/말지 설정
 parm :	optval	- [in]  0 : 라우팅 테이블을 사용
				  [in]  1 : 라우팅 테이블을 사용하지 않고, 바인드된 네트워크 인터페이스를 통하여 데이터를 보내도록 설정
		sd		- [in]  소켓 옵션을 설정한 소켓 기술자
 retn : TRUE or FALSE
*/
BOOL CTcpThread::SetSockOptRoute(INT32 optval, SOCKET sd)
{
	/* 소켓 데이터 속도 설정 */
	if (SOCKET_ERROR == setsockopt(sd, SOL_SOCKET, SO_DONTROUTE, (CHAR *)&optval, sizeof(optval)))
	{
		SocketError(L"Socket Option : Route");
		return FALSE;
	}
	return TRUE;
}

/*
 desc : Release tcp/ip socket
 parm : sd		- [in]  Socket descriptor
		time_out- [in]  Please refer to the LINGER structure (= linger time)
						If the value is 0, I disconnected immediately without any delay
 retn : None
*/
VOID CTcpThread::CloseSocket(SOCKET sd, UINT32 time_out)
{
	INT32 i32Code	= 0;
	LINGER stLinger	= {NULL};

	fd_set readfds, errorfds;
	timeval timeout;

	/* Shutdown 함수 호출하기 전에 반드시 아래 플래그부터 설정해 줌 */
	m_enSocket	= ENG_TPCS::en_closed;
	/* 연결 해제되었다고 플래그 설정 */
	m_bIsLinked	= FALSE;
	/* 현재 Client 소켓이 살아 있는지 여부 */
	if (INVALID_SOCKET != sd)
	{
		TCHAR tzMesg[LOG_MESG_SIZE]	= {NULL};
		CUniToChar csCnv;
		/* 연결 해제된 정보 알림 */
		swprintf_s(tzMesg, LOG_MESG_SIZE,
				   L"The connection has been disconnected (%s)", m_tzTargetIPv4);
		SocketError(tzMesg);

		/* Initialize (Release) the send and recv socket buffer */
		i32Code	= shutdown(sd, SD_BOTH);

		/* Wait for socket to fail (ie. closed by other end) */
		if (i32Code != SOCKET_ERROR)
		{
			FD_ZERO(&readfds);
			FD_ZERO(&errorfds);
			FD_SET(sd, &readfds);
			FD_SET(sd, &errorfds);

			timeout.tv_sec	= ULONG(time_out);	/* MAX_LINGER_SECONDS */
			timeout.tv_usec	= 0;
			::select(1, &readfds, NULL, &errorfds, &timeout);
		}

		/* 소켓에 해당되는 포트 초기화 */
		stLinger.l_onoff	= 1;				/* linger on... */
		stLinger.l_linger	= UINT16(time_out);	/* timeout in seconds ... (0 -> hard close) */
		setsockopt(sd, SOL_SOCKET, SO_LINGER, (char FAR*)&stLinger, sizeof(LINGER));

		/* 소켓 해제 및 초기화 */
		i32Code	= closesocket(sd);
	}

	/* Update the communication time for TCP/IP */
	UpdateWorkTime();

	/* 연결 해제된 시간 저장 */
	GetLocalTime(&m_stLinkTime);

	/* 소켓 에러 초기화 */
	m_i32SockError	= 0;
	/* Socket 종류에 따라 초기화 방법 다르게 처리 */
	m_sdClient	= INVALID_SOCKET;
}

/*
 desc : 가장 최근에 통신한 시간 저장 및 관리
 parm : None
 retn : None
*/
VOID CTcpThread::UpdateWorkTime()
{
	m_u64WorkTime = GetTickCount64();
	GetLocalTime(&m_stWorkTime);
}

/*
 desc : 실제 패킷 데이터 송신부
 parm : packet	- [in]  송신 패킷이 저장된 버퍼
		size	- [out] 입력: 'packet' 버퍼에 저장된 패킷 크기
						반환: 전송된 데이터의 크기
 retn : TRUE or FALSE (통신 이상이 발생한 경우)
*/
BOOL CTcpThread::Sended(PUINT8 packet, UINT32 &size)
{
	BOOL bSucc			= TRUE, bRetrySend	= FALSE;
	CHAR *pPktSend		= (CHAR*)packet;
	INT32 i32SendBytes	= 0, i32SendSize = INT32(size), i32FirstSize = i32SendSize;
	UINT32 u32TotalSize	= 0, u32TickTime = GetTickCount();	/* 송신을 무한대로 할 경우, 루프 빠져나오기 위함 */

	/* 송신 전에 무조건 송신 모드로 설정 */
	m_enSocket	= ENG_TPCS::en_sending;

	do	{

		/* 처음 보내고자 하는 패킷 크기보다 크면 에러 임 (통신 연결 에러가 발생하는 경우, 간혹 나옴) */
		if (i32FirstSize < i32SendSize)
		{
			m_enSocket	= ENG_TPCS::en_send_failed;
			return FALSE;
		}

		/* 현재 주어진 크기만큼 데이터 송신 */
		i32SendBytes = send(m_sdClient, pPktSend, i32SendSize, 0);
		if (SOCKET_ERROR == i32SendBytes)
		{
			/* 에러 값 저장 */
			m_i32SockError = WSAGetLastError();
			/* 전송 지연 모드가 아니다라면... 통신을 끊어야 된다. */
			if (m_i32SockError != WSAEWOULDBLOCK && m_i32SockError != WSA_IO_PENDING)
			{
				m_i32SockError = WSAGetLastError();
				bSucc	= FALSE;
				return FALSE;
			}

			/* ------------------------------------------------ */
			/* 일단 에러가 발생했으니, 무조건 루프를 빠져나간다 */
			/* 1차적으로 에러 현상 즉, 데이터 전송을 못했기때문 */
			/* 다음 번에 재전송 작업을 위해 무조건 루프 나가기  */
			/* ------------------------------------------------ */
			bRetrySend	= TRUE;	/* !!! Important !!! */
			break;
		}
		else
		{
			/* 현재까지 전송된 데이터 크기 저장 */
			u32TotalSize+= i32SendBytes;
			/* 다음에 전송될 데이터 크기 재계산 */
			i32SendSize	-= i32SendBytes;
			/* 다음에 전송할 위치로 이동 */
			pPktSend	+= i32SendBytes;
		}

		/* 무한 루프에 들어 갔는지 여부 확인 */
#ifdef _DEBUG
		if (((UINT32)GetTickCount() - u32TickTime) > UINT32_MAX)
#else
		/* 기본적으로 송신 완료하는데 1초 이상 걸리지 않음 */
		/* 송신 쪽의 TCP Port의 버퍼가 꽉 차있지 않은 상태 */
		if (((UINT32)GetTickCount() - u32TickTime) > TCP_SEND_TOTAL_TIMEOUT)
#endif
		{
			bSucc	= FALSE;
			break;
		}

	}	while (i32SendSize > 0);

	/* ----------------------------------------------- */
	/* 패킷 데이터를 전송 성공 했으면 전송한 시간 갱신 */
	/* ----------------------------------------------- */
	if (bRetrySend || (bSucc && u32TotalSize == size))
	{
		UpdateWorkTime();
		m_enSocket = ENG_TPCS::en_sended;
	}
	// 송신 실패인 경우
	else
	{
		bSucc	= FALSE;
		m_enSocket = ENG_TPCS::en_send_failed;
	}
	/* ----------------------- */
	/* 패킷을 전송한 크기 반환 */
	/* ----------------------- */
	size = u32TotalSize;

	/* 소켓 송신 성공이면 */
	if (bSucc)	m_i32SockError	= 0;

	return bSucc;
}

/*
 desc : 패킷을 전송할 때, 재전송 횟수 지정
 parm : packet	- [in]  송신 패킷이 저장된 버퍼
		size	- [in]  'packet' 버퍼에 저장된 패킷 크기
		retry	- [in]  재전송 시도 횟수 (무조건 0 보다 커야 됨)
		wait	- [in]  재전송 간격 시간 (단위: 밀리초)
 retn : TRUE or FALSE (통신 이상이 발생한 경우)
*/
BOOL CTcpThread::ReSended(PUINT8 packet, UINT32 size, UINT32 retry, UINT32 wait)
{
	PUINT8 pPacket	= packet;
	UINT32 u32Size	= size, u32Total = 0, u32Retry = retry;

	do {

		// 전송 실패한 경우는 통신 에러일 가능성이 큼!!!
		if (!Sended(pPacket, u32Size))
		{
			return FALSE;
		}
		/* ------------------------------------- */
		/* 일부 혹은 전체 패킷이 전송했는지 확인 */
		/* ------------------------------------- */
		/* 현재까지 전송한 데이터 크기 저장 */
		u32Total += u32Size;
		/* 모든 데이터가 전송된 경우인지 조사 */
		if (u32Total == size)
		{
			m_i32LastSendPktSize	= u32Total;
			return TRUE;
		}

		/* ------------------------------------- */
		/* 재-전송을 위한 버퍼의 시작 위치 설정  */
		/* ------------------------------------- */
		pPacket += u32Size;
		/* ------------------------------------- */
		/* 일부 데이터만 전송된 경우 나머지 패킷 */
		/* 데이터의 재-전송을 위해서 크기 재계산 */
		/* ------------------------------------- */
		u32Size = size - u32Total;

		/* 일정 시간 지연 (단위: 밀리초) */
		::Sleep(wait);

		/* 모두 반복 작업을 수행 했으면 빠져 나간다 */
		if (u32Retry < 1)
		{
			break;
		}
		u32Retry--;

	} while (1);

	return FALSE;
}

/*
 desc : 패킷을 전송할 때, 최대 전송하는데 대기하는 시간 지정
 parm : packet	- [in]  송신 패킷이 저장된 버퍼
		size	- [in]  'packet' 버퍼에 저장된 패킷 크기
		time_out- [in]  주어진 시간 동안 전송 완료 될 때까지 기다림
						(이 시간 동안 전송이 완료되지 않으면 실패 처리)
		wait	- [in]  재전송 간격 시간 (단위: 밀리초)
 retn : TRUE or FALSE (통신 이상이 발생한 경우)
*/
BOOL CTcpThread::ReSended(PUINT8 packet, UINT32 size, UINT64 time_out, UINT32 wait)
{
	PUINT8 pPacket	= packet;
	UINT32 u32Size	= size, u32Total = 0;
	UINT64 u64Tick	= GetTickCount64();

	do {

		/* 전송 실패한 경우는 통신 에러일 가능성이 큼!!! */
		if (!Sended(pPacket, u32Size))
		{
			return FALSE;
		}
		/* ------------------------------------- */
		/* 일부 혹은 전체 패킷이 전송했는지 확인 */
		/* ------------------------------------- */
		// 현재까지 전송한 데이터 크기 저장
		u32Total += u32Size;
		// 모든 데이터가 전송된 경우인지 조사
		if (u32Total == size)
		{
			return TRUE;
		}
		/* ------------------------------------- */
		/* 재-전송을 위한 버퍼의 시작 위치 설정  */
		/* ------------------------------------- */
		pPacket += u32Size;
		/* ------------------------------------- */
		/* 일부 데이터만 전송된 경우 나머지 패킷 */
		/* 데이터의 재-전송을 위해서 크기 재계산 */
		/* ------------------------------------- */
		u32Size = size - u32Total;

		/* 일정 시간 지연 (단위: 밀리초) */
		::Sleep(wait);

		/* 모두 반복 작업을 수행 했으면 빠져 나간다 */
		if (GetTickCount64() > (u64Tick + time_out))	break;

	} while (1);

	return FALSE;
}

/*
 desc : Remote Client로부터 데이터 수신 시그널이 발생한 경우
 parm : sd	- [in]  Client socket descriptor
 retn : TRUE or FALSE
*/
BOOL CTcpThread::Received(SOCKET sd)
{
	BOOL bSucc	= TRUE;
	INT32 i32ReadBytes, i32RecvSize;
	UINT32 u32SpareBytes, u32Repeat = 1;	/* 0 값이 아니라, 반드시 1 값임 */
	UINT32 u32TickTime = (UINT32)GetTickCount();	/* 수신을 무한대로 할 경우, 루프 빠져나오기 위함 */

	/* 송신 전에 무조건 송신 모드로 설정 */
	m_enSocket	= ENG_TPCS::en_recving;

	do	{

		/* ---------------------------------------------------------- */
		/* 현재 수신될 데이터를 저장할 수 있는 여유 공간의 크기 얻기   */
		/* 더 이상 수신 받을 버퍼가 없는 경우는 수신 처리하지 않는다. */
		/* ---------------------------------------------------------- */
		u32SpareBytes	= m_pPktRingBuff->GetWriteSize();
		if (u32SpareBytes <= 0)	break;	// 더 이상 수신받을 버퍼가 없으므로, 일단 버퍼를 비우는 작업을 수행하도록 처리 한다

		/* ---------------------------------------------------------------- */
		/* 남아 있는 여유 버퍼 공간의 크기만큼 데이터 수신 작업을 수행 한다 */
		/* ---------------------------------------------------------------- */
		/* 저장할 수 있는 버퍼 크기 만큼 읽되, 임시 수신 버퍼 크기에 따라, 수신 데이터 크기 결정 */
		if (u32SpareBytes > m_u32RecvBuff)	i32RecvSize	= (INT32)m_u32RecvBuff;
		else								i32RecvSize	= (INT32)u32SpareBytes;
		i32ReadBytes = recv(sd, (CHAR *)m_pPktRecvBuff, i32RecvSize, 0);
 		// 소켓 에러가 발생한 경우
 		if (i32ReadBytes == SOCKET_ERROR)
 		{
 			m_i32SockError = WSAGetLastError();
 			/* IO Pending 에러가 아니고, 소켓 에러이면 통신 연결을 해제 한다. */
 			if (m_i32SockError != WSA_IO_PENDING && m_i32SockError != WSAEWOULDBLOCK)
 			{
				LOG_ERROR(ENG_EDIC::en_engine, L"TCP/IP was disconnected because failed to recv the packet");
				CloseSocket(m_sdClient);
				return FALSE;
 			}
			
			/* TCP 통신 시간 갱신 */
			UpdateWorkTime();

			/* ----------------------------------------------------------------------- */
			/* 일정 횟수만큼 반복하여 recv 함수를 호출하여 수신된 패킷을 읽도록 하는지 */
			/* 아래와 같은 구문 즉, 반복 횟수만큼 읽도록 했을 경우, 수신속도 저하 발생 */
			/* ----------------------------------------------------------------------- */
			if (m_u32RecvRepeat > u32Repeat)
			{
				Sleep(m_u32RecvSleep);
				u32Repeat++;
			}
			else
			{
				/* 수신되었다고 모드 값 변경 */
				m_enSocket	= ENG_TPCS::en_recved;
				/* 이도저도 아니면, 다음 번에 다시 읽도록 반환 */
				return TRUE;
			}
 		}

		/* 일단 수신된 데이터를 Ring 버퍼 공간에 저장 */
		m_pPktRingBuff->WriteBinary(m_pPktRecvBuff, i32ReadBytes);

		/* 요청한 데이터 크기보다 수신된 데이터 크기가 작으면 루틴 빠져 나가기 */
		if (i32ReadBytes < i32RecvSize)	break;

		/* 무한 루프에 들어 갔는지 여부 확인 */
		if ((UINT32)GetTickCount() - u32TickTime > TCP_RECV_TOTAL_TIMEOUT)
		{
			bSucc	= FALSE;
			break;
		}

	} while (1);

	/* 소켓 수신 성공이면 */
	m_i32SockError	= 0;

	// TCP 통신 시간 갱신
	UpdateWorkTime();

	/* 데이터 분석 실패할 경우, 기존 통신 포트의 수신 버퍼 모두 해제 */
	if (!bSucc)
	{
		ReceiveAllData(sd);
		m_enSocket	= ENG_TPCS::en_recv_failed;	/* 수신되었다고 모드 값 변경 */
	}
	else
	{
		m_enSocket	= ENG_TPCS::en_recved;	/* 수신되었다고 모드 값 변경 */
		bSucc		= RecvData();
		if (!bSucc)	ReceiveAllData(sd);
	}

	return bSucc;
}

/*
 desc : 현재 통신 포트의 수신 버퍼에 남아 있는 모든 데이터를 읽어서 버린다.
		더 이상 읽어들일 데이터가 없을 때까지 ...
 parm : sd	- [in]  모든 데이터를 읽어들이고자 하는 소켓 핸들
 retn : 총 읽어서 버린 수신 바이트 크기 (-1 값이면 전혀 읽어들인 데이터가 없다?)
*/
INT32 CTcpThread::ReceiveAllData(SOCKET sd)
{
	CHAR szBuff[1024+1]	= {NULL};
	INT32 i32RecvBytes	= 0, i32RecvTotal = 0;
	UINT32 u32TickTime	= (UINT32)GetTickCount();

	if (INVALID_SOCKET != sd)
	{
		do	{

			/* 일단 무조건 1024 Bytes씩 읽어들이기 */
			i32RecvBytes = recv(sd, (CHAR *)szBuff, 1024, 0);
			i32RecvTotal += i32RecvBytes;

			/* 무한 루프에 들어 갔는지 여부 확인 - 최대 2 초 동안만 모두 읽기 수행 */
			if (((UINT32)GetTickCount() - u32TickTime) > TCP_RECV_TOTAL_TIMEOUT)	break;

		}	while (i32RecvBytes == 1024);
	}

	/* TCP/IP 통신 작업 시간 갱신 */
	UpdateWorkTime();

	/* 수신 버퍼 크기 초기화 */
	m_pPktRingBuff->ResetBuff();

	/* 이전에 패킷 송신 실패한 시간보다 주기가 너무 짧으면 부모에게 알리지 않음 */
	LOG_WARN(ENG_EDIC::en_engine, L"Read all the data in the buffer and discarded it");

	return i32RecvTotal;
}

/*
 desc : 대용량 수신 버퍼에 더 이상 수신된 패킷이 없는지 확인
 parm : None
 retn : TRUE - 수신된 버퍼에 남아있는 데이터가 있다, FALSE - 수신된 데이터가 없다
*/
BOOL CTcpThread::IsEmptyPktBuff()
{
	return (m_pPktRingBuff && m_pPktRingBuff->GetReadSize() < 1) ? TRUE : FALSE;
}

/*
 desc : 스레드에서 일정 시간동안 통신 작업을 수행하지 않았는지 여부 ...
 parm : time_gap	- [in]  이 주어진 시간 간격 값. 즉, 이 시간(단위: 밀리초)동안 통신이 이루어지지 않았는지 확인하기 위함
 retn : TRUE - 해당 시간동안 통신 관련 작업이 수행되지 않았음
		FALSE - 해당 주어진 내에 통신 관련 작업이 수행되었음
*/
BOOL CTcpThread::IsTcpWorkIdle(UINT32 time_gap)
{
	return (GetTickCount64() - m_u64WorkTime) > time_gap ? TRUE : FALSE;
}

/*
 desc : 재접속 작업을 수행해도 되는지 여부 (즉, 재접속 수행 시간이 되었는지 여부)
 parm : time_gap	- [in] 이 주어진 시간 가격 값 (단위: 밀리초)
 retn : TRUE - 재접속 작업을 수행하라, FALSE - 아직 재접속 작업 수행하지 말아라
*/
BOOL CTcpThread::IsTcpWorkConnect(UINT32 time_gap)
{
	return (GetTickCount64() - m_u64ConnectTime) > time_gap ? TRUE : FALSE;
}

/*
 desc : Alive Check가 일정 횟수 이상 실패한 경우, 강제로 종료 작업 수행
 parm : None
 retn : None
*/
VOID CTcpThread::SetForceClose()
{
	CloseSocket(m_sdClient);
	m_sdClient	= INVALID_SOCKET;
}

/*
 desc : Alive Check 혹은 Sync Time정보 전송. Server에게 통신이 살아 있는지 Check 패킷을 전송
 parm : None
 retn : TRUE or FALSE
*/
BOOL CTcpThread::SendAliveTime()
{
	UINT8 u8Buff[32]= {NULL};
	UINT32 u32Size	= 0, u32Length = htonl(2+2+4+2+2);

	/* Head */
	u8Buff[u32Size]	= 0xaa;					u32Size	+= 1;
	u8Buff[u32Size]	= 0x11;					u32Size	+= 1;
	/* Command */
	u8Buff[u32Size]	= 0xff;					u32Size	+= 1;
	u8Buff[u32Size]	= 0xf0;					u32Size	+= 1;
	/* Length */
	memcpy(u8Buff+u32Size, &u32Length, 4);	u32Size	+= 4;
	/* Payload - Skip */
	/* CRC */
	u8Buff[u32Size]	= 0x00;					u32Size	+= 1;
	u8Buff[u32Size]	= 0x00;					u32Size	+= 1;
	/* Tail */
	u8Buff[u32Size]	= 0xff;					u32Size	+= 1;
	u8Buff[u32Size]	= 0xaa;					u32Size	+= 1;

	/* Live Check 관련 패킷을 주기적으로 전송 */
	if (!ReSended(u8Buff, u32Size, UINT32(3), UINT32(50)))
	{
		TCHAR tzMesg[128]	= {NULL};
		CUniToChar csCnv;
		swprintf_s(tzMesg, 128, L"Failed to send the alive_ack to the (%s)", m_tzTCPInfo);
		LOG_WARN(ENG_EDIC::en_engine, tzMesg);

		return FALSE;
	}

	return TRUE;
}
