
/*
 desc : Structure
*/

#pragma once

#include "global.h"
#include "conf_comn.h"
#include "recipe_comn.h"


/* --------------------------------------------------------------------------------------------- */
/*                                             구조체                                            */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 8)

/*UvDi15um AbsolicsSR*/
/* The recipe base information for job file */
typedef struct  __st_recipe_job_additional_function_
{
	UINT32				material_thick;			/* Exposure Material Thickness (unit: um)						*/
	UINT32				cali_thick;				/* Cail Thickness (unit: um)									*/
	UINT16				frame_rate;				/* Frame Rate (1 ~ 999) (실제 값은 나누기 1000)					*/
	UINT8				step_size : 4;			/* Step Size (1 ~ 7)											*/
	UINT8				u8_reserved;

	FLOAT				expo_energy;			/* 노광 에너지 (unit: mJ)										*/
	FLOAT				expo_speed;				/* 노광 속도 (unit: mm/sec)										*/
	
	PCHAR				job_name;				/* job Name														*/
	PCHAR				gerber_path;			/* Gerber Path													*/
	PCHAR				gerber_name;			/* Gerber Name (Only Gerber Name, Path is not included)			*/
	PCHAR				align_recipe;			/* 얼라인 동작시 필요한 정보 레시피								*/
	PCHAR				expo_recipe;			/* 작업시 필요한 인터락 및 부가 기능 레시피						*/

	/*
	 desc : 메모리 할당
	 parm : None
	 retn : None
	*/
	VOID Init()
	{
		cali_thick = 960;
		step_size = 1;
		frame_rate = 999;
		job_name = (PCHAR)Alloc(RECIPE_NAME_LENGTH);
		gerber_path = (PCHAR)Alloc(MAX_PATH_LEN);
		gerber_name = (PCHAR)Alloc(MAX_GERBER_NAME);
		align_recipe = (PCHAR)Alloc(RECIPE_NAME_LENGTH);
		expo_recipe = (PCHAR)Alloc(RECIPE_NAME_LENGTH);

		memset(job_name, 0x00, RECIPE_NAME_LENGTH);
		memset(gerber_path, 0x00, MAX_PATH_LEN);
		memset(gerber_name, 0x00, MAX_GERBER_NAME);
		memset(align_recipe, 0x00, RECIPE_NAME_LENGTH);
		memset(expo_recipe, 0x00, RECIPE_NAME_LENGTH);
	}

	/*
	 desc : 메모리 해제
	 parm : None
	 retn : None
	*/
	VOID Close()
	{
		if (job_name)			Free(job_name);
		if (gerber_path)		Free(gerber_path);
		if (gerber_name)		Free(gerber_name);
		if (align_recipe)		Free(align_recipe);
		if (expo_recipe)		Free(expo_recipe);

	}

	/*
	 desc : Gerber Name 초기화
	 parm : None
	 retn : None
	*/
	VOID ResetMemData()
	{

		memset(job_name, 0x00, RECIPE_NAME_LENGTH);
		memset(gerber_path, 0x00, MAX_PATH_LEN);
		memset(gerber_name, 0x00, MAX_GERBER_NAME);
		memset(align_recipe, 0x00, RECIPE_NAME_LENGTH);
		memset(expo_recipe, 0x00, RECIPE_NAME_LENGTH);

	}

	/*
	 desc : 설정된 값이 유효한지 여부
	 parm : None
	 retn : TRUE (유효함) or FALSE (임의 값이 설정되지 않음)
	 note : Mark Scale Range 값은 반드시 50.0f 이상 값이어야 함
	*/
	BOOL IsValid()
	{
		BOOL bSucc = FALSE;

		//bSucc = material_thick > 0 && cali_thick > 0 && frame_rate > 0 && step_size > 0;
		bSucc = material_thick > 0;

		if (bSucc)
		{
			bSucc = strlen(job_name) > 0 && strlen(gerber_path) > 0 && strlen(gerber_name) > 0 &&
				strlen(align_recipe) > 0 && strlen(expo_recipe);
		}

		return bSucc;
	}


}	STG_RJAF, * LPG_RJAF;

/*UvDi15um AbsolicsSR*/
/*노광 동작시 필요한 인터락 및 부가 기능 정보*/
/* Recipe for interlock and additional function information required during exposure work */
typedef struct __st_recipe_expo_additional_function__
{
	UINT8				serial_flip_h : 1;		/* Serial String Flip ? Up/Down									*/
	UINT8				serial_flip_v : 1;		/* Serial String Flip ? Left/Right								*/
	UINT8				scale_flip_h : 1;		/* Scale String Flip ? Up/Down									*/
	UINT8				scale_flip_v : 1;		/* Scale String Flip ? Left/Right								*/
	UINT8				text_flip_h : 1;		/* Text String Flip ? Up/Down									*/
	UINT8				text_flip_v : 1;		/* Text String Flip ? Left/Right								*/

	UINT8				led_duty_cycle;			/* Led Duty Cycle (0 ~ 90%)										*/
	UINT8				u8_reserved[1];

	UINT16				dcode_serial;			/* Serial Number DCode											*/
	UINT16				dcode_scale;			/* Scale DCode													*/
	UINT16				dcode_text;				/* Text panel Data												*/


	DOUBLE				mark_score_accept;		/* 마크 유효 Score 값 (검색된 결과 중 이 값보다 작으면 배제)	*/
	DOUBLE				mark_scale_range;		/* 마크 유효 Scale 값 (100으로 나눈 후 이 값 +/- 범위 내에서 검색하라고 지정. 이 값 100 가까울수록 검색된 Scale 값은 1.0에 가깝고 100에 멀어질수록 1.0에 멀어짐. 설명하기 참 난해)	*/
												/* 즉, 이 값은 등록된 마크 기준으로 검색되는 대상의 크기 범위를 지정 함. 작은 것부터 큰 것까지 검색 */
												/* 만약, 이 값이 100.0이면 등록된 마크와 동일한 크기의 객체를 검색한다. */
												/* 검색 범위 내에서 등록된 마크와 가장 유사한 (Score 값이 가장 높은) 결과를 추출해 냄 */
												/* !!! 최소로 입력 가능한 값이 50.0f 값임 !!! */
	DOUBLE				real_scale_range;		/* 거버의 크기 대비 소재 크기의 배율에 대한 오차 범위*/
	DOUBLE				real_rotaion_range;		/* 소재의 수평 회전에 대한 오차 범위*/
	DOUBLE				global_mark_dist_rate;	/* Align 후 Global Mark 간의 거리 오차 허용 값 비율 (0.00001~0.0001)	*/
	PUINT32				global_mark_dist;		/* Align 후 Global Mark 간의 거리 오차 허용 값 (unit: 100 nm or 0.1 um)	*/


	PCHAR				expo_name;				/* expo Recipe Name												*/
	PCHAR				power_name;				/* Led Power Recipe Name										*/
	PCHAR				text_string;			/* Head String 부분												*/

	/*
	 desc : 메모리 할당
	 parm : None
	 retn : None
	*/
	VOID Init()
	{
		expo_name = (PCHAR)Alloc(RECIPE_NAME_LENGTH);
		power_name = (PCHAR)Alloc(LED_POWER_NAME_LENGTH);
		text_string = (PCHAR)Alloc(MAX_PANEL_TEXT_STRING);

		memset(expo_name, 0, RECIPE_NAME_LENGTH);
		memset(power_name, 0, LED_POWER_NAME_LENGTH);
		memset(text_string, 0, MAX_PANEL_TEXT_STRING);

		global_mark_dist = (PUINT32)Alloc(sizeof(UINT32) * MAX_GLOBAL_MARK_DIST_CHECK_COUNT);

		dcode_serial = 12;
		dcode_scale = 13;
		dcode_text = 14;
		mark_score_accept = 80;
		mark_scale_range = 100;
		led_duty_cycle = 10;
		real_scale_range = 0;
		real_rotaion_range = 0;


		global_mark_dist_rate = 0.0005;
		for (int i = 0; i < MAX_GLOBAL_MARK_DIST_CHECK_COUNT; i++)
		{
			global_mark_dist[i] = 0.01;			//	 warning C4244: '=': 'double'에서 'unsigned int'(으)로 변환하면서 데이터가 손실될 수 있습니다.
		}
	}

	/*
	 desc : 메모리 해제
	 parm : None
	 retn : None
	*/
	VOID Close()
	{
		if (expo_name)				Free(expo_name);
		if (text_string)			Free(text_string);
		if (global_mark_dist)		Free(global_mark_dist);
		if (power_name)				Free(power_name);
	}

	/*
	 desc : Gerber Name 초기화
	 parm : None
	 retn : None
	*/
	VOID ResetMemData()
	{
		dcode_serial = 12,
		dcode_scale = 13,
		dcode_text = 14,
		mark_score_accept = 0;
		mark_scale_range = 0;
		real_scale_range = 0;
		real_rotaion_range = 0;
		memset(text_string, 0x00, MAX_PANEL_TEXT_STRING);
		memset(expo_name, 0x00, RECIPE_NAME_LENGTH);
		memset(power_name, 0x00, LED_POWER_NAME_LENGTH);
		memset(global_mark_dist, 0x00, sizeof(UINT32) * MAX_GLOBAL_MARK_DIST_CHECK_COUNT);
	}


	/*
	 desc : 설정된 값이 유효한지 여부
	 parm : None
	 retn : TRUE (유효함) or FALSE (임의 값이 설정되지 않음)
	 note : Mark Scale Range 값은 반드시 50.0f 이상 값이어야 함
	*/
	BOOL IsValid()
	{
		UINT8 i;
		BOOL bSucc = FALSE;

		bSucc = led_duty_cycle > 0 && /*mark_count > 0 && */
			mark_score_accept > 0.0f && mark_scale_range >= 50.0f/* && expo_speed > 0.0f*/;
		if (bSucc > 0)
		{
			if ((mark_score_accept > 0.0f) && (mark_scale_range >= 50.0f))
			{
				for (i = 0; i < 6; i++)
				{
					bSucc = global_mark_dist[i] > 0;
					if (!bSucc)	break;
				}

			}
		}
		if (bSucc)
		{
			bSucc = strlen(expo_name) > 0 && strlen(power_name) > 0;
		}

		return bSucc;
	}

	/*
	 desc : 마크 데이터가 존재하는지 여부
	 parm : count	- [in]  레시피에 등록된 거버 개수
	 retn : TRUE or FALSE
	*/
	BOOL IsMarkExist()
	{
		UINT8 i = 0x00;

		if (!global_mark_dist)	return FALSE;
		for (; i < UINT32(MAX_GLOBAL_MARK_DIST_CHECK_COUNT); i++)
		{
			if (global_mark_dist[i])	return TRUE;
		}
		return FALSE;
	}

}	STG_REAF, * LPG_REAF;


/* Optical Speed Step Data */
typedef struct __st_optical_speed_step_data__
{
	UINT8				scroll_mode;			/* 1 ~ 7 */
	UINT8				u8_reserved[7];
	INT32				nega_offset_px;			/* 픽셀 크기로서 광학계 모델별로 다름. LLS25의 경우 1 픽셀 = 10.0 um, LLS10의 경우 1 픽셀 = 5.4 um */
	INT32				i32_reserved;
	INT32				step_y_nm[MAX_PH];		/* [0] array는 무조건 0 값. unit : nm */
	UINT16				frame_rate;				/* Frame Rate = 나누기 1000 값을 해준 것이 실제 값 */
	UINT16				u16_reserved[3];
	UINT32				delay_posi_nsec;		/* Delay in positive moving direction (unit : nsecs) */
	UINT32				delay_nega_nsec;		/* Delay in negative moving direction (unit : nsecs) */
	UINT32				step_x_nm[MAX_PH];		/* [0] array는 무조건 0 값. unit : nm */

	/*
	 desc : 단차 데이터 초기화
	 parm : None
	 retn : None
	*/
	VOID Reset()
	{
		frame_rate		= 0;
		nega_offset_px	= 0;
		delay_posi_nsec	= 0;
		delay_nega_nsec	= 0;
		scroll_mode		= 1;
		memset(step_x_nm, 0x00, sizeof(UINT32) * MAX_PH);
		memset(step_y_nm, 0x00, sizeof(INT32) * MAX_PH);
	}

	/*
	 desc : Step X/Y 복사
	 parm : data	- [in]  복사될 원본 데이터가 저장된 포인터
	 retn : None
	*/
	VOID Copy(PUINT32 x, PINT32 y)
	{
		memcpy(step_x_nm, x, sizeof(UINT32) * MAX_PH);
		memcpy(step_y_nm, y, sizeof(INT32) * MAX_PH);
	}

	/*
	 desc : 값이 유효한지
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsValid()
	{
		if (/*!step_x_nm[1]  || !step_y_nm[1] || */frame_rate < 1)	return FALSE;
		return TRUE;
	}

}	STG_OSSD,	*LPG_OSSD;

/* Optical Speed Correction Y Table */
typedef struct __st_optical_speed_correction_y__
{
	UINT16				frame_rate;						/* Frame Rate = 나누기 1000 값을 해준 것이 실제 값 */
	UINT16				stripe_count;					/* adjust에 등록된 데이터 (정역 단차) 개수 */
	UINT16				u16_reserved[2];
	INT32				adjust[MAX_CORRECT_Y_COUNT];	/* 픽셀 크기로서 광학계 모델별로 다름. LLS25의 경우 1 픽셀 = 10.0 um, LLS10의 경우 1 픽셀 = 5.4 um */

	/*
	 desc : 단차 데이터 초기화
	 parm : None
	 retn : None
	*/
	VOID Reset()
	{
		frame_rate		= 0;
		stripe_count	= 0;
		memset(adjust, 0x00, sizeof(INT32) * MAX_CORRECT_Y_COUNT);
	}

	/*
	 desc : 값이 유효한지
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsValid()
	{
		if (stripe_count < 1 || frame_rate < 1)	return FALSE;
		return TRUE;
	}

}	STG_OSCY,	*LPG_OSCY;

/* Motion 동작 속도에 따른 Trigger Delay & Calibration 값 관리 */
typedef struct __st_trig_delay_calibration_position__
{
	UINT16				frame_rate;			/* 고유 Recipe Index 값 (Speed Rate * 1000) */
	UINT16				u16_reserved[3];
	INT32				trig_all_delay_y;	/* Y (상/하) 방향 Trigger Delay 값 공통 적용 (단위: 0.1um or 100 nm) */
	INT32				trig_delay_y1_y3;	/* Y1 대비 Y3 Trigger Delay 오차 값 (단위: 0.1um or 100 nm) */

	/*
	 desc : 값 유효
	 parm : None
	 retn : None
	*/
	BOOL IsValid()
	{
		return frame_rate > 0;
	}

}	STG_TDCP,	*LPG_TDCP;

/* 소재 두께에 따른 Align Camera 1/2의 X 및 Stage Y1/Y2 Motion 좌표 위치 값 */
typedef struct __st_material_thick_acam_calibration_position__
{
	UINT32				film_thick_um;							/* Photohead Focus (DOF) 조정할 때 사용되는 소재의 두께 (단위: 100nm or 0.1 um. 소숫점 4자리까지 유효) */
	UINT32				u32_reserved;
	DOUBLE				mark2_stage_y[2];						/* 거버에 저장된 Mark 2번 기준 얼라인 카메라 1 or Later의 Y 축 모션 좌표 (단위: mm) */
	DOUBLE				mark2_acam_x[2];						/* 거버에 저장된 Mark 2번 기준 얼라인 카메라 1 or Later의 X 축 모션 좌표 (단위: mm) */

	/*
	 desc : 값 유효
	 parm : None
	 retn : None
	*/
	BOOL IsValid()
	{

#ifdef CUSTOM_CODE_HDDI6
		return mark2_stage_y[0] > 0.0f && mark2_stage_y[1] > 0.0f;
#else
	return mark2_stage_y[0] > 0.0f && mark2_stage_y[1] > 0.0f &&
		mark2_acam_x[0] > 0.0f && mark2_acam_x[1] > 0.0f;
#endif

		
	}

	/*
	 desc : 카메라 1번 기준으로 카메라 2번의 오차 값 반환
	 parm : None
	 retn : 카메라 1번과 2번의 높이/폭 오차 값
	*/
	DOUBLE GetACamDistY()	{	return (mark2_stage_y[0] - mark2_stage_y[1]);	}
	DOUBLE GetACamDistX()	{	return (mark2_acam_x[0] - mark2_acam_x[1]);		}

}	STG_MACP,	*LPG_MACP;

#pragma pack (pop)	/* 8 bytes boundary sort */