#pragma once

#include "Base.h"
#include <vector>

class CACamCali : public CBase
{
/* ������ & �ı��� */
public:

	CACamCali(PTCHAR work_dir, LPG_VDSM shmem);
	virtual ~CACamCali();

/* ���� �Լ� */
protected:

public:


/* ����ü ���� */
protected:


#pragma pack (push, 8)
	typedef struct __st_align_camera_calibration_position__
	{
		INT32			error_x;		/* Axis X ���� (����: 0.1 um or 100 nm) */
		INT32			error_y;		/* AXis Y ���� (����: 0.1 um or 100 nm) */
		INT32			a_cam_x;		/* Align Camera Motion X ��ġ (����: 0.1 um or 100 nm) */
		INT32			stage_y;		/* Stage Motion Y ��ġ (����: 0.1 um or 100 nm) */

	}	STM_ACCP,	*LPM_ACCP;
	typedef struct __st_align_camera_calibration_information__
	{
		INT32			acam_x;			/* Align Camera X : Motion Position (����: 0.1um or 100nm) */
		INT32			stage_x;		/* Stage X : Motion Position (����: 0.1um or 100nm) */
		INT32			stage_y;		/* Stage Y : Motion Position (����: 0.1um or 100nm) */
		UINT32			row_cnt;		/* �� ������ ���� ���� */
		UINT32			col_cnt;		/* �� ������ ���� ���� */
		UINT32			row_gap;		/* ��� �� ������ ���� ���� (����: 0.1um or 100nm) */
		UINT32			col_gap;		/* ���� �� ������ ���� ���� (����: 0.1um or 100nm) */

		LPM_ACCP		pos_xy;

	}	STM_ACCI,	*LPM_ACCI;


	typedef struct __st_2d_compensation_data_
	{
		double			dPosX;		/* Align Camera Motion X ��ġ (����: mm) */
		double			dPosY;		/* Stage Motion Y ��ġ (����: mm) */
		double			dErrorX;	/* Axis X ���� (����: mm) */
		double			dErrorY;	/* AXis Y ���� (����: mm) */

	}	STM_TDCD, * LPM_TDCD;
	typedef struct __st_2d_compensation_information__
	{
		UINT32					u32RowCnt;		/* �� ������ ���� ���� */
		UINT32					u32ColCnt;		/* �� ������ ���� ���� */

		std::vector<STM_TDCD>	stVctIndex;		/* ���� �Ҵ�� */

		void Clear()
		{
			u32RowCnt = 0;
			u32ColCnt = 0;

			stVctIndex.clear();
			stVctIndex.shrink_to_fit();
		}

	}	STM_TDCI, * LPM_TDCI;
#pragma pack (pop)	/* 8 bytes order */

/* ���� ���� */
protected:

	UINT8				m_u8MarkCount;	/* �� ��ϵ� Mark Position ���� (Align Camera 1/2 ���� */
	TCHAR				m_tzErrMsg[LOG_MESG_SIZE];	/* ���� �ֱٿ� �߻��� ���� �޽��� ��� */

	/* Align Camera Calibration Information & Position */
	LPM_ACCI			m_pstCali;
	STM_TDCI			m_stCaliEx;	// 20231121 mhbaek Add Stage ������
	CString				m_strFilePath;
	/* Vision Shared Memory (���� ��ũ�� ��ġ�� ���� Calibration XY ���� �� �ݿ��� ����) */
	LPG_VDSM			m_pstShMem;


/* ���� �Լ� */
protected:

	VOID				RemoveAlignCaliData();

	BOOL				IsCalibrationData(UINT16 cali_thick);


/* ���� �Լ� */
public:

	BOOL				LoadFile(UINT16 cali_thick);
	BOOL				IsCaliExistData()					{	return m_pstCali ? TRUE : FALSE;	}
	UINT8				GetCaliPos(UINT8 cam_id, INT32 acam_x, INT32 stage_y,
								   INT32 &error_x, INT32 &error_y);

	INT32				GetPeriodCols(UINT8 cam_id)			{	return m_pstCali[cam_id-1].col_cnt;	}
	INT32				GetPeriodRows(UINT8 cam_id)			{	return m_pstCali[cam_id-1].row_cnt;	}

	VOID				ResetAllCaliData();

	VOID				SetAlignACamVertGX(UINT8 cam_id, UINT8 mark_no, INT32 offset);
	VOID				SetAlignACamVertLX(UINT8 cam_id, UINT8 mark_no, INT32 offset);
	VOID				AddMarkPos(UINT8 cam_id, ENG_AMTF mark, UINT8 axis, UINT8 idx, DOUBLE pos);
	UINT8				SetTrigPosCaliApply();

	PTCHAR				GetLastErrMsg()	{	return m_tzErrMsg;	}

	BOOL				LoadFileEx(CString strFilePath);
	CString				GetCaliFilePath() { return m_strFilePath; }
	UINT8				GetCaliPosEx(double dPosX, double dPosY, double& dErrX, double& dErrY);

};
