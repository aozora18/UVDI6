
/*
 desc : SECS-II Message
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgMesg.h"


IMPLEMENT_DYNAMIC(CDlgMesg, CDialogEx)

/*
 desc : 생성자
 parm : None
 retn : None
*/
CDlgMesg::CDlgMesg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgMesg::IDD, pParent)
{
	m_bExitProc	= FALSE;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgMesg::~CDlgMesg()
{
}

/* 컨트롤 연결 */
void CDlgMesg::DoDataExchange(CDataExchange* dx)
{
	CDialogEx::DoDataExchange(dx);

}

/* 메시지 맵 */
BEGIN_MESSAGE_MAP(CDlgMesg, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_MESG_BTN_SEND, OnBtnTermBtnSend)
END_MESSAGE_MAP()

/*
 desc : 다이얼로그 생성 후 초기화
*/
BOOL CDlgMesg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	/* 컨트롤 초기화 */
	InitCtrl();

	return TRUE;
}

/*
 desc : 다이얼로그가 종료될 때 마지막 한번 호출
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
	/* 이미 이전에 호출된적이 있음 */
	m_bExitProc	= TRUE;

}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgMesg::InitCtrl()
{
	UINT32 i		= 0;
	TCHAR tzID[32]	= {NULL};
	CComboBox *pCmb;

	((CComboBox *)GetDlgItem(IDC_MESG_CMB_TERM_ID))->SetCurSel(0);

	/* Stream ID 생성 */
	pCmb	= ((CComboBox *)GetDlgItem(IDC_MESG_CMB_ID_STREAM));
	for (i=1; i<=19; i++)
	{
		swprintf_s(tzID, 32, L"%02X", i);
		pCmb->AddString(tzID);
	}
	pCmb->SetCurSel(0);
	/* Function ID 생성 */
	pCmb	= ((CComboBox *)GetDlgItem(IDC_MESG_CMB_ID_FUNCTION));
	for (i=1; i<=32; i++)
	{
		swprintf_s(tzID, 32, L"%02X", i);
		pCmb->AddString(tzID);
	}
	pCmb->SetCurSel(0);
}

/*
 desc : 주기적으로 호출되는 함수
 parm : None
 retn : None
*/
VOID CDlgMesg::UpdateCtrl()
{
	TCHAR tzMesg[512]	= {NULL};

}

/*
 desc : 이벤트 정보가 수신된 경우 (부모에 의해서 호출됨)
 parm : event	- [in]  이벤트 정보가 저장된 구조체 포인터
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

	/* Stream ID 생성 */
	pCmb	= ((CComboBox *)GetDlgItem(IDC_MESG_CMB_ID_STREAM));
	pCmb->GetLBText(pCmb->GetCurSel(), tzStreamID);
	/* Function ID 생성 */
	pCmb	= ((CComboBox *)GetDlgItem(IDC_MESG_CMB_ID_FUNCTION));
	pCmb->GetLBText(pCmb->GetCurSel(), tzFunctionID);

	/* SECS-II Message 전달 */
	uvCIMLib_SendPrimaryMessage(EQUIP_CONN_ID,
								(UINT8)wcstol(tzStreamID, NULL, 16),
								(UINT8)wcstol(tzFunctionID, NULL, 16),
								tzMesg,
								(UINT8)_wtoi(tzTermID), u8ChkReply);
}
