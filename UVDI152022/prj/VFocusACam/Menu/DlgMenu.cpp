
/*
 desc : Result - Base
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgMenu.h"


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
CDlgMenu::CDlgMenu(UINT32 id, CWnd *parent)
	: CDlgChild(id, parent)
{
	m_pDlgMain	= (CDlgMain *)m_pParentWnd;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgMenu::~CDlgMenu()
{
}

BEGIN_MESSAGE_MAP(CDlgMenu, CDlgChild)
END_MESSAGE_MAP()

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMenu::OnInitDlg()
{
	if (!CDlgChild::OnInitDlg())	return FALSE;

	/* 상위 객체 호출 */
	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgMenu::OnExitDlg()
{
}

