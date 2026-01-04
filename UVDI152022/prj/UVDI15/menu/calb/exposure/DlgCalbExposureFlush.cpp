// DlgCalb.cpp: 구현 파일
//

#include "pch.h"
#include "../../../pops/DlgParam.h"
#include "DlgCalbExposureFlush.h"
#include "afxdialogex.h"
#include <iostream>
#include <string>


#include "../../../GlobalVariables.h"

#include <ShellScalingApi.h>
#pragma comment(lib, "Shcore.lib")

class IDScamManager;
/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgCalbExposureFlush::CDlgCalbExposureFlush(UINT32 id, CWnd* parent)
	: CDlgSubTab(id, parent)
{
	m_enDlgID = ENG_CMDI_TAB::en_menu_tab_calb_exposure_flush;

	m_bViewCtrls = FALSE;
	m_bCamLiveState = FALSE;
	m_dLastACamPos = 0;
	m_u8MarkCount = 0x00;
	
	for (int i = 0; i < eCALB_EXPOSURE_FLUSH_STT_MARK_MAX; i++)
	{
		m_pSttResult[i] = NULL;
	}
}

CDlgCalbExposureFlush::~CDlgCalbExposureFlush()
{
	for (int i = 0; i < eCALB_EXPOSURE_FLUSH_STT_MARK_MAX; i++)
	{
		if (NULL != m_pSttResult[i])
		{
			delete m_pSttResult[i];
		}
	}
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgCalbExposureFlush::DoDataExchange(CDataExchange* dx)
{
	CDlgSubTab::DoDataExchange(dx);

	UINT32 i, u32StartID;

	u32StartID = IDC_CALB_EXPOSURE_FLUSH_BTN_PRINT;
	for (i = 0; i < eCALB_EXPOSURE_FLUSH_BTN_MAX; i++)		DDX_Control(dx, u32StartID + i, m_btn_ctl[i]);

	u32StartID = IDC_CALB_EXPOSURE_FLUSH_STT_MARK1;
	for (i = 0; i < eCALB_EXPOSURE_FLUSH_STT_MAX; i++)		DDX_Control(dx, u32StartID + i, m_stt_ctl[i]);

	u32StartID = IDC_CALB_EXPOSURE_FLUSH_TXT_STATE;
	for (i = 0; i < eCALB_EXPOSURE_FLUSH_TXT_MAX; i++)		DDX_Control(dx, u32StartID + i, m_txt_ctl[i]);

	u32StartID = IDC_CALB_EXPOSURE_FLUSH_PIC_MARK1;
	for (i = 0; i < eCALB_EXPOSURE_FLUSH_PIC_MAX; i++)		DDX_Control(dx, u32StartID + i, m_pic_ctl[i]);

	u32StartID = IDC_CALB_EXPOSURE_FLUSH_PGR_RATE;
	for (i = 0; i < eCALB_EXPOSURE_FLUSH_PGR_MAX; i++)		DDX_Control(dx, u32StartID + i, m_pgr_ctl[i]);

	u32StartID = IDC_CALB_EXPOSURE_FLUSH_GRD_PH_STEP_X;
	for (i = 0; i < eCALB_EXPOSURE_FLUSH_GRD_MAX; i++)		DDX_Control(dx, u32StartID + i, m_grd_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgCalbExposureFlush, CDlgSubTab)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CALB_EXPOSURE_FLUSH_BTN_PRINT, IDC_CALB_EXPOSURE_FLUSH_BTN_PRINT + eCALB_EXPOSURE_FLUSH_BTN_MAX, OnBtnClick)
	ON_NOTIFY_RANGE(NM_CLICK, IDC_CALB_EXPOSURE_FLUSH_GRD_PH_STEP_X, IDC_CALB_EXPOSURE_FLUSH_GRD_PH_STEP_Y, OnClickGridStep)
	ON_NOTIFY(NM_CLICK, IDC_CALB_EXPOSURE_FLUSH_GRD_RESULT, &CDlgCalbExposureFlush::OnClickGridResult)
	ON_NOTIFY(NM_CLICK, IDC_CALB_EXPOSURE_FLUSH_GRD_RESULT_BTN, &CDlgCalbExposureFlush::OnClickGridBtn)
	ON_NOTIFY(NM_CLICK, IDC_CALB_EXPOSURE_FLUSH_GRD_OPTION, &CDlgCalbExposureFlush::OnClickGridOption)
	ON_MESSAGE(eMSG_EXPO_FLUSH_RESULT, OnMsgResult)
	ON_MESSAGE(eMSG_EXPO_FLUSH_RESULT_PROGRESS, ResultProgress)
	ON_WM_TIMER()
END_MESSAGE_MAP()


/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgCalbExposureFlush::PreTranslateMessage(MSG* msg)
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
BOOL CDlgCalbExposureFlush::OnInitDlg()
{
	/* 컨트롤 초기화 */
	InitCtrl();

	InitGridPhStepX();
	InitGridPhStepY();
	InitGridResult();
	InitGridResultBtn();
	InitGridOption();
	InitDispMark();

//	RegStatic();

	LoadOption();
	ViewControls(FALSE);

	SetTimer(eCALB_EXPOSURE_FLUSH_TIMER_UPDATE, 200, NULL);

	if (FALSE == CFlushErrorMgr::GetInstance()->SetRegistModel())
	{
		AfxMessageBox(L"Failed to setup the model for search target", MB_ICONSTOP | MB_TOPMOST);
	}

	return TRUE;
}


/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgCalbExposureFlush::OnExitDlg()
{
	KillTimer(eCALB_EXPOSURE_FLUSH_TIMER_UPDATE);

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
VOID CDlgCalbExposureFlush::OnPaintDlg(CDC* dc)
{
	//ENG_VCCM enMode = uvEng_Camera_GetCamMode();
	//LPG_ACGR pstGrabData = CFlushErrorMgr::GetInstance()->GetGrabData();
	//RECT rDraw;

	///* 현재 Calibration Mode인 경우 */
	//if (uvEng_Camera_GetCamMode() == ENG_VCCM::en_cali_mode)
	//{
	//	/* 왼쪽 이미지가 출력될 윈도 영역 */
	//	m_pic_ctl[eCALB_EXPOSURE_FLUSH_PIC_MARK1].GetWindowRect(&rDraw);
	//	ScreenToClient(&rDraw);
	//	uvEng_Camera_DrawMarkDataBitmap(dc->m_hDC, rDraw, &pstGrabData[eCALB_EXPOSURE_FLUSH_PIC_MARK1], 0x01);

	//	/* 오른쪽 이미지가 출력될 윈도 영역 */
	//	m_pic_ctl[eCALB_EXPOSURE_FLUSH_PIC_MARK2].GetWindowRect(&rDraw);
	//	ScreenToClient(&rDraw);
	//	uvEng_Camera_DrawMarkDataBitmap(dc->m_hDC, rDraw, &pstGrabData[eCALB_EXPOSURE_FLUSH_PIC_MARK2], 0x01);
	//}

	/* 자식 윈도 호출 */
	CDlgSubTab::DrawOutLine();
}


/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgCalbExposureFlush::OnResizeDlg()
{
}


/*
 desc : 부모 스레드에 의해 주기적으로 호출됨
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgCalbExposureFlush::UpdatePeriod(UINT64 tick, BOOL is_busy)
{
	UpdateLiveView();
	/* Control Enable or Disable */
	UpdateControl(tick, is_busy);
}


/*
 desc : 컨트롤
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgCalbExposureFlush::UpdateControl(UINT64 tick, BOOL is_busy)
{
}


/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgCalbExposureFlush::InitCtrl()
{
	CResizeUI clsResizeUI;
	CString strTemp;
	CRect rctCtrl;

	for (int i = 0; i < eCALB_EXPOSURE_FLUSH_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
		// by sysandj : Resize UI
	}

	/* static - Normal */
	for (int i = 0; i < eCALB_EXPOSURE_FLUSH_STT_MAX; i++)
	{
		m_stt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_stt_ctl[i].SetDrawBg(1);
		m_stt_ctl[i].SetBaseProp(0, 1, 0, 0, DEF_COLOR_BTN_MENU_NORMAL, 0, WHITE_);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_stt_ctl[i]);
		// by sysandj : Resize UI

		m_stt_ctl[i].GetWindowRect(rctCtrl);
		this->ScreenToClient(rctCtrl);

		rctCtrl.right += 2;

		m_stt_ctl[i].MoveWindow(rctCtrl);
		Invalidate();
	}

	/* static - Normal */
	for (int i = 0; i < eCALB_EXPOSURE_FLUSH_PIC_MAX; i++)
	{
		//m_pic_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		//m_pic_ctl[i].SetDrawBg(1);
		//m_pic_ctl[i].SetBaseProp(0, 1, 0, 0, BLACK_, 0, WHITE_);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_pic_ctl[i]);
		// by sysandj : Resize UI
	}

	/* static - Normal */
	for (int i = 0; i < eCALB_EXPOSURE_FLUSH_TXT_MAX; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_txt_ctl[i]);
		// by sysandj : Resize UI
	}

	for (int i = 0; i < eCALB_EXPOSURE_FLUSH_PGR_MAX; i++)
	{
		m_pgr_ctl[i].SetBkColor(ALICE_BLUE);
		m_pgr_ctl[i].SetTextColor(BLACK_);
		m_pgr_ctl[i].SetRange(0, 100);
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_pgr_ctl[i]);
		// by sysandj : Resize UI
	}

	m_pgr_ctl[eCALB_EXPOSURE_FLUSH_PGR_RATE].SetShowPercent();

	for (int i = 0; i < eCALB_EXPOSURE_FLUSH_GRD_MAX; i++)
	{
		m_grd_ctl[i].SetParent(this);
	}
}

VOID CDlgCalbExposureFlush::RegStatic()
{
	CRect rctWindow;

	for (int i = 0; i < eCALB_EXPOSURE_FLUSH_STT_MARK_MAX; i++)
	{
		m_pic_ctl[i].GetWindowRect(rctWindow);
		this->ScreenToClient(rctWindow);

		//rctWindow.right += 50;

		m_pSttResult[i] = new CMyStatic();
		m_pSttResult[i]->Create(NULL, WS_VISIBLE | SS_NOTIFY | SS_LEFT, rctWindow, this, IDC_CALB_EXPOSURE_FLUSH_STT_MARK1_RESULT + i);
		m_pSttResult[i]->SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_pSttResult[i]->SetTextColor(GREEN_);
	}

	Invalidate();
}

/*
 desc : 이미지 뷰 영역 갱신
 parm : None
 retn : None
*/
VOID CDlgCalbExposureFlush::InvalidateView()
{
	CRect rView[eCALB_EXPOSURE_FLUSH_PIC_MAX];

	for (int i = 0; i < eCALB_EXPOSURE_FLUSH_PIC_MAX; i++)
	{
		m_pic_ctl[i].GetWindowRect(rView[i]);
		ScreenToClient(rView[i]);
	}

	rView[eMARK_1].right = rView[eMARK_2].right;

	InvalidateRect(rView[eMARK_1], TRUE);
}


VOID CDlgCalbExposureFlush::InitGridPhStepX()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	int nPhCnt = uvEng_GetConfig()->luria_svc.ph_count;
	std::vector <int>			vColSize(nPhCnt);
	std::vector <CString>	vTitle[eSTEP_ROW_MAX];
	CString strText;

	vTitle[eSTEP_ROW_TITLE] = { _T("ITEM") };
	vTitle[eSTEP_ROW_PREV] = { _T("Prev") };
	vTitle[eSTEP_ROW_THIS] = { _T("This") };
	vTitle[eSTEP_ROW_SAVE] = { _T("Save") };

	for (int nPhIndex = 2; nPhIndex <= nPhCnt; nPhIndex++)
	{
		strText.Format(_T("PH.%d"), nPhIndex);
		vTitle[eSTEP_ROW_TITLE].push_back(strText);

		for (int nRow = eSTEP_ROW_PREV; nRow < eSTEP_ROW_MAX; nRow++)
		{
			vTitle[nRow].push_back(_T("0.0"));
		}
	}

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_PH_STEP_X];

	clsResizeUI.ResizeControl(this, pGrid);
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	int nHeight = (int)((rGrid.Height() - 1) / eSTEP_ROW_MAX);
	int nTotalWidth = 0;

	for (auto& width : vColSize)
	{
		width = rGrid.Width() / (int)vColSize.size();
		nTotalWidth += width;
	}

	vColSize.front() += rGrid.Width() - nTotalWidth - 1;

	/* 객체 기본 설정 */
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	pGrid->SetRowCount(eSTEP_ROW_MAX);
	pGrid->SetColumnCount((int)vColSize.size());
	pGrid->SetFixedColumnCount(0);
	pGrid->SetBkColor(WHITE_);
	pGrid->SetFixedColumnSelection(0);

	for (int nRow = eSTEP_ROW_TITLE; nRow < eSTEP_ROW_MAX; nRow++)
	{
		if (eSTEP_ROW_TITLE == nRow)	pGrid->SetRowHeight(nRow, nHeight + rGrid.Height() - (nHeight * eSTEP_ROW_MAX) - 1);
		else			pGrid->SetRowHeight(nRow, nHeight);

		for (int nCol = 0; nCol < (int)vColSize.size(); nCol++)
		{
			if (eSTEP_ROW_TITLE == nRow || eSTEP_COL_TITLE == nCol)
			{
				pGrid->SetItemBkColour(nRow, nCol, ALICE_BLUE);
			}

			pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			pGrid->SetColumnWidth(nCol, vColSize[nCol]);
			pGrid->SetItemText(nRow, nCol, vTitle[nRow][nCol]);
			pGrid->SetItemState(nRow, nCol, GVIS_READONLY);
		}
	}

	/* 기본 속성 및 갱신 */
	pGrid->EnableDragAndDrop(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}

VOID CDlgCalbExposureFlush::InitGridPhStepY()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	int nPhCnt = uvEng_GetConfig()->luria_svc.ph_count;
	std::vector <int>			vColSize(nPhCnt);
	std::vector <CString>	vTitle[eSTEP_ROW_MAX];
	CString strText;

	vTitle[eSTEP_ROW_TITLE] = { _T("ITEM") };
	vTitle[eSTEP_ROW_PREV] = { _T("Prev") };
	vTitle[eSTEP_ROW_THIS] = { _T("This") };
	vTitle[eSTEP_ROW_SAVE] = { _T("Save") };

	for (int nPhIndex = 2; nPhIndex <= nPhCnt; nPhIndex++)
	{
		strText.Format(_T("PH.%d"), nPhIndex);
		vTitle[eSTEP_ROW_TITLE].push_back(strText);

		for (int nRow = eSTEP_ROW_PREV; nRow < eSTEP_ROW_MAX; nRow++)
		{
			vTitle[nRow].push_back(_T("0.0"));
		}
	}

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_PH_STEP_Y];

	clsResizeUI.ResizeControl(this, pGrid);
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	int nHeight = (int)((rGrid.Height() - 1) / eSTEP_ROW_MAX);
	int nTotalWidth = 0;

	for (auto& width : vColSize)
	{
		width = rGrid.Width() / (int)vColSize.size();
		nTotalWidth += width;
	}

	vColSize.front() += rGrid.Width() - nTotalWidth - 1;

	/* 객체 기본 설정 */
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	pGrid->SetRowCount(eSTEP_ROW_MAX);
	pGrid->SetColumnCount((int)vColSize.size());
	pGrid->SetFixedColumnCount(0);
	pGrid->SetBkColor(WHITE_);
	pGrid->SetFixedColumnSelection(0);

	for (int nRow = eSTEP_ROW_TITLE; nRow < eSTEP_ROW_MAX; nRow++)
	{
		if (eSTEP_ROW_TITLE == nRow)	pGrid->SetRowHeight(nRow, nHeight + rGrid.Height() - (nHeight * eSTEP_ROW_MAX) - 1);
		else			pGrid->SetRowHeight(nRow, nHeight);

		for (int nCol = 0; nCol < (int)vColSize.size(); nCol++)
		{
			if (eSTEP_ROW_TITLE == nRow || eSTEP_COL_TITLE == nCol)
			{
				pGrid->SetItemBkColour(nRow, nCol, ALICE_BLUE);
			}

			pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			pGrid->SetColumnWidth(nCol, vColSize[nCol]);
			pGrid->SetItemText(nRow, nCol, vTitle[nRow][nCol]);
			pGrid->SetItemState(nRow, nCol, GVIS_READONLY);
		}
	}

	/* 기본 속성 및 갱신 */
	pGrid->EnableDragAndDrop(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}

VOID CDlgCalbExposureFlush::InitGridResult()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	int nPhCnt = uvEng_GetConfig()->luria_svc.ph_count;
	int nColMaxCnt = (nPhCnt * DEF_PH_MEASURE_COUNT) + 1;
	std::vector <CString>	vTitle = { _T("NO"), _T("STEP X"), _T("STEP Y"), _T("LEFT X"), _T("LEFT Y"), _T("RIGHT X"), _T("RIGHT Y") };
	CString strText;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_RESULT];

	clsResizeUI.ResizeControl(this, pGrid);
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	int nHeight = (int)((rGrid.Height() - ::GetSystemMetrics(SM_CYHSCROLL) - 1) / eRESULT_ROW_MAX);
	int nTotalWidth = 0;

	/* 객체 기본 설정 */
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	pGrid->SetGridLineColor(BLACK_);
	pGrid->SetRowCount(eRESULT_ROW_MAX);
	pGrid->SetColumnCount(nColMaxCnt);

	pGrid->SetFixedRowCount(1);
	pGrid->SetFixedColumnCount(1);

	pGrid->SetFixedRowSelection(FALSE);
	pGrid->SetFixedColumnSelection(FALSE);

	for (int nRow = eRESULT_ROW_NO; nRow < eRESULT_ROW_MAX; nRow++)
	{
		pGrid->SetRowHeight(nRow, nHeight);
		pGrid->SetItemText(nRow, 0, vTitle[nRow]);

		for (int nCol = 0; nCol < nColMaxCnt; nCol++)
		{
			/* 타이틀 색상 지정 */
			if (eRESULT_ROW_NO == nRow || eRESULT_COL_TITLE == nCol)
			{
				pGrid->SetItemBkColour(nRow, nCol, ALICE_BLUE);
			}
			else
			{
				pGrid->SetItemBkColour(nRow, nCol, WHITE_);
			}

			/* 항목명 지정 */
			if (eRESULT_ROW_NO == nRow && eRESULT_COL_TITLE < nCol)
			{
				/* 마지막 항목은 결과값을 출력 */
				if (nColMaxCnt - 1 == nCol)
				{
					pGrid->SetItemText(nRow, nCol, _T("RESULT"));
				}
				else
				{
					int nHead = 0;
					int nIndex = 0;

					nHead = (0 == nCol - 1) ? 1 : ((nCol - 1) / DEF_PH_MEASURE_COUNT) + 1;
					nIndex = (0 == nCol - 1) ? 1 : ((nCol - 1) % DEF_PH_MEASURE_COUNT) + 1;

					strText.Format(_T("PH %d - No. %d"), nHead, nIndex);
					pGrid->SetItemText(nRow, nCol, strText);
				}
			}
			else if (eRESULT_ROW_NO < nRow && eRESULT_COL_TITLE < nCol)
			{
				pGrid->SetItemState(nRow, nCol, GVIS_READONLY);
				//230912 mhbaek Modify : 측정 실패 구간 제외 계산 기능 추가
				//pGrid->SetItemText(nRow, nCol, _T("0.0"));

				if (DEF_PH_MEASURE_COUNT == ((nCol - 1) % DEF_PH_MEASURE_COUNT) + 1)
				{
					pGrid->SetItemBkColour(nRow, nCol, PALE_GREEN);
				}
			}

			pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			pGrid->SetColumnWidth(nCol, DEF_DEFAULT_GRID_COL_SIZE);
		}
	}

	/* 기본 속성 및 갱신 */
	pGrid->EnableDragAndDrop(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}

VOID CDlgCalbExposureFlush::InitGridResultBtn()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <int>			vRowSize(3);
	std::vector <std::wstring>	vTitle = { _T("FILE"), _T("RESET"), _T("CALC") };

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_RESULT_BTN];

	clsResizeUI.ResizeControl(this, pGrid);
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	int nHeight = (int)((rGrid.Height() - 1) / (int)vRowSize.size());
	int nTotalHeight = 0;
	int nIndex = 0;

	/* 객체 기본 설정 */
	pGrid->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
	pGrid->SetRowCount((int)vRowSize.size());
	pGrid->SetColumnCount(1);
	pGrid->SetColumnWidth(0, rGrid.Width() - 1);
	pGrid->SetFixedColumnCount(0);
	pGrid->SetBkColor(RGB(255, 255, 255));
	pGrid->SetGridLineColor(BLACK_);
	pGrid->SetFixedColumnSelection(0);

	/* 타이틀 (첫 번째 행) */
	for (int i = 0; i < (int)vRowSize.size(); i++)
	{
		pGrid->SetRowHeight(i, nHeight);
		pGrid->SetItemFormat(i, 0, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pGrid->SetItemText(i, 0, vTitle[i].c_str());
		pGrid->SetItemBkColour(i, 0, DEF_COLOR_BTN_PAGE_NORMAL);
		pGrid->SetItemFgColour(i, 0, WHITE_);
	}

	/* 기본 속성 및 갱신 */
	pGrid->EnableDragAndDrop(FALSE);
	pGrid->SetBaseGridProp(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}

VOID CDlgCalbExposureFlush::InitGridOption()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;

	//std::vector <int>			vRowSize(7);
	std::vector <int>			vRowSize(6);
	std::vector <int>			vColSize(3);
	//std::vector <std::wstring>	vTitle[7];
	std::vector <std::wstring>	vTitle[6];

	vTitle[0] = { _T("Stage X Pos"),		_T("0"),		_T("mm") };
	vTitle[1] = { _T("Stage Y Pos"),		_T("0"),		_T("mm") };
	vTitle[2] = { _T("Start ACam X Pos"),	_T("0"),		_T("mm") };
	vTitle[3] = { _T("Frame Rate"),			_T("0.5"),		_T("[0.1~1.0]") };
	//vTitle[4] = { _T("Scroll Mode"),		_T("1"),		_T("Level") };
	//vTitle[5] = { _T("Grab Width"),			_T("1200"),		_T("Pixel") };
	//vTitle[6] = { _T("Grab Height"),		_T("600"),		_T("Pixel") };
	vTitle[4] = { _T("Expose Round"),		_T("1"),		_T("cnt") };
	vTitle[5] = { _T("Expose Y Pos Plus"),	_T("20"),		_T("mm") };

	std::vector <COLORREF>		vColColor = { ALICE_BLUE, WHITE_, ALICE_BLUE };

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_OPTION];

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
		width = rGrid.Width() / (int)vColSize.size() - 20;
		nTotalWidth += width;
	}

	vColSize.front() += rGrid.Width() - nTotalWidth - 1;

	/* 객체 기본 설정 */
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	pGrid->SetRowCount((int)vRowSize.size());
	pGrid->SetColumnCount((int)vColSize.size());
	pGrid->SetFixedColumnCount(0);
	pGrid->SetBkColor(WHITE_);
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
		}
	}

	/* 기본 속성 및 갱신 */
	pGrid->EnableDragAndDrop(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}


VOID CDlgCalbExposureFlush::InitDispMark() // lk91 cam 3개 적용시 변경해야함...
{
	//CWnd* pWnd[2];
	//CWnd** pWnd = new CWnd*[uvEng_GetConfig()->set_cams.acam_count];
	CWnd** pWnd = new CWnd*[2];
	//for (int i = 0; i < uvEng_GetConfig()->set_cams.acam_count; i++) { 
	for (int i = 0; i < 2; i++) {
		pWnd[i] = GetDlgItem(IDC_CALB_EXPOSURE_FLUSH_PIC_MARK1 + i);
	}
	uvEng_Camera_SetDisp(pWnd, 0x02);
	//DispResize(pWnd[0]); // lk91 현장에서 체크
	//DispResize(pWnd[1]);

	uvEng_Camera_DrawOverlayDC(false, DISP_TYPE_CALB_EXPO, 0);

	//for (int i = 0; i < uvEng_GetConfig()->set_cams.acam_count; i++)
	//for (int i = 0; i < 2; i++)
	//{
	//	delete[] pWnd[i];
	//}
	delete[] pWnd;
}

/* desc: Frame Control을 이미지 사이즈 비율로 재조정 */
void CDlgCalbExposureFlush::DispResize(CWnd* pWnd)
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FLUSH_PIC_MARK1];
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
	if ((DOUBLE(szDraw.cx) / DOUBLE(uvEng_GetConfig()->set_cams.soi_size[0])) > (DOUBLE(szDraw.cy) / DOUBLE(uvEng_GetConfig()->set_cams.soi_size[1])))
	{
		/* <세로> 비율 기준으로 영상 확대 or 축소 진행 */
		tgt_rate = DOUBLE(szDraw.cy) / DOUBLE(uvEng_GetConfig()->set_cams.soi_size[1]);
	}
	/* 세로 크기 비율이 가로 크기 비율보다 큰 경우 */
	else
	{
		/* <가로> 비율 기준으로 영상 확대 or 축소 진행 */
		tgt_rate = DOUBLE(szDraw.cx) / DOUBLE(uvEng_GetConfig()->set_cams.soi_size[0]);
	}
	/* 윈도 이미지 영역에 맞게 영상 이미지 크기 조정 */
	tgtSize.cx = (int)(tgt_rate * uvEng_GetConfig()->set_cams.soi_size[0]);
	tgtSize.cy = (int)(tgt_rate * uvEng_GetConfig()->set_cams.soi_size[1]);

	lpRect.left = ((lpRect.right - lpRect.left) / 2 + lpRect.left) - (tgtSize.cx / 2);
	lpRect.top = ((lpRect.bottom - lpRect.top) / 2 + lpRect.top) - (tgtSize.cy / 2);
	lpRect.right = lpRect.left + tgtSize.cx;
	lpRect.bottom = lpRect.top + tgtSize.cy;

	nHeight = lpRect.Height();

	lpRect.top = rGrid.bottom + 3;
	lpRect.bottom = lpRect.top + nHeight;

	pWnd->MoveWindow(lpRect, TRUE);

	uvEng_Camera_SetCalbStepDispSize(tgtSize);
}

VOID CDlgCalbExposureFlush::ResetGridData()
{
	CGridCtrl* pGrid = nullptr;

	if (TRUE == CFlushErrorMgr::GetInstance()->IsProcessWorking())
	{
		AfxMessageBox(_T("현재 측정중입니다."), MB_ICONSTOP | MB_TOPMOST);
		return;
	}

	m_dLastACamPos = 0.0f;
	m_u8MarkCount = 0;

	for (int nGrd = 0; nGrd < eCALB_EXPOSURE_FLUSH_GRD_MAX; nGrd++)
	{
		if (eCALB_EXPOSURE_FLUSH_GRD_RESULT_BTN <= nGrd)
		{
			continue;
		}

		pGrid = &m_grd_ctl[nGrd];

		for (int nRow = 0; nRow < pGrid->GetRowCount(); nRow++)
		{
			for (int nCol = 0; nCol < pGrid->GetColumnCount(); nCol++)
			{
				if (0 == nRow || 0 == nCol)
				{
					continue;
				}

				if (nGrd == eCALB_EXPOSURE_FLUSH_GRD_RESULT)
				{
					COLORREF colDefault = (0 == nCol % DEF_PH_MEASURE_COUNT) ? PALE_GREEN : WHITE_;
					pGrid->SetItemBkColour(nRow, nCol, colDefault);
				}

				pGrid->SetItemText(nRow, nCol, _T("0.0"));
				pGrid->Refresh();
			}
		}
	}
}

VOID CDlgCalbExposureFlush::SetLiveView(BOOL bLive/* = TRUE*/)
{
	/* Live View */
	if (TRUE == bLive)
	{
		m_btn_ctl[eCALB_EXPOSURE_FLUSH_BTN_LIVE].SetBgColor(LIME);
		m_btn_ctl[eCALB_EXPOSURE_FLUSH_BTN_LIVE].SetTextColor(BLACK_);

		/* Align Camera is Live Mode */
		uvEng_Camera_SetCamMode(ENG_VCCM::en_live_mode);

		uvEng_Mvenc_ReqWirteTrigLive(1, TRUE);

	}
	else
	{
		m_btn_ctl[eCALB_EXPOSURE_FLUSH_BTN_LIVE].SetBgColor(g_clrBtnColor);
		m_btn_ctl[eCALB_EXPOSURE_FLUSH_BTN_LIVE].SetTextColor(WHITE_);

		/* Trigger & Strobe : Disabled */
		if (TRUE == uvEng_Mvenc_ReqEncoderOutReset())
		{
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
					if (GetTickCount64() > (u64Tick + 2000))	break;
				}

				Sleep(10);

			} while (1);

			/* Align Camera is Calibration Mode */
			uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
		}
	}

	Invalidate(FALSE);
}

VOID CDlgCalbExposureFlush::DoPrint()
{
	STG_CELA stExpo = { NULL };
	stExpo.Init();
	stExpo.ready_mode = 1; //단차 모드 변경

	CDlgMain* pMainDlg = (CDlgMain*)::AfxGetMainWnd();
	//m_pDlgMain->RunWorkJob(ENG_BWOK::en_gerb_onlyfem);
	//m_pDlgMain->RunWorkJob(ENG_BWOK::en_expo_only, PUINT64(&stExpo));

	/*시작 위치 변경*/
	DOUBLE pStartX = uvEng_GetConfig()->luria_svc.table_expo_start_xy[0][0];
	DOUBLE pStartY = uvEng_GetConfig()->luria_svc.table_expo_start_xy[0][1];
	double startXoffset, startYoffset;

	startXoffset = pStartX;
	startYoffset = (uvEng_GetConfig()->ph_step.expose_round - 1) * uvEng_GetConfig()->ph_step.y_pos_plus + pStartY;

	if (!uvEng_Luria_ReqSetTableExposureStartPos(0x01, startXoffset, startYoffset))	/* table number is fixed */
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetExposeStartXY)");
	}


	//헤더 단차 변경
	/* 기존 단차 정보 초기화 */
	LPG_LDMC pstMemMach = &uvEng_ShMem_GetLuria()->machine;
	pstMemMach->ResetPhOffset();
	LPG_CLSI pstLuria = &uvEng_GetConfig()->luria_svc;
	INT32 i32OffsetX, i32OffsetY;
	/* 광학계 Offset X / Y 값 설정 (2 번 광학계 부터) */
	for (int i = 1; i < pstLuria->ph_count; i++)
	{
		i32OffsetX = (INT32)(pstLuria->ph_offset_x[i] * 1000000.0f);	/* unit : nm */
		i32OffsetY = (INT32)(pstLuria->ph_offset_y[i] * 1000000.0f);	/* unit : nm */

		/* Photohead의 Offset 값 적용하기 위함 (최초는 Photohead Number가 2 값임) */
		if (!uvEng_Luria_ReqSetSpecPhotoHeadOffset(i + 1, i32OffsetX, i32OffsetY))
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetSpecPhotoHeadOffset)");
		}
	}

	//m_pDlgMain->RunWorkJob(ENG_BWOK::en_expo_only, PUINT64(&stExpo));
	pMainDlg->RunWorkJob(ENG_BWOK::en_expo_only, PUINT64(&stExpo));
}


VOID CDlgCalbExposureFlush::MatchOnce()
{
	UINT8 u8MarkCnt = 1;

	if (TRUE == CFlushErrorMgr::GetInstance()->IsProcessWorking())
	{
		CFlushErrorMgr::GetInstance()->SetStopProcess();

		m_btn_ctl[eCALB_EXPOSURE_FLUSH_BTN_MATCH_ONCE].SetBgColor(g_clrBtnColor);
		m_btn_ctl[eCALB_EXPOSURE_FLUSH_BTN_MATCH_ONCE].SetWindowTextW(_T("MATCH ONCE"));
		return;
	}

	/* Live 종료 */
	if (TRUE == m_bCamLiveState)
	{
		m_bCamLiveState = FALSE;
		SetLiveView(FALSE);
	}

	/* Align Camera is Edge Detection Mode */
	// uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);

	u8MarkCnt = (1 > m_u8MarkCount) ? 1 : m_u8MarkCount;

	CFlushErrorMgr::GetInstance()->SetCurStep(u8MarkCnt);
	CFlushErrorMgr::GetInstance()->SetMaxStep(u8MarkCnt);

	CFlushErrorMgr::GetInstance()->MeasureStart(this->GetSafeHwnd(),-1,nullptr);

	m_btn_ctl[eCALB_EXPOSURE_FLUSH_BTN_MATCH_ONCE].SetBgColor(LIGHT_RED);
	m_btn_ctl[eCALB_EXPOSURE_FLUSH_BTN_MATCH_ONCE].SetWindowTextW(_T("STOP"));
}


VOID CDlgCalbExposureFlush::DoMeasure()
{
	if (TRUE == CFlushErrorMgr::GetInstance()->IsProcessWorking())
	{
		CFlushErrorMgr::GetInstance()->SetStopProcess();

		m_btn_ctl[eCALB_EXPOSURE_FLUSH_BTN_MEASURE].SetBgColor(g_clrBtnColor);
		m_btn_ctl[eCALB_EXPOSURE_FLUSH_BTN_MEASURE].SetWindowTextW(_T("AUTO MEASURE"));
		return;
	}

	ResetGridData();

	/* Live 종료 */
	if (TRUE == m_bCamLiveState)
	{
		m_bCamLiveState = FALSE;
		SetLiveView(FALSE);
	}

	/* Align Camera is Edge Detection Mode */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);

	CFlushErrorMgr::GetInstance()->SetCurStep(1);
	CFlushErrorMgr::GetInstance()->SetMaxStep(uvEng_GetConfig()->luria_svc.ph_count * DEF_PH_MEASURE_COUNT);


	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_OPTION];
	int cnt = pGrid->GetItemTextToInt(eOPTION_ROW_EXPOSE_ROUND, eOPTION_COL_VALUE);
	CFlushErrorMgr::GetInstance()->MeasureStart(this->GetSafeHwnd(), cnt, GlobalVariables::GetInstance()->GetDlgMain());
	m_btn_ctl[eCALB_EXPOSURE_FLUSH_BTN_MEASURE].SetBgColor(LIGHT_RED);
	m_btn_ctl[eCALB_EXPOSURE_FLUSH_BTN_MEASURE].SetWindowTextW(_T("STOP"));
}


VOID CDlgCalbExposureFlush::UpdateMatchVal(int nStep)
{
	LPG_ACGR pstGrabData = CFlushErrorMgr::GetInstance()->GetGrabData();
	DOUBLE dbStepX = 0.0f, dbStepY = 0.0f, dbRotate = -1.0f /* 양수 : 회전 없음. 음수 : 회전 됨 */;
	DOUBLE dbPeriod = uvEng_GetConfig()->ph_step.mark_period;
	DOUBLE dbPixelUm = uvEng_GetConfig()->acam_spec.spec_pixel_um[0];	/* 카메라 1 픽셀 크기 */
	CString strResult;
	PTCHAR pchText;

	/* 광학계 회전 설치 여부에 따라 */
	dbRotate = (1 == uvEng_GetConfig()->luria_svc.ph_rotate) ? -1.0f : 1.0f;

	/* X1 & Y1 / X2 & Y2는 각각 검색된 Mark의 중심 좌표 임 */
	if (0x00 != pstGrabData[eMARK_1].marked)
	{
		strResult.Format(_T(" MARK1 (X:%.4f, Y:%.4f, Dist:%.4f)"),
			pstGrabData[eMARK_1].mark_cent_mm_x, pstGrabData[eMARK_1].mark_cent_mm_y, pstGrabData[eMARK_1].mark_cent_mm_dist);

		pchText = strResult.GetBuffer(strResult.GetLength());

		// Imsi
		//m_pSttResult[eMARK_1]->SetTextToStr(pchText);
		strResult.ReleaseBuffer();

	}
	if (0x00 != pstGrabData[eMARK_2].marked)
	{
		strResult.Format(_T(" MARK2 (X:%.4f, Y:%.4f, Dist:%.4f)"),
			pstGrabData[eMARK_2].mark_cent_mm_x, pstGrabData[eMARK_2].mark_cent_mm_y, pstGrabData[eMARK_2].mark_cent_mm_dist);

		pchText = strResult.GetBuffer(strResult.GetLength());
		
		// Imsi
		//m_pSttResult[eMARK_2]->SetTextToStr(pchText);
		strResult.ReleaseBuffer();
	}

	/* 기본 X / Y 떨어진 거리 구함 (단위: 픽셀) */
	dbStepX = pstGrabData[eMARK_2].mark_cent_px_x - pstGrabData[eMARK_1].mark_cent_px_x;
	dbStepY = pstGrabData[eMARK_2].mark_cent_px_y - pstGrabData[eMARK_1].mark_cent_px_y;
	/* 그냥 밑변의 길이에 1 픽셀 크기를 곱하는 식 */
	dbStepX = abs(dbStepX * (dbPixelUm / 1000.0f));
	/* 실제 측정된 길이 (mm)에서 원래 떨어져야 할 길이 (mark_period: mm)를 빼면 오차 (mm)를 구하고, 여기서 1000.0f를 곱하면 um 길이  */
	dbStepX = dbStepX - dbPeriod;
	dbStepY = dbStepY * (dbPixelUm / 1000.0f) * dbRotate /* 광학계 회전 여부에 따라 */;

	/* Grid 영역 갱신 */
	UpdateMatchGrid(nStep, dbStepX, dbStepY,
		pstGrabData[eMARK_1].mark_cent_mm_x,
		pstGrabData[eMARK_1].mark_cent_mm_y,
		pstGrabData[eMARK_2].mark_cent_mm_x,
		pstGrabData[eMARK_2].mark_cent_mm_y);

	/* Grabbed Image 갱신 */
 	InvalidateView();
}


VOID CDlgCalbExposureFlush::ViewControls(BOOL bView/* = TRUE*/)
{
	CString	strText;

	m_btn_ctl[eCALB_EXPOSURE_FLUSH_BTN_SAVE_OPTION].ShowWindow(bView);
	m_stt_ctl[eCALB_EXPOSURE_FLUSH_STT_OPTION].ShowWindow(bView);
	m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_OPTION].ShowWindow(bView);

	strText = (TRUE == bView) ? _T("HIDE OPTION") : _T("VIEW OPTION");
	m_btn_ctl[eCALB_EXPOSURE_FLUSH_BTN_VIEW_OPTION].SetWindowText(strText);
}

/*
 desc : 일반 버튼 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgCalbExposureFlush::OnBtnClick(UINT32 id)
{
	int nBtnID = id - IDC_CALB_EXPOSURE_FLUSH_BTN_PRINT;

	if (TRUE == CFlushErrorMgr::GetInstance()->IsProcessWorking() &&
		nBtnID != eCALB_EXPOSURE_FLUSH_BTN_MATCH_ONCE &&
		nBtnID != eCALB_EXPOSURE_FLUSH_BTN_MEASURE)
	{
		AfxMessageBox(_T("현재 측정중입니다."), MB_ICONSTOP | MB_TOPMOST);
		return;
	}

	switch (nBtnID)
	{
	case eCALB_EXPOSURE_FLUSH_BTN_PRINT:
		DoPrint();
		break;
	case eCALB_EXPOSURE_FLUSH_BTN_MATCH_ONCE:
		MatchOnce();
		break;
	case eCALB_EXPOSURE_FLUSH_BTN_MEASURE:
		DoMeasure();
		break;
	case eCALB_EXPOSURE_FLUSH_BTN_VIEW_OPTION:
		/* 추후 계정 등급 적용 필요 */
		m_bViewCtrls = !m_bViewCtrls;
		ViewControls(m_bViewCtrls);
		break;
	case eCALB_EXPOSURE_FLUSH_BTN_OPEN_CTRL_PANEL:
		::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), eMSG_MAIN_OPEN_CONSOLE, NULL, NULL);
		break;
	case eCALB_EXPOSURE_FLUSH_BTN_LIVE:
		m_bCamLiveState = !m_bCamLiveState;
		SetLiveView(m_bCamLiveState);
		break;
	case eCALB_EXPOSURE_FLUSH_BTN_SAVE:
		SavePhOffset();
		break;
	case eCALB_EXPOSURE_FLUSH_BTN_MOVE_MEASURE:
		MoveStartPos();
		break;
	case eCALB_EXPOSURE_FLUSH_BTN_MOVE_UNLOAD:
		MoveUnloadPos();
		break;
	case eCALB_EXPOSURE_FLUSH_BTN_SAVE_OPTION:
		SaveOption();
		break;

	default:
		break;
	}
}

void CDlgCalbExposureFlush::OnClickGridStep(UINT ID, NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	int nCommand = ID - IDC_CALB_EXPOSURE_FLUSH_GRD_PH_STEP_X;
	CGridCtrl* pGrid = &m_grd_ctl[nCommand];

	pGrid->SetFocusCell(-1, -1);
}

void CDlgCalbExposureFlush::OnClickGridResult(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_RESULT];
	COLORREF colDefault = WHITE_;

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	if (TRUE == CFlushErrorMgr::GetInstance()->IsProcessWorking())
	{
		return;
	}

	if (0 != m_u8MarkCount)
	{
		colDefault = (0 == m_u8MarkCount % DEF_PH_MEASURE_COUNT) ? PALE_GREEN : WHITE_;

		for (int i = eRESULT_ROW_STEP_X; i < eRESULT_ROW_MAX; i++)
		{
			pGrid->SetItemBkColour(i, m_u8MarkCount, colDefault);
		}

		pGrid->Refresh();
	}

	if (1 > pItem->iColumn || m_u8MarkCount == pItem->iColumn)
	{
		m_u8MarkCount = 0;
		return;
	}

	m_u8MarkCount = pItem->iColumn;

	for (int i = eRESULT_ROW_STEP_X; i < eRESULT_ROW_MAX; i++)
	{
		pGrid->SetItemBkColour(i, m_u8MarkCount, LIGHT_PINK);
	}

	pGrid->Refresh();
}


void CDlgCalbExposureFlush::OnClickGridBtn(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_RESULT_BTN];

	switch (pItem->iRow)
	{
	case eGRD_BTN_ROW_FILE:
		SaveAsExcelFile();
		break;
	case eGRD_BTN_ROW_RESET:
		ResetGridData();
		break;
	case eGRD_BTN_ROW_CALC:
		SetStepCalc();
		break;
	default:
		break;
	}

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);
}


void CDlgCalbExposureFlush::OnClickGridOption(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_OPTION];
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	ENM_DITM enType;
	UINT8 u8DecPts = 0;

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	if (eOPTION_COL_VALUE != pItem->iColumn)
	{
		return;
	}
	else if (TRUE == CFlushErrorMgr::GetInstance()->IsProcessWorking())
	{
		AfxMessageBox(_T("현재 측정중입니다."), MB_ICONSTOP | MB_TOPMOST);
		return;
	}

	CString strOutput;
	CString strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn);
	double	dMin = -9999.9999;
	double	dMax = 9999.9999;

	switch (pItem->iRow)
	{
	case eOPTION_ROW_STAGE_X:
	case eOPTION_ROW_STAGE_Y:
	case eOPTION_ROW_CAM_X:
		enType = ENM_DITM::en_double;
		u8DecPts = 4;
		break;

		//case eOPTION_ROW_FRAME_RATE:
		//	dMin = 0.1;
		//	dMax = 1.0;
		//	enType = ENM_DITM::en_double;
		//	u8DecPts = 1;
		//	break;

		//case eOPTION_ROW_SCROLL_MODE:
		//	dMin = 1;
		//	dMax = 7;
		//	enType = ENM_DITM::en_int16;
		//	break;

		//case eOPTION_ROW_GRAB_WIDTH:
		//case eOPTION_ROW_GRAB_HEIGHT:
		//	dMin = 0;
		//	dMax = 9999;
		//	enType = ENM_DITM::en_int16;
		//	break;

	case eOPTION_ROW_EXPOSE_ROUND:
		dMin = 1;
		dMax = 30;
		enType = ENM_DITM::en_int16;
		u8DecPts = 1;
		break;

	case eOPTION_ROW_STAGE_Y_PLUS:
		dMin = 1;
		dMax = 500;
		enType = ENM_DITM::en_double;

	default:
		return;
	}

	if (PopupKBDN(enType, strInput, strOutput, dMin, dMax, u8DecPts))
	{
		pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);
	}

	pGrid->Refresh();
}


void CDlgCalbExposureFlush::OnTimer(UINT_PTR nIDEvent)
{
	if (eCALB_EXPOSURE_FLUSH_TIMER_UPDATE == nIDEvent)
	{
		UpdateStepXY();
		UpdateWorkProgress();
		UpdatePhOffset();

		if (FALSE == CFlushErrorMgr::GetInstance()->IsProcessWorking())
		{
			if (g_clrBtnColor != m_btn_ctl[eCALB_EXPOSURE_FLUSH_BTN_MATCH_ONCE].GetBgColor())
			{
				m_btn_ctl[eCALB_EXPOSURE_FLUSH_BTN_MATCH_ONCE].SetBgColor(g_clrBtnColor);
				m_btn_ctl[eCALB_EXPOSURE_FLUSH_BTN_MATCH_ONCE].SetWindowTextW(_T("MATCH ONCE"));
			}
			if (g_clrBtnColor != m_btn_ctl[eCALB_EXPOSURE_FLUSH_BTN_MEASURE].GetBgColor())
			{
				m_btn_ctl[eCALB_EXPOSURE_FLUSH_BTN_MEASURE].SetBgColor(g_clrBtnColor);
				m_btn_ctl[eCALB_EXPOSURE_FLUSH_BTN_MEASURE].SetWindowTextW(_T("AUTO MEASURE"));
			}
		}
	}

	CDlgSubTab::OnTimer(nIDEvent);
}

/*
 desc : 그리드 영역에 값 갱신하기
 parm : step_x	- [in]  단차 X 좌표 값 (단위: mm)
		step_y	- [in]  단차 Y 좌표 값 (단위: mm)
		cent_lx	- [in]  The center of the mark on the left X (unit : mm)
		cent_ly	- [in]  The center of the mark on the left Y (unit : mm)
		cent_rx	- [in]  The center of the mark on the right X (unit : mm)
		cent_ry	- [in]  The center of the mark on the right Y (unit : mm)
 retn : None
*/
VOID CDlgCalbExposureFlush::UpdateMatchGrid(int nStep, double dStepX, double dStepY, double dLeftX, double dLeftY,	double dRightX, double dRightY)
{
	DOUBLE dbValue		= 0.0f;
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
	ENG_MMDI enDrvID = ENG_MMDI::en_align_cam1;
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	ENG_MMDI enDrvID = ENG_MMDI::en_stage_x;
#endif
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_RESULT];

	/* 현재 카메라의 절대 위치 값과 이전에 측정한 위치 값 비교해서 다른 경우만 (5 um 수준에서 비교) */
	dbValue	= ROUNDED(m_dLastACamPos, 3) - ROUNDED(uvCmn_MC2_GetDrvAbsPos(enDrvID), 3);
	if (abs(dbValue) < 0.005)
		return;

	/* 그리드 영역 갱신 비활성화 */
	pGrid->SetRedraw(FALSE);

	/* 현재 위치에 단차 Y 값 등록 */
	pGrid->SetItemDouble(eRESULT_ROW_STEP_X, nStep, dStepX * 1000.0f, 1);
	pGrid->SetItemDouble(eRESULT_ROW_STEP_Y, nStep, dStepY * 1000.0f, 1);
	pGrid->SetItemDouble(eRESULT_ROW_LEFT_X, nStep, dLeftX, 4);
	pGrid->SetItemDouble(eRESULT_ROW_LEFT_Y, nStep, dLeftY, 4);
	pGrid->SetItemDouble(eRESULT_ROW_RIGHT_X, nStep, dRightX, 4);
	pGrid->SetItemDouble(eRESULT_ROW_RIGHT_Y, nStep, dRightY, 4);

	/* 단차 X, Y값 등록 */
	if (0 == nStep % DEF_PH_MEASURE_COUNT)
	{
		m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_PH_STEP_X].SetItemDouble(eSTEP_ROW_THIS, (int)(nStep / DEF_PH_MEASURE_COUNT), dStepX * 1000.0f, 4);
		m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_PH_STEP_X].Refresh();

		m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_PH_STEP_Y].SetItemDouble(eSTEP_ROW_THIS, (int)(nStep / DEF_PH_MEASURE_COUNT), dStepY * 1000.0f, 4);
		m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_PH_STEP_Y].Refresh();
	}

	/* 현재 Align Camera 위치 값 저장 */
	m_dLastACamPos = uvCmn_MC2_GetDrvAbsPos(enDrvID);
	
	/* 그리드 영역 갱신 활성화 */
	pGrid->SetRedraw(TRUE);
	pGrid->Invalidate(FALSE);
}

/*
 desc : Step X / Y (Photohead 간) 갱신
 parm : None
 retn : None
*/
VOID CDlgCalbExposureFlush::UpdateStepXY()
{
	CGridCtrl* pGrid = nullptr;

	double dPrev = 0;
	double dThis = 0;
	double dSave = 0;
	BOOL bRefresh = FALSE;

	for (int nGrd = ePOS_X; nGrd < ePOS_2D_MAX; nGrd++)
	{
		pGrid = &m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_PH_STEP_X + nGrd];
		bRefresh = FALSE;

		for (int i = 1; i < uvEng_GetConfig()->luria_svc.ph_count; i++)
		{
			dPrev = pGrid->GetItemTextToFloat(eSTEP_ROW_PREV, i);
			dThis = pGrid->GetItemTextToFloat(eSTEP_ROW_THIS, i);

			dSave = dPrev + dThis;

			/* 기존 값과 동일하면 갱신 하지 않기 */
			if (dSave != pGrid->GetItemTextToFloat(eSTEP_ROW_SAVE, i))
			{
				pGrid->SetItemDouble(eSTEP_ROW_SAVE, i, dSave, 1);
				bRefresh = TRUE;
			}
		}

		if (TRUE == bRefresh)
		{
			pGrid->Refresh();
		}
	}
}

/*
 desc : Photohead Offset 값 갱신
 parm : None
 retn : None
*/
VOID CDlgCalbExposureFlush::UpdatePhOffset()
{
	CGridCtrl* pGrid = nullptr;
	LPG_MCPO pStepXY = uvEng_ShMem_GetLuria()->machine.ph_offset_xy;
	double dStepValue = 0.;
	double dCompareValue = 0.;
	BOOL bRefresh = FALSE;

	for (int nGrd = ePOS_X; nGrd < ePOS_2D_MAX; nGrd++)
	{
		pGrid = &m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_PH_STEP_X + nGrd];
		bRefresh = FALSE;

		for (int nHead = 1; nHead < uvEng_GetConfig()->luria_svc.ph_count; nHead++)
		{
			dCompareValue = (ePOS_X == nGrd) ? ROUNDED(pStepXY[nHead].pos_offset_x / 1000.0f, 1) : ROUNDED(pStepXY[nHead].pos_offset_y / 1000.0f, 1);
			dStepValue = pGrid->GetItemTextToFloat(eSTEP_ROW_PREV, nHead);	/* um */

			/* 기존 값과 동일하면 갱신 하지 않기 */
			if (dStepValue != dCompareValue)
			{
				pGrid->SetItemDouble(eSTEP_ROW_PREV, nHead, dStepValue, 1);
				bRefresh = TRUE;
			}
		}

		if (TRUE == bRefresh)
		{
			pGrid->Refresh();
		}
	}
}


VOID CDlgCalbExposureFlush::LoadOption()
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_OPTION];

	pGrid->SetItemDouble(eOPTION_ROW_STAGE_X, eOPTION_COL_VALUE, uvEng_GetConfig()->ph_step.start_xy[0x00], 4);
	pGrid->SetItemDouble(eOPTION_ROW_STAGE_Y, eOPTION_COL_VALUE, uvEng_GetConfig()->ph_step.start_xy[0x01], 4);
	pGrid->SetItemDouble(eOPTION_ROW_CAM_X, eOPTION_COL_VALUE, uvEng_GetConfig()->ph_step.start_acam_x, 4);
	//	pGrid->SetItemText(eOPTION_ROW_FRAME_RATE, eOPTION_COL_VALUE, uvEng_GetConfig()->ph_step.frame_rate);
   //	pGrid->SetItemText(eOPTION_ROW_SCROLL_MODE, eOPTION_COL_VALUE, uvEng_GetConfig()->ph_step.scroll_mode);

	pGrid->SetItemText(eOPTION_ROW_EXPOSE_ROUND, eOPTION_COL_VALUE, uvEng_GetConfig()->ph_step.expose_round);
	pGrid->SetItemDouble(eOPTION_ROW_STAGE_Y_PLUS, eOPTION_COL_VALUE, uvEng_GetConfig()->ph_step.y_pos_plus);

	//	pGrid->SetItemText(eOPTION_ROW_GRAB_WIDTH, eOPTION_COL_VALUE, uvEng_GetConfig()->set_cams.soi_size[0]);
	//	pGrid->SetItemText(eOPTION_ROW_GRAB_HEIGHT, eOPTION_COL_VALUE, uvEng_GetConfig()->set_cams.soi_size[1]);


	pGrid->Refresh();
}


VOID CDlgCalbExposureFlush::SaveOption()
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_OPTION];

	if (IDNO == AfxMessageBox(L"저장하시겠습니까?", MB_YESNO))
	{
		return;
	}

	uvEng_GetConfig()->ph_step.start_xy[0x00] = pGrid->GetItemTextToFloat(eOPTION_ROW_STAGE_X, eOPTION_COL_VALUE);
	uvEng_GetConfig()->ph_step.start_xy[0x01] = pGrid->GetItemTextToFloat(eOPTION_ROW_STAGE_Y, eOPTION_COL_VALUE);
	uvEng_GetConfig()->ph_step.start_acam_x = pGrid->GetItemTextToFloat(eOPTION_ROW_CAM_X, eOPTION_COL_VALUE);

	uvEng_GetConfig()->ph_step.expose_round = pGrid->GetItemTextToFloat(eOPTION_ROW_EXPOSE_ROUND, eOPTION_COL_VALUE);
	uvEng_GetConfig()->ph_step.y_pos_plus = pGrid->GetItemTextToFloat(eOPTION_ROW_STAGE_Y_PLUS, eOPTION_COL_VALUE);

	// 	uvEng_GetConfig()->ph_step.frame_rate = (UINT16)pGrid->GetItemTextToInt(eOPTION_ROW_FRAME_RATE, eOPTION_COL_VALUE);
	//	uvEng_GetConfig()->ph_step.scroll_mode = (UINT8)pGrid->GetItemTextToInt(eOPTION_ROW_SCROLL_MODE, eOPTION_COL_VALUE);
	//	uvEng_GetConfig()->set_cams.soi_size[0] = (UINT16)pGrid->GetItemTextToInt(eOPTION_ROW_GRAB_WIDTH, eOPTION_COL_VALUE);
	//	uvEng_GetConfig()->set_cams.soi_size[1] = (UINT16)pGrid->GetItemTextToInt(eOPTION_ROW_GRAB_HEIGHT, eOPTION_COL_VALUE);


	uvEng_SaveConfig();
	uvEng_Camera_Reconnect();
}


VOID CDlgCalbExposureFlush::SaveAsExcelFile()
{
	CString strFileName;
	CString lpszExt = _T("csv Files|csv");
	CString strFilter = _T("CSV Files(*.csv)|*.csv|All Files (*.*)|*.*||");
	SYSTEMTIME stTm = { NULL };
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_RESULT];

	/* 등록된 데이터가 없는지 확인 */
	if (pGrid->GetRowCount() < 2)
		return;

	/* 현재 시간 */
	GetLocalTime(&stTm);
	/* 윈도 임시 파일로 저장 후 열기 */
	strFileName.Format(_T("%s\\step\\step_%02u%02u%02u_%02u%02u.csv"),
		g_tzWorkDir, stTm.wYear - 2000, stTm.wMonth, stTm.wDay,
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


/*
 desc : 현재까지 측정된 광학계 단차 (정역 (Stripe) 단차) 값을 가지고, Hysteresis 값 계산)
 parm : None
 retn : None
*/
VOID CDlgCalbExposureFlush::SetStepCalc()
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_RESULT];
	UINT8 u8Cnt[eDIR_MAX];
	UINT8 u8StepSize = uvEng_GetConfig()->ph_step.scroll_mode;
	UINT16 u16FrameRate = 0;
	UINT32 u32PixelSpeed = 0;
	UINT32 u32NewDelayMove = 0;

	int nMaxCount = uvEng_GetConfig()->luria_svc.ph_count * DEF_PH_MEASURE_COUNT;
	int nSign = 1;
	double dAvg[eDIR_MAX] = { NULL };
	double dTotalAvg = 0;
	double dSpeed = 0;

	u16FrameRate = (UINT16)ROUNDED(m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_OPTION].GetItemTextToInt(eOPTION_ROW_FRAME_RATE, eOPTION_COL_VALUE) * 1000.0, 0);

	/*
	if (u16FrameRate < 1 || u8StepSize < 1)
	{
		AfxMessageBox(L"Check the value of FrameRate or StepSize !!!", MB_ICONSTOP | MB_TOPMOST);
		return;
	}
	*/

	/* 노광 속도 구하기 */
	dSpeed = uvCmn_Luria_GetExposeSpeed(u16FrameRate, u8StepSize);
	/* 1 픽셀 찍기 위해 이동하는데 걸리는 시간 구하기 (단위: nsec) */
	u32PixelSpeed = (UINT32)ROUNDED((uvEng_GetConfig()->luria_svc.pixel_size * 1000000 /* msec -> nsec */) / dSpeed, 0);
	/* 컨트롤 갱신 비활성화 */
	pGrid->SetRedraw(FALSE);
	/* 맨 처음 항목이 양수인지 음수인지 여부 확인 */
	nSign = (pGrid->GetItemTextToFloat(eRESULT_ROW_STEP_Y, 1) >= 0.0f) ? -1 : 1;

	for (int i = 1; i < nMaxCount; i++)
	{
		if (0 == i % DEF_PH_MEASURE_COUNT)
		{
			continue;
		}
		//230912 mhbaek Modify : 측정 실패 구간 제외 계산 기능 추가
		// 측정 실패 = 공란이면 계산에서 제외한다.
		else if (TRUE == pGrid->GetItemText(eRESULT_ROW_STEP_Y, i).IsEmpty())
		{
			continue;
		}
		else if (0 != i % 2)	// 홀수
		{
			dAvg[eDIR_FWD] += abs(pGrid->GetItemTextToFloat(eRESULT_ROW_STEP_Y, i));
			u8Cnt[eDIR_FWD]++;
		}
		else					// 짝수
		{
			dAvg[eDIR_BWD] += abs(pGrid->GetItemTextToFloat(eRESULT_ROW_STEP_Y, i));
			u8Cnt[eDIR_BWD]++;
		}
	}

	/* 평균 구하기 */
	dAvg[eDIR_FWD] /= (double)(u8Cnt[eDIR_FWD]);
	dAvg[eDIR_BWD] /= (double)(u8Cnt[eDIR_BWD]);

	/* 첫 번째 항목이 양수인 경우, 두 수 (양수 평균, 음수 평균)의 합에 대한 평균을 절대 값으로 변환 후 음수로 변경 */
	dTotalAvg = ((abs(dAvg[eDIR_FWD]) + abs(dAvg[eDIR_BWD])) / 2.0f) * (double)nSign;

	pGrid->SetItemDouble(eRESULT_ROW_STEP_X, nMaxCount, dTotalAvg, 2);

	/* 새로 측정된 정역 단차 기준으로 Negative Offset 값 구하기 (기존 값과 현재 값 더하기) */

	/* Positive & Negative Delay 값 계산 */
	u32NewDelayMove = (UINT32)ROUNDED((dAvg[2] * 1000000.0f / dSpeed), 0);
	pGrid->SetItemDouble(eRESULT_ROW_STEP_Y, nMaxCount, u32NewDelayMove);

	/* 컨트롤 갱신 활성화 */
	pGrid->SetRedraw(TRUE, TRUE);
}


/*
 desc : 현재 측정된 광학계 간의 단차 값 저장
 parm : None
 retn : None
 note : 알고리즘 - 단차공식 (Hysterisis)_v0.31.xlsx 파일 참조
*/
VOID CDlgCalbExposureFlush::SavePhOffset()
{
	UINT8 u8Cnt[eDIR_MAX] = { 0x00, };
	double dStepY[eDIR_MAX] = { 0, };
	double dFirstValue = 0.;
	double dOffset = 0.;
	double dCalcY = 0;
	double dTotalY = 0.;
	double dSpeed = 0.;
	double dPixelSize = uvEng_GetConfig()->luria_svc.pixel_size;
	STG_OSSD stStep = { NULL };
	std::vector<double> dVctCalcY;

	UINT8 u8StepSize = uvEng_GetConfig()->ph_step.scroll_mode;
	UINT16 u16FrameRate = 0;
	INT16 i16Offset = 0;
	UINT32 u32HysAvgY = 0; /* nsec */

#ifndef _DEBUG
	/* 등록된 거버 파일이 있는지 여부 */
	if (uvCmn_Luria_GetJobCount())
	{
		AfxMessageBox(L"Registered gerber file exists\nPlease remove all the gerber files", MB_ICONSTOP | MB_TOPMOST);
		return;
	}
#endif

 	u16FrameRate = (UINT16)ROUNDED(m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_OPTION].GetItemTextToFloat(eOPTION_ROW_FRAME_RATE, eOPTION_COL_VALUE) * 1000.0, 0);

	if (u16FrameRate < 1 || u8StepSize < 1)
	{
		AfxMessageBox(L"No [Scroll Mode] or [Frame Rate] values are entered", MB_ICONSTOP | MB_TOPMOST);
		return;
	}

	/* 메모리 생성 및 초기화 */
	stStep.Reset();

	/* Frame Rate 및 Scroll 값 얻기 */
	stStep.frame_rate = u16FrameRate;
	stStep.scroll_mode = u8StepSize;

	for (int nHead = 1; nHead < uvEng_GetConfig()->luria_svc.ph_count; nHead++)
	{
		/* 광학계 간의 X (좌/우) 단차 값 얻기 */
		stStep.step_x_nm[nHead] = stStep.step_x_nm[nHead - 1] + (UINT32)ROUNDED(m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_PH_STEP_X].GetItemTextToFloat(eSTEP_ROW_SAVE, nHead) * 1000.0f, 0);
		stStep.step_y_nm[nHead] = stStep.step_y_nm[nHead - 1] + (INT32)ROUNDED(m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_PH_STEP_Y].GetItemTextToFloat(eSTEP_ROW_SAVE, 1) * 1000.0f, 0);
	}

	/* 광학계 별 정/역 스캔 방향 별 오차 및 평균 구하고, 전체 평균 임시 저장 */
	for (int i = 1; i < DEF_PH_MEASURE_COUNT; i++)
	{
		if (0 == i % DEF_PH_MEASURE_COUNT)
		{
			continue;
		}
		//230912 mhbaek Modify : 측정 실패 구간 제외 계산 기능 추가
		// 측정 실패 = 공란이면 계산에서 제외한다.
		else if (TRUE == m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_RESULT].GetItemText(eRESULT_ROW_LEFT_Y, i).IsEmpty())
		{
			continue;
		}
		else if (0 != i % 2)	// 홀수
		{
			dStepY[eDIR_FWD] += abs(m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_RESULT].GetItemTextToFloat(eRESULT_ROW_LEFT_Y, i));
			u8Cnt[eDIR_FWD]++;
		}
		else					// 짝수
		{
			dStepY[eDIR_BWD] += abs(m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_RESULT].GetItemTextToFloat(eRESULT_ROW_LEFT_Y, i));
			u8Cnt[eDIR_BWD]++;
		}
	}

	/* 광학계 내에서 평균의 오차 값 */
	dTotalY = ((dStepY[eDIR_BWD] / (double)u8Cnt[eDIR_BWD]) - (dStepY[eDIR_FWD] / (double)u8Cnt[eDIR_FWD])) / 2.0f;

	/* 1차 계산식에 의한 Negative Offset 값 구하기 */
	dFirstValue = m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_RESULT].GetItemTextToFloat(eRESULT_ROW_LEFT_Y, 1);

	/* 소숫점 이하는 무조건 버리기 위해 정수 Casting 연산 */
	i16Offset = (dFirstValue >= 0.0f) ? (INT16)(dTotalY / dPixelSize) : 1 + (INT16)(dTotalY / dPixelSize);

	/* 중간 픽셀 크기 구하기 */
	dOffset = abs(i16Offset) * dPixelSize;

	/* 포토헤드마다 만약 정/역 횟수가 홀수인 경우, 마지막 한번은 계산 안함 */
	/* 만약 현재 j 값이 2 의 배수 값이면, 이전 값하고 빼서, 총 합에 더하기 */
	for (int i = 1; i < DEF_PH_MEASURE_COUNT; i++)
	{
		/* Scan 횟수가 홀수인 경우, 각 광학계의 마지막 값은 처리하지 않음 */
		if (0 == i % DEF_PH_MEASURE_COUNT && 0 != DEF_PH_MEASURE_COUNT % 2)
		{
			continue;
		}
		else if (0 != i % 2)	// 홀수
		{
			if (0 < dFirstValue)		dCalcY = m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_RESULT].GetItemTextToFloat(eRESULT_ROW_LEFT_Y, i) + dOffset;
			else						dCalcY = m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_RESULT].GetItemTextToFloat(eRESULT_ROW_LEFT_Y, i) - dOffset;
		}
		else					// 짝수
		{
			if (0 >= dFirstValue)		dCalcY = m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_RESULT].GetItemTextToFloat(eRESULT_ROW_LEFT_Y, i) + dOffset;
			else						dCalcY = m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_RESULT].GetItemTextToFloat(eRESULT_ROW_LEFT_Y, i) - dOffset;
		}

		dVctCalcY.push_back(dCalcY);
	}

	/* 중간 계산된 값 평균 구하기 */
	for (int i = 0; i < eDIR_MAX; i++)
	{
		dStepY[i] = 0;
	}

	dStepY[eDIR_BWD] += dVctCalcY[0];
	for (int i = 1; i < dVctCalcY.size(); i++)
	{
		(0 != i % 2) ? dStepY[eDIR_FWD] += dVctCalcY[i] : dStepY[eDIR_BWD] += dVctCalcY[i];
	}

	dTotalY = -1.0f * (dStepY[eDIR_BWD] / (dVctCalcY.size() * 0.5)) - (dStepY[eDIR_FWD] / (dVctCalcY.size() * 0.5)) / 2.0f;

	/* 노광 속도 계산 (최대 속도 * Frame Rate) (단위: mm/sec) */
	dSpeed = uvCmn_Luria_GetExposeMaxSpeed(1/*(UINT8)m_edt_int[3].GetTextToNum()*/) * m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_OPTION].GetItemTextToFloat(eOPTION_ROW_FRAME_RATE, eOPTION_COL_VALUE);

	/* 최종 Negative로 이동해야 할 픽셀 크기 값 (기존 픽셀 값에 더 함) */
	stStep.nega_offset_px = i16Offset + uvEng_ShMem_GetLuria()->machine.hysteresis_type1.negative_offset;


	/* Hysterisis Negative Delay 값 구하기 (nsec 구하기 위해 1000000.0f 곱하기) */
	u32HysAvgY = (UINT32)ROUNDED((dTotalY / (dSpeed / 1000.0f)) * 1000.0f, 0);
	stStep.delay_posi_nsec = u32HysAvgY / 2;
	stStep.delay_nega_nsec = u32HysAvgY - stStep.delay_posi_nsec;

	/* 강제로 등록 작업 수행 */
	if (TRUE == OpenStepPopup(&stStep))
	{
		SaveStepData(stStep);
	}
}


VOID CDlgCalbExposureFlush::MoveStartPos()
{
	DOUBLE dbDist = 0.0f, dbVelo = 0.0f;

	if (IDNO == AfxMessageBox(L"측정 위치로 이동하시겠습니까?", MB_YESNO))
	{
		return;
	}

	/* Trigger Board의 Trigger Enable 처리 */
	// uvEng_Trig_ReqTriggerStrobe(TRUE);

	/* 카메라가 2 개인 (외부 타입) 경우 */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)uvEng_GetConfig()->set_comn.align_camera_kind)
	{
		/* Camera 2번 이동 */
		dbVelo = uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_align_cam2];
		dbDist = uvEng_GetConfig()->mc2_svc.max_dist[(UINT8)ENG_MMDI::en_align_cam2];
		uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2, dbDist, dbVelo);
		/* Camera 1번 이동 */
		dbVelo = uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_align_cam1];
		uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, uvEng_GetConfig()->ph_step.start_acam_x, dbVelo);
	}

	/* Stage X 이동 */
	if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_x, uvEng_GetConfig()->ph_step.start_xy[0x00]))
	{
		return;
	}
	dbVelo = uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_x];
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, uvEng_GetConfig()->ph_step.start_xy[0x00], dbVelo);

	/* Stage Y 이동 */
	DOUBLE StepY;
	UINT8 expose_round = uvEng_GetConfig()->ph_step.expose_round;
	DOUBLE	y_pos_plus = uvEng_GetConfig()->ph_step.y_pos_plus;
	DOUBLE				start_x = uvEng_GetConfig()->ph_step.start_xy[0x00];
	DOUBLE				start_y = uvEng_GetConfig()->ph_step.start_xy[0x01];
	StepY = (uvEng_GetConfig()->ph_step.expose_round-1) * uvEng_GetConfig()->ph_step.y_pos_plus;
	if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_y, uvEng_GetConfig()->ph_step.start_xy[0x01] + StepY))
	{
		return;
	}
	dbVelo = uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_y];
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, uvEng_GetConfig()->ph_step.start_xy[0x01] + StepY, dbVelo);
}


VOID CDlgCalbExposureFlush::MoveUnloadPos()
{
	if (IDNO == AfxMessageBox(L"배출 위치로 이동하시겠습니까?", MB_YESNO))
	{
		return;
	}

	/* 모션 (스테이지) 이동 */
	if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_x, uvEng_GetConfig()->set_align.table_unloader_xy[0][0]))
	{
		return;
	}
	if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_y, uvEng_GetConfig()->set_align.table_unloader_xy[0][1]))
	{
		return;
	}
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, uvEng_GetConfig()->set_align.table_unloader_xy[0][0], uvEng_GetConfig()->mc2_svc.move_velo);
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, uvEng_GetConfig()->set_align.table_unloader_xy[0][1], uvEng_GetConfig()->mc2_svc.move_velo);
}


BOOL CDlgCalbExposureFlush::OpenStepPopup(LPG_OSSD pstStep)
{
	CDlgParam dlg;
	DLG_PARAM stParam;
	VCT_DLG_PARAM stVctParam;

	stParam.Init();
	stParam.strName = _T("Scroll Mode");
	stParam.strValue.Format(_T("%d"), pstStep->scroll_mode);
	stParam.strUnit = _T("[1~7]");
	stParam.enFormat = ENM_DITM::en_uint8;
	stParam.dMin = 1;
	stParam.dMax = 7;
	stVctParam.push_back(stParam);

	stParam.Init();
	stParam.strName = _T("Negative Pixel Offset");
	stParam.strValue.Format(_T("%d"), pstStep->nega_offset_px);
	stParam.strUnit = _T("Pixel");
	stParam.enFormat = ENM_DITM::en_int32;
	stVctParam.push_back(stParam);

	stParam.Init();
	stParam.strName = _T("Frame Rate");
	stParam.strValue.Format(_T("%d"), pstStep->frame_rate);
	stParam.strUnit = _T("Pixel");
	stParam.enFormat = ENM_DITM::en_uint16;
	stVctParam.push_back(stParam);

 	stParam.Init();
 	stParam.strName = _T("Delay in positive moving dir");
 	stParam.strValue.Format(_T("%d"), pstStep->delay_posi_nsec);
 	stParam.strUnit = _T("nSec");
 	stParam.enFormat = ENM_DITM::en_uint32;
 	stVctParam.push_back(stParam);
 
 	stParam.Init();
 	stParam.strName = _T("Delay in negative moving dir");
 	stParam.strValue.Format(_T("%d"), pstStep->delay_nega_nsec);
 	stParam.strUnit = _T("nSec");
 	stParam.enFormat = ENM_DITM::en_uint32;
 	stVctParam.push_back(stParam);

	for (int i = 2; i <= uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		stParam.Init();
		stParam.strName.Format(_T("PH%d Step X"), i);
		stParam.strValue.Format(_T("%d"), pstStep->step_x_nm[i - 1]);
		stParam.strUnit = _T("nm");
		stParam.enFormat = ENM_DITM::en_uint32;
		stVctParam.push_back(stParam);

		stParam.Init();
		stParam.strName.Format(_T("PH%d Step Y"), i);
		stParam.strValue.Format(_T("%d"), pstStep->step_y_nm[i - 1]);
		stParam.strUnit = _T("nm");
		stParam.enFormat = ENM_DITM::en_uint32;
		stVctParam.push_back(stParam);
	}

	if (IDOK != dlg.MyDoModal(stVctParam))
	{
		return FALSE;
	}

	pstStep->scroll_mode = _ttoi(stVctParam[0].strValue);
	pstStep->nega_offset_px = _ttoi(stVctParam[1].strValue);
	pstStep->frame_rate = _ttoi(stVctParam[2].strValue);
	pstStep->delay_posi_nsec = _ttoi(stVctParam[3].strValue);
	pstStep->delay_nega_nsec = _ttoi(stVctParam[4].strValue);

	//for (int i = 0; i < uvEng_GetConfig()->luria_svc.ph_count; i++)
	for (int i = 0; i < uvEng_GetConfig()->luria_svc.ph_count-1; i++)
	{
		pstStep->step_x_nm[i] = _ttoi(stVctParam[5 + (i * 2)].strValue);
		pstStep->step_y_nm[i] = _ttoi(stVctParam[6 + (i * 2)].strValue);
	}

	return TRUE;
}


VOID CDlgCalbExposureFlush::SaveStepData(STG_OSSD stStep)
{
	/* 값 유효성 확인 */
	if (abs(INT32(stStep.delay_posi_nsec) - INT32(stStep.delay_nega_nsec)) > 1)
	{
		AfxMessageBox(L"The value entered is not valid", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* 환경 파일에 광학계 단차 값 저장 */
	DOUBLE ph_offset_x;
	DOUBLE ph_offset_y;
	for (int i = 1; i < uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		ph_offset_x = uvEng_GetConfig()->luria_svc.ph_offset_x[i];
		ph_offset_y = uvEng_GetConfig()->luria_svc.ph_offset_y[i];
		//uvEng_GetConfig()->luria_svc.ph_offset_x[i]	+= (INT8)stStep.step_x_nm[i] / 1000000.0f;
		//uvEng_GetConfig()->luria_svc.ph_offset_y[i]	+= stStep.step_y_nm[i] / 1000000.0f;
		uvEng_GetConfig()->luria_svc.ph_offset_x[i] = ph_offset_x + (stStep.step_x_nm[i] / 1000000.0f);
		uvEng_GetConfig()->luria_svc.ph_offset_y[i] = ph_offset_y - (stStep.step_y_nm[i] / 1000000.0f);
	}

	uvEng_GetConfig()->luria_svc.hys_type_1_scroll_mode		= stStep.scroll_mode;
	uvEng_GetConfig()->luria_svc.hys_type_1_negative_offset	= stStep.nega_offset_px;
 	uvEng_GetConfig()->luria_svc.hys_type_1_delay_offset[eDIR_FWD] = stStep.delay_posi_nsec / 1000000.0f;
 	uvEng_GetConfig()->luria_svc.hys_type_1_delay_offset[eDIR_BWD] = stStep.delay_nega_nsec / 1000000.0f;

	/* 환경 파일 저장 */
	uvEng_SaveConfig();

	/* 기존에 존재하는 값이면 기존 데이터 삭제 진행 */
	if (uvEng_PhStep_RecipeFind(stStep.frame_rate))
	{
		uvEng_PhStep_RecipeDelete(stStep.frame_rate);
	}

	/* 새로 갱신된 레시피 추가 */
	if (!uvEng_PhStep_RecipeAppend(&stStep))
	{
		AfxMessageBox(L"Failed to save the offset value for PH", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* 파일로 저장 */
	uvEng_PhStep_RecipeSave();
}

BOOL CDlgCalbExposureFlush::PopupKBDN(ENM_DITM enType, CString strInput, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts/* = 0*/)
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


LRESULT CDlgCalbExposureFlush::OnMsgResult(WPARAM wParam, LPARAM lParam)
{
	if (TRUE == (BOOL)wParam)
	{
		UpdateMatchVal((int)lParam);
	}

	return 0;
}

LRESULT CDlgCalbExposureFlush::ResultProgress(WPARAM wParam, LPARAM lParam)
{
	CString strText = *(CString*)lParam;

	m_pgr_ctl[eCALB_EXPOSURE_FLUSH_PGR_RATE].SetPos((int)wParam);
	m_txt_ctl[eCALB_EXPOSURE_FLUSH_TXT_STATE].SetTextToStr((PTCHAR)(LPCTSTR)strText);
	Invalidate();

	return 0;
}

/*
 desc : Updates the live image
 parm : None
 retn : None
*/
VOID CDlgCalbExposureFlush::UpdateLiveView()
{
	if (!uvEng_Camera_IsCamModeLive())	return;
	uvEng_Camera_DrawImageBitmap(DISP_TYPE_CALB_EXPO, 0, 1);
}

VOID CDlgCalbExposureFlush::UpdateWorkProgress()
{
	CDlgMain* pMainDlg = (CDlgMain*)::AfxGetMainWnd();

	// 작업중일 때
	if (TRUE == pMainDlg->IsBusyWorkJob())
	{
		// 그 작업이 expo only 이라면
		if (ENG_BWOK::en_expo_only == pMainDlg->GetWorkJobID())
		{
			m_pgr_ctl[eCALB_EXPOSURE_FLUSH_PGR_RATE].SetPos((int)uvEng_GetWorkStepRate());
			m_txt_ctl[eCALB_EXPOSURE_FLUSH_TXT_STATE].SetTextToStr(uvEng_GetWorkStepName());
		}
	}
}




///////////////////////////////////////////////////////////////
//
//
// 
// 
// 
// 
// 
//				시간없으니 합승좀 하겠습니다.
//
// 
// 
// 
// 
//
//
///////////////////////////////////////////////////////////////


CDlgCalbExposureMirrorTune::CDlgCalbExposureMirrorTune(UINT32 id, CWnd * parent)
	: CDlgSubTab(id, parent)
{
	m_enDlgID = ENG_CMDI_TAB::en_menu_tab_calb_exposure_mirrortune;
}

CDlgCalbExposureMirrorTune::~CDlgCalbExposureMirrorTune()
{

}

VOID CDlgCalbExposureMirrorTune::DoDataExchange(CDataExchange* dx)
{
	

	UINT32 i, u32StartID;

	u32StartID = IDC_BUTTON_MIRROR_LED_POWERSET;
	for (i = 0; i < btnMax; i++)		DDX_Control(dx, u32StartID + i, btns[i]);

	u32StartID = IDC_CHECK_MIRROR_LED1;
	for (i = 0; i < chkMax; i++)		DDX_Control(dx, u32StartID + i, checks[i]);

	u32StartID = IDC_COMBO_MIRROR_IMAGES;
	for (i = 0; i < cmbMax; i++)		DDX_Control(dx, u32StartID + i, combos[i]);

	u32StartID = IDC_STATIC_MIRROR_POWER_CURRENT;
	for (i = 0; i < stcMax; i++)		DDX_Control(dx, u32StartID + i, statics[i]);

	u32StartID = IDC_MIRROR_MOTION_LIST;
	for (i = 0; i < lstMax; i++)		DDX_Control(dx, u32StartID + i, lists[i]);

	u32StartID = IDC_MIRROR_MOTION_EDIT;
	for (i = 0; i < edtMax; i++)		DDX_Control(dx, u32StartID + i, edits[i]);

	u32StartID = IDC_IDSCAMERA_VIEW;
	for (i = 0; i < picMax; i++)		DDX_Control(dx, u32StartID + i, pics[i]);

	u32StartID = IDC_SLIDER_MIRROR_POWERINDEX;
	for (i = 0; i < sldMax; i++)		DDX_Control(dx, u32StartID + i, sliders[i]);

	u32StartID = IDC_GROUP_PH_SETTING;
	for (i = 0; i < grpMax; i++)		DDX_Control(dx, u32StartID + i, groups[i]);
	CDlgSubTab::DoDataExchange(dx);
}

BEGIN_MESSAGE_MAP(CDlgCalbExposureMirrorTune, CDlgSubTab)
	// 버튼(여러개 한 함수로)
	ON_CONTROL_RANGE(BN_CLICKED,
		IDC_BUTTON_MIRROR_LED_POWERSET,
		IDC_BUTTON_MIRROR_RUN_MOTION,
		&CDlgCalbExposureMirrorTune::OnMirrorBtnClick)

	// 체크박스(체크 변경도 BN_CLICKED로 받음)
	ON_CONTROL_RANGE(BN_CLICKED,
		IDC_CHECK_MIRROR_LED1,
		IDC_CHECK_MIRROR_KEEP_REFRESHING,
		&CDlgCalbExposureMirrorTune::OnMirrorCheckClick)

	// 콤보박스: 선택 변경
	ON_CBN_SELCHANGE(IDC_COMBO_MIRROR_IMAGES, &CDlgCalbExposureMirrorTune::OnSelChangeMirrorImages)
	ON_CBN_SELCHANGE(IDC_COMBO_MIRROR_PHINDEX, &CDlgCalbExposureMirrorTune::OnSelChangeMirrorPhIndex)

	// 리스트박스: 선택 변경
	ON_LBN_SELCHANGE(IDC_MIRROR_MOTION_LIST, &CDlgCalbExposureMirrorTune::OnSelChangeMirrorMotionList)

	// 슬라이더: 값 변경(드래그 중/놓을 때 둘 다)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_MIRROR_POWERINDEX, &CDlgCalbExposureMirrorTune::OnSliderMirrorPowerChanging) // (있는 경우)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_MIRROR_POWERINDEX, &CDlgCalbExposureMirrorTune::OnSliderMirrorPowerDraw)     // (드래그 중에도 많이 옴)
	ON_WM_HSCROLL()
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()



/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgCalbExposureMirrorTune::PreTranslateMessage(MSG* msg)
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
BOOL CDlgCalbExposureMirrorTune::OnInitDlg()
{
	/* 컨트롤 초기화 */
	InitCtrl();
	SetTimer(10302, 200, NULL);
	return TRUE;
}

VOID CDlgCalbExposureMirrorTune::OnExitDlg()
{
	KillTimer(10302);
}

VOID CDlgCalbExposureMirrorTune::OnPaintDlg(CDC* dc)
{
	CDlgSubTab::DrawOutLine();
}

VOID CDlgCalbExposureMirrorTune::OnResizeDlg()
{
}

VOID CDlgCalbExposureMirrorTune::UpdatePeriod(UINT64 tick, BOOL is_busy)
{
	UpdateControl(tick, is_busy);
}

VOID CDlgCalbExposureMirrorTune::UpdateControl(UINT64 tick, BOOL is_busy)
{
}




HBRUSH CDlgCalbExposureMirrorTune::OnCtlColor(
	CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDlgSubTab::OnCtlColor(pDC, pWnd, nCtlColor);

	const UINT id = pWnd->GetDlgCtrlID();

	switch (nCtlColor)
	{

	

	case CTLCOLOR_LISTBOX:
		if (id == IDC_MIRROR_MOTION_LIST)
		{
			pDC->SetTextColor(RGB(30, 30, 30));
			pDC->SetBkColor(RGB(220, 220, 220));
			return (HBRUSH)brListBg.GetSafeHandle();
		}
		break;

	case CTLCOLOR_EDIT:
		if (id == IDC_MIRROR_MOTION_EDIT)
		{
			pDC->SetTextColor(RGB(30, 30, 30));
			pDC->SetBkColor(RGB(220, 220, 220));
			return (HBRUSH)brEditBg.GetSafeHandle();
		}
		break;
	}

	if (!pWnd) return hbr;
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgCalbExposureMirrorTune::InitCtrl()
{
	CResizeUI clsResizeUI;
	CString strTemp;
	CRect rctCtrl;

	SetRedraw(FALSE);
	SetRedraw(TRUE);
	Invalidate(FALSE);

	if (brListBg.GetSafeHandle()) brListBg.DeleteObject();
	brListBg.CreateSolidBrush(RGB(180, 180, 180));

	if (brEditBg.GetSafeHandle()) brEditBg.DeleteObject();
	brEditBg.CreateSolidBrush(RGB(220, 220, 220));

	if (brPicBg.GetSafeHandle()) brPicBg.DeleteObject();
	brPicBg.CreateSolidBrush(RGB(0, 0, 0)); 

	for (int i = 0; i < grpMax; i++)
	{
		clsResizeUI.ResizeControl(this, &groups[i]);
	}


	for (int i = 0; i < btnMax; i++)
	{
		
		btns[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		btns[i].SetBgColor(g_clrBtnColor);
		btns[i].SetTextColor(g_clrBtnTextColor);
		clsResizeUI.ResizeControl(this, &btns[i]);
	}

	for (int i = 0; i < cmbMax; i++)
	{
		clsResizeUI.ResizeControl(this, &combos[i]);
	}

	for (int i = 0; i < lstMax; i++)
	{
		clsResizeUI.ResizeControl(this, &lists[i]);
	}
	
	for (int i = 0; i < chkMax; i++)
	{
		checks[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		
		clsResizeUI.ResizeControl(this, &checks[i]);
	}



	for (int i = 0; i < stcMax; i++)
	{
		clsResizeUI.ResizeControl(this, &statics[i]);	
	}




	for (int i = 0; i < sldMax; i++)
	{
		clsResizeUI.ResizeControl(this, &sliders[i]);
	}

	for (int i = 0; i < picMax; i++)
	{
		clsResizeUI.ResizeControl(this, &pics[i]);
		
	}

	FixControlToPhysicalPixels(IDC_IDSCAMERA_VIEW, 512, 512);

	for (int i = 0; i < edtMax; i++)
	{

		clsResizeUI.ResizeControl(this, &edits[i]);	
	}
}

/*
 desc : 이미지 뷰 영역 갱신
 parm : None
 retn : None
*/
VOID CDlgCalbExposureMirrorTune::InvalidateView()
{
	CRect rView;
	pics[0].GetWindowRect(rView);
	if (brListBg.GetSafeHandle()) brListBg.DeleteObject();
	if (brEditBg.GetSafeHandle()) brEditBg.DeleteObject();
	if (brPicBg.GetSafeHandle()) brPicBg.DeleteObject();
	ScreenToClient(rView);
	InvalidateRect(rView, TRUE);
}
             

/*
 desc : 일반 버튼 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgCalbExposureMirrorTune::OnMirrorBtnClick(UINT32 id)
{
	int nBtnID = id - IDC_BUTTON_MIRROR_LED_POWERSET;

	switch (nBtnID)
	{
		case LED_POWERSET:
		case LOADIMAGE:
		case UNLOADIMAGE:
		case OPEN:
		{
				//aoi설정
				//Start X = 1664
				//Start Y = 1118
				//Width = 512
				//Height = 512

			if (!GlobalVariables::GetInstance()->GetIDSManager().Connect(0, IDScamManager::Aoi(1664,1118,512,512))) //NEW IDS SETTING.INI에서 가져옴
				MessageBoxW(L"IDS camera connect failed", L"error", MB_OK);
			GlobalVariables::GetInstance()->GetIDSManager().SetPixelClockMHz(7);
			GlobalVariables::GetInstance()->GetIDSManager().SetFrameRate(0.68);
			GlobalVariables::GetInstance()->GetIDSManager().SetExposureUs(1464);

		}
		break;
		case CLOSE:
		{
			GlobalVariables::GetInstance()->GetIDSManager().Disconnect();
		}
		break;
		case SNAPSHOT:
		{
			auto& ids = GlobalVariables::GetInstance()->GetIDSManager();

			if (!ids.IsConnected())
			{
				MessageBoxW(L"IDS camera is not connected", L"error", MB_OK);
				return;
			}

			
			if (!ids.GrabOnce())
			{
				MessageBoxW(L"GrabOnce failed", L"error", MB_OK);
				return;
			}

			
			std::vector<uint8_t> buf;
			if (!ids.SnapshotCopyTo(buf))
			{
				MessageBoxW(L"SnapshotCopyTo failed", L"error", MB_OK);
				return;
			}

			
			const uint8_t* ptr = nullptr;
			int w = 0, h = 0, bpp = 0, pitch = 0;
			ids.GetFramePtr(ptr, w, h, bpp, pitch);

			
			if (bpp != 8 || w <= 0 || h <= 0)
			{
				MessageBoxW(L"Unexpected format (not MONO8)", L"error", MB_OK);
				return;
			}

			idsSnapshot.swap(buf);
			idsW = w;
			idsH = h;

			
			UpdateIDSImage();


		}
		break;

		case RECORD:
		case OPEN_MOTION_CONTROL:
		case EDIT_MOTIONLIST:
		case RUN_MOTION:
		{
			int debug = 0;
		}
		break;
		default:
		break;
	}
}


void CDlgCalbExposureMirrorTune::OnMirrorCheckClick(UINT nID)
{
	const BOOL bChecked = (IsDlgButtonChecked(nID) == BST_CHECKED);

	switch (nID)
	{
	case IDC_CHECK_MIRROR_LED1:
		// TODO: LED1 ON/OFF (bChecked)
		break;

	case IDC_CHECK_MIRROR_LED2:
		// TODO: LED2 ON/OFF (bChecked)
		break;

	case IDC_CHECK_MIRROR_LED3:
		// TODO: LED3 ON/OFF (bChecked)
		break;

	case IDC_CHECK_MIRROR_LED4:
		// TODO: LED4 ON/OFF (bChecked)
		break;

	case IDC_CHECK_MIRROR_KEEP_REFRESHING:
		// TODO: Keep Refreshing (bChecked)
		break;

	default:
		break;
	}
}

void CDlgCalbExposureMirrorTune::OnSelChangeMirrorImages()
{
	CComboBox* pCb = (CComboBox*)GetDlgItem(IDC_COMBO_MIRROR_IMAGES);
	if (!pCb) return;

	const int sel = pCb->GetCurSel();
	if (sel == CB_ERR) return;

	CString text;
	pCb->GetLBText(sel, text);

	// TODO: text/sel 사용
}

void CDlgCalbExposureMirrorTune::OnSelChangeMirrorPhIndex()
{
	CComboBox* pCb = (CComboBox*)GetDlgItem(IDC_COMBO_MIRROR_PHINDEX);
	if (!pCb) return;

	const int sel = pCb->GetCurSel();
	if (sel == CB_ERR) return;

	CString text;
	pCb->GetLBText(sel, text);

	// TODO
}

void CDlgCalbExposureMirrorTune::OnSelChangeMirrorMotionList()
{
	CListBox* pLb = (CListBox*)GetDlgItem(IDC_MIRROR_MOTION_LIST);
	if (!pLb) return;

	const int sel = pLb->GetCurSel();
	if (sel == LB_ERR) return;

	CString text;
	pLb->GetText(sel, text);

	// TODO
}

void CDlgCalbExposureMirrorTune::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	
	if (pScrollBar && pScrollBar->GetSafeHwnd() == GetDlgItem(IDC_SLIDER_MIRROR_POWERINDEX)->GetSafeHwnd())
	{
		CSliderCtrl* pSl = (CSliderCtrl*)pScrollBar;
		const int pos = pSl->GetPos();

		switch (nSBCode)
		{
		case TB_THUMBTRACK:
			
			break;

		case TB_ENDTRACK:
		case TB_THUMBPOSITION:
			
			break;

		default:
			
			break;
		}
	}

	CDlgSubTab::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CDlgCalbExposureMirrorTune::OnSliderMirrorPowerDraw(NMHDR* pNMHDR, LRESULT* pResult)
{

	*pResult = 0;
}

void CDlgCalbExposureMirrorTune::OnSliderMirrorPowerChanging(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
}


void CDlgCalbExposureMirrorTune::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 10302)
	{
		UpdateIDSImage();
	}

	CDlgSubTab::OnTimer(nIDEvent);
}

BOOL CDlgCalbExposureMirrorTune::PhotoLedOnOff(UINT8 head, UINT8 led, UINT16 index)
{
	if (index > 0)
	{	
		if (FALSE == uvEng_Luria_ReqSetLedPowerResetAll())				return FALSE;
		if (FALSE == uvEng_Luria_ReqSetActiveSequence(head, 0x01))	return FALSE;
		if (FALSE == uvEng_Luria_ReqSetLedPowerResetAll())				return FALSE;


		if (FALSE == uvEng_Luria_ReqSetFlatnessMaskOn(head, 0x00))	return FALSE;
		if (FALSE == uvEng_Luria_ReqSetDmdMirrorShake(head, 0x00))	return FALSE;
		auto res = uvEng_Luria_ReqSetLightIntensity(head, ENG_LLPI(led), index);

		if (res)
		{
			uvEng_Luria_ReqGetLedPower(head, ENG_LLPI(led));
			res = uvEng_Luria_ReqSetLightPulseDuration(head, LIGHT_PULSE_DURATION);
			if (res == false) return FALSE;
		}
	}
	else
	{
		uvEng_Luria_ReqSetLedLightOnOff(head, (ENG_LLPI)led, false);
	}
	return TRUE;
}

BOOL CDlgCalbExposureMirrorTune::PhotoImageLoad(UINT8 head, UINT8 imageNum)
{
	if (FALSE == uvEng_Luria_ReqSetLoadInternalTestImage(head, imageNum))	
		return FALSE;
	return TRUE;
}

void CDlgCalbExposureMirrorTune::UpdateIDSImage()
{
	struct BitmapInfoGray8
	{
		BITMAPINFOHEADER header;
		RGBQUAD colors[256];
	};

	CWnd* pView = GetDlgItem(IDC_IDSCAMERA_VIEW);
	if (!pView || !::IsWindow(pView->GetSafeHwnd()))
		return;

	CRect rc;
	pView->GetWindowRect(&rc);
	ScreenToClient(&rc);

	CClientDC dc(this);
	dc.FillSolidRect(&rc, RGB(0, 0, 0));

	if (idsSnapshot.empty() || idsW <= 0 || idsH <= 0)
		return;

	BitmapInfoGray8 bmi{};
	bmi.header.biSize = sizeof(BITMAPINFOHEADER);
	bmi.header.biWidth = idsW;
	bmi.header.biHeight = -idsH;      // top-down
	bmi.header.biPlanes = 1;
	bmi.header.biBitCount = 8;
	bmi.header.biCompression = BI_RGB;

	for (int i = 0; i < 256; ++i)
	{
		bmi.colors[i].rgbBlue = (BYTE)i;
		bmi.colors[i].rgbGreen = (BYTE)i;
		bmi.colors[i].rgbRed = (BYTE)i;
		bmi.colors[i].rgbReserved = 0;
	}

	::StretchDIBits(
		dc.GetSafeHdc(),
		rc.left, rc.top, rc.Width(), rc.Height(),
		0, 0, idsW, idsH,
		idsSnapshot.data(),
		reinterpret_cast<BITMAPINFO*>(&bmi),
		DIB_RGB_COLORS,
		SRCCOPY
	);
}
                 



void CDlgCalbExposureMirrorTune::FixControlToPhysicalPixels(UINT ctrlId, int targetWpx, int targetHpx)
{

	auto MulDivRound = [&](int a, int b, int c)->int{return (int)::MulDiv(a, b, c);};

	CWnd* w = GetDlgItem(ctrlId);
	if (!w || !::IsWindow(w->GetSafeHwnd()))
		return;

	CRect rc;
	w->GetWindowRect(&rc);
	ScreenToClient(&rc);

	UINT dpi = 96;
	HMODULE user32 = ::GetModuleHandleW(L"user32.dll");
	auto pGetDpiForWindow = (UINT(WINAPI*)(HWND))::GetProcAddress(user32, "GetDpiForWindow");
	if (pGetDpiForWindow)
		dpi = pGetDpiForWindow(m_hWnd);
	else
	{
		HDC hdc = ::GetDC(m_hWnd);
		dpi = (UINT)::GetDeviceCaps(hdc, LOGPIXELSX);
		::ReleaseDC(m_hWnd, hdc);
	}

	int newW = MulDivRound(targetWpx, 96, (int)dpi);
	int newH = MulDivRound(targetHpx, 96, (int)dpi);

	w->SetWindowPos(
		nullptr,
		rc.left, rc.top,
		newW, newH,
		SWP_NOZORDER | SWP_NOACTIVATE
	);

	
	w->Invalidate();
}