// DlgCalbFlatness.cpp: 구현 파일
//
// DlgCalb.cpp: 구현 파일
//

#include "pch.h"
#include "DlgCalbAutofocus.h"
#include "afxdialogex.h"
#include "../../GlobalVariables.h"

#define DEF_DEFAULT_GRID_ROW_SIZE 35

/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgCalbAutofocus::CDlgCalbAutofocus(UINT32 id, CWnd* parent)
	: CDlgSubMenu(id, parent)
{
	m_enDlgID = ENG_CMDI_SUB::en_menu_sub_conf_autofocus;

}

CDlgCalbAutofocus::~CDlgCalbAutofocus()
{
	 
}

VOID CDlgCalbAutofocus::UpdateControl(UINT64 tick, BOOL is_busy)
{

}

VOID CDlgCalbAutofocus::OnResizeDlg()
{

}

VOID CDlgCalbAutofocus::UpdatePeriod(UINT64 tick, BOOL is_busy)
{

}


VOID CDlgCalbAutofocus::InitGrid()
{
	InitGridSetting();

	InitGridZPos();// 이 둘은 순서 지켜야함. 
	InitGridPlot();//이 둘은 순서 지켜야함.

	InitGridStoredValue();
}


VOID CDlgCalbAutofocus::InitGridZPos()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <COLORREF>		vColColor = { ALICE_BLUE, WHITE_, ALICE_BLUE };
	std::vector <int>			vColSize;
	std::vector <int>			vRowSize;

	vector< std::vector <std::wstring>> vTitle;
	
	vTitle.push_back({ _T("Focus Inited"), _T("NO"), _T("YES,NO") });
	vTitle.push_back({ _T("Run AF"), _T("ON"), _T("ON,OFF")});
	vTitle.push_back({ _T("SensorType"), _T("IN"), _T("IN,EX") });
	vTitle.push_back({ _T("Sensor Active"), _T("ON"), _T("ON,OFF") });



	vRowSize.resize(vTitle.size());
	vColSize.resize(vTitle.begin()->size());

	CGridCtrl* pGrid = &grids[zpos];
	
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
	pGrid->SetEditable();
	pGrid->Refresh();
}

void CDlgCalbAutofocus::InitGridPlot()
{
	CRect rStt, rStt2, rStt3;
	

	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <COLORREF>		vColColor = { ALICE_BLUE, WHITE_, ALICE_BLUE };
	std::vector <int>			vColSize;
	std::vector <int>			vRowSize;

	vector< std::vector <std::wstring>> vTitle;

	for (int i = 0; i < uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		std::wstringstream ss;
		ss << "PH" << i + 1 << "PLOT";
		vTitle.push_back({ ss.str(), _T("0"), _T("trig") });
	}

	vRowSize.resize(vTitle.size());
	vColSize.resize(vTitle.begin()->size());

	CGridCtrl* pGrid = &grids[plot];

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
	pGrid->SetEditable();
	pGrid->Refresh();

	//안씀//
	//grids[zpos].GetWindowRect(rStt);
	//GetParent()->ScreenToClient(&rStt);

	//grids[plot].GetWindowRect(rStt2);
	//GetParent()->ScreenToClient(&rStt2);

	//labels[plot].GetWindowRect(rStt3);
	//GetParent()->ScreenToClient(&rStt3);

	//labels[plot].MoveWindow(rStt.left, rStt.bottom + 40, rStt3.Width(), rStt3.Height());
	//grids[plot].MoveWindow(rStt.left, rStt.bottom + 85, rStt2.Width(), rStt2.Height());

}


void CDlgCalbAutofocus::InitGridStoredValue()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <COLORREF>		vColColor = { ALICE_BLUE, WHITE_, ALICE_BLUE };
	std::vector <int>			vColSize;
	std::vector <int>			vRowSize;

	vector< std::vector <std::wstring>> vTitle;
	
	for (int i = 0; i < uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		std::wstringstream ss;
		ss << "PH" << i + 1;
		vTitle.push_back({ ss.str(), _T("0"), _T("trig") });
	}

	vRowSize.resize(vTitle.size());
	vColSize.resize(vTitle.begin()->size());

	CGridCtrl* pGrid = &grids[storedvalue];

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
		width = rGrid.Width() / (int)vColSize.size() ;
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
	pGrid->SetEditable();
	pGrid->Refresh();
}

void CDlgCalbAutofocus::InitGridSetting()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <COLORREF>		vColColor = { ALICE_BLUE, WHITE_, ALICE_BLUE };
	std::vector <int>			vColSize;
	std::vector <int>			vRowSize;
	
	vector< std::vector <std::wstring>> vTitle;
	vTitle.push_back({ _T("Use AF"),_T("True"),_T("bool") });
	vTitle.push_back({ _T("Sensor Type"),_T("EXTERNAL"),_T("type") });
	
	//uvEng_Luria_GetShMem()->focus.
	
	for (int i = 0; i < uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		std::wstringstream ss;
		ss << "StoredValue_PH" << i+1;
		vTitle.push_back({ ss.str(), _T("0"), _T("trig")});
	}
	
	vRowSize.resize(vTitle.size());
	vColSize.resize(vTitle.begin()->size());
	
	CGridCtrl* pGrid = &grids[setting];

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
		width = rGrid.Width() / (int)vColSize.size() ;
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
			
		}
	}

	/* Grid Size 재구성 */
	rGrid.bottom = rGrid.top + nTotalHeight + 1;
	pGrid->MoveWindow(rGrid);


	//셀속성
	CStringArray options1, options2;

	pGrid->SetCellType(0, 1, RUNTIME_CLASS(CGridCellCombo)); // af 사용여부 
	options1.Add(_T("YES"));
	options1.Add(_T("NO"));

	CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(0, 1);
	pComboCell->SetOptions(options1);
	pComboCell->SetStyle(CBS_DROPDOWN);
	
	pGrid->SetCellType(1, 1, RUNTIME_CLASS(CGridCellCombo)); // 센서타입
	options2.Add(_T("EXTERNAL"));
	options2.Add(_T("INTERNAL"));

	CGridCellCombo* pComboCell2 = (CGridCellCombo*)pGrid->GetCell(1, 1);	
	pComboCell2->SetOptions(options2);
	pComboCell2->SetStyle(CBS_DROPDOWN);
	


	/* 기본 속성 및 갱신 */
	pGrid->EnableDragAndDrop(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->SetEditable();
	pGrid->Refresh();




}




/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgCalbAutofocus::DoDataExchange(CDataExchange* dx)
{
	
	for (int i = 0; i < btnmax; i++)		
		DDX_Control(dx, IDC_AF_SETTING_LOAD + i, btns[i]);

	for (int i = 0; i < grdmax; i++)		
		DDX_Control(dx, IDC_CALB_AF_GRD_SETTING + i, grids[i]);

	for (int i = 0; i < labelmax; i++)		
		DDX_Control(dx, IDC_CALB_AF_SETTING + i, labels[i]);

	DDX_Control(dx, IDC_AF_CONTROLGROUP, group);

	CDlgSubMenu::DoDataExchange(dx);
}


void CDlgCalbAutofocus::OnClickGrid(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	CGridCtrl* pGrid = nullptr;	
	UINT clickedGridID = pNotifyStruct->idFrom;

	map<int, int> gridMap = { {IDC_CALB_AF_GRD_SETTING,setting},
		{IDC_CALB_AF_GRD_STATUS,zpos},
		{IDC_CALB_AF_GRD_STOREDVALUE,storedvalue},
		{IDC_CALB_AF_GRD_PLOTVALUE,plot},};

	pGrid = &grids[gridMap[clickedGridID]];
	pGrid->SetFocusCell(-1, -1);

	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	if (pItem == nullptr) return;

	int selRow = pItem->iRow;
	int selCol = pItem->iColumn;

	if (selCol != 1)
		return;

	ENM_DITM enType = ENM_DITM::en_int8;
	double	minV = 0;
	double	maxV = 9999.9999;
	UINT8 u8DecPts = 3;
	CString strOutput;
	bool open = false;
	switch (clickedGridID)
	{
		case IDC_CALB_AF_GRD_SETTING:
		{
			if (selRow < 2)
				return;

			minV = 21284; maxV = 44252; //<-이건 비지텍에서 정의된 값임 변경불가. 1유닛 = 0.0174um (대략)
			u8DecPts = 5;
			open = true;
		}
		break;

		case IDC_CALB_AF_GRD_STATUS:
		{
			int debug = 0;  //이거는 읽기전용
		}
		break;

		case IDC_CALB_AF_GRD_STOREDVALUE:
		{
			int debug = 0; //이것도 읽기전용
		}
		break;

		case IDC_CALB_AF_GRD_PLOTVALUE:
		{
			int debug = 0;//이것도 읽기전용
		}
		break;
	}

	if (open && PopupKBDN(enType, strOutput,minV, maxV, u8DecPts))
	{
		pGrid->SetItemTextFmt(selRow, selCol, _T("%s"), strOutput);
	}

	pGrid->Refresh();
	*pResult = 0;
}

BOOL CDlgCalbAutofocus::PopupKBDN(ENM_DITM enType, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts/* = 0*/)
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


BEGIN_MESSAGE_MAP(CDlgCalbAutofocus, CDlgSubMenu)
	ON_NOTIFY(NM_CLICK, IDC_CALB_AF_GRD_SETTING, &CDlgCalbAutofocus::OnClickGrid)
	ON_NOTIFY(NM_CLICK, IDC_CALB_AF_GRD_STATUS, &CDlgCalbAutofocus::OnClickGrid)
	ON_NOTIFY(NM_CLICK, IDC_CALB_AF_GRD_STOREDVALUE, &CDlgCalbAutofocus::OnClickGrid)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_AF_SETTING_LOAD, IDC_AF_SETTING_LOAD + btnmax, OnBtnClick)
	ON_WM_TIMER()
END_MESSAGE_MAP()


BOOL CDlgCalbAutofocus::PreTranslateMessage(MSG* msg)
{
	if (msg->message == WM_KEYDOWN)
	{
		if (msg->wParam == VK_RETURN || msg->wParam == VK_ESCAPE)
		{
			return TRUE;                // Do not process further
		}
	}

	return CDlgSubMenu::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgCalbAutofocus::OnInitDlg()
{
	InitCtrl();
	
	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgCalbAutofocus::OnExitDlg()
{
	
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgCalbAutofocus::OnPaintDlg(CDC* dc)
{

}
   
/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgCalbAutofocus::InitCtrl()
{
	CResizeUI clsResizeUI;
	CString strTemp;
	CRect rStt;

	//groupbox
	clsResizeUI.ResizeControl(this, &group);
	group.GetWindowRect(rStt);
	this->ScreenToClient(rStt);
	group.MoveWindow(rStt);

	//lebel
	for (int i = 0; i < labelmax; i++)
	{
		labels[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		labels[i].SetDrawBg(1);
		labels[i].SetBaseProp(0, 1, 0, 0, DEF_COLOR_BTN_MENU_NORMAL, 0, WHITE_);

		clsResizeUI.ResizeControl(this, &labels[i]);

		labels[i].GetWindowRect(rStt);
		this->ScreenToClient(rStt);
		rStt.right += 2;

		labels[i].MoveWindow(rStt);
	}

	InitGrid();

	for (int i = 0; i < grdmax; i++)
	{
		grids[i].SetParent(this);
	}

	for (int i = 0; i < btnmax; i++)
	{
		btns[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		btns[i].SetBgColor(g_clrBtnColor);
		btns[i].SetTextColor(g_clrBtnTextColor);
		clsResizeUI.ResizeControl(this, &btns[i]);

	}
	MoveBtns();
}

void CDlgCalbAutofocus::MoveBtns()
{
	CRect rStt;
	CRect btnRect;
	int pair = 2;
	for (int i = 0; i < grdmax; i++)
	{
		grids[i].GetWindowRect(rStt);
		GetParent()->ScreenToClient(&rStt);

		btns[(i*pair)].GetWindowRect(btnRect);
		GetParent()->ScreenToClient(&btnRect);

		int btnWidth = btnRect.Width();
		int btnHeight = btnRect.Height();

		btns[(i * pair)].MoveWindow(rStt.left, rStt.bottom + 10, btnWidth, btnHeight);

		int btnLeft = rStt.right - btnWidth;
		int btnTop = rStt.bottom;

		btns[(i * pair) + 1].MoveWindow(btnLeft, btnTop + 10, btnWidth, btnHeight);
	}
}


VOID CDlgCalbAutofocus::LoadDataConfig()
{
	
}

VOID CDlgCalbAutofocus::SaveDataConfig()
{
	 
	uvEng_SaveConfig();
}

 
  

/*
 desc : 일반 버튼 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgCalbAutofocus::OnBtnClick(UINT32 id)
{
	auto gv = GlobalVariables::GetInstance();
	int phCnt = uvEng_GetConfig()->luria_svc.ph_count;

	std::wstringstream ss;

	switch (id)
	{	
		case IDC_BTN_AF_FOCUSINIT:
		{
			gv->GetAutofocus().InitFocusDrive();
		}
		break;

		case IDC_AF_SETTING_LOAD:
		case IDC_AF_SETTING_SAVE:
		{

		}
		break;

		
		case IDC_AF_STORED_READ:
		{
			int ph[MAX_PH] = { 0, };
			bool res[MAX_PH] = { false, };

			for (int i = 0; i < phCnt; i++)
			{
				res[i] = gv->GetAutofocus().GetStoredAFPosition(i + 1, ph[i]);
				if (res[i] == false)
				{
					ss << "PH" << i + 1 << "stored value read Failed.";
					MessageBox(nullptr, ss.str().c_str(), L"failed", MB_OK);
				}
			}

		}
		break;

		case IDC_AF_STORED_WRITE:
		case IDC_AF_PLOT_READ:
		case IDC_AF_PLOT_READ2:
		
		case IDC_BTN_AF_SET_INTERNAL:
		case IDC_BTN_AF_SET_EXTERNAL:
		case IDC_BTN_AF_SENSOR_ON:
		case IDC_BTN_AF_SENSOR_OFF:
		case IDC_BTN_AF_AF_ON:
		case IDC_BTN_AF_AF_OFF:
		{
			int debug = 0;
		}
		break;

		case IDC_BTN_AF_CONTROLPANEL    :
		{
			if (AfxGetMainWnd()->GetSafeHwnd()) //모션 컨트롤패널.
			{
				::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), eMSG_MAIN_OPEN_CONSOLE, NULL, NULL);
			}
		}
		break;

	default:
		break;
	}
}
             
