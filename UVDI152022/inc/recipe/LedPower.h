
#pragma once

#include "Base.h"

class CLedPower : public CBase
{
/* 생성자 & 파괴자 */
public:

	CLedPower(TCHAR *work_dir);
	virtual ~CLedPower();


/* 로컬 변수 */
protected:

	CAtlList <LPG_PLPI>	m_lstRecipe;


/* 로컬 함수 */
protected:

	BOOL				ParseAppend(CHAR *data, UINT32 size);

	VOID				RemoveAll();


/* 공용 함수 */
public:

	BOOL				LoadFile();

	// 20230703 mhbaek Add : SaveFile
	BOOL				SaveFile(CString strData);
	BOOL				SaveFile(LPG_PLPI pstData);

	BOOL				FindLedPower(CHAR *name);
	INT32				GetLedPowerPos(CHAR *name);
	LPG_PLPI			GetLedPowerName(CHAR *name);
	LPG_PLPI			GetLedPowerIndex(UINT16 index);
	UINT16				GetLedPowerCount()	{	return (UINT16)m_lstRecipe.GetCount();	}
};
