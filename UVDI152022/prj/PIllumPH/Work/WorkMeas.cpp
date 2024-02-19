
/*
 desc : The work - Led Power
*/

#include "pch.h"
#include "../MainApp.h"
#include "WorkMeas.h"


#ifdef	_DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : Constructor
 parm : led_no	- [in]  Led Number (0x01 ~ 0x04)
		index	- [in]  Measurement Index Information
		recipe	- [in]  The structure stored the area measurement information
		run_mode- [in]  0x00 : Low Speed (Safe Mode), 0x01 : High Speed (Incomplete Mode)
		max_min	- [in]  This means that the difference between Min and Max values
						in the collected illumination data should not be greater than this value.
 retn : None
*/
CWorkMeas::CWorkMeas(UINT8 led_no, PUINT16 index, LPG_MICL recipe, UINT8 run_mode, DOUBLE max_min)
	:CWork(recipe, run_mode, max_min)
{
	m_enWorkJobID	= ENG_WJOI::en_meas;
	m_u8PhNo		= recipe->ph_no;
	m_u8LedNo		= led_no;
	/*m_u8LedNo		= recipe->led_no[0];*/
	m_u16DropMin	= recipe->drop_min_cnt[2];
	m_u16DropMax	= recipe->drop_max_cnt[2];
	m_u16PassSet	= recipe->pass_cnt[2];
	m_u16ValidSet	= recipe->valid_cnt[2];
	m_u16BeginIndex	= index[0];
	m_u16IntvlIndex	= index[2];
	m_u16EndIndex	= index[0] + index[1] * index[2] - m_u16IntvlIndex;
	m_u16AmpIndex	= 0x0000;
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CWorkMeas::~CWorkMeas()
{
}

/*
 desc : Initialize the work
 parm : None
 retn : None
*/
VOID CWorkMeas::InitWork()
{
	/* Must call it */
	CWork::Init();

	/* Initialize the local parameters */
	m_u8StepTotal	= 0x17;
	m_u16MeasCount	= 0x0000;	/* reset the number of phothead led power (amplitude) */
	m_dbMaxPower	= DBL_MIN;
}

/*
 desc : Do work step by step
 parm : None
 retn : None
*/
VOID CWorkMeas::StepWork()
{
	switch (m_u8StepIt)
	{
	/* Disable the EnStop */
	case 0x01 : m_enWorkState = SetMC2EnStop(FALSE);			break;
	case 0x02 : m_enWorkState = IsMC2EnStop(FALSE);				break;
	/* Initializes the focus motor on the z-axis of photohead */
	case 0x03 : m_enWorkState = SetAllPHZAxisInit();			break;
	case 0x04 : m_enWorkState = IsValidMotorDriveAll();			break;
	/* Enable the EnStop */
	case 0x05 : m_enWorkState = SetMC2EnStop(TRUE);				break;
	case 0x06 : m_enWorkState = IsMC2EnStop(TRUE);				break;
	/* Move to the position to be measured */
	case 0x07 : m_enWorkState = SetStageMoveXY();				break;
	case 0x08 : m_enWorkState = IsStageMoved();					break;
	/* Move to the position to be measured */
	case 0x09 : m_enWorkState = SetPHMoveZ();					break;
	case 0x0a : m_enWorkState = IsPHMovedZ();					break;
	/* Update the led power index of photohead */
	case 0x0b : m_enWorkState = UpdateAmpIndex();				break;
	/* It actually performs an action to turn off the power */
	case 0x0c : m_enWorkState = SetHWInited();					break;
	case 0x0d : m_enWorkState = IsHWInited();					break;
	/* Change to the mode thant can measure the illuminance in luria */
	case 0x0e : m_enWorkState = SetLoadInternalImage();			break;
	case 0x0f : m_enWorkState = IsLoadedInternalImage();		break;
	/* Wait until the current illumination value is close to zero level */
	case 0x10 : m_enWorkState = IsIllumPowerZero();				break;
	/* Enter the LED Index on the luria (Light On) */
	case 0x11 : m_enWorkState = SetLedPower();					break;
	case 0x12 : m_enWorkState = IsSetLedPower();				break;
	case 0x13 : m_enWorkState = IsWaitLedStable();				break;
	case 0x14 : m_enWorkState = IsCollectedQueued();			break;
	/* Measuring the illuminance value */
	case 0x15 : m_enWorkState = GetLedPower();					break;
	/* Check whether all measurement are complete */
	case 0x16 : m_enWorkState = IsMeasCompleted(0x0b, 0x14);	break;
	/* Check whether all works are complete */
	case 0x17 : m_enWorkState = IsWorkCompleted(0x0a);			break;
	}
}

/*
 desc : Move stage XY
 parm : None
 retn : TRUE or FALSE
*/
ENG_WJSI CWorkMeas::SetStageMoveXY()
{
	TCHAR tzStep[128]	= {NULL};

	/* Calculated the location to be measured next */
	m_stPosSet.x	= m_stRecipe.cent_pos_x/* move to the X-Axis */;
	m_stPosSet.y	= m_stRecipe.cent_pos_y/* move to the Y-Axis */;

	/* Set the step name */
	swprintf_s(tzStep, 128, L"StageMoveXY [%u] : X: (%.4f mm), Y: (%.4f mm)",
			   m_u8PhNo, m_stPosSet.x, m_stPosSet.y);
	SetStepName(tzStep);

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_WJSI::en_next;

	return CWork::StageMoveXY(m_stPosSet.x, m_stPosSet.y) ? ENG_WJSI::en_next : ENG_WJSI::en_fail;
}

/*
 desc : Check whether the stage has been moved
 parm : None
 retn : en_next or en_wait
*/
ENG_WJSI CWorkMeas::IsStageMoved()
{
	/* Set the step name */
	SetStepName(L"IsStageMoved");

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_WJSI::en_next;
	/* Wait for at least 1 seconds */
	if (!IsWorkTimeout(1000))	return ENG_WJSI::en_wait;
	/* Verify that the stage has been moved */
	if (!CWork::IsStageMovedXY())
	{
		/* Check if the motion movement time has exceeded */
		if (IsWorkTimeout(60000/* 1 min */))	ENG_WJSI::en_fail;
		return ENG_WJSI::en_wait;
	}

	return ENG_WJSI::en_next;
}

/*
 desc : Move Photohead Z Axis
 parm : None
 retn : TRUE or FALSE
*/
ENG_WJSI CWorkMeas::SetPHMoveZ()
{
	/* Set the step name */
	SetStepName(L"Set.PH.Move.Z");

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_WJSI::en_next;

	return CWork::SetPHMoveZ(m_stRecipe.focus_pos_z/* move to the Z-Axis */);
}

/*
 desc : Check whether the photohead z axis has been moved
 parm : None
 retn : en_next or en_wait
*/
ENG_WJSI CWorkMeas::IsPHMovedZ()
{
	/* Set the step name */
	SetStepName(L"Is.PH.Moved.Z");

	/* Whether the motion reached normal... */
	ENG_WJSI enState	= CWork::IsPHMovedZ(m_stRecipe.focus_pos_z);
	if (enState != ENG_WJSI::en_next)	return enState;

	return ENG_WJSI::en_next;
}

/*
 desc : Check whether all measurement are complete
 parm : None
 retn : en_next or en_comp
*/
ENG_WJSI CWorkMeas::UpdateAmpIndex()
{
	/* reset the number of phothead led power (amplitude) */
	m_u16PassGet	= 0x0000;
#if 0	/* Processed by UpdateCalc() */
	/* Removes all the existing measurement items */
	m_vecPower.clear();
#endif
	/* You must set the flag to FALSE */
	m_bUpdateCalc	= FALSE;
	/* Set the power index for illuminance measurement */
	m_u16AmpIndex	= m_u16BeginIndex + m_u16MeasCount * m_u16IntvlIndex;

	return ENG_WJSI::en_next;
}

/*
 desc : Check whether all works are complete
 parm : step_go	- [in]  The value of the step number that you want to skip
 retn : en_next or en_comp
*/
ENG_WJSI CWorkMeas::IsWorkCompleted(UINT8 step_go)
{
	CWork::SetWorkCompleted();

	/* Save the largest value ever measured */
	if (m_dbMaxPower < m_dbPwrAvg)	m_dbMaxPower = m_dbPwrAvg;
	/* Check the difference between the maximum value and the current measurement value */
	/* And it decides whether to proceed or not */
	if ((m_dbMaxPower - m_dbPwrAvg) > 1.0f/*1 W*/)
	{
		LOG_WARN(ENG_EDIC::en_pled_illum, L"The illuminance power is no longer rising");
		return ENG_WJSI::en_next;
	}
	/* Then measure the led power of the next photohead */
	if (m_u16AmpIndex < m_u16EndIndex)
	{
		/* Measure again after waiting for a certain period of time */
		m_u8StepIt	= step_go;
		/* Set the photohead at the next location or the first photohead number */
		m_u16MeasCount++;	/* Increase the index value for the next measurement */
	}

	return ENG_WJSI::en_next;
}
