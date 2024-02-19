
#pragma once

#include "DlgLuria.h"


class CDlgLuPD : public CDlgLuria
{
public:

	CDlgLuPD(UINT32 id, CWnd *parent=NULL);
	virtual ~CDlgLuPD();


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
	CMyStatic			m_txt_ctl[6];
	CMyEdit				m_edt_ctl[6];
	CMacButton			m_btn_ctl[1];
	CMyListBox			m_box_ctl[2];


/* ���� �Լ� */
protected:

	VOID				InitCtrl();
	VOID				LoadValue();


/* ���� �Լ� */
public:



/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBoxSelchangeDcode();
};
