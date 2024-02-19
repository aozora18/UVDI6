
#pragma once

#include "../../inc/comn/MyDialog.h"
#include "../../inc/comn/MacButton.h"
#include "../../inc/comn/MyGrpBox.h"
#include "../../inc/comn/MyStatic.h"
#include "../../inc/comn/MyEdit.h"
#include "../../inc/comn/MyListBox.h"

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

	CMainThread			*m_pMainThread;


/* 로컬 함수 */
protected:

	BOOL				InitCtrl();

	BOOL				InitLib();
	VOID				CloseLib();

	VOID				SendCmd();


/* 공용 함수 */
public:



/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT		OnMsgMainThread(WPARAM wparam, LPARAM lparam);
	afx_msg VOID		OnSysCommand(UINT32 id, LPARAM lparam);
public:
	afx_msg void OnBnClickedMainBtnCmdSetHomed();
	afx_msg void OnBnClickedMainBtnCmdGetHomed();
	afx_msg void OnBnClickedMainBtnCmdSetCurrent();
	afx_msg void OnBnClickedMainBtnCmdGetCurrent();
	afx_msg void OnBnClickedMainBtnCmdSetDirect();
	afx_msg void OnBnClickedMainBtnCmdGetDirect();
	afx_msg void OnBnClickedMainBtnCmdGetAxisPos();
	afx_msg void OnBnClickedMainBtnCmdGetDiagC();
	afx_msg void OnBnClickedMainBtnCmdGetEst();
	afx_msg void OnBnClickedMainBtnCmdGetPer();
	afx_msg void OnBnClickedMainBtnCmdGetRhs();
	afx_msg void OnBnClickedMainBtnAppExit();
	afx_msg void OnBnClickedMainBtnCmdSend();
	afx_msg void OnBnClickedMainBtnCmdReset();
	afx_msg void OnBnClickedMainBtnCmdSetHomeOffset();
	afx_msg void OnBnClickedMainBtnCmdGetHomeOffset();
	afx_msg void OnBnClickedMainBtnCmdSetSpeed();
	afx_msg void OnBnClickedMainBtnCmdGetSpeed();
	afx_msg void OnBnClickedMainBtnCmdSetJerk();
	afx_msg void OnBnClickedMainBtnCmdGetJerk();
	afx_msg void OnBnClickedMainBtnCmdSetAxisAllHoming();
	afx_msg void OnBnClickedMainBtnCmdSetAxisMove();
	afx_msg void OnBnClickedMainBtnCmdSetServoOff();
	afx_msg void OnBnClickedMainBtnCmdSetServoOn();
	afx_msg void OnBnClickedMainBtnCmdSetAxisStop();
	afx_msg void OnBnClickedMainBtnCmdSetVacOn();
	afx_msg void OnBnClickedMainBtnCmdSetVacOff();
	afx_msg void OnBnClickedMainBtnCmdGetNotchSucc();
	afx_msg void OnBnClickedMainBtnCmdGetNotchFail();
};
