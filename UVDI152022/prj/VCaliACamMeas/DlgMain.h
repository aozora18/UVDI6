
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
	BOOL				m_bSetThick;		/* 소재 두께 값 설정 했는지 여부 */
	UINT64				m_u64DrawTime;		/* Live or Grab Image Time 출력 간격 (unit: msec) */

	STG_ACCS			m_stMeas;

	CMyGrpBox			m_grp_ctl[14];
	CMyStatic			m_pic_ctl[1];
	CMacCheckBox		m_chk_ctl[10];
	CMyStatic			m_txt_ctl[26];
	CMacButton			m_btn_ctl[17];
	CMyEdit				m_edt_flt[18];
	CMyEdit				m_edt_int[7];
	CMyEdit				m_edt_txt[2];

	CGridCtrl			*m_pGridMeas;
	CMainThread			*m_pMainThread;


/* 로컬 함수 */
protected:

	UINT8				GetCheckACam();

	VOID				InitCtrl();
	VOID				InitSetup();
	VOID				InitGrid();
	VOID				CloseGrid();
	VOID				LoadParam();
	VOID				LoadMeasPos();

	BOOL				InitLib();
	VOID				CloseLib();

	VOID				UpdateDrvPos();
	VOID				UpdateCaliXY();
	VOID				UpdateCaliXY(INT32 row, INT32 col, LPG_ACGR result);
	VOID				UpdateCaliXY(INT32 row, INT32 col, DOUBLE offset_x, DOUBLE offset_y);

	VOID				EnableCtrl();

	VOID				SetLiveView();
	VOID				ChangeACamNo();

	VOID				SetRegistModel();

	VOID				MC2Recovery();
	VOID				ACamMoveAbs();

	VOID				UnloadPosition();
	VOID				InitMeasureMove();
	VOID				SaveMeasureMove();
	BOOL				ReBuildGridXY();
	VOID				SaveAsExcelFile();

	VOID				MotionMove(ENG_MDMD direct);

	VOID				StartCalibration();
	VOID				StopCalibration();

	VOID				SetMotionPos(UINT32 id);

	VOID				RunTestMatch();
	VOID				RunTestMeas();

	VOID				SetDofFilmThick();
	VOID				WorkComplete();
	VOID				UpdateWorkTime();

	VOID				InvalidateView();
	VOID				UpdateLiveView();


/* 공용 함수 */
public:



/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnSysCommand(UINT32 id, LPARAM lparam);
	afx_msg VOID		OnBtnClick(UINT32 id);
	afx_msg VOID		OnChkClick(UINT32 id);
	afx_msg VOID		OnEnChangeRebuild(UINT32 id);
	afx_msg LRESULT		OnMsgMainThread(WPARAM wparam, LPARAM lparam);
};
