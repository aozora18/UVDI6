
/*
 desc : �ó����� �ܰ� (Step) �Լ� ����
*/

#include "pch.h"
#include "../MainApp.h"
#include "WorkStep.h"
#include "../menu/DlgMmpm.h"
#include "../GlobalVariables.h"
#include "../mesg/DlgMesg.h"
//#include <fmt/core.h>

namespace fs = std::filesystem;

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 desc : ������
 parm : None
 retn : None
*/
CWorkStep::CWorkStep()
	: CWork()
{
	m_u8LastLoadStrip = 0;	/* ������ �ʱ�ȭ */
	m_dbLastPrintPosY = 0.0f;	/* ���� �ֱ��� Y �� ������ �뱤 ��� ��ġ �� (����: mm) */
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CWorkStep::~CWorkStep()
{
}

/*
 desc : ���� �ð� ���� �۾��� ����ϱ� ����
 parm : time	- [in]  �ּ����� ��⿡ �ʿ��� �ð� (����: msec)
 retn : None
*/
ENG_JWNS CWorkStep::SetWorkWaitTime(UINT64 time)
{
	BOOL bWorkAbort = FALSE;
#ifdef _DEBUG
	/* Debug ����� ���, ������ ��� ��� 1.5 �� ���� */
	m_u64WaitTime = GetTickCount64() + time + (UINT64)ROUNDED(time * 0.5, 0);
#else
	m_u64WaitTime = GetTickCount64() + time;
#endif
#if 0
	/* ���� ����*/
	if (m_csSyncAbortSet.Enter())
	{
		/* �۾� ��� ��û�� �Դ��� ���ο� ���� �� �������� ���� �Ǵ� */
		bWorkAbort = m_bWorkAbortSet;

		/* ���� ���� */
		m_csSyncAbortSet.Leave();
	}
#endif
	return bWorkAbort ? ENG_JWNS::en_error : ENG_JWNS::en_next;	/* Fixed. */
}

/*
 desc : Ư�� �ð����� ��Ⱑ �Ϸ�Ǿ����� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsWorkWaitTime()
{
	return	GetTickCount64() > m_u64WaitTime ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}
#if 1
/*
 desc : �� �۾� �ܰ� ���� ���� �ð� ���� ���� ��� �ð��� �ʰ� �Ǿ����� ����
 parm : back_step	- [in]  �ڷ� ������ �ϴ� �۾� �ܰ� Ƚ�� (�ּ� 1 ��)
		time_delay	- [in]  ���� ��� �ð� �� (����: msec)
		retry_cnt	- [in]  ��õ� Ƚ��
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsStepRepeatTimeout(UINT8 back_step, UINT64 time_delay, UINT8 retry_cnt)
{
	/* ���� ���� ��� �ð��� �ʰ� �Ǿ��ٸ�... �ݵ�� FALSE ��ȯ */
	if (IsWorkWaitTime() == ENG_JWNS::en_next)		return ENG_JWNS::en_error;
	if (back_step >= m_u8StepIt || back_step < 1)	return ENG_JWNS::en_error;

	/* ���� ���⼭ ��ð� ����ϰ� �ִٸ�... ó������ ���� ���� ������ �ٽ��ؾ� �� */
	if (m_u64DelayTime + time_delay < GetTickCount64())
	{
		/* ���� ��õ��ϴ� �۾� �ܰ谡 ������ ��õ��� �۾� �ܰ�� �����ϸ�, ��õ� Ƚ�� ���� ó�� */
		if (m_u8RetryStep != m_u8StepIt)
		{
			m_u8RetryStep = m_u8StepIt;
			m_u8RetryCount = 0x00;	/* ��õ� Ƚ�� �ʱ�ȭ */
		}

		/* �ִ� ���� ��� �ð����� ������ ������, �ᱹ Timeout�� �߻���*/
		if (m_u8RetryCount++ >= retry_cnt)
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"The number of repetitions of the same step has exceeded");
			return ENG_JWNS::en_error;
		}

		/* ���� �ܰ� �Լ��� ȣ���ϱ� ���� ���� �ܰ�� �̵� */
		m_u8StepIt -= back_step;
	}

	/* ���� ���� ��� �ð��� ������ �ʾ�����, ����� ���� */
	return ENG_JWNS::en_wait;
}
#endif
/*
 desc : Trigger�� Strobe ���� : Enable or Disable
 parm : enable	- [in]  TRUE: Enabled, FALSE: Disabled
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetTrigEnable(BOOL enable)
{
	if (!IsWorkRepeat())
	{
		if (enable)	SetStepName(L"Set.Trig.Enable");
		else		SetStepName(L"Set.Trig.Disable");
	}
	/* Trigger Mode (Trigger & Strobe)�� ������ Disable ��Ŵ */
	//if (!uvEng_Mvenc_ReqTriggerStrobe(enable))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : ���� Trigger�� Strobe�� Enable �������� Ȯ��
 parm : enable	- [in]  TRUE: Enabled, FALSE: Disabled
 retn : wait, error, complete or next
*/
//ENG_JWNS CWorkStep::IsTrigEnabled(BOOL enable)
//{
//	/* Trigger ���� Enable ���� */
//	//if (enable)
//	//{
//	//	if (!IsWorkRepeat())	SetStepName(L"Is.Trig.Enabled");
//	//	return uvCmn_Mvenc_IsStrobEnable() ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
//	//}
//	///* Trigger ���� Disable ���� */
//	//else
//	//{
//	//	if (!IsWorkRepeat())	SetStepName(L"Is.Trig.Disabled");
//	//	return uvCmn_Mvenc_IsStrobDisable() ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
//	//}
//
//	return ENG_JWNS::en_next;
//}
#if 1
/*
 desc : �������� �ʱ� ���� ��ġ (Unloader)�� �̵�
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetMovingUnloader()
{
	ENG_MMDI enVecAxis = ENG_MMDI::en_axis_none;
	LPG_MDSM pstShMC2 = uvEng_ShMem_GetMC2();

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.Move.Stage.Unload");

	/* Check if it is operating in demo mode */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

	/* ���������� ������ ���� �������� Ȯ�� */
	if (pstShMC2->IsDriveBusy(UINT8(ENG_MMDI::en_stage_x)) ||
		pstShMC2->IsDriveBusy(UINT8(ENG_MMDI::en_stage_y)) ||
		!pstShMC2->IsDriveCmdDone(UINT8(ENG_MMDI::en_stage_x)) ||
		!pstShMC2->IsDriveCmdDone(UINT8(ENG_MMDI::en_stage_y)) ||
		!pstShMC2->IsDriveReached(UINT8(ENG_MMDI::en_stage_x)) ||
		!pstShMC2->IsDriveReached(UINT8(ENG_MMDI::en_stage_y)))
	{
		return ENG_JWNS::en_wait;
	}

	/* ��� Motor Drive�� ��� �� ���� */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);

	/* X �� Y ������ �̵��ϰ��� �ϴ� �Ÿ��� ���� 1 um �̳��̸� ���� �̵����� �ʴ´�. */
	if (abs(pstShMC2->GetDrivePos(UINT8(ENG_MMDI::en_stage_x)) - uvEng_GetConfig()->set_align.table_unloader_xy[0][0]) < 0.001 &&
		abs(pstShMC2->GetDrivePos(UINT8(ENG_MMDI::en_stage_y)) - uvEng_GetConfig()->set_align.table_unloader_xy[0][1]) < 0.001)
	{
		m_enVectMoveDrv = ENG_MMDI::en_axis_none;
	}
	else
	{
		if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_x, uvEng_GetConfig()->set_align.table_unloader_xy[0][0]))
		{
			return ENG_JWNS::en_error;
		}
		if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_y, uvEng_GetConfig()->set_align.table_unloader_xy[0][1]))
		{
			return ENG_JWNS::en_error;
		}
#if 1 /* ���� �̵��� �� �������� ���� ���� ���� */
		/* Stage Moving (Vector) */
		if (!uvEng_MC2_SendDevMoveVectorXY(ENG_MMDI::en_stage_x, ENG_MMDI::en_stage_y,
			uvEng_GetConfig()->set_align.table_unloader_xy[0][0],
			uvEng_GetConfig()->set_align.table_unloader_xy[0][1],
			uvEng_GetConfig()->mc2_svc.max_velo[UINT8(ENG_MMDI::en_stage_x)],
			enVecAxis))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SendDevMoveVector)");
			return ENG_JWNS::en_error;
		}

		/* Vector Moving�ϴ� �⺻ �� ���� ���� */
		m_enVectMoveDrv = (enVecAxis == ENG_MMDI::en_stage_x) ? ENG_MMDI::en_stage_x : ENG_MMDI::en_stage_y;
#else
		/* X ������ �̵� */
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x,
			uvEng_GetConfig()->set_align.table_unloader_xy[0][0],
			uvEng_GetConfig()->mc2_svc.move_velo))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SetMovingUnloader()->uvEng_MC2_SendDevAbsMove (X))");
			return ENG_JWNS::en_error;
		}
		/* Y ������ �̵� */
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y,
			uvEng_GetConfig()->set_align.table_unloader_xy[0][1],
			uvEng_GetConfig()->mc2_svc.move_velo))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SetMovingUnloader()->uvEng_MC2_SendDevAbsMove (Y))");
			return ENG_JWNS::en_error;
		}
#endif
	}

	return ENG_JWNS::en_next;
}

/*
 desc : ���������� Unloader ��ġ�� �̵� �ߴ��� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsMovedUnloader()
{
	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Moved.Stage.Unload");
#if 1
	/* ���� ��ġ�� ���ͷ� �̵��ϰ��� �ϴ� ��ġ�� ������ ������� */
	if (ENG_MMDI::en_axis_none == m_enVectMoveDrv ||
		uvCmn_MC2_IsDrvDoneToggled(m_enVectMoveDrv))
	{
		//SetLedVacuumShutterOnOff(0x01, 0x01, 0x01);
		return ENG_JWNS::en_next;
	}
#else
	if (uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_x) &&
		uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))
	{
		return ENG_JWNS::en_next;
	}
#endif
	return ENG_JWNS::en_wait;
}
#else
/*
 desc : �������� �ʱ� ���� ��ġ�� �̵�
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetMovingUnloader()
{
	DOUBLE dbStagePosX, dbStagePosY, dbStageVeloY;
	LPG_CSAI pstAlign = &uvEng_GetConfig()->set_align;

	SetStepName(L"Set.Moving.Unloader");

	/* Stage X, Y, Camera 1 & 2 X �̵� ��ǥ ��� */
	dbStagePosX = pstAlign->mark2_stage_x;				/* Calibration �Ź��� Mark 2 ���� ��ġ */
	dbStagePosY = pstAlign->table_unloader_xy[0][1];	/* Stage Unloader ��ġ */
	/* �� Axis ���� �⺻ ���� �ӵ� �� ��� */
	dbStageVeloY = uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_y];

	/* ��� Motor Drive�� ��� �� ���� */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);

	/* Stage Moving (Vector) (Vector Moving�ϴ� �⺻ �� ���� ����) */
	if (!uvEng_MC2_SendDevMoveVector(dbStagePosX, dbStagePosY, dbStageVeloY, m_enVectMoveDrv))
		return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : ��� ����̺갡 �� �̵� �Ǿ����� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsMovedUnloader()
{
	if (!IsWorkRepeat())	SetStepName(L"Is.Moved.Unloader");

	/* ���� ��ġ�� ���ͷ� �̵��ϰ��� �ϴ� ��ġ�� ������ ������� */
	if (uvCmn_MC2_IsDrvDoneToggled(m_enVectMoveDrv))	return ENG_JWNS::en_next;

	return ENG_JWNS::en_wait;
}
#endif
/*
 desc : �Ź��� ��ϵǾ� �ִ��� �׸��� ��ϵ� �Ź��� Mark�� �����ϴ��� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsLoadedGerberCheck()
{
	LPG_CGTI pstTrans = &uvEng_GetConfig()->global_trans;
	LPG_LDPP pstPanel = &uvEng_ShMem_GetLuria()->panel;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Registed.Gerber.Check");

	if (!IsGerberCheck(0x02))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"No registered gerbers");
		return ENG_JWNS::en_error;
	}
	if (m_enWorkJobID == ENG_BWOK::en_mark_move ||
		m_enWorkJobID == ENG_BWOK::en_mark_test ||
		m_enWorkJobID == ENG_BWOK::en_expo_align ||
		m_enWorkJobID == ENG_BWOK::en_expo_cali)
	{
		//abh1000 Local
		if (!IsGerberMarkValidCheck())
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Gerber mark information is not valid");
			return ENG_JWNS::en_error;
		}
	}

	if (m_enWorkJobID == ENG_BWOK::en_expo_align ||
		m_enWorkJobID == ENG_BWOK::en_expo_only)
	{
		/* Rectangle Lock, Rotation, Scale �� Offset �� �����Ǿ� �ִ��� �� */
		if (!(pstPanel->global_rectangle_lock == pstTrans->use_rectangle &&
			pstPanel->global_transformation_recipe[0] == pstTrans->use_rotation_mode &&
			pstPanel->global_transformation_recipe[1] == pstTrans->use_scaling_mode &&
			pstPanel->global_transformation_recipe[2] == pstTrans->use_offset_mode))
		{
			return ENG_JWNS::en_wait;
		}
		if (!(pstPanel->global_fixed_rotation == pstTrans->rotation &&
			pstPanel->global_fixed_scaling_xy[0] == pstTrans->scaling[0] &&
			pstPanel->global_fixed_scaling_xy[1] == pstTrans->scaling[1] &&
			pstPanel->global_fixed_offset_xy[0] == pstTrans->offset[0] &&
			pstPanel->global_fixed_offset_xy[1] == pstTrans->offset[1]))
		{
			return ENG_JWNS::en_wait;
		}
	}
	return ENG_JWNS::en_next;
}

/*
 desc : ��� Motor Drive�� ���� �������� ���� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsMotorDriveStopAll()
{
	BOOL bSucc = TRUE;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Motor.Drive.Stop.All");
	/* ��� ����̺��� ������ ���� �������� ���� */
	bSucc = uvCmn_MC2_IsMotorDriveStopAll();
#if 0
	LPG_MDSM pstMC2 = uvEng_ShMem_GetMC2();
	if (!bSucc)
	{
		TCHAR tzMesg[128] = { NULL };
		swprintf_s(tzMesg, 128, L"0:busy(%d)/zero(%d), 1:busy(%d)/zero(%d), "
			L"4:busy(%d)/zero(%d), 5:busy(%d)/zero(%d)",
			pstMC2->act_data[0].flag_busy, pstMC2->act_data[0].flag_zero,
			pstMC2->act_data[1].flag_busy, pstMC2->act_data[1].flag_zero,
			pstMC2->act_data[4].flag_busy, pstMC2->act_data[4].flag_zero,
			pstMC2->act_data[5].flag_busy, pstMC2->act_data[5].flag_zero);
		LOG_DEBUG(tzMesg);
	}
#endif
	return bSucc ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : ���� Trigger Board�� ä�� ���� Trigger Position 16�� ��� �ʱ�ȭ
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetTrigPosReset()
{
	UINT8 u8TrigCh[2][2] = { {1, 3}, {2, 4} };
	UINT8 u8LampType = uvEng_GetConfig()->set_comn.strobe_lamp_type;	/* ���� ���� Ÿ�� (AMBER or IR)�� ����, Trigger Board�� ��ϵ� ä�� ���� */

	/* ���� ��ϵ� Trigger Position ���� �ʱ�ȭ �Ǿ����� Ȯ�� */
	if (uvEng_Mvenc_IsTrigPosReset(0x01, 2) &&
		uvEng_Mvenc_IsTrigPosReset(0x01, 2))	return ENG_JWNS::en_next;

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.Trig.Pos.Reset");

	/* ������ Trigger Board�� ��ϵ� �� ä�� ���� Trigger Position 16�� ��� �ʱ�ȭ ���� */
	if (!uvEng_Mvenc_ReqTriggerStrobe(FALSE))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : Align Mark ���� ���� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetAlignMeasMode()
{
	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.Trig.Pos.Reset");

	/* ���� Drive�� Done Toggled �� ���� */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	/* Area Mode�� ������ */
	if (!uvEng_MC2_SendDevLuriaMode(ENG_MMDI::en_stage_y, ENG_MTAE::en_area))
	{
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : Align Mark ���� ���� ���� �Ϸ� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsAlignMeasMode()
{
	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Align.Meas.Mode");
	/* Y �� ó�� ��ġ�� �̵� �ߴ��� ���� Ȯ�� */
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))	return ENG_JWNS::en_wait;


	uvEng_Camera_ResetGrabbedImage();
	uvEng_Camera_SetCamMode(ENG_VCCM::en_grab_mode);

	return ENG_JWNS::en_next;
}

/*
 desc : ������ ��ϵ� �Ź��� �ִ��� �Ź� ����Ʈ ��û
 parm : time	- [in]  Waiting time for a response (unit: msec)
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::GetJobLists(UINT32 time)
{
	/* ���� �۾� Step Name ���� */
	SetStepName(L"Get.Job.Lists");
#if 1
	/* ���� Job List ��� ���� */
	uvCmn_Luria_ResetRegisteredJob();
#endif
	/* Job List ��û */
	if (!uvEng_Luria_ReqGetJobList())
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetJobList)");
		return ENG_JWNS::en_error;
	}

	/* ��� �۽� �� �亯�� �� ������ ���� �ð� ��� */
	if (time)	SetWorkWaitTime(time);

	return ENG_JWNS::en_next;
}

/*
 desc : ���� ���õ� �������� �Ź��� �����ϵ��� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetJobNameSelecting()
{
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	LPG_LDJM pstJobMgt = &uvEng_ShMem_GetLuria()->jobmgt;
	TCHAR tzJobName[MAX_GERBER_NAME] = { NULL };
	CUniToChar csCnv1, csCnv2;

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.Job.Name.Selecting");

	if (!pstRecipe)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"No Job Name is currently selected");
		return ENG_JWNS::en_error;
	}

	/* �Ź� ��ü ��� ���� */
	swprintf_s(tzJobName, MAX_GERBER_NAME, L"%s\\%s",
		csCnv1.Ansi2Uni(pstRecipe->gerber_path),
		csCnv2.Ansi2Uni(pstRecipe->gerber_name));

	LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzJobName);

	/* ���� �������� �Ź� ���� ��û */
	if (!uvEng_Luria_ReqSelectedJobName(tzJobName, 0x00))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSelectedJobName)");
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : ���� ���õ� Job Name�� �����ϴ��� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsJobNameSelected()
{
	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Job.Name.Selected");

	/* ���� ���õ� Job Name�� �ִ��� ���� Ȯ�� */
	return uvCmn_Luria_IsSelectedJobName() ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : ���� ��ϵ� �Ź� ���� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetJobNameLoading()
{
	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.JobName.Loading");

	/* �۽� ���� */
	if (!uvEng_Luria_ReqSetLoadSelectedJob())
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetLoadSelectedJob)");
		return ENG_JWNS::en_error;
	}
	if (!uvEng_GetConfig()->luria_svc.use_announcement)
	{
		/* ���� ���� Ȯ�α��� ���� �ð� ������ ���ؼ� */
		SetSendCmdTime();
	}

	/* ���� �ֱ��� Job Loading Rate �� �ʱ�ȭ */
	m_u8LastLoadStrip = 0;

	return ENG_JWNS::en_next;
}

/*
 desc : �Ź� ���� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsJobNameLoaded()
{
	TCHAR tzStep[64] = { NULL };
	LPG_LDJM pstJobMgt = &uvEng_ShMem_GetLuria()->jobmgt;

	/* ���� �۾� Step Name ���� */
	swprintf_s(tzStep, 64, L"Is.Job.Name.Loaded (%u / %u)",
		pstJobMgt->job_state_strip_loaded, pstJobMgt->job_total_strip_loaded);
	SetStepName(tzStep);

	/* �Ź� ���簡 ��� �Ϸ� �Ǿ����� ���� */
	if (!pstJobMgt->IsJobNameRegistedSelectedLoaded())
	{
		if (!uvEng_GetConfig()->luria_svc.use_announcement)
		{
			/* �ֱ������� ���� ���� Ȯ���� ���� ��û */
			if (IsSendCmdTime(250))
			{
				if (!uvEng_Luria_ReqGetSelectedJobLoadState())
				{
					LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetSelectedJobLoadState)");
					return ENG_JWNS::en_error;
				}
			}
		}

		/* �뷮�� ū �Ź� ���� ���� ���, Time out�� �߻����� �ʵ��� �ϱ� ���� */
		if (pstJobMgt->job_total_strip_loaded)
		{
			if (pstJobMgt->job_state_strip_loaded > m_u8LastLoadStrip)
			{
				m_u64DelayTime = GetTickCount64();	/* Updates the current time */
				/* ���� �ֱٿ� ����� Strip ���� ���� �ӽ� ���� */
				m_u8LastLoadStrip = pstJobMgt->job_state_strip_loaded;
#if 0
				TRACE(L"Loaded = %u / %u\n",
					pstJobMgt->job_state_strip_loaded, pstJobMgt->job_total_strip_loaded);
#endif
			}
			/* When the last gerber data is loaded */
			else if (m_u8LastLoadStrip > 0 && pstJobMgt->IsLoadedGerber())	/*pstJobMgt->job_state_strip_loaded == pstJobMgt->job_total_strip_loaded*/
			{
				m_u64DelayTime = GetTickCount64();	/* Updates the current time */
				return ENG_JWNS::en_next;
			}
		}
		return ENG_JWNS::en_wait;
	}

	/* When all Jobs are loaded, finally DelayTime needs to be updated */
	m_u64DelayTime = GetTickCount64();	/* Updates the current time */

	return ENG_JWNS::en_next;
}


/*
 desc : Gerber ���� ���� ����� Mark (Global & Local)���� ��ġ�� �ش�Ǵ� ��� Ʈ���� ��ġ ��� �� ���
 parm : None
 retn : wait, error, complete or next
*/

ENG_JWNS CWorkStep::SetTrigPosCalcSaved()
{
	UINT8 u8MarkGlobal = uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	UINT8 u8MarkLocal = uvEng_Luria_GetMarkCount(ENG_AMTF::en_local);
	INT32 /*i32MarkDiffY	= 0, */i32ACamDistXY[2] = { NULL };

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.Trig.Calc.Regist");
	/* ī�޶� ������ �������� �Ÿ� Ȯ�� */
	GetACamCentDistXY(i32ACamDistXY[0], i32ACamDistXY[1]);
	/* Mark 1 (Left/Bottom)�� 3 (Right/Bottom) ��ũ�� ���� ���� �� */
	/*i32MarkDiffY	= uvEng_Luria_GetGlobalLeftRightBottomDiffY();*/
	/* Mark ������ ���� �ٸ��� ó�� */


	if (u8MarkGlobal && 0x04 != u8MarkGlobal)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"The number of global mark is not 4");
		return ENG_JWNS::en_error;
	}

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)

	const int minLocalFiducial = 4;

	//if (minLocalFiducial > u8MarkLocal ||  u8MarkLocal % 2 != 0) // �����ǵμ��϶��� 16�� ������ �θ� �ȵȴ�. �ּ� 4��, ¦���̱⸸ �ϸ� �ȴ�.
	//{
	//	LOG_ERROR(ENG_EDIC::en_uvdi15, L"The number of local mark is not acceptable");
	//	return ENG_JWNS::en_error;
	//}
#else
	if (0x00 != u8MarkLocal && 0x10 != u8MarkLocal)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"The number of local mark is not 16");
		return ENG_JWNS::en_error;
	}
#endif


	return CWork::SetTrigPosCalcSaved(i32ACamDistXY[1]/*, i32MarkDiffY*/) ?
		ENG_JWNS::en_next : ENG_JWNS::en_error;
}

/*
 desc : Align Mark�� Trigger Board�� ���
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetTrigRegistGlobal()
{
	UINT8 u8Count = 0x01;
	PINT32 p32Trig1, p32Trig2;

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.Trig.Regist.Global");
	/* �� ī�޶� ���� ��ϵ� Ʈ���� ���� */
	if (0x04 == uvEng_Luria_GetMarkCount(ENG_AMTF::en_global))	u8Count = 0x02;
	/* Led Lamp Type�� ����, Ʈ���� ����� ä�� ���� �� Ʈ���� ����� �迭 ������ ���� */
	p32Trig1 = uvEng_Trig_GetTrigger(ENG_AMTF::en_global, 0x01);
	p32Trig2 = uvEng_Trig_GetTrigger(ENG_AMTF::en_global, 0x02);

	//if (!p32Trig1 || !p32Trig2)	return ENG_JWNS::en_error;
	//if (p32Trig1[0] < 1 || p32Trig2[0] < 1)	return ENG_JWNS::en_error;
	if (!uvEng_Mvenc_ReqWriteAreaTrigPos(TRUE /* ������ : ���� */,
		0, u8Count, p32Trig1,
		0, u8Count, p32Trig2,
		ENG_TEED::en_negative, TRUE))
	{
		return ENG_JWNS::en_error;
	}

	TCHAR tzMsg[256] = { NULL };
	swprintf_s(tzMsg, 256, L"SetTrigRegistGlobal : Trig1[0] = %d Trig1[1] = %d Trig2[0] = %d Trig2[1] = %d", p32Trig1[0], p32Trig1[1], p32Trig2[0], p32Trig2[1]);
	LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);


	return ENG_JWNS::en_next;
}

/*
 desc : Align Mark�� Trigger Board�� ��� ��ϵǾ����� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsTrigRegistGlobal()
{
	UINT8 u8LampType = 0x00;		/* 0x00 : Coaxial Lamp (1 ch, 3 ch), 0x01 : Ring Lamp (2 ch, 4 ch) */
	UINT8 u8Count = uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	UINT8 u8TrigCh[2][2] = { {1, 3}, {2, 4} }, u8TrigChNo = 0x00;		/* 0x01 ~ 0x04 */;
	UINT32 u32TrigPos = 0;
	PINT32 p32Trig1, p32Trig2;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Trig.Regist.Global");

	/* ���� ���� Ÿ�� (AMBER or IR)�� ����, Trigger Board�� ��ϵ� ä�� ���� */
#if 1	/* ���� ��񿡼��� Ʈ���� �̺�Ʈ�� ������ ä�� 1���� ä�� 2������ �ش�� */
	LPG_RAAF pstRecipeAlign = uvEng_Mark_GetSelectAlignRecipe();
	uvEng_GetConfig()->set_comn.strobe_lamp_type = pstRecipeAlign->lamp_type;
	uvEng_SaveConfig();

	u8LampType = uvEng_GetConfig()->set_comn.strobe_lamp_type;
#endif

	/* Led Lamp Type�� ����, Ʈ���� ����� ä�� ���� �� Ʈ���� ����� �迭 ������ ���� */
	p32Trig1 = uvEng_Trig_GetTrigger(ENG_AMTF::en_global, 0x01);
	p32Trig2 = uvEng_Trig_GetTrigger(ENG_AMTF::en_global, 0x02);
	/* Trigger �Ӽ� ���� ���� */
	u8TrigChNo = u8TrigCh[u8LampType][0];
	/* Channel 1 */
	//if (!uvEng_Mvenc_IsTrigPosEqual(0x01, 0x00, p32Trig1[0]))
	//{
	//	return ENG_JWNS::en_wait;
	//}
	//if (u8Count == 0x04 && !uvEng_Mvenc_IsTrigPosEqual(0x01, 0x01, p32Trig1[1]))
	//{
	//	return ENG_JWNS::en_wait;
	//}
	///* Channel 3 */
	//u8TrigChNo	= u8TrigCh[u8LampType][1];
	//if (!uvEng_Mvenc_IsTrigPosEqual(0x02, 0, p32Trig2[0]))
	//{
	//	return ENG_JWNS::en_wait;
	//}
	//if (u8Count == 0x04 && !uvEng_Mvenc_IsTrigPosEqual(0x02, 1, p32Trig2[1]))
	//{
	//	return ENG_JWNS::en_wait;
	//}

	return ENG_JWNS::en_next;
}

/*
 desc : Align Mark�� Trigger Board�� ���
 parm : scan	- [in]  Scan Number (0 or 1)
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetTrigRegistLocal(UINT8 scan)
{
	UINT8 u8LampType = 0x00;		/* 0x00 : AMBER(1 ch, 3 ch), 0x01 : IR (2 ch, 4 ch) */
	UINT8 u8Index = 0x00, u8Count = 0x00;
	BOOL bDirect = (scan % 2) == 0 ? TRUE /* ������ (����) */ : FALSE /* ������ (����) */;
	PINT32 p32Trig1, p32Trig2;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())
	{
		if (0x00 == scan)	SetStepName(L"Set.Trig.Regist.Local (1)");
		else				SetStepName(L"Set.Trig.Regist.Local (2)");
	}
	/* ���� ���� Ÿ�� (AMBER or IR)�� ����, Trigger Board�� ��ϵ� ä�� ���� */
	u8LampType = uvEng_GetConfig()->set_comn.strobe_lamp_type;

	/* Led Lamp Type�� ����, Ʈ���� ����� ä�� ���� �� Ʈ���� ����� �迭 ������ ���� */
	p32Trig1 = uvEng_Trig_GetTrigger(ENG_AMTF::en_local, 0x01);
	p32Trig2 = uvEng_Trig_GetTrigger(ENG_AMTF::en_local, 0x02);
	if (!p32Trig1 || !p32Trig2)	return ENG_JWNS::en_error;

	/* 1 Scan�� ��, �˻��ؾ� �� Mark ���� ��� */
	u8Count = uvEng_Luria_GetLocalMarkCountPerScan();
	/* Ʈ���Ÿ� ����ϰ��� �ϴ� ���� ��ġ ��� */
	u8Index = u8Count * scan;
	p32Trig1 += UINT32(u8Index);
	p32Trig2 += UINT32(u8Index);

	ENG_TEED teed;

	if (scan % 2 == 0x00)
	{
		teed = ENG_TEED::en_positive;
	}
	else
	{
		teed = ENG_TEED::en_negative;
	}
	/* Trigger �߻� ��ġ ��� */
	if (!uvEng_Mvenc_ReqWriteAreaTrigPos(bDirect,
		0x00, u8Count, p32Trig1,
		0x00, u8Count, p32Trig2,
		teed, TRUE))
	{
		return ENG_JWNS::en_error;
	}


	TCHAR tzMsg[256] = { NULL };
	for (int i = 0; i < u8Count; i++)
	{
		swprintf_s(tzMsg, 256, L"SetTrigRegistLocal %d: Trig1[%d] = %d Trig2[%d] = %d", scan, i, p32Trig1[i], i, p32Trig1[i]);
		LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
	}


	return ENG_JWNS::en_next;
}

/*
 desc : Align Mark�� Trigger Board�� ��� ��ϵǾ����� Ȯ��
 parm : scan	- [in]  Scan Number (0 or 1)
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsTrigRegistLocal(UINT8 scan)
{
	UINT8 i = 0, u8Index = 0x00, u8Count = 0x00, u8LampType = 0x00;		/* 0x00 : AMBER(1 ch, 3 ch), 0x01 : IR (2 ch, 4 ch) */
	UINT8 u8TrigCh[2][2] = { {1, 3}, {2, 4} }/*, u8ChNo = 0x00*/;		/* 0x01 ~ 0x04 */;
	TCHAR tzWait[64] = { NULL };
	PINT32 p32Trig1, p32Trig2;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())
	{
		if (0x00 == scan)	SetStepName(L"Is.Trig.Regist.Local (1)");
		else				SetStepName(L"Is.Trig.Regist.Local (2)");
	}
	/* ���� ����� Ʈ���� ��ġ���� ���� Ȯ��. ������ �����ϸ� üũ���� ����*/
	/* ���� ���� Ÿ�� (AMBER or IR)�� ����, Trigger Board�� ��ϵ� ä�� ���� */
	u8LampType = uvEng_GetConfig()->set_comn.strobe_lamp_type;
	/* Led Lamp Type�� ����, Ʈ���� ����� ä�� ���� �� Ʈ���� ����� �迭 ������ ���� */
	p32Trig1 = uvEng_Trig_GetTrigger(ENG_AMTF::en_local, 0x01);
	p32Trig2 = uvEng_Trig_GetTrigger(ENG_AMTF::en_local, 0x02);
	if (!p32Trig1 || !p32Trig2)	return ENG_JWNS::en_error;

	///* 1 Scan�� ��, �˻��ؾ� �� Mark ���� ��� */
	//u8Count		= uvEng_Luria_GetLocalMarkCountPerScan();
	///* Ʈ���Ÿ� �˻��ϰ��� �ϴ� ���� ��ġ ��� */
	//u8Index		= u8Count * scan;
	//p32Trig1	+= UINT32(u8Index);
	//p32Trig2	+= UINT32(u8Index);
	///* 1 Scan�� ��, ����Ǵ� Mark ���� (�� ī�޶� ��) */
	//for (i=0x00; i<uvEng_Luria_GetLocalMarkCountPerScan(); i++)
	//{
	//	/* Channel 1 */
	//	if (!uvEng_Mvenc_IsTrigPosEqual(0x01, i, p32Trig1[i]))	return ENG_JWNS::en_wait;
	//	/* Channel 3 */
	//	if (!uvEng_Mvenc_IsTrigPosEqual(0x02, i, p32Trig2[i]))	return ENG_JWNS::en_wait;
	//}

	return ENG_JWNS::en_next;
}

BOOL CWorkStep::MoveAxis(ENG_MMDI axis, bool absolute, double pos, bool waiting)
{
	double curr = uvCmn_MC2_GetDrvAbsPos(axis);

	double dest = absolute ? pos : curr + pos;

	if (uvCmn_MC2_IsDriveError(axis) || uvCmn_MC2_IsMotorDriveStopAll() == false ||
		CInterLockManager::GetInstance()->CheckMoveInterlock(axis, dest))
		return false;

	uvCmn_MC2_GetDrvDoneToggled(axis);

	BOOL res = uvEng_MC2_SendDevAbsMove(axis, dest, uvEng_GetConfig()->mc2_svc.step_velo);

	if (waiting && res)
		res = GlobalVariables::GetInstance()->Waiter([&]()->bool
			{
				return uvCmn_MC2_IsDrvDoneToggled(axis);
			}, 30 * 1000);

	return res;
}


bool CWorkStep::MoveCamToSafetypos(ENG_MMDI callbackAxis, double pos)
{
	/* ���� �۾� Step Name ���� */
	SetStepName(L"Move Cam Safe Pos");


	bool doneToggleCam[2];

	LPG_CIEA pstCfg = uvEng_GetConfig();

	// ������ ���ÿ�
	auto res1 = false, res2 = false;

	res1 = MoveAxis(ENG_MMDI::en_align_cam1, true, pstCfg->set_cams.safety_pos[0], false);
	res2 = MoveAxis(ENG_MMDI::en_align_cam2, true, pstCfg->set_cams.safety_pos[1], false);

	if (res1)
		res1 = GlobalVariables::GetInstance()->Waiter([&]()->bool
			{
				return uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1);
			}, 30 * 1000);

	if (res2)
		res2 = GlobalVariables::GetInstance()->Waiter([&]()->bool
			{
				return uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam2);
			}, 30 * 1000);

	if (res1 == res2 && res1 == true && callbackAxis != ENG_MMDI::en_axis_none)
	{
		return MoveAxis(callbackAxis, true, pos, true);
	}
	else res1 == res2 && res1 == true;
}

/*
 desc : ����� ��ũ ������ ���� ���������� Unloader ��
		Align Camera 1 / 2�� ���� Mark 1 / 3 ���� ��ġ�� �̵�
 parm : None
 retn : TRUE or FALSE
*/
ENG_JWNS CWorkStep::SetAlignMovingInit()
{
	UINT8 u8BaseXY = 0x00, u8Mark = uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	UINT16 u16MarkType = 0x0000;
	DOUBLE /*dbACamDistX, dbACamDistY, dbDiffMark, */dbACamVeloX, dbStageVeloY;
	DOUBLE dbStagePosX, dbStagePosY, dbACam1PosX;
	LPG_CSAI pstAlign = &uvEng_GetConfig()->set_align;

	SetStepName(L"Set.Align.Moving.Init (Global)");

	/* Stage X, Y, Camera 1 & 2 X �̵� ��ǥ ��� */
	dbStagePosX = pstAlign->mark2_stage_x;				/* Calibration �Ź��� Mark 2 ���� ��ġ */
	dbStagePosY = pstAlign->table_unloader_xy[0][1];	/* Stage Unloader ��ġ */

	/* Mark 4 ���� �ƴ� ���, �������� ���� */
	if (u8Mark != 4)
	{
		SetStepName(L"Set.Align.Moving.Init:No marks");
		return ENG_JWNS::en_error;
	}

	/* -------------------------------------------------- */
	/* ��ũ 2�� Ȥ�� 4�� �� ���, ����ؼ� ��ġ ���� ��� */
	/* -------------------------------------------------- */
	m_dbPosACam[0] = GetACamMark2MotionX(2);	/* ����� Mark 2 (Left/Bottom)���� �ش�Ǵ� ī�޶� 1���� X �� ���� ��� ��ġ */
	m_dbPosACam[1] = GetACamMark2MotionX(4);	/* Camera 1��� Camera2 ���� �������� ���� */

	/* �� Axis ���� �⺻ ���� �ӵ� �� ��� */
	dbStageVeloY = uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_y];
	dbACamVeloX = uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_align_cam1];

	/* ��� Motor Drive�� ��� �� ���� */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam2);

	/*�뱤 ���� ��ġ�� �̵��� ���Ͷ� �˻�*/
	if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_x, dbStagePosX))
	{
		return ENG_JWNS::en_error;
	}
	if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_y, dbStagePosY))
	{
		return ENG_JWNS::en_error;
	}

	/* Stage Moving (Vector) (Vector Moving�ϴ� �⺻ �� ���� ����) */
	if (!uvEng_MC2_SendDevMoveVectorXY(ENG_MMDI::en_stage_x, ENG_MMDI::en_stage_y,
		dbStagePosX, dbStagePosY, dbStageVeloY, m_enVectMoveDrv))
		return ENG_JWNS::en_error;
	/* ���� ī�޶� 1 ���� ���� ��ġ ���� ���, �̵��ϰ��� �ϴ� ���� ��ġ ���� ū ������ Ȥ�� */
	/* ���� ������ ���ο� ����, Align Camera 1 Ȥ�� 2 �� �� ��� ���� ���� �̵��ؾ� ���� ����*/
	dbACam1PosX = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam1);
	if (m_dbPosACam[0] > dbACam1PosX)
	{
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2, m_dbPosACam[1], dbACamVeloX))
			return ENG_JWNS::en_error;
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, m_dbPosACam[0], dbACamVeloX))
			return ENG_JWNS::en_error;
	}
	else
	{
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, m_dbPosACam[0], dbACamVeloX))
			return ENG_JWNS::en_error;
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2, m_dbPosACam[1], dbACamVeloX))
			return ENG_JWNS::en_error;
	}

	TCHAR tzMsg[256] = { NULL };
	swprintf_s(tzMsg, 256, L"SetAlignMovingInit : StageX = %.4f StageY = %.4f Cam1X = %.4f Cam2X = %.4f", dbStagePosX, dbStagePosY, m_dbPosACam[0], m_dbPosACam[1]);
	LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);



	return ENG_JWNS::en_next;
}

/*
 desc : Align Mark ��ġ�� �̵� �ߴ��� Ȯ��
 ���� :	None
 retn : wait, error, complete or next
*/

ENG_JWNS CWorkStep::IsAlignMovedInit(function<bool()> callback)
{
	if (!IsWorkRepeat())	SetStepName(L"Is.Align.Moved.Init");

	if (ENG_MMDI::en_axis_none == m_enVectMoveDrv)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to moving the vector (Unknown Vector Drive)");
		return ENG_JWNS::en_error;
	}

	/* Stage X / Y, Align Camera 1 & 2�� X �̵��� ��� �Ϸ� �Ǿ����� ���� */
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam2))	return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1))	return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(m_enVectMoveDrv))			return ENG_JWNS::en_wait;

	/*LDS ���� ��*/
	if (uvEng_GetConfig()->measure_flat.u8UseThickCheck)
	{
		/*LDS ���� ��*/
		uvEng_GetConfig()->measure_flat.bThieckOnOff = TRUE;
		uvEng_GetConfig()->measure_flat.MeasurePoolClear();
	}

	if (callback != nullptr)
	{
		return callback() ? ENG_JWNS::en_next : ENG_JWNS::en_error;
	}
	return ENG_JWNS::en_next;
}

/*
 desc : ����� ��ũ ������ ���� ���������� �뱤 �� ��ġ�� �̵�
 parm : None
 retn : TRUE or FALSE
*/
ENG_JWNS CWorkStep::SetAlignMovingGlobal()
{
	DOUBLE dbStageVelo = uvEng_GetConfig()->mc2_svc.mark_velo;

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.Align.Moving.Global");

	/* ���������� �̵� */
	uvEng_Camera_SetMoveStateDirect(TRUE);

	/* ���� Axis�� Done Toggled �� ���� */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	/* ��ũ �˻��� ���� Y �� �̵� �� ������ �뱤 ���� ��ġ���� �ϸ�, �ٷ� �뱤�� �� �ֱ� ������ */

	if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_y, m_dbAlignStageY))
	{
		return ENG_JWNS::en_error;
	}

	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, m_dbAlignStageY, dbStageVelo))
		return ENG_JWNS::en_error;

	TCHAR tzMsg[256] = { NULL };
	swprintf_s(tzMsg, 256, L"SetAlignMovingGlobal StageY = %.4f Y", m_dbAlignStageY);
	LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);



	return ENG_JWNS::en_next;
}

/*
 desc : Align Mark ��ġ�� �̵� �ߴ��� Ȯ��
 ���� :	None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsAlignMovedGlobal()
{
	if (!IsWorkRepeat())	SetStepName(L"Is.Align.Moved.Global");

	if (uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))
	{
		auto& measureFlat = uvEng_GetConfig()->measure_flat;
		auto mean = measureFlat.GetThickMeasureMean();

		/*LDS ������� ���� �Ѳ� �����Ͽ� ���������� ����� ���� ó���Ͽ� �۾� ����*/
		if (measureFlat.u8UseThickCheck)
		{
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
			LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();

			DOUBLE dLDSZPOS = uvEng_GetConfig()->acam_spec.acam_z_focus[1];
			DOUBLE dmater = pstRecipe->material_thick / 1000.0f;

			DOUBLE LDSToThickOffset = 0;
			//LDSToThickOffset = 1.3;
			LDSToThickOffset = uvEng_GetConfig()->measure_flat.dOffsetZPOS;

			/*���� ���� LDS �������� ��� �ɼ°� �߰� �Ͽ� ���� ���� ������ ���*/
			//DOUBLE RealThick = LDSToThickOffset - LDSMeasure;
			//DOUBLE RealThick = LDSToThickOffset - mean + dmater;
			/*����β� 0mm ��ġ CameraZ ���� �� LDS �ʱ�ȭ �׷��� �������� ������*/
			DOUBLE RealThick = mean + dmater + LDSToThickOffset;
			DOUBLE LimitZPos = uvEng_GetConfig()->measure_flat.dLimitZPOS;
			DOUBLE MaxZPos = uvEng_GetConfig()->measure_flat.dLimitZPOS;
			DOUBLE MinZPos = uvEng_GetConfig()->measure_flat.dLimitZPOS * -1;

			TCHAR tzMsg[256] = { NULL };
			swprintf_s(tzMsg, 256, L"Real Thick :%.3f > Material Thick : %.3f + Limit : %.3f", RealThick, dmater, LimitZPos);
			LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);

			//swprintf_s(tzMsg, 256, L"Real Thick :%.3f  Material Thick : %.3f + GetThickMeasureMean : %.3f", RealThick, dmater, mean);
			//LOG_WARN(ENG_EDIC::en_acam_cali, tzMsg);

			/*LDS���� ������ ���� �ɼ°� ���Ѱ��� Limit ����*/
			if ((RealThick > (dmater + MaxZPos)) || (RealThick < (dmater + MinZPos)))
			{
				swprintf_s(tzMsg, 256, L"Failed to actual material thickness tolerance range\n [Real Thick :%.3f > Material Thick : %.3f + Limit : %.3f]", RealThick, dmater, LimitZPos);
				//CDlgMesg dlgMesg;
				//if (IDOK != dlgMesg.MyDoModal(tzMsg, 0x01))
				LOG_ERROR(ENG_EDIC::en_uvdi15, tzMsg);

				/*���� ���� ���� �߻� Ȯ��*/
				GlobalVariables::GetInstance()->GetAlignMotion().markParams.workErrorType = ENG_WETE::en_lds_thick_check;
				return ENG_JWNS::en_error;
			}

#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
			/*���� ���� LDS �������� ��� �ɼ°� �߰� �Ͽ� ���� ���� ������ ���*/
			DOUBLE RealThick = mean + uvEng_GetConfig()->measure_flat.dOffsetZPOS;
			DOUBLE LimitZPos = uvEng_GetConfig()->measure_flat.dLimitZPOS;

			TCHAR tzMsg[256] = { NULL };
			swprintf_s(tzMsg, 256, L"CheckThick LDS Measure = %.4f", mean);
			LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);

			/*LDS���� ������ ���� �ɼ°� ���Ѱ��� Limit ����*/
			//if (RealThick > LimitZPos)
			//{
			//	swprintf_s(tzMsg, 256, L"Failed to actual material thickness tolerance range\n [Real Thick :%.3f > LimitZ Pos : %.3f]", RealThick, LimitZPos);

			//	CDlgMesg dlgMesg;
			//	if (IDOK != dlgMesg.MyDoModal(tzMsg, 0x01))

			//		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMsg);
			//	return ENG_JWNS::en_error;
			//}
#endif


		}

		else
		{
			/*�� ����Ŭ�� ��� ���ٸ� LDS Check ��� On���� ����*/
			if (measureFlat.bOnePass)
			{
				uvEng_GetConfig()->measure_flat.bOnePass = FALSE;
				uvEng_GetConfig()->measure_flat.u8UseThickCheck = TRUE;
				uvEng_SaveConfig();
			}
		}

		return ENG_JWNS::en_next;
	}

	return ENG_JWNS::en_wait;
}

/*
 desc : �������� ����� ��ġ�� �̵�
 parm : mode	- [in]  0x00 - Init, 0x01 - Moving
		scan	- [in]  Align Scan�Ϸ��� ��ġ (��ȣ. 0 or 1)
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetAlignMovingLocal(UINT8 mode, UINT8 scan)
{

	//AlignMotionMode : UINT8
	//{
	//	toInitialMoving = 0, //���� ��ĵ�� ���� �˻� �ʱ���ġ�� �̵��ϴ� ����
	//	toScanMoving = 1,    //��ĵ�� ���� �̵��ϴ� ����
	//};

	const int WANGBOKCOUNT = 2;

	UINT8 u8MarkLeft, u8MarkRight;
	BOOL bStageMoveDirect = (scan % WANGBOKCOUNT == 0x00) ? FALSE /* ������ */ : TRUE /* ������ */;

	DOUBLE dbACamDistXY[2], dbAlignStageY;
	DOUBLE dbMarkDist, dbDiffMarkX, dbStageVelo, dbACamVelo;
	STG_XMXY stPoint = { STG_XMXY(), };
	LPG_CSAI pstSetAlign = &uvEng_GetConfig()->set_align;
	LPG_CMSI pstMC2Svc = &uvEng_GetConfig()->mc2_svc;
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	// by sysandj : ��������(����)
	LPG_MACP pstThickCali = uvEng_ThickCali_GetRecipe(pstRecipe->cali_thick);

	/* ���� �۾� Step Name ���� */
	if (0x00 == mode)
	{
		if (0x00 == scan)	SetStepName(L"Set.Align.Moving.Local (Init:1)");
		else				SetStepName(L"Set.Align.Moving.Local (Init:2)");
	}
	else
	{
		if (0x00 == scan)
			SetStepName(L"Set.Align.Moving.Local (Move:1)");
		else
			SetStepName(L"Set.Moving.Align.Local (Move:2)");
		/* �������� �̵� ���� ���� */

	}

	uvEng_Camera_SetMoveStateDirect(bStageMoveDirect);
	/* ī�޶� ������ �������� �Ÿ� Ȯ�� */
	GetACamCentDistXY(dbACamDistXY[0], dbACamDistXY[1]);

	/* Align Mark ���� �ʱ� ��ġ�� �̵� ����� ��� */
	if (0x00 == mode)
	{
		if (!GetLocalLeftRightBottomMarkIndex(scan, u8MarkLeft, u8MarkRight))
			return ENG_JWNS::en_error;
		/* Left�� Right ��ũ ���� ���� (����) ��� */
		dbMarkDist = uvEng_Luria_GetLocalMarkACam12DistX(0x00, scan);		/* mm */
		if (!uvEng_Luria_GetLocalMark(u8MarkLeft, &stPoint))
			return ENG_JWNS::en_error;
		dbDiffMarkX = stPoint.mark_x - pstSetAlign->mark2_org_gerb_xy[0];	/* mm */
#if 0
		TRACE(L"dbDiffMarkX = %d\n", dbDiffMarkX);
#endif
		/* ȯ�� ���Ͽ� ������ ���� Mark �߽��� ��, Align Camera ��ġ ���� ���� ���� �ջ� (���ϰų� ���ų�) */
		m_dbPosACam[0] = pstThickCali->mark2_acam_x[0] + dbDiffMarkX;	/* 100 nm or 0.1 um */
		/* Camera 1��� Camera2 ���� �������� ���� */
		m_dbPosACam[1] = m_dbPosACam[0] + dbMarkDist - dbACamDistXY[0];
		/* Camera 1 & 2�� �� �̵� �ӵ� */
		dbACamVelo = pstMC2Svc->max_velo[UINT8(ENG_MMDI::en_align_cam1)];
	}
	else
	{
		/* Stage Y �� �̵� �ӵ� */
		dbStageVelo = pstMC2Svc->mark_velo;
	}

	/* Align Mark ���� �ʱ� ��ġ�� �̵� ����� ��� */
	if (0x00 == mode)
	{
		/* ���� Done Toggled �� ���� */
		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam2);
		/* Align Camera 1 & 2 �� �̵� (�ݵ�� 2�� ī�޶���� �̵��ϱ� ����) */
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2, m_dbPosACam[1], dbACamVelo))
			return ENG_JWNS::en_error;
		/* Mark Type�� Shared���� ���ο� ���� 1�� ī�޶� �̵� ���� ���� */
		// by sysandj : �Լ�����(����)
		if (0x00 == scan || !uvEng_Recipe_IsRecipeSharedType())
		{
			uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
			if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, m_dbPosACam[0], dbACamVelo))
				return ENG_JWNS::en_error;
		}
	}
	/* Align Mark ������ ���� Y ������ �̵� */
	else
	{
		//uvEng_Mvenc_ReqTriggerStrobe(FALSE);
		//uvEng_Mvenc_ReqEncoderOutReset();
		//uvEng_Mvenc_ResetTrigPosAll();



		if (bStageMoveDirect == false)	/* ������ Y �� �̵��� ��, ���߰��� �ϴ� ��ġ �� */
			dbAlignStageY = pstSetAlign->table_unloader_xy[0][1];
		else				/* ������ Y �� �̵� */
			dbAlignStageY = uvCmn_Luria_GetStartY(ENG_MDMD::en_pos_expo_start);

		/* ���� Done Toggled �� ���� */
		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
		/* Stage Y �� �̵� */
		if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_y, dbAlignStageY))
		{
			return ENG_JWNS::en_error;
		}
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dbAlignStageY, dbStageVelo))
		{
			return ENG_JWNS::en_error;
		}
	}

	return ENG_JWNS::en_next;
}

/*
 desc : Align Mark ��ġ�� �̵� �ߴ��� Ȯ��
 ���� :	scan	- [in]  Align Scan�Ϸ��� ��ġ (��ȣ. 0 or 1)
		mode	- [in]  0x00 - Init, 0x01 - Moving
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsAlignMovedLocal(UINT8 mode, UINT8 scan)
{
	TCHAR tzStep[2][64] = { L"Is.Align.Moved.Local.Init", L"Is.Align.Moved.Local.Check" };
	BOOL bSucc = FALSE;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(tzStep[mode]);

	/* Align Mark ���� �ʱ� ��ġ�� �̵� ����� ��� */
	if (0x00 == mode)
	{
		/* Check to Done Toggled */
		bSucc = uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam2);
		/* Mark Type�� Shared���� ���ο� ���� 1�� ī�޶� �̵� ���� ���� */
		// by sysandj : �Լ�����(����)
		if (bSucc && (0x00 == scan || !uvEng_Recipe_IsRecipeSharedType()))
		{
			bSucc = uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1);
		}
		return bSucc ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
	}
	/* Align Mark ������ ���� Y ������ �̵� */
	else
	{
		bSucc = uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y);
		return bSucc ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
	}
}

/*
 desc : ������ ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetFinish()
{
	ENG_JWNS enState = ENG_JWNS::en_next;

	if (ENG_JWNS::en_next == enState)	enState = SetTrigEnable(FALSE);
	if (ENG_JWNS::en_next == enState)	enState = SetMovingUnloader();

	return enState;
}

/*
 desc : ������ Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsFinish()
{
	ENG_JWNS enState = ENG_JWNS::en_next;

	if (ENG_JWNS::en_next == enState)	enState = IsTrigEnabled(FALSE);
	if (ENG_JWNS::en_next == enState)	enState = IsMovedUnloader();

	return enState;
}

/*
 desc : Vacuum�� Shutter ���� - (LED Off, Vacuum On, Shutter Closed)
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetLedVacuumShutterIn()
{
	return SetLedVacuumShutterOnOff(0x00, 0x01, 0x00);
}

/*
 desc : Vacuum�� Shutter ���� - (LED On, Vacuum Off, Shutter Open)
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetLedVacuumShutterOut()
{
	return SetLedVacuumShutterOnOff(0x01, 0x00, 0x01);
}

/*
 desc : Vacuum�� Shutter ����
 parm : vacuum	- [in]  0x01: On, 0x00: Off, 0x02: ���� ����
		shutter	- [in]  0x01: Open, 0x00: Close, 0x02: ���� ����
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetVacuumShutterOnOff(UINT8 vacuum, UINT8 shutter)
{
	TCHAR tzTilte[128] = { NULL }, tzVac[2][8] = { L"Off", L"On" }, tzShu[2][8] = { L"Close", L"Open" };
	/* ����, Vaccum ���¸� ���� ���� �״�� �����ϴ� ��������� ���� ... */
	if (vacuum == 0x02)		vacuum = 0;// by sysandj : MCQ��ü �߰� �ʿ� : uvEng_ShMem_GetPLCExt()->r_vacuum_status;
	/* ����, Shutter ���¸� ���� ���� �״�� �����ϴ� ��������� ���� ... */
	if (shutter == 0x02)	shutter = 0;// by sysandj : MCQ��ü �߰� �ʿ� : uvEng_ShMem_GetPLCExt()->r_shutter_status;
#ifdef _DEBUG
	vacuum = 0;// by sysandj : MCQ��ü �߰� �ʿ�:uvEng_ShMem_GetPLCExt()->r_vacuum_status;	/* ���� ���� ���� */
#endif

#if 0
	/* ���� Shutter�� Vacuum�� Open (On) or Close (Off) ���°� �ƴ� ��� */
	if (ENG_JWNS::en_wait == IsShutterVacuumWait())
	{
		SetStepName(L"Shutter && Vacuum Waiting");
		return ENG_JWNS::en_wait;
	}
	if (!IsWorkRepeat())
#endif
	{
		swprintf_s(tzTilte, 128, L"Set.Vacuum(%s).Shutter(%s)", tzVac[vacuum], tzShu[shutter]);
		SetStepName(tzTilte);
	}

	/* Vacuum�� Shutter�� ���� ���� ���� */
	// by sysandj : MCQ��ü �߰� �ʿ� : if (!uvEng_MCQ_SetVacuumShutterOnOff(vacuum, shutter))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : Start Lamp / Vacuum / Shutter ����
 parm : start_led	- [in]  0x01: On, 0x00: Off
		vacuum		- [in]  0x01: On, 0x00: Off
		shutter		- [in]  0x01: Open, 0x00: Close
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetLedVacuumShutterOnOff(UINT8 start_led, UINT8 vacuum, UINT8 shutter)
{
	TCHAR tzTilte[128] = { NULL }, tzVac[2][8] = { L"Off", L"On" }, tzShu[2][8] = { L"Close", L"Open" };
	UINT8 u8StartLed = 0;// by sysandj : MCQ��ü �߰� �ʿ�:((LPG_PMRW)uvEng_ShMem_GetPLC()->data)->r_start_button;

	/* ���� Shutter�� Vacuum�� Open (On) or Close (Off) ���°� �ƴ� ��� */
	if (ENG_JWNS::en_wait == IsShutterVacuumWait())
	{
		SetStepName(L"Shutter && Vacuum Waiting");
		return ENG_JWNS::en_wait;
	}

	/* ���� �۾� Step Name ���� */
	swprintf_s(tzTilte, 128, L"Led(%s).Vacuum(%s).Shutter(%s)",
		tzVac[start_led], tzVac[vacuum], tzShu[shutter]);
	SetStepName(tzTilte);
#if 1
	vacuum = 0;// by sysandj : MCQ��ü �߰� �ʿ� : uvEng_ShMem_GetPLCExt()->r_vacuum_status;	/* ���� ���� ���� */
#endif
	/* Vacuum�� Shutter�� ���� ���� ���� */
	// by sysandj : MCQ��ü �߰� �ʿ�
// 	if (!uvEng_MCQ_LedVacuumShutter(u8StartLed, vacuum, shutter))
// 	{
// 		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (LedVacuumShutter)");
// 		return ENG_JWNS::en_error;
// 	}

	return ENG_JWNS::en_next;
}

/*
 desc : Start Lamp / Shutter ���� (Vacuum ����)
 parm : start_led	- [in]  0x01: On, 0x00: Off
		shutter		- [in]  0x01: Open, 0x00: Close
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetLedShutterOnOff(UINT8 start_led, UINT8 shutter)
{
	UINT8 u8Vacuum = 0x00;
	TCHAR tzTilte[128] = { NULL }, tzVac[2][8] = { L"Off", L"On" }, tzShu[2][8] = { L"Close", L"Open" };
#if 0
	/* ���� Shutter�� Vacuum�� Open (On) or Close (Off) ���°� �ƴ� ��� */
	if (ENG_JWNS::en_wait == IsShutterVacuumWait())
	{
		SetStepName(L"Shutter && Vacuum Waiting");
		return ENG_JWNS::en_wait;
	}
#endif
	/* ���� �۾� Step Name ���� */
	swprintf_s(tzTilte, 128, L"Led(%s).Shutter(%s)", tzVac[start_led], tzShu[shutter]);
	SetStepName(tzTilte);
	/* �ڵ� Vacuum ���� ��尡 �ƴ� ���, Vacuum�� ���� ���� �״�� ���� */
	u8Vacuum = 0;// by sysandj : MCQ��ü �߰� �ʿ�: uvEng_ShMem_GetPLCExt()->r_vacuum_status;

	/* Vacuum�� Shutter�� ���� ���� ���� */
	// by sysandj : MCQ��ü �߰� �ʿ� : if (!uvEng_MCQ_LedVacuumShutter(start_led, u8Vacuum, shutter))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : Vacuum�� ���� Wait �������� ����
 parm : None
 retn : wait, next
*/
ENG_JWNS CWorkStep::IsVacuumWait()
{
	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Vacuum.Wait");
	/* ���� Vacuum ���°� Waiting ������� ���ο� ���� */
	// by sysandj : MCQ��ü �߰� �ʿ�
// 	if (uvEng_ShMem_GetPLCExt()->r_vacuum_vac_atm_waiting)
// 	{
// 		return ENG_JWNS::en_wait;	/* Wait ���� �� */
// 	}
	return ENG_JWNS::en_next;	/* Wait ���� �ƴ� */
}

/*
 desc : Shutter�� ���� Wait �������� ����
 parm : None
 retn : wait, next
*/
ENG_JWNS CWorkStep::IsShutterWait()
{
	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Shutter.Wait");
#ifndef _DEBUG
	// 	/* ���� Vacuum ���°� Waiting ������� ���ο� ���� */
	// 	if (uvEng_ShMem_GetPLCExt()->r_shutter_open_close_waiting)
	// 	{
	// 		if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;
	// 
	// 		return ENG_JWNS::en_wait;	/* Wait ���� �� */
	// 	}
#endif
	return ENG_JWNS::en_next;	/* Wait ���� �ƴ� */
}

/*
 desc : Shutter or Vacuum�� ��� �������� ����
 parm : None
 retn : TRUE (��� ������) or FALSE (��� ���� �ƴ�)
*/
ENG_JWNS CWorkStep::IsShutterVacuumWait()
{
	BOOL bWaitVacuum = FALSE;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Shutter.Vacuum.Wait");

	if (uvEng_GetConfig()->IsCheckVacuum())
	{
		// by sysandj : MCQ��ü �߰� �ʿ�
		//bWaitVacuum	= uvEng_ShMem_GetPLCExt()->r_vacuum_vac_atm_waiting ? TRUE : FALSE;
	}
	/* ���� Shutter�� Vacuum�� Open (On) or Close (Off) ���°� �ƴ� ��� */
	// by sysandj : MCQ��ü �߰� �ʿ�
// 	return (uvEng_ShMem_GetPLCExt()->r_shutter_open_close_waiting || bWaitVacuum) ?
// 		ENG_JWNS::en_wait : ENG_JWNS::en_next;
	return ENG_JWNS::en_next;
}

/*
 desc : �뱤 ���� ��ġ�� �̵�
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetMovePrintXY()
{
	/* ���� �۾� Step Name ���� */
	SetStepName(L"Move.Printing.XY");
	/* �뱤 ���� ��ġ�� Stage�� �̵� ��Ŵ */
	return uvEng_Luria_ReqSetPrintStartMove() ? ENG_JWNS::en_next : ENG_JWNS::en_error;
}

/*
 desc : PrePrinting ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetPrePrinting()
{
	/* ���� �۾� Step Name ���� */
	SetStepName(L"Run.Pre.Printing");

	/* Printing ���� �� �ʱ�ȭ */
	uvEng_ShMem_GetLuria()->exposure.ResetExposeState();
	/* Luria Service���� PrePrinting ȣ�� */
	if (!uvEng_Luria_ReqSetPrintOpt(0x00))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetPrintOpt.PrePrint)");
		return ENG_JWNS::en_error;
	}

	SetSendCmdTime();	/* ���� �ֱٿ� �۽��� �ð� ���� */
	/* Printing Status ���� ��û */
	if (!uvEng_Luria_ReqGetExposureState())
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetExposureState.PrePrint)");
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : PrePrinting �Ϸ� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsPrePrinted()
{
	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Pre.Printed");

	/* ���� �غ� ���°� ���еǾ����� Ȯ�� */
	if (uvCmn_Luria_IsExposeStateFailed(ENG_LCEP::en_pre_print))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to run the preprint job");
		return ENG_JWNS::en_error;
	}
	/* ���� �뱤 �غ� ���°� �������� ��� */
	if (uvCmn_Luria_IsExposeStateSuccess(ENG_LCEP::en_pre_print))
	{
		return ENG_JWNS::en_next;
	}
#if 0	/* PrePrint�� ���, �ڵ����� ������� �ʰ�, �ݵ�� State ��û�ؾ� �� */
	if (!uvEng_GetConfig()->luria_svc.use_announcement)
#endif
	{
		/* ���� �ð� ��� �ߴ��� ���� */
		if (IsSendCmdTime(250))
		{
			/* �ٽ� ���� �뱤 ���� ���� ��û */
			if (!uvEng_Luria_ReqGetExposureState())
			{
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetExposureState.IsPrePrinted)");
				return ENG_JWNS::en_error;
			}
		}
	}
	return ENG_JWNS::en_wait;
}

/*
 desc : Printing ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetPrinting()
{
	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.Printing");

	/* ���� �ֱٿ� �뱤�� ���� (Stripe) ���� �ʱ�ȭ */
	m_u8PrintStripNum = 0x00;
	m_u64TickPrintStrip = GetTickCount64();	/* ���� �ֱٿ� �뱤 ������ ����� �ð� �ʱ�ȭ */
	m_i32PrintingStageY = 0;				/* �뱤 (Printing)�ϰ� �ִ� Stage Y ���� ��ġ �ʱ�ȭ (����: um) */
	/* Luria Service���� Printing ȣ�� */
	if (!uvEng_Luria_ReqSetPrintOpt(0x01))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetPrintOpt.Print)");
		return ENG_JWNS::en_error;
	}
	if (!uvEng_GetConfig()->luria_svc.use_announcement)
	{
		SetSendCmdTime();	/* ���� �ֱٿ� �۽��� �ð� ���� */
		/* Printing Status ���� ��û (�Լ� ���ο� 1�ʿ� 4�� �ۿ� ��û���� ����) */
		if (!uvEng_Luria_ReqGetExposureState())
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetExposureState.Printing)");
			return ENG_JWNS::en_error;
		}
	}

	return ENG_JWNS::en_next;
}

/*
 desc : Printing �Ϸ� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsPrinted()
{
	TCHAR tzTitle[128] = { NULL };
	INT32 i32StageY = 0;
	UINT8 u8State = 0x00;
	DOUBLE dbMotionY = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);	/* unit: mm */

	/* ���� �뱤 ����� �� ���� */
	swprintf_s(tzTitle, 128, L"Run.Printing (%.2f)", uvCmn_Luria_GetPrintRate());
	/* ���� �۾� Step Name ���� */
	SetStepName(tzTitle);

	/* ���� �뱤 ���� �� ���� �ߴ��� ���� */
	u8State = (UINT8)uvCmn_Luria_GetExposeState();
	if (0x0f == (u8State & 0x0f))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"There was a problem during the expose operation");
	}

	/* �뱤 �Ϸ� �����̸� */
	else if (uvCmn_Luria_IsExposeStateSuccess(ENG_LCEP::en_print))
	{
		return ENG_JWNS::en_next;
	}

	/* Printing Status ���� ��û (�Լ� ���ο� 1�ʿ� 2�� �ۿ� ��û���� ����) */
	if (!uvEng_GetConfig()->luria_svc.use_announcement)
	{
		/* ���� �ð� ��� �ߴ��� ���� */
		if (IsSendCmdTime(250))
		{
			/* �ٽ� ���� �뱤 ���� ���� ��û */
			if (!uvEng_Luria_ReqGetExposureState())
			{
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetExposureState.IsPrinting)");
				return ENG_JWNS::en_error;
			}
		}
	}

	/* Y �� �뱤 �������� Motion�� �����̰� ������, ��� �ð��� �ֱ� �ð����� ���� */
	if (abs(dbMotionY - m_dbLastPrintPosY) > 1.0f)	/* 1.0 mm �̻� ���̰� �߻��ߴ��� ���� */
	{
		m_dbLastPrintPosY = dbMotionY;
		m_u64DelayTime = GetTickCount64();
	}

	return ENG_JWNS::en_wait;
}
//bool useManual = false;

ENG_JWNS CWorkStep::SetAlignMarkRegistforStatic()
{
	CAtlList <STG_XMXY> lstMarks;
	//LPG_GMLV pstMarkDiff = &uvEng_GetConfig()->mark_diff;
	LPG_CSAI pstSetAlign = &uvEng_GetConfig()->set_align;
	std::tuple<double, double> val;
	STG_XMXY temp;

	TCHAR tzMsg[256] = { NULL };

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.Align.Mark.RegistForSatic");

	auto& webMonitor = GlobalVariables::GetInstance()->GetWebMonitor();
	auto& motions = GlobalVariables::GetInstance()->GetAlignMotion();
	auto status = motions.status;
	bool success = true;
	const int CENTERCAM = motions.markParams.centerCamIdx;

	if (status.globalMarkCnt != 4)
		return ENG_JWNS::en_next;

	//webMonitor.AddLog("=============================================================================");

	for (int i = 0; i < status.globalMarkCnt + status.localMarkCnt; i++)
	{
		bool isGlobal = i < status.globalMarkCnt ? true : false;


		/* Global Fiducial ���� */
		if (isGlobal)
		{
			if (uvEng_Luria_GetGlobalMark(i, &temp) == false)
			{
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to get the global mark from selected gerber file");
				return ENG_JWNS::en_error;
			}
			else
			{
				swprintf_s(tzMsg, 256, L"Global Luria Mark%d : X = %.4f Y = %.4f", temp.org_id, temp.mark_x, temp.mark_y);
				LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
			}
		}
		/* Local Fiducial ���� */
		else
		{
			if (uvEng_Luria_GetLocalMark(i - status.globalMarkCnt, &temp) == false)
			{
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to get the local mark from selected gerber file");
				return ENG_JWNS::en_error;
			}
			else
			{
				swprintf_s(tzMsg, 256, L"Local Luria Mark%d : X = %.4f Y = %.4f", temp.org_id, temp.mark_x, temp.mark_y);
				LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
			}
		}

		/* Grabbed Mark images�� ��� �� �������� */
		auto grab = CommonMotionStuffs::GetInstance().GetGrabPtr(CENTERCAM, temp.tgt_id, isGlobal ? ENG_AMTF::en_global : ENG_AMTF::en_local);

		if (grab == nullptr)
			return ENG_JWNS::en_error;

		if (grab->marked == 0)
			return ENG_JWNS::en_error;

		swprintf_s(tzMsg, 256, L"%s Mark%d Move_mm: X = %.4f Y = %.4f", (isGlobal ? L"Global" : L"Local"), temp.org_id, grab->move_mm_x, grab->move_mm_y);
		LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);


		CaliPoint expoOffset, alignOffset, grabOffset;


		bool findGrabOffset = motions.GetOffsetFromPool(OffsetType::grab, temp.tgt_id, expoOffset);

		auto grabOffsetX = findGrabOffset ? expoOffset.offsetX : grab->move_mm_x;
		auto grabOffsetY = findGrabOffset ? expoOffset.offsetY : grab->move_mm_y;

		auto bkx = temp.mark_x;
		auto bky = temp.mark_y;

		temp.mark_x -= grabOffsetX;
		temp.mark_y -= grabOffsetY;

		temp.reserve = grab->reserve;


		switch (pstSetAlign->use_mark_offset)
		{
		case (UINT8)ENG_ADRT::en_no_use:
		{
			expoOffset.offsetX = .0f; expoOffset.offsetY = .0f;
		}
		break;

		case (UINT8)ENG_ADRT::en_from_info:
		{
			double offsetX = 0, offsetY = 0;
			motions.GetMarkoffsetLegacy(ENG_AMOS::en_static_3cam, isGlobal, temp.tgt_id, offsetX, offsetY);
			expoOffset.offsetX = offsetX; expoOffset.offsetY = offsetY;
		}
		break;

		case (UINT8)ENG_ADRT::en_from_mappingfile:
		{
			if (motions.GetOffsetFromPool(OffsetType::expo, temp.tgt_id, expoOffset) == false)
				return ENG_JWNS::en_error;
		}
		break;

		default: break;
		}

		if (motions.GetOffsetFromPool(OffsetType::align, temp.tgt_id, alignOffset) == false)
			return ENG_JWNS::en_error;

		swprintf_s(tzMsg, 256, L"%s  mark%d_offset_x = %.4f mark_offset_y =%.4f", (isGlobal ? L"Global" : L"Local"), temp.org_id, expoOffset.offsetX, expoOffset.offsetY);
		LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);


		auto foffsetX = alignOffset.offsetX - (expoOffset.offsetX);
		auto foffsetY = alignOffset.offsetY - (expoOffset.offsetY);

		temp.mark_x -= foffsetX;
		temp.mark_y -= foffsetY;

		//string fmtFormat = fmt::format("tgt mark {} - �׷��ɼ�X = {:.3f}, �׷��ɼ�Y = {:.3f},  ����οɼ�X = {:.3f} ,����οɼ�Y = {:.3f} ,  �ͽ����ɼ�X = {:.3f}, �ͽ����ɼ�Y = {:.3f}, ����X = {:.3f} , ���� Y = {:.3f}, ��������X = {:.3f} , ��������Y = {:.3f}",
			//temp.tgt_id, grabOffsetX, grabOffsetY, alignOffset.offsetX, alignOffset.offsetY, expoOffset.offsetX, expoOffset.offsetX, bkx, bky, bkx - temp.mark_x, bky - temp.mark_y);
//			webMonitor.AddLog(fmtFormat);

		lstMarks.AddTail(temp);
	}


	/* -------------------------------------------------------------------------------------- */
/* �Ź��� ��ϵ� Global Fiducial Mark�� Grabbed Image�� ���� ���� / �밢�� ���� ũ�� �� */
/* -------------------------------------------------------------------------------------- */
	int i;
	BOOL bSucc = TRUE;
	CAtlList <STG_XMXY> lsOrgMarks;
	const int CrossLengthCount = 6;
	STG_XMXY stMarkPos1 = { STG_XMXY(), }, stMarkPos2 = { STG_XMXY(), };
	UINT8 u8Reg1[6] = { 0, 1, 0, 2, 0, 1 }, u8Reg2[6] = { 2, 3, 1, 3, 3, 2 };
	INT32 i32Inverse = 1, i32DistS[6] = { NULL }, i32DistT[6] = { NULL };
	for (i = 0; i < status.globalMarkCnt; i++)
	{
		bSucc = uvEng_Luria_GetGlobalMark(i, &stMarkPos1);
		if (!bSucc)	LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to get the global mark from selected gerber file");
		else		lsOrgMarks.AddTail(stMarkPos1);
	}
	/* ���� �Ź� ��ǥ ���� ��ũ ���� / ���� �� �ӽ� ���� */
	for (i = 0; bSucc && i < CrossLengthCount; i++)
	{
		stMarkPos1 = lsOrgMarks.GetAt(lsOrgMarks.FindIndex(u8Reg1[i]));
		stMarkPos2 = lsOrgMarks.GetAt(lsOrgMarks.FindIndex(u8Reg2[i]));
		/* mm -> 0.1 um or 100 nm */
		i32DistS[i] = (INT32)ROUNDED(sqrt(pow(stMarkPos1.mark_x - stMarkPos2.mark_x, 2) +
			pow(stMarkPos1.mark_y - stMarkPos2.mark_y, 2)) * 10000.0f, 0);
	}

	CUniToChar csCnv;
	UINT8 u8MarkLen[6] = { NULL };
	UINT32 u32MarkDist = 0;
	LPG_GMLV pstMarkDiff = &uvEng_GetConfig()->mark_diff;
	LPG_RJAF pstJobRecipe = uvEng_JobRecipe_GetSelectRecipe();
	LPG_REAF pstRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstJobRecipe->expo_recipe));
	/* ���� ���� ���� ����� ��ũ ���� / ���� �� �ӽ� ���� */
	if (status.globalMarkCnt == 0x04)
	{
		for (i = 0; i < CrossLengthCount; i++)
		{
			stMarkPos1 = lstMarks.GetAt(lstMarks.FindIndex(u8Reg1[i]));
			stMarkPos2 = lstMarks.GetAt(lstMarks.FindIndex(u8Reg2[i]));

			/* mm -> 0.1 um or 100 nm */
			i32DistT[i] = (INT32)ROUNDED(sqrt(pow(stMarkPos1.mark_x - stMarkPos2.mark_x, 2) +
				pow(stMarkPos1.mark_y - stMarkPos2.mark_y, 2)) * 10000.0f, 0);
			u32MarkDist = (UINT32)abs(i32DistS[i] - i32DistT[i]);
			u8MarkLen[i] = u32MarkDist <= (pstRecipe->global_mark_dist[i] * 10) ? 0x01 : 0x02;
			pstMarkDiff->SetMarkLenDiff(i, u32MarkDist);
			pstMarkDiff->SetMarkLenValid(i, u8MarkLen[i]);	/* Mark ���� ���� (�밢�� ����)�� ���� ��ȿ�� ���� (��/�� ����, ��/�� ������, �밢�� 2�� = �� 6�� */
		}
	}
	swprintf_s(tzMsg, 256, L"Limit Dist = %d, %d, %d, %d, %d, %d",
		pstRecipe->global_mark_dist[0], pstRecipe->global_mark_dist[1], pstRecipe->global_mark_dist[2],
		pstRecipe->global_mark_dist[3], pstRecipe->global_mark_dist[4], pstRecipe->global_mark_dist[5]);
	LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);

	swprintf_s(tzMsg, 256, L"Result Dist = %.1f, %.1f, %.1f, %.1f, %.1f, %.1f",
		pstMarkDiff->result[0].diff * 100000.0f, pstMarkDiff->result[1].diff * 100000.0f, pstMarkDiff->result[2].diff * 100000.0f,
		pstMarkDiff->result[3].diff * 100000.0f, pstMarkDiff->result[4].diff * 100000.0f, pstMarkDiff->result[5].diff * 100000.0f);
	LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);



	/* Global Transformation Recipe ���� ���� */
	if (uvEng_Luria_ReqSetGlobalTransformationRecipe(0x00, 0x00, 0x00) == false)
		return ENG_JWNS::en_error;
	/* ���� ���� �� �ʱ�ȭ */
	uvCmn_Luria_ResetRegistrationStatus();
	/* ������ Shared Local Zone ��� �������� �����ؾ� �� */
	if (uvEng_Luria_ReqSetUseSharedLocalZones(0x00) == false)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetSharedLocalZones)");
		return ENG_JWNS::en_error;
	}

	auto fidCnt = lstMarks.GetCount();

	//LPG_I32XY pstMarks = NULL;
	//pstMarks = new STG_I32XY[fidCnt];

	auto pstMarks = std::make_unique< STG_I32XY[]>(fidCnt);

	for (int i = 0; i < fidCnt; i++)
	{
		bool isGlobal = i < status.globalMarkCnt ? true : false;
		for (int j = isGlobal ? 0 : status.globalMarkCnt; j < fidCnt; j++)
		{
			auto at = lstMarks.GetAt(lstMarks.FindIndex(j));
			if (at.org_id == (isGlobal ? i : i - status.globalMarkCnt) && at.GetFlag(isGlobal ? STG_XMXY_RESERVE_FLAG::GLOBAL : STG_XMXY_RESERVE_FLAG::LOCAL))
			{
				swprintf_s(tzMsg, 256, L"%s Regist Mark%d : X =%.4f Y = %.4f", (isGlobal ? L"Global" : L"Local"), at.org_id, at.mark_x, at.mark_y);
				LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);

				pstMarks[i].x = (INT32)ROUNDED(at.mark_x * 1000000.0f, 0);
				pstMarks[i].y = (INT32)ROUNDED(at.mark_y * 1000000.0f, 0);

			}
		}
	}

	/* Luria�� ����� Mark ��ġ �� ��� ���� */
	if (uvEng_Luria_ReqSetRegistPointsAndRun(fidCnt, pstMarks.get()) == false)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetRegist)");
		return ENG_JWNS::en_error;
	}

	uvEng_Luria_ReqGetGetTransformationParams(0);


	SetSendCmdTime();
	return ENG_JWNS::en_next;

}


/*
 desc : Luria Service�� ������ Align Mark ���� �� ���
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetAlignMarkRegist()
{
	TCHAR tzMesg[256] = { NULL };
	UINT8 i, j, u8CamID = 0x00, u8ImgID = 0x00, u8MarkG = 0x00, u8MarkL = 0x00;
	UINT8 u8Mark1[6] = { NULL }, u8Mark2[6] = { NULL }, u8MarkLen[6] = { NULL };
	UINT8 u8Reg1[6] = { 0, 1, 0, 2, 0, 1 }, u8Reg2[6] = { 2, 3, 1, 3, 3, 2 };
	UINT16 u16MarkType = 0x0000;
	BOOL bSucc = TRUE;
	INT32 i32Inverse = 1, i32DistS[6] = { NULL }, i32DistT[6] = { NULL };
	UINT32 u32MarkDist = 0;
	STG_XMXY stMarkPos1 = { STG_XMXY(), }, stMarkPos2 = { STG_XMXY(), };
	CAtlList <STG_XMXY> lstMarks;
	LPG_I32XY pstMarks = NULL;
	LPG_ACGR pstGrab = NULL;
	LPG_GMLV pstMarkDiff = &uvEng_GetConfig()->mark_diff;
	LPG_CSAI pstSetAlign = &uvEng_GetConfig()->set_align;
	std::tuple<double, double> val;
	CUniToChar csCnv;
	LPG_RJAF pstJobRecipe = uvEng_JobRecipe_GetSelectRecipe();
	LPG_REAF pstRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstJobRecipe->expo_recipe));
	auto& motion = GlobalVariables::GetInstance()->GetAlignMotion();
	auto status = motion.status;
	ENG_AMOS motionType = motion.markParams.alignMotion; //�̰� �ǽð����� �ٲ�������� �����ϰ�, �׽�Ʈ������ ����Ѵ�. 

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.Align.Mark.Regist");

	/* XML ���Ͽ� ��ϵ� Mark ���� Ȯ�� */
	u8MarkG = uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	u8MarkL = uvEng_Luria_GetMarkCount(ENG_AMTF::en_local);
	pstMarks = (LPG_I32XY)::Alloc(sizeof(STG_I32XY) * UINT32(u8MarkG + u8MarkL));
	ASSERT(pstMarks);

	auto grabFindFunc = [&](int markTgt, CAtlList <LPG_ACGR>* grabPtr, STG_XMXY_RESERVE_FLAG matchingFlags) -> LPG_ACGR
		{
			if (grabPtr == NULL)
				return nullptr;

			for (int i = 0; i < grabPtr->GetCount(); i++)
			{
				auto grab = grabPtr->GetAt(grabPtr->FindIndex(i));
				if (grab != nullptr && grab->fiducialMarkIndex == markTgt && (grab->reserve & matchingFlags) == matchingFlags)
					return grab;
			}
			return nullptr;
		};

	/* ���� �뱤 ��尡 Align Expose �� ��츸 �����ϵ��� �� */
	switch (m_enWorkJobID)
	{
	case ENG_BWOK::en_expo_align:
	case ENG_BWOK::en_expo_cali:
	case ENG_BWOK::en_mark_test:
		/* �Ź��� ��ϵ� ��ü Mark ���� (Global + Local)�� Grabbed Image ���� �� */
		if (!uvEng_GetConfig()->IsRunDemo())
		{
			int checkSum = IsMarkTypeOnlyGlobal() ? (u8MarkG) : u8MarkG + u8MarkL;

			if (u8MarkG < 1 || uvEng_Camera_GetGrabbedCount() != (checkSum))
			{
				//abh1000 Local Test
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to inspect all marks");
				return ENG_JWNS::en_error;
			}
		}
		break;
	} //��ũī��Ʈ �׷�ī��Ʈ ���ؼ� �ٸ��� ����ó���ؼ� ����.

	/* Global ��ũ�� ��ϵǾ� ���� �ʴٸ� Pass */
	if (u8MarkG < 4)
		return ENG_JWNS::en_next;

	TCHAR tzMsg[256] = { NULL };
	/* Global Fiducial ���� */
	for (i = 0; bSucc && i < u8MarkG; i++)
	{
		bSucc = uvEng_Luria_GetGlobalMark(i, &stMarkPos1);
		if (!bSucc)	LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to get the global mark from selected gerber file");
		else		lstMarks.AddTail(stMarkPos1);

		swprintf_s(tzMsg, 256, L"Global Luria Mark%d : X = %.4f Y = %.4f", i + 1, stMarkPos1.mark_x, stMarkPos1.mark_y);
		LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
	}

	if (bSucc == false)
		return ENG_JWNS::en_error;


	const int CrossLengthCount = 6;
	/* -------------------------------------------------------------------------------------- */
	/* �Ź��� ��ϵ� Global Fiducial Mark�� Grabbed Image�� ���� ���� / �밢�� ���� ũ�� �� */
	/* -------------------------------------------------------------------------------------- */
	/* ���� �Ź� ��ǥ ���� ��ũ ���� / ���� �� �ӽ� ���� */
	for (i = 0; bSucc && i < CrossLengthCount; i++)
	{
		stMarkPos1 = lstMarks.GetAt(lstMarks.FindIndex(u8Reg1[i]));
		stMarkPos2 = lstMarks.GetAt(lstMarks.FindIndex(u8Reg2[i]));
		/* mm -> 0.1 um or 100 nm */
		i32DistS[i] = (INT32)ROUNDED(sqrt(pow(stMarkPos1.mark_x - stMarkPos2.mark_x, 2) +
			pow(stMarkPos1.mark_y - stMarkPos2.mark_y, 2)) * 10000.0f, 0);
	}


	/* Align Mark�� ������ ���� ���� Camera Calibration Offset ���� */
	if ((ENG_BWOK::en_mark_test == m_enWorkJobID ||
		ENG_BWOK::en_expo_align == m_enWorkJobID ||
		ENG_BWOK::en_expo_cali == m_enWorkJobID) &&
		!uvEng_GetConfig()->IsRunDemo())
	{

		//-------------------------    �۷ι� ��ũ (�ɼ� ������)   ���� �ϰ����� ���� ���� ------------------------
		auto grabMark = uvEng_Camera_GetGrabbedMarkAll();

		for (i = 0; bSucc && i < u8MarkG; i++)
		{
			auto& lstMarkAt = lstMarks.GetAt(lstMarks.FindIndex(i));

			pstGrab = grabFindFunc(lstMarkAt.tgt_id, grabMark, STG_XMXY_RESERVE_FLAG::GLOBAL);

			if (!pstGrab)
			{
				swprintf_s(tzMesg, 128, L"Failed to get the grabbed image (global mark) "
					L"(cam_id=%d,mark_id=%d)", u8CamID, lstMarkAt.org_id);
				LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
				return ENG_JWNS::en_error;
			}

			if (pstGrab->marked == 0x01)
			{
				lstMarkAt.mark_x -= pstGrab->move_mm_x;
				lstMarkAt.mark_y -= pstGrab->move_mm_y;

				swprintf_s(tzMsg, 256, L"Global Mark%d Move_mm: X = %.4f Y = %.4f", lstMarkAt.org_id, pstGrab->move_mm_x, pstGrab->move_mm_y);
				LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);

				if (pstSetAlign->use_mark_offset == (UINT8)ENG_ADRT::en_from_info && pstSetAlign->markOffsetPtr != nullptr)
				{
					if (pstSetAlign->markOffsetPtr->Get(motionType, true, lstMarkAt.tgt_id, val) == false)
					{
						swprintf_s(tzMesg, 128, L"Failed to get expo offset  global mark %d", lstMarkAt.org_id);
						LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
						bSucc = FALSE;
					}

					//lstMarkAt.mark_x -= std::get<0>(val);
					//lstMarkAt.mark_y -= std::get<1>(val);
					lstMarkAt.mark_y += std::get<1>(val);
					lstMarkAt.mark_y += std::get<1>(val);

					swprintf_s(tzMsg, 256, L"GLOBAL mark%d expo_offset_x = %.4f expo_offset_y =%.4f", lstMarkAt.org_id, std::get<0>(val), std::get<1>(val));
					LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
				}
				else if (pstSetAlign->use_mark_offset == (UINT8)ENG_ADRT::en_from_mappingfile) //���������� �̿��ϰԵɰ�� �̰��� �߰�.
				{

				}
			}
			else
			{
				swprintf_s(tzMesg, 128, L"The mark image (global) was grabbed, but failed to find a model "
					L"(cam_id=%d,mark_id=%d)", u8CamID, lstMarkAt.org_id);
				LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
			}
		}

		if (IsMarkTypeOnlyGlobal() == false && uvEng_Luria_GetMarkCount(ENG_AMTF::en_local) != 0)
		{
			for (i = 0; i < u8MarkL; i++)
			{
				uvEng_Luria_GetLocalMark(i, &stMarkPos1);
				lstMarks.AddTail(stMarkPos1);
				swprintf_s(tzMsg, 256, L"Local Luria Mark%d : X = %.4f Y = %.4f", stMarkPos1.org_id, stMarkPos1.mark_x, stMarkPos1.mark_y);
				LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
			}

			for (j = u8MarkG; j < u8MarkG + u8MarkL; j++)
			{
				STG_XMXY& lstMarkAt = lstMarks.GetAt(lstMarks.FindIndex(j));

				pstGrab = grabFindFunc(lstMarkAt.tgt_id, grabMark, STG_XMXY_RESERVE_FLAG::LOCAL);

				if (pstGrab == nullptr)
				{
					swprintf_s(tzMesg, 128, L"Failed to get the grabbed image (local mark) "
						L"(cam_id=%d,mark_id=%d)", u8CamID, lstMarkAt.org_id);
					LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
					return ENG_JWNS::en_error;
				}
				else
				{
					/* ---------------------------------------------------------------- */
					/* Align Mark ���� ������ ���� Mark Offset �� �����ؼ� ��ġ �� ���� */
					/* ---------------------------------------------------------------- */

					if (pstGrab->marked == 0x01)
					{
						lstMarkAt.mark_x -= pstGrab->move_mm_x;
						lstMarkAt.mark_y -= pstGrab->move_mm_y;

						swprintf_s(tzMsg, 256, L"Local Mark%d Move_mm: X = %.4f Y = %.4f", lstMarkAt.org_id, pstGrab->move_mm_x, pstGrab->move_mm_y);
						LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);

						if (pstSetAlign->use_mark_offset == (UINT8)ENG_ADRT::en_from_info && pstSetAlign->markOffsetPtr != nullptr)
						{
							if (pstSetAlign->markOffsetPtr->Get(motionType, false, lstMarkAt.tgt_id, val) == false)
							{
								swprintf_s(tzMesg, 128, L"Failed to get expo offset  global mark %d", lstMarkAt.org_id);
								LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
								bSucc = FALSE;
							}

							//lstMarkAt.mark_x -= std::get<0>(val);
							//lstMarkAt.mark_y -= std::get<1>(val);
							lstMarkAt.mark_x += std::get<0>(val);
							lstMarkAt.mark_y += std::get<1>(val);

							swprintf_s(tzMsg, 256, L"local mark%d expo_offset_x = %.4f expo_offset_y =%.4f", lstMarkAt.org_id, std::get<0>(val), std::get<1>(val));
							LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
						}
						else if (pstSetAlign->use_mark_offset == (UINT8)ENG_ADRT::en_from_mappingfile) //���������� �̿��ϰԵɰ�� �̰��� �߰�.
						{

						}
					}
					else
					{
						swprintf_s(tzMesg, 128, L"The mark image (local) was grabbed, but failed to find a model "
							L"(cam_id=%d,mark_id=%d)", u8CamID, lstMarkAt.org_id);
						LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
					}
				}
			}
		}

		if (bSucc)
		{
			/* ���� ���� ���� ����� ��ũ ���� / ���� �� �ӽ� ���� */
			if (u8MarkG == 0x04)
			{
				u32MarkDist = 0;
				for (i = 0; i < CrossLengthCount; i++)
				{
					stMarkPos1 = lstMarks.GetAt(lstMarks.FindIndex(u8Reg1[i]));
					stMarkPos2 = lstMarks.GetAt(lstMarks.FindIndex(u8Reg2[i]));

					/* mm -> 0.1 um or 100 nm */
					i32DistT[i] = (INT32)ROUNDED(sqrt(pow(stMarkPos1.mark_x - stMarkPos2.mark_x, 2) +
						pow(stMarkPos1.mark_y - stMarkPos2.mark_y, 2)) * 10000.0f, 0);
					u32MarkDist = (UINT32)abs(i32DistS[i] - i32DistT[i]);
					//u32MarkDist = uvEng_Camera_GetGrabbedMarkDist((ENG_GMDD)i) * 1000.0f;
					u8MarkLen[i] = u32MarkDist <= (pstRecipe->global_mark_dist[i] * 10) ? 0x01 : 0x02;
					//u8MarkLen[i] = u32MarkDist <= (pstRecipe->global_mark_dist[i]) ? 0x01 : 0x02;
					pstMarkDiff->SetMarkLenDiff(i, u32MarkDist);
					pstMarkDiff->SetMarkLenValid(i, u8MarkLen[i]);	/* Mark ���� ���� (�밢�� ����)�� ���� ��ȿ�� ���� (��/�� ����, ��/�� ������, �밢�� 2�� = �� 6�� */
				}
			}
			swprintf_s(tzMsg, 256, L"Limit Dist = %d, %d, %d, %d, %d, %d",
				pstRecipe->global_mark_dist[0], pstRecipe->global_mark_dist[1], pstRecipe->global_mark_dist[2],
				pstRecipe->global_mark_dist[3], pstRecipe->global_mark_dist[4], pstRecipe->global_mark_dist[5]);
			LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);

			swprintf_s(tzMsg, 256, L"Result Dist = %.1f, %.1f, %.1f, %.1f, %.1f, %.1f",
				pstMarkDiff->result[0].diff * 100000.0f, pstMarkDiff->result[1].diff * 100000.0f, pstMarkDiff->result[2].diff * 100000.0f,
				pstMarkDiff->result[3].diff * 100000.0f, pstMarkDiff->result[4].diff * 100000.0f, pstMarkDiff->result[5].diff * 100000.0f);
			LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
#if 0
			swprintf_s(tzMesg, 256, L"Dist = (top_horz = %.1f)(btm_horz = %.1f)(lft_vert = %.1f)(rgt_vert = %.1f)(lft_diag = %.1f)(rgt_diag = %.1f) �Դϴ� �뱤 �Ͻðڽ��ϱ�?",
				pstMarkDiff->result[0].diff * 100000.0f, pstMarkDiff->result[1].diff * 100000.0f, pstMarkDiff->result[2].diff * 100000.0f,
				pstMarkDiff->result[3].diff * 100000.0f, pstMarkDiff->result[4].diff * 100000.0f, pstMarkDiff->result[5].diff * 100000.0f);
			if (IDNO == AfxMessageBox(tzMesg, MB_YESNO))
			{
				bSucc = FALSE;
			}
#endif
			if (!pstMarkDiff->IsMarkLenValidAll())
			{
				bSucc = FALSE;
				swprintf_s(tzMesg, 128, L"The distance between measured marks is not vald"
					L"(1:%d)(2:%d)(3:%d)(4:%d)(5:%d)(6:%d)",
					u8MarkLen[0], u8MarkLen[1], u8MarkLen[2], u8MarkLen[3],
					u8MarkLen[4], u8MarkLen[5]);
				LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
			}
			/* Mark���� �Ÿ� �� ���� �� �����ߴٰ� �÷��� �� ���� */
			pstMarkDiff->SetMarkLenData();

			/*����� �������� ������ ���Ѱ����� ũ�� ����*/
			//if (pstMarkDiff->HorzDiff(0.005) && pstMarkDiff->VertDiff(0.003))
			if (pstMarkDiff->HorzDiff(pstSetAlign->mark_horz_diff) || pstMarkDiff->VertDiff(pstSetAlign->mark_vert_diff))
			{
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"The distance Horz, Vert Error");
				bSucc = FALSE;
			}
		}
	}

	/* Global Transformation Recipe ���� ���� */
	if (bSucc)	bSucc = uvEng_Luria_ReqSetGlobalTransformationRecipe(0x00, 0x00, 0x00);
	/* ���� ���� �� �ʱ�ȭ */
	uvCmn_Luria_ResetRegistrationStatus();
	/* ������ Shared Local Zone ��� �������� �����ؾ� �� */
	if (!uvEng_Luria_ReqSetUseSharedLocalZones(0x00))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetSharedLocalZones)");
		bSucc = FALSE;
	}

	for (i = 0; i < u8MarkG; i++)
	{
		stMarkPos1 = lstMarks.GetAt(lstMarks.FindIndex(i));

		pstMarks[stMarkPos1.org_id].x = (INT32)ROUNDED(stMarkPos1.mark_x * 1000000.0f, 0);	/* mm -> nm */
		pstMarks[stMarkPos1.org_id].y = (INT32)ROUNDED(stMarkPos1.mark_y * 1000000.0f, 0);	/* mm -> nm */

		swprintf_s(tzMsg, 256, L"Global Regist Mark%d : X =%.4f Y = %.4f", stMarkPos1.org_id, DOUBLE(pstMarks[stMarkPos1.org_id].x / 1000000.0f), DOUBLE(pstMarks[stMarkPos1.org_id].y / 1000000.0f));
		LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
	}
	/* ������ Local Mark XML ���Ͽ��� �о���� ������� ���� */
	for (j = u8MarkG; j < u8MarkG + u8MarkL; j++)
	{
		stMarkPos1 = lstMarks.GetAt(lstMarks.FindIndex(j));
		pstMarks[stMarkPos1.org_id + u8MarkG].x = (INT32)ROUNDED(stMarkPos1.mark_x * 1000000.0f, 0);	/* mm -> nm */
		pstMarks[stMarkPos1.org_id + u8MarkG].y = (INT32)ROUNDED(stMarkPos1.mark_y * 1000000.0f, 0);	/* mm -> nm */

		swprintf_s(tzMsg, 256, L"Local Regist Mark%d : X =%.4f Y = %.4f", stMarkPos1.org_id, DOUBLE(pstMarks[stMarkPos1.org_id + u8MarkG].x / 1000000.0f), DOUBLE(pstMarks[stMarkPos1.org_id + u8MarkG].y / 1000000.0f));
		LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
	}

	/* Luria�� ����� Mark ��ġ �� ��� ���� */
	if (!uvEng_Luria_ReqSetRegistPointsAndRun(u8MarkG + u8MarkL, pstMarks))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetRegist)");
		bSucc = FALSE;
	}

	uvEng_Luria_ReqGetGetTransformationParams(0);


	/* �޸� ���� */
	::Free(pstMarks);
	/* List ��ü ���� */
	lstMarks.RemoveAll();

	/* ���� ���� Ȯ�α��� ���� �ð� ������ ���ؼ� */
	SetSendCmdTime();

	//return ENG_JWNS::en_error;
	return bSucc ? ENG_JWNS::en_next : ENG_JWNS::en_error;
	//return ENG_JWNS::en_next;
}

/*
 desc : Align Mark ���� �� ��� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsAlignMarkRegist()
{
	TCHAR tzMesg[512] = { NULL };
	PTCHAR ptzMesg = NULL;
	UINT16 u16Status = 0x0000;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Align.Mark.Regist");

	/* XML ���Ͽ� ��ϵ� Mark ���� Ȯ�� */
	if (uvEng_Luria_GetMarkCount(ENG_AMTF::en_global) < 4)
	{
		/* ��ϵ� Mark�� �����Ƿ�, Pass */
		return ENG_JWNS::en_next;
	}

	/* Registration Status ���� ���������� ��ϵǾ����� Ȯ�� */
	u16Status = uvCmn_Luria_GetRegistrationStatus();
	if (0xffff == u16Status)
	{
		/* �ʹ� ���� ��û�ϸ� �ȵǹǷ� ... */
#ifdef _DEBUG
		if (IsSendCmdTime(500))
#else
		if (IsSendCmdTime(250))
#endif
		{
			/* ���� Fiducial ��� ���� �� ��û */
			if (!uvEng_Luria_ReqGetGetRegistrationStatus())
			{
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetRegistStatus)");
				return ENG_JWNS::en_error;
			}
		}
		return ENG_JWNS::en_wait;
	}
	else if (0x01 != u16Status)
	{
		ptzMesg = uvCmn_Luria_GetRegistrationStatusMesg();
		if (wcslen(ptzMesg) < 1)
		{
			swprintf_s(tzMesg, 512, L"Unknown registration status code (%d)",
				uvEng_ShMem_GetLuria()->panel.get_registration_status);
			LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
		}
		else
		{
			swprintf_s(tzMesg, 512, L"An error occurred in the registration status (%s)", ptzMesg);
			LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
		}
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : ��� ���а� Z Axis�� ���� ���� - ���� �β��� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetPhZAxisMovingAll()
{
	UINT8 i;
	DOUBLE dbPhDiffZ = 0.0f, dbPhVeloZ[8] = { NULL };

	LPG_CSAI pstAlign = &uvEng_GetConfig()->set_align;
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	LPG_CLSI pstLuria = &uvEng_GetConfig()->luria_svc;
	LPG_CMSI pstMC2 = &uvEng_GetConfig()->mc2b_svc;
	UINT8	 u8drv_id;

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Optic.ZAxis.Move");

	/* ���� �ֱ��� ������ ���а� Z �� ���� ���� �� �ʱ�ȭ */
	memset(m_dbPhZAxisSet, 0x00, sizeof(DOUBLE) * MAX_PH);

	/* ��� ���а� ���� ���� */
	if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x01)
	{
		/* ���� ���� �����Ϸ��� ���а��� Z Axis ���̿� ���� �β� ��ŭ ���� or ���� ó�� (����: um)*/
		dbPhDiffZ = pstRecipe->material_thick / 1000.0f - pstAlign->dof_film_thick;

		for (i = 0x00; i < pstLuria->ph_count; i++)
		{
			/* mm -> um ��ȯ */
			m_dbPhZAxisSet[i] = pstLuria->ph_z_focus[i] + dbPhDiffZ;
			/* Photohead Z Axis�� Min or Max �� ���� �ȿ� �ִ��� ���� */
			if (pstLuria->ph_z_move_min > m_dbPhZAxisSet[i] ||
				pstLuria->ph_z_move_max < m_dbPhZAxisSet[i])
			{
				/* Set the error message */
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"Optical Z Axis movement is out of range");
				return ENG_JWNS::en_error;
			}
		}
		/* �ѹ��� ��� ���а� Z �� ���� ���� */
		if (!uvEng_Luria_ReqSetMotorAbsPositionAll(pstLuria->ph_count, m_dbPhZAxisSet))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetMotorAbsPositionAll)");
			return ENG_JWNS::en_error;
		}
	}
	else if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x03)
	{
		/* ���� ���� �����Ϸ��� ���а��� Z Axis ���̿� ���� �β� ��ŭ ���� or ���� ó�� (����: um)*/
		dbPhDiffZ = pstRecipe->material_thick / 1000.0f /* um -> mm */ - pstAlign->dof_film_thick;

		for (i = 0x00; i < pstLuria->ph_count; i++)
		{
			/* mm -> um ��ȯ */
			//m_dbPhZAxisSet[i]	= pstLuria->ph_z_focus[i] * 1000.0f, 0 + dbPhDiffZ;
			m_dbPhZAxisSet[i] = pstLuria->ph_z_focus[i] + dbPhDiffZ;
			/* Photohead Z Axis�� Min or Max �� ���� �ȿ� �ִ��� ���� */
			if (pstMC2->min_dist[i] > m_dbPhZAxisSet[i] ||
				pstMC2->max_dist[i] < m_dbPhZAxisSet[i])
			{
				/* Set the error message */
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"Optical Z Axis movement is out of range");
				return ENG_JWNS::en_error;
			}

			/* �ѹ��� ��� ���а� Z �� ���� ���� */
			u8drv_id = (UINT8)ENG_MMDI::en_axis_ph1 + i;
			dbPhVeloZ[i] = pstMC2->max_velo[UINT8(ENG_MMDI::en_axis_ph2)];
			if (!(uvEng_MC2_SendDevAbsMove(ENG_MMDI(u8drv_id), m_dbPhZAxisSet[i], dbPhVeloZ[i])))
			{
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to move the photohead z axis (set position)");
				return ENG_JWNS::en_error;
			}
		}

	}
	/* ���� ��ġ ������ ��û���� �ʾ�����, ��û �ߴٰ� �ؾ� ��. (�ֳ�! ���� �̵� �� �ٷ� �������� �����Ƿ�) */
	SetSendCmdTime();

	return ENG_JWNS::en_next;
}

/*
 desc : ���а� Z Axis�� ���� ������ �� �Ǿ����� ���� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsPhZAxisMovedAll()
{
	UINT8 i;
	BOOL bArrived = TRUE;
	DOUBLE dbPhNowZ;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Optic.ZAxis.Moved");

	/* ���� ���� ���� ������ ���ο� ���� */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

	/* ��� ���а��� Z ���� ���ϴ� ��ġ�� ���� �ߴ��� ���� */
	for (i = 0x00; i < uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		//#if (CUSTOM_CODE_TEST_UVDI15 == 0)
		//		/* ���� ���� +/- 1 um �̳��̸� �����ߴٰ� ����*/
		//		dbPhNowZ	= uvEng_ShMem_GetLuria()->directph.focus_motor_move_abs_position[i] / 1000.0f;
		//#else
				//dbPhNowZ = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(0x02 + i));
		dbPhNowZ = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(0x06 + i));
		//#endif
		if (abs(dbPhNowZ - m_dbPhZAxisSet[i]) > 0.010 /* .0 um */) /* �� �˳��� ������ ��� �Ѵ�. */
		{
			bArrived = FALSE;
			break;
		}
	}
	/* ���� ��� ���а谡 �������� �ʾҴٸ�, ���� �ð� �Ŀ� ���� ���а� ��ġ ���� ��û */
	if (!bArrived)
	{
		if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
		{
			if (IsSendCmdTime(250))
			{
				/* ���а� Z �� ��ġ�� ������ ���� ���� �� ��û */
				if (!uvEng_Luria_ReqGetMotorAbsPositionAll() ||
					!uvEng_Luria_ReqGetMotorStateAll())
				{
					return ENG_JWNS::en_error;
				}
			}
		}
		return ENG_JWNS::en_wait;
	}

	TCHAR tzMsg[256] = { NULL };
	for (i = 0x00; i < uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		swprintf_s(tzMsg, 256, L"Ph%d Z Axis : %.4f", i + 1, m_dbPhZAxisSet[i]);
		LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
	}

	return ENG_JWNS::en_next;
}

/*
 desc : Align Camera Z Axis�� ���� ���� - ���� �β��� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetACamZAxisMovingAll(unsigned long& lastUniqueID)
{
	UINT8 i = 0x00;
	INT32 i32ACamDiffZ = 0;	/* ����: 0.1 um or 100 nm */
	LPG_CSAI pstAlign = &uvEng_GetConfig()->set_align;
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	LPG_CCDB pstACamInfo = &uvEng_GetConfig()->set_basler;
	//LPG_CAFI pstACamFocus = &uvEng_GetConfig()->acam_focus;
	LPG_CASI pstACamSpec = &uvEng_GetConfig()->acam_spec;

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.ACam.Z.Axis.Moving");

	/* ���� Align Camera Z �� ���� �� �ʱ�ȭ */
	memset(m_i32ACamZAxisSet, 0x00, sizeof(INT32) * 2);

	/* ���� ���� �����Ϸ��� ����� ī�޶��� Z Axis ���̿� ���� �β� ��ŭ ���� or ���� ó�� */
	i32ACamDiffZ = (INT32)ROUNDED(pstRecipe->material_thick * 10.0f, 0) -
		(INT32)ROUNDED(pstAlign->dof_film_thick * 10000.0f, 0);

#if (USE_IO_LINK_PHILHMI == 0)

	for (; i < uvEng_GetConfig()->set_cams.acam_count; i++)
	{
		m_i32ACamZAxisSet[i] = (INT32)ROUNDED(pstACamFocus->acam_z_focus[i] * 10000.0f, 0) + i32ACamDiffZ;
		/* Align Camera Z Axis�� Min or Max �� ���� �ȿ� �ִ��� ���� */
		if ((INT32)ROUNDED(pstACamInfo->z_axis_move_min * 10000.0f, 0) > m_i32ACamZAxisSet[i] ||
			(INT32)ROUNDED(pstACamInfo->z_axis_move_max * 10000.0f, 0) < m_i32ACamZAxisSet[i])
		{
			/* Set the error message */
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Align Camera Z Axis movement is out of range");
			return ENG_JWNS::en_error;
		}
	}
	/* �̵��Ϸ��� ��ġ ���� */
	//if (!uvEng_MCQ_SetACamMoveAbsAllZ(m_i32ACamZAxisSet))
	//{
	//	return ENG_JWNS::en_error;
	//}
#elif (USE_IO_LINK_PHILHMI == 1)
	double		dACamDiffZ = 0;	/* ����: 0.1 um or 100 nm */
	double		dACamZAxisSet[2];
	/* ���� ���� �����Ϸ��� ����� ī�޶��� Z Axis ���̿� ���� �β� ��ŭ ���� or ���� ó�� */
	dACamDiffZ = pstRecipe->material_thick / 1000.0f - pstAlign->dof_film_thick;
	dACamZAxisSet[0] = (pstACamSpec->acam_z_focus[0] * 10000.0f) + i32ACamDiffZ;
	dACamZAxisSet[1] = (pstACamSpec->acam_z_focus[1] * 10000.0f) + i32ACamDiffZ;

	STG_PP_P2C_ABS_MOVE stSend;
	STG_PP_P2C_ABS_MOVE_ACK stRecv;
	stSend.Reset();
	stRecv.Reset();

	stSend.usCount = 2;
	sprintf_s(stSend.stMove[0].szAxisName, DEF_MAX_RECIPE_NAME_LENGTH, "ALIGN_CAMERA_Z1");
	sprintf_s(stSend.stMove[1].szAxisName, DEF_MAX_RECIPE_NAME_LENGTH, "ALIGN_CAMERA_Z2");
	//sprintf_s(stSend.stMove[0].szAxisName, DEF_MAX_RECIPE_NAME_LENGTH, "CAMERA_Z1");
	//sprintf_s(stSend.stMove[1].szAxisName, DEF_MAX_RECIPE_NAME_LENGTH, "CAMERA_Z2");
	stSend.stMove[0].dPosition = dACamZAxisSet[0] / 10000.0f;
	stSend.stMove[1].dPosition = dACamZAxisSet[1] / 10000.0f;
	stSend.stMove[0].dSpeed = uvEng_GetConfig()->mc2_svc.move_velo / 2;
	stSend.stMove[1].dSpeed = uvEng_GetConfig()->mc2_svc.move_velo / 2;
	stSend.stMove[0].dAcc = 300;
	stSend.stMove[1].dAcc = 300;

	/* �̵��Ϸ��� ��ġ ���� */
	uvEng_Philhmi_Send_P2C_ABS_MOVE(stSend, stRecv);
	lastUniqueID = stRecv.ulUniqueID;
	if (stRecv.usErrorCode)
	{
		/* Set the error message */
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Philhmi is ABS Move failed");
		return ENG_JWNS::en_error;
	}

#endif

	TCHAR tzMsg[256] = { NULL };
	swprintf_s(tzMsg, 256, L"Cam1 Z Axis : %.4f, Cam2 Z Axis : %.4f", stSend.stMove[0].dPosition, stSend.stMove[1].dPosition);
	LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);

	return ENG_JWNS::en_next;
}

/*
 desc : Align Camera Z Axis�� ���� ������ �� �Ǿ����� ���� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsACamZAxisMovedAll(unsigned long& lastUniqueID)
{
	UINT8 i = 0x00;
	INT32 i32ACamNoZ = 0;	/* ����: 0.1 um or 100 nm */
	//LPG_PMRW pstPLC	= uvEng_ShMem_GetPLCExt();

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.ACam.Z.Axis.Moved");

	/* ���� ���� ���� ������ ���ο� ���� */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

#if (USE_IO_LINK_PHILHMI == 0)
	/* ��� ����� ī�޶��� Z ���� ���ϴ� ��ġ�� ���� �ߴ��� ���� */
	//for (i=0x00; i<uvEng_GetConfig()->set_cams.acam_count; i++)
	//{
	//	if (0x00 == i)	i32ACamNoZ	= pstPLC->r_camera1_z_axis_position_display;
	//	else			i32ACamNoZ	= pstPLC->r_camera2_z_axis_position_display;
	//	if (i32ACamNoZ != m_i32ACamZAxisSet[i])
	//	{
	//		return ENG_JWNS::en_wait;
	//	}
	//}
#elif (USE_IO_LINK_PHILHMI == 1)
	STG_PP_PACKET_RECV stRecv = { 0, };
	stRecv.st_c2p_abs_move_comp.Reset();
	//if (FALSE == uvEng_Philhmi_GetRecvWaitFromUniqueID(lastUniqueID, &stRecv, 10000))
	//{
	//	// Alarm ó�� �ʿ�
	//	// 10�� Timeout�� ������ complete ��ȣ�� ���� ���� ��Ȳ
	//	LOG_ERROR(ENG_EDIC::en_uvdi15, L"Philhmi is ABS Move timeout");
	//}

	if (stRecv.st_c2p_abs_move_comp.usErrorCode)
	{
		/* Set the error message */
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Philhmi is ABS Move failed");

		return ENG_JWNS::en_error;
	}

#endif

	return ENG_JWNS::en_next;
}

/*
 desc : ��ũ �ν��� ��� �Ϸ�Ǿ����� Ȯ��
 parm : type	- [in]  Align Mark Type
		scan	- [in]  Align Scan�Ϸ��� ��ġ (��ȣ. 0 or 1)
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsSetMarkValid(ENG_AMTF type, UINT8 scan)
{
	TCHAR tzTitle[128] = { NULL };
	BOOL bSucc = FALSE;
	UINT8 u8Global, u8Local, u8Total;

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Is.SetMark.Valid");

	/* �Ź��� ��ϵ� Global & Local Fiducial Mark ���� */
	u8Global = uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	u8Local = uvEng_Luria_GetMarkCount(ENG_AMTF::en_local);
	/* Global Mark 4�� ��� �νĵǾ����� ���� Ȯ�� */
	if (ENG_AMTF::en_global == type)
	{
		u8Total = u8Global;
		bSucc = uvEng_Camera_GetGrabbedCount() == u8Global;
	}
	else
	{
		/* ��ü Local ���� �� 1 Scan ���� ����Ǵ� Mark ���� */
		if (0x00 == scan)	u8Total = u8Local / 2 + u8Global;
		else				u8Total = u8Local + u8Global;
		bSucc = uvEng_Camera_GetGrabbedCount() == u8Total;
	}

	/* Title ���� */
	swprintf_s(tzTitle, 128, L"Mark.Grabbed (%u / %u)", uvEng_Camera_GetGrabbedCount(), u8Total);
	SetStepName(tzTitle);

	return bSucc ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : �ѹ��� ��ũ�� ��� �ν� ��, ���������� ��ũ�� Ȯ�εǾ� �ִ��� ����
 parm : mode	- [in]  0x00 : Align Mark (Text), 0x01 : Align Mark (Expose)
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsSetMarkValidAll(UINT8 mode, bool* manualFixed, int* camNum)
{

	auto SetManualFix = [&](bool set)
		{
			if (manualFixed != nullptr)
				*manualFixed = set;
		};

	SetManualFix(false);

	TCHAR tzTitle[128] = { NULL };
	BOOL bSucc = FALSE, bMultiMark = FALSE;
	
	auto& motions = GlobalVariables::GetInstance()->GetAlignMotion();

	auto* config = uvEng_GetConfig();

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Is.SetMark.Valid.All");

	auto result = motions.IsNeedManualFixOffset(camNum);
	
	if (config->IsRunDemo())
	{
		bSucc = TRUE;
	}
	else
	{
		
		if (result == ENG_MFOR::noNeedToFix)
		{
			bSucc = TRUE;
		}
		else
		{

			if (result == ENG_MFOR::grabcountMiss)
			{
				SetStepName(L"Not all marks are grabbed.");
				return ENG_JWNS::en_error;
			}
				
			if (!config->set_align.use_invalid_mark_cali)  //�������� �ʰڴ�.
			{
				bSucc = FALSE;
			}
			else
			{
				if (config->set_align.manualFixOffsetAtSequence) //���� �����ϰڴ�. 
				{
					CDlgMmpm dlgMmpm;
					bSucc = (IDOK == dlgMmpm.DoModal());
					SetManualFix(bSucc);
							
					UpdateWaitingTime();
				}
				else //�ƴϴ� ������ �ϰڴ�. 
				{
					LOG_ERROR(ENG_EDIC::en_uvdi15, L"All found marks are invalid");
					bSucc = FALSE;
				}	
			}
		}
	}
	
	auto u8Global = motions.status.globalMarkCnt;
	auto u8Local = motions.markParams.alignType == ENG_ATGL::en_global_4_local_0_point ? 0 : motions.status.localMarkCnt;

	/* Title ���� */
	swprintf_s(tzTitle, 128, L"Mark.Grabbed (%u / %u)", uvEng_Camera_GetGrabbedCount(camNum), u8Global + u8Local);
	SetStepName(tzTitle);

	/*Mark Align ��� Log ���*/
	UINT8 u8Img, u8Cam;
	TCHAR tzMsg[256] = { NULL };
	LPG_ACGR pstMark = NULL;
	if (bSucc)
	{
		for (int i = 0; i < u8Global; i++)
		{
			/* Global Mark < 4 > Points */
			GetGlobalMarkIndex(i, u8Cam, u8Img);

			pstMark = uvEng_Camera_GetGrabbedMark(u8Cam, u8Img);
			if (pstMark)
			{
				swprintf_s(tzMsg, 256, L"[global ]Cam%d-Img%d, %.3f,%.3f,%.4f,%.4f,",
					u8Cam, u8Img,
					pstMark->score_rate, pstMark->scale_rate,
					pstMark->move_mm_x, pstMark->move_mm_y);
				LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
			}
		}
	}

	return bSucc ? ENG_JWNS::en_next : ENG_JWNS::en_error;
}

/*
 desc : ���ο� �Ź� ��� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetGerberRegist()
{

	auto ChangeXml = [&](string xmlPath, ENG_ATGL alignType)
		{
			auto SplitPath = [&](const std::string& filepath, std::string& directory, std::string& filename)
				{
					fs::path pathObj(filepath);
					directory = pathObj.parent_path().string();
					filename = pathObj.filename().string();
				};

			auto AddSuffixToFilename = [&](const std::string& filename, const std::string& suffix)->std::string
				{
					size_t dotPos = filename.find_last_of('.');
					if (dotPos != std::string::npos)
						return filename.substr(0, dotPos) + suffix + filename.substr(dotPos);
					return filename;
				};

			auto RemovenReplace = [&](string removeName, string replaceName)
				{
					fs::remove(removeName);
					fs::copy_file(replaceName, removeName);
				};

			string root, filename;
			SplitPath(xmlPath, root, filename);

			auto combineName = AddSuffixToFilename(filename, alignType == ENG_ATGL::en_global_4_local_0_point ? "_G" : "_GL");
			RemovenReplace(xmlPath, root + "\\" + combineName);
		};


	CHAR szGerbFile[MAX_PATH_LEN] = { NULL };
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	LPG_LDJM pstJobMgt = &uvEng_ShMem_GetLuria()->jobmgt;
	CUniToChar csCnv;

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.Gerber.Regist");

	/* ���� ����Ϸ��� Recipe�� ������ ���� */
	if (!IsRecipeGerberCheck())	return ENG_JWNS::en_error;

	/* ���� ����Ϸ��� �Ź��� Luria�� ��ϵǾ� �ִ��� ���� */
	if (pstJobMgt->IsJobOnlyFinded(pstRecipe->gerber_name))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"The gerber file to be registered exists");
		return ENG_JWNS::en_error;
	}

	/* ���� ���õ� �Ź� ���� ��� ���� */
	sprintf_s(szGerbFile, MAX_PATH_LEN, "%s\\%s", pstRecipe->gerber_path, pstRecipe->gerber_name);


	LPG_RAAF pstAlign = uvEng_Mark_GetSelectAlignRecipe();
	ENG_ATGL alignType = (ENG_ATGL)pstAlign->align_type;

	ChangeXml(string(szGerbFile) + "\\rlt_settings.xml", alignType);

	if (!uvEng_Luria_ReqAddJobList(csCnv.Ansi2Uni(szGerbFile)))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to regist the gerber file");
		return ENG_JWNS::en_error;
	}

	/* ��� ��, �ٷ� ��ϵ� Job List ��û */
	SetSendCmdTime();
	if (!uvEng_Luria_ReqGetJobList())
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetJobList)");
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : ���ο� �Ź� ��� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsGerberRegisted()
{
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	LPG_LDJM pstJobMgt = &uvEng_ShMem_GetLuria()->jobmgt;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Gerber.Registed");

	if (nullptr != pstJobMgt && nullptr != pstRecipe)
	{
		/* ���� ���õ� Recipe�� Job Name�� ��ϵǾ� �ִ��� ���� Ȯ�� */
		if (pstJobMgt->IsJobOnlyFinded(pstRecipe->gerber_name))
		{
			return ENG_JWNS::en_next;
		}
	}

	return ENG_JWNS::en_wait;
}

/*
 desc : Led Duty Cycle and Frame Rate ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetStepDutyFrame()
{
	LPG_LDEW pstExpo = &uvEng_ShMem_GetLuria()->exposure;
	CUniToChar csCnv;

	LPG_RJAF pstJobRecipe = uvEng_JobRecipe_GetSelectRecipe();
	//LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetSelectRecipe();
	LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstJobRecipe->expo_recipe));

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.Step.Duty.Frame");
	/* ���� ���� ������ ��� */
	if (uvEng_GetConfig()->IsRunDemo())
	{
		// by sysandj : ��������(����)
		pstJobRecipe->step_size = 4;
		pstExpoRecipe->led_duty_cycle = 10;
		// by sysandj : ��������(����)
		pstJobRecipe->frame_rate = 999;
	}
	/* Step Size, Duty Cycle �� Frame �� �ʱ�ȭ */
	pstExpo->SetDutyStepFrame();
	/* Step Size, Duty Cycle �� Frame �� ���� ��û */
	// by sysandj : ��������(����)
	if (pstJobRecipe->step_size <= 0 || pstExpoRecipe->led_duty_cycle < 10 || pstJobRecipe->frame_rate > 998)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to Set Step_Size Led_duty_cycle Frame_rate");
		return ENG_JWNS::en_error;
	}


	if (!uvEng_Luria_ReqSetExposureFactor(pstJobRecipe->step_size,
		pstExpoRecipe->led_duty_cycle,
		pstJobRecipe->frame_rate / 1000.0f))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetExposureFactor)");
		return ENG_JWNS::en_error;
	}

	TCHAR tzMsg[256] = { NULL };
	swprintf_s(tzMsg, 256, L"Step Size = %d Led Duty Cycle = %d, Frame Rate = %d",
		pstJobRecipe->step_size, pstExpoRecipe->led_duty_cycle, pstJobRecipe->frame_rate);
	LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);


	return ENG_JWNS::en_next;
}

/*
 desc : Led Duty Cycle and Frame Rate ���� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsStepDutyFrame()
{
	LPG_LDEW pstExpo = &uvEng_ShMem_GetLuria()->exposure;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Step.Duty.Frame");
	if (!pstExpo->IsSetDutyStepFrame())
	{
		return ENG_JWNS::en_wait;
	}
	return ENG_JWNS::en_next;
}

/*
 desc : �뱤 ���� ��ġ ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetExposeStartXY(double* startXoffset, double* startYoffset)
{
	DOUBLE* pStartXY = uvEng_GetConfig()->luria_svc.table_expo_start_xy[0];
	LPG_LDMC pstMachine = &uvEng_ShMem_GetLuria()->machine;


	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.Expose.Start.XY");

	/* ���� �뱤 ���� ��ġ ���� �����̸�, ���� ó�� */
	if (pStartXY[0] < 0.0f || pStartXY[1] < 0.0f)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"The exposure start position is incorrect");
		return ENG_JWNS::en_error;
	}

	/* �뱤 ���� ��ġ ���� �ʱ�ȭ */
	pstMachine->table_expo_start_xy[0].x = -1;
	pstMachine->table_expo_start_xy[0].y = -1;

	/* �۽� ���� */
	/* Led Duty Cycle & Frame Rate ��ġ */
	if (!uvEng_Luria_ReqSetTableExposureStartPos(0x01,
		startXoffset != nullptr ? pStartXY[0] + *startXoffset : pStartXY[0],
		startYoffset != nullptr ? pStartXY[1] + *startYoffset : pStartXY[1]))	/* table number is fixed */
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetExposeStartXY)");
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : �뱤 ���� ��ġ ���� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsExposeStartXY()
{
	LPG_LDMC pstMachine = &uvEng_ShMem_GetLuria()->machine;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Expose.Start.XY");

	/* �뱤 ���� ��ġ ���� Ȯ�� �Ǿ����� ���� */
	if (pstMachine->table_expo_start_xy[0].x >= 0 && pstMachine->table_expo_start_xy[0].y >= 0)
	{
		return ENG_JWNS::en_next;
	}

	return ENG_JWNS::en_wait;
}

/*
 desc : Photohead LED Amplitude �� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetLedAmplitude()
{
	TCHAR tzMesg[128] = { NULL };
	CUniToChar csCnv;
	UINT16 i = 0x00, u16LedPower[MAX_PH][4/* Not MAX_LED */] = { NULL };
	LPG_PLPI pstLedPower = NULL;
	//LPG_REAF pstRecipe = uvEng_ExpoRecipe_GetSelectRecipe();

	LPG_RJAF pstJobRecipe = uvEng_JobRecipe_GetSelectRecipe();
	//LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetSelectRecipe();
	LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstJobRecipe->expo_recipe));

	//LPG_LDEW pstExpose	= &uvEng_ShMem_GetLuria()->exposure;


	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.Led.Amplitude");

	/* �����ǿ� ������ Led Power Name�� �̿��� Led Power Index ��ȸ */
	pstLedPower = uvEng_LedPower_GetLedPowerName(csCnv.Ansi2Uni(pstExpoRecipe->power_name));
	if (!pstLedPower)
	{
		swprintf_s(tzMesg, 128, L"Failed to get the Led Power Index (for %s name)",
			csCnv.Ansi2Uni(pstExpoRecipe->power_name));
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
		return ENG_JWNS::en_error;
	}

	/* ���� ������ Power �� �ʱ�ȭ */
	//pstExpose->ResetPowerIndex();

	/* ���� ������ Recipe �Ŀ� �� ���� */
	for (i = 0x00; i < uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
#ifdef _DEBUG
		const int MAX_ALLOW_POWER_INDEX = 3000;
		pstLedPower->led_index[i][0] = pstLedPower->led_index[i][0] > MAX_ALLOW_POWER_INDEX ? 100 : pstLedPower->led_index[i][0];
		pstLedPower->led_index[i][1] = pstLedPower->led_index[i][1] > MAX_ALLOW_POWER_INDEX ? 100 : pstLedPower->led_index[i][1];
		pstLedPower->led_index[i][2] = pstLedPower->led_index[i][2] > MAX_ALLOW_POWER_INDEX ? 100 : pstLedPower->led_index[i][2];
		pstLedPower->led_index[i][3] = pstLedPower->led_index[i][3] > MAX_ALLOW_POWER_INDEX ? 100 : pstLedPower->led_index[i][3];
#endif
		memcpy(&u16LedPower[i], pstLedPower->led_index[i], sizeof(UINT16) * 4/* Not MAX_LED */);
	}
	/* ��� �ð� �ʱ�ȭ */
	SetSendCmdTime();
	/* Led Duty Cycle ���� (�� Phothead x���� ���� LED 4���� ��� �ѹ��� ���� */
	if (!uvEng_Luria_ReqSetLedAmplitude(uvEng_GetConfig()->luria_svc.ph_count, u16LedPower))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetLedAmplitude)");
		return ENG_JWNS::en_error;
	}

	TCHAR tzMsg[256] = { NULL };
	for (i = 0x00; i < uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		swprintf_s(tzMsg, 256, L"PH%d: LED Index [%d][%d][%d][%d]", i + 1, pstLedPower->led_index[i][0], pstLedPower->led_index[i][1], pstLedPower->led_index[i][2], pstLedPower->led_index[i][3]);
		LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
	}

	return ENG_JWNS::en_next;
}

/*
 desc : Photohead LED Amplitude �� ���� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsLedAmplituded()
{
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	LPG_LDEW pstExpose = &uvEng_ShMem_GetLuria()->exposure;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Led.Amplituded");

	if (!pstExpose->IsSetPowerIndex(uvEng_GetConfig()->luria_svc.ph_count))
	{
		/* 1.0 �ʸ��� ���� Led Amplitude ��û */
		if (IsSendCmdTime(1000))	uvEng_Luria_ReqGetLedAmplitude();
		return ENG_JWNS::en_wait;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : ��� Photohead�� Z Axis�� �⺻ ��ġ�� �̵� �ϱ�
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetAllPhMotorMoving()
{
	BOOL bSucc = TRUE;
	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.All.Optic.Motor.Base.Moving");

	/* ���� ����� ������ ���� Focus ��ġ�� �̵� */
	if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x01)
	{
		bSucc = uvEng_Luria_ReqSetAllMotorBaseAbsPosition();
		/* ��� �ð� �ʱ�ȭ */
		SetSendCmdTime();
	}
	else if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x03)
	{
		//uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_axis_ph1);
		//uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_axis_ph2);
		//bSucc	= uvEng_MC2_ReqSetPhMotorFocusMove(ENG_MMDI::en_axis_ph1) &&
		//		  uvEng_MC2_ReqSetPhMotorFocusMove(ENG_MMDI::en_axis_ph2);

		int i;
		UINT8 u8drv_id;
		for (i = 0; i < uvEng_GetConfig()->luria_svc.ph_count; i++)
		{
			u8drv_id = i + DRIVEDIVIDE;
			uvCmn_MC2_GetDrvDoneToggled((ENG_MMDI)u8drv_id);
			bSucc = uvEng_MC2_ReqSetPhMotorFocusMove((ENG_MMDI)u8drv_id) && bSucc;
		}
	}
	if (!bSucc)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetAllMotorBaseAbsPosition)");
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : ��� Photohead�� Z Axis�� �⺻ ��ġ�� �̵� �ߴ��� ���� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsAllPhMotorMoved()
{
	UINT8 i = 0;
	INT32 i32Set = 0, i32Get = 0;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.All.Optic.Motor.Moved");
	if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x01)
	{
		/* �ʹ� ���� ��û�ϸ� �ȵǹǷ� ... */
		if (IsSendCmdTime(250))
		{
			/* ���� ���� ��ġ ���� ��û (��ü) */
			if (!uvEng_Luria_ReqGetMotorAbsPositionAll())
			{
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetMotorAbsPositionAll)");
				return ENG_JWNS::en_error;
			}
		}
		/* Photohead ������ŭ �⺻ ��ġ �̵� �Ϸ� ���� Ȯ�� */
		return uvCmn_Luria_IsAllPhMotorFocusMoved() ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
	}
	else if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x03)
	{
#if 0
		return uvCmn_MC2_IsAllPhMotorFocusMoved() ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
#else
		UINT8 u8drv_id;
		int i;
		BOOL bSucc = TRUE;
		for (i = 0; i < uvEng_GetConfig()->luria_svc.ph_count; i++)
		{
			u8drv_id = i + DRIVEDIVIDE;
			bSucc = uvCmn_MC2_IsDrvDoneToggled((ENG_MMDI)u8drv_id) && bSucc;
		}

		if (bSucc)	ENG_JWNS::en_next;
		else        ENG_JWNS::en_wait;
		//return uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_axis_ph1) &&
		//	   uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_axis_ph1) ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
#endif


	}

	return ENG_JWNS::en_wait;
}

/*
 desc : ��� Photohead�� Z Axis ���� �ʱ�ȭ (Homing)
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetAllPhMotorHoming()
{
#ifndef _DEBUG
	LPG_LDDP pstDirectPh = &uvEng_ShMem_GetLuria()->directph;

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.All.Optic.Motor.Homing");
	if (uvEng_GetConfig()->set_uvdi15.load_recipe_homing)
	{
		if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x01)
		{
			/* ���� ���а� ��ΰ� �ʱ�ȭ �ȵǾ��ٰ� ���� */
			pstDirectPh->SetAllMotorInited(uvEng_GetConfig()->luria_svc.ph_count, FALSE);
			/* Z Axis ���͸� �ʱ�ȭ�ϵ��� ��û */
			if (!uvEng_Luria_ReqSetMotorPositionInitAll(0x01))
			{
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetMotorPositionInitAll)");
				return ENG_JWNS::en_error;
			}
			/* ��� �ð� �ʱ�ȭ */
			SetSendCmdTime();
		}
		else if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x03)
		{
			if (!uvEng_MC2_SendDevHoming(ENG_MMDI::en_axis_ph1) ||
				!uvEng_MC2_SendDevHoming(ENG_MMDI::en_axis_ph2))
			{
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SendDevHoming(PH1 and/or PH2)");
				return ENG_JWNS::en_error;
			}
		}
	}
#endif
	return ENG_JWNS::en_next;
}

/*
 desc : ��� Photohead�� Z Axis ���Ͱ� �ʱ�ȭ �Ϸ� �Ǿ����� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsAllPhMotorHomed()
{
#ifndef _DEBUG
	LPG_LDDP pstDirectPh = &uvEng_ShMem_GetLuria()->directph;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.All.Optic.Motor.Homed");

	if (!uvEng_GetConfig()->set_uvdi15.load_recipe_homing)	return ENG_JWNS::en_next;

	if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x01)
	{
		/* ��� ���а谡 �ʱ�ȭ �Ǿ����� ���� Ȯ�� */
		if (uvCmn_Luria_IsAllMotorMiddleInited(uvEng_GetConfig()->luria_svc.ph_count))
		{
			return ENG_JWNS::en_next;
		}
		if (IsSendCmdTime(250))
		{
			/* ���� ��� photohead�� ���� ���� ���� ��û */
			if (!uvEng_Luria_ReqGetMotorStateAll())
			{
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetMotorStateAll)");
				return ENG_JWNS::en_error;
			}
		}
	}
	else if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x03)
	{
		if (uvCmn_MC2_IsDriveHomed(ENG_MMDI::en_axis_ph1) &&
			uvCmn_MC2_IsDriveHomed(ENG_MMDI::en_axis_ph2))
		{
			return ENG_JWNS::en_next;
		}
	}
	return ENG_JWNS::en_wait;
#else
	return ENG_JWNS::en_next;
#endif
}

/*
 desc : �⺻������ ����̺꿡 ���� ���� ������, Homing ��û ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::InitDriveErrorReset()
{
	UINT8 i = 0x00;

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Init.Drive.Error.Reset");

	/* ���� ��� ����̺� �� 1���� ���� �ڵ尡 �����Ѵٸ�, ���� ���� ó�� */
	for (; i < uvEng_GetConfig()->mc2_svc.drive_count; i++)
	{
		/* �����, ������ �߻��� ����̺��� ���, �ݵ�� ���� ȣ�� �� Homing �۾� �����ؾ� �� */
		/* �ᱹ�� ���� �ڵ常 Reset ��Ű�� �����̰�, ������ Homing �۾��� �����ؾ� �� */

		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI(uvEng_GetConfig()->mc2_svc.axis_id[i]));	/* Toggle �� ���� */
		if (!uvEng_MC2_SendDevFaultReset(ENG_MMDI(uvEng_GetConfig()->mc2_svc.axis_id[i])))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SendDevFaultReset)");
			ENG_JWNS::en_error;
		}

	}

#if (MC2_DRIVE_2SET == 1)
	UINT8 u8drv_id;
	/* ���� ������� ����̺� �� 1���� ���� �ڵ尡 �����Ѵٸ�, ���� ���� ó�� */
	for (i = 0; i < uvEng_GetConfig()->mc2b_svc.drive_count; i++)
	{
		u8drv_id = i + DRIVEDIVIDE;
		/* �����, ������ �߻��� ����̺��� ���, �ݵ�� ���� ȣ�� �� Homing �۾� �����ؾ� �� */
		/* �ᱹ�� ���� �ڵ常 Reset ��Ű�� �����̰�, ������ Homing �۾��� �����ؾ� �� */
		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI(u8drv_id));	/* Toggle �� ���� */
		if (!uvEng_MC2_SendDevFaultReset(ENG_MMDI(u8drv_id)))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SendDevFaultReset)");
			ENG_JWNS::en_error;
		}

	}
#endif

	return ENG_JWNS::en_next;
}

/*
 desc : ��� ����̺��� ���� ���� �ʱ�ȭ �Ǿ����� Ȯ��
 parm : back_step	- [in]  �ڷ� �̵� �� �ٽ� �۾� �����ϴ� �ܰ� Ƚ��
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsDriveErrorReseted(UINT8 back_step)
{
	UINT8 i = 0x00;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Init.Drive.Error.Reset");

	/* ���� MC2 ����̺� �ʿ� ������ ��� ���� �ִ� �������� ���� Ȯ��*/
	if (uvCmn_MC2_IsAnyDriveError())	return ENG_JWNS::en_wait;
	/* ���� ��� ����̺갡 DoneToggled �Ǿ����� Ȯ�� */
	for (; i < uvEng_GetConfig()->mc2_svc.drive_count; i++)
	{
		if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI(uvEng_GetConfig()->mc2_svc.axis_id[i])))	return ENG_JWNS::en_wait;
	}

#if (MC2_DRIVE_2SET == 1)
	UINT8 u8drv_id;
	for (i = 0; i < uvEng_GetConfig()->mc2b_svc.drive_count; i++)
	{
		u8drv_id = i + DRIVEDIVIDE;
		if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI(u8drv_id)))	return ENG_JWNS::en_wait;
	}
#endif

	return ENG_JWNS::en_next;
}

/*
 desc : ���� ����̺꿡 ���ؼ� Homing �۾� ����
 parm : drv_id	- [in]  Motion Drive ID
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::DoDriveHoming(ENG_MMDI drv_id)
{
	/* ���� �۾� Step Name ���� */
	switch (drv_id)
	{
	case ENG_MMDI::en_stage_x:	SetStepName(L"Do.Homing.Stage.X");	break;
	case ENG_MMDI::en_stage_y:	SetStepName(L"Do.Homing.Stage.Y");	break;
	case ENG_MMDI::en_align_cam1:	SetStepName(L"Do.Homing.ACam.1");	break;
	case ENG_MMDI::en_align_cam2:	SetStepName(L"Do.Homing.ACam.2");	break;
	case ENG_MMDI::en_axis_ph1:	SetStepName(L"Do.Homing.PH.1");		break;
	case ENG_MMDI::en_axis_ph2:	SetStepName(L"Do.Homing.PH.2");		break;
	}

#if 0	/* ���⼭�� ����̺� ���� üũ���� ���� */
	/* ���� ���� ����̺꿡 ���� �ڵ尡 �����ϸ�, Homing �۾� ���� ó�� */
	if (uvData_MC2_IsDriveError(drv_id))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, en_edic_proc, L"Failed to recover error of certain motion drive");
		return en_bwos_error;
	}
#endif
	if (!uvEng_MC2_SendDevHoming(drv_id))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SendDevHoming)");
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : ��� ����̺꿡 ���ؼ� Homing �۾� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::DoDriveHomingAll()
{
	/* ���� �۾� Step Name ���� */
	SetStepName(L"Do.Homing.All");

#if 0	/* ���⼭�� ����̺� ���� üũ���� ���� */
	/* ���� ���� ����̺꿡 ���� �ڵ尡 �����ϸ�, Homing �۾� ���� ó�� */
	if (uvData_MC2_IsDriveError(drv_id))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, en_edic_proc, L"Failed to recover error of certain motion drive");
		return en_bwos_error;
	}
#endif
	if (!uvEng_MC2_SendDevHomingAll())
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SendDevHomingAll)");
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : ���� ��� ����̺갡 Homing �Ϸ� �Ǿ����� ����
 parm : drv_id	- [in]  Motion Drive ID (EN_MMDI)
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsDrivedHomed(ENG_MMDI drv_id)
{
	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())
	{
		switch (drv_id)
		{
		case ENG_MMDI::en_stage_x:	SetStepName(L"Is.Homed.Stage.X");	break;
		case ENG_MMDI::en_stage_y:	SetStepName(L"Is.Homed.Stage.Y");	break;
		case ENG_MMDI::en_align_cam1:	SetStepName(L"Is.Homed.ACam.1");	break;
		case ENG_MMDI::en_align_cam2:	SetStepName(L"Is.Homed.ACam.2");	break;
		case ENG_MMDI::en_axis_ph1:	SetStepName(L"Is.Homed.PH.1");		break;
		case ENG_MMDI::en_axis_ph2:	SetStepName(L"Is.Homed.PH.2");		break;
		}
	}

	if (!uvCmn_MC2_IsDriveHomed(drv_id))
	{
		/* ��¿�� ���� �ð� ���� (���� Homing �ȵ� �� ���� ��� ��) */
		m_u64DelayTime = GetTickCount64();
		return ENG_JWNS::en_wait;
	}
	return ENG_JWNS::en_next;
}

/*
 desc : ��� ����̺갡 Homing �Ǿ����� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsDrivedHomedAll()
{
	UINT8 i = 0x00, u8Count = uvEng_GetConfig()->luria_svc.z_drive_type == 0x01 ? 0x04 : 0x06;
	ENG_MMDI enDrvID[6] = { ENG_MMDI::en_stage_x, ENG_MMDI::en_stage_y,
						   ENG_MMDI::en_align_cam1, ENG_MMDI::en_align_cam2,
						   ENG_MMDI::en_axis_ph1, ENG_MMDI::en_axis_ph2 };
	LPG_MDSM pstMC2 = uvEng_ShMem_GetMC2();

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Homed.Drive.All");

	for (; i < u8Count/*uvEng_GetConfig()->mc2_svc.drive_count*/; i++)
	{
		if (!uvCmn_MC2_IsDriveHomed(enDrvID[i]))
		{
			return ENG_JWNS::en_wait;
		}
	}

	return ENG_JWNS::en_next;
}

/*
 desc : Align Camera 2���� Homing ���� Limit�� üũ�� ��, 1100 ���� Ȯ��
 parm : cam_no	- [in]  Align Camera Number (1 or 2)
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::WaitACamError(UINT8 cam_no)
{
	ENG_MMDI enDrvID = cam_no == 1 ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;
	LPG_MDSM pstMC2 = uvEng_ShMem_GetMC2();

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Wait.Camera.Error.Check");

	/* ���� ���� ���� ������ ���ο� ���� */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

	/* ���࿡ �̹� Homing �Ϸ�Ǿ��ٸ� ��¿ �� ����. �׳� ���� �ܰ�� �Ѿ�� �ȴ�. */
	if (uvCmn_MC2_IsDriveHomed(enDrvID))
	{
		return ENG_JWNS::en_next;
	}

	/* Align Camera x�� 1100 ���� (Homing ���� Limit ���� �� ��, �߻��Ǵ� ���� ��) Ȯ�� */
	/* ���� �̶�, ��Ÿ �ٸ� ����̺굵 ��� Homing ������ �߻��Ǵ� ��Ȳ. ������ */
	if (0x1100 == uvCmn_MC2_GetDriveError(enDrvID))
	{
		return ENG_JWNS::en_next;
	}

	return ENG_JWNS::en_wait;
}

/*
 desc : Align Camera 2���� Homing ���� ������ Homing ���� ��Ŵ
 parm : None
 retn : wait, error, complete or next
 note : ī�޶� 1���� �浹 �߻��� ���� �� �ֱ� ������... (Linear Scale�� �����ϱ� ������ T.T)
*/
ENG_JWNS CWorkStep::ACam2HomingStop()
{
	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Align.Camera.2.Homing.Stop");

	/* ���� ���� ���� ������ ���ο� ���� */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

	if (0x1100 == uvEng_ShMem_GetMC2()->GetErrorCodeOne(UINT8(ENG_MMDI::en_align_cam2)))
	{
		/* ������ �̵� �ߴ� */
		if (!uvEng_MC2_SendDevStopped(ENG_MMDI::en_align_cam2))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SendDevStopped:en_align_cam2)");
			return ENG_JWNS::en_error;
		}
		return ENG_JWNS::en_next;
	}

	return ENG_JWNS::en_wait;
}

/*
 desc : Luria ���� System - Initialize.Hardware ���� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::InitializeHWInit()
{
	/* ���� �۾� Step Name ���� */
	SetStepName(L"Initialized.H/W.Init");

	/* ���� Luria Service�� ����Ǿ� �ִ��� ���� */
	if (!uvCmn_Luria_IsConnected())
	{
		return ENG_JWNS::en_wait;
	}
	/* �۽� �۾� ���� */
	if (!uvEng_Luria_ReqSetHWInit())
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetHWInit)");
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : QuartZ ��ġ�� �ִ� ���� Align Camera�� �� ���̵��� Z AXis ��ġ (Focusing) �̵�
 parm : title	- [in]  ���� ���� �ܰ� ��� ����
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetACamMovingZAxisQuartz()
{
	UINT8 u8CamID = (m_enDrvACamID == ENG_MMDI::en_align_cam1) ? 0x01 : 0x02;
	DOUBLE dbACamSetZ = 0.0f;

	/* ���� �۾� Step Name ���� */
	SetStepName(L"ACam.Moving.Z.Axis.Quartz");

	/* Quartz �� �°� Focusing ��ġ �̵� */
	dbACamSetZ = uvEng_GetConfig()->acam_spec.acam_z_focus[u8CamID - 1];
	/* Align Camera Z Axis�� ���� ��ġ�� �̵� */
	// by sysandj : MCQ��ü �߰� �ʿ�
// 	if (!uvEng_MCQ_SetACamMovePosZ(u8CamID, 0x00, dbACamSetZ))
// 	{
// 		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ACamMovePosZ)");
// 		return ENG_JWNS::en_error;
// 	}

	return ENG_JWNS::en_next;
}

/*
 desc : Quartz �� Z Axis�� ��� �̵� �ߴ��� ���� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsACamMovedZAxisQuartz()
{
	UINT8 u8CamID = (m_enDrvACamID == ENG_MMDI::en_align_cam1) ? 0x01 : 0x02;
	INT32 i32ACamSetZ = 0, i32ACamNowZ = 0;
	//LPG_PMRW pstPLC		= uvEng_ShMem_GetPLCExt();

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.ACam.Moved.Z.Axis.Quartz");

	/* ���� Align Camera Z ���� ��ġ �� (0.1 um or 100 nm --> um) */
	// by sysandj : MCQ��ü �߰� �ʿ�
	//if (0x01 == u8CamID)	i32ACamNowZ	= (INT32)ROUNDED(pstPLC->r_camera1_z_axis_position_display / 10.0f, 0);
	//else					i32ACamNowZ	= (INT32)ROUNDED(pstPLC->r_camera2_z_axis_position_display / 10.0f, 0);

	/* ���� �̵��ؾ� �� Align Camera�� Z ��ġ �� (����: mm --> um) */
	i32ACamSetZ = (INT32)ROUNDED(uvEng_GetConfig()->acam_spec.acam_z_focus[u8CamID - 1] * 1000.0f, 0);

	/* ���� �̵��ϰ��� �ߴ� ��ġ ���� ���� �̵��� ��ġ �� �� (1 um ������ ��) */
	if (i32ACamSetZ != i32ACamNowZ)
	{
		return ENG_JWNS::en_wait;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : ���� �̵��ϰ��� �ϴ� ī�޶� �ƴ� ���, �� ���� or ������ ������ �̵� ó��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetACamMovingSide()
{
	INT32 i32MovePos = 0;
	UINT32 u32MoveSpeed = 0;
	ENG_MMDI enACamDrv = (m_enDrvACamID == ENG_MMDI::en_align_cam1) ? ENG_MMDI::en_align_cam2 : ENG_MMDI::en_align_cam1;

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.ACam.Moving.Side");

	/* Align Camera 2 */
	if (enACamDrv == ENG_MMDI::en_align_cam2)
	{
		i32MovePos = (INT32)ROUNDED(uvEng_GetConfig()->mc2_svc.max_dist[UINT8(enACamDrv)] * 10000.0f, 0);
	}
	/* Align Camera 1 */
	else
	{
		i32MovePos = (INT32)ROUNDED(uvEng_GetConfig()->mc2_svc.min_dist[UINT8(enACamDrv)] * 10000.0f, 0);
	}
	u32MoveSpeed = (INT32)ROUNDED(uvEng_GetConfig()->mc2_svc.max_velo[UINT8(enACamDrv)] * 10000.0f, 0);

	/* ���� �̵��ϰ��� �ϴ� ����� Toggled �� ��� */
	uvCmn_MC2_GetDrvDoneToggled(enACamDrv);
	/* ���̵�� �̵� ��Ű�� */
	if (!uvEng_MC2_SendDevAbsMove(enACamDrv,
		i32MovePos / 10000.0f,
		u32MoveSpeed / 10000.0f))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SendDevAbsMove)");
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : �� ������ Align Camera�� �̵��ߴ��� ���� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsACamMovedSide()
{
	ENG_MMDI enACamDrv = (m_enDrvACamID == ENG_MMDI::en_align_cam1) ? ENG_MMDI::en_align_cam2 : ENG_MMDI::en_align_cam1;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.ACam.Moved.Side");

	if (!uvCmn_MC2_IsDrvDoneToggled(enACamDrv))
	{
		return ENG_JWNS::en_wait;
	}
	return ENG_JWNS::en_next;
}

/*
 desc : Align Camera�� Quartz�� �ִ� ��ġ�� �̵�
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetACamMovingQuartz()
{
	UINT8 u8CamID = (m_enDrvACamID == ENG_MMDI::en_align_cam1) ? 0x01 : 0x02;
	DOUBLE dbACamMovePos = 0.0f, dbXMovePos = 0.0f, dbYMovePos = 0.0f;
	DOUBLE dbACAmSpeed = 0.0f, dbXMoveSpeed = 0.0f, dbYMoveSpeed = 0.0f;
	LPG_CASI pstACamSpec = &uvEng_GetConfig()->acam_spec;

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.ACam.Moving.Quartz");

	/* Quartz ��ġ */
	dbXMovePos = pstACamSpec->quartz_stage_x;
	dbYMovePos = pstACamSpec->quartz_stage_y[u8CamID - 1];
	dbACamMovePos = pstACamSpec->quartz_acam[u8CamID - 1];
	/* Speed */
	dbXMoveSpeed = uvEng_GetConfig()->mc2_svc.max_velo[UINT8(ENG_MMDI::en_stage_x)];
	dbYMoveSpeed = uvEng_GetConfig()->mc2_svc.max_velo[UINT8(ENG_MMDI::en_stage_y)];
	dbACAmSpeed = uvEng_GetConfig()->mc2_svc.max_velo[UINT8(m_enDrvACamID)];

	/* ���� �̵��ϰ��� �ϴ� ����� Toggled �� ��� */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	uvCmn_MC2_GetDrvDoneToggled(m_enDrvACamID);

	/* Motion �̵� ��Ű�� */
	if (!uvEng_MC2_SendDevAbsMove(m_enDrvACamID, dbACamMovePos, dbACAmSpeed))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SendDevAbsMove.ACamID)");
		return ENG_JWNS::en_error;
	}
	if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_x, dbXMovePos))
	{
		return ENG_JWNS::en_error;
	}
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, dbXMovePos, dbXMoveSpeed))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SendDevAbsMove.StageX)");
		return ENG_JWNS::en_error;
	}
	if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_y, dbYMovePos))
	{
		return ENG_JWNS::en_error;
	}
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dbYMovePos, dbYMoveSpeed))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SendDevAbsMove.StageY)");
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : Quartz ��ġ�� ��� �̵��� �Ϸ� �Ǿ����� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsACamMovedQuartz()
{
	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.ACam.Moved.Quartz");

	if (!uvCmn_MC2_IsDrvDoneToggled(m_enDrvACamID))			return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_x))	return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : �뱤�� �غ� �Ǿ� �ִ��� ��� �� ������ Ȯ��
 parm : in_mark	- [in]  TRUE:��ũ ���� ���� �˻�, FALSE:��ũ Ȯ������ ����
		detect	- [in]  ���� �˻� ����
		vaccum	- [in]  Vacuum �˻� ����
		count	- [in]  �뱤 Ƚ��
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetExposeReady(BOOL in_mark, BOOL detect, BOOL vaccum, UINT32 count)
{
	STG_CGTI stTrans = { NULL };
	LPG_CGTI pstTrans = &uvEng_GetConfig()->global_trans;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Init.Processing");

	/* ���� �Ź��� ����Ǿ� �ִ��� Ȯ�� */
	if (!uvCmn_Luria_IsJobNameLoaded())
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"The gerber file did not complete loading");
		return ENG_JWNS::en_error;
	}
	/* ���� Loading �Ǿ� �ִ� �Ź��� Mark�� ���ԵǾ� �ִ��� Ȯ�� */
	if (in_mark && uvEng_Luria_GetMarkCount(ENG_AMTF::en_global) < 1)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"The global mark does not exist");
		return ENG_JWNS::en_error;
	}
	/* ���� �뱤 �۾� (PrePrinting, Printing, Aborting) ���� ���̸�, ���� ���� ó�� */
	if (!uvEng_Luria_ReqSetPrintOpt(0x02))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetPrintOpt.Abort)");
		return ENG_JWNS::en_error;
	}
	/* MC2 ���� ���� Ȯ�� */
	if (IsMC2ErrorCheck())					return ENG_JWNS::en_error;
	/* Recipe ���� ���� Ȯ�� */
	if (!IsSelectedRecipeValid())			return ENG_JWNS::en_error;
	/* Luria ���� ���� Ȯ�� */
	if (IsLuriaStatusError())				return ENG_JWNS::en_error;
	/* Mark ��� ���� Ȯ�� */
	if (in_mark && !IsAlignMarkRegisted())	return ENG_JWNS::en_error;
	/* �뱤�� ���簡 ���� �ִ��� Ȯ�� */
	//if (!uvCmn_MCQ_IsMaterialDetected() && detect)
	//{
	//	LOG_ERROR(ENG_EDIC::en_uvdi15, L"No material to be exposed was detected");
	//	return ENG_JWNS::en_error;
	//}
	LPG_LDEW pstExpo = &uvEng_ShMem_GetLuria()->exposure;

	if (pstExpo->frame_rate_factor > 999)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to frame rate factor");
		return ENG_JWNS::en_error;
	}

	if (!uvEng_GetConfig()->IsRunDemo())
	{
		/* ��ϵ� Ʈ���� ��ġ ���� �ʱ�ȭ �Ǿ����� ���� */
		//if (!IsTrigPosResetAll())	return ENG_JWNS::en_wait;
		/* ���а� ��ġ�� Home Position�� �ִ��� ���� ... (���а�2���� ���̰� ��� ������ ������ ��) */
		//if (uvEng_ShMem_GetLuria()->IsAllEqualPHZAxis(uvEng_GetConfig()->luria_svc.ph_count))
		//{
		//	LOG_ERROR(ENG_EDIC::en_uvdi15, L"Check the z axis height of all photoheads");
		//	return ENG_JWNS::en_error;
		//}
		/* ��� ����̺� ���� ���� Ȯ�� */
		if (!uvCmn_MC2_IsMotorDriveStopAll())
		{
			if (count < 2)	LOG_WARN(ENG_EDIC::en_uvdi15, L"Any motor is not stopped");
			return ENG_JWNS::en_wait;
		}

#if (CUSTOM_CODE_WAIT_JOB == 0)
		/* Vacuum�� On ���°� �ƴ��� ���� Ȯ�� */
		if (vaccum && !IsVacuumRunning())
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"The vacuum is not running");
			return ENG_JWNS::en_error;
		}
		/* Chiller�� On ���°� �ƴ��� ���� Ȯ�� */
		if (!IsChillerRunning())
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"The chiller is not running");
			return ENG_JWNS::en_error;
		}
		/* ��� ������ Start Button 2 ���� LED�� On �Ǿ����� ���� */
		// by sysandj : MCQ��ü �߰� �ʿ�
// 		if (!uvCmn_MCQ_IsStartButtonLedOn())
// 		{
// 			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Check the LED (Turn-on) on the start button of the front panel");
// 			return ENG_JWNS::en_error;
// 		}
#endif
		/* Reset the global transformation information */
		uvEng_ShMem_GetLuria()->panel.ResetGlobalTransRecipe();
		if (m_enWorkJobID == ENG_BWOK::en_expo_align)
		{
			memcpy(&stTrans, pstTrans, sizeof(STG_CGTI));
		}
		/* Set the Global rectangle lock */
		if (!uvEng_Luria_ReqSetGlobalRectangle(stTrans.use_rectangle))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the SetRectangleLock for Global fiducial");
			return ENG_JWNS::en_error;
		}
		/* Set the Global Transformation Information */
		if (!uvEng_Luria_ReqSetGlobalTransformationRecipe(stTrans.use_rotation_mode,
			stTrans.use_scaling_mode,
			stTrans.use_offset_mode))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the SetTransformationRecipe (Auto ? Fixed) for Global fiducial");
			return ENG_JWNS::en_error;
		}
		if (!uvEng_Luria_ReqSetGlobalFixed(stTrans.rotation / 1000.0f,
			stTrans.scaling[0] / 1000000.0f, stTrans.scaling[1] / 1000000.0f,
			stTrans.offset[0] / 1000000.0f, stTrans.offset[1] / 1000000.0f))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the SetTransformationRecipe Value for Global fiducial");
			return ENG_JWNS::en_error;
		}
	}

	return ENG_JWNS::en_next;
}

/*
 desc : ����� ī�޶� 2���� �� ���� �ڸ��� �̵�
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetMovingSideACam2()
{
	INT32 i32ACamPos2 = 0;
	UINT32 u32VeloACamX;

	SetStepName(L"Set.Moving.Side.ACam2");
	/* ���� ��� ����̺��� Toggle ���� �� ���� */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam2);

	/* �� Axis ���� �⺻ ���� �ӵ� �� ��� */
	u32VeloACamX = (UINT32)ROUNDED(uvEng_GetConfig()->mc2_svc.max_velo[UINT8(ENG_MMDI::en_align_cam2)] * 10000.0f, 0);
	/* 2 �� ī�޶� ��ġ �� ���������� �̵� ��Ŵ */
	i32ACamPos2 = (INT32)ROUNDED(uvEng_GetConfig()->mc2_svc.max_dist[UINT8(ENG_MMDI::en_align_cam2)] * 10000.0f, 0);	/* 0.1 um or 100 ns */
	/* Align Camera �̵� �ϱ� */
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2,
		i32ACamPos2 / 10000.0f, u32VeloACamX / 10000.0f))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SendDevAbsMove)");
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : ����� ī�޶� 2���� �� �̵� �Ǿ����� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsMovedSideACam2()
{
	/* ���� �ð� ��� (ī�޶� ���Ͱ� ������ ������ ���� �ð� ���) */
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam2))
	{
		return ENG_JWNS::en_wait;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : ����� ī�޶� 1���� Ư�� Mark Number ��ġ�� �̵�
 parm : mark_no	- [in]  Mark Number (0x00 ~ 0x03)
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetMovingMarkACam1(UINT8 mark_no)
{
	TCHAR tzStepName[64] = { NULL };
	INT32 i32PosStageY = 0, i32PosACamX = 0;
	UINT32 u32VeloACamX, u32VeloStageY;

	swprintf_s(tzStepName, 64, L"Moving.Mark.Pos (%d)", mark_no);
	SetStepName(tzStepName);

	/* ���� ��� ����̺��� Toggle ���� �� ���� */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);

	/* ���� �̵��ϰ��� �ϴ� ��� ��ġ �� ��� */
	GetMovePosGlobalMarkACam1(mark_no, i32PosACamX, i32PosStageY);
	/* �̵��ϰ��� �ϴ� ��ġ ���� �ӽ� ��� */
	uvEng_ACamCali_AddMarkPos(1, ENG_AMTF::en_global, 0x00, mark_no, i32PosACamX);
	uvEng_ACamCali_AddMarkPos(1, ENG_AMTF::en_global, 0x01, mark_no, GetGlobalMarkMotionPosY(mark_no));
	/* �� Axis ���� �⺻ ���� �ӵ� �� ��� */
	u32VeloACamX = (UINT32)ROUNDED(uvEng_GetConfig()->mc2_svc.max_velo[UINT8(ENG_MMDI::en_align_cam1)] * 10000.0f, 0);
	u32VeloStageY = (UINT32)ROUNDED(uvEng_GetConfig()->mc2_svc.max_velo[UINT8(ENG_MMDI::en_stage_y)] * 10000.0f, 0);
	/* ������ Align Camera 1���� Stage Y �̵� �ϱ� */
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1,
		i32PosACamX / 10000.0f,
		u32VeloACamX / 10000.0f))
		return ENG_JWNS::en_error;
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y,
		i32PosStageY / 10000.0f,
		u32VeloStageY / 10000.0f))
		return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : ����� ī�޶� 1���� �� �̵� �Ǿ����� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsMovedMarkACam1()
{
	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"IsMoved.Mark.ACam1");

	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1))	return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))		return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : ��� ����� ī�޶󿡼� ĸó�� �̹����� ���� ��
 parm : count	- [in]  ���ϰ��� �ϴ� �̹����� ���� (�� ���� �����ϰ� grabbed �Ǿ����� ����)
		delay	- [in]  Grabbed Image ������ ��� �����ϴµ� ����ϴ� �ð� (����: msec)
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsGrabbedImageCount(UINT16 count, UINT64 delay, int* camNum)
{
	UINT16 u16Grab = 0;

	u16Grab = uvEng_Camera_GetGrabbedCount(camNum);

	/*Camera ���� ���*/
	uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
	/*Mc2 Ʈ���� ��� �뱤 ���� ����*/
	uvEng_Luria_ReqSetHWInit();

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())
	{
		TCHAR tzTitle[64] = { NULL };
		swprintf_s(tzTitle, 64, L"Is.Grabbed.Image.Count (%u/%u)", count, u16Grab);
		SetStepName(tzTitle);
	}

	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;


	//abh1000 Local Test
	///* ���� �ð� ���� ĸó�� �̹��� ������ ���ٸ� ���� ó�� */
	if (count != u16Grab)
	{
		if (m_u64DelayTime + delay < GetTickCount64())
		{
			TCHAR tzMesg[128] = { NULL };
			swprintf_s(tzMesg, 128, L"Timeout waiting for all images to be captured [%d <> %d]",
				count, u16Grab);
			LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
			return ENG_JWNS::en_error;
		}
		return ENG_JWNS::en_wait;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : ����� ī�޶� 1���� ������ (��������) Trigger 1���� �߻�
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetTrigOutOneACam(UINT8 cam_id)
{
	SetStepName(L"Set.Trig.Out.One (G)");

	UINT8 u8LampType = 0x00;		/* 0x00 : AMBER(1 ch, 3 ch), 0x01 : IR (2 ch, 4 ch) */
	u8LampType = uvEng_GetConfig()->set_comn.strobe_lamp_type;

	/* Channel 1 ���� Ʈ���� 1�� �߻� �� �ٷ� Trigger Disabled ���� */
	if (!uvEng_Mvenc_ReqTrigOutOne(cam_id))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

bool CWorkStep::SetAutoFocusFeatures()
{
	if (uvEng_GetConfig()->luria_svc.useAF == false) return true;

	bool res = true;

	//������Ŀ�� ����



	return res;
	//���⼭ ����.
}


/*
 desc : ������ ���� ����� �������� ��ȿ���� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::CheckValidRecipe()
{
	TCHAR tzMesg[128] = { NULL };

	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	// by sysandj : ��������(����)
	LPG_MACP pstThick = uvEng_ThickCali_GetRecipe(pstRecipe->cali_thick);
	// by sysandj : ��������(����)
	LPG_OSSD pstPhStep = uvEng_PhStep_GetRecipeData(pstRecipe->frame_rate);
	CUniToChar csCnv;

	SetStepName(L"Check.Valid.Recipe");

	if (!pstThick)
	{
		swprintf_s(tzMesg, 128, L"There is no thickness correction info. for recipe [%s]",
			csCnv.Ansi2Uni(pstRecipe->job_name));
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
		return ENG_JWNS::en_error;
	}
	//if (!pstPhStep)
	//{
	//	swprintf_s(tzMesg, 128, L"There is no photohead error (step) info. for frame rate [%s]",
	//			   csCnv.Ansi2Uni(pstRecipe->job_name));
	//	LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
	//	return ENG_JWNS::en_error;
	//}

	TCHAR tzMsg[256] = { NULL };
	swprintf_s(tzMsg, 256, L"Expo Energy = %.4f mj Expo Speed = %.3f, Thick = %d um",
		pstRecipe->expo_energy, pstRecipe->frame_rate / 1000.0f, pstRecipe->material_thick);
	LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);

	return ENG_JWNS::en_next;
}

/*
 desc : ���� ��ϵ� Job Name�� ���� �մϴ�.
 parm : index	- [in]  0 or Later
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetSelectedJobIndex(UINT8 index)
{
	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.Selected.Job.Index (0)");

	if (uvCmn_Luria_GetJobCount() <= index)	return ENG_JWNS::en_error;
#if 0
	/* ��ϵ� Job Name�� ���ٸ� �ٷ� ���� */
	if (uvCmn_Luria_GetJobCount() < 1)	return ENG_JWNS::en_next;
#endif
	/* Job Selected ��û */
	if (!uvEng_Luria_ReqSetSelectedJobIndex(index))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetSelectedJobIndex)");
		ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}
/*
 desc : Job Name�� ���õǾ����� Ȯ��
 parm : index	- [in]  0 or Later
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsSelectedJobIndex()
{
	INT32 i32Index = 0x00;
	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.Selected.Job.Index (0)");
#if 0
	/* ��ϵ� Job Name�� ���ٸ� �ٷ� ���� */
	if (uvCmn_Luria_GetJobCount() < 1)	return ENG_JWNS::en_next;
#endif	
	/* Job Selected ��û */
	if (!uvEng_Luria_IsRecvPktData(ENG_FDPR::en_selected_job))
	{
		ENG_JWNS::en_wait;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : ��ϵ� �Ź� �� ���� �������� ��ġ (���)�� �Ź��� �����ϵ��� ��û
 parm : time	- [in]  Waiting time for a response (unit: msec)
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetDeleteSelectedJobName(UINT32 time)
{
	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.Delete.Selected.Job.Name");
#if 0
	/* ��ϵ� Job Name�� ���ٸ� �ٷ� ���� */
	if (uvCmn_Luria_GetJobCount() < 1)	return ENG_JWNS::en_next;
#endif
	/* Job Name ���� ����, ��ϵ� Job ���� �ӽ� ���� */
	m_u8LastJobCount = uvCmn_Luria_GetJobCount();

	/* Job Delete ��û */
	if (m_u8LastJobCount && !uvEng_Luria_ReqSetDeleteSelectedJob())
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetDeleteSelectedJob)");
		ENG_JWNS::en_error;
	}
#if 0
	/* Job List ��û */
	if (m_u8LastJobCount && !uvEng_Luria_ReqGetJobList())
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetJobList)");
		ENG_JWNS::en_error;
	}
#endif
	/* ��� �۽� �� �亯�� �� ������ ���� �ð� ��� */
	if (time)	SetWorkWaitTime(time);

	return ENG_JWNS::en_next;
}

/*
 desc : Job Deleted ���� ���� Ȯ��
 parm : back_step	- [in]  �ٽ� ���� �۾� ��ġ�� �̵��� Step Number
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsDeleteSelectedJobName(UINT8 back_step)
{
	ENG_JWNS enState = IsWorkWaitTime();

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Deleted.Last.Job.Name");
#if 0
	/* ��ϵ� Job Name�� ���ٸ� �ٷ� ���� */
	if (uvCmn_Luria_GetJobCount() < 1)	return ENG_JWNS::en_next;
#endif
	/* ���� �ð� ��� */
	if (enState != ENG_JWNS::en_next)	return ENG_JWNS::en_wait;

	/* ���� �� ������ ���� �� ������ �ٸ���, Job�� ������ ���� */
	if (m_u8LastJobCount && m_u8LastJobCount == uvCmn_Luria_GetJobCount())
	{
		SetWorkWaitTime(1000);
		/* Job List ��û */
		if (m_u8LastJobCount && !uvEng_Luria_ReqGetJobList())
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetJobList)");
			ENG_JWNS::en_error;
		}

		/* ���� �ܰ迡�� ��û�� �۾��� ���� ������ ���� �ð� ���� ������� Ȯ�� */
		return ENG_JWNS::en_wait;
	}
	/* ���� ���ŵ� Job List�� �ִ��� Ȯ�� */
	if (uvCmn_Luria_GetJobCount() > 0)
	{
		switch (m_enWorkJobID)
		{
		case ENG_BWOK::en_gerb_load:
		case ENG_BWOK::en_gerb_unload:	m_u8StepIt = back_step;	break;	/* �ٽ� ó������ ���� �۾� ���� */
		}
	}

	return ENG_JWNS::en_next;
}

/*
 desc : ��� ��Ʈ�ѷ��� Hysteresis�� Offset & Delay (Positive & Negative) ����
 parm : offset	- [in]  ���� ���� Calibration (Ph.Step)������ ����� �� ���� ����
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetHysteresis(UINT8 offset)
{
	TCHAR tzMesg[128] = { NULL };
	INT32 i32NegaOffset = 0;
	UINT32 u32DelayPosi = 0, u32DelayNega = 0;

	LPG_CLSI pstLuria = &uvEng_GetConfig()->luria_svc;
	LPG_LDMC pstMemMach = &uvEng_ShMem_GetLuria()->machine;
	//LPG_REAF pstRecipe = uvEng_ExpoRecipe_GetSelectRecipe();
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	// by sysandj : ��������(����)
	LPG_OSSD pstPhStep = uvEng_PhStep_GetRecipeData(pstRecipe->frame_rate);

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Hysteresis");
#if 0
	i32NegaOffset = pstPhStep->nega_offset_px;
	u32DelayNega = pstPhStep->delay_posi_nsec;
	u32DelayPosi = pstPhStep->delay_nega_nsec;

	/* �Ʒ� ������ ���� �뱤�� ���� ȯ�� ���� �� */
	if (0xff != offset)
	{
		i32NegaOffset = offset;
		u32DelayNega = 0;
		u32DelayPosi = 0;
	}

	/* Step ���� Ȯ�� */
	if (!pstPhStep)
	{
		// by sysandj : ��������(����)
		swprintf_s(tzMesg, 128, L"Failed to search the file of photohead step [frame_rate=%u]",
			pstRecipe->frame_rate);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
		return ENG_JWNS::en_error;
	}

	/* Hysterisis �� �ʱ�ȭ */
	pstMemMach->ResetHysteresisType1();
	/* Hysterisis �� ���� */
	if (!uvEng_Luria_ReqSetMotionType1Hysteresis(pstPhStep->scroll_mode,
		i32NegaOffset,
		u32DelayNega,
		u32DelayPosi))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetMotionType1Hysteresis)");
		return ENG_JWNS::en_error;
	}
#endif

	return ENG_JWNS::en_next;
}

/*
 desc : ��� ��Ʈ�ѷ��� Hysteresis�� Offset & Delay (Positive & Negative) ���� ���� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsSetHysteresis()
{
#if 0
	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Set.Hysteresis");
	return uvCmn_Luria_IsHysteresisType1() ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
#endif

	return ENG_JWNS::en_next;
}

/*
 desc : ��� ī�޶� (ä��)�� Ʈ���� ��ġ�� �ʱ�ȭ �Ǿ����� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsSetTrigPosResetAll()
{
	if (!IsWorkRepeat())	SetStepName(L"Is.Set.Trig.Pos.Reset.All");
	if (!uvEng_Mvenc_IsTrigPosReset(0x01))	return ENG_JWNS::en_wait;
	if (!uvEng_Mvenc_IsTrigPosReset(0x02))	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : Safety PLC �ʱ�ȭ (Reset)
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::ResetErrorMC2()
{
	if (!uvCmn_MC2_IsConnected())
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"MC2 not connected");
		return ENG_JWNS::en_error;
	}

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Reset.MC2.Drive");
	/* Reset ��ɾ� �۽� */
	if (!uvEng_MC2_SendDevFaultResetAll())
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (FaultResetAll)");
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : Camera Mode
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::CameraSetCamMode(ENG_VCCM mode)
{
	SetStepName(L"Set.Camera.Mode");

	/* Vision : Grabbed Mode */
	uvEng_Camera_SetCamMode(mode);

	return ENG_JWNS::en_next;
}
