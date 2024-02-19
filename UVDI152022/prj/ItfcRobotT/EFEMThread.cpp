
/*
 desc : TCP/IP Client Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "EFEMThread.h"

#include "SendQue.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

#define MAX_RECV_WAIT_TIME	3000

/*
 desc : 생성자
 parm : shmem	- [in]  EFEM 공유 메모리
		th_id	- [in]  thread id
		efem	- [in]  EFEM Server 통신 정보
 retn : None
*/
CEFEMThread::CEFEMThread(LPG_EDSM shmem, UINT16 th_id, LPG_CTCS efem)
	: CClientThread(ENG_EDIC::en_efem,
					th_id,
					efem->tcp_port,
					efem->source_ipv4,	/* CMPS Local (Client) IPv4 */
					efem->target_ipv4,	/* TES Remote (Server) IPv4 */
					efem->port_buff,
					efem->recv_buff,
					efem->ring_buff,
					efem->idle_time,
					efem->link_time,
					efem->sock_time)
{
	/* ------------------------------------------- */
	/* RunWork 함수 본문을 바로 수행 하도록 처리함 */
	/* ------------------------------------------- */
	m_u64SendTime		= 0;
	m_u64PeriodTime		= 0;
	m_u64StateTime		= GetTickCount64();
	// 초기 값은 수신 완료 상태로 설정
	m_bSendRecv			= FALSE;
	m_u8AliveCount		= 0x00;
	m_u64StateTick		= 1000;		/* 기본적으로 EFEM State 요청 주기는 1 초. 노광 중일 때는 0.1 초마다 요청 */
#if 0
	m_u8LastEfemState	= 0xff;	/* 반드시 최대 값으로 초기화 */
	m_u8LastWaferLoad	= 0xff;	/* 반드시 최대 값으로 초기화 */
#endif
	/* 현재 노광 모드 여부에 따라, EFEM Attach 처리 상태 변경 */
	m_bIsExposeMode		= FALSE;
	m_bSetFoupChange	= FALSE;

	/* 공유 메모리 연결 */
	m_pstShMemEFEM		= shmem;

	/* 수신 받은 데이터 임시 저장 */
	m_pPktRecvData	= new UINT8 [1024];
	memset(m_pPktRecvData, 0x00, 1024);

	/* ----------------------------------------------------------- */
	/* 사용자(운영자)의 명령을 임시로 저장할 송신 데이터 객체 생성 */
	/* ----------------------------------------------------------- */
	m_pSendQue		= new CSendQue(256);
	ASSERT(m_pSendQue);

	/* 임시 송신 버퍼 메모리 초기화 */
	memset(m_u8SendData, 0x00, 8);
}

/*
 desc : Destructor
 parm : None
*/
CEFEMThread::~CEFEMThread()
{
	/* Trigger Server와 연결 강제 해제 */
	CloseClient();
	/* 송신 큐 메모리 해제 */
	delete m_pSendQue;
	delete [] m_pPktRecvData;
}

/*
 desc : 스레드 실행시 주기적 호출됨
 parm : None
 retn : None
*/
VOID CEFEMThread::DoWork()
{
	/* 연결이 해제된 경우 처리*/
	if (!IsConnected())
	{
		/* 연결 해제 상태 갱신 */
		UpdateLinkTime(0x00);
		return;
	}

	/* 주기적으로 송신 Queue 감시 */
	if (!m_pSendQue->IsQueEmpty())	PopSendData();

	/* EFEM State가 특정 시간동안 갱신되지 않으면 강제로 연결 해제 */
#ifdef _DEBUG
	if (!m_pstShMemEFEM->IsUpdateState(30000))
#else
	if (!m_pstShMemEFEM->IsUpdateState(5000))
#endif
	{
		CloseClient();
		return;
	}
}

/*
 desc : 서버에 연결이된 경우
 parm : sd	- 소켓 기술자
 retn : TRUE or FALSE
*/
BOOL CEFEMThread::Connected()
{
	/* 송신 시간을 무조건 0 값으로 초기화 */
	m_u64SendTime	= 0;
	m_u64PeriodTime	= 0;
	/* 접속 완료되고 난 이후 초기 명령 송신 */
	if (!InitSendData())	return FALSE;
	/* State Time 갱신 */
	m_pstShMemEFEM->UpdateStateTime();
	/* 연결 완료 상태 갱신 */
	UpdateLinkTime(0x01);

	return TRUE;
}

/*
 desc : 서버로부터 연결 해제가 된 경우
 parm : None
 retn : None
*/
VOID CEFEMThread::CloseSocket(SOCKET sd, UINT32 time_out/* =0 */)
{

	// 상위 객체 호출
	CClientThread::CloseSocket(sd, time_out);
}

/*
 desc : EFEM에 접속되고 난 이후, 초기 명령 송신 (Send Queue 등록)
 parm : None
 retn : DI_TRUE or DI_FALSE
*/
BOOL CEFEMThread::InitSendData()
{
	BOOL bSucc	= TRUE;

	/* FOUP State 요청 */
	if (bSucc)	bSucc	= SendData(ENG_ECPC::en_req_foup_loaded);
	if (bSucc)	bSucc	= SendData(ENG_ECPC::en_req_efem_reset);

	return bSucc;
}

/*
 desc : Alive 응답 받은 정보 설정
 parm : None
 retn : None
*/
VOID CEFEMThread::SetAliveData()
{
	GetLocalTime(&m_tmAliveTime);
	m_u8AliveCount	= 0x00;
}

/*
 desc : 수신된 패킷 명령어에 따라 처리 진행
 parm : data	- [in]	수신된 데이터가 저장된 구조체 포인터
 retn : TRUE - 데이터 수신 성공, FALSE - 데이터 수신 실패
*/
BOOL CEFEMThread::PktAnalysis(LPG_PSRB data)
{
	/* Alive Check Event는 등록하지 않는다. 속도 저하 문제 때문에 */
	if (ENG_PCUC::en_comn_alive_check != ENG_PCUC(data->cmd))
	{
		/* 수신 처리를 위한 콜백 함수 호출 */
		SetRecvPacket(data);
	}

	/* Server로부터 이벤트가 들어와을 경우, Client가 다시 재요청하는 경우가 있음 */
	return ReqRefreshData(data->cmd, data->data);
}

/*
 desc : FOUP Lot List 요청 패킷 송신
 parm : None
 retn : TRUE or FALSE
*/
BOOL CEFEMThread::ReqFoupLotList()
{
	/* 노광 모드가 아니거나, FOUP Attach Event가 수신 안된 경우인지 확인 */
	if (!m_bIsExposeMode || !m_bSetFoupChange)	return TRUE;
	/* FOUP Attach Event가 수신되지 않았다고 설정 */
	m_bSetFoupChange	= FALSE;

	return SendData(ENG_ECPC::en_req_lot_list, m_u8SendData, 0x0001);
}

/*
 desc : Server의 특정 명령이 도착하면, Client는 자동으로 서버에게 특정 명령을 송신
 parm : cmd	- [in]  수신된 명령어
		data- [in]  수신된 데이터
 retn : TRUE or FALSE
*/
BOOL CEFEMThread::ReqRefreshData(UINT16 cmd, PUINT8 data)
{
	UINT8 u8Data[8]	= {NULL};
	UINT16 u16Size = 0x0000;
	ENG_ECPC enReqCmd= ENG_ECPC::en_req_none;

	switch (cmd)
	{
	/* FOUP Change 명령이 수신되면, EFEM에 남아 있는 Lot List 재요청 */
	case ENG_ECPC::en_res_foup_change	:
		if (!data[1])	return TRUE;		/* Detached 상태인 경우, 별도 요청이 없음 */
		/* 현재 노광 모드가 아닌 경우 */
		if (!m_bIsExposeMode)
		{
			enReqCmd	= ENG_ECPC::en_req_lot_list;	/* 송신할 데이터부가 없으므로, 명령만 송신 */
			u8Data[0]	= data[0];
			u16Size		= 1;
			/* FOUP Lot List 요청 */
			return SendData(enReqCmd, u8Data, u16Size);
		}
		/* 현재 노광 모드인 경우 */
		else
		{
			/* FOUP Attach Event가 수신되었다고 설정 */
			m_bSetFoupChange= TRUE;
			m_u8SendData[0]	= data[0];
		}
		break;
	}

	return TRUE;
}

/*
 desc : 주기적으로 EFEM 쪽에 State 정보 요청
 parm : None
 retn : TRUE or FALSE
*/
BOOL CEFEMThread::ReqEfemState()
{
	ENG_ECPC enReqCmd= ENG_ECPC::en_req_efem_state;
	BOOL bSucc		= TRUE;

	/* 이전에 요청한 시간과 현재 시간을 비교 후 충분한 간격을 두고 주기적으로 송신하는지 확인 */
	if (GetTickCount64() > (m_u64StateTime+m_u64StateTick))	/* send period : 1000 msec */
	{
		/* 송신한 시간 저장 */
		m_u64StateTime	= GetTickCount64();
		bSucc	= SendData(enReqCmd);
	}

	return bSucc;
}

/*
 desc : 노광 동작 여부 설정
 parm : enable	- [in]  TRUE - 노광 동작, FALSE - 노광 중지
 retn : None
*/
VOID CEFEMThread::SetExposeMode(BOOL enable)
{
#ifdef _DEBUG
	m_u64StateTick	= enable ? 500 : 1000;
#else
	m_u64StateTick	= enable ? 300 : 1000;
#endif

	/* 노광 동작 모드 설정 */
	m_bIsExposeMode	= enable;

	/* 무조건 노광 중지 모드인 경우 */
	if (!enable)
	{
		/* FOUP Change Event 초기화 */
		m_bSetFoupChange	= FALSE;
	}
}

/*
 desc : 데이터가 수신된 경우, 호출됨
 parm : None
 retn : TRUE - 성공적으로 분석 (Ring Buffer) 처리
		FALSE- 분석 처리하지 않음 (아직 완벽하지 않은 패킷임)
*/
BOOL CEFEMThread::RecvData()
{
#ifdef _DEBUG
	TCHAR tzMsg[64]	= {NULL};
#endif	// _DEBUG
	PUINT8 pPktRead/*, pPktData*/;
	UINT32 u32HeadTail	= sizeof(STG_UTPH)+sizeof(STG_UTPT);
	UINT64 u64SavedBytes;
	STG_UTPH stPktHead	= {NULL};
	STG_UTPT stPktTail	= {NULL};

	// 기본적으로 수신되었다고 설정
	m_enSocket	= ENG_TPCS::en_recved;

	do	{

		/* ------------------------------------------------------------------------------- */
		/* 현재 수신된 버퍼의 크기를 얻어온 후 패킷 헤더 크기(8 Bytes) 조차 받지 못한 경우 */
		/* 다음에 더 읽어롤 때까지 더 이상 작업을 수행하지 않도록 처리 한다                */
		/* ------------------------------------------------------------------------------- */
		u64SavedBytes	= m_pPktRingBuff->GetReadSize();
		if (u64SavedBytes < u32HeadTail)
		{
			m_enSocket	= ENG_TPCS::en_recving;
			return TRUE;
		}
		// 헤더 정보 복사
		m_pPktRingBuff->CopyBinary((UINT8*)&stPktHead, sizeof(STG_UTPH));
		stPktHead.SwapNetworkToHost();
		// 잘못된 패킷일 경우 에처 처리
		if (stPktHead.length < 1)
		{
#ifdef _DEBUG
			swprintf_s(tzMsg, 64, L"head:%02x / cmd:%02x / length:%d",
					  stPktHead.head, stPktHead.cmd, stPktHead.length);
			TRACE(L"%s\n", tzMsg);
#endif // _DEBUG
			return FALSE;
		}
		/* --------------------------------------------------------------------------------- */
		/* 현재 수신된 버퍼의 크기를 얻어온 후 패킷 본문 크기(Tail 포함) 조차 받지 못한 경우 */
		/* 다음에 더 읽어롤 때까지 더 이상 작업을 수행하지 않도록 처리 한다                  */
		/* --------------------------------------------------------------------------------- */
		if (u64SavedBytes < stPktHead.length)
		{
			m_enSocket	= ENG_TPCS::en_recving;
			return TRUE;
		}
		/* ----------------------------------------------------------------------- */
		/* 수신된 패킷 데이터 1개를 저장할 버퍼 할당                               */
		/* ----------------------------------------------------------------------- */
		m_pPktRecvData[stPktHead.length]	= 0x00;
		pPktRead	= m_pPktRecvData;

		/* ----------------------------------------------------------------------- */
		/* 버퍼에 하나 이상의 패킷이 저장되어 있다면 실제로 데이터를 읽어도록 한다 */
		/* ----------------------------------------------------------------------- */
		m_pPktRingBuff->ReadBinary(pPktRead, stPktHead.length);
		// 본문 이 시작되는 위치로 버퍼 포인트 이동
		pPktRead	+= sizeof(STG_UTPH);
		/* ----------------------------------------------------------------------- */
		/* 현재 수신된 패킷 데이터를 수신 큐 버퍼에 등록 작업을 수행하도록 처리 함 */
		/* ----------------------------------------------------------------------- */
		if (!m_pRecvQue->PushPktData(stPktHead.cmd, stPktHead.length-u32HeadTail, pPktRead))
		{
#ifdef _DEBUG
			TRACE(L"Recv Queue is Full\n");
#endif
		}
		/* ------------------------------------------------------------------------------- */
		/* 현재 패킷 하나를 읽고 난 이후, 버퍼에 남아 있는 데이터의 크기가 패킷을 구성하는 */
		/* 최소의 패킷 크기만큼 남아 있으면, 루프를 한번 더 수행하고, 최소 패킷 크기만큼   */
		/* 남아 있지 않으면 더 읽어오도록 해당 루프를 빠져나가도록 처리 한다               */
		/* ------------------------------------------------------------------------------- */
	}	while (!m_pRecvQue->IsQueFull() && (u64SavedBytes - stPktHead.length) >= u32HeadTail);

	/* !!! Important !!! - 수신되면, 송신시간 무조건 초기화 (0 값) */
	m_u64SendTime	= 0;

	return TRUE;
}

/*
 desc : 송신해도 되는지 확인 (이전에 송신에 대한 응답이 도착 했는지 확인)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CEFEMThread::IsSendData()
{
	/* 송신 후 특정 시간 동안 응답이 없는지 확인 */
	return (GetTickCount64() - m_u64SendTime) > MAX_RECV_WAIT_TIME ? TRUE : FALSE;
}

/*
 desc : 송신할 데이터가 있으면, Queue로부터 데이터 하나 가져온 후 송신 작업 수행
 parm : None
 retn : TRUE or FALSE (전송할 패킷 정보가 없는 경우)
*/
BOOL CEFEMThread::PopSendData()
{
	BOOL bSucc		= FALSE;
	LPG_PESR pstQue	= NULL;
	STG_UTPH stHead	= {NULL};
	STG_PSRB stData	= {NULL};

	/* 송신에 대한 응답이 없는 경우, 더 이상 송신 중지 처리 */
	if (!IsSendData())	return FALSE;

	/* 이전에 송신한 시간과 비교하여, 너무 짧은 간격으로 송신하면 안되므로 ... */
#ifdef _DEBUG
	if ((GetTickCount64() - m_u64PeriodTime) < 500)	return FALSE;
#else
	if ((GetTickCount64() - m_u64PeriodTime) < 300)	return FALSE;
#endif
	m_u64PeriodTime	= GetTickCount64();

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		/* ---------------------------------- */
		/* 큐에 전송 데이터가 존재하는지 확인 */
		/* ---------------------------------- */
		pstQue	= m_pSendQue->PopData();
		if (pstQue)
		{
			// 현재 송신한 시점의 시간 저장
			m_u64SendTime	= GetTickCount64();

			/* 메시지 헤드 추출 */
			memcpy(&stHead, pstQue->data, sizeof(STG_UTPH));
			stHead.SwapNetworkToHost();
			stData.cmd		= stHead.cmd;
			stData.len		= stHead.length - sizeof(STG_UTPH) - sizeof(STG_UTPT);
			stData.data		= pstQue->data + sizeof(STG_UTPH);

			/* 데이터 전송 */
			bSucc = ReSended(pstQue->data, UINT32(pstQue->size), UINT32(3), UINT32(30));
			/* 만약 데이터 송신이 실패 했다면 */
			if (!bSucc)
			{
				TCHAR tzMesg[LOG_MESG_SIZE]	= {NULL};
				swprintf_s(tzMesg, LOG_MESG_SIZE,
						   L"Failed to send the packet data [EFEM] [err_cd=%d]", m_i32SockError);
				SetErrorMesg(ENG_LNWE::en_error, tzMesg);
			}
		}
		/* 동기 해제 */
		m_syncSend.Leave();
	}

	/* 송신할 패킷 정보가 있는지 유무(TRUE-송신된 패킷이 있다? FALSE-송신된 패킷이 없다) */
	return bSucc;
}

/*
 desc : 데이터 송신
 parm : cmd		- [in]  명령어
		data	- [in]  송신 데이터가 저장된 버퍼 포인터
 retn : TRUE or FALSE
*/
BOOL CEFEMThread::SendData(ENG_ECPC cmd, PUINT8 data, UINT16 size)
{
	BOOL bSucc	= FALSE;

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		bSucc = m_pSendQue->PushData(UINT16(cmd), data, size);
		/* 동기 해제 */
		m_syncSend.Leave();
	}

	return bSucc;
}

/*
 desc : 콜백 함수 - 통신 데이터 (송신에 대한 응답) 수신 처리
 parm : event	- [in]  이벤트 정보가 저장된 구조체
		data	- [in]  수신된 패킷이 저장된 버퍼 포인터
 retn : TRUE (Update) or FALSE
*/
VOID CEFEMThread::SetRecvPacket(LPG_PSRB data)
{
	/* 수신된 데이터가 NULL인지 (그럴리 없겠지만) */
	if (!data)	return;

	switch (data->cmd)
	{
	case ENG_ECPC::en_res_efem_state		:	m_pstShMemEFEM->SetEFEMState(data->data);			break;
	case ENG_ECPC::en_res_efem_error		:	m_pstShMemEFEM->SetEFEMError(data->data);
										/* !!! 에러가 없는 상태에서 처음 에러가 발생된 에러 코드 저장 !!! */
										if (0x0000 == m_pstShMemEFEM->link.u16_error_code)
											m_pstShMemEFEM->link.u16_error_code	= m_pstShMemEFEM->error_last;
										/* 에러 코드에 대해 로그로 저장 */
										SaveErrorLogs(m_pstShMemEFEM->error_last);			break;
	case ENG_ECPC::en_res_efem_reset		:	m_pstShMemEFEM->SetEFEMReset();						break;
	case ENG_ECPC::en_res_efem_event		:	m_pstShMemEFEM->SetEFEMEvent(data->data[0]);		break;
	case ENG_ECPC::en_res_elot_moving	:	m_pstShMemEFEM->SetLotErrMoving(data->data);		break;
	case ENG_ECPC::en_res_foup_change	:	m_pstShMemEFEM->SetFoupChanged(data->data);			break;
	case ENG_ECPC::en_res_foup_loaded	:	m_pstShMemEFEM->SetFoupLoaded(data->data);			break;
	case ENG_ECPC::en_res_lot_list		:	m_pstShMemEFEM->SetLotList(data->data);				break;

	case ENG_ECPC::en_res_stop			:	break;
	default							:	break;
	}

	/* 가장 최근의 상태 정보와 현재 상태 정보가 다른 경우만 저장 */
#if 0
	if (ENG_ECPC::en_res_efem_state == data->cmd)
	{
		if (m_u8LastEfemState == data->data[0] &&	/* EFEM의 상태 정보 비교*/
			m_u8LastWaferLoad == data->data[1])		/* Wafer 놓여진 위치 비교 */
		{
			return;
		}
		/* 기존 값과 다르면 저장 */
		m_u8LastEfemState	= data->data[0];
		m_u8LastWaferLoad	= data->data[1];
	}
#endif
}

/*
 desc : 에러 코드에 대한 상세 내용을 로그에 저장
 parm : code	- [in]  에러 코드 값
 retn : None
*/
VOID CEFEMThread::SaveErrorLogs(UINT16 code)
{
	TCHAR tzMesg[LOG_MESG_SIZE]	= {NULL}, tzLogs[64] = {NULL};

	switch (code)
	{
	/* Robot Error (01xx) */
	case 0x0101	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_RBT payload mismatched");						break;
	case 0x0102	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_RBT before get action error");				break;
	case 0x0103	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_RBT after get action error");					break;
	case 0x0104	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_RBT before put action error");				break;
	case 0x0105	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_RBT after put action error");					break;

	case 0x0107	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_RBT protective stop");						break;
	case 0x0108	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_RBT robot Emergency stop");					break;
	case 0x0109	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_RBT robot violation or fault");				break;

	/* Aligner (Vision + Motor) (02xx) */
	case 0x0201	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG Fail homing");							break;
	case 0x0202	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG Cylinder error");							break;

	case 0x0211	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG cannot init camera API");					break;
	case 0x0212	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG cannot get camera API");					break;
	case 0x0213	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG cannot find camera");						break;
	case 0x0214	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG cannot connect camera");					break;
	case 0x0215	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG cannot trigger camera");					break;
	case 0x0216	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG cannot get image from camera");			break;
	case 0x0217	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG cannot process image");					break;

	case 0x0221	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG Motor drive OverCurrent");				break;
	case 0x0222	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG Motor drive OverSpeed");					break;
	case 0x0223	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG Motor drive StepOut");					break;
	case 0x0224	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG Motor drive OverLoad");					break;
	case 0x0225	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG Motor drive OverTemperature");			break;
	case 0x0226	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG Motor drive BackEMF");					break;
	case 0x0227	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG Motor drive MotorConnect");				break;
	case 0x0228	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG Motor drive EncoderConnect");				break;
	case 0x0229	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG Motor drive MotorPower");					break;
	case 0x022a	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG Motor drive InPosition");					break;
	case 0x022b	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG Motor drive SystemHalt");					break;
	case 0x022c	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG Motor drive ROMdevice");					break;
	case 0x022d	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG Motor drive OverInputVoltage");			break;
	case 0x022e	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG Motor drive PositionOverflow");			break;

	case 0x0236	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Not Ready State");								break;
	case 0x0237	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Busy State");										break;
	case 0x0238	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ZR Not Done");									break;

	case 0x0241	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Not Exist Wafer");								break;
	case 0x0242	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Invalid Wafer Position");							break;
	case 0x0243	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Fail Move to Start Position");					break;
	case 0x0244	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Too Many bad visionr Data");						break;
	case 0x0245	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Fail Find Wafer Notch");							break;
	case 0x0246	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"RESET Failed");									break;
	case 0x0247	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Servo ON Failed");								break;
	case 0x0248	:			wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Servo ON Failed");								break;

	/* LPM01 (Left / Right Opener) (03xx) */
	case 0x0310	: case 0x0410	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Clamp time over");							break;
	case 0x0311	: case 0x0411	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Unclamp time over");						break;
	case 0x0312	: case 0x0412	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Dock time over");							break;
	case 0x0313	: case 0x0413	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Undock time over");							break;
	case 0x0314	: case 0x0414	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Latch time over");							break;
	case 0x0315	: case 0x0415	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Unlatch time over");						break;
	case 0x0316	: case 0x0416	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Suction time over");						break;
	case 0x0317	: case 0x0417	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Release suction time over");				break;
	case 0x0318	: case 0x0418	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Open door time over");						break;
	case 0x0319	: case 0x0419	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Close door time over");						break;
	case 0x031A	: case 0x041A	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Mapping forward time over");				break;
	case 0x031B	: case 0x041B	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Mapping retract time over");				break;
	case 0x031F	: case 0x041F	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Communication error");						break;
	case 0x0320	: case 0x0420	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Homing time over");							break;
	case 0x0321	: case 0x0421	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Loading time over");						break;
	case 0x0322	: case 0x0422	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Unloading time over");						break;
	case 0x0323	: case 0x0423	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Inposition time over");						break;
	case 0x0328	: case 0x0428	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Door opening position moving time over");	break;
	case 0x0329	: case 0x0429	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Mapping starting positin moving time over");break;
	case 0x032A	: case 0x042A	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Mapping ending positin moving time over");	break;
	case 0x032B	: case 0x042B	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Load position moving time over");			break;
	case 0x0340	: case 0x0440	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Mappding data error");						break;
	case 0x0341	: case 0x0441	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Mode change error");						break;
	case 0x0370	: case 0x0470	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Clamp sensor error");						break;
	case 0x0371	: case 0x0471	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Dock sensor error");						break;
	case 0x0372	: case 0x0472	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Latch sensor error");						break;
	case 0x0373	: case 0x0473	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Door sensor error");						break;
	case 0x0374	: case 0x0474	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Mapping sensor error");						break;
	case 0x0377	: case 0x0477	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Elevator axis sensor error");				break;

	case 0x0390	: case 0x0490	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM Host available not input");			break;
	case 0x0391	: case 0x0491	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM No FOUP mounting");					break;
	case 0x0392	: case 0x0492	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM Not home position");				break;
	case 0x0393	: case 0x0493	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM Loading not completed");			break;
	case 0x0394	: case 0x0494	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM Clamping not completed");			break;
	case 0x0395	: case 0x0495	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM Docking not completed");			break;
	case 0x0396	: case 0x0496	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM Door vacuum not completed");		break;
	case 0x0397	: case 0x0497	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM Unlaching not completed");			break;
	case 0x0398	: case 0x0498	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM Door opening not complete");		break;
	case 0x0399	: case 0x0499	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM Mapping not started");				break;
	case 0x039A	: case 0x049A	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM Mapping not completed");			break;
	case 0x039C	: case 0x049C	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM Z axis is not door position");		break;
	case 0x039D	: case 0x049D	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM Not in mapping position");			break;
	case 0x039E	: case 0x049E	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM Undocking not completed");			break;
        
	case 0x03A0	: case 0x04A0	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Wafer drop");								break;
	case 0x03A1	: case 0x04A1	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM Wafer protrusion");					break;
	case 0x03A2	: case 0x04A2	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM FOUP mount error (mount sensor)");	break;
	case 0x03A3	: case 0x04A3	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM FOUP mount error (load sensor)");	break;
	case 0x03A5	: case 0x04A5	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM Air pressure drop");				break;
	case 0x03B0	: case 0x04B0	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM Host error");						break;
	case 0x03C0	: case 0x04C0	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM Parameter error");					break;
	case 0x03E0	: case 0x04E0	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM FAN stop alarm");					break;
	case 0x03E3	: case 0x04E3	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM Power supply voltage drop");		break;
	case 0x03FE	: case 0x04FE	: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM Dock hand pinch error");			break;

	case 0x0601					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_SYS SRU1 down");						break;
	case 0x0602					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_SYS SRU2 down");						break;
	case 0x0603					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_SYS MC down");							break;
	case 0x0604					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_SYS PM state");							break;
	case 0x0605					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_SYS EMS pressed");						break;
	case 0x0606					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_SYS Air Down");							break;
	case 0x0607					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_SYS Door LEFT opened");					break;
	case 0x0608					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_SYS Door RIGHT opened");				break;
	case 0x0609					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_SYS ION not run");						break;
	case 0x060A					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_SYS ION alarm1");						break;
	case 0x060B					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_SYS ION alarm2");						break;
	case 0x060C					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_SYS ION alarm3");						break;
	case 0x060D					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_SYS Not Ready");						break;
	case 0x0611					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_RBT operation timeout");				break;
	case 0x0612					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_RBT is not HOME");						break;
	case 0x0613					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_RBT wafer on ALG");						break;
	case 0x0614					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_RBT have no wafer");					break;
	case 0x0621					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG operation timeout");				break;
	case 0x0622					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG have wafer");						break;
	case 0x0623					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG no wafer on ALG");					break;
	case 0x0624					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_ALG cylinder failed");					break;
	case 0x0631					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_BUFFER wafer on BUFFER");				break;
	case 0x0632					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_BUFFER no wafer on BUFFER");			break;
	case 0x0641					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM1 operation timeout");				break;
	case 0x0642					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM1 have no FOUP");					break;
	case 0x0643					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM1 no good wafer in target");			break;
	case 0x0644					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM1 have wafer in target");			break;
	case 0x0645					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM1 is closed");						break;
	case 0x0651					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM2 operation timeout");				break;
	case 0x0652					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM2 have no FOUP");					break;
	case 0x0653					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM2 no good wafer in target");			break;
	case 0x0654					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM2 have wafer in target");			break;
	case 0x0655					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_LPM2 is closed");						break;
	case 0x0661					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_DI gate not opened");					break;
	case 0x0662					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_DI cover opened");						break;
	case 0x0663					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_DI EMS A pressed");						break;
	case 0x0664					: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"ERR_DI EMS B preseed");						break;
	default						: wcscpy_s(tzMesg, LOG_MESG_SIZE, L"Unknown Error Code");						break;
	}

	/* 에러 코드 포함 */
	swprintf_s(tzLogs, 64, L"%s [err_code =0x%04x]", tzMesg, code);
	uvLogs_SaveLogs(ENG_EDIC::en_efem, ENG_LNWE::en_error, tzLogs, WFILE, WLINE);
}

/*
 desc : 연결 상태 정보 갱신
 parm : flag	- [in]  0x00: 연결 해제 상태, 0x01: 연결된 상태
 retn : None
*/
VOID CEFEMThread::UpdateLinkTime(UINT8 flag)
{
	BOOL bUpate	= FALSE;

	/* 현재 연결 해제 상태이고, 이전에 연결된 상태라면 */
	if (!flag && m_pstShMemEFEM->link.is_connected)			bUpate	= TRUE;
	/* 현재 연결 상태이고, 이전에 해제된 상태라면 */
	else if (flag && !m_pstShMemEFEM->link.is_connected)	bUpate	= TRUE;

	/* 이전 상태와 다른 경우라면, 정보 변경 */
	if (bUpate)
	{
		m_pstShMemEFEM->link.is_connected	= flag;
		m_pstShMemEFEM->link.link_time[0]	= m_stLinkTime.wYear;
		m_pstShMemEFEM->link.link_time[1]	= m_stLinkTime.wMonth;
		m_pstShMemEFEM->link.link_time[2]	= m_stLinkTime.wDay;
		m_pstShMemEFEM->link.link_time[3]	= m_stLinkTime.wHour;
		m_pstShMemEFEM->link.link_time[4]	= m_stLinkTime.wMinute;
		m_pstShMemEFEM->link.link_time[5]	= m_stLinkTime.wSecond;
		m_pstShMemEFEM->link.last_sock_error= GetSockLastError();
	}
}