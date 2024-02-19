
/*
 desc : E87CMS
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgE87.h"


IMPLEMENT_DYNAMIC(CDlgE87, CDialogEx)

/*
 desc : 생성자
 parm : None
 retn : None
*/
CDlgE87::CDlgE87(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgE87::IDD, pParent)
{
	m_bExitProc	= FALSE;
	m_u64TickRecipeState= 0;
	m_bRecipeState		= FALSE;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgE87::~CDlgE87()
{
}

/* 컨트롤 연결 */
void CDlgE87::DoDataExchange(CDataExchange* dx)
{
	CDialogEx::DoDataExchange(dx);

	UINT32 i = 0, u32StartID = 0;

//	u32StartID	= IDC_SIM3_RCH_EVENT;
//	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_rch_ctl[i]);

}

/* 메시지 맵 */
BEGIN_MESSAGE_MAP(CDlgE87, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_E87_BTN_REMOTE_CTRL_ENABLE, &CDlgE87::OnBnClickedE87BtnRemoteCtrlEnable)
	ON_BN_CLICKED(IDC_E87_BTN_REMOTE_CTRL_DISABLE, &CDlgE87::OnBnClickedE87BtnRemoteCtrlDisable)
	ON_BN_CLICKED(IDC_E87_BTN_LOAD_PORT_NO, &CDlgE87::OnBnClickedE87BtnLoadPortNo)
	ON_BN_CLICKED(IDC_E87_BTN_LOAD_PORT_CARR, &CDlgE87::OnBnClickedE87BtnLoadPortCarr)
	ON_BN_CLICKED(IDC_E87_BTN_TRANSACTION_ID, &CDlgE87::OnBnClickedE87BtnTransactionId)
	ON_BN_CLICKED(IDC_E87_BTN_TRANSACTION_REPLY, &CDlgE87::OnBnClickedE87BtnTransactionReply)
	ON_BN_CLICKED(IDC_E87_BTN_CARR_OUT_BEGIN, &CDlgE87::OnBnClickedE87BtnCarrOutBegin)
	ON_BN_CLICKED(IDC_E87_BTN_CARR_OUT, &CDlgE87::OnBnClickedE87BtnCarrOut)
	ON_BN_CLICKED(IDC_E87_BTN_CARR_IN, &CDlgE87::OnBnClickedE87BtnCarrIn)
	ON_BN_CLICKED(IDC_E87_BTN_CARR_OUT_ALL, &CDlgE87::OnBnClickedE87BtnCarrOutAll)
	ON_BN_CLICKED(IDC_E87_BTN_CARR_ID_VERIFY_GET, &CDlgE87::OnBnClickedE87BtnCarrIdVerifyGet)
	ON_BN_CLICKED(IDC_E87_BTN_CARR_ID_VERIFY_PUT, &CDlgE87::OnBnClickedE87BtnCarrIdVerifyPut)
	ON_BN_CLICKED(IDC_E87_BTN_CARR_ID_ACCESS_GET, &CDlgE87::OnBnClickedE87BtnCarrIdAccessGet)
	ON_BN_CLICKED(IDC_E87_BTN_CARR_ID_ACCESS_PUT, &CDlgE87::OnBnClickedE87BtnCarrIdAccessPut)
	ON_BN_CLICKED(IDC_E87_BTN_LOAD_PORT_STATE_GET, &CDlgE87::OnBnClickedE87BtnLoadPortStateGet)
	ON_BN_CLICKED(IDC_E87_BTN_LOAD_PORT_STATE_SET, &CDlgE87::OnBnClickedE87BtnLoadPortStateSet)
	ON_BN_CLICKED(IDC_E87_BTN_CARR_ID_NOTI, &CDlgE87::OnBnClickedE87BtnCarrIdNoti)
	ON_BN_CLICKED(IDC_E87_BTN_CARR_ID_ACCESS_ST_GET, &CDlgE87::OnBnClickedE87BtnCarrIdAccessStGet)
	ON_BN_CLICKED(IDC_E87_BTN_CARR_ID_ACCESS_ST_SET, &CDlgE87::OnBnClickedE87BtnCarrIdAccessStSet)
	ON_BN_CLICKED(IDC_E87_BTN_RECIPE_NEW, &CDlgE87::OnBnClickedE87BtnRecipeNew)
	ON_BN_CLICKED(IDC_E87_BTN_RECIPE_EDIT, &CDlgE87::OnBnClickedE87BtnRecipeEdit)
	ON_BN_CLICKED(IDC_E87_BTN_RECIPE_DEL, &CDlgE87::OnBnClickedE87BtnRecipeDel)
	ON_BN_CLICKED(IDC_E87_BTN_RECIPE_OK, &CDlgE87::OnBnClickedE87BtnRecipeOk)
	ON_BN_CLICKED(IDC_E87_BTN_RECIPE_RUN, &CDlgE87::OnBnClickedE87BtnRecipeRun)
END_MESSAGE_MAP()

/*
 desc : 다이얼로그 생성 후 초기화
*/
BOOL CDlgE87::OnInitDialog()
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
VOID CDlgE87::OnClose()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnClose();
}
VOID CDlgE87::OnDestroy()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnDestroy();
}
VOID CDlgE87::ExitProc()
{
	/* 이미 이전에 호출된적이 있음 */
	m_bExitProc	= TRUE;

}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgE87::InitCtrl()
{
	((CEdit*)GetDlgItem(IDC_E87_EDT_LOAD_PORT_NO))->SetWindowText(L"1");
	((CEdit*)GetDlgItem(IDC_E87_EDT_HOST_CARR_TAG))->SetWindowText(L"123456789abcdef");
	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_OUT_PORT))->SetWindowText(L"1");
	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_OUT_CARR_ID))->SetWindowText(L"CARRIER1");
	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_ID_NOTI))->SetWindowText(L"CARRIER1");
	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_ID_ACCESS))->SetWindowText(L"CARRIER1");

	((CEdit*)GetDlgItem(IDC_E87_EDT_PORT_ID_ACCESS))->SetWindowText(L"1");
	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_ID_VERIFY))->SetWindowText(L"CARRIER1");
}

/*
 desc : 주기적으로 호출되는 함수
 parm : None
 retn : None
*/
VOID CDlgE87::UpdateCtrl()
{
	BOOL bEnable		= TRUE;
	INT32 i	= 0;
	TCHAR tzMesg[512]	= {NULL}, tzCarrID[CIM_CARRIER_ID_SIZE]	= {NULL};
	CEdit *pEdit		= {NULL};

	/* Allow Remote Control */
	pEdit	= (CEdit*)GetDlgItem(IDC_E87_EDT_REMOTE_CONTROL);
	pEdit->SetRedraw(FALSE);
	if (uvCIMLib_E87CMS_GetAllowRemoteControl(bEnable))
	{
		if (bEnable)	pEdit->SetWindowText(L"Enabled");
		else			pEdit->SetWindowText(L"Disabled");
	}
	pEdit->SetRedraw(TRUE);	pEdit->Invalidate(FALSE);

	/* Recipe State 갱신 (ProcessState) */
	UpdateRecipeState();
}

/*
 desc : 이벤트 정보가 수신된 경우 (부모에 의해서 호출됨)
 parm : event	- [in]  이벤트 정보가 저장된 구조체 포인터
 retn : None
*/
VOID CDlgE87::UpdateCtrl(STG_CCED *event)
{
	CEdit *pEdit= NULL;

	switch (event->exx_id)
	{
	case ENG_ESSC::en_e87_cms	:
		switch (event->cbf_id)
		{
		case E87_CEFI::en_change_access_mode			:	UpdatePortAccessMode(&event->evt_data.e87_cf);	break;
		case E87_CEFI::en_change_service_status		:
		case E87_CEFI::en_in_service_transfer_state	:	UpdateLoadPortTransferStatus(&event->evt_data.e87_cf);	break;
		case E87_CEFI::en_cancel_all_carrier_out		:	UpdateCarrierAllOut();	break;

		case E87_CEFI::en_reserve_at_port			:
		case E87_CEFI::en_carrier_noti				:
		case E87_CEFI::en_cancel_reserve_at_port		:
		case E87_CEFI::en_cancel_carrier_noti		:	UpdateCarrierService(event->cbf_id);	break;
		default			: ;	break;
		}
		break;
	}
}

VOID CDlgE87::UpdateCarrierService(UINT16 cbf_id)
{
	switch (cbf_id)
	{
	case E87_CEFI::en_cancel_carrier_noti	:	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_SERVICE))->SetWindowText(L"Cancel Carrier Notification");	break;
	case E87_CEFI::en_carrier_noti			:	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_SERVICE))->SetWindowText(L"Carrier Notification");	break;
	case E87_CEFI::en_reserve_at_port		:	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_SERVICE))->SetWindowText(L"Reserve At Port");	break;
	case E87_CEFI::en_cancel_reserve_at_port	:	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_SERVICE))->SetWindowText(L"Cancel Reservation At Port");	break;
	}
}

VOID CDlgE87::UpdatePortAccessMode(LPG_E87_CF data)
{
	TCHAR tzPortID[32]	= {NULL};

	((CEdit*)GetDlgItem(IDC_E87_EDT_LOAD_PORT_NO))->GetWindowText(tzPortID, 32);

	((CButton*)GetDlgItem(IDC_E87_RDO_CARR_ID_AUTO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_E87_RDO_CARR_ID_MANUAL))->SetCheck(0);
	switch (data->state)
	{
	case 1	:	uvCIMLib_E87CMS_SetAccessModeState(UINT16(_wtoi(tzPortID)), E87_LPAM::en_auto);
				((CButton*)GetDlgItem(IDC_E87_RDO_CARR_ID_AUTO))->SetCheck(1);	break;
	case 0	:	uvCIMLib_E87CMS_SetAccessModeState(UINT16(_wtoi(tzPortID)), E87_LPAM::en_manual);
				((CButton*)GetDlgItem(IDC_E87_RDO_CARR_ID_MANUAL))->SetCheck(1);break;
	}
}

VOID CDlgE87::UpdateLoadPortTransferStatus(LPG_E87_CF data)
{
	TCHAR tzPortID[32]	= {NULL};

	((CEdit*)GetDlgItem(IDC_E87_EDT_LOAD_PORT_NO))->GetWindowText(tzPortID, 32);

	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_OUT_SVC))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_TRANS_BL))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_READY_L))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_READY_U))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_IN_SVC))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_TRANS_R))->SetCheck(0);
	switch (E87_LPTS(data->state))
	{
	case E87_LPTS::en_lp_out_of_service		:	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_OUT_SVC))->SetCheck(1);	break;
	case E87_LPTS::en_lp_transfer_blocked	:	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_TRANS_BL))->SetCheck(1);break;
	case E87_LPTS::en_lp_ready_to_load		:	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_READY_L))->SetCheck(1);	break;
	case E87_LPTS::en_lp_ready_to_unload	:	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_READY_U))->SetCheck(1);	break;
	case E87_LPTS::en_lp_in_service			:	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_IN_SVC))->SetCheck(1);	break;
	case E87_LPTS::en_lp_transfer_ready		:	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_TRANS_R))->SetCheck(1);	break;
	}
}

VOID CDlgE87::UpdateCarrierAllOut()
{
	E87_LPTS enStatus1, enStatus2;
	
	/* 현재 Carrier Status 값 읽어서 갱신 */
	uvCIMLib_E87CMS_GetTransferStatus(1, enStatus1);
	switch (enStatus1)
	{
	case E87_LPTS::en_lp_out_of_service		:	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_ALL_CANCEL1))->SetWindowText(L"Out of Service");	break;
	case E87_LPTS::en_lp_transfer_blocked	:	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_ALL_CANCEL1))->SetWindowText(L"Transfer Blocked");	break;
	case E87_LPTS::en_lp_ready_to_load		:	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_ALL_CANCEL1))->SetWindowText(L"Ready To Load");	break;
	case E87_LPTS::en_lp_ready_to_unload	:	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_ALL_CANCEL1))->SetWindowText(L"Ready To Unload");	break;
	case E87_LPTS::en_lp_in_service			:	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_ALL_CANCEL1))->SetWindowText(L"In Serivce");		break;
	case E87_LPTS::en_lp_transfer_ready		:	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_ALL_CANCEL1))->SetWindowText(L"Transfer Ready");	break;
	}

	uvCIMLib_E87CMS_GetTransferStatus(2, enStatus2);
	switch (enStatus2)
	{
	case E87_LPTS::en_lp_out_of_service		:	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_ALL_CANCEL2))->SetWindowText(L"Out of Service");	break;
	case E87_LPTS::en_lp_transfer_blocked	:	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_ALL_CANCEL2))->SetWindowText(L"Transfer Blocked");	break;
	case E87_LPTS::en_lp_ready_to_load		:	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_ALL_CANCEL2))->SetWindowText(L"Ready To Load");	break;
	case E87_LPTS::en_lp_ready_to_unload	:	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_ALL_CANCEL2))->SetWindowText(L"Ready To Unload");	break;
	case E87_LPTS::en_lp_in_service			:	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_ALL_CANCEL2))->SetWindowText(L"In Serivce");		break;
	case E87_LPTS::en_lp_transfer_ready		:	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_ALL_CANCEL2))->SetWindowText(L"Transfer Ready");	break;
	}
}

void CDlgE87::OnBnClickedE87BtnRemoteCtrlEnable()
{
	uvCIMLib_E87CMS_SetAllowRemoteControl(TRUE);
}

void CDlgE87::OnBnClickedE87BtnRemoteCtrlDisable()
{
	uvCIMLib_E87CMS_SetAllowRemoteControl(FALSE);
}

void CDlgE87::OnBnClickedE87BtnLoadPortNo()
{
	TCHAR tzPortID[32]	= {NULL}, tzCarrID[CIM_CARRIER_ID_SIZE]	= {NULL};
	UINT16 u16PortNo	= 0;

	((CEdit*)GetDlgItem(IDC_E87_EDT_LOAD_PORT_NO))->GetWindowText(tzPortID, 32);
	uvCIMLib_E87CMS_GetAssociationCarrierID((UINT16)_wtoi(tzPortID), tzCarrID, CIM_CARRIER_ID_SIZE);
	((CEdit*)GetDlgItem(IDC_E87_EDT_PORT_TO_CARR_ID))->SetWindowText(tzCarrID);
}

void CDlgE87::OnBnClickedE87BtnLoadPortCarr()
{
	TCHAR tzPortID[32]	= {NULL}, tzCarrID[CIM_CARRIER_ID_SIZE]	= {NULL};
	UINT16 u16PortNo	= 0;

	((CEdit*)GetDlgItem(IDC_E87_EDT_PORT_TO_CARR_ID))->GetWindowText(tzCarrID, CIM_CARRIER_ID_SIZE);
	uvCIMLib_E87CMS_GetAssociationPortID(tzCarrID, u16PortNo);
	swprintf_s(tzPortID, 32, L"%d", u16PortNo);
	((CEdit*)GetDlgItem(IDC_E87_EDT_LOAD_PORT_NO))->SetWindowText(tzPortID);
}

void CDlgE87::OnBnClickedE87BtnTransactionId()
{
	TCHAR tzTransID[32]	= {NULL};
	LPG_CHCT pstInfo	= uvCIMLib_E87CMS_GetTransactionInfo();
	if (!pstInfo)	return;

	swprintf_s(tzTransID, 32, L"%d", pstInfo->transaction_id);
	((CEdit*)GetDlgItem(IDC_E87_EDT_HOST_TRANS_ID))->SetWindowText(tzTransID);
	((CEdit*)GetDlgItem(IDC_E87_EDT_HOST_LOC_ID))->SetWindowText(pstInfo->loc_id);
	((CEdit*)GetDlgItem(IDC_E87_EDT_HOST_CARR_ID))->SetWindowText(pstInfo->carr_id);
	((CEdit*)GetDlgItem(IDC_E87_EDT_HOST_SEG_DATA))->SetWindowText(pstInfo->data_seg);

}

void CDlgE87::OnBnClickedE87BtnTransactionReply()
{
	TCHAR tzTransID[32]	= {NULL};
	TCHAR tzCarrID[CIM_CARRIER_ID_SIZE]	= {NULL};
	TCHAR tzTagID[CIM_CARRIER_TAG_SIZE]	= {NULL};
	LPG_CHCT pstInfo	= uvCIMLib_E87CMS_GetTransactionInfo();
	if (!pstInfo)	return;

	swprintf_s(tzTransID, 32, L"%d", pstInfo->transaction_id);
	swprintf_s(tzCarrID, CIM_CARRIER_ID_SIZE, L"%s", pstInfo->carr_id);

	((CEdit*)GetDlgItem(IDC_E87_EDT_HOST_CARR_TAG))->GetWindowText(tzTagID, CIM_CARRIER_TAG_SIZE);
	uvCIMLib_E87CMS_SetAsyncCarrierTagReadComplete(tzCarrID, tzTagID, _wtoi(tzTransID));
}

void CDlgE87::OnBnClickedE87BtnCarrOutBegin()
{
	TCHAR tzPortID[32]	= {NULL}, tzCarrID[CIM_CARRIER_ID_SIZE] = {NULL};
	E87_LPAS enPortCarrierState;
	PTCHAR ptzUndockName= NULL, ptzDockName;
	E87_LPTS enTransferStatus;

	/* Carrier 가 해당 Load Port에 적재(설치) 가능한지 여부 */
	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_OUT_CARR_ID))->GetWindowText(tzCarrID, CIM_CARRIER_ID_SIZE);
	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_OUT_PORT))->GetWindowText(tzPortID, 32);
	if (!uvCIMLib_E87CMS_GetAssociationState(_wtoi(tzPortID), enPortCarrierState))	return;

	ptzDockName		= uvCIMLib_GetLoadPortDockedName((UINT16)_wtoi(tzPortID), 0x00);
	ptzUndockName	= uvCIMLib_GetLoadPortDockedName((UINT16)_wtoi(tzPortID), 0x01);

	if (E87_LPAS::en_not_associated == enPortCarrierState)
	{
		AfxMessageBox(L"Carrier를 Load Port에 놓을 수 있음", MB_OK);
	}
	else
	{
		AfxMessageBox(L"Carrier를 Load Port에 높을 수 없음", MB_ICONSTOP|MB_TOPMOST);
	}

	if (!uvCIMLib_E87CMS_SetTransferStart((UINT16)_wtoi(tzPortID)))						return;
	if (!uvCIMLib_E87CMS_SetCarrierPlaced((UINT16)_wtoi(tzPortID)))						return;
	if (!uvCIMLib_E87CMS_SetCarrierAtPort(tzCarrID, _wtoi(tzPortID), ptzUndockName))	return;
	if (!uvCIMLib_E87CMS_SetAssociationPortID(tzCarrID, _wtoi(tzPortID)))				return;
	if (!uvCIMLib_E87CMS_GetTransferStatus(_wtoi(tzPortID), enTransferStatus))			return;
	if (enTransferStatus != E87_LPTS::en_lp_ready_to_load)
	{
		if (!uvCIMLib_E87CMS_SetTransferStatus(_wtoi(tzPortID), E87_LPTS::en_lp_ready_to_load))	return;
	}

	/* 아래 함수 한개를 돌리기 위해서 위의 라이브러리 함수 7개를 호출해야 한다? */
	/* 무조건이 아니라... 아래 함수 호출 조건이 되려면... 위의 함수를 호출해줘야 됨. 전부 아니어도 됨. 굳이 조건이 되어 있다면 */
	if (!uvCIMLib_E87CMS_SetBeginCarrierOut(tzCarrID, (UINT16)_wtoi(tzPortID)))			return;
}

void CDlgE87::OnBnClickedE87BtnCarrOut()
{
	TCHAR tzPortID[32]	= {NULL}, tzCarrID[CIM_CARRIER_ID_SIZE] = {NULL};
	E87_LPTS enTransferStatus;

	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_OUT_CARR_ID))->GetWindowText(tzCarrID, CIM_CARRIER_ID_SIZE);
	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_OUT_PORT))->GetWindowText(tzPortID, 32);

	if (!uvCIMLib_E87CMS_SetCarrierOut(tzCarrID, _wtoi(tzPortID)))	return;

	if (!uvCIMLib_E87CMS_GetTransferStatus(_wtoi(tzPortID), enTransferStatus))	return;
	if (enTransferStatus != E87_LPTS::en_lp_ready_to_unload)
	{
		if (!uvCIMLib_E87CMS_SetTransferStatus(_wtoi(tzPortID), E87_LPTS::en_lp_ready_to_unload))	return;
	}
}

void CDlgE87::OnBnClickedE87BtnCarrIn()
{
	TCHAR tzPortID[32]	= {NULL}, tzCarrID[CIM_CARRIER_ID_SIZE] = {NULL};
	E87_LPTS enTransferStatus;

	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_OUT_CARR_ID))->GetWindowText(tzCarrID, CIM_CARRIER_ID_SIZE);
	if (!uvCIMLib_E87CMS_GetTransferStatus(_wtoi(tzPortID), enTransferStatus))	return;
	if (enTransferStatus != E87_LPTS::en_lp_ready_to_unload)
	{
		if (!uvCIMLib_E87CMS_SetTransferStatus(_wtoi(tzPortID), E87_LPTS::en_lp_ready_to_unload))	return;
	}
	/* 다시 원래 위치로 복귀  */
	uvCIMLib_E87CMS_SetCarrierIn(tzCarrID);
}


void CDlgE87::OnBnClickedE87BtnCarrOutAll()
{
	/* 기존에 Carrier Out Service에 등록된 Queue 값을 비우도록 함 */
	uvCIMLib_E87CMS_SetCancelAllCarrierOut();
}


void CDlgE87::OnBnClickedE87BtnCarrIdVerifyGet()
{
	TCHAR tzCarrID[CIM_CARRIER_ID_SIZE] = {NULL};
	E87_CIVS enStatus;

	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_ID_VERIFY))->GetWindowText(tzCarrID, CIM_CARRIER_ID_SIZE);

	if (uvCIMLib_E87CMS_GetCarrierIDStatus(tzCarrID, enStatus))
	{
		((CButton*)GetDlgItem(IDC_E87_RDO_CARR_ID_NOT_READ))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_E87_RDO_CARR_ID_WAIT_HOST))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_E87_RDO_CARR_ID_VERIFY_OK))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_E87_RDO_CARR_ID_VERIFY_FAIL))->SetCheck(0);
		switch (enStatus)
		{
		case E87_CIVS::en_id_not_read			:	((CButton*)GetDlgItem(IDC_E87_RDO_CARR_ID_NOT_READ))->SetCheck(1);;		break;	
		case E87_CIVS::en_id_wait_for_host		:	((CButton*)GetDlgItem(IDC_E87_RDO_CARR_ID_WAIT_HOST))->SetCheck(1);		break;
		case E87_CIVS::en_id_verification_ok	:	((CButton*)GetDlgItem(IDC_E87_RDO_CARR_ID_VERIFY_OK))->SetCheck(1);		break;
		case E87_CIVS::en_id_verification_failed:	((CButton*)GetDlgItem(IDC_E87_RDO_CARR_ID_VERIFY_FAIL))->SetCheck(1);	break;
		}
	}
}
void CDlgE87::OnBnClickedE87BtnCarrIdVerifyPut()
{
	TCHAR tzCarrID[CIM_CARRIER_ID_SIZE] = {NULL};
	E87_CIVS enStatus;
	
	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_ID_VERIFY))->GetWindowText(tzCarrID, CIM_CARRIER_ID_SIZE);

	if (((CButton*)GetDlgItem(IDC_E87_RDO_CARR_ID_NOT_READ))->GetCheck())			enStatus	= E87_CIVS::en_id_not_read;
	else if (((CButton*)GetDlgItem(IDC_E87_RDO_CARR_ID_WAIT_HOST))->GetCheck())		enStatus	= E87_CIVS::en_id_wait_for_host;
	else if (((CButton*)GetDlgItem(IDC_E87_RDO_CARR_ID_VERIFY_OK))->GetCheck())		enStatus	= E87_CIVS::en_id_verification_ok;
	else if (((CButton*)GetDlgItem(IDC_E87_RDO_CARR_ID_VERIFY_FAIL))->GetCheck())	enStatus	= E87_CIVS::en_id_verification_failed;

	if (uvCIMLib_E87CMS_SetCarrierIDStatus(tzCarrID, enStatus))	OnBnClickedE87BtnCarrIdVerifyGet();
}

void CDlgE87::OnBnClickedE87BtnCarrIdAccessGet()
{
	TCHAR tzPortID[32] = {NULL};
	E87_LPAM enStatus;
	
	((CButton*)GetDlgItem(IDC_E87_RDO_CARR_ID_AUTO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_E87_RDO_CARR_ID_MANUAL))->SetCheck(0);
	((CEdit*)GetDlgItem(IDC_E87_EDT_PORT_ID_ACCESS))->GetWindowText(tzPortID, 32);
	if (uvCIMLib_E87CMS_GetAccessModeState(UINT16(_wtoi(tzPortID)), enStatus))
	{
		switch (enStatus)
		{
		case E87_LPAM::en_auto	: ((CButton*)GetDlgItem(IDC_E87_RDO_CARR_ID_AUTO))->SetCheck(1);	break;
		case E87_LPAM::en_manual	: ((CButton*)GetDlgItem(IDC_E87_RDO_CARR_ID_MANUAL))->SetCheck(1);	break;
		}
	}
}
void CDlgE87::OnBnClickedE87BtnCarrIdAccessPut()
{
	TCHAR tzPortID[32] = {NULL};
	E87_LPAM enStatus;
	
	if (((CButton*)GetDlgItem(IDC_E87_RDO_CARR_ID_AUTO))->GetCheck())		enStatus = E87_LPAM::en_auto;
	else if (((CButton*)GetDlgItem(IDC_E87_RDO_CARR_ID_MANUAL))->GetCheck())enStatus = E87_LPAM::en_manual;
	((CEdit*)GetDlgItem(IDC_E87_EDT_PORT_ID_ACCESS))->GetWindowText(tzPortID, 32);
	uvCIMLib_E87CMS_SetAccessModeState(UINT16(_wtoi(tzPortID)), enStatus);
}


void CDlgE87::OnBnClickedE87BtnLoadPortStateGet()
{
	TCHAR tzPortID[32] = {NULL};
	E87_LPTS enStatus;
	((CEdit*)GetDlgItem(IDC_E87_EDT_PORT_ID_ACCESS))->GetWindowText(tzPortID, 32);

	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_OUT_SVC))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_TRANS_BL))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_READY_L))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_READY_U))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_IN_SVC))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_TRANS_R))->SetCheck(0);

	uvCIMLib_E87CMS_GetTransferStatus(UINT16(_wtoi(tzPortID)), enStatus);

	switch (enStatus)
	{
	case E87_LPTS::en_lp_out_of_service	:	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_OUT_SVC))->SetCheck(1);	break;
	case E87_LPTS::en_lp_transfer_blocked:	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_TRANS_BL))->SetCheck(1);break;
	case E87_LPTS::en_lp_ready_to_load	:	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_READY_L))->SetCheck(1);	break;
	case E87_LPTS::en_lp_ready_to_unload	:	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_READY_U))->SetCheck(1);	break;
	case E87_LPTS::en_lp_in_service		:	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_IN_SVC))->SetCheck(1);	break;
	case E87_LPTS::en_lp_transfer_ready	:	((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_TRANS_R))->SetCheck(1);	break;
	}
}

void CDlgE87::OnBnClickedE87BtnLoadPortStateSet()
{
	TCHAR tzPortID[32] = {NULL};
	E87_LPTS enStatus;
	((CEdit*)GetDlgItem(IDC_E87_EDT_PORT_ID_ACCESS))->GetWindowText(tzPortID, 32);

	if (((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_OUT_SVC))->GetCheck())		enStatus = E87_LPTS::en_lp_out_of_service;
	else if (((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_TRANS_BL))->GetCheck())enStatus = E87_LPTS::en_lp_transfer_blocked;
	else if (((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_READY_L))->GetCheck())	enStatus = E87_LPTS::en_lp_ready_to_load;
	else if (((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_READY_U))->GetCheck())	enStatus = E87_LPTS::en_lp_ready_to_unload;
	else if (((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_IN_SVC))->GetCheck())	enStatus = E87_LPTS::en_lp_in_service;
	else if (((CButton*)GetDlgItem(IDC_E87_RDO_LOAD_PORT_TRANS_R))->GetCheck())	enStatus = E87_LPTS::en_lp_transfer_ready;
	uvCIMLib_E87CMS_SetTransferStatus(UINT16(_wtoi(tzPortID)), enStatus);
}


void CDlgE87::OnBnClickedE87BtnCarrIdNoti()
{
	TCHAR tzCarrID[CIM_CARRIER_ID_SIZE]	= {NULL};
	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_ID_NOTI))->GetWindowText(tzCarrID, CIM_CARRIER_ID_SIZE);
	if (wcslen(tzCarrID) < 1)	return;
	uvCIMLib_E87CMS_SetCancelCarrierNotification(tzCarrID);
}


void CDlgE87::OnBnClickedE87BtnCarrIdAccessStGet()
{
	TCHAR tzCarrID[CIM_CARRIER_ID_SIZE]	= {NULL};
	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_ID_ACCESS))->GetWindowText(tzCarrID, CIM_CARRIER_ID_SIZE);
	if (wcslen(tzCarrID) < 1)	return;
	E87_CCAS enStatus;

	if (!uvCIMLib_E87CMS_GetCarrierAccessingStatus(tzCarrID, enStatus))	return;
	((CComboBox*)GetDlgItem(IDC_E87_CMB_CARRIER_ACCESS))->SetCurSel(INT32(enStatus));
}

void CDlgE87::OnBnClickedE87BtnCarrIdAccessStSet()
{
	TCHAR tzCarrID[CIM_CARRIER_ID_SIZE]	= {NULL};
	INT32 i32CurSel	= -1;
	E87_CCAS enStatus;
	((CEdit*)GetDlgItem(IDC_E87_EDT_CARR_ID_ACCESS))->GetWindowText(tzCarrID, CIM_CARRIER_ID_SIZE);
	if (wcslen(tzCarrID) < 1)	return;

	i32CurSel	= ((CComboBox*)GetDlgItem(IDC_E87_CMB_CARRIER_ACCESS))->GetCurSel();
	if (i32CurSel < 0)	return;

	enStatus	= E87_CCAS(i32CurSel);
	uvCIMLib_E87CMS_SetCarrierAccessingStatus(tzCarrID, enStatus);
}

void CDlgE87::OnBnClickedE87BtnRecipeNew()
{
	TCHAR tzRecipe[RECIPE_NAME_LENGTH]	= {NULL};
	CEdit *pEdit	= (CEdit*)GetDlgItem(IDC_E87_EDT_RECIPE_NAME);
	pEdit->GetWindowText(tzRecipe, RECIPE_NAME_LENGTH);
	if (wcslen(tzRecipe) < 1)	return;

	uvCIMLib_E87CMS_SetRecipeManagement(EQUIP_CONN_ID, tzRecipe, 0x01);
}

void CDlgE87::OnBnClickedE87BtnRecipeEdit()
{
	TCHAR tzRecipe[RECIPE_NAME_LENGTH]	= {NULL};
	CEdit *pEdit	= (CEdit*)GetDlgItem(IDC_E87_EDT_RECIPE_NAME);
	pEdit->GetWindowText(tzRecipe, RECIPE_NAME_LENGTH);
	if (wcslen(tzRecipe) < 1)	return;

	uvCIMLib_E87CMS_SetRecipeManagement(EQUIP_CONN_ID, tzRecipe, 0x02);
}

void CDlgE87::OnBnClickedE87BtnRecipeDel()
{
	TCHAR tzRecipe[RECIPE_NAME_LENGTH]	= {NULL};
	CEdit *pEdit	= (CEdit*)GetDlgItem(IDC_E87_EDT_RECIPE_NAME);
	pEdit->GetWindowText(tzRecipe, RECIPE_NAME_LENGTH);
	if (wcslen(tzRecipe) < 1)	return;

	uvCIMLib_E87CMS_SetRecipeManagement(EQUIP_CONN_ID, tzRecipe, 0x03);
}

void CDlgE87::OnBnClickedE87BtnRecipeOk()
{
	TCHAR tzRecipe[RECIPE_NAME_LENGTH]	= {NULL};
	CEdit *pEdit	= (CEdit*)GetDlgItem(IDC_E87_EDT_RECIPE_NAME);
	pEdit->GetWindowText(tzRecipe, RECIPE_NAME_LENGTH);
	if (wcslen(tzRecipe) < 1)	return;

	uvCIMLib_E87CMS_SetRecipeSelected(EQUIP_CONN_ID, tzRecipe);
}


void CDlgE87::OnBnClickedE87BtnRecipeRun()
{
	CComboBox *pCmb	= (CComboBox*)GetDlgItem(IDC_E87_RECIPE_STATE);

	if (pCmb->GetCurSel() < 0)	return;

	E30_GPSV enState	= (E30_GPSV)pCmb->GetCurSel();
	m_u64TickRecipeState= GetTickCount64();
	m_bRecipeState		= TRUE;
	uvCIMLib_SetUpdateProcessState(enState);
}

VOID CDlgE87::UpdateRecipeState()
{
	UINT64 u64Tick	= GetTickCount64();
	UNG_CVVT unValue;
	CEdit *pEdt	= (CEdit*)GetDlgItem(IDC_E87_EDT_RECIPE_STATE);

	if (m_bRecipeState && ((m_u64TickRecipeState + 3000) < u64Tick))
	{
		uvCIMLib_SetUpdateProcessState(E30_GPSV::en_idle);
		m_bRecipeState	= FALSE;
	}

	/* 현재 Recipe State 갱신 (Executing or Idling) */
	if (!uvCIMLib_GetNameValue(EQUIP_CONN_ID, L"ProcessState", unValue))	return;
	pEdt->SetWindowText(unValue.s_value);
}