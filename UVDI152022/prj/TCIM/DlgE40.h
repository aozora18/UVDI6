
#pragma once


class CDlgE40 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgE40)

/* 생성자 & 파괴자 */
public:
	CDlgE40(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgE40();
	enum { IDD = IDD_E40 };

/* 가상 함수  */
protected:

	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual BOOL		OnInitDialog();

/* 로컬 변수 */
protected:

	BOOL				m_bExitProc;

	CCheckListBox		m_box_chk[1];


/* 로컬 함수 */
protected:

	VOID				ExitProc();
	VOID				InitCtrl();



/* 공용 함수 */
public:

	VOID				UpdateCtrl();
	VOID				UpdateCtrl(STG_CCED *event);

/* 메시지 맵 */
public:

	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnClose();
	afx_msg VOID		OnDestroy();
	afx_msg void OnBnClickedE40BtnProcJobSelAll();
	afx_msg void OnBnClickedE40BtnProcJobClear();
	afx_msg void OnBnClickedE40BtnProcJobAdd();
	afx_msg void OnBnClickedE40BtnProcJobDel();
	afx_msg void OnBnClickedE40BtnPrGetSpace();
	afx_msg void OnBnClickedE40ProcState(UINT32 id);
	afx_msg void OnBnClickedE40BtnProjobCmd();
	afx_msg void OnBnClickedE40BtnPauseEventGet();
	afx_msg void OnBnClickedE40BtnPauseEventEnable();
	afx_msg void OnBnClickedE40BtnPauseEventDisable();
	afx_msg void OnBnClickedE40BtnNotiEventGet();
	afx_msg void OnBnClickedE40BtnNotiEventE30();
	afx_msg void OnBnClickedE40BtnNotiEventE40();
	afx_msg void OnBnClickedE40BtnNotiEventBoth();
	afx_msg void OnBnClickedE40BtnPrJobAlert();
	afx_msg void OnBnClickedE40BtnWaitPrAllGet();
	afx_msg void OnBnClickedE40BtnWaitPrAllYes();
	afx_msg void OnBnClickedE40BtnWaitPrAllNo();
	afx_msg void OnBnClickedE40BtnCtrljobSet();
	afx_msg void OnBnClickedE40BtnPrJobSubstUpdate();
};
