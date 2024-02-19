#pragma once

#include "../../../../inc/grid/GridCtrl.h"


/* --------------------------------------------------------------------------------------------- */
/*                                       Base Grid Control                                       */
/* --------------------------------------------------------------------------------------------- */

class CGridMiniMotor
{
/* ������ & �ı��� */
public:

	CGridMiniMotor(HWND parent, HWND grid);
	virtual ~CGridMiniMotor();

/* ���� �Լ� */
protected:

public:


/* ���� ���� */
protected:

	CWnd				*m_pParent;
	CWnd				*m_pCtrl;
	CGridCtrl			*m_pGrid;


/* ���� �Լ� */
protected:


/* ���� �Լ� */
public:

	VOID				InitGrid();
	VOID				UpdateValue();
};
