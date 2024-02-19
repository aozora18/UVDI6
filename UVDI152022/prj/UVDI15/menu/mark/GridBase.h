#pragma once

#include "../../../../inc/grid/GridCtrl.h"


/* --------------------------------------------------------------------------------------------- */
/*                                       Base Grid Control                                       */
/* --------------------------------------------------------------------------------------------- */

class CGridBase
{
/* ������ & �ı��� */
public:

	CGridBase(HWND parent, HWND grid, UINT32 g_id)
	{
		m_pGrid			= NULL;
		m_pParent		= CWnd::FromHandle(parent);
		m_pCtrl			= CWnd::FromHandle(grid);
		m_u32GridID		= g_id;

		/* �׸��� ��Ʈ�� �⺻ ���� �Ӽ� */
		m_u32Style		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
		m_stGV.mask		= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
		m_stGV.nFormat	= DT_LEFT | DT_VCENTER | DT_SINGLELINE;
		m_stGV.crFgClr	= RGB(32, 32, 32);
		m_stGV.crBkClr	= RGB(255, 255, 255);
		m_u32Height		= 23;
	}
	virtual ~CGridBase();

/* ���� �Լ� */
protected:

public:

	virtual VOID		Load() = 0;


/* ���� ���� */
protected:

	UINT32				m_u32Height;	/* �׷� �ڽ��� ���� ���� �� */
	UINT32				m_u32GridID;	/* Grid Control ID */
	UINT32				m_u32Style;

	CRect				m_rGrid;

	GV_ITEM				m_stGV;

	CWnd				*m_pParent;
	CWnd				*m_pCtrl;
	CGridCtrl			*m_pGrid;


/* ���� �Լ� */
protected:


/* ���� �Լ� */
public:

	BOOL				Init(BOOL horz=FALSE, BOOL vert=FALSE, BOOL btn=FALSE);
	VOID				Select(PTCHAR recipe);


/* ���� �Լ� */
public:

};
