
#pragma once

#include "../../../inc/comn/MyDialog.h"
#include "../../../inc/ctrl/EditCtrl.h"
// by sys&j : �������� ���� enum �߰�
enum EnReptGrp
{
	eREPT_GRP_COUNT,
	eREPT_GRP_MAX
};

enum EnReptEdt
{
	eREPT_EDT_COUNT,
	eREPT_EDT_MAX
};

enum EnReptChk
{
	eREPT_CHK_PREHEAT	,
	eREPT_CHK_MATERIAL	,
	eREPT_CHK_VACCUM	,
	eREPT_CHK_ACAM_SIDE	,
	eREPT_CHK_PH_MOVING	,
	eREPT_CHK_LED_ON,
	eREPT_CHK_MAX
};

enum EnReptBtn
{
	eREPT_BTN_APPLY	,
	eREPT_BTN_CANCEL,
	eREPT_BTN_MAX
};

class CDlgRept : public CMyDialog
{
public:

	CDlgRept(CWnd* parent = NULL);
	enum { IDD = IDD_REPT };

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

	BOOL				m_bMovingACam;	/* 2 ȸ �̻� �뱤�� ��, Align Camera�� Photohead�� ���� �������� ���� */
	BOOL				m_bMovingPH;

	UINT32				m_u32ExpoCount;	/* �뱤 Ƚ�� */

	CMacButton			m_btn_ctl[2];	/* Normal */
	CMacCheckBox		m_chk_ctl[6];	/* Normal */
	CEditNum			m_edt_int[1];
	CMyGrpBox			m_grp_ctl[1];


/* ���� �Լ� */
protected:

	BOOL				InitCtrl();


/* ���� �Լ� */
public:

	INT_PTR				MyDoModal();

	UINT32				GetExpoCount()	{	return m_u32ExpoCount;	}
	BOOL				IsMovingACam()	{	return m_bMovingACam;	}
	BOOL				IsMovingPH()	{	return m_bMovingPH;		}


/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClicked(UINT32 id);
	afx_msg VOID		OnChkClicked(UINT32 id);
};
