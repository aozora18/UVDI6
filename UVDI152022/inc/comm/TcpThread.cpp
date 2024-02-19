
/* desc : ���� �⺻ Ŭ���� */

#include "pch.h"
#include "TcpThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : sys_id		- [in]  System ID
		th_id		- [in]  Thread ID
		source_ip	- [in]  Local IPv4
		target_ip	- [in]  Remote IPv4
		tcp_port	- [in]  TCP/IP Port
		port_size	- [in]  TCP/IP Port Buffer Size
		recv_size	- [in]  Send or Recv�� ��Ŷ �� ���� ū ��Ŷ�� ũ�� (����: Bytes)
		ring_size	- [in]	Max Recv Packet Size
		wait_time	- [in]  �⺻ 30 �и��� ���� ���� �̺�Ʈ ������ ���� ���
		recv_count	- [in]  ���������� ���ŵǴ� �����͸� ������ �ּ� ���� ���� �Ҵ�
 retn : None
*/
CTcpThread::CTcpThread(ENG_EDIC sys_id,
					   UINT16 th_id,
					   UINT16 tcp_port,
					   UINT32 source_ip,
					   UINT32 target_ip,
					   UINT32 port_size,
					   UINT32 recv_size,	// Packet ������ �ִ� ũ�� (����: Bytes)
					   UINT32 ring_size,	// Ring Buffer ũ�� (����: Bytes)
					   UINT32 sock_time,
					   UINT32 recv_count)
{
	IN_ADDR stInAddr	= {NULL};

	// ���� or ���� �ȵǾ��ٰ� ����
	m_bIsLinked			= FALSE;
	m_bIsStopSend		= FALSE;
	m_enSysID			= sys_id;
	m_u16ThreadID		= th_id;
	m_u16TcpPort		= tcp_port;
	m_u32PortBuff		= port_size;
	m_u32RecvBuff		= recv_size;
	if (ring_size > recv_size)	m_u32RingBuff	= ring_size;
	else						m_u32RingBuff	= recv_size * 3;	/* ���� 2 �� �̻�. ���������� 3�� �̻� */
	/* �ּ� ��ٸ��� �ð��� 0 ���̸� �⺻ ������ 10�� �־��ָ� �˴ϴ�.*/
	m_u32SockTime		= sock_time < 10 ? 10 : sock_time;	// �⺻ 10 �и��� ���� ���� �̺�Ʈ ������ ���� ���
	/* ----------------------------------------------------------------------------- */
	/* Server or Client �ʿ��� Listen or Client Socket�� �ʱ�ȭ�� ������ �ð��� ���� */
	/* �ʱ⿡�� �ٷ� Listen or Connect�� �����ؾ� �ϱ� ������ 0 ������ �ʱ�ȭ�� ���� */
	/* ----------------------------------------------------------------------------- */
	m_u64ConnectTime	= 0;
	/* ----------------------------------------------------------------------------- */
	/* ������ 0 ������ �ʱ�ȭ �ǳ�, Ư�� ���κ��� ���� �̺�Ʈ�� �߻��� ���, ������*/
	/* ���� �۾� ��, Socket�� recv �Լ� ȣ���� ���� �Ⱓ �� Ƚ���� �ݺ������� ������ */
	/* �� �� �̻� ���ŵ� �����Ͱ� ������ Ȯ���� ��, �б� �۾��� �����߸� �ϴ� ��찡 */
	/* ������, �̷� ���, �Ʒ� �� ���� ������ �б� �ݺ� Ƚ���� �ݺ� �б� ������ ��� */
	/* �ϴ� �ð� (����: msec) ���� ������ ��� �Ѵ�. �� �κ��� �� �ʿ��� ���� ����� */
	/* ----------------------------------------------------------------------------- */
	m_u32RecvRepeat		= 0;	/* ���� �� ���� 3�� ���, Socket�� recv �Լ��� 3�� �ݺ� ȣ�� */
	m_u32RecvSleep		= 0;	/* ���� �� ���� 50�� ���, Socket�� recv �Լ� ȣ�� ������ 50 msec ����� */
	m_i32SockError		= 0;	/* ���� ���� �ʱ�ȭ */

	/* IP �ּ� ��ȯ: DWORD -> String ��ȯ */
	wmemset(m_tzSourceIPv4, 0x00, IPv4_LENGTH);
	memcpy(&stInAddr.s_addr, &source_ip, sizeof(UINT32));
	InetNtopW(AF_INET, (CONST VOID*)&stInAddr, m_tzSourceIPv4, IPv4_LENGTH);

	wmemset(m_tzTargetIPv4, 0x00, IPv4_LENGTH);
	memcpy(&stInAddr.s_addr, &target_ip, sizeof(UINT32));
	InetNtopW(AF_INET, (CONST VOID*)&stInAddr, m_tzTargetIPv4, IPv4_LENGTH);

	/* TCP/IP ���� ���� */
	swprintf_s(m_tzTCPInfo, 128, L"<src:%s> <tgt:%s> <port:%d> <th_id:%d>",
			   m_tzSourceIPv4, m_tzTargetIPv4, m_u16TcpPort, m_u16ThreadID);

	/* ���ŵ� ��Ŷ �����͸� �ӽ÷� ������ ���� */
	m_pRecvQue	= new CPktQue(recv_count);
	ASSERT(m_pRecvQue);
	/* ������ �� ���� ���� ��� ���� ���� �ʱ�ȭ */
	m_bIsInited	= InitData();
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CTcpThread::~CTcpThread()
{
	/* Release Client Socket */
	if (INVALID_SOCKET != m_sdClient)		CloseSocket(m_sdClient);
	/* Socket Event ���� */
	if (WSA_INVALID_EVENT != m_wsaClient)	WSACloseEvent(m_wsaClient);

	// ���� ���� �޸� ����
	if (m_pPktRecvBuff)	delete [] m_pPktRecvBuff;
	if (m_pPktRingBuff)	delete m_pPktRingBuff;
	// �ӽ� ��Ŷ ���� �޸� ����
	delete m_pRecvQue;
}

/*
 desc : ��� ������ �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CTcpThread::InitData()
{
	/* ��� �ʱ�ȭ */
	m_sdClient	= INVALID_SOCKET;
	m_wsaClient	= WSA_INVALID_EVENT;
	m_enSocket	= ENG_TPCS::en_closed;

	/* initialize the member variable */
	m_u64WorkTime	= 0;
	memset(&m_stWorkTime, 0x00, sizeof(SYSTEMTIME));
	memset(&m_stLinkTime, 0x00, sizeof(SYSTEMTIME));

	/* ���� ���� �ʱ�ȭ (CRingBuff Ŭ���� ����) */
	if (!InitMemory())	return TRUE;

	return TRUE;
}

/*
 desc : ��/���� ��Ŷ ���� ���� ���� �޸� �Ҵ� �� �ʱ�ȭ
 parm : None
 retn : TRUE - ���� ����, FALSE - ����
*/
BOOL CTcpThread::InitMemory()
{
	m_pPktRingBuff	= new CRingBuff;
	ASSERT(m_pPktRingBuff);
	m_pPktRingBuff->Create(m_u32RingBuff+1);
	m_pPktRingBuff->ResetBuff();

	/* �ѹ� ���� ���� ��, ������ �� �ִ� ��Ŷ ���� �Ҵ� */
	m_pPktRecvBuff	= new UINT8 [m_u32RecvBuff+1];
	ASSERT(m_u32RecvBuff);
	memset(m_pPktRecvBuff, 0x00, m_u32RecvBuff+1);

	/* Client Socket Event ���� */
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
 desc : ���� ���� ���� ���� ����
 parm : mesg	- [in]  ���� �޽���
 retn : None
*/
VOID CTcpThread::SocketError(PTCHAR mesg)
{
	UINT32 u32Len	= 0;
	PTCHAR ptzMesg	= {NULL};
	
	if (!mesg)	return;
	u32Len	= (UINT32)_tcslen(mesg);
	u32Len	= (UINT32)uvCmn_GetMultiOf8(u32Len) + 48 /* Socket TCP/IP Setup Error */;

	/* �ӽ� �޸� �Ҵ� */
	ptzMesg	= (PTCHAR)::Alloc(u32Len * sizeof(TCHAR));
	memset(ptzMesg, 0x00, u32Len);

	/* ���� �޽��� ���� */
	m_i32SockError	= WSAGetLastError();
	swprintf_s(ptzMesg, u32Len, L"Socket TCP/IP Setup Error (%s) (code:0x%08x)",
			   mesg, m_i32SockError);

	/* ������ ��Ŷ �۽� ������ �ð����� �ֱⰡ �ʹ� ª���� �θ𿡰� �˸��� ���� */
	if (!IsRunDemo())	LOG_ERROR(ENG_EDIC::en_engine, ptzMesg);

	/* �޸� ���� */
	if (ptzMesg)	::Free(ptzMesg);
}

/*
 desc : ���� �ɼ� �����ϱ� - �۽�/���� ���� ũ�� ����
 parm :	flag	- [in]  0x01:Send Buffer, 0x10:Recv Buffer, 0x11:Send/Recv Buffer
		sd		- [in]  ���� �ɼ��� ������ ���� �����
		size	- [in]  Port Buffer Size
 retn : TRUE or FALSE
*/
BOOL CTcpThread::SetSockOptBuffSize(UINT8 flag, SOCKET sd)
{

	if (INVALID_SOCKET == sd)	return FALSE;
	INT32 i32Ret = 0, i32Size = sizeof(int);

	/* �۽� �� ���� ���� ũ�� ���� */
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
 desc : ���� �ɼ� �����ϱ� - �۽�/���� Ÿ�� �ƿ� ����
 parm :	flag	- [in]  0x01:Send, 0x10:Recv, 0x11:Send/Recv
		sd		- [in]  ���� �ɼ��� ������ ���� �����
		delay	- [in]  �۽�/���� Ÿ�� �ƿ� �� ���� (����: �и���)
 retn : TRUE or FALSE
*/
BOOL CTcpThread::SetSockOptTimeout(UINT8 flag, SOCKET sd, INT32 time)
{
	if (INVALID_SOCKET == sd)	return FALSE;
	INT32 i32Ret = 0, i32Size = sizeof(int);

	/* ���� �۽�/���� Ÿ�Ӿƿ� �� ���� */
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
 desc : ���� �ɼ� �����ϱ� - ��/���� �ӵ� ���� (�����ϸ� ���ݾ� ������ ���������� �ѹ��� ���� ������(Effective TCP))
		WinSock �ǰ�� �⺻���� Nagle �˰����� ������ �Ǿ��ֽ��ϴ�. ������ ��� ��Ʈ�� ���� ���α׷�������
		��Ʈ�� �� ���۷��̳� ���Ϻ��ٴ� ���� ����ӵ��� �� �߿�� ����� ��Ȳ�� �ֽ��ϴ�.
		�׷��� ������ TCP_NODELAY  ��� �ɼ��� ����Ͽ� Nagle �˰����� ���� �� �� �ֽ��ϴ�.
 parm :	optval	- [in]  0 : Nagle �˰��� (�����ϸ� ���ݾ� ������ ������ ���� �ѹ��� ���� ������. Effective TCP)
						1 : No Delay �˰��� (��Ʈ��ũ�� ���۷����� ��Ŷ ���� �ӵ��� �߽��ϴ� ���.)
		sd		- [in]  ���� �ɼ��� ������ ���� �����
 retn : TRUE or FALSE
*/
BOOL CTcpThread::SetSockOptSpeed(INT32 optval, SOCKET sd)
{
	/* ���� ������ �ӵ� ���� */
	if (SOCKET_ERROR == setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (CHAR *)&optval, sizeof(optval)))
	{
		SocketError(L"Socket Option : Speed");
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
BOOL CTcpThread::SetSockOptAlive(INT32 optval, SOCKET sd)
{
	/* ���� ������ �ӵ� ���� */
	if (SOCKET_ERROR == setsockopt(sd, SOL_SOCKET, SO_KEEPALIVE, (CHAR *)&optval, sizeof(optval)))
	{
		SocketError(L"Socket Option : Alive check");
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
BOOL CTcpThread::SetSockOptRoute(INT32 optval, SOCKET sd)
{
	/* ���� ������ �ӵ� ���� */
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

	/* Shutdown �Լ� ȣ���ϱ� ���� �ݵ�� �Ʒ� �÷��׺��� ������ �� */
	m_enSocket	= ENG_TPCS::en_closed;
	/* ���� �����Ǿ��ٰ� �÷��� ���� */
	m_bIsLinked	= FALSE;
	/* ���� Client ������ ��� �ִ��� ���� */
	if (INVALID_SOCKET != sd)
	{
		TCHAR tzMesg[LOG_MESG_SIZE]	= {NULL};
		CUniToChar csCnv;
		/* ���� ������ ���� �˸� */
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

		/* ���Ͽ� �ش�Ǵ� ��Ʈ �ʱ�ȭ */
		stLinger.l_onoff	= 1;				/* linger on... */
		stLinger.l_linger	= UINT16(time_out);	/* timeout in seconds ... (0 -> hard close) */
		setsockopt(sd, SOL_SOCKET, SO_LINGER, (char FAR*)&stLinger, sizeof(LINGER));

		/* ���� ���� �� �ʱ�ȭ */
		i32Code	= closesocket(sd);
	}

	/* Update the communication time for TCP/IP */
	UpdateWorkTime();

	/* ���� ������ �ð� ���� */
	GetLocalTime(&m_stLinkTime);

	/* ���� ���� �ʱ�ȭ */
	m_i32SockError	= 0;
	/* Socket ������ ���� �ʱ�ȭ ��� �ٸ��� ó�� */
	m_sdClient	= INVALID_SOCKET;
}

/*
 desc : ���� �ֱٿ� ����� �ð� ���� �� ����
 parm : None
 retn : None
*/
VOID CTcpThread::UpdateWorkTime()
{
	m_u64WorkTime = GetTickCount64();
	GetLocalTime(&m_stWorkTime);
}

/*
 desc : ���� ��Ŷ ������ �۽ź�
 parm : packet	- [in]  �۽� ��Ŷ�� ����� ����
		size	- [out] �Է�: 'packet' ���ۿ� ����� ��Ŷ ũ��
						��ȯ: ���۵� �������� ũ��
 retn : TRUE or FALSE (��� �̻��� �߻��� ���)
*/
BOOL CTcpThread::Sended(PUINT8 packet, UINT32 &size)
{
	BOOL bSucc			= TRUE, bRetrySend	= FALSE;
	CHAR *pPktSend		= (CHAR*)packet;
	INT32 i32SendBytes	= 0, i32SendSize = INT32(size), i32FirstSize = i32SendSize;
	UINT32 u32TotalSize	= 0, u32TickTime = GetTickCount();	/* �۽��� ���Ѵ�� �� ���, ���� ���������� ���� */

	/* �۽� ���� ������ �۽� ���� ���� */
	m_enSocket	= ENG_TPCS::en_sending;

	do	{

		/* ó�� �������� �ϴ� ��Ŷ ũ�⺸�� ũ�� ���� �� (��� ���� ������ �߻��ϴ� ���, ��Ȥ ����) */
		if (i32FirstSize < i32SendSize)
		{
			m_enSocket	= ENG_TPCS::en_send_failed;
			return FALSE;
		}

		/* ���� �־��� ũ�⸸ŭ ������ �۽� */
		i32SendBytes = send(m_sdClient, pPktSend, i32SendSize, 0);
		if (SOCKET_ERROR == i32SendBytes)
		{
			/* ���� �� ���� */
			m_i32SockError = WSAGetLastError();
			/* ���� ���� ��尡 �ƴϴٶ��... ����� ����� �ȴ�. */
			if (m_i32SockError != WSAEWOULDBLOCK && m_i32SockError != WSA_IO_PENDING)
			{
				m_i32SockError = WSAGetLastError();
				bSucc	= FALSE;
				return FALSE;
			}

			/* ------------------------------------------------ */
			/* �ϴ� ������ �߻�������, ������ ������ ���������� */
			/* 1�������� ���� ���� ��, ������ ������ ���߱⶧�� */
			/* ���� ���� ������ �۾��� ���� ������ ���� ������  */
			/* ------------------------------------------------ */
			bRetrySend	= TRUE;	/* !!! Important !!! */
			break;
		}
		else
		{
			/* ������� ���۵� ������ ũ�� ���� */
			u32TotalSize+= i32SendBytes;
			/* ������ ���۵� ������ ũ�� ���� */
			i32SendSize	-= i32SendBytes;
			/* ������ ������ ��ġ�� �̵� */
			pPktSend	+= i32SendBytes;
		}

		/* ���� ������ ��� ������ ���� Ȯ�� */
#ifdef _DEBUG
		if (((UINT32)GetTickCount() - u32TickTime) > UINT32_MAX)
#else
		/* �⺻������ �۽� �Ϸ��ϴµ� 1�� �̻� �ɸ��� ���� */
		/* �۽� ���� TCP Port�� ���۰� �� ������ ���� ���� */
		if (((UINT32)GetTickCount() - u32TickTime) > TCP_SEND_TOTAL_TIMEOUT)
#endif
		{
			bSucc	= FALSE;
			break;
		}

	}	while (i32SendSize > 0);

	/* ----------------------------------------------- */
	/* ��Ŷ �����͸� ���� ���� ������ ������ �ð� ���� */
	/* ----------------------------------------------- */
	if (bRetrySend || (bSucc && u32TotalSize == size))
	{
		UpdateWorkTime();
		m_enSocket = ENG_TPCS::en_sended;
	}
	// �۽� ������ ���
	else
	{
		bSucc	= FALSE;
		m_enSocket = ENG_TPCS::en_send_failed;
	}
	/* ----------------------- */
	/* ��Ŷ�� ������ ũ�� ��ȯ */
	/* ----------------------- */
	size = u32TotalSize;

	/* ���� �۽� �����̸� */
	if (bSucc)	m_i32SockError	= 0;

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
BOOL CTcpThread::ReSended(PUINT8 packet, UINT32 size, UINT32 retry, UINT32 wait)
{
	PUINT8 pPacket	= packet;
	UINT32 u32Size	= size, u32Total = 0, u32Retry = retry;

	do {

		// ���� ������ ���� ��� ������ ���ɼ��� ŭ!!!
		if (!Sended(pPacket, u32Size))
		{
			return FALSE;
		}
		/* ------------------------------------- */
		/* �Ϻ� Ȥ�� ��ü ��Ŷ�� �����ߴ��� Ȯ�� */
		/* ------------------------------------- */
		/* ������� ������ ������ ũ�� ���� */
		u32Total += u32Size;
		/* ��� �����Ͱ� ���۵� ������� ���� */
		if (u32Total == size)
		{
			m_i32LastSendPktSize	= u32Total;
			return TRUE;
		}

		/* ------------------------------------- */
		/* ��-������ ���� ������ ���� ��ġ ����  */
		/* ------------------------------------- */
		pPacket += u32Size;
		/* ------------------------------------- */
		/* �Ϻ� �����͸� ���۵� ��� ������ ��Ŷ */
		/* �������� ��-������ ���ؼ� ũ�� ���� */
		/* ------------------------------------- */
		u32Size = size - u32Total;

		/* ���� �ð� ���� (����: �и���) */
		::Sleep(wait);

		/* ��� �ݺ� �۾��� ���� ������ ���� ������ */
		if (u32Retry < 1)
		{
			break;
		}
		u32Retry--;

	} while (1);

	return FALSE;
}

/*
 desc : ��Ŷ�� ������ ��, �ִ� �����ϴµ� ����ϴ� �ð� ����
 parm : packet	- [in]  �۽� ��Ŷ�� ����� ����
		size	- [in]  'packet' ���ۿ� ����� ��Ŷ ũ��
		time_out- [in]  �־��� �ð� ���� ���� �Ϸ� �� ������ ��ٸ�
						(�� �ð� ���� ������ �Ϸ���� ������ ���� ó��)
		wait	- [in]  ������ ���� �ð� (����: �и���)
 retn : TRUE or FALSE (��� �̻��� �߻��� ���)
*/
BOOL CTcpThread::ReSended(PUINT8 packet, UINT32 size, UINT64 time_out, UINT32 wait)
{
	PUINT8 pPacket	= packet;
	UINT32 u32Size	= size, u32Total = 0;
	UINT64 u64Tick	= GetTickCount64();

	do {

		/* ���� ������ ���� ��� ������ ���ɼ��� ŭ!!! */
		if (!Sended(pPacket, u32Size))
		{
			return FALSE;
		}
		/* ------------------------------------- */
		/* �Ϻ� Ȥ�� ��ü ��Ŷ�� �����ߴ��� Ȯ�� */
		/* ------------------------------------- */
		// ������� ������ ������ ũ�� ����
		u32Total += u32Size;
		// ��� �����Ͱ� ���۵� ������� ����
		if (u32Total == size)
		{
			return TRUE;
		}
		/* ------------------------------------- */
		/* ��-������ ���� ������ ���� ��ġ ����  */
		/* ------------------------------------- */
		pPacket += u32Size;
		/* ------------------------------------- */
		/* �Ϻ� �����͸� ���۵� ��� ������ ��Ŷ */
		/* �������� ��-������ ���ؼ� ũ�� ���� */
		/* ------------------------------------- */
		u32Size = size - u32Total;

		/* ���� �ð� ���� (����: �и���) */
		::Sleep(wait);

		/* ��� �ݺ� �۾��� ���� ������ ���� ������ */
		if (GetTickCount64() > (u64Tick + time_out))	break;

	} while (1);

	return FALSE;
}

/*
 desc : Remote Client�κ��� ������ ���� �ñ׳��� �߻��� ���
 parm : sd	- [in]  Client socket descriptor
 retn : TRUE or FALSE
*/
BOOL CTcpThread::Received(SOCKET sd)
{
	BOOL bSucc	= TRUE;
	INT32 i32ReadBytes, i32RecvSize;
	UINT32 u32SpareBytes, u32Repeat = 1;	/* 0 ���� �ƴ϶�, �ݵ�� 1 ���� */
	UINT32 u32TickTime = (UINT32)GetTickCount();	/* ������ ���Ѵ�� �� ���, ���� ���������� ���� */

	/* �۽� ���� ������ �۽� ���� ���� */
	m_enSocket	= ENG_TPCS::en_recving;

	do	{

		/* ---------------------------------------------------------- */
		/* ���� ���ŵ� �����͸� ������ �� �ִ� ���� ������ ũ�� ���   */
		/* �� �̻� ���� ���� ���۰� ���� ���� ���� ó������ �ʴ´�. */
		/* ---------------------------------------------------------- */
		u32SpareBytes	= m_pPktRingBuff->GetWriteSize();
		if (u32SpareBytes <= 0)	break;	// �� �̻� ���Ź��� ���۰� �����Ƿ�, �ϴ� ���۸� ���� �۾��� �����ϵ��� ó�� �Ѵ�

		/* ---------------------------------------------------------------- */
		/* ���� �ִ� ���� ���� ������ ũ�⸸ŭ ������ ���� �۾��� ���� �Ѵ� */
		/* ---------------------------------------------------------------- */
		/* ������ �� �ִ� ���� ũ�� ��ŭ �е�, �ӽ� ���� ���� ũ�⿡ ����, ���� ������ ũ�� ���� */
		if (u32SpareBytes > m_u32RecvBuff)	i32RecvSize	= (INT32)m_u32RecvBuff;
		else								i32RecvSize	= (INT32)u32SpareBytes;
		i32ReadBytes = recv(sd, (CHAR *)m_pPktRecvBuff, i32RecvSize, 0);
 		// ���� ������ �߻��� ���
 		if (i32ReadBytes == SOCKET_ERROR)
 		{
 			m_i32SockError = WSAGetLastError();
 			/* IO Pending ������ �ƴϰ�, ���� �����̸� ��� ������ ���� �Ѵ�. */
 			if (m_i32SockError != WSA_IO_PENDING && m_i32SockError != WSAEWOULDBLOCK)
 			{
				LOG_ERROR(ENG_EDIC::en_engine, L"TCP/IP was disconnected because failed to recv the packet");
				CloseSocket(m_sdClient);
				return FALSE;
 			}
			
			/* TCP ��� �ð� ���� */
			UpdateWorkTime();

			/* ----------------------------------------------------------------------- */
			/* ���� Ƚ����ŭ �ݺ��Ͽ� recv �Լ��� ȣ���Ͽ� ���ŵ� ��Ŷ�� �е��� �ϴ��� */
			/* �Ʒ��� ���� ���� ��, �ݺ� Ƚ����ŭ �е��� ���� ���, ���żӵ� ���� �߻� */
			/* ----------------------------------------------------------------------- */
			if (m_u32RecvRepeat > u32Repeat)
			{
				Sleep(m_u32RecvSleep);
				u32Repeat++;
			}
			else
			{
				/* ���ŵǾ��ٰ� ��� �� ���� */
				m_enSocket	= ENG_TPCS::en_recved;
				/* �̵����� �ƴϸ�, ���� ���� �ٽ� �е��� ��ȯ */
				return TRUE;
			}
 		}

		/* �ϴ� ���ŵ� �����͸� Ring ���� ������ ���� */
		m_pPktRingBuff->WriteBinary(m_pPktRecvBuff, i32ReadBytes);

		/* ��û�� ������ ũ�⺸�� ���ŵ� ������ ũ�Ⱑ ������ ��ƾ ���� ������ */
		if (i32ReadBytes < i32RecvSize)	break;

		/* ���� ������ ��� ������ ���� Ȯ�� */
		if ((UINT32)GetTickCount() - u32TickTime > TCP_RECV_TOTAL_TIMEOUT)
		{
			bSucc	= FALSE;
			break;
		}

	} while (1);

	/* ���� ���� �����̸� */
	m_i32SockError	= 0;

	// TCP ��� �ð� ����
	UpdateWorkTime();

	/* ������ �м� ������ ���, ���� ��� ��Ʈ�� ���� ���� ��� ���� */
	if (!bSucc)
	{
		ReceiveAllData(sd);
		m_enSocket	= ENG_TPCS::en_recv_failed;	/* ���ŵǾ��ٰ� ��� �� ���� */
	}
	else
	{
		m_enSocket	= ENG_TPCS::en_recved;	/* ���ŵǾ��ٰ� ��� �� ���� */
		bSucc		= RecvData();
		if (!bSucc)	ReceiveAllData(sd);
	}

	return bSucc;
}

/*
 desc : ���� ��� ��Ʈ�� ���� ���ۿ� ���� �ִ� ��� �����͸� �о ������.
		�� �̻� �о���� �����Ͱ� ���� ������ ...
 parm : sd	- [in]  ��� �����͸� �о���̰��� �ϴ� ���� �ڵ�
 retn : �� �о ���� ���� ����Ʈ ũ�� (-1 ���̸� ���� �о���� �����Ͱ� ����?)
*/
INT32 CTcpThread::ReceiveAllData(SOCKET sd)
{
	CHAR szBuff[1024+1]	= {NULL};
	INT32 i32RecvBytes	= 0, i32RecvTotal = 0;
	UINT32 u32TickTime	= (UINT32)GetTickCount();

	if (INVALID_SOCKET != sd)
	{
		do	{

			/* �ϴ� ������ 1024 Bytes�� �о���̱� */
			i32RecvBytes = recv(sd, (CHAR *)szBuff, 1024, 0);
			i32RecvTotal += i32RecvBytes;

			/* ���� ������ ��� ������ ���� Ȯ�� - �ִ� 2 �� ���ȸ� ��� �б� ���� */
			if (((UINT32)GetTickCount() - u32TickTime) > TCP_RECV_TOTAL_TIMEOUT)	break;

		}	while (i32RecvBytes == 1024);
	}

	/* TCP/IP ��� �۾� �ð� ���� */
	UpdateWorkTime();

	/* ���� ���� ũ�� �ʱ�ȭ */
	m_pPktRingBuff->ResetBuff();

	/* ������ ��Ŷ �۽� ������ �ð����� �ֱⰡ �ʹ� ª���� �θ𿡰� �˸��� ���� */
	LOG_WARN(ENG_EDIC::en_engine, L"Read all the data in the buffer and discarded it");

	return i32RecvTotal;
}

/*
 desc : ��뷮 ���� ���ۿ� �� �̻� ���ŵ� ��Ŷ�� ������ Ȯ��
 parm : None
 retn : TRUE - ���ŵ� ���ۿ� �����ִ� �����Ͱ� �ִ�, FALSE - ���ŵ� �����Ͱ� ����
*/
BOOL CTcpThread::IsEmptyPktBuff()
{
	return (m_pPktRingBuff && m_pPktRingBuff->GetReadSize() < 1) ? TRUE : FALSE;
}

/*
 desc : �����忡�� ���� �ð����� ��� �۾��� �������� �ʾҴ��� ���� ...
 parm : time_gap	- [in]  �� �־��� �ð� ���� ��. ��, �� �ð�(����: �и���)���� ����� �̷������ �ʾҴ��� Ȯ���ϱ� ����
 retn : TRUE - �ش� �ð����� ��� ���� �۾��� ������� �ʾ���
		FALSE - �ش� �־��� ���� ��� ���� �۾��� ����Ǿ���
*/
BOOL CTcpThread::IsTcpWorkIdle(UINT32 time_gap)
{
	return (GetTickCount64() - m_u64WorkTime) > time_gap ? TRUE : FALSE;
}

/*
 desc : ������ �۾��� �����ص� �Ǵ��� ���� (��, ������ ���� �ð��� �Ǿ����� ����)
 parm : time_gap	- [in] �� �־��� �ð� ���� �� (����: �и���)
 retn : TRUE - ������ �۾��� �����϶�, FALSE - ���� ������ �۾� �������� ���ƶ�
*/
BOOL CTcpThread::IsTcpWorkConnect(UINT32 time_gap)
{
	return (GetTickCount64() - m_u64ConnectTime) > time_gap ? TRUE : FALSE;
}

/*
 desc : Alive Check�� ���� Ƚ�� �̻� ������ ���, ������ ���� �۾� ����
 parm : None
 retn : None
*/
VOID CTcpThread::SetForceClose()
{
	CloseSocket(m_sdClient);
	m_sdClient	= INVALID_SOCKET;
}

/*
 desc : Alive Check Ȥ�� Sync Time���� ����. Server���� ����� ��� �ִ��� Check ��Ŷ�� ����
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

	/* Live Check ���� ��Ŷ�� �ֱ������� ���� */
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
