
/*
 desc : Log Files & Contents
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgConf.h"
#include "./conf/DlgConfSetting.h"
#include "./conf/DlgConfTeach.h"

#include "../mesg/DlgMesg.h"

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgConf::CDlgConf(UINT32 id, CWnd* parent)
	: CDlgMenu(id, parent)
{
	/* 자신의 다이얼로그 ID 설정 */
	m_enDlgID	= ENG_CMDI::en_menu_conf;
	m_enSubDlgID = ENG_CMDI_SUB::en_menu_sub_none;
	m_pDlgMenu = NULL;
}

CDlgConf::~CDlgConf()
{
	delete m_pDlgMenu;
		m_pDlgMenu = NULL;
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgConf::DoDataExchange(CDataExchange* dx)
{
	CDlgMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	u32StartID = IDC_CONF_SETTING_BTN;
	for (i = 0; i < eCONF_TAB_BTN_MAX; i++)	DDX_Control(dx, u32StartID + i, m_btn_Tab_ctl[i]);

	u32StartID = IDC_CONF_PIC_MENU;
	for (i = 0; i < eCONF_PIC_MAX; i++)		DDX_Control(dx, u32StartID + i, m_pic_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgConf, CDlgMenu)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_CONF_SETTING_BTN, IDC_CONF_TEACH_BTN, OnBtnClick)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgConf::PreTranslateMessage(MSG* msg)
{
	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgConf::OnInitDlg()
{
	/* 컨트롤 초기화 */
	InitCtrl();
	if (!CreateMenu(IDC_CONF_SETTING_BTN))	return FALSE;
	
	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgConf::OnExitDlg()
{
	CloseGrid();
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgConf::OnPaintDlg(CDC *dc)
{

	/* 자식 윈도 호출 */
	//CDlgMenu::DrawOutLine();
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgConf::OnResizeDlg()
{
}

/*
 desc : 부모 스레드에 의해 주기적으로 호출됨
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/

BOOL CDlgConf::CreateMenu(UINT32 id)
{
	UINT32 u32DlgID = 0;

	/* 기존 실행된 자식 윈도가 있다면 메모리 해제 */
	DeleteMenu();
	
	/* 생성하고자 하는 윈도 (메뉴 다이얼로그) ID 설정 */
	u32DlgID = IDD_CONF_SETTING + (id - IDC_CONF_SETTING_BTN);
	/* 해당 자식 (메뉴) 윈도 메모리 할당 */
	switch (id)
	{
	case IDC_CONF_SETTING_BTN:
		m_pDlgMenu = new CDlgConfSetting(u32DlgID, this);		
		m_btn_Tab_ctl[eCONF_TAB_BTN_BASIC].SetBgColor(DEF_COLOR_BTN_PAGE_SELECT);
		m_btn_Tab_ctl[eCONF_TAB_BTN_TEACH].SetBgColor(g_clrBtnColor);
		Invalidate(FALSE);
		break;
	case IDC_CONF_TEACH_BTN:		
		m_pDlgMenu = new CDlgConfTeach(u32DlgID, this);		
		m_btn_Tab_ctl[eCONF_TAB_BTN_BASIC].SetBgColor(g_clrBtnColor);
		m_btn_Tab_ctl[eCONF_TAB_BTN_TEACH].SetBgColor(DEF_COLOR_BTN_PAGE_SELECT);
		Invalidate(FALSE);
		break;
	}

	/* 자식 (메뉴) 윈도 생성 */
	if (!m_pDlgMenu->Create(m_pic_ctl[0].GetSafeHwnd()))
	{
		delete m_pDlgMenu;
		m_pDlgMenu = NULL;
		return FALSE;
	}

	return TRUE;
}

VOID CDlgConf::DeleteMenu()
{
	if (!m_pDlgMenu)	return;

	if (m_pDlgMenu->GetSafeHwnd())	m_pDlgMenu->DestroyWindow();
	delete m_pDlgMenu;
	m_pDlgMenu = NULL;
}

VOID CDlgConf::UpdatePeriod(UINT64 tick, BOOL is_busy)
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
VOID CDlgConf::UpdateControl(UINT64 tick, BOOL is_busy)
{
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgConf::InitCtrl()
{
	CResizeUI clsResizeUI;

	INT32 i = 0;

	//////추가작업 JSM
	for (i = 0; i < eCONF_TAB_BTN_MAX; i++)
	{
		m_btn_Tab_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_Tab_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_Tab_ctl[i].SetTextColor(g_clrBtnTextColor);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_Tab_ctl[i]);
		// by sysandj : Resize UI
	}

	for (int i = 0; i < eCONF_PIC_MAX; i++)
	{
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_pic_ctl[i]);
		// by sysandj : Resize UI
	}
}

/*
 desc : GridControl 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgConf::InitGrid()
{

	return TRUE;
}

/*
 desc : GridControl 해제
 parm : None
 retn : None
*/
VOID CDlgConf::CloseGrid()
{
}

/*
 desc : 객체 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgConf::InitObject()
{

	return TRUE;
}

/*
 desc : 객체 해제
 parm : None
 retn : None
*/
VOID CDlgConf::CloseObject()
{
}

/*
 desc : 일반 버튼 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgConf::OnBtnClick(UINT32 id)
{
	switch (id)
	{
	case IDC_CONF_SETTING_BTN:	
		CreateMenu(id);
		break;
	case IDC_CONF_TEACH_BTN			:	
		CreateMenu(id);
		break;
	}
}
