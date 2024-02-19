#pragma once

#include <numeric>

class CWork
{
/* Constructor / Destructor */
public:

	CWork(LPG_SCIM search);
	virtual ~CWork();

/* Virtual Function */
protected:
	virtual VOID		StepWork() = 0;
	virtual VOID		UpdateCalc() = 0;

public:
	virtual VOID		InitWork() = 0;


/* Enumberate */
protected:


/* Local Parameter */
protected:

	TCHAR				m_tzStepName[256];

	UINT8				m_u8StepIt;			/* The current work step */
	UINT8				m_u8StepTotal;		/* Total work step */
	UINT8				m_u8RetryComm;		/* Number of commands sent */

	UINT16				m_u16MeasCount;		/* The current number of measurement */

	BOOL				m_bUpdateCalc;

	UINT64				m_u64WorkTime;		/* The last work time */
	UINT64				m_u6LimitTime;		/* Maximum waiting time */
	UINT64				m_u64CommTime;		/* Communication Time */
	UINT64				m_u64WaitTime;		/* It's time to wait for the movement */

	ENG_WJSI			m_enWorkState;		/* The work state id (succ, wait, fail) */

	LPG_SCIM			m_pstSearch;		/* Search Information */

	CMySection			m_csSyncJob;		/* Synchronous Object */


/* Local Function */
protected:

	VOID				Init();

	VOID				StepNext();
	VOID				SetStepName(PTCHAR name);

	VOID				SetWaitTimeout(UINT64 wait);
	BOOL				IsWaitTimeout();
	BOOL				IsWorkTimeout(UINT64 delay_time);

	BOOL				MotionMoveXY(DOUBLE pos_x, DOUBLE pos_y);
	BOOL				IsMotionMovedXY();


/* Public Function */
public:

	VOID				DoWork();	/* It is called periodically */
	VOID				StopWork();

	BOOL				IsBusy()		{	return (ENG_WJSI::en_wait == m_enWorkState || ENG_WJSI::en_next == m_enWorkState);	}
	BOOL				IsEnded()		{	return (m_enWorkState != ENG_WJSI::en_next && m_enWorkState != ENG_WJSI::en_wait);	}
	BOOL				IsCompleted()	{	return m_enWorkState == ENG_WJSI::en_comp;	};
	BOOL				IsFailed()		{	return m_enWorkState == ENG_WJSI::en_fail;	};
	BOOL				IsUpdatedCalc();

	PTCHAR				GetStepName()	{	return m_tzStepName;	}

	UINT16				GetMeasCount()	{	return m_u16MeasCount;	}
};
