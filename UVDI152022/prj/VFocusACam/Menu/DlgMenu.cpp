
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
 desc : ������
 parm : id		- [in]  �ڽ��� ���� ID
		parent	- [in]  �ڽ��� ȣ���� �θ� ���� Ŭ���� ������
 retn : None
*/
CDlgMenu::CDlgMenu(UINT32 id, CWnd *parent)
	: CDlgChild(id, parent)
{
	m_pDlgMain	= (CDlgMain *)m_pParentWnd;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDlgMenu::~CDlgMenu()
{
}

BEGIN_MESSAGE_MAP(CDlgMenu, CDlgChild)
END_MESSAGE_MAP()

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMenu::OnInitDlg()
{
	if (!CDlgChild::OnInitDlg())	return FALSE;

	/* ���� ��ü ȣ�� */
	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgMenu::OnExitDlg()
{
}

