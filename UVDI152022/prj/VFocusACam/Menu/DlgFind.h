
#pragma once

#include "DlgMenu.h"

class CDlgFind: public CDlgMenu
{
public:

	CDlgFind(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgFind();

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

	CMacButton			m_btn_ctl[3];

	CGridCtrl			*m_pGrid;


/* ���� �Լ� */
protected:

	VOID				InitCtrl();
	VOID				InitGrid();

	VOID				GetEdgeData();
	VOID				SaveToOpen();


/* ���� �Լ� */
public:

	VOID				RefreshData();


/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg VOID		OnGridClicked();
	afx_msg VOID		OnBnClicked(UINT32 id);
};
