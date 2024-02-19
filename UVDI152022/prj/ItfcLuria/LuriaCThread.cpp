/* desc : Luria Server Thread 즉, CMPS: Client, Luria: Server */

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
 desc : 생성자
 parm : luria		- [in]  Luria 통신 정보
		conf		- [in]  Luria 환경 정보
		shmem		- [in]  Luria Shared Memory
		object		- [in]  각종 Luria Objects
 retn : None
 note : source_ip	- [in]  Local IPv4
		target_ip	- [in]  Remote IPv4
		tcp_port	- [in]  TCP/IP Port
		port_buff	- [in]  TCP/IP Port Buffer Size
		recv_buff	- [in]  Send or Recv의 패킷 중 가장 큰 패킷의 크기 (단위: Bytes)
		ring_buff	- [in]	Max Recv Packet Size
		idle_time	- [in]  원격 시스템과의 통신 유무 (Alive Check) 체크하기 위한 시간 (단위: 밀리초)
							즉, 이 시간 (단위: 초) 동안 아무런 통신이 없으면 Alive Check 송신
							이 값이 0 인 경우, 이 기능을 사용하지 않음
		link_time	- [in]  원격 시스템에 접속이 되지 않을 때, 주기적으로 접속하는 시간 (단위: 밀리초)
		sock_time	- [in]  소켓 이벤트 (송/수신) 감지 대기 시간 (단위: 밀리초)
							해당 주어진 값(시간)만큼만 소켓 이벤트 감지후 다음 단계로 루틴 이동
		common		- [in]  Luria Common Config
		init_send	- [in]  접속 이후 초기화 명령을 송신할지 여부
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
	/* 아직 연결된 후 초기화되지 않았다고 설정 */
	m_bConnectInited	= FALSE;
	m_bWorkBusy			= FALSE;
	m_u32LinkTime		= luria->link_time;	/* 재접속하는 주기 설정 (단위: 밀리초) */
	m_u32IdleTime		= luria->idle_time;	/* 해당시간 동안 통신 작업이 없으면 기존 통신 연결 해제 */
	m_u64RecvTime		= 0;
	m_u8NextJob			= 0x00;
	m_bInitSend			= init_send;
#if 0	/* GUI 쪽에서 필요할 때마다 호출해서 진행하는 것으로 해야 될 듯... 여기서는 필요 없음 */
	m_u8NextPh			= 0x01;		/* 포토헤드 1 번 부터 요청 */
	m_u8NextLed			= 0x00;		/* 0 부터 5까지 반복함 (현재는 1 ~ 4까지만 사용함) */
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
	/* 주기적으로 Luria에게 요청하는 간격 시간 초기화 */
	m_u64ReqLedOnTime	= 0;	/* 처음 Led On Time 요청할 때는 바로 호출할 수 있도록 하기 위함 */
	m_u64ReqPeriodTime	= 0;

	/* 중요!!! */
	if (init_send)	m_bLuriaInited	= FALSE;	/* Luria Service에 연결되고 초기화 되지 않았다고 설정 */
	else			m_bLuriaInited	= TRUE;		/* Luria Service에 연결되고 초기화 되지 않았다고 설정 */
	/* Check whether it operates in demo mode */
	if (GetConfig()->IsRunDemo())	m_enSysID = ENG_EDIC::en_demo;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CLuriaCThread::~CLuriaCThread()
{
	RemoveAllSendPkt();
}

/*
 desc : 기존 송신될 패킷 모두 제거
 parm : None
 retn : None
*/
VOID CLuriaCThread::RemoveAllSendPkt()
{
	POSITION pPos, pPrePos;
	STM_LPSB stPktData;

	/* 패킷 송신 임시 버퍼 메모리 해제 (직접 / 주기적 송신 패킷 리스트 모두) */
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
 desc : 주기적으로 호출됨
 parm : None
 retn : None
*/
VOID CLuriaCThread::RunWork()
{
	UINT64 u64Tick	= GetTickCount64();

	/* ------------------------------------------------------- */
	/* 현재 연결된 Client가 존재하지 않으면, 재-접속 작업 수행 */
	/* 최근에 수신된 데이터가 없는 경우, 통신 연결 강제로 해제 */
	/* 전제 조건 : 현재 주기적으로 패킷이 전송되는 조건에 한함 */
	/* ------------------------------------------------------- */
	if ((m_u32IdleTime > 0 &&
		(m_u64RecvTime + m_u32IdleTime < u64Tick)) &&	/* 환경 설정에 저장된 시간까지 응답이 없으면 ... */
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

	/* Socket Event 감지처리 */
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

	/* 수신된 데이터 있으면, 처리 */
	PopPktData(u64Tick);

	/* 동기 진입 */
	if (m_syncPktSend.Enter())
	{
		/* 기존 송신될 데이터가 있는지 있으면 송신 (더 이상 송신될 데이터가 없으면, 주기적으로 송신될 데이터 처리) */
		if (!ReqPeriodSendPacket())
		{
			ReqGetPeriodPkt(u64Tick);
		}
		/* 동기 해제 */
		m_syncPktSend.Leave();
	}
}
#if 1
/*
 desc : 주기적으로 패킷 요청
 parm : tick	- [in]  현재 CPU 시간
 retn : None
*/
VOID CLuriaCThread::ReqGetPeriodPkt(UINT64 tick)
{
	PUINT8 pPkt			= NULL;
	CBaseFamily *pBase	= NULL;

	/* !!! 현재 노광 중이지 않는지 여부 확인 !!! */
	if (!m_pstShMemLuria->exposure.IsWorkIdle())	return;
	/* !!! 현재 거버 적재 중이지 않는지 여부 확인 !!! */
	if (m_pstShMemLuria->jobmgt.job_total_strip_loaded > 0 &&
		m_pstShMemLuria->jobmgt.job_state_strip_loaded !=
		m_pstShMemLuria->jobmgt.job_total_strip_loaded)
	{
		return;
	}
	/* 현재 내부적으로 Luria 관련 작업이 진행 중인 경우, 주기적으로 송신하는 것 방지 */
	/* Luria에 부하를 주는 경우, Luria 가 죽는 현상이 발생 됨 */
	if (m_bWorkBusy)	return;

	/* 굳이 여기서 Time Delay 사용할 필요 없음. 전역적으로 이미 딜레이가 있음 */
	if (tick < m_u64ReqPeriodTime + REQ_LURIA_PERIOD_TIME * 2)	return;
	/* 가장 최근에 송신한 시간 저장 */
	m_u64ReqPeriodTime	= tick;

	switch (m_u8NextJob)
	{
	case 0x00 :	pPkt = m_pPktSS->GetPktSystemStatus();			pBase = m_pPktSS;	break;
	case 0x01 :	pPkt = m_pPktDP->GetPktMotorAbsPositionAll();	pBase = m_pPktDP;	break;
	case 0x02 :	if (0x01 == m_pstConfLuria->z_drive_type)	/* Only for stepping motors */
					pPkt = m_pPktDP->GetPktMotorStateAll();		pBase = m_pPktDP;	break;
	}

	/* 패킷이 정상적으로 만들어졌다면 */
	if (pPkt)
	{
		AddPktSend(pPkt, pBase->GetPktSize());
		::Free(pPkt);
	}

	/* 다음 패킷 송신을 위해 증가 시킴 */
	if (0x03 == ++m_u8NextJob)	m_u8NextJob	= 0x00;
}
#else
/*
 desc : 주기적으로 패킷 요청
 parm : tick	- [in]  현재 CPU 시간
 retn : None
*/
VOID CLuriaCThread::ReqGetPeriodPkt(UINT64 tick)
{
	UINT32 u32Size	= 0;
	PUINT8 pPkt		= NULL;

	/* 굳이 여기서 Time Delay 사용할 필요 없음. 전역적으로 이미 딜레이가 있음 */
	if (tick < m_u64ReqPeriodTime + REQ_LURIA_PERIOD_TIME)	return;
	/* 가장 최근에 송신한 시간 저장 */
	m_u64ReqPeriodTime	= tick;
#if 0
	/* Announcement Status 요청 */
	pPkt	= m_pPktCM->GetPktAnnouncementStatus();
	if (pPkt)	u32Size	= m_pPktCM->GetPktSize();
#else
	/* Test Announcement */
	pPkt	= m_pPktCM->GetPktGenTestAnnouncement();
	if (pPkt)	u32Size	= m_pPktCM->GetPktSize();
#endif
	/* 패킷이 정상적으로 만들어졌다면 */
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
	/* 부모 쪽의 CloseSocket 함수 호출하기 전에 수행할 업무 진행 */
	/* --------------------------------------------------------- */
	m_bConnectInited	= FALSE;
	/* 연결 해제 상태 갱신 */
	UpdateLinkTime(0x00);

	/* 상위 부모 함수 호출 */
	CLuriaThread::CloseSocket(sd, time_out);
	/* Socket 종류에 따라 초기화 방법 다르게 처리 */
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
 desc : 연결 여부 확인하고, 연결 안되어 있으면, 연결 작업 수행
		만약 연결 안된 상태에서 연결되었으면, 연결 후 초기화 작업 수행
 parm : None
 retn : None
*/
VOID CLuriaCThread::SetConnect()
{
	/* 중요!!! */
	if (m_bInitSend)	m_bLuriaInited	= FALSE;	/* Luria Service에 연결되고 초기화 되지 않았다고 설정 */
	else				m_bLuriaInited	= TRUE;	/* Luria Service에 연결되고 초기화 되지 않았다고 설정 */
	/* 수신받은 시간 갱신 */
	m_u64RecvTime	= GetTickCount64();

	/* ------------------------------------------------- */
	/* 현재 연결된 Client가 존재하지 않으면, 재접속 수행 */
	/* ------------------------------------------------- */
	/* 임의의 시간마다 재접속을 수행하도록 하기 위함 */
	if (IsTcpWorkConnect(m_u32LinkTime))
	{
		/* 접속 작업을 위해 Client 소켓을 초기화 했다고 알림 */
		if (!DoConnect())	m_i32SockError = WSAGetLastError();
	}

	/* Socket Event 감지처리 (연결되었다면, 가상함수 호출) */
	if (m_enSocket == ENG_TPCS::en_linking)
	{
		if (IsLinkEvent())
		{
			/* 가장 최근의 송신 명령 플래그 메모리 초기화 */
			m_pstShMemLuria->ResetLastRecvCmd();
			/* 연결 되었다고 플래그 설정 */
			m_bIsLinked		= TRUE;
			/* 먼저 연결 초기화 함수 호출 후 */
			Connected(m_sdClient);
		}
	}

	/* 반드시 기입. 수신되지 않았지만. 연결시도이므로, 수신되었다고 설정 */
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

	/* Server or Client의 Listen or Client Socket 재초기화에 따른 시간 갱신 */
	m_u64ConnectTime	= GetTickCount64();

	/* Ring Buffer 초기화 */
	m_pPktRingBuff->ResetBuff();

	/* Remove existing client socket exists */
	if (INVALID_SOCKET != m_sdClient)
	{
		CloseSocket(m_sdClient);
	}

	/* 접속을 시도하므로 플래그 설정 */
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
 desc : 소켓 연결 이벤트가 있는지 확인
 parm : None
 retn : TRUE - 연결 이벤트 발생, FALSE - 연결 이벤트 없음
*/
BOOL CLuriaCThread::IsLinkEvent()
{
	INT32 i32WsaRet	= 0;
	UINT32 u32WsaRet= 0;
	WSANETWORKEVENTS wsaNet;

	/* Socket Event의 Signal 상태 감지하기 */
	u32WsaRet	= WSAWaitForMultipleEvents(1,				/* 소켓 이벤트 감지 개수: 1개임 */
										   &m_wsaClient,	/* 소켓 이벤트 핸들 */
										   FALSE,			/* TRUE : 모든 소켓 이벤트 객체가 신호 상태, FALSE : 1개 이벤트 객체가 신호 상태이면 즉시 리턴 */
										   m_u32SockTime,	/* 임의 시간 (밀리초) 이내에 응답이 없으면 리턴 */
										   FALSE);			/* WSAEventSelect에서는 항상 FALSE */
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
	/* 구체적인 Network Event 알아내기 */
	u32WsaRet	-= WSA_WAIT_EVENT_0;
	i32WsaRet	= WSAEnumNetworkEvents(m_sdClient,			/* 대상 소켓 */
									   m_wsaClient,			/* 대상 소켓과 짝을 이룬 객체 핸들 */
									   &wsaNet);			/* 이벤트 정보가 저장될 구조체 포인터 */
	/* 소켓이 에러인지 확인 */
	if (i32WsaRet == SOCKET_ERROR)
	{
		SocketError(L"CLuriaThread::IsLinkEvent. An error occurred in the WSAEnumNetworkEvents");
		return FALSE;
	}

	/* 현재 발생된 이벤트가 연결 이벤트가 아니면 연결해제 후 재접속 시도 */
	if (FD_CONNECT != (FD_CONNECT & wsaNet.lNetworkEvents))
	{
#if 0
		CloseSocket(m_sdClient);
#endif
		return FALSE;
	}
	else
	{
		/* 연결 에러가 발생 했는지 여부 */
		if (wsaNet.iErrorCode[FD_CONNECT_BIT] != 0)
		{
			SocketError(L"CLuriaThread::IsLinkEvent. Though connected, it was internally rejected");
			/* 기존 연결 소켓 해제처리 */
			CloseSocket(m_sdClient);
			return FALSE;
		}
	}

	/* 연결 이벤트라고 반환 */
	return TRUE;
}

/*
 desc : 소켓 이벤트 감지 대기
 parm : None
 retn : 0x00 - 실패, 0x01 - 접속, 0x02 - 해제, 0x03 - 데이터 수신
*/
UINT8 CLuriaCThread::PopSockEvent()
{
	UINT8 u8RetCode	= 0x00;
	INT32 i32WsaRet	= 0;
	UINT32 u32WsaRet= 0;
	WSANETWORKEVENTS wsaNet;

	/* ----------------------------------------------------------------------------------------- */
	/* 이벤트 객체가 신호 상태가 될 때까지 대기한다. WSAWaitForMultipleEvents() 함수의 반환 값은 */
	/* 신호 상태가 된 이벤트 객체의 배열 인덱스 + WSA_WAIT_EVENT_0 값이다. 그러므로, 실제 인덱스 */
	/* 값을 얻으려면 WSA_WAIT_EVENT_0 (WAIT_BOJECT_0) 값을 빼야 한다.                            */
	/* ----------------------------------------------------------------------------------------- */
	u32WsaRet	= WSAWaitForMultipleEvents(1,				/* 소켓 이벤트 감지 개수: 1 개 */
										   &m_wsaClient,	/* 소켓 이벤트 핸들 */
										   FALSE,			/* TRUE : 모든 소켓 이벤트 객체가 신호 상태, FALSE : 1개 이벤트 객체가 신호 상태이면 즉시 리턴 */
										   m_u32SockTime,	/* 임의 시간 (밀리초) 이내에 응답이 없으면 리턴 */
										   FALSE);			/* WSAEventSelect에서는 항상 FALSE */
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
	/* 구체적인 Network Event 알아내기 */
	u32WsaRet	-= WSA_WAIT_EVENT_0;
	i32WsaRet	= WSAEnumNetworkEvents(m_sdClient,	/* 대상 소켓 */
									   m_wsaClient,	/* 대상 소켓과 짝을 이룬 객체 핸들 */
									   &wsaNet);	/* 이벤트 정보가 저장될 구조체 포인터 */
	/* 소켓이 에러인지 확인 */
	if (i32WsaRet == SOCKET_ERROR)
	{
		SetCycleTime(NORMAL_THREAD_SPEED);
		return 0x00;
	}

	/* 다음 스레드 동작 속도를 높이기 위한 처리 (데이터 수신모드일 경우) */
	SetCycleTime(0);
	/* Socket Event 별 분리 처리 진행 */
	if (FD_READ == (FD_READ & wsaNet.lNetworkEvents))
	{
		/* 통신 포트로부터 데이터 읽기 시도 */
		if (wsaNet.iErrorCode[FD_READ_BIT] == 0)
		{
			u8RetCode	= Received(m_sdClient) ? 0x03 : 0x02;
		}
		/* 데이터 읽기 실패인 경우, 기존 통신 포트의 버퍼 모두 읽어서 버리기 */
		else
		{
			ReceiveAllData(m_sdClient);
		}
	}
	/* 서버와 연결이 해제 혹은 기타 이벤트 (Send or Connected)인 경우, 강제 연결해제 처리 */
	else if (FD_CLOSE == (FD_CLOSE & wsaNet.lNetworkEvents))
	{
		LOG_ERROR(m_enSysID, L"The connection was lost from the remote server [Luria]");
		CloseSocket(m_sdClient);
		u8RetCode	= 0x02;
	}

	return u8RetCode;
}


/*
 desc : Server와의 접속이 이루어진 경우, 초기 한번 호출됨
 parm : sd	- [in]  접속이 이루어진 Client Socket Descriptor
 retn : None
*/
VOID CLuriaCThread::Connected(SOCKET sd)
{
	BOOL bSucc	= TRUE;
	LONG32 l32Event	= FD_READ|FD_CLOSE;

	/* 기존 송신될 패킷 모두 제거 */
	RemoveAllSendPkt();

	/* 일단 연결되었다는 시그널이 도착했으므로, DoWork의 연결시도를 막기 위함 */
	m_enSocket	= ENG_TPCS::en_linked;

	/* 연결되었으므로, 재접속을 하지 못하도록 시간을 갱신 */
	UpdateWorkTime();
	/* 소켓 옵션 설정하기 */
	if (bSucc)	bSucc = SetSockOptBuffSize(0x11, sd);	/* 8 KBytes가 표준임 */
//	if (bSucc)	bSucc = SetSockOptSpeed(1, sd);
	if (bSucc)	bSucc = SetSockOptRoute(1, sd);
//	if (bSucc)	bSucc = SetSockOptAlive(1, sd);
	/* 소켓 옵션 설정 실패 */
	if (bSucc)
	{
		/* Setup socket event (Combination of FD_READ and FD_CLOSE) */
		bSucc = (SOCKET_ERROR != WSAEventSelect(sd, m_wsaClient, l32Event));
		/* 접속 정보 리스트 컨트롤에 등록 */
		if (!bSucc)
		{
			SocketError(L"CLuriaCThread::Connected. An error occurred in the WSAEventSelect");
		}
	}

	/* 접속 허용이 성공적이면 가상함수 호출 */
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
			/* 연결 완료 상태 갱신 */
			UpdateLinkTime(0x01);
			/* 서버와 접속된 시간 저장 */
			GetLocalTime(&m_stLinkTime);
		}
	}
}

/*
 desc : 서버에 연결이된 경우
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

	/* CMPS Server 정보 전달 */
	if (m_pstConfLuria->use_announcement && m_pPktCM)
	{
		UINT8 u8IPv4[4]	= {NULL};
		CUniToChar csCnv;
		/* Announcement 사용 여부에 따라, CMPS Server 주소 설정 여부 */
		uvCmn_GetIPv4Uint32ToBytes(uvCmn_GetIPv4StrToUint32(m_tzSourceIPv4), u8IPv4);
		pPkt	= m_pPktCM->GetPktAnnounceServerIpAddr(ENG_LPGS::en_set, u8IPv4);
		/* 패킷 송신 */
		AddPktSend(pPkt, m_pPktCM->GetPktSize());	::Free(pPkt);
	}

	/* ----------------------------------------------------------------------------------------- */
	/*                                          System                                           */
	/* ----------------------------------------------------------------------------------------- */

	/* 무조건 H/W Init 요청 */
	if (m_pstConfLuria->use_hw_inited && m_pPktSS)
	{
		pPkt = m_pPktSS->GetPktInitializeHardware();
		AddPktSend(pPkt, m_pPktSS->GetPktSize());	::Free(pPkt);
	}
	/* System Status 요청 */
	if (m_pPktSS)
	{
		pPkt = m_pPktSS->GetPktSystemStatus();
		AddPktSend(pPkt, m_pPktSS->GetPktSize());	::Free(pPkt);
	}

	if (!m_bInitSend)	return TRUE;

	/* ----------------------------------------------------------------------------------------- */
	/*                                      Machine Config                                       */
	/* ----------------------------------------------------------------------------------------- */

	/* 광학계 개수 / Pitch (Stripe 개수) / 스크롤 속도 / 광학계 회전 여부 설정 및 얻기 */
	pPkt = m_pPktMC->GetPktTotalPhotoheads(ENG_LPGS::en_set, m_pstConfLuria->ph_count);
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
	pPkt = m_pPktMC->GetPktPhotoheadPitch(ENG_LPGS::en_set, m_pstConfLuria->ph_pitch);
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
	pPkt = m_pPktMC->GetPktScrollRate(ENG_LPGS::en_set, m_pstConfLuria->scroll_rate);
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
	pPkt = m_pPktMC->GetPktPhotoheadRotate(ENG_LPGS::en_set, m_pstConfLuria->ph_rotate);
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);

	/* 광학계 IPv4 */
	for (i=0; i<m_pstConfLuria->ph_count; i++)
	{
		pPkt	= m_pPktMC->GetPktPhotoheadIpAddr(ENG_LPGS::en_set, i+1, m_pstConfLuria->ph_ipv4[i]);
		AddPktSend(pPkt, m_pPktMC->GetPktSize());
		::Free(pPkt);
	}
	/* 광학계 Offset X / Y 값 설정 (2 번 광학계 부터) */
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
		/* Motion Drived ID 설정 */
		u8TableX= (i == 0x00) ? m_pstConfLuria->x_drive_id_1 : m_pstConfLuria->x_drive_id_2;
		u8TableY= (i == 0x00) ? m_pstConfLuria->y_drive_id_1 : m_pstConfLuria->y_drive_id_2;
		pPkt	= m_pPktMC->GetPktXYDriveId(ENG_LPGS::en_set, i+1, u8TableX, u8TableY);
		AddPktSend(pPkt, m_pPktMC->GetPktSize());
		::Free(pPkt);
		/* 모션 기준으로 현재 노광 시작 위치 값 요청 */
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

	/* Emulater 동작 여부 설정 (Motion / Photohead / Trigger) 및 Debugg Print Level */
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
	/* HysteresisType1 값 요청 */
	pPkt	= m_pPktMC->GetPktHysteresisType1(ENG_LPGS::en_set, m_pstConfLuria->hys_type_1_scroll_mode,
											  m_pstConfLuria->hys_type_1_negative_offset,
											  (UINT32)ROUNDED(m_pstConfLuria->hys_type_1_delay_offset[0]*1000000.0f, 0),
											  (UINT32)ROUNDED(m_pstConfLuria->hys_type_1_delay_offset[1]*1000000.0f, 0));
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
	/* 광학계 Product ID */
	pPkt = m_pPktMC->GetPktProductId(ENG_LPGS::en_get);
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
	/* Focus Z Drive Type */
	pPkt = m_pPktMC->GetPktZdriveType(ENG_LPGS::en_set, m_pstConfLuria->z_drive_type);
	AddPktSend(pPkt, m_pPktMC->GetPktSize());	::Free(pPkt);
	/* Artwork Complexity 설정 */
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
#if 0	/* 장비 (GUI) 쪽에서 설정하도록 유도 */
	/* 만약 Emulated Mode로 동작 중이라면.... 초기화 필수 */
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
 desc : 수신된 데이터 처리
 parm : tick	- [in]  현재 CPU 시간
 retn : None
*/
VOID CLuriaCThread::PopPktData(UINT64 tick)
{
	/* 동기 진입 */
	if (m_syncPktRecv.Enter())
	{
		LPG_PCLR pstPktRecv	= m_pRecvQue->PopPktData();
		/* 수신된 데이터가 존재하는지 주기적으로 확인 */
		if (pstPktRecv)
		{
			/* 최근에 수신된 데이터가 존재하는 경우, 수신받은 시간 저장 */
			m_u64RecvTime	= tick;
			PktAnalysis(pstPktRecv);
			/* user_data 메모리 할당 해제 */
			if (pstPktRecv && pstPktRecv->user_data)	delete [] pstPktRecv->user_data;
		}
		/* 동기 해제 */
		m_syncPktRecv.Leave();
	}
}

/*
 desc : 수신된 데이터 분석 처리
 parm : data	- [in]  수신된 데이터가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
BOOL CLuriaCThread::PktAnalysis(LPG_PCLR data)
{
	UINT16 u16Size	= 0;

	/* 읽기/쓰기 요청에 대한 응답 중 에러가 발생인 경우 */
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
		/* 실제 본문 데이터 즉 User Data 크기 계산 */
		u16Size	= data->num_bytes - UINT16(sizeof(STG_PCLR) - sizeof(PUINT8));
		/* 이전에 쓰기 요청에 대한 응답이 왔다고 플래그 설정만 변경 */
		/* !!! 읽기 요청에 대한 처리만 진행... WriteReply에 대한 처리는 하지 않음 !!! */
		if (data->command_type == UINT8(ENG_LTCT::en_read_reply) ||
			data->command_type == UINT8(ENG_LTCT::en_announce))
		{
			m_pstShMemLuria->RecvCMD(data->user_data_family, data->user_data_id);
			/* 일기 요청에 대한 응답인 경우 (GetJobList의 경우, 등록된 데이터가 없는 경우 u32Size = 0 임)*/
			/*if (u16Size > 0)	*/SetRecvPacket(ENG_TPCS::en_recved, (PUINT8)data);
		}
	}

	return TRUE;
}

/*
 desc : Luria Server에 Photohead LED On-Time 값 요청
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLuriaCThread::ReqGetPhLedOnTimeAll()
{
	BOOL bSucc	= TRUE;
	PUINT8 pPkt	= NULL;

	/* Photohead 파장 대별 즉 4파장을 순차적으로 요청하기 위함 */
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
 desc : Luria Server에 Photohead LED Temperature 값 요청 - 전체 Photohead 중 주파수 별 값 요청
 parm : freq_no	- [in]  전체 Photohead의 주파주 번호 (0:365, 1:385, 2:395, 4:405)
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
 desc : Luria Server에 Photohead LED Temperature 값 요청
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLuriaCThread::ReqGetPhLedTempAll()
{
	BOOL bSucc	= TRUE;
	PUINT8 pPkt	= NULL;

	/* Photohead 파장 대별 즉 4파장을 순차적으로 요청하기 위함 */
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
 desc : 소켓 데이터 송신
 parm : data	- [in]  전송할 데이터가 저장된 버퍼 포인터
		size	- [in]  'data' 버퍼에 저장된 데이터의 크기
 retn : TRUE - 송신 성공(비록 송신된 데이터가 없더라도),
		FALSE - 송신 실패(소켓 에러... 연결을 해제해야 함)
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

	/* 패킷 크기가 너무 작으면 에러 처리 */
	if (size < 0x06)
	{
		LOG_ERROR(m_enSysID, L"Optic. Service : Packet size too small (size < 0x06)");
		return FALSE;
	}

	/* 송신 패킷 중 패킷의 Family ID와 User ID 그리고 패킷 요청 or 설정 여부 분석하여 필요한 정보만 설정 */
	for (; i<size; i++)
	{
		/* 1 명령어의 패킷 크기 추출 */
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
		/* 다음 패킷 명령 위치로 이동 */
		pPktData	+= u16PktSize;
	}

	/* ---------------------------------------------------------------------------- */
	/* 읽기 요청에 대한 송신 패킷에 대해서만, 수신된 패킷이 아직 없다고 플래그 설정 */
	/* ---------------------------------------------------------------------------- */
	if (u8CmdType == (UINT8)ENG_LTCT::en_read)
	{
		/* 송신에 대한 응답 패킷이 있으면, 수신 받을 준비가 되어 있다고 설정 */
		if (!IsWriteOnlyCmd(u8FamilyID/*family_id*/, u8UserID/*user_id*/))
		{
			m_pstShMemLuria->SendCMD(u8FamilyID/*family_id*/, u8UserID/*user_id*/);
		}
	}

	/* 현재 연결되었는지 여부에 따라 */
	if (!IsConnected())	return FALSE;

	/* 동기 진입 */
	if (m_syncPktSend.Enter())
	{
		if (m_lstPktSend.GetCount() < MAX_LURIA_PACKET_COUNT)
		{
			/* 버퍼 메모리 할당 및 복사 */
			stPktSend.pkt_buff		= (PUINT8)::Alloc(size+1);
			stPktSend.pkt_buff[size]= 0x00;
			memcpy(stPktSend.pkt_buff, data, size);	/* The base size of sending packet */
			stPktSend.pkt_size		= size;

			/* 입력된 패킷이 내부에서 주기적으로 송신되는 패킷이면, 맨 마지막에 등록 */
			m_lstPktSend.AddTail(stPktSend);
			bSucc	= TRUE;
#if 0
			/* 입력된 패킷이 외부에서 직접 송신하는 패킷이면, 주기적으로 송신되는 패킷 보다 앞에 배치 */
			else
			{
				/* 전달 예정으로 등록된 패킷이 하나도 없는 경우인지 먼저 확인 */
				if (m_lstPktSend.GetCount() < 1)
				{
					m_lstPktSend.AddTail(stPktSend);
				}
				else
				{
					/* 1 개 이상 등록이 되어 있다면 ... */
					pPos	= m_lstPktSend.GetHeadPosition();
					while (pPos)
					{
						pOldPos		= pPos;	/* 현재 위치 임시 저장 */
						stPktSave	= m_lstPktSend.GetNext(pPos);
						/* 현재 검색된 위치의 패킷 데이터가 주기적으로 송신되는 패킷인지 확인 */
						if (0x01 == stPktSave.type)
						{
							m_lstPktSend.InsertBefore(pOldPos, stPktSend);
							break;	/* 루프 빠져나가기 */
						}
						/* 만약 더 이상 검색할 항목이 없다면, 매 마지막에 등록 */
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
		/* 동기 해제 */
		m_syncPktSend.Leave();
	}

	return bSucc;
}

/*
 desc : 주기적으로 요청하는 패킷 처리 루틴 (시스템 상태, 거버 적재 상태, 노광 진행 상태 등등)
 parm : None
 retn : TRUE (송신한 데이터가 있다) or FALSE (아무것도 송신한 데이터가 없다)
*/
BOOL CLuriaCThread::ReqPeriodSendPacket()
{
	STM_LPSB stPktSend	= {NULL};
	STG_PCLS stPktData	= {NULL};

	/* 기본적으로 리스트에 송신 패킷이 저장되어 있으면, 무조건 먼저 송신 처리 진행 */
	if (m_lstPktSend.GetCount())
	{
		/* 송신될 패킷 1개 가져오기 */
		stPktSend	= m_lstPktSend.GetHead();
		/* 송신 패킷 로그 전달 */
		memcpy(&stPktData, stPktSend.pkt_buff, sizeof(STG_PCLS));
		stPktData.SwapNetworkToHost();

		/* 패킷 데이터 전송 */
		if (!ReSended(stPktSend.pkt_buff, stPktSend.pkt_size, 3, 100))
		{
			TCHAR tzMesg[256]	= {NULL};
			swprintf_s(tzMesg, 256,
						L"Any number of send operations (3 times) failed. "
						L"[fid=%02x / uid=%02x / cmd_type=%02x / Remote IPv4:%s]",
						stPktData.user_data_family, stPktData.user_data_id,
						stPktData.command_type, m_tzTargetIPv4);
			/* 이전에 패킷 송신 실패한 시간보다 주기가 너무 짧으면 부모에게 알리지 않음 */
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
		/* 전송 실패건 성공이건, 리스트에서 패킷 데이터 한개 제거 */
		if (stPktSend.pkt_buff)	::Free(stPktSend.pkt_buff);
		m_lstPktSend.RemoveHead();
	}

	/* ---------------------------------------------------------------------------- */
	/* !!! 앞으로 송신할 데이터가 더 있는지 혹은 강제로 송신할 수 없는 상태인지 !!! */
	/* ---------------------------------------------------------------------------- */
	return (m_lstPktSend.GetCount() > 0);
}

/*
 desc : 패킷 수신 처리
 parm : type	- [in]  이벤트 종류 (ENG_TPCS)
		data	- [in]  송신 or 수신된 데이터가 저장된 구조체 포인터
 retn : None
*/
VOID CLuriaCThread::SetRecvPacket(ENG_TPCS type, PUINT8 data)
{
	UINT16 u16Size	= 0;
	LPG_PCLR pstPkt	= LPG_PCLR(data);

	/* 수신 데이터만 취급 */
	if (ENG_TPCS::en_recved != type || !pstPkt)
	{
		LOG_ERROR(m_enSysID, L"Failed to recv the success packet");
		return;
	}

	/* 패킷 제외하고, 데이터부 크기만 계산 */
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
	/* 초기화되지 않은 상태 즉, 접속한 이후 현재 선택된 거버 요청에 대해 응답이 수신된 경우 */
	/* 최초 한 번은 현재 선택된 거버의 상세 정보(user_data_id: 0x07, 0x08, 0x08, 0x0a) 요청 */
	/* ------------------------------------------------------------------------------------ */
	if (!m_bLuriaInited &&
		pstPkt->user_data_family == (UINT8)ENG_LUDF::en_job_management &&
		pstPkt->user_data_id == (UINT8)ENG_LCJM::en_get_max_jobs)
	{
		/* 현재 선택된 거버의 상세 정보 요청 (Fiducial Info. 포함됨) */
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

		/* Luria Service가 초기화 되었다고 설정 */
		m_bLuriaInited	= TRUE;
		/* !!! 기존 발생된 에러 코드 값 초기화 !!! */
		m_pstShMemLuria->link.u16_error_code	= 0x0000;
	}

	/* Luria 로부터 수신된 명령어 정보만 로그에 저장 */
	if (GetConfig()->set_comn.comm_log_optic)
	{
		TCHAR tzMesg[128]	= {NULL};
		swprintf_s(tzMesg, 128,
				   L"UVDI15 vs. OPTIC [Recv] [called_func=SetRecvPacket][fid=0x%02x,uid=0x%02x][size=%d]",
				   pstPkt->user_data_family, pstPkt->user_data_id, u16Size);
		LOG_MESG(m_enSysID, tzMesg);
	}
#if 0	/* 이미 이 함수를 호출하는 부모 함수에서 처리하고 있음 */
	/* 가장 최근에 수신된 패킷 명령어만 저장 (Family ID & User ID) */
	m_pstShMemLuria->RecvCMD(pstPkt->user_data_family, pstPkt->user_data_id);
#endif
}

/*
 desc : 연결 상태 정보 갱신
 parm : flag	- [in]  0x00: 연결 해제 상태, 0x01: 연결된 상태
 retn : None
*/
VOID CLuriaCThread::UpdateLinkTime(UINT8 flag)
{
	BOOL bUpate	= FALSE;

	/* 현재 연결 해제 상태이고, 이전에 연결된 상태라면 */
	if (!flag && m_pstShMemLuria->link.is_connected)		bUpate	= TRUE;
	/* 현재 연결 상태이고, 이전에 해제된 상태라면 */
	else if (flag && !m_pstShMemLuria->link.is_connected)	bUpate	= TRUE;

	/* 이전 상태와 다른 경우라면, 정보 변경 */
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
 desc : 송신 버퍼가 꽉 차있는 상태인지 여부
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLuriaCThread::IsSendBuffFull()
{
	return (UINT32)m_lstPktSend.GetCount() >= MAX_LURIA_PACKET_COUNT;
}

/*
 desc : 송신 버퍼에 남아 있는 데이터 개수 비율
 parm : None
 retn : 송신 버퍼의 여유 공간 즉, 더 송신할 수 있는 비율 (단위: %)
*/
FLOAT CLuriaCThread::GetSendBuffEmptyRate()
{
	return (FLOAT)(100.0f - (FLOAT)m_lstPktSend.GetCount() / FLOAT(MAX_LURIA_PACKET_COUNT));
}
