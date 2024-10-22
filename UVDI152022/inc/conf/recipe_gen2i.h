
/*
 desc : Gerber Recipe
*/

#pragma once

#include "../conf/recipe_comn.h"

/* --------------------------------------------------------------------------------------------- */
/*                                            상수값                                             */
/* --------------------------------------------------------------------------------------------- */

#define MAX_RECIPE_COUNT					1000
#define MIN_RECIP_DATA_LEN					45
#define MAX_RECIPE_NAME_LEN					128
#define MAX_LED_POWER_LEN					128
#define MAX_LOT_NUM_LEN						64

/* --------------------------------------------------------------------------------------------- */
/*                                            열거형                                             */
/* --------------------------------------------------------------------------------------------- */



/* --------------------------------------------------------------------------------------------- */
/*                                            구조체                                             */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 1)
#pragma pack (pop)	/* 1 bytes boundary sort */

#pragma pack (push, 4)
#pragma pack (pop)	/* 4 bytes boundary sort */

#pragma pack (push, 8)

/* The recipe information for gerber & printing */
typedef struct __st_recipe_info_gerber_printing__
{
	UINT8				gerber_count: 3;		/* 레시피에 등록된 거버 개수 (1 ~ 7) */
	UINT8				u8_bits_reserved:5;

	UINT8				get_lot;				/* 03 - 0 : Normal,	1 : Inverted */
	UINT8				put_lot;				/* 04 - 0 : Normal,	1 : Inverted */
	UINT8				mark1_pattern;
	UINT8				mark2_pattern;
	UINT8				led_duty_cycle;			/* 05 - Led Duty Cycle (0 ~ 90%)								*/
	UINT8				step_size;				/* 09 - Step Size (1 ~ 4)										*/
	UINT8				u8_reserved[1];

	UINT16				frame_rate;				/* 06 - Frame Rate (1 ~ 950)									*/
	UINT16				u16_reserved[3];

	UINT32				material_thick;			/* 04 - Exposure Material Thickness (unit: 100 nm or 0.1 um)	*/
	UINT32				u32_reserved;

	FLOAT				expo_energy_avg;		/* 전체 광학계의 평균 광량 (unit: mJ)							*/
	FLOAT				expo_speed;				/* 노광 속도 (unit: mm/sec)										*/
	FLOAT				rotation;
	FLOAT				scale_x;
	FLOAT				scale_y;
	FLOAT				offset_x;
	FLOAT				offset_y;
	FLOAT				led_365nm;
	FLOAT				led_385nm;
	FLOAT				led_390nm;
	FLOAT				led_405nm;
	FLOAT				f_reserved;

	PCHAR				recipe_name;			/* 01 - Recipe Name												*/
	PCHAR				gerber_name;			/* 02 - Gerber Name (Only Gerber Name, Path is not included)	*/
	PUINT16				*led_power_index;		/* 07 - Led Power Amplitude (5 set 4 led)						*/
	PFLOAT				*led_power_watt;		/* 08 - Led Power Amplitude (5 set 4 led)						*/
	PFLOAT				expo_energy;			/* 노광 에너지 (unit: mJ)										*/

	/*
	 desc : 메모리 할당
	 parm : None
	 retn : None
	*/
	VOID Init()
	{
		UINT16 i;
		material_thick		= 0;
		expo_energy_avg		= 0.0f;
		expo_speed			= 0.0f;	/* mm/sec */
		
		/* PCHAR */
		recipe_name			= new CHAR[MAX_RECIPE_NAME_LEN];//(PCHAR)::Alloc(MAX_RECIPE_NAME_LEN);
		gerber_name =			new CHAR[MAX_GERBER_NAME];//(PCHAR)::Alloc(MAX_GERBER_NAME);
		memset(recipe_name, 0x00, MAX_RECIPE_NAME_LEN);
		memset(gerber_name, 0x00, MAX_GERBER_NAME);

		/* PFLOAT */
		expo_energy			= (PFLOAT)::Alloc(sizeof(FLOAT) * MAX_PH);
		memset(expo_energy, 0x00, sizeof(FLOAT) * MAX_PH);

		/* PUINT16 (이중 배열 할당인 경우, 첫 번째 배열은 포인터 크기로 할당 */

		led_power_index = new PUINT16[MAX_PH]; //(PUINT16 *)::Alloc(sizeof(PUINT16) * MAX_PH);

		
		for (i=0; i<MAX_PH; i++)
		{
			led_power_index[i] = new UINT16[MAX_LED];//(PUINT16)::Alloc(sizeof(UINT16) * MAX_PH * MAX_LED);
			memset(led_power_index[i], 0x00, sizeof(UINT16) * MAX_LED);

			/* 각 헤드 마다 4개씩 LED 값 저장하기 위한 포인터 연결 */
		//	led_power_index[i]	= led_power_index[i-1] + 4;
		}

		/* PUINT16 (이중 배열 할당인 경우, 첫 번째 배열은 포인터 크기로 할당 */
		led_power_watt = new PFLOAT[MAX_PH]; //(PFLOAT *)::Alloc(sizeof(PFLOAT) * MAX_PH);

		
		for (i=0; i<MAX_PH; i++)
		{
			led_power_watt[i] = new FLOAT[MAX_LED];//(PFLOAT)::Alloc(sizeof(FLOAT) * MAX_PH * MAX_LED);
			memset(led_power_watt[i], 0x00, sizeof(FLOAT) * MAX_LED);
			/* 각 헤드 마다 4개씩 LED 값 저장하기 위한 포인터 연결 */
			//led_power_watt[i]	= led_power_watt[i-1] + 4;
		}
	}

	/*
	 desc : 메모리 해제
	 parm : None
	 retn : None
	*/
	VOID Close()
	{
		if (recipe_name)	delete recipe_name;
		if (gerber_name)	delete gerber_name;
		if (expo_energy)	delete expo_energy;

		if (led_power_index)
		{
			for (int i = 0; i < MAX_PH; i++)
			{
				delete led_power_index[i];
			}
			delete[] led_power_index;
			//if (led_power_index[0])	::Free([0]);
			//::Free(led_power_index);
		}
		if (led_power_watt)
		{

			for (int i = 0; i < MAX_PH; i++)
			{
				delete led_power_watt[i];
			}
			delete[] led_power_watt;
		}
	}

	/*
	 desc : 설정된 값이 유효한지 여부
	 parm : None
	 retn : TRUE (유효함) or FALSE (임의 값이 설정되지 않음)
	*/
	BOOL IsValid()
	{
		return (strlen(recipe_name) > 0 && strlen(gerber_name) > 0 &&
				material_thick > 0.0f && expo_energy > 0);
	}

}	STG_RIGP,	*LPG_RIGP;

#pragma pack (pop)	/* 8 bytes boundary sort */
