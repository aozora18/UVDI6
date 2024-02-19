#pragma once

class CExpoVal
{
/* 생성자 / 파괴자 */
public:
	CExpoVal(HWND *h_wnd, HWND* h_wndPgr);
	virtual ~CExpoVal();


/* 로컬 변수 */
protected:

	UINT32				m_u16TempLed[2][4];		/* 가장 최근의 PH 온도 값 임시 저장 (1 = 0.1˚C) */
	UINT32				m_u16TempBoard[2][4];	/* 가장 최근의 PH 온도 값 임시 저장 (1 = 0.1˚C) */
	UINT32				m_u32JobCount;			/* 가장 최근의 노광한 횟수 저장 */
	UINT32				m_u32TempDI[4];			/* 가장 최근의 DI 온도 값 임시 저장 (1 = 0.001˚C) */
	UINT64				m_u64JobTime;			/* 가장 최근의 작업 시간 저장 */

	//HWND				m_hProc;				/* Operation Process 윈도 핸들 */		=> MainDlg 이동
	//HWND				m_hGerb;				/* Gerber Recipe 윈도 핸들 */			=> MainDlg 이동
	HWND				m_hTact[2];				/* Tact Time 윈도 핸들 */
	HWND				m_hCount;				/* Exposure Count 윈도 핸들 */
	//HWND				m_hMark;				/* Mark Recipe 윈도 핸들 */				=> MainDlg 이동
	HWND				m_hTempDI[4];			/* DI 내부 온도 윈도 핸들 */
	HWND				m_hTempPH[2];			/* PH 내부 온도 윈도 핸들 */
	HWND				m_hRate;				/* 작업 진척도 윈도 핸들 */

	CString				m_strProcess;
/* 로컬 함수 */
protected:

	VOID				DrawValue();
	VOID				DrawText();


/* 공용 함수 */
public:

	/* 부모 스레드에 의해 주기적으로 호출 */
	VOID				DrawRecipe();
	VOID				DrawExpo();
	VOID				ResetData();
};
