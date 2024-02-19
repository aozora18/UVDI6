
/*
 desc : The work base object
*/

#include "pch.h"
#include "../MainApp.h"
#include "Work.h"


#ifdef	_DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : Constructor
 parm : search	- [in]  Search Information
 retn : None
*/
CWork::CWork(LPG_SCIM search)
{
	m_pstSearch	= search;
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CWork::~CWork()
{
}

/*
 desc : Initialize the member paramters
 parm : None
 retn : None
*/
VOID CWork::Init()
{
	/* Initialize the local parameters */
	m_u64WorkTime	= GetTickCount64();
	m_enWorkState	= ENG_WJSI::en_next;
	m_u16MeasCount	= 0x0000;
	m_bUpdateCalc	= FALSE;
	m_u8StepIt		= 0x01;
	/* Reset the step name */
	wmemset(m_tzStepName, 0x00, 256);
}

/*
 desc : It is called periodically
 parm : None
 retn : None
*/
VOID CWork::DoWork()
{
	/* Enter sync. */
	if (m_csSyncJob.Enter())
	{
		if (m_enWorkState == ENG_WJSI::en_next ||
			m_enWorkState == ENG_WJSI::en_wait)
		{
			StepWork();
			StepNext();
		}
		else if (m_enWorkState == ENG_WJSI::en_stop ||
				 m_enWorkState == ENG_WJSI::en_comp ||
				 m_enWorkState == ENG_WJSI::en_fail ||
				 m_enWorkState == ENG_WJSI::en_tout)
		{
			/* Reset the work state */
			m_enWorkState	= ENG_WJSI::en_init;
			m_u8StepIt		= 0x00;
		}

		/* Leave sync. */
		m_csSyncJob.Leave();
	}
}

/*
 desc : Decide whether to proceeded with the next step
 parm : None
 retn : None
*/
VOID CWork::StepNext()
{
	if (m_enWorkState == ENG_WJSI::en_wait)
	{
		/* Check whether there is no response for a set period of time */
		if ((GetTickCount64() - m_u64WorkTime) > m_u6LimitTime)
		{
			m_u8StepIt	= 0x00;
			m_enWorkState	= ENG_WJSI::en_tout;
			TCHAR tzMesg[256]	= {NULL};
			if (wcslen(m_tzStepName) > 0)
			{
				swprintf_s(tzMesg, 256, L"No response for a long time (Step : %s)", m_tzStepName);
			}
			else
			{
				wcscpy_s(tzMesg, 256, L"No response for a long time (Step : Unknown)");
			}
			LOG_ERROR(ENG_EDIC::en_pled_illum, tzMesg);
		}
	}
	/* Check whether the previous work (step) have been completed */
	else if (m_enWorkState == ENG_WJSI::en_next)
	{
		UINT64 u64Tick	= GetTickCount64();

		/* Check whether all works are complete */
		if (m_u8StepTotal == m_u8StepIt)
		{
			m_enWorkState	= ENG_WJSI::en_comp;
			m_u8StepIt		= 0x00;	/* Not used (This code should not be activated) */
		}
		/* Increment the step for then next work */
		else
		{
			m_u8StepIt++;
			/* Set the most recent work time */
			m_u64WorkTime	= u64Tick;
		}
		/* Update the wait_time */
		m_u6LimitTime	= u64Tick;
	}
}

/*
 desc : Set the forcibly stop the job
 parm : None
 retn : None
*/
VOID CWork::StopWork()
{
	/* Enter Sync. */
	if (m_csSyncJob.Enter())
	{
		m_enWorkState	= ENG_WJSI::en_stop;
		m_u8StepIt		= 0x00;

		/* Leave Sync. */
		m_csSyncJob.Leave();
	}
}

/*
 desc : Set the step name
 parm : name	- [in]  Step name
 retn : None
*/
VOID CWork::SetStepName(PTCHAR name)
{
	if (name)
	{
		wmemset(m_tzStepName, 0x00, 128);
		wcscpy_s(m_tzStepName, 128, name);
	}
}

/*
 desc : Check if you have waited for the set waiting time
 parm : None
 retn : None
*/
VOID CWork::SetWaitTimeout(UINT64 wait)
{
	if (uvEng_GetConfig()->IsRunDemo())	m_u64WaitTime	= 1;
	else 								m_u64WaitTime = wait;
}
BOOL CWork::IsWaitTimeout()
{
	return (m_u64WorkTime + m_u64WaitTime) < GetTickCount64();
}
BOOL CWork::IsWorkTimeout(UINT64 delay_time)
{
	return (m_u64WorkTime + delay_time) < GetTickCount64();
}

/*
 desc : Check if the average value has been updated
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsUpdatedCalc()
{
	BOOL bUpdateCalc	= m_bUpdateCalc;
	if (m_bUpdateCalc)	m_bUpdateCalc	= FALSE;

	return bUpdateCalc;
}

/* --------------------------------------------------------------------------------------------- */
/*                                           STEP Part                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Move Motion XY
 parm : pos_x,y	- [in]  move position (unit : mm)
 retn : TRUE or FALSE
*/
BOOL CWork::MotionMoveXY(DOUBLE pos_x, DOUBLE pos_y)
{
	TCHAR tzMesg[128]	= {NULL};
	DOUBLE dbVeloX		= 25.0f/*uvEng_GetConfig()->mc2_svc.max_velo[UINT8(ENG_MMDI::en_stage_x)]*/;
	DOUBLE dbVeloY		= 25.0f/*uvEng_GetConfig()->mc2_svc.max_velo[UINT8(ENG_MMDI::en_stage_y)]*/;
	ENG_MMDI enDrvX		= ENG_MMDI::en_stage_x;

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())	return TRUE;

	/* Get the toggle state of the current motor */
	if (m_pstSearch->target_x == 0x00)	enDrvX	= ENG_MMDI::en_stage_x;
	else
	{
		if (m_pstSearch->acam_id == 0x01)	enDrvX	= ENG_MMDI::en_align_cam1;
		else								enDrvX	= ENG_MMDI::en_align_cam2;
	}
	uvCmn_MC2_GetDrvDoneToggled(enDrvX);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);

	/* Move to the first position, which is the measurement position */
	if (!uvEng_MC2_SendDevAbsMove(enDrvX, pos_x, dbVeloX))
	{
		swprintf_s(tzMesg, 128, L"Failed to move the motion X (%.4f mm) axis", pos_x);
		LOG_ERROR(ENG_EDIC::en_pled_illum, tzMesg);
		return FALSE;
	}
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, pos_y, dbVeloY))
	{
		swprintf_s(tzMesg, 128, L"Failed to move the motion Y (%.4f mm) axis", pos_y);
		LOG_ERROR(ENG_EDIC::en_pled_illum, tzMesg);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : Check whether the motion has been moved
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsMotionMovedXY()
{
	ENG_MMDI enDrvX		= ENG_MMDI::en_stage_x;

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())	return TRUE;

	/* Get the motion drive for Axis X */
	if (m_pstSearch->target_x == 0x00)	enDrvX	= ENG_MMDI::en_stage_x;
	else
	{
		if (m_pstSearch->acam_id == 0x01)	enDrvX	= ENG_MMDI::en_align_cam1;
		else								enDrvX	= ENG_MMDI::en_align_cam2;
	}

	/* Check the toggle state of the motor drive */
	if (!uvCmn_MC2_IsDrvDoneToggled(enDrvX))				return FALSE;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))	return FALSE;

	return TRUE;
}

