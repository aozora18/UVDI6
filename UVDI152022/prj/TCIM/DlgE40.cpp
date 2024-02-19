
/*
 desc : E90STS
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgE40.h"


IMPLEMENT_DYNAMIC(CDlgE40, CDialogEx)

/*
 desc : 생성자
 parm : None
 retn : None
*/
CDlgE40::CDlgE40(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgE40::IDD, pParent)
{
	m_bExitProc	= FALSE;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgE40::~CDlgE40()
{
}

/* 컨트롤 연결 */
void CDlgE40::DoDataExchange(CDataExchange* dx)
{
	CDialogEx::DoDataExchange(dx);

	UINT32 i = 0, u32StartID = 0;

//	DDX_Control(dx, IDC_E90_LST_SUBST_LOC,	m_lst_ctl[0]);
}

/* 메시지 맵 */
BEGIN_MESSAGE_MAP(CDlgE40, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_E40_BTN_PROC_JOB_SEL_ALL, &CDlgE40::OnBnClickedE40BtnProcJobSelAll)
	ON_BN_CLICKED(IDC_E40_BTN_PROC_JOB_CLEAR, &CDlgE40::OnBnClickedE40BtnProcJobClear)
	ON_BN_CLICKED(IDC_E40_BTN_PROC_JOB_ADD, &CDlgE40::OnBnClickedE40BtnProcJobAdd)
	ON_BN_CLICKED(IDC_E40_BTN_PROC_JOB_DEL, &CDlgE40::OnBnClickedE40BtnProcJobDel)
	ON_BN_CLICKED(IDC_E40_BTN_PR_GET_SPACE, &CDlgE40::OnBnClickedE40BtnPrGetSpace)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_E40_BTN_PROC_JOB_SETUP, IDC_E40_BTN_OB_COMPLETE, OnBnClickedE40ProcState)
	ON_BN_CLICKED(IDC_E40_BTN_PROJOB_CMD, &CDlgE40::OnBnClickedE40BtnProjobCmd)
	ON_BN_CLICKED(IDC_E40_BTN_PAUSE_EVENT_GET, &CDlgE40::OnBnClickedE40BtnPauseEventGet)
	ON_BN_CLICKED(IDC_E40_BTN_PAUSE_EVENT_ENABLE, &CDlgE40::OnBnClickedE40BtnPauseEventEnable)
	ON_BN_CLICKED(IDC_E40_BTN_PAUSE_EVENT_DISABLE, &CDlgE40::OnBnClickedE40BtnPauseEventDisable)
	ON_BN_CLICKED(IDC_E40_BTN_NOTI_EVENT_GET, &CDlgE40::OnBnClickedE40BtnNotiEventGet)
	ON_BN_CLICKED(IDC_E40_BTN_NOTI_EVENT_E30, &CDlgE40::OnBnClickedE40BtnNotiEventE30)
	ON_BN_CLICKED(IDC_E40_BTN_NOTI_EVENT_E40, &CDlgE40::OnBnClickedE40BtnNotiEventE40)
	ON_BN_CLICKED(IDC_E40_BTN_NOTI_EVENT_BOTH, &CDlgE40::OnBnClickedE40BtnNotiEventBoth)
	ON_BN_CLICKED(IDC_E40_BTN_PR_JOB_ALERT, &CDlgE40::OnBnClickedE40BtnPrJobAlert)
	ON_BN_CLICKED(IDC_E40_BTN_WAIT_PR_ALL_GET, &CDlgE40::OnBnClickedE40BtnWaitPrAllGet)
	ON_BN_CLICKED(IDC_E40_BTN_WAIT_PR_ALL_YES, &CDlgE40::OnBnClickedE40BtnWaitPrAllYes)
	ON_BN_CLICKED(IDC_E40_BTN_WAIT_PR_ALL_NO, &CDlgE40::OnBnClickedE40BtnWaitPrAllNo)
	ON_BN_CLICKED(IDC_E40_BTN_CTRLJOB_SET, &CDlgE40::OnBnClickedE40BtnCtrljobSet)
	ON_BN_CLICKED(IDC_E40_BTN_PR_JOB_SUBST_UPDATE, &CDlgE40::OnBnClickedE40BtnPrJobSubstUpdate)
END_MESSAGE_MAP()

/*
 desc : 다이얼로그 생성 후 초기화
*/
BOOL CDlgE40::OnInitDialog()
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
VOID CDlgE40::OnClose()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnClose();
}
VOID CDlgE40::OnDestroy()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnDestroy();
}
VOID CDlgE40::ExitProc()
{
	/* 이미 이전에 호출된적이 있음 */
	m_bExitProc	= TRUE;

}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgE40::InitCtrl()
{
	INT32 i	= 0;
	TCHAR tzSlotNo[32]	= {NULL};

	/* List Box --> Check List Box 로 변경 */
	m_box_chk[0].SubclassDlgItem(IDC_E40_BOX_SLOT_ID, this);
	for (i=0; i<25; i++)
	{
		swprintf_s(tzSlotNo, 32, L"%02d", i+1);
		m_box_chk[0].InsertString(i, tzSlotNo);
		m_box_chk[0].SetItemHeight(i, 20);
		if (i < 3)	m_box_chk[0].SetCheck(i, 1);
	}
	
	((CEdit*)GetDlgItem(IDC_E40_EDT_CARR_ID))->SetWindowText(L"CARRIER1");
	((CEdit*)GetDlgItem(IDC_E40_EDT_PROC_JOB_ID))->SetWindowText(L"ProcessJob001");
	((CEdit*)GetDlgItem(IDC_E40_EDT_PROC_RECIPE_ID))->SetWindowText(L"Recipe001");

	((CComboBox*)GetDlgItem(IDC_E40_CMB_PRJOB_CMD))->SetCurSel(0);
	((CButton*)GetDlgItem(IDC_E40_CHK_PRJOB_AUTO_START))->SetCheck(1);
}

/*
 desc : 주기적으로 호출되는 함수
 parm : None
 retn : None
*/
VOID CDlgE40::UpdateCtrl()
{

}

/*
 desc : 이벤트 정보가 수신된 경우 (부모에 의해서 호출됨)
 parm : event	- [in]  이벤트 정보가 저장된 구조체 포인터
 retn : None
*/
VOID CDlgE40::UpdateCtrl(STG_CCED *event)
{
}


void CDlgE40::OnBnClickedE40BtnProcJobSelAll()
{
	INT32 i	= 0;
	for (i=0; i<m_box_chk[0].GetCount(); i++)	m_box_chk[0].SetCheck(i, BST_CHECKED);
}
void CDlgE40::OnBnClickedE40BtnProcJobClear()
{
	INT32 i	= 0;
	for (i=0; i<m_box_chk[0].GetCount(); i++)	m_box_chk[0].SetCheck(i, BST_UNCHECKED);
}


void CDlgE40::OnBnClickedE40BtnProcJobAdd()
{
	BOOL bAutoStart	= ((CButton*)GetDlgItem(IDC_E40_CHK_PRJOB_AUTO_START))->GetCheck() > 0;
	TCHAR tzCarrID[CIM_CARRIER_ID_SIZE]		= {NULL};
	TCHAR tzPRJobID[CIM_PROCESS_JOB_ID_SIZE]	= {NULL};
	TCHAR tzRecipeID[CIM_PRJOB_RECIPE_ID_SIZE]	= {NULL};
	CAtlList <UINT8> lstSlotNo;

	/* Get the carrier / process job / recipe id */
	((CEdit*)GetDlgItem(IDC_E40_EDT_CARR_ID))->GetWindowText(tzCarrID, CIM_CARRIER_ID_SIZE);
	((CEdit*)GetDlgItem(IDC_E40_EDT_PROC_JOB_ID))->GetWindowText(tzPRJobID, CIM_PROCESS_JOB_ID_SIZE);
	((CEdit*)GetDlgItem(IDC_E40_EDT_PROC_RECIPE_ID))->GetWindowText(tzRecipeID, CIM_PRJOB_RECIPE_ID_SIZE);

	for (INT32 i=0; i<m_box_chk[0].GetCount(); i++)
	{
		if (m_box_chk[0].GetCheck(i))
			lstSlotNo.AddTail(i+1);
	}

	uvCIMLib_E40PJM_SetProcessJobCreate(tzPRJobID, tzCarrID, tzRecipeID,
										E40_PPMT::en_carrier, TRUE, bAutoStart, &lstSlotNo);

	/* 메모리 해제 */
	lstSlotNo.RemoveAll();
}
void CDlgE40::OnBnClickedE40BtnProcJobDel()
{
	TCHAR tzPRJobID[CIM_PROCESS_JOB_ID_SIZE]	= {NULL};
	/* Get the process job name */
	((CEdit*)GetDlgItem(IDC_E40_EDT_PROC_JOB_ID))->GetWindowText(tzPRJobID, CIM_PROCESS_JOB_ID_SIZE);
	uvCIMLib_E40PJM_SetProcessJobRemove(tzPRJobID);
}


void CDlgE40::OnBnClickedE40BtnPrGetSpace()
{
	TCHAR tzCount[32]	= {NULL};
	UINT32 u32Count;
	if (!uvCIMLib_E40PJM_GetProcessJobSpaceCount(u32Count))	return;
	swprintf_s(tzCount, 32, L"%d", u32Count);
	((CEdit*)GetDlgItem(IDC_E40_EDT_PR_GET_SPACE))->SetWindowText(tzCount);
}


void CDlgE40::OnBnClickedE40ProcState(UINT32 id)
{
	TCHAR tzPRJobID[CIM_PROCESS_JOB_ID_SIZE]	= {NULL};
	/* Get the process job name */
	((CEdit*)GetDlgItem(IDC_E40_EDT_PROC_JOB_ID))->GetWindowText(tzPRJobID, CIM_PROCESS_JOB_ID_SIZE);

	switch (id)
	{
	case IDC_E40_BTN_PROC_JOB_SETUP		:	uvCIMLib_E40PJM_SetProcessJobQueueToSetup(tzPRJobID);				break;
	case IDC_E40_BTN_PROC_JOB_START		:	uvCIMLib_E40PJM_SetPRJobStartProcess(tzPRJobID);
											uvCIMLib_E40PJM_SetPRJobStateControl(tzPRJobID, E40_PPJC::en_start);break;
	case IDC_E40_BTN_PROC_JOB_COMPLETE	:	uvCIMLib_E40PJM_SetPRJobProcessComplete(tzPRJobID);					break;
	case IDC_E40_BTN_OB_COMPLETE		:	uvCIMLib_E40PJM_SetPRJobComplete(tzPRJobID);						break;
	}
}

void CDlgE40::OnBnClickedE40BtnProjobCmd()
{
	TCHAR tzPRJobID[CIM_PROCESS_JOB_ID_SIZE]	= {NULL};
	TCHAR tzPRJobCMD[64]	= {NULL};
	/* Get the process job name */
	((CEdit*)GetDlgItem(IDC_E40_EDT_PROC_JOB_ID))->GetWindowText(tzPRJobID, CIM_PROCESS_JOB_ID_SIZE);
	CComboBox *pCmb	= (CComboBox*)GetDlgItem(IDC_E40_CMB_PRJOB_CMD);
	if (pCmb->GetCurSel() < 0)	return;
	pCmb->GetLBText(pCmb->GetCurSel(), tzPRJobCMD);

	uvCIMLib_E40PJM_SetPRJobCommand(tzPRJobID, tzPRJobCMD);
}

void CDlgE40::OnBnClickedE40BtnPauseEventGet()
{
	BOOL bEnable			= FALSE;
	TCHAR tzEnable[2][32]	= { L"Disable", L"Enable" };

	if (!uvCIMLib_E40PJM_GetEnablePauseEvent(bEnable))	return;
	((CEdit*)GetDlgItem(IDC_E40_EDT_PAUSE_EVENT))->SetWindowText(tzEnable[bEnable]);
}
void CDlgE40::OnBnClickedE40BtnPauseEventEnable()
{
	uvCIMLib_E40PJM_SetEnablePauseEvent(TRUE);
}
void CDlgE40::OnBnClickedE40BtnPauseEventDisable()
{
	uvCIMLib_E40PJM_SetEnablePauseEvent(FALSE);
}

void CDlgE40::OnBnClickedE40BtnNotiEventGet()
{
	TCHAR tzType[4][32]	= { L"evtNone", L"evtE30", L"evtE40", L"evtBoth" };
	E40_PENT enType;
	if (!uvCIMLib_E40PJM_GetNotificationType(enType))	return;
	((CEdit*)GetDlgItem(IDC_E40_EDT_NOTI_EVENT))->SetWindowText(tzType[(INT32)enType]);
}
void CDlgE40::OnBnClickedE40BtnNotiEventE30()
{
	uvCIMLib_E40PJM_SetNotificationType(E40_PENT::en_evt_e30);
}
void CDlgE40::OnBnClickedE40BtnNotiEventE40()
{
	uvCIMLib_E40PJM_SetNotificationType(E40_PENT::en_evt_e40);
}
void CDlgE40::OnBnClickedE40BtnNotiEventBoth()
{
	uvCIMLib_E40PJM_SetNotificationType(E40_PENT::en_evt_both);
}

void CDlgE40::OnBnClickedE40BtnPrJobAlert()
{
	TCHAR tzProcID[CIM_PROCESS_JOB_ID_SIZE]	= {NULL};

	((CEdit*)GetDlgItem(IDC_E40_EDT_PROC_JOB_ID))->GetWindowText(tzProcID, CIM_PROCESS_JOB_ID_SIZE);
	uvCIMLib_E40PJM_SetPRJobAlert(tzProcID);
}

void CDlgE40::OnBnClickedE40BtnWaitPrAllGet()
{
	TCHAR tzProcID[CIM_PROCESS_JOB_ID_SIZE]	= {NULL};
	TCHAR tzWait[2][32]	= { L"No Wait ...", L"Wait All" };
	BOOL bWaitForAll	= TRUE;

	((CEdit*)GetDlgItem(IDC_E40_EDT_PROC_JOB_ID))->GetWindowText(tzProcID, CIM_PROCESS_JOB_ID_SIZE);
	if (!uvCIMLib_E40PJM_GetPRWaitForAllMaterial(tzProcID, bWaitForAll))	return;
	((CEdit*)GetDlgItem(IDC_E40_EDT_WAIT_PR_ALL))->SetWindowText(tzWait[bWaitForAll]);
}

void CDlgE40::OnBnClickedE40BtnWaitPrAllYes()
{
	TCHAR tzProcID[CIM_PROCESS_JOB_ID_SIZE]	= {NULL};
	((CEdit*)GetDlgItem(IDC_E40_EDT_PROC_JOB_ID))->GetWindowText(tzProcID, CIM_PROCESS_JOB_ID_SIZE);
	uvCIMLib_E40PJM_SetPRWaitForAllMaterial(tzProcID, TRUE);
}
void CDlgE40::OnBnClickedE40BtnWaitPrAllNo()
{
	TCHAR tzProcID[CIM_PROCESS_JOB_ID_SIZE]	= {NULL};
	((CEdit*)GetDlgItem(IDC_E40_EDT_PROC_JOB_ID))->GetWindowText(tzProcID, CIM_PROCESS_JOB_ID_SIZE);
	uvCIMLib_E40PJM_SetPRWaitForAllMaterial(tzProcID, FALSE);
}

void CDlgE40::OnBnClickedE40BtnCtrljobSet()
{
	TCHAR tzProcID[CIM_PROCESS_JOB_ID_SIZE]	= {NULL};
	((CEdit*)GetDlgItem(IDC_E40_EDT_PROC_JOB_ID))->GetWindowText(tzProcID, CIM_PROCESS_JOB_ID_SIZE);
	TCHAR tzCtrlID[CIM_CONTROL_JOB_ID_SIZE]	= {NULL};
	((CEdit*)GetDlgItem(IDC_E40_EDT_CTRL_JOB_ID))->GetWindowText(tzCtrlID, CIM_CONTROL_JOB_ID_SIZE);
	uvCIMLib_E40PJM_SetControlJobID(tzProcID, tzCtrlID);
}


void CDlgE40::OnBnClickedE40BtnPrJobSubstUpdate()
{
	uvCIMLib_Mixed_UpdateSubstrateIDsState();
}
