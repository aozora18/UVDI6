
/*
 desc : Align 노광 진행 - 얼라인 카메라 X 축 이동 없이 스테이지 Y 축만 후진
*/

#include "pch.h"
#include "../../MainApp.h"
#include "WorkExpoAlign.h"
#include "../../mesg/DlgMesg.h"


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

	/* 전체 작업 단계 */
	m_u8StepTotal	= 0x27;
	//m_u8StepTotal = 0x1b;
	/* 총 검색 대상의 마크 개수 얻기 */
	m_u8MarkCount	= uvEng_Luria_GetMarkCount(ENG_AMTF::en_global) +
					  uvEng_Luria_GetMarkCount(ENG_AMTF::en_local);

	return TRUE;
}

/*
 desc : 주기적으로 작업 수행
 parm : None
 retn : None
*/
VOID CWorkExpoAlign::DoWork()
{
	/* 작업 단계 별로 동작 처리 */
	switch (m_u8StepIt)
	{
	case 0x01 : m_enWorkState = SetExposeReady(TRUE, TRUE, TRUE, 1);		break;	/* 노광 가능한 상태인지 여부 확인 */
	case 0x02 : m_enWorkState = IsLoadedGerberCheck();						break;	/* 거버가 적재되었고, Mark가 존재하는지 확인 */
	case 0x03 : m_enWorkState = SetTrigEnable(FALSE);						break;	/* Trigger Event - 비활성화 설정 */
	case 0x04 : m_enWorkState = IsTrigEnabled(FALSE);						break;	/* Trigger Event - 빌활성화 확인  */
	case 0x05 : m_enWorkState = SetAlignMovingInit();						break;	/* Stage X/Y, Camera 1/2 - Align (Global) 시작 위치로 이동 */
	case 0x06 : m_enWorkState = SetTrigPosCalcSaved();						break;	/* Trigger 발생 위치 계산 및 임시 저장 */

	case 0x07: m_enWorkState = IsAlignMovedInit();							break;	/* Stage X/Y, Camera 1/2 - Align (Global) 시작 위치 도착 여부 */

	case 0x08 : m_enWorkState = SetTrigRegistGlobal();						break;	/* Trigger 발생 위치 - 트리거 보드에 Global Mark 위치 등록 */
	case 0x09 : m_enWorkState = IsTrigRegistGlobal();						break;	/* Trigger 발생 위치 등록 확인 */

	case 0x0a : m_enWorkState = SetAlignMeasMode();							break;
	case 0x0b : m_enWorkState = IsAlignMeasMode();							break;

	case 0x0c : m_enWorkState = SetAlignMovingGlobal();						break;	/* Global Mark 4 군데 위치 확인 */
	case 0x0d : m_enWorkState = IsAlignMovedGlobal();						break;	/* Global Mark 4 군데 측정 완료 여부 */

	case 0x0e : m_enWorkState = SetAlignMovingLocal(0x00, 0x00);			break;	/* Stage X/Y, Camera 1/2 - Align (Local:역방향) 시작 위치로 이동 */
	case 0x0f : m_enWorkState = SetTrigRegistLocal(0x00);					break;	/* Trigger (역방향) 발생 위치 - 트리거 보드에 Local Mark 위치 등록 */
	case 0x10 : m_enWorkState = IsTrigRegistLocal(0x00);					break;	/* Trigger (역방향) 발생 위치 등록 확인 */
	case 0x11 : m_enWorkState = IsAlignMovedLocal(0x00, 0x00);				break;	/* Stage X/Y, Camera 1/2 - Align (Local) 시작 위치 도착 여부 */
	case 0x12 : m_enWorkState = SetAlignMovingLocal(0x01, 0x00);			break;	/* Local Mark (역방향) 16 군데 위치 확인 */
	case 0x13 : m_enWorkState = IsAlignMovedLocal(0x01, 0x00);				break;	/* Local Mark (역방향) 16 군데 측정 완료 여부 */

	case 0x14 : m_enWorkState = SetAlignMovingLocal(0x00, 0x01);			break;	/* Stage X/Y, Camera 1/2 - Align (Local:정방향) 시작 위치로 이동 */
	case 0x15 : m_enWorkState = SetTrigRegistLocal(0x01);					break;	/* Trigger (정방향) 발생 위치 - 트리거 보드에 Local Mark 위치 등록 */
	case 0x16 : m_enWorkState = IsTrigRegistLocal(0x01);					break;	/* Trigger (정방향) 발생 위치 등록 확인 */
	case 0x17 : m_enWorkState = IsAlignMovedLocal(0x00, 0x01);				break;	/* Trigger (정방향) 발생 위치 등록 확인 */
	case 0x18 : m_enWorkState = SetAlignMovingLocal(0x01, 0x01);			break;	/* Local Mark (정방향) 16 군데 위치 확인 */
	case 0x19 : m_enWorkState = IsAlignMovedLocal(0x01, 0x01);				break;	/* Local Mark (정방향) 16 군데 측정 완료 여부 */

	case 0x1a : m_enWorkState = SetTrigEnable(FALSE);						break;	/* Trigger Event - 비활성화 설정 */
	case 0x1b : m_enWorkState = IsGrabbedImageCount(m_u8MarkCount, 3000);	break;
	case 0x1c : m_enWorkState = IsSetMarkValidAll(0x01);					break;	/* 모든 측정된 Align Mark 유효성 확인 */
	case 0x1d : m_enWorkState = SetAlignMarkRegist();						break;	/* 모든 측정된 Align Mark - Luria Service에 등록 */
	case 0x1e : m_enWorkState = IsAlignMarkRegist();						break;	/* 모든 측정된 Align Mark 등록 여부 확인 */
	case 0x1f : m_enWorkState = IsTrigEnabled(FALSE);						break;	/* Trigger Event - 비활성화 확인 */

	case 0x20 : m_enWorkState = SetPrePrinting();							break;	/* Luria Control - PrePrinting */
	case 0x21 : m_enWorkState = IsPrePrinted();								break;	/* Luria Control - PrePrinted 확인 */
	case 0x22 : m_enWorkState = SetPrinting();								break;	/* Luria Control - Printing */
	case 0x23 : m_enWorkState = IsPrinted();								break;	/* Luria Control - Printed 확인 */

	case 0x24 : m_enWorkState = SetWorkWaitTime(1000);						break;	/* 일정 시간 대기 */
	case 0x25 : m_enWorkState = IsWorkWaitTime();							break;	/* 대기 완료 여부 확인 */

	case 0x26 : m_enWorkState = SetMovingUnloader();						break;	/* Stage Unloader 위치로 이동 */
	case 0x27 : m_enWorkState = IsMovedUnloader();							break;	/* Stage Unloader 위치에 도착 했는지 여부 확인 */
	}

	/* 다음 작업 진행 여부 판단 */
	CWorkExpoAlign::SetWorkNext();
	/* 장시간 동안 동일 위치를 반복 수행한다면 에러 처리 */
	CheckWorkTimeout();
}

/*
 desc : 다음 단계로 이동하기 위한 처리
 parm : None
 retn : None
*/
VOID CWorkExpoAlign::SetWorkNext()
{
	UINT64 u64TickCount	= GetTickCount64();
	UINT64 u64JobTime	= u64TickCount - m_u64StartTime;

	/* 매 작업 구간마다 시간 값 증가 처리 */
	uvEng_UpdateJobWorkTime(u64JobTime);

	/* 모든 작업이 종료 되었는지 여부 */
	if (ENG_JWNS::en_error == m_enWorkState)
	{
		SaveExpoResult(0x00);
		m_u8StepIt	= 0x01;

		TCHAR tzMesg[128] = { NULL };
		swprintf_s(tzMesg, 128, L"Work Expo Align <Error Step It = 0x%02x>", m_u8StepIt);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);

		//m_enWorkState = ENG_JWNS::en_comp;
		m_enWorkState = ENG_JWNS::en_error;
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
		case 0x25: uvEng_Luria_ReqGetPhLedTempAll();	break;
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
				if(g_u16PhilCommand== ePHILHMI_C2P_PROCESS_EXECUTE)
				{
					SetPhilProcessCompelet();
				}
			}
			else
			{
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
				m_u8StepIt	= 0x00;
				m_enWorkState = ENG_JWNS::en_comp;
			}
		}
		/* 가장 최근에 Waiting 한 시간 저장 */
		m_u64DelayTime	= GetTickCount64();
	}
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

	/* 노광 시간 / 노광 성공 / 거버 이름 / 소재 두께 / 에너지 */
	UINT64 u64JobTime = uvEng_GetJobWorkTime();
	swprintf_s(tzResult, 1024, L"%u,%s,%S,%d,%.4f,",
		uvCmn_GetTimeToType(u64JobTime, 0x03), tzState[state], pstRecipe->gerber_name, pstRecipe->material_thick, pstRecipe->expo_energy);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);


	/* 마크 간의 6 곳 길이 측정 오차 값 과 전체 노광하는데 소요된 시간 저장 */
	LPG_GMLV pstMarkDiff = &uvEng_GetConfig()->mark_diff;
	swprintf_s(tzResult, 1024, L"%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,",
		pstMarkDiff->result[0].diff * 100.0f, pstMarkDiff->result[1].diff * 100.0f, pstMarkDiff->result[2].diff * 100.0f,
		pstMarkDiff->result[3].diff * 100.0f, pstMarkDiff->result[4].diff * 100.0f, pstMarkDiff->result[5].diff * 100.0f);
	//swprintf_s(tzResult, 1024, L"%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,",
	//		   uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_top_horz),
	//		   uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_btm_horz),
	//		   uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_lft_vert),
	//		   uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_rgt_vert),
	//		   uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_lft_diag),
	//		   uvEng_Camera_GetGrabbedMarkDist(ENG_GMDD::en_rgt_diag)
	//		   );
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	/* 얼라인 마크 검색 결과 값 저장 */
	pstMark	= uvEng_Camera_GetGrabbedMark(0x01, 0x00);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%u,%.4f,%.4f,",
				   pstMark->score_rate, pstMark->scale_rate, pstMark->scale_size,
				   pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	pstMark	= uvEng_Camera_GetGrabbedMark(0x01, 0x01);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%u,%.4f,%.4f,",
				   pstMark->score_rate, pstMark->scale_rate, pstMark->scale_size,
				   pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	pstMark	= uvEng_Camera_GetGrabbedMark(0x02, 0x00);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%u,%.4f,%.4f,",
				   pstMark->score_rate, pstMark->scale_rate, pstMark->scale_size,
				   pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	pstMark	= uvEng_Camera_GetGrabbedMark(0x02, 0x01);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%u,%.4f,%.4f,",
				   pstMark->score_rate, pstMark->scale_rate, pstMark->scale_size,
				   pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	/* 마지막엔 무조건 다음 라인으로 넘어가도록 하기 위함 */
	uvCmn_SaveTxtFileW(L"\n", (UINT32)wcslen(L"\n"), tzFile, 0x01);


	/*ExpoLog 기록*/
	memcpy(m_stExpoLog.gerber_name, pstRecipe->gerber_name, MAX_GERBER_NAME);
	m_stExpoLog.material_thick = pstRecipe->material_thick;
	m_stExpoLog.expo_energy = pstRecipe->expo_energy;
	m_stExpoLog.align_type = pstAlignRecipe->align_type;
	m_stExpoLog.mark_type = pstAlignRecipe->mark_type;
	m_stExpoLog.lamp_type = pstAlignRecipe->lamp_type;
	m_stExpoLog.gain_level[0] = pstAlignRecipe->gain_level[0];
	m_stExpoLog.gain_level[1] = pstAlignRecipe->gain_level[1];
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

	/*노광 결과 파라미터값*/
	stProcessComp.usCount = 3;
	sprintf_s(stProcessComp.stVar[0].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "CHAR");
	sprintf_s(stProcessComp.stVar[0].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "data");
	sprintf_s(stProcessComp.stVar[0].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, m_stExpoLog.data);

	sprintf_s(stProcessComp.stVar[1].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "UINT64");
	sprintf_s(stProcessComp.stVar[1].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "expo_time");
	sprintf_s(stProcessComp.stVar[1].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.expo_time);

	sprintf_s(stProcessComp.stVar[2].szParameterType, DEF_MAX_STATE_PARAM_TYPE_LENGTH, "UINT8");
	sprintf_s(stProcessComp.stVar[2].szParameterName, DEF_MAX_STATE_PARAM_NAME_LENGTH, "expo_succ");
	sprintf_s(stProcessComp.stVar[2].szParameterValue, DEF_MAX_STATE_PARAM_VALUE_LENGTH, "%d", m_stExpoLog.expo_succ);

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
