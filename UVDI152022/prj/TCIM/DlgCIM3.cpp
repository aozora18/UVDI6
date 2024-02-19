
/*
 desc : CIM300
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgCIM3.h"


IMPLEMENT_DYNAMIC(CDlgCIM3, CDialogEx)

/*
 desc : ������
 parm : None
 retn : None
*/
CDlgCIM3::CDlgCIM3(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgCIM3::IDD, pParent)
{
	m_bExitProc	= FALSE;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDlgCIM3::~CDlgCIM3()
{
}

/* ��Ʈ�� ���� */
void CDlgCIM3::DoDataExchange(CDataExchange* dx)
{
	CDialogEx::DoDataExchange(dx);

	DDX_Control(dx, IDC_CIM3_LST_PROCESS_JOB, m_lst_proc);
	DDX_Control(dx, IDC_CIM3_LST_CONTROL_JOB, m_lst_ctrl);
}

/* �޽��� �� */
BEGIN_MESSAGE_MAP(CDlgCIM3, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CIM3_CHK_SLOP_RANDOM_ALL, IDC_CIM3_CHK_SLOP_RANDOM_SKIP, OnChkClicked)
	ON_BN_CLICKED(IDC_CIM3_BTN_PROC_WITH_CARR1,		OnBnClickedProcWithCarr)
	ON_BN_CLICKED(IDC_CIM3_BTN_PROC_WITH_CARR2,		OnBnClickedProcWithCarr)
	ON_BN_CLICKED(IDC_CIM3_BTN_CANCEL_CARRIER,		OnBnClickedCancelCarrier)
	ON_BN_CLICKED(IDC_CIM3_BTN_CARR_RE_CREATE,		OnBnClickedCarrReCreate)
	ON_BN_CLICKED(IDC_CIM3_BTN_RELEASE_CARRIER,		OnBnClickedReleaseCarrier)
	ON_BN_CLICKED(IDC_CIM3_BTN_PROC_WITH_SUBST,		OnBnClickedProcWithSubst)
	ON_BN_CLICKED(IDC_CIM3_BTN_CARR_TRANSFER_IN,	OnBnClickedCarrierTransferStart)
	ON_BN_CLICKED(IDC_CIM3_BTN_CARR_PLACED,			OnBnClickedCarrierPlaced)
	ON_BN_CLICKED(IDC_CIM3_BTN_CARR_CLAMPED,		OnBnClickedCarrierClamped)
	ON_BN_CLICKED(IDC_CIM3_BTN_CARR_AT_PORT,		OnBnClickedCarrierAtPort)
	ON_BN_CLICKED(IDC_CIM3_BTN_MATERIAL_RECEIVED,	OnBnClickedMaterialReceived)
	ON_BN_CLICKED(IDC_CIM3_BTN_CARR_MOVE_DOCK,		OnBnClickedCarrierMoveDock)
	ON_BN_CLICKED(IDC_CIM3_BTN_CARR_OPENED,			OnBnClickedCarrierOpened)
	ON_BN_CLICKED(IDC_CIM3_BTN_CARR_CLOSED,			OnBnClickedCarrierClosed)
	ON_BN_CLICKED(IDC_CIM3_BTN_READ_SLOT_MAP,		OnBnClickedReadSlotMap)
	ON_BN_CLICKED(IDC_CIM3_BTN_CARR_MOVE_UNDOCK,	OnBnClickedCarrierMoveUndock)
	ON_BN_CLICKED(IDC_CIM3_BTN_CARR_UNCLAMPED,		OnBnClickedCarrierUnclamped)
	ON_BN_CLICKED(IDC_CIM3_BTN_CARR_READY_UNLOAD,	OnBnClickedCarrierReadyUnload)
	ON_BN_CLICKED(IDC_CIM3_BTN_CARR_TRANSFER_OUT,	OnBnClickedCarrierTransferStart)
	ON_BN_CLICKED(IDC_CIM3_BTN_CARR_DEPART_PORT,	OnBnClickedCarrierDepartedPort)
	ON_BN_CLICKED(IDC_CIM3_BTN_MATERIAL_REMOVED,	OnBnClickedMaterialRemoved)
	ON_BN_CLICKED(IDC_CIM3_BTN_CTRL_SELECT_JOB,		OnBnClickedSelectedControlJob)
	ON_BN_CLICKED(IDC_CIM3_BTN_CTRL_DESELECT_JOB,	OnBnClickedDeselectedControlJob)
	ON_BN_CLICKED(IDC_CIM3_BTN_CARR_ACCESSING,		OnBnClickedCarrierAccessing)
	ON_BN_CLICKED(IDC_CIM3_BTN_CARR_ACCESS_COMP,	OnBnClickedCarrierAccessComplete)
	ON_BN_CLICKED(IDC_CIM3_BTN_LOAD_MATERIAL_LIST,	OnBnClickedCim3BtnLoadMaterialList)
	ON_BN_CLICKED(IDC_CIM3_BTN_PROC_JOB_ALL_GET,	OnBnClickedProcJobAllGet)
	ON_BN_CLICKED(IDC_CIM3_BTN_CTRL_JOB_ALL_GET,	OnBnClickedCtrlJobAllGet)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CIM3_BTN_PROC_JOB_ABORT, IDC_CIM3_BTN_PROC_JOB_CANCEL, OnBnClickedPRJobCtrl)
	ON_BN_CLICKED(IDC_CIM3_BTN_PRJOB_COMPLETED,		OnBnClickedCim3BtnPrjobCompleted)
END_MESSAGE_MAP()

/*
 desc : ���̾�α� ���� �� �ʱ�ȭ
*/
BOOL CDlgCIM3::OnInitDialog()
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
VOID CDlgCIM3::OnClose()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnClose();
}
VOID CDlgCIM3::OnDestroy()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnDestroy();
}
VOID CDlgCIM3::ExitProc()
{
	/* �̹� ������ ȣ������� ���� */
	m_bExitProc	= TRUE;

}

/*
 desc : ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgCIM3::InitCtrl()
{
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->SetWindowText(L"CARRIER1");
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_LOAD_PORT_ID))->SetWindowText(L"1");
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_SLOT_NUMBER))->SetWindowText(L"01");
	/* Control Job IDs */
	m_lst_ctrl.InsertColumn(0, L"Control ID", LVCFMT_CENTER, 250);
	m_lst_ctrl.InsertColumn(1, L"State", LVCFMT_CENTER, 100);
	m_lst_ctrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	/* Process Job IDs */
	m_lst_proc.InsertColumn(0, L"Control ID", LVCFMT_CENTER, 130);
	m_lst_proc.InsertColumn(1, L"Process ID", LVCFMT_CENTER, 130);
	m_lst_proc.InsertColumn(2, L"State", LVCFMT_CENTER, 100);
	m_lst_proc.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
}

/*
 desc : �ֱ������� ȣ��Ǵ� �Լ�
 parm : None
 retn : None
*/
VOID CDlgCIM3::UpdateCtrl()
{
	INT32 i, j, k = 0, i32Count;
	TCHAR tzMesg[512]	= {NULL}, tzCarrID[CIM_CARRIER_ID_SIZE]	= {NULL};
	E40_PPJS enPPJS;
#if 0
	E87_LPTS enLPTS;
	E87_CIVS enCIVS;
	E87_SMVS enSMVS;
#endif
	CStringArray arrCtrlIDs, arrProcIDs;
	CAtlList<E94_CCJS> lstCtrlState;
	CEdit *pEdit		= {NULL};

	/* Control Job IDs �� States ��� */
	if (uvCIMLib_E94CJM_GetControlJobListIdState(arrCtrlIDs, lstCtrlState))
	{
		m_lst_ctrl.SetRedraw(FALSE);

		/* ��� */
		i32Count	= m_lst_ctrl.GetItemCount();
		for (i=0,k=0; i<arrCtrlIDs.GetCount(); i++,k++)
		{
			/* Control Job ID */
			if (i32Count > k)	m_lst_ctrl.SetItemText(k, 0, arrCtrlIDs[i]);
			else				m_lst_ctrl.InsertItem(k, arrCtrlIDs[i]);
			/* Control Job State */
			switch (lstCtrlState.GetAt(lstCtrlState.FindIndex(k)))
			{
			case E94_CCJS::en_unknown			:	m_lst_ctrl.SetItemText(k, 1, L"unknown");		   break;
			case E94_CCJS::en_queued			:	m_lst_ctrl.SetItemText(k, 1, L"queued");		   break;
			case E94_CCJS::en_selected			:	m_lst_ctrl.SetItemText(k, 1, L"selected");		   break;
			case E94_CCJS::en_waitingforstart	:	m_lst_ctrl.SetItemText(k, 1, L"waitingforstart");  break;
			case E94_CCJS::en_executing			:	m_lst_ctrl.SetItemText(k, 1, L"executing");		   break;
			case E94_CCJS::en_paused			:	m_lst_ctrl.SetItemText(k, 1, L"paused");		   break;
			case E94_CCJS::en_completed			:	m_lst_ctrl.SetItemText(k, 1, L"completed");		   break;
			}
		}

		/* ���� ��ϵ� �׸� ���� */
		i32Count	= m_lst_ctrl.GetItemCount();
		for (i=i32Count-1; i>k; i--)
		{
			m_lst_ctrl.DeleteItem(i);
		}
		/* �ӽ� ����� �׸� ��� ���� */
		lstCtrlState.RemoveAll();

		m_lst_ctrl.SetRedraw(TRUE);
		m_lst_ctrl.Invalidate(FALSE);
	}

	m_lst_proc.SetRedraw(FALSE);
	/* Process Job IDs ��� */
	for (i=0,k=0; i<arrCtrlIDs.GetCount(); i++)
	{
		if (uvCIMLib_E94CJM_GetProcessJobList(arrCtrlIDs[i].GetBuffer(), arrProcIDs))
		{
			for (j=0; j<arrProcIDs.GetCount(); j++,k++)
			{
				/* Control Job ID */
				if (m_lst_proc.GetItemCount() > k)	m_lst_proc.SetItemText(k, 0, arrCtrlIDs[i]);
				else								m_lst_proc.InsertItem(k, arrCtrlIDs[i]);
				/* Process Job ID */
				m_lst_proc.SetItemText(k, 1, arrProcIDs[j]);
				/* Process Job State */
				if (uvCIMLib_E40PJM_GetProcessJobState(arrProcIDs[j].GetBuffer(), enPPJS))
				{
					switch (enPPJS)
					{
					case E40_PPJS::en_created				:	m_lst_proc.SetItemText(k, 2, L"created");				break;	
					case E40_PPJS::en_queued				:	m_lst_proc.SetItemText(k, 2, L"queued");				break;
					case E40_PPJS::en_setting_up			:	m_lst_proc.SetItemText(k, 2, L"setting_up");
																Sleep(5000);
																uvCIMLib_E40PJM_SetPRJobStartProcess(arrProcIDs[j].GetBuffer());	break; // [Ian] ���� ó���δ� setting up ���°� ������ 5�� ���� ������ �ΰ� PRJobStart().
					case E40_PPJS::en_waiting_for_start		:	m_lst_proc.SetItemText(k, 2, L"waiting_for_start");					break;
					case E40_PPJS::en_processing			:	m_lst_proc.SetItemText(k, 2, L"processing");						
																uvCIMLib_Mixed_CheckStartNextJob(arrProcIDs[j].GetBuffer());		break; // [Ian] C#���ÿ� state change �ݹ鿡 �ش��ϴ� �κ�, processing���� �Ѿ�� ���� job�� Ȯ���ؾ���.
					case E40_PPJS::en_process_completed		:	m_lst_proc.SetItemText(k, 2, L"process_completed");					break;
					case E40_PPJS::en_reserved5				:	m_lst_proc.SetItemText(k, 2, L"reserved5");							break;
					case E40_PPJS::en_pausing				:	m_lst_proc.SetItemText(k, 2, L"pausing");							break;	
					case E40_PPJS::en_paused				:	m_lst_proc.SetItemText(k, 2, L"paused");							
																uvCIMLib_Mixed_CheckStartNextJob(arrProcIDs[j].GetBuffer());		break; // [Ian] C#���ÿ� state change �ݹ鿡 �ش��ϴ� �κ�, processing���� �Ѿ�� ���� job�� Ȯ���ؾ���.
					case E40_PPJS::en_stopping				:	m_lst_proc.SetItemText(k, 2, L"stopping");							
																uvCIMLib_Mixed_SkipSubstratesNeedingProcessing(arrProcIDs[j].GetBuffer()); // [Ian] C#���ÿ� state change �ݹ鿡 �ش��ϴ� �κ�,
																uvCIMLib_Mixed_CheckJobComplete(arrProcIDs[j].GetBuffer());				   // stopping ���°��Ǹ�, 1)���� substrates processing_state�� ��� skipped�� �ٲٰ�, 2)job complete�� üũ�ϰ�, 3)���� job�� Ȯ���ؼ� ������
																uvCIMLib_Mixed_CheckStartNextJob(arrProcIDs[j].GetBuffer());		break;
					case E40_PPJS::en_aborting				:	m_lst_proc.SetItemText(k, 2, L"aborting");							
																uvCIMLib_Mixed_SkipSubstratesNeedingProcessing(arrProcIDs[j].GetBuffer());	// aborting ���°��Ǹ�, 1)���� substrates processing_state�� ��� skipped�� �ٲٰ�, 2)job complete�� üũ�ϰ�, 3)���� job�� Ȯ���ؼ� ������
																uvCIMLib_Mixed_CheckJobComplete(arrProcIDs[j].GetBuffer());
																uvCIMLib_Mixed_CheckStartNextJob(arrProcIDs[j].GetBuffer());		break;
					case E40_PPJS::en_stopped				:	m_lst_proc.SetItemText(k, 2, L"stopped");							break;	
					case E40_PPJS::en_aborted				:	m_lst_proc.SetItemText(k, 2, L"aborted");							break;	
					case E40_PPJS::en_processjob_completed	:	m_lst_proc.SetItemText(k, 2, L"processjob_completed");				break;
					}
				}
			}
		}
		arrProcIDs.RemoveAll();
	}
	/* ���� ��ϵ� �׸� ���� */
	i32Count	= m_lst_proc.GetItemCount();
	for (i=i32Count-1; i>k; i--)
	{
		m_lst_proc.DeleteItem(i);
	}

	m_lst_proc.SetRedraw(TRUE);
	m_lst_proc.Invalidate(FALSE);
	arrCtrlIDs.RemoveAll();
#if 0
	/* Update : Load Port Transfer Status */
	for (i=0; i<2; i++)
	{
		pEdit	= (CEdit*)GetDlgItem(IDC_CIM3_EDT_TRANSER_STATE1+i);
		pEdit->SetRedraw(FALSE);
		if (uvCIMLib_E87CMS_GetLoadPortTransferStatus(UINT16(i+1), enLPTS))
		{
			switch (enLPTS)
			{
			case E87_LPTS::en_lp_out_of_service		:	pEdit->SetWindowText(L"OutOfService");		break;
			case E87_LPTS::en_lp_transfer_blocked	:	pEdit->SetWindowText(L"TransferBlocked");	break;
			case E87_LPTS::en_lp_ready_to_load		:	pEdit->SetWindowText(L"ReadyToLoad");		break;
			case E87_LPTS::en_lp_ready_to_unload		:	pEdit->SetWindowText(L"ReadyToUnload");		break;
			case E87_LPTS::en_lp_in_service			:	pEdit->SetWindowText(L"InService");			break;
			case E87_LPTS::en_lp_transfer_ready		:	pEdit->SetWindowText(L"TransferReady");		break;
			}
		}
		pEdit->SetRedraw(TRUE);
		pEdit->Invalidate(FALSE);
	}

	/* ID Verification (Carrier Object ID Status State Machine) */
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_CARRIED_ID_SIZE);
	if (wcslen(tzCarrID) > 0)
	{
		if (uvCIMLib_E87CMS_GetCarrierIDStatus(tzCarrID, enCIOS))
		{
			pEdit	= (CEdit*)GetDlgItem(IDC_CIM3_EDT_ID_VERIFICATION);
			pEdit->SetRedraw(FALSE);
			switch (enCIVS)
			{
			case E87_CIVS::en_id_not_read			:	pEdit->SetWindowText(L"Not Read");				break;
			case E87_CIVS::en_id_wait_for_host		:	pEdit->SetWindowText(L"Wait for host");			break;
			case E87_CIVS::en_id_verification_ok		:	pEdit->SetWindowText(L"Verification OK");		break;
			case E87_CIVS::en_id_verification_failed	:	pEdit->SetWindowText(L"Verification Failed");	break;
			}
			pEdit->SetRedraw(TRUE);
			pEdit->Invalidate(FALSE);
		}
	}
	/* ID Slot Map Status (The slot map status of the specified carrier) */
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_CARRIED_ID_SIZE);
	if (wcslen(tzCarrID) > 0)
	{
		if (uvCIMLib_E87CMS_GetCarrierIDSlotMapStatus(tzCarrID, enSMVS))
		{
			pEdit	= (CEdit*)GetDlgItem(IDC_CIM3_EDT_SLOT_VERIFICATION);
			pEdit->SetRedraw(FALSE);
			switch (enSMVS)
			{
			case E87_SMVS::en_sm_not_read			:	pEdit->SetWindowText(L"Not Read");				break;
			case E87_SMVS::en_sm_wait_for_host		:	pEdit->SetWindowText(L"Wait for host");			break;
			case E87_SMVS::en_sm_verification_ok		:	pEdit->SetWindowText(L"Verification OK");		break;
			case E87_SMVS::en_sm_verification_failed	:	pEdit->SetWindowText(L"Verification Failed");	break;
			}
			pEdit->SetRedraw(TRUE);
			pEdit->Invalidate(FALSE);
		}
	}
#endif
}

/*
 desc : �̺�Ʈ ������ ���ŵ� ��� (�θ� ���ؼ� ȣ���)
 parm : event	- [in]  �̺�Ʈ ������ ����� ����ü ������
 retn : None
*/
VOID CDlgCIM3::UpdateCtrl(STG_CCED *event)
{
	TCHAR tzMesg[256]	= {NULL};
	CEdit *pEdit= NULL;

	switch (event->exx_id)
	{
	case ENG_ESSC::en_e87_cms	:
		switch (event->cbf_id)
		{
		case E87_CEFI::en_state_change	:
			switch (event->evt_data.e87_sc.machine)
			{
			case E87_CDSM::en_e87lptsm	:	/* load port transfer state machine */
				pEdit	= (CEdit*)GetDlgItem(IDC_CIM3_EDT_TRANSER_STATE);
				switch (event->evt_data.e87_sc.state)
				{
				case E87_LPTS::en_lp_out_of_service		:	pEdit->SetWindowText(L"OutOfService");		break;
				case E87_LPTS::en_lp_transfer_blocked	:	pEdit->SetWindowText(L"TransferBlocked");	break;
				case E87_LPTS::en_lp_ready_to_load		:	pEdit->SetWindowText(L"ReadyToLoad");		break;
				case E87_LPTS::en_lp_ready_to_unload	:	pEdit->SetWindowText(L"ReadyToUnload");		break;
				case E87_LPTS::en_lp_in_service			:	pEdit->SetWindowText(L"InService");			break;
				case E87_LPTS::en_lp_transfer_ready		:	pEdit->SetWindowText(L"TransferReady");		break;
				}
				break;

			case E87_CDSM::en_e87cidsm	:
				pEdit	= (CEdit*)GetDlgItem(IDC_CIM3_EDT_ID_VERIFICATION);
				switch (event->evt_data.e87_sc.state)
				{
				case E87_CIVS::en_id_not_read				:	pEdit->SetWindowText(L"Not Read");				break;
				case E87_CIVS::en_id_wait_for_host			:	pEdit->SetWindowText(L"Wait for host");			break;
				case E87_CIVS::en_id_verification_ok		:	pEdit->SetWindowText(L"Verification OK");		break;
				case E87_CIVS::en_id_verification_failed	:	pEdit->SetWindowText(L"Verification Failed");	break;
				}
				break;
			case E87_CDSM::en_e87csmsm	:
				pEdit	= (CEdit*)GetDlgItem(IDC_CIM3_EDT_SLOT_VERIFICATION);
				switch (event->evt_data.e87_sc.state)
				{
				case E87_SMVS::en_sm_not_read				:	pEdit->SetWindowText(L"Not Read");				break;
				case E87_SMVS::en_sm_wait_for_host			:	pEdit->SetWindowText(L"Wait for host");			break;
				case E87_SMVS::en_sm_verification_ok		:	pEdit->SetWindowText(L"Verification OK");		break;
				case E87_SMVS::en_sm_verification_failed	:	pEdit->SetWindowText(L"Verification Failed");	break;
				}
				break;

			case E87_CDSM::en_e87lprsm	:	/* load port reservation state machine */
				pEdit	= (CEdit*)GetDlgItem(IDC_CIM3_EDT_RESERVE_STATE);
				switch (event->evt_data.e87_sc.state)
				{
				case E87_LPRS::en_lp_not_reserved	:	pEdit->SetWindowText(L"Not reserved");	break;
				case E87_LPRS::en_lp_reserved		:	pEdit->SetWindowText(L"Reserved");		break;
				}
				break;

			case E87_CDSM::en_e87lpcasm	:	/* load port/carrier association state machine */
				pEdit	= (CEdit*)GetDlgItem(IDC_CIM3_EDT_ASSOCIATE_STATE);
				switch (event->evt_data.e87_sc.state)
				{
				case E87_LPAS::en_not_associated	:	pEdit->SetWindowText(L"Not Associated");	break;
				case E87_LPAS::en_associated		:	pEdit->SetWindowText(L"Associated");		break;
				}
				break;
			}
			break;
		}
		break;

	case ENG_ESSC::en_e40_pjm	:
		switch (event->cbf_id)
		{
		case E40_CEFI::en_pr_state_change	:
			switch (event->evt_data.e40_ps.state)
			{
			case E40_PPJS::en_created				:	swprintf_s(tzMesg, 256, L"[%s] created",				event->str_data1);	break;	
			case E40_PPJS::en_queued				:	swprintf_s(tzMesg, 256, L"[%s] queued",					event->str_data1);	break;
			case E40_PPJS::en_setting_up			:	swprintf_s(tzMesg, 256, L"[%s] setting_up",				event->str_data1);	break;
			case E40_PPJS::en_waiting_for_start		:	swprintf_s(tzMesg, 256, L"[%s] waiting_for_start",		event->str_data1);	break;
			case E40_PPJS::en_processing			:	swprintf_s(tzMesg, 256, L"[%s] processing",				event->str_data1);	break;
			case E40_PPJS::en_process_completed		:	swprintf_s(tzMesg, 256, L"[%s] process_completed",		event->str_data1);	break;
			case E40_PPJS::en_reserved5				:	swprintf_s(tzMesg, 256, L"[%s] reserved5",				event->str_data1);	break;
			case E40_PPJS::en_pausing				:	swprintf_s(tzMesg, 256, L"[%s] pausing",				event->str_data1);	break;	
			case E40_PPJS::en_paused				:	swprintf_s(tzMesg, 256, L"[%s] paused",					event->str_data1);	break;
			case E40_PPJS::en_stopping				:	swprintf_s(tzMesg, 256, L"[%s] stopping",				event->str_data1);	break;
			case E40_PPJS::en_aborting				:	swprintf_s(tzMesg, 256, L"[%s] aborting",				event->str_data1);	break;
			case E40_PPJS::en_stopped				:	swprintf_s(tzMesg, 256, L"[%s] stopped",				event->str_data1);	break;	
			case E40_PPJS::en_aborted				:	swprintf_s(tzMesg, 256, L"[%s] aborted",				event->str_data1);	break;	
			case E40_PPJS::en_processjob_completed	:	swprintf_s(tzMesg, 256, L"[%s] processjob_completed",	event->str_data1);	break;
			}
			((CEdit *)GetDlgItem(IDC_CIM3_EDT_LAST_PRJOB_STATE))->SetWindowText(tzMesg);
			OnBnClickedProcJobAllGet(); //[Ian] �� �ڵ�� �Ź� 'Get All PR Job' ��ư�� ������ �ʰ� �ڵ����� PR ���� ȭ�� ������
			break;
		}
		break;
	}
}

/*
 desc : �̺�Ʈ ID
 parm : None
 retn : None
*/
VOID CDlgCIM3::OnChkClicked(UINT32 id)
{
}

/*
 desc : Carrier Transfer Start (ĳ���� �̵� ���� �˸�)
 parm : None
 retn : None
 note : 1. Place a Carrier on a loadport (Sequence Name)
		2. Remove Carrier From Loadport (Sequnce Name)
*/
VOID CDlgCIM3::OnBnClickedCarrierTransferStart()
{
	TCHAR tzPortID[CIM_VAL_ASCII_SIZE]	= {NULL};
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	if (_wtoi(tzPortID) < 1)	return;
	uvCIMLib_E87CMS_SetTransferStart((UINT16)_wtoi(tzPortID));
}

/*
 desc : Carrier Placed (ĳ���� �̵� �Ϸ� �˸�)
 parm : None
 retn : None
 note : Load Transfer Complete
*/
VOID CDlgCIM3::OnBnClickedCarrierPlaced()
{
	TCHAR tzPortID[CIM_VAL_ASCII_SIZE]	= {NULL};
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	if (_wtoi(tzPortID) < 1)	return;
	uvCIMLib_E87CMS_SetCarrierPlaced((UINT16)_wtoi(tzPortID));
}

/*
 desc : Material Received (��, FOUP�� ����)
 parm : None
 retn : None
*/
VOID CDlgCIM3::OnBnClickedMaterialReceived()
{
	TCHAR tzPortID[CIM_VAL_ASCII_SIZE] = {NULL};
	PTCHAR ptzUndockName= NULL;
	UNG_CVVT unCVVT;
	CStringArray arrVarName;
	CAtlList <UNG_CVVT> lstVarVal;
	CAtlList <E30_GPVT> lstType;

	((CEdit*)GetDlgItem(IDC_CIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	ptzUndockName	= uvCIMLib_GetLoadPortDockedName((UINT16)_wtoi(tzPortID), 0x01);

	arrVarName.Add(L"PortID");		/* PortID */
	arrVarName.Add(L"LocationID");	/* LocationID */

	unCVVT.u8_value	= (UINT8)_wtoi(tzPortID);
	lstVarVal.AddTail(unCVVT);
	swprintf_s(unCVVT.s_value, CIM_VAL_ASCII_SIZE, L"%s", ptzUndockName);
	lstVarVal.AddTail(unCVVT);

	lstType.AddTail(E30_GPVT::en_u1);
	lstType.AddTail(E30_GPVT::en_a);

	/* ����� ���� Trigger Event ���� */
	uvCIMLib_SetTrigEventValuesName(COMMN_CONN_ID, L"MaterialReceived", arrVarName, lstVarVal, lstType);

	arrVarName.RemoveAll();
	lstType.RemoveAll();
	lstVarVal.RemoveAll();
}

/*
 desc : Carrier At Port
 parm : None
 retn : None
*/
VOID CDlgCIM3::OnBnClickedCarrierAtPort()
{
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL}, tzPortID[CIM_VAL_ASCII_SIZE] = {NULL};
	PTCHAR ptzUndockName= NULL;
	UINT16 u16PortID	= 0;

	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_VAL_ASCII_SIZE);
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	u16PortID		= (UINT16)_wtoi(tzPortID);
	ptzUndockName	= uvCIMLib_GetLoadPortDockedName((UINT16)_wtoi(tzPortID), 0x01);
	if (wcslen(tzCarrID) < 1)	return;
	uvCIMLib_E87CMS_SetCarrierAtPort(tzCarrID, u16PortID, ptzUndockName);
}

/*
 desc : Carrier Clampled (ĳ���� ���� �Ϸ� �˸�)
 parm : None
 retn : None
*/
VOID CDlgCIM3::OnBnClickedCarrierClamped()
{
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]		= {NULL}, tzPortID[CIM_VAL_ASCII_SIZE] = {NULL}, tzLocID[CIM_VAL_ASCII_SIZE] = {NULL};
	PTCHAR ptzUndockName	= NULL;
	CStringArray arrVarID;
	UNG_CVVT unValue		= {NULL};
	CAtlList <UNG_CVVT> lstValue;
	CAtlList <E30_GPVT> lstType;

	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_VAL_ASCII_SIZE);
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	ptzUndockName	= uvCIMLib_GetLoadPortDockedName((UINT16)_wtoi(tzPortID), 0x01);
	if (wcslen(tzCarrID) < 1)	return;

	memset(&unValue, 0x00, sizeof(UNG_CVVT));
	arrVarID.Add(L"PortID");
	unValue.u8_value	= (UINT8)_wtoi(tzPortID);
	lstValue.AddTail(unValue);
	lstType.AddTail(E30_GPVT::en_u1);

	memset(&unValue, 0x00, sizeof(UNG_CVVT));
	arrVarID.Add(L"CarrierID");
	swprintf_s(unValue.s_value, CIM_VAL_ASCII_SIZE, L"%s", tzCarrID);
	lstValue.AddTail(unValue);
	lstType.AddTail(E30_GPVT::en_a);

	memset(&unValue, 0x00, sizeof(UNG_CVVT));
	arrVarID.Add(L"LocationID");
	swprintf_s(unValue.s_value, CIM_VAL_ASCII_SIZE, L"%s", ptzUndockName);
	lstValue.AddTail(unValue);
	lstType.AddTail(E30_GPVT::en_a);

	uvCIMLib_SetTrigEventValuesName(COMMN_CONN_ID		/* host_id for common */,
									L"CarrierClamped"	/*870006*/,
									arrVarID			/* Common Variables : PortID */,
									lstValue,
									lstType				/*AppendValueU1*/);

	arrVarID.RemoveAll();
	lstValue.RemoveAll();
	lstType.RemoveAll();
}

/*
 desc : Carrier Move - Dock
 parm : None
 retn : None
*/
VOID CDlgCIM3::OnBnClickedCarrierMoveDock()
{
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL}, tzPortID[CIM_VAL_ASCII_SIZE] = {NULL};
	PTCHAR ptzDockName	= NULL;
	UINT16 u16PortID	= 0;
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_VAL_ASCII_SIZE);
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	ptzDockName	= uvCIMLib_GetLoadPortDockedName((UINT16)_wtoi(tzPortID), 0x01);
	u16PortID	= (UINT16)_wtoi(tzPortID);
	if (wcslen(tzCarrID) < 1)	return;
	uvCIMLib_E87CMS_SetMoveCarrier(tzCarrID, u16PortID, ptzDockName);
}

/*
 desc : Carrier Departed Port (�ɸ�� �ε� ��Ʈ���� �и�)
 parm : None
 retn : None
 note : Unload Transfer Complete (Sequence Name)
*/
VOID CDlgCIM3::OnBnClickedCarrierDepartedPort()
{
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL}, tzPortID[CIM_VAL_ASCII_SIZE] = {NULL};
	PTCHAR ptzUndockName= NULL;
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_VAL_ASCII_SIZE);
	ptzUndockName	= uvCIMLib_GetLoadPortDockedName((UINT16)_wtoi(tzPortID), 0x01);
	if (_wtoi(tzPortID) < 1)	return;
	uvCIMLib_E87CMS_SetCarrierDepartedPort(tzCarrID, (UINT16)_wtoi(tzPortID), ptzUndockName);
}

/*
 desc : Carrier Unclampled (ĳ���� ���� ���� �˸�)
 parm : None
 retn : None
*/
VOID CDlgCIM3::OnBnClickedCarrierUnclamped()
{
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL}, tzPortID[CIM_VAL_ASCII_SIZE] = {NULL};
	UNG_CVVT unPortID	= {NULL};
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_VAL_ASCII_SIZE);
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	unPortID.u8_value	= (UINT8)_wtoi(tzPortID);
	if (wcslen(tzCarrID) < 1)	return;
#if 0	/* ���� �ܺ� �Լ��� ��ü */
	uvCIMLib_SetTrigEventValueName(COMMN_CONN_ID		/* host_id for common */,
								   L"CarrierUnclamped"	/* Common Events    : CarrierUnclamped */,
								   L"PortID"			/* Common Variables : PortID */,
								   &unPortID,
								   E30_GPVT::en_u1/*AppendValueU1*/);
#else
	uvCIMLib_E87CMS_SetCarrierEvent(COMMN_CONN_ID, tzCarrID, unPortID.u8_value, E87_CCES::en_carrier_unclamped);
#endif
}

/*
 desc : Material Removed (��, ��ũ ���̺��� ����)
 parm : None
 retn : None
 note : Carrier Transfer Blocked (Sequence Name)
*/
VOID CDlgCIM3::OnBnClickedMaterialRemoved()
{
	PTCHAR ptzUndockName	= NULL;
	CStringArray arrVarName;
	CAtlList <UNG_CVVT> lstVarVal;
	CAtlList <E30_GPVT> lstType;
	UNG_CVVT unData;
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL};
	TCHAR tzPortID[CIM_VAL_ASCII_SIZE]	= {NULL};
	CUniToChar csCnv;

	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_VAL_ASCII_SIZE);
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	ptzUndockName	= uvCIMLib_GetLoadPortDockedName((UINT16)_wtoi(tzPortID), 0x00);

	arrVarName.Add(L"PortID");		/* PortID */
	arrVarName.Add(L"CarrierID");	/* CarrierID */
	arrVarName.Add(L"LocationID");	/* LocationID */
	
	lstType.AddTail(E30_GPVT::en_u1);
	lstType.AddTail(E30_GPVT::en_a);
	lstType.AddTail(E30_GPVT::en_a);

	unData.u8_value	= (UINT8)_wtoi(tzPortID);
	lstVarVal.AddTail(unData);
	swprintf_s(unData.s_value, CIM_VAL_ASCII_SIZE, L"%s", tzCarrID);
	lstVarVal.AddTail(unData);
	swprintf_s(unData.s_value, CIM_VAL_ASCII_SIZE, L"%s", ptzUndockName);
	lstVarVal.AddTail(unData);

	/* ����� ���� Trigger Event ���� */
	uvCIMLib_SetTrigEventValuesName(COMMN_CONN_ID, L"MaterialRemoved", arrVarName, lstVarVal, lstType);

	arrVarName.RemoveAll();
	lstType.RemoveAll();
	lstVarVal.RemoveAll();
}

/*
 desc : Carrier Move - Undock
 parm : None
 retn : None
*/
VOID CDlgCIM3::OnBnClickedCarrierMoveUndock()
{
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL}, tzPortID[CIM_VAL_ASCII_SIZE] = {NULL};
	UINT16 u16PortID	= 0;
	PTCHAR ptzUndockName= NULL;

	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_VAL_ASCII_SIZE);
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	u16PortID		= (UINT16)_wtoi(tzPortID);
	if (wcslen(tzCarrID) < 1)	return;
	ptzUndockName	= uvCIMLib_GetLoadPortDockedName((UINT16)_wtoi(tzPortID), 0x01);
	uvCIMLib_E87CMS_SetMoveCarrier(tzCarrID, u16PortID, ptzUndockName);
}

/*
 desc : Carrier Opened
 parm : None
 retn : None
*/
VOID CDlgCIM3::OnBnClickedCarrierOpened()
{
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL}, tzPortID[CIM_VAL_ASCII_SIZE] = {NULL};
	PTCHAR ptzDockName	= NULL;

	CStringArray arrVarName;
	CAtlList <UNG_CVVT> lstVarVal;
	CAtlList <E30_GPVT> lstType;
	UNG_CVVT unValue	= {NULL};

	((CEdit*)GetDlgItem(IDC_CIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_VAL_ASCII_SIZE);
	ptzDockName	= uvCIMLib_GetLoadPortDockedName((UINT16)_wtoi(tzPortID), 0x00);

	arrVarName.Add(L"PortID");		/* PortID */
	arrVarName.Add(L"CarrierID");	/* CarrierID */
	arrVarName.Add(L"LocationID");	/* LocationID */
	lstType.AddTail(E30_GPVT::en_u1);
	lstType.AddTail(E30_GPVT::en_a);
	lstType.AddTail(E30_GPVT::en_a);
	unValue.u8_value	= (UINT8)_wtoi(tzPortID);
	lstVarVal.AddTail(unValue);
	swprintf_s(unValue.s_value, CIM_VAL_ASCII_SIZE, L"%s", tzCarrID);
	lstVarVal.AddTail(unValue);
	swprintf_s(unValue.s_value, CIM_VAL_ASCII_SIZE, L"%s", ptzDockName);
	lstVarVal.AddTail(unValue);

	/* ����� ���� Trigger Event ���� */
	uvCIMLib_SetTrigEventValuesName(COMMN_CONN_ID, L"CarrierOpened", arrVarName, lstVarVal, lstType);

	arrVarName.RemoveAll();
	lstType.RemoveAll();
	lstVarVal.RemoveAll();
}

/*
 desc : Carrier Opened
 parm : None
 retn : None
*/
VOID CDlgCIM3::OnBnClickedCarrierClosed()
{
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL}, tzPortID[CIM_VAL_ASCII_SIZE] = {NULL};
	PTCHAR ptzDockName	= NULL;

	CStringArray arrVarName;
	CAtlList <UNG_CVVT> lstVarVal;
	CAtlList <E30_GPVT> lstType;
	UNG_CVVT unValue	= {NULL};

	((CEdit*)GetDlgItem(IDC_CIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_VAL_ASCII_SIZE);
	ptzDockName	= uvCIMLib_GetLoadPortDockedName((UINT16)_wtoi(tzPortID), 0x00);

	arrVarName.Add(L"PortID");		/* PortID */
	arrVarName.Add(L"CarrierID");	/* CarrierID */
	arrVarName.Add(L"LocationID");	/* LocationID */
	lstType.AddTail(E30_GPVT::en_u1);
	lstType.AddTail(E30_GPVT::en_a);
	lstType.AddTail(E30_GPVT::en_a);
	unValue.u8_value	= (UINT8)_wtoi(tzPortID);
	lstVarVal.AddTail(unValue);
	swprintf_s(unValue.s_value, CIM_VAL_ASCII_SIZE, L"%s", tzCarrID);
	lstVarVal.AddTail(unValue);
	swprintf_s(unValue.s_value, CIM_VAL_ASCII_SIZE, L"%s", ptzDockName);
	lstVarVal.AddTail(unValue);

	/* ����� ���� Trigger Event ���� */
	uvCIMLib_SetTrigEventValuesName(COMMN_CONN_ID, L"CarrierClosed", arrVarName, lstVarVal, lstType);

	arrVarName.RemoveAll();
	lstType.RemoveAll();
	lstVarVal.RemoveAll();
}

/*
 desc : Slot Map �б�
 parm : None
 retn : None
*/
VOID CDlgCIM3::OnBnClickedReadSlotMap()
{
	UINT8 i	= 0, j = 0, u8Skip=1, u8Stat=(UINT8)E87_CSMS::en_correctly_occupied;
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL};
	UINT8 u8RandAll, u8RandSkip;
	CAtlList <E87_CSMS> lstSlotMap;

	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_VAL_ASCII_SIZE);

	u8RandAll	= (UINT8)((CButton*)GetDlgItem(IDC_CIM3_CHK_SLOP_RANDOM_ALL))->GetCheck();
	u8RandSkip	= (UINT8)((CButton*)GetDlgItem(IDC_CIM3_CHK_SLOP_RANDOM_SKIP))->GetCheck();

	for (i=0; i<25; i++)
	{
		if (u8RandAll)	u8Stat	= uvCmn_GetRandNumer(0x00, 3) % 6;
		if (!u8RandSkip)
		{
			lstSlotMap.AddTail(E87_CSMS(u8Stat));
		}
		else
		{
			u8Skip	= uvCmn_GetRandNumer(0x00, 3) % 3;
			for (j=0; j<u8Skip; j++,i++)
			{
				lstSlotMap.AddTail(E87_CSMS(E87_CSMS::en_empty));
			}
			if (i<25)
			{
				lstSlotMap.AddTail(E87_CSMS(u8Stat));
			}
		}
	}
	if (wcslen(tzCarrID) < 1)	return;
	/* Slot Map ���� */
	uvCIMLib_E87CMS_SetVerifySlotMap(tzCarrID, &lstSlotMap);
	/* �޸� ��ȯ */
	lstSlotMap.RemoveAll();
}

/*
 desc : Carrier ReadyToUnload
 parm : None
 retn : None
*/
VOID CDlgCIM3::OnBnClickedCarrierReadyUnload()
{
	TCHAR tzPortID[8]	= {NULL};
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, 8);
	if (_wtoi(tzPortID) < 1)	return;
	uvCIMLib_E87CMS_SetReadyToUnload((UINT16)_wtoi(tzPortID));
}

/*
 desc : Process Job Start Process
 parm : None
 retn : None
*/
VOID CDlgCIM3::OnBnClickedPRJobStartProcess()
{
	TCHAR tzCtrlID[128]	= {NULL};
	TCHAR tzProcID[128]	= {NULL};
#if 0
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_PROCESS_JOB_ID))->GetWindowText(tzProcID, 128);
#else
	/* ���õ� Control Job ID ���� */
	if (!uvCIMLib_E94CJM_GetControlJobFirstID(E94_CCJS::en_selected, tzCtrlID, 128))	return;
#endif
	if (wcslen(tzProcID) < 1)	return;
	uvCIMLib_E40PJM_SetPRJobStartProcess(tzProcID);
//	uvCIMLib_SetPRJobAbortComplete(tzProcID);
}

/*
 desc : Carrier�κ��� Substrate ID �Ѱ� ���� (Pop)
 parm : None
 retn : None
*/
VOID CDlgCIM3::OnBnClickedSubstrateCarrierRemoved()
{
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL};
	TCHAR tzSlotNum[CIM_VAL_ASCII_SIZE]	= {NULL};

	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_VAL_ASCII_SIZE);
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_SLOT_NUMBER))->GetWindowText(tzSlotNum, CIM_VAL_ASCII_SIZE);
	if (wcslen(tzCarrID) < 1)	return;
	if (wcslen(tzSlotNum) < 1)	return;
	uvCIMLib_E87CMS_SetCarrierPopSubstrate(tzCarrID, (UINT8)_wtoi(tzSlotNum));
}

/*
 desc : Substrate ��ġ ���� - Robot
 parm : mode	- [in]  0x00 : need_processing, 0x01 : process, 0x02 : proceessed
 retn : None
*/
VOID CDlgCIM3::SetSubstrateChangeRobotArm(UINT8 mode)
{
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL};
	TCHAR tzSlotNum[CIM_VAL_ASCII_SIZE]	= {NULL};
	TCHAR tzSubstID[CIM_VAL_ASCII_SIZE]	= {NULL};
	E90_SSPS enSSPS		= E90_SSPS::en_needs_processing;

	if (mode == 0x01)		enSSPS	= E90_SSPS::en_in_process;
	else if (mode == 0x01)	enSSPS	= E90_SSPS::en_processed;

	/* ���� Slot Number�� �ٽ� ��ȯ�� Substrate ID ��� */
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_VAL_ASCII_SIZE);
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_SLOT_NUMBER))->GetWindowText(tzSlotNum, CIM_VAL_ASCII_SIZE);
	uvCIMLib_Mixed_GetSubstrateDestinationID(tzCarrID, (UINT8)_wtoi(tzSlotNum), tzSubstID, CIM_VAL_ASCII_SIZE);

	uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, ENG_TSSI::en_robot, E90_SSTS::en_work, enSSPS);
}

/*
 desc : Substrate ��ġ ���� - PreAligner
 parm : None
 retn : None
*/
VOID CDlgCIM3::OnBnClickedSubstrateChangePreAligner()
{
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL};
	TCHAR tzSlotNum[CIM_VAL_ASCII_SIZE]	= {NULL};
	TCHAR tzSubstID[CIM_VAL_ASCII_SIZE]	= {NULL};

	/* ���� Slot Number�� �ٽ� ��ȯ�� Substrate ID ��� */
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_VAL_ASCII_SIZE);
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_SLOT_NUMBER))->GetWindowText(tzSlotNum, CIM_VAL_ASCII_SIZE);
	uvCIMLib_Mixed_GetSubstrateDestinationID(tzCarrID, (UINT8)_wtoi(tzSlotNum), tzSubstID, CIM_VAL_ASCII_SIZE);
	uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, ENG_TSSI::en_pre_aligner, E90_SSTS::en_work, E90_SSPS::en_needs_processing);
}

/*
 desc : Substrate ��ġ ���� - ProcessChamber
 parm : None
 retn : None
*/
VOID CDlgCIM3::OnBnClickedSubstrateChangeChamber()
{
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL};
	TCHAR tzSlotNum[CIM_VAL_ASCII_SIZE]	= {NULL};
	TCHAR tzSubstID[CIM_VAL_ASCII_SIZE]	= {NULL};

	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_VAL_ASCII_SIZE);
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_SLOT_NUMBER))->GetWindowText(tzSlotNum, CIM_VAL_ASCII_SIZE);

	/* ���� Slot Number�� �ٽ� ��ȯ�� Substrate ID ��� */
	uvCIMLib_Mixed_GetSubstrateDestinationID(tzCarrID, (UINT8)_wtoi(tzSlotNum), tzSubstID, CIM_VAL_ASCII_SIZE);
	uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, ENG_TSSI::en_process_chamber, E90_SSTS::en_work, E90_SSPS::en_needs_processing);
}

/*
 desc : Substrate ��ġ ���� - DestinationID
 parm : None
 retn : None
*/
VOID CDlgCIM3::OnBnClickedSubstrateCarrierInsert()
{
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL};
	TCHAR tzSubstID[CIM_VAL_ASCII_SIZE]	= {NULL};
	TCHAR tzSlotNum[CIM_VAL_ASCII_SIZE]	= {NULL}, tzDestID[CIM_SUBSTRATE_ID] = {NULL};

	/* ���� Slot Number�� �ٽ� ��ȯ�� Substrate ID ��� */
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_VAL_ASCII_SIZE);
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_SLOT_NUMBER))->GetWindowText(tzSlotNum, CIM_VAL_ASCII_SIZE);
	uvCIMLib_Mixed_GetSubstrateDestinationID(tzCarrID, (UINT8)_wtoi(tzSlotNum), tzSubstID, CIM_VAL_ASCII_SIZE);
	/* Change */
	uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, ENG_TSSI::en_robot, E90_SSTS::en_work, E90_SSPS::en_processed);
	/* ������ Substrate�� ��ȯ�� ��ġ ��� */
	wmemset(tzDestID, 0x00, CIM_SUBSTRATE_ID);
	uvCIMLib_Mixed_GetSubstrateDestinationID(tzCarrID, (UINT8)_wtoi(tzSlotNum), tzDestID, CIM_SUBSTRATE_ID);
	/* Insert */
	uvCIMLib_E87CMS_SetSubstratePlaced(tzCarrID, tzDestID, (UINT8)_wtoi(tzSlotNum));
}

/*
 desc : ���� �۾� ���� Carrier ���� �� (Accessing)
 parm : None
 retn : None
*/
VOID CDlgCIM3::OnBnClickedCarrierAccessing()
{
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL};
	TCHAR tzPortID[CIM_VAL_ASCII_SIZE]	= {NULL};

	/* ���� Slot Number�� �ٽ� ��ȯ�� Substrate ID ��� */
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_VAL_ASCII_SIZE);
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	uvCIMLib_E87CMS_SetAccessCarrier(COMMN_CONN_ID, tzCarrID, (UINT16)_wtoi(tzPortID),
									 E87_CCAS::en_carrier_in_access);
}

/*
 desc : ���� �۾� ���� Carrier �Ϸ� (?)
 parm : None
 retn : None
*/
VOID CDlgCIM3::OnBnClickedCarrierAccessComplete()
{
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL};
	TCHAR tzPortID[CIM_VAL_ASCII_SIZE]	= {NULL};

	/* ���� Slot Number�� �ٽ� ��ȯ�� Substrate ID ��� */
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_VAL_ASCII_SIZE);
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	uvCIMLib_E87CMS_SetAccessCarrier(COMMN_CONN_ID, tzCarrID, (UINT16)_wtoi(tzPortID),
									 E87_CCAS::en_carrier_complete);
}

/*
 desc : ���� Control Job�� �����ϱ�
 parm : None
 retn : None
*/
VOID CDlgCIM3::OnBnClickedSelectedControlJob()
{
	TCHAR tzCtrlID[CIM_CONTROL_JOB_ID_SIZE]	= {NULL};
	POSITION pPos	= m_lst_ctrl.GetFirstSelectedItemPosition();
	if (!pPos)	return;
	INT32 i32Sel	= m_lst_ctrl.GetNextSelectedItem(pPos);
	if (i32Sel < 0)	return;
	m_lst_ctrl.GetItemText(i32Sel, 0, tzCtrlID, CIM_CONTROL_JOB_ID_SIZE);
	uvCIMLib_E94CJM_SetSelectedControlJob(tzCtrlID);
}

/*
 desc : ���� Control Job�� �����ϱ�
 parm : None
 retn : None
*/
VOID CDlgCIM3::OnBnClickedDeselectedControlJob()
{
	TCHAR tzCtrlID[CIM_CONTROL_JOB_ID_SIZE]	= {NULL};
	POSITION pPos	= m_lst_ctrl.GetFirstSelectedItemPosition();
	if (!pPos)	return;
	INT32 i32Sel	= m_lst_ctrl.GetNextSelectedItem(pPos);
	if (i32Sel < 0)	return;
	m_lst_ctrl.GetItemText(i32Sel, 0, tzCtrlID, CIM_CONTROL_JOB_ID_SIZE);
	uvCIMLib_E94CJM_SetDeselectedControlJob(tzCtrlID);
}

void CDlgCIM3::OnBnClickedProcWithCarr()
{
	TCHAR tzPortID[CIM_VAL_ASCII_SIZE]	= {NULL};
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL};

	((CEdit*)GetDlgItem(IDC_CIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_VAL_ASCII_SIZE);
	uvCIMLib_E87CMS_SetProceedWithCarrier((UINT16)_wtoi(tzPortID), tzCarrID);
}

void CDlgCIM3::OnBnClickedCancelCarrier()
{
	TCHAR tzPortID[CIM_VAL_ASCII_SIZE]	= {NULL};
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL};

	((CEdit*)GetDlgItem(IDC_CIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_VAL_ASCII_SIZE);
	uvCIMLib_E87CMS_SetCancelCarrier((UINT16)_wtoi(tzPortID), tzCarrID);
}

void CDlgCIM3::OnBnClickedCarrReCreate()
{
	TCHAR tzPortID[CIM_VAL_ASCII_SIZE]	= {NULL};
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL};
	E87_LPTS enTransState;
	E87_LPAS enPortCarrierState;

	((CEdit*)GetDlgItem(IDC_CIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_VAL_ASCII_SIZE);

	if (!uvCIMLib_E87CMS_GetTransferStatus((UINT16)_wtoi(tzPortID), enTransState))	return;
	if (enTransState != E87_LPTS::en_lp_ready_to_unload)
	{
		AfxMessageBox(L"The carrier is not [Ready To Unload] status", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	if (!uvCIMLib_E87CMS_GetAssociationState((UINT16)_wtoi(tzPortID), enPortCarrierState))	return;
	if (enPortCarrierState == E87_LPAS::en_associated)
	{
		if (!uvCIMLib_E87CMS_SetCarrierReCreate((UINT16)_wtoi(tzPortID), tzCarrID))	return;
	}

	/* �˶� Clear ? Which ? */

	/* Load Port ���� ���� : Loading ... */

	/* TransferStart ȣ�� */
	if (!uvCIMLib_E87CMS_SetTransferStart((UINT16)_wtoi(tzPortID)))	return;
	/* Carrier Placed */
	if (!uvCIMLib_E87CMS_SetCarrierPlaced((UINT16)_wtoi(tzPortID)))	return;
}

void CDlgCIM3::OnBnClickedReleaseCarrier()
{
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL};
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_VAL_ASCII_SIZE);
	uvCIMLib_E87CMS_SetCarrierRelease(tzCarrID);
}


void CDlgCIM3::OnBnClickedProcWithSubst()
{
	TCHAR tzSubstID[CIM_VAL_ASCII_SIZE]	= {NULL};
	((CEdit*)GetDlgItem(IDC_CIM3_EDT_CARR_SLOT_NUMBER))->GetWindowText(tzSubstID, CIM_VAL_ASCII_SIZE);
	uvCIMLib_E90STS_SetProceedWithSubstrate(tzSubstID);
}


void CDlgCIM3::OnBnClickedCim3BtnLoadMaterialList()
{
//	uvCIMLib_E40PJM_GetPRMtlNameList
}


void CDlgCIM3::OnBnClickedProcJobAllGet()
{
	UINT32 i	= 0;
	TCHAR tzCtrlID[CIM_CONTROL_JOB_ID_SIZE]	= {NULL};
	CStringArray strArrProcID;
	CAtlList<E40_PPJS> lstState;

	/* ���� ��ϵ� ��� ���μ��� JOB ���� */
	if (!uvCIMLib_E40PJM_GetProcessAllJobIdState(strArrProcID, lstState))	return;

	m_lst_proc.SetRedraw(FALSE);
	m_lst_proc.DeleteAllItems();

	for (i=0; i<strArrProcID.GetCount(); i++)
	{
		wmemset(tzCtrlID, 0x00, CIM_CONTROL_JOB_ID_SIZE);
		if (uvCIMLib_E40PJM_GetControlJobID((LPTSTR)strArrProcID[i].GetBuffer(), tzCtrlID,
			CIM_CONTROL_JOB_ID_SIZE))
		{
			m_lst_proc.InsertItem(i, tzCtrlID);
			m_lst_proc.SetItemText(i, 1, (LPTSTR)strArrProcID[i].GetBuffer());
			switch (lstState.GetAt(lstState.FindIndex(i)))
			{
			case E40_PPJS::en_created				:	m_lst_proc.SetItemText(i, 2, L"created");				break;	
			case E40_PPJS::en_queued				:	m_lst_proc.SetItemText(i, 2, L"queued");				break;
			case E40_PPJS::en_setting_up			:	m_lst_proc.SetItemText(i, 2, L"setting_up");			break;
			case E40_PPJS::en_waiting_for_start		:	m_lst_proc.SetItemText(i, 2, L"waiting_for_start");		break;
			case E40_PPJS::en_processing			:	m_lst_proc.SetItemText(i, 2, L"processing");			break;
			case E40_PPJS::en_process_completed		:	m_lst_proc.SetItemText(i, 2, L"process_completed");		break;
			case E40_PPJS::en_reserved5				:	m_lst_proc.SetItemText(i, 2, L"reserved5");				break;
			case E40_PPJS::en_pausing				:	m_lst_proc.SetItemText(i, 2, L"pausing");				break;	
			case E40_PPJS::en_paused				:	m_lst_proc.SetItemText(i, 2, L"paused");				break;
			case E40_PPJS::en_stopping				:	m_lst_proc.SetItemText(i, 2, L"stopping");				break;
			case E40_PPJS::en_aborting				:	m_lst_proc.SetItemText(i, 2, L"aborting");				break;
			case E40_PPJS::en_stopped				:	m_lst_proc.SetItemText(i, 2, L"stopped");				break;	
			case E40_PPJS::en_aborted				:	m_lst_proc.SetItemText(i, 2, L"aborted");				break;	
			case E40_PPJS::en_processjob_completed	:	m_lst_proc.SetItemText(i, 2, L"processjob_completed");	break;
			}
		}
	}

	m_lst_proc.SetRedraw(TRUE);
	m_lst_proc.Invalidate(TRUE);

	strArrProcID.RemoveAll();
	lstState.RemoveAll();
}


void CDlgCIM3::OnBnClickedCtrlJobAllGet()
{
	UINT32 i	= 0;
	CStringArray strArrCtrlID;
	CAtlList<E94_CCJS> lstState;

	/* ���� ��ϵ� ��� ���μ��� JOB ���� */

	if (!uvCIMLib_E94CJM_GetControlJobListIdState(strArrCtrlID, lstState, E94_CLCJ::en_list_all))	return;
	m_lst_ctrl.SetRedraw(FALSE);
	m_lst_ctrl.DeleteAllItems();
	for (i=0; i<lstState.GetCount(); i++)
	{
		m_lst_ctrl.InsertItem(i, strArrCtrlID[i].GetBuffer());
		/* Control Job State */
		switch (lstState.GetAt(lstState.FindIndex(i)))
		{
		case E94_CCJS::en_unknown			:	m_lst_ctrl.SetItemText(i, 1, L"unknown");			break;
		case E94_CCJS::en_queued			:	m_lst_ctrl.SetItemText(i, 1, L"queued");			break;
		case E94_CCJS::en_selected			:	m_lst_ctrl.SetItemText(i, 1, L"selected");			break;
		case E94_CCJS::en_waitingforstart	:	m_lst_ctrl.SetItemText(i, 1, L"waitingforstart");	break;
		case E94_CCJS::en_executing			:	m_lst_ctrl.SetItemText(i, 1, L"executing");			break;
		case E94_CCJS::en_paused			:	m_lst_ctrl.SetItemText(i, 1, L"paused");			break;
		case E94_CCJS::en_completed			:	m_lst_ctrl.SetItemText(i, 1, L"completed");			break;
		}
	}
	m_lst_ctrl.SetRedraw(TRUE);
	m_lst_ctrl.Invalidate(TRUE);

	strArrCtrlID.RemoveAll();
	lstState.RemoveAll();
}

/*
 desc : Process Job ID�� ���� ����
 parm : id	- [in]  ��ư ��Ʈ�� ID
 retn : Non
*/
VOID CDlgCIM3::OnBnClickedPRJobCtrl(UINT32 id)
{
	TCHAR tzPRJobID[CIM_PROCESS_JOB_ID_SIZE]	= {NULL};
	INT32 i	= 0, i32SelPos	= -1;
	E40_PPJC enMode	= E40_PPJC::en_abort;
	if (m_lst_proc.GetItemCount() < 1)	return;

	for (i=0; i<m_lst_proc.GetItemCount(); i++)
	{
		if (LVIS_SELECTED == (LVIS_SELECTED & m_lst_proc.GetItemState(i, LVIS_SELECTED)))
		{
			i32SelPos = i;
			break;
		}
	}
	if (i32SelPos < 0)	return;

	m_lst_proc.GetItemText(i32SelPos, 1, tzPRJobID, CIM_PROCESS_JOB_ID_SIZE);

	switch (id)
	{
	case IDC_CIM3_BTN_PROC_JOB_ABORT	:	enMode = E40_PPJC::en_abort;	break;
	case IDC_CIM3_BTN_PROC_JOB_CANCEL	:	enMode = E40_PPJC::en_cancel;	break; // [Ian] �����ִ� Cancel���� ��带 �߰��� ��
	case IDC_CIM3_BTN_PROC_JOB_PAUSE	:	enMode = E40_PPJC::en_pause;	break;
	case IDC_CIM3_BTN_PROC_JOB_RESUME	:	enMode = E40_PPJC::en_resume;	break;
	case IDC_CIM3_BTN_PROC_JOB_START	:	enMode = E40_PPJC::en_start;	break;
	case IDC_CIM3_BTN_PROC_JOB_STOP		:	enMode = E40_PPJC::en_stop;		break;
	default:	return;
	}
	uvCIMLib_E40PJM_SetPRJobStateControl(tzPRJobID, enMode);
}

void CDlgCIM3::OnBnClickedCim3BtnPrjobCompleted()
{
//	TCHAR tzProcID[128]	= {NULL};
//	((CEdit*)GetDlgItem(IDC_CIM3_EDT_PROCESS_JOB_ID))->GetWindowText(tzProcID, 128);
//	if (wcslen(tzProcID) < 1)	return;
//	uvCIMLib_E40PJM_SetPRJobComplete(tzProcID);
}
