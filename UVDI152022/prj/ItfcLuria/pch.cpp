// pch.cpp: 미리 컴파일된 헤더에 해당하는 소스 파일

#include "pch.h"

/* --------------------------------------------------------------------------------------------- */
/*                                           전역 변수                                           */
/* --------------------------------------------------------------------------------------------- */

LPG_CIEA	g_pstConfig	= NULL;

/* --------------------------------------------------------------------------------------------- */
/*                                           전역 함수                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 엔진 내부 환경 파일 구조체 포인터 반환
 parm : None
 retn : 구조체 포인터 (공유 메모리 영역)
*/
LPG_CIEA GetConfig()
{
	return g_pstConfig;
}

/*
 desc : 엔진 동작이 데모로 동작하는지 여부
 parm : None
 retn : TRUE or FALSE
*/
BOOL IsRunDemo()
{
	if (!g_pstConfig)	return TRUE;
	return g_pstConfig->IsRunDemo();
}

/*
 desc : Write Only 명령만 존재하는 경우인지 여부
 parm : f_id	- [in]  Family ID
		u_id	- [in]  User ID
 retn : TRUE or FALSE
*/
BOOL IsWriteOnlyCmd(UINT8 f_id, UINT8 u_id)
{
	/* Failmily ID 구분 */
	switch (f_id)
	{
	case UINT8(ENG_LUDF::en_system)	:
		switch (ENG_LCSS(u_id))
		{
		case ENG_LCSS::en_initialize_hardware		:
		case ENG_LCSS::en_set_system_shutdown		:
		case ENG_LCSS::en_upgrade_firmware			:
		case ENG_LCSS::en_load_internal_test_image	:
		case ENG_LCSS::en_set_light_intensity		:	return TRUE;
		}
		break;
	case UINT8(ENG_LUDF::en_job_management)	:
		switch (ENG_LCJM(u_id))
		{
		case ENG_LCJM::en_assign_job				:
		case ENG_LCJM::en_delete_selected_job		:
		case ENG_LCJM::en_load_selected_job			:	return TRUE;
		}
		break;
	case UINT8(ENG_LUDF::en_panel_preparation)	:
		switch (ENG_LCPP(u_id))
		{
		case ENG_LCPP::en_run_registration			:
		case ENG_LCPP::en_remove_panel_data			:	return TRUE;
		}
		break;
	case UINT8(ENG_LUDF::en_exposure)	:
		switch (ENG_LCEP(u_id))
		{
		case ENG_LCEP::en_pre_print							:
		case ENG_LCEP::en_print								:
		case ENG_LCEP::en_abort								:
		case ENG_LCEP::en_reset_exposure_state				:
		case ENG_LCEP::en_move_table_to_exposure_start_pos	:
		case ENG_LCEP::en_light_ramp_up_down				:	return TRUE;
		}
		break;
	case UINT8(ENG_LUDF::en_luria_com_management)	:
		switch (ENG_LCCM(u_id))
		{
		case ENG_LCCM::en_test_announcement			:	return TRUE;
		}
		break;
	case UINT8(ENG_LUDF::en_focus)	:
		switch (ENG_LCPF(u_id))
		{
		case ENG_LCPF::en_initialize_focus			:
		case ENG_LCPF::en_set_position_multi		:	return TRUE;
		}
		break;
#if 0
	case ENG_LUDF::en_calibration	:
		break;
#endif
	}

	return FALSE;
}
