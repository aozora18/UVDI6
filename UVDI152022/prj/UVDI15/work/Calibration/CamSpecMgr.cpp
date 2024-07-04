
/*
 desc : 시나리오 기본 (Base) 함수 모음
*/

#include "pch.h"
#include "CamSpecMgr.h"
#include "../../MainApp.h"


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
CCamSpecMgr::CCamSpecMgr()
{
	m_bStop = FALSE;
	m_bRunMode = FALSE;
	m_u8ACamID = 0x01;
	m_u32RepeatTotal = 0;
}

/*
 desc : 소멸자
 parm : None
 retn : None
*/
CCamSpecMgr::~CCamSpecMgr()
{
	m_pMeasureThread = NULL;
	delete m_pMeasureThread;
}

/*
 desc : 초기화
 parm : None
 retn : None
*/
VOID CCamSpecMgr::Initialize()
{
}

/*
 desc : 종료 시 호출
 parm : None
 retn : None
*/
VOID CCamSpecMgr::Terminate()
{
	m_bStop = TRUE;
	if (m_pMeasureThread != NULL)
	{
		WaitForSingleObject(m_pMeasureThread->m_hThread, INFINITE);
	}
}


/*
 desc : 새로운 검색을 위해 검색 대상의 모델 등록
 parm : None
 retn : Non
*/
BOOL CCamSpecMgr::SetRegistModel()
{
	UINT32 u32Model = (UINT32)ENG_MMDT::en_circle, i = 0;
	DOUBLE dbMSize = 2.0f /* um */, dbMColor = uvEng_GetConfig()->acam_spec.in_ring_color /* 256:Black, 128:White */;
	LPG_CIEA pstCfg = uvEng_GetConfig();

	/* 검색 대상에 대한 모델 등록 */
	dbMSize = pstCfg->acam_spec.in_ring_size * 1000.0f;

	for (i = 0; i < pstCfg->set_cams.acam_count; i++)
	{
		if (!uvEng_Camera_SetModelDefineEx(i + 1,
			pstCfg->mark_find.model_speed,
			pstCfg->mark_find.detail_level,
			pstCfg->mark_find.max_mark_find,
			pstCfg->mark_find.model_smooth,
			&u32Model, &dbMColor, &dbMSize,
			NULL, NULL, NULL, TMP_MARK,
			pstCfg->acam_spec.in_ring_scale_min,
			pstCfg->acam_spec.in_ring_scale_max))
		{
			return FALSE;
		}
		else {
				uvCmn_Camera_SetMarkFindMode(i + 1, 0, TMP_MARK); // CALIB는 직접 만든 MARK 사용(MOD)
		}
	}

	return TRUE;
}


/*
 desc : 입력한 값만큼 대기동작을 수행한다.
 parm : 대기 시간(ms)
 retn : None
*/
VOID CCamSpecMgr::Wait(int nTime)
{
	DWORD dwStart = GetTickCount();

	while (GetTickCount() - nTime < dwStart)
	{
		if (TRUE == m_bStop)
		{
			return;
		}

		Sleep(1);
	}
}


/*
 desc : 현재 이동하고자 하는 카메라가 아닌 경우, 맨 왼쪽 or 오른쪽 끝으로 이동 처리
 parm : TimeOut, 이동 오차 거리 값
 retn : 정상 동작 시 TRUE, 실패 시 FALSE
*/
BOOL CCamSpecMgr::ACamMovingSide(int nTimeOut, double dDiffDistance)
{
	CTactTimeCheck cTime;			/* 타이머 */
	ENG_MMDI enACamDrv = ENG_MMDI::en_align_cam1;
	LPG_CMSI pstCfgMC2 = &uvEng_GetConfig()->mc2_svc;
	double dPos = 0;
	double dDiffPos = 0;

	/* 외장형 카메라 타입인 경우 */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)uvEng_GetConfig()->set_comn.align_camera_kind)
	{
		/* 측정 대상이 아닌 Align Camera Drive ID가 이동하고자 하는 위치 얻기 */
		if (0x01 == m_u8ACamID)
		{
			enACamDrv = ENG_MMDI::en_align_cam2;
			dPos = pstCfgMC2->max_dist[(UINT8)enACamDrv];
		}
		else if (0x02 == m_u8ACamID)
		{
			enACamDrv = ENG_MMDI::en_align_cam1;
			dPos = pstCfgMC2->min_dist[(UINT8)enACamDrv];
		}
		else
		{
			enACamDrv = ENG_MMDI::en_axis_none;
			dPos = 0;
		}

		if (enACamDrv != ENG_MMDI::en_axis_none)
		{
			/* 현재 이동하고자 하는 모션의 Toggled 값 얻기 */
			uvCmn_MC2_GetDrvDoneToggled(enACamDrv);

			/* 맨 우측 혹은 맨 좌측으로 측정 대상이 아닌 카메라 이동 시키기 */
			if (FALSE == uvEng_MC2_SendDevAbsMove(enACamDrv, dPos, pstCfgMC2->max_velo[(UINT8)enACamDrv]))
			{
				return FALSE;
			}
		}
		else
		{
			return true;
		}
#ifdef CAM_SPEC_SIMUL
		return TRUE;
#endif // CAM_SPEC_SIMUL

		/* 타이머 초기화 및 시작 */
		cTime.Init();
		cTime.Start();

		/* 일정 시간동안 동작 */
		while (nTimeOut > (int)cTime.GetTactMs())
		{
			if (TRUE == m_bStop)
			{
				return FALSE;
			}

			/* 타겟 위치와 현재위치의 오차를 구한다. */
			dDiffPos = dPos - uvCmn_MC2_GetDrvAbsPos(enACamDrv);

			/* Motor가 타겟위치에 도달하고 정지 상태일 경우 종료 */
			if (FALSE == uvCmn_MC2_IsDriveBusy(enACamDrv) &&
				fabs(dDiffPos) < dDiffDistance)
			{
				return TRUE;
			}

			cTime.Stop();
			Sleep(1);
		}

		/* 시간 초과 */
		return FALSE;
	}

	return TRUE;
}


/*
 desc : Align Camera를 Quartz가 잘 보이도록 Focusing을 위해 Z AXis 위치 이동
 parm : None
 retn : 정상 동작 시 TRUE, 실패 시 FALSE
*/
BOOL CCamSpecMgr::ACamMovingZAxisQuartz()
{
	// UVDI의 경우 Philhmi를 통해 Z축 제어
	// Z축 사용 여부를 확인할 수 있는 방법은?

	// 잠시 보류

	return TRUE;
}

/*
 desc : Align Camera를 Quartz가 있는 위치로 이동
 parm : TimeOut, 이동 오차 거리 값
 retn : 정상 동작 시 TRUE, 실패 시 FALSE
*/
BOOL CCamSpecMgr::ACamMovingQuartz(int nTimeOut, double dDiffDistance)
{
	//ENG_MMDI enACamDrv = (0x01 == m_u8ACamID) ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;
	ENG_MMDI enACamDrv;
	if (0x01 == m_u8ACamID)			enACamDrv = ENG_MMDI::en_align_cam1;
	else if (0x02 == m_u8ACamID)	enACamDrv = ENG_MMDI::en_align_cam2;
	else if (0x03 == m_u8ACamID)	enACamDrv = ENG_MMDI::en_axis_none;
	

	double dMoveACamPos = 0, dMoveXPos = 0, dMoveYPos = 0;
	double dDiffACamPos = 0, dDiffXPos = 0, dDiffYPos = 0;
	CTactTimeCheck cTime;

	/* Quartz Position */
	if (0x03 == m_u8ACamID)
	{
		dMoveXPos = uvEng_GetConfig()->acam_spec.quartz_acam[m_u8ACamID - 1];
	}
	else
	{
		dMoveXPos = uvEng_GetConfig()->acam_spec.quartz_stage_x;
	}

	dMoveYPos = uvEng_GetConfig()->acam_spec.quartz_stage_y[m_u8ACamID - 1];
	dMoveACamPos = uvEng_GetConfig()->acam_spec.quartz_acam[m_u8ACamID - 1];

	/* 현재 이동하고자 하는 모션의 Toggled 값 얻기 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	
	
	if (enACamDrv != ENG_MMDI::en_axis_none)
	{
		uvCmn_MC2_GetDrvDoneToggled(enACamDrv);

		/* Motion 이동 시키기 */
		if (uvEng_MC2_SendDevAbsMove(enACamDrv, dMoveACamPos, uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)enACamDrv]) == false)
			return false;
		
	}

	if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_x, dMoveXPos))
	{
		return FALSE;
	}
	if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_y, dMoveYPos))
	{
		return FALSE;
	}
	if (FALSE == uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, dMoveXPos, uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_x]))	return FALSE;
	if (FALSE == uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dMoveYPos, uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_y]))	return FALSE;

#ifdef CAM_SPEC_SIMUL
	return TRUE;
#endif // CAM_SPEC_SIMUL

	/* 타이머 초기화 및 시작 */
	cTime.Init();
	cTime.Start();

	/* 일정 시간동안 동작 */
	while (nTimeOut > (int)cTime.GetTactMs())
	{
		if (TRUE == m_bStop)
		{
			return FALSE;
		}

		/* 타겟 위치와 현재위치의 오차를 구한다. */
		dDiffXPos = dMoveXPos - uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);
		dDiffYPos = dMoveYPos - uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);
		
		dDiffACamPos = enACamDrv == ENG_MMDI::en_axis_none ? 0 : dMoveACamPos - uvCmn_MC2_GetDrvAbsPos(enACamDrv);

		/* Motor가 타겟위치에 도달하고 정지 상태일 경우 종료 */
		if (FALSE == uvCmn_MC2_IsDriveBusy(ENG_MMDI::en_stage_x) &&
			FALSE == uvCmn_MC2_IsDriveBusy(ENG_MMDI::en_stage_y) &&
			FALSE == uvCmn_MC2_IsDriveBusy(enACamDrv) &&
			fabs(dDiffXPos) < dDiffDistance &&
			fabs(dDiffYPos) < dDiffDistance &&
			fabs(dDiffACamPos) < dDiffDistance)
		{
			return TRUE;
		}

		cTime.Stop();
		Sleep(1);
	}

	/* 시간 초과 */
	return FALSE;
}


/*
 desc : 현재 Trigger와 Strobe 동작을 Enable 시킨다.
 parm : TimeOut
 retn : 정상 동작 시 TRUE, 실패 시 FALSE
*/
BOOL CCamSpecMgr::WaitTrigEnabled(int nTimeOut/* = 60000*/)
{
	CTactTimeCheck cTime;

	/* 현재 Trigger Mode가 Enable 상태인지 확인 */
	if (TRUE == uvCmn_Mvenc_IsStrobEnable())
	{
		return TRUE;
	}

	/* Trigger Mode (Trigger & Strobe)를 강제로 Enable 시킴 */
	if (FALSE == uvEng_Mvenc_ReqTriggerStrobe(TRUE))
	{
		return FALSE;
	}

	/* 타이머 초기화 및 시작 */
	cTime.Init();
	cTime.Start();

	while (nTimeOut > (int)cTime.GetTactMs())
	{
		if (TRUE == m_bStop)
		{
			return FALSE;
		}

		if (TRUE == uvCmn_Mvenc_IsStrobEnable())
		{
			return TRUE;
		}

		cTime.Stop();
		Sleep(1);
	}

	/* 시간 초과 */
	return FALSE;
}

/*
 desc : 현재 위치에서 Mark Image를 Grab !!!
 parm : 재반복 횟수
 retn : 정상 동작 시 TRUE, 실패 시 FALSE
*/
BOOL CCamSpecMgr::GrabQuartzData(STG_ACGR &stGrab, BOOL bRunMode, int nRetryCount)
{
	CTactTimeCheck cTime;
	//UINT8 u8ChNo = (1 == m_u8ACamID) ? 0x01 : 0x02;		/* Camera Number가 Channel Number */
	UINT8 u8ChNo = m_u8ACamID;

	UINT8 u8Mode = (TRUE == bRunMode) ? 0xFF : 0xFE;
	LPG_ACGR pstGrab = NULL;

	/* Grab Data 초기화 */
	//pstGrab = NULL;

	// imsi mhbaek 231018 Set Retry count 3 -> 10
	nRetryCount = 10;

	/* 기존 Live & Edge & Calibration 데이터 초기화 */
	uvEng_Camera_ResetGrabbedImage();

	/* Mark Model이 등록되어 있는지 여부 */
	if (!uvEng_Camera_IsSetMarkModelACam(u8ChNo, 2))
	{
		return FALSE;
	}

	/* 카메라 Grabbed Mode를 Calibration Mode로 동작 */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);

	for (int i = 0; i < nRetryCount; i++)
	{
		/* Trigger 1개 발생 */
		if (!uvEng_Mvenc_ReqTrigOutOne(u8ChNo))
		{
			return FALSE;
		}

		/* 타이머 초기화 및 시작 */
		cTime.Init();
		cTime.Start();

		while (3000 > (int)cTime.GetTactMs())
		{
			if (TRUE == m_bStop)
			{
				return FALSE;
			}

			/* Grabbed Image가 존재하는지 확인 */
			// imsi mhbaek 231018 change enum DISP_TYPE_CALB_CAMSPEC -> DISP_TYPE_MARK_LIVE
			pstGrab = uvEng_Camera_RunModelCali(u8ChNo, u8Mode, (UINT8)DISP_TYPE_CALB_CAMSPEC, TMP_MARK, TRUE, uvEng_GetConfig()->mark_find.image_process);	/* 이미지 중심으로 이동하기 때문에 무조건 0xff 값 */
			if (NULL != pstGrab && 0x00 != pstGrab->marked)
			{
				stGrab = *pstGrab;
				/* 다음 단계로 이동 */
				return TRUE;
			}

			cTime.Stop();
			Sleep(1);
		}
	}

	/* 시간 초과 */
	return FALSE;
}


/*
 desc : 현재 위치에서 주어진 오차 값만 모션 미세 위치 조정
 parm : 이동할 거리 값, TimeOut, 이동 오차 거리 값
 retn : 정상 동작 시 TRUE, 실패 시 FALSE
*/
BOOL CCamSpecMgr::MotionMoving(double dMoveX, double dMoveY, int nTimeOut, double dDiffDistance)
{
	CTactTimeCheck cTime;
	UINT8 u8InstAngle = uvEng_GetConfig()->set_cams.acam_inst_angle; // 카메라가 설치될 때, 회전 여부 (0: 회전 없음, 1: 180도 회전)
	//ENG_MMDI enACamDrv = m_u8ACamID == 0x01 ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;
	ENG_MMDI enACamDrv;
	if (0x01 == m_u8ACamID)			enACamDrv = ENG_MMDI::en_align_cam1;
	else if (0x02 == m_u8ACamID)	enACamDrv = ENG_MMDI::en_align_cam2;
	else if (0x03 == m_u8ACamID)	enACamDrv = ENG_MMDI::en_stage_x;
	
	double dACamPosX = 0.0f, dStagePosY = 0.0f;
	double dDiffACamPos = 0, dDiffYPos = 0;
	double dVel = uvEng_GetConfig()->mc2_svc.step_velo;

	/* 현재 위치 얻기 */
	dACamPosX = uvCmn_MC2_GetDrvAbsPos(enACamDrv);
	dStagePosY = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);

	/* Align Camera 이동 */
	if (abs(dMoveX) > DEF_MOTION_CALI_CENTERING_MIN)
	{
		uvCmn_MC2_GetDrvDoneToggled(enACamDrv);

		/* Align Camera의 X 위치가 QuartZ의 중앙 위치로 이동 */
		dACamPosX = (0x01 == u8InstAngle) ? dACamPosX - (dMoveX * 0.5) : dACamPosX + (dMoveX * 0.5);
		if (CInterLockManager::GetInstance()->CheckMoveInterlock(enACamDrv, dACamPosX))
		{
			return FALSE;
		}
		if (FALSE == uvEng_MC2_SendDevAbsMove(enACamDrv, dACamPosX, dVel))	return FALSE;
		Sleep(100);
	}

	/* Stage Y 이동 */
	if (abs(dMoveY) > DEF_MOTION_CALI_CENTERING_MIN)
	{
		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);

		/* Align Camera의 X 위치가 QuartZ의 중앙 위치로 이동 */
		dStagePosY = (0x01 == u8InstAngle) ? dStagePosY - (dMoveY * 0.5) : dStagePosY + (dMoveY * 0.5);
		if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_y, dStagePosY))
		{
			return FALSE;
		}
		if (FALSE == uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dStagePosY, dVel)) return FALSE;
		Sleep(100);
	}

#ifdef CAM_SPEC_SIMUL
	return TRUE;
#endif // CAM_SPEC_SIMUL

	/* 타이머 초기화 및 시작 */
	cTime.Init();
	cTime.Start();

	/* 일정 시간동안 동작 */
	while (nTimeOut > (int)cTime.GetTactMs())
	{
		if (TRUE == m_bStop)
		{
			return FALSE;
		}

		/* 타겟 위치와 현재위치의 오차를 구한다. */
		dDiffYPos = dStagePosY - uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);
		dDiffACamPos = dACamPosX - uvCmn_MC2_GetDrvAbsPos(enACamDrv);

		/* Motor가 타겟위치에 도달하고 정지 상태일 경우 종료 */
		if (FALSE == uvCmn_MC2_IsDriveBusy(ENG_MMDI::en_stage_y) &&
			FALSE == uvCmn_MC2_IsDriveBusy(enACamDrv) &&
			fabs(dDiffYPos) <= dDiffDistance &&
			fabs(dDiffACamPos) <= dDiffDistance)
		{
			return TRUE;
		}

		cTime.Stop();
		Sleep(1);
	}

	/* 시간 초과 */
	return FALSE;
}


/*
 desc : 현재 선택된 카메라 기준으로, Quartz의 Mark 중심 위치를 왼쪽 or 오른쪽으로 이동하기 위해
		일정 거리만큼 Stage X 좌표 이동
 parm : None
 retn : 정상 동작 시 TRUE, 실패 시 FALSE
*/
BOOL CCamSpecMgr::MotionMovingX(int nGrabCount, int nTimeOut, double dDiffDistance)
{
	CTactTimeCheck cTime;
	double dDist[eMODEL_FIND_COUNT] = { NULL };
	double dMoveXPos = 0;
	double dDiffXPos = 0;
	double dPointDist = uvEng_GetConfig()->acam_spec.point_dist;


	/* 이동 거리 입력 */
	dDist[0] = dPointDist * -1;
	dDist[1] = dPointDist;

	/* 현재 선택된 Align Camera */
	dMoveXPos = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x) + dDist[nGrabCount];

	/* 현재 이동하고자 하는 모션의 Toggled 값 얻기 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	Sleep(100);
	if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_x, dMoveXPos))
	{
		return FALSE;
	}
	/* Motion 이동 시키기 */
	if (FALSE == uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, dMoveXPos, 50))	return FALSE;
	Sleep(100);

#ifdef CAM_SPEC_SIMUL
	return TRUE;
#endif // CAM_SPEC_SIMUL

	/* 타이머 초기화 및 시작 */
	cTime.Init();
	cTime.Start();

	/* 일정 시간동안 동작 */
	while (nTimeOut > (int)cTime.GetTactMs())
	{
		if (TRUE == m_bStop)
		{
			return FALSE;
		}

		/* 타겟 위치와 현재위치의 오차를 구한다. */
		dDiffXPos = dMoveXPos - uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);

		/* Motor가 타겟위치에 도달하고 정지 상태일 경우 종료 */
		if (FALSE == uvCmn_MC2_IsDriveBusy(ENG_MMDI::en_stage_x) &&
			fabs(dDiffXPos) < dDiffDistance)
		{
			return TRUE;
		}

		cTime.Stop();
		Sleep(1);
	}

	/* 시간 초과 */
	return FALSE;
}


/*
 desc : Align Camera의 Angle과 Pixel Resolution 구하기
 parm : 
 retn : None
*/
VOID CCamSpecMgr::CalcACamSpec(STG_DBXY stCenter1, STG_DBXY stCenter2)
{
	LPG_CASI pCamSpec = &uvEng_GetConfig()->acam_spec;
	double dDegree = 0.;
	double dPixelSize = 0.;
	double dCrossDist = 0.;

	/* Align Camera : Angle */
	dDegree = (atan2((stCenter1.y - stCenter2.y), (stCenter1.x - stCenter2.x)) * 180.0f) / M_PI;
	dDegree = (1 == uvEng_GetConfig()->set_cams.acam_inst_angle) ? dDegree - 180.0f : dDegree; // unit : degree
	
	if (180.0f < fabs(dDegree))
	{
		if (0 > dDegree)
		{
			dDegree += 360.0f;
		}
		else
		{
			dDegree -= 360.0f;
		}
	}

	pCamSpec->spec_angle[m_u8ACamID - 1] = dDegree;

	/* Align Camera : Pixel Resolution */
	dCrossDist = sqrt(pow(stCenter1.x - stCenter2.x, 2) + pow(stCenter1.y - stCenter2.y, 2));
	dPixelSize = (pCamSpec->point_dist * 1000.0f) / dCrossDist;
	pCamSpec->spec_pixel_um[m_u8ACamID - 1] = dPixelSize;	// unit : um
}


BOOL CCamSpecMgr::Measurement(HWND hHwnd/* = NULL*/)
{
	UINT32 u32RepeatCount = 0;	// 현재 반복 측정 횟수
	UINT8 u8WorkStep = 0x00;	// 작업 순서
	UINT8 u8GrabCount = 0x00;	// 각도 측정 횟수 증가
	CString* pstrText = nullptr;
	CString strText;
	BOOL bRunState = TRUE;

	STG_ACGR stGrab;
	stGrab.Init();
	STG_DBXY stGrabCentXY[eCOUNT_OF_CAMERA];

	if (NULL != hHwnd)
	{
		/* Post Message */
		strText.Format(_T("START CAMERA SPEC"));
		pstrText = &strText;

		::SendMessageTimeout(hHwnd, eMSG_CAMERA_SPEC_PROGRESS, 1, (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
	}

	while (TRUE == bRunState)
	{
		/* 작업 단계 별로 동작 처리 */
		switch (u8WorkStep)
		{
		case eMEASURE_MOVE_OTHER_HEAD_OUTWORD:
			strText.Format(_T("MOVE OTHER HEAD OUTWORD"));
			bRunState = ACamMovingSide();
			break;
		case eMEASURE_MOVE_HEAD_Z_POS:
			strText.Format(_T("MOVE HEAD Z POS"));
			bRunState = ACamMovingZAxisQuartz();
			break;
		case eMEASURE_MOVE_QUARTZ_POS:
			strText.Format(_T("MOVE QUARTZ POS"));
			bRunState = ACamMovingQuartz();
			break;
		case eMEASURE_READY_TRIGGER:
			strText.Format(_T("READY TRIGGER"));
			bRunState = WaitTrigEnabled();
			break;
		case eMEASURE_GRAB_QUARTZ:
			strText.Format(_T("[%d/%d] GRAB QUARTZ"), u32RepeatCount + 1, m_u32RepeatTotal);
			bRunState = GrabQuartzData(stGrab, TRUE);

			if (TRUE == bRunState)
			{
				/* 만약 오차 값이 0.2 um 이내이면 */
				if (abs(stGrab.move_mm_x) < DEF_MOTION_CALI_CENTERING_MIN &&
					abs(stGrab.move_mm_y) < DEF_MOTION_CALI_CENTERING_MIN)
				{
					//bRunState = MotionMoving(uvEng_GetConfig()->acam_spec.point_dist * -0.5, stGrab.move_mm_y);
					/* Quartz Mark 중심을 Left or Right 이동 후 측정하기 위한 루틴으로 Skip */
					u8WorkStep++;
					Sleep(2000);
				}
			}

			break;
		case eMEASURE_MOVE_CENTER:
			strText.Format(_T("[%d/%d] MOVE CENTER"), u32RepeatCount + 1, m_u32RepeatTotal);
			bRunState = MotionMoving(stGrab.move_mm_x, stGrab.move_mm_y);
			u8WorkStep = eMEASURE_READY_TRIGGER;
			break;
		case eMEASURE_MOVE_X_POS:
			strText.Format(_T("[%d/%d] MOVE X POS %d"), u32RepeatCount + 1, m_u32RepeatTotal, u8GrabCount + 1);
			bRunState = MotionMovingX((int)u8GrabCount);
			break;
		case eMEASURE_GRAB_QUARTZ_X:
			strText.Format(_T("[%d/%d] GRAB QUARTZ X %d"), u32RepeatCount + 1, m_u32RepeatTotal, u8GrabCount + 1);
			bRunState = GrabQuartzData(stGrab, m_bRunMode);

			if (TRUE == bRunState)
			{
				/* Grabbed Image의 Mark 중심 좌표 */
				stGrabCentXY[u8GrabCount].x = stGrab.mark_cent_px_x;
				stGrabCentXY[u8GrabCount].y = stGrab.mark_cent_px_y;
			}

			break;
		default:
			// 비정상 종료
			m_bRunnigThread = FALSE;
			return FALSE;
		}

		if (NULL != hHwnd)
		{
			/* Post Message */
			pstrText = &strText;
			::SendMessageTimeout(hHwnd, eMSG_CAMERA_SPEC_PROGRESS, (WPARAM)(((double)(u32RepeatCount + 1) / m_u32RepeatTotal) * 100.), (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
		}

		if (u8WorkStep > eMEASURE_GRAB_QUARTZ)
		{
			if (NULL != hHwnd)
			{
				// Grab View 갱신
				::SendMessageTimeout(hHwnd, eMSG_CAMERA_SPEC_GRAB_VIEW, NULL, NULL, SMTO_NORMAL, 100, NULL);
			}
		}

		if (u8WorkStep >= eMEASURE_GRAB_QUARTZ_X)
		{
			if (0 == u8GrabCount)
			{
				u8GrabCount++;
				u8WorkStep = eMEASURE_MOVE_X_POS;	/* 2 번째 (Right) 측정하기 위해 이동 조치 */
			}
			else if (++u32RepeatCount == m_u32RepeatTotal)
			{
				CalcACamSpec(stGrabCentXY[0], stGrabCentXY[1]);

				if (NULL != hHwnd)
				{
					// Grab View 갱신
					::SendMessageTimeout(hHwnd, eMSG_CAMERA_SPEC_COMPLETE, NULL, NULL, SMTO_NORMAL, 100, NULL);
				}

				m_bRunnigThread = FALSE;
				return TRUE;
			}
			else
			{
				u8GrabCount = 0x00;
				u8WorkStep = eMEASURE_MOVE_X_POS;	/* QuartZ 중심 위치로 이동 */

				CalcACamSpec(stGrabCentXY[0], stGrabCentXY[1]);

				if (NULL != hHwnd)
				{
					::SendMessageTimeout(hHwnd, eMSG_CAMERA_SPEC_RESULT, NULL, NULL, SMTO_NORMAL, 100, NULL);
				}
			}
		}
		else
		{
			u8WorkStep++;
			Sleep(2000);
		}

		Sleep(10);
	}

	if (NULL != hHwnd)
	{
		/* Post Message */
		strText.Format(_T("[%d/%d] STOP"), u32RepeatCount + 1, m_u32RepeatTotal);
		pstrText = &strText;
		::SendMessageTimeout(hHwnd, eMSG_CAMERA_SPEC_PROGRESS, (WPARAM)0, (LPARAM)pstrText, SMTO_NORMAL, 500, NULL);
	}

	// Error
	m_bRunnigThread = FALSE;
	return FALSE;
}


/*
 desc : 측정 쓰레드 실행
 parm : UI 윈도우 핸들
 retn : None
*/
VOID CCamSpecMgr::MeasureStart(HWND hHwnd/* = NULL*/)
{
	m_bStop = FALSE;
	m_bRunnigThread = TRUE;
	m_pMeasureThread = AfxBeginThread(SpecMeasureThread, (LPVOID)hHwnd);
}

/*
 desc : 측정 쓰레드
 parm : UI 윈도우 핸들
 retn : 0
*/
UINT SpecMeasureThread(LPVOID lpParam)
{
	HWND hHwnd = (HWND)lpParam;

	CCamSpecMgr::GetInstance()->Measurement(hHwnd);

	return 0;
}