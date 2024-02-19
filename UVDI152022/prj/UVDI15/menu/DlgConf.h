
#pragma once

#include "DlgMenu.h"
#include "DlgSubMenu.h"

// by sys&j : �������� ���� enum �߰�

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

	/* ������ / �ı��� */
	CDlgConf(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgConf();


/* �����Լ� ���� */
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


/* ���� ���� */
protected:

	CMacButton			m_btn_Tab_ctl[eCONF_TAB_BTN_MAX];

	CDlgSubMenu*		m_pDlgMenu;
	CMyStatic			m_pic_ctl[eCONF_PIC_MAX];

/* ���� �Լ� */
protected:

	VOID				InitCtrl();

	BOOL				InitObject();
	VOID				CloseObject();

	BOOL				InitGrid();
	VOID				CloseGrid();


/* ���� �Լ� */
public:

	

/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
 	afx_msg VOID		OnBtnClick(UINT32 id);
};
