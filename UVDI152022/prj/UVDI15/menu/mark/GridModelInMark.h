#pragma once

#include "../../../../inc/grid/GridCtrl.h"

#include "GridBase.h"

class CGridModelInMark : public CGridBase
{
/* ������ & �ı��� */
public:

	CGridModelInMark(HWND parent, HWND grid, UINT32 g_id)
		: CGridBase(parent, grid, g_id)	{ }
	virtual ~CGridModelInMark()	{};

/* ���� �Լ� */
protected:

public:

	virtual VOID		Load()	{};


/* ���� ���� */
protected:


/* ���� �Լ� */
protected:


/* ���� �Լ� */
public:


/* ���� �Լ� */
public:

	VOID				UpdateModels(LPG_RAAF recipe);
	LPG_CMPV			GetRecipe(INT32 index);
};
