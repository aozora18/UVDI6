
#include "pch.h"
#include "MyTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* ------ */
/* 사용법 */
/* ------ */
//	m_pMyDlg = new CMyDlg;
//	m_pMyDlg->Create(CMyDlg::IDD, &m_tabctrl /* CXTabCtrl variable */);
//	m_tabctrl.AddTab(m_pMyDlg, "Tab caption", 0 /* image number */);

/*
 desc : 생성자
 parm : None
 retn : None
*/
CMyTabCtrl::CMyTabCtrl()
{
	m_i32SelectedTab	= 0;
	m_ptTabs.x			= 5;
	m_ptTabs.y			= 25;
	m_crSelected		= 0;
	m_crDisabled		= 0;
	m_crNormal			= 0;
	m_crMouseOver		= 0;
	m_bColorSelected	= FALSE;
	m_bColorDisabled	= FALSE;
	m_bColorNormal		= FALSE;
	m_bColorMouseOver	= FALSE;
	m_crTabBtnBack		= ::GetSysColor(COLOR_BTNTEXT);
	m_crTabWndBack		= ::GetSysColor(COLOR_BTNFACE);

	m_i32IdxMouseOver = -1;

	m_bMouseOver = FALSE;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CMyTabCtrl::~CMyTabCtrl()
{
	m_arrTab.RemoveAll();
}

BEGIN_MESSAGE_MAP(CMyTabCtrl, CTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE,	OnSelchange)
	ON_NOTIFY_REFLECT(TCN_SELCHANGING,	OnSelchanging)
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/*
 desc : 폰트 설정
 parm : lf	- [in]  LOGFONT 포인터 객체
 retn : None
*/
VOID CMyTabCtrl::SetFontInfo(LOGFONT *lf)
{
	if (m_ftTabName.GetSafeHandle())		m_ftTabName.DeleteObject();
	if (m_ftTabName.CreateFontIndirect(lf))	SetFont(&m_ftTabName);

	Invalidate(TRUE);
}

/*
 desc : Tab 크기
 parm : cx	- [in]  넓이
		cy	- [in]  높이
 retn : None
*/
VOID CMyTabCtrl::SetTabSize(UINT32 cx, UINT32 cy)
{
	CSize szTab;

	szTab.cx	= cx;
	szTab.cy	= cy;

	SetItemSize(szTab);
}

VOID CMyTabCtrl::SetTopLeftCorner(CPoint pt)
{
	m_ptTabs.x			= pt.x;
	m_ptTabs.y			= pt.y;
}

VOID CMyTabCtrl::SetMouseOverColor(COLORREF cr) 
{
	m_bColorMouseOver	= TRUE;
	m_crMouseOver		= cr;
}

VOID CMyTabCtrl::SetDisabledColor(COLORREF cr) 
{
	m_bColorDisabled	= TRUE;
	m_crDisabled		= cr;
}

VOID CMyTabCtrl::SetSelectedColor(COLORREF cr)
{
	m_bColorSelected	= TRUE;
	m_crSelected		= cr;
}

VOID CMyTabCtrl::SetNormalColor(COLORREF cr)
{
	m_bColorNormal		= TRUE;
	m_crNormal			= cr;
}

VOID CMyTabCtrl::AddTab(CWnd* parent, LPTSTR caption, INT32 image)
{
	ASSERT_VALID(parent);

	TCITEM item;
	item.mask = TCIF_TEXT|TCIF_PARAM|TCIF_IMAGE;
	item.lParam		= (LPARAM)parent;
	item.pszText	= caption;
	item.iImage		= image;

	INT32 i32Index	= (INT32)m_arrTab.GetSize();
	InsertItem(i32Index, &item);
#if 1
	parent->SetWindowPos(NULL, m_ptTabs.x, m_ptTabs.y , 0, 0,
						 SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOZORDER);
#else
	CRect rClient;
	GetClientRect(rClient);
	rClient.left	= m_ptTabs.x;
	rClient.top		= m_ptTabs.y;
	rClient.right	-= m_ptTabs.x;
	rClient.bottom	-= m_ptTabs.x;
	parent->MoveWindow(rClient);
#endif
	parent->ShowWindow(i32Index ? SW_HIDE : SW_SHOW);

	//** the initial status is enabled
	m_arrTab.Add(TRUE);
}

VOID CMyTabCtrl::EnableTab(INT32 index, BOOL enable)
{
	ASSERT(index < m_arrTab.GetSize());

	//** if it should change the status ----
	if (m_arrTab[index] != enable)
	{
		m_arrTab[index] = enable;
		//** redraw the item -------
		CRect rect;
		GetItemRect(index, &rect);
		InvalidateRect(rect);
	}
}

VOID CMyTabCtrl::DeleteAllTabs()
{
	m_arrTab.RemoveAll();
	DeleteAllItems();
}

VOID CMyTabCtrl::DeleteTab(INT32 index)
{
	ASSERT(index < m_arrTab.GetSize());
	m_arrTab.RemoveAt(index);
	DeleteItem(index);
}

BOOL CMyTabCtrl::IsTabEnabled(INT32 index)
{
	ASSERT(index < m_arrTab.GetSize());
	return m_arrTab[index];
}

BOOL CMyTabCtrl::SelectTab(INT32 index)
{
	ASSERT(index < m_arrTab.GetSize());
	if (GetCurSel() == index)	return TRUE;
	//** check if the tab is enabled --------
	if (m_arrTab.GetAt(index))
	{
		TCITEM item;
		CWnd* pWnd;
		item.mask = TCIF_PARAM;

		GetItem(GetCurSel(), &item);
		pWnd = reinterpret_cast<CWnd*> (item.lParam);
		ASSERT_VALID(pWnd);
		pWnd->ShowWindow(SW_HIDE);

		SetCurSel(index);
		GetItem(index, &item);
		pWnd = reinterpret_cast<CWnd*> (item.lParam);
		ASSERT_VALID(pWnd);
		pWnd->ShowWindow(SW_SHOW);

		return TRUE;
	}

	return FALSE;
}

/*
 desc : 해당 인덱스 위치에 등록된 윈도 객체 포인터 반환
 parm : index	- [in] 윈도가 저장된 위치 (Zero-based)
 retn : 객체 포인터. 검색 실패이면 NULL 반환
*/
CWnd *CMyTabCtrl::GetTabWnd(INT32 index)
{
	TCITEM stItem	= {NULL};
	CWnd* pWnd		= NULL;

	if (index >= m_arrTab.GetSize())	return NULL;
	if (!m_arrTab.GetAt(index))			return NULL;

	stItem.mask	= TCIF_PARAM;
	if (!GetItem(index, &stItem))		return NULL;
	pWnd	= reinterpret_cast <CWnd *> (stItem.lParam);
	
	return pWnd;
}

VOID CMyTabCtrl::OnSelchange(NMHDR* nmhdr, LRESULT* result) 
{
	INT32 i32NewTab = GetCurSel();
	if (!IsTabEnabled(i32NewTab))
	{
		SetCurSel(m_i32SelectedTab);
	}
	else
	{
		TCITEM item;
		CWnd* pWnd;

		item.mask = TCIF_PARAM;
	
		//** hide the current tab ---------
		GetItem(m_i32SelectedTab, &item);
		pWnd = reinterpret_cast<CWnd*> (item.lParam);
		ASSERT_VALID(pWnd);
		pWnd->ShowWindow(SW_HIDE);
		//** show the selected tab --------
		GetItem(i32NewTab, &item);
		pWnd = reinterpret_cast<CWnd*> (item.lParam);
		ASSERT_VALID(pWnd);
		pWnd->ShowWindow(SW_SHOW);
	}
	*result = 0;
}

VOID CMyTabCtrl::OnSelchanging(NMHDR* nmhdr, LRESULT* result) 
{
	m_i32SelectedTab = GetCurSel();
	*result = 0;
}

VOID CMyTabCtrl::PreSubclassWindow() 
{
	CTabCtrl::PreSubclassWindow();
	ModifyStyle(0, TCS_OWNERDRAWFIXED);
}

VOID CMyTabCtrl::DrawItem(LPDRAWITEMSTRUCT drawitem) 
{	
	CRect rect			= drawitem->rcItem;
	rect.top			+= ::GetSystemMetrics(SM_CYEDGE);
	INT32 i32TabIndex	= drawitem->itemID;
	
	if (i32TabIndex < 0) return;

	BOOL bSelected		= (i32TabIndex == GetCurSel());
	COLORREF crSelected	= m_bColorSelected ? m_crSelected	: GetSysColor(COLOR_BTNTEXT);
	COLORREF crNormal	= m_bColorNormal   ? m_crNormal		: GetSysColor(COLOR_BTNTEXT);
	COLORREF crDisabled	= m_bColorDisabled ? m_crDisabled	: GetSysColor(COLOR_GRAYTEXT);

#ifdef _UNICODE
	TCHAR label[64]	= {NULL};
#else
	char label[64]	= {NULL};
#endif
	TC_ITEM item;
	item.mask = TCIF_TEXT|TCIF_IMAGE;
	item.pszText = label;     
	item.cchTextMax = 63;    	
	if (!GetItem(i32TabIndex, &item))
	return;

	CDC* pDC = CDC::FromHandle(drawitem->hDC);
	if (!pDC)	return;

	INT32 nSavedDC = pDC->SaveDC();
	CRect rectItem;
	POINT pt;

	GetItemRect(i32TabIndex, &rectItem);
	GetCursorPos(&pt);
	ScreenToClient(&pt);

	if (rectItem.PtInRect(pt))
		m_i32IdxMouseOver = i32TabIndex;

	pDC->SetBkMode(TRANSPARENT);
#if 0
	pDC->FillSolidRect(rect, ::GetSysColor(COLOR_BTNFACE));
#else
	pDC->FillSolidRect(rect, m_crTabBtnBack);
#endif
	//**  Draw the image
	CImageList* pImageList = GetImageList();
	if (pImageList && item.iImage >= 0) 
	{
		IMAGEINFO info;
		rect.left += pDC->GetTextExtent(_T(" ")).cx;
		pImageList->GetImageInfo(item.iImage, &info);
		CRect ImageRect(info.rcImage);
		INT32 nYpos = rect.top;
		pImageList->Draw(pDC, item.iImage, CPoint(rect.left, nYpos), ILD_TRANSPARENT);
		rect.left += ImageRect.Width();
	}

	if (!IsTabEnabled(i32TabIndex))
	{
		pDC->SetTextColor(crDisabled);
		rect.top -= ::GetSystemMetrics(SM_CYEDGE);
		pDC->DrawText(label, rect, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	}
	else
	{
		//** selected item -----
		if (bSelected)
			pDC->SetTextColor(crSelected);
		else //** other item ---
			{
			if (m_bColorMouseOver && i32TabIndex == m_i32IdxMouseOver) 
			{
				pDC->SetTextColor(m_crMouseOver);	
			}
			else
			{
				pDC->SetTextColor(crNormal);
			}
		}

		rect.top -= ::GetSystemMetrics(SM_CYEDGE);
		pDC->DrawText(label, rect, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	}

	pDC->RestoreDC(nSavedDC);
}

VOID CMyTabCtrl::OnMouseMove(UINT32 flags, CPoint point) 
{
	//** if we should change the color of the tab ctrl ---
	if (m_bColorMouseOver)
	{
		SetTimer(1,10,NULL);

		if (m_i32IdxMouseOver != -1)
		{
			CRect rectItem;
			GetItemRect(m_i32IdxMouseOver, rectItem);
			if (!rectItem.PtInRect(point))
			{
				CRect rectOldItem;
				GetItemRect(m_i32IdxMouseOver, rectOldItem);
				m_i32IdxMouseOver = -1;
				InvalidateRect(rectOldItem);
				return;
			}
		}	

		if (!m_bMouseOver)
		{
			TCHITTESTINFO hitTest;

			m_bMouseOver= TRUE;
			hitTest.pt	= point;

			INT32 iItem = HitTest(&hitTest);
			if (iItem != -1 && m_arrTab.GetAt(iItem))
			{
				RECT rectItem;
				GetItemRect(iItem, &rectItem);
				InvalidateRect(&rectItem);
			}
		}
	}
	
	CTabCtrl::OnMouseMove(flags, point);
}

VOID CMyTabCtrl::OnTimer(UINT_PTR event) 
{
	POINT pt;
	GetCursorPos(&pt);
	CRect rectItem, rectScreen;

	GetItemRect(m_i32IdxMouseOver, rectItem);
	rectScreen = rectItem;
	ClientToScreen(rectScreen);
	
	// If mouse leaves, show normal
	if (!rectScreen.PtInRect(pt)) 
	{
		KillTimer (1);
		m_bMouseOver		= FALSE;
		m_i32IdxMouseOver	= -1;
		InvalidateRect(rectItem);
	}

	CTabCtrl::OnTimer(event);
}

BOOL CMyTabCtrl::OnEraseBkgnd(CDC* dc) 
{
	BOOL ret = CTabCtrl::OnEraseBkgnd(dc);
 
	CRect bk_rect;
	GetClientRect(bk_rect);

    // 탭의 높이를 제외한 영역만큼 색상을 칠한다.
#if 0
	bk_rect.top += 20;
#else
	bk_rect.top += 0;
#endif
	dc->FillSolidRect(bk_rect, m_crTabWndBack);
 
    return ret;
}