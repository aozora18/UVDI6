
#pragma once

#include "../../inc/comn/MyDialog.h"
#include "../../inc/comn/MacButton.h"
#include "../../inc/comn/MacProgress.h"
#include "../../inc/comn/MyGrpBox.h"
#include "../../inc/comn/MyStatic.h"
#include "../../inc/comn/MyEdit.h"
#include "../../inc/comn/MyListBox.h"
#include "../../inc/comn/MyCombo.h"
#include "../../inc/grid/GridCtrl.h"

class CMainThread;

class CMeasure;

class CDlgMain : public CMyDialog
{
public:
	CDlgMain(CWnd* parent = NULL);
	enum { IDD = IDD_MAIN };

/* 가상 함수 */
protected:

	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();
	virtual VOID		UpdatePeriod();

/* 로컬 변수 */
protected:

	UINT16				**m_pLedAmplitue;

	UINT32				m_u32ExpoSpeed;		/* 가장 최근의 Exposure Speed (um/sec) 값 임시 저장 */
	UINT64				m_u64TickLoadState;	/* Job Load State 갱신한 시점의 시간 저장 */
	UINT64				m_u64TickExpoState;	/* Exposure State 요청 시간 값 임시 저장 */

	ENG_LPES			m_enExpoStatus;		/* 가장 최근의 Exposure Status 값 임시 저장 */
	ENG_LSLS			m_enLastJobLoadStatus;	/* 가장 최근의 Job Load 상태 값 임시 저장 */

	/* Normal */
	CMyGrpBox			m_grp_ctl[9];
	CMyStatic			m_txt_ctl[29];
	CMacButton			m_btn_ctl[26];
	CMacCheckBox		m_chk_mc2[8];
	CMacCheckBox		m_chk_pos[2];
	CMyCombo			m_cmb_ctl[1];
	CMacProgress		m_pgs_ctl[1];
	CMyListBox			m_box_ctl[1];
	CMyEdit				m_edt_flt[10];
	CMyEdit				m_edt_str[2];
	CMyEdit				m_edt_int[4];

	CMainThread			*m_pMainThread;

	CGridCtrl			*m_pGridPos;	/* for Motor Position */
	CGridCtrl			*m_pGridLed;	/* for Photohead LED */


/* 로컬 함수 */
protected:

	VOID				InitCtrl();
	VOID				InitSetup();
	VOID				InitMem();

	BOOL				InitLib();
	VOID				CloseLib();

	VOID				InitGridPos();
	VOID				InitGridLed();
	VOID				CloseGrid();

	VOID				LoadDataConfig();

	VOID				UpdateJobList();
	VOID				UpdateJobLoad();
	VOID				UpdateExposure();
	VOID				UpdateLedAmpl();
	VOID				UpdateMotorPos();
	VOID				UpdateEnableCtrl();
	VOID				UpdateEtcValue();

	VOID				MC2Recovery();
	VOID				SaveDataToFile();

	VOID				JobMgtAdd();
	VOID				JobMgtDel();
	VOID				JobMgtLoad();
	VOID				JobMgtList();

	VOID				SetExpoParam();
	VOID				GetExpoParam();

	VOID				GetLedLightLvl();
	VOID				SetLedLightLvl();
	VOID				SetLedLightDefault();

	VOID				ResetOpticError();

	VOID				ExpoStartXY(UINT8 flag);
	VOID				PhZAxisInitAll();
	VOID				PhZAxisUpDown(UINT8 flag);
	VOID				PHMovingFocus();
	VOID				PHHWInit();
	VOID				UnloadPosition();


/* 공용 함수 */
public:

	VOID				StartPrint();
	VOID				StopPrint();

	VOID				MotionMove(ENG_MDMD direct);


/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT		OnMsgThread(WPARAM wparam, LPARAM lparam);
	afx_msg VOID		OnSysCommand(UINT32 id, LPARAM lparam);
	afx_msg VOID		OnBtnClicked(UINT32 id);
	afx_msg VOID		OnChkClicked(UINT32 id);
	afx_msg VOID		OnBoxSelchangeJobList();
};
