
/*
 desc : The Interface Library for Trigger Calibration
*/

#pragma once

#include "../conf/global.h"
#include "../conf/define.h"
#if (CUSTOM_CODE_UVDI15_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_UVDI15_LLS06 == DELIVERY_PRODUCT_ID)
#include "../../inc/conf/recipe_uvdi15.h"
#elif (CUSTOM_CODE_GEN2I == DELIVERY_PRODUCT_ID)
#elif (CUSTOM_CODE_UVDI15 == DELIVERY_PRODUCT_ID || \
CUSTOM_CODE_HDDI6== DELIVERY_PRODUCT_ID)
#include "../../inc/conf/recipe_uvdi15.h"

#endif


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
API_IMPORT UINT32 uvEng_ThickCali_GetCount();
/*
 desc : Calibration Recipe 적재
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_ThickCali_LoadFile();
/*
 desc : 레시피 속도 검색
 parm : thick	- [in]  검색하고자 하는 레시피 인덱스 (Thickness:100nm or 0.1um)
 retn : TRUE (Finded) or FALSE (Not find)
*/
API_IMPORT BOOL uvEng_ThickCali_RecipeFind(UINT32 thick);
/*
 desc : 레시피 이름을 통한 레시피 반환
 parm : thick	- [in]  검색하고자 하는 레시피 인덱스 (Thickness:100nm or 0.1um)
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
API_IMPORT LPG_MACP uvEng_ThickCali_GetRecipe(UINT32 thick);
/*
 desc : 레시피가 저장된 인덱스 위치에 있는 레시피 반환
 parm : index	- [in]  가져오고자 하는 레시피 인덱스 (Zero-based)
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
API_IMPORT LPG_MACP uvEng_ThickCali_GetRecipeIndex(UINT32 index);
//221219 abh1000
/*
 desc : Recipe 기본 정보 수정
 parm : recipe	- [LPG_MACP]  Recipe 기본 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_ThickCali_RecipeModify(LPG_MACP recipe);

#ifdef __cplusplus
}
#endif