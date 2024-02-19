
#pragma once

#include "../../inc/base/DlgChild.h"

class CDlgTrig : public CDlgChild
{
public:

	CDlgTrig(UINT32 id, CWnd *parent=NULL);
	virtual ~CDlgTrig();


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

	CMyGrpBox			m_grp_ctl[3];
	CMyStatic			m_txt_ctl[1];
	CMyCombo			m_cmb_ctl[1];

	CGridCtrl			*m_pGrid[3];	/* Quardrature Register / Trigger Setup / Trigger Position */

/* ���� �Լ� */
protected:

	BOOL				InitCtrl();
	BOOL				InitGrid();

	VOID				UpdateTrigValue();


/* ���� �Լ� */
public:



/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()

};
