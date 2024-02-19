

#pragma once

#include "../../inc/comn/MyTabCtrl.h"
#include "../../inc/comn/MyRich.h"

class CMainThread;
class CDlgGEM1;
class CDlgGEM2;
class CDlgTerm;
class CDlgCIM3;
class CDlgSIM3;
class CDlgE40;
class CDlgE87;
class CDlgE90;
class CDlgE94;
class CDlgCJM;
class CDlgPJM;
class CDlgMesg;
class CDlgCbks;

class CDlgMain : public CDialogEx
{

/* ������ */
public:
	CDlgMain(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	enum { IDD = IDD_MAIN };

/* ���� �Լ� */
protected:
	virtual VOID		DoDataExchange(CDataExchange* dx);	// DDX/DDV �����Դϴ�.
	virtual BOOL		OnInitDialog();


/* ���� ���� */
protected:

	BOOL				m_bExitProc;

	HICON				m_hIcon;

	CMainThread			*m_pMainThread;

	CMyTabCtrl			m_tab_ctl[1];
	CMyRich				m_rch_ctl[2];
	CListCtrl			m_lst_ctl[3];

	CDlgGEM1			*m_pDlgGEM1;
	CDlgGEM2			*m_pDlgGEM2;
	CDlgTerm			*m_pDlgTerm;
	CDlgCIM3			*m_pDlgCIM3;
	CDlgSIM3			*m_pDlgSIM3;
	CDlgMesg			*m_pDlgMesg;
	CDlgE40				*m_pDlgE40;
	CDlgE87				*m_pDlgE87;
	CDlgE90				*m_pDlgE90;
	CDlgE94				*m_pDlgE94;
	CDlgCJM				*m_pDlgCJM;
	CDlgPJM				*m_pDlgPJM;
	CDlgCbks			*m_pDlgCbks;


/* ���� �Լ� */
protected:

	VOID				ExitProc();
	VOID				CloseChild(CDialogEx *child);
	VOID				UpdatePeriod();

	BOOL				InitTabChild();


/* ���� �Լ� */
public:


/* �޽��� �� */
protected:
public:
	DECLARE_MESSAGE_MAP()
	afx_msg HCURSOR		OnQueryDragIcon();
	afx_msg VOID		OnPaint();
	afx_msg VOID		OnClose();
	afx_msg VOID		OnDestroy();
	afx_msg LRESULT		OnMsgMainThread(WPARAM wparam, LPARAM lparam);
	afx_msg VOID		OnBnClickedMainBtnClear();
	afx_msg VOID		OnBnClickedMainBtnInitCim();
	afx_msg VOID		OnBnClickedMainBtnCloseCim();
	afx_msg VOID		OnBnClickedMainBtnClearMsg();
	afx_msg VOID		OnBnClickedMainBtnSetAppname();
	afx_msg VOID		OnBnClickedMainBtnGetAppList();
	afx_msg VOID		OnBnClickedMainBtnGetHostList();
	afx_msg VOID		OnBnClickedMainBtnGetEquipList();
	afx_msg VOID		OnBnClickedMainBtnUpdate();
};
