// DlgCalb.cpp: 구현 파일
//

#include "pch.h"
#include "../../../pops/DlgParam.h"
#include "DlgCalbUVPowerCheck.h"
#include "afxdialogex.h"
#include <iostream>
#include <string>
#include <afxtaskdialog.h>

/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgCalbUVPowerCheck::CDlgCalbUVPowerCheck(UINT32 id, CWnd* parent)
	: CDlgSubTab(id, parent)
{
	m_enDlgID = ENG_CMDI_TAB::en_menu_tab_calb_uvpower_check;
	m_bManualPosMoving = FALSE;

	m_nSelectedMove = 0;
	m_nSelectedRecipe = -1;

	m_nMeasureHead = 0;
	m_nMeasureLed = 0;
	m_bMeasure = FALSE;

	for (int i = 0; i < eCALB_UVPOWER_CHECK_TIMER_MAX; i++)
	{
		m_bBlink[i] = FALSE;
	}
}

CDlgCalbUVPowerCheck::~CDlgCalbUVPowerCheck()
{
	CPowerMeasureMgr::GetInstance()->SetStopProcess();
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgCalbUVPowerCheck::DoDataExchange(CDataExchange* dx)
{
	CDlgSubTab::DoDataExchange(dx);

	UINT32 i, u32StartID;


	u32StartID = IDC_CALB_UVPOWER_CHECK_BTN_SAVE;
	for (i = 0; i < eCALB_UVPOWER_CHECK_BTN_MAX; i++)		DDX_Control(dx, u32StartID + i, m_btn_ctl[i]);

	u32StartID = IDC_CALB_UVPOWER_CHECK_STT_OPERATION;
	for (i = 0; i < eCALB_UVPOWER_CHECK_STT_MAX; i++)		DDX_Control(dx, u32StartID + i, m_stt_ctl[i]);

	u32StartID = IDC_CALB_UVPOWER_CHECK_TXT_RECIPE_NAME;
	for (i = 0; i < eCALB_UVPOWER_CHECK_TXT_MAX; i++)		DDX_Control(dx, u32StartID + i, m_txt_ctl[i]);

	u32StartID = IDC_CALB_UVPOWER_CHECK_GRD_POSITION;
	for (i = 0; i < eCALB_UVPOWER_CHECK_GRD_MAX; i++)		DDX_Control(dx, u32StartID + i, m_grd_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgCalbUVPowerCheck, CDlgSubTab)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CALB_UVPOWER_CHECK_BTN_SAVE, IDC_CALB_UVPOWER_CHECK_BTN_SAVE + eCALB_UVPOWER_CHECK_BTN_MAX, OnBtnClick)
	ON_NOTIFY(NM_CLICK, IDC_CALB_UVPOWER_CHECK_GRD_POSITION, &CDlgCalbUVPowerCheck::OnClickGridPosition)
	ON_NOTIFY(NM_CLICK, IDC_CALB_UVPOWER_CHECK_GRD_RESULT, &CDlgCalbUVPowerCheck::OnClickGridResult)
	ON_MESSAGE(eMSG_UV_POWER_RECIPE_SET_POINT, InputSetPoint)
	ON_MESSAGE(eMSG_UV_POWER_RECIPE_MEASURE_COMP, InputMeasureCompl)
	ON_MESSAGE(eMSG_UV_POWER_RECIPE_ALL_COMP, ReportResult)
	ON_WM_TIMER()
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgCalbUVPowerCheck::PreTranslateMessage(MSG* msg)
{
	return CDlgSubTab::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgCalbUVPowerCheck::OnInitDlg()
{
	int nHeadCnt = uvEng_GetConfig()->luria_svc.ph_count;

	/* 컨트롤 초기화 */
	InitCtrl();

	InitGridPosition(nHeadCnt);
	InitGridResult();
	AttachButton();

	UpdateGridPosition();

	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgCalbUVPowerCheck::OnExitDlg()
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
VOID CDlgCalbUVPowerCheck::OnPaintDlg(CDC* dc)
{

	/* 자식 윈도 호출 */
	CDlgSubTab::DrawOutLine();
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgCalbUVPowerCheck::OnResizeDlg()
{
}

/*
 desc : 부모 스레드에 의해 주기적으로 호출됨
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgCalbUVPowerCheck::UpdatePeriod(UINT64 tick, BOOL is_busy)
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
VOID CDlgCalbUVPowerCheck::UpdateControl(UINT64 tick, BOOL is_busy)
{
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgCalbUVPowerCheck::InitCtrl()
{
	CResizeUI clsResizeUI;
	CString strTemp;
	CRect rStt;

	for (int i = 0; i < eCALB_UVPOWER_CHECK_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
		// by sysandj : Resize UI
	}

	/* static - Normal */
	for (int i = 0; i < eCALB_UVPOWER_CHECK_STT_MAX; i++)
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
	for (int i = 0; i < eCALB_UVPOWER_CHECK_TXT_MAX; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_txt_ctl[i]);
		// by sysandj : Resize UI
	}
}


VOID CDlgCalbUVPowerCheck::InitGridPosition(int nHeadCount /*= DEF_DEFAULT_HEAD_COUNT*/)
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <int>			vRowSize(nHeadCount + 1);
	std::vector <int>			vColSize(eGRD_COL_POSITION_MAX);
	std::vector <std::wstring>	vTitle			= { _T("PH INDEX"), _T("X [mm]"), _T("Y [mm]"), _T("Z [mm]"), _T("BUTTON"), _T("BUTTON") };
	std::vector <std::wstring>	vItem			= { _T("HEAD"), _T("0.0000"), _T("0.0000"), _T("0.0000"), _T("READ"), _T("MOVE") };
	std::vector <COLORREF>		vColColor		= { ALICE_BLUE, WHITE_, WHITE_, WHITE_, DEF_COLOR_BTN_PAGE_NORMAL, DEF_COLOR_BTN_PAGE_NORMAL };
	std::vector <COLORREF>		vColTextColor	= { BLACK_, BLACK_, BLACK_, BLACK_, WHITE_, WHITE_ };

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_CHECK_GRD_POSITION];

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

VOID CDlgCalbUVPowerCheck::UpdateGridPosition()
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_CHECK_GRD_POSITION];

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


VOID CDlgCalbUVPowerCheck::InitGridResult()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	CString		strText;

	int nRowCnt = uvEng_GetConfig()->luria_svc.led_count + 1;
	int nColCnt = uvEng_GetConfig()->luria_svc.ph_count + 1;

	std::vector <int>			vRowSize(nRowCnt);
	std::vector <int>			vColSize(nColCnt);

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_CHECK_GRD_RESULT];

	clsResizeUI.ResizeControl(this, pGrid);
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);


	int nTotalHeight = 0;
	int nTotalWidth = 0;

	for (auto& height : vRowSize)
	{
		height = (rGrid.Height() - 1) / (int)vRowSize.size();
		nTotalHeight += height;
	}

	for (auto& width : vColSize)
	{
		width = (rGrid.Width() - 1) / (int)vColSize.size();
		nTotalWidth += width;
	}

	vColSize.front() += (rGrid.Width() - 1) - nTotalWidth;

	/* 객체 기본 설정 */
	pGrid->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
	pGrid->SetRowCount(nRowCnt);
	pGrid->SetColumnCount(nColCnt);
	pGrid->SetFixedColumnCount(0);
	pGrid->SetBkColor(BLACK_);
	pGrid->SetFixedColumnSelection(0);


	for (int nRow = 0; nRow < nRowCnt; nRow++)
	{
		pGrid->SetRowHeight(nRow, vRowSize[nRow]);

		for (int nCol = 0; nCol < nColCnt; nCol++)
		{
			pGrid->SetColumnWidth(nCol, vColSize[nCol]);
			pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			/* 타이틀 */
			if (0 == nRow || 0 == nCol)
			{
				pGrid->SetItemBkColour(nRow, nCol, ALICE_BLUE);

				if (0 == nRow && 0 != nCol)
				{
					strText.Format(_T("PhotoHead #%d"), nCol);
					pGrid->SetItemText(nRow, nCol, strText);
				}
				else if (0 == nCol && 0 != nRow)
				{
					strText.Format(_T("%d nm"), uvEng_Luria_GetLedNoToFreq(nRow));
					pGrid->SetItemText(nRow, nCol, strText);
				}
			}
			else if (0 != nCol && 0 != nRow)
			{
				pGrid->SetItemText(nRow, nCol, _T("0.000 / 0.000"));
			}
		}
	}

	/* 기본 속성 및 갱신 */
	pGrid->SetBaseGridProp(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}

VOID CDlgCalbUVPowerCheck::AttachButton()
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_CHECK_GRD_POSITION];
	CMacButton* pBtn = &m_btn_ctl[eCALB_UVPOWER_CHECK_BTN_OPEN_CTRL_PANEL];
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
BOOL CDlgCalbUVPowerCheck::InitObject()
{

	return TRUE;
}

/*
 desc : 객체 해제
 parm : None
 retn : None
*/
VOID CDlgCalbUVPowerCheck::CloseObject()
{
}


VOID CDlgCalbUVPowerCheck::SelectRecipe()
{
	LPG_PLPI pstPowerI = nullptr;
	CString strLedPower;
	CStringArray strArrRecipe;

	strArrRecipe.Add(_T("[CANCEL]"));

	for (int i = 0; i < uvEng_LedPower_GetCount(); i++)
	{
		pstPowerI = uvEng_LedPower_GetLedPowerIndex(i);

		strLedPower.Format(_T("%s"), (CString)pstPowerI->led_name);

		for (int nWatt = 0; nWatt < uvEng_GetConfig()->luria_svc.led_count; nWatt++)
		{
			strLedPower.AppendFormat(_T("\n\r%dnm ("), uvEng_Luria_GetLedNoToFreq(nWatt + 1));

			for (int nHead = 0; nHead < uvEng_GetConfig()->luria_svc.ph_count; nHead++)
			{
				strLedPower.AppendFormat(_T("%.3fW, "), pstPowerI->led_watt[nHead][nWatt]);
			}

			strLedPower.Delete(strLedPower.GetLength() - 2, 2);
			strLedPower.AppendFormat(_T(")"), nWatt);
		}

		strArrRecipe.Add(strLedPower);
	}

	int nSelectRecipe = ShowMultiSelectMsg(EN_MSG_BOX_TYPE::eQUEST, _T("LOAD LED POWER RECIPE"), strArrRecipe);

	if (0 == nSelectRecipe)
	{
		return;
	}

	m_nSelectedRecipe = nSelectRecipe - 1;
	UpdateRecipeData();
}


VOID CDlgCalbUVPowerCheck::MakeRecipe()
{
	CDlgParam dlg;
	DLG_PARAM stParam;
	VCT_DLG_PARAM stVctParam;
	STG_PLPI stData;
	float fWatt = 0.0;
	UINT16 u16Index = 0;
	SYSTEMTIME stTm;
	CString strName;

	stParam.Init();
	stParam.strName = _T("Recipe Name");
	stParam.strValue = _T("");
	stParam.strUnit = _T("Name");
	stParam.enFormat = ENM_DITM::en_string;
	stParam.dMin = 1;
	stParam.dMax = 10;
	stVctParam.push_back(stParam);

	for (int nLed = 1; nLed <= uvEng_GetConfig()->luria_svc.led_count; nLed++)
	{
		stParam.Init();
		stParam.strName.Format(_T("Set Watt (%dnm)"), uvEng_Luria_GetLedNoToFreq(nLed));
		stParam.strValue = _T("0.001");
		stParam.strUnit = _T("W");
		stParam.enFormat = ENM_DITM::en_double;
		stParam.dMin = 0.001;
		stParam.dMax = 10.0f;
		stParam.u8DecPts = 3;
		stVctParam.push_back(stParam);
	}

	if (IDOK != dlg.MyDoModal(stVctParam))
	{
		return;
	}

	if (TRUE == stVctParam[0].strValue.IsEmpty())
	{
		AfxMessageBox(_T("저장 실패!\n파일명이 입력되지 않았습니다."));
		return;
	}

	/* 현재 시간 얻기 */
	GetLocalTime(&stTm);
	strName.Format(_T("%04d-%02d-%02d(%s)"), stTm.wYear, stTm.wMonth, stTm.wDay, stVctParam[0].strValue);

	stData.Init();
	CStringA strA(strName);
	stData.led_name = strA.GetBuffer();

	for (int nPH = 0; nPH < uvEng_GetConfig()->luria_svc.ph_count; nPH++)
	{
		for (int nLed = 1; nLed <= uvEng_GetConfig()->luria_svc.led_count; nLed++)
		{
			fWatt = (float)_ttof(stVctParam[nLed].strValue);
			u16Index = CPowerMeasureMgr::GetInstance()->GetPowerIndex(nPH + 1, nLed, (double)fWatt);

			stData.led_watt[nPH][nLed - 1] = fWatt;
			stData.led_index[nPH][nLed - 1] = u16Index;
		}
	}

	uvEng_LedPower_SavePowerEx(&stData);
	strA.ReleaseBuffer();
	stData.led_name = nullptr;
	stData.Close();

	uvEng_LedPower_LoadPower();
}



VOID CDlgCalbUVPowerCheck::MakeMaxRecipe()
{
	CDlgParam dlg;
	DLG_PARAM stParam;
	VCT_DLG_PARAM stVctParam;
	STG_PLPI stData;
	double dWatt = 0.0;
	double dRate = 1.0;
	double dMinWatt = 99.99;
	UINT16 u16Index = 0;
	SYSTEMTIME stTm;
	CString strName;

	stParam.Init();
	stParam.strName = _T("Recipe Name");
	stParam.strValue = _T("");
	stParam.strUnit = _T("Name");
	stParam.enFormat = ENM_DITM::en_string;
	stParam.dMin = 1;
	stParam.dMax = 10;
	stVctParam.push_back(stParam);

	stParam.Init();
	stParam.strName = _T("Rate");
	stParam.strValue = _T("");
	stParam.strUnit = _T("%");
	stParam.enFormat = ENM_DITM::en_uint8;
	stParam.dMin = 1;
	stParam.dMax = 100;
	stVctParam.push_back(stParam);

	if (IDOK != dlg.MyDoModal(stVctParam))
	{
		return;
	}

	if (TRUE == stVctParam[0].strValue.IsEmpty())
	{
		AfxMessageBox(_T("저장 실패!\n파일명이 입력되지 않았습니다."));
		return;
	}

	/* 현재 시간 얻기 */
	GetLocalTime(&stTm);
	strName.Format(_T("%04d-%02d-%02d(%s)"), stTm.wYear, stTm.wMonth, stTm.wDay, stVctParam[0].strValue);

	stData.Init();
	CStringA strA(strName);
	stData.led_name = strA.GetBuffer();

	dRate = _ttof(stVctParam[1].strValue) / 100;

	
	for (int nLed = 1; nLed <= uvEng_GetConfig()->luria_svc.led_count; nLed++)
	{
		for (int nPH = 0; nPH < uvEng_GetConfig()->luria_svc.ph_count; nPH++)
		{
			if (FALSE == CPowerMeasureMgr::GetInstance()->GetMaxValue(nPH + 1, nLed, u16Index, dWatt))
			{
				continue;
			}

			dMinWatt = (dMinWatt > dWatt) ? dWatt : dMinWatt;
		}

		dMinWatt *= dRate;

		for (int nPH = 0; nPH < uvEng_GetConfig()->luria_svc.ph_count; nPH++)
		{
			u16Index = CPowerMeasureMgr::GetInstance()->GetPowerIndex(nPH + 1, nLed, dMinWatt);

			stData.led_watt[nPH][nLed - 1] = (float)dMinWatt;
			stData.led_index[nPH][nLed - 1] = u16Index;
		}
	}

// 	for (int nPH = 0; nPH < uvEng_GetConfig()->luria_svc.ph_count; nPH++)
// 	{
// 		for (int nLed = 1; nLed <= uvEng_GetConfig()->luria_svc.led_count; nLed++)
// 		{
// 			if (FALSE == CPowerMeasureMgr::GetInstance()->GetMaxValue(nPH + 1, nLed, u16Index, dWatt))
// 			{
// 				continue;
// 			}
// 
// 			stData.led_watt[nPH][nLed - 1] = (float)dWatt;
// 			stData.led_index[nPH][nLed - 1] = u16Index;
// 		}
// 	}

	uvEng_LedPower_SavePowerEx(&stData);
	strA.ReleaseBuffer();
	stData.led_name = nullptr;
	stData.Close();

	uvEng_LedPower_LoadPower();
}


VOID CDlgCalbUVPowerCheck::UpdateRecipeData()
{
	LPG_PLPI pstPowerI = uvEng_LedPower_GetLedPowerIndex(m_nSelectedRecipe);
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_CHECK_GRD_RESULT];
	CString strText;

	if (nullptr == pstPowerI)
	{
		return;
	}

	m_txt_ctl[eCALB_UVPOWER_CHECK_TXT_RECIPE_NAME].SetTextToStr((PTCHAR)(LPCTSTR)(CString)pstPowerI->led_name);

	for (int nRow = 1; nRow < pGrid->GetRowCount(); nRow++)
	{
		for (int nCol = 1; nCol < pGrid->GetColumnCount(); nCol++)
		{
			pGrid->SetItemBkColour(nRow, nCol, WHITE_);
			strText.Format(_T("%.3f / 0.000"), pstPowerI->led_watt[nCol - 1][nRow - 1]);
			pGrid->SetItemText(nRow, nCol, strText);
		}
	}

	pGrid->Refresh();
}


COLORREF CDlgCalbUVPowerCheck::CompareParam(double dParam1, double dParam2)
{
	LPG_CLPI pParam = &uvEng_GetConfig()->led_power;
	double dErrVal = fabs(dParam1 - dParam2);

	if (dErrVal < pParam->normal_boundary)			return GREEN_;
	else if (dErrVal < pParam->warning_boundary)	return GOLD;

	return CRIMSON;
}


LRESULT CDlgCalbUVPowerCheck::InputSetPoint(WPARAM wParam, LPARAM lParam)
{
	m_nMeasureHead = (int)wParam;
	m_nMeasureLed = (int)lParam;
	m_bMeasure = TRUE;
	return 0;
}


LRESULT CDlgCalbUVPowerCheck::InputMeasureCompl(WPARAM wParam, LPARAM lParam)
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_CHECK_GRD_RESULT];
	CString strFullText;
	CString strTargetValue;
	int nParam = (int)wParam;
	double dMeasureValue = nParam * 0.001;

	m_bMeasure = FALSE;

	strFullText = pGrid->GetItemText(m_nMeasureLed, m_nMeasureHead);
	AfxExtractSubString(strTargetValue, strFullText, 0, '/');

	strFullText.Format(_T("%s/ %.3f"), strTargetValue, dMeasureValue);
	pGrid->SetItemText(m_nMeasureLed, m_nMeasureHead, strFullText);
	pGrid->SetItemFgColour(m_nMeasureLed, m_nMeasureHead, CompareParam(_ttof(strTargetValue), dMeasureValue));
	pGrid->SetItemBkColour(m_nMeasureLed, m_nMeasureHead, WHITE_);

	pGrid->Refresh();
	return 0;
}


LRESULT CDlgCalbUVPowerCheck::ReportResult(WPARAM wParam, LPARAM lParam)
{
	CString strPath;
	CString strName;

	strPath.Format(_T("%s\\%s\\recipe\\History"), g_tzWorkDir, CUSTOM_DATA_CONFIG);

	if (FALSE == CheckFileExists(strPath))
	{
		CreateDir(strPath);
	}

	SYSTEMTIME stTm;
	/* 현재 시간 얻기 */
	GetLocalTime(&stTm);
	strName.Format(_T("%s\\%04d_%02d_%02d.csv"), strPath, stTm.wYear, stTm.wMonth, stTm.wDay);

	WriteFile(strName);
	return 0;
}

/*
 desc : 일반 버튼 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgCalbUVPowerCheck::OnBtnClick(UINT32 id)
{
	int nBtnID = id - IDC_CALB_UVPOWER_CHECK_BTN_SAVE;

	if (TRUE == CPowerMeasureMgr::GetInstance()->IsProcessWorking() &&
		nBtnID != eCALB_UVPOWER_CHECK_BTN_STOP)
	{
		AfxMessageBox(_T("현재 측정중입니다."));
		return;
	}

	switch (nBtnID)
	{
	case eCALB_UVPOWER_CHECK_BTN_SAVE:
		SaveParam();
		break;
	case eCALB_UVPOWER_CHECK_BTN_START:
		CheckStart();
		break;
	case eCALB_UVPOWER_CHECK_BTN_STOP:
		CheckStop();
		break;
	case eCALB_UVPOWER_CHECK_BTN_OPEN_CTRL_PANEL:
		ShowCtrlPanel();
		break;
	case eCALB_UVPOWER_CHECK_BTN_SELECT_RECIPE:
		SelectRecipe();
		break;
	case eCALB_UVPOWER_CHECK_BTN_MAKE_RECIPE:
		MakeRecipe();
		break;
	case eCALB_UVPOWER_CHECK_BTN_ALL_HEAD_OFF:
		CPowerMeasureMgr::GetInstance()->AllPhotoLedOff();
		break;
	case eCALB_UVPOWER_CHECK_BTN_MAKE_MAX_RECIPE:
		MakeMaxRecipe();
		break;
	default:
		break;
	}
}


void CDlgCalbUVPowerCheck::CheckStart()
{
	if (TRUE == m_bManualPosMoving)
	{
		AfxMessageBox(_T("현재 이동중입니다."));
		return;
	}

	if (-1 == m_nSelectedRecipe)
	{
		AfxMessageBox(_T("레시피를 선택하여 주십시오."));
		return;
	}

	if (IDNO == AfxMessageBox(L"측정을 시작하시겠습니까?", MB_YESNO))
	{
		return;
	}

	KillTimer(eCALB_UVPOWER_CHECK_TIMER_START);

	m_btn_ctl[eCALB_UVPOWER_CHECK_BTN_SELECT_RECIPE].EnableWindow(FALSE);
	SetTimer(eCALB_UVPOWER_CHECK_TIMER_START, 500, NULL);

	CPowerMeasureMgr::GetInstance()->SetRecipeNum(m_nSelectedRecipe);

	if (FALSE == CPowerMeasureMgr::GetInstance()->RecipeCheckStart(this->GetSafeHwnd()))
	{
		AfxMessageBox(_T("측정 실패"));
	}
}

void CDlgCalbUVPowerCheck::CheckStop()
{
	m_btn_ctl[eCALB_UVPOWER_CHECK_BTN_SELECT_RECIPE].EnableWindow(TRUE);
	KillTimer(eCALB_UVPOWER_CHECK_TIMER_START);

 	CPowerMeasureMgr::GetInstance()->SetStopProcess();
}

void CDlgCalbUVPowerCheck::ShowCtrlPanel()
{
	if (TRUE == m_bManualPosMoving)
	{
		AfxMessageBox(_T("현재 이동중입니다."));
		return;
	}

	// 230419 mhbaek Add : MotorConsole 연동
	::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), eMSG_MAIN_OPEN_CONSOLE, NULL, NULL);
}


void CDlgCalbUVPowerCheck::SaveParam()
{
	if (IDNO == AfxMessageBox(L"저장하시겠습니까?", MB_YESNO))
	{
		return;
	}

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_CHECK_GRD_POSITION];
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

	uvEng_SaveConfig();
}


void CDlgCalbUVPowerCheck::OnClickGridPosition(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	
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

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_CHECK_GRD_POSITION];

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
		SetTimer(eCALB_UVPOWER_CHECK_TIMER_MOVING, 500, NULL);

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

void CDlgCalbUVPowerCheck::OnClickGridResult(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_CHECK_GRD_RESULT];

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);
}

void CDlgCalbUVPowerCheck::OnTimer(UINT_PTR nIDEvent)
{
	if (eCALB_UVPOWER_CHECK_TIMER_START == nIDEvent)
	{
		CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_CHECK_GRD_RESULT];

		if (FALSE == CPowerMeasureMgr::GetInstance()->IsProcessWorking())
		{
			m_btn_ctl[eCALB_UVPOWER_CHECK_BTN_SELECT_RECIPE].EnableWindow(TRUE);
			Invalidate(FALSE);

			KillTimer(nIDEvent);
		}

		if (TRUE == m_bMeasure)
		{
			if (CRIMSON != pGrid->GetItemBkColour(m_nMeasureLed, m_nMeasureHead) &&		// Error
				GOLD != pGrid->GetItemBkColour(m_nMeasureLed, m_nMeasureHead) &&		// Warning
				GREEN_ != pGrid->GetItemBkColour(m_nMeasureLed, m_nMeasureHead))		// OK
			{
				if (TRUE == m_bBlink[nIDEvent - 1])
				{
					pGrid->SetItemBkColour(m_nMeasureLed, m_nMeasureHead, WHITE_);
				}
				else
				{
					pGrid->SetItemBkColour(m_nMeasureLed, m_nMeasureHead, YELLOW);
				}

				m_bBlink[nIDEvent - 1] = !m_bBlink[nIDEvent - 1];
				pGrid->Refresh();
			}
		}
		else
		{
			m_bBlink[nIDEvent - 1] = FALSE;
		}
	}

	if (eCALB_UVPOWER_CHECK_TIMER_MOVING == nIDEvent)
	{
		if (0 > m_nSelectedMove)
		{
			KillTimer(nIDEvent);
		}

		CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_CHECK_GRD_POSITION];
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

BOOL CDlgCalbUVPowerCheck::PopupKBDN(ENM_DITM enType, CString strInput, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts/* = 0*/)
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

int CDlgCalbUVPowerCheck::ShowMultiSelectMsg(EN_MSG_BOX_TYPE mType, CString strTitle, CStringArray& strArrCommand)
{
	// 	TCHAR		out[1024];
	// 	va_list		va;
	if (0 == strArrCommand.GetCount())
	{
		return -1;
	}

	CTaskDialog taskDlg(_T(""), _T(""), _T(""), NULL);

	switch (mType)
	{
	case eINFO: taskDlg.SetMainIcon(IDI_INFORMATION); break;
	case eSTOP: taskDlg.SetMainIcon(IDI_ERROR); break;
	case eWARN: taskDlg.SetMainIcon(IDI_WARNING); break;
	case eQUEST: taskDlg.SetMainIcon(IDI_INFORMATION); break;
	default:
		taskDlg.SetMainIcon(IDI_INFORMATION);
	}

	CStringArray strArrMsg;

	for (int i = 0; i < strArrCommand.GetCount(); i++)
	{
		strArrMsg.Add(strArrCommand.GetAt(i));
	}

	taskDlg.SetWindowTitle(strTitle);
	taskDlg.SetMainInstruction(strTitle);

	for (int i = 0; i < strArrMsg.GetCount(); i++)
	{
		taskDlg.AddCommandControl(201 + i, strArrMsg.GetAt(i));
	}

	taskDlg.SetDialogWidth(::GetSystemMetrics(SM_CXSCREEN) / 6);

	int nResult = (int)taskDlg.DoModal();

	return nResult - 201;
}


BOOL CDlgCalbUVPowerCheck::CheckFileExists(LPCTSTR szFileName)
{
	WIN32_FIND_DATA  data;
	HANDLE handle = FindFirstFile(szFileName, &data);
	if (handle != INVALID_HANDLE_VALUE)
	{
		FindClose(handle);
		return TRUE;
	}

	return FALSE;
}


VOID CDlgCalbUVPowerCheck::CreateDir(LPCTSTR Path)
{
	TCHAR DirName[MAX_PATH];
	TCHAR szPath[MAX_PATH];
	swprintf_s(szPath, MAX_PATH, _T("%s"), Path);

	TCHAR* p = szPath;
	TCHAR* q = DirName;
	while (*p)
	{
		if (('\\' == *p) || ('/' == *p))
		{
			if (':' != *(p - 1))
			{
				CreateDirectory(DirName, NULL);
			}
		}
		*q++ = *p++;
		*q = '\0';
	}
	CreateDirectory(DirName, NULL);
}


VOID CDlgCalbUVPowerCheck::WriteFile(CString strName)
{
	LPG_PLPI pstPowerI = uvEng_LedPower_GetLedPowerIndex(m_nSelectedRecipe);
	CStdioFile sFile;
	CString strWrite;
	CString strValue;
	CString strCell;
	SYSTEMTIME stTm;

	if (nullptr == pstPowerI)
	{
		return;
	}

	if (FALSE == sFile.Open(strName, CFile::modeCreate | CFile::modeWrite | CFile::typeText | CFile::modeNoTruncate))
	{
		return;
	}

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_UVPOWER_CHECK_GRD_RESULT];

	for (int nCol = 1; nCol < pGrid->GetColumnCount(); nCol++)
	{
		for (int nRow = 1; nRow < pGrid->GetRowCount(); nRow++)
		{
			strCell = pGrid->GetItemText(nRow, nCol);
			strCell.Replace(_T("/"), _T(","));

			strValue += strCell + _T(",");
		}

		strValue += _T(",");
	}

	/* 현재 시간 얻기 */
	GetLocalTime(&stTm);
	strWrite.Format(_T("[%02d:%02d:%02d.%03d],[%s],%s\n"), stTm.wHour, stTm.wMinute, stTm.wSecond, stTm.wMilliseconds, (CString)pstPowerI->led_name, strValue);

	sFile.SeekToEnd();
	sFile.WriteString(strWrite);
	sFile.Close();
}