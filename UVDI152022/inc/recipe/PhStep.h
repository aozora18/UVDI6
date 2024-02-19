
#pragma once

#include "Base.h"

class CPhStep : public CBase
{
/* 생성자 & 파괴자 */
public:

	CPhStep(PTCHAR work_dir, LPG_OSSD shmem);
	virtual ~CPhStep();


/* 로컬 변수 */
protected:

	UINT32				m_u32RecipeNext;	/* 현재 레시피를 가져오고자 시작 위치 값 (Zero-based) */

	LPG_OSSD			m_pstShMemRecipe;	/* 레시피가 저장될 시작 위치 (구조체 포인터) */
	LPG_OSSD			m_pstSelRecipe;		/* 현재 선택된 레시피 */


/* 로컬 함수 */
protected:

	BOOL				ParseAppend(UINT32 index, CHAR *data, UINT32 size);
	BOOL				ResetRecipe(UINT16 frame_rate);
	VOID				ResetRecipeAll();


/* 공용 함수 */
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
