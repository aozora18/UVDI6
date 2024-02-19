
#pragma once


class CDlgE40 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgE40)

/* ������ & �ı��� */
public:
	CDlgE40(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgE40();
	enum { IDD = IDD_E40 };

/* ���� �Լ�  */
protected:

	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual BOOL		OnInitDialog();

/* ���� ���� */
protected:

	BOOL				m_bExitProc;

	CCheckListBox		m_box_chk[1];


/* ���� �Լ� */
protected:

	VOID				ExitProc();
	VOID				InitCtrl();



/* ���� �Լ� */
public:

	VOID				UpdateCtrl();
	VOID				UpdateCtrl(STG_CCED *event);

/* �޽��� �� */
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
