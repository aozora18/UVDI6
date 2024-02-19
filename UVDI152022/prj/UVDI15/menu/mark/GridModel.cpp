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
 desc : �� ����
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
		/* ȭ�� ���� ��Ȱ��ȭ */
		m_pGrid->SetRedraw(FALSE);
		/* �׸� ������ŭ Row ���� */
		m_pGrid->SetRowCount(i32RowCnt);
		/* ��ϵ� �׸� �ٽ� ���� */
		for (; i<i32RowCnt; i++)
		{
			m_pGrid->SetRowHeight(i, 30);
			pstRecipe	= uvEng_Mark_GetModelIndex(i);
			m_pGrid->SetItemText(i, 0, csCnv.Ansi2Uni(pstRecipe->name));
			m_pGrid->SetItemFgColour(i, 0, m_stGV.crFgClr);
			m_pGrid->SetItemBkColour(i, 0, m_stGV.crBkClr);
		}
		/* ���� ���� ���� */
		m_pGrid->SortItems(0, TRUE);
		/* ȭ�� ���� Ȱ��ȭ */
		m_pGrid->SetRedraw(TRUE, TRUE);
	}

	/* �׸��� ��Ʈ�� ���� */
	m_pGrid->Invalidate();
}

/*
 desc : �ش� ��ġ�� ������ ���� ��ȯ
 parm : index	- [in]  ���õ� ��ġ ���� (zero-based)
 retn : ���õ� ������ ����� ����ü ������
*/
LPG_CMPV CGridModel::GetRecipe(INT32 index)
{
	TCHAR tzRecipe[MARK_MODEL_NAME_LENGTH]	= {NULL};

	/* LED Power Name ��� */
	if (index < 0)	return NULL;
	wcscpy_s(tzRecipe, MARK_MODEL_NAME_LENGTH, m_pGrid->GetItemText(index, 0));
	return uvEng_Mark_GetModelName(tzRecipe);
}

/*
 desc : ���� ���õ� �׸� ��ȯ
 parm : index	- [in]  ���õ� ��ġ ���� (zero-based)
 retn : ���õ� ������ ����� ����ü ������
*/
LPG_CMPV CGridModel::GetSelected()
{
	UINT32 i	= 0, u32State = 0;
	TCHAR tzRecipe[MARK_MODEL_NAME_LENGTH]	= {NULL};

	/* LED Power Name ��� */
	for (; i<(UINT32)m_pGrid->GetRowCount(); i++)
	{
		/* ���� ��ġ �׸� �Ӽ� �� ��� */
		u32State	= m_pGrid->GetItemState(i, 0);
		if (GVIS_SELECTED == (u32State & GVIS_SELECTED))
		{
			wcscpy_s(tzRecipe, MARK_MODEL_NAME_LENGTH, m_pGrid->GetItemText(i, 0));
			return uvEng_Mark_GetModelName(tzRecipe);
		}
	}

	return NULL;
}
