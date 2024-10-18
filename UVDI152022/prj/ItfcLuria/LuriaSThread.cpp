
/* desc : Luria Server Thread ��, CMPS: Server, Luria: Client */

#include "pch.h"
#include "LuriaSThread.h"

#include "LuriaCThread.h"	/* Client Thread */
/* Packet & User Data Family Object */
#include "JobManagement.h"
#include "Exposure.h"
#include "System.h"
#include "ComManagement.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : luria		- [in]  Luria ��� ����
		conf		- [in]  Luria ȯ�� ����
		shmem		- [in]  Luria Shared Memory
 retn : None
 ���� :	th_id		- [in]  Thread ID
		source_ip	- [in]  Local IPv4
		target_ip	- [in]  Remote IPv4
		tcp_port	- [in]  TCP/IP Port
		port_buff	- [in]  TCP/IP Port Buffer Size
		recv_buff	- [in]  Send or Recv�� ��Ŷ �� ���� ū ��Ŷ�� ũ�� (����: Bytes)
		ring_buff	- [in]	Max Recv Packet Size
		sock_time	- [in]  ���� �̺�Ʈ (��/����) ���� ��� �ð� (����: �и���)
							�ش� �־��� ��(�ð�)��ŭ�� ���� �̺�Ʈ ������ ���� �ܰ�� ��ƾ �̵�
		idle_time	- [in]  �־��� �ð� ���� ������ ����, Listen ������ ���ʱ�ȭ �ϴ� �ð� (����: �и���)

*/
CLuriaSThread::CLuriaSThread(LPG_CTCS luria,
							 LPG_CLSI conf,
							 LPG_LDSM shmem,
							 CLuriaCThread *client,
							 CJobManagement *job,
							 CExposure *exposure,
							 CSystem *system,
							 CComManagement *commgt)
	: CLuriaThread(shmem,
				   conf,
				   0x0000,
				   luria->tcp_port+1,	/* 55901 */
				   luria->source_ipv4,
				   luria->target_ipv4,
				   luria->port_buff,
				   luria->recv_buff,
				   luria->ring_buff,
				   luria->sock_time)
{
	m_u32IdleTime	= luria->link_time;	// Listen Socket �ֱ������� �ʱ�ȭ�ϴ� �ð� ���� (����: �и���)
	m_pClient		= client;
	m_pPktJM		= job;
	m_pPktEP		= exposure;
	m_pPktSS		= system;
	m_pPktCM		= commgt;
	m_u32AnnounceCount	= 0;
	// ���� �ʱ�ȭ �� �̺�Ʈ ����
	m_sdListen		= INVALID_SOCKET;
	m_sdClient		= INVALID_SOCKET;
	m_wsaListen		= WSACreateEvent();
	if (WSA_INVALID_EVENT == m_wsaListen)
		AfxMessageBox(L"Failed to create the socket event", MB_ICONSTOP|MB_TOPMOST);
	/* Check whether it operates in demo mode */
	if (GetConfig()->IsRunDemo())	m_enSysID = ENG_EDIC::en_demo;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CLuriaSThread::~CLuriaSThread()
{    
	/* Release Socket �� Event */
	if (INVALID_SOCKET != m_sdListen)		CloseSocket(m_sdListen);
	if (WSA_INVALID_EVENT != m_wsaListen)	WSACloseEvent(m_wsaListen);
}

/*
 desc : The main function of the thread
 parm : None
 retn : None
*/
VOID CLuriaSThread::RunWork()
{
	UINT8 u8PopRet	= 0x00;

	/* Server Binding */
	if (m_sdListen == INVALID_SOCKET)
	{
		if (!InitListen())
		{
			CloseSocket(m_sdClient);
			return;
		}
	}

	/* Socket Event ����ó�� */
	u8PopRet = PopSockEvent();
	if (u8PopRet == 1 || u8PopRet == 2)
	{
		if (0x01 == u8PopRet)	/* ����� Client�� �����ϴ� ��� */
		{
			/* 0. ���� ���� �ʱ�ȭ �Լ� ȣ�� �� */
			Accepted(m_sdListen);
			/* 1. ���� �Ǿ��ٰ� �÷��� ���� */
			m_bIsLinked	= TRUE;
		}
		else	/* ������ Client�� �����ϴ� ��� */
		{
			TCHAR tzMesg[128]	= {NULL};
			swprintf_s(tzMesg, 128,
					   L"Disconnected from the remote system (src:%s tgt:%s) / err_cd:%d",
					   m_tzSourceIPv4, m_tzTargetIPv4, m_i32SockError);
			LOG_ERROR(m_enSysID, tzMesg);
		}
	}

	/* ���ŵ� �����Ͱ� �����ϴ��� �ֱ������� Ȯ�� */
	LPG_PCLR pstPktRecv = m_pRecvQue->PopPktData();
	if (pstPktRecv)
	{
		PktAnalysis(pstPktRecv);
		/* user_data �޸� �Ҵ� ���� */
		if (pstPktRecv && pstPktRecv->user_data)	delete [] pstPktRecv->user_data;
	}
}

/*
 desc : Release tcp/ip socket
 parm : sd		- [in]  Socket descriptor
		time_out- [in]  Please refer to the LINGER structure (= linger time)
						If the value is 0, I disconnected immediately without any delay
 retn : None
*/
VOID CLuriaSThread::CloseSocket(SOCKET sd, UINT32 time_out)
{
	/* --------------------------------------------------------- */
	/* �θ� ���� CloseSocket �Լ� ȣ���ϱ� ���� ������ ���� ���� */
	/* --------------------------------------------------------- */
	/* ���� �θ� �Լ� ȣ�� */
	CLuriaThread::CloseSocket(sd, time_out);

	/* Socket ������ ���� �ʱ�ȭ ��� �ٸ��� ó�� */
	if (sd == m_sdListen)	m_sdListen	= INVALID_SOCKET;
	if (sd == m_sdClient)	m_sdClient	= INVALID_SOCKET;
}

/*
 desc : ���� �ʱ�ȭ ��, Server�� Listen socket �ʱ�ȭ �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLuriaSThread::InitListen()
{
	TCHAR tzMesg[128]	= {NULL};
	INT32 i32Option		= 1;
	BOOL bSucc			= TRUE;
	LONG32 l32Event		= FD_ACCEPT;
	SOCKADDR_IN stAddrIn= {NULL};

	/* ���� Listen ������ �ʱ�ȭ���� �ʾҴٸ� ... */
	if (INVALID_SOCKET != m_sdListen)
	{
		CloseSocket(m_sdListen);
	}
	/* ���� Client ������ �ʱ�ȭ���� �ʾҴٸ� ... */
	if (INVALID_SOCKET != m_sdClient)
	{
		CloseSocket(m_sdClient);
	}

	// Setupt the self socket
	stAddrIn.sin_family		= AF_INET;
	stAddrIn.sin_port		= htons(m_u16TcpPort);
	/* �������� Land Card�� IPv4 �ּ� �Է��ϸ� ������, �������� �ο��� IPv4�� ���� �� */
#if 0
//	InetPtonW(AF_INET, m_tzSourceIPv4, &stAddrIn.sin_addr.s_addr);
//	InetPtonW(AF_INET, L"192.168.105.35", &stAddrIn.sin_addr.s_addr);
//	InetPtonW(AF_INET, L"100.100.100.43", &stAddrIn.sin_addr.s_addr);
#else
	stAddrIn.sin_addr.s_addr= htonl(INADDR_ANY);
#endif

	/* ���� ���� */
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
	/* ���� �ɼ� ���� - IP �ּ� ���� */
	if (bSucc)
	{
		bSucc = (SOCKET_ERROR != setsockopt(m_sdListen,
											SOL_SOCKET,
											SO_REUSEADDR,
											(CHAR *)&i32Option,
											sizeof(i32Option)));
		if (!bSucc)
		{
			swprintf_s(tzMesg, 128, L"Socket Option: Error setting socket reuse "
								    L"[Error:0x%x, Source:%s / Target:%s]",
					   WSAGetLastError(), m_tzSourceIPv4, m_tzTargetIPv4);
			LOG_ERROR(m_enSysID, tzMesg);
		}
	}
	/* ���� �� ������ �ּ� ���ε� */
	if (bSucc)
	{
		bSucc = (SOCKET_ERROR != ::bind(m_sdListen, (LPSOCKADDR)&stAddrIn, sizeof(SOCKADDR_IN)));
		if (!bSucc)
		{
			swprintf_s(tzMesg, 128, L"Socket Option: Socket bind error "
									L"[Error:0x%x, Source:%s / Target:%s]",
					   WSAGetLastError(), m_tzSourceIPv4, m_tzTargetIPv4);
			LOG_ERROR(m_enSysID, tzMesg);
		}
	}
	/* ���� ��� ����. ���� 5 ���� ���� ���� */
	if (bSucc)
	{
		bSucc = (SOCKET_ERROR != listen(m_sdListen, 5));
		if (!bSucc)
		{
			swprintf_s(tzMesg, 128, L"Socket Option: Socket listen error "
									L"[Error:0x%x, Source:%s / Target:%s]",
					   WSAGetLastError(), m_tzSourceIPv4, m_tzTargetIPv4);
			LOG_ERROR(m_enSysID, tzMesg);
		}
	}
	/* ���� �̺�Ʈ ���� (FD_ACCEPT�� ����!!!) */
	if (bSucc)
	{
		bSucc = (SOCKET_ERROR != WSAEventSelect(m_sdListen, m_wsaListen, l32Event));
	}

	UpdateWorkTime();

	/* ������ ��� */
	if (!bSucc)
	{
		m_sdListen	= INVALID_SOCKET;
		m_enSocket	= ENG_TPCS::en_link_failed;
	}
	else
	{
		/* ���� ������ ��ٸ��� �ִٰ� ���� */
		m_enSocket	= ENG_TPCS::en_linking;
	}

	/* �θ𿡰� �˸� */
	return bSucc;
}

/*
 desc : ���� �̺�Ʈ ����
 parm : None
 retn : 0x00 - ����, 0x01 - ����, 0x02 - ����, 0x03 - ������ ����
*/
UINT8 CLuriaSThread::PopSockEvent()
{
	UINT8 u8RetCode		= 0x00;
	INT32 i32WsaRet		= 0;
	UINT32 u32WsaRet	= 0;
	SOCKET sdSock[2]	= {m_sdListen, m_sdClient};
	WSAEVENT wsaSock[2]	= {m_wsaListen, m_wsaClient};
	WSANETWORKEVENTS wsaNet;

	/* Socket Event�� Signal ���� �����ϱ� */
	u32WsaRet	= WSAWaitForMultipleEvents(2,				/* ���� �̺�Ʈ ���� ����: 1���� */
										   wsaSock,			/* ���� �̺�Ʈ �ڵ� (Listen or Client) */
										   FALSE,			/* TRUE : ��� ���� �̺�Ʈ ��ü�� ��ȣ ����, FALSE : 1�� �̺�Ʈ ��ü�� ��ȣ �����̸� ��� ���� */
										   m_u32SockTime,	/* ���� �� �̳��� ������ ������ ���� */
										   FALSE);			/* WSAEventSelect������ �׻� FALSE */
#if 1
	if (u32WsaRet == WSA_WAIT_TIMEOUT || u32WsaRet == WSA_WAIT_FAILED)
#else
	if (u32WsaRet == WSA_WAIT_TIMEOUT)
#endif
	{
		if (u32WsaRet == WSA_WAIT_FAILED)
		{
			TCHAR tzMesg[256]	= {NULL};
			swprintf_s(tzMesg, 256, L"The return value of the WSAWaitForMultipleEvents failed. "
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
	i32WsaRet	= WSAEnumNetworkEvents(sdSock[u32WsaRet],	/* ��� ���� */
									   wsaSock[u32WsaRet],	/* ��� ���ϰ� ¦�� �̷� ��ü �ڵ� */
									   &wsaNet);			/* �̺�Ʈ ������ ����� ����ü ������ */
	/* ������ �������� Ȯ�� */
	if (i32WsaRet == SOCKET_ERROR)	return 0x00;

	/* Socket Event �� �и� ó�� ���� */
	if (FD_READ == (FD_READ & wsaNet.lNetworkEvents))
	{
		/* ��� ��Ʈ�κ��� ������ �б� �õ� */
		if (wsaNet.iErrorCode[FD_READ_BIT] == 0)
		{
			if (Received(sdSock[1]))	u8RetCode = 0x03;
		}
		/* ������ �б� ������ ���, ���� ��� ��Ʈ�� ���� ��� �о ������ */
		else
		{
			ReceiveAllData(sdSock[1]);
		}
	}
	/* Ŭ���̾�Ʈ�� ������ ���� �������� ... */
	else if (FD_ACCEPT == (FD_ACCEPT & wsaNet.lNetworkEvents))
	{
		/* ���� ������ �߻� �ߴ��� ���� */
		if (wsaNet.iErrorCode[FD_ACCEPT_BIT] != 0)
		{
			m_i32SockError = WSAGetLastError();
		}
		/* ������ �����κ��� ���������� ���ι��� ��� */
		else
		{
			u8RetCode	= 0x01;
		}
	}
	/* ������ ������ �������� */
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
VOID CLuriaSThread::Accepted(SOCKET sd)
{
	TCHAR tzMesg[128]	= {NULL};
	INT32 i32AddrLen	= sizeof(SOCKADDR_IN);
	BOOL bSucc			= TRUE;
	LONG32 l32Event		= FD_READ|FD_CLOSE;
	SOCKET sdClient		= INVALID_SOCKET;
	IN_ADDR stInAddr	= {NULL};
	SOCKADDR_IN sdAddrIn= {NULL};

	/* ----------------------------------------------------------- */
	/* ����� Ŭ���̾�Ʈ�� �����Ƿ�, �ϴ� ����Ǿ��ٰ� �÷��� ���� */
	/* ----------------------------------------------------------- */
	m_enSocket = ENG_TPCS::en_linked;
	/* ------------------------------------------------------- */
	/* ����Ǿ����Ƿ�, ���� �ʱ�ȭ�� ���� ���ϵ��� �ð��� ���� */
	/* ------------------------------------------------------- */
	UpdateWorkTime();

	/* ------------------------------------------------- */
	/* ���� ������ ���ӵ� Client�� �ִٸ� ���� ���� ó�� */
	/* ------------------------------------------------- */
	if (INVALID_SOCKET != m_sdClient)
	{
#if 0	/* Announcement ���� �����ϰԵǸ�, ����ϰ� Luria Server�� �Ϲ������� ��� ����, �ٽ� �������� �� */
		swprintf_s(tzMsg, 128,
				   L"Previously connected to a remote computer [Local:%s, Remote:%s]",
				   m_tzSourceIPv4, m_tzTargetIPv4);
		LOG_ERROR(m_enSysID, tzMsg);
		CloseSocket(m_sdClient);
#else
		/* ���� ���� �� �ʱ�ȭ */
		closesocket(m_sdClient);
		m_sdClient	= INVALID_SOCKET;
#endif
	}

	/* ���Ӱ� ���ӵ� Ŭ���̾�Ʈ�� ���� �ϴ� ���� ���� ó�� */
	sdClient	= WSAAccept(sd, (LPSOCKADDR)&sdAddrIn, &i32AddrLen, 0, 0);
	memcpy(&stInAddr.s_addr, &sdAddrIn.sin_addr.s_addr, sizeof(UINT32));
	/* ���� ������ ��, ������ �߻� �ߴ��� ���� Ȯ�� */
	if (INVALID_SOCKET == sdClient)
	{
		swprintf_s(tzMesg, 128,	L"Connection approval oeration failed "
								L"[Error:0x%0x, Local:%s, Remote:%s]",
				   WSAGetLastError(), m_tzSourceIPv4, m_tzTargetIPv4);
		LOG_ERROR(m_enSysID, tzMesg);
		return;
	}
#if 0	/* Luria Server �κ��� ����� ������ ����. �뷫 ���� */
	/* -------------------------------------------------------------------------- */
	/* ���ӵ� Ŭ���̾�Ʈ�� ������ ��ϵ� Client IPv4 �ּҰ� �ƴϸ� ���� ���� ó�� */
	/* -------------------------------------------------------------------------- */
	InetNtopW(AF_INET, (CONST VOID*)&stInAddr, tzIPv4, IPv4_LENGTH);
	if (bSucc && (0 != wcscmp(m_tzTargetIPv4, tzIPv4)))
	{
		// ���� ����� Client ���� ��ü ó��
		CloseSocket(sdClient);
		swprintf_s(tzMsg, 128, L"The approved client is not an authorized system "
							   L"[Local:%s, Remote:%s]", m_tzSourceIPv4, m_tzTargetIPv4);
		LOG_ERROR(m_enSysID, tzMsg);
		return;
	}
#endif
	/* --------------------------------------- */
	/* ���� ����� ������ �پ��� �ɼ� �����ϱ� */
	/* --------------------------------------- */
	if (bSucc)	bSucc = SetSockOptBuffSize(0x11, sdClient);	/* 8 KBytes�� ǥ���� */
//	if (bSucc)	bSucc = SetSockOptSpeed(1, sdClient);
	if (bSucc)	bSucc = SetSockOptRoute(1, sdClient);
//	if (bSucc)	bSucc = SetSockOptAlive(1, sdClient);
	if (!bSucc)
	{
		swprintf_s(tzMesg, 128, L"Socket Option Settings failed "
								L"[Error:0x%0x, Local:%s, Remote:%s]",
				   WSAGetLastError(), m_tzSourceIPv4, m_tzTargetIPv4);
		LOG_ERROR(m_enSysID, tzMesg);
	}
	else
	{
		/* ���� �̺�Ʈ ���� (FD_CLOSE|FD_READ ����!!!) */
		bSucc = (SOCKET_ERROR != WSAEventSelect(sdClient, m_wsaClient, l32Event));
		if (!bSucc)
		{
			LOG_ERROR(m_enSysID, L"Socket Asynchronous Setting (FD_CLOSE|FD_READ) failed");
		}
#if 0
		/* �ݵ�� �� ������ �� ��*/
		else bSucc = TRUE;
#endif
	}

	/* Ring Buffer �ʱ�ȭ */
	m_pPktRingBuff->ResetBuff();
	/* ��� �ð� ���� */
	UpdateWorkTime();

	/* ���� ����� �������̸� �����Լ� ȣ�� */
	if (!bSucc)
	{
		CloseSocket(sdClient);
	}
	else
	{
		/* ���� �ֱٿ� ������ Client�� ���� �ڵ� ���� */
		m_sdClient	= sdClient;	// �ݵ�� Accepted �Լ� ������ ����(����)�Ǿ�� ��
		/* Ŭ���̾�Ʈ�� ������ �ð� ���� */
		GetLocalTime(&m_stLinkTime);
	}
}

/*
 desc : ���� ����� Client Socket ���� ���� ó��
 parm : None
 retn : None
*/
VOID CLuriaSThread::CloseClient()
{
	if (INVALID_SOCKET != m_sdClient)
	{
		CloseSocket(m_sdClient);
	}
}

/*
 desc : ���� Server Socket ���� ó��
 parm : None
 retn : None
*/
VOID CLuriaSThread::CloseServer()
{
	if (INVALID_SOCKET != m_sdListen)	CloseSocket(m_sdListen);
	if (INVALID_SOCKET != m_sdClient)	CloseSocket(m_sdClient);
}

/*
 desc : ���ŵ� ������ �м� ó��
 parm : data	- [in]  ���ŵ� �����Ͱ� ����� ����ü ������
 retn : TRUE or FALSE
*/
BOOL CLuriaSThread::PktAnalysis(LPG_PCLR data)
{
	TCHAR tzMesg[512]	= {NULL};
	INT32 i32Size		= 0;

	/* Announcemnet�� �ƴ��� ���� Ȯ�� */
	if (data->user_data_family != (UINT8)ENG_LUDF::en_announcement)
	{
		swprintf_s(tzMesg, 128, L"Invalid family ID value. f_id = 0x%02x [Local:%s, Remote:%s]",
				   data->user_data_family, m_tzSourceIPv4, m_tzTargetIPv4);
		LOG_WARN(m_enSysID, tzMesg);
		return TRUE;
	}

	/* �۽� or ���ŵ� User Data �� ũ�� */
	i32Size	= data->num_bytes - (sizeof(STG_PCLR) - sizeof(PUINT8));
	if (i32Size < 0)
	{
		LOG_WARN(m_enSysID, L"Received packet header too small");
		return TRUE;
	}

	/* ������ Announcement ��Ŷ�� ���� ó�� */
	if (/*data->user_data_id != (UINT8)ENG_LCEA::en_error &&*/
		(UINT8)ENG_LUDF::en_announcement == data->user_data_family)
	{
		switch (data->user_data_id)
		{
		case (INT32)ENG_LCEA::en_test_announcement	:	if (UINT_MAX == ++m_u32AnnounceCount)	m_u32AnnounceCount = 0;		break;
#if 0
		case (INT32)ENG_LCEA::en_error				:	ReqSystemStatus();								break;
#else
		case (INT32)ENG_LCEA::en_error				:	SetPktError(ENG_LNWE::en_error, 0x01, data);	break;
#endif
		case (INT32)ENG_LCEA::en_load_state			:	m_pPktJM->SetJobLoadState(data->user_data);		break;
		case (INT32)ENG_LCEA::en_exposure_state		:	m_pPktEP->SetExposureState(data->user_data);	break;
		}
	}

	/* Received Data */
	swprintf_s(tzMesg, 512, L"Service (Photohead) Announcement [%u] Received. "
							L"f_id = 0x%02x d_id = [0x%02x] [Local:%s, Remote:%s] [Status=0x%04x]",
			   m_u32AnnounceCount, data->user_data_family, data->user_data_id,
			   m_tzSourceIPv4, m_tzTargetIPv4, data->status);
	LOG_MESG(m_enSysID, tzMesg);

	return TRUE;
}

/*
 desc : �ý��� ���� ���� ��û
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLuriaSThread::ReqSystemStatus()
{
	UINT32 u32Size	= 0;
	PUINT8 pPkt		= NULL;

	/* System ���� ���� ��û ��Ŷ ���� */
	pPkt	= m_pPktSS->GetPktSystemStatus();
	u32Size	= m_pPktSS->GetPktSize();
	m_pClient->AddPktSend(pPkt, u32Size);
	delete pPkt;

	return TRUE;
}

/*
 desc : AnnouncementStatus ��û
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLuriaSThread::ReqAnnouncementStatus()
{
	UINT32 u32Size	= 0;
	PUINT8 pPkt		= NULL;

	/* System ���� ���� ��û ��Ŷ ���� */
	pPkt	= m_pPktCM->GetPktAnnouncementStatus();
	u32Size	= m_pPktCM->GetPktSize();
	if (m_pClient)	m_pClient->AddPktSend(pPkt, u32Size);
	delete pPkt;

	return TRUE;
}