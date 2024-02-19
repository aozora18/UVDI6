
/*
 desc : Main Dialog
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgMain.h"
#include "MainThread.h"

#include "DlgEFEM.h"
#include "DlgLogs.h"
#include "DlgLuri.h"
#include "DlgMC2.h"
#include "DlgMoni.h"
#include "DlgPlcQ.h"
#include "DlgTrig.h"
#include "DlgVisi.h"


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

	/* 자식 윈도 초기화 */
	m_pDlgLast		= NULL;
	m_pDlgLogs		= NULL;
	m_pDlgLuri		= NULL;
	m_pDlgEFEM		= NULL;
	m_pDlgMC2		= NULL;
	m_pDlgMoni		= NULL;
	m_pDlgPlcQ		= NULL;
	m_pDlgTrig		= NULL;
	m_pDlgVisi		= NULL;

	m_u64UpdateCtrl	= GetTickCount64();
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

	/* Groupbox - Normal */
	u32StartID	= IDC_MAIN_GRP_INFO;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Text - Normal */
	u32StartID	= IDC_MAIN_TXT_VER;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Button - Normal */
	u32StartID	= IDC_MAIN_BTN_MONI;
	for (i=0; i<8; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Check box - Normal */
	u32StartID	= IDC_MAIN_CHK_MC2;
	for (i=0; i<6; i++)		DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgMain, CMyDialog)
	ON_MESSAGE(WM_MAIN_THREAD,			OnMsgThread)
	ON_MESSAGE(WM_MAIN_TRAY_ICON,		OnMsgTrayIcon)
	ON_COMMAND(WM_MAIN_APPS_EXIT,		OnCmdAppExit)
	ON_COMMAND(WM_MAIN_LURIA_RESTART,	OnCmdLuriaRestart)
	ON_COMMAND(WM_MAIN_SHOW_HIDE,		OnCmdShowHide)
	ON_WM_SYSCOMMAND()
	ON_WM_SHOWWINDOW()
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_BTN_MONI, IDC_MAIN_BTN_LOGS,	OnBtnClicked)
END_MESSAGE_MAP()

/*
 desc : 시스템 명령어 제어
 parm : id		- 선택된 항목 ID 값
		lparam	- ???
 retn : 1 - 성공 / 0 - 실패
*/
VOID CDlgMain::OnSysCommand(UINT32 id, LPARAM lparam)
{
#ifndef _DEBUG
	switch (id)
	{
	case SC_CLOSE	:
		ShowWindow (FALSE);
		m_csTray.TrayShow(FALSE);
		return;
	}
#endif
	CMyDialog::OnSysCommand(id, lparam);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::OnInitDlg()
{
	BOOL bSucc	= TRUE;

	/* 실행되는 프로그램을 Sub Monitor에 출력 */
	/*uvCmn_MoveSubMonitor(m_hWnd);*/

	/* Main Thread 생성 */
	m_pMainThread	= new CMainThread(m_hWnd);
	ASSERT(m_pMainThread);
	if (!m_pMainThread->CreateThread(0, NULL, NORMAL_THREAD_SPEED))
	{
		delete m_pMainThread;
		m_pMainThread	= NULL;
	}

	/* 윈도 컨트롤 초기화 */
	InitCtrl();
	/* 라이브러리 초기화 */
	if (bSucc)	bSucc	= InitLib();
#ifndef _DEBUG
	/* Tray Icon 추가 */
	m_csTray.TrayShow(TRUE);
	m_csTray.AddTrayIcon(GetSafeHwnd(), IDR_MAINFRAME);
#endif
	return bSucc;
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
	/* 자식 윈도 해제 */
	KillChildDialog();
	/* 라이브러리 해제 */
	CloseLib();
#ifndef _DEBUG
	/* 트레이 아이콘 삭제 */
	m_csTray.DelTrayIcon(m_hWnd);
#endif
}

/*
 desc : 기존 출력된 자식 윈도 메모리 해제
 parm : None
 retn : None
*/
VOID CDlgMain::KillChildDialog()
{
	/* 가장 마지막에 출력된 자식 다이얼로그 메모리 해제 */
	if (m_pDlgLast)
	{
		if (m_pDlgLast->GetSafeHwnd())	m_pDlgLast->DestroyWindow();
		delete m_pDlgLast;

		/* 모든 자식 다이얼로그 객체 초기화 */
		m_pDlgEFEM	= NULL;
		m_pDlgLogs	= NULL;
		m_pDlgLuri	= NULL;
		m_pDlgLast	= NULL;
		m_pDlgMC2	= NULL;
		m_pDlgMoni	= NULL;
		m_pDlgPlcQ	= NULL;
		m_pDlgTrig	= NULL;
		m_pDlgVisi	= NULL;
	}
}

/*
 desc : 윈도가 화면에 보여질 때 수행 호출됨
 parm : show	- [in] 
		status	- [in] 
 retn : None
*/
VOID CDlgMain::OnShowWindow(BOOL show, UINT status)
{
	/* Expo Child 윈도 활성화 */
	OnBtnClicked(IDC_MAIN_BTN_MONI);

	CMyDialog::OnShowWindow(show, status);
}

/*
 desc : 자식 다이얼로그 윈도 생성
 parm : dlg_id	- [in]  생성하고자 하는 자식 윈도 ID
 retn : None
*/
VOID CDlgMain::CreateChildDialog(UINT32 dlg_id)
{
	POINT ptStart	= {NULL};
	RECT rChild		= {NULL};
	/* 기존에 실행되어 있는 화면과 동일하면 리턴 처리 */
	if (m_pDlgLast && m_pDlgLast->GetWndID() == dlg_id)	return;
	/* 만약 환경 설정 메뉴인 경우라면 */
	/* 기존 실행된 자식 윈도 메모리 해제 */
	KillChildDialog();

	/* 자식 윈도 출력 시작 위치 */
	m_grp_ctl[0].GetWindowRect(&rChild);
	ScreenToClient(&rChild);
	/* 자식 윈도 출력 시작 위치*/
	ptStart.x	= rChild.left+ 5;
	ptStart.y	= rChild.top + 13;

	/* 새로운 자식 윈도 생성 */
	switch (dlg_id)
	{
	/* EFEM 화면 */
	case IDD_EFEM	:
		m_pDlgEFEM	= new CDlgEFEM(dlg_id, this);
		m_pDlgLast	= m_pDlgEFEM;
		break;
	/* LOGS 화면 */
	case IDD_LOGS	:
		m_pDlgLogs	= new CDlgLogs(dlg_id, this);
		m_pDlgLast	= m_pDlgLogs;
		break;
	/* Luria 화면 */
	case IDD_LURI	:
		m_pDlgLuri	= new CDlgLuri(dlg_id, this);
		m_pDlgLast	= m_pDlgLuri;
		break;
	/* MC2 화면 */
	case IDD_MC2	:
		m_pDlgMC2	= new CDlgMC2(dlg_id, this);
		m_pDlgLast	= m_pDlgMC2;
		break;
	/* Monitor (Main) 화면 */
	case IDD_MONI	:
		m_pDlgMoni	= new CDlgMoni(dlg_id, this);
		m_pDlgLast	= m_pDlgMoni;
		break;
	/* PLC 화면 */
	case IDD_PLCQ	:
		m_pDlgPlcQ	= new CDlgPlcQ(dlg_id, this);
		m_pDlgLast	= m_pDlgPlcQ;
		break;
	/* Trigger 화면 */
	case IDD_TRIG	:
		m_pDlgTrig	= new CDlgTrig(dlg_id, this);
		m_pDlgLast	= m_pDlgTrig;
		break;
	/* Vision 화면 */
	case IDD_VISI	:
		m_pDlgVisi	= new CDlgVisi(dlg_id, this);
		m_pDlgLast	= m_pDlgVisi;
		break;
	}
	/* 자식 다이얼로그 생성 */
	if (m_pDlgLast)
	{
		m_pDlgLast->Create(dlg_id);
		/* 해당 자식 윈도가 생성되었다면, 위치 이동해 줌 */
		::SetWindowPos(m_pDlgLast->GetSafeHwnd(),
					   HWND_TOP, ptStart.x, ptStart.y, 0, 0, SWP_NOSIZE);
	}

	/* 전체 화면 갱신 */
	Invalidate(FALSE);
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
	INT32 i;

	/* groupbox - normal */
	for (i=0; i<1; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* text - normal */
	for (i=0; i<1; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* button - normal */
	for (i=0; i<8; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
	/* check box - normal */
	for (i=0; i<6; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf);
		m_chk_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
		m_chk_ctl[i].SetReadOnly(TRUE);
	}
}

/*
 desc : Library Init
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::InitLib()
{
	if (!uvEng_Init(ENG_ERVM::en_monitoring, 0x00))	return FALSE;

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
 desc : Program Termination
 parm : None
 retn : None
*/
VOID CDlgMain::OnCmdAppExit()
{
	::PostMessage(m_hWnd, WM_CLOSE, 0, 0L);
}

/*
 desc : Restart Luria Service
 parm : None
 retn : None
*/
VOID CDlgMain::OnCmdLuriaRestart()
{
	uvCmn_RestartService(L"Luria");
}

/*
 desc : Program Show / Hide Window
 parm : None
 retn : None
*/
VOID CDlgMain::OnCmdShowHide()
{
#ifndef _DEBUG
	ShowWindow (!m_csTray.IsTrayShow());
	m_csTray.TrayShow(!m_csTray.IsTrayShow());
#endif
}

/*
 desc : Tray Icon Message Event
 parm : wparam	- [in]  Event ID
		lparam	- [in]  Event Data
 retn : 0L
*/
LRESULT CDlgMain::OnMsgTrayIcon(WPARAM wparam, LPARAM lparam)
{
#ifndef _DEBUG
	m_csTray.ProcTrayMsg(m_hWnd, wparam, lparam);
#endif
	return 0L;
}

/*
 desc : Main Thread에서 발생된 이벤트 처리
 parm : wparam	- [in]  Event ID
		lparam	- [in]  Event Data
 retn : 0L
*/
LRESULT CDlgMain::OnMsgThread(WPARAM wparam, LPARAM lparam)
{
	UINT64 u64Tick	= GetTickCount64();

	/* 윈도 컨트롤 영역 갱신 */
	if (m_pDlgLast)
	{
		/* 1초에 2번 정도 갱신 */
		if (m_u64UpdateCtrl+500 < u64Tick)
		{
			m_u64UpdateCtrl	= u64Tick;
			m_pDlgLast->UpdateCtrl();
		}
	}
	/* 상태 정보 갱신 */
	UpdatePeriod();

	return 0L;
}

/*
 desc : 일반 버튼 이벤트 처리
 parm : id	- [in]  Click된 버튼 ID
 retn : None
*/
VOID CDlgMain::OnBtnClicked(UINT32 id)
{
	TCHAR tzTitle[64]	= {NULL};
	TCHAR tzSubj[8][8]	= { L"Main", L"MC2", L"PLC", L"EFEM", L"OPTIC", L"TRIG", L"VISI", L"LOGS" };
	UINT32 u32IDD[8]	= { IDD_MONI, IDD_MC2, IDD_PLCQ, IDD_EFEM, IDD_LURI, IDD_TRIG, IDD_VISI, IDD_LOGS };

	/* 그룹 박스 타이틀 변경 */
	swprintf_s(tzTitle, 64, L"[ %s ] Information", tzSubj[id-IDC_MAIN_BTN_MONI]);
	m_grp_ctl[0].SetText(tzTitle);

	/* 자식 윈도 생성 */
	CreateChildDialog(u32IDD[id-IDC_MAIN_BTN_MONI]);
}

/*
 desc : 주기적으로 갱신되는 항목
 parm : None
 retn : None
*/
VOID CDlgMain::UpdatePeriod()
{
	UINT8 u8Chk	= 0x00, i;
	CUniToChar csCnv;
	m_chk_ctl[0].SetCheck(uvEng_ShMem_GetMC2()->link.is_connected);
	m_chk_ctl[1].SetCheck(uvEng_ShMem_GetPLC()->link->is_connected);
	m_chk_ctl[2].SetCheck(0x00);	/* Robot */
	m_chk_ctl[3].SetCheck(uvEng_ShMem_GetLuria()->link.is_connected);
	if (uvEng_GetConfig()->use_libs.use_trig)
	{
		LPG_TPQR pst = uvEng_ShMem_GetTrig();
		m_chk_ctl[4].SetCheck(uvEng_ShMem_GetTrigLink()->is_connected);
	}
	if (uvEng_GetConfig()->use_libs.use_acam_kind == (UINT8)ENG_VCPK::en_camera_ids_ph)
	{
		u8Chk	= uvEng_ShMem_GetVisi()->link[0]->is_connected;
	}
	else
	{
		for (i=0,u8Chk=0; i<uvEng_GetConfig()->set_cams.acam_count; i++)
		{
			u8Chk	+= uvEng_ShMem_GetVisi()->link[i]->is_connected;
		}
		u8Chk	= (u8Chk == uvEng_GetConfig()->set_cams.acam_count) ? 0x01 : 0x00;
	}
	m_chk_ctl[5].SetCheck(u8Chk);

	/* Version */
	m_txt_ctl[0].SetTextToStr(csCnv.Ansi2Uni(STR_VERSION_APP_ENGINE), TRUE);
}