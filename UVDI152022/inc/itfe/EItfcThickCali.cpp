
/*
 desc : The Interface Library for Trigger Calibration
*/

#include "pch.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/* --------------------------------------------------------------------------------------------- */
/*                                           내부 함수                                           */
/* --------------------------------------------------------------------------------------------- */


#ifdef __cplusplus
extern "C"
{
#endif

/* --------------------------------------------------------------------------------------------- */
/*                          외부 함수 - Material Thick Calibration File                          */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 등록된 레시피 개수 반환
 parm : None
 retn : 등록된 개수 반환
*/
API_EXPORT UINT32 uvEng_ThickCali_GetCount()
{
	return g_pThickCali->GetRecipeCount();
}

/*
 desc : Calibration Recipe 적재
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_ThickCali_LoadFile()
{
	return g_pThickCali->LoadFile();
}

/*
 desc : 레시피 속도 검색
 parm : thick	- [in]  검색하고자 하는 레시피 인덱스 (Thickness:100nm or 0.1um)
 retn : TRUE (Finded) or FALSE (Not find)
*/
API_EXPORT BOOL uvEng_ThickCali_RecipeFind(UINT32 thick)
{
	return g_pThickCali->RecipeFind(thick);
}

/*
 desc : 레시피 이름을 통한 레시피 반환
 parm : thick	- [in]  검색하고자 하는 레시피 인덱스 (Thickness:100nm or 0.1um)
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
API_EXPORT LPG_MACP uvEng_ThickCali_GetRecipe(UINT32 thick)
{
	return g_pThickCali->GetRecipe(thick);
}

/*
 desc : 레시피가 저장된 인덱스 위치에 있는 레시피 반환
 parm : index	- [in]  가져오고자 하는 레시피 인덱스 (Zero-based)
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
API_EXPORT LPG_MACP uvEng_ThickCali_GetRecipeIndex(UINT32 index)
{
	return g_pThickCali->GetRecipeIndex(index);
}

//221219 abh1000
/*
 desc : Recipe 기본 정보 수정
 parm : recipe	- [LPG_MACP]  Recipe 기본 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_ThickCali_RecipeModify(LPG_MACP recipe)
{
	return g_pThickCali->RecipeModify(recipe);
}
#ifdef __cplusplus
}
#endif