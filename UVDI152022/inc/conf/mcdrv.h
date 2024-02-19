
/* --------------------------------------------------------------------------------------------- */
/* desc : MC Protocol 관련 헤더                                                                  */
/* --------------------------------------------------------------------------------------------- */

#pragma once


#define	MC_3E_REQ_SIG				0x50
#define	MC_3E_RSP_SIG				0xD0
#define MC_BASE_NET_NO				0x00
#define MC_BASE_PLC_NO				0xff
#define MC_BASE_STA_IO				0x03ff
#define MC_BASE_STA_NO				0x00
#define MC_BASE_MON_TIMER			0x0010
#define MC_PKT_BASE_LEN				0x09
#define MC_MAX_WORD_LEN				0x01e0	// 480을 넘지 말아야 한다 (?)

/* 한번에 읽을 수 있는 최대 크기 (단위: Bytes) - 1024 Bytes 이상 읽기 요청하면 PLC 먹통! */
#define MC_READ_BLOCK_SIZE			512

/* PLC 내 장비 정보가 저장될 워드 최대 개수 */
#define MAX_ADDR_WORD_COUNT			128

/* --------------------------------------------------------------------------------------------- */
/*                                       전역(Global) 열거형                                     */
/* --------------------------------------------------------------------------------------------- */

// MELSEC Memory 영역 구분자 정보 즉, Device Code Name 정보
typedef enum class __en_device_name_to_code__ : UINT8
{
	en_sm			= 0x91,		// 'SM'
	en_sd			= 0xA9,		// 'SD'
	en_x			= 0x9C,		// 'X'
	en_y			= 0x9D,		// 'Y'
	en_m			= 0x90,		// 'M'
	en_l			= 0x92,		// 'L'
	en_f			= 0x93,		// 'F'
	en_v			= 0x94,		// 'V'
	en_b			= 0xA0,		// 'B'
	en_d			= 0xA8,		// 'D'
	en_w			= 0xB4,		// 'W'
	en_ts			= 0xC1,		// 'TS'
	en_tc			= 0xC0,		// 'TC'
	en_ss			= 0xC7,		// 'SS'
	en_sc			= 0xC6,		// 'SC'
	en_sn			= 0xC8,		// 'SN'
	en_cs			= 0xC4,		// 'CS'
	en_cc			= 0xC3,		// 'CC'
	en_cn			= 0xC5,		// 'CN'
	en_sb			= 0xA1,		// 'SB'
	en_sw			= 0xB5,		// 'SW'
	en_s			= 0x98,		// 'S'
	en_dx			= 0xA2,		// 'DX'
	en_dy			= 0xA3,		// 'DY'
	en_z			= 0xCC,		// 'Z'
	en_r			= 0xAF,		// 'R'
	en_zr			= 0xB0,		// 'ZR'

}	ENG_DNTC;

// PLC 쪽에 MC Protocol로 데이터 요청하는 방법
typedef enum class __en_tcp_socket_req_method__ : UINT8
{
	en_req_none		= 0x00,		// 아무것도 없는 상태?
	en_req_period	= 0x01,		// 주기적으로 요청하는 방법
	en_req_operate	= 0x02,		// 사용자 (운영자) 요구 방법

}	ENG_TSRM;

// Melsec Memory의 일괄 읽기 / 쓰기
typedef enum class __en_memory_batch_read_write_ : UINT16
{
	en_read			= 0x0401,	// 일괄 읽기
	en_write		= 0x1401,	// 일괄 쓰기

}	ENG_MBRW;

// Melsec Memory의 비트 or 워드 단위 동작
typedef enum class __en_memory_read_write_mode__ : UINT16
{
	en_bits			= 0x0001,	// 비트 단위
	en_word			= 0x0000,	// 워드 단위

}	ENG_MRWM;


/* --------------------------------------------------------------------------------------------- */
/*                                       전역(Global) 구조체                                     */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 1)

///*
// subheader			2 bytes ＼
// network_no			1 byte   ＼
// pc_no				1 byte    ＼_ 9 bytes
// request_dest_io		2 bytes   /
// request_dest_sta		1 byte   /
// data_length			2 bytes /
// monitor_timer		2 bytes ＼___ 4 bytes
// command				2 bytes /
//*/

// MC Frame Header - Type 3E/Binary  - Request
typedef struct __st_mc_frame_header_send__
{
	UINT8				subheader[2];			/* subheader value */
	UINT8				network_no;				/* network/station number */
	UINT8				plc_no;					/* PLC number */
	UINT16				dest_mod_io;			/* request destination module i/o number */
	UINT8				dest_mod_sta;			/* request destination module station number */
	UINT16				data_length;			/* length of request data section */
	UINT16				monitor_timer;			/* CPU monitoring timer */
	UINT16				command;				/* command */

}	STG_MFHS,	*LPG_MFHS;

// MC Frame Header - Type 3E/Binary - Response
typedef struct __st_mc_frame_header_recv__
{
	UINT8				subheader[2];			/* subheader value */
	UINT8				network_no;				/* network/station number */
	UINT8				plc_no;					/* PLC number */
	UINT16				dest_mod_io;			/* request destination module i/o number */
	UINT8				dest_mod_sta;			/* request destination module station number */
	UINT16				data_length;			/* length of response data */
	UINT16				complete_code;			/* completion code */

}	STG_MFHR,	*LPG_MFHR;

// Type 3E/Binary - Abnormal Response structure
typedef struct __st_mc_frame_recv_abnormal__
{
	UINT8				network_no;				/* responder network/station number */
	UINT8				plc_no;					/* responder PLC number */
	UINT16				dest_mod_io;			/* request destination module i/o number */
	UINT8				dest_mod_sta;			/* request destination module station number */
	UINT16				command;				/* failed command */
	UINT16				subcommand;				/* failed subcommand */

}	STG_MFRA,	*LPG_MFRA;

// Batch Read/Write - Binary [0401/1401]
// Subcommands: [0x00] Word, [0x01] Point
typedef struct __st_mc_frame_sub_command__
{
	UINT16				subcommand;				/* subcommand (0x0000:Word, 0x0001:Bit) */
	UINT8				dev_addr[3];			/* Head device (3 byte integer) 즉, 읽어올 시작 주소 */
	UINT8				dev_code;				/* device type code 즉, 디바이스 종류 'M', 'D', 'X' 등 Hex 값 */
	UINT16				num_points;				/* number of device points */

}	STG_MFSC,	*LPG_MFSC;

#pragma pack (pop)	/* 1 bytes order*/

#pragma pack (push, 4)

/* PLC Device Code String vs. Value */
typedef struct __st_device_code_string_value__
{
	UINT8				bval;					/* 1 Bytes 코드 값 */
	CHAR				cval[3];				/* 2 Bytes 문자열 값 */
}	STG_DCSV,	*LPG_DCSV;

#pragma pack (pop)	/* 4 bytes order */

#pragma pack (push, 1)

/* User Request Command 관련 함수 전달 매개 변수의 개수를 줄이기 위함 (고속 호출을 위해서...) */
typedef struct __st_user_request_command_param__
{
	UINT8				opt_mode;				/* 0 - 값 설정되어 있지 않음. 1 - 주기적으로 요청, 2 - 운영자에 의해 수동 요청 */
	UINT8				dev_code;				/* Device Code 즉, 'M', 'D', 'X', 'Y' 등의 HEX 값 (ENG_DNTC) */
	UINT16				dev_count;				/* 읽기 혹은 쓰기 데이터 개수 (Device Count) */
	UINT32				dev_addr;				/* Device의 시작 주소 (최종 3바이트만 유효) */

}	STG_URCP,	*LPG_URCP;

/* MC Frame Header & Packet Data */
typedef struct __st_mc_frame_packet_recv__
{
	STG_URCP			req_cmd;
	UINT32				size;					/* 'data' 버퍼에 저장된 데이터의 크기 */
	UINT32				u32_reserved[1];
	PUINT8				data;

	VOID Reset()
	{
		memset(&req_cmd, 0x00, sizeof(STG_URCP));
		size	= 0;
		u32_reserved[0]	= 0;
	}

}	STG_MFPR,	*LPG_MFPR;

/* Shared Memory */
typedef struct __st_plc_value_shared_memory__
{
	UINT16				count;					/* WORD 값 설정 개수. 보통 1이고, DWORD인 경우 2 (최대 128 값) */
	UINT16				u16_reserved[3];
	UINT32				addr;					/* 데이터 값 설정을 위한 시작 주소 */
	UINT32				u32_reserved;
//	UINT16				value[MAX_ADDR_WORD_COUNT];

	/*
	 desc : 기존 설정 값 초기화
	 parm : None
	 retn : None
	*/
	VOID ResetValue()
	{
		/* 개수 값만 초기화해도 전체가 초기화 된 것으로 판단됨 */
		count	= 0x0000;
	}

	/*
	 desc : 값이 설정되어 있는지 여부
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsSetValue()
	{
		return count > 0;
	}

}	STG_PVSM,	*LPG_PVSM;

typedef struct __st_plc_data_shared_memory__
{
	LPG_DLSM			link;					/* 연결 상태 정보 */
	LPG_PVSM			setv;					/* 설정 값 */
	PUINT8				data;					/* PLC Data */

}	STG_PDSM,	*LPG_PDSM;

#pragma pack (pop)	/* 1 bytes ordering */
