
/*
 desc : PreAligner and Robot for Logosol with serial
*/

#pragma once


/* --------------------------------------------------------------------------------------------- */
/*                                           상수 정의                                           */
/* --------------------------------------------------------------------------------------------- */

#define MAX_EFU_COUNT							4			/* EFU 최대 설치 개수 */
#define MAX_CMD_EFU_RECV						128			/* 파라미터가 포함된 수신 명령어 최대 크기 */
#define EFU_PKT_STX								0x02
#define EFU_PKT_ETX								0x03
#define EFU_LV32_ID								0x01|0x80	/* (1 ~ 32)|0x80 */
#define EFU_DPU_ID								0x9F

/* --------------------------------------------------------------------------------------------- */
/*                                       열거형 (for common)                                     */
/* --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- */
/*                                       열거형 (for efu)                                        */
/* --------------------------------------------------------------------------------------------- */

/* return value types for BL500 */
typedef enum class __en_shinsung_efu_bl500_types__ : UINT8
{
	en_pv		= 0x01,	/* Processing Value */
	en_alarm	= 0x02,	/* Alarm Code */
	en_sv		= 0x03,	/* Setting Value */

}	ENG_SEBT;


/* --------------------------------------------------------------------------------------------- */
/*                                        구조체 (for efu)                                       */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 8)

/* BL500 (EFU) 속도 설정 - 가장 최근에 설정한 정보 및 응답 */
typedef struct __st_efu_bl500_speed_set__
{
	UINT8				bl_start;	/* BL500 ID (Start) */
	UINT8				bl_end;		/* BL500 ID (End) */
	UINT8				flag_ok;	/* 설정 성공 여부 (0x00 - Not define (Waiting...), 0x01-Succeeded, 0x02-Failed */
	UINT8				speed;		/* 0 ~ 140. 1 -> 10 rpm */
	UINT8				reserved[4];

}	STG_EBSS,	*LPG_EBSS;

/* BL500 (EFU) 상태 정보 */
typedef struct __st_efu_pv_alarm_sv__
{
	UINT8				bl_id;		/* BL500 ID (1 ~ 32) */
	UINT8				pv;			/* Process Value (ten's digit)*/
	UINT8				alarm;		/* Alarm Value */
	UINT8				sv;			/* Setting Value (ten's digit) */
	UINT8				reserved[4];

}	STG_EPAS,	*LPG_EPAS;

#pragma pack (pop)	/* 8 bytes order */