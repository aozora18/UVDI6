
#pragma once


class CDlgPJM	 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgPJM)

/* 생성자 & 파괴자 */
public:
	CDlgPJM(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgPJM();
	enum { IDD = IDD_PJM };

/* 가상 함수  */
protected:

	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual BOOL		OnInitDialog();

/* 열거형 or 구조체 */
protected:


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
	afx_msg void OnBnClickedPjmBtnPjStateUpdate();
	afx_msg void OnBnClickedPjmBtnPjListGet();
};
