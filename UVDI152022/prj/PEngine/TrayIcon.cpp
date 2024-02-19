#include "pch.h"
#include "TrayIcon.h"

/*
 desc : ������
 parm : None
 retn : None
*/
CTrayIcon::CTrayIcon()
{
	m_bShow		= TRUE;
	m_bAdded	= FALSE;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CTrayIcon::~CTrayIcon()
{
}

/*
 desc : �˾� �޴� ����
 parm : hwnd	- [in]  ���̾�α� �ڵ�
		pos_x,y	- [in]  �˾� �޴� ���� ��ġ
 retn : None
*/
VOID CTrayIcon::MakePopupMenu(HWND hwnd, INT32 pos_x, INT32 pos_y)
{
	BOOL bAppend= FALSE;
	HMENU hMenu	= NULL;

	/* �˾��޴��������ϰ�޴����� */
	hMenu = CreatePopupMenu();
	if (!hMenu)	return;

	/* ���̾�αװ� ������ ������ ��� */
	if(m_bShow)	bAppend	= AppendMenu(hMenu, MF_STRING, WM_MAIN_SHOW_HIDE, _T("HIDE"));
	/* ���̾�αװ� ������ ������ ��� */
    else		bAppend	= AppendMenu(hMenu, MF_STRING, WM_MAIN_SHOW_HIDE, _T("SHOW"));
	/* Luria Service ����� �޴� ��� */
	bAppend	= AppendMenu(hMenu, MF_STRING, WM_MAIN_APPS_EXIT, _T("EXIT"));
	/* ���̾�α� ���� �޴� ��� */
	bAppend	= AppendMenu(hMenu, MF_STRING, WM_MAIN_LURIA_RESTART, _T("Optic.Restart"));
	/* ������ �˾� �޴� ���� Ŭ���Ҷ� �˾� �ݱ� */
	SetForegroundWindow(hwnd);
	/* �˾��޴����� */
    TrackPopupMenu(hMenu, TPM_LEFTALIGN|TPM_RIGHTBUTTON, pos_x, pos_y, 0, hwnd, NULL);
}

/*
 desc : �˾� �޴��� �̺�Ʈ �߻� ó��
 parm : hwnd	- [in]  ���̾�α� �ڵ�
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
		/* �˾� �޴� ���� �� ��� ��ġ ���� */
		GetCursorPos(&ptPos);
		MakePopupMenu(hwnd, ptPos.x, ptPos.y);
	}
}

/*
 desc : Ʈ���� ������ ����
 parm : hwnd	- [in]  ���̾�α� �ڵ�
		icon	- [in]  Tray Icon Resource ID
 retn : TRUE or FALSE
*/
BOOL CTrayIcon::AddTrayIcon(HWND hwnd, UINT32 icon)
{
	NOTIFYICONDATA stNID	= {NULL};    /* �������� �����Ͽ� ���� */

	/* �̹� Ʈ���� �������� �ִٸ� ���� */
	if (m_bAdded)	return FALSE;

	/* Ʈ������ �����ܿ� �ʿ��� ���� ���� */
	ZeroMemory(&stNID, sizeof(NOTIFYICONDATA));
	stNID.cbSize			= sizeof(NOTIFYICONDATA);
	stNID.hWnd				= hwnd;
	stNID.uFlags			= NIF_ICON|NIF_MESSAGE|NIF_TIP;
	stNID.uCallbackMessage	= WM_MAIN_TRAY_ICON;
	wcscpy_s(stNID.szTip, 128, L"Engine for PODIS v1.0.20.0902");
	stNID.uID				= 0;
	stNID.hIcon				= LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(icon));
	/* Ʈ���� ������ �߰� */
	if(!Shell_NotifyIcon(NIM_ADD, &stNID))	return FALSE;
	/* Tray Icon ��� ����  */
	m_bAdded				= TRUE;

	return TRUE;
}

/*
 desc : Ʈ���� ������ ����
 parm : hwnd	- [in]  ���̾�α� �ڵ�
 retn : TRUE or FALSE
*/
BOOL CTrayIcon::DelTrayIcon(HWND hwnd)
{
	NOTIFYICONDATA stNID	= {NULL};    /* �������� �����Ͽ� ���� */

	/* Ʈ������ �����ܿ� �ʿ��� ���� ���� */
	ZeroMemory(&stNID, sizeof(NOTIFYICONDATA));
	stNID.cbSize	= sizeof(NOTIFYICONDATA);
	stNID.hWnd		= hwnd;
	stNID.uFlags	= NULL;

	/* Ʈ���� ������ ���� */
	if(!Shell_NotifyIcon(NIM_DELETE, &stNID))	return FALSE;
	/* Tray Icon ���� ����  */
	m_bAdded		= FALSE;

	return TRUE;
}
