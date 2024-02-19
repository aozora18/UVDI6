
/*
 desc : CIM300
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgSIM3.h"


IMPLEMENT_DYNAMIC(CDlgSIM3, CDialogEx)

/*
 desc : ������
 parm : None
 retn : None
*/
CDlgSIM3::CDlgSIM3(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSIM3::IDD, pParent)
{
	m_bExitProc	= FALSE;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDlgSIM3::~CDlgSIM3()
{
}

/* ��Ʈ�� ���� */
void CDlgSIM3::DoDataExchange(CDataExchange* dx)
{
	CDialogEx::DoDataExchange(dx);

	UINT32 i, u32StartID;

	u32StartID	= IDC_SIM3_LST_SUBST_ITEM;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_lst_ctl[i]);

}

/* �޽��� �� */
BEGIN_MESSAGE_MAP(CDlgSIM3, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SIM3_BTN_LOAD, &CDlgSIM3::OnBnClickedSim3BtnLoad)
	ON_BN_CLICKED(IDC_SIM3_BTN_UNLOAD, &CDlgSIM3::OnBnClickedSim3BtnUnload)
	ON_BN_CLICKED(IDC_SIM3_BTN_ACCESS, &CDlgSIM3::OnBnClickedSim3BtnAccess)
	ON_BN_CLICKED(IDC_SIM3_CHK_SLOT_MAP_RAND, &CDlgSIM3::OnBnClickedSim3ChkSlotMapRand)
	ON_BN_CLICKED(IDC_SIM3_CHK_SLOT_MAP_OCCUPIED, &CDlgSIM3::OnBnClickedSim3ChkSlotMapOccupied)
	ON_BN_CLICKED(IDC_SIM3_BTN_JOB_PROCESS, &CDlgSIM3::OnBnClickedSim3BtnJobProcess)
	ON_BN_CLICKED(IDC_SIM3_BTN_PROC_WITH_SUBST, &CDlgSIM3::OnBnClickedSim3BtnProcWithSubst)
	ON_BN_CLICKED(IDC_SIM3_BTN_PRJOB_COMPLETED, &CDlgSIM3::OnBnClickedSim3BtnPrjobCompleted)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_SIM3_BTN_EPT_BUSY, IDC_SIM3_BTN_EPT_IDLE, OnBnClickedExpose)
	ON_BN_CLICKED(IDC_SIM3_BTN_PRJOB_START, &CDlgSIM3::OnBnClickedSim3BtnPrjobStart)
	ON_BN_CLICKED(IDC_SIM3_BTN_RELEASE_CARRIER, &CDlgSIM3::OnBnClickedSim3BtnReleaseCarrier)
	ON_BN_CLICKED(IDC_SIM3_BTN_PRJOB_PROC_COMPLETE, &CDlgSIM3::OnBnClickedSim3BtnPrjobProcComplete)
	ON_BN_CLICKED(IDC_SIM3_BTN_PRJOB_STOP, &CDlgSIM3::OnBnClickedSim3BtnPrjobStop)
	ON_BN_CLICKED(IDC_SIM3_BTN_PRJOB_ABORT, &CDlgSIM3::OnBnClickedSim3BtnPrjobAbort)
	ON_BN_CLICKED(IDC_SIM3_BTN_PRJOB_REMOVE, &CDlgSIM3::OnBnClickedSim3BtnPrjobRemove)
	ON_LBN_SELCHANGE(IDC_SIM3_BOX_CRJOB, &CDlgSIM3::OnLbnSelchangeSim3BoxCrjob)
	ON_BN_CLICKED(IDC_SIM3_BTN_PRJOB_UPDATE, &CDlgSIM3::OnBnClickedSim3BtnPrjobUpdate)
	ON_BN_CLICKED(IDC_SIM3_BTN_CRJOB_UPDATE, &CDlgSIM3::OnBnClickedSim3BtnCrjobUpdate)
	ON_LBN_DBLCLK(IDC_SIM3_BOX_PRJOB, &CDlgSIM3::OnLbnDblclkSim3BoxPrjob)
	ON_BN_CLICKED(IDC_SIM3_BTN_CARR_RE_CREATE, &CDlgSIM3::OnBnClickedSim3BtnCarrReCreate)
	ON_BN_CLICKED(IDC_SIM3_BTN_CANCEL_CARRIER, &CDlgSIM3::OnBnClickedSim3BtnCancelCarrier)
	ON_BN_CLICKED(IDC_SIM3_BTN_LOAD2, &CDlgSIM3::OnBnClickedSim3BtnLoad2)
END_MESSAGE_MAP()

/*
 desc : ���̾�α� ���� �� �ʱ�ȭ
*/
BOOL CDlgSIM3::OnInitDialog()
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
VOID CDlgSIM3::OnClose()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnClose();
}
VOID CDlgSIM3::OnDestroy()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnDestroy();
}
VOID CDlgSIM3::ExitProc()
{
	/* �̹� ������ ȣ������� ���� */
	m_bExitProc	= TRUE;

}

/*
 desc : ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgSIM3::InitCtrl()
{
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CARR_ID))->SetWindowText(L"CARRIER1");
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_LOAD_PORT_ID))->SetWindowText(L"1");
	
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_BUFFER_LOAD))->SetWindowText(L"empty");
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_ALIGNER_LOAD))->SetWindowText(L"empty");
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_ROBOT_LOAD))->SetWindowText(L"empty");
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_STAGE_LOAD))->SetWindowText(L"empty");
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_WAFER_COUNT))->SetWindowText(L"3");	

	/* Control Job IDs */
	m_lst_ctl[0].InsertColumn(1, L"SID", LVCFMT_CENTER, 95);
	m_lst_ctl[0].InsertColumn(2, L"MapState", LVCFMT_LEFT, 80);
	m_lst_ctl[0].InsertColumn(3, L"Process", LVCFMT_LEFT, 120);
	m_lst_ctl[0].InsertColumn(4, L"ID State", LVCFMT_LEFT, 110);
	m_lst_ctl[0].InsertColumn(5, L"ID Acquired", LVCFMT_LEFT, 95);
	m_lst_ctl[0].InsertColumn(6, L"Scan", LVCFMT_LEFT, 40);
	m_lst_ctl[0].InsertColumn(7, L"DIT", LVCFMT_LEFT, 95);
	m_lst_ctl[0].SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	((CButton*)GetDlgItem(IDC_SIM3_CHK_SLOT_MAP_OCCUPIED))->SetCheck(1);
}

/*
 desc : �ֱ������� ȣ��Ǵ� �Լ�
 parm : None
 retn : None
*/
VOID CDlgSIM3::UpdateCtrl()
{
#if 0
	INT32 i;
	TCHAR tzMesg[512]	= {NULL}, tzCarrID[CIM_CARRIED_ID_SIZE]	= {NULL};
	E87_LPTS enLPTS;
	E87_CIVS enCIOS;
	E87_SMVS enSMVS;
	CEdit *pEdit		= {NULL};

	/* Update : Load Port Transfer Status */
	for (i=0; i<2; i++)
	{
		pEdit	= (CEdit*)GetDlgItem(IDC_SIM3_EDT_TRANSER_STATE1+i);
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
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_CARRIED_ID_SIZE);
	if (wcslen(tzCarrID) > 0)
	{
		if (uvCIMLib_E87CMS_GetCarrierIDStatus(tzCarrID, enCIOS))
		{
			pEdit	= (CEdit*)GetDlgItem(IDC_SIM3_EDT_ID_VERIFICATION);
			pEdit->SetRedraw(FALSE);
			switch (enCIOS)
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
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_CARRIED_ID_SIZE);
	if (wcslen(tzCarrID) > 0)
	{
		if (uvCIMLib_E87CMS_GetCarrierIDSlotMapStatus(tzCarrID, enSMVS))
		{
			pEdit	= (CEdit*)GetDlgItem(IDC_SIM3_EDT_SLOT_VERIFICATION);
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
	/* ���� ó���ϰ� �ִ� Location ��ġ ���� ��� */
	UpdateSubstrateLocation();
	/* ���� ó���ϰ� �ִ� Processing�� ���� ���� ��� */
	UpdateProcessingState();
}

/*
 desc : �̺�Ʈ ������ ���ŵ� ��� (�θ� ���ؼ� ȣ���)
 parm : event	- [in]  �̺�Ʈ ������ ����� ����ü ������
 retn : None
*/
VOID CDlgSIM3::UpdateCtrl(STG_CCED *event)
{
	CEdit *pEdit= NULL;
	switch (event->exx_id)
	{
	case ENG_ESSC::en_e87_cms	:
		switch (event->cbf_id)
		{
		case E30_CEFI::en_state_change	:
			switch (event->evt_data.e87_sc.machine)
			{
			case E87_CDSM::en_e87lptsm	:	/* load port transfer state machine */
				pEdit	= (CEdit*)GetDlgItem(IDC_SIM3_EDT_TRANSER_STATE);
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
				pEdit	= (CEdit*)GetDlgItem(IDC_SIM3_EDT_ID_VERIFICATION);
				switch (event->evt_data.e87_sc.state)
				{
				case E87_CIVS::en_id_not_read				:	pEdit->SetWindowText(L"Not Read");				break;
				case E87_CIVS::en_id_wait_for_host			:	pEdit->SetWindowText(L"Wait for host");			break;
				case E87_CIVS::en_id_verification_ok		:	pEdit->SetWindowText(L"Verification OK");		break;
				case E87_CIVS::en_id_verification_failed	:	pEdit->SetWindowText(L"Verification Failed");	break;
				}
				break;
			case E87_CDSM::en_e87csmsm	:
				pEdit	= (CEdit*)GetDlgItem(IDC_SIM3_EDT_SLOT_VERIFICATION);
				switch (event->evt_data.e87_sc.state)
				{
				case E87_SMVS::en_sm_not_read				:	pEdit->SetWindowText(L"Not Read");				break;
				case E87_SMVS::en_sm_wait_for_host			:	pEdit->SetWindowText(L"Wait for host");			break;
				case E87_SMVS::en_sm_verification_ok		:	pEdit->SetWindowText(L"Verification OK");		break;
				case E87_SMVS::en_sm_verification_failed	:	pEdit->SetWindowText(L"Verification Failed");	break;
				}
				break;

			case E87_CDSM::en_e87lprsm	:	/* load port reservation state machine */
				pEdit	= (CEdit*)GetDlgItem(IDC_SIM3_EDT_RESERVE_STATE);
				switch (event->evt_data.e87_sc.state)
				{
				case E87_LPRS::en_lp_not_reserved	:	pEdit->SetWindowText(L"Not reserved");	break;
				case E87_LPRS::en_lp_reserved		:	pEdit->SetWindowText(L"Reserved");		break;
				}
				break;

			case E87_CDSM::en_e87lpcasm	:	/* load port/carrier association state machine */
				pEdit	= (CEdit*)GetDlgItem(IDC_SIM3_EDT_ASSOCIATE_STATE);
				switch (event->evt_data.e87_sc.state)
				{
				case E87_LPAS::en_not_associated	:	pEdit->SetWindowText(L"Not Associated");	break;
				case E87_LPAS::en_associated		:	pEdit->SetWindowText(L"Associated");		break;
				}
				break;
			}
			break;
		case E87_CEFI::en_recreate_finalize	:	UpdateLoadPortCarrier();	break;
		default								: UpdateCallbackEventE87(event);	break;
		}
		break;

	case ENG_ESSC::en_e40_pjm	:
		switch (event->cbf_id)
		{
		case E40_CEFI::en_pr_command_callback2	:
			switch (event->evt_data.e40_pc.cmd_src)
			{
			case E40_PECS::en_local_direct		:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_PR_CMD_SOURCE))->SetWindowText(L"Local Direct");		break;
			case E40_PECS::en_local_control_job	:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_PR_CMD_SOURCE))->SetWindowText(L"Local Control Job");	break;
			case E40_PECS::en_host_direct		:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_PR_CMD_SOURCE))->SetWindowText(L"Host Direct");		break;
			case E40_PECS::en_host_control_job	:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_PR_CMD_SOURCE))->SetWindowText(L"Host Control Job");	break;
			default								:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_PR_CMD_SOURCE))->SetWindowText(L"Unknown");			break;
			}
			switch (event->evt_data.e40_pc.cmd)
			{
			case E40_PCCC::en_pause				:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_PR_CMD))->SetWindowText(L"Pause");						break;
			case E40_PCCC::en_resume			:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_PR_CMD))->SetWindowText(L"Resume");					break;
			case E40_PCCC::en_stop				:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_PR_CMD))->SetWindowText(L"Stop");						break;
			case E40_PCCC::en_cancelcancel		:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_PR_CMD))->SetWindowText(L"CancelCancel");				break;
			case E40_PCCC::en_cancelstop		:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_PR_CMD))->SetWindowText(L"CancelStop");				break;
			case E40_PCCC::en_cancelabort		:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_PR_CMD))->SetWindowText(L"CancelAbort");				break;
			case E40_PCCC::en_canceldequeue		:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_PR_CMD))->SetWindowText(L"CancelDeque");				break;
			case E40_PCCC::en_abort				:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_PR_CMD))->SetWindowText(L"Abort");						break;
			case E40_PCCC::en_create			:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_PR_CMD))->SetWindowText(L"Create");					break;
			case E40_PCCC::en_setup				:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_PR_CMD))->SetWindowText(L"Setup");						break;
			case E40_PCCC::en_waitingforstart	:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_PR_CMD))->SetWindowText(L"Waiting For Start");			break;
			case E40_PCCC::en_start				:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_PR_CMD))->SetWindowText(L"Start");						break;
			default								:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_PR_CMD))->SetWindowText(L"Unknown");					break;
			}
			if (E40_PCCC::en_create == (E40_PCCC)event->evt_data.e40_pc.cmd)
			{
				INT32 i32Pos	= ((CListBox*)GetDlgItem(IDC_SIM3_BOX_PRJOB))->AddString(event->str_data1);
				((CListBox*)GetDlgItem(IDC_SIM3_BOX_PRJOB))->SetCurSel(i32Pos);
			}
			break;
		}
		break;
	case ENG_ESSC::en_e90_sts	:
		switch (event->cbf_id)
		{
		case E90_CEFI::en_command_callback			:	UpdateSubstrateCommand(E90_SPSC(event->evt_data.e90_cc.cmd), event->str_data1);	break;
		case E90_CEFI::en_substrate_state_change	:	UpdateSubstrateState(event->str_data1, &event->evt_data.e90_sc);	break;
		}
		break;
	case ENG_ESSC::en_e94_cjm	:
		switch (event->cbf_id)
		{
		case E94_CEFI::en_cj_create				:
			((CListBox*)GetDlgItem(IDC_SIM3_BOX_CRJOB))->AddString(event->str_data1);
			break;
		case E94_CEFI::en_cj_command_callback3	:
			switch (event->evt_data.e94_pc.cmd_src)
			{
			case E40_PECS::en_local_direct		:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CJ_CMD_SOURCE))->SetWindowText(L"Local Direct");		break;
			case E40_PECS::en_local_control_job	:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CJ_CMD_SOURCE))->SetWindowText(L"Local Control Job");	break;
			case E40_PECS::en_host_direct		:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CJ_CMD_SOURCE))->SetWindowText(L"Host Direct");		break;
			case E40_PECS::en_host_control_job	:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CJ_CMD_SOURCE))->SetWindowText(L"Host Control Job");	break;
			default							:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CJ_CMD_SOURCE))->SetWindowText(L"Unknown");			break;
			}
			switch (event->evt_data.e94_pc.cmd)
			{
			case E94_CCMD::en_none				:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CJ_CMD))->SetWindowText(L"None");		break;
			case E94_CCMD::en_abort				:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CJ_CMD))->SetWindowText(L"Abort");		break;
			case E94_CCMD::en_cancel			:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CJ_CMD))->SetWindowText(L"Cancel");	break;
			case E94_CCMD::en_pause				:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CJ_CMD))->SetWindowText(L"Pause");		break;
			case E94_CCMD::en_resume			:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CJ_CMD))->SetWindowText(L"Resume");	break;
			case E94_CCMD::en_start				:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CJ_CMD))->SetWindowText(L"Start");		break;
			case E94_CCMD::en_stop				:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CJ_CMD))->SetWindowText(L"Stop");		break;
			case E94_CCMD::en_create			:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CJ_CMD))->SetWindowText(L"Create");	break;
			case E94_CCMD::en_hoq				:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CJ_CMD))->SetWindowText(L"HOQ");		break;
			case E94_CCMD::en_deselect			:	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CJ_CMD))->SetWindowText(L"Deselect");	break;
			}
			if (E94_CCMD::en_create == (E94_CCMD)event->evt_data.e94_pc.cmd)
			{
				INT32 i32Pos	= ((CListBox*)GetDlgItem(IDC_SIM3_BOX_CRJOB))->AddString(event->str_data1);
				((CListBox*)GetDlgItem(IDC_SIM3_BOX_CRJOB))->SetCurSel(i32Pos);
			}
			break;
		}
		break;
	}
}

VOID CDlgSIM3::UpdateCallbackEventE87(STG_CCED *event)
{
	TCHAR tzMesg[512]	= {NULL}, tzKind[64] = {NULL};
	E87_LPTS enTransStatus;

	switch (event->exx_id)
	{
	case ENG_ESSC::en_e87_cms	:
		switch(event->cbf_id)
		{
		case E87_CEFI::en_change_service_status	:
			wcscpy_s(tzKind, 64, L"change service Status");
			break;
		case E87_CEFI::en_bind			:
			wcscpy_s(tzKind, 64, L"bind");
			break;
		case E87_CEFI::en_cancel_bind	:
			wcscpy_s(tzKind, 64, L"cancel bind");
			break;
		case E87_CEFI::en_cancel_carrier	:
			wcscpy_s(tzKind, 64, L"cancel carrier");
			/* Carrier Transfer State Update */
			uvCIMLib_E87CMS_GetTransferStatus((UINT16)event->evt_data.e87_cf.port_id, enTransStatus);
			CEdit * pEdit	= (CEdit*)GetDlgItem(IDC_SIM3_EDT_TRANSER_STATE);
			switch (event->evt_data.e87_sc.state)
			{
			case E87_LPTS::en_lp_out_of_service		:	pEdit->SetWindowText(L"OutOfService");		break;
			case E87_LPTS::en_lp_transfer_blocked	:	pEdit->SetWindowText(L"TransferBlocked");	break;
			case E87_LPTS::en_lp_ready_to_load		:	pEdit->SetWindowText(L"ReadyToLoad");		break;
			case E87_LPTS::en_lp_ready_to_unload		:	pEdit->SetWindowText(L"ReadyToUnload");		break;
			case E87_LPTS::en_lp_in_service			:	pEdit->SetWindowText(L"InService");			break;
			case E87_LPTS::en_lp_transfer_ready		:	pEdit->SetWindowText(L"TransferReady");		break;
			}
			/* Load Port�� Transfer ���¸� ReadyToUnload�� ���� */
			uvCIMLib_E87CMS_SetTransferStatus((UINT8)event->evt_data.e87_cf.port_id,
											  E87_LPTS::en_lp_ready_to_unload);
			UpdateBaseInformation(1);
			break;
		}
		break;
	}
#if 0
	/* Log Time ���� */
	swprintf_s(tzMesg, 512, L"<%s> port_id=%d, state=%d",
			   tzKind, event->evt_data.e87_cf.port_id, event->evt_data.e87_cf.state);
	/* ���� ��ϵ� ���� */
	m_rch_ctl[0].Append(tzMesg, RGB(0, 0, 0), TRUE);
#endif
}

VOID CDlgSIM3::UpdateSubstrateLocation()
{
	E90_SSLS enState[4]	= { E90_SSLS::en_unoccupied, E90_SSLS::en_unoccupied,
							E90_SSLS::en_unoccupied, E90_SSLS::en_unoccupied };

	uvCIMLib_E90STS_GetSubstrateLocationStateIndex(ENG_TSSI::en_robot, enState[0]);
	uvCIMLib_E90STS_GetSubstrateLocationStateIndex(ENG_TSSI::en_pre_aligner, enState[1]);
	uvCIMLib_E90STS_GetSubstrateLocationStateIndex(ENG_TSSI::en_process_chamber, enState[2]);
	uvCIMLib_E90STS_GetSubstrateLocationStateIndex(ENG_TSSI::en_buffer, enState[3]);
	((CButton*)GetDlgItem(IDC_SIM3_CHK_WAFER_IN_ROBOT))->SetCheck((INT32)enState[0]);
	((CButton*)GetDlgItem(IDC_SIM3_CHK_WAFER_IN_ALIGNER))->SetCheck((INT32)enState[1]);
	((CButton*)GetDlgItem(IDC_SIM3_CHK_WAFER_IN_TABLE))->SetCheck((INT32)enState[2]);
	((CButton*)GetDlgItem(IDC_SIM3_CHK_WAFER_IN_BUFFER))->SetCheck((INT32)enState[3]);
}

VOID CDlgSIM3::UpdateProcessingState()
{
	if (m_lst_ctl[0].GetItemCount() < 1)	return;

}

VOID CDlgSIM3::UpdateSubstrateState(PTCHAR subst_id, LPG_E90_SC data)
{
	switch (data->machine)
	{
	case E90_VSMC::en_substrate_transport_sm	:	;	break;
	case E90_VSMC::en_substrate_processing_sm	:	;	break;
	case E90_VSMC::en_substrate_reader_sm		:	;	break;
	case E90_VSMC::en_substrate_location_sm		:	;	break;
	}
}

VOID CDlgSIM3::UpdateSubstrateCommand(E90_SPSC command, PTCHAR subst_id)
{
	TCHAR tzVerify[256]	= {NULL};

	switch (command)
	{
	case E90_SPSC::en_proceed_with_substrate	:	swprintf_s(tzVerify, 256, L"%s / Verified", subst_id);	break;	/* �ش� Substrate ID�� ���������� �о����� ������ ��� ���� �϶� */
	case E90_SPSC::en_cancel_substrate			:	swprintf_s(tzVerify, 256, L"%s / Canceled", subst_id);	break;	/* �ش� Substrate ID�� Reading �� ��, Serial or Tag ���� ������ ������, ��� �ض� */
	}
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_SUBST_ID_VERIFY))->SetWindowText(tzVerify);
}
#if 0
/* Carrier Placed */
void CDlgSIM3::OnBnClickedSim3BtnLoad()
{
	UINT8 i	= 0, j	= 0, u8Skip=1, u8Stat=(UINT8)E87_CSMS::en_correctly_occupied;
	TCHAR tzCarrID[CIM_CARRIER_ID_SIZE]	= {NULL}, tzPortID[CIM_VAL_ASCII_SIZE] = {NULL}, tzLocID[CIM_VAL_ASCII_SIZE] = {NULL};
	TCHAR tzSubstID[CIM_SUBSTRATE_ID]	= {NULL}, tzStat[4][16]	= {L"", L"empty", L"", L"occupied" }, tzCount[32] = {NULL};
	PTCHAR ptzDockName		= NULL;
	PTCHAR ptzUndockName	= NULL;
	PTCHAR ptzProcName		= NULL;
	PTCHAR ptzSubstState	= NULL;
	CStringArray arrVarID;
	UNG_CVVT unValue		= {NULL};
	CStringArray arrVarName;
	CAtlList <UNG_CVVT> lstValue;
	CAtlList <E30_GPVT> lstType;
	CAtlList <E87_CSMS> lstSlotMap;
	CStringArray arrCarrID;
	CAtlList <UINT8> lstSlotID;

#if 0
	/* Process ���� ���� */
	CListBox *pBoxPRJob	= ((CListBox*)GetDlgItem(IDC_SIM3_BOX_PRJOB));
	INT32 i32SelPos	= pBoxPRJob->GetCurSel();
	if (i32SelPos < 0)
	{
		AfxMessageBox(L"No item selected (Item:Process Job)", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	TCHAR tzPrJobID[CIM_PROCESS_JOB_ID_SIZE]	= {NULL};
	pBoxPRJob->GetText(i32SelPos, tzPrJobID);
#if 0
	uvCIMLib_E40PJM_SetProcessJobState(tzPrJobID, E40_PPJS::en_processing);
#else
	uvCIMLib_E40PJM_SetProcessJobState(tzPrJobID, E40_PPJS::en_waiting_for_start);
	uvCIMLib_E40PJM_SetPRJobStateControl(tzPrJobID, E40_PPJC::en_start);
	uvCIMLib_E40PJM_SetProcessJobState(tzPrJobID, E40_PPJS::en_processing);
#endif
#endif
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_BUFFER_LOAD))->SetWindowText(L"empty");
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_ALIGNER_LOAD))->SetWindowText(L"empty");
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_ROBOT_LOAD))->SetWindowText(L"empty");
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_STAGE_LOAD))->SetWindowText(L"empty");

	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_CARRIER_ID_SIZE);
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	ptzDockName		= uvCIMLib_GetLoadPortDockedName((UINT16)_wtoi(tzPortID), 0x00);
	ptzUndockName	= uvCIMLib_GetLoadPortDockedName((UINT16)_wtoi(tzPortID), 0x01);

	/* Substrate Reader : Enable */
	if (!uvCIMLib_E90STS_SetSubstrateReaderAvailable(1))	return;

	/* Substrate (Wafer) �ʱ� ���� ��ġ ��*/
	if (wcslen(tzCarrID) < 1)	return;

	/* �� Substrate�� ���� Location�� ���� �ʱ�ȭ */
	uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_unoccupied);
	uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_pre_aligner, E90_SSLS::en_unoccupied);
	uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_process_chamber, E90_SSLS::en_unoccupied);
	uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_buffer, E90_SSLS::en_unoccupied);
	uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_load_port_1, E90_SSLS::en_unoccupied);
	uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_load_port_2, E90_SSLS::en_unoccupied);

	/* 1. CarrierTransferStart */
	if (!uvCIMLib_E87CMS_SetTransferStart((UINT16)_wtoi(tzPortID)))	return;
	/* 2. CarrierPlaced */
	if (!uvCIMLib_E87CMS_SetCarrierPlaced((UINT16)_wtoi(tzPortID)))	return;

	uvCIMLib_SetCacheAsciiName(COMMN_CONN_ID, L"LocationID", ptzUndockName, 0, 0);
	UNG_CVVT enPortID;
	enPortID.u8_value	= (UINT8)_wtoi(tzPortID);
	uvCIMLib_SetCacheValueName(COMMN_CONN_ID, E30_GPVT::en_u1, L"PortID",enPortID, 0);

	/* �ݵ�� �ʱ�ȭ */
	m_u8StepMain	= 0x00;
	m_u8StepSub		= 0x00;
}

/* Carrier Loaded */
void CDlgSIM3::OnBnClickedSim3BtnLoad2()
{
	TCHAR tzCarrID[CIM_CARRIER_ID_SIZE]	= {NULL}, tzPortID[CIM_VAL_ASCII_SIZE] = {NULL};
	PTCHAR ptzDockName		= NULL;
	PTCHAR ptzUndockName	= NULL;
	PTCHAR ptzProcName		= NULL;
	PTCHAR ptzSubstState	= NULL;
	CStringArray arrVarID;
	UNG_CVVT unValue		= {NULL};
	CStringArray arrVarName;
	CAtlList <UNG_CVVT> lstValue;
	CAtlList <E30_GPVT> lstType;

	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_CARRIER_ID_SIZE);
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	ptzDockName		= uvCIMLib_GetLoadPortDockedName((UINT16)_wtoi(tzPortID), 0x00);
	ptzUndockName	= uvCIMLib_GetLoadPortDockedName((UINT16)_wtoi(tzPortID), 0x01);

	/* ���� ��ư���� */
	/* ------- */
	/* 3. MaterialReceived */
	arrVarName.Add(L"PortID");		/* PortID */
	arrVarName.Add(L"LocationID");	/* LocationID */
	unValue.u8_value	= (UINT8)_wtoi(tzPortID);
	lstValue.AddTail(unValue);
	swprintf_s(unValue.s_value, CIM_VAL_ASCII_SIZE, L"%s", ptzUndockName);
	lstValue.AddTail(unValue);
	lstType.AddTail(E30_GPVT::en_u1);
	lstType.AddTail(E30_GPVT::en_a);
	if (!uvCIMLib_SetTrigEventValuesName(COMMN_CONN_ID, L"MaterialReceived", arrVarName, lstValue, lstType))	return;
	arrVarName.RemoveAll();
	lstType.RemoveAll();
	lstValue.RemoveAll();
	/* 4. CarrierAtPort */
	if (!uvCIMLib_E87CMS_SetCarrierAtPort(tzCarrID, (UINT16)_wtoi(tzPortID), ptzUndockName))	return;
	/* 5. Proceeded With Carrier */
	if (!uvCIMLib_E87CMS_SetProceedWithCarrier((UINT16)_wtoi(tzPortID), tzCarrID))	return;

	Sim3BtnCarrAtClamp();
}
#else
/* Carrier Placed */
void CDlgSIM3::OnBnClickedSim3BtnLoad()
{
	UINT8 i	= 0, j	= 0, u8Skip=1, u8Stat=(UINT8)E87_CSMS::en_correctly_occupied;
	TCHAR tzCarrID[CIM_CARRIER_ID_SIZE]	= {NULL}, tzPortID[CIM_VAL_ASCII_SIZE] = {NULL}, tzLocID[CIM_VAL_ASCII_SIZE] = {NULL};
	TCHAR tzSubstID[CIM_SUBSTRATE_ID]	= {NULL}, tzStat[4][16]	= {L"", L"empty", L"", L"occupied" }, tzCount[32] = {NULL};
	PTCHAR ptzDockName		= NULL;
	PTCHAR ptzUndockName	= NULL;
	PTCHAR ptzProcName		= NULL;
	PTCHAR ptzSubstState	= NULL;
	CStringArray arrVarID;
	UNG_CVVT unValue		= {NULL};
	CStringArray arrVarName;
	CAtlList <UNG_CVVT> lstValue;
	CAtlList <E30_GPVT> lstType;
	CAtlList <E87_CSMS> lstSlotMap;
	CStringArray arrCarrID;
	CAtlList <UINT8> lstSlotID;

	((CEdit*)GetDlgItem(IDC_SIM3_EDT_BUFFER_LOAD))->SetWindowText(L"empty");
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_ALIGNER_LOAD))->SetWindowText(L"empty");
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_ROBOT_LOAD))->SetWindowText(L"empty");
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_STAGE_LOAD))->SetWindowText(L"empty");

	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_CARRIER_ID_SIZE);
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	ptzDockName		= uvCIMLib_GetLoadPortDockedName((UINT16)_wtoi(tzPortID), 0x00);
	ptzUndockName	= uvCIMLib_GetLoadPortDockedName((UINT16)_wtoi(tzPortID), 0x01);

	/* Substrate Reader : Enable */
	if (!uvCIMLib_E90STS_SetSubstrateReaderAvailable(1))	return;

	/* Substrate (Wafer) �ʱ� ���� ��ġ ��*/
	if (wcslen(tzCarrID) < 1)	return;

	/* �� Substrate�� ���� Location�� ���� �ʱ�ȭ */
	uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot,			E90_SSLS::en_unoccupied);
	uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_pre_aligner,		E90_SSLS::en_unoccupied);
	uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_process_chamber,	E90_SSLS::en_unoccupied);
	uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_buffer,			E90_SSLS::en_unoccupied);
	uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_load_port_1,		E90_SSLS::en_unoccupied);
	uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_load_port_2,		E90_SSLS::en_unoccupied);

	/* 1. CarrierTransferStart ([E] E87ReadyToLoad2TransferBlocked --> [S] TranserBlocked) */
	if (!uvCIMLib_E87CMS_SetTransferStart((UINT16)_wtoi(tzPortID)))	return;

	/* 2. MaterialReceived */
	arrVarName.Add(L"PortID");		/* PortID */
	arrVarName.Add(L"LocationID");	/* LocationID */
	unValue.u8_value	= (UINT8)_wtoi(tzPortID);
	lstValue.AddTail(unValue);
	swprintf_s(unValue.s_value, CIM_VAL_ASCII_SIZE, L"%s", ptzUndockName);
	lstValue.AddTail(unValue);
	lstType.AddTail(E30_GPVT::en_u1);
	lstType.AddTail(E30_GPVT::en_a);
	if (!uvCIMLib_SetTrigEventValuesName(COMMN_CONN_ID, L"MaterialReceived", arrVarName, lstValue, lstType))	return;
	arrVarName.RemoveAll();
	lstType.RemoveAll();
	lstValue.RemoveAll();

	/* 3. CarrierPlaced */
	if (!uvCIMLib_E87CMS_SetCarrierPlaced((UINT16)_wtoi(tzPortID)))	return;
#if 0
	uvCIMLib_SetCacheAsciiName(COMMN_CONN_ID, L"LocationID", ptzUndockName, 0, 0);
	UNG_CVVT enPortID;
	enPortID.u8_value	= (UINT8)_wtoi(tzPortID);
	uvCIMLib_SetCacheValueName(COMMN_CONN_ID, E30_GPVT::en_u1, L"PortID",enPortID, 0);
#endif
	/* �ݵ�� �ʱ�ȭ */
	m_u8StepMain	= 0x00;
	m_u8StepSub		= 0x00;
}

/* Carrier Loaded */
void CDlgSIM3::OnBnClickedSim3BtnLoad2()
{
	UINT8 i	= 0, j	= 0, u8Skip=1, u8Stat=(UINT8)E87_CSMS::en_correctly_occupied;
	TCHAR tzCarrID[CIM_CARRIER_ID_SIZE]	= {NULL}, tzPortID[CIM_VAL_ASCII_SIZE] = {NULL};
	TCHAR tzSubstID[CIM_SUBSTRATE_ID]	= {NULL}, tzStat[4][16]	= {L"", L"empty", L"", L"occupied" }, tzCount[32] = {NULL};
	PTCHAR ptzDockName		= NULL;
	PTCHAR ptzUndockName	= NULL;
	PTCHAR ptzProcName		= NULL;
	PTCHAR ptzSubstState	= NULL;
	UNG_CVVT unValue		= {NULL};
	E90_SISM enSubstState;
	E90_SSPS enSubstProc;
	CStringArray arrVarID;
	CStringArray arrCarrID;
	CStringArray arrVarName;
	CAtlList <UNG_CVVT> lstValue;
	CAtlList <E30_GPVT> lstType;
	CAtlList <E87_CSMS> lstSlotMap;

	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_CARRIER_ID_SIZE);
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	ptzDockName		= uvCIMLib_GetLoadPortDockedName((UINT16)_wtoi(tzPortID), 0x00);
	ptzUndockName	= uvCIMLib_GetLoadPortDockedName((UINT16)_wtoi(tzPortID), 0x01);

	/* 3. CarrierAtPort ([E] E87NoState2Carrier) */
	/* 4. [E] E87NoState2SlotMapNotRead --> [S] SlotMapNotRead */
	/* 4. CarrierAtPort */
	if (!uvCIMLib_E87CMS_SetCarrierAtPort(tzCarrID, (UINT16)_wtoi(tzPortID), ptzUndockName))	return;
	/* 5. Proceeded With Carrier */
	if (!uvCIMLib_E87CMS_SetProceedWithCarrier((UINT16)_wtoi(tzPortID), tzCarrID))	return;

	/* 6. Carrier Clamped */
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
	if (!uvCIMLib_SetTrigEventValuesName(COMMN_CONN_ID /* host_id for common */,
										 L"CarrierClamped" /*870006*/,
										 arrVarID /* Common Variables : PortID */,
										 lstValue,
										 lstType /*AppendValueU1*/))	return;
	arrVarID.RemoveAll();
	lstValue.RemoveAll();
	lstType.RemoveAll();

	/* 7. Carrier Move Docked */
	if (!uvCIMLib_E87CMS_SetMoveCarrier(tzCarrID, (UINT16)_wtoi(tzPortID), ptzDockName))	return;

	/* 8. Carrier Opened */
	arrVarName.Add(L"PortID");		/* PortID */
	arrVarName.Add(L"CarrierID");	/* CarrierID */
	arrVarName.Add(L"LocationID");	/* LocationID */
	lstType.AddTail(E30_GPVT::en_u1);
	lstType.AddTail(E30_GPVT::en_a);
	lstType.AddTail(E30_GPVT::en_a);
	unValue.u8_value	= (UINT8)_wtoi(tzPortID);
	lstValue.AddTail(unValue);
	swprintf_s(unValue.s_value, CIM_VAL_ASCII_SIZE, L"%s", tzCarrID);
	lstValue.AddTail(unValue);
	swprintf_s(unValue.s_value, CIM_VAL_ASCII_SIZE, L"%s", ptzDockName);
	lstValue.AddTail(unValue);
	/* ����� ���� Trigger Event ���� */
	if (!uvCIMLib_SetTrigEventValuesName(COMMN_CONN_ID, L"CarrierOpened", arrVarName, lstValue, lstType))	return;
	arrVarName.RemoveAll();
	lstType.RemoveAll();
	lstValue.RemoveAll();
#if 0
	CListBox *pBoxPRJob	= (CListBox*)GetDlgItem(IDC_SIM3_BOX_PRJOB);
	INT32 i32SelPos	= pBoxPRJob->GetCurSel();
	if (i32SelPos < 0)
	{
		AfxMessageBox(L"No item selected (Item:Process Job)", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	TCHAR tzPrJobID[CIM_PROCESS_JOB_ID_SIZE]	= {NULL};
	pBoxPRJob->GetText(i32SelPos, tzPrJobID);
	if (!uvCIMLib_E40PJM_GetPRMtlNameList(tzPrJobID, arrCarrID, lstSlotID))	return;
#endif
	/* 9. Read (Scan) Slot Map */
	m_lst_ctl[0].SetRedraw(FALSE);
	m_lst_ctl[0].DeleteAllItems();
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_WAFER_COUNT))->GetWindowText(tzCount, 32);
#if 1
	for (i=0; i<(UINT8)_wtoi(tzCount); i++)
#else
	for (i=0; i<(UINT8)arrCarrID.GetCount(); i++)
#endif
	{
		if (((CButton *)GetDlgItem(IDC_SIM3_CHK_SLOT_MAP_RAND))->GetCheck())
		{
			u8Stat	= (uvCmn_GetRandNumer(0x00, 1) % 2) == 0x00 ? 0x01 /* E87_CSMS::en_empty */ : 0x03 /* E87_CSMS::en_correctly_occupied */;
		}
		else
		{
			u8Stat	= 0x03/* E87_CSMS::en_correctly_occupied */;
		}
		lstSlotMap.AddTail(E87_CSMS(u8Stat));

		/* Substrate ID ��� */
		swprintf_s(tzSubstID, CIM_SUBSTRATE_ID, L"%s.%02d", tzCarrID, i+1);	/* Carrier ID.SlotNumber �������� ... */
		m_lst_ctl[0].InsertItem(i, tzSubstID);
		m_lst_ctl[0].SetItemText(i, 1, tzStat[u8Stat]);
	}
	m_lst_ctl[0].SetRedraw(TRUE);
	m_lst_ctl[0].Invalidate(FALSE);
	if (wcslen(tzCarrID) < 1)	return;
	/* Slot Map ���� */
	if (!uvCIMLib_E87CMS_SetVerifySlotMap(tzCarrID, &lstSlotMap))	return;
	/* �޸� ��ȯ */
	lstSlotMap.RemoveAll();
	/* 10. Proceeded With Carrier */
	if (!uvCIMLib_E87CMS_SetProceedWithCarrier((UINT16)_wtoi(tzPortID), tzCarrID))	return;

#if 1
	for (i=0; i<(UINT8)_wtoi(tzCount); i++)
#else
	for (i=0; i<(UINT8)arrCarrID.GetCount(); i++)
#endif
	{
#if 0
		swprintf_s(tzSubstID, CIM_SUBSTRATE_ID, L"%s.%d", tzCarrID, i+1);	/* Carrier ID.SlotNumber �������� ... */
#else
		wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);
		if (!uvCIMLib_Mixed_GetSubstrateID(tzCarrID, i+1, tzSubstID, CIM_SUBSTRATE_ID))	continue;
#endif
		/* 11. Substrate ID ���� ���� */
		if (!uvCIMLib_E90STS_GetSubstrateIDStatus(tzSubstID, enSubstState))	m_lst_ctl[0].SetItemText(i, 3, L"No State");
		else
		{
			switch (enSubstState)
			{
			case E90_SISM::en_no_state			:	m_lst_ctl[0].SetItemText(i, 3, L"No State");			break;
			case E90_SISM::en_not_confirmed		:	m_lst_ctl[0].SetItemText(i, 3, L"Not Confirmed");		break;
			case E90_SISM::en_waiting_for_host	:	m_lst_ctl[0].SetItemText(i, 3, L"Waiting For Host");	break;
			case E90_SISM::en_confirmed			:	m_lst_ctl[0].SetItemText(i, 3, L"Confirmed");			break;
			case E90_SISM::en_confirmation_failed:	m_lst_ctl[0].SetItemText(i, 3, L"Confirmation Failed");	break;
			}
		}
		/* 11. Substrate Processing ���� ���� */
		if (uvCIMLib_E90STS_GetSubstrateProcessingState(tzSubstID, enSubstProc))
		{
			ptzProcName	= uvCIMLib_GetEquipProcessingName(enSubstProc);
			if (ptzProcName)	m_lst_ctl[0].SetItemText(i, 2, ptzProcName);
		}
	}
#if 0
	/* ���ŵ� Process Job �������� Substrated ID ���� */
	LoadSubstIDForProcessJob();
#endif
	ENG_TSSI enLocID	= ENG_TSSI::en_load_port_1;
	if (_wtoi(tzPortID) != 1)	enLocID = ENG_TSSI::en_load_port_2;
#if 0
		/* List ���  */
		swprintf_s(tzSubstID, CIM_SUBSTRATE_ID, L"%s.%d", tzCarrID, i+1);	/* Carrier ID.SlotNumber �������� ... */
		/* ������ Substrate�� Location ID�� CIM300�� ��� (lstSlotMap�� ���ؼ� ���������� ����� �ǹǷ�, ���� ��� �۾� ���ʿ� ����) */
		if (!uvCIMLib_E90STS_SetRegisterSubstrateIndex(tzSubstID, enLocID))	continue;
		/* ���� 2���� �Լ� ȣ���� �ѹ��� �ذ� ? */
		if (!uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, enLocID, E90_SSTS::en_source, E90_SSPS::en_no_state))	continue;
		if (!uvCIMLib_E90STS_SetSubstIDStatus(tzSubstID, E90_SISM::en_no_state))	continue;
#endif
}
#endif

void CDlgSIM3::OnBnClickedSim3BtnAccess()
{
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL};
	TCHAR tzPortID[CIM_VAL_ASCII_SIZE]	= {NULL};

	CStringArray arrVarName;
	CAtlList <UNG_CVVT> lstVarVal;
	CAtlList <E30_GPVT> lstType;
	UNG_CVVT unValue	= {NULL};

	/* ���� Slot Number�� �ٽ� ��ȯ�� Substrate ID ��� */
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_VAL_ASCII_SIZE);
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);

	/* Carrier Accessing */
	if (!uvCIMLib_E87CMS_SetAccessCarrier(COMMN_CONN_ID, tzCarrID, (UINT16)_wtoi(tzPortID),
										  E87_CCAS::en_carrier_in_access))	return;
}

void CDlgSIM3::OnBnClickedSim3BtnUnload()
{
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL}, tzPortID[CIM_VAL_ASCII_SIZE] = {NULL};
	PTCHAR ptzDockName	= NULL;
	PTCHAR ptzUndockName= NULL;
	CStringArray arrVarName;
	CAtlList <UNG_CVVT> lstVarVal;
	CAtlList <E30_GPVT> lstType;
	UNG_CVVT unValue;

	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_VAL_ASCII_SIZE);
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	if (wcslen(tzCarrID) < 1)	return;
#if 0
	/* Carrier ID�� Closed �� �������� Ȯ�� */
	if (!uvCIMLib_E87CMS_IsCarrierAccessClosed(tzCarrID))
	{
		AfxMessageBox(L"The carrier is not complete status", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
#endif
#if 0	/* PRJob Complete ȣ�� �Լ� �κп��� ���� �� */
	/* Carrier Closed */
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
	if (!uvCIMLib_SetTrigEventValuesName(COMMN_CONN_ID, L"CarrierClosed", arrVarName, lstVarVal, lstType))	return;

	arrVarName.RemoveAll();
	lstType.RemoveAll();
	lstVarVal.RemoveAll();
#endif
	/* Carrier Move Undocked  */
	ptzUndockName	= uvCIMLib_GetLoadPortDockedName((UINT16)_wtoi(tzPortID), 0x01);
	uvCIMLib_E87CMS_SetMoveCarrier(tzCarrID, (UINT16)_wtoi(tzPortID), ptzUndockName);
	/* Carrier Unclamped */
	unValue.u8_value= (UINT8)_wtoi(tzPortID);
	if (!uvCIMLib_SetTrigEventValueName(COMMN_CONN_ID		/* host_id for common */,
										L"CarrierUnclamped"	/* Common Events    : CarrierUnclamped */,
										L"PortID"			/* Common Variables : PortID */,
										&unValue,
										E30_GPVT::en_u1/*AppendValueU1*/))	return;
	/* Carrier Ready To Unload */
	if (!uvCIMLib_E87CMS_SetReadyToUnload((UINT16)_wtoi(tzPortID)))	return;
	UpdateBaseInformation(1);
	UpdateBaseInformation(3);

	/* Wait for initial E84 Signal ... */
	if (!uvCIMLib_E87CMS_SetTransferStart((UINT16)_wtoi(tzPortID)))	return;
	UpdateBaseInformation(2);
	UpdateBaseInformation(3);

	/* ���� �޸𸮿��� Carrier ID ���� Control & Process Job ID ���� */
	uvCIMLib_RemoveSharedCarrID(tzCarrID);
	/* Unload Transfer Complete */
	if (!uvCIMLib_E87CMS_SetCarrierDepartedPort(tzCarrID, (UINT16)_wtoi(tzPortID), ptzUndockName))	return;
	UpdateBaseInformation(5);

	/* Carrier Transfer Blocked */
	arrVarName.Add(L"PortID");		/* PortID */
	arrVarName.Add(L"CarrierID");	/* CarrierID */
	arrVarName.Add(L"LocationID");	/* LocationID */
	lstType.AddTail(E30_GPVT::en_u1);
	lstType.AddTail(E30_GPVT::en_a);
	lstType.AddTail(E30_GPVT::en_a);
	unValue.u8_value= (UINT8)_wtoi(tzPortID);
	lstVarVal.AddTail(unValue);
	swprintf_s(unValue.s_value, CIM_VAL_ASCII_SIZE, L"%s", tzCarrID);
	lstVarVal.AddTail(unValue);
	swprintf_s(unValue.s_value, CIM_VAL_ASCII_SIZE, L"%s", ptzUndockName);
	lstVarVal.AddTail(unValue);

	/* ����� ���� Trigger Event ���� */
	if (!uvCIMLib_SetTrigEventValuesName(COMMN_CONN_ID, L"MaterialRemoved", arrVarName, lstVarVal, lstType))	return;

	arrVarName.RemoveAll();
	lstType.RemoveAll();
	lstVarVal.RemoveAll();
}

void CDlgSIM3::OnBnClickedSim3ChkSlotMapRand()
{
	((CButton*)GetDlgItem(IDC_SIM3_CHK_SLOT_MAP_RAND))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_SIM3_CHK_SLOT_MAP_OCCUPIED))->SetCheck(0);
}
void CDlgSIM3::OnBnClickedSim3ChkSlotMapOccupied()
{
	((CButton*)GetDlgItem(IDC_SIM3_CHK_SLOT_MAP_RAND))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_SIM3_CHK_SLOT_MAP_OCCUPIED))->SetCheck(1);
}

INT32 CDlgSIM3::GetEmptySlotNumInCarrier()
{
	TCHAR tzMapState[32]= {NULL};
	TCHAR tzProcess[32]	= {NULL};
	INT32 i = 0, iItem	= m_lst_ctl[0].GetItemCount();
	if (iItem < 1)	return -1;

	for (; i<iItem; i++)
	{
		wmemset(tzProcess, 0x00, 32);
		wmemset(tzMapState, 0x00, 32);
		m_lst_ctl[0].GetItemText(i, 1, tzMapState, 32);
		m_lst_ctl[0].GetItemText(i, 2, tzProcess, 32);
		if (0 == wcscmp(tzProcess, L"In Process") &&
			0 == wcscmp(tzMapState, L"empty"))	return (i);
	}

	return -1;
}
INT32 CDlgSIM3::GetNotEmptySlotNumInCarrier()
{
	BOOL bIsSlotNo	= FALSE;
	TCHAR tzMapState[32]= {NULL};
	TCHAR tzProcess[32]	= {NULL};
	TCHAR tzPRJobID[CIM_PROCESS_JOB_ID_SIZE]= {NULL};
	INT32 i = 0, iItem	= m_lst_ctl[0].GetItemCount();
	if (iItem < 1)	return -1;
	CListBox *pBoxPRJob	= (CListBox*)GetDlgItem(IDC_SIM3_BOX_CARR_SLOT);
	INT32 i32Count	= pBoxPRJob->GetCount();

	/* Destination ���� ��ġ�ϴ� ��쵵 ��� */
	if (!GetSelectPRJobID(tzPRJobID))	return -1;

	for (; i<iItem; i++)
	{
		/* ��� �ִ� �� ��, ���� ó���ϰ��� �ϴ� Process Job ID�� ���� Slot Number���� Ȯ�� �۾� �ʿ� */
		if (!uvCIMLib_E40PJM_IsFindSlotNoOfPRMtlNameList(tzPRJobID, i+1, bIsSlotNo))	continue;
		/* �˻� �����ߴ��� ���ο� ���� */
		if (!bIsSlotNo)	continue;

		wmemset(tzProcess, 0x00, 32);
		m_lst_ctl[0].GetItemText(i, 1, tzMapState, 32);
		m_lst_ctl[0].GetItemText(i, 2, tzProcess, 32);
		if (0 == wcscmp(tzProcess, L"Need Processing") &&
			0 == wcscmp(tzMapState, L"occupied"))
		{
			return i;
		}
	}

	return -1;
}
INT32 CDlgSIM3::GetNeedProcessingSlotNumInCarrier()
{
	BOOL bIsSlotNo		= FALSE;
	TCHAR tzPRJobID[CIM_PROCESS_JOB_ID_SIZE]= {NULL};
	TCHAR tzSubstID[CIM_SUBSTRATE_ID]		= {NULL};
	TCHAR tzMapState[32]= {NULL};
	TCHAR tzProcess[32]	= {NULL};
	INT32 i = 0, iItem	= m_lst_ctl[0].GetItemCount();
	E90_SSPS enSubstState;
	if (iItem < 1)	return -1;

	/* Destination ���� ��ġ�ϴ� ��쵵 ��� */
	if (!GetSelectPRJobID(tzPRJobID))	return -1;

	for (; i<iItem; i++)
	{
		/* ��� �ִ� �� ��, ���� ó���ϰ��� �ϴ� Process Job ID�� ���� Slot Number���� Ȯ�� �۾� �ʿ� */
		if (!uvCIMLib_E40PJM_IsFindSlotNoOfPRMtlNameList(tzPRJobID, i+1, bIsSlotNo))	continue;
		/* �˻� �����ߴ��� ���ο� ���� */
		if (!bIsSlotNo)	continue;

		wmemset(tzProcess, 0x00, 32);
		wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);
		m_lst_ctl[0].GetItemText(i, 0, tzSubstID, CIM_SUBSTRATE_ID);
		uvCIMLib_E90STS_GetSubstrateProcessingState(tzSubstID, enSubstState);
		
		m_lst_ctl[0].GetItemText(i, 1, tzMapState, 32);
		m_lst_ctl[0].GetItemText(i, 2, tzProcess, 32);
		if (enSubstState == E90_SSPS::en_needs_processing || enSubstState == E90_SSPS::en_no_state)
		{
			return i;
		}
	}

	return -1;
}

VOID CDlgSIM3::SetIDStateAndAcquiredID(INT32 slot_no, PTCHAR subst_id)
{
	INT32 i32Rand		= 0;
	CHAR szAcqSetID[32]	= {NULL};
	TCHAR tzAcqGetID[32]= {NULL};
	CUniToChar csCnv;
	TCHAR tzIDStatus[5][32]	= { L"No State", L"Not Confirmed", L"Waiting for Host", L"Confirmed", L"Confirmat Failed"};
	E90_SISM enIDState[5]	= { E90_SISM::en_no_state, E90_SISM::en_not_confirmed,
								E90_SISM::en_waiting_for_host, E90_SISM::en_confirmed,
								E90_SISM::en_confirmation_failed };

	/* Wafer Serial Info ���� ���� �� ���� (������ �ݹ��Լ��κ��� ������ �;� ������, ���⼭�� �׳� �����ϰ�...) */
	uvCmn_GetRandString(0x01, szAcqSetID, 12);
	/* Host (CIM300)���� Wafer Serial (Reading) �� ���� ��û (���⼭�� �ϴ� ��û�ϰ�) */
	uvCIMLib_E90STS_SetNotifySubstrateRead(subst_id, csCnv.Ansi2Uni(szAcqSetID), TRUE);	/* �ݵ��. Host���� ProceedWith... �Լ� ȣ���� ��� �� */
	/* ��� �ʿ��� ���������� �о��ٰ� ���� */
	i32Rand	= uvCmn_GetRandNumer(0x00, 1) % 5;
	uvCIMLib_E90STS_SetSubstrateIDStatus(subst_id, enIDState[i32Rand]);
	m_lst_ctl[0].SetItemText(slot_no, 3, tzIDStatus[i32Rand]);
	/* Acquired ID ��� */
	if (uvCIMLib_E90STS_GetSubstrateIDToAcquiredID(subst_id, tzAcqGetID, 32))
	{
		m_lst_ctl[0].SetItemText(slot_no, 4, tzAcqGetID);
	}
}

VOID CDlgSIM3::OnBnClickedExpose(UINT32 id)
{
	TCHAR tzCarrID[CIM_CARRIER_ID_SIZE]			= {NULL};
	TCHAR tzSubstID[CIM_SUBSTRATE_ID]			= {NULL};
	TCHAR tzPRJobID[CIM_PROCESS_JOB_ID_SIZE]	= {NULL};
	TCHAR tzRecipeID[CIM_PRJOB_RECIPE_ID_SIZE]	= {NULL};
	TCHAR tzModName[CIM_MODULE_NAME_SIZE]		= {NULL};
	TCHAR tzStepName[CIM_MODULE_STEP_SIZE]		= {NULL};

	if (!GetSelectPRJobID(tzPRJobID))
	{
		AfxMessageBox(L"The process job not selected", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	if (!uvCIMLib_E40PJM_GetRecipeID(tzPRJobID, tzRecipeID, CIM_PRJOB_RECIPE_ID_SIZE))	return;
	((CEdit *)GetDlgItem(IDC_SIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_CARRIER_ID_SIZE);
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_STAGE_LOAD))->GetWindowText(tzSubstID, CIM_SUBSTRATE_ID);

	swprintf_s(tzModName, CIM_MODULE_NAME_SIZE, L"%s", uvCIMLib_GetEquipLocationName(ENG_TSSI::en_process_chamber));
	swprintf_s(tzStepName, CIM_MODULE_STEP_SIZE, L"LithographStep%d", m_u8StepMPT);
	switch (id)
	{
	case IDC_SIM3_BTN_EPT_BUSY			:
		uvCIMLib_E116EPT_SetBusy(tzModName, L"Lithography", E116_ETBS::en_ept_process);	m_u8StepMPT	= 0x00;	break;
	case IDC_SIM3_BTN_EPT_EXE_START		:
		uvCIMLib_E157MPT_MPTExecutionStarted(tzModName, tzPRJobID, tzRecipeID, tzSubstID);	m_u8StepMPT	= 0x01;	break;
	case IDC_SIM3_BTN_EPT_SCAN_1_START	:
		uvCIMLib_E157MPT_MPTStepStarted(tzModName, tzStepName);	break;
	case IDC_SIM3_BTN_EPT_SCAN_1_COMP	:
		uvCIMLib_E157MPT_MPTStepCompleted(tzModName, TRUE);	m_u8StepMPT++;	break;
	case IDC_SIM3_BTN_EPT_SCAN_2_START	:	
		uvCIMLib_E157MPT_MPTStepStarted(tzModName, tzStepName);	break;
	case IDC_SIM3_BTN_EPT_SCAN_2_COMP	:
		uvCIMLib_E157MPT_MPTStepCompleted(tzModName, TRUE);	m_u8StepMPT++;	break;
	case IDC_SIM3_BTN_EPT_EXE_COMP		:
		uvCIMLib_E157MPT_MPTExecutionCompleted(tzModName, TRUE);	break;
	case IDC_SIM3_BTN_EPT_IDLE			:
		uvCIMLib_E116EPT_SetIdle(tzModName);	break;
	}
}

void CDlgSIM3::OnBnClickedSim3BtnJobProcess()
{
	INT32 i32PortNum = 1, i32SlotNum = 0;
	CHAR szAcqSetID[32]	= {NULL};
	TCHAR tzLoc[32]		= {NULL}, tzCarrID[CIM_CARRIER_ID_SIZE] = {NULL}, tzPort[32]	= {NULL};
	TCHAR tzSubstID[CIM_SUBSTRATE_ID] = {NULL}, tzDestID[CIM_SUBSTRATE_ID] = {NULL};
	TCHAR tzPRJobID[CIM_PROCESS_JOB_ID_SIZE]	= {NULL};
	E40_PPJS enPrjJobState;
	E90_SSPS enSubstState;
	CEdit *pEditBuffer	= (CEdit*)GetDlgItem(IDC_SIM3_EDT_BUFFER_LOAD);
	CEdit *pEditAligner	= (CEdit*)GetDlgItem(IDC_SIM3_EDT_ALIGNER_LOAD);
	CEdit *pEditRobot	= (CEdit*)GetDlgItem(IDC_SIM3_EDT_ROBOT_LOAD);
	CEdit *pEditStage	= (CEdit*)GetDlgItem(IDC_SIM3_EDT_STAGE_LOAD);

	((CEdit *)GetDlgItem(IDC_SIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_CARRIER_ID_SIZE);
	((CEdit *)GetDlgItem(IDC_SIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPort, 32);
	if (_wtoi(tzPort) == 1)	i32PortNum	= 1;
	else					i32PortNum	= 2;

	/* Process Job ���°� Start ��尡 �ƴϸ� ���� ���� ó�� */
	if (!GetSelectPRJobID(tzPRJobID))	return;
	if (!uvCIMLib_E40PJM_GetProcessJobState(tzPRJobID, enPrjJobState))	return;
	/* ��� �۾��� �Ϸ� �Ǿ��ٸ�... ���� */
	if (enPrjJobState == E40_PPJS::en_stopped || enPrjJobState == E40_PPJS::en_processjob_completed)
	{
		AfxMessageBox(L"All process job have been completed", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	if (enPrjJobState != E40_PPJS::en_setting_up && enPrjJobState != E40_PPJS::en_waiting_for_start &&
		// Anderson 0518
		enPrjJobState != E40_PPJS::en_pausing && //
		enPrjJobState != E40_PPJS::en_paused && //
		enPrjJobState != E40_PPJS::en_stopping && //
		enPrjJobState != E40_PPJS::en_aborting && //
		enPrjJobState != E40_PPJS::en_processing)
	{
		AfxMessageBox(L"The process job is not ready to start", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
#if 0
	/* ���� Processing Job�� Substrate�� ��� ó���Ǿ����� ���� */
	if (uvCIMLib_Mixed_IsSubstrateProcessCompleted(tzPRJobID))	return;
#else
	/* ���� Processing Job�� ���� Substrate ID�� Transport ��ġ�� ������ (FOUP)�� ��� ��ȯ �Ǿ� �ִ��� ���� */
	if (uvCIMLib_Mixed_IsSubstrateTransportDestination(tzPRJobID))	return;
#endif

	// Anderson 0518
	bool canPickNewWafer = true;
	switch (enPrjJobState)
	{
	case E40_PPJS::en_pausing:
	case E40_PPJS::en_paused:
	case E40_PPJS::en_stopping:
	case E40_PPJS::en_aborting:
		canPickNewWafer = false;
		break;
	}

	switch (m_u8StepMain)
	{
	case 0x00	:
		switch (m_u8StepSub)
		{
		case 0x00	: /* FOUP -> ROBOT */
			if (canPickNewWafer == true)
			{
				/* FOUP���κ��� ��� �ִ� Slot Number �������� */
				i32SlotNum = GetNotEmptySlotNumInCarrier();	/* ���� ó���ؾ� �� Wafer ��ġ ��� */
				if (i32SlotNum >= 0)
				{
					wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);
					m_lst_ctl[0].GetItemText(i32SlotNum, 0, tzSubstID, CIM_SUBSTRATE_ID);
					m_lst_ctl[0].SetItemText(i32SlotNum, 1, L"empty");
					pEditRobot->SetWindowText(tzSubstID);
					/* Substrate 1�� Carrier���� ���� �����Ƿ�, ���� Carrier�� ��ġ�� �ִ� �׸� ����*/
					uvCIMLib_E87CMS_SetCarrierPopSubstrate(tzCarrID, i32SlotNum + 1);
					/* Substrate ���� ��ġ ���� : [ Robot : Yes ] */
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_occupied);
					/* Substrate �۾� ��ġ ���� : [ Robot ] */
					uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, ENG_TSSI::en_robot,
																 E90_SSTS::en_work,
																 E90_SSPS::en_needs_processing);
					/* Set the substrateID state */
					uvCIMLib_E90STS_GetSubstrateProcessingState(tzSubstID, enSubstState);
					m_lst_ctl[0].SetItemText(i32SlotNum, 2, uvCIMLib_GetEquipProcessingName(enSubstState));
				}
			}
			m_u8StepSub++;
			return;

		case 0x01	:	/* ROBOT -> PRE_ALIGNER */
			if (HasWafer(ENG_TSSI::en_robot) == TRUE)
			{
				wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);	pEditRobot->GetWindowText(tzSubstID, CIM_SUBSTRATE_ID);
				pEditAligner->SetWindowText(tzSubstID);		pEditRobot->SetWindowText(L"empty");

				/* Substrate ���� ��ġ ���� : [ PreAligner : Yes ] / [ Robot : No ] */
				uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_pre_aligner, E90_SSLS::en_occupied);
				uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_unoccupied);
				/* Substrate �۾� ��ġ ���� : [ PreAligner ] */
				uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, ENG_TSSI::en_pre_aligner, E90_SSTS::en_work, E90_SSPS::en_needs_processing);
				/* Wafer Serial �� ���� �� Substrate ID ���� */
				i32SlotNum = GetFindSubstrateIDtoSlotNo(0x00, tzSubstID);
				SetIDStateAndAcquiredID(i32SlotNum, tzSubstID);
			}
			m_u8StepSub++;
			return;

		case 0x02	:	/* PRE_ALIGNER -> ROBOT */
			if (HasWafer(ENG_TSSI::en_pre_aligner) == TRUE)
			{
				wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);	pEditAligner->GetWindowText(tzSubstID, CIM_SUBSTRATE_ID);
				pEditRobot->SetWindowText(tzSubstID);		pEditAligner->SetWindowText(L"empty");	
				/* Substrate ��ġ ���� : [ Robot : Yes ] / [ PreAligner : No ] */
				uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_occupied);
				uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_pre_aligner, E90_SSLS::en_unoccupied);
				/* Substrate �۾� ��ġ ���� : [ Robot ] */
				uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, ENG_TSSI::en_robot, E90_SSTS::en_work, E90_SSPS::en_needs_processing);
			}
			m_u8StepSub++;
			return;

		case 0x03	:	/* ROBOT -> STAGE */
			if (HasWafer(ENG_TSSI::en_robot) == TRUE)
			{
				wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);	pEditRobot->GetWindowText(tzSubstID, CIM_SUBSTRATE_ID);
				pEditStage->SetWindowText(tzSubstID);		pEditRobot->SetWindowText(L"empty");
				i32SlotNum = GetFindSubstrateIDtoSlotNo(0x00, tzSubstID);
				/* Substrate ��ġ ���� : [ Chamber : Yes ] / [ Robot : No ] */
				uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_unoccupied);
				uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_process_chamber, E90_SSLS::en_occupied);
				/* Substrate �۾� ��ġ ���� : [ Chamber ] */
				uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, ENG_TSSI::en_process_chamber, E90_SSTS::en_work, E90_SSPS::en_in_process);
				/* Set the substrateID state */
				uvCIMLib_E90STS_GetSubstrateProcessingState(tzSubstID, enSubstState);
				m_lst_ctl[0].SetItemText(i32SlotNum, 2, uvCIMLib_GetEquipProcessingName(enSubstState));
			}
			m_u8StepSub++;
			return;

		case 0x04	: /* FOUP -> ROBOT */
			if (canPickNewWafer == true)
			{
				/* FOUP���κ��� ��� �ִ� Slot Number �������� */
				i32SlotNum = GetNotEmptySlotNumInCarrier();	/* ���� ó���ؾ� �� Wafer ��ġ ��� */
				if (i32SlotNum >= 0)
				{
					wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);
					m_lst_ctl[0].GetItemText(i32SlotNum, 0, tzSubstID, CIM_SUBSTRATE_ID);
					m_lst_ctl[0].SetItemText(i32SlotNum, 1, L"empty");
					pEditRobot->SetWindowText(tzSubstID);
					/* Substrate 1�� Carrier���� ���� �����Ƿ�, ���� Carrier�� ��ġ�� �ִ� �׸� ����*/
					uvCIMLib_E87CMS_SetCarrierPopSubstrate(tzCarrID, i32SlotNum + 1);
					/* Substrate ���� ��ġ ���� : [ Robot : Yes ] */
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_occupied);
					/* Substrate �۾� ��ġ ���� : [ Robot ] */
					uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, ENG_TSSI::en_robot, E90_SSTS::en_work, E90_SSPS::en_needs_processing);
					/* Set the substrateID state */
					uvCIMLib_E90STS_GetSubstrateProcessingState(tzSubstID, enSubstState);
					m_lst_ctl[0].SetItemText(i32SlotNum, 2, uvCIMLib_GetEquipProcessingName(enSubstState));
				}
			}
			m_u8StepSub++;
			return;

		case 0x05	:	/* ROBOT -> PRE_ALIGNER */
			if (HasWafer(ENG_TSSI::en_robot) == TRUE)
			{
				wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);	pEditRobot->GetWindowText(tzSubstID, CIM_SUBSTRATE_ID);
				pEditAligner->SetWindowText(tzSubstID);		pEditRobot->SetWindowText(L"empty");	m_u8StepSub = 0;
				/* Substrate ���� ��ġ ���� : [ PreAligner : Yes ] / [ Robot : No ] */
				uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_pre_aligner, E90_SSLS::en_occupied);
				uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_unoccupied);
				/* Substrate �۾� ��ġ ���� : [ PreAligner ] */
				uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, ENG_TSSI::en_pre_aligner, E90_SSTS::en_work, E90_SSPS::en_needs_processing);
				/* Wafer Serial �� ���� �� Substrate ID ����  */
				i32SlotNum = GetFindSubstrateIDtoSlotNo(0x00, tzSubstID);
				SetIDStateAndAcquiredID(i32SlotNum, tzSubstID);
			}
			m_u8StepMain++;
			return;
		}
		return;

	case 0x01	:
		switch (m_u8StepSub)
		{
		case 0x00	:	/* STAGE -> ROBOT */
			if (HasWafer(ENG_TSSI::en_process_chamber) == TRUE)
			{
				wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);	pEditStage->GetWindowText(tzSubstID, CIM_SUBSTRATE_ID);
				if (0 != wcscmp(tzSubstID, L"empty"))
				{
					pEditRobot->SetWindowText(tzSubstID);		pEditStage->SetWindowText(L"empty");
					i32SlotNum = GetFindSubstrateIDtoSlotNo(0x00, tzSubstID);
					/* Substrate ���� ��ġ ���� : [ Robot : Yes ] / [ Chamber : No ] */
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_occupied);
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_process_chamber, E90_SSLS::en_unoccupied);
					/* Substrate �۾� ��ġ ���� : [ Robot ] */
					uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, ENG_TSSI::en_robot, E90_SSTS::en_work, E90_SSPS::en_processed);
					/* Set the substrateID state */
					uvCIMLib_E90STS_GetSubstrateProcessingState(tzSubstID, enSubstState);
					m_lst_ctl[0].SetItemText(i32SlotNum, 2, uvCIMLib_GetEquipProcessingName(enSubstState));
				}
			}
			m_u8StepSub++;
			return;

		case 0x01	:	/* ROBOT -> BUFFER */
			if (HasWafer(ENG_TSSI::en_robot) == TRUE)
			{
				wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);	pEditRobot->GetWindowText(tzSubstID, CIM_SUBSTRATE_ID);
				pEditBuffer->SetWindowText(tzSubstID);		pEditRobot->SetWindowText(L"empty");
				/* Substrate ���� ��ġ ���� : [ Buffer : Yes ] / [ Robot : No ] */
				uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_buffer, E90_SSLS::en_occupied);
				uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_unoccupied);
				/* Substrate �۾� ��ġ ���� : [ Buffer ] */
				uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, ENG_TSSI::en_buffer, E90_SSTS::en_work, E90_SSPS::en_in_process);
			}
			m_u8StepSub++;
			return;

		case 0x02	:	/* PRE_ALIGNER -> ROBOT */
			if (HasWafer(ENG_TSSI::en_pre_aligner) == TRUE)
			{
				wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);	pEditAligner->GetWindowText(tzSubstID, CIM_SUBSTRATE_ID);
				if (0 != wcscmp(tzSubstID, L"empty"))
				{
					pEditRobot->SetWindowText(tzSubstID);		pEditAligner->SetWindowText(L"empty");
					/* Substrate ���� ��ġ ���� : [ Robot : Yes ] / [ PreAligner : No ] */
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_occupied);
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_pre_aligner, E90_SSLS::en_unoccupied);
					/* Substrate �۾� ��ġ ���� : [ Robot ] */
					uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, ENG_TSSI::en_robot, E90_SSTS::en_work, E90_SSPS::en_in_process);
				}
			}
			m_u8StepSub++;
			return;

		case 0x03	:	/* ROBOT -> STAGE */
			if (HasWafer(ENG_TSSI::en_robot) == TRUE)
			{
				wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);	pEditRobot->GetWindowText(tzSubstID, CIM_SUBSTRATE_ID);
				if (0 != wcscmp(tzSubstID, L"empty"))
				{
					pEditStage->SetWindowText(tzSubstID);		pEditRobot->SetWindowText(L"empty");
					i32SlotNum = GetFindSubstrateIDtoSlotNo(0x00, tzSubstID);
					/* Substrate ���� ��ġ ���� : [ Chamber : Yes ] / [ Robot : No ] */
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_process_chamber, E90_SSLS::en_occupied);
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_unoccupied);
					/* Substrate �۾� ��ġ ���� : [ Chamber ] */
					uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, ENG_TSSI::en_process_chamber, E90_SSTS::en_work, E90_SSPS::en_in_process);
					/* Set the substrateID state */
					uvCIMLib_E90STS_GetSubstrateProcessingState(tzSubstID, enSubstState);
					m_lst_ctl[0].SetItemText(i32SlotNum, 2, uvCIMLib_GetEquipProcessingName(enSubstState));
				}
			}
			m_u8StepSub++;
			return;

		case 0x04	: /* FOUP -> ROBOT */
			if (canPickNewWafer == true)
			{
				/* FOUP���κ��� ��� ���� ���� Slot Number �������� */
#if 0
				i32SlotNum = GetNotEmptySlotNumInCarrier();	/* ���� ó���ؾ� �� Wafer ��ġ ��� */
#else
				i32SlotNum = GetNeedProcessingSlotNumInCarrier();
#endif
				/* Slot Number�� �����̰ų�, Process Job ID�� ��ϵ� �������� ũ�� �� �̻� ó������ �ʴ´�. */
				if (i32SlotNum >= 0)
				{
					wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);
					m_lst_ctl[0].GetItemText(i32SlotNum, 0, tzSubstID, CIM_SUBSTRATE_ID);
					m_lst_ctl[0].SetItemText(i32SlotNum, 1, L"empty");
					pEditRobot->SetWindowText(tzSubstID);
					/* Substrate 1�� Carrier���� ���� �����Ƿ�, ���� Carrier�� ��ġ�� �ִ� �׸� ����*/
					uvCIMLib_E87CMS_SetCarrierPopSubstrate(tzCarrID, i32SlotNum + 1);
					/* Substrate ���� ��ġ ���� : [ Robot : Yes ] */
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_occupied);
					/* Substrate �۾� ��ġ ���� : [ Robot ] */
					uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, ENG_TSSI::en_robot, E90_SSTS::en_work, E90_SSPS::en_needs_processing);
					/* Set the substrateID state */
					uvCIMLib_E90STS_GetSubstrateProcessingState(tzSubstID, enSubstState);
					m_lst_ctl[0].SetItemText(i32SlotNum, 2, uvCIMLib_GetEquipProcessingName(enSubstState));
				}
			}
			m_u8StepSub++;
			return;

		case 0x05	:	/* ROBOT -> PRE_ALIGNER */
			if (HasWafer(ENG_TSSI::en_robot) == TRUE)
			{
				wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);	pEditRobot->GetWindowText(tzSubstID, CIM_SUBSTRATE_ID);
				if (0 != wcscmp(tzSubstID, L"empty"))
				{
					pEditAligner->SetWindowText(tzSubstID);		pEditRobot->SetWindowText(L"empty");
					/* Substrate ���� ��ġ ���� : [ PreAligner : Yes ] / [ Robot : No ] */
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_pre_aligner, E90_SSLS::en_occupied);
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_unoccupied);
					/* Substrate ���� ��ġ ���� : [ PreAligner : Yes ] */
					uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, ENG_TSSI::en_pre_aligner, E90_SSTS::en_work, E90_SSPS::en_needs_processing);
					/* Wafer Serial �� ���� �� Substrate ID ���� */
					i32SlotNum = GetFindSubstrateIDtoSlotNo(0x00, tzSubstID);
					SetIDStateAndAcquiredID(i32SlotNum, tzSubstID);
				}
			}
			m_u8StepSub++;
			return;

		case 0x06	:	/* BUFFER -> ROBOT */
			if (HasWafer(ENG_TSSI::en_buffer) == TRUE)
			{
				wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);	pEditBuffer->GetWindowText(tzSubstID, CIM_SUBSTRATE_ID);
				if (0 != wcscmp(tzSubstID, L"empty"))
				{
					pEditRobot->SetWindowText(tzSubstID);		pEditBuffer->SetWindowText(L"empty");
					/* Substrate ���� ��ġ ���� : [ Robot : Yes ] / [ Buffer : No ] */
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_occupied);
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_buffer, E90_SSLS::en_unoccupied);
					/* Substrate ���� ��ġ ���� : [ Robot : Yes ] */
					uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, ENG_TSSI::en_robot, E90_SSTS::en_work, E90_SSPS::en_processed);
				}
			}
			m_u8StepSub++;
			return;

		case 0x07	: /* ROBOT -> FOUP */
			if (HasWafer(ENG_TSSI::en_robot) == TRUE)
			{
				/* FOUP���κ��� ��� �ִ� Slot Number �������� */
				wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);	pEditRobot->GetWindowText(tzSubstID, CIM_SUBSTRATE_ID);
				if (0 != wcscmp(tzSubstID, L"empty"))
				{
					/* Destination�� ��ġ�ϴ� Slot Number �������� */
					i32SlotNum = GetFindSubstrateIDtoSlotNo(0x01, tzSubstID);
					if (i32SlotNum >= 0)
					{
						m_lst_ctl[0].SetItemText(i32SlotNum, 1, L"occupied");
						uvCIMLib_E90STS_GetSubstrateProcessingState(tzSubstID, enSubstState);
						m_lst_ctl[0].SetItemText(i32SlotNum, 2, uvCIMLib_GetEquipProcessingName(enSubstState));
					}
					pEditRobot->SetWindowText(L"empty");

					/* Substrate ���� ��ġ ���� : [ Robot : No ] */
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_unoccupied);
					/* Substrate ���� ��ġ ���� : [ Foup : Yes ] */
					uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, ENG_TSSI::en_robot, E90_SSTS::en_destination, E90_SSPS::en_processed);
					/* ������ Substrate�� ��ȯ�� ��ġ ��� */
					wmemset(tzDestID, 0x00, CIM_SUBSTRATE_ID);
					uvCIMLib_Mixed_GetSubstrateDestinationID(tzCarrID, UINT8(i32SlotNum + 1), tzDestID, CIM_SUBSTRATE_ID);
					/* Substrate FOUP ��ġ ���� : FOUP (Carrier)�� Ư�� ��ġ�� ���� �ֱ� */
					uvCIMLib_E87CMS_SetSubstratePlaced(tzCarrID, tzDestID, UINT8(i32SlotNum + 1));
				}
			}
			m_u8StepSub=0x00;
			return;
		}
		break;
	}
}

// Anderson 0518
BOOL CDlgSIM3::HasWafer(ENG_TSSI location_id)
{
	BOOL ret = uvCIMLib_E90STS_IsLocationSubstrateID(location_id);

	return ret;
}


void CDlgSIM3::OnBnClickedSim3BtnProcWithSubst()
{
	TCHAR tzSubstID[CIM_SUBSTRATE_ID]	= {NULL};

	POSITION pPos	= m_lst_ctl[0].GetFirstSelectedItemPosition();
	if (!pPos)	return;
	INT32 iItem	= m_lst_ctl[0].GetNextSelectedItem(pPos);
	if (iItem < 0)
	{
		AfxMessageBox(L"Substrate is not selected in the list_control", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	m_lst_ctl[0].GetItemText(iItem, 0, tzSubstID, CIM_SUBSTRATE_ID);
	uvCIMLib_E90STS_SetProceedWithSubstrate(tzSubstID);
}

BOOL CDlgSIM3::GetSelectPRJobID(PTCHAR pj_id)
{
	CListBox *pBoxPRJob	= ((CListBox*)GetDlgItem(IDC_SIM3_BOX_PRJOB));
	INT32 i32SelPos	= pBoxPRJob->GetCurSel();
	if (i32SelPos < 0)
	{
		AfxMessageBox(L"No item selected (Item:Process Job)", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	pBoxPRJob->GetText(i32SelPos, pj_id);

	return wcslen(pj_id) > 0;
}

void CDlgSIM3::LoadSubstIDForProcessJob()
{
	CListBox *pBoxPRJob	= ((CListBox*)GetDlgItem(IDC_SIM3_BOX_PRJOB));
	CStringArray arrCarrID;
	CByteArray arrSlotNo;

	INT32 i32SelPos	= pBoxPRJob->GetCurSel();
	if (i32SelPos < 0)
	{
		AfxMessageBox(L"No item selected (Item:Process Job)", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	CListBox *pBox	= (CListBox*)GetDlgItem(IDC_SIM3_BOX_CARR_SLOT);
	pBox->ResetContent();

	TCHAR tzPrJobID[CIM_PROCESS_JOB_ID_SIZE]	= {NULL};
	pBoxPRJob->GetText(i32SelPos, tzPrJobID);
	if (!uvCIMLib_E40PJM_GetPRMtlNameList(tzPrJobID, arrCarrID, arrSlotNo))	return;

	TCHAR tzSlotID[256]	= {NULL};
	for (INT32 i=0; i<arrCarrID.GetCount(); i++)
	{
		swprintf_s(tzSlotID, 256, L"%s.%02d", arrCarrID[i].GetBuffer(), arrSlotNo[i]);
		pBox->AddString(tzSlotID);
		/* Destination SubstrateID ���� */
		LoadDestinateSubstrateID(arrCarrID[i].GetBuffer(), (UINT8)arrSlotNo[i]);
	}

	arrSlotNo.RemoveAll();
	arrCarrID.RemoveAll();
}

void CDlgSIM3::OnBnClickedSim3BtnPrjobStart()
{
	CStringArray arrPRJobs;
	CListBox *pBoxPRJob	= ((CListBox*)GetDlgItem(IDC_SIM3_BOX_PRJOB));
	INT32 i32SelPos	= pBoxPRJob->GetCurSel();
	if (i32SelPos < 0)
	{
		AfxMessageBox(L"No item selected (Item:Process Job)", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	TCHAR tzPRJobID[CIM_PROCESS_JOB_ID_SIZE]	= {NULL};
	pBoxPRJob->GetText(i32SelPos, tzPRJobID);
	arrPRJobs.Add(tzPRJobID);

	/* �Ʒ� ���� �ٲٸ� �ȵ� */
	if (!uvCIMLib_E40PJM_SetPRJobSetStartMethod(&arrPRJobs, TRUE))	return;
	if (!uvCIMLib_E40PJM_SetProcessJobQueueToSetup(tzPRJobID))		return;
	if (!uvCIMLib_E40PJM_SetPRJobStartProcess(tzPRJobID))			return;
//	if (!uvCIMLib_E40PJM_SetProcessJobState(tzPRJobID, E40_PPJS::en_processing))	return;	/* ������ ������ �ָ� �ȵ� */
}

void CDlgSIM3::OnBnClickedSim3BtnPrjobProcComplete()
{
	CListBox *pBoxPRJob	= ((CListBox*)GetDlgItem(IDC_SIM3_BOX_PRJOB));
	INT32 i32SelPos	= pBoxPRJob->GetCurSel();
	if (i32SelPos < 0)
	{
		AfxMessageBox(L"No item selected (Item:Process Job)", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	TCHAR tzPRJobID[CIM_PROCESS_JOB_ID_SIZE]	= {NULL};
	pBoxPRJob->GetText(i32SelPos, tzPRJobID);

	/* �Ʒ� ���� �ٲٸ� �ȵ� */
	uvCIMLib_E40PJM_SetPRJobProcessComplete(tzPRJobID);
//	uvCIMLib_E40PJM_SetProcessJobState(tzPrJobID, E40_PPJS::en_process_completed);	/* ������ ������ �ָ� �ȵ� */
}

void CDlgSIM3::OnBnClickedSim3BtnPrjobCompleted()
{
	TCHAR tzPortID[CIM_VAL_ASCII_SIZE]	= {NULL};
	TCHAR tzCarrID[CIM_CARRIER_ID_SIZE]	= {NULL};
	CListBox *pBoxPRJob	= ((CListBox*)GetDlgItem(IDC_SIM3_BOX_PRJOB));
	CListBox *pBoxCRJob	= ((CListBox*)GetDlgItem(IDC_SIM3_BOX_CRJOB));
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	((CEdit *)GetDlgItem(IDC_SIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_CARRIER_ID_SIZE);
	INT32 i32SelPos	= pBoxPRJob->GetCurSel();
	if (i32SelPos < 0)
	{
		AfxMessageBox(L"No item selected (Item:Process Job)", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	TCHAR tzPrJobID[CIM_PROCESS_JOB_ID_SIZE]	= {NULL};
	pBoxPRJob->GetText(i32SelPos, tzPrJobID);

	/* Process Job Complete */
	uvCIMLib_E40PJM_SetPRJobComplete(tzPrJobID);
	/* Control Job �ʿ��� �˷���� ��  */
	if ((i32SelPos = pBoxCRJob->GetCurSel()) >= 0)
	{
		/* Process Job Process Complete */
		uvCIMLib_E94CJM_SetPRJobProcessComplete(tzPrJobID);
	}

	/* Carrier Access Complete (Carrier Closed) */
	if (!uvCIMLib_E87CMS_SetAccessCarrier(COMMN_CONN_ID, tzCarrID, (UINT16)_wtoi(tzPortID),
										  E87_CCAS::en_carrier_complete))	return;
	/* Set the Carrier Event */
	uvCIMLib_E87CMS_SetCarrierEvent(COMMN_CONN_ID, tzCarrID, (UINT16)_wtoi(tzPortID),
									E87_CCES::en_carrier_closed);
}

void CDlgSIM3::OnBnClickedSim3BtnReleaseCarrier()
{
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL};
	((CEdit *)GetDlgItem(IDC_SIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_CARRIER_ID_SIZE);
	uvCIMLib_E87CMS_SetCarrierRelease(tzCarrID);
}

/*
 desc : Source SubstrateID�� ���� Destination SubstrateID ���
 parm : carr_id	- [in]  Carrier ID
		slot_no	- [in]  Slot Number
 retn : None
*/
VOID CDlgSIM3::LoadDestinateSubstrateID(PTCHAR carr_id, UINT8 slot_no)
{
	TCHAR tzSubstID[CIM_SUBSTRATE_ID]	= {NULL};
	
	if (!uvCIMLib_Mixed_GetSubstrateDestinationID(carr_id, slot_no, tzSubstID, CIM_SUBSTRATE_ID))	return;
	if (slot_no < 1)	return;
	m_lst_ctl[0].SetItemText(slot_no-1, 6, tzSubstID);
}

/*
 desc : ���� SubstrateID�� ��ġ�Ǵ� Index (Slot Number) �� ��ȯ
 parm : type		- [in]  0x00: Source SubstrateID �˻�, 0x01: Destination SubstrateID �˻�
		subst_id	- [in]  Source Soubstrate ID ��
 retn : 0 based Index, ������ ��� -1
*/
INT32 CDlgSIM3::GetFindSubstrateIDtoSlotNo(UINT8 type, PTCHAR subst_id)
{
	TCHAR tzSubstID[CIM_SUBSTRATE_ID]	= {NULL};
	INT32 i	= 0, i32Item = 0;

	if (wcslen(subst_id) < 1)	return -1;

	i32Item	= m_lst_ctl[0].GetItemCount();
	for (i=0; i<i32Item; i++)
	{
		wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);
		if (type == 0x00)	m_lst_ctl[0].GetItemText(i, 0, tzSubstID, CIM_SUBSTRATE_ID);
		else				m_lst_ctl[0].GetItemText(i, 6, tzSubstID, CIM_SUBSTRATE_ID);
		if (0 == wcscmp(subst_id, tzSubstID))
		{
			return i;
		}
	}

	return -1;
}

void CDlgSIM3::OnBnClickedSim3BtnPrjobStop()
{
	CListBox *pBoxPRJob	= ((CListBox*)GetDlgItem(IDC_SIM3_BOX_PRJOB));
	INT32 i32SelPos	= pBoxPRJob->GetCurSel();
	if (i32SelPos < 0)
	{
		AfxMessageBox(L"No item selected (Item:Process Job)", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	TCHAR tzPrJobID[CIM_PROCESS_JOB_ID_SIZE]	= {NULL};
	pBoxPRJob->GetText(i32SelPos, tzPrJobID);

	uvCIMLib_E40PJM_SetPRJobStateControl(tzPrJobID, E40_PPJC::en_stop);
}

void CDlgSIM3::OnBnClickedSim3BtnPrjobAbort()
{
	CListBox *pBoxPRJob	= ((CListBox*)GetDlgItem(IDC_SIM3_BOX_PRJOB));
	INT32 i32SelPos	= pBoxPRJob->GetCurSel();
	if (i32SelPos < 0)
	{
		AfxMessageBox(L"No item selected (Item:Process Job)", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	TCHAR tzPrJobID[CIM_PROCESS_JOB_ID_SIZE]	= {NULL};
	pBoxPRJob->GetText(i32SelPos, tzPrJobID);

	uvCIMLib_E40PJM_SetPRJobStateControl(tzPrJobID, E40_PPJC::en_abort);
}

void CDlgSIM3::OnBnClickedSim3BtnPrjobRemove()
{
	CListBox *pBoxPRJob	= ((CListBox*)GetDlgItem(IDC_SIM3_BOX_PRJOB));
	INT32 i32SelPos	= pBoxPRJob->GetCurSel();
	if (i32SelPos < 0)
	{
		AfxMessageBox(L"No item selected (Item:Process Job)", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	TCHAR tzPrJobID[CIM_PROCESS_JOB_ID_SIZE]	= {NULL};
	pBoxPRJob->GetText(i32SelPos, tzPrJobID);

	uvCIMLib_E40PJM_SetProcessJobRemove(tzPrJobID);
}

VOID CDlgSIM3::UpdateProcessJobList()
{
	INT32 i = 0;
	CStringArray arrPRJob;
	CAtlList <E40_PPJS> lstState;

	CListBox *pBoxJobList	= (CListBox*)GetDlgItem(IDC_SIM3_BOX_PRJOB);
	pBoxJobList->ResetContent();
	if (!uvCIMLib_E40PJM_GetProcessAllJobIdState(arrPRJob, lstState))	return;
	for (; i<arrPRJob.GetCount(); i++)
	{
		pBoxJobList->AddString(arrPRJob.GetAt(i));
	}
}

VOID CDlgSIM3::UpdateControlJobList()
{
	INT32 i = 0;
	CStringArray arrCRJob;
	CAtlList <E94_CCJS> lstState;

	CListBox *pBoxJobList	= (CListBox*)GetDlgItem(IDC_SIM3_BOX_CRJOB);
	pBoxJobList->ResetContent();
	if (!uvCIMLib_E94CJM_GetControlJobList(arrCRJob, lstState))	return;
	for (; i<arrCRJob.GetCount(); i++)
	{
		pBoxJobList->AddString(arrCRJob.GetAt(i));
	}
}

void CDlgSIM3::OnLbnSelchangeSim3BoxCrjob()
{
	TCHAR tzCRJobID[CIM_CONTROL_JOB_ID_SIZE]	= {NULL};
	CListBox *pBoxCRJob	= (CListBox*)GetDlgItem(IDC_SIM3_BOX_CRJOB);
	CListBox *pBoxPRJob	= (CListBox*)GetDlgItem(IDC_SIM3_BOX_PRJOB);
	if (pBoxCRJob->GetCurSel() < 0)	return;

	pBoxCRJob->GetText(pBoxCRJob->GetCurSel(), tzCRJobID);

	INT32 i	= 0;
	CStringArray arrPRJob;

	pBoxPRJob->ResetContent();
	if (!uvCIMLib_E94CJM_GetProcessJobList(tzCRJobID, arrPRJob, E94_CLPJ::en_list_all))	return;
	for (; i<arrPRJob.GetCount(); i++)
	{
		pBoxPRJob->AddString(arrPRJob.GetAt(i));
	}
}


void CDlgSIM3::OnBnClickedSim3BtnPrjobUpdate()
{
	/* Update Process Job List */
	UpdateProcessJobList();
}


void CDlgSIM3::OnBnClickedSim3BtnCrjobUpdate()
{
	/* Update Control Job List */
	UpdateControlJobList();
}


void CDlgSIM3::OnLbnDblclkSim3BoxPrjob()
{
	LoadSubstIDForProcessJob();
}



void CDlgSIM3::OnBnClickedSim3BtnCarrReCreate()
{
	TCHAR tzPortID[CIM_VAL_ASCII_SIZE]	= {NULL};
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL};
	E87_LPTS enTransState;
	E87_LPAS enPortCarrierState;

	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_CARRIER_ID_SIZE);
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);

	if (!uvCIMLib_E87CMS_GetTransferStatus((UINT16)_wtoi(tzPortID), enTransState))	return;
	if (enTransState != E87_LPTS::en_lp_ready_to_unload)
	{
		AfxMessageBox(L"The carrier is not [Ready To Unload] status", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	if (!uvCIMLib_E87CMS_GetAssociationState((UINT16)_wtoi(tzPortID), enPortCarrierState))	return;
	if (enPortCarrierState == E87_LPAS::en_associated)
	{
		if (!uvCIMLib_E87CMS_SetCarrierReCreate((UINT16)_wtoi(tzPortID), tzCarrID))
		{
			AfxMessageBox(L"Failed to run the CarrierReCreate Func.", MB_ICONSTOP|MB_TOPMOST);
			return;
		}
	}
}

VOID CDlgSIM3::UpdateLoadPortCarrier()
{
	TCHAR tzPortID[CIM_VAL_ASCII_SIZE]	= {NULL};
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL};
	E87_LPTS enTransState;

	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_CARRIER_ID_SIZE);
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);

	/* �˶� Clear ? Which ? */

	/* Load Port ���� ���� : Loading ... */

	/* Transfer Status ���� */
	if (uvCIMLib_E87CMS_GetTransferStatus((UINT16)_wtoi(tzPortID), enTransState))
	{
	}

	/* ID Verification */
	/* Slot Map Verification */
	/* Association Status */
	/* Reservation Status */
	/* SIM300 UI�� �ִ� ���� ���� ���� */
}

void CDlgSIM3::OnBnClickedSim3BtnCancelCarrier()
{
	TCHAR tzPortID[CIM_VAL_ASCII_SIZE]	= {NULL};
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL};

	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_CARRIER_ID_SIZE);
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	uvCIMLib_E87CMS_SetCancelCarrier((UINT16)_wtoi(tzPortID), tzCarrID);
}

/*
 desc : Transfer Status, ID Verification, Slot Map Verify, Reservation, Association 
 parm : kind	- [in]  Update ����
						1 : transfer, 2 : carrier id, 3 : slot verify, 4 : reserve, 5 : associate
 retn : None
*/
VOID CDlgSIM3::UpdateBaseInformation(UINT8 kind)
{
	TCHAR tzPortID[CIM_VAL_ASCII_SIZE]	= {NULL};
	TCHAR tzCarrID[CIM_VAL_ASCII_SIZE]	= {NULL};
	UINT16 u16PortID		= 0;
	E87_LPTS enTransStatus	= E87_LPTS::en_lp_ready_to_unload;
	E87_CIVS enIDStatus		= E87_CIVS::en_id_not_read;
	E87_SMVS enSlotStatus	= E87_SMVS::en_sm_not_read;
	E87_LPRS enReserveStatus= E87_LPRS::en_lp_reserved;
	E87_LPAS enAssocStatus	= E87_LPAS::en_not_associated;
	CEdit *pEdit			= NULL;

	((CEdit*)GetDlgItem(IDC_SIM3_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_CARRIER_ID_SIZE);
	((CEdit*)GetDlgItem(IDC_SIM3_EDT_LOAD_PORT_ID))->GetWindowText(tzPortID, CIM_VAL_ASCII_SIZE);
	u16PortID	= (UINT16)_wtoi(tzPortID);

	/* Transfer Status */
	if (kind==1 && uvCIMLib_E87CMS_GetTransferStatus(u16PortID, enTransStatus))
	{
		pEdit	= (CEdit*)GetDlgItem(IDC_SIM3_EDT_TRANSER_STATE);
		pEdit->SetRedraw(FALSE);
		switch (enTransStatus)
		{
		case E87_LPTS::en_lp_out_of_service		:	pEdit->SetWindowText(L"OutOfService");		break;
		case E87_LPTS::en_lp_transfer_blocked	:	pEdit->SetWindowText(L"TransferBlocked");	break;
		case E87_LPTS::en_lp_ready_to_load		:	pEdit->SetWindowText(L"ReadyToLoad");		break;
		case E87_LPTS::en_lp_ready_to_unload		:	pEdit->SetWindowText(L"ReadyToUnload");		break;
		case E87_LPTS::en_lp_in_service			:	pEdit->SetWindowText(L"InService");			break;
		case E87_LPTS::en_lp_transfer_ready		:	pEdit->SetWindowText(L"TransferReady");		break;
		}
		pEdit->SetRedraw(TRUE);
		pEdit->Invalidate(FALSE);
	}
	/* Carrier ID Verification Status */
	if (kind==2 && uvCIMLib_E87CMS_GetCarrierIDStatus(tzCarrID, enIDStatus))
	{
		pEdit	= (CEdit*)GetDlgItem(IDC_SIM3_EDT_ID_VERIFICATION);
		pEdit->SetRedraw(FALSE);
		switch (enIDStatus)
		{
		case E87_CIVS::en_id_not_read			:	pEdit->SetWindowText(L"Not Read");				break;
		case E87_CIVS::en_id_wait_for_host		:	pEdit->SetWindowText(L"Wait for host");			break;
		case E87_CIVS::en_id_verification_ok		:	pEdit->SetWindowText(L"Verification OK");		break;
		case E87_CIVS::en_id_verification_failed	:	pEdit->SetWindowText(L"Verification Failed");	break;
		}
		pEdit->SetRedraw(TRUE);
		pEdit->Invalidate(FALSE);
	}
	/* Slot Verification Status */
	if (kind==3 && uvCIMLib_E87CMS_GetCarrierIDSlotMapStatus(tzCarrID, enSlotStatus))
	{
		pEdit	= (CEdit*)GetDlgItem(IDC_SIM3_EDT_SLOT_VERIFICATION);
		pEdit->SetRedraw(FALSE);
		switch (enSlotStatus)
		{
		case E87_SMVS::en_sm_not_read			:	pEdit->SetWindowText(L"Not Read");				break;
		case E87_SMVS::en_sm_wait_for_host		:	pEdit->SetWindowText(L"Wait for host");			break;
		case E87_SMVS::en_sm_verification_ok		:	pEdit->SetWindowText(L"Verification OK");		break;
		case E87_SMVS::en_sm_verification_failed	:	pEdit->SetWindowText(L"Verification Failed");	break;
		}
		pEdit->SetRedraw(TRUE);
		pEdit->Invalidate(FALSE);
	}
	/* Load Port Reservation Status */
	if (kind==4 && uvCIMLib_E87CMS_GetReservationState(u16PortID, enReserveStatus))
	{
		pEdit	= (CEdit*)GetDlgItem(IDC_SIM3_EDT_RESERVE_STATE);
		pEdit->SetRedraw(FALSE);
		switch (enReserveStatus)
		{
		case E87_LPRS::en_lp_not_reserved	:	pEdit->SetWindowText(L"Not reserved");	break;
		case E87_LPRS::en_lp_reserved		:	pEdit->SetWindowText(L"Reserved");		break;
		}
		pEdit->SetRedraw(TRUE);
		pEdit->Invalidate(FALSE);
	}
	/* Load port / carrier association state machine */
	if (kind==5 && uvCIMLib_E87CMS_GetAssociationState(u16PortID, enAssocStatus))
	{
		pEdit	= (CEdit*)GetDlgItem(IDC_SIM3_EDT_ASSOCIATE_STATE);
		pEdit->SetRedraw(FALSE);
		switch (enAssocStatus)
		{
		case E87_LPAS::en_not_associated	:	pEdit->SetWindowText(L"Not Associated");	break;
		case E87_LPAS::en_associated		:	pEdit->SetWindowText(L"Associated");		break;
		}
		pEdit->SetRedraw(TRUE);
		pEdit->Invalidate(FALSE);
	}
}