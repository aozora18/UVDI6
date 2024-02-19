
#pragma once

#include "Base.h"

class CLedPower : public CBase
{
/* ������ & �ı��� */
public:

	CLedPower(TCHAR *work_dir);
	virtual ~CLedPower();


/* ���� ���� */
protected:

	CAtlList <LPG_PLPI>	m_lstRecipe;


/* ���� �Լ� */
protected:

	BOOL				ParseAppend(CHAR *data, UINT32 size);

	VOID				RemoveAll();


/* ���� �Լ� */
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
