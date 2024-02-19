
#pragma once

#include "Base.h"

class CLedPower;

class CRecipePodis : public CBase
{
/* ������ & �ı��� */
public:

	CRecipePodis(PTCHAR work_dir, CLedPower *led_power, LPG_LDSM mem_luria);
	virtual ~CRecipePodis();


/* ���� ���� */
protected:

	CAtlList <LPG_RBGF>	m_lstRecipe;


	/* ���� ���õ� ������ �̸� �� �� �Ӽ� ���� */
	LPG_RBGF			m_pstRecipe;

	LPG_LDSM			m_pstLuriaMem;

	CLedPower			*m_pLedPower;


/* ���� �Լ� */
protected:

	UINT8				GetSelectRecipeLocalMarkCount();

	BOOL				ParseRecipe(PCHAR data, UINT32 size);
	BOOL				ParseAttrib(PCHAR data, UINT32 size);
	BOOL				RemoveRecipe(PCHAR name);
	BOOL				RemoveAttrib(PCHAR name);
	VOID				RemoveAll();

	BOOL				CopyRecipe(LPG_RBGF source, LPG_RBGF target);


/* ���� �Լ� */
public:

	LPG_RBGF			GetSelectRecipe();

	BOOL				LoadFile();
	BOOL				SaveFile();

	BOOL				RecipeAppend(LPG_RBGF recipe);
	BOOL				RecipeModify(LPG_RBGF recipe);

	BOOL				RecipeDelete(PCHAR recipe);
	BOOL				RecipeFind(PCHAR recipe);

	BOOL				AttribDelete(PCHAR attrib);
	BOOL				AttribFind(PCHAR attrib);

	BOOL				IsRecipeSharedType();
	BOOL				IsExistLocalMark();

	VOID				SelRecipeReset()	{	m_pstRecipe	= NULL;	}
	BOOL				SelRecipeOnlyName(PCHAR recipe);
	BOOL				SelRecipePathName(PCHAR recipe);
	BOOL				SelAttribName(PCHAR attrib);
	LPG_RBGF			GetRecipeOnlyName(PCHAR recipe);
	LPG_RBGF			GetRecipePathName(PCHAR recipe);
	LPG_RBGF			GetRecipeIndex(INT32 index);
	int					GetSelectRecipeIndex();	// by sysandj
	LPG_RBGF			GetCaliRecipe();

	VOID				ResetSelectRecipe();
	UINT32				GetRecipeCount()			{	return (UINT32)m_lstRecipe.GetCount();	}
	BOOL				GetLocalMarkToGrabNum(UINT8 mark, UINT8 &cam_id, UINT8 &img_id);
	BOOL				GetGrabNumToLocalMark(UINT8 cam_id, UINT8 img_id, UINT8 &mark);
	UINT8				GetLocalMarkToScanNum(UINT8 mark_id);
	BOOL				GetImageToStageDirect(UINT8 img_id);

	BOOL				CalcSpeedEnergy(LPG_RBGF recipe);

	UINT8				GetScanLocalMarkCount();
};
