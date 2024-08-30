#pragma once


// DlgGbrPreview


using namespace std;

#include "../../../inc/comn/MyDialog.h"
#include "../../../inc/grid/GridCtrl.h"
#include "../../../inc/kybd/DlgKBDN.h"

class DlgGbrPreview : public CMyDialog
{

	DECLARE_DYNAMIC(DlgGbrPreview)

	CBitmap bitmap;          // BMP 이미지 객체
	CRect clientRect;        // 다이얼로그 클라이언트 영역
	float scaleFactor;       // 현재 스케일 비율 (0.3 ~ 2.0)
	CPoint scrollPos;        // 스크롤 위치
	bool isDragging;         // 드래그 중인지 확인
	CPoint dragStartPoint;   // 드래그 시작 위치


public:
	enum { IDD = IDD_PREVIEW };
	DlgGbrPreview(CWnd* pParent = NULL);
	virtual ~DlgGbrPreview();

protected:

	BOOL		OnInitDlg() { return TRUE; }
	VOID		OnExitDlg() {}
	VOID		OnPaintDlg(CDC* dc) {}
	VOID		OnResizeDlg() {}
	void		RegisterUILevel() {}

	

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();




	afx_msg void OnClose();
//	afx_msg void OnDestroy();
};


