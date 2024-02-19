
/*
 desc : Main Dialog
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgMain.h"
#include "MainThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
	m_pMainThread			= NULL;
	m_pGridPos				= NULL;
	m_pGridLed				= NULL;
	m_u32ExpoSpeed			= 0;
	m_u64TickLoadState		= 0;
	m_u64TickExpoState		= 0;
	m_pLedAmplitue			= NULL;
	m_enExpoStatus			= ENG_LPES::en_not_defined;
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

	/* Normal - Group box */
	u32StartID	= IDC_MAIN_GRP_MC2;
	for (i=0; i<9; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Normal - Text */
	u32StartID	= IDC_MAIN_TXT_MC2_MOVE;
	for (i=0; i<29; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Normal - Button */
	u32StartID	= IDC_MAIN_BTN_MC2_MOVE_UP;
	for (i=0; i<26; i++)	DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Normal - Checkbox */
	u32StartID	= IDC_MAIN_CHK_MC2_DRV_0;
	for (i=0; i<8; i++)		DDX_Control(dx, u32StartID+i,	m_chk_mc2[i]);
	/* Normal - Checkbox */
	u32StartID	= IDC_MAIN_CHK_POS_R;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_chk_pos[i]);
	/* Normal - Combo box*/
	u32StartID	= IDC_MAIN_CMB_PH_Z_NO;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_cmb_ctl[i]);
	/* Normal - Progress */
	u32StartID	= IDC_MAIN_PGR_PRINT_RATE;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_pgs_ctl[i]);
	/* Normal - List box */
	u32StartID	= IDC_MAIN_BOX_JOBS_LIST;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_box_ctl[i]);
	/* Normal - Edit box for float */
	u32StartID	= IDC_MAIN_EDT_MC2_DIST;
	for (i=0; i<10; i++)	DDX_Control(dx, u32StartID+i,	m_edt_flt[i]);
	/* Normal - Edit box for string */
	u32StartID	= IDC_MAIN_EDT_JOBS_STATUS;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_edt_str[i]);
	/* Normal - Edit box for integer */
	u32StartID	= IDC_MAIN_EDT_PRINT_COUNT;
	for (i=0; i<4; i++)		DDX_Control(dx, u32StartID+i,	m_edt_int[i]);
}

BEGIN_MESSAGE_MAP(CDlgMain, CMyDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MAIN_THREAD,	OnMsgThread)
	ON_LBN_SELCHANGE(IDC_MAIN_BOX_JOBS_LIST, OnBoxSelchangeJobList)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_CHK_POS_R,		IDC_MAIN_CHK_POS_A,	OnChkClicked)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_BTN_MC2_MOVE_UP,	IDC_MAIN_BTN_EXIT,	OnBtnClicked)
END_MESSAGE_MAP()

/*
 desc : 시스템 명령어 제어
 parm : id		- 선택된 항목 ID 값
		lparam	- ???
 retn : 1 - 성공 / 0 - 실패
*/
VOID CDlgMain::OnSysCommand(UINT32 id, LPARAM lparam)
{

	CMyDialog::OnSysCommand(id, lparam);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::OnInitDlg()
{
	/* 실행되는 프로그램을 Sub Monitor에 출력 */
	/*uvCmn_MoveSubMonitor(m_hWnd);*/

	/* 라이브러리 초기화 */
	if (!InitLib())	return FALSE;

	/* 메모리 초기화 */
	InitMem();
	/* 컨트롤 초기화 */
	InitCtrl();
	InitSetup();
	/* 컨트롤 초기화 */
	InitGridPos();
	InitGridLed();

	/* 기본 환경 설정 정보 적재 */
	LoadDataConfig();
#if 0
	uvEng_ACamCali_LoadFile(900);
#endif

	/* Main Thread 생성 (!!! 중요. InitLib 가 성공하면, 반드시 생성해야 함 !!!) */
	m_pMainThread	= new CMainThread(m_hWnd);
	ASSERT(m_pMainThread);
	if (!m_pMainThread->CreateThread(0, NULL, MEDIUM_THREAD_SPEED))
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
	UINT8 i	= 0x00;

	/* 기본 감시 스레드 메모리 해제 */
	if (m_pMainThread)
	{
		m_pMainThread->Stop();
		while (m_pMainThread->IsBusy())	m_pMainThread->Sleep();
		delete m_pMainThread;
		m_pMainThread	= NULL;
	}

	/* 메모리 해제 */
	for (; i<uvEng_GetConfig()->luria_svc.ph_count; i++)	::Free(m_pLedAmplitue[i]);
	::Free(m_pLedAmplitue);

	/* 라이브러리 해제 !!! 맨 마지막에 코딩 !!! */
	CloseLib();
	/* GridControl 해제 */
	CloseGrid();
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgMain::OnPaintDlg(CDC *dc)
{
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

	/* Normal - Group box */
	for (i=0; i<9; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* Normal - Text */
	for (i=0; i<29; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(TRUE);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* Normal - Button */
	for (i=0; i<26; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
	}
	if (0x01 != uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		m_btn_ctl[7].EnableWindow(FALSE);
		m_btn_ctl[24].EnableWindow(FALSE);
	}
	/* Normal - Check box */
	for (i=0; i<8; i++)
	{
		m_chk_mc2[i].SetLogFont(g_lf);
		m_chk_mc2[i].SetBgColor(RGB(0xff, 0xff, 0xff));
		m_chk_mc2[i].SetReadOnly(FALSE);
		m_chk_mc2[i].EnableWindow(FALSE);
	}
	/* Normal - Check box */
	for (i=0; i<2; i++)
	{
		m_chk_pos[i].SetLogFont(g_lf);
		m_chk_pos[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
	m_chk_pos[0].SetCheck(1);

	/* Normal - Combo box */
	for (i=0; i<1; i++)
	{
		m_cmb_ctl[i].SetLogFont(&g_lf);
	}
	/* Progress - normal */
	for (i=0; i<1; i++)
	{
		m_pgs_ctl[i].SetColor(RGB(160, 255, 0));
	}
	/* List box - normal */
	for (i=0; i<1; i++)
	{
		m_box_ctl[i].SetLogFont(&g_lf);
	}
	/* Edit box - float */
	for (i=0; i<10; i++)
	{
		m_edt_flt[i].SetEditFont(&g_lf);
		m_edt_flt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_flt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_flt[i].SetInputType(ENM_DITM::en_float);
		if (i==7)
		{
			m_edt_flt[i].SetMouseClick(FALSE);
			m_edt_flt[i].SetReadOnly(TRUE);
		}
	}
	/* Edit box - string */
	for (i=0; i<2; i++)
	{
		m_edt_str[i].SetEditFont(&g_lf);
		m_edt_str[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_str[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_str[i].SetInputType(ENM_DITM::en_string);
		m_edt_str[i].SetReadOnly(TRUE);
		m_edt_str[i].SetMouseClick(FALSE);
	}
	/* Edit box - Integer */
	for (i=0; i<4; i++)
	{
		m_edt_int[i].SetEditFont(&g_lf);
		m_edt_int[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_int[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_int[i].SetInputType(ENM_DITM::en_int8);
	}
}

/*
 desc : 초기 컨트롤 설정
 parm : None
 retn : None
*/
VOID CDlgMain::InitSetup()
{
	UINT32 i	= 0;

	/* Combo Box 설정 */
	m_cmb_ctl[0].ResetContent();
	for (i=0; i<uvEng_Conf_GetLuria()->ph_count; i++)
	{
		m_cmb_ctl[0].AddNumber(UINT64(i+1));
	}

	/* 광학계 선택 콤보 박스 */
	m_cmb_ctl[0].SetCurSel(0);

}

/*
 desc : Library Init
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::InitLib()
{
	if (!uvEng_Init(ENG_ERVM::en_cali_vdof))	return FALSE;

	return TRUE;
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
 desc : 그리드 컨트롤 생성 - Motor Position
 parm : None
 retn : None
*/
VOID CDlgMain::InitGridPos()
{
	TCHAR tzCols[3][16]	= { L"Item", L"Pos (mm)" }, tzRows[32] = {NULL};
	INT32 i32Width[3]	= { 56, 80 }, i, j=1;
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
	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		i32RowCnt	= pstCfg->luria_svc.ph_count + pstCfg->mc2_svc.drive_count;
	}
	else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		i32RowCnt	= pstCfg->mc2_svc.drive_count;
	}
	/* 그리드 컨트롤 기본 공통 속성 */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* 객체 생성 */
	m_pGridPos	= new CGridCtrl;
	ASSERT(m_pGridPos);
	m_pGridPos->SetDrawScrollBarHorz(FALSE);
	m_pGridPos->SetDrawScrollBarVert(TRUE);
	if (!m_pGridPos->Create(rGrid, this, IDC_GRID_MOTOR_POS, dwStyle))
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
	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
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
			stGV.strText= L"0.000";
			stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
			stGV.col	= 1;
			m_pGridPos->SetItem(&stGV);
		}
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

	m_pGridPos->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGridPos->UpdateGrid();
}

/*
 desc : 컨트롤 초기화 - Photohead LED
 parm : None
 retn : None
*/
VOID CDlgMain::InitGridLed()
{
	TCHAR tzCols[5][8]	= { L"Item", L"LED1", L"LED2", L"LED3", L"LED4" }, tzRows[32] = {NULL};
	INT32 i32Width[5]	= { 40, 45, 45, 45, 45 }, i, j;
	INT32 i32ColCnt		= 5, i32RowCnt = uvEng_GetConfig()->luria_svc.ph_count;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* Last Recv. Packet / PDO Header / PDO Data (Values) */
	m_grp_ctl[7].GetWindowRect(rGrid);
	ScreenToClient(rGrid);
	rGrid.left	+= 14;
	rGrid.right	-= 85;
	rGrid.top	+= 24;
	rGrid.bottom-= 16;

	/* 그리드 컨트롤 기본 공통 속성 */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* 객체 생성 */
	m_pGridLed	= new CGridCtrl;
	ASSERT(m_pGridLed);
	m_pGridLed->SetDrawScrollBarHorz(FALSE);
	m_pGridLed->SetDrawScrollBarVert(TRUE);
	if (!m_pGridLed->Create(rGrid, this, IDC_GRID_PH_LED, dwStyle))
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
 desc : 체크 클릭 이벤트
 parm : id	- [in]  Click된 버튼 ID
 retn : None
*/
VOID CDlgMain::OnChkClicked(UINT32 id)
{
	if (id == IDC_MAIN_CHK_POS_R)
	{
		m_chk_pos[0].SetCheck(1);
		m_chk_pos[1].SetCheck(0);
	}
	else
	{
		m_chk_pos[0].SetCheck(0);
		m_chk_pos[1].SetCheck(1);
	}
}

/*
 desc : 버튼 클릭 이벤트
 parm : id	- [in]  Click된 버튼 ID
 retn : None
*/
VOID CDlgMain::OnBtnClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_MAIN_BTN_EXIT			:	PostMessage(WM_CLOSE, 0, 0L);					break;
	case IDC_MAIN_BTN_JOBS_ADD		:	JobMgtAdd();									break;
	case IDC_MAIN_BTN_JOBS_DEL		:	JobMgtDel();									break;
	case IDC_MAIN_BTN_JOBS_LIST		:	JobMgtList();									break;
	case IDC_MAIN_BTN_JOBS_LOAD		:	JobMgtLoad();									break;
	case IDC_MAIN_BTN_EXPO_SET		:	SetExpoParam();									break;
	case IDC_MAIN_BTN_EXPO_GET		:	GetExpoParam();									break;
	case IDC_MAIN_BTN_MC2_RESET		:	MC2Recovery();									break;
	case IDC_MAIN_BTN_MC2_MOVE_UP	:	MotionMove(ENG_MDMD::en_move_up);				break;
	case IDC_MAIN_BTN_MC2_MOVE_DN	:	MotionMove(ENG_MDMD::en_move_down);				break;
	case IDC_MAIN_BTN_START			:	StartPrint();									break;
	case IDC_MAIN_BTN_STOP			:	StopPrint();									break;
	case IDC_MAIN_BTN_SAVE			:	SaveDataToFile();								break;
	case IDC_MAIN_BTN_LED_GET		:	GetLedLightLvl();								break;
	case IDC_MAIN_BTN_LED_SET		:	SetLedLightLvl();								break;
	case IDC_MAIN_BTN_LED_DEFAULT	:	SetLedLightDefault();							break;
	case IDC_MAIN_BTN_OPTIC_ERROR	:	ResetOpticError();								break;
	case IDC_MAIN_BTN_START_GET		:	ExpoStartXY(0);									break;
	case IDC_MAIN_BTN_START_SET		:	ExpoStartXY(1);									break;
	case IDC_MAIN_BTN_PH_INIT		:	PhZAxisInitAll();								break;
	case IDC_MAIN_BTN_PH_Z_UP		:	PhZAxisUpDown(0x00);							break;
	case IDC_MAIN_BTN_PH_Z_DOWN		:	PhZAxisUpDown(0x01);							break;
	case IDC_MAIN_BTN_WORK_POS		:	UnloadPosition();								break;
	case IDC_MAIN_BTN_FOCUS_MOVE	:	PHMovingFocus();								break;
	case IDC_MAIN_BTN_OPTIC_INIT	:	PHHWInit();										break;
	case IDC_MAIN_BTN_PH_Z_GET		:	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
											uvEng_Luria_ReqGetMotorAbsPositionAll();	break;
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
		AfxMessageBox(L"Job Name already selected", MB_ICONSTOP|MB_TOPMOST);
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
LRESULT CDlgMain::OnMsgThread(WPARAM wparam, LPARAM lparam)
{
	switch (wparam)
	{
	case MSG_ID_NORMAL			: UpdatePeriod();			break;
	case MSG_ID_PRINT_ONE		: ;	break;
	case MSG_ID_PRINT_COMPLETE	: ;	break;
	}

	return 0L;
}

/*
 desc : 주기적으로 갱신되는 항목
 parm : None
 retn : None
*/
VOID CDlgMain::UpdatePeriod()
{
	/* Job List 갱신 */
	UpdateJobList();
	/* Job Load 갱신 */
	UpdateJobLoad();
	/* Exposure 갱신 */
	UpdateExposure();
	/* LED Power 갱신 */
	UpdateLedAmpl();
	/* for Motor Position (mm) */
	UpdateMotorPos();
	/* Control : Enable/Disable */
	UpdateEnableCtrl();
	/* Etc */
	UpdateEtcValue();
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
			if (pstJobMgt->IsJobFullFinded(csCnv.Uni2Ansi(tzGerberName)))	u8Find++;
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
		m_edt_str[0].SetTextToStr(L"None",	TRUE);
		return;
	}

	switch (pstJobMgt->selected_job_load_state)
	{
	case ENG_LSLS::en_load_none			:	m_edt_str[0].SetTextToStr(L"None",		TRUE);	break;
	case ENG_LSLS::en_load_not_started	:	m_edt_str[0].SetTextToStr(L"Ready",		TRUE);	break;
	case ENG_LSLS::en_loading			:	m_edt_str[0].SetTextToStr(L"Loading",	TRUE);	break;
	case ENG_LSLS::en_load_completed	:	m_edt_str[0].SetTextToStr(L"Loaded",	TRUE);	break;
	case ENG_LSLS::en_load_failed		:	m_edt_str[0].SetTextToStr(L"Failed",	TRUE);	break;
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
 desc : Exposure 상태 갱신 (Print Speed, Step Size, Frame Rate, Duty Cycle, Printing, Status, ...)
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateExposure()
{
	LPG_LDEW pstExpo	= &uvEng_ShMem_GetLuria()->exposure;

	/* 이전 값과 동일하면, 더 이상 처리하지 않음 */
	if ((UINT8)m_enExpoStatus != pstExpo->get_exposure_state[0])
	{
		/* Exposure State */
		switch (pstExpo->get_exposure_state[0])
		{
		case ENG_LPES::en_not_defined		:	m_edt_str[1].SetTextToStr(L"Not defined", TRUE);	break;
		case ENG_LPES::en_idle				:	m_edt_str[1].SetTextToStr(L"Idle", TRUE);			break;

		case ENG_LPES::en_preprint_running	:	m_edt_str[1].SetTextToStr(L"Pre-Print Running");	break;
		case ENG_LPES::en_preprint_success	:	m_edt_str[1].SetTextToStr(L"Pre-Print Success");	break;
		case ENG_LPES::en_preprint_failed	:	m_edt_str[1].SetTextToStr(L"Pre-Print Failed");		break;	

		case ENG_LPES::en_print_running		:	m_edt_str[1].SetTextToStr(L"Print Running");		break;
		case ENG_LPES::en_print_success		:	m_edt_str[1].SetTextToStr(L"Print Success");		break;
		case ENG_LPES::en_print_failed		:	m_edt_str[1].SetTextToStr(L"Print Failed");			break;
#if 0	/* Not used */
		case ENG_LPES::en_simul_running		:	m_edt_str[1].SetTextToStr(L"Simulate Running");		break;
		case ENG_LPES::en_simul_success		:	m_edt_str[1].SetTextToStr(L"Simulate Success");		break;
		case ENG_LPES::en_simul_failed		:	m_edt_str[1].SetTextToStr(L"Simulate Failed");		break;
#endif
		case ENG_LPES::en_abort_in_progress	:	m_edt_str[1].SetTextToStr(L"Abort Running");		break;
		case ENG_LPES::en_abort_success		:	m_edt_str[1].SetTextToStr(L"Abort Success");		break;
		case ENG_LPES::en_abort_failed		:	m_edt_str[1].SetTextToStr(L"Abort Failed");			break;
		}
		/* 가장 최근 값 임시 저장 */
		m_enExpoStatus	= (ENG_LPES)pstExpo->get_exposure_state[0];
	}

	/* 현재 노광 진행 횟수 갱신 */
	if (m_pMainThread->IsRunPrint())
	{
		m_edt_int[0].SetTextToNum(m_pMainThread->GetPrintRest());
	}


	/* Exposure Speed / Step Size / Frame Rate / Duty Cycle */
	if (m_u32ExpoSpeed != pstExpo->get_exposure_speed)
	{
		/* Exposure Speed */
		m_edt_flt[7].SetTextToNum(DOUBLE(pstExpo->get_exposure_speed/1000.0f), 3, TRUE);
		/* Step Size */
		m_edt_int[1].SetTextToNum(pstExpo->scroll_step_size, TRUE);
		/* Frame Rate */
		m_edt_flt[8].SetTextToNum(DOUBLE(pstExpo->frame_rate_factor/1000.0f), 3, TRUE);
		/* Duty Cycle */
		m_edt_int[2].SetTextToNum(pstExpo->led_duty_cycle, TRUE);

		/* 가장 최근 값 임시 저장 */
		m_u32ExpoSpeed = pstExpo->get_exposure_speed;
	}
	/* 전체 노광 진행률 표현 */
	if (m_pMainThread->IsRunPrint())
	{
		DOUBLE dbTotal	= m_pMainThread->GetStepTotal() * m_pMainThread->GetPrintTotal();
		DOUBLE dbStep	= m_pMainThread->GetPrintCount() * m_pMainThread->GetStepTotal() +
						  m_pMainThread->GetStepCount();
		m_pgs_ctl[0].SetPos((INT32)ROUNDED(dbStep/dbTotal, 0));
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
	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* Luria Photohead 위치 */
		for (i=0; i<pstCfg->luria_svc.ph_count; i++, j++)
		{
			dbPos[0] = m_pGridPos->GetItemTextToFloat(j, 1);
			dbPos[1] = (DOUBLE)ROUNDED(pstDP->focus_motor_move_abs_position[i]/1000.0f, 3);
			if (dbPos[0] != dbPos[1])
			{
				m_pGridPos->SetItemDouble(j, 1, dbPos[1], 3);
				m_pGridPos->InvalidateRowCol(j, 1);
			}
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
}

/*
 desc : 각종 컨트롤 Enable or Disable
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateEnableCtrl()
{
	UINT8 i	= 0x00;
	BOOL bBusyMotion	= FALSE, bBusyRunPrint = FALSE;
	LPG_CMSI pstMC2Svc	= &uvEng_GetConfig()->mc2_svc;

	/* Focus 측정 진행 여부 */
	bBusyRunPrint	= m_pMainThread->IsRunPrint();
	/* Motion Busy 여부 */
	bBusyMotion		= uvCmn_MC2_IsAnyDriveBusy();
	/* MC2 Up/Down Moving Button */
	for (i=0x00; i<0x02; i++)
	{
		m_btn_ctl[i].EnableWindow(!(bBusyMotion || bBusyRunPrint));
	}

	/* MC2 Drive Check Button */
	for (i=0x00; i<uvEng_GetConfig()->mc2_svc.drive_count; i++)
	{
		m_chk_mc2[pstMC2Svc->axis_id[i]].EnableWindow(!(bBusyMotion || bBusyRunPrint));
	}

	/* 반복 동작 횟수 */
	m_edt_int[0].EnableWindow(!bBusyRunPrint);

	/* Normal Button */
	m_btn_ctl[4].EnableWindow(!(bBusyMotion  || bBusyRunPrint));	/* MC2 Reset */
	m_btn_ctl[5].EnableWindow(!bBusyRunPrint);						/* Photohead Z Axis Up */
	m_btn_ctl[6].EnableWindow(!bBusyRunPrint);						/* Photohead Z Axis Down */
	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		m_btn_ctl[7].EnableWindow(!bBusyRunPrint);						/* Photohead Z Axis Homing */
		m_btn_ctl[24].EnableWindow(!bBusyRunPrint);
	}
	m_btn_ctl[8].EnableWindow(!(bBusyMotion  || bBusyRunPrint));	/* Start */
	m_btn_ctl[9].EnableWindow(bBusyRunPrint);						/* Stop */
	m_btn_ctl[10].EnableWindow(!(bBusyMotion || bBusyRunPrint));	/* Save */
	m_btn_ctl[11].EnableWindow(!(bBusyMotion || bBusyRunPrint));	/* Job Add */
	m_btn_ctl[12].EnableWindow(!(bBusyMotion || bBusyRunPrint));	/* Job Del */
	m_btn_ctl[13].EnableWindow(!bBusyRunPrint);						/* Job Load */
	m_btn_ctl[14].EnableWindow(!bBusyRunPrint);						/* Set Expose Param */
	m_btn_ctl[15].EnableWindow(!bBusyRunPrint);						/* Get Expose Param */
	m_btn_ctl[16].EnableWindow(!bBusyRunPrint);						/* LED Get */
	m_btn_ctl[17].EnableWindow(!bBusyRunPrint);						/* LED Set */
	m_btn_ctl[19].EnableWindow(!(bBusyMotion || bBusyRunPrint));	/* Work Position */
	m_btn_ctl[20].EnableWindow(!bBusyRunPrint);						/* Job List */
	m_btn_ctl[21].EnableWindow(!bBusyRunPrint);						/* PH Focus Moving */
	m_btn_ctl[22].EnableWindow(!bBusyRunPrint);						/* Set the MAX LED */
	m_btn_ctl[23].EnableWindow(!bBusyRunPrint);
}

/*
 desc : 기타 값 갱신
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateEtcValue()
{
	/* 가장 최근의 Luria Serivce 에러 값 출력 */
	m_edt_int[3].SetTextToNum(uvCmn_Luria_GetLastErrorStatus(), TRUE);

	/* 체크 버튼에 따라 입력 여부 */
	if (m_chk_pos[0].GetCheck())
	{
		m_edt_flt[4].EnableWindow(TRUE);
		m_edt_flt[9].EnableWindow(FALSE);
		m_btn_ctl[6].EnableWindow(TRUE);
	}
	else
	{
		m_edt_flt[4].EnableWindow(FALSE);
		m_edt_flt[9].EnableWindow(TRUE);
		m_btn_ctl[6].EnableWindow(FALSE);
	}
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
 desc : Exposure (Print) 조건 설정
 parm : None
 retn : None
*/
VOID CDlgMain::SetExpoParam()
{
	UINT8 u8StepSize	= (UINT8)m_edt_int[1].GetTextToNum();
	UINT8 u8DutyCycle	= (UINT8)m_edt_int[2].GetTextToNum();
	UINT16 u16FrameRate	= (UINT16)ROUNDED(m_edt_flt[8].GetTextToDouble() * 1000.0f, 0);
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
		memset(m_pLedAmplitue[i], 0x00, sizeof(UINT16) * u8LED);
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
		if (bSucc)	bSucc	= uvEng_Luria_ReqSetLedAmplitudeOne(i, ENG_LLPI(1), u16Level);	/* 365 */
		u16Level	= (UINT16)m_pGridLed->GetItemTextToInt(i, 2);
		if (bSucc)	bSucc	= uvEng_Luria_ReqSetLedAmplitudeOne(i, ENG_LLPI(2), u16Level);	/* 385 */
		u16Level	= (UINT16)m_pGridLed->GetItemTextToInt(i, 3);
		if (bSucc)	bSucc	= uvEng_Luria_ReqSetLedAmplitudeOne(i, ENG_LLPI(3), u16Level);	/* 395 */
		u16Level	= (UINT16)m_pGridLed->GetItemTextToInt(i, 4);
		if (bSucc)	bSucc	= uvEng_Luria_ReqSetLedAmplitudeOne(i, ENG_LLPI(4), u16Level);	/* 405 */
	}
	if (!bSucc)
	{
		AfxMessageBox(L"Failed to set the LED level value for photohead", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : Photohead의 LED에 새로운 Level Index 값을 정조도 값으로 설정
 parm : None
 retn : None
*/
VOID CDlgMain::SetLedLightDefault()
{
#if 0
	UINT8 i	= 0x01, j;
	UINT16 u16Level	= 0;
	BOOL bSucc	= TRUE;
	UINT16 (*pLedId)[MAX_LED]	= uvEng_GetConfig()->cmps_test.ph_led_power_id;

	for (; (bSucc && i<=uvEng_GetConfig()->luria_svc.ph_count); i++)
	{
#if 1
		for (j=1; (bSucc && j<=uvEng_GetConfig()->luria_svc.led_count); j++)
		{
			/* 365, 385, 395, 405 */
			if (bSucc)	bSucc	= uvEng_Luria_ReqSetLedAmplitudeOne(i, j, pLedId[i-1][j-1]);
		}
#else
		if (bSucc)	bSucc	= uvEng_Luria_ReqSetLedAmplitudeOne(i, 1, pLedId[i-1][0]);	/* 365 */
		if (bSucc)	bSucc	= uvEng_Luria_ReqSetLedAmplitudeOne(i, 2, pLedId[i-1][1]);	/* 385 */
		if (bSucc)	bSucc	= uvEng_Luria_ReqSetLedAmplitudeOne(i, 3, pLedId[i-1][2]);	/* 395 */
		if (bSucc)	bSucc	= uvEng_Luria_ReqSetLedAmplitudeOne(i, 4, pLedId[i-1][3]);	/* 405 */
#endif
	}
	if (!bSucc)
	{
		AfxMessageBox(L"Failed to set the LED level value for photohead", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
#endif
}

/*
 desc : Optics (Luria) 가장 최근의 에러 값 초기화
 parm : None
 retn : None
*/
VOID CDlgMain::ResetOpticError()
{
	uvCmn_Luria_ResetLastErrorStatus();
	m_edt_int[3].SetTextToStr(L"", TRUE);
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
		m_edt_flt[2].SetTextToNum(pstXY->x / 1000.0f, 3, TRUE);	/* 단위: um */
		m_edt_flt[3].SetTextToNum(pstXY->y / 1000.0f, 3, TRUE);	/* 단위: um */
	}
	else
	{
		uvEng_Luria_ReqSetTableExposureStartPos(0x01,
												(UINT32)ROUNDED(m_edt_flt[2].GetTextToDouble() * 1000.0f, 0),	/* unit: um */
												(UINT32)ROUNDED(m_edt_flt[3].GetTextToDouble() * 1000.0f, 0),	/* unit: um */
												TRUE);
	}
}

/*
 desc : 광학계 Z 초기화 (Homing)
 parm : None
 retn : None
*/
VOID CDlgMain::PhZAxisInitAll()
{
	/* 중간 위치로 이동 */
	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		uvEng_Luria_ReqSetMotorPositionInitAll(0x01);
	}
	else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		uvEng_MC2_SendDevHoming(ENG_MMDI::en_axis_ph1);
		uvEng_MC2_SendDevHoming(ENG_MMDI::en_axis_ph2);
	}
}

/*
 desc : 모든 광학계를 Focus 위치로 이동
 parm : None
 retn : None
*/
VOID CDlgMain::PHMovingFocus()
{
	UINT8 i	= 0x00, u8Drv = UINT8(ENG_MMDI::en_axis_ph1);
	DOUBLE dbPosZ, dbVelo = uvEng_GetConfig()->mc2_svc.max_velo[u8Drv];	/* 단위: mm */

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
			uvEng_MC2_SendDevAbsMove(ENG_MMDI(u8Drv+i), dbPosZ, dbVelo);
		}
	}
}

/*
 desc : 모든 광학계 HW Init 위치로 이동
 parm : None
 retn : None
*/
VOID CDlgMain::PHHWInit()
{
	BOOL bSucc	= FALSE;
	/* MC2가 초기화 되었는지 (Error가 없는지) 여부 */
	if (uvCmn_MC2_IsAnyDriveError())
	{
		AfxMessageBox(L"An error has occurred in MC2", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* PH의 Z 축 모션이 초기화 되었는지 여부 */
	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		bSucc	= uvCmn_Luria_IsAllPhZAxisMidPosition();
	}
	else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		bSucc	= uvCmn_MC2_IsDriveHomed(ENG_MMDI::en_axis_ph1) &&
				  uvCmn_MC2_IsDriveHomed(ENG_MMDI::en_axis_ph2);
	}
	if (!bSucc)
	{
		AfxMessageBox(L"The motion Z axis of the optical system is not initialized", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* Luria Service에 거버가 등록되어 있는지 여부 */
	if (uvCmn_Luria_GetJobCount() > 0)
	{
		AfxMessageBox(L"A registered Job Name exists", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* Luria Service 초기화 */
	if (!uvEng_Luria_ReqSetHWInit())
	{
		AfxMessageBox(L"The HW initialization of the service failed", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : 광학계 Z 축 업 / 다운
 parm : flag	- [in]  0x00 : UP, 0x01 : DOWN
 retn : None
*/
VOID CDlgMain::PhZAxisUpDown(UINT8 flag)
{
	UINT8 i	= 0x00, u8PhNo, u8Drv = UINT8(ENG_MMDI::en_axis_ph1);;
	DOUBLE dbMove	= m_edt_flt[4].GetTextToDouble();	/* mm */
	DOUBLE dbPosZ	= 0.0f, dbVelo = uvEng_GetConfig()->mc2_svc.max_velo[u8Drv];	/* 단위: mm */

	if (flag)	dbMove	*= -1.0f;

	/* 현재 선택된 광학계 번호 */
	if (m_cmb_ctl[0].GetCurSel() < 0)	return;
	u8PhNo	= (UINT8)(m_cmb_ctl[0].GetCurSel() + 1);

	/* 절대 좌표로 이동하는지 상대 좌표로 이동하는지 */
	if (m_chk_pos[0].GetCheck())
	{
		/* 현재 광학계의 Z 축 위치 값 */
		dbPosZ	= uvEng_ShMem_GetLuria()->directph.focus_motor_move_abs_position[u8PhNo-1]/1000.0f;
		/* 최종 이동 값 계산 */
		dbPosZ	+= dbMove;
	}
	else
	{
		dbPosZ	= m_edt_flt[9].GetTextToDouble();
	}

	/* 이동하고자 하는 값이 범위를 벗어났는지 여부 */
	if (!uvCmn_Luria_IsValidPhMoveZRange(dbPosZ))
	{
		AfxMessageBox(L"Out of moving range area", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 광학계 Z 축 이동 */
	else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		uvEng_Luria_ReqSetMotorAbsPosition(u8PhNo, dbPosZ);
	}
	else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		uvEng_MC2_SendDevAbsMove(ENG_MMDI(u8Drv+u8PhNo-1), dbPosZ, dbVelo);
	}
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
 desc : MC2의 모든 Motor 초기화 (복구) (Homing)
 parm : None
 retn : None
*/
VOID CDlgMain::MC2Recovery()
{
	ENG_MMDI enDrvID;

	enDrvID	= m_chk_mc2[0].GetCheck() ? ENG_MMDI::en_stage_x : ENG_MMDI::en_axis_none;
	if (enDrvID != ENG_MMDI::en_axis_none)	uvEng_MC2_SendDevHoming(enDrvID);
	enDrvID	= m_chk_mc2[1].GetCheck() ? ENG_MMDI::en_stage_y : ENG_MMDI::en_axis_none;
	if (enDrvID != ENG_MMDI::en_axis_none)	uvEng_MC2_SendDevHoming(enDrvID);
	enDrvID	= m_chk_mc2[4].GetCheck() ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_axis_none;
	if (enDrvID != ENG_MMDI::en_axis_none)	uvEng_MC2_SendDevHoming(enDrvID);
	enDrvID	= m_chk_mc2[5].GetCheck() ? ENG_MMDI::en_align_cam2 : ENG_MMDI::en_axis_none;
	if (enDrvID != ENG_MMDI::en_axis_none)	uvEng_MC2_SendDevHoming(enDrvID);
}

/*
 desc : 모션 (Motor Drive) 이동
 parm : direct	- [in]  Up or Down
 retn : None
*/
VOID CDlgMain::MotionMove(ENG_MDMD direct)
{
	UINT8 i	= 0x00, u8DrvNo = 0x00;	/* 0x00 ~ 0x07 */
	DOUBLE dbDrvPos	= 0, dbMinPos, dbMaxPos, dbDist, dbVelo	= 0.0f;
	BOOL bCheck		= FALSE;
	LPG_CMSI pstCfg	= &uvEng_GetConfig()->mc2_svc;
	LPG_MDSM pstMC2	= uvEng_ShMem_GetMC2();

	/* 이동하고자 하는 Drive가 체크 되어 있는지 확인 */
	for (i=0; i<pstCfg->drive_count && !bCheck; i++)
	{
		bCheck	= m_chk_mc2[pstCfg->axis_id[i]].GetCheck() > 0;
	}
	if (!bCheck)
	{
		AfxMessageBox(L"The drive to be moved is not checked", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 이동하고자 하는 속도와 거리 값이 입력되어 있는지 확인 (여기서는 Step 이동만 해당 됨) */
	dbDist	= m_edt_flt[0].GetTextToDouble();
	dbVelo	= m_edt_flt[1].GetTextToDouble();
	if (dbDist <= 0.0f || dbVelo <= 0.0f)
	{
		AfxMessageBox(L"Check the move distance and speed values", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* 현재 위치에서 이동 거리 값이 Min or Max 값을 초과 했다면, Min or Max 값 만큼 이동 */
	for (i=0; i<pstCfg->drive_count; i++)
	{
		/* 드라이브가 체크 되어 있는지 여부 확인 */
		u8DrvNo	= pstCfg->axis_id[i];
		if (m_chk_mc2[u8DrvNo].GetCheck())
		{
			/* 현재 드라이브의 위치 확인 */
			dbDrvPos	= pstMC2->act_data[u8DrvNo].real_position / 10000.0f;	/* 100 nm or 0.1um --> mm */
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
}

/*
 desc : Print 시작
 parm : None
 retn : None
*/
VOID CDlgMain::StartPrint()
{
	TCHAR tzMesg[LOG_MESG_SIZE]	= {NULL};
	UINT8 i, u8Drv		= UINT8(ENG_MMDI::en_axis_ph1);
	UINT16 u16Repeat	= 0;
	DOUBLE dbPeriodZ	= 0.0f, dbPeriodY, dbLastPos = 0.0f;

	/* 확인 메시지 */
	if (AfxMessageBox(L"Did you check the Z axis ?\n"
					  L"Did you check the Print Energy?", MB_YESNO) != IDYES)	return;
	/* 반복 개수 얻기 */
	u16Repeat	= (UINT16)m_edt_int[0].GetTextToNum();
	if (u16Repeat < 1)
	{
		AfxMessageBox(L"No repeat count value entered", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 광학계 Z 축 이동 간격 (음수:Move Down, 양수:Move Up) */
	dbPeriodZ	= m_edt_flt[5].GetTextToDouble();	/* mm */
	dbPeriodY	= m_edt_flt[6].GetTextToDouble();	/* mm */

	/* 모든 Photohead Z Axis을 노광 횟수 만큼 이동했을 경우, 이동 거리가 Range를 벗어나는지 여부 확인 */
	for (i=1; i<=uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
		{
			dbLastPos	= uvCmn_Luria_GetPhZAxisPosition(i) + (u16Repeat * dbPeriodZ);
		}
		else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
		{
			dbLastPos	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(u8Drv+i-1)) + (u16Repeat * dbPeriodZ);;
		}
		if (!uvCmn_Luria_IsValidPhMoveZRange(dbLastPos))
		{
			swprintf_s(tzMesg, LOG_MESG_SIZE,
					   L"The moving range value of PH [%d] has been exceeded", i);
			AfxMessageBox(tzMesg, MB_ICONSTOP|MB_TOPMOST);
			return;
		}
	}

	/* 노광 시작 위치 강제로 재설정 */
	uvEng_Luria_ReqSetTableExposureStartPos(0x01,
											(UINT32)ROUNDED(m_edt_flt[2].GetTextToDouble() * 1000.0f, 0),	/* unit: um */
											(UINT32)ROUNDED(m_edt_flt[3].GetTextToDouble() * 1000.0f, 0),	/* unit: um */
											TRUE);

	/* Spec 측정 시작 */
	m_pMainThread->StartPrint((INT32)ROUNDED(m_edt_flt[2].GetTextToDouble(), 0),
							  (INT32)ROUNDED(m_edt_flt[3].GetTextToDouble(), 0),
							  dbPeriodZ, dbPeriodY, u16Repeat);
	/* 진행바 영역 초기화 */
	m_pgs_ctl[0].SetRange(0, 100);
	m_pgs_ctl[0].SetStep(1);	/* 진행바 이동 크기 단위 */
	m_pgs_ctl[0].SetPos(0);		/* 진행바 초기 시작 값 */
}

/*
 desc : Print 중지
 parm : None
 retn : None
*/
VOID CDlgMain::StopPrint()
{
	/* Spec 측정 시작 */
	m_pMainThread->StopPrint();
}

/*
 desc : 기본적으로 환경 파일에 설정된 값을 적재
 parm : None
 retn : None
*/
VOID CDlgMain::LoadDataConfig()
{
	LPG_CIEA pstCfg		= uvEng_GetConfig();
	LPG_CPFI pstPhFocs	= &uvEng_GetConfig()->ph_focus;

	/* 현재 MC2에 설정된 이동 거리 및 속도 값 출력 */
	m_edt_flt[0].SetTextToNum(pstCfg->mc2_svc.move_dist, 4);
	m_edt_flt[1].SetTextToNum(pstCfg->mc2_svc.move_velo, 4);

	/* 초기 노광 시작 위치 (기본 값) */
	m_edt_flt[2].SetTextToNum(pstPhFocs->expo_stage[0], 3, TRUE);
	m_edt_flt[3].SetTextToNum(pstPhFocs->expo_stage[1], 3, TRUE);

	/* 매 노광 후 이동하는 Z 축과 Y 축 이동 거리 */
	m_edt_flt[5].SetTextToNum(-0.02f, 3, TRUE);	/* Z 축 이동 (Down) : 20 um */
	m_edt_flt[6].SetTextToNum(pstPhFocs->step_move_y, 4, TRUE);	/* Y 축 이동 (Up)   : 20 mm */

	/* 노광 반복 횟수 */
	m_edt_int[0].SetTextToNum(pstPhFocs->step_y_count, TRUE);

	/* 광학계 Z 축 이동 간격 */
	m_edt_flt[4].SetTextToNum(pstPhFocs->step_move_z, 3, TRUE);	/* Z 축 이동 (Up)   : 500 um */
}

/*
 desc : 현재 설정된 값을 환경 파일에 덮어쓰기
 parm : None
 retn : None
*/
VOID CDlgMain::SaveDataToFile()
{
	UINT8 i	= 0x00;
	LPG_CIEA pstCfg	= uvEng_GetConfig();

	/* 현재 MC2에 설정된 이동 거리 및 속도 값 저장 */
	pstCfg->mc2_svc.move_dist	= m_edt_flt[0].GetTextToDouble();
	pstCfg->mc2_svc.move_velo	= m_edt_flt[1].GetTextToDouble();

	/* 얼라인 노광 시작 위치 */
	for (i=0x00; i<pstCfg->luria_svc.table_count; i++)
	{
		pstCfg->ph_focus.expo_stage[0]	= m_edt_flt[2].GetTextToDouble();
		pstCfg->ph_focus.expo_stage[1]	= m_edt_flt[3].GetTextToDouble();
	}

	/* 환경 설정 값을 파일로 저장 */
	if (!uvEng_SaveConfig())	return;
#ifndef _DEBUG
	AfxMessageBox(L"Save completed successfully", MB_OK);
#endif
}
