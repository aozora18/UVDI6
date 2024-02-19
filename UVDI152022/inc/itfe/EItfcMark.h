
/*
 desc : The Interface Library for Mark Model & Recipe
*/

#pragma once

#include "../conf/global.h"
#include "../conf/define.h"
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
#include "../../inc/conf/vision_podis.h"
#elif (CUSTOM_CODE_GEN2I == DELIVERY_PRODUCT_ID)
#endif



#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : Model 적재
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Mark_LoadFile();
/*
 desc : Model Save
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Mark_SaveFile();
/*
 desc : ROI Save
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Mark_MarkROISave();
/*
 desc : ROI Set
 parm : data	- [in]
		index	- [in] 1st, 2nd ROI
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Mark_SetMarkROI(LPG_CRD data, UINT8 index);
/*
 desc : Model 이름 검색
 parm : name	- [in]  검색하고자 하는 Model 이름
 retn : TRUE (Finded) or FALSE (Not find)
*/
API_IMPORT BOOL uvEng_Mark_ModelFind(PTCHAR name);
/*
 desc : Model 이름을 통한 Model 반환
 parm : name	- [in]  검색하고자 하는 Model 이름
 retn : 반환될 Model이 저장된 구조체 포인터
*/
API_IMPORT LPG_CMPV uvEng_Mark_GetModelName(PTCHAR name);
/*
 desc : Recipe 이름을 통한 Recipe 반환
 parm : name	- [in]  검색하고자 하는 Model 이름
 retn : 반환될 Model이 저장된 구조체 포인터
*/
API_IMPORT LPG_CMRD uvEng_Mark_GetRecipeName(PTCHAR name);
/*
 desc : Model가 저장된 인덱스 위치에 있는 Model 반환
 parm : index	- [in]  가져오고자 하는 Model 인덱스 (Zero-based)
 retn : 반환될 Model이 저장된 구조체 포인터
*/
API_IMPORT LPG_CMPV uvEng_Mark_GetModelIndex(UINT8 index);
/*
 desc : Recipe가 저장된 인덱스 위치에 있는 Model 반환
 parm : index	- [in]  가져오고자 하는 Model 인덱스 (Zero-based)
 retn : 반환될 Recipe가 저장된 구조체 포인터
*/
API_IMPORT LPG_CMRD uvEng_Mark_GetRecipeIndex(UINT8 index);
/*
 desc : Model Append
 parm : value	- [in]  Model 정보가 저장된 구조체 포인터
		mode	- [in]  0x00 - Append, 0x01 - Modify
 retn : TRUE or FALSE (개수 초과. 최대 15개)
*/
API_IMPORT BOOL uvEng_Mark_ModelAppend(LPG_CMPV value, UINT8 mode);
/*
 desc : Recipe Append
 parm : data	- [in]  Recipe 정보가 저장된 구조체 포인터
		mode	- [in]  0x00 - Append, 0x01 - Modify
 retn : TRUE or FALSE (개수 초과. 최대 15개)
*/
API_IMPORT BOOL uvEng_Mark_MarkAppend(LPG_CMRD data, UINT8 mode);
/*
 desc : Model 이름 검색를 통한 삭제
 parm : name	- [in]  삭제하고자 하는 Model 이름
 retn : TRUE (Deleted) or FALSE (Not delete)
*/
API_IMPORT BOOL uvEng_Mark_ModelDelete(PTCHAR name);
/*
 desc : Recipe 이름 검색를 통한 삭제
 parm : name	- [in]  삭제하고자 하는 Recipe 이름
 retn : TRUE (Deleted) or FALSE (Not delete)
*/
API_IMPORT BOOL uvEng_Mark_MarkDelete(PTCHAR name);
/*
 desc : 현재 등록된 모델 개수 반환
 parm : None
 retn : 개수
*/
API_IMPORT UINT32 uvEng_Mark_GetModelCount();
/*
 desc : 현재 등록된 레시피 개수 반환
 parm : None
 retn : 개수
*/
API_IMPORT UINT32 uvEng_Mark_GetRecipeCount();
/*
 desc : 모델 타입에 따른 문자열 반환
 parm : type	- [in]  모델 타입 코드 값
 retn : 문자열 반환
*/
API_IMPORT TCHAR *uvEng_Mark_GetModelTypeToStr(UINT32 type);
/*
 desc : 현재 선택된 레시피 정보 반환
 parm : None
 retn : 현재 선택된 레시피 구조체 포인터
*/
API_IMPORT LPG_CMRD uvEng_Mark_GetSelectRecipe();
/*
 desc : 현재 선택된 레시피 초기화
 parm : None
 retn : None
*/
API_IMPORT VOID uvEng_Mark_SelRecipeReset();
/*
 desc : 레시피 이름을 통한 레시피 선택
 parm : r_name	- [in]  Mark 레시피 이름
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Mark_SelRecipeName(PTCHAR r_name);

#ifdef __cplusplus
}
#endif