
/*
 desc : Monitoring - Luria Service - Base
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgLuria.h"


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
CDlgLuria::CDlgLuria(UINT32 id, CWnd *parent)
	: CDlgChild(id, parent)
{
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDlgLuria::~CDlgLuria()
{
}

BEGIN_MESSAGE_MAP(CDlgLuria, CDlgChild)
END_MESSAGE_MAP()

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLuria::OnInitDlg()
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
VOID CDlgLuria::OnExitDlg()
{
}

