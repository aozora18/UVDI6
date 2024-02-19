
#pragma once

#include "Base.h"

class CPhStep : public CBase
{
/* ������ & �ı��� */
public:

	CPhStep(PTCHAR work_dir, LPG_OSSD shmem);
	virtual ~CPhStep();


/* ���� ���� */
protected:

	UINT32				m_u32RecipeNext;	/* ���� �����Ǹ� ���������� ���� ��ġ �� (Zero-based) */

	LPG_OSSD			m_pstShMemRecipe;	/* �����ǰ� ����� ���� ��ġ (����ü ������) */
	LPG_OSSD			m_pstSelRecipe;		/* ���� ���õ� ������ */


/* ���� �Լ� */
protected:

	BOOL				ParseAppend(UINT32 index, CHAR *data, UINT32 size);
	BOOL				ResetRecipe(UINT16 frame_rate);
	VOID				ResetRecipeAll();


/* ���� �Լ� */
public:

	LPG_OSSD			GetSelectRecipe()	{	return m_pstSelRecipe;	}

	BOOL				LoadFile();
	BOOL				SaveFile();

	BOOL				RecipeAppend(LPG_OSSD step);
	BOOL				RecipeModify(LPG_OSSD step);
	BOOL				RecipeDelete(UINT16 frame_rate);
	BOOL				RecipeFind(UINT16 frame_rate);
	BOOL				SetRecipeData(UINT16 frame_rate);
	LPG_OSSD			GetRecipeData(UINT16 frame_rate);
	LPG_OSSD			GetRecipeIndex(UINT32 index);
	INT32				GetRecipeStart();
	INT32				GetRecipeNext();
	VOID				ResetRecipe()		{	m_pstSelRecipe	= NULL;	};
	UINT32				GetRecipeCount();
};
