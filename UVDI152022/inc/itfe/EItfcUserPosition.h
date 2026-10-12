
/*
 desc : The Interface Library for LED Power
*/

#pragma once

#include "../conf/global.h"
#include "../conf/define.h"
#include "../conf/recipe_uvdi15.h"


#ifdef __cplusplus
extern "C"
{
#endif
/*
desc : User Position 데이터를 파일에서 읽어와 메모리에 적재
parm : None
retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_UserPosition_LoadFile();

/*
desc : 메모리에 있는 User Position 데이터를 파일에 저장
parm : None
retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_UserPosition_SaveFile();

/*
desc : 특정 인덱스의 User Position 데이터 가져오기
parm : nIndex - [in] 가져올 데이터의 인덱스 번호 (0 ~ 11)
retn : 성공 시 데이터 구조체 포인터(LPG_UPTP), 실패 시 NULL
*/
API_IMPORT LPG_UPTP uvEng_UserPosition_GetUserPosData(int nIndex);

API_IMPORT BOOL uvEng_UserPosition_SetUserPosData(int nIndex, LPCTSTR strName,
	double dPosX, double dPosY, double dCam1X, double dCam2X);

#ifdef __cplusplus
}
#endif