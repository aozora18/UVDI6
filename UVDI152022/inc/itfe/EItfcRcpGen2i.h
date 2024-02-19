
/*
 desc : The Interface Library for Gerber Recipe
*/

#pragma once

#include "../conf/global.h"
#include "../conf/define.h"
#include "../conf/recipe_gen2i.h"


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
API_IMPORT UINT32 uvEng_Recipe_GetRecipeCount();
/*
 desc : 레시피 이름을 통한 레시피 정보 반환
 parm : recipe	- [in]  검색하고자 하는 레시피 이름
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
API_IMPORT LPG_RIGP uvEng_Recipe_GetRecipeName(PTCHAR recipe);
/*
 desc : 현재 선택된 레시피 기본 반환
 parm : None
 retn : 레시피가 저장된 구조체 포인터
*/
API_IMPORT LPG_RIGP uvEng_Recipe_GetSelectRecipe();
/*
 desc : 현재 선택된 레시피 초기화
 parm : None
 retn : Non
*/
API_IMPORT VOID uvEng_Recipe_ResetSelectRecipe();
/*
 desc : Calibration Recipe 재-적재
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Recipe_ReloadFile();
/*
 desc : 현재 선택된 레시피의 속성 정보를 통해 스테이지 이동 속도 및 광량 값 계산
 parm : recipe	- [out] 스테이지 이동 속도 및 광량 개별 및 평균 값 반환 (mm/sec)
 retn : None
*/
API_IMPORT VOID uvEng_Recipe_CalcSpeedEnergy(LPG_RIGP recipe);
/*
 desc : 레시피 기본 존재 여부
 parm : recipe	- [in]  검색하고자 하는 레시피 이름
 retn : TRUE (Finded) or FALSE (Not find)
*/
API_IMPORT BOOL uvEng_Recipe_RecipeFind(PTCHAR recipe);
/*
 desc : 레시피 기본 정보가 저장된 인덱스 위치에 있는 레시피 반환
 parm : index	- [in]  가져오고자 하는 레시피 인덱스 (Zero-based)
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
API_IMPORT LPG_RIGP uvEng_Recipe_GetRecipeIndex(INT32 index);
/*
 desc : 레시피 기본 이름을 통한 레시피 기본 선택
 parm : recipe	- [in]  검색하고자 하는 레시피 기본 이름
 retn : TRUE (선택 성공) or FALSE (선택 실패)
*/
API_IMPORT BOOL uvEng_Recipe_SetRecipeName(PTCHAR recipe);
/*
 desc : Recipe Save
 parm : recipe	- [in]  저장하고자 하는 레시피 이름
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Recipe_RecipeSave(PTCHAR recipe);
/*
 desc : Recipe 정보 추가 (Append)
 parm : recipe	- [in]  Recipe 정보가 저장된 구조체 포인터
		check	- [in]  값 유효성 체크 여부
 retn : TRUE or FALSE (개수 초과. 최대 15개)
*/
API_IMPORT BOOL uvEng_Recipe_RecipeAppend(LPG_RIGP recipe);
/*
 desc : Recipe 수정
 parm : recipe	- [in]  Recipe 기본 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Recipe_RecipeModify(LPG_RIGP recipe);
/*
 desc : Recipe 삭제
 parm : recipe	- [in]  제거하려는 레시피의 기본 이름
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Recipe_RecipeDelete(PTCHAR recipe);

#ifdef __cplusplus
}
#endif