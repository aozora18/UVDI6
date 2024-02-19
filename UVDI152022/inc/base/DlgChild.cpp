
/*
 desc : 메인 메뉴의 자식 다이얼로그
*/

#include "pch.h"
#include "DlgChild.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : constructor
 parm : id		- [in]  dialog id
		parent	- [in]  parent object pointer
		magnet	- [in]  Whether the window magnet is set
 retn : None
*/
CDlgChild::CDlgChild(UINT32 dlg_id, CWnd *parent, BOOL magnet)
	: CMyDialog(dlg_id, parent)
{
	m_bDrawBG	= TRUE;
	m_bTooltip	= TRUE;
	m_bMagnet	= magnet;
}

CDlgChild::~CDlgChild()
{
}

BEGIN_MESSAGE_MAP(CDlgChild, CMyDialog)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

/*
 desc : Set the magnet operation of the window
 parm : lpwndpos	- [in]  window position
 retn : None
*/
VOID CDlgChild::OnWindowPosChanging(WINDOWPOS *lpwndpos)
{
	if (!m_bMagnet)	return;	/* Diable magnet action */

	CMyDialog::OnWindowPosChanging(lpwndpos);	/* Enable magnet action */
}

/*
 desc : When the window is running, it is initially called once
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgChild::OnInitDlg()
{

	return TRUE;
}
