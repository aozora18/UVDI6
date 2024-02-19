
/*
 desc : Structure
*/

#pragma once

#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
#include "conf_podis.h"
#elif (CUSTOM_CODE_GEN2I == DELIVERY_PRODUCT_ID)
#include "conf_gen2i.h"
#elif (CUSTOM_CODE_UVDI15 == DELIVERY_PRODUCT_ID)
#include "conf_uvdi15.h"

#endif

#define MARK_MODEL_NAME_LENGTH					256		/* Align Mark Model Name 길이 */
#define	MAX_GRABBED_IMAGE						8		/* 최대 Grabbed Image 개수 (카메라 별로 Grabbed Image 총 합) */
#define MAX_MODEL_REGIST_PARAM					5		/* 모델 별로 최대 등록 가능한 파라미터 개수 */
#define	MAX_REGIST_MODEL						48		/* 최대 등록 가능한 모델 개수 */

#define RECIPE_NAME_LENGTH						128
#define LED_POWER_NAME_LENGTH					128
/* --------------------------------------------------------------------------------------------- */
/*                                             열거형                                            */
/* --------------------------------------------------------------------------------------------- */

/* Align Operation Method */
typedef enum class __en_align_operation_method_id__ : UINT8
{
	en_fixed		= 0x00,			/* Align Camera 2개로 Mark 2 or 4개 측정 (이동하면서 측정) */
	en_moving		= 0x01,			/* Align Camera 2개로 Mark 2 측정 후 중간에 멈추고, 카메라 좌/우 조정 후 다시 나머지 2개 측정 (이동 후 멈추고 측정, 다시 이동 후 측정) */
									/* 거버 내 마크의 4 점이 직사각형 (카메라 FOV에 들어오지 못하는 경우)이 아닌 경우 */
	en_each			= 0x02,			/* Align Camera 1개로 임의 마크 측정 (멈추면서 측정) */

}	ENG_AOMI;


/* --------------------------------------------------------------------------------------------- */
/*                                             구조체                                            */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 8)

/* 윈도 화면에 출력된 Align Mark 결과 값 임시 저장 - 이전에 출력된 영상과 비교하기 위함 */
typedef struct __st_align_mark_draw_result__
{
	DOUBLE				score_rate;		/* 매칭 검색률 (단위: %) */
	DOUBLE				scale_rate;		/* 매칭 스케일 이미지 비율 (단위: %) */
	DOUBLE				move_mm_x;
	DOUBLE				move_mm_y;

	/*
	 desc : 쓰레기 값으로 초기화
	 parm : None
	 retn : None
	*/
	VOID ResetDump()
	{
		score_rate	= FLT_MAX;
		scale_rate	= FLT_MAX;
		move_mm_x	= FLT_MAX;
		move_mm_y	= FLT_MAX;
	}

}	STG_AMDR,	*LPG_AMDR;

/* 가장 최근에 Grabbed Image의 결과 (오차) 값 저장 */
typedef struct __st_align_camera_last_result__
{
	UINT8				marked;					/* 현재 값이 유효한 것인지 여부 */
	UINT8				u8_reserved[7];
	/* Mark 검색 결과 값 - mm */
	DOUBLE				move_mm_x;				/* 검색된 Mark의 중심에서 떨어진 오차 값 (단위: mm) */
	DOUBLE				move_mm_y;				/* 검색된 Mark의 중심에서 떨어진 오차 값 (단위: mm) */

}	STG_ACLR,	*LPG_ACLR;

/* Calibration Error Value for Align Camera (얼라인 카메라 에러 오차 보정 값) */
typedef struct __st_align_camera_calibrate_error__
{
	INT32				acam_cali_x;		/* Align Camera X 축의 현재 위치에 대한 오차 값 (단위: 0.1 um or 100 nm) */
	INT32				stage_cali_y;		/* Stage Y 축의 현재 위치에 대한 오차 값 (단위: 0.1 um or 100 nm) */
	INT32				acam_motion_x;		/* Align Camera X 축의 Motion Position (단위: 0.1 um or 100 nm) */
	INT32				stage_motion_y;		/* Stage Y 축의 Motion Position (단위: 0.1 um or 100 nm) */
	INT32				gerb_mark_x_diff;	/* 거버에 존재하는 각 Mark들의 X 축의 오차 값 임시 저장 (단위: 0.1 um or 100 nm) */
	INT32				i32_reserved;

}	STG_ACCE,	*LPG_ACCE;

#pragma pack (pop)	// 8 bytes

#pragma pack (push, 1)


#pragma pack (pop)	// 1 bytes
