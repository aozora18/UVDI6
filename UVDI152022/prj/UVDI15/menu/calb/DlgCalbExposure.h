#pragma once
#include "../DlgSubMenu.h"
#include "../DlgSubTab.h"

enum EnCalbExposureBtn
{
	eCALB_EXPOSURE_BTN_FEM,
	eCALB_EXPOSURE_BTN_FLUSH,
	eCALB_EXPOSURE_BTN_MAX,
};

enum EnCalbExposurePic
{
	eCALB_EXPOSURE_PIC_MENU,
	eCALB_EXPOSURE_PIC_MAX,
};

enum EnCalbExposureTimer
{
	eCALB_EXPOSURE_TIMER_START = 1,
};


// CDlgCalbExposure 대화 상자

class CDlgCalbExposure : public CDlgSubMenu
{
public:

	/* 생성자 / 파괴자 */
	CDlgCalbExposure(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgCalbExposure();


	/* 가상함수 선언 */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		UpdateControl(UINT64 tick, BOOL is_busy);


public:

	virtual BOOL		OnInitDlg();
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC* dc);
	virtual VOID		OnResizeDlg();
	virtual void		RegisterUILevel() {}
	virtual VOID		UpdatePeriod(UINT64 tick, BOOL is_busy);


	/* 로컬 변수 */
protected:

	CMacButton			m_btn_ctl[eCALB_EXPOSURE_BTN_MAX];
	CMyStatic			m_pic_ctl[eCALB_EXPOSURE_PIC_MAX];
	CDlgSubTab*			m_pDlgMenu;
	ENG_CMDI_TAB		m_enTabDlgID;

	/* 로컬 함수 */
protected:

	VOID				InitCtrl();

	BOOL				InitObject();
	VOID				CloseObject();

	BOOL				CreateMenu(UINT32 id);
	VOID				DeleteMenu();

	/* 공용 함수 */
public:

	/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void		OnTimer(UINT_PTR nIDEvent);
	afx_msg VOID		OnBtnClick(UINT32 id);
};
