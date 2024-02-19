// DlgCalb.cpp: 구현 파일
//

#include "pch.h"
#include "DlgCalbAccuracy.h"
#include "./accuracy/DlgCalbAccuracyMeasure.h"
#include "afxdialogex.h"


/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgCalbAccuracy::CDlgCalbAccuracy(UINT32 id, CWnd* parent)
	: CDlgSubMenu(id, parent)
{
	m_enDlgID = ENG_CMDI_SUB::en_menu_sub_calb_accuracy;
	m_enTabDlgID = ENG_CMDI_TAB::en_menu_tab_none;
	m_pDlgMenu = NULL;
}

CDlgCalbAccuracy::~CDlgCalbAccuracy()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgCalbAccuracy::DoDataExchange(CDataExchange* dx)
{
	CDlgSubMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* button - normal */
	u32StartID = IDC_CALB_ACCURACY_BTN_SUBMENU_MEASURE;
	for (i = 0; i < eCALB_ACCURACY_BTN_MAX; i++)		DDX_Control(dx, u32StartID + i, m_btn_ctl[i]);

	/* static - picture */
	u32StartID = IDC_CALB_ACCURACY_PIC_MENU;
	for (i = 0; i < eCALB_ACCURACY_PIC_MAX; i++)		DDX_Control(dx, u32StartID + i, m_pic_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgCalbAccuracy, CDlgSubMenu)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CALB_ACCURACY_BTN_SUBMENU_MEASURE, IDC_CALB_ACCURACY_BTN_SUBMENU_MEASURE, OnBtnClick)
	ON_WM_TIMER()
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgCalbAccuracy::PreTranslateMessage(MSG* msg)
{
	return CDlgSubMenu::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgCalbAccuracy::OnInitDlg()
{
	/* 컨트롤 초기화 */
	InitCtrl();

	if (!CreateMenu(IDC_CALB_ACCURACY_BTN_SUBMENU_MEASURE))	return FALSE;

	SetTimer(eCALB_ACCURACY_TIMER_START, 200, NULL);

	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgCalbAccuracy::OnExitDlg()
{
	KillTimer(eCALB_ACCURACY_TIMER_START);
	DeleteMenu();
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgCalbAccuracy::OnPaintDlg(CDC* dc)
{

	/* 자식 윈도 호출 */
	CDlgSubMenu::DrawOutLine();
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgCalbAccuracy::OnResizeDlg()
{
}

/*
 desc : 부모 스레드에 의해 주기적으로 호출됨
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgCalbAccuracy::UpdatePeriod(UINT64 tick, BOOL is_busy)
{
	m_pDlgMenu->UpdatePeriod(tick, is_busy);
	/* Control Enable or Disable */
	UpdateControl(tick, is_busy);
}

/*
 desc : 컨트롤
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgCalbAccuracy::UpdateControl(UINT64 tick, BOOL is_busy)
{
	UINT8 i = 0x00;
	/* 현재 상태 정보 메뉴 다이얼로그 ID가 아니면, 상태 버튼에는 알람 여부에 따른 색상 표현 */
	if (m_enTabDlgID != m_pDlgMenu->GetDlgID())
	{
		for (i = 0; i < eCALB_ACCURACY_BTN_MAX; i++)
		{
			m_btn_ctl[i].SetBgColor(DEF_COLOR_BTN_MENU_NORMAL);
			m_btn_ctl[i].SetTextColor(DEF_COLOR_BTN_MENU_NORMAL_TEXT);
			m_btn_ctl[i].Invalidate(TRUE);
		}

		/* 현재 선택된 메뉴 (자식) 다이얼로그 ID 저장 */
		m_enTabDlgID = m_pDlgMenu->GetDlgID();
		/* 현재 선택된 버튼에 따라 배경색 다르게 출력 */
		i = 0xff;
		switch (m_enTabDlgID)
		{
		case ENG_CMDI_TAB::en_menu_tab_calb_accuracy_measure:		i = eCALB_ACCURACY_BTN_MEASURE;		break;
		//case ENG_CMDI_TAB::en_menu_tab_calb_accuracy_check:			i = eCALB_ACCURACY_BTN_CHECK;		break;
		}
		if (i != 0xff)
		{
			//m_btn_ctl[i].SetBgColor(RGB(0x80, 0xff, 0x00));
			m_btn_ctl[i].SetBgColor(DEF_COLOR_BTN_MENU_SELECT);
			m_btn_ctl[i].Invalidate(TRUE);
		}
	}
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgCalbAccuracy::InitCtrl()
{
	CResizeUI clsResizeUI;

	/* button - normal */
	for (int i = 0; i < eCALB_ACCURACY_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
		// by sysandj : Resize UI
	}

	for (int i = 0; i < eCALB_ACCURACY_PIC_MAX; i++)
	{
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_pic_ctl[i]);
		// by sysandj : Resize UI
	}
}

/*
 desc : 객체 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgCalbAccuracy::InitObject()
{

	return TRUE;
}

/*
 desc : 객체 해제
 parm : None
 retn : None
*/
VOID CDlgCalbAccuracy::CloseObject()
{
}

/*
 desc : 일반 버튼 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgCalbAccuracy::OnBtnClick(UINT32 id)
{
	CreateMenu(id);
}

/*
 desc : 현재 실행된 자식 윈도 메모리 해제
 parm : None
 retn : None
*/
VOID CDlgCalbAccuracy::DeleteMenu()
{
	if (!m_pDlgMenu)	return;

	if (m_pDlgMenu->GetSafeHwnd())	m_pDlgMenu->DestroyWindow();
	delete m_pDlgMenu;
	m_pDlgMenu = NULL;
}

/*
 desc : 자식 메뉴 화면 생성
 parm : id	- [in]  버튼 컨트롤 ID
 retn : TRUE or FALSE
*/
BOOL CDlgCalbAccuracy::CreateMenu(UINT32 id)
{
	UINT32 u32DlgID = 0;

	/* 기존 실행된 자식 윈도가 있다면 메모리 해제 */
	DeleteMenu();

	/* 생성하고자 하는 윈도 (메뉴 다이얼로그) ID 설정 */
	u32DlgID = IDD_CALB_ACCURACY_MEASURE + (id - IDC_CALB_ACCURACY_BTN_SUBMENU_MEASURE);
	/* 해당 자식 (메뉴) 윈도 메모리 할당 */
	switch (id)
	{
	case IDC_CALB_ACCURACY_BTN_SUBMENU_MEASURE: m_pDlgMenu = new CDlgCalbAccuracyMeasure(u32DlgID, this);	break;
	}

	/* 자식 (메뉴) 윈도 생성 */
	if (!m_pDlgMenu->Create(m_pic_ctl[eCALB_ACCURACY_PIC_MENU].GetSafeHwnd()))
	{
		delete m_pDlgMenu;
		m_pDlgMenu = NULL;
		return FALSE;
	}

	return TRUE;
}


void CDlgCalbAccuracy::OnTimer(UINT_PTR nIDEvent)
{
	if (eCALB_ACCURACY_TIMER_START == nIDEvent)
	{
		UpdateControl(GetTickCount64(), FALSE);
	}

	CDlgSubMenu::OnTimer(nIDEvent);
}

