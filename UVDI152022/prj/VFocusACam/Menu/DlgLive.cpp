
/*
 desc : Align Camera - Live View
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgLive.h"


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
CDlgLive::CDlgLive(UINT32 id,CWnd* parent)
	: CDlgMenu(id, parent)
{
	m_bLiveEnable	= FALSE;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgLive::~CDlgLive()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgLive::DoDataExchange(CDataExchange* dx)
{
	CDlgMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Picture */
	u32StartID	= IDC_LIVE_PIC_VIEW;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_pic_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgLive, CDlgMenu)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgLive::PreTranslateMessage(MSG* msg)
{

	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLive::OnInitDlg()
{
	if (!CDlgMenu::OnInitDlg())	return FALSE;

	/* 컨트롤 초기화 */
	InitCtrl();

	/* 상위 객체 호출 */
	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgLive::OnExitDlg()
{
	CDlgMenu::OnExitDlg();

}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgLive::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgLive::OnResizeDlg()
{
}

/*
 desc : 윈도가 보이지 않는지 / 보여지는지 이벤트 감지
 parm : show	- [in]  숨김/보임 여부 플래그
		status	- [in]  Specifies the status of the window being shown
						It is 0 if the message is sent because of a ShowWindow member function call; otherwise nStatus is one of the following:
						SW_PARENTCLOSING : Parent window is closing (being made iconic) or a pop-up window is being hidden.
						SW_PARENTOPENING : Parent window is opening (being displayed) or a pop-up window is being shown.
 retn : None
*/
VOID CDlgLive::OnShowWindow(BOOL show, UINT32 status)
{
	/* 무조건 숨겨지는 경우에만 값 설정 */
	if (!show)	m_bLiveEnable	= show;
}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgLive::InitCtrl()
{
}

/*
 desc : 현재 윈도 컨트롤 영역 갱신
 parm : None
 retn : None
*/
VOID CDlgLive::UpdateCtrl()
{
	if (m_bLiveEnable)	InvalidateView();
}

/*
 desc : 이미지 뷰 영역 갱신
 parm : None
 retn : None
*/
VOID CDlgLive::InvalidateView()
{
}

/*
 desc : Updates the live image
 parm : None
 retn : None
*/
VOID CDlgLive::UpdateLiveView()
{
	HDC hDC	= NULL;
	RECT rDraw;

	/* 이미지가 출력될 윈도 영역 */
	m_pic_ctl[0].GetClientRect(&rDraw);
	hDC	= ::GetDC(m_pic_ctl[0].m_hWnd);
	if (hDC)
	{
		uvEng_Camera_DrawLiveBitmap(hDC, rDraw, m_pDlgMain->GetCheckACam());
		::ReleaseDC(m_pic_ctl[0].m_hWnd, hDC);
	}
}
