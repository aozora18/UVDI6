
/*
 desc : Structure
*/

#pragma once

#include "global.h"
#include "conf_comn.h"

//#define RECIPE_NAME_LENGTH						128
#define RECIPE_NAME_LENGTH						512
#define MAX_LOT_DATE_LENGTH						16
#define MAX_GLOBAL_MARK_DIST_CHECK_COUNT		6			/* Align 인식 후 Global Mark 간의 최대 허용 오차 한계 측정(검사) 개수 */
#define LED_POWER_NAME_LENGTH					128
#define MAX_EXPOSE_THICK_SIZE					12.0f		/* DI 노광기가 지원하는 소재 최대 두께 값 (단위: mm) */
#define MAX_REGIST_RECIPE_COUNT					256			/* 최대 등록 가능한 Recipe 기본 정보 개수  */
#define MAX_CORRECT_Y_COUNT						200			/* Stripe 간의 단차 등록할 최대 개수 (Maching Config -> YcorrectionTable) */

/* --------------------------------------------------------------------------------------------- */
/*                                             열거형                                            */
/* --------------------------------------------------------------------------------------------- */


/* Led Power Information for Photohead */
typedef struct __st_photohead_led_power_info__
{
	PCHAR	led_name;		/* Led Power Name (Id) */
	PUINT16	*led_index;		/* Max 8 ea / each 4 led */
	PFLOAT	*led_watt;		/* Max 8 ea / each 4 led * (unit : mW) /

	/*
	 desc : 메모리 할당
	 parm : None
	 retn : None
	*/
	VOID Init()
	{
		UINT16 i;
		/* PCHAR */
		led_name = new CHAR[LED_POWER_NAME_LENGTH];//   (PCHAR)Alloc(LED_POWER_NAME_LENGTH);
		memset(led_name, 0x00, LED_POWER_NAME_LENGTH);
		/* PUINT16 (이중 배열 할당인 경우, 첫 번째 배열은 포인터 크기로 할당 */
		
		led_index = new PUINT16[MAX_PH];//  (PUINT16*)Alloc(sizeof(PUINT16) * MAX_PH);
		for (int i = 0; i < MAX_PH; i++)
		{
			led_index[i] = new UINT16[4]; // (PUINT16)Alloc(sizeof(UINT16) * MAX_PH * 4);
			memset(led_index[i], 0x00, sizeof(UINT16) * 4);
		}
		//for (i=1; i<MAX_PH; i++)
		//{
		//	/* 각 헤드 마다 4개씩 LED 값 저장하기 위한 포인터 연결 */
		//	led_index[i]	= led_index[i-1] + 4;
		//}
		/* PUINT16 (이중 배열 할당인 경우, 첫 번째 배열은 포인터 크기로 할당 */
		led_watt = new PFLOAT[MAX_PH];// (PFLOAT*)Alloc(sizeof(PFLOAT) * MAX_PH)
		for (i=0; i<MAX_PH; i++)
		{
			led_watt[i] = new FLOAT[4];// (PFLOAT)Alloc(sizeof(FLOAT) * MAX_PH * 4);
			memset(led_watt[i], 0x00, sizeof(FLOAT) * 4);
			/* 각 헤드 마다 4개씩 LED 값 저장하기 위한 포인터 연결 */
			//led_watt[i]	= led_watt[i-1] + 4;
		}
	}

	/*
	 desc : 메모리 해제
	 parm : None
	 retn : None
	*/
	VOID Close()
	{
		if (led_name)	delete led_name;
		if (led_index)
		{
			for (int i = 0; i < MAX_PH; i++)
				delete led_index[i];

			delete []led_index;
		}
		if (led_watt)
		{
			for (int i = 0; i < MAX_PH;i++)
				delete led_watt[i];
			
			delete[] led_watt;
		}
	}

}	STG_PLPI,	*LPG_PLPI;