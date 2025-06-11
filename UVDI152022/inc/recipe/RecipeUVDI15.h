
#pragma once

#include "Base.h"

class CLedPower;

class CRecipeUVDI15 : public CBase
{
/* 생성자 & 파괴자 */
public:

	CRecipeUVDI15(PTCHAR work_dir, CLedPower* led_power, LPG_LDSM mem_luria);
	virtual ~CRecipeUVDI15();


/* 로컬 변수 */
protected:

	CAtlList <LPG_RJAF>	m_lstJobRecipe;
	CAtlList <LPG_REAF>	m_lstExpoRecipe;

	/* 현재 선택된 레시피 이름 및 상세 속성 정보 */
	LPG_RJAF			m_pstJobRecipe = nullptr;
	LPG_RJAF			m_pstlocalJobRecipe=nullptr;
	bool whatLastSelectIsLocal = false; //기본값은 host recipe select이다. 


	LPG_LDSM			m_pstLuriaMem;
	CLedPower*			m_pLedPower;

/* 로컬 함수 */
protected:

	BOOL				ParseJobRecipe(PCHAR data, UINT32 size);
	BOOL				RemoveJobRecipe(PCHAR recipe);
	VOID				RemoveAllJob();
	BOOL				CopyJobRecipe(LPG_RJAF source, LPG_RJAF target);

	BOOL				ParseExpoRecipe(PCHAR data, UINT32 size);
	BOOL				RemoveExpoRecipe(PCHAR recipe);
	VOID				RemoveAllExpo();
	BOOL				CopyExpoRecipe(LPG_REAF source, LPG_REAF target);

/* 공용 함수 */
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
