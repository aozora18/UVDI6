#pragma once

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "MyEdit.h"

class CMyEditVert : public CMyEdit
{
/* 생성자 / 파괴자 */
public:

	CMyEditVert();
	virtual ~CMyEditVert();

/* 로컬 변수 */
protected:


/* 로컬 함수 */
protected:

/* 공용 함수 */
public:


/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnNcCalcSize(BOOL calc_valid_rect, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg VOID		OnNcPaint();
	afx_msg HBRUSH		CtlColor(CDC* dc, UINT32 color);

};
