
#pragma once

#include "../../../inc/comn/MyDialog.h"

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
	virtual void		RegisterUILevel() {}

public:

	virtual VOID		UpdatePeriod()	{};


/* ���� ���� */
protected:

	UINT8				m_u8Offset;

	CMacButton			m_btn_ctl[2];	/* Normal */
	CMacCheckBox		m_chk_ctl[7];	/* Normal */


/* ���� �Լ� */
protected:

	BOOL				InitCtrl();


/* ���� �Լ� */
public:

	INT_PTR				MyDoModal();

	UINT8				GetOffset()		{	return m_u8Offset;		}


/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClicked(UINT32 id);
	afx_msg VOID		OnChkClicked(UINT32 id);
};
