
#pragma once

#include "../../inc/comn/MyRich.h"

class CDlgE87	 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgE87)

/* ������ & �ı��� */
public:
	CDlgE87(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgE87();
	enum { IDD = IDD_E87 };

/* ���� �Լ�  */
protected:

	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual BOOL		OnInitDialog();

/* ���� ���� */
protected:

	BOOL				m_bExitProc;
	BOOL				m_bRecipeState;
	UINT64				m_u64TickRecipeState;

	CMyRich				m_rch_ctl[1];


/* ���� �Լ� */
protected:

	VOID				ExitProc();
	VOID				InitCtrl();

	/* Process Job */

	/* Utility */

	VOID				UpdatePortAccessMode(LPG_E87_CF data);
	VOID				UpdateLoadPortTransferStatus(LPG_E87_CF data);
	VOID				UpdateCarrierAllOut();
	VOID				UpdateCarrierService(UINT16 cbf_id);
	VOID				UpdateRecipeState();

/* ���� �Լ� */
public:

	VOID				UpdateCtrl();
	VOID				UpdateCtrl(STG_CCED *event);

/* �޽��� �� */
public:

	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnClose();
	afx_msg VOID		OnDestroy();
	afx_msg VOID		OnBnClickedSim3BtnLoad();
	afx_msg void OnBnClickedSim3BtnUnload();
	afx_msg void OnBnClickedSim3BtnAccess();
	afx_msg void OnBnClickedE87BtnRemoteCtrlEnable();
	afx_msg void OnBnClickedE87BtnRemoteCtrlDisable();
	afx_msg void OnBnClickedE87BtnLoadPortNo();
	afx_msg void OnBnClickedE87BtnLoadPortCarr();
	afx_msg void OnBnClickedE87BtnTransactionId();
	afx_msg void OnBnClickedE87BtnTransactionReply();
	afx_msg void OnBnClickedE87BtnCarrOutBegin();
	afx_msg void OnBnClickedE87BtnCarrOut();
	afx_msg void OnBnClickedE87BtnCarrIn();
	afx_msg void OnBnClickedE87BtnCarrOutAll();
	afx_msg void OnBnClickedE87BtnCarrIdVerifyGet();
	afx_msg void OnBnClickedE87BtnCarrIdVerifyPut();
	afx_msg void OnBnClickedE87BtnCarrIdAccessGet();
	afx_msg void OnBnClickedE87BtnCarrIdAccessPut();
	afx_msg void OnBnClickedE87BtnLoadPortStateGet();
	afx_msg void OnBnClickedE87BtnLoadPortStateSet();
	afx_msg void OnBnClickedE87BtnCarrIdNoti();
	afx_msg void OnBnClickedE87BtnCarrIdAccessStGet();
	afx_msg void OnBnClickedE87BtnCarrIdAccessStSet();
	afx_msg void OnBnClickedE87BtnRecipeNew();
	afx_msg void OnBnClickedE87BtnRecipeEdit();
	afx_msg void OnBnClickedE87BtnRecipeDel();
	afx_msg void OnBnClickedE87BtnRecipeOk();
	afx_msg void OnBnClickedE87BtnRecipeRun();
};
