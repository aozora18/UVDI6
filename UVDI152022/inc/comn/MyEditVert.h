#pragma once

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "MyEdit.h"

class CMyEditVert : public CMyEdit
{
/* ������ / �ı��� */
public:

	CMyEditVert();
	virtual ~CMyEditVert();

/* ���� ���� */
protected:


/* ���� �Լ� */
protected:

/* ���� �Լ� */
public:


/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnNcCalcSize(BOOL calc_valid_rect, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg VOID		OnNcPaint();
	afx_msg HBRUSH		CtlColor(CDC* dc, UINT32 color);

};
