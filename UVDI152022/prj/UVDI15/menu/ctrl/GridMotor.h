#pragma once

#include "../../../../inc/grid/GridCtrl.h"


/* --------------------------------------------------------------------------------------------- */
/*                                       Base Grid Control                                       */
/* --------------------------------------------------------------------------------------------- */

class CGridMotor
{
/* ������ & �ı��� */
public:

	CGridMotor(HWND parent, HWND grid);
	virtual ~CGridMotor();

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
