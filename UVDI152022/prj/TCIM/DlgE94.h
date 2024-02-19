
#pragma once


class CDlgE94 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgE94)

/* ������ & �ı��� */
public:
	CDlgE94(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgE94();
	enum { IDD = IDD_E94 };

/* ���� �Լ�  */
protected:

	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual BOOL		OnInitDialog();

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
	afx_msg void OnBnClickedE94BtnPrjobUpdate();
	afx_msg void OnShowWindow(BOOL show, UINT status);
	afx_msg void OnBnClickedE94BtnCrjobCreate();
	afx_msg void OnBnClickedE94BtnCrjobDelete();
	afx_msg void OnBnClickedE94BtnCrjobUpdate();
};
