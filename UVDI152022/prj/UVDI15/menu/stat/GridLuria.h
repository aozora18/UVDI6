#pragma once

#include "../../../../inc/grid/GridCtrl.h"

/* --------------------------------------------------------------------------------------------- */
/*                                       Base Grid Control                                       */
/* --------------------------------------------------------------------------------------------- */

class CGridLuria
{
/* ������ & �ı��� */
public:

	CGridLuria(HWND parent, HWND grid, UINT32 g_id, UINT32 count=0);
	virtual ~CGridLuria();

/* ���� �Լ� */
protected:
	virtual VOID		Load() = 0;
public:
	virtual VOID		Update(UINT8 index) = 0;


/* ���� ���� */
protected:

	BOOL				m_bScrollHorz;	/* ���� ��ũ�� ��� ���� */
	BOOL				m_bScrollVert;	/* ���� ��ũ�� ��� ���� */

	UINT32				m_u32Items;		/* �� (Item)�� ���� �� */
	UINT32				m_u32RowCnt;	/* �� (Item)�� ���� */
	UINT32				m_u32GridID;	/* Grid Control ID */
	UINT32				m_u32Style;

	CRect				m_rGrid;

	GV_ITEM				m_stGV;

	PTCHAR				*m_pszItem;

	CWnd				*m_pParent;
	CWnd				*m_pCtrl;
	CGridCtrl			*m_pGrid;


/* ���� �Լ� */
public:

	BOOL				Init();


/* ���� �Լ� */
public:

};

/* --------------------------------------------------------------------------------------------- */
/*                                         MC2 ���� ����                                         */
/* --------------------------------------------------------------------------------------------- */

class CGridLuriaMC2 : public CGridLuria
{
/* ������ & �ı��� */
public:

	CGridLuriaMC2(HWND parent, HWND grid, UINT32 g_id)
		: CGridLuria(parent, grid, g_id, 3)	{}
	virtual ~CGridLuriaMC2()	{};

/* ���� �Լ� */
protected:
	virtual VOID		Load();
public:
	virtual VOID		Update(UINT8 index=0);
};

/* --------------------------------------------------------------------------------------------- */
/*                                          PC ���� ����                                         */
/* --------------------------------------------------------------------------------------------- */

class CGridLuriaPC : public CGridLuria
{
/* ������ & �ı��� */
public:

	CGridLuriaPC(HWND parent, HWND grid, UINT32 g_id)
		: CGridLuria(parent, grid, g_id, 3)	{}
	virtual ~CGridLuriaPC()	{};

/* ���� �Լ� */
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
/* ������ & �ı��� */
public:

	CGridLuriaOverall(HWND parent, HWND grid, UINT32 g_id)
		: CGridLuria(parent, grid, g_id, 9)	{}
	virtual ~CGridLuriaOverall()	{};

/* ���� �Լ� */
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
/* ������ & �ı��� */
public:

	CGridLuriaLedSource(HWND parent, HWND grid, UINT32 g_id)
		: CGridLuria(parent, grid, g_id, 7)	{}
	virtual ~CGridLuriaLedSource()	{};

/* ���� �Լ� */
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
/* ������ & �ı��� */
public:

	CGridLuriaPHStat(HWND parent, HWND grid, UINT32 g_id)
		: CGridLuria(parent, grid, g_id, 3)	{}
	virtual ~CGridLuriaPHStat()	{};

/* ���� �Լ� */
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
/* ������ & �ı��� */
public:

	CGridLuriaJob(HWND parent, HWND grid, UINT32 g_id)
		: CGridLuria(parent, grid, g_id, 3)	{}
	virtual ~CGridLuriaJob()	{};

/* ���� �Լ� */
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
/* ������ & �ı��� */
public:

	CGridLuriaPHInfo(HWND parent, HWND grid, UINT32 g_id)
		: CGridLuria(parent, grid, g_id, 4)
	{
	}
	virtual ~CGridLuriaPHInfo()	{};

/* ���� �Լ� */
protected:
	virtual VOID		Load()	{};
public:
	virtual VOID		Update(UINT8 index=0);

/* ���� �Լ� */
public:

	BOOL				Init();
};

