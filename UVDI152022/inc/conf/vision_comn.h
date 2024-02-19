
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

#define MARK_MODEL_NAME_LENGTH					256		/* Align Mark Model Name ���� */
#define	MAX_GRABBED_IMAGE						8		/* �ִ� Grabbed Image ���� (ī�޶� ���� Grabbed Image �� ��) */
#define MAX_MODEL_REGIST_PARAM					5		/* �� ���� �ִ� ��� ������ �Ķ���� ���� */
#define	MAX_REGIST_MODEL						48		/* �ִ� ��� ������ �� ���� */

#define RECIPE_NAME_LENGTH						128
#define LED_POWER_NAME_LENGTH					128
/* --------------------------------------------------------------------------------------------- */
/*                                             ������                                            */
/* --------------------------------------------------------------------------------------------- */

/* Align Operation Method */
typedef enum class __en_align_operation_method_id__ : UINT8
{
	en_fixed		= 0x00,			/* Align Camera 2���� Mark 2 or 4�� ���� (�̵��ϸ鼭 ����) */
	en_moving		= 0x01,			/* Align Camera 2���� Mark 2 ���� �� �߰��� ���߰�, ī�޶� ��/�� ���� �� �ٽ� ������ 2�� ���� (�̵� �� ���߰� ����, �ٽ� �̵� �� ����) */
									/* �Ź� �� ��ũ�� 4 ���� ���簢�� (ī�޶� FOV�� ������ ���ϴ� ���)�� �ƴ� ��� */
	en_each			= 0x02,			/* Align Camera 1���� ���� ��ũ ���� (���߸鼭 ����) */

}	ENG_AOMI;


/* --------------------------------------------------------------------------------------------- */
/*                                             ����ü                                            */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 8)

/* ���� ȭ�鿡 ��µ� Align Mark ��� �� �ӽ� ���� - ������ ��µ� ����� ���ϱ� ���� */
typedef struct __st_align_mark_draw_result__
{
	DOUBLE				score_rate;		/* ��Ī �˻��� (����: %) */
	DOUBLE				scale_rate;		/* ��Ī ������ �̹��� ���� (����: %) */
	DOUBLE				move_mm_x;
	DOUBLE				move_mm_y;

	/*
	 desc : ������ ������ �ʱ�ȭ
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

/* ���� �ֱٿ� Grabbed Image�� ��� (����) �� ���� */
typedef struct __st_align_camera_last_result__
{
	UINT8				marked;					/* ���� ���� ��ȿ�� ������ ���� */
	UINT8				u8_reserved[7];
	/* Mark �˻� ��� �� - mm */
	DOUBLE				move_mm_x;				/* �˻��� Mark�� �߽ɿ��� ������ ���� �� (����: mm) */
	DOUBLE				move_mm_y;				/* �˻��� Mark�� �߽ɿ��� ������ ���� �� (����: mm) */

}	STG_ACLR,	*LPG_ACLR;

/* Calibration Error Value for Align Camera (����� ī�޶� ���� ���� ���� ��) */
typedef struct __st_align_camera_calibrate_error__
{
	INT32				acam_cali_x;		/* Align Camera X ���� ���� ��ġ�� ���� ���� �� (����: 0.1 um or 100 nm) */
	INT32				stage_cali_y;		/* Stage Y ���� ���� ��ġ�� ���� ���� �� (����: 0.1 um or 100 nm) */
	INT32				acam_motion_x;		/* Align Camera X ���� Motion Position (����: 0.1 um or 100 nm) */
	INT32				stage_motion_y;		/* Stage Y ���� Motion Position (����: 0.1 um or 100 nm) */
	INT32				gerb_mark_x_diff;	/* �Ź��� �����ϴ� �� Mark���� X ���� ���� �� �ӽ� ���� (����: 0.1 um or 100 nm) */
	INT32				i32_reserved;

}	STG_ACCE,	*LPG_ACCE;

#pragma pack (pop)	// 8 bytes

#pragma pack (push, 1)


#pragma pack (pop)	// 1 bytes
