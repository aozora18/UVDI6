
#pragma once

#include "Base.h"

class CUserPosition : public CBase
{
/* 생성자 & 파괴자 */
public:

	CUserPosition(TCHAR *work_dir);
	virtual ~CUserPosition();


/* 로컬 변수 */
protected:

	CAtlList <LPG_UPTP>	m_lstRecipe;


/* 로컬 함수 */
protected:

	BOOL				ParseAppend(CHAR *data, UINT32 size);

	VOID				RemoveAll();


/* 공용 함수 */
public:

	BOOL				LoadFile();
	BOOL				SaveFile();

	LPG_UPTP			GetUserPosData(int nIndex);
	VOID				SetUserPosData(int nIndex, LPCTSTR strName,
		double dPosX, double dPosY, double dCam1X, double dCam2X);

};
