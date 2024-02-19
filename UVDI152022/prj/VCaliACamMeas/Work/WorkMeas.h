
#pragma once

#include "Work.h"

class CWorkMeas : public CWork
{
/* 생성자 & 파괴자 */
public:

	CWorkMeas();
	virtual ~CWorkMeas();

/* 가상함수 재정의 */
protected:
public:


/* 로컬 변수 */
protected:

	DOUBLE				m_dbDiffX;	/* The difference value of Stage X or Align Camera X ? (unit: mm) */
	DOUBLE				m_dbDiffY;	/* The difference value of Stage Y ? (unit: mm) */

	LPG_ACCS			m_pstMeas;	/* Measurement Information */


/* 로컬 함수 */
protected:

	VOID				SetWorkNext();

	ENG_JWNS			RunACamCaliData();
	ENG_JWNS			IsACamCaliData();

	ENG_JWNS			SetMovingCols();
	ENG_JWNS			IsMovedCols();
	ENG_JWNS			IsCompletedCols();

	ENG_JWNS			SetMovingRows();
	ENG_JWNS			IsMovedRows();
	ENG_JWNS			IsCompletedRows();

	ENG_JWNS			GetGrabMarkData();
	ENG_JWNS			IsGrabbedMarkData();

	ENG_JWNS			SetMotionMoving();
	ENG_JWNS			IsMotionMoved();

	BOOL				SaveBaseData();



/* 공용 함수 */
public:

	BOOL				InitWork(UINT8 cam_id, UINT16 thick, LPG_ACCS meas);
	VOID				DoWork();

	LPG_ACGR			GetGrabbedResult()	{	return m_pstGrabbedResult;	}
};
