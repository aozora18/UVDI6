
/*
 desc : �ó����� �⺻ (Base) �Լ� ����
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
 desc : ������
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
 desc : �Ҹ���
 parm : None
 retn : None
*/
CCamSpecMgr::~CCamSpecMgr()
{
	m_pMeasureThread = NULL;
	delete m_pMeasureThread;
}

/*
 desc : �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CCamSpecMgr::Initialize()
{
}

/*
 desc : ���� �� ȣ��
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
 desc : ���ο� �˻��� ���� �˻� ����� �� ���
 parm : None
 retn : Non
*/
BOOL CCamSpecMgr::SetRegistModel()
{
	UINT32 u32Model = (UINT32)ENG_MMDT::en_circle, i = 0;
	DOUBLE dbMSize = 2.0f /* um */, dbMColor = uvEng_GetConfig()->acam_spec.in_ring_color /* 256:Black, 128:White */;
	LPG_CIEA pstCfg = uvEng_GetConfig();

	/* �˻� ��� ���� �� ��� */
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
				uvCmn_Camera_SetMarkFindMode(i + 1, 0, TMP_MARK); // CALIB�� ���� ���� MARK ���(MOD)
		}
	}

	return TRUE;
}


/*
 desc : �Է��� ����ŭ ��⵿���� �����Ѵ�.
 parm : ��� �ð�(ms)
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
 desc : ���� �̵��ϰ��� �ϴ� ī�޶� �ƴ� ���, �� ���� or ������ ������ �̵� ó��
 parm : TimeOut, �̵� ���� �Ÿ� ��
 retn : ���� ���� �� TRUE, ���� �� FALSE
*/
BOOL CCamSpecMgr::ACamMovingSide(int nTimeOut, double dDiffDistance)
{
	CTactTimeCheck cTime;			/* Ÿ�̸� */
	ENG_MMDI enACamDrv = ENG_MMDI::en_align_cam1;
	LPG_CMSI pstCfgMC2 = &uvEng_GetConfig()->mc2_svc;
	double dPos = 0;
	double dDiffPos = 0;

	/* ������ ī�޶� Ÿ���� ��� */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)uvEng_GetConfig()->set_comn.align_camera_kind)
	{
		/* ���� ����� �ƴ� Align Camera Drive ID�� �̵��ϰ��� �ϴ� ��ġ ��� */
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
			/* ���� �̵��ϰ��� �ϴ� ����� Toggled �� ��� */
			uvCmn_MC2_GetDrvDoneToggled(enACamDrv);

			/* �� ���� Ȥ�� �� �������� ���� ����� �ƴ� ī�޶� �̵� ��Ű�� */
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

		/* Ÿ�̸� �ʱ�ȭ �� ���� */
		cTime.Init();
		cTime.Start();

		/* ���� �ð����� ���� */
		while (nTimeOut > (int)cTime.GetTactMs())
		{
			if (TRUE == m_bStop)
			{
				return FALSE;
			}

			/* Ÿ�� ��ġ�� ������ġ�� ������ ���Ѵ�. */
			dDiffPos = dPos - uvCmn_MC2_GetDrvAbsPos(enACamDrv);

			/* Motor�� Ÿ����ġ�� �����ϰ� ���� ������ ��� ���� */
			if (FALSE == uvCmn_MC2_IsDriveBusy(enACamDrv) &&
				fabs(dDiffPos) < dDiffDistance)
			{
				return TRUE;
			}

			cTime.Stop();
			Sleep(1);
		}

		/* �ð� �ʰ� */
		return FALSE;
	}

	return TRUE;
}


/*
 desc : Align Camera�� Quartz�� �� ���̵��� Focusing�� ���� Z AXis ��ġ �̵�
 parm : None
 retn : ���� ���� �� TRUE, ���� �� FALSE
*/
BOOL CCamSpecMgr::ACamMovingZAxisQuartz()
{
	// UVDI�� ��� Philhmi�� ���� Z�� ����
	// Z�� ��� ���θ� Ȯ���� �� �ִ� �����?

	// ��� ����

	return TRUE;
}

/*
 desc : Align Camera�� Quartz�� �ִ� ��ġ�� �̵�
 parm : TimeOut, �̵� ���� �Ÿ� ��
 retn : ���� ���� �� TRUE, ���� �� FALSE
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

	/* ���� �̵��ϰ��� �ϴ� ����� Toggled �� ��� */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	
	
	if (enACamDrv != ENG_MMDI::en_axis_none)
	{
		uvCmn_MC2_GetDrvDoneToggled(enACamDrv);

		/* Motion �̵� ��Ű�� */
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

	/* Ÿ�̸� �ʱ�ȭ �� ���� */
	cTime.Init();
	cTime.Start();

	/* ���� �ð����� ���� */
	while (nTimeOut > (int)cTime.GetTactMs())
	{
		if (TRUE == m_bStop)
		{
			return FALSE;
		}

		/* Ÿ�� ��ġ�� ������ġ�� ������ ���Ѵ�. */
		dDiffXPos = dMoveXPos - uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);
		dDiffYPos = dMoveYPos - uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);
		
		dDiffACamPos = enACamDrv == ENG_MMDI::en_axis_none ? 0 : dMoveACamPos - uvCmn_MC2_GetDrvAbsPos(enACamDrv);

		/* Motor�� Ÿ����ġ�� �����ϰ� ���� ������ ��� ���� */
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

	/* �ð� �ʰ� */
	return FALSE;
}


/*
 desc : ���� Trigger�� Strobe ������ Enable ��Ų��.
 parm : TimeOut
 retn : ���� ���� �� TRUE, ���� �� FALSE
*/
BOOL CCamSpecMgr::WaitTrigEnabled(int nTimeOut/* = 60000*/)
{
	CTactTimeCheck cTime;

	/* ���� Trigger Mode�� Enable �������� Ȯ�� */
	if (TRUE == uvCmn_Mvenc_IsStrobEnable())
	{
		return TRUE;
	}

	/* Trigger Mode (Trigger & Strobe)�� ������ Enable ��Ŵ */
	if (FALSE == uvEng_Mvenc_ReqTriggerStrobe(TRUE))
	{
		return FALSE;
	}

	/* Ÿ�̸� �ʱ�ȭ �� ���� */
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

	/* �ð� �ʰ� */
	return FALSE;
}

/*
 desc : ���� ��ġ���� Mark Image�� Grab !!!
 parm : ��ݺ� Ƚ��
 retn : ���� ���� �� TRUE, ���� �� FALSE
*/
BOOL CCamSpecMgr::GrabQuartzData(STG_ACGR &stGrab, BOOL bRunMode, int nRetryCount)
{
	CTactTimeCheck cTime;
	//UINT8 u8ChNo = (1 == m_u8ACamID) ? 0x01 : 0x02;		/* Camera Number�� Channel Number */
	UINT8 u8ChNo = m_u8ACamID;

	UINT8 u8Mode = (TRUE == bRunMode) ? 0xFF : 0xFE;
	LPG_ACGR pstGrab = NULL;

	/* Grab Data �ʱ�ȭ */
	//pstGrab = NULL;

	// imsi mhbaek 231018 Set Retry count 3 -> 10
	nRetryCount = 10;

	/* ���� Live & Edge & Calibration ������ �ʱ�ȭ */
	uvEng_Camera_ResetGrabbedImage();

	/* Mark Model�� ��ϵǾ� �ִ��� ���� */
	if (!uvEng_Camera_IsSetMarkModelACam(u8ChNo, 2))
	{
		return FALSE;
	}

	/* ī�޶� Grabbed Mode�� Calibration Mode�� ���� */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);

	for (int i = 0; i < nRetryCount; i++)
	{
		/* Trigger 1�� �߻� */
		if (!uvEng_Mvenc_ReqTrigOutOne(u8ChNo))
		{
			return FALSE;
		}

		/* Ÿ�̸� �ʱ�ȭ �� ���� */
		cTime.Init();
		cTime.Start();

		while (3000 > (int)cTime.GetTactMs())
		{
			if (TRUE == m_bStop)
			{
				return FALSE;
			}

			/* Grabbed Image�� �����ϴ��� Ȯ�� */
			// imsi mhbaek 231018 change enum DISP_TYPE_CALB_CAMSPEC -> DISP_TYPE_MARK_LIVE
			pstGrab = uvEng_Camera_RunModelCali(u8ChNo, u8Mode, (UINT8)DISP_TYPE_CALB_CAMSPEC, TMP_MARK, TRUE, uvEng_GetConfig()->mark_find.image_process);	/* �̹��� �߽����� �̵��ϱ� ������ ������ 0xff �� */
			if (NULL != pstGrab && 0x00 != pstGrab->marked)
			{
				stGrab = *pstGrab;
				/* ���� �ܰ�� �̵� */
				return TRUE;
			}

			cTime.Stop();
			Sleep(1);
		}
	}

	/* �ð� �ʰ� */
	return FALSE;
}


/*
 desc : ���� ��ġ���� �־��� ���� ���� ��� �̼� ��ġ ����
 parm : �̵��� �Ÿ� ��, TimeOut, �̵� ���� �Ÿ� ��
 retn : ���� ���� �� TRUE, ���� �� FALSE
*/
BOOL CCamSpecMgr::MotionMoving(double dMoveX, double dMoveY, int nTimeOut, double dDiffDistance)
{
	CTactTimeCheck cTime;
	UINT8 u8InstAngle = uvEng_GetConfig()->set_cams.acam_inst_angle; // ī�޶� ��ġ�� ��, ȸ�� ���� (0: ȸ�� ����, 1: 180�� ȸ��)
	//ENG_MMDI enACamDrv = m_u8ACamID == 0x01 ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;
	ENG_MMDI enACamDrv;
	if (0x01 == m_u8ACamID)			enACamDrv = ENG_MMDI::en_align_cam1;
	else if (0x02 == m_u8ACamID)	enACamDrv = ENG_MMDI::en_align_cam2;
	else if (0x03 == m_u8ACamID)	enACamDrv = ENG_MMDI::en_stage_x;
	
	double dACamPosX = 0.0f, dStagePosY = 0.0f;
	double dDiffACamPos = 0, dDiffYPos = 0;
	double dVel = uvEng_GetConfig()->mc2_svc.step_velo;

	/* ���� ��ġ ��� */
	dACamPosX = uvCmn_MC2_GetDrvAbsPos(enACamDrv);
	dStagePosY = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);

	/* Align Camera �̵� */
	if (abs(dMoveX) > DEF_MOTION_CALI_CENTERING_MIN)
	{
		uvCmn_MC2_GetDrvDoneToggled(enACamDrv);

		/* Align Camera�� X ��ġ�� QuartZ�� �߾� ��ġ�� �̵� */
		dACamPosX = (0x01 == u8InstAngle) ? dACamPosX - (dMoveX * 0.5) : dACamPosX + (dMoveX * 0.5);
		if (CInterLockManager::GetInstance()->CheckMoveInterlock(enACamDrv, dACamPosX))
		{
			return FALSE;
		}
		if (FALSE == uvEng_MC2_SendDevAbsMove(enACamDrv, dACamPosX, dVel))	return FALSE;
		Sleep(100);
	}

	/* Stage Y �̵� */
	if (abs(dMoveY) > DEF_MOTION_CALI_CENTERING_MIN)
	{
		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);

		/* Align Camera�� X ��ġ�� QuartZ�� �߾� ��ġ�� �̵� */
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

	/* Ÿ�̸� �ʱ�ȭ �� ���� */
	cTime.Init();
	cTime.Start();

	/* ���� �ð����� ���� */
	while (nTimeOut > (int)cTime.GetTactMs())
	{
		if (TRUE == m_bStop)
		{
			return FALSE;
		}

		/* Ÿ�� ��ġ�� ������ġ�� ������ ���Ѵ�. */
		dDiffYPos = dStagePosY - uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);
		dDiffACamPos = dACamPosX - uvCmn_MC2_GetDrvAbsPos(enACamDrv);

		/* Motor�� Ÿ����ġ�� �����ϰ� ���� ������ ��� ���� */
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

	/* �ð� �ʰ� */
	return FALSE;
}


/*
 desc : ���� ���õ� ī�޶� ��������, Quartz�� Mark �߽� ��ġ�� ���� or ���������� �̵��ϱ� ����
		���� �Ÿ���ŭ Stage X ��ǥ �̵�
 parm : None
 retn : ���� ���� �� TRUE, ���� �� FALSE
*/
BOOL CCamSpecMgr::MotionMovingX(int nGrabCount, int nTimeOut, double dDiffDistance)
{
	CTactTimeCheck cTime;
	double dDist[eMODEL_FIND_COUNT] = { NULL };
	double dMoveXPos = 0;
	double dDiffXPos = 0;
	double dPointDist = uvEng_GetConfig()->acam_spec.point_dist;


	/* �̵� �Ÿ� �Է� */
	dDist[0] = dPointDist * -1;
	dDist[1] = dPointDist;

	/* ���� ���õ� Align Camera */
	dMoveXPos = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x) + dDist[nGrabCount];

	/* ���� �̵��ϰ��� �ϴ� ����� Toggled �� ��� */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	Sleep(100);
	if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_x, dMoveXPos))
	{
		return FALSE;
	}
	/* Motion �̵� ��Ű�� */
	if (FALSE == uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, dMoveXPos, 50))	return FALSE;
	Sleep(100);

#ifdef CAM_SPEC_SIMUL
	return TRUE;
#endif // CAM_SPEC_SIMUL

	/* Ÿ�̸� �ʱ�ȭ �� ���� */
	cTime.Init();
	cTime.Start();

	/* ���� �ð����� ���� */
	while (nTimeOut > (int)cTime.GetTactMs())
	{
		if (TRUE == m_bStop)
		{
			return FALSE;
		}

		/* Ÿ�� ��ġ�� ������ġ�� ������ ���Ѵ�. */
		dDiffXPos = dMoveXPos - uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);

		/* Motor�� Ÿ����ġ�� �����ϰ� ���� ������ ��� ���� */
		if (FALSE == uvCmn_MC2_IsDriveBusy(ENG_MMDI::en_stage_x) &&
			fabs(dDiffXPos) < dDiffDistance)
		{
			return TRUE;
		}

		cTime.Stop();
		Sleep(1);
	}

	/* �ð� �ʰ� */
	return FALSE;
}


/*
 desc : Align Camera�� Angle�� Pixel Resolution ���ϱ�
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
	UINT32 u32RepeatCount = 0;	// ���� �ݺ� ���� Ƚ��
	UINT8 u8WorkStep = 0x00;	// �۾� ����
	UINT8 u8GrabCount = 0x00;	// ���� ���� Ƚ�� ����
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
		/* �۾� �ܰ� ���� ���� ó�� */
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
				/* ���� ���� ���� 0.2 um �̳��̸� */
				if (abs(stGrab.move_mm_x) < DEF_MOTION_CALI_CENTERING_MIN &&
					abs(stGrab.move_mm_y) < DEF_MOTION_CALI_CENTERING_MIN)
				{
					//bRunState = MotionMoving(uvEng_GetConfig()->acam_spec.point_dist * -0.5, stGrab.move_mm_y);
					/* Quartz Mark �߽��� Left or Right �̵� �� �����ϱ� ���� ��ƾ���� Skip */
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
				/* Grabbed Image�� Mark �߽� ��ǥ */
				stGrabCentXY[u8GrabCount].x = stGrab.mark_cent_px_x;
				stGrabCentXY[u8GrabCount].y = stGrab.mark_cent_px_y;
			}

			break;
		default:
			// ������ ����
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
				// Grab View ����
				::SendMessageTimeout(hHwnd, eMSG_CAMERA_SPEC_GRAB_VIEW, NULL, NULL, SMTO_NORMAL, 100, NULL);
			}
		}

		if (u8WorkStep >= eMEASURE_GRAB_QUARTZ_X)
		{
			if (0 == u8GrabCount)
			{
				u8GrabCount++;
				u8WorkStep = eMEASURE_MOVE_X_POS;	/* 2 ��° (Right) �����ϱ� ���� �̵� ��ġ */
			}
			else if (++u32RepeatCount == m_u32RepeatTotal)
			{
				CalcACamSpec(stGrabCentXY[0], stGrabCentXY[1]);

				if (NULL != hHwnd)
				{
					// Grab View ����
					::SendMessageTimeout(hHwnd, eMSG_CAMERA_SPEC_COMPLETE, NULL, NULL, SMTO_NORMAL, 100, NULL);
				}

				m_bRunnigThread = FALSE;
				return TRUE;
			}
			else
			{
				u8GrabCount = 0x00;
				u8WorkStep = eMEASURE_MOVE_X_POS;	/* QuartZ �߽� ��ġ�� �̵� */

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
 desc : ���� ������ ����
 parm : UI ������ �ڵ�
 retn : None
*/
VOID CCamSpecMgr::MeasureStart(HWND hHwnd/* = NULL*/)
{
	m_bStop = FALSE;
	m_bRunnigThread = TRUE;
	m_pMeasureThread = AfxBeginThread(SpecMeasureThread, (LPVOID)hHwnd);
}

/*
 desc : ���� ������
 parm : UI ������ �ڵ�
 retn : 0
*/
UINT SpecMeasureThread(LPVOID lpParam)
{
	HWND hHwnd = (HWND)lpParam;

	CCamSpecMgr::GetInstance()->Measurement(hHwnd);

	return 0;
}