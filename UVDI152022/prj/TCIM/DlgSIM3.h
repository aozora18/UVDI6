
#pragma once

#include "../../inc/comn/MyRich.h"

class CDlgSIM3	 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSIM3)

/* 생성자 & 파괴자 */
public:
	CDlgSIM3(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSIM3();
	enum { IDD = IDD_SIM3 };

/* 가상 함수  */
protected:

	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual BOOL		OnInitDialog();

/* 로컬 변수 */
protected:

	UINT8				m_u8StepMain, m_u8StepSub, m_u8StepMPT;
	BOOL				m_bExitProc;

	CListCtrl			m_lst_ctl[1];


/* 로컬 함수 */
protected:

	VOID				ExitProc();
	VOID				InitCtrl();

	VOID				UpdateCallbackEventE87(STG_CCED *event);

	VOID				UpdateSubstrateLocation();
	VOID				UpdateProcessingState();

	VOID				UpdateSubstrateState(PTCHAR subst_id, LPG_E90_SC data);
	VOID				UpdateSubstrateCommand(E90_SPSC command, PTCHAR subst_id);

	INT32				GetEmptySlotNumInCarrier();
	INT32				GetNotEmptySlotNumInCarrier();
	INT32				GetNeedProcessingSlotNumInCarrier();

	VOID				SetIDStateAndAcquiredID(INT32 slot_no, PTCHAR subst_id);
	BOOL				GetSelectPRJobID(PTCHAR pj_id);
	VOID				LoadDestinateSubstrateID(PTCHAR carr_id, UINT8 slot_no);
	INT32				GetFindSubstrateIDtoSlotNo(UINT8 type, PTCHAR subst_id);

	VOID				LoadSubstIDForProcessJob();
	VOID				UpdateControlJobList();
	VOID				UpdateProcessJobList();
	VOID				UpdateBaseInformation(UINT8 kind);
	VOID				UpdateLoadPortCarrier();

	BOOL				HasWafer(ENG_TSSI location_id);

/* 공용 함수 */
public:

	VOID				UpdateCtrl();
	VOID				UpdateCtrl(STG_CCED *event);

/* 메시지 맵 */
public:

	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnClose();
	afx_msg VOID		OnDestroy();
	afx_msg VOID		OnBnClickedSim3BtnLoad();
	afx_msg VOID		OnBnClickedSim3BtnUnload();
	afx_msg VOID		OnBnClickedSim3BtnAccess();
	afx_msg VOID		OnBnClickedSim3ChkSlotMapRand();
	afx_msg VOID		OnBnClickedSim3ChkSlotMapOccupied();
	afx_msg VOID		OnBnClickedSim3BtnJobProcess();
	afx_msg VOID		OnBnClickedSim3BtnProcWithSubst();
	afx_msg VOID		OnBnClickedSim3BtnPrjobCompleted();
	afx_msg VOID		OnBnClickedExpose(UINT32 id);
	afx_msg void		OnBnClickedSim3BtnPrjobStart();
	afx_msg void		OnBnClickedSim3BtnReleaseCarrier();
	afx_msg void		OnBnClickedSim3BtnPrjobProcComplete();
	afx_msg void OnBnClickedSim3BtnPrjobStop();
	afx_msg void OnBnClickedSim3BtnPrjobAbort();
	afx_msg void OnBnClickedSim3BtnPrjobRemove();
	afx_msg void OnLbnSelchangeSim3BoxCrjob();
	afx_msg void OnBnClickedSim3BtnPrjobUpdate();
	afx_msg void OnBnClickedSim3BtnCrjobUpdate();
	afx_msg void OnLbnDblclkSim3BoxPrjob();
	afx_msg void OnBnClickedSim3BtnCarrReCreate();
	afx_msg void OnBnClickedSim3BtnCancelCarrier();
	afx_msg void OnBnClickedSim3BtnLoad2();
};
