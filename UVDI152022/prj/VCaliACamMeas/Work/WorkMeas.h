
#pragma once

#include "Work.h"

class CWorkMeas : public CWork
{
/* ������ & �ı��� */
public:

	CWorkMeas();
	virtual ~CWorkMeas();

/* �����Լ� ������ */
protected:
public:


/* ���� ���� */
protected:

	DOUBLE				m_dbDiffX;	/* The difference value of Stage X or Align Camera X ? (unit: mm) */
	DOUBLE				m_dbDiffY;	/* The difference value of Stage Y ? (unit: mm) */

	LPG_ACCS			m_pstMeas;	/* Measurement Information */


/* ���� �Լ� */
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



/* ���� �Լ� */
public:

	BOOL				InitWork(UINT8 cam_id, UINT16 thick, LPG_ACCS meas);
	VOID				DoWork();

	LPG_ACGR			GetGrabbedResult()	{	return m_pstGrabbedResult;	}
};
