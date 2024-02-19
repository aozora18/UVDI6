
#pragma once

#include "../../../inc/comn/MyDialog.h"
#include "../../../inc/ctrl/EditCtrl.h"



// by sys&j : 가독성을 위해 enum 추가
enum EnIdAdminTitle
{
	eADMIN_TTL_ADMIN,
	eADMIN_TTL_MAX
};

enum EnIdAdminStt
{
	eADMIN_STT_ID,
	eADMIN_STT_PASSWORD,
	eADMIN_STT_MAX
};

enum EnIdAdminTxt
{
	eADMIN_TXT_ID,
	eADMIN_TXT_PASSWORD,
	eADMIN_TXT_MAX
};

enum EnIdAdminBtn
{
	eADMIN_BTN_CANCEL,
	eADMIN_BTN_LOGIN,
	eADMIN_BTN_MAX
};

class CDlgLoginAdmin : public CMyDialog
{

public:

	CDlgLoginAdmin(CWnd* parent = NULL);
	enum { IDD = IDD_LOGIN_ADMIN };

/* 가상 함수 */
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


/* 로컬 변수 */
protected:


	CMyStatic			m_ttl_ctl[eADMIN_TTL_MAX];
	CMyStatic			m_stt_ctl[eADMIN_STT_MAX];
	CMyStatic			m_txt_ctl[eADMIN_TXT_MAX];
	CMacButton			m_btn_ctl[eADMIN_BTN_MAX];


	CString				m_strID;
	CString				m_strPwd;


/* 로컬 함수 */
protected:

	BOOL				InitCtrl();


	void				ConvertToAsterisks(TCHAR* str);


	BOOL				IsInputID();
	BOOL				IsInputPassword();

	BOOL				IsEnableLogin(CString strID, CString strPwd);
	BOOL				IsEmptyLoginInfo();

/* 공용 함수 */
public:

	INT_PTR				MyDoModal();


/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClicked(UINT32 id);
	afx_msg VOID		OnTxtClicked(UINT32 id);
};
