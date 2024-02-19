
/*
 desc : The work - The focus measurement of photohead
*/

#include "pch.h"
#include "../MainApp.h"
#include "WorkFocus.h"


#ifdef	_DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : Constructor
 parm : recipe	- [in]  The structure stored the focus measurement information
		run_mode- [in]  0x00 : Low Speed (Safe Mode), 0x01 : High Speed (Incomplete Mode)
		max_min	- [in]  This means that the difference between Min and Max values
						in the collected illumination data should not be greater than this value.
 retn : None
*/
CWorkFocus::CWorkFocus(LPG_MICL recipe, UINT8 run_mode, DOUBLE max_min)
	:CWork(recipe, run_mode, max_min)
{
	m_enWorkJobID	= ENG_WJOI::en_focus;
	m_u8LedNo		= recipe->led_no;
	m_u16DropMin	= recipe->drop_min_cnt[1];
	m_u16DropMax	= recipe->drop_max_cnt[1];
	m_u16PassSet	= recipe->pass_cnt[1];
	m_u16ValidSet	= recipe->valid_cnt[1];
	m_u16AmpIndex	= recipe->amp_idx[1];
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CWorkFocus::~CWorkFocus()
{
}

/*
 desc : Initialize the work
 parm : None
 retn : None
*/
VOID CWorkFocus::InitWork()
{
	/* Must call it */
	CWork::Init();
		
	/* Initialize the local parameters */
	m_u8StepTotal	= 0x12;
	/* Temporarily save the focus location */
	m_dbStartTopZ	= m_stRecipe.GetPosZ(0x00);
}


/*
 desc : Do work step by step
 parm : None
 retn : None
*/
VOID CWorkFocus::StepWork()
{
	switch (m_u8StepIt)
	{
	/* Move to the position to be measured */
	case 0x01 : m_enWorkState = SetStageMoveXY();				break;
	case 0x02 : m_enWorkState = IsStageMovedXY();				break;
	/* Initializes the focus motor on the z-axis of photohead */
	case 0x03 : m_enWorkState = SetAllPHZAxisInit();			break;
	case 0x04 : m_enWorkState = IsValidMotorDriveAll();			break;
	/* Move to the position to be measured */
	case 0x05 : m_enWorkState = SetPHMoveZ();					break;
	case 0x06 : m_enWorkState = IsPHMovedZ();					break;
	/* It actually performs an action to turn off the power */
	case 0x07 : m_enWorkState = SetHWInited();					break;
	case 0x08 : m_enWorkState = IsHWInited();					break;
	/* Change to the mode thant can measure the illuminance in luria */
	case 0x09 : m_enWorkState = SetLoadInternalImage();			break;
	case 0x0a : m_enWorkState = IsLoadedInternalImage();		break;
	/* Wait until the current illumination value is close to zero level */
	case 0x0b : m_enWorkState = IsIllumPowerZero();				break;
	/* Enter the LED Index on the luria (Light On) */
	case 0x0c : m_enWorkState = SetLedPower();					break;
	case 0x0d : m_enWorkState = IsSetLedPower();				break;
	case 0x0e : m_enWorkState = IsWaitLedStable();				break;
	case 0x0f : m_enWorkState = IsCollectedQueued();			break;
	/* Measuring the illuminance value */
	case 0x10 : m_enWorkState = GetLedPower();					break;
	/* Check whether all measurement are complete */
	case 0x11 : m_enWorkState = IsMeasCompleted(0x06, 0x0f);	break;
	/* Check whether all works are complete */
	case 0x12 : m_enWorkState = IsWorkCompleted(0x02);			break;
	}
}

/*
 desc : Move stage XY
 parm : None
 retn : TRUE or FALSE
*/
ENG_WJSI CWorkFocus::SetStageMoveXY()
{
	/* Set the step name */
	SetStepName(L"Stage.Move.XY");

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_WJSI::en_next;

	return CWork::StageMoveXY(m_stStart.x, m_stStart.y) ? ENG_WJSI::en_next : ENG_WJSI::en_fail;
}

/*
 desc : Check whether the stage has been moved
 parm : None
 retn : en_next or en_wait
*/
ENG_WJSI CWorkFocus::IsStageMovedXY()
{
	/* Set the step name */
	SetStepName(L"Is.Stage.Moved");

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
ENG_WJSI CWorkFocus::SetPHMoveZ()
{
	TCHAR tzStep[128]	= {NULL};

	/* You must set the flag to FALSE */
	m_bUpdateCalc	= FALSE;
	/* Calculated the location to be measured next */
	m_dbPosSetZ	= m_dbStartTopZ - DOUBLE(m_u16MoveCnt) * m_stRecipe.move_dist_z;	/* move to the Z-Axis */

	/* Set the step name */
	swprintf_s(tzStep, 128, L"Set.PH.Move.Z : %.4f mm", m_dbPosSetZ);
	SetStepName(tzStep);

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_WJSI::en_next;

	return CWork::SetPHMoveZ(m_dbPosSetZ);
}

/*
 desc : Check whether the photohead z axis has been moved
 parm : None
 retn : en_next or en_wait
*/
ENG_WJSI CWorkFocus::IsPHMovedZ()
{
	/* Set the step name */
	SetStepName(L"Is.PHMoved.Z");

	/* Whether the motion reached normal... */
	ENG_WJSI enState	= CWork::IsPHMovedZ(m_dbPosSetZ);
	if (enState != ENG_WJSI::en_next)	return enState;

	/* reset the number of phothead led power (amplitude) */
	m_u16PassGet	= 0x0000;
	/* Removes all the existing measurement items */
	m_vecPower.clear();

	return ENG_WJSI::en_next;
}

/*
 desc : Check whether all works are complete
 parm : step_go	- [in]  The value of the step number that you want to skip
 retn : en_next or en_comp
*/
ENG_WJSI CWorkFocus::IsWorkCompleted(UINT8 step_go)
{
	/* Set the step name */
	CWork::SetWorkCompleted();

	/* Check whether all of the matrix area have been measured */
	if (m_u16MoveCnt < m_stRecipe.mat_updn * 2)
	{
		/* Move the Z-Axis to the next position to be measured */
		m_u8StepIt	= step_go;
		/* Save the number of times the motion has moved */
		m_u16MoveCnt++;
	}

	return ENG_WJSI::en_next;
}
