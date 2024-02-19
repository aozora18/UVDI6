
/*
 desc : Diamond Robot Library for Logosol with serial
*/

#include "pch.h"
#include "MainApp.h"

#include "MDRThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/* ------------------------------------------------------------------------------------------------
 단위 : T 축 단위		- 1 = 0.01°, 전체 회전하려면 36000 값 입력 (시계 방향 : +, 반시계 방향 : -)
		R1/R2 축 단위	- 1 = 0.001 inch = 25.4 um = 25400 nm
		Z 축 단위		- 1 = 0.001 inch = 25.4 um = 25400 nm
 범위 : T 축 회전 각도	- -210.00° ~ +210.00°
		R1/R2 축 거리	- -14.400" ~ +14.400" (-36.576 cm ~ +36.576 cm)
		Z 축 거리		- - 0.500" ~ +12.200" (-1.27 cm ~ +30.988 cm)
------------------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------ */
/*                                      전역 변수                                       */
/* ------------------------------------------------------------------------------------ */

LPG_MRDV					g_pstShMemMDR	= NULL;
CMDRThread					*g_pMDRThread	= NULL;		/* 작업 스레드 */

/* ------------------------------------------------------------------------------------ */
/*                                      내부 함수                                       */
/* ------------------------------------------------------------------------------------ */

/*
 desc : 각 축(R, Z, T)의 코드 값을 1 바이트의 문자 값으로 반환
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		name	- [out] Axis Name (반환 버퍼)
		size	- [in]  'name' 버퍼 크기 (최소 4 바이트 이상)
 retn : TRUE or FALSE
 */
BOOL GetAxisChar(ENG_RANC axis, PCHAR name, UINT32 size)
{
	CHAR szAxis[6][8] = { "T", "R1", "R2", "Z", "K", "A" };

	/* 버퍼 유효성 확인 */
	if (!name)		return FALSE;
	if (size < 3)
	{
		LOG_WARN(ENG_EDIC::en_lsdr, L"Invalid parameter : GetAxisChar (name, size >= 4)");
		return FALSE;
	}

	/* 반환 버퍼 초기화 */
	memset(name, 0x00, size);

	if (UINT8(axis) > 0x05)
	{
		strcpy_s(name, size, "T");	/* 잘못된 Drive 축 값이 오게 되면 무조건 'T' 문자 반환 */
		return TRUE;
	}

	/* 정상적인 문자열 축 이름 반환 */
	strcpy_s(name, size, szAxis[UINT8(axis)]);

	return TRUE;
}

/*
 desc : 각 축(R, Z, T) 중 입력된 값을 장비에 설정되는 값으로 변환
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		value	- [in]  User Input Value (Axis에 따라 단위도 다르고, 계산식도 다름)
 retn : 값 반환 (실수형 um or degree --> 정수형 inch or degree)
 */
INT32 GetAxisPosDev(ENG_RANC axis, DOUBLE value)
{
	DOUBLE dbValue		= 0.0f;
	DOUBLE dbInchToUm	= 2.54 /* 1 inch -> 2.54 cm */ * 10000 /* cm -> um */;

	/* 입력되는 값에 따라 단위 변환이 다름 */
	switch (axis)
	{
	case ENG_RANC::rac_t	:	dbValue = value / 0.01;		break;	/* 1 --> 0.01 degrees */
	case ENG_RANC::rac_r1	:
	case ENG_RANC::rac_r2	:
	case ENG_RANC::rac_z	:	dbValue = (value / dbInchToUm) /* inch */ * 1000.0f  /* 1 --> 0.001 inch */;	break;
	}

	return (INT32)ROUNDED(dbValue, 0);
}

/*
 desc : Motion이 현재 멈춘 (Idle) 상태인지 여부
 parm : None
 retn : TRUE or FALSE
*/
BOOL IsMoveCompleted()
{
	return TRUE;
}

/*
 desc : 가장 최근에 수신된 데이터가 있는지 여부
 parm : cmd		- [in]  검사 대상 명령어
		axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
 retn : TRUE or FALSE
*/
BOOL IsRecvDataInfoCmds(ENG_RSCC cmd, ENG_RANC axis)
{
	switch (cmd)
	{
	case ENG_RSCC::rsc__dt		:
	case ENG_RSCC::rsc__si		:
	case ENG_RSCC::rsc_res		:
	case ENG_RSCC::rsc_ver		: return (strlen(g_pstShMemMDR->last_string_data) != 0);
	case ENG_RSCC::rsc_snr		: return (strlen(g_pstShMemMDR->serial_number) != 0);
	}
	return FALSE;
}
BOOL IsRecvHousekeepCmds(ENG_RSCC cmd, ENG_RANC axis)
{
	return FALSE;
}
BOOL IsRecvStatusDiagCmds(ENG_RSCC cmd, ENG_RANC axis)
{
	switch (cmd)
	{
	case ENG_RSCC::rsc___cs		:
	case ENG_RSCC::rsc_diag		:
	case ENG_RSCC::rsc_per		:
	case ENG_RSCC::rsc_est		:
	case ENG_RSCC::rsc_esta		: return (strlen(g_pstShMemMDR->last_string_data) > 0);
	case ENG_RSCC::rsc_cpo		: return (g_pstShMemMDR->axis_current_position[UINT8(axis)] != DBL_MAX);
	case ENG_RSCC::rsc_eerr		: return (g_pstShMemMDR->extended_error_code != 0xffff);
	case ENG_RSCC::rsc_inf		: return (g_pstShMemMDR->info_mode != 0xff);
	case ENG_RSCC::rsc_rhs		: return (g_pstShMemMDR->home_switch_state != 0x0000);
	case ENG_RSCC::rsc_sta		: return (g_pstShMemMDR->system_status != 0xffff);
	}
	return FALSE;
}
BOOL IsRecvAbsEncCmds(ENG_RSCC cmd, ENG_RANC axis)
{
	UINT8 i	= 0x00;
	switch (cmd)
	{
	case ENG_RSCC::rsc__aest	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->enc_status[i] == -1)	return FALSE;	break;
	case ENG_RSCC::rsc__eo		:
	case ENG_RSCC::rsc__eor		: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->enc_zero_offset[i] == DBL_MAX)	return FALSE;	break;
	case ENG_RSCC::rsc__eo_t	: return (g_pstShMemMDR->enc_zero_offset[0] != DBL_MAX);
	case ENG_RSCC::rsc__eo_r1	: return (g_pstShMemMDR->enc_zero_offset[1] != DBL_MAX);
	case ENG_RSCC::rsc__eo_r2	: return (g_pstShMemMDR->enc_zero_offset[2] != DBL_MAX);
	case ENG_RSCC::rsc__eo_z	: return (g_pstShMemMDR->enc_zero_offset[3] != DBL_MAX);
	case ENG_RSCC::rsc__ralmc	: return (strlen(g_pstShMemMDR->last_string_data) != 0);
	}
	return FALSE;
}
BOOL IsRecvBasicActCmds(ENG_RSCC cmd, ENG_RANC axis)
{
	switch (cmd)
	{
	case ENG_RSCC::rsc_cstp		: return (g_pstShMemMDR->controlled_stop[0] != 0xff);
	case ENG_RSCC::rsc_din		: return (g_pstShMemMDR->external_di_in_state != 0xff);
	case ENG_RSCC::rsc_dout		: return (g_pstShMemMDR->external_di_out_state != 0xff);
	case ENG_RSCC::rsc_inp		: return (g_pstShMemMDR->digital_input_state != 0xffff);
	case ENG_RSCC::rsc_lenb		: return (g_pstShMemMDR->loaded_speed_enable != 0xff);
	case ENG_RSCC::rsc_out		: return (g_pstShMemMDR->digital_output_state != 0xffff);
	case ENG_RSCC::rsc_vst_t	: return (g_pstShMemMDR->end_effector_wafer_check[0] != 0xff);
	case ENG_RSCC::rsc_vst_b	: return (g_pstShMemMDR->end_effector_wafer_check[1] != 0xff);
	case ENG_RSCC::rsc__bb		: return (g_pstShMemMDR->backup_dc_power[0] != 0xffff);
	}
	return FALSE;
}
BOOL IsRecvBasicMotionParams(ENG_RSCC cmd, ENG_RANC axis)
{
	UINT8 i	= 0x00;
	switch (cmd)
	{
	case ENG_RSCC::rsc__ch	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->homed_position[i] == DBL_MAX)					return FALSE;	break;
	case ENG_RSCC::rsc__cl	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->current_limit[i] == DBL_MAX)						return FALSE;	break;
	case ENG_RSCC::rsc__el	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->position_error_limit[i] == INT32_MAX)			return FALSE;	break;
	case ENG_RSCC::rsc__fl	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->sw_positive_limit[i] == DBL_MAX)					return FALSE;	break;
	case ENG_RSCC::rsc__fse	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->axis_safe_envelop_forward[i] == DBL_MAX)			return FALSE;	break;
	case ENG_RSCC::rsc__hd	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->axis_diff_coeff_hold[i] == UINT32_MAX)			return FALSE;	break;
	case ENG_RSCC::rsc__he	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->pos_err_limit_hold[i] == DBL_MAX)				return FALSE;	break;
	case ENG_RSCC::rsc__hi	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->axis_inte_coeff_hold[i] == DBL_MAX)				return FALSE;	break;
	case ENG_RSCC::rsc__hl	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->axis_inte_limit_hold[i] == DBL_MAX)				return FALSE;	break;
	case ENG_RSCC::rsc__hp	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->axis_prop_coeff_hold[i] == UINT32_MAX)			return FALSE;	break;
	case ENG_RSCC::rsc__il	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->axis_integral_limit[i] == DBL_MAX)				return FALSE;	break;
	case ENG_RSCC::rsc__kd	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->axis_diff_coeff[i] == UINT32_MAX)				return FALSE;	break;
	case ENG_RSCC::rsc__kp	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->axis_prop_coeff[i] == UINT32_MAX)				return FALSE;	break;
	case ENG_RSCC::rsc__ki	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->axis_inte_coeff[i] == UINT32_MAX)				return FALSE;	break;
	case ENG_RSCC::rsc__rl	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->sw_negative_limit[i] == DBL_MAX)					return FALSE;	break;
	case ENG_RSCC::rsc__rse	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->axis_safe_envelop_reverse[i] == DBL_MAX)			return FALSE;	break;
	case ENG_RSCC::rsc_acl	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->axis_acceleration[i] == DBL_MAX)					return FALSE;	break;
	case ENG_RSCC::rsc_ajk	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->axis_acceleration_jerk[i] == UINT16_MAX)			return FALSE;	break;
	case ENG_RSCC::rsc_dcl	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->axis_deceleration[i] == DBL_MAX)					return FALSE;	break;
	case ENG_RSCC::rsc_djk	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->axis_deceleration_jerk[i] == UINT16_MAX)			return FALSE;	break;
	case ENG_RSCC::rsc_hfp	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->home_flip_position[i] == DBL_MAX)				return FALSE;	break;
	case ENG_RSCC::rsc_lacl	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->axis_loaded_acceleration[i] == DBL_MAX)			return FALSE;	break;
	case ENG_RSCC::rsc_lajk	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->axis_loaded_acceleration_jerk[i] == UINT16_MAX)	return FALSE;	break;
	case ENG_RSCC::rsc_ldcl	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->axis_loaded_deceleration[i] == DBL_MAX)			return FALSE;	break;
	case ENG_RSCC::rsc_ldjk	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->axis_loaded_deceleration_jerk[i] == UINT16_MAX)	return FALSE;	break;
	case ENG_RSCC::rsc_lspd	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->axis_loaded_speed[i] == DBL_MAX)					return FALSE;	break;
	case ENG_RSCC::rsc_spd	: for (; i<MAX_ROBOT_DRIVE_COUNT; i++)	if (g_pstShMemMDR->axis_working_speed[i] == DBL_MAX)				return FALSE;	break;
	}
	return FALSE;
}
BOOL IsRecvAxisParmSetCmds(ENG_RSCC cmd, ENG_RANC axis)
{
	switch (cmd)
	{
	case ENG_RSCC::rsc_das	:
	case ENG_RSCC::rsc_dps	: return (strlen(g_pstShMemMDR->last_string_data) != 0);
	case ENG_RSCC::rsc__cax	: return (g_pstShMemMDR->custom_axis_param != INT32_MAX);
	case ENG_RSCC::rsc__cpa	: return (g_pstShMemMDR->custom_generic_param != INT32_MAX);
	case ENG_RSCC::rsc__cst	: return (g_pstShMemMDR->custom_station_param != INT32_MAX);
	}
	return FALSE;
}
BOOL IsRecvBasicMotionCmds(ENG_RSCC cmd, ENG_RANC axis)
{
	switch (cmd)
	{
	case ENG_RSCC::rsc__afe	:
	case ENG_RSCC::rsc_afe	:
	case ENG_RSCC::rsc__fe	: return (strlen(g_pstShMemMDR->last_string_data) != 0);
#if 0	/* 아래 3개의 명령어는 수신 패킷이 없음 */
	case ENG_RSCC::rsc_hom	:
	case ENG_RSCC::rsc_mvr	:
	case ENG_RSCC::rsc_mva	: return (g_pstShMemMDR->motion_done_reply != 0x00);
#endif
	}
	return FALSE;
}
BOOL IsRecvStaParamCmds(ENG_RSCC cmd, ENG_RANC axis)
{
	switch (cmd)
	{
	case ENG_RSCC::rsc__css		: return (g_pstShMemMDR->station_set_no != 0xff);
	case ENG_RSCC::rsc__pa		: return (g_pstShMemMDR->r_axis_push_acc_sta != DBL_MAX);
	case ENG_RSCC::rsc__pa_def	: return (g_pstShMemMDR->r_axis_push_acc_def != DBL_MAX);
	case ENG_RSCC::rsc__ps		: return (g_pstShMemMDR->r_axis_push_spd_sta != DBL_MAX);
	case ENG_RSCC::rsc__ps_def	: return (g_pstShMemMDR->r_axis_push_spd_def != DBL_MAX);
	case ENG_RSCC::rsc__zc		: return (g_pstShMemMDR->coord_compensation_slot[0] != DBL_MAX);
	case ENG_RSCC::rsc_app		:
	case ENG_RSCC::rsc_ens		: return (strlen(g_pstShMemMDR->last_string_data) != 0);
	case ENG_RSCC::rsc_cof		: return (g_pstShMemMDR->station_center_offset != DBL_MAX);
	case ENG_RSCC::rsc_cofp		: return (g_pstShMemMDR->station_center_offset_edge_grip != DBL_MAX);
	case ENG_RSCC::rsc_csl		: return (g_pstShMemMDR->current_slot_no != UINT8_MAX);
	case ENG_RSCC::rsc_cst		: return (g_pstShMemMDR->station_name != '@');
	case ENG_RSCC::rsc_cstn		: return (g_pstShMemMDR->station_no != UINT8_MAX);
	case ENG_RSCC::rsc_een		: return (g_pstShMemMDR->end_effector_num != UINT8_MAX);
	case ENG_RSCC::rsc_eeo		: return (g_pstShMemMDR->end_effector_up_down_station != UINT8_MAX);
	case ENG_RSCC::rsc_ils		: return (g_pstShMemMDR->station_last_seg_speed_factor != UINT8_MAX);
	case ENG_RSCC::rsc_irr		: return (g_pstShMemMDR->station_retracted_r_position != DBL_MAX);
	case ENG_RSCC::rsc_irt		: return (g_pstShMemMDR->station_retracted_t_position != DBL_MAX);
	case ENG_RSCC::rsc_isa		: return (g_pstShMemMDR->station_exit_angle != DBL_MAX);
	case ENG_RSCC::rsc_isd		: return (g_pstShMemMDR->station_exit_distance != DBL_MAX);
	case ENG_RSCC::rsc_ise		: return (g_pstShMemMDR->station_end_effect_length != DBL_MAX);
	case ENG_RSCC::rsc_isr		: return (g_pstShMemMDR->station_corner_radius != DBL_MAX);
	case ENG_RSCC::rsc_ist		: return (g_pstShMemMDR->inline_station_type != UINT8_MAX);
	case ENG_RSCC::rsc_nsl		: return (g_pstShMemMDR->station_slot_num != UINT8_MAX);
	case ENG_RSCC::rsc_nst		: return (g_pstShMemMDR->station_num != UINT8_MAX);
	case ENG_RSCC::rsc_ofs		: return (g_pstShMemMDR->station_offset_z != DBL_MAX);
	case ENG_RSCC::rsc_pgd		: return (g_pstShMemMDR->put_get_delay_time != UINT32_MAX);
	case ENG_RSCC::rsc_pgf		: return (g_pstShMemMDR->put_get_failed_option[0] != UINT8_MAX);
	case ENG_RSCC::rsc_pgo		: return (g_pstShMemMDR->put_get_option[0] != UINT8_MAX);
	case ENG_RSCC::rsc_pit		: return (g_pstShMemMDR->station_pitch != DBL_MAX);
	case ENG_RSCC::rsc_psd		: return (g_pstShMemMDR->start_pushing_distance != DBL_MAX);
	case ENG_RSCC::rsc_psh		: return (g_pstShMemMDR->pusher_type != UINT8_MAX);
	case ENG_RSCC::rsc_pss		: return (g_pstShMemMDR->station_start_pushing_offset != DBL_MAX);
	case ENG_RSCC::rsc_psz		: return (g_pstShMemMDR->station_pushing_offset_z != DBL_MAX);
	case ENG_RSCC::rsc_pua		: return (g_pstShMemMDR->station_pickup_acceleration_z != DBL_MAX);
	case ENG_RSCC::rsc_pus		: return (g_pstShMemMDR->station_pickup_speed_z != DBL_MAX);
	case ENG_RSCC::rsc_rpo		: return (g_pstShMemMDR->station_retracted_position_r != DBL_MAX);
	case ENG_RSCC::rsc_sbusy	: return (g_pstShMemMDR->station_service_busy != UINT8_MAX);
	case ENG_RSCC::rsc_slck		: return (g_pstShMemMDR->station_locked_state != UINT8_MAX);
	case ENG_RSCC::rsc_spo		:
	case ENG_RSCC::rsc_spon		: return (g_pstShMemMDR->station_position[0] != DBL_MAX);
	case ENG_RSCC::rsc_str		: return (g_pstShMemMDR->station_stroke_z != DBL_MAX);
	case ENG_RSCC::rsc_tbl		: return (g_pstShMemMDR->station_axis_overlap_factor != UINT8_MAX);
	case ENG_RSCC::rsc_tmd		: return (g_pstShMemMDR->station_trajectory_mode != UINT8_MAX);
	case ENG_RSCC::rsc_vchk		: return (g_pstShMemMDR->station_vacuum_check[0] != UINT8_MAX);
	case ENG_RSCC::rsc_vto		: return (g_pstShMemMDR->vacuum_search_time_out != UINT32_MAX);
	}
	return FALSE;
}
BOOL IsRecvStaMotionCmds(ENG_RSCC cmd, ENG_RANC axis)
{
	switch (cmd)
	{
	case ENG_RSCC::rsc_sbusy	: return (g_pstShMemMDR->station_service_busy != UINT8_MAX);	break;
	case ENG_RSCC::rsc_slck		: return (g_pstShMemMDR->station_locked_state != UINT8_MAX);	break;
	}
	return FALSE;
}
BOOL IsRecvStaScanCmds(ENG_RSCC cmd, ENG_RANC axis)
{
	switch (cmd)
	{
	case ENG_RSCC::rsc__bt		: return (!(g_pstShMemMDR->beam_threshold[0] == DBL_MAX ||
											g_pstShMemMDR->beam_threshold[0] == DBL_MIN));
	case ENG_RSCC::rsc__sa		: return (g_pstShMemMDR->wafer_scan_z_axis_acc != DBL_MAX);
	case ENG_RSCC::rsc__mt		: return (g_pstShMemMDR->maximum_wafer_thick != DBL_MAX);
	case ENG_RSCC::rsc__ss		: return (g_pstShMemMDR->wafer_scan_z_axis_spd != DBL_MAX);
	case ENG_RSCC::rsc__st		: return (g_pstShMemMDR->scanner_type != UINT8_MAX);
	case ENG_RSCC::rsc_dscn		:
	case ENG_RSCC::rsc_tcs		:
	case ENG_RSCC::rsc_map		: return (strlen(g_pstShMemMDR->last_string_data) != 0);
	case ENG_RSCC::rsc_sbt		: return (!(g_pstShMemMDR->station_per_beam_threshold[0] == DBL_MAX ||
											g_pstShMemMDR->station_per_beam_threshold[0] == DBL_MIN));
	case ENG_RSCC::rsc_scp		: return (g_pstShMemMDR->station_scanning_center_coord[0] != DBL_MAX);
	case ENG_RSCC::rsc_sdbg		: return (g_pstShMemMDR->scan_debug_mode != UINT8_MAX);
	case ENG_RSCC::rsc_slp		: return (g_pstShMemMDR->station_scanning_left_coord[0] != DBL_MAX);
	case ENG_RSCC::rsc_smt		: return (g_pstShMemMDR->station_wafer_max_thickness != DBL_MAX);
	case ENG_RSCC::rsc_srp		: return (g_pstShMemMDR->station_scanning_right_coord[0] != DBL_MAX);
	case ENG_RSCC::rsc_szh		: return (g_pstShMemMDR->station_scaning_highest_z_coord != DBL_MIN);
	case ENG_RSCC::rsc_szl		: return (g_pstShMemMDR->station_scaning_lowest_z_coord != DBL_MAX);
	}
	return FALSE;
}
BOOL IsRecvOtFCmds(ENG_RSCC cmd, ENG_RANC axis)
{
	switch (cmd)
	{
	case ENG_RSCC::rsc_acs		: return (g_pstShMemMDR->otf_set_number != 0xffff);
	case ENG_RSCC::rsc_aofcn2	: return (g_pstShMemMDR->otf_set_second_scan_notch != 0xff);
	case ENG_RSCC::rsc_aofd		: return (g_pstShMemMDR->otf_debug_level != 0x7f);
	case ENG_RSCC::rsc_aofim	: return (g_pstShMemMDR->otf_ignore_middle_samples != 0xff);
	case ENG_RSCC::rsc_aofdump	: return (strlen(g_pstShMemMDR->last_string_data) != 0);
	}
	return FALSE;
}
BOOL IsRecvNewDualArms(ENG_RSCC cmd, ENG_RANC axis)
{
	switch (cmd)
	{
	case ENG_RSCC::rsc_aofdump	: return (g_pstShMemMDR->arm_used_scan_spec_station != 0xff);
	}
	return FALSE;
}

/* ------------------------------------------------------------------------------------ */
/*                                      외부 함수                                       */
/* ------------------------------------------------------------------------------------ */

#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : Robot Library 초기화
 parm : config	- [in]  환경 설정 정보
		shmem	- [in]  Diamond Robot 관련 공유 메모리
 retn : None
*/
API_EXPORT BOOL uvMDR_Init(LPG_CIEA config, LPG_MRDV shmem)
{
	/* 공유 메모리 연결 */
	g_pstShMemMDR	= shmem;
	/* 전역 환경 정보 */
	g_pstConfig		= config;
	/* 반드시 이 값으로 초기화 */
	g_pstShMemMDR->info_mode	= 0xff;

	/* 시리얼 통신 스레드 생성 */
	g_pMDRThread	= new CMDRThread(config->diamond_robot.port_no,
									 config->diamond_robot.baud_rate,
									 config->diamond_robot.port_buff,
									 shmem);
	ASSERT(g_pMDRThread);
	if (!g_pMDRThread->CreateThread(0, NULL, 100))	return FALSE;

	return TRUE;
}

/*
 desc : Robot Library 해제
 parm : None
 retn : None
*/
API_EXPORT VOID uvMDR_Close()
{
	/* 작업 스레드 메모리 해제 */
	if (g_pMDRThread)
	{
		if (g_pMDRThread->IsBusy())		g_pMDRThread->Stop();
		while (g_pMDRThread->IsBusy())	g_pMDRThread->Sleep();
		delete g_pMDRThread;
		g_pMDRThread	= NULL;
	}
}

/*
 desc : 현재 Logosol Robot 연결되어 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_IsConnected()
{
	if (!g_pMDRThread)	return FALSE;
	if (!g_pMDRThread->IsBusy())	return FALSE;

	/* 현재 통신 상태 반환 */
	//if (g_pMDRThread->GetCommStatus() == ENG_SCSC::en_scsc_closed) return FALSE;

	if (g_pMDRThread->IsOpenPort()) return TRUE;

	return FALSE;
}

/*
 desc : 기존 통신 관련 송/수신 데이터 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvMDR_ResetCommData()
{
	g_pMDRThread->ResetCommData();
}

/*
 desc : 가장 최근에 송신한 명령어에 대한 응답 상태 값 반환
 parm : None
 retn : 0x00 - FAIL, 0x01 - SUCC, 0xff - Not defined (Not received)
*/
API_EXPORT ENG_MSAS uvMDR_GetLastSendAckStatus()
{
	return g_pMDRThread->GetLastSendAckStatus();
}

/* --------------------------------------------------------------------------------------------- */
/*                        Command Reference  (1. Informational commands)                         */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 현재 로봇의 시스템 시간 설정 및 반환
 parm : None
 retn : TRUE or FALSE
 note : 현재 로컬 컴퓨터의 날짜와 시간을 로봇 컨트롤러에 설정
*/
API_EXPORT BOOL uvMDR_SetDateTime()
{
	CHAR szCmd[32]	= {NULL};
	SYSTEMTIME stNow= {NULL};

	GetLocalTime(&stNow);
	sprintf_s(szCmd, 32, "%04u%02u%02u %02u%02u%02u",
			  stNow.wYear, stNow.wMonth, stNow.wDay, stNow.wHour, stNow.wMinute, stNow.wSecond);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__dt, szCmd, FALSE);
}
API_EXPORT BOOL uvMDR_GetDateTime()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__dt, "_DT", TRUE);
}

/*
 desc : 시스템 구성 정보 저장 / 보고
 parm : None
 retn : TRUE or FALSE
 note : uvMDR_SetSystemConfigInfo() 이 함수는 왠만하면 호출하지 말아야 됨
*/
API_EXPORT BOOL uvMDR_SetSystemConfigInfo()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__si, "_SI w", FALSE);
}
API_EXPORT BOOL uvMDR_GetSystemConfigInfo()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__si, "_SI", TRUE);
}

/*
 desc : 모션 컨트롤러 리셋
 parm : None
 retn : TRUE or FALSE
 note : 이 명령은 컨트롤러를 재설정 합니다. 동작이 있는지 여부는 확인되지 않습니다.
		저장되지 않은 Motion 또는 station parameter가 있으면 손실 됩니다.
*/
API_EXPORT BOOL uvMDR_ResetMotionController()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_res, "RES");
}

/*
 desc : 로봇 시리얼 번호 반환
 parm : None
 retn : TRUE or FALSE
 note : 이 명령은 로봇 일련 번호를 반환 합니다. 이 번호는 변경할 수 없습니다.
*/
API_EXPORT BOOL uvMDR_GetRobotSerialNumber()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_snr, "SNR", TRUE);
}

/*
 desc : 로봇 펌웨어 버전 반환
 parm : option	- [in]  0: Normal, 1: Detail
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_GetRobotFirmwareVer(UINT8 option)
{
	if (option)	g_pMDRThread->SendData(ENG_RSCC::rsc_ver, "VER s", TRUE);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_ver, "VER", TRUE);
}

/* --------------------------------------------------------------------------------------------- */
/*           Command Reference  (2.Housekeeping (parameters save / restore) commands)            */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 기존 User (사용자)에 의해 설정된 모든 파라미터 값을 공장 초기화 값으로 복구
		사용자가 수정한 (건드린) 파라미터 관련된 부분만 복구 함
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_RestoreCustomParameter()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__cld, "_CLD");
}

/*
 desc : 기존 User (사용자)에 의해 설정된 모든 파라미터 값을 firware 영역에 저장
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SaveCustomParameter()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__csv, "_CSV");
}

/*
 desc : 기존 저장된 Motion과 Calibration Parameters 복구
 parm : None
 retn : TRUE or FALSE
 note : 이 명령은 NV (비 휘발성) 메모리에서 이전에 저장된 모션 매개 변수 및 로봇 교정 매개 변수
		(SMP 명령 사용) 복원
		현재 유효한 모든 동작 및 보정 매개 변수가 손실되고 저장된 매개 변수 적용
		다음과 같은 비축 관련 설정이 복원 됩니다.
			_BB, CSTP, INF, LENB, PGO, TMD, TBL, _BT/BTO, _BR/BRD, _BZ/BZD
		모든 축에 대해 다음 설정 (및 해당 명령)이 복원 됩니다.
			_CH, _CL, _EO, _EL, _FSE, _HI, _HL, _HO, _HP, _IL, _FL, _KD, _KI, _KP, _RL, _RSE,
			ACL, AJK, AOFSA, DCL, DJK, HFP, LSPD, LACL, LDCL, LAJK, LDJK, SPD
		로봇이 사용중인 경우 (예: 동작이 활성화된 경우)이 명령을 사용할 수 없습니다.
*/
API_EXPORT BOOL uvMDR_RestoreMotionCalibrationParam()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_rmp, "RMP");
}

/*
 desc : 기존 저장된 디스크 (paramset.ini)로부터 Parameters Sets 복구
 parm : None
 retn : TRUE or FALSE
 note : 이 명령은 paramset.ini라는 디스크 파일에서 모든 매개 변수 세트를 다시 읽고 기존 값을
		덮어 씀. 이 명령이 시작하는 동안 자동으로 실행 됨
*/
API_EXPORT BOOL uvMDR_ReadParametersSetsFromDisk()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_rps, "RPS");
}

/*
 desc : 모든 station parameter 복구
 parm : None
 retn : TRUE or FALSE
 note : 이 명령은 이전에 SSP 명령으로 저장한 all station parameter를 복원 합니다.
		다음은 station 및 wafer parameter가 복원되어 집니다.
		-. station coordinates, stroke, pitch, offset and retract position
		-. number of slots, default slot
		-. pick-up speed and acceleration
		-. vacuum time-outs and pick-up delay
		-. inline station parameters
		-. wafer mapping parameters
*/
API_EXPORT BOOL uvMDR_RestoreStationParameters()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_rsp, "RSP");
}

/*
 desc : 현재 Parameters Sets 정보를 디스크 (paramset.ini)에 저장
 parm : None
 retn : TRUE or FALSE
 note : 이 명령은 paramset.ini라는 디스크 파일에 모든 매개 변수 세트 저장
*/
API_EXPORT BOOL uvMDR_SaveParametersSetsToDisk()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_sps, "SPS");
}

/*
 desc : 모든 station parameter 저장
 parm : None
 retn : TRUE or FALSE
 note : This command cannot be used if the robot is busy (e.g. motion is active).
		이 all station parameter를 NV (비휘발성) 메모리에 저장 합니다.
		다음은 station 및 wafer parameter가 저장되어 집니다.
		-. station coordinates, stroke, pitch, offset and retract position
		-. number of slots, default slot
		-. pick-up speed and acceleration
		-. vacuum time-outs and pick-up delay
		-. inline station parameters
		-. wafer mapping parameters
*/
API_EXPORT BOOL uvMDR_SaveStationParameters()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_ssp, "SSP");
}

/* --------------------------------------------------------------------------------------------- */
/*                     Command Reference  (3.Status and diagnostic commands)                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 현재 로보의 상태 정보 반환
 parm : None
 retn : TRUE or FALSE
 note : 상세한 현재 시스템의 상태 정보 출력하여 문제 진단을 위해 유용 함
*/
API_EXPORT BOOL uvMDR_GetCurrentRobotState()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc___cs, "__CS", TRUE);
}

/*
 desc : 현재 로보의 축 위치 정보 반환
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_GetAxisCurrentPositionAll()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_cpo, "CPO", TRUE);
}

/*
 desc : 명령어 송신에 대한 응답 모드 값 설정 및 반환
 parm : mode	- [in]  응답 모드 값
						Mode 0: 동기 모드. Master / Slave 동작 모드로서, Host는 명령을 전송하고 Firmware (Robot)는 응답을 반환하게 되면, 다음 명령을 전송할 수 있는 방식 (Robot (Firmware)으로부터 응답이 수신될 때까지 Host는 대기해야 하며, 그 이전에는 다른 명령을 송신 못함)
						Mode 1: 비동기 모드. Robot에서 매크로 실행이 종료된 후 Firmware는 System상태 값 (status word)를 16진수 형식과 Prompt (‘>’ or ‘?’)로 Host에 전송
						Mode 2: “Mode 1”과 동작은 동일. 다만 Host는 Firmware로부터 Prompt (‘>’ or ‘?”) 만 응답 받음 (상태 값을 받기 위해서는 추가로 요청 명령 실행해야 함)
						Mode 3: “Mode 1”과 동작은 동일. 다만 Host는 Firmware로부터 상태 값 대신 이해하기 쉬운 문자열 값으로 응답 받음 (이 기능은 Host가 터미널 에뮬레이터로 실행 중일 때 진단하는데 유용)
						Mode 4: “Mode 1”과 동작은 동일. 다만 Host는 Firmware로부터 수신 받은 데이터 (패킷)를 보면, 이전에 Host가 Firmware로 송신한 명령어 이름이 포함되어서 응답 값과 함께 수신한다는 것이 다름 (예제 보면 쉽게 이해가 올 것임)
						Mode 5: “Mode 1”과 동작은 동일. 다만 Firmware가 Host에게 값을 알리기 전에 Host가 이전에 송신한 명령 이름 (Mode 4와 유사)를 포함하고, 여기에  오류 코드 및 상태 값도 포함하여 Host에게 전송 (예제 보면 이해 됨). 오류 코드는 0(오류 없음) 또는 1(매개변수 / 상태의 오류)이며, EERR 명령으로 추가 오류 정보 확인 가능
 retn : TRUE or FALSE
 note : 이 명령은 매크로 실행  종료 후 펌웨어 응답 모드를 설정 혹은 반환
		“InfoMode” 값은 다양한 Host를 위한 동기화 방법을 제공 합니다.
		이 파라미터는 SMP 명령과 함께 저장되며 펌웨어를 시작하는 동안 복원됩니다. 이 매개 변수의 기본값은 1 (“Mode 1”).
*/
API_EXPORT BOOL uvMDR_SetInfoMode(UINT32 mode)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "INF %d", mode);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_inf, szCmd);
}
API_EXPORT BOOL uvMDR_GetInfoMode()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_inf, "INF", TRUE);
}

/*
 desc : 현재 로봇의 상태 혹은 마지막 진단 정보 (에러, 현재 상태: 엔코더 상태, 축 상태 등등)
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_GetDiagInfoCurrent()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_diag, "DIAG C");
}
API_EXPORT BOOL uvMDR_GetDiagInfoLast()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_diag, "DIAG", TRUE);
}

/*
 desc : 확장된 오류 상태 및 상태 정보 반환
 parm : None
 retn : TRUE or FALSE
 note : 에러 상태를 숫자 값으로 반환
*/
API_EXPORT BOOL uvMDR_GetExtendedErrorNumber()
{
	/* 기존 에러 값 초기화 */
	g_pstShMemMDR->extended_error_code	= 0x0000;
	return g_pMDRThread->SendData(ENG_RSCC::rsc_eerr, "EERR", TRUE);
}

/*
 desc : 확장된 오류 상태 및 상태 정보 반환
 parm : None
 retn : TRUE or FALSE
 note : 에러 상태를 텍스트로 반환
*/
API_EXPORT BOOL uvMDR_GetExtendedErrorState()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_est, "EST", TRUE);
}

/*
 desc : 확장된 오류 상태 및 상태 정보 반환
 parm : None
 retn : TRUE or FALSE
 note : EST와는 별개로, 에러 상태를 16진수 워드 (Word) 값으로 출력하되, 현재 모션의 축 위치도 반환
*/
API_EXPORT BOOL uvMDR_GetExtendedErrorStateCode()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_esta, "ESTA", TRUE);
}

/*
 desc : 이전에 발생된 에러의 상태에 대한 이유 (원인) 반환
 parm : None
 retn : TRUE or FALSE
 note : 무조건 문자열로 반환하도록 되어 있음 (INF 3 모드로 설정되어 있지 않아도 됨)
*/
API_EXPORT BOOL uvMDR_GetPreviousErrorReason()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_per, "PER V", TRUE);
}

/*
 desc : 모든 축의 Home Switch 상태 값 반환
 parm : None
 retn : TRUE or FALSE
 note : 결과는 4 자리 16 진수 (WORD. Hexadecimal)입니다.
		각 축에는 축 스위치 상태를 보여주는 4 비트가 있습니다.
		비트 0 ~ 3은 T, 비트 4 ~ 7은 R, 비트 8 ~ 11은 Z의 상태 (형식: 0xKZRT)
*/
API_EXPORT BOOL uvMDR_GetHomeSwitchAll()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_rhs, "RHS", TRUE);
}

/*
 desc : 모든 축의 Parameter와 Robot Calibration Paramter를 비휘발성 메모리 저장
 parm : option	- [in]  0 or 1 (Working Speed와 Acceleration이 저장되지 않음)
 retn : TRUE or FALSE
 note : 로봇이 사용 중인 경우 (예: 동작이 활성화된 경우)이 명령을 사용할 수 없음
*/
API_EXPORT BOOL uvMDR_SaveMotionCalibrationParam(UINT8 option)
{
	CHAR szCmd[32]	= { "SMP" };

	if (0x01 == option)	sprintf_s(szCmd, 32, "SMP %u", option);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_smp, szCmd);
}

/*
 desc : 명령 실행 상태 값 반환 즉, 현재 컨트롤러의 일반 상태 값 반환
 parm : None
 retn : TRUE or FALSE
 note : 로봇이 사용 중인 경우 (예: 동작이 활성화된 경우)이 명령을 사용할 수 없음
*/
API_EXPORT BOOL uvMDR_GetSystemStatus()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_sta, "STA", TRUE);
}

/* --------------------------------------------------------------------------------------------- */
/*                Command Reference  (4.Support for systems with absolute encoders)              */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : AE (Absolute Encoder) reset warning / position
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		mode	- [in]  초기화 대상 모드 값 즉, 256 (Warning), 1024 (Position) 
		password- [in]  강제로 진행하기 위한 암호 값 (내부에 암호가 설정되어 있다면)
 retn : TRUE or FALSE
 note : 신중하게 사용. AE의 상태 (Warning or Error) Clear
		_AER T 256  : AE에서 Warning 에러 (ex> 배터리 충전 저하 상태) 지우고,
		              다음 모션 동작을 위한 명령이 허용 됨
		_AER T 1024 : 서비스 직원에 의해 사용됨. AE Position 제거되며,
					  로봇 보정 정보가 무효화 됨 (신중하게 사용해야 함)
 */
API_EXPORT BOOL uvMDR_SetAxisEncoderReset(ENG_RANC axis, ENG_RAEC mode, PCHAR password)
{
	CHAR szCmd[128]	= {NULL}, szAxis[8]	= {NULL};

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	if (password)	sprintf_s(szCmd, 128, "_AER %s %u %s", szAxis, UINT16(mode), password);
	else			sprintf_s(szCmd, 128, "_AER %s %u", szAxis, UINT16(mode));
	return g_pMDRThread->SendData(ENG_RSCC::rsc__aer, szCmd);
}

/*
 desc : Absolute Encoder Status (for Dual-Arm)
 parm : None
 retn : TRUE or FALSE
 note : 이 명령은 모든 축에서 절대 인코더의 상태를 보고 합니다.
		보고서의 가능한 값은 다음과 같습니다 (INF 0: 동기 모드)
*/
API_EXPORT BOOL uvMDR_GetEconderStatus()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__aest, "_AEST", TRUE);
}

/*
 desc : 물리적인 Homing 이후 논리적인 영점 (Zero-Position) 위치로 이동하고자 하는 위치 설정/반환
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		value	- [in]  오차 회전 각도 혹은 거리 (Offset) 값 (단위: angle, um)
 retn : TRUE or FALSE
 note : 현재 축 (Axis)의 위치가 Homing 이후 이동할 Zero Position 값 (Single Axis)
		축에 배터리가 지원되는 제품인 경우에 한함
*/
API_EXPORT BOOL uvMDR_SetEncoderZeroOffsetValue(ENG_RANC axis, DOUBLE value)
{
	CHAR szCmd[32]		= {NULL}, szAxis[8] = {NULL};
	ENG_RSCC enCmd[4]	= { ENG_RSCC::rsc__eo_t, ENG_RSCC::rsc__eo_r1,
							ENG_RSCC::rsc__eo_r2,ENG_RSCC::rsc__eo_z };
	if (axis > ENG_RANC::rac_z)	return FALSE;

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	sprintf_s(szCmd, 32, "_EO %s %d", szAxis, GetAxisPosDev(axis, value));
	return g_pMDRThread->SendData(enCmd[UINT8(axis)], szCmd);
}
API_EXPORT BOOL uvMDR_GetEncoderZeroOffset()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__eo, "_EO", TRUE);
}

/*
 desc : 물리적인 Homing 이후 논리적인 영점 (Zero-Position) 위치로 이동하고자 하는 위치 설정/반환
 parm : None
 retn : TRUE or FALSE
 note : 현재 축 (Axis)의 위치가 Homing 이후 이동할 Zero Position 값 (All Axes)
*/
API_EXPORT BOOL uvMDR_SetEncoderZeroOffsetAll()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__eor, "_EOR #");
}
API_EXPORT BOOL uvMDR_GetEncoderZeroOffsetAll()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__eor, "_EOR", TRUE);
}

/*
 desc : 이 명령은 주어진 축에 대한 저수준 절대 인코더 상태를 보고합니다
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
 retn : TRUE or FALSE
 note : 축 서보 루프를 끈 상태에서 실행해야 함
*/
API_EXPORT BOOL uvMDR_GetLowLevelEncoderStatus(ENG_RANC axis)
{
	CHAR szCmd[32]	= {NULL}, szAxis[8] = {NULL};

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	sprintf_s(szCmd, 32, "_RALMC %s", szAxis);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__ralmc, szCmd, TRUE);
}

/* --------------------------------------------------------------------------------------------- */
/*              Command Reference  (5.Basic (non-axes, non-station) action commands)             */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 배터리 백업 전원이 옵션으로 설치된 경우, Robot의 전원 어떻게 On / Off 할 것인지 설정
 parm : option	- [in]  0 : Power OFF (바로 장비 OFF), 1 : Power ON (일정 시간 ON 유지)
		value	- [in]  Time-out 값. 즉, 전원을 On or Off 할 때, 이 주어진 시간 값 (초; second) 만큼 동작
						(Data Range : 0x0000(0) ~ 0x1000 (4096)
 retn : TRUE or FALSE
 note : option == 0, AC 전원이 꺼지면, 바로 Turn Off 됨 (Backup Power 비활성화 됨)
		option == 1, AC 전원이 꺼지면, 바로 Turn Off 안함 (Value 값 보고 판단함)
					 Value == 0, 장비는 Backup Power가 사라질 때까지 계속 ON 유지
					 Value == 1 or Later, 장비는 Value 초 동안 유지후 장비 전원 OFF
*/
API_EXPORT BOOL uvMDR_SetBackupPowerCtrl(UINT8 option, INT32 value)
{
	CHAR szCmd[32]	= {NULL};
	if (0x00 == option)	sprintf_s(szCmd, 32, "_BB 0");
	else				sprintf_s(szCmd, 32, "_BB 1 %u", value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__bb, szCmd);
}
API_EXPORT BOOL uvMDR_GetBackupPowerCtrl()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__bb, "_BB", TRUE);
}

/*
 desc : Enable/Disable Controlled Stop Feature
 parm : state	- [in]  0 for OFF, 1 for ON
		option	- [in]  whether to report activation (활성화 보고 (알림) 여부)
 retn : TRUE or FALSE
 note : 제어된 중지 기능을 사용 가능으로 설정하면 디지털 입력이 활성화되면 모션 명령이 우아하게 중지될 수 있습니다.
		이 기능은 잘 나타나려면 올바르게 구성되어야 합니다.기능을 사용할 수 없는 경우 오류 프롬프트 "?"가 보고됩니다.
		이 기능이 활성화되어 있고 컨트롤 입력이 활성화된 경우 컨트롤 입력이 비활성화되거나
		기능이 꺼질 때까지 모든 모션 명령이 비활성화됩니다.
*/
API_EXPORT BOOL uvMDR_SetControlledStopFeature(UINT8 state, UINT8 option)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "CSTP %u %u", state, option);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_cstp, szCmd);
}
API_EXPORT BOOL uvMDR_GetControlledStopFeature()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_cstp, "CSTP", TRUE);
}

/*
 desc : 외부 디지털 입력의 상태 값 반환
 parm : None
 retn : TRUE or FALSE
 note : 이 명령은 연결된 외부 I/O 모듈 또는 확장 I/O 인터페이스 입력의 디지털 입력 상태 반환
		반환되는 값은 4 자리의 16진수 값
*/
API_EXPORT BOOL uvMDR_GetStateExtDI()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_din, "DIN", TRUE);
}

/*
 desc : 외부 디지털 출력의 상태 값 설정 / 반환
 parm : output	- [in]  0 ~ 9 번까지 Output Pin Number
		value	- [in]  상태 값
 retn : TRUE or FALSE
 note : 이 명령은 확장 I/O 인터페이스 모듈의 출력 상태 설정
		반환되는 값은 4 자리의 16진수 값
*/
API_EXPORT BOOL uvMDR_SetStateExtDONo(UINT8 output, UINT8 value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "DOUT %u %u", output, value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_dout, szCmd);
}
API_EXPORT BOOL uvMDR_GetStateExtDO()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_dout, "DOUT", TRUE);
}

/*
 desc : 외부 디지털 입력의 전체 상태 값 설정 / 반환
 parm : input	- [in]  input number
 retn : TRUE or FALSE
 note : 16진수 WORD 값으로 디지털 입력 (0 ~ 8 pin) 상태 값 반환
*/
API_EXPORT BOOL uvMDR_SetStateInput(UINT8 input)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "INP %u", input);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_inp, szCmd);
}
API_EXPORT BOOL uvMDR_GetStateInput()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_inp, "INP", TRUE);
}

/*
 desc : Enable/Disable Loaded Speed Parameters 설정 / 반환
 parm : option	- [in]  0: Disable, 1: Enable
 retn : TRUE or FALSE
 note : Wafer 적재할 때, 별도로 가속도/감속도/가가속도/가감속도 사용 여부
*/
API_EXPORT BOOL uvMDR_SetLoadedSpeedParamEnable(UINT8 option)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "LENB %u", option);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_lenb, szCmd);
}
API_EXPORT BOOL uvMDR_GetLoadedSpeedParamEnable()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_lenb, "LENB", TRUE);
}

/*
 desc : Digital Output State 설정 / 반환
 parm : output	- [in]  Output Number
		value	- [in]  optional output state (0 or 1)
 retn : TRUE or FALSE
 note : 디지털 출력 (Digital Ouput) 상태를 설정 혹은 반환
*/
API_EXPORT BOOL uvMDR_SetStateOutput(UINT8 output, UINT8 value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "OUT %u %u", output, value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_out, szCmd);
}
API_EXPORT BOOL uvMDR_GetStateOutput()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_out, "OUT", TRUE);
}

/*
 desc : Axis Servo Turn Off or On 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
 retn : TRUE or FALSE
 note : 이 명령은 매크로가 실행 중이면 허용되지 않음 (정상적으로 동작했는지는 STA 명령으로 확인)
*/
API_EXPORT BOOL uvMDR_SetAxisServoTurnOff(ENG_RANC axis)
{
	CHAR szCmd[32]	= { "SOF" }, szAxis[8] = {NULL};

	if (axis <= ENG_RANC::rac_z)
	{
		/* 축 코드에 대한 이름 반환 */
		GetAxisChar(axis, szAxis, 8);
		sprintf_s(szCmd, 32, "SOF %s", szAxis);
	}
	return g_pMDRThread->SendData(ENG_RSCC::rsc_sof, szCmd);
}
API_EXPORT BOOL uvMDR_SetAxisServoTurnOn(ENG_RANC axis)
{
	CHAR szCmd[32]	= { "SON" }, szAxis[8] = {NULL};

	if (axis <= ENG_RANC::rac_z)
	{
		/* 축 코드에 대한 이름 반환 */
		GetAxisChar(axis, szAxis, 8);
		sprintf_s(szCmd, 32, "SON %s", szAxis);
	}

	return g_pMDRThread->SendData(ENG_RSCC::rsc_son, szCmd);
}

/*
 desc : Vacuum Paddle Turn Off or On 설정
 parm : effect_num	- [in]  0 (effector number 0) or 1 (effector number 1) or 2 (All)
 retn : TRUE or FALSE
 note : 모든 진공 패들에 대해 Turn OFF 수행 (정상적으로 동작했는지는 STA 명령으로 확인)
*/
API_EXPORT BOOL uvMDR_SetVacuumPaddleTurnOff(UINT8 effect_num)
{
	CHAR szCmd[3][8] = {"VOF 0", "VOF 1", "VOF" };
	return g_pMDRThread->SendData(ENG_RSCC::rsc_vof, szCmd[effect_num]);
}
API_EXPORT BOOL uvMDR_SetVacuumPaddleTurnOn(UINT8 effect_num)
{
	CHAR szCmd[3][8] = {"VON 0", "VON 1", "VON" };
	return g_pMDRThread->SendData(ENG_RSCC::rsc_von, szCmd[effect_num]);
}

/*
 desc : Vacuum Paddle에 Wafer가 있는지 여부 반환
 parm : number	- [in]  0 or 1 (R 축이 Dual인 경우)
 retn : TRUE or FALSE
 note : 진공 패들 작업을 위해 구성된 시스템에서 이 명령은 지정된 진공 패들에서 웨이퍼 존재 여부 확인
		만약 진공이 꺼져 있는 경우라면, 진공 센서가 웨이퍼 존재를 감지하기 위해 켜집니다.
		웨이퍼가 감지되지 않으면 진공이 꺼집니다.
*/
API_EXPORT BOOL uvMDR_GetCheckWaferInVacuumPaddle(UINT8 number)
{
	CHAR szCmd[32]		= {NULL};
	ENG_RSCC enCmd[2]	= { ENG_RSCC::rsc_vst_t, ENG_RSCC::rsc_vst_b };
	if (number > 1)	number	= 0x00;
	sprintf_s(szCmd, 32, "VST %u", number);
	return g_pMDRThread->SendData(enCmd[number], szCmd, TRUE);
}

/* --------------------------------------------------------------------------------------------- */
/*                      Command Reference  (6.Basic axis motions parameters)                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Set/Report Axis Customized Home Position
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		value	- [in]  각도 혹은 축 위치 (um) 값 
 retn : TRUE or FALSE
 note : Homing이 완료되고 나서, 이후에 관련 (해당) 축이 이동하려는 곳의 위치 설정 혹은 위치 값 반환
*/
API_EXPORT BOOL uvMDR_SetCustomizedHomePosition(ENG_RANC axis, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL}, szAxis[8] = {NULL};
	UINT32 u32Value	= GetAxisPosDev(axis, value);
	if (axis > ENG_RANC::rac_z)	return FALSE;

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	sprintf_s(szCmd, 32, "_CH %s %d", szAxis, u32Value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__ch, szCmd);
}
API_EXPORT BOOL uvMDR_GetCustomizedHomePosition()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__ch, "_CH", TRUE);
}

/*
 desc : 축의 전류 한계 값 설정 / 반환
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		value	- [in]  각 축의 전류 제한 레벨 값 (1 ~ 251)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetAxisCurrentLimitLevel(ENG_RANC axis, UINT8 value)
{
	CHAR szCmd[32]	= {NULL}, szAxis[8] = {NULL};
	if (value < 1 || value > 251)	return FALSE;
	if (axis > ENG_RANC::rac_z)	return FALSE;

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	sprintf_s(szCmd, 32, "_CL %s %u", szAxis, value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__cl, szCmd);
}
API_EXPORT BOOL uvMDR_GetAxisCurrentLimitLevel()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__cl, "_CL", TRUE);
}

/*
 desc : 축의 에러 리밋 설정 (Axis Position Error Limit)
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		value	- [in]  Encoder Counts (position error limit of Axis)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetAxisPositionErrorLimit(ENG_RANC axis, INT32 value)
{
	CHAR szCmd[32]	= {NULL}, szAxis[8] = {NULL};
	if (value < 1 || value > 251)	return FALSE;
	if (axis > ENG_RANC::rac_z)	return FALSE;

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	sprintf_s(szCmd, 32, "_EL %s %d", szAxis, value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__el, szCmd);
}
API_EXPORT BOOL uvMDR_GetAxisPositionErrorLimit()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__el, "_EL", TRUE);
}

/*
 desc : 모든 축의 위치에 대해 모니터링하고 있다가 Safe Envelope 위치가 정의되어져 있는 상태에서
		임의 축 위치가 Limit를 벗어날 때, user-defined 출력이 Turn On
		(즉, 이 명령으로 정의된 값이 동작 한다고 보면 됨)
		만약 모든 축이 Safe Envelope 내에 있거나, 동작 값이 정의되어 있지 않으면 Turn Off 됨
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		direct	- [in]  0x00 (T - 시계방향, R1/R2 - Forward, Z - Up?)
						0x01 (T - 반시계 방향, R1/R2 - Reverse, Z - Down?)
		value	- [in]  회전 축인 경우, 각도 / 선형 (앞/뒤 이동) 축인 거리 (단위: um)
 retn : TRUE or FALSE
 note : SMP로 저장, RMP로 복구
 예제 : >_FSE
		0,0,0
		>_FSE R 7200 -> Safe Envelope 위치 설정
		>_MVR R 8000 -> 8000 으로 회전
		>
		>.OUT _OutputE -> 에러 메시지 출력
		1
		>
 예제 : >_RSE
		0,0,0
		>_RSE R -7200 -> Safe Envelope 위치 설정
		>_MVA R -8000 -> -8000 으로 회전
		>
		>.OUT _OutputE -> 에러 메시지 출력
		1
		>
*/
API_EXPORT BOOL uvMDR_SetAxisSafeEnvelop(ENG_RANC axis, ENG_MAMD direct, INT32 value)
{
	CHAR szCmd[32]		= {NULL}, szAxis[8] = {NULL}, szMode[2][8] = { "_FSE", "_RSE" };
	ENG_RSCC enCmd[2]	= { ENG_RSCC::rsc__fse, ENG_RSCC::rsc__rse };
	if (axis > ENG_RANC::rac_z)	return FALSE;

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	sprintf_s(szCmd, 32, "%s %s %d", szMode[UINT8(direct)], szAxis, GetAxisPosDev(axis, value));
	return g_pMDRThread->SendData(enCmd[UINT8(direct)], szCmd);
}
API_EXPORT BOOL uvMDR_GetAxisSafeEnvelop(ENG_MAMD direct)
{
	CHAR szCmd[32]		= {NULL}, szMode[2][8] = { "_FSE", "_RSE" };
	ENG_RSCC enCmd[2]	= { ENG_RSCC::rsc__fse, ENG_RSCC::rsc__rse };

	sprintf_s(szCmd, 32, "%s", szMode[UINT8(direct)]);
	return g_pMDRThread->SendData(enCmd[UINT8(direct)], szCmd);
}

/*
 desc : 축 이동 위치의 S/W Limit 설정 (Positive or Negative) / 반환
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		direct	- [in]  0x00: Positive (Forwared), 0x01: Negative (Backword)
		value	- [in]  Software Limit 위치 (단위: Angle or um)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetAxisSoftwareLimitValue(ENG_RANC axis, ENG_MAMD direct, INT32 value)
{
	CHAR szCmd[32]		= {NULL}, szAxis[8] = {NULL};
	ENG_RSCC enCmd[2]	= { ENG_RSCC::rsc__fl, ENG_RSCC::rsc__rl };
	if (axis > ENG_RANC::rac_z)	return FALSE;

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	if (direct == ENG_MAMD::amd_forward)	sprintf_s(szCmd, 32, "_FL %s %d", szAxis, value);
	return g_pMDRThread->SendData(enCmd[UINT8(direct)], szCmd);
}
API_EXPORT BOOL uvMDR_SetAxisSoftwareLimitCurrent(ENG_RANC axis, ENG_MAMD direct)
{
	CHAR szCmd[32]		= {NULL}, szAxis[8] = {NULL};
	ENG_RSCC enCmd[2]	= { ENG_RSCC::rsc__fl, ENG_RSCC::rsc__rl };
	if (axis > ENG_RANC::rac_z)	return FALSE;

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	if (direct == ENG_MAMD::amd_forward)	sprintf_s(szCmd, 32, "_FL %s", szAxis);
	else									sprintf_s(szCmd, 32, "_RL %s", szAxis);
	return g_pMDRThread->SendData(enCmd[UINT8(direct)], szCmd);
}
API_EXPORT BOOL uvMDR_GetAxisSoftwareLimit(ENG_MAMD direct)
{
	CHAR szCmd[2][32]	= { "_FL", "_RL" };
	ENG_RSCC enCmd[2]	= { ENG_RSCC::rsc__fl, ENG_RSCC::rsc__rl };

	return g_pMDRThread->SendData(enCmd[UINT8(direct)], szCmd[UINT8(direct)], TRUE);
}

/*
 desc : 축이 정지하고 있는 동안 어떻게 정지하고 있을지 즉, 정지 상태에 대한 미분 계수 (KD) 값 설정 / 반환
		즉, 축이 정지하는 동안 축에 대한 부하 값을 설정함으로써, 축의 떨림?을 조정할 수 있음?
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		value	- [in]  Differential Coefficient of Axis
 note : 이 값 (parameter)을 잘못 설정하면 축이 동작 중에 servo가 off 될 수 있음
*/
API_EXPORT BOOL uvMDR_SetDiffCoeffAxisHold(ENG_RANC axis, UINT32 value)
{
	CHAR szCmd[32]		= {NULL}, szAxis[8] = {NULL};
	if (axis > ENG_RANC::rac_z)	return FALSE;

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	sprintf_s(szCmd, 32, "_HD %s %u", szAxis, value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__hd, szCmd);
}
API_EXPORT BOOL uvMDR_GetDiffCoeffAxisHold()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__hd, "_HD", TRUE);
}

/*
 desc : 축이 정지하고 있는 동안 축의 이동 오차 (떨림) 한계 값 설정 / 반환
		즉, 축이 정지하고 있는 미세하게 떨리는데, 이 떨리는 오차 범위 값의 한계를 지정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		value	- [in]  position error limit of axis
 note : 이 값 (parameter)을 잘못 설정하면 축이 동작 중에 servo가 off 될 수 있음
*/
API_EXPORT BOOL uvMDR_SetPosErrorLimitAxisHold(ENG_RANC axis, DOUBLE value)
{
	CHAR szCmd[32]		= {NULL}, szAxis[8] = {NULL};
	if (axis > ENG_RANC::rac_z)	return FALSE;

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	sprintf_s(szCmd, 32, "_HE %s %d", szAxis, GetAxisPosDev(axis, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc__he, szCmd);
}
API_EXPORT BOOL uvMDR_GetPosErrorLimitAxisHold()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__he, "_HE", TRUE);
}

/*
 desc : 축이 정지하고 있는 동안 어떻게 정지하고 있을지 즉, 정지 상태에 대한 비례 계수 (KP) 값 설정 / 반환
		즉, 축이 정지하는 동안 축에 대한 부하 값을 설정함으로써, 축의 떨림?을 조정할 수 있음?
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		value	- [in]  proportional coefficient of Axis
 note : 이 값 (parameter)을 잘못 설정하면 축이 동작 중에 servo가 off 될 수 있음
*/
API_EXPORT BOOL uvMDR_SetPropCoeffAxisHold(ENG_RANC axis, UINT32 value)
{
	CHAR szCmd[32]		= {NULL}, szAxis[8] = {NULL};
	if (axis > ENG_RANC::rac_z)	return FALSE;

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	sprintf_s(szCmd, 32, "_HP %s %u", szAxis, value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__hp, szCmd);
}
API_EXPORT BOOL uvMDR_GetPropCoeffAxisHold()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__hp, "_HP", TRUE);
}

/*
 desc : 홈 스위치의 첫 번째 인덱스 펄스 또는 앱솔루트 엔코더의 제로 위치와 축 좌표계의 원점 사이의 거리 지정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		value	- [in]  오차 회전 각도 혹은 거리 (Offset) 값 (단위: angle, um)
 retn : TRUE or FALSE
 note : 이 명령은 배터리 지원 앱솔루트 엔코더가 있는 축에 사용해서는 안됩니다. 대신 _EO 명령을 사용해야 합니다
*/
API_EXPORT BOOL uvMDR_SetAxisHomeOffsetValue(ENG_RANC axis, DOUBLE value)
{
	CHAR szCmd[32]		= {NULL}, szAxis[8] = {NULL};
	ENG_RSCC enCmd[4]	= { ENG_RSCC::rsc__ho_t, ENG_RSCC::rsc__ho_r1,
							ENG_RSCC::rsc__ho_r2,ENG_RSCC::rsc__ho_z };
	if (axis > ENG_RANC::rac_z)	return FALSE;

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	sprintf_s(szCmd, 32, "_HO %s %d", szAxis, GetAxisPosDev(axis, value));
	return g_pMDRThread->SendData(enCmd[UINT8(axis)], szCmd);
}
API_EXPORT BOOL uvMDR_GetAxisHomeOffset()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__ho, "_HO");
}

/*
 desc : PID servo parameter의 부분 (적분 한계)이며, 축 동작에 매우 중요한 역할을 담당
		이 값은 공장 출하시 튜닝된 값으로 나오며, 변경할 때는 매우 신중해야 함
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		value	- [in]  Integral limit (적분한계) limit of axis
 note : 이 값 (parameter)을 잘못 설정하면 축이 동작 중에 servo가 off 될 수 있음
*/
API_EXPORT BOOL uvMDR_SetIntegralLimit(ENG_RANC axis, DOUBLE value)
{
	CHAR szCmd[32]		= {NULL}, szAxis[8] = {NULL};
	if (axis > ENG_RANC::rac_z)	return FALSE;

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	sprintf_s(szCmd, 32, "_IL %s %d", szAxis, GetAxisPosDev(axis, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc__il, szCmd);
}
API_EXPORT BOOL uvMDR_GetIntegralLimit()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__il, "_IL", TRUE);
}

/*
 desc : 축 이동에 대한 미분 계수 (KD) 값 설정 / 반환
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		value	- [in]  Differential Coefficient of Axis
 note : 공장에서 미리 설정되어 나오므로, 변경할 때, 신중해야 함
		이 값 (parameter)을 잘못 설정하면 축이 동작 중에 servo가 off 될 수 있음
*/
API_EXPORT BOOL uvMDR_SetDiffCoeffAxis(ENG_RANC axis, UINT32 value)
{
	CHAR szCmd[32]		= {NULL}, szAxis[8] = {NULL};
	if (axis > ENG_RANC::rac_z)	return FALSE;

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	sprintf_s(szCmd, 32, "_KD %s %u", szAxis, value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__kd, szCmd);
}
API_EXPORT BOOL uvMDR_GetDiffCoeffAxis()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__kd, "_KD", TRUE);
}

/*
 desc : 축 이동에 대한 적분 계수 (KI) 값 설정 / 반환
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		value	- [in]  Differential Coefficient of Axis
 note : 공장에서 미리 설정되어 나오므로, 변경할 때, 신중해야 함
		이 값 (parameter)을 잘못 설정하면 축이 동작 중에 servo가 off 될 수 있음
*/
API_EXPORT BOOL uvMDR_SetInteCoeffAxis(ENG_RANC axis, UINT32 value)
{
	CHAR szCmd[32]		= {NULL}, szAxis[8] = {NULL};
	if (axis > ENG_RANC::rac_z)	return FALSE;

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	sprintf_s(szCmd, 32, "_KI %s %u", szAxis, value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__ki, szCmd);
}
API_EXPORT BOOL uvMDR_GetInteCoeffAxis()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__ki, "_KI", TRUE);
}

/*
 desc : 축 이동에 대한 비례 계수 (KP) 값 설정 / 반환
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		value	- [in]  Differential Coefficient of Axis (Unit : ?)
 note : 공장에서 미리 설정되어 나오므로, 변경할 때, 신중해야 함
		이 값 (parameter)을 잘못 설정하면 축이 동작 중에 servo가 off 될 수 있음
*/
API_EXPORT BOOL uvMDR_SetPropCoeffAxis(ENG_RANC axis, UINT32 value)
{
	CHAR szCmd[32]		= {NULL}, szAxis[8] = {NULL};
	if (axis > ENG_RANC::rac_z)	return FALSE;

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	sprintf_s(szCmd, 32, "_KP %s %u", szAxis, value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__kp, szCmd);
}
API_EXPORT BOOL uvMDR_GetPropCoeffAxis()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__kp, "_KP", TRUE);
}

/*
 desc : 현재 축의 가속도/감속도 설정 / 반환
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		type	- [in]  가속도 (0x00), 감속도 (0x01)
		value	- [in]  가속도/감속도 값 (단위: um or degree)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetAxisAcceleration(ENG_RANC axis, ENG_MAST type, DOUBLE value)
{
	CHAR szCmd[32]		= {NULL}, szType[2][8] = { "ACL", "DCL" }, szAxis[8] = {NULL};
	ENG_RSCC enCmd[2]	= { ENG_RSCC::rsc_acl, ENG_RSCC::rsc_dcl };

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	sprintf_s(szCmd, 32, "%s %s %d",
			  szType[UINT8(type)], szAxis, GetAxisPosDev(axis, value));
	return g_pMDRThread->SendData(enCmd[UINT8(type)], szCmd);
}
API_EXPORT BOOL uvMDR_GetAxisAcceleration(ENG_MAST type)
{
	CHAR szCmd[2][8]	= { "ACL", "DCL" };
	ENG_RSCC enCmd[2]	= { ENG_RSCC::rsc_acl, ENG_RSCC::rsc_dcl };
	return g_pMDRThread->SendData(enCmd[UINT8(type)],  szCmd[UINT8(type)], TRUE);
}

/*
 desc : 현재 축의 가가속도/가감속도 설정 / 반환
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		type	- [in]  가속도 (0x00), 감속도 (0x01)
		value	- [in]  가감속도 값
						Non-dimensional unit, ranging from 25 to 950, with 25 being weakest, and 950 strongest jerk setting
 retn : TRUE or FALSE
 note : Acceleration Jerk의 적용 가능성은 내부 VPG (Velocity Profile Generator)에 따라 다릅니다.
		가속 저크는 VPG가 '1'(베 지어 유형)로 설정된 경우 고려되지만 VPG가 0 (기본값, 스플라인 유형)으로
		설정된 경우 무시 됩니다.
*/
API_EXPORT BOOL uvMDR_SetAxisAccelerationJerk(ENG_RANC axis, ENG_MAST type, UINT16 value)
{
	CHAR szCmd[32]		= {NULL}, szType[2][8] = { "AJK", "DJK" }, szAxis[8] = {NULL};
	ENG_RSCC enCmd[2]	= { ENG_RSCC::rsc_ajk, ENG_RSCC::rsc_djk };

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	sprintf_s(szCmd, 32, "%s %s %u", szType[UINT8(type)], szAxis, value);
	return g_pMDRThread->SendData(enCmd[UINT8(type)], szCmd);
}
API_EXPORT BOOL uvMDR_GetAxisAccelerationJerk(ENG_MAST type)
{
	CHAR szCmd[2][8]	= { "AJK", "DJK" };
	ENG_RSCC enCmd[2]	= { ENG_RSCC::rsc_ajk, ENG_RSCC::rsc_djk };
	return g_pMDRThread->SendData(enCmd[UINT8(type)],  szCmd[UINT8(type)], TRUE);
}

/*
 desc : 현재 Wafer가 Loaded될 때, 축의 가속도/감속도 설정 / 반환
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		type	- [in]  가속도 (0x00), 감속도 (0x01)
		value	- [in]  가속도/감속도 값 (단위: um or degree) (Wafer가 loaded 될 때)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetAxisLoadedAcceleration(ENG_RANC axis, ENG_MAST type, DOUBLE value)
{
	CHAR szCmd[32]		= {NULL}, szType[2][8] = { "LACL", "LDCL" }, szAxis[8] = {NULL};
	ENG_RSCC enCmd[2]	= { ENG_RSCC::rsc_lacl, ENG_RSCC::rsc_ldcl };

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	sprintf_s(szCmd, 32, "%s %s %d",
			  szType[UINT8(type)], szAxis, GetAxisPosDev(axis, value));
	return g_pMDRThread->SendData(enCmd[UINT8(type)], szCmd);
}
API_EXPORT BOOL uvMDR_GetAxisLoadedAcceleration(ENG_MAST type)
{
	CHAR szCmd[2][8]	= { "LACL", "LDCL" };
	ENG_RSCC enCmd[2]	= { ENG_RSCC::rsc_lacl, ENG_RSCC::rsc_ldcl };
	return g_pMDRThread->SendData(enCmd[UINT8(type)],  szCmd[UINT8(type)], TRUE);
}

/*
 desc : 현재 Wafer가 Loaded될 때, 축의 가가속도/가감속도 설정 / 반환
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		type	- [in]  가속도 (0x00), 감속도 (0x01)
		value	- [in]  가감속도 값 (단위: um or degree) (Wafer가 loaded 될 때)
 retn : TRUE or FALSE
 note : Acceleration Jerk의 적용 가능성은 내부 VPG (Velocity Profile Generator)에 따라 다릅니다.
		가속 저크는 VPG가 '1'(베 지어 유형)로 설정된 경우 고려되지만 VPG가 0 (기본값, 스플라인 유형)으로
		설정된 경우 무시 됩니다.
*/
API_EXPORT BOOL uvMDR_SetAxisLoadedAccelerationJerk(ENG_RANC axis, ENG_MAST type, UINT16 value)
{
	CHAR szCmd[32]		= {NULL}, szType[2][8] = { "LAJK", "LDJK" }, szAxis[8] = {NULL};
	ENG_RSCC enCmd[2]	= { ENG_RSCC::rsc_lajk, ENG_RSCC::rsc_ldjk };

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	sprintf_s(szCmd, 32, "%s %s %u", szType[UINT8(type)], szAxis, value);
	return g_pMDRThread->SendData(enCmd[UINT8(type)], szCmd);
}
API_EXPORT BOOL uvMDR_GetAxisLoadedAccelerationJerk(ENG_MAST type)
{
	CHAR szCmd[2][8]	= { "LAJK", "LDJK" };
	ENG_RSCC enCmd[2]	= { ENG_RSCC::rsc_lajk, ENG_RSCC::rsc_ldjk };
	return g_pMDRThread->SendData(enCmd[UINT8(type)],  szCmd[UINT8(type)], TRUE);
}

/*
 desc : 현재 Wafer가 Loaded될 때, 축 속도 설정 / 반환
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		value	- [in]  속도 값 (단위: um or degree) (Wafer가 loaded 될 때)
 retn : TRUE or FALSE
 note : 이 명령은 Wafer와 함께 Loaded 될 때 축의 속도를 설정 하거나 반환
		LENB 명령으로 활성화된 경우, Pick commands (GET, MSS 등) 중에 Wafer가 Arm에서 pick-up된 후
		축의 속도가 LSPD 명령으로 지정된 값으로 설정 
*/
API_EXPORT BOOL uvMDR_SetAxisLoadedSpeed(ENG_RANC axis, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL}, szAxis[8] = {NULL};

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	sprintf_s(szCmd, 32, "LSPD %s %d", szAxis, GetAxisPosDev(axis, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_lspd, szCmd);
}
API_EXPORT BOOL uvMDR_GetAxisLoadedSpeed()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_lspd, "LSPD", TRUE);
}

/*
 desc : 현재 축의 Working Speed 설정 / 반환
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		value	- [in]  속도 값 (단위: um or degree) (Wafer가 loaded 될 때)
 retn : TRUE or FALSE
 note : 이 명령은 Wafer와 함께 Loaded 될 때 축의 속도를 설정 하거나 반환
		LENB 명령으로 활성화된 경우, Pick commands (GET, MSS 등) 중에 Wafer가 Arm에서 pick-up된 후
		축의 속도가 LSPD 명령으로 지정된 값으로 설정 
*/
API_EXPORT BOOL uvMDR_SetAxisWorkingSpeed(ENG_RANC axis, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL}, szAxis[8] = {NULL};

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	sprintf_s(szCmd, 32, "SPD %s %d", szAxis, GetAxisPosDev(axis, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_spd, szCmd);
}
API_EXPORT BOOL uvMDR_GetAxisWorkingSpeed()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_spd, "SPD", TRUE);
}

/* --------------------------------------------------------------------------------------------- */
/*           Command Reference  (7.Axis parameter sets, and custom parameters commands)          */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 사용자가 특정 축에 대해 미리 정의된 값을 설정 / 반환
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		param	- [in]  사용자에 의해 지정된 인덱스 값 (0x00 ~ 0x09)
		value	- [in]  사용자에 의해 지정된 값
 retn : TRUE or FALSE
 note : 이 명령은 사용자가 자주 축별로 자주 사용하는 값을 최대 10개까지 임시로 저장 후
		필요할 때마다 꺼내서 사용할 수 있음
*/
API_EXPORT BOOL uvMDR_SetCustomAxisParam(ENG_RANC axis, UINT8 param, INT32 value)
{
	CHAR szCmd[32]	= {NULL}, szAxis[8] = {NULL};

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	sprintf_s(szCmd, 32, "_CAX %s %u %d", szAxis, param, value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__cax, szCmd);
}
API_EXPORT BOOL uvMDR_GetCustomAxisParam(ENG_RANC axis, UINT8 param)
{
	CHAR szCmd[32]	= {NULL}, szAxis[8] = {NULL};

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	sprintf_s(szCmd, 32, "_CAX %s %u", szAxis, param);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__cax, szCmd, TRUE);
}

/*
 desc : 사용자 지정 명령 구간 (번호)로서 최대 20개까지 설정 / 반환
 parm : param	- [in]  사용자 지정 명령에 해당되는 Index (mandatory parameter number) 값 (0 ~ 19)
		value	- [in]  사용자에 의해 지정할 수 있는 임의의 숫자 값 (INT32_MIN ~ INT32_MAX)
 retn : TRUE or FALSE
 note : CSV 명령을 통해서, 사용자 지정 파라미터 값을 저장할 수 있음
*/
API_EXPORT BOOL uvMDR_SetCustomGenericParam(UINT8 param, INT32 value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_CPA %u %d", param, value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__cpa, szCmd);
}
API_EXPORT BOOL uvMDR_GetCustomGenericParam(UINT8 param)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_CPA %u", param);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__cpa, szCmd, TRUE);
}

/*
 desc : 사용자가 특정 Station Name에 대해 미리 정의된 값을 설정 / 반환
 parm : station	- [in]  Station Name (문자 1)
		param	- [in]  사용자에 의해 지정된 인덱스 값 (0x00 ~ 0x04)
		value	- [in]  사용자에 의해 지정된 값
 retn : TRUE or FALSE
 note : 이 명령은 사용자가 자주 축별로 자주 사용하는 값을 최대 5개까지 임시로 저장 후
		필요할 때마다 꺼내서 사용할 수 있음
*/
API_EXPORT BOOL uvMDR_SetCustomStationParam(CHAR station, UINT8 param, INT32 value)
{
	CHAR szCmd[32]	= {NULL};

	sprintf_s(szCmd, 32, "_CST %c %u %d", station, param, value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__cst, szCmd);
}
API_EXPORT BOOL uvMDR_GetCustomStationParam(CHAR station, UINT8 param)
{
	CHAR szCmd[32]	= {NULL};

	sprintf_s(szCmd, 32, "_CST %c %u", station, param);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__cst, szCmd, TRUE);
}

/*
 desc : 주어진 Axis에 대한 모든 설정들에 대한 모든 파라미터 정보를 포함하고 있는 테이블 정보 반환
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
 retn : TRUE or FALSE
 note : 이 명령은 주어진 축에 대한 모든 세트 및 모든 매개 변수에 대한 테이블을 보고 합니다.
		특정 매개 변수 세트가 유효하지 않은 경우 해당 세트로 표시되고 테이블 끝에 메모가 표시 됩니다.
*/
API_EXPORT BOOL uvMDR_GetAllParamSetsSingleAxis(ENG_RANC axis)
{
	CHAR szCmd[32]	= {NULL}, szAxis[8] = {NULL};

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);

	sprintf_s(szCmd, 32, "DAS %s", szAxis);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_das, szCmd, TRUE);
}

/*
 desc : 파마리터 설정 (Parameter Set) 정보 반환
 parm : value	- [in]  parameter set number (0 ~ 9)
 retn : TRUE or FALSE
 note : 이 명령은 주어진 매개 변수 세트에 대한 모든 축과 모든 매개 변수에 대한 테이블 보고
		이 매개 변수 세트가 유효하지 않으면 테이블 끝에 메모가 표시
*/
API_EXPORT BOOL uvMDR_GetParameterSetInfo(UINT8 value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "DPS %u", value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_dps, szCmd, TRUE);
}

/*
 desc : Homing 동작할 때, 모든 축의 모션 속도를 Homing에 맞도록 설정
 parm : None
 retn : TRUE or FALSE
 note : Homing 하기 전에 이 명령이 반드시 호출되어야 함. Homing 할 때 모션의 속도는 느리게 해야 하므로...
*/
API_EXPORT BOOL uvMDR_SetHomingAllAxisMotionParam()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_hsa, "HSA");
}

/*
 desc : Loading 동작할 때, 모든 축의 모션 속도를 Loading에 맞도록 설정
 parm : None
 retn : TRUE or FALSE
 note : Loading 하기 전에 이 명령이 반드시 호출되어야 함
*/
API_EXPORT BOOL uvMDR_SetLoadedAllAxisMotionParam()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_lsa, "LSA");
}

/*
 desc : Working 동작할 때, 모든 축의 모션 속도를 Working에 맞도록 설정
 parm : None
 retn : TRUE or FALSE
 note : Working 하기 전에 이 명령이 반드시 호출되어야 함
*/
API_EXPORT BOOL uvMDR_SetWorkingAllAxisMotionParam()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_wsa, "WSA");
}

/* --------------------------------------------------------------------------------------------- */
/*                       Command Reference  (8.Basic axis motions commands)                      */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 홈 스위치와 절대 인코더의 영점 (Zero Position) 사이의 거리
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
						절대로 'A' 즉, All 명령은 수행되지 않음
 retn : TRUE or FALSE
 note : 이 명령은 마지막 회전에서 홈 스위치와 절대 인코더의 영점 위치 사이의 거리를 측정하는
		절차를 시작 합니다. 측정된 거리는 엔코더 수로 반환 됩니다.
*/
API_EXPORT BOOL uvMDR_GetDistanceFromHomeSwitch(ENG_RANC axis)
{
	CHAR szCmd[32]	= {NULL}, szAxis[8] = {NULL};

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);
	sprintf_s(szCmd, 32, "_AFE %s", szAxis);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__afe, szCmd, TRUE);
}
API_EXPORT BOOL uvMDR_GetDistanceFromHomeSwitchEx(ENG_RANC axis)
{
	CHAR szCmd[32]	= {NULL}, szAxis[8] = {NULL};

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);
	sprintf_s(szCmd, 32, "AFE %s", szAxis);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_afe, szCmd, TRUE);
}

/*
 desc : Find Edge (Home Switch와 Encoder의 첫 번째 인덱스 펄스 발생 위치 간의 거리 값)
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
 retn : TRUE or FALSE
 note : Axis units, and Encoder counts 값으로 반환
		ex> >_FE T
			Home Switch to index distance: 546 (4572 ec)
			>
*/
API_EXPORT BOOL uvMDR_SetFindEdgeAxis(ENG_RANC axis)
{
	CHAR szCmd[32]	= {NULL}, szAxis[8] = {NULL};

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);
	sprintf_s(szCmd, 32, "_FE %s", szAxis);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__fe, szCmd, TRUE);
}

/*
 desc : Find Home Switch
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
 retn : TRUE or FALSE
 note : This command starts a procedure which moves specified axis over position where home switch triggers
*/
API_EXPORT BOOL uvMDR_SetFindHomeSwitch(ENG_RANC axis)
{
	CHAR szCmd[32]	= {NULL}, szAxis[8] = {NULL};

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);
	sprintf_s(szCmd, 32, "_FH %s", szAxis);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__fh, szCmd);
}

/*
 desc : Home an Axis (Homing 시작)
		Homing 이후 첫 번째 인덱스 펄스 위치로 이동 하며, 해당 위치가 좌표로는 0 값임
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
 retn : TRUE or FALSE
 note : axis 축의 값이 'A' 즉, All인 경우, 전체 Homing
*/
API_EXPORT BOOL uvMDR_StartHomeAxis(ENG_RANC axis)
{
	CHAR szCmd[32]	= {NULL}, szAxis[8] = {NULL};

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);
	if (ENG_RANC::rac_a != axis)	sprintf_s(szCmd, 32, "_HM %s", szAxis);
	else							strcpy_s(szCmd, 32, "_HM");
	return g_pMDRThread->SendData(ENG_RSCC::rsc__hm, szCmd);
}

/*
 desc : Homing 절차 방법
 parm : option	- [in]  1 ~ 3 or -1
 retn : TRUE or FALSE
 note : Option 값에 따라 동작 (Homing)이 상이하게 진행
		0 : HOM 0은 증분 및 Absolute Encoder가 있는 모터의 표준 원점 복귀 절차
			Robot paddle(s)에 Wafer 유무를 확인하고, 명확한 경우 원점 복귀 절차를 실행 한 다음
			모든 축을 사용자 정의 홈 위치로 이동
		1 : HOM 1은 증분 및 Absolute Encoder가 있는 모터의 표준 원점 복귀 절차
			이 옵션을 사용하면 웨이퍼 유무 확인이 수행되지 않음
		2 : HOM 2는 배터리 지원 Absolute Encoder가 있는 로봇에서 사용
			Robot paddle(s)은 Wafer의 존재 여부를 확인하고, 명확한 경우 추가 동작없이 로봇이 홈 상태로 설정
		3 : HOM 3는 배터리 지원 Absolute Encoder가 있는 로봇에서 사용
			Wafer 유무 확인이 수행되지 않고 추가 동작없이 로봇이 홈 상태로 설정
		-1: HOM -1은 Absolute Encoder가 위치를 잃었을 때 제로 위치를 복구하기 위해
			배터리 지원 Absolute Encoder가 있는 로봇에서 사용
*/
API_EXPORT BOOL uvMDR_SetHomeMethod(INT8 option)
{
	CHAR szCmd[32]	= {NULL};
	if (!(-2 < option && option < 4))
	{
		LOG_WARN(ENG_EDIC::en_lsdr, L"HOM [Option] : Invalid option parameter [ -1, 0, 1, 2, 3]");
		return FALSE;
	}

	sprintf_s(szCmd, 32, "HOM %d", option);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_hom,  szCmd);
}

/*
 desc : 3개 축을 동시에 특정 위치로 이동
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
 retn : TRUE or FALSE
 note : axis 축의 값이 'A' 즉, All인 경우, 전체 Home 위치로 이동
*/
API_EXPORT BOOL uvMDR_MoveHomePosition(ENG_RANC axis)
{
	CHAR szCmd[32]	= {NULL}, szAxis[8] = {NULL};

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);
	if (ENG_RANC::rac_a != axis)	sprintf_s(szCmd, 32, "MTH %s", szAxis);
	else							strcpy_s(szCmd, 32, "MTH");
	return g_pMDRThread->SendData(ENG_RSCC::rsc_mth, szCmd);
}

/*
 desc : 3개 축을 동시에 특정 위치로 이동
 parm : axis_t	- [in]  회전 축 (단위: 각도)
		axis_r1	- [in]  Dual Arm 중 Primary Axis 수평 위치 (단위: um)
		axis_r2	- [in]  Dual Arm 중 Secondary Axis 수평 위치 (단위: um)
		axis_z	- [in]  수직 위치 (단위: um)
		retract	- [in]  Retracted R 즉, 2개의 Arm 수평 (R) 축의 위치를 일단 먼저 이 위치로 이동
 retn : TRUE or FALSE
 note : K 축은 해당 사항 없음 
*/
API_EXPORT BOOL uvMDR_MoveAxisAll(DOUBLE axis_t, DOUBLE axis_r1, DOUBLE axis_r2, DOUBLE axis_z)
{
	CHAR szCmd[64]	= {NULL};
	sprintf_s(szCmd, 64, "MTP %d %d %d %d",
			  GetAxisPosDev(ENG_RANC::rac_t,	axis_t),
			  GetAxisPosDev(ENG_RANC::rac_r1,	axis_r1),
			  GetAxisPosDev(ENG_RANC::rac_r2,	axis_r2),
			  GetAxisPosDev(ENG_RANC::rac_z,	axis_z));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_mtp, szCmd);
}
API_EXPORT BOOL uvMDR_MoveAxisExtAll(DOUBLE axis_t, DOUBLE axis_r1, DOUBLE axis_r2, DOUBLE axis_z,
									 DOUBLE retract)
{
	CHAR szCmd[64]	= {NULL};
	sprintf_s(szCmd, 64, "MTP %d %d %d %d %d",
			  GetAxisPosDev(ENG_RANC::rac_t,	axis_t),
			  GetAxisPosDev(ENG_RANC::rac_r1,	axis_r1),
			  GetAxisPosDev(ENG_RANC::rac_r2,	axis_r2),
			  GetAxisPosDev(ENG_RANC::rac_z,	axis_z),
			  GetAxisPosDev(ENG_RANC::rac_r1,	retract));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_mtp, szCmd);
}

/*
 desc : Robot을 지정된 station의 후퇴 위치로 이동
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		slot	- [in]  Slot Number (1 ~ 25?)
		option	- [in]  Vertical Position 지정 값 (0, 1, 2)
						0x00: veritcal position to leave the slot (SPO + OFS), default (if no specified)
						0x01: veritcal position to enter the slot (SPO + OFS - STR3) -> Manual 참고
						0x02: vertical position at slot position for current slot (SPO)
 retn : TRUE or FALSE
 note : moves the robot to retracted position of specified station and slot.
		Option parameter specifies where within the slot to position the arm
*/
API_EXPORT BOOL uvMDR_MoveAxisRetractedPosition(CHAR station, UINT8 slot, UINT8 option)
{
	CHAR szCmd[64]	= {NULL};
#if 0
	if (station == 0x00)	sprintf_s(szCmd, 64, "MTR %c %u %u", station, slot, option);
	else					sprintf_s(szCmd, 64, "MTR");
#else
	sprintf_s(szCmd, 64, "MTR %c %u %u", station, slot, option);
#endif
	return g_pMDRThread->SendData(ENG_RSCC::rsc_mtr, szCmd);
}

/*
 desc : 절대 위치로 이동
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		position- [in]  절대 위치로 이동하고자 하는 각도 혹은 위치 값 (단위: degree or um)
 retn : TRUE or FALSE
 note : HOM 명령 이전에 이 명령 실행 자제
*/
API_EXPORT BOOL uvMDR_MoveAbsolutePosition(ENG_RANC axis, DOUBLE position)
{
	CHAR szCmd[32]	= {NULL}, szAxis[8] = {NULL};

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);
	sprintf_s(szCmd, 32, "MVA %s %d", szAxis, GetAxisPosDev(axis, position));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_mva, szCmd);
}

/*
 desc : 상대 위치로 이동
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		distance- [in]  상대 위치로 이동하고자 하는 각도 혹은 위치 값 (단위: degree or um)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_MoveRelativePosition(ENG_RANC axis, DOUBLE distance)
{
	CHAR szCmd[32]	= {NULL}, szAxis[8] = {NULL};

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);
	sprintf_s(szCmd, 32, "MVR %s %d", szAxis, GetAxisPosDev(axis, distance));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_mvr, szCmd);
}

/*
 desc : End Effector를 직선으로 이동 시킴
 parm : pos_t	- [in]  최종 각도 위치 (degree)
		pos_r	- [in]  최종 Arm 위치 (um)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_MoveOverStraightLine(DOUBLE pos_t, DOUBLE pos_r)
{
	CHAR szCmd[32]		= {NULL};
	sprintf_s(szCmd, 32, "MVT1 %d %d",
			  GetAxisPosDev(ENG_RANC::rac_t, pos_t),
			  GetAxisPosDev(ENG_RANC::rac_r1, pos_r));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_mvt1, szCmd);
}

/*
 desc : End Effector를 직선으로 이동 시키는데, 2 회에 걸쳐서 이동 함
 parm : pos_t1	- [in]  처음 이동 각도 위치 (degree)
		pos_r1	- [in]  처음 이동 Arm 위치 (um)
		pos_t2	- [in]  최종 각도 위치 (degree)
		pos_r2	- [in]  최종 Arm 위치 (um)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_MoveOverTwoStraightLine(DOUBLE pos_t1, DOUBLE pos_r1, DOUBLE pos_t2, DOUBLE pos_r2)
{
	CHAR szCmd[32]		= {NULL};
	sprintf_s(szCmd, 32, "MVT2 %d %d %d %d",
			  GetAxisPosDev(ENG_RANC::rac_t, pos_t1),
			  GetAxisPosDev(ENG_RANC::rac_r1, pos_r1),
			  GetAxisPosDev(ENG_RANC::rac_t, pos_t2),
			  GetAxisPosDev(ENG_RANC::rac_r1, pos_r2));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_mvt2, szCmd);
}

/*
 desc : Station-less inline motion 명령 (MOV, MVT1, MVT2)과 함게 사용할
		End Effector 길이와 Cornering Radius (코너링 반경)을 설정
 parm : ee_len	- [in]  Length of End Effector (um)
		radius	- [in]  코너링 반경 (degree)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetEELenRadiusForStationLessMove(DOUBLE ee_len, DOUBLE radius)
{
	CHAR szCmd[32]		= {NULL};
	sprintf_s(szCmd, 32, "MVTP %u %u",
			  GetAxisPosDev(ENG_RANC::rac_r1, ee_len),
			  GetAxisPosDev(ENG_RANC::rac_t, radius));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_mvtp, szCmd);
}

/*
 desc : 설정된 감속으로 지정된 축 혹은 전체 축의 Motion을 중지 시킴
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_StopMotionTask(ENG_RANC axis)
{
	CHAR szCmd[32]	= {NULL}, szAxis[8] = {NULL};

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);
	if (ENG_RANC::rac_a != axis)	sprintf_s(szCmd, 32, "STP %s", szAxis);
	else							sprintf_s(szCmd, 32, "STP");
	return g_pMDRThread->SendData(ENG_RSCC::rsc_stp, szCmd);
}

/*
 desc : 모션이 멈춘 상태가 될 때까지 펌웨어로부터 응답 대기 함
 parm : None
 retn : TRUE or FALSE
 note : 현재 모션이 동작 중이거나 어떤 상태인지 모를 경우, 모션이 멈출 때까지 응답 기다림
*/
API_EXPORT BOOL uvMDR_SetWaitForMotionCompleted()
{
	/* 반드시 아래 플래그 초기화 */
	g_pstShMemMDR->motion_wait_finished	= 0x00;	/* 현재 모션이 멈추지 않은 상태라고 설정 */
	return g_pMDRThread->SendData(ENG_RSCC::rsc_wmc, "WMC");
}

/* --------------------------------------------------------------------------------------------- */
/*                       Command Reference  (9.Station parameters commands)                      */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Station Set Group 번호 설정 (1 ~ 4) / 반환
 parm : value	- [in]  Station Set Number (1 ~ 4)
 retn : TRUE or FALSE
 note : 현재 Station Name (소문자/대문자)으로 설정된 정보에 대해 Station Set Number로 설정 및 저장
*/
API_EXPORT BOOL uvMDR_SetCurrentStationSet(UINT8 value)
{
	CHAR szCmd[32]	= {NULL};
	if (value < 1 || value > 4)	return FALSE;
	sprintf_s(szCmd, 32, "_CSS %u", value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__css, szCmd);
}
API_EXPORT BOOL uvMDR_GetCurrentStationSet()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__css, "_CSS", TRUE);
}

/*
 desc : Station 마다 "GET, GETW, GETC 등" 작업을 수행할 때, R 축의 Pushing Acceleration 값 설정
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  가속도 값 (단위: um/sec^2)
 retn : TRUE or FALSE
 note : GetRAxisPusingAccDefault() 함수는 Station마다 부여된 Pushing Acceleration의 기본 값 출력
		GET, GETW, GETC 등 작업을 수행할 때, R 축이 앞으로 밀고 들어갈 때, 이동하는 가속도 값 설정 / 반환
*/
API_EXPORT BOOL uvMDR_SetRAxisPusingAcceleration(CHAR station, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_PA %c %d", station, g_pMDRThread->GetPosToInch(ENG_RUTI::uti_00, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc__pa, szCmd);
}
API_EXPORT BOOL uvMDR_GetRAxisPusingAcceleration(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_PA %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__pa, szCmd, TRUE);
}
API_EXPORT BOOL uvMDR_GetRAxisPusingAccelerationDefault()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__pa_def, "_PA", TRUE);
}

/*
 desc : Station 마다 "GET" 작업을 수행할 때, R 축의 Pushing Speed (이동 속도) 값 설정
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  속력 값 (단위: um/sec)
 retn : TRUE or FALSE
 note : GetRAxisPusingAccDefault() 함수는 Station마다 부여된 Pushing Soeed의 기본 값 출력
*/
API_EXPORT BOOL uvMDR_SetRAxisPusingSpeed(CHAR station, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_PS %c %d", station, g_pMDRThread->GetPosToInch(ENG_RUTI::uti_00, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc__ps, szCmd);
}
API_EXPORT BOOL uvMDR_GetRAxisPusingSpeed(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_PS %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__ps, szCmd, TRUE);
}
API_EXPORT BOOL uvMDR_GetRAxisPusingSpeedDefault()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__ps_def, "_PS", TRUE);
}

/*
 desc : 첫 번째 슬롯 기준 마지막 Slot의 위치 보상 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		value	- [in]  속력 값 (단위: um)
 retn : TRUE or FALSE
 note : 이 명령은 Z (수직) 축에 대해 수직으로 완벽하게 정렬되지 않은 Station에서 선택 및
		배치 작업을 수행하는데 유용 할 수 있습니다. 이 명령은 해당 Station의 첫 번째 슬롯
		기준으로 마지막 슬롯의 오프셋을 지정하고 해당 슬롯에 비례하여 T, R 및 K 좌표를 보정
*/
API_EXPORT BOOL uvMDR_SetStationCoordCompensationSlot(CHAR station, ENG_RANC axis, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL}, szAxis[8] = {NULL};

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);
	sprintf_s(szCmd, 32, "_ZC %c %s %d", station, szAxis, GetAxisPosDev(axis, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc__zc, szCmd);
}
API_EXPORT BOOL uvMDR_GetStationCoordCompensationSlot(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_ZC %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__zc, szCmd, TRUE);
}

/*
 desc : Inline Station의 좌표계와 파라미터 값을 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
 retn : TRUE or FALSE
 note : 이 명령은 인라인으로 정의된 Station에 대해 다양한 파라미터와 계산된 위치를 보고 합니다.
		이 정보는 인라인 Station 파라미터를 설정하고 미세 조정하는데 유용할 수 있습니다
*/
API_EXPORT BOOL uvMDR_GetInlineStationApproachPosition(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "APP %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_app, szCmd, TRUE);
}

/*
 desc : GET or PUT 동작 중에 R 축의 Linear Postion의 오차를 보정하기 위한 값 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  보정 값 (단위: um)
 retn : TRUE or FALSE
 note : 이 명령은 Z (수직) 축에 대해 수직으로 완벽하게 정렬되지 않은 Station에서 선택 및
		배치 작업을 수행하는데 유용 할 수 있습니다. 이 명령은 해당 Station의 첫 번째 슬롯
		기준으로 마지막 슬롯의 오프셋을 지정하고 해당 슬롯에 비례하여 T, R 및 K 좌표를 보정
*/
API_EXPORT BOOL uvMDR_SetStationCenterOffset(CHAR station, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "COF %c %d", station,
			  GetAxisPosDev(ENG_RANC::rac_r1, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_cof, szCmd);
}
API_EXPORT BOOL uvMDR_GetStationCenterOffset(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "COF %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_cof, szCmd, TRUE);
}

/*
 desc : GET or PUT 동작 중에 R 축의 Linear Postion의 오차를 보정하기 위한 값 설정 / 반환 (Edge gripper 용도)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  보정 값 (단위: um)
 retn : TRUE or FALSE
 note : 이 명령은 Z (수직) 축에 대해 수직으로 완벽하게 정렬되지 않은 Station에서 선택 및
		배치 작업을 수행하는데 유용 할 수 있습니다. 이 명령은 해당 Station의 첫 번째 슬롯
		기준으로 마지막 슬롯의 오프셋을 지정하고 해당 슬롯에 비례하여 T, R 및 K 좌표를 보정
*/
API_EXPORT BOOL uvMDR_SetStationCenterOffsetEdgeGrip(CHAR station, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "COFP %c %d", station,
			  GetAxisPosDev(ENG_RANC::rac_r1, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_cofp, szCmd);
}
API_EXPORT BOOL uvMDR_GetStationCenterOffsetEdgeGrip(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "COFP %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_cofp, szCmd, TRUE);
}

/*
 desc : 현재 Station의 Slot 번호 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Slot Number
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetStationSlotNo(CHAR station, UINT8 value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "CSL %c %u", station, value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_csl, szCmd);
}
API_EXPORT BOOL uvMDR_GetStationSlotNo(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "CSL %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_csl, szCmd, TRUE);
}

/*
 desc : 지정된 Station 혹은 전체 Station에 부여 (할당)된 정보 제거
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetStationParamClear(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "CSP %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_csp, szCmd);
}
API_EXPORT BOOL uvMDR_SetStationParamClearAll()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_csp, "CSP #");
}

/*
 desc : 현재 Station Name 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetCurrentStationName(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "CST %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_cst, szCmd);
}
API_EXPORT BOOL uvMDR_GetCurrentStationName()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_cst, "CST");
}

/*
 desc : 현재 Station Number 설정 / 반환
 parm : value	- [in]  Station Number (1 ~ 130)
						  1 ~  26 : set_no_1,
						 27 ~  52 : set_no_2,
						 53 ~  78 : set_no_3,
						 79 ~ 104 : set_no_4,
						105 ~ 130 : set_no_5
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetCurrentStationNo(UINT8 value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "CSTN %u", value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_cstn, szCmd);
}
API_EXPORT BOOL uvMDR_GetCurrentStationNo()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_cstn, "CSTN", TRUE);
}

/*
 desc : 원본 Station (source)의 모든 내용을 대상 Station (target)의 모든 매개 변수에 복사
 parm : source	- [in]  원본 Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		target	- [in]  대상 Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetDuplicateStationParamByName(CHAR source, CHAR target)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "DSP %c %c", source, target);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_dsp, szCmd);
}

/*
 desc : 원본 Station (source)의 모든 내용을 대상 Station (target)의 모든 매개 변수에 복사
 parm : source	- [in]  source station number (1 ~ 130)
		target	- [in]  target station number (1 ~ 130)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetDuplicateStationParamByNumber(UINT8 source, UINT8 target)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "DSPN %u %u", source, target);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_dspn, szCmd);
}

/*
 desc : Station에 사용 중인 End Effector Number 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		number	- [in]  0 – use Vacuum Valve 1 and Vacuum Sensor 1 when servicing a station (default)
						1 – use Vacuum Valve 2 and Vacuum Sensor 2 when servicing a station.
							The robot has to be equipped with two vacuum lines in order for this option to be valid.
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetEndEffectorNumForStation(CHAR station, UINT8 number)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "EEN %c %u", station, number);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_een, szCmd);
}
API_EXPORT BOOL uvMDR_GetEndEffectorNumForStation(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "EEN %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_een, szCmd, TRUE);
}

/*
 desc : Station 동작 옵션에 End Effector가 Wafer를 특정 위치에 GET or PUT 동작 할 때, Z 축을 Up or Down 하기 위해 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		up_down	- [in]  0 – Service a station with End Effector in Upper position (default).
						1 – Servicing a station with End Effector in Down Position
							End Effector is below the wafer when entering (GET) or leaving (PUT) a station during pick and place operations.
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetEndEffectorUpDownForStation(CHAR station, UINT8 up_down)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "EEO %c %u", station, up_down);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_eeo, szCmd);
}
API_EXPORT BOOL uvMDR_GetEndEffectorUpDownForStation(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "EEO %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_eeo, szCmd, TRUE);
}

/*
 desc : 현재(또는 유일한) Station 세트에 정의된 Station의 이름 반환
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_GetEnumerateDefinedStations()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_ens, "ENS", TRUE);
}
/*
 desc : Station 세트 번호 별로 정의된 Station의 이름 반환
 parm : set	- [in]  Station Set Number (1 ~ 4)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_GetEnumerateDefinedStationsNum(UINT8 set)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "ENS %u", set);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_ens, szCmd, TRUE);
}
/*
 desc : 전체 Station 세트 정의된 Station의 이름들 반환
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_GetEnumerateDefinedStationsAll()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_ens, "ENS 0", TRUE);
}
/*
 desc : 전체 Station 세트마다 각각 Station 전체에 할당된 번호들 반환
 parm : None
 retn : TRUE or FALSE
 note : 출력되는 결과물에서
		Set 0    : 모든 대문자 Station 지칭,
		Set 1 ~ 4: sets 1, 2, 3 및 4에서 소문자 Station 지칭
*/
API_EXPORT BOOL uvMDR_GetEnumerateDefinedStationsAllNum()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_ens, "ENS N", TRUE);
}

/*
 desc : Station의 마지막 부분에 Wafer를 놓을 때, 이동하는 속도 계수 값 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  0 ~ 100 값 범위 (0 값인 경우, 100 값과 동일)
						1 – use Vacuum Valve 2 and Vacuum Sensor 2 when servicing a station.
							The robot has to be equipped with two vacuum lines in order for this option to be valid.
 retn : TRUE or FALSE
 note : 이 명령은 스테이션의 마지막 부분 (Cassette?)에 들어갈 때 현재 속도 매개 변수에 적용될
		속도 계수를 설정 합니다. 이 매개 변수를 100 미만의 값으로 설정하면 웨이퍼를 카세트에
		배치하는 동안 더 부드러운 움직임에 도움이 될 수 있습니다.
		이 매개 변수를 0으로 설정하면 100으로 설정하는 것과 동일한 효과가 있습니다.
*/
API_EXPORT BOOL uvMDR_SetInlineStationLastSeqmentSpeedFactor(CHAR station, UINT8 value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "ILS %c %u", station, value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_ils, szCmd);
}
API_EXPORT BOOL uvMDR_GetInlineStationLastSeqmentSpeedFactor(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "ILS %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_ils, szCmd);
}

/*
 desc : Inline Station Retracted R (Axis) Position 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  R coordinate (unit: um)
 retn : TRUE or FALSE
 note : 이 명령은 GET 운전 중 선형 출력 스테이션을 나갈 때 R 축의 후퇴 위치를 설정하고,
		PUT 운전 중 선형 입력 스테이션의 시작 위치를 설정 합니다.
		R 위치는 RPO  명령으로 정의된 위치와 동일하지 않아도 됩니다.
		Radial motions은 RPO 명령으로 정의된 위치에서 시작 됩니다
*/
API_EXPORT BOOL uvMDR_SetInlineStationRetractedRPosition(CHAR station, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "IRR %c %u", station, GetAxisPosDev(ENG_RANC::rac_r1, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_irr, szCmd);
}
API_EXPORT BOOL uvMDR_GetInlineStationRetractedRPosition(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "IRR %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_irr, szCmd);
}

/*
 desc : Inline Station Retracted T (Axis) Position 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  T coordinate (unit: degree)
 retn : TRUE or FALSE
 note : 이 명령은 GET 작동 중 선형 출력 스테이션을 나갈 때 T 축의 후퇴 위치를 설정하고,
		PUT 작동 중 선형 입력 스테이션의 시작 위치를 설정 합니다.
		방사형 동작은 스테이션 앞의 위치, 즉 스테이션의 T 좌표에서 시작됩니다.
*/
API_EXPORT BOOL uvMDR_SetInlineStationRetractedTPosition(CHAR station, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "IRT %c %d", station, GetAxisPosDev(ENG_RANC::rac_t, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_irt, szCmd);
}
API_EXPORT BOOL uvMDR_GetInlineStationRetractedTPosition(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "IRT %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_irt, szCmd);
}

/*
 desc : Inline Station Exit Angle 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		angle	- [in]  angle (degree)
 retn : TRUE or FALSE
 note : 이 명령은 T=0 Line과 Station Line 사이의 각도를 설정
		이것은 End Effector가 GET 작업 중 linear-our Station을 종료 하거나
		PUT 작업 중 linear-in station에 들어갈 때 따라야하는 방향 입니다.
*/
API_EXPORT BOOL uvMDR_SetInlineStationExitAngle(CHAR station, DOUBLE angle)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "ISA %c %d", station, GetAxisPosDev(ENG_RANC::rac_t, angle));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_isa, szCmd);
}
API_EXPORT BOOL uvMDR_GetInlineStationExitAngle(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "ISA %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_isa, szCmd);
}

/*
 desc : Inline Station Exit Distance 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		distance- [in]  distance (um)
 retn : TRUE or FALSE
 note : 이 명령은 GET 작업 중 linear-out station을 빠져 나갈 때 또는 PUT 작업 중 linear-in station에
		들어갈 때 End Effector가 직선으로 이동 해야하는 시간을 설정 합니다.
		이 매개 변수는 "Cassette Depth"라고도 합니다.
*/
API_EXPORT BOOL uvMDR_SetInlineStationExitDistance(CHAR station, DOUBLE distance)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "ISD %c %d", station, GetAxisPosDev(ENG_RANC::rac_z, distance));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_isd, szCmd);
}
API_EXPORT BOOL uvMDR_GetInlineStationExitDistance(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "ISD %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_isd, szCmd);
}

/*
 desc : Inline Station End-Effector 길이 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		length	- [in]  length (um)
 retn : TRUE or FALSE
 note : 이 명령은 인라인 스테이션에서 GET 및 PUT 작업을 서비스하는데 사용되는 End Effector의 길이를
		설정 합니다. 길이는 upper arm link의 중심과 End Effector에 놓여진 Wafer 중심 사이의 거리로
		정의 됩니다. 이 매개 변수는 각 inline station에 대해 정의 되어야 합니다
*/
API_EXPORT BOOL uvMDR_SetInlineStationEndEffectLength(CHAR station, DOUBLE length)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "ISE %c %d", station, GetAxisPosDev(ENG_RANC::rac_z, length));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_ise, szCmd);
}
API_EXPORT BOOL uvMDR_GetInlineStationEndEffectLength(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "ISE %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_ise, szCmd);
}

/*
 desc : Inline Station Corner Radius (회전하면서 Robot 동작 (수축/확장)?)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		radius	- [in]  Radius (unit: degree)
 retn : TRUE or FALSE
 note : This command sets the radius of the arc connecting the two straight lines in Linear-In/Linear-Out motions.
 exam : None
*/
API_EXPORT BOOL uvMDR_SetInlineStationCornerRadius(CHAR station, DOUBLE radius)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "ISR %c %d", station, GetAxisPosDev(ENG_RANC::rac_t, radius));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_isr, szCmd);
}
API_EXPORT BOOL uvMDR_GetInlineStationCornerRadius(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "ISR %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_isr, szCmd);
}

/*
 desc : Inline Station Type 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		type	- [in]  0: radial in / radial out, 1: radial in / linear out, 2: linear in / linear out
 retn : TRUE or FALSE
 note : 이 명령은 PUT 또는 GET 작업 중에 station이 서비스되는 방법 정의
*/
API_EXPORT BOOL uvMDR_SetInlineStationType(CHAR station, UINT8 type)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "IST %c %u", station, type);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_ist, szCmd);
}
API_EXPORT BOOL uvMDR_GetInlineStationType(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "IST %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_ist, szCmd);
}

/*
 desc : Station에서 PUT과 GET 명령어 동작 중에 일시적인 지연 값 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  지연 시간 값 (단위: msec)
 retn : TRUE or FALSE
 note : pick operation 중 스테이션에서 웨이퍼를 들어 올리기 전에 로봇이 진공이 쌓일 때까지 대기하고
		배치 작업을 위해 밸브를 닫은 후 지연하는 지연 (milli-seconds) 시간
*/
API_EXPORT BOOL uvMDR_SetPutGetDelayTime(CHAR station, UINT32 value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "PGD %c %u", station, value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_pgd, szCmd);
}
API_EXPORT BOOL uvMDR_GetPutGetDelayTime(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "PGD %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_pgd, szCmd, TRUE);
}

/*
 desc : GetOption 및 PutOption은 웨이퍼를 감지/해제 할 수 없을 때, GET 및 PUT 명령의 동작 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		opt_get	- [in]  8 bit (BYTE) 제어 값 (0 ~ 63)
		opt_put	- [in]  8 bit (BYTE) 제어 값 (0 ~ 63)
 retn : TRUE or FALSE
 note : GetOption 및 PutOption은 웨이퍼를 감지/해제 할 수 없을 때,
		GET 및 PUT 명령에 대해 다음 동작 정의 (5 개 bit 이외에는 0 값으로 초기화 해야 됨)
		기본은 15 (또는 바이너리로 00001111) 값으로 설정되어 있음
		-. 옵션이 활성화되어 있다면 (비트 0),
		-. GET/PUT이 실패하면 진공이 ON 상태로 유지되고 (비트 1),
		-. Arm은 실패한 GET/PUT에서 스트로크 다운 됨 (비트 2)
		-. 실패한 GET/PUT (비트 3)에서 arm이 후퇴 함
*/
API_EXPORT BOOL uvMDR_SetPutGetFailureOption(CHAR station, UINT8 opt_get, UINT8 opt_put)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "PGF %c %u %u", station, opt_get, opt_put);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_pgf, szCmd);
}
API_EXPORT BOOL uvMDR_GetPutGetFailureOption(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "PGF %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_pgf, szCmd, TRUE);
}

/*
 desc : Get 및 Put Option 설정 / 반환
 parm : option	- [in]  Option Mode 값 (0 ~ 4)
 retn : TRUE or FALSE
 note : option 값 설명
		0:	웨이퍼의 유무를 확인하지 않고 GET/PUT을 합니다.
		1:	Robot Controller는 진공 센서 (또는 에지 그리퍼 센서 또는 Bernoulli 정전 용량 센서)를
			확인하여 End Effector에 Wafer가 있는지 확인 합니다.
			Wafer가 없는 경우 Robot은 다음 Slot으로 이동하여 Wafer를 찾거나 Station Slot이
			소진 될 때까지 작업을 반복 합니다.
		3:	Robot Controller는 진공 센서 (엣지 그리퍼 센서 또는 Bernoulli 정전 용량 센서)를 확인하여
			End Effector에 Wafer가 있는지 확인 합니다. Wafer가 없으면 로봇이 후퇴하고 멈춥니다.
		2,4: Reserved
*/
API_EXPORT BOOL uvMDR_SetPutGetOption(UINT8 option)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "PGO %u", option);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_pgo, szCmd);
}
API_EXPORT BOOL uvMDR_GetPutGetOption()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_pgo, "PGO", TRUE);
}

/*
 desc : Station의 Pitch Parameter 값 설정
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		pitch	- [in]  value (Pitch 값. 단위: um)
 retn : TRUE or FALSE
 note : 일반적인 설정 값은 다음과 같음
		150mm wafer의 경우 1875 (3/16”)
		200mm wafer의 경우 2500 (1/4”)
		300mm wafer의 경우 3937 (10mm)
*/
API_EXPORT BOOL uvMDR_SetStationPitch(CHAR station, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "PIT %c %d", station, g_pMDRThread->GetPosToInch(ENG_RUTI::uti_0000, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_pit, szCmd);
}
API_EXPORT BOOL uvMDR_GetStationPitch(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "PIT %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_pit, szCmd, TRUE);
}

/*
 desc : Station에서 GET Operation을 수행할 때, Pushing Distance 값 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Pushing Distance 값 (단위: um)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetStationPushingDist(CHAR station, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "PSD %c %d", station, GetAxisPosDev(ENG_RANC::rac_z, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_psd, szCmd);
}
API_EXPORT BOOL uvMDR_GetStationPushingDist(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "PSD %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_psd, szCmd, TRUE);
}

/*
 desc : Station에서 GET Operation을 수행할 때, Pushing Type 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  0: No pushing is defined, 1: pushing is defined
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetStationPushingType(CHAR station, UINT8 value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "PSH %c %u", station, value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_psh, szCmd);
}
API_EXPORT BOOL uvMDR_GetStationPushingType(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "PSH %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_psh, szCmd, TRUE);
}

/*
 desc : Station에서 GET Operation을 수행할 때, Pushing이 시작되는 Offset 설정 / 반환
		즉, Start Pushing Offset 설정 / 반환 (R Axis)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Start Pushing Offset (단위: um)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetStartPushingOffsetR(CHAR station, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "PSS %c %d", station, GetAxisPosDev(ENG_RANC::rac_r1, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_pss, szCmd);
}
API_EXPORT BOOL uvMDR_GetStartPushingOffsetR(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "PSS %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_pss, szCmd, TRUE);
}

/*
 desc : Station에서 GET Operation을 수행할 때, Pushing이 시작되는 Offset 설정 / 반환
		즉, Start Pushing Offset 설정 / 반환 (Z Axis)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Start Pushing Offset (단위: um)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetStartPushingOffsetZ(CHAR station, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "PSZ %c %d", station, GetAxisPosDev(ENG_RANC::rac_z, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_psz, szCmd);
}
API_EXPORT BOOL uvMDR_GetStartPushingOffsetZ(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "PSZ %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_psz, szCmd, TRUE);
}

/*
 desc : Station에서 GET / PUT Operation을 수행할 때, Veritical Stroke-UP or Stroke-DOWN 가속도 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Pick-Up/Down 가속도 (단위: um/sec^2)
 retn : TRUE or FALSE
 note : Z 축에 해당됨
*/
API_EXPORT BOOL uvMDR_SetStationPickUpAccelerationZ(CHAR station, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "PUA %c %d", station, GetAxisPosDev(ENG_RANC::rac_z, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_pua, szCmd);
}
API_EXPORT BOOL uvMDR_GetStationPickUpAccelerationZ(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "PUA %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_pua, szCmd, TRUE);
}

/*
 desc : Station에서 GET / PUT Operation을 수행할 때, Veritical Stroke-UP or Stroke-DOWN 속도 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Pick-Up/Down 속도 (단위: um/sec)
 retn : TRUE or FALSE
 note : Z 축에 해당됨
*/
API_EXPORT BOOL uvMDR_SetStationPickUpSpeedZ(CHAR station, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "PUS %c %d", station, GetAxisPosDev(ENG_RANC::rac_z, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_pus, szCmd);
}
API_EXPORT BOOL uvMDR_GetStationPickUpSpeedZ(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "PUS %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_pus, szCmd, TRUE);
}

/*
 desc : Station에서 GET / PUT Operation을 수행할 때, Retracted Position 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Retracted Position (단위: um)
 retn : TRUE or FALSE
 note : R 축에 해당됨
*/
API_EXPORT BOOL uvMDR_SetRetractedStationPositionR(CHAR station, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "RPO %c %d", station, GetAxisPosDev(ENG_RANC::rac_r1, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_rpo, szCmd);
}
API_EXPORT BOOL uvMDR_GetRetractedStationPositionR(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "RPO %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_rpo, szCmd, TRUE);
}

/*
 desc : Station의 Position 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		value	- [in]  Station Position (단위: um)
 retn : TRUE or FALSE
 note : R 축에 해당됨
*/
API_EXPORT BOOL uvMDR_SetStationPositionName(CHAR station, ENG_RANC axis, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL}, szAxis[8] = {NULL};

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);
	sprintf_s(szCmd, 32, "SPO %c %s %d", station, szAxis, GetAxisPosDev(axis, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_spo, szCmd);
}
API_EXPORT BOOL uvMDR_GetStationPositionName(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "SPO %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_spo, szCmd, TRUE);
}

/*
 desc : Station의 Position 반환
 parm : station	- [in]  Station Number (1 ~ 130)
 retn : TRUE or FALSE
 note : R 축에 해당됨
*/
API_EXPORT BOOL uvMDR_GetStationPositionNumber(UINT8 station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "SPON %u", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_spon, szCmd, TRUE);
}

/*
 desc : Station의 Z 축을 위한 Stroke 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Z Axis Stroke Position (단위: um)
 retn : TRUE or FALSE
 note : Z 축에 해당됨
*/
API_EXPORT BOOL uvMDR_SetStationStrokeZ(CHAR station, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "STR %c %d", station, GetAxisPosDev(ENG_RANC::rac_z, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_str, szCmd);
}
API_EXPORT BOOL uvMDR_GetStationStrokeZ(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "STR %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_str, szCmd, TRUE);
}

/*
 desc : 둘 이상의 arm이 궤적을 그리며 이동 할 때, 겹쳐지는 정도 값 설정 (값이 작을수록 성능 UP?)
 parm : value	- [in]  0 ~ 95 (unit: %)
 retn : TRUE or FALSE
 note : TMD 명령에 의해 궤적 모드가 활성화 되면, GET 명령 중 안전할 때, 
		예를 들어, Theta 축이 여전히 움직이는 동안 arm이 스테이션을 향해 확장되기 시작할 때,
		둘 이상의 축과 관련된 복잡한 동작이 겹쳐짐
		이 기능은 pick과 place  operation의 성능을 높일 수 있음
		권장 값은 최대 30까지 임.
		추가 구성 파라미터는 향후 펌웨어 릴리스에서 Theta 축 회전을 제한하기 위해 예약 됨
*/
API_EXPORT BOOL uvMDR_SetStationAxisOverlapFactor(UINT8 value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "TBL %d", value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_tbl, szCmd);
}
API_EXPORT BOOL uvMDR_GetStationAxisOverlapFactor()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_tbl, "TBL", TRUE);
}

/*
 desc : Start teaching a station 즉, 티칭 세션을 시작하고 선택한 스테이션의 티칭 모드로 진입
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		slots	- [in]  최대 Slot의 개수
 retn : TRUE or FALSE
 note : 최대 슬롯 수를 지정하지 않으면 기본값은 1.
		Status Word의 bit 11 (0x0800)은 Teach Station Position Mode 중에 설정되며,
		EOT 명령으로 지워진다. (Cleared)
		스테이션이 이미 존재하는 경우 Slots 매개변수를 생략 가능.
		스테이션이 존재하지 않고 Slots 매개 변수가 생략되면 1 슬롯으로 설정
*/
API_EXPORT BOOL uvMDR_StartTeachingStation(CHAR station, UINT8 slots)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "TCH %c %u", station, slots);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_tch, szCmd);
}

/*
 desc : Scanning 혹은 Station Parameters의 Teaching 종료 처리 (End of teaching a station)
 parm : None
 retn : TRUE or FALSE
 note : 모든 Teaching 혹은 Station Parameters 값을 설정한 경우, 이 명령 호출
*/
API_EXPORT BOOL uvMDR_SetTeachingEnd()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_eot, "EOT");
}

/*
 desc : Station으로 이동할 때, R 축의 궤적 모드를 사용할지 여부 설정 / 반환
 parm : option	- [in]  0: Disabled, 1: Enabled
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetStationTrajectoryMode(UINT8 option)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "TMD %u", option);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_tmd, szCmd);
}
API_EXPORT BOOL uvMDR_GetStationTrajectoryMode()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_tmd, "TMD", TRUE);
}

/*
 desc : Station으로 이동할 때, Wafer (Vacuum) 확인 여부 설정 / 반환
 parm : option	- [in]  0: Disabled, 1: Enabled
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetStationVacuumCheck(UINT8 option)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "VCHK %u", option);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_vchk, szCmd);
}
API_EXPORT BOOL uvMDR_GetStationVacuumCheck()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_vchk, "VCHK", TRUE);
}

/*
 desc : Station에 대한 지공 검색 시간 초과 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Search Time-out (단위: milli-seconds)
 retn : TRUE or FALSE
 note : 이 시간 제한은 Pick and Place Operation 전에 웨이퍼 존재를 확인하는데 사용
*/
API_EXPORT BOOL uvMDR_SetStationVacuumSearchTimeout(CHAR station, UINT32 value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "VTO %c %u", station, value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_vto, szCmd);
}
API_EXPORT BOOL uvMDR_GetStationVacuumSearchTimeout(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "VTO %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_vto, szCmd, TRUE);
}


/* --------------------------------------------------------------------------------------------- */
/*                        Command Reference  (10.Station motion commands)                        */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : EndOpt 값으로, Station 이름으로부터 웨이퍼 하나 가져오기
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		slot	- [in]  Slot Number (1 ~ 25?)
		end_opt	- [in]  0: default
						1: Station 내부에서 stroke-up motion 후 GETN 명령이 실행을 완료하도록 하고,
						   Station의 후퇴 위치로 최종 모션을 건너 뜁니다.
						   이 옵션을 사용하면 Station간에 Wafer를 운반 할 때 성능이 향상 됩니다.
 retn : TRUE or FALSE
 note : GET Station Slot EndOpt	: gets a wafer from the specified station and slot, with specified EndOption.
		GET Station Slot		: gets a wafer from the specified station and slot.
		GET Station				: gets a wafer from the current slot of the specified station.
*/
API_EXPORT BOOL uvMDR_SetGetWaferFromStationName(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "GET %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_get, szCmd);
}
API_EXPORT BOOL uvMDR_SetGetWaferFromStationNameSlot(CHAR station, UINT8 slot)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "GET %c %u", station, slot);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_get, szCmd);
}
API_EXPORT BOOL uvMDR_SetGetWaferFromStationNameSlotOpt(CHAR station, UINT8 slot, UINT8 end_opt)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "GET %c %u %u", station, slot, end_opt);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_get, szCmd);
}

/*
 desc : Station 이름으로부터 웨이퍼 하나 가져오기 - for Dual Arm
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		arm_no	- [in]  Dual Arm의 번호 0 (primary), 1 (secondary arm) or 2 (both arms)
		slot	- [in]  Slot Number (1 ~ 25?)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetGetWaferFromStationNameSlotDual(CHAR station, UINT8 arm_no, UINT8 slot)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "DGET %c %u %u", station, arm_no, slot);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_dget, szCmd);
}

/*
 desc : EndOpt 값으로, Station 이름으로부터 웨이퍼 하나 가져오기
 parm : station	- [in]  Station Number (1 ~ 130)
		slot	- [in]  Slot Number (1 ~ 25?)
		end_opt	- [in]  0: default
						1: Station 내부에서 stroke-up motion 후 GETN 명령이 실행을 완료하도록 하고,
						   Station의 후퇴 위치로 최종 모션을 건너 뜁니다.
						   이 옵션을 사용하면 Station간에 Wafer를 운반 할 때 성능이 향상 됩니다.
 retn : TRUE or FALSE
 note : GET Station Slot EndOpt	: gets a wafer from the specified station and slot, with specified EndOption.
		GET Station Slot		: gets a wafer from the specified station and slot.
		GET Station				: gets a wafer from the current slot of the specified station.
*/
API_EXPORT BOOL uvMDR_SetGetWaferFromStationNum(UINT8 station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "GETN %u", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_getn, szCmd);
}
API_EXPORT BOOL uvMDR_SetGetWaferFromStationNumSlot(UINT8 station, UINT8 slot)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "GET %u %u", station, slot);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_getn, szCmd);
}
API_EXPORT BOOL uvMDR_SetGetWaferFromStationNumSlotOpt(UINT8 station, UINT8 slot, UINT8 end_opt)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "GET %u %u %u", station, slot, end_opt);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_getn, szCmd);
}

/*
 desc : Station으로 부터 Arm을 빼내는 동작 설정
 parm : z_offset- [in]  Z 축 Up or Down 거리 (단위: um)
 retn : TRUE or FALSE
 note : 이 명령은 robot arm을 station에서 빼내는 motion, linear retract motion (for radial station)
		또는 in-line motion (for in-line station)을 시작 합니다. arm은 반경 방향으로 또는 two-line
		궤적을 통해 station의 retracted position으로 이동 합니다. 오프셋이 지정되면 Z 축이
		arm 후진 동작 전에 지정된 거리만큼 상대적으로 이동
*/
API_EXPORT BOOL uvMDR_SetMoveOutStationArmOffsetZ(DOUBLE z_offset)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "MOS %d", GetAxisPosDev(ENG_RANC::rac_z, z_offset));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_mos, szCmd);
}
API_EXPORT BOOL uvMDR_SetMoveOutStation()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_mos, "MOS", TRUE);
}

/*
 desc : Station에서 Station으로 부터 Wafer 옮기는 동작 실행
 parm : station_s	- [in]  Source Station (By Name)
		slot_s		- [in]  Source Slot Number
		station_t	- [in]  Target Station (By Name)
		slot_t		- [in]  Target Slot Number
		r_offset	- [in]  'PUT' 동작일 때, 사용되는 ARM R 축 Offset (단위: um)
 retn : TRUE or FALSE
 note : 이 명령은 연결된 GET 및 PUT 명령과 유사하게 one station에서 another station으로 Wafer 이동 시작
*/
API_EXPORT BOOL uvMDR_SetMoveStationToStationByNameROffset(CHAR station_s, UINT8 slot_s,
														   CHAR station_t, UINT8 slot_t,
														   DOUBLE r_offset)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "MSS %c %u %c %u %d", station_s, slot_s, station_t, slot_t,
											   GetAxisPosDev(ENG_RANC::rac_z, r_offset));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_mss, szCmd);
}
API_EXPORT BOOL uvMDR_SetMoveStationToStationByName(CHAR station_s, UINT8 slot_s,
													CHAR station_t, UINT8 slot_t)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "MSS %c %u %c %u", station_s, slot_s, station_t, slot_t);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_mss, szCmd);
}

/*
 desc : Station에서 Station으로 부터 Wafer 옮기는 동작 실행
 parm : station_s	- [in]  Source Station (By Number) (1 ~ 130)
		slot_s		- [in]  Source Slot Number
		station_t	- [in]  Target Station (By Number) (1 ~ 130)
		slot_t		- [in]  Target Slot Number
		r_offset	- [in]  'PUT' 동작일 때, 사용되는 ARM R 축 Offset (단위: um)
 retn : TRUE or FALSE
 note : 이 명령은 연결된 GET 및 PUT 명령과 유사하게 one station에서 another station으로 Wafer 이동 시작
*/
API_EXPORT BOOL uvMDR_SetMoveStationToStationByNumberROffset(UINT8 station_s, UINT8 slot_s,
															 UINT8 station_t, UINT8 slot_t,
															 DOUBLE r_offset)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "MSSN %u %u %u %u %d", station_s, slot_s, station_t, slot_t,
											   GetAxisPosDev(ENG_RANC::rac_z, r_offset));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_mssn, szCmd);
}
API_EXPORT BOOL uvMDR_SetMoveStationToStationByNumber(UINT8 station_s, UINT8 slot_s,
													  UINT8 station_t, UINT8 slot_t)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "MSSN %u %u %u %u", station_s, slot_s, station_t, slot_t);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_mssn, szCmd);
}

/*
 desc : Station 근접 (접근) 위치로 Arm 이동
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		slot	- [in]  Slot Number
		option	- [in]  veritical position (0, 1, 2 중 한 개 선택)
						0 - vertical position to leave the slot (SPO + OFS), default if not specified
						1 - vertical position to enter the slot (SPO + OFS - STR)
						2 - vertical position at slot position for current slot (SPO)
 retn : TRUE or FALSE
 note : 이 명령은 로봇을 지정된 스테이션의 접근 위치로 이동하기 시작합니다.
		인라인 스테이션의 경우 이 명령은 MTR과 동일하게 작동 합니다
*/
API_EXPORT BOOL uvMDR_SetMoveArmStationApproachPosition(CHAR station, UINT8 slot, UINT8 option)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "MTA %c %u %u", station, slot, option);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_mta, szCmd);
}

/*
 desc : Station 위치로 Arm 이동
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		slot	- [in]  Slot Number
		option	- [in]  veritical position (0, 1, 2 중 한 개 선택)
						0 - vertical position to leave the slot (SPO + OFS)
						1 - vertical position to enter the slot (SPO + OFS - STR)
						2 - vertical position at slot position for current slot (SPO), default if not specified
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetMoveArmStationPosition(CHAR station, UINT8 slot, UINT8 option)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "MTS %c %u %u", station, slot, option);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_mts, szCmd);
}

/*
 desc : Station 내에 Slot 개수 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		slot	- [in]  최대 Slot 개수 (1 ~ 25?)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetStationSlotNumber(CHAR station, UINT8 slot)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "NSL %c %u", station, slot);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_nsl, szCmd);
}
API_EXPORT BOOL uvMDR_GetStationSlotNumber(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "NSL %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_nsl, szCmd, TRUE);
}

/*
 desc : 총 등록된 Station 개수 반환
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_GetStationNumber()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_nst, "NST", TRUE);
}

/*
 desc : Station Offset 설정 / 반환 (for Z Axis)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Z Axis Offset (단위: um)
 retn : TRUE or FALSE
 note : Station에서 GET 명령이 끝나는 곳 혹은 PUT 명령이 시작되는 곳의 Z 축 Offset 값 임
*/
API_EXPORT BOOL uvMDR_SetStationOffsetZ(CHAR station, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "OFS %c %d", station, GetAxisPosDev(ENG_RANC::rac_z, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_ofs, szCmd);
}
API_EXPORT BOOL uvMDR_GetStationOffsetZ(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "OFS %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_ofs, szCmd, TRUE);
}

/*
 desc : Station 내부에 Wafer 넣기 - For Single Arm
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		slot	- [in]  slot number (1 ~ 25?)
		offset	- [in]  R Axis Offset (단위: um)
		end_opt	- [in]  0: default
						1: station의 후퇴 위치로 최종 모션을 건너 뛰고 station에
						   stroke-down motion이 들어간 후, PUT 명령 실행이 완료
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_PutWaferIntoStationName(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "PUT %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_put, szCmd);
}
API_EXPORT BOOL uvMDR_PutWaferIntoStationNameFull(CHAR station, UINT8 slot,
												  DOUBLE offset, UINT8 end_opt)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "PUT %c %u %d %u", station, slot,
											GetAxisPosDev(ENG_RANC::rac_r1, offset),
											end_opt);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_put, szCmd);
}

/*
 desc : Station 내부에 Wafer 넣기 - For Single Arm
 parm : station	- [in]  Station Number (1 ~ 130)
						  1 ~  26 : set_no_1,
						 27 ~  52 : set_no_2,
						 53 ~  78 : set_no_3,
						 79 ~ 104 : set_no_4,
						105 ~ 130 : set_no_5
 		slot	- [in]  slot number (1 ~ 25?)
		offset	- [in]  R Axis Offset (단위: um)
		end_opt	- [in]  0: default
						1: station의 후퇴 위치로 최종 모션을 건너 뛰고 station에
						   stroke-down motion이 들어간 후, PUT 명령 실행이 완료
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_PutWaferIntoStationNo(UINT8 station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "PUTN %u", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_putn, szCmd);
}
API_EXPORT BOOL uvMDR_PutWaferIntoStationNoFull(UINT8 station, UINT8 slot,
												DOUBLE offset, UINT8 end_opt)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "PUTN %u %u %d %u", station, slot,
											GetAxisPosDev(ENG_RANC::rac_r1, offset),
											end_opt);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_putn, szCmd);
}

/*
 desc : Station 내부에 Wafer 넣기 - For Duam Arm
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		arm_no	- [in]  0 (primary arm), 1 (secondary arm) or 2 (both arms)
		slot	- [in]  slot number (1 ~ 25?)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_PutWaferIntoStationNameDual(CHAR station, UINT8 arm_no, UINT8 slot)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "DPUT %c %u %u", station, arm_no, slot);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_dput, szCmd);
}

/*
 desc : 현재 Station이 서비스 상태 값 요청
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetStationServiceBusy()
{
	g_pstShMemMDR->station_service_busy	= 0xff;	/* 일단 쓰레기 값으로 초기화 */
	return g_pMDRThread->SendData(ENG_RSCC::rsc_sbusy, "SBUSY", TRUE);
}

/*
 desc : Station에서 Station으로 부터 Wafer 옮기는 동작 실행
 parm : station_s	- [in]  Source Station (By Name)
		slot_s		- [in]  Source Slot Number
		station_t	- [in]  Target Station (By Name)
		slot_t		- [in]  Target Slot Number
		r_offset	- [in]  'PUT' 동작일 때, 사용되는 ARM R 축 Offset (단위: um)
		end_opt		- [in]  0 (default) or 1
 retn : TRUE or FALSE
 note : 이 명령은 chanined GET 및 PUT 명령과 유사하게 한 스테이션에서 다른 스테이션으로 웨이퍼를
		이동하기 시작하며, 배치 작업 중 선택적 arm 오프셋 및 종료 옵션 사용
*/
API_EXPORT BOOL uvMDR_SetMoveStationToStationEndOpt(CHAR station_s, UINT8 slot_s,
													CHAR station_t, UINT8 slot_t,
													DOUBLE r_offset, UINT8 end_opt)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "SGP %c %u %c %u %d %u", station_s, slot_s, station_t, slot_t,
			  GetAxisPosDev(ENG_RANC::rac_z, r_offset), end_opt);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_sgp, szCmd);
}

/*
 desc : Station의 동작 상태를 Lock 할지 여부 설정 / 반환
 parm : station	- [in]  Source Station (By Name) or '#' : All station
		option	- [in]  0: Unlocked, 1: Locked
 retn : TRUE or FALSE
 note : 이 명령은 스테이션에 대한 "잠금" 모션 명령 허용
		스테이션 잠금 상태의 영향을 받는 명령 목록은 SBUSY 참조
		스테이션이 "잠김"상태에 있으면 이 스테이션에 영향을 주는 모든 동작 명령이 비활성화 되고
		오류 반환. 스테이션이 잠기면 잠금 해제 옵션이 있는 SLCK 명령만 잠금을 해제 가능
		특수 스테이션 이름 '#'은 정의 된 모든 스테이션에 영향 줌
*/
API_EXPORT BOOL uvMDR_SetStationLockedState(CHAR station, UINT8 option)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "SLCK %c %u", station, option);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_slck, szCmd);
}
API_EXPORT BOOL uvMDR_GetStationLockedState(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "SLCK %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_slck, szCmd, TRUE);
}

/* --------------------------------------------------------------------------------------------- */
/*            Command Reference  (11.Station scanning parameters and motion commands)            */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 최대 웨이퍼 두께 설정 / 반환
 parm : value	- [in]  설정하고자 하는 웨이퍼 두께 (단위: um)
 retn : TRUE or FALSE
 note : 내부적으로 설정된 기본 설정 값은 25 (1 = 0.001 inch; 25 = 0.025 inch)
*/
API_EXPORT BOOL uvMDR_SetMaximumWaferThick(DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_MT %d", GetAxisPosDev(ENG_RANC::rac_z, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc__mt, szCmd);
}
API_EXPORT BOOL uvMDR_GetMaximumWaferThick()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__mt, "_MT", TRUE);
}

/*
 desc : Wafer 검사할 때, Z 축 가속도 값 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  설정하고자 Scanning 가속도 (단위: um / sec^2)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetWaferScanZAxisAcceleration(CHAR station, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_SA %c %d", station, GetAxisPosDev(ENG_RANC::rac_z, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc__sa, szCmd);
}
API_EXPORT BOOL uvMDR_GetWaferScanZAxisAcceleration(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_SA %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__sa, szCmd, TRUE);
}

/*
 desc : Station에 대해 현재 위치 (좌표가 아님)를 Scanning 중심 좌표계로 설정 (반환 없음)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetCurrentPositionScanningCenterCoordinate(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_SC %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__sc, szCmd);
}

/*
 desc : Station에 대해 현재 위치 (좌표가 아님)를 Scanning 왼쪽/오른쪽 좌표계로 설정 (반환 없음)
 parm : direct	- [in]  0x00: Left, 0x01: Right
		station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetCurrentPositionScanningCoordinate(ENG_MALR direct, CHAR station)
{
	CHAR szCmd[32]		= {NULL};
	ENG_RSCC enCmd[2]	= { ENG_RSCC::rsc__sl, ENG_RSCC::rsc__sr };
	if (direct == ENG_MALR::alr_left)	sprintf_s(szCmd, 32, "_SL %c", station);
	else								sprintf_s(szCmd, 32, "_SR %c", station);
	return g_pMDRThread->SendData(enCmd[UINT8(direct)], szCmd);
}

/*
 desc : Station 마다 Z (Vertical) 축 Scanning Speed
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  설정하고자 Scanning 속도 (단위: um / sec^2)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetWaferScanZAxisSpeed(CHAR station, DOUBLE value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_SS %c %d", station, GetAxisPosDev(ENG_RANC::rac_z, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc__ss, szCmd);
}
API_EXPORT BOOL uvMDR_GetWaferScanZAxisSpeed(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_SA %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__ss, szCmd, TRUE);
}

/*
 desc : Scanner Sensor Type (Scanner Sensing Type) 설정 / 반환
 parm : value	- [in]  0 for reflective type of scanner, 1 for thru-beam type of scanner
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetScannerSensorType(UINT8 value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_ST %u", value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc__st, szCmd);
}
API_EXPORT BOOL uvMDR_GetScannerSensorType()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__st, "_ST", TRUE);
}

/*
 desc : Scanned Positions 반환
 parm : direction	- [in]  0 : means results from scanning up, (bottom to top),
							1 : means results from scanning down (top to bottom) will be printed
 retn : TRUE or FALSE
 note : 방향에서 마지막 스캔 프로세스의 레이저 스캐너가 켜짐에서 꺼짐으로 변경 사항을 보고한 모든 Z 좌표를 출력 합니다
*/
API_EXPORT BOOL uvMDR_GetScannedPositions(UINT8 direction)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "DSCN %u", direction);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_dscn, szCmd, TRUE);
}

/*
 desc : Wafer Mapping 작업 진행 이후 결과 반환
 parm : direction	- [in]  0 : means results from scanning up, (bottom to top),
							1 : means results from scanning down (top to bottom) will be printed
 retn : TRUE or FALSE
 note : Station (or Cassette)의 각 Slot 마다 놓여진 Wafer의 존재 확인 (에러인 경우, 에러 상태 정보 반환)
 note : 0	No wafer present in this slot
		1	Single wafer is present, no other cross-slotted
		2	Two wafers found in single slot
		4	Single Cross-slotted wafer
		5	Single wafer with cross-slotted wafer on next slot
		6	Double-slotted, or cross-slotted wafer with another wafer
*/
API_EXPORT BOOL uvMDR_GetResultLastWaferScanning(UINT8 direction)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "MAP %u", direction);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_map, szCmd, TRUE);
}

/*
 desc : 전역 Beam Threshold 최대값 설정 / 반환 (Upper / Bottom 차이)
 parm : value_l	- [in]  Bottom Threadhold (단위: um)
		value_h	- [in]  Uppoer Threadhold (단위: um)
 retn : TRUE or FALSE
 note : 이 명령은 cassette의 수직 위치 차이를 설명하는 추가 station별 파라미터를 정의 합니다.
		이러한 파라미터는 동일한 wafer set의 스캔 위치 차이를 설명하지만 서로 다른 cassette에
		배치되거나 Pocket 안에 단단히 고정되지 않습니다.
		이러한 매개 변수가 0이면, _BT에 정의된 parameter가 사용 됩니다.
*/
API_EXPORT BOOL uvMDR_SetBeamThreadhold(DOUBLE value_l, DOUBLE value_h)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "_BT %d %d",
			  g_pMDRThread->GetPosToInch(ENG_RUTI::uti_0000, value_l),
			  g_pMDRThread->GetPosToInch(ENG_RUTI::uti_0000, value_h));
	return g_pMDRThread->SendData(ENG_RSCC::rsc__bt, szCmd);
}
API_EXPORT BOOL uvMDR_GetBeamThreadhold()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc__bt, "_BT", TRUE);
}

/*
 desc : Station 별로 Beam Threshold 최대값 설정 / 반환 (Upper / Bottom 차이)
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value_l	- [in]  Bottom Threadhold (단위: um)
		value_h	- [in]  Uppoer Threadhold (단위: um)
 retn : TRUE or FALSE
 note : 이 명령은 cassette의 수직 위치 차이를 설명하는 추가 station별 파라미터를 정의 합니다.
		이러한 파라미터는 동일한 wafer set의 스캔 위치 차이를 설명하지만 서로 다른 cassette에
		배치되거나 Pocket 안에 단단히 고정되지 않습니다.
		이러한 매개 변수가 0이면, _BT에 정의된 parameter가 사용 됩니다.
*/
API_EXPORT BOOL uvMDR_SetPerStationBeamThreadhold(CHAR station, DOUBLE value_l, DOUBLE value_h)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "SBT %c %d %d", station,
			  g_pMDRThread->GetPosToInch(ENG_RUTI::uti_0000, value_l),
			  g_pMDRThread->GetPosToInch(ENG_RUTI::uti_0000, value_h));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_sbt, szCmd);
}
API_EXPORT BOOL uvMDR_GetPerStationBeamThreadhold(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "SBT %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_sbt, szCmd, TRUE);
}

/*
 desc : Station을 Scanning 작업 수행
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		direct	- [in]  0: 아래에서 위로, 1: 위에서 아래로, 2: 양방향으로 (동시에?)
		angle	- [in]  0: 스캔이 중앙 위치, 1: 스캔이 왼쪽 위치, 2: 스캔이 오른쪽 위치
 retn : TRUE or FALSE
 note : Direction – 옵션 값 : 0, 1 또는 2.
			0: 강제로 위로 (아래에서 위로)
			1: 강제로 아래로 (위에서 아래로)
			2: 기본값. 양방향으로 위와 아래로 강제로 스캔.
			반사 형 스캐너 (_ST 매개 변수가 0으로 설정 됨)를 사용하면,
				scan motion up은 왼쪽 스캔 좌표를 사용하고, scan motion down은 오른쪽 스캔 좌표를 사용 합니다.
			투과형 스캐너 (_ST 매개 변수가 1로 설정 됨)를 사용하면,
				두 스캔 동작 모두 중심 스캔 좌표를 사용 합니다.
		Angle – 옵션 값 : 0, 1 또는 2.
			0: 스캔이 중앙 위치 (_SC / SCP 명령으로 정의 된 하나) 사용
			1: 스캔이 왼쪽 위치 (_SL / SLP로 정의 된 하나) 사용
			2: 스캔이 오른쪽 위치 (_SR / SRP로 정의 됨) 사용
		기본값 (아무것도 지정되지 않은 경우)은 지정된 스캐너 유형 (_ST) 및 요청 방향에 따라 다릅니다.
 note : SCN station direction angle: 지정된 방향 및 지정된 위치에서 지정된 스테이션에 대한
		단일 패스 스캔 프로세스를 시작 합니다. 투과형 레이저 스캐너가 _ST 명령으로 구성된 경우
		각도는 0 (중앙 스캔 위치)과 같아야 합니다.
 note : 스캔 결과 값은 "MAP" 명령어 사용해서 얻을 수 있음
 주의 : arm (R 축)이 요청된 station의 후퇴 위치로 이동 한 상태에서 단일 패스 스캔이 완료 됩니다.
		Two-pass 스캐닝을 요청하면 왼쪽 스캐닝 위치에서 arm (R 축)을 사용하여 T 축이 좌우 스캐닝
		위치 사이를 이동 합니다. 수직 동작을 시작하기 전 또는 완료된 후에 웨이퍼가 감지되면 스캐닝이
		중단되고 이전 오류 이유로 bit 5 (0x0020)가 설정 됩니다.
*/
API_EXPORT BOOL uvMDR_StartStationWaferScanning(CHAR station, UINT8 direct, UINT8 angle)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "SCN %c %u %u", station, direct, angle);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_scn, szCmd);
}

/*
 desc : Station의 Scanning Center 좌표 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		value	- [in]  좌표 혹은 각도 값 (단위: um or degree)
 retn : TRUE or FALSE
 note : 이 명령은 주어진 축 좌표를 Station 매개 변수에 의해 주어진 station에 대한 첫 번째
		wafer에 대한 축의 스캐닝 좌표로 설정
		이 위치는 투과형 스캐너 (기본값)로 스캔 할 때 사용 되거나 SCN 명령에 의해 특별히 요청
		Z (수직) 스캐닝 좌표 (지정된 경우)는 스캐너가 첫 번째 station slot에 배치 된 wafer의
		중간을 감지하는 위치여야 함
		좌표 설정은 스캔 티칭 모드 (TCS 명령으로 입력) 중에만 유효
 주의 : Z축 좌표가 정의되고 최저 및/또는 최고 Z 스캔 위치가 0인 경우,
		이 명령은 가장 낮은 및/또는 가장 높은 Z 스캔 위치(SZL 및 SZH 명령으로 정의됨)를
		station의 첫 번째 wafer 마지막 wafer의 절반 아래 및 위로 설정
		이 값은 SSP 명령에 의해 station 매개 변수 파일에 저장되며 RSP 명령에 의해 복원 될 수 있음
*/
API_EXPORT BOOL uvMDR_SetStationScanningCenterCoord(CHAR station, ENG_RANC axis, DOUBLE value)
{
	CHAR szCmd[32]		= {NULL}, szAxis[8] = {NULL};
	ENG_RSCC enCmd[4]	= { ENG_RSCC::rsc_scp_t, ENG_RSCC::rsc_scp_r1, ENG_RSCC::rsc_scp_r2, ENG_RSCC::rsc_scp_z };

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);
	sprintf_s(szCmd, 32, "SCP %c %s %d", station, szAxis, GetAxisPosDev(axis, value));
	return g_pMDRThread->SendData(enCmd[UINT8(axis)], szCmd);
}
API_EXPORT BOOL uvMDR_GetStationScanningCenterCoord(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "SCP %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_scp, szCmd, TRUE);
}

/*
 desc : Scan Debug Mode 출력 여부 (사용 여부)
 parm : option	- [in]  0: Disabled, 1: Enabled
 retn : TRUE or FALSE
 note : 이 명령은 스캔 작업에 대한 디버그 모드를 설정 합니다.
		디버그 모드가 설정되면 SCN 명령 중에 추가 정보가 보고 됩니다.
		여기에는 스캔 매개 변수와 wafer가 감지되는 위치가 포함 됩니다.
*/
API_EXPORT BOOL uvMDR_SetScanDebugMode(UINT8 option)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "SDBG %u", option);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_sdbg, szCmd);
}
API_EXPORT BOOL uvMDR_GetScanDebugMode()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_sdbg, "SDBG");
}

/*
 desc : Station의 Scanning Left 좌표 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		value	- [in]  좌표 혹은 각도 값 (단위: um or degree)
 retn : TRUE or FALSE
 note : 이 명령은 주어진 축 좌표를 Station 매개 변수에 의해 주어진 station에 대한 첫 번째
		wafer에 대한 축의 스캐닝 좌표로 설정
		이 위치는 투과형 스캐너 (기본값)로 스캔 할 때 사용 되거나 SCN 명령에 의해 특별히 요청
		Z (수직) 스캐닝 좌표 (지정된 경우)는 스캐너가 첫 번째 station slot에 배치 된 wafer의
		중간을 감지하는 위치여야 함
		좌표 설정은 스캔 티칭 모드 (TCS 명령으로 입력) 중에만 유효
 주의 : Z축 좌표가 정의되고 최저 및/또는 최고 Z 스캔 위치가 0인 경우,
		이 명령은 가장 낮은 및/또는 가장 높은 Z 스캔 위치(SZL 및 SZH 명령으로 정의됨)를
		station의 첫 번째 wafer 마지막 wafer의 절반 아래 및 위로 설정
		이 값은 SSP 명령에 의해 station 매개 변수 파일에 저장되며 RSP 명령에 의해 복원 될 수 있음
*/
API_EXPORT BOOL uvMDR_SetStationScanningLeftCoord(CHAR station, ENG_RANC axis, DOUBLE value)
{
	CHAR szCmd[32]		= {NULL}, szAxis[8] = {NULL};
	ENG_RSCC enCmd[4]	= { ENG_RSCC::rsc_slp_t, ENG_RSCC::rsc_slp_r1, ENG_RSCC::rsc_slp_r2, ENG_RSCC::rsc_slp_z };

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);
	sprintf_s(szCmd, 32, "SLP %c %s %d", station, szAxis, GetAxisPosDev(axis, value));
	return g_pMDRThread->SendData(enCmd[UINT8(axis)], szCmd);
}
API_EXPORT BOOL uvMDR_GetStationScanningLeftCoord(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "SLP %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_slp, szCmd, TRUE);
}

/*
 desc : Station의 Scanning Right 좌표 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
		value	- [in]  좌표 혹은 각도 값 (단위: um or degree)
 retn : TRUE or FALSE
 note : 이 명령은 주어진 축 좌표를 Station 매개 변수에 의해 주어진 station에 대한 첫 번째
		wafer에 대한 축의 스캐닝 좌표로 설정
		이 위치는 투과형 스캐너 (기본값)로 스캔 할 때 사용 되거나 SCN 명령에 의해 특별히 요청
		Z (수직) 스캐닝 좌표 (지정된 경우)는 스캐너가 첫 번째 station slot에 배치 된 wafer의
		중간을 감지하는 위치여야 함
		좌표 설정은 스캔 티칭 모드 (TCS 명령으로 입력) 중에만 유효
 주의 : Z축 좌표가 정의되고 최저 및/또는 최고 Z 스캔 위치가 0인 경우,
		이 명령은 가장 낮은 및/또는 가장 높은 Z 스캔 위치(SZL 및 SZH 명령으로 정의됨)를
		station의 첫 번째 wafer 마지막 wafer의 절반 아래 및 위로 설정
		이 값은 SSP 명령에 의해 station 매개 변수 파일에 저장되며 RSP 명령에 의해 복원 될 수 있음
*/
API_EXPORT BOOL uvMDR_SetStationScanningRightCoord(CHAR station, ENG_RANC axis, DOUBLE value)
{
	CHAR szCmd[32]		= {NULL}, szAxis[8] = {NULL};
	ENG_RSCC enCmd[4]	= { ENG_RSCC::rsc_srp_t, ENG_RSCC::rsc_srp_r1, ENG_RSCC::rsc_srp_r2, ENG_RSCC::rsc_srp_z };

	/* 축 코드에 대한 이름 반환 */
	GetAxisChar(axis, szAxis, 8);
	sprintf_s(szCmd, 32, "SRP %c %s %d", station, szAxis, GetAxisPosDev(axis, value));
	return g_pMDRThread->SendData(enCmd[UINT8(axis)], szCmd);
}
API_EXPORT BOOL uvMDR_GetStationScanningRightCoord(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "SRP %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_srp, szCmd, TRUE);
}

/*
 desc : Station 별로 웨이퍼 최대 두께 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Wafer Thickness (단위: um)
 retn : TRUE or FALSE
 note : 이 명령은 최대 웨이퍼 두께에 대한 스테이션 별로 값 지정
		웨이퍼의 상단 및 하단 가장자리에 대한 스캐닝 판독 값의 최대 예상 차이 지정
		이 매개 변수를 0으로 설정하면 _MT로 정의 된 매개 변수가 적용 됨
*/
API_EXPORT BOOL uvMDR_SetStationWaferMaxThickness(CHAR station, DOUBLE value)
{
	CHAR szCmd[32]		= {NULL};
	sprintf_s(szCmd, 32, "SMT %c %d", station, GetAxisPosDev(ENG_RANC::rac_z, value));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_smt, szCmd);
}
API_EXPORT BOOL uvMDR_GetStationWaferMaxThickness(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "SMT %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_smt, szCmd, TRUE);
}

/*
 desc : Station에서 Station으로 부터 Wafer 옮기는 동작 실행
 parm : station_s	- [in]  Source Station (By Name)
		slot_s		- [in]  Source Slot Number
		station_t	- [in]  Target Station (By Name)
		slot_t		- [in]  Target Slot Number
		r_offset	- [in]  'PUT' 동작일 때, 사용되는 ARM R 축 Offset (단위: um)
		end_opt		- [in]  0 (default) or 1
 retn : TRUE or FALSE
 note : 이 명령은 chanined GET 및 PUT 명령과 유사하게 한 스테이션에서 다른 스테이션으로 웨이퍼를
		이동하기 시작하며, 배치 작업 중 선택적 arm 오프셋 및 종료 옵션 사용
*/
API_EXPORT BOOL uvMDR_SetPlaceStationToStationEndOpt(CHAR station_s, UINT8 slot_s,
													 CHAR station_t, UINT8 slot_t,
													 DOUBLE r_offset, UINT8 end_opt)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "SPG %c %u %c %u %d %u", station_s, slot_s, station_t, slot_t,
			  GetAxisPosDev(ENG_RANC::rac_z, r_offset), end_opt);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_spg, szCmd);
}

/*
 desc : Station의 Scanning을 위한 Z 축의 가장 높은 위치의 좌표 값 설정 (위치 값 설정)
 parm : direct	- [in]  0x00:Highest, 0x01:Lowest
		station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		value	- [in]  Z Axis Position (가장 높은 위치 값) (단위: um)
 retn : TRUE or FALSE
 note : 이 명령은 스캔 중 Z축에 대해 가장 높은/낮은 좌표 (상향/하향 할 때 최종 위치, 내려가면 시작 위치) 설정
		이것은 스캔 동작이 완료(또는 시작)되고 캡처 프로세스가 꺼지는 위치(또는 켜짐)
		이 위치에서 웨이퍼 mapper는 웨이퍼를 탐지해야 함. 그렇지 않으면 검색 명령이 실패
		설정 명령은 검색 티칭 모드(TCS 명령과 함께 입력) 중에만 유효 하다.
*/
API_EXPORT BOOL uvMDR_SetStationHighLowestCoordZ(UINT8 direct, CHAR station, DOUBLE value)
{
	CHAR szCmd[32]		= {NULL}, szName[2][8] = { "SZH", "SZL" };
	ENG_RSCC enCmd[2]	= { ENG_RSCC::rsc_szh, ENG_RSCC::rsc_szl };
	sprintf_s(szCmd, 32, "%s %c %d", szName[direct], station, GetAxisPosDev(ENG_RANC::rac_z, value));
	return g_pMDRThread->SendData(enCmd[direct], szCmd);
}
API_EXPORT BOOL uvMDR_GetStationHighLowestCoordZ(UINT8 direct, CHAR station)
{
	CHAR szCmd[32]		= {NULL}, szName[2][8] = { "SZH", "SZL" };
	ENG_RSCC enCmd[2]	= { ENG_RSCC::rsc_szh, ENG_RSCC::rsc_szl };
	sprintf_s(szCmd, 32, "%s %c", szName[direct], station);
	return g_pMDRThread->SendData(enCmd[direct], szCmd, TRUE);
}

/*
 desc : Teach Scanning Parameters 시작
		이 명령은 wafer scanning parameter를 설정 하거나 변경할 수 있게 합니다.
 parm : None
 retn : TRUE or FALSE
 note : scanning 파라미터(_MT, _SC, _BT, _SC/SCP, _SL/SLP, _SR/SRP 및 SZL/SZH)를 설정하는 명령보다
		먼저 발행 (실행? issue)해야 한다. TCH 명령이 적용 동안(teaching station 좌표) 또는 다른
		TCS 명령이 적용 동안에는 발행 (실행)할 수 없다. status word의 bit 14(0x4000)는
		teach scan mode 동안 설정되며, EOT 명령으로 삭제 (Clear) 된다.
*/
API_EXPORT BOOL uvMDR_SetStartTeachScanningParam()
{
	return g_pMDRThread->SendData(ENG_RSCC::rsc_tcs, "TCS", TRUE);	/* 설정 성공이면, 응답 없음. 실패일 경우만, 에러 값 (문자열: "EOT" 수신)*/
}

/* --------------------------------------------------------------------------------------------- */
/*      Command Reference  (12.Align-on-Flight (on-The-Flight, OtF) parameters and commands)     */
/*         (OtF 기능을 사용하면 선택 및/또는 위치 이동 중에 웨이퍼 중심 변위 측정 가능)          */ 
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 현재 OtF (On-The-Flight) Calibration Set Number를 반환
		단, 이전 버전과 호환성을 위해 유지하고 있는 기능.
		설정 기능은 사용되지 않음 (무시됨)
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_GetOtFSetNumber()
{
	CHAR szCmd[32]	= {"ACS"};
	return g_pMDRThread->SendData(ENG_RSCC::rsc_acs, szCmd, TRUE);
}

/*
 desc : Execute On-the-Fly Centering Calibration Run
 parm : dist	- [in]  relative distance to move arm axis during calibration (unit: um)
 retn : TRUE or FALSE
 note : end-effector에 적절하게 배치된 참조 웨이퍼 (reference wafer)를 사용하여,
		robot을 Target station으로 향하게 하고, AOFC 명령은 wafer를 지정된 거리로 이동하고,
		wafer의 전면 및 후면 가장자리 모두에서 sensor 위치들을 고정 (latch)하고,
		sensor 위치를 계산 합니다. 이전에 정의된 wafer radius, end-effector length,
		calibration start position 및 calibration distance뿐만 아니라 미래의 참조 용으로 저장 됩니다.
		이 명령을 실행하기 전에 wafer 반경과 end effector 길이를 ASL 명령으로 지정해야 합니다.
		최상의 결과를 얻으려면 향후 측정 동작과 동일한 방향으로 calibration을 수행하는 것이 좋습니다.
		정상적인 상황 (정의 팔 좌표가있는 station)에서 거리는 GETC를 사용하는 경우 음수이고
		PUTC를 사용하는 경우 양수여야 합니다.
		교정 값은 현재 station parameter와 함께 저장되므로,
		이 명령을 실행하기 전에 기본 station을 올바르게 설정해야 합니다.
		값은 SMP 명령으로 motion parameter 파일에 저장되며 RMP 명령으로 복원 할 수 있습니다.
 예제 : >MTR P 1
		>ASL 5906 11500
		>AOFC 15000
		>AOFD -1
		wafer Radius=5906 EELength=11500 Distance=15000
		Calibration data:
		Left: X=-2.9629, Y= 15.6370
		Right: X= 3.3586, Y= 16.1380
		Start positions: 6691,-5000,1689
		>
*/
API_EXPORT BOOL uvMDR_SetOtFCentCalibRun(DOUBLE dist)
{
	CHAR szCmd[32]	= {NULL};

	/* 축 코드에 대한 이름 반환 */
	sprintf_s(szCmd, 32, "AOFC %d", GetAxisPosDev(ENG_RANC::rac_r1, dist));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_aofc, szCmd);
}

/*
 desc : Second Scan (2회 검사)이 필요한 경우, Align-on-Flight (OtF) 작업 명령 (GETC, PUTC 등)이
		Notch를 검색할지 여부 설정 / Notch 검색 여부 값 반환
 parm : option	- [in]  0x00: 2회 Scan 때, Notch 검색 않도록 설정
						0x01: 2회 Scan 때, Notch 검색 하도록 설정 (강제로 검색하도록 설정)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_SetOtFNotchSecondScan(UINT8 option)
{
	CHAR szCmd[32]	= {NULL};

	/* 축 코드에 대한 이름 반환 */
	sprintf_s(szCmd, 32, "AOFCN2 %u", option);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_aofcn2, szCmd);
}
API_EXPORT BOOL uvMDR_GetOtFNotchSecondScan()
{
	CHAR szCmd[32]	= {"AOFCN2"};
	return g_pMDRThread->SendData(ENG_RSCC::rsc_aofcn2, szCmd, TRUE);
}

/*
 desc : Execute On-the-Fly Calibration Run against a station
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		direct	- [in]  direction of arm motion (Arm이 보는 위치에 따라 값이 달라짐? 대체 무슨 뜻인지 모름?)
						0: Retracted Position or Station' Position
						1: Station' Position or Retracted Position
		dist	- [in]  relative distance to move arm axis during calibration (unit: um)
 retn : TRUE or FALSE
 note : Reference wafer가 end effector에 적절하게 놓여진 경우,
		AOFCS는 먼저 지정된 station, slot 1 후튀 위치(retracted position, Direction = 0),
		또는 station position(Direction = 1)에 Arm을 위치 시키고, wafer를 지정된 거리 또는
		후퇴 위치(Direction = 1), 또는 station position(Direction = 0)로 이동시킨다.
		wafer의 전면 및 후면 가장자리에서의 sensor 위치가 래치 (latch)되고,
		sensor 위치가 계산된다. 이러한 위치 뿐만 아니라 이전에 정의된 wafer 반경,
		end effector 길이, 캘리브레이션 시작 위치 및 캘리브레이션 거리도 향후 참조를를 위해 저장 됩니다.
		이 명령을 실행하기 전에 wafer 반경과 end effector 길이를 지정해야 합니다.
		최상의 결과를 얻으려면 향후 측정 동작과 동일한 방향으로 calibration을 수행하는 것이 좋습니다.
		정상적인 상황 (정의 팔 좌표가있는 station)에서 거리는 GETC를 사용하는 경우 음수이고
		PUTC를 사용하는 경우 양수 여야합니다.
		결과 calibration 값은 SMP 명령에 의해 motion parameter 파일에 저장되고 RMP 명령에 의해 복원
		될 수 있습니다.
 예제 : >ASL 5906 11500
		>AOFCS A 1
		>AOFD -1
		wafer Radius=5906 EELength=11500 Distance=15000
		Calibration data:
		Left: X=-2.9629, Y= 15.6370
		Right: X= 3.3586, Y= 16.1380
		Start positions: 6691,-5000,1689
		>
*/
API_EXPORT BOOL uvMDR_SetOtFCaliRunStationDirection(CHAR station, UINT8 direct)
{
	CHAR szCmd[32]	= {NULL};

	sprintf_s(szCmd, 32, "AOFCS %c %u", station, direct);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_aofcs, szCmd);
}
API_EXPORT BOOL uvMDR_SetOtFCaliRunStationDistance(CHAR station, UINT8 direct, DOUBLE dist)
{
	CHAR szCmd[32]	= {NULL};

	sprintf_s(szCmd, 32, "AOFCS %c %u %d", station, direct, GetAxisPosDev(ENG_RANC::rac_r1, dist));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_aofcs, szCmd);
}

/*
 desc : On-the-Fly (Align-the-Flight or Fly) 디버그 레벨 설정 / 반환
 parm : value	- [in]  요청된 상세 수준 혹은 -1 (설정 안함?) (범위: 0 ~ 15 or -1)
 retn : TRUE or FALSE
 note : 이 명령은 OtF 작동 중에 발생되는 디버그의 레벨 정보를 설정 합니다.
		기본 값은 0 (정보 없음) 이며, 최대 15 입니다.
*/
API_EXPORT BOOL uvMDR_SetOtFDebugLevel(INT8 value)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "AOFD %d", value);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_aofd, szCmd);
}
API_EXPORT BOOL uvMDR_GetOtFDebugLevel()
{
	CHAR szCmd[32]	= {"AOFD"};
	return g_pMDRThread->SendData(ENG_RSCC::rsc_aofd, szCmd, TRUE);
}

/*
 desc : 문제 해결 목적으로, On-the-Fly (Align-the-Flight or Fly) 동안 캡처된 Wafer Samples을 Dump 하는데 사용
 parm : None
 retn : TRUE or FALSE
 예제 : >AOFD 0
		>MVRC R 12000
		>AOFDUMP
		; O=1; R=40149:l; O=-1; R=45455:R;; O=1; R=106898:l; O=-1; R=106984:R;
		>AOFD 3
		>AOFDUMP
		Total of 4 positions collected
		Order: 0x0100; R=40149:l
		Order: 0x0101; R=45455:R; **
		Order: 0x0100; R=106898:l
		Order: 0x0101; R=106984:R; **
		>
*/
API_EXPORT BOOL uvMDR_SetOtFDebugDump()
{
	CHAR szCmd[32]	= {"AOFDUMP"};
	return g_pMDRThread->SendData(ENG_RSCC::rsc_aofdump, szCmd);
}

/*
 desc : On-the-Fly (Align-the-Flight or Fly) 동안 얻어진 샘플 결과 무시 여부 설정 / 반환
 parm : option	- [in]  0x00 : 중간에 있는 동안 얻어진 Samples를 무시하지 않습니다.
							   이런한 Samples가 생성되면 Action Command가 잘못된 결과 생산 (생성)
						0x01 : 수집 중에 처음 및 마지막으로 얻은 Samples 이외의 모든 Samples 무시
 retn : TRUE or FALSE
 note : 이 옵션은 Align-on-flign 동작 명령(AOFC, AOFCS, GETC, PUTC 등)이 Wafer 중간에서
		만들어지는 추가 Samples를 해석하는 방법 제어
*/
API_EXPORT BOOL uvMDR_SetOtFIgnoreMidSamples(UINT8 option)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "AOFIM %u", option);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_aofim, szCmd);
}
API_EXPORT BOOL uvMDR_GetOtFIgnoreMidSamples()
{
	CHAR szCmd[32]	= {"AOFIM"};
	return g_pMDRThread->SendData(ENG_RSCC::rsc_aofim, szCmd);
}


/* --------------------------------------------------------------------------------------------- */
/*                  Command Reference  (13.New commands for DUAL Arm Axis Only)                  */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 2 개의 축 (for R Axis)을 절대 위치로 이동
 parm : position	- [in]  이동하고자 하는 절대 위치 (단위: um)
 retn : TRUE or FALSE
 note : 이 명령은 두 Arm Axis인 R1 및 R2를 동일한 절대 위치로 이동하기 시작
*/
API_EXPORT BOOL uvMDR_SetDualMoveArmAbsolutePositionR(DOUBLE position)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "DMVA %d", GetAxisPosDev(ENG_RANC::rac_r1, position));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_dmva, szCmd);
}

/*
 desc : 2 개의 축 (for R Axis)을 상대 위치로 이동
 parm : distance	- [in]  이동하고자 하는 거리 값 (단위: um)
 retn : TRUE or FALSE
 note : 이 명령은 두 Arm Axis의 각각 현재 위치에서, 주어진 이동 거리만큼 동시에 이동하기 시작 함
*/
API_EXPORT BOOL uvMDR_SetDualMoveArmRelativeDistanceR(DOUBLE distance)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "DMVR %d", GetAxisPosDev(ENG_RANC::rac_r1, distance));
	return g_pMDRThread->SendData(ENG_RSCC::rsc_dmvr, szCmd);
}

/*
 desc : Wafer Mapping 에 사용되는 Arm 설정 / 반환
 parm : station	- [in]  Station Name (lower or uppoer case) (a ~ z or A ~ Z) (one character)
		arm_no	- [in]  Arm Number (0: Primary (Right), 1: Secondary (Left))
 retn : TRUE or FALSE
 note : 하드웨어 구성에 따라 웨이퍼 매핑을 로봇 Arm 중 하나에 장착 할 수 있습니다.
		이 명령은 수직 모션이 시작되기 전에 스캔 좌표로 이동해야하는 Arm을 선택하는데 사용됩니다.
		다른 Arm은 스테이션 후퇴 위치로 이동 합니다
*/
API_EXPORT BOOL uvMDR_SetDualSelectMapperArm(CHAR station, UINT8 arm_no)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "SEE %c %u", station, arm_no);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_see, szCmd);
}
API_EXPORT BOOL uvMDR_GetDualSelectMapperArm(CHAR station)
{
	CHAR szCmd[32]	= {NULL};
	sprintf_s(szCmd, 32, "SEE %c", station);
	return g_pMDRThread->SendData(ENG_RSCC::rsc_see, szCmd, TRUE);
}

/* --------------------------------------------------------------------------------------------- */
/*                         외부 함수 - Robot for Logosol < for Common >                          */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 가장 최근에 송신한 명령에 대해 수신된 패킷이 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_IsLastSendAckRecv()
{
	return g_pMDRThread->IsLastSendAckRecv();
}

/*
 desc : 가장 최근에 송신한 명령에 대해 수신 응답이 성공 했는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_IsLastSendAckSucc()
{
	return g_pMDRThread->IsLastSendAckSucc();
}

/*
 desc : 현재 Station이 서비스 상태 여부 (Busy, Idle, Not defined 인지 여부. Only Station !!!)
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_IsStationServiceBusy()
{
	return (0x00 == g_pstShMemMDR->station_service_busy);
}
API_EXPORT BOOL uvMDR_IsStationServiceIdle()
{
	return (0x01 == g_pstShMemMDR->station_service_busy);
}
API_EXPORT BOOL uvMDR_IsStationServiceUndefined()
{
	return (0xff == g_pstShMemMDR->station_service_busy);
}

/*
 desc : 확장 에러 값에 대한 에러 상세 설명 반환 (즉, Error codes returned by EERR)
 parm : mesg	- [out] 에러 코드에 대한 상세 설명이 저장될 반환 버퍼
		size	- [in]  'mesg' 버퍼 크기
 retn : FALSE (에러 없음), TRUE (에러 있음)
*/
API_EXPORT BOOL uvMDR_GetExtErrorMesg(TCHAR *mesg, UINT32 size)
{
	return g_pMDRThread->GetExtErrorMesg(mesg, size);
}

/*
 desc : 시스템 상태 값에 대한 에러 메시지 반환
 parm : mesg	- [out] 에러 코드에 대한 상세 설명이 저장될 반환 버퍼
		size	- [in]  'mesg' 버퍼 크기
 retn : FALSE (에러 없음), TRUE (에러 있음)
 note : "mesg" 버퍼를 최소 1024만큼 설정해 줘야 됨
*/
API_EXPORT BOOL uvMDR_GetSystemStatusMesg(TCHAR *mesg, UINT32 size)
{
	return g_pMDRThread->GetSystemStatus(mesg, size);
}

/*
 desc : Home Switch 상태 값에 대한 에러 메시지 반환
 parm : mesg	- [out] 에러 코드에 대한 상세 설명이 저장될 반환 버퍼
		size	- [in]  'mesg' 버퍼 크기
 retn : FALSE (에러 없음), TRUE (에러 있음)
 note : "mesg" 버퍼를 최소 1024만큼 설정해 줘야 됨
*/
API_EXPORT BOOL uvMDR_GetHomeSwitchStateMesg(TCHAR *mesg, UINT32 size)
{
	return g_pMDRThread->GetHomeSwitchesState(mesg, size);
}

/*
 desc : 홈 스위치들의 상태에 대한 에러 메시지 반환 (즉, Error codes returned by RHS)
 parm : mesg	- [out] 에러 코드에 대한 상세 설명이 저장될 반환 버퍼
		size	- [in]  'mesg' 버퍼 크기
 retn : FALSE (에러 없음), TRUE (에러 있음)
*/
API_EXPORT BOOL uvMDR_GetHomeSwitchesState(TCHAR *mesg, UINT32 size)
{
	return g_pMDRThread->GetHomeSwitchesState(mesg, size);
}

/*
 desc : 가장 최근에 수신된 데이터가 있는지 여부
 parm : cmd		- [in]  검사 대상 명령어
		axis	- [in]  Axis Name (0:T (Angle), 1:R1 (Horizontal.Up), 2:R2 (Horizontal.Down), 3:Z (Vertical))
 retn : TRUE or FALSE
 note : 만약, 'cmd' 값에 값을 입력하지 않으면, 가장 최근에 송신한 명령어에 대해, 수신된 값이 존재하는지 여부
*/
API_EXPORT BOOL uvMDR_IsRecvCmdData(ENG_RSCC cmd, ENG_RANC axis)
{
	/* 가장 최근에 송신한 명령어에 대한 응답이 도착했는지 여부 확인 */
	if (!g_pMDRThread->IsLastSendAckRecv())	return FALSE;

	/* 어떠한 명령어도 입력되지 않았다면, 가장 최근에 송신한 명령어에 대한 수신 결과 확인 */
	if (cmd == ENG_RSCC::rsc_none)	cmd	= g_pMDRThread->GetLastSendCmd();

	if (cmd <= ENG_RSCC::rsc_ver)			return IsRecvDataInfoCmds(cmd, axis);		/* 1. Informational Commands */
	else if (cmd <= ENG_RSCC::rsc_ssp)		return IsRecvHousekeepCmds(cmd, axis);		/* 2. Housekeeping (Parameters save / restore) commands */
	else if (cmd <= ENG_RSCC::rsc_sta)		return IsRecvStatusDiagCmds(cmd, axis);		/* 3. Status and diagnostic commands */
	else if (cmd <= ENG_RSCC::rsc__ralmc)	return IsRecvAbsEncCmds(cmd, axis);			/* 4. Support for systems with absolute encoders */
	else if (cmd <= ENG_RSCC::rsc_vst_b)	return IsRecvBasicActCmds(cmd, axis);		/* 5. Basic (non-axes, non-station) action commands */
	else if (cmd <= ENG_RSCC::rsc_spd)		return IsRecvBasicMotionParams(cmd, axis);	/* 6. Basic axis motion parameters */
	else if (cmd <= ENG_RSCC::rsc_wsa)		return IsRecvAxisParmSetCmds(cmd, axis);	/* 7. Axis parameter sets, and custom parameters commands */
	else if (cmd <= ENG_RSCC::rsc_wmc)		return IsRecvBasicMotionCmds(cmd, axis);	/* 8. Basic axis motions commands */
	else if (cmd <= ENG_RSCC::rsc_vto)		return IsRecvStaParamCmds(cmd, axis);		/* 9. Station parameters commands */
	else if (cmd <= ENG_RSCC::rsc_spg)		return IsRecvStaMotionCmds(cmd, axis);		/* 10.Station motion commands */
	else if (cmd <= ENG_RSCC::rsc_tsp)		return IsRecvStaScanCmds(cmd, axis);		/* 11.Station scanning parameters and motion commands */
	else if (cmd <= ENG_RSCC::rsc_rwic)		return IsRecvOtFCmds(cmd, axis);			/* 12.Align-on-Flight (On-The-Flight, OtF) parameters and commands */
	else if (cmd <= ENG_RSCC::rsc_see)		return IsRecvNewDualArms(cmd, axis);		/* 13. New Command for DUAL Arm */

	return FALSE;
}
API_EXPORT BOOL uvMDR_IsRecvCmdLast(ENG_RANC axis)
{
	return uvMDR_IsRecvCmdData(g_pMDRThread->GetLastSendCmd(), axis);
}

/*
 desc : 장비 (시스템) 내부에 에러가 발생 했는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMDR_IsSystemError()
{
	/*  In case of error : 0x0001 + 0x0002 + 0x0010 + 0x0020 + 0x0040 + 0x0200 + 0x0400 + 0x2000 */
	if (g_pstShMemMDR->system_status == 0xffff)				return FALSE;
	if ((g_pstShMemMDR->system_status & 0x2673) == 0x0000)	return FALSE;
	/* 실제 에러 부분 */
	if (0x0001 == (g_pstShMemMDR->system_status & 0x0001))	LOG_ERROR(ENG_EDIC::en_lsdr, L"STA(0001): Unable to execute motion commands");
	if (0x0002 == (g_pstShMemMDR->system_status & 0x0002))	LOG_ERROR(ENG_EDIC::en_lsdr, L"STA(0002): Previous command returned an error. See PER command for details");
	if (0x0010 == (g_pstShMemMDR->system_status & 0x0010))	LOG_ERROR(ENG_EDIC::en_lsdr, L"STA(0010): Motion error"); 
	if (0x0020 == (g_pstShMemMDR->system_status & 0x0020))	LOG_ERROR(ENG_EDIC::en_lsdr, L"STA(0020): Motion limits violation");
	if (0x0040 == (g_pstShMemMDR->system_status & 0x0040))	LOG_ERROR(ENG_EDIC::en_lsdr, L"STA(0040): Home procedure not executed");
	if (0x0200 == (g_pstShMemMDR->system_status & 0x0200))	LOG_ERROR(ENG_EDIC::en_lsdr, L"STA(0200): Single-axis prealigner servo off");
	if (0x0400 == (g_pstShMemMDR->system_status & 0x0400))	LOG_ERROR(ENG_EDIC::en_lsdr, L"STA(0400): Servo OFF on one or more axes"); 
	if (0x2000 == (g_pstShMemMDR->system_status & 0x2000))	LOG_ERROR(ENG_EDIC::en_lsdr, L"STA(2000): I/O File error");

	return TRUE;
}


#ifdef __cplusplus
}
#endif