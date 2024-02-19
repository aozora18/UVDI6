#pragma once

#include "Work.h"

class CWorkFocus : public CWork
{
/* Constructor / Destructor */
public:

	CWorkFocus(LPG_MICL recipe, UINT8 run_mode, DOUBLE max_min);
	virtual ~CWorkFocus();

/* Virutal Function */
protected:

	virtual VOID		StepWork();

public:

	virtual VOID		InitWork();


/* Local Parameter */
protected:

	DOUBLE				m_dbStartTopZ;
	DOUBLE				m_dbPosSetZ;	/* unit : mm */


/* Local Function */
protected:

	ENG_WJSI			IsWorkCompleted(UINT8 step_go);

	ENG_WJSI			SetStageMoveXY();
	ENG_WJSI			IsStageMovedXY();

	ENG_WJSI			SetPHMoveZ();
	ENG_WJSI			IsPHMovedZ();


/* Public Function */
public:

	DOUBLE				GetSetPos()			{	return m_dbPosSetZ;	}
};
