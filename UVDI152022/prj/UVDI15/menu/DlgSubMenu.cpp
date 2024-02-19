
/*
 desc : 메인 메뉴의 구성에 대한 기본 객체
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgSubMenu.h"


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
CDlgSubMenu::CDlgSubMenu(UINT32 id, CWnd* parent)
	: CMyDialog(id, parent)
{
	m_bDrawBG		= TRUE;
	m_bTooltip		= TRUE;
	m_bMagnetic		= FALSE;
	m_pDlgMain		= (CDlgMain*)parent;

	/* 멤버 변수 초기화 */
	m_enDlgID		= ENG_CMDI_SUB::en_menu_sub_none;
}

CDlgSubMenu::~CDlgSubMenu()
{
}

BEGIN_MESSAGE_MAP(CDlgSubMenu, CMyDialog)
END_MESSAGE_MAP()

/*
 desc : 모달리스 윈도 생성
 parm : h_view	- [in]  자신의 윈도 (다이얼로그)가 출력되는 영역의 핸들
 retn : DI_TRUE or DI_FALSE
*/
BOOL CDlgSubMenu::Create(HWND h_view)
{
	if (!CMyDialog::Create())	return FALSE;

	ShowWindow(SW_HIDE);

	INT32 i32Width, i32Height;
	CRect rView, rChild;
	CWnd *pView	= CWnd::FromHandle(h_view);

	/* 자신의 윈도 위치 및 크기 얻기 */
	GetClientRect(rChild);

	// by sysandj : 기존 주석처리
	//i32Width	= rChild.Width();
	//i32Height	= rChild.Height();
	// by sysandj : 기존 주석처리

	/* Output Box 컨트롤의 위치 얻기 */
	pView->GetWindowRect(rView);
	ScreenToClient(rView);

	// by sysandj : TAB 화면에 맞춤
	i32Width = rView.Width();
	i32Height = rView.Height();
	// by sysandj : TAB 화면에 맞춤

	/* 특정 위치로 이동 */
	rChild.left		= rView.left /*+ 1*/;
	rChild.top		= rView.top /*+ 1*/;
	rChild.right	= rChild.left + i32Width;
	rChild.bottom	= rChild.top + i32Height;

 	MoveWindow(rChild);
	SetParent(m_pParentWnd);	/* 부모 윈도를 지정하지 않으면 POPUP 창으로 부모 위에 위치함 */
	ShowWindow(SW_SHOW);

	return TRUE;
}

/*
 desc : 윈도 갱신될 때마다, 외곽의 사각형 라인 그리기
 parm : None
 retn : None
*/
VOID CDlgSubMenu::DrawOutLine()
{
	HDC hDC	= NULL;
	HPEN hPen, hOldPen = NULL;
	CRect rClient;

	/* 자신의 영역 크기 얻기 */
	GetClientRect(rClient);

	/* Device Context 얻기 */
	hDC		= ::GetDC(m_hWnd);
	if (!hDC)	return;
	/* Pen 생성 및 선택 */
	hPen	= ::CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
	hOldPen	= (HPEN)::SelectObject(hDC, hPen);
	/* 자신의 윈도의 가장자리에 검정색 라인 그리기 */
	::MoveToEx(hDC, 0, 0, NULL);
	::LineTo(hDC, rClient.right-1, 0);
	::LineTo(hDC, rClient.right-1, rClient.bottom-1);
	::LineTo(hDC, 0, rClient.bottom-1);
	::LineTo(hDC, 0, 0);

	/* Pen 복구 */
	if (hOldPen)::SelectObject(hDC, hOldPen);
	if (hPen)	::DeleteObject(hPen);
	/* DC 닫기 */
	if (hDC)	::ReleaseDC(m_hWnd, hDC);
}