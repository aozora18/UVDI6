#if !defined(AFX_GRIDCELLCHECK_H__ECD42822_16DF_11D1_992F_895E185F9C72__INCLUDED_)
#define AFX_GRIDCELLCHECK_H__ECD42822_16DF_11D1_992F_895E185F9C72__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// GridCellCheck.h : header file
//
// MFC Grid Control - Grid check box class header file
//
// Written by Chris Maunder <cmaunder@mail.com>
// Copyright (c) 1998-2002. All Rights Reserved.
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
// Modified Sept 2004 by PJ Arends
//
//////////////////////////////////////////////////////////////////////


#include "GridCell.h"

#define NM_GRIDCELLCHECK (NM_FIRST - 82)

class CGridCellCheck;

class CGridCellCheckMouseCapture : public CWnd
{
public:
	CGridCellCheckMouseCapture(CGridCellCheck *pCell, CRect CheckRect);
protected:
	virtual void PostNcDestroy();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
private:
	CGridCellCheck *m_pCell;
	CRect m_CheckRect;
};

class CGridCellCheck : public CGridCell
{
	friend class CGridCtrl;
	friend class CGridCellCheckMouseCapture;

	DECLARE_DYNCREATE(CGridCellCheck)

public:
	CGridCellCheck();

public:
	BOOL SetCheck(BOOL bChecked = TRUE);
	BOOL GetCheck();
	virtual void SetText(LPCTSTR szText);
	virtual LPCTSTR GetText() const;
	virtual void SetData(LPARAM lParam) { SetCheck((BOOL)lParam); }

	// Operations
	virtual BOOL GetTextRect(LPRECT pRect);

protected:
	void MouseCaptureDone(BOOL InCheck);
	CRect GetCheckPlacement();

	
	virtual void OnDblClick(CRect cellRect, CPoint PointCellRelative);
	virtual void OnClickDown(CRect cellRect, CPoint PointCellRelative);
	//	virtual void OnDblClick(CPoint PointCellRelative);
	//virtual BOOL OnClickDown(CPoint PointCellRelative);

	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	virtual BOOL Edit(int, int, CRect, CPoint, UINT, UINT) { return FALSE; }

	virtual BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);

protected:
	BOOL  m_bLeftButtonDown;
	BOOL  m_bSpaceKeyDown;
	CRect m_Rect;
	int m_nRow;
	int m_nCol;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDCELLCHECK_H__ECD42822_16DF_11D1_992F_895E185F9C72__INCLUDED_)
