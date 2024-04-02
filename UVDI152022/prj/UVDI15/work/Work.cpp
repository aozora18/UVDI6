
/*
 desc : 시나리오 기본 (Base) 함수 모음
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
 desc : 생성자
 parm : None
 retn : None
*/
CWork::CWork()
{
	m_enWorkJobID	= ENG_BWOK::en_work_none;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CWork::~CWork()
{
}

/*
 desc : 초기 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::InitWork()
{
	TCHAR tzMsg[128]= {NULL};

	/* 작업 이름 설정 */
	SetWorkName();	/* !!! 필히 호출해 줘야 됨 !!! */

	m_u8SetPhNo		= 0x00;		/* 무조건 0x00 로 고정 */
	m_u8StepTotal	= 0x00;
	m_u8StepIt		= 0x01;		/* 무조건 0x01 로 고정 */
	m_u8RetryStep	= 0xff;		/* 무조건 0xff 로 고정 */
	m_u8StepLast	= 0x0ff;	/* 무조건 0xff 로 고정 */
	m_u8RetryStep	= 0x00;		/* 무조건 0x00 로 고정 */
	m_u8RetryCount	= 0x00;
#if 0
	m_bWorkAbortSet	= FALSE;	/* 작업 취소 요청이 없다고 설정 */
#endif
	m_enWorkState	= ENG_JWNS::en_init;	/* !!! 초기 진입 모드 값 !!! */
	m_u64DelayTime	= GetTickCount64();
	m_u64StartTime	= m_u64DelayTime;		/* 작업 시작 시간 저장 */

	/* Work Step Name 초기화 */
	wmemset(m_tzStepName, 0x00, STEP_NAME_LEN);

	/* 트리거 보드 무조건 Disable */
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

	/* 작업 동작 (시작) 이력 저장 */
	SaveWorkLogs(L"The job has started");

	return TRUE;
}

/*
 desc : Job Work Name 설정
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
	case ENG_BWOK::en_mark_move		: wcscpy_s(m_tzWorkName, WORK_NAME_LEN, L"Mark Moving");		break;	/* Align Mark 위치로 이동 */
	case ENG_BWOK::en_mark_test		: wcscpy_s(m_tzWorkName, WORK_NAME_LEN, L"Mark Testing");		break;	/* Align Mark 검사 */
	case ENG_BWOK::en_expo_only		: wcscpy_s(m_tzWorkName, WORK_NAME_LEN, L"Direct Exposure");	break;	/* Only 노광 진행 */
	case ENG_BWOK::en_expo_align	: wcscpy_s(m_tzWorkName, WORK_NAME_LEN, L"Align Exposure");		break;	/* Align Mark 진행 후 노광 진행 */
	default							: wcscpy_s(m_tzWorkName, WORK_NAME_LEN, L"Unknown Work Name");	break;
	}
}

/*
 desc : 종료 작업 수행
 parm : None
 retn : None
*/
VOID CWork::EndWork()
{
	SaveWorkLogs(L"The job has ended");
	/* 작업 완료 시간 갱신 */
	uvEng_SetJobWorkInfo(UINT8(m_enWorkJobID), GetTickCount64() - m_u64StartTime);

	/* 트리거 보드 무조건 Disable */
// 	uvEng_Trig_ReqTriggerStrobe(FALSE);
// 	uvEng_Trig_ReqEncoderOutReset();
// 	uvEng_Trig_ResetTrigPosAll();
	uvEng_Mvenc_ReqTriggerStrobe(FALSE);
	uvEng_Mvenc_ReqEncoderOutReset();
	uvEng_Mvenc_ResetTrigPosAll();
}

/*
 desc : 작업 관련 로그 이력
 parm : mesg	- [in]  작업 동작 관련 메시지 버퍼
 retn : None
*/
VOID CWork::SaveWorkLogs(PTCHAR mesg)
{
	TCHAR tzMsg[256]	= {NULL};
	swprintf_s(tzMsg, 256, L"Job ID (0x%02u : %s) : %s", UINT8(m_enWorkJobID), m_tzWorkName, mesg);
	LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
}

/*
 desc : 다음 단계로 이동하기 위한 처리
 parm : None
 retn : None
*/
VOID CWork::SetWorkNext()
{
	UINT64 u64JobTime	= GetTickCount64() - m_u64StartTime;

	/* 매 작업 구간마다 시간 값 증가 처리 */
	uvEng_UpdateJobWorkTime(u64JobTime);

	/* 모든 작업이 종료 되었는지 여부 */
	if (ENG_JWNS::en_error == m_enWorkState)
	{


		TCHAR tzMesg[128] = { NULL };
		swprintf_s(tzMesg, 128, L"Work <Error Step It = 0x%02x>", m_u8StepIt);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);

		m_u8StepIt = 0x00;
	}
	else if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* 작업률 계산 후 임시 저장 */
		CalcStepRate();
		/* 작업이 완료 되었는지 여부 */
		if (m_u8StepTotal == m_u8StepIt)
		{
			m_enWorkState	= ENG_JWNS::en_comp;
			/* 항상 호출*/
			CWork::EndWork();
		}
		else
		{
			/* 다음 작업 단계로 이동 */
			m_u8StepIt++;
		}
		/* 가장 최근에 Next 수행한 시간 저장 */
		m_u64DelayTime	= GetTickCount64();
	}
}
#if 0
/*
 desc : 외부에서 강제로 작업 취소 요청이 올 경우
 parm : None
 retn : None
*/
VOID CWork::SetWorkStop()
{
	/* 동기 진입 */
	if (m_csSyncAbortSet.Enter())
	{
		m_bWorkAbortSet	= TRUE;
		/* 동기 해제 */
		m_csSyncAbortSet.Leave();
	}
}
#endif
/*
 desc : 작업 진행률 설정 (percent; %)
 parm : None
 retn : None
*/
VOID CWork::CalcStepRate()
{
	DOUBLE dbRate	= DOUBLE(m_u8StepIt) / DOUBLE(m_u8StepTotal) * 100.0f;
	if (dbRate > 100.0f)	dbRate = 100.0f;
	/* 전역 메모리에 설정 */
	uvEng_SetWorkStepRate (dbRate);
}

/*
 desc : Target System에게 명령을 송신하기 전에, 송신해도 되는 Timing인지 확인
 parm : wait	- [in]  대기 시간 (단위: msec)
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
 desc : 장시간 동일 Step에서 대기 중인지 판단
 parm : None
 retn : None
*/
VOID CWork::CheckWorkTimeout()
{
	BOOL bSucc	= FALSE;

	/* 60초 이상 지연되면 ... (대형 Job Loading 때문에 ...) */
	//bSucc	= GetTickCount64() > (m_u64DelayTime + 60000);	/* 60 초 이상 지연되면 */
	bSucc = GetTickCount64() > (m_u64DelayTime + 120000);	/* 60 초 이상 지연되면 */
	if (!bSucc)	return;	/* 현재 단계에서 작업 시간이 초과 안됨 */
#if 0
	/* 지연 (대기) 시간이 초과되면 에러 처리 */
	m_enWorkState	= ENG_JWNS::en_error;
#endif
	/* 자주 호출되는 곳이므로, 함수 부하를 줄이기 위해 ? (여기서 변수 선언) */
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

	/* 동작 중지 처리를 위한 플래그 설정 */
	m_u8StepIt		= 0x00;
	m_enWorkState	= ENG_JWNS::en_time;
}

/*
 desc : 현재 작업 이름 설정
 parm : name	- [in]  Step Name
 retn : None
*/
VOID CWork::SetStepName(PTCHAR name)
{
	/* 변경된 가장 최근 작업 정보 갱신 */
	swprintf_s(m_tzStepName, STEP_NAME_LEN, L"[%s] [%02u (%02x) / %02u]",
			   name, m_u8StepIt, m_u8StepIt, m_u8StepTotal);
	/* 가장 최근 값 임시 저장 */
	m_u8StepLast	= m_u8StepIt;


	//STG_PP_P2C_PROCESS_STEP		stProcessStep;
	//stProcessStep.Reset();
	//sprintf_s(stProcessStep.szGlassName, DEF_MAX_GLASS_NAME_LENGTH, "%d", m_u8StepIt);
	//sprintf_s(stProcessStep.szProcessStepName, DEF_MAX_SUBPROCESS_NAME_LENGTH, "%s", name);
	//uvEng_Philhmi_Send_P2C_PROCESS_STEP(stProcessStep);
}

/*
 desc : 거버가 정상적으로 등록/적재/선택되어 있는지 여부 
 parm : flag	- [in]  0x00: registered, 0x01: Selected, 0x02: Loaded (선택된 Job이 적재되어 있는지 여부)
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
 desc : 마크 개수 유효성 확인 (없거나, Global 4개 이거나, Local 16개 이거나....)
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
 desc : 등록된 거버에 Mark가 존재하는지 그리고 존재할 경우, 마크 간의 거리까지 유효한지도 확인
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
	/* 중요: 카메라들간 서로 충돌나지 않을 최소한의 거리 */
	/* 중요: Y 방향으로 마크를 인식하기 위한 최소한 거리 */
	/* ------------------------------------------------- */

	/* 마크 유효성 검증 */
	if (!IsValidMarkCount())	return FALSE;

	auto status = GlobalVariables::getInstance()->GetAlignMotion().status;

	int localCnt = uvEng_Luria_GetMarkCount(ENG_AMTF::en_local);
	const int CAM1 = 1, CAM2 = 2, FIRST_SCAN = 0, FIRST_MARK = 0, SECOND_MARK = 1;
	
		
	if (localCnt != 0)
	{
		

		auto cam1_1st =  status.markMapConst[FIRST_SCAN][FIRST_MARK].tgt_id;
		auto cam1_2nd = status.markMapConst[FIRST_SCAN][SECOND_MARK].tgt_id;

		auto cam2_1st = status.markPoolForCamLocal[CAM2].begin()->tgt_id;

		/* 4th & 11th Mark 간의 값 얻어와 X 축 넓이 얻기 */
		if (!uvEng_Luria_GetLocalMark(cam1_1st, &stPosX[0]))	return FALSE;
		if (!uvEng_Luria_GetLocalMark(cam2_1st,&stPosX[1]))	return FALSE;
		/* 1th & 2th Mark 간의 값 얻어와 Y 축 길이 얻기 */
		if (!uvEng_Luria_GetLocalMark(cam1_1st, &stPosY[0]))	return FALSE;
		if (!uvEng_Luria_GetLocalMark(cam1_2nd, &stPosY[1]))	return FALSE;
		/* -------------------------------------------------------------------------------------------- */
		/* Align Camera 간의 물리적 떨어진 간격 값이 거버에 등록된 Mark 간의 넓이 값보다 큰 경우에 문제 */
		/* 거버에 등록된 2개의 Mark 넓이 (폭) 값이 물리적으로 떨어진 2대의 Align Camera보다 작다면 문제 */
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
		/* 2st & 4th Mark 간의 값 얻어와 X 축 넓이 얻기 */
		if (!uvEng_Luria_GetGlobalMark(1, &stPosX[0]))	return FALSE;
		if (!uvEng_Luria_GetGlobalMark(3, &stPosX[1]))	return FALSE;
		/* 2st & 4th Mark 간의 값 얻어와 Y 축 길이 얻기 */
		if (!uvEng_Luria_GetGlobalMark(0, &stPosY[0]))	return FALSE;
		if (!uvEng_Luria_GetGlobalMark(1, &stPosY[1]))	return FALSE;
		/* -------------------------------------------------------------------------------------------- */
		/* Align Camera 간의 물리적 떨어진 간격 값이 거버에 등록된 Mark 간의 넓이 값보다 큰 경우에 문제 */
		/* 거버에 등록된 2개의 Mark 넓이 (폭) 값이 물리적으로 떨어진 2대의 Align Camera보다 작다면 문제 */
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
 desc : Local Fiducial의 Mark 중 맨 윗 (상위) 부분의 Left / Right Mark Number 값 반환
 parm : scan	- [in]  Scan Number (0 or 1)
		left	- [out] Left Bottom Mark Number 값 반환 (Zero based)
		right	- [out] Right Bottom Mark Number 값 반환 (Zero based)
 retn : TRUE or FALSE
*/
BOOL CWork::GetLocalLeftRightTopMarkIndex(UINT8 scan, UINT8 &left, UINT8 &right)
{
	/* 현재 선택된 레시피 정보 얻기 */
	LPG_RAAF pstRecipe	= uvEng_Mark_GetSelectAlignRecipe();
	if (!pstRecipe)	return FALSE;


	
	//스캔은 각 캠 중앙에서 이루어진다. 

	auto status = GlobalVariables::getInstance()->GetAlignMotion().status;

	
	int centerCol = status.GetCenterColumn();

	left = status.markMapConst[scan].front().tgt_id; //1캠
	right = status.markMapConst[centerCol + scan].front().tgt_id; //2캠

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
 desc : Local Fiducial의 Mark 중 맨 아랫 (하위) 부분의 Left / Right Mark Number 값 반환
 parm : scan	- [in]  Scan Number (0 or 1)
		left	- [out] Left Bottom Mark Number 값 반환 (Zero based)
		right	- [out] Right Bottom Mark Number 값 반환 (Zero based)
 retn : TRUE or FALSE
*/
BOOL CWork::GetLocalLeftRightBottomMarkIndex(UINT8 scan, UINT8 &left, UINT8 &right)
{
	/* 현재 선택된 레시피 정보 얻기 */
	LPG_RAAF pstRecipe = uvEng_Mark_GetSelectAlignRecipe();
	if (!pstRecipe)	return FALSE;


	//스캔은 횟수이자. 방향을 의미 
	//스캔이 2로 나누어 떨어지면 돌아오는 타이밍인걸 의미 

	/*
	아래 코드를 보자. 

	scan이 0일땐 3 11을 반환하잖나? 즉 위로 올라갈때이다. 
	1일땐 7 15
	
	*/

	//스캔은 각 캠 중앙에서 이루어진다. 

	auto alType = pstRecipe->align_type;

	

	auto status = GlobalVariables::getInstance()->GetAlignMotion().status;

	if (status.gerberColCnt <= 0 || status.gerberRowCnt <= 0) return FALSE;

	int centerCol = status.GetCenterColumn();

	left = status.markMapConst[scan].back().tgt_id; //1캠
	right = status.markMapConst[centerCol + scan].back().tgt_id; //2캠
	

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

	/* MC2 Error가 발생된 상태인지 여부 확인 */
	if (uvCmn_MC2_IsDNCError())
	{
		m_u8StepIt	= 0x00;
		m_enWorkState	= ENG_JWNS::en_error;
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"DNC error occurred in MC2");
		bIsError = TRUE;
	}
	/* 임의 모션 드라이브에서 에러가 발생 했는지 여부 */
	if (uvCmn_MC2_IsAnyDriveError())
	{
		m_u8StepIt	= 0x00;
		m_enWorkState	= ENG_JWNS::en_error;
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Motion Drive error occurred in MC2");
		bIsError = TRUE;
	}

	/* 에러가 발생되었으면, MC2 Drive의 모든 상태 정보 저장 */
	if (bIsError)	uvCmn_MC2_SaveAllDriveState();

	return bIsError;
}

/*
 desc : 현재 레시피가 선택되었는지와 유효한 값이 저장되어 있는지 여부
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
 desc : 현재 Luria Status 가 에러인지 여부
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
 desc : 현재 2대의 align camera에 등록된 Mark가 존재하는지 여부
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsAlignMarkRegisted() // lk91!! global mark, local mark 체크
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
 desc : Align Moving 할 때, 스테이지 Y 축에 있을 Mark의 Left->Bottom 위치를 찾고자 이동하려는 위치
 parm : mark	- [in]  global or local
		scan	- [in]  Scan Number (0 or 1)
		direct	- [in]  0x00 : Left, 0x01 : Right
 retn : Y 위치 값 반환 (단위: 100 nm or 0.1 um) (음수 값은 실패)
*/
INT32 CWork::GetLeftRightBottomMarkPosY(ENG_AMTF mark, UINT8 scan, UINT8 direct)
{
	UINT8 u8Left, u8Right;
	DOUBLE dbDiffMarkY		= 0, dbMark2StageY	= 0;
	STG_XMXY stMarkPos		= {NULL};
	LPG_CSAI pstSetAlign	= &uvEng_GetConfig()->set_align;
	//LPG_RAAF pstRecipe		= uvEng_Mark_GetSelectAlignRecipe();
	LPG_RJAF pstRecipe		= uvEng_JobRecipe_GetSelectRecipe();
	// by sysandj : 변수없음(수정)
	LPG_MACP pstThickCali	= uvEng_ThickCali_GetRecipe(pstRecipe->cali_thick);

	/* 노광 속도에 따른 트리거 발생 위치 값 설정 */
	if (!pstThickCali)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"no_cali_trig_data [Stage_Y]");
		return -1;
	}
	else
	{
		dbMark2StageY	= pstThickCali->mark2_stage_y[0];	/* 일단 1 번 카메라 기준 임 */
	}

	/* 환경 파일에 설정된 기준 2 번 마크의 위치와 현재 적재된 2 번 마크 간의 오차 값 추출 */
	if (ENG_AMTF::en_global == mark)
	{
		if (0x00 == direct)
		{
			//dbMark2StageY = pstThickCali->mark2_stage_y[0];	/* 일단 1 번 카메라 기준 임 */
			if (!uvEng_Luria_GetGlobalMark(1, &stMarkPos))	return -1;
		}
		else
		{
			//dbMark2StageY = pstThickCali->mark2_stage_y[1];	/* 일단 1 번 카메라 기준 임 */
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
	/* 기준 마크의 2번 Y 좌표와 현재 적재된 거버의 2번 마크 간의 Y 축 높이 오차 값 */
	dbDiffMarkY = stMarkPos.mark_y - uvEng_GetConfig()->set_align.mark2_org_gerb_xy[1];

	/* 2 번 마크의 실제 모션 Y 좌표 값에 위에서 추출된 오차 값을 더해 주면 됨 */
	
	
	/*문제점
	*  아래는 원래 식인데 dbMark2StageY - dbDiffMarkY 였다.
	이 경우 dbdiffmark가 0이거나 (같은거버일때) 혹은 +값일때는 정상적이나 
	음수일때는  - - = +가 되기때문에 오히려 + 방향으로 움직인다 (옵셋이 반대로 가는 현상)
	 return (INT32)ROUNDED((dbMark2StageY - dbDiffMarkY) * 10000.0f, 0);	/* 100 nm or 0.1 um */ 
	
	return (INT32)ROUNDED((dbMark2StageY + dbDiffMarkY) * 10000.0f, 0);	/* 100 nm or 0.1 um */ 


}

/*
 desc : Gerber에서 Mark 위치와 대응되는 Motion Y 축의 실제 위치를 찾기 위함
 parm : mark_no	- [in]  Global Align Mark Number (0x00 ~ 0x03)
 retn : Y 위치 값 반환 (단위: 100 nm or 0.1 um) (음수 값은 실패)
*/
INT32 CWork::GetGlobalMarkMotionPosY(UINT8 mark_no)
{
	DOUBLE dbDiffMarkY		= 0, dbMark2StageY	= 0;
	STG_XMXY stMarkPos		= {NULL};
	LPG_CSAI pstSetAlign	= &uvEng_GetConfig()->set_align;
	//LPG_RAAF pstRecipe		= uvEng_Mark_GetSelectAlignRecipe();
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	// by sysandj : 변수없음(수정)
	LPG_MACP pstThickCali	= uvEng_ThickCali_GetRecipe(pstRecipe->cali_thick);

	/* 만약, Trigger Calibration에 저장 (관리)된 데이터가 없다면, 환경 파일에 있는 정보 값 가져오기 */
	if (!pstThickCali)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"no_cali_trig_data for global mark [Stage_Y]");
		return -1;
	}
	else
	{
		dbMark2StageY	= pstThickCali->mark2_stage_y[0];	/* 일단 1 번 카메라 기준 임 */
	}
	/* Mark 위치 정보 얻기 */
	if (!uvEng_Luria_GetGlobalMark(mark_no, &stMarkPos))	return -1;
	/* 환경 파일에 설정된 기준 2 번 마크의 위치와 현재 적재된 mark_no 번 마크 간의 오차 값 추출 */
	dbDiffMarkY	= stMarkPos.mark_y - uvEng_GetConfig()->set_align.mark2_org_gerb_xy[1];

#if 0
	TRACE(L"mark_no = %d  mark_y = %8.4f  mark2_y = %9d diff_y = %9d\n",
		  mark_no, stMarkPos.y, pstTrigCali->mark2_stage_y, i32DiffMarkY);
#endif

	/* Calibration 2 번 마크의 실제 모션 Y 좌표 값에 위에서 추출된 오차 값을 더해 주면 됨 */
	return (INT32)ROUNDED((dbMark2StageY + dbDiffMarkY) * 10000.0f, 0);	/* 100 nm or 0.1 um */
}

/*
 desc : Gerber에서 존재하는 Mark 위치에 해당된 Motion Y 축의 실제 위치를 찾기 위함
 parm : index	- [in]  Local Align Mark가 저장된 메모리 인덱스 (0x00 ~ )
 retn : Y 위치 값 반환 (단위: 100 nm or 0.1 um)
*/
INT32 CWork::GetLocalMarkMotionPosY(UINT8 index)
{
	DOUBLE dbDiffMarkY		= 0, dbMark2StageY	= 0;
	STG_XMXY stMarkPos		= {NULL};
	LPG_CSAI pstSetAlign	= &uvEng_GetConfig()->set_align;
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	//LPG_RAAF pstRecipe		= uvEng_Mark_GetSelectAlignRecipe();
	// by sysandj : 변수없음(수정)
	LPG_MACP pstThickCali	= uvEng_ThickCali_GetRecipe(pstRecipe->cali_thick);

	if (!pstThickCali)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"no_cali_trig_data for local mark [Stage_Y]");
		return -1;
	}
	else
	{
		dbMark2StageY	= pstThickCali->mark2_stage_y[0];	/* 일단 1 번 카메라 기준 임 */
	}
	/* Mark 위치 정보 얻기 */
	if (!uvEng_Luria_GetLocalMark(index, &stMarkPos))	return -1;
	/* 환경 파일에 설정된 기준 2 번 마크의 위치와 현재 적재된 2 번 마크 간의 오차 값 추출 */
	dbDiffMarkY	= stMarkPos.mark_y - uvEng_GetConfig()->set_align.mark2_org_gerb_xy[1];		/* 100 nm or 0.1 um */

	/* Calibration 2 번 마크의 실제 모션 Y 좌표 값에 위에서 추출된 오차 값을 더해 주면 됨 */
	return (INT32)ROUNDED((dbMark2StageY + dbDiffMarkY) * 10000.0f, 0);	/* 100 nm or 0.1 um */
}

/*
 desc : Gerber 파일 내에 저장된 Mark (Global 4개와 나머지 Local)들의
		위치에 해당되는 모든 트리거 위치 계산 및 임시 저장
 parm : acam_diff_y	- [in]  얼라인 카메라 1번 기준으로 2번 카메라가 설치된 높이 오차 값 (단위: 0.1 um or 100 nm)
		mark_diff_y	- [in]  거버 내의 좌표에서 Left와 Right의 마크 간의 Y 높이 오차 값 (단위: 0.1 um or 100 nm)
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
	LPG_MACP pstThickCali = uvEng_ThickCali_GetRecipe(pstRecipe->cali_thick); //thick값.

	double trigPosCam[SidecamCnt]={0,};// 트리거 저장될 버퍼 .
	
	GetACamCentDistXY(camDistXY[0], camDistXY[1]);
	camDistXY[0] *= 10000.0f; camDistXY[1] *= 10000.0f;

	double orgMark2[2] = { pstConfig->set_align.mark2_org_gerb_xy[0],
						   pstConfig->set_align.mark2_org_gerb_xy[1] };

	uvEng_Luria_GetGlobalMark(MARK2_INDEX, pxmlMark2);

	double yGab = orgMark2[Y] - pxmlMark2->mark_y;

	double stageYMark2 = (pstThickCali->mark2_stage_y[0] - yGab) * 10000.0f; //마크2를 바라보는 스테이지 y 좌표를 의미함. 

	int markOrder = 0;
	auto status = GlobalVariables::getInstance()->GetAlignMotion().status;
	auto scans = status.gerberColCnt / WANG_BOCK;
	UINT8 u8ScanMarks = uvEng_Recipe_GetScanLocalMarkCount();		/* 1 Scan 기준 - 저장된 Fiducial 개수 */
	auto centerCol = status.GetCenterColumn();
	bool backward = true;
	int trigAdded = 0; //현재 등록된 트리거 갯수 
		
	map<int,vector<double>> trigMap;
	for (int scan = 0; scan < scans; scan++)
	{
		UINT32 startPos = backward ? expoY : unloadY;
			
		double placeGab = fabs((double)stageYMark2 - (double)startPos);
		for (int i = 0; i < u8ScanMarks; i++, markOrder++)
		{
			
			UINT32 camLocalMark[] = { status.markPoolForCamLocal[1][markOrder].tgt_id , status.markPoolForCamLocal[2][markOrder].tgt_id}; //트리거 등록할 마크번호

			double markGab[] = { uvEng_Luria_GetGlobalBaseMarkLocalDiffY(0x00, camLocalMark[0]) * 10000.0f, 
								 uvEng_Luria_GetGlobalBaseMarkLocalDiffY(0x01, camLocalMark[1]) * 10000.0f};

			trigPosCam[0] = (placeGab + (backward ? markGab[CAM1] : -markGab[CAM1])) * (backward ? 1 : -1);
			trigPosCam[1] = (placeGab + (backward ? markGab[CAM2] : -markGab[CAM2])) * (backward ? 1 : -1);
				
			trigMap[0].push_back(trigPosCam[0] + (backward ? pstTrigSet->trig_backward  : pstTrigSet->trig_forward));
			trigMap[1].push_back(trigPosCam[1] + (backward ? pstTrigSet->trig_backward  : pstTrigSet->trig_forward) + camDistXY[Y]);


			/* Align Camera 2D 보정 여부 */
			if (uvEng_GetConfig()->set_align.use_2d_cali_data)
			{
				/* Align Mark가 측정될 모션 Y 축의 실제 위치 값 */
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

	// by sysandj : 함수없음(수정)
	UINT8 i, j, k, u8ScanMarks	= uvEng_Recipe_GetScanLocalMarkCount();		/* 1 Scan 기준 - 저장된 Fiducial 개수 */
	INT32 i32MarkMovePosY		= -1, i32MarkDiffPosY;
	INT32 i32StageUnloadY		= {NULL};
	INT32 i32AlignCam1[SidecamCnt]		= {NULL};	/* 100 nm or 0.1 um */
	INT32 i32AlignCam2[SidecamCnt]		= {NULL};	/* 100 nm or 0.1 um */
	INT32 i32Mark2TrigPos		= 0;		/* Global Fiducial Mark Left  / Bottom Position (Trigger) (100 nm or 0.1 um )*/
	INT32 i32Mark4TrigPos		= 0;		/* Global Fiducial Mark Right / Bottom Position (Trigger) (100 nm or 0.1 um )*/
	LPG_CIEA pstConfig			= uvEng_GetConfig();
	LPG_CTSP pstTrigSet			= &pstConfig->trig_grab;

	INT32 i32EndStartPosY		= 0;
	/* 반드시 초기화 수행 */
	uvEng_ACamCali_ResetAllCaliData();

	/* Stage X, Y, Camera 1 & 2 X 이동 좌표 얻기 */
	i32StageUnloadY	= (INT32)ROUNDED(uvEng_GetConfig()->set_align.table_unloader_xy[0][1] * 10000.0f, 0);	/* 100 nm or 0.1 um */
	/* Stage Y의 시작 부터 끝까지 거리 구하기 */
	//i32EndStartPosY = i32StageUnloadY - (uvEng_GetConfig()->luria_svc.table_expo_start_xy[0][1] * 10000.0f);
	/* --------------------------------------------- */
	/* Global Mark가 인식될 Trigger Position 값 등록 */
	/* --------------------------------------------- */
	/* 2 번 (Left/Bottom) Mark를 인식하기 위한 트리거 위치 얻기  */
	i32MarkMovePosY	= GetLeftRightBottomMarkPosY(ENG_AMTF::en_global, 0x00 /*scan*/, 0x00/*direct*/);	/* 100 nm or 0.1 um */
	if (i32MarkMovePosY < 0)	return FALSE;
	//i32AlignCam1[1]	= i32StageUnloadY - i32MarkMovePosY;
	i32AlignCam1[0] = i32StageUnloadY - i32MarkMovePosY;
	/* 1 번 (Left/Top) Mark를 인식하기 위한 트리거 위치 얻기  */
	i32MarkDiffPosY = (INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffY(1 /* Left/Top */, 2 /* Left/Bottom */) * 10000.0f, 0);
	//i32AlignCam1[0]	= i32AlignCam1[1] - i32MarkDiffPosY;	/* 100 nm or 0.1 um */
	i32AlignCam1[1] =  abs(i32AlignCam1[0]) - i32MarkDiffPosY;	/* 100 nm or 0.1 um */


	/* 4 번 (Right/Bottom) Mark를 인식하기 위한 트리거 위치 얻기 (카메라 간의 높이 값은 여기서 제외) */
	i32MarkMovePosY	= GetLeftRightBottomMarkPosY(ENG_AMTF::en_global, 0x00 /*scan*/, 0x01/*direct*/);	/* 100 nm or 0.1 um */
	if (i32MarkMovePosY < 0)	return FALSE;
	//i32AlignCam2[1]	= i32StageUnloadY - i32MarkMovePosY;
	i32AlignCam2[0] = i32StageUnloadY - i32MarkMovePosY;
	/* 3 번 (Left/Top) Mark를 인식하기 위한 트리거 위치 얻기  */
	i32MarkDiffPosY = (INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffY(3 /* Right/Top */, 4 /* Right/Bottom */) * 10000.0f, 0);
	//i32AlignCam2[0]	= i32AlignCam2[1] - i32MarkDiffPosY;	/* 100 nm or 0.1 um */
	i32AlignCam2[1] = abs(i32AlignCam2[0]) - i32MarkDiffPosY;	/* 100 nm or 0.1 um */


	i32AlignCam1[0] *= -1;
	i32AlignCam1[1] *= -1;

	i32AlignCam2[0] *= -1;
	i32AlignCam2[1] *= -1;

	
	

	/* Global Fiducial Mark의 Left & Right의 Bottom에 해당되는 Trigger Position 값 임시 저장 */
	i32Mark2TrigPos = i32AlignCam1[1];	/* Left  / Bottom의 Trigger Position 값 (Align Camera 1) */
	i32Mark4TrigPos = i32AlignCam2[1];	/* Right / Bottom의 Trigger Position 값 (Align Camera 1) */

	//INT32 AlignCam1_05, AlignCam2_05;
	/* 4 개의 Mark 위치에 공통으로 트리거 지연 값 적용 (단위: 0.1 um or 100 nm) */
	for (i=0; i< SidecamCnt; i++)
	{
		/* Align Camera Trigger 1 대비 Camera Trigger 2 의 Delay 값 보정 */
		/* Align Camera 1 & 2번의 물리적인 설치 오차 값도 반영해야 한다. */
		i32AlignCam2[i]	+= acam_diff_y;
		/* 메모리에 임시로 트리거가 발생될 위치 등록 */
		uvEng_Trig_SetTrigger(ENG_AMTF::en_global, 1, i, i32AlignCam1[i]+pstTrigSet->trig_forward);
		uvEng_Trig_SetTrigger(ENG_AMTF::en_global, 2, i, i32AlignCam2[i]+pstTrigSet->trig_forward);

		/* Align Camera 2D 보정 여부 */
		if (uvEng_GetConfig()->set_align.use_2d_cali_data)
		{
			uvEng_ACamCali_AddMarkPos(1, ENG_AMTF::en_global, 0x01, i,
										GetGlobalMarkMotionPosY(i)/10000.0f);
			uvEng_ACamCali_AddMarkPos(2, ENG_AMTF::en_global, 0x01, i,
										(GetGlobalMarkMotionPosY(i+2)+acam_diff_y)/10000.0f);
		}
	}
	/* 결국 노광 시작 위치까지 이동하면 됨. 마크 인식 후 바로 노광할 수 있도록 하기 위함 */
	m_dbAlignStageY	= uvCmn_Luria_GetStartY(ENG_MDMD::en_pos_expo_start);
	/* Stage Y의 시작 부터 끝까지 거리 구하기 */
	i32EndStartPosY = i32StageUnloadY - INT32(m_dbAlignStageY * 10000.0f);

	/* Local Mark가 인식될 Trigger Position 값 등록 */

	if (!IsMarkTypeOnlyGlobal())
		LocalTrigRegist();

	/* Align Camera 2D 보정 여부 */
	if (!uvEng_GetConfig()->set_align.use_2d_cali_data)	return TRUE;
	/* 최종 Trigger가 발생되는 지점 (위치)의 X / Y 모션 좌표에 대한 측정 (수집)된 Calibration X 오차 값 반영 */
	return SetAlignACamCaliX();
}

/*
 desc : 현재 등록된 Trigger (Mark) 개수 즉, 각 Trigger 위치마다 Align Camera (X 축)의 위치 값 임시 저장
		4점 얼라인 마크 용
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::SetAlignACamCaliX()
{
	UINT8 i, j, n, k, u8MarkSet	= 0x00;
	INT32 i32OffsetX[2]			= { 0 }, i32OffsetY[2]	= { 0 };
	INT32 i32ScanMark			= 0;	/* 1 Scan 기준 - 저장된 Fiducial 개수 */
	DOUBLE dbPosACam[2]			= {NULL};
	DOUBLE dbMark2OrgGerbX		= uvEng_GetConfig()->set_align.mark2_org_gerb_xy[0];
	
	LPG_XMXY stMarkLocal = {nullptr};
	
	//STG_XMXY stMarkLocal[2] = { NULL };
	//STG_XMXY	stMarkGlobal	= {NULL};

	INT32 i32ACamVertX			= 0;	/* 단위: 0.1 um or 100 nm */
	LPG_CIEA pstConfig			= uvEng_GetConfig();
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	//LPG_RAAF pstRecipe			= uvEng_Mark_GetSelectAlignRecipe();
	// by sysandj : 변수없음(수정)
	LPG_MACP pstCaliThick		= uvEng_ThickCali_GetRecipe(pstRecipe->cali_thick);
	CAtlList <DOUBLE> lstCam1, lstCam2;

	/* ---------------------------------------------------------------------------- */
	/* [Global] Luria의 Fiducial 값에 넣어 줄때, 값에 부호를 설정하는데 상당히 중요 */
	/* ---------------------------------------------------------------------------- */
	/* 추후, Grabbed Image에서 측정된 오차 값에 해당 Calibration 값을 추가로 해줘야 최종 Mark 오차 값이 됨 */
	for (i=0; i<uvEng_Luria_GetMarkCount(ENG_AMTF::en_global); i++)
	{
		/* 얼라인 카메라의 마크 인식 방법에 따라 다르게 처리해 줘야 됨 */
		uvEng_ACamCali_AddMarkPos(i/2+1, ENG_AMTF::en_global, 0x00, i%2, m_dbPosACam[i/2]);
	}
	/* Align 측정 방식에 따라. 얼라인 카메라 X 오차 값 반영해야 할지 결졍 */
	// by sysandj : 변수없음(수정)
	 LPG_CSAI pstSetAlign = &uvEng_GetConfig()->set_align;
	//if (ENG_AOMI::en_fixed == ENG_AOMI(0/*pstRecipe->align_method*/))
	if (pstSetAlign->align_method != UINT8(ENG_AOMI::en_each))
	{
		/* Mark 1 (Left/Bottom)번 기준으로 했으므로, Mark1 번과 Mark 2번의 X 좌표 오차 값을 Mark 2의 X 값에 반영 */
		i32ACamVertX = (INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(2) * 10000.0, 0);
		/* 추후 Grabbed Image에 Mark 위치 별로 별도 오차 값을 적용 해야 함 */
		uvEng_ACamCali_SetAlignACamVertGX(1, 1, -i32ACamVertX);
		/* Mark 3 (Right/Bottom)번 기준으로 했으므로, Mark3 번과 Mark 4번의 X 좌표 오차 값을 Mark 4의 X 값에 반영 */
		i32ACamVertX = (INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(3) * 10000.0, 0);
		/* 추후 Grabbed Image에 Mark 위치 별로 별도 오차 값을 적용 해야 함 */
		uvEng_ACamCali_SetAlignACamVertGX(2, 1, -i32ACamVertX);
	}
	const int WANGBOK = 2;
	/* 현재 Local Mark가지 존재한다면 */
	if (!IsMarkTypeOnlyGlobal())
	{
		/* --------------------------------------------------------------------------- */
		/* [Local] Luria의 Fiducial 값에 넣어 줄때, 값에 부호를 설정하는데 상당히 중요 */
		/* --------------------------------------------------------------------------- */
		i32ScanMark = (INT32)GlobalVariables::getInstance()->GetAlignMotion().status.gerberRowCnt;  // uvEng_Luria_GetLocalMarkCountPerScan();
		auto columnCnt = (INT32)GlobalVariables::getInstance()->GetAlignMotion().status.gerberColCnt;
		int scanCnt =  columnCnt / WANGBOK;
		
		//stMarkLocal = new STG_XMXY[scanCnt];
		auto uniquePtrStMarkLocal = std::make_unique<STG_XMXY[]>(2);

		//if (!uvEng_Luria_GetGlobalMark(0x01, &stMarkGlobal))	return FALSE;	// Get the Left/Bottom Mark for Global
		/* Camera 1 & 2에 저장될 시작 배열 인덱스 값 구하기 */
		for (i=0,k=0,n=0; i< scanCnt; i++)	/* Scan 횟수 : 2회 에서 무제한으로 변경해야함.*/
		{
			/* Scan 1 or 2일 때, 각 카메라 별 Left or Right / Bottom의 X 축 기준으로 나머진 X 축들 간의 오차 값 얻기 */
			if (uvEng_Luria_GetLocalMarkDiffVertX(i, lstCam1, lstCam2))
			{
				/* Gerber에서 Local Fiducial 중 Camera 1 & 2번에 해당되는 Left or Right / Bottom의 Mark 정보 */
				if (!uvEng_Luria_GetLocalBottomMark(i, 1, &uniquePtrStMarkLocal[0]) ||
					!uvEng_Luria_GetLocalBottomMark(i, 2, &uniquePtrStMarkLocal[1]))
				{
					return FALSE;
				}
				/* Local Mark의 Left/Bottom 위치에 해당되는 1번 카메라의 모션 드라이브 실제 위치 얻기 */
				/* Global Left/Bottom 위치와 Local Left/Bottom 의 떨어진 간격으로 실제 모션 위치 얻기 */
				dbPosACam[0] = pstCaliThick->mark2_acam_x[0] + (uniquePtrStMarkLocal[0].mark_x - dbMark2OrgGerbX);	/* mm */
				/* Align Camera 1기준으로 Align Camera 1번과 2번 간의 기본 물리적인 거리도 감안하고,  거버의 마크 간에 떨어진 위치도 고려해야 함 */
				dbPosACam[1] = pstCaliThick->mark2_acam_x[1] + (uniquePtrStMarkLocal[1].mark_x - dbMark2OrgGerbX);	/* mm */
				/* 추후, Grabbed Image에서 측정된 오차 값에 해당 Calibration 값을 추가로 해줘야 최종 Mark 오차 값이 됨 */
				
				
				
				
				//수정중//
				// //수정중//
				// //수정중//
				// //수정중//
				// //수정중//
				// //수정중//
				// //수정중//
				// //수정중//
				// //수정중//
				// //수정중//
				// //수정중//
				//수정중//
				
				
				
				
				
				for (j=0; j<i32ScanMark; j++,n++) 
				{
					/* Offset 값 지정 - Camera 1 */
					uvEng_ACamCali_AddMarkPos(1, ENG_AMTF::en_local, 0x00, n, dbPosACam[0]);
					i32ACamVertX	-= (INT32)ROUNDED(lstCam1.GetAt(lstCam1.FindIndex(j)) * 10000.0f, 0);
					/* 추후 Grabbed Image에 Mark 위치 별로 별도 오차 값을 적용 해야 함 */
					uvEng_ACamCali_SetAlignACamVertLX(1, j, i32ACamVertX);

					/* Offset 값 지정 - Camera 2 */
					uvEng_ACamCali_AddMarkPos(2, ENG_AMTF::en_local, 0x00, n, dbPosACam[1]);
					i32ACamVertX	-= (INT32)ROUNDED(lstCam2.GetAt(lstCam2.FindIndex(j)) * 10000.0f, 0);
					/* 추후 Grabbed Image에 Mark 위치 별로 별도 오차 값을 적용 해야 함 */
					uvEng_ACamCali_SetAlignACamVertLX(2, j, i32ACamVertX);
				}
				/* 기존 등록된 임시 데이터 메모리 해제 */
				lstCam1.RemoveAll();
				lstCam2.RemoveAll();
			}
		}
	}

	/* 만약 Trigger Position에 대한 2D Calibration Data가 없다면 TRUE 반환 */
	if (!uvEng_ACamCali_IsCaliExistData())	return TRUE;
	/* Trigger 발생 위치에 해당되는 Stage Y와 Align Camera X 좌표에 대한 2D Calibration Data 계산 후 적용 */
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
 desc : 카메라 설치 기준으로, 렌즈 중심 간의 2개의 카메라 설치 간격 반환
 parm : dist_w	- [out] 2대의 얼라인 카메라 설치 간격 값 반환 (단위: 100 nm or 0.1 um)
		dist_h	- [out] 2대의 얼라인 카메라 설치 간격 값 반환 (단위: 100 nm or 0.1 um)
 retn : None
*/
VOID CWork::GetACamCentDistXY(INT32 &dist_x, INT32 &dist_y)
{
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	//LPG_RAAF pstRecipe		= uvEng_Mark_GetSelectAlignRecipe();
	// by sysandj : 변수없음(수정)
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
	// by sysandj : 변수없음(수정)
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
 desc : 현재 소재 두께 기준으로, Align Camera 1번의 Mark 2 (1 ~ 4)의 모션 위치 값 반환
 parm : mark_no	- [in]  Mark Number (0x00 ~ 0x03)
 retn : 상수 값 (0.1 um or 100 nm), 음수 값인 경우 실패
*/
INT32 CWork::GetACam1Mark2MotionX(UINT8 mark_no)
{
	INT32 i32Mark2ACamX		= 0;
	STG_XMXY stMarkPos		= {NULL};
	LPG_CSAI pstSetAlign	= &uvEng_GetConfig()->set_align;
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	//LPG_RAAF pstRecipe		= uvEng_Mark_GetSelectAlignRecipe();
	// by sysandj : 변수없음(수정)
	LPG_MACP pstCaliThick = uvEng_ThickCali_GetRecipe(pstRecipe->cali_thick);

	/* Calibration Thick Data 존재 여부 */
	if (!pstCaliThick)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"There is no correction data related to material thickness among the recipe properties.");
		return -1;
	}
	i32Mark2ACamX	= (INT32)ROUNDED(pstCaliThick->mark2_acam_x[0] * 10000.0f, 0);	/* Align Camera 1 번 기준 */

	/* 현재 적재된 거버의 Mark ? 번(Left/Bottom)에 대한 X, Y 좌표 값 가져오기 */
	if (!uvEng_Luria_GetGlobalMark(mark_no, &stMarkPos))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to get the location information of global mark");
		return -1;
	}

	return	(i32Mark2ACamX - (INT32)ROUNDED(pstSetAlign->mark2_org_gerb_xy[0] * 10000.0f, 0)) + 
			(INT32)ROUNDED(stMarkPos.mark_x * 10000.0f, 0);
}

/*
 desc : Chiller 동작 확인
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsChillerRunning()
{
	/* 칠러가 동작 중인지 확인 */
	if (!uvEng_GetConfig()->IsCheckChiller())	return TRUE;
	// by sysandj : MCQ대체 추가 필요
	return FALSE;//return uvEng_ShMem_GetPLCExt()->r_chiller_controller_running == 1 ? TRUE : FALSE;
}

/*
 desc : Vacuum 동작 확인
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsVacuumRunning()
{
	/* 베큘이 동작 중인지 확인 */
	if (!uvEng_GetConfig()->IsCheckVacuum())	return TRUE;
	// by sysandj : MCQ대체 추가 필요
// 	return (uvEng_ShMem_GetPLCExt()->r_vacuum_controller_running == 1 && 
// 			uvEng_ShMem_GetPLCExt()->r_vacuum_status == 1) ? TRUE : FALSE;
	return FALSE;
}

/*
 desc : Align 측정 방법에 따라, 캡처된 Global Mark의 이미지 위치 정보 반환
 parm : mark_no	- [in]  Mark Number (0x00 ~ 0x03)
		cam_id	- [out] Grabbed Image가 저장된 카메라의 ID (0x01 or 0x02)
		img_id	- [out] Grabbed Image의 ID (0x00 ~ 0x??)
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
		// by sysandj : 변수없음(수정)
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
 desc : 현재 선택된 레시피 내에 등록된 Gerber가 존재하는지 여부
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsRecipeGerberCheck()
{
	CHAR szGerbFile[MAX_PATH_LEN]	= {NULL};
	LPG_RJAF pstRecipe	= uvEng_JobRecipe_GetSelectRecipe();
	CUniToChar csCnv;

	/* 현재 선택된 레시피가 있는지 여부 */
	if (!pstRecipe)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"The selected gerber recipe does not exist");
		return FALSE;
	}

	/* 레시피에 포함된 거버의 정보가 로컬 드라이브에 있는지 여부 확인 */
	sprintf_s(szGerbFile, MAX_PATH_LEN, "%s\\%s", pstRecipe->gerber_path, pstRecipe->gerber_name);
	if (!uvCmn_FindPath(csCnv.Ansi2Uni(szGerbFile)))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Gerber file does not exist");
		return FALSE;
	}

	return TRUE;
}

/*
 desc : 현재 거버 마크 타입이 Global인지 Global & Local 혼합 방식인지 여부
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
 desc : Mark 번호 기준으로 Align Camera X와 Stage Y의 모션 위치 얻기
 parm : mark_no	- [in]  마크 인덱스 (0x00 ~ 0x03) (Only Global Mark : 4)
		acam_x	- [out] 얼라인 카메라의 모션 위치 반환 (단위: 100nm or 0.1 um)
		stage_y	- [out] 스테이지Y의 모션 위치 반환 (단위: 100nm or 0.1 um)
 retn : None
*/
VOID CWork::GetMovePosGlobalMark(UINT8 mark_no, INT32 &acam_x, INT32 &stage_y)
{
	UINT8 u8Mark	= uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	INT32 i32ACamDist, i32ACamHeight, i32ACamPos1;
	INT32 i32DiffMark12, i32DiffMark13, i32DiffMark24;

	/* 값 초기화 */
	acam_x	= stage_y	= 0;
	/* 카메라간 떨어진 물리적인 거리 확인 */
	GetACamCentDistXY(i32ACamDist, i32ACamHeight);
	/* Stage Y 이동 좌표 얻기 (레시피에 설정된 Y 축 추가 이동만큼 더해줘야 됨) */
	stage_y	= GetGlobalMarkMotionPosY(mark_no);
	/* Mark 위치에 따라, 카메라 간의 높이 차이 만큼 다르게 함 */
	if (0x04 == u8Mark)
	{
		/* 1번과 3번 마크 간의 간격 (넓이; 오차) 얻기 */
		i32DiffMark13 = (INT32)ROUNDED(abs(uvEng_Luria_GetGlobalMarkDiffX(1, 3)) * 10000.0f, 0);	/* 100 nm or 0.1 um */
		/* 2번과 4번 마크 간의 간격 (넓이; 오차) 얻기 */
		i32DiffMark24 = (INT32)ROUNDED(abs(uvEng_Luria_GetGlobalMarkDiffX(2, 4)) * 10000.0f, 0);	/* 100 nm or 0.1 um */
		switch (mark_no)
		{
		case 0x02	:
		case 0x03	:	stage_y	-= i32ACamHeight;	break;
		}
		/* 얼라인 Mark 2번에 해당되는 카메라 1번의 X 축 실제 모션 위치 */
		switch (mark_no)
		{
		case 0x00 : case 0x01 : i32ACamPos1	= GetACam1Mark2MotionX(mark_no);		break;
		case 0x02 : case 0x03 : i32ACamPos1	= GetACam1Mark2MotionX(mark_no-0x02);	break;
		}
		/* Mark 위치에 따라, 카메라 위치 다르게 함 */
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
		/* 1번과 2번 마크 간의 간격 (넓이; 오차) 얻기 */
		i32DiffMark12	= (INT32)ROUNDED(abs((INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(2) * 10000.0f, )), 0);	/* 100 nm or 0.1 um */
		if (0x01 == mark_no)	stage_y		-= i32ACamHeight;
		/* 얼라인 Mark 2번에 해당되는 카메라 1번의 X 축 실제 모션 위치 */
		if (0x00 == mark_no)	i32ACamPos1	= GetACam1Mark2MotionX(mark_no);
		else					i32ACamPos1	= GetACam1Mark2MotionX(mark_no-0x01);
		/* Mark 위치에 따라, 카메라 위치 다르게 함 */
		if (0x00 == mark_no)	acam_x	= i32ACamPos1;
		else					acam_x	= i32ACamPos1 + i32DiffMark12 - i32ACamDist;
	}
}
#endif
/*
 desc : Camera 1번 기준으로 Mark 번호에 대한 Align Camera X와 Stage Y의 모션 위치 얻기
 parm : mark_no	- [in]  마크 인덱스 (0x00 ~ 0x04?) (Only Global Mark : 2 or 4)
		acam_x	- [out] 얼라인 카메라의 모션 위치 반환 (단위: 100nm or 0.1 um)
		stage_y	- [out] 스테이지Y의 모션 위치 반환 (단위: 100nm or 0.1 um)
 retn : None
*/
VOID CWork::GetMovePosGlobalMarkACam1(UINT8 mark_no, INT32 &acam_x, INT32 &stage_y)
{
	UINT8 u8Mark	= uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	UINT16 u16MarkType	= uvEng_Luria_GetGlobalMark3PType();
	INT32 i32ACamPos1;
	INT32 i32DiffMark1, i32DiffMark2;

	/* 값 초기화 */
	acam_x	= stage_y	= 0;
	/* Stage Y 이동 좌표 얻기 (레시피에 설정된 Y 축 추가 이동만큼 더해줘야 됨) */
	stage_y	= GetGlobalMarkMotionPosY(mark_no);
	/* Mark 위치에 따라, 카메라 간의 높이 차이 만큼 다르게 함 */
	if (0x04 == u8Mark)
	{
		/* 1번과 3번 마크 간의 간격 (넓이; 오차) 얻기 */
		i32DiffMark1	= abs((INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(0) * 10000.0f, 0));	/* 100 nm or 0.1 um */
		/* 2번과 4번 마크 간의 간격 (넓이; 오차) 얻기 */
		i32DiffMark2	= abs((INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(1) * 10000.0f, 0));	/* 100 nm or 0.1 um */
		/* 얼라인 Mark 2번에 해당되는 카메라 1번의 X 축 실제 모션 위치 */
		switch (mark_no)
		{
		case 0x00 : case 0x01 : i32ACamPos1	= GetACam1Mark2MotionX(mark_no);			break;
		case 0x02 : case 0x03 : i32ACamPos1	= GetACam1Mark2MotionX(mark_no-0x02);	break;
		}
		/* Mark 위치에 따라, 카메라 위치 다르게 함 */
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
		/* 1번과 2번 마크 간의 간격 (넓이; 오차) 얻기 */
		i32DiffMark1	= abs((INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(2) * 10000.0f, 0));	/* 100 nm or 0.1 um */
		/* 얼라인 Mark 2번에 해당되는 카메라 1번의 X 축 실제 모션 위치 */
		if (0x00 == mark_no)	i32ACamPos1	= GetACam1Mark2MotionX(mark_no);
		else					i32ACamPos1	= GetACam1Mark2MotionX(mark_no-0x01);
		/* Mark 위치에 따라, 카메라 위치 다르게 함 */
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
				case 0x02	:	acam_x	= i32ACamPos1 + abs((INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(5) * 10000.0f, 0));	break;	/* 1번과 2번 마크 간의 간격 (넓이; 오차) 얻기 */
				case 0x03	:	acam_x	= i32ACamPos1 + abs((INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(1) * 10000.0f, 0));	break;	/* 1번과 3번 마크 간의 간격 (넓이; 오차) 얻기 */
				}
				break;
			case 0x1011	:
				switch (mark_no)
				{
				case 0x02	:	acam_x	= i32ACamPos1 + abs((INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(0) * 10000.0f, 0));	break;	/* 1번과 2번 마크 간의 간격 (넓이; 오차) 얻기 */
				case 0x03	:	acam_x	= i32ACamPos1 + abs((INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(4) * 10000.0f, 0));	break;	/* 1번과 3번 마크 간의 간격 (넓이; 오차) 얻기 */
				}
				break;
			case 0x1101	:	acam_x	= i32ACamPos1 + abs((INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(1) * 10000.0f, 0));	break;	/* 1번과 3번 마크 간의 간격 (넓이; 오차) 얻기 */
			case 0x1110	:	acam_x	= i32ACamPos1 + abs((INT32)ROUNDED(uvEng_Luria_GetGlobalMarkDiffVertX(5) * 10000.0f, 0));	break;	/* 1번과 3번 마크 간의 간격 (넓이; 오차) 얻기 */
			}
		}
	}
}

/*
 desc : 노광할 준비가 되어 있도록 각종 장비 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::ResetExpoReady()
{
	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Init.Processing");

	/* 일단 Trigger 기능을 Disable 시킴 */
	if (!uvEng_Mvenc_ReqTriggerStrobe(FALSE))	return FALSE;
	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* 현재 모든 photohead의 모터 상태 정보 요청 (일단. Z Axis이 Mid 상태가 체크 되어 있는지 여부) */
		if (!uvEng_Luria_ReqGetMotorStateAll())	return FALSE;
	}
	/* 모션 드라이브 정지 유무 확인 */
	if (!uvEng_MC2_SendDevStoppedAll())			return FALSE;
	/* Vacuum가 Off 상태가 아닌지 여부 확인 */
	// by sysandj : MCQ대체 추가 필요 : if (!uvEng_MCQ_VacuumRunStop(0x01))			return FALSE;
	/* Chiller가 On 상태가 아닌지 여부 확인 */
	// by sysandj : MCQ대체 추가 필요 : if (!uvEng_MCQ_ChillerRunStop(0x01))		return FALSE;

	return TRUE;
}

/*
 desc : 모든 카메라 (채널)의 트리거 위치가 초기화 되었는지 여부
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
 desc : 현재 작업이 중지된 상태이지 여부
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
 desc : 가장 최근의 Scenario가 실패한 경우인지 여부
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWork::IsWorkError()
{
	return (m_enWorkState == ENG_JWNS::en_error) || 
		   (m_enWorkState == ENG_JWNS::en_time);
}

/*
 desc : 기준 Mark 2 번 기준으로, 현재 Mark 번호가 이동해야 할 카메라 X 위치 반환
 parm : mark_no	- [in]  Mark Number (0x01 or Later)
 retn : 상수 값 (mm)
*/
DOUBLE CWork::GetACamMark2MotionX(UINT16 mark_no)
{
	//UINT8 u8ACamCount	= uvEng_GetConfig()->set_cams.acam_count;
	UINT8 u8ACamCount = 2;
	UINT8 u8MarkCount	= uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);	/* Global Mark 개수 */
	INT32 i32Mark2ACamX	= 0;
	DOUBLE dbMark2ACamX	= 0.0f;
	STG_XMXY stMarkPos	= {NULL};
	LPG_CSAI pstAlign	= &uvEng_GetConfig()->set_align;
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	//LPG_RAAF pstRecipe	= uvEng_Mark_GetSelectAlignRecipe();	/* 현재 선택된 거버 레시피 */
	// by sysandj : 변수없음(수정)
	LPG_MACP pstThick	= uvEng_ThickCali_GetRecipe(pstRecipe->cali_thick);

	/* Thick Calibration 존재 여부 */
	if (!pstThick)
	{
		AfxMessageBox(L"Failed to find the calibration file for Thick.Cali", MB_ICONSTOP|MB_TOPMOST);
		return 0.0f;
	}

	/* Mark Number의 경우 1 based */
	if (mark_no < 0x01)	return 0.0f;
	/* 현재 적재된 거버의 Mark ? 번(Left/Bottom)에 대한 X, Y 좌표 값 가져오기 */
	if (!uvEng_Luria_GetGlobalMark(mark_no-1, &stMarkPos))	return 0.0f;

	/* 얼라인 카메라가 2개인 경우 */
	dbMark2ACamX	= pstThick->mark2_acam_x[0];	/*pstAlign->mark2_acam_x[0];*/
	/* Global Mark 번호가 2 이상인 경우 경우 */
	if (mark_no > (u8MarkCount/u8ACamCount))
	{
		/* Align Camera 2번의 Mark 2번의 모션 기준 */
		dbMark2ACamX	= pstThick->mark2_acam_x[1];	/*pstAlign->mark2_acam_x[1];*/
	}

	return	(dbMark2ACamX - pstAlign->mark2_org_gerb_xy[0]) + stMarkPos.mark_x;
}

/*
 desc : 현재 적재된 거버의 임의 Mark Number에 대해, 이동하고자 하는 모션의 Stage X / Y 위치 반환
 parm : mark_no	- [in]  Global Mark Number (1 or Later)
		acam_x	- [out] Motion 이동 위치 반환 (X : mm)
		stage_y	- [out] Motion 이동 위치 반환 (Y : mm)
 retn : TRUE or FALSE
*/
BOOL CWork::GetGlobalMarkMoveXY(UINT16 mark_no, DOUBLE &acam_x, DOUBLE &stage_y)
{
	UINT8 u8Mark		= uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	DOUBLE dbACamDistX	= 0.0f, dbACamDistY = 0.0f;	/* 2 대 이상의 Align Camera 일 경우, 각 Camera 간의 설치 오차 값 (단위: mm) */
	STG_XMXY stMarkPos	= {NULL};	/* mark_no에 대한 좌표 위치 얻기 */
	LPG_CSAI pstAlign	= &uvEng_GetConfig()->set_align;
	LPG_RJAF pstRecipe	= NULL;
	LPG_MACP pstThick	= NULL;

	pstRecipe	= uvEng_JobRecipe_GetSelectRecipe();	/* 현재 선택된 거버 레시피 */
	if (!pstRecipe)
	{
		AfxMessageBox(L"No recipes are selected [Gerber]", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	// by sysandj : 변수없음(수정)
	pstThick = uvEng_ThickCali_GetRecipe(pstRecipe->cali_thick);
	if (!pstThick)
	{
		AfxMessageBox(L"No recipes are selected [Thick Cali]", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* 만약 Mark 개수가 4개 아니면 에러 처리 */
	if (u8Mark != 4)
	{
		AfxMessageBox(L"Invalid number of Global Marks.", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 현재 거버에 등록된 Mark Number에 대한 좌표 위치 얻기 */
	if (!uvEng_Luria_GetGlobalMark(mark_no-1, &stMarkPos))	return FALSE;

	/* 일단, 얼라인 카메라 1번에 대한 1 / 2 번 마크의 Y 축 모션 좌표 값 계산 */
	stage_y	= pstThick->mark2_stage_y[0];
	/* 검색 대상 마크에서 기준 마크 2번의 Y 좌표 오차 값 구함 */

	STG_XMXY currGbrMark2;
	uvEng_Luria_GetGlobalMark(1, &currGbrMark2);

	stage_y	+= (stMarkPos.mark_y - currGbrMark2.mark_y) + (currGbrMark2.mark_y - pstAlign->mark2_org_gerb_xy[1]);
	/* 만약 얼라인 카메라 2번일 경우, 3 / 4 번 마크의 Y 축 모션 좌표 값 계산 */
	// 여기도 -- 면 +라서 변경함.
	if (mark_no > 0x02)	stage_y	+= pstThick->GetACamDistY();

	/* Align Camera X 좌표 계산 */
	acam_x	= GetACamMark2MotionX(mark_no);

	return TRUE;
}