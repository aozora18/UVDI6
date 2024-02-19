
#pragma once

#include "DlgMenu.h"
#include "DlgSubMenu.h"

// by sys&j : 가독성을 위해 enum 추가

enum EnConfTabBtn
{
	eCONF_TAB_BTN_BASIC,
	eCONF_TAB_BTN_TEACH,
	eCONF_TAB_BTN_MAX,
};

enum EnConfTabMenu
{
	eCONF_SUB_SETTING_OPTION,
	eCONF_SUB_TEACH,
	eCONF_SUB_MENU_MAX,
};

enum EnConfPic
{
	eCONF_PIC_SUB_MENU,
	eCONF_PIC_MAX,
};
class CDlgConf : public CDlgMenu
{
public:

	/* 생성자 / 파괴자 */
	CDlgConf(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgConf();


/* 가상함수 선언 */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		UpdateControl(UINT64 tick, BOOL is_busy);
	BOOL				CreateMenu(UINT32 id);
	VOID				DeleteMenu();



public:

	virtual BOOL		OnInitDlg();
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();
	virtual void		RegisterUILevel() {}
	virtual VOID		UpdatePeriod(UINT64 tick, BOOL is_busy);


/* 로컬 변수 */
protected:

	CMacButton			m_btn_Tab_ctl[eCONF_TAB_BTN_MAX];

	CDlgSubMenu*		m_pDlgMenu;
	CMyStatic			m_pic_ctl[eCONF_PIC_MAX];

/* 로컬 함수 */
protected:

	VOID				InitCtrl();

	BOOL				InitObject();
	VOID				CloseObject();

	BOOL				InitGrid();
	VOID				CloseGrid();


/* 공용 함수 */
public:

	

/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
 	afx_msg VOID		OnBtnClick(UINT32 id);
};
