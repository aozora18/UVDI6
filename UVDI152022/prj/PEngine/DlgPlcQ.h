
#pragma once

#include "../../inc/base/DlgChild.h"

class CDlgPlcQ : public CDlgChild
{
public:

	CDlgPlcQ(UINT32 id, CWnd *parent=NULL);
	virtual ~CDlgPlcQ();

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


/* ���� ���� */
protected:

	CGridCtrl			*m_pGrid;

/* ���� �Լ� */
protected:

	BOOL				InitCtrl();
	BOOL				InitGrid();

	VOID				UpdatePLCValue();


/* ���� �Լ� */
public:



/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg VOID		OnGridClicked();
};
