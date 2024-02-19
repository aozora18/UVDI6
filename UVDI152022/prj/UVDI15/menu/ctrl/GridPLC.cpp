
/*
 desc : PLC 데이터 실시간 모니터링
*/

#include "pch.h"
#include "../../MainApp.h"
#include "GridPLC.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : parent	- [in]  부모 윈도 핸들
		grid	- [in]  Grid가 출력될 윈도 핸들
 retn : None
*/
CGridPLC::CGridPLC(HWND parent, HWND grid)
{
	m_pGrid		= NULL;
	m_pParent	= CWnd::FromHandle(parent);
	m_pCtrl		= CWnd::FromHandle(grid);
	m_u8PageNo	= 0x00;
	m_u8LastPage= 0xff;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CGridPLC::~CGridPLC()
{
	if (m_pGrid)
	{
		if (m_pGrid->GetSafeHwnd())	m_pGrid->DestroyWindow();
		delete m_pGrid;
	}
}

/*
 desc : 그리드 컨트롤 생성
 parm : None
 retn : TRUE or FALSE
*/
BOOL CGridPLC::InitGrid()
{
	TCHAR tzTitle[2][32] = { L"Item", L"Value" };
	INT32 i, j, iColCnt = 2, iRowCnt = 32;
	// by sysandj : Grid Size 맞춤
	//INT32 i32Width[5] = { 325, 100 };
	INT32 i32Width[2] = { 325, 100 };
	// by sysandj : Grid Size 맞춤
	UINT32 dwStyle = WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	UINT32 dwFormat[2] = { DT_VCENTER | DT_SINGLELINE | DT_LEFT,
						   DT_VCENTER | DT_SINGLELINE | DT_CENTER };
	GV_ITEM stGV = { NULL };
	CRect rGrid;

	/* 현재 윈도 Client 크기 */
	m_pCtrl->GetWindowRect(rGrid);
	m_pParent->ScreenToClient(rGrid);
	
	CResizeUI clsResizeUI;
	/* Grid 크기 재설정 */
	rGrid.left	+= (long)(15 * clsResizeUI.GetRateX());
	rGrid.top	+= (long)(22 * clsResizeUI.GetRateY());
	rGrid.right	-= (long)(15 * clsResizeUI.GetRateX());
	rGrid.bottom-= (long)(85 * clsResizeUI.GetRateY());

	// by sysandj : Grid Size 맞춤
	int nWidth = 0;
	int nResizeWidth = 0;
	for (int i = 0; i < _countof(i32Width); i++)
	{
		nWidth += i32Width[i];
	}

	for (int i = 0; i < _countof(i32Width); i++)
	{
		i32Width[i] += (rGrid.Width() - nWidth - ::GetSystemMetrics(SM_CXVSCROLL)) / _countof(i32Width);
		nResizeWidth += i32Width[i];
	}
	i32Width[0] += rGrid.Width() - ::GetSystemMetrics(SM_CXVSCROLL) - nResizeWidth - 2;
	// by sysandj : Grid Size 맞춤

	// 그리드 컨트롤 기본 공통 속성
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);
	/* 객체 생성 */
	m_pGrid = new CGridCtrl;
	ASSERT(m_pGrid);
	m_pGrid->SetDrawScrollBarVert(TRUE);
	m_pGrid->SetDrawScrollBarHorz(FALSE);
	if (!m_pGrid->Create(rGrid, m_pParent, IDC_GRID_CTRL_PLC_IO_POINT, dwStyle))
	{
		delete m_pGrid;
		m_pGrid = NULL;
		return FALSE;
	}
	/* 객체 기본 설정 */
	m_pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	m_pGrid->SetRowCount(iRowCnt);
	m_pGrid->SetColumnCount(iColCnt);
	//m_pGrid->SetFixedRowCount(1);
	m_pGrid->SetRowHeight(0, 34);
	m_pGrid->SetFixedColumnCount(0);
	m_pGrid->SetBkColor(RGB(255, 255, 255));
	m_pGrid->SetFixedColumnSelection(0);

	/* 타이틀 (첫 번째 행) */
	stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	stGV.crBkClr = RGB(214, 214, 214);
	stGV.crFgClr = RGB(0, 0, 0);
	for (i = 0; i < iColCnt; i++)
	{
		stGV.row = 0;
		stGV.col = i;
		stGV.strText = tzTitle[i];
		m_pGrid->SetItem(&stGV);
		m_pGrid->SetColumnWidth(i, i32Width[i]);
//		m_pGrid->SetItemFont(0, i, &g_lf);

		// by sysandj
		m_pGrid->SetItemBkColour(0, i, DEF_COLOR_BTN_MENU_NORMAL);
		m_pGrid->SetItemFgColour(0, i, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
		// by sysandj
	}
	/* 본문 (1 ~ 8 Rows) */
	stGV.crBkClr = RGB(255, 255, 255);
	stGV.strText = L"";
	for (i = 1; i < iRowCnt; i++)
	{
		m_pGrid->SetRowHeight(i, 26);
		/* Column (0 ~ 4) */
		for (j = 0; j < iColCnt; j++)
		{
			stGV.nFormat = j != 0 ? dwFormat[1] : dwFormat[0];
			stGV.row = i;
			stGV.col = j;
			m_pGrid->SetItem(&stGV);
//			m_pGrid->SetItemFont(i, j, &g_lf);
		}
	}

	m_pGrid->SetBaseGridProp(0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGrid->Refresh();

	/* 생성된 후 무조건 첫 페이지 출력*/
	UpdatePeriod();

	return TRUE;
}

/*
 desc : 모니터링 값 출력
 parm : None
 retn : None
*/
VOID CGridPLC::UpdatePeriod()
{
	/* 현재 PLC 메모리 영역 값이 저장된 포인터 얻기 */
	LPG_PMRW pstRead = uvEng_ShMem_GetPLCExt();

	/* 화면 갱신 비활성화 */
	m_pGrid->SetRedraw(FALSE);

	switch (m_u8PageNo)
	{
	case 0x00	:	UpdatePage0(pstRead);	break;
	case 0x01	:	UpdatePage1(pstRead);	break;
	case 0x02	:	UpdatePage2(pstRead);	break;
	case 0x03	:	UpdatePage3(pstRead);	break;
	}

	/* 화면 갱신 활성화 */
	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate(FALSE);
}

// by sysandj : 안보혁수석님 sample 코드
VOID CGridPLC::UpdatePeriod(std::vector <std::vector <bool>>& vIOGroup)
{
	/* 현재 PLC 메모리 영역 값이 저장된 포인터 얻기 */
	LPG_PMRW pstRead = uvEng_ShMem_GetPLCExt();

	/* 화면 갱신 비활성화 */
	m_pGrid->SetRedraw(FALSE);

	switch (m_u8PageNo)
	{
	case 0x00:	UpdatePage0(pstRead);	break;
	case 0x01:	UpdatePage1(pstRead);	break;
	case 0x02:	UpdatePage2(pstRead);	break;
	case 0x03:	UpdatePage3(pstRead);	break;
	}

	int			nPage = 0;
	int			nRow = 1;
	COLORREF	clrItem[3] = { RGB(0xff, 0x00, 0x00), RGB(0xff, 0xff, 0xff), RGB(0xf2, 0xf2, 0xf2) };
	TCHAR		tzOffOn[2][8] = { L"OFF ", L"ON " };
	for (const auto &group : vIOGroup)
	{
		if (nPage == m_u8PageNo / 2)
		{
			for (const auto& io : group)
			{
				if (m_pGrid->GetRowCount() <= nRow)
				{
					continue;
				}

				if (io)
				{
					m_pGrid->SetItemBkColour(nRow, 1, clrItem[TRUE]);
					m_pGrid->SetItemText(nRow, 1, tzOffOn[TRUE]);
				}
				else
				{
					m_pGrid->SetItemBkColour(nRow, 1, clrItem[FALSE]);
					m_pGrid->SetItemText(nRow, 1, tzOffOn[FALSE]);
				}
				nRow++;
			}
		}

		nPage++;
	}

	/* 화면 갱신 활성화 */
	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate(FALSE);
}
// by sysandj : 안보혁수석님 sample 코드

/*
 desc : 현재 페이지의 다음 페이지 출력
 parm : None
 retn : None
*/
VOID CGridPLC::SetPageNext()
{
	if (m_u8PageNo > 6)	return;
	m_u8PageNo++;
}

/*
 desc : 현재 페이지의 이전 페이지 출력
 parm : None
 retn : None
*/
VOID CGridPLC::SetPagePrev()
{
	if (m_u8PageNo < 1)	return;
	m_u8PageNo--;
}

/*
 desc : 그리드 영역 갱신 (Page No. 0)
 parm : data	- [in]  PLC Memory 값이 저장된 구조체 포인터
 retn : None
*/
VOID CGridPLC::UpdatePage0(LPG_PMRW data)
{
	TCHAR tzPushPull[2][8]	= { L"PUSH ", L"PULL " };
	TCHAR tzOpenClose[2][8]	= { L"OPEN ", L"CLOSE " }, tzOffOn[2][8]	= { L"OFF ", L"ON " };;
	TCHAR tzLeakNormal[2][8]= { L"LEAK ", L"NORMAL "}, tzAlarmNormal[2][8] = { L"ALARM ", L"NORMAL " };
	UINT32 u32Index			= 1;
	COLORREF clrItem[3]		= { RGB(0xff, 0x00, 0x00), RGB(0xff, 0xff, 0xff), RGB(0xf2, 0xf2, 0xf2) };

	/* 페이지 전환 있을 때마다 1 번만 호출됨 */
	if (0x00 != m_u8LastPage)
	{
		m_pGrid->SetItemText(u32Index, 0, L"Main MC Power On/Off");						u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"EMS1 Front Side Push");						u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"EMS2/3 Right/Internal Side Push");			u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Safety Line Power (CP24S1, CP24S2)");		u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Chiller Power (ELCB101)");					u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Temperature Controller Power (ELCB102)");	u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Vacuum ControllerPower (ELCB103)");			u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Linear X Axis Power (CP101)");				u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Linear Y Axis Power (CP102)");				u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Linear CA1, CA2 Axis Power (CP103)");		u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"All SD2S Control Power (CP123)");			u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"MC2 Power (CP124)");						u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Optic. Power Line (CP104~CP113)");			u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Trigger Board Power Line (CP114,CP115)");	u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"UPS Power (CP116)");						u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Stepping Motor Drive Power (CP125)");		u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Area CAMERA Power (CP126)");				u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Bottom Left Side Cover1");					u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Bottom Left Side Cover2");					u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Bottom Left Side Cover3");					u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Bottom Front Cover");						u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Bottom Right Side Cover 1");				u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Bottom Right Side Cover 2");				u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Bottom Right Side Cover 3");				u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Bottom Rear Side Cover");					u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"TOP LEFT Side Cover");						u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"TOP RIGHT Side Cover");						u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"TOP REAR Side Cover");						u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Optic. Water");								u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Utility Box Water");						u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Chiller Alarm");							u32Index=1;
	}

	/* 데이터 값 출력 */
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_main_mc_power_onoff]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_main_mc_power_onoff]);			u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_ems1_left_side_push]);
	m_pGrid->SetItemText(u32Index,		1, tzPushPull[data->r_ems1_left_side_push]);		u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_ems2_right_side_push]);
	m_pGrid->SetItemText(u32Index,		1, tzPushPull[data->r_ems2_right_side_push]);		u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_safety_line_power]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_safety_line_power]);				u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_chiller_power]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_chiller_power]);					u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_temperature_controller_power]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_temperature_controller_power]);	u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_vacuum_controller_power]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_vacuum_controller_power]);		u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_linear_x_axis_power]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_linear_x_axis_power]);			u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_linear_y_axis_power]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_linear_y_axis_power]);			u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_linear_ca1_ca2_axis_power]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_linear_ca1_ca2_axis_power]);		u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_all_sd2s_control_power]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_all_sd2s_control_power]);		u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_mc2_power]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_mc2_power]);						u32Index++;
#if 0	/* 원래 5개 광학계가 설치되어 있어야 하지만, 현재 데모 장비 (2개만 설치됨)이기 때문에 에러 발생 */
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_photohead_power_line]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_photohead_power_line]);			u32Index++;
#else
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[1]);										u32Index++;
#endif
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_trigger_board_power_line]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_trigger_board_power_line]);		u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_ups_power]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_ups_power]);						u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_stepping_motor_drive_power]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_stepping_motor_drive_power]);	u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_area_camera_power]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_area_camera_power]);				u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_bottom_left_side_cover_1]);
	m_pGrid->SetItemText(u32Index,		1, tzOpenClose[data->r_bottom_left_side_cover_1]);	u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_bottom_left_side_cover_2]);
	m_pGrid->SetItemText(u32Index,		1, tzOpenClose[data->r_bottom_left_side_cover_2]);	u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_bottom_left_side_cover_3]);
	m_pGrid->SetItemText(u32Index,		1, tzOpenClose[data->r_bottom_left_side_cover_3]);	u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_bottom_front_cover]);
	m_pGrid->SetItemText(u32Index,		1, tzOpenClose[data->r_bottom_front_cover]);		u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_bottom_right_side_cover_1]);
	m_pGrid->SetItemText(u32Index,		1, tzOpenClose[data->r_bottom_right_side_cover_1]);	u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_bottom_right_side_cover_2]);
	m_pGrid->SetItemText(u32Index,		1, tzOpenClose[data->r_bottom_right_side_cover_2]);	u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_bottom_right_side_cover_3]);
	m_pGrid->SetItemText(u32Index,		1, tzOpenClose[data->r_bottom_right_side_cover_3]);	u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_bottom_rear_side_cover]);
	m_pGrid->SetItemText(u32Index,		1, tzOpenClose[data->r_bottom_rear_side_cover]);	u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_top_left_side_cover]);
	m_pGrid->SetItemText(u32Index,		1, tzOpenClose[data->r_top_left_side_cover]);		u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_top_right_side_cover]);
	m_pGrid->SetItemText(u32Index,		1, tzOpenClose[data->r_top_right_side_cover]);		u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_top_rear_side_cover]);
	m_pGrid->SetItemText(u32Index,		1, tzOpenClose[data->r_top_rear_side_cover]);		u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_photohead_water_leak]);
	m_pGrid->SetItemText(u32Index,		1, tzLeakNormal[data->r_photohead_water_leak]);		u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_utility_box_water_leak]);
	m_pGrid->SetItemText(u32Index,		1, tzLeakNormal[data->r_utility_box_water_leak]);	u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_chiller_alarm]);
	m_pGrid->SetItemText(u32Index,		1, tzAlarmNormal[data->r_chiller_alarm]);			u32Index++;

	/* 현재 페이지 번호 저장 */
	m_u8LastPage	= 0x00;
}

/*
 desc : 그리드 영역 갱신 (Page No. 0)
 parm : data	- [in]  PLC Memory 값이 저장된 구조체 포인터
 retn : None
*/
VOID CGridPLC::UpdatePage1(LPG_PMRW data)
{
	TCHAR tzOffOn[2][8]		= { L"OFF ", L"ON " }, tzPushPull[2][8] = { L"PUSH ", L"PULL " };
	TCHAR tzAlarmRun[2][8]	= { L"ALARM ", L"RUN " }, tzAlarmNormal[2][8] = { L"ALARM ", L"NORMAL " };
	TCHAR tzMaintAuto[2][8]	= { L"MAINT  ", L"AUTO " }, tzNoneStart[2][8] = { L"NONE ", L"START " };
	TCHAR tzCloseOpen[2][8]	= { L"CLOSE", L"OPEN " }, tzNoneDetect[2][8] = { L"NONE", L"DETECT "};
	TCHAR tzAtmVac[2][8]	= { L"ATM ", L"VAC " }, tzManualRemote[28][8] = { L"MANUAL ", L"REMOTE "};
	TCHAR tzStopRun[2][8]	= { L"STOP ", L"RUN " }, tzWaing[2][8] = { L"None", L"Waiting"};
	UINT32 u32Index			= 1;
	COLORREF clrItem[3]		= { RGB(0xff, 0x00, 0x00), RGB(0xff, 0xff, 0xff), RGB(0xf2, 0xf2, 0xf2) };

	/* 페이지 전환 있을 때마다 1 번만 호출됨 */
	if (0x01 != m_u8LastPage)
	{
		m_pGrid->SetItemText(u32Index, 0, L"Top Fan Alarm");						u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Vacuum Controller EMO Push");			u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Vacuum Controller FAN Power");			u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Vacuum Controller CC-LINK Status");		u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Vacuum Controller Inverter Alarm");		u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Camera1 Z Axis Alarm");					u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Camera2 Z Axis Alarm");					u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");					u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");					u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");					u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");					u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");					u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");					u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");					u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");					u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");					u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");					u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Auto / Maint Mode");					u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Start Button");							u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Shutter Status");						u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Glass Detecting Sensor");				u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Vacuum Status");						u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Chiller Status");						u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Temperature Status");					u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Vacuum Controller Running");			u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Vacuum Controller Mode");				u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Temp Controller Running");				u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Shutter Open/Close Waiting");			u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Vacuum VAC/ATM Waiting");				u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Signal Tower Green Lamp");				u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Signal Tower Yellow Lamp Flashing");	u32Index=1;
	}

	/* 데이터 값 출력 */
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2/*data->r_top_fan_alarm*/]);	/* DI 4 호기 (유트로닉스의 경우 예외)의 경우, Top Pan이 없는 상태 임 */
	m_pGrid->SetItemText(u32Index,		1, tzAlarmNormal[data->r_top_fan_alarm]);					u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_vacuum_controller_emo_push]);
	m_pGrid->SetItemText(u32Index,		1, tzPushPull[data->r_vacuum_controller_emo_push]);			u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_vacuum_controller_fan_power]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_vacuum_controller_fan_power]);			u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_vacuum_controller_cc_link_status]);
	m_pGrid->SetItemText(u32Index,		1, tzAlarmRun[data->r_vacuum_controller_cc_link_status]);	u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_vacuum_controller_inverter_alarm]);
	m_pGrid->SetItemText(u32Index,		1, tzAlarmNormal[data->r_vacuum_controller_inverter_alarm]);u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_camera1_z_axis_alarm]);
	m_pGrid->SetItemText(u32Index,		1, tzAlarmNormal[data->r_camera1_z_axis_alarm]);			u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_camera2_z_axis_alarm]);
	m_pGrid->SetItemText(u32Index,		1, tzAlarmNormal[data->r_camera2_z_axis_alarm]);			u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");													u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");													u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");													u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");													u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");													u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");													u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");													u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");													u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");													u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");													u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_auto_maint_mode]);
	m_pGrid->SetItemText(u32Index,		1, tzMaintAuto[data->r_auto_maint_mode]);					u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2/*data->r_start_button*/]);	/* DI 4호기의 경우, Start Button 사용하지 않는 상태 임) */
	m_pGrid->SetItemText(u32Index,		1, tzNoneStart[data->r_start_button]);						u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2/*data->r_shutter_status*/]);	/* Shutter Open / Close의 경우 알람 상태가 아님 */
	m_pGrid->SetItemText(u32Index,		1, tzCloseOpen[data->r_shutter_status]);					u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2/*data->r_glass_detecting_sensor*/]);	/* Glass 감지 여부가 알람 상태가 아님 */
	m_pGrid->SetItemText(u32Index,		1, tzNoneDetect[data->r_glass_detecting_sensor]);			u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2/*data->r_vacuum_status*/]);	/* Vacuum 동작 여부가 알람 상태가 아님 */
	m_pGrid->SetItemText(u32Index,		1, tzAtmVac[data->r_vacuum_status]);						u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_chiller_controller_running]);
	m_pGrid->SetItemText(u32Index,		1, tzStopRun[data->r_chiller_controller_running]);			u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_temperature_controller_running]);
	m_pGrid->SetItemText(u32Index,		1, tzStopRun[data->r_temperature_controller_running]);		u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_vacuum_controller_running]);
	m_pGrid->SetItemText(u32Index,		1, tzStopRun[data->r_vacuum_controller_running]);			u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_vacuum_controller_mode]);
	m_pGrid->SetItemText(u32Index,		1, tzManualRemote[data->r_vacuum_controller_mode]);			u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_temp_controller_running]);
	m_pGrid->SetItemText(u32Index,		1, tzStopRun[data->r_temp_controller_running]);				u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2/*data->r_shutter_open_close_waiting*/]);	/* Shutter Open / Close의 Waiting 여부가 알람 상태가 아님 */
	m_pGrid->SetItemText(u32Index,		1, tzWaing[data->r_shutter_open_close_waiting]);			u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2/*data->r_vacuum_vac_atm_waiting*/]);	/* Vacuum 시작 / 중지의 Waiting 여부가 알람 상태가 아님 */
	m_pGrid->SetItemText(u32Index,		1, tzWaing[data->r_vacuum_vac_atm_waiting]);				u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_signal_tower_green_lamp]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_signal_tower_green_lamp]);				u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[!data->r_signal_tower_yellow_lamp_flashing]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_signal_tower_yellow_lamp_flashing]);		u32Index++;

	/* 현재 페이지 번호 저장 */
	m_u8LastPage	= 0x01;
}

/*
 desc : 그리드 영역 갱신 (Page No. 0)
 parm : data	- [in]  PLC Memory 값이 저장된 구조체 포인터
 retn : None
*/
VOID CGridPLC::UpdatePage2(LPG_PMRW data)
{
	TCHAR tzOffOn[28][8]	= { L"OFF ", L"ON " }, tzWaing[2][8] = { L"None", L"Waiting"};
	TCHAR tzReadyBusy[2][8]	= { L"READY ", L"BUSY " }, tzNoneOn[2][8] = { L"NONE ", L"ON " };
	UINT32 u32Index			= 1;
	COLORREF clrItem[3]		= { RGB(0xff, 0x00, 0x00), RGB(0xff, 0xff, 0xff), RGB(0xf2, 0xf2, 0xf2) };

	/* 페이지 전환 있을 때마다 1 번만 호출됨 */
	if (0x02 != m_u8LastPage)
	{
		m_pGrid->SetItemText(u32Index, 0, L"Signal Tower Red Lamp Flashing");				u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Signal Tower Buzzer 1");						u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Internal Monitoering Temp (℃)");				u32Index++;   
		m_pGrid->SetItemText(u32Index, 0, L"Vacuum Controller Current Pressure (kPa)");		u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Vacuum Controller CurrenT Freq. (Hz)");			u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Temp Controller Current Temp  (℃)");			u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Temp Controller Current Setting Temp  (℃)");	u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");							u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");							u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");							u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");							u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");							u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");							u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");							u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");							u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");							u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");							u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");							u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Camera1 Z Axis Status");						u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Camera1 Z Axis P_Limit Position");				u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Camera1 Z Axis N_Limit Position");				u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Camera1 Z Axis Position");						u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Camera1 Z Axis Homing Request");				u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Camera1 Z Axis Current Position (mm)");			u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Camera1 Z Axis Alarm Code");					u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Camera1 Z Axis Home Request Alarm");			u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Camera2 Z Axis Status");						u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Camera2 Z Axis P_Limit Position");				u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Camera2 Z Axis N_Limit Position");				u32Index++;
 		m_pGrid->SetItemText(u32Index, 0, L"Camera2 Z Axis Position");						u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Camera2 Z Axis Homing Request");				u32Index=1;
	}
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[!data->r_signal_tower_red_lamp_flashing]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_signal_tower_red_lamp_flashing]);					u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[!data->r_signal_tower_buzzer_1]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_signal_tower_buzzer_1]);								u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[1]);
	m_pGrid->SetItemFloat(u32Index,		1, FLOAT(data->r_internal_monitoring_temp/1000.0f), 3, TRUE);			u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[1]);
	m_pGrid->SetItemText(u32Index,		1, INT(data->r_vacuum_controller_current_pressure));					u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[1]);
	m_pGrid->SetItemFloat(u32Index,		1, FLOAT(data->r_vacuum_controller_current_frequency/100.0f), 2, TRUE);	u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[1]);
	m_pGrid->SetItemFloat(u32Index,		1, FLOAT(data->r_temp_controller_current_temp/100.0f), 2, TRUE);		u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[1]);
	m_pGrid->SetItemFloat(u32Index,		1, FLOAT(data->r_temp_controller_current_setting_temp/100.0f), 2, TRUE);u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");																u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");																u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");																u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");																u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");																u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");																u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");																u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");																u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");																u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");																u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");																u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[!data->r_camera1_z_axis_status]);
	m_pGrid->SetItemText(u32Index,		1, tzReadyBusy[data->r_camera1_z_axis_status]);							u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_camera1_z_axis_p_limit_position]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_camera1_z_axis_p_limit_position]);					u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_camera1_z_axis_n_limit_position]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_camera1_z_axis_n_limit_position]);					u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[1]);
	m_pGrid->SetItemText(u32Index,		1, tzNoneOn[data->r_camera1_z_axis_position]);							u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[!data->r_camera1_z_axis_homing_request]);
	m_pGrid->SetItemText(u32Index,		1, tzNoneOn[data->r_camera1_z_axis_homing_request]);					u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[1]);
	m_pGrid->SetItemFloat(u32Index,		1, FLOAT(data->r_camera1_z_axis_position_display / 10000.0f), 3, TRUE);	u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[1]);
	m_pGrid->SetItemText(u32Index,		1, data->r_camera1_z_axis_alarm_code);									u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[1]);
	m_pGrid->SetItemText(u32Index,		1, data->r_camera1_z_axis_homing_request);								u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[!data->r_camera2_z_axis_status]);
	m_pGrid->SetItemText(u32Index,		1, tzReadyBusy[data->r_camera2_z_axis_status]);							u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_camera2_z_axis_p_limit_position]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_camera2_z_axis_p_limit_position]);					u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_camera2_z_axis_n_limit_position]);
	m_pGrid->SetItemText(u32Index,		1, tzOffOn[data->r_camera2_z_axis_n_limit_position]);					u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[1]);
	m_pGrid->SetItemText(u32Index,		1, tzNoneOn[data->r_camera2_z_axis_position]);							u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[!data->r_camera2_z_axis_homing_request]);
	m_pGrid->SetItemText(u32Index,		1, tzNoneOn[data->r_camera2_z_axis_homing_request]);					u32Index++;

	/* 현재 페이지 번호 저장 */
	m_u8LastPage	= 0x02;
}

/*
 desc : 그리드 영역 갱신 (Page No. 0)
 parm : data	- [in]  PLC Memory 값이 저장된 구조체 포인터
 retn : None
*/
VOID CGridPLC::UpdatePage3(LPG_PMRW data)
{
	TCHAR tzReadyBusy[2][8]	= { L"READY ", L"BUSY " }, tzOffOn[2][8] = { L"ON ", L"OFF " };
	TCHAR tzNoneOn[2][8]	= { L"NONE ", L"ON " };
	TCHAR tzAlarm[2][8]		= { L"ALARM ", L"NORMAL " }, tzRun[2][8] = { L"STOP ", L"RUN " };
	UINT32 u32Index			= 1;
	COLORREF clrItem[3]		= { RGB(0xff, 0x00, 0x00), RGB(0xff, 0xff, 0xff), RGB(0xf2, 0xf2, 0xf2) };

	/* 페이지 전환 있을 때마다 1 번만 호출됨 */
	if (0x03 != m_u8LastPage)
	{
		m_pGrid->SetItemText(u32Index, 0, L"Camera2 Z Axis Current Position (mm)");			u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Camera2 Z Axis Alarm Code");					u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Camera2 Z Axis Home Request Alarm");			u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Temp Controller Box Over Temp 1");				u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Temp Controller Box Over Temp 2");				u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Temp Controller Compressure Over Current");		u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Temp Controller Compressure Over Pressure");	u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Temp Controller Condenser Motor Over Current");	u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Temp Controller Fan Alarm");					u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Temp Controller Power Reverse Phase");			u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");							u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Temp Controller Current Temp");					u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Temp Controller Current Setting Temp");			u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Camera 1 Z Axis Home Request Alarm");			u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Camera 2 Z Axis Home Request Alarm");			u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");							u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");							u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");							u32Index++;
		m_pGrid->SetItemText(u32Index, 0, L"Unused address area");							u32Index=1;
	}
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[1]);
	m_pGrid->SetItemFloat(u32Index,		1, FLOAT(data->r_camera2_z_axis_position_display / 10000.0f), 3, TRUE);		u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[1]);
	m_pGrid->SetItemText(u32Index,		1, data->r_camera2_z_axis_alarm_code);										u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[1]);
	m_pGrid->SetItemText(u32Index,		1, data->r_camera2_z_axis_homing_request);									u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_temp_controller_box_over_temp_1]);
	m_pGrid->SetItemText(u32Index,		1, tzAlarm[data->r_temp_controller_box_over_temp_1]);						u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_temp_controller_box_over_temp_2]);
	m_pGrid->SetItemText(u32Index,		1, tzAlarm[data->r_temp_controller_box_over_temp_2]);						u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_temp_controller_compressure_over_current]);
	m_pGrid->SetItemText(u32Index,		1, tzAlarm[data->r_temp_controller_compressure_over_current]);				u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_temp_controller_compressure_over_pressure]);
	m_pGrid->SetItemText(u32Index,		1, tzAlarm[data->r_temp_controller_compressure_over_pressure]);				u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_temp_controller_condenser_motor_over_current]);
	m_pGrid->SetItemText(u32Index,		1, tzAlarm[data->r_temp_controller_condenser_motor_over_current]);			u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_temp_controller_fan_alarm]);
	m_pGrid->SetItemText(u32Index,		1, tzAlarm[data->r_temp_controller_fan_alarm]);								u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_temp_controller_power_reverse_phase]);
	m_pGrid->SetItemText(u32Index,		1, tzAlarm[data->r_temp_controller_power_reverse_phase]);					u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");																	u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[1]);
	m_pGrid->SetItemFloat(u32Index,		1, FLOAT(data->r_temp_controller_current_temp / 100.0f), 2, TRUE);			u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[1]);
	m_pGrid->SetItemFloat(u32Index,		1, FLOAT(data->r_temp_controller_current_setting_temp / 100.0f), 2, TRUE);	u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_camera1_z_axis_home_request_alarm]);
	m_pGrid->SetItemText(u32Index,		1, tzAlarm[data->r_camera1_z_axis_home_request_alarm]);						u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[data->r_camera2_z_axis_home_request_alarm]);
	m_pGrid->SetItemText(u32Index,		1, tzAlarm[data->r_camera2_z_axis_home_request_alarm]);						u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");																	u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");																	u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");																	u32Index++;
	m_pGrid->SetItemBkColour(u32Index,	1, clrItem[2]);
	m_pGrid->SetItemText(u32Index,		1, L"");																	u32Index++;

	/* 현재 페이지 번호 저장 */
	m_u8LastPage	= 0x03;
}
