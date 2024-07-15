
/*
 desc : Align (Global or Local) Mark ��ġ (Left / Bottom)�� �̵� - ����� ī�޶� �̵� ���� �������� Y �ุ �̵�
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
 desc : ������
 parm : mark_no	- [in]  Global Mark: ����� �̵��ϰ��� �ϴ� Mark Number (0x01 ~ 0x04)
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
 desc : �ı���
 parm : None
 retn : None
*/
CWorkMarkMove::~CWorkMarkMove()
{
}


/*
 desc : �ʱ� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkMarkMove::InitWork()
{
	/* ���� ��� ���� �� �ʱ�ȭ */
	if (!CWork::InitWork())	return FALSE;

	LPG_RAAF alignRecipe = uvEng_Mark_GetSelectAlignRecipe();
	SetAlignMode((ENG_AMOS)alignRecipe->align_motion, (ENG_ATGL)alignRecipe->align_type);


	uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
	
	return TRUE;
}

/*
 desc : �ֱ������� �۾� ����
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
///							����.
/// </summary>
void CWorkMarkMove::DoMovingOnthefly2cam()
{
	switch (m_u8StepIt)
	{
	case 0x01 : 
	{
		uvEng_Camera_ResetGrabbedImage();
		m_enWorkState = IsSetTrigPosResetAll();
		uvEng_Camera_SetCamMode(ENG_VCCM::en_grab_mode);/* Grab Mode ���� */
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
		uvEng_Camera_SetCamMode(ENG_VCCM::en_none);/* Grab Mode ���� */ 
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
	/* �۾� �ܰ� ���� ���� ó�� */
	
	auto& motions = GlobalVariables::GetInstance()->GetAlignMotion();
	const int CENTERCAM = motions.markParams.centerCamIdx;
	
	switch (m_u8StepIt)
	{
	case 0x01:
	{
		uvEng_Camera_ResetGrabbedImage();
		m_enWorkState = IsSetTrigPosResetAll();
		//uvEng_Camera_SetCamMode(ENG_VCCM::en_grab_mode);/* Grab Mode ���� */
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
			CommonMotionStuffs::GetInstance().GetOffsetsUseMarkPos(CENTERCAM, markPos, &alignOffsetPos, &expoOffsetPos);
			
			accumOffsets = includeAlignOffset == 1 ?  alignOffsetPos  : 
				includeAlignOffset == 2 ? alignOffsetPos + (expoOffsetPos - alignOffsetPos) : CaliPoint(0, 0, 0, 0);

			accumOffsets.offsetX *= -1.0f;
			accumOffsets.offsetY *= -1.0f;
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


		uvEng_Camera_SetCamMode(ENG_VCCM::en_none);/* Grab Mode ���� */
		m_enWorkState = ENG_JWNS::en_next;
		//RESERVE
	break;
	}


}


/// <summary>
///							���ܰ���
/// </summary>
void CWorkMarkMove::SetWorkNextOnthefly2cam()
{
	/* ���� �۾� ���� ���� �Ǵ� */
	CWork::SetWorkNext();
	/* ��ð� ���� ���� ��ġ�� �ݺ� �����Ѵٸ� ���� ó�� */
	CheckWorkTimeout();
}

void CWorkMarkMove::SetWorkNextOnthefly3cam()
{
	/* ���� �۾� ���� ���� �Ǵ� */
	CWork::SetWorkNext();
	/* ��ð� ���� ���� ��ġ�� �ݺ� �����Ѵٸ� ���� ó�� */
	CheckWorkTimeout();
}

void CWorkMarkMove::SetWorkNextStatic3cam()
{
	/* ���� �۾� ���� ���� �Ǵ� */
	CWork::SetWorkNext();
	/* ��ð� ���� ���� ��ġ�� �ݺ� �����Ѵٸ� ���� ó�� */
	CheckWorkTimeout();
}

void CWorkMarkMove::SetWorkNextStatic2cam()
{
	/* ���� �۾� ���� ���� �Ǵ� */
	CWork::SetWorkNext();
	/* ��ð� ���� ���� ��ġ�� �ݺ� �����Ѵٸ� ���� ó�� */
	CheckWorkTimeout();
}



/// <summary>
///							�ʱ�ȭ
/// </summary>
void CWorkMarkMove::DoInitStatic2cam()
{

}

void CWorkMarkMove::DoInitStatic3cam()
{
	/* ��ü �۾� �ܰ� */
	m_u8StepTotal = 0x07;
}

void CWorkMarkMove::DoInitOnthefly3cam()
{
	
}

void CWorkMarkMove::DoInitOnthefly2cam()
{
	
	m_u8StepTotal = 0x09;
	

}

/*
 desc : Align Mark�� �����ϴ� ���� ��ġ�� �̵� ����
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
	
	/* ���� �۾� Step Name ���� */
	SetStepName(L"Moving.Align.Mark");

	/* Global Mark ������ ���� */
	u8Mark	= uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	if (u8Mark != 4)
	{
		swprintf_s(tzMesg, 128, L"The number of mark is incorrect (global = %u)", u8Mark);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
		return ENG_JWNS::en_error;
	}

	/* �̵� ����� ī�޶� ���� */
	u8ACamID	= 0x01;
	if (m_u8MarkNo > 1)
	{
		enDrvACam	= ENG_MMDI::en_align_cam2;
		u8ACamID	= 0x02;
	}

	/* �� Axis ���� �⺻ ���� �ӵ� �� ��� */
	dbVeloACamX	= pstCfg->mc2_svc.max_velo[UINT8(enDrvACam)];
	dbVeloStageY= pstCfg->mc2_svc.max_velo[UINT8(ENG_MMDI::en_stage_y)];
	
	/* ���� �̵��ϰ��� �ϴ� ��� ��ġ �� ��� */
//	GetMovePosGlobalMark(m_u8MarkNo, i32PosACamX, i32PosStageY);
	if (!GetGlobalMarkMoveXY(m_u8MarkNo, dbPosACamX, dbPosStageY))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to get a position to move motion for align mark measurement");
		return ENG_JWNS::en_error;
	}
#if 0	/* �ܼ� ��ũ �̵��� ���� 2D Calibration Data �������� ���� */
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
	/* ��� Motor Drive�� ��� �� ���� */
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

	/* ī�޶� 1���� �ش�Ǵ� Mark (1, 2)�� ã�� ����� */
	if (m_u8MarkNo < 3)
	{
		/* 2 �� ī�޶� ��ġ �� ���������� �̵� ��Ŵ */
		dbPosACamX2	= pstCfg->mc2_svc.max_dist[UINT8(ENG_MMDI::en_align_cam2)];	/* 0.1 um or 100 ns */
		dbPosACamX1	= dbPosACamX;
	}
	/* ī�޶� 2���� �ش�Ǵ� Mark (3, 4)�� ã�� ����� */
	else
	{
		/* 1 �� ī�޶� ��ġ �� ó������ �̵� ��Ŵ */
		dbPosACamX1	= 0;	/* 0.1 um or 100 ns */
		dbPosACamX2	= dbPosACamX;
	}
#if 1
	TRACE(L"SetMovingAlignMark: ACam1 = %.4f mm\t ACam2 = %.4f mm\n", dbPosACamX1, dbPosACamX2);
#endif
	/* ���� ī�޶� 1 ���� ���� ��ġ ���� ���, �̵��ϰ��� �ϴ� ���� ��ġ ���� ū ������ Ȥ�� */
	/* ���� ������ ���ο� ����, Align Camera 1 Ȥ�� 2 �� �� ��� ���� ���� �̵��ؾ� ���� ����*/
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
 desc : Align Mark�� �����ϴ� ���� ��ġ�� �̵��ߴ��� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkMarkMove::IsMovedAlignMark()
{
	/* ���� �۾� Step Name ���� */
	if (IsWorkRepeat())	SetStepName(L"Is.Moved.Align.Mark");

	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))		return IsWorkWaitTime();
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1))	return IsWorkWaitTime();
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam2))	return IsWorkWaitTime();

	return ENG_JWNS::en_next;
}