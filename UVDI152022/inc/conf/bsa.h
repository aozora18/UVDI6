
/*
 desc : Vision Config Information
*/

#pragma once

/* --------------------------------------------------------------------------------------------- */
/*                                            상수값                                             */
/* --------------------------------------------------------------------------------------------- */

#define USE_MARK_ROTATE			0
#define CALI_ORIGIN_DIFF_X		50000	/* Vision Calibration의 최초 측정 지점에서 이 설정 값을 빼면, 원점이 나옴 (단위: 0.1 um or 100 nm */
#define MAX_ALIGN_MARK_COUNT	16		/* 최대 16개까지만 마크 검사 가능 (이보다 많은 마크가 있는 경우, 검사 실패 처리) */
#define	MAX_REPLY_COUNT			3
#define	MAX_CYCLE_COUNT			100
#define	MOTION_SPEED			8000

/* --------------------------------------------------------------------------------------------- */
/*                                            열거형                                             */
/* --------------------------------------------------------------------------------------------- */

// Mark Type
typedef enum class __en_vision_match_search_method__ : UINT8
{
	en_vmsm_none		= 0x00,
	en_vmsm_normal		= 0x01,		// 원래 이미지 비교
	en_vmsm_rotate		= 0x02,		// 회전 이미지 비교

}	ENG_VMSM;

// Mat Image Kind
typedef enum class __en_vision_mat_image_kind__ : UINT8
{
	en_vmik_none		= 0x00,
	en_vmik_thres		= 0x01,
	en_vmik_grab		= 0x02,
	en_vmik_markt		= 0x03,
	en_vmik_resize		= 0x04,
	en_vmik_result		= 0x05,		/* Mark 검색 결과 별도의 플래그로 설정 */

}	ENG_VMIK;

/* Mark Measurement Search Type */
typedef enum class __en_vision_mark_measurement_type__ : INT32
{
	en_vmmt_any			= 0x11000000L,
	en_vmmt_positive	= 2L,
	en_vmmt_negative	= -2L,

}	ENG_VMMT;

/* CMPS <--> Vision 통신 패킷 명령 */
typedef enum class __en_vision_cmps_packet_command__ : UINT16
{
	en_vision_req_alive			= 0xfff0,
	en_vision_res_alive			= 0xfff0,

	en_vision_req_stop			= 0x2101,		/* CMPS Client - Req - VISION Stop */
	en_vision_res_stop			= 0x2201,		/* VISN Server - Res - VISION Stop */

	en_vision_req_state			= 0x2102,		/* CMPS Client - Req - VISION State */
	en_vision_res_state			= 0x2202,		/* VISN Server - Res - VISION State */

	en_vision_req_error			= 0x2103,		/* CMPS Client - Req - VISION Device Error code */
	en_vision_res_error			= 0x2203,		/* VISN Server - Res - VISION Device Error code */

	en_vision_req_reset			= 0x2104,		/* CMPS Client - Req - VISION Device Reset */
	en_vision_res_reset			= 0x2204,		/* VISN Server - Res - VISION Device Reset */

	en_vision_req_centering		= 0x2105,		/* CMPS Client - Req - VISION Centering Event */
	en_vision_res_centering		= 0x2205,		/* VISN Server - Res - VISION Centering Event */

	en_vision_req_cam_offset	= 0x2106,		/* CMPS Client - Req - VISION Camera Offset Event */
	en_vision_res_cam_offset	= 0x2206,		/* VISN Server - Res - VISION Camera Offset Event */

	en_vision_req_focus			= 0x2107,		/* CMPS Client - Req - VISION Focus Event */
	en_vision_res_focus			= 0x2207,		/* VISN Server - Res - VISION Focus Event */

	en_vision_req_marklist		= 0x2108,		/* CMPS Client - Req - VISION Mark Pattern List */
	en_vision_res_marklist		= 0x2208,		/* VISN Server - Res - VISION Mark Pattern List */

	en_vision_req_align_grab	= 0x2109,		/* CMPS Client - Req - VISION Align Grab Event*/
	en_vision_res_align_grab	= 0x2209,		/* VISN Server - Res - VISION Align Grab Event*/

	en_vision_req_align_result	= 0x210A,		/* CMPS Client - Req - VISION Align Reuslt Event*/
	en_vision_res_align_result	= 0x220A,		/* VISN Server - Res - VISION Align Reuslt Event*/

	en_vision_req_job_pat_list	= 0x210B,		/* CMPS Client - Req - VISION Job Seted Pattern List */
	en_vision_res_job_pat_list	= 0x220B,		/* VISN Server - Res - VISION Job Seted Pattern List */

	en_vision_req_illumination	= 0x210C,		/* CMPS Client - Req - VISION Illumination Control Event*/
	en_vision_res_illumination	= 0x220C,		/* VISN Server - Res - VISION Illumination Control  Event*/

	en_vision_req_view			= 0x210F,		/* CMPS Client - Req - VISION View Control Event*/
	en_vision_res_view			= 0x220F,		/* VISN Server - Res - VISION View Control Event*/

	en_vision_reply_test		= 0xFFFF,		/* Reply Test를 위한 변수 */

}	ENG_VCPC;

/* Thread Step Status */
typedef	enum class __en_thread_step_status_code__ : UINT8
{
	en_step_thread_init		= 0x00,
	en_step_thread_next		= 0x01,
	en_step_thread_error	= 0x02,
	en_step_thread_wait		= 0x03,
	en_step_thread_done		= 0x04,
	en_step_thread_finish	= 0x05,
	en_max_step_thread,

}	ENG_TSSC;

/* Thread Centering Step Status */
typedef	enum class __en_thread_step_vision_code__ : UINT8
{
	en_step_thread_visn_init		= 0x00,
	en_step_thread_visn_next		= 0x01,
	en_step_thread_visn_error		= 0x02,
	en_step_thread_visn_retry		= 0x03,
	en_step_thread_visn_retry_cmd	= 0x04,
	en_step_thread_visn_wait		= 0x05,
	en_step_thread_visn_done		= 0x06,
	en_max_step_visn,

}	ENG_TSVC;


/* --------------------------------------------------------------------------------------------- */
/*                                            구조체                                             */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 1)

/* 패킷 송수신 구조체 */
typedef struct __st_packet_bsa_send_recv__
{
	UINT16				size;		/* data 버퍼에 저장된 데이터의 크기 */
	UINT8				data[64];	/* head + body + tail */

}	STG_PBSR,	*LPG_PBSR;

#pragma pack (pop)	/* 1 bytes order */

/* --------------------------------------------- */
/*             Vision Receive Packet             */
/* --------------------------------------------- */

#pragma pack(push, 1)

/* Align Grab 유/무 */
typedef struct __st_recv_vision_error_info__
{
	UINT8				unit;
	UINT8				error;

}	STG_RVEI, *LPG_RVEI;

typedef struct __st_recv_vision_common_info__
{
	UINT8				unit;
	UINT8				result;

}	STG_RVCI, *LPG_RVCI;

/* Centering */
typedef struct __st_recv_vision_centering_value__
{
	STG_RVCI			common;
	INT32				move_x;
	INT32				move_y;

	/* 각 멤버 변수들을 Bytes Ordering 수행 */
	VOID SwapData()
	{
		move_x = SWAP32(move_x);
		move_y = SWAP32(move_y);

	}

}	STG_RVCV, *LPG_RVCV;

/* Focus */
typedef struct __st_recv_vision_focus_value__
{
	STG_RVCI			common;
	UINT16				focus_data;
	INT32				move_z;

	/* 각 멤버 변수들을 Bytes Ordering 수행 */
	VOID SwapData()
	{
		focus_data	= SWAP16(focus_data);
		move_z		= SWAP32(move_z);
	}

}	STG_RVFV, *LPG_RVFV;

/* Camera Offset */
typedef struct __st_recv_vision_camera_offset__
{
	UINT8				result;
	INT32				offset_x;
	INT32				offset_y;

	/* 각 멤버 변수들을 Bytes Ordering 수행 */
	VOID SwapData()
	{
		offset_x = SWAP32(offset_x);
		offset_y = SWAP32(offset_y);
	}

}	STG_RVCO, *LPG_RVCO;

/* Align Pattern */
typedef struct __st_recv_vision_pattern_mark__
{
	UINT32				mark1_1;
	UINT32				mark1_2;
	UINT32				mark2_1;
	UINT32				mark2_2;

	/* 각 멤버 변수들을 Bytes Ordering 수행 */
	VOID SwapData()
	{
 		mark1_1 = SWAP32(mark1_1);
 		mark1_2 = SWAP32(mark1_2);
		mark2_1 = SWAP32(mark2_1);
		mark2_2 = SWAP32(mark2_2);
	}

}	STG_RVPM, *LPG_RVPM;

/* Align Grab 유/무 */
typedef struct __st_recv_vision_align_grab__
{
	UINT8			mark_1_grab;
	UINT8			mark_2_grab;

}	STG_RVAG, *LPG_RVAG;

/* Align Grab 유/무 */
typedef struct __st_recv_vision_align_result__
{
	UINT8			result;
	INT32			mark_1_pos_x;
	INT32			mark_1_pos_y;
	INT32			mark_2_pos_x;
	INT32			mark_2_pos_y;

	INT32			shfit_x;
	INT32			shfit_y;
	INT32			shfit_angle;

	/* 각 멤버 변수들을 Bytes Ordering 수행 */
	VOID SwapData()
	{
		mark_1_pos_x= SWAP32(mark_1_pos_x);
		mark_1_pos_y= SWAP32(mark_1_pos_y);
		mark_2_pos_x= SWAP32(mark_2_pos_x);
		mark_2_pos_y= SWAP32(mark_2_pos_y);
		shfit_x		= SWAP32(shfit_x);
		shfit_y		= SWAP32(shfit_y);
		shfit_angle = SWAP32(shfit_angle);
	}

}	STG_RVAR, *LPG_RVAR;

/* Align Grab 유/무 */
typedef struct __st_recv_vision_align_info__
{
	STG_RVAG			mark_grab;
	STG_RVAR			align_result;

}	STG_RVAI, *LPG_RVAI;

/* Job Mark list Result */
typedef struct __st_recv_vision_job_list__
{
	UINT8				result;

}	STG_RVJL, *LPG_RVJL;

/* Align Grab 유/무 */
typedef struct __st_recv_vision_view_control__
{
	UINT8				result;

}	STG_RVVC, *LPG_RVVC;

/* Vision All Information */
typedef struct __st_recv_bsa_vision_command__
{
	UINT8				vision_state;		/* vision 상태 정보 */
	UINT64				update_state_time;	/* Vision State 갱신 시간 */

	STG_RVEI			vision_error;		/* vision Error Code*/

	STG_RVCV			centering;			/* Centring 수신 패킷 */
	STG_RVFV			focus;				/* Focus 수신 패킷 */
	STG_RVCO			cam_offset;			/* Camoffset 수신 패킷 */

	STG_RVPM			pattern;			/* Align에 사용될 Pattern 수신부 (수신에만 사용) */
	STG_RVAI			align_info;			/* Align에 사용될 Align 정보 */

	STG_RVJL			job_list;			/* JobList 전송에 따른 결과 정보 */
	STG_RVVC			view_info;			/* View 설정에 따른 결과 정보 */
	
}	STG_RBVC, *LPG_RBVC;

#pragma pack(pop)	/* 1 bytes ordering */

/* --------------------------------------------- */
/*               Vision Send Packet              */
/* --------------------------------------------- */

#pragma pack(push, 1)

/* Set Centering */
typedef struct __st_send_vision_centering_value__
{
	UINT8				cam_num;
	UINT8				unit;

}	STG_SVCV, *LPG_SVCV;

/* Set Focus */
typedef struct __st_send_vision_focus_value__
{
	UINT8				cam_num;
	UINT8				unit;

}	STG_SVFV, *LPG_SVFV;

/* Set Align */
typedef struct __st_send_align_vision_value__
{
	UINT8				point_num;
	UINT8				mark_num;
	UINT8				pattern_num;
	UINT32				pos_x;
	UINT32				pos_y;

	/*
	 desc : 송신 혹은 수신 데이터의 Bytes ordering 실행
	 parm : None
	 retn : None
	*/
	VOID SwapData()
	{
		pos_x = SWAP32(pos_x);
		pos_y = SWAP32(pos_y);
	}

}	STG_SAVV, *LPG_SAVV;

/* Set Vision View */
typedef struct __st_send_vision_view_mode__
{
	UINT8				mode;

}	STG_SVVM, *LPG_SVVM;

/* Set illumination */
typedef struct __st_send_illumination_mark_onoff__
{
	UINT8				mark;
	UINT8				onoff;

}	STG_SIMO, *LPG_SIMO;

/* Set JobMarkList */
typedef struct __st_send_vision_job_info__
{
	/* 210715 */
	UINT64				mark_list_1;
	UINT64				mark_list_2;

}	STG_SVJI, *LPG_SVJI;

/* Set JobMarkList */
typedef struct __st_send_vision_job_send__
{
	UINT32				mark1_1;
	UINT32				mark1_2;
	UINT32				mark2_1;
	UINT32				mark2_2;

	void SwapData()
	{
		mark1_1 = SWAP32(mark1_1);
		mark1_2 = SWAP32(mark1_2);
		mark2_1 = SWAP32(mark2_1);
		mark2_2 = SWAP32(mark2_2);
	}

}	STG_SVJS, *LPG_SVJS;

/* Set Vision Command */
typedef struct __st_send_bsa_vision_command__
{
	STG_SVCV			centering;
	STG_SVFV			focus;
	STG_SAVV			align;
	STG_SVVM			view;
	STG_SIMO			illum;
	STG_SVJI			job_list;
	STG_SVJS			job_send;

}	STG_SBVC, *LPG_SBVC;

/* Align Result */
typedef struct __st_send_align_find_result__
{
	INT32				align_mark1_x	= 0;
	INT32				align_mark1_y	= 0;
	INT32				align_mark2_x	= 0;
	INT32				align_mark2_y	= 0;
	DOUBLE				align_degree	= 0.0f;

	VOID InitAlignResult()
	{
		align_mark1_x	= 0;
		align_mark1_y	= 0;
		align_mark2_x	= 0;
		align_mark2_y	= 0;
		align_degree	= 0.4f;
	}

}	STG_SAFT, *LPG_SAFT;

#pragma pack(pop)	/* 1 bytes ordering */






