
#pragma once

#include "../../inc/base/DlgChild.h"


class CDlgEFEM : public CDlgChild
{
public:

	CDlgEFEM(UINT32 id, CWnd *parent=NULL);
	virtual ~CDlgEFEM();


/* 가상함수 */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();

public:

	virtual VOID		UpdateCtrl();


/* 로컬 변수 */
protected:

	CMyGrpBox			m_grp_ctl[3];
	CMyStatic			m_txt_ctl[11];
	CMyCombo			m_cmb_ctl[1];
	CMyEdit				m_edt_ctl[10];
	CGridCtrl			*m_pGrid;

/* 로컬 함수 */
protected:

	VOID				InitCtrl();
	VOID				InitGrid();
	VOID				UpdateMoni();


/* 공용 함수 */
public:



/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
};
