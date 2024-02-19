
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
API_EXPORT LPG_CMRD uvEng_Mark_GetRecipeName(PTCHAR name)
{
	CUniToChar csCnv;
	if (!g_pMark)	return NULL;
	return g_pMark->GetRecipeName(csCnv.Uni2Ansi(name));
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
API_EXPORT LPG_CMRD uvEng_Mark_GetRecipeIndex(UINT8 index)
{
	if (!g_pMark)	return NULL;
	return g_pMark->GetRecipeIndex(index);
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
API_EXPORT BOOL uvEng_Mark_MarkAppend(LPG_CMRD data, UINT8 mode)
{
	if (!g_pMark)	return FALSE;
	
	if (0x00 == mode)	return g_pMark->MarkAppend(data);
	else				return g_pMark->MarkModify(data);
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
API_EXPORT BOOL uvEng_Mark_MarkDelete(PTCHAR name)
{
	CUniToChar csCnv;
	if (!g_pMark)	return FALSE;
	return g_pMark->MarkDelete(csCnv.Uni2Ansi(name));
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
API_EXPORT UINT32 uvEng_Mark_GetRecipeCount()
{
	return !g_pMark ? 0 : g_pMark->GetRecipeCount();
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
API_EXPORT LPG_CMRD uvEng_Mark_GetSelectRecipe()
{
	if (!g_pMark)	return NULL;
	return g_pMark->GetSelectRecipe();
}

/*
 desc : 현재 선택된 레시피 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvEng_Mark_SelRecipeReset()
{
	if (g_pMark)	g_pMark->SelRecipeReset();
}

/*
 desc : 레시피 이름을 통한 레시피 선택
 parm : r_name	- [in]  Mark 레시피 이름
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Mark_SelRecipeName(PTCHAR r_name)
{
	CUniToChar csCnv;
	if (!g_pMark)	return FALSE;
	return g_pMark->SelRecipeName(csCnv.Uni2Ansi(r_name));
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

#ifdef __cplusplus
}
#endif