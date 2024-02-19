
#pragma once

#include "../../inc/comn/MyDialog.h"
#include "../../inc/comn/MacButton.h"
#include "../../inc/comn/MyGrpBox.h"
#include "../../inc/comn/MyStatic.h"
#include "../../inc/comn/MyEdit.h"
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

	BOOL				m_bEnableLast;		/* 마지막 상태 값 저장 */
	BOOL				m_bReqPhOffset;		/* Photohead Offset 값을 요청 했는지 여주 */

	UINT8				m_u8FocusCount;		/* 현재 측정된 Focusing 개수 값 */
	UINT8				m_u8LastCamZAxis;	/* 가장 최근에 측정한 카메라 번호 (높이를 측정한 번호) */
	DOUBLE				m_dbLastACamPos;	/* 가장 최근에 측정한 카메라의 위치 정보 저장 */

	LPG_ACGR			m_pstGrabData;		/* 최근 Grabbed Image가 저장된 Model 정보 */

	CMyGrpBox			m_grp_ctl[13];
	CMacCheckBox		m_chk_ctl[10];
	CMyStatic			m_txt_ctl[21];
	CMacButton			m_btn_ctl[18];
	CMyEdit				m_edt_flt[18];
	CMyEdit				m_edt_int[4];

	/* For Image */
	CMyStatic			m_pic_ctl[2];

	CGridCtrl			*m_pGridStep[3];	/* 0 : StepX, 1 : StepY, 2 : Measure XY */
	CMainThread			*m_pMainThread;


/* 로컬 함수 */
protected:

	VOID				InitCtrl();
	VOID				InitSetup();
	VOID				InitGridView();
	VOID				InitGridData();
	VOID				CloseGrid();
	VOID				LoadParam();

	BOOL				InitLib();
	VOID				CloseLib();

	VOID				UpdateDrvPos();
	VOID				UpdateStepXY();
	VOID				UpdatePhOffset();
	VOID				UpdateMatchVal();

	VOID				InvalidateView();
	VOID				EnableCtrl();

	VOID				SetLiveView();
	BOOL				PutOneTrigger();

	VOID				SetRegistModel();
	BOOL				SetMatchModel();
	VOID				RunMeasure();
	VOID				SetChangeVal();

	VOID				MC2Recovery();
	VOID				ACamZAxisHoming();
	VOID				ACamMoveAbs();

	VOID				SavePhOffset();
	VOID				UnloadPosition();
	VOID				InitMeasureMove();
	VOID				SaveMeasureMove();
	VOID				ResetMeasureXY();
	VOID				SaveAsExcelFile();

	VOID				GetGrabSize();
	VOID				SetGrabSize();

	VOID				MotionMove(ENG_MDMD direct);

	VOID				UpdateMatchGrid(DOUBLE step_x, DOUBLE step_y,
										DOUBLE cent_lx, DOUBLE cent_ly,
										DOUBLE cent_rx, DOUBLE cent_ry);

	VOID				SetStepCalc();
	VOID				UpdateLiveView();


/* 공용 함수 */
public:



/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnSysCommand(UINT32 id, LPARAM lparam);
	afx_msg VOID		OnBtnClick(UINT32 id);
	afx_msg VOID		OnChkClick(UINT32 id);
	afx_msg LRESULT		OnMsgMainThread(WPARAM wparam, LPARAM lparam);
};
