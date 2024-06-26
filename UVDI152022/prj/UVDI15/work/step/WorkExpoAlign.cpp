
/*
 desc : Align 노광 진행 - 얼라인 카메라 X 축 이동 없이 스테이지 Y 축만 후진
*/

#include "pch.h"
#include "../../MainApp.h"
#include "WorkExpoAlign.h"
#include "../../mesg/DlgMesg.h"
#include "../../GlobalVariables.h"

#include <string>

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif

//extern CACamCali* g_pACamCali;

/*
 desc : 생성자
 parm : None
 retn : None
*/
CWorkExpoAlign::CWorkExpoAlign()
	: CWorkStep()
{
	m_enWorkJobID	= ENG_BWOK::en_expo_align;
	m_u32ExpoCount = 1;

	m_stExpoLog.Init();
	//memcpy(&m_stExpoLog, expo, sizeof(STG_CELA));
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CWorkExpoAlign::~CWorkExpoAlign()
{
}

/*
 desc : 초기 작업 수행
 parm : None
 retn : None
*/
BOOL CWorkExpoAlign::InitWork()
{
	/* 내부 멤버 변수 값 초기화 */
	if (!CWork::InitWork())	return FALSE;

	 globalMarkCnt = uvEng_Luria_GetMarkCount(ENG_AMTF::en_global);
	 localMarkCnt = uvEng_Luria_GetMarkCount(ENG_AMTF::en_local);

	 m_u8MarkCount = globalMarkCnt + (IsMarkTypeOnlyGlobal() == true ? 0 : localMarkCnt);


	 SetAlignMode();

	return TRUE;
}

/*
 desc : 주기적으로 작업 수행
 parm : None
 retn : None
*/
VOID CWorkExpoAlign::DoWork()
{
	const int Initstep = 0, processWork = 1, checkWorkstep = 2;
	try
	{
		alignCallback[alignMotion][processWork]();
		alignCallback[alignMotion][checkWorkstep]();
		CheckWorkTimeout();
	}
	catch (const std::exception&)
	{

	}
}


void CWorkExpoAlign::SetAlignMode()
{
	auto& motion = GlobalVariables::GetInstance()->GetAlignMotion();
	this->alignMotion = motion.markParams.alignMotion;
	this->aligntype = motion.markParams.alignType;
	const int INIT_STEP = 0;
	alignCallback[alignMotion][INIT_STEP]();
	
}

/*
 desc : 노광 완료 후 각종 정보 저장
 parm : state	- [in]  0x00: 작업 실패, 0x01: 작업 성공
 retn : None
*/
VOID CWorkExpoAlign::SaveExpoResult(UINT8 state)
{
	//UINT8 i;
	TCHAR tzResult[1024]= {NULL}, tzFile[MAX_PATH_LEN] = {NULL}, tzState[2][8] = {L"FAIL", L"SUCC"};
	TCHAR tzDrv[8]		= {NULL};
	//UINT64 u64Temp[2]	= {NULL};
	//UINT16 (*pLed)[8]	= uvEng_ShMem_GetLuria()->directph.light_source_driver_temp_led;
	//UINT16 (*pBoard)[8]	= uvEng_ShMem_GetLuria()->directph.light_source_driver_temp_board;
	SYSTEMTIME stTm		= {NULL};
	MEMORYSTATUSEX stMem= {NULL};
	LPG_ACGR pstMark	= NULL;
	LPG_RJAF pstRecipe	= uvEng_JobRecipe_GetSelectRecipe();
	CUniToChar csCnv1, csCnv2;

	CUniToChar	csCnv;
	LPG_RAAF pstAlignRecipe = uvEng_Mark_GetAlignRecipeName(csCnv.Ansi2Uni(pstRecipe->align_recipe));
	LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstRecipe->expo_recipe));

	/*레시피 정보 가져오기*/
	LPG_RJAF pstJobRecipe = uvEng_JobRecipe_GetSelectRecipe();

	/* 현재 컴퓨터 날짜를 파일명으로 설정 */
	GetLocalTime(&stTm);
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\logs\\expo\\%04d-%02d-%02d.csv",
			   g_tzWorkDir, stTm.wYear, stTm.wMonth, stTm.wDay);

	/* 만약에 열고자 하는 파일이 없으면, 해당 파일 생성 후, 타이틀 (Field) 추가 */
	if (!uvCmn_FindFile(tzFile))
	{
		wcscpy_s(tzResult, 1024,
				 //L"date,recipe,gerber,succ,hot_air,temp_di_1,temp_di_2,temp_di_3,temp_di_4,"
				 //L"hdd_free(GB),mem_free(GB),"
				 //L"led(1:1),led(1:2),led(1:3),led(1:4),board(1:1),board(1:2),board(1:3),board(1:4),"
				 //L"led(2:1),led(2:2),led(2:3),led(2:4),board(2:1),board(2:2),board(2:3),board(2:4),"
				 //L"horz_dist_13(mm),horz_dist_24(mm),vert_dist_12(mm),vert_dist_34(mm),"
				 //L"diag_dist_14(mm),diag_dist_23(mm),expo_time(ms),"
				 //L"score_1,scale_1,scale_size_1,mark_move_x1(mm),mark_move_y1(mm),"
				 //L"score_2,scale_2,scale_size_2,mark_move_x2(mm),mark_move_y2(mm),"
				 //L"score_3,scale_3,scale_size_3,mark_move_x3(mm),mark_move_y3(mm),"
				 //L"score_4,scale_4,scale_size_4,mark_move_x4(mm),mark_move_y4(mm),\n");
			L"date,tack,succ,"
			L"gerber_name,material_thick(um),expo_eneray(mj),"
			L"horz_dist_13(mm),horz_dist_24(mm),vert_dist_12(mm),vert_dist_34(mm),"
			L"diag_dist_14(mm),diag_dist_23(mm),"
			L"score_1,scale_1,scale_size_1,mark_move_x1(mm),mark_move_y1(mm),"
			L"score_2,scale_2,scale_size_2,mark_move_x2(mm),mark_move_y2(mm),"
			L"score_3,scale_3,scale_size_3,mark_move_x3(mm),mark_move_y3(mm),"
			L"score_4,scale_4,scale_size_4,mark_move_x4(mm),mark_move_y4(mm),\n");
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x00);
	}
	/* 발생 시간 */
	swprintf_s(tzResult, 1024, L"%02d:%02d:%02d,", stTm.wHour, stTm.wMinute, stTm.wSecond);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	/*ExpoLog 기록*/
	memcpy(m_stExpoLog.data, tzResult, 40);


	/* 온도 / 노광 시간 / 노광 성공 / 노광 모드 / 소재 배율 / 소재 회전 */
	swprintf_s(tzResult, 1024, L"\%llu,%s,%.2f,%.2f,",
		uvEng_GetJobWorkTime(), tzState[state], pstExpoRecipe->real_scale_range, pstExpoRecipe->real_rotaion_range);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	/*ExpoLog 기록*/
	m_stExpoLog.expo_time = uvEng_GetJobWorkTime();
	m_stExpoLog.expo_succ = state;
	m_stExpoLog.real_scale = pstExpoRecipe->real_scale_range;
	m_stExpoLog.real_rotaion = pstExpoRecipe->real_rotaion_range;


	/* 마크 간의 6 곳 길이 측정 오차 값 과 전체 노광하는데 소요된 시간 저장 */
	swprintf_s(tzResult, 1024, L"%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,",
		uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_top_horz),
		uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_btm_horz),
		uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_lft_vert),
		uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_rgt_vert),
		uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_lft_diag),
		uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_rgt_diag));
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	/*ExpoLog 기록*/
	m_stExpoLog.global_dist[0] = uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_top_horz);
	m_stExpoLog.global_dist[1] = uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_btm_horz);
	m_stExpoLog.global_dist[2] = uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_lft_vert);
	m_stExpoLog.global_dist[3] = uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_rgt_vert);
	m_stExpoLog.global_dist[4] = uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_lft_diag);
	m_stExpoLog.global_dist[5] = uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_rgt_diag);


	/*ExpoLog 기록*/
	//memcpy(m_stExpoLog.gerber_name, pstJobRecipe->gerber_name, MAX_GERBER_NAME);
	strcpy_s(m_stExpoLog.gerber_name, MAX_GERBER_NAME, pstJobRecipe->gerber_name);
	m_stExpoLog.material_thick = pstJobRecipe->material_thick;
	m_stExpoLog.expo_energy = pstJobRecipe->expo_energy;

	m_stExpoLog.align_type = pstAlignRecipe->align_type;
#ifdef USE_ALIGNMOTION
	m_stExpoLog.align_motion = pstAlignRecipe->align_motion;
#endif
	m_stExpoLog.mark_type = pstAlignRecipe->mark_type;
	m_stExpoLog.lamp_type = pstAlignRecipe->lamp_type;
	m_stExpoLog.gain_level[0] = pstAlignRecipe->gain_level[0];
	m_stExpoLog.gain_level[1] = pstAlignRecipe->gain_level[1];

	m_stExpoLog.led_duty_cycle = pstExpoRecipe->led_duty_cycle;
	//sprintf_s(m_stExpoLog.power_name, LED_POWER_NAME_LENGTH, pstExpoRecipe->power_name);
	strcpy_s(m_stExpoLog.power_name, LED_POWER_NAME_LENGTH, pstExpoRecipe->power_name);

	/* 마지막엔 무조건 다음 라인으로 넘어가도록 하기 위함 */
	uvCmn_SaveTxtFileW(L"\n", (UINT32)wcslen(L"\n"), tzFile, 0x01);


}


void CWorkExpoAlign::DoInitOnthefly2cam()
{
	m_u8StepTotal = 0x24;
}

void CWorkExpoAlign::DoAlignOnthefly2cam()
{
	auto& motions = GlobalVariables::GetInstance()->GetAlignMotion();
	switch (m_u8StepIt)/* 작업 단계 별로 동작 처리 */
	{
	case 0x01: 
	{
		m_enWorkState = SetExposeReady(TRUE, TRUE, TRUE, 1);

		if (m_enWorkState == ENG_JWNS::en_next)
		{
			offsetPool.clear();
			motions.SetFiducialPool();
		}
	}
	break;
	case 0x02: m_enWorkState = IsLoadedGerberCheck();						break;	/* 거버가 적재되었고, Mark가 존재하는지 확인 */
	case 0x03: m_enWorkState = SetTrigEnable(FALSE);						break;	/* Trigger Event - 비활성화 설정 */
	case 0x04: m_enWorkState = IsTrigEnabled(FALSE);						break;	/* Trigger Event - 빌활성화 확인  */
	case 0x05: 
	{
		m_enWorkState = SetAlignMovingInit();
	}
	break;	/* Stage X/Y, Camera 1/2 - Align (Global) 시작 위치로 이동 */

	case 0x06: m_enWorkState = SetTrigPosCalcSaved();						break;	/* Trigger 발생 위치 계산 및 임시 저장 */
	case 0x07: m_enWorkState = IsAlignMovedInit();							break;	/* Stage X/Y, Camera 1/2 - Align (Global) 시작 위치 도착 여부 */
	case 0x08: m_enWorkState = SetTrigRegistGlobal();						break;	/* Trigger 발생 위치 - 트리거 보드에 Global Mark 위치 등록 */
	case 0x09: m_enWorkState = IsTrigRegistGlobal();						break;	/* Trigger 발생 위치 등록 확인 */
	case 0x0a: m_enWorkState = SetAlignMeasMode();							break;
	case 0x0b: m_enWorkState = IsAlignMeasMode();							break;
	case 0x0c: m_enWorkState = SetAlignMovingGlobal();						break;	/* Global Mark 4 군데 위치 확인 */
	case 0x0d: m_enWorkState = IsAlignMovedGlobal();						break;	/* Global Mark 4 군데 측정 완료 여부 */
	case 0x0e:
	{
		//여기까지 왔으면 로컬얼라인이 있는것. 먼저 글로벌이 몇장찍혔나 확인해야함.
		CameraSetCamMode(ENG_VCCM::en_none);
		
		m_enWorkState = SetAlignMovingLocal((UINT8)AlignMotionMode::toInitialMoving, scanCount);	break;	/* Stage X/Y, Camera 1/2 - Align (Local:역방향) 시작 위치로 이동 */
	}

	case 0x0f: m_enWorkState = SetTrigRegistLocal(scanCount);										break;	/* Trigger (역방향) 발생 위치 - 트리거 보드에 Local Mark 위치 등록 */
	case 0x10: m_enWorkState = IsTrigRegistLocal(scanCount);										break;	/* Trigger (역방향) 발생 위치 등록 확인 */
	case 0x11:
	{
		m_enWorkState = IsAlignMovedLocal((UINT8)AlignMotionMode::toInitialMoving, scanCount);
		
	}
	break;	/* Stage X/Y, Camera 1/2 - Align (Local) 시작 위치 도착 여부 */

	case 0x12:
	{
		m_enWorkState = CameraSetCamMode(ENG_VCCM::en_grab_mode);
		
	}
	break;	/* Cam None 모드로 변경 */

	case 0x13: m_enWorkState = SetAlignMovingLocal((UINT8)AlignMotionMode::toScanMoving, scanCount);		break;	/* Local Mark (역방향) 16 군데 위치 확인 */
	case 0x14: m_enWorkState = IsAlignMovedLocal((UINT8)AlignMotionMode::toScanMoving, scanCount);		break;	/* Local Mark (역방향) 16 군데 측정 완료 여부 */

	case 0x15:
		this_thread::sleep_for(chrono::milliseconds(200)); // 잠깐 기다려주는 이유가 바슬러 스레드에서 캠 데이터를 가져가는 스레드 리프레시 타임이 있기때문.
		m_u8StepIt = GlobalVariables::GetInstance()->GetAlignMotion().CheckAlignScanFinished(++scanCount) ? 0x16 : 0x0e; //남아있으면 다시 올라감. 
		m_enWorkState = ENG_JWNS::en_forceSet;
		break;

	case 0x16:
		m_enWorkState = CameraSetCamMode(ENG_VCCM::en_none);break;	/* Cam None 모드로 변경 */

	case 0x17: m_enWorkState = SetTrigEnable(FALSE);						break;
	case 0x18:
	{
		SetUIRefresh(true);
		m_enWorkState = IsGrabbedImageCount(m_u8MarkCount, 3000);
	}
	break;
	case 0x19:
	{
		
		m_enWorkState = IsSetMarkValidAll(0x01);
	}
	break;

	case 0x1a:m_enWorkState = SetAlignMarkRegist();break;
	case 0x1b:m_enWorkState = IsAlignMarkRegist();break;
	case 0x1c: m_enWorkState = IsTrigEnabled(FALSE);						break;

	case 0x1d: m_enWorkState = SetPrePrinting();							break;	/* Luria Control - PrePrinting */
	case 0x1e: m_enWorkState = IsPrePrinted();								break;	/* Luria Control - PrePrinted 확인 */
	
	case 0x1f:
	{
		m_enWorkState = SetPrinting();
	}
	break;	/* Luria Control - Printing */
	case 0x20: m_enWorkState = IsPrinted();								break;	/* Luria Control - Printed 확인 */

	case 0x21: m_enWorkState = SetWorkWaitTime(1000);						break;	/* 일정 시간 대기 */
	case 0x22: m_enWorkState = IsWorkWaitTime();							break;	/* 대기 완료 여부 확인 */

	case 0x23: m_enWorkState = SetMovingUnloader();						break;	/* Stage Unloader 위치로 이동 */
	case 0x24: m_enWorkState = IsMovedUnloader();							break;	/* Stage Unloader 위치에 도착 했는지 여부 확인 */


	}
	
	
}

void CWorkExpoAlign::SetWorkNextOnthefly2cam()
{
	UINT64 u64TickCount = GetTickCount64();
	UINT64 u64JobTime = u64TickCount - m_u64StartTime;

	/* 매 작업 구간마다 시간 값 증가 처리 */
	uvEng_UpdateJobWorkTime(u64JobTime);

	/* 모든 작업이 종료 되었는지 여부 */
	if (ENG_JWNS::en_error == m_enWorkState)
	{
		SaveExpoResult(0x00);
		m_u8StepIt = 0x01;

		/*노광 종료가 되면 Philhmil에 완료보고*/
		SetPhilProcessCompelet();

		TCHAR tzMesg[128] = { NULL };
		swprintf_s(tzMesg, 128, L"Work Expo Align <Error Step It = 0x%02x>", m_u8StepIt);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
		m_enWorkState = ENG_JWNS::en_comp;
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
		m_enWorkState = ENG_JWNS::en_error;
#endif

	}
	else if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* 작업률 계산 후 임시 저장 */
		CWork::CalcStepRate();

		/* 필요에 따라 분기 처리 */
		switch (m_u8StepIt)
		{
			/* Local Mark 존재 여부 확인 */
		case 0x0d:	if (!uvEng_Luria_IsMarkLocal())	m_u8StepIt = 0x19;	break;
			/* 노광 작업이 완료된 이후, 바로 광학계 내 온도 값 요청 */
		case 0x24: uvEng_Luria_ReqGetPhLedTempAll();	break;
		}

		/* 모든 동작이 완료되었는지 확인 */
		if (m_u8StepTotal == m_u8StepIt)
		{
			/* 작업 완료 후 각종 필요한 정보 저장 */
			SaveExpoResult(0x01);

			//if (++m_u32ExpoCount != 10000)
			if (++m_u32ExpoCount != 2)
			{
				m_u8StepIt = 0x01;

#ifdef _DEBUG
				TCHAR szMesg[LOG_MESG_SIZE] = { NULL };

				swprintf_s(szMesg, LOG_MESG_SIZE, L"Expo Only Step :  m_u32ExpoCount= %d / m_u8StepTotal = 10000\n"
					, m_u32ExpoCount);

				/*Log 기록*/
				m_strLog.Format(szMesg);
				txtWrite(m_strLog);
#endif


				/*Auto Mdoe로 노광 종료가 되면 Philhmil에 완료보고*/
				//if (g_u8Romote == en_menu_phil_mode_auto)
				if (g_u16PhilCommand == (int)ENG_PHPC::ePHILHMI_C2P_PROCESS_EXECUTE)
				{
					SetPhilProcessCompelet();
				}
			}
			else
			{
				/*Auto Mdoe로 노광 종료가 되면 Philhmil에 완료보고*/
				if (g_u16PhilCommand == (int)ENG_PHPC::ePHILHMI_C2P_PROCESS_EXECUTE)
				{
					SetPhilProcessCompelet();
				}


				m_enWorkState = ENG_JWNS::en_comp;

				/* 항상 호출*/
				CWork::EndWork();
			}

		}
		else
		{
			/* 다음 작업 단계로 이동 */
			m_u8StepIt++;
		}
		/* Align Mark를 정상 인식 후, 노광을 진행할 것인지 여부 물어보기 */
		if (m_u8StepIt == 0x1f && uvEng_GetConfig()->set_uvdi15.check_query_expo)
		{
			CDlgMesg dlgMesg;
			if (IDOK != dlgMesg.MyDoModal(L"Do you really want to expose?", 0x02))
			{
				m_u8StepIt = 0x00;
				m_enWorkState = ENG_JWNS::en_comp;
			}
		}
		/* 가장 최근에 Waiting 한 시간 저장 */
		m_u64DelayTime = GetTickCount64();
	}
}
	 
void CWorkExpoAlign::DoInitOnthefly3cam()
{

}

void CWorkExpoAlign::DoAlignOnthefly3cam()
{

}

void CWorkExpoAlign::SetWorkNextOnthefly3cam()
{

}
	 
void CWorkExpoAlign::DoInitStatic2cam()
{

}

void CWorkExpoAlign::DoAlignStatic2cam()
{

}

void CWorkExpoAlign::SetWorkNextStatic2cam()
{

}
	 
void CWorkExpoAlign::DoInitStaticCam()
{

	m_u8StepTotal = 0x14;

	
}

void CWorkExpoAlign::DoAlignStaticCam()
{
	
	AlignMotion& motions = GlobalVariables::GetInstance()->GetAlignMotion();

	int CENTER_CAM = motions.markParams.centerCamIdx;

	try
	{
		switch (m_u8StepIt)/* 작업 단계 별로 동작 처리 */
		{
		case 0x01: 
		{
	
			if (!(motions.GetCalifeature(OffsetType::align).caliCamIdx == CENTER_CAM) ||
				!(motions.GetCalifeature(OffsetType::expo).caliCamIdx == CENTER_CAM)) //테이블이 전부 있는지부터 검사.
			{
				m_enWorkState = ENG_JWNS::en_error;
				return;
			}
			m_enWorkState = SetExposeReady(TRUE, TRUE, TRUE, 1);
		}
		break;	    /* 노광 가능한 상태인지 여부 확인 */

		case 0x02: 
		{
			map<int, ENG_MMDI> axisMap = { {1,ENG_MMDI::en_align_cam1}, {2,ENG_MMDI::en_align_cam2}, {3,ENG_MMDI::en_axis_none} };
			auto res = MoveCamToSafetypos(axisMap[CENTER_CAM], motions.GetCalifeature(OffsetType::expo).caliCamXPos);
			m_enWorkState = res ? ENG_JWNS::en_next : ENG_JWNS::en_error;  //IsLoadedGerberCheck(); 불필요.
		}
		break;	/* 거버가 적재되었고, Mark가 존재하는지 확인 */

		case 0x03: m_enWorkState = SetTrigEnable(FALSE);						break;	/* Trigger Event - 비활성화 설정 */
		case 0x04: m_enWorkState = IsTrigEnabled(FALSE);						break;	/* Trigger Event - 빌활성화 확인  */
		case 0x05:
		{		
			uvEng_ACamCali_ResetAllCaliData();
			uvEng_Camera_ResetGrabbedImage();
			m_enWorkState = CameraSetCamMode(ENG_VCCM::en_grab_mode);
		}
		break;	//3캠 이동위치 경로설정

		case 0x06:
		{
			offsetPool.clear();
			motions.SetFiducialPool();
			grabMarkPath = motions.GetFiducialPool(CENTER_CAM);
			m_enWorkState = grabMarkPath.size() == 0 ? ENG_JWNS::en_error : ENG_JWNS::en_next;

			string temp = "x" + std::to_string(CENTER_CAM);
			if (m_enWorkState == ENG_JWNS::en_next && uvEng_GetConfig()->set_align.use_2d_cali_data)
			for (int i = 0; i < grabMarkPath.size(); i++)
			{
				auto stageAlignOffset = motions.EstimateAlignOffset(CENTER_CAM, grabMarkPath[i].mark_x, grabMarkPath[i].mark_y, CENTER_CAM == 3 ? 0 : motions.GetAxises()["cam"][temp.c_str()].currPos);
				uvEng_ACamCali_AddMarkPosForce(CENTER_CAM, grabMarkPath[i].GetFlag(STG_XMXY_RESERVE_FLAG::GLOBAL) ? ENG_AMTF::en_global : ENG_AMTF::en_local, stageAlignOffset.offsetX, stageAlignOffset.offsetY);
			}

			//여기서 등록하자. 	
		}
		break;

		case 0x07:
		{
			auto SingleGrab = [&](int camIndex,int& lastGrabCnt) -> bool 
			{
				lastGrabCnt = uvEng_Camera_GetGrabbedCount(&camIndex);
				auto res =  uvEng_Mvenc_ReqTrigOutOne(camIndex);
				return res;
			};

			//auto RetrySingleGrab = [&](int camIndex) -> bool
			//{
			//	int lastGrabCnt = 0; int tryCnt = 0; bool grabResult = false;
			//	if (SingleGrab(camIndex, lastGrabCnt))
			//	{
			//		while (grabResult == false && tryCnt < 3)
			//		{
			//			grabResult = GlobalVariables::GetInstance()->Waiter([&]()->bool
			//				{
			//					int grabCnt = uvEng_Camera_GetGrabbedCount(&camIndex);
			//					if (grabCnt <= lastGrabCnt) return false;

			//					auto& grab = uvEng_Camera_GetLastGrabbedACam()[camIndex - 1];
			//					
			//					if (grab.marked == false)
			//						uvEng_Camera_ResetGrabbedImage();

			//				}, 3 * 1000);
			//			tryCnt += grabResult == false ? 1 : 0;
			//		}
			//	}
			//	else return false;
			//};

			bool complete = GlobalVariables::GetInstance()->Waiter([&]()->bool
				{
					if (motions.NowOnMoving() == true)
					{
						this_thread::sleep_for(chrono::milliseconds(100));
					}
					else
					{
						if (grabMarkPath.size() == 0)
							return true;

						auto first = grabMarkPath.begin();
						auto arrival = motions.MovetoGerberPos(CENTER_CAM, *first);

						if (arrival == true)
						{
							const int STABLE_TIME = 1000;
							this_thread::sleep_for(chrono::milliseconds(STABLE_TIME));
							
							//string temp = "x" + std::to_string(CENTER_CAM);
							
							//motions.Refresh();
							
							//auto alignOffset = motions.EstimateAlignOffset(CENTER_CAM,
							//	motions.GetAxises()["stage"]["x"].currPos,
							//	motions.GetAxises()["stage"]["y"].currPos,
							//	CENTER_CAM == 3 ? 0 : motions.GetAxises()["cam"][temp.c_str()].currPos);
							

							/*auto markPos = first->GetMarkPos();
							auto expoOffset = motions.EstimateExpoOffset(std::get<0>(markPos), std::get<1>(markPos));
							auto diff = expoOffset;

							alignOffset.srcFid = *first;
							diff.srcFid = *first;*/

							//offsetPool[OffsetType::align].push_back(alignOffset);
							//offsetPool[OffsetType::expo].push_back(diff);


							TCHAR tzMsg[256] = { NULL }; int lastGrabCnt = 0;
							if (SingleGrab(CENTER_CAM,lastGrabCnt))
							{
								//if (uvEng_GetConfig()->set_align.use_2d_cali_data)
								//{
								//	uvEng_ACamCali_AddMarkPosForce(CENTER_CAM, first->GetFlag(STG_XMXY_RESERVE_FLAG::GLOBAL) ? ENG_AMTF::en_global : ENG_AMTF::en_local, alignOffset.offsetX, alignOffset.offsetY);
								//
								//	swprintf_s(tzMsg, 256, L"align_offset_x = %.4f mark_offset_y =%.4f", alignOffset.offsetX, alignOffset.offsetY);
								//	LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);
								//}

								grabMarkPath.erase(first);

							}
						}
					}
					return false;
				}, 60 * 1000 * 2);
			m_enWorkState = complete == true ? ENG_JWNS::en_next : ENG_JWNS::en_error;
		}
		break;

		case 0x08:
		{
			SetUIRefresh(true);
			m_enWorkState = IsGrabbedImageCount(m_u8MarkCount, 3000, &CENTER_CAM);
		}
		break;


		case 0x09:
		{
			m_enWorkState = IsSetMarkValidAll(0x01, &CENTER_CAM);
		}
		break;
		
		case 0x0a:
		{
			motions.SetAlignOffsetPool(offsetPool);
			m_enWorkState = CameraSetCamMode(ENG_VCCM::en_none); 
		}
		break;

		case 0x0b: 
		{
			m_enWorkState = SetAlignMarkRegistforStatic();
		}
		break;
		
		case 0x0c:m_enWorkState = IsAlignMarkRegist(); 
			break;
		
		
		case 0x0d: 
			m_enWorkState = SetPrePrinting();							
			break;

		case 0x0e:
			m_enWorkState = IsPrePrinted();
			break;

		case 0x0f:
			m_enWorkState = SetPrinting();
			//m_enWorkState = SetPrinting(); 
			break;
		case 0x10:
			m_enWorkState = IsPrinted();								
			break;
		case 0x11:
			m_enWorkState = SetWorkWaitTime(1000);						
			break;	/* 일정 시간 대기 */
		case 0x12:
			m_enWorkState = IsWorkWaitTime();							
			break;	/* 대기 완료 여부 확인 */
		case 0x13: 
			m_enWorkState = SetMovingUnloader();						
			break;	/* Stage Unloader 위치로 이동 */
		case 0x14: 
			m_enWorkState = IsMovedUnloader();							
			break;	/* Stage Unloader 위치에 도착 했는지 여부 확인 */


		}
	}
	catch (const std::exception&)
	{

	}
}

void CWorkExpoAlign::SetWorkNextStaticCam()
{
	UINT8 u8WorkTotal = m_u8StepTotal;
	UINT64 u64JobTime = GetTickCount64() - m_u64StartTime;

	uvEng_UpdateJobWorkTime(u64JobTime);
	TCHAR tzMesg[128] = { NULL };

	if (ENG_JWNS::en_error == m_enWorkState)
	{	
		swprintf_s(tzMesg, 128, L"Align Test <Error Step It = 0x%02x>", m_u8StepIt);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);

		/*노광 종료가 되면 Philhmil에 완료보고*/
		SetPhilProcessCompelet();

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
		m_enWorkState = ENG_JWNS::en_comp;
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
		m_enWorkState = ENG_JWNS::en_error;
		SaveExpoResult(0x00);
		m_u8StepIt = 0x01;
#endif


	}
	else if (ENG_JWNS::en_next == m_enWorkState)
	{
		CWork::CalcStepRate();
		if (m_u8StepTotal == m_u8StepIt)
		{
			SaveExpoResult(0x01);

			if (++m_u32ExpoCount != 2)
			{
				m_u8StepIt = 0x01;

#ifdef _DEBUG
				TCHAR szMesg[LOG_MESG_SIZE] = { NULL };

				swprintf_s(szMesg, LOG_MESG_SIZE, L"Expo Only Step :  m_u32ExpoCount= %d / m_u8StepTotal = 10000\n"
					, m_u32ExpoCount);

				/*Log 기록*/
				m_strLog.Format(szMesg);
				txtWrite(m_strLog);
#endif


				/*Auto Mdoe로 노광 종료가 되면 Philhmil에 완료보고*/
				//if (g_u8Romote == en_menu_phil_mode_auto)
				if (g_u16PhilCommand == (int)ENG_PHPC::ePHILHMI_C2P_PROCESS_EXECUTE)
				{
					SetPhilProcessCompelet();
				}
			}
			else
			{
				/*Auto Mdoe로 노광 종료가 되면 Philhmil에 완료보고*/
				if (g_u16PhilCommand == (int)ENG_PHPC::ePHILHMI_C2P_PROCESS_EXECUTE)
				{
					SetPhilProcessCompelet();
				}


				m_enWorkState = ENG_JWNS::en_comp;

				/* 항상 호출*/
				CWork::EndWork();
			}
		}
		else
		{
			m_u8StepIt++;
		}
		m_u64DelayTime = GetTickCount64();
	}
	
}




VOID CWorkExpoAlign::SetPhilProcessCompelet()
{
	STG_PP_P2C_PROCESS_COMP			stProcessComp;
	STG_PP_P2C_PROCESS_COMP_ACK		stProcessCompAck;
	stProcessComp.Reset();
	stProcessCompAck.Reset();

	/*Process Execute으로 받은 정보*/
	sprintf_s(stProcessComp.szRecipeName, DEF_MAX_RECIPE_NAME_LENGTH, m_stExpoLog.recipe_name);
	sprintf_s(stProcessComp.szGlassID, DEF_MAX_GLASS_NAME_LENGTH, m_stExpoLog.glassID);

	stProcessComp.usCount = 15;
	sprintf_s(stProcessComp.stVar[0].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stProcessComp.stVar[0].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Data");
	sprintf_s(stProcessComp.stVar[0].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%S", m_stExpoLog.data);

	sprintf_s(stProcessComp.stVar[1].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stProcessComp.stVar[1].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Time");
	sprintf_s(stProcessComp.stVar[1].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%lld", m_stExpoLog.expo_time);

	sprintf_s(stProcessComp.stVar[2].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "BOOL");
	sprintf_s(stProcessComp.stVar[2].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Mode");
	sprintf_s(stProcessComp.stVar[2].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "0");

	sprintf_s(stProcessComp.stVar[3].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "BOOL");
	sprintf_s(stProcessComp.stVar[3].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Succ");
	sprintf_s(stProcessComp.stVar[3].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.expo_succ);

	sprintf_s(stProcessComp.stVar[4].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stProcessComp.stVar[4].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "GrabDist_top_horz");
	sprintf_s(stProcessComp.stVar[4].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%.4f", m_stExpoLog.global_dist[0]);

	sprintf_s(stProcessComp.stVar[5].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stProcessComp.stVar[5].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "GrabDist_btm_horz");
	sprintf_s(stProcessComp.stVar[5].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%.4f", m_stExpoLog.global_dist[0]);

	sprintf_s(stProcessComp.stVar[6].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stProcessComp.stVar[6].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "GrabDist_lft_vert");
	sprintf_s(stProcessComp.stVar[6].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%.4f", m_stExpoLog.global_dist[0]);

	sprintf_s(stProcessComp.stVar[7].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stProcessComp.stVar[7].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "GrabDist_rgt_vert");
	sprintf_s(stProcessComp.stVar[7].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%.4f", m_stExpoLog.global_dist[0]);

	sprintf_s(stProcessComp.stVar[8].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stProcessComp.stVar[8].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "GrabDist_lft_diag");
	sprintf_s(stProcessComp.stVar[8].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%.4f", m_stExpoLog.global_dist[0]);

	sprintf_s(stProcessComp.stVar[9].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stProcessComp.stVar[9].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "GrabDist_rgt_diag");
	sprintf_s(stProcessComp.stVar[9].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%.4f", m_stExpoLog.global_dist[0]);

	sprintf_s(stProcessComp.stVar[10].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stProcessComp.stVar[10].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Gerber_Name");
	sprintf_s(stProcessComp.stVar[10].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%s", m_stExpoLog.gerber_name);

	sprintf_s(stProcessComp.stVar[11].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "INT");
	sprintf_s(stProcessComp.stVar[11].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Material_Thick");
	sprintf_s(stProcessComp.stVar[11].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.material_thick);

	sprintf_s(stProcessComp.stVar[12].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "DOUBLE");
	sprintf_s(stProcessComp.stVar[12].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Expo_Energy");
	sprintf_s(stProcessComp.stVar[12].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, " %.1f", m_stExpoLog.expo_energy);

	sprintf_s(stProcessComp.stVar[13].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "STRING");
	sprintf_s(stProcessComp.stVar[13].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Power_Name");
	sprintf_s(stProcessComp.stVar[13].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%s", m_stExpoLog.power_name);

	sprintf_s(stProcessComp.stVar[14].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "INT");
	sprintf_s(stProcessComp.stVar[14].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "Led_Duty_Cycle");
	sprintf_s(stProcessComp.stVar[14].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.led_duty_cycle);

	if (!m_stExpoLog.expo_succ)
	{
		stProcessComp.usErrorCode = ePHILHMI_ERR_STATUS_COMPLETE;
	}

	uvEng_Philhmi_Send_P2C_PROCESS_COMP(stProcessComp, stProcessCompAck);
}


VOID CWorkExpoAlign::txtWrite(CString msg)
{
	CStdioFile	imsifile;

	TCHAR tzFile[_MAX_PATH] = { NULL };
	SYSTEMTIME stTm = { NULL };

	GetLocalTime(&stTm);

	TCHAR tzPath[_MAX_PATH];
	swprintf_s(tzPath, _MAX_PATH, L"%s\\logs\\expo\\%04d-%02d-%02d ExpoAlign.dat", g_tzWorkDir, stTm.wYear, stTm.wMonth, stTm.wDay);


	imsifile.Open(tzPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite);

	imsifile.SeekToEnd();

	swprintf_s(tzFile, L"[%04d-%02d-%02d %02d:%02d:%02d.%03d]  ",
		(UINT16)stTm.wYear, (UINT16)stTm.wMonth, (UINT16)stTm.wDay,
		(UINT16)stTm.wHour, (UINT16)stTm.wMinute, (UINT16)stTm.wSecond, (UINT16)stTm.wMilliseconds);

	imsifile.WriteString(tzFile);
	imsifile.WriteString(msg);

	imsifile.Close();

}
