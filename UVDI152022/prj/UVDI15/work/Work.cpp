
/*
 desc : �ó����� �⺻ (Base) �Լ� ����
*/

#include "pch.h"
#include "../MainApp.h"
#include "Work.h"
#include "../mesg/DlgMesg.h"
#include "../../UVDI15/GlobalVariables.h"

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
 retn : TRUE or FALSE
*/
BOOL CWork::InitWork()
{
	TCHAR tzMsg[128]= {NULL};

	/* �۾� �̸� ���� */
	SetWorkName();	/* !!! ���� ȣ���� ��� �� !!! */

	m_u8SetPhNo		= 0x00;		/* ������ 0x00 �� ���� */
	m_u8StepTotal	= 0x00;
	m_u8StepIt		= 0x01;		/* ������ 0x01 �� ���� */
	m_u8RetryStep	= 0xff;		/* ������ 0xff �� ���� */
	m_u8StepLast	= 0x0ff;	/* ������ 0xff �� ���� */
	m_u8RetryStep	= 0x00;		/* ������ 0x00 �� ���� */
	m_u8RetryCount	= 0x00;
#if 0
	m_bWorkAbortSet	= FALSE;	/* �۾� ��� ��û�� ���ٰ� ���� */
#endif
	m_enWorkState	= ENG_JWNS::en_init;	/* !!! �ʱ� ���� ��� �� !!! */
	m_u64DelayTime	= GetTickCount64();
	m_u64StartTime	= m_u64DelayTime;		/* �۾� ���� �ð� ���� */

	/* Work Step Name �ʱ�ȭ */
	wmemset(m_tzStepName, 0x00, STEP_NAME_LEN);

	/* Ʈ���� ���� ������ Disable */
// 	uvEng_Trig_ReqTriggerStrobe(FALSE);
// 	uvEng_Trig_ReqEncoderOutReset();
// 	uvEng_Trig_ResetTrigPosAll();
	uvEng_Mvenc_ReqTriggerStrobe(FALSE);
	uvEng_Mvenc_ReqEncoderOutReset();
	uvEng_Mvenc_ResetTrigPosAll();

	/* Vision : Grabbed Mode */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
	//switch (m_enWorkJobID)
	//{
	//case ENG_BWOK::en_expo_align:
	//case ENG_BWOK::en_mark_test	:
	//	uvEng_Camera_ResetGrabbedImage();
	//	uvEng_Camera_SetCamMode(ENG_VCCM::en_grab_mode);
	//	break;
	//}

	/* �۾� ���� (����) �̷� ���� */
	SaveWorkLogs(L"The job has started");

	return TRUE;
}

/*
 desc : Job Work Name ����
 parm : None
 retn : None
*/
VOID CWork::SetWorkName()
{
	wmemset(m_tzWorkName, 0x00, WORK_NAME_LEN);
	switch (m_enWorkJobID)
	{
	case ENG_BWOK::en_work_init		: wcscpy_s(m_tzWorkName, WORK_NAME_LEN, L"Equip. Initing");		break;	/* Work Init */
	case ENG_BWOK::en_work_home		: wcscpy_s(m_tzWorkName, WORK_NAME_LEN, L"Equip. Homing");		break;	/* Work Homing */
	case ENG_BWOK::en_work_stop		: wcscpy_s(m_tzWorkName, WORK_NAME_LEN, L"Work Stop");			break;	/* Work Stop */
	case ENG_BWOK::en_gerb_load		: wcscpy_s(m_tzWorkName, WORK_NAME_LEN, L"Recipe Load");		break;	/* Load the gerber (recipe) */
	case ENG_BWOK::en_gerb_unload	: wcscpy_s(m_tzWorkName, WORK_NAME_LEN, L"Recipe Unload");		break;	/* Unload the gerber (recipe) */
	case ENG_BWOK::en_mark_move		: wcscpy_s(m_tzWorkName, WORK_NAME_LEN, L"Mark Moving");		break;	/* Align Mark ��ġ�� �̵� */
	case ENG_BWOK::en_mark_test		: wcscpy_s(m_tzWorkName, WORK_NAME_LEN, L"Mark Testing");		break;	/* Align Mark �˻� */
	case ENG_BWOK::en_expo_only		: wcscpy_s(m_tzWorkName, WORK_NAME_LEN, L"Direct Exposure");	break;	/* Only �뱤 ���� */
	case ENG_BWOK::en_expo_align	: wcscpy_s(m_tzWorkName, WORK_NAME_LEN, L"Align Exposure");		break;	/* Align Mark ���� �� �뱤 ���� */
	default							: wcscpy_s(m_tzWorkName, WORK_NAME_LEN, L"Unknown Work Name");	break;
	}
}

/*
 desc : ���� �۾� ����
 parm : None
 retn : None
*/
VOID CWork::EndWork()
{
	SaveWorkLogs(L"The job has ended");
	/* �۾� �Ϸ� �ð� ���� */
	uvEng_SetJobWorkInfo(UINT8(m_enWorkJobID), GetTickCount64() - m_u64StartTime);

	/* Ʈ���� ���� ������ Disable */
// 	uvEng_Trig_ReqTriggerStrobe(FALSE);
// 	uvEng_Trig_ReqEncoderOutReset();
// 	uvEng_Trig_ResetTrigPosAll();
	uvEng_Mvenc_ReqTriggerStrobe(FALSE);
	uvEng_Mvenc_ReqEncoderOutReset();
	uvEng_Mvenc_ResetTrigPosAll();
}

/*
 desc : �۾� ���� �α� �̷�
 parm : mesg	- [in]  �۾� ���� ���� �޽��� ����
 retn : None
*/
VOID CWork::SaveWorkLogs(PTCHAR mesg)
{
	TCHAR tzMsg[256]	= {NULL};
	swprintf_s(tzMsg, 256, L"Job ID (0x%02u : %s) : %s", UINT8(m_enWorkJobID), m_tzWorkName, mesg);
	LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
}

/*
 desc : ���� �ܰ�� �̵��ϱ� ���� ó��
 parm : None
 retn : None
*/
VOID CWork::SetWorkNext()
{
	UINT64 u64JobTime	= GetTickCount64() - m_u64StartTime;

	/* �� �۾� �������� �ð� �� ���� ó�� */
	uvEng_UpdateJobWorkTime(u64JobTime);

	/* ��� �۾��� ���� �Ǿ����� ���� */
	if (ENG_JWNS::en_error == m_enWorkState)
	{


		TCHAR tzMesg[128] = { NULL };
		swprintf_s(tzMesg, 128, L"Work <Error Step It = 0x%02x>", m_u8StepIt);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);

		m_u8StepIt = 0x00;
	}
	else if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* �۾��� ��� �� �ӽ� ���� */
		CalcStepRate();
		/* �۾��� �Ϸ� �Ǿ����� ���� */
		if (m_u8StepTotal == m_u8StepIt)
		{
			m_enWorkState	= ENG_JWNS::en_comp;
			/* �׻� ȣ��*/
			CWork::EndWork();
		}
		else
		{
			/* ���� �۾� �ܰ�� �̵� */
			m_u8StepIt++;
		}
		/* ���� �ֱٿ� Next ������ �ð� ���� */
		m_u64DelayTime	= GetTickCount64();
	}
}
#if 0
/*
 desc : �ܺο��� ������ �۾� ��� ��û�� �� ���
 parm : None
 retn : None
*/
VOID CWork::SetWorkStop()
{
	/* ���� ���� */
	if (m_csSyncAbortSet.Enter())
	{
		m_bWorkAbortSet	= TRUE;
		/* ���� ���� */
		m_csSyncAbortSet.Leave();
	}
}
#endif
/*
 desc : �۾� ����� ���� (percent; %)
 parm : None
 retn : None
*/
VOID CWork::CalcStepRate()
{
	DOUBLE dbRate	= DOUBLE(m_u8StepIt) / DOUBLE(m_u8StepTotal) * 100.0f;
	if (dbRate > 100.0f)	dbRate = 100.0f;
	/* ���� �޸𸮿� ���� */
	uvEng_SetWorkStepRate (dbRate);
}

/*
 desc : Target System���� ����� �۽��ϱ� ����, �۽��ص� �Ǵ� Timing���� Ȯ��
 parm : wait	- [in]  ��� �ð� (����: msec)
 retn : TRUE or FALSE
*/
BOOL CWork::IsSendCmdTime(UINT32 wait)
{
#ifdef _DEBUG
	BOOL bSucc = GetTickCount64() > (m_u64ReqSendTime + wait + (UINT32)ROUNDED(wait * 0.5, 0));
#else
	BOOL bSucc = GetTickCount64() > (m_u64ReqSendTime + wait);
#endif
	if (bSucc)	SetSendCmdTime();

	return bSucc;
}

/*
 desc : ��ð� ���� Step���� ��� ������ �Ǵ�
 parm : None
 retn : None
*/
VOID CWork::CheckWorkTimeout()
{
	BOOL bSucc	= FALSE;

	/* 60�� �̻� �����Ǹ� ... (���� Job Loading ������ ...) */
	//bSucc	= GetTickCount64() > (m_u64DelayTime + 60000);	/* 60 �� �̻� �����Ǹ� */
	bSucc = GetTickCount64() > (m_u64DelayTime + 120000);	/* 60 �� �̻� �����Ǹ� */
	if (!bSucc)	return;	/* ���� �ܰ迡�� �۾� �ð��� �ʰ� �ȵ� */
#if 0
	/* ���� (���) �ð��� �ʰ��Ǹ� ���� ó�� */
	m_enWorkState	= ENG_JWNS::en_error;
#endif
	/* ���� ȣ��Ǵ� ���̹Ƿ�, �Լ� ���ϸ� ���̱� ���� ? (���⼭ ���� ����) */
	TCHAR tzJob[64]	= {NULL}, tzMesg[128] = {NULL};
	switch (m_enWorkJobID)
	{
	case ENG_BWOK::en_work_init		:	wcscpy_s(tzJob, 64, L"System.Init");			break;
	case ENG_BWOK::en_work_stop		:	wcscpy_s(tzJob, 64, L"System.Abort");			break;
	case ENG_BWOK::en_gerb_load		:	wcscpy_s(tzJob, 64, L"Gerber.Load");			break;
	case ENG_BWOK::en_gerb_unload	:	wcscpy_s(tzJob, 64, L"Gerber.Unload");			break;
	case ENG_BWOK::en_mark_move		:	wcscpy_s(tzJob, 64, L"Mark.Moving");			break;
	case ENG_BWOK::en_mark_test		:	wcscpy_s(tzJob, 64, L"Mark.Inspection");		break;
	case ENG_BWOK::en_expo_only		:	wcscpy_s(tzJob, 64, L"Expose.Direct");			break;
	case ENG_BWOK::en_expo_align	:	wcscpy_s(tzJob, 64, L"Expose.Only.Mark");		break;
	case ENG_BWOK::en_expo_cali		:	wcscpy_s(tzJob, 64, L"Expose.Cali.Mark");		break;
	case ENG_BWOK::en_work_none		:	wcscpy_s(tzJob, 64, L"Unknown.WorkJob");		break;
	}
	swprintf_s(tzMesg, 128, L"Working Step Timeout : %s (%d:0x%02x)", tzJob, m_u8StepIt, m_u8StepIt);
	LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);

	/* ���� ���� ó���� ���� �÷��� ���� */
	m_u8StepIt		= 0x00;
	m_enWorkState	= ENG_JWNS::en_time;
}

/*
 desc : ���� �۾� �̸� ����
 parm : name	- [in]  Step Name
 retn : None
*/
VOID CWork::SetStepName(PTCHAR name)
{
	/* ����� ���� �ֱ� �۾� ���� ���� */
	swprintf_s(m_tzStepName, STEP_NAME_LEN, L"[%s] [%02u (%02x) / %02u]",
			   name, m_u8StepIt, m_u8StepIt, m_u8StepTotal);
	/* ���� �ֱ� �� �ӽ� ���� */
	m_u8StepLast	= m_u8StepIt;


	//STG_PP_P2C_PROCESS_STEP		stProcessStep;
	//stProcessStep.Reset();
	//sprintf_s(stProcessStep.szGlassName, DEF_MAX_GLASS_NAME_LENGTH, "%d", m_u8StepIt);
	//sprintf_s(stProcessStep.szProcessStepName, DEF_MAX_SUBPROCESS_NAME_LENGTH, "%s", name);
	//uvEng_Philhmi_Send_P2C_PROCESS_STEP(stProcessStep);
}

/*
 desc : �Ź��� ���������� ���/����/���õǾ� �ִ��� ���� 
 parm : flag	- [in]  0x00: registered, 0x01: Selected, 0x02: Loaded (���õ� Job�� ����Ǿ� �ִ��� ����)
 retn : TRUE or FALSE
*/
BOOL CWork::IsGerberCheck(UINT8 flag)
{
	BOOL bCheck	= FALSE;
	
	switch (flag)
	{
	case 0x00	:	bCheck	= uvCmn_Luria_IsRegisteredJobs();	break;
	case 0x01	:	bCheck	= uvCmn_Luria_IsSelectedJobName();	break;
	case 0x02	:	bCheck	= uvCmn_Luria_IsLoadedJobName();	break;
	}

	return bCheck;
}

/*
 desc : ��ũ ���� ��ȿ�� Ȯ�� (���ų�, Global 4�� �̰ų�, Local 16�� �̰ų�....)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsValidMarkCount()
{
	if (uvEng_Luria_GetMarkCount(ENG_AMTF::en_global) != 4)	return FALSE;
	if (uvEng_Luria_GetMarkCount(ENG_AMTF::en_local) != 0)
	{
		if (uvEng_Luria_GetMarkCount(ENG_AMTF::en_local) % 2 != 0)	return FALSE;
	}

	return TRUE;
}

/*
 desc : ��ϵ� �Ź��� Mark�� �����ϴ��� �׸��� ������ ���, ��ũ ���� �Ÿ����� ��ȿ������ Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsGerberMarkValidCheck()
{
	BOOL bSucc			= TRUE;
	STG_XMXY stPosX[2]	= {NULL};
	STG_XMXY stPosY[2]	= {NULL};
	LPG_CASI pstSpec	= &uvEng_GetConfig()->acam_spec;

	/* ------------------------------------------------- */
	/* �߿�: ī�޶�鰣 ���� �浹���� ���� �ּ����� �Ÿ� */
	/* �߿�: Y �������� ��ũ�� �ν��ϱ� ���� �ּ��� �Ÿ� */
	/* ------------------------------------------------- */

	/* ��ũ ��ȿ�� ���� */
	if (!IsValidMarkCount())	return FALSE;

	auto status = GlobalVariables::getInstance()->GetAlignMotion().status;

	int localCnt = uvEng_Luria_GetMarkCount(ENG_AMTF::en_local);
	const int CAM1 = 1, CAM2 = 2, FIRST_SCAN = 0, FIRST_MARK = 0, SECOND_MARK = 1;
	
		
	if (localCnt != 0)
	{
		

		auto cam1_1st =  status.markMapConst[FIRST_SCAN][FIRST_MARK].tgt_id;
		auto cam1_2nd = status.markMapConst[FIRST_SCAN][SECOND_MARK].tgt_id;

		auto cam2_1st = status.markPoolForCamLocal[CAM2].begin()->tgt_id;

		/* 4th & 11th Mark ���� �� ���� X �� ���� ��� */
		if (!uvEng_Luria_GetLocalMark(cam1_1st, &stPosX[0]))	return FALSE;
		if (!uvEng_Luria_GetLocalMark(cam2_1st,&stPosX[1]))	return FALSE;
		/* 1th & 2th Mark ���� �� ���� Y �� ���� ��� */
		if (!uvEng_Luria_GetLocalMark(cam1_1st, &stPosY[0]))	return FALSE;
		if (!uvEng_Luria_GetLocalMark(cam1_2nd, &stPosY[1]))	return FALSE;
		/* -------------------------------------------------------------------------------------------- */
		/* Align Camera ���� ������ ������ ���� ���� �Ź��� ��ϵ� Mark ���� ���� ������ ū ��쿡 ���� */
		/* �Ź��� ��ϵ� 2���� Mark ���� (��) ���� ���������� ������ 2���� Align Camera���� �۴ٸ� ���� */
		/* -------------------------------------------------------------------------------------------- */
		if (m_enWorkJobID == ENG_BWOK::en_mark_test ||
			m_enWorkJobID == ENG_BWOK::en_expo_align||
			m_enWorkJobID == ENG_BWOK::en_expo_cali)
		{
			/*bSucc = (abs(stPosX[0].mark_x - stPosX[1].mark_x)) > pstSpec->GetACamMinDistH();
			if (bSucc)	bSucc = (abs(stPosY[0].mark_y - stPosY[1].mark_y)) > pstSpec->GetACamMinDistD();*/

		}
	}
	else if (uvEng_Luria_GetMarkCount(ENG_AMTF::en_global) == 4)
	{
		/* 2st & 4th Mark ���� �� ���� X �� ���� ��� */
		if (!uvEng_Luria_GetGlobalMark(1, &stPosX[0]))	return FALSE;
		if (!uvEng_Luria_GetGlobalMark(3, &stPosX[1]))	return FALSE;
		/* 2st & 4th Mark ���� �� ���� Y �� ���� ��� */
		if (!uvEng_Luria_GetGlobalMark(0, &stPosY[0]))	return FALSE;
		if (!uvEng_Luria_GetGlobalMark(1, &stPosY[1]))	return FALSE;
		/* -------------------------------------------------------------------------------------------- */
		/* Align Camera ���� ������ ������ ���� ���� �Ź��� ��ϵ� Mark ���� ���� ������ ū ��쿡 ���� */
		/* �Ź��� ��ϵ� 2���� Mark ���� (��) ���� ���������� ������ 2���� Align Camera���� �۴ٸ� ���� */
		/* -------------------------------------------------------------------------------------------- */
		if (m_enWorkJobID == ENG_BWOK::en_mark_test ||
			m_enWorkJobID == ENG_BWOK::en_expo_align||
			m_enWorkJobID == ENG_BWOK::en_expo_cali)
		{
			bSucc = (abs(stPosX[0].mark_x - stPosX[1].mark_x)) > pstSpec->GetACamMinDistH();
			if (!bSucc)
			{
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"Width: The period between marks is smaller than the physical camera period");
			}
			if (bSucc)
			{
				bSucc = (abs(stPosY[0].mark_y - stPosY[1].mark_y)) > pstSpec->GetACamMinDistD();
				if (!bSucc)	LOG_ERROR(ENG_EDIC::en_uvdi15, L"Height: The period between marks is smaller than the physical camera period");
			}
#if 0
			TRACE(L"pstSpec->GetACamMinDistH() = %.3f, pstSpec->GetACamMinDistD()=%.3f\n",
				  pstSpec->GetACamMinDistH(), pstSpec->GetACamMinDistD());
#endif
		}
	}

	return bSucc;
}

/*
 desc : Local Fiducial�� Mark �� �� �� (����) �κ��� Left / Right Mark Number �� ��ȯ
 parm : scan	- [in]  Scan Number (0 or 1)
		left	- [out] Left Bottom Mark Number �� ��ȯ (Zero based)
		right	- [out] Right Bottom Mark Number �� ��ȯ (Zero based)
 retn : TRUE or FALSE
*/
BOOL CWork::GetLocalLeftRightTopMarkIndex(UINT8 scan, UINT8 &left, UINT8 &right)
{
	/* ���� ���õ� ������ ���� ��� */
	LPG_RAAF pstRecipe	= uvEng_Mark_GetSelectAlignRecipe();
	if (!pstRecipe)	return FALSE;


	
	//��ĵ�� �� ķ �߾ӿ��� �̷������. 

	auto status = GlobalVariables::getInstance()->GetAlignMotion().status;

	
	int centerCol = status.GetCenterColumn();

	left = status.markMapConst[scan].front().tgt_id; //1ķ
	right = status.markMapConst[centerCol + scan].front().tgt_id; //2ķ

#if 0
	switch (pstRecipe->align_type)
	{
		/* Normal Mark Type */
	case ENG_ATGL::en_global_4_local_2x2_n_point:	/* Global (4) points / Local Division (2 x 2) (16) points */
		//abh1000 0417
		// 		if (0x00 == scan)	{	left = 0;	right = 4;	}
		// 		else				{	left = 8;	right = 12;	}
		if (0x00 == scan) { left = 0;	right = 8; }
		else { left = 4;	right = 12; }
		break;
	case ENG_ATGL::en_global_4_local_3x2_n_point:	/* Global (4) points / Local Division (3 x 2) (24) points */
		// 		if (0x00 == scan)	{	left = 0;	right = 6;	}
		// 		else				{	left = 17;	right = 18;	}
		if (0x00 == scan) { left = 0;	right = 12; }
		else { left = 6;	right = 18; }
		break;
	case ENG_ATGL::en_global_4_local_4x2_n_point:	/* Global (4) points / Local Division (4 x 2) (32) points */
		// 		if (0x00 == scan)	{	left = 0;	right = 8;	}
		// 		else				{	left = 23;	right = 24;	}
		if (0x00 == scan) { left = 0;	right = 16; }
		else { left = 8;	right = 24; }
		break;
	case ENG_ATGL::en_global_4_local_5x2_n_point:	/* Global (4) points / Local Division (5 x 2) (40) points */
		// 		if (0x00 == scan)	{	left = 0;	right = 10;	}
		// 		else				{	left = 29;	right = 30;	}
		if (0x00 == scan) { left = 0;	right = 20; }
		else { left = 10;	right = 30; }
		break;
		/* Shared Mark Type */
	case ENG_ATGL::en_global_4_local_2x2_s_point:	/* Global (4) points / Local Division (2 x 2) (13) points */
		if (0x00 == scan) { left = 0;	right = 3; }
		else { left = 3;	right = 6; }
		break;
	case ENG_ATGL::en_global_4_local_3x2_s_point:	/* Global (4) points / Local Division (3 x 2) (16) points */
		if (0x00 == scan) { left = 0;	right = 4; }
		else { left = 4;	right = 8; }
		break;
	case ENG_ATGL::en_global_4_local_4x2_s_point:	/* Global (4) points / Local Division (4 x 2) (19) points */
		if (0x00 == scan) { left = 0;	right = 5; }
		else { left = 5;	right = 10; }
		break;
	case ENG_ATGL::en_global_4_local_5x2_s_point:	/* Global (4) points / Local Division (5 x 2) (22) points */
		if (0x00 == scan) { left = 0;	right = 6; }
		else { left = 6;	right = 12; }
		break;
	default:	return FALSE;
	}
#endif
	return TRUE;
}

/*
 desc : Local Fiducial�� Mark �� �� �Ʒ� (����) �κ��� Left / Right Mark Number �� ��ȯ
 parm : scan	- [in]  Scan Number (0 or 1)
		left	- [out] Left Bottom Mark Number �� ��ȯ (Zero based)
		right	- [out] Right Bottom Mark Number �� ��ȯ (Zero based)
 retn : TRUE or FALSE
*/
BOOL CWork::GetLocalLeftRightBottomMarkIndex(UINT8 scan, UINT8 &left, UINT8 &right)
{
	/* ���� ���õ� ������ ���� ��� */
	LPG_RAAF pstRecipe = uvEng_Mark_GetSelectAlignRecipe();
	if (!pstRecipe)	return FALSE;


	//��ĵ�� Ƚ������. ������ �ǹ� 
	//��ĵ�� 2�� ������ �������� ���ƿ��� Ÿ�̹��ΰ� �ǹ� 

	/*
	�Ʒ� �ڵ带 ����. 

	scan�� 0�϶� 3 11�� ��ȯ���ݳ�? �� ���� �ö󰥶��̴�. 
	1�϶� 7 15
	
	*/

	//��ĵ�� �� ķ �߾ӿ��� �̷������. 

	auto alType = pstRecipe->align_type;

	

	auto status = GlobalVariables::getInstance()->GetAlignMotion().status;

	if (status.gerberColCnt <= 0 || status.gerberRowCnt <= 0) return FALSE;

	int centerCol = status.GetCenterColumn();

	left = status.markMapConst[scan].back().tgt_id; //1ķ
	right = status.markMapConst[centerCol + scan].back().tgt_id; //2ķ
	

	return true;

#if 0
	switch (pstRecipe->align_type)
	{
	case ENG_ATGL::en_global_4_local_2x2_n_point:	/* Global (4) points / Local Division (2 x 2) (16) points */
		//abh1000 0417
		// 		if (0x00 == scan)	{	left = 3;	right = 7;	}
		// 		else				{	left = 11;	right = 15;	}
		if (0x00 == scan) { left = 3;	right = 11; }
		else { left = 7;	right = 15; }
		break;
	case ENG_ATGL::en_global_4_local_3x2_n_point:	/* Global (4) points / Local Division (3 x 2) (24) points */
		// 		if (0x00 == scan)	{	left = 5;	right = 11;	}
		// 		else				{	left = 17;	right = 23;	}
		if (0x00 == scan) { left = 5;	right = 17; }
		else { left = 11;	right = 23; }
		break;
	case ENG_ATGL::en_global_4_local_4x2_n_point:	/* Global (4) points / Local Division (4 x 2) (32) points */
		// 		if (0x00 == scan)	{	left = 7;	right = 15;	}
		// 		else				{	left = 23;	right = 31;	}
		if (0x00 == scan) { left = 7;	right = 23; }
		else { left = 15;	right = 31; }
		break;
	case ENG_ATGL::en_global_4_local_5x2_n_point:	/* Global (4) points / Local Division (5 x 2) (40) points */
		// 		if (0x00 == scan)	{	left = 9;	right = 19;	}
		// 		else				{	left = 29;	right = 39;	}
		if (0x00 == scan) { left = 9;	right = 29; }
		else { left = 19;	right = 39; }
		break;

	case ENG_ATGL::en_global_4_local_2x2_s_point:	/* Global (4) points / Local Division (2 x 2) (13) points */
		if (0x00 == scan) { left = 2;	right = 5; }
		else { left = 5;	right = 8; }
		break;
	case ENG_ATGL::en_global_4_local_3x2_s_point:	/* Global (4) points / Local Division (3 x 2) (16) points */
		if (0x00 == scan) { left = 3;	right = 7; }
		else { left = 7;	right = 11; }
		break;
	case ENG_ATGL::en_global_4_local_4x2_s_point:	/* Global (4) points / Local Division (4 x 2) (19) points */
		if (0x00 == scan) { left = 4;	right = 9; }
		else { left = 9;	right = 14; }
		break;
	case ENG_ATGL::en_global_4_local_5x2_s_point:	/* Global (4) points / Local Division (5 x 2) (22) points */
		if (0x00 == scan) { left = 5;	right = 11; }
		else { left = 11;	right = 17; }
		break;

	default:	LOG_ERROR(ENG_EDIC::en_podis, L"Failed to find for lower left and right marks");	return FALSE;
	}
#endif

	return TRUE;
}

/*
 desc : MC2 Error Check
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsMC2ErrorCheck()
{
	BOOL bIsError	= FALSE;

	/* MC2 Error�� �߻��� �������� ���� Ȯ�� */
	if (uvCmn_MC2_IsDNCError())
	{
		m_u8StepIt	= 0x00;
		m_enWorkState	= ENG_JWNS::en_error;
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"DNC error occurred in MC2");
		bIsError = TRUE;
	}
	/* ���� ��� ����̺꿡�� ������ �߻� �ߴ��� ���� */
	if (uvCmn_MC2_IsAnyDriveError())
	{
		m_u8StepIt	= 0x00;
		m_enWorkState	= ENG_JWNS::en_error;
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Motion Drive error occurred in MC2");
		bIsError = TRUE;
	}

	/* ������ �߻��Ǿ�����, MC2 Drive�� ��� ���� ���� ���� */
	if (bIsError)	uvCmn_MC2_SaveAllDriveState();

	return bIsError;
}

/*
 desc : ���� �����ǰ� ���õǾ������� ��ȿ�� ���� ����Ǿ� �ִ��� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsSelectedRecipeValid()
{
	LPG_RJAF pstRecipe	= uvEng_JobRecipe_GetSelectRecipe();
	if (!pstRecipe || !pstRecipe->IsValid())
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"The selected recipe is not valid");
		m_u8StepIt	= 0x00;
		m_enWorkState	= ENG_JWNS::en_error;

		return FALSE;
	}

	return TRUE;
}

/*
 desc : ���� Luria Status �� �������� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsLuriaStatusError()
{
	if (uvCmn_Luria_GetLastErrorStatus())
	{
		TCHAR tzMesg[128]	= {NULL};
		swprintf_s(tzMesg, 128, L"An error has occurred in the service system (Code = %u)",
				   uvCmn_Luria_GetLastErrorStatus());
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
		m_u8StepIt	= 0x00;
		m_enWorkState	= ENG_JWNS::en_error;
		return TRUE;
	}
	return FALSE;
}

/*
 desc : ���� 2���� align camera�� ��ϵ� Mark�� �����ϴ��� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsAlignMarkRegisted() // lk91!! global mark, local mark üũ
{
	if (!uvEng_Camera_IsSetMarkModel(0x00, 0x01, GLOBAL_MARK) ||
		!uvEng_Camera_IsSetMarkModel(0x00, 0x02, GLOBAL_MARK) ||
		!uvEng_Camera_IsSetMarkModel(0x00, 0x01, LOCAL_MARK) ||
		!uvEng_Camera_IsSetMarkModel(0x00, 0x02, LOCAL_MARK))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"No align mark are registered on the camera");
		m_u8StepIt	= 0x00;
		m_enWorkState	= ENG_JWNS::en_error;
		return FALSE;
	}

	return TRUE;
}

/*
 desc : Align Moving �� ��, �������� Y �࿡ ���� Mark�� Left->Bottom ��ġ�� ã���� �̵��Ϸ��� ��ġ
 parm : mark	- [in]  global or local
		scan	- [in]  Scan Number (0 or 1)
		direct	- [in]  0x00 : Left, 0x01 : Right
 retn : Y ��ġ �� ��ȯ (����: 100 nm or 0.1 um) (���� ���� ����)
*/
INT32 CWork::GetLeftRightBottomMarkPosY(ENG_AMTF mark, UINT8 scan, UINT8 direct)
{
	UINT8 u8Left, u8Right;
	DOUBLE dbDiffMarkY		= 0, dbMark2StageY	= 0;
	STG_XMXY stMarkPos		= {NULL};
	LPG_CSAI pstSetAlign	= &uvEng_GetConfig()->set_align;
	//LPG_RAAF pstRecipe		= uvEng_Mark_GetSelectAlignRecipe();
	LPG_RJAF pstRecipe		= uvEng_JobRecipe_GetSelectRecipe();
	// by sysandj : ��������(����)
	LPG_MACP pstThickCali	= uvEng_ThickCali_GetRecipe(pstRecipe->cali_thick);

	/* �뱤 �ӵ��� ���� Ʈ���� �߻� ��ġ �� ���� */
	if (!pstThickCali)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"no_cali_trig_data [Stage_Y]");
		return -1;
	}
	else
	{
		dbMark2StageY	= pstThickCali->mark2_stage_y[0];	/* �ϴ� 1 �� ī�޶� ���� �� */
	}

	/* ȯ�� ���Ͽ� ������ ���� 2 �� ��ũ�� ��ġ�� ���� ����� 2 �� ��ũ ���� ���� �� ���� */
	if (ENG_AMTF::en_global == mark)
	{
		if (0x00 == direct)
		{
			//dbMark2StageY = pstThickCali->mark2_stage_y[0];	/* �ϴ� 1 �� ī�޶� ���� �� */
			if (!uvEng_Luria_GetGlobalMark(1, &stMarkPos))	return -1;
		}
		else
		{
			//dbMark2StageY = pstThickCali->mark2_stage_y[1];	/* �ϴ� 1 �� ī�޶� ���� �� */
			if (!uvEng_Luria_GetGlobalMark(3, &stMarkPos))	return -1;
		}
	}
	else
	{
		if (!GetLocalLeftRightBottomMarkIndex(scan, u8Left, u8Right))	return -1;
		if (0x00 == direct)
		{
			if (!uvEng_Luria_GetLocalMark(u8Left, &stMarkPos))	return -1;
		}
		else
		{
			if (!uvEng_Luria_GetLocalMark(u8Right, &stMarkPos))	return -1;
		}
	}
	/* ���� ��ũ�� 2�� Y ��ǥ�� ���� ����� �Ź��� 2�� ��ũ ���� Y �� ���� ���� �� */
	dbDiffMarkY = stMarkPos.mark_y - uvEng_GetConfig()->set_align.mark2_org_gerb_xy[1];

	/* 2 �� ��ũ�� ���� ��� Y ��ǥ ���� ������ ����� ���� ���� ���� �ָ� �� */
	
	
	/*������
	*  �Ʒ��� ���� ���ε� dbMark2StageY - dbDiffMarkY ����.
	�� ��� dbdiffmark�� 0�̰ų� (�����Ź��϶�) Ȥ�� +���϶��� �������̳� 
	�����϶���  - - = +�� �Ǳ⶧���� ������ + �������� �����δ� (�ɼ��� �ݴ�� ���� ����)
	 return (INT32)ROUNDED((dbMark2StageY - dbDiffMarkY) * 10000.0f, 0);	/* 100 nm or 0.1 um */ 
	
	return (INT32)ROUNDED((dbMark2StageY + dbDiffMarkY) * 10000.0f, 0);	/* 100 nm or 0.1 um */ 


}

/*
 desc : Gerber���� Mark ��ġ�� �����Ǵ� Motion Y ���� ���� ��ġ�� ã�� ����
 parm : mark_no	- [in]  Global Align Mark Number (0x00 ~ 0x03)
 retn : Y ��ġ �� ��ȯ (����: 100 nm or 0.1 um) (���� ���� ����)
*/
INT32 CWork::GetGlobalMarkMotionPosY(UINT8 mark_no)
{
	DOUBLE dbDiffMarkY		= 0, dbMark2StageY	= 0;
	STG_XMXY stMarkPos		= {NULL};
	LPG_CSAI pstSetAlign	= &uvEng_GetConfig()->set_align;
	//LPG_RAAF pstRecipe		= uvEng_Mark_GetSelectAlignRecipe();
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	// by sysandj : ��������(����)
	LPG_MACP pstThickCali	= uvEng_ThickCali_GetRecipe(pstRecipe->cali_thick);

	/* ����, Trigger Calibration�� ���� (����)�� �����Ͱ� ���ٸ�, ȯ�� ���Ͽ� �ִ� ���� �� �������� */
	if (!pstThickCali)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"no_cali_trig_data for global mark [Stage_Y]");
		return -1;
	}
	else
	{
		dbMark2StageY	= pstThickCali->mark2_stage_y[0];	/* �ϴ� 1 �� ī�޶� ���� �� */
	}
	/* Mark ��ġ ���� ��� */
	if (!uvEng_Luria_GetGlobalMark(mark_no, &stMarkPos))	return -1;
	/* ȯ�� ���Ͽ� ������ ���� 2 �� ��ũ�� ��ġ�� ���� ����� mark_no �� ��ũ ���� ���� �� ���� */
	dbDiffMarkY	= stMarkPos.mark_y - uvEng_GetConfig()->set_align.mark2_org_gerb_xy[1];

#if 0
	TRACE(L"mark_no = %d  mark_y = %8.4f  mark2_y = %9d diff_y = %9d\n",
		  mark_no, stMarkPos.y, pstTrigCali->mark2_stage_y, i32DiffMarkY);
#endif

	/* Calibration 2 �� ��ũ�� ���� ��� Y ��ǥ ���� ������ ����� ���� ���� ���� �ָ� �� */
	return (INT32)ROUNDED((dbMark2StageY + dbDiffMarkY) * 10000.0f, 0);	/* 100 nm or 0.1 um */
}

/*
 desc : Gerber���� �����ϴ� Mark ��ġ�� �ش�� Motion Y ���� ���� ��ġ�� ã�� ����
 parm : index	- [in]  Local Align Mark�� ����� �޸� �ε��� (0x00 ~ )
 retn : Y ��ġ �� ��ȯ (����: 100 nm or 0.1 um)
*/
INT32 CWork::GetLocalMarkMotionPosY(UINT8 index)
{
	DOUBLE dbDiffMarkY		= 0, dbMark2StageY	= 0;
	STG_XMXY stMarkPos		= {NULL};
	LPG_CSAI pstSetAlign	= &uvEng_GetConfig()->set_align;
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	//LPG_RAAF pstRecipe		= uvEng_Mark_GetSelectAlignRecipe();
	// by sysandj : ��������(����)
	LPG_MACP pstThickCali	= uvEng_ThickCali_GetRecipe(pstRecipe->cali_thick);

	if (!pstThickCali)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"no_cali_trig_data for local mark [Stage_Y]");
		return -1;
	}
	else
	{
		dbMark2StageY	= pstThickCali->mark2_stage_y[0];	/* �ϴ� 1 �� ī�޶� ���� �� */
	}
	/* Mark ��ġ ���� ��� */
	if (!uvEng_Luria_GetLocalMark(index, &stMarkPos))	return -1;
	/* ȯ�� ���Ͽ� ������ ���� 2 �� ��ũ�� ��ġ�� ���� ����� 2 �� ��ũ ���� ���� �� ���� */
	dbDiffMarkY	= stMarkPos.mark_y - uvEng_GetConfig()->set_align.mark2_org_gerb_xy[1];		/* 100 nm or 0.1 um */

	/* Calibration 2 �� ��ũ�� ���� ��� Y ��ǥ ���� ������ ����� ���� ���� ���� �ָ� �� */
	return (INT32)ROUNDED((dbMark2StageY + dbDiffMarkY) * 10000.0f, 0);	/* 100 nm or 0.1 um */
}

/*
 desc : Gerber ���� ���� ����� Mark (Global 4���� ������ Local)����
		��ġ�� �ش�Ǵ� ��� Ʈ���� ��ġ ��� �� �ӽ� ����
 parm : acam_diff_y	- [in]  ����� ī�޶� 1�� �������� 2�� ī�޶� ��ġ�� ���� ���� �� (����: 0.1 um or 100 nm)
		mark_diff_y	- [in]  �Ź� ���� ��ǥ���� Left�� Right�� ��ũ ���� Y ���� ���� �� (����: 0.1 um or 100 nm)
 retn : TRUE or FALSE
*/


void CWork::LocalTrigRegist()
{
	const int CAM1 = 0, CAM2 = 1 ,SidecamCnt = 2, WANG_BOCK = 2,Y = 1,MARK2_INDEX = 1;
	double camDistXY[2] = { 0, };
	STG_XMXY xmlMark2;
	LPG_XMXY pxmlMark2 = &xmlMark2;;
	
	LPG_CIEA pstConfig = uvEng_GetConfig();	
	INT32 expoY = (INT32)ROUNDED(uvCmn_Luria_GetStartY(ENG_MDMD::en_pos_expo_start) * 10000.0f, 0);
	INT32 unloadY = uvEng_GetConfig()->set_align.table_unloader_xy[0][1] * 10000.0f;;
	LPG_CTSP pstTrigSet = &pstConfig->trig_grab;
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	LPG_MACP pstThickCali = uvEng_ThickCali_GetRecipe(pstRecipe->cali_thick); //thick��.

	double trigPosCam[SidecamCnt]={0,};// Ʈ���� ����� ���� .
	
	GetACamCentDistXY(camDistXY[0], camDistXY[1]);
	camDistXY[0] *= 10000.0f; camDistXY[1] *= 10000.0f;

	double orgMark2[2] = { pstConfig->set_align.mark2_org_gerb_xy[0],
						   pstConfig->set_align.mark2_org_gerb_xy[1] };

	uvEng_Luria_GetGlobalMark(MARK2_INDEX, pxmlMark2);

	double yGab = orgMark2[Y] - pxmlMark2->mark_y;

	double stageYMark2 = (pstThickCali->mark2_stage_y[0] - yGab) * 10000.0f; //��ũ2�� �ٶ󺸴� �������� y ��ǥ�� �ǹ���. 

	int markOrder = 0;
	auto status = GlobalVariables::getInstance()->GetAlignMotion().status;
	auto scans = status.gerberColCnt / WANG_BOCK;
	UINT8 u8ScanMarks = uvEng_Recipe_GetScanLocalMarkCount();		/* 1 Scan ���� - ����� Fiducial ���� */
	auto centerCol = status.GetCenterColumn();
	bool backward = true;
	int trigAdded = 0; //���� ��ϵ� Ʈ���� ���� 
		
	map<int,vector<double>> trigMap;
	for (int scan = 0; scan < scans; scan++)
	{
		UINT32 startPos = backward ? expoY : unloadY;
			
		double placeGab = fabs((double)stageYMark2 - (double)startPos);
		for (int i = 0; i < u8ScanMarks; i++, markOrder++)
		{
			
			UINT32 camLocalMark[] = { status.markPoolForCamLocal[1][markOrder].tgt_id , status.markPoolForCamLocal[2][markOrder].tgt_id}; //Ʈ���� ����� ��ũ��ȣ

			double markGab[] = { uvEng_Luria_GetGlobalBaseMarkLocalDiffY(0x00, camLocalMark[0]) * 10000.0f, 
								 uvEng_Luria_GetGlobalBaseMarkLocalDiffY(0x01, camLocalMark[1]) * 10000.0f};

			trigPosCam[0] = (placeGab + (backward ? markGab[CAM1] : -markGab[CAM1])) * (backward ? 1 : -1);
			trigPosCam[1] = (placeGab + (backward ? markGab[CAM2] : -markGab[CAM2])) * (backward ? 1 : -1);
				
			trigMap[0].push_back(trigPosCam[0] + (backward ? pstTrigSet->trig_backward  : pstTrigSet->trig_forward));
			trigMap[1].push_back(trigPosCam[1] + (backward ? pstTrigSet->trig_backward  : pstTrigSet->trig_forward) + camDistXY[Y]);


			/* Align Camera 2D ���� ���� */
			if (uvEng_GetConfig()->set_align.use_2d_cali_data)
			{
				/* Align Mark�� ������ ��� Y ���� ���� ��ġ �� */
				uvEng_ACamCali_AddMarkPos(1, ENG_AMTF::en_local, 0x01, markOrder, GetLocalMarkMotionPosY(camLocalMark[0]) / 10000.0f);
				uvEng_ACamCali_AddMarkPos(2, ENG_AMTF::en_local, 0x01, markOrder, (GetLocalMarkMotionPosY(camLocalMark[1]) + camDistXY[Y]) / 10000.0f);

			}
		}

		std::sort(trigMap[CAM1].begin(), trigMap[CAM1].end()); std::sort(trigMap[CAM2].begin(), trigMap[CAM2].end());
			
		for (int i = 0; i < u8ScanMarks; i++)
		{
			uvEng_Trig_SetTrigger(ENG_AMTF::en_local, 1, trigAdded, trigMap[CAM1][i]);
			uvEng_Trig_SetTrigger(ENG_AMTF::en_local, 2, trigAdded, trigMap[CAM2][i]);
			trigAdded++;
		}
		trigMap[0].clear();trigMap[1].clear();

		backward = !backward;
	}
}

#define ALLNEWLOGIC

BOOL CWork::SetTrigPosCalcSaved(INT32 acam_diff_y/*, INT32 mark_diff_y*/)
{
	const int SidecamCnt = 2;

	// by sysandj : �Լ�����(����)
	UINT8 i, j, k, u8ScanMarks	= uvEng_Recipe_GetScanLocalMarkCount();		/* 1 Scan ���� - ����� Fiducial ���� */
	INT32 i32MarkMovePosY		= -1, i32MarkDiffPosY;
	INT32 i32StageUnloadY		= {NULL};
	INT32 i32AlignCam1[SidecamCnt]		= {NULL};	/* 100 nm or 0.1 um */
	INT32 i32AlignCam2[SidecamCnt]		= {NULL};	/* 100 nm or 0.1 um */
	INT32 i32Mark2TrigPos		= 0;		/* Global Fiducial Mark Left  / Bottom Position (Trigger) (100 nm or 0.1 um )*/
	INT32 i32Mark4TrigPos		= 0;		/* Global Fiducial Mark Right / Bottom Position (Trigger) (100 nm or 0.1 um )*/
	LPG_CIEA pstConfig			= uvEng_GetConfig();
	LPG_CTSP pstTrigSet			= &pstConfig->trig_grab;

	INT32 i32EndStartPosY		= 0;
	/* �ݵ�� �ʱ�ȭ ���� */
	uvEng_ACamCali_ResetAllCaliData();

	/* Stage X, Y, Camera 1 & 2 X �̵� ��ǥ ��� */
	i32StageUnloadY	= (INT32)ROUNDED(uvEng_GetConfig()->set_align.table_unloader_xy[0][1] * 10000.0f, 0);	/* 100 nm or 0.1 um */
	/* Stage Y�� ���� ���� ������ �Ÿ� ���ϱ� */
	//i32EndStartPosY = i32StageUnloadY - (uvEng_GetConfig()->luria_svc.table_expo_start_xy[0][1] * 10000.0f);
	/* --------------------------------------------- */
	/* Global Mark�� �νĵ� Trigger Position �� ��� */
	/* --------------------------------------------- */
	/* 2 �� (Left/Bottom) Mark�� �ν��ϱ� ���� Ʈ���� ��ġ ���  */
	i32MarkMovePosY	= GetLeftRightBottomMarkPosY(ENG_AMTF::en_global, 0x00 /*scan*/, 0x00/*direct*/);	/* 100 nm or 0.1 um */
	if (i32MarkMovePosY < 0)	return FALSE;
	//i32AlignCam1[1]	= i32StageUnloadY - i32MarkMovePosY;
	i32AlignCam1[0] = i32StageUnloadY - i32MarkMovePosY;
	/* 1 �� (Left/Top) Mark�� �ν��ϱ� ���� Ʈ���� ��ġ ���  */
	i32MarkDiffPosY = (INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffY(1 /* Left/Top */, 2 /* Left/Bottom */) * 10000.0f, 0);
	//i32AlignCam1[0]	= i32AlignCam1[1] - i32MarkDiffPosY;	/* 100 nm or 0.1 um */
	i32AlignCam1[1] =  abs(i32AlignCam1[0]) - i32MarkDiffPosY;	/* 100 nm or 0.1 um */


	/* 4 �� (Right/Bottom) Mark�� �ν��ϱ� ���� Ʈ���� ��ġ ��� (ī�޶� ���� ���� ���� ���⼭ ����) */
	i32MarkMovePosY	= GetLeftRightBottomMarkPosY(ENG_AMTF::en_global, 0x00 /*scan*/, 0x01/*direct*/);	/* 100 nm or 0.1 um */
	if (i32MarkMovePosY < 0)	return FALSE;
	//i32AlignCam2[1]	= i32StageUnloadY - i32MarkMovePosY;
	i32AlignCam2[0] = i32StageUnloadY - i32MarkMovePosY;
	/* 3 �� (Left/Top) Mark�� �ν��ϱ� ���� Ʈ���� ��ġ ���  */
	i32MarkDiffPosY = (INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffY(3 /* Right/Top */, 4 /* Right/Bottom */) * 10000.0f, 0);
	//i32AlignCam2[0]	= i32AlignCam2[1] - i32MarkDiffPosY;	/* 100 nm or 0.1 um */
	i32AlignCam2[1] = abs(i32AlignCam2[0]) - i32MarkDiffPosY;	/* 100 nm or 0.1 um */


	i32AlignCam1[0] *= -1;
	i32AlignCam1[1] *= -1;

	i32AlignCam2[0] *= -1;
	i32AlignCam2[1] *= -1;

	
	

	/* Global Fiducial Mark�� Left & Right�� Bottom�� �ش�Ǵ� Trigger Position �� �ӽ� ���� */
	i32Mark2TrigPos = i32AlignCam1[1];	/* Left  / Bottom�� Trigger Position �� (Align Camera 1) */
	i32Mark4TrigPos = i32AlignCam2[1];	/* Right / Bottom�� Trigger Position �� (Align Camera 1) */

	//INT32 AlignCam1_05, AlignCam2_05;
	/* 4 ���� Mark ��ġ�� �������� Ʈ���� ���� �� ���� (����: 0.1 um or 100 nm) */
	for (i=0; i< SidecamCnt; i++)
	{
		/* Align Camera Trigger 1 ��� Camera Trigger 2 �� Delay �� ���� */
		/* Align Camera 1 & 2���� �������� ��ġ ���� ���� �ݿ��ؾ� �Ѵ�. */
		i32AlignCam2[i]	+= acam_diff_y;
		/* �޸𸮿� �ӽ÷� Ʈ���Ű� �߻��� ��ġ ��� */
		uvEng_Trig_SetTrigger(ENG_AMTF::en_global, 1, i, i32AlignCam1[i]+pstTrigSet->trig_forward);
		uvEng_Trig_SetTrigger(ENG_AMTF::en_global, 2, i, i32AlignCam2[i]+pstTrigSet->trig_forward);

		/* Align Camera 2D ���� ���� */
		if (uvEng_GetConfig()->set_align.use_2d_cali_data)
		{
			uvEng_ACamCali_AddMarkPos(1, ENG_AMTF::en_global, 0x01, i,
										GetGlobalMarkMotionPosY(i)/10000.0f);
			uvEng_ACamCali_AddMarkPos(2, ENG_AMTF::en_global, 0x01, i,
										(GetGlobalMarkMotionPosY(i+2)+acam_diff_y)/10000.0f);
		}
	}
	/* �ᱹ �뱤 ���� ��ġ���� �̵��ϸ� ��. ��ũ �ν� �� �ٷ� �뱤�� �� �ֵ��� �ϱ� ���� */
	m_dbAlignStageY	= uvCmn_Luria_GetStartY(ENG_MDMD::en_pos_expo_start);
	/* Stage Y�� ���� ���� ������ �Ÿ� ���ϱ� */
	i32EndStartPosY = i32StageUnloadY - INT32(m_dbAlignStageY * 10000.0f);

	/* Local Mark�� �νĵ� Trigger Position �� ��� */

	if (!IsMarkTypeOnlyGlobal())
		LocalTrigRegist();

	/* Align Camera 2D ���� ���� */
	if (!uvEng_GetConfig()->set_align.use_2d_cali_data)	return TRUE;
	/* ���� Trigger�� �߻��Ǵ� ���� (��ġ)�� X / Y ��� ��ǥ�� ���� ���� (����)�� Calibration X ���� �� �ݿ� */
	return SetAlignACamCaliX();
}

/*
 desc : ���� ��ϵ� Trigger (Mark) ���� ��, �� Trigger ��ġ���� Align Camera (X ��)�� ��ġ �� �ӽ� ����
		4�� ����� ��ũ ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::SetAlignACamCaliX()
{
	UINT8 i, j, n, k, u8MarkSet	= 0x00;
	INT32 i32OffsetX[2]			= { 0 }, i32OffsetY[2]	= { 0 };
	INT32 i32ScanMark			= 0;	/* 1 Scan ���� - ����� Fiducial ���� */
	DOUBLE dbPosACam[2]			= {NULL};
	DOUBLE dbMark2OrgGerbX		= uvEng_GetConfig()->set_align.mark2_org_gerb_xy[0];
	
	LPG_XMXY stMarkLocal = {nullptr};
	
	//STG_XMXY stMarkLocal[2] = { NULL };
	//STG_XMXY	stMarkGlobal	= {NULL};

	INT32 i32ACamVertX			= 0;	/* ����: 0.1 um or 100 nm */
	LPG_CIEA pstConfig			= uvEng_GetConfig();
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	//LPG_RAAF pstRecipe			= uvEng_Mark_GetSelectAlignRecipe();
	// by sysandj : ��������(����)
	LPG_MACP pstCaliThick		= uvEng_ThickCali_GetRecipe(pstRecipe->cali_thick);
	CAtlList <DOUBLE> lstCam1, lstCam2;

	/* ---------------------------------------------------------------------------- */
	/* [Global] Luria�� Fiducial ���� �־� �ٶ�, ���� ��ȣ�� �����ϴµ� ����� �߿� */
	/* ---------------------------------------------------------------------------- */
	/* ����, Grabbed Image���� ������ ���� ���� �ش� Calibration ���� �߰��� ����� ���� Mark ���� ���� �� */
	for (i=0; i<uvEng_Luria_GetMarkCount(ENG_AMTF::en_global); i++)
	{
		/* ����� ī�޶��� ��ũ �ν� ����� ���� �ٸ��� ó���� ��� �� */
		uvEng_ACamCali_AddMarkPos(i/2+1, ENG_AMTF::en_global, 0x00, i%2, m_dbPosACam[i/2]);
	}
	/* Align ���� ��Ŀ� ����. ����� ī�޶� X ���� �� �ݿ��ؾ� ���� ���� */
	// by sysandj : ��������(����)
	 LPG_CSAI pstSetAlign = &uvEng_GetConfig()->set_align;
	//if (ENG_AOMI::en_fixed == ENG_AOMI(0/*pstRecipe->align_method*/))
	if (pstSetAlign->align_method != UINT8(ENG_AOMI::en_each))
	{
		/* Mark 1 (Left/Bottom)�� �������� �����Ƿ�, Mark1 ���� Mark 2���� X ��ǥ ���� ���� Mark 2�� X ���� �ݿ� */
		i32ACamVertX = (INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(2) * 10000.0, 0);
		/* ���� Grabbed Image�� Mark ��ġ ���� ���� ���� ���� ���� �ؾ� �� */
		uvEng_ACamCali_SetAlignACamVertGX(1, 1, -i32ACamVertX);
		/* Mark 3 (Right/Bottom)�� �������� �����Ƿ�, Mark3 ���� Mark 4���� X ��ǥ ���� ���� Mark 4�� X ���� �ݿ� */
		i32ACamVertX = (INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(3) * 10000.0, 0);
		/* ���� Grabbed Image�� Mark ��ġ ���� ���� ���� ���� ���� �ؾ� �� */
		uvEng_ACamCali_SetAlignACamVertGX(2, 1, -i32ACamVertX);
	}
	const int WANGBOK = 2;
	/* ���� Local Mark���� �����Ѵٸ� */
	if (!IsMarkTypeOnlyGlobal())
	{
		/* --------------------------------------------------------------------------- */
		/* [Local] Luria�� Fiducial ���� �־� �ٶ�, ���� ��ȣ�� �����ϴµ� ����� �߿� */
		/* --------------------------------------------------------------------------- */
		i32ScanMark = (INT32)GlobalVariables::getInstance()->GetAlignMotion().status.gerberRowCnt;  // uvEng_Luria_GetLocalMarkCountPerScan();
		auto columnCnt = (INT32)GlobalVariables::getInstance()->GetAlignMotion().status.gerberColCnt;
		int scanCnt =  columnCnt / WANGBOK;
		
		//stMarkLocal = new STG_XMXY[scanCnt];
		auto uniquePtrStMarkLocal = std::make_unique<STG_XMXY[]>(2);

		//if (!uvEng_Luria_GetGlobalMark(0x01, &stMarkGlobal))	return FALSE;	// Get the Left/Bottom Mark for Global
		/* Camera 1 & 2�� ����� ���� �迭 �ε��� �� ���ϱ� */
		for (i=0,k=0,n=0; i< scanCnt; i++)	/* Scan Ƚ�� : 2ȸ ���� ���������� �����ؾ���.*/
		{
			/* Scan 1 or 2�� ��, �� ī�޶� �� Left or Right / Bottom�� X �� �������� ������ X ��� ���� ���� �� ��� */
			if (uvEng_Luria_GetLocalMarkDiffVertX(i, lstCam1, lstCam2))
			{
				/* Gerber���� Local Fiducial �� Camera 1 & 2���� �ش�Ǵ� Left or Right / Bottom�� Mark ���� */
				if (!uvEng_Luria_GetLocalBottomMark(i, 1, &uniquePtrStMarkLocal[0]) ||
					!uvEng_Luria_GetLocalBottomMark(i, 2, &uniquePtrStMarkLocal[1]))
				{
					return FALSE;
				}
				/* Local Mark�� Left/Bottom ��ġ�� �ش�Ǵ� 1�� ī�޶��� ��� ����̺� ���� ��ġ ��� */
				/* Global Left/Bottom ��ġ�� Local Left/Bottom �� ������ �������� ���� ��� ��ġ ��� */
				dbPosACam[0] = pstCaliThick->mark2_acam_x[0] + (uniquePtrStMarkLocal[0].mark_x - dbMark2OrgGerbX);	/* mm */
				/* Align Camera 1�������� Align Camera 1���� 2�� ���� �⺻ �������� �Ÿ��� �����ϰ�,  �Ź��� ��ũ ���� ������ ��ġ�� ����ؾ� �� */
				dbPosACam[1] = pstCaliThick->mark2_acam_x[1] + (uniquePtrStMarkLocal[1].mark_x - dbMark2OrgGerbX);	/* mm */
				/* ����, Grabbed Image���� ������ ���� ���� �ش� Calibration ���� �߰��� ����� ���� Mark ���� ���� �� */
				
				
				
				
				//������//
				// //������//
				// //������//
				// //������//
				// //������//
				// //������//
				// //������//
				// //������//
				// //������//
				// //������//
				// //������//
				//������//
				
				
				
				
				
				for (j=0; j<i32ScanMark; j++,n++) 
				{
					/* Offset �� ���� - Camera 1 */
					uvEng_ACamCali_AddMarkPos(1, ENG_AMTF::en_local, 0x00, n, dbPosACam[0]);
					i32ACamVertX	-= (INT32)ROUNDED(lstCam1.GetAt(lstCam1.FindIndex(j)) * 10000.0f, 0);
					/* ���� Grabbed Image�� Mark ��ġ ���� ���� ���� ���� ���� �ؾ� �� */
					uvEng_ACamCali_SetAlignACamVertLX(1, j, i32ACamVertX);

					/* Offset �� ���� - Camera 2 */
					uvEng_ACamCali_AddMarkPos(2, ENG_AMTF::en_local, 0x00, n, dbPosACam[1]);
					i32ACamVertX	-= (INT32)ROUNDED(lstCam2.GetAt(lstCam2.FindIndex(j)) * 10000.0f, 0);
					/* ���� Grabbed Image�� Mark ��ġ ���� ���� ���� ���� ���� �ؾ� �� */
					uvEng_ACamCali_SetAlignACamVertLX(2, j, i32ACamVertX);
				}
				/* ���� ��ϵ� �ӽ� ������ �޸� ���� */
				lstCam1.RemoveAll();
				lstCam2.RemoveAll();
			}
		}
	}

	/* ���� Trigger Position�� ���� 2D Calibration Data�� ���ٸ� TRUE ��ȯ */
	if (!uvEng_ACamCali_IsCaliExistData())	return TRUE;
	/* Trigger �߻� ��ġ�� �ش�Ǵ� Stage Y�� Align Camera X ��ǥ�� ���� 2D Calibration Data ��� �� ���� */
	if (!uvEng_GetConfig()->IsRunDemo())
	{
		u8MarkSet = uvEng_ACamCali_SetTrigPosCaliApply();
		if (0x00 == u8MarkSet)		return FALSE;
		else if (0x01 == u8MarkSet)	return TRUE;
		else
		{
			CDlgMesg dlgMesg;
			if (IDOK != dlgMesg.MyDoModal(L"Some marks do not exist in the calibration area\n"
										  L"Shall we continue with the exposure work ?", 0x02))
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

/*
 desc : ī�޶� ��ġ ��������, ���� �߽� ���� 2���� ī�޶� ��ġ ���� ��ȯ
 parm : dist_w	- [out] 2���� ����� ī�޶� ��ġ ���� �� ��ȯ (����: 100 nm or 0.1 um)
		dist_h	- [out] 2���� ����� ī�޶� ��ġ ���� �� ��ȯ (����: 100 nm or 0.1 um)
 retn : None
*/
VOID CWork::GetACamCentDistXY(INT32 &dist_x, INT32 &dist_y)
{
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	//LPG_RAAF pstRecipe		= uvEng_Mark_GetSelectAlignRecipe();
	// by sysandj : ��������(����)
	LPG_MACP pstCaliThick	= uvEng_ThickCali_GetRecipe(pstRecipe->cali_thick);
	if (!pstCaliThick)
	{
		AfxMessageBox(L"Can't find the cali_thick for recipe", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	dist_x	= (INT32)ROUNDED(pstCaliThick->GetACamDistX() * 10000.0f, 0);
	dist_y	= (INT32)ROUNDED(pstCaliThick->GetACamDistY() * 10000.0f, 0);
}
VOID CWork::GetACamCentDistXY(DOUBLE &dist_x, DOUBLE &dist_y)
{
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	//LPG_RAAF pstRecipe = uvEng_Mark_GetSelectAlignRecipe();
	// by sysandj : ��������(����)
	LPG_MACP pstCaliThick = uvEng_ThickCali_GetRecipe(pstRecipe->cali_thick);
	if (!pstCaliThick)
	{
		AfxMessageBox(L"Can't find the cali_thick for recipe", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	dist_x	= pstCaliThick->GetACamDistX();
	dist_y	= pstCaliThick->GetACamDistY();
}

/*
 desc : ���� ���� �β� ��������, Align Camera 1���� Mark 2 (1 ~ 4)�� ��� ��ġ �� ��ȯ
 parm : mark_no	- [in]  Mark Number (0x00 ~ 0x03)
 retn : ��� �� (0.1 um or 100 nm), ���� ���� ��� ����
*/
INT32 CWork::GetACam1Mark2MotionX(UINT8 mark_no)
{
	INT32 i32Mark2ACamX		= 0;
	STG_XMXY stMarkPos		= {NULL};
	LPG_CSAI pstSetAlign	= &uvEng_GetConfig()->set_align;
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	//LPG_RAAF pstRecipe		= uvEng_Mark_GetSelectAlignRecipe();
	// by sysandj : ��������(����)
	LPG_MACP pstCaliThick = uvEng_ThickCali_GetRecipe(pstRecipe->cali_thick);

	/* Calibration Thick Data ���� ���� */
	if (!pstCaliThick)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"There is no correction data related to material thickness among the recipe properties.");
		return -1;
	}
	i32Mark2ACamX	= (INT32)ROUNDED(pstCaliThick->mark2_acam_x[0] * 10000.0f, 0);	/* Align Camera 1 �� ���� */

	/* ���� ����� �Ź��� Mark ? ��(Left/Bottom)�� ���� X, Y ��ǥ �� �������� */
	if (!uvEng_Luria_GetGlobalMark(mark_no, &stMarkPos))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to get the location information of global mark");
		return -1;
	}

	return	(i32Mark2ACamX - (INT32)ROUNDED(pstSetAlign->mark2_org_gerb_xy[0] * 10000.0f, 0)) + 
			(INT32)ROUNDED(stMarkPos.mark_x * 10000.0f, 0);
}

/*
 desc : Chiller ���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsChillerRunning()
{
	/* ĥ���� ���� ������ Ȯ�� */
	if (!uvEng_GetConfig()->IsCheckChiller())	return TRUE;
	// by sysandj : MCQ��ü �߰� �ʿ�
	return FALSE;//return uvEng_ShMem_GetPLCExt()->r_chiller_controller_running == 1 ? TRUE : FALSE;
}

/*
 desc : Vacuum ���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsVacuumRunning()
{
	/* ��ŧ�� ���� ������ Ȯ�� */
	if (!uvEng_GetConfig()->IsCheckVacuum())	return TRUE;
	// by sysandj : MCQ��ü �߰� �ʿ�
// 	return (uvEng_ShMem_GetPLCExt()->r_vacuum_controller_running == 1 && 
// 			uvEng_ShMem_GetPLCExt()->r_vacuum_status == 1) ? TRUE : FALSE;
	return FALSE;
}

/*
 desc : Align ���� ����� ����, ĸó�� Global Mark�� �̹��� ��ġ ���� ��ȯ
 parm : mark_no	- [in]  Mark Number (0x00 ~ 0x03)
		cam_id	- [out] Grabbed Image�� ����� ī�޶��� ID (0x01 or 0x02)
		img_id	- [out] Grabbed Image�� ID (0x00 ~ 0x??)
 retn : None
*/
VOID CWork::GetGlobalMarkIndex(UINT8 mark_no, UINT8 &cam_id, UINT8 &img_id)
{
	LPG_RAAF pstRecipe	= uvEng_Mark_GetSelectAlignRecipe();

	LPG_CSAI pstSetAlign = &uvEng_GetConfig()->set_align;

	if (!pstRecipe || mark_no > 0x03)
	{
		cam_id	= 0x01;
		img_id	= 0x00;
	}
	else
	{
		// by sysandj : ��������(����)
		//if (0/*pstRecipe->align_method*/ != UINT8(ENG_AOMI::en_each))
		if (pstSetAlign->align_method != UINT8(ENG_AOMI::en_each))
		{
			if (0x04 == uvEng_Luria_GetMarkCount(ENG_AMTF::en_global))
			{
				cam_id	= (mark_no < 2) ? 0x01 : 0x02;
				img_id	= (UINT8)ROUNDDN((mark_no % 2), 0);
			}
			/* Global Mark < 2 > Points */
			else
			{
				cam_id	= mark_no+1;
				img_id	= 0x00;
			}
		}
		else
		{
			cam_id	= 0x01;		/* Fixed to camera number = 0x01 */
			img_id	= mark_no;
		}
	}
}

/*
 desc : ���� ���õ� ������ ���� ��ϵ� Gerber�� �����ϴ��� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsRecipeGerberCheck()
{
	CHAR szGerbFile[MAX_PATH_LEN]	= {NULL};
	LPG_RJAF pstRecipe	= uvEng_JobRecipe_GetSelectRecipe();
	CUniToChar csCnv;

	/* ���� ���õ� �����ǰ� �ִ��� ���� */
	if (!pstRecipe)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"The selected gerber recipe does not exist");
		return FALSE;
	}

	/* �����ǿ� ���Ե� �Ź��� ������ ���� ����̺꿡 �ִ��� ���� Ȯ�� */
	sprintf_s(szGerbFile, MAX_PATH_LEN, "%s\\%s", pstRecipe->gerber_path, pstRecipe->gerber_name);
	if (!uvCmn_FindPath(csCnv.Ansi2Uni(szGerbFile)))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Gerber file does not exist");
		return FALSE;
	}

	return TRUE;
}

/*
 desc : ���� �Ź� ��ũ Ÿ���� Global���� Global & Local ȥ�� ������� ����
 parm : None
 retn : TRUE (Only Global) or FALSE (Mixed)
*/
BOOL CWork::IsMarkTypeOnlyGlobal()
{
	LPG_RAAF pstRecipe	= uvEng_Mark_GetSelectAlignRecipe();
	if (!pstRecipe)	return FALSE;
	
	return (ENG_ATGL::en_global_4_local_0_point == ENG_ATGL(pstRecipe->align_type));

	//if (ENG_ATGL::en_global_4_local_0x0_n_point == ENG_ATGL(pstRecipe->align_type) ||
	//	ENG_ATGL::en_global_3_local_0x0_n_point == ENG_ATGL(pstRecipe->align_type) ||
	//	ENG_ATGL::en_global_2_local_0x0_n_point == ENG_ATGL(pstRecipe->align_type))	return TRUE;

	//return FALSE;
}
#if 0
/*
 desc : Mark ��ȣ �������� Align Camera X�� Stage Y�� ��� ��ġ ���
 parm : mark_no	- [in]  ��ũ �ε��� (0x00 ~ 0x03) (Only Global Mark : 4)
		acam_x	- [out] ����� ī�޶��� ��� ��ġ ��ȯ (����: 100nm or 0.1 um)
		stage_y	- [out] ��������Y�� ��� ��ġ ��ȯ (����: 100nm or 0.1 um)
 retn : None
*/
VOID CWork::GetMovePosGlobalMark(UINT8 mark_no, INT32 &acam_x, INT32 &stage_y)
{
	UINT8 u8Mark	= uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	INT32 i32ACamDist, i32ACamHeight, i32ACamPos1;
	INT32 i32DiffMark12, i32DiffMark13, i32DiffMark24;

	/* �� �ʱ�ȭ */
	acam_x	= stage_y	= 0;
	/* ī�޶� ������ �������� �Ÿ� Ȯ�� */
	GetACamCentDistXY(i32ACamDist, i32ACamHeight);
	/* Stage Y �̵� ��ǥ ��� (�����ǿ� ������ Y �� �߰� �̵���ŭ ������� ��) */
	stage_y	= GetGlobalMarkMotionPosY(mark_no);
	/* Mark ��ġ�� ����, ī�޶� ���� ���� ���� ��ŭ �ٸ��� �� */
	if (0x04 == u8Mark)
	{
		/* 1���� 3�� ��ũ ���� ���� (����; ����) ��� */
		i32DiffMark13 = (INT32)ROUNDED(abs(uvEng_Luria_GetGlobalMarkDiffX(1, 3)) * 10000.0f, 0);	/* 100 nm or 0.1 um */
		/* 2���� 4�� ��ũ ���� ���� (����; ����) ��� */
		i32DiffMark24 = (INT32)ROUNDED(abs(uvEng_Luria_GetGlobalMarkDiffX(2, 4)) * 10000.0f, 0);	/* 100 nm or 0.1 um */
		switch (mark_no)
		{
		case 0x02	:
		case 0x03	:	stage_y	-= i32ACamHeight;	break;
		}
		/* ����� Mark 2���� �ش�Ǵ� ī�޶� 1���� X �� ���� ��� ��ġ */
		switch (mark_no)
		{
		case 0x00 : case 0x01 : i32ACamPos1	= GetACam1Mark2MotionX(mark_no);		break;
		case 0x02 : case 0x03 : i32ACamPos1	= GetACam1Mark2MotionX(mark_no-0x02);	break;
		}
		/* Mark ��ġ�� ����, ī�޶� ��ġ �ٸ��� �� */
		switch (mark_no)
		{
		case 0x00	:
		case 0x01	:	acam_x	= i32ACamPos1;	break;
		case 0x02	:	acam_x	= i32ACamPos1 + i32DiffMark13 - i32ACamDist;	break;
		case 0x03	:	acam_x	= i32ACamPos1 + i32DiffMark24 - i32ACamDist;	break;
		}
	}
	else if (0x02 == u8Mark)
	{
		/* 1���� 2�� ��ũ ���� ���� (����; ����) ��� */
		i32DiffMark12	= (INT32)ROUNDED(abs((INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(2) * 10000.0f, )), 0);	/* 100 nm or 0.1 um */
		if (0x01 == mark_no)	stage_y		-= i32ACamHeight;
		/* ����� Mark 2���� �ش�Ǵ� ī�޶� 1���� X �� ���� ��� ��ġ */
		if (0x00 == mark_no)	i32ACamPos1	= GetACam1Mark2MotionX(mark_no);
		else					i32ACamPos1	= GetACam1Mark2MotionX(mark_no-0x01);
		/* Mark ��ġ�� ����, ī�޶� ��ġ �ٸ��� �� */
		if (0x00 == mark_no)	acam_x	= i32ACamPos1;
		else					acam_x	= i32ACamPos1 + i32DiffMark12 - i32ACamDist;
	}
}
#endif
/*
 desc : Camera 1�� �������� Mark ��ȣ�� ���� Align Camera X�� Stage Y�� ��� ��ġ ���
 parm : mark_no	- [in]  ��ũ �ε��� (0x00 ~ 0x04?) (Only Global Mark : 2 or 4)
		acam_x	- [out] ����� ī�޶��� ��� ��ġ ��ȯ (����: 100nm or 0.1 um)
		stage_y	- [out] ��������Y�� ��� ��ġ ��ȯ (����: 100nm or 0.1 um)
 retn : None
*/
VOID CWork::GetMovePosGlobalMarkACam1(UINT8 mark_no, INT32 &acam_x, INT32 &stage_y)
{
	UINT8 u8Mark	= uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	UINT16 u16MarkType	= uvEng_Luria_GetGlobalMark3PType();
	INT32 i32ACamPos1;
	INT32 i32DiffMark1, i32DiffMark2;

	/* �� �ʱ�ȭ */
	acam_x	= stage_y	= 0;
	/* Stage Y �̵� ��ǥ ��� (�����ǿ� ������ Y �� �߰� �̵���ŭ ������� ��) */
	stage_y	= GetGlobalMarkMotionPosY(mark_no);
	/* Mark ��ġ�� ����, ī�޶� ���� ���� ���� ��ŭ �ٸ��� �� */
	if (0x04 == u8Mark)
	{
		/* 1���� 3�� ��ũ ���� ���� (����; ����) ��� */
		i32DiffMark1	= abs((INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(0) * 10000.0f, 0));	/* 100 nm or 0.1 um */
		/* 2���� 4�� ��ũ ���� ���� (����; ����) ��� */
		i32DiffMark2	= abs((INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(1) * 10000.0f, 0));	/* 100 nm or 0.1 um */
		/* ����� Mark 2���� �ش�Ǵ� ī�޶� 1���� X �� ���� ��� ��ġ */
		switch (mark_no)
		{
		case 0x00 : case 0x01 : i32ACamPos1	= GetACam1Mark2MotionX(mark_no);			break;
		case 0x02 : case 0x03 : i32ACamPos1	= GetACam1Mark2MotionX(mark_no-0x02);	break;
		}
		/* Mark ��ġ�� ����, ī�޶� ��ġ �ٸ��� �� */
		switch (mark_no)
		{
		case 0x00	:
		case 0x01	:	acam_x	= i32ACamPos1;					break;
		case 0x02	:	acam_x	= i32ACamPos1 + i32DiffMark1;	break;
		case 0x03	:	acam_x	= i32ACamPos1 + i32DiffMark2;	break;
		}
	}
	else if (0x02 == u8Mark)
	{
		/* 1���� 2�� ��ũ ���� ���� (����; ����) ��� */
		i32DiffMark1	= abs((INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(2) * 10000.0f, 0));	/* 100 nm or 0.1 um */
		/* ����� Mark 2���� �ش�Ǵ� ī�޶� 1���� X �� ���� ��� ��ġ */
		if (0x00 == mark_no)	i32ACamPos1	= GetACam1Mark2MotionX(mark_no);
		else					i32ACamPos1	= GetACam1Mark2MotionX(mark_no-0x01);
		/* Mark ��ġ�� ����, ī�޶� ��ġ �ٸ��� �� */
		if (0x00 == mark_no)	acam_x	= i32ACamPos1;
		else					acam_x	= i32ACamPos1 + i32DiffMark1;
	}
	else if (0x03 == u8Mark)
	{
		switch (u16MarkType)
		{
		case 0x1011	:	i32ACamPos1	= GetACam1Mark2MotionX(0);	break;
		case 0x0111	:
		case 0x1101	:
		case 0x1110	:	i32ACamPos1	= GetACam1Mark2MotionX(1);	break;
		}
		if (mark_no < 0x02)	acam_x	= i32ACamPos1;
		else
		{
			switch (u16MarkType)
			{
			case 0x0111	:
				switch (mark_no)
				{
				case 0x02	:	acam_x	= i32ACamPos1 + abs((INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(5) * 10000.0f, 0));	break;	/* 1���� 2�� ��ũ ���� ���� (����; ����) ��� */
				case 0x03	:	acam_x	= i32ACamPos1 + abs((INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(1) * 10000.0f, 0));	break;	/* 1���� 3�� ��ũ ���� ���� (����; ����) ��� */
				}
				break;
			case 0x1011	:
				switch (mark_no)
				{
				case 0x02	:	acam_x	= i32ACamPos1 + abs((INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(0) * 10000.0f, 0));	break;	/* 1���� 2�� ��ũ ���� ���� (����; ����) ��� */
				case 0x03	:	acam_x	= i32ACamPos1 + abs((INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(4) * 10000.0f, 0));	break;	/* 1���� 3�� ��ũ ���� ���� (����; ����) ��� */
				}
				break;
			case 0x1101	:	acam_x	= i32ACamPos1 + abs((INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(1) * 10000.0f, 0));	break;	/* 1���� 3�� ��ũ ���� ���� (����; ����) ��� */
			case 0x1110	:	acam_x	= i32ACamPos1 + abs((INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(5) * 10000.0f, 0));	break;	/* 1���� 3�� ��ũ ���� ���� (����; ����) ��� */
			}
		}
	}
}

/*
 desc : �뱤�� �غ� �Ǿ� �ֵ��� ���� ��� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::ResetExpoReady()
{
	/* ���� �۾� Step Name ���� */
	SetStepName(L"Init.Processing");

	/* �ϴ� Trigger ����� Disable ��Ŵ */
	if (!uvEng_Mvenc_ReqTriggerStrobe(FALSE))	return FALSE;
	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* ���� ��� photohead�� ���� ���� ���� ��û (�ϴ�. Z Axis�� Mid ���°� üũ �Ǿ� �ִ��� ����) */
		if (!uvEng_Luria_ReqGetMotorStateAll())	return FALSE;
	}
	/* ��� ����̺� ���� ���� Ȯ�� */
	if (!uvEng_MC2_SendDevStoppedAll())			return FALSE;
	/* Vacuum�� Off ���°� �ƴ��� ���� Ȯ�� */
	// by sysandj : MCQ��ü �߰� �ʿ� : if (!uvEng_MCQ_VacuumRunStop(0x01))			return FALSE;
	/* Chiller�� On ���°� �ƴ��� ���� Ȯ�� */
	// by sysandj : MCQ��ü �߰� �ʿ� : if (!uvEng_MCQ_ChillerRunStop(0x01))		return FALSE;

	return TRUE;
}

/*
 desc : ��� ī�޶� (ä��)�� Ʈ���� ��ġ�� �ʱ�ȭ �Ǿ����� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsTrigPosResetAll()
{
	if (!uvEng_Mvenc_IsTrigPosReset(0x01))	return FALSE;
	if (!uvEng_Mvenc_IsTrigPosReset(0x02))	return FALSE;

	return TRUE;
}

/*
 desc : ���� �۾��� ������ �������� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsWorkStopped()
{
	return (m_enWorkState == ENG_JWNS::en_error) || 
		   (m_enWorkState == ENG_JWNS::en_comp)  ||
		   (m_enWorkState == ENG_JWNS::en_time)  ||
		   (m_enWorkState == ENG_JWNS::en_none);
}

/*
 desc : ���� �ֱ��� Scenario�� ������ ������� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsWorkError()
{
	return (m_enWorkState == ENG_JWNS::en_error) || 
		   (m_enWorkState == ENG_JWNS::en_time);
}

/*
 desc : ���� Mark 2 �� ��������, ���� Mark ��ȣ�� �̵��ؾ� �� ī�޶� X ��ġ ��ȯ
 parm : mark_no	- [in]  Mark Number (0x01 or Later)
 retn : ��� �� (mm)
*/
DOUBLE CWork::GetACamMark2MotionX(UINT16 mark_no)
{
	//UINT8 u8ACamCount	= uvEng_GetConfig()->set_cams.acam_count;
	UINT8 u8ACamCount = 2;
	UINT8 u8MarkCount	= uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);	/* Global Mark ���� */
	INT32 i32Mark2ACamX	= 0;
	DOUBLE dbMark2ACamX	= 0.0f;
	STG_XMXY stMarkPos	= {NULL};
	LPG_CSAI pstAlign	= &uvEng_GetConfig()->set_align;
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	//LPG_RAAF pstRecipe	= uvEng_Mark_GetSelectAlignRecipe();	/* ���� ���õ� �Ź� ������ */
	// by sysandj : ��������(����)
	LPG_MACP pstThick	= uvEng_ThickCali_GetRecipe(pstRecipe->cali_thick);

	/* Thick Calibration ���� ���� */
	if (!pstThick)
	{
		AfxMessageBox(L"Failed to find the calibration file for Thick.Cali", MB_ICONSTOP|MB_TOPMOST);
		return 0.0f;
	}

	/* Mark Number�� ��� 1 based */
	if (mark_no < 0x01)	return 0.0f;
	/* ���� ����� �Ź��� Mark ? ��(Left/Bottom)�� ���� X, Y ��ǥ �� �������� */
	if (!uvEng_Luria_GetGlobalMark(mark_no-1, &stMarkPos))	return 0.0f;

	/* ����� ī�޶� 2���� ��� */
	dbMark2ACamX	= pstThick->mark2_acam_x[0];	/*pstAlign->mark2_acam_x[0];*/
	/* Global Mark ��ȣ�� 2 �̻��� ��� ��� */
	if (mark_no > (u8MarkCount/u8ACamCount))
	{
		/* Align Camera 2���� Mark 2���� ��� ���� */
		dbMark2ACamX	= pstThick->mark2_acam_x[1];	/*pstAlign->mark2_acam_x[1];*/
	}

	return	(dbMark2ACamX - pstAlign->mark2_org_gerb_xy[0]) + stMarkPos.mark_x;
}

/*
 desc : ���� ����� �Ź��� ���� Mark Number�� ����, �̵��ϰ��� �ϴ� ����� Stage X / Y ��ġ ��ȯ
 parm : mark_no	- [in]  Global Mark Number (1 or Later)
		acam_x	- [out] Motion �̵� ��ġ ��ȯ (X : mm)
		stage_y	- [out] Motion �̵� ��ġ ��ȯ (Y : mm)
 retn : TRUE or FALSE
*/
BOOL CWork::GetGlobalMarkMoveXY(UINT16 mark_no, DOUBLE &acam_x, DOUBLE &stage_y)
{
	UINT8 u8Mark		= uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	DOUBLE dbACamDistX	= 0.0f, dbACamDistY = 0.0f;	/* 2 �� �̻��� Align Camera �� ���, �� Camera ���� ��ġ ���� �� (����: mm) */
	STG_XMXY stMarkPos	= {NULL};	/* mark_no�� ���� ��ǥ ��ġ ��� */
	LPG_CSAI pstAlign	= &uvEng_GetConfig()->set_align;
	LPG_RJAF pstRecipe	= NULL;
	LPG_MACP pstThick	= NULL;

	pstRecipe	= uvEng_JobRecipe_GetSelectRecipe();	/* ���� ���õ� �Ź� ������ */
	if (!pstRecipe)
	{
		AfxMessageBox(L"No recipes are selected [Gerber]", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	// by sysandj : ��������(����)
	pstThick = uvEng_ThickCali_GetRecipe(pstRecipe->cali_thick);
	if (!pstThick)
	{
		AfxMessageBox(L"No recipes are selected [Thick Cali]", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* ���� Mark ������ 4�� �ƴϸ� ���� ó�� */
	if (u8Mark != 4)
	{
		AfxMessageBox(L"Invalid number of Global Marks.", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ���� �Ź��� ��ϵ� Mark Number�� ���� ��ǥ ��ġ ��� */
	if (!uvEng_Luria_GetGlobalMark(mark_no-1, &stMarkPos))	return FALSE;

	/* �ϴ�, ����� ī�޶� 1���� ���� 1 / 2 �� ��ũ�� Y �� ��� ��ǥ �� ��� */
	stage_y	= pstThick->mark2_stage_y[0];
	/* �˻� ��� ��ũ���� ���� ��ũ 2���� Y ��ǥ ���� �� ���� */

	STG_XMXY currGbrMark2;
	uvEng_Luria_GetGlobalMark(1, &currGbrMark2);

	stage_y	+= (stMarkPos.mark_y - currGbrMark2.mark_y) + (currGbrMark2.mark_y - pstAlign->mark2_org_gerb_xy[1]);
	/* ���� ����� ī�޶� 2���� ���, 3 / 4 �� ��ũ�� Y �� ��� ��ǥ �� ��� */
	// ���⵵ -- �� +�� ������.
	if (mark_no > 0x02)	stage_y	+= pstThick->GetACamDistY();

	/* Align Camera X ��ǥ ��� */
	acam_x	= GetACamMark2MotionX(mark_no);

	return TRUE;
}