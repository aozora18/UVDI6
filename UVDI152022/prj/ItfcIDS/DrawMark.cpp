
/*
 desc : ��ũ �׸���
*/

#include "pch.h"
#include "DrawMark.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : None
 retn : None
*/
CDrawMark::CDrawMark()
{
	m_hDrawWnd	= NULL;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDrawMark::~CDrawMark()
{
}

/*
 desc : ��ũ �׸���
 parm : inner	- [in]  ���� ũ�� (����: pixel)
		outer	- [in]  �ܺ� ũ�� (����: pixel)
		reverse	- [in]  ��� ���� ���� (0x00 - In:White/Out:Black, 0x01 - In:Black/Out:White)
		pattern	- [in]  ��ũ ���� Ÿ�� (0x00 - Circle, 0x01 - Rectangle, 0x02 - Cross)
 retn : None
*/
VOID CDrawMark::Draw(LPRECT inner, LPRECT outer, UINT8 reverse, UINT8 pattern)
{
	BOOL bDrawInner	= TRUE, bDrawOuter = TRUE;
	UINT32 u32Temp	= 0, u32Width, u32Height;
	DOUBLE dbCent	= 0.0f, dbRate = 0.0f, dbWidth = 0.0f, dbHeight = 0;
	COLORREF clrIn	= RGB(255, 255, 255), clrOut = RGB(0, 0, 0);
	HPEN hPenBoard	= NULL, hPenSel = NULL;
	HBRUSH hBrushIn	= NULL, hBrushOut = NULL, hBrushSel = NULL;
	HDC hWndDC		= NULL;
	RECT rWnd;

	if (!m_hDrawWnd)	return;

	/* �簢�� �� ����ȭ */
	bDrawInner	= (inner->right - inner->left > 0) && (inner->bottom - inner->top > 0);
	if (bDrawInner)
	{
		inner->right	= inner->right - inner->left;
		inner->left		= 0;
		inner->bottom	= inner->bottom - inner->top;
		inner->top		= 0;
	}
	bDrawOuter	= (outer->right - outer->left > 0) && (outer->bottom - outer->top > 0);
	if (bDrawOuter)
	{
		outer->right	= outer->right - outer->left;
		outer->left		= 0;
		outer->bottom	= outer->bottom - outer->top;
		outer->top		= 0;
	}
	/* �׷��� ������ ���� �ڵ鿡 ���� DC ��� */
	hWndDC	= ::GetDC(m_hDrawWnd);
	if (!hWndDC)	return;

	/* ���� �׷��� ������ ���� ũ�� ��� */
	::GetClientRect(m_hDrawWnd, &rWnd);
	/* �ܰ����� ���� �ʱ� ���� */
	rWnd.left++;
	rWnd.right--;
	rWnd.top++;
	rWnd.bottom--;

	/* ���簢������ ���� */
	if (rWnd.right - rWnd.left > rWnd.bottom - rWnd.top)
	{
		u32Temp	= (rWnd.right - rWnd.left) - (rWnd.bottom - rWnd.top);
		/* ���� �� ����. ��, X1�� X2 ��ǥ ��ġ ���� */
		rWnd.left	+= u32Temp  / 2;
		rWnd.right	= (rWnd.bottom - rWnd.top);
	}
	else
	{
		u32Temp	= (rWnd.bottom - rWnd.top) - (rWnd.right - rWnd.left);
		/* ���� �� ����. ��, Y1�� Y2 ��ǥ ��ġ ���� */
		rWnd.top	+= u32Temp  / 2;
		rWnd.bottom	= (rWnd.right - rWnd.left);
	}

	/* ���� ������ �߽��� ��� */
	dbCent	= rWnd.right / 2.0f;

	/* inner�� outer �߿� ���̿� ���� ���� ���� ū �� ���ϱ� */
	if (outer->right > inner->right)	u32Width	= outer->right;
	else								u32Width	= inner->right;
	if (outer->bottom > inner->bottom)	u32Height	= outer->bottom;
	else								u32Height	= inner->bottom;
	/* ���� ���� ũ�� ��� �׸����� �ϴ� ���� ũ�� ���� ���� */
#if 0
	if (outer->right > outer->bottom)
		dbRate	= DOUBLE(rWnd.right - rWnd.left) / DOUBLE(outer->right);
	else
		dbRate	= DOUBLE(rWnd.bottom - rWnd.top) / DOUBLE(outer->bottom);
#else
	if (u32Width > u32Height)
		dbRate	= DOUBLE(rWnd.right - rWnd.left) / DOUBLE(u32Width);
	else
		dbRate	= DOUBLE(rWnd.bottom - rWnd.top) / DOUBLE(u32Height);
#endif
	/* Inner ũ�� ���� */
	if (bDrawInner)
	{
		dbWidth			= (inner->right - inner->left) * dbRate;
		dbHeight		= (inner->bottom - inner->top) * dbRate;
		inner->left		= (INT32)ROUNDDN((dbCent - dbWidth / 2.0f), 0);
		inner->top		= (INT32)ROUNDDN((dbCent - dbHeight / 2.0f), 0);
		inner->right	= inner->left + (INT32)ROUNDDN(dbWidth, 0);
		inner->bottom	= inner->top + (INT32)ROUNDDN(dbHeight, 0);
	}
	/* Outer ũ�� ���� */
	if (bDrawOuter)
	{
		dbWidth			= (outer->right - outer->left) * dbRate;
		dbHeight		= (outer->bottom - outer->top) * dbRate;
		outer->left		= (INT32)ROUNDDN((dbCent - dbWidth / 2.0f), 0);
		outer->top		= (INT32)ROUNDDN((dbCent - dbHeight / 2.0f), 0);
		outer->right	= outer->left + (INT32)ROUNDDN(dbWidth, 0);
		outer->bottom	= outer->top + (INT32)ROUNDDN(dbHeight, 0);
	}
	/* Inner�� Outer�� ���� ���� */
	if (reverse)
	{
		clrIn	= RGB(0, 0, 0);
		clrOut	= RGB(255, 255, 255);
	}
#if 0
	/* ��輱 Pen ���� */
	hPenBoard	= ::CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
#endif
	/* ä���� Brush ���� */
	hBrushIn	= ::CreateSolidBrush(clrIn);
	hBrushOut	= ::CreateSolidBrush(clrOut);
	if (!hBrushIn || !hBrushOut/* || !hPenBoard*/)
	{
		if (hBrushIn)	::DeleteObject(hBrushIn);
		if (hBrushOut)	::DeleteObject(hBrushOut);
#if 0
		if (hPenBoard)	::DeleteObject(hPenBoard);
#endif
		::ReleaseDC(m_hDrawWnd, hWndDC);
		return;
	}

	/* ��ü ��� ���� ��� */
	hBrushSel	= (HBRUSH)::SelectObject(hWndDC, hBrushIn);
	::FillRect(hWndDC, &rWnd, hBrushIn);

	/* ���� Ÿ�Ժ� ó�� */
	switch (pattern)
	{
	/* Circle */
	case 0x00	:
		if ((HBRUSH)::SelectObject(hWndDC, hBrushOut))
		{
			/* External Circle */
			if (bDrawOuter)	::Ellipse(hWndDC, outer->left, outer->top, outer->right, outer->bottom);
			/* Internal Circle */
			if ((HBRUSH)::SelectObject(hWndDC, hBrushIn))
			{
				if (bDrawInner)	::Ellipse(hWndDC, inner->left, inner->top, inner->right, inner->bottom);
			}
		}
		break;

	/* Rectangle */
	case 0x01	:
		if ((HBRUSH)::SelectObject(hWndDC, hBrushOut))
		{
			/* External Rectangle */
			if (bDrawOuter)	::FillRect(hWndDC, outer, hBrushOut);
			if ((HBRUSH)::SelectObject(hWndDC, hBrushIn))
			{
				if (bDrawInner)	::FillRect(hWndDC, inner, hBrushIn);
			}
		}
		break;

	/* Cross */
	case 0x02	:
		if ((HBRUSH)::SelectObject(hWndDC, hBrushOut))
		{
			/* Horizonal Rectangle */
			if (bDrawInner)	::FillRect(hWndDC, inner, hBrushOut);
			/* Vertical Rectangle */
			if (bDrawOuter)	::FillRect(hWndDC, outer, hBrushOut);
		}
		break;
	}

	/* �ܰ��� �׸��� */
	hPenSel	= (HPEN)::SelectObject(hWndDC, hPenBoard);
	if (hPenSel)
	{
		::LineTo(hWndDC, rWnd.left,	rWnd.top);
		::LineTo(hWndDC, rWnd.right,rWnd.top);
		::LineTo(hWndDC, rWnd.right,rWnd.bottom);
		::LineTo(hWndDC, rWnd.left,	rWnd.bottom);
		::LineTo(hWndDC, rWnd.left,	rWnd.top);
		/* ���� */
		::SelectObject(hWndDC, hPenSel);
		::DeleteObject(hPenBoard);
	}

	/* ���� Brush ��ü ���� */
	::SelectObject(hWndDC, hBrushSel);
	/* Brush ��ü �޸� ���� */
	if (hBrushIn)	::DeleteObject(hBrushIn);
	if (hBrushOut)	::DeleteObject(hBrushOut);
	/* DC ����  */
	::ReleaseDC(m_hDrawWnd, hWndDC);
}

/*
 desc : ��ũ �׸���
 parm : file	- [in]  ���� ���� (��ü ��� ����)
		inner	- [in]  ���� ũ�� (����: pixel)
		outer	- [in]  �ܺ� ũ�� (����: pixel)
		reverse	- [in]  ��� ���� ���� (0x00 - In:White/Out:Black, 0x01 - In:Black/Out:White)
		pattern	- [in]  ��ũ ���� Ÿ�� (0x00 - Circle, 0x01 - Rectangle, 0x02 - Cross)
 retn : TRUE or FALSE
*/
BOOL CDrawMark::Save(TCHAR *file, LPRECT inner, LPRECT outer, UINT8 reverse, UINT8 pattern)
{
	BOOL bDrawInner	= TRUE, bDrawOuter = TRUE, bSucc = TRUE;
	INT32 u32Temp	= 0, i32Width = 0, i32Height = 0;
	COLORREF clrIn	= RGB(255, 255, 255), clrOut = RGB(0, 0, 0);
	HBRUSH hBrushIn	= NULL, hBrushOut = NULL, hBrushSel = NULL;
	HDC hMemDC		= NULL, hWndDC = NULL;
	HBITMAP hMemBmp	= NULL, hBmpSel= NULL;
	RECT rDraw		= {NULL};
	BITMAP stBitmap	= {NULL};
	BITMAPINFOHEADER stBmpHead	= {NULL};

	if (!m_hDrawWnd)	return FALSE;

	/* �׷��� ������ �޸� DC ��� */
	hWndDC	= ::GetDC(m_hDrawWnd);
	if (!hWndDC)	return FALSE;
	hMemDC	= ::CreateCompatibleDC(hWndDC);
	if (!hMemDC)
	{
		::ReleaseDC(m_hDrawWnd, hWndDC);
		return FALSE;
	}

	/* �簢�� �� ����ȭ */
	bDrawInner	= (inner->right - inner->left > 0) && (inner->bottom - inner->top > 0);
	if (bDrawInner)
	{
		inner->right	= inner->right - inner->left;
		inner->left		= 0;
		inner->bottom	= inner->bottom - inner->top;
		inner->top		= 0;
	}
	bDrawOuter	= (outer->right - outer->left > 0) && (outer->bottom - outer->top > 0);
	if (bDrawOuter)
	{
		outer->right	= outer->right - outer->left;
		outer->left		= 0;
		outer->bottom	= outer->bottom - outer->top;
		outer->top		= 0;
	}

	/* ���� �׷��� �̹��� ������ ũ�� ��� */
	i32Width	= inner->right;
	if (i32Width < outer->right)	i32Width = outer->right;
	i32Height	= inner->bottom;
	if (i32Height < outer->right)	i32Height = outer->right;
	rDraw.right	= i32Width;
	rDraw.bottom= i32Height;

	/* DC ������ ����� ��Ʈ�� ��ü ���� */
	hMemBmp		= ::CreateCompatibleBitmap(hWndDC, i32Width, i32Height);
	/* Memory DC�� Memory Bitmap ���� */
	hBmpSel		= (HBITMAP)::SelectObject(hMemDC, hMemBmp);
	if (hBmpSel)
	{
		/* Inner ��ġ ���� */
		if (bDrawInner)
		{
			u32Temp			= i32Width - inner->right;
			inner->left		= (INT32)ROUNDDN(u32Temp / 2.0f, 0);
			inner->right	+= inner->left;
			u32Temp			= i32Height - inner->bottom;
			inner->top		= (INT32)ROUNDDN(u32Temp / 2.0f, 0);
			inner->bottom	+= inner->top;
		}
		/* Outer ��ġ ���� */
		if (bDrawOuter)
		{
			u32Temp			= i32Width - outer->right;
			outer->left		= (INT32)ROUNDDN(u32Temp / 2.0f, 0);
			outer->right	+= outer->left;
			u32Temp			= i32Height - outer->bottom;
			outer->top		= (INT32)ROUNDDN(u32Temp / 2.0f, 0);
			outer->bottom	+= outer->top;
		}
		/* Inner�� Outer�� ���� ���� */
		if (reverse)
		{
			clrIn	= RGB(0, 0, 0);
			clrOut	= RGB(255, 255, 255);
		}

		/* ä���� Brush ���� */
		hBrushIn	= ::CreateSolidBrush(clrIn);
		hBrushOut	= ::CreateSolidBrush(clrOut);
		if (hBrushIn && hBrushOut)
		{
			/* ��ü ��� ���� ��� */
			hBrushSel	= (HBRUSH)::SelectObject(hMemDC, hBrushIn);
			::FillRect(hMemDC, &rDraw, hBrushIn);

			/* ���� Ÿ�Ժ� ó�� */
			switch (pattern)
			{
			/* Circle */
			case 0x00	:
				if ((HBRUSH)::SelectObject(hMemDC, hBrushOut))
				{
					/* External Circle */
					if (bDrawOuter)	::Ellipse(hMemDC, outer->left, outer->top, outer->right, outer->bottom);
					/* Internal Circle */
					if ((HBRUSH)::SelectObject(hMemDC, hBrushIn))
					{
						if (bDrawInner)	::Ellipse(hMemDC, inner->left, inner->top, inner->right, inner->bottom);
					}
				}
				break;

			/* Rectangle */
			case 0x01	:
				if ((HBRUSH)::SelectObject(hMemDC, hBrushOut))
				{
					/* External Rectangle */
					if (bDrawOuter)	::FillRect(hMemDC, outer, hBrushOut);
					if ((HBRUSH)::SelectObject(hMemDC, hBrushIn))
					{
						if (bDrawInner)	::FillRect(hMemDC, inner, hBrushIn);
					}
				}
				break;

			/* Cross */
			case 0x02	:
				if ((HBRUSH)::SelectObject(hMemDC, hBrushOut))
				{
					/* Horizonal Rectangle */
					if (bDrawInner)	::FillRect(hMemDC, inner, hBrushOut);
					/* Vertical Rectangle */
					if (bDrawOuter)	::FillRect(hMemDC, outer, hBrushOut);
				}
				break;
			}

			/* �޸� DC�� �̹����� ���Ϸ� ���� */
			bSucc	= uvCmn_SaveDCToBitmap(hMemDC, hMemBmp, 8, i32Width, i32Height, file);

			/* ���� Brush ��ü ���� */
			::SelectObject(hMemDC, hBrushSel);
			/* Brush ��ü �޸� ���� */
			if (hBrushIn)	::DeleteObject(hBrushIn);
			if (hBrushOut)	::DeleteObject(hBrushOut);
		}
		else bSucc	= FALSE;
		/* ��ü ����  */
		::DeleteObject(hMemBmp);
		::DeleteDC(hMemDC);
	}
	else bSucc	= FALSE;

	::ReleaseDC(m_hDrawWnd, hWndDC);

	return bSucc;
}
