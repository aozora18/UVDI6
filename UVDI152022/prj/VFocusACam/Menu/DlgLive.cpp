
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
 desc : ������
 parm : id		- [in]  �ڽ��� ���� ID
		parent	- [in]  �ڽ��� ȣ���� �θ� ���� Ŭ���� ������
 retn : None
*/
CDlgLive::CDlgLive(UINT32 id,CWnd* parent)
	: CDlgMenu(id, parent)
{
	m_bLiveEnable	= FALSE;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDlgLive::~CDlgLive()
{
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
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
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgLive::PreTranslateMessage(MSG* msg)
{

	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLive::OnInitDlg()
{
	if (!CDlgMenu::OnInitDlg())	return FALSE;

	/* ��Ʈ�� �ʱ�ȭ */
	InitCtrl();

	/* ���� ��ü ȣ�� */
	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgLive::OnExitDlg()
{
	CDlgMenu::OnExitDlg();

}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgLive::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgLive::OnResizeDlg()
{
}

/*
 desc : ������ ������ �ʴ��� / ���������� �̺�Ʈ ����
 parm : show	- [in]  ����/���� ���� �÷���
		status	- [in]  Specifies the status of the window being shown
						It is 0 if the message is sent because of a ShowWindow member function call; otherwise nStatus is one of the following:
						SW_PARENTCLOSING : Parent window is closing (being made iconic) or a pop-up window is being hidden.
						SW_PARENTOPENING : Parent window is opening (being displayed) or a pop-up window is being shown.
 retn : None
*/
VOID CDlgLive::OnShowWindow(BOOL show, UINT32 status)
{
	/* ������ �������� ��쿡�� �� ���� */
	if (!show)	m_bLiveEnable	= show;
}

/*
 desc : ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgLive::InitCtrl()
{
}

/*
 desc : ���� ���� ��Ʈ�� ���� ����
 parm : None
 retn : None
*/
VOID CDlgLive::UpdateCtrl()
{
	if (m_bLiveEnable)	InvalidateView();
}

/*
 desc : �̹��� �� ���� ����
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

	/* �̹����� ��µ� ���� ���� */
	m_pic_ctl[0].GetClientRect(&rDraw);
	hDC	= ::GetDC(m_pic_ctl[0].m_hWnd);
	if (hDC)
	{
		uvEng_Camera_DrawLiveBitmap(hDC, rDraw, m_pDlgMain->GetCheckACam());
		::ReleaseDC(m_pic_ctl[0].m_hWnd, hDC);
	}
}
