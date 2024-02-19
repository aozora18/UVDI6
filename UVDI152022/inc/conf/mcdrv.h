
/* --------------------------------------------------------------------------------------------- */
/* desc : MC Protocol ���� ���                                                                  */
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
#define MC_MAX_WORD_LEN				0x01e0	// 480�� ���� ���ƾ� �Ѵ� (?)

/* �ѹ��� ���� �� �ִ� �ִ� ũ�� (����: Bytes) - 1024 Bytes �̻� �б� ��û�ϸ� PLC ����! */
#define MC_READ_BLOCK_SIZE			512

/* PLC �� ��� ������ ����� ���� �ִ� ���� */
#define MAX_ADDR_WORD_COUNT			128

/* --------------------------------------------------------------------------------------------- */
/*                                       ����(Global) ������                                     */
/* --------------------------------------------------------------------------------------------- */

// MELSEC Memory ���� ������ ���� ��, Device Code Name ����
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

// PLC �ʿ� MC Protocol�� ������ ��û�ϴ� ���
typedef enum class __en_tcp_socket_req_method__ : UINT8
{
	en_req_none		= 0x00,		// �ƹ��͵� ���� ����?
	en_req_period	= 0x01,		// �ֱ������� ��û�ϴ� ���
	en_req_operate	= 0x02,		// ����� (���) �䱸 ���

}	ENG_TSRM;

// Melsec Memory�� �ϰ� �б� / ����
typedef enum class __en_memory_batch_read_write_ : UINT16
{
	en_read			= 0x0401,	// �ϰ� �б�
	en_write		= 0x1401,	// �ϰ� ����

}	ENG_MBRW;

// Melsec Memory�� ��Ʈ or ���� ���� ����
typedef enum class __en_memory_read_write_mode__ : UINT16
{
	en_bits			= 0x0001,	// ��Ʈ ����
	en_word			= 0x0000,	// ���� ����

}	ENG_MRWM;


/* --------------------------------------------------------------------------------------------- */
/*                                       ����(Global) ����ü                                     */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 1)

///*
// subheader			2 bytes ��
// network_no			1 byte   ��
// pc_no				1 byte    ��_ 9 bytes
// request_dest_io		2 bytes   /
// request_dest_sta		1 byte   /
// data_length			2 bytes /
// monitor_timer		2 bytes ��___ 4 bytes
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
	UINT8				dev_addr[3];			/* Head device (3 byte integer) ��, �о�� ���� �ּ� */
	UINT8				dev_code;				/* device type code ��, ����̽� ���� 'M', 'D', 'X' �� Hex �� */
	UINT16				num_points;				/* number of device points */

}	STG_MFSC,	*LPG_MFSC;

#pragma pack (pop)	/* 1 bytes order*/

#pragma pack (push, 4)

/* PLC Device Code String vs. Value */
typedef struct __st_device_code_string_value__
{
	UINT8				bval;					/* 1 Bytes �ڵ� �� */
	CHAR				cval[3];				/* 2 Bytes ���ڿ� �� */
}	STG_DCSV,	*LPG_DCSV;

#pragma pack (pop)	/* 4 bytes order */

#pragma pack (push, 1)

/* User Request Command ���� �Լ� ���� �Ű� ������ ������ ���̱� ���� (��� ȣ���� ���ؼ�...) */
typedef struct __st_user_request_command_param__
{
	UINT8				opt_mode;				/* 0 - �� �����Ǿ� ���� ����. 1 - �ֱ������� ��û, 2 - ��ڿ� ���� ���� ��û */
	UINT8				dev_code;				/* Device Code ��, 'M', 'D', 'X', 'Y' ���� HEX �� (ENG_DNTC) */
	UINT16				dev_count;				/* �б� Ȥ�� ���� ������ ���� (Device Count) */
	UINT32				dev_addr;				/* Device�� ���� �ּ� (���� 3����Ʈ�� ��ȿ) */

}	STG_URCP,	*LPG_URCP;

/* MC Frame Header & Packet Data */
typedef struct __st_mc_frame_packet_recv__
{
	STG_URCP			req_cmd;
	UINT32				size;					/* 'data' ���ۿ� ����� �������� ũ�� */
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
	UINT16				count;					/* WORD �� ���� ����. ���� 1�̰�, DWORD�� ��� 2 (�ִ� 128 ��) */
	UINT16				u16_reserved[3];
	UINT32				addr;					/* ������ �� ������ ���� ���� �ּ� */
	UINT32				u32_reserved;
//	UINT16				value[MAX_ADDR_WORD_COUNT];

	/*
	 desc : ���� ���� �� �ʱ�ȭ
	 parm : None
	 retn : None
	*/
	VOID ResetValue()
	{
		/* ���� ���� �ʱ�ȭ�ص� ��ü�� �ʱ�ȭ �� ������ �Ǵܵ� */
		count	= 0x0000;
	}

	/*
	 desc : ���� �����Ǿ� �ִ��� ����
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
	LPG_DLSM			link;					/* ���� ���� ���� */
	LPG_PVSM			setv;					/* ���� �� */
	PUINT8				data;					/* PLC Data */

}	STG_PDSM,	*LPG_PDSM;

#pragma pack (pop)	/* 1 bytes ordering */
