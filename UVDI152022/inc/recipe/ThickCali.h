
#pragma once

#include "Base.h"

class CThickCali : public CBase
{
/* 생성자 & 파괴자 */
public:

	CThickCali(PTCHAR work_dir);
	virtual ~CThickCali();


/* 로컬 변수 */
protected:

	CAtlList <LPG_MACP>	m_lstRecipe;


/* 로컬 함수 */
protected:

	BOOL				ParseAppend(CHAR *data, UINT32 size);
	VOID				RemoveAll();


/* 공용 함수 */
public:

	BOOL				LoadFile();
	BOOL				RecipeFind(UINT32 thick);
	LPG_MACP			GetRecipe(UINT32 thick);
	LPG_MACP			GetRecipeIndex(UINT32 index);

	UINT32				GetRecipeCount()	{	return (UINT32)m_lstRecipe.GetCount();	}

	//221219 abh1000
	BOOL				SaveFile();
	BOOL				RecipeModify(LPG_MACP step);
};
