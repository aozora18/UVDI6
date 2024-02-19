
/*
 desc : The Interface Library for Gerber Recipe
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
/*                                           외부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 등록된 레시피 개수 반환
 parm : None
 retn : 등록된 개수 반환
*/
API_EXPORT UINT32 uvEng_Recipe_GetRecipeCount()
{
	return g_pRecipe->GetRecipeCount();
}

/*
 desc : 레시피 이름을 통한 레시피 정보 반환
 parm : recipe	- [in]  검색하고자 하는 레시피 이름
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
API_EXPORT LPG_RIGP uvEng_Recipe_GetRecipeName(PTCHAR recipe)
{
	return g_pRecipe->GetRecipeName(recipe);
}

/*
 desc : 현재 선택된 레시피 기본 반환
 parm : None
 retn : 레시피가 저장된 구조체 포인터
*/
API_EXPORT LPG_RIGP uvEng_Recipe_GetSelectRecipe()
{
	if (!g_pRecipe)	return NULL;
	return g_pRecipe->GetSelectRecipe();
}

/*
 desc : 현재 선택된 레시피 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvEng_Recipe_ResetSelectRecipe()
{
	g_pRecipe->ResetSelectRecipe();
}

/*
 desc : Calibration Recipe 재-적재
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Recipe_ReloadFile()
{
	return g_pRecipe->RecipeLoad();
}

/*
 desc : 현재 선택된 레시피의 속성 정보를 통해 스테이지 이동 속도 및 광량 값 계산
 parm : recipe	- [out] 스테이지 이동 속도 및 광량 개별 및 평균 값 반환 (mm/sec)
 retn : None
*/
API_EXPORT VOID uvEng_Recipe_CalcSpeedEnergy(LPG_RIGP recipe)
{
	g_pRecipe->CalcSpeedEnergy(recipe);
}

/*
 desc : 레시피 기본 존재 여부
 parm : recipe	- [in]  검색하고자 하는 레시피 이름
 retn : TRUE (Finded) or FALSE (Not find)
*/
API_EXPORT BOOL uvEng_Recipe_RecipeFind(PTCHAR recipe)
{
	return g_pRecipe->RecipeFind(recipe);
}

/*
 desc : 레시피 기본 정보가 저장된 인덱스 위치에 있는 레시피 반환
 parm : index	- [in]  가져오고자 하는 레시피 인덱스 (Zero-based)
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
API_EXPORT LPG_RIGP uvEng_Recipe_GetRecipeIndex(INT32 index)
{
	return g_pRecipe->GetRecipeIndex(index);
}

/*
 desc : 레시피 기본 이름을 통한 레시피 기본 선택
 parm : recipe	- [in]  검색하고자 하는 레시피 기본 이름
 retn : TRUE (선택 성공) or FALSE (선택 실패)
*/
API_EXPORT BOOL uvEng_Recipe_SetRecipeName(PTCHAR recipe)
{
	return g_pRecipe->SetRecipeName(recipe);
}

/*
 desc : Recipe Save
 parm : recipe	- [in]  저장하고자 하는 레시피 이름
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Recipe_RecipeSave(PTCHAR recipe)
{
	return g_pRecipe->RecipeSave(recipe);
}

/*
 desc : Recipe 정보 추가 (Append)
 parm : recipe	- [in]  Recipe 정보가 저장된 구조체 포인터
		check	- [in]  값 유효성 체크 여부
 retn : TRUE or FALSE (개수 초과. 최대 15개)
*/
API_EXPORT BOOL uvEng_Recipe_RecipeAppend(LPG_RIGP recipe)
{
	return g_pRecipe->RecipeAppend(recipe);
}

/*
 desc : Recipe 수정
 parm : recipe	- [in]  Recipe 기본 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Recipe_RecipeModify(LPG_RIGP recipe)
{
	return g_pRecipe->RecipeModify(recipe);
}

/*
 desc : Recipe Modify
 parm : recipe	- [in]  제거하려는 레시피의 이름
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Recipe_RecipeDelete(PTCHAR recipe)
{
	return g_pRecipe->RecipeDelete(recipe);
}



#ifdef __cplusplus
}
#endif