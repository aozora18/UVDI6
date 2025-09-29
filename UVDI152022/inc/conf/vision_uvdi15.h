
/*
 desc : Structure
*/

#pragma once

#include "global.h"
#include "vision_comn.h"
#include "conf_comn.h"		//���� ���� �̽�

#define MARK_MODEL_NAME_LENGTH					256		/* Align Mark Model Name ���� */
#define MAX_MODEL_REGIST_PARAM					5		/* �� ���� �ִ� ��� ������ �Ķ���� ���� */
#define	MAX_REGIST_MODEL						48		/* �ִ� ��� ������ �� ���� */

/* --------------------------------------------------------------------------------------------- */
/*                                             ������                                            */
/* --------------------------------------------------------------------------------------------- */

/* Mark Model Search Method */
typedef enum class __en_mark_model_search_method__ : UINT8
{
	en_none			= 0x00,

	en_single		= 0x01,			/* ���� ��ũ �˻� ��� */
	en_cent_side	= 0x02,			/* �߽��� ��ũ (any shape) 1���� �ֺ��� ���� ��ũ (any shape)�� �˻��ϴ� ��� */
	en_multi_only	= 0x03,			/* �ֺ��� ���� ��ũ (any shape)�� �˻��ϴ� ��� */
	en_ring_circle	= 0x04,			/* �ܰ��� ��(����) ��ũ�� ���ο� �� (Circle)�� ������ ��ũ �˻��ϴ� ��� */
	en_ph_step		= 0x05,			/* ���а� ���� ���� ������ �ʿ��� */

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
/*                                             ����ü                                            */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 8)

/* Align Camera Grabbed Image Result */
typedef struct __st_align_camera_grab_result__
{
	// Grabbed Image Information			
	UINT8				marked;					/* ��ũ �˻� ���� ����. 0x00 - Not run, 0x01 - Success, 0x02 - Failed */
	UINT8				cam_id;					/* Camera Index (1 based) */
	UINT8				img_id;					/* Grabbed Image�� Index (0 or Later) , 0,1 : Global Mark, �� �� : Local Mark */
	UINT8				model_index;			/* 1 ���� Grabbed Image���� 1�� �̻� �˻��� Mark �� �� ��ϵ� Mark�� Model Index (0x00 ~ ). 1�� �˻��Ǹ� 0x00 */
	UINT8				valid_score;			/* ȯ�� ���Ͽ� ������ Score ������ ū ������� ���� */
	UINT8				valid_scale;			/* ȯ�� ���Ͽ� ������ Scale ������ ū ������� ���� */
	UINT8				valid_multi;			/* Mulit-Mixed Type�� ��ȿ�� ��ũ���� ���� */
	UINT8				manual_set;				/* ������ Align Mark ���� ���� (ȭ�� Touch)���� �����ߴ��� ���� */
	UINT8				mark_method;			/* ENG_MMSM �� */
	UINT8				scale_size;				/* 0x00:Size Equal, 0x01: Size (Scale) Up, 0x02: Size (Scale) Down */
	UINT8				u8_reserved[6];

	UINT32				cent_dist_x;			/* ENG_MMSM != en_mmsm_multi_side�� ��ȿ(�⺻ ��ũ�� �߽ɰ� �ֺ� ��ũ���� �簢�� �߽� ���� ������ �Ÿ�. ����: um) */
	UINT32				cent_dist_y;			/* ENG_MMSM != en_mmsm_multi_side�� ��ȿ(�⺻ ��ũ�� �߽ɰ� �ֺ� ��ũ���� �簢�� �߽� ���� ������ �Ÿ�. ����: um) */
	UINT32				grab_size;				/* Grabbed Image�� ũ�� (����: Bytes) */
	UINT32				grab_width;				/* Grabbed Image�� ���� (����: Pixel) */
	UINT32				grab_height;			/* Grabbed Image�� ���� (����: Pixel) */
	UINT32				u32_reserved;
	// Mark �˻� ��� �� - pixel
	DOUBLE				mark_cent_px_x;			/* �˻��� Mark�� �߽� (����: pixel) */
	DOUBLE				mark_cent_px_y;			/* �˻��� Mark�� �߽� (����: pixel) */
	DOUBLE				mark_width_px;			/* �˻��� Mark Image Size (����: pixel) */
	DOUBLE				mark_height_px;			/* �˻��� Mark Image Size (����: pixel) */
	// Mark �˻� ��� �� - mm
	DOUBLE				mark_cent_mm_x;			/* �˻��� Mark�� �߽� */
	DOUBLE				mark_cent_mm_y;			/* �˻��� Mark�� �߽� */
	DOUBLE				mark_cent_mm_dist;		/* Grabbed Image�� �߽ɿ��� �˻��� Mark Image�� �߽� ���� �Ÿ� (���, 0, ���� �� ����) */
	DOUBLE				mark_width_mm;			/* �˻��� Mark Image Size */
	DOUBLE				mark_height_mm;			/* �˻��� Mark Image Size */
	DOUBLE				mark_angle;				/* �˻��� Mark Image Rotate (����: ��) */
	DOUBLE				score_rate;				/* ��Ī �˻��� (����: %) */
	DOUBLE				scale_rate;				/* ��Ī�� �̹����� ������ (����) (����: %) */
	DOUBLE				square_rate;			/* �� �� 100�� ������� ���簢���� ����� */
	DOUBLE				coverage_rate;			/* �� �˻��� Edge���� ���� �� (����: %) */
	DOUBLE				fit_error;				/* �˻��� ������ �� ���� ���� 2���� �Ÿ� ���� �� (0.0�� ����� ���� �����) */
	/* ��, �˻��� ������ ��� ���� ���� ������ �����ϴ����� ��Ÿ�� */
	DOUBLE				circle_radius;			/* circle ������ */
	/* ----------------------------------------------------------------------------------------- */
	/* �Ʒ� 2���� ������ �߿���. ���� ����PC���� Align Mark ��ǥ �� �Ѱ��ֱ� ����. ������ mm ��. */
	/* move_x_mm : ��� ���� ���, ���簡 X ������ ���, ���� ���� ���� ���簡 X ������ Ȯ��� */
	/* move_y_mm : ��� ���� ���, ���簡 Y ������ ���, ���� ���� ���� ���簡 Y ������ Ȯ��� */
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
	 desc : ������ ��ȿ ����
	 parm : multi_mark	- [in]  Multi Mark (���� ��ũ) ���� ����
	 retn : TRUE or FALSE
	*/
	BOOL IsMarkValid(BOOL multi_mark=FALSE)
	{
		return (valid_scale && valid_score && (!multi_mark || valid_multi));
	}

	/*
	 desc : ������ Mark ��ȿ ���� ����
	 parm : flag	- [in]  0x00 - ��ȿ���� ����, 0x01 - ��ȿ ��
	 retn : None
	*/
	VOID SetMarkValid(UINT8 flag, BOOL multi_mark=FALSE)
	{
		valid_score	= flag;
		valid_scale	= flag;
		if (multi_mark)	valid_multi	= flag;
	}

	/*
	 desc : ���� �̹��� �����Ͱ� �����ϴ��� ����
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

	/* ������ */
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
	DOUBLE				dia_meter_px;			/* ���� ũ�� (����: pixel) */
	DOUBLE				dia_meter_um;			/* ���� ũ�� (����: um) */
	DOUBLE				width_um;				/* ���� ũ�� (����: um) */
	DOUBLE				height_um;				/* ���� ũ�� (����: um) */
	DOUBLE				angle;					/* �˻��� ��ü�� ȸ�� ���� */
	DOUBLE				strength;				/* ��� ���� (?) */
	DOUBLE				length;					/* �������� ����� ���е��� �� ���� (?) */
	RECT				circle_area;			/* �˻��� ���� (����: pixel) */
	RECT				box_area;				/* �˻��� ���� (����: pixel) */

	/*
	 desc : �˻��� ������ ���� �� ��ȯ
	 parm : None
	 retn : ���� �� (����: pixel)
	*/
	INT32 GetBoxWidth()
	{
		return box_area.right - box_area.left;
	}

	/*
	 desc : �˻��� ������ ���� �� ��ȯ
	 parm : None
	 retn : ���� �� (����: pixel)
	*/
	INT32 GetBoxHeight()
	{
		return box_area.bottom - box_area.top;
	}

	/*
	 desc : �˻��� ������ ���� �� ��ȯ
	 parm : None
	 retn : ���� �� (����: pixel)
	*/
	INT32 GetCircleWidth()
	{
		return circle_area.right - circle_area.left;
	}

	/*
	 desc : �˻��� ������ ���� �� ��ȯ
	 parm : None
	 retn : ���� �� (����: pixel)
	*/
	INT32 GetCircleHeight()
	{
		return circle_area.bottom - circle_area.top;
	}

}	STG_EDFR,	*LPG_EDFR;

typedef struct __st_geomatric_matching_find_result__
{
	UINT8				valid_multi;	/* Multi-Mixed Type�� ��ȿ�� ��ũ���� ���� */
	UINT8				mark_method;	/* ENG_MMSM �� */
	UINT8				scale_size;		/* 0x00:Size Equal, 0x01: Size (Scale) Up, 0x02: Size (Scale) Down */
	UINT8				u8_reserved[7];
	UINT32				cent_dist_x;	/* Mixed Mark���� �⺻ ��ũ�� �߽ɰ� �ֺ� ��ũ���� �߽� ��ġ ���� ������ �Ÿ� (����: um) */
	UINT32				cent_dist_y;	/* Mixed Mark���� �⺻ ��ũ�� �߽ɰ� �ֺ� ��ũ���� �߽� ��ġ ���� ������ �Ÿ� (����: um) */
	UINT32				model_index;	/* ModDefine �Լ��� ��ϵ� �𵨷� �˻��Ǿ��ٴ� �� �ε��� (Zero-based) */
	UINT32				mark_width;		/* Mark Size = Width (Pixel) */
	UINT32				mark_height;	/* Mark Size = Height (Pixel) */
	UINT32				u32_reserved;
	DOUBLE				scale_rate;		/* �˻��� �̹����� ������ ���� �� (����: percent) */
	DOUBLE				score_rate;		/* �˻��� (����: percent) */
	DOUBLE				cent_dist;		/* �̹��� �߽ɰ��� ������ �Ÿ� (unit : pixel) */
	DOUBLE				square_rate;	/* ���簢�� ���� �� (�� ���� 100�� ������� ���簢���� �����) */
	DOUBLE				r_angle;		/* �˻��� ���� */
	DOUBLE				coverage_rate;	/* ��ü Edge���� �� ���� �� (����: percent) */
	DOUBLE				fit_error;		/* 0.0�� ����� ���� �˻� ��� �𵨰� ����� (�� ������ �˻��� ���� ���� 2���� �Ÿ� ���� ��) */
										/* ��, �˻��� ������ ��� ���� ���� ������ �����ϴ����� ��Ÿ�� */
	DOUBLE				circle_radius;	/* circle ������ */

	DOUBLE				cent_x;			/* �̹������� �˻��� ��ũ�� �߽� ��ġ : X (unit : pixel) */
	DOUBLE				cent_y;			/* �̹������� �˻��� ��ũ�� �߽� ��ġ : Y (unit : pixel) */
	DOUBLE				diameter;		/* ���� ���� : pixel */

	/*
	 desc : ����/���� ��ũ�� ũ�� �� ���� ū �� ��ȯ
	 parm : None
	 retn : ũ�� ��ȯ (����: Pixel)
	*/
	UINT32 GetMarkSize()
	{
		return mark_width > mark_height ? mark_width : mark_height;
	}

	/*
	 desc : ���� ����� ��ũ�� ���, Hole�� Laser ���� ���� ������ ���� (����: um)
	 parm : None
	 retn : ������ �Ÿ� (�밢�� ����)
	*/
	UINT32 GetCentDistLen()
	{
		return (UINT32)ROUNDED(abs(sqrt(pow(cent_dist_x, 2) + pow(cent_dist_y, 2))), 0);
	}

	/*
	 desc : ����ü �� �ʱ�ȭ
	 parm : None
	 retn : None
	*/
	VOID InitValue()
	{
		model_index	= 0;
		scale_rate	= 0.0f;
		score_rate	= 0.0f;	/* �ݵ�� �ּ� ������ ���� */
		cent_dist	= 0.0f;
		r_angle		= 0.0f;
		cent_x		= 0.0f;
		cent_y		= 0.0f;
		diameter	= 0.0f;
		mark_method	= 0x00;
	}

	/*
	 desc : �� ����
	 parm : index		- [in]  �˻��� ����� ��ϵ� �� � �ε����� �˻��Ǿ����� �� (Zero-based)
			scale		- [in]  �˻� �̹����� ������ ����  (����: percent)
			score		- [in]  �˻� ������ (����: percent)
			dist		- [in]  �߽ɰ��� ������ ���� �Ÿ�
			angle		- [in]  �˻��� ȸ�� ����
			coverage	- [in]  ��ü Edge ���� �� ���� �� (����: percent)
			fit_err		- [in]  �˻��� ������ �� ���� ���� 2���� �Ÿ� ���� �� (0.0�� ����� ���� �����)
								��, �˻��� ������ ��� ���� ���� ������ �����ϴ����� ��Ÿ��
			x, y		- [in]  �̹������� �˻��� ��ũ�� �߽� ��ǥ (��ũ�� ��ġ�� X / Y ��ǥ)
			scale_size	- [in]  Scale Rate ���� 1 �̻��̸� 0x01, 1 �̸��̸� 0x02, �����ϸ� 0x00
			circle_radius - [in]  �� ������, M_CIRCLE, Model Finder������ ���
	 retn : None
	*/
	VOID SetValue(UINT32 index,
				  DOUBLE scale, DOUBLE score, DOUBLE dist, DOUBLE angle,
				  DOUBLE coverage, DOUBLE fit_err,
				  DOUBLE x, DOUBLE y, UINT8 scale_s, DOUBLE circle_r)
	{
		//model_index		= index;
		model_index = index; // lk91 model_index �� .. ��� ����ؾ��ϴ���
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
	 desc : �˻� ���ǿ� ���ԵǴ��� ����
	 parm : score	- [in]  ��Ī ���� ���� �� ������ ũ�� Okay
			scale	- [in]  ũ�� ���� ���� �� ������ ũ�� Okay
	 retn : TRUE or FALSE
	*/
	BOOL IsValidMark(DOUBLE score, DOUBLE scale)
	{
		return (score_rate >= score && scale_rate >= scale);
	}

	/*
	 desc : �׸� �׸��� ���� ������ �������� ����
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsValidDraw()
	{
		return (cent_x != 0.0f && cent_y != 0.0f && mark_width != 0 && mark_height != 0);
	}

	/*
	 desc : ���� ���������� �����Ǿ� �ִ��� ���� Ȯ��
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsValid()
	{
		return valid_multi;
	}

}	STG_GMFR,	*LPG_GMFR;

/* ��ũ �˻� ����� �׸��� ���� �ʿ��� ������ ���� (GMFR ���) */
typedef struct __st_geomatric_matching_sub_result__
{
	UINT8				valid_multi;	/* Multi-Mixed Type�� ��ȿ�� Mark���� ���� */
	UINT8				manual_set;		/* ���� �ν� ���� */
	UINT8				u8_reserved[6];
	UINT32				mark_width;		/* Mark Size = Width (Pixel) */
	UINT32				mark_height;	/* Mark Size = Height (Pixel) */
	DOUBLE				cent_x;			/* �˻��� �߽� ��ġ : X (unit : pixel) */
	DOUBLE				cent_y;			/* �˻��� �߽� ��ġ : Y (unit : pixel) */

	DOUBLE				move_px_x;
	DOUBLE				move_px_y;
	DOUBLE				move_mm_x;
	DOUBLE				move_mm_y;
	/*
	 desc : �����Ͱ� �����Ǿ� �ִ��� ����
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
	INT16				angle;			/* �˻��� ȸ�� ���� (unit: degree) (90���� ������ �� ��õ) (���� ���� ������ 100)	*/
	INT16				i16_reserved[3];
	UINT16				strength;		/* �˻��� ���� ���� (����: 0.0 ~ 100.0) (���� ���� ������ 100)						*/
	UINT16				contrast;		/* Edge Contrast (gray level ���� �� ������ ���� ����) (���� ���� ������ 10)		*/
										/* ���� ���� �˻� �������μ�,														*/
										/*   ���� Ŀ������ (128 ~ 255), ��輱�� ��� ��ο� ������ �ѷ��� ��				*/
										/*   ���� �۾������� (0 ~ 127), ��輱�� ��� ��ο� ������ �帴�� ��				*/
	UINT16				u16_reserved[2];
	UINT32				position[2];	/* �˻��� ��ũ X/Y ��ġ (unit: pixel) (���� ���� ������ 1000)						*/
	UINT32				percent_score;	/* ��Ŀ �߻��� ���� �ŷڵ� ���� �˻� �� (����� ǥ��)								*/
										/* �˻��� ��� ���� ������ ��� �ŷڵ� ����	(���� ���� ������ 1000)					*/
	UINT32				total_score;	/* �߰ߵ� ��� �����ڸ��� ��� �ŷڵ� ���� ��										*/
										/* �˻��� ��� ���� ������ ��� �ŷڵ� ����	(���� ���� ������ 1000)					*/
	DOUBLE				cent_diff_um;	/* �̹��� �߽ɿ������� ������ �Ÿ� (����: um) (������ ����, ����� ������)			*/
	DOUBLE				spacing_um;		/* �˻��� marker edge ���� ���� �� (���� marker edge�� ���� marker edge ���� �Ÿ�)	*/
	DOUBLE				line_width;		/* �˻��� ���� (��)�� �� ũ�� ��� (����: pixel)									*/

}	STG_ELFR,	*LPG_ELFR;

/* Mark Model Info - Base */
typedef struct __st_config_model_parameter_value__
{
	CHAR				name[MARK_MODEL_NAME_LENGTH];	/* �� (Mark) �̸� */
	CHAR				file[MARK_MODEL_NAME_LENGTH];	/* �� (Mark) ���� (mmf, pat file (Ȯ���� ����)) */
	UINT32				type;							/* �� ���� (ENG_MMDT) �� */
	UINT32				u32_reserved;
	/* 1 ~ 4�� �� ������ um */
	DOUBLE				param[MAX_MODEL_REGIST_PARAM];	/* 0 : 128 (White) or 256 (Black), 1 ~ 4 : Model Size (um) */

	// lk91 vision �߰�
	//UINT32	find_type = 0;	/* 0 : Geometric Model Finder, 1: Pattern Matching, 2:�Ѵ�����  */
	CPoint	iBaseP;			// Base Position
	CPoint	iSizeP;			// Mark Size - mark_model.dat�� �� ����
	CPoint	iOffsetP;		// Mark Center	(AreaBox�� (left,top)�� �������� ���� ���� ��ǥ..) - mark_model.dat�� �� ����
	CRect	rectSearchArea;	// Search Area - cam or �������� ����
	double	dScore;			// ��Ī��... ����
	int findCount = 1;
	// lk91 vision �߰�
	
	/*
	 desc : �ʱ�ȭ
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
	 desc : ��ȿ�� �˻�
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
	UINT8				count;			/* �� ���� */
	UINT8				u8_reserved[7];
	LPG_CMPV			model;			/* 0 : 128 (White) or 256 (Black) */

	/*
	 desc : ��ȿ�� �˻�
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
	UINT8				save_count;						/* �����ǿ� ��ϵ� ��ü �� ����					*/
	UINT8				mark_type;						/* Mark ���� ��, 0 : Geomatrix, 1 : Pattern Image	*/
	UINT8				align_type;						/* Align Mark Array(Global, Local) Type				*/

#ifdef USE_ALIGNMOTION
	UINT16				align_motion;					/* align ���Ÿ��*/
#endif

	UINT8				lamp_type;						/* ���� ���� Ÿ�� (0:Amber, 1:IR, 2:Coaxial) */
	UINT8				lamp_value[6];					/* ���� ���� ��Ⱚ (0: Cam1 Amber, 1: Cam2 Amber, 2: Cam1 IR, 3: Cam2 IR, 4: Cam1 Coaxial, 5: Cam2 Coaxial)*/
	UINT8				gain_level[2];					/* Align Camera�� ���� Gain Level �� (0 ~ 255)      */
	UINT8				search_type;					/* [ENG_MMSM] Mark �˻� ���(0~5) single, cent_side, multi_only, ring_cirecle, ph_step [mark_model]*/
	UINT8				search_count;					/* �Ź� ���ο� Global Mark �������� ��ũ �������� ǥ�õ� ��ũ ���� (��Ƽ��ũ�� �ƴ� ���� ������ 1)  */
	//UINT8				u8_reserved[3];

	UINT32				mark_area[2];					/* ����� ��ũ (Mixed Type�� ���)�� �����ϰ� �ִ� ���� /���� ���� ũ�� (unit: um) */
	UINT8				acam_num[2];						/* Align Camera Number (1 or 2) */
	CHAR				m_name[2][MARK_MODEL_NAME_LENGTH];						/* type == 0x00 -> �� (Model) �̸�, type == 0x01 -> ���� �̸� */

	CHAR				align_name[RECIPE_NAME_LENGTH];						/* align recipe Name								*/



	/*
	 desc : �� ��ȿ�� Ȯ��
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsValid()
	{
		return (save_count > 0  && search_type > 0 && search_count > 0 &&
			strlen(align_name) > 0  && m_name && acam_num);
	}

	/*
	 desc : Model Name ���� ����� ���� �޸� �Ҵ�
	 parm : cnt	- [in]  �Ҵ� ���� (1-based)
	 retn : None
	*/
	VOID Init(UINT8 cnt)
	{
		//cnt = 2
		UINT8 i = 0x00;
		mark_type	= 0x01;
		save_count	= 2;	/* ��ϵ� �� ���� ���� */
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

		/* �޸� �Ҵ� */
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
	 desc : �Ҵ�� �޸� ����
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
	UINT8				enable_angle;	/* Enable or disable the angle mode : �˻� ����� ȸ���Ǿ� �ִ��� ���� */
	UINT8				orientation;	/* Sets the orientation of an edge or stripe marker */
										/* 0 : HORIZONTAL, 1 : VERTICAL */
	UINT8				polarity[2];	/* 0 : Left (orientation:horizontal) or Top (orientation:vertical) */
										/* 1 : Right (orientation:horizontal) or Bottom (orientation:vertical) */
										/* 0 : POSITIVE : ��ο� ������ ���� ������ �˻��� ��� */
										/* 1 : NEGATVIE : ���� ������ ��ο� ������ �˻��� ��� */
	UINT8				angle_int_mode;	/* ������ �˰��� ��� (0:M_NEAREST_NEIGHBOR, 1:M_BILINEAR, 2:M_BICUBIC) */
	UINT8				u8_reserved[3];

	INT32				edge_width;		/* �˻��ϰ��� �ϴ� ����� ��� ��ȭ�� ũ�� (��; ����. pixel) */
	INT32				edge_offset;	/* �˻��ϰ��� �ϴ� ����� ��� ũ�⿡�� ��� ���� ��. ��, edge_with=40, �� ���� 10�̸� �˻� ������ 30 ~ 50 �� */

	DOUBLE				angle_delta;	/* �˻� ����� ȸ���� ���, ȸ�� ������ ���� Ȥ�� ���� �������� ���� �մϴ�. */

	STG_I32XY			box_origin;		/* Box position (unit: pixel) (left / top) : �ڽ��� ��ġ�� ������ offset �� �Դϴ� */
	STG_I32XY			box_size;		/* Inspection Area (unit: pixel) : �ڽ��� ũ�⸦ ���� �մϴ�. */

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
	INT32				roi_Left[3];	/* ROI LEFT ��ġ */ // lk91 CAM 3��
	INT32				roi_Right[3];	/* ROI RIGHT ��ġ */
	INT32				roi_Top[3];		/* ROI TOP ��ġ */
	INT32				roi_Bottom[3];	/* ROI BOTTOM ��ġ */

}	STG_CRD, * LPG_CRD;

#pragma pack (pop)	// 8 bytes

#pragma pack (push, 1)

/* Vision Shared Memory - All */
typedef struct __st_vision_data_shared_memory__
{
	/* Align Camera �� Grabbed Image�� �˻��� ���� ��ϵ� �� ���� */
	STG_CMPV			**mod_define;
	/* �� ��� ũ�� */
	STG_DBXY			**mod_size;		/* unit : pixel */

	STG_GMFR			**mark_result;	/* Mark Find ��� ���� */
	STG_EDFR			**edge_result;	/* Edge Find ��� ���� */
	STG_ELFR			**line_result;	/* Edge Line Detection ��� ���� ���� (�ִ� ���� ���� �ʿ�. MilModel:GetMeasResult) */
	STG_ACCE			***cali_global;	/* Align Camera�� X / Y ���� ��� ��ǥ�� ���� ���� ���� �� */
	STG_ACCE			***cali_local;	/* Align Camera�� X / Y ���� ��� ��ǥ�� ���� ���� ���� �� */

	STG_DLSM			**link;			/* ����� ī�޶� �� ��� ���� */

	UINT8				***mark_image;	/* Model Find ��� �̹��� */
	UINT8				**edge_image;	/* Edge Find ��� �̹��� : ī�޶� ���� �̹��� ���� 1������ ���� */
	UINT8				***line_image;	/* Line Find ��� �̹��� */

}	STG_VDSM,	*LPG_VDSM;

#pragma pack (pop)	// 1 bytes
