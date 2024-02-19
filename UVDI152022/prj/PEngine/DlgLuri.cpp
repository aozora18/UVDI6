
/*
 desc : Monitoring - Luria Service
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgLuri.h"


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
CDlgLuri::CDlgLuri(UINT32 id, CWnd *parent)
	: CDlgChild(id, parent)
{
	m_pDlgMC	= NULL;
	m_pDlgJM	= NULL;
	m_pDlgPD	= NULL;
	m_pDlgEP	= NULL;
	m_pDlgDP	= NULL;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgLuri::~CDlgLuri()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgLuri::DoDataExchange(CDataExchange* dx)
{
	CDlgChild::DoDataExchange(dx);

	UINT32 i, u32StartID;

	u32StartID	= IDC_LURI_TAB_BASE;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_tab_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgLuri, CDlgChild)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgLuri::PreTranslateMessage(MSG* msg)
{

	return CDlgChild::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLuri::OnInitDlg()
{
	if (!CDlgChild::OnInitDlg())	return FALSE;

	/* 컨트롤 초기화 */
	if (!InitCtrl())	return FALSE;
	/* Tab Control */
	if (!InitTab())		return FALSE;

	/* 상위 객체 호출 */
	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgLuri::OnExitDlg()
{
	CloseTab();
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgLuri::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgLuri::OnResizeDlg()
{
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLuri::InitCtrl()
{
	INT32 i;

	/* Tab Control */
	for (i=0; i<1; i++)
	{
		m_tab_ctl[i].SetFontInfo(&g_lf);
	}

	return TRUE;
}

/*
 desc : 현재 윈도 컨트롤 영역 갱신
 parm : None
 retn : None
*/
VOID CDlgLuri::UpdateCtrl()
{
	INT32 i32SelPos		= 0;
	CDlgLuria *pDlgLuria= NULL;

	/* 현재 선택된 Tab의 자식 윈도 얻기 */
	i32SelPos	= m_tab_ctl[0].GetCurSel();
	if (i32SelPos < 0)	return;
	/* Grid Control 내에 값 갱신 */
	pDlgLuria	= (CDlgLuria*)m_tab_ctl[0].GetTabWnd(i32SelPos);
	if (pDlgLuria)	pDlgLuria->UpdateCtrl();
}

/*
 desc : Tab Control에 올려질 자식 다이얼로그 생성 및 연결
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLuri::InitTab()
{
	/* Setup the Tab Control */
	m_tab_ctl[0].SetTabWndBackColor(RGB(255, 255, 255));	/* 탭 배경 색상 */
	m_tab_ctl[0].SetTabBtnBackColor(RGB(255, 255, 255));	/* 탭 버튼 색상 */
	m_tab_ctl[0].SetNormalColor(RGB(64, 64, 64));			/* 탭 비선택 글자 색상 */
	m_tab_ctl[0].SetSelectedColor(RGB(0, 0, 0));			/* 탭 선택   글자 색상 */
	m_tab_ctl[0].SetMouseOverColor(RGB(0, 32, 128));		/* 탭 마우스 글자 색상 */
	m_tab_ctl[0].SetTopLeftCorner(CPoint(18, 45));
	m_tab_ctl[0].SetTabSize(100, 25);
	m_tab_ctl[0].SetMinTabWidth(50);

	/* Machine Config */
	m_pDlgMC	= new CDlgLuMC(IDD_LUMC, this);
	m_pDlgMC->Create(IDD_LUMC);
	m_tab_ctl[0].AddTab(m_pDlgMC, L"Machine Config");
	/* Job Management */
	m_pDlgJM	= new CDlgLuJM(IDD_LUJM, this);
	m_pDlgJM->Create(IDD_LUJM);
	m_tab_ctl[0].AddTab(m_pDlgJM, L"Job Management");
	/* Panel Data */
	m_pDlgPD	= new CDlgLuPD(IDD_LUPD, this);
	m_pDlgPD->Create(IDD_LUPD);
	m_tab_ctl[0].AddTab(m_pDlgPD, L"Panel Data");
	/* Exposure */
	m_pDlgEP	= new CDlgLuEP(IDD_LUEP, this);
	m_pDlgEP->Create(IDD_LUEP);
	m_tab_ctl[0].AddTab(m_pDlgEP, L"Exposure");
	/* Direct Photohead */
	m_pDlgDP	= new CDlgLuDP(IDD_LUDP, this);
	m_pDlgDP->Create(IDD_LUDP);
	m_tab_ctl[0].AddTab(m_pDlgDP, L"Direct PHs");

	/* 현재 선택된 객체 연결 */
	m_tab_ctl[0].SelectTab(0);

	return TRUE;
}

/*
 desc : Tab Control에 올려진 자식 다이얼로그 해제
 parm : None
 retn : None
*/
VOID CDlgLuri::CloseTab()
{
	m_tab_ctl[0].DeleteAllTabs();
	KillChild(m_pDlgMC);
	KillChild(m_pDlgJM);
	KillChild(m_pDlgPD);
	KillChild(m_pDlgEP);
	KillChild(m_pDlgDP);
}

/*
 desc : 자식 윈도 메모리 해제
 parm : dlg	- [in]  다이얼로그 객체
 retn : None
*/
VOID CDlgLuri::KillChild(CDlgLuria *dlg)
{
	if (dlg)
	{
		if (dlg->GetSafeHwnd())	dlg->DestroyWindow();
		delete dlg;
	}
}
