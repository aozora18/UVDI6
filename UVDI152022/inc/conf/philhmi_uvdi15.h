
/*
 desc : philhmi Config Information
*/

#pragma once

#include "global.h"
#include "conf_comn.h"
#include "tcpip.h"

/* --------------------------------------------------------------------------------------------- */
/*                                           상수 정의                                           */
/* --------------------------------------------------------------------------------------------- */
#define		WM_PHILHMI_MSG_THREAD				WM_USER + 3000
#define		DEF_MAX_RECIPE_NAME_LENGTH			128
#define		DEF_MAX_RECIPE_COUNT				200
#define		DEF_MAX_RECIPE_PARAM_TYPE_LENGTH	10
#define		DEF_MAX_RECIPE_PARAM_NAME_LENGTH	40
#define		DEF_MAX_RECIPE_PARAM_VALUE_LENGTH	512
#define		DEF_MAX_RECIPE_PARAM_COUNT			200
#define		DEF_MAX_ECM_PARAM_COUNT				100
#define		DEF_MAX_PROCESS_PARAM_COUNT			100
#define		DEF_MAX_POSITION_AXIS_LENGTH		40
#define		DEF_MAX_POSITION_NAME_LENGTH		20
#define		DEF_MAX_GLASS_NAME_LENGTH			40
#define		DEF_MAX_STATE_PARAM_TYPE_LENGTH		10
#define		DEF_MAX_STATE_PARAM_NAME_LENGTH		40
#define		DEF_MAX_STATE_PARAM_VALUE_LENGTH	100
#define		DEF_MAX_PROCESS_PARAM_VALUE_LENGTH	20
#define		DEF_MAX_SUBPROCESS_NAME_LENGTH		20
#define		DEF_MAX_EVENT_NAME_LENGTH			40
#define		DEF_TIME_LENGTH						14
#define		DEF_MAX_IO_COUNT					50
#define		DEF_MAX_MOVE_COUNT					5

/* --------------------------------------------------------------------------------------------- */
/*                                            열거형                                             */
/* --------------------------------------------------------------------------------------------- */

/* Packet Command */
typedef enum class __en_philhmi_packet_command__ : UINT16
{
ePHILHMI_C2P_RECIPE_CREATE			= 101,	
	ePHILHMI_C2P_RECIPE_DELETE				,	
	ePHILHMI_C2P_RECIPE_MODIFY				,
	ePHILHMI_C2P_RECIPE_SELECT				,	
	ePHILHMI_C2P_RECIPE_LIST				,	
	ePHILHMI_C2P_RECIPE_INFORMATION			,
	ePHILHMI_C2P_ABS_MOVE				= 313,	
	ePHILHMI_C2P_ABS_MOVE_COMPLETE		= 314,	
	ePHILHMI_C2P_REL_MOVE				= 315,	
	ePHILHMI_C2P_REL_MOVE_COMPLETE		= 316,	
	ePHILHMI_C2P_CHAR_MOVE				= 317,	
	ePHILHMI_C2P_CHAR_MOVE_COMPLETE		= 318,	
	ePHILHMI_C2P_PROCESS_EXECUTE		= 502,	
	ePHILHMI_C2P_SUB_PROCESS_EXECUTE	= 503,
	ePHILHMI_C2P_EC_INFORMATION			= 701,
	ePHILHMI_C2P_STATUS_VALUE			= 702,
	ePHILHMI_C2P_MODE_CHANGE			= 703,
	ePHILHMI_C2P_INITIAL_EXECUTE		= 704,
	ePHILHMI_C2P_EVENT_STATUS			= 710,
	ePHILHMI_C2P_EVENT_NOTIFY			= 711,	
	ePHILHMI_C2P_TIME_SYNC				= 712,	
	ePHILHMI_C2P_INTERRUPT_STOP			= 991,	
	ePHILHMI_P2C_RECIPE_CREATE			= 201,	
	ePHILHMI_P2C_RECIPE_DELETE				,	
	ePHILHMI_P2C_RECIPE_MODIFY				,	
	ePHILHMI_P2C_RECIPE_SELECT				,
	ePHILHMI_P2C_IO_STATUS				= 401,	
	ePHILHMI_P2C_OUTPUT_ONOFF			= 402,	
	ePHILHMI_P2C_ABS_MOVE				= 413,	
	ePHILHMI_P2C_ABS_MOVE_COMPLETE		= 414,	
	ePHILHMI_P2C_REL_MOVE				= 415,	
	ePHILHMI_P2C_REL_MOVE_COMPLETE		= 416,	
	ePHILHMI_P2C_CHAR_MOVE				= 417,	
	ePHILHMI_P2C_CHAR_MOVE_COMPLETE		= 418,	
	ePHILHMI_P2C_PROCESS_STEP			= 601,	
	ePHILHMI_P2C_PROCESS_COMPLETE		= 602,	
	ePHILHMI_P2C_SUB_PROCESS_COMPLETE	= 603,	
	ePHILHMI_P2C_EC_MODIFY				= 801,
	ePHILHMI_P2C_INITIAL_COMPLETE		= 802,	
	ePHILHMI_P2C_ALARM_OCCUR			= 803,	
	ePHILHMI_P2C_EVENT_STATUS			= 810,
	ePHILHMI_P2C_EVENT_NOTIFY			= 811,	
	ePHILHMI_P2C_INTERRUPT_STOP			= 992,	

}	ENG_PHPC;

enum __en_phiihmi_error_code__
{
	ePHILHMI_ERR_OK,				// 이상 없음
	/* RECIPE */
	ePHILHMI_ERR_RECIPE_NOT_DEFINE				= 10001,	//	정의되지 않은 레시피
	ePHILHMI_ERR_RECIPE_NOT_DEFINE_PROCESS		= 10002,	//	정의되지 않은 보조 공정
	ePHILHMI_ERR_RECIPE_OUT_OF_RANGE			= 10003,	//	배열 최대 개수 초과
	ePHILHMI_ERR_RECIPE_DELETE					= 10004,	//	레시피 삭제 불가
	ePHILHMI_ERR_RECIPE_DUPLICATE_NAME			= 10005,	//  레시피 이름 중복 
	ePHILHMI_ERR_RECIPE_CREATE					= 10006,	//  레시피 생성 불가
	ePHILHMI_ERR_RECIPE_MODIFY					= 10007,	//  레시피 변경 불가
	ePHILHMI_ERR_RECIPE_LOAD					= 10008,	//  레시피 로드 불가
	ePHILHMI_ERR_SEND_RECD_MISMATCH				= 10010,	//  요청 <-> 완료 불일치
	ePHILHMI_ERR_RECIPE_NOT_DEFINE_TYPE			= 11000,	//	정의되지 않은 데이터 타입
	ePHILHMI_ERR_RECIPE_NOT_DEFINE_PARAMETER	= 12000,	//	정의되지 않은 파라미터
	ePHILHMI_ERR_RECIPE_EXCEED_LOW				= 13000,	//	파라미터 하한값 미달
	ePHILHMI_ERR_RECIPE_EXCEED_HIGH				= 14000,	//	파라미터 상한값 초과
	/* ACTUATOR	*/
	ePHILHMI_ERR_MOTOR_NOT_DEFINE				= 20001,	//	정의되지 않은 모터
	ePHILHMI_ERR_MOTOR_NOT_DEFINE_INPUT			= 20002,	//	정의되지 않은 입력
	ePHILHMI_ERR_MOTOR_NOT_DEFINE_OUTPUT		= 20003,	//	정의되지 않은 출력
	ePHILHMI_ERR_MOTOR_MOVING					= 20010,	//	모터 구동 중
	ePHILHMI_ERR_MOTOR_CMD_FAILED				= 20011,	//	모터 구동 실패
	ePHILHMI_ERR_MOTOR_PLM						= 20012,	//	PLM 센서 감지 중
	ePHILHMI_ERR_MOTOR_NLM						= 20013,	//	NLM 센서 감지 중
	ePHILHMI_ERR_MOTOR_ALARM					= 20014,	//	모터 알람 상태
	//ePHILHMI_ERR_MOTOR_OFF_LOCK					= 20015,	//	모터 서버 Locked off 상태
	ePHILHMI_ERR_MOTOR_RUN_TIMEOUT				= 20015,	//	모터 구동 시간 초과
	//ePHILHMI_ERR_MOTOR_IO_CHECK					= 20016,	//  모터 동작에 방해하는 IO 상태
	/* STATUS */
	ePHILHMI_ERR_STATUS_MODE					= 30001,	//	정의되지 않은 모드
	ePHILHMI_ERR_STATUS_EVENT					= 30002,	//	정의되지 않은 이벤트
	ePHILHMI_ERR_STATUS_BUSY					= 30003,	//	주 공정 혹은 보조 공정 진행 중
	ePHILHMI_ERR_STATUS_COMPLETE				= 30004,	//	주 공정 시퀀스 비정상 완료
	ePHILHMI_ERR_STATUS_FAILED					= 30005,	//	주 공정, 진행 불가
	ePHILHMI_ERR_RECIPE_VALIDATE_FAILED			= 30006,	//	레시피불일치
	ePHILHMI_ERR_STATUS_CHANGE					= 30010,	//	모드 변경 불가
	ePHILHMI_ERR_STATUS_INIT					= 30011,	//	초기화 실패
	ePHILHMI_ERR_STATUS_INIT_NEED				= 30012,	//	초기화 필요

	/* USER */
	ePHILHMI_ERR_ACK_TIMEOUT					= 60000,	
	ePHILHMI_ERR_DISCONNECT						= 60001,	
	ePHILHMI_ERR_PKT_UNIQUEID_MISSMATCH			= 60002,	
	ePHILHMI_ERR_PKT_BUFFER_FULL				= 60003,	
	ePHILHMI_ERR_NOT_EXIST_RECVDATA				= 60004,	
};

typedef enum class __en_philhmi_memory_address_array__ : UINT8
{
	

}	ENG_PMAA;

/* --------------------------------------------------------------------------------------------- */
/*                                            구조체                                             */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 1)

typedef struct __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = -1;
		ulDataLen = 0;
		ulUniqueID = 0;
		usErrorCode = 0;
	};

	int				nCommand;
	unsigned long	ulDataLen;
	unsigned long	ulUniqueID;
	unsigned short	usErrorCode;
		
}	STG_PPH, * LPG_PPH;

typedef struct __st_phil_packet_dynamic_recipe__
{
	void Reset()
	{
		ZeroMemory(this, sizeof(*this));
	}

	char			szParameterType[DEF_MAX_RECIPE_PARAM_TYPE_LENGTH];	// Parameter 데이터 타입
	//	- BOOL
	//	- INT
	//	- DOUBLE
	//	- STRING

	char			szParameterName[DEF_MAX_RECIPE_PARAM_NAME_LENGTH];  // Parameter 명은 최대 40자		
	char			szParameterValue[DEF_MAX_RECIPE_PARAM_VALUE_LENGTH];// Parameter 값은 최대 10자		
}	STG_PP_VAR, * LPG_PP_VAR;

typedef struct __st_phil_packet_dynamic_state__
{
	void Reset()
	{
		ZeroMemory(this, sizeof(*this));
	}

	char			szParameterType[DEF_MAX_STATE_PARAM_TYPE_LENGTH];	// Parameter 데이터 타입
	//	- BOOL
	//	- INT
	//	- DOUBLE
	//	- STRING

	char			szParameterName[DEF_MAX_STATE_PARAM_NAME_LENGTH];  // Parameter 명은 최대 40자		
	char			szParameterValue[DEF_MAX_STATE_PARAM_VALUE_LENGTH];// Parameter 값은 최대 10자		
}	STG_PP_STATE, * LPG_PP_STATE;

/* 송/수신 패킷 */
//////////////////////////////////////////////////////////////////////////
/// RECIPE INFORMATION
typedef struct __st_phil_packet_cp2_recipe_information__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_RECIPE_INFORMATION;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(szRecipeName, 0, sizeof(szRecipeName));
	}

	char			szRecipeName[DEF_MAX_RECIPE_NAME_LENGTH];					// Recipe 명은 최대 40자		

}	STG_PP_C2P_RCP_INFO, * LPG_PP_C2P_RCP_INFO;


typedef struct __st_phil_packet_cp2_recipe_information_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_RECIPE_INFORMATION;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(szRecipeName, 0, sizeof(szRecipeName));
		usCount = 0;

		memset(stVar, 0, sizeof(STG_PP_VAR) * DEF_MAX_RECIPE_PARAM_COUNT);
	}

	char			szRecipeName[DEF_MAX_RECIPE_NAME_LENGTH];					// Recipe 명은 최대 40자		
	unsigned short	usCount;													// DV Parameter 개수		
	STG_PP_VAR		stVar[DEF_MAX_RECIPE_PARAM_COUNT];

}	STG_PP_C2P_RCP_INFO_ACK, * LPG_PP_RCP_INFO_ACK;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// RECIPE LIST
typedef struct __st_phil_packet_c2p_recipe_list__ : public __st_phil_packet_header__
{

	void Reset()
	{
		nCommand	= (int)ENG_PHPC::ePHILHMI_C2P_RECIPE_LIST;
		ulDataLen	= sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID	= 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_C2P_RCP_LIST,	*LPG_PP_C2P_RCP_LIST;

typedef struct __st_phil_packet_c2p_recipe_list_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand		= (int)ENG_PHPC::ePHILHMI_C2P_RECIPE_LIST;
		ulDataLen		= sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID		= 0;
		usErrorCode		= ePHILHMI_ERR_OK;

		usCount = 0;
		memset(szSelectedRecipeName, 0, sizeof(char) * DEF_MAX_RECIPE_NAME_LENGTH);
		memset(szArrRecipeName, 0, sizeof(char) * DEF_MAX_RECIPE_COUNT * DEF_MAX_RECIPE_NAME_LENGTH);
	}

	unsigned short	usCount;						// Recipe 개수		
	char			szSelectedRecipeName[DEF_MAX_RECIPE_NAME_LENGTH];
	char			szArrRecipeName[DEF_MAX_RECIPE_COUNT][DEF_MAX_RECIPE_NAME_LENGTH];		// Recipe 명은 최대 40자, 200개로 제한		

}	STG_PP_C2P_RCP_LIST_ACK, * LPG_PP_C2P_RCP_LIST_ACK;
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
/// RECIPE SELECT
typedef struct __st_phil_packet_c2p_recipe_select__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand	= (int)ENG_PHPC::ePHILHMI_C2P_RECIPE_SELECT;
		ulDataLen	= sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID	= 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(szRecipeName, 0, sizeof(szRecipeName));
	}

	char szRecipeName[DEF_MAX_RECIPE_NAME_LENGTH];      // Recipe 명은 최대 40자		

}	STG_PP_C2P_RCP_SELECT, * LPG_PP_C2P_RCP_SELECT;

typedef struct __st_phil_packet_c2p_recipe_select_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_RECIPE_SELECT;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_C2P_RCP_SELECT_ACK, * LPG_PP_C2P_RCP_SELECT_ACK;

typedef struct __st_phil_packet_p2c_recipe_select__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_RECIPE_SELECT;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(szRecipeName, 0, sizeof(szRecipeName));
	}

	char szRecipeName[DEF_MAX_RECIPE_NAME_LENGTH];      // Recipe 명은 최대 40자		

}	STG_PP_P2C_RCP_SELECT, * LPG_PP_P2C_RCP_SELECT;

typedef struct __st_phil_packet_p2c_recipe_select_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_RECIPE_SELECT;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_P2C_RCP_SELECT_ACK, * LPG_PP_P2C_RCP_SELECT_ACK;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// RECIPE CREATE
typedef struct __st_phil_packet_c2p_recipe_create__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_RECIPE_CREATE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(szRecipeName, 0, sizeof(szRecipeName));
		usCount = 0;
		memset(stVar, 0, sizeof(STG_PP_VAR) * DEF_MAX_RECIPE_PARAM_COUNT);
	}

	char szRecipeName[DEF_MAX_RECIPE_NAME_LENGTH];      // Recipe 명은 최대 40자			
	unsigned short	usCount;							// DV Parameter 개수		
	STG_PP_VAR		stVar[DEF_MAX_RECIPE_PARAM_COUNT];

}	STG_PP_C2P_RCP_CREATE, * LPG_PP_C2P_RCP_CREATE;

typedef struct __st_phil_packet_c2p_recipe_create_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_RECIPE_CREATE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_C2P_RCP_CREATE_ACK, * LPG_PP_C2P_RCP_CREATE_ACK;

typedef struct __st_phil_packet_p2c_recipe_create__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_RECIPE_CREATE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(stVar, 0, sizeof(STG_PP_VAR) * DEF_MAX_RECIPE_PARAM_COUNT);
		usCount = 0;
		memset(szRecipeName, 0, sizeof(szRecipeName));
	}

	char szRecipeName[DEF_MAX_RECIPE_NAME_LENGTH];      // Recipe 명은 최대 40자			
	unsigned short	usCount;							// DV Parameter 개수		
	STG_PP_VAR		stVar[DEF_MAX_RECIPE_PARAM_COUNT];

}	STG_PP_P2C_RCP_CREATE, * LPG_PP_P2C_RCP_CREATE;

typedef struct __st_phil_packet_p2c_recipe_create_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_RECIPE_CREATE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_P2C_RCP_CREATE_ACK, * LPG_PP_P2C_RCP_CREATE_ACK;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// RECIPE DELETE
typedef struct __st_phil_packet_c2p_recipe_delete__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_RECIPE_DELETE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(szRecipeName, 0, sizeof(szRecipeName));
	}

	char szRecipeName[DEF_MAX_RECIPE_NAME_LENGTH];      // Recipe 명은 최대 40자		

}	STG_PP_C2P_RCP_DELETE, * LPG_PP_C2P_RCP_DELETE;

typedef struct __st_phil_packet_c2p_recipe_delete_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_RECIPE_DELETE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_C2P_RCP_DELETE_ACK, * LPG_PP_C2P_RCP_DELETE_ACK;

typedef struct __st_phil_packet_p2c_recipe_delete__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_RECIPE_DELETE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(szRecipeName, 0, sizeof(szRecipeName));
	}

	char szRecipeName[DEF_MAX_RECIPE_NAME_LENGTH];      // Recipe 명은 최대 40자		

}	STG_PP_P2C_RCP_DELETE, * LPG_PP_P2C_RCP_DELETE;

typedef struct __st_phil_packet_p2c_recipe_delete_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_RECIPE_DELETE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_P2C_RCP_DELETE_ACK, * LPG_PP_P2C_RCP_DELETE_ACK;
//////////////////////////////////////////////////////////////////////////

typedef struct __st_phil_packet_c2p_recipe_modify__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand	= (int)ENG_PHPC::ePHILHMI_C2P_RECIPE_MODIFY;
		ulDataLen	= sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID	= 0;
		usErrorCode	= ePHILHMI_ERR_OK;

		memset(szRecipeName, 0, sizeof(szRecipeName));
		usCount = 0;
		memset(stVar, 0, sizeof(STG_PP_VAR) * DEF_MAX_RECIPE_PARAM_COUNT);
	}

	char			szRecipeName[DEF_MAX_RECIPE_NAME_LENGTH];   // Recipe 명은 최대 40자		
	unsigned short	usCount;									// Recipe Parameter 개수		
	STG_PP_VAR		stVar[DEF_MAX_RECIPE_PARAM_COUNT];

}	STG_PP_C2P_RCP_MODIFY, * LPG_PP_C2P_RCP_MODIFY;

typedef struct __st_phil_packet_c2p_recipe_modify_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_RECIPE_MODIFY;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_C2P_RCP_MODIFY_ACK, * LPG_PP_C2P_RCP_MODIFY_ACK;

typedef struct __st_phil_packet_p2c_recipe_modify__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_RECIPE_MODIFY;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(szRecipeName, 0, sizeof(szRecipeName));
		usCount = 0;
		memset(stVar, 0, sizeof(STG_PP_VAR) * DEF_MAX_RECIPE_PARAM_COUNT);
	}

	char			szRecipeName[DEF_MAX_RECIPE_NAME_LENGTH];   // Recipe 명은 최대 40자		
	unsigned short	usCount;									// Recipe Parameter 개수		
	STG_PP_VAR		stVar[DEF_MAX_RECIPE_PARAM_COUNT];

}	STG_PP_P2C_RCP_MODIFY, * LPG_PP_P2C_RCP_MODIFY;

typedef struct __st_phil_packet_p2c_recipe_modify_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_RECIPE_MODIFY;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_P2C_RCP_MODIFY_ACK, * LPG_PP_P2C_RCP_MODIFY_ACK;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// IO_STATUS
typedef struct __st_phil_packet_p2c_io_status__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_IO_STATUS;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_P2C_IO_STATUS, * LPG_PP_P2C_IO_STATUS;

typedef struct __st_phil_packet_p2c_io_status_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_IO_STATUS;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		byInputCount = 0;
		byOutputCount = 0;
		memset(usInputData, 0, sizeof(usInputData));
		memset(usOutputData, 0, sizeof(usOutputData));
	}

	BYTE byInputCount;									// 최대 50 개		
	BYTE byOutputCount;									// 최대 50 개		
	unsigned short usInputData[DEF_MAX_IO_COUNT];		// 16점 Data : ex) 1번 Board의 1,5,15 On일 경우 0x4011		
	unsigned short usOutputData[DEF_MAX_IO_COUNT];		// 16점 Data		
	
}	STG_PP_P2C_IO_STATUS_ACK, * LPG_PP_P2C_IO_STATUS_ACK;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// OUTPUT_ONOFF
typedef struct __st_phil_packet_p2c_output_onoff__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_OUTPUT_ONOFF;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		usIndex = 0;
		usSignal = 0;
	}

	unsigned short usIndex;
	unsigned short usSignal;           // 0:Off 1:On		

}	STG_PP_P2C_OUTPUT_ONOFF, * LPG_PP_P2C_OUTPUT_ONOFF;

typedef struct __st_phil_packet_p2c_output_onoff_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_OUTPUT_ONOFF;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_P2C_OUTPUT_ONOFF_ACK, * LPG_PP_P2C_OUTPUT_ONOFF_ACK;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// ABS_MOVE
typedef struct __st_phil_packet_move_info__
{
	void Reset()
	{
		ZeroMemory(this, sizeof(*this));
	}

	char szAxisName[DEF_MAX_POSITION_AXIS_LENGTH];        // 사전 협의 된 Axis 이름		
	double dPosition;									// 이동 할 절대 Position 값		
	double dSpeed;										// 이동 할 Speed 값		
	double dAcc;										// 이동 할 Acc 값		
	unsigned short usTimeout;							// 이동 완료 대기 시간			
}	STG_PP_MOVE, * LPG_PP_MOVE;

typedef struct __st_phil_packet_move_teach__
{
	void Reset()
	{
		ZeroMemory(this, sizeof(*this));
	}

	char szAxisName[DEF_MAX_POSITION_AXIS_LENGTH];           // 사전 협의 된 Axis 이름		
	char szPositionName[DEF_MAX_POSITION_NAME_LENGTH];       // Position 명은 최대 20자		
	unsigned short usTimeout;      // 이동 완료 대기 시간		

}	STG_PP_MOVE_TEACH, * LPG_PP_MOVE_TEACH;

typedef struct __st_phil_packet_c2p_abs_move__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_ABS_MOVE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		usCount = 0;
		memset(stMove, 0, sizeof(STG_PP_MOVE) * _countof(stMove));
	}

	unsigned short	usCount;						// Move Axis 개수		
	STG_PP_MOVE		stMove[DEF_MAX_MOVE_COUNT];     // 최대 5개로 제한		

}	STG_PP_C2P_ABS_MOVE, * LPG_PP_C2P_ABS_MOVE;

typedef struct __st_phil_packet_c2p_abs_move_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_ABS_MOVE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}
}	STG_PP_C2P_ABS_MOVE_ACK, * LPG_PP_C2P_ABS_MOVE_ACK;

typedef struct __st_phil_packet_c2p_abs_move_complete__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_ABS_MOVE_COMPLETE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_C2P_ABS_MOVE_COMP, * LPG_PP_C2P_ABS_MOVE_COMP;


typedef struct __st_phil_packet_c2p_abs_move_complete_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_ABS_MOVE_COMPLETE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_C2P_ABS_MOVE_COMP_ACK, * LPG_PP_C2P_ABS_MOVE_COMP_ACK;

typedef struct __st_phil_packet_p2c_abs_move__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_ABS_MOVE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		usCount = 0;
		memset(stMove, 0, sizeof(STG_PP_MOVE) * _countof(stMove));
	}

	unsigned short	usCount;						// Move Axis 개수		
	STG_PP_MOVE		stMove[DEF_MAX_MOVE_COUNT];     // 최대 5개로 제한	

}	STG_PP_P2C_ABS_MOVE, * LPG_PP_P2C_ABS_MOVE;

typedef struct __st_phil_packet_p2c_abs_move_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_ABS_MOVE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_P2C_ABS_MOVE_ACK, * LPG_PP_P2C_ABS_MOVE_ACK;

typedef struct __st_phil_packet_p2c_abs_move_complete__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_ABS_MOVE_COMPLETE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_P2C_ABS_MOVE_COMP, * LPG_PP_P2C_ABS_MOVE_COMP;


typedef struct __st_phil_packet_p2c_abs_move_complete_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_ABS_MOVE_COMPLETE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_P2C_ABS_MOVE_COMP_ACK, * LPG_PP_P2C_ABS_MOVE_COMP_ACK;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// REL MOVE
typedef struct __st_phil_packet_c2p_rel_move__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_REL_MOVE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		usCount = 0;
		memset(stMove, 0, sizeof(STG_PP_MOVE) * _countof(stMove));
	}

	unsigned short	usCount;						// Move Axis 개수		
	STG_PP_MOVE		stMove[DEF_MAX_MOVE_COUNT];     // 최대 5개로 제한	

}	STG_PP_C2P_REL_MOVE, * LPG_PP_C2P_REL_MOVE;

typedef struct __st_phil_packet_c2p_rel_move_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_REL_MOVE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_C2P_REL_MOVE_ACK, * LPG_PP_C2P_REL_MOVE_ACK;

typedef struct __st_phil_packet_c2p_rel_move_complete__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_REL_MOVE_COMPLETE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_C2P_REL_MOVE_COMP, * LPG_PP_C2P_REL_MOVE_COMP;


typedef struct __st_phil_packet_c2p_rel_move_complete_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_REL_MOVE_COMPLETE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_C2P_REL_MOVE_COMP_ACK, * LPG_PP_C2P_REL_MOVE_COMP_ACK;

typedef struct __st_phil_packet_p2c_rel_move__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_REL_MOVE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		usCount = 0;
		memset(stMove, 0, sizeof(STG_PP_MOVE) * _countof(stMove));
	}

	unsigned short	usCount;						// Move Axis 개수		
	STG_PP_MOVE		stMove[DEF_MAX_MOVE_COUNT];     // 최대 5개로 제한

}	STG_PP_P2C_REL_MOVE, * LPG_PP_P2C_REL_MOVE;

typedef struct __st_phil_packet_p2c_rel_move_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_REL_MOVE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_P2C_REL_MOVE_ACK, * LPG_PP_P2C_REL_MOVE_ACK;

typedef struct __st_phil_packet_p2c_rel_move_complete__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_REL_MOVE_COMPLETE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_P2C_REL_MOVE_COMP, * LPG_PP_P2C_REL_MOVE_COMP;


typedef struct __st_phil_packet_p2c_rel_move_complete_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_REL_MOVE_COMPLETE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_P2C_REL_MOVE_COMP_ACK, * LPG_PP_P2C_REL_MOVE_COMP_ACK;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// CHAR_MOVE
typedef struct __st_phil_packet_c2p_char_move__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_CHAR_MOVE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		usCount = 0;
		memset(stMoveTeach, 0, sizeof(STG_PP_MOVE_TEACH) * _countof(stMoveTeach));
	}

	unsigned short		usCount;							// Move Axis 개수		
	STG_PP_MOVE_TEACH	stMoveTeach[DEF_MAX_MOVE_COUNT];    // 최대 5개로 제한

}	STG_PP_C2P_CHAR_MOVE, * LPG_PP_C2P_CHAR_MOVE;

typedef struct __st_phil_packet_c2p_char_move_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_CHAR_MOVE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_C2P_CHAR_MOVE_ACK, * LPG_PP_C2P_CHAR_MOVE_ACK;

typedef struct __st_phil_packet_c2p_char_move_complete__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_CHAR_MOVE_COMPLETE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_C2P_CHAR_MOVE_COMP, * LPG_PP_C2P_CHAR_MOVE_COMP;


typedef struct __st_phil_packet_c2p_char_move_complete_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_CHAR_MOVE_COMPLETE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_C2P_CHAR_MOVE_COMP_ACK, * LPG_PP_C2P_CHAR_MOVE_COMP_ACK;

typedef struct __st_phil_packet_p2c_char_move__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_CHAR_MOVE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		usCount = 0;
		memset(stMoveTeach, 0, sizeof(STG_PP_MOVE_TEACH) * _countof(stMoveTeach));
	}

	unsigned short		usCount;							// Move Axis 개수		
	STG_PP_MOVE_TEACH	stMoveTeach[DEF_MAX_MOVE_COUNT];    // 최대 5개로 제한

}	STG_PP_P2C_CHAR_MOVE, * LPG_PP_P2C_CHAR_MOVE;

typedef struct __st_phil_packet_p2c_char_move_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_CHAR_MOVE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_P2C_CHAR_MOVE_ACK, * LPG_PP_P2C_CHAR_MOVE_ACK;

typedef struct __st_phil_packet_p2c_char_move_complete__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_CHAR_MOVE_COMPLETE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_P2C_CHAR_MOVE_COMP, * LPG_PP_P2C_CHAR_MOVE_COMP;


typedef struct __st_phil_packet_p2c_char_move_complete_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_CHAR_MOVE_COMPLETE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_P2C_CHAR_MOVE_COMP_ACK, * LPG_PP_P2C_CHAR_MOVE_COMP_ACK;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// C2P PROCESS EXECUTE
typedef struct __st_phil_packet_c2p_process_execute__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_PROCESS_EXECUTE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(szRecipeName, 0, sizeof(szRecipeName));
		memset(szGlassID, 0, sizeof(szGlassID));
	}

	char szRecipeName[DEF_MAX_RECIPE_NAME_LENGTH];      // Recipe 명은 최대 40자		
	char szGlassID[DEF_MAX_GLASS_NAME_LENGTH];         // Glass ID는 최대 40자		

}	STG_PP_C2P_PROCESS_EXECUTE, * LPG_PP_C2P_PROCESS_EXECUTE;

typedef struct __st_phil_packet_c2p_process_execute_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_PROCESS_EXECUTE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(szRecipeName, 0, sizeof(szRecipeName));
		memset(szGlassID, 0, sizeof(szGlassID));
	}

	char szRecipeName[DEF_MAX_RECIPE_NAME_LENGTH];      // Recipe 명은 최대 40자		
	char szGlassID[DEF_MAX_GLASS_NAME_LENGTH];			// Glass ID는 최대 40자		

}	STG_PP_C2P_PROCESS_EXECUTE_ACK, * LPG_PP_C2P_PROCESS_EXECUTE_ACK;

typedef struct __st_phil_packet_p2c_process_complete__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_PROCESS_COMPLETE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(szRecipeName, 0, sizeof(szRecipeName));
		memset(szGlassID, 0, sizeof(szGlassID));
		usCount = 0;
		usProgress = 0;
		memset(stVar, 0, sizeof(STG_PP_STATE) * DEF_MAX_PROCESS_PARAM_COUNT);
	}

	char			szRecipeName[DEF_MAX_RECIPE_NAME_LENGTH];					// Recipe 명은 최대 40자		
	char			szGlassID[DEF_MAX_GLASS_NAME_LENGTH];						// Glass ID는 최대 40자	
	unsigned short usProgress;              // Glass 진행사항 0:None, 1:Done, 2:NG		
	unsigned short	usCount;													// DV Parameter 개수		
	STG_PP_STATE		stVar[DEF_MAX_PROCESS_PARAM_COUNT];


}	STG_PP_P2C_PROCESS_COMP, * LPG_PP_P2C_PROCESS_COMP;


typedef struct __st_phil_packet_p2c_process_complete_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_PROCESS_COMPLETE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(szRecipeName, 0, sizeof(szRecipeName));
		memset(szGlassID, 0, sizeof(szGlassID));
	}

	char szRecipeName[DEF_MAX_RECIPE_NAME_LENGTH];      // Recipe 명은 최대 40자		
	char szGlassID[DEF_MAX_GLASS_NAME_LENGTH];			// Glass ID는 최대 40자		

}	STG_PP_P2C_PROCESS_COMP_ACK, * LPG_PP_P2C_PROCESS_COMP_ACK;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// C2P SUBPROCESS EXECUTE
typedef struct __st_phil_packet_c2p_subprocess_execute__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_SUB_PROCESS_EXECUTE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(szAckSubProcessName, 0, sizeof(szAckSubProcessName));
	}

	char szAckSubProcessName[DEF_MAX_SUBPROCESS_NAME_LENGTH];   // Sub Process 명은 최대 20자		

}	STG_PP_C2P_SUBPROCESS_EXECUTE, * LPG_PP_C2P_SUBPROCESS_EXECUTE;

typedef struct __st_phil_packet_c2p_subprocess_execute_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_SUB_PROCESS_EXECUTE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(szAckSubProcessName, 0, sizeof(szAckSubProcessName));
	}

	char szAckSubProcessName[DEF_MAX_SUBPROCESS_NAME_LENGTH];   // Sub Process 명은 최대 20자	

}	STG_PP_C2P_SUBPROCESS_EXECUTE_ACK, * LPG_PP_C2P_SUBPROCESS_EXECUTE_ACK;

typedef struct __st_phil_packet_p2c_subprocess_complete__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_SUB_PROCESS_COMPLETE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(szSubProcessName, 0, sizeof(szSubProcessName));
	}

	char szSubProcessName[DEF_MAX_SUBPROCESS_NAME_LENGTH];   // Sub Process 명은 최대 20자	

}	STG_PP_P2C_SUBPROCESS_COMP, * LPG_PP_P2C_SUBPROCESS_COMP;


typedef struct __st_phil_packet_p2c_subprocess_complete_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_SUB_PROCESS_COMPLETE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(szAckSubProcessName, 0, sizeof(szAckSubProcessName));
	}

	char szAckSubProcessName[DEF_MAX_SUBPROCESS_NAME_LENGTH];   // Sub Process 명은 최대 20자	

}	STG_PP_P2C_SUBPROCESS_COMP_ACK, * LPG_PP_P2C_SUBPROCESS_COMP_ACK;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// P2C PROCESS STEP
typedef struct __st_phil_packet_p2c_process_step__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_PROCESS_STEP;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(szGlassName, 0, sizeof(szGlassName));
		memset(szProcessStepName, 0, sizeof(szProcessStepName));
	}

	char szGlassName[DEF_MAX_GLASS_NAME_LENGTH];
	char szProcessStepName[DEF_MAX_SUBPROCESS_NAME_LENGTH];   // Sub Process 명은 최대 20자		

}	STG_PP_P2C_PROCESS_STEP, * LPG_PP_P2C_PROCESS_STEP;

typedef struct __st_phil_packet_p2c_process_step_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_PROCESS_STEP;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(szProcessStepName, 0, sizeof(szProcessStepName));
	}

	char szProcessStepName[DEF_MAX_SUBPROCESS_NAME_LENGTH];   // Sub Process 명은 최대 20자	

}	STG_PP_P2C_PROCESS_STEP_ACK, * LPG_PP_P2C_PROCESS_STEP_ACK;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// MODE_CHANGE
typedef struct __st_phil_packet_c2p_mode_change__ : public __st_phil_packet_header__
{
	enum EN_MODE
	{
		eMODE_INIT = 1,
		eMODE_MANUAL,
		eMODE_AUTO,
	};

	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_MODE_CHANGE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		usMode = eMODE_INIT;
	}

	unsigned short usMode;      // 1:Initial 2:Manual 3:Auto 4:PM		

}	STG_PP_C2P_MODE_CHANGE, * LPG_PP_C2P_MODE_CHANGE;

typedef struct __st_phil_packet_c2p_mode_change_ack__ : public __st_phil_packet_header__
{
	enum EN_MODE
	{
		eMODE_INIT = 1,
		eMODE_MANUAL,
		eMODE_AUTO,
	};

	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_MODE_CHANGE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		usMode = eMODE_INIT;
	}

	unsigned short usMode;      // 1:Initial 2:Manual 3:Auto 4:PM		

}	STG_PP_C2P_MODE_CHANGE_ACK, * LPG_PP_C2P_MODE_CHANGE_ACK;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// INITIAL_EXECUTE
typedef struct __st_phil_packet_c2p_initial_execute__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_INITIAL_EXECUTE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_C2P_INITIAL_EXECUTE, * LPG_PP_C2P_INITIAL_EXECUTE;

typedef struct __st_phil_packet_c2p_c2p_initial_execute_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_INITIAL_EXECUTE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_C2P_INITIAL_EXECUTE_ACK, * LPG_PP_C2P_INITIAL_EXECUTE_ACK;

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// EVENT_STATUS
typedef struct __st_phil_packet_c2p_event_status__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_EVENT_STATUS;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(szEventName, 0, sizeof(szEventName));
	}

	char szEventName[DEF_MAX_EVENT_NAME_LENGTH];             // Event 명은 최대 40자		

}	STG_PP_C2P_EVENT_STATUS, * LPG_PP_C2P_EVENT_STATUS;

typedef struct __st_phil_packet_c2p_event_status_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_EVENT_STATUS;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

	char szEventName[DEF_MAX_EVENT_NAME_LENGTH];             // Event 명은 최대 40자	
	bool bEventFlag;										 //

}	STG_PP_C2P_EVENT_STATUS_ACK, * LPG_PP_C2P_EVENT_STATUS_ACK;

typedef struct __st_phil_packet_p2c_event_status__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_EVENT_STATUS;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(szEventName, 0, sizeof(szEventName));
	}

	char szEventName[DEF_MAX_EVENT_NAME_LENGTH];             // Event 명은 최대 40자		

}	STG_PP_P2C_EVENT_STATUS, * LPG_PP_P2C_EVENT_STATUS;

typedef struct __st_phil_packet_p2c_event_status_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_EVENT_STATUS;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

	char szEventName[DEF_MAX_EVENT_NAME_LENGTH];             // Event 명은 최대 40자	
	bool bEventFlag;										 //

}	STG_PP_P2C_EVENT_STATUS_ACK, * LPG_PP_P2C_EVENT_STATUS_ACK;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// EVENT_NOTIFY
typedef struct __st_phil_packet_c2p_event_notify__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_EVENT_NOTIFY;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(szEventName, 0, sizeof(szEventName));
	}

	char szEventName[DEF_MAX_EVENT_NAME_LENGTH];             // Event 명은 최대 40자		
	bool bEventFlag;

}	STG_PP_C2P_EVENT_NOTIFY, * LPG_PP_C2P_EVENT_NOTIFY;

typedef struct __st_phil_packet_c2p_event_notify_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_EVENT_NOTIFY;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_C2P_EVENT_NOTIFY_ACK, * LPG_PP_C2P_EVENT_NOTIFY_ACK;

typedef struct __st_phil_packet_p2c_event_notify__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_EVENT_NOTIFY;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(szEventName, 0, sizeof(szEventName));
	}

	char szEventName[DEF_MAX_EVENT_NAME_LENGTH];             // Event 명은 최대 40자		
	bool bEventFlag;										 //

}	STG_PP_P2C_EVENT_NOTIFY, * LPG_PP_P2C_EVENT_NOTIFY;

typedef struct __st_phil_packet_p2c_event_notify_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_EVENT_NOTIFY;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_P2C_EVENT_NOTIFY_ACK, * LPG_PP_P2C_EVENT_NOTIFY_ACK;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// TIME_SYNC
typedef struct __st_phil_packet_c2p_time_sync__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_TIME_SYNC;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		memset(szSyncTime, 0, sizeof(szSyncTime));
	}

	char szSyncTime[DEF_TIME_LENGTH] = {0,};       // YYYYMMDDhhmmss		
											// ex)20230322142640 : 2023년3월22일14시26분40초		

}	STG_PP_C2P_TIME_SYNC, * LPG_PP_C2P_TIME_SYNC;

typedef struct __st_phil_packet_c2p_time_sync_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_TIME_SYNC;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_C2P_TIME_SYNC_ACK, * LPG_PP_C2P_TIME_SYNC_ACK;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// INTERRUPT_STOP
typedef struct __st_phil_packet_c2p_interrupt_stop__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_INTERRUPT_STOP;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_C2P_INTERRUPT_STOP, * LPG_PP_C2P_INTERRUPT_STOP;

typedef struct __st_phil_packet_c2p_interrupt_stop_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_INTERRUPT_STOP;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_C2P_INTERRUPT_STOP_ACK, * LPG_PP_C2P_INTERRUPT_STOP_ACK;

typedef struct __st_phil_packet_p2c_interrupt_stop__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_INTERRUPT_STOP;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_P2C_INTERRUPT_STOP, * LPG_PP_P2C_INTERRUPT_STOP;

typedef struct __st_phil_packet_p2c_interrupt_stop_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_INTERRUPT_STOP;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_P2C_INTERRUPT_STOP_ACK, * LPG_PP_P2C_INTERRUPT_STOP_ACK;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// STATUS_VALUE
typedef struct __st_phil_packet_c2p_status_value__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_STATUS_VALUE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}
}	STG_PP_C2P_STATUS_VALUE, * LPG_PP_C2P_STATUS_VALUE;

typedef struct __st_phil_packet_c2p_status_value_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_C2P_STATUS_VALUE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		usCount = 0;

		memset(stVar, 0, sizeof(STG_PP_STATE)* DEF_MAX_STATE_PARAM_VALUE_LENGTH);

	}

	unsigned short	usCount;																				// DV Parameter 개수		
	STG_PP_STATE		stVar[DEF_MAX_STATE_PARAM_VALUE_LENGTH];


}	STG_PP_C2P_STATUS_VALUE_ACK, * LPG_PP_C2P_STATUS_VALUE_ACK;

typedef struct __st_phil_packet_p2c_ec_modify__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_EC_MODIFY;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;

		usCount = 0;
		memset(stVar, 0, sizeof(STG_PP_STATE) * DEF_MAX_ECM_PARAM_COUNT);
	}

	unsigned short	usCount;																				// DV Parameter 개수		
	STG_PP_STATE		stVar[DEF_MAX_ECM_PARAM_COUNT];

}	STG_PP_P2C_EC_MODIFY, * LPG_PP_P2C_EC_MODIFY;

typedef struct __st_phil_packet_p2c_ec_modify_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_EC_MODIFY;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}
	
}	STG_PP_P2C_EC_MODIFY_ACK, * LPG_PP_P2C_EC_MODIFY_ACK;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// INITIAL_COMPLETE
typedef struct __st_phil_packet_p2c_initial_complete__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_INITIAL_COMPLETE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_P2C_INITIAL_COMPLETE, * LPG_PP_P2C_INITIAL_COMPLETE;

typedef struct __st_phil_packet_p2c_initial_complete_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_INITIAL_COMPLETE;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_P2C_INITIAL_COMPLETE_ACK, * LPG_PP_P2C_INITIAL_COMPLETE_ACK;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// ALARM_OCCUR
typedef struct __st_phil_packet_p2c_alarm_occur__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_ALARM_OCCUR;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_P2C_ALARM_OCCUR, * LPG_PP_P2C_ALARM_OCCUR;

typedef struct __st_phil_packet_p2c_alarm_occur_ack__ : public __st_phil_packet_header__
{
	void Reset()
	{
		nCommand = (int)ENG_PHPC::ePHILHMI_P2C_ALARM_OCCUR;
		ulDataLen = sizeof(*this) - sizeof(__st_phil_packet_header__);
		ulUniqueID = 0;
		usErrorCode = ePHILHMI_ERR_OK;
	}

}	STG_PP_P2C_ALARM_OCCUR_ACK, * LPG_PP_P2C_ALARM_OCCUR_ACK;
//////////////////////////////////////////////////////////////////////////

// PHILHMI INTERFACE PROTOCOL
// CONTROL TO PROCESS		PROCESS TO CONTROL
// ENUM	COMMAND	ENUM	COMMAND
// 101	C2P RECIPE LIST				201	P2C RECIPE CREATE
// 102	C2P RECIPE SELECTE			202	P2C RECIPE DELETE
// 103	C2P RECIPE MODIFY			203	P2C RECIPE MODIFY
// 313	C2P ABS MOVE				401	P2C IO STATUS
// 314	C2P ABS MOVE COMPLETE		402	P2C OUTPUT ONOFF
// 315	C2P REL MOVE				413	P2C ABS MOVE
// 316	C2P REL MOVE COMPLETE		414	P2C ABS MOVE COMPLETE
// 317	C2P CHAR MOVE				415	P2C REL MOVE
// 318	C2P CHAR MOVE COMPLETE		416	P2C REL MOVE COMPLETE
// 502	C2P PROCESS EXECUTE			417	P2C CHAR MOVE
// 503	C2P SUB PROCESS EXECUTE		418	P2C CHAR MOVE COMPLETE
// 701	C2P STATUS VALUE			601	P2C PROCESS STEP
// 702	C2P MODE CHANGE				602	P2C PROCESS COMPLETE
// 711	C2P EVENT NOTIFY			603	P2C SUB PROCESS COMPLETE
// 712	C2P TIME SYNC				801	P2C STATUS VALUE
// 991	C2P INTERRUPT STOP			802	P2C INITIAL COMPLETE
//									811	P2C EVENT NOTIFY
//									992	P2C INTERRUPT STOP

#define DEF_PACKET_MAX_SIZE 20000

typedef union 
{
	STG_PPH								st_header;

	STG_PP_P2C_RCP_CREATE				st_p2c_rcp_create;
	STG_PP_P2C_RCP_DELETE				st_p2c_rcp_delete;
	STG_PP_P2C_RCP_MODIFY				st_p2c_rcp_modify;
	STG_PP_P2C_RCP_SELECT				st_p2c_rcp_select;
	STG_PP_P2C_IO_STATUS				st_p2c_io_status;
	STG_PP_P2C_OUTPUT_ONOFF				st_p2c_output_onoff;
	STG_PP_P2C_ABS_MOVE					st_p2c_abs_move;
	STG_PP_P2C_ABS_MOVE_COMP			st_p2c_abs_move_comp;
	STG_PP_P2C_REL_MOVE					st_p2c_rel_move;
	STG_PP_P2C_REL_MOVE_COMP			st_p2c_rel_move_comp;
	STG_PP_P2C_CHAR_MOVE				st_p2c_char_move;
	STG_PP_P2C_CHAR_MOVE_COMP			st_p2c_char_move_comp;
	STG_PP_P2C_PROCESS_STEP				st_p2c_process_step;
	STG_PP_P2C_PROCESS_COMP				st_p2c_process_comp;
	STG_PP_P2C_SUBPROCESS_COMP			st_p2c_subprocess_comp;
	STG_PP_P2C_EC_MODIFY				st_p2c_ec_modify;
	STG_PP_P2C_INITIAL_COMPLETE			st_p2c_init_comp;
	STG_PP_P2C_ALARM_OCCUR				st_p2c_alarm_occur;
	STG_PP_P2C_EVENT_NOTIFY				st_p2c_event_notify;
	STG_PP_P2C_INTERRUPT_STOP			st_p2c_interrupt_stop;

	STG_PP_C2P_RCP_CREATE_ACK			st_c2p_ack_rcp_create;
	STG_PP_C2P_RCP_DELETE_ACK			st_c2p_ack_rcp_delete;
	STG_PP_C2P_RCP_MODIFY_ACK			st_c2p_ack_rcp_modify;
	STG_PP_C2P_RCP_SELECT_ACK			st_c2p_ack_rcp_select;
	STG_PP_C2P_RCP_INFO_ACK				st_c2p_ack_rcp_info;
	STG_PP_C2P_RCP_LIST_ACK				st_c2p_ack_rcp_list;
	STG_PP_C2P_ABS_MOVE_ACK				st_c2p_ack_abs_move;
	STG_PP_C2P_ABS_MOVE_COMP_ACK		st_c2p_ack_abs_move_comp;
	STG_PP_C2P_REL_MOVE_ACK				st_c2p_ack_rel_move;
	STG_PP_C2P_REL_MOVE_COMP_ACK		st_c2p_ack_rel_move_comp;
	STG_PP_C2P_CHAR_MOVE_ACK			st_c2p_ack_char_move;
	STG_PP_C2P_CHAR_MOVE_COMP_ACK		st_c2p_ack_char_move_comp;
	STG_PP_C2P_PROCESS_EXECUTE_ACK		st_c2p_ack_process_execute;
	STG_PP_C2P_SUBPROCESS_EXECUTE_ACK	st_c2p_ack_subprocess_execute;
	STG_PP_C2P_STATUS_VALUE_ACK			st_c2p_ack_status_value;
	STG_PP_C2P_MODE_CHANGE_ACK			st_c2p_ack_mode_change;
	STG_PP_C2P_INITIAL_EXECUTE			st_c2p_ack_initial_execute;
	STG_PP_C2P_EVENT_NOTIFY_ACK			st_c2p_ack_event_notify;
	STG_PP_C2P_TIME_SYNC_ACK			st_c2p_ack_time_sync;
	STG_PP_C2P_INTERRUPT_STOP_ACK		st_c2p_ack_interrupt_stop;
	BYTE								stData[DEF_PACKET_MAX_SIZE];

}STG_PP_PACKET_SEND;

typedef union
{
	STG_PPH								st_header;

	STG_PP_P2C_RCP_CREATE_ACK			st_p2c_ack_rcp_create;
	STG_PP_P2C_RCP_DELETE_ACK			st_p2c_ack_rcp_delete;
	STG_PP_P2C_RCP_MODIFY_ACK			st_p2c_ack_rcp_modify;
	STG_PP_P2C_RCP_SELECT_ACK			st_p2c_ack_rcp_select;
	STG_PP_P2C_IO_STATUS_ACK			st_p2c_ack_io_status;
	STG_PP_P2C_OUTPUT_ONOFF_ACK			st_p2c_ack_output_onoff;
	STG_PP_P2C_ABS_MOVE_ACK				st_p2c_ack_abs_move;
	STG_PP_P2C_ABS_MOVE_COMP_ACK		st_p2c_ack_abs_move_comp;
	STG_PP_P2C_REL_MOVE_ACK				st_p2c_ack_rel_move;
	STG_PP_P2C_REL_MOVE_COMP_ACK		st_p2c_ack_rel_move_comp;
	STG_PP_P2C_CHAR_MOVE_ACK			st_p2c_ack_char_move;
	STG_PP_P2C_CHAR_MOVE_COMP_ACK		st_p2c_ack_char_move_comp;
	STG_PP_P2C_PROCESS_STEP_ACK			st_p2c_ack_process_step;
	STG_PP_P2C_PROCESS_COMP_ACK			st_p2c_ack_process_comp;
	STG_PP_P2C_SUBPROCESS_COMP_ACK		st_p2c_ack_subprocess_comp;
	STG_PP_P2C_EC_MODIFY_ACK			st_p2c_ack_ec_modify;
	STG_PP_P2C_INITIAL_COMPLETE_ACK		st_p2c_ack_init_comp;
	STG_PP_P2C_ALARM_OCCUR_ACK			st_p2c_ack_alarm_occur;
	STG_PP_P2C_EVENT_NOTIFY_ACK			st_p2c_ack_event_notify;
	STG_PP_P2C_INTERRUPT_STOP_ACK		st_p2c_ack_interrupt_stop;

	STG_PP_C2P_RCP_CREATE				st_c2p_rcp_create;
	STG_PP_C2P_RCP_DELETE				st_c2p_rcp_delete;
	STG_PP_C2P_RCP_MODIFY				st_c2p_rcp_modify;
	STG_PP_C2P_RCP_SELECT				st_c2p_rcp_select;
	STG_PP_C2P_RCP_INFO					st_c2p_rcp_info;
	STG_PP_C2P_RCP_LIST					st_c2p_rcp_list;
	STG_PP_C2P_ABS_MOVE					st_c2p_abs_move;
	STG_PP_C2P_ABS_MOVE_COMP			st_c2p_abs_move_comp;
	STG_PP_C2P_REL_MOVE					st_c2p_rel_move;
	STG_PP_C2P_REL_MOVE_COMP			st_c2p_rel_move_comp;
	STG_PP_C2P_CHAR_MOVE				st_c2p_char_move;
	STG_PP_C2P_CHAR_MOVE_COMP			st_c2p_char_move_comp;
	STG_PP_C2P_INITIAL_EXECUTE			st_c2p_initial_execute;
	STG_PP_C2P_PROCESS_EXECUTE			st_c2p_process_execute;
	STG_PP_C2P_SUBPROCESS_EXECUTE		st_c2p_subprocess_execute;
	STG_PP_C2P_STATUS_VALUE				st_c2p_status_value;
	STG_PP_C2P_MODE_CHANGE				st_c2p_mode_change;
	STG_PP_C2P_EVENT_STATUS				st_c2p_event_status;
	STG_PP_C2P_EVENT_NOTIFY				st_c2p_event_notify;
	STG_PP_C2P_TIME_SYNC				st_c2p_time_sync;
	STG_PP_C2P_INTERRUPT_STOP			st_c2p_interrupt_stop;
	BYTE								stData[DEF_PACKET_MAX_SIZE];
}STG_PP_PACKET_RECV;

typedef struct __st_phil_packet_p2c__
{
	STG_PP_PACKET_SEND pkt;
}	STG_PP_PHILHMI, * LPG_PP_PHILHMI;

typedef struct __st_philhmi_packet_register__
{
	/* Command */
	ENG_TPCS			command;					/* Read/Write : Low 1 Byte, ENG_TBPC <01 : Write, 02 : Read, 07 : IP Write> */
	
	STG_PP_PACKET_RECV	pkt = { 0 };

	/*
	 desc : 기존 데이터 모두 제거
	 parm : None
	 retn : None
	*/
	VOID ResetPktData()
	{
		command = ENG_TPCS::en_closed;
		memset(&pkt, 0x00, sizeof(pkt));
	}

}	STG_PPR, * LPG_PPR;

#pragma pack (pop)	/* 1 bytes order */