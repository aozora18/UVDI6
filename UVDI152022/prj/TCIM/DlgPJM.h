
#pragma once


class CDlgPJM	 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgPJM)

/* ������ & �ı��� */
public:
	CDlgPJM(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgPJM();
	enum { IDD = IDD_PJM };

/* ���� �Լ�  */
protected:

	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual BOOL		OnInitDialog();

/* ������ or ����ü */
protected:


/* ���� ���� */
protected:

	BOOL				m_bExitProc;



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
	afx_msg void OnBnClickedPjmBtnPjStateUpdate();
	afx_msg void OnBnClickedPjmBtnPjListGet();
};
