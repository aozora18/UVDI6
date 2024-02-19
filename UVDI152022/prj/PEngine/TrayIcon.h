#pragma once

class CTrayIcon : public CObject
{
/* 생성자 & 파괴자 */
public:

	CTrayIcon();
	~CTrayIcon();

/* 로컬 변수 */
protected:

	BOOL				m_bAdded;			/* 트레이 아이콘이 생성되었는지 여부*/
	BOOL				m_bShow;			/* 다이얼로그가 감춰진 상태인지 여부 */


/* 로컬 함수 */
protected:

	VOID				MakePopupMenu(HWND hwnd, INT32 pos_x, INT32 pos_y);


/* 공용 함수 */
public:

	BOOL				AddTrayIcon(HWND hwnd, UINT32 icon);
	BOOL				DelTrayIcon(HWND hwnd);
	VOID				ProcTrayMsg(HWND hwnd, WPARAM wparam, LPARAM lparam);

	VOID				TrayShow(BOOL show)	{	m_bShow	= show;	}

	BOOL				IsTrayShow()		{	return m_bShow;	}
};

