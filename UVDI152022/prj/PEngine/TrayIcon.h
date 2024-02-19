#pragma once

class CTrayIcon : public CObject
{
/* ������ & �ı��� */
public:

	CTrayIcon();
	~CTrayIcon();

/* ���� ���� */
protected:

	BOOL				m_bAdded;			/* Ʈ���� �������� �����Ǿ����� ����*/
	BOOL				m_bShow;			/* ���̾�αװ� ������ �������� ���� */


/* ���� �Լ� */
protected:

	VOID				MakePopupMenu(HWND hwnd, INT32 pos_x, INT32 pos_y);


/* ���� �Լ� */
public:

	BOOL				AddTrayIcon(HWND hwnd, UINT32 icon);
	BOOL				DelTrayIcon(HWND hwnd);
	VOID				ProcTrayMsg(HWND hwnd, WPARAM wparam, LPARAM lparam);

	VOID				TrayShow(BOOL show)	{	m_bShow	= show;	}

	BOOL				IsTrayShow()		{	return m_bShow;	}
};

