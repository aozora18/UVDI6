#if !defined(AFX_MEMDC_H__CA1D3541_7235_11D1_ABBA_00A0243D1382__INCLUDED_)
#define AFX_MEMDC_H__CA1D3541_7235_11D1_ABBA_00A0243D1382__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MemDC.h : header file
//

//////////////////////////////////////////////////
// CProgressMemDC - memory DC
//
// Author: Keith Rule
// Email:  keithr@europa.com
// Copyright 1996-1997, Keith Rule
//
// You may freely use or modify this code provided this
// Copyright is included in all derived versions.
//
// History - 10/3/97 Fixed scrolling bug.
//					 Added print support.
//			 25 feb 98 - fixed minor assertion bug
//
// This class implements a memory Device Context

///////////////////////////////////////////////////////////////////////////////
// Modified by:  Yury Goltsman
// email:   ygprg@go.to
// page:    http://go.to/ygprg
//
// Modifications provided without warranty of any kind.
//
// version : 1.0
// Added optional parameter : rectangle of dc area
// Added preliminary copying image from orig. dc to bitmap
// (somewhy without this it doesn't work in some places)

class CProgressMemDC : public CDC
{
public:

	// constructor sets up the memory DC
	CProgressMemDC(CDC* pDC, LPRECT pRect = NULL, BOOL fCopyOld = FALSE) : CDC()
	{
		ASSERT(pDC != NULL);

		m_pDC = pDC;
		m_pOldBitmap = NULL;
		m_bMemDC = !pDC->IsPrinting();

		if (m_bMemDC) // Create a Memory DC
		{
			if(pRect)
				m_rect = *pRect;
			else
				pDC->GetClipBox(&m_rect);
			CreateCompatibleDC(pDC);
			m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
			m_pOldBitmap = SelectObject(&m_bitmap);
			SetWindowOrg(m_rect.left, m_rect.top);
			if(fCopyOld)
				BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
				       m_pDC, m_rect.left, m_rect.top, SRCCOPY);
		}
		else  // Make a copy of the relevent parts of the current DC for printing
		{
			m_bPrinting = pDC->m_bPrinting;
			m_hDC = pDC->m_hDC;
			m_hAttribDC = pDC->m_hAttribDC;
		}
	}
	
	// Destructor copies the contents of the mem DC to the original DC
	~CProgressMemDC()
	{
		if (m_bMemDC) 
		{	 
			// Copy the offscreen bitmap onto the screen.
			m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
			              this, m_rect.left, m_rect.top, SRCCOPY);

			//Swap back the original bitmap.
			SelectObject(m_pOldBitmap);
		} 
		else 
		{
			// All we need to do is replace the DC with an illegal value,
			// this keeps us from accidently deleting the handles associated with
			// the CDC that was passed to the constructor.
			m_hDC = m_hAttribDC = NULL;
		}
	}

	// Allow usage as a pointer
	CProgressMemDC* operator->() {return this;}
		
	// Allow usage as a pointer
	operator CProgressMemDC*() {return this;}

private:
	CBitmap  m_bitmap;      // Offscreen bitmap
	CBitmap* m_pOldBitmap;  // bitmap originally found in CProgressMemDC
	CDC*     m_pDC;         // Saves CDC passed in constructor
	CRect    m_rect;        // Rectangle of drawing area.
	BOOL     m_bMemDC;      // TRUE if CDC really is a Memory DC.
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MEMDC_H__CA1D3541_7235_11D1_ABBA_00A0243D1382__INCLUDED_)
