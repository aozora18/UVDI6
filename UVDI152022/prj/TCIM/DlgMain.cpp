
/*
 desc : ���� ȭ��
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgMain.h"

#include "MainThread.h"

#include "DlgGEM1.h"
#include "DlgGEM2.h"
#include "DlgMesg.h"
#include "DlgTerm.h"
#include "DlgCIM3.h"
#include "DlgSIM3.h"
#include "DlgE40.h"
#include "DlgE87.h"
#include "DlgE90.h"
#include "DlgE94.h"
#include "DlgCJM.h"
#include "DlgPJM.h"
#include "DlgCbks.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
 desc : ������
 parm : None
 retn : None
*/
CDlgMain::CDlgMain(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgMain::IDD, pParent)
{
	m_bExitProc		= FALSE;
	m_hIcon			= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	m_pMainThread	= NULL;
	m_pDlgGEM1		= NULL;
	m_pDlgGEM2		= NULL;
	m_pDlgMesg		= NULL;
	m_pDlgTerm		= NULL;
	m_pDlgCIM3		= NULL;
	m_pDlgSIM3		= NULL;
	m_pDlgE40		= NULL;
	m_pDlgE87		= NULL;
	m_pDlgE90		= NULL;
	m_pDlgE94		= NULL;
	m_pDlgCJM		= NULL;
	m_pDlgPJM		= NULL;
	m_pDlgCbks		= NULL;
}

void CDlgMain::DoDataExchange(CDataExchange* dx)
{
	CDialogEx::DoDataExchange(dx);

	UINT32 i, u32StartID;

	u32StartID	= IDC_MAIN_TAB;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_tab_ctl[i]);
	u32StartID	= IDC_MAIN_RCH_LOGS;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_rch_ctl[i]);
	u32StartID	= IDC_MAIN_LST_APP_LIST;
	for (i=0; i<3; i++)		DDX_Control(dx, u32StartID+i,	m_lst_ctl[i]);
}

/* Message Map */
BEGIN_MESSAGE_MAP(CDlgMain, CDialogEx)
	ON_MESSAGE(WM_MAIN_THREAD,	OnMsgMainThread)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_MAIN_BTN_CLEAR,			&OnBnClickedMainBtnClear)
	ON_BN_CLICKED(IDC_MAIN_BTN_INIT_CIM,		&OnBnClickedMainBtnInitCim)
	ON_BN_CLICKED(IDC_MAIN_BTN_CLOSE_CIM,		&OnBnClickedMainBtnCloseCim)
	ON_BN_CLICKED(IDC_MAIN_BTN_CLEAR_MSG,		&OnBnClickedMainBtnClearMsg)
	ON_BN_CLICKED(IDC_MAIN_BTN_SET_APPNAME,		&OnBnClickedMainBtnSetAppname)
	ON_BN_CLICKED(IDC_MAIN_BTN_GET_APP_LIST,	&OnBnClickedMainBtnGetAppList)
	ON_BN_CLICKED(IDC_MAIN_BTN_GET_HOST_LIST,	&OnBnClickedMainBtnGetHostList)
	ON_BN_CLICKED(IDC_MAIN_BTN_GET_EQUIP_LIST,	&OnBnClickedMainBtnGetEquipList)
	ON_BN_CLICKED(IDC_MAIN_BTN_UPDATE,			&OnBnClickedMainBtnUpdate)
END_MESSAGE_MAP()


/*
 desc : �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	/* Library �ʱ�ȭ (�Ʒ� ���� �ݵ�� ��Ű��) */
	if (!uvCIMLib_Init(g_tzWorkDir))	return FALSE;

	/* Tab Control �ʱ�ȭ */
	InitTabChild();

	/* ���� ������ ���� (�� �������� ȣ��) */
	m_pMainThread	= new CMainThread(this->m_hWnd);
	ASSERT(m_pMainThread);
#ifdef _DEBUG
	if (!m_pMainThread->CreateThread(0, NULL, 100))
#else
	if (!m_pMainThread->CreateThread(0, NULL, 10))
#endif
	{
		AfxMessageBox(L"Can't create the MainThread", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* RichEdit Control */
	m_rch_ctl[0].SetKeyInputEnable(FALSE);
	m_rch_ctl[0].SetRichFont(L"D2Coding", 10);
	m_rch_ctl[0].SetAutoScroll(TRUE);
	m_rch_ctl[1].SetKeyInputEnable(FALSE);
	m_rch_ctl[1].SetRichFont(L"D2Coding", 10);
	m_rch_ctl[1].SetAutoScroll(TRUE);

	/* List Applications */
	m_lst_ctl[0].InsertColumn(1, L"ID", LVCFMT_CENTER, 50);
	m_lst_ctl[0].InsertColumn(2, L"Name", LVCFMT_LEFT, 130);
	m_lst_ctl[0].SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	/* List Connections (Hosts) */
	m_lst_ctl[1].InsertColumn(1, L"ID", LVCFMT_CENTER, 50);
	m_lst_ctl[1].InsertColumn(2, L"Name", LVCFMT_LEFT, 130);
	m_lst_ctl[1].SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	/* List Connections (Equipments) */
	m_lst_ctl[2].InsertColumn(1, L"ID", LVCFMT_CENTER, 50);
	m_lst_ctl[2].InsertColumn(2, L"Name", LVCFMT_LEFT, 120);
	m_lst_ctl[2].SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	((CEdit*)GetDlgItem(IDC_MAIN_EDT_EQUIP_ID))->SetWindowText(L"0");

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

/*
 desc : ���̾�αװ� ����� �� ������ �ѹ� ȣ��
 parm : None
 retn : None
*/
VOID CDlgMain::OnClose()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnClose();
}
VOID CDlgMain::OnDestroy()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnDestroy();
}
VOID CDlgMain::ExitProc()
{
	/* �̹� ������ ȣ������� ���� */
	m_bExitProc	= TRUE;
	/* ���� ������ ���� */
	if (m_pMainThread)
	{
		m_pMainThread->Stop();
		while (m_pMainThread->IsBusy())	Sleep(100);
		delete m_pMainThread;
	}
	/* ���� ���� �޽��� ť ���� */
	uvCmn_WaitPeekMessageQueue(1000);
	/* CIM300 ���̺귯�� ���� (�Ʒ� ���� ��Ű��) */
	uvCIMLib_Stop();
	uvCIMLib_Close();

	/* Tab �޸� ���� */
	m_tab_ctl[0].DeleteAllTabs();
	/* �ڽ� ���̾�α� �޸� ���� */
	CloseChild(m_pDlgGEM1);	m_pDlgGEM1	= NULL;	/* �ݵ�� NULL */
	CloseChild(m_pDlgGEM2);	m_pDlgGEM2	= NULL;	/* �ݵ�� NULL */
	CloseChild(m_pDlgMesg);	m_pDlgMesg	= NULL;	/* �ݵ�� NULL */
	CloseChild(m_pDlgTerm);	m_pDlgTerm	= NULL;	/* �ݵ�� NULL */
	CloseChild(m_pDlgCIM3);	m_pDlgCIM3	= NULL;	/* �ݵ�� NULL */
	CloseChild(m_pDlgSIM3);	m_pDlgSIM3	= NULL;	/* �ݵ�� NULL */
	CloseChild(m_pDlgE40);	m_pDlgE40	= NULL;	/* �ݵ�� NULL */
	CloseChild(m_pDlgE87);	m_pDlgE87	= NULL;	/* �ݵ�� NULL */
	CloseChild(m_pDlgE90);	m_pDlgE90	= NULL;	/* �ݵ�� NULL */
	CloseChild(m_pDlgE94);	m_pDlgE94	= NULL;	/* �ݵ�� NULL */
	CloseChild(m_pDlgCJM);	m_pDlgCJM	= NULL;	/* �ݵ�� NULL */
	CloseChild(m_pDlgPJM);	m_pDlgPJM	= NULL;	/* �ݵ�� NULL */
	CloseChild(m_pDlgCbks);	m_pDlgCbks	= NULL;	/* �ݵ�� NULL */
}

/*
 desc : �ڽ� ���̾�α� �޸� ����
 parm : child	- [in]  ���̾�α� ��ü ������
 retn : None
*/
VOID CDlgMain::CloseChild(CDialogEx *child)
{
	if (child)
	{
		if (child->GetSafeHwnd())	child->DestroyWindow();
		delete child;
	}
}

/*
 desc : ȭ�� ���ŵ� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgMain::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

/*
 desc : ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ��� �� �Լ� ȣ��
 parm : None
 retn : Ŀ�� �ڵ�
*/

HCURSOR CDlgMain::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*
 desc : Received to event message for MainThread
 parm : wparam	- [in]  event
		lparam	- [in]  Data
 retn : 0L
*/
LRESULT CDlgMain::OnMsgMainThread(WPARAM wparam, LPARAM lparam)
{
	/* �ֱ������� ȣ��Ǵ� �Լ� */
	UpdatePeriod();

	return 0L;
}

/*
 desc : �ֱ������� ȣ��Ǵ� �Լ�
 parm : None
 retn : None
*/
VOID CDlgMain::UpdatePeriod()
{
	BOOL bEvent			= FALSE;
	INT32 i32Index		= 0;
	TCHAR tzMesg[512]	= {NULL};
	COLORREF clrAlarm[4]= { RGB(0, 0, 0), RGB(192, 192, 192), RGB(255, 128, 0), RGB(255, 0, 0) };
	STG_CLLM stLogs		= {NULL};
	STG_CCED stEvent	= {NULL};
	CEdit *pEdit		= NULL;

	/* ���� �����尡 ���� ������ ���� */
	if (!m_pMainThread)	return;
	if (!m_pMainThread->IsBusy())	return;

	if (uvCIMLib_IsServiceRun())	((CEdit*)GetDlgItem(IDC_MAIN_EDT_CIM_RUN))->SetWindowText(L"Running");
	else							((CEdit*)GetDlgItem(IDC_MAIN_EDT_CIM_RUN))->SetWindowText(L"Stopped");
	bEvent	= uvCIMLib_PopEvent(stEvent);
	/* CIMConnection�� ������ �Ǿ� �ִ��� ���ο� ���� */
	if (uvCIMLib_IsCIMConnected())
	{

		switch (m_tab_ctl[0].GetCurSel())
		{
		case 0 : if (m_pDlgGEM1)	m_pDlgGEM1->UpdateCtrl();	break;
		case 1 : if (m_pDlgGEM2)	m_pDlgGEM2->UpdateCtrl();	break;
		case 2 : if (m_pDlgMesg)	m_pDlgMesg->UpdateCtrl();	break;
		case 3 : if (m_pDlgTerm)	m_pDlgTerm->UpdateCtrl();	break;
		case 4 : if (m_pDlgCIM3)	m_pDlgCIM3->UpdateCtrl();	break;
		case 5 : if (m_pDlgSIM3)	m_pDlgSIM3->UpdateCtrl();	break;
		case 6 : if (m_pDlgE40)		m_pDlgE40->UpdateCtrl();	break;
		case 7 : if (m_pDlgE87)		m_pDlgE87->UpdateCtrl();	break;
		case 8 : if (m_pDlgE90)		m_pDlgE90->UpdateCtrl();	break;
		case 9 : if (m_pDlgE94)		m_pDlgE94->UpdateCtrl();	break;
		case 10: if (m_pDlgCJM)		m_pDlgCJM->UpdateCtrl();	break;
		case 11: if (m_pDlgPJM)		m_pDlgPJM->UpdateCtrl();	break;
		case 12: if (m_pDlgCbks)	m_pDlgCbks->UpdateCtrl();	break;
		}
		/* �߻��� �̺�Ʈ ������ �ִ��� Ȯ�� */
		if (bEvent)
		{
			switch (stEvent.exx_id)
			{
			case ENG_ESSC::en_not_defined	:	break;
			case ENG_ESSC::en_e30_gem		:
				switch (stEvent.cbf_id)
				{
				case E30_CEFI::en_message_received	:	m_pDlgMesg->UpdateCtrl(&stEvent);	break;
				case E30_CEFI::en_terminal_msg_rcvd	:	m_pDlgTerm->UpdateCtrl(&stEvent);	break;
				default								:	m_pDlgGEM1->UpdateCtrl(&stEvent);
														m_pDlgGEM2->UpdateCtrl(&stEvent);	break;
				}
				break;
			case ENG_ESSC::en_e40_pjm		:
			case ENG_ESSC::en_e87_cms		:
			case ENG_ESSC::en_e90_sts		:
			case ENG_ESSC::en_e94_cjm		:
				switch (stEvent.exx_id)
				{
				case ENG_ESSC::en_e87_cms	:	
					m_pDlgE87->UpdateCtrl(&stEvent);	

					// Anderson 0515
					if ((E87_CEFI)stEvent.cbf_id == E87_CEFI::en_slot_map_verified)
					{
						m_pDlgCJM->UpdateCtrl(&stEvent);
					}
					break;

				case ENG_ESSC::en_e94_cjm	:
					// Anderson 0515
					if ((E94_CEFI)stEvent.cbf_id == E94_CEFI::en_cj_state_change ||
						(E94_CEFI)stEvent.cbf_id == E94_CEFI::en_attr_change_noti )
					{
						m_pDlgCJM->UpdateCtrl(&stEvent);
					}
					break;
				}
				m_pDlgCIM3->UpdateCtrl(&stEvent);
				m_pDlgSIM3->UpdateCtrl(&stEvent);	break;
			}
		}
	}

	/* �α� ������ ���� */
	if (uvCIMLib_PopLogs(stLogs))
	{
		/* Log Time ���� */
		if (stLogs.mesg)
		{
			swprintf_s(tzMesg, 512, L"%04I64u-%02I64u-%02I64u %02I64u:%02I64u:%02I64u[%03I64u] : %s",
					   stLogs.t_year, stLogs.t_month, stLogs.t_day,
					   stLogs.t_hour, stLogs.t_minute, stLogs.t_second, stLogs.t_msecond,
					   stLogs.mesg);
			/* ���� ��ϵ� ���� */
			m_rch_ctl[0].Append(tzMesg, clrAlarm[stLogs.level], TRUE);
		}

		/* �޸� ���� (�ݵ�� �ʿ�) */
		stLogs.Release();
	}

	/* �̺�Ʈ ���� ��� */
	if (bEvent)
	{
		wmemset(tzMesg, 0x00, 512);
		switch (stEvent.exx_id)
		{
		case ENG_ESSC::en_e30_gem	:
			switch (stEvent.cbf_id)
			{
			case E30_CEFI::en_value_changed1		:	wcscpy_s(tzMesg, 512, L"E30_VALUE_CHANGE / ");	break;
			case E30_CEFI::en_state_change		:	wcscpy_s(tzMesg, 512, L"E30_STATE_CHANGE / ");	break;
			case E30_CEFI::en_alarm_change		:	wcscpy_s(tzMesg, 512, L"E30_ALARM_CHANGE / ");	break;
			case E30_CEFI::en_terminal_msg_rcvd	:	wcscpy_s(tzMesg, 512, L"E30_TERM_MSG_RCV / ");	break;
			case E30_CEFI::en_message_received	:	wcscpy_s(tzMesg, 512, L"E30_USER_MSG_RCV / ");	break;
			case E30_CEFI::en_command_called		:	wcscpy_s(tzMesg, 512, L"E30_REMOTE_COMMAND / ");break;
			}
			break;
		case ENG_ESSC::en_e87_cms	:
			switch (stEvent.cbf_id)
			{
			case E87_CEFI::en_state_change		:	wcscpy_s(tzMesg, 512, L"E87_STATE_CHANGE / ");		break;
			case E87_CEFI::en_carrier_recreate	:	wcscpy_s(tzMesg, 512, L"E87_CARRIER_RECREATE / ");	break;
			}
			break;
		case ENG_ESSC::en_e40_pjm	:
			switch (stEvent.cbf_id)
			{
			case E40_CEFI::en_pr_command_callback1	:	wcscpy_s(tzMesg, 512, L"E40_PR_COMMAND_CALLBACK1 / ");	break;
			case E40_CEFI::en_pr_set_mtrl_order		:	wcscpy_s(tzMesg, 512, L"E40_PR_SET_MTRL_ORDER / ");		break;
			case E40_CEFI::en_pr_set_recipe_variable:	wcscpy_s(tzMesg, 512, L"E40_PRSET_RECIPE_VARIABLE / ");	break;
			case E40_CEFI::en_pr_state_change		:	wcscpy_s(tzMesg, 512, L"E40_PR_STATAE_CHANGE / ");		break;
			case E40_CEFI::en_pr_command_callback2	:	wcscpy_s(tzMesg, 512, L"E40_PR_COMMAND_CALLBACK2 / ");	break;
			}
			break;

		case ENG_ESSC::en_e90_sts	:
			switch (stEvent.cbf_id)
			{
			case E90_CEFI::en_command_callback			:	wcscpy_s(tzMesg, 512, L"E90_COMMAND_CALLBACK / ");		break;
			case E90_CEFI::en_substrate_state_change	:	wcscpy_s(tzMesg, 512, L"E90_SUBSTRATE_STATE_CHANGE / ");break;
			}
			break;
		case ENG_ESSC::en_e94_cjm	:
			switch (stEvent.cbf_id)
			{
			case E94_CEFI::en_cj_create					:	wcscpy_s(tzMesg, 512, L"E94_CJ_CREATE / ");				break;
			case E94_CEFI::en_cj_deselect				:	wcscpy_s(tzMesg, 512, L"E94_CJ_DESELECT / ");			break;
			case E94_CEFI::en_cj_hoq					:	wcscpy_s(tzMesg, 512, L"E94_HOQ	/ ");					break;
			case E94_CEFI::en_cj_state_change			:	wcscpy_s(tzMesg, 512, L"E94_STAGE_CHANGE / ");			break;
			case E94_CEFI::en_get_next_pj_job_id		:	wcscpy_s(tzMesg, 512, L"E94_GET_NEXT_PJ_JOB_ID / ");	break;
			case E94_CEFI::en_cj_command_callback2		:	wcscpy_s(tzMesg, 512, L"E94_COMMAND_CALLBACK2 / ");		break;
			case E94_CEFI::en_cj_command_callback3		:	wcscpy_s(tzMesg, 512, L"E94_COMMAND_CALLBACK3 / ");		break;
			case E94_CEFI::en_attr_change_noti			:	wcscpy_s(tzMesg, 512, L"E94_ATTR_CHANGE_NOTI / ");		break;
			case E94_CEFI::en_set_attr_change_request	:	wcscpy_s(tzMesg, 512, L"E94_SET_ATTR_CHANGE_REQ / ");	break;
			case E94_CEFI::en_abort_process_job			:	wcscpy_s(tzMesg, 512, L"E94_ABORT_PROCESS_JOB / ");		break;
			case E94_CEFI::en_cj_delete_pr_job			:	wcscpy_s(tzMesg, 512, L"E94_CJ_DELETE_PRJ_JOB / ");		break;
			case E94_CEFI::en_start_process_job			:	wcscpy_s(tzMesg, 512, L"E94_STATE_CHANGE / ");			break;
			case E94_CEFI::en_stop_process_job			:	wcscpy_s(tzMesg, 512, L"E94_STOP_PROCESS_JOB / ");		break;
			}
			break;
		default					:	wcscpy_s(tzMesg, 512, L"UNKNOWN_EVENT_CD / ");	break;
		}
		m_rch_ctl[1].Append(tzMesg, clrAlarm[0], FALSE);
		swprintf_s(tzMesg, 512, L"exx_id (%d) / cbf_id (%d)", stEvent.exx_id, stEvent.cbf_id);
		m_rch_ctl[1].Append(tzMesg, clrAlarm[0], TRUE);
	}

	/* �ݵ�� �޸� ���� */
	if (bEvent && stEvent.str_data1)	::Free(stEvent.str_data1);
	if (bEvent && stEvent.str_data2)	::Free(stEvent.str_data2);

	/* Get the AppName */
	pEdit	= ((CEdit*)GetDlgItem(IDC_MAIN_EDT_GET_APPNAME));
	uvCIMLib_GetAppName(tzMesg, 512);
	pEdit->SetRedraw(FALSE);
	pEdit->SetWindowText(tzMesg);
	pEdit->SetRedraw(TRUE);	pEdit->Invalidate(FALSE);
}

/*
 desc : TAB ���̾�α� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::InitTabChild()
{
	CPoint ptLeftCorner (5, 28);

	m_pDlgGEM1	= new CDlgGEM1;
	m_pDlgGEM1->Create(IDD_GEM1, &m_tab_ctl[0]);
	m_pDlgGEM2	= new CDlgGEM2;
	m_pDlgGEM2->Create(IDD_GEM2, &m_tab_ctl[0]);
	m_pDlgTerm	= new CDlgTerm;
	m_pDlgTerm->Create(IDD_TERM, &m_tab_ctl[0]);
	m_pDlgMesg	= new CDlgMesg;
	m_pDlgMesg->Create(IDD_MESG, &m_tab_ctl[0]);
	m_pDlgCIM3	= new CDlgCIM3;
	m_pDlgCIM3->Create(IDD_CIM3, &m_tab_ctl[0]);
	m_pDlgSIM3	= new CDlgSIM3;
	m_pDlgSIM3->Create(IDD_SIM3, &m_tab_ctl[0]);
	m_pDlgE40	= new CDlgE40;
	m_pDlgE40->Create(IDD_E40, &m_tab_ctl[0]);
	m_pDlgE87	= new CDlgE87;
	m_pDlgE87->Create(IDD_E87, &m_tab_ctl[0]);
	m_pDlgE90	= new CDlgE90;
	m_pDlgE90->Create(IDD_E90, &m_tab_ctl[0]);
	m_pDlgE94	= new CDlgE94;
	m_pDlgE94->Create(IDD_E94, &m_tab_ctl[0]);
	m_pDlgCJM	= new CDlgCJM;
	m_pDlgCJM->Create(IDD_CJM, &m_tab_ctl[0]);
	m_pDlgPJM	= new CDlgPJM;
	m_pDlgPJM->Create(IDD_PJM, &m_tab_ctl[0]);
	m_pDlgCbks	= new CDlgCbks;
	m_pDlgCbks->Create(IDD_CBKS, &m_tab_ctl[0]);

	/* Tab �Ӽ� ���� */
	m_tab_ctl[0].SetTopLeftCorner(ptLeftCorner);

	/* Tab ���� */
	m_tab_ctl[0].AddTab(m_pDlgGEM1,	L"GEM [1]");
	m_tab_ctl[0].AddTab(m_pDlgGEM2,	L"GEM [2]");
	m_tab_ctl[0].AddTab(m_pDlgMesg,	L"SECS-II Message");
	m_tab_ctl[0].AddTab(m_pDlgTerm,	L"TERM.MSG");
	m_tab_ctl[0].AddTab(m_pDlgCIM3,	L"CIM300");
	m_tab_ctl[0].AddTab(m_pDlgSIM3,	L"SIM300");
	m_tab_ctl[0].AddTab(m_pDlgE40,	L"E40PJM");
	m_tab_ctl[0].AddTab(m_pDlgE87,	L"E87CMS");
	m_tab_ctl[0].AddTab(m_pDlgE90,	L"E90STS");
	m_tab_ctl[0].AddTab(m_pDlgE94,	L"E94CJM");
	m_tab_ctl[0].AddTab(m_pDlgCJM,	L"Control Job");
	m_tab_ctl[0].AddTab(m_pDlgPJM,	L"Process Job");
	m_tab_ctl[0].AddTab(m_pDlgCbks,	L"Callback Event");

	/* �� ���� ���� ���� */
	m_tab_ctl[0].SetSelectedColor(RGB(255, 0, 0));
	m_tab_ctl[0].SetNormalColor(RGB(228, 228, 228));
	m_tab_ctl[0].SetMouseOverColor(RGB(255, 255, 255));

	return TRUE;
}

void CDlgMain::OnBnClickedMainBtnClear()
{
	m_rch_ctl[0].SetClearAll(TRUE);
}

void CDlgMain::OnBnClickedMainBtnInitCim()
{
	TCHAR tzFile[512]	= {L"e:\\������Ʈ\\���־���\\CIMETRIX\\������Ʈ\\TEL\\GEN-II BSA Mod1.epj"};
	TCHAR tzExt[128]	= {NULL};
	if (!uvCIMLib_Start(EQUIPMENT_ID, EQUIP_CONN_ID, tzFile))
	{
		AfxMessageBox(L"Failed to start the cim_connection", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* ���� ���񽺰� �������̸�, �ϴ� ���� �ϷḦ ���� ���� ȣ�� */
	if (!uvCIMLib_Finalized(EQUIP_CONN_ID, EQUIP_CONN_ID))
	{
		AfxMessageBox(L"Failed to completion the cim_connection", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* ������ ��� ���� ���� */
	if (!uvCIMLib_SetRecipePathName(L"e:\\������Ʈ\\���־���\\CIMETRIX\\������Ʈ\\TEL\\Recipes",
									EQUIP_CONN_ID))
	{
		AfxMessageBox(L"Failed to set the path for process program (=recipe)", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	if (!uvCIMLib_GetRecipFileExtName(EQUIP_CONN_ID, tzExt, 128))
	{
		AfxMessageBox(L"Failed to set the extension of process program (=recipe)", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
#if 0
	/* ������ Ȯ���ڰ� "*"�̸�, Ȯ���ڰ� ���� ����� */
	if (0 != wcscmp(tzExt, L"*"))
	{
		/* ������ Ȯ���� ��� */
		uvCIMLib_SetRecipeExtension(tzExt);
	}
	/* Communication Enabled */
	uvCIMLib_StartInitE30GEM();
#endif

	/* MDLN�� SOFTREV �ʱ�ȭ ���� */
	uvCIMLib_SetEquipModelType(EQUIP_CONN_ID, L"GEN-II BSA Mode I");
	uvCIMLib_SetEquipSoftRev(EQUIP_CONN_ID, L"1.2.20.0605");

	/* ��� �ڽ� ���̾�α� ��Ʈ�� �� ���� */
	OnBnClickedMainBtnUpdate();
}

void CDlgMain::OnBnClickedMainBtnCloseCim()
{
	uvCIMLib_Stop();
}


void CDlgMain::OnBnClickedMainBtnClearMsg()
{
	m_rch_ctl[1].SetClearAll(TRUE);
}

void CDlgMain::OnBnClickedMainBtnSetAppname()
{
	TCHAR tzName[128]	= {NULL};
	CEdit *pEdit	= ((CEdit*)GetDlgItem(IDC_MAIN_EDT_SET_APPNAME));
	pEdit->GetWindowText(tzName, 128);
	uvCIMLib_SetAppName(tzName);
}

void CDlgMain::OnBnClickedMainBtnGetAppList()
{
	INT32 i,i32Item	= 0;
	TCHAR tzEquipID[32]	= {NULL};
	TCHAR tzAppID[32]	= {NULL};
	CStringArray arrNames;
	CAtlList <LONG> lstIDs;

	((CEdit*)GetDlgItem(IDC_MAIN_EDT_EQUIP_ID))->GetWindowText(tzEquipID, 32);
	if (!uvCIMLib_GetListApplications((LONG)_wtoi(tzEquipID), lstIDs, arrNames))	return;

	m_lst_ctl[0].SetRedraw(FALSE);
	m_lst_ctl[0].DeleteAllItems();

	for (i=0; i<lstIDs.GetCount(); i++)
	{
		swprintf_s(tzAppID, 32, L"%d", lstIDs.GetAt(lstIDs.FindIndex(i)));
		m_lst_ctl[0].InsertItem(i, tzAppID);
		m_lst_ctl[0].SetItemText(i, 1, arrNames[i].GetBuffer());
	}

	m_lst_ctl[0].SetRedraw(TRUE);
	m_lst_ctl[0].Invalidate(TRUE);

	arrNames.RemoveAll();
	lstIDs.RemoveAll();
}

void CDlgMain::OnBnClickedMainBtnGetHostList()
{
	INT32 i,i32Item	= 0;
	TCHAR tzEquipID[32]	= {NULL};
	TCHAR tzAppID[32]	= {NULL};
	CStringArray arrNames;
	CAtlList <LONG> lstIDs;

	((CEdit*)GetDlgItem(IDC_MAIN_EDT_EQUIP_ID))->GetWindowText(tzEquipID, 32);
	if (!uvCIMLib_GetListConnections((LONG)_wtoi(tzEquipID), lstIDs, arrNames))	return;

	m_lst_ctl[1].SetRedraw(FALSE);
	m_lst_ctl[1].DeleteAllItems();

	for (i=0; i<lstIDs.GetCount(); i++)
	{
		swprintf_s(tzAppID, 32, L"%d", lstIDs.GetAt(lstIDs.FindIndex(i)));
		m_lst_ctl[1].InsertItem(i, tzAppID);
		m_lst_ctl[1].SetItemText(i, 1, arrNames[i].GetBuffer());
	}

	m_lst_ctl[1].SetRedraw(TRUE);
	m_lst_ctl[1].Invalidate(TRUE);

	arrNames.RemoveAll();
	lstIDs.RemoveAll();
}

void CDlgMain::OnBnClickedMainBtnGetEquipList()
{
	INT32 i,i32Item	= 0;
	TCHAR tzAppID[32]	= {NULL};
	CStringArray arrNames;
	CAtlList <LONG> lstIDs;

	if (!uvCIMLib_GetListEquipments(lstIDs, arrNames))	return;

	m_lst_ctl[2].SetRedraw(FALSE);
	m_lst_ctl[2].DeleteAllItems();

	for (i=0; i<lstIDs.GetCount(); i++)
	{
		swprintf_s(tzAppID, 32, L"%d", lstIDs.GetAt(lstIDs.FindIndex(i)));
		m_lst_ctl[2].InsertItem(i, tzAppID);
		m_lst_ctl[2].SetItemText(i, 1, arrNames[i].GetBuffer());
	}

	m_lst_ctl[2].SetRedraw(TRUE);
	m_lst_ctl[2].Invalidate(TRUE);

	arrNames.RemoveAll();
	lstIDs.RemoveAll();
}


void CDlgMain::OnBnClickedMainBtnUpdate()
{
	m_pDlgGEM1->UpdateCtrl();
	m_pDlgGEM2->UpdateCtrl();
	m_pDlgMesg->UpdateCtrl();
	m_pDlgTerm->UpdateCtrl();
	m_pDlgCIM3->UpdateCtrl();
	m_pDlgSIM3->UpdateCtrl();
	m_pDlgE40->UpdateCtrl();
	m_pDlgE87->UpdateCtrl();
	m_pDlgE90->UpdateCtrl();
	m_pDlgE94->UpdateCtrl();
	m_pDlgCJM->UpdateCtrl();
	m_pDlgPJM->UpdateCtrl();
	m_pDlgCbks->UpdateCtrl();
}
