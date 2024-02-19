
/*
 desc : E94CJM
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgE94.h"


IMPLEMENT_DYNAMIC(CDlgE94, CDialogEx)

/*
 desc : ������
 parm : None
 retn : None
*/
CDlgE94::CDlgE94(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgE94::IDD, pParent)
{
	m_bExitProc	= FALSE;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDlgE94::~CDlgE94()
{
}

/* ��Ʈ�� ���� */
void CDlgE94::DoDataExchange(CDataExchange* dx)
{
	CDialogEx::DoDataExchange(dx);

	UINT32 i = 0, u32StartID = 0;

//	DDX_Control(dx, IDC_E90_LST_SUBST_LOC,	m_lst_ctl[0]);
}

/* �޽��� �� */
BEGIN_MESSAGE_MAP(CDlgE94, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_E94_BTN_PRJOB_UPDATE, &CDlgE94::OnBnClickedE94BtnPrjobUpdate)
	ON_BN_CLICKED(IDC_E94_BTN_CRJOB_CREATE, &CDlgE94::OnBnClickedE94BtnCrjobCreate)
	ON_BN_CLICKED(IDC_E94_BTN_CRJOB_DELETE, &CDlgE94::OnBnClickedE94BtnCrjobDelete)
	ON_BN_CLICKED(IDC_E94_BTN_CRJOB_UPDATE, &CDlgE94::OnBnClickedE94BtnCrjobUpdate)
END_MESSAGE_MAP()

/*
 desc : ���̾�α� ���� �� �ʱ�ȭ
*/
BOOL CDlgE94::OnInitDialog()
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
VOID CDlgE94::OnClose()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnClose();
}
VOID CDlgE94::OnDestroy()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnDestroy();
}
VOID CDlgE94::ExitProc()
{
	/* �̹� ������ ȣ������� ���� */
	m_bExitProc	= TRUE;

}

/*
 desc : ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgE94::InitCtrl()
{
	INT32 i	= 0;
	TCHAR tzSlotNo[32]	= {NULL};

	((CEdit*)GetDlgItem(IDC_E94_EDT_CRJOB_NAME))->SetWindowText(L"ControlJob001");
	((CEdit*)GetDlgItem(IDC_E94_EDT_CARR_ID))->SetWindowText(L"CARRIER1");
}

/*
 desc : �ֱ������� ȣ��Ǵ� �Լ�
 parm : None
 retn : None
*/
VOID CDlgE94::UpdateCtrl()
{

}

/*
 desc : �̺�Ʈ ������ ���ŵ� ��� (�θ� ���ؼ� ȣ���)
 parm : event	- [in]  �̺�Ʈ ������ ����� ����ü ������
 retn : None
*/
VOID CDlgE94::UpdateCtrl(STG_CCED *event)
{
}

void CDlgE94::OnBnClickedE94BtnPrjobUpdate()
{
	INT32 i = 0;
	CStringArray arrPRJob;
	CAtlList <E40_PPJS> lstState;

	CListBox *pBoxJobName	= (CListBox*)GetDlgItem(IDC_E94_BOX_PRJOB_NAME);
	CListBox *pBoxJobState	= (CListBox*)GetDlgItem(IDC_E94_BOX_PRJOB_STATE);
	pBoxJobName->ResetContent();
	pBoxJobState->ResetContent();
	if (!uvCIMLib_E40PJM_GetProcessAllJobIdState(arrPRJob, lstState))	return;
	for (; i<arrPRJob.GetCount(); i++)
	{
		pBoxJobName->AddString(arrPRJob.GetAt(i));
		switch (lstState.GetAt(lstState.FindIndex(i)))
		{
		case E40_PPJS::en_created				:	pBoxJobState->AddString(L"Created");				break;
		case E40_PPJS::en_queued				:	pBoxJobState->AddString(L"Queued");					break;
		case E40_PPJS::en_setting_up			:	pBoxJobState->AddString(L"Setting Up");				break;
		case E40_PPJS::en_waiting_for_start		:	pBoxJobState->AddString(L"Waiting for Start");		break;
		case E40_PPJS::en_processing			:	pBoxJobState->AddString(L"Processing");				break;
		case E40_PPJS::en_process_completed		:	pBoxJobState->AddString(L"Process Completed");		break;
		case E40_PPJS::en_reserved5				:	pBoxJobState->AddString(L"reserved5");				break;
		case E40_PPJS::en_pausing				:	pBoxJobState->AddString(L"Pausing");				break;
		case E40_PPJS::en_paused				:	pBoxJobState->AddString(L"Paused");					break;
		case E40_PPJS::en_stopping				:	pBoxJobState->AddString(L"Stopping");				break;
		case E40_PPJS::en_aborting				:	pBoxJobState->AddString(L"Aborting");				break;
		case E40_PPJS::en_stopped				:	pBoxJobState->AddString(L"Stopped");				break;
		case E40_PPJS::en_aborted				:	pBoxJobState->AddString(L"Aborted");				break;
		case E40_PPJS::en_processjob_completed	:	pBoxJobState->AddString(L"Process Job Completed");	break;
		}
	}

	arrPRJob.RemoveAll();
	lstState.RemoveAll();

	pBoxJobName->Invalidate(TRUE);
	pBoxJobState->Invalidate(TRUE);
}

void CDlgE94::OnShowWindow(BOOL show, UINT status)
{
	if (show)
	{
		OnBnClickedE94BtnPrjobUpdate();
		OnBnClickedE94BtnCrjobUpdate();
	}
}

void CDlgE94::OnBnClickedE94BtnCrjobCreate()
{
	INT32 i;
	CStringArray arrPRJobID;
	TCHAR tzCarrID[CIM_CARRIER_ID_SIZE]			= {NULL};
	TCHAR tzCRJobID[CIM_CONTROL_JOB_ID_SIZE]	= {NULL};
	TCHAR tzPRJobID[CIM_PRJOB_RECIPE_ID_SIZE]	= {NULL};
	((CEdit*)GetDlgItem(IDC_E94_EDT_CRJOB_NAME))->GetWindowText(tzCRJobID, CIM_CONTROL_JOB_ID_SIZE);
	if (wcslen(tzCRJobID) < 1)	return;
	CListBox *pBoxJobName	= (CListBox*)GetDlgItem(IDC_E94_BOX_PRJOB_NAME);
	if (pBoxJobName->GetCurSel() < 0)	return;
	/* ���� ���õ� PRJobID ���� ��� */
	for (i=0; i<pBoxJobName->GetCount(); i++)
	{
		if (pBoxJobName->GetSel(i) > 0)
		{
			wmemset(tzPRJobID, 0x00, CIM_PRJOB_RECIPE_ID_SIZE);
			pBoxJobName->GetText(i, tzPRJobID);
			arrPRJobID.Add(tzPRJobID);
		}
	}

	/* Carrier ID ��� */
	((CEdit*)GetDlgItem(IDC_E94_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_CARRIER_ID_SIZE);
	/* Create */
	uvCIMLib_E94CJM_CreateJob(tzCRJobID, tzCarrID, &arrPRJobID, E94_CPOM::en_list, TRUE);
}

void CDlgE94::OnBnClickedE94BtnCrjobDelete()
{
	TCHAR tzCRJobID[CIM_CONTROL_JOB_ID_SIZE]	= {NULL};
	((CEdit*)GetDlgItem(IDC_E94_EDT_CRJOB_NAME))->GetWindowText(tzCRJobID, CIM_CONTROL_JOB_ID_SIZE);
	if (wcslen(tzCRJobID) < 1)	return;

}

void CDlgE94::OnBnClickedE94BtnCrjobUpdate()
{
	UINT32 i	= 0;
	CStringArray strArrCtrlID;
	CAtlList<E94_CCJS> lstState;

	CListBox *pBoxJobName	= (CListBox*)GetDlgItem(IDC_E94_BOX_CRJOB_NAME);
	CListBox *pBoxJobState	= (CListBox*)GetDlgItem(IDC_E94_BOX_CRJOB_STATE);

	/* ���� ��ϵ� ��� ���μ��� JOB ���� */

	if (!uvCIMLib_E94CJM_GetControlJobListIdState(strArrCtrlID, lstState, E94_CLCJ::en_list_all))	return;

	pBoxJobName->ResetContent();
	pBoxJobState->ResetContent();

	for (i=0; i<lstState.GetCount(); i++)
	{
		pBoxJobName->AddString(strArrCtrlID.GetAt(i));
		/* Control Job State */
		switch (lstState.GetAt(lstState.FindIndex(i)))
		{
		case E94_CCJS::en_unknown			:	pBoxJobState->AddString(L"unknown");		break;
		case E94_CCJS::en_queued			:	pBoxJobState->AddString(L"queued");			break;
		case E94_CCJS::en_selected			:	pBoxJobState->AddString(L"selected");		break;
		case E94_CCJS::en_waitingforstart	:	pBoxJobState->AddString(L"waitingforstart");break;
		case E94_CCJS::en_executing			:	pBoxJobState->AddString(L"executing");		break;
		case E94_CCJS::en_paused			:	pBoxJobState->AddString(L"paused");			break;
		case E94_CCJS::en_completed			:	pBoxJobState->AddString(L"completed");		break;
		}
	}

	strArrCtrlID.RemoveAll();
	lstState.RemoveAll();
}
