
/*
 desc : ���� �޴��� ������ ���� �⺻ ��ü
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
 desc : ������
 parm : id		- [in]  �ڽ��� ���� ID
		parent	- [in]  �ڽ��� ȣ���� �θ� ���� Ŭ���� ������
 retn : None
*/
CDlgSubMenu::CDlgSubMenu(UINT32 id, CWnd* parent)
	: CMyDialog(id, parent)
{
	m_bDrawBG		= TRUE;
	m_bTooltip		= TRUE;
	m_bMagnetic		= FALSE;
	m_pDlgMain		= (CDlgMain*)parent;

	/* ��� ���� �ʱ�ȭ */
	m_enDlgID		= ENG_CMDI_SUB::en_menu_sub_none;
}

CDlgSubMenu::~CDlgSubMenu()
{
}

BEGIN_MESSAGE_MAP(CDlgSubMenu, CMyDialog)
END_MESSAGE_MAP()

/*
 desc : ��޸��� ���� ����
 parm : h_view	- [in]  �ڽ��� ���� (���̾�α�)�� ��µǴ� ������ �ڵ�
 retn : DI_TRUE or DI_FALSE
*/
BOOL CDlgSubMenu::Create(HWND h_view)
{
	if (!CMyDialog::Create())	return FALSE;

	ShowWindow(SW_HIDE);

	INT32 i32Width, i32Height;
	CRect rView, rChild;
	CWnd *pView	= CWnd::FromHandle(h_view);

	/* �ڽ��� ���� ��ġ �� ũ�� ��� */
	GetClientRect(rChild);

	// by sysandj : ���� �ּ�ó��
	//i32Width	= rChild.Width();
	//i32Height	= rChild.Height();
	// by sysandj : ���� �ּ�ó��

	/* Output Box ��Ʈ���� ��ġ ��� */
	pView->GetWindowRect(rView);
	ScreenToClient(rView);

	// by sysandj : TAB ȭ�鿡 ����
	i32Width = rView.Width();
	i32Height = rView.Height();
	// by sysandj : TAB ȭ�鿡 ����

	/* Ư�� ��ġ�� �̵� */
	rChild.left		= rView.left /*+ 1*/;
	rChild.top		= rView.top /*+ 1*/;
	rChild.right	= rChild.left + i32Width;
	rChild.bottom	= rChild.top + i32Height;

 	MoveWindow(rChild);
	SetParent(m_pParentWnd);	/* �θ� ������ �������� ������ POPUP â���� �θ� ���� ��ġ�� */
	ShowWindow(SW_SHOW);

	return TRUE;
}

/*
 desc : ���� ���ŵ� ������, �ܰ��� �簢�� ���� �׸���
 parm : None
 retn : None
*/
VOID CDlgSubMenu::DrawOutLine()
{
	HDC hDC	= NULL;
	HPEN hPen, hOldPen = NULL;
	CRect rClient;

	/* �ڽ��� ���� ũ�� ��� */
	GetClientRect(rClient);

	/* Device Context ��� */
	hDC		= ::GetDC(m_hWnd);
	if (!hDC)	return;
	/* Pen ���� �� ���� */
	hPen	= ::CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
	hOldPen	= (HPEN)::SelectObject(hDC, hPen);
	/* �ڽ��� ������ �����ڸ��� ������ ���� �׸��� */
	::MoveToEx(hDC, 0, 0, NULL);
	::LineTo(hDC, rClient.right-1, 0);
	::LineTo(hDC, rClient.right-1, rClient.bottom-1);
	::LineTo(hDC, 0, rClient.bottom-1);
	::LineTo(hDC, 0, 0);

	/* Pen ���� */
	if (hOldPen)::SelectObject(hDC, hOldPen);
	if (hPen)	::DeleteObject(hPen);
	/* DC �ݱ� */
	if (hDC)	::ReleaseDC(m_hWnd, hDC);
}