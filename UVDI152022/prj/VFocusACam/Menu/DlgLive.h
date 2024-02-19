
#pragma once

#include "DlgMenu.h"

class CDlgLive : public CDlgMenu
{
public:

	CDlgLive(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgLive();


/* �����Լ� */
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


/* ���� ���� */
protected:

	BOOL				m_bLiveEnable;
	HWND				m_hWndLive;

	CMyStatic			m_pic_ctl[1];


/* ���� �Լ� */
protected:

	VOID				InitCtrl();

	VOID				InvalidateView();
	VOID				UpdateLiveView();


/* ���� �Լ� */
public:

	/* Live ��� ���� */
	VOID				SetLive(BOOL enable)	{	m_bLiveEnable	= enable;	}

/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg VOID		OnShowWindow(BOOL show, UINT32 status);
};
