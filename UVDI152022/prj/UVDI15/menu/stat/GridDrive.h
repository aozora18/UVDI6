#pragma once

#include "../../../../inc/grid/GridCtrl.h"

/* --------------------------------------------------------------------------------------------- */
/*                                       Base Grid Control                                       */
/* --------------------------------------------------------------------------------------------- */

class CGridDrive
{
/* 생성자 & 파괴자 */
public:

	CGridDrive(HWND parent, HWND grid, UINT32 g_id, UINT32 rows, UINT32 cols);
	virtual ~CGridDrive();

/* 가상 함수 */
protected:
	virtual VOID		Load() = 0;
public:
	virtual VOID		Update() = 0;


/* 로컬 변수 */
protected:

	UINT32				m_u32Rows;		/* 행 (Item)의 개수 */
	UINT32				m_u32Cols;		/* 열 (Item)의 개수 */
	UINT32				m_u32GridID;	/* Grid Control ID */
	UINT32				m_u32Style;

	CRect				m_rGrid;

	GV_ITEM				m_stGV;

	CWnd				*m_pParent;
	CWnd				*m_pCtrl;
	CGridCtrl			*m_pGrid;


/* 공용 함수 */
public:

	BOOL				Init(BOOL vert=TRUE);


/* 공용 함수 */
public:

};

/* --------------------------------------------------------------------------------------------- */
/*                                       MC2 Drive Status                                        */
/* --------------------------------------------------------------------------------------------- */

class CGridDriveStat : public CGridDrive
{
/* 생성자 & 파괴자 */
public:

	CGridDriveStat(HWND parent, HWND grid, UINT32 g_id)
		: CGridDrive(parent, grid, g_id, 5, 11)	{}
	virtual ~CGridDriveStat()	{};

/* 가상 함수 */
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
/* 생성자 & 파괴자 */
public:

	CGridSD2Error(HWND parent, HWND grid, UINT32 g_id)
		: CGridDrive(parent, grid, g_id, 49, 2)	{}
	virtual ~CGridSD2Error()	{};

/* 가상 함수 */
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
/* 생성자 & 파괴자 */
public:

	CGridMC2Error(HWND parent, HWND grid, UINT32 g_id)
		: CGridDrive(parent, grid, g_id, 30, 2)	{}
	virtual ~CGridMC2Error()	{};

/* 가상 함수 */
protected:
	virtual VOID		Load();
public:
	virtual VOID		Update();
};
