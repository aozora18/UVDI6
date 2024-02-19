#pragma once

#include "../../../../inc/grid/GridCtrl.h"


/* --------------------------------------------------------------------------------------------- */
/*                                       Base Grid Control                                       */
/* --------------------------------------------------------------------------------------------- */

class CGridMiniMotor
{
/* 생성자 & 파괴자 */
public:

	CGridMiniMotor(HWND parent, HWND grid);
	virtual ~CGridMiniMotor();

/* 가상 함수 */
protected:

public:


/* 로컬 변수 */
protected:

	CWnd				*m_pParent;
	CWnd				*m_pCtrl;
	CGridCtrl			*m_pGrid;


/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	VOID				InitGrid();
	VOID				UpdateValue();
};
