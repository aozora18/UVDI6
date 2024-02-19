
#pragma once

#include "../../inc/comn/MyRich.h"

class CDlgE90 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgE90)

/* 생성자 & 파괴자 */
public:
	CDlgE90(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgE90();
	enum { IDD = IDD_E90 };

/* 가상 함수  */
protected:

	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual BOOL		OnInitDialog();

/* 로컬 변수 */
protected:

	BOOL				m_bExitProc;

	CListCtrl			m_lst_ctl[2];


/* 로컬 함수 */
protected:

	VOID				ExitProc();
	VOID				InitCtrl();

	VOID				UpdateSubstrateLocID();


/* 공용 함수 */
public:

	VOID				UpdateCtrl();
	VOID				UpdateCtrl(STG_CCED *event);

/* 메시지 맵 */
public:

	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnClose();
	afx_msg VOID		OnDestroy();
	afx_msg void		OnBnClickedE90BtnRemoteCtrlGet();
	afx_msg void		OnBnClickedE90BtnRemoteCtrlEnable();
	afx_msg void		OnBnClickedE90BtnRemoteCtrlDisable();
	afx_msg void		OnBnClickedE90BtnSubstLocAdd();
	afx_msg void		OnBnClickedE90BtnSubstLocDel();
	afx_msg void		OnBnClickedE90BtnSubstIdAdd();
	afx_msg void		OnBnClickedE90BtnSubstIdDel();
	afx_msg void		OnBnClickedE90BtnSubstLocUpdate();
	afx_msg void		OnBnClickedE90BtnSubstIdUpdate();
	afx_msg void		OnBnClickedE90BtnSubstLocAdds();
	afx_msg void		OnBnClickedE90BtnSubstLocDels();
	afx_msg void		OnBnClickedE90BtnSubstLocState();
	afx_msg void OnBnClickedE90BtnSubstIdProc();
	afx_msg void OnBnClickedE90BtnSubstLocStates();
	afx_msg void OnBnClickedE90BtnSubstIdTrans();
	afx_msg void OnBnClickedE90BtnSubstIdRead();
};
