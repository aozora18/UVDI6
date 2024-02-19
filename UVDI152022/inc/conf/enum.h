
/*
 desc : Enum
*/

#pragma once

/* Log Types (8 bits ��, 0x00 ~ 0xff) */
typedef enum class __en_exe_dll_id_type__ : UINT8
{
	en_log_type_service = 0,
	en_log_type_control,
	en_log_type_motion,
	en_log_type_library,
	en_log_type_vision,
	en_log_type_philhmi,
	en_log_type_pgfs,
	en_log_type_calibration,
	en_log_type_etc,

	en_log_type_MAX

}	ENG_EDIT;

/* Process Names (8 bits ��, 0x00 ~ 0xff) */
typedef enum class __en_exe_dll_id_code__ : UINT8
{
	en_none				= 0x00,

	/* Service S/W (0x01 ~ 0x0f) */
	en_engine			= 0x01,			/* Engine App */

	/* Control S/W (0x10 ~ 0x2f) (16 ea) */
	en_podis			= 0x11,			/* PODES App */
	en_teldi			= 0x12,
	en_telbsa			= 0x13,
	en_uvdi15			= 0x14,

	/* Library S/W (0x50 ~ 0x8f) - DI Engine */
	en_ids_cam			= 0x51,			/* IDS Camera Library */
	en_luria			= 0x52,			/* Luria Library */
	en_mc2				= 0x53,			/* MC2 Library */
	en_melsec			= 0x54,			/* Melsec Q Library */
	en_trig				= 0x55,			/* Trigger Library */
	en_mil				= 0x56,			/* MIL Library */
	en_ids				= 0x57,			/* IDS Camera Library */
	en_basler			= 0x58,			/* Basler Camera Library */
	en_efem				= 0x59,			/* EFEM (Robot) Library */
	en_pm100d			= 0x5a,			/* PM100D Library */			
	en_litho			= 0x5b,			/* Linked Litho Library */
	en_efu_ss			= 0x5c,			/* EFU Library */
	en_bsa				= 0x5d,			/* BSA Library */
	en_gentec			= 0x5e,			/* GENTEC-EO Library */
	en_mvenc			= 0x5f,			/* MVENC Library */

	en_milara			= 0x60,			/* Milaram for Prealigner or Robot Library */
	en_lspa				= 0x61,			/* Prealigner Library */
	en_lsdr				= 0x62,			/* Diamond Robot Library */

	// by sysandj
	en_philhmi			= 0x63,			/* PHILHMI Interface Library */	
	en_strobe_lamp		= 0x64,			/* Strobe Lamp Interface Library*/
	// by sysandj

	/* Library S/W (0x90 ~ 0x9f) - Not DI Engine */
	en_pgfs				= 0x91,			/* PGFS Library (Common) */
	en_gfsc				= 0x92,			/* PGFS Library (Client) */
	en_gfss				= 0x93,			/* PGFS Library (Server) */

	/* Calibration Tool S/W (0xa0 ~ 0xaf) */
	en_recipe			= 0xa1,			/* Recipe Data File (Gerber Information & Attribute) */
	en_acam_cali		= 0xa2,			/* Align Camera Calibration Data File */
	en_led_power_cali	= 0xa3,			/* LED Power Calibration Data File */
	en_ph_step_cali		= 0xa4,			/* Photohead Step (Offset) Calibration Data File */
	en_trig_cali		= 0xa5,			/* Trigger Calibration Data File */
	en_acam_focus		= 0xa6,			/* Auto Focusing : Align Camera */
	en_pcam_focus		= 0xa7,			/* Auto Focusing : Photohead Camera */
	en_icam_focus		= 0xa8,			/* Auto Focusing : IDS Camera */
	en_pled_illum		= 0xa9,			/* Photohead Illuminance Measurement (PIllumPH) */
	en_stage_st			= 0xaa,			/* Stage Straightness (StageST) */
	en_measure_flatness = 0xab,			/* Measure Flatness */
	en_2d_cali			= 0xac,			/* 2D Calibration */

	/* Testing (0xf0 ~ 0xff) */
	en_debug			= 0xf1,
	en_demo				= 0xfe,			/* Check whether it operates in demo mode */

}	ENG_EDIC;

/* Memory Mappaed Device ���� �ڵ� �� */
typedef enum class __en_memory_mapped_device_code__ : UINT8
{
	en_conf				= 0x00,			/* Config File for Global */
	en_conf_teaching	= 0x01,			/* Config File for Teaching - Position */
	en_conf_expo_param	= 0x02,			/* Config File for Exposure Parameter */
	en_conf_tracking	= 0x03,			/* Wafer Tracking Information */

	en_mc2				= 0x11,			/* MC2 */
	en_luria			= 0x12,			/* Luria */
	en_plc_q			= 0x13,			/* PLC : Melsec Q Series */
	en_trig				= 0x14,			/* Trigger Board */
	en_efem				= 0x15,			/* EFEM (Robot) */
	en_visi				= 0x16,			/* Vision */
	en_pm100d			= 0x17,			/* LED Detect(PM100D) */
	en_bsa				= 0x18,			/* BSA */
	
	en_gentec			= 0x19,			/* LED Detect(Gentec) */
	en_mvenc			= 0x1a,			/* Trigger Board(Mvenc) */
	en_philhmi			= 0x1b,			/* PHILHMI Interface */
	en_strobe_lamp		= 0x1c,			/* Strobe Lamp Interface */

	en_logs				= 0x1f,			/* Logs */

	en_ph_step			= 0x21,			/* Recipe for Photohead Offset */
	en_ph_correct_y		= 0x22,			/* Recipe for Photohead Correction Table Y */
	en_lspa				= 0x23,			/* PreAligner for Logosole */
	en_lsdr				= 0x24,			/* Robot Diamond for Logosole */
	en_efu_ss			= 0x25,			/* EFU for Shinsung with rs232 */

}	ENG_MMDC;

/* Log Level */
typedef enum class __en_log_normal_warning_error__ : UINT8
{
	en_normal			= 0x00,	/* �Ϲ� or ���� */

	en_warning			= 0x11,	/* ��� */
	en_error			= 0x12,	/* ���� */
	en_fatal			= 0x13,	/* ġ�� */

	en_job_temp			= 0x21,	/* �µ� */
	en_job_work			= 0x22,	/* �۾� */

	en_all				= 0xff,	/* ���� */

}	ENG_LNWE;

/* Work Table�� ���� ��ġ ��, (0, 0) ���� ��ġ */
typedef enum class __en_mc2_stage_origin_point__ : UINT8
{
	en_left_bottom		= 0,
	en_left_top			= 1,
	en_right_bottom		= 2,
	en_right_top		= 3,

}	ENG_MSOP;

/* Stage Control */
typedef enum class __en_mc2_common_control_work__ : UINT8
{
	en_none				= 0x00,
	en_device_stop		= 0x01,		/* Stage and Camera */
	en_device_home		= 0x02,
	en_device_lock		= 0x04,		/* All device Lock On or Off */
	en_device_reset		= 0x08,		/* ���� �ʱ�ȭ */

}	ENG_MCCW;

/* �������� ����(�ӵ�, �����ӵ�) ���� ��� */
typedef enum class __en_mc2_velocity_accelation_distance__ : UINT8
{
	en_none					= 0x00,

	/* Stage : �̵� */
	en_stage_move_velo		= 0x11,		/* ��������		: Step (���� ��ġ �̵�) �ӵ� */
	en_stage_move_acce		= 0x12,		/* ��������		: Step (���� ��ġ �̵�) �����ӵ� */
	en_stage_move_dist		= 0x13,		/* ��������		: Step (���� ��ġ �̵�) �Ÿ� */

	/* Loader : �̵� */
	en_load_move_velo		= 0x21,		/* Loader		: Step (���� ��ġ �̵�) �ӵ� */
	en_load_move_acce		= 0x22,		/* Loader		: Step (���� ��ġ �̵�) �����ӵ� */
	en_load_move_dist		= 0x23,		/* Loader		: Step (���� ��ġ �̵�) �Ÿ� */

	/* Align Camera : �̵� */
	en_acam_move_velo		= 0x31,		/* ����� ī�޶�: Step (���� ��ġ �̵�) �ӵ� */
	en_acam_move_acce		= 0x32,		/* ����� ī�޶�: Step (���� ��ġ �̵�) �����ӵ� */
	en_acam_move_dist		= 0x33,		/* ����� ī�޶�: Step (���� ��ġ �̵�) �Ÿ� */

}	ENG_MVAD;
