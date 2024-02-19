/* desc : Luria Server Thread ��, CMPS: Client, Luria: Server */

#include "pch.h"
#include "MainApp.h"
#include "LuriaCThread.h"

/* Packet & User Data Family Object */
#include "MachineConfig.h"
#include "System.h"
#include "JobManagement.h"
#include "PanelPreparation.h"
#include "Exposure.h"
#include "DirectPhComn.h"
#include "ComManagement.h"
#include "PhZFocus.h"

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
		object		- [in]  ���� Luria Objects
 retn : None
 note : source_ip	- [in]  Local IPv4
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
		common		- [in]  Luria Common Config
		init_send	- [in]  ���� ���� �ʱ�ȭ ����� �۽����� ����
		<<< luria_family_object >>>
*/
CLuriaCThread::CLuriaCThread(LPG_CTCS luria,
							 LPG_CLSI conf,
							 LPG_LDSM shmem,
							 CMachineConfig *machine,
							 CJobManagement *job,
							 CPanelPreparation *panel,
							 CExposure *exposure,
							 CDirectPhComn *direct,
							 CSystem *system,
							 CComManagement *commgr,
							 CPhZFocus *focus,
							 BOOL init_send)
	: CLuriaThread(shmem,
				   conf,
				   0x0000,
				   luria->tcp_port,	/* 55900 */
				   luria->source_ipv4,
				   luria->target_ipv4,
				   luria->port_buff,
				   luria->recv_buff,
				   luria->ring_buff,
				   luria->sock_time)
{
	/* ���� ����� �� �ʱ�ȭ���� �ʾҴٰ� ���� */
	m_bConnectInited	= FALSE;
	m_bWorkBusy			= FALSE;
	m_u32LinkTime		= luria->link_time;	/* �������ϴ� �ֱ� ���� (����: �и���) */
	m_u32IdleTime		= luria->idle_time;	/* �ش�ð� ���� ��� �۾��� ������ ���� ��� ���� ���� */
	m_u64RecvTime		= 0;
	m_u8NextJob			= 0x00;
	m_bInitSend			= init_send;
#if 0	/* GUI �ʿ��� �ʿ��� ������ ȣ���ؼ� �����ϴ� ������ �ؾ� �� ��... ���⼭�� �ʿ� ���� */
	m_u8NextPh			= 0x01;		/* ������� 1 �� ���� ��û */
	m_u8NextLed			= 0x00;		/* 0 ���� 5���� �ݺ��� (����� 1 ~ 4������ �����) */
#endif
	/* The objects of user data family */
	m_pPktMC			= machine;
	m_pPktJM			= job;
	m_pPktPP			= panel;
	m_pPktEP			= exposure;
	m_pPktDP			= direct;
	m_pPktSS			= system;
	m_pPktCM			= commgr;
	m_pPktPF			= focus;
	/* �ֱ������� Luria���� ��û�ϴ� ���� �ð� �ʱ�ȭ */
	m_u64ReqLedOnTime	= 0;	/* ó�� Led On Time ��û�� ���� �ٷ� ȣ���� �� �ֵ��� �ϱ� ���� */
	m_u64ReqPeriodTime	= 0;

	/* �߿�!!! */
	if (init_send)	m_bLuriaInited	= FALSE;	/* Luria Service�� ����ǰ� �ʱ�ȭ ���� �ʾҴٰ� ���� */
	else			m_bLuriaInited	= TRUE;		/* Luria Service�� ����ǰ� �ʱ�ȭ ���� �ʾҴٰ� ���� */
	/* Check whether it operates in demo mode */
	if (GetConfig()->IsRunDemo())	m_enSysID = ENG_EDIC::en_demo;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CLuriaCThread::~CLuriaCThread()
{
	RemoveAllSendPkt();
}

/*
 desc : ���� �۽ŵ� ��Ŷ ��� ����
 parm : None
 retn : None
*/
VOID CLuriaCThread::RemoveAllSendPkt()
{
	POSITION pPos, pPrePos;
	STM_LPSB stPktData;

	/* ��Ŷ �۽� �ӽ� ���� �޸� ���� (���� / �ֱ��� �۽� ��Ŷ ����Ʈ ���) */
	pPos	= m_lstPktSend.GetHeadPosition();
	while (pPos)
	{
		pPrePos	= pPos;
		stPktData	= m_lstPktSend.GetNext(pPos);
		if (stPktData.pkt_buff)	::Free(stPktData.pkt_buff);
		m_lstPktSend.RemoveAt(pPrePos);
	}
}

/*
 desc : �ֱ������� ȣ���
 parm : None
 retn : None
*/
VOID CLuriaCThread::RunWork()
{
	UINT64 u64Tick	= GetTickCount64();

	/* ------------------------------------------------------- */
	/* ���� ����� Client�� �������� ������, ��-���� �۾� ���� */
	/* �ֱٿ� ���ŵ� �����Ͱ� ���� ���, ��� ���� ������ ���� */
	/* ���� ���� : ���� �ֱ������� ��Ŷ�� ���۵Ǵ� ���ǿ� ���� */
	/* ------------------------------------------------------- */
	if ((m_u32IdleTime > 0 &&
		(m_u64RecvTime + m_u32IdleTime < u64Tick)) &&	/* ȯ�� ������ ����� �ð����� ������ ������ ... */
		(!m_bIsLinked/*m_enSocket == ENG_TPCS::en_closed || m_enSocket == ENG_TPCS::en_linking*/))
	{
#if 0
		if (m_u64RecvTime)
		{
			TCHAR tzMesg[128]	= {NULL};
			swprintf_s(tzMesg, 128, L"Try reconnecting to the remote system (idle_time:%.3f sec)",
					   m_u32IdleTime/1000.0f);
			LOG_ERROR(m_enSysID, tzMesg);
		}
#endif
		/* Try to connect */
		SetConnect();

		return;
	}

	/* Socket Event ����ó�� */
	if (0x02 == PopSockEvent())
	{
		TCHAR tzMesg[128]	= {NULL};
		CUniToChar csCnv1, csCnv2;
		swprintf_s(tzMesg, 128,
				   L"Disconnected from the remote system (src:%s tgt:%s) / err_cd:%d",
				   m_tzSourceIPv4, m_tzTargetIPv4, m_i32SockError);
		LOG_ERROR(m_enSysID, tzMesg);
		if (m_sdClient != INVALID_SOCKET)
		{
			CloseSocket(m_sdClient);
		}
		return;
	}

	/* ���ŵ� ������ ������, ó�� */
	PopPktData(u64Tick);

	/* ���� ���� */
	if (m_syncPktSend.Enter())
	{
		/* ���� �۽ŵ� �����Ͱ� �ִ��� ������ �۽� (�� �̻� �۽ŵ� �����Ͱ� ������, �ֱ������� �۽ŵ� ������ ó��) */
		if (!ReqPeriodSendPacket())
		{
			ReqGetPeriodPkt(u64Tick);
		}
		/* ���� ���� */
		m_syncPktSend.Leave();
	}
}
#if 1
/*
 desc : �ֱ������� ��Ŷ ��û
 parm : tick	- [in]  ���� CPU �ð�
 retn : None
*/
VOID CLuriaCThread::ReqGetPeriodPkt(UINT64 tick)
{
	PUINT8 pPkt			= NULL;
	CBaseFamily *pBase	= NULL;

	/* !!! ���� �뱤 ������ �ʴ��� ���� Ȯ�� !!! */
	if (!m_pstShMemLuria->exposure.IsWorkIdle())	return;
	/* !!! ���� �Ź� ���� ������ �ʴ��� ���� Ȯ�� !!! */
	if (m_pstShMemLuria->jobmgt.job_total_strip_loaded > 0 &&
		m_pstShMemLuria->jobmgt.job_state_strip_loaded !=
		m_pstShMemLuria->jobmgt.job_total_strip_loaded)
	{
		return;
	}
	/* ���� ���������� Luria ���� �۾��� ���� ���� ���, �ֱ������� �۽��ϴ� �� ���� */
	/* Luria�� ���ϸ� �ִ� ���, Luria �� �״� ������ �߻� �� */
	if (m_bWorkBusy)	return;

	/* ���� ���⼭ Time Delay ����� �ʿ� ����. ���������� �̹� �����̰� ���� */
	if (tick < m_u64ReqPeriodTime + REQ_LURIA_PERIOD_TIME * 2)	return;
	/* ���� �ֱٿ� �۽��� �ð� ���� */
	m_u64ReqPeriodTime	= tick;

	switch (m_u8NextJob)
	{
	case 0x00 :	pPkt = m_pPktSS->GetPktSystemStatus();			pBase = m_pPktSS;	break;
	case 0x01 :	pPkt = m_pPktDP->GetPktMotorAbsPositionAll();	pBase = m_pPktDP;	break;
	case 0x02 :	if (0x01 == m_pstConfLuria->z_drive_type)	/* Only for stepping motors */
					pPkt = m_pPktDP->GetPktMotorStateAll();		pBase = m_pPktDP;	break;
	}

	/* ��Ŷ�� ���������� ��������ٸ� */
	if (pPkt)
	{
		AddPktSend(pPkt, pBase->GetPktSize());
		::Free(pPkt);
	}

	/* ���� ��Ŷ �۽��� ���� ���� ��Ŵ */
	if (0x03 == ++m_u8NextJob)	m_u8NextJob	= 0x00;
}
#else
/*
 desc : �ֱ������� ��Ŷ ��û
 parm : tick	- [in]  ���� CPU �ð�
 retn : None
*/
VOID CLuriaCThread::ReqGetPeriodPkt(UINT64 tick)
{
	UINT32 u32Size	= 0;
	PUINT8 pPkt		= NULL;

	/* ���� ���⼭ Time Delay ����� �ʿ� ����. ���������� �̹� �����̰� ���� */
	if (tick < m_u64ReqPeriodTime + REQ_LURIA_PERIOD_TIME)	return;
	/* ���� �ֱٿ� �۽��� �ð� ���� */
	m_u64ReqPeriodTime	= tick;
#if 0
	/* Announcement Status ��û */
	pPkt	= m_pPktCM->GetPktAnnouncementStatus();
	if (pPkt)	u32Size	= m_pPktCM->GetPktSize();
#else
	/* Test Announcement */
	pPkt	= m_pPktCM->GetPktGenTestAnnouncement();
	if (pPkt)	u32Size	= m_pPktCM->GetPktSize();
#endif
	/* ��Ŷ�� ���������� ��������ٸ� */
	if (pPkt)
	{
		AddPktSend(pPkt, u32Size);
		::Free(pPkt);
	}
}
#endif
/*
 desc : Release tcp/ip socket
 parm : sd		- [in]  Socket descriptor
		time_out- [in]  Please refer to the LINGER structure (= linger time)
						If the value is 0, I disconnected immediately without any delay
 retn : None
*/
VOID CLuriaCThread::CloseSocket(SOCKET sd, UINT32 time_out)
{
	/* --------------------------------------------------------- */
	/* �θ� ���� CloseSocket �Լ� ȣ���ϱ� ���� ������ ���� ���� */
	/* --------------------------------------------------------- */
	m_bConnectInited	= FALSE;
	/* ���� ���� ���� ���� */
	UpdateLinkTime(0x00);

	/* ���� �θ� �Լ� ȣ�� */
	CLuriaThread::CloseSocket(sd, time_out);
	/* Socket ������ ���� �ʱ�ȭ ��� �ٸ��� ó�� */
	if (sd == m_sdClient)	m_sdClient	= INVALID_SOCKET;
}

/*
 desc : Perform a bind operation for the client socket to connect to the remote server
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLuriaCThread::BindSock()
{
	TCHAR tzMesg[128]	= {NULL};
	SOCKADDR_IN stAddrIn= {NULL};

	if (INVALID_SOCKET == m_sdClient)	return FALSE;

	/* Setup the socket information */
	stAddrIn.sin_family	= AF_INET;
	stAddrIn.sin_port	= 0;	/* In this case, the port number is a value of 0 at all times */
	InetPtonW(AF_INET, m_tzSourceIPv4, &stAddrIn.sin_addr.s_addr);
	/* Socket binding ... */
	if (SOCKET_ERROR == ::bind(m_sdClient, (LPSOCKADDR)&stAddrIn, sizeof(SOCKADDR_IN)))
	{
		swprintf_s(tzMesg, 128, L"CLuria::BindSock. Failed to bind the server socket [err_cd : %d]",
				   m_i32SockError);
		SocketError(tzMesg);
		return FALSE;
	}
	return TRUE;
}

/*
 desc : ���� ���� Ȯ���ϰ�, ���� �ȵǾ� ������, ���� �۾� ����
		���� ���� �ȵ� ���¿��� ����Ǿ�����, ���� �� �ʱ�ȭ �۾� ����
 parm : None
 retn : None
*/
VOID CLuriaCThread::SetConnect()
{
	/* �߿�!!! */
	if (m_bInitSend)	m_bLuriaInited	= FALSE;	/* Luria Service�� ����ǰ� �ʱ�ȭ ���� �ʾҴٰ� ���� */
	else				m_bLuriaInited	= TRUE;	/* Luria Service�� ����ǰ� �ʱ�ȭ ���� �ʾҴٰ� ���� */
	/* ���Ź��� �ð� ���� */
	m_u64RecvTime	= GetTickCount64();

	/* ------------------------------------------------- */
	/* ���� ����� Client�� �������� ������, ������ ���� */
	/* ------------------------------------------------- */
	/* ������ �ð����� �������� �����ϵ��� �ϱ� ���� */
	if (IsTcpWorkConnect(m_u32LinkTime))
	{
		/* ���� �۾��� ���� Client ������ �ʱ�ȭ �ߴٰ� �˸� */
		if (!DoConnect())	m_i32SockError = WSAGetLastError();
	}

	/* Socket Event ����ó�� (����Ǿ��ٸ�, �����Լ� ȣ��) */
	if (m_enSocket == ENG_TPCS::en_linking)
	{
		if (IsLinkEvent())
		{
			/* ���� �ֱ��� �۽� ��� �÷��� �޸� �ʱ�ȭ */
			m_pstShMemLuria->ResetLastRecvCmd();
			/* ���� �Ǿ��ٰ� �÷��� ���� */
			m_bIsLinked		= TRUE;
			/* ���� ���� �ʱ�ȭ �Լ� ȣ�� �� */
			Connected(m_sdClient);
		}
	}

	/* �ݵ�� ����. ���ŵ��� �ʾ�����. ����õ��̹Ƿ�, ���ŵǾ��ٰ� ���� */
	m_u64RecvTime	= GetTickCount64();
}

/*
 desc : Attempt to connect to remote server to initialize client socket
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLuriaCThread::DoConnect()
{
	INT32 i32Option		= 1;
	BOOL bSucc			= FALSE;
	LONG32 l32Event		= FD_CONNECT;	// FD_CLOSE
	SOCKADDR_IN stAddrIn= {NULL};

	/* Server or Client�� Listen or Client Socket ���ʱ�ȭ�� ���� �ð� ���� */
	m_u64ConnectTime	= GetTickCount64();

	/* Ring Buffer �ʱ�ȭ */
	m_pPktRingBuff->ResetBuff();

	/* Remove existing client socket exists */
	if (INVALID_SOCKET != m_sdClient)
	{
		CloseSocket(m_sdClient);
	}

	/* ������ �õ��ϹǷ� �÷��� ���� */
	m_enSocket				= ENG_TPCS::en_linking;
	/* Setup the server socket information */
	memset(&stAddrIn, 0x00, sizeof(SOCKADDR_IN));
	stAddrIn.sin_family		= AF_INET;
	stAddrIn.sin_port		= htons(m_u16TcpPort);
	InetPtonW(AF_INET, m_tzTargetIPv4, &stAddrIn.sin_addr.s_addr);
	/* Create a client socket */
#if 0
	bSucc = (INVALID_SOCKET != (m_sdClient = WSASocket(AF_INET,
													   SOCK_STREAM,
													   IPPROTO_TCP,
													   NULL,
													   NULL,
													   WSA_FLAG_OVERLAPPED)));
#else
	bSucc = (INVALID_SOCKET != (m_sdClient = socket(AF_INET, SOCK_STREAM, 0)));
#endif
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
		TCHAR tzMesg[128]	= {NULL};
		swprintf_s(tzMesg, 128, L"Attempt to connect to the remote system (src:%s tgt:%s)",
				   m_tzSourceIPv4, m_tzTargetIPv4);
		LOG_MESG(m_enSysID, tzMesg);
		/* ------------------------------------------------------------------------------------- */
		bSucc = (SOCKET_ERROR != connect(m_sdClient, (LPSOCKADDR)&stAddrIn, sizeof(stAddrIn)));
		/* ------------------------------------------------------------------------------------- */
		if (!bSucc)
		{
			bSucc = (WSAGetLastError() == WSAEWOULDBLOCK);
			if (!bSucc)	SocketError(L"CLuriaThread::DoConnect. Failed to initialize the connection");
		}
	}

	return bSucc;
}

/*
 desc : ���� ���� �̺�Ʈ�� �ִ��� Ȯ��
 parm : None
 retn : TRUE - ���� �̺�Ʈ �߻�, FALSE - ���� �̺�Ʈ ����
*/
BOOL CLuriaCThread::IsLinkEvent()
{
	INT32 i32WsaRet	= 0;
	UINT32 u32WsaRet= 0;
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
			swprintf_s(tzMesg, 256, L"The return value of the WSAWaitForMultipleEvents failed. "
									L"(time out) (tgt_ipv4=%s)", m_tzTargetIPv4);
			if (m_enSysID != ENG_EDIC::en_demo)	LOG_MESG(m_enSysID, tzMesg);
			SetCycleTime(NORMAL_THREAD_SPEED);
		}
		return FALSE;
	}
	/* ��ü���� Network Event �˾Ƴ��� */
	u32WsaRet	-= WSA_WAIT_EVENT_0;
	i32WsaRet	= WSAEnumNetworkEvents(m_sdClient,			/* ��� ���� */
									   m_wsaClient,			/* ��� ���ϰ� ¦�� �̷� ��ü �ڵ� */
									   &wsaNet);			/* �̺�Ʈ ������ ����� ����ü ������ */
	/* ������ �������� Ȯ�� */
	if (i32WsaRet == SOCKET_ERROR)
	{
		SocketError(L"CLuriaThread::IsLinkEvent. An error occurred in the WSAEnumNetworkEvents");
		return FALSE;
	}

	/* ���� �߻��� �̺�Ʈ�� ���� �̺�Ʈ�� �ƴϸ� �������� �� ������ �õ� */
	if (FD_CONNECT != (FD_CONNECT & wsaNet.lNetworkEvents))
	{
#if 0
		CloseSocket(m_sdClient);
#endif
		return FALSE;
	}
	else
	{
		/* ���� ������ �߻� �ߴ��� ���� */
		if (wsaNet.iErrorCode[FD_CONNECT_BIT] != 0)
		{
			SocketError(L"CLuriaThread::IsLinkEvent. Though connected, it was internally rejected");
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
UINT8 CLuriaCThread::PopSockEvent()
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
			swprintf_s(tzMesg, 256, L"The return value of the WSAWaitForMultipleEvents failed. "
									L"(time out) (tgt_ipv4=%s)", m_tzTargetIPv4);
			if (m_enSysID != ENG_EDIC::en_demo)	LOG_WARN(m_enSysID, tzMesg);
		}
		SetCycleTime(NORMAL_THREAD_SPEED);
		return 0x00;
	}
	/* ��ü���� Network Event �˾Ƴ��� */
	u32WsaRet	-= WSA_WAIT_EVENT_0;
	i32WsaRet	= WSAEnumNetworkEvents(m_sdClient,	/* ��� ���� */
									   m_wsaClient,	/* ��� ���ϰ� ¦�� �̷� ��ü �ڵ� */
									   &wsaNet);	/* �̺�Ʈ ������ ����� ����ü ������ */
	/* ������ �������� Ȯ�� */
	if (i32WsaRet == SOCKET_ERROR)
	{
		SetCycleTime(NORMAL_THREAD_SPEED);
		return 0x00;
	}

	/* ���� ������ ���� �ӵ��� ���̱� ���� ó�� (������ ���Ÿ���� ���) */
	SetCycleTime(0);
	/* Socket Event �� �и� ó�� ���� */
	if (FD_READ == (FD_READ & wsaNet.lNetworkEvents))
	{
		/* ��� ��Ʈ�κ��� ������ �б� �õ� */
		if (wsaNet.iErrorCode[FD_READ_BIT] == 0)
		{
			u8RetCode	= Received(m_sdClient) ? 0x03 : 0x02;
		}
		/* ������ �б� ������ ���, ���� ��� ��Ʈ�� ���� ��� �о ������ */
		else
		{
			ReceiveAllData(m_sdClient);
		}
	}
	/* ������ ������ ���� Ȥ�� ��Ÿ �̺�Ʈ (Send or Connected)�� ���, ���� �������� ó�� */
	else if (FD_CLOSE == (FD_CLOSE & wsaNet.lNetworkEvents))
	{
		LOG_ERROR(m_enSysID, L"The connection was lost from the remote server [Luria]");
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
VOID CLuriaCThread::Connected(SOCKET sd)
{
	BOOL bSucc	= TRUE;
	LONG32 l32Event	= FD_READ|FD_CLOSE;

	/* ���� �۽ŵ� ��Ŷ ��� ���� */
	RemoveAllSendPkt();

	/* �ϴ� ����Ǿ��ٴ� �ñ׳��� ���������Ƿ�, DoWork�� ����õ��� ���� ���� */
	m_enSocket	= ENG_TPCS::en_linked;

	/* ����Ǿ����Ƿ�, �������� ���� ���ϵ��� �ð��� ���� */
	UpdateWorkTime();
	/* ���� �ɼ� �����ϱ� */
	if (bSucc)	bSucc = SetSockOptBuffSize(0x11, sd);	/* 8 KBytes�� ǥ���� */
//	if (bSucc)	bSucc = SetSockOptSpeed(1, sd);
	if (bSucc)	bSucc = SetSockOptRoute(1, sd);
//	if (bSucc)	bSucc = SetSockOptAlive(1, sd);
	/* ���� �ɼ� ���� ���� */
	if (bSucc)
	{
		/* Setup socket event (Combination of FD_READ and FD_CLOSE) */
		bSucc = (SOCKET_ERROR != WSAEventSelect(sd, m_wsaClient, l32Event));
		/* ���� ���� ����Ʈ ��Ʈ�ѿ� ��� */
		if (!bSucc)
		{
			SocketError(L"CLuriaCThread::Connected. An error occurred in the WSAEventSelect");
		}
	}

	/* ���� ����� �������̸� �����Լ� ȣ�� */
	if (!bSucc)
	{
		CloseSocket(sd);
	}
	else
	{
		if (!Connected())
		{
			CloseSocket(sd);
		}
		else
		{
			m_bConnectInited	= TRUE;
			/* ���� �Ϸ� ���� ���� */
			UpdateLinkTime(0x01);
			/* ������ ���ӵ� �ð� ���� */
			GetLocalTime(&m_stLinkTime);
		}
	}
}

/*
 desc : ������ �����̵� ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLuriaCThread::Connected()
{
	UINT8 i, u8TableX, u8TableY;
	PUINT8 pPkt	= NULL;
	CUniToChar csCnv1, csCnv2;
#ifndef _DEBUG
	TCHAR tzMesg[128]	= {NULL};
	swprintf_s(tzMesg, 128, L"The connection to the remote system is complete (src:%s tgt:%s)",
			   m_tzSourceIPv4, m_tzTargetIPv4);
	LOG_MESG(m_enSysID, tzMesg);
#endif

	/* ----------------------------------------------------------------------------------------- */
	/*                                     ComManagement                                         */
	/* ----------------------------------------------------------------------------------------- */

	/* CMPS Server ���� ���� */
	if (m_pstConfLuria->use_announcement && m_pPktCM)
	{
		UINT8 u8IPv4[4]	= {NULL};
		CUniToChar csCnv;
		/* Announcement ��� ���ο� ����, CMPS Server �ּ� ���� ���� */
		uvCmn_GetIPv4Uint32ToBytes(uvCmn_GetIPv4StrToUint32(m_tzSourceIPv4), u8IPv4);
		pPkt	= m_pPktCM->GetPktAnnounceServerIpAddr(ENG_LPGS::en_set, u8IPv4);
		/* ��Ŷ �۽� */
		AddPktSend(pPkt, m_pPktCM->GetPktSize());	::Free(pPkt);
	}

	/* ----------------------------------------------------------------------------------------- */
	/*                                          System                                           */
	/* ----------------------------------------------------------------------------------------- */

	/* ������ H/W Init ��û */
	if (m_pstConfLuria->use_hw_inited && m_pPktSS)
	{
		pPkt = m_pPktSS->GetPktInitializeHardware();
		AddPktSend(pPkt, m_pPktSS->GetPktSize());	::Free(pPkt);
	}
	/* System Status ��û */
	if (m_pPktSS)
	{
		pPkt = m_pPktSS->GetPktSystemStatus();
		AddPktSend(pPkt, m_pPktSS->GetPktSize());	::Free(pPkt);
	}

	if (!m_bInitSend)	return TRUE;

	/* ----------------------------------------------------------------------------------------- */
	/*                                      Machine Config                                       */
	/* ----------------------------------------------------------------------------------------- */

	/* ���а� ���� / Pitch (Stripe ����) / ��ũ�� �ӵ� / ���а� ȸ�� ���� ���� �� ��� */
	pPkt = m_pPktMC->GetPktTotalPhotoheads(ENG_LPGS::en_set, m_pstConfLuria->ph_count);
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
	pPkt = m_pPktMC->GetPktPhotoheadPitch(ENG_LPGS::en_set, m_pstConfLuria->ph_pitch);
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
	pPkt = m_pPktMC->GetPktScrollRate(ENG_LPGS::en_set, m_pstConfLuria->scroll_rate);
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
	pPkt = m_pPktMC->GetPktPhotoheadRotate(ENG_LPGS::en_set, m_pstConfLuria->ph_rotate);
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);

	/* ���а� IPv4 */
	for (i=0; i<m_pstConfLuria->ph_count; i++)
	{
		pPkt	= m_pPktMC->GetPktPhotoheadIpAddr(ENG_LPGS::en_set, i+1, m_pstConfLuria->ph_ipv4[i]);
		AddPktSend(pPkt, m_pPktMC->GetPktSize());
		::Free(pPkt);
	}
	/* ���а� Offset X / Y �� ���� (2 �� ���а� ����) */
	for (i=1; i<m_pstConfLuria->ph_count; i++)
	{
		pPkt	= m_pPktMC->GetPktPhotoheadOffset(ENG_LPGS::en_set, i+1,
												  (UINT32)ROUNDED(m_pstConfLuria->ph_offset_x[i]*1000000.0f, 0),	/* mm -> nm */
												  (INT32)ROUNDED(m_pstConfLuria->ph_offset_y[i]*1000000.0f, 0));	/* mm -> nm */
		AddPktSend(pPkt, m_pPktMC->GetPktSize());
		::Free(pPkt);
	}

	/* Motion Control Type / Motion IPv4 / Motion Speed : Y (Max) / X */
	pPkt = m_pPktMC->GetPktMotionControlType(ENG_LPGS::en_set, m_pstConfLuria->motion_control_type);
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
	pPkt = m_pPktMC->GetPktMotionControlIpAddr(ENG_LPGS::en_set, m_pstConfLuria->motion_control_ip);
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
	pPkt = m_pPktMC->GetPktMaxYMotionSpeed(ENG_LPGS::en_set, (UINT32)ROUNDED(m_pstConfLuria->max_y_motion_speed*1000.0f, 0));
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
	pPkt = m_pPktMC->GetPktXMotionSpeed(ENG_LPGS::en_set, (UINT32)ROUNDED(m_pstConfLuria->x_motion_speed*1000.0f, 0));
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);

	/* Table Settings */
	for (i=0; i<m_pstConfLuria->table_count; i++)
	{
		/* Motion Drived ID ���� */
		u8TableX= (i == 0x00) ? m_pstConfLuria->x_drive_id_1 : m_pstConfLuria->x_drive_id_2;
		u8TableY= (i == 0x00) ? m_pstConfLuria->y_drive_id_1 : m_pstConfLuria->y_drive_id_2;
		pPkt	= m_pPktMC->GetPktXYDriveId(ENG_LPGS::en_set, i+1, u8TableX, u8TableY);
		AddPktSend(pPkt, m_pPktMC->GetPktSize());
		::Free(pPkt);
		/* ��� �������� ���� �뱤 ���� ��ġ �� ��û */
		pPkt	= m_pPktMC->GetPktGetTableMotionStartPosition(i+1);
		AddPktSend(pPkt, m_pPktMC->GetPktSize());
		::Free(pPkt);
		/* Table Setting */
		UINT8 u8Direct	= i == 0 ?
				m_pstConfLuria->table_1_print_direction : m_pstConfLuria->table_2_print_direction;
		pPkt	= m_pPktMC->GetPktTableSettings(ENG_LPGS::en_set, i+1,
												m_pstConfLuria->paralleogram_motion_adjust[i],
												u8Direct);
		AddPktSend(pPkt, m_pPktMC->GetPktSize());
		::Free(pPkt);
		/* Table Exposure Start XY */
		pPkt	= m_pPktMC->GetPktTableExposureStartPos(ENG_LPGS::en_set, i+1,
														(INT32)ROUNDED(m_pstConfLuria->table_expo_start_xy[i][0]*1000.0f, 0),
														(INT32)ROUNDED(m_pstConfLuria->table_expo_start_xy[i][1]*1000.0f, 0));
		AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
		/* Table Position Limits */
		pPkt	= m_pPktMC->GetPktTablePositionLimits(ENG_LPGS::en_set, i+1,
													 (INT32)ROUNDED(m_pstConfLuria->table_limit_max_xy[i][0]*1000.0f, 0),
													 (INT32)ROUNDED(m_pstConfLuria->table_limit_max_xy[i][1]*1000.0f, 0),
													 (INT32)ROUNDED(m_pstConfLuria->table_limit_min_xy[i][0]*1000.0f, 0),
													 (INT32)ROUNDED(m_pstConfLuria->table_limit_min_xy[i][1]*1000.0f, 0));
		AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
	}
	/* Acceleration Distance / Active Table Number */
	pPkt = m_pPktMC->GetPktYaccelerationDistance(ENG_LPGS::en_set, (UINT32)ROUNDED(m_pstConfLuria->y_acceleration_distance*1000.0f, 0));
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
	pPkt = m_pPktMC->GetPktActiveTable(ENG_LPGS::en_set,  m_pstConfLuria->active_table_no);
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);

	/* Emulater ���� ���� ���� (Motion / Photohead / Trigger) �� Debugg Print Level */
#if 0
	pPkt = m_pPktMC->GetPktCommon(UINT8(ENG_LCMC::en_emulate_motor_controller));
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
	pPkt = m_pPktMC->GetPktCommon(UINT8(ENG_LCMC::en_emulate_photo_heads));
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
	pPkt = m_pPktMC->GetPktCommon(UINT8(ENG_LCMC::en_emulate_triggers));
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
#else
	pPkt = m_pPktMC->GetPktEmulate(ENG_LPGS::en_get);
#endif
	pPkt = m_pPktMC->GetPktDebugPrintLevel(ENG_LPGS::en_get);
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);

#if (USE_SIMULATION_DIR)
	/* Print Simulation Out Dir */
	if (_tcslen(m_pstConfLuria->print_simulation_out_dir) > 3)
	{
		pPkt	= m_pPktMC->GetPktPrintSimulationOutDir(ENG_LPGS::en_set,
														csCnv1.Uni2Ansi(m_pstConfLuria->print_simulation_out_dir),
														(UINT32)_tcslen(m_pstConfLuria->print_simulation_out_dir));
		AddPktSend(pPkt, m_pPktMC->GetPktSize());
		::Free(pPkt);
	}
#endif
	/* HysteresisType1 �� ��û */
	pPkt	= m_pPktMC->GetPktHysteresisType1(ENG_LPGS::en_set, m_pstConfLuria->hys_type_1_scroll_mode,
											  m_pstConfLuria->hys_type_1_negative_offset,
											  (UINT32)ROUNDED(m_pstConfLuria->hys_type_1_delay_offset[0]*1000000.0f, 0),
											  (UINT32)ROUNDED(m_pstConfLuria->hys_type_1_delay_offset[1]*1000000.0f, 0));
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
	/* ���а� Product ID */
	pPkt = m_pPktMC->GetPktProductId(ENG_LPGS::en_get);
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
	/* Focus Z Drive Type */
	pPkt = m_pPktMC->GetPktZdriveType(ENG_LPGS::en_set, m_pstConfLuria->z_drive_type);
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
	/* Artwork Complexity ���� */
	//pPkt = m_pPktMC->GetPktArtworkComplexity(ENG_LPGS::en_set, m_pstConfLuria->artwork_complexity);
	//AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
	/* If Z Drive Type is the SM (Sieb & Meyer) then ... */
	if (m_pstConfLuria->z_drive_type == 0x03)
	{
		pPkt = m_pPktMC->GetPktZdriveIpAddr(ENG_LPGS::en_set, m_pstConfLuria->z_drive_ip);
		AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
		for (i=0x00; i<m_pstConfLuria->ph_count; i++)
		{
			pPkt = m_pPktMC->GetPktLinearZdriveSetting(ENG_LPGS::en_set, i+1,
													   m_pstConfLuria->z_drive_sd2s_ph[i]);
			AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
		}
	}

	/* ----------------------------------------------------------------------------------------- */
	/*                                      Job Management                                       */
	/* ----------------------------------------------------------------------------------------- */

	/* Get Job List */
	pPkt = m_pPktJM->GetPktJobList();
	AddPktSend(pPkt, m_pPktJM->GetPktSize());	::Free(pPkt);
#if 0
	/* Selected Job */
	pPkt	= m_pPktJM->GetPktSelectedJob();
	AddPktSend(pPkt, m_pPktJM->GetPktSize());
	::Free(pPkt);
#endif
	/* Get Max Jobs */
	pPkt = m_pPktJM->GetPktGetMaxJobs();
	AddPktSend(pPkt, m_pPktJM->GetPktSize());	::Free(pPkt);
#if 0
	/* ----------------------------------------------------------------------------------------- */
	/*                                      Comm. Management                                       */
	/* ----------------------------------------------------------------------------------------- */
	/* Announcement Status */
	pPkt = m_pPktCM->GetPktAnnouncementStatus();
	AddPktSend(pPkt, m_pPktCM->GetPktSize());	::Free(pPkt);
#endif

	/* ----------------------------------------------------------------------------------------- */
	/*                                      Photohead Focus                                      */
	/* ----------------------------------------------------------------------------------------- */
#if 0	/* ��� (GUI) �ʿ��� �����ϵ��� ���� */
	/* ���� Emulated Mode�� ���� ���̶��.... �ʱ�ȭ �ʼ� */
	/*if (m_pstConfLuria->IsRunEmulated())*/
	{
		pPkt = m_pPktPF->GetPktInitializeFocus();
		AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
	}

	/* AF Gain / AF Work Range Min/Max <All> */
	pPkt = m_pPktPF->GetPktAfGain(ENG_LPGS::en_set, m_pstConfLuria->af_gain);
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
	pPkt = m_pPktPF->GetPktAbsWorkRange(ENG_LPGS::en_set, 0x00/*ALL*/,
										(INT32)ROUNDED(m_pstConfLuria->af_work_range_all[0]*1000.0, 0),
										(INT32)ROUNDED(m_pstConfLuria->af_work_range_all[1]*1000.0, 0));
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
#endif

	/* ----------------------------------------------------------------------------------------- */
	/*                                      Direct Photohead                                     */
	/* ----------------------------------------------------------------------------------------- */
	/* Initialize the existing errors */
	pPkt = m_pPktDP->SetPktClearErrorStatus(0xff);
	AddPktSend(pPkt, m_pPktDP->GetPktSize());	::Free(pPkt);

	return TRUE;
}

/*
 desc : ���ŵ� ������ ó��
 parm : tick	- [in]  ���� CPU �ð�
 retn : None
*/
VOID CLuriaCThread::PopPktData(UINT64 tick)
{
	/* ���� ���� */
	if (m_syncPktRecv.Enter())
	{
		LPG_PCLR pstPktRecv	= m_pRecvQue->PopPktData();
		/* ���ŵ� �����Ͱ� �����ϴ��� �ֱ������� Ȯ�� */
		if (pstPktRecv)
		{
			/* �ֱٿ� ���ŵ� �����Ͱ� �����ϴ� ���, ���Ź��� �ð� ���� */
			m_u64RecvTime	= tick;
			PktAnalysis(pstPktRecv);
			/* user_data �޸� �Ҵ� ���� */
			if (pstPktRecv && pstPktRecv->user_data)	delete [] pstPktRecv->user_data;
		}
		/* ���� ���� */
		m_syncPktRecv.Leave();
	}
}

/*
 desc : ���ŵ� ������ �м� ó��
 parm : data	- [in]  ���ŵ� �����Ͱ� ����� ����ü ������
 retn : TRUE or FALSE
*/
BOOL CLuriaCThread::PktAnalysis(LPG_PCLR data)
{
	UINT16 u16Size	= 0;

	/* �б�/���� ��û�� ���� ���� �� ������ �߻��� ��� */
	if (data->status)
	{
#if 1
		if (data->status != 10028 /* NotAllowedWithJobAssigned */ &&
			!(data->user_data_family == 0xA5 &&
			  data->user_data_id == 0x02 &&
			  data->status == 10008 /* ReadNotAllowed */))
#endif
		{
			SetPktError(ENG_LNWE::en_warning, 0x00, data);
		}
	}
	else
	{
		/* ���� ���� ������ �� User Data ũ�� ��� */
		u16Size	= data->num_bytes - UINT16(sizeof(STG_PCLR) - sizeof(PUINT8));
		/* ������ ���� ��û�� ���� ������ �Դٰ� �÷��� ������ ���� */
		/* !!! �б� ��û�� ���� ó���� ����... WriteReply�� ���� ó���� ���� ���� !!! */
		if (data->command_type == UINT8(ENG_LTCT::en_read_reply) ||
			data->command_type == UINT8(ENG_LTCT::en_announce))
		{
			m_pstShMemLuria->RecvCMD(data->user_data_family, data->user_data_id);
			/* �ϱ� ��û�� ���� ������ ��� (GetJobList�� ���, ��ϵ� �����Ͱ� ���� ��� u32Size = 0 ��)*/
			/*if (u16Size > 0)	*/SetRecvPacket(ENG_TPCS::en_recved, (PUINT8)data);
		}
	}

	return TRUE;
}

/*
 desc : Luria Server�� Photohead LED On-Time �� ��û
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLuriaCThread::ReqGetPhLedOnTimeAll()
{
	BOOL bSucc	= TRUE;
	PUINT8 pPkt	= NULL;

	/* Photohead ���� �뺰 �� 4������ ���������� ��û�ϱ� ���� */
	pPkt	= m_pPktDP->GetPktPhLedOnTimeAll(0);
	if (bSucc && pPkt)
	{
		bSucc	= AddPktSend(pPkt, m_pPktDP->GetPktSize());
	}
	::Free(pPkt);
	pPkt	= m_pPktDP->GetPktPhLedOnTimeAll(1);
	if (bSucc && pPkt)
	{
		bSucc	= AddPktSend(pPkt, m_pPktDP->GetPktSize());
	}
	::Free(pPkt);
	pPkt	= m_pPktDP->GetPktPhLedOnTimeAll(2);
	if (bSucc && pPkt)
	{
		bSucc	= AddPktSend(pPkt, m_pPktDP->GetPktSize());
	}
	::Free(pPkt);
	pPkt	= m_pPktDP->GetPktPhLedOnTimeAll(3);
	if (bSucc && pPkt)
	{
		bSucc	= AddPktSend(pPkt, m_pPktDP->GetPktSize());
	}
	::Free(pPkt);

	return bSucc;
}

/*
 desc : Luria Server�� Photohead LED Temperature �� ��û - ��ü Photohead �� ���ļ� �� �� ��û
 parm : freq_no	- [in]  ��ü Photohead�� ������ ��ȣ (0:365, 1:385, 2:395, 4:405)
 retn : TRUE or FALSE
*/
BOOL CLuriaCThread::ReqGetPhLedTempFreqAll(UINT8 freq_no)
{
	BOOL bSucc	= TRUE;
	PUINT8 pPkt	= NULL;

	pPkt	= m_pPktDP->GetPktPhLedTempAll(freq_no);
	if (pPkt)	bSucc	= AddPktSend(pPkt, m_pPktDP->GetPktSize());
	::Free(pPkt);

	return bSucc;
}

/*
 desc : Luria Server�� Photohead LED Temperature �� ��û
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLuriaCThread::ReqGetPhLedTempAll()
{
	BOOL bSucc	= TRUE;
	PUINT8 pPkt	= NULL;

	/* Photohead ���� �뺰 �� 4������ ���������� ��û�ϱ� ���� */
	pPkt	= m_pPktDP->GetPktPhLedTempAll(0);
	if (bSucc && pPkt)
	{
		bSucc	= AddPktSend(pPkt, m_pPktDP->GetPktSize());
	}
	::Free(pPkt);
	pPkt	= m_pPktDP->GetPktPhLedTempAll(1);
	if (bSucc && pPkt)
	{
		bSucc	= AddPktSend(pPkt, m_pPktDP->GetPktSize());
	}
	::Free(pPkt);
	pPkt	= m_pPktDP->GetPktPhLedTempAll(2);
	if (bSucc && pPkt)
	{
		bSucc	= AddPktSend(pPkt, m_pPktDP->GetPktSize());
	}
	::Free(pPkt);
	pPkt	= m_pPktDP->GetPktPhLedTempAll(3);
	if (bSucc && pPkt)
	{
		bSucc	= AddPktSend(pPkt, m_pPktDP->GetPktSize());
	}
	::Free(pPkt);

	return bSucc;
}

/*
 desc : ���� ������ �۽�
 parm : data	- [in]  ������ �����Ͱ� ����� ���� ������
		size	- [in]  'data' ���ۿ� ����� �������� ũ��
 retn : TRUE - �۽� ����(��� �۽ŵ� �����Ͱ� ������),
		FALSE - �۽� ����(���� ����... ������ �����ؾ� ��)
*/
BOOL CLuriaCThread::AddPktSend(PUINT8 data, UINT32 size)
{
	UINT8 u8CmdType		= UINT8(ENG_LTCT::en_write);
	UINT8 u8FamilyID	= 0xff;
	UINT8 u8UserID		= 0xff;
	UINT16 u16PktSize	= 0, u16PktTotal = 0x0000;
	UINT32 i			= 0;
	BOOL bSucc			= FALSE;
	STM_LPSB stPktSend	= {NULL};
	PUINT8 pPktData		= data;

	/* ��Ŷ ũ�Ⱑ �ʹ� ������ ���� ó�� */
	if (size < 0x06)
	{
		LOG_ERROR(m_enSysID, L"Optic. Service : Packet size too small (size < 0x06)");
		return FALSE;
	}

	/* �۽� ��Ŷ �� ��Ŷ�� Family ID�� User ID �׸��� ��Ŷ ��û or ���� ���� �м��Ͽ� �ʿ��� ������ ���� */
	for (; i<size; i++)
	{
		/* 1 ��ɾ��� ��Ŷ ũ�� ���� */
		memcpy(&u16PktSize, pPktData, 2);	/* ProtocolVersion included (2 + 1) */
		u16PktSize	= SWAP16(u16PktSize);
		u16PktTotal	+= u16PktSize;
		/* Command Type */
		if (pPktData[3] == (UINT8)ENG_LTCT::en_read)
		{
			u8CmdType	= pPktData[3];
			u8FamilyID	= pPktData[4];
			u8UserID	= pPktData[5];
		}
		if (u16PktTotal == size)	break;
		/* ���� ��Ŷ ��� ��ġ�� �̵� */
		pPktData	+= u16PktSize;
	}

	/* ---------------------------------------------------------------------------- */
	/* �б� ��û�� ���� �۽� ��Ŷ�� ���ؼ���, ���ŵ� ��Ŷ�� ���� ���ٰ� �÷��� ���� */
	/* ---------------------------------------------------------------------------- */
	if (u8CmdType == (UINT8)ENG_LTCT::en_read)
	{
		/* �۽ſ� ���� ���� ��Ŷ�� ������, ���� ���� �غ� �Ǿ� �ִٰ� ���� */
		if (!IsWriteOnlyCmd(u8FamilyID/*family_id*/, u8UserID/*user_id*/))
		{
			m_pstShMemLuria->SendCMD(u8FamilyID/*family_id*/, u8UserID/*user_id*/);
		}
	}

	/* ���� ����Ǿ����� ���ο� ���� */
	if (!IsConnected())	return FALSE;

	/* ���� ���� */
	if (m_syncPktSend.Enter())
	{
		if (m_lstPktSend.GetCount() < MAX_LURIA_PACKET_COUNT)
		{
			/* ���� �޸� �Ҵ� �� ���� */
			stPktSend.pkt_buff		= (PUINT8)::Alloc(size+1);
			stPktSend.pkt_buff[size]= 0x00;
			memcpy(stPktSend.pkt_buff, data, size);	/* The base size of sending packet */
			stPktSend.pkt_size		= size;

			/* �Էµ� ��Ŷ�� ���ο��� �ֱ������� �۽ŵǴ� ��Ŷ�̸�, �� �������� ��� */
			m_lstPktSend.AddTail(stPktSend);
			bSucc	= TRUE;
#if 0
			/* �Էµ� ��Ŷ�� �ܺο��� ���� �۽��ϴ� ��Ŷ�̸�, �ֱ������� �۽ŵǴ� ��Ŷ ���� �տ� ��ġ */
			else
			{
				/* ���� �������� ��ϵ� ��Ŷ�� �ϳ��� ���� ������� ���� Ȯ�� */
				if (m_lstPktSend.GetCount() < 1)
				{
					m_lstPktSend.AddTail(stPktSend);
				}
				else
				{
					/* 1 �� �̻� ����� �Ǿ� �ִٸ� ... */
					pPos	= m_lstPktSend.GetHeadPosition();
					while (pPos)
					{
						pOldPos		= pPos;	/* ���� ��ġ �ӽ� ���� */
						stPktSave	= m_lstPktSend.GetNext(pPos);
						/* ���� �˻��� ��ġ�� ��Ŷ �����Ͱ� �ֱ������� �۽ŵǴ� ��Ŷ���� Ȯ�� */
						if (0x01 == stPktSave.type)
						{
							m_lstPktSend.InsertBefore(pOldPos, stPktSend);
							break;	/* ���� ���������� */
						}
						/* ���� �� �̻� �˻��� �׸��� ���ٸ�, �� �������� ��� */
						if (!pPos)
						{
							m_lstPktSend.AddTail(stPktSend);
						}
					}
				}
			}
			bSucc	= TRUE;
#endif
		}
		else
		{
			LOG_ERROR(m_enSysID, L"List buffer waiting to be sent is full");
		}
		/* ���� ���� */
		m_syncPktSend.Leave();
	}

	return bSucc;
}

/*
 desc : �ֱ������� ��û�ϴ� ��Ŷ ó�� ��ƾ (�ý��� ����, �Ź� ���� ����, �뱤 ���� ���� ���)
 parm : None
 retn : TRUE (�۽��� �����Ͱ� �ִ�) or FALSE (�ƹ��͵� �۽��� �����Ͱ� ����)
*/
BOOL CLuriaCThread::ReqPeriodSendPacket()
{
	STM_LPSB stPktSend	= {NULL};
	STG_PCLS stPktData	= {NULL};

	/* �⺻������ ����Ʈ�� �۽� ��Ŷ�� ����Ǿ� ������, ������ ���� �۽� ó�� ���� */
	if (m_lstPktSend.GetCount())
	{
		/* �۽ŵ� ��Ŷ 1�� �������� */
		stPktSend	= m_lstPktSend.GetHead();
		/* �۽� ��Ŷ �α� ���� */
		memcpy(&stPktData, stPktSend.pkt_buff, sizeof(STG_PCLS));
		stPktData.SwapNetworkToHost();

		/* ��Ŷ ������ ���� */
		if (!ReSended(stPktSend.pkt_buff, stPktSend.pkt_size, 3, 100))
		{
			TCHAR tzMesg[256]	= {NULL};
			swprintf_s(tzMesg, 256,
						L"Any number of send operations (3 times) failed. "
						L"[fid=%02x / uid=%02x / cmd_type=%02x / Remote IPv4:%s]",
						stPktData.user_data_family, stPktData.user_data_id,
						stPktData.command_type, m_tzTargetIPv4);
			/* ������ ��Ŷ �۽� ������ �ð����� �ֱⰡ �ʹ� ª���� �θ𿡰� �˸��� ���� */
			LOG_ERROR(m_enSysID, tzMesg);
			/* Socket Closed */
			CloseSocket(m_sdClient);
		}
#if 0
		else
		{
			if (stPktData.user_data_family != 0xA7 || stPktData.user_data_id != 0x02)
			{
#ifdef _DEBUG
				TRACE(L"Luria Send : fid = 0x%02x / uid = 0x%02x / cmd_type = 0x%02x / size = %d\n",
					  stPktData.user_data_family, stPktData.user_data_id, stPktData.command_type,
					  stPktData.num_bytes);
#endif
			}
		}
#endif
		/* ���� ���а� �����̰�, ����Ʈ���� ��Ŷ ������ �Ѱ� ���� */
		if (stPktSend.pkt_buff)	::Free(stPktSend.pkt_buff);
		m_lstPktSend.RemoveHead();
	}

	/* ---------------------------------------------------------------------------- */
	/* !!! ������ �۽��� �����Ͱ� �� �ִ��� Ȥ�� ������ �۽��� �� ���� �������� !!! */
	/* ---------------------------------------------------------------------------- */
	return (m_lstPktSend.GetCount() > 0);
}

/*
 desc : ��Ŷ ���� ó��
 parm : type	- [in]  �̺�Ʈ ���� (ENG_TPCS)
		data	- [in]  �۽� or ���ŵ� �����Ͱ� ����� ����ü ������
 retn : None
*/
VOID CLuriaCThread::SetRecvPacket(ENG_TPCS type, PUINT8 data)
{
	UINT16 u16Size	= 0;
	LPG_PCLR pstPkt	= LPG_PCLR(data);

	/* ���� �����͸� ��� */
	if (ENG_TPCS::en_recved != type || !pstPkt)
	{
		LOG_ERROR(m_enSysID, L"Failed to recv the success packet");
		return;
	}

	/* ��Ŷ �����ϰ�, �����ͺ� ũ�⸸ ��� */
	u16Size	= pstPkt->num_bytes - (sizeof(STG_PCLR) - sizeof(PUINT8));
	/* User Data Family */
	switch (pstPkt->user_data_family)
	{
	case UINT8(ENG_LUDF::en_machine_config)		:	m_pPktMC->SetRecvPacket(pstPkt->user_data_id, pstPkt->user_data, u16Size);	break;
	case UINT8(ENG_LUDF::en_job_management)		:	m_pPktJM->SetRecvPacket(pstPkt->user_data_id, pstPkt->user_data, u16Size);	break;
	case UINT8(ENG_LUDF::en_panel_preparation)	:	m_pPktPP->SetRecvPacket(pstPkt->user_data_id, pstPkt->user_data, u16Size);	break;
	case UINT8(ENG_LUDF::en_exposure)			:	m_pPktEP->SetRecvPacket(pstPkt->user_data_id, pstPkt->user_data, u16Size);	break;
	case UINT8(ENG_LUDF::en_direct_photo_comm)	:	m_pPktDP->SetRecvPacket(pstPkt->user_data_id, pstPkt->user_data, u16Size);	break;
	case UINT8(ENG_LUDF::en_system)				:	m_pPktSS->SetRecvPacket(pstPkt->user_data_id, pstPkt->user_data, u16Size);	break;
	case UINT8(ENG_LUDF::en_focus)				:	m_pPktPF->SetRecvPacket(pstPkt->user_data_id, pstPkt->user_data, u16Size);	break;
	}

	/* ------------------------------------------------------------------------------------ */
	/* �ʱ�ȭ���� ���� ���� ��, ������ ���� ���� ���õ� �Ź� ��û�� ���� ������ ���ŵ� ��� */
	/* ���� �� ���� ���� ���õ� �Ź��� �� ����(user_data_id: 0x07, 0x08, 0x08, 0x0a) ��û */
	/* ------------------------------------------------------------------------------------ */
	if (!m_bLuriaInited &&
		pstPkt->user_data_family == (UINT8)ENG_LUDF::en_job_management &&
		pstPkt->user_data_id == (UINT8)ENG_LCJM::en_get_max_jobs)
	{
		/* ���� ���õ� �Ź��� �� ���� ��û (Fiducial Info. ���Ե�) */
		if (strlen(m_pstShMemLuria->jobmgt.selected_job_name) > 0)
		{
			PUINT8 pPkt	= m_pPktJM->GetPktGetJobParams((UINT32)strlen(m_pstShMemLuria->jobmgt.selected_job_name),
													   m_pstShMemLuria->jobmgt.selected_job_name);
			if (pPkt)
			{
				AddPktSend(pPkt, m_pPktJM->GetPktSize());
				::Free(pPkt);
			}
		}

		/* Luria Service�� �ʱ�ȭ �Ǿ��ٰ� ���� */
		m_bLuriaInited	= TRUE;
		/* !!! ���� �߻��� ���� �ڵ� �� �ʱ�ȭ !!! */
		m_pstShMemLuria->link.u16_error_code	= 0x0000;
	}

	/* Luria �κ��� ���ŵ� ��ɾ� ������ �α׿� ���� */
	if (GetConfig()->set_comn.comm_log_optic)
	{
		TCHAR tzMesg[128]	= {NULL};
		swprintf_s(tzMesg, 128,
				   L"UVDI15 vs. OPTIC [Recv] [called_func=SetRecvPacket][fid=0x%02x,uid=0x%02x][size=%d]",
				   pstPkt->user_data_family, pstPkt->user_data_id, u16Size);
		LOG_MESG(m_enSysID, tzMesg);
	}
#if 0	/* �̹� �� �Լ��� ȣ���ϴ� �θ� �Լ����� ó���ϰ� ���� */
	/* ���� �ֱٿ� ���ŵ� ��Ŷ ��ɾ ���� (Family ID & User ID) */
	m_pstShMemLuria->RecvCMD(pstPkt->user_data_family, pstPkt->user_data_id);
#endif
}

/*
 desc : ���� ���� ���� ����
 parm : flag	- [in]  0x00: ���� ���� ����, 0x01: ����� ����
 retn : None
*/
VOID CLuriaCThread::UpdateLinkTime(UINT8 flag)
{
	BOOL bUpate	= FALSE;

	/* ���� ���� ���� �����̰�, ������ ����� ���¶�� */
	if (!flag && m_pstShMemLuria->link.is_connected)		bUpate	= TRUE;
	/* ���� ���� �����̰�, ������ ������ ���¶�� */
	else if (flag && !m_pstShMemLuria->link.is_connected)	bUpate	= TRUE;

	/* ���� ���¿� �ٸ� �����, ���� ���� */
	if (bUpate)
	{
		m_pstShMemLuria->link.is_connected		= flag;
		m_pstShMemLuria->link.link_time[0]		= m_stLinkTime.wYear;
		m_pstShMemLuria->link.link_time[1]		= m_stLinkTime.wMonth;
		m_pstShMemLuria->link.link_time[2]		= m_stLinkTime.wDay;
		m_pstShMemLuria->link.link_time[3]		= m_stLinkTime.wHour;
		m_pstShMemLuria->link.link_time[4]		= m_stLinkTime.wMinute;
		m_pstShMemLuria->link.link_time[5]		= m_stLinkTime.wSecond;
		m_pstShMemLuria->link.last_sock_error	= GetSockLastError();
	}
}

/*
 desc : �۽� ���۰� �� ���ִ� �������� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLuriaCThread::IsSendBuffFull()
{
	return (UINT32)m_lstPktSend.GetCount() >= MAX_LURIA_PACKET_COUNT;
}

/*
 desc : �۽� ���ۿ� ���� �ִ� ������ ���� ����
 parm : None
 retn : �۽� ������ ���� ���� ��, �� �۽��� �� �ִ� ���� (����: %)
*/
FLOAT CLuriaCThread::GetSendBuffEmptyRate()
{
	return (FLOAT)(100.0f - (FLOAT)m_lstPktSend.GetCount() / FLOAT(MAX_LURIA_PACKET_COUNT));
}
