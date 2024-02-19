
#pragma once

#include "Base.h"

class CCorrectY : public CBase
{
/* ������ & �ı��� */
public:

	CCorrectY(PTCHAR work_dir, LPG_OSCY shmem);
	virtual ~CCorrectY();


/* ���� ���� */
protected:

	UINT32				m_u32RecipeNext;	/* ���� �����Ǹ� ���������� ���� ��ġ �� (Zero-based) */

	LPG_OSCY			m_pstShMemRecipe;	/* �����ǰ� ����� ���� ��ġ (����ü ������) */
	LPG_OSCY			m_pstSelRecipe;		/* ���� ���õ� ������ */


/* ���� �Լ� */
protected:

	BOOL				ParseAppend(UINT32 index, CHAR *data, UINT32 size);
	BOOL				ResetRecipe(UINT16 frame_rate);
	VOID				ResetRecipeAll();


/* ���� �Լ� */
public:

	LPG_OSCY			GetSelectRecipe()	{	return m_pstSelRecipe;	}

	BOOL				LoadFile();
	BOOL				SaveFile();

	BOOL				RecipeAppend(LPG_OSCY adjust);
	BOOL				RecipeModify(LPG_OSCY adjust);
	BOOL				RecipeDelete(UINT16 frame_rate);
	BOOL				RecipeFind(UINT16 frame_rate);
	BOOL				SetRecipeData(UINT16 frame_rate);
	LPG_OSCY			GetRecipeData(UINT16 frame_rate);
	LPG_OSCY			GetRecipeIndex(UINT32 index);
	INT32				GetRecipeStart();
	INT32				GetRecipeNext();
	VOID				ResetRecipe()		{	m_pstSelRecipe	= NULL;	};
	UINT32				GetRecipeCount();
};
