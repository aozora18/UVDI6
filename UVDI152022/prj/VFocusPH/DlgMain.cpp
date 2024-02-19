
/*
 desc : Main Dialog
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgMain.h"

#include "MainThread.h"
#include "./Meas/Measure.h"

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
	m_bDrawBG			= 0x01;
	m_hIcon				= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_dbACamZAxisPos[0]	= -100.0f;
	m_dbACamZAxisPos[1]	= -100.0f;
	m_pMainThread		= NULL;
	m_bTriggerOn		= FALSE;
	m_u8ACamNo			= 0x01;
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
	for (i=0; i<8; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Normal - Text */
	u32StartID	= IDC_MAIN_TXT_MC2_MOVE;
	for (i=0; i<21; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Normal - Button */
	u32StartID	= IDC_MAIN_BTN_MC2_MOVE_UP;
	for (i=0; i<18; i++)	DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);

	/* Picture */
	u32StartID	= IDC_MAIN_PIC_VIEW;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_pic_ctl[i]);

	/* for MC2 */
	u32StartID	= IDC_MAIN_CHK_MC2_DRV_0;
	for (i=0; i<8; i++)		DDX_Control(dx, u32StartID+i,	m_chk_mc2[i]);
	u32StartID	= IDC_MAIN_EDT_MC2_DIST;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_edt_mc2[i]);

	/* for Align Camera */
	u32StartID	= IDC_MAIN_CHK_ACAM_LIVE;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_chk_cam[i]);
	u32StartID	= IDC_MAIN_EDT_ACAM_ABS;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_edt_cam[i]);

	/* for Move Size */
	u32StartID	= IDC_MAIN_EDT_MOVEUP_SIZE;
	for (i=0; i<5; i++)		DDX_Control(dx, u32StartID+i,	m_edt_opt[i]);

	/* for Trigger Time */
	u32StartID	= IDC_MAIN_EDT_TRIG_ONTIME;
	for (i=0; i<3; i++)		DDX_Control(dx, u32StartID+i,	m_edt_trg[i]);

	/* for Work status */
	u32StartID	= IDC_MAIN_EDT_STEP;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_edt_pgr[i]);

}

BEGIN_MESSAGE_MAP(CDlgMain, CMyDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MAIN_THREAD,	OnMsgThread)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_BTN_MC2_MOVE_UP,	IDC_MAIN_BTN_EXIT,		OnBtnClicked)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_CHK_ACAM_LIVE,	IDC_MAIN_CHK_ACAM_LIVE,	OnChkClickACam)
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

	/* 윈도 컨트롤 초기화 */
	InitCtrl();
	InitGridMotor();
	InitGridMeas();
	InitSetup();

	/* 기본 환경 설정 정보 적재 */
	LoadDataConfig();
	LoadTriggerCh();

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
	/* 기본 감시 스레드 메모리 해제 */
	if (m_pMainThread)
	{
		m_pMainThread->Stop();
		while (m_pMainThread->IsBusy())	m_pMainThread->Sleep();
		delete m_pMainThread;
		m_pMainThread	= NULL;
	}

	/* GridControl 해제 */
	CloseGrid();
	/* 라이브러리 해제 !!! 맨 마지막에 코딩 !!! */
	CloseLib();
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
 desc : 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgMain::InitCtrl()
{
	INT32 i;

	/* Normal - Group box */
	for (i=0; i<8; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* Normal - Text */
	for (i=0; i<21; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(TRUE);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* Normal - Button */
	for (i=0; i<18; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
	}
	/* for MC2 */
	for (i=0; i<8; i++)
	{
		m_chk_mc2[i].SetLogFont(g_lf);
		m_chk_mc2[i].SetBgColor(RGB(0xff, 0xff, 0xff));
		m_chk_mc2[i].SetReadOnly(FALSE);
		m_chk_mc2[i].EnableWindow(FALSE);
	}
	for (i=0; i<2; i++)
	{
		m_edt_mc2[i].SetEditFont(&g_lf);
		m_edt_mc2[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_mc2[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_mc2[i].SetInputType(ENM_DITM::en_float);
	}
	/* for Align Camera */
	for (i=0; i<1; i++)
	{
		m_chk_cam[i].SetLogFont(g_lf);
		m_chk_cam[i].SetBgColor(RGB(0xff, 0xff, 0xff));
		m_chk_cam[i].SetReadOnly(FALSE);
		m_chk_cam[i].EnableWindow(FALSE);
	}

	for (i=0; i<1; i++)
	{
		m_edt_cam[i].SetEditFont(&g_lf);
		m_edt_cam[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_cam[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_cam[i].SetInputType(ENM_DITM::en_float);
	}
	/* for Trigger Time */
	for (i=0; i<3; i++)
	{
		m_edt_trg[i].SetEditFont(&g_lf);
		m_edt_trg[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_trg[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_trg[i].SetInputType(ENM_DITM::en_hex10);
	}
	/* for Work Status */
	for (i=0; i<1; i++)
	{
		m_edt_pgr[i].SetEditFont(&g_lf);
		m_edt_pgr[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_pgr[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_pgr[i].SetInputType(ENM_DITM::en_float);
	}
	/* for Measurement Option */
	for (i=0; i<5; i++)
	{
		m_edt_opt[i].SetEditFont(&g_lf);
		m_edt_opt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_opt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		if (i < 0x03)	m_edt_opt[i].SetInputType(ENM_DITM::en_float);
		else			m_edt_opt[i].SetInputType(ENM_DITM::en_int8);
	}

	/* Align Camera Focus에서 측정된 값을 가져오므로 ... */
	m_edt_opt[1].SetReadOnly(TRUE);
	m_edt_opt[1].SetMouseClick(FALSE);
}

/*
 desc : 초기 컨트롤 설정
 parm : None
 retn : None
*/
VOID CDlgMain::InitSetup()
{
	UINT32 i	= 0;
	LPG_CMSI pstMC2	= &uvEng_GetConfig()->mc2_svc;
	LPG_CSCI pstCAM	= &uvEng_GetConfig()->set_cams;

	/* for MC2 - 체크 버튼 활성화 처리 */
	for (i=0; i<pstMC2->drive_count; i++)
	{
		m_chk_mc2[pstMC2->axis_id[i]].EnableWindow(TRUE);
	}

	/* for Align Camera - 에디트 버튼 활성화 처리 */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)uvEng_GetConfig()->set_comn.align_camera_kind)
	{
		/* Velo 설정 버튼도 Disable */
		m_edt_mc2[1].EnableWindow(FALSE);
	}
}

/*
 desc : 그리드 컨트롤 생성 (Motor Position)
 parm : None
 retn : None
*/
VOID CDlgMain::InitGridMotor()
{
	TCHAR tzCols[2][16]	= { L"Item", L"Pos (mm)" }, tzRows[32] = {NULL};
	INT32 i32Width[2]	= { 54, 75 }, i, j=1;
	INT32 i32ColCnt		= 2, i32RowCnt = 0;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	LPG_CIEA pstCfg		= uvEng_GetConfig();
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* Last Recv. Packet / PDO Header / PDO Data (Values) */
	::GetWindowRect(m_grp_ctl[3].GetSafeHwnd(), rGrid);
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
	/* Basler 카메라의 경우 Z Axis의 위치 (높이) 값 얻기 위함 */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)pstCfg->set_comn.align_camera_kind)
	{
		/* Align Camera의 Z Axis 높이 설정 위치 값 저장 */
		m_u32ACamZAxisRow[0]	= i32RowCnt+1;
		m_u32ACamZAxisRow[1]	= i32RowCnt+2;
		/* Motion Left / Right 위치와 Up / Down 높이 위치 */
		i32RowCnt	+= pstCfg->set_cams.acam_count;
	}

	/* 그리드 컨트롤 기본 공통 속성 */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* 객체 생성 */
	m_pGridMotor	= new CGridCtrl;
	ASSERT(m_pGridMotor);
	m_pGridMotor->SetDrawScrollBarHorz(FALSE);
	m_pGridMotor->SetDrawScrollBarVert(TRUE);
	if (!m_pGridMotor->Create(rGrid, this, IDC_MAIN_GRID_MOTOR_POS, dwStyle))
	{
		delete m_pGridMotor;
		m_pGridMotor = NULL;
		return;
	}
	/* 객체 기본 설정 */
	m_pGridMotor->SetLogFont(g_lf);
	m_pGridMotor->SetRowCount(i32RowCnt+1);
	m_pGridMotor->SetColumnCount(i32ColCnt);
	m_pGridMotor->SetFixedRowCount(1);
	m_pGridMotor->SetRowHeight(0, 24);
	m_pGridMotor->SetFixedColumnCount(0);
	m_pGridMotor->SetBkColor(RGB(255, 255, 255));
	m_pGridMotor->SetFixedColumnSelection(0);

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
		m_pGridMotor->SetItem(&stGV);
		m_pGridMotor->SetColumnWidth(i, i32Width[i]);
	}

	/* 본문 배경색 */
	stGV.crBkClr = RGB(255, 255, 255);
	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* 본문 설정 - for Photohead Position */
		for (i=0; i<pstCfg->luria_svc.ph_count; i++,j++)
		{
			m_pGridMotor->SetRowHeight(j, 24);

			/* for Subject */
			stGV.strText.Format(L"PH %d", i+1);
			stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
			stGV.row	= j;
			stGV.col	= 0;
			m_pGridMotor->SetItem(&stGV);

			/* for Subject */
			stGV.strText= L"0.0000";
			stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
			stGV.col	= 1;
			m_pGridMotor->SetItem(&stGV);
		}
	}
	/* 본문 설정 - for Motion Drive Position */
	for (i=0; i<pstCfg->mc2_svc.drive_count; i++,j++)
	{
		m_pGridMotor->SetRowHeight(j, 24);

		/* for Subject */
		stGV.strText.Format(L"MC %d", pstCfg->mc2_svc.axis_id[i]);
		stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
		stGV.row	= j;
		stGV.col	= 0;
		m_pGridMotor->SetItem(&stGV);

		/* for Subject */
		stGV.strText= L"0.0000";
		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.col	= 1;
		m_pGridMotor->SetItem(&stGV);
	}

	/* 본문 설정 - for Basler Camera Position */
	if (pstCfg->set_comn.align_camera_kind == (UINT8)ENG_VCPK::en_camera_basler_ipv4)
	{
		/* Z Axis Up / Down Postion */
		for (i=0; i<pstCfg->set_cams.acam_count; i++,j++)
		{
			m_pGridMotor->SetRowHeight(j, 24);

			/* for Subject */
			stGV.strText.Format(L"AZ %d", i+1);
			stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
			stGV.row	= j;
			stGV.col	= 0;
			m_pGridMotor->SetItem(&stGV);

			/* for Subject */
			stGV.strText= L"0.0000";
			stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
			stGV.col	= 1;
			m_pGridMotor->SetItem(&stGV);
		}
	}

	m_pGridMotor->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGridMotor->UpdateGrid();
}

/*
 desc : 그리드 컨트롤 생성 (Measurement Result)
 parm : None
 retn : None
*/
VOID CDlgMain::InitGridMeas()
{
	TCHAR tzCols[8][16]	= { L"PH", L"Step", L"Score(%)", L"Scale(%)", 
							L"Coverage(%)", L"Fit Error", L"Repeat", L"ACam.Z(mm)" }, tzRows[32] = {NULL};
	INT32 i32Width[8]	= { 50, 48, 70, 70, 85, 65, 50, 90 }, i, j=1;
	INT32 i32ColCnt		= 8, i32RowCnt = 0;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	LPG_CIEA pstCfg		= uvEng_GetConfig();
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* Last Recv. Packet / PDO Header / PDO Data (Values) */
	::GetWindowRect(m_grp_ctl[7].GetSafeHwnd(), rGrid);
	ScreenToClient(rGrid);
	rGrid.left	+= 15;
	rGrid.right	-= 15;
	rGrid.top	+= 22;
	rGrid.bottom-= 13;

	/* 그리드 컨트롤 기본 공통 속성 */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* 객체 생성 */
	m_pGridMeas	= new CGridCtrl;
	ASSERT(m_pGridMeas);
	m_pGridMeas->SetDrawScrollBarHorz(FALSE);
	m_pGridMeas->SetDrawScrollBarVert(TRUE);
	if (!m_pGridMeas->Create(rGrid, this, IDC_MAIN_GRID_MEAS_DATA, dwStyle))
	{
		delete m_pGridMeas;
		m_pGridMeas = NULL;
		return;
	}
	/* 객체 기본 설정 */
	m_pGridMeas->SetLogFont(g_lf);
	m_pGridMeas->SetRowCount(i32RowCnt+1);
	m_pGridMeas->SetColumnCount(i32ColCnt);
	m_pGridMeas->SetFixedRowCount(1);
	m_pGridMeas->SetRowHeight(0, 25);
	m_pGridMeas->SetFixedColumnCount(0);
	m_pGridMeas->SetBkColor(RGB(255, 255, 255));
	m_pGridMeas->SetFixedColumnSelection(0);

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
		m_pGridMeas->SetItem(&stGV);
		m_pGridMeas->SetColumnWidth(i, i32Width[i]);
	}

	m_pGridMeas->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGridMeas->UpdateGrid();
}

/*
 desc : 그리드 컨트롤 해제
 parm : None
 retn : None
*/
VOID CDlgMain::CloseGrid()
{
	/* for Motor Position */
	if (m_pGridMotor)
	{
		if (m_pGridMotor->GetSafeHwnd())	m_pGridMotor->DestroyWindow();
		delete m_pGridMotor;
	}
	/* for Motor Position */
	if (m_pGridMeas)
	{
		if (m_pGridMeas->GetSafeHwnd())		m_pGridMeas->DestroyWindow();
		delete m_pGridMeas;
	}
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
 desc : 버튼 클릭 이벤트
 parm : id	- [in]  Click된 버튼 ID
 retn : None
*/
VOID CDlgMain::OnBtnClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_MAIN_BTN_EXIT			:	PostMessage(WM_CLOSE, 0, 0L);		break;
	case IDC_MAIN_BTN_MC2_RESET		:	MC2Recovery();						break;
	case IDC_MAIN_BTN_NEXT			:	NextStepMoveY();					break;
	case IDC_MAIN_BTN_ACAM_MOVE		:	ACamMoveAbs();						break;
	case IDC_MAIN_BTN_ACAM_HOMED	:	ACamHoming();						break;
	case IDC_MAIN_BTN_START			:	StartFocus();						break;
	case IDC_MAIN_BTN_STOP			:	StopFocus();						break;
	case IDC_MAIN_BTN_UNLOAD		:	UnloadStageXY();					break;
	case IDC_MAIN_BTN_MOVE			:	MovePosMeasure();					break;
	case IDC_MAIN_BTN_SAVE			:	SaveDataToFile();					break;
	case IDC_MAIN_BTN_MC2_MOVE_UP	:	MotionMove(ENG_MDMD::en_move_up);	break;
	case IDC_MAIN_BTN_MC2_MOVE_DN	:	MotionMove(ENG_MDMD::en_move_down);	break;
	case IDC_MAIN_BTN_MODEL_FIND	:	RunFindModel();						break;
	case IDC_MAIN_BTN_MODEL_RESET	:	ResetFindModel();					break;
	case IDC_MAIN_BTN_TRIG_SET		:	SetTriggerParam();					break;
	case IDC_MAIN_BTN_TRIG_CH		:	SetTriggerCh();						break;
	case IDC_MAIN_BTN_FILE_XLS		:	SaveAsExcelFile();					break;
	case IDC_MAIN_BTN_ACAM			:	SetACamNo();						break;
	}
}

/*
 desc : 체크 (for Align Camera) 클릭 이벤트
 parm : id	- [in]  Click ID
 retn : None
*/
VOID CDlgMain::OnChkClickACam(UINT32 id)
{
	switch (id)
	{
	case IDC_MAIN_CHK_ACAM_LIVE	:	SetLiveView();	break;
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
	case MSG_ID_NORMAL			:	UpdatePeriod();			break;
	case MSG_ID_STEP_RESULT		:	UpdateStepResult();
									UpdateMatchDraw();		break;
	case MSG_ID_STEP_GRAB_FAIL	:	DisplayGrabError(0x01);	break;
	case MSG_ID_STEP_FIND_FAIL	:	DisplayGrabError(0x02);	break;
	}

	/* 트리거가 발생되고 2초 정도 지났다면, 모델 검색 결과 호출 */
	if (m_csSyncTrig.Enter())
	{
		if (m_bTriggerOn)
		{
			if (m_u64TrigTime + 2000 < GetTickCount64())
			{
				m_bTriggerOn	= FALSE;
				UpdateGrabResult();
			}
		}
		/* 동기 해제 */
		m_csSyncTrig.Leave();
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
	/* for Align Camera */
	UpdateCtrlACam();
	/* for Image */
	UpdateResult();
	/* for Motor Position (mm) */
	UpdateMotorPos();
	/* for PLC Value */
	UpdatePLCVal();
	/* Control : Enable/Disable */
	UpdateEnableCtrl();
}

/*
 desc : Focus 측정 결과 갱신
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateStepResult()
{
	TCHAR tzValue[128]	= {NULL};
	UINT8 u8PhNo		= 0x00;	/* Zero-based */
	INT32 i32Item		= 1;
	UINT32 u32Count		= 0, u32Format	= DT_VCENTER | DT_SINGLELINE;
	LPG_ZAAL pstData	= NULL;
	CMeasure *pMeasure	= m_pMainThread->GetMeasure();

	/* 현재 등록된 개수 정보 얻기 */
	pstData	= m_pMainThread->GetLastGrabData();
	u8PhNo	= pstData->ph_no - 1;
	/* 등록될 위치 계산 */
	i32Item	= u8PhNo * m_u16StepCount + pstData->steps;

	/* Photohead Number */
	wmemset(tzValue, 0x00, 128);
	swprintf_s(tzValue, 128, L"PH_%u", pstData->ph_no);
	m_pGridMeas->SetItemText(i32Item,	0, tzValue);
	m_pGridMeas->SetItemFormat(i32Item,	0, u32Format|DT_CENTER);
	if (0 == (pstData->ph_no % 2))
	{
		m_pGridMeas->SetItemBkColour(i32Item, 0, RGB(255, 225, 240));
	}
	m_pGridMeas->InvalidateRowCol(i32Item, 0);
	/* Measurement Step Number */
	wmemset(tzValue, 0x00, 128);
	swprintf_s(tzValue, 128, L"%03u", pstData->steps);
	m_pGridMeas->SetItemText(i32Item,	1, tzValue);
	m_pGridMeas->SetItemFormat(i32Item,	1, u32Format|DT_CENTER);
	m_pGridMeas->InvalidateRowCol(i32Item, 1);
	/* Score Rate (100 % 에 가까울 수록 좋음) */
	wmemset(tzValue, 0x00, 128);
	swprintf_s(tzValue, 128, L"%.3f", pstData->avg_value[0]);
	m_pGridMeas->SetItemText(i32Item,	2, tzValue);
	m_pGridMeas->SetItemFormat(i32Item,	2, u32Format|DT_RIGHT);
	if (pstData->set_value[0])
	{
		/* 기존 값 배경 색상 취소 */
		if (m_i32SetValue[u8PhNo][0] != -1)
		{
			m_pGridMeas->SetItemBkColour(m_i32SetValue[u8PhNo][0], 1, RGB(255, 255, 255));
			m_pGridMeas->SetItemBkColour(m_i32SetValue[u8PhNo][0], 2, RGB(255, 255, 255));
			m_pGridMeas->InvalidateRowCol(m_i32SetValue[u8PhNo][0], 1);
			m_pGridMeas->InvalidateRowCol(m_i32SetValue[u8PhNo][0], 2);
		}
		m_pGridMeas->SetItemBkColour(i32Item, 1, RGB(224, 224, 244));
		m_pGridMeas->SetItemBkColour(i32Item, 2, RGB(224, 224, 244));
		m_i32SetValue[u8PhNo][0]	= i32Item;
	}
	/* Scale Rate (100 % 에 가까울 수록 좋음) */
	wmemset(tzValue, 0x00, 128);
	swprintf_s(tzValue, 128, L"%.3f", pstData->avg_value[1]);
	m_pGridMeas->SetItemText(i32Item,	3, tzValue);
	m_pGridMeas->SetItemFormat(i32Item,	3, u32Format|DT_RIGHT);
	if (pstData->set_value[1])
	{
		/* 기존 값 배경 색상 취소 */
		if (m_i32SetValue[u8PhNo][1] != -1)
		{
			m_pGridMeas->SetItemBkColour(m_i32SetValue[u8PhNo][1], 3, RGB(255, 255, 255));
			m_pGridMeas->InvalidateRowCol(m_i32SetValue[u8PhNo][1], 3);
		}
		m_i32SetValue[u8PhNo][1]	= i32Item;
		/* 새로운 배경 색상 설정 */
		m_pGridMeas->SetItemBkColour(i32Item, 3, RGB(224, 224, 244));
	}
	/* Coverage Rate (100 % 에 가까울 수록 좋음) */
	wmemset(tzValue, 0x00, 128);
	swprintf_s(tzValue, 128, L"%.3f", pstData->avg_value[2]);
	m_pGridMeas->SetItemText(i32Item,	4, tzValue);
	m_pGridMeas->SetItemFormat(i32Item,	4, u32Format|DT_RIGHT);
	if (pstData->set_value[2])
	{
		/* 기존 값 배경 색상 취소 */
		if (m_i32SetValue[u8PhNo][2] != -1)
		{
			m_pGridMeas->SetItemBkColour(m_i32SetValue[u8PhNo][2], 4, RGB(255, 255, 255));
			m_pGridMeas->InvalidateRowCol(m_i32SetValue[u8PhNo][2], 4);
		}
		m_i32SetValue[u8PhNo][2]	= i32Item;
		/* 새로운 배경 색상 설정 */
		m_pGridMeas->SetItemBkColour(i32Item, 4, RGB(224, 224, 244));
	}
	/* Fit Error Rate (0.00 에 가까울 수록 좋음) */
	wmemset(tzValue, 0x00, 128);
	swprintf_s(tzValue, 128, L"%.5f", pstData->avg_value[3]);
	m_pGridMeas->SetItemText(i32Item,	5, tzValue);
	m_pGridMeas->SetItemFormat(i32Item,	5, u32Format|DT_RIGHT);
	if (pstData->set_value[3])
	{
		/* 기존 값 배경 색상 취소 */
		if (m_i32SetValue[u8PhNo][3] != -1)
		{
			m_pGridMeas->SetItemBkColour(m_i32SetValue[u8PhNo][3], 5, RGB(255, 255, 255));
			m_pGridMeas->InvalidateRowCol(m_i32SetValue[u8PhNo][3], 5);
		}
		m_i32SetValue[u8PhNo][3]	= i32Item;
		/* 새로운 배경 색상 설정 */
		m_pGridMeas->SetItemBkColour(i32Item, 5, RGB(224, 224, 244));
	}
	/* Repeat Count (최대 255) */
	m_pGridMeas->SetItemText(i32Item,	6, (UINT32)pstData->arrValue[0].GetCount());
	m_pGridMeas->SetItemFormat(i32Item,	6, u32Format|DT_CENTER);
	/* Align Camera Z Position (mm) */
#if 0
	m_pGridMeas->SetItemText(i32Item, 7, uvCmn_MCQ_GetACamCurrentPosZ(0x01));
	m_pGridMeas->SetItemFormat(i32Item,	7, u32Format|DT_RIGHT);
	m_pGridMeas->SetItemText(i32Item, 8, uvCmn_MCQ_GetACamCurrentPosZ(0x02));
	m_pGridMeas->SetItemFormat(i32Item,	8, u32Format|DT_RIGHT);
#else
	m_pGridMeas->SetItemText(i32Item, 7, uvCmn_MCQ_GetACamCurrentPosZ(m_u8ACamNo));
	m_pGridMeas->SetItemFormat(i32Item,	7, u32Format|DT_RIGHT);
#endif
	/* 현재 행 갱신 */
	m_pGridMeas->InvalidateRow(i32Item);
}

/*
 desc : Match된 측정 결과 이미지 출력
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateMatchDraw()
{
	InvalidateView();
}

/*
 desc : Grabbed Image 검색 실패 에러 메시지
 parm : flag	- [in]  0x01 : 모델 검색 실패, 0x02 : 모델 분석 실패
 retn : None
*/
VOID CDlgMain::DisplayGrabError(UINT8 flag)
{
	if (0x01 == flag)	AfxMessageBox(L"Failed to find the mark model", MB_ICONSTOP|MB_TOPMOST);
	else 				AfxMessageBox(L"Failed to analyze the mark model", MB_ICONSTOP|MB_TOPMOST);
}

/*
 desc : Align Camera 관련 Control 갱신
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateCtrlACam()
{
	INT32 i32Tab	= 0;
	BOOL bEnable	=FALSE;

	/* 얼라인 카메라가 연결되어 있지 않으면 */
	if (!uvCmn_Camera_IsConnected())	bEnable	= FALSE;
	/* 얼라인 카메라가 연결되어 있다면 */
	else
	{
		/* 현재 Live View 화면이 아니면 */
		if (i32Tab != 0)	bEnable	= FALSE;
		/* 현재 Live View 화면이면, 현재 측정 모드인지 여부에 따라 */
		else				bEnable	= !m_pMainThread->IsRunFocus();
	}

	/* Live Check 버튼 활성화 및 체크 해제 처리 */
	m_chk_cam[0].EnableWindow(bEnable);
	if (!bEnable)
	{
		if (m_chk_cam[0].GetCheck())	m_chk_cam[0].SetCheck(0);
	}
}

/*
 desc : Result (Live View / Edge Histogram / Edge Object) 갱신
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateResult()
{
	TCHAR tzVal[64]	= {NULL};
	INT32 i32Tab	= 0;

	/* 현재 동작 중인지 여부 */
	if (!m_pMainThread->IsRunFocus())
	{
		if (m_chk_cam[0].GetCheck())	UpdateLiveView();
	}
	else
	{
		/* 현재 Grabbed Image 출력 */
		InvalidateView();
		/* 현재까지 진행률 출력 */
		swprintf_s(tzVal, 64, L"%.2f %%", m_pMainThread->GetWorkStepRate());
		m_edt_pgr[0].SetTextToStr(tzVal, TRUE);
	}
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
			dbPos[0] = m_pGridMotor->GetItemTextToFloat(j, 1);
			dbPos[1] = (DOUBLE)ROUNDED(pstDP->focus_motor_move_abs_position[i]/1000.0f, 3);
			if (dbPos[0] != dbPos[1])
			{
				m_pGridMotor->SetItemText(j, 1, dbPos[1]);
				m_pGridMotor->InvalidateRowCol(j, 1);
			}
		}
	}
	/* MC2 SD2S Drive 위치 (Basler Camera 포함) */
	for (i=0; i<pstCfg->mc2_svc.drive_count; i++, j++)
	{
		dbPos[0] = m_pGridMotor->GetItemTextToFloat(j, 1);
		dbPos[1] = (DOUBLE)ROUNDED(pstMC2->act_data[pstCfg->mc2_svc.axis_id[i]].real_position/10000.0f, 4);
		if (dbPos[0] != dbPos[1])
		{
			m_pGridMotor->SetItemText(j, 1, dbPos[1]);
			m_pGridMotor->InvalidateRowCol(j, 1);
		}
	}
}

/*
 desc : 각종 장비 내의 PLC 값 갱신
 parm : None
 retn : None
*/
VOID CDlgMain::UpdatePLCVal()
{
	DOUBLE dbPosZ	= 0.0f;
	LPG_CIEA pstCfg	= uvEng_GetConfig();

	/* Align Camera (for Basler) - Z Axis Move Step & Speed */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)pstCfg->set_comn.align_camera_kind)
	{
		/* 현재 Align Camera의 Z Axis 이동 속도 및 현재 위치 (Position) 값 얻기 */
		dbPosZ	= uvCmn_MCQ_GetACamCurrentPosZ(0x01);
		if (m_dbACamZAxisPos[0] != dbPosZ)
		{
			TCHAR tzVal[32]	= {NULL};
			swprintf_s(tzVal, 32, L"%.4f", dbPosZ);
			m_pGridMotor->SetItemText(m_u32ACamZAxisRow[0], 1, tzVal);	/* Position */
			m_pGridMotor->InvalidateRowCol(m_u32ACamZAxisRow[0], 1);
			m_dbACamZAxisPos[0] = dbPosZ;
		}
		dbPosZ	= uvCmn_MCQ_GetACamCurrentPosZ(0x02);
		if (m_dbACamZAxisPos[1] != dbPosZ)
		{
			TCHAR tzVal[32]	= {NULL};
			swprintf_s(tzVal, 32, L"%.4f", dbPosZ);
			m_pGridMotor->SetItemText(m_u32ACamZAxisRow[1], 1, tzVal);	/* Position */
			m_pGridMotor->InvalidateRowCol(m_u32ACamZAxisRow[1], 1);
			m_dbACamZAxisPos[1] = dbPosZ;
		}
	}
	/* Align Camera (for IDS) - Only Z Axis Step */
	else
	{
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
	BOOL bBusyMotion	= FALSE, bBusyRunFocus = FALSE, bValidFocusPos = FALSE;
	LPG_CMSI pstMC2Svc	= &uvEng_GetConfig()->mc2_svc;

	/* Focus 측정 진행 여부 */
	bBusyRunFocus	= m_pMainThread->IsRunFocus();
	/* Motion Busy 여부 */
	bBusyMotion		= uvCmn_MC2_IsAnyDriveBusy();
	/* MC2 Drive Check Button */
	for (; i<uvEng_GetConfig()->mc2_svc.drive_count; i++)
	{
		m_chk_mc2[pstMC2Svc->axis_id[i]].EnableWindow(!(bBusyMotion || bBusyRunFocus));
	}
	/* Align Camera Check Button */
	m_chk_cam[0].EnableWindow(!(bBusyMotion || bBusyRunFocus));

	/* 현재 얼라인 카메라의 Z 축 위치와 환경 파일에 저장된 Focus Z 위치와 동일한지 여부 */
	bValidFocusPos	= uvCmn_MCQ_GetACamCurrentPosZ(m_u8ACamNo) != uvEng_GetConfig()->acam_focus.acam_z_focus[m_u8ACamNo-1];

	/* Normal Button */
	m_btn_ctl[0].EnableWindow(!bBusyRunFocus);										/* MC2 Move Up */
	m_btn_ctl[1].EnableWindow(!bBusyRunFocus);										/* MC2 Move Down */
	m_btn_ctl[2].EnableWindow(!bBusyRunFocus);										/* ACam Move */
	m_btn_ctl[3].EnableWindow(!bBusyRunFocus);										/* Reset */
	m_btn_ctl[4].EnableWindow(!(bBusyMotion || bBusyRunFocus));						/* Move */
#ifndef _DEBUG
	m_btn_ctl[5].EnableWindow(!(bBusyMotion || bBusyRunFocus || bValidFocusPos));	/* Start */
	m_btn_ctl[6].EnableWindow(bBusyRunFocus);										/* Stop */
#endif
	m_btn_ctl[7].EnableWindow(bBusyRunFocus);										/* Next */
	m_btn_ctl[8].EnableWindow(!(bBusyMotion || bBusyRunFocus));						/* Save */
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
		if (!m_chk_mc2[i].GetCheck())	continue;
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
	/* Align Camera Z Axis 이동 */
	uvEng_MCQ_SetACamMovePosZ(m_u8ACamNo, 0x00, m_edt_cam[0].GetTextToDouble());
}

/*
 desc : Align Camera (Motor Drive) Homing
 parm : None
 retn : None
*/
VOID CDlgMain::ACamHoming()
{
	/* 새로운 위치로 이동 */
	uvEng_MCQ_SetACamHomingZAxis(m_u8ACamNo);
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
		bCheck	= m_chk_mc2[pstCfg->axis_id[i]].GetCheck() > 0;
	}
	if (!bCheck)
	{
		AfxMessageBox(L"The drive to be moved is not checked", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 이동하고자 하는 속도와 거리 값이 입력되어 있는지 확인 (여기서는 Step 이동만 해당 됨) */
	dbDist	= m_edt_mc2[0].GetTextToDouble();
	dbVelo	= m_edt_mc2[1].GetTextToDouble();
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
		if (!m_chk_mc2[u8DrvNo].GetCheck())	continue;

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
 desc : 측정 위치로 이동
 parm : None
 retn : None
*/
VOID CDlgMain::MovePosMeasure()
{
	UINT8 i			= 0x00;
	DOUBLE dbPos	= 0.0f, dbVelo = m_edt_mc2[1].GetTextToDouble();	/* mm/sec */
	LPG_CIEA pstCfg	= (LPG_CIEA)uvEng_GetConfig();
	if (!pstCfg)	return;

	/* 이동 속도가 정해져 있지 않거나 너무 느리다면 */
	if (dbVelo < 10.0f)	return;	/* 10 mm/sec 이동 속도 이하인 경우 */

	/* 모션 X 축 이동 */
	dbPos	= pstCfg->ph_focus.mark2_stage[0];
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, dbPos, dbVelo);

	/* 모션 Y 축 이동 */
	dbPos	= pstCfg->ph_focus.mark2_stage[1];
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dbPos, dbVelo);

	/* 모션 Camera 축 이동 */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)pstCfg->set_comn.align_camera_kind)
	{
		if (0x01 == m_u8ACamNo)
		{
			/* 2 번째 카메라를 맨 오른쪽으로 이동 시킴 */
			dbPos	= pstCfg->mc2_svc.max_dist[(UINT8)ENG_MMDI::en_align_cam2];
			uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2, dbPos, dbVelo);
			/* 1 번째 카메라를 측정 위치로 이동 시킴 */
			dbPos	= pstCfg->ph_focus.mark2_acam_x[0];
			uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, dbPos, dbVelo);
		}
		else
		{
			/* 1 번째 카메라를 맨 왼쪽으로 이동 시킴 */
			dbPos	= pstCfg->mc2_svc.min_dist[(UINT8)ENG_MMDI::en_align_cam1];
			uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, dbPos, dbVelo);
			/* 2 번째 카메라를 측정 위치로 이동 시킴 */
			dbPos	= pstCfg->ph_focus.mark2_acam_x[1];
			uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2, dbPos, dbVelo);
		}
		/* 얼라인 카메라 Z 축 이동 */
		uvEng_MCQ_SetACamMovePosZ(1/* fixed */, 0x00, pstCfg->acam_focus.acam_z_focus[0]);
	}
#if 0
	else
	{
		/* 광학계 Z 축 이동 */
		uvEng_Luria_ReqSetMotorAbsPosition(pstCfg->set_ids.ph_no,
										   pstCfg->acam_focus.acam_z_focus[0]);
	}
#endif
}

/*
 desc : 포커스 측정 중지
 parm : None
 retn : None
*/
VOID CDlgMain::StopFocus()
{
	if (m_pMainThread->IsRunFocus())	m_pMainThread->StopFocus();
}

/*
 desc : 스테이지 Unload 위치로 이동
 parm : None
 retn : None
*/
VOID CDlgMain::UnloadStageXY()
{
	DOUBLE dbMoveX	= uvEng_GetConfig()->set_align.table_unloader_xy[0][0];
	DOUBLE dbMoveY	= uvEng_GetConfig()->set_align.table_unloader_xy[0][1];
	DOUBLE dbVelo	= uvEng_GetConfig()->mc2_svc.move_velo;

	if (m_pMainThread->IsRunFocus())
	{
		AfxMessageBox(L"It is currently being measured\nPlease stop measuring first", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 모션 (스테이지) 이동 */
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, dbMoveX, dbVelo);
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dbMoveY, dbVelo);
}

/*
 desc : 다음 측정 위치로 이동 (Y 축 방향으로만 이동)
 parm : None
 retn : None
*/
VOID CDlgMain::NextStepMoveY()
{
}

/*
 desc : 트리거 채널 번호 적용
 parm : None
 retn : None
*/
VOID CDlgMain::LoadTriggerCh()
{
	UINT8 u8ChNo		= 0x01;
	TCHAR tzChNo[32]	= {NULL};
	LPG_CTSP pstTrig	= &uvEng_GetConfig()->trig_step;

	u8ChNo	= uvEng_GetConfig()->set_comn.strobe_lamp_type == 0x00 ? 0x01 : 0x02;	/* 0x00: Coaxial : 0x01 : Ring */

	swprintf_s(tzChNo, 32, L"CH\n[ %u ]", u8ChNo);
	m_btn_ctl[15].SetWindowTextW(tzChNo);

	/* Trigger Time 값 재-적재 */
	m_edt_trg[0].SetTextToNum(pstTrig->trig_on_time[u8ChNo-1]);
	m_edt_trg[1].SetTextToNum(pstTrig->strob_on_time[u8ChNo-1]);
	m_edt_trg[2].SetTextToNum(pstTrig->trig_delay_time[u8ChNo-1]);

	/* Trigger 측정 속성 값 설정 (Trigger OnTime / Strobe OnTime / Trigger Delay Time */
	if (!uvEng_Trig_ReqInitUpdate())
	{
		AfxMessageBox(L"Failed to set the trigger attribute for LED Lamp", MB_ICONSTOP|MB_TOPMOST);
	}
}

/*
 desc : 트리거 채널 번호 변경
 parm : None
 retn : None
*/
VOID CDlgMain::SetTriggerCh()
{
	UINT8 u8ChNo	= uvEng_GetConfig()->set_comn.strobe_lamp_type;
	if (!u8ChNo)	uvEng_GetConfig()->set_comn.strobe_lamp_type = 0x01;	/* Ring */
	else			uvEng_GetConfig()->set_comn.strobe_lamp_type = 0x00;	/* Coaxial */

	LoadTriggerCh();
}

/*
 desc : Align Camera 번호 변경
 parm : None
 retn : None
*/
VOID CDlgMain::SetACamNo()
{
	TCHAR tzNo[32]	= {NULL};

	m_u8ACamNo	= m_u8ACamNo == 0x01 ? 0x02 : 0x01;
	swprintf_s(tzNo, 32, L"ACam\n[ %u ]", m_u8ACamNo);
	m_btn_ctl[16].SetWindowTextW(tzNo);
}

/*
 desc : 모델 검색 수행
 parm : None
 retn : None
*/
VOID CDlgMain::SetTriggerParam()
{
	LPG_CTSP pstTrig	= &uvEng_GetConfig()->trig_step;
	/* 값 메모리에 저장 */
	pstTrig->SetData(m_u8ACamNo,
					 (UINT32)m_edt_trg[0].GetTextToNum(),
					 (UINT32)m_edt_trg[1].GetTextToNum(),
					 (UINT32)m_edt_trg[2].GetTextToNum());
	/* Trigger 쪽에 명령 전달 */
	if (!uvEng_Trig_ReqInitUpdate())
	{
		AfxMessageBox(L"Failed to send the command to trigger", MB_ICONSTOP|MB_TOPMOST);
	}
}

/*
 desc : 검색 대상의 모델 관련 파라미터 등록
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::SetModelParam()
{
	UINT32 u32Model	= (UINT32)ENG_MMDT::en_circle;
	DOUBLE dbMSize	= 0.0f /* um */, dbMColor = 256 /* 256:Black, 128:White */;
	LPG_CIEA pstCfg	= uvEng_GetConfig();

	/* 검색 대상에 대한 모델 등록 */
	dbMSize	= pstCfg->ph_focus.model_dia_size * 1000.0f;
	dbMColor= pstCfg->ph_focus.model_color;
	if (!uvEng_Camera_SetModelDefineEx(m_u8ACamNo,
									   pstCfg->mark_find.detail_level,
									   pstCfg->mark_find.model_speed,
									   pstCfg->mark_find.model_smooth,
									   &u32Model, &dbMColor, &dbMSize,
									   NULL, NULL, NULL, 1))
	{
		AfxMessageBox(L"Failed to setup the model for search target", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	return TRUE;
}


/*
 desc : 모델 검색 수행
 parm : None
 retn : None
*/
VOID CDlgMain::RunFindModel()
{
	UINT8 u8Ch	= m_u8ACamNo==0x01 ? 0x01 : 0x02;
	/* Camera 동작 모드 설정 */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);
	/* 검색 대상 모델 등록 */
	if (!SetModelParam())	return;

	/* 기존 검색된 Grabbed Data & Image 제거 */
	uvEng_Camera_ResetGrabbedImage();

	/* Trigger 강제로 (수동으로) 발생 */
	if (!uvEng_Trig_ReqTrigOutOne(u8Ch, TRUE))
	{
		AfxMessageBox(L"Failed to send the event for trigger", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* 트리거 발생했다고 플래그 설정 */
	if (m_csSyncTrig.Enter())
	{
		m_u64TrigTime	= GetTickCount64();
		m_bTriggerOn	= TRUE;

		m_csSyncTrig.Leave();
	}
}

/*
 desc : 모델 검색 수행 결과 출력
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateGrabResult()
{
	TCHAR tzValue[128]	= {NULL};
	INT32 i32Item		= 0;
	UINT32 u32Count		= 0, u32Format	= DT_VCENTER | DT_SINGLELINE;;
	LPG_ACGR pstResult	= NULL;

	/* 검색된 결과가 있는지 여부 */
	pstResult	= uvEng_Camera_RunModelCali(m_u8ACamNo, 0xff);
	if (!pstResult)
	{
		AfxMessageBox(L"There is no analysis result", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 검색된 이미지 분석 결과 확인 */
	if (pstResult->marked != 0x01)
	{
		AfxMessageBox(L"Failed to analyze the grabbed image", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* Camera 동작 모드 설정 */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_none);

	/* 그리드 컨트롤 비활성화 */
	m_pGridMeas->SetRedraw(FALSE);
	/* 현재 등록된 개수 얻기 */
	i32Item	= m_pGridMeas->GetRowCount();
	m_pGridMeas->InsertRow(i32Item);
	/* Photohead Number */
	m_pGridMeas->SetItemText(i32Item, 0, L"PH_XX");
	m_pGridMeas->SetItemFormat(i32Item, 0, u32Format|DT_CENTER);
	/* Measurement Step Number */
	m_pGridMeas->SetItemText(i32Item, 1, L"XXX");
	m_pGridMeas->SetItemFormat(i32Item, 1, u32Format|DT_CENTER);
	/* Score Rate (100 % 에 가까울 수록 좋음) */
	wmemset(tzValue, 0x00, 128);
	swprintf_s(tzValue, 128, L"%.4f", pstResult->score_rate);
	m_pGridMeas->SetItemText(i32Item, 2, tzValue);
	m_pGridMeas->SetItemFormat(i32Item, 2, u32Format|DT_RIGHT);
	m_pGridMeas->SetItemBkColour(i32Item, 2, RGB(224, 224, 244));
	/* Scale Rate (100 % 에 가까울 수록 좋음) */
	wmemset(tzValue, 0x00, 128);
	swprintf_s(tzValue, 128, L"%.4f", pstResult->scale_rate);
	m_pGridMeas->SetItemText(i32Item, 3, tzValue);
	m_pGridMeas->SetItemFormat(i32Item, 3, u32Format|DT_RIGHT);
	m_pGridMeas->SetItemBkColour(i32Item, 3, RGB(224, 224, 244));
	/* Coverage Rate (100 % 에 가까울 수록 좋음) */
	wmemset(tzValue, 0x00, 128);
	swprintf_s(tzValue, 128, L"%.4f", pstResult->coverage_rate);
	m_pGridMeas->SetItemText(i32Item, 4, tzValue);
	m_pGridMeas->SetItemFormat(i32Item, 4, u32Format|DT_RIGHT);
	m_pGridMeas->SetItemBkColour(i32Item, 4, RGB(224, 224, 244));
	/* Fit Error Rate (0.000 % 에 가까울 수록 좋음) */
	wmemset(tzValue, 0x00, 128);
	swprintf_s(tzValue, 128, L"%.5f", pstResult->fit_error);
	m_pGridMeas->SetItemText(i32Item, 5, tzValue);
	m_pGridMeas->SetItemFormat(i32Item, 5, u32Format|DT_RIGHT);
	m_pGridMeas->SetItemBkColour(i32Item, 5, RGB(224, 224, 244));
	/* Repeat Count (최대 255) */
	m_pGridMeas->SetItemText(i32Item, 6, L"XXX");
	m_pGridMeas->SetItemFormat(i32Item, 6, u32Format|DT_CENTER);
	/* Align Camera Z Position (mm) */
#if 0
	m_pGridMeas->SetItemText(i32Item, 7, uvCmn_MCQ_GetACamCurrentPosZ(0x01));
	m_pGridMeas->SetItemFormat(i32Item, 7, u32Format|DT_RIGHT);
	m_pGridMeas->SetItemText(i32Item, 8, uvCmn_MCQ_GetACamCurrentPosZ(0x02));
	m_pGridMeas->SetItemFormat(i32Item, 8, u32Format|DT_RIGHT);
#else
	m_pGridMeas->SetItemText(i32Item, 7, uvCmn_MCQ_GetACamCurrentPosZ(m_u8ACamNo));
	m_pGridMeas->SetItemFormat(i32Item, 7, u32Format|DT_RIGHT);
#endif
	/* 그리드 컨트롤 활성화 */
	m_pGridMeas->SetRedraw(TRUE);
	m_pGridMeas->Invalidate(FALSE);

	/* 현재 Grabbed Image 결과 출력 */
	UpdateMatchDraw();
}

/*
 desc : 모델 검색 초기화
 parm : None
 retn : None
*/
VOID CDlgMain::ResetFindModel()
{
	INT32 i, j, i32Rows	= (INT32)m_edt_opt[4].GetTextToNum() * uvEng_GetConfig()->luria_svc.ph_count + 1;
	/* Grid Control 모두 제거 */
	m_pGridMeas->DeleteNonFixedRows();
#if 0
	/* 그리드 행의 개수 설정 */
	m_pGridMeas->SetRowCount(i32Rows);
#endif
	/* 기존 검색된 Edge File 제거 */
	uvEng_RemoveAllFiles(0x00);
	/* 기존 검색된 Grabbed Data & Image 제거 */
	uvEng_Camera_ResetGrabbedImage();
	uvEng_Camera_SetCamMode(ENG_VCCM::en_none);

	/* 최적의 결과 값이 저장된 그리드 행과 열의 위치 초기화 */
	for (i=0; i<MAX_PH; i++)
	{
		for (j=0; j<COLLECT_DATA_COUNT; j++)	m_i32SetValue[i][j]	= -1;
	}
}

/*
 desc : 포커스 측정 시작
 parm : None
 retn : None
*/
VOID CDlgMain::StartFocus()
{
	BOOL bValid		= TRUE;
	UINT32 u32Model	= (UINT32)ENG_MMDT::en_circle;
	LPG_CIEA pstCfg	= uvEng_GetConfig();

	/* 기존 포커스 측정 결과 파일 및 변수 초기화 */
	ResetFindModel();

	/* --------------------------------------- */
	/* 위에 입력된 파라미터 값이 유효한지 판단 */
	/* --------------------------------------- */

	/* Trigger 측정 속성 값 변경 (Trigger OnTime / Strobe OnTime / Trigger Delay Time */
	UpdateTrigParam();
	if (!uvEng_Trig_ReqInitUpdate())
	{
		AfxMessageBox(L"Failed to set the trigger attribute for LED Lamp", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* 전체 측정 구간 값 임시 저장 */
	m_u16StepCount	= (UINT16)m_edt_opt[4].GetTextToNum();
	/* 정상적으로 값이 입력 되어 있는지 확인 */
	if (m_edt_opt[3].GetTextToNum() < 1)
	{
		AfxMessageBox(L"Enter the measurement repeat value", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	if (m_u16StepCount < 1)
	{
		AfxMessageBox(L"Enter the measurement step value", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	if (m_edt_opt[0].GetTextToDouble() < 1.0f)
	{
		AfxMessageBox(L"Enter the value for the step of moving the section", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* 모델 등록 */
	if (!SetModelParam())	return;

	/* Auto Focus 측정을 위한 Step Y Move Size 값 (단위: mm) */
	pstCfg->ph_focus.step_move_y	= m_edt_opt[0].GetTextToDouble();
	/* Focus에 측정되는 소재 두께 */
	pstCfg->set_align.dof_film_thick= m_edt_opt[1].GetTextToDouble();
	/* PH 간의 떨어진 거리 즉, Mark 간의 넓은 폭의 길이 (Distance. mm) */
	pstCfg->ph_focus.mark_period	= m_edt_opt[2].GetTextToDouble();
	/* 매 측정마다 측정되는 반복 횟수 */
	pstCfg->ph_focus.repeat_count	= (UINT16)m_edt_opt[3].GetTextToNum();

	/* 스레드에 값 설정 후 동작 시킴*/
	m_pMainThread->StartFocus((UINT16)m_edt_opt[4].GetTextToNum(),
							  (UINT16)m_edt_opt[3].GetTextToNum(),
							  m_edt_opt[0].GetTextToDouble());
}

/*
 desc : Live View 출력 여부 설정
 parm : None
 retn : None
*/
VOID CDlgMain::SetLiveView()
{
	BOOL bSucc		= FALSE;
	UINT32 u32OnOff	= 0x00000000;

	/* Live View 출력 여부 확인 */
	if (m_chk_cam[0].GetCheck())	/* Live View */
	{
		/* Trigger Parameter 갱신 */
		UpdateTrigParam();
		/* Align Camera is Live Mode */
		uvEng_Camera_SetCamMode(ENG_VCCM::en_live_mode);
		/* Trigger & Strobe : Enabled */
		bSucc = uvEng_Trig_ReqTriggerStrobe(TRUE);
		if (bSucc)
		{
			bSucc = uvEng_Trig_ReqEncoderLive(m_u8ACamNo);
		}
	}
	else
	{
		if (uvEng_Camera_GetCamMode() != ENG_VCCM::en_live_mode)	return;
		/* Trigger Disabled */
		bSucc = uvEng_Trig_ReqEncoderOutReset();
		if (bSucc)
		{
			/* Trigger & Strobe : Disabled */
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
					if (GetTickCount64() > (u64Tick + 2500))	break;
				}
				Sleep(10);

			}	while (1);

			/* Align Camera is Calibration Mode */
			uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
		}
	}
}

/*
 desc : 기본적으로 환경 파일에 설정된 값을 적재
 parm : None
 retn : None
*/
VOID CDlgMain::LoadDataConfig()
{
	LPG_CIEA pstCfg	= uvEng_GetConfig();

	/* 현재 MC2에 설정된 이동 거리 및 속도 값 출력 */
	m_edt_mc2[0].SetTextToNum(pstCfg->mc2_svc.move_dist, 4);
	m_edt_mc2[1].SetTextToNum(pstCfg->mc2_svc.move_velo, 4);

	/* Auto Focus 측정을 위한 Step Y Move Size 초기 값 (단위: mm) */
	m_edt_opt[0].SetTextToNum(pstCfg->ph_focus.step_move_y, 4);
	/* Focus에 측정되는 소재 두께 */
	m_edt_opt[1].SetTextToNum(pstCfg->set_align.dof_film_thick, 4);
	/* PH 간의 떨어진 거리 즉, Mark 간의 넓은 폭의 길이 (Distance. mm) */
	m_edt_opt[2].SetTextToNum(pstCfg->ph_focus.mark_period, 4);
	/* 매 측정마다 측정되는 반복 횟수 */
	m_edt_opt[3].SetTextToNum(pstCfg->ph_focus.repeat_count);
	/* Y 축 Step 이동 횟수 */
	m_edt_opt[4].SetTextToNum(pstCfg->ph_focus.step_y_count);
}

/*
 desc : 현재 설정된 값을 환경 파일에 덮어쓰기
 parm : None
 retn : None
*/
VOID CDlgMain::SaveDataToFile()
{
	LPG_CIEA pstCfg	= uvEng_GetConfig();

	/* 현재 MC2에 설정된 이동 거리 및 속도 값 저장 */
	pstCfg->mc2_svc.move_dist			= m_edt_mc2[0].GetTextToDouble();
	pstCfg->mc2_svc.move_velo			= m_edt_mc2[1].GetTextToDouble();

	/* Auto Focus 측정을 위한 Step Y Move Size 초기 값 (단위: mm) */
	pstCfg->ph_focus.step_move_y		= m_edt_opt[0].GetTextToDouble();
#if 0	/* 이 Tool에서는 저장하지 않는다. (Align Camera Focus에서 측정된 값을 가져오므로 ...) */
	/* Focus에 측정되는 소재 두께 */
	pstCfg->set_align.dof_film_thick	= m_edt_opt[1].GetTextToDouble();
#endif
	/* PH 간의 떨어진 거리 (Mark 간의 떨어진 폭 거리) */
	pstCfg->ph_focus.mark_period		= m_edt_opt[2].GetTextToDouble();
	/* 매 측정되는 구간마다 반복 측정 개수 */
	pstCfg->ph_focus.repeat_count		= (UINT16)m_edt_opt[3].GetTextToNum();
#if 0
	/* Trigger Time */
	pstCfg->trig_set.trig_on_time[0]	= (UINT32)m_edt_trg[0].GetTextToNum();
	pstCfg->trig_set.strob_on_time[0]	= (UINT32)m_edt_trg[1].GetTextToNum();
	pstCfg->trig_set.trig_delay_time[0]	= (UINT32)m_edt_trg[2].GetTextToNum();
#endif
	/* 환경 파일에 저장된 마크 처음 측정 위치와 현재 모션의 위치가 다르면 변경할지 여부 묻기 */
	if (pstCfg->ph_focus.mark2_stage[0] != uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x) ||
		pstCfg->ph_focus.mark2_stage[1] != uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y) ||
		pstCfg->ph_focus.mark2_acam_x[0] != uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam1) ||
		pstCfg->ph_focus.mark2_acam_x[1] != uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam2))
	{
		if (IDYES == AfxMessageBox(L"Mark Inspection start position has been changed\n"
								   "Shall we change it to a new position value?", MB_YESNO))
		{
			pstCfg->ph_focus.mark2_stage[0]	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);
			pstCfg->ph_focus.mark2_stage[1]	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);
			if (0x01 == m_u8ACamNo)
			{
				pstCfg->ph_focus.mark2_acam_x[0]	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam1);
			}
			else
			{
				pstCfg->ph_focus.mark2_acam_x[1]	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam2);
			}
		}
	}

	/* 환경 설정 값을 파일로 저장 */
	uvEng_SaveConfig();
#ifndef _DEBUG
	AfxMessageBox(L"Save completed successfully", MB_OK);
#endif
}

/*
 desc : 현재 자동 Focusing 작업 상태인지 확인
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::IsRunFocus()
{
	return m_pMainThread->IsRunFocus();
}

/*
 desc : Trigger Parameter 갱신
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateTrigParam()
{
	LPG_CIEA pstCfg	= uvEng_GetConfig();

	/* Trigger 측정 속성 값 변경 (Trigger OnTime / Strobe OnTime / Trigger Delay Time */
	pstCfg->trig_step.trig_on_time[0]	= (UINT32)m_edt_trg[0].GetTextToNum();
	pstCfg->trig_step.strob_on_time[0]	= (UINT32)m_edt_trg[1].GetTextToNum();
	pstCfg->trig_step.trig_delay_time[0]	= (UINT32)m_edt_trg[2].GetTextToNum();
}

/*
 desc : 그리드 컨트롤에 있는 데이터를 파일로 저장
 parm : None
 retn : None
*/
VOID CDlgMain::SaveAsExcelFile()
{
	TCHAR tzTempFile[MAX_PATH_LEN]	= {NULL};
	SYSTEMTIME stTm	= {NULL};

	/* 등록된 데이터가 없는지 확인 */
	if (m_pGridMeas->GetRowCount() < 2)	return;

	/* 현재 시간 */
	GetLocalTime(&stTm);
	/* 윈도 임시 파일로 저장 후 열기 */
	swprintf_s(tzTempFile, MAX_PATH_LEN, L"%s\\vdof\\pdof_%02u%02u%02u_%02u%02u%02u.csv",
			   g_tzWorkDir, stTm.wYear-2000, stTm.wMonth, stTm.wDay,
			   stTm.wHour, stTm.wMinute, stTm.wSecond);

	/* 파일로 저장 */
	m_pGridMeas->Save(tzTempFile);

	/* 저장된 파일 열기 */
	ShellExecute(NULL, _T("open"), _T("excel.exe"), tzTempFile, NULL, SW_SHOW);
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
		uvEng_Camera_DrawLiveBitmap(hDC, rDraw, m_u8ACamNo);
		::ReleaseDC(m_pic_ctl[0].m_hWnd, hDC);
	}
}