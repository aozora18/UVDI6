#pragma once

#include <numeric>

class CWork
{
/* Constructor / Destructor */
public:

	CWork(LPG_MICL recipe, UINT8 run_mode, DOUBLE max_min);
	virtual ~CWork();

/* Virtual Function */
protected:
	virtual VOID		StepWork() = 0;

public:
	virtual VOID		InitWork() = 0;


/* Enumberate */
protected:


/* Local Parameter */
protected:

	UINT8				m_u8PhNo;				/* Photohead Number (0x01 ~ 0x08) */
	UINT8				m_u8LedNo;				/* Led Number (0x01 ~ 0x05) */
	UINT8				m_u8RunMode;			/* Whether to use high-speed measurement mode */

	UINT8				m_u8StepIt;				/* The current work step */
	UINT8				m_u8StepTotal;			/* Total work step */
	UINT8				m_u8RetryComm;			/* Number of commands sent */

	TCHAR				m_tzStepName[256];

	UINT16				m_u16PassSet, m_u16ValidSet;
	UINT16				m_u16DropMin, m_u16DropMax;
	UINT16				m_u16PassGet;			/* The number of LED Power (Amplitude) measured (collected) at each photohead */
	UINT16				m_u16MoveCnt;			/* Saves the total number of moves */
	UINT16				m_u16AmpIndex;			/* Power Index of led in the Photohead*/

	BOOL				m_bUpdateCalc;			/* Average, Standard Deviation, Max - Min */
	BOOL				m_bUpdatePower;			/* Refresh the led power */

	UINT64				m_u64WorkTime;			/* The last work time */
	UINT64				m_u6LimitTime;			/* Maximum waiting time */
	UINT64				m_u64CommTime;			/* Communication Time */
	UINT64				m_u64WaitTime;			/* It's time to wait for the movement */

	DOUBLE				m_dbMaxMin;				/* The value stored in the buffer starts collection if the difference between the maximum and minimum values is less than this value. (unit: mW) */
	DOUBLE				m_dbPwrAvg, m_dbPwrDev;	/* Average, Standard Deviation (unit : W) */
	DOUBLE				m_dbPwrMin, m_dbPwrMax;	/* Max - Min (unit : W) */
	DOUBLE				m_dbLastPower;			/* Get the current power (Watt) */

	STG_DBXY			m_stStart;				/* Starting Position (unit : W) */
	STG_DBXY			m_stPosSet;				/* The current set position */

	vector <DOUBLE>		m_vecPower;				/* List where measurement values will be stored  */

	ENG_WJOI			m_enWorkJobID;			/* The work job id */
	ENG_WJSI			m_enWorkState;			/* The work state id (succ, wait, fail) */

	STG_MICL			m_stRecipe;				/* The measurement information currently set */

	CMySection			m_csSyncJob;			/* Synchronous Object */


/* Local Function */
protected:

	VOID				Init();

	VOID				StepNext();
	VOID				SetStepName(PTCHAR name);
	VOID				ResetSendCommand();
	VOID				ResetPHPowerAll();


	BOOL				IsSendCommand(UINT64 wait=500/* msec */);
	BOOL				IsSendOver(UINT8 retry=0x03);
	BOOL				IsWorkTimeout(UINT64 delay_time);
	BOOL				IsLuriaError();
	BOOL				UpdateCalc();

	BOOL				StageMoveXY(DOUBLE pos_x, DOUBLE pos_y);
	BOOL				IsStageMovedXY();

	VOID				SetWaitTimeout(UINT64 wait);
	BOOL				IsWaitTimeout();

	VOID				SetWorkCompleted();


	ENG_WJSI			IsWaitLedStable();
	ENG_WJSI			IsCollectedQueued();

	ENG_WJSI			IsValidMotorDriveAll();
	ENG_WJSI			SetHWInited();
	ENG_WJSI			IsHWInited();
	ENG_WJSI			SetLoadInternalImage();
	ENG_WJSI			IsLoadedInternalImage();
	ENG_WJSI			GetLedPower();
	ENG_WJSI			SetAllPHZAxisInit();
	ENG_WJSI			IsAllPHZAxisInit();
	ENG_WJSI			IsPHZAxisReachedAll(DOUBLE pos=0.0f);

	ENG_WJSI			SetPHMoveZ(DOUBLE pos_z);
	ENG_WJSI			IsPHMovedZ(DOUBLE pos_z);

	ENG_WJSI			SetLedPower();
	ENG_WJSI			IsSetLedPower();
	ENG_WJSI			ResetLedPower();
	ENG_WJSI			IsResetLedPower();
	ENG_WJSI			ResetLedPower(UINT8 ph_no, UINT8 led_no);
	ENG_WJSI			IsResetLedPower(UINT8 ph_no, UINT8 led_no);

	ENG_WJSI			IsIllumPowerZero();
	ENG_WJSI			IsMeasCompleted(UINT8 step_slow, UINT8 step_fast);

	ENG_WJSI			SetMC2EnStop(BOOL enable);
	ENG_WJSI			IsMC2EnStop(BOOL enable);


/* Public Function */
public:

	VOID				DoWork();	/* It is called periodically */
	VOID				StopWork();

	BOOL				IsBusy()		{	return (ENG_WJSI::en_wait == m_enWorkState || ENG_WJSI::en_next == m_enWorkState);	}
	BOOL				IsEnded()		{	return (m_enWorkState != ENG_WJSI::en_next && m_enWorkState != ENG_WJSI::en_wait);	}
	BOOL				IsCompleted()	{	return m_enWorkState != ENG_WJSI::en_comp;	};
	BOOL				IsFailed()		{	return m_enWorkState != ENG_WJSI::en_fail;	};
	BOOL				IsUpdatedCalc();
	BOOL				IsUpdatePower();

	UINT16				GetIgnoreCount(){	return m_u16PassGet;			}
	UINT8				GetValidCount()	{	return (UINT8)m_vecPower.size();}
	UINT16				GetMoveTotal()	{	return m_u16MoveCnt;			}

	DOUBLE				GetMaxMinDiff()	{	return m_dbPwrMax - m_dbPwrMin;	}
	DOUBLE				GetStdDev()		{	return m_dbPwrDev;				}
	DOUBLE				GetAverage()	{	return m_dbPwrAvg;				}
	DOUBLE				GetLastPower()	{	return m_dbLastPower;			}	/* The last average value */

	PTCHAR				GetStepName()	{	return m_tzStepName;			}
	LPG_DBXY			GetSetPos()		{	return &m_stPosSet;				}
};
