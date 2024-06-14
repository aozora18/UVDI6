
/*
 desc : System Configuration Information
*/

#pragma once

#include "define.h"

/* --------------------------------------------------------------------------------------------- */
/*                                           ��� ��                                             */
/* !!! �߿� !!!  �Ʒ� ��� ���� �ݵ�� 8�� ��� �̾�� ��. ���� : ����ü ��輱 �޸� ���� ���� */
/* --------------------------------------------------------------------------------------------- */

#define	USE_EQUIPMENT_SIMULATION_MODE	0		/* Simulation Mode�� �������� ���� */

#define	MAX_PH							8		/* ������� �ִ� ���� */
#define	MAX_LED							8		/* ������� ���� ��ġ�� LED ���� (�����δ� 0x00 ~ 0x05) */
#define MAX_LED_POWER					4095	/* 0 ~ 4095 */
#define	MAX_TABLE						8		/* �뱤 �۾��� ������ �� �ִ� Work Table (Stage) �ִ� ���� */
#define MAX_X_CORRECTION_TABLE_ENTRY	200		/* X �� ���� ������ �ִ� ���� */
#define MAX_Y_CORRECTION_TABLE_ENTRY	200		/* Y �� ���� ������ �ִ� ���� */
#define	MAX_SCROLL_MODE					8		/* ��ũ�� ��� ���� (1 ~ 6) */
#define	MAX_SCROLL_RATE					19000	/* �ִ� 19000 Hz */
#define	MAX_REGIST_JOB_LIST				8		/* �ִ� ��� ������ JOB ���� */
#define	MAX_HASH_VALUE_COUNT			64		/* �ִ� Hash value ���� ���� */
#define	MAX_DCODE_LIST					400		/* �Ź� ���� ��� ������ �ִ� D-Code ���� */
#define	MAX_COORD_XY					400		/* Location�� ���� Coordination �ִ� ���� */
//#define	MAX_GERBER_NAME					200		/* �ִ� Job Path Name ���� */
#define	MAX_GERBER_NAME					512		/* �ִ� Job Path Name ���� */
#define	MAX_WARP_COORD_XY				400		/* �ִ� Warp Coordinate X/Y ���� */
#define	MAX_PANEL_SERIAL_STRING			32		/* �ø��� ���ڿ� �ִ� ���� */
#define	MAX_PANEL_TEXT_STRING			32		/* �ؽ�Ʈ ���ڿ� �ִ� ���� */
#define	MAX_LOCAL_FIXED_COUNT			200		/* Local Fixed Rotaton / Scaling / Offset */
#define	MAX_SSD_SMART_DATA				16		/* 14 ea. Fixed */

#define	MAX_PANEL_STRING_COUNT			8		/* �� �뱤 �鿡 ǥ���Ǵ� Panel D-Code ���� */

#define	MAX_MC2_DRIVE					8		/* MC2�� ��ġ ������ �ִ� ����̺� ���� */
#define MAX_ALIGN_CAMERA				8		/* ��ġ ������ ����� ī�޶� �ִ� ���� (����� 2������, �� ���� 8�� ������ ����. ����ü ���� ������. !!!) */
#define MAX_SCAN_MARK_COUNT				16		/* Local Mark �� �� ��ĵ�� ��, �ִ� �˻��� �� �ִ� ��ũ ���� */

#define	WORK_NAME_LEN					128
#define	STEP_NAME_LEN					128
#define	MAX_GLOBAL_MARKS				4		/* Global Mark �ִ� ���� */
#define	MAX_LOCAL_MARKS					1280		/* �ִ� Local Mark (Fiducial) ���� */
#define	MAX_REGISTRATION_POINTS			(MAX_GLOBAL_MARKS + MAX_LOCAL_MARKS)	/* ����� ī�޶� ���� �˻�� �ִ� ��ũ ��� ���� */
#define	MAX_SELECT_VELO				4		/* MC2 jog�̵��� ���ǵ� ���� ���� */

/* --------------------------------------------------------------------------------------------- */
/*                                           ��� ��                                             */
/* !!!!!!!!! �߿� !!!  �Ʒ� ��� ���� �ݵ�� ���� �� ����ü ��輱 �����ϸ� ���� �ȵ�  !!!!!!!!! */
/* --------------------------------------------------------------------------------------------- */
#define MAX_TRIG_CHANNEL				4		/* Trigger ��ġ�� ��ġ ������ �ִ� Channel ���� */

/* �ִ� ��� ������ Photohead Step ���� (�����ǿ� ��� ������ ����) */
#define MAX_PH_STEP_LINES				255	/* 1 ����Ʈ�� ȯ�� (����ü) ���Ͽ� ���� �Ǿ� �����Ƿ� */
/* �ִ� ��� ������ Photohead�� Stripe ���� ���� */
#define MAX_PH_CORRECT_Y				255	/* 1 ����Ʈ�� ȯ�� (����ü) ���Ͽ� ���� �Ǿ� �����Ƿ� */

/* --------------------------------------------------------------------------------------------- */
/*                                             ���ڿ�                                            */
/* --------------------------------------------------------------------------------------------- */

/* ����ȭ ��ü */
#define	MTX_PROC_PLC_DATA				L"MTX_BRainX1_PROC_PLC_DATA"

/* --------------------------------------------------------------------------------------------- */
/*                                             ������                                            */
/* --------------------------------------------------------------------------------------------- */

/* System Error Code �з� �� (Code.str ���� �˻� �뵵) */
typedef enum class __en_system_code_to_string__ : UINT8
{
	en_mc2_error		= 0x11,
	en_luria_reg		= 0x21,
	en_luria_sys		= 0x22,
	en_luria_ph			= 0x23,
	en_luria_sts_1		= 0x24,
	en_luria_sts_2		= 0x25,
	en_melsec_axis_err	= 0x31,

}	ENG_SCTS;

//����� ���
typedef enum class __en_align_motion__ : UINT16
{
	none = 0,
	en_onthefly_2cam	= 0b0000001000000001, //�´��ö��� ���̵�ķ
	en_onthefly_3cam	= 0b0000001100000001, //�´��ö��� ����ķ
	en_static_2cam      = 0b0000001000000010, //����ƽ ���̵�ķ
	en_static_3cam		= 0b0000001100000010, //����ƽ ����ķ
}	ENG_AMOS;

/* Align Mark Type */
typedef enum class __en_algin_type_global_local__ : UINT8
{
	/* Zero mark */
	en_global_0_local_0x0_n_point = 0x00,
	en_global_4_local_0_point = 0x04,
	en_global_4_local_n_point = 0x05,
	en_not_defined = 0xff,

}	ENG_ATGL;
//
//typedef enum class __en_algin_type_global_local__ : UINT8
//{
//	/* Zero mark */
//	en_global_0_local_0x0_n_point = 0x00,		/* Global (0) points / Local Division (0 x 0) (00) points */
//	/* Only 2 mark */
//	en_global_2_local_0x0_n_point = 0x02,		/* Global (2) points / Local Division (0 x 0) (00) points */
//	/* Only 3 mark */
//	en_global_3_local_0x0_n_point = 0x03,		/* Global (3) points / Local Division (0 x 0) (00) points */
//	/* Only 4 mark */
//	en_global_4_local_0x0_n_point = 0x04,		/* Global (4) points / Local Division (0 x 0) (00) points */
//
//	/* None shared */
//	en_global_4_local_2x1_n_point = 0x11,		/* Global (4) points / Local Division (2 x 1) (08) points */
//
//	en_global_4_local_2x2_n_point = 0x21,		/* Global (4) points / Local Division (2 x 2) (16) points */
//	en_global_4_local_3x2_n_point = 0x22,		/* Global (4) points / Local Division (3 x 2) (24) points */
//	en_global_4_local_4x2_n_point = 0x23,		/* Global (4) points / Local Division (4 x 2) (32) points */
//	en_global_4_local_5x2_n_point = 0x24,		/* Global (4) points / Local Division (5 x 2) (40) points */
//
//	/* Shared */
//	en_global_4_local_2x2_s_point = 0x31,		/* Global (4) points / Local Division (2 x 2) (13) points */
//	en_global_4_local_3x2_s_point = 0x32,		/* Global (4) points / Local Division (3 x 2) (16) points */
//	en_global_4_local_4x2_s_point = 0x33,		/* Global (4) points / Local Division (4 x 2) (19) points */
//	en_global_4_local_5x2_s_point = 0x34,		/* Global (4) points / Local Division (5 x 2) (22) points */
//
//	/* Not defined */
//	en_not_defined = 0xff,
//
//}	ENG_ATGL;

/* Job Work Next State */
typedef enum class __en_job_work_next_state__ : UINT8
{
	en_none							= 0x00,		/* Not defined */
	en_init							= 0x01,		/* �ʱ� ���� */
	en_wait							= 0x02,		/* ���� ��� */
	en_next							= 0x03,		/* ���� ���� */
	en_comp							= 0x04,		/* �۾� �Ϸ� */
	en_forceSet						= 0x05,		/* ���Ƿ� ���ܺ��� */
	en_time							= 0xfe,		/* Ÿ�� �ƿ� */
	en_error						= 0xff,		/* ���� �߻� */
	
}	ENG_JWNS;

/* Engine Running Mode (�� 31�� �Ѿ�� �ȵ�) */
typedef enum class __en_engine_runnging_vision_mode__ : UINT8
{
	en_monitoring					= 0x00,	/* Engine Monitoring */
	en_cmps_sw						= 0x10,	/* for Control App. */
	/* for Calibration */
	en_cali_step					= 0x21,	/* Calibration S/W - The step (error) xy of optic. */
	en_cali_vdof					= 0x22,	/* Calibration S/W - The focus of optic. and camera */
	en_cali_meas					= 0x23,	/* Calibration S/W - ����� ī�޶� ���� �� ���� ���� �۾� */
	en_cali_exam					= 0x24,	/* Calibration S/W - ����� ī�޶� ���� �� ���� ���� �۾� */
	en_acam_spec					= 0x25,	/* Calibration S/W - ����� ī�޶� ��� (SPEC) ���� �۾� */
	en_led_power					= 0x26,	/* Calibration S/W - Measure and collect the illuminance of the photohead */
	en_stage_st						= 0x27,	/* Calibration S/W - The straightness of Stage XY */
	en_hole_size					= 0x28,	/* Calibration S/W - Measure the size of marker's hole */

}	ENG_ERVM;

/* ����� �뱤 ���� ��� (Direct Expose, Align Expose, Align & Calibration Expose */
typedef enum class __en_align_operation_expose_mode__ : UINT8
{
	en_direct_expose				= 0x00,	/* ���� �뱤 */
	en_align_expose					= 0x01,	/* ����� �뱤 */
	en_calib_expose					= 0x02,	/* ����� �����Ͽ� ���� �뱤 */

}	ENG_AOEM;

/* Vision Camera Product Kind */
typedef enum class __en_vision_camera_product_kind__ : UINT8
{
	en_camera_none					= 0x00,	/* Not used */
	en_camera_basler_ipv4			= 0x01,	/* for Basler (Ethernet Type) */
	en_camera_ids_ph				= 0x02,	/* for IDS (PH Included) */

}	ENG_VCPK;

/* Vision Library Product Kind */
typedef enum class __en_vision_library_product_kind__ : UINT8
{
	en_vision_none					= 0x00,	/* Not used */
	en_vision_opencv				= 0x01,	/* for OpenCV */
	en_vision_mil					= 0x02,	/* for MIL */
	en_vision_cognex				= 0x03,	/* for Cognex */
	en_vision_halcon				= 0x04,	/* for Halcon */

}	ENG_VLPK;

/* Global Mark 4���� ���� 6 ������ ��ġ ���� */
typedef enum class __en_global_mark_dist_direct__ : UINT8
{
	en_top_horz						= 0x00,	/* �簢�� -> ���� -> ���� (1 <-> 3 ��ũ ���� ����) */
	en_btm_horz						= 0x01,	/* �簢�� -> �Ʒ� -> ���� (2 <-> 4 ��ũ ���� ����) */
	en_lft_vert						= 0x02,	/* �簢�� -> ���� -> ������ (1 <-> 2 ��ũ ���� ����) */
	en_rgt_vert						= 0x03,	/* �簢�� -> ���� -> ������ (3 <-> 4 ��ũ ���� ����) */
	en_lft_diag						= 0x04,	/* �簢�� -> ���� -> �밢�� (1 <-> 4 ��ũ ���� ����) */
	en_rgt_diag						= 0x05,	/* �簢�� -> ���� -> �밢�� (2 <-> 3 ��ũ ���� ����) */

}	ENG_GMDD;

/* ����̽� �̵� ���� (Up, Down, left, right) */
typedef enum class __en_mc2_device_move_direct__ : UINT8
{
	en_none							= 0x00,

	en_move_up						= 0x01,
	en_move_down					= 0x02,

	en_move_left					= 0x03,
	en_move_right					= 0x04,

	en_move_homed_x					= 0x05,
	en_move_homed_y					= 0x06,

	en_pos_align_start				= 0x07,		/* ���������� Align ���� ��ġ�� �̵� */
	en_pos_expo_start				= 0x08,		/* ���������� �뱤 ���� ��ġ�� �̵� */

	en_move_axis_up					= 0x10,
	en_move_axis_down				= 0x11,

}	ENG_MDMD;

/* MC2 Motion Drive ID ��, SD2S Drive ID */
typedef enum class __en_mc2_motion_drive_id__ : UINT8
{
	en_stage_x						= 0x00,
	en_stage_y						= 0x01,
	//en_stage_y						= 0x02,


#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
	en_align_cam1					= 0x04,
	en_align_cam2					= 0x05,

	en_axis_ph1						= 0x06,
	en_axis_ph2						= 0x07,
	en_axis_ph3						= 0x08,
	en_axis_ph4						= 0x09,
	en_axis_ph5						= 0x10,
	en_axis_ph6						= 0x11,

	en_axis_acam1					= 0x12,
	en_axis_acam2					= 0x13,

#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	en_align_cam1					= 0x04,
	en_align_cam2					= 0x05,

	en_axis_ph1						= 0x06,
	en_axis_ph2						= 0x07,

	en_axis_acam1					= 0x12,
	en_axis_acam2					= 0x13,
#endif

	en_axis_none					= 0xf0,
	en_all							= 0xff,

}	ENG_MMDI;

/* LED Power ID */
typedef enum class __en_luria_led_power_id__ : UINT8
{
	en_zero							= 0x00,	/* !!! You should not use this value (for use inside the engine only) */

	en_365nm						= 0x01,	/* You must start with 1 */
	en_385nm						= 0x02,
	en_395nm						= 0x03,
	en_405nm						= 0x04,

	en_all							= 0xff,

}	ENG_LLPI;

/* LED Frequency */
typedef enum class __en_luria_led_frequency_value__ : UINT16
{
	en_365nm						= 365,	/* frequency : nm */
	en_385nm						= 385,
	en_395nm						= 395,
	en_405nm						= 405,

}	ENG_LLFV;

/* Model Define �� */
typedef enum class __en_mark_model_definition_type__ : UINT32
{
	en_none							= 0x00000000L,	/* Not selected				*/
	en_circle						= 0x00000008L,	/* M_CIRCLE		: 8			*/
	en_ellipse						= 0x00000010L,	/* M_ELLIPSE	: 16		*/
	en_square						= 0x00000020L,	/* M_SQUARE		: 32		*/
	en_rectangle					= 0x00000040L,	/* M_RECTANGLE	: 64		*/
	en_ring							= 0x00000100L,	/* M_RING		: 256		*/
	en_cross						= 0x00002000L,	/* M_CROSS		: 8192		*/
	en_diamond						= 0x00008000L,	/* M_DIAMOND	: 32768		*/
	en_triangle						= 0x00010000L,	/* M_TRIANGLE	: 65536		*/
	en_image						= 0x00000004L,	/* M_IMAGE		: 4	*/
	en_merge_model					= 0x00020000L,	/* M_MERGE_MODEL: 131072	*/
}	ENG_MMDT;

/* --------------------------------------------------------------------------------------------- */
/*                                    Config ����ü - Common                                     */
/* --------------------------------------------------------------------------------------------- */

/* �ݵ�� 1 bytes ���� (Shared Memory ������ 1 bytes ���� �Ұ���) */
#pragma pack (push, 1)

/* ��� ���� ���� ���� */
typedef struct __st_device_link_shared_memory__
{
	UINT8				is_connected;		/* ��� ���� ���� */
	UINT8				u8_reserved[7];
	UINT16				link_time[6];		/* ��� ����� �ð� ���� (��-��-�� ��-��-��) */
	UINT16				u16_error_code;		/* PLC : Complete Code, Luria : Luria Status */
	UINT16				u16_reserved[1];
	INT32				last_sock_error;	/* ���� �ֱٿ� �߻��� ���� ���� */
	INT32				i32_reserved;

	/*
	 desc : ����ü ���� �ʱ�ȭ
	 parm : None
	 retn : None
	*/
	VOID ResetData()
	{
		is_connected	= 0x00;
		memset(link_time, 0x00, sizeof(UINT16) * 6);
		u16_error_code	= 0x0000;
		last_sock_error	= 0x00000000;
	}
	VOID ResetError()
	{
		u16_error_code	= 0x0000;
		last_sock_error	= 0x00000000;
	}

}	STG_DLSM,	*LPG_DLSM;

#pragma pack (pop)	/* 1 bytes */

#pragma pack (push, 8)

typedef struct __st_config_common_global_setup__
{
	UINT8				grab_mark_saved:1;				/* �˻��� ����� ��ũ ���� ����. (�˻� ���п� ��� ���� ������ ���� ����) */
														/* GUI (����SW)�� ����� �������� mark ��� ���� ������ ����� */
	UINT8				log_file_saved:1;				/* 0 : �α� ���Ϸ� ���� ����, 1 : �α� ���Ϸ� �α� ���� �� */
	UINT8				log_file_type:1;				/* 0 : �ؽ�Ʈ ��� ���Ϸ� ����, 1 : Database ���� (SQLite3)���� ���� */
	UINT8				run_emulate_mode:1;				/* 0 : ���� ���� ���� �� ����, 1 : ��� ���� ���� SW ���������� ���� */
	UINT8				run_engine_monitor:1;			/* 0: ���� ���� ���α׷� �������� ����, 1: ���� ���� ���α׷� ���� */
	UINT8				strobe_lamp_type:2;				/* ä�� �� ���� �ִ� ���� 2�� ����. ���ÿ� ����� �Ұ� (0 : ���� 1��, 1 : ���� 2��) */
	UINT8				run_material_detect:1;			/* ���� ���� ����. (0:���� ���� ���� ���� �뱤 ����, 1:���� ���� Ȯ�� �� �뱤) */
														/* ��, Align Expose�� ��츸 �ش�� */

	UINT8				align_camera_kind:4;			/* 0: None, 1 : Basler (�ܺ�), 2 : IDS (VISITECH; ����) */
	UINT8				run_develop_mode:1;				/* 0 : ���� ���� ���� �� ����, 1 : ��� ���� ���� SW ���������� ���� */
	UINT8				all_alarm_ignore:1;				/* DI ��񿡼� �߻��Ǵ� ��� �˶� ���� ���� (1:����, 0:�˸�) */
	UINT8				all_warn_ignore:1;				/* DI ��񿡼� �߻��Ǵ� ��� ��� ���� ���� (1:����, 0:�˸�) */
	UINT8				comm_log_optic:1;				/* PODIS vs. Luria		0 : ��� �α� ���� ����, 1 : ��� �α� ���� */

	UINT8				comm_log_mc2:1;					/* PODIS vs. MC2		0 : ��� �α� ���� ����, 1 : ��� �α� ���� (���� ��û�� ��츸 ����) */
	UINT8				comm_log_plc:1;					/* PODIS vs. PLC		0 : ��� �α� ���� ����, 1 : ��� �α� ���� (���� ��û�� ��츸 ����) */
	UINT8				comm_log_efu:1;					/* PODIS vs. EFU		0 : ��� �α� ���� ����, 1 : ��� �α� ���� */
	UINT8				mesg_box_optic:1;				/* Luria ���������� �߻��� ���� �޽����� �������� ��� ���� */
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
	UINT8				comm_log_milara:1;				/* PODIS vs. MILARA		0 : ��� �α� ���� ����, 1 : ��� �α� ���� (Robot & Prealigner) */
	UINT8				comm_log_pm100d:1;				/* PODIS vs. PM100D		0 : ��� �α� ���� ����, 1 : ��� �α� ���� */
	UINT8				comm_log_litho:1;				/* PODIS vs. Link.Litho	0 : ��� �α� ���� ����, 1 : ��� �α� ���� */
	UINT8				u8_bit_reserved:1;
#elif (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	   CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
	UINT8				u8_bit_reserved:4;
#endif
	UINT8				engine_mode;					/* ENG_ERVM Mode �� */
	UINT8				u8_reserved[4];

	TCHAR				gerber_path[MAX_PATH_LEN];		/* �Ź� ������ ����Ǿ� �ִ� �⺻ ��� (�Ź� ���� �����Կ� �־ ������ �����ϱ� ���� ����. Ư���� �ٸ� ������ ����� ���� ����) */
	TCHAR				preproc_dll_path[MAX_PATH_LEN];	/* PreProcess ���۰� ���õ� DLL ���ϵ��� ����Ǿ� �ִ� ��� (!!! �߿� !!!) */

}	STG_CCGS,	*LPG_CCGS;

typedef struct __st_config_system_device_monitoring__
{
	UINT8				hdd_drive;					/* HDD Drive Index (0:C, 1:D, 2:E, ...) */
	UINT8				u8_reserved[7];
	TCHAR				mon_time[24];				/* ����� ���� �ð� (0000-00-00 00:00:00)*/
	/* �Ʒ� 2�� ������ �α� �������� ���α׷� ���� �� �߻��� ���� �� ��� ���� */
	UINT16				error_count;				/* ������� �߻��� ���� ���� (�ִ� 65535) */
	UINT16				warn_count;					/* ������� �߻��� ��� ���� (�ִ� 65535) */
	UINT16				u16_reserved[2];

}	STG_CSDM,	*LPG_CSDM;

typedef struct __st_config_communication_setup_info__
{
	UINT8				cmps_ipv4[4];			/* CMPS IPv4 (Client)			*/
	UINT8				luria_ipv4[4];			/* Luria IPv4 (Server)			*/
	UINT8				mc2_ipv4[4];			/* MC2 IPv4 (Server)			*/
	UINT8				mc2b_ipv4[4];			/* MC2 IPv4 (Server)			*/
	UINT8				plc_ipv4[4];			/* PLC IPv4 (Server)			*/
	UINT8				trig_ipv4[4];			/* Trigger Board IPv4 (Server)	*/
	UINT8				efem_ipv4[4];			/* EFEM (Robot) IPv4 (Server)	*/
	UINT8				visi_ipv4[4];			/* Vision (Alpha) IPv4 (Server) */
	UINT8				philhmi_ipv4[4];		/* Philhmi IPv4 (Server)		*/	// by sysandj
	UINT8				gentec_ipv4[4];			/* Gentec IPv4 (Server)			*/	// 230517 mhbaek Add
	UINT8				strb_lamp_ipv4[4];		/* Strobe Lamp IPv4 (Server)	*/
	UINT8				keyence_lds_ipv4[4];	/* Keyence LDS IPv4 (Server)	*/	// 230925 mhbaek Add
	UINT8				u8_reserved[4];
	/* ---------------------------------------------------------------------------------------------------- */
	UINT16				luria_port;				/* CMPS (Client) �� Luria (Server) �� ����� ���� ��Ʈ ��ȣ */
	UINT16				mc2s_port;				/* UDP ��Ŷ�� CMPS->MC2 ������ �۽��ϴ� ��Ʈ ��ȣ			*/
	UINT16				mc2c_port;				/* UDP ��Ŷ�� MC2->CMPS ������ �����ϴ� ��Ʈ ��ȣ			*/
	UINT16				mc2bs_port;				/* UDP ��Ŷ�� CMPS->MC2 ������ �۽��ϴ� ��Ʈ ��ȣ			*/
	UINT16				mc2bc_port;				/* UDP ��Ŷ�� MC2->CMPS ������ �����ϴ� ��Ʈ ��ȣ			*/
	UINT16				plc_port;				/* CMPS�� Melsec PLC�� ����� ���� ��Ʈ ��ȣ				*/
	UINT16				trig_port;				/* CMPS�� Trigger Board�� ����� ���� ��Ʈ ��ȣ				*/
	UINT16				efem_port;				/* CMPS�� EFEM (Robot)�� ����� ���� ��Ʈ ��ȣ				*/
	UINT16				visi_port;				/* CMPS�� Vision (Alpha)�� ����� ���� ��Ʈ ��ȣ			*/
	UINT16				philhmi_port;			/* Philhmi ����� ���� ��Ʈ ��ȣ							*/	// by sysandj
	UINT16				gentec_port;			/* Gentec ����� ���� ��Ʈ ��ȣ								*/	// 230517 mhbaek Add
	UINT16				strb_lamp_port;			/* Strobe Lamp ����� ���� ��Ʈ ��ȣ						*/
	UINT16				keyence_lds_port;		/* Keyence LDS ����� ���� ��Ʈ ��ȣ						*/	// 230925 mhbaek Add
	UINT16				u16_port[1];
	/* ---------------------------------------------------------------------------------------------------- */
	UINT32				link_time;				/* ������ ��-������ ���� �õ��� ���� ���� (����: ms) */
												/* 0 ���̸�, �� �̻� ������ �õ� �������� ���� */
	UINT32				idle_time;				/* �� �ð����� ���� �ý������� ���� ������ ������, ������ ���� ���� ���� */
												/* ���� 0 ������ �����Ǹ�, �� ����� ������� ���� */
	UINT32				sock_time;				/* ��� ��Ʈ�κ��� �����Ͱ� ���ŵǾ����� ����ϴ� �ð� (����: ms) */
												/* ���� 0 ��. �� ���� Ŭ���� ��Ʈ�� ������ ���� �����ϴµ� �������� ��� �� */
	UINT32				live_time;				/* ���� ������ �ð����� Live Check �޽��� �۽� (����: ms)*/
	UINT32				port_buff;				/* �۽� / ���� ��� ���� ũ�� (����: KBytes) */
												/* �۽� Ȥ�� ���� �����͸� �ӽ÷� �����ϱ� ���� ���� (���� 1024 ����) */
	UINT32				recv_buff;				/* �ѹ� ���ŵ� �����Ͱ� �Ͻ������� ����Ǵ� �ӽ� ���� ũ�� (����: KBytes) */
	UINT32				ring_buff;				/* ���ŵ� ��Ŷ���� ����� �ӽ� ���� ũ�� (����: KBytes) */
												/* !!! recv_buff ���� 2�� �̻� Ŀ�� �� !!! (ȯ�� ���Ͽ��� ������ ���� ���������� ũ�� ������ ����) */
	UINT32				u32_reserved;

}	STG_CCSI,	*LPG_CCSI;

typedef struct __st_config_luria_service_info__
{
#if 0	/* Not used (Only Here !!!) */
	TCHAR				print_simulation_out_dir[MAX_PATH_LEN];		/* �뱤 ����� ����� ��� ���� (������'\'�� �ɼ�. �Է��� �ʿ� ����) */
																	/* �⺻ ��� (C:\\lls_simulation) */
#endif
	UINT8				table_count:2;								/* Motion ��� Work Table Count (1 or 2) */
	UINT8				ph_rotate:1;								/* ���а� ��ġ�Ǿ� �ִ� ���� (180�� ȸ�� ����. 0:None, 1:180�� ȸ��) */
	UINT8				motion_control_type:2;						/* Motion Control Type (1:Sieb&Meyer, 2:Newport, 3:ACS) */
	UINT8				x_correction_table_1:1;						/* ���� ���̺� [ 1 ] �� �� �Է� ���� (0:�Է� ����, 1:�Է� ����. ���� ���� ���� Ȯ�� */
	UINT8				x_correction_table_2:1;						/* ���� ���̺� [ 2 ] �� �� �Է� ���� (0:�Է� ����, 1:�Է� ����. ���� ���� ���� Ȯ�� */
	UINT8				y_correction_table_1:1;						/* ���� ���̺� [ 1 ] �� �� �Է� ���� (0:�Է� ����, 1:�Է� ����. ���� ���� ���� Ȯ�� */

	UINT8				y_correction_table_2:1;						/* ���� ���̺� [ 2 ] �� �� �Է� ���� (0:�Է� ����, 1:�Է� ����. ���� ���� ���� Ȯ�� */
	UINT8				table_1_print_direction:1;					/* ������尡 �뱤�ϴ� ���� (0:Negative, 1:Positive) */
	UINT8				table_2_print_direction:1;					/* ������尡 �뱤�ϴ� ���� (0:Negative, 1:Positive) */
	UINT8				active_table_no:2;							/* �ʱ� Ȱ��ȭ �Ϸ��� Table Number (1 or 2) ��, �ʱ� �뱤�� ��, ����ϰ��� �ϴ� Work Table Number (Double gantry or Stage�� ���, �뱤�� ������ Table Number�� �ٲ�) */
	UINT8				emulate_motion:1;							/* Motion ��� ���� �׽�Ʈ �� ������ ���� (0:Motion ��� ���� �׽�Ʈ, 1:Motion ��� ���� �� �׽�Ʈ) */
	UINT8				emulate_ph:1;								/* ������� ��� ���� �׽�Ʈ �� ������ ���� (0:������� ��� ���� �׽�Ʈ, 1:������� ��� ���� �� �׽�Ʈ) */
	UINT8				emulate_trigger:1;							/* Ʈ���� ��� ���� �׽�Ʈ �� ������ ���� (0:Ʈ���� ��� ���� �׽�Ʈ, 1:Ʈ���� ��� ���� �� �׽�Ʈ) */

	UINT8				x_drive_id_1:4;								/* ������忡 X Drive ID for Table 1 �� ���� (0 ~ 7) (�Ʒ� Y Drive ID�� ������ ���� �Է��ϸ� �ȵ�) */
	UINT8				x_drive_id_2:4;								/* ������忡 X Drive ID for Table 2 �� ���� (0 ~ 7) (�Ʒ� Y Drive ID�� ������ ���� �Է��ϸ� �ȵ�) */

	UINT8				y_drive_id_1:4;								/* ������忡 Y Drive ID for Table 1 �� ���� (0 ~ 7) */
	UINT8				y_drive_id_2:4;								/* ������忡 Y Drive ID for Table 2 �� ���� (0 ~ 7) */

	UINT8				debug_print_level:2;						/* 0: Print Info. Only (�뱤 ���� �α׸�). 1: Print Info. and Debug (�뱤 �� ����� �α׸�). 2: Print Info., 3 : Debug and Trace Message (��� �α� ���) */
	UINT8				artwork_complexity:2;						/* Artwork Complexity (0:Normal, 1:High, 2:eXtreme) (�Ź� ���⵵�� ���� �� ����. ���� 0 �� ����. 1 or 2�� ���, ������忡 ��ϵǴ� �Ź� ���� ���� ) */
	UINT8				z_drive_type:3;								/* Z Drive Type (������忡 ������ Motiotn Drive Type) 1 = Stepper motor (built in), 2 = Linear drive from ACS, 3 = Linear drive from Sieb & Meyer (valid from version 2.10.0 and newer) */
	UINT8				over_pressure_mode:1;						/* ������忡 �����ϰ� �߻��� ��� �ð� �� ���� ���� (1:On, 0:Off) */

	UINT8				hys_type_1_scroll_mode:4;					/* �뱤 �ӵ� ��� �� (1 ~ 7) (���� �������� �ӵ� ����) */
	UINT8				led_count:3;								/* ���а迡�� ���Ǵ� LED ���� (���� 4��. 265, 385, 395, 405) */
	UINT8				use_announcement:1;							/* Luria Service�κ��� Announcement ��Ŷ ���� ���� ���� ��, CMPS�� Luria�κ��� ��Ŷ�� �����ϱ� ���� ���� ������ ���� ���� */

	UINT8				use_hw_inited:1;							/* Luria Service�� H/W Init ���� */
	UINT8				ph_count:4;									/* ��� ��ġ�Ǿ� �ִ� ���а� (�������) ���� (�ִ� 16. 0 ~ 15) */
	UINT8				illum_meas_type:3;							/* Photohead�� ���� ���� ��� (1:Power, 2:Energy, 3:Frequency, 4:Density) */

	UINT8				ph_pitch;									/* ������尡 ��/�� �뱤�� �� �ִ� Stripe�� ���� (�ִ� 256) */
																	/* LLS50:2, LLS24:4, LLS10:8 (��, ���а谡 1���̸� 1�� ����) */

	UINT8				motion_control_ip[4];						/* Motion Controller IPv4 (MC2 IPv4) */
																	/* 1 offset size (pulse dist) = (end - start) / pulse_count = 10.8621 um */
	UINT8				ph_ipv4[MAX_PH][4];							/* ��� ��ġ�Ǿ� �ִ� ��������� IPv4 */
	UINT8				z_drive_ip[4];								/* z_drive_type == 3�� ��츸 ��ȿ. MC2�� 1 ���� ���, motion_control_ip�� ����, 2���� ��� �ٸ� IP�̾�� �� */
	UINT8				DOFofPh; //��������� DOF
	UINT8				z_drive_sd2s_ph[MAX_PH];					/* Photohead Z Axis Drive�� Linear Motor Type�� ���, MC2�� ����� SD2S Drive ��ȣ */

	UINT16				scroll_rate;								/* ������尡 ó���� �� �ִ� �ִ� �̵� ���� �� (Default: 19000) */
	UINT16				hys_type_1_negative_offset;					/* HysteresisType1 ���� ���� (����: Trigger Pulse) */
	UINT16				mirror_count_xy[2];							/* DMD Mirror Counts (���� / ����) */
	
	
	
	UINT16				luria_ver_major;							/* ���а� ���� SW version : �ݵ�� ���� (1,2,3,4,....) */
	UINT16				luria_ver_minor;							/* ���а� ���� SW version : �ݵ�� ���� (4,5,7,11,) */
	UINT16				luria_ver_build;							/* ���а� ���� SW version : �ݵ�� ���� (2155) */
																	/* ex> 2.4.3  -> major = 2, minor = 4, build=3 */
																	/*     2.11.6 -> major = 2, minor = 11, build=6 (�ݵ�� ��Ģ ��ų��) */
	UINT32				paralleogram_motion_adjust[MAX_TABLE];		/* ��� ���� (�밢��) �̵� ���� (1000 => 1.0 (Default), 500 => 0.5) */
	UINT32				product_id;									/* ������� Product ID �� (100001:LLS25, 25002:LLS04, 50001:LLS10, 200001:LLS50, 2352002:LLS30, ...) */
	UINT32				u32_reserved;

	DOUBLE				illum_filter_rate[MAX_LED];					/* LED 0 ~ 3 ������ ���� ������ ���� ���� ���� ���� �� ���� */
																	/* (�ʱ� LED Power ������ ��, ������� ��) (������ ���� 100.0f ���� ���� �� (%)) */
	DOUBLE				illum_garbage_pwr;							/* ���� ���� ��ġ�� ���� �ּ� ���� ������ 1 nm �̹Ƿ�, �� �� �����̸� OK  */
	DOUBLE				sensor_diameter;							/* ���� ���� ���� ���� (Circle��. ����: mm) */
	DOUBLE				correction_factor;							/* ���� Power ����ϱ� ���� ���� �� */

	DOUBLE				ph_offset_x[MAX_PH];						/* ������� ���� (X ����. 1�� ������� �������� 2 ~ 7 �� ������� ������ �Ÿ� ��. ����: mm) (�Ҽ��� 4�ڸ����� ��ȿ) */
	DOUBLE				ph_offset_y[MAX_PH];						/* ������� ���� (Y ����. 1�� ������� �������� 2 ~ 7 �� ������� ������ �Ÿ� ��. ����: mm) (�Ҽ��� 4�ڸ����� ��ȿ) */
	DOUBLE				ph_z_focus[MAX_PH];							/* �� Photohead ���� ������ Z Axis �뱤 ���� �� (����: mm) [SET_ALIGN] DOF_FILM_THICK �������� Z Axis (FOCUS) ���� �� (�Ҽ��� 4�ڸ����� ��ȿ) */

	DOUBLE				table_limit_min_xy[MAX_TABLE][2];			/* Work Table�� �̵��� �� �ִ� ��/��/�¿� �ּ� �� (unit: mm) (�Ҽ��� 3�ڸ����� ��ȿ) */
	DOUBLE				table_limit_max_xy[MAX_TABLE][2];			/* Work Table�� �̵��� �� �ִ� ��/��/�¿� �ִ� �� (unit: mm) (�Ҽ��� 3�ڸ����� ��ȿ) */
	DOUBLE				table_expo_start_xy[MAX_TABLE][2];			/* �뱤 ���� ��ġ (����: mm). �뱤 ������ �ϱ� ���� Luria�� ������ִ� �뱤 (���� �뱤 ��ġ�� �ƴ�) ���� ��ġ  */

	DOUBLE				max_y_motion_speed;							/* ���̺� Y �� �������� �̵��� �� �ִ� �ִ� �ӵ� (����: mm/sec) (�Ҽ��� 3�ڸ����� ��ȿ) */
	DOUBLE				x_motion_speed;								/* ���̺� X �� �������� �̵��� �� �ִ� �ӵ� (����: mm/sec) (�Ҽ��� 3�ڸ����� ��ȿ) ���� �뱤 ���� ��ġ�� �̵��� �� X �� �̵� �ӵ� */
	DOUBLE				y_acceleration_distance;					/* Y �� ���ӵ��� ���� �˵��� ������ ���� �ּ����� ���� �Ÿ� (����: mm) (�Ҽ��� 3�ڸ����� ��ȿ) */

	DOUBLE				hys_type_1_delay_offset[2];					/* HysteresisType1 Moving Delay. Positive[0] / Negative[1] Delay (����: mm/sec) (�Ҽ��� 6�ڸ����� ��ȿ) */
	DOUBLE				ph_z_move_max;								/* ������� Z ���� �ִ��� �ٿ�/�� (DOWN) �� �� �ִ� �ִ�ġ �� (����: mm) */
	DOUBLE				ph_z_move_min;								/* ������� Z ���� �ִ��� �ٿ�/�� (UP) �� �� �ִ� �ּ�ġ �� (����: mm) */
	DOUBLE				pixel_size;									/* Lens ������ Scroll Mode (Scroll Step Size)�� ���� �ٸ� (����: um) */
	DOUBLE				ph_z_select_velo[MAX_SELECT_VELO];
																	/* Pixel Resolution (5.4 or 10.8 or 21.6 um) (unit: pm; picometer = 1/1000 nm) */
																	/* ��ȿ�� ���� nanometer ���� (�Ʒ� ���� picometer �������� ǥ����) */
							                       					/* <LLS 10> <lens:  500>						  */
																	/*         Scroll mode 1	:  5.4310344828	um	  */
																	/*         Scroll mode 2	: 10.8620689655	um	  */
																	/*         Scroll mode 3	: 16.2931034483	um	  */
																	/*         Scroll mode 4	: 21.724137931	um	  */
							                       					/* <LLS 25> <lens: 1000>						  */
																	/*         Scroll mode 1	: 10.8620689655	um	  */
																	/*         Scroll mode 2	: 21.724137931	um	  */
																	/*         Scroll mode 3	: 32.5862068966	um	  */
																	/*         Scroll mode 4	: 43.4482758621	um	  */
	
	
	
	//������Ŀ�� ������ �������
	
	UINT8				useAF;										/*Auto Foucs ��� ����*/
	UINT8				useAAQ;// Active Area Qualifier
	UINT8				useEdgeTrigger;
	
	UINT16				afGain; //									/* AutoFocus Gain (1 ~ 4095) ���� Ŭ���� ����? */
	UINT8				afPanelMode; // 0������Ʈ, 1 ��ũ,
	DOUBLE				afAAQInactivateWithinMicrometer[2];
	DOUBLE				afWorkRangeWithinMicrometer[2];						/* AutoFocus Work Range (0.000 mm ~ 24.500 mm) */
	DOUBLE				afTrimMicrometer[MAX_PH];
	DOUBLE				af_abs_workrange[MAX_PH][2];


	/*
	 desc : If it is a lower version after comparing the versions, return an error
	 parm : major	- [in]  Major number
 			minor	- [in]  Minor number
 			build	- [in]  Build number
			mode	- [in]  0x00 : high (More than : '>='), 0x01 : low (Less than : '<'), 0x02 : same (equal : '==')
	 retn: TRUE (Same or higher version), FALSE (Lower version)
	 note: Use compatible commands for Luria Protocol
	*/
	BOOL ValidVersion(UINT16 major, UINT16 minor, UINT16 build, UINT8 mode)
	{
		/* If the current protocol version is higher */
		if (0x00 == mode)
		{
			if (luria_ver_major > major)	return TRUE;
			if (luria_ver_major == major)
			{
				if (luria_ver_minor > minor)	return TRUE;
				if (luria_ver_minor == minor &&
					luria_ver_build >= build)	return TRUE;
			}
		}
		/* If the current protocol version is lower */
		else if (0x01 == mode)
		{
			if (luria_ver_major < major)	return TRUE;
			if (luria_ver_major == major)
			{
				if (luria_ver_minor < minor)	return TRUE;
				if (luria_ver_minor == minor &&
					luria_ver_build < build)	return TRUE;	/* not '<=' */
			}
		}
		/* If the current protocol version is equal */
		else if (0x02 == mode)
		{
			if (luria_ver_major == major &&
				luria_ver_minor == minor &&
				luria_ver_build == build)	return TRUE;
		}
	
		return FALSE;
	}

	/*
	 desc : Emulation ���� ���۵Ǵ� �׸��� �����ϴ��� ����
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsRunEmulated()
	{
		return (emulate_motion == 0x01 || emulate_ph == 0x01 || emulate_trigger == 0x01);
	}

	/*
	 desc : Get the Total Value of Hysteresis Delay
	 parm : None
	 retn : positive_delay + negative_delay (mm)
	*/
	DOUBLE GetHysDelayTotal()
	{
		return hys_type_1_delay_offset[0] + hys_type_1_delay_offset[1];
	}

}	STG_CLSI,	*LPG_CLSI;

/* MC2 Set Parameter */
typedef struct __st_config_mc2_setup_info__
{
	UINT8				drive_count;				/* MC2 ����� ����̺� ���� */
	UINT8				origin_point;				/* ENG_MSOP ��, 0:Left/Bottom, 1:Left/Top, 2:Right/Bottom, 3:Right/Top */
	UINT8				u8_reserved[6];
	UINT8				axis_id[MAX_MC2_DRIVE];		/* ���� ����̺� ID�� �ο��� ����̺� ��ȣ */

	UINT32				mc2_serial;					/* MC2 ����� Serial Number */
	UINT32				u32_reserved;

	DOUBLE				move_velo;					/* ���� ����̺��� �̵� �ӵ� : mm/sec */
	DOUBLE				mark_velo;					/* Align Mark �˻��� ��, Y �� �̵� �ӵ� �� : mm/sec */
	DOUBLE				step_velo;					/* ���а� �� ��Ÿ ���� ������ �� �̵� �ӵ� �� : mm/sec */
	DOUBLE				move_dist;					/* ���� ��ġ���� �̵��ؾ� �� �Ÿ� unit: mm */
	DOUBLE				min_dist[MAX_MC2_DRIVE];	/* ���� �� �� �ּ� �̵� �Ÿ� unit: mm */
	DOUBLE				max_dist[MAX_MC2_DRIVE];	/* ���� �� �� �ִ� �̵� �Ÿ� unit: mm */
	DOUBLE				max_velo[MAX_MC2_DRIVE];	/* ���� �� �� �ִ� �̵� �ӵ� unit: mm */
	DOUBLE				select_velo[MAX_MC2_DRIVE][MAX_SELECT_VELO];	/* ���� �� �� �ִ� �̵� �ӵ� unit: mm */

	/*
	 desc : ���������� �̵� ���� (�ּ� ~ �ִ�) ��ȯ (����: mm)
	 parm : drv_x	- [in]  X �� �̵� ����̺� ��ȣ (Zero-based)
			drv_y	- [in]  Y �� �̵� ����̺� ��ȣ (Zero-based)
	 retn : �簢�� ���� �� ��ȯ
	*/
	CRect GetStageMoveArea(UINT8 drv_x, UINT8 drv_y)
	{
		CRect rArea;

		rArea.left	= (UINT32)ROUNDED(min_dist[drv_x] * 10000.0f, 0);	/* min X */
		rArea.top	= (UINT32)ROUNDED(min_dist[drv_y] * 10000.0f, 0);	/* min Y */
		rArea.right	= (UINT32)ROUNDED(max_dist[drv_x] * 10000.0f, 0);	/* max X */
		rArea.bottom= (UINT32)ROUNDED(max_dist[drv_y] * 10000.0f, 0);	/* max Y */

		return rArea;
	}

	BOOL IsValid()
	{
		BOOL bSucc	= !(max_velo[0] > 0.0f || max_velo[1] > 0.0f);
		if (!bSucc)	AfxMessageBox(L"Failed to load the config [mc2_setup_info]", MB_ICONSTOP|MB_TOPMOST);

		return bSucc;
	}

}	STG_CMSI,	*LPG_CMSI;

/* PLC Set Parameter */
typedef struct __st_config_plc_setup_info__
{
	UINT8				group_no;			/* Ethernet ����� �׷� No */
											/* Group (I do not know, 0 value unconditionally) */
	UINT8				network_no;			/* Ethernet ����� ��Ʈ��ũ No */
											/* Network (fixed value of 0 if not MULTI-DROP) */
	UINT8				station_no;			/* Ethernet ����� ��(Station) No */
											/* Station number (fixed value of 0 when it is not MULTI_DROP) */
	UINT8				plc_no;				/* PLC No */
	UINT8				u8_reserved[4];
											/* PLC number (fixed value of 0xff when it is not MULTI_DROP) */
	UINT16				start_io;			/* Ethernet ����� ���� I/O */
											/* Head I / O (fixed value of 0x03ff if it is not MULTI-DROP) */
	UINT16				addr_x_count;		/* PLC X �ּ��� Total Count (1 bits area / WORD counts). ex> If the score is 64,         8 BYTES */
	UINT16				addr_y_count;		/* PLC Y �ּ��� Total Count (1 bits area / WORD counts)  ex> If the score is 128,       16 BYTES */
	UINT16				addr_m_count;		/* PLC M �ּ��� Total Count (1 bits area / WORD counts)  ex> If the score is 5100,     638 BYTES */
	UINT16				addr_d_count;		/* PLC D �ּ��� Total Count (16 bits area / WORD counts) ex> If the score is 2200,    4400 BYTES */
	UINT16				u16_reserved[3];
	UINT32				start_d_addr;		/* PLC D ���� �ּ� */
	UINT32				u32_reserved;

	/*
	 desc : PLC �ּҿ� �ش�Ǵ� �޸� �ε��� ��
	 parm : None
	 retn : �ε��� �� (0xffff�̸� ����)
	*/
	UINT16 GetAddrToIndex(UINT32 addr)
	{
		/* ��ȿ�� �ּ� ���� �ƴ� ��� */
		if (addr < start_d_addr)	return 0xffff;

		return UINT16(addr - start_d_addr);
	}

	/*
	 desc : PLC ������ �ּ� ���
	 parm : None
	 retn : ������ �ּ� �� (���� �ּ� + �ּ� ������ ����)
	*/
	UINT32 GetLastDAddr()
	{
		return (start_d_addr + addr_d_count - 1);
	}

}	STG_CPSI,	*LPG_CPSI;

/* Global Mark 4 ���� ����, 6���� ���� (LT-RT, LB-RB, LT-LB, RT-RB, LT-RB, LB-RT) �� ���� ���� �� �ӽ� ���� */
typedef struct __st_global_mark_length_result__
{
	UINT8				len_valid;		/* Global Mark 4���� ������ �̷�� �簢���� ���� ���̿� �밢�� ������ �Ѱ踦 ������� ���� */
										/* 0x00 : Not set or Reset, 0x01 : Set (Succ), 0x02 : Set (Fail) */
	UINT8				uu_reserved[7];
	DOUBLE				diff;			/* 6 ���� (LT-RT, LB-RB, LT-LB, RT-RB, LT-RB, LB-RT) �� 1 �� ��ü ������ ���� ��. �Ź� �ҽ� ���� �Ÿ��� ������ �Ÿ� ���� ���� �� (����: um) */
	DOUBLE				scale;			/* 6 ���� (LT-RT, LB-RB, LT-LB, RT-RB, LT-RB, LB-RT) �� 1 �� ��ü ������ ���� ��. �Ź� �ҽ� ���� �Ÿ��� ������ �Ÿ� ���� Scale (Source:�Ź� / Target:����) �� */

}	STG_GMLR,	*LPG_GMLR;
typedef struct __st_global_mark_length_validate__
{
	UINT8				set_data;			/* Mark ���� �����Ǿ� �ִ��� ����  */
	UINT8				u8_reserved[7];
	DOUBLE				angle[2];		/* ����(0) / ����(1) ���� ���� ��. 0.4�� �̻�Ǹ� �ȵ� (degree ����. Radian ���� �ƴ�) */
	STG_GMLR			result[6];

	/*
	 desc : ���� ������ ��� (6����) ��ũ ���� ����/�Ÿ� �� ���� �����ߴ��� ����
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsSetMarkLenData()	{	return set_data==0x01;	}
	/*
	 desc : ���� ������ ��� (6����) ��ũ ���� ����/�Ÿ� ���� �����ߴٰ� �÷��� ����
	 parm : None
	 retn : None
	*/
	VOID SetMarkLenData()	{	set_data = 0x01;	}
	/*
	 desc : �� 6 ���� ���� ���� �Ź� ��� ������ Mark �� ���̰� ��ȿ ���� ���� �ִ��� ����
	 parm : index	- [in]  0x00 ~ 0x05 (6 �� �� 1 ��)
	 retn : TRUE or FALSE
	 ���� : index �� �ǹ�
			0 : Mark.1 - Mark.3 (LT - RT)  1 : Mark.2 - Mark.4 (LB - RB) - TD
			2 : Mark.1 - Mark.2 (LT - LB)  3 : Mark.3 - Mark.4 (RT - RB) - MD
			4 : Mark.1 - Mark.4 (LT - RB)  5 : Mark.2 - Mark.3 (LB - RT)
	*/
	BOOL IsMarkLenValid(UINT8 index)	{	return result[index].len_valid == 0x01;		}
	/*
	 desc : ��ü ��ũ ���� (6����, ���� 4���� �밢�� 2��)�� ��ȿ���� ����
			1���� ��ȿ���� ������ ���� �� ��ȯ
	 parm : count	- [in]  ���ϰ��� �ϴ� ����
			����. 4�� �Է��ϸ� �� ��ȿ�� ������ ��� 4�� �̻� �־�� �ȴٴ� �ǹ�
	 retn : TRUE or FALSE
	 ���� : index �� �ǹ�
			0 : Mark.1 - Mark.3 (LT - RT)  1 : Mark.2 - Mark.4 (LB - RB) - TD
			2 : Mark.1 - Mark.2 (LT - LB)  3 : Mark.3 - Mark.4 (RT - RB) - MD
			4 : Mark.1 - Mark.4 (LT - RB)  5 : Mark.2 - Mark.3 (LB - RT)
	*/
	BOOL IsMarkLenValidAll(UINT8 count=0x06)
	{
		UINT8 i	= 0x00, u8Valid = 0x00;
		for (; i<0x06; i++)
		{
			if (IsMarkLenValid(i) == 0x01)	u8Valid++;
		}
		return (u8Valid == count);
	}
	/*
	 desc : �� 6 ���� ���� ���� �Ź� ��� ������ Mark �� ���̰� ��ȿ ���� �� ����
	 parm : index	- [in]  0x00 ~ 0x05 (6 �� �� 1 ��)
			value	- [in]  0x00 : Not set or Reset, 0x01 : Set (Succ), 0x02 : Set (Fail)
	 retn : None
	 ���� : index �� �ǹ�
			0 : Mark.1 - Mark.3 (LT - RT)  1 : Mark.2 - Mark.4 (LB - RB) - TD
			2 : Mark.1 - Mark.2 (LT - LB)  3 : Mark.3 - Mark.4 (RT - RB) - MD
			4 : Mark.1 - Mark.4 (LT - RB)  5 : Mark.2 - Mark.3 (LB - RT)
	*/
	VOID SetMarkLenValid(UINT8 index, UINT8 value)	{	result[index].len_valid	= value;	}
	/*
	 desc : �Ź��� ���� ��ũ �Ÿ��� ����� ī�޶�� ������ ��ũ ���� �Ÿ� �� Scale (����) �� ����
	 parm : index	- [in]  Zero-based (0x00 ~ 0x05)
			diff	- [in]  �Ÿ� ���� �� (����: nm)
			scale	- [in]  Gerber Mark �Ÿ� / ���� Mark �Ÿ� = Scale ��
							(���� �����Ǿ� ���� �����Ƿ�, ������ 0.0f ������ ������)
	 retn : None
	 ���� : index �� �ǹ�
			0 : Mark.1 - Mark.3 (LT - RT)  1 : Mark.2 - Mark.4 (LB - RB) - TD
			2 : Mark.1 - Mark.2 (LT - LB)  3 : Mark.3 - Mark.4 (RT - RB) - MD
			4 : Mark.1 - Mark.4 (LT - RB)  5 : Mark.2 - Mark.3 (LB - RT)
	*/
	VOID SetMarkLenDiff(UINT8 index, UINT32 diff/*, DOUBLE scale=0.0f*/)
	{
		result[index].diff	= diff / 1000000.0f;
		result[index].scale	= 0.0f/*scale*/;
	}
	/*
	 desc : �Ź��� ���� ��ũ �Ÿ��� ����� ī�޶�� ������ ��ũ ���� �Ÿ� �� ��ȯ
	 parm : index	- [in]  Zero-based (0x00 ~ 0x05)
	 retn : �Ÿ� �� ��ȯ (����: mm)
	 ���� : index �� �ǹ�
			0 : Mark.1 - Mark.3 (LT - RT)  1 : Mark.2 - Mark.4 (LB - RB) - TD
			2 : Mark.1 - Mark.2 (LT - LB)  3 : Mark.3 - Mark.4 (RT - RB) - MD
			4 : Mark.1 - Mark.4 (LT - RB)  5 : Mark.2 - Mark.3 (LB - RT)
	*/
	DOUBLE GetMarkLenDiff(UINT8 index)	{	return result[index].diff;	}
	/*
	 desc : ����(0) / ����(1) ���� ���� �� ��ȯ
	 parm : mode	- [in]  0x00:����, 0x01:����
	 retn : �Ÿ� �� ��ȯ (����: mm)
	 ���� : ����(0) / ����(1) ���� ���� ��. 0.4�� �̻�Ǹ� �ȵ� (degree ����. Radian ���� �ƴ�)
	*/
	DOUBLE GetMarkAngDiff(UINT8 mode)	{	return angle[mode];	}

	/*
	 desc : ���� �ð� ���� ����� ��ũ �Ÿ� �� Scale �� ��ȯ
	 parm : index	- [in]  �Ʒ� ����
			pos_th	- [in]  �� ��° �˻��� ��ũ �Ÿ� �� Scale �� ��ȯ ��ġ (0x00 ~ 0x05)
	 retn : Scale �� ��ȯ
	 ���� : index �� �ǹ�
			0 : Mark.1 - Mark.3 (LT - RT)  1 : Mark.2 - Mark.4 (LB - RB) - TD
			2 : Mark.1 - Mark.2 (LT - LB)  3 : Mark.3 - Mark.4 (RT - RB) - MD
			4 : Mark.1 - Mark.4 (LT - RB)  5 : Mark.2 - Mark.3 (LB - RT)
	*/
	DOUBLE GetMarkLenScale(UINT8 index)	{	return result[index].scale;	}
	/*
	 desc : ����κ��� ������ 4���� ��ũ ��������, ȸ���� ������ 0.4�� �̻� ������� ����
	 parm : None
	 retn : TRUE or FALSE
	 note : �� �˻��� Mark �� ���� ȸ���� ������ �ִ� 0.4 �� �̻� ��� ���, ���� ó�� �ؾ� ��
	*/
	BOOL IsMarkAngleValid()	{	return abs(angle[0]) <= 0.4f && abs(angle[1]) <= 0.4f;	}
	/*
	 desc : ��ũ ���� ����/���� �� �밢���� ���̿� ���� ��ȿ �� �ʱ�ȭ
	 parm : mode	- [in] 0x00 : All, 0x01 : except to scale list
	 retn : None
	*/
	VOID ResetMarkLen()
	{
		set_data	= 0x00;
		memset(result,	0x00, 6 * sizeof(STG_GMLR));
		memset(angle,	0x00, 2 * sizeof(DOUBLE));
	}
	/*
	 desc : ����(0) / ����(1) �� �������� Ȯ��
	 parm : diff	- [DOUBLE] 0x00 : ���� ����
	 retn :  TRUE or FALSE
	*/
	BOOL HorzDiff(DOUBLE diff)
	{
		DOUBLE Diff0_1;
		/*0 : Mark.1 - Mark.3 (LT - RT)  1 : Mark.2 - Mark.4 (LB - RB) - TD*/
		Diff0_1 = abs((result[0].diff * 100.0f) - (result[1].diff * 100.0f));

		if (Diff0_1 > diff)
		{
			return TRUE;
		}
		return FALSE;
	}
	/*
 desc : ����(2) / ����(3) �� �������� Ȯ��
 parm : diff	- [DOUBLE] 0x00 : ���� ����
 retn :  TRUE or FALSE
*/
	BOOL VertDiff(DOUBLE diff)
	{
		DOUBLE Diff2_3;
		/*2 : Mark.1 - Mark.2 (LT - LB)  3 : Mark.3 - Mark.4 (RT - RB) - MD*/
		Diff2_3 = abs((result[2].diff * 100.0f) - (result[3].diff * 100.0f));

		if (Diff2_3 > diff)
		{
			return TRUE;
		}
		return FALSE;
	}


}	STG_GMLV,	*LPG_GMLV;

/* CMPS SW ���� */
typedef struct __st_config_cmps_sw_value__
{
	DOUBLE				global_mark_diff_x[2];				/* �Ź� �����Ϳ��� Mark 2�� �������� Mark 1���� X ��ǥ Offset (����: um) */
															/* �Ź� �����Ϳ��� Mark 4�� �������� Mark 2���� X ��ǥ Offset (����: um) */

}	STG_CCSV,	*LPG_CCSV;

#pragma pack (pop)	/* 8 bytes */