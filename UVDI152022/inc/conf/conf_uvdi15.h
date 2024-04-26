
/*
 desc : System Configuration Information
*/

#pragma once

#include "conf_comn.h"
#include <vector>
#include <tuple>
#include <map>
/* --------------------------------------------------------------------------------------------- */
/*                                           ��� ��                                             */
/* --------------------------------------------------------------------------------------------- */

#define	WORK_NAME_LEN				128
#define	STEP_NAME_LEN				128

/* --------------------------------------------------------------------------------------------- */
/*                                       ����(Global) ������                                     */
/* --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- */
/*                                Config ����ü - Luria (Service)                                */
/* --------------------------------------------------------------------------------------------- */

#pragma pack (push, 8)

/* Setup Align �� ���� ��ó�� ���� */
typedef struct __st_config_setup_camera_info__
{
	UINT8				acam_count;								/* ��ġ�� Align Camera ��� */
	UINT8				acam_inst_angle;						/* ī�޶� ��ġ�� ��, ȸ�� ���� (0: ȸ�� ����, 1: 180�� ȸ��) */
	UINT8				u8_rserved[6];


	UINT16				acam_size[2];							/* Align Camera Size (Width / Height) (����: resolution) */
	UINT16				aoi_size[2];							/* FOV ���� ���� : Align Mark Search (Width / Height) (unit: pixel) */
	UINT16				soi_size[2];							/* FOV ���� ���� : Vision Step Search Search (Width / Height) (unit: pixel) */
	UINT16				doi_size[2];							/* FOV ���� ���� : Vision DOF Focusing (Width / Height) (unit: pixel) */
	UINT16				mes_size[2];							/* FOV ���� ���� : Vision Calibration Measurement (Width / Height) (unit: pixel) */
	UINT16				ioi_size[2];							/* FOV ���� ���� : Vision Shutting Inspection (Width / Height) (unit: pixel) */
	UINT16				spc_size[2];							/* FOV ���� ���� : Align Camera Specification (Width / Height) (unit: pixel) */
	UINT16				stg_size[2];							/* FOV ���� ���� : QuartZ (Stage Straightness) (Width / Height) (unit: pixel) */
	UINT16				hol_size[2];							/* FOV ���� ���� : Material (Hole Size) (Width / Height) (unit: pixel) */
	UINT16				u16_rserved[2];

	DOUBLE				acam_up_limit;							/* ����� ī�޶� Z �� ���� �ִ� Limit ��   (����: mm) */
	DOUBLE				acam_dn_limit;							/* ����� ī�޶� Z �� �Ʒ��� �ִ� Limit �� (����: mm) */

	DOUBLE				safety_pos[MAX_ALIGN_CAMERA];			/* ����� ī�޶� X �� ���� ��ġ �� (����: mm) */

	/*
	 desc : ī�޶� �ִ� �ػ� ũ�⿡ ���� ���� ũ�� ��ȯ (����: Bytes)
	 parm : None
	 retn : ���� �Ҵ� ũ�� (����: Bytes)
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
	 desc : ī�޶� ũ�� (cam_size���� aoi/soi/doi/ioi_size�� �� ũ�� ���� ��
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
	 desc : ����� ī�޶��� Z �� �̵� ������ ������� ����
	 parm : pos	- [in]  �̵� ��ġ �� (����: mm)
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

/* Setup Align �� ���� ��ó�� ���� */
typedef struct __st_config_setup_align_info__
{
	//UINT8				align_method;							/* 0 : Align Camera 2���� Mark 2 or 4�� ���� (�̵��ϸ鼭 ����) */ 
																/* 1 : Align Camera 2���� Mark 2 ���� �� �߰��� ���߰�, ī�޶� ��/�� ���� �� �ٽ� ������ 2�� ���� (�̵� �� ���߰� ����, �ٽ� �̵� �� ����) */
																/*     �Ź� �� ��ũ�� 4 ���� ���簢�� (ī�޶� FOV�� ������ ���ϴ� ���)�� �ƴ� ��� */
																/* 2 : Align Camera 1���� ���� ��ũ ���� (���߸鼭 ����) */
	UINT8				use_2d_cali_data;						/* Align Camera�� 2D Calibration Data ���� ���� (1:���, 0:�̻��) */
	UINT8				use_invalid_mark_cali;					/* �νĵ��� ���� Mark�� ���� ����(����)�� �νĽ�ų�� ���� (1:���, 0:�̻��) */
	UINT8				use_mark_offset;						/* �� Global Mark Offset�� �߰� ��� ���� ���� (1:���, 0:�̻��) */
	UINT8				u8_reserved[4];
	DOUBLE				dof_film_thick;							/* Align Camera & Photohead Focus ������ �� ���Ǵ¼����� �β� (����: mm) */
	DOUBLE				table_unloader_xy[MAX_TABLE][2];		/* �뱤 ���縦 �÷� ���� ���� ���������� �۾� ��ġ (����: mm) (�Ҽ��� 4�ڸ����� ��ȿ) */
	DOUBLE				mark2_org_gerb_xy[2];					/* �Ź��� ������ Mark 2���� X / Y ��ǥ (����: mm) */
	DOUBLE				mark2_stage_x;							/* �Ź��� ����� Mark 2�� ���� X �� ��� ��ǥ (����: mm) */
	//DOUBLE				mark_offset_x[4];						/* �� Global Mark�� offset ������(����: mm) */
	//DOUBLE				mark_offset_y[4];
	DOUBLE				mark_horz_diff;							/* ����(0) / ����(1) �� �������� ���Ѱ�(����: mm)*/
	DOUBLE				mark_vert_diff;							/* ����(2) / ����(2) �� �������� ���Ѱ�(����: mm)*/
	MarkoffsetInfo*		markOffsetPtr;
	//UINT8				use_Localmark_offset;					/* �� Local Mark Offset�� �߰� ��� ���� ���� (1:���, 0:�̻��) */
	//DOUBLE				localMark_offset_x[16];
	//DOUBLE				localMark_offset_y[16];

	DOUBLE				distCam2Cam[3];

	
	//DOUBLE				mark_offset_x[4];						/* �� Global Mark�� offset ������(����: mm) */
	//DOUBLE				mark_offset_y[4];

	/* �Ź��� ����� Mark 2�� ���� X �� ��� ��ǥ (����: mm) */
	void SetMarkoffsetPtr(MarkoffsetInfo& ptr)
	{
		markOffsetPtr = &ptr;
	}

}	STG_CSAI,	*LPG_CSAI;

/* Vision Camera�κ��� Grabbed Image�� ���� ��ó�� ���� */
typedef struct __st_config_grab_image_preprocess__
{
	/* �Ʒ� 2�� ���� �� 1�� ���� 0�� ���, �� ����� ������� ���� */
	UINT8				use_grab_pre_proc;		/* 0 : ��� ����, 1 : ����� */
	UINT8				use_noise_remove;		/* 0 : ������ ���� ����, 1 : ������ ���� ��� ����� */
	UINT8				use_thin_operation;		/* �̹��� ���� �ڸ��� ��� ó������ ���� */
												/* !!! ����μ� �� �÷��� ���� 0 ������ ������ ���� !!! */
	UINT8				use_small_particle;		/* ���� �̹��� ������ ���� ��� ��� ���� (0 or 1) */
	UINT8				mim_bin_condition;		/* 3 (M_EQUAL), 4 (M_NOT_EQUAL), 5 (M_GREATER), 6 (M_LESS), */
												/* 7 (M_GREATER_OR_EQUAL), 8 (M_LESS_OR_EQUAL) */
												/* 3, 4 ���� �� �� ������ ������� ����, �ַ� 7,8 �� �ϳ��� �����ؼ� ����ϱ� */
												/* 7 : ���������� ���, 8 : ��� ���� ���Ѽ� ��� */
	UINT8				remove_small_particle;	/* Remove Small Particles (unit : pixel). �� ���� ũ��, ���� ��ü���� ������ */
	UINT8				u8_reserved[2];

	UINT16				remove_small_procmode;	/* M_BINARY (4096) or M_GRAYSCALE (512) */
	UINT16				thin_proc_mode;			/* M_BINARY (4096), M_BINARY2 (1), M_BINARY3 (2) or M_GRAYSCALE (512) */
	UINT16				u16_reserved[2];

}	STG_CGIP,	*LPG_CGIP;

/* Vision Camera�κ��� �������� Mark �˻��� �ʿ��� ���� */
typedef struct __st_config_vision_mark_find__
{
	UINT8				max_mark_regist;		/* �ִ� ��� ������ ��ũ �� ���� (255�� �����θ� �ȵ�) */
	UINT8				max_mark_grab;			/* ��ũ �˻��� ���� ī�޶� Grabbed Image�� �ִ� ���� (Max : 255) (Normal : 4) */
												/* !!! ���� !!! : ī�޶� ���� �˻��� �� �ִ� �ִ� ���� */
												/* ��, ī�޶� ���� �̹����� Grab�� �� �ִ� �ִ� ���� */
												/* ex> ��ϵ� ��ũ�� ������ 4���̰�, ī�޶� 1���̸�, 4 */
												/*     ��ϵ� ��ũ�� ������ 4���̰�, ī�޶� 2���̸�, 2 */
	UINT8				max_mark_find;			/* Grabbed Image���� �˻� ������ ��ũ �ִ� ���� (�ִ� 255�� ������ �ȵ�) */
												/* ex> Grabbed Image���� �˻� ������ ��ũ �ִ� ������ */
	UINT8				model_speed;			/* ��ũ �ν� �� ��Ī���� ���� ���� ���� ��������, UI���� �������� ǥ�� */
												/* �� ������ ��Ī���� �������� �뱤�� ���ϴ� ���� �ƴ϶�, ���� Ȯ�ο� (����: %) */
	UINT8				detail_level;			/* ��ϵ� �� �̹����� Grabbed Image�̹����� Edge ������ ���� �� ���� �� */
												/* ���� Ŭ���� ���� ��������, �ӵ��� ���� �������� */
												/* LOW���� HIGH�� ������ �� ���� Edge�� ������ ���ϰ� �� */
												/* 0 (M_VERY_LOW), 1 (M_LOW), 2 (M_MEDIUM), 3 (M_HIGH), 4 (M_VERY_HIGH) */
	UINT8				mixed_cent_type;		/* Mixed Mark (Multi-Mark)�� �߽� ���ϴ� ��� ���� */
												/* 0: �˻��� ��ũ���� Min/Max ��ǥ�� ���� �� �簢���� �߽� ���� ��ǥ�� ���� */
												/* 1: �˻��� ��ũ���� �ٰ��� ��ǥ�� ���� ���� �߽� ���� ��ǥ�� ���� */
												/* 2: �˻��� ��ũ���� �ٰ��� ��ǥ�� ���� ������ (������ ��ȯ)�� ���� �߽� ���� ��ǥ�� ���� */ 
	UINT8				u8_reserved[2];
	UINT16				mixed_weight_rate;		/* ���� ����� ��ũ ���� ��, ����Ǵ� ����ġ �� (1 ~ 100) (����: %) */
	UINT16				mixed_squre_rate;		/* ���� ����� ��ũ�� �ֺ� (Side)�� ��-�簢���� ����/���� ���� ���� �� (����. ����: %) �̳��� */
												/* �ִ��� �Ǵ��� ���� ���� �� (���� : um) ��, ����/���� ũ�Ⱑ �󸶳� ���簢������ �Ǵ� */
												/* ���� �� ����, 100�̸� �ֺ� (Side) ��ũ�� ���� / ���� ũ�Ⱑ ������ ���簢������ ���� */
												/* ���� �� ����,  50�̸� �ֺ� (Side) ��ũ�� ���� / ���� ũ�Ⱑ ���簢�� ���� ��, */
												/* ���� (Ȥ�� ����)�� ���� (Ȥ�� ����) ���� 50% �̻� ũ�ٸ� ��ȿ */
	UINT16				u16_reserved[2];
	DOUBLE				mixed_dist_limit;		/* ���� ����� ��ũ�� ���, Hole�� Laser ���� �߽� ��ũ ���� �ִ� ��� ���� ���� (����: mm) (�ִ� ��. 255) (�Ҽ��� 3�ڸ����� ��ȿ) */
	DOUBLE				model_smooth;			/* �⺻ ���� 75.0�̸�, ��輱�� Noise ���� �󸶴� �ε巴�� ó���ϴ� �����̸�,
												/* ���� ������ (0.0 ~ 100.0) ��, ��輱 ������ ����� ���� ���� ũ�� ����(?)
												/* 0.0 ���̸�, �⺻ �� (���� 50.0) (!!! �̹����� ������ ���� ��Ű�µ� ���� !!!) */
	DOUBLE				score_rate;				/* ��ũ �ν� �� ��Ī�� �̹����� ���� ��ϵ� Mark ũ�� ��� �־��� ������ ũ�ų� ������ ���� ó�� (����: %) */
	DOUBLE				scale_rate;				/* ��ũ �ν� �� ��Ī�� �̹����� ���� ��ϵ� Mark ũ�� ��� �־��� ������ ũ�ų� ������ ���� ó�� (����: %) */
	UINT8				image_process;			/* Mark Find �ϱ� �� �̹��� ó�� ����, 0:�̻��, 1:���*/

}	STG_CVMF,	*LPG_CVMF;
#if 0
/* Vision Camera�κ��� Calibration �˻��� �ʿ��� ���� */
typedef struct __st_config_vision_cali_find__
{
	UINT8				model_type;				/* M_CIRCLE (8), M_ELLIPSE (16), M_SQUARE (32), M_RECTANGLE (64) */
	UINT8				u8_reserved[7];
	DOUBLE				model_color;			/* BLACK (256), WHITE (128) */
	DOUBLE				model_cali;				/* Calibration Object (MODEL SIZE (unit : um) - Width or Diameter or Radius or Etc) */

}	STG_CVCF,	*LPG_CVCF;
#endif
/* ������ Edge Line Detection ���� ���� */
typedef struct __st_config_line_edge_detection__
{
	TCHAR				text_font_name[LF_FACESIZE];	/* ���� ���� �˻� ��� ���� ��µǴ� ��Ʈ �̸� (Max. 31) */

	UINT8				part_expose_repeat:1;			/* 0 : ���� �뱤�� ���, ù ���� �뱤�� ���� ���� �����۾� ���� */
														/* 1 : ���� �뱤�� ���, �� ���� �뱤�� ������ ���� �����۾� ���� */
	UINT8				box_angle_mode:1;				/* 0: ������ BOX_ANGLE���� ������ ���� ���� ���� */
														/* 1: BOX_ANGLE�� ������ �پ��� ���� �˻� ����� ������ �� ���� */
	UINT8				search_region_clip:1;			/* �̹��� ���� �������� �˻� ������ ��� ���, �ڵ����� �̹��� �������� */
														/* �˻� ������ �����ϵ��� �� ������ ���� (0 or 1) */
	UINT8				edge_find_option:2;				/* 0 : ����� ���� �߿��� �̹��� �߽ɿ��� ���� ����� �� ���� */
														/* 1 : ����� ���� �߿��� Edge Strength�� ���� ū �� ���� */
														/* 2 : ����� ���� �߿��� �̹��� �߽ɿ��� ���� ������, Edge Strength�� ���� ū �� ���� */
														/* 3 : ����� ������ ������ ������ 1���̸� ���� �뱤, 2�� �̻��̸� ����� â ��� */
	UINT8				filter_type:2;					/* 0:EULER (Default), 1:PREWITT, 2:SHEN */
														/* EULER   : ó���ӵ��� PREWITT�� ���� �������� ������� ���� (kernel size:2) */
														/* PREWITT : ó���ӵ��� EULER���� �������� ����� �� �ΰ��� (kernel size:3) */
														/* SHEN    : ���ȣ�� ������ �ϸ�, kernel size�� �̷������� ���� ���� */
														/*           ������ ��������, EULER, PREWITT�� ���� ��������� ó���ӵ��� ���� */
														/*           �߰������� FILTER_SMOOTHNESS�� �����Ͽ� ���� ���� ��� �� */
	UINT8				u8_bit1:1;
	UINT8				polarity:2;						/* 0: NULL (���� (���) ���� ����) */
														/* 1: ANY (������� ����. ������� �ʴ� �Ͱ��� �ٸ� �ǹ�) */
														/* 2: POSITIVE (rising edge. ��ο� ������ ���� ������. 0:Black -> 255:White) */
														/* 3: NEGATIVE (falling edge. ���� ������ ��ο� ������. 255:White -> 0:Black) */
	UINT8				orientation:2;					/* �˻� ���� (�ؼ�) */
														/* 1: ANY (��Ŀ (�˻�) ������ �� �� ����?) */
														/* 2: HORIZONTAL (��������, ������ �Ʒ��� �˻�) */
														/* 3: VERTICAL (��������, ���ʿ��� ���������� �˻�) */
	UINT8				u8_bits2:4;
	UINT8				filter_smoothness;				/* ������ ���� �۾� ���� (0 ~ 100) ��, FILTER_TYPE = SHEN�� ��츸 ��ȿ */
	UINT8				edge_threshold;					/* �⺻ �� 2 (0 ~ 100) */
														/* ���� �۾�����, ������ �����ϴ� ������ ������ �����ϹǷ�, �˻� �ӵ��� ������ */
														/*                �ݸ鿡, ���� ��Ȯ�� ���� �˻��� �� */
	UINT8				sub_regions_size;				/* SubRegions�� ũ�� ���� (1 ~ 100%) ���� 50% �̻����� �ϸ� ���� (?) */
	UINT8				sub_regions_number;				/* SubRegions�� ���� ���� (1 or Later. Max 100) */
														/* �����, SUB_REGIONS_SIZE�� NUMBER�� ���� 100�� ������ �ȵ� */
														/* (�� �� ������, ���� 100�� �������� �������ָ� ����) */

	UINT8				max_find_count;					/* Line Edge �˻��� ����� �ִ� ����. ���� 4���̳�, �� ���� ���� ���� */
	UINT8				text_font_size;					/* ���� ���� �˻� ��� ���� ��µǴ� ��Ʈ ũ�� (����: 1 ~ 63). ���� 20 */
	UINT8				box_size_width;					/* �˻� ���� ũ�� (����: %) (���� ������ 15 ~ 85 �̸�, ���� Ŭ���� �̹��� ���̿� ��������) */
	UINT8				box_size_height;				/* �˻� ���� ũ�� (����: %) (���� ������ 15 ~ 85 �̸�, ���� Ŭ���� �̹��� ���̿� ��������) */
														/* BOX_SIZE �������� SIZE ũ�⾿ NUMBER ������ŭ �˻��϶�� ���� */
	UINT8				box_angle_delta_neg;			/* BOX_ANGLE �������� �� �� ���� �������� �˻� ���� (0 ~ 180) */
	UINT8				box_angle_delta_pos;			/* BOX_ANGLE �������� �� �� ���� �������� �˻� ���� (0 ~ 180) */

	INT8				sub_regions_offset;				/* SubRegions�� ��ġ ���� ���� (-100% ~ 0 ~ +100%) */
	INT8				i8_reserved[3];

	UINT16				box_angle;						/* Box search region�� ���� ���� ��, �˻� ������ �簢�� �ڽ��� ȸ�� ���� �� */
														/* ���� �˻� ����� ������ �� ���� ȸ���� ������ �� (0 ~ 360) ���� 0 */
	UINT16				u16_reserved[3];

	DOUBLE				grab_image_edge_angle;			/* Grabbed Image�� ���� �˻��� �� �˻� ���� ���� (ū �ǹ̴� ����) */
	DOUBLE				box_angle_accuracy;				/* Marker�� �뷫���� ��ġ ã�� ��, ���� �˻��� ������ Step ũ�� (0.1 ~ 180.0) */
														/* 0.0 ���� �Է��� ���, DISABLE�μ� Tolerance (��� ����) ������ ������ ũ�Ⱑ �Էµ� */
	DOUBLE				print_diff_limit;				/* �뱤 ���� ��ġ�� Edge Line �˻� ��� ��ġ ���� �Ÿ� ���� �Ѱ� (����: mm) */
														/* ���� �� (�뱤 ���� ��ġ - Edge Line �˻� ��� ��ġ (��� �˻� ���� ��ġ ���)) */
														/* ��, �뱤 ���� ��ġ�� �˻��� ��ġ ���� �Ÿ� ������ �� ������ ũ�� ���� ��Ʋ���� ���� */
														/* �ִ� �Է� ������ �� (Max: 2 mm) */

}	STG_CLED,	*LPG_CLED;

/* Match Find Option - Vision Edgein Detection */
typedef struct __st_config_mark_edge_detection__
{
	UINT8				filter_type:4;			/* 1:SHEN or 2:DERICHE, 3:PREWITT, 4:SOBEL, 5:FREICHEN */
	UINT8				threshold:4;			/* 1:Very High, 2:High, 3:Medium, 4:Low */
	UINT8				gaussian_filer:4;		/* Gaussian Filter �� : 0 ~ 9 */
	UINT8				use_float_mode:1;		/* Float Mode ��� ���� (0 or 1) */
	UINT8				use_closed_line:1;		/* �˻��� ������ �� ������ ����� �͸� ���� */
	UINT8				inc_internal_edge:1;	/* �˻��� ������ �߿��� ���ο� ���Ե� ���� �˻� ���� (0 or 1) */
	UINT8				edge_analysis_save:1;	/* 1 : �м��� �̹��� ���� (edge), 0 : �м��� �̹��� ���� ���� */
	UINT8				u8_reserved[6];

	INT32				m_accuracy;				/* -9999 (Disable), 3 (High), 4 (Very High) */
	INT32				i32_reserved;

	UINT32				filter_smoothness;		/* FILTER �˻� ���� �� (�⺻�� : 70, �ִ� 100, �ּ� 0) */
	UINT32				max_find_count;			/* �ִ� �˻��� �� �ִ� ���� */

	DOUBLE				min_dia_size;			/* �ּ� �˻� ũ�� (�˻��� ���� ��ü ����) (����: mm) */
	DOUBLE				max_dia_size;			/* �ִ� �˻� ũ�� (�˻��� ���� ��ü ����) (����: mm) */
	DOUBLE				extraction_scale;		/* 1.0 or later */
	DOUBLE				moment_elongation;		/* ���弱�� ���� �� (0.1 ~ 1.0) �Ƹ� ���� ���� ��� ��ü ã�� ���ɼ� ���� */
	DOUBLE				mark_spare_size;		/* ���� ������ ����� ��ũ ũ�⸦ �� ũ���� �� Ȥ�� ����� �� �����ϱ� ���� �� (����: um) */
	DOUBLE				fill_gap_continuity;	/* ���� ������ ������ ������ ������, ���� ��������� �ε巯�� �� ���� �Ұ����� ���� �� ���� */

	/*
	 desc : �� ��ȿ�� Ȯ��
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
	DOUBLE				measure_pos[6][3];	/* ���� �� �̵� ��ǥ [Head Number][X, Y, Z] (���� : mm) */

	/* Measure Table */
	UINT16				begin_power_index;	/* ���� ���� �ε��� �� */
	UINT16				measure_count;		/* ���� Ƚ�� */
	UINT16				interval_power;		/* ���� Ƚ����ŭ ��µ� �ε��� �� */

	/* Find Max Ill Pos */
	UINT8				max_points;			/* �ִ� ���� ��ġ�� ã�� �� ���� ����Ʈ �� */
	DOUBLE				pitch_pos;			/* �� ����Ʈ �� �Ÿ� �� */
	DOUBLE				focus_scan_pos;		/* ��� ��Ŀ�� ���� �Ÿ� �� */

	/* Speed */
	DOUBLE				stage_speed;		/* �������� �̵� �ӵ� */
	DOUBLE				head_speed;			/* ��� �̵� �ӵ� */

	/* Manual */
	UINT16				manual_power_index;	/* ���� ���� �ε��� �� */

	DOUBLE				normal_boundary;	/* ���� ������ ���� �� */
	DOUBLE				warning_boundary;	/* ��� ������ ���� �� */

}	STG_CLPI, * LPG_CLPI;

/* Photohead Focus Parameter */
typedef struct __st_config_photohead_focus_info__
{
	UINT8				print_type;				/* 0 : X = Focus Y = Energy, 1 : X = Energy Y = Focus */
	UINT16				step_x_count;			/* Stage X �������� �̵��ϴ� �� STEP ���� */
	UINT16				step_y_count;			/* Stage Y �������� �̵��ϴ� �� STEP ���� (STEP_MOVE_Y * STE_MOVE_COUNT = �� ���� Y �� �ִ� �̵� �Ÿ� ���� �ʾƾ� ��) */

	DOUBLE				start_foucs[MAX_PH];	/* ��Ŀ�� ���� ��ġ (����: mm) */
	DOUBLE				start_energy;			/* ���� ���� ��ġ */
	DOUBLE				step_foucs;				/* ��Ŀ�� ���� �� (����: mm) */
	DOUBLE				step_energy;			/* ���� ���� �� */

	DOUBLE				expo_stage[2];			/* ó�� �뱤�� ��ġ�� �̵��ϱ� ���� Motion ��ġ (����: mm) (���а迡 ���޵� ��ġ��. �Ҽ��� 3�ڸ����� ��ȿ) */

	DOUBLE				mark2_stage[2];			/* ó�� �뱤�� ��ũ�� ��ġ�� �̵��ϱ� ���� Motion ��ġ (����: mm) (�Ҽ��� 4�ڸ����� ��ȿ) */
	DOUBLE				mark2_acam_x[2];		/* ó�� �뱤�� ��ũ�� ��ġ�� �̵��ϱ� ���� Motion ��ġ (����: mm) (�Ҽ��� 4�ڸ����� ��ȿ) */
	DOUBLE				step_move_x;			/* Stage X �������� �̵��ϱ� ���� ��ũ�� ��ũ�� �̵� ���� (����: mm) (�Ҽ��� 4�ڸ����� ��ȿ) */
	DOUBLE				step_move_y;			/* Stage Y �������� �̵��ϱ� ���� ��ũ�� ��ũ�� �̵� ���� (����: mm) (�Ҽ��� 4�ڸ����� ��ȿ) */

	DOUBLE				mark_period;			/* PHOTOHEAD ���� ������ �Ÿ� ��, Mark ���� ������ �� ���� (����:  mm) (�Ҽ��� 4�ڸ����� ��ȿ) */

	DOUBLE				model_dia_size;			/* �˻��� ���� ��ü�� �߿��� �� ���� ���� ���� ����� ���� ���� (����: mm) (�Ҽ��� 3�ڸ����� ��ȿ) */
	DOUBLE				model_color;			/* 128 (���), 256 (������) */

}	STG_CPFI,	*LPG_CPFI;

/* Align Camera Calibration Parameter */
typedef struct __st_config_align_camera_calibration__
{
	UINT8				set_match_mode;					/* ���� ��ũ �˻� ���. 0 : Circle �˻�, 1 : Ring �˻� */
	UINT8				u8_reserved[7];
	UINT16				max_rows_count;					/* X ������ ��� �̵��ϸ鼭 ������ �� �ִ� �ִ� ���� */
	UINT16				max_cols_count;					/* Y ������ ��� �̵��ϸ鼭 ������ �� �ִ� �ִ� ���� */
	/* Ȯ�� (����) ���������� �Ʒ� 2�� ���� ������� ���� */
	UINT16				set_rows_count;					/* �� (Row) ���� ���� */
	UINT16				set_cols_count;					/* �� (Column) ���� ���� */

	UINT32				model_ring_type;				/* Circle (8), Ellipse (16), Square (32), Rectangle (64), Ring (256), */
	UINT32				model_shut_type;				/* Circle (8), Ellipse (16), Square (32), Rectangle (64), Ring (256), */
														/* Cross (8192), Diamond (32768), Triangle (65536), Image (?????) */
	UINT32				u32_reserved[1];

	UINT64				period_wait_time;				/* Calibration �����ϱ� �� ���Ͱ� ���߰� �� ���� ������ ������ �ּ��� ��� �ð� (����: msec) */
	DOUBLE				period_min_size;				/* X/Y ������ �̵��� ��, �̵� �ּ� �Ÿ� (����: mm) (�Ҽ��� 4�ڸ����� ��ȿ) */
	/* Ȯ�� (����) ���������� �Ʒ� 2�� ���� ������� ���� */
	DOUBLE				period_row_size;				/* �� (Row) �̵� (����) ���� (����:  mm) */
	DOUBLE				period_col_size;				/* �� (Column) �̵� (����) ���� (����:  mm) */

	DOUBLE				model_shut_color;				/* 128 (���), 256 (������) */
	DOUBLE				model_ring_color;				/* 128 (���), 256 (������) */

	DOUBLE				model_shut_size;				/* ��ϵ� �� (Circle) �� ũ�� (����: mm) (����) */
	DOUBLE				model_ring_size1;				/* ��ϵ� �� (Ring) �� ���� ũ�� (����: mm) (����) */
	DOUBLE				model_ring_size2;				/* ��ϵ� �� (Ring) �� �ܺ� ũ�� (����: mm) (����) */
#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
	DOUBLE				mark_stage_x;					/* ���� ���� (�β�)�� ������ ���� ��ũ (�뱤 ��ü) ���� ��ġ. Stage X (����: mm) */
	DOUBLE				mark_stage_y[MAX_ALIGN_CAMERA];	/* ���� ���� (�β�)�� ������ ���� ��ũ (�뱤 ��ü) ���� ��ġ (Align Camera 1�� ������ ��). Stage Y (����: mm) */
#else
	DOUBLE				mark_stage_x[MAX_ALIGN_CAMERA];	/* ���� ���� (�β�)�� ������ ���� ��ũ (�뱤 ��ü) ���� ��ġ. Stage X (����: mm) */
	DOUBLE				mark_stage_y[MAX_ALIGN_CAMERA];	/* ���� ���� (�β�)�� ������ ���� ��ũ (�뱤 ��ü) ���� ��ġ (Align Camera 1�� ������ ��). Stage Y (����: mm) */
#endif
	DOUBLE				mark_acam[MAX_ALIGN_CAMERA];	/* ���� ���� (�β�)�� ������ ���� ��ũ (�뱤 ��ü) ���� ��ġ. Camera X (����: mm) (�Ҽ��� 4�ڸ����� ��ȿ) */
	DOUBLE				scale_valid_rate;				/* ã���� �ϴ� ��ũ�� ũ�� ���� ���� 1.0 �� ��������, �� �� ���� (+/-) ���� �־�� �� (���� 0.1 �� ���� �ָ� ��) (�Ҽ��� 4�ڸ����� ��ȿ) */
	DOUBLE				score_valid_rate;				/* ã���� �ϴ� Model�� ��Ī ���� ���� �� �� ���� ���ƾ� �� (���� 90.0000) �� ���� % ������ ����. �� 95% ���� �̻� ã�� ���ϸ� 95.0000 (�Ҽ��� 4�ڸ����� ��ȿ) */
	DOUBLE				motion_speed;					/* Camera 1, Stage X, Stage Y ���� �̵� �ӵ� (unit: mm / sec) (�Ҽ��� 4�ڸ����� ��ȿ) */
	DOUBLE				err_limit_value;				/* ���� �Ѱ� �� ��, �� ���� �����, ���� ������ �Ǵ� (����: mm) ���α׷��� �׸��� �������� ������ ��� (CELL)�� "���� ����" ǥ�� */

	/*
	 desc : ��ü ���� ���� ��ȯ
	 parm : None
	 retn : ���� ����
	*/
	UINT32 GetTotalCount()
	{
		return UINT32(set_cols_count) * UINT32(set_rows_count);
	}

	/*
	 desc : ���� �Ѱ� �� ��ȯ (����: um)
	 parm : None
	 retn : ���� �Ѱ� �� ��ȯ
	*/
	DOUBLE GetErrLimitValUm()
	{
		return (DOUBLE)ROUNDED(err_limit_value * 1000.0f, 1);
	}

}	STG_CACC,	*LPG_CACC;

/* Stage xy straightness (motion xy straightness) */
typedef struct __st_config_stage_motion_straightness__
{
	DOUBLE				model_meas_color;				/* 128 (���), 256 (������) */
	DOUBLE				model_meas_size;				/* ��ϵ� �� (Circle) �� ũ�� (����: mm) (����) */
	DOUBLE				scale_valid_rate;				/* ã���� �ϴ� ��ũ�� ũ�� ���� ���� 1.0 �� ��������, �� �� ���� (+/-) ���� �־�� �� (���� 0.1 �� ���� �ָ� ��) (�Ҽ��� 4�ڸ����� ��ȿ) */
	DOUBLE				score_valid_rate;				/* ã���� �ϴ� Model�� ��Ī ���� ���� �� �� ���� ���ƾ� �� (���� 90.0000) �� ���� % ������ ����. �� 95% ���� �̻� ã�� ���ϸ� 95.0000 (�Ҽ��� 4�ڸ����� ��ȿ) */
	DOUBLE				motion_speed;					/* Camera 1, Stage X, Stage Y ���� �̵� �ӵ� (unit: mm / sec) (�Ҽ��� 4�ڸ����� ��ȿ) */
	DOUBLE				err_limit_value;				/* ���� �Ѱ� �� ��, �� ���� �����, ���� ������ �Ǵ� (����: mm) ���α׷��� �׸��� �������� ������ ��� (CELL)�� "���� ����" ǥ�� */

}	STG_CSMS,	*LPG_CSMS;

/* Align Camera Spec. Information */
typedef struct __st_config_acam_spec_info__
{
	DOUBLE				point_dist;							/* ī�޶� FOV �̳��� ���� �� �ȼ� ũ�⸦ �����ϱ� ���� �� ���� ������ �Ÿ� �� (����: mm) */
	DOUBLE				step_move_z;						/* Align Camera Up/Donw �������� �̵��ϱ� ���� �̵� ���� (����: mm) (�Ҽ��� 3�ڸ����� ��ȿ) */
	DOUBLE				acam_z_focus[MAX_ALIGN_CAMERA];		/* Align Camera ���� ��ġ�� Camera Z Axis ���� �� (����: mm) (�Ҽ��� 4�ڸ����� ��ȿ) : ���� ���� */
	DOUBLE				quartz_stage_x;						/* Align Camera Specification ������ ���� QuartZ ���� ��ġ. Stage X (����: mm) (�Ҽ��� 4�ڸ����� ��ȿ) */
	DOUBLE				quartz_stage_y[MAX_ALIGN_CAMERA];	/* Align Camera Specification ������ ���� QuartZ ���� ��ġ (Align Camera 1�� ������ ��). Stage Y (����: mm) (�Ҽ��� 4�ڸ����� ��ȿ) */
	DOUBLE				quartz_acam[MAX_ALIGN_CAMERA];		/* Align Camera Specification ������ ���� QuartZ ���� ��ġ. Camera X (����: mm) (�Ҽ��� 4�ڸ����� ��ȿ) */
	DOUBLE				in_ring_size;						/* ������Ʈ�� ������ ������ �� �� ���� ũ�� (����: mm)mm) (�Ҽ��� 3�ڸ����� ��ȿ ���� ���� �� 1000 um, �ٱ� �� 2000 um */
	DOUBLE				in_ring_color;						/* ������Ʈ�� ������ ������ �� �� ������ ���� ä���� ���� (256:Black, 128:white) */
	DOUBLE				in_ring_scale_min;					/* ���� ���� ã�� ���� �� (Scale ���� ��) 0.95 (0.5 ~ 1.0) */
	DOUBLE				in_ring_scale_max;					/* ���� ���� ã�� ���� �� (Scale ���� ��) 1.05 (1.0 ~ 2.0) */
	DOUBLE				spec_angle[MAX_ALIGN_CAMERA];		/* ������ Camera�� ��ġ�� ȸ�� ���� �� (����: degree) */
	DOUBLE				spec_pixel_um[MAX_ALIGN_CAMERA];	/* ������ Camera�� 1 �ȼ� ũ�� (����: um)*/
	DOUBLE				bracket_size_w;						/* ����� ī�޶� �����ϰ� �ִ� �������� [��] ũ�� (�������� ����) (����: mm) */
	DOUBLE				bracket_size_d;						/* ����� ī�޶� �����ϰ� �ִ� �������� [����; �β�] ũ�� (�������� ����) (����: mm) */

	/*
	 desc : ������(?)���� ������ 2 ���� ī�޶� ���� [�߽�] ���� �� ���
	 parm : dist_x	- [out] X ������ ������ �Ÿ� (����: mm) (ACam.X1 - ACam.X2)
			dist_y	- [out] Y ������ ������ �Ÿ� (����: mm) (ACam.Y1 - ACam.Y2)
	 retn : TRUE or FALSE
	 note : Align Camera�� �ݵ�� 2�� �־�߸� ��
	*/
	VOID GetACamCentDist(DOUBLE &dist_x, DOUBLE &dist_y)
	{
		dist_x	= quartz_acam[0] - quartz_acam[1];			/* ��/�� ���� ���� (����) */
		dist_y	= quartz_stage_y[0] - quartz_stage_y[1];		/* ��/�� ���� ���� (����) */
	}
	DOUBLE GetACamCentDistX()	{	return	(quartz_acam[0] - quartz_acam[1]);			}	/* X �� ���� */
	DOUBLE GetACamCentDistY()	{	return	(quartz_stage_y[0] - quartz_stage_y[1]);	}	/* Y �� ���� */

	/*
	 desc : ������(?)���� ������ 2 ���� ī�޶� ���� [�ּ�] ���� �� ���
			��, �ε����� ���� �ּ����� �Ÿ� (ī�޶� 1�� ������ ī�޶� 2�� ������ ���� ���� ���� �ּ� ����)
	 parm : dist_x	- [out] X ������ ������ �Ÿ� (����: mm) (ACam.X1 - ACam.X2)
			dist_y	- [out] Y ������ ������ �Ÿ� (����: mm) (ACam.Y1 - ACam.Y2)
	 retn : TRUE or FALSE
	 note : Align Camera�� �ݵ�� 2�� �־�߸� ��
	*/
	DOUBLE GetACamMinDistH()	{	return	GetACamCentDistX()+bracket_size_w;		}	/* ���� ���� �ε����� ���� 2�� ī�޶� ���� �ּ����� ���������� ���� */
	DOUBLE GetACamMinDistD()	{	return	bracket_size_d;							}	/* ���� ���� �ε����� ���� 2�� ī�޶� ���� �ּ����� ���������� ���� */

	/*
	 desc : Pixel To MM ũ�� ��ȯ
	 parm : cam_id	- [in]  Align Camera Index (Zero based)
	 retn : Pixel ũ�⸦ um -> mm ��ȯ
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
	DOUBLE				step_move_z;						/* Align Camera Up/Donw �������� �̵��ϱ� ���� �̵� ���� (����: mm) (�Ҽ��� 3�ڸ����� ��ȿ) */
	DOUBLE				mark2_stage_x;						/* Align Camera Specification ������ ���� QuartZ ���� ��ġ. Stage X (����: mm) (�Ҽ��� 4�ڸ����� ��ȿ) */
	DOUBLE				mark2_stage_y[MAX_ALIGN_CAMERA];	/* Align Camera Specification ������ ���� QuartZ ���� ��ġ (Align Camera 1�� ������ ��). Stage Y (����: mm) (�Ҽ��� 4�ڸ����� ��ȿ) */
	DOUBLE				mark2_acam[MAX_ALIGN_CAMERA];		/* Align Camera Specification ������ ���� QuartZ ���� ��ġ. Camera X (����: mm) (�Ҽ��� 4�ڸ����� ��ȿ) */
	DOUBLE				model_dia_size;						/* �˻� ����� Model ũ�� (����: um) (Model Type�� ������ Circle) (ũ�� ����) */
	DOUBLE				acam_z_focus[MAX_ALIGN_CAMERA];		/* Align Camera ���� ��ġ�� Camera Z Axis ���� �� (����: mm) : ���� ���� (SET_ALIGN -> DOF_FILM_THICK ���� */

}	STG_CAFI,	*LPG_CAFI;

/* Trigger Set Parameter */
typedef struct __st_config_trigger_set_parameter__
{
	UINT8				port_no;							/* �ø��� ��� ��Ʈ ��ȣ 2 ~ 19 */
	UINT8				ch_count;							/* ��ǰ�� ����� Ʈ���� ���� (���� ¦�� ����) */
	UINT8				u8_reserved[7];
	INT32				trig_forward;						/* Area Trigger (1 ~ 16) ���� �� ���� <����/����>�Ǿ�, */
															/* Strobe�� Trigger�� ��ȣ�� ������/������ �Ǿ� ��� �� (Trigger Count ��) */
	INT32				trig_backward;						/* Area Trigger (1 ~ 16) ���� �� Register ���� <����>�Ǿ� */
															/* Strobe�� Trigger�� ��ȣ�� ������/������ �Ǿ� ��� �� (Trigger Count ��) */
	UINT32				trig_on_time[MAX_TRIG_CHANNEL];		/* high�� �����Ǵ� ���� �Է� (����: ns) */
	UINT32				strob_on_time[MAX_TRIG_CHANNEL];	/* high�� �����Ǵ� ���� �Է� (����: ns) */
	UINT32				trig_delay_time[MAX_TRIG_CHANNEL];	/* ��µǴ� Trigger�� Delay �� �Է� (����: ns) */
															/* �� Register ���� 0    �̸�, Strobe�� Trigger ��ȣ�� ���ÿ� ��� �� */
															/* �� Reigster ���� 1000 �̸�, Strobe�� ��ȣ�� ��µǰ�, */
															/* 1000 ns �� ��ŭ ��� (����) �Ŀ� Trigger�� ��ȣ�� ��� �� */
	UINT32				trig_plus[MAX_TRIG_CHANNEL];		/* Area Trigger (1 ~ 16) ���� �� Register ���� <����>�Ǿ�, */
															/* Strobe�� Trigger�� ��ȣ�� �����Ǿ� ��� �� (����: ns) */
															/* Strobe�� Trigger�� ��ȣ�� �����Ǿ� ��� �� */
	UINT32				trig_minus[MAX_TRIG_CHANNEL];		/* Area Trigger (1 ~ 16) ���� �� Register ���� <����>�Ǿ� */
															/*  Strobe�� Trigger�� ��ȣ�� ������ ��� �� (����: ns) */
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
	INT32				global[MAX_ALIGN_CAMERA][MAX_GLOBAL_MARKS];	/* Global Mark�� Ʈ���� �߻� ��ġ */
	INT32				local[MAX_ALIGN_CAMERA][MAX_LOCAL_MARKS];	/* Local  Makr�� Ʈ���� �߻� ��ġ */

}	STG_CMTP,	*LPG_CMTP;

/* ���� ���� �̸� */
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
	UINT8				cam_reverse_x:1;					/* ī�޶� Grabbed Image ���� ���� (��/��) */
	UINT8				cam_reverse_y:1;					/* ī�޶� Grabbed Image ���� ���� (��/��) */
	UINT8				u8_bits_reserved:6;
	UINT8				u8_reserved[7];

	UINT8				cam_ip[MAX_ALIGN_CAMERA][4];		/* The Camera IPv4 for Basler */
	UINT8				cam_mc2_drv[MAX_ALIGN_CAMERA];		/* MC2 SD2S ���� ����̺� ��ȣ (���� ���������� ���, 0, 1 ����) */
	UINT8				cam_gain_level[MAX_ALIGN_CAMERA];	/* Align Camera Gain Level �� (0 ~ 255) */

	UINT16				acq_frames[MAX_ALIGN_CAMERA];		/* Sets the number of frames acquired in the multiframe acquisition mode */
	/* Sets the raw's expose time. (unit : us) (min : 16) */
	UINT32				grab_times[MAX_ALIGN_CAMERA];		/* GRAP_xx : PODIS App���� ���� ī�޶��� Expose Time */
	UINT32				step_times[MAX_ALIGN_CAMERA];		/* STEP_xx : VDOF App���� ���� ī�޶��� Expose Time */

	DOUBLE				z_axis_move_max;					/* Z Axis �̵� �ּ� / �ִ� �� ��, 0 : Min, 1 : Max (����: mm) */
	DOUBLE				z_axis_move_min;					/* Z Axis �̵� �ּ� / �ִ� �� ��, 0 : Min, 1 : Max (����: mm) */

}	STG_CCDB,	*LPG_CCDB;

/* Photohead Step XY ���� */
typedef struct __st_config_phothead_offset_value__
{
	UINT8				scroll_mode;						/* ���а� ���� ������ ���� ������ �뱤 �ӵ� ��� (1 ~ 7)					*/
	UINT8				ph_scan;							/* ���а踶�� �뱤�� �� ��/�Ϸ� �����̴� Scan ���� ��, Stripe �� ����		*/
	UINT8				max_ph_step;						/* ���а� ���� �����ǿ� �ִ� ��� ������ ������ ���� */
	UINT8				expose_round;						/* ���� Ȯ���� ���� ���� �뱤 Ƚ�� */
	UINT8				u8_reserved[4];
	UINT32				model_type;							/* Circle (8), Ellipse (16), Square (32), Rectangle (64), Ring (256),		*/
															/* Cross (8192), Diamond (32768), Triangle (65536), Image (?????)			*/
	UINT32				u32_reserved;
	DOUBLE				model_color;						/* Black (256), White (128)													*/
	DOUBLE				model_dia_size;						/* �˻��� ���� ��ü�� �߿��� �� ���� ���� ���� ����� ���� ���� (����: mm)	*/
															/* (�Ҽ��� 3�ڸ����� ��ȿ) (�˻� ����� ������ ��. ũ��� !!! ���� !!!)		*/
	DOUBLE				start_xy[2];						/* �� ó�� ������ ��ũ�� �ִ� ��� ��ġ (����: mm) (�Ҽ��� 4�ڸ����� ��ȿ)	*/
	DOUBLE				start_acam_x;						/* �� ó�� ������ ��ũ�� �ִ� ��� ��ġ (����: mm) (�Ҽ��� 4�ڸ����� ��ȿ)	*/
	DOUBLE				stripe_width;						/* ���а谡 �� �� �뱤�� ������ �׷����� STRIPE�� ũ�� (����: mm)			*/
	DOUBLE				mark_period;						/* 2 ���� Mark ���� ������ ���� ��, ������ ���� �߽� ���� �Ÿ�(����: mm)	*/
	DOUBLE				center_offset;						/* �� ���� Ŭ���� ���� �˻��ϳ�, ��Ȯ���� ������. �ּ� 0.1 mm �̻� (����:mm)*/

	DOUBLE				y_pos_plus;							/* ���� ���� ������ ���� Y�� �Ÿ� (����:mm) */

}	STG_CPOV,	*LPG_CPOV;

/* DI ��� ������ �µ� �� ���� */
typedef struct __st_di_internal_temp_range__
{
	DOUBLE				hot_air[2];			/* ���� �µ� �� ���� ���� (������) */
	DOUBLE				di_internal[2];		/* ���� �µ� �� ���� ���� (DI ��� ����) */
	DOUBLE				optic_led[2];		/* ���� �µ� �� ���� ���� (���а� LED) */
	DOUBLE				optic_board[2];		/* ���� �µ� �� ���� ���� (���а� ����) */

	/*
	 desc : �Ӱ�ġ �ִ� �� ��ȯ
	 parm : mode	- [in]  0x00: ���� ��, 0x01 : �ּ� ��, 0x02 : �ִ� ��
			type	- [in]  0x00:������, 0x01:di internal, 0x02: led, 0x03: board
	 retn : ���� �µ� �� ��ȯ
	*/
	DOUBLE GetTempDeg(UINT8 mode, UINT8 type)
	{
		DOUBLE *pTemp	= NULL;

		switch (type)
		{
		case 0x00:	pTemp	= hot_air;		break;	/* ������ */
		case 0x01:	pTemp	= di_internal;	break;	/* DI Internal */
		case 0x02:	pTemp	= optic_led;	break;	/* PH LED */
		case 0x03:	pTemp	= optic_board;	break;	/* PH Board */
		}
		if (!pTemp)	return 0.0f;

		/* ���� ��, �ּ� ��, �ִ� �� */
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
	UINT8				use_trans;			/* 0 : �Ʒ� �Ķ���� ��� ����, 1 : �Ʒ� �Ķ���� ����� */
	UINT8				use_rectangle;		/* 0 : ������� ����, 1 : ����� */
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

/* PODIS ���� ���� */
typedef struct __st_config_info_podis_common__
{
	UINT8				check_hot_air:1;		/* 0:������ ���� Ȯ������ ����. 1:������ ���ۿ��� Ȯ�� */
	UINT8				check_chiller:1;		/* 0:ĥ�� ���� Ȯ������ ����. 1:ĥ�� ���ۿ��� Ȯ�� */
	UINT8				check_vacuum:1;			/* 0:��Ũ ���� Ȯ������ ����. 1:��Ũ ���ۿ��� Ȯ�� */
	UINT8				check_material:1;		/* 0:�뱤 ���� ���� Ȯ������ ����. 1:�뱤 ���� ���翩�� Ȯ�� */
	UINT8				check_start_led:1;		/* 0:��� ���� Start LED ��ư Ȯ������ ���� 1:���� Ȯ���� */
	UINT8				check_query_expo:1;		/* ���� ����� ���� �� �뱤 ���� ���� ����� ���� ���� */
												/* 0:����� �� ���� ����, 1:�ѹ��� �뱤�� ������ ������ ����� */
	UINT8				use_vision_lib:1;		/* 0 : Vision Library ��� ����, 1 : Vision Library ��� �� */
	UINT8				load_recipe_homing:1;	/* ������ ������ ��, ���а� Homing ���� ���� (0 : Not homing, 1 : Homing) */
	UINT8				uu_reserved[7];
	TCHAR				recipe_name[MAX_FILE_LEN];	// by sysandj : ���� RECIPE ����
}	STG_CIPC,	*LPG_CIPC;

/* ������ �µ� �ڵ� ���� */
typedef struct __st_config_hotair_temp_control__
{
	UINT8				use_auto_hot_air;		/* 0:������ �µ� �ڵ� ���� ����. 1:������ �µ� �ڵ� ���� �� */
	UINT8				auto_temp_rate;			/* ABS(22.0 �� - 25.0 ��) = 3.0 �� ��, ���⼭ 3.0 �� ����, �� ���� �� (�����; %)�� */
												/* ���ؼ�, ������ ���� (����)���� ���ϰų� ���ָ� �� */
												/* ex> [TEMP_RANGE] -> HOT_DI_DEV_TEMP (22.0 ��), DI ���� ���� �µ� �� (25.0 ��)�̰�, */
												/*     AUTO_TEMP_RATE ���� 50 % �̶�� => abs(21.5 - 24.5) = 3.0 �� �̰�, */
												/*     3.0 �� x 0.5 (50%) = 1.5 �� �̹Ƿ�, DI ���� �µ��� �ö� �����Ƿ�, */
												/*     ���� ������ ���� ���� 21.5 �� - 1.5 �� = 20.0 �� �� ���� ��. */
	UINT8				temp_holding_time;		/* DI ���� �µ� ���� ���� �� (TEMP_MONITOR_TIME)�ð� (����: minute)���� �����Ǹ�, */
												/* ������ �µ� �ڵ� ���� (�ִ� �� 60) */
												/* ex> TEMP_MONITOR_TIME ���� 5 min�� �����Ǿ� �ִٸ�,  */
												/*     [TEMP_RANGE] -> HOT_DI_DEV_TEMP ���� ���� DI ���� �µ� ���� 3.0 �� �̻� */
												/*     ���̰� 5 min �̻󵿾� �����Ǹ�, ������ �µ� ������ �ڵ����� ���� �� */
	UINT8				u8_reserved[5];
	DOUBLE				auto_temp_range;		/* [TEMP_RANGE]->HOT_DI_DEV_TEMP ���� �µ� ���� ���� DI ���� �µ� ���� */
												/* ���̰� �� (AUTO_TEMP_RANGE)�� ���� ũ��, �ڵ� ���� ���� �� (�ִ밪 10. �Ǽ�) */
												/* ex> [TEMP_RANGE] -> HOT_DI_DEV_TEMP ���� 22.0 ��, */
												/*     [AUTO_HOT_AIR] -> AUTO_TEMP_RANGE ���� 2.0 �� �� ������ ���·� ����. */
												/*     ���� DI ���� �ǽð� �µ��� 25.0 �� ���, ABS(22.0 �� - 25.0 ��) = 3.0 �� ��, */
												/*     [TEMP_RANGE] -> HOT_DI_DEV_RANGE ���� (2.0 ��) ���� ũ�Ƿ�, �ڵ� ���� ���� */

}	STG_CHTC,	*LPG_CHTC;


/* Measure Flatness Parameter */
typedef struct __st_config_measure_auto_flatness__
{
private:
	std::mutex mtx;
public:

	UINT8 u8UseMotorT;							/* 0:�ش� �� ���� ������. 1:�ش� �� ���� ����� */
	UINT8 u8UseMotorZ;							/* 0:�ش� �� ���� ������. 1:�ش� �� ���� ����� */

	DOUBLE dSpeedX;								/* X �̵� �ӵ� */
	DOUBLE dSpeedY;								/* Y �̵� �ӵ� */
	DOUBLE dSpeedT;								/* T �̵� �ӵ� */
	DOUBLE dSpeedZ;								/* Z �̵� �ӵ� */
	DOUBLE dStartXPos;							/* X�� ���� ��ġ */
	DOUBLE dStartYPos;							/* Y�� ���� ��ġ */
	DOUBLE dStartTPos;							/* T�� ���� ��ġ */
	DOUBLE dStartZPos;							/* Z�� ���� ��ġ */
	DOUBLE dEndXPos;							/* X�� ���� ��ġ */
	DOUBLE dEndYPos;							/* Y�� ���� ��ġ */
// 	DOUBLE dIntervalX;
// 	DOUBLE dIntervalY;

	UINT8 u8CountOfXLine;						/* X�� ���� ���� Ƚ�� */
	UINT8 u8CountOfYLine;						/* Y�� ���� ���� Ƚ�� */
	UINT8 u8CountOfMeasure;						/* �� ��ġ ���� �� ���� Ƚ�� */
	UINT16 u16DelayTime;						/* �� ���� �� ���� �ð� */

	BOOL bThieckOnOff;							/*LDS ���� ���� ���� On/Off*/
	DOUBLE dRangStartYPos;						/*LDS ���� Y�� ������ ���� ��ġ*/
	DOUBLE dRangEndYPos;						/*LDS ���� Y�� ������ �� ��ġ*/
	
	UINT8	u8UseThickCheck;					/*LED ���� ���� ��� ���� Ȯ��*/
	DOUBLE	dOffsetZPOS;						/*LDS ���� �������� ���� ������*/
	DOUBLE	dLimitZPOS;							/*LDS ���� �������� �������� ���� �ִ� ������*/

	vector<DOUBLE>* dAlignMeasure;						/*LDS ���� ��*/
	

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
		return dAlignMeasure->at(measureCnt >> 1); // �߰��� ��ȯ
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


/* Engine ���� ���� */
typedef struct __st_config_info_engine_all__
{
	STG_CIPC			set_uvdi15;		/* UVDI15 ���� ���� */
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
	STG_CMTP			mark_trig;		/* mark���� Ʈ���Ű� �߻��� ��ġ �� ���� */
	STG_CPOV			ph_step;
	STG_DITR			temp_range;
	STG_GMLV			mark_diff;		/* 6 �� (LT-RT, LB-RB, LT-LB, RT-RB, LT-RB, LB-RT) ������ ���� ���� ���� */
	STG_CGTI			global_trans;
	STG_CHTC			auto_hot_air;	/* ������ �ڵ� ���� */
	STG_CCSV			cmps_sw;
	STG_CPSP			set_philhmi;		// by sysandj : philhmi ���� �Ķ����
	STG_CSSP			set_strobe_lamp;	// by sysandj : strobe lamp ���� �Ķ����
	STG_KLSP			set_keyence_lds;	// 230919 mhbaek Add keyence lds ���� �Ķ����
	STG_CMAF			measure_flat;		// 230919 mhbaek Add

	/*
	 desc : Align Camera�� Grabbed Image�� ���� or ���� ��ȯ
	 parm : flag	- [in]  0x00: Width, 0x01: Height
	 retn : ĸó�Ǵ� �̹����� ũ�� ��ȯ (����: pixel)
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
	 desc : Align Camera�� Grabbed Image�� ���� or ���� ����
	 parm : flag	- [in]  0x00: Width, 0x01: Height
			size	- [in]  �̹��� ũ�� (����: Pixel)
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
	 desc : Align Camera�� Grabbed Image�� ũ�� ��ȯ
	 parm : None
	 retn : ĸó�Ǵ� �̹����� ũ�� ��ȯ (����: bytes)
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
	 desc : ���� (Emulation Mode) ���� �����ϴ��� ����
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsRunDemo()
	{
		return set_comn.run_emulate_mode ? TRUE : FALSE;
	}

	/*
	 desc : ���� ��ġ ���� Ȯ�� ����
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsCheckHotAir()	{	return set_uvdi15.check_hot_air ? TRUE : FALSE;		}
	BOOL IsCheckChiller()	{	return set_uvdi15.check_chiller ? TRUE : FALSE;		}
	BOOL IsCheckVacuum()	{	return set_uvdi15.check_vacuum ? TRUE : FALSE;		}
	BOOL IsCheckStartLed()	{	return set_uvdi15.check_start_led ? TRUE : FALSE;	}
	/*
	 desc : �뱤 ���� ���� Ȯ�� ����
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL IsCheckMaterial()	{	return set_uvdi15.check_material ? TRUE : FALSE;		}


}	STG_CIEA,	*LPG_CIEA;



#pragma pack (pop)	// 8 bytes

/* �ݵ�� 1 bytes ���� (Shared Memory ������ 1 bytes ���� �Ұ���) */
#pragma pack (push, 1)


#pragma pack (pop)	// 1 bytes