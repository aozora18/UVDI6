
#pragma once

#include "DlgMenu.h"

class CDlgEdge : public CDlgMenu
{
public:

	CDlgEdge(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgEdge();

/* ����ü / ����ü */
protected:


/* ���� �Լ� */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();

public:

	virtual VOID		UpdateCtrl();
	virtual VOID		ResetData();


/* ���� ���� */
protected:

	CMyStatic			m_pic_ctl[1];


/* ���� �Լ� */
protected:

	VOID				InitCtrl();

	VOID				DrawEdge();


/* ���� �Լ� */
public:



/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg VOID		OnShowWindow(BOOL show, UINT32 status);
};
