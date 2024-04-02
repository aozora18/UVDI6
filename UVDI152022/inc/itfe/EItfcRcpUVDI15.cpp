
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


/* --------------------------------------------------------------------------------------------- */
/*                                           Job Recipe                                          */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 등록된 Job 레시피 개수 반환
 parm : None
 retn : 등록된 개수 반환
*/
API_EXPORT UINT32 uvEng_JobRecipe_GetCount()
{
	return g_pRecipe->GetJobRecipeCount();
}

/*
 desc : 현재 선택된 Job 레시피 기본 반환
 parm : None
 retn : 레시피가 저장된 구조체 포인터
*/
API_EXPORT LPG_RJAF uvEng_JobRecipe_GetSelectRecipe()
{
	if (!g_pRecipe)	return NULL;
	return g_pRecipe->GetSelectJobRecipe();
}

/*
 desc : 현재 선택된 Job 레시피 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvEng_JobRecipe_ResetSelectRecipe()
{
	g_pRecipe->ResetSelectJobRecipe();
}

/*
 desc : Calibration Recipe 재-적재
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_JobRecipe_ReloadFile()
{
	return g_pRecipe->LoadFile();
}

/*
 desc : 현재 선택된 레시피의 속성 정보를 통해 스테이지 이동 속도 및 광량 값 계산
 parm : recipe	- [out] 스테이지 이동 속도 및 광량 개별 및 평균 값 반환 (mm/sec)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Recipe_CalcSpeedEnergy(LPG_RJAF recipe)
{
	return g_pRecipe->CalcSpeedEnergy(recipe);
}

/*
 desc : Job 레시피 기본 존재 여부
 parm : recipe	- [in]  검색하고자 하는 레시피 이름
 retn : TRUE (Finded) or FALSE (Not find)
*/
API_EXPORT BOOL uvEng_JobRecipe_RecipeFind(PTCHAR recipe)
{
	CUniToChar csCnv;
	return g_pRecipe->JobRecipeFind(csCnv.Uni2Ansi(recipe));
}

/*
 desc : Job 레시피 이름을 통한 레시피 기본 정보 반환
 parm : recipe	- [in]  검색하고자 하는 레시피 이름
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
API_EXPORT LPG_RJAF uvEng_JobRecipe_GetRecipeOnlyName(PTCHAR recipe)
{
	CUniToChar csCnv;
	return g_pRecipe->GetJobRecipeOnlyName(csCnv.Uni2Ansi(recipe));
}

/*
 desc : Job 레시피 이름 (전체 경로 포함)을 통한 레시피 반환
 parm : path_name	- [in]  검색하고자 하는 거버 이름 (경로 포함)
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
API_EXPORT LPG_RJAF uvEng_JobRecipe_GetRecipePathName(PTCHAR name)
{
	CUniToChar csCnv;
	return g_pRecipe->GetJobRecipePathName(csCnv.Uni2Ansi(name));
}

/*
 desc : Job 레시피 기본 정보가 저장된 인덱스 위치에 있는 레시피 반환
 parm : index	- [in]  가져오고자 하는 레시피 인덱스 (Zero-based)
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
API_EXPORT LPG_RJAF uvEng_JobRecipe_GetRecipeIndex(INT32 index)
{
	return g_pRecipe->GetJobRecipeIndex(index);
}

/*
 desc : Job 레시피 기본 이름을 통한 레시피 기본 선택
 parm : recipe	- [in]  검색하고자 하는 레시피 기본 이름
 retn : TRUE (선택 성공) or FALSE (선택 실패)
*/
API_EXPORT BOOL uvEng_JobRecipe_SelRecipeOnlyName(PTCHAR recipe)
{
	CUniToChar csCnv;
	return g_pRecipe->SelJobRecipeOnlyName(csCnv.Uni2Ansi(recipe));
}

/*
 desc : Job 레시피 기본 이름 (전체 경로 포함)을 통한 레시피 기본 선택
 parm : recipe	- [in]  검색하고자 하는 레시피 기본 이름 (전체 경로 포함)
 retn : TRUE (선택 성공) or FALSE (선택 실패)
*/
API_EXPORT BOOL uvEng_JobRecipe_SelRecipePathName(PTCHAR recipe)
{
	CUniToChar csCnv;
	return g_pRecipe->SelJobRecipePathName(csCnv.Uni2Ansi(recipe));
}

/*
 desc : Job Recipe 저장
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_JobRecipe_SaveFile()
{
	return g_pRecipe->SaveJobFile();
}

/*
 desc : Job Recipe 기본 정보 추가 (Append)
 parm : recipe	- [in]  Recipe 정보가 저장된 구조체 포인터
		check	- [in]  값 유효성 체크 여부
 retn : TRUE or FALSE (개수 초과. 최대 15개)
*/
API_EXPORT BOOL uvEng_JobRecipe_RecipeAppend(LPG_RJAF recipe)
{
	return g_pRecipe->JobRecipeAppend(recipe);
}

/*
 desc : Job Recipe 기본 정보 수정
 parm : recipe	- [in]  Recipe 기본 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_JobRecipe_RecipeModify(LPG_RJAF recipe)
{
	return g_pRecipe->JobRecipeModify(recipe);
}

/*
 desc : Job Recipe 기본 삭제
 parm : recipe	- [in]  제거하려는 레시피의 기본 이름
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_JobRecipe_RecipeDelete(PTCHAR recipe)
{
	CUniToChar csCnv;
	return g_pRecipe->JobRecipeDelete(csCnv.Uni2Ansi(recipe));
}


API_EXPORT int uvEng_JobRecipe_GetSelectRecipeIndex()
{
	CUniToChar csCnv;
	return g_pRecipe->GetSelectJobRecipeIndex();
}

/* --------------------------------------------------------------------------------------------- */
/*                                           Expo Recipe                                          */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 등록된 Expo 레시피 개수 반환
 parm : None
 retn : 등록된 개수 반환
*/
API_EXPORT UINT32 uvEng_ExpoRecipe_GetCount()
{
	return g_pRecipe->GetExpoRecipeCount();
}

/*
 desc : 현재 선택된 Expo 레시피 기본 반환
 parm : None
 retn : 레시피가 저장된 구조체 포인터
*/
API_EXPORT LPG_REAF uvEng_ExpoRecipe_GetSelectRecipe()
{
	if (!g_pRecipe)	return NULL;
	return g_pRecipe->GetSelectExpoRecipe();
}

/*
 desc : 현재 선택된 Expo 레시피 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvEng_ExpoRecipe_ResetSelectRecipe()
{
	g_pRecipe->ResetSelectExpoRecipe();
}

/*
 desc : Expo 레시피 기본 존재 여부
 parm : recipe	- [in]  검색하고자 하는 레시피 이름
 retn : TRUE (Finded) or FALSE (Not find)
*/
API_EXPORT BOOL uvEng_ExpoRecipe_RecipeFind(PTCHAR recipe)
{
	CUniToChar csCnv;
	return g_pRecipe->ExpoRecipeFind(csCnv.Uni2Ansi(recipe));
}

/*
 desc : Expo 레시피 이름을 통한 레시피 기본 정보 반환
 parm : recipe	- [in]  검색하고자 하는 레시피 이름
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
API_EXPORT LPG_REAF uvEng_ExpoRecipe_GetRecipeOnlyName(PTCHAR recipe)
{
	CUniToChar csCnv;
	return g_pRecipe->GetExpoRecipeOnlyName(csCnv.Uni2Ansi(recipe));
}


/*
 desc : Expo 레시피 기본 정보가 저장된 인덱스 위치에 있는 레시피 반환
 parm : index	- [in]  가져오고자 하는 레시피 인덱스 (Zero-based)
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
API_EXPORT LPG_REAF uvEng_ExpoRecipe_GetRecipeIndex(INT32 index)
{
	return g_pRecipe->GetExpoRecipeIndex(index);
}

/*
 desc : Expo 레시피 기본 이름을 통한 레시피 기본 선택
 parm : recipe	- [in]  검색하고자 하는 레시피 기본 이름
 retn : TRUE (선택 성공) or FALSE (선택 실패)
*/
API_EXPORT BOOL uvEng_ExpoRecipe_SelRecipeOnlyName(PTCHAR recipe)
{
	CUniToChar csCnv;
	return g_pRecipe->SelExpoRecipeOnlyName(csCnv.Uni2Ansi(recipe));
}

/*
 desc : Expo Recipe 저장
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_ExpoRecipe_SaveFile()
{
	return g_pRecipe->SaveExpoFile();
}

/*
 desc : Expo Recipe 기본 정보 추가 (Append)
 parm : recipe	- [in]  Recipe 정보가 저장된 구조체 포인터
		check	- [in]  값 유효성 체크 여부
 retn : TRUE or FALSE (개수 초과. 최대 15개)
*/
API_EXPORT BOOL uvEng_ExpoRecipe_RecipeAppend(LPG_REAF recipe)
{
	return g_pRecipe->ExpoRecipeAppend(recipe);
}

/*
 desc : Expo Recipe 기본 정보 수정
 parm : recipe	- [in]  Recipe 기본 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_ExpoRecipe_RecipeModify(LPG_REAF recipe)
{
	return g_pRecipe->ExpoRecipeModify(recipe);
}

/*
 desc : Expo Recipe 기본 삭제
 parm : recipe	- [in]  제거하려는 레시피의 기본 이름
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_ExpoRecipe_RecipeDelete(PTCHAR recipe)
{
	CUniToChar csCnv;
	return g_pRecipe->ExpoRecipeDelete(csCnv.Uni2Ansi(recipe));
}


API_EXPORT int uvEng_ExpoRecipe_GetSelectRecipeIndex()
{
	CUniToChar csCnv;
	return g_pRecipe->GetSelectExpoRecipeIndex();
}




#if 0

/* --------------------------------------------------------------------------------------------- */
/*                                           Align	                                             */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 현재 선택된 Recipe의 Mark 구성 방식이 Shared Type인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Recipe_IsRecipeSharedType()
{
	return g_pRecipe->IsRecipeSharedType();
}

/*
 desc : 현재 선택된 Recipe의 Mark 구성 방식이 Local Mark가 존재하는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Recipe_IsExistLocalMark()
{
	return g_pRecipe->IsExistLocalMark();
}

/*
 desc : 가져오려는 위치의 Mark Index 값이 Camera 몇 번이고 몇 번째 이미지 정보인지 반환
 parm : mark	- [in]  Mark Index (Zero based)
		cam_id	- [out] Camera Number (1 or 2)
		img_id	- [out] Camera Grabbed Image Number (Zero based)
 retn : TRUE or FALSE
*/
//API_EXPORT BOOL uvEng_Recipe_GetLocalMarkToGrabNum(UINT8 mark, UINT8 &cam_id, UINT8 &img_id)
//{
//	return g_pRecipe->GetLocalMarkToGrabNum(mark, cam_id, img_id);
//}

/*
 desc : 현재 Local Mark Index 값 값에 해당된 Scan 번호 값 반환
 parm : mark_id	- [in]  Local Mark Index 값 (0 or Later)
 retn : Scan Number (Zero Based)
*/
//API_EXPORT UINT8 uvEng_Recipe_GetLocalMarkToScanNum(UINT8 mark_id)
//{
//	return g_pRecipe->GetLocalMarkToScanNum(mark_id);
//}

/*
 desc : 현재 Camera Index와 Grabbed Image를 가지고 몇 번째 인덱스에 저장되어 있는지 반환
 parm : cam_id	- [in]  Camera Number (1 or 2)
		img_id	- [in]  Camera Grabbed Image Number (Zero based)
		mark	- [out] Mark Index (Zero based) 반환
 retn : TRUE or FALSE
*/
//API_EXPORT BOOL uvEng_Recipe_GetGrabNumToLocalMark(UINT8 cam_id, UINT8 img_id, UINT8 &mark)
//{
//	return g_pRecipe->GetGrabNumToLocalMark(cam_id, img_id, mark);
//}

/*
 desc : 현재 카메라마다 Grabbed Image 번호에 해당되는 방향 (스테이지 이동 방향) 정보 반환
 parm : img_id	- [in]  Camera Grabbed Image Number (Zero based)
 retn : TRUE (Normal : 정방향; 전진) or FALSE (역방향; 후진)
*/
//API_EXPORT BOOL uvEng_Recipe_GetImageToStageDirect(UINT8 img_id)
//{
//	return g_pRecipe->GetImageToStageDirect(img_id);
//}

/*
 desc : 얼라인 마크 검색을 위해 1 Scan 하게 되면, 검색되는 마크의 개수 즉,
		1 Scan 당 등록된 마크의 개수 (현재 선택된 레시피(거버)에 한함)
 parm : type	- [in]  마크 구성 형식
 retn : 1 Scan 당 등록된 마크의 개수 반환 최대 10 개 (실패. 0x00)
*/
API_EXPORT UINT8 uvEng_Recipe_GetScanLocalMarkCount()
{
	return g_pRecipe->GetScanLocalMarkCount();
}

#endif

#ifdef __cplusplus
}
#endif