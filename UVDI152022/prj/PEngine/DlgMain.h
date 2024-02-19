
#pragma once

#include "../../inc/comn/MyDialog.h"
#include "../../inc/comn/MacButton.h"
#include "../../inc/comn/MyGrpBox.h"
#include "../../inc/comn/MyStatic.h"
#ifndef _DEBUG
#include "TrayIcon.h"
#endif
class CDlgEFEM;
class CDlgLogs;
class CDlgLuri;
class CDlgMC2;
class CDlgMoni;
class CDlgPlcQ;
class CDlgTrig;
class CDlgVisi;

class CDlgChild;
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

	UINT64				m_u64UpdateCtrl;	/* 자식 윈도 갱신 주기 때문에 */

	CMyGrpBox			m_grp_ctl[1];
	CMyStatic			m_txt_ctl[1];
	CMacButton			m_btn_ctl[8];
	CMacCheckBox		m_chk_ctl[6];
#ifndef _DEBUG
	CTrayIcon			m_csTray;
#endif
	CMainThread			*m_pMainThread;

	CDlgChild			*m_pDlgLast;
	CDlgEFEM			*m_pDlgEFEM;
	CDlgLogs			*m_pDlgLogs;
	CDlgLuri			*m_pDlgLuri;
	CDlgMC2				*m_pDlgMC2;
	CDlgMoni			*m_pDlgMoni;
	CDlgPlcQ			*m_pDlgPlcQ;
	CDlgTrig			*m_pDlgTrig;
	CDlgVisi			*m_pDlgVisi;
	

/* 로컬 함수 */
protected:

	VOID				InitCtrl();

	BOOL				InitLib();
	VOID				CloseLib();

	VOID				CreateChildDialog(UINT32 dlg_id);
	VOID				KillChildDialog();


/* 공용 함수 */
public:


/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT		OnMsgTrayIcon(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT		OnMsgThread(WPARAM wparam, LPARAM lparam);
	afx_msg VOID		OnSysCommand(UINT32 id, LPARAM lparam);
	afx_msg VOID		OnShowWindow(BOOL show, UINT status);
	afx_msg VOID		OnCmdAppExit();
	afx_msg VOID		OnCmdLuriaRestart();
	afx_msg VOID		OnCmdShowHide();
	afx_msg VOID		OnBtnClicked(UINT32 id);
};
