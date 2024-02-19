
/* desc : TCP Client ��ü */

#include "pch.h"
#include "ClientThread.h"


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
		idle_time	- [in]  ���� �ý��۰��� ��� ���� (Alive Check) üũ�ϱ� ���� �ð� (����: �и���)
							��, �� �ð� (����: ��) ���� �ƹ��� ����� ������ Alive Check �۽�
							�� ���� 0 �� ���, �� ����� ������� ����
		link_time	- [in]  ���� �ý��ۿ� ������ ���� ���� ��, �ֱ������� �����ϴ� �ð� (����: �и���)
		sock_time	- [in]  ���� �̺�Ʈ (��/����) ���� ��� �ð� (����: �и���)
							�ش� �־��� ��(�ð�)��ŭ�� ���� �̺�Ʈ ������ ���� �ܰ�� ��ƾ �̵�
		live_time	- [in]  Client�� Server���� �ֱ������� Live Check�� �۽��ϴ� �ð� (����: �и���)
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
	m_bInitRecv		= FALSE;		/* �ʱ� ���ŵ� �����Ͱ� �������� ���� */
	m_u32LinkTime	= link_time;	/* �������ϴ� �ֱ� ���� (����: �и���) */
	m_u32IdleTime	= idle_time;	/* �ش�ð� ���� ��� �۾��� ������ ���� ��� ���� ���� */
	m_u32LiveTime	= live_time;	/* �ش�ð� ���� Server���� Alive Check ���� �۽� ���� */
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CClientThread::~CClientThread()
{
}

/*
 desc : �ֱ������� ȣ���
 parm : None
 retn : None
*/
VOID CClientThread::RunWork()
{
	/* ���� ����� Client�� �������� ������, ������ ���� */
	if (!m_bIsLinked)
	{
		if (m_enSocket == ENG_TPCS::en_closed || m_enSocket == ENG_TPCS::en_linking)
		{
			/* ������ �ð����� �������� �����ϵ��� �ϱ� ���� */
			if (IsTcpWorkConnect(m_u32LinkTime))
			{
				/* ���� �۾� ���� (���� �ʱ�ȭ ���� ���� �õ�) */
				if (!DoConnect())	m_i32SockError = WSAGetLastError();
			}

			/* Socket Event ����ó�� (����Ǿ��ٸ�, �����Լ� ȣ��) */
			if (m_enSocket == ENG_TPCS::en_linking)
			{
				if (!IsLinkEvent())	return;
				/* ���������� ����Ǿ��� ��, �ʱ�ȭ ���� */
				Connected(m_sdClient);
			}
		}
	}
	else
	{
		/* Socket Event ���� Ȯ�� */
		if (0x02 == PopSockEvent())
		{
			TCHAR tzMesg[128]	= {NULL};
			CUniToChar csCnv;
			swprintf_s(tzMesg, 128,
					   L"Disconnected from the remote system (%s) / err_cd:%d)",
					   m_tzTCPInfo, m_i32SockError);
			LOG_ERROR(ENG_EDIC::en_bsa, tzMesg);
		}

		/* ���� �ð����� Alive Time ���� �۽� */
		if (m_u32LiveTime && IsTcpWorkIdle(m_u32LiveTime))
		{
			/* EFEM Robot ����� ��츸 �ش�� */
			if (ENG_EDIC::en_efem == m_enSysID)
			{
				SendAliveTime();
			}
		}

		/* DoWork ���� �Լ� ȣ�� */
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

	/* �ʱ� ����ǰ� �� ���� ���ŵ� �����Ͱ� ���ٰ� ���� */
	m_bInitRecv			= FALSE;
	/* Server or Client�� Listen or Client Socket ���ʱ�ȭ�� ���� �ð� ���� */
	m_u64ConnectTime	= GetTickCount64();

	/* Ring Buffer �ʱ�ȭ */
	m_pPktRingBuff->ResetBuff();

	/* Remove existing client socket exists */
	if (INVALID_SOCKET != m_sdClient)
	{
		if (!IsRunDemo())	LOG_WARN(ENG_EDIC::en_bsa, L"Release an exisiting connected socket");
		CloseSocket(m_sdClient);
	}

	/* ������ �õ��ϹǷ� �÷��� ���� */
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
 desc : ���� ���� �̺�Ʈ�� �ִ��� Ȯ��
 parm : None
 retn : TRUE - ���� �̺�Ʈ �߻�, FALSE - ���� �̺�Ʈ ����
*/
BOOL CClientThread::IsLinkEvent()
{
	INT32 i32WsaRet		= 0;
	UINT32 u32WsaRet	= 0;
	WSANETWORKEVENTS wsaNet;

	/* Socket Event�� Signal ���� �����ϱ� */
	u32WsaRet	= WSAWaitForMultipleEvents(1,				/* ���� �̺�Ʈ ���� ����: 1���� */
										   &m_wsaClient,	/* ���� �̺�Ʈ �ڵ� */
										   FALSE,			/* TRUE : ��� ���� �̺�Ʈ ��ü�� ��ȣ ����, FALSE : 1�� �̺�Ʈ ��ü�� ��ȣ �����̸� ��� ���� */
										   m_u32SockTime,	/* ���� �ð� (�и���) �̳��� ������ ������ ���� */
										   FALSE);			/* WSAEventSelect������ �׻� FALSE */
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
	/* ��ü���� Network Event �˾Ƴ��� */
	i32WsaRet	= WSAEnumNetworkEvents(m_sdClient,			/* ��� ���� */
									   m_wsaClient,			/* ��� ���ϰ� ¦�� �̷� ��ü �ڵ� */
									   &wsaNet);			/* �̺�Ʈ ������ ����� ����ü ������ */

	/* ���� �߻��� �̺�Ʈ�� ���� �̺�Ʈ�� �ƴϸ� �������� �� ������ �õ� */
	if (FD_CONNECT != (FD_CONNECT & wsaNet.lNetworkEvents))
	{
#if 0	/* �ϴ� �Ʒ� CloseSocket �Լ� ȣ������ ���� (�������� �޾����� ����) */
		CloseSocket(m_sdClient);
#endif
		return FALSE;
	}
	else
	{
		/* ���� ������ �߻� �ߴ��� ���� */
		if (wsaNet.iErrorCode[FD_CONNECT_BIT] != 0)
		{
			TCHAR tzMesg[LOG_MESG_SIZE]	= {NULL};
			CUniToChar csCnv;
			/* ���� �޽��� ��� */
			m_i32SockError = WSAGetLastError();
			if (m_i32SockError == 0)	m_i32SockError = wsaNet.iErrorCode[FD_CONNECT_BIT];
			swprintf_s(tzMesg, LOG_MESG_SIZE,
					   L"Connection denied by remote system (%s) / err_cd:%d",
					   m_tzSourceIPv4, m_i32SockError);
			/* ������ ��Ŷ �۽� ������ �ð����� �ֱⰡ �ʹ� ª���� �θ𿡰� �˸��� ���� */
			LOG_ERROR(ENG_EDIC::en_engine, tzMesg);
			/* ���� ���� ���� ����ó�� */
			CloseSocket(m_sdClient);

			return FALSE;
		}
	}

	/* ���� �̺�Ʈ��� ��ȯ */
	return TRUE;
}

/*
 desc : ���� �̺�Ʈ ���� ���
 parm : None
 retn : 0x00 - ����, 0x01 - ����, 0x02 - ����, 0x03 - ������ ����
*/
UINT8 CClientThread::PopSockEvent()
{
	UINT8 u8RetCode	= 0x00;
	INT32 i32WsaRet	= 0;
	UINT32 u32WsaRet= 0;
	WSANETWORKEVENTS wsaNet;

	/* ----------------------------------------------------------------------------------------- */
	/* �̺�Ʈ ��ü�� ��ȣ ���°� �� ������ ����Ѵ�. WSAWaitForMultipleEvents() �Լ��� ��ȯ ���� */
	/* ��ȣ ���°� �� �̺�Ʈ ��ü�� �迭 �ε��� + WSA_WAIT_EVENT_0 ���̴�. �׷��Ƿ�, ���� �ε��� */
	/* ���� �������� WSA_WAIT_EVENT_0 (WAIT_BOJECT_0) ���� ���� �Ѵ�.                            */
	/* ----------------------------------------------------------------------------------------- */
	u32WsaRet	= WSAWaitForMultipleEvents(1,				/* ���� �̺�Ʈ ���� ����: 1 �� */
										   &m_wsaClient,	/* ���� �̺�Ʈ �ڵ� */
										   FALSE,			/* TRUE : ��� ���� �̺�Ʈ ��ü�� ��ȣ ����, FALSE : 1�� �̺�Ʈ ��ü�� ��ȣ �����̸� ��� ���� */
										   m_u32SockTime,	/* ���� �ð� (�и���) �̳��� ������ ������ ���� */
										   FALSE);			/* WSAEventSelect������ �׻� FALSE */
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
	/* ��ü���� Network Event �˾Ƴ��� */
	u32WsaRet	-= WSA_WAIT_EVENT_0;
	i32WsaRet	= WSAEnumNetworkEvents(m_sdClient,	/* ��� ���� */
									   m_wsaClient,	/* ��� ���ϰ� ¦�� �̷� ��ü �ڵ� */
									   &wsaNet);	/* �̺�Ʈ ������ ����� ����ü ������ */

	/* ���� ������ ���� �ӵ��� ���̱� ���� ó�� (������ ���Ÿ���� ���) */
	SetCycleTime(0);

	/* Socket Event �� �и� ó�� ���� */
	if (FD_READ == (FD_READ & wsaNet.lNetworkEvents))
	{
		/* ��� ��Ʈ�κ��� ������ �б� �õ� */
		if (wsaNet.iErrorCode[FD_READ_BIT] == 0)
		{
			if (Received(m_sdClient))	u8RetCode = 0x03;
		}
		/* ������ �б� ������ ���, ���� ��� ��Ʈ�� ���� ��� �о ������ */
		else
		{
			ReceiveAllData(m_sdClient);
		}
	}
	/* ������ ������ ���� Ȥ�� ��Ÿ �̺�Ʈ (Send or Connected)�� ���, ���� �������� ó�� */
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
 desc : Server���� ������ �̷���� ���, �ʱ� �ѹ� ȣ���
 parm : sd	- [in]  ������ �̷���� Client Socket Descriptor
 retn : None
*/
VOID CClientThread::Connected(SOCKET sd)
{
	BOOL bSucc		= TRUE;
	LONG32 l32Event	= FD_READ|FD_CLOSE;
	CUniToChar csCnv;

	/* �ϴ� ����Ǿ��ٴ� �ñ׳��� ���������Ƿ�, DoWork�� ����õ��� ���� ���� */
	m_enSocket	= ENG_TPCS::en_linked;

	/* ����Ǿ����Ƿ�, �������� ���� ���ϵ��� �ð��� ���� */
	UpdateWorkTime();
	// ���� �ɼ� �����ϱ�
	if (bSucc)	bSucc = SetSockOptBuffSize(0x11, sd);	/* 8 KBytes�� ǥ���� */
//	if (bSucc)	bSucc = SetSockOptSpeed(1, sd);
	if (bSucc)	bSucc = SetSockOptRoute(1, sd);
//	if (bSucc)	bSucc = SetSockOptAlive(1, sd);
	/* ���� �ɼ� ���� ���� */
	if (bSucc)
	{
		/* Setup socket event (Combination of FD_READ and FD_CLOSE) */
		bSucc = (SOCKET_ERROR != WSAEventSelect(sd, m_wsaClient, l32Event));
		/* ������ ���, ���� �ڵ� ���� ���� */
		if (!bSucc)	m_i32SockError	= WSAGetLastError();
	}

	/* ���� ����� �������̸� �����Լ� ȣ�� */
	if (bSucc)	bSucc	= Connected();
	/* ������ ���ӵ� �ð� ���� */
	if (bSucc)	GetLocalTime(&m_stLinkTime);

	/* ������ ���, ���� �޽��� ó�� */
	if (!bSucc)
	{
		CloseSocket(sd);	/* ������ ��Ŷ �۽� ������ �ð����� �ֱⰡ �ʹ� ª���� �θ𿡰� �˸��� ���� */
	}
	else
	{
		/* ���� �Ǿ��ٰ� �÷��� �� �޽��� ���� */
		m_bIsLinked	= TRUE;
#ifndef _DEBUG
		LOG_MESG(ENG_EDIC::en_engine, L"The connection to the remote system has been connected");
#endif
	}
}

