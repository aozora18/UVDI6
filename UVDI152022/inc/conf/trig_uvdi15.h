
/*
 desc : Trigger Config Information
*/

#pragma once

#include "global.h"
#include "conf_comn.h"
#include "tcpip.h"

/* --------------------------------------------------------------------------------------------- */
/*                                           상수 정의                                           */
/* --------------------------------------------------------------------------------------------- */

#if 0	/* 아래 값은 conf_comn.h 파일에 정의되어 있음  */
#define MAX_TRIG_CHANNEL						4		/* Trigger 장치에 설치 가능한 최대 Channel 개수 */
#endif
#define MAX_REGIST_TRIG_POS_COUNT				101			/* 채널마다 등록 가능한 최대 트리거 개수 */
#define MAX_TRIG_MEM_COUNT						89
#define MAX_TRIG_POS							99999999

/* --------------------------------------------------------------------------------------------- */
/*                                            열거형                                             */
/* --------------------------------------------------------------------------------------------- */

/* Packet Command */
typedef enum class __en_trigger_board_packet_command__ : UINT8
{
	en_none							= 0x00,
	en_write						= 0x01,		/* 각 기능들의 설정 값 저장 */
	en_read							= 0x02,		/* 설정된 값 읽기 */
	en_reset						= 0x03,		/* Board Reset (SW Reset) */
	en_eeprom_write					= 0x04,		/* EEPROM Write */
	en_eeprom_read					= 0x05,		/* EEPROM Read */
	en_ipaddr_write					= 0x07,		/* IP Write */
	en_ipaddr_read					= 0x08,		/* IP Read */
	en_trig_internal				= 0x09,		/* Internal Trigger */
	en_trig_out_one					= 0x0A,		/* 특정 채널 별로 트리거 1개만 발생 */

}	ENG_TBPC;

/* Encoder Counter Read or Reset */
typedef enum class __en_trigger_encoder_read_reset__ : UINT8
{
	en_none							= 0x00,
	en_read							= 0x02,		/* Read */
	en_reset						= 0x03,		/* Clear */

}	ENG_TERR;

/* Trigger & Strobe Enable or Disable */
typedef enum class __en_trigger_strobe_enable_disable__ : UINT8
{
	en_none							= 0x00,
	en_enable						= 0x01,
	en_disable						= 0x03,

}	ENG_TSED;

/* Trigger Encoder Counter Mode */
typedef enum class __en_trigger_encoder_countr_mode__ : UINT8
{
	en_clear						= 0x03,
	en_read							= 0x02,

}	ENG_TECM;

/* Trigger Strobe Enable Disable */
typedef enum class __en_trigger_encoder_enable_disable__ : UINT8
{
	en_disable = 0x00,
	en_positive = 0x01,
	en_negative = 0x02,	// by sysandj : 역방향 Enable 추가
	en_bi_direction = 0x03,

	//0:DISABLE, 1:Positive, 2:Negative, 3:Bi-Direction

}	ENG_TEED;

typedef enum class __en_trigger_memory_address_array__ : UINT8
{
	en_cmd							= 0x00,		/* 0x01 : Write, 0x02 : Read */

	en_encoder_count_reset			= 0x01,		/* Read the current encoder (0x03) / Clear the encoder (0x02) */
	en_trigger_enable				= 0x02,		/* Camera & Strobe Trigger Enable (0x01) or Disable (0x02) */
	/* Camera 1 */
	en_area_mark_trig_pos_1_01		= 0x03,		/* Area Trigger Set Position (1-01) */
	en_area_mark_trig_pos_1_02		= 0x04,
	en_area_mark_trig_pos_1_03		= 0x05,
	en_area_mark_trig_pos_1_04		= 0x06,
	en_area_mark_trig_pos_1_05		= 0x07,
	en_area_mark_trig_pos_1_06		= 0x08,
	en_area_mark_trig_pos_1_07		= 0x09,
	en_area_mark_trig_pos_1_08		= 0x0a,
	en_area_mark_trig_pos_1_09		= 0x0b,
	en_area_mark_trig_pos_1_10		= 0x0c,
	en_area_mark_trig_pos_1_11		= 0x0d,
	en_area_mark_trig_pos_1_12		= 0x0e,
	en_area_mark_trig_pos_1_13		= 0x0f,
	en_area_mark_trig_pos_1_14		= 0x10,
	en_area_mark_trig_pos_1_15		= 0x11,
	en_area_mark_trig_pos_1_16		= 0x12,
	en_area_trigger_on_time_1		= 0x13,		/* high가 유지되는 값을 입력 (1 값 당 1 ns)  */
	en_area_strobe_on_time_1		= 0x14,		/* high가 유지되는 값을 입력 (1 값 당 1 ns)  */
	en_area_trigger_delay_time_1	= 0x15,		/* 출력되는 Trigger의 Delay 값 입력 (1 = 10 ns) */
												/* 이 값이 0 이면 Strobe와 Camera에 Trigger 값이 동시에 출력 */
												/* 0 값 이상이면 Trigger가 Strobe에 먼저 출력되고, 설정 값 만큼 지연된 후에 Camera에 출력 */
	en_area_trigger_plus_1			= 0x16,		/*  */
	en_area_trigger_minus_1			= 0x17,		/*  */
	/* Camera 2 */
	en_area_mark_trig_pos_2_01		= 0x18,		/* Area Trigger Set Position (1-01) */
	en_area_mark_trig_pos_2_02		= 0x19,
	en_area_mark_trig_pos_2_03		= 0x1a,
	en_area_mark_trig_pos_2_04		= 0x1b,
	en_area_mark_trig_pos_2_05		= 0x1c,
	en_area_mark_trig_pos_2_06		= 0x1d,
	en_area_mark_trig_pos_2_07		= 0x1e,
	en_area_mark_trig_pos_2_08		= 0x1f,
	en_area_mark_trig_pos_2_09		= 0x20,
	en_area_mark_trig_pos_2_10		= 0x21,
	en_area_mark_trig_pos_2_11		= 0x22,
	en_area_mark_trig_pos_2_12		= 0x23,
	en_area_mark_trig_pos_2_13		= 0x24,
	en_area_mark_trig_pos_2_14		= 0x25,
	en_area_mark_trig_pos_2_15		= 0x26,
	en_area_mark_trig_pos_2_16		= 0x27,
	en_area_trigger_on_time_2		= 0x28,		/* high가 유지되는 값을 입력 (1 값 당 1 ns)  */
	en_area_strobe_on_time_2		= 0x29,		/* high가 유지되는 값을 입력 (1 값 당 1 ns)  */
	en_area_trigger_delay_time_2	= 0x2a,		/* 출력되는 Trigger의 Delay 값 입력 (1 = 10 ns) */
												/* 이 값이 0 이면 Strobe와 Camera에 Trigger 값이 동시에 출력 */
												/* 0 값 이상이면 Trigger가 Strobe에 먼저 출력되고, 설정 값 만큼 지연된 후에 Camera에 출력 */
	en_area_trigger_plus_2			= 0x2b,		/*  */
	en_area_trigger_minus_2			= 0x2c,		/*  */
	/* Camera 3 */
	en_area_mark_trig_pos_3_01		= 0x2d,		/* Area Trigger Set Position (1-01) */
	en_area_mark_trig_pos_3_02		= 0x2e,
	en_area_mark_trig_pos_3_03		= 0x2f,
	en_area_mark_trig_pos_3_04		= 0x30,
	en_area_mark_trig_pos_3_05		= 0x31,
	en_area_mark_trig_pos_3_06		= 0x32,
	en_area_mark_trig_pos_3_07		= 0x33,
	en_area_mark_trig_pos_3_08		= 0x34,
	en_area_mark_trig_pos_3_09		= 0x35,
	en_area_mark_trig_pos_3_10		= 0x36,
	en_area_mark_trig_pos_3_11		= 0x37,
	en_area_mark_trig_pos_3_12		= 0x38,
	en_area_mark_trig_pos_3_13		= 0x39,
	en_area_mark_trig_pos_3_14		= 0x3a,
	en_area_mark_trig_pos_3_15		= 0x3b,
	en_area_mark_trig_pos_3_16		= 0x3c,
	en_area_trigger_on_time_3		= 0x3d,		/* high가 유지되는 값을 입력 (1 값 당 1 ns)  */
	en_area_strobe_on_time_3		= 0x3e,		/* high가 유지되는 값을 입력 (1 값 당 1 ns)  */
	en_area_trigger_delay_time_3	= 0x3f,		/* 출력되는 Trigger의 Delay 값 입력 (1 = 10 ns) */
												/* 이 값이 0 이면 Strobe와 Camera에 Trigger 값이 동시에 출력 */
												/* 0 값 이상이면 Trigger가 Strobe에 먼저 출력되고, 설정 값 만큼 지연된 후에 Camera에 출력 */
	en_area_trigger_plus_3			= 0x40,		/*  */
	en_area_trigger_minus_3			= 0x41,		/*  */
	/* Camera 4 */
	en_area_mark_trig_pos_4_01		= 0x42,		/* Area Trigger Set Position (1-01) */
	en_area_mark_trig_pos_4_02		= 0x43,
	en_area_mark_trig_pos_4_03		= 0x44,
	en_area_mark_trig_pos_4_04		= 0x45,
	en_area_mark_trig_pos_4_05		= 0x46,
	en_area_mark_trig_pos_4_06		= 0x47,
	en_area_mark_trig_pos_4_07		= 0x48,
	en_area_mark_trig_pos_4_08		= 0x49,
	en_area_mark_trig_pos_4_09		= 0x4a,
	en_area_mark_trig_pos_4_10		= 0x4b,
	en_area_mark_trig_pos_4_11		= 0x4c,
	en_area_mark_trig_pos_4_12		= 0x4d,
	en_area_mark_trig_pos_4_13		= 0x4e,
	en_area_mark_trig_pos_4_14		= 0x4f,
	en_area_mark_trig_pos_4_15		= 0x50,
	en_area_mark_trig_pos_4_16		= 0x51,
	en_area_trigger_on_time_4		= 0x52,		/* high가 유지되는 값을 입력 (1 값 당 1 ns)  */
	en_area_strobe_on_time_4		= 0x53,		/* high가 유지되는 값을 입력 (1 값 당 1 ns)  */
	en_area_trigger_delay_time_4	= 0x54,		/* 출력되는 Trigger의 Delay 값 입력 (1 = 10 ns) */
												/* 이 값이 0 이면 Strobe와 Camera에 Trigger 값이 동시에 출력 */
												/* 0 값 이상이면 Trigger가 Strobe에 먼저 출력되고, 설정 값 만큼 지연된 후에 Camera에 출력 */
	en_area_trigger_plus_4			= 0x55,		/*  */
	en_area_trigger_minus_4			= 0x56,		/*  */

	en_ip_setting					= 0x57,		/* IP Address Setup */
	en_enc_out_enable				= 0x58,		/* Encoder Out Enable */

}	ENG_TMAA;

/* --------------------------------------------------------------------------------------------- */
/*                                            구조체                                             */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 4)

/* 송/수신 패킷 */
typedef struct __st_trig_packet_trigger_strobe__
{
	UINT32				area_trig_pos[MAX_REGIST_TRIG_POS_COUNT];	/* 사용되지 않는 채널의 값은 최대 값 설정 MAX_UINT32 (unit : 100 nm or 0.1 um) */
	UINT32				area_trig_ontime;							/* High가 유지되는 값을 입력 (1 값 = 1 ns) */
	UINT32				area_strob_ontime;							/* Hight가 유지되는 값을 입력 (1 값 = 1 ns) */
	UINT32				area_strob_trig_delay;						/* 출력되는 Trigger의 Delay 값을 입력 (1 ns) */
	INT32				area_trig_pos_plus;							/* Area Trigger Position (1 ~ 16)의 값에 이 Register 값이 증가되어 Strobe와 Trigger 신호가 출력됨 */
																	/* ex> area_trig_pos = 100이고, 이 Register 값이 10이면 Encoder의 110에서 Strobe와 Trigger가 출력됨 */
	INT32				area_trig_pos_minus;						/* Area Trigger Position (1 ~ 16)의 값에 이 Register 값이 감소되어 Strobe와 Trigger 신호가 출력됨 */
																	/* ex> area_trig_pos = 100이고, 이 Register 값이 10이면 Encoder의 90에서 Strobe와 Trigger가 출력됨 */

	/*
	 desc : 변수로 넘어온 구조체와 자신의 데이터가 동일한지 여부
	 parm : data	- [in]  비교 대상이 저장된 구조체 포인터
	 retn : TRUE - 동일하다. FALSE - 다른 값이 존재한다
	*/
	BOOL IsEqualTrigPos(PUINT32 data)
	{
		UINT8 i	= 0;

		for (; i<MAX_REGIST_TRIG_POS_COUNT; i++)
		{
			if (area_trig_pos[i] != data[i])	return FALSE;
		}
		return TRUE;
	}

	/*
	 desc : 트리거 초기화
	 parm : None
	 retn : None
	*/
	VOID ResetTrigPos()
	{
		UINT8 i	= 0;
		for (; i<MAX_REGIST_TRIG_POS_COUNT; i++)	area_trig_pos[i] = MAX_TRIG_POS;
	}

	/*
	 desc : 현재 Trigger Position 값이 설정되어 있지 않은지 여부
	 parm : None
	 retn : TRUE (Not set) or FALSE (Set)
	*/
	BOOL IsSetTrigPos()
	{
		UINT8 i	= 0;
		for (; i<MAX_REGIST_TRIG_POS_COUNT; i++)
		{
			if (area_trig_pos[i] != MAX_TRIG_POS)	return FALSE;
		}
		return TRUE;
	}

	/*
	 desc : Host To Network 변경
	 parm : None
	 retn : None
	*/
	VOID SWAP()
	{
		UINT16 i	= 0;

		for (; i<MAX_REGIST_TRIG_POS_COUNT; i++)
		{
			area_trig_pos[i]= SWAP32(area_trig_pos[i]);
		}
		area_trig_ontime		= SWAP32(area_trig_ontime);
		area_strob_ontime		= SWAP32(area_strob_ontime);
		area_strob_trig_delay	= SWAP32(area_strob_trig_delay);
		area_trig_pos_plus		= SWAP32(area_trig_pos_plus);
		area_trig_pos_minus		= SWAP32(area_trig_pos_minus);
	}

}	STG_TPTS,	*LPG_TPTS;

typedef struct __st_trig_packet_quadrature_register__
{
	/* Command */
	UINT32				command;					/* Read/Write : Low 1 Byte, ENG_TBPC <01 : Write, 02 : Read, 07 : IP Write> */
	/* 현재 엔코더 값을 읽어 오거나 지울 수 있음 */
	INT32				enc_counter;				/* ENG_TERR <Read : 4 Bytes, Write : Low 1 Byte, Encoder Counter Read / Reset> */
	/* Trigger, Strobe 출력 Enable or Disable */
	UINT32				trig_strob_ctrl;			/* ENG_TSED <Read/Write : Low 1 Bytes, 4채널 동시 제어 <0x01 : Enable, 0x03 : Disable>> */
	/* Trigger Position, OnTime, Delay, Encoder Plus or Minus */
	STG_TPTS			trig_set[MAX_TRIG_CHANNEL];	/* 4 채널 값 */
	/* Trigger Board의 IP Address */
	UINT8				ip_addr[4];					/* IP 주소 값이 순서대로 1 Bytes 씩 들어감 */
	/* Encoder Output Value */
	UINT32				enc_out_val;				/* Encoder 내부 트리거 발생 설정 */

	/*
	 desc : 기존 데이터 모두 제거
	 parm : None
	 retn : None
	*/
	VOID ResetPktData()
	{
		command			= 0;
		enc_counter		= 0;
		trig_strob_ctrl	= 0;
		memset(trig_set, 0x00, sizeof(STG_TPTS) * MAX_TRIG_CHANNEL);
		memset(ip_addr, 0x00, 4);
		enc_out_val		= 0;
		/* 반드시 별도 초기화 수행 (트리거 발생 위치를 최대 값으로) */
		ResetTrigPosAll();
	}

	/*
	 desc : 내부 트리거 강제 발생 (Live Mode)
	 parm : None
	 retn : None
	*/
	VOID SetLiveMode()
	{
		UINT16 i	= 0;

		for (; i<MAX_TRIG_CHANNEL; i++)
		{
			trig_set[i].area_trig_ontime	= 0;
			trig_set[i].area_strob_ontime	= 0;
		}
	}

	/*
	 desc : 변수로 넘어온 구조체와 자신의 데이터가 동일한지 여부
	 parm : data	- [in]  비교 대상이 저장된 구조체 포인터
	 retn : TRUE - 동일하다. FALSE - 다른 값이 존재한다
	*/
	BOOL IsEqualData(LPG_TPTS data)
	{
		UINT16 i	= 0;

		for (; i<MAX_TRIG_CHANNEL; i++)
		{
			if (trig_set[i].area_trig_ontime		!= data[i].area_trig_ontime		||
				trig_set[i].area_strob_ontime		!= data[i].area_strob_ontime	||
				trig_set[i].area_trig_pos_plus		!= data[i].area_trig_pos_plus	||
				trig_set[i].area_trig_pos_minus		!= data[i].area_trig_pos_minus	||
				trig_set[i].area_strob_trig_delay	!= data[i].area_strob_trig_delay)	return FALSE;

			/* 16 개의 Trigger Position 값 비교 */
			if (!trig_set[i].IsEqualTrigPos(data[i].area_trig_pos))	return FALSE;
		}
		return TRUE;
	}

	/*
	 desc : 현재 Trigger & Strobe 상태가 Enable인지 여부
	 parm : None
	 retn : TRUE (Enable) or FALSE (Disable)
	*/
	BOOL IsTrigStrobEnabled()
	{
		return 0x01  == trig_strob_ctrl ? TRUE : FALSE;
	}

	/*
	 desc : 현재 Trigger & Strobe 상태가 Disable인지 여부
	 parm : None
	 retn : TRUE (Enable) or FALSE (Disable)
	*/
	BOOL IsTrigStrobDisabled()
	{
		return 0x03  == trig_strob_ctrl ? TRUE : FALSE;
	}

	/*
	 desc : 현재 Trigger Position 값이 설정되어 있지 않은지 여부
	 parm : None
	 retn : TRUE (Not set) or FALSE (Set)
	*/
	BOOL IsSetTrigPos()
	{
		UINT8 i	= 0;
		for (; i<MAX_TRIG_CHANNEL; i++)
		{
			if (!trig_set[i].IsSetTrigPos())	return FALSE;
		}

		return TRUE;
	}

	/*
	 desc : 트리거 초기화
	 parm : ch_no	- [in]  zero based (0x00 ~ 0x03)
	 retn : None
	*/
	VOID ResetTrigPos(UINT8 ch_no)
	{
		if (ch_no >= MAX_TRIG_CHANNEL)	return;
		trig_set[ch_no].ResetTrigPos();
	}

	/*
	 desc : 트리거 전체 초기화
	 parm : None
	 retn : None
	*/
	VOID ResetTrigPosAll()
	{
		for (UINT8 i=0; i<MAX_TRIG_CHANNEL; i++)	trig_set[i].ResetTrigPos();
	}

	/*
	 desc : IP 주소 초기화
	 parm : None
	 retn : None
	*/
	VOID ResetIPv4()
	{
		ip_addr[0] = ip_addr[1] = ip_addr[2] = ip_addr[3] = 0;
	}

	/*
	 desc : Host To Network 변경
	 parm : None
	 retn : None
	*/
	VOID SWAP()
	{
		UINT16 i	= 0;

		command			= SWAP32(command);
		enc_counter		= SWAP32(enc_counter);
		trig_strob_ctrl	= SWAP32(trig_strob_ctrl);
		enc_out_val		= SWAP32(enc_out_val);
		for (; i<MAX_TRIG_CHANNEL; i++)	trig_set[i].SWAP();
	}

}	STG_TPQR,	*LPG_TPQR;

#pragma pack (pop)	/* 1 bytes order */