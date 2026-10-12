
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
desc : User Position 데이터를 파일에서 읽어와 메모리에 적재
parm : None
retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_UserPosition_LoadFile()
{
	if (!g_pUserPosition)	return FALSE;
	return g_pUserPosition->LoadFile();
}

/*
	 desc : 메모리에 있는 User Position 데이터를 파일에 저장
	 parm : None
	 retn : TRUE or FALSE
	*/
API_EXPORT BOOL uvEng_UserPosition_SaveFile()
{
	if (!g_pUserPosition) return FALSE;
	return g_pUserPosition->SaveFile();
}

/*
 desc : 특정 인덱스의 User Position 데이터 가져오기
 parm : nIndex - [in] 가져올 데이터의 인덱스 번호 (0 ~ 11)
 retn : 성공 시 데이터 구조체 포인터(LPG_UPTP), 실패 시 NULL
*/
API_EXPORT LPG_UPTP uvEng_UserPosition_GetUserPosData(int nIndex)
{
	if (!g_pUserPosition) return NULL;
	return g_pUserPosition->GetUserPosData(nIndex);
}

API_EXPORT BOOL uvEng_UserPosition_SetUserPosData(int nIndex, LPCTSTR strName,
	double dPosX, double dPosY, double dCam1X, double dCam2X)
{
	if (!g_pUserPosition) return NULL;
	g_pUserPosition->SetUserPosData(nIndex, strName, dPosX, dPosY, dCam1X, dCam2X);
	return TRUE;
}
#ifdef __cplusplus
}
#endif