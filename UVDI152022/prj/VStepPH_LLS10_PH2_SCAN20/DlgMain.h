
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

/* ���� �Լ� */
protected:

	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();
	virtual VOID		UpdatePeriod();

/* ���� ���� */
protected:

	UINT8				m_u8ViewMode;		/* 0x00 : Grab Mode, 0x01 : Live Mode */

	BOOL				m_bEnableLast;		/* ������ ���� �� ���� */
	BOOL				m_bReqPhOffset;		/* Photohead Offset ���� ��û �ߴ��� ���� */

	UINT8				m_u8FocusCount;		/* ���� ������ Focusing ���� �� */
	UINT8				m_u8LastCamZAxis;	/* ���� �ֱٿ� ������ ī�޶� ��ȣ (���̸� ������ ��ȣ) */
	DOUBLE				m_dbLastACamPos;	/* ���� �ֱٿ� ������ ī�޶��� ��ġ ���� ���� */

	LPG_ACGR			m_pstGrabData;		/* �ֱ� Grabbed Image�� ����� Model ���� */

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


/* ���� �Լ� */
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


/* ���� �Լ� */
public:



/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnSysCommand(UINT32 id, LPARAM lparam);
	afx_msg VOID		OnBtnClick(UINT32 id);
	afx_msg VOID		OnChkClick(UINT32 id);
	afx_msg LRESULT		OnMsgMainThread(WPARAM wparam, LPARAM lparam);
};
