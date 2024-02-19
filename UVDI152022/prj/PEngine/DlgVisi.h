
#pragma once

#include "../../inc/base/DlgChild.h"

class CDlgVisi : public CDlgChild
{
public:

	CDlgVisi(UINT32 id, CWnd *parent=NULL);
	virtual ~CDlgVisi();


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

	CMyGrpBox			m_grp_ctl[4];
	CMyStatic			m_txt_ctl[24];
	CMyCombo			m_cmb_ctl[4];
	CMyEdit				m_edt_ctl[21];


/* ���� �Լ� */
protected:

	VOID				InitCtrl();

	VOID				UpdateMoni();


/* ���� �Լ� */
public:



/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
};
