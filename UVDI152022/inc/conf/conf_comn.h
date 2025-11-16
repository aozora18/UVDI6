
/*
 desc : System Configuration Information
*/

#pragma once

#include "define.h"

/* --------------------------------------------------------------------------------------------- */
/*                                           상수 값                                             */
/* !!! 중요 !!!  아래 상수 값은 반드시 8의 배수 이어야 함. 이유 : 구조체 경계선 메모리 정렬 때문 */
/* --------------------------------------------------------------------------------------------- */

#define	USE_EQUIPMENT_SIMULATION_MODE	0		/* Simulation Mode로 동작할지 여부 */

#define	MAX_PH							8		/* 포토헤드 최대 개수 */
#define	MAX_LED							8		/* 포토헤드 내에 설치된 LED 개수 (실제로는 0x00 ~ 0x05) */
#define MAX_LED_POWER					4095	/* 0 ~ 4095 */
#define	MAX_TABLE						8		/* 노광 작업을 수행할 수 있는 Work Table (Stage) 최대 개수 */
#define MAX_X_CORRECTION_TABLE_ENTRY	200		/* X 축 보정 데이터 최대 개수 */
#define MAX_Y_CORRECTION_TABLE_ENTRY	200		/* Y 축 보정 데이터 최대 개수 */
#define	MAX_SCROLL_MODE					8		/* 스크롤 모드 개수 (1 ~ 6) */
#define	MAX_SCROLL_RATE					19000	/* 최대 19000 Hz */
#define	MAX_REGIST_JOB_LIST				8		/* 최대 등록 가능한 JOB 개수 */
#define	MAX_HASH_VALUE_COUNT			64		/* 최대 Hash value 문자 개수 */
#define	MAX_DCODE_LIST					400		/* 거버 내에 등록 가능한 최대 D-Code 개수 */
#define	MAX_COORD_XY					400		/* Location에 대한 Coordination 최대 개수 */
//#define	MAX_GERBER_NAME					200		/* 최대 Job Path Name 길이 */
#define	MAX_GERBER_NAME					512		/* 최대 Job Path Name 길이 */
#define	MAX_WARP_COORD_XY				400		/* 최대 Warp Coordinate X/Y 개수 */
#define	MAX_PANEL_SERIAL_STRING			32		/* 시리얼 문자열 최대 길이 */
#define	MAX_PANEL_TEXT_STRING			32		/* 텍스트 문자열 최대 길이 */
#define	MAX_LOCAL_FIXED_COUNT			200		/* Local Fixed Rotaton / Scaling / Offset */
#define	MAX_SSD_SMART_DATA				16		/* 14 ea. Fixed */

#define	MAX_PANEL_STRING_COUNT			8		/* 한 노광 면에 표현되는 Panel D-Code 개수 */

#define	MAX_MC2_DRIVE					8		/* MC2에 설치 가능한 최대 드라이브 개수 */
#define MAX_ALIGN_CAMERA				8		/* 설치 가능한 얼라인 카메라 최대 개수 (현재는 2개지만, 이 값은 8로 무조건 고정. 구조체 정렬 때문에. !!!) */
#define MAX_SCAN_MARK_COUNT				16		/* Local Mark 한 번 스캔할 때, 최대 검색될 수 있는 마크 개수 */

#define	WORK_NAME_LEN					128
#define	STEP_NAME_LEN					128
#define	MAX_GLOBAL_MARKS				4		/* Global Mark 최대 개수 */
#define	MAX_LOCAL_MARKS					1280		/* 최대 Local Mark (Fiducial) 개수 */
#define	MAX_REGISTRATION_POINTS			(MAX_GLOBAL_MARKS + MAX_LOCAL_MARKS)	/* 얼라인 카메라에 의해 검사된 최대 마크 등록 개수 */
#define	MAX_SELECT_VELO				4		/* MC2 jog이동시 스피드 선택 개수 */
#define OFFSET_CNT 10
/* --------------------------------------------------------------------------------------------- */
/*                                           상수 값                                             */
/* !!!!!!!!! 중요 !!!  아래 상수 값은 반드시 고정 값 구조체 경계선 정렬하면 절대 안됨  !!!!!!!!! */
/* --------------------------------------------------------------------------------------------- */
#define MAX_TRIG_CHANNEL				4		/* Trigger 장치에 설치 가능한 최대 Channel 개수 */

/* 최대 등록 가능한 Photohead Step 개수 (레시피에 등록 가능한 개수) */
#define MAX_PH_STEP_LINES				255	/* 1 바이트로 환경 (구조체) 파일에 설정 되어 있으므로 */
/* 최대 등록 가능한 Photohead의 Stripe 단차 개수 */
#define MAX_PH_CORRECT_Y				255	/* 1 바이트로 환경 (구조체) 파일에 설정 되어 있으므로 */

/* --------------------------------------------------------------------------------------------- */
/*                                             문자열                                            */
/* --------------------------------------------------------------------------------------------- */

/* 동기화 객체 */
#define	MTX_PROC_PLC_DATA				L"MTX_BRainX1_PROC_PLC_DATA"

/* --------------------------------------------------------------------------------------------- */
/*                                             열거형                                            */
/* --------------------------------------------------------------------------------------------- */

/* System Error Code 분류 값 (Code.str 파일 검색 용도) */
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

//얼라인 모션
typedef enum class __en_align_motion__ : UINT16
{
	none = 0,
	en_onthefly_2cam	= 0b0000001000000001, //온더플라이 사이드캠
	en_onthefly_3cam	= 0b0000001100000001, //온더플라이 센터캠
	
	en_static_2cam      = 0b0000001000000010, //스테틱 사이드캠
	en_static_3cam		= 0b0000001100000010, //스테틱 센터캠
}	ENG_AMOS;


/* 얼라인 보정값 참조타입  */
typedef enum class __en_algin_data_ref_type_ : UINT8
{
	en_no_use = 0,
	en_from_info = 1,
	en_from_mappingfile = 2,
	en_not_defined = 3,

}	ENG_ADRT;


/*실패사유*/
typedef enum class __en_manualfixoffset_result__ : UINT8
{
	canFix,
	noNeedToFix,
	grabcountMiss,
	noRecipeLoaded,
	firstRun,
	none,
}	ENG_MFOR;


/* Align Mark Type */
typedef enum class __en_algin_type_global_local__ : UINT8
{
	/* Zero mark */
	en_global_0_local_0x0_n_point = 0x00,
	en_global_4_local_0_point = 0x04,
	en_global_4_local_n_point = 0x05,
	en_not_defined = 0xff,

}	ENG_ATGL;

/* Work Error Type */
typedef enum class __en_work_error_type__ : UINT8
{
	en_none = 0x00,
	en_lds_thick_check = 0x01,

}	ENG_WETE;
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
	en_init							= 0x01,		/* 초기 진입 */
	en_wait							= 0x02,		/* 현재 대기 */
	en_next							= 0x03,		/* 다음 동작 */
	en_comp							= 0x04,		/* 작업 완료 */
	en_forceSet						= 0x05,		/* 임의로 스텝변경 */
	en_time							= 0xfe,		/* 타임 아웃 */
	en_error						= 0xff,		/* 에러 발생 */
	
}	ENG_JWNS;



typedef enum class __en_req_injob_action_ : UINT32
{
	none=0,
	clearMarkData=1,
	invalidateUI,
	end=9999,
}	ENG_RIJA;




/* Engine Running Mode (총 31개 넘어가면 안됨) */
typedef enum class __en_engine_runnging_vision_mode__ : UINT8
{
	en_monitoring					= 0x00,	/* Engine Monitoring */
	en_cmps_sw						= 0x10,	/* for Control App. */
	/* for Calibration */
	en_cali_step					= 0x21,	/* Calibration S/W - The step (error) xy of optic. */
	en_cali_vdof					= 0x22,	/* Calibration S/W - The focus of optic. and camera */
	en_cali_meas					= 0x23,	/* Calibration S/W - 얼라인 카메라 보정 및 보정 검증 작업 */
	en_cali_exam					= 0x24,	/* Calibration S/W - 얼라인 카메라 보정 및 보정 검증 작업 */
	en_acam_spec					= 0x25,	/* Calibration S/W - 얼라인 카메라 사양 (SPEC) 검증 작업 */
	en_led_power					= 0x26,	/* Calibration S/W - Measure and collect the illuminance of the photohead */
	en_stage_st						= 0x27,	/* Calibration S/W - The straightness of Stage XY */
	en_hole_size					= 0x28,	/* Calibration S/W - Measure the size of marker's hole */

}	ENG_ERVM;

/* 얼라인 노광 동작 모드 (Direct Expose, Align Expose, Align & Calibration Expose */
typedef enum class __en_align_operation_expose_mode__ : UINT8
{
	en_direct_expose				= 0x00,	/* 직접 노광 */
	en_align_expose					= 0x01,	/* 얼라인 노광 */
	en_calib_expose					= 0x02,	/* 얼라인 포함하여 보정 노광 */

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

/* Global Mark 4점에 대한 6 군데의 위치 정보 */
typedef enum class __en_global_mark_dist_direct__ : UINT8
{
	en_top_horz						= 0x00,	/* 사각형 -> 위쪽 -> 수평선 (1 <-> 3 마크 간의 길이) */
	en_btm_horz						= 0x01,	/* 사각형 -> 아래 -> 수평선 (2 <-> 4 마크 간의 길이) */
	en_lft_vert						= 0x02,	/* 사각형 -> 왼쪽 -> 수직선 (1 <-> 2 마크 간의 길이) */
	en_rgt_vert						= 0x03,	/* 사각형 -> 오른 -> 수직선 (3 <-> 4 마크 간의 길이) */
	en_lft_diag						= 0x04,	/* 사각형 -> 왼쪽 -> 대각선 (1 <-> 4 마크 간의 길이) */
	en_rgt_diag						= 0x05,	/* 사각형 -> 오른 -> 대각선 (2 <-> 3 마크 간의 길이) */

}	ENG_GMDD;

/* 디바이스 이동 방향 (Up, Down, left, right) */
typedef enum class __en_mc2_device_move_direct__ : UINT8
{
	en_none							= 0x00,

	en_move_up						= 0x01,
	en_move_down					= 0x02,

	en_move_left					= 0x03,
	en_move_right					= 0x04,

	en_move_homed_x					= 0x05,
	en_move_homed_y					= 0x06,

	en_pos_align_start				= 0x07,		/* 스테이지가 Align 시작 위치로 이동 */
	en_pos_expo_start				= 0x08,		/* 스테이지가 노광 시작 위치로 이동 */

	en_move_axis_up					= 0x10,
	en_move_axis_down				= 0x11,

}	ENG_MDMD;

/* MC2 Motion Drive ID 즉, SD2S Drive ID */
typedef enum class __en_mc2_motion_drive_id__ : UINT8
{
	en_stage_x						= 0x00,
	en_stage_y						= 0x01, 
	//en_stage_y						= 0x02, //y2를 마스터로


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
	en_axis_acam3					= 0x14,
	en_axis_theta					= 0x15,
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

/* Model Define 값 */
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
/*                                    Config 구조체 - Common                                     */
/* --------------------------------------------------------------------------------------------- */

/* 반드시 1 bytes 정렬 (Shared Memory 때문에 1 bytes 정렬 불가피) */
#pragma pack (push, 1)

/* 통신 연결 상태 정보 */
typedef struct __st_device_link_shared_memory__
{
	UINT8				is_connected;		/* 통신 연결 유무 */
	UINT8				u8_reserved[7];
	UINT16				link_time[6];		/* 통신 연결된 시간 저장 (년-월-일 시-분-초) */
	UINT16				u16_error_code;		/* PLC : Complete Code, Luria : Luria Status */
	UINT16				u16_reserved[1];
	INT32				last_sock_error;	/* 가장 최근에 발생된 소켓 에러 */
	INT32				i32_reserved;

	/*
	 desc : 구조체 정보 초기화
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
	UINT8				grab_mark_saved:1;				/* 검색된 얼라인 마크 저장 유무. (검색 실패와 상관 없이 무조건 저장 유무) */
														/* GUI (제어SW)가 실행된 폴더에서 mark 라는 하위 폴더에 저장됨 */
	UINT8				log_file_saved:1;				/* 0 : 로그 파일로 저장 안함, 1 : 로그 파일로 로그 저장 함 */
	UINT8				log_file_type:1;				/* 0 : 텍스트 기반 파일로 저장, 1 : Database 포맷 (SQLite3)으로 저장 */
	UINT8				run_emulate_mode:1;				/* 0 : 실제 장비와 연동 후 동작, 1 : 장비 연결 없이 SW 내부적으로 동작 */
	UINT8				run_engine_monitor:1;			/* 0: 엔진 감시 프로그램 실행하지 않음, 1: 엔진 감시 프로그램 실행 */
	UINT8				strobe_lamp_type:2;				/* 채널 당 조명 최대 조명 2개 제공. 동시에 사용은 불가 (0 : 조명 1번, 1 : 조명 2번) */
	UINT8				run_material_detect:1;			/* 소재 감지 여부. (0:소재 유무 감지 없이 노광 진행, 1:소재 감지 확인 후 노광) */
														/* 단, Align Expose인 경우만 해당됨 */

	UINT8				align_camera_kind:4;			/* 0: None, 1 : Basler (외부), 2 : IDS (VISITECH; 내장) */
	UINT8				run_develop_mode:1;				/* 0 : 실제 장비와 연동 후 동작, 1 : 장비 연결 없이 SW 내부적으로 동작 */
	UINT8				all_alarm_ignore:1;				/* DI 장비에서 발생되는 모든 알람 무시 여부 (1:무시, 0:알림) */
	UINT8				all_warn_ignore:1;				/* DI 장비에서 발생되는 모든 경고 무시 여부 (1:무시, 0:알림) */
	UINT8				comm_log_optic:1;				/* PODIS vs. Luria		0 : 통신 로그 저장 안함, 1 : 통신 로그 저장 */
	UINT8				use_auto_align : 1;				/* 자동과 공정시 Align 동작 사용 유무 0 : Align 동작 없음[Direct Expose] 1 : Align 동작 사용 [ExposeAlign]*/

	UINT8				comm_log_mc2:1;					/* PODIS vs. MC2		0 : 통신 로그 저장 안함, 1 : 통신 로그 저장 (수동 요청된 경우만 저장) */
	UINT8				comm_log_plc:1;					/* PODIS vs. PLC		0 : 통신 로그 저장 안함, 1 : 통신 로그 저장 (설정 요청된 경우만 저장) */
	UINT8				comm_log_efu:1;					/* PODIS vs. EFU		0 : 통신 로그 저장 안함, 1 : 통신 로그 저장 */
	UINT8				mesg_box_optic:1;				/* Luria 내부적으로 발생된 에러 메시지를 엔진에서 출력 여부 */
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
	UINT8				comm_log_milara:1;				/* PODIS vs. MILARA		0 : 통신 로그 저장 안함, 1 : 통신 로그 저장 (Robot & Prealigner) */
	UINT8				comm_log_pm100d:1;				/* PODIS vs. PM100D		0 : 통신 로그 저장 안함, 1 : 통신 로그 저장 */
	UINT8				comm_log_litho:1;				/* PODIS vs. Link.Litho	0 : 통신 로그 저장 안함, 1 : 통신 로그 저장 */
	UINT8				u8_bit_reserved:1;
#elif (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	   CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
	UINT8				u8_bit_reserved:4;
#endif
	UINT8				engine_mode;					/* ENG_ERVM Mode 값 */
	UINT8				u8_reserved[4];

	TCHAR				gerber_path[MAX_PATH_LEN];		/* 거버 파일이 저장되어 있는 기본 경로 (거버 폴더 선택함에 있어서 편리함을 제공하기 위한 목적. 특별히 다른 곳에서 사용할 곳은 없음) */
	TCHAR				preproc_dll_path[MAX_PATH_LEN];	/* PreProcess 동작과 관련된 DLL 파일들이 저장되어 있는 경로 (!!! 중요 !!!) */

}	STG_CCGS,	*LPG_CCGS;

typedef struct __st_config_system_device_monitoring__
{
	UINT8				hdd_drive;					/* HDD Drive Index (0:C, 1:D, 2:E, ...) */
	UINT8				log_delect_day : 7;				/* Log 파일 유지 기간*/
	UINT8				bmp_delect_day : 7;				/* Bmp 파일 유지 기간*/
	UINT8				u8_reserved[5];
	TCHAR				mon_time[24];				/* 모니터 시작 시간 (0000-00-00 00:00:00)*/
	/* 아래 2개 변수는 로그 기준으로 프로그램 실행 중 발생된 에러 및 경고 개수 */
	UINT16				error_count;				/* 현재까지 발생된 에러 개수 (최대 65535) */
	UINT16				warn_count;					/* 현재까지 발생된 경고 개수 (최대 65535) */
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
	UINT16				luria_port;				/* CMPS (Client) 가 Luria (Server) 와 통신을 위한 포트 번호 */
	UINT16				mc2s_port;				/* UDP 패킷을 CMPS->MC2 쪽으로 송신하는 포트 번호			*/
	UINT16				mc2c_port;				/* UDP 패킷이 MC2->CMPS 쪽으로 수신하는 포트 번호			*/
	UINT16				mc2bs_port;				/* UDP 패킷을 CMPS->MC2 쪽으로 송신하는 포트 번호			*/
	UINT16				mc2bc_port;				/* UDP 패킷이 MC2->CMPS 쪽으로 수신하는 포트 번호			*/
	UINT16				plc_port;				/* CMPS가 Melsec PLC와 통신을 위한 포트 번호				*/
	UINT16				trig_port;				/* CMPS가 Trigger Board와 통신을 위한 포트 번호				*/
	UINT16				efem_port;				/* CMPS가 EFEM (Robot)과 통신을 위한 포트 번호				*/
	UINT16				visi_port;				/* CMPS가 Vision (Alpha)과 통신을 위한 포트 번호			*/
	UINT16				philhmi_port;			/* Philhmi 통신을 위한 포트 번호							*/	// by sysandj
	UINT16				gentec_port;			/* Gentec 통신을 위한 포트 번호								*/	// 230517 mhbaek Add
	UINT16				strb_lamp_port;			/* Strobe Lamp 통신을 위한 포트 번호						*/
	UINT16				keyence_lds_port;		/* Keyence LDS 통신을 위한 포트 번호						*/	// 230925 mhbaek Add
	UINT16				u16_port[1];
	/* ---------------------------------------------------------------------------------------------------- */
	UINT32				link_time;				/* 서버에 재-접속을 연결 시도를 위한 간격 (단위: ms) */
												/* 0 값이면, 더 이상 재접속 시도 수행하지 않음 */
	UINT32				idle_time;				/* 이 시간동안 원격 시스템으로 부터 응답이 없으면, 강제로 연결 해제 진행 */
												/* 만약 0 값으로 설정되면, 이 기능을 사용하지 않음 */
	UINT32				sock_time;				/* 통신 포트로부터 데이터가 수신되었는지 대기하는 시간 (단위: ms) */
												/* 보통 0 값. 이 값이 클수록 포트에 데이터 수신 감지하는데 오랫동안 대기 함 */
	UINT32				live_time;				/* 현재 설정된 시간마다 Live Check 메시지 송신 (단위: ms)*/
	UINT32				port_buff;				/* 송신 / 수신 통신 버퍼 크기 (단위: KBytes) */
												/* 송신 혹은 수신 데이터를 임시로 저장하기 위한 버퍼 (보통 1024 이하) */
	UINT32				recv_buff;				/* 한번 수신된 데이터가 일시적으로 저장되는 임시 버퍼 크기 (단위: KBytes) */
	UINT32				ring_buff;				/* 수신된 패킷들이 저장될 임시 버퍼 크기 (단위: KBytes) */
												/* !!! recv_buff 보다 2배 이상 커야 됨 !!! (환경 파일에서 작으면 엔진 내부적으로 크기 재조정 진행) */
	UINT32				u32_reserved;

}	STG_CCSI,	*LPG_CCSI;

typedef struct __st_config_luria_service_info__
{
#if 0	/* Not used (Only Here !!!) */
	TCHAR				print_simulation_out_dir[MAX_PATH_LEN];		/* 노광 결과가 저장될 경로 설정 (마지막'\'은 옵션. 입력할 필요 없음) */
																	/* 기본 경로 (C:\\lls_simulation) */
#endif
	UINT8				table_count:2;								/* Motion 기반 Work Table Count (1 or 2) */
	UINT8				ph_rotate:1;								/* 광학계 설치되어 있는 방향 (180도 회전 여부. 0:None, 1:180도 회전) */
	UINT8				motion_control_type:2;						/* Motion Control Type (1:Sieb&Meyer, 2:Newport, 3:ACS) */
	UINT8				x_correction_table_1:1;						/* 보정 테이블 [ 1 ] 번 쪽 입력 여부 (0:입력 없음, 1:입력 있음. 별도 참조 파일 확인 */
	UINT8				x_correction_table_2:1;						/* 보정 테이블 [ 2 ] 번 쪽 입력 여부 (0:입력 없음, 1:입력 있음. 별도 참조 파일 확인 */
	UINT8				y_correction_table_1:1;						/* 보정 테이블 [ 1 ] 번 쪽 입력 여부 (0:입력 없음, 1:입력 있음. 별도 참조 파일 확인 */

	UINT8				y_correction_table_2:1;						/* 보정 테이블 [ 2 ] 번 쪽 입력 여부 (0:입력 없음, 1:입력 있음. 별도 참조 파일 확인 */
	UINT8				table_1_print_direction:1;					/* 포토헤드가 노광하는 방향 (0:Negative, 1:Positive) */
	UINT8				table_2_print_direction:1;					/* 포토헤드가 노광하는 방향 (0:Negative, 1:Positive) */
	UINT8				active_table_no:2;							/* 초기 활성화 하려는 Table Number (1 or 2) 즉, 초기 노광할 때, 사용하고자 하는 Work Table Number (Double gantry or Stage일 경우, 노광할 때마다 Table Number가 바뀜) */
	UINT8				emulate_motion:1;							/* Motion 장비가 없이 테스트 할 것인지 여부 (0:Motion 장비 없이 테스트, 1:Motion 장비 연동 후 테스트) */
	UINT8				emulate_ph:1;								/* 포토헤드 장비가 없이 테스트 할 것인지 여부 (0:포토헤드 장비 없이 테스트, 1:포토헤드 장비 연동 후 테스트) */
	UINT8				emulate_trigger:1;							/* 트리거 장비가 없이 테스트 할 것인지 여부 (0:트리거 장비 없이 테스트, 1:트리거 장비 연동 후 테스트) */

	UINT8				x_drive_id_1:4;								/* 포토헤드에 X Drive ID for Table 1 값 설정 (0 ~ 7) (아래 Y Drive ID와 동일한 값을 입력하면 안됨) */
	UINT8				x_drive_id_2:4;								/* 포토헤드에 X Drive ID for Table 2 값 설정 (0 ~ 7) (아래 Y Drive ID와 동일한 값을 입력하면 안됨) */

	UINT8				y_drive_id_1:4;								/* 포토헤드에 Y Drive ID for Table 1 값 설정 (0 ~ 7) */
	UINT8				y_drive_id_2:4;								/* 포토헤드에 Y Drive ID for Table 2 값 설정 (0 ~ 7) */

	UINT8				debug_print_level:2;						/* 0: Print Info. Only (노광 관련 로그만). 1: Print Info. and Debug (노광 및 디버깅 로그만). 2: Print Info., 3 : Debug and Trace Message (모든 로그 출력) */
	UINT8				artwork_complexity:2;						/* Artwork Complexity (0:Normal, 1:High, 2:eXtreme) (거버 복잡도에 따라 값 지정. 보통 0 값 지정. 1 or 2일 경우, 포토헤드에 등록되는 거버 개수 제한 ) */
	UINT8				z_drive_type:3;								/* Z Drive Type (포토헤드에 장착된 Motiotn Drive Type) 1 = Stepper motor (built in), 2 = Linear drive from ACS, 3 = Linear drive from Sieb & Meyer (valid from version 2.10.0 and newer) */
	UINT8				over_pressure_mode:1;						/* 포토헤드에 과부하가 발생한 경우 냉각 팬 동작 여부 (1:On, 0:Off) */

	UINT8				hys_type_1_scroll_mode:4;					/* 노광 속도 모드 값 (1 ~ 7) (값이 높을수록 속도 빠름) */
	UINT8				led_count:3;								/* 광학계에서 사용되는 LED 개수 (보통 4개. 265, 385, 395, 405) */
	UINT8				use_announcement:1;							/* Luria Service로부터 Announcement 패킷 정보 수신 여부 즉, CMPS에 Luria로부터 패킷을 수신하기 위해 서버 스레드 생성 여부 */

	UINT8				use_hw_inited:1;							/* Luria Service의 H/W Init 여부 */
	UINT8				ph_count:4;									/* 장비에 설치되어 있는 광학계 (포토헤드) 개수 (최대 16. 0 ~ 15) */
	UINT8				illum_meas_type:3;							/* Photohead의 조도 측정 대상 (1:Power, 2:Energy, 3:Frequency, 4:Density) */

	UINT8				ph_pitch;									/* 포토헤드가 상/하 노광할 수 있는 Stripe의 개수 (최대 256) */
																	/* LLS50:2, LLS24:4, LLS10:8 (단, 광학계가 1개이면 1로 고정) */

	UINT8				motion_control_ip[4];						/* Motion Controller IPv4 (MC2 IPv4) */
																	/* 1 offset size (pulse dist) = (end - start) / pulse_count = 10.8621 um */
	UINT8				ph_ipv4[MAX_PH][4];							/* 장비에 설치되어 있는 포토헤드의 IPv4 */
	UINT8				z_drive_ip[4];								/* z_drive_type == 3인 경우만 유효. MC2가 1 대인 경우, motion_control_ip와 동일, 2대인 경우 다른 IP이어야 함 */
	UINT8				DOFofPh; //포토헤드의 DOF
	UINT8				z_drive_sd2s_ph[MAX_PH];					/* Photohead Z Axis Drive가 Linear Motor Type일 경우, MC2에 연결된 SD2S Drive 번호 */

	UINT16				scroll_rate;								/* 포토헤드가 처리할 수 있는 최대 이동 비율 값 (Default: 19000) */
	UINT16				hys_type_1_negative_offset;					/* HysteresisType1 정보 설정 (단위: Trigger Pulse) */
	UINT16				mirror_count_xy[2];							/* DMD Mirror Counts (가로 / 세로) */
	
	
	
	UINT16				luria_ver_major;							/* 광학계 서비스 SW version : 반드시 정수 (1,2,3,4,....) */
	UINT16				luria_ver_minor;							/* 광학계 서비스 SW version : 반드시 정수 (4,5,7,11,) */
	UINT16				luria_ver_build;							/* 광학계 서비스 SW version : 반드시 정수 (2155) */
																	/* ex> 2.4.3  -> major = 2, minor = 4, build=3 */
																	/*     2.11.6 -> major = 2, minor = 11, build=6 (반드시 규칙 지킬것) */
	UINT32				paralleogram_motion_adjust[MAX_TABLE];		/* 모션 평행 (대각선) 이동 보정 (1000 => 1.0 (Default), 500 => 0.5) */
	UINT32				product_id;									/* 포토헤드 Product ID 값 (100001:LLS25, 25002:LLS04, 50001:LLS10, 200001:LLS50, 2352002:LLS30, ...) */
	UINT32				u32_reserved;

	DOUBLE				illum_filter_rate[MAX_LED];					/* LED 0 ~ 3 번까지 조도 측정을 위한 센서 필터 전후 값 비율 */
																	/* (초기 LED Power 설정할 때, 얻어지는 값) (측정된 값에 100.0f 값을 곱한 값 (%)) */
	DOUBLE				illum_garbage_pwr;							/* 조도 측정 장치의 조도 최소 측정 단위가 1 nm 이므로, 이 값 이하이면 OK  */
	DOUBLE				sensor_diameter;							/* 조도 센서 직경 길이 (Circle임. 단위: mm) */
	DOUBLE				correction_factor;							/* 조도 Power 계산하기 위한 개수 값 */

	DOUBLE				ph_offset_x[MAX_PH];						/* 포토헤드 단차 (X 단차. 1번 포토헤드 기준으로 2 ~ 7 번 포토헤드 떨어진 거리 값. 단위: mm) (소수점 4자리까지 유효) */
	DOUBLE				ph_offset_y[MAX_PH];						/* 포토헤드 단차 (Y 단차. 1번 포토헤드 기준으로 2 ~ 7 번 포토헤드 떨어진 거리 값. 단위: mm) (소수점 4자리까지 유효) */
	DOUBLE				ph_z_focus[MAX_PH];							/* 각 Photohead 별로 설정된 Z Axis 노광 높이 값 (단위: mm) [SET_ALIGN] DOF_FILM_THICK 기준으로 Z Axis (FOCUS) 높이 값 (소수점 4자리까지 유효) */

	DOUBLE				table_limit_min_xy[MAX_TABLE][2];			/* Work Table이 이동할 수 있는 상/하/좌우 최소 값 (unit: mm) (소수점 3자리까지 유효) */
	DOUBLE				table_limit_max_xy[MAX_TABLE][2];			/* Work Table이 이동할 수 있는 상/하/좌우 최대 값 (unit: mm) (소수점 3자리까지 유효) */
	DOUBLE				table_expo_start_xy[MAX_TABLE][2];			/* 노광 시작 위치 (단위: mm). 노광 시작을 하기 위한 Luria에 등록해주는 노광 (실제 노광 위치는 아님) 시작 위치  */

	DOUBLE				max_y_motion_speed;							/* 테이블 Y 축 방향으로 이동할 수 있는 최대 속도 (단위: mm/sec) (소수점 3자리까지 유효) */
	DOUBLE				x_motion_speed;								/* 테이블 X 축 방향으로 이동할 수 있는 속도 (단위: mm/sec) (소수점 3자리까지 유효) 보통 노광 시작 위치로 이동할 때 X 축 이동 속도 */
	DOUBLE				y_acceleration_distance;					/* Y 축 가속도가 정상 궤도에 오르기 위한 최소한의 여유 거리 (단위: mm) (소수점 3자리까지 유효) */

	DOUBLE				hys_type_1_delay_offset[2];					/* HysteresisType1 Moving Delay. Positive[0] / Negative[1] Delay (단위: mm/sec) (소수점 6자리까지 유효) */
	DOUBLE				ph_z_move_max;								/* 포토헤드 Z 축을 최대한 다운/업 (DOWN) 할 수 있는 최대치 값 (단위: mm) */
	DOUBLE				ph_z_move_min;								/* 포토헤드 Z 축을 최대한 다운/업 (UP) 할 수 있는 최소치 값 (단위: mm) */
	DOUBLE				pixel_size;									/* Lens 배율과 Scroll Mode (Scroll Step Size)에 따라 다름 (단위: um) */
	DOUBLE				ph_z_select_velo[MAX_SELECT_VELO];
																	/* Pixel Resolution (5.4 or 10.8 or 21.6 um) (unit: pm; picometer = 1/1000 nm) */
																	/* 유효한 값은 nanometer 단위 (아래 값은 picometer 단위까지 표현됨) */
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
	
	
	
	//오토포커스 관련은 여기부터
	
	UINT8				useAF;										/*Auto Foucs 사용 여부*/
	UINT8				useExternalSensor;
	UINT8				useAAQ;// Active Area Qualifier
	UINT8				useEdgeTrigger;
	
	UINT16				afGain; //									/* AutoFocus Gain (1 ~ 4095) 값이 클수록 좋다? */
	UINT8				afPanelMode; // 0레지스트, 1 잉크,
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
	 desc : Emulation 모드로 동작되는 항목이 존재하는지 여부
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
	UINT8				drive_count;				/* MC2 연결된 드라이브 개수 */
	UINT8				origin_point;				/* ENG_MSOP 즉, 0:Left/Bottom, 1:Left/Top, 2:Right/Bottom, 3:Right/Top */
	UINT8				u8_reserved[6];
	UINT8				axis_id[MAX_MC2_DRIVE];		/* 모터 드라이브 ID에 부여된 드라이브 번호 */

	UINT32				mc2_serial;					/* MC2 장비의 Serial Number */
	UINT32				u32_reserved;

	DOUBLE				move_velo;					/* 현재 드라이브의 이동 속도 : mm/sec */
	DOUBLE				mark_velo;					/* Align Mark 검사할 때, Y 축 이동 속도 값 : mm/sec */
	DOUBLE				step_velo;					/* 광학계 및 기타 단차 측정할 때 이동 속도 값 : mm/sec */
	DOUBLE				move_dist;					/* 현재 위치에서 이동해야 할 거리 unit: mm */
	DOUBLE				min_dist[MAX_MC2_DRIVE];	/* 모터 축 별 최소 이동 거리 unit: mm */
	DOUBLE				max_dist[MAX_MC2_DRIVE];	/* 모터 축 별 최대 이동 거리 unit: mm */
	DOUBLE				max_velo[MAX_MC2_DRIVE];	/* 모터 축 별 최대 이동 속도 unit: mm */
	DOUBLE				select_velo[MAX_MC2_DRIVE][MAX_SELECT_VELO];	/* 모터 축 별 최대 이동 속도 unit: mm */

	/*
	 desc : 스테이지의 이동 영역 (최소 ~ 최대) 반환 (단위: mm)
	 parm : drv_x	- [in]  X 축 이동 드라이브 번호 (Zero-based)
			drv_y	- [in]  Y 축 이동 드라이브 번호 (Zero-based)
	 retn : 사각형 형역 값 반환
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
	UINT8				group_no;			/* Ethernet 모듈의 그룹 No */
											/* Group (I do not know, 0 value unconditionally) */
	UINT8				network_no;			/* Ethernet 모듈의 네트워크 No */
											/* Network (fixed value of 0 if not MULTI-DROP) */
	UINT8				station_no;			/* Ethernet 모듈의 국(Station) No */
											/* Station number (fixed value of 0 when it is not MULTI_DROP) */
	UINT8				plc_no;				/* PLC No */
	UINT8				u8_reserved[4];
											/* PLC number (fixed value of 0xff when it is not MULTI_DROP) */
	UINT16				start_io;			/* Ethernet 모듈의 선두 I/O */
											/* Head I / O (fixed value of 0x03ff if it is not MULTI-DROP) */
	UINT16				addr_x_count;		/* PLC X 주소의 Total Count (1 bits area / WORD counts). ex> If the score is 64,         8 BYTES */
	UINT16				addr_y_count;		/* PLC Y 주소의 Total Count (1 bits area / WORD counts)  ex> If the score is 128,       16 BYTES */
	UINT16				addr_m_count;		/* PLC M 주소의 Total Count (1 bits area / WORD counts)  ex> If the score is 5100,     638 BYTES */
	UINT16				addr_d_count;		/* PLC D 주소의 Total Count (16 bits area / WORD counts) ex> If the score is 2200,    4400 BYTES */
	UINT16				u16_reserved[3];
	UINT32				start_d_addr;		/* PLC D 시작 주소 */
	UINT32				u32_reserved;

	/*
	 desc : PLC 주소에 해당되는 메모리 인덱스 값
	 parm : None
	 retn : 인덱스 값 (0xffff이면 실패)
	*/
	UINT16 GetAddrToIndex(UINT32 addr)
	{
		/* 유효한 주소 값이 아닌 경우 */
		if (addr < start_d_addr)	return 0xffff;

		return UINT16(addr - start_d_addr);
	}

	/*
	 desc : PLC 마지막 주소 얻기
	 parm : None
	 retn : 마지막 주소 값 (시작 주소 + 주소 데이터 개수)
	*/
	UINT32 GetLastDAddr()
	{
		return (start_d_addr + addr_d_count - 1);
	}

}	STG_CPSI,	*LPG_CPSI;

/* Global Mark 4 점에 대해, 6군데 길이 (LT-RT, LB-RB, LT-LB, RT-RB, LT-RB, LB-RT) 에 대해 검증 값 임시 저장 */
typedef struct __st_global_mark_length_result__
{
	UINT8				len_valid;		/* Global Mark 4개의 지점을 이루는 사각형의 각각 길이와 대각선 오차가 한계를 벗어났는지 유무 */
										/* 0x00 : Not set or Reset, 0x01 : Set (Succ), 0x02 : Set (Fail) */
	UINT8				uu_reserved[7];
	DOUBLE				diff;			/* 6 군데 (LT-RT, LB-RB, LT-LB, RT-RB, LT-RB, LB-RT) 중 1 곳 실체 측정한 오차 값. 거버 소스 단의 거리와 실측한 거리 간의 오차 값 (단위: um) */
	DOUBLE				scale;			/* 6 군데 (LT-RT, LB-RB, LT-LB, RT-RB, LT-RB, LB-RT) 중 1 곳 실체 측정한 비율 값. 거버 소스 단의 거리와 실측한 거리 간의 Scale (Source:거버 / Target:실측) 값 */

}	STG_GMLR,	*LPG_GMLR;
typedef struct __st_global_mark_length_validate__
{
	UINT8				set_data;			/* Mark 값이 설정되어 있는지 여부  */
	UINT8				u8_reserved[7];
	DOUBLE				angle[2];		/* 수평(0) / 수직(1) 각도 차이 값. 0.4도 이상되면 안됨 (degree 값임. Radian 값이 아님) */
	STG_GMLR			result[6];

	/*
	 desc : 현재 설정된 모든 (6군데) 마크 간의 길이/거리 등 값이 설정했는지 여부
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsSetMarkLenData()	{	return set_data==0x01;	}
	/*
	 desc : 현재 설정된 모든 (6군데) 마크 간의 길이/거리 등을 설정했다고 플래그 설정
	 parm : None
	 retn : None
	*/
	VOID SetMarkLenData()	{	set_data = 0x01;	}
	/*
	 desc : 각 6 곳에 대해 실제 거버 대비 측정된 Mark 간 길이가 유효 범위 내에 있는지 여부
	 parm : index	- [in]  0x00 ~ 0x05 (6 곳 중 1 곳)
	 retn : TRUE or FALSE
	 참조 : index 값 의미
			0 : Mark.1 - Mark.3 (LT - RT)  1 : Mark.2 - Mark.4 (LB - RB) - TD
			2 : Mark.1 - Mark.2 (LT - LB)  3 : Mark.3 - Mark.4 (RT - RB) - MD
			4 : Mark.1 - Mark.4 (LT - RB)  5 : Mark.2 - Mark.3 (LB - RT)
	*/
	BOOL IsMarkLenValid(UINT8 index)	{	return result[index].len_valid == 0x01;		}
	/*
	 desc : 전체 마크 길이 (6군데, 직선 4개와 대각선 2개)가 유효한지 여부
			1개라도 유효하지 않으면 에러 값 반환
	 parm : count	- [in]  비교하고자 하는 개수
			가령. 4를 입력하면 총 유효한 개수가 적어도 4개 이상 있어야 된다는 의미
	 retn : TRUE or FALSE
	 참조 : index 값 의미
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
	 desc : 각 6 곳에 대해 실제 거버 대비 측정된 Mark 간 길이가 유효 여부 값 설정
	 parm : index	- [in]  0x00 ~ 0x05 (6 곳 중 1 곳)
			value	- [in]  0x00 : Not set or Reset, 0x01 : Set (Succ), 0x02 : Set (Fail)
	 retn : None
	 참조 : index 값 의미
			0 : Mark.1 - Mark.3 (LT - RT)  1 : Mark.2 - Mark.4 (LB - RB) - TD
			2 : Mark.1 - Mark.2 (LT - LB)  3 : Mark.3 - Mark.4 (RT - RB) - MD
			4 : Mark.1 - Mark.4 (LT - RB)  5 : Mark.2 - Mark.3 (LB - RT)
	*/
	VOID SetMarkLenValid(UINT8 index, UINT8 value)	{	result[index].len_valid	= value;	}
	/*
	 desc : 거버의 상의 마크 거리와 얼라인 카메라로 실측한 마크 간의 거리 및 Scale (비율) 값 저장
	 parm : index	- [in]  Zero-based (0x00 ~ 0x05)
			diff	- [in]  거리 오차 값 (단위: nm)
			scale	- [in]  Gerber Mark 거리 / 실측 Mark 거리 = Scale 값
							(현재 구현되어 있지 않으므로, 무조건 0.0f 값으로 고정됨)
	 retn : None
	 참조 : index 값 의미
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
	 desc : 거버의 상의 마크 거리와 얼라인 카메라로 실측한 마크 간의 거리 값 반환
	 parm : index	- [in]  Zero-based (0x00 ~ 0x05)
	 retn : 거리 값 반환 (단위: mm)
	 참조 : index 값 의미
			0 : Mark.1 - Mark.3 (LT - RT)  1 : Mark.2 - Mark.4 (LB - RB) - TD
			2 : Mark.1 - Mark.2 (LT - LB)  3 : Mark.3 - Mark.4 (RT - RB) - MD
			4 : Mark.1 - Mark.4 (LT - RB)  5 : Mark.2 - Mark.3 (LB - RT)
	*/
	DOUBLE GetMarkLenDiff(UINT8 index)	{	return result[index].diff;	}
	/*
	 desc : 수평(0) / 수직(1) 각도 차이 값 반환
	 parm : mode	- [in]  0x00:수평, 0x01:수직
	 retn : 거리 값 반환 (단위: mm)
	 참조 : 수평(0) / 수직(1) 각도 차이 값. 0.4도 이상되면 안됨 (degree 값임. Radian 값이 아님)
	*/
	DOUBLE GetMarkAngDiff(UINT8 mode)	{	return angle[mode];	}

	/*
	 desc : 일정 시간 동안 저장된 마크 거리 간 Scale 값 반환
	 parm : index	- [in]  아래 참조
			pos_th	- [in]  몇 번째 검색된 마크 거리 간 Scale 값 반환 위치 (0x00 ~ 0x05)
	 retn : Scale 값 반환
	 참조 : index 값 의미
			0 : Mark.1 - Mark.3 (LT - RT)  1 : Mark.2 - Mark.4 (LB - RB) - TD
			2 : Mark.1 - Mark.2 (LT - LB)  3 : Mark.3 - Mark.4 (RT - RB) - MD
			4 : Mark.1 - Mark.4 (LT - RB)  5 : Mark.2 - Mark.3 (LB - RT)
	*/
	DOUBLE GetMarkLenScale(UINT8 index)	{	return result[index].scale;	}
	/*
	 desc : 소재로부터 측정된 4점의 마크 기준으로, 회전된 각도가 0.4도 이상 벗어났는지 여부
	 parm : None
	 retn : TRUE or FALSE
	 note : 각 검색된 Mark 들 간의 회전된 각도가 최대 0.4 도 이상 벗어난 경우, 에러 처리 해야 됨
	*/
	BOOL IsMarkAngleValid()	{	return abs(angle[0]) <= 0.4f && abs(angle[1]) <= 0.4f;	}
	/*
	 desc : 마크 간의 넓이/높이 및 대각선의 길이에 대한 유효 값 초기화
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
	 desc : 수평(0) / 수평(1) 의 오차값이 확인
	 parm : diff	- [DOUBLE] 0x00 : 제한 길이
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
 desc : 수직(2) / 수직(3) 의 오차값이 확인
 parm : diff	- [DOUBLE] 0x00 : 제한 길이
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

/* CMPS SW 정보 */
typedef struct __st_config_cmps_sw_value__
{
	DOUBLE				global_mark_diff_x[2];				/* 거버 데이터에서 Mark 2번 기준으로 Mark 1번의 X 좌표 Offset (단위: um) */
															/* 거버 데이터에서 Mark 4번 기준으로 Mark 2번의 X 좌표 Offset (단위: um) */

}	STG_CCSV,	*LPG_CCSV;

#pragma pack (pop)	/* 8 bytes */