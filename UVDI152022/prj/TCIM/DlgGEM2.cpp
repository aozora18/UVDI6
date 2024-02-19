
/*
 desc : Control
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgGEM2.h"


IMPLEMENT_DYNAMIC(CDlgGEM2, CDialogEx)

/*
 desc : 생성자
 parm : None
 retn : None
*/
CDlgGEM2::CDlgGEM2(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgGEM2::IDD, pParent)
{
	m_bExitProc	= FALSE;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgGEM2::~CDlgGEM2()
{
}

/* 컨트롤 연결 */
void CDlgGEM2::DoDataExchange(CDataExchange* dx)
{
	CDialogEx::DoDataExchange(dx);
	DDX_Control(dx, IDC_GEM2_LST_LIST_VAR_GET,	m_lst_var);
	DDX_Control(dx, IDC_GEM2_LST_REMOTE_CMD,	m_lst_cmd);
	DDX_Control(dx, IDC_GEM2_LST_LIST_VAR_DI,	m_lst_di);
}

/* 메시지 맵 */
BEGIN_MESSAGE_MAP(CDlgGEM2, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_GEM2_BTN_LIST_VAR_GET, &CDlgGEM2::OnBnClickedCtrlBtnListVarGet)
	ON_BN_CLICKED(IDC_GEM2_BTN_LIST_VAL_GET, &CDlgGEM2::OnBnClickedCtrlBtnListValGet)
	ON_BN_CLICKED(IDC_GEM2_BTN_COMM_IF_GET, &CDlgGEM2::OnBnClickedCtrl2BtnCommIfGet)
	ON_BN_CLICKED(IDC_GEM2_BTN_PP_HOST_ENABLE, &CDlgGEM2::OnBnClickedCtrl2BtnPpHostEnable)
	ON_BN_CLICKED(IDC_GEM2_BTN_PP_HOST_DISABLE, &CDlgGEM2::OnBnClickedCtrl2BtnPpHostDisable)
	ON_BN_CLICKED(IDC_GEM2_BTN_PP_MGT_0, &CDlgGEM2::OnBnClickedCtrl2BtnPpMgt0)
	ON_BN_CLICKED(IDC_GEM2_BTN_PP_MGT_1, &CDlgGEM2::OnBnClickedCtrl2BtnPpMgt1)
	ON_BN_CLICKED(IDC_GEM2_BTN_PP_MGT_2, &CDlgGEM2::OnBnClickedCtrl2BtnPpMgt2)
	ON_BN_CLICKED(IDC_GEM2_BTN_LOG_SET_ENABLE, &CDlgGEM2::OnBnClickedCtrl2BtnLogSetEnable)
	ON_BN_CLICKED(IDC_GEM2_BTN_LOG_SET_DISABLE, &CDlgGEM2::OnBnClickedCtrl2BtnLogSetDisable)
	ON_BN_CLICKED(IDC_GEM2_BTN_REMOTECMD_ENABLE, &CDlgGEM2::OnBnClickedCtrl2BtnRemotecmdEnable)
	ON_BN_CLICKED(IDC_GEM2_BTN_REMOTECMD_DISABLE, &CDlgGEM2::OnBnClickedCtrl2BtnRemotecmdDisable)
	ON_BN_CLICKED(IDC_GEM2_BTN_BUILD_VER_GET, &CDlgGEM2::OnBnClickedGem2BtnBuildVerGet)
	ON_BN_CLICKED(IDC_GEM2_BTN_E87_ALARM_RESET, &CDlgGEM2::OnBnClickedGem2BtnE87AlarmReset)
	ON_BN_CLICKED(IDC_GEM2_BTN_CIMCONNECT_LOG, &CDlgGEM2::OnBnClickedGem2BtnCimconnectLog)
	ON_BN_CLICKED(IDC_GEM2_BTN_CIMCONNECT_LOG_YES, &CDlgGEM2::OnBnClickedGem2BtnCimconnectLogYes)
	ON_BN_CLICKED(IDC_GEM2_BTN_CIMCONNECT_LOG_NO, &CDlgGEM2::OnBnClickedGem2BtnCimconnectLogNo)
	ON_BN_CLICKED(IDC_GEM2_BTN_LIST_VAL_DI_LIST, &CDlgGEM2::OnBnClickedGem2BtnListValDiList)
	ON_BN_CLICKED(IDC_GEM2_BTN_LIST_VAL_DI_SET, &CDlgGEM2::OnBnClickedGem2BtnListValDiSet)
	ON_BN_CLICKED(IDC_GEM2_BTN_SECS_S14F19, &CDlgGEM2::OnBnClickedGem2BtnSecsS14f19)
	ON_BN_CLICKED(IDC_GEM2_BTN_SECS_S1F1, &CDlgGEM2::OnBnClickedGem2BtnSecsS1f1)
END_MESSAGE_MAP()

/*
 desc : 다이얼로그 생성 후 초기화
*/
BOOL CDlgGEM2::OnInitDialog()
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
VOID CDlgGEM2::OnClose()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnClose();
}
VOID CDlgGEM2::OnDestroy()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnDestroy();
}
VOID CDlgGEM2::ExitProc()
{
	/* 이미 이전에 호출된적이 있음 */
	m_bExitProc	= TRUE;

}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgGEM2::InitCtrl()
{
	((CComboBox *)GetDlgItem(IDC_GEM2_CMB_VAR_TYPE))->SetCurSel(0);
	/* Variables */
	m_lst_var.InsertColumn(1, L"ID", LVCFMT_CENTER, 50);
	m_lst_var.InsertColumn(2, L"Name", LVCFMT_LEFT, 160);
	m_lst_var.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	/* DI Variables */
	m_lst_di.InsertColumn(1, L"ID", LVCFMT_CENTER, 50);
	m_lst_di.InsertColumn(2, L"Name", LVCFMT_LEFT, 140);
	m_lst_di.InsertColumn(3, L"Value", LVCFMT_LEFT,100);
	m_lst_di.InsertColumn(4, L"Min", LVCFMT_LEFT, 110);
	m_lst_di.InsertColumn(5, L"Max", LVCFMT_LEFT, 110);
	m_lst_di.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	/* Remote Command */
	m_lst_cmd.InsertColumn(1, L"Time", LVCFMT_CENTER, 100);
	m_lst_cmd.InsertColumn(2, L"Host", LVCFMT_CENTER, 40);
	m_lst_cmd.InsertColumn(3, L"ID", LVCFMT_CENTER, 60);
	m_lst_cmd.InsertColumn(4, L"Command", LVCFMT_LEFT, 110);
	m_lst_cmd.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
}

/*
 desc : 주기적으로 호출되는 함수
 parm : None
 retn : None
*/
VOID CDlgGEM2::UpdateCtrl()
{
	BOOL bEnable		= FALSE;
	TCHAR tzValue[CIM_VAL_ASCII_SIZE]	= {NULL};
	UNG_CVVT unValue	= {NULL};
	CEdit *pEdit		= NULL;

	if (!uvCIMLib_IsCIMConnected())	return;

	/* PP (Process Program Enable or Disable) */
	uvCIMLib_GetIDValue(EQUIP_CONN_ID, 2045 /* HostPPEnable */, unValue);
	pEdit	= (CEdit*)GetDlgItem(IDC_GEM2_EDT_PP_HOST_ALLOW);
	if (unValue.b_value)	pEdit->SetWindowText(L"Enabled");
	else					pEdit->SetWindowText(L"Disabled");
	/* PP (Process Program Enable or Disable) */
	uvCIMLib_GetIDValue(EQUIP_CONN_ID, 2040 /* RecipeHandling */, unValue);
	pEdit	= (CEdit*)GetDlgItem(IDC_GEM2_EDT_PP_RECIPE_MGT);
	switch (unValue.u32_value)
	{
	case 0	: pEdit->SetWindowText(L"None");		break;
	case 1	: pEdit->SetWindowText(L"File-Based");	break;
	case 2	: pEdit->SetWindowText(L"Value-Based");	break;
	}
	/* PP Path */
	uvCIMLib_GetIDValue(EQUIP_CONN_ID, 2042 /* RecipePath */, unValue);
	pEdit	= (CEdit*)GetDlgItem(IDC_GEM2_EDT_PP_PATH);
	pEdit->SetWindowText(unValue.s_value);
	/* Logging Configuration Settings in the Windows Registry (저장 여부) */
	if (uvCIMLib_GetLoggingConfigSave(bEnable))
	{
		if (bEnable)	((CEdit *)GetDlgItem(IDC_GEM2_EDT_LOG_SET_ENABLE))->SetWindowText(L"Enable");
		else			((CEdit *)GetDlgItem(IDC_GEM2_EDT_LOG_SET_ENABLE))->SetWindowText(L"Disable");
	}
	/* Remote Command Enable or Disable */
	if (uvCIMLib_GetIDValue(EQUIP_CONN_ID, 2044 /* RemoteCmdnEnable */, unValue))
	{
		if (unValue.b_value)	((CEdit*)GetDlgItem(IDC_GEM2_EDT_REMOTECMD_ENABLE))->SetWindowText(L"Enable");
		else					((CEdit*)GetDlgItem(IDC_GEM2_EDT_REMOTECMD_ENABLE))->SetWindowText(L"Disable");
	}
}

/*
 desc : 이벤트 정보가 수신된 경우 (부모에 의해서 호출됨)
 parm : event	- [in]  이벤트 정보가 저장된 구조체 포인터
 retn : None
*/
VOID CDlgGEM2::UpdateCtrl(STG_CCED *event)
{
	switch (event->exx_id)
	{
	case ENG_ESSC::en_e30_gem	:
		switch (event->cbf_id)
		{
		case E30_CEFI::en_command_called	:	UpdateRemoteCtrl(event);	break;
		case E30_CEFI::en_value_changed1	:	/* telcim.inf 파일 참조 */
			UpdateUserValueChanged(event->evt_data.e30_vc.conn_id, event->evt_data.e30_vc.var_id);
			break;
		case E30_CEFI::en_ec_changed		:	UpdateECChangeInfo(event);	break;
		}
		break;
	}
}

/*
 desc : 원격 명령어 수신 처리
 parm : event	- [in]  이벤트 정보가 저장된 구조체 포인터
 retn : None
*/
VOID CDlgGEM2::UpdateRemoteCtrl(STG_CCED *event)
{
	TCHAR tzTime[32]= {NULL};
	TCHAR tzHID[32]	= {NULL};
	TCHAR tzAID[32]	= {NULL};
	SYSTEMTIME stTm	= {NULL};

	GetLocalTime(&stTm);
	swprintf_s(tzTime, 32, L"%02d:%02d:%02d.%03d", stTm.wHour, stTm.wMinute, stTm.wSecond, stTm.wMilliseconds);
	swprintf_s(tzHID, 32, L"%d", event->evt_data.e30_rc.conn_id);
	swprintf_s(tzAID, 32, L"%d", event->evt_data.e30_rc.rcmd_id);
	INT32 iItem = m_lst_cmd.GetItemCount();
	m_lst_cmd.InsertItem(iItem, tzTime);
	m_lst_cmd.SetItemText(iItem, 1, tzHID);
	m_lst_cmd.SetItemText(iItem, 2, tzAID);
	m_lst_cmd.SetItemText(iItem, 3, event->str_data1);
}

/*
 desc : 원격 명령어 수신 처리
 parm : event	- [in]  이벤트 정보가 저장된 구조체 포인터
 retn : None
*/
VOID CDlgGEM2::UpdateECChangeInfo(STG_CCED *event)
{
	TCHAR tzTime[32]= {NULL};
	TCHAR tzVarName[CIM_CMD_NAME_SIZE]	= {NULL}, tzVarValue[128] = {NULL};
	TCHAR tzHID[32]	= {NULL};
	LONG lVarID;

	swprintf_s(tzHID, 32, L"%d", event->evt_data.e30_ec.conn_id);
	wcscpy_s(tzVarName, CIM_CMD_NAME_SIZE, event->str_data1);
	uvCIMLib_GetNameToID(event->evt_data.e30_ec.conn_id, tzVarName, lVarID);
	uvCIMLib_GetValueToStr(event->evt_data.e30_ec.conn_id, lVarID, NULL, tzVarValue, 128);

	((CEdit*)GetDlgItem(IDC_GEM2_EDT_EC_CHANGE_VAR_NAME))->SetWindowText(tzVarName);
	((CEdit*)GetDlgItem(IDC_GEM2_EDT_EC_CHANGE_VAR_VALUE))->SetWindowText(tzVarValue);
}

/*
 desc : 현재 등록된 Variables (리스트) 모두 가져오기
 parm : None
 retn : None
*/
VOID CDlgGEM2::OnBnClickedCtrlBtnListVarGet()
{
	LONG i	= 0;
	TCHAR tzIDs[32]	= {NULL};
	CAtlList <LONG> lstVarIDs;
	CStringArray arrNames;
	E30_GVTC enType	= (E30_GVTC)((CComboBox*)GetDlgItem(IDC_GEM2_CMB_VAR_TYPE))->GetCurSel();

	if (!uvCIMLib_GetListVariables(EQUIP_CONN_ID/* or 0 */, enType, lstVarIDs, arrNames))	return;
	m_lst_var.SetRedraw(FALSE);
	m_lst_var.DeleteAllItems();
	for (; i<lstVarIDs.GetCount(); i++)
	{
		swprintf_s(tzIDs, 32, L"%d", lstVarIDs.GetAt(lstVarIDs.FindIndex(i)));
		m_lst_var.InsertItem(i, tzIDs);
		m_lst_var.SetItemText(i, 1, arrNames[i].GetBuffer());
	}

	m_lst_var.SetRedraw(TRUE);
	m_lst_var.Invalidate(FALSE);
}

void CDlgGEM2::OnBnClickedCtrlBtnListValGet()
{
	TCHAR tzBool[2][8]	= { L"FALSE", L"TRUE" };
	LONG i, lCount		= 0;
	E30_GPVT enType;
	UNG_CVVT unValue	= {NULL};
	TCHAR tzIDs[64]		= {NULL}, tzName[CIM_CMD_NAME_SIZE]	= {NULL};
	TCHAR tzValue[32]	= {NULL};

	if (m_lst_var.GetSelectedCount() < 1)	return;
	lCount	= m_lst_var.GetItemCount();
	for (i=0; i<lCount; i++)
	{
		if (LVIS_SELECTED == m_lst_var.GetItemState(i, LVIS_SELECTED))
		{
			m_lst_var.GetItemText(i, 0, tzIDs, 64);
			break;
		}
	}
	if (wcslen(tzIDs) < 0)	return;

	/* ID to Name */
	if (!uvCIMLib_GetIDValue(EQUIP_CONN_ID, _wtoi(tzIDs), unValue))	return;
	/* Value Type */
	if (!uvCIMLib_GetValueTypeID(EQUIP_CONN_ID, _wtoi(tzIDs), enType))	return;
	switch (enType)
	{
	case E30_GPVT::en_i1	:	swprintf_s(tzValue, 32, L"%d",		unValue.i8_value);			break;
	case E30_GPVT::en_i2	:	swprintf_s(tzValue, 32, L"%d",		unValue.i16_value);			break;
	case E30_GPVT::en_i4	:	swprintf_s(tzValue, 32, L"%I32d",	unValue.i32_value);			break;
	case E30_GPVT::en_i8	:	swprintf_s(tzValue, 32, L"%I64d",	unValue.i64_value);			break;
	case E30_GPVT::en_u1	:	swprintf_s(tzValue, 32, L"%u",		unValue.u8_value);			break;
	case E30_GPVT::en_u2	:	swprintf_s(tzValue, 32, L"%u",		unValue.u16_value);			break;
	case E30_GPVT::en_u4	:	swprintf_s(tzValue, 32, L"%I32u",	unValue.u32_value);			break;
	case E30_GPVT::en_u8	:	swprintf_s(tzValue, 32, L"%I64u",	unValue.u64_value);			break;
	case E30_GPVT::en_f4	:	swprintf_s(tzValue, 32, L"%.4f",	unValue.f_value);			break;
	case E30_GPVT::en_f8	:	swprintf_s(tzValue, 32, L"%.4f",	unValue.d_value);			break;
	case E30_GPVT::en_b		:	swprintf_s(tzValue, 32, L"%s",		tzBool[unValue.b_value]);	break;
	case E30_GPVT::en_bi	:	swprintf_s(tzValue, 32, L"%u",		unValue.i8_value);			break;
	}
	((CEdit*)GetDlgItem(IDC_GEM2_EDT_LIST_VAR_VALUE))->SetWindowText(tzValue);
}


void CDlgGEM2::OnBnClickedCtrl2BtnCommIfGet()
{
	TCHAR tzCommPgid[128]	= {NULL};
	TCHAR tzCommSets[256]	= {NULL};
	uvCIMLib_GetCommSetupInfo(EQUIP_CONN_ID, tzCommPgid, 128, tzCommSets, 256);

	((CEdit*)GetDlgItem(IDC_GEM2_EDT_COMM_IF_PGID))->SetWindowText(tzCommPgid);
	((CEdit*)GetDlgItem(IDC_GEM2_EDT_COMM_IF_SETTINGS))->SetWindowText(tzCommSets);
}


void CDlgGEM2::OnBnClickedCtrl2BtnPpHostEnable()
{
	UNG_CVVT unValue	= {1};
	uvCIMLib_SetCacheValueID(EQUIP_CONN_ID, E30_GPVT::en_b, 2045 /* HostPPEnable */, unValue);
}

void CDlgGEM2::OnBnClickedCtrl2BtnPpHostDisable()
{
	UNG_CVVT unValue	= {0};
	uvCIMLib_SetCacheValueID(EQUIP_CONN_ID, E30_GPVT::en_b, 2045 /* HostPPEnable */, unValue);
}

void CDlgGEM2::OnBnClickedCtrl2BtnPpMgt0()
{
	UNG_CVVT unValue	= {0};
	uvCIMLib_SetCacheValueID(EQUIP_CONN_ID, E30_GPVT::en_u4, 2040 /* RecipeHandling */, unValue);
}
void CDlgGEM2::OnBnClickedCtrl2BtnPpMgt1()
{
	UNG_CVVT unValue	= {1};
	uvCIMLib_SetCacheValueID(EQUIP_CONN_ID, E30_GPVT::en_u4, 2040 /* RecipeHandling */, unValue);
}
void CDlgGEM2::OnBnClickedCtrl2BtnPpMgt2()
{
	UNG_CVVT unValue	= {2};
	uvCIMLib_SetCacheValueID(EQUIP_CONN_ID, E30_GPVT::en_u4, 2040 /* RecipeHandling */, unValue);
}

void CDlgGEM2::OnBnClickedCtrl2BtnLogSetEnable()
{
	uvCIMLib_SetLoggingConfigSave(TRUE);
}
void CDlgGEM2::OnBnClickedCtrl2BtnLogSetDisable()
{
	uvCIMLib_SetLoggingConfigSave(FALSE);
}

void CDlgGEM2::OnBnClickedCtrl2BtnRemotecmdEnable()
{
	UNG_CVVT unValue	= {1};
	uvCIMLib_SetCacheValueID(EQUIP_CONN_ID, E30_GPVT::en_b, 2044 /* RemoteCmdEnable */, unValue);
}
void CDlgGEM2::OnBnClickedCtrl2BtnRemotecmdDisable()
{
	UNG_CVVT unValue	= {0};
	uvCIMLib_SetCacheValueID(EQUIP_CONN_ID, E30_GPVT::en_b, 2044 /* RemoteCmdEnable */, unValue);
}

void CDlgGEM2::OnBnClickedGem2BtnBuildVerGet()
{
	TCHAR tzData[128]	= {NULL};

	uvCIMLib_GetBuildDateTime(tzData, 128);
	((CEdit*)GetDlgItem(IDC_GEM2_EDT_BUILD_DATE_TIME))->SetWindowText(tzData);

	wmemset(tzData, 0x00, 128);
	uvCIMLib_GetBuildVersionStr(tzData, 128);
	((CEdit*)GetDlgItem(IDC_GEM2_EDT_VERSION_STR))->SetWindowText(tzData);
}

void CDlgGEM2::OnBnClickedGem2BtnE87AlarmReset()
{
}

void CDlgGEM2::OnBnClickedGem2BtnCimconnectLog()
{
	BOOL bYesNo	= TRUE;
	TCHAR tzYesNo[2][16]	= {L"No save", L"Saved"};

	uvCIMLib_GetLogToCIMConnect(bYesNo);
	((CEdit*)GetDlgItem(IDC_GEM2_EDT_LOG_SAVE))->SetWindowText(tzYesNo[bYesNo]);
}
void CDlgGEM2::OnBnClickedGem2BtnCimconnectLogYes()
{
	uvCIMLib_SetLogToCIMConnect(TRUE);
}
void CDlgGEM2::OnBnClickedGem2BtnCimconnectLogNo()
{
	uvCIMLib_SetLogToCIMConnect(FALSE);
}

/*
 desc : 나머지 ValueChanged 이벤트 발생에 대한 처리
 parm : var_id	- [in]  variable id
 retn : None
*/
VOID CDlgGEM2::UpdateUserValueChanged(LONG host_id, UINT32 var_id)
{
	TCHAR tzValue[128]	= {NULL};
	UINT32 u32ID		= 0;
	UNG_CVVT unData;
	if (!uvCIMLib_GetIDValue(host_id, var_id, unData))	return;

	switch (var_id)
	{
	case 18000	:	u32ID	= IDC_GEM2_EDT_EXPO_TYPE_H;
					swprintf_s(tzValue, 128, L"%u", unData.u8_value);	break;
	case 18001	:	u32ID	= IDC_GEM2_EDT_CARR_ID_VERIF;
					swprintf_s(tzValue, 128, L"%u", unData.u32_value);	break;
	case 18002	:	u32ID	= IDC_GEM2_EDT_SUB_ID_VERIF;
					swprintf_s(tzValue, 128, L"%u", unData.u32_value);	break;
	case 18003	:	u32ID	= IDC_GEM2_EDT_ALIGN_SPEED;
					swprintf_s(tzValue, 128, L"%d", unData.i32_value);	break;
	}
	if (u32ID)	((CEdit*)GetDlgItem(u32ID))->SetWindowText(tzValue);
}


void CDlgGEM2::OnBnClickedGem2BtnListValDiList()
{
	LONG i	= 0, iItem;
	TCHAR tzVarID[64]	= {NULL}, tzMin[64] = {NULL}, tzMax[64] = {NULL}, tzValue[128] = {NULL};
	CAtlList <LONG> lstVarIDs;
	CStringArray arrNames;

	m_lst_di.SetRedraw(FALSE);
	m_lst_di.DeleteAllItems();
	if (uvCIMLib_GetListVariables(EQUIP_CONN_ID/* or 0 */, E30_GVTC::en_dv, lstVarIDs, arrNames))
	{
		for (; i<lstVarIDs.GetCount(); i++)
		{
			if (lstVarIDs.GetAt(lstVarIDs.FindIndex(i)) < 10000 ||
				lstVarIDs.GetAt(lstVarIDs.FindIndex(i)) > 10029)	continue;

			swprintf_s(tzVarID, 64, L"%d", lstVarIDs.GetAt(lstVarIDs.FindIndex(i)));

			wmemset(tzMin, 0x00, 64);	wmemset(tzMax, 0x00, 64);	wmemset(tzValue, 0x00, 128);

			if (!uvCIMLib_GetVarMinMaxToStr(EQUIP_CONN_ID, (LONG)_wtoi(tzVarID), tzMin, tzMax, 64))	continue;

			uvCIMLib_GetValueToStr(EQUIP_CONN_ID, (LONG)_wtoi(tzVarID), NULL, tzValue, 128);

			iItem = m_lst_di.GetItemCount();
			m_lst_di.InsertItem(iItem, tzVarID);
			m_lst_di.SetItemText(iItem, 1, arrNames[i].GetBuffer());
			m_lst_di.SetItemText(iItem, 2, tzValue);
			m_lst_di.SetItemText(iItem, 3, tzMin);
			m_lst_di.SetItemText(iItem, 4, tzMax);		
		}
		lstVarIDs.RemoveAll();
		arrNames.RemoveAll();
	}
	if (uvCIMLib_GetListVariables(EQUIP_CONN_ID/* or 0 */, E30_GVTC::en_ec, lstVarIDs, arrNames))
	{
		for (i=0; i<lstVarIDs.GetCount(); i++)
		{
#ifdef _DEBUG
		POSITION pPos	= lstVarIDs.FindIndex(i);
		LONG lVarID		= lstVarIDs.GetAt(pPos);
#endif
			if (lstVarIDs.GetAt(lstVarIDs.FindIndex(i)) < 17999 ||
				lstVarIDs.GetAt(lstVarIDs.FindIndex(i)) > 18004)	continue;

			swprintf_s(tzVarID, 64, L"%d", lstVarIDs.GetAt(lstVarIDs.FindIndex(i)));

			wmemset(tzMin, 0x00, 64);	wmemset(tzMax, 0x00, 64);	wmemset(tzValue, 0x00, 128);

			if (!uvCIMLib_GetVarMinMaxToStr(EQUIP_CONN_ID, (LONG)_wtoi(tzVarID), tzMin, tzMax, 64))	continue;

			uvCIMLib_GetValueToStr(EQUIP_CONN_ID, (LONG)_wtoi(tzVarID), NULL, tzValue, 128);

			iItem = m_lst_di.GetItemCount();
			m_lst_di.InsertItem(iItem, tzVarID);
			m_lst_di.SetItemText(iItem, 1, arrNames[i].GetBuffer());
			m_lst_di.SetItemText(iItem, 2, tzValue);
			m_lst_di.SetItemText(iItem, 3, tzMin);
			m_lst_di.SetItemText(iItem, 4, tzMax);		
		}
		lstVarIDs.RemoveAll();
		arrNames.RemoveAll();
	}

	m_lst_di.SetRedraw(TRUE);
	m_lst_di.Invalidate(FALSE);
}


void CDlgGEM2::OnBnClickedGem2BtnListValDiSet()
{
	TCHAR tzIDs[64]	= {NULL}, tzVal[64] = {NULL};
	E30_GPVT enType;
	LONG lCount, i;
	UNG_CVVT unVal	= {NULL};
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_GEM2_EDT_LIST_VAR_DI);
	if (!pEdit)	return;

	pEdit->GetWindowText(tzVal, 64);

	if (m_lst_di.GetSelectedCount() < 1)	return;
	lCount	= m_lst_di.GetItemCount();
	for (i=0; i<lCount; i++)
	{
		if (LVIS_SELECTED == m_lst_di.GetItemState(i, LVIS_SELECTED))
		{
			m_lst_di.GetItemText(i, 0, tzIDs, 64);
			break;
		}
	}
	if (wcslen(tzIDs) < 0)	return;

	/* 값 타입 */
	if (!uvCIMLib_GetValueTypeID(EQUIP_CONN_ID, (LONG)_wtoi(tzIDs), enType))	return;
	/* 값 설정 */
	if (!uvCIMLib_SetCacheStrValueID(EQUIP_CONN_ID, enType, (LONG)_wtoi(tzIDs), tzVal))	return;
}

VOID CDlgGEM2::OnBnClickedGem2BtnSecsS14f19()
{
	TCHAR tzValue[2][64]= {NULL};
	UINT8 u8Value		= 0;
	UINT32 u32Value		= 0;
	LONG lTransID		= 0;
	LONG lHandle1, lHandle2;
#if 0
	L {U4 0} {U4 0} {A } {A TransferSubstrateObject }
	{ L {L {A SubstID}	{A CARRIER1.01} }
	{ L {A SubstUsage}  {U1 0} }
	{ L {A SubstSource} {A CARRIER1.01} }
	{ L {A SubstLotID}  {A LOT1}} }
#endif

	/* L1.01 - Root Message */
	if (!uvCIMLib_AddSecsMesgRoot(E30_GPVT::en_u4, &u32Value, 4))	return;
	/* L1.02 */
	if (!uvCIMLib_AddSecsMesgValue(0, E30_GPVT::en_u4, &u32Value, 4))	return;
	/* L1.03 */
	if (!uvCIMLib_AddSecsMesgValue(0, E30_GPVT::en_a, tzValue[0], (UINT32)wcslen(tzValue[0])))	return;
	/* L1.04 */
	swprintf_s(tzValue[0], 64, L"TransferSubstrateObject");
	if (!uvCIMLib_AddSecsMesgValue(0, E30_GPVT::en_a, tzValue[0], (UINT32)wcslen(tzValue[0])))	return;

	/* L2.List */
	if ((lHandle1 = uvCIMLib_AddSecsMesgList(0)) < 0)	return;

	/* L2.List - 1 */
	if ((lHandle2 = uvCIMLib_AddSecsMesgList(lHandle1)) < 0)	return;
	/* L2.L1.01/02*/
	swprintf_s(tzValue[0], 64, L"SubstID");		swprintf_s(tzValue[1], 64, L"CARRIER1.01");
	if (!uvCIMLib_AddSecsMesgValue(lHandle2, E30_GPVT::en_a, tzValue[0], (UINT32)wcslen(tzValue[0])))	return;
	if (!uvCIMLib_AddSecsMesgValue(lHandle2, E30_GPVT::en_a, tzValue[1], (UINT32)wcslen(tzValue[1])))	return;

	/* L2.List - 2 */
	if ((lHandle2 = uvCIMLib_AddSecsMesgList(lHandle1)) < 0)	return;
	/* L2.L1.01/02*/
	swprintf_s(tzValue[0], 64, L"SubstUsage");
	if (!uvCIMLib_AddSecsMesgValue(lHandle2, E30_GPVT::en_a, tzValue[0], (UINT32)wcslen(tzValue[0])))	return;
	if (!uvCIMLib_AddSecsMesgValue(lHandle2, E30_GPVT::en_u1, &u8Value, 1))								return;

	/* L2.List - 3 */
	if ((lHandle2 = uvCIMLib_AddSecsMesgList(lHandle1)) < 0)	return;
	/* L2.L1.01/02*/
	swprintf_s(tzValue[0], 64, L"SubstSource");	swprintf_s(tzValue[1], 64, L"CARRIER1.01");
	if (!uvCIMLib_AddSecsMesgValue(lHandle2, E30_GPVT::en_a, tzValue[0], (UINT32)wcslen(tzValue[0])))	return;
	if (!uvCIMLib_AddSecsMesgValue(lHandle2, E30_GPVT::en_a, tzValue[1], (UINT32)wcslen(tzValue[1])))	return;

	/* L2.List - 4 */
	if ((lHandle2 = uvCIMLib_AddSecsMesgList(lHandle1)) < 0)	return;
	/* L2.L1.01/02*/
	swprintf_s(tzValue[0], 64, L"SubstLotID");	swprintf_s(tzValue[1], 64, L"LOT1");
	if (!uvCIMLib_AddSecsMesgValue(lHandle2, E30_GPVT::en_a, tzValue[0], (UINT32)wcslen(tzValue[0])))	return;
	if (!uvCIMLib_AddSecsMesgValue(lHandle2, E30_GPVT::en_a, tzValue[1], (UINT32)wcslen(tzValue[1])))	return;

	/* Host에게 전달 */
	if (!uvCIMLib_SendSecsMessage(EQUIP_CONN_ID, 14, 19, TRUE, lTransID, TRUE))	return;
}


void CDlgGEM2::OnBnClickedGem2BtnSecsS1f1()
{
	LONG lTransID	= 0;

	/* 기존 등록된 메시지 정보 초기화 */
	uvCIMLib_ResetSecsMesgList();
	/* Host에게 전달 */
	if (!uvCIMLib_SendSecsMessage(EQUIP_CONN_ID, 1, 1, TRUE, lTransID, TRUE))	return;
}
