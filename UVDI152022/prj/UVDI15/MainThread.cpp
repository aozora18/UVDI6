
/*
 desc : Main Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "MainThread.h"

/* 작업 객체 */
#include "./work/step/WorkAbort.h"
#include "./work/step/WorkHoming.h"
#include "./work/step/WorkInited.h"
#include "./work/step/WorkRecipeLoad.h"
#include "./work/step/WorkRecipeUnload.h"
#include "./work/step/WorkExpoOnly.h"
#include "./work/step/WorkExpoAlign.h"
#include "./work/step/WorkMarkMove.h"
#include "./work/step/WorkMarkTest.h"
#include "./work/step/WorkOnlyFEM.h"
#include "./work/step/WorkFEMExpo.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : parent	- [in]  자신을 호출한 부모 윈도 핸들
 retn : None
*/
CMainThread::CMainThread(HWND parent)
	: CThinThread()
{
	m_hParent		= parent;
	m_pWorkJob		= NULL;
	m_bAppExit		= FALSE;
	m_u64TickFile	= 0;
	m_u64TickTemp	= 0;
	m_u64TickExit	= 0;
	m_u64TickHotMon	= GetTickCount64();
	m_u64TickHotCtrl=m_u64TickHotMon;
}

/*
 desc : 파괴자
 parm : None
*/
CMainThread::~CMainThread()
{
	/* 기존 작업 메모리 해제 */
	ResetWorkJob();
}

/*
 desc : 스레드 동작시에 초기 한번 호출됨
 parm : None
 retn : DI_TRUE or DI_FALSE
*/
BOOL CMainThread::StartWork()
{
	/* 무조건 1 초 정도 쉬고 동작하도록 함 */
	CThinThread::Sleep(1000);

	return TRUE;
}

/*
 desc : 스레드 동작시에 주기적으로 호출됨
 parm : None
 retn : None
*/
VOID CMainThread::RunWork()
{
	/* 동기 진입 */
	if (m_csSyncWork.Enter())
	{
		UINT64 u64Tick	= GetTickCount64();

		/* 에뮬레이터 모드로 동작할 경우, 조금 느리게 반응하기 위함 (메시지 출력 확인 위해) */
		if (uvEng_GetConfig() && uvEng_GetConfig()->IsRunDemo() &&
			m_pWorkJob && m_pWorkJob->IsWorkStopped())
		{
			uvCmn_uSleep(100);
		}
		/* 만약 현재 작업 중지 상태인 경우 */
		/* 시나리오 동작 */
		if (m_pWorkJob && !m_pWorkJob->IsWorkStopped())
		{
			/* Luria 관련 작업인 경우 알림 */
			if (m_pWorkJob->GetWorkJobID() == ENG_BWOK::en_gerb_load ||
				m_pWorkJob->GetWorkJobID() == ENG_BWOK::en_gerb_unload)
			{
				uvEng_Luria_SetWorkBusy(TRUE);
			}
			RunWorkJob();
		}
		else
		{
			/* 작업이 없는 경우라고 알림 */
			uvEng_Luria_SetWorkBusy(FALSE);
		}
		/* 만약 작업 동작이 에러가 발생 했다면 ... Abort 동작 수행 */
		if (m_pWorkJob && m_pWorkJob->IsWorkError())
		{
			RunWorkJob(ENG_BWOK::en_work_stop);
		}
		/* 로그 데이터 관리 */
		else if (!m_pWorkJob || m_pWorkJob->IsWorkCompleted())
		{
			/* 1 분마다 과거 이력 파일 삭제 진행 */
			if ((m_u64TickFile + 60 * 1000) < u64Tick)
			{
				m_u64TickFile	= u64Tick;
				DeleteHistFiles();
			}
			/* 특정 주기마다 장비 내부 온도 데이터 저장 */
			if ((m_u64TickTemp + 10 * 1000) < u64Tick)
			{
				m_u64TickTemp	= u64Tick;
				//SaveTempFiles();
			}
			/* 온조기 온도 제어 (특정 주기마다... 제어 수행) */
			if (uvEng_GetConfig()->auto_hot_air.use_auto_hot_air &&
				(m_u64TickHotMon + 5 * 1000) < u64Tick)
			{
				m_u64TickHotMon	= u64Tick;
				SetAutoHotAir(u64Tick);
			}
		}

		/* 프로그램 종료 메시지 처리 */
		if (m_bAppExit && (u64Tick > m_u64TickExit))
		{
			SendMesgParent(ENG_BWOK::en_app_exit, 100);
		}
		else
		{
			/* 부모에게 주기적으로 알리는 방법 제공 */
			if (m_pWorkJob && IsSetAlignedMark())	SendMesgParent(ENG_BWOK::en_mesg_mark, 100);
			else									SendMesgParent(ENG_BWOK::en_mesg_norm, 100);
		}
		/* 동기 해제 */
		m_csSyncWork.Leave();
	}
}

/*
 desc : 스레드 종료시에 마지막으로 한번 호출됨
 parm : None
 retn : None
*/
VOID CMainThread::EndWork()
{
}

/*
 desc : 기존 동작 중인 작업 메모리 해제
 parm : None
 retn : None
*/
VOID CMainThread::ResetWorkJob()
{
	if (!m_pWorkJob)	return;
	delete m_pWorkJob;
	m_pWorkJob		= NULL;
#if 0
	/* 무조건 최대 값으로 초기화 */
	m_u8AlignStepIt	= 0xff;
#endif
}

/*
 desc : 작업 수행 (작업이 종료될 때까지 수행)
 parm : None
 retn : None
*/
VOID CMainThread::RunWorkJob()
{
#if 0	/* SKC Project에서 일단 사용 안함. 좀 민감 함 */
	if (m_pWorkJob->IsWorkStopped())
	{
		if (CheckStartButton())	return;
	}
#endif
#if 0
	/* 강제 종료 버튼을 누른 경우인지 여부 */
	if (m_pWorkJob->IsWorkAbort())
	{
		ResetWorkJob();
		return;
	}
	else
#endif
	{
		/* 작업이 할당되어 있고, 중지되지 않은 상태일 경우만 수행 */
		if (!m_pWorkJob->IsWorkStopped())
		{
			switch (m_pWorkJob->GetWorkJobID())
			{
			case ENG_BWOK::en_work_stop		:
			case ENG_BWOK::en_work_home		:
			case ENG_BWOK::en_work_init		:

			case ENG_BWOK::en_gerb_load		:
			case ENG_BWOK::en_gerb_unload	:

			case ENG_BWOK::en_gerb_onlyfem:

			case ENG_BWOK::en_mark_move		:
			case ENG_BWOK::en_mark_test		:

			case ENG_BWOK::en_expo_only		:
			case ENG_BWOK::en_expo_align	:	
			case ENG_BWOK::en_gerb_expofem	:	m_pWorkJob->DoWork();	break;	/* 현재 작업 및 단계 명칭 갱신 */
			}
		}
	}
	/* 현재 작업 명칭 갱신 */
	UpdateWorkName();
}

/*
 desc : 작업 수행
 parm : job_id	- [in]  ENG_BWOK 값
		data	- [in]  Job 별 상이한 값
 retn : TRUE or FALSE
*/
BOOL CMainThread::RunWorkJob(ENG_BWOK job_id, PUINT64 data)
{
	BOOL bSucc	= FALSE;

	/* 동기 진입 */
	if (m_csSyncWork.Enter())
	{
#if 0
		/* 기존 동작 중인 작업이 있다면 취소 시킴 */
		if (m_pWorkJob && !m_pWorkJob->IsWorkStopped() &&
			job_id == ENG_BWOK::en_work_stop)
		{
			/* 강제로 작업 취소 시키기 위한 작업 시도 */
			m_pWorkJob->SetWorkStop();
			bSucc = TRUE;
		}
		else
#endif
		{
			/* 기존 작업 메모리 해제 */
			ResetWorkJob();

			/* 작업 동작 모드 */
			switch (job_id)
			{
			case ENG_BWOK::en_work_stop		:
				if (!data)	m_pWorkJob = new CWorkAbort();
				else		m_pWorkJob = new CWorkAbort(UINT8(*data));							break;
			case ENG_BWOK::en_work_home		: m_pWorkJob = new CWorkHoming();					break;
			case ENG_BWOK::en_work_init		: m_pWorkJob = new CWorkInited();					break;

			case ENG_BWOK::en_gerb_load		: m_pWorkJob = new CWorkRecipeLoad(UINT8(*data));	break;
			case ENG_BWOK::en_gerb_unload	: m_pWorkJob = new CWorkRecipeUnload();				break;

			case ENG_BWOK::en_gerb_onlyfem	: m_pWorkJob = new CWorkOnlyFEM();					break;

			case ENG_BWOK::en_mark_move		: m_pWorkJob = new CWorkMarkMove(UINT8(*data));		break;
			//case ENG_BWOK::en_mark_test	: m_pWorkJob = new CWorkMarkTest();					break;
			case ENG_BWOK::en_mark_test		: 
			{
				auto workMarkTest = new CWorkMarkTest(LPG_CELA(data));
				m_pWorkJob = static_cast<CWorkStep*>(workMarkTest);
			}
			break;

			//case ENG_BWOK::en_expo_only		: m_pWorkJob = new CWorkExpoOnly(LPG_CPHE(data));	break;
			case ENG_BWOK::en_expo_only		: m_pWorkJob = new CWorkExpoOnly(LPG_CELA(data));	break;
			case ENG_BWOK::en_expo_align	: 
			{
				auto expoAlign = new CWorkExpoAlign();
				m_pWorkJob = static_cast<CWorkStep*>(expoAlign);
			}
			break;
			//case ENG_BWOK::en_expo_align: m_pWorkJob = new CWorkExpoAlign(LPG_CELA(data));

			case ENG_BWOK::en_gerb_expofem	: m_pWorkJob = new CWorkFEMExpo(HWND(data));		break;
			}

			if (!m_pWorkJob)	bSucc = FALSE;
			else
			{
				bSucc = TRUE;
				/* 초기화 작업 수행 */
				if (!m_pWorkJob->InitWork())
				{
					ResetWorkJob();
					bSucc = FALSE;
				}
				else
				{
					/* GUI 첫 화면 갱신 알림 */
					switch (job_id)
					{
					case ENG_BWOK::en_mark_test		:
					case ENG_BWOK::en_expo_align	: SendMesgParent(ENG_BWOK::en_mesg_init, 100);	break;
					}
				}
			}
		}
		/* 동기 해제 */
		m_csSyncWork.Leave();
	}

	return bSucc;
}

/*
 desc : 부모에게 메시지 전달 (SendMessage)
 parm : msg_id	- [in]  메시지 ID (0, 1, 2, 4)
		timeout	- [in]  SendMessag Lifetime (unit: msec)
 retn : None
*/
VOID CMainThread::SendMesgParent(ENG_BWOK msg_id, UINT32 timeout)
{
	LONG lParam			= IsBusyWorkJob() ? 1 : 0;
	DWORD_PTR dwResult	= 0;
	LRESULT lResult		= 0;

	/* 부모에게 이벤트 메시지 전달 */
	lResult	= ::SendMessageTimeout(m_hParent, WM_MAIN_THREAD, WPARAM(msg_id), lParam,
								   SMTO_NORMAL, timeout, &dwResult);
	if (0 == lResult)
	{
#if 0
		/* 현재 발생된 에러 값 저장 */
		TRACE("MainThread : SendMessage Time out <Normal> = %d \n", GetLastError());
#endif
	}
}

/*
 desc : 가장 최근에 Work Job의 ID 결과 값 반환
 parm : None
 retn : ENG_BWOK 값 반환
*/
ENG_BWOK CMainThread::GetWorkJobID()
{
	if (!m_pWorkJob)	return ENG_BWOK::en_work_none;
	return m_pWorkJob->GetWorkJobID();
}

/*
 desc : 현재 작업 동작 이름 (명칭)을 메모리에 임시 저장
 parm : None
 retn : None
*/
VOID CMainThread::UpdateWorkName()
{
	TCHAR tzName[64]	= {NULL}, *ptzState = NULL;
	TCHAR tzState[5][8]	= { L"N", L"W", L"S", L"T", L"F" };
	TCHAR tzWork[WORK_NAME_LEN]		= {NULL};

	/* Scenario 작업 구분 명 */
	if (m_pWorkJob->IsWorkStopped())
	{
		switch (m_pWorkJob->GetWorkJobID())
		{
		case ENG_BWOK::en_work_stop		: wcscpy_s(tzName, 64, L"All Work Stop");		break;
		case ENG_BWOK::en_work_home		: wcscpy_s(tzName, 64, L"All Motor Homing");	break;
		case ENG_BWOK::en_work_init		: wcscpy_s(tzName, 64, L"All Device Init");		break;

		case ENG_BWOK::en_gerb_load		: wcscpy_s(tzName, 64, L"Recipe Load");			break;
		case ENG_BWOK::en_gerb_unload	: wcscpy_s(tzName, 64, L"Recipe Unload");		break;
		case ENG_BWOK::en_gerb_onlyfem	: wcscpy_s(tzName, 64, L"Recipe FEM");		break;

		case ENG_BWOK::en_mark_move		: wcscpy_s(tzName, 64, L"MARK (Move)");			break;
		case ENG_BWOK::en_mark_test		: wcscpy_s(tzName, 64, L"MARK (Test)");			break;

		case ENG_BWOK::en_expo_only		: wcscpy_s(tzName, 64, L"EXPO (Only)");			break;
		case ENG_BWOK::en_expo_align	: wcscpy_s(tzName, 64, L"EXPO (Align)");		break;

		case ENG_BWOK::en_gerb_expofem	: wcscpy_s(tzName, 64, L"EXPO (FEM)");		break;
		}
	}
	/* 현재 작업 상태에 따라, 첫 글자 결정 (진행, 타임아웃, 성공, ..) */
	switch (m_pWorkJob->GetWorkState())
	{
	case ENG_JWNS::en_next	:	ptzState = tzState[0];	break;
	case ENG_JWNS::en_wait	:	ptzState = tzState[1];	break;
	case ENG_JWNS::en_comp	:	ptzState = tzState[2];	break;
	case ENG_JWNS::en_time	:	ptzState = tzState[3];	break;
	case ENG_JWNS::en_error	:	ptzState = tzState[4];	break;
	}

	/* 상세 작업 명 설정 */
	if (ptzState)
	{
		if (!m_pWorkJob->IsWorkCompleted())
		{
			swprintf_s(tzWork, WORK_NAME_LEN, L"[%s] %s : %s",
					   ptzState, tzName, m_pWorkJob->GetStepName());
		}
		else
		{
			swprintf_s(tzWork, WORK_NAME_LEN, L"[%s] %s : Completed", ptzState, tzName);
		}
	}
	uvEng_SetWorkStepName(tzWork);
}

/*
 desc : 현재 DI 장비 내의 Start Button이 눌려졌는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainThread::CheckStartButton()
{
	/* 현재 DI 장비 전면 패널의 노광 버튼 2개가 눌려져 (Push) 있는 상태인지 여부 */
	// by sysandj : MCQ대체 추가 필요
// 	if (0x01 == uvEng_ShMem_GetPLCExt()->r_start_button)
// 	{
// 		m_u8StartButton	= 0x01;
// 	}
	/* 전면 패널의 노광 버튼 2개가 눌려 (Push)진 후, 다시 떼어졌는지 (Pull) 여부 */
	// by sysandj : MCQ대체 추가 필요
	//if (m_u8StartButton == 0x01 && uvEng_ShMem_GetPLCExt()->r_start_button == 0x00)
	{
		m_u8StartButton	= 0x00;

		/* 반드시 이벤트 메시지로 전달 */
		SendMesgParent(ENG_BWOK::en_mesg_expo, 100);

		return TRUE;
	}

	return FALSE;
}

/*
 desc : 현재 작업 중인지 여부
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainThread::IsBusyWorkJob()
{
	if (!m_pWorkJob)	return FALSE;

	return m_pWorkJob->IsWorkStopped() ? FALSE : TRUE;
}

/*
 desc : 검사가 진행된 Align Mark가 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainThread::IsSetAlignedMark()
{
	if (m_pWorkJob->GetUIRefresh())
	{
		m_pWorkJob->SetUIRefresh(false);
		return TRUE;
	}
	else
	return FALSE;
}
/*
 desc : 로그 및 기타 파일 제거
 parm : None
 retn : None
*/
VOID CMainThread::DeleteHistFiles()
{
	TCHAR tzPath[MAX_PATH_LEN]	= {NULL};

	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\save_img\\cali", g_tzWorkDir);
	uvCmn_DeleteHistoryFiles(tzPath, 31);
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\save_img\\edge", g_tzWorkDir);
	uvCmn_DeleteHistoryFiles(tzPath, 31);
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\save_img\\grab", g_tzWorkDir);
	uvCmn_DeleteHistoryFiles(tzPath, 31);

	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\save_img\\mark", g_tzWorkDir);
	uvCmn_DeleteHistoryFiles(tzPath, 31);

	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\save_img\\live", g_tzWorkDir);
	uvCmn_DeleteHistoryFiles(tzPath, 31);


	//swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\logs", g_tzWorkDir);
	//uvCmn_DeleteHistoryFiles(tzPath, 100);

	//swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\temp", g_tzWorkDir);
	//uvCmn_DeleteHistoryFiles(tzPath, 100);
}

/*
 desc : 특정 주기마다 장비 내부의 온도 값을 파일로 저장
 parm : None
 retn : None
*/
VOID CMainThread::SaveTempFiles()
{
	UINT8 i;
	TCHAR tzResult[512]	= {NULL}, tzFile[MAX_PATH_LEN] = {NULL};
	UINT16 (*pLed)[8]	= uvEng_ShMem_GetLuria()->directph.light_source_driver_temp_led;
	UINT16 (*pBoard)[8]	= uvEng_ShMem_GetLuria()->directph.light_source_driver_temp_board;
	SYSTEMTIME stTm		= {NULL};
#if 0
	/* Demo 모드로 동작할 경우 예외 */
	if (uvEng_GetConfig()->IsRunDemo())	return;
#endif
	/* 현재 컴퓨터 날짜를 파일명으로 설정 */
	GetLocalTime(&stTm);
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\logs\\temp\\%04d-%02d-%02d.csv",
			   g_tzWorkDir, stTm.wYear, stTm.wMonth, stTm.wDay);
	/* 만약에 열고자 하는 파일이 없으면, 해당 파일 생성 후, 타이틀 (Field) 추가 */
	if (!uvCmn_FindFile(tzFile))
	{
		wcscpy_s(tzResult, 512,
				 L"DATE,HOT,TEMP1,TEMP2,TEMP3,TEMP4,"
				 L"PLD(11),PLD(12),PLD(13),PLD(14),PBD(11),PBD(12),PBD(13),PBD(14),"
				 L"PLD(21),PLD(22),PLD(23),PLD(24),PBD(21),PBD(22),PBD(23),PBD(24),\n");
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x00);
	}

	/* 발생 시간 / 레시피 이름 / 거버 / 작업 성공 여부 / DI 내부 온도 (4 곳) 등 정보 저장 */
	
	/* 광학계 LED / Board 온도 값 저장 */
	for (i=0x00; i<0x02; i++)
	{
		swprintf_s(tzResult, 512, L"%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,",
				   pLed[i][0]/10.0f,  pLed[i][1]/10.0f,	 pLed[i][2]/10.0f,	pLed[i][3]/10.0f,
				   pBoard[i][0]/2.0f,pBoard[i][1]/2.0f,pBoard[i][2]/2.0f,pBoard[i][3]/2.0f);
		uvCmn_SaveTxtFileW(tzResult, (UINT32)wcslen(tzResult), tzFile, 0x01);
	}
	/* 마지막엔 무조건 다음 라인으로 넘어가도록 하기 위함 */
	uvCmn_SaveTxtFileW(L"\n", (UINT32)wcslen(L"\n"), tzFile, 0x01);
}

/*
 desc : 프로그램 종료 처리
 parm : delay	- [in]  이 시간 (단위: msec) 이후 부모 윈도에게 종료 메시지 알림
 retn : None
*/
VOID CMainThread::SetExitApp(UINT64 delay)
{
	/* 동기 진입 */
	if (m_csSyncWork.Enter())
	{
		m_u64TickExit	= GetTickCount64() + delay;
		m_bAppExit	= TRUE;

		/* 동기 해제 */
		m_csSyncWork.Leave();
	}
}

/*
 설명 : 온조기 온도 자동 제어
 변수 : tick	- [in]  현재 CPU 동작 Time
 반환 : None
*/
VOID CMainThread::SetAutoHotAir(UINT64 tick)
{
	/* 온조기 가동 조건에 부합되는 시간 (분)이 되었는지 확인하기 */
	UINT64 u64TickHold	= UINT64(uvEng_GetConfig()->auto_hot_air.temp_holding_time) * 60 * 1000;	/* min -> msec */
#if 0
	u64TickHold	= 5 * 1000;	/* fixed 5 sec */
#endif
	if (!((tick - m_u64TickHotCtrl) > u64TickHold))	return;

	/* 환경 파일의 DI 내부 온조기 가동을 위한 오차 조건 값 가져오기 */
	DOUBLE dbGetHotRange= uvEng_GetConfig()->auto_hot_air.auto_temp_range;	/* unit: ℃ */
	/* 현재 측정되는 온도 값과 DI 적정 설정 온도 값의 차이 (양수: 온도 상승 의미, 음수: 온도 하락 의미)*/
	DOUBLE dbTempDiffDI = 0; // by sysandj : MCQ대체 추가 필요 : uvEng_MCQ_GetTempDI(0) - uvEng_GetConfig()->temp_range.di_internal[0];
	/* 온조기 가동에 필요한 조건인지 확인 (내부에 설정된 range 값 보다 크면, 온조거 동작 온도 재설정) */
	if (abs(dbTempDiffDI) > dbGetHotRange)
	{
		/* 온조기 가동에 필요한 가동 유지 시간 갱신 (이 시간부터 u64TickHold 시간 동안 여기 수행되지 않음) */
		m_u64TickHotCtrl	= tick;
		/* 온조기 설정할 온도 값 계산하기 (DI 내부 온도와 감시 온도 간의 차이 값에, 설정할 온도 비율 값 곱하기) */
		DOUBLE dbDiffHotAir	= uvEng_GetConfig()->auto_hot_air.auto_temp_rate/100.0f * dbTempDiffDI;
		/* 기존 온조기에 설정된 값에 오차 값을 빼줘야 됨 (!!! 온조기 온도 설정은 반대로 해줘야 됨 !!!) */
		/* DI 내부 온도가 상승하면, 온조기 온도는 내려줘야 되고, DI 내부 온도가 하락하면 온조기 온도는 상승 시켜야 됨 */
		DOUBLE dbSetHotAir = 0;// by sysandj : MCQ대체 추가 필요 : uvEng_MCQ_GetHotAirTempSetting() - dbDiffHotAir;
		/* PLC를 통해, 온조기 제어 온도 값 재설정 (안전하게 15 ~ 30 범위 안에서만 동작하도록 함) */
		if (dbSetHotAir > 20.0f && dbSetHotAir < 30.0f)
		{
			// by sysandj : MCQ대체 추가 필요
			//uvEng_MCQ_SetHotAirTempSetting(dbSetHotAir);
		}
	}
	/* 온조기 가동에 필요한 조건이 아니면... 온도 유지 시간을 초기화 */
	else
	{
		/* 최근 시간으로 갱신 */
		m_u64TickHotCtrl	= tick;
	}
}
