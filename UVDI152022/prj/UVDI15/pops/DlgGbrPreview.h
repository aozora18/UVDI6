#pragma once


// DlgGbrPreview


using namespace std;

#include "../../../inc/comn/MyDialog.h"
#include "../../../inc/grid/GridCtrl.h"
#include "../../../inc/kybd/DlgKBDN.h"

class DlgGbrPreview : public CMyDialog
{
	DECLARE_DYNAMIC(DlgGbrPreview)

public:
	enum { IDD = IDD_PREVIEW };
	DlgGbrPreview(CWnd* pParent = NULL);
	virtual ~DlgGbrPreview();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};


