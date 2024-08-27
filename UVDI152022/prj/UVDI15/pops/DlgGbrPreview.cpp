// DlgGbrPreview.cpp: 구현 파일
//

#include "pch.h"
#include "DlgGbrPreview.h"

// DlgGbrPreview

IMPLEMENT_DYNAMIC(DlgGbrPreview, CDialogEx)

DlgGbrPreview::DlgGbrPreview(CWnd* pParent) : CMyDialog(DlgGbrPreview::IDD, pParent)
{

}

DlgGbrPreview::~DlgGbrPreview()
{
}


BEGIN_MESSAGE_MAP(DlgGbrPreview, CWnd)
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// DlgGbrPreview 메시지 처리기




BOOL DlgGbrPreview::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}


void DlgGbrPreview::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CWnd::OnMouseMove(nFlags, point);
}


void DlgGbrPreview::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CWnd::OnLButtonDown(nFlags, point);
}


void DlgGbrPreview::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CWnd::OnLButtonUp(nFlags, point);
}
