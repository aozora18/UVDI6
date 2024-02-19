#pragma once
#include "DlgMenu.h"
#include "DlgSubMenu.h"

enum EnCalbBtn
{
	eCALB_BTN_UV_POWER,
	eCALB_BTN_CAMERA,
	eCALB_BTN_EXPOSURE,
	eCALB_BTN_ACCURACY,
	eCALB_BTN_FLATNESS,
	eCALB_BTN_MAX,
};

enum EnCalbPic
{
	eCALB_PIC_MENU,
	eCALB_PIC_MAX,
};


// CDlgCalb 대화 상자

class CDlgCalb : public CDlgMenu
{
public:

	/* 생성자 / 파괴자 */
	CDlgCalb(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgCalb();


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
	UINT64				m_u64TickPeriod;	/* 주기적으로 갱신한 최근 시간 저장 */

	CMacButton			m_btn_ctl[eCALB_BTN_MAX];
	CMyStatic			m_pic_ctl[eCALB_PIC_MAX];
	CDlgSubMenu*		m_pDlgMenu;
	ENG_CMDI_SUB		m_enSubDlgID;	/* Dialog ID */

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
	afx_msg VOID		OnBtnClick(UINT32 id);
};
