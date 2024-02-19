
/*
 desc : SECS-II Message
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgMesg.h"


IMPLEMENT_DYNAMIC(CDlgMesg, CDialogEx)

/*
 desc : ������
 parm : None
 retn : None
*/
CDlgMesg::CDlgMesg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgMesg::IDD, pParent)
{
	m_bExitProc	= FALSE;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDlgMesg::~CDlgMesg()
{
}

/* ��Ʈ�� ���� */
void CDlgMesg::DoDataExchange(CDataExchange* dx)
{
	CDialogEx::DoDataExchange(dx);

}

/* �޽��� �� */
BEGIN_MESSAGE_MAP(CDlgMesg, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_MESG_BTN_SEND, OnBtnTermBtnSend)
END_MESSAGE_MAP()

/*
 desc : ���̾�α� ���� �� �ʱ�ȭ
*/
BOOL CDlgMesg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	/* ��Ʈ�� �ʱ�ȭ */
	InitCtrl();

	return TRUE;
}

/*
 desc : ���̾�αװ� ����� �� ������ �ѹ� ȣ��
 parm : None
 retn : None
*/
VOID CDlgMesg::OnClose()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnClose();
}
VOID CDlgMesg::OnDestroy()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnDestroy();
}
VOID CDlgMesg::ExitProc()
{
	/* �̹� ������ ȣ������� ���� */
	m_bExitProc	= TRUE;

}

/*
 desc : ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgMesg::InitCtrl()
{
	UINT32 i		= 0;
	TCHAR tzID[32]	= {NULL};
	CComboBox *pCmb;

	((CComboBox *)GetDlgItem(IDC_MESG_CMB_TERM_ID))->SetCurSel(0);

	/* Stream ID ���� */
	pCmb	= ((CComboBox *)GetDlgItem(IDC_MESG_CMB_ID_STREAM));
	for (i=1; i<=19; i++)
	{
		swprintf_s(tzID, 32, L"%02X", i);
		pCmb->AddString(tzID);
	}
	pCmb->SetCurSel(0);
	/* Function ID ���� */
	pCmb	= ((CComboBox *)GetDlgItem(IDC_MESG_CMB_ID_FUNCTION));
	for (i=1; i<=32; i++)
	{
		swprintf_s(tzID, 32, L"%02X", i);
		pCmb->AddString(tzID);
	}
	pCmb->SetCurSel(0);
}

/*
 desc : �ֱ������� ȣ��Ǵ� �Լ�
 parm : None
 retn : None
*/
VOID CDlgMesg::UpdateCtrl()
{
	TCHAR tzMesg[512]	= {NULL};

}

/*
 desc : �̺�Ʈ ������ ���ŵ� ��� (�θ� ���ؼ� ȣ���)
 parm : event	- [in]  �̺�Ʈ ������ ����� ����ü ������
 retn : None
*/
VOID CDlgMesg::UpdateCtrl(STG_CCED *event)
{
}

/*
 desc : Send a terminal service acknowledge event to the host. Send to host 1 by default.
 parm : None
 retn : None
*/
VOID CDlgMesg::OnBtnTermBtnSend()
{
	TCHAR tzMesg[128]		= {NULL};
	TCHAR tzTermID[32]		= {NULL};
	TCHAR tzStreamID[32]	= {NULL};
	TCHAR tzFunctionID[32]	= {NULL};
	UINT8 u8ChkReply= (UINT8)((CButton*)GetDlgItem(IDC_MESG_CHK_REPLY))->GetCheck();
	CEdit *pEdit	= (CEdit*)GetDlgItem(IDC_MESG_EDT_OUT);
	pEdit->GetWindowText(tzMesg, 128);
	CComboBox *pCmb;

	pCmb	= (CComboBox*)GetDlgItem(IDC_MESG_CMB_TERM_ID);
	pCmb->GetLBText(pCmb->GetCurSel(), tzTermID);

	/* Stream ID ���� */
	pCmb	= ((CComboBox *)GetDlgItem(IDC_MESG_CMB_ID_STREAM));
	pCmb->GetLBText(pCmb->GetCurSel(), tzStreamID);
	/* Function ID ���� */
	pCmb	= ((CComboBox *)GetDlgItem(IDC_MESG_CMB_ID_FUNCTION));
	pCmb->GetLBText(pCmb->GetCurSel(), tzFunctionID);

	/* SECS-II Message ���� */
	uvCIMLib_SendPrimaryMessage(EQUIP_CONN_ID,
								(UINT8)wcstol(tzStreamID, NULL, 16),
								(UINT8)wcstol(tzFunctionID, NULL, 16),
								tzMesg,
								(UINT8)_wtoi(tzTermID), u8ChkReply);
}
