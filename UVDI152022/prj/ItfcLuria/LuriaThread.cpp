
/* desc : Luria ���� �⺻ Ŭ���� */

#include "pch.h"
#include "LuriaThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : shmem		- [in]  Luria Shared Memory
		conf		- [in]  Luria ȯ�� ����
		th_id		- [in]  Thread ID
		source_ip	- [in]  Local IPv4
		target_ip	- [in]  Remote IPv4
		tcp_port	- [in]  TCP/IP Port
		port_size	- [in]  TCP/IP Port Buffer Size
		recv_size	- [in]  Send or Recv�� ��Ŷ �� ���� ū ��Ŷ�� ũ�� (����: Bytes)
		ring_size	- [in]	Max Recv Packet Size
		sock_time	- [in]  �⺻ 30 �и��� ���� ���� �̺�Ʈ ������ ���� ���
		recv_count	- [in]  ���������� ���ŵǴ� �����͸� ������ �ּ� ���� ���� �Ҵ�
 retn : None
*/
CLuriaThread::CLuriaThread(LPG_LDSM shmem,
						   LPG_CLSI conf,
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

	/* ���� or ���� �ȵǾ��ٰ� ���� */
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
	m_u32RecvRepeat		= 0;	// ���� �� ���� 3�� ���, Socket�� recv �Լ��� 3�� �ݺ� ȣ��
	m_u32RecvSleep		= 0;	// ���� �� ���� 50�� ���, Socket�� recv �Լ� ȣ�� ������ 50 msec �����
	/* ---------------------------------- */
	/* IP �ּ� ��ȯ: DWORD -> String ��ȯ */
	/* ---------------------------------- */
	wmemset(m_tzSourceIPv4, 0x00, IPv4_LENGTH);
	memcpy(&stInAddr.s_addr, &source_ip, sizeof(UINT32));
	InetNtopW(AF_INET, (CONST VOID*)&stInAddr, m_tzSourceIPv4, IPv4_LENGTH);
	wmemset(m_tzTargetIPv4, 0x00, IPv4_LENGTH);
	memcpy(&stInAddr.s_addr, &target_ip, sizeof(UINT32));
	InetNtopW(AF_INET, (CONST VOID*)&stInAddr, m_tzTargetIPv4, IPv4_LENGTH);

	/* --------------------------------------- */
	/* ���ŵ� ��Ŷ �����͸� �ӽ÷� ������ ���� */
	/* --------------------------------------- */
	m_pRecvQue	= new CLuriaQue(recv_count);
	ASSERT(m_pRecvQue);

	/* ������ �� ���� ���� ��� ���� ���� �ʱ�ȭ */
	m_bIsInited	= InitData();
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CLuriaThread::~CLuriaThread()
{
	/* Release Client Socket */
	if (INVALID_SOCKET != m_sdClient)		CloseSocket(m_sdClient);
	/* Socket Event ���� */
	if (WSA_INVALID_EVENT != m_wsaClient)	WSACloseEvent(m_wsaClient);

	/* ���� ���� �޸� ���� */
	if (m_pPktRecvBuff)	delete [] m_pPktRecvBuff;
	if (m_pPktRingBuff)	delete m_pPktRingBuff;

	/* ���� ���� */
	if (m_syncPktRecv.Enter())
	{
		/* ���� Que�� ��ϵ� ������ �޸� ���� */
		if (m_pRecvQue)
		{
			LPG_PCLR pstPktData	= NULL;
			while (1)
			{
				pstPktData	= m_pRecvQue->PopPktData();
				if (!pstPktData)	break;
				if (pstPktData->user_data)	delete [] pstPktData->user_data;
			}
			/* �ӽ� ��Ŷ ���� �޸� ���� */
			delete m_pRecvQue;
		}
		/* ���� ���� */
		m_syncPktRecv.Leave();
	}
}

/*
 desc : ��� ������ �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLuriaThread::InitData()
{
	/* ��� �ʱ�ȭ */
	m_sdClient		= INVALID_SOCKET;
	m_wsaClient		= WSA_INVALID_EVENT;
	m_enSocket		= ENG_TPCS::en_closed;

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
BOOL CLuriaThread::InitMemory()
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
 desc : ���� ���� ���� ���� ����
 parm : subj	- [in]  ���� Ÿ��Ʋ
 retn : None
*/
VOID CLuriaThread::SocketError(PTCHAR subj)
{
	TCHAR tzMesg[128]	= {NULL};

	/* ���� �޽��� ���� */
	m_i32SockError	= WSAGetLastError();
	swprintf_s(tzMesg, 128, L"%s [err_cd = %d]", subj, m_i32SockError);
	/* �α� ��� */
	LOG_ERROR(m_enSysID, tzMesg);
}

/*
 desc : ���� �ɼ� �����ϱ� - �۽�/���� ���� ũ�� ����
 parm :	flag	- [in]  0x01:Send Buffer, 0x10:Recv Buffer, 0x11:Send/Recv Buffer
		sd		- [in]  ���� �ɼ��� ������ ���� �����
		size	- [in]  Port Buffer Size
 retn : TRUE or FALSE
*/
BOOL CLuriaThread::SetSockOptBuffSize(UINT8 flag, SOCKET sd)
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
		SocketError(L"Socket Option : Buffer Size [Luria]");
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
BOOL CLuriaThread::SetSockOptTimeout(UINT8 flag, SOCKET sd, INT32 time)
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
		SocketError(L"Socket Option : Time-out [Luria]");
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
BOOL CLuriaThread::SetSockOptSpeed(INT32 optval, SOCKET sd)
{
	/* ���� ������ �ӵ� ���� */
	if (SOCKET_ERROR == setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (CHAR *)&optval, sizeof(optval)))
	{
		SocketError(L"Socket Option : Speed [Luria]");
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
BOOL CLuriaThread::SetSockOptAlive(INT32 optval, SOCKET sd)
{
	/* ���� ������ �ӵ� ���� */
	if (SOCKET_ERROR == setsockopt(sd, SOL_SOCKET, SO_KEEPALIVE, (CHAR *)&optval, sizeof(optval)))
	{
		SocketError(L"Socket Option : Alive check [Luria]");
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
BOOL CLuriaThread::SetSockOptRoute(INT32 optval, SOCKET sd)
{
	/* ���� ������ �ӵ� ���� */
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

	/* Shutdown �Լ� ȣ���ϱ� ���� �ݵ�� �Ʒ� �÷��׺��� ������ �� */
	m_enSocket		= ENG_TPCS::en_closed;
	/* ���� �����Ǿ��ٰ� �÷��� ���� */
	m_bIsLinked		= FALSE;
	/* ���� Client ������ ��� �ִ��� ���� */
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

		/* ���Ͽ� �ش�Ǵ� ��Ʈ �ʱ�ȭ */
		stLinger.l_onoff	= 1;				/* linger on... */
		stLinger.l_linger	= UINT16(time_out);	/* timeout in seconds ... (0 -> hard close) */
		setsockopt(sd, SOL_SOCKET, SO_LINGER, (char FAR*)&stLinger, sizeof(LINGER));
		/* ���� ���� �� �ʱ�ȭ */
		closesocket(sd);
	}

	/* Update the communication time for TCP/IP */
	UpdateWorkTime();

	/* ���� ������ �ð� ���� */
	GetLocalTime(&m_stLinkTime);
}

/*
 desc : ���� �ֱٿ� ����� �ð� ���� �� ����
 parm : None
 retn : None
*/
VOID CLuriaThread::UpdateWorkTime()
{
	m_u64WorkTime = GetTickCount64();
	GetLocalTime(&m_stWorkTime);
}

/*
 desc : Remote Server�κ��� ���� ���� �ñ׳��� �߻��� ���
 parm : sd	- [in]  Client socket descriptor
 retn : None
*/
VOID CLuriaThread::Closed(SOCKET sd)
{
	/* --------------------------------------------------------- */
	/* �ݵ�� ���� �Լ� ȣ�� ��, ���� �ڵ� ���� �Լ� ȣ���ؾ� �� */
	/* --------------------------------------------------------- */

	/* ���� ���� ���� �۾� ���� */
	CloseSocket(sd);
}

/*
 desc : ���� ��Ŷ ������ �۽ź�
 parm : packet	- [in]  �۽� ��Ŷ�� ����� ����
		size	- [out] �Է�: 'packet' ���ۿ� ����� ��Ŷ ũ��
						��ȯ: ���۵� �������� ũ��
 retn : TRUE or FALSE (��� �̻��� �߻��� ���)
*/
BOOL CLuriaThread::Sended(PUINT8 packet, UINT32 &size)
{
	BOOL bSucc		= TRUE;
	CHAR *pPktSend		= (CHAR*)packet;
	INT32 i32SendBytes	= 0, i32SendSize = INT32(size);
	UINT32 u32TotalSize	= 0, u32TickTime = (UINT32)GetTickCount();

	/* �۽� ���� ������ �۽� ���� ���� */
	m_enSocket	= ENG_TPCS::en_sending;

	do	{

		i32SendBytes = send(m_sdClient, pPktSend, i32SendSize, 0);
		if (SOCKET_ERROR == i32SendBytes)
		{
			// ���� ���� ��尡 �ƴϴٶ��... ����� ����� �ȴ�.
			INT32 i32SockError	= WSAGetLastError();
			if (i32SockError != WSAEWOULDBLOCK &&
				i32SockError != WSA_IO_PENDING)
			{
				SocketError(L"Failed to send the packet data [Luria]");
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
			/* ������� ���۵� ������ ũ�� ���� */
			u32TotalSize+= i32SendBytes;
			/* ������ ���۵� ������ ũ�� ���� */
			i32SendSize	-= i32SendBytes;
			/* ������ ������ ��ġ�� �̵� */
			pPktSend	+= i32SendBytes;
		}

		/* ���� ������ ��� ������ ���� Ȯ�� */
		if (((UINT32)GetTickCount() - u32TickTime) > TCP_SEND_TOTAL_TIMEOUT)
		{
			bSucc	= FALSE;
			break;
		}

	}	while (i32SendSize > 0);

	/* ��Ŷ �����͸� ���� ���� ������ ������ �ð� ���� */
	if (bSucc)
	{
		if (u32TotalSize == size)
		{
			UpdateWorkTime();
			m_enSocket = ENG_TPCS::en_sended;
		}
		/* �۽� ������ ��� */
		else	bSucc = FALSE;
	}

	/* ��Ŷ�� ������ ũ�� ��ȯ */
	size = u32TotalSize;

	if (!bSucc)	m_enSocket = ENG_TPCS::en_send_failed;

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
BOOL CLuriaThread::ReSended(PUINT8 packet, UINT32 size, UINT32 retry, UINT32 wait)
{
	PUINT8 pPacket	= packet;
	UINT32 u32Size	= size, u32Total = 0, u32Retry = retry;

	do {

		/* ���� ������ ���� ��� ������ ���ɼ��� ŭ!!! */
		if (!Sended(pPacket, u32Size))	return FALSE;

		/* �Ϻ� Ȥ�� ��ü ��Ŷ�� �����ߴ��� Ȯ�� */
		/* ������� ������ ������ ũ�� ���� */
		u32Total += u32Size;
		/* ��� �����Ͱ� ���۵� ������� ���� */
		if (u32Total == size)	return TRUE;

		/* ��-������ ���� ������ ���� ��ġ ����  */
		pPacket += u32Size;

		/* �Ϻ� �����͸� ���۵� ��� ������ ��Ŷ */
		/* �������� ��-������ ���ؼ� ũ�� ���� */
		u32Size = size - u32Total;

		/* ���� �ð� ���� (����: �и���) */
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
BOOL CLuriaThread::Received(SOCKET sd)
{
	BOOL bSucc	= TRUE;
	INT32 i32ReadBytes, i32RecvSize;
	UINT32 u32SpareBytes, u32Repeat = 1;	/* 0 ���� �ƴ϶�, �ݵ�� 1 ���� */
	UINT32 u32TickTime	= (UINT32)GetTickCount();

	m_enSocket	= ENG_TPCS::en_recving;

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
		i32ReadBytes = recv(sd, (CHAR *)m_pPktRecvBuff, i32RecvSize, 0);

		// ���� ������ �߻��� ���
 		if (i32ReadBytes == SOCKET_ERROR)
 		{
 			// IO Pending ������ �ƴϰ�, ���� �����̸� ��� ������ ���� �Ѵ�.
			INT32 i32SockError	= WSAGetLastError();
 			if (i32SockError != WSA_IO_PENDING && i32SockError != WSAEWOULDBLOCK)
 			{
				SocketError(L"Failed to recv the packet [CLuriaThread::Received]");
				return FALSE;
 			}
			
			// TCP ��� �ð� ����
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
				// ���ŵǾ��ٰ� ��� �� ����
				m_enSocket	= ENG_TPCS::en_recved;
				// �̵����� �ƴϸ�, ���� ���� �ٽ� �е��� ��ȯ �Ѵ�
				return TRUE;
			}
 		}

		/* �ϴ� ���ŵ� �����͸� Ring ���� ������ ���� */
		m_pPktRingBuff->WriteBinary(m_pPktRecvBuff, i32ReadBytes);
		// TCP ��� �ð� ����
		UpdateWorkTime();

		/* ��û�� ������ ũ�⺸�� ���ŵ� ������ ũ�Ⱑ ������ ��ƾ ���� ������ */
		u32SpareBytes	= m_pPktRingBuff->GetWriteSize();
		if (i32ReadBytes < i32RecvSize || u32SpareBytes < 1)	break;

		/* ���� ������ ��� ������ ���� Ȯ�� */
		if (((UINT32)GetTickCount() - u32TickTime) > TCP_RECV_TOTAL_TIMEOUT)
		{
			LOG_ERROR(m_enSysID, L"An infinite loop has been entered [CLuriaThread::Received] (3 secs)");
			bSucc	= FALSE;
			break;
		}

	} while (1);
 	
	/* ���ŵǾ��ٰ� ��� �� ���� */
	if (bSucc)
	{
		m_enSocket		= ENG_TPCS::en_recved;
		m_i32SockError	= 0;	/* ���� ���� �����̸� */
	}
	else		m_enSocket	= ENG_TPCS::en_recv_failed;

	/* ������ �м� ������ ���, ���� ��� ��Ʈ�� ���� ���� ��� ���� */
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
 desc : ���� ��� ��Ʈ�� ���� ���ۿ� ���� �ִ� ��� �����͸� �о ������.
		�� �̻� �о���� �����Ͱ� ���� ������ ...
 parm : sd	- [in]  ��� �����͸� �о���̰��� �ϴ� ���� �ڵ�
 retn : �� �о ���� ���� ����Ʈ ũ�� (-1 ���̸� ���� �о���� �����Ͱ� ����?)
*/
INT32 CLuriaThread::ReceiveAllData(SOCKET sd)
{
	CHAR szBuff[1024+1]	= {NULL};
	INT32 i32RecvBytes	=0, i32RecvTotal	= 0;
	UINT32 u32TickTime	= (UINT32)GetTickCount();

	if (INVALID_SOCKET != sd)
	{
		do	{

			/* �ϴ� ������ 1024 Bytes�� �о���̱� */
			i32RecvBytes	= recv(sd, (CHAR *)szBuff, 1024, 0);
			i32RecvTotal	+= i32RecvBytes;

			/* ���� ������ ��� ������ ���� Ȯ�� - �ִ� 2 �� ���ȸ� ��� �б� ���� */
			if (((UINT32)GetTickCount() - u32TickTime) > TCP_RECV_TOTAL_TIMEOUT)	break;

		}	while (i32RecvBytes == 1024);
	}

	/* TCP/IP ��� �۾� �ð� ���� */
	UpdateWorkTime();

	/* ���� ���� ũ�� �ʱ�ȭ */
	m_pPktRingBuff->ResetBuff();

	/* ������ ��Ŷ �۽� ������ �ð����� �ֱⰡ �ʹ� ª���� �θ𿡰� �˸��� ���� */
	LOG_ERROR(m_enSysID, L"Read all the data in the buffer and discarded it");

	return i32RecvTotal;
}

/*
 desc : ��뷮 ���� ���ۿ� �� �̻� ���ŵ� ��Ŷ�� ������ Ȯ��
 parm : None
 retn : TRUE - ���ŵ� ���ۿ� �����ִ� �����Ͱ� �ִ�, FALSE - ���ŵ� �����Ͱ� ����
*/
BOOL CLuriaThread::IsEmptyPktBuff()
{
	return (m_pPktRingBuff && m_pPktRingBuff->GetReadSize() < 1) ? TRUE : FALSE;
}

/*
 desc : �����忡�� ���� �ð����� ��� �۾��� �������� �ʾҴ��� ���� ...
 parm : time_gap	- [in]  �� �־��� �ð� ���� ��. ��, �� �ð�(����: �и���)���� ����� �̷������ �ʾҴ��� Ȯ���ϱ� ����
 retn : TRUE - �ش� �ð����� ��� ���� �۾��� ������� �ʾ���
		FALSE - �ش� �־��� ���� ��� ���� �۾��� ����Ǿ���
*/
BOOL CLuriaThread::IsTcpWorkIdle(UINT32 time_gap)
{
	return (GetTickCount64() - m_u64WorkTime) > time_gap ? TRUE : FALSE;
}

/*
 desc : ������ �۾��� �����ص� �Ǵ��� ���� (��, ������ ���� �ð��� �Ǿ����� ����)
 parm : time_gap	- [in] �� �־��� �ð� ���� �� (����: �и���)
 retn : TRUE - ������ �۾��� �����϶�, FALSE - ���� ������ �۾� �������� ���ƶ�
*/
BOOL CLuriaThread::IsTcpWorkConnect(UINT32 time_gap)
{
	return (GetTickCount64() - m_u64ConnectTime) > time_gap ? TRUE : FALSE;
}

/*
 desc : Alive Check�� ���� Ƚ�� �̻� ������ ���, ������ ���� �۾� ����
 parm : None
 retn : None
*/
VOID CLuriaThread::SetForceClose()
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
BOOL CLuriaThread::RecvData()
{
	UINT32 u32PktBase	= sizeof(STG_PCLR)-sizeof(PUINT8);
	UINT64 u64SavedBytes;
	STG_PCLR stPktPdata;

	do	{

		/* ------------------------------------------------------------------------------- */
		/* ���� ���ŵ� ������ ũ�⸦ ���� �� ��Ŷ ��� ũ��(8 Bytes) ���� ���� ���� ��� */
		/* ������ �� �о�� ������ �� �̻� �۾��� �������� �ʵ��� ó�� �Ѵ�                */
		/* ------------------------------------------------------------------------------- */
		u64SavedBytes	= m_pPktRingBuff->GetReadSize();
		if (u64SavedBytes < u32PktBase)
		{
			m_enSocket	= ENG_TPCS::en_recving;
			return TRUE;
		}

		/* ---------------------------------------------------------------*/
		/* !!! �Ʒ� ��Ŷ ����ü�� �ݵ�� �� �κп��� �޸� ���� ���� !!! */
		/* ��Ŷ�� ���������� ���ŵ� ������ user_data�� ���� �ʱ�ȭ�� ���� */
		/* ---------------------------------------------------------------*/
		memset(&stPktPdata, 0x00, sizeof(STG_PCLR));

		// ��� ���� ����
		m_pPktRingBuff->CopyBinary((PUINT8)&stPktPdata, u32PktBase);
		stPktPdata.SwapNetworkToHost();
		/* �߸��� ��Ŷ�� ��� ���� ó�� */
		if (stPktPdata.num_bytes < 8)
		{
			LOG_WARN(m_enSysID, L"Header size not received. (header size < 8 bytes)");
			return TRUE;
		}
		/* --------------------------------------------------------------------------------- */
		/* ���� ���ŵ� ������ ũ�⸦ ���� �� ��Ŷ ���� ũ��(Tail ����) ���� ���� ���� ��� */
		/* ������ �� �о�� ������ �� �̻� �۾��� �������� �ʵ��� ó�� �Ѵ�                  */
		/* --------------------------------------------------------------------------------- */
		if (u64SavedBytes < stPktPdata.num_bytes)
		{
			LOG_WARN(m_enSysID, L"Not all packets were received. Waiting ...");
			return TRUE;
		}
		/* ------------------------------------------- */
		/* user data ���� �������� ��� �о ������. */
		/* ------------------------------------------- */
		m_pPktRingBuff->PopBinary(u32PktBase);
		/* ----------------------------------------------------------------------- */
		/* ���ۿ� �ϳ� �̻��� ��Ŷ�� ����Ǿ� �ִٸ� ������ �����͸� �о�� �Ѵ� */
		/* ----------------------------------------------------------------------- */
		if (stPktPdata.num_bytes-u32PktBase > 0)
		{
			/* ���ŵ� ��Ŷ�� ���� ������ ������ ���� �޸� �Ҵ� */
			stPktPdata.user_data	= new UINT8 [stPktPdata.num_bytes-u32PktBase+1];
			stPktPdata.user_data[stPktPdata.num_bytes-u32PktBase]	= 0x00;
			/* ���ŵ� ���� ������ ������ ��������  */
			m_pPktRingBuff->ReadBinary(stPktPdata.user_data, stPktPdata.num_bytes-u32PktBase);
		}
		/* ----------------------------------------------------------------------- */
		/* ���� ���ŵ� ��Ŷ �����͸� ���� ť ���ۿ� ��� �۾��� �����ϵ��� ó�� �� */
		/* ----------------------------------------------------------------------- */
		/* ���� ���� */
		if (m_syncPktRecv.Enter())
		{
			if (!m_pRecvQue->PushPktData(&stPktPdata))
			{
				/* ��Ŷ ���� ���۰� �̹� FULL ���� */
				LOG_ERROR(m_enSysID, L"There are no more buffers for received packets");
				/* ������ ������ �޸� ����*/
				if (stPktPdata.user_data)
				{
					delete [] stPktPdata.user_data;
					stPktPdata.user_data	= NULL;
				}
			}
			/* ���� ���� */
			m_syncPktRecv.Leave();
		}

		/* ------------------------------------------------------------------------------- */
		/* ���� ��Ŷ �ϳ��� �а� �� ����, ���ۿ� ���� �ִ� �������� ũ�Ⱑ ��Ŷ�� �����ϴ� */
		/* �ּ��� ��Ŷ ũ�⸸ŭ ���� ������, ������ �ѹ� �� �����ϰ�, �ּ� ��Ŷ ũ�⸸ŭ   */
		/* ���� ���� ������ �� �о������ �ش� ������ ������������ ó�� �Ѵ�               */
		/* ------------------------------------------------------------------------------- */
	}	while (!m_pRecvQue->IsQueEmpty() && (u64SavedBytes - stPktPdata.num_bytes) >= u32PktBase);

	return TRUE;
}

/*
 desc : Luria Status �ڵ�� ���εǴ� ���� ���ڿ� (����) ��ȯ
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
 desc : Luria �κ��� ������ �߻��� ���, ó��
 parm : level	- [in]  �޽��� ���� ���� (ENG_LNWE)
		type	- [in]  0x00:Client, 0x01:Server(Announce)
		data	- [in]  ���� ��Ŷ ������ ����� ����ü ������
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
	/* �� ������ �߻��ϴ��� �� */
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
	/* ���� �ֱٿ� �߻��� Luria ���� �� �ӽ� ���� (������ ���� �ڵ尡 �����ϸ� �������� ����) */
#if 0
	if (data->status != m_pstShMemLuria->link.u16_error_code)
	{
		m_pstShMemLuria->link.u16_error_code = data->status;
	}
#else
	/* ���� �ֱ��� ���� ������ ������ ���� */
	m_pstShMemLuria->link.u16_error_code	= data->status;
#endif
}