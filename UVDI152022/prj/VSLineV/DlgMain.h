
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

#if 0
	/* Normal */
	CMyGrpBox			m_grp_ctl[8];
	CMyStatic			m_txt_ctl[21];
	CMacButton			m_btn_ctl[15];

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
#endif
	CMainThread			*m_pMainThread;

	CMySection			m_csSyncTrig;


/* 로컬 함수 */
protected:

	VOID				InitCtrl();
	BOOL				InitLib();
	VOID				CloseLib();


/* 공용 함수 */
public:



/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT		OnMsgThread(WPARAM wparam, LPARAM lparam);
	afx_msg VOID		OnSysCommand(UINT32 id, LPARAM lparam);
	afx_msg VOID		OnBtnClicked(UINT32 id);
#if 0
	afx_msg VOID		OnChkClickACam(UINT32 id);
	afx_msg VOID		OnChkClickOption(UINT32 id);
#endif
};
