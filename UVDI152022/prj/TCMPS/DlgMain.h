
#pragma once

#include "../../inc/comn/MyDialog.h"
#include "../../inc/comn/MacButton.h"
#include "../../inc/comn/MyGrpBox.h"
#include "../../inc/comn/MyStatic.h"
#include "../../inc/comn/MyEdit.h"
#include "../../inc/comn/MyListBox.h"
#include "../../inc/grid/GridCtrl.h"

class CMainThread;

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

	UINT8				m_u8ViewMode;		/* 0x00 : Grab Mode, 0x01 : Live Mode */
	BOOL				m_bMotionStop;		/* 가장 최근의 Motion 정지 State 값 저장 */
	BOOL				m_bLiveSaved;		/* Live Image 저장 여부 */
	UINT32				m_u32ACamZAxisRow;	/* Grid Control의 Row 행의 인덱스 값 (1-based)*/
	UINT64				m_u64DrawTime;		/* Live or Grab Image Time 출력 간격 (unit: msec) */
	UINT64				m_u64TickLoadState;	/* Job Loading 중 일정 간격으로 상태 값 요청하기 위함 */

	PUINT16				*m_pLedAmplitue;	/* LED Lamp Power 값 임시 저장 (이전 값과 현재 값 비교 후 다르면 갱신하기 위한 용도) */

	ENG_LSLS			m_enLastJobLoadStatus;	/* 가장 최근의 Job Load 상태 값 임시 저장 */

	CMyGrpBox			m_grp_ctl[13];

	CMyStatic			m_pic_ctl[5];

	CMacCheckBox		m_chk_cam[4];		/* for Camera Number */
	CMacCheckBox		m_chk_mak[4];		/* for Moving to Mark */
	CMacCheckBox		m_chk_drv[8];		/* for MC2 Drive */
	CMacCheckBox		m_chk_mtd[3];		/* for Align Method */
	CMacCheckBox		m_chk_liv[1];		/* for Live */

	CMyListBox			m_box_ctl[1];

	CMyStatic			m_txt_ctl[17];

	CMacButton			m_btn_ctl[37];

	CMyEdit				m_edt_int[4];
	CMyEdit				m_edt_flt[6];
	CMyEdit				m_edt_rlt[4];
	CMyEdit				m_edt_txt[3];

	CGridCtrl			*m_pGridPos;
	CGridCtrl			*m_pGridLed;
	CMainThread			*m_pMainThread;


/* 로컬 함수 */
protected:

	UINT8				GetRunMode();
	UINT8				GetCheckACam();
	UINT8				GetCheckMarkNo();
	UINT8				GetCheckMethod();

	VOID				InitMem();
	VOID				InitCtrl();
	VOID				InitSetup();
	VOID				InitGridPos();
	VOID				InitGridLed();
	VOID				CloseGrid();
	VOID				LoadParam();

	BOOL				InitLib();
	VOID				CloseLib();
#if 0
	VOID				WorkCompleted();
	VOID				WorkAlignResult();
	VOID				WorkErrorJobName();
#endif
	VOID				UpdateMotorPos();
	VOID				UpdateExposure();
	VOID				UpdateLiveView();
	VOID				UpdateJobList();
	VOID				UpdateJobLoad();
	VOID				UpdateLedAmpl();
#if 0
	VOID				UpdateJobRate();

	VOID				UpdateMarkGrabResult();
	VOID				UpdateMarkExpoResult();

	VOID				EnableCtrl();
#endif
	VOID				SetLiveView();
	VOID				SetRegistModel();

	VOID				MC2Recovery();
	VOID				ACamMoveAbs();

	VOID				RunTestMatch();

	VOID				UnloadPosition();
	VOID				MotionMove(ENG_MDMD direct);

	VOID				JobMgtAdd();
	VOID				JobMgtDel();
	VOID				JobMgtList();
	VOID				JobMgtLoad();

	VOID				SetExpoParamBase();
	VOID				SetExpoParam();
	VOID				GetExpoParam();

	VOID				ExpoStartXY(UINT8 flag);
	VOID				PhZAxisInitAll();

	VOID				GetLedLightLvl();
	VOID				SetLedLightLvl();
	VOID				SetLedLightLevel(UINT8 mode);
	VOID				MovingMotorFocus();

	VOID				WorkStart();
	VOID				WorkStop();

	VOID				VacuumCtrl(UINT8 onoff);
	VOID				ShutterCtrl(UINT8 onoff);

	VOID				PhZAxisMove();
	VOID				CalcEnergy();
	VOID				Grabbed2Saved();

	VOID				InvalidateView();


/* 공용 함수 */
public:



/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT		OnMsgMainThread(WPARAM wparam, LPARAM lparam);
	afx_msg VOID		OnSysCommand(UINT32 id, LPARAM lparam);
	afx_msg VOID		OnBtnClicked(UINT32 id);
	afx_msg VOID		OnChkClicked(UINT32 id);
	afx_msg VOID		OnBoxSelchangeJobList();
};
