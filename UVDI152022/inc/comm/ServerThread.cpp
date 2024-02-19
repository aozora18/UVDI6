
/* desc : Server TCP/IP Thread */

#include "pch.h"
#include "ServerThread.h"


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
		port_buff	- [in]  TCP/IP Port Buffer Size
		recv_buff	- [in]  Send or Recv�� ��Ŷ �� ���� ū ��Ŷ�� ũ�� (����: Bytes)
		ring_buff	- [in]	Max Recv Packet Size
		sock_time	- [in]  ���� �̺�Ʈ (��/����) ���� ��� �ð� (����: �и���)
							�ش� �־��� ��(�ð�)��ŭ�� ���� �̺�Ʈ ������ ���� �ܰ�� ��ƾ �̵�
		idle_time	- [in]  �־��� �ð� ���� ������ ����, Listen ������ ���ʱ�ȭ �ϴ� �ð� (����: �и���)
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
	m_u64IdleTime	= idle_time;	// Listen Socket �ֱ������� �ʱ�ȭ�ϴ� �ð� ���� (����: �и���)
	/* Check whether it operates in demo mode */
	if (GetConfig()->IsRunDemo())	m_enSysID = ENG_EDIC::en_demo;
	// ���� �ʱ�ȭ �� �̺�Ʈ ����
	m_sdListen		= INVALID_SOCKET;
	m_wsaListen		= WSACreateEvent();
	if (WSA_INVALID_EVENT == m_wsaListen)
		AfxMessageBox(L"Failed to create the socket event <Server Listen>", MB_ICONSTOP|MB_TOPMOST);
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CServerThread::~CServerThread()
{
	// Release Socket �� Event
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
	/* ����� Client �������� ������, Listen Socket ���ʱ�ȭ */
	/* ------------------------------------------------------- */
#ifndef _DEBUG
	// �־��� �ð����� ���� ����� �̷������ ������ ���� ���� ���ʱ�ȭ
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
	/* Socket Event ����ó�� */
	/* --------------------- */
	u8PopRet = PopSockEvent();
	if (u8PopRet == 1 || u8PopRet == 2)
	{
		TCHAR tzMesg[128]	= {NULL};
		if (0x01 == u8PopRet)	// ����� Client�� �����ϴ� ���
		{
			swprintf_s(tzMesg, 128, L"The access of <%s> has been successful", m_tzTCPInfo);
			LOG_MESG(ENG_EDIC::en_engine, tzMesg);
		}
		else					// ������ Client�� �����ϴ� ���
		{
			swprintf_s(tzMesg, 128, L"The access of <%s> has been failed", m_tzTCPInfo);
			SocketError(tzMesg);
		}
	}
	/* --------------------- */
	/* DoWork ���� �Լ� ȣ�� */
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
	/* �θ� ���� CloseSocket �Լ� ȣ���ϱ� ���� ������ ���� ���� */
	/* --------------------------------------------------------- */
	// ���� �θ� �Լ� ȣ��
	CTcpThread::CloseSocket(sd, time_out);
	// Socket ������ ���� �ʱ�ȭ ��� �ٸ��� ó��
	if (sd == m_sdListen)	m_sdListen	= INVALID_SOCKET;
	if (sd == m_sdClient)	m_sdClient	= INVALID_SOCKET;
}

/*
 desc : ���� �ʱ�ȭ ��, Server�� Listen socket �ʱ�ȭ �۾� ����
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

	// ���� Listen ������ �ʱ�ȭ���� �ʾҴٸ� ...
	if (INVALID_SOCKET != m_sdListen)	CloseSocket(m_sdListen);
	// ���� Client ������ �ʱ�ȭ���� �ʾҴٸ� ...
	if (INVALID_SOCKET != m_sdClient)	CloseSocket(m_sdClient);
#if 0
	/* -------------------------------------------------------------------- */
	/* Server or Client�� Listen or Client Socket ���ʱ�ȭ�� ���� �ð� ���� */
	/* -------------------------------------------------------------------- */
	m_u32ResetTime			= (UINT32)GetTickCount();
#endif
	// ���� ����
	stAddrIn.sin_family		= AF_INET;
	stAddrIn.sin_port		= htons(m_u16TcpPort);
	/* �������� Land Card�� IPv4 �ּ� �Է��ϸ� ������, �������� �ο��� IPv4�� ���� �� */
#if 0
	InetPtonW(AF_INET, m_tzSourceIPv4, &stAddrIn.sin_addr.s_addr);
#else
	stAddrIn.sin_addr.s_addr= htonl(INADDR_ANY);
#endif

	// ���� ����
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
	// ���� �ɼ� ���� - IP �ּ� ����
	if (bSucc)
	{
		bSucc = (SOCKET_ERROR != setsockopt(m_sdListen,
											SOL_SOCKET,
											SO_REUSEADDR,
											(CHAR *)&i32Option,
											sizeof(i32Option)));
	}
	// ���� �� ������ �ּ� ���ε�
	if (bSucc)
	{
		bSucc = (SOCKET_ERROR != ::bind(m_sdListen,
										(LPSOCKADDR)&stAddrIn,
										sizeof(SOCKADDR_IN)));
		dw32Error = WSAGetLastError();
	}
	// ���� ��� ����. ���� 5 ���� ���� ����
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
	// ���� �̺�Ʈ ���� (FD_ACCEPT�� ����!!!)
	if (bSucc)
	{
		bSucc = (SOCKET_ERROR != WSAEventSelect(m_sdListen, m_wsaListen, l32Event));
	}
	// �����ϰ� �����ϰ�, ���� ���� �ٽ� Listen�ϱ� ���� ���� ��� �ð� ������ ����
	UpdateWorkTime();
	// ���� ������ ��ٸ��� �ִٰ� ����
	m_enSocket	= ENG_TPCS::en_linking;

	// �θ𿡰� �˸�
	return bSucc;
}

/*
 desc : ���� �̺�Ʈ ����
 parm : None
 retn : 0x00 - ����, 0x01 - ����, 0x02 - ����, 0x03 - ������ ����
*/
UINT8 CServerThread::PopSockEvent()
{
	UINT8 u8RetCode	= 0x00;
	INT32 i32WsaRet	= 0;
	UINT32 u32WsaRet= 0;
	SOCKET sdSock[2]	= {m_sdListen, m_sdClient};
	WSAEVENT wsaSock[2]	= {m_wsaListen, m_wsaClient};
	WSANETWORKEVENTS wsaNet;

	// Socket Event�� Signal ���� �����ϱ�
	u32WsaRet	= WSAWaitForMultipleEvents(2,				// ���� �̺�Ʈ ���� ����: 1����
										   wsaSock,			// ���� �̺�Ʈ �ڵ� (Listen or Client)
										   FALSE,			// TRUE : ��� ���� �̺�Ʈ ��ü�� ��ȣ ����, FALSE : 1�� �̺�Ʈ ��ü�� ��ȣ �����̸� ��� ����
										   m_u32SockTime,	// ���� �� �̳��� ������ ������ ����
										   FALSE);			// WSAEventSelect������ �׻� FALSE
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
	/* �̺�Ʈ ��ü�� ��ȣ ���°� �� ������ ����Ѵ�. WSAWaitForMultipleEvents() �Լ��� ��ȯ ���� */
	/* ��ȣ ���°� �� �̺�Ʈ ��ü�� �迭 �ε��� + WSA_WAIT_EVENT_0 ���̴�. �׷��Ƿ�, ���� �ε��� */
	/* ���� �������� WSA_WAIT_EVENT_0 (WAIT_BOJECT_0) ���� ���� �Ѵ�.                            */
	/* ----------------------------------------------------------------------------------------- */
	u32WsaRet	-= WSA_WAIT_EVENT_0;
	i32WsaRet	= WSAEnumNetworkEvents(sdSock[u32WsaRet],	// ��� ����
									   wsaSock[u32WsaRet],	// ��� ���ϰ� ¦�� �̷� ��ü �ڵ�
									   &wsaNet);			// �̺�Ʈ ������ ����� ����ü ������
	// ������ �������� Ȯ��
	if (i32WsaRet == SOCKET_ERROR)	return 0x00;

	/* ------------------------------ */
	/* Socket Event �� �и� ó�� ���� */
	/* ------------------------------ */
	if (FD_READ == (FD_READ & wsaNet.lNetworkEvents))
	{
		// ��� ��Ʈ�κ��� ������ �б� �õ�
		if (wsaNet.iErrorCode[FD_READ_BIT] == 0)
		{
			if (Received(sdSock[1]))	u8RetCode = 0x03;
		}
		// ������ �б� ������ ���, ���� ��� ��Ʈ�� ���� ��� �о ������
		else
		{
			ReceiveAllData(sdSock[1]);
		}
	}
	// Ŭ���̾�Ʈ�� ������ ���� �������� ...
	else if (FD_ACCEPT == (FD_ACCEPT & wsaNet.lNetworkEvents))
	{
		// ���� ������ �߻� �ߴ��� ����
		if (wsaNet.iErrorCode[FD_ACCEPT_BIT] != 0)
		{
			m_i32SockError = WSAGetLastError();
		}
		// ������ �����κ��� ���������� ���ι��� ���
		else
		{
			// ���ӵǾ��ٰ� �÷��� ����
			m_bIsLinked	= TRUE;
			// �����Լ� ȣ��
			Accepted(m_sdListen);
			u8RetCode	= 0x01;
		}
	}
	// ������ ������ ��������
	else if (FD_CLOSE == (FD_CLOSE & wsaNet.lNetworkEvents))
	{
		CloseSocket(sdSock[1]);
		u8RetCode	= 0x02;
	}

	return u8RetCode;
}

/*
 desc : Client�κ��� ���� �ñ׳��� ������ ���
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
	/* ����� Ŭ���̾�Ʈ�� �����Ƿ�, �ϴ� ����Ǿ��ٰ� �÷��� ���� */
	/* ----------------------------------------------------------- */
	m_enSocket = ENG_TPCS::en_linked;
	/* ------------------------------------------------------- */
	/* ����Ǿ����Ƿ�, ���� �ʱ�ȭ�� ���� ���ϵ��� �ð��� ���� */
	/* ------------------------------------------------------- */
	UpdateWorkTime();

	// ���Ӱ� ���ӵ� Ŭ���̾�Ʈ�� ���� �ϴ� ���� ���� ó��
	sdClient	= WSAAccept(sd, (LPSOCKADDR)&sdAddrIn, &i32AddrLen, 0, 0);
	memcpy(&stInAddr.s_addr, &sdAddrIn.sin_addr.s_addr, sizeof(UINT32));
	// ���� ������ ��, ������ �߻� �ߴ��� ���� Ȯ��
	if (INVALID_SOCKET == sdClient)
	{
		SocketError(L"Invalid the connected ip of client");
		m_i32SockError	= WSAGetLastError();
		bSucc	= FALSE;
	}
	/* -------------------------------------------------------------------------- */
	/* ���ӵ� Ŭ���̾�Ʈ�� ������ ��ϵ� Client IPv4 �ּҰ� �ƴϸ� ���� ���� ó�� */
	/* -------------------------------------------------------------------------- */
	InetNtopW(AF_INET, (CONST VOID*)&stInAddr, tzIPv4, IPv4_LENGTH);
	if (bSucc && (0 != wcscmp(m_tzTargetIPv4, tzIPv4)))
	{
		// ���� ����� Client ���� ��ü ó��
		CloseSocket(sdClient);
		bSucc	= FALSE;
		return;
	}
	/* ------------------------------------------------- */
	/* ���� ������ ���ӵ� Client�� �ִٸ� ���� ���� ó�� */
	/* ------------------------------------------------- */
	if (INVALID_SOCKET != m_sdClient)
	{
		SocketError(L"Disconnect already the connected ip of client");
		CloseSocket(m_sdClient);
		m_sdClient	= INVALID_SOCKET;
	}
	/* --------------------------------------- */
	/* ���� ����� ������ �پ��� �ɼ� �����ϱ� */
	/* --------------------------------------- */
	if (bSucc)	bSucc = SetSockOptBuffSize(0x11, sdClient);	// 8 KBytes�� ǥ����
//	if (bSucc)	bSucc = SetSockOptSpeed(1, sdClient);
	if (bSucc)	bSucc = SetSockOptRoute(1, sdClient);
//	if (bSucc)	bSucc = SetSockOptAlive(1, sdClient);
	if (bSucc)
	{
		// ���� �̺�Ʈ ���� (FD_CLOSE|FD_READ ����!!!)
		bSucc = (SOCKET_ERROR != WSAEventSelect(sdClient, m_wsaClient, l32Event));
		if (!bSucc)
		{
			SocketError(L"Failed to setup the socket event");
		}
	}
	/* ------------------ */
	/* Ring Buffer �ʱ�ȭ */
	/* ------------------ */
	m_pPktRingBuff->ResetBuff();
	// ��� �ð� ����
	UpdateWorkTime();
	/* ------------------------------------ */
	/* ���� ����� �������̸� �����Լ� ȣ�� */
	/* ------------------------------------ */
	if (!bSucc)	CloseSocket(sdClient);
	else
	{
		/* ------------------------------------------ */
		/* ���� �ֱٿ� ������ Client�� ���� �ڵ� ���� */
		/* ------------------------------------------ */
		m_sdClient	= sdClient;	// �ݵ�� Accepted �Լ� ������ ����(����)�Ǿ�� ��
		// ���� ���ο� ���� �ʱ�ȭ �۾� ����
		if (!Accepted())
		{
			CloseSocket(m_sdClient);
			m_sdClient	= INVALID_SOCKET;
		}
		else
		{
			// Ŭ���̾�Ʈ�� ������ �ð� ����
			GetLocalTime(&m_stLinkTime);
		}
	}
}

/*
 desc : ���� ����� Client Socket ���� ���� ó��
 parm : None
 retn : None
*/
VOID CServerThread::CloseClient()
{
	if (INVALID_SOCKET != m_sdClient)	CloseSocket(m_sdClient);
}

/*
 desc : ���� Server Socket ���� ó��
 parm : None
 retn : None
*/
VOID CServerThread::CloseServer()
{
	if (INVALID_SOCKET != m_sdListen)	CloseSocket(m_sdListen);
	if (INVALID_SOCKET != m_sdClient)	CloseSocket(m_sdClient);
}
