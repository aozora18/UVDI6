
/*
 desc : Structure
*/

#pragma once

#include "global.h"
#include "conf_comn.h"
#include "recipe_comn.h"


/* --------------------------------------------------------------------------------------------- */
/*                                             ����ü                                            */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 8)

/*UvDi15um AbsolicsSR*/
/* The recipe base information for job file */
typedef struct  __st_recipe_job_additional_function_
{
	UINT32				material_thick;			/* Exposure Material Thickness (unit: um)						*/
	UINT32				cali_thick;				/* Cail Thickness (unit: um)									*/
	UINT16				frame_rate;				/* Frame Rate (1 ~ 999) (���� ���� ������ 1000)					*/
	UINT8				step_size : 4;			/* Step Size (1 ~ 7)											*/
	UINT8				u8_reserved;

	FLOAT				expo_energy;			/* �뱤 ������ (unit: mJ)										*/
	FLOAT				expo_speed;				/* �뱤 �ӵ� (unit: mm/sec)										*/
	
	PCHAR				job_name;				/* job Name														*/
	PCHAR				gerber_path;			/* Gerber Path													*/
	PCHAR				gerber_name;			/* Gerber Name (Only Gerber Name, Path is not included)			*/
	PCHAR				align_recipe;			/* ����� ���۽� �ʿ��� ���� ������								*/
	PCHAR				expo_recipe;			/* �۾��� �ʿ��� ���Ͷ� �� �ΰ� ��� ������						*/

	/*
	 desc : �޸� �Ҵ�
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
	 desc : �޸� ����
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
	 desc : Gerber Name �ʱ�ȭ
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
	 desc : ������ ���� ��ȿ���� ����
	 parm : None
	 retn : TRUE (��ȿ��) or FALSE (���� ���� �������� ����)
	 note : Mark Scale Range ���� �ݵ�� 50.0f �̻� ���̾�� ��
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
/*�뱤 ���۽� �ʿ��� ���Ͷ� �� �ΰ� ��� ����*/
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


	DOUBLE				mark_score_accept;		/* ��ũ ��ȿ Score �� (�˻��� ��� �� �� ������ ������ ����)	*/
	DOUBLE				mark_scale_range;		/* ��ũ ��ȿ Scale �� (100���� ���� �� �� �� +/- ���� ������ �˻��϶�� ����. �� �� 100 �������� �˻��� Scale ���� 1.0�� ������ 100�� �־������� 1.0�� �־���. �����ϱ� �� ����)	*/
												/* ��, �� ���� ��ϵ� ��ũ �������� �˻��Ǵ� ����� ũ�� ������ ���� ��. ���� �ͺ��� ū �ͱ��� �˻� */
												/* ����, �� ���� 100.0�̸� ��ϵ� ��ũ�� ������ ũ���� ��ü�� �˻��Ѵ�. */
												/* �˻� ���� ������ ��ϵ� ��ũ�� ���� ������ (Score ���� ���� ����) ����� ������ �� */
												/* !!! �ּҷ� �Է� ������ ���� 50.0f ���� !!! */
	DOUBLE				real_scale_range;		/* �Ź��� ũ�� ��� ���� ũ���� ������ ���� ���� ����*/
	DOUBLE				real_rotaion_range;		/* ������ ���� ȸ���� ���� ���� ����*/
	DOUBLE				global_mark_dist_rate;	/* Align �� Global Mark ���� �Ÿ� ���� ��� �� ���� (0.00001~0.0001)	*/
	PUINT32				global_mark_dist;		/* Align �� Global Mark ���� �Ÿ� ���� ��� �� (unit: 100 nm or 0.1 um)	*/


	PCHAR				expo_name;				/* expo Recipe Name												*/
	PCHAR				power_name;				/* Led Power Recipe Name										*/
	PCHAR				text_string;			/* Head String �κ�												*/

	/*
	 desc : �޸� �Ҵ�
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
			global_mark_dist[i] = 0.01;			//	 warning C4244: '=': 'double'���� 'unsigned int'(��)�� ��ȯ�ϸ鼭 �����Ͱ� �սǵ� �� �ֽ��ϴ�.
		}
	}

	/*
	 desc : �޸� ����
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
	 desc : Gerber Name �ʱ�ȭ
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
	 desc : ������ ���� ��ȿ���� ����
	 parm : None
	 retn : TRUE (��ȿ��) or FALSE (���� ���� �������� ����)
	 note : Mark Scale Range ���� �ݵ�� 50.0f �̻� ���̾�� ��
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
	 desc : ��ũ �����Ͱ� �����ϴ��� ����
	 parm : count	- [in]  �����ǿ� ��ϵ� �Ź� ����
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
	INT32				nega_offset_px;			/* �ȼ� ũ��μ� ���а� �𵨺��� �ٸ�. LLS25�� ��� 1 �ȼ� = 10.0 um, LLS10�� ��� 1 �ȼ� = 5.4 um */
	INT32				i32_reserved;
	INT32				step_y_nm[MAX_PH];		/* [0] array�� ������ 0 ��. unit : nm */
	UINT16				frame_rate;				/* Frame Rate = ������ 1000 ���� ���� ���� ���� �� */
	UINT16				u16_reserved[3];
	UINT32				delay_posi_nsec;		/* Delay in positive moving direction (unit : nsecs) */
	UINT32				delay_nega_nsec;		/* Delay in negative moving direction (unit : nsecs) */
	UINT32				step_x_nm[MAX_PH];		/* [0] array�� ������ 0 ��. unit : nm */

	/*
	 desc : ���� ������ �ʱ�ȭ
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
	 desc : Step X/Y ����
	 parm : data	- [in]  ����� ���� �����Ͱ� ����� ������
	 retn : None
	*/
	VOID Copy(PUINT32 x, PINT32 y)
	{
		memcpy(step_x_nm, x, sizeof(UINT32) * MAX_PH);
		memcpy(step_y_nm, y, sizeof(INT32) * MAX_PH);
	}

	/*
	 desc : ���� ��ȿ����
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
	UINT16				frame_rate;						/* Frame Rate = ������ 1000 ���� ���� ���� ���� �� */
	UINT16				stripe_count;					/* adjust�� ��ϵ� ������ (���� ����) ���� */
	UINT16				u16_reserved[2];
	INT32				adjust[MAX_CORRECT_Y_COUNT];	/* �ȼ� ũ��μ� ���а� �𵨺��� �ٸ�. LLS25�� ��� 1 �ȼ� = 10.0 um, LLS10�� ��� 1 �ȼ� = 5.4 um */

	/*
	 desc : ���� ������ �ʱ�ȭ
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
	 desc : ���� ��ȿ����
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsValid()
	{
		if (stripe_count < 1 || frame_rate < 1)	return FALSE;
		return TRUE;
	}

}	STG_OSCY,	*LPG_OSCY;

/* Motion ���� �ӵ��� ���� Trigger Delay & Calibration �� ���� */
typedef struct __st_trig_delay_calibration_position__
{
	UINT16				frame_rate;			/* ���� Recipe Index �� (Speed Rate * 1000) */
	UINT16				u16_reserved[3];
	INT32				trig_all_delay_y;	/* Y (��/��) ���� Trigger Delay �� ���� ���� (����: 0.1um or 100 nm) */
	INT32				trig_delay_y1_y3;	/* Y1 ��� Y3 Trigger Delay ���� �� (����: 0.1um or 100 nm) */

	/*
	 desc : �� ��ȿ
	 parm : None
	 retn : None
	*/
	BOOL IsValid()
	{
		return frame_rate > 0;
	}

}	STG_TDCP,	*LPG_TDCP;

/* ���� �β��� ���� Align Camera 1/2�� X �� Stage Y1/Y2 Motion ��ǥ ��ġ �� */
typedef struct __st_material_thick_acam_calibration_position__
{
	UINT32				film_thick_um;							/* Photohead Focus (DOF) ������ �� ���Ǵ� ������ �β� (����: 100nm or 0.1 um. �Ҽ��� 4�ڸ����� ��ȿ) */
	UINT32				u32_reserved;
	DOUBLE				mark2_stage_y[2];						/* �Ź��� ����� Mark 2�� ���� ����� ī�޶� 1 or Later�� Y �� ��� ��ǥ (����: mm) */
	DOUBLE				mark2_acam_x[2];						/* �Ź��� ����� Mark 2�� ���� ����� ī�޶� 1 or Later�� X �� ��� ��ǥ (����: mm) */

	/*
	 desc : �� ��ȿ
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
	 desc : ī�޶� 1�� �������� ī�޶� 2���� ���� �� ��ȯ
	 parm : None
	 retn : ī�޶� 1���� 2���� ����/�� ���� ��
	*/
	DOUBLE GetACamDistY()	{	return (mark2_stage_y[0] - mark2_stage_y[1]);	}
	DOUBLE GetACamDistX()	{	return (mark2_acam_x[0] - mark2_acam_x[1]);		}

}	STG_MACP,	*LPG_MACP;

#pragma pack (pop)	/* 8 bytes boundary sort */