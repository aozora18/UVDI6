
/*
 desc : The work object of Center Measurement
*/

#include "pch.h"
#include "../MainApp.h"
#include "WorkCent.h"


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
CWorkCent::CWorkCent(LPG_SCIM search)
	:CWork (search)
{
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CWorkCent::~CWorkCent()
{
}

/*
 desc : Initialize the member paramters
 parm : None
 retn : None
*/
VOID CWorkCent::InitWork()
{
	CWork::Init();

	/* Initialize the local parameters */
	m_dbMotionX		= 0.0f;
	m_dbMotionY		= 0.0f;
	m_u8StepTotal	= 0x06;
}

/*
 desc : Do work step by step
 parm : None
 retn : None
*/
VOID CWorkCent::StepWork()
{
	switch (m_u8StepIt)
	{
	/* Move to the position to be measured */
	case 0x01 : m_enWorkState = MotionMoveXY();		break;
	case 0x02 : m_enWorkState = IsMotionMoved();	break;
	/* Perform mark recognition at the current position */
	case 0x03 : m_enWorkState = GetGrabMark();		break;
	case 0x04 : m_enWorkState = IsGrabbedMark();	break;
	/* Check whether all measurement are complete */
	case 0x05 : m_enWorkState = IsMeasCompleted();	break;
	/* Check whether all works are complete */
	case 0x06 : m_enWorkState = IsWorkCompleted();	break;
	}
}

/*
 desc : Move motor XY
 parm : None
 retn : TRUE or FALSE
*/
ENG_WJSI CWorkCent::MotionMoveXY()
{
	TCHAR tzStep[128]= {NULL};
	DOUBLE dbNextPos = 0.0f;

	/* Calculated the location to be measured next */
	m_dbMotionX	= m_pstSearch->start_x/* move to the X-Axis */;
	m_dbMotionY	= m_pstSearch->start_y/* move to the Y-Axis */;
	dbNextPos	= m_pstSearch->interval * DOUBLE(m_u16MeasCount);
	if (0x01 == (0x01 & m_pstSearch->direct))	m_dbMotionX	+= dbNextPos;
	if (0x02 == (0x02 & m_pstSearch->direct))	m_dbMotionY	+= dbNextPos;

	/* Set the step name */
	swprintf_s(tzStep, 128, L"Motion.Move.XY (Count:%u)", m_u16MeasCount);
	SetStepName(tzStep);

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())
	{
//		if (!IsWorkTimeout(1000))	return ENG_WJSI::en_wait;
		return ENG_WJSI::en_next;
	}
	return CWork::MotionMoveXY(m_dbMotionX, m_dbMotionY) ? ENG_WJSI::en_next : ENG_WJSI::en_fail;
}

/*
 desc : Check whether the motor has been moved
 parm : None
 retn : en_next or en_wait
*/
ENG_WJSI CWorkCent::IsMotionMoved()
{
	/* Set the step name */
	SetStepName(L"Is.Motion.Moved");
	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())
	{
//		if (!IsWorkTimeout(1000))	return ENG_WJSI::en_wait;
		return ENG_WJSI::en_next;
	}
	/* Verify that the motor has been moved */
	if (!CWork::IsMotionMovedXY())
	{
		/* Check if the motion movement time has exceeded */
		if (IsWorkTimeout(60000/* 1 min */))	ENG_WJSI::en_fail;
		return ENG_WJSI::en_wait;
	}

	return ENG_WJSI::en_next;
}

/*
 desc : Capture the mark image from the current postion
 parm : None
 retn : wait, error, complete or next
*/
ENG_WJSI CWorkCent::GetGrabMark()
{
	UINT8 u8ChNo	= m_pstSearch->acam_id == 0x01 ? 0x01 : 0x02;

	/* Set the step name */
	SetStepName(L"Get.Grab.Mark");
	/* 기존 Live & Edge & Calibration 데이터 초기화 */
	uvEng_Camera_ResetGrabbedImage();
	/* Check if it is the emulated (demo) mode ... */
	if (!uvEng_GetConfig()->IsRunDemo())
	{
		/* Trigger 1개 발생 */
		if (!uvEng_Trig_ReqTrigOutOneOnly(u8ChNo))
		{
			LOG_ERROR(ENG_EDIC::en_stage_st, L"Failed to output the trigger");
			return ENG_WJSI::en_fail;
		}
	}
	/* Image Grabbed 시작한 시간 저장 */
	m_u64GrabbedTick	= GetTickCount64();

	return ENG_WJSI::en_next;
}

/*
 desc : Verify that the image was captured normally (successfully)
 parm : None
 retn : wait, error, complete or next
*/
ENG_WJSI CWorkCent::IsGrabbedMark()
{
	TCHAR tzVal[32]		= {NULL};
	ENG_MMDI enACamID	= ENG_MMDI(INT32(ENG_MMDI::en_align_cam1) + INT32(m_pstSearch->acam_id - 0x01));

	/* Set the step name */
	SetStepName(L"Is.Grabbed.Mark");

	if (uvEng_GetConfig()->IsRunDemo())
	{
		return ENG_WJSI::en_next;
	}
	else
	{
		/* Grabbed Image가 존재하는지 확인 */
		m_pstResult = uvEng_Camera_RunModelCali(m_pstSearch->acam_id, 0xff);
		if (m_pstResult && 0x00 != m_pstResult->marked)
		{
			return ENG_WJSI::en_next;	/* Go to the next step */
		}

		/* 3 초 동안 Grabbed Image가 없는 경우, 다시 트리거 발생 시키기 위해 뒤로 이동 */
		if (m_u64GrabbedTick+3000 < GetTickCount64())
		{
			m_u8StepIt	-= 2;
			return ENG_WJSI::en_next;
		}
	}
	return ENG_WJSI::en_wait;
}

/*
 desc : Check whether all measurement are complete
 parm : None
 retn : wait or next
*/
ENG_WJSI CWorkCent::IsMeasCompleted()
{
	/* Set the step name */
	SetStepName(L"Is.Meas.Completed");

	/* Calculates the result value */
	UpdateCalc();
	/* Increase the number of measurements */
	m_u16MeasCount++;

	return m_bUpdateCalc ? ENG_WJSI::en_next : ENG_WJSI::en_fail;
}

/*
 desc : Check whether all works are complete
 parm : None
 retn : en_next or en_comp
*/
ENG_WJSI CWorkCent::IsWorkCompleted()
{
	/* Set the step name */
	SetStepName(L"Is.Work.Completed");

	/* If all repeat measurements are done at the current location ... */
	if (m_pstSearch->count > m_u16MeasCount)
	{
		m_u8StepIt	= 0x00;	/* Go back to the initial measurement position */
	}

	return ENG_WJSI::en_next;
}

/*
 desc : Calculates the collected data
 parm : None
 retn : None
*/
VOID CWorkCent::UpdateCalc()
{
	/* To do */

	/* It the update operation was successful, set ti up */
	m_bUpdateCalc = TRUE;
}