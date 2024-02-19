
/* desc : MC2 소켓 기본 클래스 */

#include "pch.h"
#include "MC2Thread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

#define	MAX_PKT_SEND_BUFF	128		/* 임시 송신 패킷을 저장할 수 있는 버퍼 개수 */

/*
 desc : 생성자
 parm : shmem		- [in]  MC2 Service 관련 공유 메모리
		source_ip	- [in]  Local IPv4
		target_ip	- [in]  Remote IPv4
		mc2_port	- [in]  TCP/IP Port (MC2에 데이터를 송신하기 위한 포트)
		cmps_port	- [in]  MC2로부터 CMPS에 응답받기 위한 통신 포트
		port_size	- [in]  TCP/IP Port Buffer Size
		recv_size	- [in]  Send or Recv의 패킷 중 가장 큰 패킷의 크기 (단위: Bytes)
		ring_size	- [in]	Max Recv Packet Size
		sock_time	- [in]  기본 xx 밀리초 동안 소켓 이벤트 감지를 위한 대기
 retn : None
*/
CMC2Thread::CMC2Thread(LPG_MDSM shmem,
					   UINT16 mc2_port,
					   UINT16 cmps_port,
					   UINT32 source_ip,
					   UINT32 target_ip,
					   UINT32 port_size,
					   UINT32 recv_size,	// Packet 버퍼의 최대 크기 (단위: Bytes)
					   UINT32 ring_size,	// Ring Buffer 크기 (단위: Bytes)
					   UINT32 sock_time)	// 소켓 이벤트를 감지하는데 최소 대기 시간 (단위: 밀리초)
{
	IN_ADDR stInAddr	= {NULL};

	m_bConnected		= FALSE;

	m_enSysID			= m_enSysID;
	m_sdClient			= INVALID_SOCKET;

	m_u16ThreadID		= 0x0000;	/* Fixed */
	m_u16MC2Port		= mc2_port;
	m_u16CMPSPort		= cmps_port;
	m_u32PortBuff		= port_size;
	m_u32RecvBuff		= recv_size;
	m_u32RingBuff		= ring_size;
	/* 최소 기다리는 시간이 0 값이면 기본 값으로 10을 넣어주면 됩니다.*/
	m_u32SockTime		= sock_time < 10 ? 10 : sock_time;	// 기본 10 밀리초 마다 소켓 이벤트 감지를 위한 대기
	/* 공유 메모리 연결 */
	m_pstShMemMC2		= shmem;
	/* 패킷 송신 실패 시간 초기화 */
	m_u64TCPFailTime	= GetTickCount64();

	/* ----------------------------------------------------------------------------- */
	/* 보통은 0 값으로 초기화 되나, 특정 장비로부터 수신 이벤트가 발생한 경우, 데이터*/
	/* 수신 작업 즉, Socket의 recv 함수 호출을 일정 기간 및 횟수를 반복적으로 수행하 */
	/* 여 더 이상 수신된 데이터가 없는지 확인한 후, 읽기 작업을 끝내야만 하는 경우가 */
	/* 있으며, 이럴 경우, 아래 두 개의 변수에 읽기 반복 횟수와 반복 읽기 이전에 대기 */
	/* 하는 시간 (단위: msec) 값을 설정해 줘야 한다. 이 부분은 꼭 필요할 때만 사용함 */
	/* ----------------------------------------------------------------------------- */
	m_u32RecvRepeat		= 0;	// 만약 이 값이 3인 경우, Socket의 recv 함수를 3번 반복 호출
	m_u32RecvSleep		= 0;	// 만약 이 값이 50인 경우, Socket의 recv 함수 호출 이전에 50 msec 대기함
	m_i32SockError		= 0;

	ResetError();

	/* ---------------------------------- */
	/* IP 주소 변환: DWORD -> String 변환 */
	/* ---------------------------------- */
	wmemset(m_tzSourceIPv4, 0x00, IPv4_LENGTH);
	memcpy(&stInAddr.s_addr, &source_ip, sizeof(UINT32));
	InetNtopW(AF_INET, (CONST VOID*)&stInAddr, m_tzSourceIPv4, IPv4_LENGTH);

	wmemset(m_tzTargetIPv4, 0x00, IPv4_LENGTH);
	memcpy(&stInAddr.s_addr, &target_ip, sizeof(UINT32));
	InetNtopW(AF_INET, (CONST VOID*)&stInAddr, m_tzTargetIPv4, IPv4_LENGTH);
	/* 가장 최근에 송신한 시간 저장 */
	m_u64SendTime		= GetTickCount64();
	// 스레드 내 루프 속조 제어를 위한 정보 초기화
	m_bIsInited			= InitData();
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CMC2Thread::~CMC2Thread()
{
	// Release Client Socket
	if (INVALID_SOCKET != m_sdClient)		CloseSocket(m_sdClient);
	// Socket Event 해제
	if (WSA_INVALID_EVENT != m_wsaClient)	WSACloseEvent(m_wsaClient);

	// 수신 버퍼 메모리 해제
	if (m_pPktRecvBuff)	delete [] m_pPktRecvBuff;
	if (m_pPktRingBuff)	delete m_pPktRingBuff;
	/* 임시 송신 버퍼 메모리 해제 */
	if (m_pstPktSend)	delete [] m_pstPktSend;
}

/*
 desc : 통신 스레드 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMC2Thread::InitData()
{
	// 멤버 초기화
	m_sdClient		= INVALID_SOCKET;
	m_wsaClient		= WSA_INVALID_EVENT;
	m_enSocket		= ENG_TPCS::en_closed;
	// initialize the member variable
	m_u64WorkTime	= GetTickCount64();

	memset(&m_stWorkTime, 0x00, sizeof(SYSTEMTIME));
	memset(&m_stLinkTime, 0x00, sizeof(SYSTEMTIME));
	/* ---------------------------------------- */
	/* 수신 버퍼 초기화 (CRingBuff 클래스 적용) */
	/* ---------------------------------------- */
	return InitMemory();
}

/*
 desc : 송/수신 패킷 저장 관련 고정 메모리 할당 및 초기화
 parm : None
 retn : TRUE - 생성 성공, FALSE - 실패
*/
BOOL CMC2Thread::InitMemory()
{
	m_pPktRingBuff	= new CRingBuff;
	if (!m_pPktRingBuff)	return FALSE;
	m_pPktRingBuff->Create(m_u32RingBuff+1);
	m_pPktRingBuff->ResetBuff();

	/* 임시 송신 데이터 저장 (최대 32개) */
	m_pstPktSend	= new STG_MPSD [MAX_PKT_SEND_BUFF];
	memset(m_pstPktSend, 0x00, sizeof(STG_MPSD) * MAX_PKT_SEND_BUFF);
	/* ------------------------------------------------ */
	/* 한번 수신 받을 때, 저장할 수 있는 패킷 버퍼 할당 */
	/* ------------------------------------------------ */
	m_pPktRecvBuff	= new UINT8 [m_u32RecvBuff+1];
	ASSERT(m_u32RecvBuff);
	memset(m_pPktRecvBuff, 0x00, m_u32RecvBuff+1);
	// Client Socket Event 생성
	m_wsaClient		= WSACreateEvent();
	if (WSA_INVALID_EVENT == m_wsaClient)
	{
		AfxMessageBox(L"Failed to create the socket_event (MC2 Client)", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : When thread is exceuted, the first is called only once
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMC2Thread::StartWork()
{
	return m_bIsInited;
}

/*
 desc : Called by only once at the end before the thread is terminated
 parm : None
 retn : None
*/
VOID CMC2Thread::EndWork()
{
}

/*
 desc : 소켓 옵션 설정하기 - 송신/수신 버퍼 크기 설정
 parm :	flag	- [in]  0x01:Send Buffer, 0x10:Recv Buffer, 0x11:Send/Recv Buffer
		sd		- [in]  소켓 옵션을 설정한 소켓 기술자
		size	- [in]  Port Buffer Size
 retn : TRUE or FALSE
*/
BOOL CMC2Thread::SetSockOptBuffSize(UINT8 flag, SOCKET sd)
{
	if (INVALID_SOCKET == sd)	return FALSE;
	INT32 i32Ret = 0, i32Size = sizeof(int);

	// 송신 및 수신 버퍼 크기 설정
	if ((0 == i32Ret) && (0x01 == (0x01 & flag)))
		i32Ret = setsockopt(sd, SOL_SOCKET, SO_SNDBUF, (CHAR *)&m_u32PortBuff, i32Size);
	if ((0 == i32Ret) && (0x10 == (0x10 & flag)))
		i32Ret = setsockopt(sd, SOL_SOCKET, SO_RCVBUF, (CHAR *)&m_u32PortBuff, i32Size);

	if (i32Ret == SOCKET_ERROR)
	{
		LOG_ERROR(m_enSysID, L"Socket option : Failed to set the send or recv buffer size");
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
BOOL CMC2Thread::SetSockOptTimeout(UINT8 flag, SOCKET sd, INT32 time)
{
	if (INVALID_SOCKET == sd)	return FALSE;
	INT32 i32Ret = 0, i32Size = sizeof(int);

	if ((0 == i32Ret) && (0x01 == (0x01 & flag)))
		i32Ret = setsockopt(sd, SOL_SOCKET, SO_SNDTIMEO, (CHAR *)&time, i32Size);
	if ((0 == i32Ret) && (0x10 == (0x10 & flag)))
		i32Ret = setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (CHAR *)&time, i32Size);

	if (i32Ret == SOCKET_ERROR)
	{
		LOG_ERROR(m_enSysID, L"Socket option : Failed to set the socket's time-out");
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
		sd		- 소켓 옵션을 설정한 소켓 기술자
 retn : TRUE or FALSE
*/
BOOL CMC2Thread::SetSockOptSpeed(INT32 optval, SOCKET sd)
{
	if (SOCKET_ERROR == setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (CHAR *)&optval, sizeof(optval)))
	{
		LOG_ERROR(m_enSysID, L"Socket option : Failed to set the speed for socket");
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
BOOL CMC2Thread::SetSockOptAlive(INT32 optval, SOCKET sd)
{
	if (SOCKET_ERROR == setsockopt(sd, SOL_SOCKET, SO_KEEPALIVE, (CHAR *)&optval, sizeof(optval)))
	{
		LOG_ERROR(m_enSysID, L"Socket option : Failed to set the alive time for socket");
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
BOOL CMC2Thread::SetSockOptRoute(INT32 optval, SOCKET sd)
{
	if (SOCKET_ERROR == setsockopt(sd, SOL_SOCKET, SO_DONTROUTE, (CHAR *)&optval, sizeof(optval)))
	{
		LOG_ERROR(m_enSysID, L"Socket option : Failed to set the route for socket");
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
VOID CMC2Thread::CloseSocket(SOCKET sd, UINT32 time_out)
{
	INT32 i32Code	= 0;
	LINGER stLinger	= {NULL};
	fd_set readfds, errorfds;
	timeval timeout;

	// Shutdown 함수 호출하기 전에 반드시 아래 플래그부터 설정해 줌
	m_enSocket	= ENG_TPCS::en_closed;
	// 현재 Client 소켓이 살아 있는지 여부
	if (INVALID_SOCKET != sd)
	{
		TCHAR tzMesg[256]	= {NULL};
		swprintf_s(tzMesg, 256,
				   L"The remote system has been disconnected. (src:%s tgt:%s / err_cd:0x%x)",
				   m_tzSourceIPv4, m_tzTargetIPv4, m_i32SockError);
		if (!IsRunDemo())	LOG_ERROR(m_enSysID, tzMesg);

		// Initialize (Release) the send and recv socket buffer
		i32Code	= shutdown(sd, SD_BOTH);

		// Wait for socket to fail (ie. closed by other end)
		if (i32Code != SOCKET_ERROR)
		{
			FD_ZERO(&readfds);
			FD_ZERO(&errorfds);
			FD_SET(sd, &readfds);
			FD_SET(sd, &errorfds);

			timeout.tv_sec	= ULONG(time_out);	// MAX_LINGER_SECONDS;
			timeout.tv_usec	= 0;
			::select(1, &readfds, NULL, &errorfds, &timeout);
		}

		// 소켓에 해당되는 포트 초기화
		stLinger.l_onoff	= 1;				// linger on...
		stLinger.l_linger	= UINT16(time_out);	// timeout in seconds ... (0 -> hard close)
		setsockopt(sd, SOL_SOCKET, SO_LINGER, (char FAR*)&stLinger, sizeof(LINGER));
		// 소켓 해제 및 초기화
		closesocket(sd);
	}

	// Update the communication time for TCP/IP
	UpdateWorkTime();

	/* --------------------- */
	/* 연결 해제된 시간 저장 */
	/* --------------------- */
	GetLocalTime(&m_stLinkTime);
}

/*
 desc : 가장 최근에 통신한 시간 저장 및 관리
 parm : None
 retn : None
*/
VOID CMC2Thread::UpdateWorkTime()
{
	m_u64WorkTime = GetTickCount64();
	GetLocalTime(&m_stWorkTime);
}

/*
 desc : Remote Server로부터 연결 해제 시그널이 발생한 경우
 parm : sd	- [in]  Client socket descriptor
 retn : None
*/
VOID CMC2Thread::Closed(SOCKET sd)
{
	/* --------------------------------------------------------- */
	/* 반드시 가상 함수 호출 후, 소켓 핸들 해제 함수 호출해야 함 */
	/* --------------------------------------------------------- */

	// 소켓 연결 해제 작업 수행
	CloseSocket(sd);
}

/*
 desc : 실제 패킷 데이터 송신부
 parm : packet	- [in]  송신 패킷이 저장된 버퍼
		size	- [out] 입력: 'packet' 버퍼에 저장된 패킷 크기
						반환: 전송된 데이터의 크기
 retn : TRUE or FALSE (통신 이상이 발생한 경우)
*/
BOOL CMC2Thread::Sended(PUINT8 packet, UINT32 &size)
{
	BOOL bSucc			= TRUE;
	CHAR *pPktSend		= (CHAR*)packet;
	INT32 i32SendBytes	= 0, i32SendSize = INT32(size);
	UINT32 u32TotalSize	= 0, u32TickTime = (UINT32)GetTickCount();

	/* 송신 전에 무조건 송신 모드로 설정 */
	m_enSocket	= ENG_TPCS::en_sending;

	do	{

		i32SendBytes = sendto(m_sdClient,
							  pPktSend,
							  i32SendSize,
							  0,
							  (struct sockaddr*)&m_stSrvAddr,
							  m_u32AddrSize);
		if (SOCKET_ERROR == i32SendBytes)
		{
			// 에러 값 저장
			m_i32SockError = WSAGetLastError();
			// 전송 지연 모드가 아니다라면... 통신을 끊어야 된다.
			if (m_i32SockError != WSAEWOULDBLOCK &&
				m_i32SockError != WSA_IO_PENDING)
			{
				CloseSocket(m_sdClient);
				return FALSE;
			}
			/* ------------------------------------------------ */
			/* 일단 에러가 발생했으니, 무조건 루프를 빠져나간다 */
			/* 1차적으로 에러 현상 즉, 데이터 전송을 못했기때문 */
			/* 다음 번에 재전송 작업을 위해 무조건 루프 나가기  */
			/* ------------------------------------------------ */
			break;
		}
		else
		{
			// 현재까지 전송된 데이터 크기 저장
			u32TotalSize+= i32SendBytes;
			// 다음에 전송될 데이터 크기 재계산
			i32SendSize	-= i32SendBytes;
			// 다음에 전송할 위치로 이동
			pPktSend	+= i32SendBytes;
		}

		/* 무한 루프에 들어 갔는지 여부 확인 */
		if (((UINT32)GetTickCount() - u32TickTime) > TCP_SEND_TOTAL_TIMEOUT)
		{
			bSucc	= FALSE;
			break;
		}

	}	while (i32SendSize > 0);

	/* ----------------------------------------------- */
	/* 패킷 데이터를 전송 성공 했으면 전송한 시간 갱신 */
	/* ----------------------------------------------- */
	if (u32TotalSize == size)
	{
		UpdateWorkTime();
		m_enSocket = ENG_TPCS::en_sended;
	}
	// 송신 실패인 경우
	else	bSucc = FALSE;

	/* 송신 시간 저장 */
	if (bSucc)	m_u64SendTime	= GetTickCount64();

	/* ----------------------- */
	/* 패킷을 전송한 크기 반환 */
	/* ----------------------- */
	size = u32TotalSize;

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
BOOL CMC2Thread::ReSended(PUINT8 packet, UINT32 size, UINT32 retry, UINT32 wait)
{
	PUINT8 pPacket	= packet;
	UINT32 u32Size	= size, u32Total = 0, u32Retry = retry;

	do {

		// 전송 실패한 경우는 통신 에러일 가능성이 큼!!!
		if (!Sended(pPacket, u32Size))	return FALSE;

		/* ------------------------------------- */
		/* 일부 혹은 전체 패킷이 전송했는지 확인 */
		/* ------------------------------------- */
		// 현재까지 전송한 데이터 크기 저장
		u32Total += u32Size;
		// 모든 데이터가 전송된 경우인지 조사
		if (u32Total == size)	return TRUE;

		/* ------------------------------------- */
		/* 재-전송을 위한 버퍼의 시작 위치 설정  */
		/* ------------------------------------- */
		pPacket += u32Size;

		/* ------------------------------------- */
		/* 일부 데이터만 전송된 경우 나머지 패킷 */
		/* 데이터의 재-전송을 위해서 크기 재계산 */
		/* ------------------------------------- */
		u32Size = size - u32Total;

		// 일정 시간 지연 (단위: 밀리초)
		::Sleep(wait);

		/* 모두 반복 작업을 수행 했으면 빠져 나간다 */
		if (u32Retry < 1)	break;
		u32Retry--;

	} while (1);

	return FALSE;
}

/*
 desc : Remote Client로부터 데이터 수신 시그널이 발생한 경우
 parm : sd	- [in]  Client socket descriptor
 retn : TRUE or FALSE
*/
BOOL CMC2Thread::Received(SOCKET sd)
{
	BOOL bSucc	= TRUE;
	INT32 i32ReadBytes, i32RecvSize;
	INT32 i32AddrSize	= sizeof(SOCKADDR_IN);
	UINT32 u32SpareBytes;
	UINT32 u32TickTime	= (UINT32)GetTickCount();
	SOCKADDR_IN stSrvAddr	= {NULL};
	CUniToChar csCnv;

	do	{

		/* ---------------------------------------------------------- */
		/* 현재 수신될 데이터를 저장할 수 있는 여유 공간의 크기 얻기   */
		/* 더 이상 수신 받을 버퍼가 없는 경우는 수신 처리하지 않는다. */
		/* ---------------------------------------------------------- */
		u32SpareBytes	= m_pPktRingBuff->GetWriteSize();
		if (u32SpareBytes <= 0)
			break;	// 더 이상 수신받을 버퍼가 없으므로, 일단 버퍼를 비우는 작업을 수행하도록 처리 한다

		/* ---------------------------------------------------------------- */
		/* 남아 있는 여유 버퍼 공간의 크기만큼 데이터 수신 작업을 수행 한다 */
		/* ---------------------------------------------------------------- */
		// 저장할 수 있는 버퍼 크기 만큼 읽되, 임시 수신 버퍼 크기에 따라, 수신 데이터 크기 결정
		if (u32SpareBytes > m_u32RecvBuff)	i32RecvSize	= (INT32)m_u32RecvBuff;
		else								i32RecvSize	= (INT32)u32SpareBytes;
		i32ReadBytes = recvfrom(sd,
								(CHAR *)m_pPktRecvBuff,
								i32RecvSize,
								0,
								(struct sockaddr *)&stSrvAddr, &i32AddrSize);
 		// 소켓 에러가 발생한 경우
 		if (i32ReadBytes == SOCKET_ERROR)
 		{
 			m_i32SockError = WSAGetLastError();
 			// IO Pending 에러가 아니고, 소켓 에러이면 통신 연결을 해제 한다.
 			if (m_i32SockError != WSA_IO_PENDING && m_i32SockError != WSAEWOULDBLOCK)
 			{
				CloseSocket(m_sdClient);
				return FALSE;
 			}
			// TCP 통신 시간 갱신
			UpdateWorkTime();

			// 수신되었다고 모드 값 변경
			m_enSocket	= ENG_TPCS::en_recved;
			// 이도저도 아니면, 다음 번에 다시 읽도록 반환 한다
			return TRUE;
 		}

		/* ----------------------------------------------------------- */
		/* 현재 서버로부터 수신된 데이터인지 or 아닌지를 판단하기 위함 */
		/* ----------------------------------------------------------- */
		if (0 == memcmp(&stSrvAddr, &m_stSrvAddr, sizeof(stSrvAddr)))
		{
			/* 일단 수신된 데이터를 Ring 버퍼 공간에 저장 */
			m_pPktRingBuff->WriteBinary(m_pPktRecvBuff, i32ReadBytes);
		}
		else
		{
			TCHAR tzMesg[128]	= {NULL}, tzIPv4[IPv4_LENGTH] = {NULL};
			InetNtopW(AF_INET, (CONST VOID*)&stSrvAddr, tzIPv4, IPv4_LENGTH);
			swprintf_s(tzMesg, 128,
					   L"Packet data received form other MC2 : %s", tzIPv4);
			LOG_WARN(m_enSysID, tzMesg);			
		}
		/* ------------------------------------------ */
		/* 일단 수신된 데이터를 Ring 버퍼 공간에 저장 */
		/* ------------------------------------------ */
		m_pPktRingBuff->WriteBinary(m_pPktRecvBuff, i32ReadBytes);
		// TCP 통신 시간 갱신
		UpdateWorkTime();

		/* 요청한 데이터 크기보다 수신된 데이터 크기가 작으면 루틴 빠져 나가기 */
		u32SpareBytes	= m_pPktRingBuff->GetWriteSize();
		if (i32ReadBytes < i32RecvSize || u32SpareBytes < 1)	break;

		/* 무한 루프에 들어 갔는지 여부 확인 */
		if (((UINT32)GetTickCount() - u32TickTime) > TCP_RECV_TOTAL_TIMEOUT)
		{
			bSucc	= FALSE;
			break;
		}

	} while (1);
 	
	/* ------------------------------------------------------------- */
	/* 데이터 분석 실패할 경우, 기존 통신 포트의 수신 버퍼 모두 해제 */
	/* ------------------------------------------------------------- */
	return (bSucc && RecvData());
}

/*
 desc : 현재 통신 포트의 수신 버퍼에 남아 있는 모든 데이터를 읽어서 버린다.
		더 이상 읽어들일 데이터가 없을 때까지 ...
 parm : sd	- [in]  모든 데이터를 읽어들이고자 하는 소켓 핸들
 retn : 총 읽어서 버린 수신 바이트 크기 (-1 값이면 전혀 읽어들인 데이터가 없다?)
*/
INT32 CMC2Thread::ReceiveAllData(SOCKET sd)
{
	CHAR szBuff[1024+1]		= {NULL};
	INT32 i32RecvBytes		= 0, i32RecvTotal = 0, i32AddrSize = sizeof(SOCKADDR_IN);
	UINT32 u32TickTime		= (UINT32)GetTickCount();
	SOCKADDR_IN stSrvAddr	= {NULL};

	if (INVALID_SOCKET != sd)
	{
		do	{

			// 일단 무조건 1024 Bytes씩 읽어들이기
			i32RecvBytes = recvfrom(sd,
									(CHAR *)szBuff,
									1024,
									0,
									(struct sockaddr *)&stSrvAddr, &i32AddrSize);

			i32RecvTotal	+= i32RecvBytes;

			/* 무한 루프에 들어 갔는지 여부 확인 - 최대 2 초 동안만 모두 읽기 수행 */
			if (((UINT32)GetTickCount() - u32TickTime) > TCP_RECV_TOTAL_TIMEOUT)	break;

		}	while (i32RecvBytes == 1024);
	}

	// TCP/IP 통신 작업 시간 갱신
	UpdateWorkTime();

	/* --------------------- */
	/* 수신 버퍼 크기 초기화 */
	/* --------------------- */
	m_pPktRingBuff->ResetBuff();

	/* 이전에 패킷 송신 실패한 시간보다 주기가 너무 짧으면 부모에게 알리지 않음 */
	LOG_WARN(m_enSysID, L"All currently received buffers have been discarded");

	return i32RecvTotal;
}

/*
 desc : 대용량 수신 버퍼에 더 이상 수신된 패킷이 없는지 확인
 parm : None
 retn : TRUE - 수신된 버퍼에 남아있는 데이터가 있다, FALSE - 수신된 데이터가 없다
*/
BOOL CMC2Thread::IsEmptyPktBuff()
{
	return (m_pPktRingBuff && m_pPktRingBuff->GetReadSize() < 1) ? TRUE : FALSE;
}

/*
 desc : Alive Check가 일정 횟수 이상 실패한 경우, 강제로 종료 작업 수행
 parm : None
 retn : None
*/
VOID CMC2Thread::SetForceClose()
{
	CloseSocket(m_sdClient);
	m_sdClient	= INVALID_SOCKET;
}

/*
 desc : 데이터가 수신된 경우, 호출됨
 parm : None
 retn : TRUE - 성공적으로 분석 (Ring Buffer) 처리
		FALSE- 분석 처리하지 않음 (아직 완벽하지 않은 패킷임)
*/
BOOL CMC2Thread::RecvData()
{
	TCHAR tzMesg[128]	= {NULL};
	UINT8 u8PktData[512]= {NULL}, *pPktRead	= u8PktData;
	UINT32 u32PktRead	= 0;
	STG_MPRD stData		= {NULL};			/* 콜백 함수에 반환될 구조체 */
	STG_PUCH stUdpHead	= {NULL};

	do	{

		/* return_port (2) + udp_msg_type (1) + short_cycle (1) + reserved (2) + cmd_cnt (4) */;
		u32PktRead	= sizeof(STG_PUCH);
		/* 기본 수신 헤더 크기보다 적게 수신된 상태라면, 더 수신될 때까지 대기 */
		if (m_pPktRingBuff->GetReadSize() < u32PktRead)	return TRUE;
		/* 현재 수신된 버퍼의 시작 위치 얻고, 패킷 헤더 구조체 포인터에 동시 연결 */
		m_pPktRingBuff->CopyBinary((PUINT8)&stUdpHead, u32PktRead);
		/* 반드시 여기서 초기화 해줘야 됨 */
		memset(&stData, 0x00, sizeof(STG_MPRD));
		/* 수신된 데이터의 메시지 종류가 정상인지 에러인지 확인 */
		if ((UINT8)ENG_MUMT::en_cmd_nack == stUdpHead.udp_msg_type)
		{
			/* 현재 복사한 부분까지 버린다. */
			m_pPktRingBuff->PopBinary(u32PktRead);
			/* 이전에 패킷 송신 실패한 시간보다 주기가 너무 짧으면 부모에게 알리지 않음 */
			LOG_WARN(m_enSysID, L"The command buffer is full");
		}
		else if ((UINT8)ENG_MUMT::en_gen3_dnc == stUdpHead.udp_msg_type) /* e_mumt_gen3_dnc */
		{
			/* 패킷이 링 버퍼에 더 들어 있는 확인 (dnc_cmd (1) + dnc_err (1)) */
			if (m_pPktRingBuff->GetReadSize() < (u32PktRead + 2))	return TRUE;
			/* 일단 임시 버퍼로 복사 수행 */
			m_pPktRingBuff->CopyBinary(u8PktData, u32PktRead + 2);
			/* dnc_cmd (E_Gen3DncCmds) & dnc_error 위치로 이동 및 값 저장 */
			pPktRead		= u8PktData + u32PktRead;		/* error (status) 이후 */
			stData.dnc_cmd	= pPktRead[0];
			stData.dnc_err	= pPktRead[1];
			/* 다음에 읽어들일 데이터 위치 설정 */
			u32PktRead		+= 2;
			/* 수신된 패킷 정보에 에러가 있는지 여부에 따라 */
			if (0xff == stData.dnc_cmd)
			{
				TCHAR tzG3Error[96]	= {NULL};
				/* 현재 복사한 부분까지 버린다. */
				m_pPktRingBuff->PopBinary(u32PktRead);
				/* 에러 메시지 출력 */
				switch (stData.dnc_err)
				{
				case 0x00	:	wcscpy_s(tzG3Error, 96, L"No error");															break;
				case 0x08	:	wcscpy_s(tzG3Error, 96, L"The desired function is not implemented");							break;
				case 0x0a	:	wcscpy_s(tzG3Error, 96, L"The object with this index was not found in the object directory");	break;
				case 0x0b	:	wcscpy_s(tzG3Error, 96, L"Error when trying to access the shared memory area");					break;
				case 0x0c	:	wcscpy_s(tzG3Error, 96, L"Error when trying to access the shared memory block");				break;
				case 0x0d	:	wcscpy_s(tzG3Error, 96, L"The serial number included in the command is wrong");					break;
				case 0x0e	:	wcscpy_s(tzG3Error, 96, L"The requested Sub-ID does not exist for this object.");				break;
				case 0x0f	:	wcscpy_s(tzG3Error, 96, L"The object directory is not (yet) valid.");							break;
				default		:	wcscpy_s(tzG3Error, 96, L"Unknow error");														break;
				}
				swprintf_s(tzMesg, 128,
						   L"General Response Error : dnc_cmd(%d) / dnc_err(%d)(%s)",
						   stData.dnc_cmd, stData.dnc_err, tzG3Error);
				LOG_ERROR(m_enSysID, tzMesg);
			}
			else
			{
				if (GetConfig()->set_comn.comm_log_mc2)
				{
					TCHAR tzMesg[128]	= {NULL};
					swprintf_s(tzMesg, 128,
								L"UVDI15 vs. MC2 [Recv] [called_func=RecvData]"
								L"[dnc_cmd=0x%02x][sub_id=0x%02x][obj_index=0x%02x,sub_index=0x%02x][data_len=%d]",
								stData.dnc_cmd, stData.sub_id, stData.obj_index, stData.sub_index, stData.data_len);
					LOG_MESG(m_enSysID, tzMesg);
				}
				/* 에러 정보가 수신 되었다면, 에러 코드까지만 저장 후 다음 수신 데이터 읽도록 조치 */
				switch (stData.dnc_cmd)
				{
				/* 읽기 요청에 대한 응답이 수신된 경우 */
				case ENG_DCG3::en_res_read_obj	:
					/* 패킷이 링 버퍼에 더 들어 있는 확인 (data_len (2) + next_sub_index (4)) */
					if (m_pPktRingBuff->GetReadSize() < (u32PktRead + 6))	return TRUE;
					/* 일단 임시 버퍼로 복사 수행 */
					m_pPktRingBuff->CopyBinary(u8PktData, u32PktRead + 6);
					/* data_len (2) + next_sub_index (4) 저장 */
					pPktRead	= u8PktData + u32PktRead;		/* error (status) 이후 */
					memcpy(&stData.data_len,	pPktRead, 2);	pPktRead	+= 2;
					memcpy(&stData.sub_index,	pPktRead, 4);
					/* 다음에 읽어들일 데이터 위치 설정 */
					u32PktRead	+= 6;
					/* ------------------------------------------------------- */
					/* Sub Index 값이 0 이 아닌 경우 어떻게 처리를 해야 하는지 */
					/* ------------------------------------------------------- */
					if (stData.sub_index != 0 || stData.data_len != 160 /* 16 x 8 (MAX_MC2_DRIVE) */)
					{
						swprintf_s(tzMesg, 128,
								   L"Invalid sub_index or data_size : sub_index(%4d) / data_size(%d)",
								   stData.sub_index, stData.data_len);
						LOG_ERROR(m_enSysID, tzMesg);
					}
					else
					{
						/* 패킷이 링 버퍼에 더 들어 있는 확인 (data_len (size)) */
						if (m_pPktRingBuff->GetReadSize() < (u32PktRead + stData.data_len))	return TRUE;
						/* 일단 임시 버퍼로 복사 수행 */
						m_pPktRingBuff->CopyBinary(u8PktData, u32PktRead + stData.data_len);
						/* 특정 데이터보다 크거나 길이가 0 인지 확인 */
						if (stData.data_len && 0 == stData.sub_index)
						{
							/* data_len 만큼 메모리 할당 후 data 저장 */
							stData.data[stData.data_len]	= 0x00;
							memcpy(stData.data,	u8PktData+u32PktRead, stData.data_len);
						}
						/* 다음에 읽어들일 데이터 위치 설정 */
						u32PktRead	+= stData.data_len;
					}
					break;

				/* 쓰기 요청에 대한 응답이 수신된 경우 */
				case ENG_DCG3::en_res_write_obj	:
					/* 패킷이 링 버퍼에 더 들어 있는 확인 (next_sub_index (4)) */
					if (m_pPktRingBuff->GetReadSize() < (u32PktRead + 4))	return TRUE;
					/* 일단 임시 버퍼로 복사 수행 */
					m_pPktRingBuff->CopyBinary(u8PktData, u32PktRead + 4);
					/* Sub Index 복사 */
					memcpy(&stData.sub_index, u8PktData+u32PktRead, 4);
					/* 다음에 읽어들일 데이터 위치 설정 */
					u32PktRead	+= 4;
					/* ------------------------------------------------------- */
					/* Sub Index 값이 0 이 아닌 경우 어떻게 처리를 해야 하는지 */
					/* ------------------------------------------------------- */
#if 0	/* 엄연히 에러는 아니지만... 구체적으로 무슨 의미인지 이해 불가 */
					if (stData.sub_index != 0)
					{
						swprintf_s(tzMesg, 128, L"Incomprehensible : <recv> <write> sub_index = %4d",
								   stData.sub_index);
						LOG_ERROR(m_enSysID, tzMesg);
					}
#endif
					break;

				/* 기타 명령어인 경우 */
				default	:
					/* 현재 구현되지 않은 패킷이라고 설정 후, 모든 내용을 비우기 위해 크기 설정 */
					stData.undefined	= 0x01;
					u32PktRead			= m_pPktRingBuff->GetReadSize();
					/* 에러 메시지 출력 */
					swprintf_s(tzMesg, 128,
							   L"Undefined Message : msg_type(%d) / dnc_cmd(%d) / dnc_err(%d) / drv_id(%d)",
							   stData.msg_type, stData.dnc_cmd, stData.dnc_err, stData.sub_id);
					LOG_ERROR(m_enSysID, tzMesg);
					break;
				}
				/* 지금까지 읽어들인 패킷 데이터를 버퍼에서 버림 */
				m_pPktRingBuff->PopBinary(u32PktRead);
			}
		}
		/* 이상한 데이터가 수신됨 (기존 수신된 버퍼 모두 비움) */
		else
		{
			m_pPktRingBuff->ResetBuff();
			break;	/* 그냥 다음 수신 처리를 위해 리턴 처리 */
		}

		/* 현재 수신된 패킷의 명령의 개수 정보를 보고 기존 송신한 패킷 정보 검색 */
		if (0 == stData.undefined && 0 == stData.sub_index &&	/* Sub Index 값이 0이 아닌 경우인지 */
			stData.data_len > 0 && stUdpHead.send_cmd_cnt < MAX_PKT_SEND_BUFF &&
			m_pstPktSend[stUdpHead.send_cmd_cnt].cmd)
		{
			/* 기존 송신(요청)된 명령어 정보 저장 */
			stData.msg_type		= stUdpHead.udp_msg_type;
			stData.dnc_cmd		= stData.dnc_cmd & 0x0f;	/* stPktSend.cmd; */
			stData.sub_id		= m_pstPktSend[stUdpHead.send_cmd_cnt].sub_id;
			stData.obj_index	= m_pstPktSend[stUdpHead.send_cmd_cnt].obj_index;
			stData.sub_index	= m_pstPktSend[stUdpHead.send_cmd_cnt].sub_index;
			/* 수신된 패킷 정보 송신 */
			SetRecvPacket(ENG_TPCS::en_recved, &stData);
		}

	}	while (1);

	return TRUE;
}

/*
 desc : 라이브러리 데이터 처리
 parm : type	- [in]  데이터의 종류 (ENG_TPCS) (Sended or Received)
		data	- [in]  데이터가 저장된 버퍼 포인터
 retn : None
*/
VOID CMC2Thread::SetRecvPacket(ENG_TPCS type, LPG_MPRD data)
{
	if (ENG_TPCS::en_recved != type)	return;
#if 0
	STG_MPRD stData	= {NULL};
	memcpy(&stData, data, 16);
	memcpy(stData.data, data+16, 160);
#endif
	/* 수신 데이터 처리 (에러가 없는 경우만 처리) (Write or Read에 대한 응답만 처리) */
	if (0 == data->dnc_err &&
		((UINT8)ENG_DCG3::en_req_read_obj == data->dnc_cmd ||
		 (UINT8)ENG_DCG3::en_req_write_obj == data->dnc_cmd))
	{
		/* 가장 최근에 수신된 패킷 무조건 저장 */
		memcpy(&m_pstShMemMC2->last_recv, data, sizeof(STG_MPRD));
		/* 가장 최근에 수신된 PDO Object 저장 */
		SetRecvData(data->obj_index, data->data, data->data_len);
	}
	else
	{
		if (0 != data->dnc_err)
		{
			TCHAR tzMesg[128]	= {NULL};
			/* 만약 DNC 에러인 경우라면 ... */
			switch (data->dnc_err)
			{
			case ENG_MGEC::en_server_not_impl		:	LOG_ERROR(m_enSysID, L"Function not implemented (The desired function is not implemented)");	break;
			case ENG_MGEC::en_server_obj_not_found	:	LOG_ERROR(m_enSysID, L"The object with this index was not found in the object directory");	break;
			case ENG_MGEC::en_server_shm_access_err	:	LOG_ERROR(m_enSysID, L"Error when trying to access the shared memory area");					break;
			case ENG_MGEC::en_server_shm_block_err	:	LOG_ERROR(m_enSysID, L"Error when trying to access the shared memory block");				break;
			case ENG_MGEC::en_server_serial_error	:	LOG_ERROR(m_enSysID, L"The serial number included in the command is wrong");					break;
			case ENG_MGEC::en_obj_subid				:	LOG_ERROR(m_enSysID, L"The requested Sub-ID does not exist for this object");				break;
			case ENG_MGEC::en_odict_not_valid		:	LOG_ERROR(m_enSysID, L"The object directory is not (yet) valid");							break;
			default									:	swprintf_s(tzMesg, 128, L"MC2 Recv (Unknown Error) : code = %02x (Hex)", data->dnc_err);
														LOG_ERROR(m_enSysID, tzMesg);																break;
			}
		}
	}

	/* 모든 드라이브에 대한 에러 여부 조사하고, 가장 최근에 발생된 에러 값과 비교하여 다르면 저장 작업 수행 */
	AnalysisError();
}

/*
 desc : 수신받은 데이터 처리
 parm : obj_id	- [in]  Object Index (Reference or Actual)
		data	- [in]  수신된 데이터가 저장된 버퍼 포인터
		size	- [in]  데이터 버퍼에 저장된 데이터의 크기
 retn : None
*/
VOID CMC2Thread::SetRecvData(UINT32 obj_id, PUINT8 data, UINT32 size)
{
	switch (obj_id)
	{
	case ENG_PSOI::en_ref_value_collective	:	m_pstShMemMC2->SetDataRefVal(data, size);	break;
	case ENG_PSOI::en_act_value_collective	:	m_pstShMemMC2->SetDataActVal(data, size);	break;
	}
}

/*
 desc : 현재 드라이브에 에러가 발생 했는지 여부에 따라 로그 파일에 저장 유무
 parm : None
 retn : None
*/
VOID CMC2Thread::AnalysisError()
{
	UINT8 i		= 0x00;
	BOOL bError	= FALSE;

	for (; i<MAX_MC2_DRIVE; i++)
	{
		if (i==3)
		{
		}
		else
		{ 
			if (m_u16DrvError[i] != m_pstShMemMC2->act_data[i].error)
			{
				/* 가장 최근에 발생된 에러 값 저장 */	
				m_u16DrvError[i] = m_pstShMemMC2->act_data[i].error;
				/* Error가 발생했다고 설정 */
				if (m_u16DrvError[i])	bError = TRUE;
			}
		}

	}
	/* 에러가 발생 했다면 로그 파일에 등록 */
	if (bError)
	{
		TCHAR tzMesg[128]	= {NULL};
		//swprintf_s(tzMesg, 128, L"MC2 Drive Error : "
		//						L"0 <0x%04x>, 1 <0x%04x>, 2 <0x%04x>, 3 <0x%04x>, "
		//						L"4 <0x%04x>, 5 <0x%04x>, 6 <0x%04x>, 7 <0x%04x>",
		//		   m_u16DrvError[0],m_u16DrvError[1],m_u16DrvError[2],m_u16DrvError[3],
		//		   m_u16DrvError[4],m_u16DrvError[5],m_u16DrvError[6],m_u16DrvError[7]);
		swprintf_s(tzMesg, 128, L"MC2 Drive Error : "
			L"0 <0x%04x>, 1 <0x%04x>, 2 <0x%04x>, "
			L"4 <0x%04x>, 5 <0x%04x>, 6 <0x%04x>, 7 <0x%04x>",
			m_u16DrvError[0], m_u16DrvError[1], m_u16DrvError[2],
			m_u16DrvError[4], m_u16DrvError[5], m_u16DrvError[6], m_u16DrvError[7]);
		LOG_ERROR(m_enSysID, tzMesg);
	}
}

/*
 desc : 기존 발생된 에러 모두 초기화
 parm : None
 retn : None
*/
VOID CMC2Thread::ResetError()
{
	m_u8DncError	= 0x00;
	memset(m_u16DrvError, 0x00, sizeof(UINT16) * MAX_MC2_DRIVE);
}