
#pragma once

#include "../../inc/comn/MyListCtrl.h"

class CDlgGEM1 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgGEM1)

/* 생성자 & 파괴자 */
public:
	CDlgGEM1(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgGEM1();
	enum { IDD = IDD_GEM1 };

/* 가상 함수  */
protected:

	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual BOOL		OnInitDialog();

/* 로컬 변수 */
protected:

	BOOL				m_bExitProc;

	CMyListCtrl			m_lst_var;
	CMyListCtrl			m_lst_cmd;
	CMyListCtrl			m_lst_rpt;
	CMyListCtrl			m_lst_ret;
	CMyListCtrl			m_lst_prj;
	CMyListCtrl			m_lst_trc;
	CMyListCtrl			m_lst_arm;


/* 로컬 함수 */
protected:

	VOID				ExitProc();
	VOID				InitCtrl();

	VOID				AlarmGet();
	VOID				AlarmClear();
	VOID				AlarmSet();

	VOID				SetLogLevel();

	VOID				UpdateSpoolCountActual();	/* 2016 */
	VOID				UpdateSpoolCountTotal();	/* 2017 */
	VOID				UpdateSpoolFullTime();		/* 2018 */
	VOID				UpdateSpoolStartTime();		/* 2019 */

	VOID				UpdateAlarmsEnabled();		/* 2026 */

	VOID				UpdateCtrlStateSwitch();	/* 2033 */
	VOID				UpdateCtrlOnlineSwitch();	/* 2034 */
	VOID				UpdateCommEnableSwitch();	/* 2035 */

	VOID				UpdateCommState(LONG state);			/* 2036 */
	VOID				UpdateCtrlState(LONG state);			/* 2032 */
	VOID				UpdateSpoolState(LONG state);			/* 2037 */
	VOID				UpdateOverwriteSpool();		/* 4009 */
	VOID				UpdateSpoolEnabled();		/* 4010 */

	VOID				UpdateAlarmState(STG_CCED *event);
	VOID				UpdateConnectState();

/* 공용 함수 */
public:

	VOID				UpdateCtrl();
	VOID				UpdateCtrl(STG_CCED *event);


/* 메시지 맵 */
protected:

	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg VOID		OnClose();
	afx_msg VOID		OnDestroy();
	afx_msg VOID		OnBtnClicked(UINT32 id);
	afx_msg VOID		OnChkClicked(UINT32 id);
	afx_msg VOID		OnBnClickedCtrlBtnVarMinMax();
	afx_msg VOID		OnBnClickedCtrlBtnVarType();
	afx_msg VOID		OnBnClickedCtrlBtnLogStart();
	afx_msg VOID		OnBnClickedCtrlBtnLogStop();
	afx_msg VOID		OnBnClickedCtrlBtnCmdGet();
	afx_msg VOID		OnBnClickedCtrlBtnReportGet();
	afx_msg VOID		OnBnClickedCtrlBtnReporteventGet();
	afx_msg VOID		OnBnClickedCtrlBtnProjectGet();
	afx_msg VOID		OnBnClickedCtrlBtnTraceGet();
	afx_msg VOID		OnBnClickedGem1BtnSaveEpj();
	afx_msg VOID		OnItemClickGem1LstAlarms(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void		OnNMDblclkGem1LstVarItems(NMHDR *pNMHDR, LRESULT *pResult);
};
