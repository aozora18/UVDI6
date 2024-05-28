// DlgCalb.cpp: 구현 파일
//

#include "pch.h"
#include "DlgCalbCameraSpec.h"
#include "afxdialogex.h"
#include <iostream>
#include <string>

/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgCalbCameraSpec::CDlgCalbCameraSpec(UINT32 id, CWnd* parent)
	: CDlgSubTab(id, parent)
{
	m_enDlgID = ENG_CMDI_TAB::en_menu_tab_calb_camera_spec;
	m_bValidResult = FALSE;
	m_bViewCtrls = FALSE;
	m_bCamLiveState = FALSE;
	m_bEditMode = FALSE;
	m_stVctResult.clear();
	m_u8SelHead = 0;

// 	STG_CRD stRoi = { NULL };
// 	CRect tmpRoi;
// 
// 	uvEng_Mark_SetMarkROI(&stRoi, 0);
// 	uvCmn_Camera_MilSetMarkROI(0, tmpRoi);
// 	uvEng_Mark_MarkROISave();
}

CDlgCalbCameraSpec::~CDlgCalbCameraSpec()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgCalbCameraSpec::DoDataExchange(CDataExchange* dx)
{
	CDlgSubTab::DoDataExchange(dx);

	UINT32 i, u32StartID;

	u32StartID = IDC_CALB_CAMERA_SPEC_BTN_TEST_MATCH;
	for (i = 0; i < eCALB_CAMERA_SPEC_BTN_MAX; i++)		DDX_Control(dx, u32StartID + i, m_btn_ctl[i]);

	u32StartID = IDC_CALB_CAMERA_SPEC_STT_CAMERA_VIEW;
	for (i = 0; i < eCALB_CAMERA_SPEC_STT_MAX; i++)		DDX_Control(dx, u32StartID + i, m_stt_ctl[i]);

	u32StartID = IDC_CALB_CAMERA_SPEC_TXT_STATE;
	for (i = 0; i < eCALB_CAMERA_SPEC_TXT_MAX; i++)		DDX_Control(dx, u32StartID + i, m_txt_ctl[i]);

	u32StartID = IDC_CALB_CAMERA_SPEC_PIC_ROI;
	for (i = 0; i < eCALB_CAMERA_SPEC_TXT_MAX; i++)		DDX_Control(dx, u32StartID + i, m_pic_ctl[i]);

	u32StartID = IDC_CALB_CAMERA_SPEC_PGR_RATE;
	for (i = 0; i < eCALB_CAMERA_SPEC_PGR_MAX; i++)		DDX_Control(dx, u32StartID + i, m_pgr_ctl[i]);

	u32StartID = IDC_CALB_CAMERA_SPEC_GRD_SELECT_CAM;
	for (i = 0; i < eCALB_CAMERA_SPEC_GRD_MAX; i++)		DDX_Control(dx, u32StartID + i, m_grd_ctl[i]);

}


BEGIN_MESSAGE_MAP(CDlgCalbCameraSpec, CDlgSubTab)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CALB_CAMERA_SPEC_BTN_TEST_MATCH, IDC_CALB_CAMERA_SPEC_BTN_TEST_MATCH + eCALB_CAMERA_SPEC_BTN_MAX, OnBtnClick)
	ON_NOTIFY(NM_CLICK, IDC_CALB_CAMERA_SPEC_GRD_SELECT_CAM, &CDlgCalbCameraSpec::OnClickGridSelectCam)
	ON_NOTIFY(NM_CLICK, IDC_CALB_CAMERA_SPEC_GRD_RESULT, &CDlgCalbCameraSpec::OnClickGridResult)
	ON_NOTIFY(NM_CLICK, IDC_CALB_CAMERA_SPEC_GRD_SELECT_MODE, &CDlgCalbCameraSpec::OnClickGridSelectMode)
	ON_NOTIFY(NM_CLICK, IDC_CALB_CAMERA_SPEC_GRD_REPEAT_COUNT, &CDlgCalbCameraSpec::OnClickGridRepeatCount)
	ON_NOTIFY(NM_CLICK, IDC_CALB_CAMERA_SPEC_GRD_TRIGGER_OPTION, &CDlgCalbCameraSpec::OnClickGridTriggerOption)
	ON_NOTIFY(NM_CLICK, IDC_CALB_CAMERA_SPEC_GRD_OPTION, &CDlgCalbCameraSpec::OnClickGridOption)
	ON_NOTIFY(NM_CLICK, IDC_CALB_CAMERA_SPEC_GRD_CAMERA_OPTION, &CDlgCalbCameraSpec::OnClickGridCameraOption)
	ON_NOTIFY(NM_CLICK, IDC_CALB_CAMERA_SPEC_GRD_POSITION, &CDlgCalbCameraSpec::OnClickGridPosition)
	ON_MESSAGE(eMSG_CAMERA_SPEC_PROGRESS, UpdateMeasureProgress)
	ON_MESSAGE(eMSG_CAMERA_SPEC_RESULT, UpdateCaliResult)
	ON_MESSAGE(eMSG_CAMERA_SPEC_COMPLETE, MeasureComplete)
	ON_MESSAGE(eMSG_CAMERA_SPEC_GRAB_VIEW, RefreshGrabView)
	//ON_MESSAGE(WM_MAIN_THREAD, OnMsgThread)
END_MESSAGE_MAP()


/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgCalbCameraSpec::PreTranslateMessage(MSG* msg)
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
BOOL CDlgCalbCameraSpec::OnInitDlg()
{
	/* 컨트롤 초기화 */
	InitCtrl();

	InitGridSelectCam();
	InitGridSelectMode();
	InitGridRepeatCount();
	InitGridReslut();
	InitGridTriggerOption();
	InitGridOption();
	InitGridCameraOption();
	InitDispMark();
	InitGridPosition();

	LoadDataConfig();
	ViewControls(FALSE);
	//SetLiveView(FALSE);

#ifndef CAM_SPEC_SIMUL
	if (FALSE == CCamSpecMgr::GetInstance()->SetRegistModel())
	{
		AfxMessageBox(L"Failed to setup the model for search target", MB_ICONSTOP | MB_TOPMOST);
	}
#endif // !CAM_SPEC_SIMUL

	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgCalbCameraSpec::OnExitDlg()
{
	for (int i = 0; i < _countof(m_grd_ctl); i++)
	{
		m_grd_ctl[i].DeleteAllItems();
	}

	m_stVctResult.clear();
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgCalbCameraSpec::OnPaintDlg(CDC* dc)
{
	//ENG_VCCM enMode = uvEng_Camera_GetCamMode();
	//RECT rDraw;

	///* 이미지가 출력될 윈도 영역 */
	//m_pic_ctl[0].GetWindowRect(&rDraw);
	//ScreenToClient(&rDraw);

	//if (enMode == ENG_VCCM::en_cali_mode)
	//{
	//	uvEng_Camera_DrawCaliBitmap(dc->m_hDC, rDraw);
	//}

	/* 자식 윈도 호출 */
	CDlgSubTab::DrawOutLine();
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgCalbCameraSpec::OnResizeDlg()
{
}


/*
 desc : 컨트롤
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgCalbCameraSpec::UpdateControl(UINT64 tick, BOOL is_busy)
{
}


/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgCalbCameraSpec::InitCtrl()
{
	CResizeUI clsResizeUI;
	CString strTemp;
		CRect rStt;

	for (int i = 0; i < eCALB_CAMERA_SPEC_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
		// by sysandj : Resize UI
	}

	/* static - Normal */
	for (int i = 0; i < eCALB_CAMERA_SPEC_STT_MAX; i++)
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
	for (int i = 0; i < eCALB_CAMERA_SPEC_TXT_MAX; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_txt_ctl[i]);
		// by sysandj : Resize UI
	}

	for (int i = 0; i < eCALB_CAMERA_SPEC_PIC_MAX; i++)
	{
 		//m_pic_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
 		//m_pic_ctl[i].SetDrawBg(1);
 		//m_pic_ctl[i].SetBaseProp(0, 1, 0, 0, BLACK_, 0, WHITE_);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_pic_ctl[i]);
		// by sysandj : Resize UI
	}

	for (int i = 0; i < eCALB_CAMERA_SPEC_PGR_MAX; i++)
	{
		m_pgr_ctl[i].SetBkColor(ALICE_BLUE);
		m_pgr_ctl[i].SetTextColor(BLACK_);
		m_pgr_ctl[i].SetRange(0, 100);
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_pgr_ctl[i]);
		// by sysandj : Resize UI
	}

	m_pgr_ctl[eCALB_CAMERA_SPEC_PGR_RATE].SetShowPercent();

	for (int i = 0; i < eCALB_CAMERA_SPEC_GRD_MAX; i++)
	{
		m_grd_ctl[i].SetParent(this);
	}
}


VOID CDlgCalbCameraSpec::InitGridSelectCam()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <int>			vRowSize(1);
	std::vector <int>			vColSize(CAMER_ACOUNT);
	std::vector <std::wstring>	vTitle(CAMER_ACOUNT);
	std::vector <COLORREF>		vColColor(CAMER_ACOUNT);

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_SELECT_CAM];

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
		vTitle[nIndex] = _T("CAMERA") + std::to_wstring(nIndex + 1);
		vColColor[nIndex] = DEF_COLOR_BTN_PAGE_NORMAL;
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

	pGrid->SetItemBkColour(0, 0, DEF_COLOR_BTN_MENU_SELECT);

	/* 기본 속성 및 갱신 */
	pGrid->SetBaseGridProp(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}

VOID CDlgCalbCameraSpec::InitGridReslut()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <COLORREF>		vColColor = { ALICE_BLUE, LIGHT_GRAY, ALICE_BLUE };
	std::vector <int>			vColSize(eRESULT_COL_MAX);
	std::vector <int>			vRowSize(eRESULT_ROW_MAX);
	std::vector <std::wstring>	vTitle[eRESULT_ROW_MAX];

	vTitle[0] = { _T("Angle"), _T("0.000") , _T("º") };
	vTitle[1] = { _T("Pixel"), _T("0.000") , _T("um") };
	vTitle[2] = { _T("Score"), _T("0.000") , _T("%") };
	vTitle[3] = { _T("Scale"), _T("0.000") , _T("%") };
	vTitle[4] = { _T("Mark Offset X"), _T("0.000") , _T("mm") };
	vTitle[5] = { _T("Mark Offset Y"), _T("0.000") , _T("mm") };

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_RESULT];

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
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
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

VOID CDlgCalbCameraSpec::InitGridSelectMode()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <int>			vRowSize(eSETTING_ROW_MAX);
	std::vector <int>			vColSize(eSETTING_COL_MAX_MODE);
	std::vector <std::wstring>	vTitle = { _T("ANGLE"), _T("PIXEL SIZE") };
	std::vector <COLORREF>		vColColor(eSETTING_COL_MAX_MODE);
	int nMode = (int)CCamSpecMgr::GetInstance()->GetRunMode();

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_SELECT_MODE];

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
		pGrid->SetItemFgColour(0, i, WHITE_);
	}

	pGrid->SetItemBkColour(0, nMode, DEF_COLOR_BTN_MENU_SELECT);

	/* 기본 속성 및 갱신 */
	pGrid->SetBaseGridProp(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}


VOID CDlgCalbCameraSpec::InitGridRepeatCount()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <COLORREF>		vColColor = { ALICE_BLUE, WHITE_ };
	std::vector <int>			vColSize(eSETTING_COL_MAX);
	std::vector <std::wstring>	vTitle;

	vTitle = { _T("Repeat Count"),		_T("10")};

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_REPEAT_COUNT];

	clsResizeUI.ResizeControl(this, pGrid);
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	int nHeight = (int)(DEF_DEFAULT_GRID_ROW_SIZE * clsResizeUI.GetRateY());

	int nTotalWidth = 0;
	for (auto& width : vColSize)
	{
		width = rGrid.Width() / (int)vColSize.size() - 50;
		nTotalWidth += width;
	}

	vColSize.front() += rGrid.Width() - nTotalWidth - 2;

	/* 객체 기본 설정 */
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	pGrid->SetRowCount(1);
	pGrid->SetColumnCount((int)vColSize.size());
	pGrid->SetFixedColumnCount(0);
	pGrid->SetBkColor(RGB(255, 255, 255));
	pGrid->SetFixedColumnSelection(0);
	pGrid->SetRowHeight(0, nHeight);

	for (int nCol = 0; nCol < (int)vColSize.size(); nCol++)
	{
		pGrid->SetItemFormat(0, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pGrid->SetColumnWidth(nCol, vColSize[nCol]);
		pGrid->SetItemText(0, nCol, vTitle[nCol].c_str());
		pGrid->SetItemBkColour(0, nCol, vColColor[nCol]);
		pGrid->SetItemState(0, nCol, GVIS_READONLY);
	}

	/* Grid Size 재구성 */
	rGrid.bottom = rGrid.top + nHeight + 1;
	pGrid->MoveWindow(rGrid);

	/* 기본 속성 및 갱신 */
	pGrid->EnableDragAndDrop(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}


VOID CDlgCalbCameraSpec::InitGridTriggerOption()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <COLORREF>		vColColor = { ALICE_BLUE, WHITE_, ALICE_BLUE };
	std::vector <int>			vColSize(eOPTION_COL_MAX);
	std::vector <int>			vRowSize(eOPTION_ROW_TRIGGER_MAX);
	std::vector <std::wstring>	vTitle[eOPTION_ROW_TRIGGER_MAX];

	vTitle[0] = { _T("Trigger On-Time"),		_T("0"),			_T("ns") };
	vTitle[1] = { _T("Strobe On-Time"),			_T("0"),			_T("ns") };
	vTitle[2] = { _T("Delay Time"),				_T("0"),			_T("ns") };

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_TRIGGER_OPTION];

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
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
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


VOID CDlgCalbCameraSpec::InitGridOption()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <COLORREF>		vColColor = { ALICE_BLUE, WHITE_, ALICE_BLUE };
	std::vector <int>			vColSize(eOPTION_COL_MAX);
	std::vector <int>			vRowSize(eOPTION_ROW_MAX);
	std::vector <std::wstring>	vTitle[eOPTION_ROW_MAX];

	vTitle[0] = { _T("Stage Measure Pos X"),		_T("0.0000"),		_T("mm") };
	vTitle[1] = { _T("Stage Measure Pos Y"),		_T("0.0000"),		_T("mm") };
	vTitle[2] = { _T("Camera Measure Pos X"),		_T("0.0000"),		_T("mm") };
	vTitle[3] = { _T("Camera Measure Pos Z"),		_T("0.0000"),		_T("mm") };
	vTitle[4] = { _T("Grab Width"),					_T("1200"),			_T("Pixel") };
	vTitle[5] = { _T("Grab Height"),				_T("600"),			_T("Pixel") };

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_OPTION];

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
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
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


VOID CDlgCalbCameraSpec::InitGridCameraOption()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <COLORREF>		vColCellColor = { ALICE_BLUE, WHITE_, g_clrBtnColor };
	std::vector <COLORREF>		vColTextColor = { BLACK_, BLACK_, g_clrBtnTextColor };
	std::vector <int>			vColSize(eCAM_OPTION_COL_MAX);
	std::vector <int>			vRowSize(eCAM_OPTION_ROW_MAX);
	std::vector <std::wstring>	vTitle[eCAM_OPTION_ROW_MAX];

	vTitle[0] = { _T("Cam1 Gain Level"),		_T("0"),		_T("SET") };
	vTitle[1] = { _T("Cam2 Gain Level"),		_T("0"),		_T("SET") };

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_CAMERA_OPTION];

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
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
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
			pGrid->SetItemBkColour(nRow, nCol, vColCellColor[nCol]);
			pGrid->SetItemFgColour(nRow, nCol, vColTextColor[nCol]);
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


VOID CDlgCalbCameraSpec::InitGridPosition()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <int>			vRowSize(1);
	std::vector <int>			vColSize(CAMER_ACOUNT);
	std::vector <std::wstring>	vTitle(CAMER_ACOUNT);
	std::vector <COLORREF>		vColColor(CAMER_ACOUNT);

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_POSITION];

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
		vTitle[nIndex] = _T("MOVE CAMERA ") + std::to_wstring(nIndex + 1) + _T(" POS");
		vColColor[nIndex] = DEF_COLOR_BTN_PAGE_NORMAL;
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

	/* 기본 속성 및 갱신 */
	pGrid->SetBaseGridProp(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}


VOID CDlgCalbCameraSpec::InitDispMark()
{
	CWnd* pWnd[2];
	pWnd[0] = GetDlgItem(IDC_CALB_CAMERA_SPEC_PIC_ROI);
	pWnd[1] = NULL;
	uvEng_Camera_SetDisp(pWnd, 0x01);
	DispResize(pWnd[0]);

	uvEng_Camera_DrawOverlayDC(false, DISP_TYPE_CALB_CAMSPEC, 0);
}

/* desc: Frame Control을 이미지 사이즈 비율로 재조정 */
void CDlgCalbCameraSpec::DispResize(CWnd* pWnd)
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_SELECT_CAM];
	CRect rGrid;
	CRect lpRect;
	int nHeight = 0;

	pGrid->GetWindowRect(rGrid);
	ScreenToClient(rGrid);

	pWnd->GetWindowRect(lpRect);
	ScreenToClient(lpRect);

	CSize szDraw(lpRect.right - lpRect.left, lpRect.bottom - lpRect.top);
	CSize tgtSize;

	/* 반드시 1.0f 값으로 초기화 */
	double tgt_rate = 1.0f;

	/* 가로 크기 비율이 세로 크기 비율보다 큰 경우 */
	if ((DOUBLE(szDraw.cx) / DOUBLE(uvEng_GetConfig()->set_cams.spc_size[0])) > (DOUBLE(szDraw.cy) / DOUBLE(uvEng_GetConfig()->set_cams.spc_size[1])))
	{
		/* <세로> 비율 기준으로 영상 확대 or 축소 진행 */
		tgt_rate = DOUBLE(szDraw.cy) / DOUBLE(uvEng_GetConfig()->set_cams.spc_size[1]);
	}
	/* 세로 크기 비율이 가로 크기 비율보다 큰 경우 */
	else
	{
		/* <가로> 비율 기준으로 영상 확대 or 축소 진행 */
		tgt_rate = DOUBLE(szDraw.cx) / DOUBLE(uvEng_GetConfig()->set_cams.spc_size[0]);
	}
	/* 윈도 이미지 영역에 맞게 영상 이미지 크기 조정 */
	tgtSize.cx = (int)(tgt_rate * uvEng_GetConfig()->set_cams.spc_size[0]);
	tgtSize.cy = (int)(tgt_rate * uvEng_GetConfig()->set_cams.spc_size[1]);

	lpRect.left = ((lpRect.right - lpRect.left) / 2 + lpRect.left) - (tgtSize.cx / 2);
	lpRect.top = ((lpRect.bottom - lpRect.top) / 2 + lpRect.top) - (tgtSize.cy / 2);
	lpRect.right = lpRect.left + tgtSize.cx;
	lpRect.bottom = lpRect.top + tgtSize.cy;

	nHeight = lpRect.Height();

	lpRect.top = rGrid.bottom + 3;
	lpRect.bottom = lpRect.top + nHeight;

	pWnd->MoveWindow(lpRect, TRUE);

	uvEng_Camera_SetCalbCamSpecDispSize(tgtSize);
}

/*
 desc : 이미지 뷰 영역 갱신
 parm : None
 retn : None
*/
VOID CDlgCalbCameraSpec::InvalidateView()
{
	CRect rView;
	m_pic_ctl[eCALB_CAMERA_SPEC_PIC_ROI].GetWindowRect(rView);
	ScreenToClient(rView);
	InvalidateRect(rView, TRUE);
}


VOID CDlgCalbCameraSpec::ResetResult()
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_RESULT];

	for (int i = 0; i < eRESULT_ROW_MAX; i++)
	{
		pGrid->SetItemText(i, eRESULT_COL_VALUE, _T("0.000"));
	}

	m_bValidResult = FALSE;
	pGrid->Refresh();
}

VOID CDlgCalbCameraSpec::ViewControls(BOOL bView)
{
	CString	strText;

	m_btn_ctl[eCALB_CAMERA_SPEC_BTN_OPEN_CTRL_PANEL].ShowWindow(bView);
	m_btn_ctl[eCALB_CAMERA_SPEC_BTN_SAVE].ShowWindow(bView);
	m_stt_ctl[eCALB_CAMERA_SPEC_STT_TRIGGER_OPTION].ShowWindow(bView);
	m_grd_ctl[eCALB_CAMERA_SPEC_GRD_TRIGGER_OPTION].ShowWindow(bView);
	m_stt_ctl[eCALB_CAMERA_SPEC_STT_OPTION].ShowWindow(bView);
	m_grd_ctl[eCALB_CAMERA_SPEC_GRD_OPTION].ShowWindow(bView);
	m_stt_ctl[eCALB_CAMERA_SPEC_STT_CAMERA_OPTION].ShowWindow(bView);
	m_grd_ctl[eCALB_CAMERA_SPEC_GRD_CAMERA_OPTION].ShowWindow(bView);

	strText = (TRUE == bView) ? _T("HIDE OPTION") : _T("VIEW OPTION");
	m_btn_ctl[eCALB_CAMERA_SPEC_BTN_VIEW_OPTION].SetWindowText(strText);
}


VOID CDlgCalbCameraSpec::SetLiveView(BOOL bLive)
{
	BOOL bSucc = FALSE;

	if (TRUE == bLive)
	{
		m_btn_ctl[eCALB_CAMERA_SPEC_BTN_LIVE].SetBgColor(LIME);
		m_btn_ctl[eCALB_CAMERA_SPEC_BTN_LIVE].SetTextColor(BLACK_);

		/* Align Camera is Live Mode */
		uvEng_Camera_SetCamMode(ENG_VCCM::en_live_mode);

		/* Trigger & Strobe : Enabled */
		//if (uvEng_Mvenc_ReqTriggerStrobe(TRUE))
		//{
		//	bSucc = uvEng_Mvenc_ReqEncoderLive(u8ACamID, m_u8lamp_type);
		//}
		uvEng_Mvenc_ReqWirteTrigLive(m_u8SelHead + 1, TRUE);
	}
	else
	{
		m_btn_ctl[eCALB_CAMERA_SPEC_BTN_LIVE].SetBgColor(g_clrBtnColor);
		m_btn_ctl[eCALB_CAMERA_SPEC_BTN_LIVE].SetTextColor(WHITE_);

		// if (uvEng_Camera_GetCamMode() != ENG_VCCM::en_live_mode)	return;

		/* Trigger & Strobe : Disabled */
		//bSucc = uvEng_Mvenc_ReqEncoderOutReset();
		bSucc = 1;
		/* Trigger & Strobe : Disabled */
		if (bSucc)
		{
			/* Trigger & Strobe : Disabled */
			uvEng_Mvenc_ReqTriggerStrobe(FALSE);
			UINT64 u64Tick = GetTickCount64();

			do
			{
				/* Trigger Board로부터 설정에 대한 응답 값이 왔는지 확인 */
				if (0 == uvEng_ShMem_GetTrig()->enc_out_val)
				{
					/* 최대 1 초 동안 대기 후 루프 빠져나감 */
					if (GetTickCount64() > (u64Tick + 1000))	break;
				}
				else
				{
					/* 최대 2 초 동안 대기 후 응답 없으면, 루프 빠져나감 */
					//if (GetTickCount64() > (u64Tick + 2500))	break;
					if (GetTickCount64() > (u64Tick + 2000))	break;
				}

				Wait_(10);

			} while (1);

			/* Align Camera is Calibration Mode */
			uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
		}
	}

	//Invalidate(FALSE);
	Invalidate(TRUE);
}


VOID CDlgCalbCameraSpec::LoadDataConfig()
{
	UINT8 u8ChNo = GetCheckACam();
	CGridCtrl* pGrdTrigger = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_TRIGGER_OPTION];
	CGridCtrl* pGrdOption = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_OPTION];
	CGridCtrl* pGrdCamOption = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_CAMERA_OPTION];
	LPG_CTSP pTrigger = &uvEng_GetConfig()->trig_step;
	LPG_CASI pOption = &uvEng_GetConfig()->acam_spec;
	LPG_CCDB pCamera = &uvEng_GetConfig()->set_basler;
	LPG_CSCI pFov = &uvEng_GetConfig()->set_cams;

	pGrdTrigger->SetItemText(eOPTION_ROW_TRIGGER_ON_TIME, eOPTION_COL_VALUE, pTrigger->trig_on_time[u8ChNo], FALSE);
	pGrdTrigger->SetItemText(eOPTION_ROW_STROBE_ON_TIME, eOPTION_COL_VALUE, pTrigger->strob_on_time[u8ChNo], FALSE);
	pGrdTrigger->SetItemText(eOPTION_ROW_DELAY_TIME, eOPTION_COL_VALUE, pTrigger->trig_delay_time[u8ChNo], FALSE);

	//pGrdOption->SetItemDouble(eOPTION_ROW_STAGE_MEASURE_POS_X, eOPTION_COL_VALUE, pOption->quartz_stage_x, 4);
	pGrdOption->SetItemDouble(eOPTION_ROW_STAGE_MEASURE_POS_Y, eOPTION_COL_VALUE, pOption->quartz_stage_y[u8ChNo], 4);
	//pGrdOption->SetItemDouble(eOPTION_ROW_CAMERA_MEASURE_POS_X, eOPTION_COL_VALUE, pOption->quartz_acam[u8ChNo], 4);
	pGrdOption->SetItemDouble(eOPTION_ROW_CAMERA_MEASURE_POS_Z, eOPTION_COL_VALUE, pOption->acam_z_focus[u8ChNo], 4);


#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
	pGrdOption->SetItemDouble(eOPTION_ROW_STAGE_MEASURE_POS_X, eOPTION_COL_VALUE, pOption->quartz_stage_x, 4);
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	if (u8ChNo == 2)
	{
		pGrdOption->SetItemDouble(eOPTION_ROW_STAGE_MEASURE_POS_X, eOPTION_COL_VALUE, pOption->quartz_acam[u8ChNo], 4);
		pGrdOption->SetItemDouble(eOPTION_ROW_CAMERA_MEASURE_POS_X, eOPTION_COL_VALUE, 0, 4);
	}
	else
	{
		pGrdOption->SetItemDouble(eOPTION_ROW_STAGE_MEASURE_POS_X, eOPTION_COL_VALUE, pOption->quartz_stage_x, 4);
		pGrdOption->SetItemDouble(eOPTION_ROW_CAMERA_MEASURE_POS_X, eOPTION_COL_VALUE, pOption->quartz_acam[u8ChNo], 4);
	}
#endif

	pGrdOption->SetItemText(eOPTION_ROW_GRAB_WIDTH, eOPTION_COL_VALUE, pFov->spc_size[0]);
	pGrdOption->SetItemText(eOPTION_ROW_GRAB_HEIGHT, eOPTION_COL_VALUE, pFov->spc_size[1]);

	pGrdCamOption->SetItemText(eCAM_OPTION_ROW_GAIN1, eCAM_OPTION_COL_VALUE, pCamera->cam_gain_level[eCAM_OPTION_ROW_GAIN1]);
	pGrdCamOption->SetItemText(eCAM_OPTION_ROW_GAIN2, eCAM_OPTION_COL_VALUE, pCamera->cam_gain_level[eCAM_OPTION_ROW_GAIN2]);

	pGrdTrigger->Refresh();
	pGrdOption->Refresh();
	pGrdCamOption->Refresh();
}


VOID CDlgCalbCameraSpec::SaveDataConfig()
{
	UINT8 u8ChNo = GetCheckACam();
	CGridCtrl* pGrdTrigger = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_TRIGGER_OPTION];
	CGridCtrl* pGrdOption = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_OPTION];
	LPG_CTSP pTrigger = &uvEng_GetConfig()->trig_step;
	LPG_CASI pOption = &uvEng_GetConfig()->acam_spec;
	LPG_CSCI pFov = &uvEng_GetConfig()->set_cams;

	if (IDNO == AfxMessageBox(L"저장하시겠습니까?", MB_YESNO))
	{
		return;
	}

	pTrigger->trig_on_time[u8ChNo] = pGrdOption->GetItemTextToInt(eOPTION_ROW_TRIGGER_ON_TIME, eOPTION_COL_VALUE);
	pTrigger->strob_on_time[u8ChNo] = pGrdOption->GetItemTextToInt(eOPTION_ROW_STROBE_ON_TIME, eOPTION_COL_VALUE);
	pTrigger->trig_delay_time[u8ChNo] = pGrdOption->GetItemTextToInt(eOPTION_ROW_DELAY_TIME, eOPTION_COL_VALUE);

	//pOption->quartz_stage_x = pGrdOption->GetItemTextToFloat(eOPTION_ROW_STAGE_MEASURE_POS_X, eOPTION_COL_VALUE);
	pOption->quartz_stage_y[u8ChNo] = pGrdOption->GetItemTextToFloat(eOPTION_ROW_STAGE_MEASURE_POS_Y, eOPTION_COL_VALUE);
	//pOption->quartz_acam[u8ChNo] = pGrdOption->GetItemTextToFloat(eOPTION_ROW_CAMERA_MEASURE_POS_X, eOPTION_COL_VALUE);
	pOption->acam_z_focus[u8ChNo] = pGrdOption->GetItemTextToFloat(eOPTION_ROW_CAMERA_MEASURE_POS_Z, eOPTION_COL_VALUE);


#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
	pOption->quartz_acam[u8ChNo] = pGrdOption->GetItemTextToFloat(eOPTION_ROW_CAMERA_MEASURE_POS_X, eOPTION_COL_VALUE);
	pOption->quartz_stage_x = pGrdOption->GetItemTextToFloat(eOPTION_ROW_STAGE_MEASURE_POS_X, eOPTION_COL_VALUE);
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	if (u8ChNo == 2)
	{
		pOption->quartz_acam[u8ChNo] = pGrdOption->GetItemTextToFloat(eOPTION_ROW_STAGE_MEASURE_POS_X, eOPTION_COL_VALUE);
	}
	else
	{
		pOption->quartz_acam[u8ChNo] = pGrdOption->GetItemTextToFloat(eOPTION_ROW_CAMERA_MEASURE_POS_X, eOPTION_COL_VALUE);
		pOption->quartz_stage_x = pGrdOption->GetItemTextToFloat(eOPTION_ROW_STAGE_MEASURE_POS_X, eOPTION_COL_VALUE);
	}
#endif

	pFov->spc_size[0] = pGrdOption->GetItemTextToInt(eOPTION_ROW_GRAB_WIDTH, eOPTION_COL_VALUE);
	pFov->spc_size[1] = pGrdOption->GetItemTextToInt(eOPTION_ROW_GRAB_HEIGHT, eOPTION_COL_VALUE);

	/* 환경 설정 값을 파일로 저장 */
	uvEng_SaveConfig();
}


VOID CDlgCalbCameraSpec::SaveResult()
{
	//ENG_MMDI enACamX = (0x00 == GetCheckACam()) ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;
	ENG_MMDI enACamX;
	if (0x00 == GetCheckACam())			enACamX = ENG_MMDI::en_align_cam1;
	else if (0x01 == GetCheckACam())	enACamX = ENG_MMDI::en_align_cam2;
	else if (0x02 == GetCheckACam())	enACamX = ENG_MMDI::en_stage_x;

	LPG_CASI pstACamSpec = &uvEng_GetConfig()->acam_spec;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_RESULT];
	double dValue = 0.;

	if (IDNO == AfxMessageBox(L"결과를 저장하시겠습니까?", MB_YESNO))
	{
		return;
	}

	/* 카메라 설치 회전 각도 */
	dValue = pGrid->GetItemTextToFloat(eRESULT_ROW_ANGLE, eRESULT_COL_VALUE);
	if (0.0f != dValue)
	{
		pstACamSpec->spec_angle[GetCheckACam()] = dValue;
	}

	/* 카메라 픽셀 크기 */
	dValue = pGrid->GetItemTextToFloat(eRESULT_ROW_PIXEL, eRESULT_COL_VALUE);
	if (0.0f != dValue)
	{
		pstACamSpec->spec_pixel_um[GetCheckACam()] = dValue;
	}

	/* 현재 선택된 카메라 번호에 따라, Focus 위치 정보 */
	// by sysandj : MCQ대체 추가 필요

	/* 현재 모션 위치 값 */
	//pstACamSpec->quartz_stage_x = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);
	pstACamSpec->quartz_stage_y[GetCheckACam()] = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);
	pstACamSpec->quartz_acam[GetCheckACam()] = uvCmn_MC2_GetDrvAbsPos(enACamX);

	/* 환경 설정 값을 파일로 저장 */
	uvEng_SaveConfig();

	Wait_(10);

	LoadDataConfig();
}


/*
 desc : 카메라의 조명 타입에 따른 트리거 보드의 채널 번호 값 반환
 parm : None
 retn : 0 or Later
*/
UINT8 CDlgCalbCameraSpec::GetTrigChNo()
{
	UINT8 u8ChNo = 0x00;

	if (0xFF == GetCheckACam())
	{
		return u8ChNo;
	}

	switch (uvEng_GetConfig()->set_comn.strobe_lamp_type)
	{
	case 0x00:	// Amber
		u8ChNo = (0 == GetCheckACam()) ? 0x00 : 0x02;
		break;
	case 0x01:	// IR 
		u8ChNo = (0 == GetCheckACam()) ? 0x01 : 0x03;
		break;
	default:
		return 0xFF;
	}

	return u8ChNo;
}


/*
 desc : 측정 위치로 이동
 parm : None
 retn : None
*/
VOID CDlgCalbCameraSpec::MovePosMeasure(UINT8 u8SelHead)
{
	ENG_MMDI enACamDrv = (0x01 == u8SelHead) ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;
	ENG_MMDI enAvoidCam = (0x01 == u8SelHead) ? ENG_MMDI::en_align_cam2 : ENG_MMDI::en_align_cam1;
	double dMoveACamPos = 0, dMoveXPos = 0, dMoveYPos = 0;
	double dDiffACamPos = 0, dDiffXPos = 0, dDiffYPos = 0;
	CTactTimeCheck cTime;

	/* Quartz Position */
	dMoveXPos = uvEng_GetConfig()->acam_spec.quartz_stage_x;
	dMoveYPos = uvEng_GetConfig()->acam_spec.quartz_stage_y[u8SelHead - 1];
	dMoveACamPos = uvEng_GetConfig()->acam_spec.quartz_acam[u8SelHead - 1];

	/* 현재 이동하고자 하는 모션의 Toggled 값 얻기 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	uvCmn_MC2_GetDrvDoneToggled(enACamDrv);
	uvCmn_MC2_GetDrvDoneToggled(enAvoidCam);

	if (FALSE == uvEng_MC2_SendDevAbsMove(enAvoidCam,
		uvEng_GetConfig()->set_cams.safety_pos[(int)enAvoidCam - (int)ENG_MMDI::en_align_cam1],
		uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)enAvoidCam]))
		return;

	Wait_(500);

	/* Motion 이동 시키기 */
	if (FALSE == uvEng_MC2_SendDevAbsMove(enACamDrv, dMoveACamPos, uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)enACamDrv]))	return;

	if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_x, dMoveXPos))
	{
		return;
	}
	if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_y, dMoveYPos))
	{
		return;
	}
	if (FALSE == uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, dMoveXPos, uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_x]))	return;
	if (FALSE == uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dMoveYPos, uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_y]))	return;

#ifdef CAM_SPEC_SIMUL
	return;
#endif // CAM_SPEC_SIMUL

	/* 타이머 초기화 및 시작 */
	cTime.Init();
	cTime.Start();

	/* 일정 시간동안 동작 */
	while (60000 > (int)cTime.GetTactMs())
	{
		/* 타겟 위치와 현재위치의 오차를 구한다. */
		dDiffXPos = dMoveXPos - uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);
		dDiffYPos = dMoveYPos - uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);
		dDiffACamPos = dMoveACamPos - uvCmn_MC2_GetDrvAbsPos(enACamDrv);

		/* Motor가 타겟위치에 도달하고 정지 상태일 경우 종료 */
		if (FALSE == uvCmn_MC2_IsDriveBusy(ENG_MMDI::en_stage_x) &&
			FALSE == uvCmn_MC2_IsDriveBusy(ENG_MMDI::en_stage_y) &&
			FALSE == uvCmn_MC2_IsDriveBusy(enACamDrv) &&
			fabs(dDiffXPos) < 0.0005 &&
			fabs(dDiffYPos) < 0.0005 &&
			fabs(dDiffACamPos) < 0.0005)
		{
			return;
		}

		cTime.Stop();
		Wait_(1);
	}

	/* 시간 초과 */
	return;
}


/*
 desc : 현재 위치에서 QuartZ 인식 후 카메라의 중심에서 얼마나 떨어졌는지 확인
 parm : None
 retn : None
*/
VOID CDlgCalbCameraSpec::MarkGrabbedResult()
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_RESULT];
	CTactTimeCheck cTime;
	LPG_ACGR pstResult = NULL;

	/* Live 종료 */
	if (TRUE == m_bCamLiveState)
	{
		m_bCamLiveState = FALSE;
		SetLiveView(FALSE);
	}

	//UINT8 u8ACamID = (0 == GetCheckACam()) ? 0x01 : 0x02;
	UINT8 u8ACamID = GetCheckACam() + 1;

	/* 현재 등록된 모델이 존재하는지 여부 */
// 	if (FALSE == uvEng_Camera_IsSetMarkModel(0x01, u8ACamID, GLOBAL_MARK)) // lk91!! calib 사용하는 mark 체크
// 	{
// 		AfxMessageBox(L"There are no registered models", MB_ICONSTOP | MB_TOPMOST);
// 		return;
// 	}

	ResetResult();

	if (ENG_VCCM::en_image_mode != uvEng_Camera_GetCamMode())
	{	
		/* Mark Model이 등록되어 있는지 여부 */
		if (!uvEng_Camera_IsSetMarkModelACam(u8ACamID, 2))
		{
			AfxMessageBox(L"No Mark Model registered", MB_ICONSTOP | MB_TOPMOST);
			return;
		}

		/* 카메라 Grabbed Mode를 Calibration Mode로 동작 */
		uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);

		/* 기존 검색된 Grabbed Data & Image 제거 */
		// uvEng_Camera_ResetGrabbedImage();

		/* Camera 쪽에 Trigger Event 강제로 1개 발생 */
		if (!uvEng_Mvenc_ReqTrigOutOne(u8ACamID))
		{
			uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
			AfxMessageBox(L"Failed to send the event for trigger", MB_ICONSTOP | MB_TOPMOST);
			return;
		}
	}

	/* 타이머 초기화 및 시작 */
	cTime.Init();
	cTime.Start();

	/* 일정 시간동안 동작 */
	while (1000 > (int)cTime.GetTactMs())
	{
		/* 검색된 결과가 있는지 조사 */
		// imsi mhbaek 231018 change enum DISP_TYPE_CALB_CAMSPEC -> DISP_TYPE_MARK_LIVE
		pstResult = uvEng_Camera_RunModelCali(u8ACamID, 0xff, (UINT8)DISP_TYPE_CALB_CAMSPEC, TMP_MARK, TRUE, uvEng_GetConfig()->mark_find.image_process);

		if (nullptr != pstResult)
		{
			if (NULL != pstResult->marked)
			{
				break;
			}
		}

		cTime.Stop();
		Wait_(100);
	}

	/* 검색된 결과가 있는지 여부 */
	if (NULL == pstResult->marked)
	{
		AfxMessageBox(L"There is no analysis result", MB_ICONSTOP | MB_TOPMOST);
		return;
	}
	
	pGrid->SetItemDouble(eRESULT_ROW_SCORE, eRESULT_COL_VALUE, pstResult->score_rate, 3);
	pGrid->SetItemDouble(eRESULT_ROW_SCALE, eRESULT_COL_VALUE, pstResult->scale_rate, 3);
	pGrid->SetItemDouble(eRESULT_ROW_MARK_OFFSET_X, eRESULT_COL_VALUE, pstResult->move_mm_x, 3);
	pGrid->SetItemDouble(eRESULT_ROW_MARK_OFFSET_Y, eRESULT_COL_VALUE, pstResult->move_mm_y, 3);

	/* 화면 갱신 */
	InvalidateView();
	pGrid->Refresh();
}


VOID CDlgCalbCameraSpec::MeasureStart()
{
	if (IDNO == AfxMessageBox(L"측정을 시작하시겠습니까?", MB_YESNO))
	{
		return;
	}

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_REPEAT_COUNT];
	UINT32 u32RepeatCount = (UINT32)pGrid->GetItemTextToInt(eSETTING_ROW_ITEM, eSETTING_COL_VALUE);

	m_stVctResult.clear();

	CCamSpecMgr::GetInstance()->SetRepeatCount(u32RepeatCount);
	CCamSpecMgr::GetInstance()->MeasureStart(this->GetSafeHwnd());
}


VOID CDlgCalbCameraSpec::MeasureStop()
{
// 	m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_SETTING].EnableWindow(TRUE);
// 	m_cmb_ctl[eCALB_UVPOWER_MEASURE_CMB_WAVELENGTH].EnableWindow(TRUE);
// 	m_btn_ctl[eCALB_UVPOWER_MEASURE_BTN_START].SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
// 	Invalidate(FALSE);

	CCamSpecMgr::GetInstance()->SetStopProcess();
}


VOID CDlgCalbCameraSpec::UpdateCaliResult()
{
	UINT8 u8ChNo = GetTrigChNo();
	UINT8 u8ACamNo = GetCheckACam();
	CGridCtrl* pGrd = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_RESULT];
	LPG_CASI pOption = &uvEng_GetConfig()->acam_spec;
	LPG_ACGR pstCali = NULL;
	STM_RAPV stVal;

	/* 회전 각도 및 픽셀 크기 (um) 구하기 */
	pGrd->SetItemDouble(eRESULT_ROW_ANGLE, eRESULT_COL_VALUE, pOption->spec_angle[u8ACamNo], 4);
	pGrd->SetItemDouble(eRESULT_ROW_PIXEL, eRESULT_COL_VALUE, pOption->spec_pixel_um[u8ACamNo], 4);

	/* 매칭 결과 화면 갱신 */
	pstCali = uvEng_Camera_GetLastGrabbedMark();
	if (NULL != pstCali && 0x00 != pstCali->marked)
	{
		pGrd->SetItemDouble(eRESULT_ROW_SCORE, eRESULT_COL_VALUE, pstCali->score_rate, 2);
		pGrd->SetItemDouble(eRESULT_ROW_SCALE, eRESULT_COL_VALUE, pstCali->scale_rate, 2);
	}

	/* 현재 수신된 데이터 등록 */
	stVal.dValue[eRESULT_ROW_ANGLE] = uvEng_GetConfig()->acam_spec.spec_angle[u8ACamNo];
	stVal.dValue[eRESULT_ROW_PIXEL] = uvEng_GetConfig()->acam_spec.spec_pixel_um[u8ACamNo];
	stVal.dValue[eRESULT_ROW_SCORE] = pstCali->score_rate;
	stVal.dValue[eRESULT_ROW_SCALE] = pstCali->scale_rate;

	m_stVctResult.push_back(stVal);
	pGrd->Refresh();
}


/*
 desc : 정규 분포 계산 (카메라의 각도 및 픽셀 크기)
 parm : rate	- [in]  정규 분포 계수 값
 retn : None
*/
VOID CDlgCalbCameraSpec::CalcNormalDist(double dRate/* = 2.0f*/)
{
	CGridCtrl* pGrd = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_RESULT];
	LPG_CASI pOption = &uvEng_GetConfig()->acam_spec;
	int nCnt[eRESULT_PARAM_MAX];
	double dSum[eRESULT_PARAM_MAX];
	double dAvg[eRESULT_PARAM_MAX];
	double dStd[eRESULT_PARAM_MAX];

	/* 값이 동일하게 등록되어 있지 않는지 확인 */
	if (1 > (int)m_stVctResult.size())
	{
		return;
	}

	/* 총합 구하기 */
	dSum[eRESULT_ROW_ANGLE] = dSum[eRESULT_ROW_PIXEL] = 0.0f;

	for (int i = 0; i < (int)m_stVctResult.size(); i++)
	{
		for (int j = 0; j < eRESULT_PARAM_MAX; j++)
		{
			dSum[j] += m_stVctResult[i].dValue[j];
		}
	}

	/* 평균 구하기 */
	for (int j = 0; j < eRESULT_PARAM_MAX; j++)
	{
		dAvg[j] = dSum[j] / (double)m_stVctResult.size();
		dSum[j] = 0.;	// 다음 사용을 위해 초기화
	}

	/* 제곱근 구하기 */
	for (int i = 0; i < (int)m_stVctResult.size(); i++)
	{
		for (int j = 0; j < eRESULT_PARAM_MAX; j++)
		{
			dSum[j] += pow(m_stVctResult[i].dValue[j] - dAvg[j], 2.0f);
		}
	}

	/* 표준 편차 구하기 */
	for (int j = 0; j < eRESULT_PARAM_MAX; j++)
	{
		if (0.0f < dSum[j])
		{
			dStd[j] = sqrt(dSum[j] / (double)m_stVctResult.size());
		}

		dSum[j] = 0.0f;		// 다음 사용을 위해 초기화
		nCnt[j] = 0;		// 다음 사용을 위해 초기화
	}

	/* 표준 편차 구간에 벗어난 값은 제외 (이상치 제외)하고 총 합 구하기 (정규 분포로 값 구하기) */
	/* 총합 구하기 */
	for (int i = 0; i < (int)m_stVctResult.size(); i++)
	{
		/* (현재 값) 이 (평균 값 기준 +/- 편차 값) 값 범위 내에 있는지 확인 */
		for (int j = 0; j < eRESULT_PARAM_MAX; j++)
		{
			if (((dAvg[j] - dStd[j] * dRate) <= m_stVctResult[i].dValue[j]) &&
				((dAvg[j] + dStd[j] * dRate) >= m_stVctResult[i].dValue[j]))
			{
				dSum[j] += m_stVctResult[i].dValue[j];
				nCnt[j]++;
			}
		}
	}

	/* 평균 구하기 */
	for (int j = 0; j < eRESULT_PARAM_MAX; j++)
	{
		dAvg[j] = dSum[j] / (double)nCnt[j];
	}

	pGrd->SetItemDouble(eRESULT_ROW_ANGLE, eRESULT_COL_VALUE, dAvg[eRESULT_ROW_ANGLE], 4);
	pGrd->SetItemDouble(eRESULT_ROW_PIXEL, eRESULT_COL_VALUE, dAvg[eRESULT_ROW_PIXEL], 4);
	pGrd->SetItemDouble(eRESULT_ROW_SCORE, eRESULT_COL_VALUE, dAvg[eRESULT_ROW_SCORE], 2);
	pGrd->SetItemDouble(eRESULT_ROW_SCALE, eRESULT_COL_VALUE, dAvg[eRESULT_ROW_SCALE], 2);
	pGrd->Refresh();
}


BOOL CDlgCalbCameraSpec::PopupKBDN(ENM_DITM enType, CString strInput, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts/* = 0*/)
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


LRESULT CDlgCalbCameraSpec::UpdateMeasureProgress(WPARAM wParam, LPARAM lParam)
{
	CString strText = *(CString*)lParam;

	m_pgr_ctl[eCALB_CAMERA_SPEC_PGR_RATE].SetPos((int)wParam);
	m_txt_ctl[eCALB_CAMERA_SPEC_TXT_STATE].SetTextToStr((PTCHAR)(LPCTSTR)strText);
	Invalidate();
	return 0;
}


LRESULT CDlgCalbCameraSpec::UpdateCaliResult(WPARAM wParam, LPARAM lParam)
{
	UpdateCaliResult();
	InvalidateView();
	return 0;
}


LRESULT CDlgCalbCameraSpec::MeasureComplete(WPARAM wParam, LPARAM lParam)
{
	UpdateCaliResult();
	CalcNormalDist();
	InvalidateView();

	SaveResult();
	return 0;
}


LRESULT CDlgCalbCameraSpec::RefreshGrabView(WPARAM wParam, LPARAM lParam)
{
	InvalidateView();
	return 0;
}


/*
 desc : 일반 버튼 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgCalbCameraSpec::OnBtnClick(UINT32 id)
{
	int nBtnID = id - IDC_CALB_CAMERA_SPEC_BTN_TEST_MATCH;

	if (TRUE == CCamSpecMgr::GetInstance()->IsProcessWorking() &&
		nBtnID != eCALB_CAMERA_SPEC_BTN_STOP)
	{
		AfxMessageBox(_T("현재 측정중입니다."));
		return;
	}

	switch (nBtnID)
	{
	case eCALB_CAMERA_SPEC_BTN_TEST_MATCH:
		MarkGrabbedResult();
		break;
	case eCALB_CAMERA_SPEC_BTN_START:
		MeasureStart();
		break;
	case eCALB_CAMERA_SPEC_BTN_STOP:
		MeasureStop();
		break;
	case eCALB_CAMERA_SPEC_BTN_SAVE_RESULT:
		SaveResult();
		break;
	case eCALB_CAMERA_SPEC_BTN_VIEW_OPTION:
		/* 추후 계정 등급 적용 필요 */
		m_bViewCtrls = !m_bViewCtrls;
		ViewControls(m_bViewCtrls);
		break;
	case eCALB_CAMERA_SPEC_BTN_OPEN_CTRL_PANEL:
		::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), eMSG_MAIN_OPEN_CONSOLE, NULL, NULL);
		break;
	case eCALB_CAMERA_SPEC_BTN_SAVE:
		SaveDataConfig();
		break;
	case eCALB_CAMERA_SPEC_BTN_LIVE:
		m_bCamLiveState = !m_bCamLiveState;
		SetLiveView(m_bCamLiveState);
		break;
	default:
		break;
	}
}


void CDlgCalbCameraSpec::OnClickGridSelectCam(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_SELECT_CAM];

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	if (pItem->iColumn == m_u8SelHead)
	{
		return;
	}

	if (TRUE == m_bEditMode)
	{
		if (IDNO == AfxMessageBox(L"지금 수정중인 내용이 있습니다.\n카메라 선택을 변경하시겠습니까?", MB_YESNO))
		{
			return;
		}

		m_bEditMode = FALSE;
	}

	/* Live 종료 */
	if (TRUE == m_bCamLiveState)
	{
		m_bCamLiveState = FALSE;
		SetLiveView(FALSE);
	}

	m_u8SelHead = pItem->iColumn;

	for (int i = 0; i < pGrid->GetColumnCount(); i++)
	{
		pGrid->SetItemBkColour(0, i, DEF_COLOR_BTN_PAGE_NORMAL);
	}

	pGrid->SetItemBkColour(0, pItem->iColumn, DEF_COLOR_BTN_MENU_SELECT);

	CCamSpecMgr::GetInstance()->SetCamID(m_u8SelHead + 1);
	LoadDataConfig();

	pGrid->Refresh();
}


void CDlgCalbCameraSpec::OnClickGridRepeatCount(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_REPEAT_COUNT];

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	if (eSETTING_COL_VALUE != pItem->iColumn)
	{
		return;
	}
	else if (TRUE == CCamSpecMgr::GetInstance()->IsProcessWorking())
	{
		AfxMessageBox(_T("현재 측정중입니다."));
		return;
	}

	CString strOutput;
	CString strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn);

	if (PopupKBDN(ENM_DITM::en_int16, strInput, strOutput, 0, 100))
	{
		pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);
	}

	pGrid->Refresh();
}


void CDlgCalbCameraSpec::OnClickGridResult(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_RESULT];

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);
}


void CDlgCalbCameraSpec::OnClickGridSelectMode(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_SELECT_MODE];
	int nMode = (int)CCamSpecMgr::GetInstance()->GetRunMode();

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	if (pItem->iColumn == nMode)
	{
		return;
	}

	for (int i = 0; i < pGrid->GetColumnCount(); i++)
	{
		pGrid->SetItemBkColour(0, i, DEF_COLOR_BTN_PAGE_NORMAL);
	}

	pGrid->SetItemBkColour(0, pItem->iColumn, DEF_COLOR_BTN_MENU_SELECT);

	CCamSpecMgr::GetInstance()->SetRunMode((BOOL)pItem->iColumn);
	pGrid->Refresh();
}


void CDlgCalbCameraSpec::OnClickGridTriggerOption(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_TRIGGER_OPTION];
	UINT8 u8DecPts = 0;

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	if (eOPTION_COL_VALUE != pItem->iColumn)
	{
		return;
	}
	else if (TRUE == CCamSpecMgr::GetInstance()->IsProcessWorking())
	{
		AfxMessageBox(_T("현재 측정중입니다."));
		return;
	}

	CString strOutput;
	CString strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn);

	if (PopupKBDN(ENM_DITM::en_int16, strInput, strOutput, 0, 9999))
	{
		pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);
	}

	pGrid->Refresh();
}


void CDlgCalbCameraSpec::OnClickGridOption(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_OPTION];
	ENM_DITM enType = ENM_DITM::en_double;
	double dMin = -9999.9999;
	double dMax = 9999.9999;
	UINT8 u8DecPts = 4;

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	if (eOPTION_COL_VALUE != pItem->iColumn)
	{
		return;
	}
	else if (TRUE == CCamSpecMgr::GetInstance()->IsProcessWorking())
	{
		AfxMessageBox(_T("현재 측정중입니다."));
		return;
	}

	switch (pItem->iRow)
	{
	case eOPTION_ROW_GRAB_WIDTH:
	case eOPTION_ROW_GRAB_HEIGHT:
		enType = ENM_DITM::en_int16;
		dMin = 0;
		dMax = 2000;
		u8DecPts = 0;
		break;
	default:
		break;
	}

	CString strOutput;
	CString strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn);

	if (PopupKBDN(enType, strInput, strOutput, dMin, dMax, u8DecPts))
	{
		pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);
	}

	pGrid->Refresh();
}


void CDlgCalbCameraSpec::OnClickGridCameraOption(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_CAMERA_OPTION];

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	if (TRUE == CCamSpecMgr::GetInstance()->IsProcessWorking())
	{
		AfxMessageBox(_T("현재 측정중입니다."));
		return;
	}

	CString strOutput;
	CString strInput = pGrid->GetItemText(pItem->iRow, eCAM_OPTION_COL_VALUE);;

	switch (pItem->iColumn)
	{
	case eCAM_OPTION_COL_NAME:
		return;
	case eCAM_OPTION_COL_VALUE:
		if (PopupKBDN(ENM_DITM::en_int16, strInput, strOutput, 0, 1000, 0))
		{
			pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);
		}

		pGrid->Refresh();
		return;
	case eCAM_OPTION_COL_SET:

		if (pItem->iRow < eCAM_OPTION_ROW_GAIN1 || pItem->iRow >= eCAM_OPTION_ROW_MAX)
		{
			return;
		}

		uvEng_GetConfig()->set_basler.cam_gain_level[pItem->iRow] = _ttoi(strInput);
		uvEng_Camera_SetGainLevel(pItem->iRow + 1, _ttoi(strInput));
		uvEng_SaveConfig();

	default:
		break;
	}
}


void CDlgCalbCameraSpec::OnClickGridPosition(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_CAMERA_SPEC_GRD_POSITION];

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	if (IDNO == AfxMessageBox(L"이동하시겠습니까?", MB_YESNO))
	{
		return;
	}

	MovePosMeasure(pItem->iColumn + 1);
}


/*
 desc : Main Thread에서 발생된 이벤트 처리
 parm : wparam	- [in]  Event ID
		lparam	- [in]  Event Data
 retn : 0L
*/
//LRESULT CDlgCalbCameraSpec::OnMsgThread(WPARAM wparam, LPARAM lparam)
//{
//	switch (wparam)
//	{
//	case MSG_ID_NORMAL: UpdatePeriod();			break;
//	//case MSG_ID_CALI_INIT: UpdateCaliInit();			break;
//	//case MSG_ID_CALI_RESULT: UpdateCaliResult(FALSE);	break;
//	//case MSG_ID_CALI_COMPLETE: UpdateCaliResult(TRUE);	break;
//	case MSG_ID_GRAB_VIEW: InvalidateView();			break;
//	}
//
//	return 0L;
//}
/*
VOID CDlgCalbCameraSpec::UpdatePeriod(UINT64 tick, BOOL is_busy)
{

	UpdateControl(tick, is_busy);
}*/
/*
 desc : 주기적으로 갱신되는 항목
 parm : None
 retn : None
*/
VOID CDlgCalbCameraSpec::UpdatePeriod(UINT64 tick, BOOL is_busy)
{
	/* for Align Camera */
	UpdateCtrlACam();
	/* for Motor Position (mm) */
	//UpdateMotorPos();
	/* for PLC Value */
	//UpdatePLCVal();
	/* Control : Enable/Disable */
	//UpdateEnableCtrl();
	/* Grabbed Image View 출력 */
	UpdateLiveView();
	//UpdateStepRate();

	UpdateControl(tick, is_busy);
}

/*
 desc : Align Camera 관련 Control 갱신
 parm : None
 retn : None
*/
VOID CDlgCalbCameraSpec::UpdateCtrlACam()
{
	BOOL bEnable = TRUE;

	/* 얼라인 카메라가 연결되어 있지 않으면 */
	if (bEnable)	bEnable = uvCmn_Camera_IsConnected();
#if 0
	/* 현재 Live View 화면이면, 현재 측정 모드인지 여부에 따라 */
	if (bEnable)	bEnable = uvEng_Camera_IsCamModeLive();
#endif
	///* 현재 측정 작업 중인지 여부 */
	//if (bEnable)	bEnable = !m_pMainThread->IsRunSpec();
	///* Live Check 버튼 활성화 및 체크 해제 처리 */
	//m_chk_cam[4].EnableWindow(bEnable);
	//if (!bEnable)
	//{
	//	if (m_chk_cam[4].GetCheck())	m_chk_cam[4].SetCheck(0);
	//}
	/* 현재 측정 작업 중인지 여부 */
	//if (bEnable)	bEnable = !m_pMainThread->IsRunSpec();
	/* Live Check 버튼 활성화 및 체크 해제 처리 */
	m_btn_ctl[eCALB_CAMERA_SPEC_BTN_LIVE].EnableWindow(bEnable);
	if (!bEnable)
	{
		m_btn_ctl[eCALB_CAMERA_SPEC_BTN_LIVE].SetBgColor(g_clrBtnColor);
		m_btn_ctl[eCALB_CAMERA_SPEC_BTN_LIVE].SetTextColor(WHITE_);
	}
}

/*
 desc : Updates the live image
 parm : None
 retn : None
*/
VOID CDlgCalbCameraSpec::UpdateLiveView()
{
	//HDC hDC = NULL;
	//RECT rDraw;
	//UINT8 u8ACamID = (0 == GetCheckACam()) ? 0x01 : 0x02;

	//if (!uvEng_Camera_IsCamModeLive())	return;
	///* 이미지가 출력될 윈도 영역 */
	//m_pic_ctl[0].GetClientRect(&rDraw);
	//hDC = ::GetDC(m_pic_ctl[0].m_hWnd);
	//if (hDC)
	//{
	//	uvEng_Camera_DrawLiveBitmap(hDC, rDraw, u8ACamID, FALSE);
	//	::ReleaseDC(m_pic_ctl[0].m_hWnd, hDC);
	//}

	if (!uvEng_Camera_IsCamModeLive())	return;
	//UINT8 u8ACamID = (0 == GetCheckACam()) ? 0x01 : 0x02;
	UINT8 u8ACamID = GetCheckACam() + 1;
	uvEng_Camera_DrawImageBitmap(DISP_TYPE_CALB_CAMSPEC, u8ACamID-1, u8ACamID);
}


VOID CDlgCalbCameraSpec::DoEvents()
{
	MSG	lMessage;

	while (PeekMessage(&lMessage, NULL, 0, 0, PM_REMOVE) != FALSE)
	{
		TranslateMessage(&lMessage);
		DispatchMessage(&lMessage);
	}
}


VOID CDlgCalbCameraSpec::Wait_(DWORD dwTime)
{
	DWORD dwStart = GetTickCount();

	while (GetTickCount() - dwTime < dwStart)
	{
		DoEvents();
		Sleep(1);
	}
}