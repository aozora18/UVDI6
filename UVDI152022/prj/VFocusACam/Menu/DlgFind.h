
#pragma once

#include "DlgMenu.h"

class CDlgFind: public CDlgMenu
{
public:

	CDlgFind(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgFind();

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

	CMacButton			m_btn_ctl[3];

	CGridCtrl			*m_pGrid;


/* 로컬 함수 */
protected:

	VOID				InitCtrl();
	VOID				InitGrid();

	VOID				GetEdgeData();
	VOID				SaveToOpen();


/* 공용 함수 */
public:

	VOID				RefreshData();


/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg VOID		OnGridClicked();
	afx_msg VOID		OnBnClicked(UINT32 id);
};
