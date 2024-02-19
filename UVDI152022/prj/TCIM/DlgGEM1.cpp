
/*
 desc : Control
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgGEM1.h"

#include "DlgData.h"

IMPLEMENT_DYNAMIC(CDlgGEM1, CDialogEx)

/*
 desc : 생성자
 parm : None
 retn : None
*/
CDlgGEM1::CDlgGEM1(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgGEM1::IDD, pParent)
{
	m_bExitProc	= FALSE;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgGEM1::~CDlgGEM1()
{
}

/* 컨트롤 연결 */
void CDlgGEM1::DoDataExchange(CDataExchange* dx)
{
	CDialogEx::DoDataExchange(dx);
	DDX_Control(dx, IDC_GEM1_LST_VAR_ITEMS, m_lst_var);
	DDX_Control(dx, IDC_GEM1_LST_VAR_COMMANDS, m_lst_cmd);
	DDX_Control(dx, IDC_GEM1_LST_VAR_REPORT, m_lst_rpt);
	DDX_Control(dx, IDC_GEM1_LST_VAR_RPTEVT, m_lst_ret);
	DDX_Control(dx, IDC_GEM1_LST_PROJECTS, m_lst_prj);
	DDX_Control(dx, IDC_GEM1_LST_VAR_TRACE, m_lst_trc);
	DDX_Control(dx, IDC_GEM1_LST_ALARMS, m_lst_arm);
}

/* 메시지 맵 */
BEGIN_MESSAGE_MAP(CDlgGEM1, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_GEM1_BTN_COMM_ENABLE, IDC_GEM1_BTN_LOG_LEVEL,	OnBtnClicked)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_GEM1_CHK_SPOOL_OVERWRITE, IDC_GEM1_CHK_SPOOL_OVERWRITE,OnChkClicked)
	ON_BN_CLICKED(IDC_GEM1_BTN_VAR_MIN_MAX,			OnBnClickedCtrlBtnVarMinMax)
	ON_BN_CLICKED(IDC_GEM1_BTN_VAR_TYPE,			OnBnClickedCtrlBtnVarType)
	ON_BN_CLICKED(IDC_GEM1_BTN_LOG_START,			OnBnClickedCtrlBtnLogStart)
	ON_BN_CLICKED(IDC_GEM1_BTN_LOG_STOP,			OnBnClickedCtrlBtnLogStop)
	ON_BN_CLICKED(IDC_GEM1_BTN_CMD_GET,				OnBnClickedCtrlBtnCmdGet)
	ON_BN_CLICKED(IDC_GEM1_BTN_REPORT_GET,			OnBnClickedCtrlBtnReportGet)
	ON_BN_CLICKED(IDC_GEM1_BTN_REPORTEVENT_GET,		OnBnClickedCtrlBtnReporteventGet)
	ON_BN_CLICKED(IDC_GEM1_BTN_PROJECT_GET,			OnBnClickedCtrlBtnProjectGet)
	ON_BN_CLICKED(IDC_GEM1_BTN_TRACE_GET,			OnBnClickedCtrlBtnTraceGet)
	ON_BN_CLICKED(IDC_GEM1_BTN_SAVE_EPJ,			OnBnClickedGem1BtnSaveEpj)
	ON_NOTIFY(NM_CLICK, IDC_GEM1_LST_ALARMS,		OnItemClickGem1LstAlarms)
	ON_NOTIFY(NM_DBLCLK, IDC_GEM1_LST_VAR_ITEMS,	OnNMDblclkGem1LstVarItems)
END_MESSAGE_MAP()

/*
 desc : 다이얼로그 생성 후 초기화
*/
BOOL CDlgGEM1::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	/* 컨트롤 초기화 */
	InitCtrl();

	/* 컨트롤에 값 초기화 */
	((CEdit*)GetDlgItem(IDC_GEM1_EDT_VAR_MINMAX_ID))->SetWindowText(L"11");
	((CComboBox*)GetDlgItem(IDC_GEM1_CMB_VAR_TYPE))->SetCurSel(0);

	return TRUE;
}

/*
 desc : 다이얼로그가 종료될 때 마지막 한번 호출
 parm : None
 retn : None
*/
VOID CDlgGEM1::OnClose()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnClose();
}
VOID CDlgGEM1::OnDestroy()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnDestroy();
}
VOID CDlgGEM1::ExitProc()
{
	/* 이미 이전에 호출된적이 있음 */
	m_bExitProc	= TRUE;

}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgGEM1::InitCtrl()
{
	CComboBox *pCmbLogLevel	= ((CComboBox*)GetDlgItem(IDC_GEM1_CMB_LOG_LEVEL));
	pCmbLogLevel->SetItemData(0, 0);
	pCmbLogLevel->SetItemData(1, 0x8000);
	pCmbLogLevel->SetItemData(2, 0x00010000);
	pCmbLogLevel->SetItemData(3, 0x00020000);
	pCmbLogLevel->SetItemData(4, 0x00080000);
	pCmbLogLevel->SetItemData(5, 0x00400000);
	pCmbLogLevel->SetItemData(6, 0x00100000);
	pCmbLogLevel->SetItemData(7, 0xffffffff);
	pCmbLogLevel->SetCurSel(7);

	/* Process Job IDs */
	m_lst_var.InsertColumn(1, L"ID",		LVCFMT_CENTER,	60);
	m_lst_var.InsertColumn(2, L"Name",		LVCFMT_LEFT,	140);
	m_lst_var.InsertColumn(3, L"C.Value",	LVCFMT_RIGHT,	95);
	m_lst_var.InsertColumn(4, L"D.Value",	LVCFMT_RIGHT,	95);
	m_lst_var.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	/* Commands */
	m_lst_cmd.InsertColumn(1, L"ID", LVCFMT_CENTER, 50);
	m_lst_cmd.InsertColumn(2, L"Name", LVCFMT_LEFT, 100);
	m_lst_cmd.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	/* Report */
	m_lst_rpt.InsertColumn(1, L"ID", LVCFMT_CENTER, 50);
	m_lst_rpt.InsertColumn(2, L"Name", LVCFMT_LEFT, 100);
	m_lst_rpt.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	/* Report & Event */
	m_lst_ret.InsertColumn(1, L"R_ID", LVCFMT_CENTER, 75);
	m_lst_ret.InsertColumn(2, L"E_ID", LVCFMT_LEFT, 75);
	m_lst_ret.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	/* Project */
	m_lst_prj.InsertColumn(1, L"Names", LVCFMT_CENTER, 200);
	m_lst_prj.InsertColumn(2, L"Size", LVCFMT_LEFT, 60);
	m_lst_prj.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	/* Trace */
	m_lst_trc.InsertColumn(1, L"ID", LVCFMT_CENTER, 50);
	m_lst_trc.InsertColumn(2, L"Name", LVCFMT_LEFT, 100);
	m_lst_trc.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	/* Alarms */
	m_lst_arm.InsertColumn(1, L"ID", LVCFMT_CENTER, 50);
	m_lst_arm.InsertColumn(2, L"Set", LVCFMT_LEFT, 100);
	m_lst_arm.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	((CComboBox *)GetDlgItem(IDC_GEM1_CMB_HOST_ID))->SetCurSel(0);
}

/*
 desc : 주기적으로 호출되는 함수
 parm : None
 retn : None
*/
VOID CDlgGEM1::UpdateCtrl()
{
	TCHAR tzValue[128]	= {NULL};
	BOOL bSucc			= FALSE;
//	BOOL bValue;
	UINT8 u8Value		= 0;
	UINT32 u32Value		= 0;
	LONG lValue			= 0;
	E30_SSMC enSpool	= E30_SSMC::en_inactive;
	CEdit *pEdit		= NULL;
	CButton *pBtn		= NULL;

	if (!uvCIMLib_IsCIMConnected())	return;

	UNG_CVVT unValue = {NULL};
	unValue.b_value	= FALSE;
//	uvCIMLib_GetIDValue(EQUIP_CONN_ID, 900604, unValue);
//	uvCIMLib_GetIDValue(EQUIP_CONN_ID, 900605, unValue);
//	uvCIMLib_SetCacheValueID(EQUIP_CONN_ID, E30_GPVT::en_b, 900605, unValue);
#if 0
	/* 통신 사용 갱신 */
	pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_COMM_ENABLE);	u32Value = 0L;
	bSucc	= uvCIMLib_GetIDValueU4(EQUIP_CONN_ID, 2036 /* CommState */, u32Value);
	pEdit->SetRedraw(FALSE);
	if (u32Value)	pEdit->SetWindowText(L"Enabled");
	else			pEdit->SetWindowText(L"Disabled");
	pEdit->SetRedraw(TRUE);
	pEdit->Invalidate(FALSE);
	/* 통신 상태 갱신 */
	pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_COMM_STATE);		u32Value = 0L;
	bSucc	= uvCIMLib_GetIDValueU4(EQUIP_CONN_ID, 2036 /* CommState */, u32Value);
	pEdit->SetRedraw(FALSE);
	switch (u32Value)
	{
	case E30_ESMC::en_disabled		:	pEdit->SetWindowText(L"Disabled");				break;
	case E30_ESMC::en_communicating	:	pEdit->SetWindowText(L"Communicating");			break;
	case E30_ESMC::en_wait_cra		:	pEdit->SetWindowText(L"WaitCRAOrCRFromHost");	break;
	case E30_ESMC::en_wait_delay		:	pEdit->SetWindowText(L"WaitDelayOrCRFromHost");	break;
	}
	pEdit->SetRedraw(TRUE);
	pEdit->Invalidate(FALSE);
#endif
#if 0
	/* CONTROLSTATE : Local or Remote */
	bSucc	= uvCIMLib_GetIDValueU1(EQUIP_CONN_ID, 2028 /* CONTROLSTATE */, u8Value);
	pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_RUN_MODE);
	pEdit->SetRedraw(FALSE);
	if (u8Value < 4)		pEdit->SetWindowText(L"None");
	else if (u8Value == 4)	pEdit->SetWindowText(L"Local");		/* 4 : OnlineLocal */
	else					pEdit->SetWindowText(L"Remote");	/* 5 : OnlineRemote */
	pEdit->SetRedraw(TRUE);
	pEdit->Invalidate(FALSE);
	/* CONTROLSTATE */
	pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_ONOFF_LINE);
	pEdit->SetRedraw(FALSE);
	if (u8Value < 4)	pEdit->SetWindowText(L"Offline");
	else				pEdit->SetWindowText(L"Online");
	pEdit->SetRedraw(TRUE);
	pEdit->Invalidate(FALSE);
	/* CONTROLSTATE */
	pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_ONOFF_STATE);
	pEdit->SetRedraw(FALSE);
	if (!bSucc)	pEdit->SetWindowText(L"None");
	else
	{
		switch (u8Value)
		{
		case 1 : pEdit->SetWindowText(L"EquipOffline");		break;
		case 2 : pEdit->SetWindowText(L"AttemptOnline");	break;
		case 3 : pEdit->SetWindowText(L"HostOffline");		break;
		case 4 : pEdit->SetWindowText(L"OnlineLocal");		break;
		case 5 : pEdit->SetWindowText(L"OnlineRemote");		break;
		}
	}
	pEdit->SetRedraw(TRUE);
	pEdit->Invalidate(FALSE);
#endif
#if 0
	/* Spool Enable or Disable */
	pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_SPOOL_STATE);
	pEdit->SetRedraw(FALSE);
	bSucc	= uvCIMLib_GetIDValueBo(EQUIP_CONN_ID, 4010 /* SpoolEnabled */, bValue);
	if (!bSucc)	pEdit->SetWindowText(L"None");
	else
	{
		if (bValue)	pEdit->SetWindowText(L"Enable");
		else		pEdit->SetWindowText(L"Disabled");
	}
	pEdit->SetRedraw(TRUE);
	pEdit->Invalidate(FALSE);
	/* Spool Overwrite : Check or Not check */
	pBtn	= (CButton*)GetDlgItem(IDC_GEM1_CHK_SPOOL_OVERWRITE);
	bSucc	= uvCIMLib_GetIDValueBo(EQUIP_CONN_ID, 4009 /* OverwriteSpool */, bValue);
	pBtn->SetCheck(bSucc && bValue);
	/* Spool Etc */
	bSucc	= uvCIMLib_GetIDValueU4(EQUIP_CONN_ID, 2037 /* SpoolState */, u32Value);
	if (u32Value)
	{
		/* Spool State : Values */
		pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_SPOOL_ACTIVE);
		pEdit->SetRedraw(FALSE);
		switch (u32Value)
		{
		case E30_SSMC::en_inactive			:	pEdit->SetWindowText(L"Inactive");			break;
		case E30_SSMC::en_active			:	pEdit->SetWindowText(L"Active");			break;
		case E30_SSMC::en_no_spool_output	:	pEdit->SetWindowText(L"No Spool Output");	break;	
		case E30_SSMC::en_transmit_spool	:	pEdit->SetWindowText(L"Transmit Spool");	break;
		case E30_SSMC::en_purge_spool		:	pEdit->SetWindowText(L"Purge Spool");		break;	
		case E30_SSMC::en_spool_not_full	:	pEdit->SetWindowText(L"Spool Not Full");	break;
		case E30_SSMC::en_spool_full		:	pEdit->SetWindowText(L"Spool Full");		break;
		}
		pEdit->SetRedraw(TRUE);
		pEdit->Invalidate(FALSE);
		/* Spool Actual & Total Count */
		pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_SPOOL_ACTUAL);
		if (bSucc)	bSucc	= uvCIMLib_GetIDValueU4(EQUIP_CONN_ID, 2016 /* SpoolCountActual */, u32Value);
		pEdit->SetRedraw(FALSE);
		swprintf_s(tzValue, 128, L"%d", u32Value);	pEdit->SetWindowText(tzValue);
		pEdit->SetRedraw(TRUE);	pEdit->Invalidate(FALSE);
		pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_SPOOL_TOTAL);
		if (bSucc)	bSucc	= uvCIMLib_GetIDValueU4(EQUIP_CONN_ID, 2017 /* SpoolCountTotal */, u32Value);
		pEdit->SetRedraw(FALSE);
		swprintf_s(tzValue, 128, L"%d", u32Value);	pEdit->SetWindowText(tzValue);
		pEdit->SetRedraw(TRUE);	pEdit->Invalidate(FALSE);

		/* Spool Full & Start Time */
		pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_SPOOL_FULL_TIME);	wmemset(tzValue, 0x00, 128);
		bSucc	= uvCIMLib_GetAsciiVariable(EQUIP_CONN_ID, 2018 /* SpoolFullTime */, tzValue, 128);
		pEdit->SetRedraw(FALSE);
		if (bSucc)	pEdit->SetWindowText(tzValue);
		pEdit->SetRedraw(TRUE);	pEdit->Invalidate(FALSE);
		pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_SPOOL_START_TIME);	wmemset(tzValue, 0x00, 128);
		bSucc	= uvCIMLib_GetAsciiVariable(EQUIP_CONN_ID, 2019 /* SpoolStartTime */, tzValue, 128);
		pEdit->SetRedraw(FALSE);
		if (bSucc)	pEdit->SetWindowText(tzValue);
		pEdit->SetRedraw(TRUE);	pEdit->Invalidate(FALSE);
	}
	else
	{
		pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_SPOOL_ACTIVE);		pEdit->SetWindowText(L"InActive");
		pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_SPOOL_ACTUAL);		pEdit->SetWindowText(L"0");
		pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_SPOOL_TOTAL);		pEdit->SetWindowText(L"0");
		pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_SPOOL_FULL_TIME);	pEdit->SetWindowText(L"");
		pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_SPOOL_START_TIME);	pEdit->SetWindowText(L"");
	}
#endif
#if 0	/* 왜 에러가 발생하는지 ... */
	/* Alarm Enable 여부 */
	UpdateAlarmsEnabled();
#endif
	/* 통신 상태 갱신 */
	UpdateConnectState();

	pEdit = (CEdit *)GetDlgItem(IDC_GEM1_EDT_LOG_STATUS);
	pEdit->SetRedraw(FALSE);
	uvCIMLib_IsLogging(bSucc);
	if (bSucc)	pEdit->SetWindowText(L"Logging ...");
	else		pEdit->SetWindowText(L"Not logging");
	pEdit->SetRedraw(TRUE);	pEdit->Invalidate(FALSE);

	/* 장비 (DI 장비) ID 값 반환 */
	bSucc	= uvCIMLib_GetHostToEquipID(lValue);
	if (bSucc)
	{
		swprintf_s(tzValue, 128, L"%d", lValue);
		pEdit = (CEdit*)GetDlgItem(IDC_GEM1_EDT_EQUIP_ID);
		pEdit->SetRedraw(FALSE);
		pEdit->SetWindowText(tzValue);
		pEdit->SetRedraw(TRUE);	pEdit->Invalidate(FALSE);
	}
}

/*
 desc : Update - CommEnableSwitch
 parm : None
 retn : None
*/
VOID CDlgGEM1::UpdateCommEnableSwitch()
{
	BOOL bSucc		= FALSE;
	CEdit *pEdit	= NULL;
	UINT32 u32Value;

	/* 컨트롤 갱신 비활성화 */
	pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_COMM_ENABLE);
	bSucc	= uvCIMLib_GetIDValueU4(EQUIP_CONN_ID, 2035 /* CommEnableSwitch */, u32Value);
	/* 전체 화면 갱신 중지 */
	pEdit->SetRedraw(FALSE);
	if (u32Value)	pEdit->SetWindowText(L"Enabled");
	else			pEdit->SetWindowText(L"Disabled");
	/* 컨트롤 갱신 활성화 */
	pEdit->SetRedraw(TRUE);
	pEdit->Invalidate(FALSE);
}

/*
 desc : Update - CommState
 parm : state	- [in]  현재 상태 값
 retn : None
*/
VOID CDlgGEM1::UpdateCommState(LONG state)
{
	CEdit *pEdit1	= NULL, *pEdit2 = NULL;

	/* 통신 사용 갱신 */
	pEdit1	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_COMM_ENABLE);
	pEdit2	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_COMM_STATE);
	pEdit1->SetRedraw(FALSE);
	pEdit2->SetRedraw(FALSE);
	if (E30_ESMC::en_disabled == (E30_ESMC)state)	pEdit1->SetWindowText(L"Disabled");
	else											pEdit1->SetWindowText(L"Enabled");

	switch (state)
	{
	case E30_ESMC::en_wait_cra					:	pEdit2->SetWindowText(L"Wait CRA");				break;
	case E30_ESMC::en_wait_delay				:	pEdit2->SetWindowText(L"Wait Delay");			break;
	case E30_ESMC::en_communicating				:	pEdit2->SetWindowText(L"Communicating");		break;
	case E30_ESMC::en_cr_from_host				:	pEdit2->SetWindowText(L"CR From Host");			break;
	case E30_ESMC::en_wait_cra_wait_cr_from_host:	pEdit2->SetWindowText(L"CRA Wait CR From Host");break;
	}
	/* 컨트롤 갱신 활성화 */
	pEdit1->SetRedraw(TRUE);	pEdit1->Invalidate(FALSE);
	pEdit2->SetRedraw(TRUE);	pEdit2->Invalidate(FALSE);
}

/*
 desc : Update - CommState
 parm : state	- [in]  현재 상태 값
 retn : None
*/
VOID CDlgGEM1::UpdateCtrlState(LONG state)
{
	CEdit *pEdit	= NULL;

	/* 컨트롤 갱신 비활성화 */
	pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_RUN_MODE);
	/* 전체 화면 갱신 중지 */
	pEdit->SetRedraw(FALSE);
	if (E30_CSMC(state) < E30_CSMC::en_online_local)	pEdit->SetWindowText(L"None");
	else
	{
		if (E30_CSMC::en_online_local == (E30_CSMC)state)	pEdit->SetWindowText(L"Local");
		else												pEdit->SetWindowText(L"Remote");
	}
	/* 컨트롤 갱신 활성화 */
	pEdit->SetRedraw(TRUE);
	pEdit->Invalidate(FALSE);

	/* 컨트롤 갱신 비활성화 */
	pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_ONOFF_LINE);
	/* 전체 화면 갱신 중지 */
	pEdit->SetRedraw(FALSE);
	if ((E30_CSMC)state <= E30_CSMC::en_host_offline)	pEdit->SetWindowText(L"Offline");
	else												pEdit->SetWindowText(L"Online");

	/* 컨트롤 갱신 활성화 */
	pEdit->SetRedraw(TRUE);
	pEdit->Invalidate(FALSE);

	/* 컨트롤 갱신 비활성화 */
	pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_ONOFF_STATE);
	/* 전체 화면 갱신 중지 */
	pEdit->SetRedraw(FALSE);
	switch (state)
	{
	case E30_CSMC::en_eq_offline	: pEdit->SetWindowText(L"EquipOffline");	break;
	case E30_CSMC::en_attempt_online: pEdit->SetWindowText(L"AttemptOnline");	break;
	case E30_CSMC::en_host_offline	: pEdit->SetWindowText(L"HostOffline");		break;
	case E30_CSMC::en_online_local	: pEdit->SetWindowText(L"OnlineLocal");		break;
	case E30_CSMC::en_online_remote	: pEdit->SetWindowText(L"OnlineRemote");	break;
	}
	/* 컨트롤 갱신 활성화 */
	pEdit->SetRedraw(TRUE);
	pEdit->Invalidate(FALSE);
}

/*
 desc : Update - ControlStateSwitch
 parm : None
 retn : None
*/
VOID CDlgGEM1::UpdateCtrlStateSwitch()
{
	BOOL bSucc		= FALSE;
	CEdit *pEdit	= NULL;
	UINT32 u32Value;

	/* 컨트롤 갱신 비활성화 */
	pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_RUN_MODE);
	bSucc	= uvCIMLib_GetIDValueU4(EQUIP_CONN_ID, 2033 /* ControlStateSwitch */, u32Value);
	/* 전체 화면 갱신 중지 */
	pEdit->SetRedraw(FALSE);
	if (!bSucc)	pEdit->SetWindowText(L"None");
	else
	{
		if (u32Value)	pEdit->SetWindowText(L"Remote");
		else			pEdit->SetWindowText(L"Local");
	}
	/* 컨트롤 갱신 활성화 */
	pEdit->SetRedraw(TRUE);
	pEdit->Invalidate(FALSE);
}

/*
 desc : Update - CtrlOnlineSwitch
 parm : None
 retn : None
*/
VOID CDlgGEM1::UpdateCtrlOnlineSwitch()
{
	BOOL bSucc		= FALSE;
	CEdit *pEdit	= NULL;
	UINT8 u8Value;
	UINT32 u32Value;

	/* 컨트롤 갱신 비활성화 */
	pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_ONOFF_LINE);
	bSucc	= uvCIMLib_GetIDValueU4(EQUIP_CONN_ID, 2034 /* CtrlOnlineSwitch */, u32Value);
	/* 전체 화면 갱신 중지 */
	pEdit->SetRedraw(FALSE);
	if (!bSucc)	pEdit->SetWindowText(L"None");
	else
	{
		if (u32Value)	pEdit->SetWindowText(L"Online");
		else			pEdit->SetWindowText(L"Offline");
	}
	/* 컨트롤 갱신 활성화 */
	pEdit->SetRedraw(TRUE);
	pEdit->Invalidate(FALSE);

	/* 컨트롤 갱신 비활성화 */
	pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_ONOFF_STATE);
	bSucc	= uvCIMLib_GetIDValueU1(EQUIP_CONN_ID, 2028 /* CONTROLSTATE */, u8Value);
	/* 전체 화면 갱신 중지 */
	pEdit->SetRedraw(FALSE);
	if (!bSucc)	pEdit->SetWindowText(L"None");
	else
	{
		switch (u8Value)
		{
		case 1 : pEdit->SetWindowText(L"EquipOffline");		break;
		case 2 : pEdit->SetWindowText(L"AttemptOnline");	break;
		case 3 : pEdit->SetWindowText(L"HostOffline");		break;
		case 4 : pEdit->SetWindowText(L"OnlineLocal");		break;
		case 5 : pEdit->SetWindowText(L"OnlineRemote");		break;
		}
	}
	/* 컨트롤 갱신 활성화 */
	pEdit->SetRedraw(TRUE);
	pEdit->Invalidate(FALSE);
}

/*
 desc : Update - SpoolState
 parm : None
 retn : None
*/
VOID CDlgGEM1::UpdateSpoolState(LONG state)
{
	CEdit *pEdit	= NULL;

	/* Spool State : Values */
	pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_SPOOL_ACTIVE);
	pEdit->SetWindowText(L"InActive");
	switch (state)
	{
	case E30_SSMC::en_inactive			:	pEdit->SetWindowText(L"Inactive");			break;
	case E30_SSMC::en_active				:	pEdit->SetWindowText(L"Active");			break;
	case E30_SSMC::en_no_spool_output	:	pEdit->SetWindowText(L"No Spool Output");	break;	
	case E30_SSMC::en_transmit_spool		:	pEdit->SetWindowText(L"Transmit Spool");	break;
	case E30_SSMC::en_purge_spool		:	pEdit->SetWindowText(L"Purge Spool");		break;	
	case E30_SSMC::en_spool_not_full		:	pEdit->SetWindowText(L"Spool Not Full");	break;
	case E30_SSMC::en_spool_full			:	pEdit->SetWindowText(L"Spool Full");		break;
	}
}

/*
 desc : Update - OverwriteSpool
 parm : None
 retn : None
*/
VOID CDlgGEM1::UpdateOverwriteSpool()
{
	BOOL bSucc		= FALSE;
	CButton *pBtn	= NULL;
	BOOL bValue;

	/* Spool Overwrite : Check or Not check */
	pBtn	= (CButton*)GetDlgItem(IDC_GEM1_CHK_SPOOL_OVERWRITE);
	bSucc	= uvCIMLib_GetIDValueBo(EQUIP_CONN_ID, 4009 /* OverwriteSpool */, bValue);
	if (bSucc)	pBtn->SetCheck(bSucc && bValue);
}

/*
 desc : Update - SpoolEnabled
 parm : None
 retn : None
*/
VOID CDlgGEM1::UpdateSpoolEnabled()
{
	BOOL bSucc		= FALSE;
	CEdit *pEdit	= NULL;
	BOOL bValue;

	/* Spool Enable or Disable */
	pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_SPOOL_STATE);
	bSucc	= uvCIMLib_GetIDValueBo(EQUIP_CONN_ID, 4010 /* SpoolEnabled */, bValue);
	if (!bSucc)	pEdit->SetWindowText(L"None");
	else
	{
		if (bValue)	pEdit->SetWindowText(L"Enable");
		else		pEdit->SetWindowText(L"Disabled");
	}
}

/*
 desc : Update - SpoolEnabled
 parm : None
 retn : None
*/
VOID CDlgGEM1::UpdateSpoolCountActual()
{
	TCHAR tzValue[128]	= {NULL};
	BOOL bSucc			= FALSE;
	CEdit *pEdit		= NULL;
	UINT32 u32Value;

	/* Spool Actual Count */
	pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_SPOOL_ACTUAL);
	if (bSucc)	bSucc	= uvCIMLib_GetIDValueU4(EQUIP_CONN_ID, 2016 /* SpoolCountActual */, u32Value);
	swprintf_s(tzValue, 128, L"%d", u32Value);
	pEdit->SetWindowText(tzValue);
}


/*
 desc : Update - SpoolEnabled
 parm : None
 retn : None
*/
VOID CDlgGEM1::UpdateSpoolCountTotal()
{
	TCHAR tzValue[128]	= {NULL};
	BOOL bSucc			= FALSE;
	CEdit *pEdit		= NULL;
	UINT32 u32Value;

	/* Spool Total Count */
	pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_SPOOL_TOTAL);
	if (bSucc)	bSucc	= uvCIMLib_GetIDValueU4(EQUIP_CONN_ID, 2017 /* SpoolCountTotal */, u32Value);
	swprintf_s(tzValue, 128, L"%d", u32Value);
	pEdit->SetWindowText(tzValue);
}


/*
 desc : Update - SpoolStartTime
 parm : None
 retn : None
*/
VOID CDlgGEM1::UpdateSpoolStartTime()
{
	TCHAR tzValue[128]	= {NULL};
	BOOL bSucc			= FALSE;
	CEdit *pEdit		= NULL;

	/* Spool Start Time */
	pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_SPOOL_START_TIME);	wmemset(tzValue, 0x00, 128);
	bSucc	= uvCIMLib_GetAsciiVariable(EQUIP_CONN_ID, 2019 /* SpoolStartTime */, tzValue, 128);
	if (bSucc)	pEdit->SetWindowText(tzValue);
}

/*
 desc : Update - SpoolFullTime
 parm : None
 retn : None
*/
VOID CDlgGEM1::UpdateSpoolFullTime()
{
	TCHAR tzValue[128]	= {NULL};
	BOOL bSucc			= FALSE;
	CEdit *pEdit		= NULL;

	/* Spool Full Time */
	pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_SPOOL_FULL_TIME);	wmemset(tzValue, 0x00, 128);
	bSucc	= uvCIMLib_GetAsciiVariable(EQUIP_CONN_ID, 2018 /* SpoolFullTime */, tzValue, 128);
	if (bSucc)	pEdit->SetWindowText(tzValue);
}

/*
 desc : Update - AlarmsEnabled
 parm : None
 retn : None
*/
VOID CDlgGEM1::UpdateAlarmsEnabled()
{
	BOOL bSucc		= FALSE;
	CEdit *pEdit	= NULL;
	INT32 i32Value;

	/* Spool Enable or Disable */
	pEdit	= (CEdit *)GetDlgItem(IDC_GEM1_EDT_ALARM_ENABLE);
	bSucc	= uvCIMLib_GetIDValueI4(EQUIP_CONN_ID, 2026 /* ALARMSENABLED */, i32Value);
	if (!bSucc)	pEdit->SetWindowText(L"None");
	else
	{
		if (i32Value)	pEdit->SetWindowText(L"Enable");
		else			pEdit->SetWindowText(L"Disabled");
	}
}

/*
 desc : Update - AlarmState
 parm : event	- [in]  Alarm Event 정보가 저장된 구조체 포인터
 retn : None
*/
VOID CDlgGEM1::UpdateAlarmState(STG_CCED *event)
{
	TCHAR tzID[16]	= {NULL}, tzCode[32]= {NULL};
	LONG i	= 0, iItem = 0;

	/* 기존 등록된 알람 정보 갱신 */
	AlarmGet();

	iItem	= m_lst_arm.GetItemCount();
	for (i=0; i<iItem; i++)
	{
		/* 현재 위치의 Alarm ID 값 얻기 */
		m_lst_arm.GetItemText(i, 0, tzID, 16);
		/* 현재 위치와 발생된 알람 ID와 동일한지 여부 */
		if (event->evt_data.e30_ac.alarm_id == LONG(_wtoi(tzID)))
		{
			/* 알람 발생 여부 값 추가 */
			swprintf_s(tzCode, 32, L"%d", event->evt_data.e30_ac.alarm_cd);
			m_lst_arm.SetItemText(i, 1, tzCode);
			break;
		}
	}
	if (i != iItem)	uvCmn_MoveScrollPosList(m_lst_arm.GetSafeHwnd(), i, TRUE);
}

/*
 desc : 이벤트 정보가 수신된 경우
 parm : event	- [in]  이벤트 정보가 저장된 구조체 포인터
 retn : None
*/
VOID CDlgGEM1::UpdateCtrl(STG_CCED *event)
{
	if (!uvCIMLib_IsCIMConnected())	return;
	switch (event->exx_id)
	{
	case ENG_ESSC::en_e30_gem	:
		switch (event->cbf_id)
		{
		case E30_CEFI::en_state_change	:
			switch (event->evt_data.e30_sc.machine)
			{
			case E30_SSMS::en_commstate	:	UpdateCommState(event->evt_data.e30_sc.new_state);		break;
			case E30_SSMS::en_control	:	UpdateCtrlState(event->evt_data.e30_sc.new_state);		break;
			case E30_SSMS::en_spooling	:	UpdateSpoolState(event->evt_data.e30_sc.new_state);		break;
			}
			break;

		case E30_CEFI::en_value_changed1	:	/* telcim.inf 파일 참조 */
			switch (event->evt_data.e30_vc.var_id)
			{
			case 2016 /* SpoolCountActual */			:	UpdateSpoolCountActual();			break;
			case 2017 /* SpoolCountTotal */				:	UpdateSpoolCountTotal();			break;
			case 2018 /* SpoolFullTime */				:	UpdateSpoolFullTime();				break;
			case 2019 /* SpoolStartTime */				:	UpdateSpoolStartTime();				break;

			case 2026 /* SetAlarmsEnabled */			:	UpdateAlarmsEnabled();				break;

			case 2033 /* ControlStateSwitch */			:	UpdateCtrlStateSwitch();			break;
			case 2034 /* CtrlOnlineSwitch */			:	UpdateCtrlOnlineSwitch();			break;
			case 2035 /* CommEnableSwitch */			:	UpdateCommEnableSwitch();			break;
			case 4009 /* OverwriteSpool */				:	UpdateOverwriteSpool();				break;
			case 4010 /* SpoolEnabled */				:	UpdateSpoolEnabled();				break;
			}
			break;

		case E30_CEFI::en_alarm_change			:	UpdateAlarmState(event);	break;
		}
		break;
	}
}

/*
 desc : 이벤트 ID
 parm : None
 retn : None
*/
VOID CDlgGEM1::OnBtnClicked(UINT32 id)
{
	switch(id)
	{
	case IDC_GEM1_BTN_COMM_ENABLE		:	uvCIMLib_SetCommunication(EQUIP_CONN_ID, TRUE);		break;
	case IDC_GEM1_BTN_COMM_DISABLE		:	uvCIMLib_SetCommunication(EQUIP_CONN_ID, FALSE);		break;
	case IDC_GEM1_BTN_CTRL_OFFLINE		:	uvCIMLib_SetControlOnline(EQUIP_CONN_ID, FALSE);		break;
	case IDC_GEM1_BTN_CTRL_ONLINE		:	uvCIMLib_SetControlOnline(EQUIP_CONN_ID, TRUE);		break;
	case IDC_GEM1_BTN_RUN_LOCAL			:	uvCIMLib_SetControlRemote(EQUIP_CONN_ID, FALSE);		break;
	case IDC_GEM1_BTN_RUN_REMOTE		:	uvCIMLib_SetControlRemote(EQUIP_CONN_ID, TRUE);		break;
	case IDC_GEM1_BTN_SPOOL_ENABLE		:	uvCIMLib_SetSpoolState(EQUIP_CONN_ID, TRUE);			break;
	case IDC_GEM1_BTN_SPOOL_DISABLE		:	uvCIMLib_SetSpoolState(EQUIP_CONN_ID, FALSE);			break;
	case IDC_GEM1_BTN_SPOOL_PURGE		:	uvCIMLib_SetSpoolPurge(EQUIP_CONN_ID);					break;
	case IDC_GEM1_BTN_ALARM_ENABLE		:	uvCIMLib_SetAlarmsEnabled(EQUIP_CONN_ID, 2026, TRUE);	break;
	case IDC_GEM1_BTN_ALARM_DISABLE		:	uvCIMLib_SetAlarmsEnabled(EQUIP_CONN_ID, 2026, FALSE);	break;
	case IDC_GEM1_BTN_ALARM_CLEAR_ALL	:	uvCIMLib_AlarmClearAll();								break;
	case IDC_GEM1_BTN_ALARM_CLEAR_SEL	:	AlarmClear();											break;
	case IDC_GEM1_BTN_ALARM_LIST_GET	:	AlarmGet();												break;
	case IDC_GEM1_BTN_ALARM_SET			:	AlarmSet();												break;
	case IDC_GEM1_BTN_LOG_LEVEL			:	SetLogLevel();											break;
	}
}

/*
 desc : 이벤트 ID
 parm : None
 retn : None
*/
VOID CDlgGEM1::OnChkClicked(UINT32 id)
{
	BOOL bValue	= FALSE;
	CButton *pBtn	= (CButton*)GetDlgItem(id);

	bValue	= pBtn->GetCheck() > 0 ? TRUE : FALSE;
	switch(id)
	{
	case IDC_GEM1_CHK_SPOOL_OVERWRITE	:	uvCIMLib_SetSpoolOverwrite(EQUIP_CONN_ID, bValue);	break;
	}
}

/*
 desc : 알람 리스트 출력
 parm : None
 retn : None
*/
VOID CDlgGEM1::AlarmGet()
{
	TCHAR tzID[16]	= {NULL}, tzState[16]= {NULL};
	LONG i	= 0, lAlarmID, lState;
	POSITION pPos	= NULL;
	CAtlList <LONG> lstAlarmID;
	CAtlList <LONG> lstAlarmState;

	m_lst_arm.SetRedraw(FALSE);
	m_lst_arm.DeleteAllItems();

	uvCIMLib_GetAlarmList(lstAlarmID, lstAlarmState, FALSE);
	for (; i<lstAlarmID.GetCount(); i++)
	{
		wmemset(tzID, 0x00, 16);
		wmemset(tzState, 0x00, 16);
		pPos	= lstAlarmID.FindIndex(i);
		lAlarmID= lstAlarmID.GetAt(pPos);
		pPos	= lstAlarmState.FindIndex(i);
		lState	= lstAlarmState.GetAt(pPos);
		swprintf_s(tzID, 16, L"%d", lAlarmID);
		swprintf_s(tzState, 16, L"%d", lState);
		m_lst_arm.InsertItem(i, tzID);
		m_lst_arm.SetItemText(i, 1, tzState);
	}

	m_lst_arm.SetRedraw(TRUE);
	m_lst_arm.Invalidate(TRUE);

	lstAlarmID.RemoveAll();
	lstAlarmState.RemoveAll();
}

/*
 desc : 알람 Clear
 parm : None
 retn : None
*/
VOID CDlgGEM1::AlarmClear()
{
	TCHAR tzID[16]	= {NULL};
	LONG i	= 0;

	/* 선택된 항목 얻기 */
	for (i=0; i<m_lst_arm.GetItemCount(); i++)
	{
		if (LVIS_SELECTED & m_lst_arm.GetItemState(i, LVIS_SELECTED))
		{
			m_lst_arm.GetItemText(i, 0, tzID, 16);
			break;
		}
	}
	if (wcslen(tzID) < 1)	return;
	uvCIMLib_AlarmClearID((LONG)_wtoi(tzID));
}

/*
 desc : 알람 Set
 parm : None
 retn : None
*/
VOID CDlgGEM1::AlarmSet()
{
	TCHAR tzID[16]	= {NULL};
	LONG i	= 0;

	/* 선택된 항목 얻기 */
	for (i=0; i<m_lst_arm.GetItemCount(); i++)
	{
		if (LVIS_SELECTED & m_lst_arm.GetItemState(i, LVIS_SELECTED))
		{
			m_lst_arm.GetItemText(i, 0, tzID, 16);
			break;
		}
	}
	if (wcslen(tzID) < 1)	return;
	uvCIMLib_AlarmSetID((LONG)_wtoi(tzID));
}

/*
 desc : 로그 발생 종류 선택
 parm : None
 retn : None
*/
VOID CDlgGEM1::SetLogLevel()
{
	CComboBox *pCmbLogLevel	= ((CComboBox*)GetDlgItem(IDC_GEM1_CMB_LOG_LEVEL));
	INT32 i32CurSel		= pCmbLogLevel->GetCurSel();
	UINT32 u32LogLevel	= 0;
	if (i32CurSel >= 0)
	{
		u32LogLevel	= (UINT32)pCmbLogLevel->GetItemData(i32CurSel);
		uvCIMLib_SetLogLevelAll(ENG_ILLV(u32LogLevel));
	}
}

/*
 desc : 통신 상태 갱신
 parm : None
 retn : None
*/
VOID CDlgGEM1::UpdateConnectState()
{
	E30_GCSS enState;
	E30_GCSH enSubstate;

	if (!uvCIMLib_GetCommState(EQUIP_CONN_ID, enState, enSubstate))	return;

	switch (enState)
	{
	case E30_GCSS::en_disabled					:	((CEdit*)GetDlgItem(IDC_GEM1_EDT_CONNECT_STATE))->SetWindowText(L"Disable");			break;
	case E30_GCSS::en_enabled_not_communicating	:	((CEdit*)GetDlgItem(IDC_GEM1_EDT_CONNECT_STATE))->SetWindowText(L"Not communicating");break;
	case E30_GCSS::en_communicating				:	((CEdit*)GetDlgItem(IDC_GEM1_EDT_CONNECT_STATE))->SetWindowText(L"communicating");	break;
	}

	switch (enSubstate)
	{
	case E30_GCSH::en_inactive			:	((CEdit*)GetDlgItem(IDC_GEM1_EDT_CONNECT_STATE_SUB))->SetWindowText(L"Inactive");		break;
	case E30_GCSH::en_not_connected		:	((CEdit*)GetDlgItem(IDC_GEM1_EDT_CONNECT_STATE_SUB))->SetWindowText(L"Not connected");	break;
	case E30_GCSH::en_con_not_selected	:	((CEdit*)GetDlgItem(IDC_GEM1_EDT_CONNECT_STATE_SUB))->SetWindowText(L"Not selected");	break;
	case E30_GCSH::en_con_selected		:	((CEdit*)GetDlgItem(IDC_GEM1_EDT_CONNECT_STATE_SUB))->SetWindowText(L"Selected");		break;
	}

	TCHAR tzName[128]	= {NULL};
	if (uvCIMLib_GetConnectionName(EQUIP_CONN_ID, tzName, 128))	((CEdit*)GetDlgItem(IDC_GEM1_EDT_CONNECT_NAME))->SetWindowText(tzName);
	if (uvCIMLib_GetCurrentProjectName(tzName, 128))				((CEdit*)GetDlgItem(IDC_GEM1_EDT_PROJECT_NAME_CUR))->SetWindowText(tzName);
	if (uvCIMLib_GetDefaultProjectName(tzName, 128))				((CEdit*)GetDlgItem(IDC_GEM1_EDT_PROJECT_NAME_DEF))->SetWindowText(tzName);
}

VOID CDlgGEM1::OnBnClickedCtrlBtnVarMinMax()
{
	TCHAR tzVarID[32]	= {NULL}, tzMin[32] = {NULL}, tzMax[32] = {NULL};
	E30_GPVT enType;
	INT64 i64Min = 0, i64Max = 0;
	((CEdit*)GetDlgItem(IDC_GEM1_EDT_VAR_MINMAX_ID))->GetWindowText(tzVarID, 32);
	if (wcslen(tzVarID) < 1)	return;
	if (!uvCIMLib_GetVarMinMax(EQUIP_CONN_ID, (LONG)_wtoi(tzVarID), enType, (PVOID)&i64Min, (PVOID)&i64Max))	return;
	switch (enType)
	{
	case E30_GPVT::en_i1	:
	case E30_GPVT::en_i2	:
	case E30_GPVT::en_i4	:
	case E30_GPVT::en_i8	:	swprintf_s(tzMin, 32, L"%I64d", i64Min);
								swprintf_s(tzMax, 32, L"%I64d", i64Max);			break;
	case E30_GPVT::en_u1	:
	case E30_GPVT::en_u2	:
	case E30_GPVT::en_u4	:
	case E30_GPVT::en_u8	:	swprintf_s(tzMin, 32, L"%I64u", UINT64(i64Min));
								swprintf_s(tzMax, 32, L"%I64u", UINT64(i64Max));	break;
	case E30_GPVT::en_f4	:
	case E30_GPVT::en_f8	:	swprintf_s(tzMin, 32, L"%.8f", DOUBLE(i64Min));
								swprintf_s(tzMax, 32, L"%.8f", DOUBLE(i64Max));		break;
	case E30_GPVT::en_b		:	swprintf_s(tzMin, 32, L"%d", BOOL(i64Min));
								swprintf_s(tzMax, 32, L"%d", BOOL(i64Max));			break;
	case E30_GPVT::en_bi	:	swprintf_s(tzMin, 32, L"%d", UINT8(i64Min));
								swprintf_s(tzMax, 32, L"%d", UINT8(i64Max));		break;
	case E30_GPVT::en_list	:	;	break;
	case E30_GPVT::en_jis8	:	;	break;
	case E30_GPVT::en_a		:	;	break;
	case E30_GPVT::en_w		:	;	break;
	}
	((CEdit*)GetDlgItem(IDC_GEM1_EDT_VAR_MIN))->SetWindowText(tzMin);
	((CEdit*)GetDlgItem(IDC_GEM1_EDT_VAR_MAX))->SetWindowText(tzMax);
}


void CDlgGEM1::OnBnClickedCtrlBtnVarType()
{
	TCHAR tzData[128]	= {NULL};
	INT32 i, i32CurSel	= ((CComboBox*)GetDlgItem(IDC_GEM1_CMB_VAR_TYPE))->GetCurSel();
	CAtlList <STG_CEVI> lstData;

	CComboBox *pCmbHostID	= (CComboBox *)GetDlgItem(IDC_GEM1_CMB_HOST_ID);
	INT32 i32HostID	= pCmbHostID->GetCurSel();
	if (i32HostID < 0)	return;
	uvCIMLib_GetHostVariables(i32HostID /* 0 or 1 or 2, ... */, E30_GVTC(i32CurSel), lstData);

	m_lst_var.SetRedraw(FALSE);
	for (i=0; i<lstData.GetCount(); i++)
	{
		STG_CEVI stData	= lstData.GetAt(lstData.FindIndex(i));
		swprintf_s(tzData, 128, L"%d", stData.var_id);

		if (m_lst_var.GetItemCount() > i)	m_lst_var.SetItemText(i, 0, tzData);
		else								m_lst_var.InsertItem(i, tzData);
		m_lst_var.SetItemText(i, 1, stData.var_name);
		m_lst_var.SetItemText(i, 2, stData.cur_val_str);
		m_lst_var.SetItemText(i, 3, stData.def_val_str);

		/* 메모리 해제 */
		stData.Delete();
	}

	/* 나머지 레코드 제거 */
	for (; i<m_lst_var.GetItemCount(); i++)	m_lst_var.DeleteItem(i);

	lstData.RemoveAll();
	m_lst_var.SetRedraw(TRUE);
	m_lst_var.Invalidate(FALSE);
}
void CDlgGEM1::OnBnClickedCtrlBtnLogStart()
{
	uvCIMLib_SetLogEnable(TRUE);
}
void CDlgGEM1::OnBnClickedCtrlBtnLogStop()
{
	uvCIMLib_SetLogEnable(FALSE);
}

void CDlgGEM1::OnBnClickedCtrlBtnCmdGet()
{
	UINT32 i	= 0;
	TCHAR tzID[32]	= {NULL};
	CAtlList <LONG> lstIDs;
	CStringArray arrNames;

	if (!uvCIMLib_GetCommandList(lstIDs, arrNames))	return;

	m_lst_cmd.SetRedraw(FALSE);
	m_lst_cmd.DeleteAllItems();
	for (i=0; i<lstIDs.GetCount(); i++)
	{
		wmemset(tzID, 0x00, 32);
		swprintf_s(tzID, 32, L"%d", lstIDs.GetAt(lstIDs.FindIndex(i)));
		m_lst_cmd.InsertItem(i, tzID);
		m_lst_cmd.SetItemText(i, 1, arrNames[i].GetBuffer());
	}
	lstIDs.RemoveAll();
	arrNames.RemoveAll();
	m_lst_cmd.SetRedraw(TRUE);
	m_lst_cmd.Invalidate(FALSE);
}


void CDlgGEM1::OnBnClickedCtrlBtnReportGet()
{
	UINT32 i	= 0;
	TCHAR tzID[32]	= {NULL};
	CAtlList <LONG> lstIDs;
	CStringArray arrNames;

	if (!uvCIMLib_GetReports(EQUIP_CONN_ID, lstIDs, arrNames))	return;
	
	m_lst_rpt.SetRedraw(FALSE);
	m_lst_rpt.DeleteAllItems();
	for (i=0; i<lstIDs.GetCount(); i++)
	{
		wmemset(tzID, 0x00, 32);
		swprintf_s(tzID, 32, L"%d", lstIDs.GetAt(lstIDs.FindIndex(i)));
		m_lst_rpt.InsertItem(i, tzID);
		m_lst_rpt.SetItemText(i, 1, arrNames[i].GetBuffer());
	}
	lstIDs.RemoveAll();
	arrNames.RemoveAll();
	m_lst_rpt.SetRedraw(TRUE);
	m_lst_rpt.Invalidate(FALSE);
}

void CDlgGEM1::OnBnClickedCtrlBtnReporteventGet()
{
	UINT32 i	= 0;
	TCHAR tzID[32]	= {NULL};
	CAtlList <LONG> lstRIDs;
	CAtlList <LONG> lstEIDs;

	if (!uvCIMLib_GetReportsEvents(EQUIP_CONN_ID, lstRIDs, lstEIDs))	return;
	
	m_lst_ret.SetRedraw(FALSE);
	m_lst_ret.DeleteAllItems();
	for (i=0; i<lstRIDs.GetCount(); i++)
	{
		wmemset(tzID, 0x00, 32);
		swprintf_s(tzID, 32, L"%d", lstRIDs.GetAt(lstRIDs.FindIndex(i)));
		m_lst_ret.InsertItem(i, tzID);
		swprintf_s(tzID, 32, L"%d", lstEIDs.GetAt(lstEIDs.FindIndex(i)));
		m_lst_ret.SetItemText(i, 1, tzID);
	}
	lstRIDs.RemoveAll();
	lstEIDs.RemoveAll();
	m_lst_ret.SetRedraw(TRUE);
	m_lst_ret.Invalidate(FALSE);
}

void CDlgGEM1::OnBnClickedCtrlBtnProjectGet()
{
	UINT32 i	= 0;
	TCHAR tzID[32]	= {NULL};
	CAtlList <LONG> lstSizes;
	CStringArray arrNames;

	if (!uvCIMLib_GetEPJList(arrNames, lstSizes))	return;
	
	m_lst_prj.SetRedraw(FALSE);
	m_lst_prj.DeleteAllItems();
	for (i=0; i<arrNames.GetCount(); i++)
	{
		wmemset(tzID, 0x00, 32);
		swprintf_s(tzID, 32, L"%d", lstSizes.GetAt(lstSizes.FindIndex(i)));
		m_lst_prj.InsertItem(i, arrNames[i].GetBuffer());
		m_lst_prj.SetItemText(i, 1, tzID);
	}
	lstSizes.RemoveAll();
	arrNames.RemoveAll();
	m_lst_prj.SetRedraw(TRUE);
	m_lst_prj.Invalidate(FALSE);
}


void CDlgGEM1::OnBnClickedCtrlBtnTraceGet()
{
	UINT32 i	= 0;
	TCHAR tzID[32]	= {NULL};
	CAtlList <LONG> lstIDs;
	CStringArray arrNames;

	if (!uvCIMLib_GetListTraces(EQUIP_CONN_ID, lstIDs, arrNames))	return;
	
	m_lst_trc.SetRedraw(FALSE);
	m_lst_trc.DeleteAllItems();
	for (i=0; i<lstIDs.GetCount(); i++)
	{
		wmemset(tzID, 0x00, 32);
		swprintf_s(tzID, 32, L"%d", lstIDs.GetAt(lstIDs.FindIndex(i)));
		m_lst_trc.InsertItem(i, tzID);
		m_lst_trc.SetItemText(i, 1, arrNames[i].GetBuffer());
	}
	lstIDs.RemoveAll();
	arrNames.RemoveAll();
	m_lst_trc.SetRedraw(TRUE);
	m_lst_trc.Invalidate(FALSE);
}

void CDlgGEM1::OnBnClickedGem1BtnSaveEpj()
{
	TCHAR tzTempPath[_MAX_PATH]	={NULL};
	TCHAR tzEPJFile[_MAX_PATH]	={NULL};
	TCHAR tzOpenFile[_MAX_PATH]	={NULL};
	GetTempPathW(_MAX_PATH, tzTempPath);

	swprintf_s(tzEPJFile, _MAX_PATH, L"%stemp.epj", tzTempPath);
	if (!uvCIMLib_SaveProjetsToEPJ(tzEPJFile))	return;
	swprintf_s(tzOpenFile,_MAX_PATH, L"/select,%s", tzEPJFile);
	ShellExecute(NULL, L"open", L"explorer", tzOpenFile, NULL, SW_SHOW);
}

void CDlgGEM1::OnItemClickGem1LstAlarms(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	LONG i	= 0;
	TCHAR tzID[16]	= {NULL};
	TCHAR tzName[CIM_CMD_NAME_SIZE]	= {NULL};

	GetDlgItem(IDC_GEM1_EDT_ALARM_NAME)->SetWindowText(L"");
	/* 선택된 항목 얻기 */
	for (i=0; i<m_lst_arm.GetItemCount(); i++)
	{
		if (LVIS_SELECTED & m_lst_arm.GetItemState(i, LVIS_SELECTED))
		{
			m_lst_arm.GetItemText(i, 0, tzID, 16);
			break;
		}
	}
	if (wcslen(tzID) < 1)	return;

	/* 알람 ID에 대한 알람 Name 얻기 */
	if (!uvCIMLib_GetAlarmIDToName(UINT32(_wtoi(tzID)), tzName, CIM_CMD_NAME_SIZE))	return;
	GetDlgItem(IDC_GEM1_EDT_ALARM_NAME)->SetWindowText(tzName);

	*pResult = 0;
}


void CDlgGEM1::OnNMDblclkGem1LstVarItems(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	NM_LISTVIEW *pListView	= (NM_LISTVIEW*)(pNMHDR);
	INT32 i32SelItem		= pListView->iItem;
	if (i32SelItem < 0)	return;
	TCHAR tzValue[4][128]	= {NULL};
	E30_GPVT enValType;
	E30_GVTC enVarType;

	INT32 i32HostID	= ((CComboBox *)GetDlgItem(IDC_GEM1_CMB_HOST_ID))->GetCurSel();

	/* 선택된 항목에 대한 VID, VName, VValue, VType 등 얻기오기 */
	m_lst_var.GetItemText(i32SelItem, 0, tzValue[0], 128);	/* VID */
	m_lst_var.GetItemText(i32SelItem, 1, tzValue[1], 128);	/* VName */
	m_lst_var.GetItemText(i32SelItem, 2, tzValue[2], 128);	/* Current.Value */
	m_lst_var.GetItemText(i32SelItem, 3, tzValue[3], 128);	/* Default.Value */

	if (!uvCIMLib_GetValueTypeID(i32HostID, (LONG)_wtoi(tzValue[0]), enValType))	return;
	if (!uvCIMLib_GetVarTypeID(i32HostID, (LONG)_wtoi(tzValue[0]), enVarType))		return;

	UINT32 u32Size	= (UINT32)wcslen(tzValue[2]) * sizeof(TCHAR);
	if (u32Size < 1)	return;
	PUINT8 pValue	= (PUINT8)::Alloc(u32Size+1);
	pValue[u32Size]	= 0x00;
	memcpy(pValue, tzValue[2], u32Size);
	CDlgData dlgData;
	dlgData.MyDoModal(i32HostID, enValType,
					  (UINT32)_wtoi(tzValue[0]),
					  tzValue[1],
					  pValue,
					  u32Size);
	::Free(pValue);

	*pResult = 0;
}
