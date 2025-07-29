
/*
 desc : �뱤�� �ʿ��� ��ü ������ ����
*/

#include "pch.h"
#include "../../MainApp.h"
#include "WorkRecipeLoad.h"
#include "../../../UVDI15/GlobalVariables.h"

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : ������
 parm : offset	- [in]  ���� �뱤�� ��, ������ ����� ���� �� (Pixel)
						�ٸ�, 0xff�̸�, ���� ȯ�� ���Ͽ� ������ ���� �� �״�� �뱤
						Hysterisys�� Negative Pixel ��
 retn : None
*/
CWorkRecipeLoad::CWorkRecipeLoad(ENG_BWOK workType, UINT8 offset, ENG_BWOK relayWork)
	: CWorkStep(relayWork)
{
	m_u8Offset		= offset;
	m_enWorkJobID = workType; //ENG_BWOK::en_gerb_load;
	m_lastUniqueID = 0;
	
	/*UI�� ǥ�õǴ� �߱Ⱚ �ʱ�ȭ*/
	LPG_PPTP pstParams = &uvEng_ShMem_GetLuria()->panel.get_transformation_params;
	pstParams->rotation = 0;
	pstParams->scale_xy[0] = 0;
	pstParams->scale_xy[1] = 0;
	uvEng_GetConfig()->measure_flat.MeasurePoolClear();
	LPG_GMLV pstMarkDiff = &uvEng_GetConfig()->mark_diff;
	pstMarkDiff->ResetMarkLen();
	GlobalVariables::GetInstance()->GetAlignMotion().markParams.workErrorType = ENG_WETE::en_none;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CWorkRecipeLoad::~CWorkRecipeLoad()
{
}

/*
 desc : �ʱ� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkRecipeLoad::InitWork()
{
	/* ���� ��� ���� �� �ʱ�ȭ */
	if (!CWork::InitWork())	return FALSE;

	/* InitWork ������ 1 ������ ���� ������, ���⼭�� �ٽ� 2 ������ �����ؾ� �� */
	m_u8SetPhNo		= 2;	/* Photohead Offset �� ���� ����� 2�� Photohead ���� �� */
	/* ��ü �۾� �ܰ� */
	m_u8StepTotal	= 0x25;
	//m_u8StepTotal = 37;

	/*Recipe Comp �̺�Ʈ Falue ����*/
	PhilRecipeInit();

	return TRUE;
}

/*
 desc : �ֱ������� �۾� ����
 parm : None
 retn : None
*/
VOID CWorkRecipeLoad::DoWork()
{

	/* �۾� �ܰ� ���� ���� ó�� */
	switch (m_u8StepIt)
	{
	case 0x01 : 
	{
		uvEng_Camera_ResetGrabbedImage();
		m_enWorkState = CheckValidRecipe();

		if (useAFtemp)
		{
			auto afInst = GlobalVariables::GetInstance()->GetAutofocus();

			if (uvEng_Luria_GetShMem()->focus.initialized == false)
			{
				m_enWorkState = afInst.InitFocusDrive() == false ? ENG_JWNS::en_error : m_enWorkState;
			}
			
			if (m_enWorkState != ENG_JWNS::en_error)
			{
				auto res1 = afInst.SetAFOnOff(1, false);
				auto res2 = afInst.SetAFOnOff(2, false);

				m_enWorkState = res1 && res2 ? m_enWorkState : ENG_JWNS::en_error;

				if (m_enWorkState != ENG_JWNS::en_error)
				{

					res1 = afInst.SetAFSensorOnOff(1, false);
					res2 = afInst.SetAFSensorOnOff(2, false);

					m_enWorkState = res1 && res2 ? m_enWorkState : ENG_JWNS::en_error;
				}
			}
		}
		m_enWorkState = m_enWorkState == ENG_JWNS::en_next && SetAutoFocusFeatures() ? m_enWorkState : ENG_JWNS::en_error;
	}
	break;

	case 0x02 : m_enWorkState = SetAllPhMotorHoming();			break;
	/* ��� Photohead�� �ʱ�ȭ�Ǵµ� �ʿ��� �ּ� ��� �ð� */
	case 0x03 : m_enWorkState = SetWorkWaitTime(1000);			break;
	case 0x04 : m_enWorkState = IsWorkWaitTime();				break;
	case 0x05 : m_enWorkState = IsAllPhMotorHomed();			break;
	/* ���� ��ϵ� �Ź� ����Ʈ ��û */
	case 0x06 : m_enWorkState = GetJobLists(2000);				break;
	case 0x07 : m_enWorkState = IsRecvJobLists();				break;
	/* ���� ����� Job Index ���� */
	case 0x08 : m_enWorkState = SetSelectedJobIndex();			break;
	case 0x09 : m_enWorkState = IsSelectedJobIndex();			break;
	/* ���õ� ���� ����� �Ź� ���� */
	case 0x0a : m_enWorkState = SetDeleteSelectedJobName(1000);	break;
	case 0x0b : m_enWorkState = IsDeleteSelectedJobName(0x05);	break;
	/* ��� Photohead Offset ���� */
	case 0x0c : m_enWorkState = SetPhOffset();					break;
	case 0x0d : m_enWorkState = IsPhOffset();					break;
	/* ��� Photohead Hysteresis ���� */
	case 0x0e : m_enWorkState = SetHysteresis(m_u8Offset);		break;
	case 0x0f : m_enWorkState = IsSetHysteresis();				break;
	/* Exposure Start XY */
	case 0x10 : m_enWorkState = SetExposeStartXY();				break;
	case 0x11 : m_enWorkState = IsExposeStartXY();				break;
	/* Gerber Regist */
	case 0x12 : m_enWorkState = SetGerberRegist();				break;
	case 0x13 : m_enWorkState = IsGerberRegisted();				break;
	/* Gerber Selecting */
	case 0x14 : m_enWorkState = SetJobNameSelecting();			break;
	case 0x15 : m_enWorkState = IsJobNameSelected();			break;
	/* Gerber Job Param */
	case 0x16 : m_enWorkState = GetGerberJobParam();			break;
	case 0x17 : m_enWorkState = IsSetGerberJobParam();			break;
	/* Gerber Loading */
	case 0x18 : m_enWorkState = SetJobNameLoading();			break;
	case 0x19 : m_enWorkState = IsJobNameLoaded();				break;
	/* Photohead Z Axis Up & Down */
	case 0x1a : m_enWorkState = SetPhZAxisMovingAll();			break;
	case 0x1b : m_enWorkState = IsPhZAxisMovedAll();			break;
	/* Align Camera Z Axis Up & Down */
	case 0x1c:
	{
		auto res = SetACamZAxisMovingAll(m_lastUniqueID);
		m_enWorkState = res != ENG_JWNS::en_next ? ENG_JWNS::en_wait : res;
		if (m_enWorkState != ENG_JWNS::en_next)
			this_thread::sleep_for(std::chrono::microseconds(500));
	}
	break;
	case 0x1d : m_enWorkState = IsACamZAxisMovedAll(m_lastUniqueID);		break;
	/* Led Duty Cycle & Frame Rate */
	case 0x1e : m_enWorkState = SetStepDutyFrame();				break;
	case 0x1f : m_enWorkState = IsStepDutyFrame();				break;
	/* Led Amplitude */
	case 0x20 : m_enWorkState = SetLedAmplitude();				break;
	case 0x21 : m_enWorkState = IsLedAmplituded();				break;
	/* Rectangle Lock */
	case 0x22 : m_enWorkState = SetRectangleLock();				break;
	case 0x23 : m_enWorkState = IsRectangleLock();				break;
	/* Load to xml file */
	case 0x24 : m_enWorkState = LoadSelectJobXML();				break;
#if 1
	/* �۾� ���� ���� ȭ�� */
	case 0x25 : m_enWorkState = IsGerberJobLoaded();			break;
#endif
	}

	/* ���� �۾� ���� ���� �Ǵ� */
	CWorkRecipeLoad::SetWorkNext();
	/* ��ð� ���� ���� ��ġ�� �ݺ� �����Ѵٸ� ���� ó�� */
	CWork::CheckWorkTimeout();
}

/*
 desc : ���� �ܰ�� �̵��ϱ� ���� ó��
 parm : None
 retn : None
*/
VOID CWorkRecipeLoad::SetWorkNext()
{
	UINT64 u64JobTime	= GetTickCount64() - m_u64StartTime;

	if (GetAbort())
	{
		CWork::EndWork();
		return;

	}
	/* �� �۾� �������� �ð� �� ���� ó�� */
	uvEng_UpdateJobWorkTime(u64JobTime);

	/* ��ġ�� ���а� ���� */
	UINT8 u8PhCount		= uvEng_GetConfig()->luria_svc.ph_count;
	UINT8 u8ACamCount	= uvEng_GetConfig()->set_cams.acam_count;

	if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* �۾��� ��� �� �ӽ� ���� */
		CWorkRecipeLoad::CalcStepRate();

		if (m_u8StepIt == m_u8StepTotal)
		{
			/*Philhmi�� Load ���� ��ȣ ������*/
			PhilRecipeLoadComp(0x01);

			m_enWorkState = ENG_JWNS::en_comp;
			/* �ʱ� Luria Error �� �ʱ�ȭ */
			uvCmn_Luria_ResetLastErrorStatus();

			/* �׻� ȣ��*/
			CWork::EndWork();
		}
		else
		{
			switch (m_u8StepIt)
			{
			/* Photohead ������ 2�� �̸��� ��� ��, 1���� ���� ���� ������ �� ���� */
			case 0x0b : if (u8PhCount < 2)				m_u8StepIt = 0x0d;	break;
			/* ��� Photohead�� Offset �� ���� �Ϸ� ���� */
			//case 0x0d : if (m_u8SetPhNo <= u8PhCount)	m_u8StepIt = 0x0b;	break;
			}
		}
		/* ���� �۾��� ó���ϱ� ���� */
		m_u8StepIt++;
		m_u64DelayTime	= GetTickCount64();
	}
//#if (CUSTOM_CODE_TEST_UVDI15 == 1)
	else if (ENG_JWNS::en_error == m_enWorkState)
	{
		/*Philhmi�� Load ���� ��ȣ ������*/
		PhilRecipeLoadComp(0x00);

		TCHAR tzMesg[128] = { NULL };
		swprintf_s(tzMesg, 128, L"Work Expo Align <Error Step It = 0x%02x>", m_u8StepIt);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);

		//m_enWorkState = ENG_JWNS::en_comp;
		m_u8StepIt = 0x00;
	}
//#endif

}

/*
 desc : �۾� ����� ���� (percent; %)
 parm : None
 retn : None
*/
VOID CWorkRecipeLoad::CalcStepRate()
{
	//UINT8 u8PhCount	= uvEng_GetConfig()->luria_svc.ph_count;
	//DOUBLE dbRate	= DOUBLE(m_u8StepIt + m_u8SetPhNo) / DOUBLE(m_u8StepTotal + u8PhCount) * 100.0f;
	DOUBLE dbRate = DOUBLE(m_u8StepIt + m_u8SetPhNo) / DOUBLE(m_u8StepTotal) * 100.0f;
	if (dbRate > 100.0f)	dbRate = 100.0f;
	/* ���� �޸𸮿� ���� */
	uvEng_SetWorkStepRate(dbRate);
}

/*
 desc : Job List ���� ���� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkRecipeLoad::IsRecvJobLists()
{
	ENG_JWNS enState	= IsWorkWaitTime();

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Recv.Job.Lists");

	/* ���� �ð� ��� */
	if (enState != ENG_JWNS::en_next)	return enState;
	/* �Ź� ����Ʈ (������)�� ���ŵǾ����� ���� Ȯ�� */
	if (!uvEng_Luria_IsRecvPktData(ENG_FDPR::en_get_job_list))
	{
		return ENG_JWNS::en_wait;
	}
	/* �Ź��� ���� ���, �� �̻� ������ �Ź��� ���� ������ */
	if (!uvCmn_Luria_IsJobList())
	{
		/* Luria HW Inited ����. (�׳� ������ ����) */
		if (!uvEng_Luria_ReqSetHWInit())
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (uvEng_Luria_ReqSetHWInit)");
			return ENG_JWNS::en_next;
		}
		/* ���� ��ϵ� �Ź� ��� ������ �ʿ� ���� */
		m_u8StepIt	= 0x0b;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : Set Rectangle Lock ���� ��Ŵ
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkRecipeLoad::SetRectangleLock()
{
	LPG_LDPP pstMemPanel	= &uvEng_ShMem_GetLuria()->panel;

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Rectangle.Lock");

	LPG_CGTI pstTrans = &uvEng_GetConfig()->global_trans;

	/* �ϴ� Enable�� �� ���� */
	pstMemPanel->global_rectangle_lock = 0x01;

	//abh1000 1019
	/* ������ 0x00 ������ ���� (����) ��Ŵ */
	//if (!uvEng_Luria_ReqSetGlobalRectangle(0x00))	return ENG_JWNS::en_error;
	if (!uvEng_Luria_ReqSetGlobalRectangle(pstTrans->use_rectangle))	return ENG_JWNS::en_error;
	/* ���� ���� Ȯ�α��� ���� �ð� ������ ���ؼ� */
	m_u64ReqSendTime = GetTickCount64();

	TCHAR tzMsg[256] = { NULL };
	swprintf_s(tzMsg, 256, L"Use Rectangle = %d", pstTrans->use_rectangle);
	LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);

	return ENG_JWNS::en_next;
}

/*
 desc : Rectangle Lock ���� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkRecipeLoad::IsRectangleLock()
{
	LPG_LDPP pstMemPanel	= &uvEng_ShMem_GetLuria()->panel;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Rectangle.Lock");
	/* Rectangle Lock�� ���� ���� ���ŵǾ��µ�, 0x00 ���� �ƴϸ�, ������ ���� ó�� */
		//abh1000 1019
	//if (pstMemPanel->global_rectangle_lock)	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : ���а� ��� ���� X/Y Offset �� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkRecipeLoad::SetPhOffset()
{
	LPG_LDMC pstMemMach	= &uvEng_ShMem_GetLuria()->machine;
	

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.PH.Offset");

	/* ���� ���� Photohead ������ 1���̸�, SKIP */
	if (uvEng_GetConfig()->luria_svc.ph_count < 2)
	{
		m_u8StepIt	= 0x0b /* IsDeletedLastJobName */;
	}
	else
	{
		/*�뱤 �ӵ��� ���� ������ ���� ���� ��� ������ ph_step*/

		/* ���� ���� ���� �ʱ�ȭ */
		pstMemMach->ResetPhOffset();
		LPG_CLSI pstLuria = &uvEng_GetConfig()->luria_svc;
		INT32 i32OffsetX, i32OffsetY;
		/* ���а� Offset X / Y �� ���� (2 �� ���а� ����) */
		for (int i = 1; i < pstLuria->ph_count; i++)
		{
			//pPkt = m_pPktMC->GetPktPhotoheadOffset(ENG_LPGS::en_set, i + 1,
			//	(UINT32)ROUNDED(pstLuria->ph_offset_x[i] * 1000000.0f, 0),	/* mm -> nm */
			//	(INT32)ROUNDED(pstLuria->ph_offset_y[i] * 1000000.0f, 0));	/* mm -> nm */
			//AddPktSend(pPkt, m_pPktMC->GetPktSize());
			//::Free(pPkt);

			i32OffsetX = (INT32)(pstLuria->ph_offset_x[i] * 1000000.0f);	/* unit : nm */
			i32OffsetY = (INT32)(pstLuria->ph_offset_y[i] * 1000000.0f);	/* unit : nm */

			/* Photohead�� Offset �� �����ϱ� ���� (���ʴ� Photohead Number�� 2 ����) */
			if (!uvEng_Luria_ReqSetSpecPhotoHeadOffset(i + 1, i32OffsetX, i32OffsetY))
			{
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetSpecPhotoHeadOffset)");
				return ENG_JWNS::en_error;
			}
		}

}

	//#else
//	if (uvEng_JobRecipe_GetSelectRecipe())
//	{
//		m_pDlgMain->PostMessageW(WM_MAIN_CHILD, WPARAM(ENG_CMDI::en_menu_expo), 0L);
//	}
//#endif

	return ENG_JWNS::en_next;
}

/*
 desc : ���а� ��� ���� X/Y Offset �� ���� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkRecipeLoad::IsPhOffset()
{
#if 0
	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Photohead.Offset");
	/* Photohead�� Offset ���� ���� �Ǿ����� ���� */
	if (!uvCmn_Luria_IsPhOffset(m_u8SetPhNo-1))	ENG_JWNS::en_wait;
	/* ���� Photohead Offset�� ���� ���� �����ϱ� ���� */
	

#endif
	//m_u8SetPhNo++;

	TCHAR tzMsg[256] = { NULL };
	LPG_CLSI pstLuria = &uvEng_GetConfig()->luria_svc;

	/* ���а� Offset X / Y �� ���� (2 �� ���а� ����) */
	for (int i = 1; i < pstLuria->ph_count; i++)
	{

		swprintf_s(tzMsg, 256, L"Ph%d Offset : X = %.4f Y = %.4f", i + 1, pstLuria->ph_offset_x[i], pstLuria->ph_offset_y[i]);
		LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
	}

	return ENG_JWNS::en_next;
}


/*
 desc : ���� ���õ� �Ź��� ���� Job Parameter ��û
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkRecipeLoad::GetGerberJobParam()
{
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Get.Gerber.Job.Param");

	/* ���� �޸𸮿� �ִ� �Ź� �Ķ���� ���� */
	pstJobMgt->ResetJobParam();

	/* Job ��� ��û */
	return uvEng_Luria_ReqGerberJobParam() ? ENG_JWNS::en_next : ENG_JWNS::en_error;
}

/*
 desc : ���� ���õ� �Ź��� ���� Job Parameter�� �����Ǿ� �ִ��� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkRecipeLoad::IsSetGerberJobParam()
{
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;
	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Set.Gerber.Job.Param");

	/* Job Param �����Ͱ� �����Ǿ� �ִ��� ���� */
	return pstJobMgt->get_job_params.IsValidData() ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : ���� ���õ� �������� �Ź��� ���� XML ���� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkRecipeLoad::LoadSelectJobXML()
{
	CUniToChar	csCnv;
	ENG_ATGL enType		= ENG_ATGL::en_not_defined;
	ENG_AMOS enMotion = ENG_AMOS::en_onthefly_2cam;
	bool isLocalSelRecipe = uvEng_JobRecipe_WhatLastSelectIsLocal();
	LPG_RJAF pstRecipe	= uvEng_JobRecipe_GetSelectRecipe(isLocalSelRecipe);
	//LPG_RAAF pstAlignRecipe = uvEng_Mark_GetSelectAlignRecipe();
	LPG_RAAF pstAlignRecipe = uvEng_Mark_GetAlignRecipeName(csCnv.Ansi2Uni(pstRecipe->align_recipe));
	/* ���� �۾� Step Name ���� */
	SetStepName(L"Load.Select.Job.XML");  

	//if (!pstRecipe || !pstAlignRecipe)
	//{
	//	LOG_ERROR(ENG_EDIC::en_uvdi15, L"There is no recipe currently selected");
	//	return ENG_JWNS::en_error;
	//}

		
	enType	= ENG_ATGL(pstAlignRecipe->align_type);
	enMotion = ENG_AMOS(pstAlignRecipe->align_motion);

	if (!uvEng_Luria_LoadSelectJobXML(enType))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to load the xml file for selected recipe");
		return ENG_JWNS::en_error;
	}

	//GlobalVariables::GetInstance()->GetAlignMotion().DoInitial(uvEng_GetConfig());
	GlobalVariables::GetInstance()->GetAlignMotion().UpdateParamValues();
	GlobalVariables::GetInstance()->GetRefindMotion().UpdateParamValues();

	//GlobalVariables::GetInstance()->GetAlignMotion().SetFiducial(uvEng_Luria_GetGlobalFiducial(),uvEng_Luria_GetLocalFiducial(), uvEng_GetConfig()->set_cams.acam_count);
	
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)

#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)

#endif
	

	/* GerberJobLoaded ��� �ð� �� ī���� �ʱ�ȭ */
	m_u64TickLoaded	= GetTickCount64();
	m_u8WaitLoaded	= 0x00;

	/* Job Param �����Ͱ� �����Ǿ� �ִ��� ���� */
	return ENG_JWNS::en_next;
}
#if 1
/*
 desc : ���� ��� (����) �� ���õ� �Ź� ������ (����)�� ���� ������ ���簡 �Ϸ� �Ǿ����� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkRecipeLoad::IsGerberJobLoaded()
{
	/* ���� ���õ� �Ź��� ���� �Ϸ� ���� (Loaded)���� ���� */
	BOOL bLoaded	= uvCmn_Luria_IsJobNameLoaded();

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Is.Loaded.Select.Job");

	/* ���ſ� �Ź� ������ ���, Job Time out�� �߻��� �� �����Ƿ�, 30�� �������� Counting �ʿ� */
	if (m_u64TickLoaded+30000 /* 30 sec*/ < GetTickCount64())
	{
		/* �Ź� ���� �Ϸ� ��� �ӽ� �ð��� ���� �ð����� �ʱ�ȭ */
		m_u64TickLoaded	= GetTickCount64();
		/* ���� Ƚ����ŭ ����ϵ��� ī���� */
		m_u8WaitLoaded++;
		/* �� 2 �� ���� ��� �ߴµ���, �Ϸᰡ �ȵǸ� Fail ó�� */
		if (m_u8WaitLoaded > 3)
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Timeout waiting for gerber file to complete loading");
			return ENG_JWNS::en_error;
		}
	}

	/* Job Param �����Ͱ� �����Ǿ� �ִ��� ���� */
	return bLoaded ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : Philhmi�� Reicpe Load �Ϸ� ��ȣ ������
 parm : None
 retn : None
*/
VOID CWorkRecipeLoad::PhilRecipeLoadComp(UINT8 state)
{
	STG_PP_P2C_EVENT_NOTIFY			stP2CEventNotify;
	STG_PP_P2C_EVENT_NOTIFY_ACK		stEventNotifyAck;

	stP2CEventNotify.Reset();
	stEventNotifyAck.Reset();

	if (state == 0x00)
	{
		stEventNotifyAck.usErrorCode = ePHILHMI_ERR_RECIPE_LOAD;
		stP2CEventNotify.bEventFlag = FALSE;
	}
	else
	{
		stP2CEventNotify.bEventFlag = TRUE;
	}


	sprintf_s(stP2CEventNotify.szEventName, DEF_MAX_EVENT_NAME_LENGTH, "RECIPE_COMP");
	uvEng_Philhmi_Send_P2C_EVENT_NOTIFY(stP2CEventNotify, stEventNotifyAck);


}

/*
 desc : Philhmi�� Reicpe Load �ʱ�ȭ ��ȣ
 parm : None
 retn : None
*/
VOID CWorkRecipeLoad::PhilRecipeInit()
{
	STG_PP_P2C_EVENT_NOTIFY			stP2CEventNotify;
	STG_PP_P2C_EVENT_NOTIFY_ACK		stEventNotifyAck;

	stP2CEventNotify.Reset();
	stEventNotifyAck.Reset();


	stEventNotifyAck.usErrorCode = ePHILHMI_ERR_RECIPE_LOAD;
	stP2CEventNotify.bEventFlag = FALSE;


	sprintf_s(stP2CEventNotify.szEventName, DEF_MAX_EVENT_NAME_LENGTH, "RECIPE_COMP");
	uvEng_Philhmi_Send_P2C_EVENT_NOTIFY(stP2CEventNotify, stEventNotifyAck);


}
#endif