
/*
 desc : Align Camera 2D Calibration �⺻ ��ü
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
 desc : ������
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
 desc : �ı���
 parm : None
 retn : None
*/
CWork::~CWork()
{
}

/*
 desc : �ʱ� �۾� ����
 parm : None
 retn : None
*/
VOID CWork::InitWork()
{
	/* ��ü �۾� �ܰ� */
	m_u8WorkTotal	= 0x0c;
	m_u8WorkStep	= 0x01;
	m_u8TrigRetry	= 0x00;				/* ��ũ �ν� Ƚ�� �ʱ�ȭ */
	m_u64DelayTime	= GetTickCount64();
	m_enWorkState	= ENG_JWNS::en_none;
	m_u8WorkLast	= 0x0ff;	/* ������ 0xff �� ���� */

	/* Job Name �ʱ�ȭ */
	wmemset(m_tzLastWorkJob, 0x00, MAX_WORK_JOB_NAME);

	/* Trigger & Strobe - Enable ó�� */
	uvEng_Trig_ReqTriggerStrobe(TRUE);
}

/*
 desc : ���� �۾� �̸��� �ܰ� ����
 parm : job_name	- [in]  �۾� �̸�
 retn : None
*/
VOID CWork::SetWorkJobName(PTCHAR job_name)
{
	/* ���� �ֱ� ���ϰ� �����ϸ� �� �̻� �������� ���� */
	if (m_u8WorkLast == m_u8WorkStep)	return;
	/* ����� ���� �ֱ� �۾� ���� ���� */
	swprintf_s(m_tzLastWorkJob, MAX_WORK_JOB_NAME, L"%s [0x%02X / 0x%02X]",
			   job_name, m_u8WorkStep, m_u8WorkTotal);
	/* ���� �ֱ� �� �ӽ� ���� */
	m_u8WorkLast	= m_u8WorkStep;
}

/*
 desc : ���� �۾� ����� ��ȯ
 parm : None
 retn : �۾� ����� (Percentage)
*/
DOUBLE CWork::GetWorkRate()
{
	DOUBLE dbRate	= DOUBLE(m_u8WorkStep) / (m_u8WorkTotal * 1.0f) * 100.0f;
	if (dbRate > 100.0f)	dbRate = 100.0f;
	return dbRate;
}

/*
 desc : ��ð� ������ ��ġ���� ��� ������ ����
 parm : None
 retn : None
*/
VOID CWork::IsWorkTimeOut()
{
	BOOL bSucc	= FALSE;
#ifdef _DEBUG
	bSucc	= GetTickCount64() > (m_u64DelayTime + 60000);	/* 60 �� �̻� �����Ǹ� */
#else
	bSucc	= GetTickCount64() > (m_u64DelayTime + 20000);	/* 20 �� �̻� �����Ǹ� */
#endif
	/* ���� (���) �ð��� �ʰ��Ǹ� ���� ó�� */
	if (bSucc)
	{
		m_enWorkState	= ENG_JWNS::en_error;
	}
}

/*
 desc : ���� �ð� ���� �۾��� ����ϱ� ����
 parm : time	- [in]  �ּ����� ��⿡ �ʿ��� �ð� (����: msec)
 retn : None
*/
VOID CWork::SetWorkWaitTime(UINT64 time)
{
	m_u64WaitTime	= GetTickCount64() + time;
}

/*
 desc : Ư�� �ð����� ��Ⱑ �Ϸ�Ǿ����� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsWorkWaitTime()
{
	return	GetTickCount64() > m_u64WaitTime ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : ��� Motor Drive�� ���� �������� ���� Ȯ��
 parm : title	- [in]  ���� ���� �ܰ� ��� ����
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsMotorDriveStopAll()
{
	if (!IsWorkRepeat())	SetWorkJobName(L"IsMotorDriveStopAll");
	/* ���� ��� Ʈ���̺갡 �ٻڸ� ���� ó�� */
#if 0
	if (!uvCmn_MC2_IsAnyDriveBusy())	TRACE(L"Busy Drive Number = %d\n", uvCmn_MC2_GetFirstBusyDriveNo());
#endif
	return !uvCmn_MC2_IsAnyDriveBusy() ? ENG_JWNS::en_next : ENG_JWNS::en_error;
}

/*
 desc : ���� Trigger�� Strobe ������ Disable ��Ų��.
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::SetTrigDisabled()
{
	if (!IsWorkRepeat())	SetWorkJobName(L"SetTrigDisabled");

	/* Trigger Mode (Trigger & Strobe)�� ������ Disable ��Ŵ */
	if (!uvEng_Trig_ReqTriggerStrobe(FALSE))	return ENG_JWNS::en_error;
#if 0
	/* Trigger Disable Check �ð� ���� */
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
 desc : ���� Trigger�� Strobe�� Disable �������� Ȯ��
 parm : check	- [in]  Trigger Disabled ���� üũ ����
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsTrigDisabled(BOOL check)
{
	UINT8 u8TrigCh[2][2]= { {1, 3}, {2, 4} };

	if (!IsWorkRepeat())	SetWorkJobName(L"IsTrigDisabled");
	/* ���� Trigger Mode�� Disable �������� Ȯ�� */
	if (check && uvCmn_Trig_IsStrobEnable())	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : �Ź��� ��ϵǾ� �ִ��� �׸��� ��ϵ� �Ź��� Mark�� �����ϴ��� Ȯ��
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
 desc : ��ϵ� �Ź��� Mark�� �����ϴ��� ����
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
 desc : Align Mark�� �����ϴ� ���� ��ġ�� �̵� ����
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

	/* Global Mark ������ ���� */
	u8Mark	= uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	if (u8Mark != 2 && u8Mark != 4)	return ENG_JWNS::en_error;

	switch (u8Mark)
	{
	case 0x04	:	enACamDrv = (m_u16MarkNo < 3) ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;	break;
	case 0x02	:	enACamDrv = (m_u16MarkNo < 2) ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;	break;
	}
	/* �� Axis ���� �⺻ ���� �ӵ� �� ��� */
	dbVeloACamX	= pstCfgMC2->max_velo[(UINT8)enACamDrv];
	dbVeloStageY= pstCfgMC2->max_velo[(UINT8)ENG_MMDI::en_stage_y];

	/* ���� �̵��ϰ��� �ϴ� ��� ��ġ �� ��� */
	if (!uvEng_Luria_GetGlobalMarkMoveXY(m_u16MarkNo, dbPosACamX, dbPosStageY))	return ENG_JWNS::en_error;

	/* ��� Motor Drive�� ��� �� ���� */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam2);

	/* Stage Moving (Vector) */
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dbPosStageY, dbVeloStageY))	return ENG_JWNS::en_error;

	/* ��ϵ� ����� ��ũ ������ ���� */
	if (u8Mark == 0x04)
	{
		if (m_u16MarkNo > 2)	bIsACam1	= FALSE;
	}
	else if (u8Mark == 0x02)
	{
		if (m_u16MarkNo == 2)	bIsACam1	= FALSE;
	}
	/* ī�޶� 1���� �ش�Ǵ� Mark (1, 2)�� ã�� ����� */
	if (bIsACam1)
	{
		/* 2 �� ī�޶� ��ġ �� ���������� �̵� ��Ŵ */
		dbACamPos2	= pstCfgMC2->max_dist[(UINT8)ENG_MMDI::en_align_cam2];	/* 0.1 um or 100 ns */
		dbACamPos1	= dbPosACamX;
	}
	/* ī�޶� 2���� �ش�Ǵ� Mark (3, 4)�� ã�� ����� */
	else
	{
		/* 1 �� ī�޶� ��ġ �� ó������ �̵� ��Ŵ */
		dbACamPos1	= 0.0f;	/* 0.1 um or 100 ns */
		dbACamPos2	= dbPosACamX;
	}

	/* Align Camera �̵� �ϱ� */
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
 desc : Align Mark�� �����ϴ� ���� ��ġ�� �̵��ߴ��� Ȯ��
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
 desc : Ʈ���� 1�� �߻� ��Ŵ
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::PutOneTrigger()
{
	UINT8 u8ChNo= m_u8ACamID == 1 ? 0x01 : 0x02;
	BOOL bSucc	= FALSE;

	if (!IsWorkRepeat())	SetWorkJobName(L"PutOneTrigger");
	/* Camera ���� ��� ���� */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);
	/* ���� �˻��� Grabbed Data & Image ���� */
	uvEng_Camera_ResetGrabbedMark();
	uvEng_Camera_ResetGrabbedImage();

	/* Trigger ������ (��������) �߻� */
	if (!uvEng_Trig_ReqTrigOutOne(u8ChNo, FALSE))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : ���� ���� �ֱٿ� Grabbed Result �� ���
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::GetGrabResult()
{
	if (!IsWorkRepeat())	SetWorkJobName(L"GetGrabResult");
	/* �˻��� ����� �ִ��� ���� */
	if (!(m_pstGrabResult = uvEng_Camera_RunModelCali(m_u8ACamID, 0xff)))
	{
		return ENG_JWNS::en_error;
	}

	/* ���������� ������� ��� ���� */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_none);

	return ENG_JWNS::en_next;
}

/*
 desc : ����� ��ũ ������ ���� ���������� Unloader ��
		Align Camera 1 / 2�� ���� Mark 1 / 3 ���� ��ġ�� �̵�
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
	/* ī�޶� ������ �������� �Ÿ� Ȯ�� */
	uvEng_GetConfig()->acam_spec.GetACamDist(dbACamDistX, dbACamDistY);
	switch (u8Mark)
	{
	case 0x04	:	/* 2���� 4�� ��ũ ���� ���� (����; ����) ��� */
		dbDiffMark	= abs(uvEng_Luria_GetGlobalMarkWidth(1));	break;	/* mm */
	case 0x02	:	/* 1���� 2�� ��ũ ���� ���� (����; ����) ��� */		
		dbDiffMark	= abs(uvEng_Luria_GetGlobalMarkWidth(2));	break;	/* mm */
	}
#endif
	/* Stage X, Y, Camera 1 & 2 X �̵� ��ǥ ��� */
	dbStagePosX	= pstAlign->mark2_stage_x;				/* Calibration �Ź��� Mark 2 ���� ��ġ */
	dbStagePosY	= pstAlign->table_unloader_xy[0][1];	/* Stage Unloader ��ġ */

	/* Mark 2 or 4 ���� �ƴ� ���, �������� ���� */
	if (u8Mark != 2 && u8Mark != 4)
	{
		SetWorkJobName(L"SetAlignMovingInit:No marks");
		return ENG_JWNS::en_error;
	}

	/* -------------------------------------------------- */
	/* ��ũ 2�� Ȥ�� 4�� �� ���, ����ؼ� ��ġ ���� ��� */
	/* -------------------------------------------------- */
	dbACamX[0]	= uvEng_Luria_GetACamMark2MotionX(2);		/* ����� Mark 2 (Left/Bottom)���� �ش�Ǵ� ī�޶� 1���� X �� ���� ��� ��ġ */
#if 0
	dbACamX[1]	= dbACamX[0] + dbDiffMark - dbACamDistX;	/* Camera 1��� Camera2 ���� �������� ���� */
#else
	dbACamX[1]	= uvEng_Luria_GetACamMark2MotionX(4);		/* Camera 1��� Camera2 ���� �������� ���� */
#endif
	/* �� Axis ���� �⺻ ���� �ӵ� �� ��� */
	dbStageVeloY	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_y];
	dbACamVeloX		= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_align_cam1];

	/* ��� Motor Drive�� ��� �� ���� */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam2);

	/* Stage Moving (Vector) (Vector Moving�ϴ� �⺻ �� ���� ����) */
	if (!uuvEng_MC2_SendDevMoveVector(dbStagePosX, dbStagePosY, dbStageVeloY, m_enVecMoveAxis))
		return ENG_JWNS::en_error;

	/* ACam2 Axis �̵� ���� (ACam1 ���� �ݵ�� ���� ����) */
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2, dbACamX[1], dbACamVeloX))	return ENG_JWNS::en_error;
	/* ACam1 Axis �̵� ���� */
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, dbACamX[0], dbACamVeloX))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : Align Mark ��ġ�� �̵� �ߴ��� Ȯ��
 ���� :	None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsAlignMovedInit()
{
	if (!IsWorkRepeat())	SetWorkJobName(L"IsAlignMovedInit");

	if (ENG_MMDI::en_axis_none == m_enVecMoveAxis)	return ENG_JWNS::en_error;

	/* Stage X / Y, Align Camera 1 & 2�� X �̵��� ��� �Ϸ� �Ǿ����� ���� */
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam2))	return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1))	return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(m_enVecMoveAxis))		return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : Luria ��� �� ���� - Area
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::SetLuriaAreaMode()
{
	if (!IsWorkRepeat())	SetWorkJobName(L"SetLuriaAreaMode");

	/* ���� Drive�� Done Toggled �� ���� */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	/* Area Mode�� ������ */
	if (!uvEng_MC2_SendDevLuriaMode(ENG_MTAE::en_area))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : Luria ��� �� ���� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsLuriaAreaMode()
{
	if (!IsWorkRepeat())	SetWorkJobName(L"IsLuriaAreaMode");

	/* Y �� ó�� ��ġ�� �̵� �ߴ��� ���� Ȯ�� */
	if (uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))	return ENG_JWNS::en_next;

	return ENG_JWNS::en_wait;
}

/*
 desc : Gerber ���� ���� ����� Mark ���� ��ġ�� �ش�Ǵ� ��� Ʈ���� ��ġ ��� �� ���
 parm : None
 retn : None
 note : Ʈ���� ��ǥ�� ��� �о��� �� ���簢�� (Ȥ�� ���簢��)�� �ƴϰ�,
		��ũ 0�� 1 �׸��� ��ũ 3�� 2�� X ��ǥ�� ���� �ʴ� ��쿡
		��ũ 1�� ��� ��ũ 0�� X ��ǥ �������� �󸶳� ������ �ִ��� ���� �� ��� �� �ӽ� ����
		��ũ 3�� ��� ��ũ 3�� X ��ǥ �������� �󸶳� ������ �ִ��� ���� �� ��� �� �ӽ� ����
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
	/* ǥ���� DI ����� ��� */
	m_dbAlignMoveEndY	= DBL_MAX;	/* �׻� �ִ� ������ �ʱ�ȭ */
#if 0
	/* ����� ī�޶� 2 �밣�� ������ ��ġ ���� (�������� �Ÿ�) �� Ȯ�� */
	/* (ī�޶� 1�� �������� 2�� ī�޶��� X (��/��), Y (��/��) ���� ��) */
	uvEng_GetConfig()->acam_spec.GetACamDist(dbACamDistX, dbACamDistY);
#endif
	/* --------------------------------------------- */
	/* Global Mark�� �νĵ� Trigger Position �� ��� */
	/* --------------------------------------------- */
	/* 4 ���� Mark ��ġ�� ������ Ʈ���� ���� �� ���� */
	/* (ī�޶� ���� ���� �� ���� �� ����) (����: mm) */
	/* --------------------------------------------- */
	/* 2 �� (Left/Bottom) Mark�� �ν��ϱ� ���� Ʈ���� ��ġ ���  */
	if (!uvEng_Luria_GetGlobalMarkMoveXY(0x02, dbMarkPosX, dbMarkPosY))	return ENG_JWNS::en_error;
	m_dbTrigPos[0][1]	= dbUnloadPosY - dbMarkPosY + pTrigDelay[0] /* Trigger Delay */;	/* mm */
	if (m_dbAlignMoveEndY > dbMarkPosY)	m_dbAlignMoveEndY	= dbMarkPosY;
	if (ENG_AOEM::en_calib_expose == m_enAlignMode)
	{
		if (!uvEng_ACamCali_GetCaliPos(0x01, dbMarkPosX, dbMarkPosY, dbCaliErrX, dbCaliErrY))	return ENG_JWNS::en_error;
		if (!uvEng_Camera_SetCaliGDiffXY(0x01, 0x01, dbCaliErrX, dbCaliErrY))	return ENG_JWNS::en_error;
	}
	/* �⺻������ 2�� ī�޶��� 4�� Mark�� �ν��ϱ� ���� Y Axis Ʈ���� ��ġ �� ���� (ī�޶� ���� ���� ���� ���⼭ ����) */
	if (!uvEng_Luria_GetGlobalMarkMoveXY(0x04, dbMarkPosX, dbMarkPosY))	return ENG_JWNS::en_error;
	m_dbTrigPos[1][1]	= dbUnloadPosY - dbMarkPosY + pTrigDelay[1] /* Trigger Delay */;	/* mm */
	if (m_dbAlignMoveEndY > dbMarkPosY)	m_dbAlignMoveEndY	= dbMarkPosY;
	if (ENG_AOEM::en_calib_expose == m_enAlignMode)
	{
		if (!uvEng_ACamCali_GetCaliPos(0x02, dbMarkPosX, dbMarkPosY, dbCaliErrX, dbCaliErrY))	return ENG_JWNS::en_error;
		if (!uvEng_Camera_SetCaliGDiffXY(0x02, 0x01, dbCaliErrX, dbCaliErrY))	return ENG_JWNS::en_error;
	}
	/* 1 �� (Left/Top) Mark�� �ν��ϱ� ���� Ʈ���� ��ġ ��� */
	if (!uvEng_Luria_GetGlobalMarkMoveXY(0x01, dbMarkPosX, dbMarkPosY))	return ENG_JWNS::en_error;
	m_dbTrigPos[0][0]	= dbUnloadPosY - dbMarkPosY + pTrigDelay[0] /* Trigger Delay */;	/* mm */
	if (ENG_AOEM::en_calib_expose == m_enAlignMode)
	{
		if (!uvEng_ACamCali_GetCaliPos(0x01, dbMarkPosX, dbMarkPosY, dbCaliErrX, dbCaliErrY))	return ENG_JWNS::en_error;
		if (!uvEng_Camera_SetCaliGDiffXY(0x01, 0x00, dbCaliErrX, dbCaliErrY))	return ENG_JWNS::en_error;
	}
	/* 3 �� (Right/Top) Mark�� �ν��ϱ� ���� Ʈ���� ��ġ ���  */
	if (!uvEng_Luria_GetGlobalMarkMoveXY(0x03, dbMarkPosX, dbMarkPosY))	return ENG_JWNS::en_error;
	m_dbTrigPos[1][0]	= dbUnloadPosY - dbMarkPosY + pTrigDelay[1] /* Trigger Delay */;	/* mm */
	if (ENG_AOEM::en_calib_expose == m_enAlignMode)
	{
		if (!uvEng_ACamCali_GetCaliPos(0x02, dbMarkPosX, dbMarkPosY, dbCaliErrX, dbCaliErrY))	return ENG_JWNS::en_error;
		if (!uvEng_Camera_SetCaliGDiffXY(0x02, 0x00, dbCaliErrX, dbCaliErrY))	return ENG_JWNS::en_error;
	}

	/* ����� ��ũ�� �ν��ϱ� ����.... ���� �̵��� Motion Y �� ��ǥ�� �� ��  */
	m_dbAlignMoveEndY	-= 5.0f;	/* 5 mm �� �Ʒ��� �̵��ϱ� ���� */

	/* ----------------------------------------------------------------------------------- */
	/* �Ź� �����Ϳ� �����ϴ� 1,2 �� 3, 4 �� ���� X ��ǥ ���� �� ����ؼ� �ӽ� ���ۿ� ���� */
	/* ----------------------------------------------------------------------------------- */
#if 0
	dbMarkDiff12X	= 0.0f;
	dbMarkDiff34X	= 0.0f;
#else
	/* Mark 1 (Left/Bottom)�� �������� �����Ƿ�, Mark1 ���� Mark 2���� X ��ǥ ���� ���� Mark 2�� X ���� �ݿ� */
	dbMarkDiff12X = uvEng_Luria_GetGlobalMarkDiffX(1, 2);
	/* Mark 3 (Right/Bottom)�� �������� �����Ƿ�, Mark3 ���� Mark 4���� X ��ǥ ���� ���� Mark 4�� X ���� �ݿ� */
	dbMarkDiff34X = uvEng_Luria_GetGlobalMarkDiffX(3, 4);
#endif
	uvEng_ACamCali_SetGlobalMarkDiffX(dbMarkDiff12X, dbMarkDiff34X);
#endif
	return ENG_JWNS::en_next;
}

/*
 desc : Align Mark�� Trigger Board�� ���
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

	/* ���� ���� Ÿ�� (AMBER or IR)�� ����, Trigger Board�� ��ϵ� ä�� ���� */
	u8LampType	= uvEng_GetConfig()->set_comn.strobe_lamp_type;
	if (u8LampType > 0x01)	return ENG_JWNS::en_error;
#if 0
	/* �� ī�޶� ���� ��ϵ� Ʈ���� ���� */
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
 desc : Align Mark�� Trigger Board�� ��� ��ϵǾ����� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsTrigRegistedGlobal4()
{
	UINT8 u8LampType	= 0x00;		/* 0x00 : Coaxial Lamp (1 ch, 3 ch), 0x01 : Ring Lamp (2 ch, 4 ch) */
	UINT8 u8Count		= uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	UINT8 u8TrigCh[2][2]= { {1, 3}, {2, 4} }, u8TrigChNo = 0x00;		/* 0x01 ~ 0x04 */;
	UINT32 u32TrigPos	= 0;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetWorkJobName(L"Is.Trig.Regist.Global");

	/* ���� ���� Ÿ�� (AMBER or IR)�� ����, Trigger Board�� ��ϵ� ä�� ���� */
#if 1	/* ���� ��񿡼��� Ʈ���� �̺�Ʈ�� ������ ä�� 1���� ä�� 2������ �ش�� */
	u8LampType	= uvEng_GetConfig()->set_comn.strobe_lamp_type;
#endif

	/* Trigger �Ӽ� ���� ���� */
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
 desc : ����� ��ũ ������ ���� ���������� �뱤 �� ��ġ�� �̵�
 parm : None
 retn : TRUE or FALSE
*/
ENG_JWNS CWork::SetAlignMovingGlobal4()
{
	DOUBLE dbStageVeloY	= uvEng_GetConfig()->mc2_svc.mark_velo;

	if (!IsWorkRepeat())	SetWorkJobName(L"SetAlignMovingGlobal4");

	/* ���� ��ϵ� ī�޶� �� 1�� �̻� ������ �ȵ� ���, ���� ó�� */
	if (!uvCmn_Camera_IsConnected())	return ENG_JWNS::en_error;

	/* ���� Axis�� Done Toggled �� ���� */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);

	/* -------------------------------------------------------------------- */
	/* Align Mark�� ��� ���� (�˻�)�� �� �ֵ���, Y Axis ����� Stroke ���� */
	/* ������, �뱤 ���� ��ġ�� �̵��ϵ�, Stage Velocity �����ؼ� �߰� �ʿ� */
	/* -------------------------------------------------------------------- */
	/* ���� ��ġ�� �̵� ��, Mark 1�� Mark2 �� �߰� �������� �̵� */
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, m_dbAlignMoveEndY, dbStageVeloY))
	{
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : Align Mark ��ġ�� �̵� �ߴ��� Ȯ��
 ���� :	None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsAlignMovedGlobal()
{
	if (!IsWorkRepeat())	SetWorkJobName(L"IsAlignMovedGlobal");

	if (uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))	return ENG_JWNS::en_next;

	return ENG_JWNS::en_wait;
}

/*
 desc : �ѹ��� ��ũ�� ��� �ν� ��, ���������� ��ũ�� Ȯ�εǾ� �ִ��� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsSetMarkValidGlobal4()
{
	UINT8 u8Mark= uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);	/* �Ź��� ��ϵ� Global & Local Fiducial Mark ���� */

	if (!IsWorkRepeat())	SetWorkJobName(L"IsSetMarkValidGlobal4");

	/* Global Mark 4�� ��� �νĵǾ����� ���� Ȯ�� */
	if (uvEng_Camera_GetGrabbedCount() != u8Mark)	return ENG_JWNS::en_wait;
	/* �˻��� ��ũ�� ��ȿ���� Ȯ�� */
	if (!uvEng_Camera_IsGrabbedMarkValidAll(FALSE))	return ENG_JWNS::en_error;

	/* ��� Mark�� �νĵ� ������ ��� */
	return ENG_JWNS::en_wait;
}

/*
 desc : �������� �ʱ� ���� ��ġ�� �̵�
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::SetMovingUnloader()
{
	DOUBLE dbStagePosX, dbStagePosY, dbStageVeloY;
	LPG_CSAI pstAlign	= &uvEng_GetConfig()->set_align;

	if (!IsWorkRepeat())	SetWorkJobName(L"SetMovingUnloader");

	/* Stage X, Y, Camera 1 & 2 X �̵� ��ǥ ��� */
	dbStagePosX	= pstAlign->mark2_stage_x;				/* Calibration �Ź��� Mark 2 ���� ��ġ */
	dbStagePosY	= pstAlign->table_unloader_xy[0][1];	/* Stage Unloader ��ġ */
	/* �� Axis ���� �⺻ ���� �ӵ� �� ��� */
	dbStageVeloY= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_y];

	/* ��� Motor Drive�� ��� �� ���� */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);

	/* Stage Moving (Vector) (Vector Moving�ϴ� �⺻ �� ���� ����) */
	if (!uvEng_MC2_SendDevMoveVector(dbStagePosX, dbStagePosY, dbStageVeloY, m_enVecMoveAxis))
		return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : ��� ����̺갡 �� �̵� �Ǿ����� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsMovedUnloader()
{
	if (!IsWorkRepeat())	SetWorkJobName(L"IsMovedUnloader");

	/* ���� ��ġ�� ���ͷ� �̵��ϰ��� �ϴ� ��ġ�� ������ ������� */
	if (uvCmn_MC2_IsDrvDoneToggled(m_enVecMoveAxis))	return ENG_JWNS::en_next;

	return ENG_JWNS::en_wait;
}

/*
 desc : Luria Service�� ������ Align Mark ���� �� ���
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

	/* XML ���Ͽ� ��ϵ� Mark ���� Ȯ�� */
	u8Mark	= uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);

	/* Direct Expose�̸鼭, ����� ��ũ�� ���� ��� */
	if (m_enWorkJobID == ENG_BWOK::en_expo_only && 0x00 == u8Mark)
	{
	}
	else
	{
		/* ��ü ��ũ�� �˻��� ��츸 ���� */
		if (0x04 != u8Mark)	return ENG_JWNS::en_error;

		/* Global Fiducial ���� */
		for (i=0; i<u8Mark; i++)
		{
			/* Gerber�� ��ϵ� ��ũ ��ġ ���� ��� */
			if (!uvEng_Luria_GetGlobalMark(i, stMarkPos))	return ENG_JWNS::en_error;
			/* ---------------------------------------------------------------------------- */
			/* �������� A2/0x02 �ʿ� �ԷµǴ� Mark�� ������ nm (�������) �̹Ƿ� (mm -> nm) */
			/* ---------------------------------------------------------------------------- */
			stMarkReg[i].mark_x	= stMarkPos.mark_x;
			stMarkReg[i].mark_y	= stMarkPos.mark_y;
		}

		/* Align Mark�� ������ ���� ���� */
		if (m_enWorkJobID == ENG_BWOK::en_expo_mark || m_enWorkJobID == ENG_BWOK::en_expo_cali)
		{
			for (i=0; i<u8Mark; i++)
			{
				/* Global Mark < 4 > Points / Grabbed Image Index (Zero-based) */
				u8CamID	= i < 2 ? 0x01 : 0x02;
				u8ImgID	= (UINT8)ROUNDDN((i % 2), 0);
				/* Grabbed Mark images�� ��� �� �������� */
				pstGrab	= uvEng_Camera_GetGrabbedMark(u8CamID, u8ImgID);
				if (!pstGrab || !pstGrab->marked)	return ENG_JWNS::en_error;
				/* ------------------------------------------------------------------------------------------------- */
				/* Align Mark ���� ������ ���� Mark Offset �� �����ؼ� ��ġ�� �� ���� (�������� A2/0x02�� ������ nm) */
				/* ------------------------------------------------------------------------------------------------- */
				stMarkReg[i].mark_x	-= pstGrab->move_mm_x;
				stMarkReg[i].mark_y	+= pstGrab->move_mm_y;
			}
		}
		/* Align Mark�� ������ ���� ���� Camera Calibration Offset ���� */
		if (m_enWorkJobID == ENG_BWOK::en_expo_cali)
		{
		}

		/* Global Transformation Recipe ���� ���� */
		if (!uvEng_Luria_ReqSetTransformationRecipe())	return ENG_JWNS::en_error;

		/* ���� ���� �� �ʱ�ȭ */
		uvCmn_Luria_ResetRegistrationStatus();
#if 0
		/* Luria�� ����� Mark ��ġ �� ��� ���� */
		if (!uvEng_Luria_ReqSetRegistPointsAndRun(u8Mark, stMarkReg))	return ENG_JWNS::en_error;
#endif
		/* ��Ŷ ��û �ð� �ӽ� ���� */
		m_u64ReqSendTime	= GetTickCount64();

		/* Align Mark ��� ���� �� ��û */
		if (!uvEng_Luria_ReqGetGetRegistrationStatus())	return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : Align Mark ���� �� ��� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsAlignMarkRegisted()
{
	TCHAR tzStatus[128]	= {NULL};
	UINT8 u8Mark		= 0x00;
	UINT16 u16Status	= 0x0000;

	if (!IsWorkRepeat())	SetWorkJobName(L"IsAlignMarkRegisted");

	/* Direct Expose�̸鼭, ����� ��ũ�� ���� ��� */
	if (m_enWorkJobID == ENG_BWOK::en_expo_only && 0x00 == u8Mark)
	{
	}
	else
	{
		/* Registration Status ���� ���������� ��ϵǾ����� Ȯ�� */
		u16Status	= uvCmn_Luria_GetRegistrationStatus();
		if (0xffff == u16Status)
		{
			/* �ʹ� ���� ��û�ϸ� �ȵǹǷ� ... */
			if ((m_u64ReqSendTime + 250) < GetTickCount64())
			{
				/* �ֱ� ���� ��û ��� ������ �ð� ���� */
				m_u64ReqSendTime = GetTickCount64();
				/* ���� Fiducial ��� ���� �� ��û */
				if (!uvEng_Luria_ReqGetGetRegistrationStatus())	return ENG_JWNS::en_error;
			}
			return ENG_JWNS::en_wait;
		}
		/* ���� ���� �߻� */
		else if (0x01 != u16Status)
		{
			return ENG_JWNS::en_error;
		}
	}

	return ENG_JWNS::en_next;
}

/*
 desc : PrePrinting ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::SetPrePrinting()
{
	if (!IsWorkRepeat())	SetWorkJobName(L"SetPrePrinting");

#if (USE_LUIRA_REQ_PERIOD_PKT)
	/* �ֱ������� ��Ŷ �۽����� ����� �÷��� ���� */
	uvEng_Luria_SendPeriodPkt(FALSE);
#endif
	/* Luria Service���� PrePrinting ȣ�� (���������� Printing ���� �� �ʱ�ȭ) */
	if (!uvEng_Luria_ReqSetPrintOpt(0x00))	return ENG_JWNS::en_error;
	/* ��� ��û �ð� �ӽ� ���� */
	m_u64ReqSendTime	= GetTickCount64();
	/* Printing Status ���� ��û */
	if (!uvEng_Luria_ReqGetExposureState())	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : PrePrinting �Ϸ� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsPrePrinted()
{
#if 1	/* Printing �ܰ迡���� üũ �ϹǷ�, ���⼭�� ���� üũ���� �ʾƵ� �� */
	if (!IsWorkRepeat())	SetWorkJobName(L"IsPrePrinted");

	ENG_LPES enState	= (ENG_LPES)uvCmn_Luria_GetExposeState();
	/* ���� �뱤 ���°� �غ�Ǿ����� Ȯ�� */;
	if (0x0f == (UINT8(enState) & 0x0f))			return ENG_JWNS::en_error;
	/* ���� �뱤 ���� ���°� �������� ��� */
	if (ENG_LPES::en_preprint_success == enState)	return ENG_JWNS::en_next;
	/* �ٽ� ���� �뱤 ���� ���� ��û */
	if (!uvEng_Luria_ReqGetExposureState())			return ENG_JWNS::en_error;
	return ENG_JWNS::en_wait;
#else
	return ENG_JWNS::en_next;
#endif
}

/*
 desc : Printing ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::SetPrinting()
{
	if (!IsWorkRepeat())	SetWorkJobName(L"SetPrinting");

	/* ���� �ֱٿ� �뱤�� ���� (Stripe) ���� �ʱ�ȭ */
	m_u8PrintStripe	= 0x00;
	m_u64StripeTime	= GetTickCount64();	/* ���� �ֱٿ� �뱤 ������ ����� �ð� �ʱ�ȭ */

	/* Luria Service���� Printing ȣ�� (���������� Printing ���� �� �ʱ�ȭ) */
	if (!uvEng_Luria_ReqSetPrintOpt(0x01))	return ENG_JWNS::en_error;
	/* Printing Status ���� ��û (�Լ� ���ο� 1�ʿ� 4�� �ۿ� ��û���� ����) */
	if (!uvEng_Luria_ReqGetExposureState())	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : Printing �Ϸ� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsPrinted()
{
	TCHAR tzTitle[128]	= {NULL};
	ENG_LPES enState	= (ENG_LPES)uvCmn_Luria_GetExposeState();

	if (!IsWorkRepeat())	SetWorkJobName(L"IsPrinted");

	/* ���� �뱤 ���°� �غ�Ǿ����� Ȯ�� */
	if (0x0f == (UINT8(enState) & 0x0f))
	{
#if 1	/* �ٷ� ������ �߻���Ű�� ����, ���� ��� (�ᱹ Timeout �� ������ ���) */
		return ENG_JWNS::en_error;
#endif
	}
	/* ���� ���� �뱤 ���� ���̸� ... */
	if (enState == ENG_LPES::en_print_running)
	{
		/* ���� Delay Time Out �ð��� �ֱ� �ð����� ���� */
		m_u64DelayTime	= GetTickCount64();
	}
	else if (uvCmn_Luria_IsExposeStateSuccess(ENG_LCEP::en_print))
	{
		return ENG_JWNS::en_next;
	}
	/* ���� ����� �뱤 Ƚ���� �ִ��� ���ο� ���� ��ð� ���� �뱤 �۾��� �������� ���� �Ǵ� */
	if (uvCmn_Luria_GetExposePrintCount() > m_u8PrintStripe)
	{
		m_u8PrintStripe	= uvCmn_Luria_GetExposePrintCount();
		m_u64StripeTime	= GetTickCount64();
	}
	else
	{
		/* 1 Stripe �뱤�ϴµ� �ּ� 30�� �̻� ������ ���ٸ� ���� ó�� */
#if 0
		if (GetTickCount64() > m_u64StripeTime + 150000 /* 150�� */)
#else
		TRACE(L"Stripe Expose Time-out = %d\n", uvCmn_Luria_GetOneStripeExposeTime());
		if (GetTickCount64() > m_u64StripeTime + uvCmn_Luria_GetOneStripeExposeTime())
#endif
		{
			return ENG_JWNS::en_error;
		}
	}
	/* Printing Status ���� ��û (�Լ� ���ο� 1�ʿ� 2�� �ۿ� ��û���� ����) */
#if 1	/* ���� ���� ó������ ����... ��� ���·� ���� */
	if (0x00 == uvEng_Luria_ReqGetExposureState())	return ENG_JWNS::en_error;
#else
	uvEng_Luria_ReqGetExposureState();
#endif

	/* Printing �����ٰ� �ؼ�, �ٷ� �������� �̵��ϸ� ���� �߻� */
#ifdef _DEBUG
	SetWorkWaitTime(500);
#else
	SetWorkWaitTime(300);
#endif

#if (USE_LUIRA_REQ_PERIOD_PKT)
	/* �ֱ������� ��Ŷ �۽� �۾� ���� */
	uvEng_Luria_SendPeriodPkt(TRUE);
#endif
	return ENG_JWNS::en_wait;
}

/*
 desc : �뱤 �۾��� �غ� �Ǿ� �ִ��� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::SetExpoReady()
{
	ENG_JWNS enState	= ENG_JWNS::en_next;

	if (!IsWorkRepeat())	SetWorkJobName(L"SetExpoReady");

	/* ���� ��� photohead�� ���� ���� ���� ��û (�ϴ�. Z Axis�� Mid ���°� üũ �Ǿ� �ִ��� ���� */
	if (!uvEng_Luria_ReqGetMotorStateAll())				return ENG_JWNS::en_error;
	/* ��� ��� ����̺갡 ���� �������� ���� Ȯ�� (���� �ð� ���� ������ ��ٷ� ��) */
	if ( uvCmn_MC2_IsAnyDriveBusy())					return ENG_JWNS::en_wait;
	/* Gerber ��� ���� */
	if (ENG_JWNS::en_next != IsRegistGerberCheck())		return ENG_JWNS::en_error;
	/* Photohead ���� LED Power�� �����Ǿ� �ִ��� ���� */
#ifndef _DEBUG
	if (!uvCmn_Luria_IsPHSetLedPowerAll())				return ENG_JWNS::en_error;
#endif
	/* ���� Vacuum�� On ���°� �ƴ��� Ȥ�� Chiller�� On ���°� �ƴ��� ���� */
	if (!IsVacuumOnOff(0x01) || !IsChillerRunning())	return ENG_JWNS::en_error;

	return SetTrigDisabled();
}

/*
 desc : Vacuum ����/���� Ȯ��
 parm : mode	- [in]  0x01 : Run, 0x00 : Stop
 retn : TRUE or FALSE
*/
BOOL CWork::IsVacuumOnOff(UINT8 mode)
{
	UINT8 u8Read	= 0x00;

	/* ���� Vacuum ���°� Waiting ������� ���ο� ���� */
	u8Read	= uvCmn_MCQ_ReadBitsValue((UINT32)ENG_PIOA::en_vacuum_vac_atm_waiting);
	if (u8Read)			return FALSE;
	/* ���� Vacuum Status�� On or Off ���� Ȯ�� */
	u8Read	= uvCmn_MCQ_ReadBitsValue((UINT32)ENG_PIOA::en_vacuum_status);
	if (u8Read != mode)	return FALSE;

	return TRUE;
}

/*
 desc : Chiller ���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsChillerRunning()
{
	/* ĥ���� ���� ������ Ȯ�� */
	return uvCmn_MCQ_ReadBitsValue((UINT32)ENG_PIOA::en_chiller_status) == 0x01;
}

/*
 desc : ���а� �ʱ�ȭ : Homing
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::SetHomingPHAll()
{
	/* ���� ���� ���� �� �ʱ�ȭ */
	uvCmn_Luria_ResetAllPhZAxisMidPosition();

	/* �߰� ��ġ�� �̵� */
	if (!uvEng_Luria_ReqSetMotorPositionInitAll(0x01))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : ���а� �ʱ�ȭ : Homed?
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWork::IsHomedPHAll()
{
	if (!uvCmn_Luria_IsAllPhZAxisMidPosition())	return ENG_JWNS::en_wait;
	return ENG_JWNS::en_next;
}