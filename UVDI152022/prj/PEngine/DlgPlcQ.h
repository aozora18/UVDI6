
#pragma once

#include "../../inc/base/DlgChild.h"

class CDlgPlcQ : public CDlgChild
{
public:

	CDlgPlcQ(UINT32 id, CWnd *parent=NULL);
	virtual ~CDlgPlcQ();

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

	CGridCtrl			*m_pGrid;

/* 로컬 함수 */
protected:

	BOOL				InitCtrl();
	BOOL				InitGrid();

	VOID				UpdatePLCValue();


/* 공용 함수 */
public:



/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg VOID		OnGridClicked();
};
