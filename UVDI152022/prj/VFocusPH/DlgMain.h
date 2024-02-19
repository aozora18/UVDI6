
#pragma once

#include "../../inc/comn/MyDialog.h"
#include "../../inc/comn/MacButton.h"
#include "../../inc/comn/MyGrpBox.h"
#include "../../inc/comn/MyStatic.h"
#include "../../inc/comn/MyEdit.h"
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

	BOOL				m_bTriggerOn;			/* 트리거 발생 했는지 여부 */
	UINT8				m_u8ACamNo;				/* 1 or 2 */
	UINT16				m_u16StepCount;			/* 전체 측정하고자 하는 구간 값 임시 저장 */

	/* 0x00 : score, 0x01 : scale, 0x02 : Coverage, 0x03 : Fit Error */
	INT32				m_i32SetValue[MAX_PH][COLLECT_DATA_COUNT];		/* 현재 가장 적합한 값의 위치 임시 저장 */

	UINT32				m_u32ACamZAxisRow[2];	/* Grid Control의 Row 행의 인덱스 값 (1-based)*/
	UINT32				m_u32SkipCount;			/* Skip Counting 값 저장 */
	UINT64				m_u64TrigTime;			/* 트리거 발생 시간 저장 */
	DOUBLE				m_dbACamZAxisPos[2];	/* Align Camera Z Axis 이동 위치 (Position 이동만 해당) */

	/* Normal */
	CMyGrpBox			m_grp_ctl[8];
	CMyStatic			m_txt_ctl[21];
	CMacButton			m_btn_ctl[18];

	/* For Image */
	CMyStatic			m_pic_ctl[1];

	/* for MC2 */
	CMacCheckBox		m_chk_mc2[8];	/* Drv No 1 ~ 8 */
	CMyEdit				m_edt_mc2[2];	/* Dist / Velo */

	/* for Align Camera */
	CMacCheckBox		m_chk_cam[1];	/* Drv No 1 ~ 4, Live */
	CMyEdit				m_edt_cam[2];

	/* for Measurement Option */
	CMyEdit				m_edt_opt[5];
	/* for Trigger Time */
	CMyEdit				m_edt_trg[3];	/* strobe, triger, delay */
	/* for Progress Rate */
	CMyEdit				m_edt_pgr[1];

	CGridCtrl			*m_pGridMotor;	/* for Motor Position */
	CGridCtrl			*m_pGridMeas;	/* for Measurement Result */
	
	CMainThread			*m_pMainThread;

	CMySection			m_csSyncTrig;


/* 로컬 함수 */
protected:

	VOID				InitCtrl();
	VOID				InitGridMotor();
	VOID				InitGridMeas();
	VOID				InitSetup();
	BOOL				InitLib();
	VOID				CloseGrid();
	VOID				CloseLib();
	VOID				LoadTriggerCh();

	VOID				UpdateCtrlACam();
	VOID				UpdateResult();
	VOID				UpdateMotorPos();
	VOID				UpdatePLCVal();
	VOID				UpdateEnableCtrl();

	VOID				MC2Recovery();

	VOID				ACamMoveAbs();
	VOID				ACamHoming();

	VOID				StartFocus();
	VOID				StopFocus();
	VOID				NextStepMoveY();
	VOID				UnloadStageXY();

	BOOL				SetModelParam();
	VOID				SetTriggerParam();
	VOID				SetTriggerCh();
	VOID				SetACamNo();
	VOID				RunFindModel();
	VOID				ResetFindModel();

	VOID				MotionMove(ENG_MDMD direct);

	VOID				MovePosMeasure();
	VOID				SetLiveView();

	VOID				LoadDataConfig();
	VOID				SaveDataToFile();

	VOID				UpdateStepResult();
	VOID				UpdateMatchDraw();
	VOID				UpdateGrabResult();

	VOID				DisplayGrabError(UINT8 flag);

	VOID				SaveAsExcelFile();
	VOID				InvalidateView();
	VOID				UpdateLiveView();


/* 공용 함수 */
public:

	BOOL				IsRunFocus();
	VOID				UpdateTrigParam();


/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT		OnMsgThread(WPARAM wparam, LPARAM lparam);
	afx_msg VOID		OnSysCommand(UINT32 id, LPARAM lparam);
	afx_msg VOID		OnBtnClicked(UINT32 id);
	afx_msg VOID		OnChkClickACam(UINT32 id);
	afx_msg VOID		OnChkClickOption(UINT32 id);
};
