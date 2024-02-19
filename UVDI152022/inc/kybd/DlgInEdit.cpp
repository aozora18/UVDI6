

#include "pch.h"
#include "DlgInEdit.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : �ڽ��� ���� ID, �θ� ���� ��ü
 retn : None
*/
CDlgInEdit::CDlgInEdit(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgInEdit::IDD, parent)
{
	m_bDrawBG	= 0x01;
	m_hIcon		= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgInEdit::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	DDX_Control(dx, IDC_IN_EDT_BTN_APPLY,	m_btn_ctl[0]);
	DDX_Control(dx, IDC_IN_EDT_BTN_CANCEL,	m_btn_ctl[1]);
	DDX_Control(dx, IDC_IN_EDT_EDT_VALUE,	m_edt_ctl[0]);
}

BEGIN_MESSAGE_MAP(CDlgInEdit, CMyDialog)
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_IN_EDT_BTN_APPLY,		OnBtnApply)
	ON_BN_CLICKED(IDC_IN_EDT_BTN_CANCEL,	OnBtnCancel)
END_MESSAGE_MAP()


BOOL CDlgInEdit::PreTranslateMessage(MSG* msg)
{
	if (msg->message == WM_KEYDOWN && msg->wParam == VK_RETURN)
	{
		OnBtnApply();
		return TRUE;
	}

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : �ý��� ��ɾ� ����
 parm : id		- ���õ� �׸� ID ��
		lparam	- ???
 retn : 1 - ���� / 0 - ����
*/
VOID CDlgInEdit::OnSysCommand(UINT32 id, LPARAM lparam)
{
//	switch (id)
//	{
//	}

	CMyDialog::OnSysCommand(id, lparam);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : DI_TRUE or DI_FALSE
*/
BOOL CDlgInEdit::OnInitDlg()
{
	// ���� ��Ʈ�� �ʱ�ȭ
	if (!InitControl())		return FALSE;

	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgInEdit::OnExitDlg()
{
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgInEdit::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgInEdit::OnResizeDlg()
{
}

/*
 desc : ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : DI_TRUE or DI_FALSE
*/
BOOL CDlgInEdit::InitControl()
{
	INT32 i;

	// ����Ʈ
	for (i=0; i<1; i++)
	{
		m_edt_ctl[i].SetEditFont(&g_lf);
		m_edt_ctl[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_ctl[i].SetTextColor(RGB(0x00, 0x00, 0x00));
	}
	m_edt_ctl[0].SetFocus();
	// �Ϲ� ��ư
	for (i=0; i<2; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}

	return TRUE;
}

/*
 desc : OnOK
 parm : None
 retn : None
*/
VOID CDlgInEdit::OnBtnApply()
{
	if (m_edt_ctl[0].GetTextToLen() < 1)
	{
		AfxMessageBox(L"Not value entered!", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	m_i64Value	= m_edt_ctl[0].GetTextToNum();
	m_u64Value	= (UINT64)m_edt_ctl[0].GetTextToNum();
	m_dbValue	= m_edt_ctl[0].GetTextToDouble();

	CMyDialog::OnOK();
}

/*
 desc : OnCancel
 parm : None
 retn : None
*/
VOID CDlgInEdit::OnBtnCancel()
{

	CMyDialog::OnCancel();
}
