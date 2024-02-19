/*
 desc : Mark Model Grid Control
 */

#include "pch.h"
#include "../../MainApp.h"
#include "GridModelInMark.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : Mark 레시피에 포함된 Mark 모델 정보 이름 출력
 parm : recipe	- [in]  Mark Recipe 구조체 포인터
 retn : None
*/
VOID CGridModelInMark::UpdateModels(LPG_RAAF recipe)
{
	INT32 i = 0;
	CUniToChar csCnv;

	/* 화면 갱신 비활성화 */
	m_pGrid->SetRedraw(FALSE);
	/* 항목 개수만큼 Row 생성 */
	if (recipe)
	{
		m_pGrid->SetRowCount(recipe->save_count);
		/* 등록된 항목 다시 적재 */
		for (; i<recipe->save_count; i++)
		{
			m_pGrid->SetRowHeight(i, 30);
			m_pGrid->SetItemText(i, 0, csCnv.Ansi2Uni(recipe->m_name[i]));
			m_pGrid->SetItemFgColour(i, 0, m_stGV.crFgClr);
			m_pGrid->SetItemBkColour(i, 0, m_stGV.crBkClr);
		}
	}
	else
	{
		m_pGrid->SetRowCount(0);
	}
#if 0	/* 정렬하면 안됨 */
	/* 오름 차순 정렬 */
	m_pGrid->SortItems(0, TRUE);
#endif
	/* 화면 갱신 활성화 */
	m_pGrid->SetRedraw(TRUE, TRUE);

	/* 그리드 컨트롤 갱신 */
	m_pGrid->Invalidate();
}

/*
 desc : 해당 위치의 레시피 정보 반환
 parm : index	- [in]  선택된 위치 정보 (zero-based)
 retn : 선택된 정보가 저장된 구조체 포인터
*/
LPG_CMPV CGridModelInMark::GetRecipe(INT32 index)
{
	TCHAR tzRecipe[MARK_MODEL_NAME_LENGTH]	= {NULL};

	/* LED Power Name 얻기 */
	if (index < 0)	return NULL;
	wcscpy_s(tzRecipe, MARK_MODEL_NAME_LENGTH, m_pGrid->GetItemText(index, 0));
	return uvEng_Mark_GetModelName(tzRecipe);
}
