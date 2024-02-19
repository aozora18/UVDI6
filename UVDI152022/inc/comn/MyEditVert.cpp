
/*
 desc : 에디트 박스 내 문자열 수직으로 가운데 오도록 처리
*/

#include "pch.h"
#include "MyEditVert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMyEditVert::CMyEditVert() 
{
}

CMyEditVert::~CMyEditVert()
{
}


BEGIN_MESSAGE_MAP(CMyEditVert, CMyEdit)
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

VOID CMyEditVert::OnNcCalcSize(BOOL calc_valid_rect, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	CRect rWnd, rClient;
	
	/* calculate client area height needed for a font */
	CRect rText;
	rText.SetRectEmpty();
	
	CFont *pFont= GetFont();
	CDC *pDC	= GetDC();	
	CFont *pOld	= pDC->SelectObject(pFont);
	pDC->DrawText(L"Ky", rText, DT_CALCRECT | DT_LEFT);
	UINT32 u32VClientHeight	= rText.Height();
	
	pDC->SelectObject(pOld);
	ReleaseDC(pDC);
	
	// calculate NC area to center text.
	GetClientRect(rClient);
	GetWindowRect(rWnd);
	ClientToScreen(rClient);
	UINT32 u32CenterOffset = (rClient.Height() - u32VClientHeight) / 2;
	UINT32 u32CY= (rWnd.Height() - rClient.Height()) / 2;
	UINT32 u32CX= (rWnd.Width() - rClient.Width()) / 2;

	rWnd.OffsetRect(-rWnd.left, -rWnd.top);
	m_rNCTop	= rWnd;

	m_rNCTop.DeflateRect(u32CX, u32CY, u32CX, u32CenterOffset + u32VClientHeight + u32CY);

	m_rNCBottom	= rWnd;

	m_rNCBottom.DeflateRect(u32CX, u32CenterOffset + u32VClientHeight + u32CY, u32CX, u32CY);

	lpncsp->rgrc[0].top		+= u32CenterOffset;
	lpncsp->rgrc[0].bottom	-= u32CenterOffset;

	lpncsp->rgrc[0].left	+= u32CX;
	lpncsp->rgrc[0].right	-= u32CY;
}

VOID CMyEditVert::OnNcPaint() 
{
	Default();

	CWindowDC dc(this);
	CBrush csBrush(GetSysColor(COLOR_WINDOW));

	dc.FillRect(m_rNCBottom, &csBrush);
	dc.FillRect(m_rNCTop, &csBrush);
}

HBRUSH CMyEditVert::CtlColor(CDC* dc, UINT32 color) 
{
	if(m_rNCTop.IsRectEmpty())
	{
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
	}

	return CMyEdit::CtlColor(dc, color);
//	return NULL;
}
