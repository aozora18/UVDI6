
/* desc : MC2 ���� �⺻ Ŭ���� */

#include "pch.h"
#include "MC2Thread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

#define	MAX_PKT_SEND_BUFF	128		/* �ӽ� �۽� ��Ŷ�� ������ �� �ִ� ���� ���� */

/*
 desc : ������
 parm : shmem		- [in]  MC2 Service ���� ���� �޸�
		source_ip	- [in]  Local IPv4
		target_ip	- [in]  Remote IPv4
		mc2_port	- [in]  TCP/IP Port (MC2�� �����͸� �۽��ϱ� ���� ��Ʈ)
		cmps_port	- [in]  MC2�κ��� CMPS�� ����ޱ� ���� ��� ��Ʈ
		port_size	- [in]  TCP/IP Port Buffer Size
		recv_size	- [in]  Send or Recv�� ��Ŷ �� ���� ū ��Ŷ�� ũ�� (����: Bytes)
		ring_size	- [in]	Max Recv Packet Size
		sock_time	- [in]  �⺻ xx �и��� ���� ���� �̺�Ʈ ������ ���� ���
 retn : None
*/
CMC2Thread::CMC2Thread(LPG_MDSM shmem,
					   UINT16 mc2_port,
					   UINT16 cmps_port,
					   UINT32 source_ip,
					   UINT32 target_ip,
					   UINT32 port_size,
					   UINT32 recv_size,	// Packet ������ �ִ� ũ�� (����: Bytes)
					   UINT32 ring_size,	// Ring Buffer ũ�� (����: Bytes)
					   UINT32 sock_time)	// ���� �̺�Ʈ�� �����ϴµ� �ּ� ��� �ð� (����: �и���)
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
	/* �ּ� ��ٸ��� �ð��� 0 ���̸� �⺻ ������ 10�� �־��ָ� �˴ϴ�.*/
	m_u32SockTime		= sock_time < 10 ? 10 : sock_time;	// �⺻ 10 �и��� ���� ���� �̺�Ʈ ������ ���� ���
	/* ���� �޸� ���� */
	m_pstShMemMC2		= shmem;
	/* ��Ŷ �۽� ���� �ð� �ʱ�ȭ */
	m_u64TCPFailTime	= GetTickCount64();

	/* ----------------------------------------------------------------------------- */
	/* ������ 0 ������ �ʱ�ȭ �ǳ�, Ư�� ���κ��� ���� �̺�Ʈ�� �߻��� ���, ������*/
	/* ���� �۾� ��, Socket�� recv �Լ� ȣ���� ���� �Ⱓ �� Ƚ���� �ݺ������� ������ */
	/* �� �� �̻� ���ŵ� �����Ͱ� ������ Ȯ���� ��, �б� �۾��� �����߸� �ϴ� ��찡 */
	/* ������, �̷� ���, �Ʒ� �� ���� ������ �б� �ݺ� Ƚ���� �ݺ� �б� ������ ��� */
	/* �ϴ� �ð� (����: msec) ���� ������ ��� �Ѵ�. �� �κ��� �� �ʿ��� ���� ����� */
	/* ----------------------------------------------------------------------------- */
	m_u32RecvRepeat		= 0;	// ���� �� ���� 3�� ���, Socket�� recv �Լ��� 3�� �ݺ� ȣ��
	m_u32RecvSleep		= 0;	// ���� �� ���� 50�� ���, Socket�� recv �Լ� ȣ�� ������ 50 msec �����
	m_i32SockError		= 0;

	ResetError();

	/* ---------------------------------- */
	/* IP �ּ� ��ȯ: DWORD -> String ��ȯ */
	/* ---------------------------------- */
	wmemset(m_tzSourceIPv4, 0x00, IPv4_LENGTH);
	memcpy(&stInAddr.s_addr, &source_ip, sizeof(UINT32));
	InetNtopW(AF_INET, (CONST VOID*)&stInAddr, m_tzSourceIPv4, IPv4_LENGTH);

	wmemset(m_tzTargetIPv4, 0x00, IPv4_LENGTH);
	memcpy(&stInAddr.s_addr, &target_ip, sizeof(UINT32));
	InetNtopW(AF_INET, (CONST VOID*)&stInAddr, m_tzTargetIPv4, IPv4_LENGTH);
	/* ���� �ֱٿ� �۽��� �ð� ���� */
	m_u64SendTime		= GetTickCount64();
	// ������ �� ���� ���� ��� ���� ���� �ʱ�ȭ
	m_bIsInited			= InitData();
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CMC2Thread::~CMC2Thread()
{
	// Release Client Socket
	if (INVALID_SOCKET != m_sdClient)		CloseSocket(m_sdClient);
	// Socket Event ����
	if (WSA_INVALID_EVENT != m_wsaClient)	WSACloseEvent(m_wsaClient);

	// ���� ���� �޸� ����
	if (m_pPktRecvBuff)	delete [] m_pPktRecvBuff;
	if (m_pPktRingBuff)	delete m_pPktRingBuff;
	/* �ӽ� �۽� ���� �޸� ���� */
	if (m_pstPktSend)	delete [] m_pstPktSend;
}

/*
 desc : ��� ������ �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMC2Thread::InitData()
{
	// ��� �ʱ�ȭ
	m_sdClient		= INVALID_SOCKET;
	m_wsaClient		= WSA_INVALID_EVENT;
	m_enSocket		= ENG_TPCS::en_closed;
	// initialize the member variable
	m_u64WorkTime	= GetTickCount64();

	memset(&m_stWorkTime, 0x00, sizeof(SYSTEMTIME));
	memset(&m_stLinkTime, 0x00, sizeof(SYSTEMTIME));
	/* ---------------------------------------- */
	/* ���� ���� �ʱ�ȭ (CRingBuff Ŭ���� ����) */
	/* ---------------------------------------- */
	return InitMemory();
}

/*
 desc : ��/���� ��Ŷ ���� ���� ���� �޸� �Ҵ� �� �ʱ�ȭ
 parm : None
 retn : TRUE - ���� ����, FALSE - ����
*/
BOOL CMC2Thread::InitMemory()
{
	m_pPktRingBuff	= new CRingBuff;
	if (!m_pPktRingBuff)	return FALSE;
	m_pPktRingBuff->Create(m_u32RingBuff+1);
	m_pPktRingBuff->ResetBuff();

	/* �ӽ� �۽� ������ ���� (�ִ� 32��) */
	m_pstPktSend	= new STG_MPSD [MAX_PKT_SEND_BUFF];
	memset(m_pstPktSend, 0x00, sizeof(STG_MPSD) * MAX_PKT_SEND_BUFF);
	/* ------------------------------------------------ */
	/* �ѹ� ���� ���� ��, ������ �� �ִ� ��Ŷ ���� �Ҵ� */
	/* ------------------------------------------------ */
	m_pPktRecvBuff	= new UINT8 [m_u32RecvBuff+1];
	ASSERT(m_u32RecvBuff);
	memset(m_pPktRecvBuff, 0x00, m_u32RecvBuff+1);
	// Client Socket Event ����
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
 desc : ���� �ɼ� �����ϱ� - �۽�/���� ���� ũ�� ����
 parm :	flag	- [in]  0x01:Send Buffer, 0x10:Recv Buffer, 0x11:Send/Recv Buffer
		sd		- [in]  ���� �ɼ��� ������ ���� �����
		size	- [in]  Port Buffer Size
 retn : TRUE or FALSE
*/
BOOL CMC2Thread::SetSockOptBuffSize(UINT8 flag, SOCKET sd)
{
	if (INVALID_SOCKET == sd)	return FALSE;
	INT32 i32Ret = 0, i32Size = sizeof(int);

	// �۽� �� ���� ���� ũ�� ����
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
 desc : ���� �ɼ� �����ϱ� - �۽�/���� Ÿ�� �ƿ� ����
 parm :	flag	- [in]  0x01:Send, 0x10:Recv, 0x11:Send/Recv
		sd		- [in]  ���� �ɼ��� ������ ���� �����
		delay	- [in]  �۽�/���� Ÿ�� �ƿ� �� ���� (����: �и���)
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
 desc : ���� �ɼ� �����ϱ� - ��/���� �ӵ� ���� (�����ϸ� ���ݾ� ������ ���������� �ѹ��� ���� ������(Effective TCP))
		WinSock �ǰ�� �⺻���� Nagle �˰����� ������ �Ǿ��ֽ��ϴ�. ������ ��� ��Ʈ�� ���� ���α׷�������
		��Ʈ�� �� ���۷��̳� ���Ϻ��ٴ� ���� ����ӵ��� �� �߿�� ����� ��Ȳ�� �ֽ��ϴ�.
		�׷��� ������ TCP_NODELAY  ��� �ɼ��� ����Ͽ� Nagle �˰����� ���� �� �� �ֽ��ϴ�.
 parm :	optval	- [in]  0 : Nagle �˰��� (�����ϸ� ���ݾ� ������ ������ ���� �ѹ��� ���� ������. Effective TCP)
						1 : No Delay �˰��� (��Ʈ��ũ�� ���۷����� ��Ŷ ���� �ӵ��� �߽��ϴ� ���.)
		sd		- ���� �ɼ��� ������ ���� �����
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
 desc : ���� �ð����� �ش� ������ ���� ������ ���� ���� ���¸� Ȯ���ϱ� ���� �ְ� �ޱ⸦ �����Ѵ�.
		��, ���� �ð����� ��ȣ���� ������ ���ٸ�, ���������� ��� 2�ð����� �ѹ� ���� ACK �ñ׳��� �ְ� �޴´�.
 parm :	optval	- [in]  0 : alive check ����
				  [in]  1 : alive check �����
		sd		- [in]  ���� �ɼ��� ������ ���� �����
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
 desc : ����� ���̺��� �������/���� ����
 parm :	optval	- [in]  0 : ����� ���̺��� ���
				  [in]  1 : ����� ���̺��� ������� �ʰ�, ���ε�� ��Ʈ��ũ �������̽��� ���Ͽ� �����͸� �������� ����
		sd		- [in]  ���� �ɼ��� ������ ���� �����
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

	// Shutdown �Լ� ȣ���ϱ� ���� �ݵ�� �Ʒ� �÷��׺��� ������ ��
	m_enSocket	= ENG_TPCS::en_closed;
	// ���� Client ������ ��� �ִ��� ����
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

		// ���Ͽ� �ش�Ǵ� ��Ʈ �ʱ�ȭ
		stLinger.l_onoff	= 1;				// linger on...
		stLinger.l_linger	= UINT16(time_out);	// timeout in seconds ... (0 -> hard close)
		setsockopt(sd, SOL_SOCKET, SO_LINGER, (char FAR*)&stLinger, sizeof(LINGER));
		// ���� ���� �� �ʱ�ȭ
		closesocket(sd);
	}

	// Update the communication time for TCP/IP
	UpdateWorkTime();

	/* --------------------- */
	/* ���� ������ �ð� ���� */
	/* --------------------- */
	GetLocalTime(&m_stLinkTime);
}

/*
 desc : ���� �ֱٿ� ����� �ð� ���� �� ����
 parm : None
 retn : None
*/
VOID CMC2Thread::UpdateWorkTime()
{
	m_u64WorkTime = GetTickCount64();
	GetLocalTime(&m_stWorkTime);
}

/*
 desc : Remote Server�κ��� ���� ���� �ñ׳��� �߻��� ���
 parm : sd	- [in]  Client socket descriptor
 retn : None
*/
VOID CMC2Thread::Closed(SOCKET sd)
{
	/* --------------------------------------------------------- */
	/* �ݵ�� ���� �Լ� ȣ�� ��, ���� �ڵ� ���� �Լ� ȣ���ؾ� �� */
	/* --------------------------------------------------------- */

	// ���� ���� ���� �۾� ����
	CloseSocket(sd);
}

/*
 desc : ���� ��Ŷ ������ �۽ź�
 parm : packet	- [in]  �۽� ��Ŷ�� ����� ����
		size	- [out] �Է�: 'packet' ���ۿ� ����� ��Ŷ ũ��
						��ȯ: ���۵� �������� ũ��
 retn : TRUE or FALSE (��� �̻��� �߻��� ���)
*/
BOOL CMC2Thread::Sended(PUINT8 packet, UINT32 &size)
{
	BOOL bSucc			= TRUE;
	CHAR *pPktSend		= (CHAR*)packet;
	INT32 i32SendBytes	= 0, i32SendSize = INT32(size);
	UINT32 u32TotalSize	= 0, u32TickTime = (UINT32)GetTickCount();

	/* �۽� ���� ������ �۽� ���� ���� */
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
			// ���� �� ����
			m_i32SockError = WSAGetLastError();
			// ���� ���� ��尡 �ƴϴٶ��... ����� ����� �ȴ�.
			if (m_i32SockError != WSAEWOULDBLOCK &&
				m_i32SockError != WSA_IO_PENDING)
			{
				CloseSocket(m_sdClient);
				return FALSE;
			}
			/* ------------------------------------------------ */
			/* �ϴ� ������ �߻�������, ������ ������ ���������� */
			/* 1�������� ���� ���� ��, ������ ������ ���߱⶧�� */
			/* ���� ���� ������ �۾��� ���� ������ ���� ������  */
			/* ------------------------------------------------ */
			break;
		}
		else
		{
			// ������� ���۵� ������ ũ�� ����
			u32TotalSize+= i32SendBytes;
			// ������ ���۵� ������ ũ�� ����
			i32SendSize	-= i32SendBytes;
			// ������ ������ ��ġ�� �̵�
			pPktSend	+= i32SendBytes;
		}

		/* ���� ������ ��� ������ ���� Ȯ�� */
		if (((UINT32)GetTickCount() - u32TickTime) > TCP_SEND_TOTAL_TIMEOUT)
		{
			bSucc	= FALSE;
			break;
		}

	}	while (i32SendSize > 0);

	/* ----------------------------------------------- */
	/* ��Ŷ �����͸� ���� ���� ������ ������ �ð� ���� */
	/* ----------------------------------------------- */
	if (u32TotalSize == size)
	{
		UpdateWorkTime();
		m_enSocket = ENG_TPCS::en_sended;
	}
	// �۽� ������ ���
	else	bSucc = FALSE;

	/* �۽� �ð� ���� */
	if (bSucc)	m_u64SendTime	= GetTickCount64();

	/* ----------------------- */
	/* ��Ŷ�� ������ ũ�� ��ȯ */
	/* ----------------------- */
	size = u32TotalSize;

	return bSucc;
}

/*
 desc : ��Ŷ�� ������ ��, ������ Ƚ�� ����
 parm : packet	- [in]  �۽� ��Ŷ�� ����� ����
		size	- [in]  'packet' ���ۿ� ����� ��Ŷ ũ��
		retry	- [in]  ������ �õ� Ƚ�� (������ 0 ���� Ŀ�� ��)
		wait	- [in]  ������ ���� �ð� (����: �и���)
 retn : TRUE or FALSE (��� �̻��� �߻��� ���)
*/
BOOL CMC2Thread::ReSended(PUINT8 packet, UINT32 size, UINT32 retry, UINT32 wait)
{
	PUINT8 pPacket	= packet;
	UINT32 u32Size	= size, u32Total = 0, u32Retry = retry;

	do {

		// ���� ������ ���� ��� ������ ���ɼ��� ŭ!!!
		if (!Sended(pPacket, u32Size))	return FALSE;

		/* ------------------------------------- */
		/* �Ϻ� Ȥ�� ��ü ��Ŷ�� �����ߴ��� Ȯ�� */
		/* ------------------------------------- */
		// ������� ������ ������ ũ�� ����
		u32Total += u32Size;
		// ��� �����Ͱ� ���۵� ������� ����
		if (u32Total == size)	return TRUE;

		/* ------------------------------------- */
		/* ��-������ ���� ������ ���� ��ġ ����  */
		/* ------------------------------------- */
		pPacket += u32Size;

		/* ------------------------------------- */
		/* �Ϻ� �����͸� ���۵� ��� ������ ��Ŷ */
		/* �������� ��-������ ���ؼ� ũ�� ���� */
		/* ------------------------------------- */
		u32Size = size - u32Total;

		// ���� �ð� ���� (����: �и���)
		::Sleep(wait);

		/* ��� �ݺ� �۾��� ���� ������ ���� ������ */
		if (u32Retry < 1)	break;
		u32Retry--;

	} while (1);

	return FALSE;
}

/*
 desc : Remote Client�κ��� ������ ���� �ñ׳��� �߻��� ���
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
		/* ���� ���ŵ� �����͸� ������ �� �ִ� ���� ������ ũ�� ���   */
		/* �� �̻� ���� ���� ���۰� ���� ���� ���� ó������ �ʴ´�. */
		/* ---------------------------------------------------------- */
		u32SpareBytes	= m_pPktRingBuff->GetWriteSize();
		if (u32SpareBytes <= 0)
			break;	// �� �̻� ���Ź��� ���۰� �����Ƿ�, �ϴ� ���۸� ���� �۾��� �����ϵ��� ó�� �Ѵ�

		/* ---------------------------------------------------------------- */
		/* ���� �ִ� ���� ���� ������ ũ�⸸ŭ ������ ���� �۾��� ���� �Ѵ� */
		/* ---------------------------------------------------------------- */
		// ������ �� �ִ� ���� ũ�� ��ŭ �е�, �ӽ� ���� ���� ũ�⿡ ����, ���� ������ ũ�� ����
		if (u32SpareBytes > m_u32RecvBuff)	i32RecvSize	= (INT32)m_u32RecvBuff;
		else								i32RecvSize	= (INT32)u32SpareBytes;
		i32ReadBytes = recvfrom(sd,
								(CHAR *)m_pPktRecvBuff,
								i32RecvSize,
								0,
								(struct sockaddr *)&stSrvAddr, &i32AddrSize);
 		// ���� ������ �߻��� ���
 		if (i32ReadBytes == SOCKET_ERROR)
 		{
 			m_i32SockError = WSAGetLastError();
 			// IO Pending ������ �ƴϰ�, ���� �����̸� ��� ������ ���� �Ѵ�.
 			if (m_i32SockError != WSA_IO_PENDING && m_i32SockError != WSAEWOULDBLOCK)
 			{
				CloseSocket(m_sdClient);
				return FALSE;
 			}
			// TCP ��� �ð� ����
			UpdateWorkTime();

			// ���ŵǾ��ٰ� ��� �� ����
			m_enSocket	= ENG_TPCS::en_recved;
			// �̵����� �ƴϸ�, ���� ���� �ٽ� �е��� ��ȯ �Ѵ�
			return TRUE;
 		}

		/* ----------------------------------------------------------- */
		/* ���� �����κ��� ���ŵ� ���������� or �ƴ����� �Ǵ��ϱ� ���� */
		/* ----------------------------------------------------------- */
		if (0 == memcmp(&stSrvAddr, &m_stSrvAddr, sizeof(stSrvAddr)))
		{
			/* �ϴ� ���ŵ� �����͸� Ring ���� ������ ���� */
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
		/* �ϴ� ���ŵ� �����͸� Ring ���� ������ ���� */
		/* ------------------------------------------ */
		m_pPktRingBuff->WriteBinary(m_pPktRecvBuff, i32ReadBytes);
		// TCP ��� �ð� ����
		UpdateWorkTime();

		/* ��û�� ������ ũ�⺸�� ���ŵ� ������ ũ�Ⱑ ������ ��ƾ ���� ������ */
		u32SpareBytes	= m_pPktRingBuff->GetWriteSize();
		if (i32ReadBytes < i32RecvSize || u32SpareBytes < 1)	break;

		/* ���� ������ ��� ������ ���� Ȯ�� */
		if (((UINT32)GetTickCount() - u32TickTime) > TCP_RECV_TOTAL_TIMEOUT)
		{
			bSucc	= FALSE;
			break;
		}

	} while (1);
 	
	/* ------------------------------------------------------------- */
	/* ������ �м� ������ ���, ���� ��� ��Ʈ�� ���� ���� ��� ���� */
	/* ------------------------------------------------------------- */
	return (bSucc && RecvData());
}

/*
 desc : ���� ��� ��Ʈ�� ���� ���ۿ� ���� �ִ� ��� �����͸� �о ������.
		�� �̻� �о���� �����Ͱ� ���� ������ ...
 parm : sd	- [in]  ��� �����͸� �о���̰��� �ϴ� ���� �ڵ�
 retn : �� �о ���� ���� ����Ʈ ũ�� (-1 ���̸� ���� �о���� �����Ͱ� ����?)
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

			// �ϴ� ������ 1024 Bytes�� �о���̱�
			i32RecvBytes = recvfrom(sd,
									(CHAR *)szBuff,
									1024,
									0,
									(struct sockaddr *)&stSrvAddr, &i32AddrSize);

			i32RecvTotal	+= i32RecvBytes;

			/* ���� ������ ��� ������ ���� Ȯ�� - �ִ� 2 �� ���ȸ� ��� �б� ���� */
			if (((UINT32)GetTickCount() - u32TickTime) > TCP_RECV_TOTAL_TIMEOUT)	break;

		}	while (i32RecvBytes == 1024);
	}

	// TCP/IP ��� �۾� �ð� ����
	UpdateWorkTime();

	/* --------------------- */
	/* ���� ���� ũ�� �ʱ�ȭ */
	/* --------------------- */
	m_pPktRingBuff->ResetBuff();

	/* ������ ��Ŷ �۽� ������ �ð����� �ֱⰡ �ʹ� ª���� �θ𿡰� �˸��� ���� */
	LOG_WARN(m_enSysID, L"All currently received buffers have been discarded");

	return i32RecvTotal;
}

/*
 desc : ��뷮 ���� ���ۿ� �� �̻� ���ŵ� ��Ŷ�� ������ Ȯ��
 parm : None
 retn : TRUE - ���ŵ� ���ۿ� �����ִ� �����Ͱ� �ִ�, FALSE - ���ŵ� �����Ͱ� ����
*/
BOOL CMC2Thread::IsEmptyPktBuff()
{
	return (m_pPktRingBuff && m_pPktRingBuff->GetReadSize() < 1) ? TRUE : FALSE;
}

/*
 desc : Alive Check�� ���� Ƚ�� �̻� ������ ���, ������ ���� �۾� ����
 parm : None
 retn : None
*/
VOID CMC2Thread::SetForceClose()
{
	CloseSocket(m_sdClient);
	m_sdClient	= INVALID_SOCKET;
}

/*
 desc : �����Ͱ� ���ŵ� ���, ȣ���
 parm : None
 retn : TRUE - ���������� �м� (Ring Buffer) ó��
		FALSE- �м� ó������ ���� (���� �Ϻ����� ���� ��Ŷ��)
*/
BOOL CMC2Thread::RecvData()
{
	TCHAR tzMesg[128]	= {NULL};
	UINT8 u8PktData[512]= {NULL}, *pPktRead	= u8PktData;
	UINT32 u32PktRead	= 0;
	STG_MPRD stData		= {NULL};			/* �ݹ� �Լ��� ��ȯ�� ����ü */
	STG_PUCH stUdpHead	= {NULL};

	do	{

		/* return_port (2) + udp_msg_type (1) + short_cycle (1) + reserved (2) + cmd_cnt (4) */;
		u32PktRead	= sizeof(STG_PUCH);
		/* �⺻ ���� ��� ũ�⺸�� ���� ���ŵ� ���¶��, �� ���ŵ� ������ ��� */
		if (m_pPktRingBuff->GetReadSize() < u32PktRead)	return TRUE;
		/* ���� ���ŵ� ������ ���� ��ġ ���, ��Ŷ ��� ����ü �����Ϳ� ���� ���� */
		m_pPktRingBuff->CopyBinary((PUINT8)&stUdpHead, u32PktRead);
		/* �ݵ�� ���⼭ �ʱ�ȭ ����� �� */
		memset(&stData, 0x00, sizeof(STG_MPRD));
		/* ���ŵ� �������� �޽��� ������ �������� �������� Ȯ�� */
		if ((UINT8)ENG_MUMT::en_cmd_nack == stUdpHead.udp_msg_type)
		{
			/* ���� ������ �κб��� ������. */
			m_pPktRingBuff->PopBinary(u32PktRead);
			/* ������ ��Ŷ �۽� ������ �ð����� �ֱⰡ �ʹ� ª���� �θ𿡰� �˸��� ���� */
			LOG_WARN(m_enSysID, L"The command buffer is full");
		}
		else if ((UINT8)ENG_MUMT::en_gen3_dnc == stUdpHead.udp_msg_type) /* e_mumt_gen3_dnc */
		{
			/* ��Ŷ�� �� ���ۿ� �� ��� �ִ� Ȯ�� (dnc_cmd (1) + dnc_err (1)) */
			if (m_pPktRingBuff->GetReadSize() < (u32PktRead + 2))	return TRUE;
			/* �ϴ� �ӽ� ���۷� ���� ���� */
			m_pPktRingBuff->CopyBinary(u8PktData, u32PktRead + 2);
			/* dnc_cmd (E_Gen3DncCmds) & dnc_error ��ġ�� �̵� �� �� ���� */
			pPktRead		= u8PktData + u32PktRead;		/* error (status) ���� */
			stData.dnc_cmd	= pPktRead[0];
			stData.dnc_err	= pPktRead[1];
			/* ������ �о���� ������ ��ġ ���� */
			u32PktRead		+= 2;
			/* ���ŵ� ��Ŷ ������ ������ �ִ��� ���ο� ���� */
			if (0xff == stData.dnc_cmd)
			{
				TCHAR tzG3Error[96]	= {NULL};
				/* ���� ������ �κб��� ������. */
				m_pPktRingBuff->PopBinary(u32PktRead);
				/* ���� �޽��� ��� */
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
				/* ���� ������ ���� �Ǿ��ٸ�, ���� �ڵ������ ���� �� ���� ���� ������ �е��� ��ġ */
				switch (stData.dnc_cmd)
				{
				/* �б� ��û�� ���� ������ ���ŵ� ��� */
				case ENG_DCG3::en_res_read_obj	:
					/* ��Ŷ�� �� ���ۿ� �� ��� �ִ� Ȯ�� (data_len (2) + next_sub_index (4)) */
					if (m_pPktRingBuff->GetReadSize() < (u32PktRead + 6))	return TRUE;
					/* �ϴ� �ӽ� ���۷� ���� ���� */
					m_pPktRingBuff->CopyBinary(u8PktData, u32PktRead + 6);
					/* data_len (2) + next_sub_index (4) ���� */
					pPktRead	= u8PktData + u32PktRead;		/* error (status) ���� */
					memcpy(&stData.data_len,	pPktRead, 2);	pPktRead	+= 2;
					memcpy(&stData.sub_index,	pPktRead, 4);
					/* ������ �о���� ������ ��ġ ���� */
					u32PktRead	+= 6;
					/* ------------------------------------------------------- */
					/* Sub Index ���� 0 �� �ƴ� ��� ��� ó���� �ؾ� �ϴ��� */
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
						/* ��Ŷ�� �� ���ۿ� �� ��� �ִ� Ȯ�� (data_len (size)) */
						if (m_pPktRingBuff->GetReadSize() < (u32PktRead + stData.data_len))	return TRUE;
						/* �ϴ� �ӽ� ���۷� ���� ���� */
						m_pPktRingBuff->CopyBinary(u8PktData, u32PktRead + stData.data_len);
						/* Ư�� �����ͺ��� ũ�ų� ���̰� 0 ���� Ȯ�� */
						if (stData.data_len && 0 == stData.sub_index)
						{
							/* data_len ��ŭ �޸� �Ҵ� �� data ���� */
							stData.data[stData.data_len]	= 0x00;
							memcpy(stData.data,	u8PktData+u32PktRead, stData.data_len);
						}
						/* ������ �о���� ������ ��ġ ���� */
						u32PktRead	+= stData.data_len;
					}
					break;

				/* ���� ��û�� ���� ������ ���ŵ� ��� */
				case ENG_DCG3::en_res_write_obj	:
					/* ��Ŷ�� �� ���ۿ� �� ��� �ִ� Ȯ�� (next_sub_index (4)) */
					if (m_pPktRingBuff->GetReadSize() < (u32PktRead + 4))	return TRUE;
					/* �ϴ� �ӽ� ���۷� ���� ���� */
					m_pPktRingBuff->CopyBinary(u8PktData, u32PktRead + 4);
					/* Sub Index ���� */
					memcpy(&stData.sub_index, u8PktData+u32PktRead, 4);
					/* ������ �о���� ������ ��ġ ���� */
					u32PktRead	+= 4;
					/* ------------------------------------------------------- */
					/* Sub Index ���� 0 �� �ƴ� ��� ��� ó���� �ؾ� �ϴ��� */
					/* ------------------------------------------------------- */
#if 0	/* ������ ������ �ƴ�����... ��ü������ ���� �ǹ����� ���� �Ұ� */
					if (stData.sub_index != 0)
					{
						swprintf_s(tzMesg, 128, L"Incomprehensible : <recv> <write> sub_index = %4d",
								   stData.sub_index);
						LOG_ERROR(m_enSysID, tzMesg);
					}
#endif
					break;

				/* ��Ÿ ��ɾ��� ��� */
				default	:
					/* ���� �������� ���� ��Ŷ�̶�� ���� ��, ��� ������ ���� ���� ũ�� ���� */
					stData.undefined	= 0x01;
					u32PktRead			= m_pPktRingBuff->GetReadSize();
					/* ���� �޽��� ��� */
					swprintf_s(tzMesg, 128,
							   L"Undefined Message : msg_type(%d) / dnc_cmd(%d) / dnc_err(%d) / drv_id(%d)",
							   stData.msg_type, stData.dnc_cmd, stData.dnc_err, stData.sub_id);
					LOG_ERROR(m_enSysID, tzMesg);
					break;
				}
				/* ���ݱ��� �о���� ��Ŷ �����͸� ���ۿ��� ���� */
				m_pPktRingBuff->PopBinary(u32PktRead);
			}
		}
		/* �̻��� �����Ͱ� ���ŵ� (���� ���ŵ� ���� ��� ���) */
		else
		{
			m_pPktRingBuff->ResetBuff();
			break;	/* �׳� ���� ���� ó���� ���� ���� ó�� */
		}

		/* ���� ���ŵ� ��Ŷ�� ����� ���� ������ ���� ���� �۽��� ��Ŷ ���� �˻� */
		if (0 == stData.undefined && 0 == stData.sub_index &&	/* Sub Index ���� 0�� �ƴ� ������� */
			stData.data_len > 0 && stUdpHead.send_cmd_cnt < MAX_PKT_SEND_BUFF &&
			m_pstPktSend[stUdpHead.send_cmd_cnt].cmd)
		{
			/* ���� �۽�(��û)�� ��ɾ� ���� ���� */
			stData.msg_type		= stUdpHead.udp_msg_type;
			stData.dnc_cmd		= stData.dnc_cmd & 0x0f;	/* stPktSend.cmd; */
			stData.sub_id		= m_pstPktSend[stUdpHead.send_cmd_cnt].sub_id;
			stData.obj_index	= m_pstPktSend[stUdpHead.send_cmd_cnt].obj_index;
			stData.sub_index	= m_pstPktSend[stUdpHead.send_cmd_cnt].sub_index;
			/* ���ŵ� ��Ŷ ���� �۽� */
			SetRecvPacket(ENG_TPCS::en_recved, &stData);
		}

	}	while (1);

	return TRUE;
}

/*
 desc : ���̺귯�� ������ ó��
 parm : type	- [in]  �������� ���� (ENG_TPCS) (Sended or Received)
		data	- [in]  �����Ͱ� ����� ���� ������
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
	/* ���� ������ ó�� (������ ���� ��츸 ó��) (Write or Read�� ���� ���丸 ó��) */
	if (0 == data->dnc_err &&
		((UINT8)ENG_DCG3::en_req_read_obj == data->dnc_cmd ||
		 (UINT8)ENG_DCG3::en_req_write_obj == data->dnc_cmd))
	{
		/* ���� �ֱٿ� ���ŵ� ��Ŷ ������ ���� */
		memcpy(&m_pstShMemMC2->last_recv, data, sizeof(STG_MPRD));
		/* ���� �ֱٿ� ���ŵ� PDO Object ���� */
		SetRecvData(data->obj_index, data->data, data->data_len);
	}
	else
	{
		if (0 != data->dnc_err)
		{
			TCHAR tzMesg[128]	= {NULL};
			/* ���� DNC ������ ����� ... */
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

	/* ��� ����̺꿡 ���� ���� ���� �����ϰ�, ���� �ֱٿ� �߻��� ���� ���� ���Ͽ� �ٸ��� ���� �۾� ���� */
	AnalysisError();
}

/*
 desc : ���Ź��� ������ ó��
 parm : obj_id	- [in]  Object Index (Reference or Actual)
		data	- [in]  ���ŵ� �����Ͱ� ����� ���� ������
		size	- [in]  ������ ���ۿ� ����� �������� ũ��
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
 desc : ���� ����̺꿡 ������ �߻� �ߴ��� ���ο� ���� �α� ���Ͽ� ���� ����
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
				/* ���� �ֱٿ� �߻��� ���� �� ���� */	
				m_u16DrvError[i] = m_pstShMemMC2->act_data[i].error;
				/* Error�� �߻��ߴٰ� ���� */
				if (m_u16DrvError[i])	bError = TRUE;
			}
		}

	}
	/* ������ �߻� �ߴٸ� �α� ���Ͽ� ��� */
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
 desc : ���� �߻��� ���� ��� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CMC2Thread::ResetError()
{
	m_u8DncError	= 0x00;
	memset(m_u16DrvError, 0x00, sizeof(UINT16) * MAX_MC2_DRIVE);
}