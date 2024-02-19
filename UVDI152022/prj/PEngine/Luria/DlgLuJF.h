
#pragma once

#include "../../../inc/comn/MyDialog.h"
#include "../../../inc/comn/MyGrpBox.h"
#include "../../../inc/comn/MyStatic.h"
#include "../../../inc/comn/MyEdit.h"
#include "../../../inc/comn/MyListBox.h"

class CDlgLuJF : public CMyDialog
{
public:

	CDlgLuJF(CWnd *parent=NULL);
	enum { IDD = IDD_LUJF };

/* 가상 함수 */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();

public:


/* 로컬 변수 */
protected:

	CMyGrpBox			m_grp_ctl[2];
	CMyStatic			m_txt_ctl[2];
	CMyEdit				m_edt_ctl[2];
	CMyListBox			m_box_ctl[2];


/* 로컬 함수 */
protected:

	VOID				InitCtrl();
	VOID				LoadValue();


/* 공용 함수 */
public:


/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
};
