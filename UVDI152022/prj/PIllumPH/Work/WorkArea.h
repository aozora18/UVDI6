#pragma once

#include "Work.h"

class CWorkArea : public CWork
{
/* Constructor / Destructor */
public:

	CWorkArea(LPG_MICL recipe, UINT8 run_mode, DOUBLE max_min);
	virtual ~CWorkArea();

/* Virutal Function */
protected:

	virtual VOID		StepWork();

public:

	virtual VOID		InitWork();


/* Local Parameter */
protected:

	UINT16				m_u16MoveCol;		/* Temporarily saves the count information measured so far */
	UINT16				m_u16MoveRow;


/* Local Function */
protected:

	ENG_WJSI			IsWorkCompleted(UINT8 step_go);

	ENG_WJSI			SetStageMoveXY();
	ENG_WJSI			IsStageMovedXY();

	ENG_WJSI			SetPHMoveZ();
	ENG_WJSI			IsPHMovedZ();

/* Public Function */
public:

	UINT16				GetMoveY()		{	return m_u16MoveRow + 1;	}
	UINT16				GetMoveX()		{	return m_u16MoveCol + 1;	}
};
