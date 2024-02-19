
/*
 desc : Frame Rate ���� ��ȭ ����
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgStep.h"

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
CDlgStep::CDlgStep(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgStep::IDD, parent)
{
	m_bDrawBG		= TRUE;
	m_bTooltip		= TRUE;

	m_u8Offset		= 0xff;
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgStep::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Button - Normal */
	u32StartID	= IDC_STEP_BTN_APPLY;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Checkbox - Normal */
	u32StartID	= IDC_STEP_CHK_OFFSET_0;
	for (i=0; i<7; i++)		DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgStep, CMyDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_STEP_CHK_OFFSET_0, IDC_STEP_CHK_OFFSET_NONE, OnChkClicked)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_STEP_BTN_APPLY, IDC_STEP_BTN_CANCEL, OnBtnClicked)
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgStep::PreTranslateMessage(MSG* msg)
{

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgStep::OnInitDlg()
{
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
VOID CDlgStep::OnExitDlg()
{
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgStep::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgStep::OnResizeDlg()
{
}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgStep::InitCtrl()
{
	INT32 i;
	PTCHAR pText	= NULL;

	/* button - normal */
	for (i=0; i<2; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);
	}
	/* checkbox - normal */
	for (i=0; i<7; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_chk_ctl[i].SetBgColor(RGB(255, 255, 255));
	}
	m_chk_ctl[6].SetCheck(1);

	return TRUE;
}


/*
 desc : ���� �߻��� ���� ���� ���
 parm : None
 retn : None
*/
VOID CDlgStep::OnBtnClicked(UINT32 id)
{
	if (id == IDC_STEP_BTN_APPLY)
	{
		CMyDialog::EndDialog(IDOK);
	}
	else
	{
		CMyDialog::EndDialog(IDCANCEL);
	}
}

/*
 desc : ���� �߻��� ���� ���� ���
 parm : None
 retn : None
*/
VOID CDlgStep::OnChkClicked(UINT32 id)
{
	for (UINT8 i=0; i<7; i++)	m_chk_ctl[i].SetCheck(0);
	
	m_chk_ctl[id - IDC_STEP_CHK_OFFSET_0].SetCheck(1);
	m_u8Offset	= UINT8(id - IDC_STEP_CHK_OFFSET_0);

	if (m_u8Offset == 0x06)	m_u8Offset	= 0xff;
}

/*
 desc : DoModal Override �Լ�
 parm : None
 retn : 0L
*/
INT_PTR CDlgStep::MyDoModal()
{
	return DoModal();
}
