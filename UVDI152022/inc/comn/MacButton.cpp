
#include "pch.h"
#include <math.h>
#include "MacButton.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

COLORREF GetColour(double dAngle, COLORREF crBright, COLORREF crDark)
{
#define Rad2Deg	180.0/3.1415 
#define LIGHT_SOURCE_ANGLE	-2.356		// -2.356 radians = -135 degrees, i.e. From top left

	ASSERT(dAngle > -3.1416 && dAngle < 3.1416);
	double dAngleDifference = LIGHT_SOURCE_ANGLE - dAngle;

	if (dAngleDifference < -3.1415) dAngleDifference = 6.293 + dAngleDifference;
	else if (dAngleDifference > 3.1415) dAngleDifference = 6.293 - dAngleDifference;

	double Weight = 0.5*(cos(dAngleDifference)+1.0);

	BYTE Red   = (BYTE) (Weight*GetRValue(crBright) + (1.0-Weight)*GetRValue(crDark));
	BYTE Green = (BYTE) (Weight*GetGValue(crBright) + (1.0-Weight)*GetGValue(crDark));
	BYTE Blue  = (BYTE) (Weight*GetBValue(crBright) + (1.0-Weight)*GetBValue(crDark));

	//TRACE("LightAngle = %0.0f, Angle = %3.0f, Diff = %3.0f, Weight = %0.2f, RGB %3d,%3d,%3d\n", 
	//	  LIGHT_SOURCE_ANGLE*Rad2Deg, dAngle*Rad2Deg, dAngleDifference*Rad2Deg, Weight,Red,Green,Blue);

	return RGB(Red, Green, Blue);
}

void DrawCircle(CDC* pDC, CPoint p, LONG lRadius, COLORREF crColour, BOOL bDashed=FALSE)
{
	const int nDashLength = 1;
	LONG lError, lXoffset, lYoffset;
	int  nDash = 0;
	BOOL bDashOn = TRUE;

	//Check to see that the coordinates are valid
	ASSERT( (p.x + lRadius <= LONG_MAX) && (p.y + lRadius <= LONG_MAX) );
	ASSERT( (p.x - lRadius >= LONG_MIN) && (p.y - lRadius >= LONG_MIN) );

	//Set starting values
	lXoffset = lRadius;
	lYoffset = 0;
	lError   = -lRadius;

	do {
		if (bDashOn) {
			pDC->SetPixelV(p.x + lXoffset, p.y + lYoffset, crColour);
			pDC->SetPixelV(p.x + lXoffset, p.y - lYoffset, crColour);
			pDC->SetPixelV(p.x + lYoffset, p.y + lXoffset, crColour);
			pDC->SetPixelV(p.x + lYoffset, p.y - lXoffset, crColour);
			pDC->SetPixelV(p.x - lYoffset, p.y + lXoffset, crColour);
			pDC->SetPixelV(p.x - lYoffset, p.y - lXoffset, crColour);
			pDC->SetPixelV(p.x - lXoffset, p.y + lYoffset, crColour);
			pDC->SetPixelV(p.x - lXoffset, p.y - lYoffset, crColour);
		}

		//Advance the error term and the constant X axis step
		lError += lYoffset++;

		//Check to see if error term has overflowed
		if ((lError += lYoffset) >= 0)
			lError -= --lXoffset * 2;

		if (bDashed && (++nDash == nDashLength)) {
			nDash = 0;
			bDashOn = !bDashOn;
		}

	} while (lYoffset <= lXoffset);	//Continue until halfway point
} 

void DrawCircle(CDC* pDC, CPoint p, LONG lRadius, COLORREF crBright, COLORREF crDark)
{
	LONG lError, lXoffset, lYoffset;

	//Check to see that the coordinates are valid
	ASSERT( (p.x + lRadius <= LONG_MAX) && (p.y + lRadius <= LONG_MAX) );
	ASSERT( (p.x - lRadius >= LONG_MIN) && (p.y - lRadius >= LONG_MIN) );

	//Set starting values
	lXoffset = lRadius;
	lYoffset = 0;
	lError   = -lRadius;

	do {
		const double Pi = 3.141592654, 
					 Pi_on_2 = Pi * 0.5,
					 Three_Pi_on_2 = Pi * 1.5;
		COLORREF crColour;
		double   dAngle = atan2(lYoffset, lXoffset);

		//Draw the current pixel, reflected across all eight arcs
		crColour = GetColour(dAngle, crBright, crDark);
		pDC->SetPixelV(p.x + lXoffset, p.y + lYoffset, crColour);

		crColour = GetColour(Pi_on_2 - dAngle, crBright, crDark);
		pDC->SetPixelV(p.x + lYoffset, p.y + lXoffset, crColour);

		crColour = GetColour(Pi_on_2 + dAngle, crBright, crDark);
		pDC->SetPixelV(p.x - lYoffset, p.y + lXoffset, crColour);

		crColour = GetColour(Pi - dAngle, crBright, crDark);
		pDC->SetPixelV(p.x - lXoffset, p.y + lYoffset, crColour);

		crColour = GetColour(-Pi + dAngle, crBright, crDark);
		pDC->SetPixelV(p.x - lXoffset, p.y - lYoffset, crColour);

		crColour = GetColour(-Pi_on_2 - dAngle, crBright, crDark);
		pDC->SetPixelV(p.x - lYoffset, p.y - lXoffset, crColour);

		crColour = GetColour(-Pi_on_2 + dAngle, crBright, crDark);
		pDC->SetPixelV(p.x + lYoffset, p.y - lXoffset, crColour);

		crColour = GetColour(-dAngle, crBright, crDark);
		pDC->SetPixelV(p.x + lXoffset, p.y - lYoffset, crColour);

		//Advance the error term and the constant X axis step
		lError += lYoffset++;

		//Check to see if error term has overflowed
		if ((lError += lYoffset) >= 0)
			lError -= --lXoffset * 2;

	} while (lYoffset <= lXoffset);	//Continue until halfway point
} 

/* --------------------------------------------------------------------------------------------- */
/* CMacButton                                                                                    */
/* --------------------------------------------------------------------------------------------- */
CMacButton::CMacButton()
{
	m_nType			= TYPE_STANDARD;
	m_nCheck		= 0;
	m_bMouseDown	= FALSE;
	m_bBold			= FALSE;
	m_hIconEnable	= NULL;
	m_hIconDisable	= NULL;
	m_hBitmap		= NULL;
	m_sizeImage		= CSize(0, 0);
	m_nImageEffect	= 0;
	m_iFontSize		= 0;
	m_nMultiLines	= 2;
	m_crFace		= RGB(0, 0, 0);
	GetColors();
	CreatePens();
}

CMacButton::~CMacButton()
{
	DeletePens();
	// 폰트 제거
	if (m_csFont.GetSafeHandle())	m_csFont.DeleteObject();
	if (m_hIconEnable)				DestroyIcon(m_hIconEnable);
	if (m_hIconDisable)				DestroyIcon(m_hIconDisable);
	if (m_hBitmap)					DeleteObject(m_hBitmap);
}

BEGIN_MESSAGE_MAP(CMacButton, CButton)
	ON_WM_SYSCOLORCHANGE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

VOID CMacButton::PreSubclassWindow() 
{
	CButton::PreSubclassWindow();
	ModifyStyle(0, BS_OWNERDRAW);
}

VOID CMacButton::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{
	DrawButton(lpDIS);
}

VOID CMacButton::DrawButton(LPDRAWITEMSTRUCT lpDIS)
{
	if (lpDIS->hDC == nullptr)
	{
		int debug = 0;
	}
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	CRect rectItem(lpDIS->rcItem);

	UINT32 nState = lpDIS->itemState;
	// by sysandj : Flat Button Style로 변경
	UINT32 nStyle = BS_FLAT | GetStyle();
	// by sysandj : Flat Button Style로 변경
	BOOL bPushLike = BOOL(nStyle & BS_PUSHLIKE);

	// Create a mem DC for drawing
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rectItem.Width(), rectItem.Height());
	CBitmap *pOldBmp = dcMem.SelectObject(&bmp);

	INT32 nSaveDC = dcMem.SaveDC();

	CBrush *pOldBrush = (CBrush *)dcMem.SelectStockObject(NULL_BRUSH);
	dcMem.FillSolidRect(rectItem, m_crFace);

	// If the button is standard or pushlike, draw it now.
	// Wait until after drawing the text/image and focus to draw
	//	the radio button or check box. This way, the text or image
	// will not cover the radio or check.
	if (m_nType == TYPE_STANDARD)	DrawStandardButton(&dcMem, rectItem, nStyle, nState);
	else if (m_nType == TYPE_CHECKBOX || m_nType == TYPE_RADIO)	bPushLike = FALSE;	// by sysandj : BS_FLAT 추가에 따른 버그 수정
	else if (bPushLike)				DrawPushLikeButton(&dcMem, rectItem, nStyle, nState);

	CRect rectText(rectItem);
	CRect rectImage(rectItem);
	CString sText;
	GetWindowText(sText);

	// Draw an image or the text.
	if ((m_hIconEnable || m_hIconDisable) || m_hBitmap)	DrawImage(&dcMem, rectImage, nStyle, nState);
	else												DrawButtonText(&dcMem, rectText, sText, nStyle, nState);

	// Draw the focus rect.
	if (nState & ODS_FOCUS)
	{
		if ((m_nType == TYPE_STANDARD) || bPushLike)
		{
			rectText = lpDIS->rcItem;
			rectText.DeflateRect(4, 4, 4, 4);
			dcMem.DrawFocusRect(rectText);
		}
		else if (m_hIconEnable || m_hIconDisable || m_hBitmap)
		{
			rectImage.InflateRect(1, 1, 1, 1);
			if (rectImage.top < rectItem.top)
				rectImage.top = rectItem.top;
			if (rectImage.bottom > rectItem.bottom)
				rectImage.bottom = rectItem.bottom;
			if (rectImage.left < rectItem.left)
				rectImage.left = rectItem.left;
			if (rectImage.right > rectItem.right)
				rectImage.right = rectItem.right;

			dcMem.DrawFocusRect(rectImage);
		}
		else
		{
			rectText.InflateRect(1, 1, 1, 2);
			if (rectText.top < rectItem.top)
				rectText.top = rectItem.top;
			if (rectText.bottom > rectItem.bottom)
				rectText.bottom = rectItem.bottom;
			if (rectText.left < rectItem.left)
				rectText.left = rectItem.left;
			if (rectText.right > rectItem.right)
				rectText.right = rectItem.right;

			dcMem.DrawFocusRect(rectText); 
		}
	}	// if the button has focus
	// Draw the check box or radio button now.
	if (!bPushLike)
	{
		// Determine the rect for the check mark.
		CRect rectCheck = GetCheckRect(rectItem, nStyle);

		if (m_nType == TYPE_CHECKBOX)	DrawCheckBox(&dcMem, rectCheck, nStyle, nState);
		else if (m_nType == TYPE_RADIO)	DrawRadioButton(&dcMem, rectCheck, nStyle, nState);
	}

	pDC->BitBlt(rectItem.left, rectItem.top, rectItem.Width(), rectItem.Height(), &dcMem, rectItem.left, rectItem.top, SRCCOPY);
	// Clean up.
	dcMem.SelectObject(pOldBrush);
	dcMem.SelectObject(pOldBmp);
	dcMem.RestoreDC(nSaveDC);
	dcMem.DeleteDC();
	bmp.DeleteObject();
}

VOID CMacButton::DrawStandardButton(CDC *pDC, const CRect &rect, UINT32 nStyle, UINT32 nState)
{
	// Draw a flat button.
	if (nStyle & BS_FLAT)
	{
		COLORREF crFill = ::GetSysColor(COLOR_WINDOW);
		CBrush brFill(nState & ODS_SELECTED ? ~crFill : crFill);
		CBrush *pOldBrush = (CBrush *)pDC->SelectObject(&brFill);
		CPen pen;
		
		if (nState & ODS_DISABLED)			pen.CreatePen(PS_SOLID, 1, m_crShadow);
		else if (m_nCheck || m_bMouseDown)	pen.CreatePen(PS_SOLID, 1, crFill);
		else								pen.CreatePen(PS_SOLID, 1, ~crFill);

		CPen *pOldPen = (CPen *)pDC->SelectObject(&pen);
		// by sysandj : Flat Button Style로 변경
		//pDC->RoundRect(rect, CPoint(6, 6));
		// by sysandj : Flat Button Style로 변경

		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
		pen.DeleteObject();
		return;
	}
	
	CBrush brFill(nState & ODS_SELECTED ? m_crLiteShadow : m_crFace);
	CBrush *pOldBrush	= (CBrush *)pDC->SelectObject(&brFill);
	CPen penFrame(PS_SOLID, 1, ::GetSysColor(COLOR_WINDOWFRAME));
	CPen *pOldPen = (CPen *)pDC->SelectObject(nState & ODS_DISABLED ? &m_penShadow : &penFrame);

	// Draw a 3D button.
	if (nState & ODS_DISABLED)
	{
		pDC->RoundRect(rect, CPoint(6, 6));
	}
	else if (nState & ODS_SELECTED)
	{
		DrawPressedPushButton(pDC, rect);
	}
	else
	{
		DrawUnpressedPushButton(pDC, rect);
	}

	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
}

VOID CMacButton::DrawCheckBox(CDC *pDC, CRect rect, UINT32 nStyle, UINT32 nState)
{
	BOOL bDisabled = nState & ODS_DISABLED;
	
	// Deflate the rect by two on the right (for the check mark's shadow).
	rect.DeflateRect(0, 0, 2, 0);

	CPen *pOldPen = pDC->GetCurrentPen();
	CBrush *pOldBrush = pDC->GetCurrentBrush();

	// Draw a flat checkbox.
	if (nStyle & BS_FLAT)
	{
		COLORREF crFrame = ::GetSysColor(COLOR_WINDOW);
		CPen penFrame(PS_SOLID, 1, ::GetSysColor(COLOR_WINDOWFRAME));
		CBrush brFill(crFrame);
		pDC->SelectObject(&penFrame);
		pDC->SelectObject(&brFill);

		pDC->Rectangle(rect);

		if (bDisabled)
		{
			for (INT32 i = rect.left + 1, j = rect.top + 1; i < rect.right; i += 2, j += 2)
			{
				pDC->SetPixel(i, rect.top, crFrame);
				pDC->SetPixel(i - 1, rect.bottom - 1, crFrame);
				pDC->SetPixel(rect.left, j, crFrame);
				pDC->SetPixel(rect.right - 1, j - 1, crFrame);
			}
		}

		rect.DeflateRect(1, 1, 1, 1);

		if (m_bMouseDown)	pDC->Rectangle(rect);

		// Inflate the rect by two on the right (for the check mark's shadow).
		rect.InflateRect(0, 0, 2, 0);

		INT32 nLeft = rect.left;
		INT32 nTop = rect.top;

		CPen penDkShadow(PS_SOLID, 1, m_crDkShadow);
		pDC->SelectObject(&penDkShadow);

		// Draw the check, cross, or tri-state mark.
		if (m_nCheck == 1)
		{
			if (m_nCheckStyle == CHECK_STYLE_CHECK)
			{
				if (!bDisabled)
				{
					pDC->MoveTo(nLeft + 1, nTop + 4);
					pDC->LineTo(nLeft + 4, nTop + 7);
					pDC->LineTo(nLeft + 12,nTop - 1);
				}
				pDC->MoveTo(nLeft + 2, nTop + 4);
				pDC->LineTo(nLeft + 4, nTop + 6);
				pDC->LineTo(nLeft + 11, nTop -1);
			}
			else if (m_nCheckStyle == CHECK_STYLE_CROSS)
			{
				pDC->MoveTo(nLeft, nTop);
				pDC->LineTo(nLeft + 10, nTop + 10);

				INT32 nAdjust = bDisabled ? 1 : 0;
				pDC->MoveTo(nLeft + nAdjust, nTop + 9 - nAdjust);
				pDC->LineTo(nLeft + 10 - nAdjust, nTop - 1 + nAdjust);
			}
		}
		else if (m_nCheck == 2)
		{
			pDC->SelectObject(&m_penDkShadow);
			pDC->MoveTo(nLeft + 2, nTop + 4);
			pDC->LineTo(nLeft + 8, nTop + 4);
			pDC->MoveTo(nLeft + 2, nTop + 5);
			pDC->LineTo(nLeft + 8, nTop + 5);
		}

	}
	// Else draw a 3D checkbox.
	else
	{
		CBrush brFrame(bDisabled ? m_crShadow : ::GetSysColor(COLOR_WINDOWFRAME));
		pDC->FrameRect(rect, &brFrame);
		rect.DeflateRect(1, 1);

		if (m_bMouseDown)
		{								
			CBrush brShadow(m_crLiteShadow);
			pDC->FillRect(rect, &brShadow);
			pDC->Draw3dRect(rect, m_crShadow, m_crLiteFace);
		}
		else
		{
			pDC->FillSolidRect(rect, m_crFace);

			if (!bDisabled)
				pDC->Draw3dRect(rect, m_crHilight, m_crShadow);
		}

		pDC->SetPixel(CPoint(rect.right - 1, rect.top), m_crFace);
		pDC->SetPixel(CPoint(rect.left, rect.bottom - 1), m_crFace);

		// Inflate the rect by two on the right (for the check mark's shadow).
		rect.InflateRect(0, 0, 2, 0);

		INT32 nLeft = rect.left;
		INT32 nTop = rect.top;

		// Draw the check, cross, or tri-state mark.
		if (m_nCheck == 1)
		{
			// Draw the check mark's shadow
			if (m_nCheckStyle == CHECK_STYLE_CHECK)
			{
				// Draw the check mark's shadow
				pDC->SelectObject(bDisabled ? &m_penLiteShadow : &m_penShadow);
				pDC->MoveTo(nLeft + 4, nTop + 8);
				pDC->LineTo(nLeft + 9, nTop + 3);
				pDC->LineTo(nLeft + 9, nTop + 5);

				pDC->SelectObject(&m_penLiteShadow);
				pDC->MoveTo(nLeft + 2, nTop + 6);
				pDC->LineTo(nLeft + 4, nTop + 8);
				pDC->MoveTo(nLeft + 5, nTop + 8);
				pDC->LineTo(nLeft + 9, nTop + 4);

				pDC->SetPixel(nLeft + 11, nTop + 1, bDisabled ? m_crLiteShadow : m_crShadow);
				pDC->SetPixel(nLeft + 12, nTop + 1, m_crLiteShadow);
				pDC->SetPixel(nLeft + 11, nTop + 2, m_crLiteShadow);

				// Draw the check mark.
				pDC->SelectObject(bDisabled ? &m_penShadow : &m_penDkShadow);
				pDC->MoveTo(nLeft + 1, nTop + 4);
				pDC->LineTo(nLeft + 4, nTop + 7);
				pDC->LineTo(nLeft + 12,nTop - 1);
				pDC->MoveTo(nLeft + 2, nTop + 4);
				pDC->LineTo(nLeft + 4, nTop + 6);
				pDC->LineTo(nLeft + 11, nTop -1);
			}
			else if (m_nCheckStyle == CHECK_STYLE_CROSS)
			{
				// Draw the check mark's shadow
				pDC->SelectObject(bDisabled ? &m_penLiteShadow : &m_penShadow);
				pDC->MoveTo(nLeft + 3, nTop + 7);
				pDC->LineTo(nLeft + 9, nTop + 1);
				pDC->SelectObject(&m_penLiteShadow);
				pDC->MoveTo(nLeft + 3, nTop + 8);
				pDC->LineTo(nLeft + 9, nTop + 2);
				pDC->SetPixel(nLeft + 8, nTop + 7, bDisabled ? m_crLiteShadow : m_crShadow);
				pDC->SetPixel(nLeft + 8, nTop + 8, m_crLiteShadow);
				
				// Draw the check mark.
				pDC->SelectObject(bDisabled ? &m_penShadow : &m_penDkShadow);
				pDC->MoveTo(nLeft + 2, nTop + 1);
				pDC->LineTo(nLeft + 8, nTop + 7);
				pDC->MoveTo(nLeft + 2, nTop + 2);
				pDC->LineTo(nLeft + 8, nTop + 8);
				
				pDC->MoveTo(nLeft + 2, nTop + 6);
				pDC->LineTo(nLeft + 8, nTop);
				pDC->MoveTo(nLeft + 2, nTop + 7);
				pDC->LineTo(nLeft + 8, nTop + 1);
			}
		}
		else if (m_nCheck == 2)
		{
			pDC->SelectObject(&m_penLiteShadow);
			pDC->MoveTo(nLeft + 3, nTop + 6);
			pDC->LineTo(nLeft + 9, nTop + 6);
			pDC->SetPixel(nLeft + 8, nTop + 5, m_crLiteShadow);

			pDC->SelectObject(bDisabled ? &m_penShadow : &m_penDkShadow);
			pDC->MoveTo(nLeft + 2, nTop + 4);
			pDC->LineTo(nLeft + 8, nTop + 4);
			pDC->MoveTo(nLeft + 2, nTop + 5);
			pDC->LineTo(nLeft + 8, nTop + 5);
		}
	}

	// Restore the pen and brush.
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}

VOID CMacButton::DrawRadioButton(CDC *pDC, CRect rect, UINT32 nStyle, UINT32 nState)
{
	// Give the user a visual indication that the button is pressed by darkening some colors.
	BOOL bDarkened = FALSE;
	if (m_bMouseDown && !(nStyle & BS_FLAT))
	{
		m_crFace		= DarkenColor(m_crFace, 40);
		m_crShadow		= DarkenColor(m_crShadow, 40);
		m_crHilight		= DarkenColor(m_crHilight, 40);
		m_crLiteShadow	= DarkenColor(m_crLiteShadow, 40);
		m_crLiteFace	= DarkenColor(m_crLiteFace, 40);
		CreatePens();
		bDarkened = TRUE;
	}

	BOOL bDisabled = nState & ODS_DISABLED;

	// Draw the outer round rect.
	CPen penFrame;
	CBrush br;
	if (nStyle & BS_FLAT)
	{
		penFrame.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_WINDOWFRAME));
		br.CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
	}
	else
	{
		penFrame.CreatePen(PS_SOLID, 1, bDisabled ? m_crShadow : ::GetSysColor(COLOR_WINDOWFRAME));
		br.CreateSolidBrush(m_crFace);
	}

	CPen *pOldPen = (CPen *)pDC->SelectObject(&penFrame);
	CBrush *pOldBrush = (CBrush *)pDC->SelectObject(&br);
	pDC->RoundRect(rect, CPoint(10, 10));
	if ((nStyle & BS_FLAT) && m_bMouseDown)
	{
		rect.DeflateRect(1, 1, 1, 1);
		pDC->RoundRect(rect, CPoint(8, 8));
		rect.InflateRect(1, 1, 1, 1);
	}
	pDC->SelectObject(pOldBrush);
	br.DeleteObject();
	penFrame.DeleteObject();

	// Draw some shadowing
	if (!(nStyle & BS_FLAT))
	{
		INT32 nLeft = rect.left;
		INT32 nTop = rect.top;
		INT32 nAdjust = (m_nCheck || m_bMouseDown ? -11 : 0);
		INT32 nSign = (m_nCheck || m_bMouseDown ? -1 : 1);

		if (!bDisabled)
		{
			pDC->SelectObject(&m_penDarkDkShadow);
			pDC->MoveTo(nLeft + 0, nTop + 4);
			pDC->LineTo(nLeft + 5, nTop - 1);
			pDC->MoveTo(nLeft + 7, nTop);
			pDC->LineTo(nLeft + 12, nTop + 5);
			pDC->MoveTo(nLeft + 11, nTop + 7);
			pDC->LineTo(nLeft + 6, nTop + 12);
			pDC->MoveTo(nLeft + 4, nTop + 11);
			pDC->LineTo(nLeft - 1, nTop + 6);
		}

		if (!bDisabled)
		{
			pDC->SelectObject(&m_penLiteFace);
			pDC->MoveTo(nLeft + (nSign * (nAdjust + 1)), nTop + (nSign * (nAdjust + 6)));
			pDC->LineTo(nLeft + (nSign * (nAdjust + 1)), nTop + (nSign * (nAdjust + 4)));
			pDC->LineTo(nLeft + (nSign * (nAdjust + 4)), nTop + (nSign * (nAdjust + 1)));
			pDC->LineTo(nLeft + (nSign * (nAdjust + 7)), nTop + (nSign * (nAdjust + 1)));
			pDC->MoveTo(nLeft + (nSign * (nAdjust + 2)), nTop + (nSign * (nAdjust + 8)));
  			pDC->LineTo(nLeft + (nSign * (nAdjust + 9)), nTop + (nSign * (nAdjust + 1)));
			pDC->MoveTo(nLeft + (nSign * (nAdjust + 3)), nTop + (nSign * (nAdjust + 8)));
  			pDC->LineTo(nLeft + (nSign * (nAdjust + 9)), nTop + (nSign * (nAdjust + 2)));
			pDC->MoveTo(nLeft + (nSign * (nAdjust + 4)), nTop + (nSign * (nAdjust + 8)));
  			pDC->LineTo(nLeft + (nSign * (nAdjust + 9)), nTop + (nSign * (nAdjust + 3)));

			pDC->SelectObject(&m_penHilight);
			pDC->MoveTo(nLeft + (nSign * (nAdjust + 2)), nTop + (nSign * (nAdjust + 7)));
			pDC->LineTo(nLeft + (nSign * (nAdjust + 2)), nTop + (nSign * (nAdjust + 4)));
			pDC->LineTo(nLeft + (nSign * (nAdjust + 6)), nTop + (nSign * (nAdjust + 4)));
			pDC->MoveTo(nLeft + (nSign * (nAdjust + 4)), nTop + (nSign * (nAdjust + 5)));
			pDC->LineTo(nLeft + (nSign * (nAdjust + 4)), nTop + (nSign * (nAdjust + 2)));
			pDC->LineTo(nLeft + (nSign * (nAdjust + 8)), nTop + (nSign * (nAdjust + 2)));

			pDC->SelectObject(&m_penLiteShadow);
			pDC->MoveTo(nLeft + (nSign * (nAdjust + 6)), nTop + (nSign * (nAdjust + 9))); 
			pDC->LineTo(nLeft + (nSign * (nAdjust + 10)), nTop + (nSign * (nAdjust + 5))); 
			pDC->MoveTo(nLeft + (nSign * (nAdjust + 7)), nTop + (nSign * (nAdjust + 9))); 
			pDC->LineTo(nLeft + (nSign * (nAdjust + 10)), nTop + (nSign * (nAdjust + 6))); 

			pDC->SelectObject(&m_penShadow);
			pDC->MoveTo(nLeft + (nSign * (nAdjust + 4)), nTop + (nSign * (nAdjust + 10)));
			pDC->LineTo(nLeft + (nSign * (nAdjust + 7)), nTop + (nSign * (nAdjust + 10))); 
			pDC->LineTo(nLeft + (nSign * (nAdjust + 10)), nTop + (nSign * (nAdjust + 7))); 
			pDC->LineTo(nLeft + (nSign * (nAdjust + 10)), nTop + (nSign * (nAdjust + 3))); 

			pDC->SetPixel(nLeft + (nSign * (nAdjust + 3)), nTop + (nSign * (nAdjust + 5)), m_crHilight);
			pDC->SetPixel(nLeft + (nSign * (nAdjust + 5)), nTop + (nSign * (nAdjust + 3)), m_crHilight);
			pDC->SetPixel(nLeft + (nSign * (nAdjust + 2)), nTop + (nSign * (nAdjust + 2)), m_crFace);
			pDC->SetPixel(nLeft + (nSign * (nAdjust + 3)), nTop + (nSign * (nAdjust + 3)), m_crLiteFace);
			pDC->SetPixel(nLeft + (nSign * (nAdjust + 3)), nTop + (nSign * (nAdjust + 6)), m_crLiteFace);
			pDC->SetPixel(nLeft + (nSign * (nAdjust + 6)), nTop + (nSign * (nAdjust + 3)), m_crLiteFace);
		}

		// Do a little "touch-up" top make the radios look better.
		COLORREF crTemp = bDisabled ? m_crLiteShadow : m_crShadow;
		pDC->SetPixel(nLeft, nTop + 3, crTemp);
		pDC->SetPixel(nLeft + 3, nTop, crTemp);
		pDC->SetPixel(nLeft + 8, nTop, crTemp);
		pDC->SetPixel(nLeft + 11, nTop + 3, crTemp);
		pDC->SetPixel(nLeft + 11, nTop + 8, crTemp);
		pDC->SetPixel(nLeft + 8, nTop + 11, crTemp);
		pDC->SetPixel(nLeft + 3, nTop + 11, crTemp);
		pDC->SetPixel(nLeft, nTop + 8, crTemp);
	}

	// Draw the check if necessary.
	if (m_nCheck)
	{
		COLORREF crTemp = (bDisabled && !(nStyle & BS_FLAT) ? m_crShadow : m_crDkShadow);
		CBrush brTemp(crTemp);
		CPen pen(PS_SOLID, 1, crTemp);
		CBrush *pOldTemp = pDC->SelectObject(&brTemp);
		pDC->SelectObject(&pen);
		rect.DeflateRect(3, 3, 3, 3);
		pDC->RoundRect(rect, CPoint(2, 2));
		pDC->SelectObject(pOldTemp);
	}

	// Lighten the colors if they were darkened.
	if (bDarkened)
	{
		m_crFace		= LightenColor(m_crFace, 40);
		m_crShadow		= LightenColor(m_crShadow, 40);
		m_crHilight		= LightenColor(m_crHilight, 40);
		m_crLiteShadow	= LightenColor(m_crLiteShadow, 40);
		m_crLiteFace	= LightenColor(m_crLiteFace, 40);
		CreatePens();
	}

	pDC->SelectObject(pOldPen);
}
	
VOID CMacButton::OnLButtonDblClk(UINT32 /*nFlags*/, CPoint point) 
{
	SendMessage(WM_LBUTTONDOWN, 0, MAKELPARAM(point.x, point.y));	
}

VOID CMacButton::OnSysColorChange() 
{
	CButton::OnSysColorChange();
	
	GetColors();
	CreatePens();
}

VOID CMacButton::GetColors()
{
	if (m_nType == TYPE_MORE)
	{
		m_crDkShadow	= ::GetSysColor(COLOR_WINDOWFRAME);
		m_crShadow		= ::GetSysColor(COLOR_3DSHADOW);
		m_crLiteFace	= LightenColor(m_crFace, 51);
		m_crLiteShadow	= DarkenColor(m_crFace, 51);
	}
	else
	{
		m_crHilight = ::GetSysColor(COLOR_3DHILIGHT);
		m_crLiteFace = ::GetSysColor(COLOR_3DLIGHT);
		if (0 == m_crFace)	m_crFace = ::GetSysColor(COLOR_3DFACE);
		m_crShadow = ::GetSysColor(COLOR_3DSHADOW);
		m_crDkShadow = ::GetSysColor(COLOR_3DDKSHADOW);

		// Create some intermediate colors. This is done by averaging two colors.
		BYTE byRedFace		= GetRValue(m_crFace);
		BYTE byGreenFace	= GetGValue(m_crFace);
		BYTE byBlueFace		= GetBValue(m_crFace);
		BYTE byRedShadow	= GetRValue(m_crShadow);
		BYTE byGreenShadow	= GetGValue(m_crShadow);
		BYTE byBlueShadow	= GetBValue(m_crShadow);
		BYTE byRedDkShadow	= GetRValue(m_crDkShadow);
		BYTE byGreenDkShadow= GetGValue(m_crDkShadow);
		BYTE byBlueDkShadow	= GetBValue(m_crDkShadow);

		m_crLiteShadow = RGB(byRedFace + ((byRedShadow - byRedFace) >> 1),
									byGreenFace + ((byGreenShadow - byGreenFace) >> 1),
									byBlueFace + ((byBlueShadow - byBlueFace) >> 1));

		m_crDarkDkShadow = RGB(byRedShadow + ((byRedDkShadow - byRedShadow) >> 1),
									byGreenShadow + ((byGreenDkShadow - byGreenShadow) >> 1),
									byBlueShadow + ((byBlueDkShadow - byBlueShadow) >> 1));
	}
}

COLORREF CMacButton::LightenColor(const COLORREF crColor, BYTE byIncreaseVal)
{
	BYTE byRed = GetRValue(crColor);
	BYTE byGreen = GetGValue(crColor);
	BYTE byBlue = GetBValue(crColor);

	byRed = (byRed + byIncreaseVal) <= 255 ? BYTE(byRed + byIncreaseVal) : 255;;
	byGreen = (byGreen + byIncreaseVal)	<= 255 ? BYTE(byGreen + byIncreaseVal) : 255;
	byBlue = (byBlue + byIncreaseVal) <= 255 ? BYTE(byBlue + byIncreaseVal) : 255;

	return RGB(byRed, byGreen, byBlue);
}

COLORREF CMacButton::DarkenColor(const COLORREF crColor, BYTE byReduceVal)
{
	BYTE byRed = GetRValue(crColor);
	BYTE byGreen = GetGValue(crColor);
	BYTE byBlue = GetBValue(crColor);

	byRed = byRed >= byReduceVal ? BYTE(byRed - byReduceVal) : 0;
	byGreen = byGreen >= byReduceVal ? BYTE(byGreen - byReduceVal) : 0;
	byBlue = (byBlue >= byReduceVal) ? BYTE(byBlue - byReduceVal) : 0;


	return RGB(byRed, byGreen, byBlue);
}

VOID CMacButton::CreatePens()
{
	DeletePens();

	m_penFace.CreatePen(PS_SOLID, 1, m_crFace);
	m_penLiteFace.CreatePen(PS_SOLID, 1, m_crLiteFace);
	m_penLiteShadow.CreatePen(PS_SOLID, 1, m_crLiteShadow);
	m_penShadow.CreatePen(PS_SOLID, 1, m_crShadow);
	m_penDkShadow.CreatePen(PS_SOLID, 1, m_crDkShadow);

	if (m_nType != TYPE_MORE)
	{
		m_penHilight.CreatePen(PS_SOLID, 1, m_crHilight);
		m_penDarkDkShadow.CreatePen(PS_SOLID, 1, m_crDarkDkShadow);
	}
}

VOID CMacButton::DeletePens()
{
	if (m_penHilight.m_hObject)
		m_penHilight.DeleteObject();
	if (m_penFace.m_hObject)
		m_penFace.DeleteObject();
	if (m_penLiteFace.m_hObject)
		m_penLiteFace.DeleteObject();
	if (m_penLiteShadow.m_hObject)
		m_penLiteShadow.DeleteObject();
	if (m_penShadow.m_hObject)
		m_penShadow.DeleteObject();
	if (m_penDkShadow.m_hObject)
		m_penDkShadow.DeleteObject();
	if (m_penDarkDkShadow.m_hObject)
		m_penDarkDkShadow.DeleteObject();
}

VOID CMacButton::DrawImage(CDC *pDC, CRect &rect, UINT32 nStyle, UINT32 nState)
{
	// Calculate the left and right sides of the rect.
	if ((m_nType == TYPE_STANDARD) || (nStyle & BS_PUSHLIKE))
	{
		if (nStyle & BS_LEFT)
			rect.left += 5;
		else if (nStyle & BS_RIGHT)
			rect.left = rect.right - m_sizeImage.cx - 5;
		else
			rect.left = (rect.Width() - m_sizeImage.cx) >> 1;
	}	// if the button is standard or push-like
	else
	{
		if (nStyle & BS_LEFT)
			rect.left = (nStyle & BS_LEFTTEXT ? 2 : CHECKBOX_HEIGHT + 5);
		else if (nStyle & BS_RIGHT)
			rect.left = rect.right - m_sizeImage.cx - (nStyle & BS_LEFTTEXT ? CHECKBOX_HEIGHT + 7 : 2);
		// Else use the default vertical alignment (left for radios & checkboxes, center for push buttons).
		else
		{
			if (m_nType == TYPE_CHECKBOX || m_nType == TYPE_RADIO)
				rect.left = (nStyle & BS_LEFTTEXT ? 2 : CHECKBOX_HEIGHT + 5);
			else
				rect.left = (rect.Width() - m_sizeImage.cx) >> 1;
		}
	}
	rect.right = rect.left + m_sizeImage.cx;

	// Calculate the top and bottom of the rect.
	if (nStyle & BS_TOP)
		rect.top += ((m_nType == TYPE_STANDARD) || (nStyle & BS_PUSHLIKE) ? 4 : 1);
	else if (nStyle & BS_BOTTOM)
		rect.top = rect.bottom - m_sizeImage.cy - ((m_nType == TYPE_STANDARD) || (nStyle & BS_PUSHLIKE) ? 4 : 1);
	else
		rect.top = (rect.Height() - m_sizeImage.cy) >> 1;
	rect.bottom = rect.top + m_sizeImage.cy;

	// Adjust the image's rectangle depending on the effect
	if (m_nImageEffect == IMAGE_EFFECT_RAISED || m_nImageEffect == IMAGE_EFFECT_SUNKEN)
	{
		if ((m_nType == TYPE_STANDARD) || (nStyle & BS_PUSHLIKE))
		{
			if (nStyle & BS_LEFT)
			{
				rect.left++;
				rect.right++;
			}
			else if (nStyle & BS_RIGHT)
			{
				rect.left--;
				rect.right--;
			}
		}
		
		if (nStyle & BS_TOP)
		{
			rect.top++;
			rect.bottom++;
		}
		else if (nStyle & BS_BOTTOM)
		{
			rect.top--;
			rect.bottom--;
		}
	}

	// Draw the image.
	if (m_hIconEnable || m_hIconDisable)
	{
#if 0
		pDC->DrawState(rect.TopLeft(), m_sizeImage, 
						m_hIcon, nState & ODS_DISABLED ? DSS_DISABLED : DSS_NORMAL, (HBRUSH)NULL);
#else
		if (ODS_DISABLED == (nState & ODS_DISABLED))
		{
			if (m_hIconDisable != GetIcon())	CButton::SetIcon(m_hIconDisable);	/* 일단, 해당 리소스를 가진 Icon 핸들 적용 */
			pDC->DrawState(rect.TopLeft(), m_sizeImage, m_hIconDisable, DSS_NORMAL, (HBRUSH)NULL);
		}
		else
		{
			if (m_hIconEnable != GetIcon())	CButton::SetIcon(m_hIconEnable);	/* 일단, 해당 리소스를 가진 Icon 핸들 적용 */
			pDC->DrawState(rect.TopLeft(), m_sizeImage, m_hIconEnable, DSS_NORMAL, (HBRUSH)NULL);
		}
#endif
	}
	else if (m_hBitmap)
		pDC->DrawState(rect.TopLeft(), m_sizeImage, 
						m_hBitmap, nState & ODS_DISABLED ? DSS_DISABLED : DSS_NORMAL, (HBRUSH)NULL);

	if (m_nImageEffect)
	{
		rect.InflateRect(1, 1, 1, 1);
		if (m_nImageEffect == IMAGE_EFFECT_RAISED)
			pDC->Draw3dRect(rect, m_crHilight, m_crShadow);
		if (m_nImageEffect == IMAGE_EFFECT_SUNKEN)
			pDC->Draw3dRect(rect, m_crShadow, m_crHilight);
	}
}

/*
 desc : Icon Resource ID 설정
 parm : enable	- [in]  버튼 활성화 될 때, 표현될 Icon Resource ID
		disable	- [in]  버튼 비활성화 될 때, 표현될 Icon Resource ID
 retn : None
*/
VOID CMacButton::SetIconID(UINT32 enable, UINT32 disable)
{
	// Get icon dimensions.
	ICONINFO iconInfo;
	::memset(&iconInfo, 0x00, sizeof(ICONINFO));

	if (m_hBitmap)		DeleteObject(m_hBitmap);
	if (enable < 1 && disable < 1)	return;
	if (enable)		m_hIconEnable	= LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(enable));
	if (disable)	m_hIconDisable	= LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(disable));

	if (!m_hIconEnable && !m_hIconDisable)	return;
	if (m_hIconEnable)	::GetIconInfo(m_hIconEnable, &iconInfo);
	else				::GetIconInfo(m_hIconDisable, &iconInfo);

	m_sizeImage.cx = iconInfo.xHotspot << 1;
	m_sizeImage.cy = iconInfo.yHotspot << 1;
	/* 반드시 객체 핸들 제거 해야 됨. 메모리 릭 발생 */
	::DeleteObject(iconInfo.hbmColor);
	::DeleteObject(iconInfo.hbmMask);

	RedrawWindow();
}

HBITMAP CMacButton::SetBitmap(HBITMAP hBitmap)
{
	m_hBitmap		= hBitmap;

	if (m_hIconEnable)	DestroyIcon(m_hIconEnable);
	if (m_hIconDisable)	DestroyIcon(m_hIconDisable);
	m_hIconEnable	= NULL;
	m_hIconDisable	= NULL;

	if (hBitmap)
	{
		// Get bitmap dimensions.
		CBitmap *pBmp = CBitmap::FromHandle(hBitmap);
		ASSERT(pBmp);
		BITMAP bmp;
		::memset(&bmp, 0x00, sizeof(BITMAP));
		pBmp->GetBitmap(&bmp);

		m_sizeImage.cx = bmp.bmWidth;
		m_sizeImage.cy = bmp.bmHeight;
	}

	RedrawWindow();

	return CButton::SetBitmap(hBitmap);
}

CRect CMacButton::GetCheckRect(const CRect &rect, UINT32 nStyle)
{
	CRect rectCheck(rect);

	// Calculate the left and right sides of the rect.
	if (nStyle & BS_LEFTTEXT)
	{
		rectCheck.left = rect.right - CHECKBOX_HEIGHT - (m_nType == TYPE_RADIO ? 0 : 2);
	}
	else
	{
		rectCheck.left += (m_nType == TYPE_RADIO ? 1 : 0);
		rectCheck.right = rect.left + CHECKBOX_HEIGHT + (m_nType == TYPE_RADIO ? 1 : 2);
	}

	// Calculate the top and bottom sides of the rect.
	if (nStyle & BS_TOP)
		rectCheck.top += 1;
	else if (nStyle & BS_BOTTOM)
		rectCheck.top = rect.bottom - CHECKBOX_HEIGHT - 3;
	else
		rectCheck.top = ((rect.Height() - CHECKBOX_HEIGHT) >> 1) - 1;
	rectCheck.bottom = rectCheck.top + CHECKBOX_HEIGHT;

	return rectCheck;
}

VOID CMacButton::DrawUnpressedPushButton(CDC *pDC, const CRect &rect)
{
	pDC->RoundRect(rect, CPoint(6, 6));

	// Save myself some typing.
	INT32 nLeft = rect.left;
	INT32 nTop = rect.top;
	INT32 nRight = rect.right;
	INT32 nBottom = rect.bottom;

	pDC->SelectObject(&m_penHilight);
	pDC->MoveTo(nLeft + 2, nBottom - 4);
	pDC->LineTo(nLeft + 2, nTop + 2);
	pDC->LineTo(nRight - 3, nTop + 2);
	pDC->SetPixel(nLeft + 3, nTop + 3, m_crHilight);

	pDC->SelectObject(&m_penLiteShadow);
	pDC->MoveTo(nLeft + 3, nBottom - 3);
	pDC->LineTo(nRight - 4, nBottom - 3);
	pDC->LineTo(nRight - 4, nBottom - 4);
	pDC->LineTo(nRight - 3, nBottom - 4);
	pDC->LineTo(nRight - 3, nTop + 2);

	pDC->SelectObject(&m_penShadow);
	pDC->MoveTo(nLeft + 3, nBottom - 2);
	pDC->LineTo(nRight - 3, nBottom - 2);
	pDC->LineTo(nRight - 3, nBottom - 3);
	pDC->LineTo(nRight - 2, nBottom - 3);
	pDC->LineTo(nRight - 2, nTop + 2);

	pDC->SetPixel(nLeft, nTop + 2, m_crDarkDkShadow);
	pDC->SetPixel(nLeft + 2, nTop, m_crDarkDkShadow);
	pDC->SetPixel(nRight - 3, nTop, m_crDarkDkShadow);
	pDC->SetPixel(nRight - 1, nTop + 2, m_crDarkDkShadow);
	pDC->SetPixel(nRight - 1, nBottom - 3, m_crDarkDkShadow);
	pDC->SetPixel(nRight - 3, nBottom - 1, m_crDarkDkShadow);
	pDC->SetPixel(nLeft + 2, nBottom - 1, m_crDarkDkShadow);
	pDC->SetPixel(nLeft, nBottom - 3, m_crDarkDkShadow);

	pDC->SetPixel(nLeft + 1, nTop + 2, m_crLiteShadow);
	pDC->SetPixel(nLeft + 2, nTop + 1, m_crLiteShadow);
	pDC->SetPixel(nRight - 3, nTop + 1, m_crLiteShadow);
	pDC->SetPixel(nRight - 2, nTop + 2, m_crLiteShadow);
	pDC->SetPixel(nLeft + 2, nBottom - 2, m_crLiteShadow);
	pDC->SetPixel(nLeft + 1, nBottom - 3, m_crLiteShadow);

	DrawCornerShadows(pDC, rect);
}

VOID CMacButton::DrawPressedPushButton(CDC *pDC, const CRect &rect)
{
	pDC->RoundRect(rect, CPoint(6, 6));

	// Save myself some typing.
	INT32 nLeft = rect.left;
	INT32 nTop = rect.top;
	INT32 nRight = rect.right;
	INT32 nBottom = rect.bottom;

	pDC->SelectObject((m_nType == TYPE_STANDARD) || m_bMouseDown ? &m_penDarkDkShadow : &m_penShadow);
	pDC->MoveTo(nLeft + 1, nBottom - 4);
	pDC->LineTo(nLeft + 1, nTop + 2);
	pDC->LineTo(nLeft + 2, nTop + 2);
	pDC->LineTo(nLeft + 2, nTop + 1);
	pDC->LineTo(nRight - 3, nTop + 1);

	pDC->SelectObject((m_nType == TYPE_STANDARD) || m_bMouseDown ? &m_penShadow : &m_penLiteShadow);
	pDC->MoveTo(nLeft + 2, nBottom - 4);
	pDC->LineTo(nLeft + 2, nTop + 3);
	pDC->LineTo(nLeft + 3, nTop + 3);
	pDC->LineTo(nLeft + 3, nTop + 2);
	pDC->LineTo(nRight - 3, nTop + 2);

	pDC->SelectObject((m_nType == TYPE_STANDARD) || !(m_nCheck && !m_bMouseDown) ? &m_penLiteFace : &m_penHilight);
	pDC->MoveTo(nLeft + 3, nBottom - 2);
	pDC->LineTo(nRight - 4, nBottom - 2);
	pDC->LineTo(nRight - 2, nBottom - 4);
	pDC->LineTo(nRight - 2, nTop + 2);

	pDC->SelectObject((m_nType == TYPE_STANDARD) || !(m_nCheck && !m_bMouseDown) ? &m_penFace : &m_penLiteFace);
	pDC->MoveTo(nLeft + 3, nBottom - 3);
	pDC->LineTo(nRight - 4, nBottom - 3);
	pDC->LineTo(nRight - 4, nBottom - 4);
	pDC->LineTo(nRight - 3, nBottom - 4);
	pDC->LineTo(nRight - 3, nTop + 2);

	pDC->SetPixel(nRight - 2, nTop + 2, ((m_nType == TYPE_STANDARD) || m_bMouseDown ? m_crFace : m_crLiteFace));
	pDC->SetPixel(nLeft + 2, nBottom - 2, ((m_nType == TYPE_STANDARD) || m_bMouseDown ? m_crFace : m_crLiteFace));
	pDC->SetPixel(nRight - 3, nTop + 1, m_crShadow);
	pDC->SetPixel(nLeft + 1, nBottom - 3, m_crShadow);

	DrawCornerShadows(pDC, rect);
}

VOID CMacButton::DrawCornerShadows(CDC *pDC, const CRect &rect)
{
	pDC->SetPixel(rect.left, rect.top + 2, m_crDarkDkShadow);
	pDC->SetPixel(rect.left + 2, rect.top, m_crDarkDkShadow);
	pDC->SetPixel(rect.right - 3, rect.top, m_crDarkDkShadow);
	pDC->SetPixel(rect.right - 1, rect.top + 2, m_crDarkDkShadow);
	pDC->SetPixel(rect.right - 1, rect.bottom - 3, m_crDarkDkShadow);
	pDC->SetPixel(rect.right - 3, rect.bottom - 1, m_crDarkDkShadow);
	pDC->SetPixel(rect.left + 2, rect.bottom - 1, m_crDarkDkShadow);
	pDC->SetPixel(rect.left, rect.bottom - 3, m_crDarkDkShadow);
}

VOID CMacButton::DrawPushLikeButton(CDC *pDC, const CRect &rect, UINT32 nStyle, UINT32 nState)
{
	CBrush brFill;

	HBITMAP hBmp;
	CBitmap *pBmp;
	CPen *pOldPen;

	// Draw a flat button.
	if (nStyle & BS_FLAT)
	{
		COLORREF crFill = ::GetSysColor(COLOR_WINDOW);

		hBmp = GetDitherBitmap(pDC, crFill, ~crFill);
		pBmp = (hBmp ? CBitmap::FromHandle(hBmp) : NULL);
		ASSERT(pBmp);

		// If the mouse is down, use a pattern brush to fill with. 
		// If the button is checked, use the inverted window color.
		if (m_bMouseDown)	brFill.CreatePatternBrush(pBmp);
		else if (m_nCheck)	brFill.CreateSolidBrush(~crFill);
		else				brFill.CreateSolidBrush(crFill);
		::DeleteObject(hBmp);

		CBrush *pOldBrush = (CBrush *)pDC->SelectObject(&brFill);
		CPen pen;
		if (nState & ODS_DISABLED)			pen.CreatePen(PS_SOLID, 1, m_crDarkDkShadow);
		else if (m_nCheck || m_bMouseDown)	pen.CreatePen(PS_SOLID, 1, crFill);
		else								pen.CreatePen(PS_SOLID, 1, ~crFill);

		pOldPen = (CPen *)pDC->SelectObject(&pen);
		pDC->RoundRect(rect, CPoint(6, 6));
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
		pen.DeleteObject();
		brFill.DeleteObject();
	}
	else
	{
		hBmp = GetDitherBitmap(pDC, m_crHilight, m_crFace);
		pBmp = (hBmp ? CBitmap::FromHandle(hBmp) : NULL);
		ASSERT(pBmp);

		// Create the brush to fill with.
		if (m_bMouseDown)	brFill.CreateSolidBrush(m_crLiteShadow);
		else if (m_nCheck)	brFill.CreatePatternBrush(pBmp);
		else				brFill.CreateSolidBrush(m_crFace);
		::DeleteObject(hBmp);

		CBrush *pOldBrush	= (CBrush *)pDC->SelectObject(&brFill);
		CPen penBorder(PS_SOLID, 1, nState & ODS_DISABLED ? m_crShadow : ::GetSysColor(COLOR_WINDOWFRAME));
		pOldPen = (CPen *)pDC->SelectObject(&penBorder);
		// Draw the pressed or unpressed button
		if (nState & ODS_DISABLED)			pDC->RoundRect(rect, CPoint(6, 6));
		else if (m_bMouseDown || m_nCheck)	DrawPressedPushButton(pDC, rect);
		else								DrawUnpressedPushButton(pDC, rect);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
		brFill.DeleteObject();
	}
}

HBITMAP CMacButton::GetDitherBitmap(CDC *pDC, COLORREF crColor1, COLORREF crColor2)
{
	struct  // BITMAPINFO with 16 colors
	{
		BITMAPINFOHEADER    bmiHeader;
		RGBQUAD             bmiColors[16];
	}	bmi;

	UINT32 patGray[8];

	bmi.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth        = 2;
	bmi.bmiHeader.biHeight       = 2;
	bmi.bmiHeader.biPlanes       = 1;
	bmi.bmiHeader.biBitCount     = 1;
	bmi.bmiHeader.biCompression  = BI_RGB;
	bmi.bmiHeader.biSizeImage    = 0;
	bmi.bmiHeader.biXPelsPerMeter= 0;
	bmi.bmiHeader.biYPelsPerMeter= 0;
	bmi.bmiHeader.biClrUsed      = 0;
	bmi.bmiHeader.biClrImportant = 0;

	bmi.bmiColors[0].rgbRed      = GetRValue(crColor1);
	bmi.bmiColors[0].rgbGreen    = GetGValue(crColor1);
	bmi.bmiColors[0].rgbBlue     = GetBValue(crColor1);
	bmi.bmiColors[0].rgbReserved = 0;

	bmi.bmiColors[1].rgbRed      = GetRValue(crColor2);
	bmi.bmiColors[1].rgbGreen    = GetGValue(crColor2);
	bmi.bmiColors[1].rgbBlue     = GetBValue(crColor2);
	bmi.bmiColors[1].rgbReserved = 0;

   // Create the byte array for CreateDIBitmap.
   patGray[6] = patGray[4] = patGray[2] = patGray[0] = 0x5555AAAAL;
   patGray[7] = patGray[5] = patGray[3] = patGray[1] = 0xAAAA5555L;

	return CreateDIBitmap(pDC->m_hDC, &bmi.bmiHeader, CBM_INIT, patGray, (LPBITMAPINFO)&bmi, DIB_RGB_COLORS);
}

VOID CMacButton::RedrawCheck()
{
	CRect rect;
	GetClientRect(rect);
	InvalidateRect(GetCheckRect(rect, GetStyle()));
}

/*
 desc : 버튼 폰트에 대해 강제로 적용
 parm : lf	- 로그 폰트
 retn : None
*/
VOID CMacButton::SetLogFont(LOGFONT lf)
{
	if (m_csFont.GetSafeHandle())
	{
		m_csFont.DeleteObject();
	}
	if (m_csFont.CreateFontIndirect(&lf))
	{
		SetFont(&m_csFont);
	}

#if 0
	m_iFontSize		= -11;

	wmemset(m_szFontName, 0x00, LF_FACESIZE);
	swprintf_s(m_szFontName, LF_FACESIZE, _T("맑은 고딕"));
#endif
}

VOID CMacButton::DrawButtonText(CDC *pDC, CRect &rect, const CString &sText, UINT32 nStyle, UINT32 nState)
{
#if 0
	LOGFONT lf;
	CFont *pFont, *pOldFont, fontNorm, fontBold;
	CSize sizeText;

	// 폰트 적용
	pFont = GetFont();
	if (!pFont)	return;
	pFont->GetLogFont(&lf);
	wcscpy_s(lf.lfFaceName, _tcslen(m_szFontName)+1, m_szFontName);
	lf.lfCharSet= HANGUL_CHARSET;
	lf.lfHeight	= m_iFontSize;
	fontNorm.CreateFontIndirect(&lf);
	pOldFont	= (CFont *)pDC->SelectObject(&fontNorm);

	// 진하게 표시한 경우
	if (m_bBold)
	{
		pFont->GetLogFont(&lf);
		wcscpy_s(lf.lfFaceName, _tcslen(m_szFontName)+1, m_szFontName);
		lf.lfWeight	= FW_BOLD;
		fontBold.CreateFontIndirect(&lf);
		pDC->SelectObject(&fontBold);
	}
#else
	LOGFONT lf;
	CFont *pFont, *pOldFont, fontNorm, fontBold;
	CSize sizeText;

	// 폰트 적용
	pFont = GetFont();
	if (!pFont)	return;
	pFont->GetLogFont(&lf);
	fontNorm.CreateFontIndirect(&lf);
	pOldFont	= (CFont *)pDC->SelectObject(&fontNorm);

	// 진하게 표시한 경우
	if (m_bBold)
	{
		lf.lfWeight	= FW_BOLD;
		fontBold.CreateFontIndirect(&lf);
		pDC->SelectObject(&fontBold);
	}
#endif
	sizeText = pDC->GetTextExtent(sText);
	// Determine the rect for the text.
	if ((m_nType == TYPE_STANDARD) || (nStyle & BS_PUSHLIKE))
	{
		if (nStyle & BS_LEFT)		rect.left += 5;
		else if (nStyle & BS_RIGHT)	rect.left = rect.right - sizeText.cx - 5;
		else						rect.left = (rect.Width() - sizeText.cx) >> 1;
		rect.right = rect.left + sizeText.cx;

		if (nStyle & BS_TOP)			rect.top += (m_nType == TYPE_RADIO? 4 : 5);
		else if (nStyle & BS_BOTTOM)	rect.top = rect.bottom - sizeText.cy - 5;
		else
		{
			if (nStyle & BS_MULTILINE)	rect.top = rect.Height() / m_nMultiLines - sizeText.cy;
			else						rect.top = (rect.Height() - sizeText.cy) >> 1;
		}
		if (nStyle & BS_MULTILINE)		rect.bottom = rect.top + sizeText.cy * m_nMultiLines;
		else							rect.bottom = rect.top + sizeText.cy;
	}
	else
	{
		if (nStyle & BS_LEFT)		rect.left = (nStyle & BS_LEFTTEXT ? 2 : CHECKBOX_HEIGHT + 5);
		else if (nStyle & BS_RIGHT)	rect.left = rect.right - sizeText.cx - (nStyle & BS_LEFTTEXT ? CHECKBOX_HEIGHT + 10 : 5);
		else
		{
			if (m_nType == TYPE_CHECKBOX || m_nType == TYPE_RADIO)	rect.left = (nStyle & BS_LEFTTEXT ? 2 : CHECKBOX_HEIGHT + 5);
			else													rect.left = (rect.Width() - sizeText.cx) >> 1;
		}
		rect.right = rect.left + sizeText.cx + (nStyle & BS_LEFTTEXT ? 2 : 0);

		if (nStyle & BS_TOP)			rect.top = rect.top;
		else if (nStyle & BS_BOTTOM)	rect.top = rect.bottom - sizeText.cy - 3;
		else
		{
			if (nStyle & BS_MULTILINE)	rect.top = rect.Height() / m_nMultiLines - sizeText.cy;
			else						rect.top = (rect.Height() - sizeText.cy) >> 1;
		}
		if (nStyle & BS_MULTILINE)		rect.bottom = rect.top + sizeText.cy * m_nMultiLines;
		else							rect.bottom = rect.top + sizeText.cy;
	}

	if (((m_nType == TYPE_STANDARD) && (nState & ODS_SELECTED)) || 
		 (nStyle & BS_PUSHLIKE) && (m_bMouseDown || ((nStyle & BS_FLAT) && m_nCheck)))
		pDC->SetTextColor(m_crHilight);
	if ((m_nType == TYPE_CHECKBOX) && (nStyle & BS_PUSHLIKE) && (m_nCheck == 2))
	{
		pDC->SetTextColor(m_crShadow);
	}

	// Draw the text.
	pDC->SetBkMode(TRANSPARENT);
	if (nState & ODS_DISABLED)
	{
		pDC->SetTextColor(m_crShadow);
		pDC->DrawText(sText, rect, DT_CENTER|DT_WORDBREAK);
	}
	else
	{
		pDC->SetTextColor(m_crText);
		pDC->DrawText(sText, rect, DT_CENTER|DT_WORDBREAK);
	}

	// Restore the original font.
	pDC->SelectObject(pOldFont);
	if (fontNorm.GetSafeHandle())				fontNorm.DeleteObject();
	if (m_bBold && fontBold.GetSafeHandle())	fontBold.DeleteObject();
}

VOID CMacButton::SetImageEffect(INT32 nEffect)
{
	m_nImageEffect = nEffect;	
	RedrawWindow();
}

VOID CMacButton::SetCheck(INT32 nCheck)
{
	if (m_nType == TYPE_STANDARD)
		return;

	INT32 nOldCheck = m_nCheck;

	m_nCheck = nCheck;

	if (m_nCheck != nOldCheck)
	{
		if (GetStyle() & BS_PUSHLIKE)	RedrawWindow();
		else							RedrawCheck();
	}
}

INT32 CMacButton::GetCheck() const
{
	return m_nCheck;
}

VOID CMacButton::SetBold(BOOL bBold /*= TRUE*/)
{
	m_bBold = bBold;
	RedrawWindow();
}

BOOL CMacButton::GetBold() const
{
	return m_bBold;
}


/* --------------------------------------------------------------------------------------------- */
/* CMacCheckBox                                                                                  */
/* --------------------------------------------------------------------------------------------- */
CMacCheckBox::CMacCheckBox()
	: CMacButton()
{
	m_bReadOnly		= FALSE;
	m_nType			= TYPE_CHECKBOX;
	m_nCheckStyle	= CHECK_STYLE_CROSS;
}

CMacCheckBox::~CMacCheckBox()
{
}

BEGIN_MESSAGE_MAP(CMacCheckBox, CMacButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

VOID CMacCheckBox::DrawItem(LPDRAWITEMSTRUCT lpdis) 
{
	DrawButton(lpdis);
}

VOID CMacCheckBox::OnLButtonUp(UINT32 /*nFlags*/, CPoint point) 
{
	if (m_bReadOnly)	return;
	ReleaseCapture();

	UINT32 nStyle = GetStyle();

	if (!(nStyle & BS_AUTOCHECKBOX))	return;

	CRect rect;
	GetClientRect(rect);

	if (rect.PtInRect(point))
	{
		m_nCheck = (m_nCheck == 0 ? 1 : 0);

		// Send notify message to parent window.
		// Added by Eric Hwang.
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)GetSafeHwnd());
	}

	m_bMouseDown = FALSE;

	if (nStyle & BS_PUSHLIKE)	RedrawWindow();
	else						RedrawCheck();
}

VOID CMacCheckBox::OnLButtonDown(UINT32 /*nFlags*/, CPoint /*point*/) 
{
	if (m_bReadOnly)	return;
	m_bMouseDown = TRUE;
	SetFocus();
	SetCapture();

	if (GetStyle() & BS_PUSHLIKE)	RedrawWindow();
	else							RedrawCheck();
}

VOID CMacCheckBox::OnLButtonDblClk(UINT32 /*nFlags*/, CPoint point) 
{
	if (m_bReadOnly)	return;
	SendMessage(WM_LBUTTONDOWN, 0, MAKELPARAM(point.x, point.y));	
}

VOID CMacCheckBox::OnMouseMove(UINT32 /*nFlags*/, CPoint point) 
{
	if (GetCapture() != this)	return;

	BOOL bWasMouseDown = m_bMouseDown;

	CRect rect;
	GetClientRect(rect);
	m_bMouseDown = rect.PtInRect(point);

	if (bWasMouseDown != m_bMouseDown)
	{
		if (GetStyle() & BS_PUSHLIKE)	RedrawWindow();
		else							RedrawCheck();
	}
}

VOID CMacCheckBox::OnKeyUp(UINT32 nChar, UINT32 /*nRepCnt*/, UINT32 /*nFlags*/) 
{		
	if (m_bReadOnly)	return;
	if (nChar == VK_SPACE)
	{
		m_bMouseDown = FALSE;
		m_nCheck = (m_nCheck == 0 ? 1 : 0);

		// Send notify message to parent window.
		// Added by Eric Hwang.
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)GetSafeHwnd());

		if (GetStyle() & BS_PUSHLIKE)
			RedrawWindow();
		else
			RedrawCheck();
	}
}

VOID CMacCheckBox::OnKeyDown(UINT32 nChar, UINT32 /*nRepCnt*/, UINT32 /*nFlags*/) 
{
	if (m_bReadOnly)	return;
	if (nChar == VK_SPACE)
	{
		if (!m_bMouseDown)
		{
			m_bMouseDown = TRUE;
			if (GetStyle() & BS_PUSHLIKE)
				RedrawWindow();
			else
				RedrawCheck();
		}
	}
}

VOID CMacCheckBox::SetCheckStyle(INT32 nStyle /* = CHECK_STYLE_CHECK*/)
{
	m_nCheckStyle = nStyle;
	RedrawCheck();
}

INT32 CMacCheckBox::GetCheckStyle() const
{
	return m_nCheckStyle;
}


/* --------------------------------------------------------------------------------------------- */
/* CMacRadioButton                                                                               */
/* --------------------------------------------------------------------------------------------- */
CMacRadioButton::CMacRadioButton()
	: CMacButton()
{
	m_nType = TYPE_RADIO;
}

CMacRadioButton::~CMacRadioButton()
{
}


BEGIN_MESSAGE_MAP(CMacRadioButton, CMacButton)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

VOID CMacRadioButton::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{
	DrawButton(lpDIS);
}

BOOL CMacRadioButton::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_DOWN || pMsg->wParam == VK_UP ||
			 pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT)
		{
			UncheckOtherRadios();

			// Get the previous/next button and check it.
			CWnd *pParent = GetParent();
			ASSERT(pParent);

			CWnd *pWnd = pParent->GetNextDlgGroupItem((CWnd *)this, (pMsg->wParam == VK_LEFT || pMsg->wParam == VK_UP ? TRUE : FALSE));
			if (pWnd && (((CButton*)pWnd)->GetButtonStyle() & BS_AUTORADIOBUTTON) && ((CMacRadioButton *)pWnd)->m_nType == TYPE_RADIO)
			{
				((CMacRadioButton *)pWnd)->SetCheck(1);

				// Send notify message to parent window.
				// Added by Eric Hwang.
				pParent->SendMessage(WM_COMMAND, MAKEWPARAM(pWnd->GetDlgCtrlID(), BN_CLICKED), (LPARAM)pWnd->GetSafeHwnd());
			}
			SetCheck(0);
			return FALSE;
		}
	}
	return CMacButton::PreTranslateMessage(pMsg);
}

VOID CMacRadioButton::OnLButtonUp(UINT32 /*nFlags*/, CPoint point) 
{
	ReleaseCapture();

	UINT32 nStyle = GetStyle();

	CRect rect;
	GetClientRect(rect);
	if (rect.PtInRect(point))
	{
		if (!m_nCheck)
			m_nCheck = 1;

		// Uncheck all other radios in the group
		if ((nStyle & BS_AUTORADIOBUTTON) && m_nCheck)
		{
			UncheckOtherRadios();
		}

		// Send notify message to parent window.
		// Added by Eric Hwang
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)GetSafeHwnd());
	}

	m_bMouseDown = FALSE;

	if (nStyle & BS_PUSHLIKE)
		RedrawWindow();
	else
		RedrawCheck();

//	CMacButton::OnLButtonUp(nFlags, point);
}

VOID CMacRadioButton::OnLButtonDown(UINT32 /*nFlags*/, CPoint /*point*/) 
{
	m_bMouseDown = TRUE;
	SetFocus();
	SetCapture();

	if (GetStyle() & BS_PUSHLIKE)
		RedrawWindow();
	else
		RedrawCheck();
//	CMacButton::OnLButtonDown(nFlags, point);
}

VOID CMacRadioButton::OnMouseMove(UINT32 nFlags, CPoint point) 
{
	if (GetCapture() != this)
		return;

	BOOL bWasMouseDown = m_bMouseDown;

	CRect rect;
	GetClientRect(rect);
	m_bMouseDown = rect.PtInRect(point);

	if (bWasMouseDown != m_bMouseDown)
	{
		if (GetStyle() & BS_PUSHLIKE)
			RedrawWindow();
		else
			RedrawCheck();
	}

	CMacButton::OnMouseMove(nFlags, point);
}

VOID CMacRadioButton::OnLButtonDblClk(UINT32 /*nFlags*/, CPoint point) 
{
	SendMessage(WM_LBUTTONDOWN, 0, MAKELPARAM(point.x, point.y));	
}

VOID CMacRadioButton::OnKeyUp(UINT32 nChar, UINT32 /*nRepCnt*/, UINT32 /*nFlags*/) 
{
	if (nChar == VK_SPACE)
	{
		m_nCheck = 1;

		// Send notify message to parent window.
		// Added by Eric Hwang.
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)GetSafeHwnd());

		UncheckOtherRadios();
		m_bMouseDown = FALSE;
		if (GetStyle() & BS_PUSHLIKE)
			RedrawWindow();
		else
			RedrawCheck();
	}
}

VOID CMacRadioButton::OnKeyDown(UINT32 nChar, UINT32 /*nRepCnt*/, UINT32 /*nFlags*/) 
{
	if (nChar == VK_SPACE)
	{
		if (!m_bMouseDown)
		{
			m_bMouseDown = TRUE;
			if (GetStyle() & BS_PUSHLIKE)
				RedrawWindow();
			else
				RedrawCheck();
		}
	}
}

VOID CMacRadioButton::OnSysColorChange() 
{
	CMacButton::OnSysColorChange();
	
	GetColors();
	CreatePens();
}

VOID CMacRadioButton::UncheckOtherRadios()
{
	// Go backwards (GetNextDlgItem will wrap around to the end of the group).
	CWnd *pParent = GetParent();
	ASSERT(pParent);

	CWnd *pWnd = pParent->GetNextDlgGroupItem((CWnd *)this);
	while (pWnd && (pWnd != this))
	{
		if ((((CButton*)pWnd)->GetButtonStyle() & BS_AUTORADIOBUTTON) && 
			 ((CMacRadioButton *)pWnd)->m_nType == TYPE_RADIO)
		{
			((CMacRadioButton *)pWnd)->SetCheck(0);
		}
		pWnd = pParent->GetNextDlgGroupItem(pWnd);
	}
}

VOID CMacRadioButton::OnSetFocus(CWnd* pOldWnd) 
{
	CMacButton::OnSetFocus(pOldWnd);
	
	if (m_nCheck || m_bMouseDown)
		return;

	// If another button in this group is checked, set the 
	// focus to that button; if not, set the focus to this button.

	// Get the previous/next button and check it.
	CWnd *pParent = GetParent();
	ASSERT(pParent);

	CWnd *pWnd = pParent->GetNextDlgGroupItem((CWnd *)this);
	while (pWnd && (pWnd != this) && (((CButton*)pWnd)->GetButtonStyle() & BS_AUTORADIOBUTTON) && ((CMacRadioButton *)pWnd)->m_nType == TYPE_RADIO)
	{
		if (((CMacRadioButton*)pWnd)->GetCheck())
		{
			pWnd->SetFocus();
			break;
		}

		pWnd = pParent->GetNextDlgGroupItem(pWnd);
	}
}


/* --------------------------------------------------------------------------------------------- */
/* CMacMoreButton                                                                                */
/* --------------------------------------------------------------------------------------------- */
CMacMoreButton::CMacMoreButton()
{
	m_nType = TYPE_MORE;
	m_nAlignText = ALIGN_TEXT_LEFT;
	m_bMouseDown = FALSE;
	m_bDoEffect = FALSE;
	m_bMoreShown = FALSE;
	SetColor(::GetSysColor(COLOR_3DFACE));
}

CMacMoreButton::~CMacMoreButton()
{
}

BEGIN_MESSAGE_MAP(CMacMoreButton, CButton)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

VOID CMacMoreButton::PreSubclassWindow()
{
	CMacButton::PreSubclassWindow();

	// Add length to the right of the button to accommodate the arrow.
	CRect rect;
	GetClientRect(rect);
	rect.right += 12;

	// Convert the button's client coordinates to the parent's client coordinates.
	ClientToScreen(rect);
	CWnd* pParent = GetParent();
	if (pParent)						
		pParent->ScreenToClient(rect);
	
	// Resize the window.
	MoveWindow(rect, TRUE);
}

VOID CMacMoreButton::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	CRect rectItem(lpDIS->rcItem);

	// Create a mem DC for drawing
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rectItem.Width(), rectItem.Height());
	CBitmap *pOldBmp = dcMem.SelectObject(&bmp);

	INT32 nSaveDC = dcMem.SaveDC();

	dcMem.SelectStockObject(NULL_BRUSH);
	dcMem.FillSolidRect(rectItem, ::GetSysColor(COLOR_3DFACE));

	// Draw the text, if any.
	CString sText;
	GetWindowText(sText);
	if (!sText.IsEmpty())
	{
		CFont *pOldFont = (CFont *)dcMem.SelectObject(GetFont());
		CSize sizeText = dcMem.GetTextExtent(sText);
		dcMem.SetBkMode(TRANSPARENT);

		CRect rectText(rectItem);
		rectText.top = (rectText.Height() - sizeText.cy) >> 1;

		dcMem.SetTextColor(lpDIS->itemState & ODS_DISABLED ? m_crShadow : m_crDkShadow);

		if (m_nAlignText == ALIGN_TEXT_RIGHT)
			rectText.left += 16;

		dcMem.DrawText(sText, rectText, DT_LEFT | DT_VCENTER);

		dcMem.SelectObject(pOldFont);
	}

	INT32 nRight = (sText.IsEmpty() || (m_nAlignText == ALIGN_TEXT_RIGHT) ? 
					rectItem.left + 10 : rectItem.right - 2);
	INT32 nTop = (rectItem.Height() - 11) >> 1;

	if (lpDIS->itemState & ODS_DISABLED)
	{
		// Select the frame pen, since we always draw with that first.
		CPen penShadow(PS_SOLID, 1, m_crShadow);
		CPen *pOldPen = (CPen *)dcMem.SelectObject(&penShadow);
		
		if (m_bMoreShown)
		{
			for (INT32 i = 0; i < 6; i++)
			{
				dcMem.MoveTo(nRight - i, nTop + 3 + i);
				dcMem.LineTo((nRight - 11) + i, nTop + 3 + i);
			}
		}
		else
		{
			for (INT32 i = 0; i < 6; i++)
			{
				dcMem.MoveTo((nRight - 7) + i, nTop + i);
				dcMem.LineTo((nRight - 7) + i, nTop + 11 - i);
			}
		}

		rectItem = lpDIS->rcItem;
		pDC->BitBlt(rectItem.left, rectItem.top, rectItem.Width(), rectItem.Height(), &dcMem, rectItem.left, rectItem.top, SRCCOPY);

		dcMem.SelectObject(pOldPen);
		dcMem.SelectObject(pOldBmp);
		dcMem.RestoreDC(nSaveDC);
		dcMem.DeleteDC();
		bmp.DeleteObject();

		return;
	}	// if disabled


	// Give the effect of the button "turning" by drawing an intermediate step.
	if (m_bDoEffect)
	{
		// Select the frame pen, since we always draw with that first.
		CPen *pOldPen = (CPen *)dcMem.SelectObject(&m_penDkShadow);

		m_bDoEffect = FALSE;

		dcMem.MoveTo(nRight - 10, nTop + 7);
		dcMem.LineTo(nRight - 3, nTop);
		dcMem.LineTo(nRight - 3, nTop + 7);
		dcMem.LineTo(nRight - 10, nTop + 7);

		dcMem.SelectObject(&m_penShadow);
		dcMem.MoveTo(nRight - 9, nTop + 8);
		dcMem.LineTo(nRight - 2, nTop + 8);
		dcMem.LineTo(nRight - 2, nTop);

		dcMem.SelectObject(&m_penFace);
		dcMem.MoveTo(nRight - 7, nTop + 6);
		dcMem.LineTo(nRight - 4, nTop + 3);
		dcMem.SetPixel(nRight - 5, nTop + 5, m_crFace);

		dcMem.SelectObject(&m_penLiteFace);
		dcMem.MoveTo(nRight - 8, nTop + 6);
		dcMem.LineTo(nRight - 3, nTop + 1);

		dcMem.SelectObject(&m_penLiteShadow);
		dcMem.MoveTo(nRight - 6, nTop + 6);
		dcMem.LineTo(nRight - 4, nTop + 6);
		dcMem.LineTo(nRight - 4, nTop + 2);
		
		rectItem = lpDIS->rcItem;
		pDC->BitBlt(rectItem.left, rectItem.top, rectItem.Width(), rectItem.Height(), &dcMem, rectItem.left, rectItem.top, SRCCOPY);

		dcMem.SelectObject(pOldPen);
		dcMem.SelectObject(pOldBmp);
		dcMem.RestoreDC(nSaveDC);
		dcMem.DeleteDC();
		bmp.DeleteObject();

		Sleep(50);
		RedrawWindow();
		return;
	}	// if drawing effect

	// Darken the colors if the mouse is down.
	COLORREF crSaved = m_crFace;
	if (lpDIS->itemState & ODS_SELECTED)
	{
		m_crFace = DarkenColor(m_crFace, 51);
		GetColors();
		CreatePens();
	}

	// Select the frame pen, since we always draw with that first.
	CPen *pOldPen = (CPen *)dcMem.SelectObject(&m_penDkShadow);

	// If "more" is shown, draw the arrow in the "more" position.
	if (m_bMoreShown)
	{
		nTop += 3;

		dcMem.MoveTo(nRight, nTop);
		dcMem.LineTo(nRight - 10, nTop);
		dcMem.LineTo(nRight - 5, nTop + 5);
		dcMem.LineTo(nRight, nTop);

		dcMem.SelectObject(&m_penShadow);
		dcMem.MoveTo(nRight - 4, nTop + 5);
		dcMem.LineTo(nRight + 1, nTop);

		dcMem.SelectObject(&m_penFace);
		dcMem.MoveTo(nRight - 7, nTop + 2);
		dcMem.LineTo(nRight - 3, nTop + 2);
		dcMem.MoveTo(nRight - 6, nTop + 3);
		dcMem.LineTo(nRight - 4, nTop + 3);
		dcMem.SetPixel(nRight - 2, nTop + 1, m_crFace);

		dcMem.SelectObject(&m_penLiteFace);
		dcMem.MoveTo(nRight - 8, nTop + 1);
		dcMem.LineTo(nRight - 2, nTop + 1);

		dcMem.SelectObject(&m_penLiteShadow);
		dcMem.MoveTo(nRight - 5, nTop + 4);
		dcMem.LineTo(nRight - 2, nTop + 1);
	}
	// Else draw the button in the normal position.
	else
	{
		dcMem.MoveTo(nRight - 7, nTop);
		dcMem.LineTo(nRight - 7, nTop + 10);
		dcMem.LineTo(nRight - 2, nTop + 5);
		dcMem.LineTo(nRight - 7, nTop);

		dcMem.SelectObject(&m_penShadow);
		dcMem.MoveTo(nRight - 6, nTop + 10);
		dcMem.LineTo(nRight - 1, nTop + 5);

		dcMem.SelectObject(&m_penFace);
		dcMem.MoveTo(nRight - 5, nTop + 3);
		dcMem.LineTo(nRight - 5, nTop + 7);
		dcMem.MoveTo(nRight - 4, nTop + 4);
		dcMem.LineTo(nRight - 4, nTop + 6);
		dcMem.SetPixel(nRight - 6, nTop + 8, m_crFace);

		dcMem.SelectObject(&m_penLiteFace);
		dcMem.MoveTo(nRight - 6, nTop + 2);
		dcMem.LineTo(nRight - 6, nTop + 8);

		dcMem.SelectObject(&m_penLiteShadow);
		dcMem.MoveTo(nRight - 5, nTop + 7);
		dcMem.LineTo(nRight - 2, nTop + 4);
	}	// else draw normal button

	rectItem = lpDIS->rcItem;
	pDC->BitBlt(rectItem.left, rectItem.top, rectItem.Width(), rectItem.Height(), &dcMem, rectItem.left, rectItem.top, SRCCOPY);

	// Restore the colors if they were darkened.
	if (lpDIS->itemState & ODS_SELECTED)
	{
		m_crFace = crSaved;
		GetColors();
		CreatePens();
	}

	// Clean up.
	dcMem.SelectObject(pOldPen);
	dcMem.SelectObject(pOldBmp);
	dcMem.RestoreDC(nSaveDC);
	dcMem.DeleteDC();
	bmp.DeleteObject();
}

VOID CMacMoreButton::SetColor(COLORREF crColor)
{
	m_crFace = crColor;
	GetColors();
	CreatePens();
}

VOID CMacMoreButton::OnLButtonUp(UINT32 nFlags, CPoint point) 
{
	CMacButton::OnLButtonUp(nFlags, point);

	CRect rect;
	GetClientRect(rect);
	if (m_bMouseDown && rect.PtInRect(point))
	{
		m_bMoreShown = !m_bMoreShown;
		m_bDoEffect = TRUE;
		RedrawWindow();
	}

	m_bMouseDown = FALSE;
}

VOID CMacMoreButton::OnLButtonDown(UINT32 nFlags, CPoint point) 
{
	m_bMouseDown = TRUE;
	
	CMacButton::OnLButtonDown(nFlags, point);
}

BOOL CMacMoreButton::GetMoreShown() const
{
	return m_bMoreShown;
}

VOID CMacMoreButton::SetTextAlign(INT32 nAlign)
{
	m_nAlignText = nAlign;
	RedrawWindow();
}

/* --------------------------------------------------------------------------------------------- */
/* CMacRoundButton                                                                               */
/* --------------------------------------------------------------------------------------------- */

CMacRoundButton::CMacRoundButton()
{
	m_bDrawDashedFocusCircle = TRUE;
}

CMacRoundButton::~CMacRoundButton()
{
	m_rgn.DeleteObject();
}

BEGIN_MESSAGE_MAP(CMacRoundButton, CButton)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CMacRoundButton::PreSubclassWindow() 
{
	CButton::PreSubclassWindow();

	ModifyStyle(0, BS_OWNERDRAW);

	CRect rect;
	GetClientRect(rect);

	// Resize the window to make it square
	rect.bottom	= rect.right = min(rect.bottom,rect.right);

	// Get the vital statistics of the window
	m_ptCenter	= rect.CenterPoint();
	m_i32Radius	= rect.bottom/2-1;

	// Set the window region so mouse clicks only activate the round section 
	// of the button
	m_rgn.DeleteObject(); 
	SetWindowRgn(NULL, FALSE);
	m_rgn.CreateEllipticRgnIndirect(rect);
	SetWindowRgn(m_rgn, TRUE);

	// Convert client coords to the parents client coords
	ClientToScreen(rect);
	CWnd* pParent = GetParent();
	if (pParent) pParent->ScreenToClient(rect);

	// Resize the window
	MoveWindow(rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
}

void CMacRoundButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	ASSERT(lpDrawItemStruct != NULL);
	
	CDC* pDC   = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rect = lpDrawItemStruct->rcItem;
	UINT state = lpDrawItemStruct->itemState;
	UINT nStyle = GetStyle();
	int nRadius = m_i32Radius;

	int nSavedDC = pDC->SaveDC();

	pDC->SelectStockObject(NULL_BRUSH);
	pDC->FillSolidRect(rect, ::GetSysColor(COLOR_BTNFACE));

	// Draw the focus circle around the button
	if ((state & ODS_FOCUS) && m_bDrawDashedFocusCircle)
		DrawCircle(pDC, m_ptCenter, nRadius--, RGB(0,0,0));

	// Draw the raised/sunken edges of the button (unless flat)
	if (nStyle & BS_FLAT) {
		DrawCircle(pDC, m_ptCenter, nRadius--, RGB(0,0,0));
		DrawCircle(pDC, m_ptCenter, nRadius--, ::GetSysColor(COLOR_3DHIGHLIGHT));
	} else {
		if ((state & ODS_SELECTED))	{
			DrawCircle(pDC, m_ptCenter, nRadius--, 
					   ::GetSysColor(COLOR_3DDKSHADOW), ::GetSysColor(COLOR_3DHIGHLIGHT));
			DrawCircle(pDC, m_ptCenter, nRadius--, 
					   ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DLIGHT));
		} else {
			DrawCircle(pDC, m_ptCenter, nRadius--, 
					   ::GetSysColor(COLOR_3DHIGHLIGHT), ::GetSysColor(COLOR_3DDKSHADOW));
			DrawCircle(pDC, m_ptCenter, nRadius--, 
					   ::GetSysColor(COLOR_3DLIGHT), ::GetSysColor(COLOR_3DSHADOW));
		}
	}
	
	// draw the text if there is any
	CString strText;
	GetWindowText(strText);

	if (!strText.IsEmpty())
	{
		CRgn rgn;
		rgn.CreateEllipticRgn(m_ptCenter.x-nRadius, m_ptCenter.y-nRadius, 
							  m_ptCenter.x+nRadius, m_ptCenter.y+nRadius);
		pDC->SelectClipRgn(&rgn);

		CSize Extent = pDC->GetTextExtent(strText);
		CPoint pt = CPoint( m_ptCenter.x - Extent.cx/2, m_ptCenter.x - Extent.cy/2 );

		if (state & ODS_SELECTED) pt.Offset(1,1);

		pDC->SetBkMode(TRANSPARENT);

		if (state & ODS_DISABLED)
			pDC->DrawState(pt, Extent, strText, DSS_DISABLED, TRUE, 0, (HBRUSH)NULL);
		else
			pDC->TextOut(pt.x, pt.y, strText);

		pDC->SelectClipRgn(NULL);
		rgn.DeleteObject();
	}

	// Draw the focus circle on the inside of the button
	if ((state & ODS_FOCUS) && m_bDrawDashedFocusCircle)
		DrawCircle(pDC, m_ptCenter, nRadius-2, RGB(0,0,0), TRUE);

	pDC->RestoreDC(nSavedDC);
}
