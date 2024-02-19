
#pragma once


class CDlgCJM	 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCJM)

/* 생성자 & 파괴자 */
public:
	CDlgCJM(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgCJM();
	enum { IDD = IDD_CJM };

/* 가상 함수  */
protected:

	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual BOOL		OnInitDialog();

/* 열거형 or 구조체 */
protected:

	typedef enum class __en_e94_cjm_control_job_state__
	{
		Sel,
		No,

		Id,
		State,
		CarrierInputSpec,
		ProcessJobs,
		AutoStart,
		CompletedTime,
		MaterialArraived,

	} E94_COL;

/* 로컬 변수 */
protected:

	BOOL				m_bExitProc;

	CListCtrl			m_lst_ctl[1];


/* 로컬 함수 */
protected:

	VOID				ExitProc();
	VOID				InitCtrl();
	VOID				UpdateCJAction();

	INT32				GetSelectedRow();
	E94_CCJS			GetState(INT32 row);
	E94_CCJS			ToStateEnum(CString text);
	CString				E94_CCJS_ToString(E94_CCJS state);
	CString				GetStringFromList(CStringList & listValue);
	BOOL				IsSelected(INT32 row);

	VOID				ClearSelection();

/* 공용 함수 */
public:

	VOID				UpdateCtrl();
	VOID				UpdateCtrl(STG_CCED *event);


/* 메시지 맵 */
public:

	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnClose();
	afx_msg VOID		OnDestroy();
	afx_msg void OnBnClickedCjmBtnClearSelection();
	afx_msg void OnBnClickedCjmBtnStart();
	afx_msg void OnBnClickedCjmBtnStop();
	afx_msg void OnBnClickedCjmBtnAbort();
	afx_msg void OnBnClickedCjmBtnPause();
	afx_msg void OnBnClickedCjmBtnResume();
	afx_msg void OnBnClickedCjmBtnSelect();
	afx_msg void OnBnClickedCjmBtnDeselect();
	afx_msg void OnBnClickedCjmBtnHoq();
	afx_msg void OnBnClickedCjmBtnCancel();
	afx_msg void OnNMClickCjmLstCjstatus(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCjmBtnRun();
	afx_msg void OnBnClickedCjmBtnUpdate();
};
