
#pragma once


class CDlgE94 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgE94)

/* 생성자 & 파괴자 */
public:
	CDlgE94(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgE94();
	enum { IDD = IDD_E94 };

/* 가상 함수  */
protected:

	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual BOOL		OnInitDialog();

/* 로컬 변수 */
protected:

	BOOL				m_bExitProc;



/* 로컬 함수 */
protected:

	VOID				ExitProc();
	VOID				InitCtrl();



/* 공용 함수 */
public:

	VOID				UpdateCtrl();
	VOID				UpdateCtrl(STG_CCED *event);

/* 메시지 맵 */
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
