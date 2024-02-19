// DlgCalbFlatness.cpp: 구현 파일
//
// DlgCalb.cpp: 구현 파일
//

#include "pch.h"
#include "DlgCalbFlatness.h"
#include "afxdialogex.h"


/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgCalbFlatness::CDlgCalbFlatness(UINT32 id, CWnd* parent)
	: CDlgSubMenu(id, parent)
{
	m_enDlgID = ENG_CMDI_SUB::en_menu_sub_calb_flatness;

	m_bEditMode = FALSE;
	m_bBtnManualFlag = FALSE;
	m_bBtnProcFlag = FALSE;
	m_dOldBasseValue = -1;

	m_cellOldPos.row = -1;
	m_cellOldPos.col = -1;

	m_pLDSMeasureThread = NULL;
	m_pFlatProcThread = NULL;

	m_nStep = 0;
	m_nCurSeqNum = 0;
	m_bStop = FALSE;

	m_bLDSMeasureThread = FALSE;
	m_bIngLDSMeasureThread = FALSE;

	m_bFlatProcThread = FALSE;
	m_bIngFlatProcThread = FALSE;

	m_dVctData.clear();
}

CDlgCalbFlatness::~CDlgCalbFlatness()
{
	m_bLDSMeasureThread = FALSE;
	m_bFlatProcThread = FALSE;

	// 쓰레드 종료까지 대기
	WaitForSingleObject(m_pLDSMeasureThread, INFINITE);
	WaitForSingleObject(m_pFlatProcThread, INFINITE);
	//

	m_pLDSMeasureThread = NULL;
	m_pFlatProcThread = NULL;

	delete m_pLDSMeasureThread;
	delete m_pFlatProcThread;

	m_dVctData.clear();
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgCalbFlatness::DoDataExchange(CDataExchange* dx)
{
	CDlgSubMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	u32StartID = IDC_CALB_FLATNESS_BTN_SAVE_DATA;
	for (i = 0; i < eBTN_FLATNESS_MAX; i++)			DDX_Control(dx, u32StartID + i, m_btn_ctl[i]);

	u32StartID = IDC_CALB_FLATNESS_STT_RESULT;
	for (i = 0; i < eSTT_FLATNESS_TITLE_MAX; i++)	DDX_Control(dx, u32StartID + i, m_stt_ctl[i]);

	u32StartID = IDC_CALB_FLATNESS_GRD_RESULT;
	for (i = 0; i < eGRD_FLATNESS_MAX; i++)			DDX_Control(dx, u32StartID + i, m_grd_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgCalbFlatness, CDlgSubMenu)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CALB_FLATNESS_BTN_SAVE_DATA, IDC_CALB_FLATNESS_BTN_OPEN_CTRL_PANEL, OnBtnClick)
	ON_NOTIFY(NM_CLICK, IDC_CALB_FLATNESS_GRD_RESULT, &CDlgCalbFlatness::OnGrdResultClick)
	ON_NOTIFY(NM_CLICK, IDC_CALB_FLATNESS_GRD_MANUAL_MEASURE, &CDlgCalbFlatness::OnGrdManualMeasureClick)
	ON_NOTIFY(NM_CLICK, IDC_CALB_FLATNESS_GRD_SETTING, &CDlgCalbFlatness::OnGrdSettingClick)
	ON_WM_TIMER()
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgCalbFlatness::PreTranslateMessage(MSG* msg)
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
BOOL CDlgCalbFlatness::OnInitDlg()
{
	/* 컨트롤 초기화 */
	InitCtrl();
	InitGridResult();
	InitGridSetting();
	InitGridManualMeasure();
	
	LoadDataConfig();
	SetTimer(eCALB_FLATNESS_TIMER_START, 100, NULL);
	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgCalbFlatness::OnExitDlg()
{
	KillTimer(eCALB_FLATNESS_TIMER_START);
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgCalbFlatness::OnPaintDlg(CDC* dc)
{

	/* 자식 윈도 호출 */
	CDlgSubMenu::DrawOutLine();
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgCalbFlatness::OnResizeDlg()
{
}

/*
 desc : 부모 스레드에 의해 주기적으로 호출됨
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgCalbFlatness::UpdatePeriod(UINT64 tick, BOOL is_busy)
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
VOID CDlgCalbFlatness::UpdateControl(UINT64 tick, BOOL is_busy)
{
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgCalbFlatness::InitCtrl()
{
	CResizeUI clsResizeUI;
	CString strTemp;
	CRect rStt;

	for (int i = 0; i < eBTN_FLATNESS_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL1_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
		// by sysandj : Resize UI
	}

	/* static - Normal */
	for (int i = 0; i < eSTT_FLATNESS_TITLE_MAX; i++)
	{
		m_stt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_stt_ctl[i].SetDrawBg(1);
		m_stt_ctl[i].SetBaseProp(0, 1, 0, 0, DEF_COLOR_BTN_MENU_NORMAL, 0, WHITE_);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_stt_ctl[i]);
		m_stt_ctl[i].GetWindowRect(rStt);
		this->ScreenToClient(rStt);
		rStt.right += 2;

		m_stt_ctl[i].MoveWindow(rStt);
	}
}

VOID CDlgCalbFlatness::InitGridResult()
{
	CGridCtrl* pGrd = &m_grd_ctl[eGRD_FLATNESS_RESULT];
	CResizeUI clsResizeUI;
	CRect rGrid;

	clsResizeUI.ResizeControl(this, pGrd);
	pGrd->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	pGrd->SetColumnResize(FALSE);
	pGrd->SetRowResize(FALSE);
	pGrd->SetEditable(FALSE);
	pGrd->EnableSelection(FALSE);
	pGrd->ModifyStyle(WS_HSCROLL, 0);		// Scroll delete

	RefreshResultGrid();
}

VOID CDlgCalbFlatness::InitGridSetting()
{
	CGridCtrl* pGrd = &m_grd_ctl[eGRD_FLATNESS_SETTING];
	CResizeUI clsResizeUI;
	CString strSettingList[eFLAT_SETTING_ROW_MAX][eFLAT_SETTING_COL_MAX] = {
		{_T("START POS"),	_T("X"),	_T("0.0000"),	_T("mm"),	_T("END POS"),			_T("X"),	_T("0.0000"),	_T("mm")	},
		{_T(""),			_T("Y"),	_T("0.0000"),	_T("mm"),	_T(""),					_T("Y"),	_T("0.0000"),	_T("mm")	},
		{_T(""),			_T("T"),	_T("0.0000"),	_T("mm"),	_T(""),					_T("T"),	_T("0.0000"),	_T("mm")	},
		{_T(""),			_T("Z"),	_T("0.0000"),	_T("mm"),	_T(""),					_T("Z"),	_T("0.0000"),	_T("mm")	},
		{_T(""),			_T("MOVE"),	_T(""),			_T(""),		_T(""),					_T("MOVE"),	_T(""),			_T("")		},
		{_T("SPEED"),		_T("X"),	_T("0.000"),	_T("mm/s"),	_T("COUNT OF LINE"),	_T("X"),	_T("0"),		_T("CNT")	},
		{_T(""),			_T("Y"),	_T("0.000"),	_T("mm/s"),	_T(""),					_T("Y"),	_T("0"),		_T("CNT")	},
		{_T(""),			_T("T"),	_T("0.000"),	_T("mm/s"),	_T("INTERVAL"),			_T("X"),	_T("0.0000"),	_T("mm")	},
		{_T(""),			_T("Z"),	_T("0.000"),	_T("mm/s"),	_T(""),					_T("Y"),	_T("0.0000"),	_T("mm")	},
		{_T("DELAY TIME"),	_T(""),		_T("0"),		_T("ms"),	_T("CYCLE"),			_T(""),		_T("0"),		_T("CNT")	}
	};

	CRect rcGrdRect;

	clsResizeUI.ResizeControl(this, pGrd);
	pGrd->GetWindowRect(rcGrdRect);
	this->ScreenToClient(rcGrdRect);
	pGrd->DeleteAllItems();

	int	nHeight = rcGrdRect.Height() - 1;
	int nCellHeight = nHeight / eFLAT_SETTING_ROW_MAX;

	int	nHeightDiffer = nHeight - (nCellHeight * eFLAT_SETTING_ROW_MAX);
	int	nWidthDiffer = 0;

	ST_GRID_COL	stGridCol[eFLAT_SETTING_COL_MAX] =
	{
		{ _T(""),	DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS,	(int)((rcGrdRect.Width() / eFLAT_SETTING_COL_MAX) * 1.2)},
		{ _T(""),	DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS,	(int)((rcGrdRect.Width() / eFLAT_SETTING_COL_MAX) * 0.4)},
		{ _T(""),	DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS,	(int)((rcGrdRect.Width() / eFLAT_SETTING_COL_MAX) * 1.4)},
		{ _T(""),	DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS,	(int)((rcGrdRect.Width() / eFLAT_SETTING_COL_MAX) * 0.6)},
		{ _T(""),	DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS,	(int)((rcGrdRect.Width() / eFLAT_SETTING_COL_MAX) * 1.2)},
		{ _T(""),	DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS,	(int)((rcGrdRect.Width() / eFLAT_SETTING_COL_MAX) * 0.4)},
		{ _T(""),	DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS,	(int)((rcGrdRect.Width() / eFLAT_SETTING_COL_MAX) * 1.4)},
		{ _T(""),	DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS,	(int)((rcGrdRect.Width() / eFLAT_SETTING_COL_MAX) * 0.6)}
	};

	for (int i = 0; i < eFLAT_SETTING_COL_MAX; i++)
	{
		nWidthDiffer += stGridCol[i].nWidth;
	}

	nWidthDiffer = rcGrdRect.Width() - nWidthDiffer - 1;

	if (rcGrdRect.Height() < nCellHeight * eFLAT_SETTING_ROW_MAX)
	{
		nWidthDiffer -= ::GetSystemMetrics(SM_CXVSCROLL);
	}

	stGridCol[eFLAT_SETTING_COL_LEFT_NAME].nWidth += (int)(nWidthDiffer * 0.5);
	stGridCol[eFLAT_SETTING_COL_RIGHT_NAME].nWidth += (int)(nWidthDiffer * 0.5);

	pGrd->SetRowCount(eFLAT_SETTING_ROW_MAX);
	pGrd->SetColumnCount(eFLAT_SETTING_COL_MAX);

	for (int nRow = 0; nRow < eFLAT_SETTING_ROW_MAX; nRow++)
	{
		for (int nCol = 0; nCol < eFLAT_SETTING_COL_MAX; nCol++)
		{
			pGrd->SetItemState(nRow, nCol, GVIS_READONLY);
			pGrd->SetItemText(nRow, nCol, strSettingList[nRow][nCol]);
			pGrd->SetItemFormat(nRow, nCol, stGridCol[nCol].nFormat);
			pGrd->SetColumnWidth(nCol, stGridCol[nCol].nWidth);

			if (eFLAT_SETTING_ROW_MOVE == nRow && (eFLAT_SETTING_COL_LEFT_MOVE == nCol || eFLAT_SETTING_COL_RIGHT_MOVE == nCol))
			{
				pGrd->SetItemBkColour(nRow, nCol, g_clrBtnColor);
				pGrd->SetItemFgColour(nRow, nCol, g_clrBtnTextColor);
			}
			else if (eFLAT_SETTING_COL_LEFT_NAME == nCol || eFLAT_SETTING_COL_RIGHT_NAME == nCol
				|| eFLAT_SETTING_COL_LEFT_POS == nCol || eFLAT_SETTING_COL_RIGHT_POS == nCol
				|| eFLAT_SETTING_COL_LEFT_UNIT == nCol || eFLAT_SETTING_COL_RIGHT_UNIT == nCol)
			{
				pGrd->SetItemBkColour(nRow, nCol, ALICE_BLUE);
				pGrd->SetItemFgColour(nRow, nCol, BLACK_);
			}
			else
			{
				pGrd->SetItemBkColour(nRow, nCol, DEF_COLOR_BTN_MENU_NORMAL_TEXT);

				if ((eFLAT_SETTING_COL_RIGHT_PARAM == nCol && eFLAT_SETTING_ROW_POS_T == nRow)
					|| (eFLAT_SETTING_COL_RIGHT_PARAM == nCol && eFLAT_SETTING_ROW_POS_Z == nRow))
				{
					pGrd->SetItemFgColour(nRow, nCol, DARK_GRAY);
				}
				else
				{
					pGrd->SetItemFgColour(nRow, nCol, BLACK_);
				}
			}
		}

		if (eFLAT_SETTING_ROW_MOVE == nRow)
		{
			pGrd->SetRowHeight(nRow, nCellHeight + nHeightDiffer);
		}
		else
		{
			pGrd->SetRowHeight(nRow, nCellHeight);
		}
	}

	pGrd->MergeCells(CCellRange(eFLAT_SETTING_ROW_POS_X, eFLAT_SETTING_COL_LEFT_NAME, eFLAT_SETTING_ROW_MOVE, eFLAT_SETTING_COL_LEFT_NAME));
	pGrd->MergeCells(CCellRange(eFLAT_SETTING_ROW_MOVE, eFLAT_SETTING_COL_LEFT_MOVE, eFLAT_SETTING_ROW_MOVE, eFLAT_SETTING_COL_LEFT_UNIT));
	pGrd->MergeCells(CCellRange(eFLAT_SETTING_ROW_POS_X, eFLAT_SETTING_COL_RIGHT_NAME, eFLAT_SETTING_ROW_MOVE, eFLAT_SETTING_COL_RIGHT_NAME));
	pGrd->MergeCells(CCellRange(eFLAT_SETTING_ROW_MOVE, eFLAT_SETTING_COL_RIGHT_MOVE, eFLAT_SETTING_ROW_MOVE, eFLAT_SETTING_COL_RIGHT_UNIT));

	pGrd->MergeCells(CCellRange(eFLAT_SETTING_ROW_PARAM_X, eFLAT_SETTING_COL_LEFT_NAME, eFLAT_SETTING_ROW_PARAM_Z, eFLAT_SETTING_COL_LEFT_NAME));
	pGrd->MergeCells(CCellRange(eFLAT_SETTING_ROW_PARAM_X, eFLAT_SETTING_COL_RIGHT_NAME, eFLAT_SETTING_ROW_PARAM_Y, eFLAT_SETTING_COL_RIGHT_NAME));
	pGrd->MergeCells(CCellRange(eFLAT_SETTING_ROW_PARAM_T, eFLAT_SETTING_COL_RIGHT_NAME, eFLAT_SETTING_ROW_PARAM_Z, eFLAT_SETTING_COL_RIGHT_NAME));

	pGrd->MergeCells(CCellRange(eFLAT_SETTING_ROW_MEASURE, eFLAT_SETTING_COL_LEFT_NAME, eFLAT_SETTING_ROW_MEASURE, eFLAT_SETTING_COL_LEFT_POS));
 	pGrd->MergeCells(CCellRange(eFLAT_SETTING_ROW_MEASURE, eFLAT_SETTING_COL_RIGHT_NAME, eFLAT_SETTING_ROW_MEASURE, eFLAT_SETTING_COL_RIGHT_POS));

	pGrd->SetColumnResize(FALSE);
	pGrd->SetRowResize(FALSE);
	pGrd->EnableSelection(FALSE);
	pGrd->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
}

VOID CDlgCalbFlatness::InitGridManualMeasure()
{
	CGridCtrl* pGrd = &m_grd_ctl[eGRD_FLATNESS_MANUAL_MEASURE];
	CResizeUI clsResizeUI;

	CString strList[eFLAT_MANUAL_ROW_MAX][eFLAT_MANUAL_COL_MAX] = {
//		{_T("NONE"),	_T("")},
		{_T("RESULT"),	_T("0.000")},
		{_T("BASE"),	_T("0.000")}
	};

	// 디바이스 정보 불러오기
// 	CString strDevice;
// 	CStringArray strArrDevice;
// 
// 	GetNickName(strArrDevice);
// 
// 	if (NULL != strArrDevice.GetSize())
// 	{
// 		m_strCurManualDev = strArrDevice[0];
// 		strList[eFLAT_MANUAL_ROW_DEVICE][eFLAT_MANUAL_COL_TITLE] = strArrDevice[0];
// 	}

	CRect rcGrdRect;

	clsResizeUI.ResizeControl(this, pGrd);
	pGrd->GetWindowRect(rcGrdRect);
	this->ScreenToClient(rcGrdRect);
	pGrd->DeleteAllItems();

	int	nHeight = rcGrdRect.Height() - 1;
	int nCellHeight = nHeight / eFLAT_MANUAL_ROW_MAX;
	int	nHeightDiffer = nHeight - (nCellHeight * eFLAT_MANUAL_ROW_MAX);
	int	nWidthDiffer = 0;

	ST_GRID_COL	stGridCol[eFLAT_MANUAL_COL_MAX] =
	{
		{ _T(""),	DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS,	(int)((rcGrdRect.Width() / eFLAT_MANUAL_COL_MAX) * 0.6)},
		{ _T(""),	DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS,	(int)((rcGrdRect.Width() / eFLAT_MANUAL_COL_MAX))}
	};

	for (int i = 0; i < eFLAT_MANUAL_COL_MAX; i++)
	{
		nWidthDiffer += stGridCol[i].nWidth;
	}

	nWidthDiffer = rcGrdRect.Width() - nWidthDiffer - 1;

	if (rcGrdRect.Height() < nCellHeight * eFLAT_MANUAL_ROW_MAX)
	{
		nWidthDiffer -= ::GetSystemMetrics(SM_CXVSCROLL);
	}

	stGridCol[eFLAT_MANUAL_COL_VALUE].nWidth += nWidthDiffer;

	pGrd->SetRowCount(eFLAT_MANUAL_ROW_MAX);
	pGrd->SetColumnCount(eFLAT_MANUAL_COL_MAX);

	for (int nRow = 0; nRow < eFLAT_MANUAL_ROW_MAX; nRow++)
	{
		for (int nCol = 0; nCol < eFLAT_MANUAL_COL_MAX; nCol++)
		{
			pGrd->SetItemState(nRow, nCol, GVIS_READONLY);
			pGrd->SetItemText(nRow, nCol, strList[nRow][nCol]);
			pGrd->SetItemFormat(nRow, nCol, stGridCol[nCol].nFormat);
			pGrd->SetColumnWidth(nCol, stGridCol[nCol].nWidth);

// 			if (eFLAT_MANUAL_ROW_DEVICE == nRow)
// 			{
// 				pGrd->SetItemBkColour(nRow, nCol, LIGHT_GRAY);
// 			}
			if (/*eFLAT_MANUAL_ROW_DEVICE != nRow &&*/ eFLAT_MANUAL_COL_TITLE == nCol)
			{
				pGrd->SetItemBkColour(nRow, nCol, ALICE_BLUE);
			}
// 			else
// 			{
// 				pGrd->SetItemBkColour(nRow, nCol, LIGHT_GRAY);
// 			}

			pGrd->SetItemFgColour(nRow, nCol, BLACK_);
		}

// 		if (eFLAT_MANUAL_ROW_DEVICE == nRow)
// 		{
// 			pGrd->SetRowHeight(nRow, nCellHeight + nHeightDiffer);
// 		}
// 		else
		{
			pGrd->SetRowHeight(nRow, nCellHeight);
		}
	}

// 	pGrd->MergeCells(CCellRange(eFLAT_MANUAL_ROW_DEVICE, eFLAT_MANUAL_COL_TITLE, eFLAT_MANUAL_ROW_DEVICE, eFLAT_MANUAL_COL_VALUE));
// 	pGrd->SetItemState(eFLAT_MANUAL_ROW_DEVICE, eFLAT_MANUAL_COL_TITLE, GVIS_FIXED);
// 	pGrd->SetCellType(eFLAT_MANUAL_ROW_DEVICE, eFLAT_MANUAL_COL_TITLE, RUNTIME_CLASS(CGridCellCombo));
// 	CGridCellCombo* pGridCombo = (CGridCellCombo*)pGrd->GetCell(eFLAT_MANUAL_ROW_DEVICE, eFLAT_MANUAL_COL_TITLE);
// 	pGridCombo->SetOptions(strArrDevice);
// 	pGridCombo->SetStyle(CBS_DROPDOWNLIST);

	pGrd->SetColumnResize(FALSE);
	pGrd->SetRowResize(FALSE);
	pGrd->EnableSelection(FALSE);
	pGrd->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
	pGrd->Refresh();

//	strArrDevice.RemoveAll();
}

VOID CDlgCalbFlatness::RefreshResultGrid()
{
	CGridCtrl* pGrd = &m_grd_ctl[eGRD_FLATNESS_RESULT];
	LPG_CMAF pParam = &uvEng_GetConfig()->measure_flat;
	CRect rectSize;
	CString strCellText;
	LOGFONT stFont;

	int nRowMax = pParam->u8CountOfYLine + eFLAT_RESULT_ADD_TITLE;
	int nColMax = pParam->u8CountOfXLine + eFLAT_RESULT_ADD_TITLE;
	int nRow = 0;
	int nCol = 0;
	int nTitleHeight = 40;	/* Title Size */
	int nTitleWidth = 40;	/* Title Size */
	int nCellHeight = 0;
	int nCellWidth = 0;
	int nTitleSize = 0;
	int nFontSize = 0;

	pGrd->DeleteAllItems();

	pGrd->SetRowCount(nRowMax);
	pGrd->SetColumnCount(nColMax);
	pGrd->GetWindowRect(rectSize);

	if (0 == pParam->u8CountOfYLine || 0 == pParam->u8CountOfXLine)
	{
		return;
	}

	nCellHeight = (int)((rectSize.Height() - nTitleHeight) / pParam->u8CountOfYLine);
	nCellWidth = (int)((rectSize.Width() - nTitleWidth) / pParam->u8CountOfXLine);

	nCellHeight = (nCellHeight < 50) ? 50 : nCellHeight;	/* Min Cell Size */
	nCellWidth = (nCellWidth < 50) ? 50 : nCellWidth;		/* Min Cell Size */

	//nTitleHeight = rectSize.Height() - (nCellHeight * pParam->u8CountOfYLine) - 1;
	//nTitleWidth = rectSize.Width() - (nCellWidth * pParam->u8CountOfXLine) - 1;

	nColMax = (nColMax > 25) ? 25 : nColMax;

	nTitleSize = (int)((30. - ((double)nColMax * 0.7)));
	nFontSize = (int)((double)(30 - nColMax));

	nTitleSize = (nTitleSize < 16) ? 16 : nTitleSize;		/* Min Font Size */
	nFontSize = (nFontSize < 16) ? 16 : nFontSize;

	for (nRow = 0; nRow < nRowMax; nRow++)
	{
		for (nCol = 0; nCol < nColMax; nCol++)
		{
			if (eFLAT_RESULT_TITLE == nRow || eFLAT_RESULT_TITLE == nCol)
			{
				pGrd->SetRowHeight(nRow, nTitleHeight);
				pGrd->SetColumnWidth(nCol, nTitleWidth);

				if (eFLAT_RESULT_TITLE == nRow && eFLAT_RESULT_TITLE == nCol)
				{
					strCellText = _T("▶");
				}
				else if (eFLAT_RESULT_TITLE == nCol)
				{
					strCellText.Format(_T("Y%d"), nRow);
				}
				else if (eFLAT_RESULT_TITLE == nRow)
				{
					strCellText.Format(_T("X%d"), nCol);
				}

				stFont = GetUserLogFont(nTitleSize, TRUE);
			}
			else
			{
				pGrd->SetRowHeight(nRow, nCellHeight);
				pGrd->SetColumnWidth(nCol, nCellWidth);

				strCellText = _T("0.000");
				stFont = GetUserLogFont(nFontSize, TRUE);
			}

			pGrd->SetItemFont(nRow, nCol, &stFont);
			pGrd->SetItemState(nRow, nCol, GVIS_READONLY);
			pGrd->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS);
			pGrd->SetItemText(nRow, nCol, strCellText);
		}
	}

	CleerResultGridColor();
}


VOID CDlgCalbFlatness::SetResultGridRestartColor()
{
	CGridCtrl* pGrd = &m_grd_ctl[eGRD_FLATNESS_RESULT];
	LPG_CMAF pParam = &uvEng_GetConfig()->measure_flat;

	int nCol = 0;
	int nRow = 0;
	int nSeqNum = 0;

	for (nSeqNum = 0; nSeqNum < m_nCurSeqNum; nSeqNum++)
	{
		nCol = nSeqNum % pParam->u8CountOfXLine;
		nRow = nSeqNum / pParam->u8CountOfXLine;

		// nRow의 값이 홀수인 경우 좌측 방향으로 진행
		if (0 != nRow % 2)
		{
			nCol = pParam->u8CountOfXLine - nCol - 1;
		}

		nCol++;
		nRow++;

		SetResultGridCellColor(nRow, nCol, PALE_GREEN);
	}
}


VOID CDlgCalbFlatness::LoadDataConfig()
{
	CGridCtrl* pGrd = &m_grd_ctl[eGRD_FLATNESS_SETTING];
	LPG_CMAF pParam = &uvEng_GetConfig()->measure_flat;
	COLORREF clCell = WHITE_;

	clCell = (0 == pParam->u8UseMotorT) ? DARK_GRAY : WHITE_;
	pGrd->SetItemFgColour(eFLAT_SETTING_ROW_POS_T, eFLAT_SETTING_COL_LEFT_PARAM, clCell);
	pGrd->SetItemFgColour(eFLAT_SETTING_ROW_PARAM_T, eFLAT_SETTING_COL_LEFT_PARAM, clCell);

	clCell = (0 == pParam->u8UseMotorZ) ? DARK_GRAY : WHITE_;
	pGrd->SetItemFgColour(eFLAT_SETTING_ROW_POS_Z, eFLAT_SETTING_COL_LEFT_PARAM, clCell);
	pGrd->SetItemFgColour(eFLAT_SETTING_ROW_PARAM_Z, eFLAT_SETTING_COL_LEFT_PARAM, clCell);

	pGrd->SetItemDouble(eFLAT_SETTING_ROW_POS_X, eFLAT_SETTING_COL_LEFT_PARAM, pParam->dStartXPos, 4);
	pGrd->SetItemDouble(eFLAT_SETTING_ROW_POS_Y, eFLAT_SETTING_COL_LEFT_PARAM, pParam->dStartYPos, 4);
	pGrd->SetItemDouble(eFLAT_SETTING_ROW_POS_T, eFLAT_SETTING_COL_LEFT_PARAM, pParam->dStartTPos, 4);
	pGrd->SetItemDouble(eFLAT_SETTING_ROW_POS_Z, eFLAT_SETTING_COL_LEFT_PARAM, pParam->dStartZPos, 4);
	pGrd->SetItemDouble(eFLAT_SETTING_ROW_POS_X, eFLAT_SETTING_COL_RIGHT_PARAM, pParam->dEndXPos, 4);
	pGrd->SetItemDouble(eFLAT_SETTING_ROW_POS_Y, eFLAT_SETTING_COL_RIGHT_PARAM, pParam->dEndYPos, 4);
	pGrd->SetItemDouble(eFLAT_SETTING_ROW_POS_T, eFLAT_SETTING_COL_RIGHT_PARAM, pParam->dStartTPos, 4);
	pGrd->SetItemDouble(eFLAT_SETTING_ROW_POS_Z, eFLAT_SETTING_COL_RIGHT_PARAM, pParam->dStartZPos, 4);

	pGrd->SetItemDouble(eFLAT_SETTING_ROW_PARAM_X, eFLAT_SETTING_COL_LEFT_PARAM, pParam->dSpeedX, 3);
	pGrd->SetItemDouble(eFLAT_SETTING_ROW_PARAM_Y, eFLAT_SETTING_COL_LEFT_PARAM, pParam->dSpeedY, 3);
	pGrd->SetItemDouble(eFLAT_SETTING_ROW_PARAM_T, eFLAT_SETTING_COL_LEFT_PARAM, pParam->dSpeedT, 3);
	pGrd->SetItemDouble(eFLAT_SETTING_ROW_PARAM_Z, eFLAT_SETTING_COL_LEFT_PARAM, pParam->dSpeedZ, 3);

	pGrd->SetItemText(eFLAT_SETTING_ROW_PARAM_X, eFLAT_SETTING_COL_RIGHT_PARAM, pParam->u8CountOfXLine);
	pGrd->SetItemText(eFLAT_SETTING_ROW_PARAM_Y, eFLAT_SETTING_COL_RIGHT_PARAM, pParam->u8CountOfYLine);
// 	pGrd->SetItemDouble(eFLAT_SETTING_ROW_PARAM_T, eFLAT_SETTING_COL_RIGHT_PARAM, pParam->dSpeedT, 4);
// 	pGrd->SetItemDouble(eFLAT_SETTING_ROW_PARAM_Z, eFLAT_SETTING_COL_RIGHT_PARAM, pParam->dSpeedZ, 4);

	pGrd->SetItemText(eFLAT_SETTING_ROW_MEASURE, eFLAT_SETTING_COL_LEFT_PARAM, pParam->u16DelayTime);
	pGrd->SetItemText(eFLAT_SETTING_ROW_MEASURE, eFLAT_SETTING_COL_RIGHT_PARAM, pParam->u8CountOfMeasure);

	pGrd->Refresh();
}

VOID CDlgCalbFlatness::SaveDataConfig()
{
	CGridCtrl* pGrd = &m_grd_ctl[eGRD_FLATNESS_SETTING];
	LPG_CMAF pParam = &uvEng_GetConfig()->measure_flat;

	if (IDNO == AfxMessageBox(L"저장하시겠습니까?", MB_YESNO))
	{
		return;
	}

	pParam->dStartXPos = pGrd->GetItemTextToFloat(eFLAT_SETTING_ROW_POS_X, eFLAT_SETTING_COL_LEFT_PARAM);
	pParam->dStartYPos = pGrd->GetItemTextToFloat(eFLAT_SETTING_ROW_POS_Y, eFLAT_SETTING_COL_LEFT_PARAM);
	pParam->dStartTPos = pGrd->GetItemTextToFloat(eFLAT_SETTING_ROW_POS_T, eFLAT_SETTING_COL_LEFT_PARAM);
	pParam->dStartZPos = pGrd->GetItemTextToFloat(eFLAT_SETTING_ROW_POS_Z, eFLAT_SETTING_COL_LEFT_PARAM);
	pParam->dEndXPos = pGrd->GetItemTextToFloat(eFLAT_SETTING_ROW_POS_X, eFLAT_SETTING_COL_RIGHT_PARAM);
	pParam->dEndYPos = pGrd->GetItemTextToFloat(eFLAT_SETTING_ROW_POS_Y, eFLAT_SETTING_COL_RIGHT_PARAM);
	pParam->dStartTPos = pGrd->GetItemTextToFloat(eFLAT_SETTING_ROW_POS_T, eFLAT_SETTING_COL_RIGHT_PARAM);
	pParam->dStartZPos = pGrd->GetItemTextToFloat(eFLAT_SETTING_ROW_POS_Z, eFLAT_SETTING_COL_RIGHT_PARAM);

	pParam->dSpeedX = pGrd->GetItemTextToFloat(eFLAT_SETTING_ROW_PARAM_X, eFLAT_SETTING_COL_LEFT_PARAM);
	pParam->dSpeedY = pGrd->GetItemTextToFloat(eFLAT_SETTING_ROW_PARAM_Y, eFLAT_SETTING_COL_LEFT_PARAM);
	pParam->dSpeedT = pGrd->GetItemTextToFloat(eFLAT_SETTING_ROW_PARAM_T, eFLAT_SETTING_COL_LEFT_PARAM);
	pParam->dSpeedZ = pGrd->GetItemTextToFloat(eFLAT_SETTING_ROW_PARAM_Z, eFLAT_SETTING_COL_LEFT_PARAM);

	pParam->u8CountOfXLine = pGrd->GetItemTextToInt(eFLAT_SETTING_ROW_PARAM_X, eFLAT_SETTING_COL_RIGHT_PARAM);
	pParam->u8CountOfYLine = pGrd->GetItemTextToInt(eFLAT_SETTING_ROW_PARAM_Y, eFLAT_SETTING_COL_RIGHT_PARAM);

	pParam->u16DelayTime = pGrd->GetItemTextToInt(eFLAT_SETTING_ROW_MEASURE, eFLAT_SETTING_COL_LEFT_PARAM);
	pParam->u8CountOfMeasure = pGrd->GetItemTextToInt(eFLAT_SETTING_ROW_MEASURE, eFLAT_SETTING_COL_RIGHT_PARAM);

	/* 환경 설정 값을 파일로 저장 */
	uvEng_SaveConfig();
}


BOOL CDlgCalbFlatness::SaveResult(CString strPath)
{
	CStdioFile sFile;

	if (FALSE == sFile.Open(strPath, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
	{
		return FALSE;
	}

	LPG_CMAF pParam = &uvEng_GetConfig()->measure_flat;
	CString strWrite;

	unsigned int nData = 0;
	double dMin = 9999.999;
	double dMax = -999.999;

	sFile.WriteString(_T("Result,"));

	// Set row
	for (int i = 1; i <= pParam->u8CountOfXLine; i++)
	{
		strWrite.Format(_T("X%d,"), i);
		sFile.WriteString(strWrite);
	}

	sFile.WriteString(_T("\n"));

	for (int i = 0; i < pParam->u8CountOfYLine; i++)
	{
		// Set Col
		strWrite.Format(_T("Y%d,"), i + 1);
		sFile.WriteString(strWrite);

		for (int j = 0; j < pParam->u8CountOfXLine; j++)
		{
			if (0 == i % 2)
			{
				nData = (i * pParam->u8CountOfXLine) + j;
			}
			else
			{
				nData = (i * pParam->u8CountOfXLine) + ((pParam->u8CountOfXLine - 1) - j);
			}

			// Insert Data
			if (m_dVctData.size() <= nData)
			{
				sFile.WriteString(_T("####,"));
			}
			else
			{
				if (9999.999 == fabs(m_dVctData[nData]))
				{
					strWrite = _T("####,");
				}
				else
				{
					if (dMin > m_dVctData[nData])
					{
						dMin = m_dVctData[nData];
					}
					if (dMax < m_dVctData[nData])
					{
						dMax = m_dVctData[nData];
					}

					strWrite.Format(_T("%.3f,"), m_dVctData[nData]);
				}

				sFile.WriteString(strWrite);
			}
		}

		sFile.WriteString(_T("\n"));
	}

	strWrite.Format(_T("\nMAX :,%.3f,\nMIN :,%.3f\nError :,%.3f\n"), dMax, dMin, dMax - dMin);
	sFile.WriteString(strWrite);

	sFile.Close();
	return TRUE;
}


BOOL CDlgCalbFlatness::BtnAuthority(int nBtn)
{
	switch (nBtn)
	{
	case eBTN_FLATNESS_EDIT:
	case eBTN_FLATNESS_SAVE_DATA:
	case eBTN_FLATNESS_REMEASUREMENT:
	case eBTN_FLATNESS_POINT_MOVE:
	case eBTN_FLATNESS_MANUAL_MEASURE:
		if (TRUE == m_bIngFlatProcThread)
		{
			return FALSE;
		}

		break;
	case eBTN_FLATNESS_START:
	case eBTN_FLATNESS_STOP:
		if (TRUE == m_bIngLDSMeasureThread
			|| TRUE == m_bEditMode)
		{
			return FALSE;
		}

		break;
	case eBTN_FLATNESS_OPEN_CTRL_PANEL:
		break;
	default:
		return FALSE;
	}

	return TRUE;
}


VOID CDlgCalbFlatness::StartManualMeasure()
{
	if (FALSE == m_bIngLDSMeasureThread)
	{
		m_bLDSMeasureThread = TRUE;
		m_pLDSMeasureThread = AfxBeginThread(LDSManualMeasureThread, this);
	}
}


VOID CDlgCalbFlatness::StopManualMeasure()
{
	if (TRUE == m_bIngLDSMeasureThread)
	{
		m_bLDSMeasureThread = FALSE;

		WaitForSingleObject(m_pLDSMeasureThread, INFINITE);
		m_pLDSMeasureThread = NULL;
	}
}


/*
 desc : 일반 버튼 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgCalbFlatness::OnBtnClick(UINT32 id)
{
	int nBtnID = id - IDC_CALB_FLATNESS_BTN_SAVE_DATA;

	if (FALSE == BtnAuthority(nBtnID))
	{
		return;
	}

	switch (nBtnID)
	{
	case eBTN_FLATNESS_SAVE_DATA:
		OnClickButtonSaveData();
		break;
	case eBTN_FLATNESS_REMEASUREMENT:
		OnClickButtonRemeasurement();
		break;
	case eBTN_FLATNESS_POINT_MOVE:
		OnClickButtonPointMove();
		break;
	case eBTN_FLATNESS_START:
		OnClickButtonStart();
		break;
	case eBTN_FLATNESS_STOP:
		OnClickButtonStop();
		break;
	case eBTN_FLATNESS_MANUAL_MEASURE:
		OnClickButtonManualMeasure();
		break;
	case eBTN_FLATNESS_EDIT:
		OnClickButtonEdit();
		break;
	case eBTN_FLATNESS_OPEN_CTRL_PANEL:
		OnClickButtonOpenCtrlPanel();
		break;
	default:
		break;
	}
}


VOID CDlgCalbFlatness::OnTimer(UINT_PTR nIDEvent)
{
	if (eCALB_FLATNESS_TIMER_START == nIDEvent)
	{
		CGridCtrl* pGrd = &m_grd_ctl[eGRD_FLATNESS_MANUAL_MEASURE];
		double dValue = 0;
		CString strInput;

		// Base Value 동기화
		if (m_dOldBasseValue != pGrd->GetItemTextToFloat(eFLAT_MANUAL_ROW_BASE, eFLAT_MANUAL_COL_VALUE))
		{
			m_dOldBasseValue = uvEng_KeyenceLDS_GetBaseValue();
			strInput.Format(_T("%.3f"), m_dOldBasseValue);
			pGrd->SetItemText(eFLAT_MANUAL_ROW_BASE, eFLAT_MANUAL_COL_VALUE, strInput);
		}

		// Manual Measure Button 변경
		if (TRUE == m_bIngLDSMeasureThread && FALSE == m_bBtnManualFlag)
		{
			m_btn_ctl[eBTN_FLATNESS_MANUAL_MEASURE].SetWindowText(_T("STOP"));
			m_btn_ctl[eBTN_FLATNESS_MANUAL_MEASURE].SetBgColor(LIGHT_RED);
			m_bBtnManualFlag = TRUE;
			Invalidate(FALSE);
		}
		else if (FALSE == m_bIngLDSMeasureThread && TRUE == m_bBtnManualFlag)
		{
			m_btn_ctl[eBTN_FLATNESS_MANUAL_MEASURE].SetWindowText(_T("MEASURE"));
			m_btn_ctl[eBTN_FLATNESS_MANUAL_MEASURE].SetBgColor(g_clrBtnColor);
			m_bBtnManualFlag = FALSE;
			Invalidate(FALSE);
		}

		// Auto Button 변경
		if (TRUE == m_bIngFlatProcThread && FALSE == m_bBtnProcFlag)
		{
			m_btn_ctl[eBTN_FLATNESS_STOP].SetBgColor(LIGHT_RED);
			m_bBtnProcFlag = TRUE;
			Invalidate(FALSE);
		}
		else if (FALSE == m_bIngFlatProcThread && TRUE == m_bBtnProcFlag)
		{
			m_btn_ctl[eBTN_FLATNESS_STOP].SetBgColor(g_clrBtnColor);
			m_bBtnProcFlag = FALSE;
			Invalidate(FALSE);
		}
	}

	CDlgSubMenu::OnTimer(nIDEvent);
}

VOID CDlgCalbFlatness::OnGrdResultClick(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrd = &m_grd_ctl[eGRD_FLATNESS_RESULT];

	if (FALSE == m_bIngFlatProcThread)
	{
		CCellID cellCurPos = pGrd->GetFocusCell();

		CleerResultGridColor();

		cellCurPos.col = pItem->iColumn;
		cellCurPos.row = pItem->iRow;

		if (cellCurPos != m_cellOldPos && 0 != cellCurPos.col && 0 != cellCurPos.row)
		{
			SetResultGridCellColor(cellCurPos.row, cellCurPos.col, HOT_PINK);
			m_cellOldPos = cellCurPos;
		}
		else
		{
			pGrd->SetFocusCell(eNONE, eNONE);
			m_cellOldPos.col = eNONE;
			m_cellOldPos.row = eNONE;
		}
	}

	pGrd->SetFocusCell(eNONE, eNONE);
	pGrd->Refresh();
}

VOID CDlgCalbFlatness::OnGrdManualMeasureClick(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrd = &m_grd_ctl[eGRD_FLATNESS_MANUAL_MEASURE];
	CString strInput;
	CString strMsg;

	if (TRUE == m_bIngLDSMeasureThread)
	{
		pGrd->SetFocusCell(eNONE, eNONE);
		return;
	}

	if (eFLAT_MANUAL_COL_VALUE == pItem->iColumn && eFLAT_MANUAL_ROW_BASE == pItem->iRow)
	{
		if (FALSE == m_bIngFlatProcThread)
		{
			if (TRUE == PopupKBDN(ENM_DITM::en_double, strInput, -99.999, 99.999, 3))
			{
				// 소숫점 자르기
				double dValue = _ttof(strInput);
				strInput.Format(_T("%.3f"), dValue);

				strMsg.Format(_T("[Base Value]를 [%s]로 변경하시겠습니까?"), strInput);
				if (IDYES == AfxMessageBox(strMsg, MB_YESNO))
				{
					uvEng_KeyenceLDS_ZeroSet(dValue);
					pGrd->SetItemText(eFLAT_MANUAL_ROW_BASE, eFLAT_MANUAL_COL_VALUE, strInput);
				}
			}
		}
	}

	pGrd->SetFocusCell(eNONE, eNONE);
	pGrd->Refresh();
}

VOID CDlgCalbFlatness::OnGrdSettingClick(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrd = &m_grd_ctl[eGRD_FLATNESS_SETTING];
	LPG_CMAF pParam = &uvEng_GetConfig()->measure_flat;
	ENM_DITM enType = ENM_DITM::en_double;
	double	dMin = 0;
	double	dMax = 9999.9999;
	UINT8 u8DecPts = 3;
	CString strOutput;
	BOOL bAutoChageParam = FALSE;

	/* Reset Focus */
	pGrd->SetFocusCell(eNONE, eNONE);

	// 동작 버튼
	if (eFLAT_SETTING_ROW_MOVE == pItem->iRow)
	{
		OnClickMoveFlatPos(pItem->iColumn);
		return;
	}

	if (!(eFLAT_SETTING_COL_LEFT_PARAM == pItem->iColumn || eFLAT_SETTING_COL_RIGHT_PARAM == pItem->iColumn))
	{
		return;
	}
	else if (FALSE == m_bEditMode)
	{
		return;
	}
	else if (TRUE == m_bIngFlatProcThread)
	{
		AfxMessageBox(_T("현재 측정중입니다."));
		return;
	}

	switch (pItem->iRow)
	{
	case eFLAT_SETTING_ROW_POS_T:
		if (FALSE == pParam->u8UseMotorT || eFLAT_SETTING_COL_RIGHT_PARAM == pItem->iColumn)	return;
		dMax = 360;
		u8DecPts = 4;
		bAutoChageParam = TRUE;
		break;
	case eFLAT_SETTING_ROW_POS_Z:
		if (FALSE == pParam->u8UseMotorZ || eFLAT_SETTING_COL_RIGHT_PARAM == pItem->iColumn)	return;
		dMax = 100;
		u8DecPts = 4;
		bAutoChageParam = TRUE;
		break;
	case eFLAT_SETTING_ROW_POS_X:
	case eFLAT_SETTING_ROW_POS_Y:
		dMax = 5000;
		u8DecPts = 4;
		enType = ENM_DITM::en_double;
		bAutoChageParam = TRUE;
		break;

	case eFLAT_SETTING_ROW_PARAM_X:
	case eFLAT_SETTING_ROW_PARAM_Y:
		if (eFLAT_SETTING_COL_RIGHT_PARAM == pItem->iColumn)
		{
			dMin = 1;
			dMax = 500;
			u8DecPts = 0;
			enType = ENM_DITM::en_int8;
			bAutoChageParam = TRUE;
		}
		break;
	case eFLAT_SETTING_ROW_PARAM_T:
		if (FALSE == pParam->u8UseMotorT && eFLAT_SETTING_COL_LEFT_PARAM == pItem->iColumn)	return;
		else if (eFLAT_SETTING_COL_RIGHT_PARAM == pItem->iColumn)
		{
			u8DecPts = 4;
			bAutoChageParam = TRUE;
		}
		break;
	case eFLAT_SETTING_ROW_PARAM_Z:
		if (FALSE == pParam->u8UseMotorZ && eFLAT_SETTING_COL_LEFT_PARAM == pItem->iColumn)	return;
		else if (eFLAT_SETTING_COL_RIGHT_PARAM == pItem->iColumn)
		{
			u8DecPts = 4;
			bAutoChageParam = TRUE;
		}
		break;
	case eFLAT_SETTING_ROW_MEASURE:

		if (eFLAT_SETTING_COL_LEFT_PARAM == pItem->iColumn)
		{
			dMin = 100;
			dMax = 3600;
		}
		else
		{
			dMin = 1;
			dMax = 50;
		}

		u8DecPts = 0;
		enType = ENM_DITM::en_int8;
		break;
	default:
		return;
	}

	if (PopupKBDN(enType, strOutput, dMin, dMax, u8DecPts))
	{
		pGrd->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);
	}

	pGrd->Refresh();
}

/*
 desc : X, Y 이동
 parm : 이동할 위치 값, 속도
 retn : 이동 성공 유무
*/
BOOL CDlgCalbFlatness::StageMoveXY(double dX, double dY, double dSpeedX, double dSpeedY)
{
	double dMin = -9999.;
	double dMax = 9999.;
	double dMaxSpeed = 9999.;

#ifdef MOTOR_SIMUL
	return TRUE;
#endif // MOTOR_SIMUL

	if (FALSE == uvCmn_MC2_IsDevLocked(ENG_MMDI::en_stage_x) ||
		FALSE == uvCmn_MC2_IsDevLocked(ENG_MMDI::en_stage_y))
	{
		LOG_MESG(ENG_EDIC::en_measure_flatness, _T("[FALTNESS][STAGE OFF]"));
		return FALSE;
	}
	// 에러 상태 확인
	if (TRUE == uvCmn_MC2_IsDriveError(ENG_MMDI::en_stage_x) ||
		TRUE == uvCmn_MC2_IsDriveError(ENG_MMDI::en_stage_y))
	{
		LOG_MESG(ENG_EDIC::en_measure_flatness, _T("[FALTNESS][STAGE ERROR]"));
		return FALSE;
	}
	// Busy 신호 확인
	if (TRUE == uvCmn_MC2_IsDriveBusy(ENG_MMDI::en_stage_x) ||
		TRUE == uvCmn_MC2_IsDriveBusy(ENG_MMDI::en_stage_y))
	{
		LOG_MESG(ENG_EDIC::en_measure_flatness, _T("[FALTNESS][STAGE BUSY]"));
		return FALSE;
	}

	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);

	if (0.005 < fabs(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x) - dX))
	{
		/* 이동 값의 최대 최소를 넘어가지 않는다. */
		dMin = uvEng_GetConfig()->mc2_svc.min_dist[(UINT8)ENG_MMDI::en_stage_x];
		dMax = uvEng_GetConfig()->mc2_svc.max_dist[(UINT8)ENG_MMDI::en_stage_x];
		dMaxSpeed = uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_x];

		dX = (dX < dMin) ? dMin : dX;
		dX = (dX > dMax) ? dMax : dX;
		dSpeedX = (dSpeedX > dMaxSpeed) ? dMaxSpeed : dSpeedX;

		if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_x, dX))
		{
			return FALSE;
		}
		if (FALSE == uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, dX, dSpeedX))
		{
			LOG_MESG(ENG_EDIC::en_measure_flatness, _T("[FALTNESS][STAGE X MOVE FAIL]"));
			return FALSE;
		}
	}
	if (0.005 < fabs(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y) - dY))
	{
		/* 이동 값의 최대 최소를 넘어가지 않는다. */
		dMin = uvEng_GetConfig()->mc2_svc.min_dist[(UINT8)ENG_MMDI::en_stage_y];
		dMax = uvEng_GetConfig()->mc2_svc.max_dist[(UINT8)ENG_MMDI::en_stage_y];
		dMaxSpeed = uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_y];

		dY = (dY < dMin) ? dMin : dY;
		dY = (dY > dMax) ? dMax : dY;
		dSpeedY = (dSpeedY > dMaxSpeed) ? dMaxSpeed : dSpeedY;

		if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_y, dY))
		{
			return FALSE;
		}
		if (FALSE == uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dY, dSpeedY))
		{
			LOG_MESG(ENG_EDIC::en_measure_flatness, _T("[FALTNESS][STAGE Y MOVE FAIL]"));
			return FALSE;
		}
	}
	return TRUE;
}

/*
 desc : X, Y 이동 확인
 parm : 이동해야 할 위치 값, 기다릴 시간 값, 이동 위치 오차 값
 retn : 이동이 완료되었는지 유무
*/
BOOL CDlgCalbFlatness::IsStageMovedXY(double dTargetX, double dTargetY, int nTimeOut/* = 60000*/, double dDiffDistance/* = 0.005*/)
{
	CTactTimeCheck cTime;			/* 타이머 */
	double dDiffX = 0.;				/* X위치 오차값 */
	double dDiffY = 0.;				/* Y위치 오차값 */

#ifdef MOTOR_SIMUL
	return TRUE;
#endif // MOTOR_SIMUL

	/* Check if it is the emulated (demo) mode ... */
	if (uvEng_GetConfig()->IsRunDemo())
	{
		return TRUE;
	}

	/* 타이머 초기화 및 시작 */
	cTime.Init();
	cTime.Start();

	/* 일정 시간동안 동작 */
	while (nTimeOut > (int)cTime.GetTactMs())
	{
		if (TRUE == m_bStop)
		{
			LOG_MESG(ENG_EDIC::en_measure_flatness, _T("[FALTNESS][STOP]"));
			return FALSE;
		}

		/* 타겟 위치와 현재위치의 오차를 구한다. */
		dDiffX = dTargetX - uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);
		dDiffY = dTargetY - uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);

		/* Motor가 타겟위치에 도달하고 정지 상태일 경우 종료 */
		if (FALSE == uvCmn_MC2_IsDriveBusy(ENG_MMDI::en_stage_x) &&
			FALSE == uvCmn_MC2_IsDriveBusy(ENG_MMDI::en_stage_y) &&
			fabs(dDiffX) < dDiffDistance &&
			fabs(dDiffY) < dDiffDistance)
		{
			return TRUE;
		}

		cTime.Stop();
		Sleep(1);
	}

	/* 시간 초과 */
	LOG_MESG(ENG_EDIC::en_measure_flatness, _T("[FALTNESS][MOVE TIME OUT]"));
	return FALSE;
}


VOID CDlgCalbFlatness::OnClickButtonSaveData()
{
	if (0 >= m_dVctData.size())
	{
		AfxMessageBox(_T("측정 결과를 찾을 수 없습니다."));
		return;
	}

	CString strExt = _T("csv Files|csv");
	CString strFileName;
	CString strTime;
	CString strFilter = _T("CSV Files(*.csv)|*.csv|All Files (*.*)|*.*||");
	SYSTEMTIME stTm;

	/* 현재 시간 얻기 */
	GetLocalTime(&stTm);
	strTime.Format(_T("%04d-%02d-%02d(%02d%02d)"), stTm.wYear, stTm.wMonth, stTm.wDay, stTm.wHour, stTm.wMinute);
	strFileName.Format(_T("FLAT_%s.csv"), strTime);

	CFileDialog fileSaveDialog(FALSE, strExt, strFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter);

	if (fileSaveDialog.DoModal() == IDOK)
	{
		strFileName = fileSaveDialog.GetPathName();

		if (FALSE == SaveResult(strFileName))
		{
			AfxMessageBox(_T("저장 실패"));
		}
		else
		{
			AfxMessageBox(_T("저장 완료"));
		}
	}
}


VOID CDlgCalbFlatness::OnClickButtonRemeasurement()
{
	if (0 >= m_dVctData.size())
	{
		return;
	}

	LPG_CMAF pParam = &uvEng_GetConfig()->measure_flat;
	DPOINT dOffset = CalOffset(m_cellOldPos.row, m_cellOldPos.col);

	if (0.01 < fabs(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x) - (pParam->dStartXPos + dOffset.x)) ||
		0.01 < fabs(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y) - (pParam->dStartYPos + dOffset.y)))
	{
		if (IDNO == AfxMessageBox(L"현재 위치가 다릅니다.\n재측정 하시겠습니까?", MB_YESNO))
		{
			return;
		}
	}

	double dValue = 0;
	double dSumValue = 0;
	double dAvrValue = 0;
	int nMeasureCnt = 0;
	int nSeq = 0;

	for (int i = 0; i < pParam->u8CountOfMeasure; i++)
	{
		dValue = uvEng_KeyenceLDS_Measure(DEF_DEVICE_NUM);

		if (0 != dValue)
		{
			dSumValue += dValue;
			nMeasureCnt++;
		}

		Wait_(100);
	}

	if (9999.999 == fabs(dValue))
	{
		AfxMessageBox(_T("재측정 실패"));
	}

	dAvrValue = dSumValue / nMeasureCnt;

	if (0 == (m_cellOldPos.row - 1) % 2)
	{
		nSeq = ((m_cellOldPos.row - 1) * pParam->u8CountOfXLine) + (m_cellOldPos.col - 1);
	}
	else
	{
		nSeq = ((m_cellOldPos.row - 1) * pParam->u8CountOfXLine) + ((pParam->u8CountOfXLine - 1) - (m_cellOldPos.col - 1));
	}

	m_dVctData[nSeq] = dAvrValue;
	InputResultGridValue(m_cellOldPos.row, m_cellOldPos.col, dAvrValue);
}


VOID CDlgCalbFlatness::OnClickButtonPointMove()
{
	CString strMsg;

	//20221014 mhbaek Add : 아무것도 선택하지 않고 버튼 클릭 시 -1, -1 위치로 이동하려는 현상 제한
	if (-1 == m_cellOldPos.col || -1 == m_cellOldPos.row)
	{
		AfxMessageBox(_T("이동할 위치를 선택해주세요."));
		return;
	}

	strMsg.Format(_T("[X:%d Y:%d] 위치로 이동하시겠습니까?"), m_cellOldPos.col, m_cellOldPos.row);

	if (IDNO == AfxMessageBox(strMsg, MB_YESNO))
	{
		return;
	}

	if (FALSE == MoveOffsetPos(CalOffset(m_cellOldPos.row, m_cellOldPos.col)))
	{
		AfxMessageBox(_T("이동 실패"));
	}
}


VOID CDlgCalbFlatness::OnClickButtonStart()
{
	if (TRUE == m_bIngFlatProcThread)
	{
		return;
	}
	if (TRUE == m_bIngLDSMeasureThread)
	{
		AfxMessageBox(_T("메뉴얼 측정 동작중입니다."));
		return;
	}
	if (IDNO == AfxMessageBox(_T("자동 측정을 시작하시겠습니까?"), MB_YESNO))
	{
		return;
	}

	CGridCtrl* pGrd = &m_grd_ctl[eGRD_FLATNESS_RESULT];
	LPG_CMAF pParam = &uvEng_GetConfig()->measure_flat;
	BOOL bClearCell = TRUE;
	m_nStep = eFLAT_PROC_INIT;

	// Kill Focus
	CleerResultGridColor();
	pGrd->SetFocusCell(eNONE, eNONE);
	m_cellOldPos.col = eNONE;
	m_cellOldPos.row = eNONE;
	//

	if (0 < m_dVctData.size() && m_dVctData.size() < pParam->u8CountOfXLine * pParam->u8CountOfYLine)
	{
		if (IDYES == AfxMessageBox(_T("측정 이력이 남아있습니다.\n재측정하시겠습니까?"), MB_YESNO))
		{
			m_nStep = eFLAT_PROC_MOVE_WAIT_POS;

			// 측정된 Cell 완료 표시 
			SetResultGridRestartColor();
			bClearCell = FALSE;
		}
	}

	if (TRUE == bClearCell)
	{
		RefreshResultGrid();
	}

	m_bStop = FALSE;
	m_bFlatProcThread = TRUE;
	m_pLDSMeasureThread = AfxBeginThread(FlatnessProcessThread, this);
}


VOID CDlgCalbFlatness::OnClickButtonStop()
{
	m_bStop = TRUE;

	if (TRUE == m_bIngFlatProcThread)
	{
		m_bFlatProcThread = FALSE;

		WaitForSingleObject(m_pFlatProcThread, INFINITE);
		m_pFlatProcThread = NULL;
	}
}


VOID CDlgCalbFlatness::OnClickButtonManualMeasure()
{
	if (FALSE == m_bLDSMeasureThread)
	{
		StartManualMeasure();
	}
	else
	{
		StopManualMeasure();
	}
}


VOID CDlgCalbFlatness::OnClickButtonEdit()
{
	CMacButton* pBtn = &m_btn_ctl[eBTN_FLATNESS_EDIT];
	if (TRUE == m_bEditMode)
	{
		if (IDYES == AfxMessageBox(_T("저장하시겠습니까?"), MB_YESNO))
		{
			SaveDataConfig();
			RefreshResultGrid();
		}

		m_bEditMode = FALSE;
		pBtn->SetWindowText(_T("EDIT"));
		pBtn->SetTextColor(g_clrBtnTextColor);
		pBtn->SetBgColor(g_clrBtnColor);
	}
	else
	{
		if (IDNO == AfxMessageBox(_T("수정하시겠습니까?"), MB_YESNO))
		{
			return;
		}

		m_bEditMode = TRUE;
		pBtn->SetWindowText(_T("SAVE"));
		pBtn->SetTextColor(BLACK_);
		pBtn->SetBgColor(GOLD);
	}

	Invalidate(FALSE);
}


VOID CDlgCalbFlatness::OnClickButtonOpenCtrlPanel()
{
	::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), eMSG_MAIN_OPEN_CONSOLE, NULL, NULL);
}


VOID CDlgCalbFlatness::OnClickMoveFlatPos(int nPos)
{
	BOOL bResult = FALSE;

	// Start Pos
	if (eFLAT_SETTING_COL_LEFT_MOVE == nPos)
	{
		if (IDNO == AfxMessageBox(_T("시작 위치로 이동하시겠습니까?"), MB_YESNO))
		{
			return;
		}

		// 시작 위치로 이동
		bResult = MoveStartPos();
	}
	// End Pos
	else if (eFLAT_SETTING_COL_RIGHT_MOVE == nPos)
	{
		if (IDNO == AfxMessageBox(_T("종료 위치로 이동하시겠습니까?"), MB_YESNO))
		{
			return;
		}

		// 종료 위치로 이동
		bResult = MoveEndPos();
	}

	if (FALSE == bResult)
	{
		AfxMessageBox(_T("이동 실패"));
	}
}


LOGFONT CDlgCalbFlatness::GetUserLogFont(int nSize, BOOL bIsBold)
{
	LOGFONT lf;

	lf.lfHeight = nSize;        // 10 -> 18
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfItalic = false;
	lf.lfUnderline = false;
	lf.lfStrikeOut = false;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = CLEARTYPE_NATURAL_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH;
	StrCpy(lf.lfFaceName, DEF_FONT_NAME);

	lf.lfWeight = bIsBold ? FW_BOLD : FW_NORMAL;

	return lf;
}

void CDlgCalbFlatness::InputManualValue(double dValue)
{
	CGridCtrl* pGrd = &m_grd_ctl[eGRD_FLATNESS_MANUAL_MEASURE];
	CString strValue;
	strValue.Format(_T("%.3f"), dValue);

	pGrd->SetItemText(eFLAT_MANUAL_ROW_RESULT, eFLAT_MANUAL_COL_VALUE, strValue);
	pGrd->Refresh();
}


VOID CDlgCalbFlatness::InputResultGridValue(int nRow, int nCol, double dData)
{
	CGridCtrl* pGrd = &m_grd_ctl[eGRD_FLATNESS_RESULT];
	CString strValue;

	if (nRow > pGrd->GetRowCount() ||
		nCol > pGrd->GetColumnCount() ||
		0 >= nRow || 0 >= nCol)
	{
		return;
	}

	if (9999.999 == dData)
	{
		strValue = _T("####");
	}
	else
	{
		strValue.Format(_T("%.3f"), dData);
	}

	pGrd->SetItemText(nRow, nCol, strValue);
	pGrd->Refresh();
}


VOID CDlgCalbFlatness::CleerResultGridColor()
{
	CGridCtrl* pGrd = &m_grd_ctl[eGRD_FLATNESS_RESULT];
	COLORREF clrCell;

	for (int nRow = 0; nRow < pGrd->GetRowCount(); nRow++)
	{
		for (int nCol = 0; nCol < pGrd->GetColumnCount(); nCol++)
		{
			clrCell = (eFLAT_RESULT_TITLE == nRow || eFLAT_RESULT_TITLE == nCol) ? LIGHT_GRAY : WHITE_;
			pGrd->SetItemBkColour(nRow, nCol, clrCell);
		}
	}

	pGrd->Refresh();
}


VOID CDlgCalbFlatness::SetResultGridCellColor(int nRow, int nCol, COLORREF cColor)
{
	CGridCtrl* pGrd = &m_grd_ctl[eGRD_FLATNESS_RESULT];

	if (nRow > pGrd->GetRowCount() ||
		nCol > pGrd->GetColumnCount() ||
		0 >= nRow || 0 >= nCol)
	{
		return;
	}

	pGrd->SetItemBkColour(nRow, nCol, cColor);
	pGrd->Refresh();
}


DPOINT CDlgCalbFlatness::CalOffset(int nRow, int nCol)
{
	LPG_CMAF pParam = &uvEng_GetConfig()->measure_flat;
	DPOINT ptOffset;

	if (1 < pParam->u8CountOfXLine)
	{
		ptOffset.x = ((pParam->dEndXPos - pParam->dStartXPos) / (pParam->u8CountOfXLine - 1)) * (nCol - 1);
	}
	if (1 < pParam->u8CountOfYLine)
	{
		ptOffset.y = ((pParam->dEndYPos - pParam->dStartYPos) / (pParam->u8CountOfYLine - 1)) * (nRow - 1);
	}

	return ptOffset;
}


BOOL CDlgCalbFlatness::MoveStartPos()
{
	LPG_CMAF pParam = &uvEng_GetConfig()->measure_flat;

	StageMoveXY(pParam->dStartXPos, pParam->dStartYPos, pParam->dSpeedX, pParam->dSpeedY);
	return IsStageMovedXY(pParam->dStartXPos, pParam->dStartYPos);
}

BOOL CDlgCalbFlatness::MoveEndPos()
{
	LPG_CMAF pParam = &uvEng_GetConfig()->measure_flat;

	StageMoveXY(pParam->dEndXPos, pParam->dEndYPos, pParam->dSpeedX, pParam->dSpeedY);
	return IsStageMovedXY(pParam->dEndXPos, pParam->dEndYPos);
}

BOOL CDlgCalbFlatness::MoveOffsetPos(DPOINT dOffset)
{
	LPG_CMAF pParam = &uvEng_GetConfig()->measure_flat;

	StageMoveXY(pParam->dStartXPos + dOffset.x, pParam->dStartYPos + dOffset.y, pParam->dSpeedX, pParam->dSpeedY);
	return IsStageMovedXY(pParam->dStartXPos + dOffset.x, pParam->dStartYPos + dOffset.y);
}


BOOL CDlgCalbFlatness::PopupKBDN(ENM_DITM enType, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts/* = 0*/)
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
		if (IDOK != dlg.MyDoModal(tzTitle, FALSE, TRUE, dMin, dMax))	return FALSE;
		break;
	case ENM_DITM::en_float:
	case ENM_DITM::en_double:
		swprintf_s(tzPoint, 512, L"%s:%%.%df,%s:%%.%df", tzMinMax[0], u8DecPts, tzMinMax[1], u8DecPts);
		swprintf_s(tzTitle, 1024, tzPoint, dMin, dMax);
		if (IDOK != dlg.MyDoModal(tzTitle, TRUE, TRUE, dMin, dMax))		return FALSE;
		break;
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
		break;
	}

	return TRUE;
}


void Wait_(int nTime_)
{
	DWORD dwStart = GetTickCount();

	while (GetTickCount() - nTime_ < dwStart)
	{
		Sleep(1);
	}
}

UINT LDSManualMeasureThread(LPVOID lpParam)
{
	CDlgCalbFlatness* pClass = (CDlgCalbFlatness*)lpParam;
	double dValue = 0;

	pClass->m_bIngLDSMeasureThread = TRUE;

	while (TRUE == pClass->m_bLDSMeasureThread)
	{
		dValue = uvEng_KeyenceLDS_Measure(DEF_DEVICE_NUM);
		pClass->InputManualValue(dValue);
		Sleep(200);
	}

	pClass->m_bIngLDSMeasureThread = FALSE;
	pClass->m_bLDSMeasureThread = FALSE;

	return 0;
}

UINT FlatnessProcessThread(LPVOID lpParam)
{
	CDlgCalbFlatness* pClass = (CDlgCalbFlatness*)lpParam;
	LPG_CMAF pParam = &uvEng_GetConfig()->measure_flat;
	DPOINT dOffset;
	CString strLog;

	BOOL bResult = TRUE;
	TCHAR tzMesg[1024] = { NULL };

	int nCol = 0;
	int nRow = 0;
	int nEndSeqNum = pParam->u8CountOfXLine * pParam->u8CountOfYLine;
	int nErrorCnt = 0;
	int nMeasureCnt = 0;

	double dValue = 0;
	double dSumValue = 0;
	double dAvrValue = 0;

	double dSeqAvrValue = 0;

	pClass->m_bIngFlatProcThread = TRUE;

	while (TRUE == pClass->m_bFlatProcThread)
	{
		memset(tzMesg, 0, sizeof(tzMesg));

		switch (pClass->m_nStep)
		{
		case eFLAT_PROC_FAIL:

			LOG_MESG(ENG_EDIC::en_measure_flatness, _T("[FALTNESS][ERROR OCCURRENCE]"));
			Wait_(100);
			pClass->m_bFlatProcThread = FALSE;
			break;

		case eFLAT_PROC_INIT:

			LOG_MESG(ENG_EDIC::en_measure_flatness, _T("[FALTNESS][INITIALIZE]"));

			pClass->m_nCurSeqNum = 0;
			pClass->m_dVctData.clear();
			
			pClass->CleerResultGridColor();

			pClass->m_nStep = eFLAT_PROC_MOVE_WAIT_POS;
			break;

		case eFLAT_PROC_MOVE_WAIT_POS:

			LOG_MESG(ENG_EDIC::en_measure_flatness, _T("[FALTNESS][MOVE WAIT POS]"));

			// 다른 안전 동작 진행 예정
			//

			pClass->m_nStep = (FALSE == bResult) ? eFLAT_PROC_FAIL : eFLAT_PROC_MOVE_MEASURE_POS;
			break;

		case eFLAT_PROC_MOVE_MEASURE_POS:

			// Point 계산
			nCol = pClass->m_nCurSeqNum % pParam->u8CountOfXLine;
			nRow = pClass->m_nCurSeqNum / pParam->u8CountOfXLine;

			// nRow의 값이 홀수인 경우 좌측 방향으로 진행
			if (0 != nRow % 2)
			{
				nCol = pParam->u8CountOfXLine - nCol - 1;
			}

			nCol++;
			nRow++;

			swprintf_s(tzMesg, 1024, L"[FALTNESS][MOVE MEASURE POS][%d][%d]", nCol, nRow);
			LOG_MESG(ENG_EDIC::en_measure_flatness, tzMesg);

			pClass->SetResultGridCellColor(nRow, nCol, YELLOW);

			// 측정 위치로 이동
			dOffset = pClass->CalOffset(nRow, nCol);
			bResult = pClass->MoveOffsetPos(dOffset);
			//

			pClass->m_nStep = (FALSE == bResult) ? eFLAT_PROC_FAIL : eFLAT_PROC_MEASUREMENT;
			break;

		case eFLAT_PROC_MEASUREMENT:

			swprintf_s(tzMesg, 1024, L"[FALTNESS][MEASUREMENT][%d][%d]", nCol, nRow);
			LOG_MESG(ENG_EDIC::en_measure_flatness, tzMesg);

			Wait_(pParam->u16DelayTime);

			dSumValue = 0;
			nMeasureCnt = 0;

			for (int i = 0; i < pParam->u8CountOfMeasure; i++)
			{
				dValue = uvEng_KeyenceLDS_Measure(DEF_DEVICE_NUM);

				if (0 != dValue)
				{
					dSumValue += dValue;
					nMeasureCnt++;
				}

				Sleep(100);
			}

			dAvrValue = dSumValue / nMeasureCnt;

			pClass->m_dVctData.push_back(dAvrValue);
			pClass->InputResultGridValue(nRow, nCol, dAvrValue);
			pClass->SetResultGridCellColor(nRow, nCol, PALE_GREEN);

			swprintf_s(tzMesg, 1024, L"[FALTNESS][MEASUREMENT][RESULT][%.3f]", dAvrValue);
			LOG_MESG(ENG_EDIC::en_measure_flatness, tzMesg);

			pClass->m_nCurSeqNum++;

			if (FALSE == bResult)
			{
				pClass->m_nStep = eFLAT_PROC_FAIL;
			}
			else if (pClass->m_nCurSeqNum < nEndSeqNum)
			{
				pClass->m_nStep = eFLAT_PROC_MOVE_MEASURE_POS;
			}
			else
			{
				pClass->m_nStep = eFLAT_PROC_END;
			}

			break;
		case eFLAT_PROC_END:

			Wait_(1000);

			// 20220315 mhbaek Add : 전체 평균 값에서 오차값만 출력
			for (int i = 0; i < pClass->m_nCurSeqNum; i++)
			{
				if (9999 > pClass->m_dVctData[i])
				{
					dSeqAvrValue += pClass->m_dVctData[i];
				}
				else
				{
					nErrorCnt++;
				}
			}

			dSeqAvrValue /= (pClass->m_nCurSeqNum - nErrorCnt);

			swprintf_s(tzMesg, 1024, L"[FALTNESS][MEASUREMENT][BASE DATA][%.3f]", dSeqAvrValue);
			LOG_MESG(ENG_EDIC::en_measure_flatness, tzMesg);

			for (int i = 0; i < pClass->m_nCurSeqNum; i++)
			{
				nCol = i % pParam->u8CountOfXLine;
				nRow = i / pParam->u8CountOfXLine;

				// nRow의 값이 홀수인 경우 좌측 방향으로 진행
				if (0 != nRow % 2)
				{
					nCol = pParam->u8CountOfXLine - nCol - 1;
				}

				nCol++;
				nRow++;

				if (9999 > pClass->m_dVctData[i])
				{
					pClass->m_dVctData[i] -= dSeqAvrValue;
					pClass->InputResultGridValue(nRow, nCol, pClass->m_dVctData[i]);
					pClass->SetResultGridCellColor(nRow, nCol, GREEN_);
				}
				else
				{
					pClass->InputResultGridValue(nRow, nCol, pClass->m_dVctData[i]);
					pClass->SetResultGridCellColor(nRow, nCol, TOMATO);
				}
			}

			LOG_MESG(ENG_EDIC::en_measure_flatness, _T("[FALTNESS][END]"));

			pClass->m_bFlatProcThread = FALSE;
			break;

		default:

			pClass->m_nStep = eFLAT_PROC_FAIL;
			break;
		}

		Sleep(100);
	}

	pClass->m_bIngFlatProcThread = FALSE;
	return 0;
}
