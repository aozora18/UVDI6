
/*
 desc : Terminal
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgTerm.h"


IMPLEMENT_DYNAMIC(CDlgTerm, CDialogEx)

/*
 desc : ������
 parm : None
 retn : None
*/
CDlgTerm::CDlgTerm(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgTerm::IDD, pParent)
{
	m_bExitProc	= FALSE;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDlgTerm::~CDlgTerm()
{
}

/* ��Ʈ�� ���� */
void CDlgTerm::DoDataExchange(CDataExchange* dx)
{
	CDialogEx::DoDataExchange(dx);

}

/* �޽��� �� */
BEGIN_MESSAGE_MAP(CDlgTerm, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_TERM_BTN_ACK, OnBtnTermBtnAck)
	ON_BN_CLICKED(IDC_TERM_BTN_SEND, OnBtnTermBtnSend)
END_MESSAGE_MAP()

/*
 desc : ���̾�α� ���� �� �ʱ�ȭ
*/
BOOL CDlgTerm::OnInitDialog()
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
VOID CDlgTerm::OnClose()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnClose();
}
VOID CDlgTerm::OnDestroy()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnDestroy();
}
VOID CDlgTerm::ExitProc()
{
	/* �̹� ������ ȣ������� ���� */
	m_bExitProc	= TRUE;

}

/*
 desc : ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgTerm::InitCtrl()
{
	((CComboBox *)GetDlgItem(IDC_TERM_CMB_TERM_ID))->SetCurSel(0);
}

/*
 desc : �ֱ������� ȣ��Ǵ� �Լ�
 parm : None
 retn : None
*/
VOID CDlgTerm::UpdateCtrl()
{
}

/*
 desc : �̺�Ʈ ������ ���ŵ� ��� (�θ� ���ؼ� ȣ���)
 parm : event	- [in]  �̺�Ʈ ������ ����� ����ü ������
 retn : None
*/
VOID CDlgTerm::UpdateCtrl(STG_CCED *event)
{
	TCHAR tzTerm[1024]	= {NULL};
	CEdit *pEdit		= NULL;

	if ((ENG_ESSC)event->exx_id != ENG_ESSC::en_e30_gem ||
		(E30_CEFI)event->cbf_id != E30_CEFI::en_terminal_msg_rcvd)	return;
	if (!event->str_data1)				return;
	if (wcslen(event->str_data1) < 1)	return;
	
	swprintf_s(tzTerm, 1024, L"host_id=%d, term_id=%d, mesg=%s",
			   event->evt_data.e30_tm.conn_id, event->evt_data.e30_tm.term_id, event->str_data1);
	/* ���ڿ� ���� �ٹٲ� (Carrage Return) �ֱ� */
	INT32 i32len	= (INT32)wcslen(tzTerm);
	tzTerm[i32len]	= L'\r';	i32len++;
	tzTerm[i32len]	= L'\n';	i32len++;
	tzTerm[i32len]	= NULL;
	/* Edit Control�� ��� */
	pEdit	= (CEdit *)GetDlgItem(IDC_TERM_EDT_IN);
	uvCmn_AppendEditMesg(pEdit, tzTerm);
}

/*
 desc : Send a terminal service acknowledge event to the host. Send to host 1 by default.
 parm : None
 retn : None
*/
VOID CDlgTerm::OnBtnTermBtnAck()
{
	uvCIMLib_SendTerminalAcknowledge(EQUIP_CONN_ID);
}

/*
 desc : Send a terminal service acknowledge event to the host. Send to host 1 by default.
 parm : None
 retn : None
*/
VOID CDlgTerm::OnBtnTermBtnSend()
{
	TCHAR tzMesg[512]	= {NULL};
	TCHAR tzTermID[8]	= {NULL};
	((CEdit *)GetDlgItem(IDC_TERM_EDT_OUT))->GetWindowText(tzMesg, 1024);
	if (wcslen(tzMesg) < 1)	return;
	CComboBox *pCmb	= (CComboBox*)GetDlgItem(IDC_TERM_CMB_TERM_ID);
	if (pCmb->GetCurSel() < 0)	return;
	pCmb->GetLBText(pCmb->GetCurSel(), tzTermID);
	uvCIMLib_SendTerminalMessage(EQUIP_CONN_ID, _wtoi(tzTermID), tzMesg);
}
