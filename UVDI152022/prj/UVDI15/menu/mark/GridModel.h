#pragma once

#include "../../../../inc/grid/GridCtrl.h"

#include "GridBase.h"

class CGridModel : public CGridBase
{
/* ������ & �ı��� */
public:

	CGridModel(HWND parent, HWND grid, UINT32 g_id)
		: CGridBase(parent, grid, g_id)
	{
		m_u32Height	= 55;
	}
	virtual ~CGridModel()	{};

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

	LPG_CMPV			GetSelected();
	LPG_CMPV			GetRecipe(INT32 index);
};
