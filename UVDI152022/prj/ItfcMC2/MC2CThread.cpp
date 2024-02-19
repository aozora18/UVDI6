
/* desc : Luria Server Thread 즉, CMPS: Client, Luria: Server */

#include "pch.h"
#include "MC2CThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

/*
 desc : 생성자
 parm : config		- [in]  환경 설정 정보
		shmem		- [in]  MC2 Service 관련 공유 메모리
		source_ip	- [in]  Local IPv4
		target_ip	- [in]  Remote IPv4
		serial_no	- [in]  Serial number of the device or 0
		mc2_port	- [in]  TCP/IP Port (MC2에 데이터를 송신하기 위한 포트)
		cmps_port	- [in]  MC2로부터 CMPS에 응답받기 위한 통신 포트
		port_buff	- [in]  TCP/IP Port Buffer Size
		recv_buff	- [in]  Send or Recv의 패킷 중 가장 큰 패킷의 크기 (단위: Bytes)
		ring_buff	- [in]	Max Recv Packet Size
		idle_time	- [in]  이 시간(단위:밀리초) 동안 MC2와 통신 작업(송신 or 수신)이 없는 경우
							Client는 기존 통신 연결을 닫고 재접속 수행
							이 값이 0 인 경우, 이 기능을 사용하지 않는다.
		sock_time	- [in]  소켓 이벤트 (송/수신) 감지 대기 시간 (단위: 밀리초)
							해당 주어진 값(시간)만큼만 소켓 이벤트 감지후 다음 단계로 루틴 이동
 retn : None
*/
CMC2CThread::CMC2CThread(LPG_CMSI config,
						 LPG_MDSM shmem,
						 UINT16 mc2_port,
						 UINT16 cmps_port,
						 UINT32 source_ip,
						 UINT32 target_ip,
						 UINT32 serial_no,
						 UINT32 port_buff,
						 UINT32 recv_buff,
						 UINT32 ring_buff,
						 UINT32 idle_time,
						 UINT32 sock_time)
	: CMC2Thread(shmem,
				 mc2_port,
				 cmps_port,
				 source_ip,
				 target_ip,
				 port_buff,
				 recv_buff,
				 ring_buff,
				 sock_time)
{
	m_pstConfig		= config;
	m_u16ReqIndex	= 0x0000;
	m_bSendPeriodPkt= TRUE;
	m_u32SerialNo	= serial_no;
	m_u32IdleTime	= idle_time;
	m_u64PeriodTime	= 250;	/* 기본 값으로 빠르게 주기적으로 요청 */
	m_u64RecvTime	= 0;	/* 반드시 0 값으로 초기화 */
	m_u64SendTime	= 0;	/* 반드시 현재 시간으로 설정 */
	m_bConnected	= FALSE;
	m_bInitSocket	= FALSE;
	m_pPktReadBuff	= NULL;
	m_pPktReqWrite	= NULL;
	m_pstReqHead	= NULL;
	m_pstReqRead	= NULL;
	/* Check whether it operates in demo mode */
	if (GetConfig()->IsRunDemo())	m_enSysID = ENG_EDIC::en_demo;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CMC2CThread::~CMC2CThread()
{
	/* 패킷 버퍼 메모리 해제 */
	delete [] m_pPktReadBuff;
	delete [] m_pPktReqWrite;

	/* 기존 송신 패킷 메모리 해제 */
	m_lstSendPkt.RemoveAll();
}

/*
 desc : 스레드 초기 실행시, 최초 한번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::StartWork()
{
	/* 패킷 송신 버퍼 할당 */
	m_pPktReadBuff	= new UINT8 [PKT_SIZE_READ_OBJECT+1];	/* fixed : 26 bytes */
	memset(m_pPktReadBuff, 0x00, PKT_SIZE_READ_OBJECT+1);
	m_pPktReqWrite	= new UINT8 [512];	/* SD2S의 8 개 드라이브에 대한 모든 정보를 읽어오는데 충분 */
	memset(m_pPktReqWrite, 0x00, 512);

	/* 패킷 송신 버퍼에 구조체 포인터 연결 */
	m_pstReqHead	= (LPG_PUCH)m_pPktReadBuff;
	m_pstReqRead	= (LPG_PQRD)(m_pPktReadBuff + 10);

	/* UDP Header 기본 값 설정 */
	m_pstReqHead->return_port	= m_u16CMPSPort;					/* UDP target port of the client */
	m_pstReqHead->udp_msg_type	= (UINT8)ENG_MUMT::en_gen3_dnc;		/* read request -> fixed */
	m_pstReqHead->short_cycle	= 0;								/* 0 - Long Cycle, 1 = Short cycle 응답 반응 (?) */
	m_pstReqHead->u16_reserved	= 0;
	m_pstReqHead->send_cmd_cnt	= 0;								/* 서버에게 송신한 명령 횟수 or 구분자 (Client가 Server로 보낸 명령어이며, 응답 받을 때, 식별하기 위함) */
	/* Reading Object */
	m_pstReqRead->e_gen3_dnc_cmd= (UINT8)ENG_DCG3::en_req_read_obj;	/* read object value index -> fixed */
	m_pstReqRead->serial_no		= m_u32SerialNo;					/* MC2 Serial Number */

	return CMC2Thread::StartWork();
}

/*
 desc : 주기적으로 호출됨
 parm : None
 retn : None
*/
VOID CMC2CThread::RunWork()
{
	BOOL bIsConnect	= FALSE;
	UINT64 u64Tick	= GetTickCount64();

	/* 동기 진입 */
	if (m_syncPeriodPkt.Enter())
	{
		/* MC와 연결(?) 후 가장 최근에 송신한 시간과 수신된 시간 간격이 크면, 통신 연결해제로 봄 */
		/* 즉, 송신된 시간 이후 5초 이내로 수신된 시간이 없다면, 통신 연결해제로 봄 */
		if (!m_bConnected)
		{
			if (m_u32IdleTime && (m_u64RecvTime + m_u32IdleTime < u64Tick))	bIsConnect	= TRUE;
			else if (u64Tick > m_u64RecvTime + 10000 /* 10 초 */)			bIsConnect	= TRUE;
			if (bIsConnect)
			{
				/* ------------------------------------------------- */
				/* 접속 작업을 위해 Client 소켓을 초기화 했다고 알림 */
				/* ------------------------------------------------- */
				m_bInitSocket	= InitSocket(u64Tick);	/* 소켓이 초기화 되었다고 설정 */
			}
		}
		/* --------------------- */
		/* Socket Event 감지처리 */
		/* --------------------- */
 		if (0x02 == PopSockEvent())
 		{
 			TCHAR tzMesg[128]	= {NULL};
			CUniToChar csCnv1, csCnv2;
 			swprintf_s(tzMesg, 128,
					   L"The remote system has been disconnected (src:%s tgt:%s / err_cd:0x%x)",
					   m_tzSourceIPv4, m_tzTargetIPv4, m_i32SockError);
 			if (!IsRunDemo())	LOG_ERROR(m_enSysID, tzMesg);
 		}

		/* 주기적으로 송신될 패킷이 있는지 여부 확인 */
		if (m_bInitSocket)	SendPeriodPkt(u64Tick);

		/* 동기 해제 */
		m_syncPeriodPkt.Leave();
	}
}

/*
 desc : 주기적으로 패킷 송신하는 기능
 parm : tick	- [in]  현재 CPU 시간
 retn : None
 note : 패킷 송신에 대해 최소 응답 시간은 100 ms , 비정상 응답은 150 ms 임
*/
VOID CMC2CThread::SendPeriodPkt(UINT64 tick)
{
	BOOL bSended	= FALSE;

	/* 동기 진입 */
	if (m_syncPktSend.Enter())
	{
		/* ------------------------------------------------------------------- */
		/* 매우 중요. 200 msec (초당 5번  이상)이하로 데이터 요청하면 MC2 먹통 */
		/* ------------------------------------------------------------------- */
		/* 최근에 수신된 시간이 Period Time 보다 크다면, 무조건 다음 패킷 송신 */
		/* 최근에 송신된 패킷의 응답이 최소 200ms 이내에 없다면 다음 패킷 송신 */
		/* ------------------------------------------------------------------- */
		/* ~정말 중요~ MC2는 무조건 응답이 들어온 이후 다음 패킷을 송신해야 됨 */
		/* 응답이 확인하지 않고, 다음 명령을 송신하면, 이전 송신 명령은 무시됨 */
		/* ------------------------------------------------------------------- */
		if (m_u64SendTime+m_u64PeriodTime < tick)
		{
			/* 가장 최근에 송신한 시간 저장 */
			m_u64SendTime	= tick;
			/* 기존 등록된 패킷부터 송신 */
			if (m_lstSendPkt.GetCount() > 0)
			{
				/* 패킷 가져오기 */
				STM_PSDS stPktData	= m_lstSendPkt.GetHead();
				bSended = ReSended(stPktData.data, stPktData.size, 3, 100);
				if (bSended && stPktData.mode && GetConfig()->set_comn.comm_log_mc2)
				{
					TCHAR tzMesg[128]	= {NULL};
					PUINT8 pPktData		= stPktData.data;
					LPG_PUCH pstHead	= (LPG_PUCH)pPktData;	pPktData	+= sizeof(STG_PUCH);
					LPG_PQWD pstWrite	= (LPG_PQWD)pPktData;
					swprintf_s(tzMesg, 128,
							   L"UVDI15 vs. MC2 [Send] [called_func=SendPeriodPkt]"
							   L"[send_cmd_cnt=0x%08x,e_gen3_dnc_cmd=0x%02x][sub_id=0x%02x][data_len=%d]",
							   pstHead->send_cmd_cnt, pstWrite->e_gen3_dnc_cmd, pstWrite->sub_id, pstWrite->data_len);
					LOG_MESG(m_enSysID, tzMesg);
				}
				/* 송신된 패킷 메모리 해제 */
				m_lstSendPkt.RemoveHead();
			}
			else
			{
				/* 주기적인 패킷 데이터 송신 */
				ReadReqPacket();
			}
		}
		/* 동기해제 */
		m_syncPktSend.Leave();
	}
}

/*
 desc : Release tcp/ip socket
 parm : sd		- [in]  Socket descriptor
		time_out- [in]  Please refer to the LINGER structure (= linger time)
						If the value is 0, I disconnected immediately without any delay
 retn : None
*/
VOID CMC2CThread::CloseSocket(SOCKET sd, UINT32 time_out)
{
	/* --------------------------------------------------------- */
	/* 부모 쪽의 CloseSocket 함수 호출하기 전에 수행할 업무 진행 */
	/* --------------------------------------------------------- */
	m_bConnected	= FALSE;
	/* 연결 해제 상태 */
	UpdateLinkTime(0x00);

	// 상위 부모 함수 호출
	CMC2Thread::CloseSocket(sd, time_out);
	// Socket 종류에 따라 초기화 방법 다르게 처리
	if (sd == m_sdClient)	m_sdClient	= INVALID_SOCKET;
}

/*
 desc : Init Socket
 parm : tick	- [in]  CPU 현재 시간
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::InitSocket(UINT64 tick)
{
	INT32 i32Option		= 1;
	TCHAR tzMesg[128]	= {NULL};
	LONG32 l32Event		= FD_READ;
	SOCKADDR_IN stAddrIn= {NULL};

	/* 소켓이 초기화 되지 않았다고 설정 */
	m_bInitSocket	= FALSE;
	// Remove existing client socket exists
	if (INVALID_SOCKET != m_sdClient)
	{
		CloseSocket(m_sdClient);
	}

	// 접속을 시도하므로 플래그 설정
	m_enSocket		= ENG_TPCS::en_linking;
	m_u32AddrSize	= sizeof(SOCKADDR_IN);
	memset(&m_stSrvAddr, 0x00, m_u32AddrSize);

	// Create a client socket
	m_sdClient = socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == m_sdClient)	return FALSE;

	/* ----------------------- */
	/* Setup the server socket */
	/* ----------------------- */
	stAddrIn.sin_family			= AF_INET;
	stAddrIn.sin_port			= htons(m_u16CMPSPort);
	InetPtonW(AF_INET, m_tzSourceIPv4, &stAddrIn.sin_addr.s_addr);

	/* ----------------------- */
	/* Setup the client socket */
	/* ----------------------- */
	m_stSrvAddr.sin_family		= AF_INET;
	m_stSrvAddr.sin_port		= htons(m_u16MC2Port);	// In this case, the port number is a value of 0 at all times
	InetPtonW(AF_INET, m_tzTargetIPv4, &m_stSrvAddr.sin_addr.s_addr);

	/* Setup socketoption - reuse IPv4 */
	setsockopt(m_sdClient, SOL_SOCKET, SO_REUSEADDR, (char *)&i32Option, sizeof(i32Option));
	/* Socket binding ... */
	if (SOCKET_ERROR == ::bind(m_sdClient, (LPSOCKADDR)&stAddrIn, sizeof(SOCKADDR_IN)))
	{
		m_i32SockError = WSAGetLastError();
		swprintf_s(tzMesg, 128,
				   L"Socket binding error : 0x%x (Source IPv4=%s)", m_i32SockError, m_tzSourceIPv4);
		LOG_ERROR(m_enSysID, tzMesg);
		return FALSE;
	}

	// Setup socket event (FD_READ)
	if (SOCKET_ERROR == WSAEventSelect(m_sdClient, m_wsaClient, l32Event))	return FALSE;
	swprintf_s(tzMesg, 128,
			   L"Connected to UDP Port (src:%s tgt:%s)", m_tzSourceIPv4, m_tzTargetIPv4);
	LOG_MESG(m_enSysID, tzMesg);


	/* ----------------------------------------- */
	/* !!! Important !!! Send / Recv Time 초기화 */
	/* ----------------------------------------- */
	m_u64RecvTime	= tick;	/* 현재 CPU 시간으로 저장 (재-접속을 막기 위함) */
	m_u64SendTime	= 0;	/* 연결 초기화 후, 바로 송신하기 위해 0 값으로 초기화 */

	return TRUE;
}

/*
 desc : 소켓 이벤트 감지 대기
 parm : None
 retn : 0x00 - 실패, 0x01 - 접속, 0x02 - 해제, 0x03 - 데이터 수신
*/
UINT8 CMC2CThread::PopSockEvent()
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
	u32WsaRet	= WSAWaitForMultipleEvents(1,				// 소켓 이벤트 감지 개수: 1 개
										   &m_wsaClient,	// 소켓 이벤트 핸들
										   FALSE,			// TRUE : 모든 소켓 이벤트 객체가 신호 상태, FALSE : 1개 이벤트 객체가 신호 상태이면 즉시 리턴
										   m_u32SockTime,	// 임의 시간 (밀리초) 이내에 응답이 없으면 리턴
										   FALSE);			// WSAEventSelect에서는 항상 FALSE
#if 1
	if (u32WsaRet == WSA_WAIT_TIMEOUT || u32WsaRet == WSA_WAIT_FAILED)
#else
	if (u32WsaRet == WSA_WAIT_TIMEOUT)
#endif
	{
#if 1
		if (u32WsaRet == WSA_WAIT_FAILED)
		{
			TCHAR tzMesg[256]	= {NULL};
			swprintf_s(tzMesg, 256, L"The return value of the WSAWaitForMultipleEvents failed. "
									L"(time out) (tgt_ipv4=%s)", m_tzTargetIPv4);
			if (m_enSysID != ENG_EDIC::en_demo)	LOG_MESG(m_enSysID, tzMesg);
		}
#endif
		SetCycleTime(NORMAL_THREAD_SPEED);
		return 0x00;
	}
	// 구체적인 Network Event 알아내기
	u32WsaRet	-= WSA_WAIT_EVENT_0;
	i32WsaRet	= WSAEnumNetworkEvents(m_sdClient,			// 대상 소켓
									   m_wsaClient,			// 대상 소켓과 짝을 이룬 객체 핸들
									   &wsaNet);			// 이벤트 정보가 저장될 구조체 포인터
	// 소켓이 에러인지 확인
	if (i32WsaRet == SOCKET_ERROR)
	{
		SetCycleTime(NORMAL_THREAD_SPEED);
		return 0x00;
	}

	/* ----------------------------------------------------------------- */
	/* 다음 스레드 동작 속도를 높이기 위한 처리 (데이터 수신모드일 경우) */
	/* ----------------------------------------------------------------- */
	SetCycleTime(0);
	/* ------------------------------ */
	/* Socket Event 별 분리 처리 진행 */
	/* ------------------------------ */
	if (FD_READ == (FD_READ & wsaNet.lNetworkEvents))
	{
		/* 동기 진입 */
		if (m_syncPktRecv.Enter())
		{
			// 통신 포트로부터 데이터 읽기 시도
			if (wsaNet.iErrorCode[FD_READ_BIT] == 0)
			{
				if (Received(m_sdClient))
				{
					u8RetCode = 0x03;
					/* !!! 수신된 시간 갱신 !!! */
					m_u64RecvTime	= GetTickCount64();
					/* 수신된 데이터가 있으면, 연결된 것으로 간주 */
					m_bConnected	= TRUE;
					/* 연결 완료 상태 */
					UpdateLinkTime(0x01);
				}
			}
			/* 동기 해제 */
			m_syncPktRecv.Leave();
		}
	}
	// 서버와 연결이 해제 혹은 기타 이벤트 (Send or Connected)인 경우, 강제 연결해제 처리
#if 1
	else if (FD_CLOSE == (FD_CLOSE & wsaNet.lNetworkEvents))
#else
	else
#endif
	{
		CloseSocket(m_sdClient);
		u8RetCode	= 0x02;
	}

	return u8RetCode;
}

/*
 desc : 소켓 데이터 송신
 parm : data	- [in]  전송할 데이터가 저장된 버퍼 포인터
		size	- [in]  'data' 버퍼에 저장된 데이터의 크기
		mode	- [in]  주기적으로 요청하는 경우 (0x00), 외부 (수동)에서 요청한 경우 (0x01)
 retn : TRUE - 송신 성공(비록 송신된 데이터가 없더라도),
		FALSE - 송신 실패(소켓 에러... 연결을 해제해야 함)
*/
BOOL CMC2CThread::AddPktData(PUINT8 data, UINT32 size, UINT8 mode)
{
	BOOL bSucc			= FALSE;
	STM_PSDS stPktData	= {NULL};

	/* 동기 진입 */
	if (m_syncPktSend.Enter())
	{
		if (m_lstSendPkt.GetCount() > MAX_MC2_PACKET_COUNT)
		{
			/* 맨 먼저 등록된 패킷 제거 */
			m_lstSendPkt.RemoveHead();
			LOG_WARN(m_enSysID,
					 L"Older packets were removed because the number of packets was exceeded");
		}
		if (size < MAX_MC2_PACKET_SIZE)
		{
			/* 새로운 패킷 등록 */
			memcpy(stPktData.data, data, size);
			stPktData.size	= size;
			stPktData.mode	= mode;
			m_lstSendPkt.AddTail(stPktData);

			/* 다음 패킷 송신을 위해 명령어 횟수 값 증가 처리 : Server와 Client 간의 송/수신 식별 위함 */
			m_pstReqHead->NextCmdCount();

			bSucc	= TRUE;
		}
		/* 동기 해제 */
		m_syncPktSend.Leave();
	}

	return bSucc;
}

/*
 desc : 주기적으로 호출되는 함수
 parm : None
 retn : None
*/
VOID CMC2CThread::ReadReqPacket()
{
	/* 주기적으로 MC2의 상태 정보 요청 */
	switch (m_u16ReqIndex)
	{
	/* ------------------------------------------------------------- */
	/* PDOs (Process Data Objects) Reference Value Collective Object */
	/* ------------------------------------------------------------- */
	case 0x0000	:	ReadObjectValueByIndex(0, MAX_WANTED_READ_SIZE, 0, (UINT32)ENG_PSOI::en_ref_value_collective);	break;
	case 0x0001	:	ReadObjectValueByIndex(0, MAX_WANTED_READ_SIZE, 0, (UINT32)ENG_PSOI::en_act_value_collective);	break;
	}

	/* 패킷 데이터 송신 */
	if (AddPktData(m_pPktReadBuff, PKT_SIZE_READ_OBJECT, 0x00))
	{
	}
	/* 다음 명령어 요청을 위한 인덱스 증가 처리 */
	if (++m_u16ReqIndex == 2)	m_u16ReqIndex	= 0x0000;
}

/*
 desc : E_G3의 읽기 명령어 요청
		Reads the value of an object.
		The object is selected per object index.
		The response always includes the max. possible number of data bytes.
		The length of the response is limited by:
			-. the size of the object
			-. the length of user data of a UDP telegram of 1280 bytes
			-. the definition of the parameter <SizeWanted>.
 parm : sub_id		- [in]  SubID of the SD3 device (Drive ID)
		read_size	- [in]  Number of bytes to be read / 0 = all available
							This parameter limits the length of user data of a UDP telegram of 1280 bytes.
							If, beginning from the SubIn-dex, more data are included 
							in the object than defined in the parameter <SizeWanted>,
							only the data bytes defined in SizeWanted are read.
							If the value 0 or a value greater than 1280 is defined for SizeWanted,
							this limitation will be inactive and max. 1280 bytes will be read per read command.
		sub_index	- [in]  Reading shall be started at this SubIndex (Offset)
		index		- [in]  Index of the object to be read (Object ID)
 retn : None
*/
VOID CMC2CThread::ReadObjectValueByIndex(UINT8 sub_id,
										 UINT16 read_size,
										 UINT32 sub_index,
										 UINT32 index)
{
	STG_MPSD stPktSend			= {NULL};
	/* 읽기 본문 채우기 */
	m_pstReqRead->sub_id		= sub_id;
	m_pstReqRead->size_wanted	= read_size;
	m_pstReqRead->sub_index		= sub_index;
	m_pstReqRead->index			= index;

	/* 미리 송신될 패킷 정보 저장 */
	stPktSend.cmd				= m_pstReqRead->e_gen3_dnc_cmd;
	stPktSend.obj_index			= m_pstReqRead->index;
	stPktSend.sub_id			= m_pstReqRead->sub_id;
	stPktSend.sub_index			= m_pstReqRead->sub_index;
	/* 메모리 맵에 등록 */
	memcpy(&m_pstPktSend[m_pstReqHead->send_cmd_cnt], &stPktSend, sizeof(STG_MPSD));
}

/*
 desc : 쓰기 명령어 메모리 맵에 등록 (Object 0x3801, PDO Reference Value Collective Object)
 parm : sub_id		- [in]  SubID of the SD3 device (Drive ID)
		sub_index	- [in]  Writing shall be started at this SubIndex (Offset)
		index		- [in]  Index of the object to be read (Object ID)
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::WriteObjectValueByIndex(UINT8 sub_id, UINT32 sub_index, UINT32 index)
{
	STG_MPSD stPktSend	= {NULL};
#if 0
	/* 현재 MC2 State Bit 값 보고 Busy 상태이거나 Busy 상태가 아닌 경우, 특정 시간 동안 대기 후 처리 */
	if (!m_pObjAct->GetDrive(sub_id)->IsStopped())	return FALSE;
	/* Done Toggled Flag 값이 바뀔 때까지 일정 시간 대기 (0.5 초 대기) */
	CMC2Thread::Sleep(500);
#endif
	/* 미리 송신될 패킷 정보 저장 */
	stPktSend.cmd		= (UINT8)ENG_DCG3::en_req_write_obj;
	stPktSend.obj_index	= index;
	stPktSend.sub_id	= sub_id;
	stPktSend.sub_index	= sub_index;
	/* 메모리 맵에 등록 */
	memcpy(&m_pstPktSend[m_pstReqHead->send_cmd_cnt], &stPktSend, sizeof(STG_MPSD));

	return TRUE;
}

/*
 desc : Command E_G3_WriteObjectValueByIndex (Object 0x3801, PDO Reference Value Collective Object)
		PDOs (Process Data Objects) : PDO Reference Value Collective Object (address. 0x3801)
 parm : drv_head	- [in]  총 8개의 드라이브에 공통적으로 적용되는 Header 정보
		drv_data	- [in]  총 8개로 구성된 Drive Data (Control/Reserved/Parameter-1~3)
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::WriteObjectValueByIndex(LPG_RVCH drv_head, LPG_RVCD drv_data)
{
	BOOL bSucc			= FALSE;
	UINT32 u32Size		= 0;
	LPG_PQWD pstWrite	= NULL;
	PUINT8 pData	= NULL;	/* STG_RVCH + STG_RVCD (8 set) */

	/* 동기 진입 */
	if (m_syncPktSend.Enter())
	{
		/* 전체 송신 패킷 크기 구하기 */
		u32Size		= sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8);
		u32Size		+= sizeof(STG_RVCH) + sizeof(STG_RVCD) * MAX_MC2_DRIVE;

		m_pPktReqWrite[u32Size]	= 0x00;
		/* UDP 헤더 정보 복사 */
		memcpy(m_pPktReqWrite, m_pstReqHead, sizeof(STG_PUCH));
		/* 본문 및 데이터 연결 */
		pstWrite	= (LPG_PQWD)(m_pPktReqWrite + sizeof(STG_PUCH));
		pData		= m_pPktReqWrite + sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8);

		/* 기본 송신 명령 설정 */
		pstWrite->e_gen3_dnc_cmd= (UINT8)ENG_DCG3::en_req_write_obj;
		pstWrite->sub_id		= 0x00/* fixed. SubID of the SD2 device */;
		/* 나머지 패킷 설정 */
		pstWrite->serial_no		= m_u32SerialNo;
		pstWrite->index			= (UINT32)ENG_PSOI::en_ref_value_collective;
		pstWrite->sub_index		= 0x00000000;	/* Offset */
		pstWrite->data_len		= (UINT16)(sizeof(STG_RVCH) + sizeof(STG_RVCD) * MAX_MC2_DRIVE);
		/* 최대 8개의 Reference Drive Data 복사 */
		memcpy(pData, drv_head, sizeof(STG_RVCH));
		pData	+= (UINT32)sizeof(STG_RVCH);
		memcpy(pData, drv_data, sizeof(STG_RVCD) * MAX_MC2_DRIVE);

		/* 메모리 멥에 등록 */
		bSucc	= WriteObjectValueByIndex(0x00, 0x0000, pstWrite->index);
		if (bSucc)
		{
			/* MC2 패킷 송신 */
			bSucc	= AddPktData(m_pPktReqWrite, u32Size, 0x01);
		}

		/* 동기 해제 */
		m_syncPktSend.Leave();
	}

	return bSucc;
}

/*
 desc : 상대(Step) 위치로 이동 (Up, Down, Left, Bottom) 위치 값 반환
 parm : drv_id	- [in]  Motor Drive ID (SD2 Device ID) 즉, 0x00, 0x01, 0x04, 0x05
		direct	- [in]  이동 방향 (Up, Down, Left, Right)
		dist	- [in]  이동 거리 (단위: 0.1 um or 100 nm)
 retn : 절대 위치 값 반환 (단위: 0.1 um or 100 nm)
*/
INT32 CMC2CThread::GetDevStepMovePos(ENG_MMDI drv_id, ENG_MDMD direct, INT32 dist)
{
	INT32 i32MaxPos	= (INT32)ROUNDED(m_pstConfig->max_dist[(UINT8)drv_id] * 10000.0f, 0);
	INT32 i32MinPos = (INT32)ROUNDED(m_pstConfig->min_dist[(UINT8)drv_id] * 10000.0f, 0);
	INT32 i32ActPos	= m_pstShMemMC2->act_data[(UINT8)drv_id].real_position;
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15_LLS10 ||\
	DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15_LLS06)
	switch (direct)
	{
	/* 후진 or 좌측 이동 */
	case ENG_MDMD::en_move_down	:
	case ENG_MDMD::en_move_left	:	if (i32ActPos + dist > i32MaxPos)	i32ActPos	= i32MaxPos;
									else								i32ActPos	-= dist;		break;
	/* 전진 or 우측 이동 */
	case ENG_MDMD::en_move_up	:	if (i32ActPos - dist < 0)			i32ActPos	= i32MinPos;
									else								i32ActPos	-= dist;		break;
	case ENG_MDMD::en_move_right:	if (i32ActPos - dist < i32MinPos)	i32ActPos	= i32MinPos;
									else								i32ActPos	+= dist;		break;
	}
#else
	AfxMessageBox(L"It is not currently supported (CMC2CThread::GetDevStepMovePos Function)", MB_ICONSTOP|MB_TOPMOST);
#endif
	return i32ActPos;
}

/*
 desc : 특정(Step) 위치로 이동 (Up, Down, Left, Bottom) 위치 값 반환
 parm : drv_id	- [in]  Motor Drive ID (SD2 Device ID) 즉, 0x00, 0x01, 0x04, 0x05
		pos		- [in]  절대 위치 (단위: 0.1 um or 100 nm)
 retn : 절대 위치 값 반환 (단위: 0.1 um or 100 nm)
*/
INT32 CMC2CThread::GetDevAbsMovePos(ENG_MMDI drv_id, INT32 pos)
{
	INT32 i32MaxPos	= 0;
	INT32 i32ActPos	= pos;

	i32MaxPos	= (INT32)ROUNDED(m_pstConfig->max_dist[(UINT8)drv_id] * 10000.0f, 0);
#if 0
	if (drv_id == ENG_MMDI::en_align_cam1 || drv_id == ENG_MMDI::en_align_cam2)
	{
		i32ActPos	+= (INT32)ROUNDED(m_pstConfig->min_dist[(UINT8)drv_id] * 10000.0f, 0);
	}
#endif
	/* 최종 이동될 거리로 이동 */
	if (i32ActPos > i32MaxPos)	i32ActPos = i32MaxPos;

	return i32ActPos;
}

/*
 desc : 임의(Jog) 위치로 이동 (Up, Down, Left, Bottom) 위치 값 반환
 parm : drv_id	- [in]  Motor Drive ID (SD2 Device ID) 즉, 0x00, 0x01, 0x04, 0x05
		direct	- [in]  이동 방향 (Up, Down, Left, Right)
 retn : 절대 위치 값 반환 (단위: 0.1 um or 100 nm)
*/
INT32 CMC2CThread::GetDevJogMovePos(ENG_MMDI drv_id, ENG_MDMD direct)
{
	INT32 i32ActPos = 0;
	switch (direct)
	{
	/* 후진 or 좌측 이동 */
	case ENG_MDMD::en_move_down	:
	case ENG_MDMD::en_move_left	:	i32ActPos	= (INT32)ROUNDED(m_pstConfig->max_dist[(UINT8)drv_id] * 10000.0f, 0);		break;
	/* 전진 or 우측 이동 */
	case ENG_MDMD::en_move_up	:
	case ENG_MDMD::en_move_right:	i32ActPos	= (INT32)ROUNDED(m_pstConfig->min_dist[(UINT8)drv_id] * 10000.0f, 0);		break;
	}

	return i32ActPos;
}

/*
 desc : 스테이지 상대 이동 (Up, Down, Left, Bottom) - Reference Drive
 parm : method	- [in]  ENG_MMMM (Step or Jog)
		drv_id	- [in]  Motor Drive ID (SD2 Device ID) 즉, 0x00, 0x01, 0x04, 0x05
		direct	- [in]  이동 방향 (Up, Down, Left, Right)
		dist	- [in]  이동 거리 (단위: 0.1 um or 100 nm)
		velo	- [in]  이동 속도 (단위: 0.1 um or 100 nm / sec)
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::WriteDevMoveRef(ENG_MMMM method, ENG_MMDI drv_id, ENG_MDMD direct,
								  INT32 dist, INT32 velo)
{
	BOOL bSucc			= FALSE;
	INT32 i32ActPos		= 0;
	UINT32 u32Size		= sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8) + sizeof(STG_RVCD);
	LPG_PQWD pstWrite	= NULL;
	LPG_RVCD pstData	= NULL;

	/* 동기 진입 */
	if (m_syncPktSend.Enter())
	{
		m_pPktReqWrite[u32Size]	= 0x00;
		/* UDP 헤더 정보 복사 */
		memcpy(m_pPktReqWrite, m_pstReqHead, sizeof(STG_PUCH));
		/* 본문 및 데이터 연결 */
		pstWrite	= (LPG_PQWD)(m_pPktReqWrite + sizeof(STG_PUCH));
		pstData		= (LPG_RVCD)(m_pPktReqWrite + sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8));

		/* 기본 송신 명령 설정 */
		pstWrite->e_gen3_dnc_cmd= (UINT8)ENG_DCG3::en_req_write_obj;
		pstWrite->sub_id		= (UINT8)drv_id;
#if (ROTATED_CAMERA_IN_PHOTOHEAD)
		switch (direct)
		{
		case ENG_MDMD::en_move_up	:	direct	= ENG_MDMD::en_move_down;	break;
		case ENG_MDMD::en_move_down	:	direct	= ENG_MDMD::en_move_up;		break;
		}
#endif
		/* 특정 거리만큼 이동인지 Jog로 임의 위치까지 이동하는 것인지 */
		if (method == ENG_MMMM::en_move_step)
		{
			i32ActPos	= GetDevStepMovePos(drv_id, direct, dist);
		}
		else
		{
			i32ActPos	= GetDevJogMovePos(drv_id, direct);
		}
		/* 나머지 패킷 설정 */
		pstWrite->serial_no		= m_u32SerialNo;
		pstWrite->index			= (UINT32)ENG_PSOI::en_sdo_ref_drive;
		pstWrite->sub_index		= 0x00000000;		/* Offset */
		pstWrite->data_len		= (UINT16)sizeof(STG_RVCD);
		pstData->ctrl_cmd		= (UINT8)ENG_MCCN::en_go_position;
		pstData->ctrl_cmd_toggle= !m_pstShMemMC2->GetDriveRef((UINT8)drv_id)->ctrl_cmd_toggle;
		pstData->param_0		= 0;			/* ??? */
		pstData->param_1		= i32ActPos;	/* Target Position */
		pstData->param_2		= velo;			/* Moving Speed */
		pstData->param_3		= 0;

		/* 메모리 멥에 등록 */
		if (WriteObjectValueByIndex(UINT8(drv_id), 0x0000, pstWrite->index))
		{
			/* MC2 패킷 송신 */
			bSucc	= AddPktData(m_pPktReqWrite, u32Size, 0x01);
		}
		/* 동기 해제 */
		m_syncPktSend.Leave();
	}

	return bSucc;
}

/*
 desc : 스테이지 절대 이동 (Up, Down, Left, Bottom) - Reference Drive
 parm : drv_id	- [in]  Motor Drive ID (SD2 Device ID) 즉, 0x00, 0x01, 0x04, 0x05
		pos		- [in]  스테이지가 이동될 위치 (단위: 0.1 um or 100 nm)
		velo	- [in]  이동 속도 (단위: 0.1 um or 100 nm / sec)
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::WriteDevMoveAbs(ENG_MMDI drv_id, INT32 pos, UINT32 velo)
{
	BOOL bSucc			= FALSE;
	INT32 i32ActPos		= 0;
	UINT32 u32Size		= sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8) + sizeof(STG_RVCD);
	LPG_PQWD pstWrite	= NULL;
	LPG_RVCD pstData	= NULL;

	/* 동기 진입 */
	if (m_syncPktSend.Enter())
	{
		m_pPktReqWrite[u32Size]	= 0x00;
		/* UDP 헤더 정보 복사 */
		memcpy(m_pPktReqWrite, m_pstReqHead, sizeof(STG_PUCH));
		/* 본문 및 데이터 연결 */
		pstWrite	= (LPG_PQWD)(m_pPktReqWrite + sizeof(STG_PUCH));
		pstData		= (LPG_RVCD)(m_pPktReqWrite + sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8));

		/* 기본 송신 명령 설정 */
		pstWrite->e_gen3_dnc_cmd= (UINT8)ENG_DCG3::en_req_write_obj;
		pstWrite->sub_id		= (UINT8)drv_id;

		/* 특정 거리만큼 이동인지 Jog로 임의 위치까지 이동하는 것인지 */
		i32ActPos	= GetDevAbsMovePos(drv_id, pos);
		/* 나머지 패킷 설정 */
		pstWrite->serial_no		= m_u32SerialNo;
		pstWrite->index			= (UINT32)ENG_PSOI::en_sdo_ref_drive;
		pstWrite->sub_index		= 0x00000000;		/* Offset */
		pstWrite->data_len		= (UINT16)sizeof(STG_RVCD);
		pstData->ctrl_cmd		= (UINT8)ENG_MCCN::en_go_position;
		pstData->ctrl_cmd_toggle= !m_pstShMemMC2->GetDriveRef((UINT8)drv_id)->ctrl_cmd_toggle;
		pstData->param_0		= 0;			/* ??? */
		pstData->param_1		= i32ActPos;	/* Target Position */
		pstData->param_2		= velo;			/* Moving Speed */
		pstData->param_3		= 0;

		/* 메모리 멥에 등록 (Object 0x3801, PDO Reference Value Collective Object)*/
		if (WriteObjectValueByIndex(UINT8(drv_id), 0x0000, pstWrite->index))
		{
			/* MC2 패킷 송신 */
			bSucc	= AddPktData(m_pPktReqWrite, u32Size, 0x01);
		}
		/* 동기 해제 */
		m_syncPktSend.Leave();
	}

	return bSucc;
}

/*
 desc : 스테이지를 Vector 로 이동 - Reference Drive
 parm : m_drv_id	- [in]  Vector 이동 기준이되는 Master Drive ID 값
		s_drv_id	- [in]  Vector 이동 대상인 Slave Driver ID 값
		m_drv_pos	- [in]  스테이지가 최종 이동될 1 번 축의 위치 (단위: 0.1 um or 100 nm)
		s_drv_pos	- [in]  스테이지가 최종 이동될 2 번 축의 위치 (단위: 0.1 um or 100 nm)
		velo		- [in]  Vector 이동 속도 값 (단위: 0.1 um or 100 nm / sec)
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::WriteStageMoveVec(ENG_MMDI m_drv_id, ENG_MMDI s_drv_id,
									INT32 m_drv_pos, INT32 s_drv_pos, INT32 velo)
{
	BOOL bSucc			= FALSE;
	UINT32 u32Size		= sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8) + sizeof(STG_RVCD);
	LPG_PQWD pstWrite	= NULL;
	LPG_RVCD pstData	= NULL;

	/* 동기 진입 */
	if (m_syncPktSend.Enter())
	{
		m_pPktReqWrite[u32Size]	= 0x00;
		/* UDP 헤더 정보 복사 */
		memcpy(m_pPktReqWrite, m_pstReqHead, sizeof(STG_PUCH));
		/* 본문 및 데이터 연결 */
		pstWrite	= (LPG_PQWD)(m_pPktReqWrite + sizeof(STG_PUCH));
		pstData		= (LPG_RVCD)(m_pPktReqWrite + sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8));

		/* 기본 송신 명령 설정 */
		pstWrite->e_gen3_dnc_cmd= (UINT8)ENG_DCG3::en_req_write_obj;
		pstWrite->sub_id		= UINT8(m_drv_id);
		/* 나머지 패킷 설정 */
		pstWrite->serial_no		= m_u32SerialNo;
		pstWrite->index			= (UINT32)ENG_PSOI::en_sdo_ref_drive;
		pstWrite->sub_index		= 0x00000000;		/* Offset */
		pstWrite->data_len		= (UINT16)sizeof(STG_RVCD);
		pstData->ctrl_cmd		= (UINT8)ENG_MCCN::en_do_vector;

		/* Master Drive 축 기준으로 Vector 이동인 경우 */
		pstData->ctrl_cmd_toggle= !m_pstShMemMC2->GetDriveRef(UINT8(m_drv_id))->ctrl_cmd_toggle;
		pstData->param_0		= UINT8(s_drv_id);	/* Slave Drive */
		pstData->param_1		= m_drv_pos;		/* Master Target Position */
		pstData->param_2		= s_drv_pos;		/* Slave Target Position */
		pstData->param_3		= velo;

		/* 메모리 멥에 등록 (Object 0x3801, PDO Reference Value Collective Object) */
		bSucc	= WriteObjectValueByIndex(UINT8(m_drv_id), 0x0000, pstWrite->index);
		if (bSucc)
		{
			/* MC2 패킷 송신 */
			bSucc	= AddPktData(m_pPktReqWrite, u32Size, 0x01);
		}

		/* 동기 해제 */
		m_syncPktSend.Leave();
	}

	return bSucc;
}

/*
 desc : Device Control - Reference Drive
 parm : drv_id	- [in] Axis 정보 (0, 1, 4, 5)
		cmd		- [in]  ENG_MCCN
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::WriteDevCtrl(ENG_MMDI drv_id, ENG_MCCN cmd)
{
	BOOL bSucc	= TRUE;

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
	if (cmd == ENG_MCCN::en_do_homing)
	{
		if (ENG_MMST(m_pstShMemMC2->ref_head.mc_en_stop) == ENG_MMST::en_stop_yes)	bSucc = FALSE;
	}
	else
	{
		if (ENG_MMST(m_pstShMemMC2->ref_head.mc_en_stop) != ENG_MMST::en_stop_yes)	bSucc = FALSE;
	}
	if (!bSucc)	LOG_WARN(m_enSysID,
						 L"The command was canceled because of an MControl::EN_STOP condition");
#endif

	return bSucc && WriteSdoRefDrive(drv_id, cmd, 0, 0, 0, 0);
}

/*
 desc : 모터 속도 제어
 parm : drv_id	- [in]  Axis 정보 (0, 1, 4, 5)
		velo	- [in]  이동 속도 (단위: mm/sec)
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::WriteDevGoVelo(ENG_MMDI drv_id, UINT32 velo)
{
	return WriteSdoRefDrive(drv_id, ENG_MCCN::en_go_velocity, 0, INT32(velo), 0, 0);
}

/*
 desc : 모터 기본 속도 및 가속도 제어
 parm : drv_id	- [in]  Axis 정보 (0, 1, 4, 5)
		velo	- [in]  이동 속도 (단위: mm/sec)
		acce	- [in]  이동 가속도 (단위: mm^2)
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::WriteDevBaseVeloAcce(ENG_MMDI drv_id, UINT32 velo, UINT32 acce)
{
	if (!WriteSdoRefDrive(drv_id, ENG_MCCN::en_set_velocity, 0, INT32(velo), 0, 0))		return FALSE;
	if (!WriteSdoRefDrive(drv_id, ENG_MCCN::en_set_acceleration, 0, INT32(acce), 0, 0))	return FALSE;
	return TRUE;
}

/*
 desc : Luria Expose Mode로 설정
 parm : drv_id	- [in]  노광 방향의 축 Drive ID
		mode	- [in]  Area (Align) mode or Expose mode
						Area Mode		: 절대 스테이지가 움직이지 않음
						Expo Mode		: 현재 위치 값보다 Min 값이 작으면, 움직이지 않으며, 나머지는 Min 값으로 이동 함
		pixel	- [in]  Pixel Resolution (5.4 or 10.8 or 21.6 um) (unit: pm; picometer = 1/1000 nm)
						<LLS 10>
						Scroll mode 1	:  5.4310344828	um
						Scroll mode 2	: 10.8620689655	um
						Scroll mode 3	: 16.2931034483	um
						Scroll mode 4	: 21.724137931	um
						<LLS 25>
						Scroll mode 1	: 10.8620689655	um
						Scroll mode 2	: 21.724137931	um
						Scroll mode 3	: 32.5862068966	um
						Scroll mode 4	: 43.4482758621	um
		begin	- [in]  노광 시작 위치 (단위: 0.1 um or 100 nm)
		end		- [in]  노광 종료 위치 (단위: 0.1 um or 100 nm)
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::WriteDevLuriaMode(ENG_MMDI drv_id, ENG_MTAE mode,
									UINT64 pixel, INT32 begin, INT32 end)
{
	ENG_MCCN enCmd[2]	= { ENG_MCCN::en_set_trig_mode_v1, ENG_MCCN::en_set_trig_mode_v2 };
	/* 첫 번째 파라미터에 들어갈 값 선택 : 중요 */
	UINT8 u8Param0	= ENG_MTAE::en_expo == mode ? 0x00 : 0x02;
	if (ENG_MTAE::en_area == mode)
	{
		begin = 17;
		end = 0;
	}
	return WriteSdoRefDrive(drv_id, ENG_MCCN::en_set_trig_mode_v1,
							u8Param0, INT32(pixel), begin, end);
}

/*
 desc : Device Control - Reference Header <PDO>
 parm : drv_id	- [in]  Axis 정보 (0, 1, 4, 5)
		mode	- [in]  ENG_MMSM (0x00:Only Homing operation or 0x01: Other operation)
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::WritePdoRefHead(ENG_PSOI obj_index, UINT64 data)
{
	BOOL bSucc			= FALSE;
	UINT32 u32Size		= sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8) + sizeof(STG_RVCH);
	LPG_PQWD pstWrite	= NULL;
	LPG_RVCH pstHead	= NULL;

	/* 동기 진입 */
	if (m_syncPktSend.Enter())
	{
		/* 송신 패킷 메모리 할당 */
		m_pPktReqWrite[u32Size]	= 0x00;
		/* UDP 헤더 정보 복사 */
		memcpy(m_pPktReqWrite, m_pstReqHead, sizeof(STG_PUCH));
		/* 본문 및 데이터 연결 */
		pstWrite	= (LPG_PQWD)(m_pPktReqWrite + sizeof(STG_PUCH));
		pstHead		= (LPG_RVCH)(m_pPktReqWrite + sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8));

		/* 기본 송신 명령 설정 */
		pstWrite->e_gen3_dnc_cmd= (UINT8)ENG_DCG3::en_req_write_obj;
		pstWrite->sub_id		= 0x00/*UINT8(drv_id)*/;
		/* 나머지 패킷 설정 */
		pstWrite->serial_no		= m_u32SerialNo;
		pstWrite->index			= UINT32(obj_index)/*ENG_PSOI::en_pdo_ref_head_mcontrol*/;
		pstWrite->sub_index		= 0x00000000;	/* MC Control : Offset */
		pstWrite->data_len		= (UINT16)sizeof(STG_RVCH);
		/* Reference Header Part -> 먼저 기본 값 설정하고 */
		memcpy(pstHead, &m_pstShMemMC2->ref_head, sizeof(STG_RVCH));
		/* Object Type에 따라 값을 다르게 설정 해줌 */
		switch (obj_index)
		{
		case ENG_PSOI::en_pdo_ref_head_mcontrol		:
			memcpy(pstHead, &data, 1);				break;	/*pstHead->mc_en_stop	= UINT8(data);	break;*/
		case ENG_PSOI::en_pdo_ref_head_v_scale		:
			pstHead->v_scale		= UINT8(data);	break;;
		case ENG_PSOI::en_pdo_ref_head_d_outputs	:
			pstHead->digital_outputs= UINT64(data);	break;;
		}

		/* 메모리 멥에 등록 (Object 0x3801, PDO Reference Value Collective Object) */
		if (WriteObjectValueByIndex(pstWrite->sub_id, 0x0000, pstWrite->index))
		{
			/* MC2 패킷 송신 */
			bSucc	= AddPktData(m_pPktReqWrite, u32Size, 0x01);
		}
		/* 동기 해제 */
		m_syncPktSend.Leave();
	}

	return bSucc;
}

/*
 desc : Device Control - Reference Drive <SDO> (0x3808)
 parm : drv_id	- [in]  Axis 정보 (0, 1, 4, 5)
		cmd		- [in]  ENG_MCCN
		param0	- [in]  Parameter 0
		param1	- [in]  Parameter 1
		param2	- [in]  Parameter 2
		param3	- [in]  Parameter 3
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::WriteSdoRefDrive(ENG_MMDI drv_id, ENG_MCCN cmd,
								   UINT8 param0, INT32 param1, INT32 param2, INT32 param3)
{
	BOOL bSucc			= FALSE;
	UINT32 u32Size		= sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8) + sizeof(STG_RVCD);
	LPG_PQWD pstWrite	= NULL;
	LPG_RVCD pstData	= NULL;

	/* 동기 진입 */
	if (m_syncPktSend.Enter())
	{
		/* 송신 패킷 메모리 할당 */
		m_pPktReqWrite[u32Size]	= 0x00;
		/* UDP 헤더 정보 복사 */
		memcpy(m_pPktReqWrite, m_pstReqHead, sizeof(STG_PUCH));
		/* 본문 및 데이터 연결 */
		pstWrite	= (LPG_PQWD)(m_pPktReqWrite + sizeof(STG_PUCH));
		pstData		= (LPG_RVCD)(m_pPktReqWrite + sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8));

		/* 기본 송신 명령 설정 */
		pstWrite->e_gen3_dnc_cmd= (UINT8)ENG_DCG3::en_req_write_obj;
		pstWrite->sub_id		= UINT8(drv_id);
		/* 나머지 패킷 설정 */
		pstWrite->serial_no		= m_u32SerialNo;
		pstWrite->index			= (UINT32)ENG_PSOI::en_sdo_ref_drive;
		pstWrite->sub_index		= 0x00000000;	/* MC Control : Offset */
		pstWrite->data_len		= (UINT16)sizeof(STG_RVCD);
		/* Reference Data Part */
		pstData->ctrl_cmd		= (UINT8)cmd;
		pstData->ctrl_cmd_toggle= !m_pstShMemMC2->GetDriveRef((UINT8)drv_id)->ctrl_cmd_toggle;
		pstData->param_0		= param0;		/* Base Axis */
		pstData->param_1		= param1;		/* Target Position */
		pstData->param_2		= param2;		/* Target Position - Slave */
		pstData->param_3		= param3;

		/* 메모리 멥에 등록 (Object 0x3801, PDO Reference Value Collective Object) */
		if (WriteObjectValueByIndex(pstWrite->sub_id, 0x0000, pstWrite->index))
		{
			/* MC2 패킷 송신 */
			bSucc	= AddPktData(m_pPktReqWrite, u32Size, 0x01);
		}
		/* 동기 해제 */
		m_syncPktSend.Leave();
	}

	return bSucc;
}

/*
 desc : Luria가 현재 노광 (Printing) 중인지 여부에 따라,
		MC2에게 주기적으로 통신 데이터 요청 여부 설정
 parm : enable	- [in]  TRUE : 요청 진행, FALSE : 요청 중지 (아예 요청 중지가 아님)
 retn : None
*/
VOID CMC2CThread::SetSendPeriodPkt(BOOL enable)
{
	/* 동기 진입 */
	if (m_syncPeriodPkt.Enter())
	{
		/* 주기적으로 데이터 요청 여부 */
		m_bSendPeriodPkt	= enable;
		if (enable)	m_u64PeriodTime	= 250;	/* 좀더 빠르게 주기적으로 요청 */
		else		m_u64PeriodTime	= 500;	/* 좀더 느리게 주기적으로 요청 */

		/* 동기 해제 */
		m_syncPeriodPkt.Leave();
	}
}

/*
 desc : 연결 상태 정보 갱신
 parm : flag	- [in]  0x00: 연결 해제 상태, 0x01: 연결된 상태
 retn : None
*/
VOID CMC2CThread::UpdateLinkTime(UINT8 flag)
{
	BOOL bUpate	= FALSE;

	/* 현재 연결 해제 상태이고, 이전에 연결된 상태라면 */
	if (!flag && m_pstShMemMC2->link.is_connected)			bUpate	= TRUE;
	/* 현재 연결 상태이고, 이전에 해제된 상태라면 */
	else if (flag && !m_pstShMemMC2->link.is_connected)	bUpate	= TRUE;

	/* 이전 상태와 다른 경우라면, 정보 변경 */
	if (bUpate)
	{
		m_pstShMemMC2->link.is_connected	= flag;
		m_pstShMemMC2->link.link_time[0]	= m_stLinkTime.wYear;
		m_pstShMemMC2->link.link_time[1]	= m_stLinkTime.wMonth;
		m_pstShMemMC2->link.link_time[2]	= m_stLinkTime.wDay;
		m_pstShMemMC2->link.link_time[3]	= m_stLinkTime.wHour;
		m_pstShMemMC2->link.link_time[4]	= m_stLinkTime.wMinute;
		m_pstShMemMC2->link.link_time[5]	= m_stLinkTime.wSecond;
		m_pstShMemMC2->link.last_sock_error	= GetSockLastError();
	}
}

/*
 desc : Reference Data 전체 요청
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::ReadReqPktRefAll()
{
	/* Reference Data 전체 요청 패킷 구성 */
	ReadObjectValueByIndex(0, MAX_WANTED_READ_SIZE, 0, (UINT32)ENG_PSOI::en_ref_value_collective);
	/* 패킷 데이터 송신 */
	return AddPktData(m_pPktReadBuff, PKT_SIZE_READ_OBJECT, 0x01);
}

/*
 desc : Active Data 전체 요청
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::ReadReqPktActAll()
{
	/* Actvie Data 전체 요청 패킷 구성 */
	ReadObjectValueByIndex(0, MAX_WANTED_READ_SIZE, 0, (UINT32)ENG_PSOI::en_act_value_collective);
	/* 패킷 데이터 송신 */
	return AddPktData(m_pPktReadBuff, PKT_SIZE_READ_OBJECT, 0x01);
}
