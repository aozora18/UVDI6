
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
 desc : ������
 parm : id		- [in]  �ڽ��� ���� ID
		parent	- [in]  �ڽ��� ȣ���� �θ� ���� Ŭ���� ������
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
 desc : �ı���
 parm : None
 retn : None
*/
CDlgLuri::~CDlgLuri()
{
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
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
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgLuri::PreTranslateMessage(MSG* msg)
{

	return CDlgChild::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLuri::OnInitDlg()
{
	if (!CDlgChild::OnInitDlg())	return FALSE;

	/* ��Ʈ�� �ʱ�ȭ */
	if (!InitCtrl())	return FALSE;
	/* Tab Control */
	if (!InitTab())		return FALSE;

	/* ���� ��ü ȣ�� */
	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgLuri::OnExitDlg()
{
	CloseTab();
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgLuri::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgLuri::OnResizeDlg()
{
}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
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
 desc : ���� ���� ��Ʈ�� ���� ����
 parm : None
 retn : None
*/
VOID CDlgLuri::UpdateCtrl()
{
	INT32 i32SelPos		= 0;
	CDlgLuria *pDlgLuria= NULL;

	/* ���� ���õ� Tab�� �ڽ� ���� ��� */
	i32SelPos	= m_tab_ctl[0].GetCurSel();
	if (i32SelPos < 0)	return;
	/* Grid Control ���� �� ���� */
	pDlgLuria	= (CDlgLuria*)m_tab_ctl[0].GetTabWnd(i32SelPos);
	if (pDlgLuria)	pDlgLuria->UpdateCtrl();
}

/*
 desc : Tab Control�� �÷��� �ڽ� ���̾�α� ���� �� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLuri::InitTab()
{
	/* Setup the Tab Control */
	m_tab_ctl[0].SetTabWndBackColor(RGB(255, 255, 255));	/* �� ��� ���� */
	m_tab_ctl[0].SetTabBtnBackColor(RGB(255, 255, 255));	/* �� ��ư ���� */
	m_tab_ctl[0].SetNormalColor(RGB(64, 64, 64));			/* �� ���� ���� ���� */
	m_tab_ctl[0].SetSelectedColor(RGB(0, 0, 0));			/* �� ����   ���� ���� */
	m_tab_ctl[0].SetMouseOverColor(RGB(0, 32, 128));		/* �� ���콺 ���� ���� */
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

	/* ���� ���õ� ��ü ���� */
	m_tab_ctl[0].SelectTab(0);

	return TRUE;
}

/*
 desc : Tab Control�� �÷��� �ڽ� ���̾�α� ����
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
 desc : �ڽ� ���� �޸� ����
 parm : dlg	- [in]  ���̾�α� ��ü
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
