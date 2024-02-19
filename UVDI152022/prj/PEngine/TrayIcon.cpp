#include "pch.h"
#include "TrayIcon.h"

/*
 desc : 생성자
 parm : None
 retn : None
*/
CTrayIcon::CTrayIcon()
{
	m_bShow		= TRUE;
	m_bAdded	= FALSE;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CTrayIcon::~CTrayIcon()
{
}

/*
 desc : 팝업 메뉴 생성
 parm : hwnd	- [in]  다이얼로그 핸들
		pos_x,y	- [in]  팝업 메뉴 생성 위치
 retn : None
*/
VOID CTrayIcon::MakePopupMenu(HWND hwnd, INT32 pos_x, INT32 pos_y)
{
	BOOL bAppend= FALSE;
	HMENU hMenu	= NULL;

	/* 팝업메뉴를생성하고메뉴구성 */
	hMenu = CreatePopupMenu();
	if (!hMenu)	return;

	/* 다이얼로그가 숨겨진 상태인 경우 */
	if(m_bShow)	bAppend	= AppendMenu(hMenu, MF_STRING, WM_MAIN_SHOW_HIDE, _T("HIDE"));
	/* 다이얼로그가 보여진 상태인 경우 */
    else		bAppend	= AppendMenu(hMenu, MF_STRING, WM_MAIN_SHOW_HIDE, _T("SHOW"));
	/* Luria Service 재시작 메뉴 등록 */
	bAppend	= AppendMenu(hMenu, MF_STRING, WM_MAIN_APPS_EXIT, _T("EXIT"));
	/* 다이얼로그 종료 메뉴 등록 */
	bAppend	= AppendMenu(hMenu, MF_STRING, WM_MAIN_LURIA_RESTART, _T("Optic.Restart"));
	/* 생성된 팝업 메뉴 밖을 클릭할때 팝업 닫기 */
	SetForegroundWindow(hwnd);
	/* 팝업메뉴띄우기 */
    TrackPopupMenu(hMenu, TPM_LEFTALIGN|TPM_RIGHTBUTTON, pos_x, pos_y, 0, hwnd, NULL);
}

/*
 desc : 팝업 메뉴의 이벤트 발생 처리
 parm : hwnd	- [in]  다이얼로그 핸들
		wparam	- [in]  Not used
		lparam	- [in]  Mouse Event (LButton, RButton, etc)
 retn : None
*/
VOID CTrayIcon::ProcTrayMsg(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	HMENU hMenu	= NULL;
	POINT ptPos	= {NULL};

	if (lparam == WM_LBUTTONDOWN)
	{
		/* 팝업 메뉴 생성 및 출력 위치 설정 */
		GetCursorPos(&ptPos);
		MakePopupMenu(hwnd, ptPos.x, ptPos.y);
	}
}

/*
 desc : 트레이 아이콘 생성
 parm : hwnd	- [in]  다이얼로그 핸들
		icon	- [in]  Tray Icon Resource ID
 retn : TRUE or FALSE
*/
BOOL CTrayIcon::AddTrayIcon(HWND hwnd, UINT32 icon)
{
	NOTIFYICONDATA stNID	= {NULL};    /* 아이콘을 생성하여 설정 */

	/* 이미 트레이 아이콘이 있다면 종료 */
	if (m_bAdded)	return FALSE;

	/* 트레이이 아이콘에 필요한 정보 설정 */
	ZeroMemory(&stNID, sizeof(NOTIFYICONDATA));
	stNID.cbSize			= sizeof(NOTIFYICONDATA);
	stNID.hWnd				= hwnd;
	stNID.uFlags			= NIF_ICON|NIF_MESSAGE|NIF_TIP;
	stNID.uCallbackMessage	= WM_MAIN_TRAY_ICON;
	wcscpy_s(stNID.szTip, 128, L"Engine for PODIS v1.0.20.0902");
	stNID.uID				= 0;
	stNID.hIcon				= LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(icon));
	/* 트레이 아이콘 추가 */
	if(!Shell_NotifyIcon(NIM_ADD, &stNID))	return FALSE;
	/* Tray Icon 등록 성공  */
	m_bAdded				= TRUE;

	return TRUE;
}

/*
 desc : 트레이 아이콘 제거
 parm : hwnd	- [in]  다이얼로그 핸들
 retn : TRUE or FALSE
*/
BOOL CTrayIcon::DelTrayIcon(HWND hwnd)
{
	NOTIFYICONDATA stNID	= {NULL};    /* 아이콘을 생성하여 설정 */

	/* 트레이이 아이콘에 필요한 정보 설정 */
	ZeroMemory(&stNID, sizeof(NOTIFYICONDATA));
	stNID.cbSize	= sizeof(NOTIFYICONDATA);
	stNID.hWnd		= hwnd;
	stNID.uFlags	= NULL;

	/* 트레이 아이콘 삭제 */
	if(!Shell_NotifyIcon(NIM_DELETE, &stNID))	return FALSE;
	/* Tray Icon 삭제 성공  */
	m_bAdded		= FALSE;

	return TRUE;
}
