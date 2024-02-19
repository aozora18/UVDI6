#pragma once

#include "Work.h"

class CWorkMeas : public CWork
{
/* Constructor / Destructor */
public:

	CWorkMeas(UINT8 led_no, PUINT16 index, LPG_MICL recipe, UINT8 run_mode, DOUBLE max_min);
	virtual ~CWorkMeas();

/* Virutal Function */
protected:

	virtual VOID		StepWork();

public:

	virtual VOID		InitWork();


/* Local Parameter */
protected:

	UINT16				m_u16MeasCount;	/* The number of LED Power (Amplitude) measured (collected) at each photohead */

	UINT16				m_u16BeginIndex;
	UINT16				m_u16EndIndex;
	UINT16				m_u16IntvlIndex;

	DOUBLE				m_dbMaxPower;	/* unit : W */


/* Local Function */
protected:

	ENG_WJSI			SetStageMoveXY();
	ENG_WJSI			IsStageMoved();

	ENG_WJSI			SetPHMoveZ();
	ENG_WJSI			IsPHMovedZ();

	ENG_WJSI			IsWorkCompleted(UINT8 step_go);

	ENG_WJSI			UpdateAmpIndex();


/* Public Function */
public:

	UINT16				GetMeasCount()		{	return m_u16MeasCount;	}

};
