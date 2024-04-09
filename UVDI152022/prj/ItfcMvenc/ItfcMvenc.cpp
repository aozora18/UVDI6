
/*
 desc : Trigger Board Communication Libarary (Trig Protocol)
*/

#include "pch.h"
#include "MainApp.h"
#include "MvencThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                           전역 변수                                           */
/* --------------------------------------------------------------------------------------------- */

CMvencThread				*g_pMvencThread		= NULL;		// 데이터 수집 및 감시 스레드

/* 공유 메모리 */
LPG_TPQR					g_pstShMemTrig		= NULL;
LPG_DLSM					g_pstShMemLink		= NULL;
STG_CTCS					g_stCommInfo		= {NULL};

/* --------------------------------------------------------------------------------------------- */
/*                                           내부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 현재 PLC가 연결되어 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
BOOL IsConnected()
{
	if (!g_pMvencThread)				return FALSE;

	return (g_pMvencThread->IsInitRecvData() && g_pMvencThread->IsConnected());
}

/*
 desc : Trigger
 parm : cam_id		- [in]  Align Camera Index (1 or 2)
		lamp_type	- [in]  램프 조명 타입 (0:Ring or 1:Coaxial)
 retn : TRUE or FALSE
 note : Camera 1 : 1 CH (Lamp: Coxial Type), 2 CH (Lamp: Ring Type)
		Camera 2 : 3 CH (Lamp: Coxial Type), 4 CH (Lamp: Ring Type)
*/
//UINT32 GetTrigEncOutVal(UINT8 cam_id)
UINT32 GetTrigEncOutVal(UINT8 cam_id, UINT8 lamp_type)
{
	BOOL bSucc		= FALSE;
	UINT32 u32EncOut= 0x00000000;

	if (!IsConnected())	return FALSE;

	//switch (g_pstConfig->set_comn.strobe_lamp_type)
	switch (lamp_type)
	{
	case 0x00	:	/* 조명 1 (Lamp: Coaxial) */
		switch (cam_id)
		{
		case 0x01	:	u32EncOut = 0x01000000;	break;	/* Trigger Board: 1 CH Enable */
		case 0x02	:	u32EncOut = 0x00000100;	break;	/* Trigger Board: 3 CH Enable */
		}
		break;
	case 0x01	:	/* 조명 2 (Lamp: Ring) */
		/* Ring Lamp의 경우, 1 & 3 CH (Camera) 쪽에 강제로 Trigger 발생시키기 위함 */
		switch (cam_id)
		{
		case 0x01	:	u32EncOut = 0x01010000;	break;	/* Trigger Board: 2 CH Enable */
		case 0x02	:	u32EncOut = 0x00000101;	break;	/* Trigger Board: 4 CH Enable */
		}
		break;
	}

	return u32EncOut;
}


/* --------------------------------------------------------------------------------------------- */
/*                                           외부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/*
 desc : PLC Server와 통신을 담당하는 Client 스레드 생성 및 실행
 parm : config	- [in]  환경 설정 정보
		shmem	- [in]  Trigger Service 관련 공유 메모리
		link	- [in]  통신 연결 공유 메모리
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_Init(BYTE byPort, LPG_CIEA config, LPG_TPQR shmem, LPG_DLSM link)
{
	LPG_CTSP pstTrig= NULL;

	/* Luria Shared Memory */
	g_pstShMemTrig	= shmem;
	g_pstShMemLink	= link;
	/* All Configuration */
	g_pstConfig		= config;

	// Client 스레드 생성 및 동작 시킴
	pstTrig	= &config->trig_grab;
	g_pMvencThread = new CMvencThread(byPort, pstTrig, shmem, link, &config->set_comn);
	if (!g_pMvencThread)	return FALSE;
	if (!g_pMvencThread->CreateThread(0, 0, MEDIUM_THREAD_SPEED))
	{
		delete g_pMvencThread;
		g_pMvencThread	= NULL;
		return FALSE;
	}

	return TRUE;
}

/*
 desc : PLC Server와 통신을 담당하는 Client 스레드 생성 및 실행
 parm : config	- [in]  환경 설정 정보
		shmem	- [in]  Trigger Service 관련 공유 메모리
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_Open(LPG_CIEA config, LPG_TPQR shmem)
{
	LPG_CTSP pstTrig= NULL;

	/* Luria Shared Memory */
	g_pstShMemTrig	= shmem;
	/* All Configuration */
	g_pstConfig		= config;

	return TRUE;
}

/*
 desc : 라이브러리 해제
 parm : None
 retn : None
*/
API_EXPORT VOID uvMvenc_Close()
{
	// 스레드 정지 시키는 명령 전달 후에 정지될 때까지 대기
	if (g_pMvencThread)
	{
		/* Trigger Board - Disabled */
		g_pMvencThread->ReqWriteEncoderOut(0x00000000);
		g_pMvencThread->ReqWriteTriggerStrobe(FALSE);
		/* 약 데이터 송신될 때까지 대기 */
		Sleep(500);
		// 스레드 중지 호출
		g_pMvencThread->Stop();
		while (g_pMvencThread->IsBusy())	g_pMvencThread->Sleep(100);
		//g_pMvencThread->CloseHandle();
		delete g_pMvencThread;
		g_pMvencThread	= NULL;
	}
	/* 해주면 안됨 */
//	g_pstConfig	= NULL;
}

/*
 desc : 시스템 재접속 수행
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_Reconnected(BYTE byPort)
{
	LPG_CTSP pstTrig= NULL;

	uvMvenc_Close();
	pstTrig	= &g_pstConfig->trig_grab;

	return uvMvenc_Init(byPort, g_pstConfig, g_pstShMemTrig, g_pstShMemLink);
}

/*
 desc : 현재 PLC가 연결되어 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_IsConnected()
{
	return IsConnected();
}

/*
 desc : 초기 연결되고 난 이후 수신된 데이터가 존재하는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_IsInitRecv()
{
	return !g_pMvencThread ? FALSE : g_pMvencThread->IsInitRecvData();
}
#if 1
/*
 desc : Trigger Board에 처음 연결되고 난 이후, 환경 파일에 설정된 값으로 초기화 진행
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_ReqInitUpdate()
{
	return !g_pMvencThread ? FALSE : g_pMvencThread->ReqInitUpdate();
}
#endif
/*
 desc : 기존 등록된 4개 채널에 대한 Trigger Position 값 초기화 (최대 값) 수행
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_ResetTrigPosAll()
{
	if (!IsConnected())	return FALSE;
	return g_pMvencThread->ResetTrigPosAll();
}

/*
 desc : 현재 트리거의 위치 등록 값이 초기화 되었는지 여부
 parm : cam_id	- [in]  Align Camera Index (0x01 or 0x02)
		count	- [in]  채널 번호에 해당되는 검사할 트리거 등록 개수 (MAX: 16)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_IsTrigPosReset(UINT8 cam_id, UINT8 count)
{
	UINT8 i	= 0;
	UINT8 u8ChNo	= g_pMvencThread->GetTrigChNo(cam_id);
	if (!u8ChNo)	return FALSE;

	for (; i<count; i++)
	{
		if (g_pstShMemTrig->trig_set[u8ChNo-1].area_trig_pos[i] != MAX_TRIG_POS)	return FALSE;
	}

	return TRUE;
}

/*
 desc : 채널 별로 트리거 위치 등록
 parm : direct	- [in]  TRUE: 정방향 (전진), FALSE: 역방향 (후진)
		start	- [in]  트리거 저장 위치 (0 ~ 15)
		count	- [in]  트리거 등록 개수 (1 ~ 16)
		pos		- [in]  트리거 등록 위치가 저장되어 있는 배열 포인터 (unit : 100 nm or 0.1 um)
		enable	- [in]  트리거 Enable or Disable
		clear	- [in]  트리거 위치 등록하면서 현재 Trigger Encoder 값을
						Clear (TRUE)할것인지 혹은 읽기 모드 (FALSE)를 유지할 것인지 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_ReqWriteAreaTrigPos(BOOL direct,
										   UINT8 start1, UINT8 count1, PINT32 pos1,
										   UINT8 start2, UINT8 count2, PINT32 pos2,
										   ENG_TEED enable, BOOL clear)
{
	if (!IsConnected())	return FALSE;
	return g_pMvencThread->ReqWriteAreaTrigPos(direct,
											  start1, count1, pos1,
											  start2, count2, pos2,
											  enable, clear);
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
API_EXPORT BOOL uvMvenc_ReqWriteAreaTrigPosCh(UINT8 cam_id, UINT8 start, UINT8 count, PINT32 pos,
											 ENG_TEED enable, BOOL clear)
{
	if (!IsConnected())	return FALSE;
	return g_pMvencThread->ReqWriteAreaTrigPosCh(cam_id, start, count, pos, enable, clear);
}

/*
 desc : Trigger & Strobe Enable or Disable
 parm : enable	- [in]  TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_ReqTriggerStrobe(BOOL enable)
{
	if (!IsConnected())	return FALSE;
	return g_pMvencThread->ReqWriteTriggerStrobe(enable);
}

/*
 사용하지 않음
 desc : IP 변경하기
 parm : ip_addr	- [in]  패킷 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_ReqIP4Addr(PUINT8 ip_addr)
{
	if (!IsConnected())	return FALSE;
	return TRUE;
}

/*
 desc : EEPROM Write
 parm : mode	- [in]  0x01 - Read, 0x02 - Write
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_ReqEEPROMCtrl(UINT8 mode)
{
	if (!IsConnected())	return FALSE;
	return g_pMvencThread->ReqEEPROMCtrl(mode);
}

/*
 desc : Board S/W Reset
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_ReqBoardReset()
{
	if (!IsConnected())	return FALSE;
	return g_pMvencThread->ReqWriteBoardReset();
}

/*
 desc : 내부 트리거 설정
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
		lamp_type	- [in]  램프 조명 타입 (0:Ring or 1:Coaxial)
 retn : TRUE or FALSE
 note : Camera 1 : 1 CH (Lamp: Coxial Type), 2 CH (Lamp: Ring Type)
		Camera 2 : 3 CH (Lamp: Coxial Type), 4 CH (Lamp: Ring Type)
*/
API_EXPORT BOOL uvMvenc_ReqEncoderLive(UINT8 cam_id, UINT8 lamp_type)
{
	BOOL bSucc		= FALSE;
	UINT32 u32EncOut= GetTrigEncOutVal(cam_id, lamp_type);

	if (!IsConnected())	return FALSE;
#if 0
	switch (g_pstConfig->set_comn.strobe_lamp_type)
	{
	case 0x00	:	/* 조명 1 (Lamp: Coaxial) */
		switch (cam_id)
		{
		case 0x01	:	u32OnOff = 0x01000000;	break;	/* Trigger Board: 1 CH Enable */
		case 0x02	:	u32OnOff = 0x00000100;	break;	/* Trigger Board: 3 CH Enable */
		}
		break;
	case 0x01	:	/* 조명 2 (Lamp: Ring) */
		switch (cam_id)
		{
		case 0x01	:	u32OnOff = 0x00010000;	break;	/* Trigger Board: 2 CH Enable */
		case 0x02	:	u32OnOff = 0x00000001;	break;	/* Trigger Board: 4 CH Enable */
		}
		break;
	}
#endif
	return g_pMvencThread->ReqWriteEncoderOut(u32EncOut);
}

/*
 desc : 내부 트리거 설정 초기화
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_ReqEncoderOutReset()
{
	if (!IsConnected())	return FALSE;
	return g_pMvencThread->ReqWriteEncoderOut(0x00000000);
}

/*
 desc : 트리거 1개 발생 시킴 (Only Trigger Event)
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
		lamp_type	- [in]  램프 조명 타입 (0:Ring or 1:Coaxial)
 retn : TRUE or FALSE
 note : Camera 1 : 1 CH (Lamp: Coxial Type), 2 CH (Lamp: Ring Type)
		Camera 2 : 3 CH (Lamp: Coxial Type), 4 CH (Lamp: Ring Type)
*/
API_EXPORT BOOL uvMvenc_ReqTrigOutOneOnly(UINT8 cam_id, UINT8 lamp_type)
{
	BOOL bSucc	= FALSE;
	UINT32 u32EncOut= GetTrigEncOutVal(cam_id, lamp_type);

	if (!IsConnected())	return FALSE;
#if 0
	switch (g_pstConfig->set_comn.strobe_lamp_type)
	{
	case 0x00	:	/* 조명 1 (채널 번호가 아님) */
		if (0x01 == ch_no)	bSucc = g_pMvencThread->ReqWriteTrigOutOne(0x01000000);
		else				bSucc = g_pMvencThread->ReqWriteTrigOutOne(0x00000100);	break;
	case 0x01	:	/* 조명 2 (채널 번호가 아님) */
		if (0x01 == ch_no)	bSucc = g_pMvencThread->ReqWriteTrigOutOne(0x00010000);
		else				bSucc = g_pMvencThread->ReqWriteTrigOutOne(0x00000001);	break;
	}
#endif
	//return bSucc = g_pMvencThread->ReqWriteTrigOutOne(u32EncOut);
	return bSucc = g_pMvencThread->ReqWriteTrigOutOne(cam_id);
}

/*
 desc : 트리거 1개 발생 시킴 (Trigger Enable -> Trigger Event -> Trigger Disable)
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
		lamp_type	- [in]  램프 조명 타입 (0:Ring or 1:Coaxial)
		enable	- [in]  트리거 내보낸 이후 Disable 시킬지 여부
						트리거 1개 발생 후, 곧바로 트리거 Disable 할지 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_ReqTrigOutOne(UINT8 cam_id, UINT8 lamp_type, BOOL enabl)
{
	BOOL bSucc	= FALSE;
	UINT32 u32EncOut= GetTrigEncOutVal(cam_id, lamp_type);

	if (!IsConnected())	return FALSE;
	/* Trigger & Strobe : Enabled */
	//bSucc	= uvMvenc_ReqTriggerStrobe(TRUE);
#if 0
	/* Trigger Out : one pulse */
	if (bSucc)
	{
		switch (g_pstConfig->set_comn.strobe_lamp_type)
		{
		case 0x00	:	/* 조명 1 (채널 번호가 아님) */
			if (0x01 == cam_id)	bSucc = g_pMvencThread->ReqWriteTrigOutOne(0x01000000);
			else				bSucc = g_pMvencThread->ReqWriteTrigOutOne(0x00000100);	break;
		case 0x01	:	/* 조명 2 (채널 번호가 아님) */
			if (0x01 == cam_id)	bSucc = g_pMvencThread->ReqWriteTrigOutOne(0x00010000);
			else				bSucc = g_pMvencThread->ReqWriteTrigOutOne(0x00000001);	break;
		}
	}
#else
	//bSucc = g_pMvencThread->ReqWriteTrigOutOne(u32EncOut);
//	bSucc = g_pMvencThread->ReqWriteTrigOutOne(cam_id);
	bSucc = g_pMvencThread->ReqWriteTrigOutOne_(0b1000 | (1 << (cam_id-1)));

#endif
	/* Trigger & Strobe : Disabled */
	//if (bSucc && enabl)	bSucc = uvMvenc_ReqTriggerStrobe(FALSE);

	return bSucc;
}

API_EXPORT BOOL uvMvenc_ReqTrigOutOne_(UINT8 channelBit)
{
	if (!IsConnected())	return FALSE;
	BOOL bSucc = g_pMvencThread->ReqWriteTrigOutOne_(channelBit);
	return bSucc;
}



/*
 desc : 기존에 입력된 Trigger 위치 값과 Trigger Board로부터 수신된 입력 값 비교
 parm : ch_no	- [in]  채널 번호 (0x01 ~ 0x04)
		index	- [in]  트리거 저장 위치 (0x000 ~ 0x0f)
		pos		- [in]  트리거 값
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMvenc_IsTrigPosEqual(UINT8 ch_no, UINT8 index, UINT32 pos)
{
	if (!IsConnected())	return FALSE;
	return g_pMvencThread->IsTrigPosEqual(ch_no, index, pos);
}

/*
 desc : 카메라 별 가상 트리거 발생하여 Live 모드로 진행
 parm : cam_id	- [in]  Align Camera Index (0x01 or 0x02)
		enable	- [in]  TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE

*/
API_EXPORT BOOL uvMvenc_ReqWirteTrigLive(UINT8 cam_id, BOOL enable)
{
	if (!IsConnected())	return FALSE;
	return g_pMvencThread->ReqWirteTrigLive(cam_id, enable);
}

/*
 사용하지 않음
 desc : 현재 트리거에 송신될 명령 버퍼의 대기 개수가 임의 개수 이하인지 여부
 parm : count	- [in]  이 개수 미만이면 TRUE, 이상이면 FALSE
 retn : count 개수 미만 (미포함)이면 TRUE, 이상 (포함)이면 FALSE 반환
*/
API_EXPORT BOOL uvMvenc_IsSendCmdCountUnder(UINT16 count)
{
	return TRUE;
}

#ifdef __cplusplus
}
#endif