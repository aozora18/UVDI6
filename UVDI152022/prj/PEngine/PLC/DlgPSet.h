
#pragma once

#include "../../../inc/comn/MyDialog.h"
#include "../../../inc/comn/MyGrpBox.h"
#include "../../../inc/comn/MyCombo.h"
#include "../../../inc/comn/MyStatic.h"
#include "../../../inc/comn/MyEdit.h"
#include "../../../inc/comn/MacButton.h"

class CDlgPSet : public CMyDialog
{
public:

	CDlgPSet(CWnd* parent = NULL);
	enum { IDD = IDD_PSET };

// 가상함수 선언
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();

public:


// 로컬 변수
protected:

	UINT16				m_u16SetAddr;	/* 값 설정하고자 하는 주소 인덱스 (WORD Index) */

	CMyGrpBox			m_grp_ctl[1];
	CMyCombo			m_cmb_ctl[1];
	CMyStatic			m_txt_ctl[2];
	CMyEdit				m_edt_ctl[1];

	CMacButton			m_btn_ctl[1];	/* Normal */
	CMacCheckBox		m_chk_ctl[20];


// 로컬 함수
protected:

	BOOL				InitCtrl();

	VOID				LoadAddr();
	VOID				UpdateValue();
	VOID				SetValue();
	VOID				ChangeChkValue();
	VOID				ChangeEdtValue();
	VOID				ChangeHexValue();


// 공용 함수
public:

	INT_PTR				MyDoModal(UINT16 addr);
	

// 메시지 맵
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClicked(UINT32 id);
	afx_msg VOID		OnChkClicked(UINT32 id);
	afx_msg VOID		OnCmbSelChangeDAddr();
};
