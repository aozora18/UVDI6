
/*
 desc : Align (Global or Local) Mark 위치 (Left / Bottom)로 이동 - 얼라인 카메라 이동 없이 스테이지 Y 축만 이동
*/

#include "pch.h"
#include "../../MainApp.h"
#include "WorkMarkMove.h"
#include "../../GlobalVariables.h"
#include <bitset>

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : 생성자
 parm : mark_no	- [in]  Global Mark: 모션이 이동하고자 하는 Mark Number (0x01 ~ 0x04)
 retn : None
*/
CWorkMarkMove::CWorkMarkMove(UINT16 mark_no)
	: CWorkStep()
{
	int options = (mark_no & ~0b0000000000001111) >> 4;
	int markNum = mark_no & ~0b1111111111110000;

	includeAlignOffset = options;
	mark_no = markNum;
	m_enWorkJobID	= ENG_BWOK::en_mark_move;
	m_u8MarkNo		= mark_no;
	m_u8ACamID		= mark_no > 2 ? 0x02 : 0x01;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CWorkMarkMove::~CWorkMarkMove()
{
}


/*
 desc : 초기 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkMarkMove::InitWork()
{
	/* 내부 멤버 변수 값 초기화 */
	if (!CWork::InitWork())	return FALSE;

	CUniToChar csCnv;
	bool isLocalSelRecipe = uvEng_JobRecipe_WhatLastSelectIsLocal();
	LPG_RJAF rcp = uvEng_JobRecipe_GetSelectRecipe(isLocalSelRecipe);
	LPG_RAAF pstRecipeAlign = uvEng_Mark_GetAlignRecipeName(csCnv.Ansi2Uni(rcp->align_recipe));

	SetAlignMode((ENG_AMOS)pstRecipeAlign->align_motion, (ENG_ATGL)pstRecipeAlign->align_type);


	uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
	
	return TRUE;
}

/*
 desc : 주기적으로 작업 수행
 parm : None
 retn : None
*/
VOID CWorkMarkMove::DoWork()
{
	const int Initstep = 0, processWork = 1, checkWorkstep = 2;
	try
	{
		markMoveCallback[alignMotion][processWork]();
		markMoveCallback[alignMotion][checkWorkstep]();
		CheckWorkTimeout();
	}
	catch (const std::exception&){}
}

/// <summary>
///							진행.
/// </summary>
void CWorkMarkMove::DoMovingOnthefly2cam()
{
	switch (m_u8StepIt)
	{
	case 0x01 : 
	{
		uvEng_Camera_ResetGrabbedImage();
		m_enWorkState = IsSetTrigPosResetAll();
		uvEng_Camera_SetCamMode(ENG_VCCM::en_grab_mode);/* Grab Mode 설정 */
	}
	break;
	case 0x02 : m_enWorkState = IsMotorDriveStopAll();		break;
	case 0x03 : m_enWorkState = SetTrigEnable(FALSE);		break;
	case 0x04 : m_enWorkState = IsTrigEnabled(FALSE);		break;
	case 0x05 : m_enWorkState = IsLoadedGerberCheck();		break;
	case 0x06 : m_enWorkState = SetAlignMovingInit();		break;
	case 0x07 : m_enWorkState = IsAlignMovedInit();			break;
	case 0x08 : m_enWorkState = SetMovingAlignMark();		break;
	case 0x09 : 
	{
		uvEng_Camera_SetCamMode(ENG_VCCM::en_none);/* Grab Mode 설정 */ 
		m_enWorkState = IsMovedAlignMark();
	}
	break;
	}
	
	
}

void CWorkMarkMove::DoMovingOnthefly3cam()
{

}

void CWorkMarkMove::DoMovingStatic2cam()
{

}

STG_XMXY markPos = { STG_XMXY(), };

void CWorkMarkMove::DoMovingStatic3cam()
{
	/* 작업 단계 별로 동작 처리 */
	
	auto& motions = GlobalVariables::GetInstance()->GetAlignMotion();
	const int CENTERCAM = motions.markParams.centerCamIdx;
	
	switch (m_u8StepIt)
	{
	case 0x01:
	{
		uvEng_Camera_ResetGrabbedImage();
		m_enWorkState = IsSetTrigPosResetAll();
		//uvEng_Camera_SetCamMode(ENG_VCCM::en_grab_mode);/* Grab Mode 설정 */
	}
	break;
	case 0x02: m_enWorkState = IsMotorDriveStopAll();		break;
	case 0x03: m_enWorkState = SetTrigEnable(FALSE);		break;
	case 0x04: m_enWorkState = IsTrigEnabled(FALSE);		break;
	case 0x05: m_enWorkState = ENG_JWNS::en_next; break; //IsLoadedGerberCheck();		break;
	case 0x06:
	{
		
		if (uvEng_Luria_GetGlobalMark(m_u8MarkNo - 1, &markPos))
		{
			CaliPoint alignOffsetPos,expoOffsetPos,accumOffsets;

			alignOffsetPos = CaliPoint(0, 0, 0, 0);
			accumOffsets = CaliPoint(0, 0, 0, 0);
			expoOffsetPos = CaliPoint(0, 0, 0, 0);
			if(CommonMotionStuffs::GetInstance().GetOffsetsUseMarkPos(CENTERCAM, markPos, &alignOffsetPos, &expoOffsetPos) == false)
			{
				m_enWorkState = ENG_JWNS::en_error;
				return;
			}
			
			accumOffsets = includeAlignOffset == 1 ?  alignOffsetPos  : 
				includeAlignOffset == 2 ? alignOffsetPos + (expoOffsetPos - alignOffsetPos) : CaliPoint(0, 0, 0, 0);

			auto arrival = motions.MovetoGerberPos(CENTERCAM, markPos, &accumOffsets);
			m_enWorkState = arrival == true ? ENG_JWNS::en_next : ENG_JWNS::en_error;
		}
		else
			m_enWorkState = ENG_JWNS::en_error;
	}
	break;

	case 0x07:
		//string temp = "x" + std::to_string(CENTERCAM);
		//motions.Refresh();
		//auto alignOffset = motions.EstimateAlignOffset(CENTERCAM,
		//	motions.GetAxises()["stage"]["x"].currPos,
		//	motions.GetAxises()["stage"]["y"].currPos,
		//	CENTERCAM == 3 ? 0 : motions.GetAxises()["cam"][temp.c_str()].currPos);

		//auto expoOffset = motions.EstimateExpoOffset(markPos.mark_x, markPos.mark_y);


		uvEng_Camera_SetCamMode(ENG_VCCM::en_none);/* Grab Mode 설정 */
		m_enWorkState = ENG_JWNS::en_next;
		//RESERVE
	break;
	}


}


/// <summary>
///							스텝관리
/// </summary>
void CWorkMarkMove::SetWorkNextOnthefly2cam()
{
	/* 다음 작업 진행 여부 판단 */
	CWork::SetWorkNext();
	/* 장시간 동안 동일 위치를 반복 수행한다면 에러 처리 */
	CheckWorkTimeout();
}

void CWorkMarkMove::SetWorkNextOnthefly3cam()
{
	/* 다음 작업 진행 여부 판단 */
	CWork::SetWorkNext();
	/* 장시간 동안 동일 위치를 반복 수행한다면 에러 처리 */
	CheckWorkTimeout();
}

void CWorkMarkMove::SetWorkNextStatic3cam()
{
	/* 다음 작업 진행 여부 판단 */
	CWork::SetWorkNext();
	/* 장시간 동안 동일 위치를 반복 수행한다면 에러 처리 */
	CheckWorkTimeout();
}

void CWorkMarkMove::SetWorkNextStatic2cam()
{
	/* 다음 작업 진행 여부 판단 */
	CWork::SetWorkNext();
	/* 장시간 동안 동일 위치를 반복 수행한다면 에러 처리 */
	CheckWorkTimeout();
}



/// <summary>
///							초기화
/// </summary>
void CWorkMarkMove::DoInitStatic2cam()
{

}

void CWorkMarkMove::DoInitStatic3cam()
{
	/* 전체 작업 단계 */
	m_u8StepTotal = 0x07;
}

void CWorkMarkMove::DoInitOnthefly3cam()
{
	
}

void CWorkMarkMove::DoInitOnthefly2cam()
{
	
	m_u8StepTotal = 0x09;
	GlobalVariables::GetInstance()->GetAlignMotion().SetFiducialPool();

}

/*
 desc : Align Mark가 존재하는 시작 위치로 이동 수행
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkMarkMove::SetMovingAlignMark()
{
	TCHAR tzMesg[128]	= {NULL};
	UINT8 u8Mark		= 0x00, u8ACamID = 0;
	DOUBLE dbPosStageY	= 0.0f, dbPosACamX = 0.0f, dbPosACamX1 = 0.0f, dbPosACamX2 = 0.0f;
	DOUBLE dbVeloStageY	= 0.0f, dbVeloACamX = 0.0f/*, dbCaliACamX, dbCaliACamY*/;
	ENG_MMDI enDrvACam	= ENG_MMDI::en_align_cam1;
	LPG_CIEA pstCfg		= uvEng_GetConfig();
	
	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Moving.Align.Mark");

	/* Global Mark 개수에 따라 */
	u8Mark	= uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	if (u8Mark != 4)
	{
		swprintf_s(tzMesg, 128, L"The number of mark is incorrect (global = %u)", u8Mark);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
		return ENG_JWNS::en_error;
	}

	/* 이동 대상의 카메라 선택 */
	u8ACamID	= 0x01;
	if (m_u8MarkNo > 1)
	{
		enDrvACam	= ENG_MMDI::en_align_cam2;
		u8ACamID	= 0x02;
	}

	/* 각 Axis 별로 기본 동작 속도 값 얻기 */
	dbVeloACamX	= pstCfg->mc2_svc.max_velo[UINT8(enDrvACam)];
	dbVeloStageY= pstCfg->mc2_svc.max_velo[UINT8(ENG_MMDI::en_stage_y)];
	
	/* 최종 이동하고자 하는 모션 위치 값 얻기 */
//	GetMovePosGlobalMark(m_u8MarkNo, i32PosACamX, i32PosStageY);
	if (!GetGlobalMarkMoveXY(m_u8MarkNo, dbPosACamX, dbPosStageY))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to get a position to move motion for align mark measurement");
		return ENG_JWNS::en_error;
	}
#if 0	/* 단순 마크 이동할 때는 2D Calibration Data 적용하지 않음 */
	if (uvEng_GetConfig()->set_align.use_2d_cali_data)
	{
		if (!uvEng_ACamCali_GetCaliPos(u8ACamID, dbPosACamX, dbPosStageY, dbCaliACamX, dbCaliACamY))
		{
			swprintf_s(tzMesg, 128, L"The calibration position does not find from calibration file");
			return ENG_JWNS::en_error;
		}
		dbPosACamX	+= dbCaliACamX;
		dbPosStageY	+= dbCaliACamY;
	}
#endif
	/* 모든 Motor Drive의 토글 값 저장 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam2);
	/* Stage Moving (Vector) */

	AlignMotion& alignMotion = GlobalVariables::GetInstance()->GetAlignMotion();

	auto stageX =  alignMotion.GetAxises()["stage"]["x"].currPos;
	//auto exti =  alignMotion.EstimateOffset(1, dbPosStageY, stageX, dbPosACamX);

	if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_y, dbPosStageY))
	{
		return ENG_JWNS::en_error;
	}
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dbPosStageY, dbVeloStageY))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SendDevAbsMove.StageY)");
		return ENG_JWNS::en_error;
	}

	/* 카메라 1번에 해당되는 Mark (1, 2)를 찾는 경우라면 */
	if (m_u8MarkNo < 3)
	{
		/* 2 번 카메라 위치 맨 마지막으로 이동 시킴 */
		dbPosACamX2	= pstCfg->mc2_svc.max_dist[UINT8(ENG_MMDI::en_align_cam2)];	/* 0.1 um or 100 ns */
		dbPosACamX1	= dbPosACamX;
	}
	/* 카메라 2번에 해당되는 Mark (3, 4)를 찾는 경우라면 */
	else
	{
		/* 1 번 카메라 위치 맨 처음으로 이동 시킴 */
		dbPosACamX1	= 0;	/* 0.1 um or 100 ns */
		dbPosACamX2	= dbPosACamX;
	}
#if 1
	TRACE(L"SetMovingAlignMark: ACam1 = %.4f mm\t ACam2 = %.4f mm\n", dbPosACamX1, dbPosACamX2);
#endif
	/* 현재 카메라 1 번의 절대 위치 기준 대비, 이동하고자 하는 절대 위치 값이 큰 값인지 혹은 */
	/* 작은 값인지 여부에 따라, Align Camera 1 혹은 2 번 중 어느 것을 먼저 이동해야 할지 결정*/
	dbPosACamX	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam1);
#if 1
	if (dbPosACamX1 > dbPosACamX)
#else
	if (m_u8MarkNo == 0x00 || m_u8MarkNo == 0x01)
#endif
	{
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2, dbPosACamX2, dbVeloACamX))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SendDevAbsMove.ACamX2)");
			return ENG_JWNS::en_error;
		}
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, dbPosACamX1, dbVeloACamX))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SendDevAbsMove.ACamX1)");
			return ENG_JWNS::en_error;
		}
	}
	else
	{
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, dbPosACamX1, dbVeloACamX))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SendDevAbsMove.ACamX1)");
			return ENG_JWNS::en_error;
		}
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2, dbPosACamX2, dbVeloACamX))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SendDevAbsMove.ACamX2)");
			return ENG_JWNS::en_error;
		}
	}
	return ENG_JWNS::en_next;
}

/*
 desc : Align Mark가 존재하는 시작 위치로 이동했는지 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkMarkMove::IsMovedAlignMark()
{
	/* 현재 작업 Step Name 설정 */
	if (IsWorkRepeat())	SetStepName(L"Is.Moved.Align.Mark");

	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))		return IsWorkWaitTime();
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1))	return IsWorkWaitTime();
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam2))	return IsWorkWaitTime();

	return ENG_JWNS::en_next;
}