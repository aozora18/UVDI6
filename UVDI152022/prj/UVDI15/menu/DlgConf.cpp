
/*
 desc : Log Files & Contents
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgConf.h"
#include "./conf/DlgConfSetting.h"
#include "./conf/DlgConfTeach.h"

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
CDlgConf::CDlgConf(UINT32 id, CWnd* parent)
	: CDlgMenu(id, parent)
{
	/* �ڽ��� ���̾�α� ID ���� */
	m_enDlgID	= ENG_CMDI::en_menu_conf;
	m_enSubDlgID = ENG_CMDI_SUB::en_menu_sub_none;
	m_pDlgMenu = NULL;
}

CDlgConf::~CDlgConf()
{
	delete m_pDlgMenu;
		m_pDlgMenu = NULL;
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgConf::DoDataExchange(CDataExchange* dx)
{
	CDlgMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	u32StartID = IDC_CONF_SETTING_BTN;
	for (i = 0; i < eCONF_TAB_BTN_MAX; i++)	DDX_Control(dx, u32StartID + i, m_btn_Tab_ctl[i]);

	u32StartID = IDC_CONF_PIC_MENU;
	for (i = 0; i < eCONF_PIC_MAX; i++)		DDX_Control(dx, u32StartID + i, m_pic_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgConf, CDlgMenu)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_CONF_SETTING_BTN, IDC_CONF_TEACH_BTN, OnBtnClick)
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgConf::PreTranslateMessage(MSG* msg)
{
	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgConf::OnInitDlg()
{
	/* ��Ʈ�� �ʱ�ȭ */
	InitCtrl();
	if (!CreateMenu(IDC_CONF_SETTING_BTN))	return FALSE;
	
	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgConf::OnExitDlg()
{
	CloseGrid();
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgConf::OnPaintDlg(CDC *dc)
{

	/* �ڽ� ���� ȣ�� */
	//CDlgMenu::DrawOutLine();
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgConf::OnResizeDlg()
{
}

/*
 desc : �θ� �����忡 ���� �ֱ������� ȣ���
 parm : tick	- [in]  ���� CPU �ð�
		is_busy	- [in]  TRUE: ���� �ó����� ���� �� ..., FALSE: ���� Idle ����
 retn : None
*/

BOOL CDlgConf::CreateMenu(UINT32 id)
{
	UINT32 u32DlgID = 0;

	/* ���� ����� �ڽ� ������ �ִٸ� �޸� ���� */
	DeleteMenu();
	
	/* �����ϰ��� �ϴ� ���� (�޴� ���̾�α�) ID ���� */
	u32DlgID = IDD_CONF_SETTING + (id - IDC_CONF_SETTING_BTN);
	/* �ش� �ڽ� (�޴�) ���� �޸� �Ҵ� */
	switch (id)
	{
	case IDC_CONF_SETTING_BTN:
		m_pDlgMenu = new CDlgConfSetting(u32DlgID, this);		
		m_btn_Tab_ctl[eCONF_TAB_BTN_BASIC].SetBgColor(DEF_COLOR_BTN_PAGE_SELECT);
		m_btn_Tab_ctl[eCONF_TAB_BTN_TEACH].SetBgColor(g_clrBtnColor);
		Invalidate(FALSE);
		break;
	case IDC_CONF_TEACH_BTN:		
		m_pDlgMenu = new CDlgConfTeach(u32DlgID, this);		
		m_btn_Tab_ctl[eCONF_TAB_BTN_BASIC].SetBgColor(g_clrBtnColor);
		m_btn_Tab_ctl[eCONF_TAB_BTN_TEACH].SetBgColor(DEF_COLOR_BTN_PAGE_SELECT);
		Invalidate(FALSE);
		break;
	}

	/* �ڽ� (�޴�) ���� ���� */
	if (!m_pDlgMenu->Create(m_pic_ctl[0].GetSafeHwnd()))
	{
		delete m_pDlgMenu;
		m_pDlgMenu = NULL;
		return FALSE;
	}

	return TRUE;
}

VOID CDlgConf::DeleteMenu()
{
	if (!m_pDlgMenu)	return;

	if (m_pDlgMenu->GetSafeHwnd())	m_pDlgMenu->DestroyWindow();
	delete m_pDlgMenu;
	m_pDlgMenu = NULL;
}

VOID CDlgConf::UpdatePeriod(UINT64 tick, BOOL is_busy)
{

	/* Control Enable or Disable */
	UpdateControl(tick, is_busy);
}

/*
 desc : ��Ʈ��
 parm : tick	- [in]  ���� CPU �ð�
		is_busy	- [in]  TRUE: ���� �ó����� ���� �� ..., FALSE: ���� Idle ����
 retn : None
*/
VOID CDlgConf::UpdateControl(UINT64 tick, BOOL is_busy)
{
}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgConf::InitCtrl()
{
	CResizeUI clsResizeUI;

	INT32 i = 0;

	//////�߰��۾� JSM
	for (i = 0; i < eCONF_TAB_BTN_MAX; i++)
	{
		m_btn_Tab_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_Tab_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_Tab_ctl[i].SetTextColor(g_clrBtnTextColor);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_Tab_ctl[i]);
		// by sysandj : Resize UI
	}

	for (int i = 0; i < eCONF_PIC_MAX; i++)
	{
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_pic_ctl[i]);
		// by sysandj : Resize UI
	}
}

/*
 desc : GridControl �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgConf::InitGrid()
{

	return TRUE;
}

/*
 desc : GridControl ����
 parm : None
 retn : None
*/
VOID CDlgConf::CloseGrid()
{
}

/*
 desc : ��ü �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgConf::InitObject()
{

	return TRUE;
}

/*
 desc : ��ü ����
 parm : None
 retn : None
*/
VOID CDlgConf::CloseObject()
{
}

/*
 desc : �Ϲ� ��ư Ŭ���� ���
 parm : id	- [in]  �Ϲ� ��ư ID
 retn : None
*/
VOID CDlgConf::OnBtnClick(UINT32 id)
{
	switch (id)
	{
	case IDC_CONF_SETTING_BTN:	
		CreateMenu(id);
		break;
	case IDC_CONF_TEACH_BTN			:	
		CreateMenu(id);
		break;
	}
}
