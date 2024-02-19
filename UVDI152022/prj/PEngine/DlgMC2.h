
#pragma once

#include "../../inc/base/DlgChild.h"

class CDlgMC2 : public CDlgChild
{
public:

	CDlgMC2(UINT32 id, CWnd *parent=NULL);
	virtual ~CDlgMC2();


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
	CMyStatic			m_txt_ctl[1];
	CMyCombo			m_cmb_ctl[1];
	CMacCheckBox		m_chk_ctl[2];

	CGridCtrl			*m_pGrid[3];	/* Header / Values (Data) / Error */

/* 로컬 함수 */
protected:

	BOOL				InitCtrl();
	BOOL				InitGrid();

	VOID				UpdateMC2Value();


/* 공용 함수 */
public:



/* 메시지 맵 */
protected:

	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnChkClicked(UINT32 id);
};
