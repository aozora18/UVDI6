
#pragma once

#include "DlgLuria.h"


class CDlgLuDP : public CDlgLuria
{
public:

	CDlgLuDP(UINT32 id, CWnd *parent=NULL);
	virtual ~CDlgLuDP();


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
	CMyStatic			m_txt_ctl[13];
	CMyEdit				m_edt_ctl[22];
	CMyCombo			m_cmb_ctl[1];
	CMacCheckBox		m_chk_ctl[12];


/* ���� �Լ� */
protected:

	VOID				InitCtrl();


/* ���� �Լ� */
public:



/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()

};
