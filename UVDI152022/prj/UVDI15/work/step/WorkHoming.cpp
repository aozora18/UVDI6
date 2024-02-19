
/*
 desc : 기본 필수 장비 Homing
*/

#include "pch.h"
#include "../../MainApp.h"
#include "WorkHoming.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : 생성자
 parm : None
 retn : None
*/
CWorkHoming::CWorkHoming()
	: CWorkStep()
{
	m_enWorkJobID	= ENG_BWOK::en_work_home;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CWorkHoming::~CWorkHoming()
{
}

/*
 desc : 초기 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkHoming::InitWork()
{
	/* 내부 멤버 변수 값 초기화 */
	if (!CWork::InitWork())	return FALSE;

	/* 전체 작업 단계 */
	m_u8StepTotal = 0x11;

	return TRUE;
}

/*
 desc : 주기적으로 작업 수행
 parm : None
 retn : None
*/
VOID CWorkHoming::DoWork()
{
	/* 작업 단계 별로 동작 처리 */
	switch (m_u8StepIt)
	{
	case 0x01 : m_enWorkState = InitDriveErrorReset();					break;
	case 0x02 : m_enWorkState = IsDriveErrorReseted();					break;
	/* 모든 드라이브가 Homing 시작하는데 최소로 필요한 대기 시간 */
	case 0x03 : m_enWorkState = SetWorkWaitTime(1000);					break;
	case 0x04 : m_enWorkState = IsWorkWaitTime();						break;
	/* Stage X / Y / ACam1/2 and PH1/2 Drive에 대해서 Homing 수행 */
	case 0x05 : m_enWorkState = DoDriveHomingAll();						break;
	/* Stage X / Y / ACam1/2 and PH1/2 Drive의 Homing 하는데 최소 대기 (굳이 할 필요 없지만...) */
	case 0x06 : m_enWorkState = SetWorkWaitTime(500);					break;
	case 0x07 : m_enWorkState = IsWorkWaitTime();						break;
	/* Align Camera 2번의 리밋 여부 확인이 필요 (강제 멈춤 필요) */
	case 0x08 : m_enWorkState = WaitACamError(2);						break;
	/* Align Camera 2번이 리밋 치고, 약간 여유 동안 대기하기 위함 */
	case 0x09 : m_enWorkState = SetWorkWaitTime(1000);					break;
	case 0x0a : m_enWorkState = IsWorkWaitTime();						break;
	/* Align Camera 2번 강제로 멈추고, 1번 Align Camera Limit 상태 확인 */
	case 0x0b : m_enWorkState = ACam2HomingStop();						break;
	/* Align Camera 1번 Homing 완료될 때까지 대기 */
	case 0x0c : m_enWorkState = IsDrivedHomed(ENG_MMDI::en_align_cam1);	break;
	/* Align Camera 1번 리밋 확인 후 Align Camera 2번 Re-homing함 */
	case 0x0d : m_enWorkState = DoDriveHoming(ENG_MMDI::en_align_cam2);	break;
	/* Align Camera 2 Drive가 Homing 시작하는데 최소로 필요한 대기 시간 */
	case 0x0e : m_enWorkState = SetWorkWaitTime(2000);					break;
	case 0x0f : m_enWorkState = IsWorkWaitTime();						break;
	/* 모든 드라이브가 Homing 되었는지를 확인 (ACam2가 제일 늦음) */
	case 0x10 : m_enWorkState = IsDrivedHomedAll();						break;
	/* Luria Service 재시작 이후에 H/W 초기화를 반드시 해야하고 */
	/* 그리고 모션 초기화 이후에도 H/W 초기화를 반드시 해야하고 */
	case 0x11 : m_enWorkState = InitializeHWInit();						break;
	}

	/* 다음 작업 진행 여부 판단 */
	CWork::SetWorkNext();
	/* 장시간 동안 동일 위치를 반복 수행한다면 에러 처리 */
	CWork::CheckWorkTimeout();
}
