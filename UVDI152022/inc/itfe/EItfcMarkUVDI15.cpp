
/*
 desc : The Interface Library for Mark Model & Recipe
*/

#include "pch.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


#ifdef __cplusplus
extern "C"
{
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                          Model (Mark)                                         */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Model 적재
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Mark_LoadFile()
{
	if (!g_pMark)	return FALSE;
	return g_pMark->LoadFile();
}

/*
 desc : Model Save
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Mark_SaveFile()
{
	if (!g_pMark)	return FALSE;
	return g_pMark->SaveFile();
}

/*
 desc : Model 이름 검색
 parm : name	- [in]  검색하고자 하는 Model 이름
 retn : TRUE (Finded) or FALSE (Not find)
*/
API_EXPORT BOOL uvEng_Mark_ModelFind(PTCHAR name)
{
	CUniToChar csCnv;
	if (!g_pMark)	return FALSE;
	return g_pMark->ModelFind(csCnv.Uni2Ansi(name));
}

/*
 desc : Model 이름을 통한 Model 반환
 parm : name	- [in]  검색하고자 하는 Model 이름
 retn : 반환될 Model이 저장된 구조체 포인터
*/
API_EXPORT LPG_CMPV uvEng_Mark_GetModelName(PTCHAR name)
{
	CUniToChar csCnv;
	if (!g_pMark)	return NULL;
	return g_pMark->GetModelName(csCnv.Uni2Ansi(name));
}

/*
 desc : Recipe 이름을 통한 Recipe 반환
 parm : name	- [in]  검색하고자 하는 Model 이름
 retn : 반환될 Model이 저장된 구조체 포인터
*/
API_EXPORT LPG_RAAF uvEng_Mark_GetAlignRecipeName(PTCHAR name)
{
	CUniToChar csCnv;
	if (!g_pMark)	return NULL;
	return g_pMark->GetAlignRecipeName(csCnv.Uni2Ansi(name));
}

/*
 desc : Model가 저장된 인덱스 위치에 있는 Model 반환
 parm : index	- [in]  가져오고자 하는 Model 인덱스 (Zero-based)
 retn : 반환될 Model이 저장된 구조체 포인터
*/
API_EXPORT LPG_CMPV uvEng_Mark_GetModelIndex(UINT8 index)
{
	if (!g_pMark)	return NULL;
	return g_pMark->GetModelIndex(index);
}

/*
 desc : Recipe가 저장된 인덱스 위치에 있는 Model 반환
 parm : index	- [in]  가져오고자 하는 Model 인덱스 (Zero-based)
 retn : 반환될 Recipe가 저장된 구조체 포인터
*/
API_EXPORT LPG_RAAF uvEng_Mark_GetAlignRecipeIndex(UINT8 index)
{
	if (!g_pMark)	return NULL;
	return g_pMark->GetAlignRecipeIndex(index);
}

/*
 desc : Model Append
 parm : value	- [in]  Model 정보가 저장된 구조체 포인터
		mode	- [in]  0x00 - Append, 0x01 - Modify
 retn : TRUE or FALSE (개수 초과. 최대 15개)
*/
API_EXPORT BOOL uvEng_Mark_ModelAppend(LPG_CMPV value, UINT8 mode)
{
	if (!g_pMark)	return FALSE;
	
	if (0x00 == mode)	return g_pMark->ModelAppend(value);
	else				return g_pMark->ModelModify(value);
}

/*
 desc : Recipe Append
 parm : data	- [in]  Recipe 정보가 저장된 구조체 포인터
		mode	- [in]  0x00 - Append, 0x01 - Modify
 retn : TRUE or FALSE (개수 초과. 최대 15개)
*/
API_EXPORT BOOL uvEng_Mark_AlignRecipeAppend(LPG_RAAF data, UINT8 mode)
{
	if (!g_pMark)	return FALSE;
	
	if (0x00 == mode)	return g_pMark->AlignRecipeAppend(data);
	else				return g_pMark->AlignRecipeModify(data);
}

API_EXPORT BOOL uvEng_Mark_AlignRecipeModify(LPG_RAAF data)
{
	if (!g_pMark)	return FALSE;

	g_pMark->AlignRecipeModify(data);

	return TRUE;
}

/*
 desc : Model 이름 검색를 통한 삭제
 parm : name	- [in]  삭제하고자 하는 Model 이름
 retn : TRUE (Deleted) or FALSE (Not delete)
*/
API_EXPORT BOOL uvEng_Mark_ModelDelete(PTCHAR name)
{
	CUniToChar csCnv;
	if (!g_pMark)	return FALSE;
	return g_pMark->ModelDelete(csCnv.Uni2Ansi(name));
}

/*
 desc : Recipe 이름 검색를 통한 삭제
 parm : name	- [in]  삭제하고자 하는 Recipe 이름
 retn : TRUE (Deleted) or FALSE (Not delete)
*/
API_EXPORT BOOL uvEng_Mark_AlignRecipeDelete(PTCHAR name)
{
	CUniToChar csCnv;
	if (!g_pMark)	return FALSE;
	return g_pMark->AlignRecipeDelete(csCnv.Uni2Ansi(name));
}

/*
 desc : 현재 등록된 모델 개수 반환
 parm : None
 retn : 개수
*/
API_EXPORT UINT32 uvEng_Mark_GetModelCount()
{
	return !g_pMark ? 0 : g_pMark->GetModelCount();
}

/*
 desc : 현재 등록된 레시피 개수 반환
 parm : None
 retn : 개수
*/
API_EXPORT UINT32 uvEng_Mark_GetAlignRecipeCount()
{
	return !g_pMark ? 0 : g_pMark->GetAlignRecipeCount();
}

/*
 desc : 모델 타입에 따른 문자열 반환
 parm : type	- [in]  모델 타입 코드 값
 retn : 문자열 반환
*/
API_EXPORT PTCHAR uvEng_Mark_GetModelTypeToStr(UINT32 type)
{
	if (!g_pMark)	return L"";
	return g_pMark->GetModelTypeToStr(type);
}

/*
 desc : 현재 선택된 레시피 정보 반환
 parm : None
 retn : 현재 선택된 레시피 구조체 포인터
*/
API_EXPORT LPG_RAAF uvEng_Mark_GetSelectAlignRecipe()
{
	if (!g_pMark)	return NULL;
	return g_pMark->GetSelectAlignRecipe();
}

/*
 desc : 현재 선택된 레시피 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvEng_MarkSelAlignRecipeReset()
{
	if (g_pMark)	g_pMark->SelAlignRecipeReset();
}

/*
 desc : 레시피 이름을 통한 레시피 선택
 parm : r_name	- [in]  Mark 레시피 이름
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Mark_SelAlignRecipeName(PTCHAR r_name)
{
	CUniToChar csCnv;
	if (!g_pMark)	return FALSE;
	return g_pMark->SelAlignRecipeName(csCnv.Uni2Ansi(r_name));
}

/* ----------------------------------------------------------------------------------------- */
/*                                 lk91 VISION 추가 함수                                     */
/* ----------------------------------------------------------------------------------------- */
/* desc : ROI Save */
API_EXPORT BOOL uvEng_Mark_MarkROISave()
{
	if (!g_pMark)	return FALSE;
	return g_pMark->SaveROI();
}
/* desc : ROI Setting */
API_EXPORT BOOL uvEng_Mark_SetMarkROI(LPG_CRD data, UINT8 index)
{
	if (!g_pMark)	return FALSE;
	return g_pMark->SetMarkROI(data, index);
}

/* ----------------------------------------------------------------------------------------- */
/*                                 Align_type			                                     */
/* ----------------------------------------------------------------------------------------- */



/*
 desc : 현재 선택된 Recipe의 Mark 구성 방식이 Shared Type인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Recipe_IsRecipeSharedType()
{
	return g_pMark->IsRecipeSharedType();
}

/*
 desc : 현재 선택된 Recipe의 Mark 구성 방식이 Local Mark가 존재하는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Recipe_IsExistLocalMark()
{
	return g_pMark->IsExistLocalMark();
}

/*
 desc : 가져오려는 위치의 Mark Index 값이 Camera 몇 번이고 몇 번째 이미지 정보인지 반환
 parm : mark	- [in]  Mark Index (Zero based)
		cam_id	- [out] Camera Number (1 or 2)
		img_id	- [out] Camera Grabbed Image Number (Zero based)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Recipe_GetLocalMarkToGrabNum(UINT8 mark, UINT8& cam_id, UINT8& img_id)
{
	return g_pMark->GetLocalMarkToGrabNum(mark, cam_id, img_id);
}

/*
 desc : 현재 Local Mark Index 값 값에 해당된 Scan 번호 값 반환
 parm : mark_id	- [in]  Local Mark Index 값 (0 or Later)
 retn : Scan Number (Zero Based)
*/
API_EXPORT UINT8 uvEng_Recipe_GetLocalMarkToScanNum(UINT8 mark_id)
{
	return g_pMark->GetLocalMarkToScanNum(mark_id);
}

/*
 desc : 현재 Camera Index와 Grabbed Image를 가지고 몇 번째 인덱스에 저장되어 있는지 반환
 parm : cam_id	- [in]  Camera Number (1 or 2)
		img_id	- [in]  Camera Grabbed Image Number (Zero based)
		mark	- [out] Mark Index (Zero based) 반환
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Recipe_GetGrabNumToLocalMark(UINT8 cam_id, UINT8 img_id, UINT8& mark)
{
	return g_pMark->GetGrabNumToLocalMark(cam_id, img_id, mark);
}

/*
 desc : 현재 카메라마다 Grabbed Image 번호에 해당되는 방향 (스테이지 이동 방향) 정보 반환
 parm : img_id	- [in]  Camera Grabbed Image Number (Zero based)
 retn : TRUE (Normal : 정방향; 전진) or FALSE (역방향; 후진)
*/
API_EXPORT BOOL uvEng_Recipe_GetImageToStageDirect(UINT8 img_id)
{
	return g_pMark->GetImageToStageDirect(img_id);
}

/*
 desc : 얼라인 마크 검색을 위해 1 Scan 하게 되면, 검색되는 마크의 개수 즉,
		1 Scan 당 등록된 마크의 개수 (현재 선택된 레시피(거버)에 한함)
 parm : type	- [in]  마크 구성 형식
 retn : 1 Scan 당 등록된 마크의 개수 반환 최대 10 개 (실패. 0x00)
*/
API_EXPORT UINT8 uvEng_Recipe_GetScanLocalMarkCount()
{
	return g_pMark->GetScanLocalMarkCount();
}


#ifdef __cplusplus
}
#endif