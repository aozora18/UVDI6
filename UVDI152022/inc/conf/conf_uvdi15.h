
/*
 desc : System Configuration Information
*/

#pragma once

#include "conf_comn.h"
#include <vector>
#include <tuple>
#include <map>
/* --------------------------------------------------------------------------------------------- */
/*                                           상수 값                                             */
/* --------------------------------------------------------------------------------------------- */

#define	WORK_NAME_LEN				128
#define	STEP_NAME_LEN				128

/* --------------------------------------------------------------------------------------------- */
/*                                       전역(Global) 열거형                                     */
/* --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- */
/*                                Config 구조체 - Luria (Service)                                */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 8)

/* Setup Align 에 대한 전처리 정보 */
typedef struct __st_config_setup_camera_info__
{
	UINT8				acam_count;								/* 설치된 Align Camera 대수 */
	UINT8				acam_inst_angle;						/* 카메라가 설치될 때, 회전 여부 (0: 회전 없음, 1: 180도 회전) */
	UINT8				u8_rserved[6];


	UINT16				acam_size[2];							/* Align Camera Size (Width / Height) (단위: resolution) */
	UINT16				aoi_size[2];							/* FOV 관심 영역 : Align Mark Search (Width / Height) (unit: pixel) */
	UINT16				soi_size[2];							/* FOV 관심 영역 : Vision Step Search Search (Width / Height) (unit: pixel) */
	UINT16				doi_size[2];							/* FOV 관심 영역 : Vision DOF Focusing (Width / Height) (unit: pixel) */
	UINT16				mes_size[2];							/* FOV 관심 영역 : Vision Calibration Measurement (Width / Height) (unit: pixel) */
	UINT16				ioi_size[2];							/* FOV 관심 영역 : Vision Shutting Inspection (Width / Height) (unit: pixel) */
	UINT16				spc_size[2];							/* FOV 관심 영역 : Align Camera Specification (Width / Height) (unit: pixel) */
	UINT16				stg_size[2];							/* FOV 관심 영역 : QuartZ (Stage Straightness) (Width / Height) (unit: pixel) */
	UINT16				hol_size[2];							/* FOV 관심 영역 : Material (Hole Size) (Width / Height) (unit: pixel) */
	UINT16				u16_rserved[2];

	DOUBLE				acam_up_limit;							/* 얼라인 카메라 Z 축 위로 최대 Limit 값   (단위: mm) */
	DOUBLE				acam_dn_limit;							/* 얼라인 카메라 Z 축 아래로 최대 Limit 값 (단위: mm) */

	DOUBLE				safety_pos[MAX_ALIGN_CAMERA];			/* 얼라인 카메라 X 축 안전 위치 값 (단위: mm) */

	/*
	 desc : 카메라 최대 해상도 크기에 따른 버퍼 크기 반환 (단위: Bytes)
	 parm : None
	 retn : 버퍼 할당 크기 (단위: Bytes)
	*/
	UINT32 GetCamSizeBytes()
	{
#if 1
		return (acam_size[0] * acam_size[1]);
#else
		UINT16 u16Width	= WIDTHBYTES(1, cam_size[0]);
		return (u16Width * acam_size[1]);
#endif
	}

	/*
	 desc : 카메라 크기 (cam_size보다 aoi/soi/doi/ioi_size가 더 크면 에러 임
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsCamSizeValid()
	{
		if (acam_size[0] < aoi_size[0] || acam_size[1] < aoi_size[1])	return FALSE;
		if (acam_size[0] < soi_size[0] || acam_size[1] < soi_size[1])	return FALSE;
		if (acam_size[0] < doi_size[0] || acam_size[1] < doi_size[1])	return FALSE;
		if (acam_size[0] < ioi_size[0] || acam_size[1] < ioi_size[1])	return FALSE;
		if (acam_size[0] < mes_size[0] || acam_size[1] < mes_size[1])	return FALSE;
		if (acam_size[0] < hol_size[0] || acam_size[1] < hol_size[1])	return FALSE;

		return TRUE;
	}

	/*
	 desc : 얼라인 카메라의 Z 축 이동 범위가 벗어났는지 여부
	 parm : pos	- [in]  이동 위치 값 (단위: mm)
	 retn : TRUE or FALSE
	*/
	BOOL IsValidRangePos(DOUBLE pos)
	{
		return (acam_up_limit >= pos && acam_dn_limit <= pos);
	}

}	STG_CSCI,	*LPG_CSCI;



struct MarkoffsetInfo
{

private:
	map<bool, vector <std::tuple<double,double>>> offsetValues;
public:
	void Push(bool isGlobal, std::tuple<double, double> val) 
	{
		offsetValues[isGlobal].push_back(val);
	}

	bool Get(bool isGlobal, int tgtIdx , std::tuple<double, double>& val)
	{
		if (offsetValues[isGlobal].size() <= tgtIdx) return false;
		val = offsetValues[isGlobal][tgtIdx];
		return true;
	}
};

/* Setup Align 에 대한 전처리 정보 */
typedef struct __st_config_setup_align_info__
{
	//UINT8				align_method;							/* 0 : Align Camera 2개로 Mark 2 or 4개 측정 (이동하면서 측정) */ 
																/* 1 : Align Camera 2개로 Mark 2 측정 후 중간에 멈추고, 카메라 좌/우 조정 후 다시 나머지 2개 측정 (이동 후 멈추고 측정, 다시 이동 후 측정) */
																/*     거버 내 마크의 4 점이 직사각형 (카메라 FOV에 들어오지 못하는 경우)이 아닌 경우 */
																/* 2 : Align Camera 1개로 임의 마크 측정 (멈추면서 측정) */
	UINT8				use_2d_cali_data;						/* Align Camera의 2D Calibration Data 적용 여부 (1:사용, 0:미사용) */
	UINT8				use_invalid_mark_cali;					/* 인식되지 않은 Mark에 대해 강제(수동)로 인식시킬지 여부 (1:사용, 0:미사용) */
	UINT8				use_mark_offset;						/* 각 Global Mark Offset에 추가 기능 적용 여부 (1:사용, 0:미사용) */
	UINT8				u8_reserved[4];
	DOUBLE				dof_film_thick;							/* Align Camera & Photohead Focus 조정할 때 사용되는소재의 두께 (단위: mm) */
	DOUBLE				table_unloader_xy[MAX_TABLE][2];		/* 노광 소재를 올려 놓기 위한 스테이지의 작업 위치 (단위: mm) (소수점 4자리까지 유효) */
	DOUBLE				mark2_org_gerb_xy[2];					/* 거버에 저정된 Mark 2번의 X / Y 좌표 (단위: mm) */
	DOUBLE				mark2_stage_x;							/* 거버에 저장된 Mark 2번 기준 X 축 모션 좌표 (단위: mm) */
	//DOUBLE				mark_offset_x[4];						/* 각 Global Mark의 offset 설정값(단위: mm) */
	//DOUBLE				mark_offset_y[4];
	DOUBLE				mark_horz_diff;							/* 수평(0) / 수평(1) 의 오차값의 제한값(단위: mm)*/
	DOUBLE				mark_vert_diff;							/* 수직(2) / 수직(2) 의 오차값의 제한값(단위: mm)*/
	MarkoffsetInfo*		markOffsetPtr;
	//UINT8				use_Localmark_offset;					/* 각 Local Mark Offset에 추가 기능 적용 여부 (1:사용, 0:미사용) */
	//DOUBLE				localMark_offset_x[16];
	//DOUBLE				localMark_offset_y[16];

	DOUBLE				distCam2Cam[3];

	
	//DOUBLE				mark_offset_x[4];						/* 각 Global Mark의 offset 설정값(단위: mm) */
	//DOUBLE				mark_offset_y[4];

	/* 거버에 저장된 Mark 2번 기준 X 축 모션 좌표 (단위: mm) */
	void SetMarkoffsetPtr(MarkoffsetInfo& ptr)
	{
		markOffsetPtr = &ptr;
	}

}	STG_CSAI,	*LPG_CSAI;

/* Vision Camera로부터 Grabbed Image에 대한 전처리 정보 */
typedef struct __st_config_grab_image_preprocess__
{
	/* 아래 2개 변수 중 1개 값이 0인 경우, 이 기능을 사용하지 않음 */
	UINT8				use_grab_pre_proc;		/* 0 : 사용 안함, 1 : 사용함 */
	UINT8				use_noise_remove;		/* 0 : 노이즈 제거 안함, 1 : 노이즈 제거 기능 사용함 */
	UINT8				use_thin_operation;		/* 이미지 가장 자리를 얇게 처리할지 여부 */
												/* !!! 현재로선 이 플래그 값은 0 값으로 무조건 설정 !!! */
	UINT8				use_small_particle;		/* 작은 이미지 조각들 제거 기능 사용 여부 (0 or 1) */
	UINT8				mim_bin_condition;		/* 3 (M_EQUAL), 4 (M_NOT_EQUAL), 5 (M_GREATER), 6 (M_LESS), */
												/* 7 (M_GREATER_OR_EQUAL), 8 (M_LESS_OR_EQUAL) */
												/* 3, 4 값은 될 수 있으면 사용하지 말자, 주로 7,8 중 하나를 선택해서 사용하길 */
												/* 7 : 정상적으로 출력, 8 : 흑백 반전 시켜서 출력 */
	UINT8				remove_small_particle;	/* Remove Small Particles (unit : pixel). 이 값이 크면, 작은 객체들은 없어짐 */
	UINT8				u8_reserved[2];

	UINT16				remove_small_procmode;	/* M_BINARY (4096) or M_GRAYSCALE (512) */
	UINT16				thin_proc_mode;			/* M_BINARY (4096), M_BINARY2 (1), M_BINARY3 (2) or M_GRAYSCALE (512) */
	UINT16				u16_reserved[2];

}	STG_CGIP,	*LPG_CGIP;

/* Vision Camera로부터 수동으로 Mark 검색에 필요한 정보 */
typedef struct __st_config_vision_mark_find__
{
	UINT8				max_mark_regist;		/* 최대 등록 가능한 마크 모델 개수 (255개 넘으로면 안됨) */
	UINT8				max_mark_grab;			/* 마크 검색을 위해 카메라 Grabbed Image의 최대 개수 (Max : 255) (Normal : 4) */
												/* !!! 주의 !!! : 카메라 별로 검색할 수 있는 최대 개수 */
												/* 즉, 카메라 마다 이미지를 Grab할 수 있는 최대 개수 */
												/* ex> 등록된 마크의 개수가 4개이고, 카메라가 1대이면, 4 */
												/*     등록된 마크의 개수가 4개이고, 카메라가 2대이면, 2 */
	UINT8				max_mark_find;			/* Grabbed Image에서 검색 가능한 마크 최대 개수 (최대 255개 넘으면 안됨) */
												/* ex> Grabbed Image에서 검색 가능한 마크 최대 개수임 */
	UINT8				model_speed;			/* 마크 인식 후 매칭률이 현재 설정 값보 떨어지면, UI에서 색상으로 표현 */
												/* 이 값보다 매칭률이 떨어지면 노광을 안하는 것이 아니라, 단지 확인용 (단위: %) */
	UINT8				detail_level;			/* 등록된 모델 이미지와 Grabbed Image이미지의 Edge 영역에 대한 비교 정도 값 */
												/* 값이 클수록 상세히 비교하지만, 속도는 많이 느려지짐 */
												/* LOW보다 HIGH로 갈수록 더 많은 Edge를 가지고 비교하게 됨 */
												/* 0 (M_VERY_LOW), 1 (M_LOW), 2 (M_MEDIUM), 3 (M_HIGH), 4 (M_VERY_HIGH) */
	UINT8				mixed_cent_type;		/* Mixed Mark (Multi-Mark)의 중심 구하는 방식 설정 */
												/* 0: 검색된 마크들의 Min/Max 좌표를 구한 후 사각형의 중심 값을 좌표로 설정 */
												/* 1: 검색된 마크들의 다각형 좌표에 대한 무게 중심 값을 좌표로 설정 */
												/* 2: 검색된 마크들의 다각형 좌표에 대한 교차점 (허프만 변환)을 통한 중심 값을 좌표로 설정 */ 
	UINT8				u8_reserved[2];
	UINT16				mixed_weight_rate;		/* 복합 얼라인 마크 사용될 때, 적용되는 가중치 값 (1 ~ 100) (단위: %) */
	UINT16				mixed_squre_rate;		/* 다점 얼라인 마크의 주변 (Side)의 정-사각형의 가로/세로 비율 오차 값 (비율. 단위: %) 이내에 */
												/* 있는지 판단을 위한 비율 값 (단위 : um) 즉, 가로/세로 크기가 얼마나 정사각형인지 판단 */
												/* 만약 이 값이, 100이면 주변 (Side) 마크의 가로 / 세로 크기가 완전한 정사각형으로 구성 */
												/* 만약 이 값이,  50이면 주변 (Side) 마크의 가로 / 세로 크기가 직사각형 구조 즉, */
												/* 가로 (혹은 세로)가 세로 (혹은 가로) 보다 50% 이상 크다면 유효 */
	UINT16				u16_reserved[2];
	DOUBLE				mixed_dist_limit;		/* 복합 얼라인 마크의 경우, Hole과 Laser 간의 중심 마크 간의 최대 허용 오차 간격 (단위: mm) (최대 값. 255) (소수점 3자리까지 유효) */
	DOUBLE				model_smooth;			/* 기본 값은 75.0이며, 경계선의 Noise 값을 얼마다 부드럽게 처리하는 정도이며,
												/* 값의 범위는 (0.0 ~ 100.0) 즉, 경계선 구분이 어려울 수록 값이 크면 좋다(?)
												/* 0.0 값이면, 기본 값 (보통 50.0) (!!! 이미지의 노이즈 감소 시키는데 유용 !!!) */
	DOUBLE				score_rate;				/* 마크 인식 후 매칭된 이미지가 실제 등록된 Mark 크기 대비 주어진 값보다 크거나 작으면 실패 처리 (단위: %) */
	DOUBLE				scale_rate;				/* 마크 인식 후 매칭된 이미지가 실제 등록된 Mark 크기 대비 주어진 값보다 크거나 작으면 실패 처리 (단위: %) */
	UINT8				image_process;			/* Mark Find 하기 전 이미지 처리 여부, 0:미사용, 1:사용*/

}	STG_CVMF,	*LPG_CVMF;
#if 0
/* Vision Camera로부터 Calibration 검색에 필요한 정보 */
typedef struct __st_config_vision_cali_find__
{
	UINT8				model_type;				/* M_CIRCLE (8), M_ELLIPSE (16), M_SQUARE (32), M_RECTANGLE (64) */
	UINT8				u8_reserved[7];
	DOUBLE				model_color;			/* BLACK (256), WHITE (128) */
	DOUBLE				model_cali;				/* Calibration Object (MODEL SIZE (unit : um) - Width or Diameter or Radius or Etc) */

}	STG_CVCF,	*LPG_CVCF;
#endif
/* 소재의 Edge Line Detection 관련 정보 */
typedef struct __st_config_line_edge_detection__
{
	TCHAR				text_font_name[LF_FACESIZE];	/* 엣지 라인 검색 결과 값이 출력되는 폰트 이름 (Max. 31) */

	UINT8				part_expose_repeat:1;			/* 0 : 분할 노광일 경우, 첫 분할 노광일 때만 엣지 검출작업 진행 */
														/* 1 : 분할 노광일 경우, 매 분할 노광일 때마다 엣지 검출작업 진행 */
	UINT8				box_angle_mode:1;				/* 0: 오로지 BOX_ANGLE에서 설정된 값에 의해 수행 */
														/* 1: BOX_ANGLE을 포함한 다양한 각도 검색 방법을 수행할 수 있음 */
	UINT8				search_region_clip:1;			/* 이미지 밖의 영역으로 검색 영역이 벗어난 경우, 자동으로 이미지 안쪽으로 */
														/* 검색 영역을 선택하도록 할 것인지 여부 (0 or 1) */
	UINT8				edge_find_option:2;				/* 0 : 검출된 엣지 중에서 이미지 중심에서 가장 가까운 값 기준 */
														/* 1 : 검출된 엣지 중에서 Edge Strength가 가장 큰 값 기준 */
														/* 2 : 검출된 엣지 중에서 이미지 중심에서 가장 가깝고, Edge Strength가 가장 큰 값 기준 */
														/* 3 : 검출된 엣지의 개수가 오로지 1개이면 정상 노광, 2개 이상이면 물어보는 창 출력 */
	UINT8				filter_type:2;					/* 0:EULER (Default), 1:PREWITT, 2:SHEN */
														/* EULER   : 처리속도는 PREWITT에 의해 빠르지만 노이즈에는 약함 (kernel size:2) */
														/* PREWITT : 처리속도는 EULER보다 느리지만 노이즈에 덜 민감함 (kernel size:3) */
														/* SHEN    : 재귀호출 지원을 하며, kernel size는 이론적으로 제한 없음 */
														/*           노이즈 강하지만, EULER, PREWITT에 비해 상대적으로 처리속도는 느림 */
														/*           추가적으로 FILTER_SMOOTHNESS와 결합하여 가장 좋은 결과 얻어냄 */
	UINT8				u8_bit1:1;
	UINT8				polarity:2;						/* 0: NULL (적용 (사용) 하지 않음) */
														/* 1: ANY (고려하지 않음. 사용하지 않는 것과는 다른 의미) */
														/* 2: POSITIVE (rising edge. 어두운 곳에서 밝은 곳으로. 0:Black -> 255:White) */
														/* 3: NEGATIVE (falling edge. 밝은 곳에서 어두운 곳으로. 255:White -> 0:Black) */
	UINT8				orientation:2;					/* 검색 방향 (극성) */
														/* 1: ANY (마커 (검색) 방향을 알 수 없다?) */
														/* 2: HORIZONTAL (수평으로, 위에서 아래로 검색) */
														/* 3: VERTICAL (수직으로, 왼쪽에서 오른쪽으로 검색) */
	UINT8				u8_bits2:4;
	UINT8				filter_smoothness;				/* 노이즈 제거 작업 수행 (0 ~ 100) 단, FILTER_TYPE = SHEN인 경우만 유효 */
	UINT8				edge_threshold;					/* 기본 값 2 (0 ~ 100) */
														/* 값이 작아지면, 영역을 구분하는 엣지의 개수가 증가하므로, 검색 속도가 느려짐 */
														/*                반면에, 좀더 정확한 엣지 검색이 됨 */
	UINT8				sub_regions_size;				/* SubRegions의 크기 조절 (1 ~ 100%) 보통 50% 이상으로 하면 좋다 (?) */
	UINT8				sub_regions_number;				/* SubRegions의 개수 조절 (1 or Later. Max 100) */
														/* 참고로, SUB_REGIONS_SIZE와 NUMBER의 곱이 100을 넘으면 안됨 */
														/* (될 수 있으면, 곱이 100이 나오도록 설정해주면 좋음) */

	UINT8				max_find_count;					/* Line Edge 검색할 대상의 최대 개수. 보통 4개이나, 더 많을 수도 있음 */
	UINT8				text_font_size;					/* 엣지 라인 검색 결과 값이 출력되는 폰트 크기 (단위: 1 ~ 63). 보통 20 */
	UINT8				box_size_width;					/* 검사 영역 크기 (단위: %) (값의 범위는 15 ~ 85 이면, 값이 클수록 이미지 넓이와 동일해짐) */
	UINT8				box_size_height;				/* 검사 영역 크기 (단위: %) (값의 범위는 15 ~ 85 이면, 값이 클수록 이미지 높이와 동일해짐) */
														/* BOX_SIZE 영역에서 SIZE 크기씩 NUMBER 개수만큼 검색하라는 뜻임 */
	UINT8				box_angle_delta_neg;			/* BOX_ANGLE 기준으로 이 값 이전 각도부터 검색 수행 (0 ~ 180) */
	UINT8				box_angle_delta_pos;			/* BOX_ANGLE 기준으로 이 값 이후 각도까지 검색 수행 (0 ~ 180) */

	INT8				sub_regions_offset;				/* SubRegions의 위치 조절 비율 (-100% ~ 0 ~ +100%) */
	INT8				i8_reserved[3];

	UINT16				box_angle;						/* Box search region의 각도 설정 즉, 검색 영역의 사각형 박스의 회전 각도 값 */
														/* 엣지 검색 대상의 영역을 몇 도로 회전할 것인지 값 (0 ~ 360) 보통 0 */
	UINT16				u16_reserved[3];

	DOUBLE				grab_image_edge_angle;			/* Grabbed Image의 엣지 검사할 때 검사 기준 각도 (큰 의미는 없음) */
	DOUBLE				box_angle_accuracy;				/* Marker의 대략적인 위치 찾은 후, 상세히 검색할 각도의 Step 크기 (0.1 ~ 180.0) */
														/* 0.0 값을 입력한 경우, DISABLE로서 Tolerance (허용 오차) 각도와 동일한 크기가 입력됨 */
	DOUBLE				print_diff_limit;				/* 노광 시작 위치와 Edge Line 검색 결과 위치 간의 거리 오차 한계 (단위: mm) */
														/* 절대 값 (노광 시작 위치 - Edge Line 검색 결과 위치 (모션 검사 시작 위치 계산)) */
														/* 즉, 노광 시작 위치와 검색된 위치 간의 거리 간격이 이 값보다 크면 소재 뒤틀림이 심함 */
														/* 최대 입력 가능한 값 (Max: 2 mm) */

}	STG_CLED,	*LPG_CLED;

/* Match Find Option - Vision Edgein Detection */
typedef struct __st_config_mark_edge_detection__
{
	UINT8				filter_type:4;			/* 1:SHEN or 2:DERICHE, 3:PREWITT, 4:SOBEL, 5:FREICHEN */
	UINT8				threshold:4;			/* 1:Very High, 2:High, 3:Medium, 4:Low */
	UINT8				gaussian_filer:4;		/* Gaussian Filter 값 : 0 ~ 9 */
	UINT8				use_float_mode:1;		/* Float Mode 사용 여부 (0 or 1) */
	UINT8				use_closed_line:1;		/* 검색된 엣지들 중 완전히 연결된 것만 추출 */
	UINT8				inc_internal_edge:1;	/* 검색된 엣지들 중에서 내부에 포함된 엣지 검색 여부 (0 or 1) */
	UINT8				edge_analysis_save:1;	/* 1 : 분석된 이미지 저장 (edge), 0 : 분석된 이미지 저장 안함 */
	UINT8				u8_reserved[6];

	INT32				m_accuracy;				/* -9999 (Disable), 3 (High), 4 (Very High) */
	INT32				i32_reserved;

	UINT32				filter_smoothness;		/* FILTER 검색 조건 값 (기본값 : 70, 최대 100, 최소 0) */
	UINT32				max_find_count;			/* 최대 검색할 수 있는 개수 */

	DOUBLE				min_dia_size;			/* 최소 검색 크기 (검색된 원의 객체 지름) (단위: mm) */
	DOUBLE				max_dia_size;			/* 최대 검색 크기 (검색된 원의 객체 지름) (단위: mm) */
	DOUBLE				extraction_scale;		/* 1.0 or later */
	DOUBLE				moment_elongation;		/* 연장선의 정도 값 (0.1 ~ 1.0) 아마 낮을 수록 모든 객체 찾을 가능성 있음 */
	DOUBLE				mark_spare_size;		/* 기존 엣지로 검출된 마크 크기를 더 크게할 때 혹은 축소할 때 적용하기 위한 값 (단위: um) */
	DOUBLE				fill_gap_continuity;	/* 가장 인접한 점끼로 연결할 것인지, 가장 곡면정도가 부드러운 것 끼리 할것인지 강도 값 설정 */

	/*
	 desc : 값 유효성 확인
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsValid()
	{
		BOOL bSucc = max_find_count > 0 && max_find_count < 256 &&
					 filter_type > 0 && filter_type < 6 && threshold > 0 && threshold < 6 &&
					 m_accuracy != 0 && (mark_spare_size >= -500.0f && mark_spare_size <= 500.0f);
		if (!bSucc)
		{
			AfxMessageBox(L"Failed to load the config [config_edge_detection_option]", MB_ICONSTOP|MB_TOPMOST);
		}
		return bSucc;
	}

}	STG_CMED,	*LPG_CMED;

/* LED Power Parameter */
typedef struct __st_config_led_power_info__
{
	/* Position */
	DOUBLE				measure_pos[6][3];	/* 측정 시 이동 좌표 [Head Number][X, Y, Z] (단위 : mm) */

	/* Measure Table */
	UINT16				begin_power_index;	/* 측정 시작 인덱스 값 */
	UINT16				measure_count;		/* 측정 횟수 */
	UINT16				interval_power;		/* 측정 횟수만큼 상승될 인덱스 값 */

	/* Find Max Ill Pos */
	UINT8				max_points;			/* 최대 조도 위치를 찾을 때 측정 포인트 수 */
	DOUBLE				pitch_pos;			/* 각 포인트 별 거리 값 */
	DOUBLE				focus_scan_pos;		/* 헤드 포커스 측정 거리 값 */

	/* Speed */
	DOUBLE				stage_speed;		/* 스테이지 이동 속도 */
	DOUBLE				head_speed;			/* 헤드 이동 속도 */

	/* Manual */
	UINT16				manual_power_index;	/* 수동 측정 인덱스 값 */

	DOUBLE				normal_boundary;	/* 정상 범주의 오차 값 */
	DOUBLE				warning_boundary;	/* 경고 범주의 오차 값 */

}	STG_CLPI, * LPG_CLPI;

/* Photohead Focus Parameter */
typedef struct __st_config_photohead_focus_info__
{
	UINT8				print_type;				/* 0 : X = Focus Y = Energy, 1 : X = Energy Y = Focus */
	UINT16				step_x_count;			/* Stage X 방향으로 이동하는 총 STEP 개수 */
	UINT16				step_y_count;			/* Stage Y 방향으로 이동하는 총 STEP 개수 (STEP_MOVE_Y * STE_MOVE_COUNT = 이 값이 Y 축 최대 이동 거리 넘지 않아야 함) */

	DOUBLE				start_foucs[MAX_PH];	/* 포커스 시작 위치 (단위: mm) */
	DOUBLE				start_energy;			/* 광량 시작 위치 */
	DOUBLE				step_foucs;				/* 포커스 증가 값 (단위: mm) */
	DOUBLE				step_energy;			/* 광량 증가 값 */

	DOUBLE				expo_stage[2];			/* 처음 노광될 위치로 이동하기 위한 Motion 위치 (단위: mm) (광학계에 전달될 위치임. 소숫점 3자리까지 유효) */

	DOUBLE				mark2_stage[2];			/* 처음 노광된 마크의 위치로 이동하기 위한 Motion 위치 (단위: mm) (소숫점 4자리까지 유효) */
	DOUBLE				mark2_acam_x[2];		/* 처음 노광된 마크의 위치로 이동하기 위한 Motion 위치 (단위: mm) (소숫점 4자리까지 유효) */
	DOUBLE				step_move_x;			/* Stage X 방향으로 이동하기 위한 마크와 마크의 이동 간격 (단위: mm) (소숫점 4자리까지 유효) */
	DOUBLE				step_move_y;			/* Stage Y 방향으로 이동하기 위한 마크와 마크의 이동 간격 (단위: mm) (소숫점 4자리까지 유효) */

	DOUBLE				mark_period;			/* PHOTOHEAD 간의 떨어진 거리 즉, Mark 간의 떨어진 폭 길이 (단위:  mm) (소숫점 4자리까지 유효) */

	DOUBLE				model_dia_size;			/* 검색된 엣지 객체들 중에서 이 설절 값과 가장 가까운 값을 인정 (단위: mm) (소숫점 3자리까지 유효) */
	DOUBLE				model_color;			/* 128 (흰색), 256 (검정색) */

}	STG_CPFI,	*LPG_CPFI;

/* Align Camera Calibration Parameter */
typedef struct __st_config_align_camera_calibration__
{
	UINT8				set_match_mode;					/* 단일 마크 검색 대상. 0 : Circle 검색, 1 : Ring 검색 */
	UINT8				u8_reserved[7];
	UINT16				max_rows_count;					/* X 축으로 모션 이동하면서 측정할 수 있는 최대 개수 */
	UINT16				max_cols_count;					/* Y 축으로 모션 이동하면서 측정할 수 있는 최대 개수 */
	/* 확장 (개선) 버전에서는 아래 2개 변수 사용하지 않음 */
	UINT16				set_rows_count;					/* 행 (Row) 측정 개수 */
	UINT16				set_cols_count;					/* 열 (Column) 측정 개수 */

	UINT32				model_ring_type;				/* Circle (8), Ellipse (16), Square (32), Rectangle (64), Ring (256), */
	UINT32				model_shut_type;				/* Circle (8), Ellipse (16), Square (32), Rectangle (64), Ring (256), */
														/* Cross (8192), Diamond (32768), Triangle (65536), Image (?????) */
	UINT32				u32_reserved[1];

	UINT64				period_wait_time;				/* Calibration 측정하기 전 모터가 멈추가 난 이후 안정될 때까지 최소한 대기 시간 (단위: msec) */
	DOUBLE				period_min_size;				/* X/Y 축으로 이동할 때, 이동 최소 거리 (단위: mm) (소숫점 4자리까지 유효) */
	/* 확장 (개선) 버전에서는 아래 2개 변수 사용하지 않음 */
	DOUBLE				period_row_size;				/* 행 (Row) 이동 (측정) 간격 (단위:  mm) */
	DOUBLE				period_col_size;				/* 열 (Column) 이동 (측정) 간격 (단위:  mm) */

	DOUBLE				model_shut_color;				/* 128 (흰색), 256 (검정색) */
	DOUBLE				model_ring_color;				/* 128 (흰색), 256 (검정색) */

	DOUBLE				model_shut_size;				/* 등록될 모델 (Circle) 의 크기 (단위: mm) (지름) */
	DOUBLE				model_ring_size1;				/* 등록될 모델 (Ring) 의 내부 크기 (단위: mm) (지름) */
	DOUBLE				model_ring_size2;				/* 등록될 모델 (Ring) 의 외부 크기 (단위: mm) (지름) */
#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
	DOUBLE				mark_stage_x;					/* 기준 소재 (두께)의 측정을 위한 마크 (노광 객체) 기준 위치. Stage X (단위: mm) */
	DOUBLE				mark_stage_y[MAX_ALIGN_CAMERA];	/* 기준 소재 (두께)의 측정을 위한 마크 (노광 객체) 기준 위치 (Align Camera 1번 측정할 때). Stage Y (단위: mm) */
#else
	DOUBLE				mark_stage_x[MAX_ALIGN_CAMERA];	/* 기준 소재 (두께)의 측정을 위한 마크 (노광 객체) 기준 위치. Stage X (단위: mm) */
	DOUBLE				mark_stage_y[MAX_ALIGN_CAMERA];	/* 기준 소재 (두께)의 측정을 위한 마크 (노광 객체) 기준 위치 (Align Camera 1번 측정할 때). Stage Y (단위: mm) */
#endif
	DOUBLE				mark_acam[MAX_ALIGN_CAMERA];	/* 기준 소재 (두께)의 측정을 위한 마크 (노광 객체) 기준 위치. Camera X (단위: mm) (소수점 4자리까지 유효) */
	DOUBLE				scale_valid_rate;				/* 찾고자 하는 마크의 크기 비율 값이 1.0 값 기준으로, 이 값 오차 (+/-) 내에 있어야 됨 (보통 0.1 값 정도 주면 됨) (소수점 4자리까지 유효) */
	DOUBLE				score_valid_rate;				/* 찾고자 하는 Model의 매칭 비율 값이 이 값 보다 높아야 됨 (보통 90.0000) 이 값은 % 값으로 설정. 즉 95% 수준 이상 찾길 원하면 95.0000 (소수점 4자리까지 유효) */
	DOUBLE				motion_speed;					/* Camera 1, Stage X, Stage Y 측정 이동 속도 (unit: mm / sec) (소수점 4자리까지 유효) */
	DOUBLE				err_limit_value;				/* 오차 한계 값 즉, 이 값이 벗어나면, 에러 값으로 판단 (단위: mm) 프로그램의 그리드 영역에서 측정된 배경 (CELL)에 "임의 배경색" 표현 */

	/*
	 desc : 전체 측정 개수 반환
	 parm : None
	 retn : 측정 개수
	*/
	UINT32 GetTotalCount()
	{
		return UINT32(set_cols_count) * UINT32(set_rows_count);
	}

	/*
	 desc : 오차 한계 값 반환 (단위: um)
	 parm : None
	 retn : 오차 한계 값 반환
	*/
	DOUBLE GetErrLimitValUm()
	{
		return (DOUBLE)ROUNDED(err_limit_value * 1000.0f, 1);
	}

}	STG_CACC,	*LPG_CACC;

/* Stage xy straightness (motion xy straightness) */
typedef struct __st_config_stage_motion_straightness__
{
	DOUBLE				model_meas_color;				/* 128 (흰색), 256 (검정색) */
	DOUBLE				model_meas_size;				/* 등록될 모델 (Circle) 의 크기 (단위: mm) (지름) */
	DOUBLE				scale_valid_rate;				/* 찾고자 하는 마크의 크기 비율 값이 1.0 값 기준으로, 이 값 오차 (+/-) 내에 있어야 됨 (보통 0.1 값 정도 주면 됨) (소수점 4자리까지 유효) */
	DOUBLE				score_valid_rate;				/* 찾고자 하는 Model의 매칭 비율 값이 이 값 보다 높아야 됨 (보통 90.0000) 이 값은 % 값으로 설정. 즉 95% 수준 이상 찾길 원하면 95.0000 (소수점 4자리까지 유효) */
	DOUBLE				motion_speed;					/* Camera 1, Stage X, Stage Y 측정 이동 속도 (unit: mm / sec) (소수점 4자리까지 유효) */
	DOUBLE				err_limit_value;				/* 오차 한계 값 즉, 이 값이 벗어나면, 에러 값으로 판단 (단위: mm) 프로그램의 그리드 영역에서 측정된 배경 (CELL)에 "임의 배경색" 표현 */

}	STG_CSMS,	*LPG_CSMS;

/* Align Camera Spec. Information */
typedef struct __st_config_acam_spec_info__
{
	DOUBLE				point_dist;							/* 카메라 FOV 이내에 각도 및 픽셀 크기를 측정하기 위한 두 지점 사이의 거리 값 (단위: mm) */
	DOUBLE				step_move_z;						/* Align Camera Up/Donw 방향으로 이동하기 위한 이동 간격 (단위: mm) (소수점 3자리까지 유효) */
	DOUBLE				acam_z_focus[MAX_ALIGN_CAMERA];		/* Align Camera 내에 설치된 Camera Z Axis 높이 값 (단위: mm) (소수점 4자리까지 유효) : 쿼츠 기준 */
	DOUBLE				quartz_stage_x;						/* Align Camera Specification 측정을 위한 QuartZ 기준 위치. Stage X (단위: mm) (소수점 4자리까지 유효) */
	DOUBLE				quartz_stage_y[MAX_ALIGN_CAMERA];	/* Align Camera Specification 측정을 위한 QuartZ 기준 위치 (Align Camera 1번 측정할 때). Stage Y (단위: mm) (소수점 4자리까지 유효) */
	DOUBLE				quartz_acam[MAX_ALIGN_CAMERA];		/* Align Camera Specification 측정을 위한 QuartZ 기준 위치. Camera X (단위: mm) (소수점 4자리까지 유효) */
	DOUBLE				in_ring_size;						/* 스테이트에 고정된 쿼츠의 링 중 안쪽 크기 (단위: mm)mm) (소수점 3자리까지 유효 보통 안쪽 링 1000 um, 바깥 링 2000 um */
	DOUBLE				in_ring_color;						/* 스테이트에 고정된 쿼츠의 링 중 안쪽의 원이 채워진 색상 (256:Black, 128:white) */
	DOUBLE				in_ring_scale_min;					/* 내부 링의 찾기 조건 값 (Scale 조건 값) 0.95 (0.5 ~ 1.0) */
	DOUBLE				in_ring_scale_max;					/* 내부 링의 찾기 조건 값 (Scale 조건 값) 1.05 (1.0 ~ 2.0) */
	DOUBLE				spec_angle[MAX_ALIGN_CAMERA];		/* 측정된 Camera가 설치된 회전 각도 값 (단위: degree) */
	DOUBLE				spec_pixel_um[MAX_ALIGN_CAMERA];	/* 측정된 Camera의 1 픽셀 크기 (단위: um)*/
	DOUBLE				bracket_size_w;						/* 얼라인 카메라를 지지하고 있는 구조물의 [폭] 크기 (센서까지 포함) (단위: mm) */
	DOUBLE				bracket_size_d;						/* 얼라인 카메라를 지지하고 있는 구조물의 [깊이; 두께] 크기 (센서까지 포함) (단위: mm) */

	/*
	 desc : 물리적(?)으로 떨어진 2 대의 카메라 간의 [중심] 오차 값 얻기
	 parm : dist_x	- [out] X 축으로 떨어진 거리 (단위: mm) (ACam.X1 - ACam.X2)
			dist_y	- [out] Y 축으로 떨어진 거리 (단위: mm) (ACam.Y1 - ACam.Y2)
	 retn : TRUE or FALSE
	 note : Align Camera가 반드시 2대 있어야만 함
	*/
	VOID GetACamCentDist(DOUBLE &dist_x, DOUBLE &dist_y)
	{
		dist_x	= quartz_acam[0] - quartz_acam[1];			/* 좌/우 길이 간격 (오차) */
		dist_y	= quartz_stage_y[0] - quartz_stage_y[1];		/* 상/하 길이 간격 (오차) */
	}
	DOUBLE GetACamCentDistX()	{	return	(quartz_acam[0] - quartz_acam[1]);			}	/* X 축 오차 */
	DOUBLE GetACamCentDistY()	{	return	(quartz_stage_y[0] - quartz_stage_y[1]);	}	/* Y 축 오차 */

	/*
	 desc : 물리적(?)으로 떨어진 2 대의 카메라 간의 [최소] 오차 값 얻기
			즉, 부딪히지 않을 최소한의 거리 (카메라 1번 센서와 카메라 2번 센서가 서로 닿지 않은 최소 간격)
	 parm : dist_x	- [out] X 축으로 떨어진 거리 (단위: mm) (ACam.X1 - ACam.X2)
			dist_y	- [out] Y 축으로 떨어진 거리 (단위: mm) (ACam.Y1 - ACam.Y2)
	 retn : TRUE or FALSE
	 note : Align Camera가 반드시 2대 있어야만 함
	*/
	DOUBLE GetACamMinDistH()	{	return	GetACamCentDistX()+bracket_size_w;		}	/* 센서 간의 부딪히질 않은 2대 카메라 간의 최소한의 떨어져야할 간격 */
	DOUBLE GetACamMinDistD()	{	return	bracket_size_d;							}	/* 센서 간의 부딪히질 않은 2대 카메라 간의 최소한의 떨어져야할 간격 */

	/*
	 desc : Pixel To MM 크기 반환
	 parm : cam_id	- [in]  Align Camera Index (Zero based)
	 retn : Pixel 크기를 um -> mm 반환
	*/
	DOUBLE GetPixelToMM(UINT8 cam_id)
	{
		if (MAX_ALIGN_CAMERA <= cam_id)	return 0.0f;
		return (spec_pixel_um[cam_id] / 1000.0f);
	}

}	STG_CASI,	*LPG_CASI;

/* Align Camera Focus Parameter */
typedef struct __st_config_acam_focus_info__
{
	DOUBLE				step_move_z;						/* Align Camera Up/Donw 방향으로 이동하기 위한 이동 간격 (단위: mm) (소수점 3자리까지 유효) */
	DOUBLE				mark2_stage_x;						/* Align Camera Specification 측정을 위한 QuartZ 기준 위치. Stage X (단위: mm) (소수점 4자리까지 유효) */
	DOUBLE				mark2_stage_y[MAX_ALIGN_CAMERA];	/* Align Camera Specification 측정을 위한 QuartZ 기준 위치 (Align Camera 1번 측정할 때). Stage Y (단위: mm) (소수점 4자리까지 유효) */
	DOUBLE				mark2_acam[MAX_ALIGN_CAMERA];		/* Align Camera Specification 측정을 위한 QuartZ 기준 위치. Camera X (단위: mm) (소수점 4자리까지 유효) */
	DOUBLE				model_dia_size;						/* 검색 대상의 Model 크기 (단위: um) (Model Type은 무조건 Circle) (크기 지름) */
	DOUBLE				acam_z_focus[MAX_ALIGN_CAMERA];		/* Align Camera 내에 설치된 Camera Z Axis 높이 값 (단위: mm) : 소재 기준 (SET_ALIGN -> DOF_FILM_THICK 기준 */

}	STG_CAFI,	*LPG_CAFI;

/* Trigger Set Parameter */
typedef struct __st_config_trigger_set_parameter__
{
	UINT8				port_no;							/* 시리얼 통신 포트 번호 2 ~ 19 */
	UINT8				ch_count;							/* 제품에 적용된 트리거 개수 (보통 짝수 개임) */
	UINT8				u8_reserved[7];
	INT32				trig_forward;						/* Area Trigger (1 ~ 16) 값에 이 값이 <증가/감소>되어, */
															/* Strobe와 Trigger의 신호가 빠르게/느리게 되어 출력 됨 (Trigger Count 값) */
	INT32				trig_backward;						/* Area Trigger (1 ~ 16) 값에 이 Register 값이 <감소>되어 */
															/* Strobe와 Trigger의 신호가 빠르게/느리게 되어 출력 됨 (Trigger Count 값) */
	UINT32				trig_on_time[MAX_TRIG_CHANNEL];		/* high가 유지되는 값을 입력 (단위: ns) */
	UINT32				strob_on_time[MAX_TRIG_CHANNEL];	/* high가 유지되는 값을 입력 (단위: ns) */
	UINT32				trig_delay_time[MAX_TRIG_CHANNEL];	/* 출력되는 Trigger의 Delay 값 입력 (단위: ns) */
															/* 이 Register 값이 0    이면, Strobe와 Trigger 신호는 동시에 출력 됨 */
															/* 이 Reigster 값이 1000 이면, Strobe에 신호가 출력되고, */
															/* 1000 ns 초 만큼 대기 (지연) 후에 Trigger에 신호가 출력 됨 */
	UINT32				trig_plus[MAX_TRIG_CHANNEL];		/* Area Trigger (1 ~ 16) 값에 이 Register 값이 <증가>되어, */
															/* Strobe와 Trigger의 신호가 지연되어 출력 됨 (단위: ns) */
															/* Strobe와 Trigger의 신호가 지연되어 출력 됨 */
	UINT32				trig_minus[MAX_TRIG_CHANNEL];		/* Area Trigger (1 ~ 16) 값에 이 Register 값이 <감소>되어 */
															/*  Strobe와 Trigger의 신호가 빠르게 출력 됨 (단위: ns) */
	/*
	 desc : Trigger Param Save
	 parm : cam_id			- [in]  Align Camera Index
			trig_time		- [in]  Trigger On Time (unit: ns)
			strob_time		- [in]  Strobe On Time (unit: ns)
			delay_time		- [in]  Trigger Delay Time (unit: ns)
			trig_forward	- [in]  Trigger Plus Time (unit: count)
			trig_backward	- [in]  Trigger Minus Time (unit: count)
	 retn : None
	*/
	VOID SetData(UINT8 cam_id, UINT32 trig_time, UINT32 strob_time,
				 UINT32 delay_time, INT32 forward=0, INT32 backward=0)
	{
		if (cam_id == 0x01)
		{
			trig_on_time[2]		= trig_on_time[0]	= trig_time;
			strob_on_time[2]	= strob_on_time[0]	= strob_time;
			trig_delay_time[2]	= trig_delay_time[0]= delay_time;
		}
		else
		{
			trig_on_time[3]		= trig_on_time[1]	= trig_time;
			strob_on_time[3]	= strob_on_time[1]	= strob_time;
			trig_delay_time[3]	= trig_delay_time[1]= delay_time;
		}
		trig_forward	= forward;
		trig_backward	= backward;
	}

}	STG_CTSP,	*LPG_CTSP;

/* Trigger Position for Align Mark */
typedef struct __st_config_mark_trigger_position__
{
	INT32				global[MAX_ALIGN_CAMERA][MAX_GLOBAL_MARKS];	/* Global Mark의 트리거 발생 위치 */
	INT32				local[MAX_ALIGN_CAMERA][MAX_LOCAL_MARKS];	/* Local  Makr의 트리거 발생 위치 */

}	STG_CMTP,	*LPG_CMTP;

/* 각종 파일 이름 */
typedef struct __st_config_file_data_name__
{
	TCHAR				led_power[MAX_FILE_LEN];
	TCHAR				job_recipe[MAX_FILE_LEN];
	TCHAR				expo_recipe[MAX_FILE_LEN];
	TCHAR				align_recipe[MAX_FILE_LEN];
	TCHAR				mark_model[MAX_FILE_LEN];		/* Mark Model */
	TCHAR				mark_recipe[MAX_FILE_LEN];		/* Mark Recipe */
	TCHAR				mark_roi[MAX_FILE_LEN];		/* Mark ROI */
	TCHAR				ph_step[MAX_FILE_LEN];
	TCHAR				acam_cali[MAX_FILE_LEN];
	TCHAR				trig_cali[MAX_FILE_LEN];
	TCHAR				thick_cali[MAX_FILE_LEN];
	TCHAR				correct_y[MAX_FILE_LEN];
	TCHAR				staticAcamCali[3][MAX_FILE_LEN];
}	STG_CFDN,	*LPG_CFDN;

/* Setup the Align Camera Device for Basler */
typedef struct __st_config_camera_device_basler__
{
	UINT8				cam_reverse_x:1;					/* 카메라 Grabbed Image 영상 반전 (좌/우) */
	UINT8				cam_reverse_y:1;					/* 카메라 Grabbed Image 영상 반전 (상/하) */
	UINT8				u8_bits_reserved:6;
	UINT8				u8_reserved[7];

	UINT8				cam_ip[MAX_ALIGN_CAMERA][4];		/* The Camera IPv4 for Basler */
	UINT8				cam_mc2_drv[MAX_ALIGN_CAMERA];		/* MC2 SD2S 기준 드라이브 번호 (보통 스테이지의 경우, 0, 1 번임) */
	UINT8				cam_gain_level[MAX_ALIGN_CAMERA];	/* Align Camera Gain Level 값 (0 ~ 255) */

	UINT16				acq_frames[MAX_ALIGN_CAMERA];		/* Sets the number of frames acquired in the multiframe acquisition mode */
	/* Sets the raw's expose time. (unit : us) (min : 16) */
	UINT32				grab_times[MAX_ALIGN_CAMERA];		/* GRAP_xx : PODIS App에서 사용될 카메라의 Expose Time */
	UINT32				step_times[MAX_ALIGN_CAMERA];		/* STEP_xx : VDOF App에서 사용될 카메라의 Expose Time */

	DOUBLE				z_axis_move_max;					/* Z Axis 이동 최소 / 최대 값 즉, 0 : Min, 1 : Max (단위: mm) */
	DOUBLE				z_axis_move_min;					/* Z Axis 이동 최소 / 최대 값 즉, 0 : Min, 1 : Max (단위: mm) */

}	STG_CCDB,	*LPG_CCDB;

/* Photohead Step XY 정보 */
typedef struct __st_config_phothead_offset_value__
{
	UINT8				scroll_mode;						/* 광학계 단차 측정에 사용된 소재의 노광 속도 모드 (1 ~ 7)					*/
	UINT8				ph_scan;							/* 광학계마다 노광할 때 상/하로 움직이는 Scan 개수 즉, Stripe 총 개수		*/
	UINT8				max_ph_step;						/* 광학계 단차 레시피에 최대 등록 가능한 레시피 개수 */
	UINT8				expose_round;						/* 단차 확인을 위한 현재 노광 횟수 */
	UINT8				u8_reserved[4];
	UINT32				model_type;							/* Circle (8), Ellipse (16), Square (32), Rectangle (64), Ring (256),		*/
															/* Cross (8192), Diamond (32768), Triangle (65536), Image (?????)			*/
	UINT32				u32_reserved;
	DOUBLE				model_color;						/* Black (256), White (128)													*/
	DOUBLE				model_dia_size;						/* 검색된 엣지 객체들 중에서 이 설절 값과 가장 가까운 값을 인정 (단위: mm)	*/
															/* (소수점 3자리까지 유효) (검색 대상은 무조건 원. 크기는 !!! 지름 !!!)		*/
	DOUBLE				start_xy[2];						/* 맨 처음 측정할 마크가 있는 모션 위치 (단위: mm) (소수점 4자리까지 유효)	*/
	DOUBLE				start_acam_x;						/* 맨 처음 측정할 마크가 있는 모션 위치 (단위: mm) (소수점 4자리까지 유효)	*/
	DOUBLE				stripe_width;						/* 광학계가 한 번 노광할 때마다 그려지는 STRIPE의 크기 (단위: mm)			*/
	DOUBLE				mark_period;						/* 2 개의 Mark 간의 떨어진 간격 즉, 인접한 원의 중심 간의 거리(단위: mm)	*/
	DOUBLE				center_offset;						/* 이 값이 클수록 빨리 검색하나, 정확도는 떨어짐. 최소 0.1 mm 이상 (단위:mm)*/

	DOUBLE				y_pos_plus;							/* 단차 측정 동작을 위한 Y축 거리 (단위:mm) */

}	STG_CPOV,	*LPG_CPOV;

/* DI 장비 내부의 온도 및 범위 */
typedef struct __st_di_internal_temp_range__
{
	DOUBLE				hot_air[2];			/* 기준 온도 및 오차 범위 (온조기) */
	DOUBLE				di_internal[2];		/* 기준 온도 및 오차 범위 (DI 장비 내부) */
	DOUBLE				optic_led[2];		/* 기준 온도 및 오차 범위 (광학계 LED) */
	DOUBLE				optic_board[2];		/* 기준 온도 및 오차 범위 (광학계 보드) */

	/*
	 desc : 임계치 최대 값 반환
	 parm : mode	- [in]  0x00: 기준 값, 0x01 : 최소 값, 0x02 : 최대 값
			type	- [in]  0x00:온조기, 0x01:di internal, 0x02: led, 0x03: board
	 retn : 설정 온도 값 반환
	*/
	DOUBLE GetTempDeg(UINT8 mode, UINT8 type)
	{
		DOUBLE *pTemp	= NULL;

		switch (type)
		{
		case 0x00:	pTemp	= hot_air;		break;	/* 온조기 */
		case 0x01:	pTemp	= di_internal;	break;	/* DI Internal */
		case 0x02:	pTemp	= optic_led;	break;	/* PH LED */
		case 0x03:	pTemp	= optic_board;	break;	/* PH Board */
		}
		if (!pTemp)	return 0.0f;

		/* 기준 값, 최소 값, 최대 값 */
		switch (mode)
		{
		case 0x00	:	return pTemp[0];
		case 0x01	:	return pTemp[0] - pTemp[1];
		case 0x02	:	return pTemp[0] + pTemp[1];
		}

		return 0.0f;
	}

}	STG_DITR,	*LPG_DITR;

/* Global Transformation Information */
typedef struct __st_config_global_transformation_information__
{
	UINT8				use_trans;			/* 0 : 아래 파라미터 사용 안함, 1 : 아래 파라미터 사용함 */
	UINT8				use_rectangle;		/* 0 : 사용하지 않음, 1 : 사용함 */
	UINT8				use_rotation_mode;	/* Rotation0x00:Auto, 0x01: Fixed */
	UINT8				use_scaling_mode;	/* Scaling, 0x00:Auto, 0x01: Fixed */
	UINT8				use_offset_mode;	/* Offset, 0x00:Auto, 0x01: Fixed */
	UINT8				u8_reserved[3];

	INT32				rotation;			/* unit : um degree */
	INT32				offset[2];			/* X/Y Offset (unit: nm) */
	INT32				i32_reserved;

	UINT32				scaling[2];			/* X/Y Scaling (factor * 10^6) */

	BOOL IsSetTransValue()
	{
		return (use_rotation_mode != 0 || use_scaling_mode != 0 || use_offset_mode != 0);
	}

}	STG_CGTI,	*LPG_CGTI;

/* PODIS 공통 정보 */
typedef struct __st_config_info_podis_common__
{
	UINT8				check_hot_air:1;		/* 0:온조기 동작 확인하지 않음. 1:온조기 동작여부 확인 */
	UINT8				check_chiller:1;		/* 0:칠러 동작 확인하지 않음. 1:칠러 동작여부 확인 */
	UINT8				check_vacuum:1;			/* 0:베큠 동작 확인하지 않음. 1:베큠 동작여부 확인 */
	UINT8				check_material:1;		/* 0:노광 소재 존재 확인하지 않음. 1:노광 소재 존재여부 확인 */
	UINT8				check_start_led:1;		/* 0:장비 전면 Start LED 버튼 확인하지 않음 1:동작 확인함 */
	UINT8				check_query_expo:1;		/* 최종 얼라인 진행 후 노광 진행 여부 물어보기 설정 여부 */
												/* 0:물어보는 것 없이 진행, 1:한번더 노광을 진행할 것인지 물어보기 */
	UINT8				use_vision_lib:1;		/* 0 : Vision Library 사용 안함, 1 : Vision Library 사용 함 */
	UINT8				load_recipe_homing:1;	/* 레시피 적재할 때, 광학계 Homing 할지 여부 (0 : Not homing, 1 : Homing) */
	UINT8				uu_reserved[7];
	TCHAR				recipe_name[MAX_FILE_LEN];	// by sysandj : 현재 RECIPE 저장
}	STG_CIPC,	*LPG_CIPC;

/* 온조기 온도 자동 제어 */
typedef struct __st_config_hotair_temp_control__
{
	UINT8				use_auto_hot_air;		/* 0:온조기 온도 자동 제어 안함. 1:온조기 온도 자동 제어 함 */
	UINT8				auto_temp_rate;			/* ABS(22.0 ℃ - 25.0 ℃) = 3.0 ℃ 로, 여기서 3.0 ℃ 값에, 이 설정 값 (백분위; %)을 */
												/* 곱해서, 온조기 설정 (기준)값에 더하거나 빼주면 됨 */
												/* ex> [TEMP_RANGE] -> HOT_DI_DEV_TEMP (22.0 ℃), DI 현재 내부 온도 값 (25.0 ℃)이고, */
												/*     AUTO_TEMP_RATE 값이 50 % 이라면 => abs(21.5 - 24.5) = 3.0 ℃ 이고, */
												/*     3.0 ℃ x 0.5 (50%) = 1.5 ℃ 이므로, DI 내부 온도가 올라가 있으므로, */
												/*     최종 온조기 설정 값은 21.5 ℃ - 1.5 ℃ = 20.0 ℃ 로 설정 됨. */
	UINT8				temp_holding_time;		/* DI 내부 온도 차이 값이 이 (TEMP_MONITOR_TIME)시간 (단위: minute)동안 유지되면, */
												/* 온조기 온도 자동 제어 (최대 값 60) */
												/* ex> TEMP_MONITOR_TIME 값이 5 min로 설정되어 있다면,  */
												/*     [TEMP_RANGE] -> HOT_DI_DEV_TEMP 값과 현재 DI 내부 온도 값이 3.0 ℃ 이상 */
												/*     차이가 5 min 이상동안 유지되면, 온조기 온도 설정이 자동으로 제어 됨 */
	UINT8				u8_reserved[5];
	DOUBLE				auto_temp_range;		/* [TEMP_RANGE]->HOT_DI_DEV_TEMP 설정 온도 값과 현재 DI 내부 온도 값의 */
												/* 차이가 이 (AUTO_TEMP_RANGE)값 보다 크면, 자동 제어 수행 함 (최대값 10. 실수) */
												/* ex> [TEMP_RANGE] -> HOT_DI_DEV_TEMP 값이 22.0 ℃, */
												/*     [AUTO_HOT_AIR] -> AUTO_TEMP_RANGE 값이 2.0 ℃ 로 설정된 상태로 가정. */
												/*     현재 DI 내부 실시간 온도가 25.0 ℃ 라면, ABS(22.0 ℃ - 25.0 ℃) = 3.0 ℃ 로, */
												/*     [TEMP_RANGE] -> HOT_DI_DEV_RANGE 설정 (2.0 ℃) 보다 크므로, 자동 제어 수행 */

}	STG_CHTC,	*LPG_CHTC;


/* Measure Flatness Parameter */
typedef struct __st_config_measure_auto_flatness__
{
private:
	std::mutex mtx;
public:

	UINT8 u8UseMotorT;							/* 0:해당 축 모터 사용안함. 1:해당 축 모터 사용함 */
	UINT8 u8UseMotorZ;							/* 0:해당 축 모터 사용안함. 1:해당 축 모터 사용함 */

	DOUBLE dSpeedX;								/* X 이동 속도 */
	DOUBLE dSpeedY;								/* Y 이동 속도 */
	DOUBLE dSpeedT;								/* T 이동 속도 */
	DOUBLE dSpeedZ;								/* Z 이동 속도 */
	DOUBLE dStartXPos;							/* X축 시작 위치 */
	DOUBLE dStartYPos;							/* Y축 시작 위치 */
	DOUBLE dStartTPos;							/* T축 시작 위치 */
	DOUBLE dStartZPos;							/* Z축 시작 위치 */
	DOUBLE dEndXPos;							/* X축 종료 위치 */
	DOUBLE dEndYPos;							/* Y축 종료 위치 */
// 	DOUBLE dIntervalX;
// 	DOUBLE dIntervalY;

	UINT8 u8CountOfXLine;						/* X축 방향 측정 횟수 */
	UINT8 u8CountOfYLine;						/* Y축 방향 측정 횟수 */
	UINT8 u8CountOfMeasure;						/* 한 위치 측정 시 측정 횟수 */
	UINT16 u16DelayTime;						/* 각 측정 시 지연 시간 */

	BOOL bThieckOnOff;							/*LDS 측정 동작 실행 On/Off*/
	DOUBLE dRangStartYPos;						/*LDS 측정 Y축 가능한 시작 위치*/
	DOUBLE dRangEndYPos;						/*LDS 측정 Y축 가능한 끝 위치*/
	
	UINT8	u8UseThickCheck;					/*LED 측정 동작 사용 유무 확인*/
	DOUBLE	dOffsetZPOS;						/*LDS 실제 측정값에 대한 보정값*/
	DOUBLE	dLimitZPOS;							/*LDS 실제 측정값과 설정값에 대한 최대 오차값*/

	vector<DOUBLE>* dAlignMeasure;						/*LDS 측정 값*/
	

	void SetThickMeasureResult(DOUBLE d)
	{
		std::lock_guard<std::mutex> lock(mtx);
		dAlignMeasure->push_back(d);
	}

	DOUBLE GetThickMeasure()
	{
		std::lock_guard<std::mutex> lock(mtx);
		return dAlignMeasure->empty() ? 0 : dAlignMeasure->back();
	}

	DOUBLE GetThickMeasureMean()
	{
		std::lock_guard<std::mutex> lock(mtx);
		int measureCnt = dAlignMeasure->size();
		if (measureCnt == 0) return 0;

		std::sort(dAlignMeasure->begin(), dAlignMeasure->end());
		return dAlignMeasure->at(measureCnt >> 1); // 중간값 반환
	}

	void MeasurePoolClear()
	{
		std::lock_guard<std::mutex> lock(mtx);
		dAlignMeasure->clear();
		
	}



}	STG_CMAF, * LPG_CMAF;


/* philhmi Set Parameter */
typedef struct __st_config_philhmi_set_parameter__
{
	

}	STG_CPSP, * LPG_CPSP;


/* Strobe Lamp Set Parameter */
typedef struct __st_config_strobe_lamp_set_parameter__
{
	UINT16 u16StrobeValue[8];
	UINT16 u16BufferValue[8];


}	STG_CSSP, * LPG_CSSP;


/* Strobe Lamp Set Parameter */
typedef struct __st_config_keyence_lds_set_parameter__
{


}	STG_KLSP, * LPG_KLSP;


/* Engine 관련 정보 */
typedef struct __st_config_info_engine_all__
{
	STG_CIPC			set_uvdi15;		/* UVDI15 공통 정보 */
	STG_CLED			line_find;
	STG_CFDN			file_dat;
	STG_CGIP			grab_proc;
	STG_CLSI			luria_svc;
	STG_CMED			edge_find;
	STG_CVMF			mark_find;
	STG_CMSI			mc2_svc;
	STG_CMSI			mc2b_svc;
	STG_CSDM			monitor;
	STG_CLPI			led_power;
	STG_CPFI			ph_focus;
	STG_CAFI			acam_focus;
	STG_CASI			acam_spec;
	STG_CACC			acam_cali;
	STG_CSMS			stage_st;
	STG_CPSI			melsec_q_svc;
	STG_CSAI			set_align;
	STG_CCDB			set_basler;
	STG_CSCI			set_cams;
	STG_CCSI			set_comm;
	STG_CCGS			set_comn;
	STG_CTSP			trig_grab;
	STG_CTSP			trig_step;
	STG_CMTP			mark_trig;		/* mark마다 트리거가 발생될 위치 값 저장 */
	STG_CPOV			ph_step;
	STG_DITR			temp_range;
	STG_GMLV			mark_diff;		/* 6 곳 (LT-RT, LB-RB, LT-LB, RT-RB, LT-RB, LB-RT) 영역의 길이 정보 저장 */
	STG_CGTI			global_trans;
	STG_CHTC			auto_hot_air;	/* 온조기 자동 제어 */
	STG_CCSV			cmps_sw;
	STG_CPSP			set_philhmi;		// by sysandj : philhmi 관련 파라메터
	STG_CSSP			set_strobe_lamp;	// by sysandj : strobe lamp 관련 파라메터
	STG_KLSP			set_keyence_lds;	// 230919 mhbaek Add keyence lds 관련 파라메터
	STG_CMAF			measure_flat;		// 230919 mhbaek Add

	/*
	 desc : Align Camera의 Grabbed Image의 넓이 or 높이 반환
	 parm : flag	- [in]  0x00: Width, 0x01: Height
	 retn : 캡처되는 이미지의 크기 반환 (단위: pixel)
	*/
	UINT32 GetACamGrabSize(UINT8 flag)
	{
		UINT32 u32Size	= 0;
		switch (ENG_ERVM(set_comn.engine_mode))
		{
		case ENG_ERVM::en_cmps_sw	:	if (!flag)	u32Size	= set_cams.aoi_size[0];
										else		u32Size	= set_cams.aoi_size[1];	break;
		case ENG_ERVM::en_cali_step	:	if (!flag)	u32Size	= set_cams.soi_size[0];
										else		u32Size	= set_cams.soi_size[1];	break;
		case ENG_ERVM::en_cali_vdof	:	if (!flag)	u32Size	= set_cams.doi_size[0];
										else		u32Size	= set_cams.doi_size[1];	break;
		case ENG_ERVM::en_cali_meas	:	if (!flag)	u32Size	= set_cams.mes_size[0];
										else		u32Size	= set_cams.mes_size[1];	break;
		case ENG_ERVM::en_cali_exam	:	if (!flag)	u32Size	= set_cams.ioi_size[0];
										else		u32Size	= set_cams.ioi_size[1];	break;
		case ENG_ERVM::en_acam_spec	:	if (!flag)	u32Size	= set_cams.spc_size[0];
										else		u32Size	= set_cams.spc_size[1];	break;
		case ENG_ERVM::en_hole_size	:	if (!flag)	u32Size	= set_cams.hol_size[0];
										else		u32Size	= set_cams.hol_size[1];	break;
		}
		return u32Size;
	}

	/*
	 desc : Align Camera의 Grabbed Image의 넓이 or 높이 설정
	 parm : flag	- [in]  0x00: Width, 0x01: Height
			size	- [in]  이미지 크기 (단위: Pixel)
	 retn : None
	*/
	UINT32 SetACamGrabSize(UINT8 flag, UINT32 size)
	{
		UINT32 u32Size	= 0;
		switch (ENG_ERVM(set_comn.engine_mode))
		{
		case ENG_ERVM::en_cmps_sw	:	if (!flag)	set_cams.aoi_size[0] = size;
										else		set_cams.aoi_size[1] = size;	break;
		case ENG_ERVM::en_cali_step	:	if (!flag)	set_cams.soi_size[0] = size;
										else		set_cams.soi_size[1] = size;	break;
		case ENG_ERVM::en_cali_vdof	:	if (!flag)	set_cams.doi_size[0] = size;
										else		set_cams.doi_size[1] = size;	break;
		case ENG_ERVM::en_cali_exam	:	if (!flag)	set_cams.ioi_size[0] = size;
										else		set_cams.ioi_size[1] = size;	break;
		case ENG_ERVM::en_cali_meas	:	if (!flag)	set_cams.mes_size[0] = size;
										else		set_cams.mes_size[1] = size;	break;
		case ENG_ERVM::en_acam_spec	:	if (!flag)	set_cams.spc_size[0] = size;
										else		set_cams.spc_size[1] = size;	break;
		case ENG_ERVM::en_hole_size	:	if (!flag)	set_cams.hol_size[0] = size;
										else		set_cams.hol_size[1] = size;	break;
		}
		return u32Size;
	}

	/*
	 desc : Align Camera의 Grabbed Image의 크기 반환
	 parm : None
	 retn : 캡처되는 이미지의 크기 반환 (단위: bytes)
	*/
	UINT32 GetACamGrabBytes()
	{
		UINT32 u32Size	= 0;
#if 1
		switch (ENG_ERVM(set_comn.engine_mode))
		{
		case ENG_ERVM::en_cmps_sw	:	u32Size	= set_cams.aoi_size[0] * set_cams.aoi_size[1];	break;
		case ENG_ERVM::en_cali_step	:	u32Size	= set_cams.soi_size[0] * set_cams.soi_size[1];	break;
		case ENG_ERVM::en_cali_vdof	:	u32Size	= set_cams.doi_size[0] * set_cams.doi_size[1];	break;
		case ENG_ERVM::en_cali_meas	:	u32Size	= set_cams.mes_size[0] * set_cams.mes_size[1];	break;
		case ENG_ERVM::en_cali_exam	:	u32Size	= set_cams.ioi_size[0] * set_cams.ioi_size[1];	break;
		case ENG_ERVM::en_acam_spec	:	u32Size	= set_cams.spc_size[0] * set_cams.spc_size[1];	break;
		case ENG_ERVM::en_hole_size	:	u32Size	= set_cams.hol_size[0] * set_cams.hol_size[1];	break;
		}
#else
		switch (set_comn.engine_mode)
		{
		case 0x01/*en_vorm_cmps*/	:	u32Size	= WIDTHBYTES(1, set_cams.aoi_size[0]) * set_cams.aoi_size[1];	break;
		case 0x02/*en_vorm_step*/	:	u32Size	= WIDTHBYTES(1, set_cams.soi_size[0]) * set_cams.soi_size[1];	break;
		case 0x03/*en_vorm_vdof*/	:	u32Size	= WIDTHBYTES(1, set_cams.doi_size[0]) * set_cams.doi_size[1];	break;
		case 0x04/*en_vorm_exam*/	:	u32Size	= WIDTHBYTES(1, set_cams.ioi_size[0]) * set_cams.ioi_size[1];	break;
		}
#endif
		return u32Size;
	}

	/*
	 desc : 데모 (Emulation Mode) 모드로 동작하는지 여부
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsRunDemo()
	{
		return set_comn.run_emulate_mode ? TRUE : FALSE;
	}

	/*
	 desc : 각종 장치 동작 확인 여부
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsCheckHotAir()	{	return set_uvdi15.check_hot_air ? TRUE : FALSE;		}
	BOOL IsCheckChiller()	{	return set_uvdi15.check_chiller ? TRUE : FALSE;		}
	BOOL IsCheckVacuum()	{	return set_uvdi15.check_vacuum ? TRUE : FALSE;		}
	BOOL IsCheckStartLed()	{	return set_uvdi15.check_start_led ? TRUE : FALSE;	}
	/*
	 desc : 노광 소재 존재 확인 여부
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsCheckMaterial()	{	return set_uvdi15.check_material ? TRUE : FALSE;		}


}	STG_CIEA,	*LPG_CIEA;



#pragma pack (pop)	// 8 bytes

/* 반드시 1 bytes 정렬 (Shared Memory 때문에 1 bytes 정렬 불가피) */
#pragma pack (push, 1)


#pragma pack (pop)	// 1 bytes