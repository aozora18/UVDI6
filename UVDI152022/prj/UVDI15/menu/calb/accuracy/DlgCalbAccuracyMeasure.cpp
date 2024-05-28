// DlgCalb.cpp: 구현 파일
//

#include "pch.h"
#include "../../../pops/DlgParam.h"
#include "DlgCalbAccuracyMeasure.h"
#include "afxdialogex.h"
#include <iostream>
#include <string>

#include "../../../GlobalVariables.h"

/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgCalbAccuracyMeasure::CDlgCalbAccuracyMeasure(UINT32 id, CWnd* parent)
	: CDlgSubTab(id, parent)
{
	m_enDlgID = ENG_CMDI_TAB::en_menu_tab_calb_accuracy_measure;
	m_bCamLiveState = FALSE;
	m_u8SelHead = 0;
	m_u32IndexNum = 0;
	m_ullTimeMove = 0;
	m_bHoldCtrl = FALSE;
}

CDlgCalbAccuracyMeasure::~CDlgCalbAccuracyMeasure()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgCalbAccuracyMeasure::DoDataExchange(CDataExchange* dx)
{
	CDlgSubTab::DoDataExchange(dx);
	UINT32 i, u32StartID;

	u32StartID = IDC_CALB_ACCURACY_MEASURE_BTN_OPEN_CTRL_PANEL;
	for (i = 0; i < eCALB_ACCURACY_MEASURE_BTN_MAX; i++)		DDX_Control(dx, u32StartID + i, m_btn_ctl[i]);

	u32StartID = IDC_CALB_ACCURACY_MEASURE_STT_POINT;
	for (i = 0; i < eCALB_ACCURACY_MEASURE_STT_MAX; i++)		DDX_Control(dx, u32StartID + i, m_stt_ctl[i]);

	u32StartID = IDC_CALB_ACCURACY_MEASURE_PIC_ROI;
	for (i = 0; i < eCALB_ACCURACY_MEASURE_PIC_MAX; i++)		DDX_Control(dx, u32StartID + i, m_pic_ctl[i]);

	u32StartID = IDC_CALB_ACCURACY_MEASURE_GRD_SELECT_CAM;
	for (i = 0; i < eCALB_ACCURACY_MEASURE_GRD_MAX; i++)			DDX_Control(dx, u32StartID + i, m_grd_ctl[i]);
}


BEGIN_MESSAGE_MAP(CDlgCalbAccuracyMeasure, CDlgSubTab)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CALB_ACCURACY_MEASURE_BTN_OPEN_CTRL_PANEL, IDC_CALB_ACCURACY_MEASURE_BTN_OPEN_CTRL_PANEL + eCALB_ACCURACY_MEASURE_BTN_MAX, OnBtnClick)
	ON_NOTIFY(NM_CLICK, IDC_CALB_ACCURACY_MEASURE_GRD_SELECT_CAM, &CDlgCalbAccuracyMeasure::OnClickGridSelectCam)
	ON_NOTIFY(NM_CLICK, IDC_CALB_ACCURACY_MEASURE_GRD_OPTION, &CDlgCalbAccuracyMeasure::OnClickGridOption)
	ON_NOTIFY(NM_CLICK, IDC_CALB_ACCURACY_MEASURE_GRD_DATA, &CDlgCalbAccuracyMeasure::OnClickGridData)
	ON_MESSAGE(eMSG_ACCR_MEASURE_GRAB, RefreshGrabView)
	ON_MESSAGE(eMSG_ACCR_MEASURE_REPORT, ReciveReportMsg)
	ON_WM_TIMER()
END_MESSAGE_MAP()


/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgCalbAccuracyMeasure::PreTranslateMessage(MSG* msg)
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
BOOL CDlgCalbAccuracyMeasure::OnInitDlg()
{
	/* 컨트롤 초기화 */
	InitCtrl();
	InitGridSelectCam();
	InitGridOption();
	InitGridData();
	InitDispMark();
	
	if (FALSE == CAccuracyMgr::GetInstance()->SetRegistModel())
	{
		AfxMessageBox(L"Failed to setup the model for search target", MB_ICONSTOP | MB_TOPMOST);
	}
	
	return TRUE;
}

VOID CDlgCalbAccuracyMeasure::InitDispMark()
{
	CWnd* pWnd[2];
	pWnd[0] = GetDlgItem(IDC_CALB_ACCURACY_MEASURE_PIC_ROI);
	LPRECT temp = nullptr;


	CRect baseRect;
	auto baseControl = GetDlgItem(IDC_CALB_ACCURACY_MEASURE_STT_CAMERA_VIEW);
	baseControl->GetWindowRect(baseRect);

	pWnd[0]->MoveWindow(baseRect.left, baseRect.top, baseRect.Width() * 0.95, baseRect.Width() * 0.95, TRUE);

	baseControl = GetDlgItem(IDC_CALB_ACCURACY_MEASURE_PIC_ROI);
	baseControl->GetWindowRect(baseRect);

	/*CRect grabBtn;
	pWnd[1] = GetDlgItem(IDC_CALB_ACCURACY_MEASURE_BTN_GRAB);
	pWnd[1]->GetWindowRect(grabBtn);
	pWnd[1]->MoveWindow(grabBtn.left, baseRect.top + baseRect.Height(), grabBtn.Width(), grabBtn.Height(), TRUE);*/

	uvEng_Camera_SetDisp(pWnd, 0x03);
	//DispResize(pWnd[0]);

	uvEng_Camera_DrawOverlayDC(false, DISP_TYPE_CALB_ACCR, 0);
}

/* desc: Frame Control을 이미지 사이즈 비율로 재조정 */
VOID CDlgCalbAccuracyMeasure::DispResize(CWnd* pWnd)
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_ACCURACY_MEASURE_GRD_SELECT_CAM];
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

	uvEng_Camera_SetAccuracyMeasureDispSize(tgtSize);
}


/*
 desc : 이미지 뷰 영역 갱신
 parm : None
 retn : None
*/
VOID CDlgCalbAccuracyMeasure::InvalidateView()
{
	CRect rView;
	m_pic_ctl[eCALB_ACCURACY_MEASURE_PIC_ROI].GetWindowRect(rView);
	ScreenToClient(rView);
	InvalidateRect(rView, TRUE);
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgCalbAccuracyMeasure::OnExitDlg()
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
VOID CDlgCalbAccuracyMeasure::OnPaintDlg(CDC* dc)
{
	ENG_VCCM enMode = uvEng_Camera_GetCamMode();
	RECT rDraw;

	/* 이미지가 출력될 윈도 영역 */
	m_pic_ctl[0].GetWindowRect(&rDraw);
	ScreenToClient(&rDraw);

	/* 현재 Calibration Mode인 경우 */
	if (enMode == ENG_VCCM::en_cali_mode)
	{
		uvEng_Camera_DrawCaliBitmap(dc->m_hDC, rDraw);
	}

	/* 자식 윈도 호출 */
	CDlgSubTab::DrawOutLine();
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgCalbAccuracyMeasure::OnResizeDlg()
{
}


/*
 desc : 컨트롤
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgCalbAccuracyMeasure::UpdateControl(UINT64 tick, BOOL is_busy)
{
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgCalbAccuracyMeasure::InitCtrl()
{
	CResizeUI clsResizeUI;
	CString strTemp;
	CRect rStt;

	for (int i = 0; i < eCALB_ACCURACY_MEASURE_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);

		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
	}

	/* static - Normal */
	for (int i = 0; i < eCALB_ACCURACY_MEASURE_STT_MAX; i++)
	{
		m_stt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_stt_ctl[i].SetDrawBg(1);
		m_stt_ctl[i].SetBaseProp(0, 1, 0, 0, DEF_COLOR_BTN_MENU_NORMAL, 0, WHITE_);

		clsResizeUI.ResizeControl(this, &m_stt_ctl[i]);

		m_stt_ctl[i].GetWindowRect(rStt);
		this->ScreenToClient(rStt);
		rStt.right += 2;

		m_stt_ctl[i].MoveWindow(rStt);
	}

	for (int i = 0; i < eCALB_ACCURACY_MEASURE_PIC_MAX; i++)
	{
		clsResizeUI.ResizeControl(this, &m_pic_ctl[i]);
	}

	for (int i = 0; i < eCALB_ACCURACY_MEASURE_GRD_MAX; i++)
	{
		m_grd_ctl[i].SetParent(this);
	}
}


VOID CDlgCalbAccuracyMeasure::InitGridSelectCam()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <int>			vRowSize(1);
	std::vector <int>			vColSize(CAMER_ACOUNT);
	std::vector <std::wstring>	vTitle(CAMER_ACOUNT);
	std::vector <COLORREF>		vColColor(CAMER_ACOUNT);

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_ACCURACY_MEASURE_GRD_SELECT_CAM];

	clsResizeUI.ResizeControl(this, pGrid);
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	int nTotalWidth = 0;
	int nIndex = 0;
	for (auto& width : vColSize)
	{
		vTitle[nIndex] = _T("CAMERA ") + std::to_wstring(nIndex + 1);
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
	pGrid->SetRowHeight(0, rGrid.Height() - 1);
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


VOID CDlgCalbAccuracyMeasure::InitGridOption()
{
	CResizeUI					clsResizeUI;
	CRect						rGrid;
	CString						strFilePath;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_ACCURACY_MEASURE_GRD_OPTION];
	std::vector <int>			vColSize(eOPTION_COL_MAX);
	std::vector <int>			vRowSize(eOPTION_ROW_MAX);
	std::vector <std::wstring>	vTitle[eOPTION_ROW_MAX];
	std::vector <COLORREF>		vColColor = { ALICE_BLUE, WHITE_ };

	vTitle[eOPTION_ROW_X_DRV] = { _T("Select X motor"), _T("STAGE X") };
	vTitle[eOPTION_ROW_USE_CAL] = { _T("Use calibration data"), _T("NO") };
	vTitle[eOPTION_ROW_CAL_FILE_PATH] = { _T("Calibration file"), _T("None") };
	vTitle[eOPTION_DOUBLE_MARK] = { _T("Double Mark"), _T("NO") };
	
	vTitle[eOPTION_EXPO_AREA_MEASURE] = { _T("Expo area measure"), _T("NO") };
	
	 //serchmode = ;
	CAccuracyMgr::GetInstance()->SetSearchMode(CAccuracyMgr::SearchMode::single);

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

	strFilePath = uvEng_ACamCali_GetFilePath();
	if (FALSE == strFilePath.IsEmpty())
	{
		int nCnt = strFilePath.GetLength() - strFilePath.ReverseFind('\\') - 1;

		if (0 < nCnt)
		{
			vTitle[eOPTION_ROW_CAL_FILE_PATH][1] = strFilePath.Right(nCnt);
		}
	}

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

			if (eOPTION_COL_VALUE == nCol)
			{
				if (eOPTION_ROW_CAL_FILE_PATH == nRow)
				{
					pGrid->SetItemState(nRow, nCol, GVIS_READONLY);
				}
				else
				{
					pGrid->SetCellType(nRow, nCol, RUNTIME_CLASS(CGridCellCombo));
					CStringArray options;

					switch (nRow)
					{
						case eOPTION_ROW_X_DRV:
						{
							options.Add(_T("STAGE X"));
							options.Add(_T("CAMERA X"));
						}
						break;
							
						default:
						{
							options.Add(_T("NO"));
							options.Add(_T("YES"));
						}
						break;
					}

					CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, nCol);
					pComboCell->SetOptions(options);
					pComboCell->SetStyle(CBS_DROPDOWN);
				}
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

VOID CDlgCalbAccuracyMeasure::InitGridData(BOOL bIsReload/* = FALSE*/)
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <int>			vRowSize(CAccuracyMgr::GetInstance()->GetPointCount() + 1);
	std::vector <int>			vColSize(eRESULT_COL_MAX);
	std::vector <std::wstring>	vTitle = { _T("INDEX"), _T("POINT X"), _T("POINT Y"), _T("X ERR"), _T("Y ERR") };
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_ACCURACY_MEASURE_GRD_DATA];
	ST_ACCR_PARAM stParam;

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
		width = (rGrid.Width() / (int)vColSize.size()) + 5;
		nTotalWidth += width;
	}

	vColSize.front() += rGrid.Width() - nTotalWidth - 2;

	if (rGrid.Height() < nTotalHeight)
	{
		vColSize.front() -= ::GetSystemMetrics(SM_CXVSCROLL);
	}

	/* 객체 기본 설정 */
	pGrid->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
	pGrid->SetRowCount((int)vRowSize.size());
	pGrid->SetColumnCount((int)vColSize.size());
	pGrid->SetRowHeight(0, nHeight);
	pGrid->SetFixedColumnCount(0);
	pGrid->SetBkColor(WHITE_);
	pGrid->SetFixedColumnSelection(0);

	for (int nRow = 0; nRow < (int)vRowSize.size(); nRow++)
	{
		pGrid->SetRowHeight(nRow, nHeight);

		for (int nCol = 0; nCol < (int)vColSize.size(); nCol++)
		{
			pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			// Title
			if (0 == nCol || 0 == nRow)
			{
				pGrid->SetItemBkColour(nRow, nCol, ALICE_BLUE);
				pGrid->SetItemFgColour(nRow, nCol, BLACK_);

				if (0 == nRow)
				{
					pGrid->SetColumnWidth(nCol, vColSize[nCol]);
					pGrid->SetItemText(nRow, nCol, vTitle[nCol].c_str());
				}
			}
		}

		if (0 < nRow)
		{
			if (TRUE == CAccuracyMgr::GetInstance()->GetPointParam(nRow - 1, stParam))
			{
				pGrid->SetItemTextFmt(nRow, 0, _T("%d"), nRow);
				pGrid->SetItemTextFmt(nRow, 1, _T("%.4f"), stParam.dMotorX);
				pGrid->SetItemTextFmt(nRow, 2, _T("%.4f"), stParam.dMotorY);
				pGrid->SetItemTextFmt(nRow, 3, _T("%.4f"), stParam.dValueX);
				pGrid->SetItemTextFmt(nRow, 4, _T("%.4f"), stParam.dValueY);
			}
		}
	}

	/* 기본 속성 및 갱신 */
	pGrid->SetBaseGridProp(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}

/*
 desc : 일반 버튼 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgCalbAccuracyMeasure::OnBtnClick(UINT32 id)
{
	int nBtnID = id - IDC_CALB_ACCURACY_MEASURE_BTN_OPEN_CTRL_PANEL;

	switch (nBtnID)
	{
	case eCALB_ACCURACY_MEASURE_BTN_OPEN_CTRL_PANEL:
		::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), eMSG_MAIN_OPEN_CONSOLE, NULL, NULL);
		break;
	case eCALB_ACCURACY_MEASURE_BTN_START:

		if (0 >= CAccuracyMgr::GetInstance()->GetPointCount())
		{
			AfxMessageBox(_T("LOAD 버튼을 눌러 측정 영역 문서를 선택하세요."));
			return;
		}

		if (IDYES == AfxMessageBox(L"현재 해당 기능은 임시 기능이며\n인터락이 적용되어 있지 않습니다.\n간섭 여부 확인을 위해 중단하시겠습니까?", MB_YESNO))
		{
			return;
		}

		MeasureStart();
		break;
	case eCALB_ACCURACY_MEASURE_BTN_STOP:
		CAccuracyMgr::GetInstance()->SetStopProcess();

		for (int i = 0; i < eCALB_ACCURACY_MEASURE_TIMER_MAX; i++)
		{
			KillTimer(i);
		}

		HoldControl(FALSE);
		break;
	case eCALB_ACCURACY_MEASURE_BTN_LIVE:
		m_bCamLiveState = !m_bCamLiveState;
		SetLiveView(m_bCamLiveState);
		break;
	case eCALB_ACCURACY_MEASURE_BTN_LOAD:
		LoadMeasureField();
		break;
	case eCALB_ACCURACY_MEASURE_BTN_NEW:
		MakeField();
		break;
	case eCALB_ACCURACY_MEASURE_BTN_REMEASURE:
		Remeasurement();
		break;
	case eCALB_ACCURACY_MEASURE_BTN_MOVE:
		PointMoveStart();
		break;
	case eCALB_ACCURACY_MEASURE_BTN_GRAB:
		MarkGrab();
		break;
	case eCALB_ACCURACY_MEASURE_BTN_MAKE_FILE:
		SaveClacFile();
		break;
	case eCALB_ACCURACY_MEASURE_BTN_EXCEL_EXPORT:
		SaveAsExcelFile();
		break;
	default:
		break;
	}
}


VOID CDlgCalbAccuracyMeasure::OnClickGridSelectCam(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_ACCURACY_MEASURE_GRD_SELECT_CAM];

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	if (pItem->iColumn == m_u8SelHead)
	{
		return;
	}

	if (TRUE == m_bHoldCtrl)
	{
		return;
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

	//CCamSpecMgr::GetInstance()->SetCamID(m_u8SelHead + 1);
	CAccuracyMgr::GetInstance()->SetCamID(m_u8SelHead + 1);

	pGrid->Refresh();
}


VOID CDlgCalbAccuracyMeasure::OnClickGridOption(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_ACCURACY_MEASURE_GRD_OPTION];

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	if (TRUE == m_bHoldCtrl)
	{
		return;
	}

	if (eOPTION_ROW_CAL_FILE_PATH == pItem->iRow &&
		eOPTION_COL_VALUE == pItem->iColumn)
	{
		LoadCalibrationFile();
	}


	pGrid->Refresh();
}


VOID CDlgCalbAccuracyMeasure::OnClickGridData(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_ACCURACY_MEASURE_GRD_DATA];

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	if (0 >= pItem->iRow)
	{
		return;
	}

	if (TRUE == m_bHoldCtrl)
	{
		return;
	}

	if (0 != m_u32IndexNum)
	{
		ResetDataViewPoint(m_u32IndexNum);
	}

	if (m_u32IndexNum != pItem->iRow)
	{
		m_u32IndexNum = pItem->iRow;

		for (int nCol = 0; nCol < pGrid->GetColumnCount(); nCol++)
		{
			pGrid->SetItemBkColour(m_u32IndexNum, nCol, LIME);
		}
	}
	else
	{
		m_u32IndexNum = 0;
	}


	pGrid->Refresh();
}

/*
 desc : 주기적으로 갱신되는 항목
 parm : None
 retn : None
*/
VOID CDlgCalbAccuracyMeasure::UpdatePeriod(UINT64 tick, BOOL is_busy)
{
	// 	/* for Align Camera */
	// 	UpdateCtrlACam();

	if (!is_busy)
	{
		// 	/* Grabbed Image View 출력 */
		UpdateLiveView();
	}

	UpdateControl(tick, is_busy);
}

/*
 desc : Updates the live image
 parm : None
 retn : None
*/
VOID CDlgCalbAccuracyMeasure::UpdateLiveView()
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

	

	if (!uvEng_Camera_IsCamModeLive())
	{


	}
	else
	{
		UINT8 u8ACamID = (0 == GetCheckACam()) ? 0x01 : 0x02;
		uvEng_Camera_DrawImageBitmap(DISP_TYPE_CALB_ACCR, u8ACamID - 1, u8ACamID);
		uvEng_Camera_DrawOverlayDC(false, DISP_TYPE_CALB_ACCR, 0);
	}
}

bool CDlgCalbAccuracyMeasure::IsSearchMarkTypeChanged()
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_ACCURACY_MEASURE_GRD_OPTION];
	auto itemText = pGrid->GetItemText(eOPTION_DOUBLE_MARK, eOPTION_COL_VALUE);
	auto select = (itemText == "NO" || itemText == "No")? CAccuracyMgr::SearchMode::single : CAccuracyMgr::SearchMode::multi;

	if (CAccuracyMgr::GetInstance()->GetSearchMode() != select)
	{
		CAccuracyMgr::GetInstance()->SetSearchMode(select);
		return true;
	}
	return false;
}

VOID CDlgCalbAccuracyMeasure::ResetDataViewPoint(UINT32 u32Row)
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_ACCURACY_MEASURE_GRD_DATA];
	pGrid->SetItemBkColour(u32Row, 0, ALICE_BLUE);

	for (int nCol = 1; nCol < pGrid->GetColumnCount(); nCol++)
	{
		pGrid->SetItemBkColour(u32Row, nCol, WHITE_);
	}

	pGrid->Refresh();
}


VOID CDlgCalbAccuracyMeasure::SetLiveView(BOOL bLive)
{
	BOOL bSucc = FALSE;
	uvEng_Camera_ClearShapes(DISP_TYPE_CALB_ACCR);
	if (TRUE == bLive)
	{
		m_btn_ctl[eCALB_ACCURACY_MEASURE_BTN_LIVE].SetBgColor(LIME);
		m_btn_ctl[eCALB_ACCURACY_MEASURE_BTN_LIVE].SetTextColor(BLACK_);

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
		m_btn_ctl[eCALB_ACCURACY_MEASURE_BTN_LIVE].SetBgColor(g_clrBtnColor);
		m_btn_ctl[eCALB_ACCURACY_MEASURE_BTN_LIVE].SetTextColor(WHITE_);

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

				Sleep(10);

			} while (1);

			/* Align Camera is Calibration Mode */
			uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
		}
	}

	//Invalidate(FALSE);
	Invalidate(TRUE);
}


BOOL CDlgCalbAccuracyMeasure::SaveClacFile()
{
	CString strExt = _T("dat Files|dat");
	CString strFileName;
	CString strTime;
	CString strFilter = _T("Data Files(*.dat)|*.dat|All Files (*.*)|*.*||");
	SYSTEMTIME stTm;

	/* 현재 시간 얻기 */
	GetLocalTime(&stTm);
	strTime.Format(_T("%04d-%02d-%02d(%02d%02d)"), stTm.wYear, stTm.wMonth, stTm.wDay, stTm.wHour, stTm.wMinute);
	strFileName.Format(_T("%s\\%s\\xy2d\\2D_%s.dat"), g_tzWorkDir, CUSTOM_DATA_CONFIG, strTime, _T("%s"));
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_ACCURACY_MEASURE_GRD_OPTION];
	CFileDialog fileSaveDialog(FALSE, strExt, strFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter);
	BOOL expoAreaMeasure = pGrid->GetItemText(eOPTION_EXPO_AREA_MEASURE, eOPTION_COL_VALUE) == "YES";

	CAccuracyMgr::GetInstance()->SetExpoAreaMeasure(expoAreaMeasure);

	if (fileSaveDialog.DoModal() == IDOK)
	{
		strFileName = fileSaveDialog.GetPathName();

		strFileName = strFileName.Left(strFileName.GetLength() - 4);
		strFileName.Append(_T("%s"));

		if (FALSE == CAccuracyMgr::GetInstance()->SaveCaliFile(strFileName))
		{
			AfxMessageBox(_T("저장 실패"));
		}
		else
		{
			AfxMessageBox(_T("저장 완료"));
		}
	}

	return TRUE;
}


VOID CDlgCalbAccuracyMeasure::LoadMeasureField()
{
	CString strExt = _T("CSV Files|csv");
	CString strFileName;
	CString strFilter = _T("CSV Files(*.csv)|*.csv|All Files (*.*)|*.*||");

	strFileName.Format(_T("%s\\%s\\cali\\"), g_tzWorkDir, CUSTOM_DATA_CONFIG);

	CFileDialog fileSaveDialog(TRUE, strExt, strFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter);

	if (fileSaveDialog.DoModal() == IDOK)
	{
		strFileName = fileSaveDialog.GetPathName();

		CAccuracyMgr::GetInstance()->LoadMeasureField(strFileName);
		InitGridData(TRUE);
	}

	return;
}


VOID CDlgCalbAccuracyMeasure::LoadCalibrationFile()
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_ACCURACY_MEASURE_GRD_OPTION];
	CString strExt = _T("Data Files|dat");
	CString strFileName;
	CString strFilter = _T("Data Files(*.dat)|*.dat|All Files (*.*)|*.*||");

	strFileName.Format(_T("%s\\%s\\xy2d\\"), g_tzWorkDir, CUSTOM_DATA_CONFIG);

	CFileDialog fileSaveDialog(TRUE, strExt, strFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter);

	if (fileSaveDialog.DoModal() == IDOK)
	{
		if (!uvEng_ACamCali_LoadFileEx(fileSaveDialog.GetPathName()))
		{
			if (IDNO == AfxMessageBox(L"Failed to load the calibration data for align camera\n Do you want to continue", MB_YESNO))
			{
				return;
			}
		}

		pGrid->SetItemText(eOPTION_ROW_CAL_FILE_PATH, eOPTION_COL_VALUE, fileSaveDialog.GetFileName());
		pGrid->Refresh();
	}
}


VOID CDlgCalbAccuracyMeasure::MakeField()
{
	CDlgParam dlg;
	DLG_PARAM stParam;
	VCT_DLG_PARAM stVctParam;
	CString strName;
	CDPoint dpStartPos;

	stParam.Init();
	stParam.strName = _T("Start X Position");
	stParam.strValue = _T("");
	stParam.strUnit = _T("mm");
	stParam.enFormat = ENM_DITM::en_double;
	stParam.u8DecPts = 4;
	stVctParam.push_back(stParam); //0

	stParam.Init();
	stParam.strName = _T("Start Y Position");
	stParam.strValue = _T("");
	stParam.strUnit = _T("mm");
	stParam.enFormat = ENM_DITM::en_double;
	stParam.u8DecPts = 4;
	stVctParam.push_back(stParam); //1

	stParam.Init();
	stParam.strName = _T("Max Grid X Point");
	stParam.strValue = _T("");
	stParam.strUnit = _T("Cnt");
	stParam.enFormat = ENM_DITM::en_int32;
	stParam.dMin = 0;
	stVctParam.push_back(stParam);//2

	stParam.Init();
	stParam.strName = _T("Max Grid Y Point");;
	stParam.strValue = _T("");
	stParam.strUnit = _T("Cnt");
	stParam.enFormat = ENM_DITM::en_int32;
	stParam.dMin = 0;
	stVctParam.push_back(stParam);//3

	stParam.Init();
	stParam.strName = _T("Angle");
	stParam.strValue = _T("0");
	stParam.strUnit = _T("°");
	stParam.enFormat = ENM_DITM::en_double;
	stParam.u8DecPts = 10;
	stVctParam.push_back(stParam);//4

	stParam.Init();
	stParam.strName = _T("Pitch");
	stParam.strValue = _T("");
	stParam.strUnit = _T("mm");
	stParam.enFormat = ENM_DITM::en_double;
	stParam.dMin = 0.0001;
	stParam.u8DecPts = 4;
	stVctParam.push_back(stParam);//5

	stParam.Init();
	stParam.strName = _T("Direction");
	stParam.strValue = _T("1");
	stParam.strUnit = _T("(1 = x, 2 = y)");
	stParam.enFormat = ENM_DITM::en_double;
	stParam.dMin = 1;
	stParam.dMax = 2;
	stParam.u8DecPts = 0;
	stVctParam.push_back(stParam);//6

	stParam.Init();
	stParam.strName = _T("Turn back");
	stParam.strValue = _T("2");
	stParam.strUnit = _T("(1 = true, 2 = false)");
	stParam.enFormat = ENM_DITM::en_double;
	stParam.dMin = 1;
	stParam.dMax = 2;
	stParam.u8DecPts = 0;
	stVctParam.push_back(stParam);//7


	stParam.Init();
	stParam.strName = _T("first pad Pos x");
	stParam.strValue = _T("");
	stParam.strUnit = _T("mm");
	stParam.enFormat = ENM_DITM::en_double;
	stParam.dMin = 0;
	stParam.dMax = 1000;
	stParam.u8DecPts = 4;
	stVctParam.push_back(stParam);//8

	stParam.Init();
	stParam.strName = _T("first pad Pos y");
	stParam.strValue = _T("");
	stParam.strUnit = _T("mm");
	stParam.enFormat = ENM_DITM::en_double;
	stParam.dMin = 0;
	stParam.dMax = 1000;
	stParam.u8DecPts = 4;
	stVctParam.push_back(stParam);//9


	if (IDOK == dlg.MyDoModal(stVctParam))
	{
		;
		CString strExt = _T("CSV Files|csv");
		CString strFileName;
		CString strFilter = _T("CSV Files(*.csv)|*.csv|All Files (*.*)|*.*||");

		strFileName.Format(_T("%s\\%s\\cali\\"), g_tzWorkDir, CUSTOM_DATA_CONFIG);

		CFileDialog fileSaveDialog(FALSE, strExt, strFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter);

		if (fileSaveDialog.DoModal() == IDOK)
		{
			strFileName = fileSaveDialog.GetPathName();

			dpStartPos.x = _ttof(stVctParam[0].strValue);
			dpStartPos.y = _ttof(stVctParam[1].strValue);
			dpStartPos.x2 = std::stod(string(CT2A(stVctParam[8].strValue.GetLength() == 0 ? L"0" : stVctParam[8].strValue)));
			dpStartPos.y2 = std::stod(string(CT2A(stVctParam[9].strValue.GetLength() == 0 ? L"0" : stVctParam[9].strValue)));

			CAccuracyMgr::GetInstance()->MakeMeasureField(strFileName, dpStartPos, 0, 0,
				_ttof(stVctParam[4].strValue),
				_ttof(stVctParam[5].strValue),
				CPoint(_ttoi(stVctParam[2].strValue), _ttoi(stVctParam[3].strValue)) , 
				stVctParam[6].strValue == "1" ? true : false,
				stVctParam[7].strValue == "1" ? true : false);
		
		}
	}
}


VOID CDlgCalbAccuracyMeasure::SaveAsExcelFile()
{
	CString strFileName;
	CString lpszExt = _T("csv Files|csv");
	CString strFilter = _T("CSV Files(*.csv)|*.csv|All Files (*.*)|*.*||");
	SYSTEMTIME stTm = { NULL };
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_ACCURACY_MEASURE_GRD_DATA];

	/* 등록된 데이터가 없는지 확인 */
	if (pGrid->GetRowCount() < 2)
		return;

	/* 현재 시간 */
	GetLocalTime(&stTm);
	/* 윈도 임시 파일로 저장 후 열기 */
	strFileName.Format(_T("%s\\%s\\cali\\2D_FIELD_%02u%02u%02u%02u%02u.csv"),
		g_tzWorkDir, CUSTOM_DATA_CONFIG, stTm.wYear - 2000, stTm.wMonth, stTm.wDay,
		stTm.wHour, stTm.wMinute);

	CFileDialog fileSaveDialog(FALSE, lpszExt, strFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter);

	if (fileSaveDialog.DoModal() == IDOK)
	{
		strFileName = fileSaveDialog.GetPathName();

		/* 파일로 저장 */
		pGrid->Save(strFileName);

		/* 저장된 파일 열기 */
		ShellExecute(NULL, _T("open"), _T("excel.exe"), _T("\"") + strFileName + _T("\""), NULL, SW_SHOW);
	}
}


VOID CDlgCalbAccuracyMeasure::MeasureStart()
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_ACCURACY_MEASURE_GRD_OPTION];
	CString strText;
	BOOL bSet = FALSE;

	bSet = (0 == pGrid->GetItemText(eOPTION_ROW_X_DRV, eOPTION_COL_VALUE).Compare(_T("STAGE X"))) ? FALSE : TRUE;
	CAccuracyMgr::GetInstance()->SetUseCamDrv(bSet);

	bSet = (0 == pGrid->GetItemText(eOPTION_ROW_USE_CAL, eOPTION_COL_VALUE).Compare(_T("NO"))) ? FALSE : TRUE;
	CAccuracyMgr::GetInstance()->SetUseCalData(bSet);

	bool expoAreaMeasure = pGrid->GetItemText(eOPTION_EXPO_AREA_MEASURE, eOPTION_COL_VALUE) == "YES";

	CAccuracyMgr::GetInstance()->SetExpoAreaMeasure(expoAreaMeasure);

	if (expoAreaMeasure)
	{
		if (CAccuracyMgr::GetInstance()->IsGbrpadPosSetted() == false)
		{
			AfxMessageBox(_T("시작 거버 포지션 지정이 되어있지 않습니다. "), MB_OK);
			return;
		}

		CAccuracyMgr::GetInstance()->SetUseCamDrv(FALSE);
		strText = _T("노광영역 보정테이블 작성.\n계속 진행하시겠습니까?");
	}
	else
		strText.Format(_T("동작할 X축 : %s\n보정 파일 적용 여부 : %s\n계속 진행하시겠습니까?"),
			pGrid->GetItemText(eOPTION_ROW_X_DRV, eOPTION_COL_VALUE),
			pGrid->GetItemText(eOPTION_ROW_USE_CAL, eOPTION_COL_VALUE));

	CAccuracyMgr::GetInstance()->SetMeasureMode(CAccuracyMgr::MeasureRegion::align);

	if (IDNO == AfxMessageBox(strText, MB_YESNO))
	{
		return;
	}


	if (pGrid->GetItemText(eOPTION_EXPO_AREA_MEASURE, eOPTION_COL_VALUE) == "YES")
	{
		auto centerCamIdx = GlobalVariables::GetInstance()->GetAlignMotion().markParams.centerCamIdx;
		CAccuracyMgr::GetInstance()->SetCamID(centerCamIdx);
		CAccuracyMgr::GetInstance()->SetMeasureMode(CAccuracyMgr::MeasureRegion::expo);	
	}

	if (expoAreaMeasure)
	{

	}
	else
	{
		if (TRUE == uvEng_ACamCali_GetFilePath().IsEmpty() && TRUE == CAccuracyMgr::GetInstance()->GetUseCalData())
		{
			CAccuracyMgr::GetInstance()->SetUseCalData(FALSE);

			if (IDYES == AfxMessageBox(_T("선택된 보정 파일이 없습니다.\n중단 하시겠습니까?"), MB_YESNO))
			{
				return;
			}
		}

		if (0 != m_u32IndexNum)
		{
			if (IDYES == AfxMessageBox(_T("선택한 포인트에서부터 시작하시겠습니까?"), MB_YESNO))
			{
				CAccuracyMgr::GetInstance()->SetStartIndex((int)m_u32IndexNum);
			}
			else
			{
				CAccuracyMgr::GetInstance()->SetStartIndex(0);
			}

			ResetDataViewPoint(m_u32IndexNum);
			m_u32IndexNum = 0;
		}
	}
	
	HoldControl(TRUE);

	CAccuracyMgr::GetInstance()->MeasureStart(this->GetSafeHwnd());
	SetTimer(eCALB_ACCURACY_MEASURE_TIMER_WORK, 500, NULL);
}


BOOL CDlgCalbAccuracyMeasure::MarkGrab(double* pdErrX, double* pdErrY)
{
	/* Live 종료 */
	if (TRUE == m_bCamLiveState)
	{
		m_bCamLiveState = FALSE;
		SetLiveView(FALSE);
	}

	UINT8 u8ACamID = GetCheckACam() + 1;
	CTactTimeCheck cTime;
	

	/* 기존 Live & Edge & Calibration 데이터 초기화 */
	uvEng_Camera_ResetGrabbedImage();

	if (ENG_VCCM::en_image_mode != uvEng_Camera_GetCamMode())
	{
		//doubleMarkAccurtest.RegistMark(u8ACamID);

		/* Mark Model이 등록되어 있는지 여부 */
		if (IsSearchMarkTypeChanged() || !uvEng_Camera_IsSetMarkModelACam(u8ACamID, 2))
		{
			auto serchmode = CAccuracyMgr::GetInstance()->GetSearchMode();

			if (FALSE == (serchmode == CAccuracyMgr::SearchMode::single ? CAccuracyMgr::GetInstance()->SetRegistModel() : CAccuracyMgr::GetInstance()->RegistMultiMark()))
				AfxMessageBox(L"Failed to setup the model for search target", MB_ICONSTOP | MB_TOPMOST);
				
			//AfxMessageBox(L"No Mark Model registered", MB_ICONSTOP | MB_TOPMOST);
			//return FALSE;
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
			return FALSE;
		}
	}

	/* 타이머 초기화 및 시작 */
	cTime.Init();
	cTime.Start();

	LPG_ACGR pstResult = nullptr;
	/* 일정 시간동안 동작 */
	while (1000 > (int)cTime.GetTactMs())
	{
		
		/* 검색된 결과가 있는지 조사 */
		auto searchMode = CAccuracyMgr::GetInstance()->GetSearchMode();
		//여기서 그려야됨
		if (searchMode == CAccuracyMgr::SearchMode::single)
		{
			pstResult = uvEng_Camera_RunModelCali(u8ACamID, 0xFE, (UINT8)DISP_TYPE_CALB_ACCR, TMP_MARK, TRUE, uvEng_GetConfig()->mark_find.image_process);

			if (NULL != pstResult)
			{
				if (NULL != pstResult->marked)
				{
					InvalidateView();

					if (NULL != pdErrX && NULL != pdErrY)
					{
						*pdErrX = pstResult->move_mm_x;
						*pdErrY = pstResult->move_mm_y;
					}
					return TRUE;
				}
			}
		}
		else
		{
			auto res = CAccuracyMgr::GetInstance()->RunDoublemarkTestExam(u8ACamID, pdErrX, pdErrY);
			
			if (res)return true;
			/*
			if(pstResult != nullptr && pstResult[0].marked == true && pstResult[1].marked == true)
			{
				delete[] pstResult;
				return true;
			}
			else delete[] pstResult;*/
		}
		cTime.Stop();
		Wait_(100);
	}

	AfxMessageBox(L"There is no analysis result", MB_ICONSTOP | MB_TOPMOST);
	return FALSE;
}


VOID CDlgCalbAccuracyMeasure::HoldControl(BOOL bSet)
{
	m_bHoldCtrl = bSet;
	m_grd_ctl[eCALB_ACCURACY_MEASURE_GRD_OPTION].EnableWindow(!bSet);

	for (int i = eCALB_ACCURACY_MEASURE_BTN_START; i < eCALB_ACCURACY_MEASURE_BTN_MAX; i++)
	{
		m_btn_ctl[i].EnableWindow(!bSet);
	}
}

BOOL CDlgCalbAccuracyMeasure::MotionMove(ENG_MMDI eMotor, double dPos)
{
	double dVel = uvEng_GetConfig()->mc2_svc.step_velo;

	if (CInterLockManager::GetInstance()->CheckMoveInterlock(eMotor, dPos))
	{
		return FALSE;
	}

	if (FALSE == uvEng_MC2_SendDevAbsMove(eMotor, dPos, dVel))
	{
		return FALSE;
	}

	return TRUE;
}

VOID CDlgCalbAccuracyMeasure::PointMoveStart()
{
	LPG_CIEA pstCfg = uvEng_GetConfig();
	CGridCtrl* pGrdData = &m_grd_ctl[eCALB_ACCURACY_MEASURE_GRD_DATA];
	CGridCtrl* pGrdOption = &m_grd_ctl[eCALB_ACCURACY_MEASURE_GRD_OPTION];
	ENG_MMDI eMotorX = ENG_MMDI::en_stage_x;
	CDPoint dpPos;

	if (0 != pGrdOption->GetItemText(eOPTION_ROW_X_DRV, eOPTION_COL_VALUE).Compare(_T("STAGE X")))
	{
		if (0 == m_u8SelHead)
		{
			eMotorX = ENG_MMDI::en_align_cam1;
		}
		else
		{
			eMotorX = ENG_MMDI::en_align_cam2;
		}
	}

	if (0 >= m_u32IndexNum)
	{
		AfxMessageBox(L"이동할 포인트를 선택하여 주시기 바랍니다.", MB_ICONSTOP | MB_TOPMOST);
		return;
	}

	HoldControl(TRUE);

	dpPos.x = pGrdData->GetItemTextToFloat(m_u32IndexNum, eRESULT_COL_POINT_X);
	dpPos.y = pGrdData->GetItemTextToFloat(m_u32IndexNum, eRESULT_COL_POINT_Y);

	if (ENG_MMDI::en_align_cam1 == eMotorX)
	{
		MotionMove(ENG_MMDI::en_align_cam2, pstCfg->set_cams.safety_pos[1]);
	}
	else if (ENG_MMDI::en_align_cam2 == eMotorX)
	{
		MotionMove(ENG_MMDI::en_align_cam1, pstCfg->set_cams.safety_pos[0]);
	}

	Wait_(200);

	MotionMove(ENG_MMDI::en_stage_y, dpPos.y);
	MotionMove(eMotorX, dpPos.x);

	m_ullTimeMove = GetTickCount64();
	SetTimer(eCALB_ACCURACY_MEASURE_TIMER_MOVE_START, 200, NULL);
}


VOID CDlgCalbAccuracyMeasure::Remeasurement()
{
	double dErrX = 0., dErrY = 0.;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_ACCURACY_MEASURE_GRD_DATA];

	if (0 == m_u32IndexNum)
	{
		AfxMessageBox(L"재측정할 포인트를 선택하여 주시기 바랍니다.", MB_ICONSTOP | MB_TOPMOST);
		return;
	}

	if (IDNO == AfxMessageBox(L"지금 위치에서 해당 포인트를 재측정하시겠습니까?", MB_YESNO))
	{
		return;
	}

	HoldControl(TRUE);

	if (FALSE == MarkGrab(&dErrX, &dErrY))
	{
		HoldControl(FALSE);
		return;
	}

	CAccuracyMgr::GetInstance()->SetPointErrValue(m_u32IndexNum - 1, dErrX, dErrY);

	pGrid->SetItemDouble(m_u32IndexNum, eRESULT_COL_VALUE_X, dErrX, 4);
	pGrid->SetItemDouble(m_u32IndexNum, eRESULT_COL_VALUE_Y, dErrY, 4);

	pGrid->Refresh();

	HoldControl(FALSE);
}

LRESULT CDlgCalbAccuracyMeasure::RefreshGrabView(WPARAM wParam, LPARAM lParam)
{
	double dX = 0;
	double dY = 0;

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_ACCURACY_MEASURE_GRD_DATA];

	InvalidateView();

	CAccuracyMgr::GetInstance()->GetPointErrValue((int)wParam, dX, dY);

	pGrid->SetItemDouble((int)wParam + 1, eRESULT_COL_VALUE_X, dX, 4);
	pGrid->SetItemDouble((int)wParam + 1, eRESULT_COL_VALUE_Y, dY, 4);

	pGrid->SetItemBkColour((int)wParam + 1, eRESULT_COL_VALUE_X, LIGHT_GRAY);
	pGrid->SetItemBkColour((int)wParam + 1, eRESULT_COL_VALUE_Y, LIGHT_GRAY);

	pGrid->SetCellView((int)wParam + 1, 0);

	pGrid->Refresh();

	return 0;
}

LRESULT CDlgCalbAccuracyMeasure::ReciveReportMsg(WPARAM wParam, LPARAM lParam)
{
	if (FALSE == CAccuracyMgr::GetInstance()->IsStopProcess())
	{
		switch ((int)wParam)
		{
		case CAccuracyMgr::e2DCAL_REPORT_OK:
			AfxMessageBox(_T("측정이 완료되었습니다."));
			break;
		case CAccuracyMgr::e2DCAL_REPORT_MOTION_FIAL:
			AfxMessageBox(L"측정을 정지합니다.\n정지 사유 : Motion Fail", MB_ICONSTOP | MB_TOPMOST);
			break;
		case CAccuracyMgr::e2DCAL_REPORT_GRAB_FIAL:
			AfxMessageBox(L"측정을 정지합니다.\n정지 사유 : Grab Fail", MB_ICONSTOP | MB_TOPMOST);
			break;
		default:
			break;
		}
	}

	return 0;
}

VOID CDlgCalbAccuracyMeasure::DoEvents()
{
	MSG	lMessage;

	while (PeekMessage(&lMessage, NULL, 0, 0, PM_REMOVE) != FALSE)
	{
		TranslateMessage(&lMessage);
		DispatchMessage(&lMessage);
	}
}

VOID CDlgCalbAccuracyMeasure::Wait_(DWORD dwTime)
{
	DWORD dwStart = GetTickCount();

	while (GetTickCount() - dwTime < dwStart)
	{
		DoEvents();
		Sleep(1);
	}
}

void CDlgCalbAccuracyMeasure::OnTimer(UINT_PTR nIDEvent)
{
	if (eCALB_ACCURACY_MEASURE_TIMER_WORK == nIDEvent)
	{
		if (FALSE == CAccuracyMgr::GetInstance()->IsProcessWorking())
		{
			KillTimer(nIDEvent);

			HoldControl(FALSE);
			return;
		}
	}
	else if (eCALB_ACCURACY_MEASURE_TIMER_MOVE_START == nIDEvent)
	{
		CGridCtrl* pGrdData = &m_grd_ctl[eCALB_ACCURACY_MEASURE_GRD_DATA];
		CGridCtrl* pGrdOption = &m_grd_ctl[eCALB_ACCURACY_MEASURE_GRD_OPTION];
		ENG_MMDI eMotorX = ENG_MMDI::en_stage_x;

		double dCurX = 0;
		double dCurY = 0;
		double dTargetX = 0;
		double dTargetY = 0;

		/* TIME OUT 1 Min */
		if (GetTickCount64() - m_ullTimeMove > DEF_DEFAULT_INTERLOCK_TIMEOUT)
		{
			m_ullTimeMove = 0;
			KillTimer(nIDEvent);

			AfxMessageBox(_T("Time Out Error"));
			HoldControl(FALSE);
			return;
		}

		if (0 != pGrdOption->GetItemText(eOPTION_ROW_X_DRV, eOPTION_COL_VALUE).Compare(_T("STAGE X")))
		{
			if (0 == m_u8SelHead)
			{
				eMotorX = ENG_MMDI::en_align_cam1;
			}
			else
			{
				eMotorX = ENG_MMDI::en_align_cam2;
			}
		}

		dTargetX = _ttof(pGrdData->GetItemText(m_u32IndexNum, eRESULT_COL_POINT_X));
		dTargetY = _ttof(pGrdData->GetItemText(m_u32IndexNum, eRESULT_COL_POINT_Y));
		dCurX = uvCmn_MC2_GetDrvAbsPos(eMotorX);
		dCurY = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);

		if (0.001 > fabs(dTargetX - dCurX) &&
			0.001 > fabs(dTargetY - dCurY))
		{
			m_ullTimeMove = 0;
			KillTimer(nIDEvent);

			AfxMessageBox(_T("이동 완료"));
			HoldControl(FALSE);
			return;
		}
	}

	CDlgSubTab::OnTimer(nIDEvent);
}
