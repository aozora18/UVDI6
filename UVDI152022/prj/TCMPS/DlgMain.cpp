
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
	m_bDrawBG				= 0x01;
	m_hIcon					= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_u8ViewMode			= 0x00;
	m_u64DrawTime			= 0;
	m_bMotionStop			= FALSE;
	m_bLiveSaved			= FALSE;
	m_pMainThread			= NULL;
	m_u64TickLoadState		= 0;
	m_pLedAmplitue			= NULL;
	m_enLastJobLoadStatus	= ENG_LSLS::en_load_none;
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
	u32StartID	= IDC_MAIN_GRP_MARK_VIEW;
	for (i=0; i<13; i++)	DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* list box */
	u32StartID	= IDC_MAIN_BOX_JOBS_LIST;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_box_ctl[i]);
	/* For Image */
	u32StartID	= IDC_MAIN_PIC_MARK_VIEW_11;
	for (i=0; i<5; i++)		DDX_Control(dx, u32StartID+i,	m_pic_ctl[i]);
	/* Check - acam no */
	u32StartID	= IDC_MAIN_CHK_ACAM_NO_11;
	for (i=0; i<4; i++)		DDX_Control(dx, u32StartID+i,	m_chk_cam[i]);
	/* Check - moving to mark */
	u32StartID	= IDC_MAIN_CHK_MARK_1;
	for (i=0; i<4; i++)		DDX_Control(dx, u32StartID+i,	m_chk_mak[i]);
	/* Check - mc2 */
	u32StartID	= IDC_MAIN_CHK_MC2_DRV_0;
	for (i=0; i<8; i++)		DDX_Control(dx, u32StartID+i,	m_chk_drv[i]);
	/* Check - Align Method */
	u32StartID	= IDC_MAIN_CHK_EXPO_DIRECT;
	for (i=0; i<3; i++)		DDX_Control(dx, u32StartID+i,	m_chk_mtd[i]);
	/* Check - live */
	u32StartID	= IDC_MAIN_CHK_ACAM_LIVE;
	for (i=0; i<3; i++)		DDX_Control(dx, u32StartID+i,	m_chk_liv[i]);
	/* Button - Normal */
	u32StartID	= IDC_MAIN_BTN_MOVE_ACAM;
	for (i=0; i<37; i++)	DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Static - Normal */
	u32StartID	= IDC_MAIN_TXT_ACAM_ABS;
	for (i=0; i<17; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Edit - integer */
	u32StartID	= IDC_MAIN_EDT_EXPO_STEP;
	for (i=0; i<4; i++)		DDX_Control(dx, u32StartID+i,	m_edt_int[i]);
	/* edit - float */
	u32StartID	= IDC_MAIN_EDT_MC2_DIST;
	for (i=0; i<6; i++)		DDX_Control(dx, u32StartID+i,	m_edt_flt[i]);
	/* edit - float */
	u32StartID	= IDC_MAIN_EDT_MARK_RESULT_11;
	for (i=0; i<4; i++)		DDX_Control(dx, u32StartID+i,	m_edt_rlt[i]);
	/* edit - text */
	u32StartID	= IDC_MAIN_EDT_JOBS_STATUS;
	for (i=0; i<3; i++)		DDX_Control(dx, u32StartID+i,	m_edt_txt[i]);
}

BEGIN_MESSAGE_MAP(CDlgMain, CMyDialog)
	ON_MESSAGE(WM_MAIN_THREAD,	OnMsgMainThread)
	ON_WM_SYSCOMMAND()
	ON_LBN_SELCHANGE(IDC_MAIN_BOX_JOBS_LIST, OnBoxSelchangeJobList)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_BTN_MOVE_ACAM, IDC_MAIN_BTN_APP_EXIT,		OnBtnClicked)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_CHK_ACAM_NO_11, IDC_MAIN_CHK_ACAM_LIVE,	OnChkClicked)
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
	UINT32 u32Size	= 0, i = 0;

	/* 라이브러리 초기화 */
	if (!InitLib())		return FALSE;

	InitMem();		/* 초기 메모리 할당 */
	InitCtrl();		/* 윈도 컨트롤 초기화 */
	LoadParam();	/* 가장 최근에 설정된 값 불러오기 */
	InitGridPos();	/* 그리드 초기화 */
	InitGridLed();	/* 그리드 초기화 */
	InitSetup();

	/* Main Thread 생성 (!!! 중요. InitLib 가 성공하면, 반드시 생성해야 함 !!!) */
	m_pMainThread	= new CMainThread(m_hWnd);
	ASSERT(m_pMainThread);
	if (!m_pMainThread->CreateThread(0, NULL, NORMAL_THREAD_SPEED))
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
	/* 메모리 해제 */
	if (m_pLedAmplitue)
	{
		for (; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
		{
			::Free(m_pLedAmplitue[i]);
		}
		::Free(m_pLedAmplitue);
	}
	/* 라이브러리 해제 */
	CloseLib();
	/* Grid Control 메모리 해제*/
	CloseGrid();
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgMain::OnPaintDlg(CDC *dc)
{
	RECT rDraw;
	ENG_VCCM enMode	= uvEng_Camera_GetCamMode();

	/* 이미지가 출력될 윈도 영역 */
	m_pic_ctl[4].GetWindowRect(&rDraw);
	ScreenToClient(&rDraw);

	/* 현재 마크 개별 검색 모드인 경우 */
	switch (enMode)
	{
	case ENG_VCCM::en_live_mode	:	uvEng_Camera_DrawLive(dc->m_hDC, rDraw, FALSE);	break;
	case ENG_VCCM::en_grab_mode	:	uvEng_Camera_DrawGrab(dc->m_hDC, rDraw, GetCheckACam(), TRUE);	break;
	}
#if 0
	if (uvEng_Camera_GetGrabbedCount() == 0x04)
	{
		ENG_BWOK enJobID	= m_pMainThread->GetWorkJobID();
		switch (enJobID)
		{
		case ENG_BWOK::en_mark_test	:
		case ENG_BWOK::en_expo_mark	:
		case ENG_BWOK::en_expo_cali	:	UpdateMarkExpoResult();	break;
		}
	}
#endif
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
	return uvEng_Init(ENG_ERVM::en_cmps_sw);
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
 desc : 메모리 초기화
 parm : None
 retn : None
*/
VOID CDlgMain::InitMem()
{
	UINT8 i		= 0x00;
	UINT8 u8PHs	= uvEng_GetConfig()->luria_svc.ph_count;
	UINT8 u8LED	= uvEng_GetConfig()->luria_svc.led_count;

	/* Photohead LED Power 정보 임시 저장 */
	m_pLedAmplitue	= (UINT16**)::Alloc(u8PHs * sizeof(PUINT16));
	for (; i<u8PHs; i++)
	{
		m_pLedAmplitue[i]	= (PUINT16)::Alloc(sizeof(UINT16) * u8LED);
		memset(m_pLedAmplitue[i], 0x00, sizeof(UINT16) * u8LED);
	}
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
	for (i=0; i<13; i++)	m_grp_ctl[i].SetFont(&g_lf);
	/* List box - normal */
	for (i=0; i<1; i++)		m_box_ctl[i].SetLogFont(&g_lf);
	/* text - normal */
	for (i=0; i<17; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* button - normal */
	for (i=0; i<37; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
	}
	/* checkbox */
	for (i=0; i<4; i++)
	{
		m_chk_cam[i].SetLogFont(g_lf);
		m_chk_cam[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
	for (i=0; i<4; i++)
	{
		m_chk_mak[i].SetLogFont(g_lf);
		m_chk_mak[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
	for (i=0; i<8; i++)
	{
		m_chk_drv[i].SetLogFont(g_lf);
		m_chk_drv[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
	for (i=0; i<4; i++)
	{
		m_chk_mtd[i].SetLogFont(g_lf);
		m_chk_mtd[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
	for (i=0; i<1; i++)
	{
		m_chk_liv[i].SetLogFont(g_lf);
		m_chk_liv[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
	/* edit - integer */
	for (i=0; i<4; i++)
	{
		m_edt_int[i].SetEditFont(&g_lf);
		m_edt_int[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_int[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_int[i].SetInputType(EN_DITM::en_int);
	}
	/* edit - float */
	for (i=0; i<6; i++)
	{
		m_edt_flt[i].SetEditFont(&g_lf);
		m_edt_flt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_flt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_flt[i].SetInputType(EN_DITM::en_float);
	}
	/* edit - float */
	for (i=0; i<4; i++)
	{
		m_edt_rlt[i].SetEditFont(&g_lf);
		m_edt_rlt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_rlt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_rlt[i].SetInputType(EN_DITM::en_float);
	}
	/* edit - text */
	for (i=0; i<3; i++)
	{
		m_edt_txt[i].SetEditFont(&g_lf);
		m_edt_txt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_txt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_txt[i].SetInputType(EN_DITM::en_string);
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
	for (i=0x00; i<MAX_MC2_DRIVE; i++)		m_chk_drv[i].EnableWindow(FALSE);
	/* for MC2 - 체크 버튼 활성화 처리 */
	for (i=0; i<pstMC2->drive_count; i++)	m_chk_drv[pstMC2->axis_id[i]].EnableWindow(TRUE);
	/* Align Camera */
	for (i=0; i<pstCAM->acam_count; i++)	m_chk_cam[pstCAM->acam_count+i].EnableWindow(FALSE);
	/* Read Only */
//	for (i=0; i<3; i++)	m_edt_int[i].SetMouseClick(FALSE);
//	for (i=0; i<6; i++)	m_edt_flt[i].SetMouseClick(FALSE);
	for (i=0; i<4; i++)	m_edt_rlt[i].SetMouseClick(FALSE);
	for (i=0; i<1; i++)	m_edt_txt[i].SetMouseClick(FALSE);

	/* 노광 방법 선택 */
	m_chk_mtd[0].SetCheck(1);
	for (i=0; i<3; i++)	m_chk_mtd[i].EnableWindow(FALSE);
	/* 마크 이동 번호 선택 */
	m_chk_mak[0].SetCheck(1);
	/* 얼라인 카메라 선택 */
	m_chk_cam[0].SetCheck(1);

	/* 기본 값 설정 (속도, 거리) */
	m_edt_flt[0].SetTextToNum(uvEng_GetConfig()->mc2_svc.move_dist, 4);
	m_edt_flt[1].SetTextToNum(uvEng_GetConfig()->mc2_svc.move_velo, 4);

	/* 실행 환경에 맞는 검색 모델 등록 */
	SetRegistModel();

	/* 직접 노광 모드로 설정 */
	uvEng_Camera_SetAlignMode(ENG_AOEM::en_direct_expose);
}

/*
 desc : 그리드 컨트롤 생성
 parm : None
 retn : None
*/
VOID CDlgMain::InitGridPos()
{
	TCHAR tzCols[3][16]	= { L"Item", L"Pos (mm)" }, tzRows[32] = {NULL};
	INT32 i32Width[3]	= { 55, 85 }, i, j=1;
	INT32 i32ColCnt		= 2, i32RowCnt = 0;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	LPG_CIEA pstCfg		= uvEng_GetConfig();
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* Last Recv. Packet / PDO Header / PDO Data (Values) */
	::GetWindowRect(m_grp_ctl[2].GetSafeHwnd(), rGrid);
	ScreenToClient(rGrid);
	rGrid.left	+= 15;
	rGrid.right	-= 15;
	rGrid.top	+= 22;
	rGrid.bottom-= 13;

	/* ------------------ */
	/* for Motor Position */
	/* ------------------ */

	/* 총 출력될 행 (Row)의 개수 구하기 */
	i32RowCnt	= pstCfg->luria_svc.ph_count + pstCfg->mc2_svc.drive_count;
	/* Basler 카메라의 경우 Z Axis의 위치 (높이) 값 얻기 위함 */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)pstCfg->set_comn.align_camera_kind)
	{
		/* Align Camera의 Z Axis 높이 설정 위치 값 저장 */
		m_u32ACamZAxisRow	= i32RowCnt;
		/* Motion Left / Right 위치와 Up / Down 높이 위치 */
		i32RowCnt	+= pstCfg->set_cams.acam_count;
	}

	/* 그리드 컨트롤 기본 공통 속성 */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* 객체 생성 */
	m_pGridPos	= new CGridCtrl;
	ASSERT(m_pGridPos);
	m_pGridPos->SetModifyStyleEx(WS_EX_STATICEDGE);
	m_pGridPos->SetDrawScrollBarHorz(FALSE);
	m_pGridPos->SetDrawScrollBarVert(TRUE);
	if (!m_pGridPos->Create(rGrid, this, IDC_MAIN_GRID_MOTOR_POS, dwStyle))
	{
		delete m_pGridPos;
		m_pGridPos = NULL;
		return;
	}
	/* 객체 기본 설정 */
	m_pGridPos->SetLogFont(g_lf);
	m_pGridPos->SetRowCount(i32RowCnt+1);
	m_pGridPos->SetColumnCount(i32ColCnt);
	m_pGridPos->SetFixedRowCount(1);
	m_pGridPos->SetRowHeight(0, 24);
	m_pGridPos->SetFixedColumnCount(0);
	m_pGridPos->SetBkColor(RGB(255, 255, 255));
	m_pGridPos->SetFixedColumnSelection(0);

	/* 타이틀 (첫 번째 행) */
	stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	stGV.crBkClr = RGB(214, 214, 214);
	stGV.crFgClr = RGB(0, 0, 0);
	stGV.lfFont	= g_lf;
	for (i=0; i<i32ColCnt; i++)
	{
		stGV.nFormat= DT_VCENTER | DT_SINGLELINE | DT_CENTER;
		stGV.row	= 0;
		stGV.col	= i;
		stGV.strText= tzCols[i];
		m_pGridPos->SetItem(&stGV);
		m_pGridPos->SetColumnWidth(i, i32Width[i]);
	}

	/* 본문 배경색 */
	stGV.crBkClr = RGB(255, 255, 255);

	/* 본문 설정 - for Photohead Position */
	for (i=0; i<pstCfg->luria_svc.ph_count; i++,j++)
	{
		m_pGridPos->SetRowHeight(j, 24);

		/* for Subject */
		stGV.strText.Format(L"PH %d", i+1);
		stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
		stGV.row	= j;
		stGV.col	= 0;
		m_pGridPos->SetItem(&stGV);

		/* for Subject */
		stGV.strText= L"0.0000";
		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.col	= 1;
		m_pGridPos->SetItem(&stGV);
	}

	/* 본문 설정 - for Motion Drive Position */
	for (i=0; i<pstCfg->mc2_svc.drive_count; i++,j++)
	{
		m_pGridPos->SetRowHeight(j, 24);

		/* for Subject */
		stGV.strText.Format(L"MC %d", pstCfg->mc2_svc.axis_id[i]);
		stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
		stGV.row	= j;
		stGV.col	= 0;
		m_pGridPos->SetItem(&stGV);

		/* for Subject */
		stGV.strText= L"0.0000";
		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.col	= 1;
		m_pGridPos->SetItem(&stGV);
	}

	/* 본문 설정 - for Basler Camera Position */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)pstCfg->set_comn.align_camera_kind)
	{
		/* Z Axis Up / Down Postion */
		for (i=0; i<pstCfg->set_cams.acam_count; i++,j++)
		{
			m_pGridPos->SetRowHeight(j, 24);

			/* for Subject */
			stGV.strText.Format(L"AZ %d", i+1);
			stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
			stGV.row	= j;
			stGV.col	= 0;
			m_pGridPos->SetItem(&stGV);

			/* for Subject */
			stGV.strText= L"0.0000";
			stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
			stGV.col	= 1;
			m_pGridPos->SetItem(&stGV);
		}
	}

	m_pGridPos->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGridPos->UpdateGrid();
}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgMain::InitGridLed()
{
	TCHAR tzCols[5][8]	= { L"Item", L"LED1", L"LED2", L"LED3", L"LED4" }, tzRows[32] = {NULL};
	INT32 i32Width[5]	= { 48, 48, 48, 48, 48 }, i, j;
	INT32 i32ColCnt		= 5, i32RowCnt = uvEng_GetConfig()->luria_svc.ph_count;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* Last Recv. Packet / PDO Header / PDO Data (Values) */
	m_grp_ctl[6].GetWindowRect(rGrid);
	ScreenToClient(rGrid);
	rGrid.left	+= 14;
	rGrid.right	-= 78;
	rGrid.top	+= 24;
	rGrid.bottom-= 16;

	/* 그리드 컨트롤 기본 공통 속성 */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* 객체 생성 */
	m_pGridLed	= new CGridCtrl;
	ASSERT(m_pGridLed);
	m_pGridLed->SetModifyStyleEx(WS_EX_STATICEDGE);
	m_pGridLed->SetDrawScrollBarHorz(FALSE);
	m_pGridLed->SetDrawScrollBarVert(TRUE);
	if (!m_pGridLed->Create(rGrid, this, IDC_MAIN_GRID_PH_LED, dwStyle))
	{
		delete m_pGridLed;
		m_pGridLed = NULL;
		return;
	}
	/* 객체 기본 설정 */
	m_pGridLed->SetLogFont(g_lf);
	m_pGridLed->SetRowCount(i32RowCnt+1);
	m_pGridLed->SetColumnCount(i32ColCnt);
	m_pGridLed->SetFixedRowCount(1);
	m_pGridLed->SetRowHeight(0, 25);
	m_pGridLed->SetFixedColumnCount(0);
	m_pGridLed->SetBkColor(RGB(255, 255, 255));
	m_pGridLed->SetFixedColumnSelection(0);

	/* 타이틀 (첫 번째 행) */
	stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	stGV.crBkClr = RGB(214, 214, 214);
	stGV.crFgClr = RGB(0, 0, 0);
	stGV.lfFont	= g_lf;
	for (i=0; i<i32ColCnt; i++)
	{
		stGV.row	= 0;
		stGV.col	= i;
		stGV.strText= tzCols[i];
		m_pGridLed->SetItem(&stGV);
		m_pGridLed->SetColumnWidth(i, i32Width[i]);
	}

	/* 타이틀 (첫 번째 열) */
	stGV.crBkClr = RGB(239, 239, 239);
	for (i=1; i<=i32RowCnt; i++)
	{
		stGV.row	= i;
		stGV.col	= 0;
		stGV.strText.Format(L"PH %d", i);
		m_pGridLed->SetItem(&stGV);
		m_pGridLed->SetRowHeight(i, 24);
	}

	/* 본문 */
	stGV.nFormat = DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
	stGV.crBkClr = RGB(255, 255, 255);
	for (i=1; i<=i32RowCnt; i++)
	{
		for (j=1; j<i32ColCnt; j++)
		{
			stGV.row	= i;
			stGV.col	= j;
			stGV.strText= L"";
			m_pGridLed->SetItem(&stGV);
		}
	}

	m_pGridLed->SetBaseGridProp(1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0);
	m_pGridLed->UpdateGrid();
}

/*
 desc : 그리드 컨트롤 해제
 parm : None
 retn : None
*/
VOID CDlgMain::CloseGrid()
{
	if (m_pGridPos)
	{
		if (m_pGridPos->GetSafeHwnd())	m_pGridPos->DestroyWindow();
		delete m_pGridPos;
	}
	if (m_pGridLed)
	{
		if (m_pGridLed->GetSafeHwnd())	m_pGridLed->DestroyWindow();
		delete m_pGridLed;
	}
}

/*
 desc : List Box에서 거버를 선택한 경우, Luria Service에서 해당 선택된 거버로 선택되도록 처리
 parm : None
 retn : None
*/
VOID CDlgMain::OnBoxSelchangeJobList()
{
	INT32 i32CurSel	= 0;
	TCHAR tzJobName[MAX_PATH_LEN]	= {NULL};
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;
	CUniToChar csCnv;

	/* List Box에서 선택된 항목 가져오기 */
	i32CurSel	= m_box_ctl[0].GetCurSel();
	if (i32CurSel < 0)
	{
		AfxMessageBox(L"The selected item does not exist", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 선택된 항목이 유효한지 검사 */
	m_box_ctl[0].GetText(i32CurSel, tzJobName);
	if (wcslen(tzJobName) < 1)	return;
	/* 현재 선택된 Job Name가 동일하지 여부 확인 */
	if (0 == wcscmp(tzJobName, csCnv.Ansi2Uni(pstJobMgt->selected_job_name)))
	{
		AfxMessageBox(L"Job Name already selected", MB_ICONINFORMATION);
		return;
	}
	/* Luria에 선택된 거버가 선택되도록 명령어 전달 */
	if (!uvEng_Luria_ReqSelectedJobName(tzJobName, 0x00))
	{
		AfxMessageBox(L"Failed to selected the Job Name", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
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
	switch (wparam)
	{
	case MSG_WORK_PERIOD		:	UpdatePeriod();		break;
#if 0
	case MSG_WORK_COMPLETED		:	WorkCompleted();	break;
	case MSG_WORK_ALIGN_RESULT	:	WorkAlignResult();	break;
	case MSG_WORK_ERROR			:	WorkErrorJobName();	break;
#endif
	}

	return 0L;
}

/*
 desc : 부모 스레드에 의해 주기적으로 호출됨
 parm : None
 retn : None
*/
VOID CDlgMain::UpdatePeriod()
{
	/* Drive Position */
	UpdateMotorPos();
	/* Job List 갱신 */
	UpdateJobList();
	/* Job Load 갱신 */
	UpdateJobLoad();
	/* Exposure 갱신 */
	UpdateExposure();
	/* LED Power 갱신 */
	UpdateLedAmpl();
	/* Update Live 갱신 */
	UpdateLiveView();
	/* 모션 컨트롤이 움직이는 여부에 따라 버튼 Enable / Disable */
//	EnableCtrl();
	/* Update Job Rate */
//	UpdateJobRate();
}
#if 0
/*
 desc : 작업 완료 정보인 경우
 parm : None
 retn : None
*/
VOID CDlgMain::WorkCompleted()
{
	switch (m_pMainThread->GetWorkJobID())
	{
	case ENG_BWOK::en_mark_move	:	;	break;
	case ENG_BWOK::en_mark_grab	:	UpdateMarkGrabResult();	break;
	case ENG_BWOK::en_mark_test	:
	case ENG_BWOK::en_expo_mark	:
	case ENG_BWOK::en_expo_cali	:	;	break;
	}
	m_edt_txt[1].SetTextToStr(L"Completed", TRUE);
}

/*
 desc : 얼라인 작업 완료 정보인 경우
 parm : None
 retn : None
*/
VOID CDlgMain::WorkAlignResult()
{
	switch (m_pMainThread->GetWorkJobID())
	{
	case ENG_BWOK::en_mark_test	:
	case ENG_BWOK::en_expo_mark	:
	case ENG_BWOK::en_expo_cali	:	UpdateMarkExpoResult();	break;
	}
}

/*
 desc : 작업 실패 정보인 경우
 parm : None
 retn : None
*/
VOID CDlgMain::WorkErrorJobName()
{
	if (m_pMainThread->GetErrorJobName())
	{
		m_edt_txt[1].SetTextToStr(m_pMainThread->GetErrorJobName(), TRUE);
	}
}
#endif
/*
 desc : 각종 장비 내의 Motor Position (mm) 값 갱신
 parm : None
 retn : None
 note : Photohead Z Axis, Motion Drive, Align Camera Z Axis 등등
*/
VOID CDlgMain::UpdateMotorPos()
{
	UINT8 i	= 0x00, j = 1;
	DOUBLE dbPos[2]	= {NULL};
	LPG_CIEA pstCfg	= uvEng_GetConfig();
	LPG_LDDP pstDP	= &uvEng_ShMem_GetLuria()->directph;
	LPG_MDSM pstMC2	= uvEng_ShMem_GetMC2();

	/* Luria Photohead 위치 */
	for (i=0; i<pstCfg->luria_svc.ph_count; i++, j++)
	{
		dbPos[0] = m_pGridPos->GetItemTextToFloat(j, 1);
		dbPos[1] = (DOUBLE)ROUNDED(pstDP->focus_motor_move_abs_position[i]/1000.0f, 3);
		if (dbPos[0] != dbPos[1])
		{
			m_pGridPos->SetItemText(j, 1, dbPos[1]);
			m_pGridPos->InvalidateRowCol(j, 1);
		}
	}

	/* MC2 SD2S Drive 위치 (Basler Camera 포함) */
	for (i=0; i<pstCfg->mc2_svc.drive_count; i++, j++)
	{
		dbPos[0] = m_pGridPos->GetItemTextToFloat(j, 1);
		dbPos[1] = (DOUBLE)ROUNDED(pstMC2->act_data[pstCfg->mc2_svc.axis_id[i]].real_position/10000.0f, 4);
		if (dbPos[0] != dbPos[1])
		{
			m_pGridPos->SetItemText(j, 1, dbPos[1]);
			m_pGridPos->InvalidateRowCol(j, 1);
		}
	}

	/* Align Camera 위치 (Basler 카메라인 경우만 해당) */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)pstCfg->set_comn.align_camera_kind)
	{
		for (i=0; i<pstCfg->set_cams.acam_count; i++, j++)
		{
			dbPos[0] = m_pGridPos->GetItemTextToFloat(j, 1);
			dbPos[1] = (DOUBLE)ROUNDED(uvCmn_MCQ_GetACamCurrentPosZ(i+1), 4);
			if (dbPos[0] != dbPos[1])
			{
				m_pGridPos->SetItemText(j, 1, dbPos[1]);
				m_pGridPos->InvalidateRowCol(j, 1);
			}
		}
	}
}

/*
 desc : Job List 갱신
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateJobList()
{
	UINT8 i, u8Find		= 0x00;
	TCHAR tzGerberName[MAX_GERBER_NAME]	= {NULL};
	BOOL bUpdateJobs	= FALSE;
	INT32 i32JobList	= m_box_ctl[0].GetCount(), i32AddIndex;
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;
	CUniToChar csCnv;

	/* 메모리에 등록된 것이 없으면서, 리스트에 등록되것이 없으면 Skip */
	if (pstJobMgt->job_regist_count < 1 && i32JobList < 1)	return;
	/* 메모리에 등록된 개수와 리스트 박스에 등록된 개수가 다른지 확인 */
	if (i32JobList != pstJobMgt->job_regist_count)	bUpdateJobs	= TRUE;
	/* 공유 메모리와 리스트에 등록된 개수는 같지만, 혹시 거버 이름이 다를수 도 있으므로 */
	if (!bUpdateJobs)
	{
		for (i=0; i<pstJobMgt->job_regist_count; i++)
		{
			/* List Box에 등록된 Gerber Name 가져오기 */
			wmemset(tzGerberName, 0x00, MAX_GERBER_NAME);
			m_box_ctl[0].GetText(i, tzGerberName);
			/* 동일한 항목을 찾았으면... 찾은 개수 증가 시키기 */
			if (pstJobMgt->IsJobOnlyFinded(csCnv.Uni2Ansi(tzGerberName)))	u8Find++;
		}
		/* 모두 동일한 항목인지 확인 / 동일하지 않으면, 리스트 박스 갱신 필요 */
		if (u8Find != pstJobMgt->job_regist_count)	bUpdateJobs	= TRUE;
	}
	/* 갱신할 필요 없다면, 리턴 */
	if (!bUpdateJobs)	return;

	/* ------------------------ */
	/* Job List 등록 혹은 갱신  */
	/* ------------------------ */

	/* 갱신 비활성화 */
	m_box_ctl[0].SetRedraw(FALSE);
	/* 기존 항목 모두 지우기 */
	m_box_ctl[0].ResetContent();
	/* 공유 메모리에 있는 항목 등록 */
	for (i=0; i<pstJobMgt->job_regist_count; i++)
	{
		i32AddIndex	= m_box_ctl[0].AddString(csCnv.Ansi2Uni(pstJobMgt->job_name_list[i]));
		/* 현재 선택된 항목인지 확인 후, 선택된 항목이면, 선택 작업 진행 */
		if (0 == strcmp(pstJobMgt->job_name_list[i], pstJobMgt->selected_job_name))
		{
			m_box_ctl[0].SetCurSel(i32AddIndex);
		}
	}

	/* 갱신 활성화 */
	m_box_ctl[0].SetRedraw(TRUE);
	m_box_ctl[0].Invalidate(FALSE);
}

/*
 desc : Job Load 상태 갱신
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateJobLoad()
{
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;

	/* 현재 선택된 Job Name이 존재하는지 여부 */
	if (!pstJobMgt->IsJobNameSelected())
	{
		m_edt_txt[0].SetTextToStr(L"None",	TRUE);
		return;
	}

	switch (pstJobMgt->selected_job_load_state)
	{
	case ENG_LSLS::en_load_none			:	m_edt_txt[0].SetTextToStr(L"None",		TRUE);	break;
	case ENG_LSLS::en_load_not_started	:	m_edt_txt[0].SetTextToStr(L"Ready",		TRUE);	break;
	case ENG_LSLS::en_loading			:	m_edt_txt[0].SetTextToStr(L"Loading",	TRUE);	break;
	case ENG_LSLS::en_load_completed	:	m_edt_txt[0].SetTextToStr(L"Loaded",	TRUE);	break;
	case ENG_LSLS::en_load_failed		:	m_edt_txt[0].SetTextToStr(L"Failed",	TRUE);	break;
	}

	/* 현재 상태가 Completed이고 이전 (최근) 상태가 Completed가 아니라면 ... */
	if (ENG_LSLS(pstJobMgt->selected_job_load_state) == ENG_LSLS::en_load_completed &&
		ENG_LSLS(pstJobMgt->selected_job_load_state) != m_enLastJobLoadStatus)
	{
		/* XML 파일 적재 진행 */
		if (!uvEng_Luria_LoadSelectJobXML())
		{
			AfxMessageBox(L"Failed to load the currently selected gerber file", MB_ICONSTOP|MB_TOPMOST);
		}
	}
	/* 가장 최근에 Job Load 상태 값 갱신 */
	m_enLastJobLoadStatus	= ENG_LSLS(pstJobMgt->selected_job_load_state);

	/* Luria로부터 통지 서비스를 받는지 여부에 따라 */
	if (!uvEng_GetConfig()->luria_svc.use_announcement)
	{
		/* 만약, 현재 거버가 적재 중인 상태인 경우, 지속적으로 Job Load State 요청 */
		if ((pstJobMgt->selected_job_load_state == (UINT8)ENG_LSLS::en_load_not_started ||
			 pstJobMgt->selected_job_load_state == (UINT8)ENG_LSLS::en_loading) &&
			 (GetTickCount64() > m_u64TickLoadState + 250))
		{
			/* 다음 요청 주기 결정할 시간 값 임시 저장 */
			m_u64TickLoadState	= GetTickCount64();
			/* Job Load State 요청 */
			uvEng_Luria_ReqGetSelectedJobLoadState();
		}
	}
}

/*
 desc : LED Power 갱신
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateLedAmpl()
{
	UINT8 i		= 0x00;
	UINT8 u8PHs	= uvEng_GetConfig()->luria_svc.ph_count;
	UINT8 u8LED	= uvEng_GetConfig()->luria_svc.led_count;
	BOOL bUpdate	= FALSE;
	LPG_LDEW pstExpo= &uvEng_ShMem_GetLuria()->exposure;

	/* 기존 값과 동일하면 갱신하지 않음 */
	for (; i<u8PHs && !bUpdate; i++)
	{
		if (0 != memcmp(m_pLedAmplitue[i], pstExpo->led_amplitude_power[i], sizeof(UINT16) * u8LED))
		{
			memcpy(m_pLedAmplitue[i], pstExpo->led_amplitude_power[i], sizeof(UINT16) * u8LED);
			bUpdate = TRUE;
		}
	}
	/* 변경된 값이 없다면 ... */
	if (!bUpdate)	return;

	/* 만약 LED Power 값이 다르면 갱신 */
	m_pGridLed->SetRedraw(FALSE);
	for (i=0x00; i<u8PHs; i++)
	{
		m_pGridLed->SetItemText(i+1, 1, UINT32(pstExpo->led_amplitude_power[i][0]));
		m_pGridLed->SetItemText(i+1, 2, UINT32(pstExpo->led_amplitude_power[i][1]));
		m_pGridLed->SetItemText(i+1, 3, UINT32(pstExpo->led_amplitude_power[i][2]));
		m_pGridLed->SetItemText(i+1, 4, UINT32(pstExpo->led_amplitude_power[i][3]));
	}
	m_pGridLed->SetRedraw(TRUE);
	m_pGridLed->Invalidate(FALSE);
}
/*
 desc : Exposure 상태 갱신 (Print Speed, Step Size, Frame Rate, Duty Cycle, Printing, Status, ...)
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateExposure()
{
	UINT32 u32Speed		= (UINT32)ROUNDED(m_edt_flt[3].GetTextToDouble() * 1000.0f, 0);	/* 현재 노광 속도 값 얻기 */
	LPG_LDEW pstExpo	= &uvEng_ShMem_GetLuria()->exposure;
	LPG_LDMC pstMachine	= &uvEng_ShMem_GetLuria()->machine;

	/* Exposure Speed / Step Size / Frame Rate / Duty Cycle */
	if (u32Speed != pstExpo->get_exposure_speed)
	{
		/* Exposure Speed */
		m_edt_flt[3].SetTextToNum(DOUBLE(pstExpo->get_exposure_speed/1000.0f), 3, TRUE);
		/* Step Size */
		m_edt_int[0].SetTextToNum(pstExpo->scroll_step_size, TRUE);
		/* Duty Cycle */
		m_edt_int[1].SetTextToNum(pstExpo->led_duty_cycle, TRUE);
		/* Frame Rate */
		m_edt_int[2].SetTextToNum(pstExpo->frame_rate_factor, TRUE);
	}
}
#if 0
/*
 desc : Exposure 상태 갱신 (Print Speed, Step Size, Frame Rate, Duty Cycle, Printing, Status, ...)
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateJobRate()
{
	if (m_pMainThread->IsRunWork())
	{
		TCHAR tzRate[32]	= {NULL};
		swprintf_s(tzRate, 32, L"%.02f %%", m_pMainThread->GetWorkRate());
		m_edt_txt[2].SetTextToStr(tzRate, TRUE);
	}
}
#endif
/*
 desc : Live Grabbed Image 출력
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateLiveView()
{
	/* Grabbed Image Display */
	if (m_chk_liv[0].GetCheck())	/* Live Mode */
	{
		if (GetTickCount64() > (m_u64DrawTime+100))
		{
			/* 현재 출력된 시점의 시간 저장 */
			m_u64DrawTime	= GetTickCount64();
			InvalidateView();
		}
		m_u8ViewMode	= 0x01;	/* Live Mode 저장 */
	}
	else	/* Grabbed Mode */
	{
		/* 방금 전까지 Live Mode였다가 Grab Mode로 바뀐 경우 */
		if (0x01 == m_u8ViewMode)
		{
			m_u8ViewMode	= 0x00;	/* Grab Mode 저장 */
		}
	}
}
#if 0
/*
 desc : Mark Grabbed Image의 결과에 대한 정보 출력
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateMarkGrabResult()
{
	TCHAR tzResult[128]	= {NULL};
	UINT8 u8ACamID		= GetCheckACam();
	UINT8 u8MarkNo		= GetCheckMarkNo();
	LPG_ACGR pstResult	= m_pMainThread->GetLastGrabResult();
	CMyEdit *pEdit		= NULL;
	CMyStatic *pPic		= NULL;

	if (!u8ACamID)
	{
		AfxMessageBox(L"There is no camera selected!", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* 선택된 Mark가 없는 경우 */
	if (0x00 == u8MarkNo)	return;
	pEdit	= &m_edt_rlt[u8MarkNo-1];
	pPic	= &m_pic_ctl[u8MarkNo-1];

	/* 결과 값 출력 */
	swprintf_s(tzResult, 128, L"[%.1f,%.1f um] [%.1f %%] [%.1f %%]",
			   (DOUBLE)ROUNDED(pstResult->move_mm_x * 1000.0f, 1),
			   (DOUBLE)ROUNDED(pstResult->move_mm_y * 1000.0f, 1),
			   pstResult->score_rate, pstResult->scale_rate);
	pEdit->SetTextToStr(tzResult, TRUE);
	/* 화면 갱신 */
	uvEng_Camera_DrawCali(pPic->GetSafeHwnd());
}

/*
 desc : Mark Grabbed Image의 결과에 대한 정보 출력
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateMarkExpoResult()
{
	TCHAR tzResult[128]	= {NULL};
	UINT8 i, u8CamID, u8ImgID;
	LPG_ACGR pstMark	= NULL;
	CMyEdit *pEdit		= NULL;
	CMyStatic *pPic		= NULL;

	for (i=0x00; i<4; i++)
	{
		u8CamID	= i < 2 ? 0x01 : 0x02;
		u8ImgID	= i % 2;
		pstMark	= uvEng_Camera_GetGrabbedMark(u8CamID, u8ImgID);
		if (!pstMark)	continue;
		/* 검색 결과 출력 */
		pEdit	= &m_edt_rlt[i];
		/* 결과 값 출력 */
		swprintf_s(tzResult, 128, L"[%.1f,%.1f um] [%.1f %%] [%.1f %%]",
				   (DOUBLE)ROUNDED(pstMark->move_mm_x * 1000.0f, 1),
				   (DOUBLE)ROUNDED(pstMark->move_mm_y * 1000.0f, 1),
				   pstMark->score_rate, pstMark->scale_rate);
		pEdit->SetTextToStr(tzResult, TRUE);
		/* 검색된 이미지 출력 */
		pPic	= &m_pic_ctl[i];
		uvEng_Camera_DrawMarkData(pPic->GetSafeHwnd(), pstMark, pstMark->marked);
	}
}
#endif
/*
 desc : 일반 버튼 이벤트 처리
 parm : id	- [in]  Click된 버튼 ID
 retn : None
*/
VOID CDlgMain::OnBtnClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_MAIN_BTN_APP_EXIT		:	PostMessage(WM_CLOSE, 0, 0L);		break;
	case IDC_MAIN_BTN_MC2_MOVE_UP	:	MotionMove(ENG_MDMD::en_move_up);	break;
	case IDC_MAIN_BTN_MC2_MOVE_DN	:	MotionMove(ENG_MDMD::en_move_down);	break;
	case IDC_MAIN_BTN_MC2_RESET		:	MC2Recovery();						break;
	case IDC_MAIN_BTN_MOVE_ACAM		:	ACamMoveAbs();						break;
	case IDC_MAIN_BTN_MOVE_UNLOAD	:	UnloadPosition();					break;
	case IDC_MAIN_BTN_JOBS_ADD		:	JobMgtAdd();						break;
	case IDC_MAIN_BTN_JOBS_DEL		:	JobMgtDel();						break;
	case IDC_MAIN_BTN_JOBS_LIST		:	JobMgtList();						break;
	case IDC_MAIN_BTN_JOBS_LOAD		:	JobMgtLoad();						break;
	case IDC_MAIN_BTN_EXPO_BASE		:	SetExpoParamBase();					break;
	case IDC_MAIN_BTN_EXPO_SET		:	SetExpoParam();						break;
	case IDC_MAIN_BTN_EXPO_GET		:	GetExpoParam();						break;
	case IDC_MAIN_BTN_LED_GET		:	GetLedLightLvl();					break;
	case IDC_MAIN_BTN_LED_SET		:	SetLedLightLvl();					break;
	case IDC_MAIN_BTN_LED_DEFAULT	:	SetLedLightLevel(0x01);				break;
	case IDC_MAIN_BTN_LED_MIN		:	SetLedLightLevel(0x00);				break;
	case IDC_MAIN_BTN_START_GET		:	ExpoStartXY(0);						break;
	case IDC_MAIN_BTN_START_SET		:	ExpoStartXY(1);						break;
	case IDC_MAIN_BTN_START_MOVE	:	uvEng_Luria_ReqSetPrintStartMove();	break;
	case IDC_MAIN_BTN_PH_HOME		:	PhZAxisInitAll();					break;
	case IDC_MAIN_BTN_MOTOR_FOCUS	:	MovingMotorFocus();					break;
	case IDC_MAIN_BTN_LOAD_CONFIG	:	uvEng_ReLoadConfig();				break;
//	case IDC_MAIN_BTN_WORK_STOP		:	WorkStop();							break;
	case IDC_MAIN_BTN_SHUTER_OPEN	:	ShutterCtrl(1);						break;
	case IDC_MAIN_BTN_SHUTER_CLOSE	:	ShutterCtrl(0);						break;
	case IDC_MAIN_BTN_VACUUM_ON		:	VacuumCtrl(1);						break;
	case IDC_MAIN_BTN_VACUUM_OFF	:	VacuumCtrl(0);						break;
	case IDC_MAIN_BTN_JOB_MATERIAL	:	PhZAxisMove();						break;
	case IDC_MAIN_BTN_CALC_ENERGY	:	CalcEnergy();						break;
	case IDC_MAIN_BTN_GRAB_SAVE		:	Grabbed2Saved();					break;
#if 0
	case IDC_MAIN_BTN_MARK_MOVE		:
	case IDC_MAIN_BTN_MARK_GRAB		:
	case IDC_MAIN_BTN_MARK_TEST		:
	case IDC_MAIN_BTN_WORK_START	:	m_edt_txt[1].SetTextToStr(L"", TRUE);
										m_edt_txt[2].SetTextToStr(L"0.00 %%", TRUE);
		switch (id)
		{
		case IDC_MAIN_BTN_MARK_MOVE		:	MoveToMarkPos();				break;
		case IDC_MAIN_BTN_MARK_GRAB		:	MarkGrab();						break;
		case IDC_MAIN_BTN_MARK_TEST		:	MarkTest();						break;
//		case IDC_MAIN_BTN_WORK_START	:	WorkStart();					break;
		}
#endif
	}
}

/*
 desc : 체크 버튼 이벤트 처리
 parm : id	- [in]  Click된 버튼 ID
 retn : None
*/
VOID CDlgMain::OnChkClicked(UINT32 id)
{
	UINT8 i	= 0x00;

	switch (id)
	{
	case IDC_MAIN_CHK_ACAM_LIVE			:	SetLiveView();		break;

	case IDC_MAIN_CHK_EXPO_DIRECT		:
	case IDC_MAIN_CHK_EXPO_ALIGN_ONLY	:
	case IDC_MAIN_CHK_EXPO_ALIGN_CALI	:
		for (i=0; i<3; i++)	m_chk_mtd[i].SetCheck(0);
		m_chk_mtd[id-IDC_MAIN_CHK_EXPO_DIRECT].SetCheck(1);
		uvEng_Camera_SetAlignMode(ENG_AOEM(id-IDC_MAIN_CHK_EXPO_DIRECT));	break;

	case IDC_MAIN_CHK_MARK_1			:
	case IDC_MAIN_CHK_MARK_2			:
	case IDC_MAIN_CHK_MARK_3			:
	case IDC_MAIN_CHK_MARK_4			:
		for (i=0; i<4; i++)	m_chk_mak[i].SetCheck(0);
		m_chk_mak[id-IDC_MAIN_CHK_MARK_1].SetCheck(1);		break;

	case IDC_MAIN_CHK_ACAM_NO_11		:
	case IDC_MAIN_CHK_ACAM_NO_12		:
	case IDC_MAIN_CHK_ACAM_NO_21		:
	case IDC_MAIN_CHK_ACAM_NO_22		:
		for (i=0; i<uvEng_GetConfig()->set_cams.acam_count; i++)	m_chk_cam[i].SetCheck(0);
		m_chk_cam[id-IDC_MAIN_CHK_ACAM_NO_11].SetCheck(1);		break;
	}
}

/*
 desc : 현재 선택된 얼라인 카메라 번호 반환 (1 or 2)
 parm : None
 retn : 1 or 2 (0x00 값이면 선택된 것이 읍따)
*/
UINT8 CDlgMain::GetCheckACam()
{
	TCHAR tzNum[8]	= {NULL};

	if (m_chk_cam[0].GetCheck())	return 0x01;
	if (m_chk_cam[1].GetCheck())	return 0x02;
	if (m_chk_cam[2].GetCheck())	return 0x03;
	if (m_chk_cam[3].GetCheck())	return 0x04;
	return 0x00;
}

/*
 desc : 현재 선택된 Mark Number 값 반환
 parm : None
 retn : 선택된 Mark Number 값 (0x01 ~ 0x04)
*/
UINT8 CDlgMain::GetCheckMarkNo()
{
	UINT8 u8MarkNo	= 0x00;

	/* 현재 선택된 마크 번호 확인 */
	if (m_chk_mak[0].GetCheck())	u8MarkNo = 0x01;
	if (m_chk_mak[1].GetCheck())	u8MarkNo = 0x02;
	if (m_chk_mak[2].GetCheck())	u8MarkNo = 0x03;
	if (m_chk_mak[3].GetCheck())	u8MarkNo = 0x04;

	return u8MarkNo;
}

/*
 desc : 현재 선택된 Mark Number 값 반환
 parm : None
 retn : 선택된 Job Type 값 (0x01 : Direct Expose, 0x02 : Align Expose, 0x03 : Align & Calibration Expose)
*/
UINT8 CDlgMain::GetCheckMethod()
{
	if (m_chk_mtd[1].GetCheck())	return 0x02;	/* Only Align Expose */
	if (m_chk_mtd[2].GetCheck())	return 0x03;

	return 0x01;	/* Only Direct Expose*/
}

/*
 desc : Live Mode 설정
 parm : None
 retn : None
*/
VOID CDlgMain::SetLiveView()
{
	UINT8 u8ACamID	= GetCheckACam();
	BOOL bSucc		= FALSE;
	UINT32 u32OnOff	= 0x00000000;

	if (!u8ACamID)
	{
		AfxMessageBox(L"There is no camera selected!", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* 일단 Live Image 저장 없이 동작 */
	m_bLiveSaved	= FALSE;

	/* 현재 채크된 상태이면 */
	if (m_chk_liv[0].GetCheck())	/* Live View */
	{
		/* Align Camera is Live Mode */
		uvEng_Camera_SetCamMode(ENG_VCCM::en_live_mode);
		/* Trigger & Strobe : Enabled */
		bSucc = uvEng_Trig_ReqTriggerStrobe(TRUE);
		if (bSucc)
		{
			bSucc = uvEng_Trig_ReqEncoderLive(u8ACamID);
#if 0
			/* Live Image 저장 여부 */
			if (IDYES == AfxMessageBox(L"Would you like to save the live image to a file?", MB_YESNO))
				m_bLiveSaved = TRUE;
#endif
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
	UINT32 u32Model	= (UINT32)ENG_MMDT::en_circle, i = 0;
	DOUBLE dbMSize	= 1.0f /* um */, dbMColor = 256.0f /* 256:Black, 128:White */;
	LPG_CIEA pstCfg	= uvEng_GetConfig();
	/* 검색 대상에 대한 모델 등록 */
	dbMSize	= pstCfg->cmps_test.model_dia_size * 1000.0f;
	dbMColor= pstCfg->cmps_test.model_mark_color;

	for (i=0; i<pstCfg->set_cams.acam_count; i++)
	{
		if (!uvEng_Camera_SetModelDefineEx(i+1,
										   pstCfg->mark_find.model_speed,
										   pstCfg->mark_find.model_smooth,
										   &u32Model, &dbMColor, &dbMSize,
										   NULL, NULL, NULL, 1))
		{
			AfxMessageBox(L"Failed to setup the model for search target", MB_ICONSTOP|MB_TOPMOST);
			return;
		}
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
		bCheck	= m_chk_drv[pstCfg->axis_id[i]].GetCheck() > 0;
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
		if (!m_chk_drv[u8DrvNo].GetCheck())	continue;

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
		if (!m_chk_drv[i].GetCheck())	continue;
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
	DOUBLE dbPosZ	= 0.0f;	/* unit : mm */
	UINT8 u8ACamID	= GetCheckACam();
	if (!u8ACamID)
	{
		AfxMessageBox(L"There is no camera selected!", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* Photohead or Align Camera Z Axis 이동 */
	dbPosZ	= m_edt_flt[2].GetTextToDouble();
	/* 새로운 위치로 이동 */
	uvEng_MCQ_SetACamMovePosZ(u8ACamID, 0x00 /* Absolute Moving */, dbPosZ);
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
#if 0
	/* Measurement Row and Column count */
	m_edt_int[0].SetTextToNum(UINT16(10));
	m_edt_int[1].SetTextToNum(UINT16(40));
	/* The size of Grabbed Image */
	m_edt_int[2].SetTextToNum(uvEng_GetConfig()->set_cams.ioi_size[0]);
	m_edt_int[3].SetTextToNum(uvEng_GetConfig()->set_cams.ioi_size[1]);
	/* Move Velo and Distance */
	m_edt_flt[0].SetTextToNum(100.0f,	4);
	m_edt_flt[1].SetTextToNum(uvEng_GetConfig()->mc2_svc.move_velo,	4);
	/* Up or Down For camera z axis */
	m_edt_flt[9].SetTextToNum(1.0f, 4);
	/* Measurement Row and Column Period Size (mm) */
	m_edt_flt[12].SetTextToNum(5.0f, 4);
	m_edt_flt[13].SetTextToNum(5.0f, 4);
#endif
}
#if 0
/*
 desc : 버튼 및 각종 컨트롤 Enable or Disable 처리
 parm : None
 retn : None
*/
VOID CDlgMain::EnableCtrl()
{
	UINT8 i;
	BOOL bStopped	= TRUE, bRunExpo = m_pMainThread->IsRunWork();
	LPG_CMSI pstMC2Svc	= &uvEng_GetConfig()->mc2_svc;

	/* 필요한 버튼들 Enable or Disable 처리 */
	if (!uvCmn_MC2_IsConnected()	||
		!uvCmn_Camera_IsConnected()	||
		!uvCmn_Trig_IsConnected()	||
		!uvCmn_MCQ_IsConnected())
	{
		return;
	}

	/* 현재 선택된 카메라가 이동 상태인지 여부에 따라 */
	bStopped	= uvEng_ShMem_GetMC2()->GetDriveAct((UINT8)ENG_MMDI::en_stage_x)->IsStopped()	&&
				  uvEng_ShMem_GetMC2()->GetDriveAct((UINT8)ENG_MMDI::en_stage_y)->IsStopped()	&&
				  uvEng_ShMem_GetMC2()->GetDriveAct((UINT8)ENG_MMDI::en_align_cam1)->IsStopped()&&
				  uvEng_ShMem_GetMC2()->GetDriveAct((UINT8)ENG_MMDI::en_align_cam2)->IsStopped();
	/* 가장 최근 상태 갱신 */
	m_bMotionStop	= !(!bStopped || bRunExpo);

#if 1
	for (i=0; i<9; i++)		m_btn_ctl[i].EnableWindow(m_bMotionStop);
	for (i=10;i<29; i++)	m_btn_ctl[i].EnableWindow(m_bMotionStop);
	m_btn_ctl[5].EnableWindow(!m_bMotionStop);	/* Stop Button */
#if 0
	/* MC2 Drive Check Button */
	for (i=0; i<uvEng_GetConfig()->mc2_svc.drive_count; i++)
	{
		m_chk_drv[pstMC2Svc->axis_id[i]].EnableWindow(m_bMotionStop);
	}
#endif
	for (i=0; i<uvEng_GetConfig()->set_cams.acam_count; i++)
	{
		m_chk_cam[i].EnableWindow(m_bMotionStop);
	}
	for (i=0; i<4; i++)	m_chk_mak[i].EnableWindow(m_bMotionStop);
	for (i=0; i<4; i++)	m_chk_mtd[i].EnableWindow(m_bMotionStop);
	for (i=0; i<1; i++)	m_chk_liv[i].EnableWindow(m_bMotionStop);
#endif
}
#endif
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

	if (!u8ACamID)
	{
		AfxMessageBox(L"There is no camera selected!", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 현재 Live Mode 방식인지 확인 */
	if (m_chk_liv[0].GetCheck())
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
	/* Align Camera is Edge Detection Mode */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);
#if 0
	/* 첫번 행과 열 부분 값 초기화 */
	m_pGridMeas[0].SetItemText(1, 1, L"+0.0");
	m_pGridMeas[1].SetItemText(1, 1, L"+0.0");
	m_pGridMeas[0].InvalidateRowCol(1, 1);
	m_pGridMeas[1].InvalidateRowCol(1, 1);
#endif
	/* 기존 검색된 Grabbed Data & Image 제거 */
	uvEng_Camera_ResetGrabbedMark();
	uvEng_Camera_ResetGrabbedImage();

	/* Trigger 강제로 (수동으로) 발생 */
	if (!uvEng_Trig_ReqTrigOutOne(u8ACamID, TRUE))
	{
		uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
		AfxMessageBox(L"Failed to send the event for trigger", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* 작업 요청 대기 시간 */
	u64StartTick	= GetTickCount64();
	/* 1초 정도 대기 */
	do {
		if (u64StartTick+2000 < GetTickCount64())	break;
		uvCmn_uSleep(100);
	} while (1);

	/* 검색된 결과가 있는지 여부 */
	pstResult	= uvEng_Camera_RunModelCali(u8ACamID, 0xff);
	if (!pstResult->marked)
	{
		AfxMessageBox(L"Failed to analyze the grabbed image", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

#if 0
	/* 화면 갱신 */
	DrawGrabbedImage();
	/* 그리드 컨트롤 갱신 */
	UpdateCaliXY(1, 1, pstResult);
	/* Camera 동작 모드 설정 */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
#endif
}

/*
 desc : 새로운 Job Name 등록
 parm : None
 retn : None
*/
VOID CDlgMain::JobMgtAdd()
{
	TCHAR tzJobName[MAX_PATH_LEN]	= {NULL}, tzRLTFiles[MAX_PATH_LEN] = {NULL};
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;
	CUniToChar csCnv;

	/* Job Name 디렉토리 선택 */
	if (!uvCmn_RootSelectPath(m_hWnd, uvEng_GetConfig()->set_comn.gerber_path, tzJobName))	return;
	/* Job Name 내에 하위 디렉토리가 존재하는지 확인 */
	if (uvCmn_GetChildPathCount(tzJobName) != 0)
	{
		AfxMessageBox(L"This is not a gerber directory", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 해당 디렉토리 내에 rlt_settings.xml 파일이 존재하는지 확인 */
	swprintf_s(tzRLTFiles, MAX_PATH_LEN, L"%s\\rlt_settings.xml", tzJobName);
	if (!uvCmn_FindFile(tzRLTFiles))
	{
		AfxMessageBox(L"File (rlt_settings.xml) does not exist", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 기존에 등록된 거버 파일 중에 동일한 거버가 있는지 여부 확인 */
	if (pstJobMgt->IsJobOnlyFinded(csCnv.Uni2Ansi(tzJobName)))
	{
		AfxMessageBox(L"Job Name already registered exist", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* Luria Service에 등록 작업 진행 */
	if (!uvEng_Luria_ReqAddJobList(tzJobName))
	{
		AfxMessageBox(L"Failed to register Job Name", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : 기존 Job Name 제거
 parm : None
 retn : None
*/
VOID CDlgMain::JobMgtDel()
{
	INT32 i32CurSel	= 0;
	TCHAR tzJobName[MAX_PATH_LEN]	= {NULL};
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;
	CUniToChar csCnv;

	/* List Box에서 선택된 항목 가져오기 */
	i32CurSel	= m_box_ctl[0].GetCurSel();
	if (i32CurSel < 0)
	{
		AfxMessageBox(L"The selected item does not exist", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 선택된 항목이 유효한지 검사 */
	m_box_ctl[0].GetText(i32CurSel, tzJobName);
	if (wcslen(tzJobName) < 1)	return;
	if (0 != wcscmp(tzJobName, csCnv.Ansi2Uni(pstJobMgt->selected_job_name)))
	{
		AfxMessageBox(L"The gerber for removal is not selected in the optics", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 선택된 항목을 거버에서 삭제 */
	if (!uvEng_Luria_ReqSetDeleteSelectedJob())
	{
		AfxMessageBox(L"Failed to delete the selected Gerber Name", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 현재 상태된 Job List 요청 */
	uvEng_Luria_ReqGetJobList();
}

/*
 desc : 현재 상태된 Job List 요청
 parm : None
 retn : None
*/
VOID CDlgMain::JobMgtList()
{
	uvEng_Luria_ReqGetJobList();
}

/*
 desc : 현재 상태된 Job Name 적재 진행
 parm : None
 retn : None
*/
VOID CDlgMain::JobMgtLoad()
{
	INT32 i32CurSel	= 0;
	TCHAR tzJobName[MAX_PATH_LEN]	= {NULL};
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;
	CUniToChar csCnv;

	/* List Box에서 선택된 항목 가져오기 */
	i32CurSel	= m_box_ctl[0].GetCurSel();
	if (i32CurSel < 0)
	{
		AfxMessageBox(L"The selected item does not exist", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 선택된 항목이 유효한지 검사 */
	m_box_ctl[0].GetText(i32CurSel, tzJobName);
	if (wcslen(tzJobName) < 1)	return;
	if (0 != wcscmp(tzJobName, csCnv.Ansi2Uni(pstJobMgt->selected_job_name)))
	{
		AfxMessageBox(L"The gerber is not selected in the optics", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 현재 선택된 Job의 적재 진행 요청 */
	if (!uvEng_Luria_ReqSetLoadSelectedJob())
	{
		AfxMessageBox(L"Failed to load the Gerber Name", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 현재 노광 조건 정보 요청 */
	if (!uvEng_Luria_ReqGetExposureFactor())
	{
		AfxMessageBox(L"Failed to request the Exposure Factor", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : Exposure (Print) 조건 설정 (기본 값)
 parm : None
 retn : None
*/
VOID CDlgMain::SetExpoParamBase()
{
	LPG_CCTV pstCMPS	= &uvEng_GetConfig()->cmps_test;
#if 0
	m_edt_int[0].SetTextToNum(pstCMPS->expo_step_size);
	m_edt_int[1].SetTextToNum(pstCMPS->expo_duty_cycle);
	m_edt_int[2].SetTextToNum(pstCMPS->expo_frame_rate);
#endif
	if (!uvEng_Luria_ReqSetExposureFactor(pstCMPS->expo_step_size,
										  pstCMPS->expo_duty_cycle,
										  pstCMPS->expo_frame_rate/1000.0f))
	{
		AfxMessageBox(L"Failed to set the value required for exposure", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : Exposure (Print) 조건 설정
 parm : None
 retn : None
*/
VOID CDlgMain::SetExpoParam()
{
	UINT8 u8StepSize	= (UINT8)m_edt_int[0].GetTextToNum();
	UINT8 u8DutyCycle	= (UINT8)m_edt_int[1].GetTextToNum();
	UINT16 u16FrameRate	= (UINT16)m_edt_int[2].GetTextToNum();
	LPG_LDEW pstExpo	= &uvEng_ShMem_GetLuria()->exposure;

	/* 현재 설정 값과 이전 값이 다르면 갱신하지 않음 */
	if (u8StepSize != pstExpo->scroll_step_size ||
		u8DutyCycle != pstExpo->led_duty_cycle ||
		u16FrameRate != pstExpo->frame_rate_factor)
	{
		if (!uvEng_Luria_ReqSetExposureFactor(u8StepSize, u8DutyCycle, u16FrameRate/1000.0f))
		{
			AfxMessageBox(L"Failed to set the value required for exposure", MB_ICONSTOP|MB_TOPMOST);
			return;
		}
	}
}

/*
 desc : Exposure (Print) 조건 얻기
 parm : None
 retn : None
*/
VOID CDlgMain::GetExpoParam()
{
	if (!uvEng_Luria_ReqGetExposureFactor())
	{
		AfxMessageBox(L"Failed to get the value required for exposure", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : 현재 Photohead의 LED에 설정된 Level Index 값 요청
 parm : None
 retn : None
*/
VOID CDlgMain::GetLedLightLvl()
{
	UINT8 i		= 0x00;
	UINT8 u8PHs	= uvEng_GetConfig()->luria_svc.ph_count;
	UINT8 u8LED	= uvEng_GetConfig()->luria_svc.led_count;

	/* 기존 값 초기화 */
	for (; i<u8PHs; i++)
	{
		memset(m_pLedAmplitue[i], 0xff, sizeof(UINT16) * u8LED);
	}

	/* 현재 설정되어 있는 값 요청 */
	if (!uvEng_Luria_ReqGetLedAmplitude())
	{
		AfxMessageBox(L"Failed to get the LED level value for photohead", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : Photohead의 LED에 새로운 Level Index 값 설정
 parm : None
 retn : None
*/
VOID CDlgMain::SetLedLightLvl()
{
	UINT8 i	= 0x01;
	UINT16 u16Level	= 0;
	BOOL bSucc	= TRUE;

	for (; i<=uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		u16Level	= (UINT16)m_pGridLed->GetItemTextToInt(i, 1);
		if (bSucc)	bSucc	= uvEng_Luria_ReqSetLedAmplitudeOne(i, 1, u16Level);	/* 365 */
		u16Level	= (UINT16)m_pGridLed->GetItemTextToInt(i, 2);
		if (bSucc)	bSucc	= uvEng_Luria_ReqSetLedAmplitudeOne(i, 2, u16Level);	/* 385 */
		u16Level	= (UINT16)m_pGridLed->GetItemTextToInt(i, 3);
		if (bSucc)	bSucc	= uvEng_Luria_ReqSetLedAmplitudeOne(i, 3, u16Level);	/* 395 */
		u16Level	= (UINT16)m_pGridLed->GetItemTextToInt(i, 4);
		if (bSucc)	bSucc	= uvEng_Luria_ReqSetLedAmplitudeOne(i, 4, u16Level);	/* 405 */
	}
	if (!bSucc)
	{
		AfxMessageBox(L"Failed to set the LED level value for photohead", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : Photohead의 LED에 새로운 Level Index 값을 최소 혹은 정조도 값으로 설정
 parm : mode	- [in] 0x00 : 최소 값 즉, 0 값 입력, 0x01 : 정조도 값 입력
 retn : None
*/
VOID CDlgMain::SetLedLightLevel(UINT8 mode)
{
	UINT8 i	= 0x01, j = 0;
	UINT16 u16Level	= 0;
	BOOL bSucc	= TRUE;

	UINT16 (*pLedId)[MAX_LED]	= uvEng_GetConfig()->cmps_test.ph_led_power_id;

	for (; i<=uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
#if 1
		for (j=1; (bSucc && j<=uvEng_GetConfig()->luria_svc.led_count); j++)
		{
			/* 365, 385, 395, 405 */
			if (mode)	u16Level	= pLedId[i-1][j-1];
			uvEng_Luria_ReqSetLedAmplitudeOne(i, j, u16Level);
		}
#else
		if (mode)	u16Level	= pLedId[i-1][0];
		uvEng_Luria_ReqSetLedAmplitudeOne(i, 1, u16Level);	/* 365 */
		if (mode)	u16Level	= pLedId[i-1][1];
		uvEng_Luria_ReqSetLedAmplitudeOne(i, 2, u16Level);	/* 385 */
		if (mode)	u16Level	= pLedId[i-1][2];
		uvEng_Luria_ReqSetLedAmplitudeOne(i, 3, u16Level);	/* 395 */
		if (mode)	u16Level	= pLedId[i-1][3];
		uvEng_Luria_ReqSetLedAmplitudeOne(i, 4, u16Level);	/* 405 */
#endif
	}
	if (!bSucc)
	{
		AfxMessageBox(L"Failed to set the LED level value for photohead", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : 노광 시작 위치 얻기 혹은 설정
 parm : flag	- [in]  0x00 : 얻기, 0x01 : 설정
 retn : None
*/
VOID CDlgMain::ExpoStartXY(UINT8 flag)
{
	/* 노광이 시작될 위치 (실제 모션 동작 위치가 아니고, Luria에 입력되는 위치) */
	LPG_I4XY pstXY	= &uvEng_ShMem_GetLuria()->machine.table_expo_start_xy[0];
	if (0x00 == flag)
	{
		m_edt_flt[4].SetTextToNum(pstXY->x / 1000.0f, 3, TRUE);	/* 단위: um */
		m_edt_flt[5].SetTextToNum(pstXY->y / 1000.0f, 3, TRUE);	/* 단위: um */
	}
	else
	{
		uvEng_Luria_ReqSetTableExposureStartPos(0x01,
												(UINT32)ROUNDED(m_edt_flt[4].GetTextToDouble() * 1000.0f, 0),	/* unit: mm */
												(UINT32)ROUNDED(m_edt_flt[5].GetTextToDouble() * 1000.0f, 0),	/* unit: mm */
												TRUE);
	}
}

/*
 desc : 광학계 Z 초기화
 parm : None
 retn : None
*/
VOID CDlgMain::PhZAxisInitAll()
{
	/* 중간 위치로 이동 */
	uvEng_Luria_ReqSetMotorPositionInitAll(0x01);
}

/*
 desc : 모든 광학계 및 얼라인 카메라를 소재 두께 대비 노광/마크 인식 포커스 위치로 이동
 parm : None
 retn : None
*/
VOID CDlgMain::MovingMotorFocus()
{
	UINT8 i			= 0x00;
	DOUBLE dbOffset	= 0.0f;
	DOUBLE dbPosZ	= 0.0f;	/* 단위: mm */

	/* 보정 관련 노광 소재 두께 대비, 현재 노광 소재 두께 차이 구하고 */
	dbOffset	= uvEng_GetConfig()->cmps_test.expo_film_thick - uvEng_GetConfig()->set_align.dof_film_thick;
	/* 광학계 위치를 기존 노광 포커스 대비 그 차이만큼 이동하는 작업함 */
	for (i=0; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		/* 광학계 Z 축 Focus 값 얻기 */
		dbPosZ	= uvEng_GetConfig()->luria_svc.ph_z_focus[i] + dbOffset /* 소재 두께 차이만큼 더 혹은 덜 이동 */;
		/* 이동하고자 하는 값이 범위를 벗어났는지 여부 */
		if (!uvCmn_Luria_IsValidPhMoveZRange(dbPosZ))
		{
			AfxMessageBox(L"Out of moving range area", MB_ICONSTOP|MB_TOPMOST);
			return;
		}
		/* 광학계 Z 축 이동 */
		uvEng_Luria_ReqSetMotorAbsPosition(i+1, dbPosZ);
	}

	/* 얼라인 카메라 포커스로 이동 (Basler Camera만 해당됨) */
	if (ENG_VCPK::en_camera_basler_ipv4 != (ENG_VCPK)uvEng_GetConfig()->set_comn.align_camera_kind)	return;
	for (i=0; i<uvEng_GetConfig()->set_cams.acam_count; i++)
	{
		/* 얼라인 카메라 Z 축 Focus 값 얻기 */
		dbPosZ	= uvEng_GetConfig()->acam_focus.acam_z_focus[i] + dbOffset /* 소재 두께 차이만큼 더 혹은 덜 이동 */;
		uvEng_MCQ_SetACamMovePosZ(i+1, 0x00, dbPosZ);
	}
}

/*
 desc : 노광 작업 시작
 parm : None
 retn : None
*/
VOID CDlgMain::WorkStart()
{
	UINT8 u8Work	= GetCheckMethod();

	/* 기존에 동작 중인지 확인 */
	if (m_pMainThread->IsRunWork())	return;

	/* 기존 검색된 Grabbed Data & Image 제거 */
	uvEng_Camera_ResetGrabbedMark();
	uvEng_Camera_ResetGrabbedImage();
	/* Align Mark Grab.Mode */
	if (0x01 != u8Work)	uvEng_Camera_SetCamMode(ENG_VCCM::en_grab_mode);
	else				uvEng_Camera_SetCamMode(ENG_VCCM::en_none);

	/* Align Camera의 Calibration File에 있는 보정 데이터 값 적재 */
	if (0x03 == u8Work)
	{
#if 0
		if (!uvEng_ACamCali_LoadFile(UINT16(m_edt_int[2].GetTextToNum())))
		{
			AfxMessageBox(L"There is no calibration data", MB_ICONSTOP|MB_TOPMOST);
			return;
		}
#endif
	}

	m_pMainThread->SetWorkExpoOnly();
}

/*
 desc : 노광 작업 중지
 parm : None
 retn : None
*/
VOID CDlgMain::WorkStop()
{
	/* 작업 동작 관련 중지 프로세스 실행 */
#if 1
	m_pMainThread->StopWorkJob();
#else
#endif
}

/*
 desc : Shutter 열고/닫기
 parm : None
 retn : None
*/
VOID CDlgMain::ShutterCtrl(UINT8 onoff)
{
	UINT32 u32Addr = (UINT32)ENG_PIOA::en_shutter_open_close;
	/* Write */
	uvEng_MCQ_WriteBits(u32Addr, onoff);
}

/*
 desc : Vacuum On/Off
 parm : None
 retn : None
*/
VOID CDlgMain::VacuumCtrl(UINT8 onoff)
{
	UINT32 u32Addr = (UINT32)ENG_PIOA::en_vacuum_on_off;
	/* Write */
	uvEng_MCQ_WriteBits(u32Addr, onoff);
}

/*
 desc : 기준 소재 두께 대비, 노광 소재 두께 차이만큼 광학계 Up or Down
 parm : None
 retn : None
*/
VOID CDlgMain::PhZAxisMove()
{
	UINT32 i;
	DOUBLE dbPhPos[MAX_PH] = {NULL}, dbBase, dbPos;
	UINT32 u32Thick  = (UINT32)m_edt_int[3].GetTextToNum();
	LPG_CIEA pstCfg = uvEng_GetConfig();

	/* 기준 소재 두께 */
	dbBase	= (u32Thick / 1000.0f) - pstCfg->set_align.dof_film_thick;

	/* 기존 광학계 높이에서 소재 두께 오차 만큼 높이를 ... 가산 혹은 감산 */
	for (i=0; i<pstCfg->luria_svc.ph_count; i++)
	{
		dbPos	= pstCfg->luria_svc.ph_z_focus[i] + dbBase;
		uvEng_Luria_ReqSetMotorAbsPosition(i+1, dbPos);
	}
}

/*
 desc : 현재 설정된 파라미터 기준 노광 에너지 값 출력
 parm : None
 retn : None
*/
VOID CDlgMain::CalcEnergy()
{
	TCHAR tzMesg[128]	= {NULL};
	UINT8 i	= 0;
	DOUBLE dbTotal		= 0.0f;
	LPG_LDEW pstExpo	= &uvEng_ShMem_GetLuria()->exposure;

	for (; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		dbTotal	+= uvCmn_Luria_GetExposeEnergy(pstExpo->scroll_step_size,
											   pstExpo->led_duty_cycle,
											   pstExpo->frame_rate_factor,
											   uvEng_GetConfig()->cmps_test.ph_led_power_wt[i]);
	}

	swprintf_s(tzMesg, 128, L"Average Energy = %.3f\n", dbTotal / DOUBLE(i));
	AfxMessageBox(tzMesg, MB_ICONINFORMATION);
}

/*
 desc : 현재 Trigger Board 조건 (Trig On Time, Strobe On Time, Trig Delay Time)에 따라, 이미지를 Grab 후 저장
 parm : None
 retn : None
*/
VOID CDlgMain::Grabbed2Saved()
{
	/* 반드시 초기화 해줘야 됨 */
	uvEng_Camera_ResetGrabbedImage();

	uvEng_Camera_SetCamMode(ENG_VCCM::en_grab_mode);
	uvEng_Trig_ReqTrigOutOne(GetCheckACam(), TRUE);	/* 트리거 내보낸 후 바로 Disable 처리 */

	/* 트리거 발생 후 일정 시간 지연 */
	UINT64 u64Tick	= GetTickCount64();
	do {

		uvCmn_uSleep(100);
		if ((u64Tick+3000) < GetTickCount64())	return;
		if (uvEng_Camera_GetGrabbedCount())	break;

	} while (1);

	InvalidateView();
}

/*
 desc : 이미지 뷰 영역 갱신
 parm : None
 retn : None
*/
VOID CDlgMain::InvalidateView()
{
	CRect rView;
	m_pic_ctl[4].GetWindowRect(rView);
	ScreenToClient(rView);
	InvalidateRect(rView, TRUE);
}
