// DlgCalb.cpp: 구현 파일
//

#include "pch.h"
#include "DlgCalbUVPowerMeasure.h"
#include "afxdialogex.h"
#include <iostream>
#include <string>

/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgCalbUVPowerMeasure::CDlgCalbUVPowerMeasure(UINT32 id, CWnd* parent)
	: CDlgSubTab(id, parent)
{
	m_enDlgID = ENG_CMDI_TAB::en_menu_tab_calb_uvpower_measure;
	m_nSelectedHead = 0;
	m_nSelectedMove = 0;
	m_bManualPosMoving = FALSE;
	m_bAutoRead = FALSE;
	m_bViewCtrls = FALSE;

	for (int i = 0; i < eCALB_UVPOWER_MEASURE_TIMER_MAX; i++)
	{
		m_bBlink[i] = FALSE;
	}
}

CDlgCalbUVPowerMeasure::~CDlgCalbUVPowerMeasure()
{
	CPowerMeasureMgr::GetInstance()->SetStopProcess();
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgCalbUVPowerMeasure::DoDataExchange(CDataExchange* dx)
{
	CDlgSubTab::DoDataExchange(dx);

	UINT32 i, u32StartID;

	
	u32StartID = IDC_CALB_UVPOWER_MEASURE_BTN_SAVE;
	for (i = 0; i < eCALB_UVPOWER_MEASURE_BTN_MAX; i++)		DDX_Control(dx, u32StartID + i, m_btn_ctl[i]);

	u32StartID = IDC_CALB_UVPOWER_MEASURE_STT_DATA;
	for (i = 0; i < eCALB_UVPOWER_MEASURE_STT_MAX; i++)		DDX_Control(dx, u32StartID + i, m_stt_ctl[i]);

	u32StartID = IDC_CALB_UVPOWER_MEASURE_TXT_STATE;
	for (i = 0; i < eCALB_UVPOWER_MEASURE_TXT_MAX; i++)		DDX_Control(dx, u32StartID + i, m_txt_ctl[i]);

	u32StartID = IDC_CALB_UVPOWER_MEASURE_PGR_RATE;
	for (i = 0; i < eCALB_UVPOWER_MEASURE_PGR_MAX; i++)		DDX_Control(dx, u32StartID + i, m_pgr_ctl[i]);

	u32StartID = IDC_CALB_UVPOWER_MEASURE_GRD_DATA;
	for (i = 0; i < eCALB_UVPOWER_MEASURE_GRD_MAX; i++)		DDX_Control(dx, u32StartID + i, m_grd_ctl[i]);

	u32StartID = IDC_CALB_UVPOWER_MEASURE_CMB_WAVELENGTH;
	for (i = 0; i < eCALB_UVPOWER_MEASURE_CMB_MAX; i++)		DDX_Control(dx, u32StartID + i, m_cmb_ctl[i]);

}

BEGIN_MESSAGE_MAP(CDlgCalbUVPowerMeasure, CDlgSubTab)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CALB_UVPOWER_MEASURE_BTN_SAVE, IDC_CALB_UVPOWER_MEASURE_BTN_SAVE + eCALB_UVPOWER_MEASURE_BTN_MAX, OnBtnClick)
	ON_NOTIFY(NM_CLICK, IDC_CALB_UVPOWER_MEASURE_GRD_SEL_HEAD, &CDlgCalbUVPowerMeasure::OnClickGridSelectHead)
	ON_NOTIFY(NM_CLICK, IDC_CALB_UVPOWER_MEASURE_GRD_SETTING, &CDlgCalbUVPowerMeasure::OnClickGridSetting)
	ON_NOTIFY(NM_CLICK, IDC_CALB_UVPOWER_MEASURE_GRD_POSITION, &CDlgCalbUVPowerMeasure::OnClickGridPosition)
	ON_NOTIFY(NM_CLICK, IDC_CALB_UVPOWER_MEASURE_GRD_MANUAL, &CDlgCalbUVPowerMeasure::OnClickGridManualIndex)
	ON_NOTIFY(NM_CLICK, IDC_CALB_UVPOWER_MEASURE_GRD_MANUAL_AUTOREAD, &CDlgCalbUVPowerMeasure::OnClickGridAutoRead)
	ON_MESSAGE(eMSG_UV_POWER_INPUT_TABLE, InputTableValue)
	ON_MESSAGE(eMSG_UV_POWER_INPUT_PROGRESS, InputProgress)
	ON_MESSAGE(eMSG_UV_POWER_INPUT_XY_POS, InputXYPosition)
	ON_MESSAGE(eMSG_UV_POWER_INPUT_Z_POS, InputZPosition)
	ON_WM_TIMER()
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgCalbUVPowerMeasure::PreTranslateMessage(MSG* msg)
{
	if (msg->message == WM_KEYDOWN)
	{
		if (msg->wParam == VK_ESCAPE)
		{
			m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_DATA].SetFocusCell(-1, -1);
			m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_DATA].SetSelectedRange(-1, -1, -1, -1);
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
BOOL CDlgCalbUVPowerMeasure::OnInitDlg()
{
	int nHeadCnt = uvEng_GetConfig()->luria_svc.ph_count;

	/* 컨트롤 초기화 */
	InitCtrl();

	InitGridSetting();
	InitGridSelectHead(nHeadCnt);
	InitGridPosition(nHeadCnt);
	InitGridMeasureManualSet();
	InitGridMeasureManualAutoRead();
	AttachButton();

	UpdateGridPosition();
	UpdateGridMeasureManualSet();
	
	ViewControls(FALSE);

	InitGridData(*CPowerMeasureMgr::GetInstance()->GetMeasureTable());

	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgCalbUVPowerMeasure::OnExitDlg()
{
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
VOID CDlgCalbUVPowerMeasure::OnPaintDlg(CDC* dc)
{

	/* 자식 윈도 호출 */
	CDlgSubTab::DrawOutLine();
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgCalbUVPowerMeasure::OnResizeDlg()
{
}

/*
 desc : 부모 스레드에 의해 주기적으로 호출됨
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgCalbUVPowerMeasure::UpdatePeriod(UINT64 tick, BOOL is_busy)
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
VOID CDlgCalbUVPowerMeasure::UpdateControl(UINT64 tick, BOOL is_busy)
{
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgCalbUVPowerMeasure::InitCtrl()
{
	CResizeUI clsResizeUI;
	CString strTemp;
	CRect rStt;

	for (int i = 0; i < eCALB_UVPOWER_MEASURE_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
		// by sysandj : Resize UI
	}

	/* static - Normal */
	for (int i = 0; i < eCALB_UVPOWER_MEASURE_STT_MAX; i++)
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
	for (int i = 0; i < eCALB_UVPOWER_MEASURE_TXT_MAX; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_txt_ctl[i]);
		// by sysandj : Resize UI
	}

	for (int i = 0; i < eCALB_UVPOWER_MEASURE_PGR_MAX; i++)
	{
		m_pgr_ctl[i].SetBkColor(ALICE_BLUE);
		m_pgr_ctl[i].SetTextColor(BLACK_);
		m_pgr_ctl[i].SetRange(0, 100);
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_pgr_ctl[i]);
		// by sysandj : Resize UI
	}

	m_pgr_ctl[eCALB_UVPOWER_MEASURE_PGR_RATE].SetShowPercent();

	for (int i = 0; i < eCALB_UVPOWER_MEASURE_CMB_MAX; i++)
	{
		m_cmb_ctl[i].SetLogFont(&g_lf[eFONT_LEVEL2_BOLD]);;
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_cmb_ctl[i]);
		// by sysandj : Resize UI
	}

	for (int nLed = 1; nLed <= uvEng_GetConfig()->luria_svc.led_count; nLed++)
	{
		strTemp.Format(_T("%d nm"), uvEng_Luria_GetLedNoToFreq(nLed));
		m_cmb_ctl[eCALB_UVPOWER_MEASURE_CMB_WAVELENGTH].AddString(strTemp);
	}

	m_cmb_ctl[eCALB_UVPOWER_MEASURE_CMB_WAVELENGTH].SetCurSel(0);

	for (int i = 0; i < eCALB_UVPOWER_MEASURE_GRD_MAX; i++)
	{
		m_grd_ctl[i].SetParent(this);
	}
}

VOID CDlgCalbUVPowerMeasure::InitGridData(VCT_LED_TABLE& pVctPowerIndex, BOOL bIsReload/* = FALSE*/)
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <int>			vRowSize(pVctPowerIndex.size() + 1);
	std::vector <int>			vColSize(2);
	std::vector <std::wstring>	vTitle = { _T("INDEX"), _T("POWER") };
	
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_DATA];

	pGrid->DeleteAllItems();
	pGrid->Refresh();

	if (FALSE == bIsReload)
	{
		clsResizeUI.ResizeControl(this, pGrid);
	}
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
	for (auto& width : vColSize)
	{
		width = rGrid.Width() / (int)vColSize.size();
		nTotalWidth += width;
	}

	vColSize.front() += rGrid.Width() - nTotalWidth - 2;

	if (rGrid.Height() < nTotalHeight)
	{
		vColSize.front() -= ::GetSystemMetrics(SM_CXVSCROLL);
	}

	/* 객체 기본 설정 */
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	pGrid->SetRowCount((int)vRowSize.size());
	pGrid->SetColumnCount((int)vColSize.size());
	pGrid->SetRowHeight(0, nHeight);
	pGrid->SetFixedColumnCount(0);
	pGrid->SetBkColor(RGB(255, 255, 255));
	pGrid->SetFixedColumnSelection(0);

	/* 타이틀 (첫 번째 행) */
	for (int i = 0; i < (int)vColSize.size(); i++)
	{
		pGrid->SetItemFormat(0, i, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pGrid->SetColumnWidth(i, vColSize[i]);
		pGrid->SetItemText(0, i, vTitle[i].c_str());
		pGrid->SetItemBkColour(0, i, ALICE_BLUE);
		pGrid->SetItemFgColour(0, i, BLACK_);
	}


	for (int i = 1; i < (int)vRowSize.size(); i++)
	{
		pGrid->SetItemFormat(i, 0, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pGrid->SetItemFormat(i, 1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pGrid->SetRowHeight(i, nHeight);
		pGrid->SetItemTextFmt(i, 0, _T("%d"), pVctPowerIndex.at(i - 1).u16Index);
		pGrid->SetItemTextFmt(i, 1, _T("%.3f"), pVctPowerIndex.at(i - 1).dPower);
		pGrid->SetItemBkColour(i, 0, ALICE_BLUE);
		pGrid->SetItemFgColour(i, 0, BLACK_);
		pGrid->SetItemBkColour(i, 1, WHITE_);
		pGrid->SetItemFgColour(i, 1, BLACK_);
	}

	/* 기본 속성 및 갱신 */
	pGrid->SetBaseGridProp(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	pGrid->Refresh();
}

VOID CDlgCalbUVPowerMeasure::InitGridSetting()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <int>			vColSize(eSETTING_COL_MAX);
	std::vector <int>			vRowSize(eSETTING_ROW_MAX);
	std::vector <std::wstring>	vTitle[eSETTING_ROW_MAX];
	
	vTitle[eSETTING_ROW_MAX_ILL] = { _T("Search For Max Illuminance Pos"), _T("OFF") , _T("SET") };
	vTitle[eSETTING_ROW_ALL_WAVE] = { _T("Measure All WaveLength"), _T("OFF") , _T("SET") };
	vTitle[eSETTING_ROW_ALL_PH] = { _T("Measure All PhotoHead"), _T("OFF") , _T("SET") };
	std::vector <COLORREF>		vColColor	= { ALICE_BLUE, WHITE_, ALICE_BLUE };

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_SETTING];

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
	for (auto& width : vColSize)
	{
		width = rGrid.Width() / (int)vColSize.size() - 50;
		nTotalWidth += width;
	}

	vColSize.front() += rGrid.Width() - nTotalWidth - 2;

	/* 객체 기본 설정 */
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	pGrid->SetRowCount((int)vRowSize.size());
	pGrid->SetColumnCount((int)vColSize.size());
	pGrid->SetFixedColumnCount(0);
	pGrid->SetBkColor(RGB(255, 255, 255));
	pGrid->SetFixedColumnSelection(0);

	/* 타이틀 (첫 번째 행) */
	for (int nRow = 0; nRow < (int)vRowSize.size(); nRow++)
	{
		pGrid->SetRowHeight(nRow, nHeight);

		for (int nCol = 0; nCol < (int)vColSize.size(); nCol++)
		{
			pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			pGrid->SetColumnWidth(nCol, vColSize[nCol]);
			pGrid->SetItemText(nRow, nCol, vTitle[nRow][nCol].c_str());
			pGrid->SetItemBkColour(nRow, nCol, vColColor[nCol]);

			if (eSETTING_COL_VALUE == nCol)
			{
				pGrid->SetCellType(nRow, nCol, RUNTIME_CLASS(CGridCellCombo));
				CStringArray options;
				options.Add(_T("OFF"));
				options.Add(_T("ON"));
				CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, nCol);
				pComboCell->SetOptions(options);
				pComboCell->SetStyle(CBS_DROPDOWN);
			}
			else
			{
				pGrid->SetItemState(nRow, nCol, GVIS_READONLY);
			}
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

VOID CDlgCalbUVPowerMeasure::InitGridSelectHead(int nHeadCount /*= DEF_DEFAULT_HEAD_COUNT*/)
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <int>			vRowSize(1);
	std::vector <int>			vColSize(nHeadCount);
	std::vector <std::wstring>	vTitle(nHeadCount);
	std::vector <COLORREF>		vColColor(nHeadCount);

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_SEL_HEAD];

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
		vTitle[nIndex]		= _T("HEAD") + std::to_wstring(nIndex + 1);
		vColColor[nIndex]	= DEF_COLOR_BTN_PAGE_NORMAL;
		width = rGrid.Width() / (int)vColSize.size();
		nTotalWidth += width;

		nIndex++;
	}

	vColSize.front() += rGrid.Width() - nTotalWidth - 2;

	/* 객체 기본 설정 */
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
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

	pGrid->SetItemBkColour(0, m_nSelectedHead, DEF_COLOR_BTN_MENU_SELECT);

	/* 기본 속성 및 갱신 */
	pGrid->SetBaseGridProp(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}

VOID CDlgCalbUVPowerMeasure::InitGridPosition(int nHeadCount /*= DEF_DEFAULT_HEAD_COUNT*/)
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <int>			vRowSize(nHeadCount + 1);
	std::vector <int>			vColSize(eGRD_COL_POSITION_MAX);
	std::vector <std::wstring>	vTitle			= { _T("PH INDEX"), _T("X [mm]"), _T("Y [mm]"), _T("Z [mm]"), _T("BUTTON"), _T("BUTTON") };
	std::vector <std::wstring>	vItem			= { _T("HEAD"), _T("0.0000"), _T("0.0000"), _T("0.0000"), _T("READ"), _T("MOVE") };
	std::vector <COLORREF>		vColColor		= { ALICE_BLUE, WHITE_, WHITE_, WHITE_, DEF_COLOR_BTN_PAGE_NORMAL, DEF_COLOR_BTN_PAGE_NORMAL };
	std::vector <COLORREF>		vColTextColor	= { BLACK_, BLACK_, BLACK_, BLACK_, WHITE_, WHITE_ };

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_POSITION];

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
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	pGrid->SetRowCount((int)vRowSize.size());
	pGrid->SetColumnCount((int)vColSize.size());
	pGrid->SetFixedColumnCount(0);
	pGrid->SetGridLineColor(BLACK_);
	pGrid->SetBkColor(WHITE_);
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

VOID CDlgCalbUVPowerMeasure::UpdateGridPosition()
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_POSITION];

	for (int nRow = 0; nRow < (int)pGrid->GetRowCount(); nRow++)
	{
		/* Title Skip */
		if (0 == nRow)
		{
			continue;
		}

		for (int nCol = 0; nCol < (int)pGrid->GetColumnCount(); nCol++)
		{
			if (0 == nCol)
			{
				continue;
			}
			else if (nCol > _countof(uvEng_GetConfig()->led_power.measure_pos[nRow - 1]))
			{
				break;
			}

			pGrid->SetItemTextFmt(nRow, nCol, _T("%.4f"), uvEng_GetConfig()->led_power.measure_pos[nRow - 1][nCol - 1]);
		}
	}

	pGrid->Refresh();
}

VOID CDlgCalbUVPowerMeasure::InitGridMeasureManualSet()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;

	std::vector <int>			vRowSize(9);
	std::vector <int>			vColSize(3);
	std::vector <std::wstring>	vTitle[9];

	vTitle[0] = { _T("Begin Power Index"),	_T("200") ,		_T("Index") };
	vTitle[1] = { _T("Measure Count"),		_T("10") ,		_T("Cnt") };
	vTitle[2] = { _T("Interval Power"),		_T("10") ,		_T("Index") };
	vTitle[3] = { _T("Max Points"),			_T("9") ,		_T("Cnt") };
	vTitle[4] = { _T("Pitch"),				_T("10") ,		_T("mm") };
	vTitle[5] = { _T("Focus Scan"),			_T("10") ,		_T("mm") };
	vTitle[6] = { _T("Move Speed XY"),		_T("10") ,		_T("mm/s") };
	vTitle[7] = { _T("Move Speed Z"),		_T("10") ,		_T("mm/s") };
	vTitle[8] = { _T("Manual Power"),		_T("1000") ,	_T("Index") };

	std::vector <COLORREF>		vColColor = { ALICE_BLUE, WHITE_, ALICE_BLUE };

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_MANUAL];

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
		width = rGrid.Width() / (int)vColSize.size();
		nTotalWidth += width;
	}

	vColSize.front() += rGrid.Width() - nTotalWidth - 2;

	/* 객체 기본 설정 */
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	pGrid->SetRowCount((int)vRowSize.size());
	pGrid->SetColumnCount((int)vColSize.size());
	pGrid->SetFixedColumnCount(0);
	pGrid->SetBkColor(RGB(255, 255, 255));
	pGrid->SetFixedColumnSelection(0);

	/* 타이틀 (첫 번째 행) */
	for (int nRow = 0; nRow < (int)vRowSize.size(); nRow++)
	{
		pGrid->SetRowHeight(nRow, vRowSize[nRow]);
		for (int nCol = 0; nCol < (int)vColSize.size(); nCol++)
		{
			pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			pGrid->SetColumnWidth(nCol, vColSize[nCol]);
			pGrid->SetItemText(nRow, nCol, vTitle[nRow][nCol].c_str());
			pGrid->SetItemBkColour(nRow, nCol, vColColor[nCol]);

			if (nRow == eGRD_ROW_MANUAL_MAX_POINTS && nCol == eGRD_COL_MANUAL_PARAM)
			{
				pGrid->SetCellType(nRow, nCol, RUNTIME_CLASS(CGridCellCombo));
				CStringArray options;
				options.Add(_T("9"));
				options.Add(_T("25"));
				options.Add(_T("49"));
				CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, nCol);
				pComboCell->SetOptions(options);
				pComboCell->SetStyle(CBS_DROPDOWN);
			}
		}
	}
	
	/* 기본 속성 및 갱신 */
	//pGrid->SetBaseGridProp(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	pGrid->EnableDragAndDrop(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}

VOID CDlgCalbUVPowerMeasure::UpdateGridMeasureManualSet()
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_MANUAL];
	int nRow = 0;
	LPG_CLPI pParam = &uvEng_GetConfig()->led_power;

	pGrid->SetItemTextFmt(nRow++, 1, _T("%d"), pParam->begin_power_index);
	pGrid->SetItemTextFmt(nRow++, 1, _T("%d"), pParam->measure_count);
	pGrid->SetItemTextFmt(nRow++, 1, _T("%d"), pParam->interval_power);
	pGrid->SetItemTextFmt(nRow++, 1, _T("%d"), pParam->max_points);
	pGrid->SetItemTextFmt(nRow++, 1, _T("%.4f"), pParam->pitch_pos);
	pGrid->SetItemTextFmt(nRow++, 1, _T("%.4f"), pParam->focus_scan_pos);
	pGrid->SetItemTextFmt(nRow++, 1, _T("%.3f"), pParam->stage_speed);
	pGrid->SetItemTextFmt(nRow++, 1, _T("%.3f"), pParam->head_speed);
	pGrid->SetItemTextFmt(nRow++, 1, _T("%d"), pParam->manual_power_index);

	CPowerMeasureMgr::GetInstance()->SetPowerTable(pParam->measure_count, pParam->begin_power_index, pParam->interval_power);
	CPowerMeasureMgr::GetInstance()->SetFindPosParam(pParam->max_points, pParam->pitch_pos, pParam->focus_scan_pos);
	CPowerMeasureMgr::GetInstance()->SetManualIndexPower(pParam->manual_power_index);

	pGrid->Refresh();
}

VOID CDlgCalbUVPowerMeasure::InitGridMeasureManualAutoRead()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <int>			vRowSize(1);
	std::vector <int>			vColSize(3);
	std::vector <std::wstring>	vTitle = { _T("Measure Power"), _T("0.00") , _T("AUTO READ") };
	std::vector <COLORREF>		vColColor = { ALICE_BLUE, WHITE_, DEF_COLOR_BTN_PAGE_NORMAL };
	std::vector <COLORREF>		vColTextColor = { BLACK_, BLACK_, WHITE_ };

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_MANUAL_AUTOREAD];

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
	for (auto& width : vColSize)
	{
		width = rGrid.Width() / (int)vColSize.size();
		nTotalWidth += width;
	}

	vColSize.front() += rGrid.Width() - nTotalWidth - 2;

	/* 객체 기본 설정 */
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	pGrid->SetRowCount((int)vRowSize.size());
	pGrid->SetColumnCount((int)vColSize.size());
	pGrid->SetRowHeight(0, nHeight);
	pGrid->SetFixedColumnCount(0);
	pGrid->SetBkColor(RGB(255, 255, 255));
	pGrid->SetFixedColumnSelection(0);

	/* 타이틀 (첫 번째 행) */
	for (int i = 0; i < (int)vColSize.size(); i++)
	{
		pGrid->SetItemFormat(0, i, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pGrid->SetColumnWidth(i, vColSize[i]);
		pGrid->SetItemText(0, i, vTitle[i].c_str());
		pGrid->SetItemBkColour(0, i, vColColor[i]);
		pGrid->SetItemFgColour(0, i, vColTextColor[i]);
	}

	/* 기본 속성 및 갱신 */
	pGrid->SetBaseGridProp(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}

VOID CDlgCalbUVPowerMeasure::AttachButton()
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_POSITION];
	CMacButton* pBtn = &m_btn_ctl[eCALB_UVPOWER_MEASURE_BTN_OPEN_CTRL_PANEL];
	CRect rGrid, rBtn;
	int nHeight = 0;

	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	pBtn->GetWindowRect(rBtn);
	this->ScreenToClient(rBtn);

	nHeight = rBtn.Height();

	rBtn.top = rGrid.bottom + 3;
	rBtn.bottom = rBtn.top + nHeight;

	pBtn->MoveWindow(rBtn);
	Invalidate();
}

/*
 desc : 객체 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgCalbUVPowerMeasure::InitObject()
{

	return TRUE;
}

/*
 desc : 객체 해제
 parm : None
 retn : None
*/
VOID CDlgCalbUVPowerMeasure::CloseObject()
{
}

VOID CDlgCalbUVPowerMeasure::ViewControls(BOOL bView/* = TRUE*/)
{
	CString	strText;

	m_btn_ctl[eCALB_UVPOWER_MEASURE_BTN_OPEN_CTRL_PANEL].ShowWindow(bView);
	m_btn_ctl[eCALB_UVPOWER_MEASURE_BTN_MANUAL_ON].ShowWindow(bView);
	m_btn_ctl[eCALB_UVPOWER_MEASURE_BTN_MANUAL_OFF].ShowWindow(bView);
	m_btn_ctl[eCALB_UVPOWER_MEASURE_BTN_SAVE].ShowWindow(bView);

	m_stt_ctl[eCALB_UVPOWER_MEASURE_STT_POSITION].ShowWindow(bView);
	m_stt_ctl[eCALB_UVPOWER_MEASURE_STT_MANUAL_MEASURE].ShowWindow(bView);

	m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_POSITION].ShowWindow(bView);
	m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_MANUAL].ShowWindow(bView);
	m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_MANUAL_AUTOREAD].ShowWindow(bView);

	strText = (TRUE == bView) ? _T("HIDE OPTION") : _T("VIEW OPTION");
	m_btn_ctl[eCALB_UVPOWER_MEASURE_BTN_VIEW_OPTION].SetWindowText(strText);
}

/*
 desc : 일반 버튼 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgCalbUVPowerMeasure::OnBtnClick(UINT32 id)
{
	int nBtnID = id - IDC_CALB_UVPOWER_MEASURE_BTN_SAVE;
	
	if (TRUE == CPowerMeasureMgr::GetInstance()->IsProcessWorking() &&
		nBtnID != eCALB_UVPOWER_MEASURE_BTN_STOP)
	{
		AfxMessageBox(_T("현재 측정중입니다."));
		return;
	}

	switch (nBtnID)
	{
	case eCALB_UVPOWER_MEASURE_BTN_SAVE:
		SaveParam();
		break;
	case eCALB_UVPOWER_MEASURE_BTN_START:
		MeasureStart();
		break;
	case eCALB_UVPOWER_MEASURE_BTN_STOP:
		MeasureStop();
		break;
	case eCALB_UVPOWER_MEASURE_BTN_OPEN_CTRL_PANEL:
		ShowCtrlPanel();
		break;
	case eCALB_UVPOWER_MEASURE_BTN_MANUAL_ON:
		ManualUvOn();
		break;
	case eCALB_UVPOWER_MEASURE_BTN_MANUAL_OFF:
		ManualUvOff();
		break;
	case eCALB_UVPOWER_MEASURE_BTN_VIEW_OPTION:
		/* 추후 계정 등급 적용 필요 */
		m_bViewCtrls = !m_bViewCtrls;
		ViewControls(m_bViewCtrls);
		break;
	case eCALB_UVPOWER_MEASURE_BTN_VIEW_SAVE_RESULT:
		SaveResult();
	default:
		break;
	}
}

void CDlgCalbUVPowerMeasure::MeasureStart()
{
	if (TRUE == m_bManualPosMoving)
	{
		AfxMessageBox(_T("현재 이동중입니다."));
		return;
	}

	KillTimer(eCALB_UVPOWER_MEASURE_TIMER_START);

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_SETTING];
	LPG_CLPI pParam = &uvEng_GetConfig()->led_power;
	BOOL bSet = FALSE;

	CPowerMeasureMgr::GetInstance()->SetPhotoHeadOption(m_nSelectedHead + 1,
		m_cmb_ctl[eCALB_UVPOWER_MEASURE_CMB_WAVELENGTH].GetCurSel() + 1);

	CPowerMeasureMgr::GetInstance()->SetPowerTable(pParam->measure_count, pParam->begin_power_index, pParam->interval_power);
	InitGridData(*CPowerMeasureMgr::GetInstance()->GetMeasureTable(), TRUE);

	bSet = (0 == pGrid->GetItemText(eSETTING_ROW_MAX_ILL, eSETTING_COL_VALUE).Compare(_T("ON"))) ? TRUE : FALSE;
	CPowerMeasureMgr::GetInstance()->SetMoveMaxIll(bSet);

	bSet = (0 == pGrid->GetItemText(eSETTING_ROW_ALL_WAVE, eSETTING_COL_VALUE).Compare(_T("ON"))) ? TRUE : FALSE;
	CPowerMeasureMgr::GetInstance()->SetMoveAllWave(bSet);

	bSet = (0 == pGrid->GetItemText(eSETTING_ROW_ALL_PH, eSETTING_COL_VALUE).Compare(_T("ON"))) ? TRUE : FALSE;
	CPowerMeasureMgr::GetInstance()->SetMoveAllPH(bSet);

	m_cmb_ctl[eCALB_UVPOWER_MEASURE_CMB_WAVELENGTH].EnableWindow(FALSE);
	pGrid->EnableWindow(FALSE);

	m_txt_ctl[eCALB_UVPOWER_MEASURE_TXT_STATE].SetTextToStr(_T("NONE"));
	m_pgr_ctl[eCALB_UVPOWER_MEASURE_PGR_RATE].SetPos(0);
	m_btn_ctl[eCALB_UVPOWER_MEASURE_BTN_START].SetBgColor(DEF_COLOR_BTN_PAGE_SELECT);
	Invalidate(FALSE);

	if (IDNO == AfxMessageBox(L"측정을 시작하시겠습니까?", MB_YESNO))
	{
		pGrid->EnableWindow(TRUE);
		m_cmb_ctl[eCALB_UVPOWER_MEASURE_CMB_WAVELENGTH].EnableWindow(TRUE);
		return;
	}
	
	SetTimer(eCALB_UVPOWER_MEASURE_TIMER_START, 1000, NULL);

	if (FALSE == CPowerMeasureMgr::GetInstance()->MeasureStart(this->GetSafeHwnd()))
	{
		AfxMessageBox(_T("측정 실패"));
	}
}

void CDlgCalbUVPowerMeasure::MeasureStop()
{
	KillTimer(eCALB_UVPOWER_MEASURE_TIMER_START);

	m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_SETTING].EnableWindow(TRUE);
	m_cmb_ctl[eCALB_UVPOWER_MEASURE_CMB_WAVELENGTH].EnableWindow(TRUE);
	m_btn_ctl[eCALB_UVPOWER_MEASURE_BTN_START].SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
	Invalidate(FALSE);

	CPowerMeasureMgr::GetInstance()->SetStopProcess();
}

void CDlgCalbUVPowerMeasure::ShowCtrlPanel()
{
	if (TRUE == m_bManualPosMoving)
	{
		AfxMessageBox(_T("현재 이동중입니다."));
		return;
	}

	// 230419 mhbaek Add : MotorConsole 연동
	::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), eMSG_MAIN_OPEN_CONSOLE, NULL, NULL);
}

void CDlgCalbUVPowerMeasure::ManualUvOn()
{
	// 230516 mhbaek Add
	m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_SETTING].EnableWindow(FALSE);
	m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_SEL_HEAD].EnableWindow(FALSE);
	m_cmb_ctl[eCALB_UVPOWER_MEASURE_CMB_WAVELENGTH].EnableWindow(FALSE);

	m_btn_ctl[eCALB_UVPOWER_MEASURE_BTN_MANUAL_ON].SetBgColor(TOMATO);
	m_btn_ctl[eCALB_UVPOWER_MEASURE_BTN_MANUAL_OFF].SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
	Invalidate(FALSE);


	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_MANUAL];
	auto manualPower = _ttoi(pGrid->GetItemText(eGRD_ROW_MANUAL_POWER_INDEX, eGRD_COL_MANUAL_PARAM));


	//메뉴얼 파워의 경우는 config 이 아닌 입력된 메뉴얼 광량을 기준으로 작동하게 한다. (수정)
	CPowerMeasureMgr::GetInstance()->PhotoLedOnOff(m_nSelectedHead + 1,
		m_cmb_ctl[eCALB_UVPOWER_MEASURE_CMB_WAVELENGTH].GetCurSel() + 1,
		manualPower,
		TRUE);

	/*CPowerMeasureMgr::GetInstance()->PhotoLedOnOff(m_nSelectedHead + 1,
		m_cmb_ctl[eCALB_UVPOWER_MEASURE_CMB_WAVELENGTH].GetCurSel() + 1,
		uvEng_GetConfig()->led_power.manual_power_index,
		TRUE);*/

	m_ullTimeManualON = GetTickCount64();
	SetTimer(eCALB_UVPOWER_MEASURE_TIMER_UV_ON, 1000, NULL);
}

void CDlgCalbUVPowerMeasure::ManualUvOff()
{
	// 230516 mhbaek Add
	m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_SETTING].EnableWindow(TRUE);
	m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_SEL_HEAD].EnableWindow(TRUE);
	m_cmb_ctl[eCALB_UVPOWER_MEASURE_CMB_WAVELENGTH].EnableWindow(TRUE);

	KillTimer(eCALB_UVPOWER_MEASURE_TIMER_UV_ON);
	m_ullTimeManualON = 0;

	CPowerMeasureMgr::GetInstance()->PhotoLedOnOff(m_nSelectedHead + 1,
		m_cmb_ctl[eCALB_UVPOWER_MEASURE_CMB_WAVELENGTH].GetCurSel() + 1,
		0,
		FALSE);

	m_btn_ctl[eCALB_UVPOWER_MEASURE_BTN_MANUAL_ON].SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
	m_btn_ctl[eCALB_UVPOWER_MEASURE_BTN_MANUAL_OFF].SetBgColor(DEF_COLOR_BTN_PAGE_SELECT);
	Invalidate(FALSE);
}

void CDlgCalbUVPowerMeasure::SaveParam()
{
	if (IDNO == AfxMessageBox(L"저장하시겠습니까?", MB_YESNO))
	{
		return;
	}

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_POSITION];
	LPG_CLPI pParam = &uvEng_GetConfig()->led_power;

	for (int nRow = 0; nRow < pGrid->GetRowCount(); nRow++)
	{
		if (eGRD_ROW_POSITION_TITLE == nRow)
		{
			continue;
		}

		pParam->measure_pos[nRow - 1][0] = _ttof(pGrid->GetItemText(nRow, eGRD_COL_POSITION_POS_X));
		pParam->measure_pos[nRow - 1][1] = _ttof(pGrid->GetItemText(nRow, eGRD_COL_POSITION_POS_Y));
		pParam->measure_pos[nRow - 1][2] = _ttof(pGrid->GetItemText(nRow, eGRD_COL_POSITION_POS_Z));
	}

	pGrid = &m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_MANUAL];
	pParam->begin_power_index = _ttoi(pGrid->GetItemText(eGRD_ROW_MANUAL_BEGIN_POWER_INDEX, eGRD_COL_MANUAL_PARAM));
	pParam->measure_count = _ttoi(pGrid->GetItemText(eGRD_ROW_MANUAL_MEASURE_COUNT, eGRD_COL_MANUAL_PARAM));
	pParam->interval_power = _ttoi(pGrid->GetItemText(eGRD_ROW_MANUAL_INTERVAL_POWER, eGRD_COL_MANUAL_PARAM));
	pParam->max_points = _ttoi(pGrid->GetItemText(eGRD_ROW_MANUAL_MAX_POINTS, eGRD_COL_MANUAL_PARAM));
	pParam->pitch_pos = _ttof(pGrid->GetItemText(eGRD_ROW_MANUAL_PITCH_POS, eGRD_COL_MANUAL_PARAM));
	pParam->focus_scan_pos = _ttof(pGrid->GetItemText(eGRD_ROW_MANUAL_FOCUS_SCAN_POS, eGRD_COL_MANUAL_PARAM));
	pParam->stage_speed = _ttof(pGrid->GetItemText(eGRD_ROW_MANUAL_STAGE_SPEED, eGRD_COL_MANUAL_PARAM));
	pParam->head_speed = _ttof(pGrid->GetItemText(eGRD_ROW_MANUAL_HEAD_SPEED, eGRD_COL_MANUAL_PARAM));
	pParam->manual_power_index = _ttoi(pGrid->GetItemText(eGRD_ROW_MANUAL_POWER_INDEX, eGRD_COL_MANUAL_PARAM));

	CPowerMeasureMgr::GetInstance()->SetPowerTable(pParam->measure_count, pParam->begin_power_index, pParam->interval_power);
	CPowerMeasureMgr::GetInstance()->SetFindPosParam(pParam->max_points, pParam->pitch_pos, pParam->focus_scan_pos);
	CPowerMeasureMgr::GetInstance()->SetManualIndexPower(pParam->manual_power_index);

	uvEng_SaveConfig();

	InitGridData(*CPowerMeasureMgr::GetInstance()->GetMeasureTable(), TRUE);
}


void CDlgCalbUVPowerMeasure::SaveResult()
{
	if (TRUE == CPowerMeasureMgr::GetInstance()->IsProcessWorking())
	{
		AfxMessageBox(_T("현재 측정중입니다."));
		return;
	}

	if (0 >= CPowerMeasureMgr::GetInstance()->GetMeasureTable()->size())
	{
		AfxMessageBox(_T("측정 테이블이 존재하지 않습니다."));
		return;
	}

	if (0 == CPowerMeasureMgr::GetInstance()->GetMeasureTable()->back().dPower)
	{
		if (IDNO == AfxMessageBox(L"아직 측정 테이블이 완성되지 않았습니다.\n저장하시겠습니까?", MB_YESNO))
		{
			return;
		}
	}

	if (IDNO == AfxMessageBox(L"측정된 테이블을 저장하시겠습니까?", MB_YESNO))
	{
		return;
	}

	CPowerMeasureMgr::GetInstance()->SaveAllHeadtableFile();
}

void CDlgCalbUVPowerMeasure::OnClickGridSelectHead(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_SEL_HEAD];

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	if (0 > pItem->iColumn)
	{
		return;
	}
	else if (TRUE == CPowerMeasureMgr::GetInstance()->IsProcessWorking())
	{
		AfxMessageBox(_T("현재 측정중입니다."));
		return;
	}

	m_nSelectedHead = pItem->iColumn;


	for (int i = 0; i < pGrid->GetColumnCount(); i++)
	{
		pGrid->SetItemBkColour(0, i, DEF_COLOR_BTN_PAGE_NORMAL);
	}
	
	pGrid->SetItemBkColour(0, m_nSelectedHead, DEF_COLOR_BTN_MENU_SELECT);
	pGrid->Refresh();
}


void CDlgCalbUVPowerMeasure::OnClickGridSetting(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_SETTING];

	if (0 > pItem->iColumn)
	{
		return;
	}

	if (eSETTING_COL_VALUE != pItem->iColumn)
	{
		/* Reset Focus */
		pGrid->SetFocusCell(-1, -1);
	}

	//m_nSelectedHead = pItem->iColumn;
	// 	for (int nCol = 0; nCol < pGrid->GetColumnCount(); nCol++)
// 	{
// 		pGrid->SetItemBkColour(0, nCol, DEF_COLOR_BTN_PAGE_NORMAL);
// 	}
// 
// 	pGrid->SetItemBkColour(0, m_nSelectedHead, DEF_COLOR_BTN_MENU_SELECT);
// 	pGrid->Refresh();
}


void CDlgCalbUVPowerMeasure::OnClickGridPosition(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_POSITION];

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	if (eGRD_COL_POSITION_POS_X > pItem->iColumn ||
		eGRD_ROW_POSITION_TITLE >= pItem->iRow)
	{
		return;
	}
	else if (TRUE == m_bManualPosMoving)
	{
		AfxMessageBox(_T("현재 이동중입니다."));
		return;
	}
	else if (TRUE == CPowerMeasureMgr::GetInstance()->IsProcessWorking())
	{
		AfxMessageBox(_T("현재 측정중입니다."));
		return;
	}


	switch (pItem->iColumn)
	{
	case eGRD_COL_POSITION_POS_X:
	case eGRD_COL_POSITION_POS_Y:
	case eGRD_COL_POSITION_POS_Z:
	{
		CString strOutput;
		CString strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn);
		double	dMin = -9999.9999;
		double	dMax = 9999.9999;

		strInput.TrimRight();

		if (PopupKBDN(ENM_DITM::en_double, strInput, strOutput, dMin, dMax, 4))
		{
			pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);
		}

		break;
	}
	case eGRD_COL_POSITION_READ:
	{
		if (IDNO == AfxMessageBox(L"수정하시겠습니까?", MB_YESNO))
		{
			return;
		}

		UINT8 u8ConvHeadNo = (UINT8)ENG_MMDI::en_axis_ph1 + (pItem->iRow - 1);
		pGrid->SetItemTextFmt(pItem->iRow, eGRD_COL_POSITION_POS_X, _T("%.4f"), uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x));
		pGrid->SetItemTextFmt(pItem->iRow, eGRD_COL_POSITION_POS_Y, _T("%.4f"), uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y));
		pGrid->SetItemTextFmt(pItem->iRow, eGRD_COL_POSITION_POS_Z, _T("%.4f"), uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(u8ConvHeadNo)));

		break;
	}
	case eGRD_COL_POSITION_MOVE:
	{
		if (IDNO == AfxMessageBox(L"이동하시겠습니까?", MB_YESNO))
		{
			return;
		}

		LPG_CLPI pParam = &uvEng_GetConfig()->led_power;
		UINT8 u8ConvHeadNo = (UINT8)ENG_MMDI::en_axis_ph1 + (pItem->iRow - 1);

		double dX = _ttof(pGrid->GetItemText(pItem->iRow, eGRD_COL_POSITION_POS_X));
		double dY = _ttof(pGrid->GetItemText(pItem->iRow, eGRD_COL_POSITION_POS_Y));
		double dZ = _ttof(pGrid->GetItemText(pItem->iRow, eGRD_COL_POSITION_POS_Z));

		m_nSelectedMove = pItem->iRow;

		m_bManualPosMoving = TRUE;
		m_ullTimeHeadMove = GetTickCount64();
		SetTimer(eCALB_UVPOWER_MEASURE_TIMER_MOVING, 500, NULL);

		if (0.005 < fabs(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x) - dX))
		{
			if (FALSE == uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, dX, pParam->stage_speed))
			{
				return;
			}
		}

		if (0.005 < fabs(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y) - dY))
		{
			if (FALSE == uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dY, pParam->stage_speed))
			{
				return;
			}
		}

		if (0.005 < fabs(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(u8ConvHeadNo)) - dZ))
		{
			if (FALSE == uvEng_MC2_SendDevAbsMove(ENG_MMDI(u8ConvHeadNo), dZ, pParam->head_speed))
			{
				return;
			}
		}

		break;
	}
	default:
		return;
	}
	
	pGrid->Refresh();
}

void CDlgCalbUVPowerMeasure::OnClickGridManualIndex(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_MANUAL];
	ENM_DITM enType;
	UINT8 u8DecPts = 0;

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	if (1 != pItem->iColumn)
	{
		return;
	}
	else if (TRUE == CPowerMeasureMgr::GetInstance()->IsProcessWorking())
	{
		AfxMessageBox(_T("현재 측정중입니다."));
		return;
	}

	CString strOutput;
	CString strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn);
	double	dMin = -9999.9999;
	double	dMax = 9999.9999;

	if (pItem->iRow == eGRD_ROW_MANUAL_MAX_POINTS && pItem->iColumn == eGRD_COL_MANUAL_PARAM)
	{
		return;
	}

	switch (pItem->iRow)
	{
	case eGRD_ROW_MANUAL_BEGIN_POWER_INDEX:
	case eGRD_ROW_MANUAL_MEASURE_COUNT:
	case eGRD_ROW_MANUAL_INTERVAL_POWER:
	case eGRD_ROW_MANUAL_POWER_INDEX:
		dMin = 0;
		dMax = 5000;
		enType = ENM_DITM::en_int16;
		break;
	case eGRD_ROW_MANUAL_PITCH_POS:
	case eGRD_ROW_MANUAL_FOCUS_SCAN_POS:
		enType = ENM_DITM::en_double;
		u8DecPts = 4;
		break;
	case eGRD_ROW_MANUAL_STAGE_SPEED:
	case eGRD_ROW_MANUAL_HEAD_SPEED:
		enType = ENM_DITM::en_double;
		u8DecPts = 3;;
		break;
	default:
		return;
	}

	if (PopupKBDN(enType, strInput, strOutput, dMin, dMax, u8DecPts))
	{
		pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);
	}

	pGrid->Refresh();
}


void CDlgCalbUVPowerMeasure::OnClickGridAutoRead(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	INT16 nWave = uvEng_Luria_GetLedNoToFreq(m_cmb_ctl[eCALB_UVPOWER_MEASURE_CMB_WAVELENGTH].GetCurSel() + 1);
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_MANUAL_AUTOREAD];

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	if (2 != pItem->iColumn)
	{
		return;
	}
	else if (TRUE == CPowerMeasureMgr::GetInstance()->IsProcessWorking())
	{
		AfxMessageBox(_T("현재 측정중입니다."));
		return;
	}

	if (FALSE == m_bAutoRead)
	{
		uvEng_Gentec_ResetValue();
		uvEng_Gentec_RunMeasure(TRUE);

		uvEng_Gentec_SetQueryWaveLength(nWave);

		if (nWave != uvEng_Gentec_GetQueryWaveLength())
		{

			uvEng_Gentec_SetQueryWaveLength(nWave);
		}

		m_bAutoRead = TRUE;
		pGrid->SetItemText(0, 2, _T("OFF"));
		m_ullTimeAutoRead = GetTickCount64();
		SetTimer(eCALB_UVPOWER_MEASURE_TIMER_AUTOREAD, 500, NULL);
	}
	else
	{
		uvEng_Gentec_RunMeasure(FALSE);
		m_bAutoRead = FALSE;
		pGrid->SetItemText(0, 2, _T("AUTO READ"));
		pGrid->SetItemBkColour(0, 2, DEF_COLOR_BTN_PAGE_NORMAL);
		m_ullTimeAutoRead = 0;
		KillTimer(eCALB_UVPOWER_MEASURE_TIMER_AUTOREAD);
		pGrid->Refresh();
	}
}

void CDlgCalbUVPowerMeasure::OnTimer(UINT_PTR nIDEvent)
{
	if (eCALB_UVPOWER_MEASURE_TIMER_START == nIDEvent)
	{
		if (FALSE == CPowerMeasureMgr::GetInstance()->IsProcessWorking())
		{
			m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_SETTING].EnableWindow(TRUE);
			m_cmb_ctl[eCALB_UVPOWER_MEASURE_CMB_WAVELENGTH].EnableWindow(TRUE);
			m_btn_ctl[eCALB_UVPOWER_MEASURE_BTN_START].SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
			Invalidate(FALSE);

			KillTimer(nIDEvent);
			
// 			if (IDYES == AfxMessageBox(L"측정 테이블이 수정되었습니다.\n결과를 저장하시겠습니까?", MB_YESNO))
// 			{
// 				CPowerMeasureMgr::GetInstance()->MeasureEnd();
// 			}
		}
	}

	if (eCALB_UVPOWER_MEASURE_TIMER_AUTOREAD == nIDEvent)
	{
		CGridCtrl*	pGrid	= &m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_MANUAL_AUTOREAD];
		LPG_PGDV stValue = uvEng_Gentec_GetValue();

		if (NULL != stValue)
		{
			pGrid->SetItemTextFmt(0, 1, _T("%.3f"), stValue->last_val);
		}

		if (m_bBlink[nIDEvent - 1])
		{
			pGrid->SetItemBkColour(0, 2, DEF_COLOR_BTN_PAGE_NORMAL);
		}
		else
		{
			pGrid->SetItemBkColour(0, 2, SEA_GREEN);
		}

		m_bBlink[nIDEvent - 1] = !m_bBlink[nIDEvent - 1];
		pGrid->Refresh();

		if (GetTickCount64() - m_ullTimeAutoRead > DEF_DEFAULT_AUTO_READ_TIME)
		{
			uvEng_Gentec_RunMeasure(FALSE);
			m_bAutoRead = FALSE;
			pGrid->SetItemText(0, 2, _T("AUTO READ"));
			pGrid->SetItemBkColour(0, 2, DEF_COLOR_BTN_PAGE_NORMAL);
			m_ullTimeAutoRead = 0;
			KillTimer(nIDEvent);
		}
	}

	if (eCALB_UVPOWER_MEASURE_TIMER_UV_ON == nIDEvent)
	{
		if (m_bBlink[nIDEvent - 1])
		{
			m_btn_ctl[eCALB_UVPOWER_MEASURE_BTN_MANUAL_ON].SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
		}
		else
		{
			m_btn_ctl[eCALB_UVPOWER_MEASURE_BTN_MANUAL_ON].SetBgColor(TOMATO);
		}

		m_bBlink[nIDEvent - 1] = !m_bBlink[nIDEvent - 1];

		if (GetTickCount64() - m_ullTimeManualON > DEF_DEFAULT_INTERLOCK_TIMEOUT)
		{
			ManualUvOff();
		}

		m_btn_ctl[eCALB_UVPOWER_MEASURE_BTN_MANUAL_ON].Invalidate(FALSE);
	}

	if (eCALB_UVPOWER_MEASURE_TIMER_MOVING == nIDEvent)
	{
		if (0 >= m_nSelectedMove)
		{
			KillTimer(nIDEvent);
		}

		CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_POSITION];
		UINT8 u8ConvHeadNo = (UINT8)ENG_MMDI::en_axis_ph1 + (m_nSelectedMove - 1);

		double dCurX = 0;
		double dCurY = 0;
		double dCurZ = 0;
		double dTargetX = 0;
		double dTargetY = 0;
		double dTargetZ = 0;

		if (m_bBlink[nIDEvent - 1])
		{
			pGrid->SetItemBkColour(m_nSelectedMove, eGRD_COL_POSITION_POS_X, WHITE_);
			pGrid->SetItemBkColour(m_nSelectedMove, eGRD_COL_POSITION_POS_Y, WHITE_);
			pGrid->SetItemBkColour(m_nSelectedMove, eGRD_COL_POSITION_POS_Z, WHITE_);
		}
		else
		{
			pGrid->SetItemBkColour(m_nSelectedMove, eGRD_COL_POSITION_POS_X, SEA_GREEN);
			pGrid->SetItemBkColour(m_nSelectedMove, eGRD_COL_POSITION_POS_Y, SEA_GREEN);
			pGrid->SetItemBkColour(m_nSelectedMove, eGRD_COL_POSITION_POS_Z, SEA_GREEN);
		}

		m_bBlink[nIDEvent - 1] = !m_bBlink[nIDEvent - 1];
		pGrid->Refresh();

		/* TIME OUT 1 Min */
		if (GetTickCount64() - m_ullTimeHeadMove > DEF_DEFAULT_INTERLOCK_TIMEOUT)
		{
			pGrid->SetItemBkColour(m_nSelectedMove, eGRD_COL_POSITION_POS_X, DEF_COLOR_BTN_PAGE_NORMAL);
			pGrid->SetItemBkColour(m_nSelectedMove, eGRD_COL_POSITION_POS_Y, DEF_COLOR_BTN_PAGE_NORMAL);
			pGrid->SetItemBkColour(m_nSelectedMove, eGRD_COL_POSITION_POS_Z, DEF_COLOR_BTN_PAGE_NORMAL);

			m_ullTimeHeadMove = 0;
			m_nSelectedMove = 0;
			m_bManualPosMoving = FALSE;
			KillTimer(nIDEvent);
		}

		dTargetX = _ttof(pGrid->GetItemText(m_nSelectedMove, eGRD_COL_POSITION_POS_X));
		dTargetY = _ttof(pGrid->GetItemText(m_nSelectedMove, eGRD_COL_POSITION_POS_Y));
		dTargetZ = _ttof(pGrid->GetItemText(m_nSelectedMove, eGRD_COL_POSITION_POS_Z));
		dCurX = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);
		dCurY = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);
		dCurZ = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(u8ConvHeadNo));

		if (0.001 > fabs(dTargetX - dCurX) &&
			0.001 > fabs(dTargetY - dCurY) &&
			0.001 > fabs(dTargetZ - dCurZ))
		{
			pGrid->SetItemBkColour(m_nSelectedMove, eGRD_COL_POSITION_POS_X, WHITE_);
			pGrid->SetItemBkColour(m_nSelectedMove, eGRD_COL_POSITION_POS_Y, WHITE_);
			pGrid->SetItemBkColour(m_nSelectedMove, eGRD_COL_POSITION_POS_Z, WHITE_);

			m_nSelectedMove = 0;
			m_bManualPosMoving = FALSE;
			KillTimer(nIDEvent);
		}
	}

	CDlgSubTab::OnTimer(nIDEvent);
}

BOOL CDlgCalbUVPowerMeasure::PopupKBDN(ENM_DITM enType, CString strInput, CString &strOutput, double dMin, double dMax, UINT8 u8DecPts/* = 0*/)
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

LRESULT CDlgCalbUVPowerMeasure::InputTableValue(WPARAM wParam, LPARAM lParam)
{
	CString strText = *(CString*)lParam;

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_DATA];

	// 측정 테이블 UI 갱신
	if (1 == (int)wParam && LIGHT_GRAY == pGrid->GetItemBkColour((int)wParam, 1))
	{
		InitGridData(*CPowerMeasureMgr::GetInstance()->GetMeasureTable(), TRUE);
	}

	pGrid->SetItemTextFmt((int)wParam, 1, strText);
	pGrid->SetCellView((int)wParam, 1);
	pGrid->SetItemBkColour((int)wParam, 1, LIGHT_GRAY);

	pGrid->Refresh();
	return 0;
}

LRESULT CDlgCalbUVPowerMeasure::InputProgress(WPARAM wParam, LPARAM lParam)
{
	CString strText = *(CString*)lParam;

	m_pgr_ctl[eCALB_UVPOWER_MEASURE_PGR_RATE].SetPos((int)wParam);
	m_txt_ctl[eCALB_UVPOWER_MEASURE_TXT_STATE].SetTextToStr((PTCHAR)(LPCTSTR)strText);
	return 0;
}

LRESULT CDlgCalbUVPowerMeasure::InputXYPosition(WPARAM wParam, LPARAM lParam)
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_POSITION];
	LPG_CLPI pParam = &uvEng_GetConfig()->led_power;

	pParam->measure_pos[m_nSelectedHead][0] = *(double*)wParam;
	pParam->measure_pos[m_nSelectedHead][1] = *(double*)lParam;

	pGrid->SetItemTextFmt(m_nSelectedHead + 1, eGRD_COL_POSITION_POS_X, _T("%.4f"), *(double*)wParam);
	pGrid->SetItemTextFmt(m_nSelectedHead + 1, eGRD_COL_POSITION_POS_Y, _T("%.4f"), *(double*)lParam);
	pGrid->Refresh();

	uvEng_SaveConfig();
	return 0;
}

LRESULT CDlgCalbUVPowerMeasure::InputZPosition(WPARAM wParam, LPARAM lParam)
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_POSITION];
	LPG_CLPI pParam = &uvEng_GetConfig()->led_power;

	pParam->measure_pos[m_nSelectedHead][2] = *(double*)wParam;

	pGrid->SetItemTextFmt(m_nSelectedHead + 1, eGRD_COL_POSITION_POS_Z, _T("%.4f"), *(double*)wParam);
	pGrid->Refresh();

	uvEng_SaveConfig();
	return 0;
}