
#pragma once

#include "../../inc/comn/MyDialog.h"
#include "../../inc/comn/MacButton.h"
#include "../../inc/comn/MyGrpBox.h"
#include "../../inc/comn/MyStatic.h"
#include "../../inc/comn/MyEdit.h"
#include "../../inc/grid/GridCtrl.h"

class CMainThread;
class CDlgIVal;

class CDlgMain : public CMyDialog
{
public:
	CDlgMain(CWnd* parent = NULL);
	enum { IDD = IDD_MAIN };

/* Virtual functions */
protected:

	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();
	virtual VOID		UpdatePeriod();

/* Local parameters */
protected:

	UINT8				m_u8ViewMode;		/* 0x00 : Grab Mode, 0x01 : Live Mode */
	UINT8				m_u8GrabbedMode;	/* 0x00 : Reset, 0x01 : Grabbed Mode, 0x02 : Match Mode */
	BOOL				m_bSetThick;		/* 소재 두께 값 설정 했는지 여부 */
	UINT64				m_u64DrawTime;		/* Live or Grab Image Time 출력 간격 (unit: msec) */

	CMyGrpBox			m_grp_ctl[8];
	CMyStatic			m_pic_ctl[1];
	CMacCheckBox		m_chk_ctl[10];
	CMyStatic			m_txt_ctl[12];
	CMacButton			m_btn_ctl[13];
	CMyEdit				m_edt_flt[8];
	CMyEdit				m_edt_int[3];

	CGridCtrl			*m_pGridResult;
	CGridCtrl			*m_pGridOption;

	CDlgIVal			*m_pDlgIVal;
	CMainThread			*m_pMainThread;


/* Local functions */
protected:

	UINT8				GetCheckACam();

	VOID				InitCtrl();
	VOID				InitGridResult();
	VOID				InitGridOption();
	VOID				LoadDefault();

	BOOL				InitLib();
	VOID				CloseLib();
	VOID				DestroyChild();

	VOID				UpdateDrvPos();
	VOID				EnableCtrl();

	VOID				SetLiveView();
	VOID				ChangeACamNo();
	BOOL				ApplyOption(STG_MSMP &param, BOOL show);

	VOID				MC2Recovery();

	VOID				UnloadPosition();
	VOID				SaveAsExcelFile();

	VOID				MotionNextMove();
	VOID				MotionMove(ENG_MDMD direct);
	VOID				ZMotorMove(ENG_MDMD direct);
	VOID				RunMeasMatch();
	VOID				UpdateStripXY(LPG_MSMR result);
	VOID				SetResultReset();

	VOID				SetDofFilmThick();

	VOID				InvalidateView();
	VOID				UpdateLiveView();

	VOID				GetGrabbedImage();

	ENG_MMDI			GetSelectDrvID();


/* Public functions */
public:



/* Message Map */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnSysCommand(UINT32 id, LPARAM lparam);
	afx_msg VOID		OnBtnClick(UINT32 id);
	afx_msg VOID		OnChkClick(UINT32 id);
	afx_msg LRESULT		OnMsgMainThread(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT		OnMsgDialog(WPARAM wparam, LPARAM lparam);
	afx_msg VOID		OnGridColumnClick(UINT32 id, NMHDR *nmhdr, LRESULT *result);
	afx_msg VOID		OnLButtonDown(UINT32 flags, CPoint point);
};
