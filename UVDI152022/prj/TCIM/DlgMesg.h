
#pragma once

class CDlgMesg : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMesg)

/* ������ & �ı��� */
public:
	CDlgMesg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgMesg();
	enum { IDD = IDD_MESG };

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
	afx_msg VOID		OnBtnTermBtnSend();
};
