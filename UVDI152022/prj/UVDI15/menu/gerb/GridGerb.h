#pragma once

#include "../../../../inc/grid/GridCtrl.h"


/* --------------------------------------------------------------------------------------------- */
/*                                       Base Grid Control                                       */
/* --------------------------------------------------------------------------------------------- */

class CGridGerb
{
/* ������ & �ı��� */
public:

	CGridGerb(HWND parent, HWND grid)
	{
		m_pGrid			= NULL;
		m_pParent		= CWnd::FromHandle(parent);
		m_pCtrl			= CWnd::FromHandle(grid);

		/* �׸��� ��Ʈ�� �⺻ ���� �Ӽ� */
		m_u32Style		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
		m_stGV.mask		= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
		m_stGV.nFormat	= DT_LEFT | DT_VCENTER | DT_SINGLELINE;
		m_stGV.crFgClr	= RGB(32, 32, 32);
		m_stGV.crBkClr	= RGB(255, 255, 255);
	}
	virtual ~CGridGerb();

/* ���� �Լ� */
protected:

public:

	virtual BOOL		Init() = 0;
	virtual VOID		Load() = 0;

/* ���� ���� */
protected:

	UINT32				m_u32Style;

	CRect				m_rGrid;

	GV_ITEM				m_stGV;

	CWnd				*m_pParent;
	CWnd				*m_pCtrl;
	CGridCtrl			*m_pGrid;


/* ���� �Լ� */
protected:

	VOID				InitGrid();


/* ���� �Լ� */
public:

};

/* --------------------------------------------------------------------------------------------- */
/*                                         Gerber Recipe                                         */
/* --------------------------------------------------------------------------------------------- */

class CGridRecipe : public CGridGerb
{
/* ������ & �ı��� */
public:

	CGridRecipe(HWND parent, HWND grid)
		: CGridGerb(parent, grid)	{ }
	virtual ~CGridRecipe()	{};

/* ���� �Լ� */
protected:

public:
	virtual BOOL		Init();
	virtual VOID		Load();

/* ���� ���� */
protected:


/* ���� �Լ� */
protected:


/* ���� �Լ� */
public:

	LPG_RJAF			GetSelected();
	LPG_RJAF			GetRecipe(INT32 index);
	VOID				Select(PTCHAR recipe);
};

/* --------------------------------------------------------------------------------------------- */
/*                                LED Power Grid Control - Items                                 */
/* --------------------------------------------------------------------------------------------- */

class CGridPowerI : public CGridGerb
{
/* ������ & �ı��� */
public:

	CGridPowerI(HWND parent, HWND grid)
		: CGridGerb(parent, grid)	{ }
	virtual ~CGridPowerI()	{};

/* ���� �Լ� */
protected:

public:

	virtual BOOL		Init();
	virtual VOID		Load();

/* ���� �Լ� */
public:

	LPG_PLPI			GetSelect(INT32 index);
	LPG_PLPI			GetSelected();
	VOID				Select(PTCHAR recipe);
};

/* --------------------------------------------------------------------------------------------- */
/*                                 LED Power Grid Control - Values                               */
/* --------------------------------------------------------------------------------------------- */

class CGridPowerV : public CGridGerb
{
/* ������ & �ı��� */
public:

	CGridPowerV(HWND parent, HWND grid)
		: CGridGerb(parent, grid)	{ }
	virtual ~CGridPowerV()	{};

/* ���� �Լ� */
protected:

public:

	virtual BOOL		Init();
	virtual VOID		Load()	{};


/* ���� �Լ� */
public:

	VOID				SetRecipe(LPG_PLPI recipe);
	DOUBLE				GetWatt(UINT8 row, UINT8 col);
};

/* --------------------------------------------------------------------------------------------- */
/*                          Material Thick Calibration for Align Camera                          */
/* --------------------------------------------------------------------------------------------- */

class CGridThick : public CGridGerb
{
/* ������ & �ı��� */
public:

	CGridThick(HWND parent, HWND grid)
		: CGridGerb(parent, grid)	{ }
	virtual ~CGridThick()	{};

/* ���� �Լ� */
protected:

public:

	virtual BOOL		Init();
	virtual VOID		Load();

/* ���� �Լ� */
public:

	VOID				Select(UINT32 thick);
	UINT32				GetSelectThick();
};
