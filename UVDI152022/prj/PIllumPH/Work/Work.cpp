
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
 parm : recipe	- [in]  Measurement Information
		run_mode- [in]  0x00 : Low Speed (Safe Mode), 0x01 : High Speed (Incomplete Mode)
		max_min	- [in]  This means that the difference between Min and Max values
						in the collected illumination data should not be greater than this value.
						(unit: mW)
 retn : None
*/
CWork::CWork(LPG_MICL recipe, UINT8 run_mode, DOUBLE max_min)
{
	m_u8RunMode	= run_mode;
	m_dbMaxMin	= max_min/*unit: mW*/;
	if (recipe)
	{
		m_enWorkJobID	= ENG_WJOI::en_none;
		m_u6LimitTime	= 30000;	/* Set the default waiting time to 30 seconds */
		m_u8PhNo		= recipe->ph_no;
		if (recipe)	memcpy(&m_stRecipe, recipe, sizeof(STG_MICL));
	}
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CWork::~CWork()
{
	m_vecPower.clear();
}

/*
 desc : Initialize the member paramters
 parm : None
 retn : None
*/
VOID CWork::Init()
{
	INT16 i16Freq	= (INT16)uvEng_Luria_GetLedNoToFreq(m_u8LedNo);
	/* Initialize the local parameters */
	m_u64WorkTime	= GetTickCount64();
	m_enWorkState	= ENG_WJSI::en_next;
	m_u8StepIt		= 0x01;
	m_u16PassGet	= 0x0000;	/* reset the number of phothead led power (amplitude) */
	m_u16MoveCnt	= 0x0000;
	m_bUpdateCalc	= FALSE;
	m_bUpdatePower	= FALSE;

	/* Reset the step name */
	wmemset(m_tzStepName, 0x00, 256);
	/* Temporarily save the initial start location */
	m_stStart.x		= m_stRecipe.GetPosXY(0x10);
	m_stStart.y		= m_stRecipe.GetPosXY(0x11);

	/* Start measuring the frequency */
	uvEng_PM100D_SetQueryWaveLength(i16Freq);
	uvEng_PM100D_RunMeasure(TRUE);
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
			/* Reset the illuminance of led */
			SetHWInited();
			uvEng_PM100D_RunMeasure(FALSE);
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
#if 0
	/* Check whether an error occurred in the previous work (step) */
	/* or Check whether all works are complete */
	if (m_enWorkState == ENG_WJSI::en_fail ||
		m_enWorkState == ENG_WJSI::en_stop ||
		m_enWorkState == ENG_WJSI::en_comp)
	{
		return;
	}
	/* Check whether the previous work (step) have been not completed */
	else if (m_enWorkState == ENG_WJSI::en_wait)
#else
	if (m_enWorkState == ENG_WJSI::en_wait)
#endif
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
			SetHWInited();	/* Unconditionally turn off the LED power */
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
		/* Reset the illuminance of led */
		SetHWInited();
		uvEng_PM100D_RunMeasure(FALSE);

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
 desc : Reset the communication parameter
 parm : None
 retn : None
*/
VOID CWork::ResetSendCommand()
{
	/* Set the current time (CPU Tick-count) */
	m_u64CommTime	= GetTickCount64();
	m_u8RetryComm	= 0x00;
}

/*
 desc : Check if the command needs to be resent
 parm : retry	- [int]  number of re-send
 retn : TRUE or FALSE
*/
BOOL CWork::IsSendOver(UINT8 retry)
{
	if (m_u8RetryComm > retry)
	{
		TCHAR tzMesg[128]	= {NULL};
		swprintf_s(tzMesg, 128, L"There is no response to sending (retry=%u)", retry);
		LOG_ERROR(ENG_EDIC::en_pled_illum,  tzMesg);
		return TRUE;
	}
	return FALSE;
}

/*
 desc : Check if the command needs to be resent
 parm : wait	- [int]  waiting time (Check if you have waited for this time value)
 retn : TRUE or FALSE
*/
BOOL CWork::IsSendCommand(UINT64 wait)
{
	if (GetTickCount64() > m_u64CommTime + wait)
	{
		m_u64CommTime	= GetTickCount64();
		m_u8RetryComm++;
		return TRUE;
	}
	return FALSE;
}

/*
 desc : Calculates the collected data
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::UpdateCalc()
{
	/* Sort in ascending order */
	sort(m_vecPower.begin(), m_vecPower.end());
	/* Remove elements that do not meet the conditions */
	if ((m_u16DropMax+m_u16DropMin) < m_vecPower.size())
	{
		/* Remove the smallest values */
		if (m_u16DropMin > 0)
		{
			m_vecPower.erase(m_vecPower.begin(), m_vecPower.begin()+m_u16DropMin);
		}
		/* Remove the largest values */
		if (m_u16DropMax > 0)
		{
			m_vecPower.erase(m_vecPower.end()-m_u16DropMax, m_vecPower.end());
		}

		/* Calculate the average and Min.Max */
		DOUBLE dbSum = accumulate(m_vecPower.begin(), m_vecPower.end(), double(0.0f));
		m_dbPwrAvg	= dbSum / m_vecPower.size();
		m_dbPwrMin	= m_vecPower.front();
		m_dbPwrMax	= m_vecPower.back();
		/* Calculate the standard deviation */
		DOUBLE dbTotal	= 0.0f;
		vector<DOUBLE>::iterator itVec = m_vecPower.begin();
		for (; itVec != m_vecPower.end(); itVec++)
		{
			dbTotal	+= pow((*itVec - m_dbPwrAvg), 2);
		}
		m_dbPwrDev	= sqrt(dbTotal / DOUBLE(m_vecPower.size()));

		/* Remove all existing collected data (vector lists) */
		m_vecPower.clear();
		m_u16PassGet	= 0x0000;

		return TRUE;
	}

	TCHAR tzMesg[128]	= {NULL};
	swprintf_s(tzMesg, 128, L"The number of collected data is small (drop_min=%u, drop_max=%u, valid=%u)",
			   m_u16DropMin, m_u16DropMax, (UINT16)m_vecPower.size());
	LOG_ERROR(ENG_EDIC::en_pled_illum,  tzMesg);

	return FALSE;
}

/*
 desc : Reset the led power for all the photohead
 parm : None
 retn : None
*/
VOID CWork::ResetPHPowerAll()
{
#if 0
	/* Set all value zero... */
	uvEng_Luria_ReqSetLightIntensityAll(0xff);
	/* Get the currenty led powe index */
	uvEng_Luria_ReqGetLedPowerAll();
	/* Turn off all led (light) */
	uvEng_Luria_ReqSetLedLightOnOffAll(0x00);
#endif
	/* Init the HW for Luria */
	uvEng_Luria_ReqSetHWInit();
}

/*
 desc : Check if the LED power has been updated
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsUpdatePower()
{
	BOOL bUpdate	= m_bUpdatePower;
	if (m_bUpdatePower)	m_bUpdatePower	= FALSE;
	return bUpdate;
}

/*
 desc : Check if the average value has been updated
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsUpdatedCalc()
{
	BOOL bUpdate	= m_bUpdateCalc;
	if (m_bUpdateCalc)	m_bUpdateCalc	= FALSE;
	return bUpdate;
}

/*
 desc : Move stage XY
 parm : pos_x,y	- [in]  move position (unit : mm)
 retn : TRUE or FALSE
*/
BOOL CWork::StageMoveXY(DOUBLE pos_x, DOUBLE pos_y)
{
	TCHAR tzMesg[128]	= {NULL};
	DOUBLE dbVeloX		= 25.0f/*uvEng_GetConfig()->mc2_svc.max_velo[UINT8(ENG_MMDI::en_stage_x)]*/;
	DOUBLE dbVeloY		= 25.0f/*uvEng_GetConfig()->mc2_svc.max_velo[UINT8(ENG_MMDI::en_stage_y)]*/;

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())	return TRUE;

	/* Get the toggle state of the current motor */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);

	/* Move to the first position, which is the measurement position */
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, pos_x, dbVeloX))
	{
		swprintf_s(tzMesg, 128, L"Failed to move the stage X (%.4f mm) axis", pos_x);
		LOG_ERROR(ENG_EDIC::en_pled_illum, tzMesg);
		return FALSE;
	}
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, pos_y, dbVeloY))
	{
		swprintf_s(tzMesg, 128, L"Failed to move the stage Y (%.4f mm) axis", pos_y);
		LOG_ERROR(ENG_EDIC::en_pled_illum, tzMesg);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : Check whether the stage has been moved
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsStageMovedXY()
{
	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())	return TRUE;

	/* Check the toggle state of the motor drive */
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_x))	return FALSE;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))	return FALSE;

	return TRUE;
}

/*
 desc : Check whether the luria occurred error
 parm : None
 retn : next , wait or fail
*/
BOOL CWork::IsLuriaError()
{
	/* Set the step name */
	SetStepName(L"Is.Luria.Error.Check");

	/* Check if it is the emulated (demo) mode ... */
	if (!uvEng_GetConfig()->IsRunDemo())
	{
		/* Check if an erros has occurred */
		if (uvCmn_Luria_IsLastError())
		{
			TCHAR tzMesg[256]	= {NULL};
			swprintf_s(tzMesg, 256, L"There is currently an error in the Luria (err_cd=%d)",
					   uvCmn_Luria_GetLastErrorStatus());
			LOG_ERROR(ENG_EDIC::en_pled_illum, tzMesg);
			return FALSE;
		}
	}

	return TRUE;
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
 desc : Perform basic completion work
 parm : None
 retn : None
*/
VOID CWork::SetWorkCompleted()
{
	/* Set the step name */
	SetStepName(L"Is.Work.Completed");
	/* Unconditionally turn off the power. */
	ResetPHPowerAll();
}

/* --------------------------------------------------------------------------------------------- */
/*                                           STEP Part                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Move photohead Z
 parm : pos_z	- [in]  move position (unit : mm)
 retn : TRUE or FALSE
*/
ENG_WJSI CWork::SetPHMoveZ(DOUBLE pos_z)
{
	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_WJSI::en_next;

	/* Reset the communication parameter */
	ResetSendCommand();
	/* Move to the set position */
	if (!uvEng_Luria_ReqSetPositionLM(m_u8PhNo, pos_z))	return ENG_WJSI::en_fail;
	/* Get the set position */
	if (!uvEng_Luria_ReqGetPositionLM(m_u8PhNo))	return ENG_WJSI::en_fail;
	return ENG_WJSI::en_next;
}

/*
 desc : Check whether the photohead has been moved
 parm : pos_z	- [in]  move position (unit : mm)
 retn : en_next or en_wait
*/
ENG_WJSI CWork::IsPHMovedZ(DOUBLE pos_z)
{
	LPG_LDPF pstPF	= &uvEng_ShMem_GetLuria()->focus;
	DOUBLE dbDiff	= DOUBLE(pstPF->z_pos[m_u8PhNo-1]/1000000.0f /* nm -> mm */) - pos_z;
	BOOL bReached	= abs(dbDiff) < 0.0015f;	/* If it's less than 1.5 um ... */

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_WJSI::en_next;
	if (!bReached)
	{
		if (IsSendCommand(2000))
		{
			ResetSendCommand();
			if (!uvEng_Luria_ReqGetPositionLM(m_u8PhNo))	return ENG_WJSI::en_fail;	/* Get the set position */
		}
		return ENG_WJSI::en_wait;
	}

	return ENG_WJSI::en_next;
}

/*
 desc : Check all motor drives for errors or inited (Homing)
 parm : None
 retn : en_next or en_fail
*/
ENG_WJSI CWork::IsValidMotorDriveAll()
{
	UINT8 i	= 0x00, u8DrvCnt = uvEng_GetConfig()->mc2_svc.drive_count;

	/* Set the step name */
	SetStepName(L"Is.Valid.Motor.Drive.All.Check");

	/* The motor temporarily waits for stabilization to begin */
	if (!IsWorkTimeout(1000))	return ENG_WJSI::en_wait;
	/* Reset the all error */
	uvCmn_Logs_ResetErrorMesg();
	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_WJSI::en_next;

	/* Check whether the servo status of all motor drives is locked */
	if (!uvCmn_MC2_IsAllDevLocked())
	{
		LOG_WARN(ENG_EDIC::en_pled_illum, L"The servo status of all motor drives is not locked");
		return ENG_WJSI::en_wait;
	}
	/* Check whether the calibrated status of all motor drives is calibrated (Homed) */
	if (!uvCmn_MC2_IsDriveHomedAll())
	{
		LOG_WARN(ENG_EDIC::en_pled_illum, L"The calibrated status of all motor drives is not homing");
		return ENG_WJSI::en_wait;
	}
	/* Check whether an error has occurred in the any motor drive */
	if (uvCmn_MC2_IsDriveErrorAll())
	{
		LOG_ERROR(ENG_EDIC::en_pled_illum, L"The error status of all motor drives is not cleared");
		return ENG_WJSI::en_fail;
	}

	/* Reset the All error */
	uvCmn_Luria_ResetLastErrorStatus();
	uvCmn_Logs_ResetErrorMesg();

	return ENG_WJSI::en_next;
}

/*
 desc : Request the hw_inited
 parm : None
 retn : next or fail
*/
ENG_WJSI CWork::SetHWInited()
{
	/* Set the step name */
	if (ENG_WJSI::en_next == m_enWorkState)
	{
		SetStepName(L"Set.HW.Inited");
	}
	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_WJSI::en_next;
	/* Send the HWInit command */
	if (!uvEng_Luria_ReqSetHWInit())
	{
		LOG_WARN(ENG_EDIC::en_pled_illum, L"Failed to initialize the HW Init of Service");
		return ENG_WJSI::en_fail;
	}

	return ENG_WJSI::en_next;
}

/*
 desc : Whether HW initialization is normal
 parm : None
 retn : wait, fail or next
*/
ENG_WJSI CWork::IsHWInited()
{
	BOOL bIsRecved		= TRUE;
	UINT64 u64WaitTime	= 0;

	/* Set the step name */
	SetStepName(L"Is.HW.Inited");

	if (uvEng_GetConfig()->IsRunDemo())	u64WaitTime = 1;
	else
	{
		/* The waiting time varies depending on the number of measurement data */
		u64WaitTime	= (UINT64)ROUNDED(UINT64(m_u16PassGet + m_u16PassSet) * 2.0f, 0) * 1000;
		if (0x00 == m_u8RunMode)	u64WaitTime = 3000/*!!! Important !!!*/;
	}
	/* Wait for the HWInit is completed */
	if (!IsWorkTimeout(u64WaitTime))	return ENG_WJSI::en_wait;
	/* Verify that the HWInit Command is received */
	if (!uvEng_GetConfig()->IsRunDemo())
	{
		bIsRecved	= uvEng_ShMem_GetLuria()->IsRecvCmdHWInited();
		if (!IsLuriaError())	return ENG_WJSI::en_fail;
	}

	return bIsRecved ? ENG_WJSI::en_next : ENG_WJSI::en_wait;
}

/*
 desc : Load the internal image No.3 on the PH
 parm : None
 retn : en_fail or en_next
*/
ENG_WJSI CWork::SetLoadInternalImage()
{
	UINT8 i	= 0x00;

	/* Set the step name */
	SetStepName(L"Set.Load.Internal.Image");

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_WJSI::en_next;

	/* Reset the power of all PH and Led */
#if 0
	uvEng_Luria_ReqSetLightIntensity(0xff, ENG_LLPI::en_all);
#else
	for (; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		uvEng_Luria_ReqSetLightIntensityAll(i+1, 0x00);
	}
#endif
	/* Set the Image number in the Spot Mode */
	if (!uvEng_Luria_ReqSetLoadInternalTestImage(m_u8PhNo, 0x03))
	{
		LOG_ERROR(ENG_EDIC::en_pled_illum, L"Failed to load the internal image No.3 on the PH");
		return ENG_WJSI::en_fail;
	}
	
	return ENG_WJSI::en_next;
}
ENG_WJSI CWork::IsLoadedInternalImage()
{
	BOOL bIsRecved	= uvEng_ShMem_GetLuria()->IsRecvCmdLoadedInternalImage();

	/* Set the step name */
	SetStepName(L"Is.Loaded.Internal.Image");

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_WJSI::en_next;
	/* Set the Image number in the Spot Mode */
	if (!bIsRecved)	return ENG_WJSI::en_wait;
	
	return ENG_WJSI::en_next;
}

/*
 desc : Set the LED Power Index on the luria
 parm : None
 retn : en_fail or en_next
*/
ENG_WJSI CWork::SetLedPower()
{
	UINT8 i	= 0x00;

	/* Set the step name */
	SetStepName(L"Set.Led.Power");

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_WJSI::en_next;

	/* Reset the communication parameter */
	ResetSendCommand();
#if 0
	/* Initializes the previously set LED power value */
	for (; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		uvEng_Luria_ReqSetLightIntensityAll(i+1);
	}
#endif
	/* Set the LED Power in the Spot Mode */
	if (!uvEng_Luria_ReqSetLightIntensity(m_u8PhNo, ENG_LLPI(m_u8LedNo), m_u16AmpIndex))
	{
		LOG_ERROR(ENG_EDIC::en_pled_illum, L"Failed to set the led power index of PH");
		return ENG_WJSI::en_fail;
	}
#if 0
	/* It doesn't work normally */
	/* Request the power index that was set in the luria */
	if (!uvEng_Luria_ReqGetLedPower(m_u8PhNo, ENG_LLPI(m_u8LedNo)))
#else
	if (!uvEng_Luria_ReqGetLedPowerLedAll(ENG_LLPI(m_u8LedNo)))
#endif
	{
		LOG_ERROR(ENG_EDIC::en_pled_illum, L"Failed to get the led power index of PH");
		return ENG_WJSI::en_fail;
	}
#if 0
	TRACE(L"Ph [%u] Led [%u] Power Index = %u\n",m_u8PhNo, ENG_LLPI(m_u8LedNo), m_u16AmpIndex);
#endif
	return ENG_WJSI::en_next;
}
ENG_WJSI CWork::IsSetLedPower()
{
	ENG_WJSI enState= ENG_WJSI::en_wait;
	BOOL bIsRecved	= uvEng_ShMem_GetLuria()->IsRecvCmdSetLedPower();
	UINT16 u16AmpIdx= uvEng_ShMem_GetLuria()->directph.GetPhLedAmpIdx(m_u8PhNo, ENG_LLPI(m_u8LedNo));

	/* Set the step name */
	SetStepName(L"Is.Set.Led.Power");

	/* Check if a vale is set */
	if (bIsRecved && u16AmpIdx == m_u16AmpIndex || uvEng_GetConfig()->IsRunDemo())
	{
		return ENG_WJSI::en_next;
	}

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_WJSI::en_next;
	/* Check if you have waited until the given time */
	if (!IsSendCommand(1000))
	{
		/* Check whether you have waited more than 0.5 seconds */
		if (IsSendOver())	return ENG_WJSI::en_fail;
		return ENG_WJSI::en_wait;
	}
	/* Request the LED Power */
	ResetSendCommand();
#if 0
	/* It doesn't work normally */
	if (!uvEng_Luria_ReqGetLedPower(m_u8PhNo, ENG_LLPI(m_u8LedNo)))
#else
	if (!uvEng_Luria_ReqGetLedPowerLedAll(ENG_LLPI(m_u8LedNo)))
#endif
	{
		LOG_ERROR(ENG_EDIC::en_pled_illum, L"Failed to get the led power index of PH");
		return ENG_WJSI::en_fail;
	}

	return ENG_WJSI::en_wait;
}

/*
 desc : Reset the LED Power Index on the luria  (Power Index = 0)
 parm : None
 retn : en_fail or en_next
*/
ENG_WJSI CWork::ResetLedPower()
{
	/* Set the step name */
	SetStepName(L"Reset.Led.Power");

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_WJSI::en_next;

	/* Reset the communication parameter */
	ResetSendCommand();
	/* Set the LED Power in the Spot Mode and Turn off all led (light) */
	if (!uvEng_Luria_ReqSetLightIntensity(m_u8PhNo, ENG_LLPI(m_u8LedNo), 0x0000))
	{
		LOG_ERROR(ENG_EDIC::en_pled_illum, L"Failed to turn off the led power of PH");
		return ENG_WJSI::en_fail;
	}

	return ENG_WJSI::en_next;
}
ENG_WJSI CWork::IsResetLedPower()
{
	ENG_WJSI enState= ENG_WJSI::en_wait;
	UINT16 u16AmpIdx= uvEng_ShMem_GetLuria()->directph.GetPhLedAmpIdx(m_u8PhNo, ENG_LLPI(m_u8LedNo));

	/* Set the step name */
	SetStepName(L"Is.Reset.Led.Power");

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_WJSI::en_next;

	/* Check if a vale is set */
	if (u16AmpIdx == 0)	return ENG_WJSI::en_next;

	/* Check if you have waited until the given time */
	if (!IsSendCommand(1000))
	{
		/* Check whether you have waited more than 0.5 seconds */
		if (IsSendOver())	return ENG_WJSI::en_fail;
		return ENG_WJSI::en_wait;
	}
#if 0
	/* It doesn't work normally */
	/* Request the spot mode that was set in the luria */
	if (!uvEng_Luria_ReqGetLedPower(m_u8PhNo, ENG_LLPI(m_u8LedNo)))
#else
	if (!uvEng_Luria_ReqGetLedPowerLedAll(ENG_LLPI(m_u8LedNo)))
#endif
	{
		LOG_ERROR(ENG_EDIC::en_pled_illum, L"Failed to get the led power index of PH");
		return ENG_WJSI::en_fail;
	}

	return ENG_WJSI::en_wait;
}

/*
 desc : Reset the LED Power Index on the luria  (Power Index = 0)
 parm : ph_no	- [in]  Photohead Number (0xff:All, 1 ~ 8)
		led_no	- [in]	Led Number (0xff:All, 1 ~ 4)
 retn : en_fail or en_next
*/
ENG_WJSI CWork::ResetLedPower(UINT8 ph_no, UINT8 led_no)
{
	/* Set the step name */
	SetStepName(L"Reset.Led.Power");

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_WJSI::en_next;
	/* Reset the communication parameter */
	ResetSendCommand();
	/* Set the LED Power in the Spot Mode and Turn off all led (light) */
	if (!uvEng_Luria_ReqSetLightIntensity(ph_no, ENG_LLPI(led_no), 0x0000))
	{
		LOG_ERROR(ENG_EDIC::en_pled_illum, L"Failed to turn off the led power of PH");
		return ENG_WJSI::en_fail;
	}

	return ENG_WJSI::en_next;
}
ENG_WJSI CWork::IsResetLedPower(UINT8 ph_no, UINT8 led_no)
{
	ENG_WJSI enState= ENG_WJSI::en_wait;
	BOOL bIsRecved	= uvEng_ShMem_GetLuria()->IsRecvCmdSetLedPower();
	UINT16 u16AmpIdx= uvEng_ShMem_GetLuria()->directph.GetPhLedAmpIdx(ph_no, ENG_LLPI(led_no));

	/* Set the step name */
	SetStepName(L"Is.Reset.Led.Power");

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_WJSI::en_next;

	/* Check if a vale is set */
	if (bIsRecved && u16AmpIdx == 0)	return ENG_WJSI::en_next;

	/* Check if you have waited until the given time */
	if (!IsSendCommand(1000))
	{
		/* Check whether you have waited more than 0.5 seconds */
		if (IsSendOver())	return ENG_WJSI::en_fail;
		return ENG_WJSI::en_wait;
	}
#if 0
	/* It doesn't work normally */
	/* Request the spot mode that was set in the luria */
	if (!uvEng_Luria_ReqGetLedPower(m_u8PhNo, ENG_LLPI(m_u8LedNo)))
#else
	if (!uvEng_Luria_ReqGetLedPowerLedAll(ENG_LLPI(led_no)))
#endif
	{
		LOG_ERROR(ENG_EDIC::en_pled_illum, L"Failed to get the led power index of PH");
		return ENG_WJSI::en_fail;
	}

	return ENG_WJSI::en_wait;
}

/*
 desc : Wait for LED Power to stabilize
 parm: None
 retn : next or wait
*/
ENG_WJSI CWork::IsWaitLedStable()
{
	/* Set the step name */
	SetStepName(L"Is.Wait.Led.Stable");

	/* Wait about 2 seconds for it to stabilize */
	if (!IsWorkTimeout(2000))	ENG_WJSI::en_wait;
	/* Get the current power of led in the photohead */
	DOUBLE dbGarbage= uvEng_GetConfig()->luria_svc.illum_garbage_pwr;
	LPG_PGDV pstVal	= uvEng_PM100D_GetValue();
	/* Verify that the data collected from the current LED is a garbage value */
	if (!uvEng_GetConfig()->IsRunDemo() &&
		dbGarbage > pstVal->last_val * 1000.0f /*W -> mW*/)
	{
#if 0
		TCHAR tzMesg[128]	= {NULL};
		swprintf_s(tzMesg, 128, L"The LED power was not collected (power = %.3f mW)", dbGarbage*1000.0f);
		TRACE(L"%s\n", tzMesg);
		LOG_WARN(ENG_EDIC::en_pled_illum, tzMesg);
#endif
		/* Have a little wait time */
		Sleep(100);
		return ENG_WJSI::en_wait;
	}
	/* Reset the current (Last) power (Watt) */
	m_dbLastPower	= 0.0f;
	/* Initializes the buffer where the existing measured values are stored */
	uvEng_PM100D_ResetValue();

	return ENG_WJSI::en_next;
}

/*
 desc : Verify that the data was collected successfully.
 parm : retry	- [in]  The number of power measurement attempts
 retn : en_wait or en_next
*/
ENG_WJSI CWork::IsCollectedQueued()
{
	/* Set the step name */
	SetStepName(L"Is.Collected.Queued");

	/* Check whether the buffer is full */
	if (!uvEng_PM100D_IsQueFulled())	return ENG_WJSI::en_wait;

	return ENG_WJSI::en_next;
}

/*
 desc : Check if the led power measurement of photohead is complete
 parm : None
 retn : en_wait or en_next
*/
ENG_WJSI CWork::GetLedPower()
{
	/* Get the current power of led in the photohead */
	LPG_PGDV pstVal	= uvEng_PM100D_GetValue();

	/* Set the step name */
	SetStepName(L"Get.Led.Power (Average)");

	/* Verify that the maximum/minimum comparison value is set */
	if (m_dbMaxMin <= 0.0f)
	{
		TCHAR tzMesg[128]	= {NULL};
		swprintf_s(tzMesg, 128, L"The maximum/minimum comparison value is not set. (Max-Min = %.3f mW)", m_dbMaxMin*1000.0f);
		TRACE(L"%s\n", tzMesg);
		LOG_WARN(ENG_EDIC::en_pled_illum, tzMesg);
		return ENG_WJSI::en_fail;
	}
	/* If the difference between the maximum and minimum values of the currently collected data */
	/* is too large, let them be collected again. */
	if (!uvEng_GetConfig()->IsRunDemo() &&
		pstVal->max_min * 1000.0f > m_dbMaxMin)
	{
		Sleep(100);
		return ENG_WJSI::en_wait;
	}
	/* Increase the number of power measurements of the photohead */
	if (m_u16PassGet < m_u16PassSet)
	{
		m_u16PassGet++;
	}
	else
	{
#if 1
		if (pstVal->average == 0.0f)
		{
			TRACE(L"GetLedPower() => pstVal->average = power is zero\n");
		}
#endif
		m_vecPower.push_back(pstVal->average);
	}

	/* Save the last power (Watt) */
	m_dbLastPower	= pstVal->average;
	/* Update the led power */
	m_bUpdatePower	= TRUE;

	return ENG_WJSI::en_next;
}

/*
 desc : Initializes the focus motor of all the photoheads z-axis
 parm : None
 retn : fail or next
*/
ENG_WJSI CWork::SetAllPHZAxisInit()
{
	UINT8 i	= 0x00, u8PhCnt	= uvEng_GetConfig()->luria_svc.ph_count;

	/* Set the step name */
	SetStepName(L"Set.All.PH.Z.Axis.Init");
	/* Initializes the z-axis of all the photohead */
	uvEng_ShMem_GetLuria()->focus.ResetAllPhZAxisPos();
	/* Do homing */
	if (!uvEng_Luria_ReqSetMotorPositionInitAll(0x00))	return ENG_WJSI::en_fail;
	if (uvEng_GetConfig()->IsRunDemo())	m_u64WaitTime	= 1;
	else 								m_u64WaitTime	= 2000;

	return ENG_WJSI::en_next;
}

/*
 desc : Check whether the focus motors of all photohead z-axis are initialized
 parm : None
 retn : fail or next
*/
ENG_WJSI CWork::IsAllPHZAxisInit()
{
	UINT8 i	= 0x00, u8PhCnt	= uvEng_GetConfig()->luria_svc.ph_count;

	/* Set the step name */
	SetStepName(L"Is.All.PH.Z.Axis.Init");
	/* Check if it is operating in demo mode */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_WJSI::en_next;
	return uvEng_ShMem_GetLuria()->system.IsAllPHInited() ? ENG_WJSI::en_next : ENG_WJSI::en_wait;
}

/*
 desc : Compare the position of the photohead
 parm : pos	- [in]  position value to be reached (unit : mm)
 retn : fail or next
*/
ENG_WJSI CWork::IsPHZAxisReachedAll(DOUBLE pos)
{
	UINT8 i	= 0x00, u8PhCnt	= uvEng_GetConfig()->luria_svc.ph_count;
	BOOL bReached	= TRUE;
	DOUBLE dbPos	= 0.0f;
	/* Set the step name */
	SetStepName(L"Is.PH.Z.Axis.Reached.All");

	/* Check if it is operating in demo mode */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_WJSI::en_next;

	/* Compare with the current position of the motor */
	for (; i<u8PhCnt; i++)
	{
		dbPos	= uvEng_ShMem_GetLuria()->focus.z_pos[i]/1000000.0f;
		if (abs(pos-dbPos) > 0.0015 /* 1.5 um */)
		{
			bReached	= FALSE;
			break;
		}
	}
	if (bReached)	return ENG_WJSI::en_next;

	/* Check if you have waited until the given time */
	if (IsSendCommand(1000))
	{
		for (i=0x00; i<u8PhCnt; i++)
		{
			if (!uvEng_Luria_ReqGetPositionLM(i+0x01))	return ENG_WJSI::en_fail;
		}
	}

	return ENG_WJSI::en_wait;
}

/*
 desc : Wait until the current illumination value is close to zero level
 parm : None
 retn : wait or next
*/
ENG_WJSI CWork::IsIllumPowerZero()
{
	SetStepName(L"Is.Illum.Power.Zero");

	/* Get the current power */
	DOUBLE dbPower	= uvEng_PM100D_GetCurPower() * 1000.0f /* W -> mW */;
#if 1
	BOOL bIsFulled	= uvEng_PM100D_IsQueFulled();
	if (uvEng_GetConfig()->IsRunDemo() ||
		dbPower != 0.0f && bIsFulled &&
		dbPower < uvEng_GetConfig()->luria_svc.illum_garbage_pwr/* 0.001 mW */)
	{
		return ENG_WJSI::en_next;
	}
#else
	if (uvEng_GetConfig()->IsRunDemo() ||
		dbPower < uvEng_GetConfig()->luria_svc.illum_garbage_pwr/* 0.001 mW */)
	{
		return ENG_WJSI::en_next;
	}
#endif
	uvCmn_uSleep(300);
	m_u8StepIt--;

	return ENG_WJSI::en_wait;
}

/*
 desc : Check whether all measurement are complete
 parm : step_slow	- [in]  The value of the step number that you want to branch
		step_fast	- [in]  The value of the step number that you want to skip
 retn : wait or next
*/
ENG_WJSI CWork::IsMeasCompleted(UINT8 step_slow, UINT8 step_fast)
{
	/* Set the step name */
	SetStepName(L"Is.Meas.Completed");

	/* If all repeat measurements are done at the current location ... */
	if ((UINT8)m_vecPower.size() < m_u16ValidSet)
	{
		/* Wait a certain amount of time for sufficient data is collected in PM100D */
		if (!uvEng_GetConfig()->IsRunDemo())
		{
			if (!IsWorkTimeout(1000))	return ENG_WJSI::en_wait;
		}
		/* Go to the next work step */
		if (0x00 == m_u8RunMode)	m_u8StepIt	= step_slow;	/* Measure again after waiting for a certain period of time */
		else						m_u8StepIt	= step_fast;	/* Go straight to the GetLedPower() function */
	}
	/* if current LED index values are all measured */
	else
	{
		/* Remove elements that do not meet the conditions */
		m_bUpdateCalc = UpdateCalc();	/* You must set the flag to TRUE */
	}

	return ENG_WJSI::en_next;
}

/*
 desc : Set the Enable or Disable the EN_STOP
 parm : enable	- [in]  TRUE : Enable EN_STOP, FALSE : Disable EN_STOP
 retn : wait or next
*/
ENG_WJSI CWork::SetMC2EnStop(BOOL enable)
{
	TCHAR tzTitle[128] = { NULL }, tzEnable[2][32] = { L"Disable", L"Enable" };

	/* Set the step name */
	swprintf_s(tzTitle, 128, L"Set.MC2.EnStop (%s)", tzEnable[enable]);
	SetStepName(tzTitle);
	/* Check if it is operating in demo mode */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_WJSI::en_next;

	ENG_MMST enStop = enable ? ENG_MMST::en_stop_yes : ENG_MMST::en_stop_no;
	return uvEng_MC2_SetRefHeadEnStopEx(enStop) ? ENG_WJSI::en_next : ENG_WJSI::en_fail;
}

/*
 desc : Verify that the EN_STOP of MC2 is Enabled or Disabled
 parm : enable	- [in]  TRUE : Enable EN_STOP, FALSE : Disable EN_STOP
 retn : wait or next
*/
ENG_WJSI CWork::IsMC2EnStop(BOOL enable)
{
	TCHAR tzTitle[128] = { NULL }, tzEnable[2][32] = { L"Disabled", L"Enabled" };

	/* Set the step name */
	swprintf_s(tzTitle, 128, L"Is.MC2.EnStop (%s)", tzEnable[enable]);
	SetStepName(tzTitle);
	/* Check if it is operating in demo mode */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_WJSI::en_next;

	BOOL bEnStopped	= uvEng_ShMem_GetMC2()->ref_head.mc_en_stop == UINT8(enable);
	return (TRUE == bEnStopped) ? ENG_WJSI::en_next : ENG_WJSI::en_wait;
}
