
#pragma once

#include "../../inc/comn/MyDialog.h"
#include "../../inc/comn/MacButton.h"
#include "../../inc/comn/MyStatic.h"
#include "../../inc/comn/MyEdit.h"
#include "../../inc/ctrl/EditCtrl.h"

class CDlgMenu;
class CDlgArea;
class CDlgFocu;
class CDlgMeas;

class CMainThread;
class CGridCtrl;

class CDlgMain : public CMyDialog
{
public:
	CDlgMain(CWnd* parent = NULL);
	enum { IDD = IDD_MAIN };

/* Virtual Function */
protected:

	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();
	virtual VOID		UpdatePeriod();


/* Local Parameter*/
protected:

	UINT8				m_u8ChkID;		/* This is the most recenlty selected check box id valud */
	UINT8				m_u8ErrIdx;

	BOOL				m_bInited;		/* Inited the luria HW and Focus Motor */

	UINT64				m_u64TickMsg;	/* Message Output Interval */
	UINT64				m_u64TickInit;	/* Saved the HW initalized time */

	STG_MICL			m_stRecipe;		/* Recipe Information */

	CMyGrpBox			m_grp_ctl[7];
	CMyStatic			m_pic_ctl[1];
	CMacButton			m_btn_ctl[18];
	CMacCheckBox		m_chk_ctl[4];
	CMyStatic			m_txt_ctl[10];
	CEditNum			m_edt_int[4];
	CEditNum			m_edt_flt[7];
	CEditTxt			m_edt_txt[2];

	CDlgMenu			*m_pDlgLast;
	CDlgMenu			*m_pDlgMenu[3];

	CMainThread			*m_pMainThread;
	CGridCtrl			*m_pGridMC2;	/* for Motor Position and Error */
	CGridCtrl			*m_pGridPH;		/* for Photohead Light Index */


/* Local Function */
protected:

	VOID				InitCtrl();
	VOID				InitData();

	VOID				InitGridMC2();
	VOID				InitGridPH();
	VOID				CloseGrid();

	VOID				InitChild();
	VOID				KillChild();
	VOID				ShowChild(UINT8 index);

	BOOL				InitLib();
	VOID				CloseLib();

	VOID				SetLuriaInit();
	VOID				SetLoadImageNo();

	VOID				SetPHAmplitude();
	VOID				GetPHAmplitudeAll();
	VOID				ResetPHLedPowerOff();

	VOID				ResetErroMessage();

	VOID				MoveStage(UINT8 direct);

	VOID				EnableCtrl();
	VOID				UpdateLogMesg();
	VOID				UpdateControls();
	VOID				UpdatePH();
	VOID				UpdateMotion();
	VOID				UpdateAirPressure();
	VOID				UpdateStepName();

	VOID				LoadRecipe();
	VOID				ReloadRecipe();
	VOID				SaveRecipe();
	VOID				MoveStart();
	VOID				StartWork();


/* Public Function */
public:



/* Message Map */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT		OnMsgThread(WPARAM wparam, LPARAM lparam);
	afx_msg VOID		OnSysCommand(UINT32 id, LPARAM lparam);
	afx_msg VOID		OnBtnClicked(UINT32 id);
	afx_msg VOID		OnChkClicked(UINT32 id);
	afx_msg VOID		OnGridColumnClick(UINT32 id, NMHDR *nmhdr, LRESULT *result);
};
