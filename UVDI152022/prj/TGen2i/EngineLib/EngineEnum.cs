using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

/* --------------------------------------------------------------------------------------------- */
/*                                       Engine -> Common                                        */
/* --------------------------------------------------------------------------------------------- */

namespace TGen2i.Enum.Comn
{
	/* Process Names (8 bits 즉, 0x00 ~ 0xff) */
	public enum ENG_EDIC /* en_exe_dll_id_code */ : Byte
	{
		en_none				= 0x00,

		/* Service S/W (0x01 ~ 0x0f) */
		en_engine			= 0x01,			/* Engine App */

		/* Control S/W (0x10 ~ 0x2f) (16 ea) */
		en_podis			= 0x11,			/* PODES App */
		en_teldi			= 0x12,
		en_telbsa			= 0x13,

		/* Calibration Tool S/W (0x40 ~ 0x4f) */
		en_acam_focus		= 0x40,			/* Auto Focusing : Align Camera */
		en_pcam_focus		= 0x41,			/* Auto Focusing : Photohead Camera */
		en_icam_focus		= 0x42,			/* Auto Focusing : IDS Camera */

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
		en_lspa				= 0x5c,			/* Prealigner Library */
		en_lsdr				= 0x5d,			/* Diamond Robot Library */
		en_bsa				= 0x5e,			/* BSA Library */
		en_efu_ss			= 0x5f,			/* EFU Library */

		/* Library S/W (0x90 ~ 0x9f) - Not DI Engine */
		en_pgfs				= 0x91,			/* PGFS Library (Common) */
		en_gfsc				= 0x92,			/* PGFS Library (Client) */
		en_gfss				= 0x93,			/* PGFS Library (Server) */

		/* Data File (0xa0 ~ 0xaf) */
		en_recipe			= 0xa1,			/* Recipe Data File (Gerber Information & Attribute) */
		en_acam_cali		= 0xa2,			/* Align Camera Calibration Data File */
		en_led_power_cali	= 0xa3,			/* LED Power Calibration Data File */
		en_ph_step_cali		= 0xa4,			/* Photohead Step (Offset) Calibration Data File */
		en_trig_cali		= 0xa5,			/* Trigger Calibration Data File */

		/* Testing (0xf0 ~ 0xff) */
		en_debug			= 0xf1,
	}

	/* Engine SW 구동 모드 (engine_runnging_vision_mode) */
	public enum ENG_ERVM : Byte /* unsigned char */
	{
		en_monitoring			= 0x00,   /* Engine Monitoring */
		/* for Control App. */
		en_cmps_sw				= 0x10,  /* 제어 SW */
		/* for Calibration */
		en_cali_step			= 0x21,    /* 보정 SW - 광학계 단차 작업 */
		en_cali_vdof			= 0x22,    /* 보정 SW - 광학계 및 얼라인 카메라 Focus 작업 */
		en_cali_exam			= 0x23,    /* 보정 SW - 얼라인 카메라 보정 및 보정 검증 작업 */
		en_acam_spec			= 0x24,    /* 보정 SW - 얼라인 카메라 사양 (SPEC) 검증 작업 */
	}

	/* 디바이스 이동 방향 (Up, Down, left, right. mc2_device_move_direct) */
	public enum ENG_MDMD : Byte
	{
		en_none					= 0x00,

		en_move_up				= 0x01,
		en_move_down			= 0x02,

		en_move_left			= 0x03,
		en_move_right			= 0x04,

		en_move_homed_x			= 0x05,
		en_move_homed_y			= 0x06,

		en_pos_align_start		= 0x07,		/* 스테이지가 Align 시작 위치로 이동 */
		en_pos_expo_start		= 0x08,		/* 스테이지가 노광 시작 위치로 이동 */

		en_move_axis_up			= 0x10,
		en_move_axis_down		= 0x11,
	}

	/* Global Mark 4점에 대한 6 군데의 위치 정보 */
	public enum ENG_GMDD /* global_mark_dist_direct */ : Byte
	{
		en_top_horz								= 0x00,	/* 사각형 -> 위쪽 -> 수평선 (1 <-> 3 마크 간의 길이) */
		en_btm_horz								= 0x01,	/* 사각형 -> 아래 -> 수평선 (2 <-> 4 마크 간의 길이) */
		en_lft_vert								= 0x02,	/* 사각형 -> 왼쪽 -> 수직선 (1 <-> 2 마크 간의 길이) */
		en_rgt_vert								= 0x03,	/* 사각형 -> 오른 -> 수직선 (3 <-> 4 마크 간의 길이) */
		en_lft_diag								= 0x04,	/* 사각형 -> 왼쪽 -> 대각선 (1 <-> 4 마크 간의 길이) */
		en_rgt_diag								= 0x05,	/* 사각형 -> 오른 -> 대각선 (2 <-> 3 마크 간의 길이) */
	}

	/* Align Mark Type */
	public enum ENG_ATGL /* algin_type_global_local */ : Byte
	{
		/* Zero mark */
		en_global_0_local_0x0_n_point	= 0x00,		/* Global (0) points / Local Division (0 x 0) (00) points */
		/* Only 2 mark */
		en_global_2_local_0x0_n_point	= 0x02,		/* Global (2) points / Local Division (0 x 0) (00) points */
		/* Only 3 mark */
		en_global_3_local_0x0_n_point	= 0x03,		/* Global (3) points / Local Division (0 x 0) (00) points */
		/* Only 4 mark */
		en_global_4_local_0x0_n_point	= 0x04,		/* Global (4) points / Local Division (0 x 0) (00) points */

		/* None shared */
		en_global_4_local_2x1_n_point	= 0x11,		/* Global (4) points / Local Division (2 x 1) (08) points */

		en_global_4_local_2x2_n_point	= 0x21,		/* Global (4) points / Local Division (2 x 2) (16) points */
		en_global_4_local_3x2_n_point	= 0x22,		/* Global (4) points / Local Division (3 x 2) (24) points */
		en_global_4_local_4x2_n_point	= 0x23,		/* Global (4) points / Local Division (4 x 2) (32) points */
		en_global_4_local_5x2_n_point	= 0x24,		/* Global (4) points / Local Division (5 x 2) (40) points */

		/* Shared */
		en_global_4_local_2x2_s_point	= 0x31,		/* Global (4) points / Local Division (2 x 2) (13) points */
		en_global_4_local_3x2_s_point	= 0x32,		/* Global (4) points / Local Division (3 x 2) (16) points */
		en_global_4_local_4x2_s_point	= 0x33,		/* Global (4) points / Local Division (4 x 2) (19) points */
		en_global_4_local_5x2_s_point	= 0x34,		/* Global (4) points / Local Division (5 x 2) (22) points */

		/* Not defined */
		en_not_defined					= 0xff,
	}
}

/* --------------------------------------------------------------------------------------------- */
/*                                         Engine -> MC2                                         */
/* --------------------------------------------------------------------------------------------- */

namespace TGen2i.Enum.MC2
{
	/* MC2 Motion Drive ID 즉, SD2S Drive ID (mc2_motion_drive_id) */
	public enum ENG_MMDI : Byte
	{
		en_stage_x_master		= 0x00,
		en_stage_y_master		= 0x01,
		en_stage_x_slave		= 0x02,
		en_stage_y_slave		= 0x03,

		en_axis_ph1				= 0x04,
		en_axis_ph2				= 0x05,
		en_axis_ph3				= 0x06,

		en_axis_bsa_z			= 0x21,
		en_axis_pin_z			= 0x22,
		en_axis_tsa_z			= 0x23,
		en_axis_pin_x			= 0x24,

		en_axis_none			= 0xf0,
		en_all					= 0xff,
	}

	/* 모션 드라이브 쪽에 에러가 발생했을 경우, 드라이브가 스스로 중지 여부 (en_mc2_motion_stop_type) */
	public enum ENG_MMST : Byte
	{
		en_stop_no				= 0x00,		/* Only homing operation */
		en_stop_yes				= 0x01,		/* In case other operation */
	}

	/* 모션 드라이브 제어 방법 (mc2_motion_moving_method) */
	public enum ENG_MMMM : Byte
	{
		en_none					= 0x00,

		en_move_jog				= 0x01,		/* 지속적으로 이동 */
		en_move_step			= 0x02,		/* 설정된 값 만큼 이동 */
		en_move_abs				= 0x03,		/* 절대 위치로 이동 */
	}

	/* SD2S Drive에서 Actual Data의 상태 값 플래그 (sd2s_actual_data_status) */
	public enum ENG_SADS : Byte
	{
		en_flag_done			= 0x00,		/* command_done */
		en_flag_cal				= 0x01,		/* drive_calibrated */
		en_flag_on				= 0x02,		/* power_stage_on */
		en_flag_err				= 0x03,		/* error_code_available */
		en_flag_msg				= 0x04,		/* message_code_available */
		en_flag_busy			= 0x05,		/* command_busy */
		en_flag_zero			= 0x06,		/* velocity_zero */
		en_flag_inpos			= 0x07,		/* target_position_reached */
		en_flag_varc			= 0x08,		/* target_velocity_reached */
		en_flag_init			= 0x09,		/* drive_ready */
		en_flag_done_toggle		= 0x0a,		/* 항상 반전되는 값 */
	}
}

/* --------------------------------------------------------------------------------------------- */
/*                                        Engine -> Luria                                        */
/* --------------------------------------------------------------------------------------------- */

namespace TGen2i.Enum.Luria
{
	/* Packet Function Type : Get or Set */
	public enum ENG_LPGS	/* luria_packet_get_set */ : Byte
	{
		en_set								= 0x00,
		en_get								= 0x01,
	}

	/* Luria Client or Luria Server */
	public enum ENG_LSSC /* luria_system_server_client */ : Byte
	{
		en_server							= 0x00,
		en_client							= 0x01,
	}

	/* LED Power ID */
	public enum ENG_LLPI /* luria_led_power_id */ : Byte
	{
		en_365nm							= 0x00,
		en_385nm							= 0x01,
		en_395nm							= 0x02,
		en_405nm							= 0x03,

		en_none								= 0xff,
	}

	/* Optic Product ID */
	public enum ENG_LPPI /* luria_photohead_product */ : UInt32
	{
		en_lls_2500							= 16702,	/* LLS2500 */
		en_lls_04							= 25002,	/* LLS04 */
		en_lls_10							= 50001,	/* LLS10 */
		en_lls_15							= 100002,	/* LLS15 */
		en_lls_25							= 100001,	/* LLS25 */
		en_lls_30							= 235002,	/* LLS30 */
		en_lls_50							= 200001,	/* LLS50 */
	}

	/* Artwork Complexity */
	public enum ENG_LGAC /* luria_gerber_artwork_complexity */ : Byte
	{
		en_normal							= 0x00,
		en_high								= 0x01,
		en_extreme							= 0x02,
	}

	/* TCP command types (CommandType) */
	public enum ENG_LTCT /* luria_tcp_command_type */ : Byte
	{
		en_write							= 0x10,		// Write something to the LURIA-system
		en_write_reply						= 0x11,		// A reply to a previous Write
		en_read								= 0x20,		// Initiate a read of something from the LURIA-system
		en_read_reply						= 0x21,		// The reply resulting of a previous read
		en_announce							= 0x41,		// Asynchronous announcement from the LURIA-system
	}

	/* User Data Family */
	public enum ENG_LUDF /* luria_user_data_family */ : Byte
	{
		en_none								= 0x00,		/* Not defined */

		en_machine_config					= 0xA0,		// Defines a set of properties that are used to configure general
														// machine based parameters that are normally done once only.
														// Changing these parameters may require a re-assignment of the job(s). (Read/Write)
		en_job_management					= 0xA1,		// Properties and commands that allows a job to be selected
														// and loaded, and get some information about the preprocessed job. (Read/Write)
		en_panel_preparation				= 0xA2,		// Properties and commands that controls the registration and panel data. (Read/Write)
		en_exposure							= 0xA3,		// Properties and commands that controls the printing of a panel. (Read/Write)
		en_direct_photo_comm				= 0xA4,		// Allows for communicating with the photo heads themselves,
														// using commands (called records) defined in [1]. (Read)
		en_luria_com_management				= 0xA5,		// Set up channel for Announcements in LURIA. (Read/Write) (Not used)
		en_announcement						= 0xA6,		// Generated by the LURIA. (Not used)
		en_system							= 0xA7,		// Do various system related operations. (Read/Write)
		en_focus							= 0xA8,		/* Control the focus of the photo head(s) */
		en_calibration						= 0xA9,		/* Functions related to calibration of heads in x, y and z */
	}

	/* Luria Command - MachineConfig (user_id:0xA0) */
	public enum ENG_LCMC /* luria_command_machine_config */ : Byte
	{
		en_total_photo_heads				= 0x01,		// Total number of photoheads in system (1 ~ 8)
		en_photo_head_ip_addr				= 0x02,		// The IP Addresses for the photo heads connected to Luria
		en_photo_head_pitch					= 0x03,		// Number of strips between two photo heads. Pitch between photoheads, given in number of exposure strips

		en_not_used_04						= 0x04,		// Lens magnification factor. Lens magnification as a factor, multiplied by 1000. (Eg. factor 0.5 is given as 500)

		en_photo_head_rotate				= 0x05,		// The alignment of the photo heads. Orientation of photoheads. 0 = No rotation. 1 = Rotated 180 degrees.
		en_parallelogram_motion_adjust		= 0x06,		// The parallelogram motion angle of the motion controller will be adjusted by this factor. Adjustment factor, multiplied by 1000. (Eg. factor 0.97 is given as 970, factor 1.025 is given as 1025).
		en_scroll_rate						= 0x07,		// The maximum scroll rate that a photo head can tolerate. The maximum scroll rate of the system. (Default 19000)
		en_motion_control_type				= 0x08,		// The motion controller connected to the system. The motion controller type used. 1 = Sieb & Meyer, 2 = Newport
		en_motion_control_ipaddr			= 0x09,		// IP address of the motion controller.

		en_not_used_0a						= 0x0a,		// The ID to be set for the x-axis in the motion controller. Note! Valid if MotionControlType = 1 only. Ignored for other types
		en_not_used_0b						= 0x0b,		// The ID to be set for the y-axis in the motion controller. Up to two y-axis are supported. Note! Valid if MotionControlType = 1 only. Ignored for other types

		en_x_correction_table				= 0x0c,		// X-correction table for the x-motion. The first entry must have 0 adjustment. The correction given for the last table entry will be used for x-position at higher x-positions than the x-position for the last table entry
		en_table_position_limits			= 0x0d,		// The minimum and maximum coordinate of the motion controller. This tells the Luria maximum allowable motion position. (Not used yet!)
		en_table_exposure_start_pos			= 0x0e,		// The start position for an exposure for the given table.
		en_max_y_motion_speed				= 0x0f,		// This value limits the maximum speed that the motion controller will run in y-direction
		en_x_motion_speed					= 0x10,		// This is the speed to be used for x-movements. This speed will also be used to move to start position during pre-print.
		en_theta_position					= 0x11,		// Theta drive position for motion controllers having a theta-axis. Note! Valid if MotionControlType = 2 only. Ignored for other types
		en_y_acceleration					= 0x12,		// Motion controller Y-axis acceleration. Note! Valid if MotionControlType = 2 only. Ignored for other types
		en_y_acceleration_distance			= 0x13,		// Y-axis acceleration/deceleration distance. Note! Valid if MotionControlType = 1 only. Ignored for other types
		en_hysteresis_type1					= 0x14,		// Motion controller hysteresis. Note! Valid if MotionControlType = 1 only. Ignored for other types
		en_hysteresis_type2					= 0x15,		// Motion controller hysteresis. Note! Valid if MotionControlType = 2 only. Ignored for other types
		en_photo_head_offset				= 0x16,		// Measured offset in x- and y directions compared to photo head number 1.
		en_table_print_direction			= 0x17,		// Start-direction of y axis when printing.
		en_active_table						= 0x18,		// Select which table that is the active table. Note! Valid if MotionControlType = 1 only. Ignored for other types
		en_get_table_motion_start_position	= 0x19,		// This will give the exact position where the motor will be placed when starting a print. This position will be based on the current TableExposureStartPos and YaccelerationDistance
		en_emulate_motor_controller			= 0x1a,		// Enables use of the Luria software without a motion controller.
		en_emulate_photo_heads				= 0x1b,		// Enables use of the Luria software without photo head(s).
		en_emulate_triggers					= 0x1c,		// Enables use of the Luria software without trigger input signals to the photo head(s).
		en_debug_print						= 0x1d,		// Prints debug information to logfile.
		en_print_simulation_out_dir			= 0x1e,		/* Output path name text string (without null-termination) of simulation result files. The path must be absolute. Atrailing backslash (\) is optional */
		en_not_used_1f						= 0x1f,		/* 0 = Mirror shake is enabled (default), 1 = Mirror shake is disabled. Be careful not to disable mirror shake if not strictly needed. Also note that this setting is not stored permanently. At power-up, mirror shake will always be enabled. */

		en_y_correction_table				= 0x20,		/* Y-correction table, one for each table. Each printed strip can be adjusted individually, in positive or negative direction. Max y-adjustment is +/- 500 um (500000 nm) */
		en_linear_z_drive_settings			= 0x21,		/* the ID to be set for the z-axis in the motion controller for this specific photo head. Note! Valid if ZdirveType = 2 only. */
		en_hysteresis_type_3				= 0x22,		/* Motion controller hysteresis for motor controller type 3. Note! Valid if MotionControlType = 3 Only. Ignored for other types. */
		en_z_drive_ip_addr					= 0x23,		/* IP Address of the Z drive (Focus drive). Note! Valid if ZDriveType = 2 */
		en_xy_drive_id						= 0x24,		/* The ID to be set for the x and y-axis in the motion controller for up to two tables. Note! Valid if MotionControlType = 1 and 3 only. Ignored for other types */
		en_product_id						= 0x25,		/* ID identifying the LLS-system */
		en_artwork_complexity				= 0x26,		/* ArtworkComplexity reflects how fine-pitched the artwork is. A complex artwork will result in larger preprocessed files. Selecting a higher ArworkComplexity will allow larger, more complex preprocessed files to be accepted by Luria, by reserving more space inside the photo head(s) storage. */
														/* NOTE: Higher complexity setting will result in fewer jobs to be held simultaneously in Luria. Therefore, ArworkComplexity should be held as low as possible. It should only be increased if loading a preprocessed job results in StatusCode "StripFileSizeTooLarge" */
		en_z_drive_type						= 0x27,		/* Drive Type: 1 = Stepper motor (built-in), 2 = Linear drive (external) */
														/* Focus Z drive type (1 = Stepper motor (use built in driver in photo heads), 2 = Linear drive (use external driver, such as ACS or similar). This setting is equal for all photo heads */
		en_over_pressure_mode				= 0x28,		/* When enabled, this mode will configure the fans so that an over-pressure is created inside the photo head */
		en_depth_of_focus					= 0x29,		/* Valid for linear Z-motor only, ignored for stepper motor */
		en_extra_long_stripes				= 0x2a,		/* When this is enabled the length of a jobs artwork stripes is approximately twice the length of normal stripes */
		en_mtc_mode							= 0x2b,		/* Enabling MTC will result in smoother edges in cases where the motion of the y-axis is not perfect. It is required that the job is preprocessed with the MTC mode on. Note that enabling MTC will reduce the maximum scroll rate that can be used. */
		en_use_ethercat_for_af				= 0x2c,		/* When not using ethercat for AF Luria will not try to communicate over ethercat with the z-motion controller. */
		en_spx_level						= 0x2d,		/* The spx-level from preprocessed files must correspond to the value set here */
		en_ocp_error_mask					= 0x2e,		/* This will mask any OCP errors for the time that is given. This will apply to ALL light sources. */
		en_position_hysteresis_type1		= 0x2f,		/* Motion controller hysteresis. Note! Valid if MotionControlType = 1 only. Ignored for other types This hysteresis is position based, and will not be affected by the print speed. Note: This hysteresis is added to the hysteresis done by HysteresisType1. */
	}

	/* Job Management command (user_id : 0xA1) */
	public enum ENG_LCJM /* luria_command_job_management */ : Byte
	{
		en_root_directory					= 0x01,		/* Root directory where pre-processed jobs are placed. Note: There must be no assigned jobs when changing RootDirectory. */
														/* If trying to change RootDirectory while jobs are assigned, an error will be returned. The RootDirectory given must exist unless giving an empty string (L = 0), which will clear the root directory. */
		en_get_job_list						= 0x02,		/* If no jobs are assigned, then nothing is returned, L = 0. The paths may be a mix of relative and absolute paths, depending on how the jobs were assigned. */
		en_assign_job						= 0x03,		/* Path name text string (without null-termination) of a preprocessed job. Could be relative path (relative to RootDirectory) or absolute path. A trailing backslash (\) is optional. */
														/* Assign a new job. The path name given is the path to valid pre-processed job. An error message will be given if the path does not contain valid files. */
														/* The first assigned job will be set as selected job automatically. */
		en_delete_selected_job				= 0x04,		/* Delete the selected job from the list of assigned jobs. If no jobs assigned, an error will be returned. (This command will not delete anything from the disk, only from the list of assigned jobs). */
		en_selected_job						= 0x05,		/* Path name text string (without null-termination) of selected job. The path may be relative or absolute. A trailing backslash (\) is optional. */
		en_load_selected_job				= 0x06,		/* Load the currently selected job to the photo head(s). */
														/* reply is returned immediately and the load progress can be queried using "GetSelectedJobLoadState". */
														/* Alternatively, announcements from the Luria can be used uria will send "LoadState"-announcements after each strip being loaded to a photo head and also when all files are loaded completely (or if any failure happened). */
		en_get_selected_job_load_state		= 0x07,		/* Returns the current load-state of the system (0x01-Load not started, 0x02 - Loading, 0x03-Load complete, 0x0f-Load failed) */
		en_get_fiducials					= 0x08,		/* Returns the global and local fiducial points of the given job. */
		en_get_parameters					= 0x09,		/* Returns a set of parameters for the given job */
		en_get_panel_data_dcode_list		= 0x0a,		/* Returns all defined dynamic panel data dcodes for the given job. */
		en_get_panel_data_info				= 0x0b,		/* Returns all the panel data information for the given d-code and job. */
		en_get_max_jobs						= 0x0c,		/* The maximum number of jobs that can be assigned simultaneously */
														/* The number of jobs than can be assigned simultaneously will vary according to photo head hardware and MachineConfig: ArtworkComplexity setting */
	}

	/* Panel Preparation command (user_id : 0xA2) */
	public enum ENG_LCPP /* luria_command_panel_preparation */ : Byte
	{
		en_dynamic_fiducials				= 0x01,		/* Dynamic fiducial coordinates. When sending dynamic fiducials these will be used instead of global and local fiducials from the gerber-file. */
														/* Up to 8 (N<= 8) global dynamic fiducials, while up to 200 (N <= 200) local dynamic fiducials are supported. */
														/* NOTE: To enable use of global and local fiducials from gerber-file again, then a special DynamicFiducials command where type = 0 (global) and N = 0 must be sent. */
		en_registration_points				= 0x02,		/* Registration points. Up to 400 registration points are supported, ie. N <= 400. */
		en_global_transformation_receipe	= 0x03,		/* Transformation recipe that is valid for both global and local zones. If set to Fixed, the values in FixedRotation, FixedScaling and FixedOffset are used. */
		en_global_fixed_rotation			= 0x04,		/* Global fixed rotation, in micro-degrees. Ignored for TransformationReceipe 0. */
		en_global_fixed_scaling				= 0x05,		/* Global fixed x and y-scaling factor * 106 . Ignored for TransformationReceipe 0. */
		en_global_fixed_offset				= 0x06,		/* Global fixed x and y-offset. In nanometers. Ignored for TransformationReceipe 0. */
		en_local_zone_geometry				= 0x07,		/* This will determine the zone geometry to be used for local zones. */
		en_get_number_of_local_zones		= 0x08,		/* The number of calculated local zones. */
		en_get_transformation_params		= 0x09,		/* Retrieve the transformation parameters based on the current registration points and transformation recipe. */
		en_unused							= 0x0a,
		en_run_registration					= 0x0b,		/* Will run registration based on the current fiducials and registration points. */
														/* If registration error is received, then the detailed error can be found using */
														/* "GetRegistrationStatus" command. (Note that this command is also run automatically during PrePrint) */
		en_get_registration_status			= 0x0c,		/* Retrieve the current status of the registration module in Luria */
		en_panel_data_serial_number			= 0x0d,		/* Set the serial number string and number to be incremented per board. */
														/* If the serial number string is set to ABCD####WXYZ, the symbol marking increment number is '#' and starting value is set to 0123, */
														/* then the first serial number will be printed as ABCD0123WXYZ, the next ABCD0124WXYZ and */
														/* so on for all the D-codes defined as SerialNumber. If the number reaches max value, e.g. 999, the next value will be 000. */
		en_panel_data						= 0x0e,		/* Specify the panel data to be printed on the panel, where to print it and the size/orientation. */
														/* D-code must match what is specified in the job (from PreProcessing), where also the PanelData type is pre-defined. */
														/* For SerialNumber and Scaling information, the Length of string should be set to 0 and Text string (S) should be ignored, */
														/* but for GeneralText this is the text string that will be printed. */
		en_use_shared_local_zones			= 0x0f,		/* If “Use shared zones” is selected, then the system will try to create shared zones where possible */
		en_local_transformation_recipe		= 0x10,		/* Transformation recipe that is valid for local zones. If set to Fixed, the values in LocalFixedRotation, LocalFixedScaling and LocalFixedOffset are used. */
		en_local_fixed_rotation				= 0x11,		/* Has effect in fixed transformation mode only. Fixed rotation to be used. N <= 200. */
		en_local_fixed_scaling				= 0x12,		/* Has effect in fixed transformation mode only. Fixed scaling to be used. N <= 200. (Eg. Factor 1.0002 is given as 1000200, factor 0.9998 is given as 999800) */
		en_local_fixed_offset				= 0x13,		/* Has effect in fixed transformation mode only. Fixed offset to be used. N <= 200. */
		en_global_rectangle_lock			= 0x14,		/* When global auto transformation recipe is selected, it is possible to force the registration points to make a rectangle. */
		en_remove_panel_data				= 0x15,		/* Will remove the panel data that is assigned to the given d-code */
		en_get_warp_of_limits_coordinates	= 0x16,		/* Get coordinates where registration result in warp out of fpga limits. Max reported errors is 400. In this case the whole panel may be out of war limits and thus print should not be proceeded */
		en_snap_to_zone_mode				= 0x17,		/* When this mode is enabled, the artwork is always warped to the closest zone registration setting */
														/* This mode can only be used in combination with non-shared local zones. And the zones have to be distributed in a grid parallel to x and y axis (not scattered). Recommended to use only one zone in y direction. */
		en_local_zone_fid_search_bw			= 0x18,		/* Set the band width for searching fiducials in a zone. Nominal values are typically below 1mm to allow strictly rectangular zones only. This interface may be used to increase the search limit to allow for non square zones. Values should be set as low as possible to avoid risk of confusing fiducials between zones. */
	}

	/* Exposure Command (user_id : 0xA3) */
	public enum ENG_LCEP /* luria_command_exposure_printing */ : Byte
	{
		en_led_amplitude					= 0x01,		/* Set the LED amplitude that will be used for the exposure */
														/* Luria will set the given amplitude in the photo head(s) when printing starts */
		en_scroll_step_size					= 0x02,		/* Set the scroll step size to be used for the exposure. */
		en_frame_rate_factor				= 0x03,		/* Set the frame rate factor to be used for the exposure. */
														/* The factor will reduce the exposure speed by the given factor. */
														/* Exp. factor 0,5 (given as 500) will reduce the speed to half. */
		en_led_duty_cycle					= 0x04,		/* Set the desired duty cycle of the LEDs in the photo head. */
														/* Luria will set the LED on-time in all LEDs when starting an exposure. LED on-time = (1 / (ScrollRate * ScrollStepSize * FrameRateFactor)) * LedDutyCycle/100 */
		en_pre_print						= 0x05,		/* Prepare for a new print. This will make "Print" starting immediately when is called later. */
														/* This command can be run in "shadow" time. Note that this will move the system to its exposure starting position. */
														/* The current settings (led amplitude, scroll step size, duty cycle, etc) are set in the photo heads at this time. */
														/* NOTE: This command always returns OK immediately. Any errors occurring during pre-print process is sent as an asynchronous announcement error. */
		en_print							= 0x06,		/* Start a print of the currently selected job. Requires PrePrint to be run in advance. Status during printing can be queried using GetExposureState or by reading incoming announcements. */
														/* NOTE: This command always returns OK immediately. Any errors occurring during print process is sent as an asynchronous announcement error. */
		en_abort							= 0x07,		/* Abort the current action. Note that it may take some time until the abort-operation is completed. This can be checked using GetExposureState, see Table 1 below. */
		en_get_exposure_state				= 0x08,		/* Get the current exposure state. If state is "PrintRunning", then information about the current motion is given. */
		en_get_exposure_speed				= 0x09,		/* the motion speed that will be used (or that is being used if printing). */
														/* This speed is calculated as follows: speed (meter/sec) = ScrollRate * PixelSize * ScrollStepSize * FrameRateFactor. Note: speed <= MaxMotionSpeed */
		en_reset_exposure_state				= 0x0a,		/* Reset the exposure state for the currently selected job irrespective of current state */
		en_enable_af_copy_last_stripe		= 0x0b,		/* Enable AF copy function. 0 = Disable (default), 1 = Enable. */
														/* When function is enabled, the photo head printing the last strip will copy the autofocus profile of the second to last strip and IMPLEMENTED */
														/* use that profile for focusing the last strip. If autofocus is disabled in the photo head, then this function is bypassed. */
	#if (false)
		/* Not used. (Only here!!!) */
		en_simulate_print					= 0x0c,		/* Start a simulated print. The selected job will be printed to a file instead of exposed through the photo head(s). The resulting */
														/* output image will be written to the output directory given in MachineConfig::PrintSimulationOutDir */
	#endif
		en_move_table_to_exposure_start_pos	= 0x0d,		/* This will move the active table to its exposure start position. */
		en_get_number_of_light_sources		= 0x0e,		/* Get the number of available light sources for the given photo head. */
		en_get_light_source_status			= 0x0f,		/* Get the status of the requested light source (en_light_source_status_multi로 대체) */
		en_disable_af_first_strip			= 0x10,		/* This command makes it possible to turn off autofocus for the first strip of a print, for example if this strip is outside the panel. if autofocus is disabled in the photo head, then this function is bypassed */
		en_af_sensor						= 0x11,		/* The selected AF sensor type and laser setting are set in the phot head(s) when running PrePrint. Note that these seetings only apply if autofocus is used */
		en_light_ramp_up_down				= 0x12,		/* This command should be sent to Luria if the current light amplitude is significantly higer / lower than the amplitude used in the previsou print. */
														/* Important: This command must be sent after sending LightAmplitude command but before PrePrint */
														/* Note that this command is not valid for RLT systems, If sending this command when RLT heads are connected, then an error message will be given. */
		en_af_sensor_measure_laser_pwm		= 0x13,		/* Function returns optimal laser PWM by using AGC function. The measured values are saved in Luria for current Job. When new job are loaded new measurements have to be take. */
		en_light_source_status_multi		= 0x14,		/* Get the light source type and status of all light sources in all photo heads. See table below for detailed information on the status word */
	}

	/* Direct Photohead (user_id : 0xA4) */
	public enum ENG_LCDP /* luria_command_direct_photohead */ : Byte
	{
		en_photohead_command				= 0x01,		/* Photohead Command (Read Only) */
	}

	/* LuxBeam 4600 ~ 4700 or 4800 Software Interface - Record Number */
	public enum ENG_LLRN /* lurai_luxbeam_record_number */ : UInt16
	{
		en_none								= 0x0000,

		/* Sequencer State */
		en_set_sequencer_state				= 0x006A,	/* 106 : Set Sequencer State */
		en_get_sequencer_state				= 0x0132,	/* 306 : Get Sequencer State */
		en_res_sequencer_state				= 0x01FA,	/* 506 : Reply */
		/* Line Sensor Plot */
		en_set_plot_enable					= 0x0076,	/* 118 : Set Line Sensor Plot */
		en_get_plot_enable					= 0x013e,	/* 318 : Get Line Sensor Plot */
		en_res_plot_enable					= 0x0206,	/* 318 : Reply */
		/* Motor Position Initialize */
		en_set_fcs_mtr_mid_pos				= 0x007d,	/* 125 : Set Focus Motor to Mid-position (Initialize) */
		/* Focus Motor Calibration - Measured Distance */
		en_set_fcs_mtr_measure_dist			= 0x007e,	/* 126 : Set Focus Motor Measured Distance */
		en_get_fcs_mtr_measure_dist			= 0x0146,	/* 326 : Get Focus Motor Measured Distance */
		en_res_fcs_mtr_measure_dist			= 0x020e,	/* 326 : Reply */
		/* Focus Motor Move Absolute Position */
		en_set_fcs_mtr_abs_pos				= 0x0084,	/* 132 : Set Focus Motor Move Absolute Postion */
		en_get_fcs_mtr_abs_pos				= 0x014C,	/* 332 : Get Focus Motor Move Absolute Postion */
		en_res_fcs_mtr_abs_pos				= 0x0214,	/* 532 : Reply */
		/* LED Driver Amplitude */
		en_set_led_amplitude				= 0x0085,	/* 133 : Set Led Amplitude */
		en_get_led_amplitude				= 0x014D,	/* 333 : Get Led Amplitude */
		en_res_led_amplitude				= 0x0215,	/* 533 : Reply */
		/* LED Driver Light On / Off */
		en_set_led_light_on_off				= 0x0089,	/* 137 : Set Led Light On / Off */
		en_get_led_light_on_off				= 0x0151,	/* 337 : Get Led Light On / Off */
		en_res_led_light_on_off				= 0x0219,	/* 537 : Reply */
		/* Clear Error Status */
		en_set_clear_error_status			= 0x00AE,	/* 174 : Clear Error Status */
		/* Image Type */
		en_get_image_type					= 0x012f,	/* 303 : Get Image Type */
		en_res_image_type					= 0x01f7,	/* 503 : Reply */
		/* Focus Motor Status */
		en_get_fcs_motor_status				= 0x0148,	/* 328 : Get Focus Motor Status */
		en_res_fcs_motor_status				= 0x0210,	/* 528 : Set Focus Motor Status */
		/* Photohead LED Temperature */
		en_get_ph_led_temp					= 0x014f,	/* 335 : Get Phothead Led On-Time */
		en_res_ph_led_temp					= 0x0217,	/* 535 : Reply */
		/* Current Autofocus Position */
		en_get_autofocus_position			= 0x0159,	/* 345 : Get Autofocus Position */
		en_res_autofocus_position			= 0x0221,	/* 545 : Reply */
		/* Photohead LED On-Time */
		en_get_ph_led_ontime				= 0x016a,	/* 362 : Get Phothead Led On-Time */
		en_res_ph_led_ontime				= 0x0232,	/* 562 : Reply */
		/* Internal Image */
		en_set_load_internal_image			= 0x00A9,	/* 169 : Set Load Internal Image */
		en_get_load_internal_image			= 0x0171,	/* 369 : Get Load Internal Image */
		en_res_load_internal_image			= 0x0239,	/* 569 : Reply */
		/* Active Sequence File */
		en_set_active_sequence				= 0x00AB,	/* 171 : Set Active Sequence */
		en_get_active_sequence				= 0x0173,	/* 371 : Get Active Sequence */
		en_res_active_sequence				= 0x023B,	/* 571 : Reply */
		/* Enable Flatness Correction Mask (PPC) */
		en_set_flatness_correction_mask		= 0x0094,	/* 148 : Set Enable Flatness Correction Mask (PPC) */
		en_get_flatness_correction_mask		= 0x015C,	/* 348 : Get Enable Flatness Correction Mask (PPC) */
		en_res_flatness_correction_mask		= 0x0224,	/* 548 : Reply */
		/* Camera Illumination LED */
		en_set_illum_led					= 0x00C4,	/* 196 : Set Camera Illumination LED */
		en_get_illum_led					= 0x018C,	/* 396 : Get Camera Illumination LED */
		en_res_illum_led					= 0x0254,	/* 596 : Reply */
		/* Overall Error Status Word */
		en_get_overall_error_status			= 0x0178,	/* 376 : Get Overall Error Status Word (LB4700) */
		en_res_overall_error_status			= 0x0240,	/* 576 : Reply */
		/* Light Source Info */
		en_get_light_source_info			= 0x01A5,	/* 421 : Get Light Source Info */
		en_res_light_source_info			= 0x026D,	/* 621 : Reply */

		/* Reply Ack */
		en_reply_ack						= 0x01F5,	/* 501 : General Reply Record (ReplyAck) */
	}

	/* Com Management Command (user_id : 0xA5) */
	public enum ENG_LCCM /* luria_command_com_management */ : Byte
	{
		en_announce_server_ipaddr			= 0x01,		/* The IP address of the UI (announce server) that the Luria (announce client) will send announcements to. */
														/* The default IP address at Luria power-on is 0.0.0.0. Luria will send announcements only if the IP address is different from 0.0.0.0. */
														/* Hence, if UI do not want the Luria to generate announcements, the IP address should be set to 0.0.0.0. */
		en_test_announcement				= 0x02,		/* Force a "TestAnnouncement" to be sent from Luria. */
														/* This will be sent if AnnounceServerIpAddr is different from 0.0.0.0. */
														/* If the "TestAnnouncement" was unsuccessfully sent for the third time in a row (i.e. the reply to "GenerateTestAnnouncement" was not OK three times), */
														/* all announcements will be disabled, irrespective of AnnounceServerIpAddr not being 0.0.0.0. */
														/* The announcements will be enabled again as soon as a "TestAnnouncement" was successfully sent (i.e. if"GenerateTestAnnouncement" is ok) */
		en_announcement_status				= 0x03,		/* The announcements will be automatically disabled by Luria if no announcements could be sent three times in a row. */
														/* The only way it will be enabled again is by successfully sending a "GenerateTestAnnouncement". */
														/* Note that the status will be "disabled" if AnnounceServerIpAddr is 0.0.0.0. */
	}

	/* Announcement Command (user_id : 0xA6) */
	public enum ENG_LCEA /* luria_command_event_announcement */ : Byte
	{
		en_test_announcement				= 0x01,		/* Sent only when requested by peer (UI), using LuriaComManagement -> "GenerateTestAnnouncement" */
		en_error							= 0x02,		/* This announcement is sent if an error occurs asynchronously. */
														/* The "Status"-field holds an error number, indicating the specific error. */
		en_load_state						= 0x03,		/* This announcement is sent every time a strip has been loaded to a photo head and when a complete gerber has been loaded successfully (or when loading fails). */
		en_exposure_state					= 0x04,		/* This announcement is sent every time a print motion is complete and when a print/pre-print is completed successfully (or with failure). */
	}

	/* System status command (user_id : 0xA7) */
	public enum ENG_LCSS /* luria_command_system_status */ : Byte
	{
		en_init_hardware					= 0x01,		/* This command will try to connect to all external devices (photo heads, motion controller, etc) and initialize these */
														/* This command must be called before being able to print */
		en_get_system_status				= 0x02,		/* Query the status of the complete system (Read Only) */
		en_set_system_shutdown				= 0x03,		/* This command will safely shut down all components of the system. (Write Only) */
		en_set_dummy_cmd					= 0x04,		/* No action is taken for this command. Luria will ignore it and send no reply. For test-purposes only */
		en_upgrade_firmware					= 0x05,		/* This command will upgrade all photo heads with the firmware in the given bundle path or firmware file. The path name can be an absolute path to a LLS software bundle or a path to a tar.gz file to be used for upgrading. When the upgrade starts, a new initializeHardware will be required. If trying to do initializeHardware before upgrade process is complete, BusyUpgradingFirmware status code is given. */
		en_get_smart_data					= 0x06,		/* This command will read out the SMART data on the SSD. The SMART data consists of 14 sources with each 12 parameters */
		en_load_internal_test_image			= 0x07,		/* This will load the selected test image that is built-in the photo head and display it on the DMD */
		en_set_light_intensity				= 0x08,		/* Use this command to turn on the light in the photo head(s) in order to display the internal test image selected using command LoadInternalTestImage */
		en_mainboard_temperature			= 0x09,		/* This will give the temperature of the mainboard for each photo head */
		en_overall_error_status_multi		= 0x0a,		/* Get the error status for all photo heads. See table below for bit definitions of the error status word */
	}

	/* Photohead Focus command (user_id : 0xA8) */
	public enum ENG_LCPF /* luria_command_photohead_focus */ : Byte
	{
		en_initialize_focus					= 0x01,		/* This will initialize the z-axis. Note that this may take up to 30 seconds to complete. If initialize failes, the status code FocusMotorOperationFailed is give */
		en_position							= 0x02,		/* Focus Z-position given in micrometers. */
		en_enable_af						= 0x03,		/* Start / Stop (Enable / Disable) photo head auto focus system */
		en_trim								= 0x04,		/* Focus trim in nanometers */
		en_active_area_qualifier			= 0x05,		/* Autofocus will only operate inside the limits given here */
		en_abs_work_range					= 0x06,		/* Note that setting AbsWOrkRange will clear the AbsWorkRangeStatus for the given photo heads(s) */
		en_abs_work_range_status			= 0x07,		/* The status-bit for a given photo head will be cleared next time a new AbsWorkRange has been set. */
		en_af_edge_trigger					= 0x08,		/* If the next focus point is further below or above the current focus point than the given value, then the focus will not be changed */
		en_set_position_multi				= 0x09,		/* Set focus position for several photo heads simulatneously. */
	}

	/* Photohead Motor Status */
	public enum ENG_LPMS /* luria_photohead_motor_status */ : UInt16
	{
		en_busy_moving						= 0x0000,
		en_must_move_to_mid_pos				= 0x0001,
		en_endpt_reached					= 0x0002,
		en_mid_pos_proc_running				= 0x0004,
		en_mid_pos_proc_ep_low_reached		= 0x0008,
		en_mid_pos_proc_ep_upp_reached		= 0x0010,
		en_timeout_mid_pos_process			= 0x0020,
		en_af_outcast_detected				= 0x0040,
		en_af_sensor_signal_missing			= 0x0080,
		en_af_position_outside_work_range	= 0x0100,
		en_controller_connected				= 0x0200,
		en_mid_pos_proc_aborted				= 0x0400,
	}

	/* Job Management - SelectedJobLoadState */
	public enum ENG_SJLS /* selected_job_load_state */ : Byte
	{
		en_load_not_started					= 0x01,
		en_loading							= 0x02,
		en_load_complete					= 0x03,
		en_load_failed						= 0x0f,
	}

	/* Job Management - Panel Data Type */
	public enum ENG_PDTC /* panel_data_type_code */ : Byte
	{
		en_text								= 0x00,	/* Text */
		en_qr_code							= 0x01,	/* QR Code */
		en_ecc_200							= 0x02,	/* ECC 200 */
	}

	/* Job Management - Panel Data Content */
	public enum ENG_PDCC /* panel_data_content_code */ : Byte
	{
		en_serial							= 0x00,	/* Board Serial Number */
		en_scale							= 0x01,	/* Scaling Information */
		en_text								= 0x02,	/* General Text */
	}

	/* Align Mark Type Flag */
	public enum ENG_AMTF /* align_mark_type_flag */ : Byte
	{
		en_none								= 0x00,
		en_global							= 0x01,
		en_local							= 0x02,	/* 분할 노광 방식에서 혼합된 Mark (Global & Local)*/
		en_mixed							= 0x03,	/* Global + Local 혼합 의미. (개수 구할 때 필요) */
	}

	/* Job Management - Get Registration Status */
	public enum ENG_PDRS /* panel_data_registration_status */ : Byte
	{
		en_ok													= 1,
		en_missing_fiducials									= 2,
		en_missing_registration									= 3,
		en_generate_not_run										= 4,
		en_strip_height_not_set									= 5,
		en_strip_width_not_set									= 6,
		en_number_of_stripes_not_set							= 7,
		en_overlap_not_set										= 8,
		en_scaling_point_distance_not_set						= 9,
		en_corner_shifts_outside_limits							= 10,
		en_number_of_reg_and_fid_not_equal						= 11,
		en_unable_to_interpret_fiducials						= 12,
		en_unable_to_interpret_registrations					= 13,
		en_invalid_number_of_scaling_points						= 14,
		en_number_of_stripes_matrix_size_mismatch				= 15,
		en_missing_offsets										= 16,
		en_missing_strip_flip_list								= 17,
		en_missing_xbase_step_size								= 18,
		en_rotation_outside_limits								= 19,
		en_scaling_outside_limits								= 20,
		en_offset_outside_limits								= 21,
		en_number_of_fixed_paramin_correct						= 22,
		en_calc_rotation_out_of_limits							= 23,
		en_calc_scale_out_of_limits								= 24,
		en_calc_offset_out_of_limits							= 25,
		en_fixed_rotation_param_out_of_limits					= 26,
		en_fixed_scale_param_out_of_limits						= 27,
		en_fixed_offset_paramo_ut_of_limits						= 28,
		en_warp_outside_limits_in_zone							= 29,
		en_warp_outside_limits_outside_zone						= 30,
		en_warp_unable_to_calculate_warp_zone					= 31,
		en_warp_undefined_local_zones_fixed_param_not_allowed	= 32,
		en_warp_fixed_param_not_allowed_with_shared_zones		= 33,
		en_warp_outside_fpga_limits								= 34,
		en_internal_error										= 99,
	}

	/* Coordinate Type Code */
	public enum ENG_PDCT /* panel_data_coord_type */ : Byte
	{
		en_dynamic_fiducial_global			= 0x00,
		en_dynamic_fiducial_local			= 0x01,
		en_registration_point				= 0x02,
	}

	/* 주요 데이터 처리 방법 플래그 */
	public enum ENG_FDPR /* flag_data_packet_recv */ : UInt16
	{
		en_luria_none							= 0x0000,
		/* Luria - Machine Config */
		en_mc_total_photo_heads					= 0xA001,
		en_mc_photo_head_ip_addr				= 0xA002,
		en_mc_photo_head_pitch					= 0xA003,
		en_mc_photo_head_rotate					= 0xA005,
		en_mc_parallel_ogram_motion_adjust		= 0xA006,
		en_mc_scroll_rate						= 0xA007,	
		en_mc_motion_control_type				= 0xA008,	
		en_mc_motion_control_ipaddr				= 0xA009,
		en_mc_x_correction_table				= 0xA00C,
		en_mc_table_position_limits				= 0xA00D,
		en_mc_table_exposure_start_pos			= 0xA00E,
		en_mc_max_y_motion_speed				= 0xA00F,
		en_mc_x_motion_speed					= 0xA010,
		en_mc_theta_position					= 0xA011,
		en_mc_y_acceleration					= 0xA012,
		en_mc_y_acceleration_distance			= 0xA013,	
		en_mc_hysteresis_type1					= 0xA014,
		en_mc_hysteresis_type2					= 0xA015,
		en_mc_photo_head_offset					= 0xA016,
		en_mc_table_print_direction				= 0xA017,
		en_mc_active_table						= 0xA018,
		en_mc_get_table_motion_start_position	= 0xA019,	
		en_mc_emulate_motor_controller			= 0xA01A,
		en_mc_emulate_photo_heads				= 0xA01B,	
		en_mc_emulate_triggers					= 0xA01C,
		en_mc_debug_print						= 0xA01D,	
		en_mc_print_simulation_out_dir			= 0xA01E,
		en_mc_y_correction_table				= 0xA020,
		en_mc_linear_z_drive_settings			= 0xA021,	
		en_mc_hysteresis_type_3					= 0xA022,
		en_mc_z_drive_ip_addr					= 0xA023,	
		en_mc_xy_drive_id						= 0xA024,	
		en_mc_product_id						= 0xA025,
		en_mc_artwork_complexity				= 0xA026,
		en_mc_z_drive_type						= 0xA027,
		en_mc_over_pressure_mode				= 0xA028,
		en_mc_depth_of_focus					= 0xA029,
		en_mc_extra_long_stripes				= 0xA02A,
		en_mc_mtc_mode							= 0xA02B,
		en_mc_use_ethercat_for_af				= 0xA02C,
		en_mc_spx_level							= 0xA02D,

		/* Luria - Job Management */
		en_root_directory						= 0xA101,
		en_get_job_list							= 0xA102,
		en_selected_job							= 0xA105,
		en_get_selected_job_load_state			= 0xA107,
		en_get_fiducials						= 0xA108,
		en_get_parameters						= 0xA109,
		en_get_panel_data_dcode_list			= 0xA10A,
		en_get_panel_data_info					= 0xA10B,
		en_get_max_jobs							= 0xA10C,

		/* Luria - Panel Preparation */
		en_dynamic_fiducials					= 0xA201,
		en_registration_points					= 0xA202,
		en_global_transformation_receipe		= 0xA203,
		en_global_fixed_rotation				= 0xA204,
		en_global_fixed_scaling					= 0xA205,
		en_global_fixed_offset					= 0xA206,
		en_local_zone_geometry					= 0xA207,
		en_get_number_of_local_zones			= 0xA208,
		en_get_transformation_params			= 0xA209,
		en_get_registration_status				= 0xA20C,
		en_panel_data_serial_number				= 0xA20D,
		en_panel_data							= 0xA20E,
		en_use_shared_local_zones				= 0xA20F,
		en_local_transformation_recipe			= 0xA210,
		en_local_fixed_rotation					= 0xA211,
		en_local_fixed_scaling					= 0xA212,
		en_local_fixed_offset					= 0xA213,
		en_global_rectangle_lock				= 0xA214,
		en_get_warp_of_limits_coordinates		= 0xA216,

		/* Luria - Exposure */
		en_led_amplitude						= 0xA301,
		en_scroll_step_size						= 0xA302,
		en_frame_rate_factor					= 0xA303,
		en_led_duty_cycle						= 0xA304,
		en_get_exposure_state					= 0xA308,
		en_get_exposure_speed					= 0xA309,
		en_enable_af_copy_last_stripe			= 0xA30B,
		en_get_number_of_light_sources			= 0xA30E,
		en_get_light_source_status				= 0xA30F,
		en_disable_af_first_strip				= 0xA310,
		en_af_sensor							= 0xA311,
		en_af_sensor_measure_laser_pwm			= 0xA313,

		/* Luria - Direct Photohead */
		en_luria_direct_ph						= 0xA401,

		/* Luria - System Config */
		en_get_system_status					= 0xA701,
		en_set_dummy_cmd						= 0xA704,
		en_get_smart_data						= 0xA706,

		/* Trigger */
		en_trig_recv_data						= 0xB001,		/* Send / Recv Flag */
	}

	/* Exposoure State */
	public enum ENG_LPES /* luria_printing_exposure_state */ : Byte
	{
		en_not_defined							= 0x00,

		en_idle									= 0x01,

		en_preprint_running						= 0x11,
		en_preprint_success						= 0x12,
		en_preprint_failed						= 0x1f,

		en_print_running						= 0x21,
		en_print_success						= 0x22,
		en_print_failed							= 0x2f,
	#if (false)
		/* Not used (Only Here !!!) */
		en_simul_running						= 0x31,
		en_simul_success						= 0x32,
		en_simul_failed							= 0x3f,
	#endif
		en_abort_in_progress					= 0xA1,
		en_abort_success						= 0xA2,
		en_abort_failed							= 0xAf,
	}

	/* Selected Job Load State */
	public enum ENG_LSLS /* luria_select_load_state */ : Byte
	{
		en_load_none							= 0x00,
		en_load_not_started						= 0x01,
		en_loading								= 0x02,
		en_load_completed						= 0x03,
		en_load_failed							= 0x04,
	}
}

/* --------------------------------------------------------------------------------------------- */
/*                                         Engine -> PLC                                         */
/* --------------------------------------------------------------------------------------------- */

namespace TGen2i.Enum.PLC
{
	/* ---------------------------------------------------------------------------- */
	/* PLC Address 정의 (장비 마다 주소 값이 다를 수 있음. 16 진수로 구성 : _____)  */
	/* 즉, 앞의 3 자리는 WORD 주소 (0000 ~ FFFF) / 마지막 1 자리는 BIT 주소 (0 ~ F) */
	/* ---------------------------------------------------------------------------- */
	public enum ENG_PIOA /*__en_plc_input_output_address__*/ : UInt16
	{
		/* ----------------------------------------------- */
		/*                    Read Area                    */
		/* ----------------------------------------------- */

		/* 치명적인 알람 영역 (0x44C ~ 0x44E) */
		en_spd_fail_alarm								= 0x44C0,
		en_linear_x_axis_power_line_cp100				= 0x44C1,
		en_linear_y_axis_power_line_cp101				= 0x44C2,
		en_linear_z1_z3_axis_smps_power_fail			= 0x44C3,
		en_linear_z1_z3_power_cp103_cp105				= 0x44C4,
		en_optics_1_smps_power_fail						= 0x44C5,
		en_optics_2_smps_power_fail						= 0x44C6,
		en_optics_1_2_power_cp107_cp109					= 0x44C7,
		en_robot_prealigner_power_cp110_cp112			= 0x44C8,
		en_all_sd2s_control_power_cp116					= 0x44C9,
		en_mc2_power_cp117								= 0x44CA,
		en_lift_pin_axis_cp119							= 0x44CB,
		en_vision_system_power_cp122					= 0x44CC,
		en_efu_modul_power_cp123						= 0x44CD,
		en_server_pc_1_2_power_cp124_cp125				= 0x44CE,
		en_front_safety_cover_1							= 0x44CF,

		en_front_safety_cover_2							= 0x44D0,
		en_front_safety_cover_3							= 0x44D1,
		en_front_safety_cover_4							= 0x44D2,
		en_rear_saferty_cover_1							= 0x44D3,
		en_rear_saferty_cover_2							= 0x44D4,
		en_rear_saferty_cover_3							= 0x44D5,
		en_rear_saferty_cover_4							= 0x44D6,
		en_rear_saferty_cover_5							= 0x44D7,
		en_utility_box_water_leak_sensor				= 0x44D8,
		en_ph_water_leak_sensor_1						= 0x44D9,
		en_ph_water_leak_sensor_2						= 0x44DA,
		en_water_flow_low_alarm							= 0x44DB,
		en_water_flow_high_alarm						= 0x44DC,
		en_water_pressure_low_alarm						= 0x44DD,
		en_water_pressure_high_alarm					= 0x44DE,
		en_air_pressure_low_alarm						= 0x44DF,

		en_halogen_ring_light_burn_out_alarm			= 0x44E0,
		en_halogen_coaxial_light_burn_out_alarm			= 0x44E1,
		en_lift_pin_x_axis_drive_alarm					= 0x44E2,
		en_lift_pin_x_axis_operation_alarm				= 0x44E3,
		en_lift_pin_x_axis_home_request_alarm			= 0x44E4,
		en_lift_pin_z_axis_drive_alarm					= 0x44E5,
		en_lift_pin_z_axis_operation_alarm				= 0x44E6,
		en_lift_pin_z_axis_home_request_alarm			= 0x44E7,
		en_bsa_z_axis_operation_alarm					= 0x44E8,
		en_bsa_z_axis_home_request_alarm				= 0x44E9,
		en_led_lamp_controller_power_cp127				= 0x44EA,
		en_lift_pin_x_not_move_alarm					= 0x44EB,
		en_lift_pin_z_not_move_alarm					= 0x44EC,
		en_lift_pin_x_position_count_alarm				= 0x44ED,
		en_lift_pin_z_position_count_alarm				= 0x44EE,
		en_spare_bit_44EF								= 0x44EF,

		en_spare_word_44F0								= 0x44F0,

		/* 경고 알람 영역 (0x450 ~ 0x451) */

		en_pc_rack_upper_out_fan_alarm_1				= 0x4500,
		en_pc_rack_upper_out_fan_alarm_2				= 0x4501,
		en_pc_rack_bottom_out_fan_alarm_1				= 0x4502,
		en_pc_rack_bottom_out_fan_alarm_2				= 0x4503,
		en_power_box_in_fan_alarm_1						= 0x4504,
		en_power_box_in_fan_alarm_2						= 0x4505,
		en_power_box_in_fan_alarm_3						= 0x4506,
		en_power_box_in_fan_alarm_4						= 0x4507,
		en_power_box_out_fan_alarm_1					= 0x4508,
		en_power_box_out_fan_alarm_2					= 0x4509,
		en_power_box_out_fan_alarm_3					= 0x450A,
		en_power_box_out_fan_alarm_4					= 0x450B,
		en_spare_bit_450C								= 0x450C,
		en_spare_bit_450D								= 0x450D,
		en_spare_bit_450E								= 0x450E,
		en_spare_bit_450F								= 0x450F,

		en_spare_word_4510								= 0x4510,

		/* 일반적인 데이터 영역 (0x452 ~ 0x45F) */

		en_auto_maint_mode								= 0x4520,
		en_wafer_chuck_vacuum_status					= 0x4521,
		en_wafer_chuck_atm_status						= 0x4522,
		en_robot_vacuum_status							= 0x4523,
		en_robot_atm_status								= 0x4524,
		en_aligner_vacuum_status						= 0x4525,
		en_aligner_atm_status							= 0x4526,
		en_safety_reset_possible_status					= 0x4527,
		en_spare_bit_4528								= 0x4528,
		en_spare_bit_4529								= 0x4529,
		en_stage_wafer_detact							= 0x452A,
		en_buffer_wafer_in_detact						= 0x452B,
		en_buffer_wafer_out_detact						= 0x452C,
		en_robot_output_signal_1_ex_alarm				= 0x452D,
		en_robot_output_signal_2						= 0x452E,
		en_robot_output_signal_3						= 0x452F,

		en_robot_output_signal_4						= 0x4530,
		en_robot_output_signal_5						= 0x4531,
		en_robot_output_signal_6						= 0x4532,
		en_robot_output_signal_7						= 0x4533,
		en_robot_output_signal_8						= 0x4534,
		en_spare_bit_4535								= 0x4535,
		en_signal_tower_green_lamp						= 0x4536,
		en_signal_tower_yellow_lamp_flashing			= 0x4537,
		en_signal_tower_red_lamp_flashing				= 0x4538,
		en_signal_tower_buzzer_1						= 0x4539,
		en_spare_bit_453A								= 0x453A,
		en_spare_bit_453B								= 0x453B,
		en_spare_bit_453C								= 0x453C,
		en_spare_bit_453D								= 0x453D,
		en_spare_bit_453E								= 0x453E,
		en_spare_bit_453F								= 0x453F,

		en_spare_word_4540								= 0x4540,

		/* TEL Interface Input (Track -> Exposure) */

		en_trk_ink_track_inline							= 0x4550,
		en_trk_rdy_track_ready							= 0x4551,
		en_trk_snd_track_send							= 0x4552,
		en_trk_lend_track_end_of_lot					= 0x4553,
		en_trk_reserve_1								= 0x4554,
		en_trk_reserve_2								= 0x4555,
		en_trk_reserve_3								= 0x4556,
		en_trk_reserve_4								= 0x4557,
		en_trk_reserve_5								= 0x4558,
		en_spare_bit_4559								= 0x4559,
		en_spare_bit_455A								= 0x455A,
		en_spare_bit_455B								= 0x455B,
		en_spare_bit_455C								= 0x455C,
		en_spare_bit_455D								= 0x455D,
		en_spare_bit_455E								= 0x455E,
		en_spare_bit_455F								= 0x455F,

		/* Analog Input Area */
		en_differential_pressure						= 0x4560,
		en_monitoring_temperature						= 0x4570,
		en_air_pressure									= 0x4580,
		en_spare_word_4590								= 0x4590,
		en_power_box_temperature						= 0x45A0,
		en_pc_rack_upper_monitoring_temperature			= 0x45B0,
		en_pc_rack_bottom_monitoring_temperature		= 0x45C0,
		en_spare_word_45D0								= 0x45D0,
		en_spare_word_45E0								= 0x45E0,
		en_spare_word_45F0								= 0x45F0,

		/* Lift Pin [ X ] Axis (Lamp) Bit */
		en_lift_pin_xi_axis_status						= 0x4600,
		en_lift_pin_xi_axis_p_limit_position			= 0x4601,
		en_lift_pin_xi_axis_n_limit_psition				= 0x4602,
		en_lift_pin_xi_axis_position					= 0x4603,
		en_lift_pin_xi_axis_loading_position			= 0x4604,
		en_lift_pin_xi_axis_safety_position				= 0x4605,
		en_spare_bit_4606								= 0x4606,
		en_spare_bit_4607								= 0x4607,
		en_spare_bit_4608								= 0x4608,
		en_spare_bit_4609								= 0x4609,
		en_spare_bit_460A								= 0x460A,
		en_spare_bit_460B								= 0x460B,
		en_spare_bit_460C								= 0x460C,
		en_spare_bit_460D								= 0x460D,
		en_spare_bit_460E								= 0x460E,
		en_spare_bit_460F								= 0x460F,

		/* Lift Pin [ Z ] Axis (Lamp) Bit */
		en_lift_pin_zi_axis_status						= 0x4610,
		en_lift_pin_zi_axis_p_limit_position			= 0x4611,
		en_lift_pin_zi_axis_n_limit_psition				= 0x4612,
		en_lift_pin_zi_axis_position					= 0x4613,
		en_lift_pin_zi_axis_loading_position			= 0x4614,
		en_lift_pin_zi_axis_safety_position				= 0x4615,
		en_spare_bit_4616								= 0x4616,
		en_spare_bit_4617								= 0x4617,
		en_spare_bit_4618								= 0x4618,
		en_spare_bit_4619								= 0x4619,
		en_spare_bit_461A								= 0x461A,
		en_spare_bit_461B								= 0x461B,
		en_spare_bit_461C								= 0x461C,
		en_spare_bit_461D								= 0x461D,
		en_spare_bit_461E								= 0x461E,
		en_spare_bit_461F								= 0x461F,

		/* BSA Camera [ Z ] Axis (Lamp) Bit */
		en_bsa_zi_axis_status							= 0x4620,
		en_bsa_zi_axis_p_limit_position					= 0x4621,
		en_bsa_zi_axis_n_limit_psition					= 0x4622,
		en_bsa_zi_axis_position							= 0x4623,
		en_spare_bit_4624								= 0x4624,
		en_spare_bit_4625								= 0x4625,
		en_spare_bit_4626								= 0x4626,
		en_spare_bit_4627								= 0x4627,
		en_spare_bit_4628								= 0x4628,
		en_spare_bit_4629								= 0x4629,
		en_spare_bit_462A								= 0x462A,
		en_spare_bit_462B								= 0x462B,
		en_spare_bit_462C								= 0x462C,
		en_spare_bit_462D								= 0x462D,
		en_spare_bit_462E								= 0x462E,
		en_spare_bit_462F								= 0x462F,

		/* Lift Pin [ X ] Axis Word */
		en_lift_pin_xi_axis_currnt_position_display		= 0x4630, /* DWord */
		en_lift_pin_xi_axis_alarm_code					= 0x4650,
		/* Lift_Pin [ Z ] Axis_Word */
		en_lift_pin_zi_axis_currnt_position_display		= 0x4660, /* DWord */
		en_lift_pin_zi_axis_alarm_code					= 0x4680,
		/* BSA Camera [ Z ] Axis Word */
		en_bsa_camera_zi_axis_currnt_position_display	= 0x4690,  /* DWord */
		en_bsa_camera_zi_axis_alarm_code				= 0x46B0,

		/* ----------------------------------------------- */
		/*                    Write Area                   */
		/* ----------------------------------------------- */

		/* Digitial Output Bit */
		en_safety_reset_cmd								= 0x47E0,
		en_tower_green_lamp_cmd							= 0x47E1,
		en_tower_yellow_lamp_flashing_cmd				= 0x47E2,
		en_tower_red_lamp_flashing_cmd					= 0x47E3,
		en_tower_buzzer_1_cmd							= 0x47E4,
		en_z1_axis_air_cooling_on_off					= 0x47E5,
		en_z2_axis_air_cooling_on_off					= 0x47E6,
		en_z3_axis_air_cooling_on_off					= 0x47E7,
		en_z1_axis_cylinder_on_off						= 0x47E8,
		en_z2_axis_cylinder_on_off						= 0x47E9,
		en_z3_axis_cylinder_on_off						= 0x47EA,
		en_wafer_chuck_vacuum_on_off					= 0x47EB,
		en_wafer_chuck_exhaust_on_off					= 0x47EC,
		en_robot_vacuumuum_on_off						= 0x47ED,
		en_aligner_vacuum_on_off						= 0x47EE,
		en_photohead_1_power_off_cmd					= 0x47EF,

		en_photohead_2_power_off_cmd					= 0x47F0,
		en_illumination_sensor_pwr_on_off				= 0x47F1,
		en_halogen_ring_light_power_on					= 0x47F2,
		en_halogen_coaxial_light_power_on				= 0x47F3,
		en_spare_bit_47F4								= 0x47F4,
		en_spare_bit_47F5								= 0x47F5,
		en_robot_input_signal_01						= 0x47F6,
		en_robot_input_signal_02						= 0x47F7,
		en_robot_input_signal_03						= 0x47F8,
		en_robot_input_signal_04						= 0x47F9,
		en_robot_input_signal_05						= 0x47FA,
		en_robot_input_signal_06						= 0x47FB,
		en_robot_input_signal_07						= 0x47FC,
		en_robot_input_signal_08						= 0x47FD,
		en_robot_input_signal_09						= 0x47FE,
		en_robot_input_signal_10						= 0x47FF,

		en_xy_stage_move_prohibit						= 0x4800,
		en_xy_stage_move_prohibit_release				= 0x4801,
		en_spare_bit_4802								= 0x4802,
		en_spare_bit_4803								= 0x4803,
		en_spare_bit_4804								= 0x4804,
		en_spare_bit_4805								= 0x4805,
		en_spare_bit_4806								= 0x4806,
		en_spare_bit_4807								= 0x4807,
		en_spare_bit_4808								= 0x4808,
		en_spare_bit_4809								= 0x4809,
		en_spare_bit_480A								= 0x480A,
		en_spare_bit_480B								= 0x480B,
		en_spare_bit_480C								= 0x480C,
		en_spare_bit_480D								= 0x480D,
		en_spare_bit_480E								= 0x480E,
		en_spare_bit_480F								= 0x480F,

		/* TEL Interface Output (Exposre -> Track) */
		en_exp_ink_exposure_inline						= 0x4810,
		en_exp_rej_exposure_reject						= 0x4811,
		en_exp_snd_exposure_send_to_ready				= 0x4812,
		en_exp_rdy_exposure_receive_to_ready			= 0x4813,
		en_exp_reserve_1								= 0x4814,
		en_exp_reserve_2								= 0x4815,
		en_exp_reserve_3								= 0x4816,
		en_exp_reserve_4								= 0x4817,
		en_exp_reserve_5								= 0x4818,
		en_spare_B39									= 0x4819,
		en_spare_B3A									= 0x481A,
		en_spare_B3B									= 0x481B,
		en_spare_B3C									= 0x481C,
		en_spare_B3D									= 0x481D,
		en_spare_B3E									= 0x481E,
		en_spare_B3F									= 0x481F,

		/* Analog Write Area */
		en_halogen_ring_light_power						= 0x4820,
		en_halogen_coaxial_light_power					= 0x4830,

		/* PC Live_Word */
		en_pc_live_check								= 0x4840,
		en_spare_word_4850								= 0x4850,
		en_spare_word_4860								= 0x4860,
		en_spare_word_4870								= 0x4870,
		/* Lift Pin [ X ] Axis (SW) Bit */
		en_lift_pin_xo_axis_enable_disable				= 0x4880,
		en_lift_pin_xo_axis_reset						= 0x4881,
		en_lift_pin_xo_axis_stop						= 0x4882,
		en_spare										= 0x4883,
		en_lift_pin_xo_axis_jog_plus_move				= 0x4884,
		en_lift_pin_xo_axis_jog_minus_move				= 0x4885,
		en_lift_pin_xo_axis_home_position				= 0x4886,
		en_lift_pin_xo_axis_position					= 0x4887,
		en_lift_pin_xo_axis_drive_alarm_reset			= 0x4888,
		en_spare_4889									= 0x4889,
		en_spare_488A									= 0x488A,
		en_spare_488B									= 0x488B,
		en_spare_488C									= 0x488C,
		en_spare_488D									= 0x488D,
		en_spare_488E									= 0x488E,
		en_spare_488F									= 0x488F,
		/* Lift Pin [ Z ] Axis (SW) Bit */
		en_lift_pin_zo_axis_enable_disable				= 0x4890,
		en_lift_pin_zo_axis_reset						= 0x4891,
		en_lift_pin_zo_axis_stop						= 0x4892,
		en_spare_4893									= 0x4893,
		en_lift_pin_zo_axis_jog_plus_move				= 0x4894,
		en_lift_pin_zo_axis_jog_minus_move				= 0x4895,
		en_lift_pin_zo_axis_home_position				= 0x4896,
		en_lift_pin_zo_axis_position					= 0x4897,
		en_lift_pin_zo_axis_dreve_alarm_reset			= 0x4898,
		en_spare_4899									= 0x4899,
		en_spare_489A									= 0x489A,
		en_spare_489B									= 0x489B,
		en_spare_489C									= 0x489C,
		en_spare_489D									= 0x489D,
		en_spare_489E									= 0x489E,
		en_spare_489F									= 0x489F,
		/* BSA Camera [ Z ] Axis (SW) Bit */
		en_bsa_camera_zo_axis_enable_disable			= 0x48A0,
		en_bsa_camera_zo_axis_reset						= 0x48A1,
		en_bsa_camera_zo_axis_stop						= 0x48A2,
		en_spare_48A3									= 0x48A3,
		en_bsa_camera_zo_axis_jog_plus_move				= 0x48A4,
		en_bsa_camera_zo_axis_jog_minus_move			= 0x48A5,
		en_bsa_camera_zo_axis_home_position				= 0x48A6,
		en_bsa_camera_zo_axis_position					= 0x48A7,
		en_spare_48A8									= 0x48A8,
		en_spare_48A9									= 0x48A9,
		en_spare_48AA									= 0x48AA,
		en_spare_48AB									= 0x48AB,
		en_spare_48AC									= 0x48AC,
		en_spare_48AD									= 0x48AD,
		en_spare_48AE									= 0x48AE,
		en_spare_48AF									= 0x48AF,
		/* Lift Pin [ X ] Axis (PC->PLC) Word */
		en_lift_pin_xo_axis_jog_speed_setting			= 0x48B0, /* DWord */
		en_lift_pin_xo_axis_position_speed_setting		= 0x48D0, /* DWord */
		en_lift_pin_xo_axis_position_setting			= 0x48F0, /* DWord */
		/* Lift Pin [ Z ] Axis (PC->PLC) Word */
		en_lift_pin_zo_axis_jog_speed_setting			= 0x4910, /* DWord */
		en_lift_pin_zo_axis_position_speed_setting		= 0x4930, /* DWord */
		en_lift_pin_zo_axis_position_setting			= 0x4950, /* DWord */
		/* BSA Camera [ Z ] Axis (PC->PLC) Word */
		en_bsa_camera_zo_axis_jog_speed_setting			= 0x4970,  /* DWord */
		en_bsa_camera_zo_axis_position_speed_setting	= 0x4990,  /* DWord */
		en_bsa_camera_zo_axis_position_setting			= 0x49B0,  /* DWord */
		/* because of "for loop" */
		END,
	};

	/*_PIO_구분_값_*/
	public enum ENG_PIOT /* pin_input_output_type */ : Byte
	{
		en_scanner	= 0x00,
		en_track	= 0x01,
	};

	/* Halogen Light Type */
	public enum ENG_HLPT /* halogen_light_power_type */ : Byte
	{
		en_ring		= 0x00,
		en_coaxial	= 0x01,
	}

	/* Tower Lamp Color Index */
	public enum ENG_TLCI /* tower_lamp_color_index */ : Byte
	{
		en_green	= 0x01,
		en_yellow	= 0x02,
		en_red		= 0x03,
		en_all		= 0x04,
	}

	/* Linked Litho. Exposure -> Track (Reference to PLC Document) */
	public enum ENG_LLET /* linked_litho_exposure_track */ : Byte
	{
		en_exp_ink	= 0x01,
		en_exp_rej	= 0x02,
		en_exp_snd	= 0x03,
		en_exp_rdy	= 0x04,
	}

	/* Lift Pin Axis Type */
	public enum ENG_LPAT /* lift_pin_axis_type */ : Byte
	{
		en_pin_x_axis	= 0x00,
		en_pin_z_axis	= 0x01,
		en_bsa_z_axis	= 0x02,
	}

	/* Lift Pin Axis Control */
	public enum ENG_LPAC /* lift_pin_axis_control */ : Byte
	{
		en_mode		= 0x01,	/* Enable or Disable */
		en_reset	= 0x02,
		en_stop		= 0x03,
	}

	/* Lift Pin Axis Action */
	public enum ENG_LPAA /* lift_pin_axis_action */ : Byte
	{
		en_jog_move_plus	= 0x01,	/* Jog + Move */
		en_jog_move_minus	= 0x02,	/* Jog - Move */
		en_home_position	= 0x03,	/* Home Position */
		en_move_position	= 0x04,	/* Axis Position */
		en_alarm_reset		= 0x05,	/* Axis Drive Alarm Reset */
	}
}

/* --------------------------------------------------------------------------------------------- */
/*                                       Engine -> Milara                                        */
/* --------------------------------------------------------------------------------------------- */

namespace TGen2i.Enum.Milara
{
	/* Milara Axis Direction */
	public enum ENG_MAMD /*__en_milara_axis_move_direction__*/ : Byte
	{
		amd_forward			= 0x00,	/* 정방향 */
		amd_reverse			= 0x01,	/* 역방향 */
	};

	/* Milara Axis Left / Right */
	public enum ENG_MALR /*__en_milara_axis_left_right__*/ : Byte
	{
		alr_left			= 0x00,	/* 왼  쪽 */
		alr_right			= 0x01,	/* 오른쪽 */
	};

	/* Milara Axis Speed Type (Acceleration or Deceleration) */
	public enum ENG_MAST /*__en_milara_axis_speed_type__*/ : Byte
	{
		ast_acl				= 0x00,	/* 가속력: inch / sec^2 or degree / sec^2 */
		ast_dcl				= 0x01,	/* 감속력: inch / sec^2 or degree / sec^2 */
	};

	/* Response status value for sending data */
	public enum ENG_MSAS/*__en_milara_send_ack_status__*/ : Byte
	{
		none				= 0xff,		/* Not defined */
		fail				= 0x00,		/* Failed */
		succ				= 0x01,		/* Success */
		bell				= 0x02,		/* If the host sends data to Prealigner or Robot 2 times in a row, BELL (0x07 character is received) from Prealigner or Robot */
	};
}

/* --------------------------------------------------------------------------------------------- */
/*                                         Engine -> MPA                                         */
/* --------------------------------------------------------------------------------------------- */

namespace TGen2i.Enum.MPA
{
	/* Axis Name */
	public enum ENG_PANC /*__en_prealigner_axis_name_code__*/ : Byte
	{
		pac_t				= 0x00,		/* T : Rotation */
		pac_r				= 0x01,		/* R : Horizontal */
		pac_z				= 0x02,		/* Z : Vertical */
		pac_a				= 0x03,		/* T|R|Z 즉, All axes */
	};

	/* Prealigner Axis position movement method (Absolute or Reference) */
	public enum ENG_PAMT /*__en_prealigner_axis_move_type__*/ : Byte
	{
		amt_abolute			= 0x00,
		amt_reference		= 0x01,

	};

	/* Prealigner Axis의 Sensor Control (ex> Servo OFF or ON, Vacuum OFF or ON, etc) */
	public enum ENG_PSOO /*__en_prealigner_sensor_on_off__*/ : Byte
	{
		soo_off				= 0x00,		/* Servo Control OFF --> You may need to hom */
		soo_on				= 0x01,		/* Servo Control ON */
	};

	/* The type (place) where Prealinger's Wafer will be placed (Chuck or Pins) */
	public enum ENG_PWPT /*__en_prealigner_wafer_place_type__ */: Byte
	{
		wpt_pins			= 0x00,
		wpt_chuck			= 0x01,
	};

	/* Whether Prealinger's Wafer Alignment search succeeded */
	public enum ENG_PASR /*__en_prealigner_alignment_search_result__*/ : Byte
	{
		asr_fail			= 0x01,
		asr_succ			= 0x00,
	};

	/* Prealinger's Wafer Notch Separation Type */
	public enum ENG_PNTI /*__en_prealigner_notch_type_info__*/ : Byte
	{
		nti_depth			= 0x00,		/* Notch Depth (피자 조각 모양) */
		nti_width			= 0x01,		/* Notch Width (호의 길이) */
	};

	/* Min or Max division value */
	public enum ENG_MMMR /*__en_milara_min_max_range__*/ : Byte
	{
		mmr_min				= 0x00,
		mmr_max				= 0x01,
	};

	/* Wafer Size */
	public enum ENG_MWKT /*__en_milara_wafer_kind_type__*/ : Byte
	{
		wkt_2				= 0x02,		/* Wafer Size :   2 inch */
		wkt_3				= 0x03,		/* Wafer Size :   3 inch */
		wkt_4				= 0x04,		/* Wafer Size :   4 inch */
		wkt_5				= 0x05,		/* Wafer Size :   5 inch */
		wkt_6				= 0x06,		/* Wafer Size :   6 inch */
		wkt_8				= 0x08,		/* Wafer Size :   8 inch */
		wkt_12				= 0x0C,		/* Wafer Size :  12 inch */
		wkt_18				= 0x12,		/* Wafer Size :  18 inch */
	};

	/* The Wafer Size Auto Detection of Prealigner */
	public enum ENG_PWSD /*__en_prealigner_wafer_size_detection__*/ : Byte
	{
		wsd_manual			= 0x00,		/* Not performed the Automatic Wafer Size Detection */
		wsd_auto			= 0x01,		/* Performed the Automatic Wafer Size Detection */
	};

	/* Whether related flags such as behavior/environment settings are enabled */
	public enum ENG_MFED /*__en_milara_flag_enable_disable__*/ : Byte
	{
		fed_disable			= 0x00,		/* Not performed the Automatic Wafer Size Detection */
		fed_enable			= 0x01,		/* Performed the Automatic Wafer Size Detection */
	};

	/* Whether the material of the wafer is transparent or not */
	public enum ENG_PWGO /*__en_prealigner_wafer_galss_object__*/ : Byte
	{
		wgo_non_transparent	= 0x00,	/* 투명하지 않은 재질의 Wafer */
		wgo_transparent		= 0x01,	/*  투명한 Wafer 재질 */
	};

	/* The Load Up/Down Position Flag of Wafer */
	public enum ENG_PWLP /*__en_prealigner_wafer_load_position__*/ : Byte
	{
		wlp_load_down		= 0x00,		/* wafer loading position: Down */
		wlp_load_up			= 0x01,		/* wafer loading position: Up */
	};

	/* The Sampling Mode of Wafer */
	public enum ENG_PRSM /*__en_prealigner_rotation_sample_mode__*/ : Byte
	{
		rsm_one_third_0		= 0x00,		/* Encoder Lines의 1/3배 (아주 덜 정밀하지만, 아주 빠른 정렬 시간) */
		rsm_half_1			= 0x00,		/* Encoder Lines의 1/2배 (Half) (덜 정밀하지만, 정렬 시간 빠름) */
		rsm_default_2		= 0x00,		/* 샘플링 개수는 Chuck (T-Axis) Encoder에서 Encoder Lines의 수와 동일 (기본 값) */
		rsm_twice_3			= 0x00,		/* Encoder Lines의 2배 (더 정밀하지만, 정렬 시간 오래 걸림) */
	};

	/* Global Wafer Type */
	public enum ENG_PGWT /*__en_prealinger_global_wafer_type__*/ : SByte
	{
		gwt_marking_notch_wafer	= -2,
		gwt_none_notch_wafer	= -1,
		gwt_one_notch_wafer		= 0,
		gwt_more_notch_wafer	= 1,
		gwt_square_substrate	= 2,
	};

	public enum ENG_PSCC /*__en_prealigner_serial_command_code__*/ : UInt16
	{
		psc_none				= 0xffff,	/* None */

		/* 1. Status and Information Commands */
		psc_mcpo				= 0x1010,	/* Read current position */
		psc_diag				= 0x1020,	/* Display current/last error diagnostics */
		psc_est					= 0x1030,	/* Extended error status */
		psc_per					= 0x1040,	/* Display reason for previous error */
		psc_inf					= 0x1050,	/* Set/Display inform mode */
		psc_rhs					= 0x1060,	/* Read home switches state */
		psc_sta					= 0x1070,	/* Command execution status */
		psc_ver					= 0x1080,	/* Read firmware version number */

		/* 2. Axis Parameters Set and Retrieve */
		psc__ch					= 0x2010,	/* Set/Display customized home position */
		psc__cl					= 0x2020,	/* Set/Display current limit */
		psc__el					= 0x2030,	/* Set/Display position error limit */
		psc__fl					= 0x2040,	/* Set/Display forward direction software limit */
		psc__ho					= 0x2050,	/* Set/Display home offset */
		psc__rl					= 0x20a0,	/* Set/Display reverse direction software limit */
		psc_acl					= 0x20b0,	/* Set/Display axis acceleration */
		psc_ajk					= 0x20c0,	/* Set/Display axis acceleration jerk */
		psc_dcl					= 0x20d0,	/* Set/Display axis acceleration */
		psc_djk					= 0x20e0,	/* Set/Display axis deceleration */
		psc_pro					= 0x20f0,	/* Set/Display axis velocity profile mode */
		psc_spd					= 0x2100,	/* Set/Display speed */

		/* 3.Basic Axis Control */
		psc_hom					= 0x3030,	/* Start homing the prealigner */
		psc_mth					= 0x3040,	/* Move to home position */
		psc_mva					= 0x3050,	/* Move to absolute position */
		psc_mvr					= 0x3060,	/* Move to relative position */

		psc_sof					= 0x3080,	/* Turn axis Servo OFF */
		psc_son					= 0x3090,	/* Turn axis Servo ON */
		psc_stp					= 0x30a0,	/* Stop motion */
		psc_wmc					= 0x30b0,	/* Wait for motion completion */

		/* 4.Alignment Commands */
		psc__ao2				= 0x4010,	/* Set/Display align offset twice */
		psc__aof				= 0x4020,	/* Set/Display acceptable offset */
		psc__asz				= 0x4030,	/* Set/Display automatic wafer Size detection */
		psc__autoscanagain		= 0x4040,	/* Set/Display align again in case of CCD data error */

		psc__finalvacc			= 0x40c0,	/* Set/Display vacuum on chuck after BAL state */

		psc_mglm				= 0x40f0,	/* Set/Display glass mode operation */

		psc__ld					= 0x4140,	/* Set/Display load down position */
		psc__lu					= 0x4150,	/* Set/Display load up position */
		psc__ma					= 0x4160,	/* Set/Display measure acceleration */
		psc__md					= 0x4170,	/* Set/Display measure deceleration */

		psc__mgd				= 0x4190,	/* Set/Display minimum good data samples */
		psc__maxwsz				= 0x41a0,	/* Set/Display maximum wafer size */
		psc__minwsz				= 0x41b0,	/* Set/Display minimum wafer size */
		psc__notchdepthmode		= 0x41c0,	/* Set/Display notch / flat depth calculation mode */
		psc__mnd				= 0x41d0,	/* Set/Display minimum notch depth */
		psc__mnw				= 0x41e0,	/* Set/Display minimum notch width */

		psc_mmtl				= 0x4200,	/* Move Wafer to the load position */
		psc_mmtm				= 0x4210,	/* Set/Display measure position */
		psc__mtpmode			= 0x4220,	/* Set/Display MTP command mode of operation */

		psc__mxd				= 0x4240,	/* Set/Display maximum notch depth */
		psc__mxw				= 0x4250,	/* Set/Display maximum notch width */
		psc__ms					= 0x4260,	/* Set/Display measure speed */

		psc__pp					= 0x4280,	/* Set/Display Z-axis coordinate at pins position */
		psc__ppcd				= 0x4290,	/* Set/Display pins position correction when going down */
		psc__ppcu				= 0x42a0,	/* Set/Display pins position correction when going up */

		psc__sa					= 0x42c0,	/* Set/Display CCD sensor angle */
		psc__sam				= 0x42d0,	/* Set/Display sample averaging mode */
		psc__sc					= 0x42e0,	/* Set/Display CCD sensor center value */

		psc__sm					= 0x4320,	/* Set/Display sampling mode */

		psc__sp					= 0x4350,	/* Set/Display stop on pins mode */
		psc__sqmax				= 0x4360,	/* Set/Display maximum CCD value for square substrates */
		psc__sqmin				= 0x4370,	/* Set/Display minimum CCD value for square substrates */

		psc__td					= 0x4390,	/* Set/Display transfer down position */
		psc__to					= 0x43a0,	/* Set/Display timeouts */
		psc__tu					= 0x43b0,	/* Set/Display transfer up position */
		psc__wszautoset			= 0x43c0,	/* Assign a set of parameters to wafer size */
		psc__wt					= 0x43d0,	/* Set/Display “wafer” type */
		psc_bal					= 0x43e0,	/* Start alignment */
		psc_ccd					= 0x43f0,	/* Read CCD sensor */

		psc_mcvf				= 0x4400,	/* Turn off the chuck vacuum */
		psc_mcvn				= 0x4410,	/* Turn on the chuck vacuum */
		psc_mpvf				= 0x4420,	/* Pins vacuum off */
		psc_mpvn				= 0x4430,	/* Pins vacuum on */
		psc_doc					= 0x4420,	/* Detect an object using CCD sensors (Wafer Loaded) */
		psc_mfwo				= 0x4430,	/* Set/Display final wafer orientation */

		psc_ltc					= 0x4450,	/* Set/Display loading type (on chuck/on pins) */
		psc_nbn					= 0x4460,	/* Display number of bad notches */
		psc_ngn					= 0x4470,	/* Display number of good notches */
		psc_ndw					= 0x4480,	/* Display notch depth and width */

		psc_rma					= 0x44d0,	/* Perform wafer misalignment */
		psc_mrwi				= 0x44e0,	/* Read wafer information */

		psc_wsz					= 0x4580,	/* Set/Display wafer size */
		psc_wsz2				= 0x4590,	/* Set/Display wafer size when two LEDs are available */

		/* 5.Housekeeping Commands */

		psc_lps					= 0x5010,	/* Load performance set parameters */
		psc_rmp					= 0x5020,	/* Restore motion and calibration parameters */
		psc_rps					= 0x5030,	/* Restore parameter sets */
		psc_smp					= 0x5040,	/* Save motion and calibration parameters */
		psc_sps					= 0x5050,	/* Save parameter sets */
		psc_wps					= 0x5060,	/* Save performance set parameters */

		/* 6.Calibration and Service Commands */
		psc__autodumps			= 0x6010,	/* Enables saving the CCD samples from unsuccessful alignment */
		psc__autodumpsf			= 0x6020,	/* Specifies the file name for CCD samples from unsuccessful alignment */
		psc__cal				= 0x6030,	/* Complete calibration procedure of standard (non-edge handling) prealigners */
		psc__cal_cor			= 0x6040,	/* Calibration procedure of CCD compensation factors */
		psc_ddpf				= 0x6050,	/* Saves CCD samples to .BAL file */
		psc_ddumpsamples		= 0x6060,	/* Saves CCD samples to .CSV file */
		psc_drps				= 0x6070,	/* Prepares CCD samples for saving to .BAL file */

		/* 7. Edge Handling Prealigner-Specific Commands */
		psc__eh					= 0x7010,	/* Verifies the prealigner is edge handling */
		psc__ehc				= 0x7020,	/* Maximum chuck CCD value */
		psc__ehc1				= 0x7030,	/* Maximum chuck CCD value */
		psc__ehc2				= 0x7040,	/* Maximum chuck CCD value */
		psc__ehp				= 0x7050,	/* Maximum chuck pins CCD value */
		psc__ehp1				= 0x7060,	/* Maximum chuck pins CCD value */
		psc__ehp2				= 0x7070,	/* Maximum chuck pins CCD value */

		psc__sfz				= 0x70b0,	/* Set/Display safe Z position above which which the chuck is clear of the stationary pins can rotate freely */

		/* 8. Exotic Commands */
		psc__mindepthmain		= 0x8010,	/* Set/Display minimum notch depth */
		psc__notchfindmode		= 0x8020,	/* Set/Display notch angle calculation mode */
		psc__notchmaxderval		= 0x8030,	/* Set/Display maximum notch/flat slope */
		psc__notchminderval		= 0x8040,	/* Set/Display minimum notch/flat slope */
		psc__notchminderwidth	= 0x8050,	/* Set/Display minimum samples with specified slope */
		psc__notchnavg			= 0x8060,	/* Set/Display number of samples outside notch/flat */

		/* 9. Miscellaneous Commands */

		psc__dt					= 0x9010,	/* Set system date and time */

		psc_hsa					= 0x9030,	/* HSA */
		psc_wsa					= 0x9040,	/* WSA */

		psc_res					= 0x9060,	/* Reset the controller */
	};

	/* Prealigner Axis Switch */
	public enum ENG_PASP /* prealigner_axis_switch_position */ : Byte
	{
		asp_h				= 0x00,		/* H : Home Switch */
		asp_f				= 0x01,		/* F : Forward Switch */
		asp_r				= 0x02,		/* R : Reverse Switch */
	};

}

/* --------------------------------------------------------------------------------------------- */
/*                                         Engine -> MDR                                         */
/* --------------------------------------------------------------------------------------------- */

namespace TGen2i.Enum.MDR
{
	/* Absolute Encoder Clear Mode */
	public enum ENG_RAEC /*__en_robot_absolute_encoder_clear__*/ : UInt16
	{
		aec_mode_warn		= 256,		/* Warning 지우기 (초기화) */
		aec_mode_position	= 1024,		/* 위치 보정 값 지우기 (위험?) */
	};

	/* Axis Name */
	public enum ENG_RANC /*__en_robot_axis_name_code__*/ : Byte
	{
		rac_t				= 0x00,		/* T : Rotation */
		rac_r1				= 0x01,		/* R1: Horizontal */
		rac_r2				= 0x02,		/* R2: Horizontal */
		rac_z				= 0x03,		/* Z : Vertical */
		rac_k				= 0x05,		/* K : K axes (Not used)  */
		rac_a				= 0x06,		/* T|R|Z 즉, All axes */
	};

	public enum ENG_RSCC /*__en_robot_serial_command_code__*/ : UInt16
	{
		rsc_none			= 0xffff,	/* None */

		/* 1. Informational Commands */
		rsc__dt				= 0x1010,	/* Set system date and time */
		rsc__si				= 0x1020,	/* Report System Configuration Information */
		rsc_res				= 0x1030,	/* Reset the robot controller */
		rsc_snr				= 0x1040,	/* Report serial number */
		rsc_ver				= 0x1050,	/* Report firmware version number */

		/* 2. Housekeeping (Parameters save / restore) commands */
		rsc__cld			= 0x2010,	/* Restore Custom Parameters */
		rsc__csv			= 0x2020,	/* Save Custom Parameters */
		rsc_rmp				= 0x2030,	/* Restore robot’s Motion and Calibration Parameters */
		rsc_rps				= 0x2040,	/* Restore Parameters Sets */
		rsc_rsp				= 0x2050,	/* Restore station parameters */
		rsc_smp				= 0x2060,	/* Save robot’s motion and calibration parameters */
		rsc_sps				= 0x2070,	/* Save Parameters Sets */
		rsc_rpp				= 0x2080,	/* Restore Calibration Parameters for single-axis prealigner */
		rsc_spp				= 0x2090,	/* Save Calibration Parameters for single-axis prealigner */
		rsc_ssp				= 0x20a0,	/* Save station parameters */

		/* 3. Status and diagnostic commands */
		rsc___cs			= 0x3010,	/* Report current robot state */
		rsc_cpo				= 0x3020,	/* Report axis current position */
		rsc_diag			= 0x3030,	/* Report last error information */
		rsc_eerr			= 0x3040,	/* Report extended error information */
		rsc_esta			= 0x3050,	/* Report enhanced system status */
		rsc_est				= 0x3060,	/* Report last error technical information */
		rsc_inf				= 0x3070,	/* Set/Report inform-after-motion mode */
		rsc_lop				= 0x3080,	/* Report last position object sensor triggered */
		rsc_per				= 0x3090,	/* Report previous error reason (robot) */
		rsc_rhs				= 0x30a0,	/* Report home switches state */
		rsc_sta				= 0x30b0,	/* Report command execution status */

		/* 4. Support for systems with absolute encoders */
		rsc__aer			= 0x4010,	/* Reset absolute encoder warning or error state */
		rsc__aest			= 0x4020,	/* Report absolute encoder status */
		rsc__eo				= 0x4030,	/* Set/Report encoder offset */
		rsc__eo_t			= 0x4031,	/* Set/Report encoder offset for Angle */
		rsc__eo_r1			= 0x4032,	/* Set/Report encoder offset for Horizontal R1 */
		rsc__eo_r2			= 0x4033,	/* Set/Report encoder offset for Horizontal R2 */
		rsc__eo_z			= 0x4034,	/* Set/Report encoder offset for Vertical Z */
		rsc__eor			= 0x4040,	/* Set/Report Encoder Zero Offset for all axis */
		rsc__ralmc			= 0x4050,	/* Report low-level encoder status for an Axis */

		/* 5. Basic (non-axes, non-station) action commands */
		rsc__bb				= 0x5010,	/* Turn backup DC power on/off */
		rsc_cstp			= 0x5020,	/* Enable/Disable QuickStop feature */
		rsc_din				= 0x5030,	/* Read input value from external I/O module */
		rsc_dout			= 0x5040,	/* Set output value to external I/O module */
		rsc_edg				= 0x5050,	/* Get status or operate Edge Gripper */
		rsc_inp				= 0x5060,	/* Read an input value */
		rsc_lenb			= 0x5070,	/* Enable/Disable "Loaded Speeds" feature */
		rsc_out				= 0x5080,	/* Set an output value */
		rsc_sof				= 0x5090,	/* Turn robot, axis or REE Servo OFF */
		rsc_son				= 0x50a0,	/* Check for wafer presense using vacuum */
		rsc_vof				= 0x50b0,	/* Turn vacuum OFF */
		rsc_von				= 0x50c0,	/* Turn vacuum ON */
		rsc_vst_t			= 0x50d1,	/* Check for wafer presence - Top Effector */
		rsc_vst_b			= 0x50d2,	/* Check for wafer presence - Bottom Effector */

		/* 6. Basic axis motion parameters */
		rsc__ch				= 0x6010,	/* Set/Report customized home position */
		rsc__cl				= 0x6020,	/* Set/Report axis current limit */
		rsc__el				= 0x6030,	/* Set/Report axis position error limit */
		rsc__fl				= 0x6040,	/* Set/Report axis positive software limit */
		rsc__fse			= 0x6050,	/* Set/Report axis forward safe envelope */
		rsc__hd				= 0x6060,	/* Set/Report axis PID loop differential coefficient during axis hold */
		rsc__he				= 0x6070,	/* Set/Report axis position error limit during axis hold */
		rsc__hi				= 0x6080,	/* Set/Report axis PID loop integral coefficient during axis hold */
		rsc__hl				= 0x6090,	/* Set/Report axis PID loop integral limit during axis hold */
		rsc__ho				= 0x60a0,	/* Set/Report axis home offset */
		rsc__ho_t			= 0x60a1,	/* Set/Report Axis home offset for T */
		rsc__ho_r1			= 0x60a2,	/* Set/Report Axis home offset for R1 */
		rsc__ho_r2			= 0x60a3,	/* Set/Report Axis home offset for R2 */
		rsc__ho_z			= 0x60a4,	/* Set/Report Axis home offset for Z */
		rsc__hp				= 0x60b0,	/* Set/Report axis PID loop proportional coefficient during axis hold */
		rsc__il				= 0x60c0,	/* Set/Report axis PID loop integral limit */
		rsc__kd				= 0x60d0,	/* Set/Report axis PID loop differential coefficient */
		rsc__ki				= 0x60e0,	/* Set/Report axis PID loop integral coefficient */
		rsc__kp				= 0x60f0,	/* Set/Report axis PID loop proportional coefficient */
		rsc__rl				= 0x6100,	/* Set/Report axis negative software limit */
		rsc__rse			= 0x6110,	/* Set/Report axis reverse safe envelope */
		rsc_acl				= 0x6120,	/* Set/Report axis acceleration */
		rsc_ajk				= 0x6130,	/* Set/Report axis acceleration jerk */
		rsc_dcl				= 0x6140,	/* Set/Report axis deceleration */
		rsc_djk				= 0x6150,	/* Set/Report axis deceleration jerk */
		rsc_hfp				= 0x6160,	/* Set/Report axis safe flip position */
		rsc_lacl			= 0x6170,	/* Set/Report axis loaded acceleration */
		rsc_lajk			= 0x6180,	/* Set/Report axis loaded acceleration jerk */
		rsc_ldcl			= 0x6190,	/* Set/Report axis loaded deceleration */
		rsc_ldjk			= 0x61a0,	/* Set/Report axis loaded deceleration jerk */
		rsc_lspd			= 0x61b0,	/* Set/Report axis loaded speed */
		rsc_spd				= 0x61c0,	/* Set/Report axis working speed */

		/* 7. Axis parameter sets, and custom parameters commands */
		rsc__cax			= 0x7010,	/* Set/Report custom axis parameter */
		rsc__cpa			= 0x7020,	/* Set/Report custom generic parameter */
		rsc__cst			= 0x7030,	/* Set/Report Custom Station Parameter */
		rsc_css				= 0x7040,	/* Copy One Parameter Set into Another */
		rsc_csw				= 0x7050,	/* Copy Parameter Set into Working Values */
		rsc_cwp				= 0x7060,	/* Copy PID Parameters to all Parameter Set */
		rsc_cws				= 0x7070,	/* Copy Current Working Values as Parameter Set */
		rsc_das				= 0x7080,	/* Report All Parameter Sets for a Single Axis */
		rsc_dps				= 0x7090,	/* Report a Parameter Set */
		rsc_hsa				= 0x70a0,	/* Set homing speed and acceleration */
		rsc_lsa				= 0x70b0,	/* Set loaded speed and acceleration */
		rsc_wsa				= 0x70c0,	/* Set working speed and acceleration */

		/* 8. Basic axis motions commands */
		rsc__afe			= 0x8010,	/* Find limit-switch to absolute encoder zero distance */
		rsc__fe				= 0x8020,	/* Find Edge (distance between home switch and motor index) */
		rsc__fh				= 0x8030,	/* Find Home Switch */
		rsc__hm				= 0x8040,	/* Home an Axis */
		rsc_afe				= 0x8050,	/* Find limit-switch to absolute encoder zero distance */
		rsc_hom				= 0x8060,	/* Start homing the robot */
		rsc_mov				= 0x8070,	/* Move relative over vector */
		rsc_mth				= 0x8080,	/* Move all axes to home position */
		rsc_mtb				= 0x8090,	/* Move an axis until sensor is triggered */
		rsc_mtp				= 0x80a0,	/* Move three axes to position */
		rsc_mva				= 0x80b0,	/* Move an axis to absolute position */
		rsc_mvr				= 0x80c0,	/* Move an axis to relative position */
		rsc_mvt1			= 0x80d0,	/* Move over line */
		rsc_mvt2			= 0x80e0,	/* Move over two lines */
		rsc_mvtp			= 0x80f0,	/* Set/Report End Effector Length for station-less motions */
		rsc_rdw				= 0x8100,	/* Rotate end-effector down */
		rsc_rup				= 0x8110,	/* Rotate end-effector up */
		rsc_stp				= 0x8120,	/* Stop motion */
		rsc_wmc				= 0x8130,	/* Wait for motion completed */

		/* 9. Station parameters commands */
		rsc__css			= 0x9010,	/* Set/Report current station set number */
		rsc__pa				= 0x9020,	/* Set/Report R axis pushing acceleration */
		rsc__pa_def			= 0x9021,	/* Set/Report R axis pushing acceleration (Default) */
		rsc__ps				= 0x9030,	/* Set/Report R axis pushing speed */
		rsc__ps_def			= 0x9031,	/* Set/Report R Axis pushing speed (Default) */
		rsc__zc				= 0x9040,	/* Set/Report station coordinate compensation with slot */
		rsc_app				= 0x9050,	/* Report Inline Station' Approach position */
		rsc_cof				= 0x9060,	/* Set/Report station center Offset */
		rsc_cofp			= 0x9070,	/* Set/Report additional station center Offset when using edge gripper */
		rsc_csl				= 0x9080,	/* Set/Report station current slot */
		rsc_csp				= 0x9090,	/* Clear single or all station parameters */
		rsc_cst				= 0x90a0,	/* Set/Report current station, by name */
		rsc_cstn			= 0x90b0,	/* Set/Report current station, by number */
		rsc_dsp				= 0x90c0,	/* Duplicate Station Parameters, by name */
		rsc_dspn			= 0x90d0,	/* Duplicate Station Parameters, by number */
		rsc_een				= 0x90e0,	/* Set/Report station servicing end effector */
		rsc_eeo				= 0x90f0,	/* Set/Report rotating end effector orientation for station */
		rsc_ens				= 0x9100,	/* Enumerate existing stations */
		rsc_eot				= 0x9110,	/* End of teaching a station */
		rsc_ils				= 0x9120,	/* Set/Report inline station last segment speed factor */
		rsc_irr				= 0x9130,	/* Set/Report inline station retracted R position */
		rsc_irt				= 0x9140,	/* Set/Report inline station retracted T position */
		rsc_isa				= 0x9150,	/* Set/Report inline station exit angle */
		rsc_isd				= 0x9160,	/* Set/Report inline station exit distance */
		rsc_ise				= 0x9170,	/* Set/Report inline station end effector length */
		rsc_isr				= 0x9180,	/* Set/Report inline station corner radius */
		rsc_ist				= 0x9190,	/* Set/Report inline station type */
		rsc_nsl				= 0x91a0,	/* Set/Report station number of slots */
		rsc_nst				= 0x91b0,	/* Report number of stations */
		rsc_ofs				= 0x91c0,	/* Set/Report station offset */
		rsc_pgd				= 0x91d0,	/* Set/Report station Put and Get Delay */
		rsc_pgf				= 0x91e0,	/* Put (place) and Get (pick) failure options */
		rsc_pgo				= 0x91f0,	/* Set/Report Put/Get Option and T-Z axes synchronization mode */
		rsc_pit				= 0x9200,	/* Set/Report station pitch */
		rsc_psd				= 0x9210,	/* Set/Report pushing distance */
		rsc_psh				= 0x9220,	/* Set/Report pusher type to use with a station */
		rsc_pss				= 0x9230,	/* Set/Report start pushing offset */
		rsc_psz				= 0x9240,	/* Set/Report Z push offset */
		rsc_pua				= 0x9250,	/* Set/Report station pickup Z acceleration */
		rsc_pus				= 0x9260,	/* Set/Report station pickup Z speed */
		rsc_rfl				= 0x9270,	/* Turn on or off rotate-on-the-fly permanently */
		rsc_rof				= 0x9280,	/* Turn on or off rotate-on-the-fly for next GET or PUT operation */
		rsc_rpo				= 0x9290,	/* Set/Report radial retracted station position */
		rsc_spo				= 0x92a0,	/* Set/Report station coordinate, by name */
		rsc_spon			= 0x92b0,	/* Set/Report station coordinate, by number */
		rsc_str				= 0x92c0,	/* Set/Report station' stroke distance */
		rsc_tbl				= 0x92d0,	/* Set/Report Trajectory Mode Overlap factor */
		rsc_tch				= 0x92e0,	/* Start teaching a station */
		rsc_tmd				= 0x92f0,	/* Set/Report Trajectory Motion mode */
		rsc_vchk			= 0x9300,	/* Set/Report vacuum check mode before Put and Get */
		rsc_vto				= 0x9310,	/* Set/Report vacuum timeout before Put and Get */

		/* 10.Station motion commands */
		rsc_get				= 0xa010,	/* Get (pick) a wafer from a station (by name), with end option */
		rsc_getn			= 0xa020,	/* Get (pick) a wafer from a station (by number) */
		rsc_geto			= 0xa030,	/* Get (pick) a wafer from a station, with temporary offsets from taught position */
		rsc_getr			= 0xa040,	/* Get (pick) a wafer from a station (by name), with end option */
		rsc_getrt			= 0xa050,	/* Get (pick) a wafer from a station (by name), with retry option */
		rsc_getw			= 0xa060,	/* Get (pick) a wafer from a station, with wait option at pick-up position */
		rsc_getxy			= 0xa070,	/* Get (pick) a wafer from a station with defined X/Y offsets */
		rsc_mos				= 0xa080,	/* Move arm out of station */
		rsc_mss				= 0xa090,	/* Move a wafer from station to station (by name) */
		rsc_mssn			= 0xa0a0,	/* Move a wafer from station to station (by number) */
		rsc_mta				= 0xa0b0,	/* Move to Station' Approach position */
		rsc_mtr				= 0xa0c0,	/* Move to Station' Retracted position */
		rsc_mts				= 0xa0d0,	/* Move to Station Coordinate */
		rsc_pgwa			= 0xa0e0,	/* Set/Report wait-at-station position state */
		rsc_put				= 0xa0f0,	/* Put (place) a wafer into a station (by name) */
		rsc_putn			= 0xa100,	/* Put (place) a wafer into a station (number) */
		rsc_puto			= 0xa110,	/* Put (place) a wafer into a station, with temporary offsets */
		rsc_putw			= 0xa120,	/* Put (place) a wafer into a station, with wait option at drop-down */
		rsc_sbusy			= 0xa130,	/* Check whether a station is being serviced */
		rsc_sgp				= 0xa140,	/* Move a wafer from station to station, with end option */
		rsc_slck			= 0xa150,	/* Set/Report Station Lock State */
		rsc_spg				= 0xa160,	/* Place a wafer, pick a wafer, with end option */

		/* 11.Station scanning parameters and motion commands */
		rsc__bt				= 0xb010,	/* Set/Report Beam threshold above/below a wafer (global) */
		rsc__mt				= 0xb020,	/* Set/Report Maximum Wafer Thickness (global) */
		rsc__sa				= 0xb030,	/* Set/Report Wafer Scanning Z Axis Acceleration */
		rsc__sc				= 0xb040,	/* Set Current Position as Scanning Center Coordinate (Report는 없음) */
		rsc__sl				= 0xb050,	/* Set Current Position as Scanning Left Coordinate (Report는 없음) */
		rsc__sr				= 0xb060,	/* Set Current Position as Scanning Right Coordinate (Report는 없음) */
		rsc__ss				= 0xb070,	/* Set/Report Wafer Scanning Z Axis Speed */
		rsc__st				= 0xb080,	/* Set/Report Scanner Type (Scanner Sensing Type) */
		rsc_dscn			= 0xb090,	/* Report Scanned Positions */
		rsc_map				= 0xb0a0,	/* Prints the result of last scanning operation */
		rsc_msc				= 0xb0b0,	/* Move to Scanning Coordinate */
		rsc_sbt				= 0xb0c0,	/* Set/Report Beam threshold above/below a wafer (per-station) */
		rsc_scn				= 0xb0d0,	/* Start Scanning procedure */
		rsc_scp				= 0xb0e0,	/* Set/Report scanning center position */
		rsc_scp_t			= 0xb0e1,	/* Set/Report scanning center position for Rotation (T) */
		rsc_scp_r1			= 0xb0e2,	/* Set/Report scanning center position for R1 (Horizontal) */
		rsc_scp_r2			= 0xb0e3,	/* Set/Report scanning center position for R2 (Horizontal) */
		rsc_scp_z			= 0xb0e4,	/* Set/Report scanning center position for Z (Vertical) */
		rsc_sdbg			= 0xb0f0,	/* Set/Report scanning debug mode */
		rsc_slp				= 0xb100,	/* Set/Report scanning left position */
		rsc_slp_t			= 0xb101,	/* Set/Report scanning left position for Rotation (T) */
		rsc_slp_r1			= 0xb102,	/* Set/Report scanning left position for R1 (Horizontal) */
		rsc_slp_r2			= 0xb103,	/* Set/Report scanning left position for R2 (Horizontal) */
		rsc_slp_z			= 0xb104,	/* Set/Report scanning left position for Z (Vertical) */
		rsc_smt				= 0xb110,	/* Set/Report Maximum Wafer Thickness (per-station) */
		rsc_srp				= 0xb120,	/* Set/Report scanning right position */
		rsc_srp_t			= 0xb121,	/* Set/Report scanning right position for Rotation (T) */
		rsc_srp_r1			= 0xb122,	/* Set/Report scanning right position for R1 (Horizontal) */
		rsc_srp_r2			= 0xb123,	/* Set/Report scanning right position for R2 (Horizontal) */
		rsc_srp_z			= 0xb124,	/* Set/Report scanning right position for Z (Vertical) */
		rsc_szh				= 0xb130,	/* Set/Report scanning highest Z position */
		rsc_szl				= 0xb140,	/* Set/Report scanning lowest Z position */
		rsc_tcs				= 0xb150,	/* (re)Teach scanning position */
		rsc_tsp				= 0xb160,	/* Test scanning position */

		/* 12.Align-on-Flight (On-The-Flight, OtF) parameters and commands */
		rsc_aofcn2			= 0xc010,	/* Set/Report Check for Notch on Second Scan Option */
		rsc_acs				= 0xc020,	/* Report OtF configuration set number */
		rsc_aofc			= 0xc030,	/* Execute OtF calibration run */
		rsc_aofcs			= 0xc040,	/* Execute OtF calibration run against a station */
		rsc_aofd			= 0xc050,	/* Set/Report OtF debug level */
		rsc_aofim			= 0xc060,	/* Set/Report OtF samples Ignore Mode */
		rsc_aofdump			= 0xc070,	/* Report Samples from Last OtF Operation */
		rsc_aofmd			= 0xc080,	/* Set/Report Default Maximum Allowed center offsets */
		rsc_aofm			= 0xc090,	/* Set/Report Maximum allowed center offsets */
		rsc_aofnl			= 0xc0a0,	/* Set/Report OtF Notch Limit */
		rsc_aofpmi			= 0xc0b0,	/* Report Debug Informatin from last OtF Execution */
		rsc_aofsa			= 0xc0c0,	/* Set/Report Axis Search Speed and Acceleration */
		rsc_asl				= 0xc0d0,	/* Set/Report On-The-Fly wafer radius/end effector length */
		rsc_getc			= 0xc0e0,	/* Get (pick) a wafer from a station with on-the-fly offset calculation */
		rsc_getcw			= 0xc0f0,	/* Get (pick) a wafer from a station with on-the-fly offset calculation, and wait option */
		rsc_mvrc			= 0xc100,	/* Execute relative axis motion with on-the-fly sensor capture */
		rsc_putc			= 0xc110,	/* Put (place) a wafer into a station (letter) with OtF offset calculation and correction */
		rsc_putcw			= 0xc120,	/* Put (place) a wafer into a station (letter) with OtF offset and wait option */
		rsc_putm			= 0xc130,	/* Put (place) a wafer into a station (letter) with OtF Offset Measurement */
		rsc_putxy			= 0xc140,	/* Put (place) a wafer into a station with X/Y offsets from OtF calculation */
		rsc_putxyw			= 0xc150,	/* Put (place) a wafer into a station with X/Y offsets from OtF and wait option */
		rsc_rwic			= 0xc160,	/* Report center offset from last OtF operation */

		/* 13. New Command for DUAL Arm */
		rsc_dmva			= 0xd010,	/* Move Both Arms to Same Absolute Position */
		rsc_dmvr			= 0xd020,	/* Move Both Arms Over Same Relative Distance */
		rsc_dget			= 0xd030,	/* Execute Object Pick-Up Operation with Designated or Both Arms */
		rsc_dput			= 0xd040,	/* Execute Object Place Operation with Designated or Both Arms */
		rsc_see				= 0xd050,	/* Set/Report Arm to Be Used for Wafer Mapping */
	};
}

/* --------------------------------------------------------------------------------------------- */
/*                                         Engine -> PLC                                         */
/* --------------------------------------------------------------------------------------------- */

namespace TGen2i.Enum.EFU
{
	/* return value types for BL500 */
	public enum ENG_SEBT /*__en_shinsung_efu_bl500_types__*/ : Byte
	{
		en_pv				= 0x01,		/* Processing Value */
		en_alarm			= 0x02,		/* Alarm Code */
		en_sv				= 0x03,		/* Setting Value */
	};
}
