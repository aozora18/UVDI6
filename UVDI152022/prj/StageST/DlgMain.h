
#pragma once

#include "../../inc/comn/MyDialog.h"
#include "../../inc/comn/MacButton.h"
#include "../../inc/comn/MyStatic.h"
#include "../../inc/comn/MyEdit.h"
#include "../../inc/ctrl/EditCtrl.h"
#include "../../inc/grid/GridCtrl.h"

class CMainThread;
class CGridCtrl;
class CWorkCent;

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

	UINT8				m_u8ErrIdx;			/* Error index value to get from log buffer */
	BOOL				m_bLiveMode;		/* Sets whether it is in the current live mode */
	BOOL				m_bDofThick;		/* Whether to set the thickness of the material */

	UINT64				m_u64TickMesg;		/* Message Output Interval */
	UINT64				m_u64TickCtrl;		/* Enable or Disable Update Interval */
	UINT64				m_u64TickText;		/* Control Update Interval */
	UINT64				m_u64TickLogs;		/* Log Update Interval */
	UINT64				m_u64TickLive;		/* Live Update Interval */

	/* Positions and error values of the first and last points */
	DOUBLE				m_dbMatchXY[2][2];

	STG_SCIM			m_stSearch;			/* The structure containing information for search */

	vector<DOUBLE>		m_vMarkX, m_vMarkY;	/* Only used in demo version */

	CMyGrpBox			m_grp_ctl[11];
	CMyStatic			m_pic_ctl[1];
	CMacButton			m_btn_ctl[15];
	CMacCheckBox		m_chk_ctl[4];
	CMyStatic			m_txt_ctl[16];
	CEditNum			m_edt_int[1];
	CEditNum			m_edt_flt[13];
	CEditTxt			m_edt_txt[1];
	CMyCombo			m_cmb_ctl[1];

	CMainThread			*m_pMainThread;
	CGridCtrl			*m_pGridMeas;
	CGridCtrl			*m_pGridMC2;	/* for Motor Position and Error */

	CWorkCent			*m_pWorkCent;


/* Local Function */
protected:

	VOID				InitCtrl();
	VOID				InitData();
	VOID				InitDemo();

	VOID				InitGridMC2();
	VOID				InitGridMeas();
	VOID				CloseGrid();

	BOOL				InitLib();
	VOID				CloseLib();

	VOID				UpdateLogMesg();
	VOID				ResetErroMessage();
	VOID				EnableControls();
	VOID				UpdateControls();
	VOID				UpdateMotion();
	VOID				UpdateStepName();
	VOID				UpdateAirPressure();
	VOID				UpdateWorkStep();
	VOID				UpdateLiveView();
	VOID				UpdateGrabImage();
	VOID				UpdateGridData();
	VOID				UpdateGridTrendLines();

	VOID				CalcMeasXY();
	VOID				MoveToMeas();
	VOID				MoveToMotor(UINT8 direct);

	VOID				SetStartXY();
	VOID				SetLiveView();
	VOID				ResetGridCtrl();

	VOID				StartWork();
	VOID				StopWork();

	VOID				SetBaseMatch(UINT8 mode);
	BOOL				SetRegistModel();

	VOID				UnloadPosition();
	VOID				SetDofFilmThick();
	VOID				ChangeTarget();
	VOID				SaveGridFile();


/* Public Function */
public:



/* Message Map */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT		OnMsgThread(WPARAM wparam, LPARAM lparam);
	afx_msg VOID		OnSysCommand(UINT32 id, LPARAM lparam);
	afx_msg VOID		OnBtnClicked(UINT32 id);
	afx_msg VOID		OnChkClicked(UINT32 id);
	afx_msg VOID		OnGridSelChanged(UINT32 id, NMHDR *nmhdr, LRESULT *result);
	afx_msg VOID		OnEnChangeSetMeasVal();
};
