
#pragma once


class CDlgCJM	 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCJM)

/* ������ & �ı��� */
public:
	CDlgCJM(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgCJM();
	enum { IDD = IDD_CJM };

/* ���� �Լ�  */
protected:

	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual BOOL		OnInitDialog();

/* ������ or ����ü */
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

/* ���� ���� */
protected:

	BOOL				m_bExitProc;

	CListCtrl			m_lst_ctl[1];


/* ���� �Լ� */
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

/* ���� �Լ� */
public:

	VOID				UpdateCtrl();
	VOID				UpdateCtrl(STG_CCED *event);


/* �޽��� �� */
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
