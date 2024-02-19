#pragma once

#include "../../../../inc/grid/GridCtrl.h"

/* --------------------------------------------------------------------------------------------- */
/*                                       Base Grid Control                                       */
/* --------------------------------------------------------------------------------------------- */

class CGridLuria
{
/* 생성자 & 파괴자 */
public:

	CGridLuria(HWND parent, HWND grid, UINT32 g_id, UINT32 count=0);
	virtual ~CGridLuria();

/* 가상 함수 */
protected:
	virtual VOID		Load() = 0;
public:
	virtual VOID		Update(UINT8 index) = 0;


/* 로컬 변수 */
protected:

	BOOL				m_bScrollHorz;	/* 수평 스크롤 출력 여부 */
	BOOL				m_bScrollVert;	/* 수직 스크롤 출력 여부 */

	UINT32				m_u32Items;		/* 행 (Item)의 글자 수 */
	UINT32				m_u32RowCnt;	/* 행 (Item)의 개수 */
	UINT32				m_u32GridID;	/* Grid Control ID */
	UINT32				m_u32Style;

	CRect				m_rGrid;

	GV_ITEM				m_stGV;

	PTCHAR				*m_pszItem;

	CWnd				*m_pParent;
	CWnd				*m_pCtrl;
	CGridCtrl			*m_pGrid;


/* 공용 함수 */
public:

	BOOL				Init();


/* 공용 함수 */
public:

};

/* --------------------------------------------------------------------------------------------- */
/*                                         MC2 연결 상태                                         */
/* --------------------------------------------------------------------------------------------- */

class CGridLuriaMC2 : public CGridLuria
{
/* 생성자 & 파괴자 */
public:

	CGridLuriaMC2(HWND parent, HWND grid, UINT32 g_id)
		: CGridLuria(parent, grid, g_id, 3)	{}
	virtual ~CGridLuriaMC2()	{};

/* 가상 함수 */
protected:
	virtual VOID		Load();
public:
	virtual VOID		Update(UINT8 index=0);
};

/* --------------------------------------------------------------------------------------------- */
/*                                          PC 연결 상태                                         */
/* --------------------------------------------------------------------------------------------- */

class CGridLuriaPC : public CGridLuria
{
/* 생성자 & 파괴자 */
public:

	CGridLuriaPC(HWND parent, HWND grid, UINT32 g_id)
		: CGridLuria(parent, grid, g_id, 3)	{}
	virtual ~CGridLuriaPC()	{};

/* 가상 함수 */
protected:
	virtual VOID		Load();
public:
	virtual VOID		Update(UINT8 index=0);
};

/* --------------------------------------------------------------------------------------------- */
/*                                  Overall Error Status Multi                                   */
/* --------------------------------------------------------------------------------------------- */

class CGridLuriaOverall : public CGridLuria
{
/* 생성자 & 파괴자 */
public:

	CGridLuriaOverall(HWND parent, HWND grid, UINT32 g_id)
		: CGridLuria(parent, grid, g_id, 9)	{}
	virtual ~CGridLuriaOverall()	{};

/* 가상 함수 */
protected:
	virtual VOID		Load();
public:
	virtual VOID		Update(UINT8 index=0);
};

/* --------------------------------------------------------------------------------------------- */
/*                                   Light Source Status Multi                                   */
/* --------------------------------------------------------------------------------------------- */

class CGridLuriaLedSource : public CGridLuria
{
/* 생성자 & 파괴자 */
public:

	CGridLuriaLedSource(HWND parent, HWND grid, UINT32 g_id)
		: CGridLuria(parent, grid, g_id, 7)	{}
	virtual ~CGridLuriaLedSource()	{};

/* 가상 함수 */
protected:
	virtual VOID		Load();
public:
	virtual VOID		Update(UINT8 index=0);
};

/* --------------------------------------------------------------------------------------------- */
/*                                 Photohead Init / Comm. Status                                 */
/* --------------------------------------------------------------------------------------------- */

class CGridLuriaPHStat : public CGridLuria
{
/* 생성자 & 파괴자 */
public:

	CGridLuriaPHStat(HWND parent, HWND grid, UINT32 g_id)
		: CGridLuria(parent, grid, g_id, 3)	{}
	virtual ~CGridLuriaPHStat()	{};

/* 가상 함수 */
protected:
	virtual VOID		Load();
public:
	virtual VOID		Update(UINT8 index=0);
};

/* --------------------------------------------------------------------------------------------- */
/*                                Job Load / Mark Regist / Expose                                */
/* --------------------------------------------------------------------------------------------- */

class CGridLuriaJob : public CGridLuria
{
/* 생성자 & 파괴자 */
public:

	CGridLuriaJob(HWND parent, HWND grid, UINT32 g_id)
		: CGridLuria(parent, grid, g_id, 3)	{}
	virtual ~CGridLuriaJob()	{};

/* 가상 함수 */
protected:
	virtual VOID		Load();
public:
	virtual VOID		Update(UINT8 index=0);
};

/* --------------------------------------------------------------------------------------------- */
/*                  PH Information (LED & Board Temp, LED Life Time, LED ON/OFF                  */
/* --------------------------------------------------------------------------------------------- */

class CGridLuriaPHInfo : public CGridLuria
{
/* 생성자 & 파괴자 */
public:

	CGridLuriaPHInfo(HWND parent, HWND grid, UINT32 g_id)
		: CGridLuria(parent, grid, g_id, 4)
	{
	}
	virtual ~CGridLuriaPHInfo()	{};

/* 가상 함수 */
protected:
	virtual VOID		Load()	{};
public:
	virtual VOID		Update(UINT8 index=0);

/* 공용 함수 */
public:

	BOOL				Init();
};

