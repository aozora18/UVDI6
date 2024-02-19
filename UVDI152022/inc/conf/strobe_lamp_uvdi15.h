
/*
 desc : Strobe Lamp Config Information
*/

#pragma once

#include "global.h"
#include "conf_comn.h"
#include "tcpip.h"

#include <cstdint>
#include <vector>

/* --------------------------------------------------------------------------------------------- */
/*                                           ��� ����                                           */
/* --------------------------------------------------------------------------------------------- */
#define		WM_STROBE_LAMP_MSG_THREAD			WM_USER + 4000

#define		DEF_MAX_PAGE_NUMBER					7
#define		DEF_MAX_CHANNEL_NUMBER				7
#define		DEF_MAX_DELAY_VALUE					6000		//	0x1770
#define		DEF_MIN_DELAY_VALUE					0 
#define		DEF_MAX_STROBE_VALUE				1000		//	0x03E8
#define		DEF_MIN_STROBE_VALUE				0
#define		DEF_MAX_STROBE_MODE_NUMBER			7

#define		FIX_DATA_HEADER						(0x4C)
#define		FIX_DATA_TAIL_1						(0x0D)
#define		FIX_DATA_TAIL_2						(0x0A)

#define		DEF_MAX_ARRAY_SIZE					8

/* --------------------------------------------------------------------------------------------- */
/*                                            ������                                             */
/* --------------------------------------------------------------------------------------------- */

/* Packet Command */
enum CommandCode : uint8_t
{
	eSTROBE_LAMP_CHANNEL_DELAY_CONTROL				= 0x12,
	eSTROBE_LAMP_CHANNEL_STROBE_CONTROL				= 0x13,
	eSTROBE_LAMP_CHANNEL_WRITE						= 0x14,
	eSTROBE_LAMP_PAGE_DATA_WRITE					= 0x15,
	eSTROBE_LAMP_PAGE_DATA_READ						= 0x16,
	eSTROBE_LAMP_PAGE_LOOP_DATA_WRITE				= 0x17,
	eSTROBE_LAMP_PAGE_LOOP_DATA_READ				= 0x18,
	eSTROBE_LAMP_STROBE_MODE						= 0x19,
	eSTROBE_LAMP_TRIGGER_MODE						= 0x1A,
	eSTROBE_LAMP_DATA_SAVE							= 0x1B,
	eSTROBE_LAMP_DATA_LOAD							= 0x1C,
	eSTROBE_LAMP_STROBE_TRIGGER_MODE_READ			= 0x20,
};

typedef enum __en_strobe_lamp_error_code__
{
	eSTROBE_LAMP_ERR_OK,				// �̻� ����

 	/* STATUS */
 	eSTROBE_LAMP_ERR_STATUS_MODE					= 10001,	//	���ǵ��� ���� ���
 	eSTROBE_LAMP_ERR_STATUS_EVENT					= 10002,	//	���ǵ��� ���� �̺�Ʈ
 	eSTROBE_LAMP_ERR_STATUS_BUSY					= 10003,	//	�� ���� Ȥ�� ���� ���� ���� ��
 	eSTROBE_LAMP_ERR_STATUS_COMPLETE				= 10004,	//	�� ���� ������ ������ �Ϸ�
 	eSTROBE_LAMP_ERR_STATUS_CHANGE					= 10010,	//	��� ���� �Ұ�
 	eSTROBE_LAMP_ERR_STATUS_INIT					= 10011,	//	�ʱ�ȭ ����
 
	/* �Է� ���� ���� */
	eSTROBE_LAMP_ERR_INVALID_PAGE					= 20000,	//	��ȿ���� ���� ������
	eSTROBE_LAMP_ERR_INVALID_CHANNEL				= 20001,	//	��ȿ���� ���� ä��
	eSTROBE_LAMP_ERR_INVALID_DELAY_VALUE			= 20002,	//	��ȿ���� ���� ������ ��
	eSTROBE_LAMP_ERR_INVALID_STROBE_VALUE			= 20003,	//	��ȿ���� ���� ��Ʈ�κ� ��
	eSTROBE_LAMP_ERR_INVALID_STROBE_MODE			= 20004,	//	��ȿ���� ���� ��Ʈ�κ� ���
	eSTROBE_LAMP_ERR_INVALID_SIZE					= 20005,	//	��ȿ���� ���� ������

 	/* USER */
 	eSTROBE_LAMP_ERR_ACK_TIMEOUT					= 60000,	
 	eSTROBE_LAMP_ERR_DISCONNECT						= 60001,	
 	eSTROBE_LAMP_ERR_PKT_UNIQUEID_MISSMATCH			= 60002,	
 	eSTROBE_LAMP_ERR_PKT_BUFFER_FULL				= 60003,	
 	eSTROBE_LAMP_ERR_NOT_EXIST_RECVDATA				= 60004,	

}	ERR_CODE_STROBE;

typedef enum class __en_strobe_lamp_memory_address_array__ : UINT8
{
	

}	ENG_SLMAA;

/* --------------------------------------------------------------------------------------------- */
/*                                            ����ü                                             */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 1)

struct Header
{
	uint8_t header;						//	0x4C Fixed
	uint8_t command;
};

union Body
{
	
	struct 
	{
		uint8_t page;
		uint8_t channel;
		uint8_t delayHighValue;
		uint8_t delayLowValue;
	} channelDelayControl;				// ���� ä�� Delay ����

	struct
	{
		uint8_t page;
		uint8_t channel;
		uint8_t strobeHighValue;
		uint8_t strobeLowValue;
	} channelStrobeControl;				// ���� ä�� Strobe ����

	struct
	{
		uint8_t page;
		uint8_t channel;
		uint8_t delayHighValue;
		uint8_t delayLowValue;
		uint8_t strobeHighValue;
		uint8_t strobeLowValue;
	} channelWrite;						// ���� ä�� Write

	struct
	{
		uint8_t page;
		uint8_t delayHighValue[DEF_MAX_ARRAY_SIZE];
		uint8_t delayLowValue[DEF_MAX_ARRAY_SIZE];
		uint8_t strobeHighValue[DEF_MAX_ARRAY_SIZE];
		uint8_t strobeLowValue[DEF_MAX_ARRAY_SIZE];
	} pageDataWrite;					// Page Data Write

	struct
	{
		uint8_t page;
	} pageDataReadRequest;				// Page Date Read Request

	struct
	{
		uint8_t page;
		uint8_t delayHighValue[DEF_MAX_ARRAY_SIZE];
		uint8_t delayLowValue[DEF_MAX_ARRAY_SIZE];
		uint8_t strobeHighValue[DEF_MAX_ARRAY_SIZE];
		uint8_t strobeLowValue[DEF_MAX_ARRAY_SIZE];
	} pageDataReadResponse;				// Page Date Read Response

	struct
	{
		uint8_t page;
		uint8_t loopDelay3Value;
		uint8_t loopDelay2Value;
		uint8_t loopDelay1Value;
		uint8_t loopDelay0Value;
		uint8_t loopCount;
	} pageLoopDataWrite;				// Page Loop Data Write

	struct
	{
		uint8_t page;
	} pageLoopDataReadRequest;			// Page Loop Data Read Request

	struct
	{
		uint8_t page;
		uint8_t loopDelay3Value;
		uint8_t loopDelay2Value;
		uint8_t loopDelay1Value;
		uint8_t loopDelay0Value;
		uint8_t loopCount;
	} pageLoopDataReadResponse;			// Page Loop Data Read Response

	struct
	{
		uint8_t mode;
	} strobeMode;						// Strobe Mode

	struct
	{
		uint8_t mode;
	} triggerMode;						// Trigger Mode

	struct
	{
		uint8_t reserved;
	} dataSave;							// Data Save

	struct
	{
		uint8_t reserved;
	} dataLoad;							// Data Load

	struct
	{
		uint8_t reserved;
	} strobeTriggerModeReadRequest;		// Strobe/Trigger Mode Read Request

	struct
	{
		uint8_t mode1;
		uint8_t mode2;
	} strobeTriggerModeReadResponse;	// Strobe/Trigger Mode Read Response
};

struct Tail
{
	uint8_t checksum;			//	Header�� CheckSum, Tail1, 2�� ������ ��� Data�� X-OR�� ������Դϴ�.
	uint8_t tail1;				//	0x0D Fixed
	uint8_t tail2;				//	0x0A Fixed
};

#pragma pack (pop)	/* 1 bytes order */

#pragma pack (push, 1)
typedef struct stStrobeLampMessage
{
	Header	header;
	Body	body;
	Tail	tail;

	stStrobeLampMessage(uint8_t command = 0)
	{
		header.header	= FIX_DATA_HEADER;
		header.command	= command;
		tail.tail1		= FIX_DATA_TAIL_1;
		tail.tail2		= FIX_DATA_TAIL_2;
	}

	size_t GetSize() const
	{
		return sizeof(header) + GetBodyRequestSize() + sizeof(tail);
	}

	size_t stStrobeLampMessage::GetBodyRequestSize() const
	{
		switch (header.command)
		{
		case eSTROBE_LAMP_CHANNEL_DELAY_CONTROL:			return sizeof(body.channelDelayControl);
		case eSTROBE_LAMP_CHANNEL_STROBE_CONTROL:			return sizeof(body.channelStrobeControl);
		case eSTROBE_LAMP_CHANNEL_WRITE:					return sizeof(body.channelWrite);
		case eSTROBE_LAMP_PAGE_DATA_WRITE:					return sizeof(body.pageDataWrite);
		case eSTROBE_LAMP_PAGE_DATA_READ:					return sizeof(body.pageDataReadRequest);
		case eSTROBE_LAMP_PAGE_LOOP_DATA_WRITE:				return sizeof(body.pageLoopDataWrite);
		case eSTROBE_LAMP_PAGE_LOOP_DATA_READ:				return sizeof(body.pageLoopDataReadRequest);
		case eSTROBE_LAMP_STROBE_MODE:						return sizeof(body.strobeMode);
		case eSTROBE_LAMP_TRIGGER_MODE:						return sizeof(body.triggerMode);
		case eSTROBE_LAMP_DATA_SAVE:						return sizeof(body.dataSave);
		case eSTROBE_LAMP_DATA_LOAD:						return sizeof(body.dataLoad);
		case eSTROBE_LAMP_STROBE_TRIGGER_MODE_READ:			return sizeof(body.strobeTriggerModeReadRequest);
		default:											return sizeof(body);			
		}
	}

}	STG_SLPR_SEND, STG_SLPR_RECV, * LPG_SLPR;



#pragma pack (pop)	/* 1 bytes order */