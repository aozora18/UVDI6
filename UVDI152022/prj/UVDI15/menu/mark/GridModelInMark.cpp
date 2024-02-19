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
 desc : Mark �����ǿ� ���Ե� Mark �� ���� �̸� ���
 parm : recipe	- [in]  Mark Recipe ����ü ������
 retn : None
*/
VOID CGridModelInMark::UpdateModels(LPG_RAAF recipe)
{
	INT32 i = 0;
	CUniToChar csCnv;

	/* ȭ�� ���� ��Ȱ��ȭ */
	m_pGrid->SetRedraw(FALSE);
	/* �׸� ������ŭ Row ���� */
	if (recipe)
	{
		m_pGrid->SetRowCount(recipe->save_count);
		/* ��ϵ� �׸� �ٽ� ���� */
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
#if 0	/* �����ϸ� �ȵ� */
	/* ���� ���� ���� */
	m_pGrid->SortItems(0, TRUE);
#endif
	/* ȭ�� ���� Ȱ��ȭ */
	m_pGrid->SetRedraw(TRUE, TRUE);

	/* �׸��� ��Ʈ�� ���� */
	m_pGrid->Invalidate();
}

/*
 desc : �ش� ��ġ�� ������ ���� ��ȯ
 parm : index	- [in]  ���õ� ��ġ ���� (zero-based)
 retn : ���õ� ������ ����� ����ü ������
*/
LPG_CMPV CGridModelInMark::GetRecipe(INT32 index)
{
	TCHAR tzRecipe[MARK_MODEL_NAME_LENGTH]	= {NULL};

	/* LED Power Name ��� */
	if (index < 0)	return NULL;
	wcscpy_s(tzRecipe, MARK_MODEL_NAME_LENGTH, m_pGrid->GetItemText(index, 0));
	return uvEng_Mark_GetModelName(tzRecipe);
}
