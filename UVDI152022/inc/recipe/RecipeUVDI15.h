
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
	LPG_RJAF			m_pstJobRecipe = nullptr;
	LPG_RJAF			m_pstlocalJobRecipe=nullptr;
	bool whatLastSelectIsLocal = false; //�⺻���� host recipe select�̴�. 


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

	LPG_RJAF			GetSelectJobRecipe(bool local);
	

	BOOL GetWhatLastSelectIsLocal();
	VOID SetWhatLastSelectIsLocal(bool localJobAtTime);

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





	BOOL				SelJobRecipeOnlyName(PCHAR recipe, bool isLocalJobRecipe);
	BOOL				SelJobRecipePathName(PCHAR recipe, bool isLocalJobRecipe);
	BOOL				SelExpoRecipeOnlyName(PCHAR recipe);


	VOID				ResetSelectJobRecipe();
	
	UINT32				GetJobRecipeCount()			{	return (UINT32)m_lstJobRecipe.GetCount();	}
	UINT32				GetExpoRecipeCount()		{   return (UINT32)m_lstExpoRecipe.GetCount(); }

};
