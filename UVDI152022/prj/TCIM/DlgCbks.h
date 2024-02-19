
#pragma once

class CDlgCbks : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCbks)

/* ������ & �ı��� */
public:
	CDlgCbks(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgCbks();
	enum { IDD = IDD_CBKS };

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
};
