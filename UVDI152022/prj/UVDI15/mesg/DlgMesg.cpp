
/*
 desc : �޽��� �ڽ�
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgMesg.h"

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
CDlgMesg::CDlgMesg(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgMesg::IDD, parent)
{
	m_bDrawBG	= TRUE;
	m_bTooltip	= TRUE;

	wmemset(m_tzApply, 0x00, 128);
	wmemset(m_tzCancel, 0x00, 128);
	wmemset(m_tzMiddle, 0x00, 128);
	wmemset(m_tzQuery, 0x00, 1024);
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgMesg::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Group - Normal */
	u32StartID	= IDC_MESG_TXT_MESG;
	for (i=0; i< eMESG_TXT_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_txt_msg[i]);
	/* Button - Normal */
	u32StartID	= IDC_MESG_BTN_APPLY;
	for (i=0; i< eMESG_BTN_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgMesg, CMyDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_MESG_BTN_APPLY, IDC_MESG_BTN_MIDDLE, OnBtnClicked)
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgMesg::PreTranslateMessage(MSG* msg)
{

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMesg::OnInitDlg()
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
VOID CDlgMesg::OnExitDlg()
{
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgMesg::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgMesg::OnResizeDlg()
{
}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMesg::InitCtrl()
{
	// by sysandj : Resize UI
	CResizeUI clsResizeUI;
	// by sysandj : Resize UI

	INT32 i = 0;
	PTCHAR pText	= NULL;

	/* button - normal */
	for (i=0; i< eMESG_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]); 
		// by sysandj : Resize UI
	}
	/* text - normal */
	for (i=0; i< eMESG_TXT_MAX; i++)
	{
		m_txt_msg[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_txt_msg[i].SetDrawBg(1);
		m_txt_msg[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_txt_msg[i]);
		// by sysandj : Resize UI
	}

	/* ���� �޽��� �Է� */
	m_txt_msg[eMESG_TXT_MESG].SetWindowTextW(m_tzQuery);

	/* ��ư ���Ŀ� ���� */
	if (0x01 == m_u8BtnType)
	{
		m_btn_ctl[eMESG_BTN_APPLY].ShowWindow(SW_HIDE);
		m_btn_ctl[eMESG_BTN_CANCEL].ShowWindow(SW_SHOW);
		m_btn_ctl[eMESG_BTN_MIDDLE].ShowWindow(SW_HIDE);
		m_btn_ctl[eMESG_BTN_CANCEL].SetWindowTextW(L"Close");
		SetWindowTextW(L"Information");
	}
	else if (0x02 == m_u8BtnType)
	{
		m_btn_ctl[eMESG_BTN_APPLY].ShowWindow(SW_SHOW);
		m_btn_ctl[eMESG_BTN_CANCEL].ShowWindow(SW_SHOW);
		m_btn_ctl[eMESG_BTN_CANCEL].ShowWindow(SW_HIDE);
		m_btn_ctl[eMESG_BTN_APPLY].SetWindowTextW(m_tzApply);
		m_btn_ctl[eMESG_BTN_CANCEL].SetWindowTextW(m_tzCancel);
	}
	else if (0x03 == m_u8BtnType)
	{
		m_btn_ctl[eMESG_BTN_APPLY].SetWindowTextW(m_tzApply);
		m_btn_ctl[eMESG_BTN_CANCEL].SetWindowTextW(m_tzCancel);
		m_btn_ctl[eMESG_BTN_MIDDLE].SetWindowTextW(m_tzMiddle);
	}
	else
	{
		m_btn_ctl[eMESG_BTN_APPLY].SetWindowTextW(m_tzApply);
		m_btn_ctl[eMESG_BTN_CANCEL].SetWindowTextW(m_tzCancel);
		m_btn_ctl[eMESG_BTN_MIDDLE].SetWindowTextW(m_tzMiddle);
		m_btn_ctl[eMESG_BTN_MIDDLE].ShowWindow(SW_HIDE);
		SetWindowTextW(L"Question ?");
	}


	return TRUE;
}

/*
 desc : ���� �߻��� ���� ���� ���
 parm : None
 retn : None
*/
VOID CDlgMesg::OnBtnClicked(UINT32 id)
{
	if (id == IDC_MESG_BTN_APPLY)		CMyDialog::EndDialog(IDOK);
	else if (id == IDC_MESG_BTN_MIDDLE)	CMyDialog::EndDialog(IDIGNORE);
	else								CMyDialog::EndDialog(IDCANCEL);
}

/*
 desc : DoModal Override �Լ�
 parm : mesg	- [in]  ���̾�α� ���ڿ� ��µǴ� �޽���
		type	- [in]  0x00 : ��ư 2��, 0x01 : ��ư 1��, 0x02 : ��ư 2�� (Yes, No), 0x03 : ��ư 2�� (Edit Thick, Cancel)
 retn : 0L
*/
INT_PTR CDlgMesg::MyDoModal(PTCHAR mesg, UINT8 type)
{
	if (!mesg || wcslen(mesg) < 1)	return -1;
	
	wmemset(m_tzQuery, 0x00, 1024);
	swprintf_s(m_tzQuery, 1024, L"%s", mesg);
	m_u8BtnType	= type;

	if (type != 0x01)
	{
		wmemset(m_tzApply, 0x00, 128);
		wmemset(m_tzCancel, 0x00, 128);
		if (0x00 == type)
		{
			wcscpy_s(m_tzApply, 128, L"Apply");
			wcscpy_s(m_tzCancel, 128, L"Cancel");
		}
		else if (0x02 == type)
		{
			wcscpy_s(m_tzApply, 128, L"Yes");
			wcscpy_s(m_tzCancel, 128, L"No");
		}
		else if (0x03 == type)
		{
			wcscpy_s(m_tzApply, 128, L"Edit Thick");
			wcscpy_s(m_tzMiddle, 128, L"Pass");
			wcscpy_s(m_tzCancel, 128, L"Cancel");
		}
	}

	return DoModal();
}

/*
 desc : DoModal Override �Լ�
 parm : mesg	- [in]  ���̾�α� ���ڿ� ��µǴ� �޽���
		apply	- [in]  Apply ��ư ���ڿ�
		cance	- [in]  Cancel ��ư ���ڿ�
 retn : 0L
*/
INT_PTR CDlgMesg::MyDoModal(PTCHAR mesg, PTCHAR apply, PTCHAR cancel)
{
	if (!mesg || wcslen(mesg) < 1)	return -1;

	/* ���� �� ��� ��ư�� Ÿ��Ʋ �� �ӽ� ���� */
	swprintf_s(m_tzApply, 128, L"%s", apply);
	swprintf_s(m_tzCancel, 128, L"%s", cancel);

	return MyDoModal(mesg, 0x00);
}
