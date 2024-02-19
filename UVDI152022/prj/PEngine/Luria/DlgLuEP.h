
#pragma once

#include "DlgLuria.h"


class CDlgLuEP : public CDlgLuria
{
public:

	CDlgLuEP(UINT32 id, CWnd *parent=NULL);;
	virtual ~CDlgLuEP();


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

	CMyGrpBox			m_grp_ctl[5];
	CMyStatic			m_txt_ctl[7];
	CMyEdit				m_edt_ctl[8];
	CMacCheckBox		m_chk_ctl[2];
	CMacProgress		m_pgs_ctl[1];

	CGridCtrl			*m_pGridAmpl;


/* ���� �Լ� */
protected:

	VOID				InitCtrl();
	VOID				InitGrid();


/* ���� �Լ� */
public:



/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
};
