
/*
 desc : 노광에 필요한 전체 레시피 적재
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
 desc : 생성자
 parm : offset	- [in]  직접 노광할 때, 강제로 적용될 단차 값 (Pixel)
						다만, 0xff이면, 기존 환경 파일에 설정된 단차 값 그대로 노광
						Hysterisys의 Negative Pixel 값
 retn : None
*/
CWorkRecipeLoad::CWorkRecipeLoad(ENG_BWOK workType, UINT8 offset, ENG_BWOK relayWork)
	: CWorkStep(relayWork)
{
	m_u8Offset		= offset;
	m_enWorkJobID = workType; //ENG_BWOK::en_gerb_load;
	m_lastUniqueID = 0;
	
	/*UI에 표시되는 추기값 초기화*/
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
 desc : 파괴자
 parm : None
 retn : None
*/
CWorkRecipeLoad::~CWorkRecipeLoad()
{
}

/*
 desc : 초기 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkRecipeLoad::InitWork()
{
	/* 내부 멤버 변수 값 초기화 */
	if (!CWork::InitWork())	return FALSE;

	/* InitWork 에서는 1 값으로 고정 됐지만, 여기서는 다시 2 값으로 변경해야 함 */
	m_u8SetPhNo		= 2;	/* Photohead Offset 값 적용 대상은 2번 Photohead 부터 임 */
	/* 전체 작업 단계 */
	m_u8StepTotal	= 0x25;
	//m_u8StepTotal = 37;

	/*Recipe Comp 이벤트 Falue 변경*/
	PhilRecipeInit();

	return TRUE;
}

/*
 desc : 주기적으로 작업 수행
 parm : None
 retn : None
*/
VOID CWorkRecipeLoad::DoWork()
{

	/* 작업 단계 별로 동작 처리 */
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
	/* 모든 Photohead가 초기화되는데 필요한 최소 대기 시간 */
	case 0x03 : m_enWorkState = SetWorkWaitTime(1000);			break;
	case 0x04 : m_enWorkState = IsWorkWaitTime();				break;
	case 0x05 : m_enWorkState = IsAllPhMotorHomed();			break;
	/* 현재 등록된 거버 리스트 요청 */
	case 0x06 : m_enWorkState = GetJobLists(2000);				break;
	case 0x07 : m_enWorkState = IsRecvJobLists();				break;
	/* 삭제 대상의 Job Index 설정 */
	case 0x08 : m_enWorkState = SetSelectedJobIndex();			break;
	case 0x09 : m_enWorkState = IsSelectedJobIndex();			break;
	/* 선택된 삭제 대상의 거버 제거 */
	case 0x0a : m_enWorkState = SetDeleteSelectedJobName(1000);	break;
	case 0x0b : m_enWorkState = IsDeleteSelectedJobName(0x05);	break;
	/* 모든 Photohead Offset 설정 */
	case 0x0c : m_enWorkState = SetPhOffset();					break;
	case 0x0d : m_enWorkState = IsPhOffset();					break;
	/* 모든 Photohead Hysteresis 설정 */
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
	/* 작업 적재 여부 화면 */
	case 0x25 : m_enWorkState = IsGerberJobLoaded();			break;
#endif
	}

	/* 다음 작업 진행 여부 판단 */
	CWorkRecipeLoad::SetWorkNext();
	/* 장시간 동안 동일 위치를 반복 수행한다면 에러 처리 */
	CWork::CheckWorkTimeout();
}

/*
 desc : 다음 단계로 이동하기 위한 처리
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
	/* 매 작업 구간마다 시간 값 증가 처리 */
	uvEng_UpdateJobWorkTime(u64JobTime);

	/* 설치된 광학계 개수 */
	UINT8 u8PhCount		= uvEng_GetConfig()->luria_svc.ph_count;
	UINT8 u8ACamCount	= uvEng_GetConfig()->set_cams.acam_count;

	if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* 작업률 계산 후 임시 저장 */
		CWorkRecipeLoad::CalcStepRate();

		if (m_u8StepIt == m_u8StepTotal)
		{
			/*Philhmi에 Load 성공 신호 보내기*/
			PhilRecipeLoadComp(0x01);

			m_enWorkState = ENG_JWNS::en_comp;
			/* 초기 Luria Error 값 초기화 */
			uvCmn_Luria_ResetLastErrorStatus();

			/* 항상 호출*/
			CWork::EndWork();
		}
		else
		{
			switch (m_u8StepIt)
			{
			/* Photohead 개수가 2개 미만인 경우 즉, 1개인 경우는 단차 적용할 수 없음 */
			case 0x0b : if (u8PhCount < 2)				m_u8StepIt = 0x0d;	break;
			/* 모든 Photohead의 Offset 값 설정 완료 여부 */
			//case 0x0d : if (m_u8SetPhNo <= u8PhCount)	m_u8StepIt = 0x0b;	break;
			}
		}
		/* 다음 작업을 처리하기 위함 */
		m_u8StepIt++;
		m_u64DelayTime	= GetTickCount64();
	}
//#if (CUSTOM_CODE_TEST_UVDI15 == 1)
	else if (ENG_JWNS::en_error == m_enWorkState)
	{
		/*Philhmi에 Load 실패 신호 보내기*/
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
 desc : 작업 진행률 설정 (percent; %)
 parm : None
 retn : None
*/
VOID CWorkRecipeLoad::CalcStepRate()
{
	//UINT8 u8PhCount	= uvEng_GetConfig()->luria_svc.ph_count;
	//DOUBLE dbRate	= DOUBLE(m_u8StepIt + m_u8SetPhNo) / DOUBLE(m_u8StepTotal + u8PhCount) * 100.0f;
	DOUBLE dbRate = DOUBLE(m_u8StepIt + m_u8SetPhNo) / DOUBLE(m_u8StepTotal) * 100.0f;
	if (dbRate > 100.0f)	dbRate = 100.0f;
	/* 전역 메모리에 설정 */
	uvEng_SetWorkStepRate(dbRate);
}

/*
 desc : Job List 수신 여부 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkRecipeLoad::IsRecvJobLists()
{
	ENG_JWNS enState	= IsWorkWaitTime();

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Recv.Job.Lists");

	/* 일정 시간 대기 */
	if (enState != ENG_JWNS::en_next)	return enState;
	/* 거버 리스트 (데이터)가 수신되었는지 여부 확인 */
	if (!uvEng_Luria_IsRecvPktData(ENG_FDPR::en_get_job_list))
	{
		return ENG_JWNS::en_wait;
	}
	/* 거버가 없는 경우, 더 이상 삭제할 거버가 없기 때문에 */
	if (!uvCmn_Luria_IsJobList())
	{
		/* Luria HW Inited 날림. (그냥 무조건 날림) */
		if (!uvEng_Luria_ReqSetHWInit())
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (uvEng_Luria_ReqSetHWInit)");
			return ENG_JWNS::en_next;
		}
		/* 기존 등록된 거버 목록 삭제할 필요 없음 */
		m_u8StepIt	= 0x0b;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : Set Rectangle Lock 해제 시킴
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkRecipeLoad::SetRectangleLock()
{
	LPG_LDPP pstMemPanel	= &uvEng_ShMem_GetLuria()->panel;

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Rectangle.Lock");

	LPG_CGTI pstTrans = &uvEng_GetConfig()->global_trans;

	/* 일단 Enable로 값 변경 */
	pstMemPanel->global_rectangle_lock = 0x01;

	//abh1000 1019
	/* 무조건 0x00 값으로 설정 (해제) 시킴 */
	//if (!uvEng_Luria_ReqSetGlobalRectangle(0x00))	return ENG_JWNS::en_error;
	if (!uvEng_Luria_ReqSetGlobalRectangle(pstTrans->use_rectangle))	return ENG_JWNS::en_error;
	/* 다음 상태 확인까지 일정 시간 지연을 위해서 */
	m_u64ReqSendTime = GetTickCount64();

	TCHAR tzMsg[256] = { NULL };
	swprintf_s(tzMsg, 256, L"Use Rectangle = %d", pstTrans->use_rectangle);
	LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);

	return ENG_JWNS::en_next;
}

/*
 desc : Rectangle Lock 설정 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkRecipeLoad::IsRectangleLock()
{
	LPG_LDPP pstMemPanel	= &uvEng_ShMem_GetLuria()->panel;

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Rectangle.Lock");
	/* Rectangle Lock에 대한 값이 수신되었는데, 0x00 값이 아니면, 무조건 실패 처리 */
		//abh1000 1019
	//if (pstMemPanel->global_rectangle_lock)	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : 광학계 헤드 간의 X/Y Offset 값 설정
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkRecipeLoad::SetPhOffset()
{
	LPG_LDMC pstMemMach	= &uvEng_ShMem_GetLuria()->machine;
	

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.PH.Offset");

	/* 만약 현재 Photohead 개수가 1개이면, SKIP */
	if (uvEng_GetConfig()->luria_svc.ph_count < 2)
	{
		m_u8StepIt	= 0x0b /* IsDeletedLastJobName */;
	}
	else
	{
		/*노광 속도에 따른 차별적 단차 조정 기능 미적용 ph_step*/

		/* 기존 단차 정보 초기화 */
		pstMemMach->ResetPhOffset();
		LPG_CLSI pstLuria = &uvEng_GetConfig()->luria_svc;
		INT32 i32OffsetX, i32OffsetY;
		/* 광학계 Offset X / Y 값 설정 (2 번 광학계 부터) */
		for (int i = 1; i < pstLuria->ph_count; i++)
		{
			//pPkt = m_pPktMC->GetPktPhotoheadOffset(ENG_LPGS::en_set, i + 1,
			//	(UINT32)ROUNDED(pstLuria->ph_offset_x[i] * 1000000.0f, 0),	/* mm -> nm */
			//	(INT32)ROUNDED(pstLuria->ph_offset_y[i] * 1000000.0f, 0));	/* mm -> nm */
			//AddPktSend(pPkt, m_pPktMC->GetPktSize());
			//::Free(pPkt);

			i32OffsetX = (INT32)(pstLuria->ph_offset_x[i] * 1000000.0f);	/* unit : nm */
			i32OffsetY = (INT32)(pstLuria->ph_offset_y[i] * 1000000.0f);	/* unit : nm */

			/* Photohead의 Offset 값 적용하기 위함 (최초는 Photohead Number가 2 값임) */
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
 desc : 광학계 헤드 간의 X/Y Offset 값 설정 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkRecipeLoad::IsPhOffset()
{
#if 0
	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Photohead.Offset");
	/* Photohead의 Offset 값이 적용 되었는지 여부 */
	if (!uvCmn_Luria_IsPhOffset(m_u8SetPhNo-1))	ENG_JWNS::en_wait;
	/* 다음 Photohead Offset의 간격 값을 설정하기 위함 */
	

#endif
	//m_u8SetPhNo++;

	TCHAR tzMsg[256] = { NULL };
	LPG_CLSI pstLuria = &uvEng_GetConfig()->luria_svc;

	/* 광학계 Offset X / Y 값 설정 (2 번 광학계 부터) */
	for (int i = 1; i < pstLuria->ph_count; i++)
	{

		swprintf_s(tzMsg, 256, L"Ph%d Offset : X = %.4f Y = %.4f", i + 1, pstLuria->ph_offset_x[i], pstLuria->ph_offset_y[i]);
		LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
	}

	return ENG_JWNS::en_next;
}


/*
 desc : 현재 선택된 거버에 대한 Job Parameter 요청
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkRecipeLoad::GetGerberJobParam()
{
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Get.Gerber.Job.Param");

	/* 기존 메모리에 있는 거버 파라미터 제거 */
	pstJobMgt->ResetJobParam();

	/* Job 명령 요청 */
	return uvEng_Luria_ReqGerberJobParam() ? ENG_JWNS::en_next : ENG_JWNS::en_error;
}

/*
 desc : 현재 선택된 거버에 대한 Job Parameter가 설정되어 있는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkRecipeLoad::IsSetGerberJobParam()
{
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;
	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Set.Gerber.Job.Param");

	/* Job Param 데이터가 설정되어 있는지 여부 */
	return pstJobMgt->get_job_params.IsValidData() ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : 현재 선택된 레시피의 거버에 대한 XML 파일 적재
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
	/* 현재 작업 Step Name 설정 */
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
	

	/* GerberJobLoaded 대기 시간 및 카운팅 초기화 */
	m_u64TickLoaded	= GetTickCount64();
	m_u8WaitLoaded	= 0x00;

	/* Job Param 데이터가 설정되어 있는지 여부 */
	return ENG_JWNS::en_next;
}
#if 1
/*
 desc : 현재 등록 (저장) 후 선택된 거버 데이터 (파일)가 광학 엔진에 적재가 완료 되었는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkRecipeLoad::IsGerberJobLoaded()
{
	/* 현재 선택된 거버가 적재 완료 상태 (Loaded)인지 여부 */
	BOOL bLoaded	= uvCmn_Luria_IsJobNameLoaded();

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Is.Loaded.Select.Job");

	/* 무거운 거버 파일인 경우, Job Time out이 발생할 수 있으므로, 30초 간격으로 Counting 필요 */
	if (m_u64TickLoaded+30000 /* 30 sec*/ < GetTickCount64())
	{
		/* 거버 적재 완료 대기 임시 시간을 현재 시간으로 초기화 */
		m_u64TickLoaded	= GetTickCount64();
		/* 일정 횟수만큼 대기하도록 카운팅 */
		m_u8WaitLoaded++;
		/* 약 2 분 정도 대기 했는데도, 완료가 안되면 Fail 처리 */
		if (m_u8WaitLoaded > 3)
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Timeout waiting for gerber file to complete loading");
			return ENG_JWNS::en_error;
		}
	}

	/* Job Param 데이터가 설정되어 있는지 여부 */
	return bLoaded ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : Philhmi에 Reicpe Load 완료 신호 보내기
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
 desc : Philhmi에 Reicpe Load 초기화 신호
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