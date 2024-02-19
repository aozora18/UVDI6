#include "pch.h"
#include "MainApp.h"
#include "BSAThread.h"
#include "SendQue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

#define MAX_RECV_WAIT_TIME	500

/*
 desc : 생성자
 parm : vision	- [in]  Vision Server 통신 정보
		shmem	- [in]  Luria Shared Memory
 retn : None
*/
CBSAThread::CBSAThread(LPG_CTCS vision, LPG_RBVC shmem)
	: CClientThread(ENG_EDIC::en_bsa, 0x0000, vision->tcp_port,
					vision->source_ipv4,	/* CMPS Local (Client) IPv4 */
					vision->target_ipv4,	/* PLS Remote (Server) IPv4 */
					vision->port_buff, vision->recv_buff, vision->ring_buff,
					vision->idle_time, vision->link_time, vision->sock_time)
{
	m_bInitUpdate	= FALSE;
	m_u64ReqTime	= GetTickCount64();
	m_u64RecvTime	= GetTickCount64();
	m_u64SendTime	= GetTickCount64();
	m_u64PeriodTime	= 0;
	m_bNotiEvent	= FALSE;
	/* 공유 메모리 연결 */
	m_pstShMemVisi	= shmem;

	/* 수신 받은 데이터 임시 저장 */
	m_pPktRecvData = (PUINT8)::Alloc(1024);
	memset(m_pPktRecvData, 0x00, 1024);
	/* 패킷 송신에 대한 수신 여부 설정 플래그 */
	m_pPktRecvFlag	= (PUINT8)::Alloc(32);
	memset(m_pPktRecvFlag, 0x00, 32);
	/* Event Message 버퍼 할당 */
	m_pEventMsg	= (PTCHAR)::Alloc(sizeof(TCHAR) * 1024+1);
	wmemset(m_pEventMsg, 0x00, 1024+1);

	/* 사용자(운영자)의 명령을 임시로 저장할 송신 데이터 객체 생성 */
	m_pSendQue = new CSendQue(256);
	ASSERT(m_pSendQue);
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CBSAThread::~CBSAThread()
{
	/* Trigger Server와 연결 강제 해제 */
	CloseClient();
	/* Event Message 버퍼 해제 */
	::Free(m_pEventMsg);
	/* 패킷 송신에 대한 수신 플래그 해제 */
	::Free(m_pPktRecvFlag);
	/* 송신 큐 메모리 해제 */
	delete m_pSendQue;
	::Free(m_pPktRecvData);
}

/*
 desc : 스레드 실행시 주기적 호출됨
 parm : None
 retn : None
*/
VOID CBSAThread::DoWork()
{
	/* 주기적으로 송신 Queue 감시 */
	if (!m_pSendQue->IsQueEmpty())	PopSendData();

	/* Vision State가 특정 시간동안 갱신되지 않으면 강제로 연결 해제 */
#ifdef _DEBUG
	if(!IsUpdateStateTime(10000))
#else
	if (!IsUpdateStateTime(5000))
#endif
	{
		/* 가장 최근 수신된 작업 시간 저장 */
		m_u64RecvTime = GetTickCount64();
		/* 강제로 연결 해제 수행 */
		CloseClient();

		LOG_WARN(ENG_EDIC::en_bsa, L"5 sec no response close socket");
		return;
	}

	/* 수신된 데이터가 존재하는지 주기적으로 확인 */
	LPG_PSRB pstPktData	= m_pRecvQue->PopPktData();
	if (pstPktData)	PktAnalysis(pstPktData);
}

/*
 desc : 서버에 연결이된 경우
 parm : sd	- 소켓 기술자
 retn : TRUE or FALSE
*/
BOOL CBSAThread::Connected()
{
	/* 연결되었을 때, 각 시간 모두 저장 */
	m_u64SendTime	= GetTickCount64();
	m_u64RecvTime	= GetTickCount64();
	m_u64ReqTime	= GetTickCount64();

	/* State Time 갱신 */
	SetUpdateStateTime();

	return TRUE;
}

/*
 desc : 서버로부터 연결 해제가 된 경우
 parm : None
 retn : Nonek
*/
VOID CBSAThread::CloseSocket(SOCKET sd, UINT32 time_out/* =0 */)
{
	// 상위 객체 호출
	CClientThread::CloseSocket(sd, time_out);
}

/*
 desc : 수신된 패킷 명령어에 따라 처리 진행
 parm : data	- [in]	수신된 데이터가 저장된 구조체 포인터
 retn : TRUE - 데이터 수신 성공, FALSE - 데이터 수신 실패
*/
BOOL CBSAThread::PktAnalysis(LPG_PSRB data)
{
	/* Alive Check Event는 등록하지 않는다. 속도 저하 문제 때문에 */
	if ((UINT16)ENG_PCUC::en_comn_alive_check != data->cmd)
	{
		/* 수신 처리를 위한 콜백 함수 호출 */
		SetSockData(0x02, data);
	}

	return TRUE;
}

/*
 desc : 데이터가 수신된 경우, 호출됨
 parm : None
 retn : TRUE - 성공적으로 분석 (Ring Buffer) 처리
		FALSE- 분석 처리하지 않음 (아직 완벽하지 않은 패킷임)
*/
BOOL CBSAThread::RecvData()
{
	TCHAR tzMesg[64]	= { NULL };
	PUINT8 pPktRead/*, pPktData*/;
	UINT32 u32HeadTail	= sizeof(STG_UTPH) + sizeof(STG_UTPT);
	UINT64 u64SavedBytes;
	STG_UTPH stPktHead	= { NULL };
	STG_UTPT stPktTail	= { NULL };

	// 기본적으로 수신되었다고 설정
	m_enSocket = ENG_TPCS::en_recved;
	do {
		/* ------------------------------------------------------------------------------- */
		/* 현재 수신된 버퍼의 크기를 얻어온 후 패킷 헤더 크기(8 Bytes) 조차 받지 못한 경우 */
		/* 다음에 더 읽어롤 때까지 더 이상 작업을 수행하지 않도록 처리 한다                */
		/* ------------------------------------------------------------------------------- */
		u64SavedBytes = m_pPktRingBuff->GetReadSize();
		if (u64SavedBytes < u32HeadTail)
		{
			m_enSocket = ENG_TPCS::en_recving;
			return TRUE;
		}
		// 헤더 정보 복사
		m_pPktRingBuff->CopyBinary((UINT8*)&stPktHead, sizeof(STG_UTPH));
		stPktHead.SwapNetworkToHost();
		// 잘못된 패킷일 경우 에처 처리
		if (stPktHead.length < 1)
		{
			swprintf_s(tzMesg, 64, L"head:%02x / cmd:%02x / length:%d",
					   stPktHead.head, stPktHead.cmd, stPktHead.length);
			LOG_ERROR(ENG_EDIC::en_bsa, tzMesg);
			return FALSE;
		}
		/* --------------------------------------------------------------------------------- */
		/* 현재 수신된 버퍼의 크기를 얻어온 후 패킷 본문 크기(Tail 포함) 조차 받지 못한 경우 */
		/* 다음에 더 읽어롤 때까지 더 이상 작업을 수행하지 않도록 처리 한다                  */
		/* --------------------------------------------------------------------------------- */
		if (u64SavedBytes < stPktHead.length)
		{
			m_enSocket = ENG_TPCS::en_recving;
			return TRUE;
		}
		//////////////////////////////////////////////////////////////////////////
		/* ----------------------------------------------------------------------- */
		/* 수신된 패킷 데이터 1개를 저장할 버퍼 할당                               */
		/* ----------------------------------------------------------------------- */
		m_pPktRecvData[stPktHead.length] = 0x00;
		pPktRead = m_pPktRecvData;

		/* ----------------------------------------------------------------------- */
		/* 버퍼에 하나 이상의 패킷이 저장되어 있다면 실제로 데이터를 읽어도록 한다 */
		/* ----------------------------------------------------------------------- */
		m_pPktRingBuff->ReadBinary(pPktRead, stPktHead.length);
		// 본문 이 시작되는 위치로 버퍼 포인트 이동
		pPktRead += sizeof(STG_UTPH);
		/* ----------------------------------------------------------------------- */
		/* 현재 수신된 패킷 데이터를 수신 큐 버퍼에 등록 작업을 수행하도록 처리 함 */
		/* ----------------------------------------------------------------------- */
		if (!m_pRecvQue->PushPktData(stPktHead.cmd, stPktHead.length - u32HeadTail, pPktRead))
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
	} while (!m_pRecvQue->IsQueFull() && (u64SavedBytes - stPktHead.length) >= u32HeadTail);

	/* !!! Important !!! - 수신되면, 송신시간 무조건 초기화 (0 값) */
	m_u64SendTime = 0;

	return TRUE;
}

/*
 desc : 송신해도 되는지 확인 (이전에 송신에 대한 응답이 도착 했는지 확인)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CBSAThread::IsSendData()
{
	/* 송신 후 특정 시간 동안 응답이 없는지 확인 */
	return (GetTickCount64() - m_u64SendTime) > MAX_RECV_WAIT_TIME ? TRUE : FALSE;
}

/*
 desc : 현재 송신될 패킷이 존재하는지 여부 확인
 parm : None
 retn : FALSE - POP 할 데이터가 있다. TRUE - POP 할 데이터가 없다
*/
BOOL CBSAThread::IsPopEmptyData()
{
	// Queue가 비어 있다면 TRUE이므로, POP 할 수 없다.
	return m_pSendQue->IsQueEmpty();
}

/*
 desc : 송신할 데이터가 있으면, Queue로부터 데이터 하나 가져온 후 송신 작업 수행
 parm : None
 retn : TRUE or FALSE (전송할 패킷 정보가 없는 경우)
*/
BOOL CBSAThread::PopSendData()
{
	BOOL bSucc = FALSE;
	LPG_PBSR pstQue = NULL;
	STG_UTPH stHead = { NULL };
	STG_PSRB stData = { NULL };

	/* 송신에 대한 응답이 없는 경우, 더 이상 송신 중지 처리 */
	if (!IsSendData())
	{
		LOG_ERROR(ENG_EDIC::en_bsa, L"There is no response to a previously sent command");
		return FALSE;
	}

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		/* ---------------------------------- */
		/* 큐에 전송 데이터가 존재하는지 확인 */
		/* ---------------------------------- */
		pstQue = m_pSendQue->PopData();
		if (pstQue)
		{
			// 현재 송신한 시점의 시간 저장
			m_u64SendTime = GetTickCount64();

			/* 메시지 헤드 추출 */
			memcpy(&stHead, pstQue->data, sizeof(STG_UTPH));
			stHead.SwapNetworkToHost();
			stData.cmd = stHead.cmd;
			stData.len = stHead.length - sizeof(STG_UTPH) - sizeof(STG_UTPT);
			stData.data = pstQue->data + sizeof(STG_UTPH);

			/* 송신 메시지 알림 */
			SetSockData(0x01, &stData);

			/* 데이터 전송 */
			bSucc = ReSended(pstQue->data, UINT32(pstQue->size), UINT32(3), UINT32(30));
			/* 만약 데이터 송신이 실패 했다면 */
			if (!bSucc)
			{
				/* 송신 FAIL 메시지 알림 */
				SetSockData(0xf1, &stData);
			}
		}
		/* 동기 해제 */
		m_syncSend.Leave();
	}

	/* 송신할 패킷 정보가 있는지 유무(TRUE-송신된 패킷이 있다? FALSE-송신된 패킷이 없다) */
	return bSucc;
}

//////////////////////////////////////////////////////////////////////////
/*
 desc : 데이터 송신
 parm : cmd		- [in]  명령어
		data	- [in]  송신 데이터가 저장된 버퍼 포인터
 retn : TRUE or FALSE
*/
BOOL CBSAThread::SendData(ENG_VCPC cmd, PUINT8 data, UINT16 size)
{
	BOOL bSucc = FALSE;

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
 desc : Vision Centering Command Data Send
 parm :	cmd		- [in] command
		cam_no	- [in] 카메라 설정 (0x01: bsa , 0x02: up)
		unit	- [in] 측정 위치정보(0x01: quaz, 0x02: mark1, 0x03: mark2)
 retn : TRUE or FALSE
*/
BOOL CBSAThread::SendCenteringData(ENG_VCPC cmd, UINT8 cam_no, UINT8 unit)
{
	UINT8 u8Pkt[2]	= { cam_no, unit };
	return SendData(cmd, u8Pkt, 2);
}

/*
 desc : Vision Centering Command Data Send
 parm :	cmd		- [in] command
		cam_no	- [in] 카메라 설정 (0x01: bsa , 0x02: up)
		unit	- [in] 측정 위치정보(0x01: quaz, 0x02: mark1, 0x03: mark2)
 retn : TRUE or FALSE
*/
BOOL CBSAThread::SendFocusData(ENG_VCPC cmd, UINT8 cam_no, UINT8 unit)
{
	UINT8 u8Pkt[2]	= { cam_no, unit };
	return SendData(cmd, u8Pkt, 2);
}

/*
 desc : Vision Focus Command Data Send
 parm :	cmd		- [in] command
		point	- [in] 측정 순서 (0x01: 측정1, 0x02: 측정2)
		mark	- [in] Mark 번호 (0x01: mark1, 0x02: mark2)
		pattern	- [in] pattern 번호 
		pos_x	- [in] 현재 Stage X축 위치
		pos_y	- [in] 현재 Stage Y축 위치
 retn : TRUE or FALSE
*/
BOOL CBSAThread::SendGrabData(ENG_VCPC cmd, UINT8 point, UINT8 mark, UINT8 pattern,
							  INT32 pos_x, INT32 pos_y)
{
	UINT8 u8Cmd[3]	= { point, mark, pattern }, u8Pkt[16] = {NULL};
	UINT32 u32Cmd[2]= { SWAP32(UINT32(pos_x)), SWAP32(UINT32(pos_y)) };

	memcpy(u8Pkt, u8Cmd, 3);
	memcpy(u8Pkt+3, u32Cmd, 8);

	return SendData(cmd, u8Pkt, 11);
}

/*
 desc : Vision Centering Command Data Send
 parm :	cmd		- [in] command
		mode	- [in] ?
 retn : TRUE or FALSE
*/
BOOL CBSAThread::SendViewData(ENG_VCPC cmd, UINT8 mode)
{
	return SendData(cmd, (PUINT8)&mode, 1);
}

/*
 desc : Vision Centering Command Data Send
 parm :	cmd		- [in] command
		mark	- [in] Mark 번호 (0x01: mark1, 0x02: mark2)
		unit	- [in] 측정 위치정보(0x01: quaz, 0x02: mark1, 0x03: mark2)
 retn : TRUE or FALSE
*/
BOOL CBSAThread::SendIllumination(ENG_VCPC cmd, UINT8 mark, UINT8 onoff)
{
	UINT8 u8Pkt[2]	= { mark, onoff };
	return SendData(cmd, u8Pkt, 2);
}

/*
 desc : Vision Joblist Command Data Send
 parm :	cmd			- [in] command
 retn : TRUE or FALSE
*/
BOOL CBSAThread::SendJoblist(ENG_VCPC cmd, UINT64 mark_1, UINT64 mark_2)
{
	UINT32 i, u32Pkt[4]	= { NULL };
	u32Pkt[0]	= (mark_1 & 0xffffffff00000000) >> 32;
	u32Pkt[1]	= (mark_1 & 0x00000000ffffffff);
	u32Pkt[2]	= (mark_2 & 0xffffffff00000000) >> 32;
	u32Pkt[3]	= (mark_2 & 0x00000000ffffffff);
	for (i=0; i<4; i++)	u32Pkt[i]	= SWAP32(u32Pkt[i]);

 	return SendData(cmd, (PUINT8)u32Pkt, sizeof(UINT32));
}

/*
 desc : 콜백 함수 처리
 parm : flag	- [in]  send_succ:0x01, recv_succ:0x02, send_fail_0xf1, recv_fail:0xf2
		data	- [in]  송신 or 수신된 데이터가 저장된 구조체 포인터
 retn : None
*/
VOID CBSAThread::SetSockData(UINT8 flag, LPG_PSRB data)
{
	switch (flag)
	{
	case 0x01 : SetSendData(data);	break;
	case 0x02 : SetRecvData(data);	break;
	case 0xf1 :
	case 0xf2 :
		{
			TCHAR tzErr[2][16]	= { L"send", L"recv" };
			TCHAR tzMesg[128]	= { NULL };
			swprintf_s(tzMesg, 128, L"Failed to %s the packet (cmd:%02x) for vision_tcp/ip",
					   tzErr[flag-0xf1], data->cmd);
			LOG_ERROR(ENG_EDIC::en_bsa, tzMesg);
		}
		break;
	}
}

/*
 desc : 수신된 데이터가 있는지 여부 반환
 parm : cmd	- [in]  명령어
 retn : TRUE or FALS
*/
BOOL CBSAThread::IsRecvData(ENG_VCPC cmd)
{
	UINT8 u8Cmd	= 0x00;

	/* 패킷 송신에 대한 플래그 설정 */
	u8Cmd	= (UINT16(cmd) & 0x00ff);
	if (u8Cmd > 32)
	{
		TCHAR tzMesg[64]	= {NULL};
		swprintf_s(tzMesg, 64, L"Invalid command call (cmd : %02x)", UINT8(cmd));
		LOG_WARN(ENG_EDIC::en_bsa, tzMesg);
		return FALSE;
	}

	return m_pPktRecvFlag[u8Cmd] == 0x02;
}

/*
 desc : 송신 데이터 처리
 parm : data	- [in]  수신된 데이터가 저장된 구조체 포인터
 retn : None
*/
VOID CBSAThread::SetSendData(LPG_PSRB data)
{
	UINT8 u8Cmd	= 0x00;
	TCHAR tzMesg[1024]	= { NULL };
	TCHAR tzData[512]	= { NULL };
	STG_SBVC stPktSend	= { NULL };

	/* 패킷 송신에 대한 플래그 설정 */
	u8Cmd	= (data->cmd & 0x00ff);
	if (u8Cmd < 32)	m_pPktRecvFlag[u8Cmd]	= 0x01;	/* Sended */

	switch (data->cmd)
	{
	case ENG_VCPC::en_vision_req_alive			:
	case ENG_VCPC::en_vision_req_state			:
	case ENG_VCPC::en_vision_req_error			:
	case ENG_VCPC::en_vision_req_reset			:
	case ENG_VCPC::en_vision_req_stop			:
	case ENG_VCPC::en_vision_req_cam_offset		:
	case ENG_VCPC::en_vision_req_marklist		:
	case ENG_VCPC::en_vision_req_job_pat_list	:
	case ENG_VCPC::en_vision_req_align_result	:	swprintf_s(tzData, 512, L"null");		break;
	case ENG_VCPC::en_vision_req_centering		:
		memcpy(&stPktSend.centering, data->data, sizeof(STG_SVCV));
		swprintf_s(tzData, 512, L"cam:%x, unit:%x",
				   stPktSend.centering.cam_num, stPktSend.centering.unit);					break;
	case ENG_VCPC::en_vision_req_focus			:
		memcpy(&stPktSend.focus, data->data, sizeof(STG_SVFV));
		swprintf_s(tzData, 512, L"cam:%x, unit:%x",
				   stPktSend.focus.cam_num, stPktSend.focus.unit);							break;
	case ENG_VCPC::en_vision_req_align_grab		:
		memcpy(&stPktSend.align, data->data, sizeof(STG_SAVV));
		stPktSend.align.SwapData();
		swprintf_s(tzData, 512, L"pt:%x, Mark:%x, Pat:%x, pos_x:%d, pos_y:%d",
				   stPktSend.align.point_num, stPktSend.align.mark_num, stPktSend.align.pattern_num,
				   stPktSend.align.pos_x, stPktSend.align.pos_y);							break;
	case ENG_VCPC::en_vision_req_illumination	:
		memcpy(&stPktSend.illum, data->data, sizeof(STG_SIMO));
		swprintf_s(tzData, 512, L"mark:%x, onoff:%d",
				   stPktSend.illum.mark, stPktSend.illum.onoff);							break;
	case ENG_VCPC::en_vision_req_view			:
		memcpy(&stPktSend.view, data->data, sizeof(STG_SVVM));
		swprintf_s(tzData, 512, L"unit : %x", stPktSend.view.mode);							break;
	}

	/* Cmd + Data */
	swprintf_s(tzMesg, 1024, L"SEND :[cmd=%x],[%s]", data->cmd, tzData);
	SetNotiMsg(tzMesg);
}

/*
 desc : 수신받은 데이터 처리
 parm : data	- [in]  수신된 데이터가 저장된 구조체 포인터
 retn : None
*/
VOID CBSAThread::SetRecvData(LPG_PSRB data)
{
	UINT8 u8Cmd	= 0x00;
	PUINT8 pData= data->data;

	/* 패킷 수신에 대한 플래그 설정 */
	if (data->cmd != (UINT16)ENG_VCPC::en_vision_res_state)
	{
		u8Cmd	= (data->cmd & 0x00ff);
		if (u8Cmd < 32)	m_pPktRecvFlag[u8Cmd]	= 0x02;	/* Received */
	}

	switch (data->cmd)
	{
	case ENG_VCPC::en_vision_res_alive			:
	case ENG_VCPC::en_vision_res_stop			:
	case ENG_VCPC::en_vision_res_reset			:
	case ENG_VCPC::en_vision_res_illumination	:	break;;

	case ENG_VCPC::en_vision_res_state			:	m_pstShMemVisi->vision_state	= pData[0];
													m_pstShMemVisi->update_state_time	= GetTickCount64();			break;
	case ENG_VCPC::en_vision_res_error			:	memcpy(&m_pstShMemVisi->vision_error, pData, sizeof(STG_RVEI));	break;
	case ENG_VCPC::en_vision_res_centering		:	memcpy(&m_pstShMemVisi->centering, pData, sizeof(STG_RVCV));
													m_pstShMemVisi->centering.SwapData();							break;
	case ENG_VCPC::en_vision_res_cam_offset		:	memcpy(&m_pstShMemVisi->cam_offset, pData, sizeof(STG_RVCO));
													m_pstShMemVisi->cam_offset.SwapData();							break;

	case ENG_VCPC::en_vision_res_focus			:	memcpy(&m_pstShMemVisi->focus, pData, sizeof(STG_RVFV));
													m_pstShMemVisi->focus.SwapData();								break;
	case ENG_VCPC::en_vision_res_marklist		:	memcpy(&m_pstShMemVisi->pattern, pData, sizeof(STG_RVPM));
													m_pstShMemVisi->pattern.SwapData();								break;
	case ENG_VCPC::en_vision_res_align_grab		:	SetAlignGrab(pData);										break;
	case ENG_VCPC::en_vision_res_align_result	:	memcpy(&m_pstShMemVisi->align_info.align_result, pData, sizeof(STG_RVAR));
													m_pstShMemVisi->align_info.align_result.SwapData();				break;
	case ENG_VCPC::en_vision_res_job_pat_list	:	m_pstShMemVisi->job_list.result	= pData[0];						break;
	
	case ENG_VCPC::en_vision_res_view			:	m_pstShMemVisi->view_info.result	= pData[0];					break;
	}

	/* NotiEvnet Msg 저장 */
	SetNotiData(data);
}

/*
 desc : Align에 Mark Grab 유/무 수신
 parm : data	- [in]  수신된 데이터가 저장된 구조체 포인터
 retn : None
*/
VOID CBSAThread::SetAlignGrab(PUINT8 data)
{
	/* Mark번호(data[1]), Grab데이터(data[2]) 데이터 유효성 검사 */
	if (data[1] < 0 || data[2] < 0)			return;
	/* Align Mark 별 Grab 데이터 저장 */
	switch (data[0])
	{
	case 0x01 : m_pstShMemVisi->align_info.mark_grab.mark_1_grab = data[2];	break;	/* Mark 1 */
	case 0x02 : m_pstShMemVisi->align_info.mark_grab.mark_2_grab = data[2];	break;	/* Mark 2 */
	}
}

/*
 desc : NotiEvnet Msg 저장
 parm : mesg	- [in]  Noti Mesage
 retn : None
*/
VOID CBSAThread::SetNotiMsg(PTCHAR mesg)
{
	TCHAR tzMesg[1024]	= {NULL};

 	m_bNotiEvent= TRUE;
	CTime cTime	= CTime::GetCurrentTime();	/* 현재 시스템으로부터 날짜 및 시간을 얻어 온다. */
	swprintf_s(m_pEventMsg, 1024, L"(%02d:%02d:%02d) %s",
			   cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond(), mesg);

	/* 로그 파일에 저장 */
	swprintf_s(tzMesg, 1024, L"Vision set log data <%s>", mesg);
	LOG_MESG(ENG_EDIC::en_bsa, tzMesg);
}

/*
 desc : View Control 에 측정 결과 수신
 parm : data	- [in]  수신된 데이터가 저장된 구조체 포인터
 retn : None
*/
VOID CBSAThread::SetNotiData(LPG_PSRB data)
{
	TCHAR tzMesg[1024]		= { NULL };
	TCHAR tzData[512]		= { NULL };
	TCHAR tzstatus[4][32]	= { L"READY", L"BUSY", L"MANUAL", L"ERROR" };
	TCHAR tzunit[3][32]		= { L"NONE", L"BSA", L"UP" };
	TCHAR tzMark[3][32]		= { L"NONE", L"MARK1", L"MARK2" };
	TCHAR tzgrab[2][32]		= { L"GRAB", L"FAIL" };

	switch (data->cmd)
	{
	case ENG_VCPC::en_vision_res_alive			:
	case ENG_VCPC::en_vision_res_stop			:
	case ENG_VCPC::en_vision_res_reset			:	
	case ENG_VCPC::en_vision_res_illumination	:	swprintf_s(tzData, 512, L"null");				break;
	case ENG_VCPC::en_vision_res_state			:
		swprintf_s(tzData, 512, L"status:%s", tzstatus[m_pstShMemVisi->vision_state]);				break;
	case ENG_VCPC::en_vision_res_error			:
		swprintf_s(tzData, 512, L"unit:%x, error:%x", m_pstShMemVisi->vision_error.unit,
				   m_pstShMemVisi->vision_error.error);												break;
	case ENG_VCPC::en_vision_res_centering		:
		if (m_pstShMemVisi->centering.common.unit > 3 || m_pstShMemVisi->centering.common.unit < 0)	return;
		swprintf_s(tzData, 512, L"cam:%s, result:%x, dx:%d, dy:%d",
				   tzunit[m_pstShMemVisi->centering.common.unit], m_pstShMemVisi->centering.common.result,
				   m_pstShMemVisi->centering.move_x, m_pstShMemVisi->centering.move_y);				break;
	case ENG_VCPC::en_vision_res_cam_offset		:
		swprintf_s(tzData, 512, L"result:%x, ofx:%d, ofy:%d", m_pstShMemVisi->cam_offset.result,
				   m_pstShMemVisi->cam_offset.offset_x, m_pstShMemVisi->cam_offset.offset_y);		break;
	case ENG_VCPC::en_vision_res_focus			:
		if (m_pstShMemVisi->focus.common.unit > 3 || m_pstShMemVisi->focus.common.unit < 0)			return;
		swprintf_s(tzData, 512, L"cam:%s, result:%x, data:%d, dz:%d",
				   tzunit[m_pstShMemVisi->focus.common.unit], m_pstShMemVisi->focus.common.result,
				   m_pstShMemVisi->focus.focus_data, m_pstShMemVisi->focus.move_z);					break;
	case ENG_VCPC::en_vision_res_marklist		:	swprintf_s(tzData, 512, L"MarkList");			break;
	case ENG_VCPC::en_vision_res_align_grab		:
		if (data->data[1] < 0 || data->data[1] > 4)	 return;
		if (data->data[2] < 0 || data->data[2] > 4)	 return;
		swprintf_s(tzData, 512, L"Mark:%s, result:%s",
				   tzMark[data->data[1]], tzgrab[data->data[2]]);									break;
	case ENG_VCPC::en_vision_res_align_result	:
		swprintf_s(tzData, 512, L"result:%x, m1x:%d, m1y:%d, m2x:%d, m2y:%d, sfx:%d, sfy:%d, sfa:%d",
			m_pstShMemVisi->align_info.align_result.result,
			m_pstShMemVisi->align_info.align_result.mark_1_pos_x,
			m_pstShMemVisi->align_info.align_result.mark_1_pos_y,
			m_pstShMemVisi->align_info.align_result.mark_2_pos_x,
			m_pstShMemVisi->align_info.align_result.mark_2_pos_y,
			m_pstShMemVisi->align_info.align_result.shfit_x,
			m_pstShMemVisi->align_info.align_result.shfit_y,
			m_pstShMemVisi->align_info.align_result.shfit_angle);									break;
	case ENG_VCPC::en_vision_res_job_pat_list	:
		swprintf_s(tzData, 512, L"result:%x", m_pstShMemVisi->job_list.result);						break;
	case ENG_VCPC::en_vision_res_view			:
		swprintf_s(tzData, 512, L"result:%x", m_pstShMemVisi->view_info.result);					break;
	}

	swprintf_s(tzMesg, 1024, L"RECV :[cmd=%x],[%s]", data->cmd, tzData);
	SetNotiMsg(tzMesg);
}

/*
 desc : Vision State 상태가 주어진 시간 안에 갱신되었는지 확인
 parm : time	- [in]  이 시간 이내에 갱신되었는지 확인 시간 (단위: msec)
 retn : TRUE (주어진 시간보다 짧게 갱신되었다)
		FALSE (주어진 시간 이내에 갱신되지 않았다. 즉, 수신된 데이터가 아직 없다.)
*/
BOOL CBSAThread::IsUpdateStateTime(UINT64 time)
{
	/* 현재 시간 대비, 최근에 갱신한 시간을 빼서, 주어진 시간보다 큰지 여부 */
	return (GetTickCount64() > (m_pstShMemVisi->update_state_time + time)) ? FALSE : TRUE;
}
