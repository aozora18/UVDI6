
#pragma once

#include "../../inc/comn/MyDialog.h"
#include "../../inc/comn/MyGrpBox.h"
#include "../../inc/comn/MyStatic.h"
#include "../../inc/comn/MacButton.h"
#include "../../inc/comn/MyEdit.h"

class CGridCtrl;

class CDlgStep : public CMyDialog
{
public:

	CDlgStep(CWnd* parent = NULL);
	enum { IDD = IDD_STEP };

/* ���� �Լ� */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();

public:

	virtual VOID		UpdatePeriod()	{};


/* ���� ���� */
protected:

	STG_OSSD			m_stStep;

	CMyGrpBox			m_grp_ctl[1];
	CMyStatic			m_txt_ctl[6];
	CMacButton			m_btn_ctl[2];
	CMyEdit				m_edt_ctl[3];

	CGridCtrl			*m_pGridStepXY;


/* ���� �Լ� */
protected:

	VOID				InitCtrl();
	VOID				InitGrid();
	VOID				CloseGrid();

	VOID				LoadStepXY();
	VOID				ApplyStepXY();


/* ���� �Լ� */
public:

	INT_PTR				MyDoModal(LPG_OSSD data);


/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClicked(UINT32 id);
};
