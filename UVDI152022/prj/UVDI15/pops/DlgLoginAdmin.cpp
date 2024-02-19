
/*
 desc : �뱤 �ݺ� Ƚ�� ���� ��ȭ ����
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgLoginAdmin.h"

#include "../mesg/DlgMesg.h"
#include "../db/DBMgr.h"
#include "../db/DBDefine.h"


#if USE_KEYBOARD_TYPE_TOUCH
#include "../../../inc/kybd/DlgKBDT.h"
#include "../../../inc/kybd/DlgKBDN.h"
#endif

#define DEF_DEFAULT_GRID_ROW_SIZE		40

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif

//IMPLEMENT_DYNAMIC(CDlgIdAdmin, CDialogEx)

/*
 desc : ������
 parm : id		- [in]  �ڽ��� ���� ID
		parent	- [in]  �ڽ��� ȣ���� �θ� ���� Ŭ���� ������
 retn : None
*/
CDlgLoginAdmin::CDlgLoginAdmin(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgLoginAdmin::IDD, parent)
{
	m_bDrawBG		= TRUE;
	m_bTooltip		= TRUE;
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgLoginAdmin::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Title - Normal */
	u32StartID	= IDC_LOGIN_ADMIN_TTL_ADMIN;
	for (i=0; i< eADMIN_TTL_MAX; i++)		DDX_Control(dx, u32StartID+i, m_ttl_ctl[i]);
	/* Static - Normal */
	u32StartID	= IDC_LOGIN_ADMIN_STT_ID;
	for (i=0; i< eADMIN_STT_MAX; i++)		DDX_Control(dx, u32StartID+i, m_stt_ctl[i]);
	/* Text - Normal */
	u32StartID	= IDC_LOGIN_ADMIN_TXT_ID;
	for (i=0; i< eADMIN_TXT_MAX; i++)		DDX_Control(dx, u32StartID+i, m_txt_ctl[i]);
	/* Button - Normal */
	u32StartID	= IDC_LOGIN_ADMIN_BTN_CANCEL;
	for (i=0; i< eADMIN_BTN_MAX; i++)		DDX_Control(dx, u32StartID+i, m_btn_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgLoginAdmin, CMyDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_LOGIN_ADMIN_BTN_CANCEL, IDC_LOGIN_ADMIN_BTN_CANCEL + eADMIN_BTN_MAX, OnBtnClicked)
	ON_CONTROL_RANGE(STN_CLICKED, IDC_LOGIN_ADMIN_TXT_ID, IDC_LOGIN_ADMIN_TXT_ID + eADMIN_TXT_MAX, OnTxtClicked)
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä��
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgLoginAdmin::PreTranslateMessage(MSG* msg)
{

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLoginAdmin::OnInitDlg()
{
	CResizeUI clsResizeUI;
	clsResizeUI.ResizeControl(this, this);

	/* ��Ʈ�� �ʱ�ȭ */
	if (!InitCtrl())	return FALSE;

	/* TOP_MOST & Center */
	CenterParentTopMost();
	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgLoginAdmin::OnExitDlg()
{
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgLoginAdmin::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgLoginAdmin::OnResizeDlg()
{
}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLoginAdmin::InitCtrl()
{
	CResizeUI clsResizeUI;

	INT32 i = 0;
	PTCHAR pText	= NULL;

	/* title - normal */
	for (i = 0; i < eADMIN_TTL_MAX; i++)
	{
		m_ttl_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
//		m_ttl_ctl[i].SetDrawBg(1);
		m_ttl_ctl[i].SetBaseProp(0, 1, 0, 0, FORD_GRAPHITE, 0, WHITE_);
		clsResizeUI.ResizeControl(this, &m_ttl_ctl[i]);
	}

	/* static - normal */
	for (i = 0; i < eADMIN_STT_MAX; i++)
	{
		m_stt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_stt_ctl[i].SetDrawBg(1);
		m_stt_ctl[i].SetBaseProp(0, 1, 0, 0, DEF_COLOR_BTN_MENU_NORMAL, 0, WHITE_);
		clsResizeUI.ResizeControl(this, &m_stt_ctl[i]);
	}

	/* static - text */
	for (i = 0; i < eADMIN_TXT_MAX; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetNotify();		//	set	click event
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, WHITE_, 0, WHITE_);
		clsResizeUI.ResizeControl(this, &m_txt_ctl[i]);
	}

	/* button - normal */
	for (i = 0; i< eADMIN_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);

		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
	}

	return TRUE;
}

void CDlgLoginAdmin::ConvertToAsterisks(TCHAR* str)
{
	//	�Էµ� ���ڸ� ��� ��ǥ(*)�� ��ȯ
	if (str == NULL)
		return;

	while (*str) {
		*str = _T('*');
		str++;
	}
}

BOOL CDlgLoginAdmin::IsInputID()
{
	CDlgMesg dlgMesg;
	CString str;

	m_txt_ctl[eADMIN_TXT_ID].GetWindowTextW(str);
	if (_T("") == str)
	{
		dlgMesg.MyDoModal(L"���Է�! \n������ ID�� �Է����ּ���.", 0x01);
		return FALSE;
	}

	return TRUE;
}

BOOL CDlgLoginAdmin::IsInputPassword()
{
	CDlgMesg dlgMesg;
	CString str;

	m_txt_ctl[eADMIN_TXT_PASSWORD].GetWindowTextW(str);
	if (_T("") == str)
	{
		dlgMesg.MyDoModal(L"���Է�! \n [ PASSWORD ] �� �Է����ּ���.", 0x01);
		return FALSE;
	}

	return TRUE;
}

BOOL CDlgLoginAdmin::IsEnableLogin(CString strID, CString strPwd)
{
	CDlgMesg dlgMesg;
	std::vector < ST_DB_LOGIN > vLoginInfo = CDBMgr::GetInstance()->GetLoginInfo();

	for (auto login : vLoginInfo)
	{
		if (login.strID == strID)
		{
			if (login.strLevel != _T("ADMIN"))
			{
				dlgMesg.MyDoModal(L"���� ����! \n������ ����� ID�� �ƴմϴ�. �ٽ� �Է����ּ���.", 0x01);
				return FALSE;
			}
			else if (login.strPwd != strPwd)
			{
				dlgMesg.MyDoModal(L"���� ����! \n��й�ȣ�� Ʋ�Ƚ��ϴ�. �ٽ� �Է����ּ���.", 0x01);
				return FALSE;
			}
			else
			{
				//	���� ����
				return TRUE;
			}
		}		
	}

	dlgMesg.MyDoModal(L"���� ����! \n��ϵ��� ���� ������ ID �Դϴ�. �ٽ� �Է����ּ���.", 0x01);

	return FALSE;
}

BOOL CDlgLoginAdmin::IsEmptyLoginInfo()
{
	std::vector < ST_DB_LOGIN > vLoginInfo = CDBMgr::GetInstance()->GetLoginInfo();

	if (vLoginInfo.size() == 0)
	{
		return TRUE;
	}

	return FALSE;
}

/*
 desc : ���� ��ư �̺�Ʈ
 parm : None
 retn : None
*/
VOID CDlgLoginAdmin::OnBtnClicked(UINT32 id)
{
	CDlgMesg dlgMesg;

	if (id == IDC_LOGIN_ADMIN_BTN_LOGIN)
	{
		if(!IsEmptyLoginInfo())
		{
			if (!IsInputID()) return;
			if (!IsInputPassword()) return;
			if (!IsEnableLogin(m_strID, m_strPwd)) return;
		}

		m_strID = _T("");
		m_strPwd = _T("");
		m_txt_ctl[eADMIN_TXT_ID].SetWindowTextW(_T(""));
		m_txt_ctl[eADMIN_TXT_PASSWORD].SetWindowTextW(_T(""));

		/* ID ���� ȭ�� ȣ�� */
		::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), eMSG_MAIN_MANAGER_CONSOLE, NULL, NULL);

		CMyDialog::EndDialog(IDOK);
	}
	else
	{
		CMyDialog::EndDialog(IDCANCEL);
	}
}

VOID CDlgLoginAdmin::OnTxtClicked(UINT32 id)
{
	CDlgKBDT dlg;
	CString	str;
	TCHAR tzText[1024] = { NULL };

	switch (id)
	{
	case IDC_LOGIN_ADMIN_TXT_ID:
		if (IDOK == dlg.MyDoModal(32))
		{
			dlg.GetText(tzText, 1024);
			m_strID = tzText;

			m_txt_ctl[eADMIN_TXT_ID].SetWindowTextW(tzText);
		}
		break;

	case IDC_LOGIN_ADMIN_TXT_PASSWORD:
		//	ID �Է� Ȯ��
		if (!IsInputID()) return;

		//	PASSWORD �Է�
		if (IDOK == dlg.MyDoModal(32, TRUE))
		{
			dlg.GetText(tzText, 1024);
			m_strPwd = tzText;

			ConvertToAsterisks(tzText);
			m_txt_ctl[eADMIN_TXT_PASSWORD].SetWindowTextW(tzText);
		}
		break;
	}
}

/*
 desc : DoModal Override �Լ�
 parm : None
 retn : 0L
*/
INT_PTR CDlgLoginAdmin::MyDoModal()
{
	return DoModal();
}
