
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
	m_enWorkJobID	= ENG_BWOK::en_work_none;
	m_enVecMoveAxis	= ENG_MMDI::en_axis_none;
	m_u8WorkLast	= 0xff;
	m_enAlignMode	= ENG_AOEM::en_direct_expose;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CWork::~CWork()
{
}

/*
 desc : 초기 작업 수행
 parm : None
 retn : None
*/
VOID CWork::InitWork()
{
	/* 전체 작업 단계 */
	m_u8WorkTotal	= 0x0c;
	m_u8WorkStep	= 0x01;
	m_u8TrigRetry	= 0x00;				/* 마크 인식 횟수 초기화 */
	m_u64DelayTime	= GetTickCount64();
	m_enWorkState	= ENG_JWNS::en_none;
	m_u8WorkLast	= 0x0ff;	/* 무조건 0xff 로 고정 */

	/* Job Name 초기화 */
	wmemset(m_tzLastWorkJob, 0x00, MAX_WORK_JOB_NAME);

	/* Trigger & Strobe - Enable 처리 */
	uvEng_Trig_ReqTriggerStrobe(TRUE);
}

/*
 desc : 현재 작업 이름과 단계 설정
 parm : job_name	- [in]  작업 이름
 retn : None
*/
VOID CWork::SetWorkJobName(PTCHAR job_name)
{
	/* 가장 최근 값하고 동일하면 더 이상 갱신하지 않음 */
	if (m_u8WorkLast == m_u8WorkStep)	return;
	/* 변경된 가장 최근 작업 정보 갱신 */
	swprintf_s(m_tzLastWorkJob, MAX_WORK_JOB_NAME, L"%s [0x%02X / 0x%02X]",
			   job_name, m_u8WorkStep, m_u8WorkTotal);
	/* 가장 최근 값 임시 저장 */
	m_u8WorkLast	= m_u8WorkStep;
}

/*
 desc : 현재 작업 진행률 반환
 parm : None
 retn : 작업 진행률 (Percentage)
*/
DOUBLE CWork::GetWorkRate()
{
	DOUBLE dbRate	= DOUBLE(m_u8WorkStep) / (m_u8WorkTotal * 1.0f) * 100.0f;
	if (dbRate > 100.0f)	dbRate = 100.0f;
	return dbRate;
}

/*
 desc : 장시간 동일한 위치에서 대기 중인지 여부
 parm : None
 retn : None
*/
VOID CWork::IsWorkTimeOut()
{
	BOOL bSucc	= FALSE;
#ifdef _DEBUG
	bSucc	= GetTickCount64() > (m_u64DelayTime + 60000);	/* 60 초 이상 지연되면 */
#else
	bSucc	= GetTickCount64() > (m_u64DelayTime + 20000);	/* 20 초 이상 지연되면 */
#endif
	/* 지연 (대기) 시간이 초과되면 에러 처리 */
	if (bSucc)
	{
		m_enWorkState	= ENG_JWNS::en_error;
	}
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
 desc : 특정 시간동안 대기가 완료되었는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsWorkWaitTime()
{
	return	GetTickCount64() > m_u64WaitTime ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : 모든 Motor Drive가 중지 상태인지 여부 확인
 parm : title	- [in]  현재 수행 단계 출력 여부
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsMotorDriveStopAll()
{
	if (!IsWorkRepeat())	SetWorkJobName(L"IsMotorDriveStopAll");
	/* 임의 모션 트라이브가 바쁘면 에러 처리 */
#if 0
	if (!uvCmn_MC2_IsAnyDriveBusy())	TRACE(L"Busy Drive Number = %d\n", uvCmn_MC2_GetFirstBusyDriveNo());
#endif
	return !uvCmn_MC2_IsAnyDriveBusy() ? ENG_JWNS::en_next : ENG_JWNS::en_error;
}

/*
 desc : 현재 Trigger와 Strobe 동작을 Disable 시킨다.
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::SetTrigDisabled()
{
	if (!IsWorkRepeat())	SetWorkJobName(L"SetTrigDisabled");

	/* Trigger Mode (Trigger & Strobe)를 강제로 Disable 시킴 */
	if (!uvEng_Trig_ReqTriggerStrobe(FALSE))	return ENG_JWNS::en_error;
#if 0
	/* Trigger Disable Check 시간 저장 */
	TCHAR tzMesg[256]= {NULL};
	SYSTEMTIME stTm	= {NULL};
	GetLocalTime(&stTm);
	swprintf_s(tzMesg, 256, L"Add,SetTrigDisabled,%04d-%02d-%02d %02d:%02d:%02d",
				stTm.wYear, stTm.wMonth, stTm.wDay, stTm.wHour, stTm.wMinute, stTm.wSecond);
	TRACE(L"%s\n", tzMesg);
#endif
	return ENG_JWNS::en_next;
}

/*
 desc : 현재 Trigger와 Strobe가 Disable 상태인지 확인
 parm : check	- [in]  Trigger Disabled 상태 체크 여부
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsTrigDisabled(BOOL check)
{
	UINT8 u8TrigCh[2][2]= { {1, 3}, {2, 4} };

	if (!IsWorkRepeat())	SetWorkJobName(L"IsTrigDisabled");
	/* 현재 Trigger Mode가 Disable 상태인지 확인 */
	if (check && uvCmn_Trig_IsStrobEnable())	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : 거버가 등록되어 있는지 그리고 등록된 거버에 Mark가 존재하는지 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsRegistGerberCheck()
{
	if (!IsWorkRepeat())	SetWorkJobName(L"IsRegistGerberCheck");
	if (!uvCmn_Luria_IsSelectedJobName())	return ENG_JWNS::en_error;
	if (!IsExistGerberMark())				return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : 등록된 거버에 Mark가 존재하는지 여부
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsExistGerberMark()
{
	UINT8 i, u8Mark	= uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	STG_XMXY stPos	= {NULL};

	for (i=0; i<u8Mark; i++)
	{
		if (!uvEng_Luria_GetGlobalMark(i, stPos))	return FALSE;
	}

	return TRUE;
}

/*
 desc : Align Mark가 존재하는 시작 위치로 이동 수행
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::SetMovingAlignMark()
{
	BOOL bIsACam1		= TRUE;
	UINT8 u8Mark		= 0x00;
	DOUBLE dbPosStageY	= 0.0f, dbPosACamX = 0.0f, dbACamPos1, dbACamPos2;
	DOUBLE dbVeloACamX	= 0.0f, dbVeloStageY = 0.0f;
	ENG_MMDI enACamDrv	= ENG_MMDI::en_align_cam1;
	LPG_CMSI pstCfgMC2	= &uvEng_GetConfig()->mc2_svc;
	
	if (!IsWorkRepeat())	SetWorkJobName(L"SetMovingAlignMark");

	/* Global Mark 개수에 따라 */
	u8Mark	= uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	if (u8Mark != 2 && u8Mark != 4)	return ENG_JWNS::en_error;

	switch (u8Mark)
	{
	case 0x04	:	enACamDrv = (m_u16MarkNo < 3) ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;	break;
	case 0x02	:	enACamDrv = (m_u16MarkNo < 2) ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;	break;
	}
	/* 각 Axis 별로 기본 동작 속도 값 얻기 */
	dbVeloACamX	= pstCfgMC2->max_velo[(UINT8)enACamDrv];
	dbVeloStageY= pstCfgMC2->max_velo[(UINT8)ENG_MMDI::en_stage_y];

	/* 최종 이동하고자 하는 모션 위치 값 얻기 */
	if (!uvEng_Luria_GetGlobalMarkMoveXY(m_u16MarkNo, dbPosACamX, dbPosStageY))	return ENG_JWNS::en_error;

	/* 모든 Motor Drive의 토글 값 저장 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam2);

	/* Stage Moving (Vector) */
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dbPosStageY, dbVeloStageY))	return ENG_JWNS::en_error;

	/* 등록된 얼라인 마크 개수에 따라 */
	if (u8Mark == 0x04)
	{
		if (m_u16MarkNo > 2)	bIsACam1	= FALSE;
	}
	else if (u8Mark == 0x02)
	{
		if (m_u16MarkNo == 2)	bIsACam1	= FALSE;
	}
	/* 카메라 1번에 해당되는 Mark (1, 2)를 찾는 경우라면 */
	if (bIsACam1)
	{
		/* 2 번 카메라 위치 맨 마지막으로 이동 시킴 */
		dbACamPos2	= pstCfgMC2->max_dist[(UINT8)ENG_MMDI::en_align_cam2];	/* 0.1 um or 100 ns */
		dbACamPos1	= dbPosACamX;
	}
	/* 카메라 2번에 해당되는 Mark (3, 4)를 찾는 경우라면 */
	else
	{
		/* 1 번 카메라 위치 맨 처음으로 이동 시킴 */
		dbACamPos1	= 0.0f;	/* 0.1 um or 100 ns */
		dbACamPos2	= dbPosACamX;
	}

	/* Align Camera 이동 하기 */
	if (dbACamPos1 == 0.0f)
	{
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, dbACamPos1, dbVeloACamX))	return ENG_JWNS::en_error;
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2, dbACamPos2, dbVeloACamX))	return ENG_JWNS::en_error;
	}
	else
	{
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2, dbACamPos2, dbVeloACamX))	return ENG_JWNS::en_error;
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, dbACamPos1, dbVeloACamX))	return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : Align Mark가 존재하는 시작 위치로 이동했는지 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsMovedAlignMark()
{
	if (!IsWorkRepeat())	SetWorkJobName(L"IsMovedAlignMark");
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))		return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1))	return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam2))	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : 트리거 1개 발생 시킴
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::PutOneTrigger()
{
	UINT8 u8ChNo= m_u8ACamID == 1 ? 0x01 : 0x02;
	BOOL bSucc	= FALSE;

	if (!IsWorkRepeat())	SetWorkJobName(L"PutOneTrigger");
	/* Camera 동작 모드 설정 */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);
	/* 기존 검색된 Grabbed Data & Image 제거 */
	uvEng_Camera_ResetGrabbedMark();
	uvEng_Camera_ResetGrabbedImage();

	/* Trigger 강제로 (수동으로) 발생 */
	if (!uvEng_Trig_ReqTrigOutOne(u8ChNo, FALSE))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : 현재 가장 최근에 Grabbed Result 값 얻기
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::GetGrabResult()
{
	if (!IsWorkRepeat())	SetWorkJobName(L"GetGrabResult");
	/* 검색된 결과가 있는지 여부 */
	if (!(m_pstGrabResult = uvEng_Camera_RunModelCali(m_u8ACamID, 0xff)))
	{
		return ENG_JWNS::en_error;
	}

	/* 성공적으로 얻었으면 모드 변경 */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_none);

	return ENG_JWNS::en_next;
}

/*
 desc : 얼라인 마크 측정을 위해 스테이지를 Unloader 및
		Align Camera 1 / 2를 각각 Mark 1 / 3 번의 위치로 이동
 parm : None
 retn : TRUE or FALSE
*/
ENG_JWNS CWork::SetAlignMovingInit()
{
	UINT8 u8BaseXY		= 0x00, u8Mark = uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	UINT16 u16MarkType	= 0x0000;
	DOUBLE /*dbACamDistX, dbACamDistY, dbDiffMark, */dbACamVeloX, dbStageVeloY;
	DOUBLE dbStagePosX, dbStagePosY, dbACamX[2];
	LPG_CSAI pstAlign	= &uvEng_GetConfig()->set_align;

	if (!IsWorkRepeat())	SetWorkJobName(L"SetAlignMovingInit");
#if 0
	/* 카메라간 떨어진 물리적인 거리 확인 */
	uvEng_GetConfig()->acam_spec.GetACamDist(dbACamDistX, dbACamDistY);
	switch (u8Mark)
	{
	case 0x04	:	/* 2번과 4번 마크 간의 간격 (넓이; 오차) 얻기 */
		dbDiffMark	= abs(uvEng_Luria_GetGlobalMarkWidth(1));	break;	/* mm */
	case 0x02	:	/* 1번과 2번 마크 간의 간격 (넓이; 오차) 얻기 */		
		dbDiffMark	= abs(uvEng_Luria_GetGlobalMarkWidth(2));	break;	/* mm */
	}
#endif
	/* Stage X, Y, Camera 1 & 2 X 이동 좌표 얻기 */
	dbStagePosX	= pstAlign->mark2_stage_x;				/* Calibration 거버의 Mark 2 기준 위치 */
	dbStagePosY	= pstAlign->table_unloader_xy[0][1];	/* Stage Unloader 위치 */

	/* Mark 2 or 4 개가 아닌 경우, 지원하지 않음 */
	if (u8Mark != 2 && u8Mark != 4)
	{
		SetWorkJobName(L"SetAlignMovingInit:No marks");
		return ENG_JWNS::en_error;
	}

	/* -------------------------------------------------- */
	/* 마크 2개 혹은 4개 일 경우, 고려해서 위치 정보 계산 */
	/* -------------------------------------------------- */
	dbACamX[0]	= uvEng_Luria_GetACamMark2MotionX(2);		/* 얼라인 Mark 2 (Left/Bottom)번에 해당되는 카메라 1번의 X 축 실제 모션 위치 */
#if 0
	dbACamX[1]	= dbACamX[0] + dbDiffMark - dbACamDistX;	/* Camera 1대비 Camera2 값을 벌리려는 간격 */
#else
	dbACamX[1]	= uvEng_Luria_GetACamMark2MotionX(4);		/* Camera 1대비 Camera2 값을 벌리려는 간격 */
#endif
	/* 각 Axis 별로 기본 동작 속도 값 얻기 */
	dbStageVeloY	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_y];
	dbACamVeloX		= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_align_cam1];

	/* 모든 Motor Drive의 토글 값 저장 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam2);

	/* Stage Moving (Vector) (Vector Moving하는 기본 축 정보 저장) */
	if (!uuvEng_MC2_SendDevMoveVector(dbStagePosX, dbStagePosY, dbStageVeloY, m_enVecMoveAxis))
		return ENG_JWNS::en_error;

	/* ACam2 Axis 이동 수행 (ACam1 보다 반드시 먼저 수행) */
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2, dbACamX[1], dbACamVeloX))	return ENG_JWNS::en_error;
	/* ACam1 Axis 이동 수행 */
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, dbACamX[0], dbACamVeloX))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : Align Mark 위치로 이동 했는지 확인
 변수 :	None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsAlignMovedInit()
{
	if (!IsWorkRepeat())	SetWorkJobName(L"IsAlignMovedInit");

	if (ENG_MMDI::en_axis_none == m_enVecMoveAxis)	return ENG_JWNS::en_error;

	/* Stage X / Y, Align Camera 1 & 2의 X 이동이 모두 완료 되었는지 여부 */
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam2))	return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1))	return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(m_enVecMoveAxis))		return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : Luria 모드 값 설정 - Area
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::SetLuriaAreaMode()
{
	if (!IsWorkRepeat())	SetWorkJobName(L"SetLuriaAreaMode");

	/* 현재 Drive의 Done Toggled 값 저장 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	/* Area Mode만 설정함 */
	if (!uvEng_MC2_SendDevLuriaMode(ENG_MTAE::en_area))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : Luria 모드 값 선택 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsLuriaAreaMode()
{
	if (!IsWorkRepeat())	SetWorkJobName(L"IsLuriaAreaMode");

	/* Y 축 처음 위치로 이동 했는지 여부 확인 */
	if (uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))	return ENG_JWNS::en_next;

	return ENG_JWNS::en_wait;
}

/*
 desc : Gerber 파일 내에 저장된 Mark 들의 위치에 해당되는 모든 트리거 위치 계산 및 등록
 parm : None
 retn : None
 note : 트리거 좌표를 모두 읽었을 때 직사각형 (혹은 정사각형)이 아니고,
		마크 0과 1 그리고 마크 3과 2의 X 좌표가 같지 않는 경우에
		마크 1의 경우 마크 0의 X 좌표 기준으로 얼마나 떨어져 있는지 오차 값 계산 후 임시 저장
		마크 3의 경우 마크 3의 X 좌표 기준으로 얼마나 떨어져 있는지 오차 값 계산 후 임시 저장
*/
ENG_JWNS CWork::SetTrigCalcGlobal4()
{
	DOUBLE dbMarkPosX=0.0f, dbMarkPosY=0.0f, dbMarkDiff12X=0.0f, dbMarkDiff34X=0.0f;
	DOUBLE dbCaliErrX=0.0f, dbCaliErrY=0.0f;
#if 0
	DOUBLE dbUnloadPosY	= uvEng_GetConfig()->set_align.table_unloader_xy[0][1];
	DOUBLE *pTrigDelay	= uvEng_GetConfig()->cmps_sw.trig_cali_acam;
#endif
	if (!IsWorkRepeat())	SetWorkJobName(L"SetTrigCalcGlobal4");
#if 0
	/* 표준형 DI 장비의 경우 */
	m_dbAlignMoveEndY	= DBL_MAX;	/* 항상 최대 값으로 초기화 */
#if 0
	/* 얼라인 카메라 2 대간의 떨어진 설치 오차 (물리적인 거리) 값 확인 */
	/* (카메라 1번 기준으로 2번 카메라의 X (좌/우), Y (상/하) 오차 값) */
	uvEng_GetConfig()->acam_spec.GetACamDist(dbACamDistX, dbACamDistY);
#endif
	/* --------------------------------------------- */
	/* Global Mark가 인식될 Trigger Position 값 등록 */
	/* --------------------------------------------- */
	/* 4 개의 Mark 위치에 공통의 트리거 지연 값 적용 */
	/* (카메라 간의 높이 값 차이 값 포함) (단위: mm) */
	/* --------------------------------------------- */
	/* 2 번 (Left/Bottom) Mark를 인식하기 위한 트리거 위치 얻기  */
	if (!uvEng_Luria_GetGlobalMarkMoveXY(0x02, dbMarkPosX, dbMarkPosY))	return ENG_JWNS::en_error;
	m_dbTrigPos[0][1]	= dbUnloadPosY - dbMarkPosY + pTrigDelay[0] /* Trigger Delay */;	/* mm */
	if (m_dbAlignMoveEndY > dbMarkPosY)	m_dbAlignMoveEndY	= dbMarkPosY;
	if (ENG_AOEM::en_calib_expose == m_enAlignMode)
	{
		if (!uvEng_ACamCali_GetCaliPos(0x01, dbMarkPosX, dbMarkPosY, dbCaliErrX, dbCaliErrY))	return ENG_JWNS::en_error;
		if (!uvEng_Camera_SetCaliGDiffXY(0x01, 0x01, dbCaliErrX, dbCaliErrY))	return ENG_JWNS::en_error;
	}
	/* 기본적으로 2번 카메라의 4번 Mark를 인식하기 위한 Y Axis 트리거 위치 값 설정 (카메라 간의 높이 값은 여기서 제외) */
	if (!uvEng_Luria_GetGlobalMarkMoveXY(0x04, dbMarkPosX, dbMarkPosY))	return ENG_JWNS::en_error;
	m_dbTrigPos[1][1]	= dbUnloadPosY - dbMarkPosY + pTrigDelay[1] /* Trigger Delay */;	/* mm */
	if (m_dbAlignMoveEndY > dbMarkPosY)	m_dbAlignMoveEndY	= dbMarkPosY;
	if (ENG_AOEM::en_calib_expose == m_enAlignMode)
	{
		if (!uvEng_ACamCali_GetCaliPos(0x02, dbMarkPosX, dbMarkPosY, dbCaliErrX, dbCaliErrY))	return ENG_JWNS::en_error;
		if (!uvEng_Camera_SetCaliGDiffXY(0x02, 0x01, dbCaliErrX, dbCaliErrY))	return ENG_JWNS::en_error;
	}
	/* 1 번 (Left/Top) Mark를 인식하기 위한 트리거 위치 얻기 */
	if (!uvEng_Luria_GetGlobalMarkMoveXY(0x01, dbMarkPosX, dbMarkPosY))	return ENG_JWNS::en_error;
	m_dbTrigPos[0][0]	= dbUnloadPosY - dbMarkPosY + pTrigDelay[0] /* Trigger Delay */;	/* mm */
	if (ENG_AOEM::en_calib_expose == m_enAlignMode)
	{
		if (!uvEng_ACamCali_GetCaliPos(0x01, dbMarkPosX, dbMarkPosY, dbCaliErrX, dbCaliErrY))	return ENG_JWNS::en_error;
		if (!uvEng_Camera_SetCaliGDiffXY(0x01, 0x00, dbCaliErrX, dbCaliErrY))	return ENG_JWNS::en_error;
	}
	/* 3 번 (Right/Top) Mark를 인식하기 위한 트리거 위치 얻기  */
	if (!uvEng_Luria_GetGlobalMarkMoveXY(0x03, dbMarkPosX, dbMarkPosY))	return ENG_JWNS::en_error;
	m_dbTrigPos[1][0]	= dbUnloadPosY - dbMarkPosY + pTrigDelay[1] /* Trigger Delay */;	/* mm */
	if (ENG_AOEM::en_calib_expose == m_enAlignMode)
	{
		if (!uvEng_ACamCali_GetCaliPos(0x02, dbMarkPosX, dbMarkPosY, dbCaliErrX, dbCaliErrY))	return ENG_JWNS::en_error;
		if (!uvEng_Camera_SetCaliGDiffXY(0x02, 0x00, dbCaliErrX, dbCaliErrY))	return ENG_JWNS::en_error;
	}

	/* 얼라인 마크를 인식하기 위해.... 최종 이동될 Motion Y 축 좌표의 끝 점  */
	m_dbAlignMoveEndY	-= 5.0f;	/* 5 mm 더 아래로 이동하기 위함 */

	/* ----------------------------------------------------------------------------------- */
	/* 거버 데이터에 존재하는 1,2 과 3, 4 번 간의 X 좌표 오차 값 계산해서 임시 버퍼에 저장 */
	/* ----------------------------------------------------------------------------------- */
#if 0
	dbMarkDiff12X	= 0.0f;
	dbMarkDiff34X	= 0.0f;
#else
	/* Mark 1 (Left/Bottom)번 기준으로 했으므로, Mark1 번과 Mark 2번의 X 좌표 오차 값을 Mark 2의 X 값에 반영 */
	dbMarkDiff12X = uvEng_Luria_GetGlobalMarkDiffX(1, 2);
	/* Mark 3 (Right/Bottom)번 기준으로 했으므로, Mark3 번과 Mark 4번의 X 좌표 오차 값을 Mark 4의 X 값에 반영 */
	dbMarkDiff34X = uvEng_Luria_GetGlobalMarkDiffX(3, 4);
#endif
	uvEng_ACamCali_SetGlobalMarkDiffX(dbMarkDiff12X, dbMarkDiff34X);
#endif
	return ENG_JWNS::en_next;
}

/*
 desc : Align Mark를 Trigger Board에 등록
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::SetTrigRegistGlobal4()
{
	UINT8 u8LampType	= 0x01;		/* 0x01 : AMBER(1 ch, 3 ch), 0x02 : IR (2 ch, 4 ch) */
	UINT8 u8Mark		= uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	UINT8 u8Count		= 0x01;
	UINT8 u8TrigCh[2][2]= { {1, 3}, {2, 4} };

	if (!IsWorkRepeat())	SetWorkJobName(L"SetTrigRegistGlobal4");

	/* 현재 조명 타입 (AMBER or IR)에 따라, Trigger Board에 등록될 채널 변경 */
	u8LampType	= uvEng_GetConfig()->set_comn.strobe_lamp_type;
	if (u8LampType > 0x01)	return ENG_JWNS::en_error;
#if 0
	/* 각 카메라 마다 등록될 트리거 개수 */
	if (0x04 == u8Mark)	u8Count	= 0x02;
	if (!uvEng_Trig_ReqWriteAreaTrigPos(u8TrigCh[u8LampType][0], 0, u8Count, m_dbTrigPos[0],
										u8TrigCh[u8LampType][1], 0, u8Count, m_dbTrigPos[1],
										ENG_TEED::en_enable, TRUE))
	{
		return ENG_JWNS::en_error;
	}
#endif
	return ENG_JWNS::en_next;
}

/*
 desc : Align Mark가 Trigger Board에 모두 등록되었는지 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsTrigRegistedGlobal4()
{
	UINT8 u8LampType	= 0x00;		/* 0x00 : Coaxial Lamp (1 ch, 3 ch), 0x01 : Ring Lamp (2 ch, 4 ch) */
	UINT8 u8Count		= uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	UINT8 u8TrigCh[2][2]= { {1, 3}, {2, 4} }, u8TrigChNo = 0x00;		/* 0x01 ~ 0x04 */;
	UINT32 u32TrigPos	= 0;

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetWorkJobName(L"Is.Trig.Regist.Global");

	/* 현재 조명 타입 (AMBER or IR)에 따라, Trigger Board에 등록될 채널 변경 */
#if 1	/* 현재 장비에서는 트리거 이벤트는 무조건 채널 1번과 채널 2번에만 해당됨 */
	u8LampType	= uvEng_GetConfig()->set_comn.strobe_lamp_type;
#endif

	/* Trigger 속성 정보 설정 */
	u8TrigChNo	= u8TrigCh[u8LampType][0];
	u32TrigPos	= uvEng_ShMem_GetTrig()->trig_set[u8TrigChNo-1].area_trig_pos[0];
	/* Channel 1 */
	if (!uvEng_Trig_IsTrigPosEqual(0x01, 0x00, u32TrigPos))
	{
		return ENG_JWNS::en_wait;
	}
	u32TrigPos	= uvEng_ShMem_GetTrig()->trig_set[u8TrigChNo-1].area_trig_pos[1];
	if (u8Count == 0x04 && !uvEng_Trig_IsTrigPosEqual(0x01, 0x01, u32TrigPos))
	{
		return ENG_JWNS::en_wait;
	}
	/* Channel 3 */
	u8TrigChNo	= u8TrigCh[u8LampType][1];
	u32TrigPos	= uvEng_ShMem_GetTrig()->trig_set[u8TrigChNo-1].area_trig_pos[0];
	if (!uvEng_Trig_IsTrigPosEqual(0x02, 0, u32TrigPos))
	{
		return ENG_JWNS::en_wait;
	}
	u32TrigPos	= uvEng_ShMem_GetTrig()->trig_set[u8TrigChNo-1].area_trig_pos[1];
	if (u8Count == 0x04 && !uvEng_Trig_IsTrigPosEqual(0x02, 1, u32TrigPos))
	{
		return ENG_JWNS::en_wait;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : 얼라인 마크 측정을 위해 스테이지를 노광 끝 위치로 이동
 parm : None
 retn : TRUE or FALSE
*/
ENG_JWNS CWork::SetAlignMovingGlobal4()
{
	DOUBLE dbStageVeloY	= uvEng_GetConfig()->mc2_svc.mark_velo;

	if (!IsWorkRepeat())	SetWorkJobName(L"SetAlignMovingGlobal4");

	/* 현재 등록된 카메라 중 1개 이상 연결이 안된 경우, 에러 처리 */
	if (!uvCmn_Camera_IsConnected())	return ENG_JWNS::en_error;

	/* 이전 Axis의 Done Toggled 값 저장 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);

	/* -------------------------------------------------------------------- */
	/* Align Mark가 모두 측정 (검색)할 수 있도록, Y Axis 충분히 Stroke 수행 */
	/* 무조건, 노광 시작 위치로 이동하되, Stage Velocity 감안해서 추가 필요 */
	/* -------------------------------------------------------------------- */
	/* 절대 위치로 이동 즉, Mark 1과 Mark2 번 중간 지점까지 이동 */
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, m_dbAlignMoveEndY, dbStageVeloY))
	{
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : Align Mark 위치로 이동 했는지 확인
 변수 :	None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsAlignMovedGlobal()
{
	if (!IsWorkRepeat())	SetWorkJobName(L"IsAlignMovedGlobal");

	if (uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))	return ENG_JWNS::en_next;

	return ENG_JWNS::en_wait;
}

/*
 desc : 한번에 마크를 모두 인식 후, 정상적으로 마크가 확인되어 있는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsSetMarkValidGlobal4()
{
	UINT8 u8Mark= uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);	/* 거버에 등록된 Global & Local Fiducial Mark 개수 */

	if (!IsWorkRepeat())	SetWorkJobName(L"IsSetMarkValidGlobal4");

	/* Global Mark 4개 모두 인식되었는지 여부 확인 */
	if (uvEng_Camera_GetGrabbedCount() != u8Mark)	return ENG_JWNS::en_wait;
	/* 검색된 마크가 유효한지 확인 */
	if (!uvEng_Camera_IsGrabbedMarkValidAll(FALSE))	return ENG_JWNS::en_error;

	/* 모든 Mark가 인식될 때까지 대기 */
	return ENG_JWNS::en_wait;
}

/*
 desc : 스테이지 초기 시작 위치로 이동
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::SetMovingUnloader()
{
	DOUBLE dbStagePosX, dbStagePosY, dbStageVeloY;
	LPG_CSAI pstAlign	= &uvEng_GetConfig()->set_align;

	if (!IsWorkRepeat())	SetWorkJobName(L"SetMovingUnloader");

	/* Stage X, Y, Camera 1 & 2 X 이동 좌표 얻기 */
	dbStagePosX	= pstAlign->mark2_stage_x;				/* Calibration 거버의 Mark 2 기준 위치 */
	dbStagePosY	= pstAlign->table_unloader_xy[0][1];	/* Stage Unloader 위치 */
	/* 각 Axis 별로 기본 동작 속도 값 얻기 */
	dbStageVeloY= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_y];

	/* 모든 Motor Drive의 토글 값 저장 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);

	/* Stage Moving (Vector) (Vector Moving하는 기본 축 정보 저장) */
	if (!uvEng_MC2_SendDevMoveVector(dbStagePosX, dbStagePosY, dbStageVeloY, m_enVecMoveAxis))
		return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : 모든 드라이브가 다 이동 되었는지 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsMovedUnloader()
{
	if (!IsWorkRepeat())	SetWorkJobName(L"IsMovedUnloader");

	/* 현재 위치와 벡터로 이동하고자 하는 위치가 동일한 경우인지 */
	if (uvCmn_MC2_IsDrvDoneToggled(m_enVecMoveAxis))	return ENG_JWNS::en_next;

	return ENG_JWNS::en_wait;
}

/*
 desc : Luria Service에 측정된 Align Mark 오차 값 등록
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::SetAlignMarkRegist()
{
	TCHAR tzMesg[128]	= {NULL};
	UINT8 i, u8CamID	= 0x00, u8ImgID = 0x00, u8Mark;
	INT32 i32Inverse	= 1;
	STG_XMXY stMarkPos	= {NULL}, stMarkReg[4] = {NULL};
	LPG_ACGR pstGrab	= NULL;

	if (!IsWorkRepeat())	SetWorkJobName(L"SetAlignMarkRegist");

	/* XML 파일에 등록된 Mark 개수 확인 */
	u8Mark	= uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);

	/* Direct Expose이면서, 얼라인 마크가 없는 경우 */
	if (m_enWorkJobID == ENG_BWOK::en_expo_only && 0x00 == u8Mark)
	{
	}
	else
	{
		/* 전체 마크가 검색된 경우만 수행 */
		if (0x04 != u8Mark)	return ENG_JWNS::en_error;

		/* Global Fiducial 적재 */
		for (i=0; i<u8Mark; i++)
		{
			/* Gerber에 등록된 마크 위치 정보 얻기 */
			if (!uvEng_Luria_GetGlobalMark(i, stMarkPos))	return ENG_JWNS::en_error;
			/* ---------------------------------------------------------------------------- */
			/* 프로토콜 A2/0x02 쪽에 입력되는 Mark의 단위가 nm (나노미터) 이므로 (mm -> nm) */
			/* ---------------------------------------------------------------------------- */
			stMarkReg[i].mark_x	= stMarkPos.mark_x;
			stMarkReg[i].mark_y	= stMarkPos.mark_y;
		}

		/* Align Mark로 측정된 값만 적용 */
		if (m_enWorkJobID == ENG_BWOK::en_expo_mark || m_enWorkJobID == ENG_BWOK::en_expo_cali)
		{
			for (i=0; i<u8Mark; i++)
			{
				/* Global Mark < 4 > Points / Grabbed Image Index (Zero-based) */
				u8CamID	= i < 2 ? 0x01 : 0x02;
				u8ImgID	= (UINT8)ROUNDDN((i % 2), 0);
				/* Grabbed Mark images의 결과 값 가져오기 */
				pstGrab	= uvEng_Camera_GetGrabbedMark(u8CamID, u8ImgID);
				if (!pstGrab || !pstGrab->marked)	return ENG_JWNS::en_error;
				/* ------------------------------------------------------------------------------------------------- */
				/* Align Mark 측정 오차에 최종 Mark Offset 값 포함해서 위치가 재 조정 (프로토콜 A2/0x02의 단위가 nm) */
				/* ------------------------------------------------------------------------------------------------- */
				stMarkReg[i].mark_x	-= pstGrab->move_mm_x;
				stMarkReg[i].mark_y	+= pstGrab->move_mm_y;
			}
		}
		/* Align Mark로 측정된 오차 값에 Camera Calibration Offset 적용 */
		if (m_enWorkJobID == ENG_BWOK::en_expo_cali)
		{
		}

		/* Global Transformation Recipe 정보 설정 */
		if (!uvEng_Luria_ReqSetTransformationRecipe())	return ENG_JWNS::en_error;

		/* 기존 상태 값 초기화 */
		uvCmn_Luria_ResetRegistrationStatus();
#if 0
		/* Luria에 변경된 Mark 위치 값 등록 진행 */
		if (!uvEng_Luria_ReqSetRegistPointsAndRun(u8Mark, stMarkReg))	return ENG_JWNS::en_error;
#endif
		/* 패킷 요청 시간 임시 저장 */
		m_u64ReqSendTime	= GetTickCount64();

		/* Align Mark 등록 상태 값 요청 */
		if (!uvEng_Luria_ReqGetGetRegistrationStatus())	return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : Align Mark 오차 값 등록 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsAlignMarkRegisted()
{
	TCHAR tzStatus[128]	= {NULL};
	UINT8 u8Mark		= 0x00;
	UINT16 u16Status	= 0x0000;

	if (!IsWorkRepeat())	SetWorkJobName(L"IsAlignMarkRegisted");

	/* Direct Expose이면서, 얼라인 마크가 없는 경우 */
	if (m_enWorkJobID == ENG_BWOK::en_expo_only && 0x00 == u8Mark)
	{
	}
	else
	{
		/* Registration Status 값이 정상적으로 등록되었는지 확인 */
		u16Status	= uvCmn_Luria_GetRegistrationStatus();
		if (0xffff == u16Status)
		{
			/* 너무 자주 요청하면 안되므로 ... */
			if ((m_u64ReqSendTime + 250) < GetTickCount64())
			{
				/* 최근 상태 요청 명령 전송한 시간 저장 */
				m_u64ReqSendTime = GetTickCount64();
				/* 현재 Fiducial 등록 상태 값 요청 */
				if (!uvEng_Luria_ReqGetGetRegistrationStatus())	return ENG_JWNS::en_error;
			}
			return ENG_JWNS::en_wait;
		}
		/* 응답 에러 발생 */
		else if (0x01 != u16Status)
		{
			return ENG_JWNS::en_error;
		}
	}

	return ENG_JWNS::en_next;
}

/*
 desc : PrePrinting 수행
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::SetPrePrinting()
{
	if (!IsWorkRepeat())	SetWorkJobName(L"SetPrePrinting");

#if (USE_LUIRA_REQ_PERIOD_PKT)
	/* 주기적으로 패킷 송신하지 말라고 플래그 설정 */
	uvEng_Luria_SendPeriodPkt(FALSE);
#endif
	/* Luria Service에게 PrePrinting 호출 (내부적으로 Printing 상태 값 초기화) */
	if (!uvEng_Luria_ReqSetPrintOpt(0x00))	return ENG_JWNS::en_error;
	/* 명령 요청 시간 임시 저장 */
	m_u64ReqSendTime	= GetTickCount64();
	/* Printing Status 정보 요청 */
	if (!uvEng_Luria_ReqGetExposureState())	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : PrePrinting 완료 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsPrePrinted()
{
#if 1	/* Printing 단계에서도 체크 하므로, 여기서는 굳이 체크하지 않아도 됨 */
	if (!IsWorkRepeat())	SetWorkJobName(L"IsPrePrinted");

	ENG_LPES enState	= (ENG_LPES)uvCmn_Luria_GetExposeState();
	/* 현재 노광 상태가 준비되었는지 확인 */;
	if (0x0f == (UINT8(enState) & 0x0f))			return ENG_JWNS::en_error;
	/* 현재 노광 진행 상태가 성공적인 경우 */
	if (ENG_LPES::en_preprint_success == enState)	return ENG_JWNS::en_next;
	/* 다시 현재 노광 진행 상태 요청 */
	if (!uvEng_Luria_ReqGetExposureState())			return ENG_JWNS::en_error;
	return ENG_JWNS::en_wait;
#else
	return ENG_JWNS::en_next;
#endif
}

/*
 desc : Printing 수행
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::SetPrinting()
{
	if (!IsWorkRepeat())	SetWorkJobName(L"SetPrinting");

	/* 가장 최근에 노광한 개수 (Stripe) 개수 초기화 */
	m_u8PrintStripe	= 0x00;
	m_u64StripeTime	= GetTickCount64();	/* 가장 최근에 노광 개수가 변경된 시간 초기화 */

	/* Luria Service에게 Printing 호출 (내부적으로 Printing 상태 값 초기화) */
	if (!uvEng_Luria_ReqSetPrintOpt(0x01))	return ENG_JWNS::en_error;
	/* Printing Status 정보 요청 (함수 내부에 1초에 4번 밖에 요청하지 못함) */
	if (!uvEng_Luria_ReqGetExposureState())	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : Printing 완료 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsPrinted()
{
	TCHAR tzTitle[128]	= {NULL};
	ENG_LPES enState	= (ENG_LPES)uvCmn_Luria_GetExposeState();

	if (!IsWorkRepeat())	SetWorkJobName(L"IsPrinted");

	/* 현재 노광 상태가 준비되었는지 확인 */
	if (0x0f == (UINT8(enState) & 0x0f))
	{
#if 1	/* 바로 에러로 발생시키지 말고, 무한 대기 (결국 Timeout 될 때까지 대기) */
		return ENG_JWNS::en_error;
#endif
	}
	/* 만약 현재 노광 진행 중이면 ... */
	if (enState == ENG_LPES::en_print_running)
	{
		/* 현재 Delay Time Out 시간을 최근 시간으로 갱신 */
		m_u64DelayTime	= GetTickCount64();
	}
	else if (uvCmn_Luria_IsExposeStateSuccess(ENG_LCEP::en_print))
	{
		return ENG_JWNS::en_next;
	}
	/* 현재 변경된 노광 횟수가 있는지 여부에 따라 장시간 동안 노광 작업이 없었는지 유무 판단 */
	if (uvCmn_Luria_GetExposePrintCount() > m_u8PrintStripe)
	{
		m_u8PrintStripe	= uvCmn_Luria_GetExposePrintCount();
		m_u64StripeTime	= GetTickCount64();
	}
	else
	{
		/* 1 Stripe 노광하는데 최소 30초 이상 반응이 없다면 에러 처리 */
#if 0
		if (GetTickCount64() > m_u64StripeTime + 150000 /* 150초 */)
#else
		TRACE(L"Stripe Expose Time-out = %d\n", uvCmn_Luria_GetOneStripeExposeTime());
		if (GetTickCount64() > m_u64StripeTime + uvCmn_Luria_GetOneStripeExposeTime())
#endif
		{
			return ENG_JWNS::en_error;
		}
	}
	/* Printing Status 정보 요청 (함수 내부에 1초에 2번 밖에 요청하지 못함) */
#if 1	/* 굳이 에러 처리하지 말고... 대기 상태로 진입 */
	if (0x00 == uvEng_Luria_ReqGetExposureState())	return ENG_JWNS::en_error;
#else
	uvEng_Luria_ReqGetExposureState();
#endif

	/* Printing 끝났다고 해서, 바로 스테이지 이동하면 에러 발생 */
#ifdef _DEBUG
	SetWorkWaitTime(500);
#else
	SetWorkWaitTime(300);
#endif

#if (USE_LUIRA_REQ_PERIOD_PKT)
	/* 주기적으로 패킷 송신 작업 수행 */
	uvEng_Luria_SendPeriodPkt(TRUE);
#endif
	return ENG_JWNS::en_wait;
}

/*
 desc : 노광 작업할 준비가 되어 있는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::SetExpoReady()
{
	ENG_JWNS enState	= ENG_JWNS::en_next;

	if (!IsWorkRepeat())	SetWorkJobName(L"SetExpoReady");

	/* 현재 모든 photohead의 모터 상태 정보 요청 (일단. Z Axis이 Mid 상태가 체크 되어 있는지 여부 */
	if (!uvEng_Luria_ReqGetMotorStateAll())				return ENG_JWNS::en_error;
	/* 모든 모션 드라이브가 멈춘 상태이지 여부 확인 (일정 시간 멈출 때까지 기다려 줌) */
	if ( uvCmn_MC2_IsAnyDriveBusy())					return ENG_JWNS::en_wait;
	/* Gerber 등록 여부 */
	if (ENG_JWNS::en_next != IsRegistGerberCheck())		return ENG_JWNS::en_error;
	/* Photohead 별로 LED Power가 설정되어 있는지 여부 */
#ifndef _DEBUG
	if (!uvCmn_Luria_IsPHSetLedPowerAll())				return ENG_JWNS::en_error;
#endif
	/* 현재 Vacuum이 On 상태가 아닌지 혹은 Chiller가 On 상태가 아닌지 여부 */
	if (!IsVacuumOnOff(0x01) || !IsChillerRunning())	return ENG_JWNS::en_error;

	return SetTrigDisabled();
}

/*
 desc : Vacuum 가동/중지 확인
 parm : mode	- [in]  0x01 : Run, 0x00 : Stop
 retn : TRUE or FALSE
*/
BOOL CWork::IsVacuumOnOff(UINT8 mode)
{
	UINT8 u8Read	= 0x00;

	/* 현재 Vacuum 상태가 Waiting 모드인지 여부에 따라 */
	u8Read	= uvCmn_MCQ_ReadBitsValue((UINT32)ENG_PIOA::en_vacuum_vac_atm_waiting);
	if (u8Read)			return FALSE;
	/* 현재 Vacuum Status가 On or Off 인지 확인 */
	u8Read	= uvCmn_MCQ_ReadBitsValue((UINT32)ENG_PIOA::en_vacuum_status);
	if (u8Read != mode)	return FALSE;

	return TRUE;
}

/*
 desc : Chiller 동작 확인
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsChillerRunning()
{
	/* 칠러가 동작 중인지 확인 */
	return uvCmn_MCQ_ReadBitsValue((UINT32)ENG_PIOA::en_chiller_status) == 0x01;
}

/*
 desc : 광학계 초기화 : Homing
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::SetHomingPHAll()
{
	/* 기존 모터 상태 값 초기화 */
	uvCmn_Luria_ResetAllPhZAxisMidPosition();

	/* 중간 위치로 이동 */
	if (!uvEng_Luria_ReqSetMotorPositionInitAll(0x01))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : 광학계 초기화 : Homed?
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsHomedPHAll()
{
	if (!uvCmn_Luria_IsAllPhZAxisMidPosition())	return ENG_JWNS::en_wait;
	return ENG_JWNS::en_next;
}