
#pragma once

#include "DlgMenu.h"

class CDlgLive : public CDlgMenu
{
public:

	CDlgLive(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgLive();


/* 가상함수 */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();

public:

	virtual VOID		UpdateCtrl();
	virtual VOID		ResetData()	{};


/* 로컬 변수 */
protected:

	BOOL				m_bLiveEnable;
	HWND				m_hWndLive;

	CMyStatic			m_pic_ctl[1];


/* 로컬 함수 */
protected:

	VOID				InitCtrl();

	VOID				InvalidateView();
	VOID				UpdateLiveView();


/* 공용 함수 */
public:

	/* Live 출력 여부 */
	VOID				SetLive(BOOL enable)	{	m_bLiveEnable	= enable;	}

/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg VOID		OnShowWindow(BOOL show, UINT32 status);
};
