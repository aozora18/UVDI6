
/*
 desc : Windows Child Control ���
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
 desc : �۾� ǥ������ ����(Ʈ���� ������ ����) ���� ���� �˻�
 parm : None
 retn : ���� - �ش� �۾� ǥ������ ���� ���� �ڵ� ��ȯ, ���� - NULL;
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
			// WinXP ������ SysPager ���� ����
			if(hWndSysPager)
				hWndToolbarWindow32 = ::FindWindowEx(hWndSysPager, NULL,_T("ToolbarWindow32"), NULL);
			// Win2000 �� ��쿡�� SysPager �� ���� TrayNotifyWnd -> ToolbarWindow32 �� �Ѿ��
			else
				hWndToolbarWindow32 = ::FindWindowEx(hWndTrayNotifyWnd, NULL,_T("ToolbarWindow32"), NULL);
		}
	}

	return hWndToolbarWindow32;
}

/*
 desc : CEdit ��Ʈ�� (MULTI-LINE, CARRAGE-RETRUN)�� ���ڿ� �߰� (���� ���� �츮�鼭...)
 parm : edit	- [in]  CEditCtrl ��ü ������
		mesg	- [in]  �߰� ���ڿ� ����
		clean	- [in]  ���� ���ڿ� ����� ����
		enter	- [in]  Carriage Return (Line Feed) �� ���� ����
		scroll	- [in]  0x00 - Not used
						0x01 - �� ������ ��ġ�� Scroll �̵�
 retn : None
*/
VOID CWndCtrl::AppendEditMesg(CEdit *edit, PTCHAR mesg, BOOL clean, BOOL enter, UINT8 scroll)
{
	CRect rClient;

	if (!edit)	return;
	if (!edit->GetSafeHwnd())	return;

	/* Edit ���� ũ�� ��� */
	edit->GetClientRect(rClient);

	edit->SetRedraw(FALSE);
	if (clean)
	{
		edit->SetWindowText(mesg);
	}
	else
	{
		/* Ŀ���� ����Ʈ �ڽ� �� ������ �̵� (SetSel (-1, -1)�� ���� ���� ����) */
		INT32 i32Len	= edit->GetWindowTextLength();
		edit->SetSel(i32Len, i32Len);
		if (enter)
		{
			CString strMesg;
			strMesg.Format(L"%s\r\n", mesg);
			edit->ReplaceSel(strMesg);	/* ���Ӱ� ���ڿ� �߰� */
		}
		else		edit->ReplaceSel(mesg);	/* ���Ӱ� ���ڿ� �߰� */
	}
	/* Scroll ���� ���� */
	if (scroll)
	{
		/* �� ������ ��ġ�� �̵� */
		edit->LineScroll(edit->GetLineCount()+1);
	}
	edit->SetRedraw(TRUE);
	edit->Invalidate(TRUE);
}

/*
 desc : ���� ������ ���� ���� ��ġ ������ �̵�
 parm : section	- [in]  ������Ʈ���� Section Name
		hwnd	- [in]  ��ġ�� �����ϰ��� �ϴ� �ڽ��� ���� �ڵ�
 retn : TRUE or FALSE
*/
BOOL CWndCtrl::LoadPlacement(PTCHAR section, HWND hwnd)
{
	INT32 i32Width, i32Height;
	CRect r;
	CWinApp* pApp = AfxGetApp();
	if (!pApp)	return FALSE;

	// ���� ���� ũ�� �� ��ġ ���
	::GetWindowRect(hwnd, r);
	i32Width	= r.right - r.left;
	i32Height	= r.bottom- r.top;

	// ������Ʈ���� ������ ���� ���� ũ�� �� ��ġ ���� ���
	r.left	= pApp->GetProfileInt(section,	_T("left"),		r.left);
	r.top	= pApp->GetProfileInt(section,	_T("top"),		r.top);
	r.right	= pApp->GetProfileInt(section,	_T("right"),	r.right);
	r.bottom= pApp->GetProfileInt(section,	_T("bottom"),	r.bottom);
	// ���� �̵�
	::MoveWindow(hwnd, r.left, r.top, i32Width, i32Height, 0);

	return TRUE;
}

/*
 desc : ���� ���� ��ġ ������ ������Ʈ���� �����ϱ�
 parm : section	- [in]  ������Ʈ���� Section Name
		hwnd	- [in]  ��ġ�� �����ϰ��� �ϴ� �ڽ��� ���� �ڵ�
 retn : TRUE or FALSE
*/
BOOL CWndCtrl::SavePlacement(PTCHAR section, HWND hwnd)
{
	CRect r;
	CWinApp* pApp = AfxGetApp();
	if (!pApp)	return FALSE;

	// ���� ���� ũ�� �� ��ġ ���
	::GetWindowRect(hwnd, r);

	// ������Ʈ���� ����
	pApp->WriteProfileInt(section,	_T("left"),		r.left);
	pApp->WriteProfileInt(section,	_T("top"),		r.top);
	pApp->WriteProfileInt(section,	_T("right"),	r.right);
	pApp->WriteProfileInt(section,	_T("bottom"),	r.bottom);

	return TRUE;
}

/*
 desc : ����Ʈ ��Ʈ���� Ư�� �� ��ġ�� ��ũ�� �̵� 
 parm : ctrl	- [in]  ListControl Handle
		pos		- [in]  �̵� ��ġ (Zero-based)
		select	- [in]  'pos' ��ġ�� �׸� ���� ����
 retn : None
*/
VOID CWndCtrl::MoveScrollPosList(HWND ctrl, UINT32 pos, BOOL select)
{
	INT32 i32Num	= -1, i32ScrollPos = 0;
	POSITION pPos	= NULL;
	CRect rList;
	CSize szHeight;

	/* List Control Object ��� */
	CListCtrl *pCtrl	= (CListCtrl*)CWnd::FromHandle(ctrl);

	/* ���� ��ũ�� ��ġ */
#if 0
	i32ScrollPos	= list->GetScrollPos(SB_VERT);
#else
	i32ScrollPos	= pos;
#endif
	/* List�� ���� �� ���� */
	pPos	= pCtrl->GetFirstSelectedItemPosition();
	if (!pPos)	i32Num	= 0;
	else		i32Num	= pCtrl->GetNextSelectedItem(pPos);
	/* ��ũ�� �̵� */
	pCtrl->GetItemRect(0, rList, LVIR_BOUNDS);
	szHeight	= CSize (0, rList.Height() * i32ScrollPos);
	pCtrl->Scroll(szHeight);	/* ��ũ�� ũ�� �Է� */
	pCtrl->SetScrollPos(SB_VERT, i32ScrollPos, FALSE);
	/* �׸� ���� ���ο� ���� */
	if (select && pos >= 0)
	{
		/* ���� ���� ���� ���� ���� */
		pCtrl->SetItemState(-1, 0, LVIS_SELECTED|LVIS_FOCUSED);
		/* ���ϴ� ������ ���� */
		pCtrl->SetItemState(pos, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		/* ���õ� ������ ǥ�� */
		pCtrl->EnsureVisible(pos, FALSE);
		/* ����Ʈ ��Ʈ�ѿ� ��Ŀ�� ���� */
		pCtrl->SetFocus();
	}
}

/*
 desc : 2�� �̻��� ����͸� ����ϴ� ��� Sub (����) ����ͷ� ���α׷� �̵�
 parm : hwnd	- [in]  ���� �ڵ�
 retn : TRUE or FALSE
*/
BOOL CWndCtrl::MoveSubMonitor(HWND hwnd)
{
	HMONITOR hMonitorThis, hMonitorThat;
	MONITORINFO stMonitorThis, stMonitorThat;
	POINT ptPoint, ptCent;
	RECT rThis;

	/* ���� ������� ������ 1���� ��� ������ FALSE */
	if (::GetSystemMetrics(SM_CMONITORS) < 2)	return FALSE;

	/* �ҽ� ������ ũ�� */
	::GetWindowRect(hwnd, &rThis);
	/* �ҽ� ������ �߽� */
	ptCent.x	= (rThis.right - rThis.left) / 2;
	ptCent.y	= (rThis.bottom - rThis.top) / 2;

	/* ����ü ũ�� ����  */
	stMonitorThis.cbSize	= sizeof(MONITORINFO);
	stMonitorThat.cbSize	= sizeof(MONITORINFO);

	/* ���� ������ ����� ��ġ ���ϱ� */
	hMonitorThis	= ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
	::GetMonitorInfo(hMonitorThis, &stMonitorThis);

	/* Sub ������ ����� ��ġ ���ϱ� */
	if (stMonitorThis.rcMonitor.left != 0)
	{
		/* ���� ����(HWND)�� ���� ����Ϳ� ��ġ�� �ִ� ��� */
		/* ����(HWND)�� ��ġ�� ���� ���� �����(0, 0)�� ���� */
		ptPoint.x	= 0;
		ptPoint.y	= 0;
		hMonitorThat	= ::MonitorFromPoint(ptPoint, MONITOR_DEFAULTTONEAREST);
	}
	else
	{
		/* ���� ����(HWND)�� ���� ����Ϳ� ��ġ�� �ִ� ��� */
		/* ���� ����Ͱ� ���� or �����ʿ� �ִ��� Ȯ���ؾ� �� */
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

	/* ���� �̵� */
	::MoveWindow(hwnd,
				 stMonitorThat.rcMonitor.left + ptCent.x,
				 stMonitorThat.rcMonitor.top + ptCent.y,
				 rThis.right - rThis.left,
				 rThis.bottom - rThis.top, TRUE);

	return TRUE;
}

/*
 desc : Ʈ���� ������ ���� (���� ������ ������ ����)
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

	// Tray�� ������ �ڵ� ���
	hTrayWnd = FindTrayToolbarWindow();
	if (hTrayWnd == NULL)	return FALSE;

	// Tray�� ������ ���ϰ�
	u32TrayCount = (UINT32)::SendMessage(hTrayWnd, TB_BUTTONCOUNT, 0, 0);
	// Tray������ �ڵ��� PID �� ���Ѵ�
	GetWindowThreadProcessId(hTrayWnd, &dwTrayPid);
	// �ش� Tray�� Process �� ��� �޸𸮸� �Ҵ��Ѵ�
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, dwTrayPid);
	if (!hProcess)	return FALSE;
	// �ش� ���μ��� ���� �޸𸮸� �Ҵ�
	lpData = VirtualAllocEx(hProcess, NULL, sizeof (TBBUTTON), MEM_COMMIT, PAGE_READWRITE);
	if (!lpData)	return FALSE;
	// Tray��ŭ ������
	for (i=0; i<u32TrayCount; i++)
	{
		::SendMessage(hTrayWnd, TB_GETBUTTON, i, (LPARAM)lpData);
		// TBBUTTON �� ����ü�� TRAYDATA�� ������ ���
		ReadProcessMemory(hProcess, lpData, (LPVOID)&stTB, sizeof (TBBUTTON), NULL);
		ReadProcessMemory(hProcess, (LPCVOID)stTB.dwData, (LPVOID)&stTIRD, sizeof (stTIRD), NULL);
		// ���� Ʈ������ ���μ��� ��ȣ�� ��
		dwProcessId = 0;
		GetWindowThreadProcessId(stTIRD.hwnd, &dwProcessId);
		// Process �� ���� ��� TrayIcon �� �����Ѵ�
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

	// ���� �޸� ������ ���μ��� �ڵ� �ݱ�
	VirtualFreeEx(hProcess, lpData, NULL, MEM_RELEASE);
	CloseHandle(hProcess);

	return TRUE;
}
