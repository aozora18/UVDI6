
#pragma once

class CDlgCIM3 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCIM3)

/* 생성자 & 파괴자 */
public:
	CDlgCIM3(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgCIM3();
	enum { IDD = IDD_CIM3 };

/* 가상 함수  */
protected:

	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual BOOL		OnInitDialog();

/* 로컬 변수 */
protected:

	BOOL				m_bExitProc;

	CListCtrl			m_lst_proc;	/* Process Job ID & Status */
	CListCtrl			m_lst_ctrl;	/* Control Job ID & Status */


/* 로컬 함수 */
protected:

	VOID				ExitProc();
	VOID				InitCtrl();

	/* Process Job */
	VOID				SetSubstrateChangeRobotArm(UINT8 mode);

	/* Utility */

/* 공용 함수 */
public:

	VOID				UpdateCtrl();
	VOID				UpdateCtrl(STG_CCED *event);

/* 메시지 맵 */
protected:

	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnClose();
	afx_msg VOID		OnDestroy();
	afx_msg VOID		OnChkClicked(UINT32 id);
	afx_msg VOID		OnBnClickedProcWithCarr();
	afx_msg VOID		OnBnClickedCancelCarrier();
	afx_msg VOID		OnBnClickedCarrReCreate();
	afx_msg VOID		OnBnClickedReleaseCarrier();
	afx_msg VOID		OnBnClickedProcWithSubst();
	afx_msg VOID		OnBnClickedCarrierTransferStart();
	afx_msg VOID		OnBnClickedSubstrateChangePreAligner();
	afx_msg VOID		OnBnClickedSubstrateChangeChamber();
	afx_msg VOID		OnBnClickedSubstrateCarrierInsert();
	afx_msg VOID		OnBnClickedCarrierAccessing();
	afx_msg VOID		OnBnClickedCarrierAccessComplete();
	afx_msg VOID		OnBnClickedSelectedControlJob();
	afx_msg VOID		OnBnClickedDeselectedControlJob();
	afx_msg VOID		OnBnClickedCarrierPlaced();
	afx_msg VOID		OnBnClickedCarrierClamped();
	afx_msg VOID		OnBnClickedCarrierAtPort();
	afx_msg VOID		OnBnClickedMaterialReceived();
	afx_msg VOID		OnBnClickedCarrierMoveDock();
	afx_msg VOID		OnBnClickedCarrierOpened();
	afx_msg VOID		OnBnClickedCarrierClosed();
	afx_msg VOID		OnBnClickedReadSlotMap();
	afx_msg VOID		OnBnClickedCarrierMoveUndock();
	afx_msg VOID		OnBnClickedCarrierUnclamped();
	afx_msg VOID		OnBnClickedCarrierReadyUnload();
	afx_msg VOID		OnBnClickedCarrierDepartedPort();
	afx_msg VOID		OnBnClickedMaterialRemoved();
	afx_msg VOID		OnBnClickedPRJobStartProcess();
	afx_msg VOID		OnBnClickedSubstrateCarrierRemoved();
	afx_msg void OnBnClickedCim3BtnLoadMaterialList();
	afx_msg void OnBnClickedProcJobAllGet();
	afx_msg void OnBnClickedCtrlJobAllGet();
	afx_msg VOID		OnBnClickedPRJobCtrl(UINT32 id);
public:
	afx_msg void OnBnClickedCim3BtnPrjobCompleted();
};
