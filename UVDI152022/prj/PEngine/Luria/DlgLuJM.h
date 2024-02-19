
#pragma once

#include "DlgLuria.h"


class CDlgLuJM : public CDlgLuria
{
public:

	CDlgLuJM(UINT32 id, CWnd *parent=NULL);
	virtual ~CDlgLuJM();


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

	CMyGrpBox			m_grp_ctl[2];
	CMyStatic			m_txt_ctl[14];
	CMyEdit				m_edt_ctl[14];
	CMyListBox			m_box_ctl[1];
	CMacButton			m_btn_ctl[1];
	CMacCheckBox		m_chk_ctl[5];


/* 로컬 함수 */
protected:

	VOID				InitCtrl();


/* 공용 함수 */
public:



/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg VOID		OnBtnClicked(UINT32 id);
};
