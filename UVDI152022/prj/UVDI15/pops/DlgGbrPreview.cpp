// DlgGbrPreview.cpp: 구현 파일
//

#include "pch.h"
#include "DlgGbrPreview.h"

// DlgGbrPreview

const float MINSCALE = 0.1f;
const float MAXSCALE = 2.0f;
const float SCALERATE = 1.1f;

IMPLEMENT_DYNAMIC(DlgGbrPreview, CDialogEx)

DlgGbrPreview::DlgGbrPreview(CWnd* pParent) : CMyDialog(DlgGbrPreview::IDD, pParent)
{

}

DlgGbrPreview::~DlgGbrPreview()
{
	if (bitmap.GetSafeHandle() != NULL)
		bitmap.DeleteObject();

	CMyDialog::~CMyDialog();
}


BEGIN_MESSAGE_MAP(DlgGbrPreview, CWnd)
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	ON_WM_CLOSE()
//	ON_WM_DESTROY()
END_MESSAGE_MAP()



// DlgGbrPreview 메시지 처리기

BOOL DlgGbrPreview::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	BITMAP bmpInfo;
	bitmap.GetBitmap(&bmpInfo);

	int currentImgWidth = static_cast<int>(bmpInfo.bmWidth * scaleFactor);
	int currentImgHeight = static_cast<int>(bmpInfo.bmHeight * scaleFactor);

	
	CPoint currentCenter
	(
		scrollPos.x + clientRect.Width() / 2,
		scrollPos.y + clientRect.Height() / 2
	);

	if (zDelta > 0 && scaleFactor < MAXSCALE)
		scaleFactor *= SCALERATE;
	else if (zDelta < 0 && scaleFactor > MINSCALE)
		scaleFactor /= SCALERATE; 

	
	int newImgWidth = static_cast<int>(bmpInfo.bmWidth * scaleFactor);
	int newImgHeight = static_cast<int>(bmpInfo.bmHeight * scaleFactor);

	CPoint newScrollPos;
	newScrollPos.x = currentCenter.x - (clientRect.Width() * currentCenter.x) / newImgWidth;
	newScrollPos.y = currentCenter.y - (clientRect.Height() * currentCenter.y) / newImgHeight;
	
	newScrollPos.x = max(0, min(newScrollPos.x, newImgWidth - clientRect.Width()));
	newScrollPos.y = max(0, min(newScrollPos.y, newImgHeight - clientRect.Height()));

	scrollPos = newScrollPos;

	Invalidate();

	return TRUE;
}


void DlgGbrPreview::OnMouseMove(UINT nFlags, CPoint point)
{
	if (isDragging)
	{
		CPoint diff = point - dragStartPoint;
		scrollPos -= diff;
		dragStartPoint = point;

		Invalidate();
	}

	CWnd::OnMouseMove(nFlags, point);
}


void DlgGbrPreview::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	isDragging = true;
	dragStartPoint = point;
	CWnd::OnLButtonDown(nFlags, point);
}


void DlgGbrPreview::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	isDragging = false;
	CWnd::OnLButtonUp(nFlags, point);
}


void DlgGbrPreview::OnPaint()
{
	
	CPaintDC dc(this); // 그리기 DC

	if (bitmap.GetSafeHandle() == NULL)
		return;

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);

	BITMAP bmpInfo;
	bitmap.GetBitmap(&bmpInfo);

	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);

	int imgWidth = static_cast<int>(bmpInfo.bmWidth * scaleFactor);
	int imgHeight = static_cast<int>(bmpInfo.bmHeight * scaleFactor);

	
	int xPos = max(0, (clientRect.Width() - imgWidth) / 2);
	int yPos = max(0, (clientRect.Height() - imgHeight) / 2);

	
	if (imgWidth > clientRect.Width())
		xPos = -scrollPos.x;

	if (imgHeight > clientRect.Height())
		yPos = -scrollPos.y;

	dc.StretchBlt(xPos, yPos, imgWidth, imgHeight, &memDC, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, SRCCOPY);

	memDC.SelectObject(pOldBitmap);
}


BOOL DlgGbrPreview::OnInitDialog()
{
	CMyDialog::OnInitDialog();
	CHAR szFile[MAX_PATH_LEN] = { NULL };
	CUniToChar u2c;

	scaleFactor = MINSCALE;
	scrollPos = CPoint(0, 0);
	isDragging = false;


	GetClientRect(&clientRect);
	bool isLocalSelRecipe = uvEng_JobRecipe_WhatLastSelectIsLocal();
	auto recipe = uvEng_JobRecipe_GetSelectRecipe(isLocalSelRecipe);
	sprintf_s(szFile, MAX_PATH_LEN, "%s\\%s\\PreviewDownscale.bmp", recipe->gerber_path, recipe->gerber_name);

	auto uniName = u2c.Ansi2Uni(szFile);
	if (uvCmn_FindFile(uniName) == false)
	{
		OnClose();
	}
	else
	{
		
		bitmap.Attach((HBITMAP)::LoadImage(NULL, uniName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION));
	}
	return TRUE;
}


void DlgGbrPreview::OnClose()
{
	// 다이얼로그 종료
	EndDialog(IDOK); // 또는 IDCANCEL을 사용하여 다이얼로그를 종료할 수 있습니다.
	CMyDialog::OnClose();
}

