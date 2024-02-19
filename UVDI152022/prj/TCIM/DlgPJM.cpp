
/*
 desc : CIM300
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgPJM.h"


IMPLEMENT_DYNAMIC(CDlgPJM, CDialogEx)

/*
 desc : ������
 parm : None
 retn : None
*/
CDlgPJM::CDlgPJM(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgPJM::IDD, pParent)
{
	m_bExitProc	= FALSE;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDlgPJM::~CDlgPJM()
{
}

/* ��Ʈ�� ���� */
void CDlgPJM::DoDataExchange(CDataExchange* dx)
{
	CDialogEx::DoDataExchange(dx);

}

/* �޽��� �� */
BEGIN_MESSAGE_MAP(CDlgPJM, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_PJM_BTN_PJ_STATE_UPDATE, &CDlgPJM::OnBnClickedPjmBtnPjStateUpdate)
	ON_BN_CLICKED(IDC_PJM_BTN_PJ_LIST_GET, &CDlgPJM::OnBnClickedPjmBtnPjListGet)
END_MESSAGE_MAP()

/*
 desc : ���̾�α� ���� �� �ʱ�ȭ
*/
BOOL CDlgPJM::OnInitDialog()
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
VOID CDlgPJM::OnClose()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnClose();
}
VOID CDlgPJM::OnDestroy()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnDestroy();
}
VOID CDlgPJM::ExitProc()
{
	/* �̹� ������ ȣ������� ���� */
	m_bExitProc	= TRUE;

}

/*
 desc : ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgPJM::InitCtrl()
{
}

/*
 desc : �ֱ������� ȣ��Ǵ� �Լ�
 parm : None
 retn : None
*/
VOID CDlgPJM::UpdateCtrl()
{

}

/*
 desc : �̺�Ʈ ������ ���ŵ� ��� (�θ� ���ؼ� ȣ���)
 parm : event	- [in]  �̺�Ʈ ������ ����� ����ü ������
 retn : None
*/
VOID CDlgPJM::UpdateCtrl(STG_CCED *event)
{

}


VOID CDlgPJM::OnBnClickedPjmBtnPjStateUpdate()
{
	TCHAR tzPRJobID[CIM_PROCESS_JOB_ID_SIZE]	= {NULL};
	TCHAR tzState[256]	= {NULL};
	CListBox *pBoxPRJob = (CListBox*)GetDlgItem(IDC_PJM_BOX_PJ_LIST);
	CListBox *pBoxSubst = (CListBox*)GetDlgItem(IDC_PJM_BOX_PJ_STATE_UPDATE);
	CAtlList <STG_CSDI> lstState;
	STG_CSDI stState;

	INT32 i32Sel	= pBoxPRJob->GetCurSel();
	if (i32Sel < 0)	return;
	pBoxPRJob->GetText(i32Sel, tzPRJobID);
	if (wcslen(tzPRJobID) < 1)	return;

	pBoxSubst->SetRedraw(FALSE);
	pBoxSubst->ResetContent();

	uvCIMLib_GetSubstIDStateAll(tzPRJobID, lstState);
	for (INT32 i=0; i<lstState.GetCount(); i++)
	{
		stState	= lstState.GetAt(lstState.FindIndex(i));
		swprintf_s(tzState, 256, L"PRJobID:%s.CarrierID:%s.Process:%d.Transport:%d",
				   tzPRJobID, stState.carr_id, stState.proc_state, stState.trans_state);
	}
	lstState.RemoveAll();

	pBoxSubst->SetRedraw(TRUE);
	pBoxSubst->Invalidate(TRUE);
}

VOID CDlgPJM::OnBnClickedPjmBtnPjListGet()
{
	INT32 i	= 0;
	CListBox *pBox = (CListBox*)GetDlgItem(IDC_PJM_BOX_PJ_LIST);
	CStringArray arrPRJobID;

	pBox->SetRedraw(FALSE);
	pBox->ResetContent();

	if (uvCIMLib_E40PJM_GetProcessAllJobId(arrPRJobID))
	{
		for (; i<arrPRJobID.GetCount(); i++)	pBox->AddString(arrPRJobID[i].GetBuffer());
	}

	pBox->SetRedraw(TRUE);
	pBox->Invalidate(TRUE);
}
