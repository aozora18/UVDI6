
/*
 desc : PreAligner Library for Logosol with serial
*/

#include "pch.h"
#include "MainApp.h"

#include "MPAThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

/* ------------------------------------------------------------------------------------------------
 단위 : T 축 단위		- 1 = 0.1°, 전체 회전하려면 3600 값 입력 (시계 방향 : +, 반시계 방향 : -)
		R 축 단위		- 1 = 0.0001 inch = 2.54 um = 2540 nm
		Z 축 단위		- 1 = 0.0001 inch = 2.54 um = 2540 nm
 범위 : T 축 회전 각도	- -210.00° ~ +210.00°
		R1/R2 축 거리	- -14.400" ~ +14.400" (-36.576 cm ~ +36.576 cm)
		Z 축 거리		- - 0.500" ~ +12.200" (-1.27 cm ~ +30.988 cm)
------------------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------ */
/*                                      전역 변수                                       */
/* ------------------------------------------------------------------------------------ */

LPG_MPDV					g_pstShMemMPA		= NULL;
CMPAThread					*g_pMPAThread		= NULL;		/* 작업 스레드 */


/* ------------------------------------------------------------------------------------ */
/*                                      내부 함수                                       */
/* ------------------------------------------------------------------------------------ */

/*
 desc : 각 축(R, Z, T)의 코드 값을 1 바이트의 문자 값으로 반환
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
 retn : 문자 1 개 즉, T, R, Z 중 1 개 값
 */
CHAR GetAxisChar(ENG_PANC axis)
{
	CHAR szAxis[4] = { 'T', 'R', 'Z', 'A' };

	if (UINT8(axis) > 0x03)	return 'T';	/* 잘못된 Drive 축 값이 오게 되면 무조건 'T' 문자 반환 */

	return szAxis[UINT8(axis)];
}

/*
 desc : 각 축(R, Z, T) 중 입력된 값을 장비에 설정되는 값으로 변환
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		value	- [in]  User Input Value (Axis에 따라 단위도 다르고, 계산식도 다름) (unit : um or degree)
 retn : 값 반환 (실수형 um or degree --> 정수형 inch or degree)
 */
UINT32 GetAxisPosDev(ENG_PANC axis, DOUBLE value)
{
	DOUBLE dbValue	= 0.0f;

	/* 입력되는 값에 따라 단위 변환이 다름 */
	switch (axis)
	{
	case ENG_PANC::pac_t	:	dbValue = value / 0.1;	break;	/* 1 --> 0.1 degrees */
	case ENG_PANC::pac_r	:
	case ENG_PANC::pac_z	:	dbValue = value / 2.54;	break;	/* 1 --> 0.0001 inch */
	}

	return (UINT32)ROUNDED(dbValue, 0);
}

/*
 desc : 가장 최근에 수신된 데이터가 있는지 여부
 parm : cmd		- [in]  검사 대상 명령어
		axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
 retn : TRUE or FALSE
*/
BOOL IsRecvStatusInfoCmd(ENG_PSCC cmd)
{
	UINT8 i	= 0x00;

	switch (cmd)
	{
	case ENG_PSCC::psc_mcpo	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	if (g_pstShMemMPA->axis_pos[i] == DBL_MAX)	return FALSE;	break;
	case ENG_PSCC::psc_diag	: 
	case ENG_PSCC::psc_ver	:
	case ENG_PSCC::psc_est	: return (strlen(g_pstShMemMPA->last_string_data) > 0);
	case ENG_PSCC::psc_per	: return (g_pstShMemMPA->previous_cmd_fail != UINT16_MAX);
	case ENG_PSCC::psc_inf	: return (g_pstShMemMPA->info_mode != UINT8_MAX);
	case ENG_PSCC::psc_rhs	: return (g_pstShMemMPA->axis_switch_active != 0xfeee);
	case ENG_PSCC::psc_sta	: return (g_pstShMemMPA->system_status != UINT16_MAX);
	default					: return FALSE;
	}
	return TRUE;
}
BOOL IsRecvAxisParamSetRetrieveCmd(ENG_PSCC cmd)
{
	UINT8 i	= 0x00;
	switch (cmd)
	{
	case ENG_PSCC::psc__ch	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	if (g_pstShMemMPA->homed_pos[i] == DBL_MAX)			return FALSE;	break;
	case ENG_PSCC::psc__cl	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	if (g_pstShMemMPA->current_limit[i] == UINT8_MAX)	return FALSE;	break;
	case ENG_PSCC::psc__el	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	if (g_pstShMemMPA->error_limit[i] == UINT16_MAX)	return FALSE;	break;
	case ENG_PSCC::psc__fl	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	if (g_pstShMemMPA->direct_f_limit[i] == DBL_MAX)	return FALSE;	break;
	case ENG_PSCC::psc__ho	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	if (g_pstShMemMPA->home_offset[i] == DBL_MAX)		return FALSE;	break;
	case ENG_PSCC::psc__rl	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	if (g_pstShMemMPA->direct_r_limit[i] == DBL_MAX)	return FALSE;	break;
	case ENG_PSCC::psc_acl	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	if (g_pstShMemMPA->axis_acl[i] == DBL_MAX)			return FALSE;	break;
	case ENG_PSCC::psc_ajk	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	if (g_pstShMemMPA->axis_ajk[i] == DBL_MAX)			return FALSE;	break;
	case ENG_PSCC::psc_dcl	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	if (g_pstShMemMPA->axis_dcl[i] == DBL_MAX)			return FALSE;	break;
	case ENG_PSCC::psc_djk	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	if (g_pstShMemMPA->axis_djk[i] == DBL_MAX)			return FALSE;	break;
	case ENG_PSCC::psc_pro	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	if (g_pstShMemMPA->velocity_profile[i] == UINT8_MAX)return FALSE;	break;
	case ENG_PSCC::psc_spd	: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	if (g_pstShMemMPA->axis_speed[i] == DBL_MAX)		return FALSE;	break;
	default					: return FALSE;
	}
	return TRUE;
}
BOOL IsRecvBasicControlCmd(ENG_PSCC code)
{
	switch (code)
	{
	case ENG_PSCC::psc_hom	: ;	break;
	case ENG_PSCC::psc_mth	: ;	break;
	case ENG_PSCC::psc_mva	: ;	break;
	case ENG_PSCC::psc_mvr	: ;	break;
	case ENG_PSCC::psc_sof	: ;	break;
	case ENG_PSCC::psc_son	: ;	break;
	case ENG_PSCC::psc_stp	: ;	break;
	case ENG_PSCC::psc_wmc	: ;	break;
	default					: return FALSE;
	}
	return TRUE;
}
BOOL IsRecvAlignmentCmd(ENG_PSCC code)
{
	UINT8 i	= 0x00;
	switch (code)
	{
	case ENG_PSCC::psc__asz				: return (g_pstShMemMPA->wafer_size_detection != UINT8_MAX);		/* Set/Display automatic wafer Size detection */
	case ENG_PSCC::psc__autoscanagain	: return (g_pstShMemMPA->wafer_scan_again != UINT8_MAX);			/* Set/Display align again in case of CCD data error */
	case ENG_PSCC::psc__finalvacc		: return (g_pstShMemMPA->vacuum_state_after_align != UINT8_MAX);	/* Set/Display vacuum on chuck after BAL state */
	case ENG_PSCC::psc_mglm				: return (g_pstShMemMPA->wafer_glass_type != UINT8_MAX);			/* Set/Display glass mode operation */
	case ENG_PSCC::psc__ld				: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	if (g_pstShMemMPA->wafer_loading_down_pos[i] == DBL_MAX)	return FALSE;	break;	/* Set/Display load down position */
	case ENG_PSCC::psc__lu				: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	if (g_pstShMemMPA->wafer_loading_up_pos[i] == DBL_MAX)		return FALSE;	break;	/* Set/Display load up position */
	case ENG_PSCC::psc__ma				: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	if (g_pstShMemMPA->measure_acceleration[i] == DBL_MAX)		return FALSE;	break;	/* Set/Display measure acceleration */
	case ENG_PSCC::psc__md				: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	if (g_pstShMemMPA->measure_deceleration[i] == DBL_MAX)		return FALSE;	break;	/* Set/Display measure deceleration */
	case ENG_PSCC::psc__mgd				: return (g_pstShMemMPA->min_good_sample_count != UINT16_MAX);		/* Set/Display minimum good data samples */
	case ENG_PSCC::psc__maxwsz			: return (g_pstShMemMPA->wafer_max_size != UINT8_MAX);				/* Set/Display maximum wafer size */
	case ENG_PSCC::psc__minwsz			: return (g_pstShMemMPA->wafer_min_size != UINT8_MAX);				/* Set/Display minimum wafer size */
	case ENG_PSCC::psc__notchdepthmode	: return (g_pstShMemMPA->notch_depth_mode != UINT8_MAX);			/* Set/Display notch / flat depth calculation mode */
	case ENG_PSCC::psc__mnd				: return (g_pstShMemMPA->notch_range_depth_min != UINT32_MAX);		/* Set/Display minimum notch depth */
	case ENG_PSCC::psc__mnw				: return (g_pstShMemMPA->notch_range_width_min != UINT32_MAX);		/* Set/Display minimum notch width */
	case ENG_PSCC::psc__mtpmode			: return (g_pstShMemMPA->mode_z_after_move_pins != UINT8_MAX);		/* Set/Display MTP command mode of operation */
	case ENG_PSCC::psc__mxd				: return (g_pstShMemMPA->notch_range_depth_max != UINT32_MAX);		/* Set/Display maximum notch depth */
	case ENG_PSCC::psc__mxw				: return (g_pstShMemMPA->notch_range_width_max != UINT32_MAX);		/* Set/Display maximum notch width */
	case ENG_PSCC::psc__ms				: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	if (g_pstShMemMPA->measure_speed[i] == DBL_MAX)				return FALSE;	break;	/* Set/Display measure speed */
	case ENG_PSCC::psc__pp				: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	if (g_pstShMemMPA->level_chuck_pins_pos_z == DBL_MAX)		return FALSE;	break;	/* Set/Display Z-axis coordinate at pins position */
	case ENG_PSCC::psc__ppcd			: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	if (g_pstShMemMPA->offset_chuck_pin_pos_up == DBL_MAX)		return FALSE;	break;	/* Set/Display pins position correction when going down */
	case ENG_PSCC::psc__ppcu			: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	if (g_pstShMemMPA->offset_chuck_pin_pos_down == DBL_MAX)	return FALSE;	break;	/* Set/Display pins position correction when going up */
	case ENG_PSCC::psc__sa				: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	if (g_pstShMemMPA->ccd_r_axis_angle == DBL_MAX)				return FALSE;	break;	/* Set/Display CCD sensor angle */
	case ENG_PSCC::psc__sam				: return (g_pstShMemMPA->notch_average_mode != UINT8_MAX);			/* Set/Display sample averaging mode */
	case ENG_PSCC::psc__sc				: return (g_pstShMemMPA->ccd_sensor_center != UINT32_MAX);			/* Set/Display CCD sensor center value */
	case ENG_PSCC::psc__sm				: return (g_pstShMemMPA->rotation_sample_mode != UINT8_MAX);		/* Set/Display sampling mode */
	case ENG_PSCC::psc__sp				: return (g_pstShMemMPA->stop_on_pins != UINT8_MAX);				/* Set/Display stop on pins mode */
	case ENG_PSCC::psc__sqmin			: return (g_pstShMemMPA->ccd_grab_size_min != UINT32_MAX);			/* Set/Display minimum CCD value for square substrates */
	case ENG_PSCC::psc__sqmax			: return (g_pstShMemMPA->ccd_grab_size_max != UINT32_MAX);			/* Set/Display maximum CCD value for square substrates */
	case ENG_PSCC::psc__td				: return (g_pstShMemMPA->wafer_trans_safe_pos_up != DBL_MAX);		/* Set/Display transfer down position */
	case ENG_PSCC::psc__to				: return (g_pstShMemMPA->last_param_timeout != UINT32_MAX);			/* Set/Display timeouts */
	case ENG_PSCC::psc__tu				: return (g_pstShMemMPA->wafer_trans_safe_pos_up != DBL_MAX);		/* Set/Display transfer up position */
	case ENG_PSCC::psc__wszautoset		: return (g_pstShMemMPA->wafer_size_inch != INT8_MAX);				/* Assign a set of parameters to wafer size */
	case ENG_PSCC::psc__wt				: return (g_pstShMemMPA->wafer_type != INT8_MAX);					/* Set/Display “wafer” type */
	case ENG_PSCC::psc_ccd				: return (g_pstShMemMPA->ccd_pixels != UINT16_MAX);					/* Read CCD sensor */
	case ENG_PSCC::psc_doc				: return (g_pstShMemMPA->wafer_loaded != UINT8_MAX);				/* Detect an object using CCD sensors (Wafer Loaded) */
	case ENG_PSCC::psc_mfwo				: return (g_pstShMemMPA->rotate_after_align != DBL_MAX);			/* Set/Display final wafer orientation */
	case ENG_PSCC::psc_ltc				: return (g_pstShMemMPA->wafer_loading_type != UINT8_MAX);			/* Set/Display loading type (on chuck/on pins) */
	case ENG_PSCC::psc_nbn				: return (g_pstShMemMPA->last_align_fail != INT16_MAX);				/* Display number of bad notches */
	case ENG_PSCC::psc_ngn				: return (g_pstShMemMPA->last_align_succ != INT16_MAX);				/* Display number of good notches */
	case ENG_PSCC::psc_ndw				: for (; i<2; i++)							if (g_pstShMemMPA->find_depth_width[i] == UINT32_MAX)	return FALSE;	break;	/* Display notch depth and width */
	case ENG_PSCC::psc_mrwi				: for (; i<MAX_ALIGNER_DRIVE_COUNT; i++)	if (g_pstShMemMPA->rwi_wafer_info[i] == DBL_MAX)		return FALSE;	break;	/* Read wafer information */
	case ENG_PSCC::psc_wsz				: return (g_pstShMemMPA->wafer_work_size != UINT8_MAX);				/* Set/Display wafer size */
	case ENG_PSCC::psc_wsz2				: return (g_pstShMemMPA->notch_light_source != UINT8_MAX);			/* Set/Display wafer size when two LEDs are available */
	default								: return FALSE;
	}
	return TRUE;
}
BOOL IsRecvHousekeepingCmd(ENG_PSCC code)
{
	switch (code)
	{
	case ENG_PSCC::psc_lps	:	break;
	case ENG_PSCC::psc_rmp	:	break;
	case ENG_PSCC::psc_rps	:	break;
	case ENG_PSCC::psc_smp	:	break;
	case ENG_PSCC::psc_sps	:	break;
	case ENG_PSCC::psc_wps	:	break;
	default					: return FALSE;
	}
	return TRUE;
}
BOOL IsRecvCaliSvcCmd(ENG_PSCC code)
{
	switch (code)
	{
	case ENG_PSCC::psc__autodumps	: return (g_pstShMemMPA->align_fail_auto_dumps != UINT8_MAX);	/* Set/Display wafer size when two LEDs are available */
	case ENG_PSCC::psc__autodumpsf	: return (g_pstShMemMPA->auto_samples_dump_file != UINT8_MAX);	/* Specifies the file name for CCD samples from unsuccessful alignment */
	case ENG_PSCC::psc_ddpf			:																/* Saves CCD samples to .BAL file */
	case ENG_PSCC::psc_ddumpsamples	:																/* Saves CCD samples to .CSV file */
	case ENG_PSCC::psc_drps			: return strlen(g_pstShMemMPA->last_string_data) > 0;
	default							: return FALSE;
	}
	return TRUE;
}
BOOL IsRecvEdgeHandlingPreAlignerSpecCmd(ENG_PSCC code)
{
	switch (code)
	{
	case ENG_PSCC::psc__eh		: return (g_pstShMemMPA->is_edge_handling != UINT8_MAX);			/* Verifies the prealigner is edge handling */
	case ENG_PSCC::psc__ehc		: return (g_pstShMemMPA->ccd_chuck_sensing_value != UINT16_MAX);	/* Maximum chuck CCD value for Normal Size */
	case ENG_PSCC::psc__ehc1	: return (g_pstShMemMPA->ccd_chuck_sensing_small != UINT16_MAX);	/* Maximum chuck CCD value for Smaller Size */
	case ENG_PSCC::psc__ehc2	: return (g_pstShMemMPA->ccd_chuck_sensing_great != UINT16_MAX);	/* Maximum chuck CCD value for Greater Size */
	case ENG_PSCC::psc__ehp		: return (g_pstShMemMPA->ccd_pins_sensing_value != UINT16_MAX);		/* Maximum chuck pins CCD value for Normal Size */
	case ENG_PSCC::psc__ehp1	: return (g_pstShMemMPA->ccd_pins_sensing_small != UINT16_MAX);		/* Maximum chuck pins CCD value for Smaller Size */
	case ENG_PSCC::psc__ehp2	: return (g_pstShMemMPA->ccd_pins_sensing_great != UINT16_MAX);		/* Maximum chuck pins CCD value for Greater Size */
	case ENG_PSCC::psc__sfz		: return (g_pstShMemMPA->safe_rotate_z_pos != DBL_MAX);;			/* Set/Display safe Z position above which which the chuck is clear of the stationary pins can rotate freely */
	default						: return FALSE;
	}
	return TRUE;
}
BOOL IsRecvExoticCmd(ENG_PSCC code)
{
	switch (code)
	{
	case ENG_PSCC::psc__mindepthmain		: return (g_pstShMemMPA->minimum_notch_depth != UINT8_MAX);
	case ENG_PSCC::psc__notchfindmode		: return (g_pstShMemMPA->notch_find_mode != UINT8_MAX);
	case ENG_PSCC::psc__notchmaxderval		: return (g_pstShMemMPA->notch_max_der_val != UINT8_MAX);
	case ENG_PSCC::psc__notchminderval		: return (g_pstShMemMPA->notch_min_der_val != UINT8_MAX);
	case ENG_PSCC::psc__notchminderwidth	: return (g_pstShMemMPA->notch_min_der_width != UINT8_MAX);
	case ENG_PSCC::psc__notchnavg			: return (g_pstShMemMPA->notch_num_average != UINT8_MAX);
	default									: return FALSE;
	}
	return TRUE;
}
BOOL IsRecvMiscCmd(ENG_PSCC code)
{
	switch (code)
	{
	case ENG_PSCC::psc__dt	: break;
	case ENG_PSCC::psc_hsa	: break;
	case ENG_PSCC::psc_wsa	: break;
	case ENG_PSCC::psc_res	: break;
	default					: return FALSE;
	}
	return TRUE;
}


/* ------------------------------------------------------------------------------------ */
/*                                      외부 함수                                       */
/* ------------------------------------------------------------------------------------ */

#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : PreAlinger Library 초기화
 parm : config	- [in]  환경 설정 정보
		shmem	- [in]  Prealigner 관련 공유 메모리
 retn : None
*/
API_EXPORT BOOL uvMPA_Init(LPG_CIEA config, LPG_MPDV shmem)
{
	/* 공유 메모리 연결 */
	g_pstShMemMPA	= shmem;
	/* 환경 정보 연결 */
	g_pstConfig		= config;

	/* 시리얼 통신 스레드 생성 */
	g_pMPAThread	= new CMPAThread(config->pre_aligner.port_no,
									 config->pre_aligner.baud_rate,
									 config->pre_aligner.port_buff,
									 shmem);
	ASSERT(g_pMPAThread);
	if (!g_pMPAThread->CreateThread(0, NULL, 100))	return FALSE;

	return TRUE;
}

/*
 desc : PreAlinger Library 해제
 parm : None
 retn : None
*/
API_EXPORT VOID uvMPA_Close()
{
	/* 작업 스레드 메모리 해제 */
	if (g_pMPAThread)
	{
		if (g_pMPAThread->IsBusy())		g_pMPAThread->Stop();
		while (g_pMPAThread->IsBusy())	g_pMPAThread->Sleep();
		delete g_pMPAThread;
		g_pMPAThread	= NULL;
	}
}

/*
 desc : 현재 Logosol Pre-Aligner와 연결되어 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_IsConnected()
{
	if (!g_pMPAThread->IsBusy())	return FALSE;
	return g_pMPAThread->IsOpenPort();
}

/*
 desc : MPA 가 초기화 완료 되었는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_IsInitialized()
{
	if (!g_pMPAThread->IsBusy())	return FALSE;
	return g_pMPAThread->IsInitialized();
}


/*
 desc : 기존 통신 관련 송/수신 데이터 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvMPA_ResetCommData()
{
	g_pMPAThread->ResetCommData();
}

/*
 desc : 가장 최근에 송신한 명령어에 대한 응답 상태 값 반환
 parm : None
 retn : 0x00 - FAIL, 0x01 - SUCC, 0xff - Not defined (Not received)
*/
API_EXPORT ENG_MSAS uvMPA_GetLastSendAckStatus()
{
	return g_pMPAThread->GetLastSendAckStatus();
}

/*
 desc : 최근에 수신된 명령어 크기 (단위: Bytes)
 parm : None
 retn : 최근에 수신된 명령어 크기 (단위: Bytes)
*/
API_EXPORT UINT32 uvMPA_GetRecvSize()
{
	return g_pMPAThread->GetRecvSize();
}

/*
 desc : 명령어 송신에 대한 응답 모드 값 설정
 parm : mode	- [in]  응답 모드 값
						0: 동기 방식 (엄격한 Master-Slave 방식) - 응답이 올 때까지 다른 명령 송신할 수 없음
						   보통 1초 이내에 응답이 올 때, 사용하는 방법
						1: 비동기 방식 - 응답 값이 16진수 응답 값으로 반환 (추천)
						2: 비동기 방식 - 응답 값이 오로지,'>' or '?' 값 둘중에 하나로만 반환
						3: 비동기 방식 (1과 비슷하지만 숫자가 아닌 문자열 반환 받을 때 사용)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetInfoMode(UINT32 mode)
{
	CHAR szCmd[32]	= {NULL};

	/* 송신 후 응답 확인 */
	sprintf_s(szCmd, 32, "INF %d", mode);
	if (!g_pMPAThread->SendData(ENG_PSCC::psc_inf, szCmd))	return FALSE;

	return TRUE;
}
API_EXPORT BOOL uvMPA_GetInfoMode()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_inf, "INF");
}

/*
 desc : 모션 축 (T:Rotation, R:Horizontal, Z:Vertical)의 위치 정보 얻기
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetAxisPos()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_mcpo, "MCPO");
}

/*
 desc : 현재 Prealigner의 장비 진단 상태 얻기
 parm : None
 retn : TRUE or FALSE
 note : 장비의 현재 상태와 가장 최근의 에러 상태 (FAULT)
*/
API_EXPORT BOOL uvMPA_GetDiagState()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_diag, "DIAG C");
}

/*
 desc : 현재 Prealigner의 모션 동작 중에 발생된 에러 상태 얻기
 parm : None
 retn : TRUE or FALSE
 note : 장비의 가장 최근 에러에 대한 상세 정보 즉, 모션 동작 중에 에러가 발생한 경우
*/
API_EXPORT BOOL uvMPA_GetMotionState()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_est, "EST");
}

/*
 desc : 현재 Prealigner의 직전 (이전)에 발생된 에러 원인 정보 얻기
 parm : None
 retn : TRUE or FALSE
 note : Prealigner의 동작 중에 에러가 발생 했다면, 발생 원인을 텍스트 형식으로 제공
*/
API_EXPORT BOOL uvMPA_GetErrorReason()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_per, "PER");
}

/*
 desc : Prealinger의 모든 축에 대한 상태 (Homing 했는지 즉, Active 여부) 값 반환
 parm : None
 retn : TRUE or FALSE
 note : 각 축이 Homing 할 때, Home과 Limit 쪽 스위치를 인지 했는지 여부
*/
API_EXPORT BOOL uvMPA_GetAllSwitchState()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_rhs, "RHS");
}

/*
 desc : Prealigner의 컨트롤러 상태 정보 반환 (코드 값으로 반환)
 parm : None
 retn : TRUE or FALSE
 note : 강제로 값 출력 모드를 INF 2 (1 값이 아님에 주의) 설정 함
*/
API_EXPORT BOOL uvMPA_GetSystemStatus()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_sta, "STA");
}

/*
 desc : Prealigner내에 동작되는 각 모듈들의 전체 펌웨어 버전 반환
 parm : None
 retn : TRUE or FALSE
 note : 반환 예> MCL Code Version 2.0.1.6
				 System-1.3.2.9 PreAl-1.1.1.37 Calib-1.1.1.18 PA-B1.2.1.8
*/
API_EXPORT BOOL uvMPA_GetVerModuleAll()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_ver, "VER S");
}

/*
 desc : 각 축 (R, Z, T)들의 Home Position 값 반환
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetAxisHomePosition()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_ver, "_CH");
}

/*
 desc : 각 축(R, Z, T) 중 임의 Home Position 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		value	- [in]  Axis에 따른 Home Position 값
						입력 범위는 음수/양수 값 모두 가능 ? (개발사로부터 답변이 옴)
						(unit: R/Z: um, T: degree)
 retn : TRUE or FALSE
 */
API_EXPORT BOOL uvMPA_SetAxisHomePosition(ENG_PANC axis, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	UINT32 u32Value	= GetAxisPosDev(axis, value);
	/* 소숫점을 없애고 문자열로 변환 */
	sprintf_s(szCmd, 32, "_CH %c %u", GetAxisChar(axis), u32Value);

	return g_pMPAThread->SendData(ENG_PSCC::psc__ch, szCmd);
}

/*
 desc : 각 축(R, Z, T) 중 현재 위치를 Home Position 값으로 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
 retn : TRUE or FALSE
 */
API_EXPORT BOOL uvMPA_SetAxisCurHomePosition(ENG_PANC axis)
{
	CHAR szCmd[32]	= {NULL};
	/* 소숫점을 없애고 문자열로 변환 */
	sprintf_s(szCmd, 32, "_CH %c", GetAxisChar(axis));

	return g_pMPAThread->SendData(ENG_PSCC::psc__ch, szCmd);
}

/*
 desc : 각 축 (R, Z, T)들의 전류 제한 (레벨) 값 반환
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetAxisCurrentLimit()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc__cl, "_CL");
}

/*
 desc : 각 축(R, Z, T) 중 임의 전류 제한 (레벨) 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		value	- [in]  Axis에 따른 Current Limit 값 (1 ~ 251)
 retn : TRUE or FALSE
 */
API_EXPORT BOOL uvMPA_SetAxisCurrentLimit(ENG_PANC axis, UINT8 value)
{
	CHAR szCmd[32]	= {NULL};
	/* 입력 값이 제한 범위를 벗어 났는지 확인 */
	if (value < 1 || value > 251)	return FALSE;
	/* 소숫점을 없애고 문자열로 변환 */
	sprintf_s(szCmd, 32, "_CL %c %u", GetAxisChar(axis), value);

	return g_pMPAThread->SendData(ENG_PSCC::psc__cl, szCmd);
}

/*
 desc : 각 축 (R, Z, T)들의 Position Limit Error 값 반환
 parm : None
 retn : TRUE or FALSE
 note : Motor (Servo)가 가지고 있는 민감도에 따라, 다르게 설정되어 있음
		특정 환경마다 설치된 Servo Motor의 Tuning 계수를 설정해야 됨
		Motor 마다 설정된 이 에러 값 기준으로, 이동할 때마다 Motor가 벗어나면 자동으로 멈추게 됨
*/
API_EXPORT BOOL uvMPA_GetAxisPositionLimitError()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc__el, "_EL");
}

/*
 desc : 각 축(R, Z, T) 중 Position Limit Error 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		value	- [in]  Axis에 따른 Position Limit Error 값 (1 ~ 65535)
						Encoder Counts 값 (주위! um 값이 아님!)
 retn : TRUE or FALSE
 note : 설정된 값 보다, Motor가 그 이상으로 동작 했다면, Prealinger 내부의 펌웨어서 자동 멈춤
 */
API_EXPORT BOOL uvMPA_SetAxisPositionLimitError(ENG_PANC axis, UINT16 value)
{
	CHAR szCmd[32]	= {NULL};
	/* 소숫점을 없애고 문자열로 변환 */
	sprintf_s(szCmd, 32, "_EL %c %u", GetAxisChar(axis), value);

	return g_pMPAThread->SendData(ENG_PSCC::psc__el, szCmd);
}

/*
 desc : 각 축 (R, Z, T)들의 Forward or Reversion Direction에 대한 Limit 값 반환
 parm : direct	- [in]  Axis Direction (축의 이동 방향 즉, 정방향 or 역방향)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetAxisDirectLimit(ENG_MAMD direct)
{
	CHAR szCmd[2][8] = { "_FL", "_RL" };
	ENG_PSCC enCmd[2]= { ENG_PSCC::psc__fl, ENG_PSCC::psc__rl };

	return g_pMPAThread->SendData(enCmd[UINT8(direct)], szCmd[UINT8(direct)]);
}

/*
 desc : 각 축 (R, Z, T)들의 Forward or Reversion Direction에 대한 Limit 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		direct	- [in]  Axis Direction (축의 이동 방향 즉, 정방향 or 역방향)
		value	- [in]  Axis에 따른 Home Position 값
						입력 범위는 음수/양수 값 모두 가능 ? (개발사로부터 답변이 옴)
						(unit: R/Z: um, T: degree)
 retn : TRUE or FALSE
 */
API_EXPORT BOOL uvMPA_SetAxisDirectLimit(ENG_PANC axis, ENG_MAMD direct, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL}, szDirect[2][8] = { "_FL", "_RL" };
	ENG_PSCC enCmd[2]= { ENG_PSCC::psc__fl, ENG_PSCC::psc__rl };
	UINT32 u32Value	= GetAxisPosDev(axis, value);
	/* 소숫점을 없애고 문자열로 변환 */
	sprintf_s(szCmd, 32, "%s %c %u", szDirect[UINT8(direct)], GetAxisChar(axis), u32Value);

	return g_pMPAThread->SendData(enCmd[UINT8(direct)], szCmd);
}

/*
 desc : 각 축 (R, Z, T)들의 Home Offset 값 반환
 parm : None
 retn : TRUE or FALSE
 note : 설정된 Home Position으로 Homing 했을 때, 실제 이동한 위치 값이 0이 아닌 경우,
		0 으로 바뀌기 위한 오차 값
*/
API_EXPORT BOOL uvMPA_GetAxisHomeOffset()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc__ho, "_HO");
}

/*
 desc : 각 축(R, Z, T) 중 임의 Home Offset 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		value	- [in]  Axis에 따른 Home Position 값
						입력 범위는 음수/양수 값 모두 가능 ? (개발사로부터 답변이 옴)
						(unit: R/Z: um, T: degree)
 retn : TRUE or FALSE
 note : 개발 쪽에서는 거의 사용 안함. (생산자 쪽에서 사용하는 명령어)
 */
API_EXPORT BOOL uvMPA_SetAxisHomeOffset(ENG_PANC axis, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	UINT32 u32Value	= GetAxisPosDev(axis, value);
	/* 소숫점을 없애고 문자열로 변환 */
	sprintf_s(szCmd, 32, "_HO %c %u", GetAxisChar(axis), u32Value);

	return g_pMPAThread->SendData(ENG_PSCC::psc__ho, szCmd);
}

/*
 desc : 각 축(R, Z, T) 중 현재 위치를 Home Offset 값으로 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
 retn : TRUE or FALSE
 */
API_EXPORT BOOL uvMPA_SetAxisCurHomeOffset(ENG_PANC axis)
{
	CHAR szCmd[32]	= {NULL};
	/* 소숫점을 없애고 문자열로 변환 */
	sprintf_s(szCmd, 32, "_HO %c", GetAxisChar(axis));

	return g_pMPAThread->SendData(ENG_PSCC::psc__ho, szCmd);
}

/*
 desc : 각 축 (R, Z, T)들의 Acceleration or Deceleration (가속도 or 감속도) 값 반환
 parm : type	- [in]  0x00: Acceleration, 0x01: Deceleration
 retn : TRUE or FALSE
 note : 각 축의 순간 속력 (가속력 혹은 감속력) 값을 초당 얼마만큼 설정되어 있는지 반환
*/
API_EXPORT BOOL uvMPA_GetAxisInstSpeed(ENG_MAST type)
{
	CHAR szCmd[2][8] = { "ACL", "DCL" };
	ENG_PSCC enCmd[2]= { ENG_PSCC::psc_acl, ENG_PSCC::psc_dcl };

	return g_pMPAThread->SendData(enCmd[UINT8(type)], szCmd[UINT8(type)]);
}

/*
 desc : 각 축 (R, Z, T)들의 Acceleration or Deceleration (가속도 or 감속도) 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		type	- [in]  0x00: Acceleration, 0x01: Deceleration
		value	- [in]  Axis에 따른 Acceleration or Decleration 값
						(unit: R/Z: um / sec^2, T: degree / sec^2)
 retn : TRUE or FALSE
 */
API_EXPORT BOOL uvMPA_SetAxisInstSpeed(ENG_PANC axis, ENG_MAST type, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL}, szType[2][8] = { "ACL", "DCL" };
	ENG_PSCC enCmd[2]= { ENG_PSCC::psc_acl, ENG_PSCC::psc_dcl };
	UINT32 u32Value	= GetAxisPosDev(axis, value);
	/* 소숫점을 없애고 문자열로 변환 */
	sprintf_s(szCmd, 32, "%s %c %u", szType[UINT8(type)], GetAxisChar(axis), u32Value);

	return g_pMPAThread->SendData(enCmd[UINT8(type)], szCmd);
}

/*
 desc : 각 축 (R, Z, T)들의 Acceleration or Deceleration for JERK (가가속도 or 가감속도) 값 반환
 parm : type	- [in]  0x00: Acceleration, 0x01: Deceleration
 retn : TRUE or FALSE
 note : 각 축의 순간 속력 (가가속력 혹은 가감속력) 값을 초당 얼마만큼 설정되어 있는지 반환
		!!! Peraligner의 성능에는 영향을 주지 않음 !!!
*/
API_EXPORT BOOL uvMPA_GetAxisJerkSpeed(ENG_MAST type)
{
	CHAR szCmd[2][8] = { "AJK", "DJK" };
	ENG_PSCC enCmd[2]= { ENG_PSCC::psc_ajk, ENG_PSCC::psc_djk };

	return g_pMPAThread->SendData(enCmd[UINT8(type)], szCmd[UINT8(type)]);
}

/*
 desc : 각 축 (R, Z, T)들의 Acceleration or Deceleration for JERK (가가속도 or 가감속도) 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		type	- [in]  0x00: Acceleration, 0x01: Deceleration
		value	- [in]  Axis에 따른 Acceleration or Decleration for JERK 값
						(unit: R/Z: um / sec^2, T: degree / sec^2)
 retn : TRUE or FALSE
 note : !!! Peraligner의 성능에는 영향을 주지 않음 !!!
 */
API_EXPORT BOOL uvMPA_SetAxisJerkSpeed(ENG_PANC axis, ENG_MAST type, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL}, szDirect[2][8] = { "AJK", "DJK" };
	ENG_PSCC enCmd[2]= { ENG_PSCC::psc_ajk, ENG_PSCC::psc_djk };
	UINT32 u32Value	= GetAxisPosDev(axis, value);
	/* 소숫점을 없애고 문자열로 변환 */
	sprintf_s(szCmd, 32, "%s %c %u", szDirect[UINT8(type)], GetAxisChar(axis), u32Value);

	return g_pMPAThread->SendData(enCmd[UINT8(type)], szCmd);
}

/*
 desc : 각 축 (R, Z, T)들의 Velocity Profile 값 반환
 parm : None
 retn : TRUE or FALSE
 note : 0 : Trapezoidal velocity profile	(Web Search)
		1 : Bezier curve velocity profile	(Web Search)
		2 : Spline curve velocity profile	(Web Search)
		3 : S-Curved velocity profile		(Web Search)
		9 : Device velocity profile			(Web Search)
*/
API_EXPORT BOOL uvMPA_GetVelocityProfile()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_pro, "PRO");
}

/*
 desc : 각 축 (R, Z, T)들의 Velocity Profile 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		profile	- [in]  Profile Value (0, 1, 2, 3, 9 중 1개)
 retn : TRUE or FALSE
 note : 0 : Trapezoidal velocity profile	(Web Search)
		1 : Bezier curve velocity profile	(Web Search)
		2 : Spline curve velocity profile	(Web Search)
		3 : S-Curved velocity profile		(Web Search)
		9 : Device velocity profile			(Web Search)
*/
API_EXPORT BOOL uvMPA_SetVelocityProfile(ENG_PANC axis, UINT8 profile)
{
	CHAR szCmd[32]	= {NULL};

	/* Profile 값 유효성 검사 */
	switch (profile)
	{
	case 0 : case 1 : case 3 : case 9 :	break;
	default: return FALSE;
	}
	/* 소숫점을 없애고 문자열로 변환 */
	sprintf_s(szCmd, 32, "PRO %c %u", GetAxisChar(axis), profile);

	return g_pMPAThread->SendData(ENG_PSCC::psc_pro, szCmd);
}

/*
 desc : 각 축 (R, Z, T)들의 Speed 값 반환
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetAxisSpeed()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_spd, "SPD");
}

/*
 desc : 각 축 (R, Z, T)들의 Speed 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		speed	- [in]  Speed Value
						(unit: R/Z: um / sec^2, T: degree / sec^2)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetAxisSpeed(ENG_PANC axis, DOUBLE speed)
{
	CHAR szCmd[32]	= {NULL};
	UINT32 u32Value	= GetAxisPosDev(axis, speed);
	sprintf_s(szCmd, 32, "SPD %c %u", GetAxisChar(axis), u32Value);
	return g_pMPAThread->SendData(ENG_PSCC::psc_spd, szCmd);
}

/*
 desc : Prealigner의 모든 축들에 대해 초기화 (Homing...)
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetAxisAllHoming()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_hom, "HOM");
}

/*
 desc : 각 축 (R, Z, T)들의 Speed 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_MoveHomePosition(ENG_PANC axis)
{
	CHAR szCmd[4][8]= { "MTH T", "MTH R", "MTH Z", "MTH" };
	return g_pMPAThread->SendData(ENG_PSCC::psc_mth, szCmd[UINT8(axis)]);
}

/*
 desc : 각 축 (R, Z, T)들의 위치를 이동 (Move to Position)
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		type	- [in]  절대 이동 (0x00)인지, 상대 이동 (0x01)인지 여부
		value	- [in]  Axis에 따른 이동 거리 값 (상대 이동일 경우, 음수도 입력 가능?)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_MoveToAxis(ENG_PANC axis, ENG_PAMT type, DOUBLE value)
{
	CHAR szCmd[32]		= {NULL}, szType[2] = {'A', 'R' };
	ENG_PSCC enCmd[2]	= { ENG_PSCC::psc_mva, ENG_PSCC::psc_mvr };
	UINT32 u32Value		= GetAxisPosDev(axis, value);
	sprintf_s(szCmd, 32, "MV%c %c %u", szType[UINT8(type)], GetAxisChar(axis), u32Value);
	return g_pMPAThread->SendData(enCmd[UINT8(type)], szCmd);
}

/*
 desc : 각 축 (R, Z, T)들의 Servo Control ON or OFF
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		type	- [in]  Control Operation 즉, OFF (0x00) or ON (0x01)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_ServoControl(ENG_PANC axis, ENG_PSOO type)
{
	CHAR szCmd[32]		= {NULL}, szType[2] = {'F', 'N' };
	ENG_PSCC enCmd[2]	= { ENG_PSCC::psc_sof, ENG_PSCC::psc_son };
	if (axis == ENG_PANC::pac_a)	sprintf_s(szCmd, 32, "SO%c",	szType[UINT8(type)]);
	else							sprintf_s(szCmd, 32, "SO%c %c",	szType[UINT8(type)], GetAxisChar(axis));
	return g_pMPAThread->SendData(enCmd[UINT8(type)], szCmd);
}

/*
 desc : 임의 (선택된) 축 혹은 전체 축에 대해서 동작을 중지 시킴
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical)), 3: All
 retn : TRUE or FALSE
 note : 현재 실행 중인 Macros에 대해서도 중지 (취소) 시킴
*/
API_EXPORT BOOL uvMPA_StopToAxis(ENG_PANC axis)
{
	CHAR szCmd[32]	= {NULL};
	/* 소숫점을 없애고 문자열로 변환 */
	if (axis == ENG_PANC::pac_a)	sprintf_s(szCmd, 32, "STP");
	else							sprintf_s(szCmd, 32, "STP %c", GetAxisChar(axis));

	return g_pMPAThread->SendData(ENG_PSCC::psc_stp, szCmd);
}

/*
 desc : Prealigner에 제출된 모든 명령 (Macros)들과 작업 (Procedure)들의 실행(동작)이 완료될
		때까지 대기하고 특정 (임의) 축의 이동은 중지 시킴
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
 retn : TRUE or FALSE
 참고 :	가령 WMC Axis1: Axis1의 이동이 멈출 때까지 대기하고, 모든 Macros이 실행이 완료 됨
*/
API_EXPORT BOOL uvMPA_WaitPrealigner(ENG_PANC axis)
{
	CHAR szCmd[32]	= {NULL};
	/* 소숫점을 없애고 문자열로 변환 */
	if (axis == ENG_PANC::pac_a)	sprintf_s(szCmd, 32, "WMC");
	else							sprintf_s(szCmd, 32, "WMC %c", GetAxisChar(axis));

	return g_pMPAThread->SendData(ENG_PSCC::psc_wmc, szCmd);
}

/*
 desc : Chuck으로부터 Wafer를 제거하기 위해 Chuck Vacuum을 Close (OFF) or Open (ON) 수행
 parm : type	- [in]  Control Operation 즉, OFF (0x00) or ON (0x01)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_ChuckVacuum(ENG_PSOO type)
{
	CHAR szCmd[2][32]	= { "MCVF", "MCVN" };
	ENG_PSCC enCmd[2]	= { ENG_PSCC::psc_mcvf, ENG_PSCC::psc_mcvn };

	return g_pMPAThread->SendData(enCmd[UINT8(type)], szCmd[UINT8(type)]);
}

/*
 desc : Pins으로부터 Wafer를 제거하기 위해 Pins Vacuum을 Close (OFF) or Open (ON) 수행
 parm : type	- [in]  Control Operation 즉, OFF (0x00) or ON (0x01)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_PinsVacuum(ENG_PSOO type)
{
	CHAR szCmd[2][32] = { "MPVF", "MPVN" };
	ENG_PSCC enCmd[2] = { ENG_PSCC::psc_mpvf, ENG_PSCC::psc_mpvn };

	return g_pMPAThread->SendData(enCmd[UINT8(type)], szCmd[UINT8(type)]);
}

/*
 desc : Wafer Align (정렬) 수행 : Notch 검색
 parm : bal_mode	- [in]  Balance Mode (0, 2 ~ 5, if 1 값인 경우는 해당 사항 없음)
		max_offset	- [in]  Alignment Offset (unit: um)
		offset_try	- [in]  Offset Retry Count
		max_angle	- [in]  Notch Angle (unit: degree)
		angle_try	- [in]  Angle Retry Count
		orient_notch- [in]  Orient Notch 적용 여부 (0 or 1)
							if 1, Notch가 항상 Chuck에서 동일한 방식으로 배치되도록 Pin 아래 Chuck 회전
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_FindNotchAngle(DOUBLE max_offset, UINT8 offset_try,
									 DOUBLE max_angle, UINT8 angle_try, UINT8 orient_notch)
{
	CHAR szCmd[64]	= { "BAL" }, tzParam[32] = {NULL};
	UINT32 u32Offset= GetAxisPosDev(ENG_PANC::pac_r, max_offset);
	UINT32 u32Angle	= GetAxisPosDev(ENG_PANC::pac_t, max_angle);

	/* Offset 값이 설정되어 있다면 명령어에 파라미터 추가 */
	if (max_offset != 0.0f && offset_try)
	{
		/* Space 추가 */
		strcat_s(szCmd, 64-strlen(szCmd), " ");
		/* MaxOffset 추가 */
		sprintf_s(tzParam, 32, "%u", u32Offset);
		strcat_s(szCmd, 64-strlen(szCmd), tzParam);

		if (max_angle != 0.0f && angle_try)
		{
			/* Space 추가 */
			strcat_s(szCmd, 64-strlen(szCmd), " ");
			/* MaxAngle 추가 */
			sprintf_s(tzParam, 32, "%u", u32Angle);
			strcat_s(szCmd, 64-strlen(szCmd), tzParam);
		}
		if (orient_notch)
		{			
			strcat_s(szCmd, 64-strlen(szCmd), " 1"); /* Space 추가 */
		}
	}

	return g_pMPAThread->SendData(ENG_PSCC::psc_bal, szCmd);
}
API_EXPORT BOOL uvMPA_FindNotchAngleExt(UINT8 mode)
{
	CHAR szCmd[32]	= {NULL};

	if (mode == 0x01 || mode > 0x05)
	{
		LOG_WARN(ENG_EDIC::en_lspa, L"Invalid command parameter (Not BAL param (0, 2, 3, 4, 5)");
		return FALSE;
	}
	sprintf_s(szCmd, 32, "BAL %u", mode);
	return g_pMPAThread->SendData(ENG_PSCC::psc_bal, szCmd);
}
/*
 desc : CCD Sensor (Image Sensor)로부터 수집 (Grab)된 이미지의 Pixel 개수 반환
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetCCDSensorPixels()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_ccd, "CCD");
}

/*
 desc : CCD Sensor (Image Sensor)로부터 수집 (Grab)된 이미지의 Pixel 개수 설정
 parm : rate	- [in]  CCD Sensor가 Sampling 할 때, 읽어들 일 (캡처;Grab) 이미지의 픽셀 개수
						기본 값은 10으로 설정되어 있음 (0 ~ 4890)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetCCDSensorPixels(UINT16 value)
{
	CHAR szCmd[32]	= {NULL};
	/* 값이 잘못 입력되어 있다면, 유효 값으로 설정 */
	if (value < 1)	value = 1;
	/* 명령어 설정 */
	sprintf_s(szCmd, 32, "CCD %u", value);

	return g_pMPAThread->SendData(ENG_PSCC::psc_ccd, szCmd);
}

/*
 desc : Wafer가 놓여 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_CheckWaferLoaded()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_doc, "MDOC");
}

/*
 desc : Wafer Alignment 검색 후 Wafer 회전을 얼마만큼 진행할지 회전 각도 값 설정
 parm : angle	- [in]  Rotation Angle (degree)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetRotationAfterAlign(DOUBLE angle)
{
	CHAR szCmd[32]	= {NULL};
	UINT32 u32Angle	= GetAxisPosDev(ENG_PANC::pac_t, angle);
	sprintf_s(szCmd, 32, "MFWO %u", u32Angle);
	return g_pMPAThread->SendData(ENG_PSCC::psc_mfwo, szCmd);
}

/*
 desc : Wafer Alignment 검색 후 Wafer 회전을 얼마만큼 진행할지 회전 각도 값 반환
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetRotationAfterAlign()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_mfwo, "MFWO");
}

/*
 desc : Wafer가 Prealigner에 놓여질 유형 (장소) 설정
 parm : type	- [in]  PINS (0x00), CHCUK (0x01)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetWaferPlaceType(ENG_PWPT type)
{
	CHAR szCmd[32]	= {NULL};
	/* 명령어 설정 */
	sprintf_s(szCmd, 32, "LTC %u", UINT8(type));
	return g_pMPAThread->SendData(ENG_PSCC::psc_ltc, szCmd);
}

/*
 desc : Wafer가 Prealigner에 놓여질 유형 (장소) 값 반환
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetWaferPlaceType()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_ltc, "LTC");
}

/*
 desc : 현재 Prealigner가 Wafer Alignment 성공 혹은 실패한 개수 얻기
 parm : type	- [in]  fail (0x00), succ (0x01)
 retn : TRUE or FALSE
 note : 이 함수 호출하기에 앞서, uvMPA_FindNotchAngle(...) 호출이 선행되어야 함
*/
API_EXPORT BOOL uvMPA_GetWaferNotchCount(ENG_PASR type)
{
	CHAR szCmd[2][8]	= { "NBN", "NGN" };
	ENG_PSCC enCmd[2]	= { ENG_PSCC::psc_nbn, ENG_PSCC::psc_ngn };
	return g_pMPAThread->SendData(enCmd[UINT8(type)], szCmd[UINT8(type)]);
}

/*
 desc : 현재 Prealigner가 Wafer Alignment 검색 후 성공 및 실패에 개수 통틀어 각각의 깊이와 폭 값 요청
 parm : index	- [in]  Range (0 ~ Total: -1)
 retn : TRUE or FALSE
 note : 주의!!! 앞 부분에는 검색 (BAL) 후 Good 정보 (깊이/폭), 뒷 부분에는 Bad 정보 (깊이/폭)
		이 함수 호출하기에 앞서, uvMPA_GetWaferNotchCount(...) 호출이 선행되어야 함
*/
API_EXPORT BOOL uvMPA_GetWaferAlignDepthWidth(INT16 index)
{
	CHAR szCmd[32]	= {NULL};

	if (INT16(g_pstShMemMPA->last_align_fail+g_pstShMemMPA->last_align_succ) <= index)
	{
		TCHAR tzMesg[1024]	= {NULL};
		swprintf_s(tzMesg, 1024,
				   L"NDW parameter value is too large [bal_good=%d, bal_bad=%d]",
				   g_pstShMemMPA->last_align_succ, g_pstShMemMPA->last_align_fail);
		LOG_WARN(ENG_EDIC::en_lspa, tzMesg);
		return FALSE;
	}
	sprintf_s(szCmd, 32, "NDW %d", index);

	return g_pMPAThread->SendData(ENG_PSCC::psc_ndw, szCmd);
}

/*
 desc : Prealigner의 Notch 유형 (Depth, Width) 별 검색 조건 (Min & Max) 값 얻기
 parm : type	- [in] 0x00: Depth, 0x01: Width
		kind	- [in] 0x00: Min, 0x01: Max
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetWaferFindNotchRange(ENG_PNTI type, UINT8 kind)
{
	CHAR szCmd[32]	= {NULL};
	ENG_PSCC enCmd	= ENG_PSCC::psc_none;

	if (type == ENG_PNTI::nti_depth)
	{
		if (0x00 == kind)	{	strcpy_s(szCmd, 32, "_MND");	enCmd = ENG_PSCC::psc__mnd;	}	/* The minimum value for Depth */
		else				{	strcpy_s(szCmd, 32, "_MXD");	enCmd = ENG_PSCC::psc__mxd;	}	/* The maximum value for Depth */
	}
	else
	{
		if (0x00 == kind)	{	strcpy_s(szCmd, 32, "_MNW");	enCmd = ENG_PSCC::psc__mnw;	}	/* The minimum value for Width */
		else				{	strcpy_s(szCmd, 32, "_MXW");	enCmd = ENG_PSCC::psc__mxw;	}	/* The maximum value for Width */
	}

	return g_pMPAThread->SendData(enCmd, szCmd);
}

/*
 desc : Prealigner의 Notch 유형 (Depth, Width) 별 검색 조건 (Min & Max) 값 얻기
 parm : type	- [in] 0x00: Depth, 0x01: Width
		kind	- [in] 0x00: Min, 0x01: Max
		range	- [in] Range Value 값
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetWaferFindNotchRange(ENG_PNTI type, UINT8 kind, UINT32 range)
{
	CHAR szCmd[32]	= {NULL};
	ENG_PSCC enCmd	= ENG_PSCC::psc_none;

	if (type == ENG_PNTI::nti_depth)
	{
		if (0x00 == kind)	{	sprintf_s(szCmd, 32, "_MND %u", range);	enCmd = ENG_PSCC::psc__mnd;	}	/* The minimum value for Depth */
		else				{	sprintf_s(szCmd, 32, "_MXD %u", range);	enCmd = ENG_PSCC::psc__mxd;	}	/* The maximum value for Depth */
	}
	else
	{
		if (0x00 == kind)	{	sprintf_s(szCmd, 32, "_MNW %u", range);	enCmd = ENG_PSCC::psc__mnw;	}	/* The minimum value for Width */
		else				{	sprintf_s(szCmd, 32, "_MXW %u", range);	enCmd = ENG_PSCC::psc__mxw;	}	/* The maximum value for Width */
	}

	return g_pMPAThread->SendData(enCmd, szCmd);
}

/*
 desc : CCD Sampling의 개수(장수) 얻기 (이 값에 따라, Notch 검색하는데 소요되는 시간 결정?)
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetMinCCDSampleCount()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc__mgd, "_MGD");
}

/*
 desc : 최소 CCD Sampling의 개수(장수) 설정 (GOOD 판정한데 필요한 최소 Sample 개수)
 parm : samples	- [in]  Notch 검색하는데 필요한 CCD Sampling 개수 (10 ~ 2500)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetMinCCDSampleCount(UINT16 samples)
{
	CHAR szCmd[32]	= {NULL};

	/* 송신 후 응답 확인 (만약에 응답이 올 때까지 장시간 대기하면 어쩌나...) */
	if (samples < 10)	samples = 10;
	if (samples > 2500)	samples	= 2500;
	sprintf_s(szCmd, 32, "_MGD %u", samples);

	return g_pMPAThread->SendData(ENG_PSCC::psc__mgd, szCmd);
}

/*
 desc : 현재 Prealinger의 작업 가능한 Wafer Size 설정
 parm : type	- [in]  Min / Max 구분 값
		kind	- [in]  Wafer Size의 구분 (종류) 값 (2, 3, 4, 5, 6, 8, 12, 18 중 1개 값)
						50mm(2″), 75mm(3″), 100mm(4″), 125mm(5″), 150mm(6″),
						200mm(8″), 300mm(12″), 450mm(18″)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetWaferMinMaxSize(ENG_MMMR type, ENG_MWKT kind)
{
	CHAR szCmd[32]		= {NULL};
	ENG_PSCC enCmd[2]	= { ENG_PSCC::psc__minwsz, ENG_PSCC::psc__maxwsz };

	if (type == ENG_MMMR::mmr_min)	sprintf_s(szCmd, 32, "_MINWSZ %u", UINT8(kind));
	else							sprintf_s(szCmd, 32, "_MAXWSZ %u", UINT8(kind));

	return g_pMPAThread->SendData(enCmd[UINT8(type)], szCmd);
}

/*
 desc : 현재 Prealinger에 설정된 작업 가능한 Wafer Size 얻기
 parm : type	- [in]  Min / Max 구분 값
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetWaferMinMaxSize(ENG_MMMR type)
{
	CHAR szCmd[2][8]	= { "_MINWSZ", "_MAXWSZ" };
	ENG_PSCC enCmd[2]	= { ENG_PSCC::psc__minwsz, ENG_PSCC::psc__maxwsz };
	return g_pMPAThread->SendData(enCmd[UINT8(type)], szCmd[UINT8(type)]);
}

/*
 desc : 현재 Prealinger에서 작업하려는 Wafer Size 설정
 parm : kind	- [in]  Wafer Size의 구분 (종류) 값 (2, 3, 4, 5, 6, 8, 12, 18 중 1개 값)
						50mm(2″), 75mm(3″), 100mm(4″), 125mm(5″), 150mm(6″),
						200mm(8″), 300mm(12″), 450mm(18″)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetWaferWorkSize(ENG_MWKT kind)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "WSZ %u", UINT8(kind));
	return g_pMPAThread->SendData(ENG_PSCC::psc_wsz, szCmd);
}

/*
 desc : 현재 Prealinger에서 작업하려는 Wafer Size 얻기
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetWaferWorkSize()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_wsz, "WSZ");
}

/*
 desc : 현재 Prealinger에서 Wafer Notch 인식하기 위한 Light Source 설정
 parm : light	- [in]  0: 첫 번째 Light Source, 1: 두 번째 Light Source
 retn : TRUE or FALSE
 note : Some prealigners have two different light sources per wafer size.
		For example,
			LPA1218 has two light sources for the 12 inch sensor – for 300 and 310 mm and
			two light sources for the 18 inch sensor – for 450 and 460 mm wafers.
			This command sets or reports the selected light source.
*/
API_EXPORT BOOL uvMPA_SetWaferWorkLight(UINT8 light)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "WSZ2 %u", light);
	return g_pMPAThread->SendData(ENG_PSCC::psc_wsz2, szCmd);
}

/*
 desc : 현재 Prealinger에서 Wafer Notch 인식하기 위한 Light Source 반환
 parm : None
 retn : TRUE or FALSE
 note : Some prealigners have two different light sources per wafer size.
		For example,
			LPA1218 has two light sources for the 12 inch sensor – for 300 and 310 mm and
			two light sources for the 18 inch sensor – for 450 and 460 mm wafers.
			This command sets or reports the selected light source.
*/
API_EXPORT BOOL uvMPA_GetWaferWorkLight()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_wsz2, "WSZ2");
}

/*
 desc : Wafer의 중심 위치와 방향 정보 등을 반환
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetWaferCenterOrientation()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_mrwi, "MRWI");
}

/*
 desc : Wafer Size를 자동으로 감지할지 여부 설정 (내부적으로 Wafer Size를 설정하지 않은 경우 사용)
 parm : type	- [in]  0x00: Auto Detection 사용 안함, 0x01: Wafer Size를 자동으로 감지하는 모드 사용
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetAutoDetectWaferSize(ENG_PWSD type)
{
	CHAR szCmd[2][8]	= { "_ASZ 0", "_ASZ 1" };
	return g_pMPAThread->SendData(ENG_PSCC::psc__asz, szCmd[UINT8(type)]);
}

/*
 desc : Wafer Size를 자동으로 감지 여부 값 얻기
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetAutoDetectWaferSize()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc__asz, "_ASZ");
}

/*
 desc : 정렬 작업 (명령) 즉, Wafer Notch 검색이 실패한 경우,
		Wafer Edge의 추가적인 검사를 할지 여부 설정
 parm : type	- [in]  Auto Scan Again 관련 플래그 동작 활성화 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetAutoScanAgain(ENG_MFED type)
{
	CHAR szCmd[2][24]	= { "_AUTOSCANAGAIN 0", "_AUTOSCANAGAIN 1" };
	return g_pMPAThread->SendData(ENG_PSCC::psc__autoscanagain, szCmd[UINT8(type)]);
}

/*
 desc : 정렬 작업 (명령) 즉, Wafer Notch 검색이 실패한 경우,
		Wafer Edge의 추가적인 검사를 할지 여부 값 얻기
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetAutoScanAgain()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc__autoscanagain, "_AUTOSCANAGAIN");
}

/*
 desc : "BAL" 명령 (정렬 작업) 완료 후 Chuck 위에 Wafer의 Vacuum 상태를 On으로 할지 Off로 할지 설정
 parm : type	- [in]  Vacuum On (0x01) or Off (0x00)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetVacuumSetAfterBAL(ENG_PSOO type)
{
	CHAR szCmd[2][16]	= { "_FINALVACC 0", "_FINALVACC 1" };
	return g_pMPAThread->SendData(ENG_PSCC::psc__finalvacc, szCmd[UINT8(type)]);
}

/*
 desc : "BAL" 명령 (정렬 작업) 완료 후 Chuck 위에 Wafer의 Vacuum 상태 설정 값 반환
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetVacuumSetAfterBAL()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc__finalvacc, "_FINALACC");
}

/*
 desc : Wafer의 재질이 투명한지 여부 설정
 parm : type	- [in]  Transparent (0x01) or Non-transparent (0x00)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetWaferGlassType(ENG_PWGO type)
{
	CHAR szCmd[2][16]	= { "MGLM 0", "MGLM 1" };
	return g_pMPAThread->SendData(ENG_PSCC::psc_mglm, szCmd[UINT8(type)]);
}

/*
 desc : Wafer의 재질이 투명한지 여부 값 얻기
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetWaferGlassType()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_mglm, "MGLM");
}

/*
 desc : Prealigner에 Wafer가 놓여질 때 또는 Wafer를 가져갈 때 (가져 올 때), 위치 값 얻기
 parm : type	- [in] 0x00: Loading Down Position, 0x01: Loading Up Position
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetWaferLoadPosition(ENG_PWLP type)
{
	CHAR szCmd[32]		= {NULL};
	ENG_PSCC enCmd[2]	= { ENG_PSCC::psc__ld, ENG_PSCC::psc__lu };
	/* 명령 선택 */
	if (ENG_PWLP::wlp_load_down == type)	strcpy_s(szCmd, 32, "_LD");
	else									strcpy_s(szCmd, 32, "_LU");
	return g_pMPAThread->SendData(enCmd[UINT8(type)], szCmd);
}

/*
 desc : Prealigner에 Wafer가 놓여질 때 또는 Wafer를 가져갈 때 (가져 올 때), 위치 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		type	- [in] 0x00: Loading Down Position, 0x01: Loading Up Position
		pos		- [in] Position 값 (unit: um)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetWaferLoadPosition(ENG_PANC axis, ENG_PWLP type, DOUBLE pos)
{
	CHAR szCmd[32]		= {NULL};
	ENG_PSCC enCmd[2]	= { ENG_PSCC::psc__ld, ENG_PSCC::psc__lu };
	UINT32 u32Value		= GetAxisPosDev(ENG_PANC::pac_z, pos);

	/* 명령 선택 */
	if (ENG_PWLP::wlp_load_down == type)	sprintf_s(szCmd, 32, "_LD %c %u", GetAxisChar(axis), u32Value);
	else									sprintf_s(szCmd, 32, "_LU %c %u", GetAxisChar(axis), u32Value);

	return g_pMPAThread->SendData(enCmd[UINT8(type)], szCmd);
}

/*
 desc : 측정 단계 동안 T (Rotation) 축, Chuck 및 Pins 사이의 Wafer 이동 동안
		R (Horzontal)과 Z (Vertical) 축의 가속도 / 감속도 값 얻기
 parm : type	- [in] 0x00: 가속도 (Acceleration), 0x01: 감속도 (Deceleration)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetMeasureAccDcl(ENG_MAST type)
{
	CHAR szCmd[32]		= {NULL};
	ENG_PSCC enCmd[2]	= { ENG_PSCC::psc__ma, ENG_PSCC::psc__md };
	if (ENG_MAST::ast_acl == type)	strcpy_s(szCmd, 32, "_MA");
	else							strcpy_s(szCmd, 32, "_MD");
	return g_pMPAThread->SendData(enCmd[UINT8(type)], szCmd);
}

/*
 desc : 측정 단계 동안 T (Rotation) 축, Chuck 및 Pins 사이의 Wafer 이동 동안
		R (Horzontal)과 Z (Vertical) 축의 가속도 / 감속도 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		type	- [in] 0x00: 가속도 (Acceleration), 0x01: 감속도 (Deceleration)
		value	- [in]  이동하는데 필요한 각도의 가속도 혹은 이동하는데 가속도 값 등
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetMeasureAccDcl(ENG_PANC axis, ENG_MAST type, DOUBLE value)
{
	CHAR szCmd[32]		= {NULL};
	ENG_PSCC enCmd[2]	= { ENG_PSCC::psc__ma, ENG_PSCC::psc__md };
	UINT32 u32Value	= GetAxisPosDev(axis, value);
	if (ENG_MAST::ast_acl == type)	sprintf_s(szCmd, 32, "_MA %c %u", GetAxisChar(axis), u32Value);
	else							sprintf_s(szCmd, 32, "_MD %c %u", GetAxisChar(axis), u32Value);
	return g_pMPAThread->SendData(enCmd[UINT8(type)], szCmd);
}

/*
 desc : 측정 단계 동안 T (Rotation) 축, Chuck 및 Pins 사이의 Wafer 이동 동안
		R (Horzontal)과 Z (Vertical) 축의 속도 값 얻기
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetMeasureSpeedAxis()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc__ms, "_MS");
}

/*
 desc : 측정 단계 동안 T (Rotation) 축, Chuck 및 Pins 사이의 Wafer 이동 동안
		R (Horzontal)과 Z (Vertical) 축의 속도 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		value	- [in]  이동하는데 필요한 각도의 가속도 혹은 이동하는데 가속도 값 등
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetMeasureSpeedAxis(ENG_PANC axis, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	UINT32 u32Value	= GetAxisPosDev(axis, value);
	sprintf_s(szCmd, 32, "_MS %c %u", GetAxisChar(axis), u32Value);
	return g_pMPAThread->SendData(ENG_PSCC::psc__ms, szCmd);
}

/*
 desc : Notch의 Depth 해상도의 모드 값 얻기
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetNotchDepthMode()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc__notchdepthmode, "_NOTCHDEPTHMODE");
}

/*
 desc : Notch의 Depth 해상도의 모드 값 설정
 parm : enable	- [in] Notch Depth 해상도를 표준 (기본) 값보다 더 높이 할것인지 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetNotchDepthMode(bool enable)
{
	CHAR szCmd[2][32]	= { "_NOTCHDEPTHMODE 0", "_NOTCHDEPTHMODE 1" };
	return g_pMPAThread->SendData(ENG_PSCC::psc__notchdepthmode, szCmd[UINT8(enable)]);
}

/*
 desc : Wafer Alignment 동작될 때, Prealinger의 축(T/R/Z)의 위치 이동 (Measureing Position)
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetWaferAlignmentMeasuringPos()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_mmtm, "MMTM");
}

/*
 desc : Wafer Alignment 동작될 때, Prealinger의 축(T/R/Z)의 위치를 반환 (Loading Position)
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetWaferAlignmentLoadingPos()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_mmtl, "MMTL");
}

/*
 desc : MTP 명령 이후에 Z (Veritcal)축의 최종 위치 모드 값 얻기
 parm : None
 retn : TRUE or FALSE
 note : MTP Command: Wafer를 Pin으로 이동 시작하라는 명령어
*/
API_EXPORT BOOL uvMPA_GetModeZAfterMovePins()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc__mtpmode, "_MTPMODE");
}

/*
 desc : MTP 명령 이후에 Z (Veritcal)축의 최종 위치 모드 값 얻기
 parm : mode	- [in]  _LD (Load Down Position; 0), _LU (Load Up Position; 1)
 retn : TRUE or FALSE
 note : MTP Command: Wafer를 Pin으로 이동 시작하라는 명령어
		_LD Command: Wafer가 올려지는 Pins의 Load Down Position
		_LU Command: Wafer가 올려지는 Pins의 Load Up Position
*/
API_EXPORT BOOL uvMPA_SetModeZAfterMovePins(ENG_PWLP mode)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_MTPMODE %u", UINT8(mode));
	return g_pMPAThread->SendData(ENG_PSCC::psc__mtpmode, szCmd);
}

/*
 desc : Chuck과 Pins의 수평인 Z (Vertical) 축의 위치 (Position) 값 얻기
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetLevelChcukToPinsPosZ()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc__pp, "_PP");
}

/*
 desc : Chuck과 Pins의 수평인 Z (Vertical) 축의 위치 (Position) 값 얻기
 parm : value	- [in]  Z (Vertical) 축 높이 값 (단위: um)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetLevelChcukToPinsPosZ(DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	UINT32 u32Value	= GetAxisPosDev(ENG_PANC::pac_z, value);
	sprintf_s(szCmd, 32, "_PP %u", u32Value);
	return g_pMPAThread->SendData(ENG_PSCC::psc__pp, szCmd);
}

/*
 desc : Wafer가 Chuck에서 Pins로 이송될 때, Up/Down 오차 (Backlash)를 허용하는 Offset 값 얻기
 parm : type	- [in]  0x00: Down Postion Offset, 0x01: Up Position Offset
 retn : TRUE or FALSE
 note : 얇게 휘어진 Wafer를 Chuck To Pins으로의 이송에 대한 신뢰성 개선을 위한 Offset 값
		Up에서 Down / Up으로 Pins이 이동할 때, 발생할 수 있는 오차 (Backlash? 현상) 부분을
		허용하기 위한 Offset (오차) 값
*/
API_EXPORT BOOL uvMPA_GetOffsetChcukToPinsPosZ(ENG_PWLP type)
{
	CHAR szCmd[2][8]= { "_PPCD", "_PPCU" };
	ENG_PSCC enCmd[2]	= { ENG_PSCC::psc__ppcd, ENG_PSCC::psc__ppcu };
	return g_pMPAThread->SendData(enCmd[UINT8(type)], szCmd[UINT8(type)]);
}

/*
 desc : Wafer가 Chuck에서 Pins로 이송될 때, Up/Down 오차 (Backlash)를 허용하는 Offset 값 설정
 parm : type	- [in]  0x00: Down Postion Offset, 0x01: Up Position Offset
		value	- [in]  Z (Vertical) 축 Offset 값 (단위: um)
						Pins Position offset when moving down
 retn : TRUE or FALSE
 note : 얇게 휘어진 Wafer를 Chuck To Pins으로의 이송에 대한 신뢰성 개선을 위한 Offset 값
		Up에서 Down / Up으로 Pins이 이동할 때, 발생할 수 있는 오차 (Backlash? 현상) 부분을
		허용하기 위한 Offset (오차) 값
*/
API_EXPORT BOOL uvMPA_SetOffsetChcukToPinsPosZ(ENG_PWLP type, DOUBLE value)
{
	CHAR szCmd[32]		= {NULL};
	ENG_PSCC enCmd[2]	= { ENG_PSCC::psc__ppcd, ENG_PSCC::psc__ppcu };
	UINT32 u32Value		= GetAxisPosDev(ENG_PANC::pac_z, value);
	if (type == ENG_PWLP::wlp_load_down)	sprintf_s(szCmd, 32, "_PPCD %u", u32Value);
	else									sprintf_s(szCmd, 32, "_PPCU %u", u32Value);
	return g_pMPAThread->SendData(enCmd[UINT8(type)], szCmd);
}

/*
 desc : CCD Sensor와 R (Horizontal) 축 간의 각도 값 출력
 parm : None
 retn : TRUE or FALSE
 note : 주의!!! Wafer Size를 고려하지 않음
*/
API_EXPORT BOOL uvMPA_GetAngleRAxisToCCD()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc__sa, "_SA");
}

/*
 desc : CCD Sensor와 R (Horizontal) 축 간의 각도 값 설정
 parm : value	- [in]  R (horizontal) 축과 CCD Sensor 사이의 각도 (degrees)
 retn : TRUE or FALSE
 note : 주의!!! Wafer Size를 고려하지 않음
*/
API_EXPORT BOOL uvMPA_SetAngleRAxisToCCD(DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	UINT32 u32Value	= GetAxisPosDev(ENG_PANC::pac_t, value);
	sprintf_s(szCmd, 32, "_SA %u", u32Value);
	return g_pMPAThread->SendData(ENG_PSCC::psc__sa, szCmd);
}

/*
 desc : 현재 샘플 평균화 모드를 출력
		만약 모드 값이 3 or 5로 설정되어 있다면, 처음 측정된 Notch/Flat에 대해,
		차후 (이후부터) 검색되는 나머지 Wafer들의 Notch/Flat를 검색하는 기준 (평균)이 됨
 parm : None
 retn : TRUE or FALSE
 note : 이 명령을 사용하면, 임의 Wafer에 대한 Notch/flag 등을 Sampling 후에 평균치로 저장
		나머지 Wafer들을 검색할 때는 다시 모드 값을 3 혹은 5 이외의 값으로 설정해 놓아야 됨
*/
API_EXPORT BOOL uvMPA_GetNotchAverageMode()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc__sam, "_SAM");
}

/*
 desc : 현재 샘플 평균화 모드를 설정
		만약 모드 값이 3 or 5로 설정되어 있다면, 처음 측정된 Notch/Flat에 대해,
		차후 (이후부터) 검색되는 나머지 Wafer들의 Notch/Flat를 검색하는 기준 (평균)이 됨
 parm : enable	- [in]  TRUE: 처음 측정되는 Wafer의 Notch/flag을 평균화 값으로 적용
						FALSE: 평균화 값 적용 안함
 retn : TRUE or FALSE
 note : 이 명령을 사용하면, 임의 Wafer에 대한 Notch/flag 등을 Sampling 후에 평균치로 저장
		나머지 Wafer들을 검색할 때는 다시 모드 값을 3 혹은 5 이외의 값으로 설정해 놓아야 됨
*/
API_EXPORT BOOL uvMPA_SetNotchAverageMode(BOOL enable)
{
	CHAR szCmd[32]	= {NULL};
	if (enable)	strcpy_s(szCmd, 32, "_SAM 3");
	else		strcpy_s(szCmd, 32, "_SAM 0");
	return g_pMPAThread->SendData(ENG_PSCC::psc__sam, szCmd);
}

/*
 desc : CCD Sensor의 중심 설정 혹은 출력
		Wafer 크기 기준으로 Wafer Notch / flat 부분에 해당되는 위치 (단위: pixel) (개인적인 판단)
 parm : None
 retn : TRUE or FALSE
 note : Wafer Size는 고려 대상이 아님 (Philoptics Library Part)
*/
API_EXPORT BOOL uvMPA_GetCCDSensorCenter()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc__sc, "_SC");
}

/*
 desc : CCD Sensor의 중심 설정 혹은 출력
		Wafer 크기 기준으로 Wafer Notch / flat 부분에 해당되는 위치 (단위: pixel) (개인적인 판단)
 parm : pixel	- [in]  Wafer Size 기준으로 Wafer Notch or Flat 부분의 위치 값 (단위: pixel)
 retn : TRUE or FALSE
 note : Wafer Size는 고려 대상이 아님 (Philoptics Library Part)
*/
API_EXPORT BOOL uvMPA_SetCCDSensorCenter(UINT32 pixel)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_SC %u", pixel);
	return g_pMPAThread->SendData(ENG_PSCC::psc__sc, szCmd);
}

/*
 desc : Sampling Mode의 값을 출력
		즉, 이 모드는 회전 당 샘플링 된 샘플 수와 검사 속도 (“_MS”)를 설정하고
		“_MS”  명령어에 의해 반환된 값을 내부적으로 설정
		(즉, “_MS” 명령에 의해 설정된 속도 값을 기준으로 모드 값에 따라 다르게 동작 시킴)
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetRotationSampleMode()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc__sm, "_SM");
}

/*
 desc : Sampling Mode의 값을 설정
		즉, 이 모드는 회전 당 샘플링 된 샘플 수와 검사 속도 (“_MS”)를 설정하고
		“_MS”  명령어에 의해 반환된 값을 내부적으로 설정
		(즉, “_MS” 명령에 의해 설정된 속도 값을 기준으로 모드 값에 따라 다르게 동작 시킴)
 parm : mode	- [in]  Sampling Mode 값 (0, 1, 2, 3)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetRotationSampleMode(ENG_PRSM mode)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_SM %u", UINT8(mode));
	return g_pMPAThread->SendData(ENG_PSCC::psc__sm, szCmd);
}

/*
 desc : Wafer 이송 중에 Chuck 모션을 Pins Position 지점에서 멈출게 할지 여부 제어 값 반환
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetStopOnPins()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc__sp, "_SP");
}

/*
 desc : Wafer 이송 중에 Chuck 모션을 Pins Position 지점에서 멈출게 할지 여부 제어 값 설정
 parm : mode	- [in]  Sampling Mode 값 (0, 1)
						1: Chuck에서 Pins로 Wafer 이송 중에 Pins Position에서 멈추고,
						   다시 돌아갑니다. 이 Pins position에서, Chuck Vacuum이 Off되기 전에
						   Pins Vacuum은 On이 되어짐 (현재 수행단계까지 진행 후 멈춤?)
						0: Chuck은 Pins Position에서 정지하지 않고, Pins Vacuum이 On되기 전에
						   Chuck Vacuum은 Off 되어짐 (바로 멈춤?)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetStopOnPins(UINT8 mode)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_SP %u", UINT8(mode));
	return g_pMPAThread->SendData(ENG_PSCC::psc__sp, szCmd);
}

/*
 desc : 펌웨어에서 Substrate의 가장자리 코너 부분으로부터 가져올 수 있는 CCD Data의
		최소 / 최대 값을 출력
 parm : mode	- [in]  0x00: Min, 0x01: Max
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetSuqreValueOfCCD(ENG_MMMR mode)
{
	CHAR szCmd[2][8]	= { "_SQMIN", "_SQMAX" };
	ENG_PSCC enCmd[2]	= { ENG_PSCC::psc__sqmin, ENG_PSCC::psc__sqmax };
	return g_pMPAThread->SendData(enCmd[UINT8(mode)], szCmd[UINT8(mode)]);
}

/*
 desc : 펌웨어에서 Substrate의 가장자리 코너 부분으로부터 가져올 수 있는 CCD Data의
		최소 / 최대 값을 설정
 parm : mode	- [in]  0x00: Min, 0x01: Max
		area	- [in]  가져올 영역의 크기 (Square 즉, 가로/세로 동일한 크기) (단위: pixels)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetSuqreValueOfCCD(ENG_MMMR mode, UINT32 area)
{
	CHAR szCmd[32]		= {NULL};
	ENG_PSCC enCmd[2]	= { ENG_PSCC::psc__sqmin, ENG_PSCC::psc__sqmax };
	if (mode == ENG_MMMR::mmr_min)	sprintf_s(szCmd, 32, "_SQMIN %u", area);
	else							sprintf_s(szCmd, 32, "_SQMAX %u", area);
	return g_pMPAThread->SendData(enCmd[UINT8(mode)], szCmd);
}

/*
 desc : Chuck이 Pins에 방해받지 않도록 R (Horizontal) 축을 이동하는데,
		안전한 지점인 Pins 아래쪽 위치를 출력
		또는
		Chuck의 Wafer가 Pins 위에 있도록 R (Horizontal) 축을 이동하는데,
		안전한 지점인 Pins 위쪽 위치를 출력 (Wafer를 가지고 가기 위한 안전한 지점)
 parm : mode	- [in]  0x00: Transfer Down, 0x01: Transfer Up
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetWaferTranserSafePos(ENG_PWLP mode)
{
	CHAR szCmd[2][8]	= { "_TD", "_TU" };
	ENG_PSCC enCmd[2]	= { ENG_PSCC::psc__td, ENG_PSCC::psc__tu };
	return g_pMPAThread->SendData(enCmd[UINT8(mode)], szCmd[UINT8(mode)]);
}

/*
 desc : Chuck이 Pins에 방해받지 않도록 R (Horizontal) 축을 이동하는데,
		안전한 지점인 Pins 아래쪽 위치를 설정
		또는
		Chuck의 Wafer가 Pins 위에 있도록 R (Horizontal) 축을 이동하는데,
		안전한 지점인 Pins 위쪽 위치를 설정 (Wafer를 가지고 가기 위한 안전한 지점)
 parm : mode	- [in]  0x00: Transfer Down, 0x01: Transfer Up
		value	- [in]  Chuck의 위치 값 (단위: um)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetWaferTranserSafePos(ENG_PWLP mode, DOUBLE value)
{
	CHAR szCmd[32]		= {NULL};
	ENG_PSCC enCmd[2]	= { ENG_PSCC::psc__td, ENG_PSCC::psc__tu };
	UINT32 u32Value		= GetAxisPosDev(ENG_PANC::pac_z, value);
	if (mode == ENG_PWLP::wlp_load_down)	sprintf_s(szCmd, 32, "_TD %u", u32Value);
	else									sprintf_s(szCmd, 32, "_TU %u", u32Value);
	return g_pMPAThread->SendData(enCmd[UINT8(mode)], szCmd);
}

/*
 desc : Prealigner 동작 관련된 여타 다른 부분들의 동작 타임 아웃 값을 출력
 parm : param	- [in]  Parameter Number (0 ~ 10) (ENUM 형으로 정의하지 못했음. 넘 난해 함)
 retn : TRUE or FALSE
 참고 :	00 Timeout while waiting for vacuum to build on chuck when checking for a wafer.
		01 Timeout while waiting for vacuum to build on pins.
		02 Time to wait after R-axis movement during wafer transfer.
		03 Time to wait after Z-axis movement during wafer transfer.
		04 Time to wait at pins position during “Stop at pins” mode (_SP 1).
		05 Reserved.
		06 Time to wait after MVA, MVR and BAL command have finished before returning prompt.
		07 Delay between switching on pins vacuum and switching off chuck vacuum during transfer from chuck to pins.
		08 Delay between switching on chuck vacuum and switching off pins vacuum during transfer from pins to chuck.
		09 Delay before moving Z-axis to transfer down position during transfer.
		10 Delay before moving Z-axis to transfer down position transfer.
*/
API_EXPORT BOOL uvMPA_GetParamTimeOut(UINT8 param)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_TO %u", param);
	return g_pMPAThread->SendData(ENG_PSCC::psc__to, szCmd);
}

/*
 desc : Prealigner 동작 관련된 여타 다른 부분들의 동작 타임 아웃 값을 설정
 parm : param	- [in]  Parameter Number (0 ~ 10) (ENUM 형으로 정의하지 못했음. 넘 난해 함)
		value	- [in]  Timeout or Delay 값 (msec)
 retn : TRUE or FALSE
 참고 :	00 Timeout while waiting for vacuum to build on chuck when checking for a wafer.
		01 Timeout while waiting for vacuum to build on pins.
		02 Time to wait after R-axis movement during wafer transfer.
		03 Time to wait after Z-axis movement during wafer transfer.
		04 Time to wait at pins position during “Stop at pins” mode (_SP 1).
		05 Reserved.
		06 Time to wait after MVA, MVR and BAL command have finished before returning prompt.
		07 Delay between switching on pins vacuum and switching off chuck vacuum during transfer from chuck to pins.
		08 Delay between switching on chuck vacuum and switching off pins vacuum during transfer from pins to chuck.
		09 Delay before moving Z-axis to transfer down position during transfer.
		10 Delay before moving Z-axis to transfer down position transfer.
*/
API_EXPORT BOOL uvMPA_SetParamTimeOut(UINT8 param, UINT32 value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_TO %u %u", param, value);
	return g_pMPAThread->SendData(ENG_PSCC::psc__to, szCmd);
}

/*
 desc : 선택한 웨이퍼 크기와 함께 자동으로 사용할 매개 변수 집합 할당 (설정; Assigned)
		Wafer 크기가 변경될 때마다 기존 할당된 세트의 파라미터들이 작업 파라미터로 적재 됨
 parm : set		- [in]  Set Number (-1 or 1 or Later)
		wafer	- [in]  Wafer Size (2, 3, 4, 5, 6, 8, 12, 18)
 retn : TRUE or FALSE
 note : 이 명령은 선택한 웨이퍼 크기와 함께 자동으로 사용할 파라미터 세트 할당
		웨이퍼 크기가 변경될 때마다 할당된 세트의 파라미터가 작업 파라미터 로드
*/
API_EXPORT BOOL uvMPA_SetAssingAutoLoadWafer(INT8 set, ENG_MWKT wafer)
{
	CHAR szCmd[32]	= {NULL};
	if (set < 0)	strcpy_s(szCmd, 32, "_WSZAUTOSET -1");
	else			sprintf_s(szCmd, 32, "_WSZAUTOSET %u %u", set, UINT8(wafer));
	return g_pMPAThread->SendData(ENG_PSCC::psc__wszautoset, szCmd);
}

/*
 desc : Wafer Type 얻기
 parm : param	- [in]  Parameter Number (0 ~ 10) (ENUM 형으로 정의하지 못했음. 넘 난해 함)
 retn : TRUE or FALSE
 note : -2 – Wafer with a notch detected by an external sensor.
		-1 – Wafer without notch or flat (these wafers are not oriented, only centered).
		 0 – Wafer with a notch.
		 1 – Wafer with one or more flats.
		 2 – Square substrate.
*/
API_EXPORT BOOL uvMPA_GetWaferType()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc__wt, "_WT");
}

/*
 desc : Wafer Type 설정
 parm : type	- [in]  Wafer Type (-2, -1, 0, 1, 2)
 retn : TRUE or FALSE
 note : -2 – Wafer with a notch detected by an external sensor.
		-1 – Wafer without notch or flat (these wafers are not oriented, only centered).
		 0 – Wafer with a notch.
		 1 – Wafer with one or more flats.
		 2 – Square substrate.
*/
API_EXPORT BOOL uvMPA_SetWaferType(ENG_PGWT type)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_WT %d", INT8(type));
	return g_pMPAThread->SendData(ENG_PSCC::psc__wt, szCmd);
}

/*
 desc : Prealigner에 저장되어 있는 "Performance Set"을 파일 ("perform.ini")에서 읽어들인 후
		기존 "Performance Set"에 덮어쓰고, 활성화 시킴
 parm : set_no	- [in]  Performance Set Number
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_RestorePerformSetFile(UINT8 set_no)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "LPS %u", set_no);
	return g_pMPAThread->SendData(ENG_PSCC::psc_lps, szCmd);
}

/*
 desc : 현재 Prealigner에 설정된 모든 파라미터들의 설정된 값을 파일 ("paramset.ini")에 저장
		기존 파일에 덮어쓰는 구조 임
 parm : set_no	- [in]  Performance Set Number
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_WritePerformSetFile(UINT8 set_no)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "WPS %u", set_no);
	return g_pMPAThread->SendData(ENG_PSCC::psc_wps, szCmd);
}

/*
 desc : 이전 Prealigner에 저장된 모든 파라미터들의 설정된 값을 파일 ("paramset.ini")에서 불러옴
		기존 설정된 파라미터들의 값은 모두 사라짐
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_RestoreParamSetFile()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_rps, "RPS");
}

/*
 desc : 현재 Prealigner에 설정된 모든 파라미터들의 설정된 값을 파일 ("paramset.ini")에 저장
		기존 파일에 덮어쓰는 구조 임
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SaveParamSetFile()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_sps, "SPS");
}

/*
 desc : 이전 Prealigner에 저장된 모든 파라미터들의 설정된 값을 비메모리 ("Non-volatile") 영역에서 불러옴
		기존 설정된 파라미터들의 값은 모두 사라짐
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_RestoreParamSetMemory()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_rmp, "RMP");
}

/*
 desc : 현재 Prealigner에 설정된 모든 파라미터들의 설정된 값을 비메모리 ("Non-volatile") 영역에 저장
		기존에 저장된 정보는 사라짐 (덮어써 버림)
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SaveParamSetMemory()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_smp, "SMP");
}

/*
 desc : 현재 Prealigner의 시스템 시간 얻기
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetSystemTime()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc__dt, "_DT");
}

/*
 desc : 현재 Prealigner의 시스템 시간 설정 (현재 시간 설정)
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetSystemTime()
{
	CHAR szCmd[64]	= {NULL};
	SYSTEMTIME stDT	= {NULL};

	GetLocalTime(&stDT);
	sprintf_s(szCmd, 64, "_DT %04u%02u%02u %02u%02u%02u",
			  stDT.wYear, stDT.wMonth, stDT.wDay,
			  stDT.wHour, stDT.wMinute, stDT.wSecond);

	return g_pMPAThread->SendData(ENG_PSCC::psc__dt, szCmd);
}

/*
 desc : 원점 복귀 궤적 매개 변수를 모든 축에 할당 즉, 속도, 가속 및 감속도 설정
 parm : mode	- [in]  0x00:HSA (Homing Mode), 0x01:WSA (Working Mode)
 retn : TRUE or FALSE
 note : Homing 할 때, 적용됨 (?: QnA 보냈지만, 답변 없음)
		현재 각 축들의 속도 관련 설정된 정보 값을 그대로 반영
		(SPD 명령을 통해서 현재 각 축들의 속도 정보 확인할 수 있음)
*/
API_EXPORT BOOL uvMPA_RestoreTrajectorySpeedAcc(UINT8 mode)
{
	CHAR szCmd[2][8]	= { "HSA", "WSA" };
	ENG_PSCC enCmd[2]	= { ENG_PSCC::psc_hsa, ENG_PSCC::psc_wsa };
	return g_pMPAThread->SendData(enCmd[mode], szCmd[mode]);
}

/*
 desc : 원점 복귀 궤적 매개 변수를 모든 축에 할당 즉, 속도, 가속 및 감속도 설정
 parm : None
 retn : TRUE or FALSE
 note : Homing 할 때, 적용됨 (?: QnA 보냈지만, 답변 없음)
		현재 각 축들의 속도 관련 설정된 정보 값을 그대로 반영
		(SPD 명령을 통해서 현재 각 축들의 속도 정보 확인할 수 있음)
*/
API_EXPORT BOOL uvMPA_RestoreHomeSpeedAcc()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_hsa, "HSA");
}

/*
 desc : 현재 Prealigner의 장비를 초기화 (Reset; Turn off -> Turn on)
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_ResetPrealigner()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_res, "RES");
}

/*
 desc : Prealigner가 정렬 (BAL Command) 작업이 실패한 경우, CCD Data를 포함하고 있는 파일을
		자동으로 생성할지 여부 값 얻기
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetAlignFailAutoDumps()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc__autodumps, "_AUTODUMPS");
}

/*
 desc : Prealigner가 정렬 (BAL Command) 작업이 실패한 경우, CCD Data를 포함하고 있는 파일을
		자동으로 생성할지 여부 값 얻기
 parm : mode	- [in]  0x00: 자동 생성 안함, 0x01: 자동 생성 함
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetAlignFailAutoDumps(UINT8 mode)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_AUTODUMPS %u", mode);
	return g_pMPAThread->SendData(ENG_PSCC::psc__autodumps, szCmd);
}

/*
 desc : “_AUTODUMPS” 모드 값이 “1”로 설정된 경우, 유효한 명령어
		생성하고자 하는 CCD Samples 파일에 대한 File Numbering 값 반환
 parm : None
 retn : TRUE or FALSE
 note : 파일 이름은 “AUTOS”와 File Number로 구성되고, 확장자는 “CSV”로 구성
		ex> AUTOS1.CSV, AUTOS2.CSV, …, AUTOS9.CSV
		파일이 생성되고 난 이후, File Number는 자동으로 증가.
		즉, 정렬 (BAL Command) 명령 후 Align 결과가 실패한 경우, File Number가 자동으로 증가
		만약 File Number가 9 넘어가면, 자동으로 0으로 바뀌는지는 모름?  문의 ?
		그래서, “BAL” Command어 실행하기 전에 항상 “_AUTODUMPFS 0” 명령어 호출해줘야 됨?
*/
API_EXPORT BOOL uvMPA_GetAlignFailAutoDumpFile()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc__autodumpsf, "_AUTODUMPSF");
}

/*
 desc : “_AUTODUMPS” 모드 값이 “1”로 설정된 경우, 유효한 명령어
		생성하고자 하는 CCD Samples 파일에 대한 File Numbering 값 설정
 parm : file_no	- [in]  File Numbering (0 ~ 9)
 retn : TRUE or FALSE
 note : 파일 이름은 “AUTOS”와 File Number로 구성되고, 확장자는 “CSV”로 구성
		ex> AUTOS1.CSV, AUTOS2.CSV, …, AUTOS9.CSV
		파일이 생성되고 난 이후, File Number는 자동으로 증가.
		즉, 정렬 (BAL Command) 명령 후 Align 결과가 실패한 경우, File Number가 자동으로 증가
		만약 File Number가 9 넘어가면, 자동으로 0으로 바뀌는지는 모름?  문의 ?
		그래서, “BAL” Command어 실행하기 전에 항상 “_AUTODUMPFS 0” 명령어 호출해줘야 됨?
*/
API_EXPORT BOOL uvMPA_SetAlignFailAutoDumpFile(UINT8 file_no)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_AUTODUMPSF %u", file_no);
	return g_pMPAThread->SendData(ENG_PSCC::psc__autodumpsf, szCmd);
}

/*
 desc : Main Prealigner Calibration 절차 초기화 (개시) 진행
		Chuck 혹은 Pins의 교체 (재배치)와 같이 Prealigner에서 기구(기계)적인 변경
		(ex> 교체, 위치 조정 혹은 수정 등)가 있었을 경우, 이 명령어 사용
 parm : None
 retn : TRUE or FALSE
 note : Prealigner에 의해 지원되는 비-투명 3, 4, 8, 12 혹은 18 인치를 이용하여,
		이 명령을 실행해야 함
*/
API_EXPORT BOOL uvMPA_SetPhysicalChangeCalibration()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc__cal, "_CAL");
}

/*
 desc : CCD 보상 계수 (“_SH” and “_SL”) 그리고 현재 Wafer에 대한
		CCD 중심의 보상 (Calibration)을 초기화 (시작) 수행
		Light House 높이가 변경 혹은 Z 축 측정 위치 (“MMTM T/R/Z”)가 변경된 경우, 이 명령을 수행 추천
 parm : mode1	- [in]  use small iteration steps (알고리즘 성격 정의)
						0x00: 빠른 보정 알고리즘
						0x01: 느린 보정 알고리즘 (대신 정밀한 검사 수행)
		mode2	- [in]  CCD 중심 검색 여부
 retn : TRUE or FALSE
 note : Wafer가 Prealigner에 놓여져 있는 상태에서 수행되며
		Calibration된 파라미터 값들이 저장되지 않으므로, “SMP” 명령을 통해서 수동으로 저장
*/
API_EXPORT BOOL uvMPA_SetPhysicalChangeCompensation(UINT8 mode1, UINT8 mode2)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_CAL_COR %u %u", mode1, mode2);
	return g_pMPAThread->SendData(ENG_PSCC::psc__cal_cor, szCmd);
}

/*
 desc : CCD Samples를 마지막 검사에서 “samples.BAL”인 파일로 저장
		(파일 이름 끝에 임의의 Numbering이 붙음)
 parm : file_no	- [in]  File Numbering (0 ~ 9)
 retn : TRUE or FALSE
 note : 이 파일은 제조 쪽에 Alignment Algorithm의 상세 문제 (Troubleshooting)에 대해
		알리고자 할 때 사용됨 (이 명령을 실행하기 앞서, “DRPS” 명령이 실행되어져야 함)
*/
API_EXPORT BOOL uvMPA_SetDetailTroubleFileByManufacture(UINT8 file_no)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "DDPF %u", file_no);
	return g_pMPAThread->SendData(ENG_PSCC::psc_ddpf, szCmd);
}

/*
 desc : CCD Samples를 마지막 검사에서 “samples.CSV”인 파일로 저장
		(파일 이름 끝에 임의의 Numbering이 붙음)
 parm : file_no	- [in]  File Numbering (0 ~ 9)
 retn : TRUE or FALSE
 note : 이 파일은 제조 쪽 혹은 서비스 직원에 Alignment Algorithm의 기본 문제 (Troubleshooting)에 대해
		알리고자 할 때 사용됨
*/
API_EXPORT BOOL uvMPA_SetBasicTroubleFileByManufacture(UINT8 file_no)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "DDUMPSAMPLES %u", file_no);
	return g_pMPAThread->SendData(ENG_PSCC::psc_ddpf, szCmd);
}

/*
 desc : 이 명령은 마지막 데이터 수집 실행 중에 얻어진 samples를 내부 메모리로 읽어들이고,
		 “DDPF” 명령을 사용하기 위해 samples를 준비해 놓음 즉, 읽어들인 샘플의 개수 반환
 parm : None
 retn : TRUE or FALSE
 */
API_EXPORT BOOL uvMPA_SetTroubleDumpToMemory()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc_drps, "DRPS");
}

/*
 desc : 현재 Prealigner의 Edge 조작 여부 (Edge Handling 여부)
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetEdgeHandleType()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc__eh, "_EH");
}

/*
 desc : Target -> Chuck Pins
		Edge Handling Prealigner의 척이 Sensor를 덮을 때, 최대 CCD 값 지정
		CCD Sensor가 검사 (Scanning) 중일 때, 이 파라미터 (CCD Values)
		하위 (아래)에 있는 값일 경우 Wafer가 없는 것으로 판단
		(Sensing 거리 값. Wafer를 감지하기 위한 분포 값)
 parm : type	- [in]  0x00: 일반 Wafer Size, 0x01: Smaller Wafer Size, 0x02: Greater Wafer Size
 retn : TRUE or FALSE
 note : Some edge handling prealigners support two different wafer sizes.
		_ECH  : This parameter specifies the maximum CCD value when only the chuck of
				an edge handling prealigner covers the sensor
		_EHC1 : This command sets the chuck CCD value of the smaller size.
		_EHC2 : This command sets the chuck CCD value of the larger size.
*/
API_EXPORT BOOL uvMPA_GetCCDChuckSensingDist(UINT8 type)
{
	CHAR szCmd[3][8]	= { "_EHC", "_EHC1", "_EHC2" };
	ENG_PSCC enCmd[3]	= { ENG_PSCC::psc__ehc, ENG_PSCC::psc__ehc1, ENG_PSCC::psc__ehc2 };
	return g_pMPAThread->SendData(enCmd[UINT8(type)], szCmd[UINT8(type)]);
}

/*
 desc : Target -> Chuck Pins
		Edge Handling Prealigner의 척이 Sensor를 덮을 때, 최대 CCD 값 지정
		CCD Sensor가 검사 (Scanning) 중일 때, 이 파라미터 (CCD Values)
		하위 (아래)에 있는 값일 경우 Wafer가 없는 것으로 판단
		(Sensing 거리 값. Wafer를 감지하기 위한 분포 값)
 parm : type	- [in]  0x00: 일반 Wafer Size, 0x01: Smaller Wafer Size, 0x02: Greater Wafer Size
		pixel	- [in]  Sensing 거리 값 (단위: pixels)
 retn : TRUE or FALSE
 참고 :	이 명령을 설정된 파라미터 값은 “SMP” 명령어에 의해 내부 메모리에 저장 됨
*/
API_EXPORT BOOL uvMPA_SetCCDChuckSensingDist(UINT16 pixel, UINT8 type)
{
	CHAR szCmd[32]		= {NULL};
	ENG_PSCC enCmd[3]	= { ENG_PSCC::psc__ehc, ENG_PSCC::psc__ehc1, ENG_PSCC::psc__ehc2 };
	switch (type)
	{
	case 0x00 : sprintf_s(szCmd, 32, "_EHC %u", pixel);
	case 0x01 : sprintf_s(szCmd, 32, "_EHC1 %u", pixel);
	case 0x02 : sprintf_s(szCmd, 32, "_EHC2 %u", pixel);
	}
	return g_pMPAThread->SendData(enCmd[UINT8(type)], szCmd);
}

/*
 desc : Target -> Chuck Pins
		Edge Handling Prealigner의 척이 Sensor를 덮을 때, 최대 CCD 값 지정
		CCD Sensor가 검사 (Scanning) 중일 때, 이 파라미터 (CCD Values)
		하위 (아래)에 있는 값일 경우 Wafer가 없는 것으로 판단
		(Sensing 거리 값. Wafer를 감지하기 위한 분포 값)
 parm : type	- [in]  0x00: 일반 Wafer Size, 0x01: Smaller Wafer Size, 0x02: Greater Wafer Size
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetCCDPinsSensingDist(UINT8 type)
{
	CHAR szCmd[3][8]	= { "_EHP", "_EHP1", "_EHP2" };
	ENG_PSCC enCmd[3]	= { ENG_PSCC::psc__ehp, ENG_PSCC::psc__ehp1, ENG_PSCC::psc__ehp2 };
	return g_pMPAThread->SendData(enCmd[UINT8(type)], szCmd[UINT8(type)]);
}

/*
 desc : Target -> Chuck Pins
		Edge Handling Prealigner의 척이 Sensor를 덮을 때, 최대 CCD 값 지정
		CCD Sensor가 검사 (Scanning) 중일 때, 이 파라미터 (CCD Values)
		하위 (아래)에 있는 값일 경우 Wafer가 없는 것으로 판단
		(Sensing 거리 값. Wafer를 감지하기 위한 분포 값)
 parm : type	- [in]  0x00: 일반 Wafer Size, 0x01: Smaller Wafer Size, 0x02: Greater Wafer Size
		pixel	- [in]  Sensing 거리 값 (단위: pixels)
 retn : TRUE or FALSE
 참고 :	이 명령을 설정된 파라미터 값은 “SMP” 명령어에 의해 내부 메모리에 저장 됨
*/
API_EXPORT BOOL uvMPA_SetCCDPinsSensingDist(UINT16 pixel, UINT8 type)
{
	CHAR szCmd[32]		= {NULL};
	ENG_PSCC enCmd[3]	= { ENG_PSCC::psc__ehp, ENG_PSCC::psc__ehp1, ENG_PSCC::psc__ehp2 };

	/* 송신 후 응답 확인 (만약에 응답이 올 때까지 장시간 대기하면 어쩌나...) */
	switch (type)
	{
	case 0x00 : sprintf_s(szCmd, 32, "_EHP %u", pixel);
	case 0x01 : sprintf_s(szCmd, 32, "_EHP1 %u", pixel);
	case 0x02 : sprintf_s(szCmd, 32, "_EHP2 %u", pixel);
	}
	return g_pMPAThread->SendData(enCmd[UINT8(type)], szCmd);
}

/*
 desc : Edge Handling Prealigner의 Chuck 위에서 수직 축을 따라 고정된 Pins 위에 안전하게
		회전할 수 있는 위치를 출력
		즉, Wafer가 안전하게 회전할 수 있는 수직 (Z; Vertical)의 위치를 설정 내지 출력해 줌
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_GetSafeRotateAxisZ()
{
	return g_pMPAThread->SendData(ENG_PSCC::psc__sfz, "_SFZ");
}

/*
 desc : Edge Handling Prealigner의 Chuck 위에서 수직 축을 따라 고정된 Pins 위에 안전하게
		회전할 수 있는 위치를 설정
		즉, Wafer가 안전하게 회전할 수 있는 수직 (Z; Vertical)의 위치를 설정 내지 출력해 줌
 parm : value	- [in]  안전하게 회전할 Z 축 위치 (단위: um)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_SetSafeRotateAxisZ(DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	UINT32 u32Value	= GetAxisPosDev(ENG_PANC::pac_z, value);
	sprintf_s(szCmd, 32, "_SFZ %u", u32Value);
	return g_pMPAThread->SendData(ENG_PSCC::psc__sfz, szCmd);
}

/* --------------------------------------------------------------------------------------------- */
/*                                            Common                                             */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 가장 최근에 송신한 명령에 대해 수신된 패킷이 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_IsLastSendAckRecv()
{
	return g_pMPAThread->IsLastSendAckRecv();
}

/*
 desc : 가장 최근에 송신한 명령에 대해 수신 응답이 성공 했는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_IsLastSendAckSucc()
{
	return g_pMPAThread->IsLastSendAckSucc();
}

/*
 desc : 시스템 상태 값에 대한 에러 메시지 반환
 parm : mesg	- [out] 에러 코드에 대한 상세 설명이 저장될 반환 버퍼
		size	- [in]  'mesg' 버퍼 크기
 retn : FALSE (에러 없음), TRUE (에러 있음)
 note : "mesg" 버퍼를 최소 1024만큼 설정해 줘야 됨
*/
API_EXPORT BOOL uvMPA_GetSystemStatusMesg(TCHAR *mesg, UINT32 size)
{
	return g_pMPAThread->GetSystemStatus(mesg, size);
}

/*
 desc : 이전에 송신된 명령어의 에러 값 (코드)에 대한 상세 문자열 정보 반환
 parm : mesg	- [out] 에러 코드에 대한 상세 설명이 저장될 반환 버퍼
		size	- [in]  'mesg' 버퍼 크기
 retn : FALSE (에러 없음), TRUE (에러 있음)
*/
API_EXPORT BOOL uvMPA_GetPreviousError(TCHAR *mesg, UINT32 size)
{
	return g_pMPAThread->GetPreviousError(mesg, size);
}

/*
 desc : 가장 최근에 수신된 데이터가 있는지 여부
 parm : cmd	- [in]  검사 대상 명령어
		r_cmd	- [out] 가장 최근에 수신된 명령어가 저장될 버퍼
		size	- [in]  'r_cmd' 버퍼의 크기
 retn : TRUE or FALSE
 note : 만약, 'cmd' 값에 값을 입력하지 않으면, 가장 최근에 송신한 명령어에 대해, 수신된 값이 존재하는지 여부
*/
API_EXPORT BOOL uvMPA_IsRecvCmdData(ENG_PSCC cmd)
{
	/* 가장 최근에 송신한 명령어에 대한 응답이 도착했는지 여부 확인 */
	if (!g_pMPAThread->IsLastSendAckRecv())	return FALSE;

	/* 어떠한 명령어도 입력되지 않았다면, 가장 최근에 송신한 명령어에 대한 수신 결과 확인 */
	if (cmd == ENG_PSCC::psc_none)	cmd	= g_pMPAThread->GetLastSendCmd();

	if (cmd <= ENG_PSCC::psc_ver)				return IsRecvStatusInfoCmd(cmd);					/* 1. Status and Information Commands */
	else if (cmd <= ENG_PSCC::psc_spd)			return IsRecvAxisParamSetRetrieveCmd(cmd);			/* 2. Axis Param eters Set and Retrieve */
	else if (cmd <= ENG_PSCC::psc_wmc)			return IsRecvBasicControlCmd(cmd);					/* 3. Basic Axis Control */
	else if (cmd <= ENG_PSCC::psc_wsz2)			return IsRecvAlignmentCmd(cmd);					/* 4. Alignment Commands */
	else if (cmd <= ENG_PSCC::psc_wps)			return IsRecvHousekeepingCmd(cmd);					/* 5. Housekeeping Commands */
	else if (cmd <= ENG_PSCC::psc_drps)			return IsRecvCaliSvcCmd(cmd);						/* 6. Calibration and Service Commands */
	else if (cmd <= ENG_PSCC::psc__sfz)			return IsRecvEdgeHandlingPreAlignerSpecCmd(cmd);	/* 7. Edge Handling Prealigner-Specific Commands */
	else if (cmd <= ENG_PSCC::psc__notchnavg)	return IsRecvExoticCmd(cmd);						/* 8. Exotic Commands */
	else if (cmd <= ENG_PSCC::psc_res)			return IsRecvMiscCmd(cmd);							/* 9. Miscellaneous Commands */

	return FALSE;
}
API_EXPORT BOOL uvMPA_IsRecvCmdLast()
{
	return uvMPA_IsRecvCmdData(g_pMPAThread->GetLastSendCmd());
}
API_EXPORT BOOL uvMPA_GetRecvCmdLast(PTCHAR r_cmd, UINT32 size)
{
	CUniToChar csCnv;
	/* 가장 최근에 송신한 명령어에 대한 응답이 도착했는지 여부 확인 */
	if (!g_pMPAThread->IsLastSendAckRecv())	return FALSE;
	wcscpy_s(r_cmd, size, csCnv.Ansi2Uni(g_pMPAThread->GetLastRecvPkt()));

	return TRUE;
}

/*
 desc : 장비 (시스템) 내부에 에러가 발생 했는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMPA_IsSystemError()
{
	/*  In case of error : 0x0001 + 0x0002 + 0x0010 + 0x0020 + 0x0040 + 0x0400 + 0x2000 */
	if (g_pstShMemMPA->system_status == 0xffff)				return FALSE;
	if ((g_pstShMemMPA->system_status & 0x2473) == 0x0000)	return FALSE;
#if 0	/* old version */
	if (0x0001 == (g_pstShMemMPA->system_status & 0x0001))	LOG_ERROR(ENG_EDIC::en_lspa, L"STA(0001): Unable to execute motion commands");
	if (0x0002 == (g_pstShMemMPA->system_status & 0x0002))	LOG_ERROR(ENG_EDIC::en_lspa, L"STA(0002): Previous command returned an error. See PER command for details");
	if (0x0010 == (g_pstShMemMPA->system_status & 0x0010))	LOG_ERROR(ENG_EDIC::en_lspa, L"STA(0010): Motion error"); 
	if (0x0020 == (g_pstShMemMPA->system_status & 0x0020))	LOG_ERROR(ENG_EDIC::en_lspa, L"STA(0020): Motion limits violation");
	if (0x0040 == (g_pstShMemMPA->system_status & 0x0040))	LOG_ERROR(ENG_EDIC::en_lspa, L"STA(0040): Home procedure not executed");
	if (0x0400 == (g_pstShMemMPA->system_status & 0x0400))	LOG_ERROR(ENG_EDIC::en_lspa, L"STA(0400): Servo OFF on one or more axes"); 
	if (0x2000 == (g_pstShMemMPA->system_status & 0x2000))	LOG_ERROR(ENG_EDIC::en_lspa, L"STA(2000): I/O File error");
#else /* new version */
	if (0x0001 == (g_pstShMemMPA->system_status & 0x0001))	LOG_ERROR(ENG_EDIC::en_lspa, L"STA(0001): Unable to execute motion commands");
	if (0x0002 == (g_pstShMemMPA->system_status & 0x0002))	LOG_ERROR(ENG_EDIC::en_lspa, L"STA(0002): Previous command returned an error. See PER command for details");
	if (0x0010 == (g_pstShMemMPA->system_status & 0x0010))	LOG_ERROR(ENG_EDIC::en_lspa, L"STA(0010): Motion error");
	if (0x0020 == (g_pstShMemMPA->system_status & 0x0020))	LOG_ERROR(ENG_EDIC::en_lspa, L"STA(0020): Motion limits violation");
	if (0x0040 == (g_pstShMemMPA->system_status & 0x0040))	LOG_ERROR(ENG_EDIC::en_lspa, L"STA(0040): Home procedure not executed");
	if (0x0400 == (g_pstShMemMPA->system_status & 0x0400))	LOG_ERROR(ENG_EDIC::en_lspa, L"STA(0400): Servo OFF on one or more axes");
	if (0x2000 == (g_pstShMemMPA->system_status & 0x2000))	LOG_ERROR(ENG_EDIC::en_lspa, L"STA(0800): Reserved");
	if (0x2000 == (g_pstShMemMPA->system_status & 0x2000))	LOG_ERROR(ENG_EDIC::en_lspa, L"STA(1000): Reserved");
	if (0x2000 == (g_pstShMemMPA->system_status & 0x2000))	LOG_ERROR(ENG_EDIC::en_lspa, L"STA(2000): Reserved");
	if (0x2000 == (g_pstShMemMPA->system_status & 0x2000))	LOG_ERROR(ENG_EDIC::en_lspa, L"STA(4000): Reserved");
	if (0x2000 == (g_pstShMemMPA->system_status & 0x2000))	LOG_ERROR(ENG_EDIC::en_lspa, L"STA(5000): Reserved");
#endif
	return TRUE;
}

#ifdef __cplusplus
}
#endif