/*
 desc : Gerber Mark Grid Control
 */

#include "pch.h"
#include "../../MainApp.h"
#include "GridBase.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                       Base Grid Control                                       */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CGridBase::~CGridBase()
{
	if (m_pGrid)
	{
		if (m_pGrid->GetSafeHwnd())	m_pGrid->DestroyWindow();
		delete m_pGrid;
	}
}

/*
 desc : 초기화 (생성)
 parm : horz	- [in]  수평 스크롤바 생성 여부
		vert	- [in]  수직 스크롤바 생성 여부
		btn		- [in]  우측 버튼 생성 여부
 retn : TRUE or FALSE
*/
BOOL CGridBase::Init(BOOL horz, BOOL vert, BOOL btn)
{
	/* 현재 윈도 Client 크기 */
	m_pCtrl->GetWindowRect(m_rGrid);
	m_pParent->ScreenToClient(m_rGrid);

	CResizeUI clsResizeUI;
	/* Grid 크기 재설정 */
	m_rGrid.left	+= (long)(15 * clsResizeUI.GetRateX());
	m_rGrid.top		+= (long)(m_u32Height * clsResizeUI.GetRateY());
	if(!btn)
		m_rGrid.right	-= (long)(15 * clsResizeUI.GetRateX());
	else
		m_rGrid.right -= (long)(15 * clsResizeUI.GetRateX() + 40); 
	m_rGrid.bottom	-= (long)(15 * clsResizeUI.GetRateY());

	/* 객체 생성 */
	m_pGrid = new CGridCtrl;
	ASSERT(m_pGrid);
	m_pGrid->SetDrawScrollBarHorz(horz);
	m_pGrid->SetDrawScrollBarVert(vert);
	if (!m_pGrid->Create(m_rGrid, m_pParent, m_u32GridID, m_u32Style))
	{
		delete m_pGrid;
		return FALSE;
	}

	/* 객체 기본 설정 */
	m_pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	m_pGrid->SetColumnCount(1);
	m_pGrid->SetBkColor(RGB(255, 255, 255));

	/* 본문 */
	m_pGrid->SetColumnWidth(0, m_rGrid.Width()-21);

	/* 기본 속성 및 갱신 */
	m_pGrid->SetBaseGridProp(0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGrid->Refresh();

	/* 거버 레시피 적재 */
	Load();

	return TRUE;
}

/*
 desc : 거버 레시피 이름과 동일한 항목만 선택
 parm : recipe	- [in]  Gerber Recipe Name
 retn : None
*/
VOID CGridBase::Select(PTCHAR recipe)
{
	UINT32 i	= 0, u32State = 0;
	TCHAR tzRecipe[512]	= {NULL};

	/* 비활성화 */
	m_pGrid->SetRedraw(FALSE);

	for (; i<(UINT32)m_pGrid->GetRowCount(); i++)
	{
		/* 현재 위치 항목 속성 값 얻기 */
		u32State	= m_pGrid->GetItemState(i, 0);
		wmemset(tzRecipe, 0x00, 512);
		wcscpy_s(tzRecipe, 512, m_pGrid->GetItemText(i, 0));
		if (m_pGrid->GetItemText(i, 0).Compare(recipe))	u32State &= ~GVIS_SELECTED;
		else											u32State |= GVIS_SELECTED;
		/* 항목 선택 여부 설정 */
		m_pGrid->SetItemState(i, 0, u32State);
	}

	/* 활성화 */
	m_pGrid->SetRedraw(TRUE, TRUE);
	m_pGrid->Invalidate(FALSE);
}
