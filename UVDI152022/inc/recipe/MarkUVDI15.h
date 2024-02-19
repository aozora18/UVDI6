#pragma once

#include "Base.h"

class AlignMotion;
class CMarkUVDI15 : public CBase
{
/* 생성자 & 파괴자 */
public:

	CMarkUVDI15(PTCHAR work_dir);
	virtual ~CMarkUVDI15();


/* 로컬 변수 */
protected:

	LPG_RAAF			m_pstSelected;	/* 현재 선택된 레시피 */

	CAtlList <LPG_CMPV>	m_lstModel;
	CAtlList <LPG_RAAF>	m_lstAlignRecipe;

	LPG_CRD				m_pstROI; // Search ROI
	AlignMotion* alignMotion = nullptr;
/* 로컬 함수 */
protected:

	BOOL				ParseModel(PCHAR data, UINT32 size);
	BOOL				RemoveModel(PCHAR m_name);
	VOID				RemoveModelAll();

	BOOL				ParseAlignRecipe(PCHAR data, UINT32 size);
	BOOL				RemoveAlignRecipe(PCHAR r_name);
	VOID				RemoveAlignRecipeAll();
	VOID				CopyAlignRecipe(LPG_RAAF source, LPG_RAAF target);
	BOOL				ParseSearchROI(PCHAR data, UINT32 size, UINT8 cnt);

	BOOL				SaveModel();
	BOOL				SaveAlignRecipe();

	BOOL				FindAlignRecipe(PCHAR align_name);
	BOOL				FindModelInMark(PCHAR m_name);


/* 공용 함수 */
public:
	VOID				SetAlignMotionPtr(AlignMotion& ptr) { alignMotion = &ptr; }
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

	BOOL				AlignRecipeAppend(LPG_RAAF recipe);
	BOOL				AlignRecipeModify(LPG_RAAF recipe);
	BOOL				AlignRecipeDelete(PCHAR r_name);
	BOOL				AlignRecipeFind(PCHAR r_name);
	LPG_RAAF			GetAlignRecipeName(PCHAR r_name);
	LPG_RAAF			GetAlignRecipeIndex(UINT8 index);
	CHAR				*GetAlignRecipeName(UINT8 index);
	UINT32				GetAlignRecipeCount()	{	return (UINT32)m_lstAlignRecipe.GetCount();	}

	PTCHAR				GetModelTypeToStr(UINT32 type);

	LPG_RAAF			GetSelectAlignRecipe()		{	return m_pstSelected;	}
	VOID				SelAlignRecipeReset()		{	m_pstSelected	= NULL;	}
	BOOL				SelAlignRecipeName(PCHAR r_name);


	BOOL				IsRecipeSharedType();
	BOOL				IsExistLocalMark();
	UINT8				GetSelectRecipeLocalMarkCount();
	BOOL				GetLocalMarkToGrabNum(UINT8 mark, UINT8& cam_id, UINT8& img_id);
	UINT8				GetLocalMarkToScanNum(UINT8 mark_id);
	BOOL				GetGrabNumToLocalMark(UINT8 cam_id, UINT8 img_id, UINT8& mark);
	BOOL				GetImageToStageDirect(UINT8 img_id);
	UINT8				GetScanLocalMarkCount();
};
