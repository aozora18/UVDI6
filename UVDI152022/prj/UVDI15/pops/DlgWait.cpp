
/*
 desc : ���α׷� ����� ��, ��� �˸�
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgWait.h"
#include "../mesg/DlgMesg.h"


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
CDlgWait::CDlgWait(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgWait::IDD, parent)
{
	m_bDrawBG	= TRUE;
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgWait::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	DDX_Control(dx, IDC_WAIT_TXT_ANI, m_csWaitAni);
}

BEGIN_MESSAGE_MAP(CDlgWait, CMyDialog)
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgWait::PreTranslateMessage(MSG* msg)
{

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgWait::OnInitDlg()
{
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	CDlgMesg dlgMesg;

	/* Ÿ��Ʋ �̹��� ���� */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\ui_img\\logo\\wait.jpg", g_tzWorkDir);
	if (!m_csWaitPic.Load(tzFile))
	{
		dlgMesg.MyDoModal(L"Failed to load the image (wait.jpg)", 0x01);
	}

	/* �ִϸ��̼� �̹��� ���� */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\ui_img\\ani\\shutdown_wait.gif", g_tzWorkDir);
	if (!m_csWaitAni.Load(tzFile, FALSE))
	{
		dlgMesg.MyDoModal(L"Failed to load the image (shutdown_wait.gif)", 0x01);
	}
	m_csWaitAni.Play();

	/* TOP_MOST & Center */
	CenterParentTopMost();

	return TRUE;
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgWait::OnPaintDlg(CDC *dc)
{
	if (m_csWaitPic.IsDrawImage())
	{
		CRect rDraw;
		GetClientRect(rDraw);
		rDraw.left	+= 13;
		rDraw.top	+= 13;
		rDraw.right	= rDraw.left + m_csWaitPic.GetWidth();
		rDraw.bottom= rDraw.top + m_csWaitPic.GetHeight();
		m_csWaitPic.Show(dc, rDraw);
	}
}