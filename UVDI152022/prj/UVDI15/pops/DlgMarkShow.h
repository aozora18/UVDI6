#pragma once

// #include "../../../inc/conf/global.h"
// #include "../../../inc/conf/define.h"
// #include "../../../inc/comn/AniGif.h/"

#include "../../../inc/comn/MyDialog.h"
#include "../../../inc/grid/GridCtrl.h"
#include "../../../inc/kybd/DlgKBDN.h"

using namespace std;

class CDlgMarkShow : public CMyDialog
{
	DECLARE_DYNAMIC(CDlgMarkShow)

public:
	CDlgMarkShow(CWnd* pParent = NULL);
	virtual ~CDlgMarkShow();

	enum { IDD = IDD_MARKSHOW	};
	
	enum EN_MSG_BOX_TYPE
	{
		eINFO,
		eSTOP,
		eWARN,
		eQUEST,
	};

/* ���� �Լ� */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* pDX);
	virtual VOID		OnExitDlg()		{};
	virtual VOID		OnPaintDlg(CDC *pDc);
	virtual VOID		OnResizeDlg()	{};
	virtual void		RegisterUILevel() {}
public:

	virtual VOID		UpdatePeriod()	{};
	void				SetMarkType(BOOL bIsLocal) { m_bIsLocal = bIsLocal; }
	void				SetMarkIndex(int nIndex) { m_nMarkIndex = nIndex; }
	LPG_ACGR			GetMarkInfo(int nIndex, BOOL bIsLocal);

	int					GetMarkACamNoFromIndex(int index, BOOL bIsLocal);
	int					GetMarkImgIDFromIndex(int index, BOOL bIsLocal);
/* ���� ���� */
protected:

	HWND				m_hTxtMark;
	HWND				m_hPicMark;
	RECT				m_rPicMark;

	CMyStatic			m_txtTitle;
	CMyStatic			m_picMark;
	CMyStatic			m_txtMark;

	BOOL				m_bIsLocal;
	int					m_nMarkIndex;

/* ���� �Լ� */
protected:

	VOID				CreateControl();
	VOID				DrawInit();
	BOOL				DrawMark(int index);

/* ���� �Լ� */
public:
	LOGFONT				GetLogFont(int nSize, BOOL bIsBold);

/* �޽��� �Լ� */
protected:

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
