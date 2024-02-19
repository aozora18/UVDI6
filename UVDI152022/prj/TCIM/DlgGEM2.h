
#pragma once

class CDlgGEM2 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgGEM2)

/* 생성자 & 파괴자 */
public:
	CDlgGEM2(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgGEM2();
	enum { IDD = IDD_GEM2 };

/* 가상 함수  */
protected:

	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual BOOL		OnInitDialog();

/* 로컬 변수 */
protected:

	BOOL				m_bExitProc;

	CListCtrl			m_lst_var;
	CListCtrl			m_lst_di;
	CListCtrl			m_lst_cmd;


/* 로컬 함수 */
protected:

	VOID				ExitProc();
	VOID				InitCtrl();

	VOID				UpdateRemoteCtrl(STG_CCED *event);
	VOID				UpdateECChangeInfo(STG_CCED *event);
	VOID				UpdateUserValueChanged(LONG host_id, UINT32 var_id);

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
	afx_msg VOID		OnBnClickedCtrlBtnListVarGet();
	afx_msg VOID		OnBnClickedCtrlBtnListValGet();
	afx_msg VOID		OnBnClickedCtrl2BtnCommIfGet();
	afx_msg VOID		OnBnClickedCtrl2BtnPpHostEnable();
	afx_msg VOID		OnBnClickedCtrl2BtnPpHostDisable();
	afx_msg VOID		OnBnClickedCtrl2BtnPpMgt0();
	afx_msg VOID		OnBnClickedCtrl2BtnPpMgt1();
	afx_msg VOID		OnBnClickedCtrl2BtnPpMgt2();
	afx_msg VOID		OnBnClickedCtrl2BtnLogSetEnable();
	afx_msg VOID		OnBnClickedCtrl2BtnLogSetDisable();
	afx_msg VOID		OnBnClickedCtrl2BtnRemotecmdEnable();
	afx_msg VOID		OnBnClickedCtrl2BtnRemotecmdDisable();
	afx_msg void OnBnClickedGem2BtnBuildVerGet();
	afx_msg void OnBnClickedGem2BtnE87AlarmReset();
	afx_msg void OnBnClickedGem2BtnCimconnectLog();
	afx_msg void OnBnClickedGem2BtnCimconnectLogYes();
	afx_msg void OnBnClickedGem2BtnCimconnectLogNo();
	afx_msg void OnBnClickedGem2BtnListValDiList();
	afx_msg void OnBnClickedGem2BtnListValDiSet();
	afx_msg void OnBnClickedGem2BtnSecsS14f19();
	afx_msg void OnBnClickedGem2BtnSecsS1f1();
};
