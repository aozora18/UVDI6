
/*
 desc : The Interface Library for VISITECH's Photohead Step Calibration
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
/*                                  외부 함수 - PH Offset (Step)                                 */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 광학계 단차 (Photohead Offset) 레시피 적재
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_PhStep_RecipeLoad();
/*
 desc : 광학계 단차 (Photohead Offset) 레시피 저장
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_PhStep_RecipeSave();
/*
 desc : 광학계 단차 (Photohead Offset) 레시피 추가
 parm : step	- [in]  추가될 단차 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_PhStep_RecipeAppend(LPG_OSSD step);
/*
 desc : 광학계 단차 (Photohead Offset) 레시피 수정
 parm : step	- [in]  수정될 단차 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_PhStep_RecipeModifiy(LPG_OSSD step);
/*
 desc : 레시피 정보 검색
 parm : frame_rate	- [in]  검색하고자 하는 스테이지의 동작 속도 즉, 노광 속도
 retn : TRUE (Finded) or FALSE (Not find)
*/
API_IMPORT BOOL uvEng_PhStep_RecipeFind(UINT16 frame_rate);
/*
 desc : Recipe Delete
 parm : frame_rate	- [in]  제거하려는 스테이지의 이동 속도 즉, 노광 속도
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_PhStep_RecipeDelete(UINT16 frame_rate);
/*
 desc : 레시피 이름을 통한 레시피 반환
 parm : frame_rate	- [in]  검색하고자 하는 스테이지의 동작 속도 즉, 노광 속도
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
API_IMPORT LPG_OSSD uvEng_PhStep_GetRecipeData(UINT16 frame_rate);
/*
 desc : 레시피 Index를 통한 레시피 반환
 parm : index	- [in]  Zero-base (config (ph_step)에서 최대 값보다 크면 안됨)
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
API_IMPORT LPG_OSSD uvEng_PhStep_GetRecipeIndex(UINT8 index);

#ifdef __cplusplus
}
#endif