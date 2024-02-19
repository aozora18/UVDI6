
/*
 desc : 마크 그리기
*/

#include "pch.h"
#include "DrawMark.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : None
 retn : None
*/
CDrawMark::CDrawMark()
{
	m_hDrawWnd	= NULL;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDrawMark::~CDrawMark()
{
}

/*
 desc : 마크 그리기
 parm : inner	- [in]  내부 크기 (단위: pixel)
		outer	- [in]  외부 크기 (단위: pixel)
		reverse	- [in]  흑백 반전 여부 (0x00 - In:White/Out:Black, 0x01 - In:Black/Out:White)
		pattern	- [in]  마크 패턴 타입 (0x00 - Circle, 0x01 - Rectangle, 0x02 - Cross)
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

	/* 사각형 값 졍규화 */
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
	/* 그려질 영역의 윈도 핸들에 대한 DC 얻기 */
	hWndDC	= ::GetDC(m_hDrawWnd);
	if (!hWndDC)	return;

	/* 현재 그려질 영역의 윈도 크기 얻기 */
	::GetClientRect(m_hDrawWnd, &rWnd);
	/* 외곽선을 넘지 않기 위함 */
	rWnd.left++;
	rWnd.right--;
	rWnd.top++;
	rWnd.bottom--;

	/* 정사각형으로 조정 */
	if (rWnd.right - rWnd.left > rWnd.bottom - rWnd.top)
	{
		u32Temp	= (rWnd.right - rWnd.left) - (rWnd.bottom - rWnd.top);
		/* 넓이 값 조정. 즉, X1과 X2 좌표 위치 조정 */
		rWnd.left	+= u32Temp  / 2;
		rWnd.right	= (rWnd.bottom - rWnd.top);
	}
	else
	{
		u32Temp	= (rWnd.bottom - rWnd.top) - (rWnd.right - rWnd.left);
		/* 높이 값 조정. 즉, Y1과 Y2 좌표 위치 조정 */
		rWnd.top	+= u32Temp  / 2;
		rWnd.bottom	= (rWnd.right - rWnd.left);
	}

	/* 윈도 영역의 중심점 얻기 */
	dbCent	= rWnd.right / 2.0f;

	/* inner와 outer 중에 넓이와 높이 값이 가장 큰 값 구하기 */
	if (outer->right > inner->right)	u32Width	= outer->right;
	else								u32Width	= inner->right;
	if (outer->bottom > inner->bottom)	u32Height	= outer->bottom;
	else								u32Height	= inner->bottom;
	/* 현재 윈도 크기 대비 그리고자 하는 패턴 크기 비율 조정 */
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
	/* Inner 크기 조정 */
	if (bDrawInner)
	{
		dbWidth			= (inner->right - inner->left) * dbRate;
		dbHeight		= (inner->bottom - inner->top) * dbRate;
		inner->left		= (INT32)ROUNDDN((dbCent - dbWidth / 2.0f), 0);
		inner->top		= (INT32)ROUNDDN((dbCent - dbHeight / 2.0f), 0);
		inner->right	= inner->left + (INT32)ROUNDDN(dbWidth, 0);
		inner->bottom	= inner->top + (INT32)ROUNDDN(dbHeight, 0);
	}
	/* Outer 크기 조정 */
	if (bDrawOuter)
	{
		dbWidth			= (outer->right - outer->left) * dbRate;
		dbHeight		= (outer->bottom - outer->top) * dbRate;
		outer->left		= (INT32)ROUNDDN((dbCent - dbWidth / 2.0f), 0);
		outer->top		= (INT32)ROUNDDN((dbCent - dbHeight / 2.0f), 0);
		outer->right	= outer->left + (INT32)ROUNDDN(dbWidth, 0);
		outer->bottom	= outer->top + (INT32)ROUNDDN(dbHeight, 0);
	}
	/* Inner와 Outer의 색상 설정 */
	if (reverse)
	{
		clrIn	= RGB(0, 0, 0);
		clrOut	= RGB(255, 255, 255);
	}
#if 0
	/* 경계선 Pen 설정 */
	hPenBoard	= ::CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
#endif
	/* 채워질 Brush 설정 */
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

	/* 전체 배경 색상 출력 */
	hBrushSel	= (HBRUSH)::SelectObject(hWndDC, hBrushIn);
	::FillRect(hWndDC, &rWnd, hBrushIn);

	/* 패턴 타입별 처리 */
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

	/* 외곽선 그리기 */
	hPenSel	= (HPEN)::SelectObject(hWndDC, hPenBoard);
	if (hPenSel)
	{
		::LineTo(hWndDC, rWnd.left,	rWnd.top);
		::LineTo(hWndDC, rWnd.right,rWnd.top);
		::LineTo(hWndDC, rWnd.right,rWnd.bottom);
		::LineTo(hWndDC, rWnd.left,	rWnd.bottom);
		::LineTo(hWndDC, rWnd.left,	rWnd.top);
		/* 복원 */
		::SelectObject(hWndDC, hPenSel);
		::DeleteObject(hPenBoard);
	}

	/* 이전 Brush 객체 복원 */
	::SelectObject(hWndDC, hBrushSel);
	/* Brush 객체 메모리 해제 */
	if (hBrushIn)	::DeleteObject(hBrushIn);
	if (hBrushOut)	::DeleteObject(hBrushOut);
	/* DC 해제  */
	::ReleaseDC(m_hDrawWnd, hWndDC);
}

/*
 desc : 마크 그리기
 parm : file	- [in]  저장 파일 (전체 경로 포함)
		inner	- [in]  내부 크기 (단위: pixel)
		outer	- [in]  외부 크기 (단위: pixel)
		reverse	- [in]  흑백 반전 여부 (0x00 - In:White/Out:Black, 0x01 - In:Black/Out:White)
		pattern	- [in]  마크 패턴 타입 (0x00 - Circle, 0x01 - Rectangle, 0x02 - Cross)
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

	/* 그려질 영역의 메모리 DC 얻기 */
	hWndDC	= ::GetDC(m_hDrawWnd);
	if (!hWndDC)	return FALSE;
	hMemDC	= ::CreateCompatibleDC(hWndDC);
	if (!hMemDC)
	{
		::ReleaseDC(m_hDrawWnd, hWndDC);
		return FALSE;
	}

	/* 사각형 값 정규화 */
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

	/* 현재 그려질 이미지 영역의 크기 얻기 */
	i32Width	= inner->right;
	if (i32Width < outer->right)	i32Width = outer->right;
	i32Height	= inner->bottom;
	if (i32Height < outer->right)	i32Height = outer->right;
	rDraw.right	= i32Width;
	rDraw.bottom= i32Height;

	/* DC 영역에 연결된 비트맵 객체 생성 */
	hMemBmp		= ::CreateCompatibleBitmap(hWndDC, i32Width, i32Height);
	/* Memory DC와 Memory Bitmap 연결 */
	hBmpSel		= (HBITMAP)::SelectObject(hMemDC, hMemBmp);
	if (hBmpSel)
	{
		/* Inner 위치 조정 */
		if (bDrawInner)
		{
			u32Temp			= i32Width - inner->right;
			inner->left		= (INT32)ROUNDDN(u32Temp / 2.0f, 0);
			inner->right	+= inner->left;
			u32Temp			= i32Height - inner->bottom;
			inner->top		= (INT32)ROUNDDN(u32Temp / 2.0f, 0);
			inner->bottom	+= inner->top;
		}
		/* Outer 위치 조정 */
		if (bDrawOuter)
		{
			u32Temp			= i32Width - outer->right;
			outer->left		= (INT32)ROUNDDN(u32Temp / 2.0f, 0);
			outer->right	+= outer->left;
			u32Temp			= i32Height - outer->bottom;
			outer->top		= (INT32)ROUNDDN(u32Temp / 2.0f, 0);
			outer->bottom	+= outer->top;
		}
		/* Inner와 Outer의 색상 설정 */
		if (reverse)
		{
			clrIn	= RGB(0, 0, 0);
			clrOut	= RGB(255, 255, 255);
		}

		/* 채워질 Brush 설정 */
		hBrushIn	= ::CreateSolidBrush(clrIn);
		hBrushOut	= ::CreateSolidBrush(clrOut);
		if (hBrushIn && hBrushOut)
		{
			/* 전체 배경 색상 출력 */
			hBrushSel	= (HBRUSH)::SelectObject(hMemDC, hBrushIn);
			::FillRect(hMemDC, &rDraw, hBrushIn);

			/* 패턴 타입별 처리 */
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

			/* 메모리 DC의 이미지를 파일로 저장 */
			bSucc	= uvCmn_SaveDCToBitmap(hMemDC, hMemBmp, 8, i32Width, i32Height, file);

			/* 이전 Brush 객체 복원 */
			::SelectObject(hMemDC, hBrushSel);
			/* Brush 객체 메모리 해제 */
			if (hBrushIn)	::DeleteObject(hBrushIn);
			if (hBrushOut)	::DeleteObject(hBrushOut);
		}
		else bSucc	= FALSE;
		/* 객체 해제  */
		::DeleteObject(hMemBmp);
		::DeleteDC(hMemDC);
	}
	else bSucc	= FALSE;

	::ReleaseDC(m_hDrawWnd, hWndDC);

	return bSucc;
}
