
/*
 desc : Expo 화면의 컨트롤 영역에 실시간으로 값 갱신
 */

#include "pch.h"
#include "../../MainApp.h"
#include "ExpoVal.h"

#include "../../../../inc/comn/MyStatic.h"
#include "../../../../inc/comn/MacProgress.h"

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : 생성자
 parm : h_wnd	- [in]  각종 출력 대상의 윈도 핸들 포인터
 retn : None
 note : h_proc	- [in]  작업 동작 정보 출력 대상의 윈도 핸들
		h_gerb	- [in]  레시피 출력 대상의 윈도 핸들
		h_tact	- [in]  Tact Time 정보 출력 대상의 윈도 핸들
		h_count	- [in]  노광 작업 횟수 출력 대상의 윈도 핸들
		h_temp	- [in]  DI 내부 온도 출력 대상의 윈도 핸들
		h_mark	- [in]  Mark Recipe 출력 윈도 핸들
		h_rate	- [in]  전체 작업 진행률 출력 대상의 윈도 핸들
*/
CExpoVal::CExpoVal(HWND *h_wnd, HWND* h_wndPgr)
{
	//m_hProc		= h_wnd[0];	// by sys&j : MainDlg로 이동
	//m_hGerb		= h_wnd[1];	// by sys&j : MainDlg로 이동
	m_hTact[0]	= h_wnd[0];
	m_hTact[1]	= h_wnd[1];
	m_hCount	= h_wnd[2];
	//m_hMark		= h_wnd[5];	// by sys&j : MainDlg로 이동
	m_hTempDI[0]= h_wnd[3];
	m_hTempDI[1]= h_wnd[4];
	m_hTempDI[2]= h_wnd[5];
	m_hTempDI[3]= h_wnd[6];
	m_hTempPH[0]= h_wnd[7];
	m_hTempPH[1]= h_wnd[8];
	m_hRate		= h_wndPgr[0];

	/* 내부 멤버 변수 초기화 */
	memset(m_u32TempDI,			UINT32_MAX, sizeof(UINT32) * 4);
	memset(m_u16TempLed[0],		UINT16_MAX, sizeof(UINT16) * 4/* Max LED*/);
	memset(m_u16TempLed[1],		UINT16_MAX, sizeof(UINT16) * 4/* Max LED*/);
	memset(m_u16TempBoard[0],	UINT16_MAX, sizeof(UINT16) * 4/* Max LED*/);
	memset(m_u16TempBoard[1],	UINT16_MAX, sizeof(UINT16) * 4/* Max LED*/);
	m_u64JobTime	= UINT64_MAX;
	m_u32JobCount	= UINT32_MAX;

	/* Progress 범위 설정 */
	// by sysandj : 외부로 뺌
	// CMacProgress *pRate	= (CMacProgress*)CWnd::FromHandle(m_hRate);
	// if (pRate)	pRate->SetRange(0, 100);
	// by sysandj : 외부로 뺌
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CExpoVal::~CExpoVal()
{
}

/*
 desc : 초기화
 parm : None
 retn : None
*/
VOID CExpoVal::ResetData()
{
	m_u64JobTime	= UINT64_MAX;
	m_u32JobCount	= UINT32_MAX;
}

/*
 desc : 초기화 작업
 parm : None
 retn : None
*/
VOID CExpoVal::DrawRecipe()
{
	CMyStatic *pText	= NULL;
	LPG_RJAF pstJob		= uvEng_JobRecipe_GetSelectRecipe();
	LPG_REAF pstExpo	= uvEng_ExpoRecipe_GetSelectRecipe();
	LPG_RAAF pstMark	= uvEng_Mark_GetSelectAlignRecipe();
	CUniToChar csCnv;

	CString strReicpe, strExpo, strAlign;
	/* 거버 레시피 출력 */
	//pText	= (CMyStatic *)CWnd::FromHandle(m_hGerb);
	//if (!pstGerb)	pText->SetWindowTextW(L"No recipe selected");
	//else			pText->SetWindowTextW(csCnv.Ansi2Uni(pstGerb->recipe_name));
	//pText->Invalidate(TRUE);
	if (!pstJob)	strReicpe = L"No recipe selected";
	else			strReicpe = csCnv.Ansi2Uni(pstJob->job_name);

	if (!pstJob)	strExpo = L"No recipe selected";
	else			strExpo = csCnv.Ansi2Uni(pstJob->expo_recipe);

	if (!pstJob)	strAlign = L"No recipe selected";
	else			strAlign = csCnv.Ansi2Uni(pstJob->align_recipe);

	::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_MAIN_RECIPE_UPDATE, (WPARAM)0, (LPARAM)&strReicpe);
	::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_MAIN_RECIPE_UPDATE, (WPARAM)1, (LPARAM)&strExpo);
	::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_MAIN_RECIPE_UPDATE, (WPARAM)2, (LPARAM)&strAlign);
}

/*
 desc : Expo 화면에 실시간으로 출력되는 값 표현
 parm : None
 retn : None
*/
VOID CExpoVal::DrawExpo()
{
	DrawText();
	DrawValue();
}

/*
 desc : 실시간 값 (문자) 표현
 parm : None
 retn : None
*/
VOID CExpoVal::DrawText()
{
	UINT32 u32JobCount	= uvEng_GetJobWorkCount();
	UINT64 u64JobTime	= uvEng_GetJobWorkTime();
	UINT64 u64JobTimeAvg= uvEng_GetJobWorkTimeAverage();
	TCHAR tzName[128]	= {NULL}, tzProcName[WORK_NAME_LEN] = {NULL};
	PTCHAR ptzProcName	= NULL;
	CMyStatic *pText	= NULL;
	CMacProgress *pRate	= (CMacProgress*)CWnd::FromHandle(m_hRate);

	/* 현재 작업 진행률 */
	// by sysandj : 외부로 뺌
	/*UINT8 u8Rate = (UINT8)ROUNDED(uvEng_GetWorkStepRate(), 0);
	pRate->SetPos(u8Rate);*/
	// by sysandj : 외부로 뺌
	
	/* 현재 처리 중인 작업 이름 갱신 */		// by sys&j : MainDlg로 이동
	//pText	= (CMyStatic *)CWnd::FromHandle(m_hProc);
	//pText->GetWindowTextW(tzProcName, WORK_NAME_LEN);
	ptzProcName	= uvEng_GetWorkStepName();
	if (wcslen(ptzProcName) < 1)
	{
		if (0 != wcscmp(L"Ready", ptzProcName))
		{
			//pText->SetWindowTextW(L"Ready");
			//pText->Invalidate(TRUE);
			m_strProcess = L"Ready";
			::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_MAIN_PROCESS_UPDATE, (WPARAM)&m_strProcess, NULL);
		}
	}
	else
	{
		if (0 != wcscmp(m_strProcess, ptzProcName))
		{
			//pText->SetWindowTextW(ptzProcName);
			//pText->Invalidate(TRUE);
			m_strProcess.Format(_T("%s"), ptzProcName);
			::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_MAIN_PROCESS_UPDATE, (WPARAM)&m_strProcess, NULL);
		}
	}
	/* 노광 횟수 갱신 */
	if (m_u32JobCount != u32JobCount)
	{
		m_u32JobCount	= u32JobCount;
		swprintf_s(tzName, 128, L"%u ea", u32JobCount);
		((CMyStatic*)CWnd::FromHandle(m_hCount))->SetWindowTextW(tzName);
	}
	/* Tact Time 갱신 */
	if (m_u64JobTime != u64JobTime)
	{
		/* 가장 최근 시간 저장 */
		m_u64JobTime	= u64JobTime;

		/* 가장 최근 시간 출력 */
		swprintf_s(tzName, 128, L"Last : %u m %02u s",
				   uvCmn_GetTimeToType(u64JobTime, 0x01),
				   uvCmn_GetTimeToType(u64JobTime, 0x02));
		((CMyStatic*)CWnd::FromHandle(m_hTact[0]))->SetWindowTextW(tzName);

		/* 전체 평균 시간 출력 */
		swprintf_s(tzName, 128, L"Avgs : %u m %02u s",
				   uvCmn_GetTimeToType(u64JobTimeAvg, 0x01),
				   uvCmn_GetTimeToType(u64JobTimeAvg, 0x02));
		((CMyStatic*)CWnd::FromHandle(m_hTact[1]))->SetWindowTextW(tzName);
	}
}

/*
 desc : 실시간 값 (숫자) 표현
 parm : None
 retn : None
*/
VOID CExpoVal::DrawValue()
{
	TCHAR tzValue[32]	= {NULL};
	UINT16 u16TempLed, u16TempBoard;
	PUINT16 pPhTempLed	= NULL;
	PUINT16 pPhTempBoard= NULL;
	UINT32 u32PLCAddr[4]= { UINT32(ENG_PIOA::en_internal_monitoring_temp),
							UINT32(ENG_PIOA::en_internal_monitoring_temp_2_only_4th),
							UINT32(ENG_PIOA::en_internal_monitoring_temp_3_only_4th),
							UINT32(ENG_PIOA::en_internal_monitoring_temp_4_only_4th) };
	UINT32 i = 0;

	/* DI 내부 온도 출력 */
	// by sysandj : MCQ대체 추가 필요	

	/* PH 내부 온도 출력 */
	for (i=0; i<2; i++)
	{
		pPhTempLed	= uvEng_ShMem_GetLuria()->directph.light_source_driver_temp_led[i];
		pPhTempBoard= uvEng_ShMem_GetLuria()->directph.light_source_driver_temp_board[i];
		if (0 !=memcmp(pPhTempLed, m_u16TempLed[i], sizeof(UINT16) * 4) ||
			0 !=memcmp(pPhTempBoard, m_u16TempBoard[i], sizeof(UINT16) * 4))
		{
			/* 가장 최신 값으로 갱신 */
			memcpy(m_u16TempLed[i], pPhTempLed, sizeof(UINT16) * 4);
			/* 현재 광학계의 가장 높은 온도를 가진 Led와 Board 값 얻기 */
			uvEng_ShMem_GetLuria()->directph.GetMaxTempLedBD(i+1, u16TempLed, u16TempBoard);
			swprintf_s(tzValue, 32, L"L:%3.1f / B:%3.1f °C", u16TempLed/10.0f, u16TempBoard/2.0f);
			((CMyStatic*)CWnd::FromHandle(m_hTempPH[i]))->SetTextToStr(tzValue);
		}
	}
}
