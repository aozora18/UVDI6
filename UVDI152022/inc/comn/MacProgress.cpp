
/*
 desc : 프로그레스 컨트롤 재정의
*/

#include "pch.h"
#include "MacProgress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	IDT_INDETERMINATE		100
#define	IND_BAND_WIDTH			20


/*
 desc : Standard constructor.
 parm : None
 retn : None
*/
CMacProgress::CMacProgress()
{
	m_bIndeterminate	= FALSE;
	m_nIndOffset		= 0;
	m_crColor			= ::GetSysColor(COLOR_HIGHLIGHT);
	GetColors();
	CreatePens();
}

/*
 desc : Standard deconstructor.
 parm : None
 retn : None
*/
CMacProgress::~CMacProgress()
{
	DeletePens();
}

BEGIN_MESSAGE_MAP(CMacProgress, CProgressCtrl)
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

/*
 desc : Lightens a color by increasing the RGB values by the given number.
 parm : color	- References a COLORREF structure.
		reduce	- The amount to reduce the RGB values by.
 retn : None
*/
COLORREF LightenColor(const COLORREF color, BYTE reduce)
{
	BYTE byRed	= GetRValue(color);
	BYTE byGreen= GetGValue(color);
	BYTE byBlue	= GetBValue(color);

	if ((byRed + reduce) <= 255)	byRed	= BYTE(byRed + reduce);
	if ((byGreen + reduce)	<= 255)	byGreen	= BYTE(byGreen + reduce);
	if ((byBlue + reduce) <= 255)	byBlue	= BYTE(byBlue + reduce);

	return RGB(byRed, byGreen, byBlue);
}

/*
 desc : Darkens a color by reducing the RGB values by the given number.
 parm : color	- References a COLORREF structure.
		reduce	- The amount to reduce the RGB values by.
 retn : None
*/
COLORREF DarkenColor(const COLORREF color, BYTE reduce)
{
	BYTE byRed	= GetRValue(color);
	BYTE byGreen= GetGValue(color);
	BYTE byBlue	= GetBValue(color);

	if (byRed >= reduce)	byRed	= BYTE(byRed - reduce);
	if (byGreen >= reduce)	byGreen	= BYTE(byGreen - reduce);
	if (byBlue >= reduce)	byBlue	= BYTE(byBlue - reduce);

	return RGB(byRed, byGreen, byBlue);
}

/*
 desc : The framework calls this member function when Windows or an application makes a request to repaint a portion of an application뭩 window.
 parm : None
 retn : None
*/
void CMacProgress::OnPaint() 
{
	int nLower, nUpper;
	CPaintDC dcPaint(this); // device context for painting
	CRect rect, rectClient;
	CDC dc;
	CBitmap bmp;
	BOOL bVertical = GetStyle() & PBS_VERTICAL;

	GetClientRect(rectClient);
	rect = rectClient;

	// Create a memory DC for drawing.
	dc.CreateCompatibleDC(&dcPaint);
 	int nSavedDC	= dc.SaveDC();
	bmp.CreateCompatibleBitmap(&dcPaint, rect.Width(), rect.Height());
	CBitmap *pOldBmp= dc.SelectObject(&bmp);
	
	CBrush br1(m_crColorLightest);
	CBrush br2(::GetSysColor(COLOR_3DFACE));
	dc.FillRect(rect, &br2);

	GetRange(nLower, nUpper);

	// Determine the size of the bar and draw it.
	if (bVertical)
	{
		if (!m_bIndeterminate)
			rect.top = rect.bottom - int(((float)rect.Height() * float(GetPos() - nLower)) / float(nUpper - nLower));
		dc.FillRect(rect, &br1);
		DrawVerticalBar(&dc, rect);
	}
	else
  	{
		if (!m_bIndeterminate)
			rect.right = int(((float)rect.Width() * float(GetPos() - nLower)) / float(nUpper - nLower));
		dc.FillRect(rect, &br1);
		DrawHorizontalBar(&dc, rect);
	}

	dcPaint.BitBlt(rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), 
						&dc, rectClient.left, rectClient.top, SRCCOPY);

	dc.SelectObject(pOldBmp);
	dc.RestoreDC(nSavedDC);
	dc.DeleteDC();
}

/*
 desc : Draws a horizontal progress bar.
 parm : dc	- Specifies the device context object.
		rect- Specifies the rectangle of the progess bar.
 retn : None
*/
void CMacProgress::DrawHorizontalBar(CDC *dc, const CRect rect)
{
	int i;
	if (!rect.Width())	return;

	int nLeft = rect.left;
	int nTop = rect.top;
	int nBottom = rect.bottom;

	// Assume we're not drawing the indeterminate state.
	CPen *pOldPen = dc->SelectObject(&m_penColorLight);

	if (m_bIndeterminate)
	{
		pOldPen		= dc->SelectObject(&m_penColor);
		int nNumBands= (rect.Width() / IND_BAND_WIDTH) + 2;
		int nHeight	= rect.Height() + 1;

		int nAdjust	= nLeft - IND_BAND_WIDTH + m_nIndOffset;
		int nXpos	= 0;

		for (i= 0; i<nNumBands; i++)
		{
			nXpos = nAdjust + (i * IND_BAND_WIDTH);

			dc->SelectObject(&m_penColorDarker);
			dc->MoveTo(nXpos + 1, nTop);
			dc->LineTo(nXpos + nHeight, nBottom);

			dc->SelectObject(&m_penColorDark);
			dc->MoveTo(nXpos + 2, nTop);
			dc->LineTo(nXpos + nHeight + 1, nBottom);
			dc->MoveTo(nXpos + 10, nTop);
			dc->LineTo(nXpos + nHeight + 9, nBottom);

			dc->SelectObject(&m_penColor);
			dc->MoveTo(nXpos + 3, nTop);
			dc->LineTo(nXpos + nHeight + 2, nBottom);
			dc->MoveTo(nXpos + 9, nTop);
			dc->LineTo(nXpos + nHeight + 8, nBottom);

			dc->SelectObject(&m_penColorLight);
			dc->MoveTo(nXpos + 4, nTop);
			dc->LineTo(nXpos + nHeight + 3, nBottom);
			dc->MoveTo(nXpos + 8, nTop);
			dc->LineTo(nXpos + nHeight + 7, nBottom);

			dc->SelectObject(&m_penColorLighter);
			dc->MoveTo(nXpos + 5, nTop);
			dc->LineTo(nXpos + nHeight + 4, nBottom);
			dc->MoveTo(nXpos + 7, nTop);
			dc->LineTo(nXpos + nHeight + 6, nBottom);
		}	// for the number of bands
	}	// if indeterminate
	else
	{
		int nRight = rect.right;
	
		dc->MoveTo(nLeft + 2, nBottom - 4);
		dc->LineTo(nRight - 2, nBottom - 4);
		dc->MoveTo(nLeft + 2, nTop + 2);
		dc->LineTo(nRight - 2, nTop + 2);
		dc->SetPixel(nLeft + 1, nBottom - 3, m_crColorLight);
		dc->SetPixel(nLeft + 1, nTop + 1, m_crColorLight);

		dc->SelectObject(&m_penColorLighter);
		dc->MoveTo(nLeft + 2, nBottom - 5);
		dc->LineTo(nRight - 3, nBottom - 5);
		dc->LineTo(nRight - 3, nTop + 3);
		dc->LineTo(nLeft + 1, nTop + 3);
		dc->SetPixel(nLeft + 1, nBottom - 4, m_crColorLighter);
		dc->SetPixel(nLeft + 1, nTop + 2, m_crColorLighter);

		dc->SelectObject(&m_penColor);
		dc->MoveTo(nLeft, nBottom - 1);
		dc->LineTo(nLeft, nTop);
		dc->LineTo(nLeft + 2, nTop);
		dc->SetPixel(nLeft + 1, nBottom - 2, m_crColor);
		dc->MoveTo(nLeft + 2, nBottom - 3);
		dc->LineTo(nRight - 2, nBottom - 3);
		dc->MoveTo(nLeft + 2, nTop + 1);
		dc->LineTo(nRight - 1, nTop + 1);
		
		dc->SelectObject(&m_penColorDark);
		dc->MoveTo(nLeft + 2, nBottom - 2);
		dc->LineTo(nRight - 2, nBottom - 2);
		dc->LineTo(nRight - 2, nTop + 1);
		dc->MoveTo(nLeft + 2, nTop);
		dc->LineTo(nRight, nTop);
		dc->SetPixel(nLeft + 1, nBottom - 1, m_crColorDark);

		dc->SelectObject(&m_penColorDarker);
		dc->MoveTo(nLeft + 2, nBottom - 1);
		dc->LineTo(nRight - 1, nBottom - 1);
		dc->LineTo(nRight - 1, nTop);

		dc->SelectObject(&m_penShadow);
		dc->MoveTo(nRight, nTop);
 		dc->LineTo(nRight, nBottom);

		dc->SelectObject(&m_penLiteShadow);
 		dc->MoveTo(nRight + 1, nTop);
		dc->LineTo(nRight + 1, nBottom);
	}

	dc->SelectObject(pOldPen);
}

/*
 desc : Draws a vertical progress bar.
 parm : dc	- Specifies the device context object.
		rect- Specifies the rectangle of the progess bar.
 retn : None
*/
void CMacProgress::DrawVerticalBar(CDC *dc, const CRect rect)
{
	int i;
	int nHeight = rect.Height();
	if (!nHeight)	return;

	int nLeft	= rect.left;
	int nTop	= rect.top;
	int nRight	= rect.right;
	int nBottom	= rect.bottom;

	CPen *pOldPen = dc->SelectObject(&m_penColor);

	if (m_bIndeterminate)
	{
		int nNumBands = (nHeight / IND_BAND_WIDTH) + 2;

		int nAdjust = nBottom - m_nIndOffset;
		int nXpos1 = nLeft;
		int nXpos2 = nRight + 1;
		int nYpos = nTop + 1;

		nHeight = rect.Width() + 1;

		for (i=0; i<nNumBands; i++)
		{
			nYpos = nAdjust - (i * IND_BAND_WIDTH);

			dc->SelectObject(&m_penColorDarker);
			dc->MoveTo(nXpos1, nYpos);
			dc->LineTo(nXpos2, nYpos + nHeight);

			dc->SelectObject(&m_penColorDark);
			dc->MoveTo(nXpos1, nYpos + 1);
			dc->LineTo(nXpos2, nYpos + nHeight + 1);
			dc->MoveTo(nXpos1, nYpos + 9);
			dc->LineTo(nXpos2, nYpos + nHeight + 9);

			dc->SelectObject(&m_penColor);
			dc->MoveTo(nXpos1, nYpos + 2);
			dc->LineTo(nXpos2, nYpos + nHeight + 2);
			dc->MoveTo(nXpos1, nYpos + 8);
			dc->LineTo(nXpos2, nYpos + nHeight + 8);

			dc->SelectObject(&m_penColorLight);
			dc->MoveTo(nXpos1, nYpos + 3);
			dc->LineTo(nXpos2, nYpos + nHeight + 3);
			dc->MoveTo(nXpos1, nYpos + 7);
			dc->LineTo(nXpos2, nYpos + nHeight + 7);

			dc->SelectObject(&m_penColorLighter);
			dc->MoveTo(nXpos1, nYpos + 4);
			dc->LineTo(nXpos2, nYpos + nHeight + 4);
			dc->MoveTo(nXpos1, nYpos + 6);
			dc->LineTo(nXpos2, nYpos + nHeight + 6);
		}
	}
	else
	{
		if (nHeight > 3)
		{
			dc->MoveTo(nLeft, nTop + 1);
			dc->LineTo(nLeft, nTop);
			dc->LineTo(nRight, nTop);
			dc->MoveTo(nLeft + 1, nBottom - 2);
			dc->LineTo(nLeft + 1, nTop + 1);
			dc->MoveTo(nRight - 3, nBottom - 3);
			dc->LineTo(nRight - 3, nTop + 1);
			dc->SetPixel(nRight - 2, nTop + 1, m_crColor);

			dc->SelectObject(&m_penColorLight);
			dc->MoveTo(nLeft + 2, nBottom - 3);
			dc->LineTo(nLeft + 2, nTop + 1);
			dc->MoveTo(nRight - 4, nBottom - 3);
			dc->LineTo(nRight - 4, nTop + 1);
			dc->SetPixel(nLeft + 1, nTop + 1, m_crColorLight);
			dc->SetPixel(nRight - 3, nTop + 1, m_crColorLight);
			
			dc->SelectObject(&m_penColorLighter);
			dc->MoveTo(nLeft + 3, nBottom - 3);
			dc->LineTo(nLeft + 3, nTop + 1);
			dc->MoveTo(nRight - 5, nBottom - 3);
			dc->LineTo(nRight - 5, nTop + 1);
			dc->SetPixel(nLeft + 2, nTop + 1, m_crColorLighter);
			dc->SetPixel(nRight - 4, nTop + 1, m_crColorLighter);

			dc->SelectObject(&m_penColorDark);
			dc->MoveTo(nLeft, nBottom - 1);
			dc->LineTo(nLeft, nTop + 1);
			dc->MoveTo(nLeft + 2, nBottom - 2);
			dc->LineTo(nRight - 2, nBottom - 2);
			dc->LineTo(nRight - 2, nTop + 1);
			dc->SetPixel(nRight - 1, nTop + 1, m_crColorDark);

			dc->SelectObject(&m_penColorDarker);
			dc->MoveTo(nLeft + 1, nBottom - 1);
			dc->LineTo(nRight - 1, nBottom - 1);
			dc->LineTo(nRight - 1, nTop + 1);
		}
		else
		{
			CBrush br(m_crColor);
			CBrush *pOldBrush = dc->SelectObject(&br);
			dc->SelectObject(&m_penColorDark);
			dc->Rectangle(rect);
			dc->SelectObject(pOldBrush);
		}
	}

	dc->SelectObject(pOldPen);
}

/*
 desc : Calculates the lighter and darker colors, as well as the shadow colors.
 parm : None
 retn : None
*/
void CMacProgress::GetColors()
{
	m_crColorLight		= LightenColor(m_crColor, 51);
	m_crColorLighter	= LightenColor(m_crColorLight, 51);
	m_crColorLightest	= LightenColor(m_crColorLighter, 51);
	m_crColorDark		= DarkenColor(m_crColor, 51);
	m_crColorDarker		= DarkenColor(m_crColorDark, 51);
	m_crDkShadow		= ::GetSysColor(COLOR_3DDKSHADOW);
	m_crLiteShadow		= ::GetSysColor(COLOR_3DSHADOW);

	// Get a color halfway between COLOR_3DDKSHADOW and COLOR_3DSHADOW
	BYTE byRed3DDkShadow	= GetRValue(m_crDkShadow);
	BYTE byRed3DLiteShadow	= GetRValue(m_crLiteShadow);
	BYTE byGreen3DDkShadow	= GetGValue(m_crDkShadow);
	BYTE byGreen3DLiteShadow= GetGValue(m_crLiteShadow);
	BYTE byBlue3DDkShadow	= GetBValue(m_crDkShadow);
	BYTE byBlue3DLiteShadow	= GetBValue(m_crLiteShadow);

	m_crShadow = RGB(byRed3DLiteShadow + ((byRed3DDkShadow - byRed3DLiteShadow) >> 1),
						byGreen3DLiteShadow + ((byGreen3DDkShadow - byGreen3DLiteShadow) >> 1),
						byBlue3DLiteShadow + ((byBlue3DDkShadow - byBlue3DLiteShadow) >> 1));
}

/*
 desc : Sets the progress bar control's color. The lighter darker colors are recalculated, and the pens recreated.
 parm : color - New color.
 retn : None
*/
void CMacProgress::SetColor(COLORREF color)
{
	m_crColor	= color;
	GetColors();
	CreatePens();
	RedrawWindow();
}

/*
 desc : Returns the progress bar control's current color.
 parm : None
 retn : The current color.
*/
COLORREF CMacProgress::GetColor()
{
	return m_crColor;
}

/*
 desc : Deletes the pen objects, if necessary, and creates them.
 parm : None
 retn : None
*/
void CMacProgress::CreatePens()
{
	DeletePens();

	m_penColorLight.CreatePen(PS_SOLID, 1, m_crColorLight);
	m_penColorLighter.CreatePen(PS_SOLID, 1, m_crColorLighter);
	m_penColor.CreatePen(PS_SOLID, 1, m_crColor);
	m_penColorDark.CreatePen(PS_SOLID, 1, m_crColorDark);
	m_penColorDarker.CreatePen(PS_SOLID, 1, m_crColorDarker);
	m_penDkShadow.CreatePen(PS_SOLID, 1, m_crDkShadow);
	m_penShadow.CreatePen(PS_SOLID, 1, m_crShadow);
	m_penLiteShadow.CreatePen(PS_SOLID, 1, m_crLiteShadow);
}

/*
 desc : Deletes the pen objects.
 parm : None
 retn : None
*/
void CMacProgress::DeletePens()
{
	if (m_penColorLight.m_hObject)		m_penColorLight.DeleteObject();
	if (m_penColorLighter.m_hObject)	m_penColorLighter.DeleteObject();
	if (m_penColor.m_hObject)			m_penColor.DeleteObject();
	if (m_penColorDark.m_hObject)		m_penColorDark.DeleteObject();
	if (m_penColorDarker.m_hObject)		m_penColorDarker.DeleteObject();
	if (m_penDkShadow.m_hObject)		m_penDkShadow.DeleteObject();
	if (m_penShadow.m_hObject)			m_penShadow.DeleteObject();
	if (m_penLiteShadow.m_hObject)		m_penLiteShadow.DeleteObject();
}

/*
 desc : Sets the indeterminate flag.
 parm : indeterminate	- Specifies the indeterminate state.
 retn : None
*/
void CMacProgress::SetIndeterminate(BOOL indeterminate)
{
	m_bIndeterminate = indeterminate;

	if (m_bIndeterminate)
	{
		CRect rect;
		GetClientRect(rect);
		m_nIndOffset = 0;

		RedrawWindow();
		SetTimer(IDT_INDETERMINATE, 25, NULL);
	}
	else
	{
		KillTimer(IDT_INDETERMINATE);
		RedrawWindow();
	}
}

/*
 desc : Returns m_bIndeterminate.
 parm : None
 retn : m_bIndeterminate
*/
BOOL CMacProgress::GetIndeterminate()
{
	return m_bIndeterminate;
}

/*
 desc : The framework calls this member function after each interval specified in the SetTimer member function used to install a timer.
 parm : id	- Specifies the identifier of the timer.
 retn : m_bIndeterminate
*/
void CMacProgress::OnTimer(UINT_PTR id) 
{
	// Increment the indeterminate bar offset and redraw the window.
	if (id == IDT_INDETERMINATE)
	{
		KillTimer(id);

		if (++m_nIndOffset > IND_BAND_WIDTH - 1)	m_nIndOffset = 0;
		RedrawWindow();

		SetTimer(IDT_INDETERMINATE, 25, NULL);
	}
}