
/* --------------------------------------------------------------------------------------------- */
/* desc : Serial Extern Header File                                                              */
/* --------------------------------------------------------------------------------------------- */

#pragma once

// Intel 경고 무시하기
#if 0
#pragma warning(disable : 1885)	// #pragma region unclosed at end of file
#else
#pragma warning(disable : 4996)	// Winsock2
#endif


/* --------------------------------------------------------------------------------------------- */
/*                                       전역(Global) 열거형                                     */
/* --------------------------------------------------------------------------------------------- */

// Serial Callback Return Type
typedef enum class __en_cbf_return_data_type__ : UINT8
{
	en_crdt_unknown						= 0x00,
	en_crdt_opened						= 0x01,		/* Port opened */
	en_crdt_closed						= 0x02,		/* Port closed */
	en_crdt_recieved					= 0x03,		/* Data received */

}	ENG_CRDT;

// 시리얼 통신 상태
typedef enum class __en_serial_comm_status_code__ : UINT8 
{
	en_scsc_closed						= 0x00,		/* Close the port */
	en_scsc_opened						= 0x01,		/* Open the port */
	en_scsc_ready						= 0x02,		/* Wait for the ready or the write to port */
	en_scsc_writing						= 0x10,
	en_scsc_writed						= 0x11,
	en_scsc_write_fail					= 0x12,
	en_scsc_reading						= 0x20,
	en_scsc_readed						= 0x21,
	en_scsc_read_fail					= 0x22,

}	ENG_SCSC;

// Serial Baud Rate Value
typedef enum class __en_serial_baud_rate_value__ : UINT32
{
	en_sbrv_baud_rate_110				= CBR_110,
	en_sbrv_baud_rate_300				= CBR_300,
	en_sbrv_baud_rate_600				= CBR_600,
	en_sbrv_baud_rate_1200				= CBR_1200,
	en_sbrv_baud_rate_2400				= CBR_2400,
	en_sbrv_baud_rate_4800				= CBR_4800,
	en_sbrv_baud_rate_9600				= CBR_9600,
	en_sbrv_baud_rate_14400				= CBR_14400,
	en_sbrv_baud_rate_19200				= CBR_19200,
	en_sbrv_baud_rate_38400				= CBR_38400,
	en_sbrv_baud_rate_56000				= CBR_56000,
	en_sbrv_baud_rate_57600				= CBR_57600,
	en_sbrv_baud_rate_115200			= CBR_115200,
	en_sbrv_baud_rate_128000			= CBR_128000,
	en_sbrv_baud_rate_256000			= CBR_256000,

}	ENG_SBRV;

/* Serial Handshake Operation Type */
typedef enum class __en_serial_handshake_operation_type__ : UINT8
{
	en_shot_handshake_off				= 0x00,
	en_shot_handshake_hw				= 0x01,		// Hardware
	en_shot_handshake_sw				= 0x02,		// Software
	en_shot_handshake_jted				= 0x03,		// Handshake JusTek 보드용
	en_shot_img_opt_lens				= 0x04,		// 결상 광학계 용
	en_shot_strobe_camera				= 0x05,		// Strobe Camera
	en_shot_led_can						= 0x06,		// LED Can Converter

}	ENG_SHOT;

/* Read time out mode */
typedef enum class __en_comm_read_timeout_block__ : UINT8
{
	en_crtb_read_timeout_blocking		= 0x00,
	en_crtb_read_timeout_nonblocking	= 0x01,

}	ENG_CRTB;

/* --------------------------------------------------------------------------------------------- */
/*                                       전역(Global) 공용체                                     */
/* --------------------------------------------------------------------------------------------- */

#pragma pack(push, 8)

/* Setup Serial Configuration Information (Not DCB) */
typedef struct __st_setup_serial_config_info__
{
	// 통신 제어 정보
	UINT8				byte_size;				// Equal to DCB.ByteSize
	UINT8				stop_bits;				// Equal to DCB.StopBits
	UINT8				is_binary;				// Equal to DCB.fBinary
	UINT8				is_parity;				// Equal to DCB.fParity
	UINT8				parity;					// Equal to DCB.Parity
	UINT8				u8_reserved[3];
	// 송/수신 버퍼 크기
	UINT32				port_buff;				// COM Port : Size of Read/Write. Buffer
	// 송/수신 대기 시간 (단위: msec)
	UINT32				read_Interval_time;		// COMTIMEOUTS 구조체 참조
	UINT32				read_total_multiple;	// COMTIMEOUTS 구조체 참조
	UINT32				read_const_time;		// COMTIMEOUTS 구조체 참조
	UINT32				write_total_multiple;	// COMTIMEOUTS 구조체 참조
	UINT32				write_const_time;		// COMTIMEOUTS 구조체 참조

}	STG_SSCI,	*LPG_SSCI;

#pragma pack (pop)	// 8 Bytes Boundary Sort!!!
