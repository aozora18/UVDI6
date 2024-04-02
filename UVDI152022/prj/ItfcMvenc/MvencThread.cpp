
/*
 desc : TCP/IP Client Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "MvencThread.h"


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
CMvencThread::CMvencThread(BYTE byPort, LPG_CTSP conf, LPG_TPQR shmem, LPG_DLSM link, LPG_CCGS comn)
{
	m_handle		= NULL;
	m_byPort		= byPort;
	/* ------------------------------------------- */
	/* RunWork 함수 본문을 바로 수행 하도록 처리함 */
	/* ------------------------------------------- */
	m_bInitRecv		= FALSE;
	m_u64SendTime	= GetTickCount64();
	
	m_pstConfTrig	= conf;
	m_pstShMemTrig	= shmem;
	m_pstShMemLink	= link;
	m_pstConfComn	= comn;

	/* 가장 최근 값 초기화 */
	UINT8 i, j;
	for (i=0; i<MAX_TRIG_CHANNEL; i++)
	{
		for (j=0; j<MAX_TRIG_CHANNEL; j++)
		{
			m_u32SendTrigPos[i][j]	= MAX_TRIG_POS;
		}
	}
	MvsEncSetPositiveRun(m_handle, 15);

#if (TRIG_RECV_OUTPUT)
	m_u64TraceTime	= 0;
#endif
}

/*
 desc : Destructor
 parm : None
*/
CMvencThread::~CMvencThread()
{
	//CloseHandle();
}

/*
 desc : 스레드 실행시 초기 한번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMvencThread::StartWork()
{
	return TRUE;
}

/*
 desc : 스레드 실행시 주기적 호출됨
 parm : None
 retn : None
*/
VOID CMvencThread::RunWork()
{
	BOOL bSendData	= FALSE;
	/* 현재 작업 시간 */
	UINT64 u64Tick	= GetTickCount64();

	if (NULL == m_handle)
	{
		m_handle = MvsEncCreateHandler(); //핸들러 생성
		char szPort[5] = { NULL };
		sprintf_s(szPort, _countof(szPort), "COM%d", m_byPort);
		MvsEncOpen(m_handle, szPort); //시리얼 포트 오픈
		if (MvsEncIsOpen(m_handle) != true) //오픈 체크
		{
			//연결실패
			m_handle = NULL;
			return;
		}

		// 초기값 설정
		for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
		{

			m_stTrigCtrl[i].EncoderType				= 2;	//0:DIFF, 1:TTL, 2:Virtual Encoder
			m_stTrigCtrl[i].EncoderCh				= i;	//Encoder CH (0~3)
			m_stTrigCtrl[i].Multi					= 4;	//Encoder Multi(1:x1, 2:x2, 4:x4)
			m_stTrigCtrl[i].CondFactor				= 7;	//0:DI0 ~ 6:DI6, 7:High, 8:Low
			m_stTrigCtrl[i].TriggerOutputOperator	= 0;	//0:AND, 1:OR, 2:XOR, 3:NAND
			m_stTrigCtrl[i].EncoderResetFactor		= 0;	//0:DISABLE, 1:DI0 ~ 7:DI6, 8:CW, 9:CCW
			m_stTrigCtrl[i].PositionDirection		= 1;	//0:DISABLE, 1:Positive, 2:Negative, 3:Bi-Direction
			m_stTrigCtrl[i].Temp = 0;
			m_stTrigCtrl[i].Correction				= 0;	//0:DISABLE, 1:ENABLE
			m_stTrigCtrl[i].TriggerBase				= 0;	//0:COUNT, 1:POSITION
			UINT uiStatus = MvsEncSetTriggerControls(m_handle, i, &m_stTrigCtrl[i]);//구조체 설정
			if (uiStatus != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetTriggerControls API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			}
		}

		m_u64SendTime = GetTickCount64();
		m_bInitRecv = TRUE;
	}

	/* ------------------------------------------------------------------ */
	/* 주기적으로 Trigger의 상태 값을 읽어오는 주기는 1000 msec 마다 진행 */
	/* ------------------------------------------------------------------ */
	if (u64Tick > (m_u64SendTime + 250))
	{
		bSendData	= TRUE;	/* Trigger 상태 값 요청 패킷 송신 */
		ReqReadSetup();
	}
}

/*
 desc : 스레드 종료시 한번 호출됨
 parm : None
 retn : None
*/
VOID CMvencThread::EndWork()
{
	if (NULL != m_handle)
	{
		CloseHandle();
	}
}


BOOL CMvencThread::IsConnected()
{
	if (NULL == m_handle)
	{
		return FALSE;
	}

	if (MvsEncIsOpen(m_handle))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CMvencThread::CloseHandle()
{
	MvsEncClose(m_handle);			//포트 닫기
	MvsEncCloseHandler(m_handle);	//핸들러 제거
	MvsEncRelease();				//라이브러리 종료

	m_handle = NULL;
}

/*
 desc : Camera Index (1 or 2)에 대한 채널 번호 값 반환
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
 retn : Camera Index에 해당되는 Trigger Board의 Channel ID (0x01 ~ 0x04) 값 반환
		0x00 값이면 실패 처리
*/
UINT8 CMvencThread::GetTrigChNo(UINT8 cam_id)
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
 desc : Trigger Board에 설정된 값 요청
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMvencThread::ReqReadSetup()
{
	UINT uiStatus		= 0;
	BOOL bSucc			= TRUE;
	STG_TPQR stPktData	= { NULL };
	/* 동기 진입 */
	//if (m_syncSend.Enter())
	//{
	//	/* Command */
	//	//UINT32				command;					/* Read/Write : Low 1 Byte, ENG_TBPC <01 : Write, 02 : Read, 07 : IP Write> */
	//	/* 현재 엔코더 값을 읽어 오거나 지울 수 있음 */
	//	//INT32				enc_counter;				/* ENG_TERR <Read : 4 Bytes, Write : Low 1 Byte, Encoder Counter Read / Reset> */
	//	/* Trigger, Strobe 출력 Enable or Disable */
	//	//UINT32				trig_strob_ctrl;			/* ENG_TSED <Read/Write : Low 1 Bytes, 4채널 동시 제어 <0x01 : Enable, 0x03 : Disable>> */
	//	/* Trigger Position, OnTime, Delay, Encoder Plus or Minus */
	//	//STG_TPTS			trig_set[MAX_TRIG_CHANNEL];	/* 4 채널 값 */
	//	/* Trigger Board의 IP Address */
	//	//UINT8				ip_addr[4];					/* IP 주소 값이 순서대로 1 Bytes 씩 들어감 */
	//	/* Encoder Output Value */
	//	//UINT32				enc_out_val;				/* Encoder 내부 트리거 발생 설정 */
	//	
	//	memcpy(&stPktData, m_pstShMemTrig, sizeof(STG_TPQR));
	//	for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
	//	{
	//		stPktData.command = UINT32(ENG_TBPC::en_read);
	//		uiStatus = MvsEncGetTriggerControls(m_handle, i, &m_stTrigCtrl[i]);//구조체 설정
	//		if (uiStatus != 0x00)
	//		{
	//			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
	//			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncGetTriggerControls API Error (status:%d)", uiStatus);
	//			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
	//			bSucc = FALSE;
	//		}

	//		uiStatus = MvsEncGetEncoderPosition(m_handle, i, &stPktData.enc_counter); //현재 위치를 가져옴
	//		if (uiStatus != 0x00)
	//		{
	//			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
	//			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncGetEncoderPosition API Error (status:%d)", uiStatus);
	//			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
	//			bSucc = FALSE;
	//		}

	//		//	area_trig_pos[MAX_REGIST_TRIG_POS_COUNT];	/* 사용되지 않는 채널의 값은 최대 값 설정 MAX_UINT32 (unit : 100 nm or 0.1 um) */
	//		//	area_trig_ontime;							/* High가 유지되는 값을 입력 (1 값 = 1 ns) */
	//		//	area_strob_ontime;							/* Hight가 유지되는 값을 입력 (1 값 = 1 ns) */
	//		//	area_strob_trig_delay;						/* 출력되는 Trigger의 Delay 값을 입력 (1 ns) */
	//		//	area_trig_pos_plus;							/* Area Trigger Position (1 ~ 16)의 값에 이 Register 값이 증가되어 Strobe와 Trigger 신호가 출력됨 */
	//		//												/* ex> area_trig_pos = 100이고, 이 Register 값이 10이면 Encoder의 110에서 Strobe와 Trigger가 출력됨 */
	//		//	area_trig_pos_minus;						/* Area Trigger Position (1 ~ 16)의 값에 이 Register 값이 감소되어 Strobe와 Trigger 신호가 출력됨 */
	//														/* ex> area_trig_pos = 100이고, 이 Register 값이 10이면 Encoder의 90에서 Strobe와 Trigger가 출력됨 */

	//		int		nTrigPosition = 0;
	//		UINT	nTrigCount = 0;
	//		uiStatus = MvsEncGetTriggerCount(m_handle, i, &nTrigCount);
	//		if (uiStatus != 0x00)
	//		{
	//			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
	//			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncGetTriggerCount API Error (status:%d)", uiStatus);
	//			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
	//			bSucc = FALSE;
	//		}

	//		for (int posID = 0; posID < MAX_REGIST_TRIG_POS_COUNT/*Count 만큼안 읽어올 경우 변경 필요*/; posID++)
	//		{
	//			uiStatus = MvsEncGetIndexTriggerPosition(m_handle, i, posID, &nTrigPosition);
	//			if (uiStatus != 0x00)
	//			{
	//				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
	//				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncGetIndexTriggerPosition API Error (status:%d)", uiStatus);
	//				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
	//				bSucc = FALSE;
	//			}
	//			stPktData.trig_set[i].area_trig_pos[posID] = nTrigPosition;
	//		}

	//		double	dPulseWidth = 0;
	//		uiStatus = MvsEncGetTriggerPulseWidth(m_handle, i, &dPulseWidth);
	//		if (uiStatus != 0x00)
	//		{
	//			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
	//			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncGetTriggerPulseWidth API Error (status:%d)", uiStatus);
	//			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
	//			bSucc = FALSE;
	//		}
	//		stPktData.trig_set[i].area_trig_ontime = UINT32(dPulseWidth * 1000.0);	// um
	//		stPktData.trig_set[i].area_strob_ontime = UINT32(dPulseWidth * 1000.0);	// um
	//		
	//		double	dDelay = 0;
	//		uiStatus = MvsEncGetTriggerDelay(m_handle, i, &dDelay);
	//		if (uiStatus != 0x00)
	//		{
	//			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
	//			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncGetTriggerDelay API Error (status:%d)", uiStatus);
	//			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
	//			bSucc = FALSE;
	//		}

	//		stPktData.trig_set[i].area_strob_trig_delay = UINT32(dDelay * 1000.0);	// um
	//	}

	//	// by sysandj : 문의필요 => stPktData.trig_strob_ctrl 
	//	// by sysandj : 문의필요 => stPktData.trig_set[i]area_trig_pos_plus 
	//	// by sysandj : 문의필요 => stPktData.trig_set[i]area_trig_pos_minus 
	//	// by sysandj : 문의필요 => stPktData.enc_out_val 

	//	if (bSucc)
	//	{
	//		memcpy(m_pstShMemTrig, &stPktData, sizeof(stPktData));
	//	}
	//	m_u64SendTime = GetTickCount64();
	//	/* 동기 해제 */
	//	m_syncSend.Leave();
	//}

	return bSucc;
}

/*
 desc : 특정한 경우에만 사용. (트리거보드의 트리거는 2개 밖에 발생안되기 때문에...)
 parm : None
 retn : TRUE or FALSE
 note : 트리거는 무조건 1, 3 번 카메라(램프;채널)만 발생하기 때문에...
		2, 4번 카메라 (램프;채널)로 발생시키기 위한 방법
*/
BOOL CMvencThread::ReqInitUpdate()
{
	BOOL	bSucc = TRUE;
	UINT	uiStatus = 0;
	UINT8	u8LampType	= m_pstConfComn->strobe_lamp_type;	/* 0x00: Coaxial, 0x01: Ring */
	UINT32 i = 0, j		= 0;

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		/* 기존 수신된 데이터 초기화 */
		m_pstShMemTrig->ResetPktData();

		/* 1th : Base Setup */
		uiStatus = MvsEncClearAll(m_handle); // 모든 카운트 초기화
		m_pstShMemTrig->command			= UINT32(ENG_TBPC::en_write);
		m_pstShMemTrig->enc_counter		= INT32(ENG_TECM::en_clear);
		m_pstShMemTrig->trig_strob_ctrl	= UINT32(ENG_TEED::en_disable);


	
		if (uiStatus != 0x00)
		{
			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncClearAll API Error (status:%d)", uiStatus);
			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			bSucc = FALSE;
		}
		m_u64SendTime = GetTickCount64();
		/* 동기 해제 */
		m_syncSend.Leave();
	}
	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : 기존 등록된 4개 채널에 대한 Trigger Position 값 초기화 (최대 값) 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMvencThread::ResetTrigPosAll()
{
	BOOL bSucc			= TRUE;
	UINT uiStatus		= 0;
	UINT32 i = 0, j		= 0;

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		MvsEncClearAll(m_handle);
		///* 초기 값 설정 */
		//for (; i<MAX_TRIG_CHANNEL; i++)
		//{
		//	/* 초기 트리거 발생 위치 값을 0인 아닌 MAX 값으로 설정 */
		//	for (j=0; j<MAX_REGIST_TRIG_POS_COUNT; j++)	m_pstShMemTrig->trig_set[i].area_trig_pos[j]	= MAX_TRIG_POS;
		//}
		///* 최근 트리거 위치 값 초기화 (반드시 패킷 버퍼에 넣는것보다 앞에 기입해야 함) */
		//memset(m_u32SendTrigPos, MAX_TRIG_POS, sizeof(UINT32)*MAX_TRIG_CHANNEL*MAX_REGIST_TRIG_POS_COUNT);

		//uiStatus				= MvsEncClearTriggerAll(m_handle); // 트리거출력 카운트 초기화
		//m_pstShMemTrig->command = (UINT32)ENG_TBPC::en_write;
		//if (uiStatus != 0x00)
		//{
		//	TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
		//	swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncClearTriggerAll API Error (status:%d)", uiStatus);
		//	LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
		//	bSucc = FALSE;
		//}
		m_u64SendTime = GetTickCount64();
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
BOOL CMvencThread::ReqEEPROMCtrl(UINT8 mode)
{
	BOOL bSucc			= TRUE;
	UINT uiStatus		= 0;
	UINT32 u32Cmd		= mode == 0x01 ? UINT32(ENG_TBPC::en_eeprom_read) : UINT32(ENG_TBPC::en_eeprom_write);

	/* 만약 EEPROM으로부터 읽기 모드인 경우, 기존 값 모두 지우기 */
	if (0x01 == mode)	m_pstShMemTrig->ResetPktData();

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		/* 초기에 EEPROM에 저장된 내용 가져오기 */
		uiStatus = MvsEncLoadFlash(m_handle); // 플래시메모리에서 설정값 가져오기
		m_pstShMemTrig->command = u32Cmd;
		if (uiStatus != 0x00)
		{
			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncLoadFlash API Error (status:%d)", uiStatus);
			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			bSucc = FALSE;
		}
		m_u64SendTime = GetTickCount64();
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
BOOL CMvencThread::ReqWriteBoardReset()
{
	BOOL bSucc			= TRUE;
	UINT uiStatus		= 0;

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		/* 명령어 설정 */
		uiStatus = MvsEncLoadDefaultParameters(m_handle); // 보드초기화
		m_pstShMemTrig->command = UINT32(ENG_TBPC::en_reset);
		if (uiStatus != 0x00)
		{
			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncLoadFlash API Error (status:%d)", uiStatus);
			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			bSucc = FALSE;
		}
		m_u64SendTime = GetTickCount64();
		/* 동기 해제 */
		m_syncSend.Leave();
	}

	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : Area Trigger Event 설정
 parm : direct	- [in]  TRUE: 정방향, FALSE: 역방향
		trig_set- [out] Trigger Event 정보 설정 후 반환
 retn : None
 note : Trigger Event를 받는 곳은 채널 1/3 번뿐이고, Strobe Event를 받는 곳은 채널 1/2/3/4 모두 받음
		채널 2/4번을 통해 이미지를 받으려면 채널 1/3번으로 Trigger 이벤트 발생 시켜야 됨
*/
VOID CMvencThread::SetTrigEvent(BOOL direct, LPG_TPTS trig_set)
{
	UINT8 u8LampType= m_pstConfComn->strobe_lamp_type;	/* 0x00: Coaxial, 0x01: Ring */
	UINT32 i = 0;

	/* 초기 값 설정 */
	for (i=0x00; i<0x04; i++)
	{
		trig_set[i].area_trig_ontime		= m_pstConfTrig->trig_on_time[i];
		trig_set[i].area_strob_ontime		= m_pstConfTrig->strob_on_time[i];
		trig_set[i].area_strob_trig_delay	= m_pstConfTrig->trig_delay_time[i];
		trig_set[i].area_trig_pos_plus		= 0/*m_pstConfTrig->trig_plus[i]*/;
		trig_set[i].area_trig_pos_minus		= 0/*m_pstConfTrig->trig_minus[i]*/;
	}
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
VOID CMvencThread::SetTrigPos(LPG_TPTS trig_set,
							 UINT8 start1, UINT8 count1, PINT32 pos1,
							 UINT8 start2, UINT8 count2, PINT32 pos2)
{

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
	UINT8 u8Cam1No[2] = { 1, 2 };
	UINT8 u8Cam2No[2] = { 0, 3 };

	/*Cam1 Cam2 포지션 설정*/
	memcpy(&trig_set[u8Cam1No[0]].area_trig_pos + start1, pos1, sizeof(UINT32) * count1);
	memcpy(&trig_set[u8Cam2No[0]].area_trig_pos + start2, pos2, sizeof(UINT32) * count2);
	/*조명1 조명2 포지션 설정*/
	memcpy(&trig_set[u8Cam1No[1]].area_trig_pos + start1, pos1, sizeof(UINT32) * count1);
	memcpy(&trig_set[u8Cam2No[1]].area_trig_pos + start2, pos2, sizeof(UINT32) * count2);

#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)

	/*Cam1 포지션 설정*/
	memcpy(&trig_set[0].area_trig_pos + start1, pos1, sizeof(UINT32) * count1);
	/*Cam2 포지션 설정*/
	memcpy(&trig_set[1].area_trig_pos + start2, pos2, sizeof(UINT32) * count2);
	/*공용 조명 설정*/

	vector<INT32> sorting;

	for (int i = 0; i < count1; i++)
		sorting.push_back(pos1[i]);
	
	for (int i=0; i < count2; i++)
		sorting.push_back(pos2[i]);
	
	std::sort(sorting.begin(), sorting.end(), [&](INT32& a, INT32& b) {return a < b; });
	
	memcpy(&trig_set[3].area_trig_pos + start1, sorting.data(), sizeof(UINT32) * (count1 + count2));
#endif
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
BOOL CMvencThread::ReqWriteAreaTrigPos(BOOL direct,
	UINT8 start1, UINT8 count1, PINT32 pos1,
	UINT8 start2, UINT8 count2, PINT32 pos2,
	ENG_TEED enable, BOOL clear)
{
	UINT8 i1 = 0, i2 = 0, u3MaxTrig = MAX_REGIST_TRIG_POS_COUNT;
	UINT8 u8ChNo[2] = { NULL };

	UINT8 u8Cam1No[2] = { 1, 2 };
	UINT8 u8Cam2No[2] = { 0, 3 };
	BOOL bSucc = TRUE;
	UINT uiStatus = 0;
	UINT uiStatus2 = 0;
	UINT uiStatus3 = 0;
	STG_TPQR stPktData = { NULL };

	/* 각 Align Camera에 대한 채널 번호 얻기 */
	//u8ChNo[0]	= GetTrigChNo(0x01);
//	u8ChNo[1]	= GetTrigChNo(0x02);

	if ((!(start1 >= 0 && start1 < MAX_REGIST_TRIG_POS_COUNT) || !pos1 ||
		(start1 + count1) > MAX_REGIST_TRIG_POS_COUNT) ||
		(!(start2 >= 0 && start2 < MAX_REGIST_TRIG_POS_COUNT) || !pos2 ||
			(start2 + count2) > MAX_REGIST_TRIG_POS_COUNT))
	{
		TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
		swprintf_s(tzMesg, LOG_MESG_SIZE,
			L"Trigger Invalid Parameter (start1:%d count1:%d start2:%d count2:%d)",
			start1, count1, start2, count2);
		LOG_ERROR(ENG_EDIC::en_mvenc, tzMesg);
		return FALSE;
	}
	//u8ChNo[0]--;
	//u8ChNo[1]--;

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		m_pstShMemTrig->command = (UINT32)ENG_TBPC::en_write;/* Write */
		m_pstShMemTrig->trig_strob_ctrl = (UINT32)enable;			 /* Trigger Enable or Disable */

#if 0	
		/*Cam1 Seting*/
		/*Mode 설정*/
		m_stTrigCtrl[u8Cam1No[0]].EncoderType = (UINT16)0;	//0:DIFF, 1:TTL, 2:Virtual Encoder
		m_stTrigCtrl[u8Cam1No[1]].EncoderType = (UINT16)0;	//0:DIFF, 1:TTL, 2:Virtual Encoder
		// by sysandj: Position 방향 설정
		m_stTrigCtrl[u8Cam1No[0]].PositionDirection = (UINT16)enable;	//0:DISABLE, 1:Positive, 2:Negative, 3:Bi-Direction
		m_stTrigCtrl[u8Cam1No[1]].PositionDirection = (UINT16)enable;	//0:DISABLE, 1:Positive, 2:Negative, 3:Bi-Direction
		/*채널 ch0으로 고정[MC2와 연결된 채널이라 나머지 트리거와 공유 하기 위해]*/
		m_stTrigCtrl[u8Cam1No[0]].EncoderCh = 0;						//Encoder CH (0~3)
		m_stTrigCtrl[u8Cam1No[1]].EncoderCh = 0;						//Encoder CH (0~3)
		/*트리거 보드 초기 설정값 변경*/
		uiStatus = MvsEncSetTriggerControls(m_handle, u8Cam1No[0], &m_stTrigCtrl[u8Cam1No[0]]);	//구조체 설정
		uiStatus2 = MvsEncSetTriggerControls(m_handle, u8Cam1No[1], &m_stTrigCtrl[u8Cam1No[1]]);	//구조체 설정
		if (uiStatus != 0x00 && uiStatus2 != 0x00)
		{
			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetTriggerControls API Error (status:%d)", uiStatus);
			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			bSucc = FALSE;
		}
		
		
		/*Cam2 Seting*/
		/*Mode 설정*/
		m_stTrigCtrl[u8Cam2No[0]].EncoderType = (UINT16)0;	//0:DIFF, 1:TTL, 2:Virtual Encoder
		m_stTrigCtrl[u8Cam2No[1]].EncoderType = (UINT16)0;	//0:DIFF, 1:TTL, 2:Virtual Encoder
		// by sysandj: Position 방향 설정
		m_stTrigCtrl[u8Cam2No[0]].PositionDirection = (UINT16)enable;	//0:DISABLE, 1:Positive, 2:Negative, 3:Bi-Direction
		m_stTrigCtrl[u8Cam2No[1]].PositionDirection = (UINT16)enable;	//0:DISABLE, 1:Positive, 2:Negative, 3:Bi-Direction
		/*채널 ch0으로 고정[MC2와 연결된 채널이라 나머지 트리거와 공유 하기 위해]*/
		m_stTrigCtrl[u8Cam2No[0]].EncoderCh = 0;						//Encoder CH (0~3)
		m_stTrigCtrl[u8Cam2No[1]].EncoderCh = 0;						//Encoder CH (0~3)
		/*트리거 보드 초기 설정값 변경*/
		uiStatus = MvsEncSetTriggerControls(m_handle, u8Cam2No[0], &m_stTrigCtrl[u8Cam2No[0]]);	//구조체 설정
		uiStatus2 = MvsEncSetTriggerControls(m_handle, u8Cam2No[1], &m_stTrigCtrl[u8Cam2No[1]]);//구조체 설정
		if (uiStatus != 0x00 && uiStatus2 != 0x00)
		{
			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetTriggerControls API Error (status:%d)", uiStatus);
			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			bSucc = FALSE;
		}
#endif
		SetPositionTrigMode(enable);

		/*Cam Seting End*/

		/* 기존 Area Trigger 값 모두 저장 */
		memcpy(stPktData.trig_set, m_pstShMemTrig->trig_set, sizeof(STG_TPTS) * MAX_TRIG_CHANNEL /* Max 4 channel */);

		/* 트리거 이벤트 설정 */
		SetTrigEvent(direct, stPktData.trig_set);
		/* 트리거 위치 설정 */
		SetTrigPos(stPktData.trig_set, start1, count1, pos1, start2, count2, pos2);
		/* 나머지 트리거 등록 위치는 최대 값 저장 */
		for (i1 = 0; i1 < start1; i1++)						stPktData.trig_set[u8ChNo[0]].area_trig_pos[i1] = MAX_TRIG_POS;
		for (i1 = start1 + count1; i1 < u3MaxTrig; i1++)	stPktData.trig_set[u8ChNo[0]].area_trig_pos[i1] = MAX_TRIG_POS;
		for (i2 = 0; i2 < start2; i2++)						stPktData.trig_set[u8ChNo[1]].area_trig_pos[i2] = MAX_TRIG_POS;
		for (i2 = start2 + count1; i2 < u3MaxTrig; i2++)	stPktData.trig_set[u8ChNo[1]].area_trig_pos[i2] = MAX_TRIG_POS;


		// Trigger Offset 보상
		UINT32				area_trig_pos_offset[MAX_TRIG_CHANNEL] = { 0 };
		if (ENG_TEED::en_positive == enable)
		{
			for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
			{
				area_trig_pos_offset[i] = stPktData.trig_set[i].area_trig_pos_plus;
			}
		}
		else if (ENG_TEED::en_negative == enable)
		{
			for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
			{
				area_trig_pos_offset[i] = stPktData.trig_set[i].area_trig_pos_minus;
			}
		}
		// Trigger Offset 보상

		for (int posID = 0; posID < MAX_REGIST_TRIG_POS_COUNT; posID++)
		{
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
			// Trigger Position count가 있다면 더 효율적으로 사용 가능
			/*Cam1 다수의 Trigger Position 설정*/
			uiStatus = MvsEncSetIndexTriggerPosition(m_handle, u8Cam1No[0], posID, stPktData.trig_set[u8Cam1No[0]].area_trig_pos[posID] + area_trig_pos_offset[u8Cam1No[0]]);
			uiStatus2 = MvsEncSetIndexTriggerPosition(m_handle, u8Cam1No[1], posID, stPktData.trig_set[u8Cam1No[1]].area_trig_pos[posID] + area_trig_pos_offset[u8Cam1No[1]]);

			if (uiStatus != 0x00 && uiStatus2 != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetIndexTriggerPosition API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
				bSucc = FALSE;
			}

			/*Cam2 다수의 Trigger Position 설정*/
			uiStatus = MvsEncSetIndexTriggerPosition(m_handle, u8Cam2No[0], posID, stPktData.trig_set[u8Cam2No[0]].area_trig_pos[posID] + area_trig_pos_offset[u8Cam2No[0]]);
			uiStatus2 = MvsEncSetIndexTriggerPosition(m_handle, u8Cam2No[1], posID, stPktData.trig_set[u8Cam2No[1]].area_trig_pos[posID] + area_trig_pos_offset[u8Cam2No[1]]);

			if (uiStatus != 0x00 && uiStatus2 != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetIndexTriggerPosition API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
				bSucc = FALSE;
			}
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
			// Trigger Position count가 있다면 더 효율적으로 사용 가능
			/*Cam1 다수의 Trigger Position 설정*/
			uiStatus = MvsEncSetIndexTriggerPosition(m_handle, 0, posID, stPktData.trig_set[0].area_trig_pos[posID] + area_trig_pos_offset[0]);
			/*Cam2 다수의 Trigger Position 설정*/
			uiStatus2 = MvsEncSetIndexTriggerPosition(m_handle, 1, posID, stPktData.trig_set[1].area_trig_pos[posID] + area_trig_pos_offset[1]);
			/*공용 조명 다수의 Trigger Position 설정*/
			uiStatus3 = MvsEncSetIndexTriggerPosition(m_handle, 3, posID, stPktData.trig_set[3].area_trig_pos[posID] + area_trig_pos_offset[3]);

			if (uiStatus != 0x00 && uiStatus2 != 0x00 && uiStatus3 != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetIndexTriggerPosition API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
				bSucc = FALSE;
			}

#endif

		}
		for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
		{
			/*트리거 사이클 Max값*/
			uiStatus = MvsEncSetTriggerGenerator(m_handle, i, MAX_TRIG_POS, MAX_TRIG_POS);
			if (uiStatus != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetTriggerGenerator API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
				bSucc = FALSE;
			}
		}
		/*노광 방향에 따라 각 트리거 발생 모드 동작 실행*/
		if (ENG_TEED::en_positive == enable)
		{
			MvsEncSetPositiveRun(m_handle, 15);
		}
		else if (ENG_TEED::en_negative == enable)
		{
			MvsEncSetNegativeRun(m_handle, 15);
		}


		/*Clear*/
		if (clear)
		{
			/*Position*/
			uiStatus = MvsEncClearEncoderPositionAll(m_handle);
			if (uiStatus != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncClearEncoderPositionAll API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
				bSucc = FALSE;
			}
			/*Trigger MvsEncClearTriggerCount*/
			uiStatus = MvsEncClearTriggerAll(m_handle);
			if (uiStatus != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncClearTriggerAll API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
				bSucc = FALSE;
			}
		}


		m_u64SendTime = GetTickCount64();
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
BOOL CMvencThread::ReqWriteAreaTrigPosCh(UINT8 cam_id, UINT8 start, UINT8 count, PINT32 pos,
										ENG_TEED enable, BOOL clear)
{
	UINT8 i =0 ,j		= 0, u8ChNo;
	BOOL bSucc			= TRUE;
	UINT uiStatus		= 0;
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
		LOG_ERROR(ENG_EDIC::en_mvenc, tzMesg);
		return FALSE;
	}
	u8ChNo--;

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		m_pstShMemTrig->command			= (UINT32)ENG_TBPC::en_write;									/* Write */
		/* Clear to encoder */
		if (clear)
		{
			uiStatus = MvsEncClearEncoderPositionAll(m_handle);
			if (uiStatus != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncClearEncoderPositionAll API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
				bSucc = FALSE;
			}
		}

		m_pstShMemTrig->trig_strob_ctrl	= (UINT32)enable;												/* Trigger Enable or Disable */
		// by sysandj: Position 방향 설정
		m_stTrigCtrl[u8ChNo].PositionDirection = (UINT16)enable;	//0:DISABLE, 1:Positive, 2:Negative, 3:Bi-Direction
		UINT uiStatus = MvsEncSetTriggerControls(m_handle, u8ChNo, &m_stTrigCtrl[u8ChNo]);//구조체 설정
		// by sysandj: Position 방향 설정

		/* 트리거 이벤트 설정 */
		SetTrigEvent(TRUE, stPktData.trig_set);
		/* 새로운 값을 덮어 씀 */
		memcpy(&stPktData.trig_set[u8ChNo].area_trig_pos+start, pos, sizeof(UINT32) * count);
		/* 가장 최근의 트리거 송신 위치 값 저장 */
		memcpy(&m_u32SendTrigPos[u8ChNo]+start, pos, sizeof(UINT32) * count);

		// Trigger Offset 보상
		UINT32				area_trig_pos_offset[MAX_TRIG_CHANNEL] = { 0 };
		if (ENG_TEED::en_positive == enable)
		{
			for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
			{
				area_trig_pos_offset[i] = stPktData.trig_set[i].area_trig_pos_plus;
			}
		}
		else if (ENG_TEED::en_negative == enable)
		{
			for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
			{
				area_trig_pos_offset[i] = stPktData.trig_set[i].area_trig_pos_minus;
			}
		}
		// Trigger Offset 보상
		
		for (int i = start; i < start+count; i++)
		{
			uiStatus = MvsEncSetIndexTriggerPosition(m_handle, u8ChNo, i, pos[i] + area_trig_pos_offset[u8ChNo]);
			if (uiStatus != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetIndexTriggerPosition API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
				bSucc = FALSE;
			}
		}
		m_u64SendTime = GetTickCount64();
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
BOOL CMvencThread::ReqWriteTriggerStrobe(BOOL enable)
{

	UINT8 i	= 0x00;
	BOOL bSucc			= TRUE;
	UINT uiStatus		= 0;
	STG_TPQR stPktData	= {NULL};

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
#if 1
		/* Encoder Counter Reset Command */
		//stPktData.command			= (UINT32)ENG_TBPC::en_write;
		//stPktData.trig_strob_ctrl	= enable ? (UINT32)ENG_TEED::en_enable : (UINT32)ENG_TEED::en_disable;

		m_pstShMemTrig->command			= (UINT32)ENG_TBPC::en_write;
		m_pstShMemTrig->trig_strob_ctrl	= enable ? (UINT32)ENG_TSED::en_enable : (UINT32)ENG_TSED::en_disable;


		for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
		{
			m_stTrigCtrl[i].PositionDirection = (UINT16)enable;	//0:DISABLE, 1:Positive, 2:Negative, 3:Bi-Direction
			//채널별 Trigger Controls Set
			uiStatus = MvsEncSetTriggerControls(m_handle, i, &m_stTrigCtrl[i]);//구조체 설정
			if (uiStatus != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetTriggerControls API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
				bSucc = FALSE;
			}
		}

		//현재위치 초기화
		uiStatus = MvsEncClearEncoderPositionAll(m_handle);
		if (uiStatus != 0x00)
		{
			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncClearEncoderPositionAll API Error (status:%d)", uiStatus);
			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			bSucc = FALSE;
		}

		/* 트리거 이벤트 설정 */
		//if (enable)	SetTrigEvent(TRUE, stPktData.trig_set);
		if (enable)	SetTrigEvent(TRUE, m_pstShMemTrig->trig_set);

		/* --------------------------------------------------------------------------------- */
		/* Triger & Strobe Enable 내지 Disable든 기존 등록된 Trigger Position 값 모두 초기화 */
		/* --------------------------------------------------------------------------------- */
		/* 최대 값 적용 */
		//stPktData.ResetTrigPos(0);
		//stPktData.ResetTrigPos(1);
		//stPktData.ResetTrigPos(2);
		//stPktData.ResetTrigPos(3);
		m_pstShMemTrig->ResetTrigPos(0);
		m_pstShMemTrig->ResetTrigPos(1);
		m_pstShMemTrig->ResetTrigPos(2);
		m_pstShMemTrig->ResetTrigPos(3);

		
		//트리거출력 카운트 초기화
		uiStatus = MvsEncClearTriggerAll(m_handle);
		if (uiStatus != 0x00)
		{
			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncClearTriggerAll API Error (status:%d)", uiStatus);
			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			bSucc = FALSE;
		}
#endif
		//for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
		//{
		//	MvsEncSetTriggerOutputMode(m_handle, i, 2);
		//	//Sleep(500);
		//
		//	//bSucc = MvsEncSetTriggerPosition0(m_handle, i, 0); //StartPosition 설정
		//	//Sleep(500);
		//	//bSucc = MvsEncSetTriggerPosition1(m_handle, i, 1000000); //StartPosition 설정
		//	//Sleep(500);
		//
		//	/*가상 트리거*/
		//	bSucc = MvsEncSetTriggerGenerator(m_handle, i, 0, 500);
		//	//int cycle, high;
		//	//MvsEncGetTriggerGenerator(m_handle, i, &cycle, &high);
		//	//Sleep(500);
		//	
		//	//bSucc = MvsEncSetVirtualEncoderFrequency(m_handle, i, 10000);
		//	//Sleep(500);
		//
		//	//bSucc = MvsEncSetPositiveRun(m_handle, i);
		//	//Sleep(500);
		//	//bSucc = MvsEncSetTrgMode(m_handle, i);
		//	//Sleep(500);
		//}
		//if (enable == TRUE)
		//{
		//	MvsEncSetPositiveRun(m_handle, 15);
		//}
		//else
		//{
		//	MvsEncSetPositiveRun(m_handle, 0);
		//}
		//
		//Sleep(500);
		//MvsEncClearAll(m_handle);
		//MvsEncClearTriggerAll(m_handle);

		MvsEncSetPositiveRun(m_handle, 0);

		m_u64SendTime = GetTickCount64();
		/* 동기 해제 */
		m_syncSend.Leave();
	}

	/* 현재 상태 값 읽기 요청 */
	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : 내부 트리거 설정
 parm : onoff	- [in]  1 바이트 씩 의미가 있음 (4 Bytes 이므로, 총 4채널)
						초당 10 frame 출력
 retn : TRUE or FALSE
*/
BOOL CMvencThread::ReqWriteEncoderOut(UINT32 enc_out)
{
	UINT8 i				= 0x00;
	BOOL bSucc			= TRUE;
	UINT uiStatus		= 0;
	STG_TPQR stPktData	= {NULL};

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		/* 명령어 설정 */
		stPktData.command		= (UINT32)ENG_TBPC::en_trig_internal;
		stPktData.enc_out_val	= enc_out;
		for (; i < MAX_TRIG_CHANNEL; i++)
		{
#if 0
			stPktData.trig_set[i].area_trig_ontime	= 2500;
			stPktData.trig_set[i].area_strob_ontime	= 2500;
#else	/* 현재 DI 장비 특성 때문에 (Trigger는 무조건 1 & 3 CH 에서만 발생 시켜야 됨) */
			if (i == 0x00 || i == 0x02)	stPktData.trig_set[i].area_trig_ontime	= 2500;
			else						stPktData.trig_set[i].area_trig_ontime	= 0;
			stPktData.trig_set[0].area_strob_ontime	= 2500;
#endif

			// 강제 트리거 발생 : 단위 확인 필요
			uiStatus = MvsEncSetTriggerGenerator(m_handle, i
				, stPktData.trig_set[i].area_trig_ontime
				, stPktData.trig_set[i].area_trig_ontime);

			if (uiStatus != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetTriggerGenerator API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
				bSucc = FALSE;
			}
		}
		m_u64SendTime = GetTickCount64();
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
BOOL CMvencThread::ReqWriteTrigOutOne(UINT32 enc_out)
{
	BOOL bSucc			= TRUE;
	UINT uiStatus		= 0;
	STG_TPQR stPktData	= {NULL};

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		/* 명령어 설정 */
		stPktData.command		= (UINT32)ENG_TBPC::en_trig_out_one;
		stPktData.enc_out_val	= enc_out;
		
		for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
		{
			// 강제 트리거 발생 : 단위 확인 필요
			uiStatus = MvsEncSetTriggerGenerator(m_handle, i
				, m_pstShMemTrig->trig_set[i].area_trig_ontime
				, m_pstShMemTrig->trig_set[i].area_trig_ontime);

			if (uiStatus != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetTriggerGenerator API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
				bSucc = FALSE;
			}
		}

#ifdef RUNTRIGGER
		MvsEncSetPositiveRun(m_handle, 15);
		Sleep(10);
		MvsEncSetPositiveRun(m_handle, 0);
#endif

		m_u64SendTime = GetTickCount64();
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
BOOL CMvencThread::ReqResetTrigCount()
{
	BOOL bSucc			= TRUE;
	UINT uiStatus		= 0;
	STG_TPQR stPktData	= {NULL};

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		/* Encoder Counter Reset Command */
		stPktData.command		= (UINT32)ENG_TBPC::en_write;
		stPktData.enc_counter	= (INT32)ENG_TECM::en_clear;	/* 0x03 (Clear),  0x02 (Read) */

		uiStatus = MvsEncClearEncoderPositionAll(m_handle);
		if (uiStatus != 0x00)
		{
			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncClearEncoderPositionAll API Error (status:%d)", uiStatus);
			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			bSucc = FALSE;
		}
		
		uiStatus = MvsEncClearTriggerAll(m_handle);

		if (uiStatus != 0x00)
		{
			TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
			swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetTriggerGenerator API Error (status:%d)", uiStatus);
			LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			bSucc = FALSE;
		}
		m_u64SendTime = GetTickCount64();
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
BOOL CMvencThread::IsTrigPosEqual(UINT8 cam_id, UINT8 index, UINT32 pos)
{
	/* 각 Align Camera에 대한 채널 번호 얻기 */
	UINT8 u8ChNo	= GetTrigChNo(cam_id);
	if (!u8ChNo)	return FALSE;
	u8ChNo--;

	return (m_pstShMemTrig->trig_set[u8ChNo].area_trig_pos[index] == pos);
}

/*
 desc : 카메라 별 가상 트리거 발생하여 Live 모드로 진행
 parm : cam_id	- [in]  Align Camera Index (0x01 or 0x02)
		enable	- [in]  TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE
*/
BOOL CMvencThread::ReqWirteTrigLive(UINT8 cam_id, BOOL enable)
{
	UINT8 i = 0x00;
	BOOL bSucc = TRUE;
	UINT uiStatus = 0;
	STG_TPQR stPktData = { NULL };

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		//for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
		//{
		//	m_stTrigCtrl[i].PositionDirection = (UINT16)enable;	//0:DISABLE, 1:Positive, 2:Negative, 3:Bi-Direction
		//	//채널별 Trigger Controls Set
		//	uiStatus = MvsEncSetTriggerControls(m_handle, i, &m_stTrigCtrl[i]);//구조체 설정
		//	if (uiStatus != 0x00)
		//	{
		//		TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
		//		swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetTriggerControls API Error (status:%d)", uiStatus);
		//		LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
		//		bSucc = FALSE;
		//	}
		//}
		//
		//for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
		//{
		//	MvsEncSetTriggerOutputMode(m_handle, i, 2);
		//	//Sleep(500);
		//
		//	//bSucc = MvsEncSetTriggerPosition0(m_handle, i, 0); //StartPosition 설정
		//	//Sleep(500);
		//	//bSucc = MvsEncSetTriggerPosition1(m_handle, i, 1000000); //StartPosition 설정
		//	//Sleep(500);
		//
		//	/*가상 트리거*/
		//	bSucc = MvsEncSetTriggerGenerator(m_handle, i, 0, 50);
		//	//int cycle, high;
		//	//MvsEncGetTriggerGenerator(m_handle, i, &cycle, &high);
		//	//Sleep(500);
		//
		//	//bSucc = MvsEncSetVirtualEncoderFrequency(m_handle, i, 10000);
		//	//Sleep(500);
		//
		//	//bSucc = MvsEncSetTrgMode(m_handle, i);
		//	//Sleep(500);
		//}
		SetLiveMode();

		if (enable == TRUE)
		{
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
			if (cam_id == 0x01)
			{
				//cam1 및 조명(0110) 트리거 실행
				MvsEncSetPositiveRun(m_handle, 6);
		}
			if (cam_id == 0x02)
			{
				//cam2 및 조명(1001) 트리거 실행
				MvsEncSetPositiveRun(m_handle, 9);
			}
			//MvsEncSetPositiveRun(m_handle, 15);
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
			if (cam_id == 0x01)
			{
				//cam1 및 조명(1001) 트리거 실행
				MvsEncSetPositiveRun(m_handle, 9);
			}
			if (cam_id == 0x02)
			{
				//cam2 및 조명(0101) 트리거 실행 => (1010) => 10
				MvsEncSetPositiveRun(m_handle, 10);
			}
			if (cam_id == 0x03)
			{
				//cam2 및 조명(0011) 트리거 실행 (1100)
				MvsEncSetPositiveRun(m_handle, 12);
			}
#endif

		}
		else
		{
			//실행 종료(0000)
			MvsEncSetPositiveRun(m_handle, 0);
		}


		Sleep(500);
		MvsEncClearAll(m_handle);
		MvsEncClearTriggerAll(m_handle);



		m_u64SendTime = GetTickCount64();
		/* 동기 해제 */
		m_syncSend.Leave();
	}


	return bSucc;
}



/*
 desc : 트리거 보드 Live Mode로 변경
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMvencThread::SetLiveMode()
{
	UINT8 i = 0x00;
	BOOL bSucc = TRUE;
	UINT uiStatus = 0;

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{

		// 초기값 설정
		for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
		{

			m_stTrigCtrl[i].EncoderType = 2;	//0:DIFF, 1:TTL, 2:Virtual Encoder
			m_stTrigCtrl[i].EncoderCh = i;		//Encoder CH (0~3)
			m_stTrigCtrl[i].Multi = 4;			//Encoder Multi(1:x1, 2:x2, 4:x4)
			m_stTrigCtrl[i].CondFactor = 7;		//0:DI0 ~ 6:DI6, 7:High, 8:Low
			m_stTrigCtrl[i].TriggerOutputOperator = 0;	//0:AND, 1:OR, 2:XOR, 3:NAND
			m_stTrigCtrl[i].EncoderResetFactor = 0;	//0:DISABLE, 1:DI0 ~ 7:DI6, 8:CW, 9:CCW
			m_stTrigCtrl[i].PositionDirection = 1;	//0:DISABLE, 1:Positive, 2:Negative, 3:Bi-Direction
			m_stTrigCtrl[i].Temp = 0;
			m_stTrigCtrl[i].Correction = 0;		//0:DISABLE, 1:ENABLE
			m_stTrigCtrl[i].TriggerBase = 0;	//0:COUNT, 1:POSITION
			uiStatus = MvsEncSetTriggerControls(m_handle, i, &m_stTrigCtrl[i]);//구조체 설정
			if (uiStatus != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetTriggerControls API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			}

			MvsEncSetTriggerPosition0(m_handle, i, 0);
			MvsEncSetTriggerPosition1(m_handle, i, 100000000);
			MvsEncSetTriggerGenerator(m_handle, i, 0, 1000);
			MvsEncSetTriggerDelay(m_handle, i, 0);

		}

		//MvsEncSetPositiveRun(m_handle, 15);
		MvsEncClearAll(m_handle);

		m_u64SendTime = GetTickCount64();
		/* 동기 해제 */
		m_syncSend.Leave();
	}
	return bSucc;
}


/*
 desc : 트리거 보드 Position Trigger 모드로 변경
 parm : enable	- [ENG_TEED]  트리거 Positive or Negative
 retn : TRUE or FALSE
*/
BOOL CMvencThread::SetPositionTrigMode(ENG_TEED enable)
{
	UINT8 i = 0x00;
	BOOL bSucc = TRUE;
	UINT uiStatus = 0;

	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		/*초기화*/
		MvsEncSetPositiveRun(m_handle, 0);

		// 초기값 설정
		for (int i = 0; i < MAX_TRIG_CHANNEL; i++)
		{

			m_stTrigCtrl[i].EncoderType = 0;	//0:DIFF, 1:TTL, 2:Virtual Encoder
			m_stTrigCtrl[i].EncoderCh = 0;		//Encoder CH (0~3)
			m_stTrigCtrl[i].Multi = 4;			//Encoder Multi(1:x1, 2:x2, 4:x4)
			m_stTrigCtrl[i].CondFactor = 7;		//0:DI0 ~ 6:DI6, 7:High, 8:Low
			m_stTrigCtrl[i].TriggerOutputOperator = 0;	//0:AND, 1:OR, 2:XOR, 3:NAND
			m_stTrigCtrl[i].EncoderResetFactor = 0;	//0:DISABLE, 1:DI0 ~ 7:DI6, 8:CW, 9:CCW

			m_stTrigCtrl[i].Temp = 0;
			m_stTrigCtrl[i].Correction = 0;		//0:DISABLE, 1:ENABLE
			m_stTrigCtrl[i].TriggerBase = 0;	//0:COUNT, 1:POSITION

			if (ENG_TEED::en_positive == enable)
			{
				m_stTrigCtrl[i].PositionDirection = 1;	//0:DISABLE, 1:Positive, 2:Negative, 3:Bi-Direction
			}
			else if (ENG_TEED::en_negative == enable)
			{
				m_stTrigCtrl[i].PositionDirection = 2;	//0:DISABLE, 1:Positive, 2:Negative, 3:Bi-Direction
			}
			uiStatus = MvsEncSetTriggerControls(m_handle, i, &m_stTrigCtrl[i]);//구조체 설정
			if (uiStatus != 0x00)
			{
				TCHAR tzMesg[LOG_MESG_SIZE] = { NULL };
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"MvsEncSetTriggerControls API Error (status:%d)", uiStatus);
				LOG_WARN(ENG_EDIC::en_mvenc, tzMesg);
			}

			MvsEncSetTriggerGenerator(m_handle, i, MAX_TRIG_POS, MAX_TRIG_POS);

			if (ENG_TEED::en_positive == enable)
			{
				MvsEncSetTriggerPosition0(m_handle, i, 100);
				MvsEncSetTriggerPosition1(m_handle, i, 0);
			}
			else if (ENG_TEED::en_negative == enable)
			{
				MvsEncSetTriggerPosition0(m_handle, i, 0);
				MvsEncSetTriggerPosition1(m_handle, i, -100);

			}

			MvsEncSetTriggerDelay(m_handle, 0, 10);
			MvsEncSetTriggerDelay(m_handle, 1, 10);
			MvsEncSetTriggerDelay(m_handle, 2, 0);
			MvsEncSetTriggerDelay(m_handle, 3, 0);

			MvsEncSetTriggerPulseWidth(m_handle, i, 200);

		}

		//if (ENG_TEED::en_positive == enable)
		//{
		//	//MvsEncSetPositiveRun(m_handle, 15);
		//	MvsEncSetPositiveRun(m_handle, 6);
		//	MvsEncSetPositiveRun(m_handle, 9);
		//}
		//else if (ENG_TEED::en_negative == enable)
		//{
		//	//MvsEncSetNegativeRun(m_handle, 15);
		//	MvsEncSetNegativeRun(m_handle, 6);
		//	MvsEncSetNegativeRun(m_handle, 9);
		//}
		//MvsEncClearAll(m_handle);

		m_u64SendTime = GetTickCount64();
		/* 동기 해제 */
		m_syncSend.Leave();
	}
	return bSucc;
}