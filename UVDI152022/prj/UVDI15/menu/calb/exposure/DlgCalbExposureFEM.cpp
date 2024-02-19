// DlgCalb.cpp: 구현 파일
//

#include "pch.h"
#include "DlgCalbExposureFEM.h"
#include "afxdialogex.h"
#include <iostream>
#include <string>

/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgCalbExposureFEM::CDlgCalbExposureFEM(UINT32 id, CWnd* parent)
	: CDlgSubTab(id, parent)
{
	m_enDlgID = ENG_CMDI_TAB::en_menu_tab_calb_exposure_fem;
	m_enLastJobLoadStatus = ENG_LSLS::en_load_none;
	m_n8SelJob = -1;

	for (int i = 0; i < eCALB_EXPOSURE_FEM_TIMER_MAX; i++)
	{
		m_bBlink[i] = FALSE;
	}

	m_nPrintRow = -1;
	m_nPrintCol = -1;
	m_bPrinting = FALSE;
}

CDlgCalbExposureFEM::~CDlgCalbExposureFEM()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgCalbExposureFEM::DoDataExchange(CDataExchange* dx)
{
	CDlgSubTab::DoDataExchange(dx);

	UINT32 i, u32StartID;

	u32StartID = IDC_CALB_EXPOSURE_FEM_BTN_START;
	for (i = 0; i < eCALB_EXPOSURE_FEM_BTN_MAX; i++)		DDX_Control(dx, u32StartID + i, m_btn_ctl[i]);

	u32StartID = IDC_CALB_EXPOSURE_FEM_STT_OPERATION;
	for (i = 0; i < eCALB_EXPOSURE_FEM_STT_MAX; i++)		DDX_Control(dx, u32StartID + i, m_stt_ctl[i]);

	u32StartID = IDC_CALB_EXPOSURE_FEM_TXT_STATE;
	for (i = 0; i < eCALB_EXPOSURE_FEM_TXT_MAX; i++)		DDX_Control(dx, u32StartID + i, m_txt_ctl[i]);

	u32StartID = IDC_CALB_EXPOSURE_FEM_PGR_RATE;
	for (i = 0; i < eCALB_EXPOSURE_FEM_PGR_MAX; i++)		DDX_Control(dx, u32StartID + i, m_pgr_ctl[i]);

	u32StartID = IDC_CALB_EXPOSURE_FEM_GRD_RESULT;
	for (i = 0; i < eCALB_EXPOSURE_FEM_GRD_MAX; i++)		DDX_Control(dx, u32StartID + i, m_grd_ctl[i]);

}

BEGIN_MESSAGE_MAP(CDlgCalbExposureFEM, CDlgSubTab)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CALB_EXPOSURE_FEM_BTN_START, IDC_CALB_EXPOSURE_FEM_BTN_START + eCALB_EXPOSURE_FEM_BTN_MAX, OnBtnClick)
	ON_NOTIFY(NM_CLICK, IDC_CALB_EXPOSURE_FEM_GRD_RESULT, &CDlgCalbExposureFEM::OnClickGridOpticResult)
	ON_NOTIFY(NM_CLICK, IDC_CALB_EXPOSURE_FEM_GRD_OPTIC_STATE, &CDlgCalbExposureFEM::OnClickGridOpticState)
	ON_NOTIFY(NM_CLICK, IDC_CALB_EXPOSURE_FEM_GRD_PRINT_OPTION, &CDlgCalbExposureFEM::OnClickGridPrintOption)
	ON_NOTIFY(NM_CLICK, IDC_CALB_EXPOSURE_FEM_GRD_POSITION, &CDlgCalbExposureFEM::OnClickGridPosition)
	ON_NOTIFY(NM_CLICK, IDC_CALB_EXPOSURE_FEM_GRD_EXPOSURE_OPTION, &CDlgCalbExposureFEM::OnClickGridExposureOption)
	ON_NOTIFY(NM_CLICK, IDC_CALB_EXPOSURE_FEM_GRD_JOBS, &CDlgCalbExposureFEM::OnClickGridJobs)
	ON_NOTIFY(NM_CLICK, IDC_CALB_EXPOSURE_FEM_GRD_JOB_STATE, &CDlgCalbExposureFEM::OnClickGridJobState)
	ON_NOTIFY(NM_CLICK, IDC_CALB_EXPOSURE_FEM_GRD_HEAD_FOCUS, &CDlgCalbExposureFEM::OnClickGridHeadFocus)
	ON_NOTIFY(NM_CLICK, IDC_CALB_EXPOSURE_FEM_GRD_PRINT_TYPE, &CDlgCalbExposureFEM::OnClickGridPrintType)
	ON_NOTIFY(NM_CLICK, IDC_CALB_EXPOSURE_FEM_GRD_HEAD_FOCUS_CTRL, &CDlgCalbExposureFEM::OnClickGridHeadFocusCtrl)
	ON_MESSAGE(eMSG_EXPO_FEM_PRINT_SET_POINT, InputSetPoint)
	ON_MESSAGE(eMSG_EXPO_FEM_PRINT_PRINT_COMP, InputPrintCompl)
	ON_MESSAGE(eMSG_EXPO_FEM_RESULT_PROGRESS, ReportResult)
	ON_WM_TIMER()
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgCalbExposureFEM::PreTranslateMessage(MSG* msg)
{
	if (msg->message == WM_KEYDOWN)
	{
		if (msg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
	}
	return CDlgSubTab::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgCalbExposureFEM::OnInitDlg()
{
	/* 컨트롤 초기화 */
	InitCtrl();

	InitGridResult(1, 1, TRUE);
	InitGridOpticState();
	InitGridPrintOption();
	InitGridPosition();
	InitGridExpoOption();
	InitGridJobs();
	InitGridJobState();
	InitGridHeadFocus();
	InitGridPrintType();
	InitGridHeadFocusCtrl();

	LoadDataConfig();
	UpdateJobList();
	RefreshResultCell();

	SetTimer(eCALB_EXPOSURE_FEM_TIMER_UPDATE_VALUE, 100, NULL);
	SetTimer(eCALB_EXPOSURE_FEM_TIMER_REQ_LURIA_LOAD_STATE, 250, NULL);
	SetTimer(eCALB_EXPOSURE_FEM_TIMER_PRINT_PROGRESS, 500, NULL);

	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgCalbExposureFEM::OnExitDlg()
{
	for (int i = 0; i < eCALB_EXPOSURE_FEM_TIMER_MAX; i++)
	{
		KillTimer(i);
	}

	for (int i = 0; i < _countof(m_grd_ctl); i++)
	{
		m_grd_ctl[i].DeleteAllItems();
	}
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgCalbExposureFEM::OnPaintDlg(CDC* dc)
{

	/* 자식 윈도 호출 */
	CDlgSubTab::DrawOutLine();
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgCalbExposureFEM::OnResizeDlg()
{
}

/*
 desc : 부모 스레드에 의해 주기적으로 호출됨
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgCalbExposureFEM::UpdatePeriod(UINT64 tick, BOOL is_busy)
{

	/* Control Enable or Disable */
	UpdateControl(tick, is_busy);
}

/*
 desc : 컨트롤
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgCalbExposureFEM::UpdateControl(UINT64 tick, BOOL is_busy)
{
}


/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgCalbExposureFEM::InitCtrl()
{
	CResizeUI clsResizeUI;
	CString strTemp;
	CRect rStt;

	for (int i = 0; i < eCALB_EXPOSURE_FEM_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
		// by sysandj : Resize UI
	}

	/* static - Normal */
	for (int i = 0; i < eCALB_EXPOSURE_FEM_STT_MAX; i++)
	{
		m_stt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_stt_ctl[i].SetDrawBg(1);
		m_stt_ctl[i].SetBaseProp(0, 1, 0, 0, DEF_COLOR_BTN_MENU_NORMAL, 0, WHITE_);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_stt_ctl[i]);
		// by sysandj : Resize UI

		m_stt_ctl[i].GetWindowRect(rStt);
		this->ScreenToClient(rStt);
		rStt.right += 2;

		m_stt_ctl[i].MoveWindow(rStt);
	}

	/* static - Normal */
	for (int i = 0; i < eCALB_EXPOSURE_FEM_TXT_MAX; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_txt_ctl[i]);
		// by sysandj : Resize UI
	}

	for (int i = 0; i < eCALB_EXPOSURE_FEM_PGR_MAX; i++)
	{
		m_pgr_ctl[i].SetBkColor(ALICE_BLUE);
		m_pgr_ctl[i].SetTextColor(BLACK_);
		m_pgr_ctl[i].SetRange(0, 100);
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_pgr_ctl[i]);
		// by sysandj : Resize UI
	}

	m_pgr_ctl[eCALB_EXPOSURE_FEM_PGR_RATE].SetShowPercent();

	for (int i = 0; i < eCALB_EXPOSURE_FEM_GRD_MAX; i++)
	{
		m_grd_ctl[i].SetParent(this);
	}
}


VOID CDlgCalbExposureFEM::InitGridResult(int nRowCount, int nColCount, BOOL bInit)
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_RESULT];

	(TRUE == bInit) ? clsResizeUI.ResizeControl(this, pGrid) : NULL;
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	ASSERT(nRowCount > 0 && nColCount > 0);

	int nHeight = (int)(rGrid.Height() / nRowCount);
	int nWidth = (int)(rGrid.Width() / nColCount);

	int nDiffHeight = rGrid.Height() - (nHeight * nRowCount);
	int nDiffWidth = rGrid.Width() - (nWidth * nColCount);

	/* 객체 기본 설정 */
	pGrid->SetRowCount(nRowCount);
	pGrid->SetColumnCount(nColCount);
	pGrid->SetFixedColumnCount(0);
	pGrid->SetBkColor(RGB(255, 255, 255));
	pGrid->SetFixedColumnSelection(0);

	for (int nRow = 0; nRow < nRowCount; nRow++)
	{
		if (nRow < nDiffHeight)
		{
			pGrid->SetRowHeight(nRow, nHeight + 1);
		}
		else
		{
			pGrid->SetRowHeight(nRow, nHeight);
		}

		for (int nCol = 0; nCol < nColCount; nCol++)
		{
			pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			pGrid->SetItemState(nRow, nCol, GVIS_READONLY);
			pGrid->SetItemBkColour(nRow, nCol, WHITE_);

			if (nCol < nDiffWidth)
			{
				pGrid->SetColumnWidth(nCol, nWidth + 1);
			}
			else
			{
				pGrid->SetColumnWidth(nCol, nWidth);
			}
		}
	}

	/* 기본 속성 및 갱신 */
	pGrid->EnableDragAndDrop(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}

VOID CDlgCalbExposureFEM::InitGridOpticState()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <COLORREF>		vColColor = { ALICE_BLUE, WHITE_, LIGHT_GRAY, LIGHT_GRAY };
	std::vector <int>			vColSize(4);
	std::vector <std::wstring>	vTitle = { _T("Optic Error"), _T("0"), _T("INIT"), _T("RESET") };

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_OPTIC_STATE];

	clsResizeUI.ResizeControl(this, pGrid);
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	int nHeight = (int)(DEF_DEFAULT_GRID_ROW_SIZE * clsResizeUI.GetRateY());

	int nTotalWidth = 0;
	for (auto& width : vColSize)
	{
		width = rGrid.Width() / (int)vColSize.size() - 20;
		nTotalWidth += width;
	}

	vColSize.front() += rGrid.Width() - nTotalWidth - 1;

	/* 객체 기본 설정 */
	pGrid->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
	pGrid->SetRowCount(1);
	pGrid->SetColumnCount((int)vColSize.size());
	pGrid->SetFixedColumnCount(0);
	pGrid->SetBkColor(RGB(255, 255, 255));
	pGrid->SetFixedColumnSelection(0);

	for (int nRow = 0; nRow < 1; nRow++)
	{
		pGrid->SetRowHeight(nRow, nHeight);

		for (int nCol = 0; nCol < (int)vColSize.size(); nCol++)
		{
			pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			pGrid->SetColumnWidth(nCol, vColSize[nCol]);
			pGrid->SetItemText(nRow, nCol, vTitle[nCol].c_str());
			pGrid->SetItemBkColour(nRow, nCol, vColColor[nCol]);
			pGrid->SetItemState(nRow, nCol, GVIS_READONLY);
		}
	}

	/* 기본 속성 및 갱신 */
	pGrid->EnableDragAndDrop(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}

VOID CDlgCalbExposureFEM::InitGridPrintOption()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <COLORREF>		vColColor = { ALICE_BLUE, WHITE_, ALICE_BLUE };
	std::vector <int>			vColSize(3);
	std::vector <int>			vRowSize(4);
	std::vector <std::wstring>	vTitle[4];

	vTitle[0] = {	_T("Printing Count X"),		_T("1"),			_T("Cnt") };
	vTitle[1] = {	_T("Printing Count Y"),		_T("1"),			_T("Cnt") };
	vTitle[2] = {	_T("Period X"),				_T("0.0000"),		_T("mm") };
	vTitle[3] = {	_T("Period Y"),				_T("0.0000"),		_T("mm") };

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_PRINT_OPTION];

	clsResizeUI.ResizeControl(this, pGrid);
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	int nHeight = (int)(DEF_DEFAULT_GRID_ROW_SIZE * clsResizeUI.GetRateY());
	int nTotalHeight = 0;
	for (auto& height : vRowSize)
	{
		height = nHeight;
		nTotalHeight += height;
	}

	if (rGrid.Height() < nTotalHeight)
	{
		nTotalHeight = 0;

		for (auto& height : vRowSize)
		{
			height = (rGrid.Height()) / (int)vRowSize.size();
			nTotalHeight += height;
		}
	}

	int nTotalWidth = 0;
	for (auto& width : vColSize)
	{
		width = rGrid.Width() / (int)vColSize.size() - 50;
		nTotalWidth += width;
	}

	vColSize.front() += rGrid.Width() - nTotalWidth - 2;

	/* 객체 기본 설정 */
	pGrid->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
	pGrid->SetRowCount((int)vRowSize.size());
	pGrid->SetColumnCount((int)vColSize.size());
	pGrid->SetFixedColumnCount(0);
	pGrid->SetBkColor(RGB(255, 255, 255));
	pGrid->SetFixedColumnSelection(0);

	for (int nRow = 0; nRow < (int)vRowSize.size(); nRow++)
	{
		pGrid->SetRowHeight(nRow, vRowSize[nRow]);

		for (int nCol = 0; nCol < (int)vColSize.size(); nCol++)
		{
			pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			pGrid->SetColumnWidth(nCol, vColSize[nCol]);
			pGrid->SetItemText(nRow, nCol, vTitle[nRow][nCol].c_str());
			pGrid->SetItemBkColour(nRow, nCol, vColColor[nCol]);
			pGrid->SetItemState(nRow, nCol, GVIS_READONLY);
		}
	}

	/* Grid Size 재구성 */
	rGrid.bottom = rGrid.top + nTotalHeight + 1;
	pGrid->MoveWindow(rGrid);

	/* 기본 속성 및 갱신 */
	pGrid->EnableDragAndDrop(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}

VOID CDlgCalbExposureFEM::InitGridPosition()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <int>			vColSize(3);
	std::vector <int>			vRowSize(3);
	std::vector <std::wstring>	vTitle[3];

	vTitle[0] = { _T("ITEM"),		_T("X [mm]"),	_T("Y [mm]") };
	vTitle[1] = { _T("Exposure"),	_T("0.0000"),	_T("0.0000") };
	vTitle[2] = { _T("Unload"),		_T("0.0000"),	_T("0.0000") };

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_POSITION];

	clsResizeUI.ResizeControl(this, pGrid);
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	int nHeight = (int)(DEF_DEFAULT_GRID_ROW_SIZE * clsResizeUI.GetRateY());
	int nTotalHeight = 0;
	for (auto& height : vRowSize)
	{
		height = nHeight;
		nTotalHeight += height;
	}

	if (rGrid.Height() < nTotalHeight)
	{
		nTotalHeight = 0;

		for (auto& height : vRowSize)
		{
			height = (rGrid.Height()) / (int)vRowSize.size();
			nTotalHeight += height;
		}
	}

	int nTotalWidth = 0;
	for (auto& width : vColSize)
	{
		width = rGrid.Width() / (int)vColSize.size() - 10;
		nTotalWidth += width;
	}

	vColSize.front() += rGrid.Width() - nTotalWidth - 2;

	/* 객체 기본 설정 */
	pGrid->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
	pGrid->SetRowCount((int)vRowSize.size());
	pGrid->SetColumnCount((int)vColSize.size());
	pGrid->SetFixedColumnCount(0);
	pGrid->SetBkColor(RGB(255, 255, 255));
	pGrid->SetFixedColumnSelection(0);

	for (int nRow = 0; nRow < (int)vRowSize.size(); nRow++)
	{
		pGrid->SetRowHeight(nRow, vRowSize[nRow]);

		for (int nCol = 0; nCol < (int)vColSize.size(); nCol++)
		{
			if (0 == nRow || 0 == nCol)
			{
				pGrid->SetItemBkColour(nRow, nCol, ALICE_BLUE);
			}

			pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			pGrid->SetColumnWidth(nCol, vColSize[nCol]);
			pGrid->SetItemText(nRow, nCol, vTitle[nRow][nCol].c_str());
			pGrid->SetItemState(nRow, nCol, GVIS_READONLY);
		}
	}

	/* Grid Size 재구성 */
	rGrid.bottom = rGrid.top + nTotalHeight + 1;
	pGrid->MoveWindow(rGrid);

	/* 기본 속성 및 갱신 */
	pGrid->EnableDragAndDrop(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}

VOID CDlgCalbExposureFEM::InitGridExpoOption()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <COLORREF>		vColColor = { ALICE_BLUE, WHITE_, ALICE_BLUE };
	std::vector <int>			vColSize(eGRD_EXPO_COL_MAX);
	std::vector <int>			vRowSize(eGRD_EXPO_ROW_MAX);
	std::vector <std::wstring>	vTitle[eGRD_EXPO_ROW_MAX];

	vTitle[0] = { _T("Step Focus"),		_T("0.0000"),	_T("mm") };
	vTitle[1] = { _T("Start Energy"),	_T("0.000"),	_T("mJ") };
	vTitle[2] = { _T("Step Energy"),	_T("0.000"),	_T("mJ") };
	vTitle[3] = { _T("Max Energy"),		_T("0.000"),	_T("mJ") };

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_EXPOSURE_OPTION];

	clsResizeUI.ResizeControl(this, pGrid);
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	int nHeight = (int)(DEF_DEFAULT_GRID_ROW_SIZE * clsResizeUI.GetRateY());
	int nTotalHeight = 0;
	for (auto& height : vRowSize)
	{
		height = nHeight;
		nTotalHeight += height;
	}

	if (rGrid.Height() < nTotalHeight)
	{
		nTotalHeight = 0;

		for (auto& height : vRowSize)
		{
			height = (rGrid.Height()) / (int)vRowSize.size();
			nTotalHeight += height;
		}
	}

	int nTotalWidth = 0;
	for (auto& width : vColSize)
	{
		width = rGrid.Width() / (int)vColSize.size() - 50;
		nTotalWidth += width;
	}

	vColSize.front() += rGrid.Width() - nTotalWidth - 2;

	/* 객체 기본 설정 */
	pGrid->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
	pGrid->SetRowCount((int)vRowSize.size());
	pGrid->SetColumnCount((int)vColSize.size());
	pGrid->SetFixedColumnCount(0);
	pGrid->SetBkColor(BLACK_);
	pGrid->SetFixedColumnSelection(0);

	for (int nRow = 0; nRow < (int)vRowSize.size(); nRow++)
	{
		pGrid->SetRowHeight(nRow, vRowSize[nRow]);

		for (int nCol = 0; nCol < (int)vColSize.size(); nCol++)
		{
			pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			pGrid->SetColumnWidth(nCol, vColSize[nCol]);
			pGrid->SetItemText(nRow, nCol, vTitle[nRow][nCol].c_str());
			pGrid->SetItemBkColour(nRow, nCol, vColColor[nCol]);
			pGrid->SetItemState(nRow, nCol, GVIS_READONLY);
		}
	}

	/* Grid Size 재구성 */
	rGrid.bottom = rGrid.top + nTotalHeight + 1;
	pGrid->MoveWindow(rGrid);

	/* 기본 속성 및 갱신 */
	pGrid->EnableDragAndDrop(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}

VOID CDlgCalbExposureFEM::InitGridJobs()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_JOBS];

	clsResizeUI.ResizeControl(this, pGrid);
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	int nHeight = rGrid.Height() - 1;
	int nWidth = rGrid.Width() - 1;

	/* 객체 기본 설정 */
	pGrid->SetRowCount(1);
	pGrid->SetColumnCount(1);
	pGrid->SetFixedColumnCount(0);
	pGrid->SetBkColor(WHITE_);
	pGrid->SetFixedColumnSelection(0);

	pGrid->SetItemFormat(0, 0, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pGrid->SetRowHeight(0, nHeight);
	pGrid->SetColumnWidth(0, nWidth);
	pGrid->SetItemState(0, 0, GVIS_READONLY);

	/* 기본 속성 및 갱신 */
	pGrid->EnableDragAndDrop(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}

VOID CDlgCalbExposureFEM::InitGridJobState()
{

	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <COLORREF>		vColColor = { ALICE_BLUE, WHITE_ };
	std::vector <int>			vColSize(2);
	std::vector <std::wstring>	vTitle = { _T("Status"), _T("None") };

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_JOB_STATE];

	clsResizeUI.ResizeControl(this, pGrid);
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	int nHeight = (int)(DEF_DEFAULT_GRID_ROW_SIZE * clsResizeUI.GetRateY());

	int nTotalWidth = 0;
	for (auto& width : vColSize)
	{
		width = rGrid.Width() / (int)vColSize.size() - 20;
		nTotalWidth += width;
	}

	vColSize.front() += rGrid.Width() - nTotalWidth - 1;

	/* 객체 기본 설정 */
	pGrid->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
	pGrid->SetRowCount(1);
	pGrid->SetColumnCount((int)vColSize.size());
	pGrid->SetFixedColumnCount(0);
	pGrid->SetBkColor(BLACK_);
	pGrid->SetFixedColumnSelection(0);

	for (int nRow = 0; nRow < 1; nRow++)
	{
		pGrid->SetRowHeight(nRow, nHeight);

		for (int nCol = 0; nCol < (int)vColSize.size(); nCol++)
		{
			pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			pGrid->SetColumnWidth(nCol, vColSize[nCol]);
			pGrid->SetItemText(nRow, nCol, vTitle[nCol].c_str());
			pGrid->SetItemBkColour(nRow, nCol, vColColor[nCol]);
			pGrid->SetItemState(nRow, nCol, GVIS_READONLY);
		}
	}

	/* 기본 속성 및 갱신 */
	pGrid->EnableDragAndDrop(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}


VOID CDlgCalbExposureFEM::InitGridHeadFocus()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	int nHeadCount = uvEng_GetConfig()->luria_svc.ph_count;
	std::vector <int>			vRowSize(nHeadCount + 1);
	std::vector <int>			vColSize(eGRD_FOCUS_COL_MAX);
	std::vector <std::wstring>	vTitle = { _T("PH INDEX"), _T("START FOCUS [mm]"), _T("END FOCUS [mm]") };
	std::vector <std::wstring>	vItem = { _T("HEAD"), _T("0.0000"), _T("0.0000") };
	std::vector <COLORREF>		vColColor = { ALICE_BLUE, WHITE_, LIGHT_GRAY };
	std::vector <COLORREF>		vColTextColor = { BLACK_, BLACK_, BLACK_ };

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_HEAD_FOCUS];

	clsResizeUI.ResizeControl(this, pGrid);
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	int nHeight = (int)(DEF_DEFAULT_GRID_ROW_SIZE * clsResizeUI.GetRateY() - 1);
	int nTotalHeight = 0;
	int nTotalWidth = 0;

	for (auto& height : vRowSize)
	{
		height = nHeight;
		nTotalHeight += height;
	}

	if (rGrid.Height() < nTotalHeight)
	{
		nTotalHeight = 0;

		for (auto& height : vRowSize)
		{
			height = (rGrid.Height() - 1) / (int)vRowSize.size();
			nTotalHeight += height;
		}
	}

	for (auto& width : vColSize)
	{
		width = (rGrid.Width() - 1) / (int)vColSize.size();
		nTotalWidth += width;
	}

	vColSize.front() += (rGrid.Width() - 1) - nTotalWidth;

	/* 객체 기본 설정 */
	pGrid->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
	pGrid->SetRowCount((int)vRowSize.size());
	pGrid->SetColumnCount((int)vColSize.size());
	pGrid->SetFixedColumnCount(0);
	pGrid->SetBkColor(RGB(255, 255, 255));
	pGrid->SetFixedColumnSelection(0);

	/* 타이틀 (첫 번째 행) */
	for (int i = 0; i < (int)vColSize.size(); i++)
	{
		pGrid->SetItemFormat(0, i, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pGrid->SetColumnWidth(i, vColSize[i]);
	}

	for (int nRow = 0; nRow < (int)vRowSize.size(); nRow++)
	{
		pGrid->SetRowHeight(nRow, vRowSize[nRow]);

		for (int nCol = 0; nCol < (int)vColSize.size(); nCol++)
		{
			if (0 == nRow)
			{
				pGrid->SetItemTextFmt(nRow, nCol, _T("%s"), vTitle[nCol].c_str());

				pGrid->SetItemBkColour(nRow, nCol, vColColor[0]);
				pGrid->SetItemFgColour(nRow, nCol, vColTextColor[0]);
			}
			else
			{
				if (0 == nCol)
				{
					pGrid->SetItemTextFmt(nRow, nCol, _T("%s%d"), vItem[nCol].c_str(), nRow);
				}
				else
				{
					pGrid->SetItemTextFmt(nRow, nCol, _T("%s"), vItem[nCol].c_str());
				}

				pGrid->SetItemBkColour(nRow, nCol, vColColor[nCol]);
				pGrid->SetItemFgColour(nRow, nCol, vColTextColor[nCol]);
			}

			pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
	}

	/* Grid Size 재구성 */
	rGrid.bottom = rGrid.top + nTotalHeight + 1;
	pGrid->MoveWindow(rGrid);

	/* 기본 속성 및 갱신 */
	pGrid->SetBaseGridProp(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}


VOID CDlgCalbExposureFEM::InitGridPrintType()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <int>			vRowSize(eGRD_TYPE_ROW_MAX);
	std::vector <int>			vColSize(eGRD_TYPE_COL_MAX);
	std::vector <std::wstring>	vTitle = { _T("X : FOCUS, Y : ENERGY"), _T("X : ENERGY, Y : FOCUS") };
	std::vector <COLORREF>		vColColor(eGRD_TYPE_COL_MAX);
	int nMode = (int)CCamSpecMgr::GetInstance()->GetRunMode();

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_PRINT_TYPE];

	clsResizeUI.ResizeControl(this, pGrid);
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	int nHeight = (int)(DEF_DEFAULT_GRID_ROW_SIZE * clsResizeUI.GetRateY());
	int nTotalHeight = 0;
	for (auto& height : vRowSize)
	{
		height = nHeight;
		nTotalHeight += height;
	}

	int nTotalWidth = 0;
	int nIndex = 0;
	for (auto& width : vColSize)
	{
		vColColor[nIndex] = DEF_COLOR_BTN_PAGE_NORMAL;
		width = rGrid.Width() / (int)vColSize.size();
		nTotalWidth += width;

		nIndex++;
	}

	vColSize.front() += rGrid.Width() - nTotalWidth - 2;

	/* 객체 기본 설정 */
	pGrid->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
	pGrid->SetRowCount((int)vRowSize.size());
	pGrid->SetColumnCount((int)vColSize.size());
	pGrid->SetRowHeight(0, nHeight);
	pGrid->SetFixedColumnCount(0);
	pGrid->SetGridLineColor(BLACK_);
	pGrid->SetBkColor(WHITE_);
	pGrid->SetFixedColumnSelection(0);

	/* 타이틀 (첫 번째 행) */
	for (int i = 0; i < (int)vColSize.size(); i++)
	{
		pGrid->SetItemFormat(0, i, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pGrid->SetColumnWidth(i, vColSize[i]);
		pGrid->SetItemText(0, i, vTitle[i].c_str());
		pGrid->SetItemBkColour(0, i, vColColor[i]);
		pGrid->SetItemFgColour(0, i, WHITE_);
	}

	pGrid->SetItemBkColour(0, nMode, DEF_COLOR_BTN_MENU_SELECT);

	/* 기본 속성 및 갱신 */
	pGrid->SetBaseGridProp(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}


VOID CDlgCalbExposureFEM::InitGridHeadFocusCtrl()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <COLORREF>		vColColor = { DEF_COLOR_BTN_PAGE_NORMAL, WHITE_, DEF_COLOR_BTN_PAGE_NORMAL, DEF_COLOR_BTN_PAGE_NORMAL };
	std::vector <COLORREF>		vColTxtColor = { DEF_COLOR_BTN_PAGE_NORMAL_TEXT, BLACK_, DEF_COLOR_BTN_PAGE_NORMAL_TEXT, DEF_COLOR_BTN_PAGE_NORMAL_TEXT };
	std::vector <int>			vColSize(4);
	std::vector <std::wstring>	vTitle = { _T("LOAD"), _T("0.0000"), _T("+"), _T("-") };

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_HEAD_FOCUS_CTRL];

	clsResizeUI.ResizeControl(this, pGrid);
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	int nHeight = (int)(DEF_DEFAULT_GRID_ROW_SIZE * clsResizeUI.GetRateY());

	int nTotalWidth = 0;

	for (int i = 0; i < (int)vColSize.size() - 1; i++)
	{
		vColSize[i] = rGrid.Width() / ((int)vColSize.size() - 1);
		nTotalWidth += vColSize[i];
	}

	vColSize[2] = (int)(vColSize[2] * 0.5);
	vColSize[3] = vColSize[2];

	//vColSize.front() += rGrid.Width() - nTotalWidth - 1;

	/* 객체 기본 설정 */
	pGrid->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
	pGrid->SetRowCount(1);
	pGrid->SetColumnCount((int)vColSize.size());
	pGrid->SetFixedColumnCount(0);
	pGrid->SetGridLineColor(BLACK_);
	pGrid->SetBkColor(WHITE_);
	pGrid->SetFixedColumnSelection(0);

	for (int nRow = 0; nRow < 1; nRow++)
	{
		pGrid->SetRowHeight(nRow, nHeight);

		for (int nCol = 0; nCol < (int)vColSize.size(); nCol++)
		{
			pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			pGrid->SetColumnWidth(nCol, vColSize[nCol]);
			pGrid->SetItemText(nRow, nCol, vTitle[nCol].c_str());
			pGrid->SetItemBkColour(nRow, nCol, vColColor[nCol]);
			pGrid->SetItemFgColour(nRow, nCol, vColTxtColor[nCol]);
			pGrid->SetItemState(nRow, nCol, GVIS_READONLY);
		}
	}

	/* 기본 속성 및 갱신 */
	pGrid->EnableDragAndDrop(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}


VOID CDlgCalbExposureFEM::LoadDataConfig()
{
	LPG_CPFI pstPhFocs = &uvEng_GetConfig()->ph_focus;
	CGridCtrl* pGrid = nullptr;

	pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_PRINT_OPTION];
	pGrid->SetItemText(eGRD_PRINT_ROW_COUNT_X, eGRD_PRINT_COL_VALUE, (int)pstPhFocs->step_x_count, FALSE);
	pGrid->SetItemText(eGRD_PRINT_ROW_COUNT_Y, eGRD_PRINT_COL_VALUE, (int)pstPhFocs->step_y_count, FALSE);
	pGrid->SetItemDouble(eGRD_PRINT_ROW_PERIOD_X, eGRD_PRINT_COL_VALUE, (int)pstPhFocs->step_move_x, 4);
	pGrid->SetItemDouble(eGRD_PRINT_ROW_PERIOD_Y, eGRD_PRINT_COL_VALUE, (int)pstPhFocs->step_move_y, 4);
	pGrid->Refresh();

	pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_POSITION];
	pGrid->SetItemDouble(eGRD_POS_ROW_EXPOSURE, eGRD_POS_COL_X, uvEng_GetConfig()->luria_svc.table_expo_start_xy[0][0], 4);
	pGrid->SetItemDouble(eGRD_POS_ROW_EXPOSURE, eGRD_POS_COL_Y, uvEng_GetConfig()->luria_svc.table_expo_start_xy[0][1], 4);
	pGrid->SetItemDouble(eGRD_POS_ROW_UNLOAD, eGRD_POS_COL_X, uvEng_GetConfig()->set_align.table_unloader_xy[0][0], 4);
	pGrid->SetItemDouble(eGRD_POS_ROW_UNLOAD, eGRD_POS_COL_Y, uvEng_GetConfig()->set_align.table_unloader_xy[0][1], 4);
	pGrid->Refresh();

	pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_EXPOSURE_OPTION];
	pGrid->SetItemDouble(eGRD_EXPO_ROW_STEP_FOCUS, eGRD_EXPO_COL_VALUE, pstPhFocs->step_foucs, 4);
	pGrid->SetItemDouble(eGRD_EXPO_ROW_START_ENERGY, eGRD_EXPO_COL_VALUE, pstPhFocs->start_energy, 3);
	pGrid->SetItemDouble(eGRD_EXPO_ROW_STEP_ENERGY, eGRD_EXPO_COL_VALUE, pstPhFocs->step_energy, 3);

	pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_HEAD_FOCUS];
	for (int i = 1; i <= uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		pGrid->SetItemDouble(i, eGRD_FOCUS_COL_START, pstPhFocs->start_foucs[i - 1], 4);
	}

	CalcMaxEnergy();
	CalcMaxFocus();
	SelectPrintType(pstPhFocs->print_type);
}


VOID CDlgCalbExposureFEM::SaveDataConfig()
{
	LPG_CPFI pstPhFocs = &uvEng_GetConfig()->ph_focus;
	CGridCtrl* pGrid = nullptr;

	if (IDNO == AfxMessageBox(L"저장하시겠습니까?", MB_YESNO))
	{
		return;
	}

	pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_PRINT_OPTION];
	pstPhFocs->step_x_count = pGrid->GetItemTextToInt(eGRD_PRINT_ROW_COUNT_X, eGRD_PRINT_COL_VALUE);
	pstPhFocs->step_y_count = pGrid->GetItemTextToInt(eGRD_PRINT_ROW_COUNT_Y, eGRD_PRINT_COL_VALUE);
	pstPhFocs->step_move_x = pGrid->GetItemTextToFloat(eGRD_PRINT_ROW_PERIOD_X, eGRD_PRINT_COL_VALUE);
	pstPhFocs->step_move_y = pGrid->GetItemTextToFloat(eGRD_PRINT_ROW_PERIOD_Y, eGRD_PRINT_COL_VALUE);

	pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_POSITION];
	uvEng_GetConfig()->luria_svc.table_expo_start_xy[0][0] = pGrid->GetItemTextToFloat(eGRD_POS_ROW_EXPOSURE, eGRD_POS_COL_X);
	uvEng_GetConfig()->luria_svc.table_expo_start_xy[0][1] = pGrid->GetItemTextToFloat(eGRD_POS_ROW_EXPOSURE, eGRD_POS_COL_Y);
	uvEng_GetConfig()->set_align.table_unloader_xy[0][0] = pGrid->GetItemTextToFloat(eGRD_POS_ROW_UNLOAD, eGRD_POS_COL_X);
	uvEng_GetConfig()->set_align.table_unloader_xy[0][1] = pGrid->GetItemTextToFloat(eGRD_POS_ROW_UNLOAD, eGRD_POS_COL_Y);

	pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_EXPOSURE_OPTION];

	pstPhFocs->start_energy = pGrid->GetItemTextToFloat(eGRD_EXPO_ROW_START_ENERGY, eGRD_EXPO_COL_VALUE);
	pstPhFocs->step_foucs = pGrid->GetItemTextToFloat(eGRD_EXPO_ROW_STEP_FOCUS, eGRD_EXPO_COL_VALUE);
	pstPhFocs->step_energy = pGrid->GetItemTextToFloat(eGRD_EXPO_ROW_STEP_ENERGY, eGRD_EXPO_COL_VALUE);

	pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_HEAD_FOCUS];
	for (int i = 1; i <= uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		pstPhFocs->start_foucs[i - 1] = pGrid->GetItemTextToFloat(i, eGRD_FOCUS_COL_START);
	}

	uvEng_SaveConfig();
}


VOID CDlgCalbExposureFEM::LoadFocusData()
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_HEAD_FOCUS];

	for (int i = 1; i <= uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		pGrid->SetItemDouble(i, eGRD_FOCUS_COL_START, uvEng_GetConfig()->luria_svc.ph_z_focus[i - 1], 4);
	}

	CalcMaxFocus();
}


VOID CDlgCalbExposureFEM::MoveFocusData(double dValue)
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_HEAD_FOCUS];

	for (int i = 1; i <= uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		pGrid->SetItemDouble(i, eGRD_FOCUS_COL_START, pGrid->GetItemTextToFloat(i, eGRD_FOCUS_COL_START) + dValue, 4);
	}

	CalcMaxFocus();
}


VOID CDlgCalbExposureFEM::UnloadPosition()
{
	if (IDNO == AfxMessageBox(L"Stage를 배출 위치로 이동하시겠습니까?", MB_YESNO))
	{
		return;
	}

	DOUBLE dbMoveX = uvEng_GetConfig()->set_align.table_unloader_xy[0][0];
	DOUBLE dbMoveY = uvEng_GetConfig()->set_align.table_unloader_xy[0][1];

	/* 모션 (스테이지) 이동 */
	if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_x, dbMoveX))
	{
		return;
	}
	if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_y, dbMoveY))
	{
		return;
	}
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, dbMoveX, uvEng_GetConfig()->mc2_svc.move_velo);
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dbMoveY, uvEng_GetConfig()->mc2_svc.move_velo);
}


/*
 desc : 모든 광학계를 Focus 위치로 이동
 parm : None
 retn : None
*/
VOID CDlgCalbExposureFEM::PHMovingFocus()
{
	UINT8 u8Drv = UINT8(ENG_MMDI::en_axis_ph1);
	double dPosZ;	/* 단위: mm */

	if (IDNO == AfxMessageBox(L"Photo Head를 포커스 위치로 이동하시겠습니까?", MB_YESNO))
	{
		return;
	}

	for (UINT8 i = 0; i < uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		/* 광학계 Z 축 Focus 값 얻기 */
		dPosZ = uvEng_GetConfig()->luria_svc.ph_z_focus[i];

		/* 이동하고자 하는 값이 범위를 벗어났는지 여부 */
		if (!uvCmn_Luria_IsValidPhMoveZRange(dPosZ))
		{
			AfxMessageBox(L"Out of moving range area", MB_ICONSTOP | MB_TOPMOST);
			return;
		}

		/* 광학계 Z 축 이동 */
		if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type) // Stepper motor
		{
			uvEng_Luria_ReqSetMotorAbsPosition(i + 1, dPosZ);
		}
		else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type) // Linear drive from Sieb & Meyer
		{
			uvEng_MC2_SendDevAbsMove(ENG_MMDI(u8Drv + i), dPosZ, uvEng_GetConfig()->mc2_svc.max_velo[u8Drv]);
		}
	}
}


VOID CDlgCalbExposureFEM::RefreshJobList()
{
	uvEng_Luria_ReqGetJobList();
}


VOID CDlgCalbExposureFEM::LoadJob()
{
	TCHAR tzJobName[MAX_PATH_LEN] = { NULL };
	LPG_LDJM pstJobMgt = &uvEng_ShMem_GetLuria()->jobmgt;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_JOBS];
	CString strSelJobName = (CString)pstJobMgt->selected_job_name;
	CString strJobName;

	/* List Box에서 선택된 항목 가져오기 */

	if (0 > m_n8SelJob)
	{
		AfxMessageBox(L"Please select an item.", MB_ICONSTOP | MB_TOPMOST);
		return;
	}

	/* 선택된 항목이 유효한지 검사 */
	strJobName = pGrid->GetItemText(m_n8SelJob, 0);
	if (1 > strJobName.GetLength())
	{
		return;
	}

	if (0 != strJobName.Compare(strSelJobName))
	{
		AfxMessageBox(L"The gerber for removal is not selected in the optics", MB_ICONSTOP | MB_TOPMOST);
		return;
	}

	/* 현재 선택된 Job의 적재 진행 요청 */
	if (!uvEng_Luria_ReqSetLoadSelectedJob())
	{
		AfxMessageBox(L"Failed to load the Gerber Name", MB_ICONSTOP | MB_TOPMOST);
		return;
	}
	/* 현재 노광 조건 정보 요청 */
	if (!uvEng_Luria_ReqGetExposureFactor())
	{
		AfxMessageBox(L"Failed to request the Exposure Factor", MB_ICONSTOP | MB_TOPMOST);
		return;
	}
}


VOID CDlgCalbExposureFEM::GetExpoParam()
{
	if (!uvEng_Luria_ReqGetExposureFactor())
	{
		AfxMessageBox(L"Failed to get the value required for exposure", MB_ICONSTOP | MB_TOPMOST);
		return;
	}

	LoadDataConfig();
}



/*
 desc : 새로운 Job Name 등록
 parm : None
 retn : None
*/
VOID CDlgCalbExposureFEM::AddJob()
{
	TCHAR tzJobName[MAX_PATH_LEN] = { NULL };
	TCHAR tzRLTFiles[MAX_PATH_LEN] = { NULL };
	LPG_LDJM pstJobMgt = &uvEng_ShMem_GetLuria()->jobmgt;
	CUniToChar csCnv;

	// Job Name 디렉토리 선택
	if (FALSE == uvCmn_RootSelectPath(m_hWnd, uvEng_GetConfig()->set_comn.gerber_path, tzJobName))
	{
		return;
	}

	// Job Name 내에 하위 디렉토리가 존재하는지 확인
	if (0 != uvCmn_GetChildPathCount(tzJobName))
	{
		AfxMessageBox(L"This is not a gerber directory", MB_ICONSTOP | MB_TOPMOST);
		return;
	}

	// 해당 디렉토리 내에 rlt_settings.xml 파일이 존재하는지 확인
	swprintf_s(tzRLTFiles, MAX_PATH_LEN, L"%s\\rlt_settings.xml", tzJobName);

	if (FALSE == uvCmn_FindFile(tzRLTFiles))
	{
		AfxMessageBox(L"File (rlt_settings.xml) does not exist", MB_ICONSTOP | MB_TOPMOST);
		return;
	}

	// 기존에 등록된 거버 파일 중에 동일한 거버가 있는지 여부 확인
	if (TRUE == pstJobMgt->IsJobOnlyFinded(csCnv.Uni2Ansi(tzJobName)))
	{
		AfxMessageBox(L"Job Name already registered exist", MB_ICONSTOP | MB_TOPMOST);
		return;
	}

	// Luria Service에 등록 작업 진행
	if (FALSE == uvEng_Luria_ReqAddJobList(tzJobName))
	{
		AfxMessageBox(L"Failed to register Job Name", MB_ICONSTOP | MB_TOPMOST);
		return;
	}
}

/*
 desc : 기존 Job Name 제거
 parm : None
 retn : None
*/
VOID CDlgCalbExposureFEM::DeleteJob()
{
	TCHAR tzJobName[MAX_PATH_LEN] = { NULL };
	LPG_LDJM pstJobMgt = &uvEng_ShMem_GetLuria()->jobmgt;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_JOBS];
	CString strSelJobName = (CString)uvEng_ShMem_GetLuria()->jobmgt.selected_job_name;
	CString strJobName;

	// List Box에서 선택된 항목 가져오기
	if (0 > m_n8SelJob)
	{
		AfxMessageBox(L"The selected item does not exist", MB_ICONSTOP | MB_TOPMOST);
		return;
	}

	// 선택된 항목이 유효한지 검사
	strJobName = pGrid->GetItemText(m_n8SelJob, 0);
	if (1 > strJobName.GetLength())
	{
		return;
	}

	if (0 != strJobName.Compare(strSelJobName))
	{
		AfxMessageBox(L"The gerber for removal is not selected in the optics", MB_ICONSTOP | MB_TOPMOST);
		return;
	}

	// 선택된 항목을 거버에서 삭제
	if (!uvEng_Luria_ReqSetDeleteSelectedJob())
	{
		AfxMessageBox(L"Failed to delete the selected Gerber Name", MB_ICONSTOP | MB_TOPMOST);
		return;
	}
}


VOID CDlgCalbExposureFEM::UpdateJobList()
{
	BOOL bUpdateJobs = FALSE;
	LPG_LDJM pstJobMgt = &uvEng_ShMem_GetLuria()->jobmgt;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_JOBS];
	CString strJobName;
	CString strSelJobName;
	CRect rGrid;

	/* 메모리에 등록된 것이 없으면서, 리스트에 등록되것이 없으면 Skip */
	if (1 > pstJobMgt->job_regist_count && 1 > pGrid->GetRowCount())
	{
		return;
	}

	/* 메모리에 등록된 개수와 리스트 박스에 등록된 개수가 다른지 확인 */
	if (pGrid->GetRowCount() != pstJobMgt->job_regist_count)
	{
		bUpdateJobs = TRUE;
	}

	/* 공유 메모리와 리스트에 등록된 개수는 같지만, 혹시 거버 이름이 다를수 도 있으므로 */
	if (FALSE == bUpdateJobs)
	{
		for (int i = 0; i < pstJobMgt->job_regist_count; i++)
		{
			/* 동일한 항목을 찾았으면... 찾은 개수 증가 시키기 */
			strJobName = pGrid->GetItemText(i, 0);
			if (FALSE == pstJobMgt->IsJobFullFinded((LPSTR)(LPCTSTR)strJobName))
			{
				bUpdateJobs = TRUE;
			}
		}
	}

	/* 갱신할 필요 없다면, 리턴 */
	if (FALSE == bUpdateJobs)
	{
		return;
	}

	/* ------------------------ */
	/* Job List 등록 혹은 갱신  */
	/* ------------------------ */

	/* 기존 항목 모두 지우기 */
	pGrid->DeleteAllItems();

	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	//int nHeight = rGrid.Height() - 1;
	int nWidth = rGrid.Width() - 1;

	pGrid->SetRowCount(pstJobMgt->job_regist_count);
	pGrid->SetColumnCount(1);
	pGrid->SetColumnWidth(0, nWidth);

	/* 공유 메모리에 있는 항목 등록 */
	for (int i = 0; i < pGrid->GetRowCount(); i++)
	{
		strSelJobName = pstJobMgt->selected_job_name;
		strJobName = pstJobMgt->job_name_list[i];
		pGrid->SetItemText(i, 0, strJobName);

		/* 현재 선택된 항목인지 확인 후, 선택된 항목이면, 선택 작업 진행 */
		if (0 == strJobName.Compare(strSelJobName))
		{
			pGrid->SetItemBkColour(i, 0, LIGHT_PINK);
		}
	}

	pGrid->Refresh();
}


VOID CDlgCalbExposureFEM::UpdateJobState()
{
	LPG_LDJM pstJobMgt = &uvEng_ShMem_GetLuria()->jobmgt;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_JOB_STATE];
	CString strMsg;

	/* 현재 선택된 Job Name이 존재하는지 여부 */
	if (FALSE == pstJobMgt->IsJobNameSelected())
	{
		pGrid->SetItemText(eGRD_JOB_STATE_ROW, eGRD_JOB_STATE_COL_STATE, _T("None"));
		return;
	}

	switch (pstJobMgt->selected_job_load_state)
	{
	case ENG_LSLS::en_load_none:		 strMsg = _T("None");		break;
	case ENG_LSLS::en_load_not_started:	 strMsg = _T("Ready");		break;
	case ENG_LSLS::en_loading:			 strMsg = _T("Loading");	break;
	case ENG_LSLS::en_load_completed:	 strMsg = _T("Loaded");		break;
	case ENG_LSLS::en_load_failed:		 strMsg = _T("Failed");		break;
	default:							 strMsg = _T("Error");		break;
	}

	pGrid->SetItemText(eGRD_JOB_STATE_ROW, eGRD_JOB_STATE_COL_STATE, strMsg);

	/* 현재 상태가 Completed이고 이전 (최근) 상태가 Completed가 아니라면 ... */
	if (ENG_LSLS(pstJobMgt->selected_job_load_state) == ENG_LSLS::en_load_completed &&
		ENG_LSLS(pstJobMgt->selected_job_load_state) != m_enLastJobLoadStatus)
	{
		/* XML 파일 적재 진행 */
		if (FALSE == uvEng_Luria_LoadSelectJobXML())
		{
			AfxMessageBox(L"Failed to load the currently selected gerber file", MB_ICONSTOP | MB_TOPMOST);
		}
	}
}


VOID CDlgCalbExposureFEM::UpdateWorkProgress()
{
	CDlgMain* pMainDlg = (CDlgMain*)::AfxGetMainWnd();

	// 작업중일 때
	if (TRUE == pMainDlg->IsBusyWorkJob())
	{
		// 그 작업이 FEM이라면
		if (ENG_BWOK::en_gerb_expofem == pMainDlg->GetWorkJobID())
		{
			m_pgr_ctl[eCALB_EXPOSURE_FEM_PGR_RATE].SetPos((int)uvEng_GetWorkStepRate());
			m_txt_ctl[eCALB_EXPOSURE_FEM_TXT_STATE].SetTextToStr(uvEng_GetWorkStepName());
		}
	}
}


VOID CDlgCalbExposureFEM::UpdateLuriaError()
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_OPTIC_STATE];

	int nErrCode = (int)uvCmn_Luria_GetLastErrorStatus();

	if (nErrCode != pGrid->GetItemTextToInt(eGRD_OPTIC_STATE_ROW, eGRD_OPTIC_STATE_COL_VALUE))
	{
		pGrid->SetItemText(eGRD_OPTIC_STATE_ROW, eGRD_OPTIC_STATE_COL_VALUE, nErrCode);
		pGrid->Refresh();
	}
}

VOID CDlgCalbExposureFEM::ReqLuriaState()
{
	LPG_LDJM pstJobMgt = &uvEng_ShMem_GetLuria()->jobmgt;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_JOB_STATE];

	/* Luria로부터 통지 서비스를 받는지 여부에 따라 */
	if (FALSE == uvEng_GetConfig()->luria_svc.use_announcement)
	{
		/* 만약, 현재 거버가 적재 중인 상태인 경우, 지속적으로 Job Load State 요청 */
		if ((pstJobMgt->selected_job_load_state == (UINT8)ENG_LSLS::en_load_not_started ||
			pstJobMgt->selected_job_load_state == (UINT8)ENG_LSLS::en_loading))
		{
			/* Job Load State 요청 */
			uvEng_Luria_ReqGetSelectedJobLoadState();
		}
	}
}


VOID CDlgCalbExposureFEM::RefreshResultCell()
{
	LPG_CPFI pstPhFocs = &uvEng_GetConfig()->ph_focus;

	if (0 >= pstPhFocs->step_x_count ||
		0 >= pstPhFocs->step_y_count)
	{
		return;
	}

	InitGridResult((int)pstPhFocs->step_x_count, (int)pstPhFocs->step_y_count);
}


VOID CDlgCalbExposureFEM::SelectPrintType(UINT8 u8Type)
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_PRINT_TYPE];

	if (u8Type >= eGRD_TYPE_COL_MAX)
	{
		return;
	}

	for (int i = 0; i < pGrid->GetColumnCount(); i++)
	{
		pGrid->SetItemBkColour(0, i, DEF_COLOR_BTN_PAGE_NORMAL);
	}

	pGrid->SetItemBkColour(0, u8Type, DEF_COLOR_BTN_MENU_SELECT);
	pGrid->Refresh();
}


VOID CDlgCalbExposureFEM::CalcMaxEnergy()
{
	CGridCtrl* pExpoGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_EXPOSURE_OPTION];
	CGridCtrl* pPrfGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_PRINT_OPTION];

	double dCountEnergy = 0;
	double dStartEnergy = pExpoGrid->GetItemTextToFloat(eGRD_EXPO_ROW_START_ENERGY, eGRD_EXPO_COL_VALUE);
	double dStepEnergy = pExpoGrid->GetItemTextToFloat(eGRD_EXPO_ROW_STEP_ENERGY, eGRD_EXPO_COL_VALUE);
	double dMaxEnergy = 0.;

	/* 0 : X = Focus Y = Energy, 1 : X = Energy Y = Focus */
	if (0 == uvEng_GetConfig()->ph_focus.print_type)
	{
		dCountEnergy = pPrfGrid->GetItemTextToFloat(eGRD_PRINT_ROW_COUNT_Y, eGRD_PRINT_COL_VALUE) - 1;
	}
	else
	{
		dCountEnergy = pPrfGrid->GetItemTextToFloat(eGRD_PRINT_ROW_COUNT_X, eGRD_PRINT_COL_VALUE) - 1;
	}

	dMaxEnergy = dStartEnergy + (dStepEnergy * dCountEnergy);

	pExpoGrid->SetItemDouble(eGRD_EXPO_ROW_MAX_ENERGY, eGRD_EXPO_COL_VALUE, dMaxEnergy, 3);
	pExpoGrid->Refresh();
}


VOID CDlgCalbExposureFEM::CalcStepEnergy()
{
	CGridCtrl* pExpoGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_EXPOSURE_OPTION];
	CGridCtrl* pPrfGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_PRINT_OPTION];

	double dCountEnergy = 0;
	double dStartEnergy = pExpoGrid->GetItemTextToFloat(eGRD_EXPO_ROW_START_ENERGY, eGRD_EXPO_COL_VALUE);
	double dMaxEnergy = pExpoGrid->GetItemTextToFloat(eGRD_EXPO_ROW_MAX_ENERGY, eGRD_EXPO_COL_VALUE);
	double dStepFocus = 0;
	double dStepEnergy = 0;

	/* 0 : X = Focus Y = Energy, 1 : X = Energy Y = Focus */
	if (0 == uvEng_GetConfig()->ph_focus.print_type)
	{
		dCountEnergy = pPrfGrid->GetItemTextToFloat(eGRD_PRINT_ROW_COUNT_Y, eGRD_PRINT_COL_VALUE) - 1;
	}
	else
	{
		dCountEnergy = pPrfGrid->GetItemTextToFloat(eGRD_PRINT_ROW_COUNT_X, eGRD_PRINT_COL_VALUE) - 1;
	}

	if (0 != dMaxEnergy - dStartEnergy && 0 != dCountEnergy)
	{
		dStepEnergy = (dMaxEnergy - dStartEnergy) / dCountEnergy;
	}

	pExpoGrid->SetItemDouble(eGRD_EXPO_ROW_STEP_ENERGY, eGRD_EXPO_COL_VALUE, dStepEnergy, 3);
	pExpoGrid->Refresh();
}


VOID CDlgCalbExposureFEM::CalcMaxFocus()
{
	CGridCtrl* pExpoGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_EXPOSURE_OPTION];
	CGridCtrl* pPrfGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_PRINT_OPTION];
	CGridCtrl* pFocusGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_HEAD_FOCUS];

	double dCountFocus = 0;
	double dStepFocus = pExpoGrid->GetItemTextToFloat(eGRD_EXPO_ROW_STEP_FOCUS, eGRD_EXPO_COL_VALUE);
	double dStartFocus = 0;
	double dMaxFocus = 0;

	/* 0 : X = Focus Y = Energy, 1 : X = Energy Y = Focus */
	if (0 == uvEng_GetConfig()->ph_focus.print_type)
	{
		dCountFocus = pPrfGrid->GetItemTextToFloat(eGRD_PRINT_ROW_COUNT_X, eGRD_PRINT_COL_VALUE) - 1;
	}
	else
	{
		dCountFocus = pPrfGrid->GetItemTextToFloat(eGRD_PRINT_ROW_COUNT_Y, eGRD_PRINT_COL_VALUE) - 1;
	}

	for (int i = 1; i <= uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		dStartFocus = pFocusGrid->GetItemTextToFloat(i, eGRD_FOCUS_COL_START);
		dMaxFocus = dStartFocus + (dStepFocus * dCountFocus);

		pFocusGrid->SetItemDouble(i, eGRD_FOCUS_COL_END, dMaxFocus, 4);
	}

	pFocusGrid->Refresh();
}


/*
 desc : 모든 광학계 HW Init 위치로 이동
 parm : None
 retn : None
*/
VOID CDlgCalbExposureFEM::PHHWInit()
{
	BOOL bSucc = FALSE;

	/* MC2가 초기화 되었는지 (Error가 없는지) 여부 */
	if (uvCmn_MC2_IsAnyDriveError())
	{
		AfxMessageBox(L"An error has occurred in MC2", MB_ICONSTOP | MB_TOPMOST);
		return;
	}

	/* PH의 Z 축 모션이 초기화 되었는지 여부 */
	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		bSucc = uvCmn_Luria_IsAllPhZAxisMidPosition();
	}
	else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		bSucc = uvCmn_MC2_IsDriveHomed(ENG_MMDI::en_axis_ph1) &&
			uvCmn_MC2_IsDriveHomed(ENG_MMDI::en_axis_ph2);
	}

	if (FALSE == bSucc)
	{
		AfxMessageBox(L"The motion Z axis of the optical system is not initialized", MB_ICONSTOP | MB_TOPMOST);
		return;
	}

	/* Luria Service에 거버가 등록되어 있는지 여부 */
	if (0 < uvCmn_Luria_GetJobCount())
	{
		AfxMessageBox(L"A registered Job Name exists", MB_ICONSTOP | MB_TOPMOST);
		return;
	}

	/* Luria Service 초기화 */
	if (FALSE == uvEng_Luria_ReqSetHWInit())
	{
		AfxMessageBox(L"The HW initialization of the service failed", MB_ICONSTOP | MB_TOPMOST);
		return;
	}
}


/*
 desc : Print 시작
 parm : None
 retn : None
*/
VOID CDlgCalbExposureFEM::StartPrint()
{
	CDlgMain* pMainDlg = (CDlgMain*)::AfxGetMainWnd();

	if (TRUE == pMainDlg->IsBusyWorkJob())
	{
		AfxMessageBox(_T("현재 작업이 진행중입니다."), MB_ICONSTOP | MB_TOPMOST);
		return;
	}

	if (IDNO == AfxMessageBox(L"노광을 시작하시겠습니까?", MB_YESNO))
	{
		return;
	}

	RefreshResultCell();
	pMainDlg->RunWorkJob(ENG_BWOK::en_gerb_expofem, (PUINT64)this->GetSafeHwnd());
}


/*
 desc : Print 중지
 parm : None
 retn : None
*/
VOID CDlgCalbExposureFEM::StopPrint()
{
	CDlgMain* pMainDlg = (CDlgMain*)::AfxGetMainWnd();

	pMainDlg->RunWorkJob(ENG_BWOK::en_work_stop);
}


double CDlgCalbExposureFEM::GetSpeedToEnergy(double dFrameRate)
{
	CUniToChar csCnv;
	LPG_RJAF pstJobRecipe = uvEng_JobRecipe_GetSelectRecipe();

	if (NULL == pstJobRecipe)
	{
		return -1;
	}

	LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstJobRecipe->expo_recipe));
	DOUBLE dbTotal = 0.0f, dbPowerWatt[MAX_LED] = { NULL };
	LPG_PLPI pstPowerI = uvEng_LedPower_GetLedPowerName(csCnv.Ansi2Uni(pstExpoRecipe->power_name));
	UINT16 u16FrameRate = (UINT16)(dFrameRate * 1000);

	if (uvEng_GetConfig()->IsRunDemo())
	{
		// by sysandj : 변수없음(수정)
		pstJobRecipe->step_size = 4;
	}

	for (int i = 0; i < uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		for (int j = 0; j < uvEng_GetConfig()->luria_svc.led_count; j++)
		{
			dbPowerWatt[j] = pstPowerI->led_watt[i][j];
		}

		dbTotal += uvCmn_Luria_GetSpeedToEnergy(pstJobRecipe->step_size, u16FrameRate,
			pstExpoRecipe->led_duty_cycle, dbPowerWatt);
	}

	return dbTotal / uvEng_GetConfig()->luria_svc.ph_count;
}


BOOL CDlgCalbExposureFEM::PopupKBDN(ENM_DITM enType, CString strInput, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts/* = 0*/)
{
	TCHAR tzTitle[1024] = { NULL }, tzPoint[512] = { NULL }, tzMinMax[2][32] = { NULL };
	CDlgKBDN dlg;

	wcscpy_s(tzMinMax[0], 32, L"Min");
	wcscpy_s(tzMinMax[1], 32, L"Max");

	switch (enType)
	{
	case ENM_DITM::en_int8:
	case ENM_DITM::en_int16:
	case ENM_DITM::en_int32:
	case ENM_DITM::en_int64:
	case ENM_DITM::en_uint8:
	case ENM_DITM::en_uint16:
	case ENM_DITM::en_uint32:
	case ENM_DITM::en_uint64:
		swprintf_s(tzPoint, 512, L"%s:%%.%df,%s:%%.%df", tzMinMax[0], u8DecPts, tzMinMax[1], u8DecPts);
		swprintf_s(tzTitle, 1024, tzPoint, dMin, dMax);
		if (IDOK != dlg.MyDoModal(tzTitle, FALSE, TRUE, dMin, dMax))	return FALSE;	break;
	case ENM_DITM::en_float:
	case ENM_DITM::en_double:
		swprintf_s(tzPoint, 512, L"%s:%%.%df,%s:%%.%df", tzMinMax[0], u8DecPts, tzMinMax[1], u8DecPts);
		swprintf_s(tzTitle, 1024, tzPoint, dMin, dMax);
		if (IDOK != dlg.MyDoModal(tzTitle, TRUE, TRUE, dMin, dMax))	return FALSE;	break;
	}
	switch (enType)
	{
	case ENM_DITM::en_int8:
	case ENM_DITM::en_int16:
	case ENM_DITM::en_int32:
	case ENM_DITM::en_int64:
	case ENM_DITM::en_uint8:
	case ENM_DITM::en_uint16:
	case ENM_DITM::en_uint32:
	case ENM_DITM::en_uint64:
		strOutput.Format(_T("%d"), dlg.GetValueInt32());
		break;
	case ENM_DITM::en_float:
	case ENM_DITM::en_double:
		swprintf_s(tzPoint, 512, L"%%.%df", u8DecPts);
		strOutput.Format(tzPoint, dlg.GetValueDouble());
	}

	return TRUE;
}


LRESULT CDlgCalbExposureFEM::InputSetPoint(WPARAM wParam, LPARAM lParam)
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_RESULT];

	m_nPrintRow = (pGrid->GetRowCount()) - (int)wParam;
	m_nPrintCol = (pGrid->GetColumnCount()) - (int)lParam;
	m_bPrinting = TRUE;

	return 0;
}


LRESULT CDlgCalbExposureFEM::InputPrintCompl(WPARAM wParam, LPARAM lParam)
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_RESULT];

	m_bPrinting = FALSE;

 	pGrid->SetItemBkColour(m_nPrintRow, m_nPrintCol, GREEN_);
	pGrid->Refresh();
	return 0;
}


LRESULT CDlgCalbExposureFEM::ReportResult(WPARAM wParam, LPARAM lParam)
{


	return 0;
}


/*
 desc : 일반 버튼 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgCalbExposureFEM::OnBtnClick(UINT32 id)
{
	int nBtnID = id - IDC_CALB_EXPOSURE_FEM_BTN_START;

// 	if (TRUE == CPowerMeasureMgr::GetInstance()->IsProcessWorking() &&
// 		nBtnID != eCALB_UVPOWER_CHECK_BTN_STOP)
// 	{
// 		AfxMessageBox(_T("현재 측정중입니다."));
// 		return;
// 	}

	switch (nBtnID)
	{
	case eCALB_EXPOSURE_FEM_BTN_START:
		StartPrint();
		break;
	case eCALB_EXPOSURE_FEM_BTN_STOP:
		StopPrint();
		break;
	case eCALB_EXPOSURE_FEM_BTN_MOVE_UNLOAD:
		UnloadPosition();
		break;
	case eCALB_EXPOSURE_FEM_BTN_MOVE_FOCUS:
		PHMovingFocus();
		break;
	case eCALB_EXPOSURE_FEM_BTN_RELOAD_LURIA_DATA:
		GetExpoParam();
		break;
	case eCALB_EXPOSURE_FEM_BTN_JOB_LOAD:
		LoadJob();
		break;
	case eCALB_EXPOSURE_FEM_BTN_JOB_REFRESH:
		RefreshJobList();
		break;
	case eCALB_EXPOSURE_FEM_BTN_SAVE:
		SaveDataConfig();
		RefreshResultCell();
		break;
	case eCALB_EXPOSURE_FEM_BTN_JOB_ADD:
		AddJob();
		break;
	case eCALB_EXPOSURE_FEM_BTN_JOB_DELETE:
		DeleteJob();
		RefreshJobList();
		break;
	case eCALB_EXPOSURE_FEM_BTN_OPEN_CTRL_PANEL:
		::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), eMSG_MAIN_OPEN_CONSOLE, NULL, NULL);
		break;
	default:
		break;
	}

}


void CDlgCalbExposureFEM::OnClickGridOpticResult(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_RESULT];
	pGrid->SetFocusCell(-1, -1);
}


void CDlgCalbExposureFEM::OnClickGridOpticState(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_OPTIC_STATE];
	pGrid->SetFocusCell(-1, -1);

	switch (pItem->iColumn)
	{
	case eGRD_OPTIC_STATE_COL_INIT:
		PHHWInit();
		break;
	case eGRD_OPTIC_STATE_COL_RESET:
		uvCmn_Luria_ResetLastErrorStatus();
		pGrid->SetItemText(eGRD_OPTIC_STATE_ROW, eGRD_OPTIC_STATE_COL_VALUE, _T("0"));
		break;

	default:
		return;
	}
}


void CDlgCalbExposureFEM::OnClickGridPrintOption(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_PRINT_OPTION];
	ENM_DITM enType;
	UINT8 u8DecPts = 0;
	CString strOutput;
	CString strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn);
	double	dMin = -9999.9999;
	double	dMax = 9999.9999;
	int	nOldValue = 0;
	double	dMaxValue = 0;

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	if (eGRD_PRINT_COL_VALUE != pItem->iColumn)
	{
		return;
	}

	switch (pItem->iRow)
	{
	case eGRD_PRINT_ROW_COUNT_X:
	case eGRD_PRINT_ROW_COUNT_Y:
		dMin = 1;
		dMax = 100;
		enType = ENM_DITM::en_uint32;
		break;
	case eGRD_PRINT_ROW_PERIOD_Y:
	case eGRD_PRINT_ROW_PERIOD_X:
		enType = ENM_DITM::en_double;
		u8DecPts = 4;
		break;
	default:
		return;
	}

	dMaxValue = GetSpeedToEnergy(0.01);
	nOldValue = pGrid->GetItemTextToInt(pItem->iRow, pItem->iColumn);

	if (FALSE == PopupKBDN(enType, strInput, strOutput, dMin, dMax, u8DecPts))
	{
		return;
	}

	pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);

	/* 0 : X = Focus Y = Energy, 1 : X = Energy Y = Focus */
	if (0 == uvEng_GetConfig()->ph_focus.print_type)
	{
		switch (pItem->iRow)
		{
		case eGRD_PRINT_ROW_COUNT_X:
			CalcMaxFocus();
			break;
		case eGRD_PRINT_ROW_COUNT_Y:
			CalcMaxEnergy();

			if (dMaxValue < m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_EXPOSURE_OPTION].GetItemTextToFloat(eGRD_EXPO_ROW_MAX_ENERGY, eGRD_EXPO_COL_VALUE))
			{
				CString strText;
				strText.Format(_T("Max Energy가 %.3f의 값을 넘을 수 없습니다."), dMaxValue);
				AfxMessageBox(strText, MB_ICONSTOP | MB_TOPMOST);

				pGrid->SetItemText(pItem->iRow, pItem->iColumn, nOldValue);
				CalcMaxEnergy();
			}
			else if (GetSpeedToEnergy(0.99) > m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_EXPOSURE_OPTION].GetItemTextToFloat(eGRD_EXPO_ROW_MAX_ENERGY, eGRD_EXPO_COL_VALUE))
			{
				CString strText;
				strText.Format(_T("Max Energy가 %.3f의 값보다 작을 수 없습니다."), GetSpeedToEnergy(0.99));
				AfxMessageBox(strText, MB_ICONSTOP | MB_TOPMOST);

				pGrid->SetItemText(pItem->iRow, pItem->iColumn, nOldValue);
				CalcMaxEnergy();
			}
			break;
		default:
			break;
		}
	}
	else
	{
		switch (pItem->iRow)
		{
		case eGRD_PRINT_ROW_COUNT_Y:
			CalcMaxFocus();
			break;
		case eGRD_PRINT_ROW_COUNT_X:
			CalcMaxEnergy();

			if (dMaxValue < m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_EXPOSURE_OPTION].GetItemTextToFloat(eGRD_EXPO_ROW_MAX_ENERGY, eGRD_EXPO_COL_VALUE))
			{
				CString strText;
				strText.Format(_T("Max Energy가 %.3f의 값을 넘을 수 없습니다."), dMaxValue);
				AfxMessageBox(strText, MB_ICONSTOP | MB_TOPMOST);

				pGrid->SetItemText(pItem->iRow, pItem->iColumn, nOldValue);
				CalcMaxEnergy();
			}
			else if (GetSpeedToEnergy(0.99) > m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_EXPOSURE_OPTION].GetItemTextToFloat(eGRD_EXPO_ROW_MAX_ENERGY, eGRD_EXPO_COL_VALUE))
			{
				CString strText;
				strText.Format(_T("Max Energy가 %.3f의 값보다 작을 수 없습니다."), GetSpeedToEnergy(0.99));
				AfxMessageBox(strText, MB_ICONSTOP | MB_TOPMOST);

				pGrid->SetItemText(pItem->iRow, pItem->iColumn, nOldValue);
				CalcMaxEnergy();
			}
			break;
		default:
			break;
		}
	}

	pGrid->Refresh();
}


void CDlgCalbExposureFEM::OnClickGridPosition(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_POSITION];
	ENM_DITM enType = ENM_DITM::en_double;
	UINT8 u8DecPts = 4;
	CString strOutput;
	CString strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn);
	double	dMin = -9999.9999;
	double	dMax = 9999.9999;

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	if (eGRD_POS_COL_TITLE == pItem->iColumn ||
		eGRD_POS_ROW_TITLE == pItem->iRow)
	{
		return;
	}

	if (FALSE == PopupKBDN(enType, strInput, strOutput, dMin, dMax, u8DecPts))
	{
		return;
	}

	pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);
	pGrid->Refresh();
}


void CDlgCalbExposureFEM::OnClickGridExposureOption(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_EXPOSURE_OPTION];
	UINT8 u8DecPts = 3;
	CString strOutput;
	CString strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn);
	double	dMin = 0;
	double	dMax = 100;
	double	dOldValue = 0;
	double	dMaxValue = 0;

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	if (eGRD_EXPO_COL_VALUE != pItem->iColumn)
	{
		return;
	}

	dMaxValue = GetSpeedToEnergy(0.01);

	switch (pItem->iRow)
	{
	case eGRD_EXPO_ROW_STEP_FOCUS:
		dMin = -100;
		dMax = 100;
		u8DecPts = 4;
		break;
	case eGRD_EXPO_ROW_START_ENERGY:
	case eGRD_EXPO_ROW_MAX_ENERGY:
		dMin = GetSpeedToEnergy(0.99);
		dMax = dMaxValue;
		break;
	case eGRD_EXPO_ROW_STEP_ENERGY:
		dMin = dMaxValue * -1;
		dMax = dMaxValue;
		break;
	}

	dOldValue = pGrid->GetItemTextToFloat(pItem->iRow, pItem->iColumn);

	if (FALSE == PopupKBDN(ENM_DITM::en_double, strInput, strOutput, dMin, dMax, u8DecPts))
	{
		return;
	}

	pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);

	switch (pItem->iRow)
	{
	case eGRD_EXPO_ROW_STEP_FOCUS:
		CalcMaxFocus();
		break;

	case eGRD_EXPO_ROW_MAX_ENERGY:
		CalcStepEnergy();
		break;
	default:
		CalcMaxEnergy();

		if (dMaxValue < pGrid->GetItemTextToFloat(eGRD_EXPO_ROW_MAX_ENERGY, pItem->iColumn))
		{
			CString strText;
			strText.Format(_T("Max Energy가 %.3f의 값을 넘을 수 없습니다."), dMaxValue);
			AfxMessageBox(strText, MB_ICONSTOP | MB_TOPMOST);

			pGrid->SetItemDouble(pItem->iRow, pItem->iColumn, dOldValue, 3);
			CalcMaxEnergy();
		}
		else if (GetSpeedToEnergy(0.99) > pGrid->GetItemTextToFloat(eGRD_EXPO_ROW_MAX_ENERGY, pItem->iColumn))
		{
			CString strText;
			strText.Format(_T("Max Energy가 %.3f의 값보다 작을 수 없습니다."), dMin);
			AfxMessageBox(strText, MB_ICONSTOP | MB_TOPMOST);

			pGrid->SetItemDouble(pItem->iRow, pItem->iColumn, dOldValue, 3);
			CalcMaxEnergy();
		}

		break;
	}

	pGrid->Refresh();
}


void CDlgCalbExposureFEM::OnClickGridJobs(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_JOBS];
	COLORREF clrReset = WHITE_;
	CString strJobName = pGrid->GetItemText(pItem->iRow, 0);
	CString strSelJobName = (CString)uvEng_ShMem_GetLuria()->jobmgt.selected_job_name;

	pGrid->SetFocusCell(-1, -1);

	m_n8SelJob = pItem->iRow;

	if (0 > m_n8SelJob)
	{
		AfxMessageBox(L"The selected item does not exist", MB_ICONSTOP | MB_TOPMOST);
		return;
	}

	/* 선택된 항목이 유효한지 검사 */
	if (1 > strJobName.GetLength())
	{
		return;
	}

	/* 현재 선택된 Job Name가 동일하지 여부 확인 */
	if (0 == strJobName.Compare(strSelJobName))
	{
		AfxMessageBox(L"Job Name already selected", MB_ICONSTOP | MB_TOPMOST);
		return;
	}

	/* Luria에 선택된 거버가 선택되도록 명령어 전달 */
	if (!uvEng_Luria_ReqSelectedJobName((PTCHAR)(LPCTSTR)strJobName, 0x00))
	{
		AfxMessageBox(L"Failed to selected the Job Name", MB_ICONSTOP | MB_TOPMOST);
		return;
	}
}


void CDlgCalbExposureFEM::OnClickGridJobState(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_JOB_STATE];
	pGrid->SetFocusCell(-1, -1);
}


void CDlgCalbExposureFEM::OnClickGridHeadFocus(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_HEAD_FOCUS];
	CString strOutput;
	CString strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn);
	double	dMin = 0;
	double	dMax = 100;


#if (MC2_DRIVE_2SET == 1)
	dMin = uvEng_GetConfig()->mc2b_svc.min_dist[pItem->iRow - 1];
	dMax = uvEng_GetConfig()->mc2b_svc.max_dist[pItem->iRow - 1];
#else
	dMin = uvEng_GetConfig()->mc2_svc.min_dist[pItem->iRow - 1];
	dMax = uvEng_GetConfig()->mc2_svc.max_dist[pItem->iRow - 1];
#endif

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	if (eGRD_FOCUS_COL_START != pItem->iColumn ||
		0 == pItem->iRow)
	{
		return;
	}

	if (FALSE == PopupKBDN(ENM_DITM::en_double, strInput, strOutput, dMin, dMax, 4))
	{
		return;
	}

	pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);
	pGrid->Refresh();

	CalcMaxFocus();
}


void CDlgCalbExposureFEM::OnClickGridPrintType(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_PRINT_TYPE];

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	if (pItem->iColumn == uvEng_GetConfig()->ph_focus.print_type)
	{
		return;
	}

	if (IDNO == AfxMessageBox(L"정말로 변경하시겠습니까?\n수정중인 데이터는 돌아갑니다.", MB_YESNO))
	{
		return;
	}


	LoadDataConfig();

	uvEng_GetConfig()->ph_focus.print_type = pItem->iColumn;
	SelectPrintType((UINT8)pItem->iColumn);

	//SaveDataConfig();

	CalcMaxEnergy();
	CalcMaxFocus();
}


void CDlgCalbExposureFEM::OnClickGridHeadFocusCtrl(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_HEAD_FOCUS_CTRL];
	CString strOutput;
	CString strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn);
	double	dMin = -100;
	double	dMax = 100;

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	switch (pItem->iColumn)
	{
	case eGRD_CTRL_COL_LOAD:
		LoadFocusData();
		break;
	case eGRD_CTRL_COL_VALUE:
		if (FALSE == PopupKBDN(ENM_DITM::en_double, strInput, strOutput, dMin, dMax, 4))
		{
			return;
		}

		pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);
		pGrid->Refresh();
		break;
	case eGRD_CTRL_COL_PLUS:
		MoveFocusData(pGrid->GetItemTextToFloat(eGRD_CTRL_ROW_PARAM, eGRD_CTRL_COL_VALUE));
		break;
	case eGRD_CTRL_COL_MINUS:
		MoveFocusData(pGrid->GetItemTextToFloat(eGRD_CTRL_ROW_PARAM, eGRD_CTRL_COL_VALUE) * -1);
		break;
	default:
		break;
	}
}


void CDlgCalbExposureFEM::OnTimer(UINT_PTR nIDEvent)
{
	if (eCALB_EXPOSURE_FEM_TIMER_UPDATE_VALUE == nIDEvent)
	{
		UpdateLuriaError();
		UpdateJobState();
		UpdateWorkProgress();
		UpdateJobList();
	}
	else if (eCALB_EXPOSURE_FEM_TIMER_REQ_LURIA_LOAD_STATE == nIDEvent)
	{
		ReqLuriaState();
	}
	else if (eCALB_EXPOSURE_FEM_TIMER_PRINT_PROGRESS == nIDEvent)
	{
		CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_RESULT];

		if (TRUE == m_bPrinting)
		{
			if (GREEN_ != pGrid->GetItemBkColour(m_nPrintRow, m_nPrintCol))
			{
				if (TRUE == m_bBlink[eCALB_EXPOSURE_FEM_TIMER_PRINT_PROGRESS])
				{
					pGrid->SetItemBkColour(m_nPrintRow, m_nPrintCol, WHITE_);
				}
				else
				{
					pGrid->SetItemBkColour(m_nPrintRow, m_nPrintCol, YELLOW);
				}

				m_bBlink[eCALB_EXPOSURE_FEM_TIMER_PRINT_PROGRESS] = !m_bBlink[eCALB_EXPOSURE_FEM_TIMER_PRINT_PROGRESS];
				pGrid->Refresh();
			}
		}
		else
		{
			m_bBlink[eCALB_EXPOSURE_FEM_TIMER_PRINT_PROGRESS] = FALSE;
		}
	}

	CDlgSubTab::OnTimer(nIDEvent);
}
