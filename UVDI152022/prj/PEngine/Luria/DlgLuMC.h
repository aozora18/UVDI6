
#pragma once

#include "DlgLuria.h"


class CDlgLuMC : public CDlgLuria
{
public:

	CDlgLuMC(UINT32 id, CWnd *parent=NULL);
	virtual ~CDlgLuMC();


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

	CMyGrpBox			m_grp_ctl[4];
	CMyCombo			m_cmb_ctl[2];
	CMacCheckBox		m_chk_ctl[2];
	CMyStatic			m_txt_ctl[18];
	CMyEdit				m_edt_ctl[19];


/* 로컬 함수 */
protected:

	VOID				InitCtrl();


/* 공용 함수 */
public:



/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
};
