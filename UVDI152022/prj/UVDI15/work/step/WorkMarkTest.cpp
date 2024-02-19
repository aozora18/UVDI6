
/*
 desc : Align (Both Global and Local Mark) Mark 동작 (인식)
*/

#include "pch.h"
#include "../../MainApp.h"
#include "WorkMarkTest.h"


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
CWorkMarkTest::CWorkMarkTest(LPG_CELA expo)
	: CWorkStep()
{
	m_enWorkJobID	= ENG_BWOK::en_mark_test;
	m_u32ExpoCount = 0;

	m_stExpoLog.Init();
	memcpy(&m_stExpoLog, expo, sizeof(STG_CELA));
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CWorkMarkTest::~CWorkMarkTest()
{
}

/*
 desc : 초기 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkMarkTest::InitWork()
{
	/* 내부 멤버 변수 값 초기화 */
	if (!CWork::InitWork())	return FALSE;

	/* 전체 작업 단계 */
#if 1
	//m_u8StepTotal	= 0x23;
	m_u8StepTotal = 0x27;
#else
	m_u8StepTotal	= 0x0e;
#endif
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
VOID CWorkMarkTest::DoWork()
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

	case 0x0e: m_enWorkState = CameraSetCamMode(ENG_VCCM::en_none);			break;	/* Cam None 모드로 변경 */
	case 0x0f : m_enWorkState = SetAlignMovingLocal(0x00, 0x00);			break;	/* Stage X/Y, Camera 1/2 - Align (Local:역방향) 시작 위치로 이동 */
	case 0x10 : m_enWorkState = SetTrigRegistLocal(0x00);					break;	/* Trigger (역방향) 발생 위치 - 트리거 보드에 Local Mark 위치 등록 */
	case 0x11 : m_enWorkState = IsTrigRegistLocal(0x00);					break;	/* Trigger (역방향) 발생 위치 등록 확인 */
	case 0x12 : m_enWorkState = IsAlignMovedLocal(0x00, 0x00);				break;	/* Stage X/Y, Camera 1/2 - Align (Local) 시작 위치 도착 여부 */
	case 0x13: m_enWorkState = CameraSetCamMode(ENG_VCCM::en_grab_mode);	break;	/* Cam None 모드로 변경 */
	case 0x14 : m_enWorkState = SetAlignMovingLocal(0x01, 0x00);			break;	/* Local Mark (역방향) 16 군데 위치 확인 */
	case 0x15 : m_enWorkState = IsAlignMovedLocal(0x01, 0x00);				break;	/* Local Mark (역방향) 16 군데 측정 완료 여부 */

	case 0x16: m_enWorkState = CameraSetCamMode(ENG_VCCM::en_none);			break;	/* Cam None 모드로 변경 */
	case 0x17 : m_enWorkState = SetAlignMovingLocal(0x00, 0x01);			break;	/* Stage X/Y, Camera 1/2 - Align (Local:정방향) 시작 위치로 이동 */
	case 0x18 : m_enWorkState = SetTrigRegistLocal(0x01);					break;	/* Trigger (정방향) 발생 위치 - 트리거 보드에 Local Mark 위치 등록 */
	case 0x19 : m_enWorkState = IsTrigRegistLocal(0x01);					break;	/* Trigger (정방향) 발생 위치 등록 확인 */
	case 0x1a : m_enWorkState = IsAlignMovedLocal(0x00, 0x01);				break;	/* Trigger (정방향) 발생 위치 등록 확인 */
	case 0x1b: m_enWorkState = CameraSetCamMode(ENG_VCCM::en_grab_mode);	break;	/* Cam None 모드로 변경 */
	case 0x1c : m_enWorkState = SetAlignMovingLocal(0x01, 0x01);			break;	/* Local Mark (정방향) 16 군데 위치 확인 */
	case 0x1d : m_enWorkState = IsAlignMovedLocal(0x01, 0x01);				break;	/* Local Mark (정방향) 16 군데 측정 완료 여부 */

	case 0x1e : m_enWorkState = SetTrigEnable(FALSE);						break;
	case 0x1f : m_enWorkState = IsGrabbedImageCount(m_u8MarkCount,3000);	break;
	case 0x20 : m_enWorkState = IsSetMarkValidAll(0x01);					break;
	case 0x21 : m_enWorkState = SetAlignMarkRegist();						break;
	case 0x22 : m_enWorkState = IsAlignMarkRegist();						break;
	case 0x23 : m_enWorkState = IsTrigEnabled(FALSE);						break;
	case 0x24 : m_enWorkState = SetWorkWaitTime(2000);						break;
	case 0x25 : m_enWorkState = IsWorkWaitTime();							break;
	case 0x26 : m_enWorkState = SetMovingUnloader();						break;
	case 0x27 : m_enWorkState = IsMovedUnloader();							break;
		
	case 0x28: m_enWorkState = SetHomingACamSide();							break;
	case 0x29: m_enWorkState = IsHomedACamSide();							break;
		//case 0x26: m_enWorkState = DoDriveHoming(ENG_MMDI::en_stage_y);			break;
	case 0x2a: m_enWorkState = SetWorkWaitTime(10000);						break;
	case 0x2b: m_enWorkState = IsWorkWaitTime();							break;
		//case 0x29: m_enWorkState = IsDrivedHomed(ENG_MMDI::en_stage_y);			break;
	}

	/* 다음 작업 진행 여부 판단 */
	CWorkMarkTest::SetWorkNext();
	/* 장시간 동안 동일 위치를 반복 수행한다면 에러 처리 */
	CheckWorkTimeout();
}

/*
 desc : 다음 단계로 이동하기 위한 처리
 parm : None
 retn : None
*/
VOID CWorkMarkTest::SetWorkNext()
{
	UINT8 u8WorkTotal	= IsMarkTypeOnlyGlobal() ? (m_u8StepTotal - 12) : m_u8StepTotal;
	UINT64 u64JobTime	= GetTickCount64() - m_u64StartTime;

	/* 매 작업 구간마다 시간 값 증가 처리 */
	uvEng_UpdateJobWorkTime(u64JobTime);

	/* 모든 작업이 종료 되었는지 여부 */
	if (ENG_JWNS::en_error == m_enWorkState)
	{
		TCHAR tzMesg[128] = {NULL};
		swprintf_s(tzMesg, 128, L"Align Test <Error Step It = 0x%02x>", m_u8StepIt);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);
		
		SaveExpoResult(0x00);
		//m_u8StepIt = 0x26;
		m_u8StepIt = 0x01;

		//m_enWorkState = ENG_JWNS::en_comp;
		m_enWorkState = ENG_JWNS::en_error;
		//m_enWorkState = ENG_JWNS::en_next;
	}
	else if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* 작업률 계산 후 임시 저장 */
		CWork::CalcStepRate();
		/* 현재 Global Mark까지 인식 했는지 여부 */
		//if (m_u8StepIt == 0x0d)
		if (m_u8StepIt == 0x0d)
		{
#if 1
			/* 현재 동작 모드가 Global 방식인지 Local 포함 방식인지 여부에 따라 다름 */
			//if (IsMarkTypeOnlyGlobal())	m_u8StepIt	= 0x1a;
			if (IsMarkTypeOnlyGlobal())	m_u8StepIt	= 0x1e;

#else
			/* 현재 동작 모드가 Global 방식인지 Local 포함 방식인지 여부에 따라 다름 */
			m_u8WorkStep	= 0x1f;
#endif
#if 0	/* CWork::SetAlignMarkRegist() 여기서 처리하고 있음 */
			uvData_Mark_SetMarkLenActive();
#endif
		}
		if (m_u8StepTotal == m_u8StepIt)
		{
			/* 작업 완료 후 각종 필요한 정보 저장 */
			SaveExpoResult(0x01);

			//if (++m_u32ExpoCount < 11)
			//if (++m_u32ExpoCount < 2)
			if (++m_u32ExpoCount != m_stExpoLog.expo_count)
			{
				m_u8StepIt = 0x01;
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
		/* 가장 최근에 Waiting 한 시간 저장 */
		m_u64DelayTime	= GetTickCount64();
	}
}



/*
 desc : 얼라인 카메라 2대를 좌/우 끝으로 이동 시킴 (장비 예열 목적으로 사용됨)
 parm : None
 retn : TRUE or FALSE
*/
ENG_JWNS CWorkMarkTest::SetHomingACamSide()
{
	LPG_CMSI pstMC2 = &uvEng_GetConfig()->mc2_svc;

	/* 현재 토글 상태 값 얻기 */
	//uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
	//uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam2);
	/* 짝수일 땐, 최소 값 (좌측 끝)으로 이동 */
	//if (!m_bMoveACamSide)
	//{
		/* 1번 카메라부터 home */
		if(!uvEng_MC2_SendDevHoming(ENG_MMDI::en_align_cam1))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to home the align camera (1) to (left)");
			return  ENG_JWNS::en_wait;
		}
		/* 2번 카메라부터 home */
		if (!uvEng_MC2_SendDevHoming(ENG_MMDI::en_align_cam2))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to home the align camera (2) to (left)");
			return  ENG_JWNS::en_wait;
		}
	//}
	///* 홀수일 때, 최대 값 (우측 끝)으로 이동 */
	//else
	//{
	//	/* 2번 카메라부터 home */
	//	if (!uvEng_MC2_SendDevHoming(ENG_MMDI::en_align_cam2))
	//	{
	//		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to home the align camera (1) to (right)");
	//		return FALSE;
	//	}
	//	/* 1번 카메라부터 home */
	//	if (!uvEng_MC2_SendDevHoming(ENG_MMDI::en_align_cam1))
	//	{
	//		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to home the align camera (2) to (right)");
	//		return FALSE;
	//	}
	//}

	return	ENG_JWNS::en_next;
}

/*
 desc : 2대를 좌/우 끝으로 이동이 완료 되었는지 확인 (장비 예열 목적으로 사용됨)
 parm : None
 retn : TRUE or FALSE
*/
ENG_JWNS CWorkMarkTest::IsHomedACamSide()
{
	/* 최소 1 분마다 동작하도록 설정 함 */
//	if (GetTickCount64() < (m_u64TickACamSide + 60000) /* 1 minutue */)	return FALSE;

	/* 모두 이동 했는지 여부 확인 */
	if (!uvCmn_MC2_IsDriveHomed(ENG_MMDI::en_align_cam1))	return  ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDriveHomed(ENG_MMDI::en_align_cam2))	return  ENG_JWNS::en_wait;

	/* 반대편으로 이동 */
	//m_bMoveACamSide = !m_bMoveACamSide;

	/* 최근 동작한 시간 저장 */
//	m_u64TickACamSide = GetTickCount64();

	return	ENG_JWNS::en_next;
}

/*
 desc : 노광 완료 후 각종 정보 저장
 parm : state	- [in]  0x00: 작업 실패, 0x01: 작업 성공
 retn : None
*/
VOID CWorkMarkTest::SaveExpoResult(UINT8 state)
{
	TCHAR tzResult[1024] = { NULL }, tzFile[MAX_PATH_LEN] = { NULL }, tzState[2][8] = { L"FAIL", L"SUCC" };
	LPG_ACGR pstMark = NULL;
	SYSTEMTIME stTm = { NULL };
	MEMORYSTATUSEX stMem = { NULL };

	uvEng_Luria_ReqGetPhLedTempAll();
	UINT16(*pLed)[8] = uvEng_ShMem_GetLuria()->directph.light_source_driver_temp_led;
	UINT16(*pBoard)[8] = uvEng_ShMem_GetLuria()->directph.light_source_driver_temp_board;

	/* 현재 컴퓨터 날짜를 파일명으로 설정 */
	GetLocalTime(&stTm);
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\logs\\expo\\%04d-%02d-%02d MarkText.csv",
		g_tzWorkDir, stTm.wYear, stTm.wMonth, stTm.wDay);

	/* 만약에 열고자 하는 파일이 없으면, 해당 파일 생성 후, 타이틀 (Field) 추가 */
	if (!uvCmn_FindFile(tzFile))
	{
		wcscpy_s(tzResult, 1024,
			L"Count,Time,succ,"
			L"score_1,scale_1,mark_move_x1(mm),mark_move_y1(mm),"
			L"score_2,scale_2,mark_move_x2(mm),mark_move_y2(mm),"
			L"score_3,scale_3,mark_move_x3(mm),mark_move_y3(mm),"
			L"score_4,scale_4,mark_move_x4(mm),mark_move_y4(mm),"
			//L"led(1:1),led(1:2),led(1:3),led(1:4),board(1:1),board(1:2),board(1:3),board(1:4),"
			//L"led(2:1),led(2:2),led(2:3),led(2:4),board(2:1),board(2:2),board(2:3),board(2:4),"
			//L"led(3:1),led(3:2),led(3:3),led(3:4),board(3:1),board(3:2),board(3:3),board(3:4),"
			//L"led(4:1),led(4:2),led(4:3),led(4:4),board(4:1),board(4:2),board(4:3),board(4:4),"
			//L"led(5:1),led(5:2),led(5:3),led(5:4),board(5:1),board(5:2),board(5:3),board(5:4),"
			//L"led(6:1),led(6:2),led(6:3),led(6:4),board(6:1),board(6:2),board(6:3),board(6:4),\n");
			L"horz_dist_13(mm),horz_dist_24(mm),vert_dist_12(mm),vert_dist_34(mm),"
			L"diag_dist_14(mm),diag_dist_23(mm),\n");
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x00);
	}

	swprintf_s(tzResult, 1024, L"Count=%d, %02d:%02d:%02d,%s,",
		m_u32ExpoCount, stTm.wHour, stTm.wMinute, stTm.wSecond, tzState[state]);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);

	/* 얼라인 마크 검색 결과 값 저장 */
	pstMark = uvEng_Camera_GetGrabbedMark(0x01, 0x00);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%.4f,%.4f,",
			pstMark->score_rate, pstMark->scale_rate,
			pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	pstMark = uvEng_Camera_GetGrabbedMark(0x01, 0x01);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%.4f,%.4f,",
			pstMark->score_rate, pstMark->scale_rate,
			pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	pstMark = uvEng_Camera_GetGrabbedMark(0x02, 0x00);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%.4f,%.4f,",
			pstMark->score_rate, pstMark->scale_rate,
			pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	pstMark = uvEng_Camera_GetGrabbedMark(0x02, 0x01);
	if (pstMark)
	{
		swprintf_s(tzResult, 1024, L"%.3f,%.3f,%.4f,%.4f,",
			pstMark->score_rate, pstMark->scale_rate,
			pstMark->move_mm_x, pstMark->move_mm_y);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}

	/* 광학계 LED / Board 온도 값 저장 */
	//for (int i = 0x00; i < 0x06; i++)
	//{
	//	swprintf_s(tzResult, 1024, L"%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,",
	//		pLed[i][0] / 10.0f, pLed[i][1] / 10.0f, pLed[i][2] / 10.0f, pLed[i][3] / 10.0f,
	//		pBoard[i][0] / 2.0f, pBoard[i][1] / 2.0f, pBoard[i][2] / 2.0f, pBoard[i][3] / 2.0f);
	//	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	//}
		/* 마크 간의 6 곳 길이 측정 오차 값 과 전체 노광하는데 소요된 시간 저장 */
	LPG_GMLV pstMarkDiff = &uvEng_GetConfig()->mark_diff;
	swprintf_s(tzResult, 1024, L"%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,",
		pstMarkDiff->result[0].diff * 100.0f, pstMarkDiff->result[1].diff * 100.0f, pstMarkDiff->result[2].diff * 100.0f,
		pstMarkDiff->result[3].diff * 100.0f, pstMarkDiff->result[4].diff * 100.0f, pstMarkDiff->result[5].diff * 100.0f);
	uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);

	/* 마지막엔 무조건 다음 라인으로 넘어가도록 하기 위함 */
	uvCmn_SaveTxtFileW(L"\n", (UINT32)wcslen(L"\n"), tzFile, 0x01);
}