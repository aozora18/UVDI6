
/*
 desc : CIM300
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgSIM3.h"


IMPLEMENT_DYNAMIC(CDlgSIM3, CDialogEx)

/*
 desc : 생성자
 parm : None
 retn : None
*/
CDlgSIM3::CDlgSIM3(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSIM3::IDD, pParent)
{
	m_bExitProc	= FALSE;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgSIM3::~CDlgSIM3()
{
}

/* 컨트롤 연결 */
void CDlgSIM3::DoDataExchange(CDataExchange* dx)
{
	CDialogEx::DoDataExchange(dx);

	UINT32 i, u32StartID;

	u32StartID	= IDC_SIM3_LST_SUBST_ITEM;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_lst_ctl[i]);

}

/* 메시지 맵 */
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
 desc : 다이얼로그 생성 후 초기화
*/
BOOL CDlgSIM3::OnInitDialog()
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
	/* 이미 이전에 호출된적이 있음 */
	m_bExitProc	= TRUE;

}

/*
 desc : 컨트롤 초기화
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
 desc : 주기적으로 호출되는 함수
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
	/* 현재 처리하고 있는 Location 위치 정보 출력 */
	UpdateSubstrateLocation();
	/* 현재 처리하고 있는 Processing의 상태 정보 출력 */
	UpdateProcessingState();
}

/*
 desc : 이벤트 정보가 수신된 경우 (부모에 의해서 호출됨)
 parm : event	- [in]  이벤트 정보가 저장된 구조체 포인터
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
			/* Load Port의 Transfer 상태를 ReadyToUnload로 변경 */
			uvCIMLib_E87CMS_SetTransferStatus((UINT8)event->evt_data.e87_cf.port_id,
											  E87_LPTS::en_lp_ready_to_unload);
			UpdateBaseInformation(1);
			break;
		}
		break;
	}
#if 0
	/* Log Time 설정 */
	swprintf_s(tzMesg, 512, L"<%s> port_id=%d, state=%d",
			   tzKind, event->evt_data.e87_cf.port_id, event->evt_data.e87_cf.state);
	/* 현재 등록된 개수 */
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
	case E90_SPSC::en_proceed_with_substrate	:	swprintf_s(tzVerify, 256, L"%s / Verified", subst_id);	break;	/* 해당 Substrate ID가 정상적으로 읽어진게 맞으니 계속 진행 하라 */
	case E90_SPSC::en_cancel_substrate			:	swprintf_s(tzVerify, 256, L"%s / Canceled", subst_id);	break;	/* 해당 Substrate ID의 Reading 값 즉, Serial or Tag 값에 문제가 있으니, 취소 해라 */
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
	/* Process 상태 변경 */
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

	/* Substrate (Wafer) 초기 시작 위치 값*/
	if (wcslen(tzCarrID) < 1)	return;

	/* 각 Substrate가 놓일 Location의 상태 초기화 */
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

	/* 반드시 초기화 */
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

	/* 별도 버튼으로 */
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

	/* Substrate (Wafer) 초기 시작 위치 값*/
	if (wcslen(tzCarrID) < 1)	return;

	/* 각 Substrate가 놓일 Location의 상태 초기화 */
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
	/* 반드시 초기화 */
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
	/* 사용자 정의 Trigger Event 전달 */
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

		/* Substrate ID 등록 */
		swprintf_s(tzSubstID, CIM_SUBSTRATE_ID, L"%s.%02d", tzCarrID, i+1);	/* Carrier ID.SlotNumber 조합으로 ... */
		m_lst_ctl[0].InsertItem(i, tzSubstID);
		m_lst_ctl[0].SetItemText(i, 1, tzStat[u8Stat]);
	}
	m_lst_ctl[0].SetRedraw(TRUE);
	m_lst_ctl[0].Invalidate(FALSE);
	if (wcslen(tzCarrID) < 1)	return;
	/* Slot Map 검증 */
	if (!uvCIMLib_E87CMS_SetVerifySlotMap(tzCarrID, &lstSlotMap))	return;
	/* 메모리 반환 */
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
		swprintf_s(tzSubstID, CIM_SUBSTRATE_ID, L"%s.%d", tzCarrID, i+1);	/* Carrier ID.SlotNumber 조합으로 ... */
#else
		wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);
		if (!uvCIMLib_Mixed_GetSubstrateID(tzCarrID, i+1, tzSubstID, CIM_SUBSTRATE_ID))	continue;
#endif
		/* 11. Substrate ID 상태 설정 */
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
		/* 11. Substrate Processing 상태 설정 */
		if (uvCIMLib_E90STS_GetSubstrateProcessingState(tzSubstID, enSubstProc))
		{
			ptzProcName	= uvCIMLib_GetEquipProcessingName(enSubstProc);
			if (ptzProcName)	m_lst_ctl[0].SetItemText(i, 2, ptzProcName);
		}
	}
#if 0
	/* 수신된 Process Job 기준으로 Substrated ID 적재 */
	LoadSubstIDForProcessJob();
#endif
	ENG_TSSI enLocID	= ENG_TSSI::en_load_port_1;
	if (_wtoi(tzPortID) != 1)	enLocID = ENG_TSSI::en_load_port_2;
#if 0
		/* List 등록  */
		swprintf_s(tzSubstID, CIM_SUBSTRATE_ID, L"%s.%d", tzCarrID, i+1);	/* Carrier ID.SlotNumber 조합으로 ... */
		/* 실제로 Substrate와 Location ID를 CIM300에 등록 (lstSlotMap을 통해서 내부적으로 등록이 되므로, 별도 등록 작업 할필요 없음) */
		if (!uvCIMLib_E90STS_SetRegisterSubstrateIndex(tzSubstID, enLocID))	continue;
		/* 위의 2개의 함수 호출을 한번에 해결 ? */
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

	/* 현재 Slot Number가 다시 반환될 Substrate ID 얻기 */
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
	/* Carrier ID가 Closed 된 상태인지 확인 */
	if (!uvCIMLib_E87CMS_IsCarrierAccessClosed(tzCarrID))
	{
		AfxMessageBox(L"The carrier is not complete status", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
#endif
#if 0	/* PRJob Complete 호출 함수 부분에서 기입 됨 */
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
	/* 사용자 정의 Trigger Event 전달 */
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

	/* 공유 메모리에서 Carrier ID 관련 Control & Process Job ID 제거 */
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

	/* 사용자 정의 Trigger Event 전달 */
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

	/* Destination 값과 일치하는 경우도 고려 */
	if (!GetSelectPRJobID(tzPRJobID))	return -1;

	for (; i<iItem; i++)
	{
		/* 비어 있는 것 중, 현재 처리하고자 하는 Process Job ID에 속한 Slot Number인지 확인 작업 필요 */
		if (!uvCIMLib_E40PJM_IsFindSlotNoOfPRMtlNameList(tzPRJobID, i+1, bIsSlotNo))	continue;
		/* 검색 성공했는지 여부에 따라 */
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

	/* Destination 값과 일치하는 경우도 고려 */
	if (!GetSelectPRJobID(tzPRJobID))	return -1;

	for (; i<iItem; i++)
	{
		/* 비어 있는 것 중, 현재 처리하고자 하는 Process Job ID에 속한 Slot Number인지 확인 작업 필요 */
		if (!uvCIMLib_E40PJM_IsFindSlotNoOfPRMtlNameList(tzPRJobID, i+1, bIsSlotNo))	continue;
		/* 검색 성공했는지 여부에 따라 */
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

	/* Wafer Serial Info 읽은 상태 값 설정 (원래는 콜백함수로부터 응답이 와야 되지만, 여기서는 그냥 랜덤하게...) */
	uvCmn_GetRandString(0x01, szAcqSetID, 12);
	/* Host (CIM300)에게 Wafer Serial (Reading) 값 검증 요청 (여기서는 일단 요청하고) */
	uvCIMLib_E90STS_SetNotifySubstrateRead(subst_id, csCnv.Ansi2Uni(szAcqSetID), TRUE);	/* 반드시. Host에서 ProceedWith... 함수 호출해 줘야 됨 */
	/* 장비 쪽에서 독립적으로 읽었다고 설정 */
	i32Rand	= uvCmn_GetRandNumer(0x00, 1) % 5;
	uvCIMLib_E90STS_SetSubstrateIDStatus(subst_id, enIDState[i32Rand]);
	m_lst_ctl[0].SetItemText(slot_no, 3, tzIDStatus[i32Rand]);
	/* Acquired ID 얻기 */
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

	/* Process Job 상태가 Start 모드가 아니면 실행 실패 처리 */
	if (!GetSelectPRJobID(tzPRJobID))	return;
	if (!uvCIMLib_E40PJM_GetProcessJobState(tzPRJobID, enPrjJobState))	return;
	/* 모든 작업이 완료 되었다면... 리턴 */
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
	/* 현재 Processing Job의 Substrate가 모두 처리되었는지 여부 */
	if (uvCIMLib_Mixed_IsSubstrateProcessCompleted(tzPRJobID))	return;
#else
	/* 현재 Processing Job에 속한 Substrate ID의 Transport 위치가 목적지 (FOUP)에 모두 반환 되어 있는지 여부 */
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
				/* FOUP으로부터 비어 있는 Slot Number 가져오기 */
				i32SlotNum = GetNotEmptySlotNumInCarrier();	/* 현재 처리해야 할 Wafer 위치 얻기 */
				if (i32SlotNum >= 0)
				{
					wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);
					m_lst_ctl[0].GetItemText(i32SlotNum, 0, tzSubstID, CIM_SUBSTRATE_ID);
					m_lst_ctl[0].SetItemText(i32SlotNum, 1, L"empty");
					pEditRobot->SetWindowText(tzSubstID);
					/* Substrate 1개 Carrier에서 가져 왔으므로, 기존 Carrier의 위치에 있는 항목 제거*/
					uvCIMLib_E87CMS_SetCarrierPopSubstrate(tzCarrID, i32SlotNum + 1);
					/* Substrate 존재 위치 설정 : [ Robot : Yes ] */
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_occupied);
					/* Substrate 작업 위치 설정 : [ Robot ] */
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

				/* Substrate 존재 위치 설정 : [ PreAligner : Yes ] / [ Robot : No ] */
				uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_pre_aligner, E90_SSLS::en_occupied);
				uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_unoccupied);
				/* Substrate 작업 위치 설정 : [ PreAligner ] */
				uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, ENG_TSSI::en_pre_aligner, E90_SSTS::en_work, E90_SSPS::en_needs_processing);
				/* Wafer Serial 값 설정 및 Substrate ID 설정 */
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
				/* Substrate 위치 설정 : [ Robot : Yes ] / [ PreAligner : No ] */
				uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_occupied);
				uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_pre_aligner, E90_SSLS::en_unoccupied);
				/* Substrate 작업 위치 설정 : [ Robot ] */
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
				/* Substrate 위치 설정 : [ Chamber : Yes ] / [ Robot : No ] */
				uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_unoccupied);
				uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_process_chamber, E90_SSLS::en_occupied);
				/* Substrate 작업 위치 설정 : [ Chamber ] */
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
				/* FOUP으로부터 비어 있는 Slot Number 가져오기 */
				i32SlotNum = GetNotEmptySlotNumInCarrier();	/* 현재 처리해야 할 Wafer 위치 얻기 */
				if (i32SlotNum >= 0)
				{
					wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);
					m_lst_ctl[0].GetItemText(i32SlotNum, 0, tzSubstID, CIM_SUBSTRATE_ID);
					m_lst_ctl[0].SetItemText(i32SlotNum, 1, L"empty");
					pEditRobot->SetWindowText(tzSubstID);
					/* Substrate 1개 Carrier에서 가져 왔으므로, 기존 Carrier의 위치에 있는 항목 제거*/
					uvCIMLib_E87CMS_SetCarrierPopSubstrate(tzCarrID, i32SlotNum + 1);
					/* Substrate 존재 위치 설정 : [ Robot : Yes ] */
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_occupied);
					/* Substrate 작업 위치 설정 : [ Robot ] */
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
				/* Substrate 존재 위치 설정 : [ PreAligner : Yes ] / [ Robot : No ] */
				uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_pre_aligner, E90_SSLS::en_occupied);
				uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_unoccupied);
				/* Substrate 작업 위치 설정 : [ PreAligner ] */
				uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, ENG_TSSI::en_pre_aligner, E90_SSTS::en_work, E90_SSPS::en_needs_processing);
				/* Wafer Serial 값 설정 및 Substrate ID 설정  */
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
					/* Substrate 존재 위치 설정 : [ Robot : Yes ] / [ Chamber : No ] */
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_occupied);
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_process_chamber, E90_SSLS::en_unoccupied);
					/* Substrate 작업 위치 설정 : [ Robot ] */
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
				/* Substrate 존재 위치 설정 : [ Buffer : Yes ] / [ Robot : No ] */
				uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_buffer, E90_SSLS::en_occupied);
				uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_unoccupied);
				/* Substrate 작업 위치 설정 : [ Buffer ] */
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
					/* Substrate 존재 위치 설정 : [ Robot : Yes ] / [ PreAligner : No ] */
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_occupied);
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_pre_aligner, E90_SSLS::en_unoccupied);
					/* Substrate 작업 위치 설정 : [ Robot ] */
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
					/* Substrate 존재 위치 설정 : [ Chamber : Yes ] / [ Robot : No ] */
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_process_chamber, E90_SSLS::en_occupied);
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_unoccupied);
					/* Substrate 작업 위치 설정 : [ Chamber ] */
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
				/* FOUP으로부터 비어 있지 않은 Slot Number 가져오기 */
#if 0
				i32SlotNum = GetNotEmptySlotNumInCarrier();	/* 현재 처리해야 할 Wafer 위치 얻기 */
#else
				i32SlotNum = GetNeedProcessingSlotNumInCarrier();
#endif
				/* Slot Number가 음수이거나, Process Job ID에 등록된 개수보다 크면 더 이상 처리하지 않는다. */
				if (i32SlotNum >= 0)
				{
					wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);
					m_lst_ctl[0].GetItemText(i32SlotNum, 0, tzSubstID, CIM_SUBSTRATE_ID);
					m_lst_ctl[0].SetItemText(i32SlotNum, 1, L"empty");
					pEditRobot->SetWindowText(tzSubstID);
					/* Substrate 1개 Carrier에서 가져 왔으므로, 기존 Carrier의 위치에 있는 항목 제거*/
					uvCIMLib_E87CMS_SetCarrierPopSubstrate(tzCarrID, i32SlotNum + 1);
					/* Substrate 존재 위치 설정 : [ Robot : Yes ] */
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_occupied);
					/* Substrate 작업 위치 설정 : [ Robot ] */
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
					/* Substrate 존재 위치 설정 : [ PreAligner : Yes ] / [ Robot : No ] */
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_pre_aligner, E90_SSLS::en_occupied);
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_unoccupied);
					/* Substrate 존재 위치 설정 : [ PreAligner : Yes ] */
					uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, ENG_TSSI::en_pre_aligner, E90_SSTS::en_work, E90_SSPS::en_needs_processing);
					/* Wafer Serial 값 설정 및 Substrate ID 설정 */
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
					/* Substrate 존재 위치 설정 : [ Robot : Yes ] / [ Buffer : No ] */
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_occupied);
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_buffer, E90_SSLS::en_unoccupied);
					/* Substrate 존재 위치 설정 : [ Robot : Yes ] */
					uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, ENG_TSSI::en_robot, E90_SSTS::en_work, E90_SSPS::en_processed);
				}
			}
			m_u8StepSub++;
			return;

		case 0x07	: /* ROBOT -> FOUP */
			if (HasWafer(ENG_TSSI::en_robot) == TRUE)
			{
				/* FOUP으로부터 비어 있는 Slot Number 가져오기 */
				wmemset(tzSubstID, 0x00, CIM_SUBSTRATE_ID);	pEditRobot->GetWindowText(tzSubstID, CIM_SUBSTRATE_ID);
				if (0 != wcscmp(tzSubstID, L"empty"))
				{
					/* Destination과 일치하는 Slot Number 가져오기 */
					i32SlotNum = GetFindSubstrateIDtoSlotNo(0x01, tzSubstID);
					if (i32SlotNum >= 0)
					{
						m_lst_ctl[0].SetItemText(i32SlotNum, 1, L"occupied");
						uvCIMLib_E90STS_GetSubstrateProcessingState(tzSubstID, enSubstState);
						m_lst_ctl[0].SetItemText(i32SlotNum, 2, uvCIMLib_GetEquipProcessingName(enSubstState));
					}
					pEditRobot->SetWindowText(L"empty");

					/* Substrate 존재 위치 설정 : [ Robot : No ] */
					uvCIMLib_E90STS_SetSubstrateLocationStateIndex(ENG_TSSI::en_robot, E90_SSLS::en_unoccupied);
					/* Substrate 존재 위치 설정 : [ Foup : Yes ] */
					uvCIMLib_E90STS_SetChangeSubstrateStateIndex(tzSubstID, ENG_TSSI::en_robot, E90_SSTS::en_destination, E90_SSPS::en_processed);
					/* 가져온 Substrate가 반환될 위치 얻기 */
					wmemset(tzDestID, 0x00, CIM_SUBSTRATE_ID);
					uvCIMLib_Mixed_GetSubstrateDestinationID(tzCarrID, UINT8(i32SlotNum + 1), tzDestID, CIM_SUBSTRATE_ID);
					/* Substrate FOUP 위치 지정 : FOUP (Carrier)의 특정 위치에 집어 넣기 */
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
		/* Destination SubstrateID 적재 */
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

	/* 아래 순서 바꾸면 안됨 */
	if (!uvCIMLib_E40PJM_SetPRJobSetStartMethod(&arrPRJobs, TRUE))	return;
	if (!uvCIMLib_E40PJM_SetProcessJobQueueToSetup(tzPRJobID))		return;
	if (!uvCIMLib_E40PJM_SetPRJobStartProcess(tzPRJobID))			return;
//	if (!uvCIMLib_E40PJM_SetProcessJobState(tzPRJobID, E40_PPJS::en_processing))	return;	/* 강제로 설정해 주면 안됨 */
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

	/* 아래 순서 바꾸면 안됨 */
	uvCIMLib_E40PJM_SetPRJobProcessComplete(tzPRJobID);
//	uvCIMLib_E40PJM_SetProcessJobState(tzPrJobID, E40_PPJS::en_process_completed);	/* 강제로 설정해 주면 안됨 */
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
	/* Control Job 쪽에도 알려줘야 됨  */
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
 desc : Source SubstrateID에 대한 Destination SubstrateID 얻기
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
 desc : 현재 SubstrateID와 일치되는 Index (Slot Number) 값 반환
 parm : type		- [in]  0x00: Source SubstrateID 검색, 0x01: Destination SubstrateID 검색
		subst_id	- [in]  Source Soubstrate ID 값
 retn : 0 based Index, 실패일 경우 -1
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

	/* 알람 Clear ? Which ? */

	/* Load Port 상태 변경 : Loading ... */

	/* Transfer Status 갱신 */
	if (uvCIMLib_E87CMS_GetTransferStatus((UINT16)_wtoi(tzPortID), enTransState))
	{
	}

	/* ID Verification */
	/* Slot Map Verification */
	/* Association Status */
	/* Reservation Status */
	/* SIM300 UI에 있는 각종 상태 갱신 */
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
 parm : kind	- [in]  Update 종류
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