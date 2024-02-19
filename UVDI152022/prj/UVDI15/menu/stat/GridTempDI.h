#pragma once

#include "../../../../inc/grid/GridCtrl.h"

/* --------------------------------------------------------------------------------------------- */
/*                                       Base Grid Control                                       */
/* --------------------------------------------------------------------------------------------- */

class CGridTempDI
{
/* 생성자 & 파괴자 */
public:

	CGridTempDI(HWND parent, HWND grid);
	virtual ~CGridTempDI();

/* 로컬 변수 */
protected:

	CRect				m_rGrid;

	CWnd				*m_pParent;
	CWnd				*m_pCtrl;
	CGridCtrl			*m_pGrid;

	CAtlList <DOUBLE>	m_lstTemp[4];


/* 공용 함수 */
public:


/* 공용 함수 */
public:

	BOOL				Init();
	VOID				Update();
};
