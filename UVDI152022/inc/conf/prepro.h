
/*
 desc : Structure
*/

#pragma once

#include "global.h"
#include "config.h"

#define RECIPE_NAME_LENGTH						128
#define RECIPE_ATTR_LENGTH						128													/* �������� �� �Ӽ� ���� ������ ����� ���� �̸� (��� ������) */
#define MAX_LOT_NUM_LENGTH						128
#define MAX_GLOBAL_MARK_DIFF_CHECK_COUNT		6													/* Align �ν� �� Global Mark ���� �ִ� ��� ���� �Ѱ� ����(�˻�) ���� */
#define MAX_RECIPE_GERBER_COUNT					5													/* ������ �� ����� �� �ִ� �Ź� �ִ� ���� */
#define LED_POWER_NAME_LENGTH					128
#define MAX_EXPOSE_THICK_SIZE					12.0f												/* DI �뱤�Ⱑ �����ϴ� ���� �ִ� �β� �� (����: mm) */
#define MAX_REGIST_RECIPE_COUNT					256													/* �ִ� ��� ������ Recipe �⺻ ���� ����  */
#define MAX_REGIST_ATTRIB_COUNT					32													/* �ִ� ��� ������ Recipe �Ӽ� ���� ����  */

/* --------------------------------------------------------------------------------------------- */
/*                                             ������                                            */
/* --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- */
/*                                             ����ü                                            */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 8)

/* The recipe base information for gerber file */
typedef struct __st_prepro_rlt_setting_file__
{	
	/* Tab advanced */
	UINT16				advanced_spx;			/* 1, 4, 9, 16, 25, 36. */
	UINT16				advanced_Thread;	/* specify number of CPU threads.Default = number of logical cores on the machines CPU. */
	BOOL				advanced_warnning;
	UINT16				advanced_threadhold;
	UINT16				advanced_retrycount; /* val - specify number of retries to clean/fix gerber for shape compensation. Default=0. */
 	BOOL				advanced_shpCompens; 

	/* Tab Image Input */ 
	INT32				image_input_rotation;			/* valid values: 0, 90, 180, 270. */
	FLOAT				image_input_erox;
	FLOAT				image_input_eroy;
	BOOL				image_input_Inverse;
	BOOL				image_input_inverse_frame;
	BOOL				image_input_fliph;
	BOOL				image_input_flipv;
	BOOL				image_input_negydir;
	INT32				image_input_globalfid;
	INT32				image_input_localfid;
	BOOL				image_input_prev;
	FLOAT				image_input_offsetx;
	FLOAT				image_input_offsety;
	FLOAT				image_input_winllx;
	FLOAT				image_input_winlly;
	FLOAT				image_input_winurx;
	FLOAT				image_input_winury;
 	UINT8				image_input_bmpfrmcheck;

	/* Head Input */
	BOOL		head_input_hdip_rotate;
	UINT8		head_input_hdip_photohd;
	INT32		head_input_hdip_hdpitch;
	FLOAT		head_input_hdip_phdisthd[8];
	FLOAT		m_combo_doub_HDip_Lensmag;
	
	FLOAT		NominalPHDist2;
	FLOAT		NominalPHDist3;
	FLOAT		NominalPHDist4;
	FLOAT		NominalPHDist5;
	FLOAT		NominalPHDist6;
	FLOAT		NominalPHDist7;
	FLOAT		NominalPHDist8;

	/* main tab */
 	PCHAR				main_gerber_input_path;			/* Gerber Input Path										*/
 	PCHAR				main_gerber_output_path;		/* Gerber Input Path										*/
 	PCHAR				main_work_dir_path;				/* Preprocessing Work Path									*/
 	PCHAR				main_suffix;

	/*
	 desc : �޸� �Ҵ�
	 parm : None
	 retn : None
	*/
	VOID Init()
	{
		main_gerber_input_path = (PCHAR)Alloc(MAX_PATH_LEN);
		memset(main_gerber_input_path, 0x00, MAX_PATH_LEN);
		main_gerber_output_path = (PCHAR)Alloc(MAX_PATH_LEN);
		memset(main_gerber_output_path, 0x00, MAX_PATH_LEN);
		main_work_dir_path = (PCHAR)Alloc(MAX_PATH_LEN);
		memset(main_work_dir_path, 0x00, MAX_PATH_LEN);
		main_suffix = (PCHAR)Alloc(MAX_FILE_LEN);
		memset(main_suffix, 0x00, MAX_FILE_LEN);

// 		lot_number			= (PCHAR)Alloc(MAX_LOT_NUM_LENGTH);
// 		memset(lot_number, 0x00, MAX_LOT_NUM_LENGTH);
// 		attrib_name			= (PCHAR)Alloc(RECIPE_ATTR_LENGTH);
// 		memset(attrib_name, 0x00, RECIPE_ATTR_LENGTH);
// 		gerber_name			= (PCHAR *)Alloc(sizeof(PCHAR) * MAX_RECIPE_GERBER_COUNT);
// 		gerber_name[0]		= (PCHAR)Alloc(MAX_GERBER_NAME * sizeof (CHAR) * MAX_RECIPE_GERBER_COUNT);
// 		memset(gerber_name[0], 0x00, MAX_GERBER_NAME * sizeof (CHAR) * MAX_RECIPE_GERBER_COUNT);
// 		for (i=1; i<MAX_RECIPE_GERBER_COUNT; i++)	gerber_name[i]	= gerber_name[i-1] + MAX_GERBER_NAME;
// 		text_string			= (PCHAR)Alloc(MAX_PANEL_TEXT_STRING);
// 		memset(text_string, 0x00, MAX_PANEL_TEXT_STRING);
// 		/* Align �ν� �� Gloab Mark ���� �ִ� ��� ���� �� ���� */
// 		global_mark_diff	= (PUINT32)Alloc(sizeof(UINT32) * MAX_GLOBAL_MARK_DIFF_CHECK_COUNT * MAX_RECIPE_GERBER_COUNT);
// 		memset(global_mark_diff, 0x00, sizeof(UINT32) * MAX_GLOBAL_MARK_DIFF_CHECK_COUNT * MAX_RECIPE_GERBER_COUNT);
	}

	/*
	 desc : �޸� ����
	 parm : None
	 retn : None
	*/
	VOID Close()
	{
 		if (main_gerber_input_path)			Free(main_gerber_input_path);
 		if (main_gerber_output_path)		Free(main_gerber_output_path);
 		if (main_work_dir_path)				Free(main_work_dir_path);
 		if (main_suffix)					Free(main_suffix);
	}

	/*
	 desc : Gerber Name �ʱ�ȭ
	 parm : None
	 retn : None
	*/
	VOID ResetMemData()
	{
		memset(main_gerber_input_path,		0x00, MAX_PATH_LEN);
		memset(main_gerber_output_path,		0x00, MAX_PATH_LEN);
		memset(main_work_dir_path,			0x00, MAX_PATH_LEN);
		memset(main_suffix,					0x00, MAX_FILE_LEN);
	}

	/*
	 desc : ������ ���� ��ȿ���� ����
	 parm : None
	 retn : TRUE (��ȿ��) or FALSE (���� ���� �������� ����)
	*/
	BOOL IsValid()
	{

 		return (strlen(main_gerber_input_path) > 0 && strlen(main_gerber_output_path) > 0 &&
 				strlen(main_work_dir_path) > 0);
	}

	/*
	 desc : ��ũ �����Ͱ� �����ϴ��� ����
	 parm : count	- [in]  �����ǿ� ��ϵ� �Ź� ����
	 retn : TRUE or FALSE
	*/
	BOOL IsMarkExist(UINT8 count)
	{
// 		UINT8 i	= 0x00;
// 		
// 		if (!global_mark_diff)	return FALSE;
// 		for (; i<UINT32(MAX_GLOBAL_MARK_DIFF_CHECK_COUNT) * UINT32(count); i++)
// 		{
// 			if (global_mark_diff[i])	return TRUE;
// 		}
// 		return FALSE;
	}

}	STG_PRSF,	*LPG_PRSF;
/* Function Return Code ���� �� (8 bits ��, 0x0 ~ 0xff�� ����) */
typedef enum __en_function_return_code_kind__
{
	// Fail Code
	en_frck_fail = 0x10,		/* �з� �ڵ� */
	en_frck_fail_work = 0x11,		/* �Ϲ� �۾� */
	en_frck_fail_sock = 0x12,		/* ��� �۾� */
	en_frck_fail_code = 0x13,		/* ���� �ڵ� */
	en_frck_fail_device = 0x14,		/* ��� ���� */
	en_frck_fail_send = 0x15,		/* �۽� ���� */
	en_frck_fail_recv = 0x16,		/* ���� ���� */
	en_frck_fail_axis = 0x17,		/* ���� ���� */
	en_frck_fail_expo = 0x18,		/* �뱤 ���� */

	// Noti Code
	en_frck_noti = 0x20,		/* �з� �ڵ� */
	en_frck_noti_process = 0x21,		/* �뱤 ��Ȳ */
	en_frck_noti_trig = 0x22,		/* Ʈ���� �� */
	en_frck_noti_mem_change = 0x23,		/* �޸� �� */
	en_frck_noti_mesg = 0x24,		/* �Ϲݸ޽���*/

	// Alarm Code
	en_frck_alarm = 0xd0,		/* �з� �ڵ� */
	en_frck_alarm_warn = 0xd1,		/* ��� �˸� */
	en_frck_alarm_alarm = 0xd2,		/* �˶� �˸� */
	en_frck_alarm_fault = 0xd3,		/* ���� �˸� */

	// Socket Code
	en_frck_sock = 0xe0,		/* �з� �ڵ� */
	en_frck_sock_connecting = 0xe1,		/* ���� ��� */
	en_frck_sock_listening = 0xe2,		/* ���� ���� */
	en_frck_sock_connected = 0xe3,		/* ���� ���� */
	en_frck_sock_accepted = 0xe4,		/* ���� ��� */
	en_frck_sock_closed = 0xe5,		/* ���� ���� */
	en_frck_sock_ip_invalid = 0xe6,		/* �ּ� ���� */
	en_frck_sock_ip_exist = 0xe7,		/* �ּ� �ߺ� */
	en_frck_sock_sended = 0xe8,		/* �۽� �Ϸ� */
	en_frck_sock_recved = 0xe9,		/* ���� �Ϸ� */
	en_frck_sock_binding = 0xea,		/* ���� ��� */
	en_frck_sock_livecheck = 0xeb,		/* ���� Ȯ�� */
	en_frck_sock_text_mesg = 0xec,		/* ���� ���� */

	// Unknown Code
	en_frck_none = 0xf0,		/* �˼� ���� */

}	ENG_FRCK;

/* Auto Run Data */
typedef struct __st_prepro_auto_run_data__
{
	/* main tab */
	PCHAR				gds_file_path;			/* Gerber Input Path										*/
	//TCHAR				gds_file_path[MAX_PATH_LEN];			/* Gds Input Path

	/*
	 desc : �޸� �Ҵ�
	 parm : None
	 retn : None
	*/
	VOID Init()
	{
		gds_file_path = (PCHAR)Alloc(MAX_PATH_LEN);
		memset(gds_file_path, 0x00, MAX_PATH_LEN);
	}
	/*
	 desc : �޸� ����
	 parm : None
	 retn : None
	*/
	VOID Close()
	{
		if (gds_file_path)			Free(gds_file_path);
	}
	/*
	 desc : gds file name �ʱ�ȭ
	 parm : None
	 retn : None
	*/
	VOID ResetMemData()
	{
		memset(gds_file_path, 0x00, MAX_PATH_LEN);
	}
}	STG_PARN, *LPG_PARN;
#pragma pack (pop)	/* 8 bytes boundary sort */