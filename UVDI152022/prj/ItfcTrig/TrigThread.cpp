
/*
 desc : TCP/IP Client Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "TrigThread.h"
#include "SendQue.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : trig	- [in]  Trigger Server 통신 정보
		conf	- [in]  Trigger 환경 정보
		shmem	- [in]  Trigger Shared Memory
		comn	- [in]  전체 공통 환경 정보
 retn : None
*/
CTrigThread::CTrigThread(LPG_CTCS trig, LPG_CTSP conf, LPG_TPQR shmem,
						 LPG_DLSM link, LPG_CCGS comn)
	: CClientThread(ENG_EDIC::en_trig,
					0x0000,
					trig->tcp_port,
					trig->source_ipv4,	// CMPS Local (Client) IPv4
					trig->target_ipv4,	// PLS Remote (Server) IPv4
					trig->port_buff,
					trig->recv_buff,
					trig->ring_buff,
					trig->idle_time,
					trig->link_time,
					trig->sock_time)
{
	/* ------------------------------------------- */
	/* RunWork 함수 본문을 바로 수행 하도록 처리함 */
	/* ------------------------------------------- */
	m_bInitRecv		= FALSE;	/* 초기 수신된 데이터가 존재하지 않음 */
	m_bInitUpdate	= FALSE;
	m_u64RecvTime	= GetTickCount64();
	m_u64SendTime	= GetTickCount64();
	m_pstConfTrig	= conf;
	m_pstShMemTrig	= shmem;
	m_pstShMemLink	= link;
	m_pstConfComn	= comn;

	/* Check whether it operates in demo mode */
	if (GetConfig()->IsRunDemo())	m_enSysID = ENG_EDIC::en_demo;

	/* ----------------------------------------------------------- */
	/* 사용자(운영자)의 명령을 임시로 저장할 송신 데이터 객체 생성 */
	/* ----------------------------------------------------------- */
	m_pSendQue		= new CSendQue(256);
	ASSERT(m_pSendQue);
	/* 가장 최근 값 초기화 */
	UINT8 i, j;
	for (i=0; i<MAX_TRIG_CHANNEL; i++)
	{
		for (j=0; j<MAX_TRIG_CHANNEL; j++)
		{
			m_u32SendTrigPos[i][j]	= MAX_TRIG_POS;
		}
	}

#if (TRIG_RECV_OUTPUT)
	m_u64TraceTime	= 0;
#endif
}

/*
 desc : Destructor
 parm : None
*/
CTrigThread::~CTrigThread()
{
	/* Trigger Server와 연결 강제 해제 */
	CloseClient();
	/* 송신 큐 메모리 해제 */
	delete m_pSendQue;
}

/*
 desc : 스레드 실행시 주기적 호출됨
 parm : None
 retn : None
*/
VOID CTrigThread::DoWork()
{
	BOOL bSendData	= FALSE;
	/* 현재 작업 시간 */
	UINT64 u64Tick	= GetTickCount64();
#if 1
	/* 임의 시간 동안 수신된 데이터가 없다면 연결해제 처리 */
	if (m_u32IdleTime && ((m_u64RecvTime + UINT64(m_u32IdleTime)) < u64Tick))
	{
		/* 강제로 연결 해제 되었다고 로그 남김 */
		TCHAR tzMesg[LOG_MESG_SIZE]	= {NULL};
		swprintf_s(tzMesg, LOG_MESG_SIZE,
				   L"Alive Time-out (idle time-out : recv_time:%I64u + idle_time:%I32u < cur_time:%I64u)",
				   m_u64RecvTime, m_u32IdleTime, u64Tick);
		LOG_ERROR(ENG_EDIC::en_trig, tzMesg);
		/* 가장 최근 작업 시간 저장 */
		m_u64RecvTime	= u64Tick;
		/* 강제로 연결 해제 수행 */
		CloseClient();
		return;
	}
#endif
	/* 초기 송신에 대한 수신이 있고, 큐에 송신될 명령가 없다면, 송신 작업 진행 */
	if (IsPopEmptyData())
	{
		/* ------------------------------------------------------------------ */
		/* 주기적으로 Trigger의 상태 값을 읽어오는 주기는 1000 msec 마다 진행 */
		/* ------------------------------------------------------------------ */
		if (u64Tick > (m_u64SendTime + 250))
		{
			bSendData	= TRUE;	/* Trigger 상태 값 요청 패킷 송신 */
			ReqReadSetup();
		}
	}
	else
	{
		/* ---------------------------------------------------------------------- */
		/* 방금 전에 수신된 데이터가 있으면 50 msec 이후에, 송신 수행하고, 이전에 */
		/* 송신된 데이터에 대해, 250 msec 대기시간 동안 수신되지 않으면 송신 진행 */
		/* ---------------------------------------------------------------------- */
		if (u64Tick > (m_u64SendTime + 250) || u64Tick > (m_u64RecvTime + 50))
		{
			bSendData	= TRUE;		/* 임의 패킷 데이터 송신 */
		}
	}

	/* 패킷 데이터 송신 진행 */
	if (bSendData && (u64Tick > (m_u64SendTime + 250)))
	{
		PopSendData();
	}
}

/*
 desc : Camera Index (1 or 2)에 대한 채널 번호 값 반환
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
 retn : Camera Index에 해당되는 Trigger Board의 Channel ID (0x01 ~ 0x04) 값 반환
		0x00 값이면 실패 처리
*/
UINT8 CTrigThread::GetTrigChNo(UINT8 cam_id)
{
#if 1
	UINT8 u8Type	= m_pstConfComn->strobe_lamp_type;
	if (0x01 == cam_id)
	{
		if (!u8Type)	return 0x01;
		else			return 0x02;
	}
	else
	{
		if (!u8Type)	return 0x03;
		else			return 0x04;
	}
#else	/* Trigger Event는 무조건 1 or 3 CH에서만 발생시켜야 됨 (현재 DI 장비 구조적 문제) */
	if (0x01 == cam_id)	return 0x01;
	else				return 0x03;
#endif

	return 0x00;
}

/*
 desc : 데이터가 수신된 경우, 호출됨
 parm : None
 retn : TRUE - 성공적으로 분석 (Ring Buffer) 처리
		FALSE- 분석 처리하지 않음 (아직 완벽하지 않은 패킷임)
*/
BOOL CTrigThread::RecvData()
{
	UINT32 u32Read	= 0;
	STG_TPQR stData	= {NULL};		// 매 실시간 수신되는 데이터 임시 저장

	/* 현재 패킷 수신 모드라고 설정 */
	m_enSocket	= ENG_TPCS::en_recving;

	do	{

		/* ----------------------------------------------------------------- */
		/* 현재 수신된 데이터가 기본 헤더 정보 크기만큼 저장되어 있는지 확인 */
		/* ----------------------------------------------------------------- */
		u32Read	= m_pPktRingBuff->GetReadSize();
		if (u32Read < sizeof(STG_TPQR))	break;
		/* ----------------------------------------------------------- */
		/* 먼저 기존에 Copy된 응답 헤더 부분을 버퍼로부터 읽은 후 버림 */
		/* ----------------------------------------------------------- */
		m_pPktRingBuff->ReadBinary((PUINT8)&stData, sizeof(STG_TPQR));
		/* ----------------------------------------------------------- */
		/* 아래 NotiEvent 함수에서 STG_MFPR의 크기 값은 큰 의미가 없음 */
		/* ----------------------------------------------------------- */
		SetRecvPacket(ENG_TPCS::en_recved, (PUINT8)&stData, sizeof(STG_TPQR));
		/* 초기 데이터가 수신되었다고 설정 */
		if (!m_bInitRecv)	m_bInitRecv	= TRUE;

	} while (1);

	/* --------------------------------------------------------- */
	/* 가장 최근에 수신된 시간 저장. 장시간 수신 없으면 연결 해제*/
	/* --------------------------------------------------------- */
	m_u64RecvTime	= GetTickCount64();

	return TRUE;
}

/*
 desc : 패킷 수신 처리
 parm : type	- [in]  이벤트 종류 (ENG_TPCS)
		data	- [in]  송신 or 수신된 데이터가 저장된 구조체 포인터
		size	- [in]  'data' 버퍼에 저장된 크기
 retn : None
*/
VOID CTrigThread::SetRecvPacket(ENG_TPCS type, PUINT8 data, UINT32 size)
{
	LPG_TPQR pstData	= LPG_TPQR(data);
	/* 수신된 데이터 Big -> Little Endian */
	pstData->SWAP();

	/* Shared Memory 복사 */
	if (ENG_TPCS::en_recved == type)
	{
		memcpy(m_pstShMemTrig, data, size);
	}
#if (TRIG_RECV_OUTPUT)
	if (pstData->trig_strob_ctrl == 0x01 && m_u64TraceTime+1000 <GetTickCount64())
	{
		m_u64TraceTime	= GetTickCount64();
		LPG_TPQR d = pstData;
		for (UINT8 i=0; i<MAX_TRIG_CHANNEL; i++)
		{
			LPG_TPTS t = &pstData->trig_set[i];
			TRACE(L"EncCounter = %10d TrigStrobCtrl = %1u EncOutVal = %10u "
				  L"CH = <%1u> AreaOnTime.Trig/Strobe/Delay = <%5u> <%5u> <%4u> "
				  L"AreaTrigPos.Plus/Minus = <%5u> <%5u> Trig.Pos = [%10u] [%10u] [%10u] [%10u]\n",
				  d->enc_counter, d->trig_strob_ctrl, d->enc_out_val,
				  i+1, t->area_trig_ontime, t->area_strob_ontime, t->area_strob_trig_delay,
				  t->area_trig_pos_plus, t->area_trig_pos_minus,
				  t->area_trig_pos[0], t->area_trig_pos[1],
				  t->area_trig_pos[2], t->area_trig_pos[3]);
		}
		TRACE("\n");
	}
#endif
}

/*
 desc : 서버에 연결이된 경우
 parm : sd	- 소켓 기술자
 retn : TRUE or FALSE
*/
BOOL CTrigThread::Connected()
{
	/* 연결되었을 때, 각 시간 모두 저장 */
	m_u64SendTime	= GetTickCount64();
	m_u64RecvTime	= GetTickCount64();
	/* 연결되고 난 이후 초기화 요청하지 않았다고 설정 */
	m_bInitRecv		= FALSE;	/* 초기 연결되고 난 이후 수신된 데이터가 없다고 설정 */
	/* 연결 완료 상태 */
	UpdateLinkTime(0x01);
#if 1
	/* 초기 값 설정 명령어 송신 */
	return ReqInitUpdate();
#else
	return TRUE;
#endif
}

/*
 desc : 서버로부터 연결 해제가 된 경우
 parm : None
 retn : None
*/
VOID CTrigThread::CloseSocket(SOCKET sd, UINT32 time_out/* =0 */)
{

	// 상위 객체 호출
	CClientThread::CloseSocket(sd, time_out);
}

/*
 desc : 현재 송신될 패킷이 존재하는지 여부 확인
 parm : None
 retn : FALSE - POP 할 데이터가 있다. TRUE - POP 할 데이터가 없다
*/
BOOL CTrigThread::IsPopEmptyData()
{
	BOOL bIsEmpty	= TRUE;

	/* 동기화 진입 */
	if (m_syncSend.Enter())
	{
		// Queue가 비어 있다면 TRUE이므로, POP 할 수 없다.
		bIsEmpty = m_pSendQue->IsQueEmpty();

		/* 동기화 해제 */
		m_syncSend.Leave();
	}

	return bIsEmpty;
}

/*
 desc : 송신할 데이터가 있으면, Queue로부터 데이터 하나 가져온 후 송신 작업 수행
 parm : None
 retn : None
*/
VOID CTrigThread::PopSendData()
{
	BOOL bSucc		= FALSE;
	LPG_TPQR pstQue	= NULL;

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		/* ---------------------------------- */
		/* 큐에 전송 데이터가 존재하는지 확인 */
		/* ---------------------------------- */
		pstQue	= m_pSendQue->PopData();
		if (pstQue)
		{
			/* !!! 반드시 송신 시간 정보를 여기에 기입. !!! 가장 최근에 송신한 시간 저장 */
			m_u64SendTime	= GetTickCount64();
			/* !!! 중요 !!! -> 마지막 Link 크기 제외 */
			/* 데이터 전송 */
			bSucc = ReSended((PUINT8)pstQue, sizeof(STG_TPQR), UINT32(3), 30);
			/* 송신 실패이면 ... */
			if (!bSucc)
			{
				LOG_ERROR(ENG_EDIC::en_trig, L"Failed to send the packet data [Trigger Info]");
			}
		}

		/* 동기 해제 */
		m_syncSend.Leave();
	}
}

/*
 desc : Trigger Board에 설정된 값 요청
 parm : None
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ReqReadSetup()
{
	BOOL bSucc			= FALSE;
	STG_TPQR stPktData	= {NULL};

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		/* 초기에 EEPROM에 저장된 내용 가져오기 */
		stPktData.command	= htonl(UINT32(ENG_TBPC::en_read));	/* SwapData 함수 호출할 경우, 부하가 크므로 */
		/* 송신될 데이터 버퍼에 등록 */
		bSucc = m_pSendQue->PushData(&stPktData);
		if (!bSucc)
		{
			LOG_WARN(ENG_EDIC::en_trig, L"The send packet buffer is full");
		}
		/* 동기 해제 */
		m_syncSend.Leave();
	}

	return bSucc;
}

#if 0
/*
 desc : Trigger Board에 처음 연결되고 난 이후, 환경 파일에 설정된 값으로 초기화 진행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ReqInitUpdate()
{
	BOOL bSucc			= FALSE;
	UINT32 i = 0, j		= 0;
	STG_TPQR stPktData	= {NULL};

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		/* 기존 수신된 데이터 초기화 */
		m_pstShMemTrig->ResetPktData();

		/* 1th : Base Setup */
		stPktData.command			= UINT32(ENG_TBPC::en_write);
		stPktData.enc_counter		= INT32(ENG_TECM::en_clear);
		stPktData.trig_strob_ctrl	= UINT32(ENG_TEED::en_disable);
		/* 초기 값 설정 */
		for (i=0x00; i<0x04; i++)
		{
			stPktData.trig_set[i].area_trig_ontime		= m_pstConfTrig->trig_on_time[i];
			stPktData.trig_set[i].area_strob_ontime		= m_pstConfTrig->strob_on_time[i];
			stPktData.trig_set[i].area_trig_pos_plus	= m_pstConfTrig->trig_plus[i];
			stPktData.trig_set[i].area_trig_pos_minus	= m_pstConfTrig->trig_minus[i];
			stPktData.trig_set[i].area_strob_trig_delay	= m_pstConfTrig->trig_delay_time[i];
			/* 초기 트리거 발생 위치 값을 0인 아닌 MAX 값으로 설정 */
			for (j=0; j<MAX_REGIST_TRIG_POS_COUNT; j++)
			{
				stPktData.trig_set[i].area_trig_pos[j]	= MAX_TRIG_POS;
			}
		}
		/* Network Bytes Ordering */
		stPktData.SWAP();
		bSucc = m_pSendQue->PushData(&stPktData);

		/* 동기 해제 */
		m_syncSend.Leave();
	}

	/* 2th : Internal Trigger - Disabled */
	ReqWriteEncoderOut(0x00000000);

	return bSucc ? ReqReadSetup() : FALSE;
}
#else
/*
 desc : 특정한 경우에만 사용. (트리거보드의 트리거는 2개 밖에 발생안되기 때문에...)
 parm : None
 retn : TRUE or FALSE
 note : 트리거는 무조건 1, 3 번 카메라(램프;채널)만 발생하기 때문에...
		2, 4번 카메라 (램프;채널)로 발생시키기 위한 방법
*/
BOOL CTrigThread::ReqInitUpdate()
{
	BOOL bSucc			= FALSE;
	UINT8 u8LampType	= m_pstConfComn->strobe_lamp_type;	/* 0x00: Coaxial, 0x01: Ring */
	UINT32 i = 0, j		= 0;
	STG_TPQR stPktData	= {NULL};

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		/* 기존 수신된 데이터 초기화 */
		m_pstShMemTrig->ResetPktData();

		/* 1th : Base Setup */
		stPktData.command			= UINT32(ENG_TBPC::en_write);
		stPktData.enc_counter		= INT32(ENG_TECM::en_clear);
		stPktData.trig_strob_ctrl	= UINT32(ENG_TEED::en_disable);
		/* 트리거 이벤트 설정 */
		SetTrigEvent(TRUE, stPktData.trig_set);
		/* Network Bytes Ordering */
		stPktData.SWAP();
		bSucc = m_pSendQue->PushData(&stPktData);

		/* 동기 해제 */
		m_syncSend.Leave();
	}
#if 0
	/* 2th : Internal Trigger - Disabled */
	ReqWriteEncoderOut(0x00000000);
#endif
	return bSucc ? ReqReadSetup() : FALSE;
}
#endif

/*
 desc : 기존 등록된 4개 채널에 대한 Trigger Position 값 초기화 (최대 값) 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ResetTrigPosAll()
{
	BOOL bSucc			= FALSE;
	UINT32 i = 0, j		= 0;
	STG_TPQR stPktData	= {NULL};

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		stPktData.command	= (UINT32)ENG_TBPC::en_write;
		/* 기존 값 복사 */
		memcpy(&stPktData, m_pstShMemTrig, sizeof(STG_TPQR));
		/* 초기 값 설정 */
		for (; i<MAX_TRIG_CHANNEL; i++)
		{
			/* 초기 트리거 발생 위치 값을 0인 아닌 MAX 값으로 설정 */
			for (j=0; j<MAX_REGIST_TRIG_POS_COUNT; j++)	stPktData.trig_set[i].area_trig_pos[j]	= MAX_TRIG_POS;
		}
		/* 최근 트리거 위치 값 초기화 (반드시 패킷 버퍼에 넣는것보다 앞에 기입해야 함) */
		memset(m_u32SendTrigPos, MAX_TRIG_POS, sizeof(UINT32)*MAX_TRIG_CHANNEL*MAX_REGIST_TRIG_POS_COUNT);
		/* Network Bytes Ordering */
		stPktData.SWAP();
		bSucc = m_pSendQue->PushData(&stPktData);

		/* 동기 해제 */
		m_syncSend.Leave();
	}

	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : Trigger Board에 연결되고 난 이후 EEPROM 정보를 읽고/쓰기 위함
 parm : mode	- [in]  0x01 - Read, 0x02 - Write
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ReqEEPROMCtrl(UINT8 mode)
{
	BOOL bSucc			= FALSE;
	UINT32 u32Cmd		= mode == 0x01 ? UINT32(ENG_TBPC::en_eeprom_read) : UINT32(ENG_TBPC::en_eeprom_write);
	STG_TPQR stPktData	= {NULL};

	/* 만약 EEPROM으로부터 읽기 모드인 경우, 기존 값 모두 지우기 */
	if (0x01 == mode)	m_pstShMemTrig->ResetPktData();

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		/* 초기에 EEPROM에 저장된 내용 가져오기 */
		stPktData.command	= htonl(u32Cmd);	/* SwapData 함수 호출할 경우, 부하가 크므로 */
		bSucc = m_pSendQue->PushData(&stPktData);

		/* 동기 해제 */
		m_syncSend.Leave();
	}

	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : Board Reset
 parm : None
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ReqWriteBoardReset()
{
	BOOL bSucc			= FALSE;
	STG_TPQR stPktData	= {NULL};

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		/* 명령어 설정 */
		stPktData.command	= htonl(UINT32(ENG_TBPC::en_reset));
		/* 송신될 데이터 버퍼에 등록 */
		bSucc = m_pSendQue->PushData(&stPktData);
		/* 동기 해제 */
		m_syncSend.Leave();
	}
#if 0
	/* 기존 수신된 데이터 초기화 */
	m_pstShMemTrig->ResetPktData();
#endif
#if 0	/* SW Reset이면, 바로 응답이 어려우므로, 읽기 요청해도 무의미 */
	/* 무조건 1번 더 요청 (응답을 받기 위함) */
	return bSucc ? ReqReadSetup() : FALSE;
#else
#if 0	/* 나중에 Trigger Board에서 연결 해제 시그널이 오지 않으면 안됨 */
	if (bSucc)
	{
		/* 원격지가 끊어지므로, 여기서도 소켓 Close 수행 ? */
		CloseSocket(m_sdClient);
	}
#endif
	return bSucc;
#endif
}

/*
 desc : Area Trigger Event 설정
 parm : direct	- [in]  TRUE: 정방향, FALSE: 역방향
		trig_set- [out] Trigger Event 정보 설정 후 반환
 retn : None
 note : Trigger Event를 받는 곳은 채널 1/3 번뿐이고, Strobe Event를 받는 곳은 채널 1/2/3/4 모두 받음
		채널 2/4번을 통해 이미지를 받으려면 채널 1/3번으로 Trigger 이벤트 발생 시켜야 됨
*/
VOID CTrigThread::SetTrigEvent(BOOL direct, LPG_TPTS trig_set)
{
	UINT8 u8LampType= m_pstConfComn->strobe_lamp_type;	/* 0x00: Coaxial, 0x01: Ring */
	UINT32 i = 0;
#if 0
	/* 초기 값 설정 */
	for (i=0x00; i<0x04; i++)
	{
		/* Camera 1 / 2 - Coaxial Lamp (for 1 & 3 ch) */
		if (0x00 == u8LampType)
		{
			/* Coaxial Lamp 활성화 설정 */
			if (i == 0x00 || i == 0x02)
			{
				trig_set[i].area_trig_ontime				= m_pstConfTrig->trig_on_time[i];
				trig_set[i].area_strob_ontime				= m_pstConfTrig->strob_on_time[i];
				trig_set[i].area_strob_trig_delay			= m_pstConfTrig->trig_delay_time[i];
				if (direct)	trig_set[i].area_trig_pos_plus	= m_pstConfTrig->trig_plus[i];
				else		trig_set[i].area_trig_pos_minus	= m_pstConfTrig->trig_minus[i];
			}
		}
		/* Camera 1 / 2 - Ring Lamp (for 2 & 4 ch) (DI 장비 구조적 문제로 인해 T.T) */
		else
		{
			/* 1 & 3 CH의 경우, Trigger만 발생시키고, Strobe 는 발생하지 않도록 해야 함 */
			if (i == 0x00 || i == 0x02)
			{
				/* Trigger On Time의 경우, 1 & 3 CH의 경우, 2 & 4 CH의 값이 설정되도록 함 */
				trig_set[i].area_trig_ontime				= m_pstConfTrig->trig_on_time[i+1];
				if (direct)	trig_set[i].area_trig_pos_plus	= m_pstConfTrig->trig_plus[i+1];
				else 		trig_set[i].area_trig_pos_minus	= m_pstConfTrig->trig_minus[i+1];
			}
			/* 2 & 4 ch의 경우, Trigger가 발생해도 의미 없고 (HW 구조적 문제) Strobe 만 발생하도록 유지 */
			else
			{
				trig_set[i].area_strob_ontime		= m_pstConfTrig->strob_on_time[i];
				trig_set[i].area_strob_trig_delay	= m_pstConfTrig->trig_delay_time[i];
			}
		}
	}
#else
#if 0
	/* 초기 값 설정 */
	for (i=0x00; i<0x04; i++)
	{
		/* Camera 1 / 2 - Coaxial Lamp (for 1 & 3 ch) */
		if (0x00 == u8LampType)
		{
			/* Coaxial Lamp 활성화 설정 */
			if (i == 0x00 || i == 0x02)
			{
				trig_set[i].area_trig_ontime		= m_pstConfTrig->trig_on_time[i];
				trig_set[i].area_strob_ontime		= m_pstConfTrig->strob_on_time[i];
				trig_set[i].area_strob_trig_delay	= m_pstConfTrig->trig_delay_time[i];
				if (direct)
				{
					trig_set[i].area_trig_pos_plus	= m_pstConfTrig->trig_plus[i];
					trig_set[i].area_trig_pos_minus	= m_pstConfTrig->trig_minus[i];
				}
				else
				{
					trig_set[i].area_trig_pos_plus	= m_pstConfTrig->trig_minus[i];
					trig_set[i].area_trig_pos_minus	= m_pstConfTrig->trig_plus[i];
				}
			}
		}
		/* Camera 1 / 2 - Ring Lamp (for 2 & 4 ch) (DI 장비 구조적 문제로 인해 T.T) */
		else
		{
			/* 1 & 3 CH의 경우, Trigger만 발생시키고, Strobe 는 발생하지 않도록 해야 함 */
			if (i == 0x00 || i == 0x02)
			{
				/* Trigger On Time의 경우, 1 & 3 CH의 경우, 2 & 4 CH의 값이 설정되도록 함 */
				trig_set[i].area_trig_ontime		= m_pstConfTrig->trig_on_time[i+1];
				if (direct)
				{
					trig_set[i].area_trig_pos_plus	= m_pstConfTrig->trig_plus[i+1];
					trig_set[i].area_trig_pos_minus	= m_pstConfTrig->trig_minus[i+1];
				}
				else 
				{
					trig_set[i].area_trig_pos_plus	= m_pstConfTrig->trig_minus[i+1];
					trig_set[i].area_trig_pos_minus	= m_pstConfTrig->trig_plus[i+1];
				}
			}
			/* 2 & 4 ch의 경우, Trigger가 발생해도 의미 없고 (HW 구조적 문제) Strobe 만 발생하도록 유지 */
			else
			{
				trig_set[i].area_strob_ontime		= m_pstConfTrig->strob_on_time[i];
				trig_set[i].area_strob_trig_delay	= m_pstConfTrig->trig_delay_time[i];
			}
		}
	}
#else
	/* 초기 값 설정 */
	for (i=0x00; i<0x04; i++)
	{
		trig_set[i].area_trig_ontime		= m_pstConfTrig->trig_on_time[i];
		trig_set[i].area_strob_ontime		= m_pstConfTrig->strob_on_time[i];
		trig_set[i].area_strob_trig_delay	= m_pstConfTrig->trig_delay_time[i];
#if 0
		if (direct)
		{
			trig_set[i].area_trig_pos_plus	= 0/*m_pstConfTrig->trig_plus[i]*/;
			trig_set[i].area_trig_pos_minus	= 0/*m_pstConfTrig->trig_minus[i]*/;
		}
		else
		{
			trig_set[i].area_trig_pos_plus	= 0/*m_pstConfTrig->trig_minus[i]*/;
			trig_set[i].area_trig_pos_minus	= 0/*m_pstConfTrig->trig_plus[i]*/;
		}
#else
		trig_set[i].area_trig_pos_plus		= 0/*m_pstConfTrig->trig_plus[i]*/;
		trig_set[i].area_trig_pos_minus		= 0/*m_pstConfTrig->trig_minus[i]*/;
#endif
	}
#endif
#endif
}

/*
 desc : Area Trigger Position 설정
 parm : trig_set- [in]  Trigger Position 정보
		start	- [in]  트리거 저장 위치 (0 ~ 15)
		count	- [in]  트리거 등록 개수 (1 ~ 16)
		pos		- [in]  트리거 등록 위치가 저장되어 있는 배열 포인터 (unit : 100 nm or 0.1 um)
 note : Trigger Event를 받는 곳은 채널 1/3 번뿐이고, Strobe Event를 받는 곳은 채널 1/2/3/4 모두 받음
		채널 2/4번을 통해 이미지를 받으려면 채널 1/3번으로 Trigger 이벤트 발생 시켜야 됨
*/
VOID CTrigThread::SetTrigPos(LPG_TPTS trig_set,
							 UINT8 start1, UINT8 count1, PUINT32 pos1,
							 UINT8 start2, UINT8 count2, PUINT32 pos2)
{
	UINT8 u8LampType= m_pstConfComn->strobe_lamp_type;	/* 0x00: Coaxial, 0x01: Ring */
	UINT32 i = 0;
	UINT8 u8ChNo[2]	= {NULL};

	/* 각 Align Camera에 대한 채널 번호 얻기 */
	u8ChNo[0]	= GetTrigChNo(0x01) - 1;
	u8ChNo[1]	= GetTrigChNo(0x02) - 1;

	/* Camera 1 / 2 - Coaxial Trigger (for 1 & 3 ch) */
	if (0x00 == u8LampType)
	{
		/* 새로운 값 덮어 쓰고, 현재 설정된 값 임시 저장 */
		memcpy(&trig_set[u8ChNo[0]].area_trig_pos+start1, pos1, sizeof(UINT32) * count1);
		memcpy(&trig_set[u8ChNo[1]].area_trig_pos+start2, pos2, sizeof(UINT32) * count2);
		memcpy(&m_u32SendTrigPos[u8ChNo[0]]+start1, pos1, sizeof(UINT32) * count1);
		memcpy(&m_u32SendTrigPos[u8ChNo[1]]+start2, pos2, sizeof(UINT32) * count2);
	}
	/* Camera 1 / 2 - Ring Trigger (for 2 & 4 ch) (DI 장비 구조적 문제로 인해 T.T) */
	else
	{
		/* 일단 무조건 1/3 채널에도 Trigger 위치 설정 (카메라 이벤트 발생 시키기 위함) */
		memcpy(&trig_set[u8ChNo[0]-1].area_trig_pos+start1, pos1, sizeof(UINT32) * count1);
		memcpy(&trig_set[u8ChNo[1]-1].area_trig_pos+start2, pos2, sizeof(UINT32) * count2);
		/* 새로운 값 덮어 쓰고, 현재 설정된 값 임시 저장 */
		memcpy(&trig_set[u8ChNo[0]].area_trig_pos+start1, pos1, sizeof(UINT32) * count1);
		memcpy(&trig_set[u8ChNo[1]].area_trig_pos+start2, pos2, sizeof(UINT32) * count2);
		memcpy(&m_u32SendTrigPos[u8ChNo[0]]+start1, pos1, sizeof(UINT32) * count1);
		memcpy(&m_u32SendTrigPos[u8ChNo[1]]+start2, pos2, sizeof(UINT32) * count2);
	}
}

/*
 desc : 채널 별로 트리거 위치 등록
 parm : direct	- [in]  TRUE: 정방향, FALSE: 역방향
		start	- [in]  트리거 저장 위치 (0 ~ 15)
		count	- [in]  트리거 등록 개수 (1 ~ 16)
		pos		- [in]  트리거 등록 위치가 저장되어 있는 배열 포인터 (unit : 100 nm or 0.1 um)
		enable	- [in]  트리거 Enable or Disable
		clear	- [in]  트리거 위치 등록하면서 현재 Trigger Encoder 값을
						Clear (TRUE)할것인지 혹은 읽기 모드 (FALSE)를 유지할 것인지 여부
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ReqWriteAreaTrigPos(BOOL direct,
									  UINT8 start1, UINT8 count1, PUINT32 pos1,
									  UINT8 start2, UINT8 count2, PUINT32 pos2,
									  ENG_TEED enable, BOOL clear)
{
	UINT8 i1 = 0, i2	= 0, u3MaxTrig = MAX_REGIST_TRIG_POS_COUNT;
	UINT8 u8ChNo[2]		= {NULL};
	BOOL bSucc			= FALSE;
	STG_TPQR stPktData	= {NULL};

	/* 각 Align Camera에 대한 채널 번호 얻기 */
	u8ChNo[0]	= GetTrigChNo(0x01);
	u8ChNo[1]	= GetTrigChNo(0x02);

	if ((u8ChNo[0] < 1 || u8ChNo[0] > 4 || !(start1 >= 0 && start1 < MAX_REGIST_TRIG_POS_COUNT) || !pos1 ||
		 (start1 + count1) > MAX_REGIST_TRIG_POS_COUNT) ||
		(u8ChNo[1] < 1 || u8ChNo[1] > 4 || !(start2 >= 0 && start2 < MAX_REGIST_TRIG_POS_COUNT) || !pos2 ||
		 (start2 + count2) > MAX_REGIST_TRIG_POS_COUNT))
	{
		TCHAR tzMesg[LOG_MESG_SIZE]	= {NULL};
		swprintf_s(tzMesg, LOG_MESG_SIZE,
				   L"Trigger Invalid Parameter (ch_no1:%d ch_no2:%d start1:%d count1:%d start2:%d count2:%d)",
				   u8ChNo[0], u8ChNo[1], start1, count1, start2, count2);
		LOG_ERROR(ENG_EDIC::en_trig, tzMesg);
		return FALSE;
	}
	u8ChNo[0]--;
	u8ChNo[1]--;

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		stPktData.command			= (UINT32)ENG_TBPC::en_write;									/* Write */
		stPktData.enc_counter		= clear ? INT32(ENG_TECM::en_clear) : INT32(ENG_TECM::en_read);	/* Clear to encoder */
		stPktData.trig_strob_ctrl	= (UINT32)enable;														/* Trigger Enable or Disable */
		/* 기존 Area Trigger 값 모두 저장 */
		memcpy(stPktData.trig_set, m_pstShMemTrig->trig_set, sizeof(STG_TPTS) * 4 /* Max 4 channel */);
		/* 트리거 이벤트 설정 */
		SetTrigEvent(direct, stPktData.trig_set);
		/* 트리거 위치 설정 */
		SetTrigPos(stPktData.trig_set, start1, count1, pos1, start2, count2, pos2);
		/* 나머지 트리거 등록 위치는 최대 값 저장 */
		for (i1=0; i1<start1; i1++)					stPktData.trig_set[u8ChNo[0]].area_trig_pos[i1] = MAX_TRIG_POS;
		for (i1=start1+count1; i1<u3MaxTrig; i1++)	stPktData.trig_set[u8ChNo[0]].area_trig_pos[i1] = MAX_TRIG_POS;
		for (i2=0; i2<start2; i2++)					stPktData.trig_set[u8ChNo[1]].area_trig_pos[i2] = MAX_TRIG_POS;
		for (i2=start2+count1; i2<u3MaxTrig; i2++)	stPktData.trig_set[u8ChNo[1]].area_trig_pos[i2] = MAX_TRIG_POS;

		stPktData.SWAP();

		/* 송신될 데이터 버퍼에 등록 */
		bSucc = m_pSendQue->PushData(&stPktData);

		/* 동기 해제 */
		m_syncSend.Leave();
	}

	/* 현재 상태 값 읽기 요청 */
	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : 특정 채널에 트리거 위치 등록
 parm : cam_id	- [in]  Align Camera Index (0x01 or 0x02)
		start	- [in]  트리거 저장 위치 (0 ~ 15)
		count	- [in]  트리거 등록 개수 (1 ~ 16)
		pos		- [in]  트리거 등록 위치가 저장되어 있는 배열 포인터 (unit : 100 nm or 0.1 um)
		enable	- [in]  트리거 Enable or Disable
		clear	- [in]  트리거 위치 등록하면서 현재 Trigger Encoder 값을
						Clear (TRUE)할것인지 혹은 읽기 모드 (FALSE)를 유지할 것인지 여부
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ReqWriteAreaTrigPosCh(UINT8 cam_id, UINT8 start, UINT8 count, PUINT32 pos,
										ENG_TEED enable, BOOL clear)
{
	UINT8 i =0 ,j		= 0, u8ChNo;
	BOOL bSucc			= FALSE;
	STG_TPQR stPktData	= {NULL};

	/* 각 Align Camera에 대한 채널 번호 얻기 */
	u8ChNo	= GetTrigChNo(cam_id);

	if (u8ChNo < 1 || u8ChNo > 4 || !(start >= 0 && start < MAX_REGIST_TRIG_POS_COUNT) || !pos ||
		(start + count) > MAX_REGIST_TRIG_POS_COUNT)
	{
		TCHAR tzMesg[LOG_MESG_SIZE]	= {NULL};
		swprintf_s(tzMesg, LOG_MESG_SIZE,
				   L"Trigger Invalid Parameter (ch_no:%d start:%d count:%d)",
				   u8ChNo, start, count);
		LOG_ERROR(ENG_EDIC::en_trig, tzMesg);
		return FALSE;
	}
	u8ChNo--;

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		stPktData.command			= (UINT32)ENG_TBPC::en_write;									/* Write */
		stPktData.enc_counter		= clear ? (INT32)ENG_TECM::en_clear : (INT32)ENG_TECM::en_read;	/* Clear to encoder */
		stPktData.trig_strob_ctrl	= (UINT32)enable;												/* Trigger Enable or Disable */
		/* 트리거 이벤트 설정 */
		SetTrigEvent(TRUE, stPktData.trig_set);
		/* 새로운 값을 덮어 씀 */
		memcpy(&stPktData.trig_set[u8ChNo].area_trig_pos+start, pos, sizeof(UINT32) * count);
		/* 가장 최근의 트리거 송신 위치 값 저장 */
		memcpy(&m_u32SendTrigPos[u8ChNo]+start, pos, sizeof(UINT32) * count);
		stPktData.SWAP();
		/* 송신될 데이터 버퍼에 등록 */
		bSucc = m_pSendQue->PushData(&stPktData);
		/* 동기 해제 */
		m_syncSend.Leave();
	}

	/* 현재 상태 값 읽기 요청 */
	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : Trigger & Strobe Enable or Disable
 parm : enable	- [in]  TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ReqWriteTriggerStrobe(BOOL enable)
{
	UINT8 i	= 0x00;
	BOOL bSucc			= FALSE;
	STG_TPQR stPktData	= {NULL};

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		/* Encoder Counter Reset Command */
		stPktData.command			= (UINT32)ENG_TBPC::en_write;
		stPktData.enc_counter		= (INT32)ENG_TECM::en_clear;
		stPktData.trig_strob_ctrl	= enable ? (UINT32)ENG_TEED::en_enable : (UINT32)ENG_TEED::en_disable;
#if 1
		/* 트리거 이벤트 설정 */
		if (enable)	SetTrigEvent(TRUE, stPktData.trig_set);
#else	
		/* <!!! 중요 !!!> 기존 Area Trigger 값 모두 저장 */
		for (; i<MAX_TRIG_CHANNEL; i++)
		{
			stPktData.trig_set[i].area_trig_ontime		= m_pstConfTrig->trig_on_time[i];
			stPktData.trig_set[i].area_strob_ontime		= m_pstConfTrig->strob_on_time[i];
			stPktData.trig_set[i].area_strob_trig_delay	= m_pstConfTrig->trig_delay_time[i];
		}
#endif
		/* --------------------------------------------------------------------------------- */
		/* Triger & Strobe Enable 내지 Disable든 기존 등록된 Trigger Position 값 모두 초기화 */
		/* --------------------------------------------------------------------------------- */
		/* 최대 값 적용 */
		stPktData.ResetTrigPos(0);
		stPktData.ResetTrigPos(1);
		stPktData.ResetTrigPos(2);
		stPktData.ResetTrigPos(3);
		/* Network Bytes Ordering */
		stPktData.SWAP();
		/* 송신될 데이터 버퍼에 등록 */
		bSucc = m_pSendQue->PushData(&stPktData);
		/* 동기 해제 */
		m_syncSend.Leave();
	}

	/* 현재 상태 값 읽기 요청 */
	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : IP 변경하기
 parm : ip_addr	- [in]  패킷 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ReqWriteIP4Addr(PUINT8 ip_addr)
{
	BOOL bSucc			= TRUE;
	STG_TPQR stPktData	= {NULL};

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		/* 기존 등록된 IP 주소 초기화 */
		m_pstShMemTrig->ResetIPv4();
		/* IP 변경을 위한 명령어 설정 */
		if (bSucc)
		{
			stPktData.command	= htonl((UINT32)ENG_TBPC::en_ipaddr_write);
			memcpy(&stPktData.ip_addr, ip_addr, 4);
			bSucc = m_pSendQue->PushData(&stPktData);
		}
		if (bSucc)
		{
			/* 설정된 IP를 얻기 위한 명령어 설정 */
			stPktData.command	= htonl((UINT32)ENG_TBPC::en_ipaddr_read);
			bSucc = m_pSendQue->PushData(&stPktData);
		}
		/* 동기 해제 */
		m_syncSend.Leave();
	}

	/* 현재 상태 값 읽기 요청 */
	return bSucc;	/* IP가 바뀌면 연결이 끊어지던지, 리셋을 하던지 해야 함 */
}

/*
 desc : 내부 트리거 설정
 parm : onoff	- [in]  1 바이트 씩 의미가 있음 (4 Bytes 이므로, 총 4채널)
						초당 10 frame 출력
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ReqWriteEncoderOut(UINT32 enc_out)
{
	UINT8 i				= 0x00;
	BOOL bSucc			= FALSE;
	STG_TPQR stPktData	= {NULL};

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		/* 명령어 설정 */
		stPktData.command		= htonl((UINT32)ENG_TBPC::en_trig_internal);
		stPktData.enc_out_val	= htonl(enc_out);
		for (; i<4; i++)
		{
#if 0
			stPktData.trig_set[i].area_trig_ontime	= 2500;
			stPktData.trig_set[i].area_strob_ontime	= 2500;
#else	/* 현재 DI 장비 특성 때문에 (Trigger는 무조건 1 & 3 CH 에서만 발생 시켜야 됨) */
			if (i == 0x00 || i == 0x02)	stPktData.trig_set[i].area_trig_ontime	= 2500;
			else						stPktData.trig_set[i].area_trig_ontime	= 0;
			stPktData.trig_set[0].area_strob_ontime	= 2500;
#endif
		}
		bSucc = m_pSendQue->PushData(&stPktData);
		/* 동기 해제 */
		m_syncSend.Leave();
	}

	/* 현재 상태 값 읽기 요청 */
	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : 개별 채널 별로 트리거 1개만 발생
 parm : enc_out	- [in]  1 바이트 씩 의미가 있음 (4 Bytes 이므로, 총 4채널)
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ReqWriteTrigOutOne(UINT32 enc_out)
{
	BOOL bSucc			= FALSE;
	STG_TPQR stPktData	= {NULL};

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		/* 명령어 설정 */
		stPktData.command		= htonl((UINT32)ENG_TBPC::en_trig_out_one);
		stPktData.enc_out_val	= htonl(enc_out);
		bSucc = m_pSendQue->PushData(&stPktData);
		if (!bSucc)
		{
			LOG_ERROR(ENG_EDIC::en_trig, L"The send packet buffer is full");
		}
		/* 동기 해제 */
		m_syncSend.Leave();
	}

	/* 현재 상태 값 읽기 요청 */
	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : Reset Trigger Count
 parm : None
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ReqResetTrigCount()
{
	BOOL bSucc			= FALSE;
	STG_TPQR stPktData	= {NULL};

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		/* Encoder Counter Reset Command */
		stPktData.command		= (UINT32)ENG_TBPC::en_write;
		stPktData.enc_counter	= (INT32)ENG_TECM::en_clear;	/* 0x03 (Clear),  0x02 (Read) */
		/* Network Bytes Ordering */
		stPktData.SWAP();

		/* 송신될 데이터 버퍼에 등록 */
		bSucc = m_pSendQue->PushData(&stPktData);
		/* 동기 해제 */
		m_syncSend.Leave();
	}

	/* 현재 상태 값 읽기 요청 */
	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : 기존에 입력된 Trigger 위치 값과 Trigger Board로부터 수신된 입력 값 비교
 parm : cam_id	- [in]  Align Camera Index (0x01 or 0x02)
		index	- [in]  트리거 저장 위치 (0x000 ~ 0x0f)
		pos		- [in]  트리거 값 (비교하려는 입력된 트리거 위치 값)
 retn : TRUE or FALSE
*/
BOOL CTrigThread::IsTrigPosEqual(UINT8 cam_id, UINT8 index, UINT32 pos)
{
	/* 각 Align Camera에 대한 채널 번호 얻기 */
	UINT8 u8ChNo	= GetTrigChNo(cam_id);
	if (!u8ChNo)	return FALSE;
	u8ChNo--;

	return (m_pstShMemTrig->trig_set[u8ChNo].area_trig_pos[index] == pos);
}

/*
 desc : 연결 상태 정보 갱신
 parm : flag	- [in]  0x00: 연결 해제 상태, 0x01: 연결된 상태
 retn : None
*/
VOID CTrigThread::UpdateLinkTime(UINT8 flag)
{
	BOOL bUpate	= FALSE;

	/* 현재 연결 해제 상태이고, 이전에 연결된 상태라면 */
	if (!flag && m_pstShMemLink->is_connected)		bUpate	= TRUE;
	/* 현재 연결 상태이고, 이전에 해제된 상태라면 */
	else if (flag && !m_pstShMemLink->is_connected)	bUpate	= TRUE;

	/* 이전 상태와 다른 경우라면, 정보 변경 */
	if (bUpate)
	{
		SYSTEMTIME stLinkTime;
		GetLocalTime(&stLinkTime);
		m_pstShMemLink->is_connected	= flag;
		m_pstShMemLink->link_time[0]	= stLinkTime.wYear;
		m_pstShMemLink->link_time[1]	= stLinkTime.wMonth;
		m_pstShMemLink->link_time[2]	= stLinkTime.wDay;
		m_pstShMemLink->link_time[3]	= stLinkTime.wHour;
		m_pstShMemLink->link_time[4]	= stLinkTime.wMinute;
		m_pstShMemLink->link_time[5]	= stLinkTime.wSecond;
		m_pstShMemLink->last_sock_error	= GetSockLastError();
	}
}

/*
 desc : 송신될 명령어가 저장된 버퍼의 개수 반환
 parm : None
 retn : 버퍼 (큐)에 저장된 개수
*/
UINT16 CTrigThread::GetQueCount()
{
	return m_pSendQue->GetQueCount();
}