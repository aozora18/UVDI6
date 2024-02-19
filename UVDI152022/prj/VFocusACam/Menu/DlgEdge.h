
#pragma once

#include "DlgMenu.h"

class CDlgEdge : public CDlgMenu
{
public:

	CDlgEdge(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgEdge();

/* 구조체 / 공용체 */
protected:


/* 가상 함수 */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();

public:

	virtual VOID		UpdateCtrl();
	virtual VOID		ResetData();


/* 로컬 변수 */
protected:

	CMyStatic			m_pic_ctl[1];


/* 로컬 함수 */
protected:

	VOID				InitCtrl();

	VOID				DrawEdge();


/* 공용 함수 */
public:



/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg VOID		OnShowWindow(BOOL show, UINT32 status);
};
