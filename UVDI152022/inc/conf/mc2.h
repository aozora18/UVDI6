
/*
 desc : MC2 Config Information
*/

#pragma once

#include "global.h"
#include "enum.h"
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
#include "conf_podis.h"
#elif (CUSTOM_CODE_GEN2I == DELIVERY_PRODUCT_ID)
#include "conf_gen2i.h"
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                           상수 값                                             */
/* --------------------------------------------------------------------------------------------- */

#define MAX_MC2_DRIVE							8													/* MC2에 부여할 수 있는 최대 모터 드라이브 개수 */
#define MAX_MC2_DRIVE_NAME						16
#define MAX_PACKET_DATA_LENGTH					1280
#define MAX_AXIS_ERROR							0x0160												/* 이 이상 값이 나오면 에러가 아님 */
#define PKT_SIZE_READ_OBJECT					26													/* Read Object Value Index 송신 패킷 길이 */
#define MAX_TEMP_PKT_SEND						128													/* 임시로 이전에 송신한 패킷을 저장하는 최대 개수 */

/* --------------------------------------------------------------------------------------------- */
/*                                       전역(Global) 열거형                                     */
/* --------------------------------------------------------------------------------------------- */

/* MC Area / Expose Mode */
typedef enum class __en_mc2_trigger_area_expose__ : UINT8
{
	en_area							= 0x01,		/* Area mode (1 trigger = 0.1 um) (스테이지 움직이지 않음)*/
	en_expo							= 0x02,		/* Trigger mode (1 trigger = 5.4 or 10.8 um) (최소 값으로 이동하지만, 현재 위치가 Min 값 보다 작으면 이동하지 않음) */

}	ENG_MTAE;

/* 모션 드라이브 제어 방법 */
typedef enum class __en_mc2_motion_moving_method__ : UINT8
{
	en_none							= 0x00,

	en_move_jog						= 0x01,		/* 지속적으로 이동 */
	en_move_step					= 0x02,		/* 설정된 값 만큼 이동 */
	en_move_abs						= 0x03,		/* 절대 위치로 이동 */

}	ENG_MMMM;

/* 모션 드라이브 쪽에 에러가 발생했을 경우, 드라이브가 스스로 중지 여부 */
typedef enum class __en_mc2_motion_stop_type__ : UINT8
{
	en_stop_no						= 0x00,		/* Only homing operation */
	en_stop_yes						= 0x01,		/* In case other operation */

}	ENG_MMST;

/* PDO : Object 0x3801 - Commander Number */
typedef enum class __en_mc2_control_command_number__ : UINT8
{
	en_do_nothing					= 0x00,
	en_do_homing					= 0x01,		/* Parameter-0 : empty			Parameter-1 : Homing Off set or 0	Parameter-2 : empty					Parameter-3 : empty					*/
	en_set_position_limits			= 0x02,		/* Parameter-0 : empty			Parameter-1 : Limit-Low,			Parameter-2 : Limit-High			Parameter-3 : empty					*/
	en_set_velocity					= 0x03,		/* Parameter-0 : empty			Parameter-1 : Profile Velocity		Parameter-2 : empty					Parameter-3 : empty					*/
	en_set_acceleration				= 0x04,		/* Parameter-0 : empty			Parameter-1 : Profiel Acceleration	Parameter-2 : empty					Parameter-3 : empty					*/
	en_go_position					= 0x05,		/* Parameter-0 : empty			Parameter-1 : Target Position		Parameter-2 : 0						Parameter-3 : empty					*/
	en_cancel_command				= 0x06,		/* Parameter-0 : empty			Parameter-1 : empty					Parameter-2 : empty					Parameter-3 : empty					*/
	en_do_power_on					= 0x07,		/* Parameter-0 : empty			Parameter-1 : empty					Parameter-2 : empty					Parameter-3 : empty					*/
	en_do_power_off					= 0x08,		/* Parameter-0 : empty			Parameter-1 : empty					Parameter-2 : empty					Parameter-3 : empty					*/
	en_set_jerk						= 0x09,		/* Parameter-0 : empty			Parameter-1 : Profile Jerk			Parameter-2 : empty					Parameter-3 : empty					*/
	en_set_position_window			= 0x0a,		/* Parameter-0 : empty			Parameter-1 : Window Size			Parameter-2 : empty					Parameter-3 : empty					*/
	en_reserved1					= 0x0b,		/* Parameter-0 : empty			Parameter-1 : empty					Parameter-2 : empty					Parameter-3 : empty					*/
	en_reserved2					= 0x0c,		/* Parameter-0 : empty			Parameter-1 : empty					Parameter-2 : empty					Parameter-3 : empty					*/
	en_reserved3					= 0x0d,		/* Parameter-0 : empty			Parameter-1 : empty					Parameter-2 : empty					Parameter-3 : empty					*/
	en_set_puls_output				= 0x0e,		/* Parameter-0 : empty			Parameter-1 : 0=Off / 5=On			Parameter-2 : empty					Parameter-3 : empty					*/
	en_reserved4					= 0x0f,		/* Parameter-0 : empty			Parameter-1 : empty					Parameter-2 : empty					Parameter-3 : empty					*/
	en_do_arc						= 0x10,		/* Parameter-0 : slave_drive	Parameter-1 : Radius				Parameter-2 : Start angle			Parameter-3 : End angle				*/
	en_do_vector					= 0x11,		/* Parameter-0 : slave_drive	Parameter-1 : Target Position		Parameter-2 : Target Position Slave	Parameter-3 : Profile Velocity or 0 */
	en_reserved5					= 0x12,		/* Parameter-0 : empty			Parameter-1 : empty					Parameter-2 : empty					Parameter-3 : empty					*/
	en_set_gear_mode				= 0x13,		/* Parameter-0 : master_drive	Parameter-1 : Off=0, On=1			Parameter-2 : Cycle Length			Parameter-3 : Master Cycle Length	*/
	en_restart_drive				= 0x14,		/* Parameter-0 : empty			Parameter-1 : empty					Parameter-2 : empty					Parameter-3 : empty					*/
	en_set_cal_off_set				= 0x15,		/* Parameter-0 : empty			Parameter-1 : Cal Off Set			Parameter-2 : empty					Parameter-3 : empty					*/
	en_go_velocity					= 0x16,		/* Parameter-0 : empty			Parameter-1 : Target Velocity		Parameter-2 : empty					Parameter-3 : empty					*/
	en_set_trig_mode_v1				= 0x17,		/* Parameter-0 : 0 or 2			Parameter-1 : 0 or 1				Parameter-2 : empty					Parameter-3 : empty					*/
	en_fault_reset					= 0x18,		/* Parameter-0 : empty			Parameter-1 : empty					Parameter-2 : empty					Parameter-3 : empty					*/
	en_messure_cutting_edge			= 0x19,		/* Parameter-0 : empty			Parameter-1 : empty					Parameter-2 : empty					Parameter-3 : empty					*/

	en_set_trig_mode_v2				= 0x21,		/* Parameter-0 : 0 or 2			Parameter-1 : 0 or 1				Parameter-2 : empty					Parameter-3 : empty					*/

}	ENG_MCCN;

/* PDO & SDO Object Index */
typedef enum class __en_pdo_sdo_object_index__ : UINT32
{
	en_pg							= 0x00000d70,	/* Object 0x0d70, PG */

	en_pdo_ref_head_mcontrol		= 0x00001310,	/* POD Reference Header - MControl (0x1310) */
	en_pdo_ref_head_v_scale			= 0x00001311,	/* POD Reference Header - V_SCALE (0x1311) */
	en_pdo_ref_head_d_outputs		= 0x00001314,	/* POD Reference Header - DIGITAL_OUTPUTS (0x1314) */

	en_ref_value_collective			= 0x00003801,	/* Object 0x3801, PDO Reference Value Collective Object (Read Only) */
	en_act_value_collective			= 0x00003802,	/* Object 0x3802, PDO Actual Value Collective Object (Read & Write) */
	en_sdo_ref_header				= 0x00003807,	/* Object 0x3807, SDO Reference Header */
	en_sdo_ref_drive				= 0x00003808,	/* Object 0x3808, SDO Reference Drive */
	en_sdo_act_header				= 0x00003809,	/* Object 0x3809, SDO Actual Header */
	en_sdo_act_drive				= 0x0000380a,	/* Object 0x380a, SDO Actual Drive */
	en_pdo_spt_command				= 0x0000381d,	/* */

}	ENG_PSOI;

/* MC2 Drive Error Kind */
typedef enum class __en_mc2_drive_error_kind__ : UINT8
{
	en_err_objects					= 0x01,
	en_err_command					= 0x02,
	en_err_drive					= 0x03,
	en_err_sd2_faults				= 0x04,
	en_err_nrun						= 0x05,
	en_err_windows					= 0x06,
	en_err_sd2_err					= 0x07,
	en_err_sd2_quick_stop_code		= 0x08,
	en_err_sd2_warning_code			= 0x09,

}	ENG_MDEK;

/* Listing of all UDP telegrams */
typedef enum class __en_mc2_udp_msg_type__ : UINT8
{
	en_cmd_nack						= 0x07,		// The command nack telegram: The command buffer is full.
	en_gen3_dnc						= 0x0a,		// Generation 3 DNC command telegram or response telegram

}	ENG_MUMT;

/* DNC Commands for Generation 3 */
typedef enum class __en_dnc_commands_generation_3__
{
	en_req_read_obj					= 0x03,			// Reads the value by means of the object number.
	en_req_write_obj				= 0x04,			// Writes the value by means of the object number.
	en_req_write_read_obj			= 0x07,			// Simultaneous writing and reading
	en_res_read_obj					= 0x80 | 0x03,	// read response object value per index
 	en_res_write_obj				= 0x80 | 0x04,	// write response object value per index
	en_res_write_read_obj			= 0x87,			// Simultaneous writing and reading
	en_res_error					= 0xff,			// General error response

}	ENG_DCG3;

/* Possible errors during operation */
typedef enum class __en_mc2_gen3_error_code__ : UINT8
{
	en_none							= 0x00,		// No error
	en_server_not_impl				= 0x08,		// Function not implemented
												// The desired function is not implemented
	en_server_obj_not_found			= 0x0a,		// Object not found
												// The object with this index was not found in the object directory
	/* The errors en_server_shm_access_err and en_server_shm_block_err are signs of an internal problem in the SD3 device */
	en_server_shm_access_err		= 0x0b,		// Error in shared memory area
												// Error when trying to access the shared memory area
	en_server_shm_block_err			= 0x0c,		// Error in shared memory block
												// Error when trying to access the shared memory block
	en_server_serial_error			= 0x0d,		// Wrong serial number
												// The serial number included in the command is wrong.
	en_obj_subid					= 0x0e,		// Wrong Sub-ID
												// The requested Sub-ID does not exist for this object.
	/* If the error en_odict_not_valid occurs, the SD3 device is not yet ready for operation. */
	en_odict_not_valid				= 0x0f,		// Object directory is not valid
												// The object directory is not (yet) valid.

}	ENG_MGEC;

/* SD2S Drive에서 Actual Data의 상태 값 플래그 */
typedef enum class __en_sd2s_actual_data_status__ : UINT8
{
	en_flag_done					= 0x00,		/* command_done */
	en_flag_cal						= 0x01,		/* drive_calibrated */
	en_flag_on						= 0x02,		/* power_stage_on */
	en_flag_err						= 0x03,		/* error_code_available */
	en_flag_msg						= 0x04,		/* message_code_available */
	en_flag_busy					= 0x05,		/* command_busy */
	en_flag_zero					= 0x06,		/* velocity_zero */
	en_flag_inpos					= 0x07,		/* target_position_reached */
	en_flag_varc					= 0x08,		/* target_velocity_reached */
	en_flag_init					= 0x09,		/* drive_ready */
	en_flag_done_toggle				= 0x0a,		/* 항상 반전되는 값 */

}	ENG_SADS;

/* --------------------------------------------------------------------------------------------- */
/*                                       전역(Global) 구조체                                     */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 1)

/* MC2 UDP Packet Sender Header (Little-endian byte ordering) */
typedef struct __st_packet_udp_common_header__
{
	UINT16				return_port;				/* target port of the UDP response telegram */
	UINT8				udp_msg_type;				/* Describes the type of the UDP telegram. (ENG_MUMT) */
	UINT8				short_cycle;				/* 0 = long cycle, 1 = response of a short cycle */
	UINT16				u16_reserved;				/* Reserved, set to 0 */
	UINT32				send_cmd_cnt;				/* Continuous command counter of the client (명령을 송신할 때마다 증가시킴) */
													/* UDP 통신이기 때문에, 송신한 명령들이 여러개일 경우, 응답 받을 때, */
													/* 이전에 송신한 개수와 일치하기 위함 즉, 명령어 개수라기 보다 명령어 구분 식별 ID */
	VOID NextCmdCount()
	{
		if (++send_cmd_cnt > (MAX_TEMP_PKT_SEND-1))	send_cmd_cnt = 0;
	}

}	STG_PUCH,	*LPG_PUCH;

/* Command telegram for reading object */
typedef struct __st_packet_request_read_data__
{
	UINT8				e_gen3_dnc_cmd;				/* Fixed 0x03. In other words, Constant E_G3_RadObjectValueByIndex */
	/* These two parameters are the target address of the DNC command. */
	UINT8				sub_id;						/* SubID of the SD3 device (SubId selects the dimension within the object directory) */
													/* sub_id = drive no. (single 0 ~ n. 나머지는 무조건 0) */
	UINT32				serial_no;					/* Serial number of the device or 0 */
	UINT16				size_wanted;				/* Number of bytes to be read 0 = all available */
	UINT32				sub_index;					/* Reading shall be started at this SubIndex (Offset) */
													/* Protocol Document 내의 Offset가 동일 */
	UINT32				index;						/* Index of the object to be read */
													/* Object Number (Protocol Document) */

}	STG_PQRD,	*LPG_PQRD;

/* Command telegram for writing object */
typedef struct __st_packet_request_write_data__
{
	UINT8				e_gen3_dnc_cmd;				/* Fixed 0x04. In other words, Constant E_G3_WriteObjectValueByIndex */
	/* These two parameters are the target address of the DNC command. */
	UINT8				sub_id;						/* SubID of the SD3 device (SubId selects the dimension within the object directory) */
	UINT32				serial_no;					/* Serial number of the device or 0 (SerialNo identifies the device) */
													/* If check-up of the serial number is not desired, enter the value 0 for SerialNo */

	UINT32				index;						/* Index of the object to be written */
	UINT32				sub_index;					/* Writing shall be started at this SubIndex (Offset) */
													/* Beginning from this offset the data are written into the object. */
													/* Data are only written to the end of the object. */
													/* If the value of SubIndex is greater than the length of the object, no data will be written and an error will be generated. */
	UINT16				data_len;					/* Number of bytes that are read and recorded in Data */
													/* Number of data to be written with this command */
	PUINT8				data;						/* Data to be written (MAX_PACKET_DATA_LENGTH) */

}	STG_PQWD,	*LPG_PQWD;

/* Reference - Header (32 bytes) */
typedef struct __st_reference_value_collective_header__
{
	/* MControl */
	UINT8			mc_reserved:6;		/* reserved */
	UINT8			mc_en_stop:1;		/* All drives stop after an error has occurred (not yet implemented) */
	UINT8			mc_heart_beat:1;	/* Heart beat toggle bit */
	/* V_SCALE */
	UINT8			v_scale;			/* This parameter allows percentage modification of the speed of all drives (1 ~ 100 %) */
	/* Reserved */
	UINT8			u8_reserved1[18];
	/* OUTPUTS */
	UINT64			digital_outputs;	/* The parameter OUTPUTS allows influencing the possibly existing digital outputs at the MC2 control. The field has a width of 64 bits. */
	/* Reserved */
	UINT8			u8_reserved2[4];

}	STG_RVCH,	*LPG_RVCH;

/* Reference - Drive Data (16 bytes) */
typedef struct __st_reference_value_collective_drive__
{
	/* Control */
	UINT16			ctrl_cmd:8;			/* Command number (ENG_MCCN) */
	UINT16			ctrl_reserved:7;
	UINT16			ctrl_cmd_toggle:1;	/* Command Toggle Bit */

	UINT8			param_0;			/* */
	UINT8			u8_reserved1;
	/* ctrl_cmd 변수의 값에 따라 다른 값이 저장됨 (ENG_MCCN 참조) */
	INT32			param_1;			/* (ENG_MCCN 참조) */
	INT32			param_2;			/* (ENG_MCCN 참조) */
	INT32			param_3;			/* (ENG_MCCN 참조) */

}	STG_RVCD,	*LPG_RVCD;

/* Actual - Header (32 bytes) */
typedef struct __st_actual_value_collective_header__
{
	/* MState */
	UINT8			ms_reserved:6;		/* reserved */
	UINT8			ms_heart_beat1:1;	/*  */
	UINT8			ms_heart_beat2:1;	/* Heart beat toggle bit */

	UINT8			u8_reserved1;

	/* MError */
	UINT16			merror;				/*  */
	UINT8			u8_reserved2[4];
	/* Inputs & Outputs */
	UINT64			inputs;
	UINT8			u8_reserved3[4];
	UINT64			outputs;			/*  */
	UINT8			u8_reserved4[4];

}	STG_AVCH,	*LPG_AVCH;

/* Actual - Drive Data (16 bytes) */
typedef struct __st_actual_value_collective_drive__
{
	/* State */
	UINT16			flag_done:1;		/* command_done */
	UINT16			flag_cal:1;			/* drive_calibrated */
	UINT16			flag_on:1;			/* power_stage_on */
	UINT16			flag_err:1;			/* error_code_available */
	UINT16			flag_msg:1;			/* message_code_available */
	UINT16			flag_busy:1;		/* command_busy */
	UINT16			flag_zero:1;		/* velocity_zero */
	UINT16			flag_inpos:1;		/* target_position_reached */
	UINT16			flag_varc:1;		/* target_velocity_reached */
	UINT16			flag_init:1;		/* drive_ready */
	UINT16			flag_reserved:5;	/* u16_reserved */
	UINT16			flag_done_toggle:1;	/* 항상 반전되는 값 */

	UINT16			error;				/* Error Code */
	INT32			real_position;		/* Actual Position (unit: 100 nm or 0.1 um) */
	INT32			result1;
	INT32			result2;

	/*
	 desc : 현재 드라이버 (Axis)가 동작 (이동) 중인지 여부
	 parm : None
	 retn : TRUE (중지 중임) or FALSE (움직이는 중임)
	*/
	BOOL IsStopped()
	{
#if 1
#if 0
		return ((1 == flag_zero) && (0 == flag_busy));
#else
		return (0 == flag_busy);
#endif
#else
		return ((1 == flag_zero) && (0 == flag_busy) && (1 == flag_inpos));
#endif
	}

	/*
	 desc : 원하는 위치에 도달했는지 여부
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsReached()
	{
		return 1 == flag_inpos;
	}

	/*
	 desc : 현재 Servo On / Off인지 여부 즉, Calibrated 되었는지 여부
	 parm : None
	 retn : TRUE - Servo Off, FALSE - Servo On
	*/
	BOOL IsServoOn()
	{
		return (1 == flag_on);
	}
	BOOL IsServoOff()
	{
		return (0 == flag_on);
	}

	/*
	 desc : 현재 Servo On인지 여부 즉, Calibrated 되었는지 여부
	 parm : None
	 retn : TRUE - Servo Off, FALSE - Servo On
	*/
	BOOL IsCalibrated()
	{
		return (1 == flag_cal);
	}

	/*
	 desc : 현재 Device Error 인지 여부
	 parm : None
	 retn : TRUE - Servo Off, FALSE - Servo On
	*/
	BOOL IsServoError()
	{
		return ((0 == flag_on) &&
				((1 == flag_err) &&
				 (error != 0 && error <= MAX_AXIS_ERROR) ||
				 (error != 0 && error == 0x1100)));	/* The 0x1100 is hardware error */
	}

	/*
	 desc : 현재 동작 중인지 여부
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsBusy()
	{
		return flag_busy ? TRUE : FALSE;
	}

	/*
	 desc : 현재 명령어가 끝났는지 여부
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsCmdDone()
	{
		return flag_done ? TRUE : FALSE;
	}

}	STG_AVCD,	*LPG_AVCD;

/* MC2로부터 수신된 데이터 정보 */
typedef struct __st_mc2_packet_recv_data__
{
	UINT8				msg_type;				/* Message Type (E_CMD_NACK:7 or E_GEN3_DNC:10) */
	UINT8				dnc_cmd;				/* ENG_DCG3 즉, Read Request or Write Request */
	UINT8				dnc_err;				/* DNC Command Error (수신된 패킷의 에러 정보) (ENG_MGEC) */
	UINT8				sub_id;					/* SubID of the SD3 device */
	UINT8				undefined;				/* 정상일 때는 0x00, 정의(구현)하지 않은 명령어가 수신된 경우 양수 값 (0x01) */
	UINT8				data_len;				/* data 버퍼에 저장된 길이 (크기) (0일 경우, data = NULL 임) */
	UINT8				object_index;			/* ENG_PSOI (PDO & SDO Object Index) */
	UINT8				u8_reserved[1];

	UINT32				obj_index;				/* Object ID 즉, PDO or SDO */
	UINT32				sub_index;				/* Object의 Offset (송신 쪽의 Sub Index이지, 수신쪽의 Sub Index는 아님) */

	UINT8				data[168];				/* 수신된 데이터가 저장된 버퍼, 추후 이 메모리는 수동으로 해제 */

}	STG_MPRD,	*LPG_MPRD;

/* MC2에 요청(Read or Write)한 정보 (응답 패킷과 비교하기 위함) */
typedef struct __st_mc2_packet_send_data__
{
	UINT8				cmd;					/* E_G3_ReadObjectValueByIndex or E+G3_WriteObjectValueByIndex */
	/* These two parameters are the target address of the DNC command. */
	UINT8				sub_id;					/* SubID of the SD3 device (SubId selects the dimension within the object directory) */
												/* sub_id = drive no. (single 0 ~ n. 나머지는 무조건 0) */
	UINT8				data_len;				/* data 버퍼에 저장된 길이 (크기) */
	UINT8				object_index;			/* ENG_PSOI (PDO & SDO Object Index) */
	UINT8				u8_reserved[4];

	UINT32				sub_index;				/* Reading shall be started at this SubIndex (Offset) */
	UINT32				obj_index;				/* Object Number (Protocol Document) */
	UINT8				data[168];

}	STG_MPSD,	*LPG_MPSD;

/* MC2 전체 환경 파일 */
typedef struct __st_mc2_data_shared_memory__
{
	/* Reference Objects */
	STG_RVCH			ref_head;
	STG_RVCD			ref_data[MAX_MC2_DRIVE];
	/* Actual Objects */
	STG_AVCH			act_head;
	STG_AVCD			act_data[MAX_MC2_DRIVE];
	/* Last Recv Packet */
	STG_MPRD			last_recv;

	/* 연결 상태 */
	STG_DLSM			link;

	/*
	 desc : Homing 수행해도 되는지 안되는지 여부
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsRunHoming()
	{
		return ref_head.mc_en_stop == 0 ? TRUE : FALSE;
	}

	/*
	 desc : DNC Error 여부
	 parm : None
	 retn : 0x00 (No Error), 0x01 or Later (Error)
	*/
	UINT8 GetDNCError()
	{
		return last_recv.dnc_err;
	}

	/* ----------------------------------------------------------------------------------------- */
	/*                  Object 0x3801, PDO Reference Value Collective Object                     */
	/* ----------------------------------------------------------------------------------------- */

	/*
	 desc : PDO Reference Value 설정
	 parm : data	- [in]  수신된 데이터가 저장된 버퍼 포인터
			size	- [in]  'data' 버퍼에 저장된 크기
	 retn : None
	*/
	VOID SetDataRefVal(PUINT8 data, UINT32 size)
	{
		if (size == (sizeof(STG_RVCH) + sizeof(STG_RVCD) * MAX_MC2_DRIVE))
		{
			/* Header */
			memcpy(&ref_head, data, sizeof(STG_RVCH));
			/* Body : Drive 8 ea */
			memcpy(&ref_data, data+sizeof(STG_RVCH), sizeof(STG_RVCD) * MAX_MC2_DRIVE);
		}
	}

	/*
	 desc : 헤더 정보 반환
	 parm : None
	 retn : 헤더 정보가 저장된 구조체 포인터
	*/
	LPG_RVCH GetHeadRef()
	{
		return &ref_head;
	}

	/*
	 desc : 드라이브 정보 반환
	 parm : drv_id	- [in]  SD2 드라이브러 번호 (0x00 ~ 0x07)
	 retn : 드라이브 정보가 저장된 구조체 포인터
	*/
	LPG_RVCD GetDriveRef(UINT8 drv_id)
	{
		return &ref_data[drv_id];
	}

	/*
	 desc : 현재 명령 상태 토글 값 반환
	 parm : drv_id	- [in]  SD2 드라이브러 번호 (0x00 ~ 0x07)
	 retn : 토글 값 반환
	*/
	UINT8 GetCmdToggled(UINT8 drv_id)
	{
		return ref_data[drv_id].ctrl_cmd_toggle;
	}

	/* ----------------------------------------------------------------------------------------- */
	/*                    Object 0x3802, PDO Active Value Collective Object                      */
	/* ----------------------------------------------------------------------------------------- */

	/*
	 desc : 헤더 정보 반환
	 parm : None
	 retn : 헤더 정보가 저장된 구조체 포인터
	*/
	LPG_AVCH GetHeadAct()
	{
		return &act_head;
	}

	/*
	 desc : 드라이브 정보 반환
	 parm : drv_id	- [in]  SD2 드라이브러 번호 (0x00 ~ 0x07)
	 retn : 드라이브 정보가 저장된 구조체 포인터
	*/
	LPG_AVCD GetDriveAct(UINT8 drv_id)
	{
		return &act_data[drv_id];
	}

	/*
	 desc : PDO Actual Value 설정
	 parm : data	- [in]  수신된 데이터가 저장된 버퍼 포인터
			size	- [in]  'data' 버퍼에 저장된 크기
	 retn : None
	*/
	VOID SetDataActVal(PUINT8 data, UINT32 size)
	{
		if (size == (sizeof(STG_AVCH) + sizeof(STG_AVCD) * MAX_MC2_DRIVE))
		{
			/* Header */
			memcpy(&act_head, data, sizeof(STG_AVCH));
			/* Body : Drive 8 ea */
			memcpy(&act_data, data+sizeof(STG_AVCH), sizeof(STG_AVCD) * MAX_MC2_DRIVE);
		}
	}

	/*
	 desc : 특정 드라이브의 에러 여부
	 parm : drv_id	- [in]  에러 유무 확인하고자 하는 Driver ID
	 retn : TRUE or FALSE
	*/
	BOOL IsDriveError(UINT8 drv_id)
	{
#if 0	/* 아래 MERROR는 사용되지 않음 */
		/* 기본 Header 에러 정보 유무 */
		if (act_head.merror > 0)	return TRUE;
#endif
		return (act_data[drv_id].flag_err && (act_data[drv_id].error > 0));
	}

	/*
	 desc : 특정 드라이브가 Busy 인지 여부
	 parm : drv_id	- [in]  에러 유무 확인하고자 하는 Driver ID
	 retn : TRUE or FALSE
	*/
	BOOL IsDriveBusy(UINT8 drv_id)
	{
		return act_data[drv_id].IsBusy();
	}
	BOOL IsDriveCmdDone(UINT8 drv_id)
	{
		return act_data[drv_id].IsCmdDone();
	}
	BOOL IsDriveReached(UINT8 drv_id)
	{
		return act_data[drv_id].IsReached();
	}

	/*
	 desc : 임의 Drive 중 한 개 이상에서 에러가 발생 했는지 여부
	 parm : count	- [in]  검색할 개수
			drv_id	- [in]  Drive ID가 저장된 배열 포인터
	 retn : TRUE or FALSE
	*/
	BOOL IsAnyDriveError(UINT8 count, UINT8 *drv_id)
	{
		UINT8 i = 0x00;

		for (; i<count; i++)
		{
			if (IsDriveError(drv_id[i]))	return TRUE;
		}
		return FALSE;
	}

	/*
	 desc : 모션 드라이브들 중 처음에 조회된 드라이브의 Error Code 값 반환
	 parm : drv_id	- [in]  에러 유무 확인하고자 하는 Driver ID가 저장된 배열 포인터
			count	- [in]  'drv_id'에 저장된 Drive ID 개수
	 retn : 에러 코드 값. 0 값이면 에러 없음
	*/
	UINT16 GetErrorCode(UINT8 *drv_id, UINT8 count)
	{
		UINT8 i	= 0x00;
		for (; i<count; i++)
		{
			if (0 != act_data[drv_id[i]].flag_err)
			{
				return act_data[drv_id[i]].error;
			}
		}

		return 0x0000;
	}
	UINT16 GetErrorCodeOne(UINT8 drv_id)
	{
		if (0 == act_data[drv_id].flag_err)	return 0x0000;
		return act_data[drv_id].error;
	}

	/*
	 desc : 모든 드라이브의 에러 값을 초기화
	 parm : None
	 retn : None
	*/
	VOID ResetDriveAll()
	{
		UINT8 i	= 0x00;
		act_head.merror	= 0x0000;
		for (i=0; i<MAX_MC2_DRIVE; i++)	act_data[i].error	= 0x0000;
	}

	/*
	 desc : 임의 드라이버 중 한 개라도 Servo가 On되어 있는지 여부 즉, Calibrated 되었는지 여부
	 parm : drv_id	- [in]  에러 유무 확인하고자 하는 Driver ID가 저장된 배열 포인터
			count	- [in]  'drv_id'에 저장된 Drive ID 개수
	 retn : TRUE or FALSE
	*/
	BOOL IsServoOn(UINT8 *drv_id, UINT8 count)
	{
		UINT8 i = 0x00;

		for (; i<count; i++)
		{
			if (act_data[drv_id[i]].IsServoOn())	return TRUE;
		}

		return FALSE;
	}

	/*
	 desc : 임의 서보 드라이브가 Off 되어 있는지 여부
	 parm : drv_id	- [in]  SD2 Driver ID 즉, 0x00, 0x01, 0x04, 0x05, ..., 0x07
	 retn : TRUE (Servo Off) or FALSE (Servo On)
	*/
	BOOL IsServoOff(UINT8 drv_id)
	{
		if (!act_data[drv_id].IsServoOn())	return TRUE;

		return FALSE;
	}

	/*
	 desc : 현재 드라이브의 Done Toggled 값 반환
	 parm : drv_id	- [in]  Drive ID (0x00 ~ 0x07)
	 retn : 현재 Done Toggled 값
	*/
	UINT8 GetDoneToggled(UINT8 drv_id)
	{
		return UINT8(act_data[drv_id].flag_done_toggle);
	}

	/*
	 desc : 현재 드라이브의 위치 정보 반환 (단위: mm)
	 parm : drv_id - [in]  Drive ID (ENG_MMDI)
	 retn : 드라이브의 위치 (unit: mm)
	*/
	DOUBLE GetDrivePos(UINT8 drv_id)
	{
		return act_data[drv_id].real_position / 10000.0f;
	}

}	STG_MDSM,	*LPG_MDSM;


#pragma pack (pop)	/* 1 bytes */
