
/*
 desc : Main Dialog
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgMain.h"

#include "MainThread.h"

#define ONE_SCAN_WIDTH	20.6f	/* unit : mm */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : 자신의 윈도 ID, 부모 윈도 객체
 retn : None
*/
CDlgMain::CDlgMain(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgMain::IDD, parent)
{
	m_bDrawBG		= 0x01;
	m_hIcon			= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_u8ViewMode	= 0x00;
	m_pMainThread	= NULL;
	m_bSetThick		= FALSE;
	memset(&m_stMeas, 0x00, sizeof(STG_ACCS));
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgMain::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;
	/* group box */
	u32StartID	= IDC_MAIN_GRP_VIEW;
	for (i=0; i<14; i++)	DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* For Image */
	u32StartID	= IDC_MAIN_PIC_VIEW;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_pic_ctl[i]);
	/* Check - Normal */
	u32StartID	= IDC_MAIN_CHK_MC2_DRV_0;
	for (i=0; i<10; i++)	DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
	/* Button - Normal */
	u32StartID	= IDC_MAIN_BTN_MC2_MOVE_UP;
	for (i=0; i<17; i++)	DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Static - Normal */
	u32StartID	= IDC_MAIN_TXT_MC2_MOVE;
	for (i=0; i<26; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* edit - float */
	u32StartID	= IDC_MAIN_EDT_MC2_DIST;
	for (i=0; i<18; i++)	DDX_Control(dx, u32StartID+i,	m_edt_flt[i]);
	/* Edit - integer */
	u32StartID	= IDC_MAIN_EDT_COUNT_ROWS_CT;
	for (i=0; i<7; i++)		DDX_Control(dx, u32StartID+i,	m_edt_int[i]);
	/* Edit - text */
	u32StartID	= IDC_MAIN_EDT_WORK_TIME;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_edt_txt[i]);
}

BEGIN_MESSAGE_MAP(CDlgMain, CMyDialog)
	ON_MESSAGE(WM_MAIN_THREAD,	OnMsgMainThread)
	ON_WM_SYSCOMMAND()
 	ON_CONTROL_RANGE(BN_CLICKED,IDC_MAIN_BTN_MC2_MOVE_UP,	IDC_MAIN_BTN_EXIT,			OnBtnClick)
 	ON_CONTROL_RANGE(BN_CLICKED,IDC_MAIN_CHK_MC2_DRV_0,		IDC_MAIN_CHK_ACAM_NO,		OnChkClick)
	ON_CONTROL_RANGE(EN_CHANGE,	IDC_MAIN_EDT_COUNT_ROWS_CT,	IDC_MAIN_EDT_COUNT_COLS_CR,	OnEnChangeRebuild)
	ON_CONTROL_RANGE(EN_CHANGE,	IDC_MAIN_EDT_SKIP_ROW,		IDC_MAIN_EDT_PERIOD_COLS,	OnEnChangeRebuild)
END_MESSAGE_MAP()

/*
 desc : 시스템 명령어 제어
 parm : id		- 선택된 항목 ID 값
		lparam	- ???
 retn : 1 - 성공 / 0 - 실패
*/
VOID CDlgMain::OnSysCommand(UINT32 id, LPARAM lparam)
{
//	switch (id)
//	{
//	}

	CMyDialog::OnSysCommand(id, lparam);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::OnInitDlg()
{
	UINT32 u32Size	= 0, i = 0, u32Speed = 100/*NORMAL_THREAD_SPEED*/;

	/* 라이브러리 초기화 */
	if (!InitLib())		return FALSE;
	InitCtrl();		/* 윈도 컨트롤 초기화 */
	LoadParam();	/* 가장 최근에 설정된 값 불러오기 */
	InitGrid();		/* 그리드 초기화 */
	InitSetup();

	/* Main Thread 생성 (!!! 중요. InitLib 가 성공하면, 반드시 생성해야 함 !!!) */
	m_pMainThread	= new CMainThread(m_hWnd);
	ASSERT(m_pMainThread);
	if (uvEng_GetConfig()->IsRunDemo())	u32Speed = 10;
	if (!m_pMainThread->CreateThread(0, NULL, u32Speed))
	{
		delete m_pMainThread;
		m_pMainThread	= NULL;
	}

	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgMain::OnExitDlg()
{
	UINT32 i	= 0;
	/* 기본 감시 스레드 메모리 해제 */
	if (m_pMainThread)
	{
		m_pMainThread->Stop();
		while (m_pMainThread->IsBusy())	m_pMainThread->Sleep();
		delete m_pMainThread;
		m_pMainThread	= NULL;
	}
	/* 라이브러리 해제 */
	CloseLib();
	/* Grid Control 메모리 해제*/
	if (m_pGridMeas)
	{
		for (i=0; i<2; i++)	m_pGridMeas[i].DestroyWindow();
		delete [] m_pGridMeas;
	}
	/* 행과 열의 메모리 해제 */
	m_stMeas.FreeData();
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgMain::OnPaintDlg(CDC *dc)
{
	ENG_VCCM enMode	= uvEng_Camera_GetCamMode();
	RECT rDraw;

	/* 이미지가 출력될 윈도 영역 */
	m_pic_ctl[0].GetWindowRect(&rDraw);
	ScreenToClient(&rDraw);

	/* 현재 Calibration Mode인 경우 */
	if (enMode == ENG_VCCM::en_cali_mode)
	{
		uvEng_Camera_DrawCaliBitmap(dc->m_hDC, rDraw);
	}
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgMain::OnResizeDlg()
{
}

/*
 desc : Library Init
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::InitLib()
{
	return uvEng_Init(ENG_ERVM::en_cali_meas);
}

/*
 desc : Library Close
 parm : None
 retn : None
*/
VOID CDlgMain::CloseLib()
{
	uvEng_Close();
}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgMain::InitCtrl()
{
	INT32 i;

	/* group box */
	for (i=0; i<14; i++)	m_grp_ctl[i].SetFont(&g_lf);

	/* text - normal */
	for (i=0; i<26; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* button - normal */
	for (i=0; i<17; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
	}
	/* checkbox */
	for (i=0; i<10; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf);
		m_chk_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}

	/* edit - text */
	for (i=0; i<2; i++)
	{
		m_edt_txt[i].SetEditFont(&g_lf);
		m_edt_txt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_txt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_int[i].SetInputType(ENM_DITM::en_string);
		m_edt_txt[i].SetReadOnly(TRUE);
	}
	/* edit - integer */
	for (i=0; i<7; i++)
	{
		m_edt_int[i].SetEditFont(&g_lf);
		m_edt_int[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_int[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_int[i].SetInputType(ENM_DITM::en_int8);
	}
	/* edit - float */
	for (i=0; i<18; i++)
	{
		m_edt_flt[i].SetEditFont(&g_lf);
		m_edt_flt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_flt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_flt[i].SetInputType(ENM_DITM::en_float);
		if ((i >= 2 && i <= 8) || (i >= 10 && i <= 11))
		{
			m_edt_flt[i].SetReadOnly(TRUE);
			m_edt_flt[i].SetMouseClick(FALSE);
		}
	}
}

/*
 desc : 초기 값 설정
 parm : None
 retn : None
*/
VOID CDlgMain::InitSetup()
{
	UINT32 i	= 0;
	LPG_CMSI pstMC2	= &uvEng_GetConfig()->mc2_svc;
	LPG_CSCI pstCAM	= &uvEng_GetConfig()->set_cams;

	/* 일단 모두 Disable */
	for (i=0x00; i<MAX_MC2_DRIVE; i++)	m_chk_ctl[i].EnableWindow(FALSE);
#ifdef _DEBUG
	/* for MC2 - 체크 버튼 활성화 처리 */
	for (i=0; i<pstMC2->drive_count; i++)
	{
		m_chk_ctl[pstMC2->axis_id[i]].EnableWindow(TRUE);
	}
#endif

	/* 실행 환경에 맞는 검색 모델 등록 */
	SetRegistModel();
}

/*
 desc : 그리드 컨트롤 생성 - Step XY
 parm : None
 retn : None
*/
VOID CDlgMain::InitGrid()
{
	UINT32 dwStyle	= WS_CHILD | WS_TABSTOP | WS_VISIBLE, i = 0;

	GV_ITEM stGV = { NULL };
	CRect rGrid[2];

	// 현재 윈도 Client 크기
	m_grp_ctl[1].GetWindowRect(rGrid[0]);
	ScreenToClient(rGrid[0]);
	rGrid[0].left	+= 15;
	rGrid[0].right	-= 15;
	rGrid[0].top	+= 24;
	rGrid[0].bottom	-= 15;
	rGrid[1].bottom	= rGrid[0].bottom;
	rGrid[0].bottom	= rGrid[0].top + (rGrid[0].Height() / 2 - 2);
	rGrid[1].left	= rGrid[0].left;
	rGrid[1].right	= rGrid[0].right;
	rGrid[1].top	+= rGrid[0].bottom + 4;

	// 그리드 컨트롤 기본 공통 속성
	stGV.mask	= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr= RGB(32, 32, 32);
	/* 객체 생성 */
	m_pGridMeas	= new CGridCtrl[2];
	ASSERT(m_pGridMeas);
	for (i=0; i<2; i++)
	{
		m_pGridMeas[i].SetModifyStyleEx(WS_EX_STATICEDGE);
		m_pGridMeas[i].SetDrawScrollBarVert(TRUE);
		m_pGridMeas[i].SetDrawScrollBarHorz(TRUE);
		if (!m_pGridMeas[i].Create(rGrid[i], this, IDC_MAIN_GRID_OFFSET_X+i, dwStyle))
		{
			delete m_pGridMeas;
			m_pGridMeas = NULL;
			return;
		}
		/* 객체 기본 설정 */
		m_pGridMeas[i].SetLogFont(g_lf);
		m_pGridMeas[i].SetRowCount(1);
		m_pGridMeas[i].SetColumnCount(1);
		m_pGridMeas[i].SetRowHeight(0, 25);
		m_pGridMeas[i].SetColumnWidth(0, 40);
		m_pGridMeas[i].SetBkColor(RGB(255, 255, 255));
		m_pGridMeas[i].SetFixedColumnSelection(0);

		m_pGridMeas[i].SetBaseGridProp(1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	}

	/* Updates the grid control */
	ReBuildGridXY();
}

/*
 desc : 이미지 뷰 영역 갱신
 parm : None
 retn : None
*/
VOID CDlgMain::InvalidateView()
{
	CRect rView;
	m_pic_ctl[0].GetWindowRect(rView);
	ScreenToClient(rView);
	InvalidateRect(rView, TRUE);
}

/*
 desc : Main Thread에서 발생된 이벤트 처리
 parm : wparam	- [in]  Event ID
		lparam	- [in]  Event Data
 retn : 0L
*/
LRESULT CDlgMain::OnMsgMainThread(WPARAM wparam, LPARAM lparam)
{
	/* 주기적으로 호출함 */
	UpdatePeriod();
	/* Drive Position */
	UpdateDrvPos();
	/* Update Cali XY */
	switch (wparam)
	{
	case MSG_ID_WORK_COMP	:	WorkComplete();
	case MSG_ID_MEAS_MARK	:
	case MSG_ID_MEAS_CALI	:
	case MSG_ID_EXAM_SHUT	:	InvalidateView();
		switch (wparam)
		{
		case MSG_ID_MEAS_CALI	:
		case MSG_ID_EXAM_SHUT	:	UpdateCaliXY();
									UpdateWorkTime();	break;
		}
	}

#if 1
	/* 모션 컨트롤이 움직이는 여부에 따라 버튼 Enable / Disable */
	EnableCtrl();
#endif
	return 0L;
}

/*
 desc : 부모 스레드에 의해 주기적으로 호출됨
 parm : None
 retn : None
*/
VOID CDlgMain::UpdatePeriod()
{
	/* Grabbed Image Display */
	if (m_chk_ctl[8].GetCheck())	/* Live Mode */
	{
		UpdateLiveView();
		if (m_u8ViewMode == 0x00)	m_u8ViewMode	= 0x01;	/* Live Mode 저장 */
	}
	else	/* Grabbed Mode */
	{
		/* 방금 전까지 Live Mode였다가 Grab Mode로 바뀐 경우 */
		if (0x01 == m_u8ViewMode && !m_pMainThread->IsRunWork())
		{
			InvalidateView();
			m_u8ViewMode	= 0x00;	/* Grab Mode 저장 */
		}
	}
}

/*
 desc : Drive Position 출력
 parm : None
 retn : TRUE or FALSE
*/
VOID CDlgMain::UpdateDrvPos()
{
	m_edt_flt[2].SetTextToNum(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x),		4, TRUE);
	m_edt_flt[3].SetTextToNum(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y),		4, TRUE);
	m_edt_flt[4].SetTextToNum(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam1),	4, TRUE);
	m_edt_flt[5].SetTextToNum(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam2),	4, TRUE);
	m_edt_flt[16].SetTextToNum(uvCmn_MCQ_GetACamCurrentPosZ(GetCheckACam()),	4, TRUE);
}

/*
 desc : Align Camera Calibration X / Y 갱신
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateCaliXY()
{
	UINT8 u8Color[2]	= {NULL};
	INT32 i32Row		= 1, i32Col = 1;
	DOUBLE dbOffsetX	= 0.0f, dbOffsetY = 0.0f;
	TCHAR tzValue[2][32]= {NULL};
	COLORREF clrFg[3]	= { RGB(64, 64, 64), RGB(0, 0, 255), RGB(255, 0, 0) };
	COLORREF clrBk		= RGB(255, 255, 192);

	/* 현재 위치 얻기 */
	m_pMainThread->GetCurRowCol(i32Row, i32Col);
	i32Row	+= 1;
	i32Col	+= 1;

	/* 오차 값 얻기 */
	m_pMainThread->GetCurOffset(dbOffsetX, dbOffsetY);

	/* 단위 (오차)를 um 로 변경 */
	dbOffsetX	= ROUNDED(dbOffsetX * 1000.0f, 1);
	dbOffsetY	= ROUNDED(dbOffsetY * 1000.0f, 1);
	swprintf_s(tzValue[0], 32, L"%+.1f", dbOffsetX);
	swprintf_s(tzValue[1], 32, L"%+.1f", dbOffsetY);
	/* 색상 지정 */
	if (abs(dbOffsetX) > m_edt_flt[17].GetTextToDouble())
	{
		if (dbOffsetX >= 0.0f)	u8Color[0] = 0x01;
		else					u8Color[0] = 0x02;
	}
	if (abs(dbOffsetY) > m_edt_flt[17].GetTextToDouble())
	{
		if (dbOffsetY >= 0.0f)	u8Color[1] = 0x01;
		else					u8Color[1] = 0x02;
	}
	/* 그리드 영역에 값 갱신 (단위: um) */
	m_pGridMeas[0].SetItemText(i32Row, i32Col, tzValue[0]);
	m_pGridMeas[1].SetItemText(i32Row, i32Col, tzValue[1]);
	m_pGridMeas[0].SetItemFgColour(i32Row, i32Col, clrFg[u8Color[0]]);
	m_pGridMeas[1].SetItemFgColour(i32Row, i32Col, clrFg[u8Color[1]]);
	m_pGridMeas[0].SetItemBkColour(i32Row, i32Col, clrBk);
	m_pGridMeas[1].SetItemBkColour(i32Row, i32Col, clrBk);
	m_pGridMeas[0].InvalidateRowCol(i32Row, i32Col);
	m_pGridMeas[1].InvalidateRowCol(i32Row, i32Col);
}

/*
 desc : Align Camera Calibration X / Y 갱신
 parm : row	- [in]  강제로 값을 넣어야할 위치 지정 (자동인 경우, 0)
		col	- [in]  강제로 값을 넣어야할 위치 지정 (자동인 경우, 0)
 retn : None
*/
VOID CDlgMain::UpdateCaliXY(INT32 row, INT32 col, LPG_ACGR result)
{
	/* 그리드 영역에 값 갱신 (단위: um) */
	m_pGridMeas[0].SetItemDouble(row, col, result->move_mm_x*1000.0f, 1);	/* mm -> um */
	m_pGridMeas[1].SetItemDouble(row, col, result->move_mm_y*1000.0f, 1);	/* mm -> um */
	m_pGridMeas[0].InvalidateRowCol(row, col);
	m_pGridMeas[1].InvalidateRowCol(row, col);
}

/*
 desc : Align Camera Calibration X / Y 갱신
 parm : row	- [in]  강제로 값을 넣어야할 위치 지정 (자동인 경우, 0)
		col	- [in]  강제로 값을 넣어야할 위치 지정 (자동인 경우, 0)
 retn : None
*/
VOID CDlgMain::UpdateCaliXY(INT32 row, INT32 col, DOUBLE offset_x, DOUBLE offset_y)
{
	/* 그리드 영역에 값 갱신 (단위: um) */
	m_pGridMeas[0].SetItemDouble(row, col, offset_x*1000.0f, 1);	/* mm -> um */
	m_pGridMeas[1].SetItemDouble(row, col, offset_y*1000.0f, 1);	/* mm -> um */
	m_pGridMeas[0].InvalidateRowCol(row, col);
	m_pGridMeas[1].InvalidateRowCol(row, col);
}

/*
 desc : 일반 버튼 이벤트 처리
 parm : id	- [in]  Click된 버튼 ID
 retn : None
*/
VOID CDlgMain::OnBtnClick(UINT32 id)
{
	switch (id)
	{
	case IDC_MAIN_BTN_STAGE_X		:
	case IDC_MAIN_BTN_STAGE_Y		:
	case IDC_MAIN_BTN_ACAM_X		:	SetMotionPos(id);					break;

	case IDC_MAIN_BTN_EXIT			:	PostMessage(WM_CLOSE, 0, 0L);		break;
	case IDC_MAIN_BTN_MC2_RESET		:	MC2Recovery();						break;
	case IDC_MAIN_BTN_ACAM_Z_MOVE	:	ACamMoveAbs();						break;
	case IDC_MAIN_BTN_MOVE_UNLOAD	:	UnloadPosition();					break;
	case IDC_MAIN_BTN_SAVE_OFFSET	:	SaveAsExcelFile();					break;
	case IDC_MAIN_BTN_MC2_MOVE_UP	:	MotionMove(ENG_MDMD::en_move_up);	break;
	case IDC_MAIN_BTN_MC2_MOVE_DN	:	MotionMove(ENG_MDMD::en_move_down);	break;
	case IDC_MAIN_BTN_WORK_REBUILD	:	ReBuildGridXY();					break;
	case IDC_MAIN_BTN_MOVE_MEASURE	:	InitMeasureMove();					break;
	case IDC_MAIN_BTN_SAVE_MEASURE	:	SaveMeasureMove();					break;
	case IDC_MAIN_BTN_WORK_START	:	StartCalibration();					break;
	case IDC_MAIN_BTN_WORK_STOP		:	StopCalibration();					break;
	case IDC_MAIN_BTN_TEST_MATCH	:	RunTestMatch();						break;
	case IDC_MAIN_BTN_THICK_DOF		:	SetDofFilmThick();					break;
	}
}

/*
 desc : 체크 버튼 이벤트 처리
 parm : id	- [in]  Click된 버튼 ID
 retn : None
*/
VOID CDlgMain::OnChkClick(UINT32 id)
{
	switch (id)
	{
	case IDC_MAIN_CHK_LIVE_VIEW	:	SetLiveView();		break;
	case IDC_MAIN_CHK_ACAM_NO	:	ChangeACamNo();		break;
	}
}

/*
 desc : This is an event that occurs when the value of edit box changes
 parm : id	- [in]  ID of control where the event occurred
 retn : None
*/
VOID CDlgMain::OnEnChangeRebuild(UINT32 id)
{
	ReBuildGridXY();
}

/*
 desc : 그리드 영역 재설계
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::ReBuildGridXY()
{
	UINT8 i, j, k;
	TCHAR tzTitle[2][8]	= { L"X", L"Y" };
	UINT16 u16Row[2], u16Col[2];
	DOUBLE dbPeriodRow, dbPeriodCol, dbSkipRow, dbSkipCol;
	INT32 i32Rows, i32Cols;
	GV_ITEM stGV	= { NULL };

	/* Because of event message in the edit box */
	if (!m_pGridMeas)	return FALSE;

	m_edt_txt[0].SetTextToStr(L"0 m 0 s", TRUE);
	m_edt_txt[1].SetTextToStr(L"0.000 %", TRUE);
	/* 현재 설정 값을 전역 변수에 저장 */
	u16Row[0]	= (UINT16)m_edt_int[0].GetTextToNum();	/* Corner : Row count (Top) */
	u16Col[0]	= (UINT16)m_edt_int[1].GetTextToNum();	/* Corner : Col count (Left) */
	u16Row[1]	= (UINT16)m_edt_int[2].GetTextToNum();	/* Corner : Row count (Bottom) */
	u16Col[1]	= (UINT16)m_edt_int[3].GetTextToNum();	/* Corner : Col count (Right) */
	/* 중간에 측정되지 않는 영역 Skip 하는 크기 설정 */
	dbSkipRow	= m_edt_flt[12].GetTextToDouble();
	dbSkipCol	= m_edt_flt[13].GetTextToDouble();
	/* 측정을 위한 Matrix (행과 열) 위치 (간격) 정보 (단위: mm) */
	dbPeriodRow	= m_edt_flt[14].GetTextToDouble();
	dbPeriodCol	= m_edt_flt[15].GetTextToDouble();
	/* 행과 열의 개수가 최소 2개 이상은 있어야 됨 */
	i32Rows		= INT32(u16Row[0] + u16Row[1]);
	i32Cols		= INT32(u16Col[0] + u16Col[1]);
	if (u16Row[0] < 2 || u16Row[1] < 2 || u16Col[0] < 2 || u16Col[1] < 2 ||
		dbSkipCol <= 0.0f || dbSkipRow <= 0.0f)
	{
		/* 데이터 영역 모두 제거 */
		m_pGridMeas[0].DeleteNonFixedRows();	
		m_pGridMeas[1].DeleteNonFixedRows();	
		return FALSE;
	}
	/* 행과 열의 개수가 너무 많은 경우 */
	if (i32Rows > uvEng_GetConfig()->acam_cali.max_rows_count ||
		i32Cols > uvEng_GetConfig()->acam_cali.max_cols_count)
	{
		AfxMessageBox(L"Too many rows and columns", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	for (k=0; k<2; k++)
	{
		/* 그리드 영역 갱신 비활성화 */
		m_pGridMeas[k].SetRedraw(FALSE);
		/* 데이터 영역 모두 제거 */
		m_pGridMeas[k].DeleteNonFixedRows();	
		/* 행과 열의 개수 설정 */
		m_pGridMeas[k].SetRowCount(i32Rows+1);
		m_pGridMeas[k].SetColumnCount(i32Cols+1);
		/* 타이틀 설정 */
		stGV.mask	= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
		stGV.crFgClr= RGB(32, 32, 32);
		stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
		stGV.crBkClr= RGB(214, 214, 214);
		stGV.row	= 0;
		stGV.col	= 0;
		stGV.strText= tzTitle[k];
		stGV.lfFont	= g_lf;
		m_pGridMeas[k].SetItem(&stGV);
		/* 타이틀 행과 열 설정 */
		for (i=1,stGV.col=0; i<=i32Rows; i++)
		{
			stGV.row	= i;
			stGV.strText.Format(L"%02u", i);
			m_pGridMeas[k].SetItem(&stGV);
			m_pGridMeas[k].SetRowHeight(i, 24);
		}
		for (i=1,stGV.row=0; i<=i32Cols; i++)
		{
			stGV.col	= i;
			stGV.strText.Format(L"%02u", i);
			m_pGridMeas[k].SetItem(&stGV);
			m_pGridMeas[k].SetColumnWidth(i, 38);
		}

		/* 본문 행과 열 설정 */
		stGV.mask	= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
		stGV.crFgClr= RGB(16, 16, 16);
		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.crBkClr= RGB(255, 255, 255);
		for (i=1; i<=i32Rows; i++)
		{
			for (j=1; j<=i32Cols; j++)
			{
				stGV.row	= i;
				stGV.col	= j;
				stGV.strText.Format(L"+0.0");
				m_pGridMeas[k].SetItem(&stGV);
			}
		}

		/* 그리드 영역 갱신 활성화 */
		m_pGridMeas[k].SetRedraw(TRUE);
		m_pGridMeas[k].Invalidate(FALSE);
	}

	/* 노광 면적 계산 */
	DOUBLE dbAreaW	= dbSkipCol + dbPeriodCol * DOUBLE(u16Col[0] + u16Col[1] - 2);
	DOUBLE dbAreaH	= dbSkipRow + dbPeriodRow * DOUBLE(u16Row[0] + u16Row[1] - 2);
	m_edt_flt[10].SetTextToNum(dbAreaH, 4);	/* Height */
	m_edt_flt[11].SetTextToNum(dbAreaW, 4);	/* Width */

	return TRUE;
}

/*
 desc : Align Camera 번호 변경
 parm : None
 retn : None
*/
VOID CDlgMain::ChangeACamNo()
{
	TCHAR tzNum[8]	= {NULL};

	m_chk_ctl[9].GetWindowText(tzNum, 8);
	if (0 == wcscmp(tzNum, L"1"))	m_chk_ctl[9].SetWindowText(L"2");
	else							m_chk_ctl[9].SetWindowText(L"1");
	/* 모델 다시 등록 */
	SetRegistModel();
	/* 카메라 마다 부여된 측정 위치 적재 */
	LoadMeasPos();
}

/*
 desc : 현재 선택된 얼라인 카메라 번호 반환 (1 or 2)
 parm : None
 retn : 1 or 2
*/
UINT8 CDlgMain::GetCheckACam()
{
	TCHAR tzNum[8]	= {NULL};

	m_chk_ctl[9].GetWindowText(tzNum, 8);
	return (UINT8)_wtoi(tzNum);
}

/*
 desc : Live Mode 설정
 parm : None
 retn : None
*/
VOID CDlgMain::SetLiveView()
{
	BOOL bSucc		= FALSE;
	UINT32 u32OnOff	= 0x00000000;

	/* 현재 채크된 상태이면 */
	if (m_chk_ctl[8].GetCheck())	/* Live View */
	{
		/* Align Camera is Live Mode */
		uvEng_Camera_SetCamMode(ENG_VCCM::en_live_mode);
		/* Trigger & Strobe : Enabled */
		bSucc = uvEng_Trig_ReqTriggerStrobe(TRUE);
		if (bSucc)
		{
			bSucc = uvEng_Trig_ReqEncoderLive(GetCheckACam());
		}
	}
	else
	{
		/* Trigger Disabled */
		bSucc = uvEng_Trig_ReqEncoderOutReset();
		/* Trigger & Strobe : Disabled */
		if (bSucc)
		{
			bSucc = uvEng_Trig_ReqTriggerStrobe(FALSE);
			UINT64 u64Tick = GetTickCount64();

			do	{

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

			}	while (1);
			/* Align Camera is Calibration Mode */
			uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
		}
	}
	Invalidate(TRUE);
}

/*
 desc : 새로운 검색을 위해 검색 대상의 모델 등록
 parm : None
 retn : Non
*/
VOID CDlgMain::SetRegistModel()
{
	UINT8 u8ACamID		= GetCheckACam();
	UINT32 u32Model[2]	= { (UINT32)ENG_MMDT::en_circle, (UINT32)ENG_MMDT::en_ring }, u32Find = 1;
	DOUBLE dbMSize1[2]	= {NULL} /* um */, dbMColor[2] = {NULL} /* 256:Black, 128:White */;
	DOUBLE dbMSize2[2]	= {NULL};
	LPG_CIEA pstCfg		= uvEng_GetConfig();

	/* Register the search target related model */
	u32Model[0]	= (UINT32)ENG_MMDT::en_circle;
	dbMSize1[0]	= pstCfg->acam_cali.model_shut_size * 1000.0f;
	dbMColor[0]	= pstCfg->acam_cali.model_shut_color;
	u32Model[1]	= (UINT32)ENG_MMDT::en_ring;
	dbMSize1[1]	= pstCfg->acam_cali.model_ring_size1 * 1000.0f;
	dbMSize2[1]	= pstCfg->acam_cali.model_ring_size2 * 1000.0f;
	dbMColor[1]	= pstCfg->acam_cali.model_ring_color;
	if (!uvEng_Camera_SetModelDefineEx(u8ACamID,
									   pstCfg->mark_find.model_speed,
									   pstCfg->mark_find.detail_level,
									   pstCfg->mark_find.model_smooth,
									   u32Model, dbMColor, dbMSize1,
									   dbMSize2, NULL, NULL, u32Find))
	{
		AfxMessageBox(L"Failed to setup the model for search target", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : 모션 (Motor Drive) 이동
 parm : direct	- [in]  Up or Down
 retn : None
*/
VOID CDlgMain::MotionMove(ENG_MDMD direct)
{
	UINT8 i	= 0x00, u8DrvNo = 0x00;	/* 0x00 ~ 0x07 */
	BOOL bCheck		= FALSE;
	DOUBLE dbDrvPos	= 0, dbMinPos, dbMaxPos, dbDist, dbVelo;
	LPG_CMSI pstCfg	= &uvEng_GetConfig()->mc2_svc;
	LPG_MDSM pstMC2	= uvEng_ShMem_GetMC2();

	/* 이동하고자 하는 Drive가 체크 되어 있는지 확인 */
	for (i=0; i<pstCfg->drive_count && !bCheck; i++)
	{
		bCheck	= m_chk_ctl[pstCfg->axis_id[i]].GetCheck() > 0;
	}
	if (!bCheck)
	{
		AfxMessageBox(L"The drive to be moved is not checked", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 이동하고자 하는 속도와 거리 값이 입력되어 있는지 확인 (여기서는 Step 이동만 해당 됨) */
	dbDist	= m_edt_flt[0].GetTextToDouble();
	dbVelo	= m_edt_flt[1].GetTextToDouble();
	if (dbDist < 0.0001f || dbVelo < 1.0f)
	{
		AfxMessageBox(L"Check the move distance and speed values", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* 현재 위치에서 이동 거리 값이 Min or Max 값을 초과 했다면, Min or Max 값 만큼 이동 */
	for (i=0; i<pstCfg->drive_count; i++)
	{
		/* 드라이브가 체크 되어 있는지 여부 확인 */
		u8DrvNo	= pstCfg->axis_id[i];
		if (!m_chk_ctl[u8DrvNo].GetCheck())	continue;

		/* 현재 드라이브의 위치 확인 */
		dbDrvPos	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(u8DrvNo));
		dbMinPos	= pstCfg->min_dist[u8DrvNo];
		dbMaxPos	= pstCfg->max_dist[u8DrvNo];

		/* 위로 혹은 오른 쪽으로 이동하면, 기존 위치에 이동 거리 값만큼 더해 주기 */
		if (direct == ENG_MDMD::en_move_up || direct == ENG_MDMD::en_move_right)
		{
			dbDrvPos	+= dbDist;	/* 이동할 거리 값 계산 */
			if (dbDrvPos > dbMaxPos)	dbDrvPos	= dbMaxPos;	/* 최대 이동 거리 넘어가면, 최대 값으로 대체 */
		}
		else
		{
			dbDrvPos	-= dbDist;	/* 이동할 거리 값 계산 */
			if (dbDrvPos < dbMinPos)	dbDrvPos	= dbMinPos;	/* 최소 이동 거리 넘어가면, 최대 값으로 대체 */
		}

		/* Motion Drive 이동 */
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI(u8DrvNo), dbDrvPos, dbVelo))
		{
			AfxMessageBox(L"Failed to move the motion drive", MB_ICONSTOP|MB_TOPMOST);
			return;
		}
	}
}

/*
 desc : MC2의 모든 Motor 초기화 (복구) (Homing)
 parm : None
 retn : None
*/
VOID CDlgMain::MC2Recovery()
{
	UINT8 i	= 0x00;
#if 1
	for (i=0; i<MAX_MC2_DRIVE; i++)
	{
		if (!m_chk_ctl[i].GetCheck())	continue;
		if (!uvEng_MC2_SendDevHoming(ENG_MMDI(i)))
		{
			AfxMessageBox(L"The home operation of all drives has failed", MB_ICONSTOP|MB_TOPMOST);
			return;
		}
	}
#else
	if (!uvEng_MC2_SendDevHomingAll())
	{
		AfxMessageBox(L"The home operation of all drives has failed", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
#endif
}

/*
 desc : Align Camera (Motor Drive) 이동 (절대 위치로 이동)
 parm : None
 retn : None
*/
VOID CDlgMain::ACamMoveAbs()
{
	UINT8 u8ACamID	= GetCheckACam();
	DOUBLE dbPosZ	= 0.0f;	/* unit : mm */

	/* Photohead or Align Camera Z Axis 이동 */
	dbPosZ	= m_edt_flt[9].GetTextToDouble();
	/* 새로운 위치로 이동 */
	uvEng_MCQ_SetACamMovePosZ(u8ACamID, 0x00 /* Absolute Moving */, dbPosZ);
}

/*
 desc : 그리드 컨트롤에 있는 데이터를 파일로 저장
 parm : None
 retn : None
*/
VOID CDlgMain::SaveAsExcelFile()
{
	TCHAR tzTempFile[2][MAX_PATH_LEN]	= {NULL};
	UINT8 u8ACamNo	= GetCheckACam();
	SYSTEMTIME stTm	= {NULL};

	/* 현재 컴퓨터 시간 */
	GetLocalTime(&stTm);

	/* 등록된 데이터가 없는지 확인 */
	if (m_pGridMeas->GetRowCount() < 2 || m_pGridMeas->GetColumnCount() < 2)	return;

	/* 윈도 임시 파일로 저장 후 열기 */
	swprintf_s(tzTempFile[0], MAX_PATH_LEN, L"%s\\xy2d\\acam_cali_x_%u_%04u_%04d%02d%02d_%02d%02d.csv",
			   g_tzWorkDir, u8ACamNo, (UINT16)m_edt_int[6].GetTextToNum(),
			   stTm.wYear, stTm.wMonth, stTm.wDay, stTm.wHour, stTm.wMinute);
	m_pGridMeas[0].Save(tzTempFile[0]);

	/* 윈도 임시 파일로 저장 후 열기 */
	swprintf_s(tzTempFile[1], MAX_PATH_LEN, L"%s\\xy2d\\acam_cali_y_%u_%04u_%04d%02d%02d_%02d%02d.csv",
			   g_tzWorkDir, u8ACamNo, (UINT16)m_edt_int[6].GetTextToNum(),
			   stTm.wYear, stTm.wMonth, stTm.wDay, stTm.wHour, stTm.wMinute);
	m_pGridMeas[1].Save(tzTempFile[1]);
}

/*
 desc : 초기 측정 위치로 이동
 parm : None
 retn : None
*/
VOID CDlgMain::InitMeasureMove()
{
	UINT8 i	= 0x00, u8ACamID = GetCheckACam(), u8DrvPh = UINT8(ENG_MMDI::en_axis_ph1);
	DOUBLE dbDist	= 0.0f, dbVelo = 0.0f, dbPosZ;
	ENG_MMDI enACam, enSide;

	/* 카메라가 2 개인 (외부 타입) 경우 */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)uvEng_GetConfig()->set_comn.align_camera_kind)
	{
		if (u8ACamID == 1)
		{
			enACam	= ENG_MMDI::en_align_cam1;
			enSide	= ENG_MMDI::en_align_cam2;
			dbDist	= uvEng_GetConfig()->mc2_svc.max_dist[(UINT8)enSide];
		}
		else
		{
			enACam	= ENG_MMDI::en_align_cam2;
			enSide	= ENG_MMDI::en_align_cam1;
			dbDist	= uvEng_GetConfig()->mc2_svc.min_dist[(UINT8)enSide];
		}

		/* Camera 2번 이동 */
		dbVelo	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)enSide];
		uvEng_MC2_SendDevAbsMove(enSide, dbDist, dbVelo);
		/* Camera 1번 이동 */
		dbVelo	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)enACam];
		uvEng_MC2_SendDevAbsMove(enACam, uvEng_GetConfig()->acam_cali.mark_acam[u8ACamID-1], dbVelo);
#if 0
		/* 카메라 Z Axis 이동 */
		uvEng_MCQ_SetACamMovePosZ(u8ACamID, 0x00 /* Absolute Moving */, uvEng_GetConfig()->acam_focus.acam_z_focus[u8ACamID-1]);
#endif
	}

	/* Stage X 이동 */
	dbVelo	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_x];
#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, uvEng_GetConfig()->acam_cali.mark_stage_x, dbVelo);
#else
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, uvEng_GetConfig()->acam_cali.mark_stage_x[u8ACamID-1], dbVelo);
#endif
	/* Stage Y 이동 */
	dbVelo	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_y];
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, uvEng_GetConfig()->acam_cali.mark_stage_y[u8ACamID-1], dbVelo);

	for (i=0; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		/* 광학계 Z 축 Focus 값 얻기 */
		dbPosZ	= uvEng_GetConfig()->luria_svc.ph_z_focus[i];
		/* 이동하고자 하는 값이 범위를 벗어났는지 여부 */
		if (!uvCmn_Luria_IsValidPhMoveZRange(dbPosZ))
		{
			AfxMessageBox(L"Out of moving range area", MB_ICONSTOP|MB_TOPMOST);
			return;
		}

		/* 광학계 Z 축 이동 */
		if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
		{
			uvEng_Luria_ReqSetMotorAbsPosition(i+1, dbPosZ);
		}
		else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
		{
			dbVelo = uvEng_GetConfig()->mc2_svc.max_velo[u8DrvPh+i];
			uvEng_MC2_SendDevAbsMove(ENG_MMDI(u8DrvPh+i), dbPosZ, dbVelo);
		}
	}
}

/*
 desc : 현재 측정 시작 위치 저장
 parm : None
 retn : None
*/
VOID CDlgMain::SaveMeasureMove()
{
	UINT8 u8ACam	= GetCheckACam();
	LPG_CACC pstCfg	= &uvEng_GetConfig()->acam_cali;

#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
	pstCfg->mark_stage_x			= m_edt_flt[6].GetTextToDouble();
#else
	pstCfg->mark_stage_x[u8ACam-1]	= m_edt_flt[6].GetTextToDouble();
#endif
	pstCfg->mark_stage_y[u8ACam-1]	= m_edt_flt[7].GetTextToDouble();
	pstCfg->mark_acam[u8ACam-1]		= m_edt_flt[8].GetTextToDouble();

	/* Init 파일로 저장 */
	uvEng_SaveACamCali();
	/* 화면에 다시 출력 */
	LoadMeasPos();
}

/*
 desc : 스테이지 위치를 작업자 위치로 이동
 parm : None
 retn : None
*/
VOID CDlgMain::UnloadPosition()
{
	DOUBLE dbMoveX	= uvEng_GetConfig()->set_align.table_unloader_xy[0][0];
	DOUBLE dbMoveY	= uvEng_GetConfig()->set_align.table_unloader_xy[0][1];

	/* Vacuum과 Shutter를 동시 제어 수행 */
	uvEng_MCQ_LedVacuumShutter(0x00, 0x01, 0x00);

	/* 모션 (스테이지) 이동 */
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, dbMoveX, m_edt_flt[1].GetTextToDouble());
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dbMoveY, m_edt_flt[1].GetTextToDouble());
}

/*
 desc : 현재 환경 파일에 저장된 회전 값 불러오기
 parm : None
 retn : None
*/
VOID CDlgMain::LoadParam()
{
#if 1
	/* Measurement Row and Column count */
	m_edt_int[0].SetTextToNum(UINT16(12));	/* Corner : Row count (Top) */
	m_edt_int[1].SetTextToNum(UINT16(30));	/* Corner : Col count (Left) */
	m_edt_int[2].SetTextToNum(UINT16(12));	/* Corner : Row count (Bottom) */
	m_edt_int[3].SetTextToNum(UINT16(30));	/* Corner : Col count (Right) */
	/* The size of Grabbed Image */
	m_edt_int[4].SetTextToNum(uvEng_GetConfig()->set_cams.ioi_size[0]);
	m_edt_int[5].SetTextToNum(uvEng_GetConfig()->set_cams.ioi_size[1]);
	m_edt_int[6].SetTextToNum(UINT16(960));	/* Material Thick */
	/* Move Velo and Distance */
	m_edt_flt[0].SetTextToNum(100.0f, 4);
	m_edt_flt[1].SetTextToNum(uvEng_GetConfig()->mc2_svc.move_velo,	4);
	/* Up or Down For camera z axis */
	m_edt_flt[9].SetTextToNum(1.0f, 4);
	/* Measurement Row and Column Period Size (Normal) (mm) */
	m_edt_flt[12].SetTextToNum(400.0f, 4);	/* Skip : Rows (Height) */
	m_edt_flt[13].SetTextToNum(360.0f, 4);	/* Skip : Cols (Width) */
#else
	/* Measurement Row and Column count */
	m_edt_int[0].SetTextToNum(UINT16(2));	/* Corner : Row count (Top) */
	m_edt_int[1].SetTextToNum(UINT16(2));	/* Corner : Col count (Left) */
	m_edt_int[2].SetTextToNum(UINT16(2));	/* Corner : Row count (Bottom) */
	m_edt_int[3].SetTextToNum(UINT16(2));	/* Corner : Col count (Right) */
	/* The size of Grabbed Image */
	m_edt_int[4].SetTextToNum(uvEng_GetConfig()->set_cams.ioi_size[0]);
	m_edt_int[5].SetTextToNum(uvEng_GetConfig()->set_cams.ioi_size[1]);
	m_edt_int[6].SetTextToNum(UINT16(960));	/* Material Thick */
	/* Move Velo and Distance */
	m_edt_flt[0].SetTextToNum(100.0f, 4);
	m_edt_flt[1].SetTextToNum(uvEng_GetConfig()->mc2_svc.move_velo,	4);
	/* Up or Down For camera z axis */
	m_edt_flt[9].SetTextToNum(1.0f, 4);
	/* Measurement Row and Column Period Size (Normal) (mm) */
	m_edt_flt[12].SetTextToNum(500.0f, 4);	/* Skip : Rows (Height) */
	m_edt_flt[13].SetTextToNum(500.0f, 4);	/* Skip : Cols (Width) */
#endif
	/* Measurement Row and Column Period Size (Cornor) (mm) */
	m_edt_flt[14].SetTextToNum(5.0f, 4);	/* Period : Cornor Rows */
	m_edt_flt[15].SetTextToNum(2.5f, 4);	/* Period : Cornor Rows */
	/* Offset Limit XY */
	m_edt_flt[17].SetTextToNum(uvEng_GetConfig()->acam_cali.GetErrLimitValUm(), 2);

	/* 현재 카메라의 측정 위치 적재 */
	LoadMeasPos();
}

/*
 desc : 측정 위치 적재
 parm : None
 retn : None
*/
VOID CDlgMain::LoadMeasPos()
{
	UINT8 u8ACamID	= GetCheckACam();

#if (0x00 == (0x0f & USE_ALIGN_CAMERA_2D_CALI_VER))
	m_edt_flt[6].SetTextToNum(uvEng_GetConfig()->acam_cali.mark_stage_x, 4);
#else
	m_edt_flt[6].SetTextToNum(uvEng_GetConfig()->acam_cali.mark_stage_x[u8ACamID-1], 4);
#endif
	m_edt_flt[7].SetTextToNum(uvEng_GetConfig()->acam_cali.mark_stage_y[u8ACamID-1], 4);
	m_edt_flt[8].SetTextToNum(uvEng_GetConfig()->acam_cali.mark_acam[u8ACamID-1], 4);
}

/*
 desc : 버튼 및 각종 컨트롤 Enable or Disable 처리
 parm : None
 retn : None
*/
VOID CDlgMain::EnableCtrl()
{
	UINT8 i;
	BOOL bStopped	= TRUE, bRunCali = m_pMainThread->IsRunWork();
	BOOL bConnected	= TRUE, bEnable	= TRUE;
	BOOL bIsRunDemo	= uvEng_GetConfig()->IsRunDemo();
	LPG_CMSI pstMC2Svc	= &uvEng_GetConfig()->mc2_svc;

	/* 필요한 버튼들 Enable or Disable 처리 */
	bConnected	= uvCmn_MC2_IsConnected() && uvCmn_Camera_IsConnected() &&
				  uvCmn_Trig_IsConnected()&& uvCmn_MCQ_IsConnected();
	/* 현재 선택된 카메라가 이동 상태인지 여부에 따라 */
	bStopped	= uvEng_ShMem_GetMC2()->GetDriveAct((UINT8)ENG_MMDI::en_stage_x)->IsStopped()	&&
				  uvEng_ShMem_GetMC2()->GetDriveAct((UINT8)ENG_MMDI::en_stage_y)->IsStopped()	&&
				  uvEng_ShMem_GetMC2()->GetDriveAct((UINT8)ENG_MMDI::en_align_cam1)->IsStopped()&&
				  uvEng_ShMem_GetMC2()->GetDriveAct((UINT8)ENG_MMDI::en_align_cam2)->IsStopped();
	if (!bIsRunDemo)
	{
		/* 버튼 활성화 조건 */
		bEnable	= bStopped && bConnected && !bRunCali;

		for (i=0; i<5; i++)
		{
			if (m_btn_ctl[i].IsWindowEnabled() == bEnable)	continue;
			m_btn_ctl[i].EnableWindow(bEnable);
		}
		for (i=5; i<14; i++)
		{
			if (m_btn_ctl[i].IsWindowEnabled() == bEnable)	continue;
			m_btn_ctl[i].EnableWindow(bEnable);
		}

		if (m_btn_ctl[14].IsWindowEnabled() != (!bEnable))
		{
			m_btn_ctl[14].EnableWindow(!bEnable);
		}
		m_btn_ctl[15].EnableWindow(bEnable);

		/* MC2 Drive Check Button */
		for (i=0; i<uvEng_GetConfig()->mc2_svc.drive_count; i++)
		{
			m_chk_ctl[pstMC2Svc->axis_id[i]].EnableWindow(bEnable);
		}
		for (i=8; i<11; i++)	m_chk_ctl[i].EnableWindow(bEnable);
	}

	m_chk_ctl[9].EnableWindow(!m_chk_ctl[8].GetCheck());
}

/*
 desc : Calibration Start
 parm : None
 retn : None
*/
VOID CDlgMain::StartCalibration()
{
	UINT16 u16Row[2], u16Col[2], i;
	UINT16 u16Thick	= (UINT16)m_edt_int[6].GetTextToNum();
	BOOL bIsRunDemo	= uvEng_GetConfig()->IsRunDemo();
	DOUBLE dbSkipRow, dbSkipCol, dbPeriodRow, dbPeriodCol;

	/* 소재 두께 값에 대해서 설정 버튼을 눌렀는지 여부 */
	if (bIsRunDemo)	u16Thick	= 0xffff;
	else
	{
		if (!m_bSetThick)
		{
			AfxMessageBox(L"No material thickness value set", MB_ICONSTOP|MB_TOPMOST);
			return;
		}
	}
	/* 그리드 컨트롤 리빌드 */
	if (!ReBuildGridXY())	return;

	/* Trigger 측정 속성 값 설정 (Trigger OnTime / Strobe OnTime / Trigger Delay Time */
	if (!uvEng_Trig_ReqInitUpdate())
	{
		AfxMessageBox(L"Failed to set the trigger attribute for LED Lamp", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* 현재 설정 값을 전역 변수에 저장 */
	u16Row[0]	= (UINT16)m_edt_int[0].GetTextToNum(); /* Top */
	u16Row[1]	= (UINT16)m_edt_int[2].GetTextToNum(); /* Bottom */
	u16Col[0]	= (UINT16)m_edt_int[1].GetTextToNum(); /* Left */
	u16Col[1]	= (UINT16)m_edt_int[3].GetTextToNum(); /* Right */
	/* 측정을 위한 Matrix (행과 열) 위치 (간격) 정보 (단위: mm) */
	dbSkipRow	= m_edt_flt[12].GetTextToDouble();
	dbSkipCol	= m_edt_flt[13].GetTextToDouble();
	dbPeriodRow	= m_edt_flt[14].GetTextToDouble();
	dbPeriodCol	= m_edt_flt[15].GetTextToDouble();
	/* 행과 열의 간격이 저장될 버퍼 할당 */
	m_stMeas.set_rows	= u16Row[0] + u16Row[1];
	m_stMeas.set_cols	= u16Col[0] + u16Col[1];
	m_stMeas.AllocData();
	/* Important */
	uvEng_GetConfig()->acam_cali.period_row_size= 0/*Fixed*/;
	uvEng_GetConfig()->acam_cali.period_col_size= 0/*Fixed*/;
	uvEng_GetConfig()->acam_cali.set_rows_count	= m_stMeas.set_rows;
	uvEng_GetConfig()->acam_cali.set_cols_count	= m_stMeas.set_cols;
	/* 행과 열의 간격 값을 버퍼에 등록 */
	for (i=1; i<u16Row[1]; i++)			m_stMeas.it_rows[i]	= m_stMeas.it_rows[i-1] + dbPeriodRow;	/* Bottom */
										m_stMeas.it_rows[i]	= m_stMeas.it_rows[i-1] + dbSkipRow;
	for (++i; i<m_stMeas.set_rows; i++)	m_stMeas.it_rows[i]	= m_stMeas.it_rows[i-1] + dbPeriodRow;	/* Top */
	for (i=1; i<u16Col[0]; i++)			m_stMeas.it_cols[i]	= m_stMeas.it_cols[i-1] + dbPeriodCol;	/* Left */
										m_stMeas.it_cols[i]	= m_stMeas.it_cols[i-1] + dbSkipCol;
	for (++i; i<m_stMeas.set_cols; i++)	m_stMeas.it_cols[i]	= m_stMeas.it_cols[i-1] + dbPeriodCol;	/* Right */
#if 0	
	for (i=0; i<m_stMeas.set_rows; i++)	TRACE(L"Row[%d] = %.4f\n", i, m_stMeas.it_rows[i]);
	for (i=0; i<m_stMeas.set_cols; i++)	TRACE(L"Col[%d] = %.4f\n", i, m_stMeas.it_rows[i]);
#endif

#if 0
	/* Open the file */
	CHAR szFile[_MAX_PATH]	= {NULL}, szValue[32];
	CUniToChar csCnv;
	FILE *fpCali;
	sprintf_s(szFile, _MAX_PATH, "%s\\xy2d\\demo_xy2d.dat", csCnv.Uni2Ansi(g_tzWorkDir));
	errno_t eRet = fopen_s(&fpCali, szFile, "wt");
	if (0 != eRet)
	{
		if (0 == eRet)	fclose(fpCali);
		fpCali	= NULL;
		return;
	}

	for (UINT16 k=0; k<m_stMeas.set_rows; k++)
	{
		for (UINT16 l=0; l<m_stMeas.set_cols; l++)
		{
			memset(szValue, 0x00, 32);
			sprintf_s(szValue, 32, "(%09.4f,%09.4f)", m_stMeas.it_cols[l], m_stMeas.it_rows[k]);
			fputs(szValue, fpCali);
		}
		fputs("\n", fpCali);
	}
	fclose(fpCali);
#endif

	/* Material Thickness 값 설정 여부 */
	if (m_edt_int[6].GetTextToNum() < 1)
	{
		AfxMessageBox(L"The material thickness is not entered", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* 작업 시작 */
	m_pMainThread->StartCali(GetCheckACam(), u16Thick, &m_stMeas);
}

/*
 desc : Calibration Stop
 parm : None
 retn : None
*/
VOID CDlgMain::StopCalibration()
{
	UINT64 u64Tick = GetTickCount64();

	m_pMainThread->StopCali();	/* 작업 중지 */

	do {

		uvCmn_uSleep(100);

		/* 10 초 이상 대기하면 에러 빠져나가기 */
		if (u64Tick+10000 < GetTickCount64())
		{
			break;
		}

	} while (m_pMainThread->IsRunWork());
}

/*
 desc : 현재 모션 위치 값을 에디트 박스에 출력
 parm : id	- [in]  Button ID
 retn : None
*/
VOID CDlgMain::SetMotionPos(UINT32 id)
{
	UINT8 u8EdtID	= 0x06 + UINT8(id - IDC_MAIN_BTN_STAGE_X);
	UINT8 u8ACamID	= GetCheckACam();
	DOUBLE dbPos	= 0.0f;
	ENG_MMDI enACam	= u8ACamID == 1 ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;

	switch (id)
	{
	case IDC_MAIN_BTN_STAGE_X	:	dbPos	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);	break;
	case IDC_MAIN_BTN_STAGE_Y	:	dbPos	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);	break;
	case IDC_MAIN_BTN_ACAM_X	:	dbPos	= uvCmn_MC2_GetDrvAbsPos(enACam);				break;
	}
	m_edt_flt[u8EdtID].SetTextToNum(dbPos, 4);
}

/*
 desc : 정상적으로 동작하는지 매칭 확인
 parm : None
 반화 : None
*/
VOID CDlgMain::RunTestMatch()
{
	UINT8 u8ACamID		= GetCheckACam();
	BOOL bFinded		= FALSE;
	UINT64 u64TickLoop	= 0, u64StartTick;
	LPG_ACGR pstResult	= NULL;

	/* 현재 Live Mode 방식인지 확인 */
	if (m_chk_ctl[8].GetCheck())
	{
		AfxMessageBox(L"Currently running in <live mode>", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 현재 등록된 모델이 존재하는지 여부 */
	if (!uvEng_Camera_IsSetMarkModel(0x01, u8ACamID))
	{
		AfxMessageBox(L"There are no registered models", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* Align Camera is Calibration Mode */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);

	/* 첫번 행과 열 부분 값 초기화 */
	m_pGridMeas[0].SetItemText(1, 1, L"+0.0");
	m_pGridMeas[1].SetItemText(1, 1, L"+0.0");
	m_pGridMeas[0].InvalidateRowCol(1, 1);
	m_pGridMeas[1].InvalidateRowCol(1, 1);

	/* 기존 검색된 Grabbed Data & Image 제거 */
	uvEng_Camera_ResetGrabbedImage();

	/* 작업 요청 대기 시간 */
	u64StartTick	= GetTickCount64();

	/* Trigger 강제로 (수동으로) 발생 */
	if (!uvEng_Trig_ReqTrigOutOne(u8ACamID, TRUE))
	{
		uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
		AfxMessageBox(L"Failed to send the event for trigger", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* 작업 요청 대기 시간 */
	u64StartTick	= GetTickCount64();
	/* 1 ~ 2 초 정도 대기 */
	do {

		if (u64StartTick+2000 < GetTickCount64())	break;
		uvCmn_uSleep(100);

	} while (1);

	/* 검색 결과 확인 */
	RunTestMeas();

	/* 화면 갱신 */
	InvalidateView();
	/* Camera 동작 모드 설정 */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
}

/*
 desc : 링 중심에 위치한 Circle 검색 및 이미지의 중심에서 오차가 얼마나 발생 했는지 확인
 parm : None
 반화 : None
*/
VOID CDlgMain::RunTestMeas()
{
	UINT8 u8ACamID		= GetCheckACam();
	LPG_ACGR pstResult	= NULL;

	/* 검색 결과 확인 */
	pstResult	= uvEng_Camera_RunModelCali(u8ACamID, 0xff);
	if (!pstResult || !pstResult->marked)
	{
		AfxMessageBox(L"Failed to analyze the grabbed image", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 그리드 컨트롤 갱신 */
	UpdateCaliXY(1, 1, pstResult);
}

/*
 desc : Calibration Material Thick 설정 만큼 카메라 Z 축 모두 이동 시킴
 parm : None
 retn : Non
*/
VOID CDlgMain::SetDofFilmThick()
{
	DOUBLE dbSetThick	= (DOUBLE)m_edt_int[6].GetTextToNum() / 1000.0f;	/* mm */
	DOUBLE dbBaseThick	= uvEng_GetConfig()->set_align.dof_film_thick;
	DOUBLE dbDiffThick	= dbSetThick - dbBaseThick;
	DOUBLE *pdbACamFocus= uvEng_GetConfig()->acam_focus.acam_z_focus;

	if (dbSetThick < 0.05f /* 50 um 이하이면 처리하지 못함 */)	return;

	/* 기준 두께 대비 차이만큼 Align Camera Z 축 이동 시킴 */
	uvEng_MCQ_SetACamMovePosZ(0x01, 0x00, pdbACamFocus[0]+dbDiffThick);
	uvEng_MCQ_SetACamMovePosZ(0x02, 0x00, pdbACamFocus[1]+dbDiffThick);

	/* 두께 값을 설정했다고 플래그 설정 */
	m_bSetThick	= TRUE;
}

/*
 desc : 모든 작업이 완료된 경우
 parm : None
 retn : None
*/
VOID CDlgMain::WorkComplete()
{
	m_edt_txt[1].SetTextToStr(L"100.000 %", TRUE);
#if 0
	TCHAR tzTime[32]	= {NULL};
	UINT64 u64JobTime	= m_pMainThread->GetWorkTime();

	swprintf_s(tzTime, 32, L"%.3f min", (u64JobTime/1000.0f) / 60.0f);
	m_edt_int[3].SetTextToStr(tzTime, TRUE);
#endif
}

/*
 설명 : 현재 작업 시간 갱신
 변수 : None
 반환 : None
*/
VOID CDlgMain::UpdateWorkTime()
{
	if (m_pMainThread && m_pMainThread->IsRunWork())
	{
		TCHAR tzVal[32]		= {NULL};
		UINT32 u32TimeSec	= (UINT32)(m_pMainThread->GetWorkTime()/1000);
		UINT32 u32TimeMin	= (UINT32)(u32TimeSec / 60);
		swprintf_s(tzVal, 32, L"%u m %u s", u32TimeMin, u32TimeSec%60);
		m_edt_txt[0].SetTextToStr(tzVal, TRUE);
		swprintf_s(tzVal, 32, L"%.3f %%", m_pMainThread->GetProcRate());
		m_edt_txt[1].SetTextToStr(tzVal, TRUE);
	}
}

/*
 desc : Updates the live image
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateLiveView()
{
	HDC hDC	= NULL;
	RECT rDraw;

	/* 이미지가 출력될 윈도 영역 */
	m_pic_ctl[0].GetClientRect(&rDraw);
	hDC	= ::GetDC(m_pic_ctl[0].m_hWnd);
	if (hDC)
	{
		uvEng_Camera_DrawLiveBitmap(hDC, rDraw, GetCheckACam());
		::ReleaseDC(m_pic_ctl[0].m_hWnd, hDC);
	}
}
