
/* CIM300 Header */

#pragma once


/* -------------------------------------------------------------------------------------------- */
/*                                      윈도 이벤트 메시지                                      */
/* -------------------------------------------------------------------------------------------- */

// 윈도 메시지 사용자 영역 범위: WM_USER(0x0400) ~ WM_APP-1(0x8000)
#define	WM_APP_CIM300UI							WM_USER + 1000

/* --------------------------------------------------------------------------------------------- */
/*                                    뮤텍스 공유 고유 이름                                      */
/* --------------------------------------------------------------------------------------------- */

#define	MUTEX_APP_CIM300UI						_T("UvDI MUTEX CIM300UI App (DI_Team)")
#define MUTEX_CIM300_LIB						_T("UvDI MUTEX CIM300 LIB (DI_Team)")

/* --------------------------------------------------------------------------------------------- */
/*                                     주요 기능 사용 여부                                       */
/* --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- */
/*                                   환경 설정 및 데이터 파일                                    */
/* --------------------------------------------------------------------------------------------- */



/* --------------------------------------------------------------------------------------------- */
/*                                      스레드 동작 속도 값                                      */
/* --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- */
/*                                           Data Size                                           */
/* --------------------------------------------------------------------------------------------- */

/* CIM300 관련 상수 값 정의 */
#define	USE_EPJ_LOAD_FILE						0													/* EPJ File 적재 후 기능 사용 여부 */
#define MAX_CIMCONNECT_COUNT					1													/* 최대 연결 가능한 CIM Connection 개수 (최대 5개지만, TEL (Intel)의 경우 무조건 1) */
#ifdef _DEBUG
#define MAX_CIMLOG_COUNT						32													/* 최대 임시로 등록 가능한 개수 */
#else
#define MAX_CIMLOG_COUNT						1024												/* 최대 임시로 등록 가능한 개수 */
#endif
#define	MAX_RECIPE_COUNT						128													/* 최대 등록 가능한 레시피 (Process Program) 개수 */
#define	MAX_FOUP_COUNT							2													/* DI 장비에 설치된 FOUP의 최대 개수 */
#define	MAX_CARRIER_SLOT_COUNT					25													/* Carrier 1개에 등록할 수 있는 최대 Slot Count */

#define	CIM_E40_JOB_QUE_SIZE					25													/* Maximum size of Process Job queue (문서에서는 25 추천) */
#define	CIM_E94_JOB_QUE_SIZE					25													/* Maximum size of Control Job queue (문서에서는 25 추천) */

#define CIM_CMD_NAME_SIZE						64													/* EPJ 파일 내에 ID와 매핑되는 Names의 최대 길이 */
#define	CIM_CMD_DESC_SIZE						512													/* Command name에 해당되는 Description */
#define	CIM_CMD_UNIT_SIZE						16													/* Command name에 해당되는 Unit */
#define	CIM_CMD_MESG_SIZE						256													/* Command name에 해당되는 Message */
#define	CIM_CMD_REMOTE_SIZE						20													/* Remote Command Name Size (Lengthe) */
#define	CIM_VAL_ASCII_SIZE						128													/* Command name에 해당된 Ascii 문자열 길이 값 (현재 / 기본 값) */
#define	CIM_CP_NAME_SIZE						64													/* Command Parameter Name 길이 */
#define CIM_LOG_MESG_SIZE						512													/* 로그 문자열 메시지 최대 길이 */
#define	CIM_EVENT_DATA_SIZE						4096												/* Callback Event Data 최대 크기 */

#define	CIM_LOAD_PORT_COUNT						2													/* 설치되는 Load Port 개수 */
#define	CIM_MAP_EPJ_ID_COUNT					4096												/* EPJ 파일에 등록된 ID (or Names) 전체 개수 결국 라인 수 (주석 제외) */
#define	CIM_EVENT_DATA_COUNT					16													/* CIM300 Event (Callback) 최대 저장 개수 */
#define	CIM_TERM_MESG_SIZE						1024												/* CIMConnect Terminal Message Size */

#define	CIM_MODULE_NAME_SIZE					128													/* Module Name Size (MPT: Module Process Tracking) */
#define	CIM_MODULE_STEP_SIZE					128													/* Step Size for Module Name */
#define	CIM_CONTROL_JOB_ID_SIZE					128													/* Control Job ID (for string) 이름의 최대 길이 */
#define	CIM_PROCESS_JOB_ID_SIZE					128													/* Process Job ID (for string) 이름의 최대 길이 */
#define	CIM_PRJOB_RECIPE_ID_SIZE				128													/* Process Job ID를 위한 Recipe ID 길이 */
#define	CIM_CARRIER_ID_SIZE						128													/* Carrier ID (for string) 이름의 최대 길이 */
#define	CIM_CARRIER_TAG_SIZE					64													/* Carrier ID에 부착된 Tag 길이 (Serial, QR Code, Bar Code) */
#define	CIM_CARRIER_LOC_SIZE					64													/* Carrier ID가 위치한 Location ID (Name) 길이 */
#define	CIM_DATA_SEGMENT_SIZE					1024												/* The DataSeg parameter (size) */
#define	CIM_SUBSTRATE_ID						64													/* Substrate ID 문자열 길이  */
#define	CIM_SUBSTRATE_LOC_ID					128													/* Substrate Location ID 문자열 길이 */
#define CIM_TIME_SIZE							24													/* YearMonthDayHourMinuteSecondCentisecond : yyyymmddhhmissmss */

#define	CIM_WORK_STEP_NAME						64													/* CIM300 Work Step의 작업 이름 */

/* --------------------------------------------------------------------------------------------- */
/*                                          Mutex String                                         */
/* --------------------------------------------------------------------------------------------- */

#define MUTEX_SHARED_EVENT						L"Mutex.TelGEN-II.CIMSharedData.Event"
#define MUTEX_SHARED_TERM_MSG					L"Mutex.TelGEN-II.CIMSharedData.TermMsg"
#define MUTEX_CIMLOG_DATA						L"Mutex.TelGEN-II.CIMCIMLogData.Data"
#define	MUTEX_SHARED_RECIPE						L"Mutex.TelGEN-II.CIMSharedData.Recipe"
#define	MUTEX_E40_CALLBACK						L"Mutex.TelGEN-II.CIMSharedData.E40_Callback"
#define	MUTEX_E94_CALLBACK						L"Mutex.TelGEN-II.CIMSharedData.E94_Callback"

/* --------------------------------------------------------------------------------------------- */
/*                                           File Names                                          */
/* --------------------------------------------------------------------------------------------- */

#define	TEL_CIM_CONFIG							L"telcim.inf"

/* --------------------------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------------------------- */
/*                                           COMMON                                              */
/* --------------------------------------------------------------------------------------------- */

/* 작업 상태 */
typedef enum class __en_work_job_process_state__ : UINT8
{
	en_idle						= 0x00,	/* 아무런 작업이 없는 상태임 */
	en_init						= 0x01,	/* 초기 상태임 */
	en_wait						= 0x02,	/* 에러가 아니고, 기다리고 있는 상태 */
	en_next						= 0x04,	/* 다음 작업 단계로 이동하라고 함 */
	en_comp						= 0x08,	/* 작업이 완료된 상태임  */
	en_stop						= 0x10,	/* 에러가 발생하여 STOP 된 경우임 */

}	ENG_WJPS;

/* CIM300 Internal Logging Level Values */
typedef enum class __en_cim300_internal_logging_level_values__ : UINT32
{
	en_c3ll_none				= 0,			/* Logging Disabled */
	en_c3ll_information			= 0x8000,		/* Information about API calls and state changes */
	en_c3ll_error				= 0x00010000,	/* Errors */
	en_c3ll_messages			= 0x00020000,	/* SECS Messages sent and received */
	en_c3ll_callbacks			= 0x00080000,	/* Callbacks called and return */
	en_c3ll_entryexit			= 0x00400000,	/* Function entry and exit */
	en_c3ll_debug				= 0x00100000,	/* Debugging logs */
	en_c3ll_all					= 0xffffffff,	/* All logs */

}	ENG_ILLV;

/* GEM300 표준 코드 */
typedef enum class __en_gem300_e_series_standard_code__ : UINT8
{
	en_not_defined				= 0x00,
	en_e30_gem					= 0x1e,	/* E30 : GEM (E30) */
	en_e32_mmm					= 0x20,	/* E32 : Material Movement Management */
	en_e38_ctms					= 0x26,	/* E38 : Cluster Tool Module Communication */
	en_e39_oss					= 0x27,	/* E39 : Object Service Standard */
	en_e40_pjm					= 0x28,	/* E40 : Process Job Management */
	en_e41_em					= 0x29,	/* E41 : Exception Management */
	en_e53_er					= 0x35,	/* E53 : Event Reporting */
	en_e81_cf					= 0x51,	/* E81 : CIM Framework */
	en_e84_ehpio				= 0x54,	/* E84 : Enhanced Handoff Parallel I/O Interface */
	en_e87_cms					= 0x57,	/* E87 : Specification for Carrier Management */
	en_e90_sts					= 0x5a,	/* E90 : Specification for Substrate Tracking */
	en_e94_cjm					= 0x5e,	/* E94 : Control Job Management */
	en_e98_oem					= 0x62,	/* E98 : Object based Equipment Model */
	en_e116_ept					= 0x74, /* E116: Equipment Performance Tracking */
	en_e148_ts					= 0x94, /* E148: Time Sync */
	en_e157_mpt					= 0x9d, /* E157: Module Process Tracking */

}	ENG_ESSC;

/* GEM300 Tracking System Subscript Index */
typedef enum class __en_gem300_tracking_system_subscript_index__ : UINT8
{
	en_robot					= 0x01,	/* <Robot> Subscript Index */
	en_pre_aligner				= 0x02,	/* <PreAligner> Subscript Index */
	en_process_chamber			= 0x03,	/* <ProcessChamber> Subscript Index */
	en_buffer					= 0x04,	/* <Buffer> Subscript Index */
	en_load_port_1				= 0x05,	/* <LoadPort1> Subscript Index */
	en_load_port_2				= 0x06,	/* <LoadPort2> Subscript Index */

}	ENG_TSSI;

/* MPT Module */
typedef enum class __en_gem300_tracking_module_process_index__ : UINT8
{
	en_process_chamber			= 0x01,	/* <ProcessChamber> Subscript Index */

}	ENG_TMPI;

/* Load Port State Information */
typedef enum class __en_gem300_load_port_operating_state__ : UINT8
{
	en_unknown					= 0xff,
	en_unloading				= 0x01,
	en_unloaded					= 0x02,
	en_loading					= 0x02,
	en_loaded					= 0x03,

}	ENG_LPOS;

/* --------------------------------------------------------------------------------------------- */
/*                                          E30 (GEM)                                            */
/* --------------------------------------------------------------------------------------------- */

/* E30 Callback Event ID */
typedef enum class __en_e30_callback_event_func_id__ : UINT8
{
	/* ICxEMAppCallback */
	en_alarm_change				= 0x01,	/* AlarmChange */
	en_async_msg_error			= 0x02,
	en_callback_replaced		= 0x03,
	en_command_called			= 0x04,
	en_event_triggered			= 0x05,
	en_get_value				= 0x06,
	en_get_value_to_byte_buff	= 0x07,
	en_get_values				= 0x08,
	en_get_values_to_byte_buff	= 0x09,
	en_host_pp_load_inq_ack		= 0x0a,
	en_host_pp_send_ack			= 0x0b,
	en_host_term_msg_ack		= 0x0c,
	en_message_received			= 0x0d,
	en_pp_data_request			= 0x0e,	/* Not used */
	en_pp_data					= 0x0f,	/* Not used */
	en_pp_data_file				= 0x10,
	en_pp_delete				= 0x11,	/* Not used */
	en_pp_load_inquire			= 0x12,
	en_pp_request				= 0x13,
	en_pp_send					= 0x14,	/* Not used */
	en_pp_send_file				= 0x15,
	en_pp_send_file_verify		= 0x16,
	en_set_value				= 0x17,
	en_set_value_from_byte_buff	= 0x18,
	en_set_values				= 0x19,
	en_set_values_from_byte_buff= 0x1a,
	en_state_change				= 0x1b,
	en_terminal_msg_rcvd		= 0x1c,
	en_value_changed1			= 0x1d,
	en_values_changed			= 0x1e,
	en_verify_value				= 0x1f,
	en_verify_values			= 0x20,
	/* ICxEMAppCallback2 */
	en_callback_replaced2		= 0x30,
	en_get_report_value			= 0x31,
	/* ICxEMAppCallback3 */
	en_get_value2				= 0x40,
	/* ICxEMAppCallback4 */
	en_clock_change				= 0x50,
	en_request_to_change_clock	= 0x51,
	/* ICxEMAppCallback5 */
	en_ec_changed				= 0x60,
	/* ICxEMAppCallback6 */
	en_value_changed2			= 0x70,
	/* ICxEMAppDataCallback */
	en_event_report				= 0x80,
	en_exception_report			= 0x81,
	en_trace_report				= 0x82,
	/* ICxEMAppMsgCallback */
	en_icx_em_service			= 0x90,
	en_icx_host_emulator		= 0x91,

}	E30_CEFI;

/* ICx Interface Kind */
typedef enum class __en_cim300_icx_interface_kind__ : UINT16
{
	en_e30_gem						= 0x0001,	/* E30GEM */
	en_e39_gui						= 0x0002,	/* E39GUI */
	en_e40_pjm						= 0x0004,	/* E40PJM */
	en_e87_cms						= 0x0008,	/* E87CMS */
	en_e90_sts						= 0x0010,	/* E90STS */
	en_e94_cjm						= 0x0020,	/* E94CJM */
	en_e116_ept						= 0x0040,	/* E116EPT */
	en_e148_ts						= 0x0080,	/* E148TS */
	en_e157_mpt						= 0x0100,	/* E157MPT */

	en_cim_all						= 0xffff,	/* ALL */

}	EN_CIIK;

/* Alarm Level Code */
typedef enum class __en_global_alarm_level_code__ : UINT8
{
	en_normal						= 0x00,	/* Normal */
	en_warning						= 0x01,	/* Warning */
	en_alarm						= 0x02,	/* Error or Alarm */
	en_unknown						= 0x03,	/* Unknown */

}	ENG_GALC;

/* EPJ File의 Subject Type Code */
typedef enum class __epj_subject_type_code__ : UINT32
{
	en_not_defined					= 0x00000000,
	en_events						= 0x10000000,
	en_variables					= 0x20000000,
	en_alarms						= 0x40000000,
	en_reserved						= 0x80000000,

}	ENG_ESTC;

/* CIM300 Callback Event Data ID */
typedef enum class __en_gem_callback_param_id__ : UINT8
{
	en_empty_param					= 0x00,	/* Empty Parameter */

	en_e30_variable_name			= 0x11,	/* Variable Name */
	en_e30_terminal_mesg			= 0x12,	/* Terminal Message */
	en_e30_user_mesg				= 0x13,	/* User Received Message */
	en_e30_remote_command			= 0x14,	/* Remote Command */
	en_e30_pp_recipe_name			= 0x15,	/* Recipe Name (Recipe File) */

	en_e40_prcommand_callback		= 0x21,	/* Process Command Callback */
	en_e40_process_job_id			= 0x22,	/* Process State Change */

	en_e87_carrier_id				= 0x41,	/* Carrier ID */
	en_e87_location_id				= 0x42,	/* Location ID for Load Port */
	en_e87_alarm_id					= 0x43,	/* Location ID for Load Port */
	en_e87_transcation_id			= 0x44,	/* Transaction ID for Carrier Tag */

	en_e90_substrate_id				= 0x51,	/* Substrate ID */

	en_e94_control_job_id			= 0xe1,	/* Control Job ID */
	en_e94_process_job_id			= 0xe2,	/* Process Job ID */

	en_e94_cjcommand_callback		= 0xe1,	/* Control Command Callback */

}	ENG_GCPI;

/* CIM300 Callback Return Code CAACK */
typedef enum class __en_gem_callback_return_code_caack__ : UINT8
{
	en_acknowledged					= 0x00,	/* Command performed successfully */
	en_invalid_command				= 0x01,	/* Invalid (unknown) remote command */
	en_cannot_perform_now			= 0x02,	/* Command cannot be performed now */
	en_invalid_data					= 0x03,	/* Invalid parameter data */
	en_will_be_performed			= 0x04,	/* Command will be performed later. A collection event should be used to notify the host of completion */
	en_rejected						= 0x05,	/* Command is rejected */
	en_performed_with_errors		= 0x06,	/* The command was performed, but there were errors */

}	ENG_CAACK;

/* --------------------------------------------------------------------------------------------- */
/*                                           E30 (GEM)                                           */
/* --------------------------------------------------------------------------------------------- */

/* GEM EPJ Variable Type */
typedef enum class __en_e30_gem_variable_type_code__ : UINT8
{
	en_any							= 0,	/* disregard type */
	en_sv							= 1,	/* Status Variable */
	en_ec							= 2,	/* Equipment Constant */
	en_dv							= 3,	/* Discrete (Data) Vairable */

}	E30_GVTC;

/* Value Type */
typedef enum class __en_e30_gem_phil_value_type__ : UINT16
{
	en_any							= 0,

	en_i1							= 1,	/* I1 : 1 bytes signed integer */
	en_i2							= 2,	/* I2 : 2 bytes signed integer */
	en_i4							= 3,	/* I4 : 4 bytes signed integer */
	en_i8							= 4,	/* I8 : 8 bytes signed integer */

	en_u1							= 5,	/* U1 : 1 bytes unsigned integer */
	en_u2							= 6,	/* U2 : 2 bytes unsigned integer */
	en_u4							= 7,	/* U4 : 4 bytes unsigned integer */
	en_u8							= 8,	/* U8 : 8 bytes unsigned integer */

	en_f4							= 9,	/* F4 : 4 bytes signed point */
	en_f8							= 10,	/* F8 : 8 bytes signed point */

	en_a							= 11,	/* A  : ascii string */
	en_w							= 12,	/* W  : Wide (2 byte) character string */

	en_b							= 13,	/* Bo : 4 bytes boolean 1 or 0 */
	en_bi							= 14,	/* Bi : 1 bytes signed integer */
	en_list							= 15,	/* L  : List value */
	en_jis8							= 16,	/* J  : JIS-8 string (for japaness language) */

	en_array						= 4096,

}	E30_GPVT;

/* Process State for CIMConnect */
typedef enum class __en_e30_gem_process_state_value__ : UINT8
{
	en_setup						= 0x00,
	en_idle							= 0x01,
	en_execute						= 0x02,
	en_complete						= 0x03,

}	E30_GPSV;

/* State Machine values for StateChange callback */
/* Used to identify which state machine has changed */
typedef enum class __en_e30_service_state_machine_state__ : INT8
{
	en_none							= -1,
	en_commstate					= 0,	/*  */
	en_control						= 1,	/*  */
	en_spooling						= 2,	/*  */
	en_process						= 3,	/*  */
	en_protocol						= 4,	/*  */

}	E30_SSMS;

/* GEM Communication State Machine state values for StateChange callback */
/* Used to decode state values in the StateChange callback when the StateMachine=smCommunications */
typedef enum class __en_e30_ethernet_state_machine_code__ : UINT16
{
	en_disabled						= 0x0000,	/*   0 */
	en_enabled						= 0x0100,	/* 256 */
	en_wait_cra						= 0x0101,	/* 257 */
	en_wait_delay					= 0x0102,	/* 258 */
	en_communicating				= 0x0104,	/* 260 : communications has been established */
	en_cr_from_host					= 0x0110,	/* 272 */
	en_wait_cra_wait_cr_from_host	= 0x0111,	/* 273 : waiting for S1F13 or S1F14 from host */

}	E30_ESMC;

/* GEM Spooling State Machine state values for StateChange callback */
typedef enum class __en_e30_control_state_machine_code__ : UINT8
{
	en_eq_offline					= 0x01,
	en_attempt_online				= 0x02,
	en_host_offline					= 0x03,
	en_online_local					= 0x04,
	en_online_remote				= 0x05,

}	E30_CSMC;

/* GEM Spooling State Machine state values for StateChange callback */
typedef enum class __en_e30_spool_state_machine_code__ : UINT16
{
	en_inactive						= 0x0000,
	en_active						= 0x0100,	/* 256 */
	en_no_spool_output				= 0x0101,	/* 257 */
	en_transmit_spool				= 0x0102,	/* 258 */
	en_purge_spool					= 0x0104,	/* 260 */
	en_spool_not_full				= 0x0110,	/* 272 */
	en_spool_full					= 0x0120,	/* 288 */

}	E30_SSMC;

/* GEM Connection state */
typedef enum class __en_e30_gem_communication_state_state__ : UINT8
{
	en_disabled						= 0,
	en_enabled_not_communicating	= 1,
	en_communicating				= 2,

}	E30_GCSS;
typedef enum class __en_e30_gem_communication_state_hsms__ : INT8
{
	en_inactive						= -1,		/* Handler is not active */
	en_not_connected				= 1,		/* Not connected */
	en_con_not_selected				= 2,		/* connected not selected */
	en_con_selected					= 3,		/* connected and selected */

}	E30_GCSH;

/* Remote Command ID */
typedef enum class __en_e30_gem_remote_command_id__ : UINT8
{
	en_remote_cmd_start				= 0x01,		/* Equipment Start */
	en_remote_cmd_stop				= 0x02,		/* Equipment Stop */
	en_remote_cmd_ppselect			= 0x03,		/* Recipe: PP-Select (Process Program Select) */
	en_remote_cmd_pause				= 0x04,		/* Equipment Pause */
	en_remote_cmd_resume			= 0x05,		/* Equipment Resume */
	en_remote_cmd_abort				= 0x06,		/* Equipment Abort */

}	E30_GRCI;

/* Host Process Program Load Inquiry ACK (Host로부터 Recipe의 적재 요청에 대해 응답 코드 값) */
typedef enum class __en_e30_host_pp_load_inquiry_ack__ : UINT8
{
	en_rg_ok						= 0,	/* accepted */
	en_rg_have						= 1,	/* already have the recipe */
	en_rg_no_space					= 2,	/* no space left for the recipe */
	en_rg_invalid_ppid				= 3,	/* recipe ID is invalid */
	en_rg_busy						= 4,	/* busy, unable to process */
	en_rg_rejected					= 5,	/* recipe rejected */
	en_rg_not_handled				= 6,	/* not handled */

	en_rg_reset						= 0xff,

}	E30_HPLA;

/* Status returned by recipe handlers (The host S7F4 ACKC7 as an enumeration value) */
/* 장비 (Equipment)로부터 수신 (전달)된 레시피 데이터의 정상 수신 여부 값 (Host에서 응답해 준 결과 값) */
typedef enum class __en_e30_status_recipe_handler_ack__ : INT16
{
	en_ra_accepted					= 0,	/* accepted */
	en_ra_rejected					= 1,	/* rejected */
	en_ra_length					= 2,	/* length error */
	en_ra_overflow					= 3,	/* overflow */
	en_ra_invalid_id				= 4,	/* PPID not found */
	en_ra_unsupported				= 5,	/* mode unsupported */
	en_ra_signal_later				= 6,	/* Command will be performed with completion signaled later */
	en_ra_not_handled				= -1,	/* not handled, use this value to let other registered callbacks or CIMConnect handle the callback */

	en_ra_reset						= 0xff,

}	E30_SRHA;

/* Status returned by TerminalMsg handler (장비 (Equipment)로부터 송신된 터미널 메시지에 대한 Host의 응답 코드) */
typedef enum class __en_e30_status_terminalmsg_handler_ack__ : UINT8
{
	en_t_accepted					= 0,	/* message accepted */
	en_t_rejected					= 1,	/* message rejected */
	en_t_unavailable				= 2,	/* specified terminal is unavailable */

	en_t_reset						= 0xff,

}	E30_STHA;

/* Recipe Management Job Code */
typedef enum class __en_e30_recipe_management_job_code__ : UINT8
{
	en_recipe_file_requested		= 0x01,	/* 레시피 파일 요청 응답이 수신 */
	en_recipe_file_received			= 0x02,	/* 레시피 파일 도착 응답이 수신 */
	en_recipe_file_uploaded			= 0x03,	/* 레시피 업로드 요청 후 응답 수신 */
	en_recipe_file_deleted			= 0x04,	/* 레시피 파일 삭제 후 응답이 수신 */

	en_reset						= 0xff,

}	E30_RMJC;

/* Status Value Error Type */
typedef enum class __en_e30_status_value_error_type__ : UINT8
{
	en_caack						= 0x01,	/* Format: U1:1 (invariant) Used by: S3F18 S3F20 S3F22 S3F24 S3F26 S3F28 S3F30 S3F32 S3F34 */
	en_svcack						= 0x02,	/* Format: Bi:1 (invariant) Used by: S14F20 S14F20 S14F21 */
	en_objack						= 0x03,	/* Format: U1:1 (invariant) Used by: S14F2 S14F4 S14F6 S14F8 S14F10 S14F12 S14F14 S14F16 S14F18 S14F26 S14F28 */
	en_acka							= 0x04,	/* Format: B1:1 (invariant) Used by: S10F2 S10F4 S10F6 S10F10 */

}	E30_SVET;


/* --------------------------------------------------------------------------------------------- */
/*                                           E40 (PJM)                                           */
/* --------------------------------------------------------------------------------------------- */

/* E40 Callback Event ID */
typedef enum class __en_e40_callback_event_func_id__ : UINT8
{
	/* ICxE40Callback */
	en_pr_command_callback1			= 0x01,
	en_pr_set_mtrl_order			= 0x02,
	en_pr_set_recipe_variable		= 0x03,
	en_pr_state_change				= 0x04,

	/* ICxE40Callback2 */
	en_pr_command_callback			= 0x11,
	en_pr_command_callback2			= 0x12,

}	E40_CEFI;

/* Process Job State Code */
typedef enum class __en_e40_pjm_process_job_state__ : INT8
{
	en_created						= -1,	/* Process job has been created */
	en_queued						= 0,	/* Process job has been queued */
	en_setting_up					= 1,	/* Process job is setting up */
	en_waiting_for_start			= 2,	/* Process job is waiting for start */
	en_processing					= 3,	/* Process job is in PROCESSING state */
	en_process_completed			= 4,	/* Process job's process is completed */
	en_reserved5					= 5,	/* Reserved (not used) */
	en_pausing						= 6,	/* Process job is in PAUSING state */
	en_paused						= 7,	/* Process job has been paused */
	en_stopping						= 8,	/* Process job is in stopping */
	en_aborting						= 9,	/* Process job is aborting */
	en_stopped						= 10,	/* Process job is in post-active STOPPED state */
	en_aborted						= 11,	/* Process job is in post-active ABORTED state */
	en_processjob_completed			= 12,	/* Process job is completed (extinct) */

}	E40_PPJS;

/* Process Job Control Code */
typedef enum class __en_e40_pjm_process_job_control__ : UINT8
{
	en_abort						= 0x00,	/* Moves Process Job from EXECUTING, PAUSING/PAUSED or STOPPING state to ABORTING state */
	en_cancel						= 0x01,	/* Moves Process Job from QUEUED state to JOB COMPLETED state and removes it */
	en_pause						= 0x02,	/* Moves Process Job from any EXECUTING state to PAUSING state */
	en_paused						= 0x03,	/* Moves Process Job from PAUSING state to PAUSED state */
	en_resume						= 0x04,	/* Moves Process Job from PAUSING or PAUSED state to EXECUTING state */
	en_start						= 0x05,	/* Moves Process Job from WAITING FOR START state to PROCESSING state */
	en_stop							= 0x06,	/* Moves Process Job from any EXECUTING or PAUSING/PAUSED state to STOPPING state */
	en_stopped						= 0x07,	/*  */
	en_aborted						= 0x08,	/*  */

}	E40_PPJC;

/* E40 Material Type code (MF) */
typedef enum class __en_e40_pjm_process_material_type__ : UINT8
{
	en_wafer						= 0x01,	/* Quantities in wafers */
	en_cassette						= 0x02,	/* Quantities in cassette */
	en_die_chip						= 0x03,	/* Quantities in die or chips */
	en_boat							= 0x04,	/* Quantities in boats */
	en_ingot						= 0x05,	/* Quantities in ingots */
	en_leadframe					= 0x06,	/* Quantities in lead frames */
	en_lot							= 0x07,	/* Quantities in lots */
	en_magazine						= 0x08,	/* Quantities in magazines */
	en_package						= 0x09,	/* Quantities in packages */
	en_plate						= 0x0a,	/* Quantities in plates */
	en_tube							= 0x0b,	/* Quantities in tubes */
	en_waterframe					= 0x0c,	/* Quantities in water frames */
	en_carrier						= 0x0d,	/* Quantities in carriers */
	en_substrate					= 0x0e,	/* Quantities in substrates */

}	E40_PPMT;

/* E40 Callback Excute Command Source */
typedef enum class __en_e40_pjm_execute_command_source__ : UINT8
{
	en_unknown						= 0x00,	/* Unknown Source */
	en_local_direct					= 0x01,	/* Command Issued through the PJM API */
	en_local_control_job			= 0x02,	/* Command Issued through the CJM APIs */
	en_host_direct					= 0x03,	/* Command issued via Message from the host using PJM controls */
	en_host_control_job				= 0x04,	/* Command issued via Message from the host using CJM controls */

}	E40_PECS,	E94_PECS;

/* E40 Callback Command Code */
typedef enum class __en_e40_pjm_callback_command_code__ : UINT8
{
	en_create						= 0x00,	/* Creation of new Process Job is requested */
	en_setup						= 0x01,	/* Request for Process Job to start pre-conditioning */
	en_waitingforstart				= 0x02,	/* Notify equipment when Process Job goes from SETTING UP to WAITING FOR START state */
	en_start						= 0x03,	/* Request to start processing for Process Job */
	en_pause						= 0x04,	/* Request to pause processing of Process Job. */
	en_resume						= 0x05,	/* Request to resume Process Job (it will return to state, previous to pause request) */
	en_stop							= 0x06,	/* Request to stop Process Job */
	en_abort						= 0x07,	/* Request to abort Process Job */
	en_cancelcancel					= 0x08,	/* Host sends S16F5 CANCEL command to cancel a queued process job. State Transition 18 */
	en_cancelstop					= 0x09,	/* Host sends S16F5 STOP command to cancel a queued process job. State Transition 18 */
	en_cancelabort					= 0x0a,	/* Host sends S16F5 ABORT command to cancel a queued process job. State Transition 18 */
	en_canceldequeue				= 0x0b,	/* Host sends S16F17 PRJobDequeue. State Transition 18 */

}	E40_PCCC;

/* Event notification type */
typedef enum class __en_e40_pjm_event_notification_type__ : UINT8
{
	en_evt_none						= 0x00,	/* No event notification */
	en_evt_e30						= 0x01,	/* Use only E30 event notification */
	en_evt_e40						= 0x02,	/* Use only E40 event notification */
	en_evt_both						= 0x03,	/* Use both E30 and E40 event notification */

}	E40_PENT;

/* Identifier of the specific event which occurred (PREVENTID) */
typedef enum class __en_e40_pjm_specific_event_which_occurred__ : UINT8
{
	en_pr_job_waiting_for_material	= 0x01,	/* Process job is waiting for material */
	en_pr_job_state_change			= 0x02,	/* Process job has changed state */

}	E40_PSEO;

/* --------------------------------------------------------------------------------------------- */
/*                                           E87 (CMS)                                           */
/* --------------------------------------------------------------------------------------------- */

/* E87 Callback Event ID */
typedef enum class __en_e87_callback_event_func_id__ : UINT8
{
	/* ICxE87Callback */
	en_bind							= 0x01,
	en_cancel_all_carrier_out		= 0x02,
	en_cancel_bind					= 0x03,	/* Cancel Bind Service */
	en_cancel_carrier				= 0x04,	/* CANCELCARRIER Service */
	en_cancel_carrier_at_port		= 0x05,
	en_cancel_carrier_noti			= 0x06,
	en_cancel_carrier_out			= 0x07,
	en_cancel_reserve_at_port		= 0x08,
	en_carrier_in					= 0x09,
	en_carrier_noti					= 0x0a,
	en_carrier_out					= 0x0b,
	en_carrier_recreate				= 0x0c,
	en_carrier_release				= 0x0d,
	en_carrier_tag_read_data		= 0x0e,
	en_carrier_tag_write_data		= 0x0f,
	en_change_access_mode			= 0x10,
	en_change_service_status		= 0x11,
	en_proceed_with_carrier			= 0x12,
	en_reserve_at_port				= 0x13,
	/* ICxE87Callback2 */
	en_alarm_changed				= 0x21,
	en_carrier_id_verified			= 0x22,
	en_slot_map_verified			= 0x23,
	/* ICxE87Callback3 */
	en_in_service_transfer_state	= 0x31,
	en_state_change					= 0x32,
	/* ICxE87Callback4 */
	en_recreate_finalize			= 0x41,
	/* ICxE87Callback5 */
	en_cancel_carrier2				= 0x51,
	en_cancel_carrier_at_port2		= 0x52,
	/* ICxE87Callback6 */
	en_change_access_mode2			= 0x61,
	/* ICxE87Callback7 */
	en_bind2						= 0x71,
	en_carrier_noti2				= 0x72,
	en_proceed_with_carrier2		= 0x73,
	/* ICxE87Callback8 */
	en_carrier_removed				= 0x81,
	en_port_alarm_changed			= 0x82,
	/* ICxE87Callback9 */
	en_proceed_with_carrier3		= 0x91,
	/* ICxE87Callback10 */
	en_change_access_mode3			= 0xa1,
	/* ICxE87AsyncCarrierTagCallback */
	en_async_carrier_tag_read		= 0xb1,
	en_async_carrier_tag_write		= 0xb2,

}	E87_CEFI;

/* Carrier access status */
typedef enum class __en_e87_cms_carrier_access_status__ : UINT8
{
	en_carrier_not_accessed			= 0x00,	/* The carrier has not been accessed */
	en_carrier_in_access			= 0x01,	/* The carrier is being accessed */
	en_carrier_complete				= 0x02,	/* The equipment finishes accessing the carrier normally */
	en_carrier_stopped				= 0x03,	/* The equipment finishes accessing the carrier abnormally */

}	E87_CCAS;

/* Carrier event status */
typedef enum class __en_e87_cms_carrier_event_status__ : UINT8
{
	en_carrier_closed				= 0x01,
	en_carrier_opened				= 0x02,
	en_carrier_clamped				= 0x03,
	en_carrier_unclamped			= 0x04,
	en_carrier_approached			= 0x05,	/* Carrier Approaching Complete */
	en_carrier_process_changed		= 0x06,	/* Carrier Processing Status Changed */
#if 0	/* Currently not support (List 로 구성된 Property parameter가 들어가야 됨. 복잡함) */
	en_carrier_loc_changed			= 0x07,	/* Carrier Approaching Complete */
	en_carrier_id_read_fail			= 0x08,
#endif
	en_carrier_id_read_enable		= 0x09,	/* ID Reader Available */
	en_carrier_id_read_disable		= 0x0a,	/* ID Reader Unavailable */
	en_carrier_id_unknown			= 0x0b,	/* a carrier arrives at a not associated loadport and the carrierID reader is not available */
	en_carrier_id_duplicate			= 0x0c,	/* a carrier has been received with the same id as a carrier being processed */
	
}	E87_CCES;

/* CIM300 Carrier Slot Map Status */
typedef enum class __en_e87_carrier_slot_map_status__ : UINT8
{
	en_undefined					= 0x00,
	en_empty						= 0x01,
	en_not_empty					= 0x02,
	en_correctly_occupied			= 0x03,
	en_double_slotted				= 0x04,
	en_cross_slotted				= 0x05,

}	E87_CSMS;
/* StateChange : Codes for the different state machines */
typedef enum class __en_e87_code_different_stat_machine__ : UINT8
{
	en_e87lptsm						= 0x00,	/* LPTSM	: load port transfer state machine */
	en_e87cidsm						= 0x01,	/* COSM		: carrier object id status state machine */
	en_e87csmsm						= 0x02,	/* COSM		: carrier object slot map status state machine */
	en_e87casm						= 0x03,	/* CASM		: carrier object accessing state machine */
	en_e87lprsm						= 0x04,	/* LPRSM	: load port reservation state machine */
	en_e87lpcasm					= 0x05,	/* LPCASM	: load port/carrier association state machine */
	en_e87lpasm						= 0x06,	/* LPASM	: load port access mode state model */
	en_undefined					= 0xff,

}	E87_CDSM;
/* StateChange : Load Port Transfer State */
/* note : IN SERVICE인 경우, 내부적으로 아래와 같은 상태가 같이 포함된 경우임 */
/*		  READY TO UNLOAD, READY TO LoAD, TRANSSFER READY, TRANSFER BLOCKED   */
typedef enum class __en_e87_load_port_transfer_state__ : UINT8
{
    en_lp_out_of_service				= 0x00,	/* The load port is out of service */
    en_lp_transfer_blocked				= 0x01,	/* The load port is blocked for transfer */
    en_lp_ready_to_load					= 0x02,	/* The load port is ready to load */
    en_lp_ready_to_unload				= 0x03,	/* The load port is ready to unload */
    en_lp_in_service					= 0x04,	/* The load port is in service */
    en_lp_transfer_ready				= 0x05,	/* The load port is ready for transfer */

}	E87_LPTS;

/* StateChange : Carrier ID Verfication Status */
typedef enum class __en_e87_carrier_id_verfication_status__ : UINT8
{
    en_id_not_read						= 0x00,	/* The carrier ID is not read */
    en_id_wait_for_host					= 0x01,	/* The carrier is waiting for the host */
    en_id_verification_ok				= 0x02,	/* The carrier ID is verified */
    en_id_verification_failed			= 0x03,	/* The carrier ID verification has failed */

}	E87_CIVS;
/* StateChange : Slot Map Verification Status */
typedef enum class __en_e87_slot_map_verification_status__ : UINT8
{
    en_sm_not_read						= 0x00,	/* The slot map is not read */
    en_sm_wait_for_host					= 0x01,	/* The slot map is waiting for the host */
    en_sm_verification_ok				= 0x02,	/* The slot map is verified */
    en_sm_verification_failed			= 0x03,	/* The slot map verification has failed */

}	E87_SMVS;
/* StateChange : Carrier accessing status */
typedef enum class __en_e87_carrier_id_accessing_status__ : UINT8
{
    en_not_accessed						= 0x00,	/* The carrier has not been accessed */
    en_in_access						= 0x01,	/* The carrier is being accessed */
    en_carrier_complete					= 0x02,	/* The equipment finishes accessing the carrier normally */
    en_carrier_stopped					= 0x03,	/* The equipment finishes accessing the carrier abnormally */

}	E87_CIAS;
/* StateChange : Load port reservation state */
typedef enum class __en_e87_load_port_reservation_state__ : UINT8
{
    en_lp_not_reserved					= 0x00,	/* The load port is not reserved */
    en_lp_reserved						= 0x01,	/* The load port is reserved */

}	E87_LPRS;
/* StateChange : Load port-carrier association status */
typedef enum class __en_e87_load_port_carrier_association_status__ : UINT8
{
	en_not_associated					= 0x00,	/* The load port is not associated with a carrier */
	en_associated						= 0x01,	/* The load port is associated with a carrier */

}	E87_LPAS;
/* StateChange : Load port access mode */
typedef enum class __en_e87_load_port_acess_mode__ : UINT8
{
	en_manual							= 0x00,	/* The load port is accessed manually */
	en_auto								= 0x01,	/* The load port is accessed automatically */

}	E87_LPAM;

/* --------------------------------------------------------------------------------------------- */
/*                                           E90 (STS)                                           */
/* --------------------------------------------------------------------------------------------- */

/* E90 Callback Event ID */
typedef enum class __en_e90_callback_event_func_id__ : UINT8
{
	/* ICxE90Callback */
	en_command_callback			= 0x01,
	en_substrate_state_change	= 0x02,

}	E90_CEFI;

/* E90 possible_substrate_command */
typedef enum class __en_e90_sts_possible_substrate_command__ : UINT8
{
	en_proceed_with_substrate		= 0x00,	/* Substrate ID is valid, and ready to be used */
	en_cancel_substrate				= 0x01,	/* Substrate ID is invalid, and should be rejected */

}	E90_SPSC;

/* Substrate transport state for CIM300 */
typedef enum class __en_e90_substrate_transport_state__ : INT8
{
	en_no_state						= -1,	/* The substrate not have transport state */
	en_source						= 0,	/* The substrate is at source */
	en_work							= 1,	/* The substrate is at work */
	en_destination					= 2,	/* The substrate is at destination */

}	E90_SSTS;

/* Substrate processing state for CIM300 */
typedef enum class __en_e90_substrate_processing_state__ : INT32
{
	en_no_state						= -1,	/* The substrate does not have processing state */
	en_needs_processing				= 0,	/* Initial value to indicate that processing in the equipment is required and has not started */
	en_in_process					= 1,	/* Processing has started */
	en_processed					= 2,	/* Processing has completed normally */
	en_aborted						= 3,	/* Processing has terminated abnormally */
	en_stopped						= 4,	/* Processing has terminated at completion of certain processing step */
	en_rejected						= 5,	/* The substrate has been identified as needing special treatment */
	en_lost							= 6,	/* The substrate has been removed from the equipment by an external agent or it is physically missing */
	en_skipped						= 7,	/* The substrate was not processed */

}	E90_SSPS;

/* Substrate Location State */
typedef enum class __en_e90_substrate_location_state__ : UINT8
{
	en_unoccupied					= 0,	/* The substrate location is not occupied */
	en_occupied						= 1,	/* The substrate location is occupied */
	en_no_state						= 2,	/* The substrate location does not exist */

}	E90_SSLS;

/* States in the new state model */
typedef enum class __en_e90_subst_id_status_model__ : INT8
{
	en_no_state						= -1,	/* substrate id is uninitialized */
	en_not_confirmed				= 0,	/* substrate id is created, no verification has occurred */
	en_waiting_for_host				= 1,	/* substrate verification must come from the host */
	en_confirmed					= 2,	/* substrate id has been successfully verified */
	en_confirmation_failed			= 3,	/* substrate id verification failed, and should be rejected */

}	E90_SISM;

/* The various e90 state machines */
typedef enum class __en_e90_various_state_machine_code__ : UINT8
{
	en_no_sm						= 0,	/* no state model */
	en_substrate_transport_sm		= 1,	/* substrate transport state machine */
	en_substrate_processing_sm		= 2,	/* substrate processing state machine */
	en_substrate_reader_sm			= 3,	/* substrate reader state machine */
	en_substrate_location_sm		= 4,	/* substrate location state machine */
	en_batch_location_sm			= 5,	/* batch location state machine */

}	E90_VSMC;

/* Equipment Substrate Type */
typedef enum class __en_e90_equipment_material_substrate_type__ : UINT8
{
	en_water						= 0,	/* Substrate is wafer */
	en_flat_panel					= 1,	/* Substrate is flat panel */
	en_cd							= 2,	/* Substrate is CD */
	en_mask							= 3,	/* Substrate is mask */

}	E90_EMST;

/* --------------------------------------------------------------------------------------------- */
/*                                           E94 (CJM)                                           */
/* --------------------------------------------------------------------------------------------- */

/* E94 Callback Event ID */
typedef enum class __en_e94_callback_event_func_id__ : UINT8
{
	/* ICxE94ControlJobCB */
	en_cj_create					= 0x01,
	en_cj_deselect					= 0x02,
	en_cj_hoq						= 0x03,
	en_cj_state_change				= 0x04,
	en_get_next_pj_job_id			= 0x05,
	/* ICxE94ControlJobCB2 */
	en_cj_command_callback2			= 0x11,
	/* ICxE94ControlJobCB3 */
	en_cj_command_callback3			= 0x21,
	/* ICxE94ControlJobCB4 */
	en_attr_change_noti				= 0x31,
	en_set_attr_change_request		= 0x32,
	/* ICxE94ControlProcessJobCB */
	en_abort_process_job			= 0x41,
	en_cj_delete_pr_job				= 0x42,
	en_start_process_job			= 0x43,
	en_stop_process_job				= 0x44,

}	E94_CEFI;

/* Filter code for listing control job's process jobs */
typedef enum class __en_e94_cjm_list_process_jobs__ : UINT8
{
	en_list_all						= 0,	/* List of all process jobs of the control job */
	en_list_queued					= 1,	/* List of all queued process jobs of the control job */
	en_list_active					= 2,	/* List of all active process jobs of the control job */
	en_list_completed				= 3,	/* List of all completed process jobs of the control job */

}	E94_CLPJ;

/* Filter code for listing control job */
typedef enum class __en_e94_cjm_list_control_jobs__ : UINT8
{
	en_list_queued					= 1,	/* List of all queued control jobs */
	en_list_active					= 2,	/* List of all active control jobs */
	en_list_completed				= 4,	/* List of all completed control jobs */
	en_list_all						= 7,	/* List of all control jobs (en_clcj_list_queued|en_clcj_list_active|en_clcj_list_completed) */

}	E94_CLCJ;

/* Control Job State Code */
typedef enum class __en_e94_cjm_control_job_state__ : INT8
{
	en_unknown						= -1,	/* (No state) */
	en_queued						= 0,	/* QUEUED state */
	en_selected						= 1,	/* SELECTED state */
	en_waitingforstart				= 2,	/* WAITINGFORSTART state */
	en_executing					= 3,	/* EXECUTING state */
	en_paused						= 4,	/* PAUSED state */
	en_completed					= 5,	/* COMPLETED state */

}	E94_CCJS;

/* E94 control job attribute ProcessOrderMgmt code */
typedef enum class __en_e94_cjm_process_order_management__ : UINT8
{
	en_arrival						= 1,	/* Process jobs are initiated according to the material arrival status */
	en_optimize						= 2,	/* Process jobs are initiated according to the equipment's resources */
	en_list							= 3,	/* Process jobs are initiated according to the control job's list */

}	E94_CPOM;

typedef enum class __en_e94_cjm_commands__ : UINT8
{
	en_none							= 0,	/* no command */
	en_abort						= 1,	/* Abort Control Job */
	en_cancel						= 2,	/* Cancel Control Job */
	en_pause						= 3,	/* Pause Control Job */
	en_resume						= 4,	/* Resume Control Job */
	en_start						= 5,	/* Start Control Job */
	en_stop							= 6,	/* Stop Control Job */
	en_create						= 7,	/* Create Control Job */
	en_hoq							= 8,	/* HOQ (Head Of Queue) Control Job */
	en_deselect						= 9,	/* Deselect Control Job */

}	E94_CCMD;

/* Enumeration identifying the source of the command execution (E94CJMLib::Enumerations::E94CJAction) */
typedef enum class __en_e94_cjm_control_job_action__ : UINT8
{
	en_save_jobs					= 0,	/* Save control job's process job(s) when it is aborted, stopped or canceled */
	en_remove_jobs					= 1,	/* Remove control job's process job(s) when it is aborted, stopped or canceled */

}	E94_CCJA;

/* --------------------------------------------------------------------------------------------- */
/*                                         E116 (EPT)                                            */
/* --------------------------------------------------------------------------------------------- */

/* EPT State (EPT task types for BUSY state) */
typedef enum class __en_e116_ept_task_types_busy_state__ : UINT8
{
	en_ept_notask					= 0,	/* no task */
	en_ept_unspecified				= 1,	/* unspecified */
	en_ept_process					= 2,	/* processing, adding value */
	en_ept_support					= 3,	/* support, incapable of adding value */
	en_ept_maintenance				= 4,	/* equipment maintenance, e.g. clean cycle */
	en_ept_diagnostics				= 5,	/* equipment diagnostics, e.g. health check */
	en_ept_waiting					= 6,	/* normal waiting period */

}	E116_ETBS;

/* ---------------------------------------------------------------------------------------------- */
/*                                            구조체                                              */
/* ---------------------------------------------------------------------------------------------- */

#pragma pack (push, 8)

/* CIM Lib Log message*/
typedef struct __st_cim_library_log_message__
{
	UINT64			t_year:13;				/* 4 자리 (0 ~ 8912) */
	UINT64			t_month:5;				/* 2 자리 (0 ~ 32) */
	UINT64			t_day:5;				/* 2 자리 (0 ~ 32) */
	UINT64			t_hour:5;				/* 2 자리 (0 ~ 32) */
	UINT64			t_minute:6;				/* 2 자리 (0 ~ 64) */
	UINT64			t_second:6;				/* 2 자리 (0 ~ 64) */
	UINT64			t_msecond:10;			/* 3 자리 (0 ~ 1024) */
	UINT64			level:2;				/* 1 자리 (0 ~ 3) : 0 (Normal), 1 (Warning), 2 (Error), 3 (Unknown) */
	UINT64			bit_reserved:12;
	PTCHAR			mesg;

	/*
		설명 : 메모리 생성
		변수 : None
		반환 : None
	*/
	VOID Init()
	{
		//mesg	= (PTCHAR)::Alloc(sizeof(TCHAR) * CIM_LOG_MESG_SIZE+ sizeof(TCHAR));
		mesg = new TCHAR[CIM_LOG_MESG_SIZE + 1];
		ASSERT(mesg);
		wmemset(mesg, 0x00, CIM_LOG_MESG_SIZE+1);
	}

	/*
		설명 : 메모리 해제
		변수 : None
		반환 : None
	*/
	VOID Release()
	{
		if (mesg)	delete mesg;//::Free(mesg);
	}

}	STG_CLLM,	*LPG_CLLM;

/* EPJ (Equipment Project) File 정보 관리 */
typedef struct __st_cim300_equipment_project_file__
{
	UINT32				id;				/* Event, Alarm, Connect, Variables, etc */
	CHAR				name;			/* ID에 매핑되는 이름 */

}	STG_CEPF,	*LPG_CEPF;

/* Handle and Count Information for ListAt */
typedef struct __st_cim300_listat_handle_count__
{
	LONG				handle;			/* Data가 저장된 ListAt Handle */
	LONG				index;			/* Data가 저장된 ListAt Index */
	LONG				level;			/* Data가 저장된 깊이 즉, <XML> Tag의 깊이라고 생각하면 됨 */
										/* <XML> ... </XML> 등 이런 Tag 값을 깊이 값 (0-based) */
	LONG				l_reserved;

}	STG_CLHC,	*LPG_CLHC;

/* CIM300 Callback Event Data */
#if 0
typedef struct __st_cim300_call_event_data__
{
	UINT32				kind;			/* Event Kind (E30_GCEK)											*/
	UINT32				stat;			/* 작업 성공 여부, 상태 값, 기타 값 등 kind 값에 따라 의미가 달라짐	*/
	UINT32				u32_reserved;
										/* kind 값에 따라, 값의 성격이 달라짐								*/
	LONG				d_id;			/* Host ID (Common : 0, 1 or Later)									*/
										/* if (kind == alarm)	h_id = alarm_id								*/
	LONG				m_id;			/* Variables, Events, Alarms, etc : Identification					*/
										/* ---------------------------------------------------------------- */
										/* if (kind == alarm)	m_id = alarm_code							*/
										/* case of alarm_code : If bit 8 is 1, then the alarm is set.		*/
										/*						If bit 8 is 0, then the alarm is cleared.	*/
										/*						Bits 7-1 contain the alarm category.		*/
										/*						Ignore the high-order word					*/
										/* ---------------------------------------------------------------- */
										/* if (kind == termmsg)	m_id = term_id								*/
										/* ---------------------------------------------------------------- */
	UINT32				size;			/* 'data' 에 저장된 버퍼 크기                                       */
	PTCHAR				data;			/* kind, v_id 등에 값에 따라, 다양한 문자열 정보가 저장되는 버퍼 임 */
}	STG_CCED,	*LPG_CCED;
#else
/* Callback Event : E30 (State Change) */
typedef struct __st_cim300_event_e30_state_change__
{
	UINT8				machine;	/* E30_SSMS : 1 byte */
	UINT8				u8_reserved[7];
	LONG				conn_id;
	LONG				new_state;	/* machine 값에 따라 값의 성격이 달라짐 */
/*
									switch (machine)
									{
									case sisNO_SM					:	break;
									case sisSUBSTRATE_TRANSPORT_SM	:	SubstrateTransportStateEnum;	break;
									case sisSUBSTRATE_PROCESSING_SM	:	SubstrateProcessingStateEnum;	break;
									case sisSUBSTRATE_READER_SM		:	?
									case sisSUBSTRATE_LOCATION_SM	:	SubstrateLocationStateEnum;		break;
									case sisBATCH_LOCATION_SM		:	break;	// 이 값은 무시
									}
*/
}	STG_E30_SC,	*LPG_E30_SC;
/* Callback Event : E30 (Value Change) */
typedef struct __st_cim300_event_e30_value_change__
{
	LONG				conn_id;
	LONG				var_id;
	UINT8				u8_reserved[8];

}	STG_E30_VC,	*LPG_E30_VC;
/* Callback Event : E30 (Alarm Change) */
typedef struct __st_cim300_event_e30_alarm_change__
{
	LONG				alarm_id;	/* Alarm Variable ID */
	LONG				alarm_cd;	/* Alarm Code */
	UINT8				u8_reserved[8];

}	STG_E30_AC,	*LPG_E30_AC;
/* Callback Event : E30 (Terminal Message) */
typedef struct __st_cim300_event_e30_terminal_message__
{
	LONG				conn_id;
	LONG				term_id;	/* Terminal ID, Common의 경우, 무조건 0 값? */
	UINT8				u8_reserved[8];

}	STG_E30_TM,	*LPG_E30_TM;
/* Callback Event : E30 (User Message) */
typedef struct __st_cim300_event_e30_message_received__
{
	LONG				conn_id;
	LONG				mesg_id;	/* Message ID */
	LONG				reply;		/* Reply 값 (mNotHandled:0, mReplyLater:1, mReply:2, mNoReply:3) */
	LONG				l_reserved;

}	STG_E30_MR,	*LPG_E30_MR;
/* Callback Event : E30 (Remote Command) */
typedef struct __st_cim300_event_e30_remote_command__
{
	LONG				conn_id;
	LONG				rcmd_id;	/* Remote Command ID (E30_GRCI) */
	UINT8				u8_reserved[8];

}	STG_E30_RC,	*LPG_E30_RC;
/* Callback Event : E30 (PPData : Process Program Data) */
typedef struct __st_cim300_event_e30_process_program__
{
	LONG				conn_id;
	LONG				l_reserved;
	UINT8				u8_reserved[8];

}	STG_E30_PP,	*LPG_E30_PP;
/* Callback Event : E40 (PR Command Callback) */
typedef struct __st_cim300_event_e40_process_command_callback__
{
	UINT8				cmd_src;		/* E40_PECS : Enumeration identifying the source of the command execution */
	UINT8				cmd;			/* E40_PCCC : Callback command code */
	UINT8				u8_reserved[6];
	PVOID				param_cmd;		/* CE40CmdParam Object Clase or NULL */

}	STG_E40_PC,	*LPG_E40_PC, STG_E94_PC,	*LPG_E94_PC;
/* Callback Event : E40 (PR State Change )*/
typedef union __st_cim300_event_e40_process_state_change__
{
	UINT8				u8_reserved[8];
	LONG				l_reserved;
	LONG				state;		/* E40_PPJS */

}	STG_E40_PS,	*LPG_E40_PS;

/* Callback Event : E87 (State Change) */
typedef struct __st_cim300_event_e87_state_change__
{
	UINT8				machine;	/* E87_CDSM : 1 byte */
	UINT8				u8_reserved[7];
	LONG				port_id;	/* The load port identifier (0 if port ID is not known) */
	LONG				state;

}	STG_E87_SC,	*LPG_E87_SC;
/* Callback Event : E87 (Others Callback Function) */
typedef struct __st_cim300_event_e87_callback_function__
{
	UINT8				u8_reserved[8];
	LONG				port_id;	/* The load port identifier (0 if port ID is not known) */
	LONG				state;

}	STG_E87_CF,	*LPG_E87_CF;
/* Callback Event : E90 (State Change) */
typedef struct __st_cim300_event_e90_state_change__
{
	UINT8				machine;	/* E90_VSMC : 1 byte */
	UINT8				u8_reserved[7];
	LONG				state;
	LONG				l_reserved;

}	STG_E90_SC,	*LPG_E90_SC;
/* Callback Event : E90 (Command Callback) */
typedef struct __st_cim300_event_e90_command_callback__
{
	UINT8				cmd;			/* E90_SPSC : Possible Substrate Commands */
	UINT8				u8_reserved[7];
	LONG				l1_reserved;	/* reserved */
	LONG				l2_reserved;	/* reserved */

}	STG_E90_CC,	*LPG_E90_CC;
/* Callback Event : E94 (State Change) */
typedef struct __st_cim300_event_e94_state_change__
{
	UINT8				u8_reserved[8];
	LONG				l_reserved;
	LONG				state;		/* E94_CCJS */

}	STG_E94_SC,	*LPG_E94_SC;
/* Callback Event : E94 (CJProcessOrderMgmt) */
typedef union __st_cim300_event_e94_process_oerder_management__
{
	UINT8				u8_reserved[8];
	LONG				l_reserved;
	LONG				state;		/* E94_CPOM */

}	STG_E94_PO,	*LPG_E94_PO;

/* Callback Event For UNION */
typedef union __un_cim300_callback_event_data__
{
	STG_E30_SC			e30_sc;		/* Event : E30_StateChange */
	STG_E30_VC			e30_vc;		/* Event : E30_ValueChange */
	STG_E30_VC			e30_ec;		/* Event : E30_ECChange (Equipment Constant) */
	STG_E30_AC			e30_ac;		/* Event : E30_AlarmChange */
	STG_E30_TM			e30_tm;		/* Event : E30_TerminalMessage */
	STG_E30_MR			e30_mr;		/* Event : E30_MessageReceived */
	STG_E30_RC			e30_rc;		/* Event : E30_RemoteCommand */
	STG_E30_PP			e30_pp;		/* Event : E30_PPData */

	STG_E40_PC			e40_pc;		/* Event : E40_PRCommandCallback */
	STG_E40_PS			e40_ps;		/* Event : E40_PRStateChange */

	STG_E87_SC			e87_sc;		/* Event : E87_StateChange */
	STG_E87_CF			e87_cf;		/* Event : E87_Callback.Function */

	STG_E90_SC			e90_sc;		/* Event : E90_StateChange */
	STG_E90_CC			e90_cc;		/* Event : E90_CommandCallback */

	STG_E94_SC			e94_sc;		/* Event : E94_StateChange */
	STG_E94_PO			e94_po;		/* Event : E94_CJProcessOrderMgmt */
	STG_E94_PC			e94_pc;		/* Event : E90_CJCommandCallback */

}	UNG_CCED,	*LNG_CCED;
/* Callback Event For UNION */
typedef struct __st_cim300_callback_event_data__
{
	UINT8				id_data1;			/* ENG_GCPI for str_data1 */
	UINT8				id_data2;			/* ENG_GCPI for str_data1 */
	UINT8				cbf_id;				/* Callback Function ID (Exx_CFEI) */
	UINT8				u8_reserved[5];

	UINT16				exx_id;				/* Event Main ID (ENG_ESSC) */
	UINT16				u16_reserved[3];	/* Event Main ID (ENG_ESSC) */

	UNG_CCED			evt_data;	/* Event Data */

	PTCHAR				str_data1;	/* String Data. ex> Variable Name, Carrier ID, Message (Terminal, Secs Command), Etc */
	PTCHAR				str_data2;	/* String Data. ex> Variable Name, Carrier ID, Message (Terminal, Secs Command), Etc */

	PVOID				param_obj;	/* Parameter Object Class (일반적인 이벤트는 NULL, Parameter List의 경우 값 저장) */

}	STG_CCED,	*LPG_CCED;
#endif

/* EPJ Variable Information */
typedef union __un_cim300_variable_value_type__
{
	INT8				i8_value;
	UINT8				u8_value;
	UINT8				bi_value;	/* binary value */
	UINT8				u8_reserved[5];

	INT16				i16_value;
	UINT16				u16_value;
	UINT16				u16_reserved[2];

	INT32				i32_value;
	UINT32				u32_value;

	BOOL				b_value;
	FLOAT				f_value;

	INT64				i64_value;
	UINT64				u64_value;

	DOUBLE				d_value;

	TCHAR				s_value[CIM_VAL_ASCII_SIZE];

	/*
	 desc : 값 저장 (숫자값)
	 parm : type	- [in]  Data Value Type
			value	- [in]  Data Value
	 반환: None
	*/
	VOID SetValue(E30_GPVT type, PVOID value)
	{
		/* 저장되는 값 형식에 따라 */
		switch (type)
		{
		case E30_GPVT::en_b	:	b_value		= *(PBOOL)value;	break;
		case E30_GPVT::en_bi	:	bi_value	= *(PUINT8)value;	break;

		case E30_GPVT::en_i1	:	i8_value	= *(PINT8)value;	break;
		case E30_GPVT::en_i2	:	i16_value	= *(PINT16)value;	break;
		case E30_GPVT::en_i4	:	i32_value	= *(PINT32)value;	break;
		case E30_GPVT::en_i8	:	i64_value	= *(PINT64)value;	break;

		case E30_GPVT::en_u1	:	u8_value	= *(PUINT8)value;	break;
		case E30_GPVT::en_u2	:	u16_value	= *(PUINT16)value;	break;
		case E30_GPVT::en_u4	:	u32_value	= *(PUINT32)value;	break;
		case E30_GPVT::en_u8	:	u64_value	= *(PUINT64)value;	break;

		case E30_GPVT::en_f4	:	f_value		= *(PFLOAT)value;	break;
		case E30_GPVT::en_f8	:	d_value		= *(DOUBLE*)value;	break;
		}
	}

	/*
	 desc : 값 저장 (숫자값)
	 parm : type	- [in]  Data Value Type
			value	- [in]  Data Value
	 반환: None
	*/
	VOID SetText(E30_GPVT type, PTCHAR value)
	{
		/* 저장되는 값 형식에 따라 */
		switch (type)
		{
		case E30_GPVT::en_b		:	b_value		= (BOOL)_wtoi(value);	break;
		case E30_GPVT::en_bi	:	bi_value	= (UINT8)_wtoi(value);	break;

		case E30_GPVT::en_i1	:	i8_value	= (UINT8)_wtoi(value);	break;
		case E30_GPVT::en_i2	:	i16_value	= (INT16)_wtoi(value);	break;
		case E30_GPVT::en_i4	:	i32_value	= (INT32)_wtoi(value);	break;
		case E30_GPVT::en_i8	:	i64_value	= (INT64)_wtoi(value);	break;

		case E30_GPVT::en_u1	:	u8_value	= (UINT8)_wtoi(value);	break;
		case E30_GPVT::en_u2	:	u16_value	= (UINT16)_wtoi(value);	break;
		case E30_GPVT::en_u4	:	u32_value	= (UINT32)_wtoi(value);	break;
		case E30_GPVT::en_u8	:	u64_value	= (UINT64)_wtoi(value);	break;

		case E30_GPVT::en_f4	:	f_value		= (FLOAT)_wtof(value);	break;
		case E30_GPVT::en_f8	:	d_value		= (DOUBLE)_wtof(value);	break;
		case E30_GPVT::en_w		:
		case E30_GPVT::en_a		:	swprintf_s(s_value, CIM_VAL_ASCII_SIZE, L"%s", value);	break;
		}
	}

}	UNG_CVVT,	*LNG_CVVT;
typedef struct __st_cim300_epj_variable_information__
{
	UINT8				is_persistent:1;
	UINT8				is_private:1;
	UINT8				b_reserved:6;

	UINT8				var_type;		/* Variable Type (E30_GVTC) */
	UINT8				u8_reserved[7];

	UINT32				var_id;			/* Variable ID */
	UINT32				val_type;		/* Value Type (E30_GPVT) */

	UNG_CVVT			current_val;	/* Current Value */
	UNG_CVVT			default_val;	/* Default Value */

	PTCHAR				var_name;		/* Variable Name */
	PTCHAR				var_desc;		/* Variable Description */
	PTCHAR				cur_val_str;	/* Default Value String */
	PTCHAR				def_val_str;	/* Default Value String */

	/*
	 desc : 메모리 생성
	 parm : None
	 retn : None
	*/
	VOID Init()
	{
		var_type	= 0x00;
		val_type	= 0x00;
		var_id		= 0;
		current_val.u64_value	= 0;
		default_val.u64_value	= 0;

		/* 메모리 할당 */
		var_name	= new TCHAR[CIM_CMD_NAME_SIZE+1];			//(PTCHAR)::Alloc(sizeof(TCHAR)*CIM_CMD_NAME_SIZE+sizeof(TCHAR));
		var_desc	= new TCHAR[CIM_CMD_DESC_SIZE+1];			//(PTCHAR)::Alloc(sizeof(TCHAR)*CIM_CMD_DESC_SIZE+sizeof(TCHAR));
		cur_val_str	= new TCHAR[CIM_VAL_ASCII_SIZE+1];			//(PTCHAR)::Alloc(sizeof(TCHAR)*CIM_VAL_ASCII_SIZE+sizeof(TCHAR));
		def_val_str	= new TCHAR[CIM_VAL_ASCII_SIZE+1];			//(PTCHAR)::Alloc(sizeof(TCHAR)*CIM_VAL_ASCII_SIZE+sizeof(TCHAR));
		
		wmemset(var_name,	0x00, CIM_CMD_NAME_SIZE+1);
		wmemset(var_desc,	0x00, CIM_CMD_DESC_SIZE+1);
		wmemset(cur_val_str,0x00, CIM_VAL_ASCII_SIZE+1);
		wmemset(def_val_str,0x00, CIM_VAL_ASCII_SIZE+1);
	}

	/*
	 desc : 메모리 반환
	 parm : None
	 retn : None
	*/
	VOID Delete()
	{
		delete var_name;
		delete var_desc;
		delete cur_val_str;
		delete def_val_str;
	}

	/*
	 desc : 값 저장 (숫자값)
	 parm : type	- [in]  Data Value Type
			flag	- [in]  0x00:Current, 0x01:Default
			value	- [in]  Data Value
	 반환: None
	*/
	VOID SetValueNum(E30_GPVT type, UINT8 flag, PVOID value)
	{
		TCHAR tzBool[2][8]	{ L"FALSE", L"TRUE" };
		PTCHAR ptzVal		= NULL;
		LNG_CVVT pstVal		= NULL;

		/* 저장되는 값 종류에 따라 */
		if (!flag)
		{
			pstVal	= &current_val;
			ptzVal	= cur_val_str;
		}
		else
		{
			pstVal	= &default_val;
			ptzVal	= def_val_str;
		}

		/* 저장되는 값 형식에 따라 */
		switch (type)
		{
		case E30_GPVT::en_b		:
			if (*(PUINT16)value == USHORT_MAX)	pstVal->b_value	= TRUE;
			else								pstVal->b_value	= FALSE;
			swprintf_s(ptzVal, 32, L"%s", tzBool[pstVal->b_value]);
			break;
		case E30_GPVT::en_bi		:	pstVal->bi_value	= *(PUINT8)value;
			swprintf_s(ptzVal, 32, L"%u", pstVal->bi_value);
			break;
		case E30_GPVT::en_i1		:
		case E30_GPVT::en_i2		:
		case E30_GPVT::en_i4		:
		case E30_GPVT::en_i8		:
			switch (type)
			{
			case E30_GPVT::en_i1	:	pstVal->i8_value	= *(PINT8)value;	break;
			case E30_GPVT::en_i2	:	pstVal->i16_value	= *(PINT16)value;	break;
			case E30_GPVT::en_i4	:	pstVal->i32_value	= *(PINT32)value;	break;
			case E30_GPVT::en_i8	:	pstVal->i64_value	= *(PINT64)value;	break;
			}
			swprintf_s(ptzVal, 32, L"%I64d", *(PINT64)value);
			break;

		case E30_GPVT::en_u1		:
		case E30_GPVT::en_u2		:
		case E30_GPVT::en_u4		:
		case E30_GPVT::en_u8		:
			switch (type)
			{
			case E30_GPVT::en_u1	:	pstVal->u8_value	= *(PUINT8)value;	break;
			case E30_GPVT::en_u2	:	pstVal->u16_value	= *(PUINT16)value;	break;
			case E30_GPVT::en_u4	:	pstVal->u32_value	= *(PUINT32)value;	break;
			case E30_GPVT::en_u8	:	pstVal->u64_value	= *(PUINT64)value;	break;
			}
			swprintf_s(ptzVal, 32, L"%I64u", *(PUINT64)value);
			break;
		case E30_GPVT::en_f4		:
		case E30_GPVT::en_f8		:
			switch (type)
			{
			case E30_GPVT::en_f4	:	pstVal->f_value		= *(PFLOAT)value;	break;
			case E30_GPVT::en_f8	:	pstVal->d_value		= *(DOUBLE*)value;	break;
			}
			swprintf_s(ptzVal, 32, L"%.3f", *(DOUBLE*)value);
			break;
		}
	}

	/*
	 desc : 값 저장 (문자값)
	 parm : type	- [in]  Data Value Type
			flag	- [in]  0x00:Current, 0x01:Default
			value	- [in]  Data Value
	 반환: None
	*/
	VOID SetValueStr(UINT8 flag, PTCHAR value)
	{
		PTCHAR ptzVal	= NULL;

		/* 저장되는 값 종류에 따라 */
		if (!flag)	ptzVal	= cur_val_str;
		else		ptzVal	= def_val_str;
		
		wmemset(ptzVal, 0x00, CIM_VAL_ASCII_SIZE);
		wcscpy_s(ptzVal, CIM_VAL_ASCII_SIZE, value);
	}

}	STG_CEVI,	*LPG_CEVI;

/* 가장 최근의 Host로부터 요청 받은 Carrier의 Tag 정보 */
typedef struct __st_cim300_host_carrier_tag__
{
	LONG				data_len;		/* The Data Length parameter (Indicates the number of bytes of data to read or write) */
										/* 'data_seg' 버퍼 크기 ? 혹은 'data_seg' 버퍼에 저장된 문자열 길이 */
	LONG				transaction_id;	/* The transaction ID of (S3F29) */
	PTCHAR				loc_id;			/* Carrier ID가 놓여질 Location ID (with String) */
	PTCHAR				carr_id;		/* Carrier ID */
	PTCHAR				data_seg;		/* The DataSeg parameter (As a Protocol-specific, Indicates specific section of data to read or write) */

	/*
	 desc : 메모리 할당
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL Init()
	{
		data_len= CIM_CARRIER_ID_SIZE;
		transaction_id	= 0;
		loc_id = new TCHAR[CIM_CARRIER_LOC_SIZE+1]; // (PTCHAR)::Alloc(sizeof(TCHAR) * CIM_CARRIER_LOC_SIZE+sizeof(TCHAR));
		ASSERT(loc_id);
		wmemset(loc_id, 0x00, CIM_CARRIER_LOC_SIZE+1);

		carr_id = new TCHAR[CIM_CARRIER_ID_SIZE+1]; //(PTCHAR)::Alloc(sizeof(TCHAR) * CIM_CARRIER_ID_SIZE+sizeof(TCHAR));
		ASSERT(carr_id);
		wmemset(carr_id, 0x00, CIM_CARRIER_ID_SIZE+1);
		data_seg = new TCHAR[CIM_DATA_SEGMENT_SIZE+1];// (PTCHAR)::Alloc(sizeof(TCHAR) * CIM_DATA_SEGMENT_SIZE+sizeof(TCHAR));
		ASSERT(data_seg);

		wmemset(data_seg, 0x00, CIM_DATA_SEGMENT_SIZE+1);

		return TRUE;
	}

	/*
	 desc : 메모리 해제
	 parm : None
	 retn : None
	*/
	VOID Close()
	{
		if (loc_id)		delete loc_id;		loc_id	= NULL;
		if (carr_id)	delete carr_id;	carr_id	= NULL;
		if (data_seg)	delete data_seg;	data_seg= NULL;
		transaction_id	= 0;
		data_len		= 0;
	}

}	STG_CHCT,	*LPG_CHCT;

/* GEM Recipe File Data */
typedef struct __st_cim300_recipe_file_data__
{
	UINT32				conn_id;		/* 1 based or later */
	UINT32				u_reserved[7];

	PTCHAR				file;			/* Recipe File Name (Included Path) */

}	STG_CRFD,	LPG_CRFD;

/* Substrate Detail Information */
typedef struct __st_cim300_substrate_detail_information__
{
	UINT8				slot_no;							/* source Slot ID (원래는 UINT8이나, 구조체 메모리 크기 줄이기 위해 T.T) */
	UINT8				loc_state;							/* Substate Location state (E90_SSLS) (Currently not used) */
	UINT8				u8_reserved[6];
	INT32				proc_state;							/* Current Processing State (E90_SSPS) */
	INT32				trans_state;						/* Current Transport State (E90_SSTS) */
	INT32				mtl_status;							/* Substrate material status (ID Status; E90_SISM) */
	INT32				subst_id_status;					/* States in the new State Model (E90_SISM) */
	TCHAR				carr_id[CIM_CARRIER_ID_SIZE];		/* source Carrier ID */

	/*
	 desc : 새로운 상태 값 갱신
	 parm : machine	- [in]  갱신하려는 대상의 Machine Type 값
			state	- [in]  갱신 값
	 retn : None
	*/
	VOID UpdateState(E90_VSMC machine, INT32 state)
	{
		switch (machine)
		{
		case E90_VSMC::en_no_sm						:
		case E90_VSMC::en_batch_location_sm			:	;	break;
		case E90_VSMC::en_substrate_transport_sm	:	trans_state	= state;	break;
		case E90_VSMC::en_substrate_processing_sm	:	proc_state	= state;	break;
		case E90_VSMC::en_substrate_reader_sm		:	mtl_status	= state;	break;
		case E90_VSMC::en_substrate_location_sm		:	loc_state	= state;	break;
		}
	}

	/*
	 desc : 검색 대상의 상태 값인지 여부
	 parm : machine	- [in]  검색 대상의 Machine Type 값
			state	- [in]  검색 대상의 상태 값
	 retn : None
	*/
	BOOL IsFindState(E90_VSMC machine, INT32 state)
	{
		switch (machine)
		{
		case E90_VSMC::en_no_sm						:
		case E90_VSMC::en_batch_location_sm			:	;	break;
		case E90_VSMC::en_substrate_transport_sm	:	return (trans_state	== state);
		case E90_VSMC::en_substrate_processing_sm	:	return (proc_state	== state);
		case E90_VSMC::en_substrate_reader_sm		:	return (mtl_status	== state);
		case E90_VSMC::en_substrate_location_sm		:	return (loc_state	== state);
		}

		return FALSE;
	}

}	STG_CSDI,	*LPG_CSDI;

/* CIM300 : Substrate History for E90STS */
typedef struct __st_cim300_substrate_history_information__
{
	TCHAR				loc_id[CIM_SUBSTRATE_LOC_ID];	/* Location ID */
	TCHAR				time_in[CIM_TIME_SIZE];			/* Time In */
	TCHAR				time_out[CIM_TIME_SIZE];		/* Time Out */

}	STG_CSHI,	*LPG_CSHI;

/* Load Port 정보 */
typedef struct __st_cim300_load_port_information__
{
	UINT8				port_id;				/* Load Port Number. 1-based */
	UINT8				port_carr_associate;	/* Load port-carrier association status. (E87_LPAS) */
	UINT8				port_access_mode_state;	/* Load Port access mode. (E87_LPAM) */
	UINT8				port_reservation_state;	/* Load port reservation state. (E87_LPRS) */
	UINT8				port_transfer_state;	/* Load port transfer state. (E87_LPTS) */
	UINT8				carrier_id_state;		/* Carrier ID verification status. (E87_CIVS) */
	UINT8				slot_map_status;		/* Carrier slot status. (E87_SMVS) */
	UINT8				u8_reserved;			/* slot_verifiy_state; Slot map verification status. (E87_CSMS) */

	TCHAR				carr_id[CIM_CARRIER_ID_SIZE];	/* Load Port와 관련된 Carrier ID */

}	STG_CLPI,	*LPG_CLPI;

/* telcim.inf : Value.Changed.ID 정보 */
typedef struct __st_telcim_value_change_id__
{
	UINT32				conn_id;	/* CONNECTIONx or 0 (COMMON) */
	UINT32				var_id;
	TCHAR				var_name[CIM_CMD_NAME_SIZE];

}	STG_TVCI,	*LPG_TVCI;

/* telcim.inf : SECS-II.Message.ID 정보 */
typedef struct __st_telcim_secs_message_id__
{
	UINT8				conn_id;	/* CONNECTIONx or 0 (COMMON) */
	UINT8				s_id;		/* Stream ID*/
	UINT8				f_id;		/* Function ID */
	UINT8				u8_reserved[5];
	TCHAR				mesg_desc[CIM_CMD_MESG_SIZE];

}	STG_TSMI,	*LPG_TSMI;

/* telcim.inf : SECS-II.Message.List */
typedef struct __st_telcim_secs_message_value__
{
	UINT16				type;						/* Value 버퍼에 저장된 값의 형 (ENG_GPVT 즉, U1, U2, U4, ASCII (CHAR), Float, Double) */
	UINT16				level;						/* XML 작성 기준으로 TAB 이동 기준. 즉, 오른쪽으로 몇 번 이동 ? */
	UINT16				u8_reserved[2];
	LONG				handle;						/* 수신할 때는 CIMLib. Library 내에서만 사용됨 (Zero-based) */
													/* 송신할 때는 List의 Depth (Tab Size. 간격) 값 (Zero-based) */
	LONG				l_reserved;
	TCHAR				value[CIM_SUBSTRATE_ID];	/* 값을 무조건 String Buffer에 저장 */

}	STG_TSMV,	*LPG_TSMV;

typedef struct __st_telcim_secs_message_data__
{
	UINT8				conn_id;		/* CONNECTIONx or 0 (COMMON) */
	UINT8				u8_reserved[7];
	LONG				mesg_id;		/* SECS-II Message ID (16진수 기준. Stream ID / Function ID) */
	LONG				trans_id;		/* Transaction ID (송신 혹은 수신받은 데이터 간의 동기화 용도) */
	LONG				count;			/* Value Buffer 개수 */
	LONG				l_reserved;

	LPG_TSMV			value;			/* Value list */

}	STG_TSMD,	*LPG_TSMD;

#pragma pack (pop)	/* align 8 bytes */