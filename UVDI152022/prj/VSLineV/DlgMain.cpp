
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
	m_pMainThread		= NULL;
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgMain::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);
#if 0
	UINT32 i, u32StartID;
	/* Normal - Group box */
	u32StartID	= IDC_MAIN_GRP_MC2;
	for (i=0; i<8; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Normal - Text */
	u32StartID	= IDC_MAIN_TXT_MC2_MOVE;
	for (i=0; i<21; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Normal - Button */
	u32StartID	= IDC_MAIN_BTN_MC2_MOVE_UP;
	for (i=0; i<15; i++)	DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);

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
	u32StartID	= IDC_MAIN_EDT_STROBE_ONTIME;
	for (i=0; i<3; i++)		DDX_Control(dx, u32StartID+i,	m_edt_trg[i]);

	/* for Work status */
	u32StartID	= IDC_MAIN_EDT_STEP;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_edt_pgr[i]);
#endif
}

BEGIN_MESSAGE_MAP(CDlgMain, CMyDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MAIN_THREAD,	OnMsgThread)
#if 0
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_BTN_MC2_MOVE_UP,	IDC_MAIN_BTN_EXIT,		OnBtnClicked)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_CHK_ACAM_LIVE,	IDC_MAIN_CHK_ACAM_LIVE,	OnChkClickACam)
#endif
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
#if 0
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
	for (i=0; i<15; i++)
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
		m_edt_mc2[i].SetInputType(EN_DITM::en_float);
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
		m_edt_cam[i].SetInputType(EN_DITM::en_float);
	}
	/* for Trigger Time */
	for (i=0; i<3; i++)
	{
		m_edt_trg[i].SetEditFont(&g_lf);
		m_edt_trg[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_trg[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_trg[i].SetInputType(EN_DITM::en_hex10);
	}
	/* for Work Status */
	for (i=0; i<1; i++)
	{
		m_edt_pgr[i].SetEditFont(&g_lf);
		m_edt_pgr[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_pgr[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_pgr[i].SetInputType(EN_DITM::en_float);
	}
	/* for Measurement Option */
	for (i=0; i<5; i++)
	{
		m_edt_opt[i].SetEditFont(&g_lf);
		m_edt_opt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_opt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		if (0x03 < i)	m_edt_opt[i].SetInputType(EN_DITM::en_float);
		else			m_edt_opt[i].SetInputType(EN_DITM::en_int);
	}

	/* Align Camera Focus에서 측정된 값을 가져오므로 ... */
	m_edt_opt[1].SetReadOnly(TRUE);
	m_edt_opt[1].SetMouseClick(FALSE);
#endif
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
#if 0
	switch (id)
	{
	case IDC_MAIN_BTN_EXIT			:	PostMessage(WM_CLOSE, 0, 0L);		break;
	case IDC_MAIN_BTN_MC2_RESET		:	MC2Recovery();	ACamZAxisHome();	break;
	case IDC_MAIN_BTN_NEXT			:	NextStepMoveY();					break;
	case IDC_MAIN_BTN_ACAM_MOVE		:	ACamMoveAbs();						break;
	case IDC_MAIN_BTN_ACAM_HOMED	:	ACamHoming();						break;
	case IDC_MAIN_BTN_START			:	StartFocus();						break;
	case IDC_MAIN_BTN_STOP			:	StopFocus();						break;
	case IDC_MAIN_BTN_MOVE			:	MovePosMeasure();					break;
	case IDC_MAIN_BTN_SAVE			:	SaveDataToFile();					break;
	case IDC_MAIN_BTN_MC2_MOVE_UP	:	MotionMove(ENG_MDMD::en_move_up);		break;
	case IDC_MAIN_BTN_MC2_MOVE_DN	:	MotionMove(ENG_MDMD::en_move_down);		break;
	case IDC_MAIN_BTN_MODEL_FIND	:	RunFindModel();						break;
	case IDC_MAIN_BTN_MODEL_RESET	:	ResetFindModel();					break;
	case IDC_MAIN_BTN_TRIG_SET		:	SetTriggerParam();					break;
	case IDC_MAIN_BTN_FILE_XLS		:	SaveAsExcelFile();					break;
	}
#endif
}

/*
 desc : Main Thread에서 발생된 이벤트 처리
 parm : wparam	- [in]  Event ID
		lparam	- [in]  Event Data
 retn : 0L
*/
LRESULT CDlgMain::OnMsgThread(WPARAM wparam, LPARAM lparam)
{
#if 0
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
#endif
	return 0L;
}

/*
 desc : 주기적으로 갱신되는 항목
 parm : None
 retn : None
*/
VOID CDlgMain::UpdatePeriod()
{
}
