
/* desc : Luria 소켓 기본 클래스 */

#include "pch.h"
#include "LuriaThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : shmem		- [in]  Luria Shared Memory
		conf		- [in]  Luria 환경 정보
		th_id		- [in]  Thread ID
		source_ip	- [in]  Local IPv4
		target_ip	- [in]  Remote IPv4
		tcp_port	- [in]  TCP/IP Port
		port_size	- [in]  TCP/IP Port Buffer Size
		recv_size	- [in]  Send or Recv의 패킷 중 가장 큰 패킷의 크기 (단위: Bytes)
		ring_size	- [in]	Max Recv Packet Size
		sock_time	- [in]  기본 30 밀리초 동안 소켓 이벤트 감지를 위한 대기
		recv_count	- [in]  연속적으로 수신되는 데이터를 저장할 최소 버퍼 개수 할당
 retn : None
*/
CLuriaThread::CLuriaThread(LPG_LDSM shmem,
						   LPG_CLSI conf,
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

	/* 연결 or 접속 안되었다고 설정 */
	m_bIsLinked			= FALSE;
	m_sdClient			= INVALID_SOCKET;
	/* Luria Shared Memory */
	m_pstShMemLuria		= shmem;
	/* Luria Configuration */
	m_pstConfLuria		= conf;
	m_enSysID			= m_enSysID;

	m_u16ThreadID		= th_id;
	m_u16TcpPort		= tcp_port;
	m_u32PortBuff		= port_size;
	m_u32RecvBuff		= recv_size;
	m_u32RingBuff		= ring_size;
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
	m_u32RecvRepeat		= 0;	// 만약 이 값이 3인 경우, Socket의 recv 함수를 3번 반복 호출
	m_u32RecvSleep		= 0;	// 만약 이 값이 50인 경우, Socket의 recv 함수 호출 이전에 50 msec 대기함
	/* ---------------------------------- */
	/* IP 주소 변환: DWORD -> String 변환 */
	/* ---------------------------------- */
	wmemset(m_tzSourceIPv4, 0x00, IPv4_LENGTH);
	memcpy(&stInAddr.s_addr, &source_ip, sizeof(UINT32));
	InetNtopW(AF_INET, (CONST VOID*)&stInAddr, m_tzSourceIPv4, IPv4_LENGTH);
	wmemset(m_tzTargetIPv4, 0x00, IPv4_LENGTH);
	memcpy(&stInAddr.s_addr, &target_ip, sizeof(UINT32));
	InetNtopW(AF_INET, (CONST VOID*)&stInAddr, m_tzTargetIPv4, IPv4_LENGTH);

	/* --------------------------------------- */
	/* 수신된 패킷 데이터를 임시로 저장할 버퍼 */
	/* --------------------------------------- */
	m_pRecvQue	= new CLuriaQue(recv_count);
	ASSERT(m_pRecvQue);

	/* 스레드 내 루프 속조 제어를 위한 정보 초기화 */
	m_bIsInited	= InitData();
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CLuriaThread::~CLuriaThread()
{
	/* Release Client Socket */
	if (INVALID_SOCKET != m_sdClient)		CloseSocket(m_sdClient);
	/* Socket Event 해제 */
	if (WSA_INVALID_EVENT != m_wsaClient)	WSACloseEvent(m_wsaClient);

	/* 수신 버퍼 메모리 해제 */
	if (m_pPktRecvBuff)	delete [] m_pPktRecvBuff;
	if (m_pPktRingBuff)	delete m_pPktRingBuff;

	/* 동기 진입 */
	if (m_syncPktRecv.Enter())
	{
		/* 기존 Que에 등록된 데이터 메모리 해제 */
		if (m_pRecvQue)
		{
			LPG_PCLR pstPktData	= NULL;
			while (1)
			{
				pstPktData	= m_pRecvQue->PopPktData();
				if (!pstPktData)	break;
				if (pstPktData->user_data)	delete [] pstPktData->user_data;
			}
			/* 임시 패킷 버퍼 메모리 해제 */
			delete m_pRecvQue;
		}
		/* 동기 해제 */
		m_syncPktRecv.Leave();
	}
}

/*
 desc : 통신 스레드 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLuriaThread::InitData()
{
	/* 멤버 초기화 */
	m_sdClient		= INVALID_SOCKET;
	m_wsaClient		= WSA_INVALID_EVENT;
	m_enSocket		= ENG_TPCS::en_closed;

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
BOOL CLuriaThread::InitMemory()
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
	m_wsaClient		= WSACreateEvent();
	if (WSA_INVALID_EVENT == m_wsaClient)
	{
		AfxMessageBox(L"Failed to create the socket_event <Luria Client>", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : When thread is exceuted, the first is called only once
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLuriaThread::StartWork()
{
	return m_bIsInited;
}

/*
 desc : Called by only once at the end before the thread is terminated
 parm : None
 retn : None
*/
VOID CLuriaThread::EndWork()
{
}

/*
 desc : 소켓 에러 관련 정보 설정
 parm : subj	- [in]  에러 타이틀
 retn : None
*/
VOID CLuriaThread::SocketError(PTCHAR subj)
{
	TCHAR tzMesg[128]	= {NULL};

	/* 에러 메시지 설정 */
	m_i32SockError	= WSAGetLastError();
	swprintf_s(tzMesg, 128, L"%s [err_cd = %d]", subj, m_i32SockError);
	/* 로그 등록 */
	LOG_ERROR(m_enSysID, tzMesg);
}

/*
 desc : 소켓 옵션 설정하기 - 송신/수신 버퍼 크기 설정
 parm :	flag	- [in]  0x01:Send Buffer, 0x10:Recv Buffer, 0x11:Send/Recv Buffer
		sd		- [in]  소켓 옵션을 설정한 소켓 기술자
		size	- [in]  Port Buffer Size
 retn : TRUE or FALSE
*/
BOOL CLuriaThread::SetSockOptBuffSize(UINT8 flag, SOCKET sd)
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
		SocketError(L"Socket Option : Buffer Size [Luria]");
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
BOOL CLuriaThread::SetSockOptTimeout(UINT8 flag, SOCKET sd, INT32 time)
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
		SocketError(L"Socket Option : Time-out [Luria]");
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
BOOL CLuriaThread::SetSockOptSpeed(INT32 optval, SOCKET sd)
{
	/* 소켓 데이터 속도 설정 */
	if (SOCKET_ERROR == setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (CHAR *)&optval, sizeof(optval)))
	{
		SocketError(L"Socket Option : Speed [Luria]");
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
BOOL CLuriaThread::SetSockOptAlive(INT32 optval, SOCKET sd)
{
	/* 소켓 데이터 속도 설정 */
	if (SOCKET_ERROR == setsockopt(sd, SOL_SOCKET, SO_KEEPALIVE, (CHAR *)&optval, sizeof(optval)))
	{
		SocketError(L"Socket Option : Alive check [Luria]");
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
BOOL CLuriaThread::SetSockOptRoute(INT32 optval, SOCKET sd)
{
	/* 소켓 데이터 속도 설정 */
	if (SOCKET_ERROR == setsockopt(sd, SOL_SOCKET, SO_DONTROUTE, (CHAR *)&optval, sizeof(optval)))
	{
		SocketError(L"Socket Option : Route [Luria]");
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
VOID CLuriaThread::CloseSocket(SOCKET sd, UINT32 time_out)
{
	INT32 i32Code	= 0;
	LINGER stLinger	= {NULL};
	fd_set readfds, errorfds;
	timeval timeout;
	CUniToChar csCnv1, csCnv2;

	/* Shutdown 함수 호출하기 전에 반드시 아래 플래그부터 설정해 줌 */
	m_enSocket		= ENG_TPCS::en_closed;
	/* 연결 해제되었다고 플래그 설정 */
	m_bIsLinked		= FALSE;
	/* 현재 Client 소켓이 살아 있는지 여부 */
	if (INVALID_SOCKET != sd)
	{
		TCHAR tzMesg[256]	= {NULL};
		swprintf_s(tzMesg, 256, L"The connection to the remote system has been disconnected (CloseSocket) "
								L"(src:%s tgt:%s / err_cd:%d)",
				   m_tzSourceIPv4, m_tzTargetIPv4, m_i32SockError);
		if (!IsRunDemo())	LOG_ERROR(m_enSysID, tzMesg);

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
		closesocket(sd);
	}

	/* Update the communication time for TCP/IP */
	UpdateWorkTime();

	/* 연결 해제된 시간 저장 */
	GetLocalTime(&m_stLinkTime);
}

/*
 desc : 가장 최근에 통신한 시간 저장 및 관리
 parm : None
 retn : None
*/
VOID CLuriaThread::UpdateWorkTime()
{
	m_u64WorkTime = GetTickCount64();
	GetLocalTime(&m_stWorkTime);
}

/*
 desc : Remote Server로부터 연결 해제 시그널이 발생한 경우
 parm : sd	- [in]  Client socket descriptor
 retn : None
*/
VOID CLuriaThread::Closed(SOCKET sd)
{
	/* --------------------------------------------------------- */
	/* 반드시 가상 함수 호출 후, 소켓 핸들 해제 함수 호출해야 함 */
	/* --------------------------------------------------------- */

	/* 소켓 연결 해제 작업 수행 */
	CloseSocket(sd);
}

/*
 desc : 실제 패킷 데이터 송신부
 parm : packet	- [in]  송신 패킷이 저장된 버퍼
		size	- [out] 입력: 'packet' 버퍼에 저장된 패킷 크기
						반환: 전송된 데이터의 크기
 retn : TRUE or FALSE (통신 이상이 발생한 경우)
*/
BOOL CLuriaThread::Sended(PUINT8 packet, UINT32 &size)
{
	BOOL bSucc		= TRUE;
	CHAR *pPktSend		= (CHAR*)packet;
	INT32 i32SendBytes	= 0, i32SendSize = INT32(size);
	UINT32 u32TotalSize	= 0, u32TickTime = (UINT32)GetTickCount();

	/* 송신 전에 무조건 송신 모드로 설정 */
	m_enSocket	= ENG_TPCS::en_sending;

	do	{

		i32SendBytes = send(m_sdClient, pPktSend, i32SendSize, 0);
		if (SOCKET_ERROR == i32SendBytes)
		{
			// 전송 지연 모드가 아니다라면... 통신을 끊어야 된다.
			INT32 i32SockError	= WSAGetLastError();
			if (i32SockError != WSAEWOULDBLOCK &&
				i32SockError != WSA_IO_PENDING)
			{
				SocketError(L"Failed to send the packet data [Luria]");
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
			/* 현재까지 전송된 데이터 크기 저장 */
			u32TotalSize+= i32SendBytes;
			/* 다음에 전송될 데이터 크기 재계산 */
			i32SendSize	-= i32SendBytes;
			/* 다음에 전송할 위치로 이동 */
			pPktSend	+= i32SendBytes;
		}

		/* 무한 루프에 들어 갔는지 여부 확인 */
		if (((UINT32)GetTickCount() - u32TickTime) > TCP_SEND_TOTAL_TIMEOUT)
		{
			bSucc	= FALSE;
			break;
		}

	}	while (i32SendSize > 0);

	/* 패킷 데이터를 전송 성공 했으면 전송한 시간 갱신 */
	if (bSucc)
	{
		if (u32TotalSize == size)
		{
			UpdateWorkTime();
			m_enSocket = ENG_TPCS::en_sended;
		}
		/* 송신 실패인 경우 */
		else	bSucc = FALSE;
	}

	/* 패킷을 전송한 크기 반환 */
	size = u32TotalSize;

	if (!bSucc)	m_enSocket = ENG_TPCS::en_send_failed;

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
BOOL CLuriaThread::ReSended(PUINT8 packet, UINT32 size, UINT32 retry, UINT32 wait)
{
	PUINT8 pPacket	= packet;
	UINT32 u32Size	= size, u32Total = 0, u32Retry = retry;

	do {

		/* 전송 실패한 경우는 통신 에러일 가능성이 큼!!! */
		if (!Sended(pPacket, u32Size))	return FALSE;

		/* 일부 혹은 전체 패킷이 전송했는지 확인 */
		/* 현재까지 전송한 데이터 크기 저장 */
		u32Total += u32Size;
		/* 모든 데이터가 전송된 경우인지 조사 */
		if (u32Total == size)	return TRUE;

		/* 재-전송을 위한 버퍼의 시작 위치 설정  */
		pPacket += u32Size;

		/* 일부 데이터만 전송된 경우 나머지 패킷 */
		/* 데이터의 재-전송을 위해서 크기 재계산 */
		u32Size = size - u32Total;

		/* 일정 시간 지연 (단위: 밀리초) */
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
BOOL CLuriaThread::Received(SOCKET sd)
{
	BOOL bSucc	= TRUE;
	INT32 i32ReadBytes, i32RecvSize;
	UINT32 u32SpareBytes, u32Repeat = 1;	/* 0 값이 아니라, 반드시 1 값임 */
	UINT32 u32TickTime	= (UINT32)GetTickCount();

	m_enSocket	= ENG_TPCS::en_recving;

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
		i32ReadBytes = recv(sd, (CHAR *)m_pPktRecvBuff, i32RecvSize, 0);

		// 소켓 에러가 발생한 경우
 		if (i32ReadBytes == SOCKET_ERROR)
 		{
 			// IO Pending 에러가 아니고, 소켓 에러이면 통신 연결을 해제 한다.
			INT32 i32SockError	= WSAGetLastError();
 			if (i32SockError != WSA_IO_PENDING && i32SockError != WSAEWOULDBLOCK)
 			{
				SocketError(L"Failed to recv the packet [CLuriaThread::Received]");
				return FALSE;
 			}
			
			// TCP 통신 시간 갱신
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
				// 수신되었다고 모드 값 변경
				m_enSocket	= ENG_TPCS::en_recved;
				// 이도저도 아니면, 다음 번에 다시 읽도록 반환 한다
				return TRUE;
			}
 		}

		/* 일단 수신된 데이터를 Ring 버퍼 공간에 저장 */
		m_pPktRingBuff->WriteBinary(m_pPktRecvBuff, i32ReadBytes);
		// TCP 통신 시간 갱신
		UpdateWorkTime();

		/* 요청한 데이터 크기보다 수신된 데이터 크기가 작으면 루틴 빠져 나가기 */
		u32SpareBytes	= m_pPktRingBuff->GetWriteSize();
		if (i32ReadBytes < i32RecvSize || u32SpareBytes < 1)	break;

		/* 무한 루프에 들어 갔는지 여부 확인 */
		if (((UINT32)GetTickCount() - u32TickTime) > TCP_RECV_TOTAL_TIMEOUT)
		{
			LOG_ERROR(m_enSysID, L"An infinite loop has been entered [CLuriaThread::Received] (3 secs)");
			bSucc	= FALSE;
			break;
		}

	} while (1);
 	
	/* 수신되었다고 모드 값 변경 */
	if (bSucc)
	{
		m_enSocket		= ENG_TPCS::en_recved;
		m_i32SockError	= 0;	/* 소켓 수신 성공이면 */
	}
	else		m_enSocket	= ENG_TPCS::en_recv_failed;

	/* 데이터 분석 실패할 경우, 기존 통신 포트의 수신 버퍼 모두 해제 */
	if (!bSucc)
	{
		ReceiveAllData(sd);
	}
	else
	{
		bSucc	= RecvData();
		if (!bSucc)
		{
			ReceiveAllData(sd);
		}
	}

	return bSucc;
}

/*
 desc : 현재 통신 포트의 수신 버퍼에 남아 있는 모든 데이터를 읽어서 버린다.
		더 이상 읽어들일 데이터가 없을 때까지 ...
 parm : sd	- [in]  모든 데이터를 읽어들이고자 하는 소켓 핸들
 retn : 총 읽어서 버린 수신 바이트 크기 (-1 값이면 전혀 읽어들인 데이터가 없다?)
*/
INT32 CLuriaThread::ReceiveAllData(SOCKET sd)
{
	CHAR szBuff[1024+1]	= {NULL};
	INT32 i32RecvBytes	=0, i32RecvTotal	= 0;
	UINT32 u32TickTime	= (UINT32)GetTickCount();

	if (INVALID_SOCKET != sd)
	{
		do	{

			/* 일단 무조건 1024 Bytes씩 읽어들이기 */
			i32RecvBytes	= recv(sd, (CHAR *)szBuff, 1024, 0);
			i32RecvTotal	+= i32RecvBytes;

			/* 무한 루프에 들어 갔는지 여부 확인 - 최대 2 초 동안만 모두 읽기 수행 */
			if (((UINT32)GetTickCount() - u32TickTime) > TCP_RECV_TOTAL_TIMEOUT)	break;

		}	while (i32RecvBytes == 1024);
	}

	/* TCP/IP 통신 작업 시간 갱신 */
	UpdateWorkTime();

	/* 수신 버퍼 크기 초기화 */
	m_pPktRingBuff->ResetBuff();

	/* 이전에 패킷 송신 실패한 시간보다 주기가 너무 짧으면 부모에게 알리지 않음 */
	LOG_ERROR(m_enSysID, L"Read all the data in the buffer and discarded it");

	return i32RecvTotal;
}

/*
 desc : 대용량 수신 버퍼에 더 이상 수신된 패킷이 없는지 확인
 parm : None
 retn : TRUE - 수신된 버퍼에 남아있는 데이터가 있다, FALSE - 수신된 데이터가 없다
*/
BOOL CLuriaThread::IsEmptyPktBuff()
{
	return (m_pPktRingBuff && m_pPktRingBuff->GetReadSize() < 1) ? TRUE : FALSE;
}

/*
 desc : 스레드에서 일정 시간동안 통신 작업을 수행하지 않았는지 여부 ...
 parm : time_gap	- [in]  이 주어진 시간 간격 값. 즉, 이 시간(단위: 밀리초)동안 통신이 이루어지지 않았는지 확인하기 위함
 retn : TRUE - 해당 시간동안 통신 관련 작업이 수행되지 않았음
		FALSE - 해당 주어진 내에 통신 관련 작업이 수행되었음
*/
BOOL CLuriaThread::IsTcpWorkIdle(UINT32 time_gap)
{
	return (GetTickCount64() - m_u64WorkTime) > time_gap ? TRUE : FALSE;
}

/*
 desc : 재접속 작업을 수행해도 되는지 여부 (즉, 재접속 수행 시간이 되었는지 여부)
 parm : time_gap	- [in] 이 주어진 시간 가격 값 (단위: 밀리초)
 retn : TRUE - 재접속 작업을 수행하라, FALSE - 아직 재접속 작업 수행하지 말아라
*/
BOOL CLuriaThread::IsTcpWorkConnect(UINT32 time_gap)
{
	return (GetTickCount64() - m_u64ConnectTime) > time_gap ? TRUE : FALSE;
}

/*
 desc : Alive Check가 일정 횟수 이상 실패한 경우, 강제로 종료 작업 수행
 parm : None
 retn : None
*/
VOID CLuriaThread::SetForceClose()
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
BOOL CLuriaThread::RecvData()
{
	UINT32 u32PktBase	= sizeof(STG_PCLR)-sizeof(PUINT8);
	UINT64 u64SavedBytes;
	STG_PCLR stPktPdata;

	do	{

		/* ------------------------------------------------------------------------------- */
		/* 현재 수신된 버퍼의 크기를 얻어온 후 패킷 헤더 크기(8 Bytes) 조차 받지 못한 경우 */
		/* 다음에 더 읽어롤 때까지 더 이상 작업을 수행하지 않도록 처리 한다                */
		/* ------------------------------------------------------------------------------- */
		u64SavedBytes	= m_pPktRingBuff->GetReadSize();
		if (u64SavedBytes < u32PktBase)
		{
			m_enSocket	= ENG_TPCS::en_recving;
			return TRUE;
		}

		/* ---------------------------------------------------------------*/
		/* !!! 아래 패킷 구조체는 반드시 이 부분에서 메모리 해제 수행 !!! */
		/* 패킷이 연속적으로 수신될 때마다 user_data의 변수 초기화를 수행 */
		/* ---------------------------------------------------------------*/
		memset(&stPktPdata, 0x00, sizeof(STG_PCLR));

		// 헤더 정보 복사
		m_pPktRingBuff->CopyBinary((PUINT8)&stPktPdata, u32PktBase);
		stPktPdata.SwapNetworkToHost();
		/* 잘못된 패킷일 경우 에러 처리 */
		if (stPktPdata.num_bytes < 8)
		{
			LOG_WARN(m_enSysID, L"Header size not received. (header size < 8 bytes)");
			return TRUE;
		}
		/* --------------------------------------------------------------------------------- */
		/* 현재 수신된 버퍼의 크기를 얻어온 후 패킷 본문 크기(Tail 포함) 조차 받지 못한 경우 */
		/* 다음에 더 읽어롤 때까지 더 이상 작업을 수행하지 않도록 처리 한다                  */
		/* --------------------------------------------------------------------------------- */
		if (u64SavedBytes < stPktPdata.num_bytes)
		{
			LOG_WARN(m_enSysID, L"Not all packets were received. Waiting ...");
			return TRUE;
		}
		/* ------------------------------------------- */
		/* user data 영역 이전까지 모두 읽어서 버린다. */
		/* ------------------------------------------- */
		m_pPktRingBuff->PopBinary(u32PktBase);
		/* ----------------------------------------------------------------------- */
		/* 버퍼에 하나 이상의 패킷이 저장되어 있다면 실제로 데이터를 읽어도록 한다 */
		/* ----------------------------------------------------------------------- */
		if (stPktPdata.num_bytes-u32PktBase > 0)
		{
			/* 수신된 패킷의 본문 데이터 저장을 위한 메모리 할당 */
			stPktPdata.user_data	= new UINT8 [stPktPdata.num_bytes-u32PktBase+1];
			stPktPdata.user_data[stPktPdata.num_bytes-u32PktBase]	= 0x00;
			/* 수신된 본문 데이터 나머지 가져오기  */
			m_pPktRingBuff->ReadBinary(stPktPdata.user_data, stPktPdata.num_bytes-u32PktBase);
		}
		/* ----------------------------------------------------------------------- */
		/* 현재 수신된 패킷 데이터를 수신 큐 버퍼에 등록 작업을 수행하도록 처리 함 */
		/* ----------------------------------------------------------------------- */
		/* 동기 진입 */
		if (m_syncPktRecv.Enter())
		{
			if (!m_pRecvQue->PushPktData(&stPktPdata))
			{
				/* 패킷 수신 버퍼가 이미 FULL 상태 */
				LOG_ERROR(m_enSysID, L"There are no more buffers for received packets");
				/* 기존에 생성된 메모리 해제*/
				if (stPktPdata.user_data)
				{
					delete [] stPktPdata.user_data;
					stPktPdata.user_data	= NULL;
				}
			}
			/* 동기 해제 */
			m_syncPktRecv.Leave();
		}

		/* ------------------------------------------------------------------------------- */
		/* 현재 패킷 하나를 읽고 난 이후, 버퍼에 남아 있는 데이터의 크기가 패킷을 구성하는 */
		/* 최소의 패킷 크기만큼 남아 있으면, 루프를 한번 더 수행하고, 최소 패킷 크기만큼   */
		/* 남아 있지 않으면 더 읽어오도록 해당 루프를 빠져나가도록 처리 한다               */
		/* ------------------------------------------------------------------------------- */
	}	while (!m_pRecvQue->IsQueEmpty() && (u64SavedBytes - stPktPdata.num_bytes) >= u32PktBase);

	return TRUE;
}

/*
 desc : Luria Status 코드와 매핑되는 에러 문자열 (설명) 반환
 parm : code	- [in]  Luria Status (Error) Code
		mesg	- [in]  Return buffer
		size	- [in]  'mesg' buffer size
 retn : None
*/
VOID CLuriaThread::GetLuriaStatusToDesc(UINT16 code, PTCHAR mesg, UINT32 size)
{
	switch (code)
	{
	case 0		:	wcscpy_s(mesg, size, L"S_OK");	break;
	case 10000	:	wcscpy_s(mesg, size, L"IncorrectMsgSizeInHeader 10000");				break;
	case 10001	:	wcscpy_s(mesg, size, L"UnknownProtocolVer 10001");						break;
	case 10002	:	wcscpy_s(mesg, size, L"UnknownCommandType 10002");						break;
	case 10003	:	wcscpy_s(mesg, size, L"UnknownUserDataFamily 10003");					break;
	case 10004	:	wcscpy_s(mesg, size, L"UnknownUserDataId 10004");						break;
	case 10005	:	wcscpy_s(mesg, size, L"IncorrectMsgSize 10005");						break;
	case 10006	:	wcscpy_s(mesg, size, L"ParameterOutOfRange 10006");						break;
	case 10007	:	wcscpy_s(mesg, size, L"InternalXmlFileNotFound 10007");					break;
	case 10008	:	wcscpy_s(mesg, size, L"ReadNotAllowed 10008");							break;
	case 10009	:	wcscpy_s(mesg, size, L"WriteNotAllowed 10009");							break;
	case 10010	:	wcscpy_s(mesg, size, L"MaxStringLengthExceeded 10010");					break;
	case 10011	:	wcscpy_s(mesg, size, L"MaxJobNoExceeded 10011");						break;
	case 10012	:	wcscpy_s(mesg, size, L"JobCreationFailed 10012");						break;
	case 10013	:	wcscpy_s(mesg, size, L"NoJobsAssigned 10013");							break;
	case 10014	:	wcscpy_s(mesg, size, L"JobAlreadyAssigned 10014");						break;
	case 10015	:	wcscpy_s(mesg, size, L"SelectingUnassignedJob 10015");					break;
	case 10016	:	wcscpy_s(mesg, size, L"JobNotLoaded 10016");							break;
	case 10017	:	wcscpy_s(mesg, size, L"PreprocXmlFileNotFound 10017");					break;
	case 10018	:	wcscpy_s(mesg, size, L"ArtworkOutsideTableLimits 10018");				break;
	case 10019	:	wcscpy_s(mesg, size, L"InternalError 10019");							break;
	case 10020	:	wcscpy_s(mesg, size, L"CommsWithPhotoheadFailed 10020");				break;
	case 10021	:	wcscpy_s(mesg, size, L"StripFileNotFound 10021");						break;
	case 10022	:	wcscpy_s(mesg, size, L"IncompatibleProductId 10022");					break;
	case 10023	:	wcscpy_s(mesg, size, L"NoConnectionToAnnounceServer 10023");			break;
	case 10024	:	wcscpy_s(mesg, size, L"BusyLoadingToSsd 10024");						break;
	case 10025	:	wcscpy_s(mesg, size, L"BusyPrePrinting 10025");							break;
	case 10026	:	wcscpy_s(mesg, size, L"BusyPrinting 10026");							break;
	case 10027	:	wcscpy_s(mesg, size, L"PrePrintRequired 10027");						break;
	case 10028	:	wcscpy_s(mesg, size, L"NotAllowedWithJobsAssigned 10028");				break;
	case 10029	:	wcscpy_s(mesg, size, L"FunctionNotImplementedInLuria 10029");			break;
	case 10030	:	wcscpy_s(mesg, size, L"DirectoryNotFound 10030");						break;
	case 10031	:	wcscpy_s(mesg, size, L"TimeoutWaitingForSsd 10031");					break;
	case 10032	:	wcscpy_s(mesg, size, L"CommandSizeTooLarge 10032");						break;
	case 10033	:	wcscpy_s(mesg, size, L"InvalidPhCmdControlValue 10033");				break;
	case 10034	:	wcscpy_s(mesg, size, L"IncompatibleRotatePhotoheadSetting 10034");		break;
	case 10035	:	wcscpy_s(mesg, size, L"TimeoutWaitingForTrigs 10035");					break;
	case 10036	:	wcscpy_s(mesg, size, L"RegistrationError 10036");						break;
	case 10037	:	wcscpy_s(mesg, size, L"LicenseError 10037");							break;
	case 10038	:	wcscpy_s(mesg, size, L"InvalidPhCommand 10038");						break;
	case 10039	:	wcscpy_s(mesg, size, L"InvalidZoneNumber 10039");						break;
	case 10040	:	wcscpy_s(mesg, size, L"PhotoheadOffsetOutsideValidRange 10040");		break;
	case 10041	:	wcscpy_s(mesg, size, L"CrcErrorWriteSsd 10041");						break;
	case 10042	:	wcscpy_s(mesg, size, L"CrcErrorReadSsd 10042");							break;
	case 10043	:	wcscpy_s(mesg, size, L"CommsWithPhotoheadDisabled 10043");				break;
	case 10044	:	wcscpy_s(mesg, size, L"IncompatibleNumPhotoheadsPitchSetting 10044");	break;
	case 10045	:	wcscpy_s(mesg, size, L"JobPreprocessedWithOutdatedVersion 10045");		break;
	case 10046	:	wcscpy_s(mesg, size, L"DataUnderflow 10046");							break;
	case 10047	:	wcscpy_s(mesg, size, L"DcodeNotFound 10047");							break;
	case 10048	:	wcscpy_s(mesg, size, L"StringTooLong 10048");							break;
	case 10049	:	wcscpy_s(mesg, size, L"TooManyFiducials 10049");						break;
	case 10050	:	wcscpy_s(mesg, size, L"InvalidParamVal 10050");							break;
	case 10051	:	wcscpy_s(mesg, size, L"InvalidFormatSerialNum 10051");					break;
	case 10052	:	wcscpy_s(mesg, size, L"PanelDataOutsidePlaceholder 10052");				break;
	case 10053	:	wcscpy_s(mesg, size, L"UdpErrorMotionCtrl 10053");						break;
	case 10054	:	wcscpy_s(mesg, size, L"XcorrectionTableTooLarge 10054");				break;
	case 10055	:	wcscpy_s(mesg, size, L"BusySimulatingPrint 10055");						break;
	case 10056	:	wcscpy_s(mesg, size, L"FixedRegistrationNotSet 10056");					break;
	case 10057	:	wcscpy_s(mesg, size, L"JobBusyProcessing 10057");						break;
	case 10058	:	wcscpy_s(mesg, size, L"InitHardwareFailed 10058");						break;
	case 10059	:	wcscpy_s(mesg, size, L"NeedToRunInitHardware 10059");					break;
	case 10060	:	wcscpy_s(mesg, size, L"IncompatiblePhotoheadAndProdId 10060");			break;
	case 10061	:	wcscpy_s(mesg, size, L"IncompatibleSpxFactor 10061");					break;
	case 10062	:	wcscpy_s(mesg, size, L"LuriaDiskFailureWarning 10062");					break;
	case 10063	:	wcscpy_s(mesg, size, L"LuriaDiskFullWarning 10063");					break;
	case 10064	:	wcscpy_s(mesg, size, L"MotionControllerError 10064");					break;
	case 10065	:	wcscpy_s(mesg, size, L"PanelDataConflictingPositions 10065");			break;
	case 10066	:	wcscpy_s(mesg, size, L"PanelDataTextTooLong 10066");					break;
	case 10067	:	wcscpy_s(mesg, size, L"IncompatiblePrintDirection 10067");				break;
	case 10068	:	wcscpy_s(mesg, size, L"FuncNotAvailForCurrMotorcontroller 10068");		break;
	case 10069	:	wcscpy_s(mesg, size, L"YcorrectionTableTooLarge 10069");				break;
	case 10070	:	wcscpy_s(mesg, size, L"MixedPhotoheadFwVersions 10070");				break;
	case 10071	:	wcscpy_s(mesg, size, L"PreprocStripTooLong 10071");						break;
	case 10072	:	wcscpy_s(mesg, size, L"InvalidFirmwareVersion 10072");					break;
	case 10073	:	wcscpy_s(mesg, size, L"InvalidPhotoheadType 10073");					break;
	case 10074	:	wcscpy_s(mesg, size, L"BusyUpgradingFirmware 10074");					break;
	case 10075	:	wcscpy_s(mesg, size, L"AutoUpgradeNotSupported 10075");					break;
	case 10076	:	wcscpy_s(mesg, size, L"FirmwareUpgradeFailed 10076");					break;
	case 10077	:	wcscpy_s(mesg, size, L"LightSourceNotConnected 10077");					break;
	case 10078	:	wcscpy_s(mesg, size, L"FunctionNotAvailable 10078");					break;
	case 10079	:	wcscpy_s(mesg, size, L"FocusMotorOperationFailed 10079");				break;
	case 10080	:	wcscpy_s(mesg, size, L"StripFileSizeTooLarge 10080");					break;
	case 10081	:	wcscpy_s(mesg, size, L"JobExceedsStripLimit 10081");					break;
	case 10082	:	wcscpy_s(mesg, size, L"CopyAfLastStripeNotPossible 10082");				break;
	case 10083	:	wcscpy_s(mesg, size, L"LightSourceStatusFailed 10083");					break;
	case 10084	:	wcscpy_s(mesg, size, L"FocusMotorNotInitialized 10084");				break;
	case 10085	:	wcscpy_s(mesg, size, L"WarningWarpOutsideFpgaLimits 10085");			break;
	case 10086	:	wcscpy_s(mesg, size, L"TrigFreqTooHigh 10086");							break;
	case 10087	:	wcscpy_s(mesg, size, L"InconsistentSettingsInHeads 10087");				break;
	case 10088	:	wcscpy_s(mesg, size, L"InvalidTablePosition 10088");					break;
	case 10089	:	wcscpy_s(mesg, size, L"NumberOfEntriesExceeded 10089");					break;
	case 10090	:	wcscpy_s(mesg, size, L"PanelThicknessEntryNotFound 10090");				break;
	case 10091	:	wcscpy_s(mesg, size, L"IncompatibleMsgContent 10091");					break;
	case 10092	:	wcscpy_s(mesg, size, L"AFSensorMeasureLaserPWMFailed 10092");			break;
	case 10093	:	wcscpy_s(mesg, size, L"WarningBadRaidVolumeHealth 10093");				break;
	case 10094	:	wcscpy_s(mesg, size, L"IncompatibleBufferSettings 10094");				break;
	case 10095	:	wcscpy_s(mesg, size, L"InvalidSsdInfo 10095");							break;
	case 10096	:	wcscpy_s(mesg, size, L"InvalidPaneldataBitmapInput 10096");				break;
	case 10097	:	wcscpy_s(mesg, size, L"IncompatibleJobParameters 10097");				break;
	case 10098	:	wcscpy_s(mesg, size, L"InvalidMotionCtrlFwVersion 10098");				break;
	case 10099	:	wcscpy_s(mesg, size, L"MainboardTempTooHigh 10099");					break;
	case 10100	:	wcscpy_s(mesg, size, L"AfNotPossibleWithoutEthercat 10100");			break;
	case 10101	:	wcscpy_s(mesg, size, L"TimeoutWaitingForStreaming 10101");				break;
	case 10102	:	wcscpy_s(mesg, size, L"ScrollRateSettingTooHigh 10102");				break;
	case 10103	:	wcscpy_s(mesg, size, L"MixedNumberOfLightSourcesInHeads 10103");		break;
	case 10104	:	wcscpy_s(mesg, size, L"TableCalibrationFailed 10104");					break;
	case 10105	:	wcscpy_s(mesg, size, L"TableCalibrationPreproJobFailed 1010105");		break;
	}
}

/*
 desc : Luria 로부터 에러가 발생한 경우, 처리
 parm : level	- [in]  메시지 에러 레벨 (ENG_LNWE)
		type	- [in]  0x00:Client, 0x01:Server(Announce)
		data	- [in]  에러 패킷 정보가 저장된 구조체 포인터
 retn : None
*/
VOID CLuriaThread::SetPktError(ENG_LNWE level, UINT8 type, LPG_PCLR data)
{
	TCHAR tzMesg[128]	= {NULL};
	TCHAR tzStatus[128]	= {NULL};
	TCHAR tzType[2][8]	= { L"Client", L"Server" };
	CUniToChar csCnv;

	GetLuriaStatusToDesc(data->status, tzStatus, 128);
	swprintf_s(tzMesg, 128,
			   L"An error has occurred (Luria)(%s) fid=0x%02x, uid=0x%02x, status=%d <%s>",
			   tzType[type], data->user_data_family, data->user_data_id, data->status, tzStatus);
	switch (level)
	{
	case ENG_LNWE::en_fatal		:
	case ENG_LNWE::en_error		:	LOG_ERROR(m_enSysID, tzMesg);	break;
	case ENG_LNWE::en_warning	:	LOG_WARN(m_enSysID, tzMesg);	break;
	default						:	LOG_MESG(m_enSysID, tzMesg);	break;
	}
#if 0
	/* 왜 에러가 발생하는지 모름 */
	if (data->user_data_family == (UINT8)ENG_LUDF::en_luria_com_management &&
		data->user_data_id == (UINT8)ENG_LCCM::en_announce_server_ipaddr)	return;
#endif
//#ifdef _DEBUG
	//GetConfig()->set_comn.mesg_box_optic	= 0x01;
//#endif
	if (GetConfig()->set_comn.mesg_box_optic && data->status != 10008)
	{
		AfxMessageBox(tzMesg, MB_ICONSTOP|MB_TOPMOST);
	}
	/* 가장 최근에 발생된 Luria 에러 값 임시 저장 (기존에 에러 코드가 존재하면 저장하지 않음) */
#if 0
	if (data->status != m_pstShMemLuria->link.u16_error_code)
	{
		m_pstShMemLuria->link.u16_error_code = data->status;
	}
#else
	/* 가장 최근의 상태 값으로 무조건 갱신 */
	m_pstShMemLuria->link.u16_error_code	= data->status;
#endif
}