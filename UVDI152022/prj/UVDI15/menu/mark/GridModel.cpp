/*
 desc : Mark Model Grid Control
 */

#include "pch.h"
#include "../../MainApp.h"
#include "GridModel.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : 모델 적재
 parm : None
 retn : None
*/
VOID CGridModel::Load()
{
	INT32 i = 0, i32RowCnt	= uvEng_Mark_GetModelCount();
	LPG_CMPV pstRecipe		= NULL;
	CUniToChar csCnv;

	if (i32RowCnt)
	{
		/* 화면 갱신 비활성화 */
		m_pGrid->SetRedraw(FALSE);
		/* 항목 개수만큼 Row 생성 */
		m_pGrid->SetRowCount(i32RowCnt);
		/* 등록된 항목 다시 적재 */
		for (; i<i32RowCnt; i++)
		{
			m_pGrid->SetRowHeight(i, 30);
			pstRecipe	= uvEng_Mark_GetModelIndex(i);
			m_pGrid->SetItemText(i, 0, csCnv.Ansi2Uni(pstRecipe->name));
			m_pGrid->SetItemFgColour(i, 0, m_stGV.crFgClr);
			m_pGrid->SetItemBkColour(i, 0, m_stGV.crBkClr);
		}
		/* 오름 차순 정렬 */
		m_pGrid->SortItems(0, TRUE);
		/* 화면 갱신 활성화 */
		m_pGrid->SetRedraw(TRUE, TRUE);
	}

	/* 그리드 컨트롤 갱신 */
	m_pGrid->Invalidate();
}

/*
 desc : 해당 위치의 레시피 정보 반환
 parm : index	- [in]  선택된 위치 정보 (zero-based)
 retn : 선택된 정보가 저장된 구조체 포인터
*/
LPG_CMPV CGridModel::GetRecipe(INT32 index)
{
	TCHAR tzRecipe[MARK_MODEL_NAME_LENGTH]	= {NULL};

	/* LED Power Name 얻기 */
	if (index < 0)	return NULL;
	wcscpy_s(tzRecipe, MARK_MODEL_NAME_LENGTH, m_pGrid->GetItemText(index, 0));
	return uvEng_Mark_GetModelName(tzRecipe);
}

/*
 desc : 현재 선택된 항목 반환
 parm : index	- [in]  선택된 위치 정보 (zero-based)
 retn : 선택된 정보가 저장된 구조체 포인터
*/
LPG_CMPV CGridModel::GetSelected()
{
	UINT32 i	= 0, u32State = 0;
	TCHAR tzRecipe[MARK_MODEL_NAME_LENGTH]	= {NULL};

	/* LED Power Name 얻기 */
	for (; i<(UINT32)m_pGrid->GetRowCount(); i++)
	{
		/* 현재 위치 항목 속성 값 얻기 */
		u32State	= m_pGrid->GetItemState(i, 0);
		if (GVIS_SELECTED == (u32State & GVIS_SELECTED))
		{
			wcscpy_s(tzRecipe, MARK_MODEL_NAME_LENGTH, m_pGrid->GetItemText(i, 0));
			return uvEng_Mark_GetModelName(tzRecipe);
		}
	}

	return NULL;
}
