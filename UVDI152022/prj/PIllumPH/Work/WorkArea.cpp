
/*
 desc : The work - Area (Search the area to find the exact measurement location)
*/

#include "pch.h"
#include "../MainApp.h"
#include "WorkArea.h"


#ifdef	_DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : Constructor
 parm : recipe	- [in]  The structure stored the area measurement information
		run_mode- [in]  0x00 : Low Speed (Safe Mode), 0x01 : High Speed (Incomplete Mode)
		max_min	- [in]  This means that the difference between Min and Max values
						in the collected illumination data should not be greater than this value.
 retn : None
*/
CWorkArea::CWorkArea(LPG_MICL recipe, UINT8 run_mode, DOUBLE max_min)
	: CWork(recipe, run_mode, max_min)
{
	m_enWorkJobID	= ENG_WJOI::en_area;
	m_u8LedNo		= recipe->led_no;
	m_u16DropMin	= recipe->drop_min_cnt[0];
	m_u16DropMax	= recipe->drop_max_cnt[0];
	m_u16PassSet	= recipe->pass_cnt[0];
	m_u16ValidSet	= recipe->valid_cnt[0];
	m_u16AmpIndex	= recipe->amp_idx[0];
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CWorkArea::~CWorkArea()
{
}

/*
 desc : Initialize the work
 parm : None
 retn : None
*/
VOID CWorkArea::InitWork()
{
	/* Must call it */
	CWork::Init();
		
	/* Initialize the local parameters */
	m_u8StepTotal	= 0x12;
	m_u16MoveCol	= 0;
	m_u16MoveRow	= 0;
}


/*
 desc : Do work step by step
 parm : None
 retn : None
*/
VOID CWorkArea::StepWork()
{
	switch (m_u8StepIt)
	{
	/* Initializes the focus motor on the z-axis of photohead */
	case 0x01 : m_enWorkState = SetAllPHZAxisInit();			break;
	case 0x02 : m_enWorkState = IsValidMotorDriveAll();			break;
	/* Move to the position to be measured */
	case 0x03 : m_enWorkState = SetPHMoveZ();					break;
	case 0x04 : m_enWorkState = IsPHMovedZ();					break;
	/* Move to the position to be measured */
	case 0x05 : m_enWorkState = SetStageMoveXY();				break;
	case 0x06 : m_enWorkState = IsStageMovedXY();				break;
	/* It actually performs an action to turn off the power */
	case 0x07 : m_enWorkState = SetHWInited();					break;
	case 0x08 : m_enWorkState = IsHWInited();					break;
	/* Change to the mode thant can measure the illuminance in luria */
	case 0x09 : m_enWorkState = SetLoadInternalImage();			break;
	case 0x0a : m_enWorkState = IsLoadedInternalImage();		break;
	/* Wait for a while for it to stabilize (!!! Important !!!) */
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
	case 0x12 : m_enWorkState = IsWorkCompleted(0x04);			break;
	}
}

/*
 desc : Move stage XY
 parm : None
 retn : TRUE or FALSE
*/
ENG_WJSI CWorkArea::SetStageMoveXY()
{
	/* You must set the flag to FALSE */
	m_bUpdateCalc	= FALSE;
	/* Calculated the location to be measured next */
#if 0
	m_stPosSet.x	= m_stStart.x + DOUBLE(m_u16MoveCol) * m_stRecipe.move_dist_col /* move to the X-Axis */;
	m_stPosSet.y	= m_stStart.y + DOUBLE(m_u16MoveRow) * m_stRecipe.move_dist_row /* move to the Y-Axis */;
#else
	m_stPosSet.x = m_stRecipe.GetPosXY(0x00, (INT16)m_u16MoveCol);
	m_stPosSet.y = m_stRecipe.GetPosXY(0x02, (INT16)m_u16MoveRow);
#endif
	/* Set the step name */
	SetStepName(L"Set.Stage.Move.XY");

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_WJSI::en_next;

	return CWork::StageMoveXY(m_stPosSet.x, m_stPosSet.y) ? ENG_WJSI::en_next : ENG_WJSI::en_fail;
}

/*
 desc : Check whether the stage has been moved
 parm : None
 retn : en_next or en_wait
*/
ENG_WJSI CWorkArea::IsStageMovedXY()
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

	/* reset the number of phothead led power (amplitude) */
	m_u16PassGet	= 0x0000;
	/* Removes all the existing measurement items */
	m_vecPower.clear();

	return ENG_WJSI::en_next;
}

/*
 desc : Move Photohead Z Axis
 parm : None
 retn : TRUE or FALSE
*/
ENG_WJSI CWorkArea::SetPHMoveZ()
{
	/* Set the step name */
	SetStepName(L"Set.PH.Move.Z");
	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_WJSI::en_next;
	return CWork::SetPHMoveZ(12.0f);
}

/*
 desc : Check whether the photohead z axis has been moved
 parm : None
 retn : en_next or en_wait
*/
ENG_WJSI CWorkArea::IsPHMovedZ()
{
	/* Set the step name */
	SetStepName(L"IsPHMovedZ");

	/* Whether the motion reached normal... */
	return CWork::IsPHMovedZ(12.0f);
}

/*
 desc : Check whether all works are complete
 parm : step_go	- [in]  The value of the step number that you want to skip
 retn : en_next or en_comp
*/
ENG_WJSI CWorkArea::IsWorkCompleted(UINT8 step_go)
{
	CWork::SetWorkCompleted();

	/* Check whether all of the matrix area have been measured */
	if (!(m_u16MoveCol == (m_stRecipe.mat_cols * 2) &&
		  m_u16MoveRow == (m_stRecipe.mat_rows * 2)))
	{
		/* If all repeat measurements are done at the current location ... */
		m_u8StepIt	= step_go;
		/* Since it has to be measured from the beginning again, it initializes the column value */
		if (m_u16MoveCol == m_stRecipe.mat_cols * 2)
		{
			m_u16MoveRow++;
			m_u16MoveCol= 0x0000;
		}
		else
		{
			/* Measure the position of the following columns */
			m_u16MoveCol++;
		}
	}

	return ENG_WJSI::en_next;
}
