
/*
 desc : PreAligner Library for Logosol with serial
*/

#include "pch.h"
#include "MainApp.h"
#include "LithoThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/* ------------------------------------------------------------------------------------ */
/*                                      전역 변수                                       */
/* ------------------------------------------------------------------------------------ */

/* 아래 두 전역 변수는 동일한 메모리 영역을 가리킴 */
PUINT8					m_pMemMap		= NULL;	/* PLC 메모리 (D 영역) */
LPG_PMDV				m_pstMemMap		= NULL;	/* PLC 메모리 (D 영역) */
CLithoThread			*g_pLithoThread	= NULL;


/* ------------------------------------------------------------------------------------ */
/*                                      내부 함수                                       */
/* ------------------------------------------------------------------------------------ */

/*
 desc : PLC 기본 주소를 WORD 주소와 BIT 위치 값으로 분리 후 반환
 parm : addr	- [in]  PLC I/O 주소
		w_th	- [out] Word 주소가 저장될 참조 변수
		b_th	- [out] Word 주소의 비트 위치 (0 ~ F)가 저장될 참조 변수
 retn : None
*/
VOID GetAddrWordBits(ENG_PIOA addr, UINT32 &w_th, UINT8 &b_th)
{
#if 1
	UINT32 u32Addr	= UINT32(addr);

	/* WORD 주소 내 Bit 위치 값 (0 ~ F) */
	b_th	= (u32Addr & 0x0000000F);
	/* WORD 주소 값 */
	w_th	= (u32Addr >> 4);
#else
	UINT16 u16Addr	= UINT16(addr);

	/* WORD 주소 내 Bit 위치 값 (0 ~ F) */
	b_th = (u16Addr & 0x000F);
	/* WORD 주소 값 */
	w_th = (u16Addr >> 4);
#endif
}

/*
 desc : 특정 시작 주소의 메모리 데이터 값 [ 2 Bytes 값 ] 반환
 parm : addr	- [in]  비트 정보가 저장된 주소 (BIT Index 값. 실제 메모리 상의 주소 값)
						Zero based 기준. 0 번지부터 시작 (100번지 요청인 경우, 101번째 메모리 값)
		value	- [out] 반환되어 저장될 참조 값
 retn : TRUE or FALSE
*/
BOOL GetWordValue(UINT32 addr, UINT16 &value)
{
	PUINT16 pMemMap	= (PUINT16)m_pMemMap;	/* 2 Bytes (WORD)로 포인터 연결 */

	/* bits 위치가 0x0f보다 큰 값인지 그리고 주소 값이 유효한지 확인 */
	if (g_pstConfig->melsec_q_svc.start_d_addr > addr)	return FALSE;
	/* 입력된 주소 값이 큰 경우라면 */
	if ((addr - g_pstConfig->melsec_q_svc.start_d_addr) >= g_pstConfig->melsec_q_svc.addr_d_count)	return FALSE;

	/* 먼저 WORD 단위의 주소 값 얻기 */
	value	= pMemMap[addr - g_pstConfig->melsec_q_svc.start_d_addr];

	return TRUE;
}

/*
 desc : 입력된 주소 값이 유효한지 여부
 parm : addr	- [in]  주소 값
 retn : TRUE or FALSE
*/
BOOL IsValidAddr(UINT32 addr)
{
	LPG_CPSI pstPLC = &g_pstConfig->melsec_q_svc;

	if ((pstPLC->start_d_addr > addr) ||
		(pstPLC->start_d_addr + pstPLC->addr_d_count) < addr)
	{
		TCHAR tzMesg[64]	= {NULL};
		swprintf_s(tzMesg, 64, L"PLC Address [%08x] is incorrect", addr);
		LOG_ERROR(ENG_EDIC::en_litho, tzMesg);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : MC Protocol - 특정 위치의 Bit 데이터 쓰기
 parm : addr	- [in]  디바이스 시작 주소 값 (4 Bytes이지만, 3 Bytes 값만 유효)
		value	- [in]  Bit 1개 값이 저장된 버퍼
 retn : TRUE or FALSE (에러 코드가 저장됨 ?)
*/
BOOL MCQ_WriteBits(ENG_PIOA addr, UINT8 value)
{
	UINT32 u32Addr	= 0;
	UINT16 u16Value	= 0;
	UINT8 u8AddrT;

	/* PLC Driver 연결 여부 */
	if (!uvMCQDrv_IsConnected())	return FALSE;

	/* 주소와 비트 위치 분리 */
	GetAddrWordBits(addr, u32Addr, u8AddrT);

	/* 입력된 주소 값이 유효한지 여부 확인 */
	if (!IsValidAddr(u32Addr))	return FALSE;
	/* 해당 메모리 영역의 1 Word 값 */
	if (!GetWordValue(UINT32(addr), u16Value))	return FALSE;

	/* 특정 위치의 비트 값 변경 */
	if (value)	u16Value	|= (1 << u8AddrT);	/* 특정 위치의 비트 값을 1 로 변경 */
	else		u16Value	&= ~(1 << u8AddrT);	/* 특정 위치의 비트 값을 0 로 변경 */

	return uvMCQDrv_WriteWord(ENG_DNTC::en_d, u32Addr, 0x0001, &u16Value);
}

/* ------------------------------------------------------------------------------------ */
/*                                      외부 함수                                       */
/* ------------------------------------------------------------------------------------ */

#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : Linked Litho 초기화
 parm : config	- [in]  환경 설정 정보
		shmem	- [in]  PLC D Memory 영역을 가리키는 포인터
 retn : None
*/
API_EXPORT BOOL uvLitho_Init(LPG_CIEA config, PUINT8 shmem)
{
	g_pstConfig	= config;
	/* 아래 2개의 변수는 동일한 메모리 영역을 가리킴 */
	m_pMemMap	= shmem;
	m_pstMemMap	= LPG_PMDV(shmem);

	/* PIO 값 감시 스레드 생성 */
	g_pLithoThread	= new CLithoThread(m_pstMemMap);
	ASSERT(g_pLithoThread);
	if (!g_pLithoThread->CreateThread(NULL, NULL, MEDIUM_THREAD_SPEED))
	{
		delete g_pLithoThread;
		g_pLithoThread	= NULL;
		return FALSE;
	}

	return TRUE;
}

/*
 desc : Linked Litho Library 해제
 parm : None
 retn : None
*/
API_EXPORT VOID uvLitho_Close()
{
	/* PIO 값 감시 스레드 해제 */
	if (g_pLithoThread)
	{
		if (g_pLithoThread->IsBusy())		g_pLithoThread->Stop();
		while (g_pLithoThread->IsBusy())	Sleep(10);
		delete g_pLithoThread;
		g_pLithoThread	= NULL;
	}
	/* 반드시 해줘야 됨 */
	g_pstConfig	= NULL;
}

/*
 desc : 현재 Track 장비 내의 상태 값 반환
 parm : None
 retn : Track 장비 내의 상태 정보가 저장된 구조체 포인터
*/
API_EXPORT LPG_ODET uvLitho_GetStateTrack()
{
	return &m_pstMemMap->out_expo_track.s_track;
}

/*
 desc : 현재 Scanner 장비 내의 상태 값 반환
 parm : None
 retn : Track 장비 내의 상태 정보가 저장된 구조체 포인터
*/
API_EXPORT LPG_IDTE uvLitho_GetStateScanner()
{
	return &m_pstMemMap->in_track_expo.s_scan;
}

/*
 desc : PIO History 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvLitho_ResetHistPIO()
{
	g_pLithoThread->ResetHistPIO();
}

/*
 desc : 특정 위치의 PIO 상태 값 반환 (가장 최근부터 이전에 발생된 상태 값 가져오기)
 parm : count	- [in]  최소 1 값 (가장 최근에 발생된 순으로 값 가져오기 위한 개수 값)
		state	- [out] PIO 상태 값 저장 ('count' 개수만큼 저장될 구조체 버퍼 포인터)
 retn : TRUE or FALSE (버퍼가 비어 있거나, 해당 위치 (Index)에 값이 없거나 ...)
*/
API_EXPORT BOOL uvLitho_GetHistPIO(UINT8 count, LPG_STSE state)
{
	return g_pLithoThread->GetHistPIO(count, state);
}

/*
 desc : EXP-INL (Scanner Inline) 값 제어 (ON or OFF)
 parm : signal	- [in]  ON (0x01) or OFF (0x00)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvLitho_SetScanInline(bool signal)
{
	/* 현재 설정 값하고 입력하고자 하는 값이 동일한 값이면 바로 FALSE 리턴 */
	if (signal == m_pstMemMap->out_expo_track.s_track.exp_ink_exposure_inline)
	{
		LOG_WARN(ENG_EDIC::en_litho, L"The old signal and the change signal are the same [EXP_INL]");
		return FALSE;
	}
	/* 값 설정 (데이터 전송) */
	return MCQ_WriteBits(ENG_PIOA::en_exp_ink_exposure_inline, signal);
}

/*
 desc : EXP-RDY (Scanner Received to Ready) 값 제어 (ON or OFF)
 parm : signal	- [in]  ON (0x01) or OFF (0x00)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvLitho_SetScanRecvToReady(bool signal)
{
	/* 현재 설정 값하고 입력하고자 하는 값이 동일한 값이면 바로 FALSE 리턴 */
	if (signal == m_pstMemMap->out_expo_track.s_track.exp_rdy_exposure_receive_to_ready)
	{
		LOG_WARN(ENG_EDIC::en_litho, L"The old signal and the change signal are the same [EXP_RDY]");
		return FALSE;
	}
	/* 값 설정 (데이터 전송) */
	return MCQ_WriteBits(ENG_PIOA::en_exp_rdy_exposure_receive_to_ready, signal);
}

/*
 desc : EXP_SND (Scanner Send to Ready) 값 제어 (ON or OFF)
 parm : signal	- [in]  ON (0x01) or OFF (0x00)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvLitho_SetScanSendToReady(bool signal)
{
	/* 현재 설정 값하고 입력하고자 하는 값이 동일한 값이면 바로 FALSE 리턴 */
	if (signal == m_pstMemMap->out_expo_track.s_track.exp_snd_exposure_send_to_ready)
	{
		LOG_WARN(ENG_EDIC::en_litho, L"The old signal and the change signal are the same [EXP_SND]");
		return FALSE;
	}
	/* 값 설정 (데이터 전송) */
	return MCQ_WriteBits(ENG_PIOA::en_exp_snd_exposure_send_to_ready, signal);
}

/*
 desc : EXP-REJ (Scanner Reject) 값 제어 (ON or OFF)
 parm : signal	- [in]  ON (0x01) or OFF (0x00)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvLitho_SetScanReject(bool signal)
{
	/* 현재 설정 값하고 입력하고자 하는 값이 동일한 값이면 바로 FALSE 리턴 */
	if (signal == m_pstMemMap->out_expo_track.s_track.exp_rej_exposure_reject)
	{
		LOG_WARN(ENG_EDIC::en_litho, L"The old signal and the change signal are the same [EXP_REJ]");
		return FALSE;
	}
	/* 값 설정 (데이터 전송) */
	return MCQ_WriteBits(ENG_PIOA::en_exp_rej_exposure_reject, signal);
}

/*
 desc : Track 에서 Scanner로 Wafer가 이송 중인지 여부 (Transporting from Track To Scanner)
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvLitho_IsMovingWaferFromTrackToScanner()
{
	STG_STSE stState[3]	= {NULL};

	/* 가장 최근에 발생된 특정 개수 가져옴. 0 번째가 가장 최근에 발생된 값 (현재 값) */
	if (!g_pLithoThread->GetHistPIO(0x03, stState))	return FALSE;
	/* Index - 1 th */
	if (0x00 == stState[0].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x01 == stState[0].out_expo_track.s_track.exp_rdy_exposure_receive_to_ready)	return FALSE;
	if (0x00 == stState[0].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x00 == stState[0].in_track_expo.s_scan.trk_snd_track_send &&
		0x00 == stState[0].in_track_expo.s_scan.trk_lend_track_end_of_lot)				return FALSE;
	/* Index - 2 th */
	if (0x00 == stState[1].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x00 == stState[1].out_expo_track.s_track.exp_rdy_exposure_receive_to_ready)	return FALSE;
	if (0x00 == stState[1].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x00 == stState[1].in_track_expo.s_scan.trk_snd_track_send &&
		0x00 == stState[1].in_track_expo.s_scan.trk_lend_track_end_of_lot)				return FALSE;
	/* Index - 3 th */
	if (0x00 == stState[2].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x00 == stState[2].out_expo_track.s_track.exp_rdy_exposure_receive_to_ready)	return FALSE;
	if (0x00 == stState[2].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x01 == stState[2].in_track_expo.s_scan.trk_snd_track_send ||
		0x01 == stState[2].in_track_expo.s_scan.trk_lend_track_end_of_lot)				return FALSE;

	return TRUE;
}

/*
 desc : 현재 Track에서 Scanner로 Wafer를 이송 가능한지 여부
		다른 말로 표현하면, Scanner에서 Wafer를 받을 준비가 되어 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvLitho_IsMoveWaferFromTrackToScanner()
{
	/* 가장 최근 상태 값에서 판단 */
	LPG_IDTE pstTrackExpo	= &m_pstMemMap->in_track_expo.s_scan;
	LPG_ODET pstExpoTrack	= &m_pstMemMap->out_expo_track.s_track;

	/* On the scanner side */
	if (0x00 == pstExpoTrack->exp_ink_exposure_inline)				return FALSE;
	if (0x01 == pstExpoTrack->exp_rdy_exposure_receive_to_ready)	return FALSE;

	/* On the track side */
	if (0x00 == pstTrackExpo->trk_ink_track_inline)					return FALSE;
	if (0x01 == pstTrackExpo->trk_snd_track_send ||
		0x01 == pstTrackExpo->trk_lend_track_end_of_lot)			return FALSE;

	return TRUE;
}

/*
 desc : 현재 Track에서 Scanner로 Wafer (in Lot)가 이동이 완료되었는지 여부
 parm : None
 retn : TRUE or FALSE (이동이 완료되지 않은 상태... 즉, 다른 상태 임)
*/
API_EXPORT BOOL uvLitho_IsMovedWaferFromTrackToScanner()
{
	STG_STSE stState[4]	= {NULL};

	/* 가장 최근에 발생된 PIO 상태 4개 가져오기 (0 번째가 가장 최근. 현재 값임) */
	if (!g_pLithoThread->GetHistPIO(0x04, stState))		return FALSE;

	/* Index - 1 th */
	if (0x00 == stState[0].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x01 == stState[0].out_expo_track.s_track.exp_rdy_exposure_receive_to_ready)	return FALSE;
	if (0x00 == stState[0].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x01 == stState[0].in_track_expo.s_scan.trk_snd_track_send ||
		0x01 == stState[0].in_track_expo.s_scan.trk_lend_track_end_of_lot)				return FALSE;
	/* Index - 2 th */
	if (0x00 == stState[1].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x01 == stState[1].out_expo_track.s_track.exp_rdy_exposure_receive_to_ready)	return FALSE;
	if (0x00 == stState[1].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x00 == stState[1].in_track_expo.s_scan.trk_snd_track_send &&
		0x00 == stState[1].in_track_expo.s_scan.trk_lend_track_end_of_lot)				return FALSE;
	/* Index - 3 th */
	if (0x00 == stState[2].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x00 == stState[2].out_expo_track.s_track.exp_rdy_exposure_receive_to_ready)	return FALSE;
	if (0x00 == stState[2].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x00 == stState[2].in_track_expo.s_scan.trk_snd_track_send &&
		0x00 == stState[2].in_track_expo.s_scan.trk_lend_track_end_of_lot)				return FALSE;
	/* Index - 4 th */
	if (0x00 == stState[3].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x00 == stState[3].out_expo_track.s_track.exp_rdy_exposure_receive_to_ready)	return FALSE;
	if (0x00 == stState[3].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x01 == stState[3].in_track_expo.s_scan.trk_snd_track_send ||
		0x01 == stState[3].in_track_expo.s_scan.trk_lend_track_end_of_lot)				return FALSE;

	return TRUE;
}

/*
 desc : 현재 Track에서 Wafer 이송 동작 작업을 멈추게 할 수 있는지 여부
		즉, Wafer 이송 동작을 멈추게 하는 명령 (제어)이 가능한지 여부
		현재 Track에서 Wafer가 이송 중일 때는 멈추게 할 수 없기 때문임
 parm : None
 retn : TRUE or FALSE
 note : 현재 Track의 EFEM 이 동작 중이라는 가정 (설정)임
*/
API_EXPORT BOOL uvLitho_IsMoveStopWaferOnTrack()
{
	/* 가장 최근 상태 값에서 판단 */
	LPG_IDTE pstTrackExpo	= &m_pstMemMap->in_track_expo.s_scan;
	LPG_ODET pstExpoTrack	= &m_pstMemMap->out_expo_track.s_track;


	/* On the scanner side */
	if (0x00 == pstExpoTrack->exp_ink_exposure_inline)				return FALSE;
	if (0x00 == pstExpoTrack->exp_rdy_exposure_receive_to_ready)	return FALSE;

	/* On the track side */
	if (0x00 == pstTrackExpo->trk_snd_track_send &&
		0x00 == pstTrackExpo->trk_lend_track_end_of_lot)			return FALSE;

	return TRUE;
}

/*
 desc : 현재 Lot Wafer의 마지막 Wafer 이동이 Track에서 Scanner로 이동이 취소 되었는지 여부
 parm : None
 retn : TRUE or FALSE
 참고 ; End of Lot Wafer Aborted
*/
API_EXPORT BOOL uvLitho_IsAbortedEndOfLotWaferFromTrackToScanner()
{
	STG_STSE stState[4]	= {NULL};

	/* 가장 최근에 발생된 PIO 상태 4개 가져오기 (0 번째가 가장 최근. 현재 값임) */
	if (!g_pLithoThread->GetHistPIO(0x04, stState))		return FALSE;

	/* Index - 1 th */
	if (0x00 == stState[0].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x01 == stState[0].out_expo_track.s_track.exp_rdy_exposure_receive_to_ready)	return FALSE;
	if (0x00 == stState[0].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x01 == stState[0].in_track_expo.s_scan.trk_snd_track_send)						return FALSE;
	if (0x01 == stState[0].in_track_expo.s_scan.trk_lend_track_end_of_lot)				return FALSE;
	/* Index - 2 th */
	if (0x00 == stState[1].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x01 == stState[1].out_expo_track.s_track.exp_rdy_exposure_receive_to_ready)	return FALSE;
	if (0x00 == stState[1].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x01 == stState[1].in_track_expo.s_scan.trk_snd_track_send)						return FALSE;
	if (0x00 == stState[1].in_track_expo.s_scan.trk_lend_track_end_of_lot)				return FALSE;
	/* Index - 3 th */
	if (0x00 == stState[2].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x00 == stState[2].out_expo_track.s_track.exp_rdy_exposure_receive_to_ready)	return FALSE;
	if (0x00 == stState[2].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x01 == stState[2].in_track_expo.s_scan.trk_snd_track_send)						return FALSE;
	if (0x00 == stState[2].in_track_expo.s_scan.trk_lend_track_end_of_lot)				return FALSE;
	/* Index - 4 th */
	if (0x00 == stState[3].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x00 == stState[3].out_expo_track.s_track.exp_rdy_exposure_receive_to_ready)	return FALSE;
	if (0x00 == stState[3].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x01 == stState[3].in_track_expo.s_scan.trk_snd_track_send)						return FALSE;
	if (0x01 == stState[3].in_track_expo.s_scan.trk_lend_track_end_of_lot)				return FALSE;

	return TRUE;
}

/*
 desc : Scanner 에서 Track로 Wafer가 이송 중인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvLitho_IsMovingWaferFromScannerToTrack()
{
	STG_STSE stState[2]	= {NULL};

	/* 가장 최근에 발생된 특정 개수 가져옴. 0 번째가 가장 최근에 발생된 값 (현재 값) */
	if (!g_pLithoThread->GetHistPIO(0x02, stState))		return FALSE;
	/* Index - 1 th */
	if (0x00 == stState[0].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x00 == stState[0].out_expo_track.s_track.exp_snd_exposure_send_to_ready)		return FALSE;
	if (0x01 == stState[0].out_expo_track.s_track.exp_rej_exposure_reject)				return FALSE;
	if (0x00 == stState[0].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x01 == stState[0].in_track_expo.s_scan.trk_rdy_track_ready)					return FALSE;
	/* Index - 2 th */
	if (0x00 == stState[1].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x00 == stState[1].out_expo_track.s_track.exp_rdy_exposure_receive_to_ready)	return FALSE;
	if (0x01 == stState[1].out_expo_track.s_track.exp_rej_exposure_reject)				return FALSE;
	if (0x00 == stState[1].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x00 == stState[1].in_track_expo.s_scan.trk_rdy_track_ready)					return FALSE;

	return TRUE;
}

/*
 desc : Scanner 에서 Track로 Wafer가 이송 완료 되었는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvLitho_IsMovedWaferFromScannerToTrack()
{
	STG_STSE stState[3]	= {NULL};

	/* 가장 최근에 발생된 특정 개수 가져옴. 0 번째가 가장 최근에 발생된 값 (현재 값) */
	if (!g_pLithoThread->GetHistPIO(0x03, stState))		return FALSE;
	/* Index - 1 th */
	if (0x00 == stState[0].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x01 == stState[0].out_expo_track.s_track.exp_snd_exposure_send_to_ready)		return FALSE;
	if (0x01 == stState[0].out_expo_track.s_track.exp_rej_exposure_reject)				return FALSE;
	if (0x00 == stState[0].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x01 == stState[0].in_track_expo.s_scan.trk_rdy_track_ready)					return FALSE;
	/* Index - 2 th */
	if (0x00 == stState[1].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x00 == stState[1].out_expo_track.s_track.exp_rdy_exposure_receive_to_ready)	return FALSE;
	if (0x01 == stState[1].out_expo_track.s_track.exp_rej_exposure_reject)				return FALSE;
	if (0x00 == stState[1].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x01 == stState[1].in_track_expo.s_scan.trk_rdy_track_ready)					return FALSE;
	/* Index - 3 th */
	if (0x00 == stState[2].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x00 == stState[2].out_expo_track.s_track.exp_rdy_exposure_receive_to_ready)	return FALSE;
	if (0x01 == stState[2].out_expo_track.s_track.exp_rej_exposure_reject)				return FALSE;
	if (0x00 == stState[2].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x00 == stState[2].in_track_expo.s_scan.trk_rdy_track_ready)					return FALSE;

	return TRUE;
}

/*
 desc : Scanner 에서 Track로 Wafer가 이송 거부 중인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvLitho_IsRejectingWaferFromScannerToTrack()
{
	STG_STSE stState[2]	= {NULL};

	/* 가장 최근에 발생된 특정 개수 가져옴. 0 번째가 가장 최근에 발생된 값 (현재 값) */
	if (!g_pLithoThread->GetHistPIO(0x02, stState))		return FALSE;
	/* Index - 1 th */
	if (0x00 == stState[0].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x00 == stState[0].out_expo_track.s_track.exp_snd_exposure_send_to_ready)		return FALSE;
	if (0x00 == stState[0].out_expo_track.s_track.exp_rej_exposure_reject)				return FALSE;
	if (0x00 == stState[0].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x01 == stState[0].in_track_expo.s_scan.trk_rdy_track_ready)					return FALSE;
	/* Index - 2 th */
	if (0x00 == stState[1].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x00 == stState[1].out_expo_track.s_track.exp_rdy_exposure_receive_to_ready)	return FALSE;
	if (0x00 == stState[1].out_expo_track.s_track.exp_rej_exposure_reject)				return FALSE;
	if (0x00 == stState[1].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x00 == stState[1].in_track_expo.s_scan.trk_rdy_track_ready)					return FALSE;

	return TRUE;
}

/*
 desc : Scanner 에서 Track로 Wafer가 이송 거부 완료 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvLitho_IsRejectedWaferFromScannerToTrack()
{
	STG_STSE stState[3]	= {NULL};

	/* 가장 최근에 발생된 특정 개수 가져옴. 0 번째가 가장 최근에 발생된 값 (현재 값) */
	if (!g_pLithoThread->GetHistPIO(0x03, stState))		return FALSE;
	/* Index - 1 th */
	if (0x00 == stState[0].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x01 == stState[0].out_expo_track.s_track.exp_snd_exposure_send_to_ready)		return FALSE;
	if (0x01 == stState[0].out_expo_track.s_track.exp_rej_exposure_reject)				return FALSE;
	if (0x00 == stState[0].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x01 == stState[0].in_track_expo.s_scan.trk_rdy_track_ready)					return FALSE;
	/* Index - 2 th */
	if (0x00 == stState[1].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x00 == stState[1].out_expo_track.s_track.exp_snd_exposure_send_to_ready)		return FALSE;
	if (0x00 == stState[1].out_expo_track.s_track.exp_rej_exposure_reject)				return FALSE;
	if (0x00 == stState[1].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x01 == stState[1].in_track_expo.s_scan.trk_rdy_track_ready)					return FALSE;
	/* Index - 3 th */
	if (0x00 == stState[2].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x00 == stState[2].out_expo_track.s_track.exp_rdy_exposure_receive_to_ready)	return FALSE;
	if (0x00 == stState[2].out_expo_track.s_track.exp_rej_exposure_reject)				return FALSE;
	if (0x00 == stState[2].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x00 == stState[2].in_track_expo.s_scan.trk_rdy_track_ready)					return FALSE;

	return TRUE;
}

/*
 desc : Scanner 에서 Wafer가 제거 (삭제) 중인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvLitho_IsDeletingWaferInScanner()
{
	STG_STSE stState[2]	= {NULL};

	/* 가장 최근에 발생된 특정 개수 가져옴. 0 번째가 가장 최근에 발생된 값 (현재 값) */
	if (!g_pLithoThread->GetHistPIO(0x02, stState))		return FALSE;
	/* Index - 1 th */
	if (0x00 == stState[0].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x01 == stState[0].out_expo_track.s_track.exp_snd_exposure_send_to_ready)		return FALSE;
	if (0x00 == stState[0].out_expo_track.s_track.exp_rej_exposure_reject)				return FALSE;
	if (0x00 == stState[0].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x01 == stState[0].in_track_expo.s_scan.trk_rdy_track_ready)					return FALSE;
	/* Index - 2 th */
	if (0x00 == stState[1].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x01 == stState[1].out_expo_track.s_track.exp_rdy_exposure_receive_to_ready)	return FALSE;
	if (0x00 == stState[1].out_expo_track.s_track.exp_rej_exposure_reject)				return FALSE;
	if (0x00 == stState[1].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x00 == stState[1].in_track_expo.s_scan.trk_rdy_track_ready)					return FALSE;

	return TRUE;
}

/*
 desc : Scanner 에서 Wafer가 제거 (삭제) 중인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvLitho_IsDeletedWaferInScanner()
{
	STG_STSE stState[3]	= {NULL};

	/* 가장 최근에 발생된 특정 개수 가져옴. 0 번째가 가장 최근에 발생된 값 (현재 값) */
	if (!g_pLithoThread->GetHistPIO(0x03, stState))		return FALSE;
	/* Index - 1 th */
	if (0x00 == stState[0].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x01 == stState[0].out_expo_track.s_track.exp_snd_exposure_send_to_ready)		return FALSE;
	if (0x01 == stState[0].out_expo_track.s_track.exp_rej_exposure_reject)				return FALSE;
	if (0x00 == stState[0].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x01 == stState[0].in_track_expo.s_scan.trk_rdy_track_ready)					return FALSE;
	/* Index - 2 th */
	if (0x00 == stState[1].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x01 == stState[1].out_expo_track.s_track.exp_snd_exposure_send_to_ready)		return FALSE;
	if (0x00 == stState[1].out_expo_track.s_track.exp_rej_exposure_reject)				return FALSE;
	if (0x00 == stState[1].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x01 == stState[1].in_track_expo.s_scan.trk_rdy_track_ready)					return FALSE;
	/* Index - 3 th */
	if (0x00 == stState[2].out_expo_track.s_track.exp_ink_exposure_inline)				return FALSE;
	if (0x01 == stState[2].out_expo_track.s_track.exp_rdy_exposure_receive_to_ready)	return FALSE;
	if (0x00 == stState[2].out_expo_track.s_track.exp_rej_exposure_reject)				return FALSE;
	if (0x00 == stState[2].in_track_expo.s_scan.trk_ink_track_inline)					return FALSE;
	if (0x00 == stState[2].in_track_expo.s_scan.trk_rdy_track_ready)					return FALSE;

	return TRUE;
}


#ifdef __cplusplus
}
#endif