
#pragma once

#include "../../inc/base/DlgChild.h"

class CDlgLogs : public CDlgChild
{
public:

	CDlgLogs(UINT32 id, CWnd *parent=NULL);
	virtual ~CDlgLogs();


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

	BOOL				m_bUpdateLogs;	/* �α� ������ ��� ���� */

	CMyRich				m_rch_ctl[1];

	CMacButton			m_btn_ctl[2];
	CMacCheckBox		m_chk_ctl[1];

/* ���� �Լ� */
protected:

	BOOL				InitCtrl();

	VOID				UpdateLogs();


/* ���� �Լ� */
public:



/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClicked(UINT32 id);
	afx_msg VOID		OnChkClicked(UINT32 id);
};
