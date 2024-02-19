#pragma once

#include "Base.h"
#include <vector>

class CACamCali : public CBase
{
/* 생성자 & 파괴자 */
public:

	CACamCali(PTCHAR work_dir, LPG_VDSM shmem);
	virtual ~CACamCali();

/* 가상 함수 */
protected:

public:


/* 구조체 변수 */
protected:


#pragma pack (push, 8)
	typedef struct __st_align_camera_calibration_position__
	{
		INT32			error_x;		/* Axis X 오차 (단위: 0.1 um or 100 nm) */
		INT32			error_y;		/* AXis Y 오차 (단위: 0.1 um or 100 nm) */
		INT32			a_cam_x;		/* Align Camera Motion X 위치 (단위: 0.1 um or 100 nm) */
		INT32			stage_y;		/* Stage Motion Y 위치 (단위: 0.1 um or 100 nm) */

	}	STM_ACCP,	*LPM_ACCP;
	typedef struct __st_align_camera_calibration_information__
	{
		INT32			acam_x;			/* Align Camera X : Motion Position (단위: 0.1um or 100nm) */
		INT32			stage_x;		/* Stage X : Motion Position (단위: 0.1um or 100nm) */
		INT32			stage_y;		/* Stage Y : Motion Position (단위: 0.1um or 100nm) */
		UINT32			row_cnt;		/* 총 측정된 행의 개수 */
		UINT32			col_cnt;		/* 총 측정된 열의 개수 */
		UINT32			row_gap;		/* 행과 행 사이의 측정 간격 (단위: 0.1um or 100nm) */
		UINT32			col_gap;		/* 열과 열 사이의 측정 간격 (단위: 0.1um or 100nm) */

		LPM_ACCP		pos_xy;

	}	STM_ACCI,	*LPM_ACCI;


	typedef struct __st_2d_compensation_data_
	{
		double			dPosX;		/* Align Camera Motion X 위치 (단위: mm) */
		double			dPosY;		/* Stage Motion Y 위치 (단위: mm) */
		double			dErrorX;	/* Axis X 오차 (단위: mm) */
		double			dErrorY;	/* AXis Y 오차 (단위: mm) */

	}	STM_TDCD, * LPM_TDCD;
	typedef struct __st_2d_compensation_information__
	{
		UINT32					u32RowCnt;		/* 총 측정된 행의 개수 */
		UINT32					u32ColCnt;		/* 총 측정된 열의 개수 */

		std::vector<STM_TDCD>	stVctIndex;		/* 동적 할당용 */

		void Clear()
		{
			u32RowCnt = 0;
			u32ColCnt = 0;

			stVctIndex.clear();
			stVctIndex.shrink_to_fit();
		}

	}	STM_TDCI, * LPM_TDCI;
#pragma pack (pop)	/* 8 bytes order */

/* 로컬 변수 */
protected:

	UINT8				m_u8MarkCount;	/* 총 등록된 Mark Position 개수 (Align Camera 1/2 총합 */
	TCHAR				m_tzErrMsg[LOG_MESG_SIZE];	/* 가장 최근에 발생된 에러 메시지 출력 */

	/* Align Camera Calibration Information & Position */
	LPM_ACCI			m_pstCali;
	STM_TDCI			m_stCaliEx;	// 20231121 mhbaek Add Stage 검증용
	CString				m_strFilePath;
	/* Vision Shared Memory (현재 마크의 위치에 대한 Calibration XY 오차 값 반영을 위해) */
	LPG_VDSM			m_pstShMem;


/* 로컬 함수 */
protected:

	VOID				RemoveAlignCaliData();

	BOOL				IsCalibrationData(UINT16 cali_thick);


/* 공용 함수 */
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
