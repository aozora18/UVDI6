#pragma once

#include "../../../../inc/grid/GridCtrl.h"

/* --------------------------------------------------------------------------------------------- */
/*                                       Base Grid Control                                       */
/* --------------------------------------------------------------------------------------------- */

class CGridTempDI
{
/* ������ & �ı��� */
public:

	CGridTempDI(HWND parent, HWND grid);
	virtual ~CGridTempDI();

/* ���� ���� */
protected:

	CRect				m_rGrid;

	CWnd				*m_pParent;
	CWnd				*m_pCtrl;
	CGridCtrl			*m_pGrid;

	CAtlList <DOUBLE>	m_lstTemp[4];


/* ���� �Լ� */
public:


/* ���� �Լ� */
public:

	BOOL				Init();
	VOID				Update();
};
