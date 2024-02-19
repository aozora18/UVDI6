
/*
 desc : The Interface Library for Gerber Recipe
*/

#pragma once

#include "../conf/global.h"
#include "../conf/define.h"
#include "../conf/recipe_podis.h"



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
API_IMPORT UINT32 uvEng_Recipe_GetCount();
/*
 desc : 현재 선택된 레시피 기본 반환
 parm : None
 retn : 레시피가 저장된 구조체 포인터
*/
API_IMPORT LPG_RBGF uvEng_Recipe_GetSelectRecipe();
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
 parm : attrib	- [out] 스테이지 이동 속도 및 광량 개별 및 평균 값 반환 (mm/sec)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Recipe_CalcSpeedEnergy(LPG_RBGF recipe);
/*
 desc : 레시피 기본 존재 여부
 parm : recipe	- [in]  검색하고자 하는 레시피 이름
 retn : TRUE (Finded) or FALSE (Not find)
*/
API_IMPORT BOOL uvEng_Recipe_RecipeFind(PTCHAR recipe);
/*
 desc : 레시피 이름을 통한 레시피 기본 정보 반환
 parm : recipe	- [in]  검색하고자 하는 레시피 이름
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
API_IMPORT LPG_RBGF uvEng_Recipe_GetRecipeOnlyName(PTCHAR recipe);
/*
 desc : 레시피 이름 (전체 경로 포함)을 통한 레시피 반환
 parm : path_name	- [in]  검색하고자 하는 거버 이름 (경로 포함)
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
API_IMPORT LPG_RBGF uvEng_Recipe_GetRecipePathName(PTCHAR name);
/*
 desc : 레시피 기본 정보가 저장된 인덱스 위치에 있는 레시피 반환
 parm : index	- [in]  가져오고자 하는 레시피 인덱스 (Zero-based)
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
API_IMPORT LPG_RBGF uvEng_Recipe_GetRecipeIndex(INT32 index);

API_IMPORT int uvEng_Recipe_GetSelectRecipeIndex();

/*
 desc : 레시피 기본 이름을 통한 레시피 기본 선택
 parm : recipe	- [in]  검색하고자 하는 레시피 기본 이름
 retn : TRUE (선택 성공) or FALSE (선택 실패)
*/
API_IMPORT BOOL uvEng_Recipe_SelRecipeOnlyName(PTCHAR recipe);
/*
 desc : 레시피 기본 이름 (전체 경로 포함)을 통한 레시피 기본 선택
 parm : recipe	- [in]  검색하고자 하는 레시피 기본 이름 (전체 경로 포함)
 retn : TRUE (선택 성공) or FALSE (선택 실패)
*/
API_IMPORT BOOL uvEng_Recipe_SelRecipePathName(PTCHAR recipe);
/*
 desc : Recipe 저장
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Recipe_SaveFile();
/*
 desc : Recipe 기본 정보 추가 (Append)
 parm : recipe	- [in]  Recipe 정보가 저장된 구조체 포인터
		check	- [in]  값 유효성 체크 여부
 retn : TRUE or FALSE (개수 초과. 최대 15개)
*/
API_IMPORT BOOL uvEng_Recipe_RecipeAppend(LPG_RBGF recipe);
/*
 desc : Recipe 기본 정보 수정
 parm : recipe	- [in]  Recipe 기본 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Recipe_RecipeModify(LPG_RBGF recipe);
/*
 desc : Recipe 기본 삭제
 parm : recipe	- [in]  제거하려는 레시피의 기본 이름
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Recipe_RecipeDelete(PTCHAR recipe);
/*
 desc : 현재 선택된 Recipe의 Mark 구성 방식이 Shared Type인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Recipe_IsRecipeSharedType();
/*
 desc : 현재 선택된 Recipe의 Mark 구성 방식이 Local Mark가 존재하는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Recipe_IsExistLocalMark();
/*
 desc : 가져오려는 위치의 Mark Index 값이 Camera 몇 번이고 몇 번째 이미지 정보인지 반환
 parm : mark	- [in]  Mark Index (Zero based)
		cam_id	- [out] Camera Number (1 or 2)
		img_id	- [out] Camera Grabbed Image Number (Zero based)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Recipe_GetLocalMarkToGrabNum(UINT8 mark, UINT8 &cam_id, UINT8 &img_id);
/*
 desc : 현재 Local Mark Index 값 값에 해당된 Scan 번호 값 반환
 parm : mark_id	- [in]  Local Mark Index 값 (0 or Later)
 retn : Scan Number (Zero Based)
*/
API_IMPORT UINT8 uvEng_Recipe_GetLocalMarkToScanNum(UINT8 mark_id);
/*
 desc : 현재 Camera Index와 Grabbed Image를 가지고 몇 번째 인덱스에 저장되어 있는지 반환
 parm : cam_id	- [in]  Camera Number (1 or 2)
		img_id	- [in]  Camera Grabbed Image Number (Zero based)
		mark	- [out] Mark Index (Zero based) 반환
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Recipe_GetGrabNumToLocalMark(UINT8 cam_id, UINT8 img_id, UINT8 &mark);
/*
 desc : 현재 카메라마다 Grabbed Image 번호에 해당되는 방향 (스테이지 이동 방향) 정보 반환
 parm : img_id	- [in]  Camera Grabbed Image Number (Zero based)
 retn : TRUE (Normal : 정방향; 전진) or FALSE (역방향; 후진)
*/
API_IMPORT BOOL uvEng_Recipe_GetImageToStageDirect(UINT8 img_id);
/*
 desc : 얼라인 마크 검색을 위해 1 Scan 하게 되면, 검색되는 마크의 개수 즉,
		1 Scan 당 등록된 마크의 개수 (현재 선택된 레시피(거버)에 한함)
 parm : type	- [in]  마크 구성 형식
 retn : 1 Scan 당 등록된 마크의 개수 반환 최대 10 개 (실패. 0x00)
*/
API_IMPORT UINT8 uvEng_Recipe_GetScanLocalMarkCount();

#ifdef __cplusplus
}
#endif