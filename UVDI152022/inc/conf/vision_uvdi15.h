
/*
 desc : Structure
*/

#pragma once

#include "global.h"
#include "vision_comn.h"
#include "conf_comn.h"		//버젼 에러 이슈

#define MARK_MODEL_NAME_LENGTH					256		/* Align Mark Model Name 길이 */
#define MAX_MODEL_REGIST_PARAM					5		/* 모델 별로 최대 등록 가능한 파라미터 개수 */
#define	MAX_REGIST_MODEL						48		/* 최대 등록 가능한 모델 개수 */

/* --------------------------------------------------------------------------------------------- */
/*                                             열거형                                            */
/* --------------------------------------------------------------------------------------------- */

/* Mark Model Search Method */
typedef enum class __en_mark_model_search_method__ : UINT8
{
	en_none			= 0x00,

	en_single		= 0x01,			/* 단일 마크 검색 방법 */
	en_cent_side	= 0x02,			/* 중심의 마크 (any shape) 1개와 주변의 다점 마크 (any shape)로 검색하는 방법 */
	en_multi_only	= 0x03,			/* 주변의 다점 마크 (any shape)로 검색하는 방법 */
	en_ring_circle	= 0x04,			/* 외곽에 링(도넛) 마크와 내부에 원 (Circle)로 구성된 마크 검색하는 방법 */
	en_ph_step		= 0x05,			/* 광학계 간의 단차 측정에 필요함 */

}	ENG_MMSM;

/* Capture Mode */
typedef enum class __en_vision_cam_capture_mode__ : UINT8
{
	en_none			= 0x00,
	en_live_mode	= 0x01,			/* Vision Camera Live Stream Mode */
	en_grab_mode	= 0x02,			/* Vision Camera Grabbed Mode */
	en_cali_mode	= 0x03,			/* Vision Camera Calibration Mode */
	en_edge_mode	= 0x04,			/* Vision Camera Edge Detection Mode */
	en_line_mode	= 0x05,			/* Vision Camera Line Detection Mode */
	en_hole_mode	= 0x06,			/* Vision Camera Measurement (hole) Mode */
	en_image_mode	= 0x07,			/* Vision Image Load Mode */

}	ENG_VCCM;



typedef enum class __en_vision_cam_trigger_mode__ : UINT8
{
	en_mode_none = 0x00,
	en_line_mode = 0x01,
	en_Sw_mode = 0x02,

}	ENG_TRGM;

/* --------------------------------------------------------------------------------------------- */
/*                                             구조체                                            */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 8)

/* Align Camera Grabbed Image Result */
typedef struct __st_align_camera_grab_result__
{
	// Grabbed Image Information			
	UINT8				marked;					/* 마크 검색 성공 여부. 0x00 - Not run, 0x01 - Success, 0x02 - Failed */
	UINT8				cam_id;					/* Camera Index (1 based) */
	UINT8				img_id;					/* Grabbed Image의 Index (0 or Later) , 0,1 : Global Mark, 그 외 : Local Mark */
	UINT8				model_index;			/* 1 개의 Grabbed Image에서 1개 이상 검색된 Mark 들 중 등록된 Mark의 Model Index (0x00 ~ ). 1개 검색되면 0x00 */
	UINT8				valid_score;			/* 환경 파일에 설정된 Score 값보다 큰 경우인지 여부 */
	UINT8				valid_scale;			/* 환경 파일에 설정된 Scale 값보다 큰 경우인지 여부 */
	UINT8				valid_multi;			/* Mulit-Mixed Type이 유효한 마크인지 여부 */
	UINT8				manual_set;				/* 강제로 Align Mark 값을 수동 (화면 Touch)으로 설정했는지 여부 */
	UINT8				mark_method;			/* ENG_MMSM 값 */
	UINT8				scale_size;				/* 0x00:Size Equal, 0x01: Size (Scale) Up, 0x02: Size (Scale) Down */
	UINT8				u8_reserved[6];

	UINT32				cent_dist_x;			/* ENG_MMSM != en_mmsm_multi_side만 유효(기본 마크의 중심과 주변 마크들의 사각형 중심 간의 떨어진 거리. 단위: um) */
	UINT32				cent_dist_y;			/* ENG_MMSM != en_mmsm_multi_side만 유효(기본 마크의 중심과 주변 마크들의 사각형 중심 간의 떨어진 거리. 단위: um) */
	UINT32				grab_size;				/* Grabbed Image의 크기 (단위: Bytes) */
	UINT32				grab_width;				/* Grabbed Image의 넓이 (단위: Pixel) */
	UINT32				grab_height;			/* Grabbed Image의 높이 (단위: Pixel) */
	UINT32				u32_reserved;
	// Mark 검색 결과 값 - pixel
	DOUBLE				mark_cent_px_x;			/* 검색된 Mark의 중심 (단위: pixel) */
	DOUBLE				mark_cent_px_y;			/* 검색된 Mark의 중심 (단위: pixel) */
	DOUBLE				mark_width_px;			/* 검색된 Mark Image Size (단위: pixel) */
	DOUBLE				mark_height_px;			/* 검색된 Mark Image Size (단위: pixel) */
	// Mark 검색 결과 값 - mm
	DOUBLE				mark_cent_mm_x;			/* 검색된 Mark의 중심 */
	DOUBLE				mark_cent_mm_y;			/* 검색된 Mark의 중심 */
	DOUBLE				mark_cent_mm_dist;		/* Grabbed Image의 중심에서 검색된 Mark Image의 중심 간의 거리 (양수, 0, 음수 값 존재) */
	DOUBLE				mark_width_mm;			/* 검색된 Mark Image Size */
	DOUBLE				mark_height_mm;			/* 검색된 Mark Image Size */
	DOUBLE				mark_angle;				/* 검색된 Mark Image Rotate (단위: 도) */
	DOUBLE				score_rate;				/* 매칭 검색률 (단위: %) */
	DOUBLE				scale_rate;				/* 매칭된 이미지의 스케일 (배율) (단위: %) */
	DOUBLE				square_rate;			/* 이 값 100에 가까워야 정사각형에 가까움 */
	DOUBLE				coverage_rate;			/* 총 검색된 Edge들의 비율 값 (단위: %) */
	DOUBLE				fit_error;				/* 검색된 엣지와 모델 엣지 간의 2차원 거리 에러 값 (0.0에 가까울 수록 비슷함) */
	/* 즉, 검색된 엣지가 어느 정도 모델의 엣지에 대응하는지를 나타냄 */
	DOUBLE				circle_radius;			/* circle 반지름 */
	/* ----------------------------------------------------------------------------------------- */
	/* 아래 2개의 변수는 중요함. 추후 제어PC에게 Align Mark 좌표 값 넘겨주기 위함. 단위는 mm 임. */
	/* move_x_mm : 양수 값인 경우, 소재가 X 축으로 축소, 음수 값인 경우는 소재가 X 축으로 확대됨 */
	/* move_y_mm : 양수 값인 경우, 소재가 Y 축으로 축소, 음수 값인 경우는 소재가 Y 축으로 확대됨 */
	/* ----------------------------------------------------------------------------------------- */
	DOUBLE				move_px_x;
	DOUBLE				move_px_y;
	DOUBLE				move_mm_x;
	DOUBLE				move_mm_y;
	DWORD				grabTime;
	int					fiducialMarkIndex;
	UINT32				reserve;
	PUINT8				grab_data;				/* Grabbed Image Buffer */

	//LPG_GMSR			submaskFindInfo;

 
	BOOL IsEqualData(LPG_AMDR data)
	{
		return (move_mm_x == data->move_mm_x &&
				move_mm_y == data->move_mm_y &&
				score_rate== data->score_rate);
	}

	/*
	 desc : 데이터 유효 여부
	 parm : multi_mark	- [in]  Multi Mark (다중 마크) 적용 여부
	 retn : TRUE or FALSE
	*/
	BOOL IsMarkValid(BOOL multi_mark=FALSE)
	{
		return (valid_scale && valid_score && (!multi_mark || valid_multi));
	}

	/*
	 desc : 강제로 Mark 유효 여부 결정
	 parm : flag	- [in]  0x00 - 유효하지 않음, 0x01 - 유효 함
	 retn : None
	*/
	VOID SetMarkValid(UINT8 flag, BOOL multi_mark=FALSE)
	{
		valid_score	= flag;
		valid_scale	= flag;
		if (multi_mark)	valid_multi	= flag;
	}

	/*
	 desc : 현재 이미지 데이터가 존재하는지 여부
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsEmpty()
	{
		return !(grab_size > 0 && grab_width > 0 && grab_height > 0);	/* Check to Grabbed Image Size */
	}

	void Init()
	{
		grab_size = 0;
		grab_width = 0;
		grab_height = 0;
		marked = 0;
		cam_id = 0;
		img_id = 0;
		model_index = 0;
		valid_score = 0;
		valid_scale = 0;
		valid_multi = 0;
		manual_set = 0;
		mark_method = 0;
		scale_size = 0;
		u32_reserved = 0;
		mark_cent_px_x = 0.0;
		mark_cent_px_y = 0.0;
		mark_width_px = 0.0;
		mark_height_px = 0.0;
		mark_cent_mm_x = 0.0;
		mark_cent_mm_y = 0.0;
		mark_cent_mm_dist = 0.0;
		mark_width_mm = 0.0;
		mark_height_mm = 0.0;
		mark_angle = 0.0;
		score_rate = 0.0;
		scale_rate = 0.0;
		square_rate = 0.0;
		coverage_rate = 0.0;
		fit_error = 0.0;
		circle_radius = 0.0;
		move_px_x = 0.0;
		move_px_y = 0.0;
		move_mm_x = 0.0;
		move_mm_y = 0.0;
		grabTime = 0;
		reserve = 0;
		fiducialMarkIndex = -1818;
		grab_data = nullptr;
	}

	void FixOffsets(double offsetX, double offsetY,bool overWrite = false)
	{
		this->move_mm_x = overWrite ? offsetX : this->move_mm_x + offsetX;
		this->move_mm_y = overWrite ? offsetY : this->move_mm_y + offsetY;
	}

	/* 생성자 */
	__st_align_camera_grab_result__()
	{
		Init();
	}
	
	void Release()
	{
		//if (grab_data == nullptr) return;
		//delete[]grab_data;
		//grab_data = nullptr;
		/*if (IsEmpty() == false)
		{
			::free(grab_data);
		}*/
		//grab_data = nullptr;
	}

	~__st_align_camera_grab_result__()
	{
		Release();
	}


}	STG_ACGR,	*LPG_ACGR;

/* Edge Detection Find Results */
typedef struct __st_edge_detect_find_results__
{
	UINT8				set_data;				/* 0x00 : None data, 0x01 : Set Data */
	UINT8				u8_reserved[7];
	DOUBLE				dia_meter_px;			/* 직경 크기 (단위: pixel) */
	DOUBLE				dia_meter_um;			/* 직경 크기 (단위: um) */
	DOUBLE				width_um;				/* 넓이 크기 (단위: um) */
	DOUBLE				height_um;				/* 넓이 크기 (단위: um) */
	DOUBLE				angle;					/* 검색된 객체의 회전 각도 */
	DOUBLE				strength;				/* 평균 강도 (?) */
	DOUBLE				length;					/* 엣지들이 연결된 선분들의 총 길이 (?) */
	RECT				circle_area;			/* 검색된 영역 (단위: pixel) */
	RECT				box_area;				/* 검색된 영역 (단위: pixel) */

	/*
	 desc : 검색된 영역의 넓이 값 반환
	 parm : None
	 retn : 넓이 값 (단위: pixel)
	*/
	INT32 GetBoxWidth()
	{
		return box_area.right - box_area.left;
	}

	/*
	 desc : 검색된 영역의 높이 값 반환
	 parm : None
	 retn : 넓이 값 (단위: pixel)
	*/
	INT32 GetBoxHeight()
	{
		return box_area.bottom - box_area.top;
	}

	/*
	 desc : 검색된 영역의 넓이 값 반환
	 parm : None
	 retn : 넓이 값 (단위: pixel)
	*/
	INT32 GetCircleWidth()
	{
		return circle_area.right - circle_area.left;
	}

	/*
	 desc : 검색된 영역의 높이 값 반환
	 parm : None
	 retn : 넓이 값 (단위: pixel)
	*/
	INT32 GetCircleHeight()
	{
		return circle_area.bottom - circle_area.top;
	}

}	STG_EDFR,	*LPG_EDFR;

typedef struct __st_geomatric_matching_find_result__
{
	UINT8				valid_multi;	/* Multi-Mixed Type이 유효한 마크인지 여부 */
	UINT8				mark_method;	/* ENG_MMSM 값 */
	UINT8				scale_size;		/* 0x00:Size Equal, 0x01: Size (Scale) Up, 0x02: Size (Scale) Down */
	UINT8				u8_reserved[7];
	UINT32				cent_dist_x;	/* Mixed Mark에서 기본 마크의 중심과 주변 마크들의 중심 위치 간의 떨어진 거리 (단위: um) */
	UINT32				cent_dist_y;	/* Mixed Mark에서 기본 마크의 중심과 주변 마크들의 중심 위치 간의 떨어진 거리 (단위: um) */
	UINT32				model_index;	/* ModDefine 함수에 등록된 모델로 검색되었다는 모델 인덱스 (Zero-based) */
	UINT32				mark_width;		/* Mark Size = Width (Pixel) */
	UINT32				mark_height;	/* Mark Size = Height (Pixel) */
	UINT32				u32_reserved;
	DOUBLE				scale_rate;		/* 검색된 이미지의 스케일 비율 값 (단위: percent) */
	DOUBLE				score_rate;		/* 검색률 (단위: percent) */
	DOUBLE				cent_dist;		/* 이미지 중심과의 떨어진 거리 (unit : pixel) */
	DOUBLE				square_rate;	/* 정사각형 비율 값 (이 값이 100에 가까워야 정사각형에 가까움) */
	DOUBLE				r_angle;		/* 검색된 각도 */
	DOUBLE				coverage_rate;	/* 전체 Edge들의 총 비율 값 (단위: percent) */
	DOUBLE				fit_error;		/* 0.0에 가까울 수록 검색 대상 모델과 비슷함 (모델 엣지와 검색된 엣지 간의 2차원 거리 에러 값) */
										/* 즉, 검색된 엣지가 어느 정도 모델의 엣지에 대응하는지를 나타냄 */
	DOUBLE				circle_radius;	/* circle 반지름 */

	DOUBLE				cent_x;			/* 이미지에서 검색된 마크의 중심 위치 : X (unit : pixel) */
	DOUBLE				cent_y;			/* 이미지에서 검색된 마크의 중심 위치 : Y (unit : pixel) */
	DOUBLE				diameter;		/* 지름 길이 : pixel */

	/*
	 desc : 가로/세로 마크의 크기 중 가장 큰 값 반환
	 parm : None
	 retn : 크기 반환 (단위: Pixel)
	*/
	UINT32 GetMarkSize()
	{
		return mark_width > mark_height ? mark_width : mark_height;
	}

	/*
	 desc : 복합 얼라인 마크의 경우, Hole과 Laser 간의 실제 떨어진 길이 (단위: um)
	 parm : None
	 retn : 떨어진 거리 (대각선 길이)
	*/
	UINT32 GetCentDistLen()
	{
		return (UINT32)ROUNDED(abs(sqrt(pow(cent_dist_x, 2) + pow(cent_dist_y, 2))), 0);
	}

	/*
	 desc : 구조체 값 초기화
	 parm : None
	 retn : None
	*/
	VOID InitValue()
	{
		model_index	= 0;
		scale_rate	= 0.0f;
		score_rate	= 0.0f;	/* 반드시 최소 값으로 설정 */
		cent_dist	= 0.0f;
		r_angle		= 0.0f;
		cent_x		= 0.0f;
		cent_y		= 0.0f;
		diameter	= 0.0f;
		mark_method	= 0x00;
	}

	/*
	 desc : 값 설정
	 parm : index		- [in]  검색된 결과가 등록된 모델 어떤 인덱스로 검색되었는지 값 (Zero-based)
			scale		- [in]  검색 이미지의 스케일 비율  (단위: percent)
			score		- [in]  검색 성공률 (단위: percent)
			dist		- [in]  중심과의 떨어진 직선 거리
			angle		- [in]  검색된 회전 각도
			coverage	- [in]  전체 Edge 들의 총 비율 값 (단위: percent)
			fit_err		- [in]  검색된 엣지와 모델 엣지 간의 2차원 거리 에러 값 (0.0에 가까울 수록 비슷함)
								즉, 검색된 엣지가 어느 정도 모델의 엣지에 대응하는지를 나타냄
			x, y		- [in]  이미지에서 검색된 마크의 중심 좌표 (마크가 위치한 X / Y 좌표)
			scale_size	- [in]  Scale Rate 값이 1 이상이면 0x01, 1 미만이면 0x02, 동일하면 0x00
			circle_radius - [in]  원 반지름, M_CIRCLE, Model Finder에서만 사용
	 retn : None
	*/
	VOID SetValue(UINT32 index,
				  DOUBLE scale, DOUBLE score, DOUBLE dist, DOUBLE angle,
				  DOUBLE coverage, DOUBLE fit_err,
				  DOUBLE x, DOUBLE y, UINT8 scale_s, DOUBLE circle_r)
	{
		//model_index		= index;
		model_index = index; // lk91 model_index 값 .. 어떻게 사용해야하는지
		scale_rate		= scale;
		score_rate		= score;
		cent_dist		= dist;
		r_angle			= angle;
		coverage_rate	= coverage;
		fit_error		= fit_err;
		cent_x			= x;
		cent_y			= y;
		scale_size		= scale_s;
		circle_radius	= circle_r;
	}

	VOID SetValue(UINT32 index,
		DOUBLE scale, DOUBLE score, DOUBLE dist, DOUBLE angle,
		DOUBLE coverage, DOUBLE fit_err,
		DOUBLE x, DOUBLE y, UINT8 scale_size)
	{
		model_index = index;
		scale_rate = scale;
		score_rate = score;
		cent_dist = dist;
		r_angle = angle;
		coverage_rate = coverage;
		fit_error = fit_err;
		cent_x = x;
		cent_y = y;
		scale_size = scale_size;
	}

	/*
	 desc : 검색 조건에 부함되는지 여부
	 parm : score	- [in]  매칭 비율 값이 이 값보다 크면 Okay
			scale	- [in]  크기 비율 값이 이 값보다 크면 Okay
	 retn : TRUE or FALSE
	*/
	BOOL IsValidMark(DOUBLE score, DOUBLE scale)
	{
		return (score_rate >= score && scale_rate >= scale);
	}

	/*
	 desc : 그림 그리기 위한 정보가 적합한지 여부
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsValidDraw()
	{
		return (cent_x != 0.0f && cent_y != 0.0f && mark_width != 0 && mark_height != 0);
	}

	/*
	 desc : 값이 정상적으로 설정되어 있는지 여부 확인
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsValid()
	{
		return valid_multi;
	}

}	STG_GMFR,	*LPG_GMFR;

/* 마크 검색 결과를 그리기 위한 필요한 정보만 추출 (GMFR 축소) */
typedef struct __st_geomatric_matching_sub_result__
{
	UINT8				valid_multi;	/* Multi-Mixed Type이 유효한 Mark인지 여부 */
	UINT8				manual_set;		/* 수동 인식 여부 */
	UINT8				u8_reserved[6];
	UINT32				mark_width;		/* Mark Size = Width (Pixel) */
	UINT32				mark_height;	/* Mark Size = Height (Pixel) */
	DOUBLE				cent_x;			/* 검색된 중심 위치 : X (unit : pixel) */
	DOUBLE				cent_y;			/* 검색된 중심 위치 : Y (unit : pixel) */

	DOUBLE				move_px_x;
	DOUBLE				move_px_y;
	DOUBLE				move_mm_x;
	DOUBLE				move_mm_y;
	/*
	 desc : 데이터가 설정되어 있는지 여부
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsEmptyData()
	{
		return (cent_x < 1 && cent_y < 1 && mark_width < 1 && mark_height < 1);
	}

}	STG_GMSR,	*LPG_GMSR;

/* Edge Line Find Results */
typedef struct __st_edge_line_find_results__
{
	INT16				angle;			/* 검색된 회전 각도 (unit: degree) (90도에 근접한 것 추천) (실제 값은 나누기 100)	*/
	INT16				i16_reserved[3];
	UINT16				strength;		/* 검색된 엣지 강도 (범위: 0.0 ~ 100.0) (실제 값은 나누기 100)						*/
	UINT16				contrast;		/* Edge Contrast (gray level 분포 및 범위에 대한 측도) (실제 값은 나누기 10)		*/
										/* 엣지 영역 검색 기준으로서,														*/
										/*   값이 커질수록 (128 ~ 255), 경계선의 밝고 어두운 영역이 뚜렷해 짐				*/
										/*   값이 작아질수록 (0 ~ 127), 경계선의 밝고 어두운 영역이 흐릿해 짐				*/
	UINT16				u16_reserved[2];
	UINT32				position[2];	/* 검색된 마크 X/Y 위치 (unit: pixel) (실제 값은 나누기 1000)						*/
	UINT32				percent_score;	/* 마커 발생에 대한 신뢰도 점수 검색 값 (백분율 표현)								*/
										/* 검색된 모든 엣지 영역의 평균 신뢰도 점수	(실제 값은 나누기 1000)					*/
	UINT32				total_score;	/* 발견된 모든 가장자리의 평균 신뢰도 점수 값										*/
										/* 검색된 모든 엣지 영역의 평균 신뢰도 점수	(실제 값은 나누기 1000)					*/
	DOUBLE				cent_diff_um;	/* 이미지 중심에서부터 떨어진 거리 (단위: um) (음수는 왼쪽, 양수는 오른쪽)			*/
	DOUBLE				spacing_um;		/* 검색된 marker edge 간의 간격 값 (현재 marker edge와 다음 marker edge 간의 거리)	*/
	DOUBLE				line_width;		/* 검색된 라인 (선)의 폭 크기 얻기 (단위: pixel)									*/

}	STG_ELFR,	*LPG_ELFR;

/* Mark Model Info - Base */
typedef struct __st_config_model_parameter_value__
{
	CHAR				name[MARK_MODEL_NAME_LENGTH];	/* 모델 (Mark) 이름 */
	CHAR				file[MARK_MODEL_NAME_LENGTH];	/* 모델 (Mark) 파일 (mmf, pat file (확장자 제외)) */
	UINT32				type;							/* 모델 구분 (ENG_MMDT) 값 */
	UINT32				u32_reserved;
	/* 1 ~ 4의 값 단위는 um */
	DOUBLE				param[MAX_MODEL_REGIST_PARAM];	/* 0 : 128 (White) or 256 (Black), 1 ~ 4 : Model Size (um) */

	// lk91 vision 추가
	//UINT32	find_type = 0;	/* 0 : Geometric Model Finder, 1: Pattern Matching, 2:둘다존재  */
	CPoint	iBaseP;			// Base Position
	CPoint	iSizeP;			// Mark Size - mark_model.dat에 값 저장
	CPoint	iOffsetP;		// Mark Center	(AreaBox의 (left,top)를 원점으로 봤을 때의 좌표..) - mark_model.dat에 값 저장
	CRect	rectSearchArea;	// Search Area - cam or 시퀀스별 진행
	double	dScore;			// 매칭률... 공통
	int findCount = 1;
	// lk91 vision 추가
	
	/*
	 desc : 초기화
	 parm : None
	 retn : None
	*/
	VOID Reset()
	{
		type	= 0;
		memset(name, 0x00, MARK_MODEL_NAME_LENGTH);
		memset(file, 0x00, MARK_MODEL_NAME_LENGTH);
		memset(param, 0x00, sizeof(DOUBLE) * MAX_MODEL_REGIST_PARAM);
	}

	/*
	 desc : 유효성 검사
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsValid()
	{

		if (findCount < 1)
			return FALSE;

		if (ENG_MMDT(type) == ENG_MMDT::en_image)
			return (strlen(file) != 0) ? TRUE : FALSE;
			//return TRUE; 
		//if (ENG_MMDT(type) != ENG_MMDT::en_image)
		else
			return (type > 0 && param[0] > 0.0f && param[1] > 0.0f) ? TRUE : FALSE;
	}

}	STG_CMPV,	*LPG_CMPV;

typedef struct __st_config_model_parameter_set__
{
	UINT8				count;			/* 모델 개수 */
	UINT8				u8_reserved[7];
	LPG_CMPV			model;			/* 0 : 128 (White) or 256 (Black) */

	/*
	 desc : 유효성 검사
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsValid()
	{
		UINT8 i		= 0x00;
		BOOL bSucc	= FALSE;

		if (count < 1 || count > MAX_REGIST_MODEL)	return FALSE;
		for (; i<count && model; i++)
		{
			if (!bSucc && model[i].IsValid())	bSucc = TRUE;
		}

		if (!bSucc)
		{
			AfxMessageBox(L"Failed to load the config [config_model_parameter_set]", MB_ICONSTOP|MB_TOPMOST);
		}

		return bSucc;
	}

}	STG_CMPS,	*LPG_CMPS;

/*UvDi15um AbsolicsSR*/
/*Information required for align operation*/
typedef struct __st_recipe_align_additional_function_
{
	UINT8				save_count;						/* 레시피에 등록된 전체 모델 개수					*/
	UINT8				mark_type;						/* Mark 구분 즉, 0 : Geomatrix, 1 : Pattern Image	*/
	UINT8				align_type;						/* Align Mark Array(Global, Local) Type				*/

#ifdef USE_ALIGNMOTION
	UINT16				align_motion;					/* align 모션타입*/
#endif

	UINT8				lamp_type;						/* 램프 조명 타입 (0:Amber, 1:IR, 2:Coaxial) */
	UINT8				lamp_value[6];					/* 램프 조명 밝기값 (0: Cam1 Amber, 1: Cam2 Amber, 2: Cam1 IR, 3: Cam2 IR, 4: Cam1 Coaxial, 5: Cam2 Coaxial)*/
	UINT8				gain_level[2];					/* Align Camera에 대한 Gain Level 값 (0 ~ 255)      */
	UINT8				search_type;					/* [ENG_MMSM] Mark 검색 방법(0~5) single, cent_side, multi_only, ring_cirecle, ph_step [mark_model]*/
	UINT8				search_count;					/* 거버 내부에 Global Mark 기준으로 마크 영역마다 표시된 마크 개수 (멀티마크가 아닌 경우는 무조건 1)  */
	//UINT8				u8_reserved[3];

	UINT32				mark_area[2];					/* 얼라인 마크 (Mixed Type인 경우)를 포함하고 있는 가로 /세로 영역 크기 (unit: um) */
	UINT8				acam_num[2];						/* Align Camera Number (1 or 2) */
	CHAR				m_name[2][MARK_MODEL_NAME_LENGTH];						/* type == 0x00 -> 모델 (Model) 이름, type == 0x01 -> 파일 이름 */

	CHAR				align_name[RECIPE_NAME_LENGTH];						/* align recipe Name								*/



	/*
	 desc : 값 유효성 확인
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsValid()
	{
		return (save_count > 0  && search_type > 0 && search_count > 0 &&
			strlen(align_name) > 0  && m_name && acam_num);
	}

	/*
	 desc : Model Name 정보 등록을 위한 메모리 할당
	 parm : cnt	- [in]  할당 개수 (1-based)
	 retn : None
	*/
	VOID Init(UINT8 cnt)
	{
		//cnt = 2
		UINT8 i = 0x00;
		mark_type	= 0x01;
		save_count	= 2;	/* 등록된 모델 개수 저장 */
		align_type	= 0x01;
		lamp_type	= 0x01;
		lamp_value[0] = 0x00;
		lamp_value[1] = 0x00;
		lamp_value[2] = 0x00;
		lamp_value[3] = 0x00;
		lamp_value[4] = 0x00;
		lamp_value[5] = 0x00;
#ifdef USE_ALIGNMOTION
		align_motion = (UINT16)ENG_AMOS::en_onthefly_2cam;
#endif
		gain_level[0] = 0x00;
		gain_level[1] = 0x00;
		search_type = 0x01;
		search_count = 0x01;
		mark_area[0] = 0x00;
		mark_area[1] = 0x00;



		//align_name = new CHAR[RECIPE_NAME_LENGTH];//(PCHAR)Alloc(RECIPE_NAME_LENGTH);
		memset(align_name, 0, RECIPE_NAME_LENGTH);

		/* 메모리 할당 */
		//acam_num = new UINT8[save_count];//  (PUINT8)::Alloc(sizeof(UINT8) * save_count);
		memset(acam_num, 0x00, sizeof(UINT8) * save_count);
		//m_name = new CHAR*[save_count]; //(PCHAR*)::Alloc(sizeof(PCHAR) * save_count);

		for (; i < cnt; i++)
		{
			//m_name[i] = new CHAR[MARK_MODEL_NAME_LENGTH];// (PCHAR)::Alloc(sizeof(CHAR) * MARK_MODEL_NAME_LENGTH);
			memset(m_name[i], 0x00, sizeof(CHAR) * MARK_MODEL_NAME_LENGTH);
		}
	}

	/*
	 desc : 할당된 메모리 해제
	 parm : None
	 retn : None
	*/
	VOID Close()
	{
		UINT8 i = 0x00;
		//if (m_name && save_count)
		//{
			//delete acam_num;

			//::Free(acam_num);
			//for (; i < save_count; i++)	::delete m_name[i];
			//delete[] m_name;
		//}

	/*	if (align_name)			
			delete align_name;*/
	}

}	STG_RAAF, * LPG_RAAF;


/* The parameter information of strip marker */
typedef struct __st_measurement_strip_marker_parameter__
{
	UINT8				enable_angle;	/* Enable or disable the angle mode : 검색 대상이 회전되어 있는지 여부 */
	UINT8				orientation;	/* Sets the orientation of an edge or stripe marker */
										/* 0 : HORIZONTAL, 1 : VERTICAL */
	UINT8				polarity[2];	/* 0 : Left (orientation:horizontal) or Top (orientation:vertical) */
										/* 1 : Right (orientation:horizontal) or Bottom (orientation:vertical) */
										/* 0 : POSITIVE : 어두운 곳에서 밝은 곳으로 검사할 경우 */
										/* 1 : NEGATVIE : 밝은 곳에서 어두운 곳으로 검사할 경우 */
	UINT8				angle_int_mode;	/* 보간법 알고리즘 방식 (0:M_NEAREST_NEIGHBOR, 1:M_BILINEAR, 2:M_BICUBIC) */
	UINT8				u8_reserved[3];

	INT32				edge_width;		/* 검색하고자 하는 대상의 밝기 변화의 크기 (폭; 넓이. pixel) */
	INT32				edge_offset;	/* 검색하고자 하는 대상의 밝기 크기에서 허용 오차 값. 즉, edge_with=40, 이 값이 10이면 검색 범위는 30 ~ 50 임 */

	DOUBLE				angle_delta;	/* 검색 대상이 회전된 경우, 회전 범위를 양의 혹은 음의 방향으로 지정 합니다. */

	STG_I32XY			box_origin;		/* Box position (unit: pixel) (left / top) : 박스의 위치를 지정할 offset 값 입니다 */
	STG_I32XY			box_size;		/* Inspection Area (unit: pixel) : 박스의 크기를 지정 합니다. */

	BOOL IsVald()
	{
		return (box_size.x > 0 && box_size.y > 0 &&
				(edge_width < box_size.x && edge_offset < box_size.y) &&
				edge_width > edge_offset);
	}

}	STG_MSMP,	*LPG_MSMP;

/* The result of strip marker */
typedef struct __st_measurement_strip_marker_result__
{
	DOUBLE				cent_x,cent_y;	/* Retrieves the position for each marker occurrence */
	DOUBLE				score;			/* Retrieves the confidence score, as a percentage, for each marker occurrence */
	DOUBLE				angle;			/* Retrieves the angle for each marker occurrence, in degrees */
	DOUBLE				width;			/* Retrieves the edge width of the edge marker or the width of the stripe marker, in pixels or real-world units, for each marker occurrence */
	DOUBLE				length;			/* Retrieves the length, in pixels or real-world units, for each marker occurrence */

}	STG_MSMR,	*LPG_MSMR;

/* ROI */
typedef struct __st_config_roi_data__
{
	INT32				roi_Left[3];	/* ROI LEFT 위치 */ // lk91 CAM 3개
	INT32				roi_Right[3];	/* ROI RIGHT 위치 */
	INT32				roi_Top[3];		/* ROI TOP 위치 */
	INT32				roi_Bottom[3];	/* ROI BOTTOM 위치 */

}	STG_CRD, * LPG_CRD;

#pragma pack (pop)	// 8 bytes

#pragma pack (push, 1)

/* Vision Shared Memory - All */
typedef struct __st_vision_data_shared_memory__
{
	/* Align Camera 별 Grabbed Image의 검색을 위한 등록된 모델 정보 */
	STG_CMPV			**mod_define;
	/* 모델 등록 크기 */
	STG_DBXY			**mod_size;		/* unit : pixel */

	STG_GMFR			**mark_result;	/* Mark Find 결과 정보 */
	STG_EDFR			**edge_result;	/* Edge Find 결과 정보 */
	STG_ELFR			**line_result;	/* Edge Line Detection 결과 저장 버퍼 (최대 개수 설정 필요. MilModel:GetMeasResult) */
	STG_ACCE			***cali_global;	/* Align Camera의 X / Y 축의 모션 좌표에 대한 보정 오차 값 */
	STG_ACCE			***cali_local;	/* Align Camera의 X / Y 축의 모션 좌표에 대한 보정 오차 값 */

	STG_DLSM			**link;			/* 얼라인 카메라 별 통신 상태 */

	UINT8				***mark_image;	/* Model Find 결과 이미지 */
	UINT8				**edge_image;	/* Edge Find 결과 이미지 : 카메라 마다 이미지 버퍼 1개씩만 존재 */
	UINT8				***line_image;	/* Line Find 결과 이미지 */

}	STG_VDSM,	*LPG_VDSM;

#pragma pack (pop)	// 1 bytes
