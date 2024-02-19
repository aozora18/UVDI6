
#pragma once

#include "../../../inc/comn/MyDialog.h"
#include "../../../inc/ctrl/EditCtrl.h"
#include "../../../inc/grid/GridCtrl.h"
#include "../../../inc/grid/GridCellCombo.h"



// by sys&j : �������� ���� enum �߰�
enum EnIdLoginTitle
{
	eLOGIN_TTL_LOGIN,
	eLOGIN_TTL_MAX
};

enum EnIdLoginStt
{
	eLOGIN_STT_LEVEL,
	eLOGIN_STT_ID,
	eLOGIN_STT_PASSWORD,
	eLOGIN_STT_MAX
};

enum EnIdLoginTxt
{
	eLOGIN_TXT_PASSWORD,
	eLOGIN_TXT_MAX
};

enum EnIdLoginGrd
{
	eLOGIN_GRD_ID,
	eLOGIN_GRD_MAX
};

enum EnIdLoginBtn
{
	eLOGIN_BTN_OPERATOR,
	eLOGIN_BTN_ENGINEER,
	eLOGIN_BTN_ADMIN,
	eLOGIN_BTN_MANAGER,
	eLOGIN_BTN_CANCEL,
	eLOGIN_BTN_LOGIN,
	eLOGIN_BTN_MAX
};

class CDlgLoginMain : public CMyDialog
{

public:

	CDlgLoginMain(CWnd* parent = NULL);
	enum { IDD = IDD_LOGIN_MAIN };

/* ���� �Լ� */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();
	virtual VOID		RegisterUILevel()	{};

public:

	virtual VOID		UpdatePeriod()	{};


/* ���� ���� */
protected:


	CMyStatic			m_ttl_ctl[eLOGIN_TTL_MAX];
	CMyStatic			m_stt_ctl[eLOGIN_STT_MAX];
	CMyStatic			m_txt_ctl[eLOGIN_TXT_MAX];
	CGridCtrl			m_grd_ctl[eLOGIN_GRD_MAX];
	CMacButton			m_btn_ctl[eLOGIN_BTN_MAX];

	CString				m_strID;
	CString				m_strPwd;
	CString				m_strLevel;

	EN_LOGIN_LEVEL		m_eLevel;


/* ���� �Լ� */
protected:

	BOOL				InitCtrl();
	BOOL				InitGrid();
	BOOL				UpdateGridItem();


	void				SelectBtnLevel(EN_LOGIN_LEVEL eLevel);
	void				ConvertToAsterisks(TCHAR* str);


	BOOL				IsInputID();
	BOOL				IsInputPassword();

	BOOL				IsEnableLogin(CString strID, CString strPwd, CString strLevel);
	BOOL				IsNullLoginInfo();

/* ���� �Լ� */
public:


	CString				GetLevel() { return m_strLevel; }

	INT_PTR				MyDoModal();


/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClicked(UINT32 id);
	afx_msg VOID		OnTxtClicked(UINT32 id);
};
