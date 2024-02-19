
/*
 desc : EFEM (Robot) Information
*/

#pragma once

#include "conf_comn.h"

/* --------------------------------------------------------------------------------------------- */
/*                                            상수 값                                            */
/* --------------------------------------------------------------------------------------------- */

#define MAX_LOT_COUNT	32
#define	MAX_ERR_RECENT	25

/* --------------------------------------------------------------------------------------------- */
/*                                      전역(Global) 열거형                                      */
/* --------------------------------------------------------------------------------------------- */

/* FOUP의 Lot (Wafer) Action */
typedef enum class __en_lot_action_normal_invert__ : UINT8
{
	en_normal						= 0x00,		/* Lot (Wafer) 정상 */
	en_invert						= 0x01,		/* Lot (Wafer) 반전 */

}	ENG_LANI;

/* FOUP Number */
typedef enum class __en_efem_foup_id_number__ : UINT8
{
	en_none							= 0x00,
	en_1							= 0x01,		/* FOUP Number 1*/
	en_2							= 0x02,		/* FOUP Number 1*/

}	ENG_EFIN;

/* Lot (Wafer)가 놓여진 장비(위치) 정보 */
typedef enum class __en_efem_lot_place_device__ : UINT8
{
	en_none							= 0x00,
	en_di_loader					= 0x01,		/* DI Loader / Unloader */
	en_free_aligner					= 0x02,		/* EFEM의 Free Aligner */
	en_lot_buffer					= 0x03,		/* Lot (Wafer)가 임시로 놓여진 버퍼 */
	en_efem_head					= 0x04,		/* EFEM의 Head가 Clip한 곳 */

}	ENG_ELPD;

/* FOUP Change 정보 */
typedef enum class __en_efem_foup_attached_detached__ : UINT8
{
	en_none							= 0x00,
	en_foup_attached				= 0x01,		/* FOUP Attached */
	en_foup_detached				= 0x02,		/* FOUP Detached */

}	ENG_EFAD;

/* EFEM State */
typedef enum class __en_efem_current_state_value__ : UINT8
{
	en_ready						= 0x00,		/* 대기 상태 */
	en_busy							= 0x01,		/* 현재 Busy 상태 */
	en_error						= 0x02,		/* 에러가 발생한 상태 */
	en_none							= 0xff,

}	ENG_ECSV;

/* EFEM 내부에서 Lot (Wafer)이 놓여지는 곳의 구분 값. 즉, ID   */
/* 만약 header 와 aligner, buffer에 Wafer가 놓여 있다면 : 0x07 */
/* Lot (Wafer)가 가장 마지막에 적재된 위치 (ENG_EWLI) Foup_new:0, Head:1, Aligner:2, Buffer:4, Loader:8, Foup_old:16 */	
typedef enum class __en_efem_wafer_load_id__ : UINT8
{
	en_wafer_in_foup_new			= 0x00,	/* 최초 FOUP에 들어가 있는 상태 */
	en_wafer_in_header				= 0x01,	/* Header가 물고(?)있는 상태 */
	en_wafer_in_aligner				= 0x02,	/* Pre-aligner에서 Align 중인 상태 */
	en_wafer_in_buffer				= 0x04,	/* Buffer에서 대기 중인 상태 */
	en_wafer_in_loader				= 0x08,	/* 노광 중인 상태 */
	en_wafer_in_foup_old			= 0x10,	/* 노광 이후 FOUP에 들어가 있는 상태 */

}	ENG_EWLI;

/* FOUP Event Code */
typedef enum class __en_efem_foup_event_code__ : UINT8
{
	en_none							= 0x00,

	en_cmps_from_foup_to_aligner	= 0x01,
	en_efem_from_foup_to_aligner	= 0x10,

	en_cmps_from_buffer_to_foup		= 0x02,
	en_efem_from_buffer_to_foup		= 0x20,

	en_cmps_open_shutter			= 0x03,
	en_efem_load_completed			= 0x30,

	en_cmps_complete_to_expose		= 0x04,
	en_efem_load_on_buffer			= 0x40,

}	ENG_EFEC;

/* CMPS <--> EFEM 통신 패킷 명령 */
typedef enum class __en_efem_cmps_packet_command__ : UINT16
{
	en_req_none						= 0x1100,
	en_res_none						= 0x1200,

	en_req_stop						= 0x1101,		/* CMPS Client - Req - EFEM Stop */
	en_res_stop						= 0x1201,		/* CMPS Server - Res - EFEM Stop */

	en_req_efem_state				= 0x1102,		/* CMPS Client - Req - EFEM State */
	en_res_efem_state				= 0x1202,		/* EFEM Server - Res - EFEM State */

	en_req_efem_error				= 0x1103,		/* CMPS Client - Req - EFEM Device Error code */
	en_res_efem_error				= 0x1203,		/* EFEM Server - Res - EFEM Device Error code */

	en_req_efem_reset				= 0x1104,		/* CMPS Client - Req - EFEM Device Reset */
	en_res_efem_reset				= 0x1204,		/* EFEM Server - Res - EFEM Device Reset */

	en_req_efem_event				= 0x1105,		/* CMPS Client - Req - EFEM Work Event */
	en_res_efem_event				= 0x1205,		/* EFEM Server - Res - EFEM Work Event */

	en_req_elot_moving				= 0x1107,		/* CMPS Client - Req - Error Lot (Wafer) Moving */
	en_res_elot_moving				= 0x1207,		/* EFEM Server - Res - Error Lot (Wafer) Moving */

	en_req_foup_change				= 0x1109,		/* CMPS Client - Req - Wafer FOUP Lots Change */
	en_res_foup_change				= 0x1209,		/* EFEM Server - Res - Wafer FOUP Lots Change */

	en_req_lot_list					= 0x110a,		/* CMPS Client - Req - Lot (Water) list */
	en_res_lot_list					= 0x120a,		/* EFEM Server - Res - Lot (Water) list */

	en_req_foup_loaded				= 0x110b,		/* CMPS Client - Req - Foup loaded */
	en_res_foup_loaded				= 0x120b,		/* EFEM Server - Res - Foup loaded */

}	ENG_ECPC;

/* --------------------------------------------------------------------------------------------- */
/*                            전역(Global) 구조체  - Protocol Packet                             */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 1)

/* 패킷 송수신 구조체 */
typedef struct __st_packet_efem_send_recv__
{
	UINT16				size;		/* data 버퍼에 저장된 데이터의 크기 */
	UINT8				data[64];	/* head + body + tail */

}	STG_PESR,	*LPG_PESR;

#pragma pack (pop)	/* 1 bytes order */

#pragma pack (push, 8)

/* processing lot (wafer) recipe data */
typedef struct __st_proc_lot_recipe_data__
{
	/* EFEM Info. */
	UINT8				get;						/* 0 : Normal,	1 : Inverted */
	UINT8				put;						/* 0 : Normal,	1 : Inverted */
	UINT8				wafer;						/* Lot (Wafer)가 가장 마지막에 적재된 위치 (ENG_EWLI) Foup (New):00, Head:01, Aligner:02, Buffer:04, Loader:08, Foup (Old) : 10 (ENG_EWLI) */
	UINT8				index;						/* 노광 작업 순번 (Zero based) (0 ~ max 24) */
	UINT8				foup;						/* FOUP Number (1 or 2) */
	UINT8				lot;						/* Lot Number (1 ~ 25), 0 값이면 노광되지 않는 Wafer임 */
	UINT8				load;						/*  ENG_EGLS */
	UINT8				u8_reserved[1];
	/* LED Power Detail Info. */
	UINT16				led_365;					/* LED Power Index : 365 */
	UINT16				led_385;					/* LED Power Index : 380 */
	UINT16				led_390;					/* LED Power Index : 395 */
	UINT16				led_405;					/* LED Power Index : 405 */
	DOUBLE				f_rate;						/* Frame Rate (0.1 ~ 0.9) */
	DOUBLE				energy;						/* Exposure Energy */
	/* Gerber Info. */
	CHAR				gerber[MAX_GERBER_NAME];	/* Gerber Name (Path included) */

}	STG_PLRD,	*LPG_PLRD;

#pragma pack (pop)	/* 8 bytes order */

/* --------------------------------------------------------------------------------------------- */
/*                                     Shared Memory 구조체                                      */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 1)


#pragma pack (pop)	/* 1 bytes order */

#pragma pack (push, 8)

/* Lot (Wafer) 상태 정보 */
typedef struct __st_lot_work_detail_state__
{
	UINT8				loaded;		/* Lot 적재 여부. 0x00 - empty, 0x01 - exist */
	UINT8				expose;		/* Job Work state. 0x00 - New (노광 전), 0x01 - Old (노광 후) */
	UINT8				error;		/* Lot (Wafer) 작업할 때, 에러 발생 여부 (0x00 - None, 0x01 - Error) */
	UINT8				reserved[5];

}	STG_LWDS,	*LPG_LWDS;

/* EFEM으로부터 수신된 에러 정보. (에러 코드 및 발생된 시간) */
typedef struct __st_error_and_time_data__
{
	UINT32				error	:16;			/* 0x0000 ~ 0xffff */
	UINT32				u32_reserved:16;

	UINT32				year	: 6;			/* 0 ~ 31 (2000 based) */
	UINT32				month	: 4;			/* 1 ~ 12 */
	UINT32				day		: 5;			/* 1 ~ 31 */
	UINT32				hour	: 5;			/* 0 ~ 24 */
	UINT32				minute	: 6;			/* 0 ~ 59 */
	UINT32				second	: 6;			/* 0 ~ 59 */

}	STG_EATD,	*LPG_EATD;

/* EFEM (Robot) Shared Memory - All */
typedef struct __st_efem_data_shared_memory__
{
	UINT8				foup_1_load	: 1;		/* FOUP 1번 장착 여부. 0 - 탈거, 1 - 장착 */
	UINT8				foup_2_load	: 1;		/* FOUP 2번 장착 여부. 0 - 탈거, 1 - 장착 */
	UINT8				efem_state	: 2;		/* EFEM Device State (0x00 : Ready, 0x01 : Busy, 0x02 : Error) (ENG_ECSV) */
	UINT8				efem_wafer	: 4;		/* EFEM Lot 존재 여부(0x00 : Empty, 0x01 : wafer in head, 0x02 : wafer in aligner, 0x04 : wafer in buffer) */

	UINT8				err_front;				/* 다음 발생된 에러 정보가 저장될 메모리 위치 */
	UINT8				err_rear;				/* 현재 저장된 에러 정보를 반환할 메모리 위치 */

	UINT8				cmps_event;				/* Current CMPS Event Code */
	UINT8				efem_event;				/* Current EFEM Event Code */

	UINT8				efem_rate;				/* Reset 이후 진행률 값 (에러 발생 후 리셋 명령어 전달 후, 복구될 때까지 진행되는 Percent 값) */
	UINT8				efem_reset;				/* EFEM (Robot)으로 부터 Reset 송신 (처리)에 대한 응답이 수신되었는지 여부 */

	UINT8				foup_lots;				/* FOUP 마다 등록 가능한 최대 Lot (Wafer) 개수 */

	UINT16				error_last;				/* 가장 최근에 수신된 에러 코드 값 */
	UINT16				u16_reserved[3];

	UINT64				state_time;				/* 상태 정보 수신된 시간 저장 */

	STG_EATD			error_code[MAX_ERR_RECENT];
	STG_LWDS			foup_1_list[MAX_LOT_COUNT];
	STG_LWDS			foup_2_list[MAX_LOT_COUNT];

	STG_DLSM			link;					/* 연결 상태 정보 */

	/*
	 desc : 멤버 변수 초기화 (반드시 호출해 줘야 됨)
	 parm : None
	 retn : None
	*/
	VOID Init()
	{
		err_front	= 0x00;
		err_rear	= 0x00;
		cmps_event	= 0x00;
		efem_event	= 0x00;
	}

	/*
	 desc : EFEM으로 부터 Reset 요청에 대한 응답이 정상적으로 수신되었는지 여부
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsEFEMReset()
	{
		return efem_reset ? TRUE : FALSE;
	}

	/*
	 desc : 환형 큐 버퍼 FULL인지 확인
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsQueFull()
	{
		return (((err_rear + 1) % MAX_ERR_RECENT) == err_front) ? TRUE : FALSE;
	}

	/*
	 desc : 환형 큐 버퍼 Empty인지 확인
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsQueEmpty()
	{
		return (err_front == err_rear) ? TRUE : FALSE;
	}

	/*
	 desc : 큐에 새로운 데이터를 추가하고자 하는 공간 (인덱스) 반환
			중요!!! - 이 함수 호출되면, Rear 값이 증가됨
	 parm : None
	 retn : 새로 추가 (저장)하고자 하는 배열의 인덱스 값
	*/
	UINT32 PushRearIndex()
	{
		return (err_rear = (err_rear + 1) % MAX_ERR_RECENT);
	}

	/*
	 desc : 큐에 기존에 데이터를 가져오고자 하는 공간 (인덱스) 반환
			중요!!! - 이 함수 호출되면, Front 값이 증가됨
	 parm : None
	 retn : 기존 가져(제거)하고자 하는 배열의 인덱스 값
	*/
	UINT32 PopFrontIndex()
	{
		return (err_front = (err_front + 1) % MAX_ERR_RECENT);
	}

	/*
	 desc : Lot 개수 반환
	 parm : FOUP Index (1 or 2)
	 retn : 개수
	*/
	UINT8 GetLotCount(UINT8 foup)
	{
		UINT8 i	= 0, u8Count = 0;;
		LPG_LWDS pList	= foup_1_list;
		if (2 == foup)	pList	= foup_2_list;

		for (; i<foup_lots; i++)	u8Count	+= pList[i].loaded;

		return u8Count;
	}

	/*
	 desc : Reset Event Sync
	 parm : None
	 retn : None
	*/
	VOID ResetEventSync()
	{
		cmps_event	= 0x00;
		efem_event	= 0x00;
	}

	/*
	 desc : 에러 초기화
	 parm : None
	 retn : None
	*/
	VOID ResetError()
	{
		err_front	= 0x00;
		err_rear	= 0x00;
		error_last	= 0x0000;
		memset(error_code, 0x00, sizeof(STG_EATD) * MAX_ERR_RECENT);
	}

	/*
	 desc : 노광 초기화 (기존 노광된 정보 모두 초기화)
	 parm : None
	 retn : None
	*/
	VOID ResetExpose()
	{
		UINT8 i	= 0;

		for (; i<25; i++)
		{
			foup_1_list[i].expose	= 0;
			foup_2_list[i].expose	= 0;
		}
	}

	/*
	 desc : EFEM 상태 초기화
	 parm : None
	 retn : None
	*/
	VOID ResetState()
	{
		/* EFEM Device State (0x00 : Ready, 0x01 : Busy, 0x02 : Error) (ENG_ECSV)*/
		/* EFEM Lot 존재 여부(0x00 : Empty, 0x01 : wafer in head, 0x02 : wafer in aligner, 0x04 : wafer in buffer) */
		efem_state	= 0x01;	/* 현재 Busy 상태로 설정 */
		efem_wafer	= 0x07;	/* 모두 존재한다고 설정 */
	}

	/*
	 desc : FOUP Loaded 여부에 따라 List 초기화 여부
	 parm : None
	 retn : None
	*/
	VOID RefreshFoupList()
	{
		if (!foup_1_load)	memset(foup_1_list, 0x00, sizeof(STG_LWDS) * foup_lots);
		if (!foup_2_load)	memset(foup_2_list, 0x00, sizeof(STG_LWDS) * foup_lots);
	}

	/*
	 desc : 송신과 수신 이벤트가 서로 동기화 (궁합이)가 맞는지 여부 확인
	 parm : None
	 parm : TRUE - 맞다. FALSE - 틀리다. (이전 CMPS 이벤트와 현재 EFEM 이벤트가 서로 다르다)
	*/
	BOOL IsEventSync()
	{
		/* 현재 아무런 이벤트도 발생시키지 않은 경우 */
		if (0x00 == cmps_event && 0x00 == efem_event)	return FALSE;
		/* 이전에 명령 내린 CMPS 이벤트와 가장 최근 수신받은 EFEM 이벤트가 동일한 성격이지 확인 */
		return (cmps_event == (UINT8)(efem_event >> 4)) ? TRUE : FALSE;
	}

	/*
	 desc : State Time 갱신 (현재 CPU 시간)
	 parm : None
	 retn : None
	*/
	VOID UpdateStateTime()
	{
		state_time	= GetTickCount64();
	}

	/*
	 desc : EFEM State 상태가 주어진 시간 안에 갱신되었는지 확인
	 parm : time	- [in]  이 시간 이내에 갱신되었는지 확인 시간 (단위: msec)
	 retn : TRUE (주어진 시간보다 짧게 갱신되었다)
			FALSE (주어진 시간 이내에 갱신되지 않았다. 즉, 수신된 데이터가 아직 없다.)
	*/
	BOOL IsUpdateState(UINT64 time)
	{
		/* 현재 시간 대비, 최근에 갱신한 시간을 빼서, 주어진 시간보다 큰지 여부 */
		return (GetTickCount64() > (state_time + time)) ? FALSE : TRUE;
	}

	/*
	 desc : 전체 초기화
	 parm : None
	 retn : None
	*/
	VOID SetEFEMReset()
	{
		foup_1_load	= 0x00;
		foup_2_load	= 0x00;
		efem_state	= 0x00;
		cmps_event	= 0x00;
		efem_event	= 0x00;
		efem_rate	= 0x00;
		memset(foup_1_list, 0x00, sizeof(STG_LWDS) * foup_lots);
		memset(foup_2_list, 0x00, sizeof(STG_LWDS) * foup_lots);

		ResetError();
	}

	/*
	 desc : EFEM State 설정
	 parm : event	- [in]  현재 발생된 이벤트 정보가 저장된 구조체 포인터
			data[0]	- [in]  상태 값 설정  (Ready : 0x00, Busy : 0x01, Error : 0x02)
			data[1]	- [in]  Lot (Wafer) 존재 여부
							0x01 : wafer in RBT Head
							0x02 : wafer in RBT Aligner
							0x03 : wafer in RBT Buffer
			data[2] - [in]  에러 발생 후 리셋 명령어 전달 후, 복구될 때까지 진행되는 Percent 값
							100 값이면 모두 복구 완료 됨
	 retn : None
	*/
	VOID SetEFEMState(PUINT8 data)
	{
		if (data[0] >= 0 && data[0] < 0x04)
		{
			/* 상태 값 갱신 */
			efem_state	= data[0];
			efem_wafer	= data[1];
			efem_rate	= data[2];

			/* 현재 수신된 시간 갱신 */
			state_time	= GetTickCount64();
		}
	}

	/*
	 desc : EFEM & CMPS Event 설정
	 parm : code	- [in]  EFEM Event 값
	 retn : None
	*/
	VOID SetEFEMEvent(UINT8 code)	{	efem_event	= code;	}
	VOID SetCMPSEvent(UINT8 code)	{	cmps_event	= code;	}

	/*
	 desc : EFEM의 FOUP이 교체될 때마다 Changed 변경 처리
	 parm : data	- [in]  수신된 데이터
	 retn : None
	*/
	VOID SetFoupChanged(PUINT8 data)
	{
		if (data[0] == 1)	foup_1_load	= data[1];
		else				foup_2_load	= data[1];

		/* FOUP이 탈거 되었다면, 기존 적재된 Lot (Wafer) List 제거 */
		RefreshFoupList();
	}

	/*
	 desc : EFEM의 FOUP이 교체될 때마다 Loaded 변경 처리
	 parm : data	- [in]  수신된 데이터
	 retn : None
	*/
	VOID SetFoupLoaded(PUINT8 data)
	{
		if (data[0] < 0x02 && data[1] < 0x02)
		{
			foup_1_load	= data[0];
			foup_2_load	= data[1];

			/* FOUP이 탈거 되었다면, 기존 적재된 Lot (Wafer) List 제거 */
			RefreshFoupList();
		}
	}

	/*
	 desc : EFEM의 FOUP에 적재되어 있는 Lot (Wafer) 리스트 정보
	 parm : data	- [in]  수신된 데이터
	 retn : None
	*/
	VOID SetLotList(PUINT8 data)
	{
		UINT8 i			= 0x01;
		LPG_LWDS pList	= NULL;

		/* FOUP Number 선택*/
		if (0x01 == data[0])	pList = foup_1_list;
		else					pList = foup_2_list;

		/* FOUP에 최대 25개가 넘을 수 없기 때문이다. */
		for (; i<foup_lots+1; i++)
		{
			pList[i-1].loaded	= data[i];
			/* 장착 여부 이외의 값이 저장되어 있다면 에러 처리 */
			if (data[i] > 0x01)	data[i]	= 0x02;
		}
	}

	/*
	 desc : 수동으로 Lot (Wafer)의 위치를 변경한 경우, Lot의 위치 정보 변경
	 parm : data	- [in]  수신된 데이터
	 retn : None
	*/
	VOID SetLotErrMoving(PUINT8 data)
	{
		CHAR szValue[128]	= {NULL};
		LPG_LWDS pList		= NULL;

		if ((data[0] < 0x03) && (data[1] > 0x00 && data[1] < foup_lots+1 /* 26 */) && (data[2] <0x03))
		{
			/* FOUP 위치 설정 */
			pList	= data[0] == 1 ? foup_1_list : foup_2_list;
			/* 해당 FOUP에 Lot (Wafer)이 존재한다고 설정 */
			pList[data[1]-1].loaded	= 0x01;
		}
	}

	/*
	 desc : FOUP에 Lot (Wafer)가 적재 (Loaded)되어 있는지 여부
	 parm : foup	- [in]  FOUP Number (1 or 2)
			lot		- [in]  FOUP의 Lot (Wafer) 위치 (1 ~ 25)
	 retn : TRUE or FALSE
	*/
	BOOL IsLotLoaded(ENG_EFIN foup, UINT8 lot)
	{
		LPG_LWDS pList	= NULL;

		/* 조건이 유효한지 여부 */
		if ((foup != ENG_EFIN::en_1 && foup != ENG_EFIN::en_2) || lot < 1 || lot > foup_lots+1)
			return FALSE;

		/* FOUP이 탈거 되었는지 여부 */
		if (ENG_EFIN::en_1 == foup && 0 == foup_1_load)	return FALSE;
		if (ENG_EFIN::en_2 == foup && 0 == foup_2_load)	return FALSE;

		/* FOUP List 선택 */
		pList	= foup == ENG_EFIN::en_1 ? foup_1_list : foup_2_list;

		/* 적재 여부 */
		return pList[lot-1].loaded ? TRUE : FALSE;
	}

	/*
	 desc : FOUP이 적재 (Loaded)되어 있는지 여부
	 parm : foup	- [in]  FOUP Number (1 or 2)
	 retn : TRUE or FALSE
	*/
	BOOL IsFoupLoaded(ENG_EFIN foup)
	{
		if (foup == ENG_EFIN::en_1)	return foup_1_load ? TRUE : FALSE;
		if (foup == ENG_EFIN::en_2)	return foup_2_load ? TRUE : FALSE;

		return FALSE;
	}

	/*
	 desc : 현재 Lot (Wafer)이 임의 위치에 적재되어 있는지 여부
	 parm : find	- [in]  해당 위치에 Lot (Wafer)이 적재되어 있는지 확인하기 위한 검색어
	 retn : TRUE or FALSE
	*/
	BOOL IsLotLoaded(ENG_EWLI find)
	{
		return (UINT8(find) == (efem_wafer & UINT8(find)));
	}

	/*
	 desc : 에러 등록
	 parm : code	- [in]  발생된 에러 코드
	 retn : None
	*/
	VOID SetEFEMError(PUINT8 code)
	{
		UINT16 u16Code	= 0x0000;
		UINT32 u32Rear	= 0;
		STG_EATD stErr	= {NULL};
		SYSTEMTIME stTm	= {NULL};

		/* 더 이상 등록할 수 있는 공간이 없는지 확인 (IsQueueFull) */
		if (IsQueFull())
		{
			LOG_ERROR(L"There is not enough space in the queue. It is full.");
			return;
		}

#if 0
		/* 2 Bytes 버퍼에 복사 */
		memcpy(&u16Code, code, 2);
#else
		u16Code	= (UINT16)(*code);
#endif
		u16Code	= ntohs(u16Code);
		/* 현재 시간 */
		GetLocalTime(&stTm);
		/* 현재 등록될 위치 얻기 */
		u32Rear	= PushRearIndex();
		/* 가장 최근에 발생된 에러 코드 값 저장 */
		error_last	= u16Code;
		/* 가장 최근에 발생된 에러 정보 설정 */
		error_code[u32Rear].error	= u16Code;
		error_code[u32Rear].year	= UINT8(stTm.wYear - 2020);
		error_code[u32Rear].month	= UINT8(stTm.wMonth);
		error_code[u32Rear].day		= UINT8(stTm.wDay);
		error_code[u32Rear].hour	= UINT8(stTm.wHour);
		error_code[u32Rear].minute	= UINT8(stTm.wMinute);
		error_code[u32Rear].second	= UINT8(stTm.wSecond);
	}

	/*
	 desc : 가장 최근에 발생된 EFEM의 에러 코드 반환
	 parm : None
	 retn : 0x0000 or 에러 코드 반환 (0x0001 ~ 0xffff)
	*/
	UINT16 GetError()
	{
		/* 발생된 에러가 없는지 확인 */
		if (IsQueEmpty())	return 0x0000;
		/* 가장 최근에 발생된 에러 반환 */
		return error_code[PopFrontIndex()].error;
	}

	/*
	 desc : 가장 최근에 발생된 EFEM의 에러 코드 및 시간 정보 반환
	 parm : None
	 retn : 0x0000 or 에러 코드 반환 (0x0001 ~ 0xffff)
	*/
	LPG_EATD GetErrorTime()
	{
		/* 발생된 에러가 없는지 확인 */
		if (IsQueEmpty())	return NULL;
		/* 가장 최근에 발생된 에러 반환 */
		return &error_code[PopFrontIndex()];
	}


}	STG_EDSM,	*LPG_EDSM;

#pragma pack (pop)	/* 8 bytes order */