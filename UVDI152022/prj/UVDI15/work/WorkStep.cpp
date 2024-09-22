
/*
 desc : 시나리오 단계 (Step) 함수 모음
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
 desc : 생성자
 parm : None
 retn : None
*/
CWorkStep::CWorkStep()
	: CWork()
{
	m_u8LastLoadStrip = 0;	/* 무조건 초기화 */
	m_dbLastPrintPosY = 0.0f;	/* 가장 최근의 Y 축 방향의 노광 모션 위치 값 (단위: mm) */
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CWorkStep::~CWorkStep()
{
}

/*
 desc : 일정 시간 동안 작업을 대기하기 위함
 parm : time	- [in]  최소한의 대기에 필요한 시간 (단위: msec)
 retn : None
*/
ENG_JWNS CWorkStep::SetWorkWaitTime(UINT64 time)
{
	BOOL bWorkAbort = FALSE;
#ifdef _DEBUG
	/* Debug 모드일 경우, 릴리즈 모드 대비 1.5 배 정도 */
	m_u64WaitTime = GetTickCount64() + time + (UINT64)ROUNDED(time * 0.5, 0);
#else
	m_u64WaitTime = GetTickCount64() + time;
#endif
#if 0
	/* 동기 진행*/
	if (m_csSyncAbortSet.Enter())
	{
		/* 작업 취소 요청이 왔는지 여부에 따라 더 진행할지 여부 판단 */
		bWorkAbort = m_bWorkAbortSet;

		/* 동기 해제 */
		m_csSyncAbortSet.Leave();
	}
#endif
	return bWorkAbort ? ENG_JWNS::en_error : ENG_JWNS::en_next;	/* Fixed. */
}

/*
 desc : 특정 시간동안 대기가 완료되었는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsWorkWaitTime()
{
	return	GetTickCount64() > m_u64WaitTime ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}
#if 1
/*
 desc : 각 작업 단계 별로 임의 시간 동안 응답 대기 시간이 초과 되었는지 여부
 parm : back_step	- [in]  뒤로 가고자 하는 작업 단계 횟수 (최소 1 값)
		time_delay	- [in]  응답 대기 시간 값 (단위: msec)
		retry_cnt	- [in]  재시도 횟수
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsStepRepeatTimeout(UINT8 back_step, UINT64 time_delay, UINT8 retry_cnt)
{
	/* 만약 전역 대기 시간이 초과 되었다면... 반드시 FALSE 반환 */
	if (IsWorkWaitTime() == ENG_JWNS::en_next)		return ENG_JWNS::en_error;
	if (back_step >= m_u8StepIt || back_step < 1)	return ENG_JWNS::en_error;

	/* 만약 여기서 장시간 대기하고 있다면... 처음부터 모터 높이 설정을 다시해야 함 */
	if (m_u64DelayTime + time_delay < GetTickCount64())
	{
		/* 현재 재시도하는 작업 단계가 이전에 재시도한 작업 단계와 동일하면, 재시도 횟수 증가 처리 */
		if (m_u8RetryStep != m_u8StepIt)
		{
			m_u8RetryStep = m_u8StepIt;
			m_u8RetryCount = 0x00;	/* 재시도 횟수 초기화 */
		}

		/* 최대 응답 대기 시간까지 응답이 없으면, 결국 Timeout이 발생됨*/
		if (m_u8RetryCount++ >= retry_cnt)
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"The number of repetitions of the same step has exceeded");
			return ENG_JWNS::en_error;
		}

		/* 이전 단계 함수를 호출하기 위해 전전 단계로 이동 */
		m_u8StepIt -= back_step;
	}

	/* 아직 응답 대기 시간이 지나지 않았으니, 대기해 봐라 */
	return ENG_JWNS::en_wait;
}
#endif
/*
 desc : Trigger와 Strobe 동작 : Enable or Disable
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
	/* Trigger Mode (Trigger & Strobe)를 강제로 Disable 시킴 */
	//if (!uvEng_Mvenc_ReqTriggerStrobe(enable))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : 현재 Trigger와 Strobe가 Enable 상태인지 확인
 parm : enable	- [in]  TRUE: Enabled, FALSE: Disabled
 retn : wait, error, complete or next
*/
//ENG_JWNS CWorkStep::IsTrigEnabled(BOOL enable)
//{
//	/* Trigger 상태 Enable 여부 */
//	//if (enable)
//	//{
//	//	if (!IsWorkRepeat())	SetStepName(L"Is.Trig.Enabled");
//	//	return uvCmn_Mvenc_IsStrobEnable() ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
//	//}
//	///* Trigger 상태 Disable 여부 */
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
 desc : 스테이지 초기 시작 위치 (Unloader)로 이동
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetMovingUnloader()
{
	ENG_MMDI enVecAxis = ENG_MMDI::en_axis_none;
	LPG_MDSM pstShMC2 = uvEng_ShMem_GetMC2();

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.Move.Stage.Unload");

	/* Check if it is operating in demo mode */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

	/* 스테이지가 완전히 멈춘 상태인지 확인 */
	if (pstShMC2->IsDriveBusy(UINT8(ENG_MMDI::en_stage_x)) ||
		pstShMC2->IsDriveBusy(UINT8(ENG_MMDI::en_stage_y)) ||
		!pstShMC2->IsDriveCmdDone(UINT8(ENG_MMDI::en_stage_x)) ||
		!pstShMC2->IsDriveCmdDone(UINT8(ENG_MMDI::en_stage_y)) ||
		!pstShMC2->IsDriveReached(UINT8(ENG_MMDI::en_stage_x)) ||
		!pstShMC2->IsDriveReached(UINT8(ENG_MMDI::en_stage_y)))
	{
		return ENG_JWNS::en_wait;
	}

	/* 모든 Motor Drive의 토글 값 저장 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);

	/* X 와 Y 축으로 이동하고자 하는 거리가 각각 1 um 이내이면 굳이 이동하지 않는다. */
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
#if 1 /* 벡터 이동이 잘 동작하지 않을 때가 있음 */
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

		/* Vector Moving하는 기본 축 정보 저장 */
		m_enVectMoveDrv = (enVecAxis == ENG_MMDI::en_stage_x) ? ENG_MMDI::en_stage_x : ENG_MMDI::en_stage_y;
#else
		/* X 축으로 이동 */
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x,
			uvEng_GetConfig()->set_align.table_unloader_xy[0][0],
			uvEng_GetConfig()->mc2_svc.move_velo))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SetMovingUnloader()->uvEng_MC2_SendDevAbsMove (X))");
			return ENG_JWNS::en_error;
		}
		/* Y 축으로 이동 */
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
 desc : 스테이지가 Unloader 위치로 이동 했는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsMovedUnloader()
{
	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Moved.Stage.Unload");
#if 1
	/* 현재 위치와 벡터로 이동하고자 하는 위치가 동일한 경우인지 */
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
 desc : 스테이지 초기 시작 위치로 이동
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetMovingUnloader()
{
	DOUBLE dbStagePosX, dbStagePosY, dbStageVeloY;
	LPG_CSAI pstAlign = &uvEng_GetConfig()->set_align;

	SetStepName(L"Set.Moving.Unloader");

	/* Stage X, Y, Camera 1 & 2 X 이동 좌표 얻기 */
	dbStagePosX = pstAlign->mark2_stage_x;				/* Calibration 거버의 Mark 2 기준 위치 */
	dbStagePosY = pstAlign->table_unloader_xy[0][1];	/* Stage Unloader 위치 */
	/* 각 Axis 별로 기본 동작 속도 값 얻기 */
	dbStageVeloY = uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_y];

	/* 모든 Motor Drive의 토글 값 저장 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);

	/* Stage Moving (Vector) (Vector Moving하는 기본 축 정보 저장) */
	if (!uvEng_MC2_SendDevMoveVector(dbStagePosX, dbStagePosY, dbStageVeloY, m_enVectMoveDrv))
		return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : 모든 드라이브가 다 이동 되었는지 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsMovedUnloader()
{
	if (!IsWorkRepeat())	SetStepName(L"Is.Moved.Unloader");

	/* 현재 위치와 벡터로 이동하고자 하는 위치가 동일한 경우인지 */
	if (uvCmn_MC2_IsDrvDoneToggled(m_enVectMoveDrv))	return ENG_JWNS::en_next;

	return ENG_JWNS::en_wait;
}
#endif
/*
 desc : 거버가 등록되어 있는지 그리고 등록된 거버에 Mark가 존재하는지 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsLoadedGerberCheck()
{
	LPG_CGTI pstTrans = &uvEng_GetConfig()->global_trans;
	LPG_LDPP pstPanel = &uvEng_ShMem_GetLuria()->panel;

	/* 현재 작업 Step Name 설정 */
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
		/* Rectangle Lock, Rotation, Scale 및 Offset 값 설정되어 있는지 비교 */
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
 desc : 모든 Motor Drive가 중지 상태인지 여부 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsMotorDriveStopAll()
{
	BOOL bSucc = TRUE;

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Motor.Drive.Stop.All");
	/* 모든 드라이브의 동작이 멈춘 상태인지 여부 */
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
 desc : 현재 Trigger Board의 채널 별로 Trigger Position 16개 모두 초기화
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetTrigPosReset()
{
	UINT8 u8TrigCh[2][2] = { {1, 3}, {2, 4} };
	UINT8 u8LampType = uvEng_GetConfig()->set_comn.strobe_lamp_type;	/* 현재 조명 타입 (AMBER or IR)에 따라, Trigger Board에 등록될 채널 변경 */

	/* 기존 등록된 Trigger Position 값이 초기화 되었는지 확인 */
	if (uvEng_Mvenc_IsTrigPosReset(0x01, 2) &&
		uvEng_Mvenc_IsTrigPosReset(0x01, 2))	return ENG_JWNS::en_next;

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.Trig.Pos.Reset");

	/* 강제로 Trigger Board에 등록된 각 채널 별로 Trigger Position 16개 모두 초기화 수행 */
	if (!uvEng_Mvenc_ReqTriggerStrobe(FALSE))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : Align Mark 측정 모드로 변경
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetAlignMeasMode()
{
	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.Trig.Pos.Reset");

	/* 현재 Drive의 Done Toggled 값 저장 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	/* Area Mode만 설정함 */
	if (!uvEng_MC2_SendDevLuriaMode(ENG_MMDI::en_stage_y, ENG_MTAE::en_area))
	{
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : Align Mark 측정 모드로 변경 완료 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsAlignMeasMode()
{
	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Align.Meas.Mode");
	/* Y 축 처음 위치로 이동 했는지 여부 확인 */
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))	return ENG_JWNS::en_wait;


	uvEng_Camera_ResetGrabbedImage();
	uvEng_Camera_SetCamMode(ENG_VCCM::en_grab_mode);

	return ENG_JWNS::en_next;
}

/*
 desc : 기존에 등록된 거버가 있는지 거버 리스트 요청
 parm : time	- [in]  Waiting time for a response (unit: msec)
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::GetJobLists(UINT32 time)
{
	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Get.Job.Lists");
#if 1
	/* 기존 Job List 모두 제거 */
	uvCmn_Luria_ResetRegisteredJob();
#endif
	/* Job List 요청 */
	if (!uvEng_Luria_ReqGetJobList())
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetJobList)");
		return ENG_JWNS::en_error;
	}

	/* 명령 송신 후 답변이 올 때까지 일정 시간 대기 */
	if (time)	SetWorkWaitTime(time);

	return ENG_JWNS::en_next;
}

/*
 desc : 현재 선택된 레시피의 거버를 선택하도록 설정
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetJobNameSelecting()
{
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	LPG_LDJM pstJobMgt = &uvEng_ShMem_GetLuria()->jobmgt;
	TCHAR tzJobName[MAX_GERBER_NAME] = { NULL };
	CUniToChar csCnv1, csCnv2;

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.Job.Name.Selecting");

	if (!pstRecipe)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"No Job Name is currently selected");
		return ENG_JWNS::en_error;
	}

	/* 거버 전체 경로 설정 */
	swprintf_s(tzJobName, MAX_GERBER_NAME, L"%s\\%s",
		csCnv1.Ansi2Uni(pstRecipe->gerber_path),
		csCnv2.Ansi2Uni(pstRecipe->gerber_name));

	LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzJobName);

	/* 현재 레시피의 거버 선택 요청 */
	if (!uvEng_Luria_ReqSelectedJobName(tzJobName, 0x00))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSelectedJobName)");
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : 현재 선택된 Job Name이 존재하는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsJobNameSelected()
{
	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Job.Name.Selected");

	/* 현재 선택된 Job Name이 있는지 여부 확인 */
	return uvCmn_Luria_IsSelectedJobName() ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : 현재 등록된 거버 적재 진행
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetJobNameLoading()
{
	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.JobName.Loading");

	/* 송신 설정 */
	if (!uvEng_Luria_ReqSetLoadSelectedJob())
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetLoadSelectedJob)");
		return ENG_JWNS::en_error;
	}
	if (!uvEng_GetConfig()->luria_svc.use_announcement)
	{
		/* 다음 상태 확인까지 일정 시간 지연을 위해서 */
		SetSendCmdTime();
	}

	/* 가장 최근의 Job Loading Rate 값 초기화 */
	m_u8LastLoadStrip = 0;

	return ENG_JWNS::en_next;
}

/*
 desc : 거버 적재 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsJobNameLoaded()
{
	TCHAR tzStep[64] = { NULL };
	LPG_LDJM pstJobMgt = &uvEng_ShMem_GetLuria()->jobmgt;

	/* 현재 작업 Step Name 설정 */
	swprintf_s(tzStep, 64, L"Is.Job.Name.Loaded (%u / %u)",
		pstJobMgt->job_state_strip_loaded, pstJobMgt->job_total_strip_loaded);
	SetStepName(tzStep);

	/* 거버 적재가 모두 완료 되었는지 여부 */
	if (!pstJobMgt->IsJobNameRegistedSelectedLoaded())
	{
		if (!uvEng_GetConfig()->luria_svc.use_announcement)
		{
			/* 주기적으로 적재 상태 확인을 위해 요청 */
			if (IsSendCmdTime(250))
			{
				if (!uvEng_Luria_ReqGetSelectedJobLoadState())
				{
					LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetSelectedJobLoadState)");
					return ENG_JWNS::en_error;
				}
			}
		}

		/* 용량이 큰 거버 적재 중일 경우, Time out이 발생하지 않도록 하기 위함 */
		if (pstJobMgt->job_total_strip_loaded)
		{
			if (pstJobMgt->job_state_strip_loaded > m_u8LastLoadStrip)
			{
				m_u64DelayTime = GetTickCount64();	/* Updates the current time */
				/* 가장 최근에 적재된 Strip 개수 정보 임시 저장 */
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
 desc : Gerber 파일 내에 저장된 Mark (Global & Local)들의 위치에 해당되는 모든 트리거 위치 계산 및 등록
 parm : None
 retn : wait, error, complete or next
*/

ENG_JWNS CWorkStep::SetTrigPosCalcSaved()
{
	UINT8 u8MarkGlobal = uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	UINT8 u8MarkLocal = uvEng_Luria_GetMarkCount(ENG_AMTF::en_local);
	INT32 /*i32MarkDiffY	= 0, */i32ACamDistXY[2] = { NULL };

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.Trig.Calc.Regist");
	/* 카메라간 떨어진 물리적인 거리 확인 */
	GetACamCentDistXY(i32ACamDistXY[0], i32ACamDistXY[1]);
	/* Mark 1 (Left/Bottom)과 3 (Right/Bottom) 마크의 높이 차이 값 */
	/*i32MarkDiffY	= uvEng_Luria_GetGlobalLeftRightBottomDiffY();*/
	/* Mark 개수에 따라 다르게 처리 */


	if (u8MarkGlobal && 0x04 != u8MarkGlobal)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"The number of global mark is not 4");
		return ENG_JWNS::en_error;
	}

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)

	const int minLocalFiducial = 4;

	//if (minLocalFiducial > u8MarkLocal ||  u8MarkLocal % 2 != 0) // 가변피두셜일때는 16개 제한을 두면 안된다. 최소 4점, 짝수이기만 하면 된다.
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
 desc : Align Mark를 Trigger Board에 등록
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetTrigRegistGlobal()
{
	UINT8 u8Count = 0x01;
	PINT32 p32Trig1, p32Trig2;

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.Trig.Regist.Global");
	/* 각 카메라 마다 등록될 트리거 개수 */
	if (0x04 == uvEng_Luria_GetMarkCount(ENG_AMTF::en_global))	u8Count = 0x02;
	/* Led Lamp Type에 따라, 트리거 등록할 채널 선택 및 트리거 저장된 배열 포인터 설정 */
	p32Trig1 = uvEng_Trig_GetTrigger(ENG_AMTF::en_global, 0x01);
	p32Trig2 = uvEng_Trig_GetTrigger(ENG_AMTF::en_global, 0x02);

	//if (!p32Trig1 || !p32Trig2)	return ENG_JWNS::en_error;
	//if (p32Trig1[0] < 1 || p32Trig2[0] < 1)	return ENG_JWNS::en_error;
	if (!uvEng_Mvenc_ReqWriteAreaTrigPos(TRUE /* 정방향 : 전진 */,
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
 desc : Align Mark가 Trigger Board에 모두 등록되었는지 확인
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

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Trig.Regist.Global");

	/* 현재 조명 타입 (AMBER or IR)에 따라, Trigger Board에 등록될 채널 변경 */
#if 1	/* 현재 장비에서는 트리거 이벤트는 무조건 채널 1번과 채널 2번에만 해당됨 */
	LPG_RAAF pstRecipeAlign = uvEng_Mark_GetSelectAlignRecipe();
	uvEng_GetConfig()->set_comn.strobe_lamp_type = pstRecipeAlign->lamp_type;
	uvEng_SaveConfig();

	u8LampType = uvEng_GetConfig()->set_comn.strobe_lamp_type;
#endif

	/* Led Lamp Type에 따라, 트리거 등록할 채널 선택 및 트리거 저장된 배열 포인터 설정 */
	p32Trig1 = uvEng_Trig_GetTrigger(ENG_AMTF::en_global, 0x01);
	p32Trig2 = uvEng_Trig_GetTrigger(ENG_AMTF::en_global, 0x02);
	/* Trigger 속성 정보 설정 */
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
 desc : Align Mark를 Trigger Board에 등록
 parm : scan	- [in]  Scan Number (0 or 1)
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetTrigRegistLocal(UINT8 scan)
{
	UINT8 u8LampType = 0x00;		/* 0x00 : AMBER(1 ch, 3 ch), 0x01 : IR (2 ch, 4 ch) */
	UINT8 u8Index = 0x00, u8Count = 0x00;
	BOOL bDirect = (scan % 2) == 0 ? TRUE /* 역방향 (후진) */ : FALSE /* 정방향 (전진) */;
	PINT32 p32Trig1, p32Trig2;

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())
	{
		if (0x00 == scan)	SetStepName(L"Set.Trig.Regist.Local (1)");
		else				SetStepName(L"Set.Trig.Regist.Local (2)");
	}
	/* 현재 조명 타입 (AMBER or IR)에 따라, Trigger Board에 등록될 채널 변경 */
	u8LampType = uvEng_GetConfig()->set_comn.strobe_lamp_type;

	/* Led Lamp Type에 따라, 트리거 등록할 채널 선택 및 트리거 저장된 배열 포인터 설정 */
	p32Trig1 = uvEng_Trig_GetTrigger(ENG_AMTF::en_local, 0x01);
	p32Trig2 = uvEng_Trig_GetTrigger(ENG_AMTF::en_local, 0x02);
	if (!p32Trig1 || !p32Trig2)	return ENG_JWNS::en_error;

	/* 1 Scan할 때, 검색해야 할 Mark 개수 얻기 */
	u8Count = uvEng_Luria_GetLocalMarkCountPerScan();
	/* 트리거를 등록하고자 하는 시작 위치 얻기 */
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
	/* Trigger 발생 위치 등록 */
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
 desc : Align Mark가 Trigger Board에 모두 등록되었는지 확인
 parm : scan	- [in]  Scan Number (0 or 1)
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsTrigRegistLocal(UINT8 scan)
{
	UINT8 i = 0, u8Index = 0x00, u8Count = 0x00, u8LampType = 0x00;		/* 0x00 : AMBER(1 ch, 3 ch), 0x01 : IR (2 ch, 4 ch) */
	UINT8 u8TrigCh[2][2] = { {1, 3}, {2, 4} }/*, u8ChNo = 0x00*/;		/* 0x01 ~ 0x04 */;
	TCHAR tzWait[64] = { NULL };
	PINT32 p32Trig1, p32Trig2;

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())
	{
		if (0x00 == scan)	SetStepName(L"Is.Trig.Regist.Local (1)");
		else				SetStepName(L"Is.Trig.Regist.Local (2)");
	}
	/* 새로 변경된 트리거 위치인지 여부 확인. 기존과 동일하면 체크하지 않음*/
	/* 현재 조명 타입 (AMBER or IR)에 따라, Trigger Board에 등록될 채널 변경 */
	u8LampType = uvEng_GetConfig()->set_comn.strobe_lamp_type;
	/* Led Lamp Type에 따라, 트리거 등록할 채널 선택 및 트리거 저장된 배열 포인터 설정 */
	p32Trig1 = uvEng_Trig_GetTrigger(ENG_AMTF::en_local, 0x01);
	p32Trig2 = uvEng_Trig_GetTrigger(ENG_AMTF::en_local, 0x02);
	if (!p32Trig1 || !p32Trig2)	return ENG_JWNS::en_error;

	///* 1 Scan할 때, 검색해야 할 Mark 개수 얻기 */
	//u8Count		= uvEng_Luria_GetLocalMarkCountPerScan();
	///* 트리거를 검사하고자 하는 시작 위치 얻기 */
	//u8Index		= u8Count * scan;
	//p32Trig1	+= UINT32(u8Index);
	//p32Trig2	+= UINT32(u8Index);
	///* 1 Scan할 때, 검출되는 Mark 개수 (각 카메라별 임) */
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
	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Move Cam Safe Pos");


	bool doneToggleCam[2];

	LPG_CIEA pstCfg = uvEng_GetConfig();

	// 동작은 동시에
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
 desc : 얼라인 마크 측정을 위해 스테이지를 Unloader 및
		Align Camera 1 / 2를 각각 Mark 1 / 3 번의 위치로 이동
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

	/* Stage X, Y, Camera 1 & 2 X 이동 좌표 얻기 */
	dbStagePosX = pstAlign->mark2_stage_x;				/* Calibration 거버의 Mark 2 기준 위치 */
	dbStagePosY = pstAlign->table_unloader_xy[0][1];	/* Stage Unloader 위치 */

	/* Mark 4 개가 아닌 경우, 지원하지 않음 */
	if (u8Mark != 4)
	{
		SetStepName(L"Set.Align.Moving.Init:No marks");
		return ENG_JWNS::en_error;
	}

	/* -------------------------------------------------- */
	/* 마크 2개 혹은 4개 일 경우, 고려해서 위치 정보 계산 */
	/* -------------------------------------------------- */
	m_dbPosACam[0] = GetACamMark2MotionX(2);	/* 얼라인 Mark 2 (Left/Bottom)번에 해당되는 카메라 1번의 X 축 실제 모션 위치 */
	m_dbPosACam[1] = GetACamMark2MotionX(4);	/* Camera 1대비 Camera2 값을 벌리려는 간격 */

	/* 각 Axis 별로 기본 동작 속도 값 얻기 */
	dbStageVeloY = uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_y];
	dbACamVeloX = uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_align_cam1];

	/* 모든 Motor Drive의 토글 값 저장 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam2);

	/*노광 시작 위치로 이동시 인터락 검사*/
	if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_x, dbStagePosX))
	{
		return ENG_JWNS::en_error;
	}
	if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_y, dbStagePosY))
	{
		return ENG_JWNS::en_error;
	}

	/* Stage Moving (Vector) (Vector Moving하는 기본 축 정보 저장) */
	if (!uvEng_MC2_SendDevMoveVectorXY(ENG_MMDI::en_stage_x, ENG_MMDI::en_stage_y,
		dbStagePosX, dbStagePosY, dbStageVeloY, m_enVectMoveDrv))
		return ENG_JWNS::en_error;
	/* 현재 카메라 1 번의 절대 위치 기준 대비, 이동하고자 하는 절대 위치 값이 큰 값인지 혹은 */
	/* 작은 값인지 여부에 따라, Align Camera 1 혹은 2 번 중 어느 것을 먼저 이동해야 할지 결정*/
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
 desc : Align Mark 위치로 이동 했는지 확인
 변수 :	None
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

	/* Stage X / Y, Align Camera 1 & 2의 X 이동이 모두 완료 되었는지 여부 */
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam2))	return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1))	return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(m_enVectMoveDrv))			return ENG_JWNS::en_wait;

	/*LDS 센서 온*/
	if (uvEng_GetConfig()->measure_flat.u8UseThickCheck)
	{
		/*LDS 센서 온*/
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
 desc : 얼라인 마크 측정을 위해 스테이지를 노광 끝 위치로 이동
 parm : None
 retn : TRUE or FALSE
*/
ENG_JWNS CWorkStep::SetAlignMovingGlobal()
{
	DOUBLE dbStageVelo = uvEng_GetConfig()->mc2_svc.mark_velo;

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.Align.Moving.Global");

	/* 정방향으로 이동 */
	uvEng_Camera_SetMoveStateDirect(TRUE);

	/* 이전 Axis의 Done Toggled 값 저장 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	/* 마크 검색을 위해 Y 축 이동 끝 지점을 노광 시작 위치까지 하면, 바로 노광할 수 있기 때문임 */

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
 desc : Align Mark 위치로 이동 했는지 확인
 변수 :	None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsAlignMovedGlobal()
{
	if (!IsWorkRepeat())	SetStepName(L"Is.Align.Moved.Global");

	if (uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))
	{
		auto& measureFlat = uvEng_GetConfig()->measure_flat;
		auto mean = measureFlat.GetThickMeasureMean();

		/*LDS 기능으로 소재 뚜께 측정하여 허용범위에서 벗어나면 에러 처리하여 작업 중지*/
		if (measureFlat.u8UseThickCheck)
		{
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
			LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();

			DOUBLE dLDSZPOS = uvEng_GetConfig()->acam_spec.acam_z_focus[1];
			DOUBLE dmater = pstRecipe->material_thick / 1000.0f;

			DOUBLE LDSToThickOffset = 0;
			//LDSToThickOffset = 1.3;
			LDSToThickOffset = uvEng_GetConfig()->measure_flat.dOffsetZPOS;

			/*현재 측정 LDS 측정값에 장비 옵셋값 추가 하여 실제 소재 측정값 계산*/
			//DOUBLE RealThick = LDSToThickOffset - LDSMeasure;
			//DOUBLE RealThick = LDSToThickOffset - mean + dmater;
			/*소재두께 0mm 위치 CameraZ 설정 후 LDS 초기화 그래서 오차값만 측정됨*/
			DOUBLE RealThick = mean + dmater + LDSToThickOffset;
			DOUBLE LimitZPos = uvEng_GetConfig()->measure_flat.dLimitZPOS;
			DOUBLE MaxZPos = uvEng_GetConfig()->measure_flat.dLimitZPOS;
			DOUBLE MinZPos = uvEng_GetConfig()->measure_flat.dLimitZPOS * -1;

			TCHAR tzMsg[256] = { NULL };
			swprintf_s(tzMsg, 256, L"Real Thick :%.3f > Material Thick : %.3f + Limit : %.3f", RealThick, dmater, LimitZPos);
			LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);

			//swprintf_s(tzMsg, 256, L"Real Thick :%.3f  Material Thick : %.3f + GetThickMeasureMean : %.3f", RealThick, dmater, mean);
			//LOG_WARN(ENG_EDIC::en_acam_cali, tzMsg);

			/*LDS에서 측정한 값과 옵셋값 더한값이 Limit 범위*/
			if ((RealThick > (dmater + MaxZPos)) || (RealThick < (dmater + MinZPos)))
			{
				swprintf_s(tzMsg, 256, L"Failed to actual material thickness tolerance range\n [Real Thick :%.3f > Material Thick : %.3f + Limit : %.3f]", RealThick, dmater, LimitZPos);
				//CDlgMesg dlgMesg;
				//if (IDOK != dlgMesg.MyDoModal(tzMsg, 0x01))
				LOG_ERROR(ENG_EDIC::en_uvdi15, tzMsg);

				/*소재 측정 에러 발생 확인*/
				GlobalVariables::GetInstance()->GetAlignMotion().markParams.workErrorType = ENG_WETE::en_lds_thick_check;
				return ENG_JWNS::en_error;
			}

#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
			/*현재 측정 LDS 측정값에 장비 옵셋값 추가 하여 실제 소재 측정값 계산*/
			DOUBLE RealThick = mean + uvEng_GetConfig()->measure_flat.dOffsetZPOS;
			DOUBLE LimitZPos = uvEng_GetConfig()->measure_flat.dLimitZPOS;

			TCHAR tzMsg[256] = { NULL };
			swprintf_s(tzMsg, 256, L"CheckThick LDS Measure = %.4f", mean);
			LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);

			/*LDS에서 측정한 값과 옵셋값 더한값이 Limit 범위*/
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
			/*한 싸이클만 통과 였다면 LDS Check 기능 On으로 변경*/
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
 desc : 스테이지 얼라인 위치로 이동
 parm : mode	- [in]  0x00 - Init, 0x01 - Moving
		scan	- [in]  Align Scan하려는 위치 (번호. 0 or 1)
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetAlignMovingLocal(UINT8 mode, UINT8 scan)
{

	//AlignMotionMode : UINT8
	//{
	//	toInitialMoving = 0, //최초 스캔을 위해 검사 초기위치로 이동하는 스탭
	//	toScanMoving = 1,    //스캔을 위해 이동하는 스텝
	//};

	const int WANGBOKCOUNT = 2;

	UINT8 u8MarkLeft, u8MarkRight;
	BOOL bStageMoveDirect = (scan % WANGBOKCOUNT == 0x00) ? FALSE /* 역방향 */ : TRUE /* 정방향 */;

	DOUBLE dbACamDistXY[2], dbAlignStageY;
	DOUBLE dbMarkDist, dbDiffMarkX, dbStageVelo, dbACamVelo;
	STG_XMXY stPoint = { STG_XMXY(), };
	LPG_CSAI pstSetAlign = &uvEng_GetConfig()->set_align;
	LPG_CMSI pstMC2Svc = &uvEng_GetConfig()->mc2_svc;
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	// by sysandj : 변수없음(수정)
	LPG_MACP pstThickCali = uvEng_ThickCali_GetRecipe(pstRecipe->cali_thick);

	/* 현재 작업 Step Name 설정 */
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
		/* 스테이지 이동 방향 설정 */

	}

	uvEng_Camera_SetMoveStateDirect(bStageMoveDirect);
	/* 카메라간 떨어진 물리적인 거리 확인 */
	GetACamCentDistXY(dbACamDistXY[0], dbACamDistXY[1]);

	/* Align Mark 측정 초기 위치로 이동 모드인 경우 */
	if (0x00 == mode)
	{
		if (!GetLocalLeftRightBottomMarkIndex(scan, u8MarkLeft, u8MarkRight))
			return ENG_JWNS::en_error;
		/* Left와 Right 마크 간의 간격 (넓이) 얻기 */
		dbMarkDist = uvEng_Luria_GetLocalMarkACam12DistX(0x00, scan);		/* mm */
		if (!uvEng_Luria_GetLocalMark(u8MarkLeft, &stPoint))
			return ENG_JWNS::en_error;
		dbDiffMarkX = stPoint.mark_x - pstSetAlign->mark2_org_gerb_xy[0];	/* mm */
#if 0
		TRACE(L"dbDiffMarkX = %d\n", dbDiffMarkX);
#endif
		/* 환경 파일에 설정된 이전 Mark 중심일 때, Align Camera 위치 값에 오차 값을 합산 (더하거나 빼거나) */
		m_dbPosACam[0] = pstThickCali->mark2_acam_x[0] + dbDiffMarkX;	/* 100 nm or 0.1 um */
		/* Camera 1대비 Camera2 값을 벌리려는 간격 */
		m_dbPosACam[1] = m_dbPosACam[0] + dbMarkDist - dbACamDistXY[0];
		/* Camera 1 & 2번 축 이동 속도 */
		dbACamVelo = pstMC2Svc->max_velo[UINT8(ENG_MMDI::en_align_cam1)];
	}
	else
	{
		/* Stage Y 축 이동 속도 */
		dbStageVelo = pstMC2Svc->mark_velo;
	}

	/* Align Mark 측정 초기 위치로 이동 모드인 경우 */
	if (0x00 == mode)
	{
		/* 현재 Done Toggled 값 저장 */
		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam2);
		/* Align Camera 1 & 2 번 이동 (반드시 2번 카메라부터 이동하기 시작) */
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2, m_dbPosACam[1], dbACamVelo))
			return ENG_JWNS::en_error;
		/* Mark Type이 Shared인지 여부에 따라 1번 카메라 이동 여부 결정 */
		// by sysandj : 함수없음(수정)
		if (0x00 == scan || !uvEng_Recipe_IsRecipeSharedType())
		{
			uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
			if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, m_dbPosACam[0], dbACamVelo))
				return ENG_JWNS::en_error;
		}
	}
	/* Align Mark 검출을 위해 Y 축으로 이동 */
	else
	{
		//uvEng_Mvenc_ReqTriggerStrobe(FALSE);
		//uvEng_Mvenc_ReqEncoderOutReset();
		//uvEng_Mvenc_ResetTrigPosAll();



		if (bStageMoveDirect == false)	/* 역방향 Y 축 이동할 때, 멈추고자 하는 위치 값 */
			dbAlignStageY = pstSetAlign->table_unloader_xy[0][1];
		else				/* 정방향 Y 축 이동 */
			dbAlignStageY = uvCmn_Luria_GetStartY(ENG_MDMD::en_pos_expo_start);

		/* 현재 Done Toggled 값 저장 */
		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
		/* Stage Y 축 이동 */
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
 desc : Align Mark 위치로 이동 했는지 확인
 변수 :	scan	- [in]  Align Scan하려는 위치 (번호. 0 or 1)
		mode	- [in]  0x00 - Init, 0x01 - Moving
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsAlignMovedLocal(UINT8 mode, UINT8 scan)
{
	TCHAR tzStep[2][64] = { L"Is.Align.Moved.Local.Init", L"Is.Align.Moved.Local.Check" };
	BOOL bSucc = FALSE;

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(tzStep[mode]);

	/* Align Mark 측정 초기 위치로 이동 모드인 경우 */
	if (0x00 == mode)
	{
		/* Check to Done Toggled */
		bSucc = uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam2);
		/* Mark Type이 Shared인지 여부에 따라 1번 카메라 이동 여부 결정 */
		// by sysandj : 함수없음(수정)
		if (bSucc && (0x00 == scan || !uvEng_Recipe_IsRecipeSharedType()))
		{
			bSucc = uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1);
		}
		return bSucc ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
	}
	/* Align Mark 검출을 위해 Y 축으로 이동 */
	else
	{
		bSucc = uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y);
		return bSucc ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
	}
}

/*
 desc : 마무리 진행
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
 desc : 마무리 확인
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
 desc : Vacuum과 Shutter 제어 - (LED Off, Vacuum On, Shutter Closed)
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetLedVacuumShutterIn()
{
	return SetLedVacuumShutterOnOff(0x00, 0x01, 0x00);
}

/*
 desc : Vacuum과 Shutter 제어 - (LED On, Vacuum Off, Shutter Open)
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetLedVacuumShutterOut()
{
	return SetLedVacuumShutterOnOff(0x01, 0x00, 0x01);
}

/*
 desc : Vacuum과 Shutter 제어
 parm : vacuum	- [in]  0x01: On, 0x00: Off, 0x02: 기존 유지
		shutter	- [in]  0x01: Open, 0x00: Close, 0x02: 기존 유지
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetVacuumShutterOnOff(UINT8 vacuum, UINT8 shutter)
{
	TCHAR tzTilte[128] = { NULL }, tzVac[2][8] = { L"Off", L"On" }, tzShu[2][8] = { L"Close", L"Open" };
	/* 만약, Vaccum 상태를 기존 상태 그대로 유지하는 경우인지에 따라 ... */
	if (vacuum == 0x02)		vacuum = 0;// by sysandj : MCQ대체 추가 필요 : uvEng_ShMem_GetPLCExt()->r_vacuum_status;
	/* 만약, Shutter 상태를 기존 상태 그대로 유지하는 경우인지에 따라 ... */
	if (shutter == 0x02)	shutter = 0;// by sysandj : MCQ대체 추가 필요 : uvEng_ShMem_GetPLCExt()->r_shutter_status;
#ifdef _DEBUG
	vacuum = 0;// by sysandj : MCQ대체 추가 필요:uvEng_ShMem_GetPLCExt()->r_vacuum_status;	/* 현재 상태 유지 */
#endif

#if 0
	/* 현재 Shutter나 Vacuum이 Open (On) or Close (Off) 상태가 아닌 경우 */
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

	/* Vacuum과 Shutter를 동시 제어 수행 */
	// by sysandj : MCQ대체 추가 필요 : if (!uvEng_MCQ_SetVacuumShutterOnOff(vacuum, shutter))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : Start Lamp / Vacuum / Shutter 제어
 parm : start_led	- [in]  0x01: On, 0x00: Off
		vacuum		- [in]  0x01: On, 0x00: Off
		shutter		- [in]  0x01: Open, 0x00: Close
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetLedVacuumShutterOnOff(UINT8 start_led, UINT8 vacuum, UINT8 shutter)
{
	TCHAR tzTilte[128] = { NULL }, tzVac[2][8] = { L"Off", L"On" }, tzShu[2][8] = { L"Close", L"Open" };
	UINT8 u8StartLed = 0;// by sysandj : MCQ대체 추가 필요:((LPG_PMRW)uvEng_ShMem_GetPLC()->data)->r_start_button;

	/* 현재 Shutter나 Vacuum이 Open (On) or Close (Off) 상태가 아닌 경우 */
	if (ENG_JWNS::en_wait == IsShutterVacuumWait())
	{
		SetStepName(L"Shutter && Vacuum Waiting");
		return ENG_JWNS::en_wait;
	}

	/* 현재 작업 Step Name 설정 */
	swprintf_s(tzTilte, 128, L"Led(%s).Vacuum(%s).Shutter(%s)",
		tzVac[start_led], tzVac[vacuum], tzShu[shutter]);
	SetStepName(tzTilte);
#if 1
	vacuum = 0;// by sysandj : MCQ대체 추가 필요 : uvEng_ShMem_GetPLCExt()->r_vacuum_status;	/* 현재 상태 유지 */
#endif
	/* Vacuum과 Shutter를 동시 제어 수행 */
	// by sysandj : MCQ대체 추가 필요
// 	if (!uvEng_MCQ_LedVacuumShutter(u8StartLed, vacuum, shutter))
// 	{
// 		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (LedVacuumShutter)");
// 		return ENG_JWNS::en_error;
// 	}

	return ENG_JWNS::en_next;
}

/*
 desc : Start Lamp / Shutter 제어 (Vacuum 제외)
 parm : start_led	- [in]  0x01: On, 0x00: Off
		shutter		- [in]  0x01: Open, 0x00: Close
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetLedShutterOnOff(UINT8 start_led, UINT8 shutter)
{
	UINT8 u8Vacuum = 0x00;
	TCHAR tzTilte[128] = { NULL }, tzVac[2][8] = { L"Off", L"On" }, tzShu[2][8] = { L"Close", L"Open" };
#if 0
	/* 현재 Shutter나 Vacuum이 Open (On) or Close (Off) 상태가 아닌 경우 */
	if (ENG_JWNS::en_wait == IsShutterVacuumWait())
	{
		SetStepName(L"Shutter && Vacuum Waiting");
		return ENG_JWNS::en_wait;
	}
#endif
	/* 현재 작업 Step Name 설정 */
	swprintf_s(tzTilte, 128, L"Led(%s).Shutter(%s)", tzVac[start_led], tzShu[shutter]);
	SetStepName(tzTilte);
	/* 자동 Vacuum 동작 모드가 아닌 경우, Vacuum은 현재 상태 그대로 유지 */
	u8Vacuum = 0;// by sysandj : MCQ대체 추가 필요: uvEng_ShMem_GetPLCExt()->r_vacuum_status;

	/* Vacuum과 Shutter를 동시 제어 수행 */
	// by sysandj : MCQ대체 추가 필요 : if (!uvEng_MCQ_LedVacuumShutter(start_led, u8Vacuum, shutter))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : Vacuum이 현재 Wait 상태인지 여부
 parm : None
 retn : wait, next
*/
ENG_JWNS CWorkStep::IsVacuumWait()
{
	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Vacuum.Wait");
	/* 현재 Vacuum 상태가 Waiting 모드인지 여부에 따라 */
	// by sysandj : MCQ대체 추가 필요
// 	if (uvEng_ShMem_GetPLCExt()->r_vacuum_vac_atm_waiting)
// 	{
// 		return ENG_JWNS::en_wait;	/* Wait 상태 임 */
// 	}
	return ENG_JWNS::en_next;	/* Wait 상태 아님 */
}

/*
 desc : Shutter이 현재 Wait 상태인지 여부
 parm : None
 retn : wait, next
*/
ENG_JWNS CWorkStep::IsShutterWait()
{
	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Shutter.Wait");
#ifndef _DEBUG
	// 	/* 현재 Vacuum 상태가 Waiting 모드인지 여부에 따라 */
	// 	if (uvEng_ShMem_GetPLCExt()->r_shutter_open_close_waiting)
	// 	{
	// 		if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;
	// 
	// 		return ENG_JWNS::en_wait;	/* Wait 상태 임 */
	// 	}
#endif
	return ENG_JWNS::en_next;	/* Wait 상태 아님 */
}

/*
 desc : Shutter or Vacuum이 대기 상태인지 여부
 parm : None
 retn : TRUE (대기 상태임) or FALSE (대기 상태 아님)
*/
ENG_JWNS CWorkStep::IsShutterVacuumWait()
{
	BOOL bWaitVacuum = FALSE;

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Shutter.Vacuum.Wait");

	if (uvEng_GetConfig()->IsCheckVacuum())
	{
		// by sysandj : MCQ대체 추가 필요
		//bWaitVacuum	= uvEng_ShMem_GetPLCExt()->r_vacuum_vac_atm_waiting ? TRUE : FALSE;
	}
	/* 현재 Shutter나 Vacuum이 Open (On) or Close (Off) 상태가 아닌 경우 */
	// by sysandj : MCQ대체 추가 필요
// 	return (uvEng_ShMem_GetPLCExt()->r_shutter_open_close_waiting || bWaitVacuum) ?
// 		ENG_JWNS::en_wait : ENG_JWNS::en_next;
	return ENG_JWNS::en_next;
}

/*
 desc : 노광 시작 위치로 이동
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetMovePrintXY()
{
	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Move.Printing.XY");
	/* 노광 시작 위치로 Stage를 이동 시킴 */
	return uvEng_Luria_ReqSetPrintStartMove() ? ENG_JWNS::en_next : ENG_JWNS::en_error;
}

/*
 desc : PrePrinting 수행
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetPrePrinting()
{
	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Run.Pre.Printing");

	/* Printing 상태 값 초기화 */
	uvEng_ShMem_GetLuria()->exposure.ResetExposeState();
	/* Luria Service에게 PrePrinting 호출 */
	if (!uvEng_Luria_ReqSetPrintOpt(0x00))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetPrintOpt.PrePrint)");
		return ENG_JWNS::en_error;
	}

	SetSendCmdTime();	/* 가장 최근에 송신한 시간 저장 */
	/* Printing Status 정보 요청 */
	if (!uvEng_Luria_ReqGetExposureState())
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetExposureState.PrePrint)");
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : PrePrinting 완료 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsPrePrinted()
{
	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Pre.Printed");

	/* 현재 준비 상태가 실패되었는지 확인 */
	if (uvCmn_Luria_IsExposeStateFailed(ENG_LCEP::en_pre_print))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to run the preprint job");
		return ENG_JWNS::en_error;
	}
	/* 현재 노광 준비 상태가 성공적인 경우 */
	if (uvCmn_Luria_IsExposeStateSuccess(ENG_LCEP::en_pre_print))
	{
		return ENG_JWNS::en_next;
	}
#if 0	/* PrePrint의 경우, 자동으로 응답되지 않고, 반드시 State 요청해야 함 */
	if (!uvEng_GetConfig()->luria_svc.use_announcement)
#endif
	{
		/* 일정 시간 대기 했는지 여부 */
		if (IsSendCmdTime(250))
		{
			/* 다시 현재 노광 진행 상태 요청 */
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
 desc : Printing 수행
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetPrinting()
{
	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.Printing");

	/* 가장 최근에 노광한 개수 (Stripe) 개수 초기화 */
	m_u8PrintStripNum = 0x00;
	m_u64TickPrintStrip = GetTickCount64();	/* 가장 최근에 노광 개수가 변경된 시간 초기화 */
	m_i32PrintingStageY = 0;				/* 노광 (Printing)하고 있는 Stage Y 축의 위치 초기화 (단위: um) */
	/* Luria Service에게 Printing 호출 */
	if (!uvEng_Luria_ReqSetPrintOpt(0x01))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetPrintOpt.Print)");
		return ENG_JWNS::en_error;
	}
	if (!uvEng_GetConfig()->luria_svc.use_announcement)
	{
		SetSendCmdTime();	/* 가장 최근에 송신한 시간 저장 */
		/* Printing Status 정보 요청 (함수 내부에 1초에 4번 밖에 요청하지 못함) */
		if (!uvEng_Luria_ReqGetExposureState())
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetExposureState.Printing)");
			return ENG_JWNS::en_error;
		}
	}

	return ENG_JWNS::en_next;
}

/*
 desc : Printing 완료 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsPrinted()
{
	TCHAR tzTitle[128] = { NULL };
	INT32 i32StageY = 0;
	UINT8 u8State = 0x00;
	DOUBLE dbMotionY = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);	/* unit: mm */

	/* 현재 노광 진행률 값 설정 */
	swprintf_s(tzTitle, 128, L"Run.Printing (%.2f)", uvCmn_Luria_GetPrintRate());
	/* 현재 작업 Step Name 설정 */
	SetStepName(tzTitle);

	/* 현재 노광 진행 중 실패 했는지 여부 */
	u8State = (UINT8)uvCmn_Luria_GetExposeState();
	if (0x0f == (u8State & 0x0f))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"There was a problem during the expose operation");
	}

	/* 노광 완료 상태이면 */
	else if (uvCmn_Luria_IsExposeStateSuccess(ENG_LCEP::en_print))
	{
		return ENG_JWNS::en_next;
	}

	/* Printing Status 정보 요청 (함수 내부에 1초에 2번 밖에 요청하지 못함) */
	if (!uvEng_GetConfig()->luria_svc.use_announcement)
	{
		/* 일정 시간 대기 했는지 여부 */
		if (IsSendCmdTime(250))
		{
			/* 다시 현재 노광 진행 상태 요청 */
			if (!uvEng_Luria_ReqGetExposureState())
			{
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetExposureState.IsPrinting)");
				return ENG_JWNS::en_error;
			}
		}
	}

	/* Y 축 노광 방향으로 Motion이 움직이고 있으면, 대기 시간을 최근 시간으로 갱신 */
	if (abs(dbMotionY - m_dbLastPrintPosY) > 1.0f)	/* 1.0 mm 이상 차이가 발생했는지 여부 */
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

	/* 현재 작업 Step Name 설정 */
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


		/* Global Fiducial 적재 */
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
		/* Local Fiducial 적재 */
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

		/* Grabbed Mark images의 결과 값 가져오기 */
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

		//string fmtFormat = fmt::format("tgt mark {} - 그랩옵셋X = {:.3f}, 그랩옵셋Y = {:.3f},  얼라인옵셋X = {:.3f} ,얼라인옵셋Y = {:.3f} ,  익스포옵셋X = {:.3f}, 익스포옵셋Y = {:.3f}, 원래X = {:.3f} , 원래 Y = {:.3f}, 적용차이X = {:.3f} , 적용차이Y = {:.3f}",
			//temp.tgt_id, grabOffsetX, grabOffsetY, alignOffset.offsetX, alignOffset.offsetY, expoOffset.offsetX, expoOffset.offsetX, bkx, bky, bkx - temp.mark_x, bky - temp.mark_y);
//			webMonitor.AddLog(fmtFormat);

		lstMarks.AddTail(temp);
	}


	/* -------------------------------------------------------------------------------------- */
/* 거버에 등록된 Global Fiducial Mark와 Grabbed Image들 간의 직선 / 대각선 등의 크기 비교 */
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
	/* 원래 거버 좌표 상의 마크 넓이 / 높이 값 임시 저장 */
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
	/* 측정 오차 값이 적용된 마크 넓이 / 높이 값 임시 저장 */
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
			pstMarkDiff->SetMarkLenValid(i, u8MarkLen[i]);	/* Mark 간의 간격 (대각선 포함)에 대해 유효성 설정 (상/하 수평선, 좌/우 수직선, 대각선 2개 = 총 6개 */
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



	/* Global Transformation Recipe 정보 설정 */
	if (uvEng_Luria_ReqSetGlobalTransformationRecipe(0x00, 0x00, 0x00) == false)
		return ENG_JWNS::en_error;
	/* 기존 상태 값 초기화 */
	uvCmn_Luria_ResetRegistrationStatus();
	/* 무조건 Shared Local Zone 사용 안함으로 설정해야 함 */
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

	/* Luria에 변경된 Mark 위치 값 등록 진행 */
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
 desc : Luria Service에 측정된 Align Mark 오차 값 등록
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
	ENG_AMOS motionType = motion.markParams.alignMotion; //이건 실시간으로 바뀔수있음을 참고하고, 테스트용으로 사용한다. 

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.Align.Mark.Regist");

	/* XML 파일에 등록된 Mark 개수 확인 */
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

	/* 현재 노광 모드가 Align Expose 인 경우만 수행하도록 함 */
	switch (m_enWorkJobID)
	{
	case ENG_BWOK::en_expo_align:
	case ENG_BWOK::en_expo_cali:
	case ENG_BWOK::en_mark_test:
		/* 거버에 등록된 전체 Mark 개수 (Global + Local)와 Grabbed Image 개수 비교 */
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
	} //마크카운트 그랩카운트 비교해서 다르면 에러처리해서 나감.

	/* Global 마크가 등록되어 있지 않다면 Pass */
	if (u8MarkG < 4)
		return ENG_JWNS::en_next;

	TCHAR tzMsg[256] = { NULL };
	/* Global Fiducial 적재 */
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
	/* 거버에 등록된 Global Fiducial Mark와 Grabbed Image들 간의 직선 / 대각선 등의 크기 비교 */
	/* -------------------------------------------------------------------------------------- */
	/* 원래 거버 좌표 상의 마크 넓이 / 높이 값 임시 저장 */
	for (i = 0; bSucc && i < CrossLengthCount; i++)
	{
		stMarkPos1 = lstMarks.GetAt(lstMarks.FindIndex(u8Reg1[i]));
		stMarkPos2 = lstMarks.GetAt(lstMarks.FindIndex(u8Reg2[i]));
		/* mm -> 0.1 um or 100 nm */
		i32DistS[i] = (INT32)ROUNDED(sqrt(pow(stMarkPos1.mark_x - stMarkPos2.mark_x, 2) +
			pow(stMarkPos1.mark_y - stMarkPos2.mark_y, 2)) * 10000.0f, 0);
	}


	/* Align Mark로 측정된 오차 값에 Camera Calibration Offset 적용 */
	if ((ENG_BWOK::en_mark_test == m_enWorkJobID ||
		ENG_BWOK::en_expo_align == m_enWorkJobID ||
		ENG_BWOK::en_expo_cali == m_enWorkJobID) &&
		!uvEng_GetConfig()->IsRunDemo())
	{

		//-------------------------    글로벌 마크 (옵셋 미적용)   아주 일관성이 없어 아주 ------------------------
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
				else if (pstSetAlign->use_mark_offset == (UINT8)ENG_ADRT::en_from_mappingfile) //맵핑파일을 이용하게될경우 이곳에 추가.
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
					/* Align Mark 측정 오차에 최종 Mark Offset 값 포함해서 위치 재 조정 */
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
						else if (pstSetAlign->use_mark_offset == (UINT8)ENG_ADRT::en_from_mappingfile) //맵핑파일을 이용하게될경우 이곳에 추가.
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
			/* 측정 오차 값이 적용된 마크 넓이 / 높이 값 임시 저장 */
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
					pstMarkDiff->SetMarkLenValid(i, u8MarkLen[i]);	/* Mark 간의 간격 (대각선 포함)에 대해 유효성 설정 (상/하 수평선, 좌/우 수직선, 대각선 2개 = 총 6개 */
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
			swprintf_s(tzMesg, 256, L"Dist = (top_horz = %.1f)(btm_horz = %.1f)(lft_vert = %.1f)(rgt_vert = %.1f)(lft_diag = %.1f)(rgt_diag = %.1f) 입니다 노광 하시겠습니까?",
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
			/* Mark들의 거리 및 간격 값 설정했다고 플래그 값 변경 */
			pstMarkDiff->SetMarkLenData();

			/*수평과 수직간의 오차가 제한값보다 크면 정지*/
			//if (pstMarkDiff->HorzDiff(0.005) && pstMarkDiff->VertDiff(0.003))
			if (pstMarkDiff->HorzDiff(pstSetAlign->mark_horz_diff) || pstMarkDiff->VertDiff(pstSetAlign->mark_vert_diff))
			{
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"The distance Horz, Vert Error");
				bSucc = FALSE;
			}
		}
	}

	/* Global Transformation Recipe 정보 설정 */
	if (bSucc)	bSucc = uvEng_Luria_ReqSetGlobalTransformationRecipe(0x00, 0x00, 0x00);
	/* 기존 상태 값 초기화 */
	uvCmn_Luria_ResetRegistrationStatus();
	/* 무조건 Shared Local Zone 사용 안함으로 설정해야 함 */
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
	/* 나머지 Local Mark XML 파일에서 읽어들인 순서대로 저장 */
	for (j = u8MarkG; j < u8MarkG + u8MarkL; j++)
	{
		stMarkPos1 = lstMarks.GetAt(lstMarks.FindIndex(j));
		pstMarks[stMarkPos1.org_id + u8MarkG].x = (INT32)ROUNDED(stMarkPos1.mark_x * 1000000.0f, 0);	/* mm -> nm */
		pstMarks[stMarkPos1.org_id + u8MarkG].y = (INT32)ROUNDED(stMarkPos1.mark_y * 1000000.0f, 0);	/* mm -> nm */

		swprintf_s(tzMsg, 256, L"Local Regist Mark%d : X =%.4f Y = %.4f", stMarkPos1.org_id, DOUBLE(pstMarks[stMarkPos1.org_id + u8MarkG].x / 1000000.0f), DOUBLE(pstMarks[stMarkPos1.org_id + u8MarkG].y / 1000000.0f));
		LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
	}

	/* Luria에 변경된 Mark 위치 값 등록 진행 */
	if (!uvEng_Luria_ReqSetRegistPointsAndRun(u8MarkG + u8MarkL, pstMarks))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetRegist)");
		bSucc = FALSE;
	}

	uvEng_Luria_ReqGetGetTransformationParams(0);


	/* 메모리 해제 */
	::Free(pstMarks);
	/* List 객체 해제 */
	lstMarks.RemoveAll();

	/* 다음 상태 확인까지 일정 시간 지연을 위해서 */
	SetSendCmdTime();

	//return ENG_JWNS::en_error;
	return bSucc ? ENG_JWNS::en_next : ENG_JWNS::en_error;
	//return ENG_JWNS::en_next;
}

/*
 desc : Align Mark 오차 값 등록 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsAlignMarkRegist()
{
	TCHAR tzMesg[512] = { NULL };
	PTCHAR ptzMesg = NULL;
	UINT16 u16Status = 0x0000;

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Align.Mark.Regist");

	/* XML 파일에 등록된 Mark 개수 확인 */
	if (uvEng_Luria_GetMarkCount(ENG_AMTF::en_global) < 4)
	{
		/* 등록된 Mark가 없으므로, Pass */
		return ENG_JWNS::en_next;
	}

	/* Registration Status 값이 정상적으로 등록되었는지 확인 */
	u16Status = uvCmn_Luria_GetRegistrationStatus();
	if (0xffff == u16Status)
	{
		/* 너무 자주 요청하면 안되므로 ... */
#ifdef _DEBUG
		if (IsSendCmdTime(500))
#else
		if (IsSendCmdTime(250))
#endif
		{
			/* 현재 Fiducial 등록 상태 값 요청 */
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
 desc : 모든 광학계 Z Axis의 높이 조절 - 소재 두께에 따라
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

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Optic.ZAxis.Move");

	/* 가장 최근의 설정한 광학계 Z 축 높이 조절 값 초기화 */
	memset(m_dbPhZAxisSet, 0x00, sizeof(DOUBLE) * MAX_PH);

	/* 모든 광학계 높이 조절 */
	if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x01)
	{
		/* 현재 높이 조절하려는 광학계의 Z Axis 높이에 소재 두께 만큼 증가 or 감소 처리 (단위: um)*/
		dbPhDiffZ = pstRecipe->material_thick / 1000.0f - pstAlign->dof_film_thick;

		for (i = 0x00; i < pstLuria->ph_count; i++)
		{
			/* mm -> um 변환 */
			m_dbPhZAxisSet[i] = pstLuria->ph_z_focus[i] + dbPhDiffZ;
			/* Photohead Z Axis의 Min or Max 값 범위 안에 있는지 여부 */
			if (pstLuria->ph_z_move_min > m_dbPhZAxisSet[i] ||
				pstLuria->ph_z_move_max < m_dbPhZAxisSet[i])
			{
				/* Set the error message */
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"Optical Z Axis movement is out of range");
				return ENG_JWNS::en_error;
			}
		}
		/* 한번에 모든 광학계 Z 축 높이 조정 */
		if (!uvEng_Luria_ReqSetMotorAbsPositionAll(pstLuria->ph_count, m_dbPhZAxisSet))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetMotorAbsPositionAll)");
			return ENG_JWNS::en_error;
		}
	}
	else if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x03)
	{
		/* 현재 높이 조절하려는 광학계의 Z Axis 높이에 소재 두께 만큼 증가 or 감소 처리 (단위: um)*/
		dbPhDiffZ = pstRecipe->material_thick / 1000.0f /* um -> mm */ - pstAlign->dof_film_thick;

		for (i = 0x00; i < pstLuria->ph_count; i++)
		{
			/* mm -> um 변환 */
			//m_dbPhZAxisSet[i]	= pstLuria->ph_z_focus[i] * 1000.0f, 0 + dbPhDiffZ;
			m_dbPhZAxisSet[i] = pstLuria->ph_z_focus[i] + dbPhDiffZ;
			/* Photohead Z Axis의 Min or Max 값 범위 안에 있는지 여부 */
			if (pstMC2->min_dist[i] > m_dbPhZAxisSet[i] ||
				pstMC2->max_dist[i] < m_dbPhZAxisSet[i])
			{
				/* Set the error message */
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"Optical Z Axis movement is out of range");
				return ENG_JWNS::en_error;
			}

			/* 한번에 모든 광학계 Z 축 높이 조정 */
			u8drv_id = (UINT8)ENG_MMDI::en_axis_ph1 + i;
			dbPhVeloZ[i] = pstMC2->max_velo[UINT8(ENG_MMDI::en_axis_ph2)];
			if (!(uvEng_MC2_SendDevAbsMove(ENG_MMDI(u8drv_id), m_dbPhZAxisSet[i], dbPhVeloZ[i])))
			{
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to move the photohead z axis (set position)");
				return ENG_JWNS::en_error;
			}
		}

	}
	/* 현재 위치 정보를 요청하지 않았지만, 요청 했다고 해야 함. (왜냐! 모터 이동 후 바로 도착하지 않으므로) */
	SetSendCmdTime();

	return ENG_JWNS::en_next;
}

/*
 desc : 광학계 Z Axis의 높이 조절이 다 되었는지 여부 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsPhZAxisMovedAll()
{
	UINT8 i;
	BOOL bArrived = TRUE;
	DOUBLE dbPhNowZ;

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Optic.ZAxis.Moved");

	/* 데모 모드로 동작 중인지 여부에 따라 */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

	/* 모든 광학계의 Z 축이 원하는 위치에 도달 했는지 여부 */
	for (i = 0x00; i < uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		//#if (CUSTOM_CODE_TEST_UVDI15 == 0)
		//		/* 오차 값이 +/- 1 um 이내이면 도달했다고 설정*/
		//		dbPhNowZ	= uvEng_ShMem_GetLuria()->directph.focus_motor_move_abs_position[i] / 1000.0f;
		//#else
				//dbPhNowZ = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(0x02 + i));
		dbPhNowZ = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(0x06 + i));
		//#endif
		if (abs(dbPhNowZ - m_dbPhZAxisSet[i]) > 0.010 /* .0 um */) /* 좀 넉넉히 설정해 줘야 한다. */
		{
			bArrived = FALSE;
			break;
		}
	}
	/* 만약 모든 광학계가 도달하지 않았다면, 일정 시간 후에 현재 광학계 위치 정보 요청 */
	if (!bArrived)
	{
		if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
		{
			if (IsSendCmdTime(250))
			{
				/* 광학계 Z 축 위치와 모터의 현재 상태 값 요청 */
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
 desc : Align Camera Z Axis의 높이 조절 - 소재 두께에 따라
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetACamZAxisMovingAll(unsigned long& lastUniqueID)
{
	UINT8 i = 0x00;
	INT32 i32ACamDiffZ = 0;	/* 단위: 0.1 um or 100 nm */
	LPG_CSAI pstAlign = &uvEng_GetConfig()->set_align;
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	LPG_CCDB pstACamInfo = &uvEng_GetConfig()->set_basler;
	//LPG_CAFI pstACamFocus = &uvEng_GetConfig()->acam_focus;
	LPG_CASI pstACamSpec = &uvEng_GetConfig()->acam_spec;

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.ACam.Z.Axis.Moving");

	/* 기존 Align Camera Z 축 높이 값 초기화 */
	memset(m_i32ACamZAxisSet, 0x00, sizeof(INT32) * 2);

	/* 현재 높이 조절하려는 얼라인 카메라의 Z Axis 높이에 소재 두께 만큼 증가 or 감소 처리 */
	i32ACamDiffZ = (INT32)ROUNDED(pstRecipe->material_thick * 10.0f, 0) -
		(INT32)ROUNDED(pstAlign->dof_film_thick * 10000.0f, 0);

#if (USE_IO_LINK_PHILHMI == 0)

	for (; i < uvEng_GetConfig()->set_cams.acam_count; i++)
	{
		m_i32ACamZAxisSet[i] = (INT32)ROUNDED(pstACamFocus->acam_z_focus[i] * 10000.0f, 0) + i32ACamDiffZ;
		/* Align Camera Z Axis의 Min or Max 값 범위 안에 있는지 여부 */
		if ((INT32)ROUNDED(pstACamInfo->z_axis_move_min * 10000.0f, 0) > m_i32ACamZAxisSet[i] ||
			(INT32)ROUNDED(pstACamInfo->z_axis_move_max * 10000.0f, 0) < m_i32ACamZAxisSet[i])
		{
			/* Set the error message */
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Align Camera Z Axis movement is out of range");
			return ENG_JWNS::en_error;
		}
	}
	/* 이동하려는 위치 설정 */
	//if (!uvEng_MCQ_SetACamMoveAbsAllZ(m_i32ACamZAxisSet))
	//{
	//	return ENG_JWNS::en_error;
	//}
#elif (USE_IO_LINK_PHILHMI == 1)
	double		dACamDiffZ = 0;	/* 단위: 0.1 um or 100 nm */
	double		dACamZAxisSet[2];
	/* 현재 높이 조절하려는 얼라인 카메라의 Z Axis 높이에 소재 두께 만큼 증가 or 감소 처리 */
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

	/* 이동하려는 위치 설정 */
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
 desc : Align Camera Z Axis의 높이 조절이 다 되었는지 여부 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsACamZAxisMovedAll(unsigned long& lastUniqueID)
{
	UINT8 i = 0x00;
	INT32 i32ACamNoZ = 0;	/* 단위: 0.1 um or 100 nm */
	//LPG_PMRW pstPLC	= uvEng_ShMem_GetPLCExt();

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.ACam.Z.Axis.Moved");

	/* 데모 모드로 동작 중인지 여부에 따라 */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

#if (USE_IO_LINK_PHILHMI == 0)
	/* 모든 얼라인 카메라의 Z 축이 원하는 위치에 도달 했는지 여부 */
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
	//	// Alarm 처리 필요
	//	// 10초 Timeout이 지나도 complete 신호가 오지 않은 상황
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
 desc : 마크 인식이 모두 완료되었는지 확인
 parm : type	- [in]  Align Mark Type
		scan	- [in]  Align Scan하려는 위치 (번호. 0 or 1)
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsSetMarkValid(ENG_AMTF type, UINT8 scan)
{
	TCHAR tzTitle[128] = { NULL };
	BOOL bSucc = FALSE;
	UINT8 u8Global, u8Local, u8Total;

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Is.SetMark.Valid");

	/* 거버에 등록된 Global & Local Fiducial Mark 개수 */
	u8Global = uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	u8Local = uvEng_Luria_GetMarkCount(ENG_AMTF::en_local);
	/* Global Mark 4개 모두 인식되었는지 여부 확인 */
	if (ENG_AMTF::en_global == type)
	{
		u8Total = u8Global;
		bSucc = uvEng_Camera_GetGrabbedCount() == u8Global;
	}
	else
	{
		/* 전체 Local 개수 중 1 Scan 마다 검출되는 Mark 개수 */
		if (0x00 == scan)	u8Total = u8Local / 2 + u8Global;
		else				u8Total = u8Local + u8Global;
		bSucc = uvEng_Camera_GetGrabbedCount() == u8Total;
	}

	/* Title 설정 */
	swprintf_s(tzTitle, 128, L"Mark.Grabbed (%u / %u)", uvEng_Camera_GetGrabbedCount(), u8Total);
	SetStepName(tzTitle);

	return bSucc ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : 한번에 마크를 모두 인식 후, 정상적으로 마크가 확인되어 있는지 여부
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

	/* 현재 작업 Step Name 설정 */
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
				
			if (!config->set_align.use_invalid_mark_cali)  //교정하지 않겠다.
			{
				bSucc = FALSE;
			}
			else
			{
				if (config->set_align.manualFixOffsetAtSequence) //지금 교정하겠다. 
				{
					CDlgMmpm dlgMmpm;
					bSucc = (IDOK == dlgMmpm.DoModal());
					SetManualFix(bSucc);
							
					UpdateWaitingTime();
				}
				else //아니다 다음에 하겠다. 
				{
					LOG_ERROR(ENG_EDIC::en_uvdi15, L"All found marks are invalid");
					bSucc = FALSE;
				}	
			}
		}
	}
	
	auto u8Global = motions.status.globalMarkCnt;
	auto u8Local = motions.markParams.alignType == ENG_ATGL::en_global_4_local_0_point ? 0 : motions.status.localMarkCnt;

	/* Title 설정 */
	swprintf_s(tzTitle, 128, L"Mark.Grabbed (%u / %u)", uvEng_Camera_GetGrabbedCount(camNum), u8Global + u8Local);
	SetStepName(tzTitle);

	/*Mark Align 결과 Log 출력*/
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
 desc : 새로운 거버 등록 진행
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

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.Gerber.Regist");

	/* 현재 등록하려는 Recipe가 없는지 여부 */
	if (!IsRecipeGerberCheck())	return ENG_JWNS::en_error;

	/* 현재 등록하려는 거버가 Luria에 등록되어 있는지 여부 */
	if (pstJobMgt->IsJobOnlyFinded(pstRecipe->gerber_name))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"The gerber file to be registered exists");
		return ENG_JWNS::en_error;
	}

	/* 현재 선택된 거버 파일 등록 수행 */
	sprintf_s(szGerbFile, MAX_PATH_LEN, "%s\\%s", pstRecipe->gerber_path, pstRecipe->gerber_name);


	LPG_RAAF pstAlign = uvEng_Mark_GetSelectAlignRecipe();
	ENG_ATGL alignType = (ENG_ATGL)pstAlign->align_type;

	ChangeXml(string(szGerbFile) + "\\rlt_settings.xml", alignType);

	if (!uvEng_Luria_ReqAddJobList(csCnv.Ansi2Uni(szGerbFile)))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to regist the gerber file");
		return ENG_JWNS::en_error;
	}

	/* 등록 후, 바로 등록된 Job List 요청 */
	SetSendCmdTime();
	if (!uvEng_Luria_ReqGetJobList())
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetJobList)");
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : 새로운 거버 등록 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsGerberRegisted()
{
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	LPG_LDJM pstJobMgt = &uvEng_ShMem_GetLuria()->jobmgt;

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Gerber.Registed");

	if (nullptr != pstJobMgt && nullptr != pstRecipe)
	{
		/* 현재 선택된 Recipe의 Job Name이 등록되어 있는지 여부 확인 */
		if (pstJobMgt->IsJobOnlyFinded(pstRecipe->gerber_name))
		{
			return ENG_JWNS::en_next;
		}
	}

	return ENG_JWNS::en_wait;
}

/*
 desc : Led Duty Cycle and Frame Rate 설정
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

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.Step.Duty.Frame");
	/* 만약 데모 버전일 경우 */
	if (uvEng_GetConfig()->IsRunDemo())
	{
		// by sysandj : 변수없음(수정)
		pstJobRecipe->step_size = 4;
		pstExpoRecipe->led_duty_cycle = 10;
		// by sysandj : 변수없음(수정)
		pstJobRecipe->frame_rate = 999;
	}
	/* Step Size, Duty Cycle 및 Frame 값 초기화 */
	pstExpo->SetDutyStepFrame();
	/* Step Size, Duty Cycle 및 Frame 값 설정 요청 */
	// by sysandj : 변수없음(수정)
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
 desc : Led Duty Cycle and Frame Rate 설정 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsStepDutyFrame()
{
	LPG_LDEW pstExpo = &uvEng_ShMem_GetLuria()->exposure;

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Step.Duty.Frame");
	if (!pstExpo->IsSetDutyStepFrame())
	{
		return ENG_JWNS::en_wait;
	}
	return ENG_JWNS::en_next;
}

/*
 desc : 노광 시작 위치 설정
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetExposeStartXY(double* startXoffset, double* startYoffset)
{
	DOUBLE* pStartXY = uvEng_GetConfig()->luria_svc.table_expo_start_xy[0];
	LPG_LDMC pstMachine = &uvEng_ShMem_GetLuria()->machine;


	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.Expose.Start.XY");

	/* 만약 노광 시작 위치 값이 음수이면, 에러 처리 */
	if (pStartXY[0] < 0.0f || pStartXY[1] < 0.0f)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"The exposure start position is incorrect");
		return ENG_JWNS::en_error;
	}

	/* 노광 시작 위치 정보 초기화 */
	pstMachine->table_expo_start_xy[0].x = -1;
	pstMachine->table_expo_start_xy[0].y = -1;

	/* 송신 설정 */
	/* Led Duty Cycle & Frame Rate 설치 */
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
 desc : 노광 시작 위치 설정 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsExposeStartXY()
{
	LPG_LDMC pstMachine = &uvEng_ShMem_GetLuria()->machine;

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Expose.Start.XY");

	/* 노광 시작 위치 값이 확인 되었는지 여부 */
	if (pstMachine->table_expo_start_xy[0].x >= 0 && pstMachine->table_expo_start_xy[0].y >= 0)
	{
		return ENG_JWNS::en_next;
	}

	return ENG_JWNS::en_wait;
}

/*
 desc : Photohead LED Amplitude 값 설정
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


	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.Led.Amplitude");

	/* 레시피에 설정된 Led Power Name을 이용한 Led Power Index 조회 */
	pstLedPower = uvEng_LedPower_GetLedPowerName(csCnv.Ansi2Uni(pstExpoRecipe->power_name));
	if (!pstLedPower)
	{
		swprintf_s(tzMesg, 128, L"Failed to get the Led Power Index (for %s name)",
			csCnv.Ansi2Uni(pstExpoRecipe->power_name));
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
		return ENG_JWNS::en_error;
	}

	/* 기존 설정된 Power 값 초기화 */
	//pstExpose->ResetPowerIndex();

	/* 현재 설정된 Recipe 파워 값 복사 */
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
	/* 통신 시간 초기화 */
	SetSendCmdTime();
	/* Led Duty Cycle 설정 (각 Phothead x개에 각각 LED 4개씩 모두 한번에 설정 */
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
 desc : Photohead LED Amplitude 값 설정 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsLedAmplituded()
{
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	LPG_LDEW pstExpose = &uvEng_ShMem_GetLuria()->exposure;

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Led.Amplituded");

	if (!pstExpose->IsSetPowerIndex(uvEng_GetConfig()->luria_svc.ph_count))
	{
		/* 1.0 초마다 현재 Led Amplitude 요청 */
		if (IsSendCmdTime(1000))	uvEng_Luria_ReqGetLedAmplitude();
		return ENG_JWNS::en_wait;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : 모든 Photohead의 Z Axis을 기본 위치로 이동 하기
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetAllPhMotorMoving()
{
	BOOL bSucc = TRUE;
	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.All.Optic.Motor.Base.Moving");

	/* 기준 소재로 측정된 기준 Focus 위치로 이동 */
	if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x01)
	{
		bSucc = uvEng_Luria_ReqSetAllMotorBaseAbsPosition();
		/* 통신 시간 초기화 */
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
 desc : 모든 Photohead의 Z Axis이 기본 위치로 이동 했는지 여부 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsAllPhMotorMoved()
{
	UINT8 i = 0;
	INT32 i32Set = 0, i32Get = 0;

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.All.Optic.Motor.Moved");
	if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x01)
	{
		/* 너무 자주 요청하면 안되므로 ... */
		if (IsSendCmdTime(250))
		{
			/* 현재 모터 위치 정보 요청 (전체) */
			if (!uvEng_Luria_ReqGetMotorAbsPositionAll())
			{
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetMotorAbsPositionAll)");
				return ENG_JWNS::en_error;
			}
		}
		/* Photohead 개수만큼 기본 위치 이동 완료 여부 확인 */
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
 desc : 모든 Photohead의 Z Axis 모터 초기화 (Homing)
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetAllPhMotorHoming()
{
#ifndef _DEBUG
	LPG_LDDP pstDirectPh = &uvEng_ShMem_GetLuria()->directph;

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.All.Optic.Motor.Homing");
	if (uvEng_GetConfig()->set_uvdi15.load_recipe_homing)
	{
		if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x01)
		{
			/* 기존 광학계 모두가 초기화 안되었다고 설정 */
			pstDirectPh->SetAllMotorInited(uvEng_GetConfig()->luria_svc.ph_count, FALSE);
			/* Z Axis 모터를 초기화하도록 요청 */
			if (!uvEng_Luria_ReqSetMotorPositionInitAll(0x01))
			{
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetMotorPositionInitAll)");
				return ENG_JWNS::en_error;
			}
			/* 통신 시간 초기화 */
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
 desc : 모든 Photohead의 Z Axis 모터가 초기화 완료 되었는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsAllPhMotorHomed()
{
#ifndef _DEBUG
	LPG_LDDP pstDirectPh = &uvEng_ShMem_GetLuria()->directph;

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.All.Optic.Motor.Homed");

	if (!uvEng_GetConfig()->set_uvdi15.load_recipe_homing)	return ENG_JWNS::en_next;

	if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x01)
	{
		/* 모든 광학계가 초기화 되었는지 여부 확인 */
		if (uvCmn_Luria_IsAllMotorMiddleInited(uvEng_GetConfig()->luria_svc.ph_count))
		{
			return ENG_JWNS::en_next;
		}
		if (IsSendCmdTime(250))
		{
			/* 현재 모든 photohead의 모터 상태 정보 요청 */
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
 desc : 기본적으로 드라이브에 에러 값이 있으면, Homing 요청 수행
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::InitDriveErrorReset()
{
	UINT8 i = 0x00;

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Init.Drive.Error.Reset");

	/* 현재 모든 드라이브 중 1개로 에러 코드가 존재한다면, 에러 리셋 처리 */
	for (; i < uvEng_GetConfig()->mc2_svc.drive_count; i++)
	{
		/* 참고로, 에러가 발생된 드라이브의 경우, 반드시 리셋 호출 후 Homing 작업 수행해야 함 */
		/* 결국에 에러 코드만 Reset 시키기 위함이고, 별도로 Homing 작업을 수행해야 함 */

		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI(uvEng_GetConfig()->mc2_svc.axis_id[i]));	/* Toggle 값 저장 */
		if (!uvEng_MC2_SendDevFaultReset(ENG_MMDI(uvEng_GetConfig()->mc2_svc.axis_id[i])))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SendDevFaultReset)");
			ENG_JWNS::en_error;
		}

	}

#if (MC2_DRIVE_2SET == 1)
	UINT8 u8drv_id;
	/* 현재 포토헤드 드라이브 중 1개로 에러 코드가 존재한다면, 에러 리셋 처리 */
	for (i = 0; i < uvEng_GetConfig()->mc2b_svc.drive_count; i++)
	{
		u8drv_id = i + DRIVEDIVIDE;
		/* 참고로, 에러가 발생된 드라이브의 경우, 반드시 리셋 호출 후 Homing 작업 수행해야 함 */
		/* 결국에 에러 코드만 Reset 시키기 위함이고, 별도로 Homing 작업을 수행해야 함 */
		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI(u8drv_id));	/* Toggle 값 저장 */
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
 desc : 모든 드라이브의 에러 값이 초기화 되었는지 확인
 parm : back_step	- [in]  뒤로 이동 후 다시 작업 수행하는 단계 횟수
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsDriveErrorReseted(UINT8 back_step)
{
	UINT8 i = 0x00;

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Init.Drive.Error.Reset");

	/* 현재 MC2 드라이브 쪽에 에러가 계속 남아 있는 상태인지 여부 확인*/
	if (uvCmn_MC2_IsAnyDriveError())	return ENG_JWNS::en_wait;
	/* 현재 모든 드라이브가 DoneToggled 되었는지 확인 */
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
 desc : 임의 드라이브에 대해서 Homing 작업 수행
 parm : drv_id	- [in]  Motion Drive ID
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::DoDriveHoming(ENG_MMDI drv_id)
{
	/* 현재 작업 Step Name 설정 */
	switch (drv_id)
	{
	case ENG_MMDI::en_stage_x:	SetStepName(L"Do.Homing.Stage.X");	break;
	case ENG_MMDI::en_stage_y:	SetStepName(L"Do.Homing.Stage.Y");	break;
	case ENG_MMDI::en_align_cam1:	SetStepName(L"Do.Homing.ACam.1");	break;
	case ENG_MMDI::en_align_cam2:	SetStepName(L"Do.Homing.ACam.2");	break;
	case ENG_MMDI::en_axis_ph1:	SetStepName(L"Do.Homing.PH.1");		break;
	case ENG_MMDI::en_axis_ph2:	SetStepName(L"Do.Homing.PH.2");		break;
	}

#if 0	/* 여기서는 드라이브 에러 체크하지 않음 */
	/* 만약 임의 드라이브에 에러 코드가 존재하면, Homing 작업 실패 처리 */
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
 desc : 모든 드라이브에 대해서 Homing 작업 수행
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::DoDriveHomingAll()
{
	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Do.Homing.All");

#if 0	/* 여기서는 드라이브 에러 체크하지 않음 */
	/* 만약 임의 드라이브에 에러 코드가 존재하면, Homing 작업 실패 처리 */
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
 desc : 임의 모션 드라이브가 Homing 완료 되었는지 여부
 parm : drv_id	- [in]  Motion Drive ID (EN_MMDI)
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsDrivedHomed(ENG_MMDI drv_id)
{
	/* 현재 작업 Step Name 설정 */
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
		/* 어쩔수 없이 시간 갱신 (만약 Homing 안될 때 무한 대기 됨) */
		m_u64DelayTime = GetTickCount64();
		return ENG_JWNS::en_wait;
	}
	return ENG_JWNS::en_next;
}

/*
 desc : 모든 드라이브가 Homing 되었는지 확인
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

	/* 현재 작업 Step Name 설정 */
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
 desc : Align Camera 2번이 Homing 도중 Limit을 체크될 때, 1100 에러 확인
 parm : cam_no	- [in]  Align Camera Number (1 or 2)
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::WaitACamError(UINT8 cam_no)
{
	ENG_MMDI enDrvID = cam_no == 1 ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;
	LPG_MDSM pstMC2 = uvEng_ShMem_GetMC2();

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Wait.Camera.Error.Check");

	/* 데모 모드로 동작 중인지 여부에 따라 */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

	/* 만약에 이미 Homing 완료되었다면 어쩔 수 없다. 그냥 다음 단계로 넘어가면 된다. */
	if (uvCmn_MC2_IsDriveHomed(enDrvID))
	{
		return ENG_JWNS::en_next;
	}

	/* Align Camera x번 1100 에러 (Homing 도중 Limit 도달 할 때, 발생되는 에러 값) 확인 */
	/* 물론 이때, 여타 다른 드라이브도 모두 Homing 에러가 발생되는 상황. 무시함 */
	if (0x1100 == uvCmn_MC2_GetDriveError(enDrvID))
	{
		return ENG_JWNS::en_next;
	}

	return ENG_JWNS::en_wait;
}

/*
 desc : Align Camera 2번이 Homing 도중 강제로 Homing 중지 시킴
 parm : None
 retn : wait, error, complete or next
 note : 카메라 1번과 충돌 발생이 생길 수 있기 때문에... (Linear Scale을 공유하기 때문임 T.T)
*/
ENG_JWNS CWorkStep::ACam2HomingStop()
{
	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Align.Camera.2.Homing.Stop");

	/* 데모 모드로 동작 중인지 여부에 따라 */
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

	if (0x1100 == uvEng_ShMem_GetMC2()->GetErrorCodeOne(UINT8(ENG_MMDI::en_align_cam2)))
	{
		/* 강제로 이동 중단 */
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
 desc : Luria 내의 System - Initialize.Hardware 정보 설정
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::InitializeHWInit()
{
	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Initialized.H/W.Init");

	/* 현재 Luria Service에 연결되어 있는지 여부 */
	if (!uvCmn_Luria_IsConnected())
	{
		return ENG_JWNS::en_wait;
	}
	/* 송신 작업 진행 */
	if (!uvEng_Luria_ReqSetHWInit())
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetHWInit)");
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : QuartZ 위치에 있는 임의 Align Camera가 잘 보이도록 Z AXis 위치 (Focusing) 이동
 parm : title	- [in]  현재 수행 단계 출력 여부
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetACamMovingZAxisQuartz()
{
	UINT8 u8CamID = (m_enDrvACamID == ENG_MMDI::en_align_cam1) ? 0x01 : 0x02;
	DOUBLE dbACamSetZ = 0.0f;

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"ACam.Moving.Z.Axis.Quartz");

	/* Quartz 에 맞게 Focusing 위치 이동 */
	dbACamSetZ = uvEng_GetConfig()->acam_spec.acam_z_focus[u8CamID - 1];
	/* Align Camera Z Axis을 절대 위치로 이동 */
	// by sysandj : MCQ대체 추가 필요
// 	if (!uvEng_MCQ_SetACamMovePosZ(u8CamID, 0x00, dbACamSetZ))
// 	{
// 		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ACamMovePosZ)");
// 		return ENG_JWNS::en_error;
// 	}

	return ENG_JWNS::en_next;
}

/*
 desc : Quartz 의 Z Axis이 모두 이동 했는지 여부 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsACamMovedZAxisQuartz()
{
	UINT8 u8CamID = (m_enDrvACamID == ENG_MMDI::en_align_cam1) ? 0x01 : 0x02;
	INT32 i32ACamSetZ = 0, i32ACamNowZ = 0;
	//LPG_PMRW pstPLC		= uvEng_ShMem_GetPLCExt();

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.ACam.Moved.Z.Axis.Quartz");

	/* 현재 Align Camera Z 축의 위치 값 (0.1 um or 100 nm --> um) */
	// by sysandj : MCQ대체 추가 필요
	//if (0x01 == u8CamID)	i32ACamNowZ	= (INT32)ROUNDED(pstPLC->r_camera1_z_axis_position_display / 10.0f, 0);
	//else					i32ACamNowZ	= (INT32)ROUNDED(pstPLC->r_camera2_z_axis_position_display / 10.0f, 0);

	/* 최종 이동해야 할 Align Camera의 Z 위치 값 (단위: mm --> um) */
	i32ACamSetZ = (INT32)ROUNDED(uvEng_GetConfig()->acam_spec.acam_z_focus[u8CamID - 1] * 1000.0f, 0);

	/* 원래 이동하고자 했던 위치 값과 현재 이동한 위치 값 비교 (1 um 단위로 비교) */
	if (i32ACamSetZ != i32ACamNowZ)
	{
		return ENG_JWNS::en_wait;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : 현재 이동하고자 하는 카메라가 아닌 경우, 맨 왼쪽 or 오른쪽 끝으로 이동 처리
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetACamMovingSide()
{
	INT32 i32MovePos = 0;
	UINT32 u32MoveSpeed = 0;
	ENG_MMDI enACamDrv = (m_enDrvACamID == ENG_MMDI::en_align_cam1) ? ENG_MMDI::en_align_cam2 : ENG_MMDI::en_align_cam1;

	/* 현재 작업 Step Name 설정 */
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

	/* 현재 이동하고자 하는 모션의 Toggled 값 얻기 */
	uvCmn_MC2_GetDrvDoneToggled(enACamDrv);
	/* 사이드로 이동 시키기 */
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
 desc : 맨 끝으로 Align Camera가 이동했는지 여부 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsACamMovedSide()
{
	ENG_MMDI enACamDrv = (m_enDrvACamID == ENG_MMDI::en_align_cam1) ? ENG_MMDI::en_align_cam2 : ENG_MMDI::en_align_cam1;

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.ACam.Moved.Side");

	if (!uvCmn_MC2_IsDrvDoneToggled(enACamDrv))
	{
		return ENG_JWNS::en_wait;
	}
	return ENG_JWNS::en_next;
}

/*
 desc : Align Camera를 Quartz가 있는 위치로 이동
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetACamMovingQuartz()
{
	UINT8 u8CamID = (m_enDrvACamID == ENG_MMDI::en_align_cam1) ? 0x01 : 0x02;
	DOUBLE dbACamMovePos = 0.0f, dbXMovePos = 0.0f, dbYMovePos = 0.0f;
	DOUBLE dbACAmSpeed = 0.0f, dbXMoveSpeed = 0.0f, dbYMoveSpeed = 0.0f;
	LPG_CASI pstACamSpec = &uvEng_GetConfig()->acam_spec;

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.ACam.Moving.Quartz");

	/* Quartz 위치 */
	dbXMovePos = pstACamSpec->quartz_stage_x;
	dbYMovePos = pstACamSpec->quartz_stage_y[u8CamID - 1];
	dbACamMovePos = pstACamSpec->quartz_acam[u8CamID - 1];
	/* Speed */
	dbXMoveSpeed = uvEng_GetConfig()->mc2_svc.max_velo[UINT8(ENG_MMDI::en_stage_x)];
	dbYMoveSpeed = uvEng_GetConfig()->mc2_svc.max_velo[UINT8(ENG_MMDI::en_stage_y)];
	dbACAmSpeed = uvEng_GetConfig()->mc2_svc.max_velo[UINT8(m_enDrvACamID)];

	/* 현재 이동하고자 하는 모션의 Toggled 값 얻기 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	uvCmn_MC2_GetDrvDoneToggled(m_enDrvACamID);

	/* Motion 이동 시키기 */
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
 desc : Quartz 위치로 모두 이동이 완료 되었는지 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsACamMovedQuartz()
{
	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.ACam.Moved.Quartz");

	if (!uvCmn_MC2_IsDrvDoneToggled(m_enDrvACamID))			return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_x))	return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : 노광할 준비가 되어 있는지 장비 및 레시피 확인
 parm : in_mark	- [in]  TRUE:마크 존재 유무 검사, FALSE:마크 확인하지 않음
		detect	- [in]  소재 검사 유무
		vaccum	- [in]  Vacuum 검사 유무
		count	- [in]  노광 횟수
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetExposeReady(BOOL in_mark, BOOL detect, BOOL vaccum, UINT32 count)
{
	STG_CGTI stTrans = { NULL };
	LPG_CGTI pstTrans = &uvEng_GetConfig()->global_trans;

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Init.Processing");

	/* 현재 거버가 적재되어 있는지 확인 */
	if (!uvCmn_Luria_IsJobNameLoaded())
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"The gerber file did not complete loading");
		return ENG_JWNS::en_error;
	}
	/* 현재 Loading 되어 있는 거버에 Mark가 포함되어 있는지 확인 */
	if (in_mark && uvEng_Luria_GetMarkCount(ENG_AMTF::en_global) < 1)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"The global mark does not exist");
		return ENG_JWNS::en_error;
	}
	/* 기존 노광 작업 (PrePrinting, Printing, Aborting) 진행 중이면, 강제 종료 처리 */
	if (!uvEng_Luria_ReqSetPrintOpt(0x02))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetPrintOpt.Abort)");
		return ENG_JWNS::en_error;
	}
	/* MC2 에러 여부 확인 */
	if (IsMC2ErrorCheck())					return ENG_JWNS::en_error;
	/* Recipe 선태 여부 확인 */
	if (!IsSelectedRecipeValid())			return ENG_JWNS::en_error;
	/* Luria 에러 여부 확인 */
	if (IsLuriaStatusError())				return ENG_JWNS::en_error;
	/* Mark 등록 여부 확인 */
	if (in_mark && !IsAlignMarkRegisted())	return ENG_JWNS::en_error;
	/* 노광할 소재가 놓여 있는지 확인 */
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
		/* 등록된 트리거 위치 값이 초기화 되었는지 여부 */
		//if (!IsTrigPosResetAll())	return ENG_JWNS::en_wait;
		/* 광학계 위치가 Home Position에 있는지 여부 ... (광학계2대의 높이가 모두 동일한 것으로 비교) */
		//if (uvEng_ShMem_GetLuria()->IsAllEqualPHZAxis(uvEng_GetConfig()->luria_svc.ph_count))
		//{
		//	LOG_ERROR(ENG_EDIC::en_uvdi15, L"Check the z axis height of all photoheads");
		//	return ENG_JWNS::en_error;
		//}
		/* 모션 드라이브 정지 유무 확인 */
		if (!uvCmn_MC2_IsMotorDriveStopAll())
		{
			if (count < 2)	LOG_WARN(ENG_EDIC::en_uvdi15, L"Any motor is not stopped");
			return ENG_JWNS::en_wait;
		}

#if (CUSTOM_CODE_WAIT_JOB == 0)
		/* Vacuum가 On 상태가 아닌지 여부 확인 */
		if (vaccum && !IsVacuumRunning())
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"The vacuum is not running");
			return ENG_JWNS::en_error;
		}
		/* Chiller가 On 상태가 아닌지 여부 확인 */
		if (!IsChillerRunning())
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"The chiller is not running");
			return ENG_JWNS::en_error;
		}
		/* 장비 전면의 Start Button 2 곳에 LED가 On 되었는지 여부 */
		// by sysandj : MCQ대체 추가 필요
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
 desc : 얼라인 카메라 2번을 맨 가장 자리로 이동
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetMovingSideACam2()
{
	INT32 i32ACamPos2 = 0;
	UINT32 u32VeloACamX;

	SetStepName(L"Set.Moving.Side.ACam2");
	/* 현재 모션 드라이브의 Toggle 상태 값 저장 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam2);

	/* 각 Axis 별로 기본 동작 속도 값 얻기 */
	u32VeloACamX = (UINT32)ROUNDED(uvEng_GetConfig()->mc2_svc.max_velo[UINT8(ENG_MMDI::en_align_cam2)] * 10000.0f, 0);
	/* 2 번 카메라 위치 맨 마지막으로 이동 시킴 */
	i32ACamPos2 = (INT32)ROUNDED(uvEng_GetConfig()->mc2_svc.max_dist[UINT8(ENG_MMDI::en_align_cam2)] * 10000.0f, 0);	/* 0.1 um or 100 ns */
	/* Align Camera 이동 하기 */
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2,
		i32ACamPos2 / 10000.0f, u32VeloACamX / 10000.0f))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SendDevAbsMove)");
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : 얼라인 카메라 2번이 다 이동 되었는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsMovedSideACam2()
{
	/* 일정 시간 대기 (카메라 모터가 움직일 때까지 일정 시간 대기) */
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam2))
	{
		return ENG_JWNS::en_wait;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : 얼라인 카메라 1번을 특정 Mark Number 위치로 이동
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

	/* 현재 모션 드라이브의 Toggle 상태 값 저장 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);

	/* 최종 이동하고자 하는 모션 위치 값 얻기 */
	GetMovePosGlobalMarkACam1(mark_no, i32PosACamX, i32PosStageY);
	/* 이동하고자 하는 위치 정보 임시 등록 */
	uvEng_ACamCali_AddMarkPos(1, ENG_AMTF::en_global, 0x00, mark_no, i32PosACamX);
	uvEng_ACamCali_AddMarkPos(1, ENG_AMTF::en_global, 0x01, mark_no, GetGlobalMarkMotionPosY(mark_no));
	/* 각 Axis 별로 기본 동작 속도 값 얻기 */
	u32VeloACamX = (UINT32)ROUNDED(uvEng_GetConfig()->mc2_svc.max_velo[UINT8(ENG_MMDI::en_align_cam1)] * 10000.0f, 0);
	u32VeloStageY = (UINT32)ROUNDED(uvEng_GetConfig()->mc2_svc.max_velo[UINT8(ENG_MMDI::en_stage_y)] * 10000.0f, 0);
	/* 무조건 Align Camera 1번과 Stage Y 이동 하기 */
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
 desc : 얼라인 카메라 1번이 다 이동 되었는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsMovedMarkACam1()
{
	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"IsMoved.Mark.ACam1");

	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1))	return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))		return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : 모든 얼라인 카메라에서 캡처된 이미지의 개수 비교
 parm : count	- [in]  비교하고자 하는 이미지의 개수 (이 값과 동일하게 grabbed 되었는지 여부)
		delay	- [in]  Grabbed Image 개수가 모두 도착하는데 대기하는 시간 (단위: msec)
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsGrabbedImageCount(UINT16 count, UINT64 delay, int* camNum)
{
	UINT16 u16Grab = 0;

	u16Grab = uvEng_Camera_GetGrabbedCount(camNum);

	/*Camera 정지 모드*/
	uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
	/*Mc2 트리거 모드 노광 모드로 변경*/
	uvEng_Luria_ReqSetHWInit();

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())
	{
		TCHAR tzTitle[64] = { NULL };
		swprintf_s(tzTitle, 64, L"Is.Grabbed.Image.Count (%u/%u)", count, u16Grab);
		SetStepName(tzTitle);
	}

	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;


	//abh1000 Local Test
	///* 임의 시간 동안 캡처된 이미지 개수가 없다면 에러 처리 */
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
 desc : 얼라인 카메라 1번에 강제로 (수동으로) Trigger 1개를 발생
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetTrigOutOneACam(UINT8 cam_id)
{
	SetStepName(L"Set.Trig.Out.One (G)");

	UINT8 u8LampType = 0x00;		/* 0x00 : AMBER(1 ch, 3 ch), 0x01 : IR (2 ch, 4 ch) */
	u8LampType = uvEng_GetConfig()->set_comn.strobe_lamp_type;

	/* Channel 1 번에 트리거 1개 발생 후 바로 Trigger Disabled 수행 */
	if (!uvEng_Mvenc_ReqTrigOutOne(cam_id))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

bool CWorkStep::SetAutoFocusFeatures()
{
	if (uvEng_GetConfig()->luria_svc.useAF == false) return true;

	bool res = true;

	//오토포커스 설정



	return res;
	//여기서 설정.
}


/*
 desc : 레시피 관련 연결된 정보들이 유효한지 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::CheckValidRecipe()
{
	TCHAR tzMesg[128] = { NULL };

	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	// by sysandj : 변수없음(수정)
	LPG_MACP pstThick = uvEng_ThickCali_GetRecipe(pstRecipe->cali_thick);
	// by sysandj : 변수없음(수정)
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
 desc : 현재 등록된 Job Name을 선택 합니다.
 parm : index	- [in]  0 or Later
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetSelectedJobIndex(UINT8 index)
{
	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.Selected.Job.Index (0)");

	if (uvCmn_Luria_GetJobCount() <= index)	return ENG_JWNS::en_error;
#if 0
	/* 등록된 Job Name이 없다면 바로 리턴 */
	if (uvCmn_Luria_GetJobCount() < 1)	return ENG_JWNS::en_next;
#endif
	/* Job Selected 요청 */
	if (!uvEng_Luria_ReqSetSelectedJobIndex(index))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetSelectedJobIndex)");
		ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}
/*
 desc : Job Name이 선택되었는지 확인
 parm : index	- [in]  0 or Later
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsSelectedJobIndex()
{
	INT32 i32Index = 0x00;
	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.Selected.Job.Index (0)");
#if 0
	/* 등록된 Job Name이 없다면 바로 리턴 */
	if (uvCmn_Luria_GetJobCount() < 1)	return ENG_JWNS::en_next;
#endif	
	/* Job Selected 요청 */
	if (!uvEng_Luria_IsRecvPktData(ENG_FDPR::en_selected_job))
	{
		ENG_JWNS::en_wait;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : 등록된 거버 중 가장 마지막에 위치 (등록)한 거버를 삭제하도록 요청
 parm : time	- [in]  Waiting time for a response (unit: msec)
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::SetDeleteSelectedJobName(UINT32 time)
{
	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.Delete.Selected.Job.Name");
#if 0
	/* 등록된 Job Name이 없다면 바로 리턴 */
	if (uvCmn_Luria_GetJobCount() < 1)	return ENG_JWNS::en_next;
#endif
	/* Job Name 삭제 전에, 등록된 Job 개수 임시 저장 */
	m_u8LastJobCount = uvCmn_Luria_GetJobCount();

	/* Job Delete 요청 */
	if (m_u8LastJobCount && !uvEng_Luria_ReqSetDeleteSelectedJob())
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetDeleteSelectedJob)");
		ENG_JWNS::en_error;
	}
#if 0
	/* Job List 요청 */
	if (m_u8LastJobCount && !uvEng_Luria_ReqGetJobList())
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetJobList)");
		ENG_JWNS::en_error;
	}
#endif
	/* 명령 송신 후 답변이 올 때까지 일정 시간 대기 */
	if (time)	SetWorkWaitTime(time);

	return ENG_JWNS::en_next;
}

/*
 desc : Job Deleted 수신 여부 확인
 parm : back_step	- [in]  다시 이전 작업 위치로 이동할 Step Number
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsDeleteSelectedJobName(UINT8 back_step)
{
	ENG_JWNS enState = IsWorkWaitTime();

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Deleted.Last.Job.Name");
#if 0
	/* 등록된 Job Name이 없다면 바로 리턴 */
	if (uvCmn_Luria_GetJobCount() < 1)	return ENG_JWNS::en_next;
#endif
	/* 일정 시간 대기 */
	if (enState != ENG_JWNS::en_next)	return ENG_JWNS::en_wait;

	/* 삭제 전 개수와 삭제 후 개수가 다르면, Job이 삭제된 것임 */
	if (m_u8LastJobCount && m_u8LastJobCount == uvCmn_Luria_GetJobCount())
	{
		SetWorkWaitTime(1000);
		/* Job List 요청 */
		if (m_u8LastJobCount && !uvEng_Luria_ReqGetJobList())
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetJobList)");
			ENG_JWNS::en_error;
		}

		/* 이전 단계에서 요청한 작업에 대해 응답이 일정 시간 없는 경우인지 확인 */
		return ENG_JWNS::en_wait;
	}
	/* 현재 수신된 Job List가 있는지 확인 */
	if (uvCmn_Luria_GetJobCount() > 0)
	{
		switch (m_enWorkJobID)
		{
		case ENG_BWOK::en_gerb_load:
		case ENG_BWOK::en_gerb_unload:	m_u8StepIt = back_step;	break;	/* 다시 처음부터 삭제 작업 수행 */
		}
	}

	return ENG_JWNS::en_next;
}

/*
 desc : 모션 컨트롤러의 Hysteresis의 Offset & Delay (Positive & Negative) 설정
 parm : offset	- [in]  기존 단차 Calibration (Ph.Step)정보에 적용된 값 적용 여부
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
	// by sysandj : 변수없음(수정)
	LPG_OSSD pstPhStep = uvEng_PhStep_GetRecipeData(pstRecipe->frame_rate);

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Hysteresis");
#if 0
	i32NegaOffset = pstPhStep->nega_offset_px;
	u32DelayNega = pstPhStep->delay_posi_nsec;
	u32DelayPosi = pstPhStep->delay_nega_nsec;

	/* 아래 정보는 단차 노광을 위한 환경 설정 임 */
	if (0xff != offset)
	{
		i32NegaOffset = offset;
		u32DelayNega = 0;
		u32DelayPosi = 0;
	}

	/* Step 정보 확인 */
	if (!pstPhStep)
	{
		// by sysandj : 변수없음(수정)
		swprintf_s(tzMesg, 128, L"Failed to search the file of photohead step [frame_rate=%u]",
			pstRecipe->frame_rate);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
		return ENG_JWNS::en_error;
	}

	/* Hysterisis 값 초기화 */
	pstMemMach->ResetHysteresisType1();
	/* Hysterisis 값 설정 */
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
 desc : 모션 컨트롤러의 Hysteresis의 Offset & Delay (Positive & Negative) 설정 여부 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkStep::IsSetHysteresis()
{
#if 0
	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Set.Hysteresis");
	return uvCmn_Luria_IsHysteresisType1() ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
#endif

	return ENG_JWNS::en_next;
}

/*
 desc : 모든 카메라 (채널)의 트리거 위치가 초기화 되었는지 여부
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
 desc : Safety PLC 초기화 (Reset)
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

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Reset.MC2.Drive");
	/* Reset 명령어 송신 */
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
