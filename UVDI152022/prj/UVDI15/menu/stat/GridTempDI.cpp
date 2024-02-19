/*
 desc : DI ���� �µ�
 */

#include "pch.h"
#include "../../MainApp.h"
#include "GridTempDI.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif

/*
 desc : ������
 parm : parent	- [in]  �ڽ��� ȣ���� �θ� �ڵ�
		grid	- [in]  Grid Control�� ��µ� ���� �ڵ�
 retn : None
*/
CGridTempDI::CGridTempDI(HWND parent, HWND grid)
{
	m_pGrid		= NULL;
	m_pParent	= CWnd::FromHandle(parent);
	m_pCtrl		= CWnd::FromHandle(grid);
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CGridTempDI::~CGridTempDI()
{
	if (m_pGrid)
	{
		if (m_pGrid->GetSafeHwnd())	m_pGrid->DestroyWindow();
		delete m_pGrid;
	}
	for (UINT8 i=0x00; i<4; i++)	m_lstTemp[i].RemoveAll();
}

/*
 desc : �ʱ�ȭ (����)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CGridTempDI::Init()
{
	TCHAR tzCol[5][16]	= { L"Item", L"Sensor 1", L"Sensor 2", L"Sensor 3", L"Sensor 4" };
	TCHAR tzRow[4][16]	= { L"Now", L"Avg", L"Min", L"Max" };
	INT32 i, j, iColCnt = 5, iRowCnt = 5;
	INT32 i32Width[5]	= { 50, 80, 80, 80, 80 };
	UINT32 dwFormat		= DT_VCENTER | DT_SINGLELINE | DT_RIGHT, u32Style;
	GV_ITEM stItem		= {NULL};

	/* �׸��� ��Ʈ�� �⺻ ���� �Ӽ� */
	u32Style		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	stItem.mask		= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stItem.nFormat	= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	stItem.crFgClr	= RGB(0, 0, 0);
	stItem.crBkClr	= RGB(240, 240, 240);
	stItem.lfFont = g_lf[eFONT_LEVEL2_BOLD];

	/* ���� ���� Client ũ�� */
	m_pCtrl->GetWindowRect(m_rGrid);
	m_pParent->ScreenToClient(m_rGrid);

	CResizeUI clsResizeUI;
	/* Grid ũ�� �缳�� */
	m_rGrid.left	+= (long)(15 * clsResizeUI.GetRateX());
	m_rGrid.top		+= (long)(23 * clsResizeUI.GetRateY());
	m_rGrid.right	-= (long)(15 * clsResizeUI.GetRateX());
	m_rGrid.bottom	-= (long)(15 * clsResizeUI.GetRateY());

	/* ��ü ���� */
	m_pGrid = new CGridCtrl;
	ASSERT(m_pGrid);
	m_pGrid->SetDrawScrollBarVert(FALSE);
	m_pGrid->SetDrawScrollBarHorz(FALSE);
	if (!m_pGrid->Create(m_rGrid, m_pParent, IDC_GRID_STAT_PLC_DI_TEMP, u32Style))
	{
		delete m_pGrid;
		m_pGrid = NULL;
		return FALSE;
	}

	// by sysandj : Grid Size ����
	int nWidth = 0;
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

	m_rGrid.bottom += (iRowCnt * 25) - m_rGrid.Height();
	m_pGrid->MoveWindow(m_rGrid);
	// by sysandj : Grid Size ����

	/* ��ü �⺻ ���� */
	m_pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	m_pGrid->SetRowCount(iRowCnt);
	m_pGrid->SetColumnCount(iColCnt);
	//m_pGrid->SetFixedRowCount(1);
	//m_pGrid->SetFixedColumnCount(1);
	m_pGrid->SetRowHeight(0, 24);
	m_pGrid->SetFixedColumnCount(0);
	m_pGrid->SetBkColor(RGB(255, 255, 255));
	m_pGrid->SetFixedColumnSelection(0);

	/* Ÿ��Ʋ (���� ��) */
	for (i=0; i<iColCnt; i++)
	{
		stItem.row = 0;
		stItem.col = i;
		stItem.strText = tzCol[i];
		m_pGrid->SetItem(&stItem);
		m_pGrid->SetColumnWidth(i, i32Width[i]);

		// by sysandj
		m_pGrid->SetItemBkColour(0, i, DEF_COLOR_BTN_MENU_NORMAL);
		m_pGrid->SetItemFgColour(0, i, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
		// by sysandj
	}
	/* Ÿ��Ʋ (���� ��) */
	for (i=1; i<iRowCnt; i++)
	{
		m_pGrid->SetRowHeight(i, 25);
		stItem.row	= i;
		stItem.col	= 0;
		stItem.strText.Format(L"%s", tzRow[i-1]);
		m_pGrid->SetItem(&stItem);

		// by sysandj
		m_pGrid->SetItemBkColour(i, 0, DEF_COLOR_BTN_MENU_NORMAL);
		m_pGrid->SetItemFgColour(i, 0, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
		// by sysandj
	}
	stItem.crBkClr = RGB(255, 255, 255);
	for (i=1; i<iRowCnt; i++)
	{
		for (j=1; j<iColCnt; j++)
		{
			stItem.nFormat	= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
			stItem.row		= i;
			stItem.col		= j;
			stItem.strText	= L"";
			m_pGrid->SetItem(&stItem);
		}
	}

	m_pGrid->SetBaseGridProp(0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGrid->Refresh();
#if 1
	/* �ٷ� �� ���� */
	Update();
#endif
	return TRUE;
}

/*
 desc : ���� �� ����
 parm : None
 retn : ���õ� ������ ����� ����ü ������
*/
VOID CGridTempDI::Update()
{
	UINT32 i = 0;
	TCHAR tzValue[64]	= { NULL };

	/* ȭ�� ���� ��Ȱ��ȭ */
	m_pGrid->SetRedraw(FALSE);

	for (i=0; i<4; i++)
	{
		/* Current Temperature */
		swprintf_s(tzValue, 64, L"%.3f ��c ", uvEng_PLC_GetTempDI(i));
		m_pGrid->SetItemText(1, i+1, tzValue);
		/* Average Temperature */
		swprintf_s(tzValue, 64, L"%.3f ��c ", uvEng_PLC_GetTempDIAvg(i));
		m_pGrid->SetItemText(2, i+1, tzValue);
		/* Min Temperature */
		swprintf_s(tzValue, 64, L"%.3f ��c ", uvEng_PLC_GetTempDIMin(i));
		m_pGrid->SetItemText(3, i+1, tzValue);
		/* Max Temperature */
		swprintf_s(tzValue, 64, L"%.3f ��c ", uvEng_PLC_GetTempDIMax(i));
		m_pGrid->SetItemText(4, i+1, tzValue);
	}

	/* ȭ�� ���� Ȱ��ȭ */
	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate();
}