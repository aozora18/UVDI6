
#pragma once

class CDlgTerm : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgTerm)

/* ������ & �ı��� */
public:
	CDlgTerm(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgTerm();
	enum { IDD = IDD_TERM };

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
protected:

	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg VOID		OnClose();
	afx_msg VOID		OnDestroy();
	afx_msg VOID		OnBtnTermBtnAck();
	afx_msg VOID		OnBtnTermBtnSend();
};
