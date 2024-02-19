
#pragma once

class CDlgTerm : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgTerm)

/* 생성자 & 파괴자 */
public:
	CDlgTerm(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgTerm();
	enum { IDD = IDD_TERM };

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
protected:

	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg VOID		OnClose();
	afx_msg VOID		OnDestroy();
	afx_msg VOID		OnBtnTermBtnAck();
	afx_msg VOID		OnBtnTermBtnSend();
};
