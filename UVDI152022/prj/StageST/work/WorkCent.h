#pragma once

#include "Work.h"

class CWorkCent : public CWork
{
/* Constructor / Destructor */
public:

	CWorkCent(LPG_SCIM search);
	virtual ~CWorkCent();

/* Virutal Function */
protected:

	virtual VOID		StepWork();
	virtual VOID		UpdateCalc();

public:

	virtual VOID		InitWork();


/* Local Parameter */
protected:

	UINT64				m_u64GrabbedTick;			/* Saves the time when the image is capture */

	DOUBLE				m_dbMotionX, m_dbMotionY;	/* Get the current position of Motion XY (unit: mm) */
	LPG_ACGR			m_pstResult;


/* Local Function */
protected:

	ENG_WJSI			MotionMoveXY();
	ENG_WJSI			IsMotionMoved();

	ENG_WJSI			GetGrabMark();
	ENG_WJSI			IsGrabbedMark();

	ENG_WJSI			IsMeasCompleted();
	ENG_WJSI			IsWorkCompleted();


/* Public Function */
public:

	DOUBLE				GetStageX()		{	return m_dbMotionX;	}
	DOUBLE				GetStageY()		{	return m_dbMotionY;	}
	LPG_ACGR			GetResult()		{	return m_pstResult;	}
};
