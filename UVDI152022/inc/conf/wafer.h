
/*
 desc : System Configuration Information
*/

#pragma once

#include "define.h"


/* --------------------------------------------------------------------------------------------- */
/*                                            ��� ����                                          */
/* --------------------------------------------------------------------------------------------- */

#define ALIGNER_FOUP_NAME	256

/* --------------------------------------------------------------------------------------------- */
/*                                              enum                                             */
/* --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- */
/*                                       ����(Global) ����ü                                     */
/* --------------------------------------------------------------------------------------------- */

#pragma pack(push, 8)

typedef struct __st_config_wafer_aligner_info__
{
	TCHAR				foup_name[ALIGNER_FOUP_NAME];	/* In Aligner Foup Name	*/
	UINT8				foup_num;						/* Bsa ī�޶� �̵��� ������ Interlock Y Position (Interlock P1 ~ P4)	*/
	UINT8				lot_num;						/* STAGE X �� TABLE�� MIN POSITION	*/
	UINT8				get_mode;
	UINT8				put_mode;
	UINT8				u8_reserved[4];
	
}	STG_CWAI,	*LPG_CWAI;

/* Wafer Tracking Information */
typedef struct __st_config_wafer_tracking_information__
{
	STG_CWAI			aligner;
	STG_CWAI			loader;
	STG_CWAI			handler;
	STG_CWAI			buffer;

}	STG_CWTI,	*LPG_CWTI;


#pragma pack (pop)	/* 8 bytes */