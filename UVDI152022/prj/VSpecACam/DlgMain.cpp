
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
	m_bDrawBG			= 0x01;
	m_hIcon				= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_i32ACamZAxisPos	= 0;
	m_pMainThread		= NULL;
	m_pGridPos			= NULL;
	m_hWndView			= NULL;
	m_u64DrawTime		= 0;
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
	for (i=0; i<10; i++)	DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);

	/* Normal - Text */
	u32StartID	= IDC_MAIN_TXT_MC2_MOVE;
	for (i=0; i<15; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Normal - Button */
	u32StartID	= IDC_MAIN_BTN_MC2_MOVE_UP;
	for (i=0; i<12; i++)	DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);

	/* for RunMode */
	u32StartID	= IDC_MAIN_CHK_RUN_ANGLE;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_chk_run[i]);

	/* for MC2 */
	u32StartID	= IDC_MAIN_CHK_MC2_DRV_0;
	for (i=0; i<8; i++)		DDX_Control(dx, u32StartID+i,	m_chk_mc2[i]);
	u32StartID	= IDC_MAIN_EDT_MC2_DIST;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_edt_mc2[i]);

	/* for Align Camera */
	u32StartID	= IDC_MAIN_CHK_ACAM_NO_1;
	for (i=0; i<5; i++)		DDX_Control(dx, u32StartID+i,	m_chk_cam[i]);
	u32StartID	= IDC_MAIN_EDT_ACAM_STEP;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_edt_cam[i]);

	/* for Calibration Result */
	u32StartID	= IDC_MAIN_EDT_SPEC_ANGLE;
	for (i=0; i<8; i++)		DDX_Control(dx, u32StartID+i,	m_edt_rst[i]);

	/* Picture */
	u32StartID	= IDC_MAIN_PIC_VIEW;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_pic_ctl[i]);

	/* for Trigger Time */
	u32StartID	= IDC_MAIN_EDT_STROBE_ONTIME;
	for (i=0; i<3; i++)		DDX_Control(dx, u32StartID+i,	m_edt_trg[i]);
}

BEGIN_MESSAGE_MAP(CDlgMain, CMyDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MAIN_THREAD,	OnMsgThread)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_BTN_MC2_MOVE_UP,	IDC_MAIN_BTN_EXIT,		OnBtnClicked)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_CHK_ACAM_NO_1,	IDC_MAIN_CHK_ACAM_LIVE,	OnChkClickACam)
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
	InitSetup();
	InitGrid();

	/* 기본 환경 설정 정보 적재 */
	LoadDataConfig();

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
	/* 기본 감시 스레드 메모리 해제 */
	if (m_pMainThread)
	{
		m_pMainThread->Stop();
		while (m_pMainThread->IsBusy())	m_pMainThread->Sleep();
		delete m_pMainThread;
		m_pMainThread	= NULL;
	}

	/* 라이브러리 해제 !!! 맨 마지막에 코딩 !!! */
	CloseLib();
	/* GridControl 해제 */
	CloseGrid();

	/* 임시 측정 결과 버퍼 해제 */
	m_lstResult.RemoveAll();
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
	for (i=0; i<10; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* Normal - Text */
	for (i=0; i<15; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(TRUE);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* Normal - Button */
	for (i=0; i<12; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
	}

	/* for RunMode */
	for (i=0; i<2; i++)
	{
		m_chk_run[i].SetLogFont(g_lf);
		m_chk_run[i].SetBgColor(RGB(0xff, 0xff, 0xff));
		m_chk_run[i].SetReadOnly(FALSE);
//		m_chk_run[i].EnableWindow(FALSE);
	}

	/* 무조건 처음에는 회전 각도 구해야 됨 */
	m_chk_run[0].SetCheck(1);

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
	for (i=0; i<5; i++)
	{
		m_chk_cam[i].SetLogFont(g_lf);
		m_chk_cam[i].SetBgColor(RGB(0xff, 0xff, 0xff));
		m_chk_cam[i].SetReadOnly(FALSE);
		m_chk_cam[i].EnableWindow(FALSE);
	}
	/* 무조건 1번 카메라는 Check 해야 됨 */
	m_chk_cam[0].SetCheck(1);

	for (i=0; i<1; i++)
	{
		m_edt_cam[i].SetEditFont(&g_lf);
		m_edt_cam[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_cam[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_cam[i].SetInputType(ENM_DITM::en_float);
	}

	/* for Cali Result */
	for (i=0; i<8; i++)
	{
		m_edt_rst[i].SetEditFont(&g_lf);
		m_edt_rst[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_rst[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		if (i != 6)	m_edt_rst[i].SetInputType(ENM_DITM::en_float);
		else		m_edt_rst[i].SetInputType(ENM_DITM::en_int8);
	}

	/* for Trigger Time */
	for (i=0; i<3; i++)
	{
		m_edt_trg[i].SetEditFont(&g_lf);
		m_edt_trg[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_trg[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_trg[i].SetInputType(ENM_DITM::en_int8);
	}

	/* Live 영역 핸들 즉, Picture Control 핸들 */
	m_hWndView	= m_pic_ctl[0].GetSafeHwnd();
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

	/* for Align Camera - 체크 버튼 활성화 처리 */
	for (i=0; i<pstCAM->acam_count; i++)
	{
		m_chk_cam[i].EnableWindow(TRUE);
	}

	/* 기본적으로 반복 측정 횟수 설정 */
	m_edt_rst[5].SetTextToNum(10, FALSE);

	/* 실행 환경에 맞는 검색 모델 등록 */
	SetRegistModel();
}

/*
 desc : Library Init
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::InitLib()
{
	if (!uvEng_Init(ENG_ERVM::en_acam_spec))	return FALSE;

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
 desc : 그리드 컨트롤 생성
 parm : None
 retn : None
*/
VOID CDlgMain::InitGrid()
{
	TCHAR tzCols[3][16]	= { L"Item", L"Pos (mm)" }, tzRows[32] = {NULL};
	INT32 i32Width[3]	= { 52, 85 }, i, j=1;
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
	if (UINT8(ENG_VCPK::en_camera_basler_ipv4) == pstCfg->set_comn.align_camera_kind)
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
			stGV.strText= L"0.0000";
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

	/* 본문 설정 - for Basler Camera Position */
	if (pstCfg->set_comn.align_camera_kind == UINT8(ENG_VCPK::en_camera_basler_ipv4))
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
 desc : 그리드 컨트롤 해제
 parm : None
 retn : None
*/
VOID CDlgMain::CloseGrid()
{
	/* for Motor Position */
	if (m_pGridPos)
	{
		if (m_pGridPos->GetSafeHwnd())	m_pGridPos->DestroyWindow();
		delete m_pGridPos;
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
	case IDC_MAIN_BTN_EXIT			:	PostMessage(WM_CLOSE, 0, 0L);			break;
	case IDC_MAIN_BTN_MC2_RESET		:	MC2Recovery();							break;
	case IDC_MAIN_BTN_MC2_MOVE_UP	:	MotionMove(ENG_MDMD::en_move_up);		break;
	case IDC_MAIN_BTN_MC2_MOVE_DN	:	MotionMove(ENG_MDMD::en_move_down);		break;
	case IDC_MAIN_BTN_ACAM_MOVE_UP	:	ACamMove(ENG_MDMD::en_move_axis_up);	break;
	case IDC_MAIN_BTN_ACAM_MOVE_DN	:	ACamMove(ENG_MDMD::en_move_axis_down);	break;
	case IDC_MAIN_BTN_ACAM_HOMING	:	ACamZAxisRecovery();					break;
	case IDC_MAIN_BTN_START			:	StartSpec();							break;
	case IDC_MAIN_BTN_STOP			:	StopSpec();								break;
	case IDC_MAIN_BTN_MOVE			:	MovePosMeasure();						break;
	case IDC_MAIN_BTN_GRAB			:	MarkGrabbedResult();					break;
	case IDC_MAIN_BTN_SAVE			:	SaveDataToFile();						break;
	}
}

/*
 desc : 체크 (for Align Camera) 클릭 이벤트
 parm : id	- [in]  Click ID
 retn : None
*/
VOID CDlgMain::OnChkClickACam(UINT32 id)
{
	UINT8 i	= 0x00;

	switch (id)
	{
	case IDC_MAIN_CHK_RUN_ANGLE	:
	case IDC_MAIN_CHK_RUN_PIXEL	:
		for (i=0; i<2; i++)	m_chk_run[i].SetCheck(0);
		m_chk_run[id-IDC_MAIN_CHK_RUN_ANGLE].SetCheck(1);
		break;
	case IDC_MAIN_CHK_ACAM_NO_1	:
	case IDC_MAIN_CHK_ACAM_NO_2	:
	case IDC_MAIN_CHK_ACAM_NO_3	:
	case IDC_MAIN_CHK_ACAM_NO_4	:
		for (i=0; i<4; i++)	m_chk_cam[i].SetCheck(0);
		m_chk_cam[id-IDC_MAIN_CHK_ACAM_NO_1].SetCheck(1);
		break;
	case IDC_MAIN_CHK_ACAM_LIVE	:	SetLiveView();	break;
	}

	/* Trigger Time 값 재-적재 */
	LoadTriggerTime(GetCheckACam());
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
	case MSG_ID_CALI_INIT		: UpdateCaliInit();			break;
	case MSG_ID_CALI_RESULT		: UpdateCaliResult(FALSE);	break;
	case MSG_ID_CALI_COMPLETE	: UpdateCaliResult(TRUE);	break;
	case MSG_ID_GRAB_VIEW		: InvalidateView();			break;
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
	/* for Motor Position (mm) */
	UpdateMotorPos();
	/* for PLC Value */
	UpdatePLCVal();
	/* Control : Enable/Disable */
	UpdateEnableCtrl();
	/* Grabbed Image View 출력 */
	UpdateLiveView();
	UpdateStepRate();
}

/*
 desc : Align Camera 관련 Control 갱신
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateCtrlACam()
{
	BOOL bEnable	= TRUE;

	/* 얼라인 카메라가 연결되어 있지 않으면 */
	if (bEnable)	bEnable	= uvCmn_Camera_IsConnected();
#if 0
	/* 현재 Live View 화면이면, 현재 측정 모드인지 여부에 따라 */
	if (bEnable)	bEnable	= uvEng_Camera_IsCamModeLive();
#endif
	/* 현재 측정 작업 중인지 여부 */
	if (bEnable)	bEnable	= !m_pMainThread->IsRunSpec();
	/* Live Check 버튼 활성화 및 체크 해제 처리 */
	m_chk_cam[4].EnableWindow(bEnable);
	if (!bEnable)
	{
		if (m_chk_cam[4].GetCheck())	m_chk_cam[4].SetCheck(0);
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
			dbPos[0] = m_pGridPos->GetItemTextToFloat(j, 1);
			dbPos[1] = (DOUBLE)ROUNDED(pstDP->focus_motor_move_abs_position[i]/1000.0f, 3);
			if (dbPos[0] != dbPos[1])
			{
				m_pGridPos->SetItemText(j, 1, dbPos[1]);
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

	/* Align Camera 위치 (Basler 카메라인 경우만 해당) */
	if (UINT8(ENG_VCPK::en_camera_basler_ipv4) == pstCfg->set_comn.align_camera_kind)
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
 desc : 현재 선택 (체크)된 얼라인 카메라 번호 얻기
 parm : None
 retn : 선택된 얼라인 카메라 번호 (1-based), 실패인 경우 0xff
*/
UINT8 CDlgMain::GetCheckACam()
{
	UINT8 i	= 0x00;

	for (; i<uvEng_GetConfig()->set_cams.acam_count; i++)
	{
		if (m_chk_cam[i].GetCheck())	return (i+1);
	}

	return 0xff;
}

/*
 desc : 각종 장비 내의 PLC 값 갱신
 parm : None
 retn : None
*/
VOID CDlgMain::UpdatePLCVal()
{
	UINT8 u8ACamNo	= 0x00;
	INT32 i32PosZ	= 0;
	LPG_CIEA pstCfg	= uvEng_GetConfig();

	/* Align Camera (for Basler) - Z Axis Move Step & Speed */
	if (UINT8(ENG_VCPK::en_camera_basler_ipv4) == pstCfg->set_comn.align_camera_kind)
	{
		/* 현재 Align Camera의 Z Axis 이동 속도 및 현재 위치 (Position) 값 얻기 */
		u8ACamNo= GetCheckACam();
		if (0xff == u8ACamNo)	return;
		i32PosZ	= (INT32)ROUNDED(uvCmn_MCQ_GetACamCurrentPosZ(u8ACamNo) * 10000.0f, 0);
		if (m_i32ACamZAxisPos != i32PosZ)
		{
			TCHAR tzVal[32]	= {NULL};
			swprintf_s(tzVal, 32, L"%.4f", uvCmn_MCQ_GetACamCurrentPosZ(u8ACamNo));
			m_pGridPos->SetItemText(m_u32ACamZAxisRow, 1, tzVal);	/* Position */
			m_i32ACamZAxisPos = i32PosZ;
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
	BOOL bBusyMotion	= FALSE, bBusyRunFocus = FALSE;
	LPG_CMSI pstMC2Svc	= &uvEng_GetConfig()->mc2_svc;

	/* Focus 측정 진행 여부 */
	bBusyRunFocus	= m_pMainThread->IsRunSpec();
	/* Motion Busy 여부 */
	bBusyMotion		= uvCmn_MC2_IsAnyDriveBusy();
	/* MC2/Align Camera Up/Down Moving Button */
	for (i=0x00; i<4; i++)
	{
		m_btn_ctl[i].EnableWindow(!(bBusyMotion || bBusyRunFocus));
	}
	/* MC2 Drive Check Button */
	for (; i<uvEng_GetConfig()->mc2_svc.drive_count; i++)
	{
		m_chk_mc2[pstMC2Svc->axis_id[i]].EnableWindow(!(bBusyMotion || bBusyRunFocus));
	}
	/* Align Camera Check Button */
	for (; i<uvEng_GetConfig()->set_cams.acam_count; i++)
	{
		m_chk_cam[i].EnableWindow(!(bBusyMotion || bBusyRunFocus));
	}
	/* Run.Mode Check Button */
	for (; i<2; i++)
	{
		m_chk_cam[i].EnableWindow(!(bBusyMotion || bBusyRunFocus));
	}

	/* Normal Button */
	m_btn_ctl[4].EnableWindow(!(bBusyMotion || bBusyRunFocus));		/* ACam Homing */
	m_btn_ctl[5].EnableWindow(!bBusyRunFocus);						/* Reset */
	m_btn_ctl[6].EnableWindow(!(bBusyMotion || bBusyRunFocus));		/* Move */
	m_btn_ctl[7].EnableWindow(!(bBusyMotion || bBusyRunFocus));		/* Start */
	m_btn_ctl[8].EnableWindow(bBusyRunFocus);						/* Stop */
	m_btn_ctl[9].EnableWindow(!(bBusyMotion || bBusyRunFocus));		/* Save */
	m_btn_ctl[10].EnableWindow(!(bBusyMotion || bBusyRunFocus));	/* Save */
}

/*
 desc : 진행률 출력
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateStepRate()
{
	/* Step 출력 */
	if (!m_pMainThread->IsRunSpec())	return;
	TCHAR tzRate[64]	= {NULL};

	m_edt_rst[4].SetRedraw(FALSE);
	swprintf_s(tzRate, 64, L"%5.2f / %2u",
				m_pMainThread->GetStepRate(),
				m_pMainThread->GetMeasureCount());
	m_edt_rst[4].SetTextToStr(tzRate, TRUE);
	m_edt_rst[4].SetRedraw(TRUE);
	m_edt_rst[4].Invalidate(TRUE);
}

/*
 desc : Align Camera의 Calibration 초기화
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateCaliInit()
{
	UINT8 i	= 0x00;

	/* 컨트롤 값 초기화 */
	for (; i<4; i++)	m_edt_rst[i].SetTextToStr(L"", TRUE);
}

/*
 desc : Align Camera의 Calibration (Specification) 수행 후 결과 출력
 parm : flag	- [in]  TRUE : 모든 측정 작업 완료, FALSE : 1 회 측정 작업 완료
 retn : None
*/
VOID CDlgMain::UpdateCaliResult(BOOL flag)
{
	UINT8 u8ACamNo	= GetCheckACam();
	TCHAR tzVal[64]	= {NULL};
	LPG_ACGR pstCali= NULL;
	STM_RAPV stVal	= {NULL};

	/* 회전 각도 및 픽셀 크기 (um) 구하기 */
	swprintf_s(tzVal, 64, L"%.4f ˚",	uvEng_GetConfig()->acam_spec.spec_angle[u8ACamNo-1]);
	m_edt_rst[0].SetTextToStr(tzVal, TRUE);
	swprintf_s(tzVal, 64, L"%.4f um",	uvEng_GetConfig()->acam_spec.spec_pixel_um[u8ACamNo-1]);
	m_edt_rst[1].SetTextToStr(tzVal, TRUE);

	/* 매칭 결과 화면 갱신 */
	pstCali	= uvEng_Camera_GetLastGrabbedMark();
	if (pstCali && 0x00 != pstCali->marked)
	{
		swprintf_s(tzVal, 64, L"%.2f %%", pstCali->score_rate);
		m_edt_rst[2].SetTextToStr(tzVal, TRUE);
		swprintf_s(tzVal, 64, L"%.2f %%", pstCali->scale_rate);
		m_edt_rst[3].SetTextToStr(tzVal, TRUE);
	}

	/* 현재 수신된 데이터 등록 */
	stVal.value[0]	= uvEng_GetConfig()->acam_spec.spec_angle[u8ACamNo-1];
	stVal.value[1]	= uvEng_GetConfig()->acam_spec.spec_pixel_um[u8ACamNo-1];
	stVal.value[2]	= pstCali->score_rate;
	stVal.value[3]	= pstCali->scale_rate;
	m_lstResult.AddTail(stVal);

	/* 모든 측정이 완료되면, 정규 분포 진행*/
	if (flag)
	{
		CalcNormalDist(2.0f);
		AfxMessageBox(L"Operation compleed successfully", MB_OK);
	}
	/* 측정된 이미지 출력 */
	InvalidateView();
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
 desc : Align Camera - Recovery (Error 복구)
 parm : None
 retn : None
*/
VOID CDlgMain::ACamZAxisRecovery()
{
	uvEng_MCQ_SetACamHomingZAxis(0x01);
	uvEng_MCQ_SetACamHomingZAxis(0x02);
}

/*
 desc : 모션 (Motor Drive) 이동
 parm : direct	- [in]  Up or Down
 retn : None
*/
VOID CDlgMain::MotionMove(ENG_MDMD direct)
{
	UINT8 i	= 0x00, u8DrvNo = 0x00;	/* 0x00 ~ 0x07 */
	DOUBLE dbDrvPos	= 0.0f, dbMinPos, dbMaxPos, dbDist, dbVelo;
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
	dbDist	= m_edt_mc2[0].GetTextToDouble();
	dbVelo	= m_edt_mc2[1].GetTextToDouble();
	if (dbDist == 0.0f || dbVelo < 1.0f)
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
 desc : Align Camera (Motor Drive) 이동
 parm : direct	- [in]  Up or Down
 retn : None
*/
VOID CDlgMain::ACamMove(ENG_MDMD direct)
{
	UINT8 u8ACamNo/*, u8PhNo*/;
	INT32 i32PosZ	= 0;
	UINT32 u32PosZ	= 0;	/* unit : um */
	DOUBLE dbStepMM	= 0.0f, dbPosZ = 0.0f;
	LPG_CIEA pstCfg	= uvEng_GetConfig();
	LPG_LDDP pstMem	= &uvEng_ShMem_GetLuria()->directph;

	/* Photohead Z Axis 이동 */
	dbStepMM	= m_edt_cam[0].GetTextToDouble();

	/* Align Camera 위치 (Basler 카메라인 경우) */
	if (UINT8(ENG_VCPK::en_camera_basler_ipv4) == pstCfg->set_comn.align_camera_kind)
	{
		/* 선택된 얼라인 카메라 얻기 */
		u8ACamNo= GetCheckACam();
		/* 현재 얼라인 카메라 위치 얻기 */
		dbPosZ	= uvCmn_MCQ_GetACamCurrentPosZ(u8ACamNo);

		/* 위로 혹은 오른 쪽으로 이동하면, 기존 위치에 이동 거리 값만큼 더해 주기 */
		if (direct == ENG_MDMD::en_move_axis_up)	dbPosZ	+= dbStepMM;
		else										dbPosZ	-= dbStepMM;
		/* 현재 얼라인 카메라의 위치가 Range 벗어난 경우라면 ... */
		if (!pstCfg->set_cams.IsValidRangePos(dbPosZ))
		{
			AfxMessageBox(L"Movement range is exceeded", MB_ICONSTOP|MB_TOPMOST);
			return;
		}

		/* 이동 명령 실행 */
		if (!uvEng_MCQ_SetACamMovePosZ(u8ACamNo, 0x00, dbPosZ))
		{
			AfxMessageBox(L"Failed to execute the command for [Moving the Z Axis]", MB_ICONSTOP|MB_TOPMOST);
			return;
		}
	}
#if 0
	/* Photohead 위치 */
	else
	{
		/* 카메라가 장착된 포토헤드의 위치 정보 얻기 */
		u8PhNo	= pstCfg->set_ids.ph_no;
		u32PosZ	= pstMem->focus_motor_move_abs_position[u8PhNo-1];	/* unit : um */
		/* 위로 혹은 오른 쪽으로 이동하면, 기존 위치에 이동 거리 값만큼 더해 주기 */
		if (direct == ENG_MDMD::en_move_up || direct == ENG_MDMD::en_move_right)	dbPosZ	+= dbStepMM;
		else															dbPosZ	-= dbStepMM;
		/* Photohead Z Axis Up / Down 설정 */
		uvEng_Luria_ReqSetMotorAbsPosition(u8PhNo, u32PosZ);
	}
#endif
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
	dbPos	= pstCfg->acam_spec.quartz_stage_x;
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, dbPos, dbVelo);

	/* 모션 Y 축 이동 */
	dbPos	= pstCfg->acam_spec.quartz_stage_y[0];
	if (UINT8(ENG_VCPK::en_camera_basler_ipv4) == pstCfg->set_comn.align_camera_kind && 0x02 == GetCheckACam())
	{
		dbPos	= pstCfg->acam_spec.quartz_stage_y[1];
	}
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dbPos, dbVelo);

	/* 모션 Camera 축 이동 for Basler */
	if (UINT8(ENG_VCPK::en_camera_basler_ipv4) == pstCfg->set_comn.align_camera_kind)
	{
		UINT8 u8ACamNo	= GetCheckACam();
		/* 2 번 카메라를 선택한 경우 */
		if (0x02 == u8ACamNo)
		{
			/* 1 번째 카메라를 맨 왼쪽으로 이동 시킴 */
			uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, 0, dbVelo);
			/* 2 번째 카메라를 측정 위치로 이동 시킴 */
			dbPos	= pstCfg->acam_spec.quartz_acam[1];
			uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2, dbPos, dbVelo);
		}
		else
		{
			/* 2 번째 카메라를 맨 오른쪽으로 이동 시킴 */
			dbPos	= pstCfg->mc2_svc.max_dist[(UINT8)ENG_MMDI::en_align_cam2];
			uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2, dbPos, dbVelo);
			/* 1 번째 카메라를 측정 위치로 이동 시킴 */
			dbPos	= pstCfg->acam_spec.quartz_acam[0];
			uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, dbPos, dbVelo);
		}

		/* 얼라인 카메라 Z 축 이동 */
		for (i=0; i<pstCfg->set_cams.acam_count; i++)
		{
			uvEng_MCQ_SetACamMovePosZ(i+1, 0x00, pstCfg->acam_spec.acam_z_focus[i]);
		}
	}
#if 0
	else
	{
		/* 광학계 Z 축 이동 */
		uvEng_Luria_ReqSetMotorAbsPosition(pstCfg->set_ids.ph_no, pstCfg->acam_spec.acam_z_focus[0]);
	}
#endif
}

/*
 desc : 카메라의 조명 타입에 따른 트리거 보드의 채널 번호 값 반환
 parm : None
 retn : 0 or Later
*/
UINT8 CDlgMain::GetTrigChNo()
{
	UINT8 u8ChNo	= 0x00;
	UINT8 u8ACamNo	= GetCheckACam();
	LPG_CIEA pstCfg	= uvEng_GetConfig();

	if (u8ACamNo == 0xff)	return u8ChNo;

	switch (pstCfg->set_comn.strobe_lamp_type)
	{
	case 0x00	:	u8ChNo	= u8ACamNo == 1 ? 0x00 : 0x02;	break;	/* Amber */
	case 0x01	:	u8ChNo	= u8ACamNo == 1 ? 0x01 : 0x03;	break;	/* IR */
	}

	return u8ChNo;
}

/*
 desc : 얼라인 카메라 별로 Trigger Time 적재
 parm : cam_id	- [in]  Align Camera Number (1 or Later)
 retn : None
*/
VOID CDlgMain::LoadTriggerTime(UINT8 cam_id)
{
	UINT8 u8ChNo	= GetTrigChNo();
	LPG_CIEA pstCfg	= uvEng_GetConfig();
	
	m_edt_trg[0].SetTextToNum(pstCfg->trig_step.strob_on_time[u8ChNo]);
	m_edt_trg[1].SetTextToNum(pstCfg->trig_step.trig_on_time[u8ChNo]);
	m_edt_trg[2].SetTextToNum(pstCfg->trig_step.trig_delay_time[u8ChNo]);
}

/*
 desc : 포커스 측정 시작
 parm : None
 retn : None
*/
VOID CDlgMain::StartSpec()
{
	UINT8 u8ACamNo	= GetCheckACam();
	UINT8 u8ChNo	= u8ACamNo == 1 ? 0x00 : 0x02, u8RunMode = 0x00;
	INT32 i32Repeat	= 0;
	LPG_CIEA pstCfg	= uvEng_GetConfig();

	/* 반복 개수 얻기 */
	i32Repeat	= (INT32)m_edt_rst[5].GetTextToNum();
	if (i32Repeat < 1)
	{
		AfxMessageBox(L"No repeat count value entered", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* 임시 버퍼 해제 */
	m_lstResult.RemoveAll();

	/* 기존 Grabbed Image 모두 제거 */
	uvEng_Camera_ResetGrabbedImage();
	/* Align Camera is Calibration Mode */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);

	/* Trigger 측정 속성 값 변경 (Trigger OnTime / Strobe OnTime / Trigger Delay Time */
	pstCfg->trig_step.strob_on_time[u8ChNo]		= (UINT32)m_edt_trg[0].GetTextToNum();
	pstCfg->trig_step.trig_on_time[u8ChNo]		= (UINT32)m_edt_trg[1].GetTextToNum();
	pstCfg->trig_step.trig_delay_time[u8ChNo]	= (UINT32)m_edt_trg[2].GetTextToNum();

	/* 측정 모드 값 설정 */
	if (m_chk_run[1].GetCheck())	u8RunMode = 0x01;
	/* Spec 측정 시작 */
	m_pMainThread->StartSpec(u8RunMode, GetCheckACam(), UINT32(i32Repeat));
}

/*
 desc : 포커스 측정 시작
 parm : None
 retn : None
*/
VOID CDlgMain::StopSpec()
{
	/* 기존 Grabbed Image 모두 제거 */
	uvEng_Camera_ResetGrabbedImage();
	/* Align Camera is Calibration Mode */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);

	/* Spec 측정 시작 */
	m_pMainThread->StopSpec();
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
	if (m_chk_cam[4].GetCheck())	/* Live View */
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

	/* Align Camera에 설정된 이동 단계 거리 값 출력 */
	m_edt_cam[0].SetTextToNum(pstCfg->acam_spec.step_move_z, 3);

	/* for Trigger Time */
	LoadTriggerTime(0x01);
}

/*
 desc : 현재 설정된 값을 환경 파일에 덮어쓰기
 parm : None
 retn : None
*/
VOID CDlgMain::SaveDataToFile()
{
	UINT8 u8ACamNo		= GetCheckACam();
	ENG_MMDI enACamX	= u8ACamNo == 0x01 ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;
	LPG_CIEA pstCfg		= uvEng_GetConfig();
	LPG_CASI pstACamSpec= &uvEng_GetConfig()->acam_spec;

	/* 현재 MC2에 설정된 이동 거리 및 속도 값 저장 */
	pstCfg->mc2_svc.move_dist	= m_edt_mc2[0].GetTextToDouble();
	pstCfg->mc2_svc.move_velo	= m_edt_mc2[1].GetTextToDouble();

	/* Align Camera에 설정된 이동 단계 거리 값 저장 */
	pstACamSpec->step_move_z	= m_edt_cam[0].GetTextToDouble();

	/* 카메라 설치 회전 각도 */
	if (m_edt_rst[0].GetTextToDouble() != 0.0f)
	{
		pstACamSpec->spec_angle[u8ACamNo-1]	= m_edt_rst[0].GetTextToDouble();
	}
	/* 카메라 픽셀 크기 */
	if (m_edt_rst[1].GetTextToDouble() != 0.0f)
	{
		pstACamSpec->spec_pixel_um[u8ACamNo-1]= m_edt_rst[1].GetTextToDouble();
	}
	/* 현재 선택된 카메라 번호에 따라, Focus 위치 정보 */
	pstACamSpec->acam_z_focus[u8ACamNo-1]	= uvCmn_MCQ_GetACamCurrentPosZ(u8ACamNo);
	/* 현재 모션 위치 값 */
	pstACamSpec->quartz_stage_x				= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);
	pstACamSpec->quartz_stage_y[u8ACamNo-1]	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);
	pstACamSpec->quartz_acam[u8ACamNo-1]	= uvCmn_MC2_GetDrvAbsPos(enACamX);

	/* 환경 설정 값을 파일로 저장 */
	if (!uvEng_SaveConfig())	return;
#ifndef _DEBUG
	AfxMessageBox(L"Save completed successfully", MB_OK);
#endif
}

/*
 desc : 정규 분포 계산 (카메라의 각도 및 픽셀 크기)
 parm : rate	- [in]  정규 분포 계수 값
 retn : None
*/
VOID CDlgMain::CalcNormalDist(DOUBLE rate)
{
	TCHAR tzVal[64]	= {NULL};
	UINT32 i, j, u32Cnt[4], u32Total = (UINT32)m_lstResult.GetCount();
	DOUBLE dbSum[4]	= {NULL}, dbAvg[4] = {NULL}, dbStd[4] = {NULL};
	STM_RAPV stVal	= {NULL};

	/* 값이 동일하게 등록되어 있지 않는지 확인 */
	if (u32Total < 1)	return;

	/* 총합 구하기 */
	dbSum[0] = dbSum[1] = 0.0f;
	for (i=0; i<u32Total; i++)
	{
		stVal	= m_lstResult.GetAt(m_lstResult.FindIndex(i));
		for (j=0; j<4; j++)
		{
			dbSum[j] += stVal.value[j];
		}
	}

	/* 평균 구하기 */
	for (j=0; j<4; j++)
	{
		dbAvg[j]	= dbSum[j] / DOUBLE(u32Total);
		dbSum[j]	= 0.0f;	/* 다음 사용을 위해 초기화 */
	}

	/* 제곱근 구하기 */
	for (i=0; i<u32Total; i++)
	{
		stVal	= m_lstResult.GetAt(m_lstResult.FindIndex(i));
		for (j=0; j<4; j++)
		{
			dbSum[j]	+= pow(stVal.value[j] - dbAvg[j], 2.0f);
		}
	}
	/* 표준 편차 구하기 */
	for (j=0; j<4; j++)
	{
		if (dbSum[j] > 0.0f)	dbStd[j] = sqrt(dbSum[j] / DOUBLE(u32Total));
		dbSum[j]	= 0.0f;		/* 다음 사용을 위해 초기화 */
		u32Cnt[j]	= 0;		/* 다음 사용을 위해 초기화 */
	}

	/* 표준 편차 구간에 벗어난 값은 제외 (이상치 제외)하고 총 합 구하기 (정규 분포로 값 구하기) */
	/* 총합 구하기 */
	for (i=0; i<u32Total; i++)
	{
		stVal	= m_lstResult.GetAt(m_lstResult.FindIndex(i));

		/* (현재 값) 이 (평균 값 기준 +/- 편차 값) 값 범위 내에 있는지 확인 */
		for (j=0; j<4; j++)
		{
			if (((dbAvg[j] - dbStd[j] * rate) <= stVal.value[j]) &&
				((dbAvg[j] + dbStd[j] * rate) >= stVal.value[j]))
			{
				dbSum[j]	+= stVal.value[j];
				u32Cnt[j]++;
			}
		}
	}

	/* 평균 구하기 */
	for (j=0; j<4; j++)	dbAvg[j]	= dbSum[j] / DOUBLE(u32Cnt[j]);
#if 0
	/* 회전 각도 (Degree) 구하기 */
	if (m_chk_run[0].GetCheck())
	{
		swprintf_s(tzVal, 64, L"%.4f ˚",	dbAvg[0]);	/* Angle */
		m_edt_rst[0].SetTextToStr(tzVal, TRUE);
		m_edt_rst[1].SetTextToStr(L"0.0000 um", TRUE);	/* Pixel */
	}
	/* 픽셀 크기 (um) 구하기 */
	else
	{
		m_edt_rst[0].SetTextToStr(L"0.0000 ˚", TRUE);	/* Angle */
		swprintf_s(tzVal, 64, L"%.4f um",	dbAvg[1]);	/* Pixel */
		m_edt_rst[1].SetTextToStr(tzVal, TRUE);
	}
#else
	/* 회전 각도 (Degree) 구하기 */
	swprintf_s(tzVal, 64, L"%.4f ˚",	dbAvg[0]);	/* Angle */
	m_edt_rst[0].SetTextToStr(tzVal, TRUE);
	m_edt_rst[0].Invalidate(TRUE);
	/* 픽셀 크기 (um) 구하기 */
	swprintf_s(tzVal, 64, L"%.4f um",	dbAvg[1]);	/* Pixel */
	m_edt_rst[1].SetTextToStr(tzVal, TRUE);
	m_edt_rst[1].Invalidate(TRUE);
#endif
	swprintf_s(tzVal, 64, L"%.2f %%",	dbAvg[2]);	/* Score */
	m_edt_rst[2].SetTextToStr(tzVal, TRUE);
	m_edt_rst[2].Invalidate(TRUE);
	swprintf_s(tzVal, 64, L"%.2f %%",	dbAvg[3]);	/* Scale */
	m_edt_rst[3].SetTextToStr(tzVal, TRUE);
	m_edt_rst[3].Invalidate(TRUE);
}

/*
 desc : 현재 위치에서 QuartZ 인식 후 카메라의 중심에서 얼마나 떨어졌는지 확인
 parm : None
 retn : None
*/
VOID CDlgMain::MarkGrabbedResult()
{
	TCHAR tzResult[32]	= {NULL};
	UINT8 u8ACamID		= GetCheckACam();
	BOOL bFinded		= FALSE;
	UINT64 u64TickLoop	= 0, u64StartTick;
	LPG_ACGR pstResult	= NULL;

	/* 현재 Live Mode 방식인지 확인 */
	if (m_chk_cam[4].GetCheck())
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

	/* 첫번 행과 열 부분 값 초기화 */
	m_edt_rst[2].SetTextToStr(L"+0.000 %%");
	m_edt_rst[3].SetTextToStr(L"+0.000 %%");
	m_edt_rst[6].SetTextToStr(L"+0.000 um");
	m_edt_rst[7].SetTextToStr(L"+0.000 um");

	/* 기존 검색된 Grabbed Data & Image 제거 */
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

	/* 최대 5초 정도 대기 */
	do {
		if (u64StartTick+5000 < GetTickCount64())	break;
		uvCmn_uSleep(100);

		/* 검색된 결과가 있는지 조사 */
		pstResult	= uvEng_Camera_RunModelCali(u8ACamID, 0xff);
		if (pstResult->marked)	break;

	} while (1);

	/* 검색된 결과가 있는지 여부 */
	if (!pstResult->marked)
	{
 		AfxMessageBox(L"There is no analysis result", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* 화면 갱신 */
	InvalidateView();

	/* 결과 값 출력 */
	swprintf_s(tzResult, 32, L"%+.03f", pstResult->move_mm_x);
	m_edt_rst[6].SetTextToStr(tzResult, TRUE);
	swprintf_s(tzResult, 32, L"%+.03f", pstResult->move_mm_y);
	m_edt_rst[7].SetTextToStr(tzResult, TRUE);
	/* Score */
	swprintf_s(tzResult, 32, L"%+.03f", pstResult->score_rate);
	m_edt_rst[2].SetTextToStr(tzResult, TRUE);
	/* Scale */
	swprintf_s(tzResult, 32, L"%+.03f", pstResult->scale_rate);
	m_edt_rst[3].SetTextToStr(tzResult, TRUE);
#if 0
	/* Camera 동작 모드 설정 */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
#endif
}

/*
 desc : 새로운 검색을 위해 검색 대상의 모델 등록
 parm : None
 retn : Non
*/
VOID CDlgMain::SetRegistModel()
{
	UINT32 u32Model	= (UINT32)ENG_MMDT::en_circle, i = 0;
	DOUBLE dbMSize	= 2.0f /* um */, dbMColor = uvEng_GetConfig()->acam_spec.in_ring_color /* 256:Black, 128:White */;
	LPG_CIEA pstCfg	= uvEng_GetConfig();
	/* 검색 대상에 대한 모델 등록 */
	dbMSize	= pstCfg->acam_spec.in_ring_size * 1000.0f;

	for (i=0; i<pstCfg->set_cams.acam_count; i++)
	{
		if (!uvEng_Camera_SetModelDefineEx(i+1,
										   pstCfg->mark_find.detail_level,
										   pstCfg->mark_find.model_speed,
										   pstCfg->mark_find.model_smooth,
										   &u32Model, &dbMColor, &dbMSize,
										   NULL, NULL, NULL, 1, 
										   pstCfg->acam_spec.in_ring_scale_min,
										   pstCfg->acam_spec.in_ring_scale_max))
		{
			AfxMessageBox(L"Failed to setup the model for search target", MB_ICONSTOP|MB_TOPMOST);
			return;
		}
	}
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

	if (!uvEng_Camera_IsCamModeLive())	return;
	/* 이미지가 출력될 윈도 영역 */
	m_pic_ctl[0].GetClientRect(&rDraw);
	hDC	= ::GetDC(m_pic_ctl[0].m_hWnd);
	if (hDC)
	{
		uvEng_Camera_DrawLiveBitmap(hDC, rDraw, GetCheckACam());
		::ReleaseDC(m_pic_ctl[0].m_hWnd, hDC);
	}
}
