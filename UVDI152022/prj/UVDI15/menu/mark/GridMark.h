#pragma once

#include "../../../../inc/grid/GridCtrl.h"

#include "GridBase.h"

class CGridMark : public CGridBase
{
/* ������ & �ı��� */
public:

	CGridMark(HWND parent, HWND grid, UINT32 g_id)
		: CGridBase(parent, grid, g_id)
	{
		m_u32Height	= 55;
	}
	virtual ~CGridMark()	{};

/* ���� �Լ� */
protected:

public:

	virtual VOID		Load();


/* ���� ���� */
protected:


/* ���� �Լ� */
protected:


/* ���� �Լ� */
public:


/* ���� �Լ� */
public:

	LPG_RAAF			GetSelected();
	LPG_RAAF			GetRecipe(INT32 index);
};
