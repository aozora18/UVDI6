#pragma once

#include "../../../../inc/grid/GridCtrl.h"

/* --------------------------------------------------------------------------------------------- */
/*                                       Base Grid Control                                       */
/* --------------------------------------------------------------------------------------------- */

class CGridDrive
{
/* ������ & �ı��� */
public:

	CGridDrive(HWND parent, HWND grid, UINT32 g_id, UINT32 rows, UINT32 cols);
	virtual ~CGridDrive();

/* ���� �Լ� */
protected:
	virtual VOID		Load() = 0;
public:
	virtual VOID		Update() = 0;


/* ���� ���� */
protected:

	UINT32				m_u32Rows;		/* �� (Item)�� ���� */
	UINT32				m_u32Cols;		/* �� (Item)�� ���� */
	UINT32				m_u32GridID;	/* Grid Control ID */
	UINT32				m_u32Style;

	CRect				m_rGrid;

	GV_ITEM				m_stGV;

	CWnd				*m_pParent;
	CWnd				*m_pCtrl;
	CGridCtrl			*m_pGrid;


/* ���� �Լ� */
public:

	BOOL				Init(BOOL vert=TRUE);


/* ���� �Լ� */
public:

};

/* --------------------------------------------------------------------------------------------- */
/*                                       MC2 Drive Status                                        */
/* --------------------------------------------------------------------------------------------- */

class CGridDriveStat : public CGridDrive
{
/* ������ & �ı��� */
public:

	CGridDriveStat(HWND parent, HWND grid, UINT32 g_id)
		: CGridDrive(parent, grid, g_id, 5, 11)	{}
	virtual ~CGridDriveStat()	{};

/* ���� �Լ� */
protected:
	virtual VOID		Load();
public:
	virtual VOID		Update();
};

/* --------------------------------------------------------------------------------------------- */
/*                                     SD2 Drive Error Message                                   */
/* --------------------------------------------------------------------------------------------- */

class CGridSD2Error : public CGridDrive
{
/* ������ & �ı��� */
public:

	CGridSD2Error(HWND parent, HWND grid, UINT32 g_id)
		: CGridDrive(parent, grid, g_id, 49, 2)	{}
	virtual ~CGridSD2Error()	{};

/* ���� �Լ� */
protected:
	virtual VOID		Load();
public:
	virtual VOID		Update();
};

/* --------------------------------------------------------------------------------------------- */
/*                                     MC2 Drive Error Message                                   */
/* --------------------------------------------------------------------------------------------- */

class CGridMC2Error : public CGridDrive
{
/* ������ & �ı��� */
public:

	CGridMC2Error(HWND parent, HWND grid, UINT32 g_id)
		: CGridDrive(parent, grid, g_id, 30, 2)	{}
	virtual ~CGridMC2Error()	{};

/* ���� �Լ� */
protected:
	virtual VOID		Load();
public:
	virtual VOID		Update();
};
