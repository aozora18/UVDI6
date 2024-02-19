
#pragma once

class CMyTabCtrl : public CTabCtrl
{
// 생성자 & 파괴자
public:
	CMyTabCtrl();
	virtual ~CMyTabCtrl();

// 가상함수 재정의
protected:

	virtual VOID		DrawItem(LPDRAWITEMSTRUCT drawitem);
	virtual VOID		PreSubclassWindow();

public:
	

// 로걸 변수
protected:

	CArray<BOOL, BOOL>	m_arrTab;

	INT32				m_i32SelectedTab;
	INT32				m_i32IdxMouseOver;

	POINT				m_ptTabs;
	COLORREF			m_crSelected;
	COLORREF			m_crDisabled;
	COLORREF			m_crNormal;
	COLORREF			m_crMouseOver;

	COLORREF			m_crTabBtnBack;	// 탭 컨트롤의 배경색
	COLORREF			m_crTabWndBack;	// 탭 영역의 배경색

	CFont				m_ftTabName;
	
	BOOL				m_bMouseOver;
	BOOL				m_bColorMouseOver;
	BOOL				m_bColorNormal;
	BOOL				m_bColorDisabled;
	BOOL				m_bColorSelected;


public:

	VOID				AddTab(CWnd* parent, LPTSTR caption, INT32 image =0);
	VOID				EnableTab(INT32 index, BOOL enable = TRUE);
	BOOL				SelectTab(INT32 index);
	VOID				DeleteAllTabs();
	VOID				DeleteTab(INT32 index);
	VOID				SetTopLeftCorner(CPoint pt);
	BOOL				IsTabEnabled(INT32 index);

	VOID				SetDisabledColor(COLORREF cr);
	VOID				SetSelectedColor(COLORREF cr);
	VOID				SetNormalColor(COLORREF cr);
	VOID				SetMouseOverColor(COLORREF cr);
	VOID				SetTabBtnBackColor(COLORREF cr)	{	m_crTabBtnBack	= cr;	}
	VOID				SetTabWndBackColor(COLORREF cr)	{	m_crTabWndBack	= cr;	}

	VOID				SetFontInfo(LOGFONT *lf);
	VOID				SetTabSize(UINT32 cx, UINT32 cy);

	CWnd				*GetTabWnd(INT32 index);


protected:

	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnSelchange(NMHDR* nmhdr, LRESULT* result);
	afx_msg VOID		OnSelchanging(NMHDR* nmhdr, LRESULT* result);
	afx_msg VOID		OnMouseMove(UINT32 flags, CPoint point);
	afx_msg VOID		OnTimer(UINT_PTR event);
	afx_msg BOOL		OnEraseBkgnd(CDC* dc) ;
};
