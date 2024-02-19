// GridCellCheck.cpp : implementation file
//
// MFC Grid Control - Main grid cell class
//
// Provides the implementation for a combobox cell type of the
// grid control.
//
// Written by Chris Maunder <cmaunder@mail.com>
// Copyright (c) 1998-2002. All Rights Reserved.
//
// Parts of the code contained in this file are based on the original
// CInPlaceList from http://www.codeguru.com/listview
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. 
//
// An email letting me know how you are using it would be nice as well. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// For use with CGridCtrl v2.22+
//
// History:
// 23 Jul 2001 - Complete rewrite
// 13 Mar 2004 - GetCellExtent and GetCheckPlacement fixed by Yogurt
//             - Read-only now honoured - Yogurt
//
// Sept 2004 - modified by PJ Arends
//               - placed the check box in center of the cell
//               - check box now responds like a normal check box to mouse
//                 clicks and space bar
//               - it is now possible to drag the check state between check
//                 boxes on grid
//               - use the m_lParam member to store check state. Can now use
//                 CGridCellBase::Get/SetData functions
//               - GetText returns "1" or "0" for check state. Can use
//                 CGridCtrl::Save to properly save check state
//          
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "GridCell.h"
#include "GridCtrl.h"
#include "GridCellCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////
// class CGridCellCheckMouseCapture

CGridCellCheckMouseCapture::CGridCellCheckMouseCapture(CGridCellCheck *pCell,
	CRect CheckRect)
{
	ASSERT(pCell);
	m_pCell = pCell;

	ASSERT(!CheckRect.IsRectEmpty());
	m_CheckRect = CheckRect;

	Create(NULL,
		_T("GridCellCheckMouseCaptureWindow"),
		WS_CHILD,
		CRect(0, 0, 0, 0),
		m_pCell->GetGrid(),
		0x1feb1965,
		NULL);
	SetCapture();
}

BEGIN_MESSAGE_MAP(CGridCellCheckMouseCapture, CWnd)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void CGridCellCheckMouseCapture::OnLButtonUp(UINT nFlags, CPoint)
{
	ReleaseCapture();
	CPoint pt;
	GetCursorPos(&pt);
	GetParent()->ScreenToClient(&pt);
	m_pCell->MouseCaptureDone(m_CheckRect.PtInRect(pt));
	GetParent()->PostMessage(WM_LBUTTONUP, nFlags, MAKELPARAM(pt.x, pt.y));
	DestroyWindow();
}

void CGridCellCheckMouseCapture::PostNcDestroy()
{
	delete this;
}

//////////////////////////////////////////////////////////////////
// class CGridCellCheck

IMPLEMENT_DYNCREATE(CGridCellCheck, CGridCell)

CGridCellCheck::CGridCellCheck() : CGridCell()
{
	m_lParam = FALSE;
	m_nRow = -1;
	m_nCol = -1;
	m_bLeftButtonDown = FALSE;
	m_bSpaceKeyDown = FALSE;
}

BOOL CGridCellCheck::GetTextRect(LPRECT pRect)
{
	BOOL bResult = CGridCell::GetTextRect(pRect);
	pRect->bottom = pRect->left = pRect->right = pRect->top = 0;
	return bResult;
}

BOOL CGridCellCheck::Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd /*=TRUE*/)
{
	// Store the cell's coordinates for later
	m_Rect = rect;
	m_nRow = nRow;
	m_nCol = nCol;

	// This code copied from CGridCellBase::OnDraw()
	// Eliminated the unneccesary text and image drawing parts

	// Note - all through this function we totally brutalise 'rect'. Do not
	// depend on it's value being that which was passed in.

	CGridCtrl* pGrid = GetGrid();
	ASSERT(pGrid);

	if (!pGrid || !pDC)
		return FALSE;

	if (rect.Width() <= 0 || rect.Height() <= 0)  // prevents imagelist item from drawing even
		return FALSE;                             //  though cell is hidden

	//TRACE3("Drawing %scell %d, %d\n", IsFixed()? _T("Fixed ") : _T(""), nRow, nCol);

	int nSavedDC = pDC->SaveDC();
	pDC->SetBkMode(TRANSPARENT);

	// Get the default cell implementation for this kind of cell. We use it if this cell
	// has anything marked as "default"
	CGridDefaultCell *pDefaultCell = (CGridDefaultCell*)GetDefaultCell();
	if (!pDefaultCell)
		return FALSE;

	// Set up text and background colours
	COLORREF TextClr, TextBkClr;

	TextClr = (GetTextClr() == CLR_DEFAULT) ? pDefaultCell->GetTextClr() : GetTextClr();
	if (GetBackClr() == CLR_DEFAULT)
		TextBkClr = pDefaultCell->GetBackClr();
	else
	{
		bEraseBkgnd = TRUE;
		TextBkClr = GetBackClr();
	}

	// Draw cell background and highlighting (if necessary)
	if (IsFocused() || IsDropHighlighted())
	{
		// Always draw even in list mode so that we can tell where the
		// cursor is at.  Use the highlight colors though.
		if (GetState() & GVIS_SELECTED)
		{
			TextBkClr = ::GetSysColor(COLOR_HIGHLIGHT);
			TextClr = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
			bEraseBkgnd = TRUE;
		}

		rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
		if (bEraseBkgnd)
		{
			TRY
			{
				CBrush brush(TextBkClr);
				pDC->FillRect(rect, &brush);
			}
				CATCH(CResourceException, e)
			{
				//e->ReportError();
			}
			END_CATCH
		}

		// Don't adjust frame rect if no grid lines so that the
		// whole cell is enclosed.
		if (pGrid->GetGridLines() != GVL_NONE)
		{
			rect.right--;
			rect.bottom--;
		}

		if (pGrid->GetFrameFocusCell())
		{
			// Use same color as text to outline the cell so that it shows
			// up if the background is black.
			TRY
			{
				CBrush brush(TextClr);
				pDC->FrameRect(rect, &brush);
			}
				CATCH(CResourceException, e)
			{
				//e->ReportError();
			}
			END_CATCH
		}
		pDC->SetTextColor(TextClr);

		// Adjust rect after frame draw if no grid lines
		if (pGrid->GetGridLines() == GVL_NONE)
		{
			rect.right--;
			rect.bottom--;
		}

		//rect.DeflateRect(0,1,1,1);  - Removed by Yogurt
	}
	else if ((GetState() & GVIS_SELECTED))
	{
		rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
		pDC->FillSolidRect(rect, ::GetSysColor(COLOR_HIGHLIGHT));
		rect.right--; rect.bottom--;
		pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
	}
	else
	{
		if (bEraseBkgnd)
		{
			rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
			CBrush brush(TextBkClr);
			pDC->FillRect(rect, &brush);
			rect.right--; rect.bottom--;
		}
		pDC->SetTextColor(TextClr);
	}

	// Draw lines only when wanted
	if (IsFixed() && pGrid->GetGridLines() != GVL_NONE)
	{
		CCellID FocusCell = pGrid->GetFocusCell();

		// As above, always show current location even in list mode so
		// that we know where the cursor is at.
		BOOL bHiliteFixed = pGrid->GetTrackFocusCell() && pGrid->IsValid(FocusCell) &&
			(FocusCell.row == nRow || FocusCell.col == nCol);

		// If this fixed cell is on the same row/col as the focus cell,
		// highlight it.
		if (bHiliteFixed)
		{
			rect.right++; rect.bottom++;
			pDC->DrawEdge(rect, BDR_SUNKENINNER /*EDGE_RAISED*/, BF_RECT);
			rect.DeflateRect(1, 1);
		}
		else
		{
			CPen lightpen(PS_SOLID, 1, ::GetSysColor(COLOR_3DHIGHLIGHT)),
				darkpen(PS_SOLID, 1, ::GetSysColor(COLOR_3DDKSHADOW)),
				*pOldPen = pDC->GetCurrentPen();

			pDC->SelectObject(&lightpen);
			pDC->MoveTo(rect.right, rect.top);
			pDC->LineTo(rect.left, rect.top);
			pDC->LineTo(rect.left, rect.bottom);

			pDC->SelectObject(&darkpen);
			pDC->MoveTo(rect.right, rect.top);
			pDC->LineTo(rect.right, rect.bottom);
			pDC->LineTo(rect.left, rect.bottom);

			pDC->SelectObject(pOldPen);
			rect.DeflateRect(1, 1);
		}
	}

	// End of copied code from CGridCellBase::OnDraw
	// We can know draw the check box

	// restore 'rect'
	rect = m_Rect;

	CRect CheckRect = GetCheckPlacement();
	rect.left = CheckRect.right;

	UINT nState = DFCS_BUTTONCHECK;
	if (GetCheck())
		nState |= DFCS_CHECKED;
	if (m_bLeftButtonDown || m_bSpaceKeyDown)
		nState |= DFCS_INACTIVE;

	// Do the draw 
	pDC->DrawFrameControl(GetCheckPlacement(), DFC_BUTTON, nState);

	pDC->RestoreDC(nSavedDC);
	return TRUE;
}

void CGridCellCheck::OnDblClick(CRect cellRect, CPoint PointCellRelative)
{
	if (!IsReadOnly())
	{
		PointCellRelative += m_Rect.TopLeft();
		if (GetCheckPlacement().PtInRect(PointCellRelative))
		{
			new CGridCellCheckMouseCapture(this, GetCheckPlacement());
			m_bLeftButtonDown = TRUE;
			GetGrid()->InvalidateRect(m_Rect);
			return;
		}
	}
	CGridCell::OnDblClick(cellRect, PointCellRelative);
}

void CGridCellCheck::OnClickDown(CRect cellRect, CPoint PointCellRelative)
{
	if (!IsReadOnly())
	{
		PointCellRelative += m_Rect.TopLeft();
		if (GetCheckPlacement().PtInRect(PointCellRelative))
		{
			new CGridCellCheckMouseCapture(this, GetCheckPlacement());
			m_bLeftButtonDown = TRUE;
			GetGrid()->InvalidateRect(m_Rect);
			return;
		}
	}

	CGridCell::OnClickDown(cellRect, PointCellRelative);

	return;
}

BOOL CGridCellCheck::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar != VK_SPACE)
		return CGridCell::OnKeyDown(nChar, nRepCnt, nFlags);

	if (!IsReadOnly())
	{
		if (!m_bSpaceKeyDown)
		{
			m_bSpaceKeyDown = TRUE;
			GetGrid()->InvalidateRect(m_Rect);
		}
	}

	return TRUE;
}

BOOL CGridCellCheck::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar != VK_SPACE)
	{
		//CGridCell::OnKeyUp(nChar, nRepCnt, nFlags);
		return TRUE;
	}
	if (m_bSpaceKeyDown)
		SetCheck(!GetCheck());
	m_bSpaceKeyDown = FALSE;

	return TRUE;
}

BOOL CGridCellCheck::OnChar(UINT, UINT, UINT)
{
	return TRUE;
}

BOOL CGridCellCheck::SetCheck(BOOL bChecked /*=TRUE*/)
{
	bChecked = bChecked ? TRUE : FALSE;
	BOOL bTemp = GetCheck();
	if (bTemp != bChecked)
	{
		m_nState |= GVIS_MODIFIED;
		m_lParam = bChecked;
		if (!m_Rect.IsRectEmpty())
			GetGrid()->InvalidateRect(m_Rect);
		if (m_nRow != -1 && m_nCol != -1)
			SendMessageToParent(m_nRow, m_nCol, NM_GRIDCELLCHECK);
	}

	return bTemp;
}

BOOL CGridCellCheck::GetCheck()
{
	return m_lParam ? TRUE : FALSE;
}

// Returns the dimensions and placement of the checkbox in GridCtrl client coords.
CRect CGridCellCheck::GetCheckPlacement()
{
	int nWidth = GetSystemMetrics(SM_CXHSCROLL);
	CRect place = m_Rect;
	place.left += (m_Rect.Width() - nWidth) / 2;
	place.right = place.left + nWidth;
	place.top += (m_Rect.Height() - nWidth) / 2;
	place.bottom = place.top + nWidth;

	if (m_Rect.Height() < nWidth + 2 * static_cast<int> (GetMargin()))
	{
		place.top = m_Rect.top + (m_Rect.Height() - nWidth) / 2;
		place.bottom = place.top + nWidth;
	}

	return place;
}

// called only from CGridCellCheckMouseCapture::OnLButtonUp
void CGridCellCheck::MouseCaptureDone(BOOL InCheck)
{
	m_bLeftButtonDown = FALSE;
	if (InCheck)
		SetCheck(!GetCheck());
	GetGrid()->InvalidateRect(m_Rect);
}

void CGridCellCheck::SetText(LPCTSTR szText)
{
	if (!szText || _tcscmp(szText, _T("0")) == 0)
		SetCheck(FALSE);
	else
		SetCheck(TRUE);
}

LPCTSTR CGridCellCheck::GetText() const
{
	return (m_lParam ? _T("1") : _T("0"));
}