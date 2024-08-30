#pragma once


// DlgGbrPreview


using namespace std;

#include "../../../inc/comn/MyDialog.h"
#include "../../../inc/grid/GridCtrl.h"
#include "../../../inc/kybd/DlgKBDN.h"

class DlgGbrPreview : public CMyDialog
{

	DECLARE_DYNAMIC(DlgGbrPreview)

	CBitmap bitmap;          // BMP �̹��� ��ü
	CRect clientRect;        // ���̾�α� Ŭ���̾�Ʈ ����
	float scaleFactor;       // ���� ������ ���� (0.3 ~ 2.0)
	CPoint scrollPos;        // ��ũ�� ��ġ
	bool isDragging;         // �巡�� ������ Ȯ��
	CPoint dragStartPoint;   // �巡�� ���� ��ġ


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


