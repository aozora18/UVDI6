
/*
 desc : The Interface Library for LED Power
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

/*
 desc : Led Power 적재
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_LedPower_LoadPower()
{
	if (!g_pLedPower)	return FALSE;
	return g_pLedPower->LoadFile();
}

/*
 desc : Led Power 문자열 저장
 parm : strLedPower	- [in]  저장하고자 하는 Led Power 내용의 문자열
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_LedPower_SavePower(CString strLedPower)
{
	if (!g_pLedPower)	return FALSE;
	return g_pLedPower->SaveFile(strLedPower);
}

/*
 desc : Led Power 저장
 parm : stLedPower	- [in]  저장하고자 하는 Led Power 내용의 구조체
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_LedPower_SavePowerEx(LPG_PLPI pstLedPower)
{
	if (!g_pLedPower)	return FALSE;
	return g_pLedPower->SaveFile(pstLedPower);
}

/*
 desc : Led Power 이름 검색
 parm : name	- [in]  검색하고자 하는 Led Power 이름
 retn : TRUE (Finded) or FALSE (Not find)
*/
API_EXPORT BOOL uvEng_LedPower_FindLedPower(TCHAR *name)
{
	CUniToChar csCnv;
	if (!g_pLedPower)	return FALSE;
	return g_pLedPower->FindLedPower(csCnv.Uni2Ansi(name));
}

/*
 desc : Led Power 이름을 통해 몇 번째 저장되어 있는지
 parm : name	- [in]  검색하고자 하는 Led Power 이름
 retn : 리스트에 저장되어 있는 위치 반환 (음수이면 검색 실패)
*/
API_EXPORT INT32 uvEng_LedPower_GetLedPowerPos(TCHAR *name)
{
	CUniToChar csCnv;
	if (!g_pLedPower)	return FALSE;
	return g_pLedPower->GetLedPowerPos(csCnv.Uni2Ansi(name));
}

/*
 desc : Led Power 이름을 통한 레시피 반환
 parm : name	- [in]  검색하고자 하는 Led Power 이름
 retn : 반환될 레시피가 저장된 구조체 포인터
*/
API_EXPORT LPG_PLPI uvEng_LedPower_GetLedPowerName(TCHAR *name)
{
	CUniToChar csCnv;
	if (!g_pLedPower)	return NULL;
	return g_pLedPower->GetLedPowerName(csCnv.Uni2Ansi(name));
}

/*
 desc : Led Power가 저장된 인덱스 위치에 있는 Led Power 반환
 parm : index	- [in]  가져오고자 하는 Led Power 인덱스 (Zero-based)
 retn : 반환될 Led Power가 저장된 구조체 포인터
*/
API_EXPORT LPG_PLPI uvEng_LedPower_GetLedPowerIndex(UINT16 index)
{
	if (!g_pLedPower)	return NULL;
	return g_pLedPower->GetLedPowerIndex(index);
}

/*
 desc : 등록된 Led Power 개수 얻기
 parm : None
 retn : 등록된 개수 반환 (없으면, 0)
*/
API_EXPORT UINT16 uvEng_LedPower_GetCount()
{
	if (!g_pLedPower)	return 0;
	return g_pLedPower->GetLedPowerCount();
}

#ifdef __cplusplus
}
#endif