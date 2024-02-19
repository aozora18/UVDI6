
/*
 desc : Windows Child Control 모듈
*/

#include "pch.h"
#include "WndCtrl.h"


#ifdef _DEBUG
#define new	DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : Constructor
 parm : None
 retn : None
*/
CWndCtrl::CWndCtrl()
{
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CWndCtrl::~CWndCtrl()
{
}

/*
 desc : 작업 표시줄의 툴바(트레이 아이콘 영역) 영역 윈도 검색
 parm : None
 retn : 성공 - 해당 작업 표시줄의 툴바 윈도 핸들 반환, 실패 - NULL;
*/
static HWND FindTrayToolbarWindow()
{
	HWND hWndToolbarWindow32= NULL;
	HWND hWndShellTrayWnd	= NULL;
	HWND hWndTrayNotifyWnd	= NULL;
	HWND hWndSysPager		= NULL;

	hWndShellTrayWnd = ::FindWindow(_T("Shell_TrayWnd"), NULL);
	if(hWndShellTrayWnd)
	{
		hWndTrayNotifyWnd = ::FindWindowEx(hWndShellTrayWnd,NULL,_T("TrayNotifyWnd"), NULL);
		if(hWndTrayNotifyWnd)
		{
			hWndSysPager = ::FindWindowEx(hWndTrayNotifyWnd,NULL,_T("SysPager"), NULL); //WinXP
			// WinXP 에서는 SysPager 까지 추적
			if(hWndSysPager)
				hWndToolbarWindow32 = ::FindWindowEx(hWndSysPager, NULL,_T("ToolbarWindow32"), NULL);
			// Win2000 일 경우에는 SysPager 가 없이 TrayNotifyWnd -> ToolbarWindow32 로 넘어간다
			else
				hWndToolbarWindow32 = ::FindWindowEx(hWndTrayNotifyWnd, NULL,_T("ToolbarWindow32"), NULL);
		}
	}

	return hWndToolbarWindow32;
}

/*
 desc : CEdit 컨트롤 (MULTI-LINE, CARRAGE-RETRUN)에 문자열 추가 (기존 내용 살리면서...)
 parm : edit	- [in]  CEditCtrl 객체 포인터
		mesg	- [in]  추가 문자열 버퍼
		clean	- [in]  기존 문자열 지우기 여부
		enter	- [in]  Carriage Return (Line Feed) 값 적용 여부
		scroll	- [in]  0x00 - Not used
						0x01 - 맨 마지막 위치로 Scroll 이동
 retn : None
*/
VOID CWndCtrl::AppendEditMesg(CEdit *edit, PTCHAR mesg, BOOL clean, BOOL enter, UINT8 scroll)
{
	CRect rClient;

	if (!edit)	return;
	if (!edit->GetSafeHwnd())	return;

	/* Edit 영역 크기 얻기 */
	edit->GetClientRect(rClient);

	edit->SetRedraw(FALSE);
	if (clean)
	{
		edit->SetWindowText(mesg);
	}
	else
	{
		/* 커서를 에디트 박스 맨 끝으로 이동 (SetSel (-1, -1)은 선택 영역 해제) */
		INT32 i32Len	= edit->GetWindowTextLength();
		edit->SetSel(i32Len, i32Len);
		if (enter)
		{
			CString strMesg;
			strMesg.Format(L"%s\r\n", mesg);
			edit->ReplaceSel(strMesg);	/* 새롭게 문자열 추가 */
		}
		else		edit->ReplaceSel(mesg);	/* 새롭게 문자열 추가 */
	}
	/* Scroll 정보 설정 */
	if (scroll)
	{
		/* 맨 마지막 위치로 이동 */
		edit->LineScroll(edit->GetLineCount()+1);
	}
	edit->SetRedraw(TRUE);
	edit->Invalidate(TRUE);
}

/*
 desc : 기존 저장해 놓은 윈도 위치 정보로 이동
 parm : section	- [in]  레지스트리의 Section Name
		hwnd	- [in]  위치를 저장하고자 하는 자신의 윈도 핸들
 retn : TRUE or FALSE
*/
BOOL CWndCtrl::LoadPlacement(PTCHAR section, HWND hwnd)
{
	INT32 i32Width, i32Height;
	CRect r;
	CWinApp* pApp = AfxGetApp();
	if (!pApp)	return FALSE;

	// 현재 윈도 크기 및 위치 얻기
	::GetWindowRect(hwnd, r);
	i32Width	= r.right - r.left;
	i32Height	= r.bottom- r.top;

	// 레지스트리에 저장해 놓은 윈도 크기 및 위치 정보 얻기
	r.left	= pApp->GetProfileInt(section,	_T("left"),		r.left);
	r.top	= pApp->GetProfileInt(section,	_T("top"),		r.top);
	r.right	= pApp->GetProfileInt(section,	_T("right"),	r.right);
	r.bottom= pApp->GetProfileInt(section,	_T("bottom"),	r.bottom);
	// 윈도 이동
	::MoveWindow(hwnd, r.left, r.top, i32Width, i32Height, 0);

	return TRUE;
}

/*
 desc : 현재 윈도 위치 정보를 레지스트리에 저장하기
 parm : section	- [in]  레지스트리의 Section Name
		hwnd	- [in]  위치를 저장하고자 하는 자신의 윈도 핸들
 retn : TRUE or FALSE
*/
BOOL CWndCtrl::SavePlacement(PTCHAR section, HWND hwnd)
{
	CRect r;
	CWinApp* pApp = AfxGetApp();
	if (!pApp)	return FALSE;

	// 현재 윈도 크기 및 위치 얻기
	::GetWindowRect(hwnd, r);

	// 레지스트리에 저장
	pApp->WriteProfileInt(section,	_T("left"),		r.left);
	pApp->WriteProfileInt(section,	_T("top"),		r.top);
	pApp->WriteProfileInt(section,	_T("right"),	r.right);
	pApp->WriteProfileInt(section,	_T("bottom"),	r.bottom);

	return TRUE;
}

/*
 desc : 리스트 컨트롤의 특정 행 위치로 스크롤 이동 
 parm : ctrl	- [in]  ListControl Handle
		pos		- [in]  이동 위치 (Zero-based)
		select	- [in]  'pos' 위치의 항목 선택 여부
 retn : None
*/
VOID CWndCtrl::MoveScrollPosList(HWND ctrl, UINT32 pos, BOOL select)
{
	INT32 i32Num	= -1, i32ScrollPos = 0;
	POSITION pPos	= NULL;
	CRect rList;
	CSize szHeight;

	/* List Control Object 얻기 */
	CListCtrl *pCtrl	= (CListCtrl*)CWnd::FromHandle(ctrl);

	/* 현재 스크롤 위치 */
#if 0
	i32ScrollPos	= list->GetScrollPos(SB_VERT);
#else
	i32ScrollPos	= pos;
#endif
	/* List의 현재 행 추출 */
	pPos	= pCtrl->GetFirstSelectedItemPosition();
	if (!pPos)	i32Num	= 0;
	else		i32Num	= pCtrl->GetNextSelectedItem(pPos);
	/* 스크롤 이동 */
	pCtrl->GetItemRect(0, rList, LVIR_BOUNDS);
	szHeight	= CSize (0, rList.Height() * i32ScrollPos);
	pCtrl->Scroll(szHeight);	/* 스크롤 크기 입력 */
	pCtrl->SetScrollPos(SB_VERT, i32ScrollPos, FALSE);
	/* 항목 선택 여부에 따라 */
	if (select && pos >= 0)
	{
		/* 먼저 현재 선택 상태 해제 */
		pCtrl->SetItemState(-1, 0, LVIS_SELECTED|LVIS_FOCUSED);
		/* 원하는 아이템 선택 */
		pCtrl->SetItemState(pos, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		/* 선택된 아이템 표시 */
		pCtrl->EnsureVisible(pos, FALSE);
		/* 리스트 컨트롤에 포커스 설정 */
		pCtrl->SetFocus();
	}
}

/*
 desc : 2개 이상의 모니터를 사용하는 경우 Sub (보조) 모니터로 프로그램 이동
 parm : hwnd	- [in]  윈도 핸들
 retn : TRUE or FALSE
*/
BOOL CWndCtrl::MoveSubMonitor(HWND hwnd)
{
	HMONITOR hMonitorThis, hMonitorThat;
	MONITORINFO stMonitorThis, stMonitorThat;
	POINT ptPoint, ptCent;
	RECT rThis;

	/* 만약 모니터의 개수가 1개인 경우 무조건 FALSE */
	if (::GetSystemMetrics(SM_CMONITORS) < 2)	return FALSE;

	/* 소스 윈도의 크기 */
	::GetWindowRect(hwnd, &rThis);
	/* 소스 윈도의 중심 */
	ptCent.x	= (rThis.right - rThis.left) / 2;
	ptCent.y	= (rThis.bottom - rThis.top) / 2;

	/* 구조체 크기 설정  */
	stMonitorThis.cbSize	= sizeof(MONITORINFO);
	stMonitorThat.cbSize	= sizeof(MONITORINFO);

	/* 현재 윈도의 모니터 위치 구하기 */
	hMonitorThis	= ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
	::GetMonitorInfo(hMonitorThis, &stMonitorThis);

	/* Sub 윈도의 모니터 위치 구하기 */
	if (stMonitorThis.rcMonitor.left != 0)
	{
		/* 현재 윈도(HWND)가 서브 모니터에 위치해 있는 경우 */
		/* 윈도(HWND)가 위치할 곳을 메인 모니터(0, 0)로 설정 */
		ptPoint.x	= 0;
		ptPoint.y	= 0;
		hMonitorThat	= ::MonitorFromPoint(ptPoint, MONITOR_DEFAULTTONEAREST);
	}
	else
	{
		/* 현재 윈도(HWND)가 메인 모니터에 위치해 있는 경우 */
		/* 보조 모니터가 왼쪽 or 오른쪽에 있는지 확인해야 함 */
		ptPoint.x	= -1;
		ptPoint.y	= 0;
		hMonitorThat	= ::MonitorFromPoint(ptPoint, MONITOR_DEFAULTTONULL);
		if (hMonitorThat <= 0)
		{
			ptPoint.x	= stMonitorThis.rcMonitor.right + 1;
			ptPoint.y	= 0;
			hMonitorThat= ::MonitorFromPoint(ptPoint, MONITOR_DEFAULTTONULL);
		}
	}
	::GetMonitorInfo(hMonitorThat, &stMonitorThat);

	/* 윈도 이동 */
	::MoveWindow(hwnd,
				 stMonitorThat.rcMonitor.left + ptCent.x,
				 stMonitorThat.rcMonitor.top + ptCent.y,
				 rThis.right - rThis.left,
				 rThis.bottom - rThis.top, TRUE);

	return TRUE;
}

/*
 desc : 트레이 아이콘 갱신 (기존 생성된 아이콘 제거)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWndCtrl::RefreshTrayIcon()
{
	HANDLE hProcess;
	LPVOID lpData;
	TBBUTTON stTB;
	STG_TIRD stTIRD	= {NULL};
	DWORD dwTrayPid, dwProcessId;
	UINT32 i, u32TrayCount;
	HWND hTrayWnd	= NULL;
	NOTIFYICONDATA stNID;

	// Tray의 윈도우 핸들 얻기
	hTrayWnd = FindTrayToolbarWindow();
	if (hTrayWnd == NULL)	return FALSE;

	// Tray의 개수를 구하고
	u32TrayCount = (UINT32)::SendMessage(hTrayWnd, TB_BUTTONCOUNT, 0, 0);
	// Tray윈도우 핸들의 PID 를 구한다
	GetWindowThreadProcessId(hTrayWnd, &dwTrayPid);
	// 해당 Tray의 Process 를 열어서 메모리를 할당한다
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, dwTrayPid);
	if (!hProcess)	return FALSE;
	// 해당 프로세스 내에 메모리를 할당
	lpData = VirtualAllocEx(hProcess, NULL, sizeof (TBBUTTON), MEM_COMMIT, PAGE_READWRITE);
	if (!lpData)	return FALSE;
	// Tray만큼 뺑뺑이
	for (i=0; i<u32TrayCount; i++)
	{
		::SendMessage(hTrayWnd, TB_GETBUTTON, i, (LPARAM)lpData);
		// TBBUTTON 의 구조체와 TRAYDATA의 내용을 얻기
		ReadProcessMemory(hProcess, lpData, (LPVOID)&stTB, sizeof (TBBUTTON), NULL);
		ReadProcessMemory(hProcess, (LPCVOID)stTB.dwData, (LPVOID)&stTIRD, sizeof (stTIRD), NULL);
		// 각각 트레이의 프로세스 번호를 얻어서
		dwProcessId = 0;
		GetWindowThreadProcessId(stTIRD.hwnd, &dwProcessId);
		// Process 가 없는 경우 TrayIcon 을 삭제한다
		if (dwProcessId == 0)
		{
			stNID.cbSize			= sizeof(NOTIFYICONDATA);
			stNID.hIcon				= stTIRD.hicon;
			stNID.hWnd				= stTIRD.hwnd;
			stNID.uCallbackMessage	= stTIRD.cb_msg;
			stNID.uID				= stTIRD.id;
			Shell_NotifyIcon(NIM_DELETE, &stNID);
		}
	}

	// 가상 메모리 해제와 프로세스 핸들 닫기
	VirtualFreeEx(hProcess, lpData, NULL, MEM_RELEASE);
	CloseHandle(hProcess);

	return TRUE;
}
