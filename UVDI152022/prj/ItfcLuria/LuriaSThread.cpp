
/* desc : Luria Server Thread 즉, CMPS: Server, Luria: Client */

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
 desc : 생성자
 parm : luria		- [in]  Luria 통신 정보
		conf		- [in]  Luria 환경 정보
		shmem		- [in]  Luria Shared Memory
 retn : None
 참고 :	th_id		- [in]  Thread ID
		source_ip	- [in]  Local IPv4
		target_ip	- [in]  Remote IPv4
		tcp_port	- [in]  TCP/IP Port
		port_buff	- [in]  TCP/IP Port Buffer Size
		recv_buff	- [in]  Send or Recv의 패킷 중 가장 큰 패킷의 크기 (단위: Bytes)
		ring_buff	- [in]	Max Recv Packet Size
		sock_time	- [in]  소켓 이벤트 (송/수신) 감지 대기 시간 (단위: 밀리초)
							해당 주어진 값(시간)만큼만 소켓 이벤트 감지후 다음 단계로 루틴 이동
		idle_time	- [in]  주어진 시간 동안 연결이 없는, Listen 소켓을 재초기화 하는 시간 (단위: 밀리초)

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
	m_u32IdleTime	= luria->link_time;	// Listen Socket 주기적으로 초기화하는 시간 설정 (단위: 밀리초)
	m_pClient		= client;
	m_pPktJM		= job;
	m_pPktEP		= exposure;
	m_pPktSS		= system;
	m_pPktCM		= commgt;
	m_u32AnnounceCount	= 0;
	// 소켓 초기화 및 이벤트 생성
	m_sdListen		= INVALID_SOCKET;
	m_sdClient		= INVALID_SOCKET;
	m_wsaListen		= WSACreateEvent();
	if (WSA_INVALID_EVENT == m_wsaListen)
		AfxMessageBox(L"Failed to create the socket event", MB_ICONSTOP|MB_TOPMOST);
	/* Check whether it operates in demo mode */
	if (GetConfig()->IsRunDemo())	m_enSysID = ENG_EDIC::en_demo;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CLuriaSThread::~CLuriaSThread()
{    
	/* Release Socket 및 Event */
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

	/* Socket Event 감지처리 */
	u8PopRet = PopSockEvent();
	if (u8PopRet == 1 || u8PopRet == 2)
	{
		if (0x01 == u8PopRet)	/* 연결된 Client가 존재하는 경우 */
		{
			/* 0. 먼저 연결 초기화 함수 호출 후 */
			Accepted(m_sdListen);
			/* 1. 연결 되었다고 플래그 설정 */
			m_bIsLinked	= TRUE;
		}
		else	/* 해제된 Client가 존재하는 경우 */
		{
			TCHAR tzMesg[128]	= {NULL};
			swprintf_s(tzMesg, 128,
					   L"Disconnected from the remote system (src:%s tgt:%s) / err_cd:%d",
					   m_tzSourceIPv4, m_tzTargetIPv4, m_i32SockError);
			LOG_ERROR(m_enSysID, tzMesg);
		}
	}

	/* 수신된 데이터가 존재하는지 주기적으로 확인 */
	LPG_PCLR pstPktRecv = m_pRecvQue->PopPktData();
	if (pstPktRecv)
	{
		PktAnalysis(pstPktRecv);
		/* user_data 메모리 할당 해제 */
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
	/* 부모 쪽의 CloseSocket 함수 호출하기 전에 수행할 업무 진행 */
	/* --------------------------------------------------------- */
	/* 상위 부모 함수 호출 */
	CLuriaThread::CloseSocket(sd, time_out);

	/* Socket 종류에 따라 초기화 방법 다르게 처리 */
	if (sd == m_sdListen)	m_sdListen	= INVALID_SOCKET;
	if (sd == m_sdClient)	m_sdClient	= INVALID_SOCKET;
}

/*
 desc : 소켓 초기화 즉, Server의 Listen socket 초기화 작업 진행
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

	/* 기존 Listen 소켓이 초기화되지 않았다면 ... */
	if (INVALID_SOCKET != m_sdListen)
	{
		CloseSocket(m_sdListen);
	}
	/* 기존 Client 소켓이 초기화되지 않았다면 ... */
	if (INVALID_SOCKET != m_sdClient)
	{
		CloseSocket(m_sdClient);
	}

	// Setupt the self socket
	stAddrIn.sin_family		= AF_INET;
	stAddrIn.sin_port		= htons(m_u16TcpPort);
	/* 물리적인 Land Card의 IPv4 주소 입력하면 되지만, 가상으로 부여한 IPv4는 실패 함 */
#if 0
//	InetPtonW(AF_INET, m_tzSourceIPv4, &stAddrIn.sin_addr.s_addr);
//	InetPtonW(AF_INET, L"192.168.105.35", &stAddrIn.sin_addr.s_addr);
//	InetPtonW(AF_INET, L"100.100.100.43", &stAddrIn.sin_addr.s_addr);
#else
	stAddrIn.sin_addr.s_addr= htonl(INADDR_ANY);
#endif

	/* 소켓 생성 */
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
	/* 소켓 옵션 설정 - IP 주소 재사용 */
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
	/* 소켓 및 설정된 주소 바인딩 */
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
	/* 서버 대기 설정. 동시 5 개의 연결 보장 */
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
	/* 소켓 이벤트 설정 (FD_ACCEPT만 설정!!!) */
	if (bSucc)
	{
		bSucc = (SOCKET_ERROR != WSAEventSelect(m_sdListen, m_wsaListen, l32Event));
	}

	UpdateWorkTime();

	/* 실패한 경우 */
	if (!bSucc)
	{
		m_sdListen	= INVALID_SOCKET;
		m_enSocket	= ENG_TPCS::en_link_failed;
	}
	else
	{
		/* 현재 연결을 기다리고 있다고 설정 */
		m_enSocket	= ENG_TPCS::en_linking;
	}

	/* 부모에게 알림 */
	return bSucc;
}

/*
 desc : 소켓 이벤트 감지
 parm : None
 retn : 0x00 - 실패, 0x01 - 접속, 0x02 - 해제, 0x03 - 데이터 수신
*/
UINT8 CLuriaSThread::PopSockEvent()
{
	UINT8 u8RetCode		= 0x00;
	INT32 i32WsaRet		= 0;
	UINT32 u32WsaRet	= 0;
	SOCKET sdSock[2]	= {m_sdListen, m_sdClient};
	WSAEVENT wsaSock[2]	= {m_wsaListen, m_wsaClient};
	WSANETWORKEVENTS wsaNet;

	/* Socket Event의 Signal 상태 감지하기 */
	u32WsaRet	= WSAWaitForMultipleEvents(2,				/* 소켓 이벤트 감지 개수: 1개임 */
										   wsaSock,			/* 소켓 이벤트 핸들 (Listen or Client) */
										   FALSE,			/* TRUE : 모든 소켓 이벤트 객체가 신호 상태, FALSE : 1개 이벤트 객체가 신호 상태이면 즉시 리턴 */
										   m_u32SockTime,	/* 임의 초 이내에 응답이 없으면 리턴 */
										   FALSE);			/* WSAEventSelect에서는 항상 FALSE */
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
	/* 이벤트 객체가 신호 상태가 될 때까지 대기한다. WSAWaitForMultipleEvents() 함수의 반환 값은 */
	/* 신호 상태가 된 이벤트 객체의 배열 인덱스 + WSA_WAIT_EVENT_0 값이다. 그러므로, 실제 인덱스 */
	/* 값을 얻으려면 WSA_WAIT_EVENT_0 (WAIT_BOJECT_0) 값을 빼야 한다.                            */
	/* ----------------------------------------------------------------------------------------- */
	u32WsaRet	-= WSA_WAIT_EVENT_0;
	i32WsaRet	= WSAEnumNetworkEvents(sdSock[u32WsaRet],	/* 대상 소켓 */
									   wsaSock[u32WsaRet],	/* 대상 소켓과 짝을 이룬 객체 핸들 */
									   &wsaNet);			/* 이벤트 정보가 저장될 구조체 포인터 */
	/* 소켓이 에러인지 확인 */
	if (i32WsaRet == SOCKET_ERROR)	return 0x00;

	/* Socket Event 별 분리 처리 진행 */
	if (FD_READ == (FD_READ & wsaNet.lNetworkEvents))
	{
		/* 통신 포트로부터 데이터 읽기 시도 */
		if (wsaNet.iErrorCode[FD_READ_BIT] == 0)
		{
			if (Received(sdSock[1]))	u8RetCode = 0x03;
		}
		/* 데이터 읽기 실패인 경우, 기존 통신 포트의 버퍼 모두 읽어서 버리기 */
		else
		{
			ReceiveAllData(sdSock[1]);
		}
	}
	/* 클라이언트로 연결이 들어온 상태인지 ... */
	else if (FD_ACCEPT == (FD_ACCEPT & wsaNet.lNetworkEvents))
	{
		/* 연결 에러가 발생 했는지 여부 */
		if (wsaNet.iErrorCode[FD_ACCEPT_BIT] != 0)
		{
			m_i32SockError = WSAGetLastError();
		}
		/* 연결이 서버로부터 정상적으로 승인받은 경우 */
		else
		{
			u8RetCode	= 0x01;
		}
	}
	/* 서버와 연결이 해제인지 */
	else if (FD_CLOSE == (FD_CLOSE & wsaNet.lNetworkEvents))
	{
		CloseSocket(sdSock[1]);
		u8RetCode	= 0x02;
	}

	return u8RetCode;
}

/*
 desc : Client로부터 접속 시그널이 도착한 경우
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
	/* 연결된 클라이언트가 있으므로, 일단 연결되었다고 플래그 설정 */
	/* ----------------------------------------------------------- */
	m_enSocket = ENG_TPCS::en_linked;
	/* ------------------------------------------------------- */
	/* 연결되었으므로, 소켓 초기화를 하지 못하도록 시간을 갱신 */
	/* ------------------------------------------------------- */
	UpdateWorkTime();

	/* ------------------------------------------------- */
	/* 만약 기존에 접속된 Client가 있다면 연결 해제 처리 */
	/* ------------------------------------------------- */
	if (INVALID_SOCKET != m_sdClient)
	{
#if 0	/* Announcement 모드로 동작하게되면, 희안하게 Luria Server가 일방적으로 통신 끊고, 다시 재접속해 옴 */
		swprintf_s(tzMsg, 128,
				   L"Previously connected to a remote computer [Local:%s, Remote:%s]",
				   m_tzSourceIPv4, m_tzTargetIPv4);
		LOG_ERROR(m_enSysID, tzMsg);
		CloseSocket(m_sdClient);
#else
		/* 소켓 해제 및 초기화 */
		closesocket(m_sdClient);
		m_sdClient	= INVALID_SOCKET;
#endif
	}

	/* 새롭게 접속된 클라이언트에 대해 일단 접속 승인 처리 */
	sdClient	= WSAAccept(sd, (LPSOCKADDR)&sdAddrIn, &i32AddrLen, 0, 0);
	memcpy(&stInAddr.s_addr, &sdAddrIn.sin_addr.s_addr, sizeof(UINT32));
	/* 접속 승인할 때, 에러가 발생 했는지 여부 확인 */
	if (INVALID_SOCKET == sdClient)
	{
		swprintf_s(tzMesg, 128,	L"Connection approval oeration failed "
								L"[Error:0x%0x, Local:%s, Remote:%s]",
				   WSAGetLastError(), m_tzSourceIPv4, m_tzTargetIPv4);
		LOG_ERROR(m_enSysID, tzMesg);
		return;
	}
#if 0	/* Luria Server 로부터 빈번히 접속해 들어옴. 대략 난감 */
	/* -------------------------------------------------------------------------- */
	/* 접속된 클라이언트가 기존의 등록된 Client IPv4 주소가 아니면 접속 해제 처리 */
	/* -------------------------------------------------------------------------- */
	InetNtopW(AF_INET, (CONST VOID*)&stInAddr, tzIPv4, IPv4_LENGTH);
	if (bSucc && (0 != wcscmp(m_tzTargetIPv4, tzIPv4)))
	{
		// 현재 연결된 Client 접속 해체 처리
		CloseSocket(sdClient);
		swprintf_s(tzMsg, 128, L"The approved client is not an authorized system "
							   L"[Local:%s, Remote:%s]", m_tzSourceIPv4, m_tzTargetIPv4);
		LOG_ERROR(m_enSysID, tzMsg);
		return;
	}
#endif
	/* --------------------------------------- */
	/* 현재 연결된 소켓의 다양한 옵션 설정하기 */
	/* --------------------------------------- */
	if (bSucc)	bSucc = SetSockOptBuffSize(0x11, sdClient);	/* 8 KBytes가 표준임 */
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
		/* 소켓 이벤트 설정 (FD_CLOSE|FD_READ 설정!!!) */
		bSucc = (SOCKET_ERROR != WSAEventSelect(sdClient, m_wsaClient, l32Event));
		if (!bSucc)
		{
			LOG_ERROR(m_enSysID, L"Socket Asynchronous Setting (FD_CLOSE|FD_READ) failed");
		}
#if 0
		/* 반드시 값 설정해 줄 것*/
		else bSucc = TRUE;
#endif
	}

	/* Ring Buffer 초기화 */
	m_pPktRingBuff->ResetBuff();
	/* 통신 시간 갱신 */
	UpdateWorkTime();

	/* 접속 허용이 성공적이면 가상함수 호출 */
	if (!bSucc)
	{
		CloseSocket(sdClient);
	}
	else
	{
		/* 가장 최근에 접속한 Client의 소켓 핸들 저장 */
		m_sdClient	= sdClient;	// 반드시 Accepted 함수 이전에 설정(저장)되어야 함
		/* 클라이언트가 접속한 시간 저장 */
		GetLocalTime(&m_stLinkTime);
	}
}

/*
 desc : 현재 연결된 Client Socket 연결 해제 처리
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
 desc : 현재 Server Socket 해제 처리
 parm : None
 retn : None
*/
VOID CLuriaSThread::CloseServer()
{
	if (INVALID_SOCKET != m_sdListen)	CloseSocket(m_sdListen);
	if (INVALID_SOCKET != m_sdClient)	CloseSocket(m_sdClient);
}

/*
 desc : 수신된 데이터 분석 처리
 parm : data	- [in]  수신된 데이터가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
BOOL CLuriaSThread::PktAnalysis(LPG_PCLR data)
{
	TCHAR tzMesg[512]	= {NULL};
	INT32 i32Size		= 0;

	/* Announcemnet가 아닌지 여부 확인 */
	if (data->user_data_family != (UINT8)ENG_LUDF::en_announcement)
	{
		swprintf_s(tzMesg, 128, L"Invalid family ID value. f_id = 0x%02x [Local:%s, Remote:%s]",
				   data->user_data_family, m_tzSourceIPv4, m_tzTargetIPv4);
		LOG_WARN(m_enSysID, tzMesg);
		return TRUE;
	}

	/* 송신 or 수신된 User Data 부 크기 */
	i32Size	= data->num_bytes - (sizeof(STG_PCLR) - sizeof(PUINT8));
	if (i32Size < 0)
	{
		LOG_WARN(m_enSysID, L"Received packet header too small");
		return TRUE;
	}

	/* 무조건 Announcement 패킷만 수신 처리 */
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
 desc : 시스템 상태 정보 요청
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLuriaSThread::ReqSystemStatus()
{
	UINT32 u32Size	= 0;
	PUINT8 pPkt		= NULL;

	/* System 상태 정보 요청 패킷 생성 */
	pPkt	= m_pPktSS->GetPktSystemStatus();
	u32Size	= m_pPktSS->GetPktSize();
	m_pClient->AddPktSend(pPkt, u32Size);
	delete pPkt;

	return TRUE;
}

/*
 desc : AnnouncementStatus 요청
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLuriaSThread::ReqAnnouncementStatus()
{
	UINT32 u32Size	= 0;
	PUINT8 pPkt		= NULL;

	/* System 상태 정보 요청 패킷 생성 */
	pPkt	= m_pPktCM->GetPktAnnouncementStatus();
	u32Size	= m_pPktCM->GetPktSize();
	if (m_pClient)	m_pClient->AddPktSend(pPkt, u32Size);
	delete pPkt;

	return TRUE;
}