
#pragma once

#include "../../inc/base/DlgChild.h"

class CDlgLogs : public CDlgChild
{
public:

	CDlgLogs(UINT32 id, CWnd *parent=NULL);
	virtual ~CDlgLogs();


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

	BOOL				m_bUpdateLogs;	/* 로그 데이터 출력 여부 */

	CMyRich				m_rch_ctl[1];

	CMacButton			m_btn_ctl[2];
	CMacCheckBox		m_chk_ctl[1];

/* 로컬 함수 */
protected:

	BOOL				InitCtrl();

	VOID				UpdateLogs();


/* 공용 함수 */
public:



/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClicked(UINT32 id);
	afx_msg VOID		OnChkClicked(UINT32 id);
};
