
#pragma once

#include "../WorkStep.h"

class CWorkRecipeLoad : public CWorkStep
{
/* 생성자 & 파괴자 */
public:

	CWorkRecipeLoad(ENG_BWOK workType , UINT8 offset,ENG_BWOK relayWork);
	virtual ~CWorkRecipeLoad();

/* 가상함수 재정의 */
protected:
public:
	bool useAFtemp = false;
	virtual	BOOL		InitWork();
	virtual VOID		DoWork();


/* 로컬 변수 */
protected:

	UINT8				m_u8Offset;
	UINT8				m_u8WaitLoaded;
	UINT16				m_u16LampValue[8];

	UINT64				m_u64TickLoaded;

/* 로컬 함수 */
protected:

	VOID				SetWorkNext();
	VOID				CalcStepRate();

	ENG_JWNS			IsRecvJobLists();

	ENG_JWNS			SetRectangleLock();
	ENG_JWNS			IsRectangleLock();
	ENG_JWNS SetMarkRecipe();
	ENG_JWNS			SetPhOffset();
	ENG_JWNS			IsPhOffset();

	ENG_JWNS			GetGerberJobParam();
	ENG_JWNS			IsSetGerberJobParam();

	ENG_JWNS			LoadSelectJobXML();
#if 1
	ENG_JWNS			IsGerberJobLoaded();
#endif
	ENG_JWNS			SetLampJobParam();
	//ENG_JWNS			IsLampJobParam();
	VOID				PhilRecipeLoadComp(UINT8 state);
	VOID				PhilRecipeInit();
/* 공용 함수 */
public:

};
