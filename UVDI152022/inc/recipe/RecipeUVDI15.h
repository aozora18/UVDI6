
#pragma once

#include "Base.h"

class CLedPower;

class CRecipeUVDI15 : public CBase
{
/* ������ & �ı��� */
public:

	CRecipeUVDI15(PTCHAR work_dir, CLedPower* led_power, LPG_LDSM mem_luria);
	virtual ~CRecipeUVDI15();


/* ���� ���� */
protected:

	CAtlList <LPG_RJAF>	m_lstJobRecipe;
	CAtlList <LPG_REAF>	m_lstExpoRecipe;

	/* ���� ���õ� ������ �̸� �� �� �Ӽ� ���� */
	LPG_RJAF			m_pstJobRecipe;
	LPG_REAF			m_pstExpoRecipe;
	LPG_LDSM			m_pstLuriaMem;
	CLedPower*			m_pLedPower;

/* ���� �Լ� */
protected:

	BOOL				ParseJobRecipe(PCHAR data, UINT32 size);
	BOOL				RemoveJobRecipe(PCHAR recipe);
	VOID				RemoveAllJob();
	BOOL				CopyJobRecipe(LPG_RJAF source, LPG_RJAF target);

	BOOL				ParseExpoRecipe(PCHAR data, UINT32 size);
	BOOL				RemoveExpoRecipe(PCHAR recipe);
	VOID				RemoveAllExpo();
	BOOL				CopyExpoRecipe(LPG_REAF source, LPG_REAF target);

/* ���� �Լ� */
public:

	LPG_RJAF			GetSelectJobRecipe();
	LPG_REAF			GetSelectExpoRecipe();

	BOOL				LoadFile();
	BOOL				SaveJobFile();
	BOOL				SaveExpoFile();

	BOOL				JobRecipeAppend(LPG_RJAF recipe);
	BOOL				JobRecipeModify(LPG_RJAF recipe);
	BOOL				JobRecipeDelete(PCHAR recipe);
	BOOL				JobRecipeFind(PCHAR recipe);
	LPG_RJAF			GetJobRecipeOnlyName(PCHAR recipe);
	LPG_RJAF			GetJobRecipePathName(PCHAR recipe);
	LPG_RJAF			GetJobRecipeIndex(INT32 index);
	int					GetSelectJobRecipeIndex();	// by sysandj

	BOOL				ExpoRecipeAppend(LPG_REAF recipe);
	BOOL				ExpoRecipeModify(LPG_REAF recipe);
	BOOL				ExpoRecipeDelete(PCHAR recipe);
	BOOL				ExpoRecipeFind(PCHAR recipe);
	LPG_REAF			GetExpoRecipeOnlyName(PCHAR recipe);
	LPG_REAF			GetExpoRecipeIndex(INT32 index);
	int					GetSelectExpoRecipeIndex();
	BOOL				CalcSpeedEnergy(LPG_RJAF recipe);



	VOID				SelJobRecipeReset()		 {	m_pstJobRecipe	= NULL;	}
	VOID				SelExpoRecipeReset()	 {  m_pstExpoRecipe = NULL; }


	BOOL				SelJobRecipeOnlyName(PCHAR recipe);
	BOOL				SelJobRecipePathName(PCHAR recipe);
	BOOL				SelExpoRecipeOnlyName(PCHAR recipe);


	VOID				ResetSelectJobRecipe();
	VOID				ResetSelectExpoRecipe();
	UINT32				GetJobRecipeCount()			{	return (UINT32)m_lstJobRecipe.GetCount();	}
	UINT32				GetExpoRecipeCount()		{   return (UINT32)m_lstExpoRecipe.GetCount(); }

};
