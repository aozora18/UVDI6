
#pragma once

#include "../../../inc/comn/MyDialog.h"
#include "../../../inc/comn/MacButton.h"
#include "../../../inc/comn/MyStatic.h"

// by sys&j : �������� ���� enum �߰�
enum EnMesgText
{
	eMESG_TXT_MESG,
	eMESG_TXT_MAX
};

enum EnMesgBtn
{
	eMESG_BTN_APPLY	,
	eMESG_BTN_CANCEL,
	eMESG_BTN_MAX
};

class CDlgMesg : public CMyDialog
{
public:

	CDlgMesg(CWnd* parent = NULL);
	enum { IDD = IDD_MESG };

/* ���� �Լ� */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();
	virtual void		RegisterUILevel() {}

public:

	virtual VOID		UpdatePeriod()	{};


/* ���� ���� */
protected:

	TCHAR				m_tzQuery[1024];
	TCHAR				m_tzApply[128], m_tzCancel[128];

	UINT8				m_u8BtnType;	/* 0x00 : ��ư 2��, 0x01 : ��ư 1���� */

	CMyStatic			m_txt_msg[eMESG_TXT_MAX];
	CMacButton			m_btn_ctl[eMESG_BTN_MAX];	/* Normal */


/* ���� �Լ� */
protected:

	BOOL				InitCtrl();


/* ���� �Լ� */
public:

	INT_PTR				MyDoModal(PTCHAR mesg, UINT8 type=0x00);
	INT_PTR				MyDoModal(PTCHAR mesg, PTCHAR apply, PTCHAR cancel);


/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClicked(UINT32 id);
};
