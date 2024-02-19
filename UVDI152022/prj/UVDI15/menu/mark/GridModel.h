#pragma once

#include "../../../../inc/grid/GridCtrl.h"

#include "GridBase.h"

class CGridModel : public CGridBase
{
/* 생성자 & 파괴자 */
public:

	CGridModel(HWND parent, HWND grid, UINT32 g_id)
		: CGridBase(parent, grid, g_id)
	{
		m_u32Height	= 55;
	}
	virtual ~CGridModel()	{};

/* 가상 함수 */
protected:

public:

	virtual VOID		Load();


/* 로컬 변수 */
protected:


/* 로컬 함수 */
protected:


/* 공용 함수 */
public:


/* 공용 함수 */
public:

	LPG_CMPV			GetSelected();
	LPG_CMPV			GetRecipe(INT32 index);
};
