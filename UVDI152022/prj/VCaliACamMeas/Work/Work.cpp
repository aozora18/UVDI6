
/*
 desc : Align Camera 2D Calibration 기본 객체
*/

#include "pch.h"
#include "../MainApp.h"
#include "Work.h"


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
CWork::CWork()
{
	/* 파일 초기화 (전체 경로 포함) */
	wmemset(m_tzFile, 0x00, MAX_PATH_LEN);
	m_fpCali		= NULL;
	/* Grabbed Image 크기 얻기 (단위: bytes) */
	m_u32GrabSize	= uvEng_GetConfig()->set_cams.ioi_size[0] *
					  uvEng_GetConfig()->set_cams.ioi_size[1];
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CWork::~CWork()
{
	/* 혹시 파일이 열려져 있으면 닫기 */
	CloseCaliFile();
}

/*
 desc : 현재 열려진 파일 닫기
 parm : None
 retn : None
*/
VOID CWork::CloseCaliFile()
{
	if (m_fpCali)	fclose(m_fpCali);
	m_fpCali	= NULL;
}

/*
 desc : 초기 작업 수행
 parm : cam_id	- [in]  오차 측정 카메라 인덱스 (1 or 2)
 retn : None
*/
VOID CWork::InitWork(UINT8 cam_id)
{
	m_pstCali		= &uvEng_GetConfig()->acam_cali;
	/* 측정 대상 카메라 ID */
	m_u8ACamID		= cam_id;
	/* 전체 작업 단계 */
	m_u8WorkTotal	= 0x0c;
	m_u8StepIt		= 0x01;
	m_u8TrigRetry	= 0x00;				/* 마크 인식 횟수 초기화 */
	m_u16MoveRows	= 0;
	m_u16MoveCols	= 0;
	m_u64StartTime	= GetTickCount64();
	m_u64WorkTime	= GetTickCount64();
	m_enWorkState	= ENG_JWNS::en_none;

	/* Align Camera is Calibration Mode */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);
	/* Trigger & Strobe - Enable 처리 */
	uvEng_Trig_ReqTriggerStrobe(TRUE);
}

/*
 desc : 일정 시간동안 대기 했는지 여부
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsWorkWaitTime()
{
#ifdef _DEBUG
	return	GetTickCount64() > (m_u64WaitTime + 60000);	/* 60 초 이상 지연되면 */
#else
	return	GetTickCount64() > (m_u64WaitTime + 20000);	/* 20 초 이상 지연되면 */
#endif
}

/*
 desc : 일정 시간 동안 작업을 대기하기 위함
 parm : time	- [in]  최소한의 대기에 필요한 시간 (단위: msec)
 retn : None
*/
VOID CWork::SetWorkWaitTime(UINT64 time)
{
	m_u64WaitTime	= GetTickCount64() + time;
}

/*
 desc : Check if the wait time for the work has been exceeded
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsWorkTimeout()
{
#ifdef _DEBUG
	return GetTickCount64() > (m_u64WorkTime + 30 * 1000/*30 sec*/);
#else
	return GetTickCount64() > (m_u64WorkTime + 10 * 1000/*10 sec*/);
#endif
}

/*
 desc : 임의 위치로 이동
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::SetMovingInit()
{
	UINT8 u8StartLed	= ((LPG_PMRW)uvEng_ShMem_GetPLC()->data)->r_start_button;
	DOUBLE dbPosStage[2], dbPosACamX[2], dbACam1PosX;
	DOUBLE dbVeloStage[2], dbVeloACam[2];

	/* 각 Axis 별로 기본 동작 속도 값 얻기 */
	dbVeloStage[0]	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_x];
	dbVeloStage[1]	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_y];
	dbVeloACam[0]	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_align_cam1];
	dbVeloACam[1]	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_align_cam2];

	/* 최종 이동하고자 하는 모션 위치 값 얻기 */
#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
	dbPosStage[0]	= m_pstCali->mark_stage_x;
#else
	dbPosStage[0]	= m_pstCali->mark_stage_x[m_u8ACamID-1];
#endif
	dbPosStage[1]	= m_pstCali->mark_stage_y[m_u8ACamID-1];
	if (0x01 == m_u8ACamID)
	{
		dbPosACamX[0]	= m_pstCali->mark_acam[0];
		dbPosACamX[1]	= uvEng_GetConfig()->mc2_svc.max_dist[(UINT8)ENG_MMDI::en_align_cam2];
	}
	else
	{
		dbPosACamX[0]	= uvEng_GetConfig()->mc2_svc.min_dist[(UINT8)ENG_MMDI::en_align_cam1];
		dbPosACamX[1]	= m_pstCali->mark_acam[1];
	}
#if 0
	/* Vacuum과 Shutter를 동시 제어 수행 */
	if (!uvEng_MCQ_LedVacuumShutter(u8StartLed, 0x01, 0x01))	return ENG_JWNS::en_error;
#endif
	/* 모든 Motor Drive의 토글 값 저장 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam2);

	/* 현재 카메라 1 번의 절대 위치 기준 대비, 이동하고자 하는 절대 위치 값이 큰 값인지 혹은 */
	/* 작은 값인지 여부에 따라, Align Camera 1 혹은 2 번 중 어느 것을 먼저 이동해야 할지 결정*/
	dbACam1PosX	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam1);
	/* Align Camera 1 or 2번을 무조건 끝 (왼쪽 (1) or (2) 오른쪽)으로 이동 */
	if (dbPosACamX[0] > dbACam1PosX)
	{
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2, dbPosACamX[1], dbVeloACam[1]))	return ENG_JWNS::en_error;
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, dbPosACamX[0], dbVeloACam[0]))	return ENG_JWNS::en_error;
	}
	else
	{
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, dbPosACamX[0], dbVeloACam[0]))	return ENG_JWNS::en_error;
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2, dbPosACamX[1], dbVeloACam[1]))	return ENG_JWNS::en_error;
	}
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, dbPosStage[0], dbVeloStage[0]))		return ENG_JWNS::en_error;
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dbPosStage[1], dbVeloStage[1]))		return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : Align Mark가 존재하는 시작 위치로 이동했는지 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsMovedInited()
{
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_x))		return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))		return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1))	return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam2))	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : 현재 진행률 값 반환
 parm : None
 retn : 진행률 값
*/
DOUBLE CWork::GetProcRate()
{
	DOUBLE dbRate	= 0.0f;

	/* 현재까지 작업 진행률 저장 */
	dbRate	= m_u8WorkTotal * (UINT32(m_u16MoveRows) * UINT32(m_pstCali->set_cols_count) + m_u16MoveCols);
	dbRate	+= m_u8StepIt + 1;

	return (dbRate / m_dbTotalRate) * 100.0f;
}