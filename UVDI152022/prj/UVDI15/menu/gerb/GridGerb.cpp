/*
 desc : Gerber Recipe Grid Control
 */

#include "pch.h"
#include "../../MainApp.h"
#include "GridGerb.h"


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
CGridGerb::~CGridGerb()
{
	if (m_pGrid)
	{
		if (m_pGrid->GetSafeHwnd())	m_pGrid->DestroyWindow();
		delete m_pGrid;
	}
}

/*
 desc : Grid Control이 공통 초기화
 parm : None
 retn : None
*/
VOID CGridGerb::InitGrid()
{
	/* 현재 윈도 Client 크기 */
	m_pCtrl->GetWindowRect(m_rGrid);
	m_pParent->ScreenToClient(m_rGrid);

	CResizeUI clsResizeUI;
	/* Grid 크기 재설정 */
	m_rGrid.left	+= (long)(10 * clsResizeUI.GetRateX());
	m_rGrid.top		+= (long)(22 * clsResizeUI.GetRateY());
	m_rGrid.right	-= (long)(10 * clsResizeUI.GetRateX());
	m_rGrid.bottom	-= (long)(15 * clsResizeUI.GetRateY());
}

/* --------------------------------------------------------------------------------------------- */
/*                                         Gerber Recipe                                         */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 초기화 (생성)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CGridRecipe::Init()
{
	CGridGerb::InitGrid();

	/* 객체 생성 */
	m_pGrid = new CGridCtrl;
	ASSERT(m_pGrid);
	m_pGrid->SetDrawScrollBarVert(TRUE);
	m_pGrid->SetDrawScrollBarHorz(FALSE);
	if (!m_pGrid->Create(m_rGrid, m_pParent, IDC_GRID_GERB_GERBER_LIST, m_u32Style))
	{
		delete m_pGrid;
		return FALSE;
	}

	/* 객체 기본 설정 */
	m_pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	m_pGrid->SetColumnCount(1);
	m_pGrid->SetBkColor(RGB(255, 255, 255));

	/* 본문 */
	m_pGrid->SetColumnWidth(0, m_rGrid.Width()-20);

	/* 기본 속성 및 갱신 */
	m_pGrid->SetBaseGridProp(0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGrid->Refresh();

	/* 거버 레시피 적재 */
	Load();

	return TRUE;
}

/*
 desc : 레시피 적재
 parm : None
 retn : None
*/
VOID CGridRecipe::Load()
{
	INT32 i = 0, i32RowCnt	= uvEng_JobRecipe_GetCount();
	LPG_RJAF pstRecipe		= NULL;
	CUniToChar csCnv;

	/* 화면 갱신 비활성화 */
	m_pGrid->SetRedraw(FALSE);
	/* 기존 항목 모두 제거 */
	m_pGrid->DeleteNonFixedRows();

	if (i32RowCnt)
	{
		/* 항목 개수만큼 Row 생성 */
		m_pGrid->SetRowCount(i32RowCnt);
		/* 등록된 항목 다시 적재 */
		for (; i<i32RowCnt; i++)
		{
			m_pGrid->SetRowHeight(i, 30);
			pstRecipe	= uvEng_JobRecipe_GetRecipeIndex(i);
			m_pGrid->SetItemText(i, 0, csCnv.Ansi2Uni(pstRecipe->job_name));
			m_pGrid->SetItemFgColour(i, 0, m_stGV.crFgClr);
			m_pGrid->SetItemBkColour(i, 0, m_stGV.crBkClr);
		}
	}
	/* 오름 차순 정렬 */
	m_pGrid->SortItems(0, TRUE);
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
LPG_RJAF CGridRecipe::GetRecipe(INT32 index)
{
	TCHAR tzRecipe[RECIPE_NAME_LENGTH]	= {NULL};

	/* LED Power Name 얻기 */
	if (index < 0)	return NULL;
	wcscpy_s(tzRecipe, RECIPE_NAME_LENGTH, m_pGrid->GetItemText(index, 0));
	return uvEng_JobRecipe_GetRecipeOnlyName(tzRecipe);
}

/*
 desc : 현재 선택된 항목 반환
 parm : index	- [in]  선택된 위치 정보 (zero-based)
 retn : 선택된 정보가 저장된 구조체 포인터
*/
LPG_RJAF CGridRecipe::GetSelected()
{
	UINT32 i	= 0, u32State = 0;
	TCHAR tzRecipe[RECIPE_NAME_LENGTH]	= {NULL};

	/* LED Power Name 얻기 */
	for (; i<(UINT32)m_pGrid->GetRowCount(); i++)
	{
		/* 현재 위치 항목 속성 값 얻기 */
		u32State	= m_pGrid->GetItemState(i, 0);
		if (GVIS_SELECTED == (u32State & GVIS_SELECTED))
		{
			wcscpy_s(tzRecipe, RECIPE_NAME_LENGTH, m_pGrid->GetItemText(i, 0));
			return uvEng_JobRecipe_GetRecipeOnlyName(tzRecipe);
		}
	}

	return NULL;
}

/*
 desc : 거버 레시피 이름과 동일한 항목만 선택
 parm : recipe	- [in]  Gerber Recipe Name
 retn : None
*/
VOID CGridRecipe::Select(PTCHAR recipe)
{
	UINT32 i	= 0, u32State = 0;
	TCHAR tzRecipe[RECIPE_NAME_LENGTH]	= {NULL};

	/* 비활성화 */
	m_pGrid->SetRedraw(FALSE);
	for (; i<(UINT32)m_pGrid->GetRowCount(); i++)
	{
		/* 현재 위치 항목 속성 값 얻기 */
		u32State	= m_pGrid->GetItemState(i, 0);
		wmemset(tzRecipe, 0x00, RECIPE_NAME_LENGTH);
		wcscpy_s(tzRecipe, RECIPE_NAME_LENGTH, m_pGrid->GetItemText(i, 0));
		if (!recipe)	u32State &= ~GVIS_SELECTED;
		else
		{
			if (m_pGrid->GetItemText(i, 0).Compare(recipe))	u32State &= ~GVIS_SELECTED;
			else											u32State |= GVIS_SELECTED;
		}
		/* 항목 선택 여부 설정 */
		m_pGrid->SetItemState(i, 0, u32State);
	}
//	m_pGrid->SetItemState(0, 0, u32State |= GVIS_SELECTED);
	/* 활성화 */
	m_pGrid->SetRedraw(TRUE, TRUE);
	m_pGrid->Invalidate(FALSE);
}


/* --------------------------------------------------------------------------------------------- */
/*                                LED Power Grid Control - Items                                 */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 초기화 (생성)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CGridPowerI::Init()
{
	CGridGerb::InitGrid();

	CResizeUI clsResizeUI;
	/* Grid 크기 재설정 */
	m_rGrid.bottom	= m_rGrid.top + (m_rGrid.Height());
	// by sys&j : 화면 맞춤을 위해 수정
	m_rGrid.right	= m_rGrid.left + (m_rGrid.Width() / 2);
	// by sys&j : 화면 맞춤을 위해 수정

	/* 객체 생성 */
	m_pGrid = new CGridCtrl;
	ASSERT(m_pGrid);
	//m_pGrid->SetDrawScrollBarVert(TRUE);
	//m_pGrid->SetDrawScrollBarHorz(FALSE);
	if (!m_pGrid->Create(m_rGrid, m_pParent, IDC_GRID_GERB_LED_LIST, m_u32Style))
	{
		delete m_pGrid;
		return FALSE;
	}

	/* 객체 기본 설정 */
	m_pGrid->SetItem(&m_stGV);
	m_pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	m_pGrid->SetColumnCount(1);
	m_pGrid->SetBkColor(RGB(255, 255, 255));

	/* 본문 */
	m_pGrid->SetColumnWidth(0, m_rGrid.Width()-20);

	/* 기본 속성 및 갱신 */
	m_pGrid->SetBaseGridProp(0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGrid->Refresh();

	/* 거버 레시피 적재 */
	Load();

	return TRUE;
}

/*
 desc : 레시피 적재
 parm : None
 retn : None
*/
VOID CGridPowerI::Load()
{
	INT32 i = 0, i32RowCnt	= uvEng_LedPower_GetCount();
	LPG_PLPI pstPowerI		= NULL;
	CUniToChar csCnv;

	/* 화면 갱신 비활성화 */
	m_pGrid->SetRedraw(FALSE);
	/* 기존 항목 모두 제거 */
	m_pGrid->DeleteNonFixedRows();

	if (i32RowCnt)
	{
		/* 항목 개수만큼 Row 생성 */
		m_pGrid->SetRowCount(i32RowCnt);
		/* 등록된 항목 다시 적재 */
		for (; i<i32RowCnt; i++)
		{
			m_pGrid->SetRowHeight(i, 30);
			pstPowerI	= uvEng_LedPower_GetLedPowerIndex(i);
			m_pGrid->SetItemText(i, 0, csCnv.Ansi2Uni(pstPowerI->led_name));
			m_pGrid->SetItemFgColour(i, 0, m_stGV.crFgClr);
			m_pGrid->SetItemBkColour(i, 0, m_stGV.crBkClr);
		}
	}

	/* 화면 갱신 활성화 */
	m_pGrid->SetRedraw(TRUE, TRUE);
	/* 그리드 컨트롤 갱신 */
	m_pGrid->Invalidate();
}

/*
 desc : 입력 위치의 항목 반환
 parm : index	- [in]  선택된 위치 정보 (zero-based)
 retn : 선택된 정보가 저장된 구조체 포인터
*/
LPG_PLPI CGridPowerI::GetSelect(INT32 index)
{
	TCHAR tzRecipe[LED_POWER_NAME_LENGTH]	= {NULL};

	/* LED Power Name 얻기 */
	if (index < 0)	return NULL;
	wcscpy_s(tzRecipe, LED_POWER_NAME_LENGTH, m_pGrid->GetItemText(index, 0));
	return uvEng_LedPower_GetLedPowerName(tzRecipe);
}

/*
 desc : 현재 선택된 항목 반환
 parm : index	- [in]  선택된 위치 정보 (zero-based)
 retn : 선택된 정보가 저장된 구조체 포인터
*/
LPG_PLPI CGridPowerI::GetSelected()
{
	UINT32 i	= 0, u32State = 0;
	TCHAR tzRecipe[LED_POWER_NAME_LENGTH]	= {NULL};

	/* LED Power Name 얻기 */
	for (; i<(UINT32)m_pGrid->GetRowCount(); i++)
	{
		/* 현재 위치 항목 속성 값 얻기 */
		u32State	= m_pGrid->GetItemState(i, 0);
		if (GVIS_SELECTED == (u32State & GVIS_SELECTED))
		{
			wcscpy_s(tzRecipe, LED_POWER_NAME_LENGTH, m_pGrid->GetItemText(i, 0));
			return uvEng_LedPower_GetLedPowerName(tzRecipe);
		}
	}

	return NULL;
}

/*
 desc : LED Power 이름과 동일한 항목만 선택
 parm : recipe	- [in]  LED Power Recipe Name
 retn : None
*/
VOID CGridPowerI::Select(PTCHAR recipe)
{
	UINT32 i	= 0, u32State = 0;
	TCHAR tzRecipe[LED_POWER_NAME_LENGTH]	= {NULL};

	/* 비활성화 */
	m_pGrid->SetRedraw(FALSE);

	for (; i<(UINT32)m_pGrid->GetRowCount(); i++)
	{
		/* 현재 위치 항목 속성 값 얻기 */
		u32State	= m_pGrid->GetItemState(i, 0);
		wmemset(tzRecipe, 0x00, LED_POWER_NAME_LENGTH);
		wcscpy_s(tzRecipe, LED_POWER_NAME_LENGTH, m_pGrid->GetItemText(i, 0));
		if (m_pGrid->GetItemText(i, 0).Compare(recipe))	u32State &= ~GVIS_SELECTED;
		else											u32State |= GVIS_SELECTED;
		/* 항목 선택 여부 설정 */
		m_pGrid->SetItemState(i, 0, u32State);
	}

	/* 활성화 */
	m_pGrid->SetRedraw(TRUE, TRUE);
	m_pGrid->Invalidate(FALSE);
}

/* --------------------------------------------------------------------------------------------- */
/*                                 LED Power Grid Control - Values                               */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 초기화 (생성)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CGridPowerV::Init()
{
	CGridGerb::InitGrid();

	TCHAR tzTitle[5][16]= { L"No", L"365(W)", L"380(W)", L"395(W)", L"405(W)" };
	INT32 i, j, iColCnt = 5, iRowCnt = uvEng_GetConfig()->luria_svc.ph_count + 1;
	INT32 i32Width[5]	= { 25, 47, 47, 47, 47 };

	CResizeUI clsResizeUI;
	/* Grid 크기 재설정 */
	// by sys&j : 화면 맞춤을 위해 수정
	//m_rGrid.top	+= m_rGrid.Height() / 2 + 52;
	m_rGrid.bottom	= m_rGrid.top + (m_rGrid.Height());
	m_rGrid.right	= m_rGrid.left + (m_rGrid.Width() / 2);
	int nWidth = m_rGrid.Width();
	m_rGrid.left	= m_rGrid.right;
	m_rGrid.right	+= nWidth;

	// by sysandj : Grid Size 맞춤
	nWidth = 0;
	int nResizeWidth = 0;
	for (int i = 0; i < _countof(i32Width); i++)
	{
		nWidth += i32Width[i];
	}

	for (int i = 0; i < _countof(i32Width); i++)
	{
		i32Width[i] += (m_rGrid.Width() - nWidth) / _countof(i32Width);
		nResizeWidth += i32Width[i];
	}
	i32Width[0] += m_rGrid.Width() - nResizeWidth - 2;
	// by sysandj : Grid Size 맞춤
	// by sys&j : 화면 맞춤을 위해 수정

	/* 객체 생성 */
	m_pGrid = new CGridCtrl;
	ASSERT(m_pGrid);
	m_pGrid->SetDrawScrollBarVert(TRUE);
	m_pGrid->SetDrawScrollBarHorz(FALSE);
	if (!m_pGrid->Create(m_rGrid, m_pParent, IDC_GRID_GERB_LED_POWER, m_u32Style))
	{
		delete m_pGrid;
		return FALSE;	
	}

	/* 객체 기본 설정 */
	m_pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	m_pGrid->SetRowCount(iRowCnt);
	m_pGrid->SetColumnCount(iColCnt);
	//m_pGrid->SetFixedRowCount(1);
	m_pGrid->SetRowHeight(0, 25);
	m_pGrid->SetFixedColumnCount(0);
	m_pGrid->SetBkColor(RGB(255, 255, 255));
	m_pGrid->SetFixedColumnSelection(0);

	/* 타이틀 (첫 번째 행) */
	m_stGV.crBkClr = RGB(214, 214, 214);
	m_stGV.crFgClr = RGB(0, 0, 0);
	m_stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	for (i=0; i<iColCnt; i++)
	{
		m_stGV.row = 0;
		m_stGV.col = i;
		m_stGV.strText = tzTitle[i];
		m_pGrid->SetItem(&m_stGV);
		m_pGrid->SetColumnWidth(i, i32Width[i]);
		// by sysandj
		m_pGrid->SetItemBkColour(0, i, DEF_COLOR_BTN_MENU_NORMAL);
		m_pGrid->SetItemFgColour(0, i, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
		// by sysandj
		// 
//		m_pGrid->SetItemFont(0, i, &g_lf);
	}
	/* 본문 (1 ~ 8 Rows) */
	m_stGV.crBkClr = RGB(255, 255, 255);
	for (i=1; i<iRowCnt; i++)
	{
		m_pGrid->SetRowHeight(i, 25);
		/* Column (0 ~ 4) */
		for (j=0; j<iColCnt; j++)
		{
			m_stGV.nFormat = j != 0 ? DT_RIGHT|DT_VCENTER|DT_SINGLELINE : DT_CENTER|DT_VCENTER|DT_SINGLELINE;
			m_stGV.row = i;
			m_stGV.col = j;
			if (0 != j)	m_stGV.strText = L"";
			else		m_stGV.strText.Format(L"%d", i);
			m_pGrid->SetItem(&m_stGV);
//			m_pGrid->SetItemFont(i, j, &g_lf);
		}
	}

	m_pGrid->SetBaseGridProp(0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGrid->Refresh();

	return TRUE;
}

/*
 desc : 레시피 적재
 parm : None
 retn : None
*/
VOID CGridPowerV::SetRecipe(LPG_PLPI recipe)
{
	INT32 i = 0, i32RowCnt	= uvEng_GetConfig()->luria_svc.ph_count;

	/* 화면 갱신 비활성화 */
	m_pGrid->SetRedraw(FALSE);

	if (i32RowCnt > 0)
	{
		/* 항목 개수만큼 Row 생성 */
		m_pGrid->SetRowCount(i32RowCnt+1);
		/* 등록된 항목 다시 적재 */
		for (; i<=i32RowCnt; i++)
		{
			m_pGrid->SetRowHeight(i, 25);
			if (recipe)
			{
				m_pGrid->SetItemDouble(i+1, 1, recipe->led_watt[i][0], 3);
				m_pGrid->SetItemDouble(i+1, 2, recipe->led_watt[i][1], 3);
				m_pGrid->SetItemDouble(i+1, 3, recipe->led_watt[i][2], 3);
				m_pGrid->SetItemDouble(i+1, 4, recipe->led_watt[i][3], 3);
			}
			else
			{
				m_pGrid->SetItemText(i+1, 1, L"");
				m_pGrid->SetItemText(i+1, 2, L"");
				m_pGrid->SetItemText(i+1, 3, L"");
				m_pGrid->SetItemText(i+1, 4, L"");
			}
		}
	}
	/* 화면 갱신 활성화 */
	m_pGrid->SetRedraw(TRUE, TRUE);
	/* 그리드 컨트롤 갱신 */
	m_pGrid->Invalidate();
}

/*
 desc : 현재 행과 열에 출력된 값 반환
 parm : row	- [in]  행 값 (1 이상)
		col	- [in]  열 값 (1 이상)
 retn : Watt 값. 실패일 경우, 0.0 값 반환
*/
DOUBLE CGridPowerV::GetWatt(UINT8 row, UINT8 col)
{
	if (row < 1 || col < 1)	return 0.0f;
	if (row > uvEng_GetConfig()->luria_svc.ph_count)	return 0.0f;
	if (col > MAX_LED)		return 0.0f;

	return m_pGrid->GetItemTextToFloat(row, col);
}

/* --------------------------------------------------------------------------------------------- */
/*                          Material Thick Calibration for Align Camera                          */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 초기화 (생성)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CGridThick::Init()
{
	CGridGerb::InitGrid();

	/* 객체 생성 */
	m_pGrid = new CGridCtrl;
	ASSERT(m_pGrid);
	m_pGrid->SetDrawScrollBarVert(TRUE);
	m_pGrid->SetDrawScrollBarHorz(FALSE);
	if (!m_pGrid->Create(m_rGrid, m_pParent, IDC_GRID_GERB_MATERIAL_THICK, m_u32Style))
	{
		delete m_pGrid;
		return FALSE;
	}

	/* 객체 기본 설정 */
	m_pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	m_pGrid->SetColumnCount(1);
	m_pGrid->SetBkColor(RGB(255, 255, 255));

	/* 본문 */
	m_pGrid->SetColumnWidth(0, m_rGrid.Width()-20);

	/* 기본 속성 및 갱신 */
	m_pGrid->SetBaseGridProp(0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGrid->Refresh();

	/* 거버 레시피 적재 */
	Load();

	return TRUE;
}

/*
 desc : 레시피 적재
 parm : None
 retn : None
*/
VOID CGridThick::Load()
{
	INT32 i32RowCnt		= uvEng_ThickCali_GetCount(), i = 0;
	TCHAR tzValue[32]	= {NULL};
	LPG_MACP pstRecipe	= NULL;
	GV_ITEM m_stGV		= {NULL};

	/* 화면 갱신 비활성화 */
	m_pGrid->SetRedraw(FALSE);
	/* 기존 항목 모두 제거 */
	m_pGrid->DeleteNonFixedRows();

	if (i32RowCnt)
	{
		/* 항목 개수만큼 Row 생성 */
		m_pGrid->SetRowCount(i32RowCnt);
		/* 등록된 항목 다시 적재 */
		for (; i<i32RowCnt; i++)
		{
			m_pGrid->SetRowHeight(i, 30);
			pstRecipe	= uvEng_ThickCali_GetRecipeIndex(i);
			swprintf_s(tzValue, 32, L" %5u um", pstRecipe->film_thick_um);
			m_pGrid->SetItemText(i, 0, tzValue);
			m_pGrid->SetItemData(i, 0, UINT32(pstRecipe->film_thick_um));
			m_pGrid->SetItemFormat(i, 0, m_pGrid->GetItemFormat(i, 0) | DT_RIGHT);
		}
	}

	/* 화면 갱신 활성화 */
	m_pGrid->SetRedraw(TRUE, TRUE);
	/* 그리드 컨트롤 갱신 */
	m_pGrid->Invalidate(TRUE);
}

/*
 desc : 소재 두께와 동일한 값이 존재하는 항목 선택
 parm : thick	- [in]  소재 두께 (단위: um)
 retn : None
*/
VOID CGridThick::Select(UINT32 thick)
{
	UINT32 i	= 0, u32State = 0;

	/* 비활성화 */
	m_pGrid->SetRedraw(FALSE);

	for (; i<(UINT32)m_pGrid->GetRowCount(); i++)
	{
		/* 현재 위치 항목 속성 값 얻기 */
		u32State	= m_pGrid->GetItemState(i, 0);
		if (thick != m_pGrid->GetItemData(i, 0))	u32State &= ~GVIS_SELECTED;
		else										u32State |= GVIS_SELECTED;
		/* 항목 선택 여부 설정 */
		m_pGrid->SetItemState(i, 0, u32State);
	}

	/* 활성화 */
	m_pGrid->SetRedraw(TRUE, TRUE);
	m_pGrid->Invalidate(FALSE);
}

/*
 desc : 현재 선택된 항목의 값 반환
 parm : None
 retn : 소재 두께 값 반환 (단위: um)
*/
UINT32 CGridThick::GetSelectThick()
{
	UINT32 i	= 0, u32State = 0;

	for (; i<(UINT32)m_pGrid->GetRowCount(); i++)
	{
		/* 현재 위치 항목 속성 값 얻기 */
		u32State	= m_pGrid->GetItemState(i, 0);
		if (GVIS_SELECTED == (u32State & GVIS_SELECTED))
		{
			return (UINT32)m_pGrid->GetItemData(i, 0);
		}
	}

	return 0;
}
