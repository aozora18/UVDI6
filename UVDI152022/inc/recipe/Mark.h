#pragma once

#include "Base.h"

class CMark : public CBase
{
/* 생성자 & 파괴자 */
public:

	CMark(PTCHAR work_dir);
	virtual ~CMark();


/* 로컬 변수 */
protected:

	LPG_CMRD			m_pstSelected;	/* 현재 선택된 레시피 */

	CAtlList <LPG_CMPV>	m_lstModel; 
	CAtlList <LPG_CMRD>	m_lstMark; 
	
	LPG_CRD				m_pstROI; // Search ROI

/* 로컬 함수 */
protected:
	BOOL				ParseModel(PCHAR data, UINT32 size);
	BOOL				RemoveModel(PCHAR m_name);
	VOID				RemoveModelAll();

	BOOL				ParseMark(PCHAR data, UINT32 size);
	BOOL				RemoveMark(PCHAR r_name);
	VOID				RemoveMarkAll();
	VOID				CopyRecipe(LPG_CMRD source, LPG_CMRD target);

	BOOL				ParseSearchROI(PCHAR data, UINT32 size, UINT8 cnt);

	BOOL				SaveModel();
	BOOL				SaveRecipe();
	BOOL				FindModelInMark(PCHAR m_name);

/* 공용 함수 */
public:

	BOOL				SetMarkROI(LPG_CRD data, UINT8 index);
	BOOL				SaveROI();
	LPG_CRD				GetMarkROI();
	BOOL				LoadFile();
	BOOL				SaveFile();

	BOOL				ModelAppend(LPG_CMPV value);
	BOOL				ModelModify(LPG_CMPV value);
	BOOL				ModelDelete(PCHAR m_name);
	BOOL				ModelFind(PCHAR m_name);
	LPG_CMPV			GetModelName(PCHAR m_name);
	LPG_CMPV			GetModelIndex(UINT8 index);
	PCHAR				GetModelName(UINT8 index);
	UINT32				GetModelCount()		{	return (UINT32)m_lstModel.GetCount();	}

	BOOL				MarkAppend(LPG_CMRD recipe);
	BOOL				MarkModify(LPG_CMRD recipe);
	BOOL				MarkDelete(PCHAR r_name);
	BOOL				MarkFind(PCHAR r_name);
	LPG_CMRD			GetRecipeName(PCHAR r_name);
	LPG_CMRD			GetRecipeIndex(UINT8 index);
	CHAR				*GetRecipeName(UINT8 index);
	UINT32				GetRecipeCount()	{	return (UINT32)m_lstMark.GetCount();	}

	PTCHAR				GetModelTypeToStr(UINT32 type);

	LPG_CMRD			GetSelectRecipe()		{	return m_pstSelected;	}
	VOID				SelRecipeReset()		{	m_pstSelected	= NULL;	}
	BOOL				SelRecipeName(PCHAR r_name);
};
