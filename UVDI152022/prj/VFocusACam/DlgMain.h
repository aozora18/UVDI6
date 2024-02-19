
#pragma once

#include "../../inc/comn/MyDialog.h"
#include "../../inc/comn/MacButton.h"
#include "../../inc/comn/MyGrpBox.h"
#include "../../inc/comn/MyStatic.h"
#include "../../inc/comn/MyEdit.h"
#include "../../inc/comn/MyTabCtrl.h"
#include "../../inc/grid/GridCtrl.h"

class CDlgMenu;
class CDlgExpo;
class CDlgEdge;
class CDlgFind;
class CDlgHist;
class CDlgLive;
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

	INT32				m_i32ACamZAxisPos;	/* Align Camera Z Axis 이동 위치 (Position 이동만 해당) */

	UINT32				m_u32ACamZAxisRow;	/* Grid Control의 Row 행의 인덱스 값 (1-based)*/

	UINT32				m_u32SkipCount;		/* Skip Counting 값 저장 */

	/* Normal */
	CMyGrpBox			m_grp_ctl[6];
	CMyStatic			m_txt_ctl[19];
	CMacButton			m_btn_ctl[11];
	CMyTabCtrl			m_tab_ctl[1];

	/* for MC2 */
	CMacCheckBox		m_chk_mc2[8];	/* Drv No 1 ~ 8 */
	CMyEdit				m_edt_mc2[2];	/* Dist / Velo */

	/* for Align Camera */
	CMacCheckBox		m_chk_cam[5];	/* Drv No 1 ~ 4, Live */
	CMyEdit				m_edt_cam[1];

	/* for Focusing Option */
	CMacCheckBox		m_chk_opt[2];
	CMyEdit				m_edt_opt[5];

	/* for Trigger Time */
	CMyEdit				m_edt_trg[3];	/* strobe, triger, delay */

	/* for Work Status */
	CMyEdit				m_edt_pgr[1];

	CDlgMenu			*m_pDlgLast;
	CDlgExpo			*m_pDlgExpo;
	CDlgEdge			*m_pDlgEdge;
	CDlgFind			*m_pDlgFind;
	CDlgHist			*m_pDlgHist;
	CDlgLive			*m_pDlgLive;

	CGridCtrl			*m_pGridPos;	/* for Motor Position */
	
	CMainThread			*m_pMainThread;


/* 로컬 함수 */
protected:

	VOID				InitCtrl();

	VOID				InitTab();
	VOID				CloseTab();

	VOID				InitSetup();

	VOID				InitGrid();
	VOID				CloseGrid();

	BOOL				InitLib();
	VOID				CloseLib();

	VOID				KillChild(CDlgMenu *dlg);
	
	VOID				UpdateCtrlACam();
	VOID				UpdateResult();
	VOID				UpdateMotorPos();
	VOID				UpdatePLCVal();
	VOID				UpdateEnableCtrl();
	VOID				UpdateChildCtrl();

	VOID				MC2Recovery();
	VOID				ACamZAxisRecovery();

	VOID				MotionMove(ENG_MDMD direct);
	VOID				ACamMoveAbs();

	VOID				StartFocus();
	VOID				ResetFocus();
	VOID				StopFocus();

	VOID				MovePosMeasure();
	VOID				SetLiveView();

	VOID				LoadDataConfig();
	VOID				SaveDataToFile();

	VOID				UpdateStepResult();

	UINT8				GetTrigChNo();

	VOID				LoadTriggerCh();
	VOID				SetTriggerCh();


/* 공용 함수 */
public:

	/* 현재까지 측정된 정보 반환 */
	UINT32				GetGrabCount();
	UINT8				GetCheckACam();
	LPG_ZAAL			GetGrabData(UINT32 index);
	LPG_ZAAL			GetGrabSel();
	CMeasure			*GetMeasure();
	DOUBLE				GetMinMax(UINT8 mode, UINT8 type);

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
