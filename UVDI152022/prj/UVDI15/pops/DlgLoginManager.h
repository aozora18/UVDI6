
#pragma once

#include "../../../inc/comn/MyDialog.h"
#include "../../../inc/ctrl/EditCtrl.h"
#include "../../../inc/grid/GridCtrl.h"
#include "../../../inc/grid/GridCellCombo.h"



enum EnIdManagerTitle
{
	eMNG_TTL_MANAGER,
	eMNG_TTL_LIST,
	eMNG_TTL_MAX
};

enum EnIdManagerStt
{
	eMNG_STT_ID,
	eMNG_STT_PASSWORD,
	eMNG_STT_PASSWORD_CONFIRM,
	eMNG_STT_LEVEL,
	eMNG_STT_MAX
};

enum EnIdManagerTxt
{
	eMNG_TXT_ID,
	eMNG_TXT_PASSWORD,
	eMNG_TXT_PASSWORD_CONFIRM,
	eMNG_TXT_MAX
};

enum EnIdManagerGrd
{
	eMNG_GRD_LIST,
	eMNG_GRD_MAX
};

enum EnIdManagerBtn
{
	eMNG_BTN_OPERATOR,
	eMNG_BTN_ENGINEER,
	eMNG_BTN_ADMIN,

	eMNG_BTN_DEL,
	eMNG_BTN_ADD,
	eMNG_BTN_CLOSE,

	eMNG_BTN_MAX
};

class CDlgLoginManager : public CMyDialog
{

public:

	CDlgLoginManager(CWnd* parent = NULL);
	enum { IDD = IDD_LOGIN_MANAGER };

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

	TCHAR				m_tzUnit[32];

	CMyStatic			m_ttl_ctl[eMNG_TTL_MAX];
	CMyStatic			m_stt_ctl[eMNG_STT_MAX];
	CMyStatic			m_txt_ctl[eMNG_TXT_MAX];
	CMyEdit				m_edt_txt[eMNG_TXT_MAX];		//	Resize가 제공되지 않음.
	CGridCtrl			m_grd_ctl[eMNG_GRD_MAX];
	CMacButton			m_btn_ctl[eMNG_BTN_MAX];

	CString				m_strID;
	CString				m_strPwd;
	CString				m_strLevel;
	CString				m_strSel;

	int					m_nSelRow;

/* 로컬 함수 */
protected:

	BOOL				InitCtrl();
	BOOL				InitGrid();
	BOOL				UpdateGrid();
	
	void				SelectBtnLevel(EN_LOGIN_LEVEL eLevel);
	void				ConvertToAsterisks(TCHAR* str);
	void				ConvertToAsterisks(CString& str);



	BOOL				IsInputID();
	BOOL				IsInputPassword();
	BOOL				IsInputPasswordConfirm();
	BOOL				IsMatchPassword(CString str);

	BOOL				IsOverlapID(CString str);
	BOOL				IsSelectID();
	BOOL				IsDeleteID();

	void				CleanText();


	void				AddDBLoginInfo();
	void				DelDBLoginInfo();

	


/* 공용 함수 */
public:

	INT_PTR				MyDoModal();


/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClicked(UINT32 id);
	afx_msg VOID		OnTxtClicked(UINT32 id);
	afx_msg	void		OnClickGrid(NMHDR* pNotifyStruct, LRESULT* pResult);
};
