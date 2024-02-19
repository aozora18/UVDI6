
/*
 desc : E90STS
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgE90.h"


IMPLEMENT_DYNAMIC(CDlgE90, CDialogEx)

/*
 desc : 생성자
 parm : None
 retn : None
*/
CDlgE90::CDlgE90(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgE90::IDD, pParent)
{
	m_bExitProc	= FALSE;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgE90::~CDlgE90()
{
}

/* 컨트롤 연결 */
void CDlgE90::DoDataExchange(CDataExchange* dx)
{
	CDialogEx::DoDataExchange(dx);

	UINT32 i = 0, u32StartID = 0;

	DDX_Control(dx, IDC_E90_LST_SUBST_LOC,	m_lst_ctl[0]);
	DDX_Control(dx, IDC_E90_LST_SUBST_ID,	m_lst_ctl[1]);
}

/* 메시지 맵 */
BEGIN_MESSAGE_MAP(CDlgE90, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_E90_BTN_REMOTE_CTRL_GET, &CDlgE90::OnBnClickedE90BtnRemoteCtrlGet)
	ON_BN_CLICKED(IDC_E90_BTN_REMOTE_CTRL_ENABLE, &CDlgE90::OnBnClickedE90BtnRemoteCtrlEnable)
	ON_BN_CLICKED(IDC_E90_BTN_REMOTE_CTRL_DISABLE, &CDlgE90::OnBnClickedE90BtnRemoteCtrlDisable)
	ON_BN_CLICKED(IDC_E90_BTN_SUBST_LOC_ADD, &CDlgE90::OnBnClickedE90BtnSubstLocAdd)
	ON_BN_CLICKED(IDC_E90_BTN_SUBST_LOC_DEL, &CDlgE90::OnBnClickedE90BtnSubstLocDel)
	ON_BN_CLICKED(IDC_E90_BTN_SUBST_ID_ADD, &CDlgE90::OnBnClickedE90BtnSubstIdAdd)
	ON_BN_CLICKED(IDC_E90_BTN_SUBST_ID_DEL, &CDlgE90::OnBnClickedE90BtnSubstIdDel)
	ON_BN_CLICKED(IDC_E90_BTN_SUBST_LOC_UPDATE, &CDlgE90::OnBnClickedE90BtnSubstLocUpdate)
	ON_BN_CLICKED(IDC_E90_BTN_SUBST_ID_UPDATE, &CDlgE90::OnBnClickedE90BtnSubstIdUpdate)
	ON_BN_CLICKED(IDC_E90_BTN_SUBST_LOC_ADDS, &CDlgE90::OnBnClickedE90BtnSubstLocAdds)
	ON_BN_CLICKED(IDC_E90_BTN_SUBST_LOC_DELS, &CDlgE90::OnBnClickedE90BtnSubstLocDels)
	ON_BN_CLICKED(IDC_E90_BTN_SUBST_LOC_STATE, &CDlgE90::OnBnClickedE90BtnSubstLocState)
	ON_BN_CLICKED(IDC_E90_BTN_SUBST_ID_PROC, &CDlgE90::OnBnClickedE90BtnSubstIdProc)
	ON_BN_CLICKED(IDC_E90_BTN_SUBST_LOC_STATES, &CDlgE90::OnBnClickedE90BtnSubstLocStates)
	ON_BN_CLICKED(IDC_E90_BTN_SUBST_ID_TRANS, &CDlgE90::OnBnClickedE90BtnSubstIdTrans)
	ON_BN_CLICKED(IDC_E90_BTN_SUBST_ID_READ, &CDlgE90::OnBnClickedE90BtnSubstIdRead)
END_MESSAGE_MAP()

/*
 desc : 다이얼로그 생성 후 초기화
*/
BOOL CDlgE90::OnInitDialog()
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
VOID CDlgE90::OnClose()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnClose();
}
VOID CDlgE90::OnDestroy()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnDestroy();
}
VOID CDlgE90::ExitProc()
{
	/* 이미 이전에 호출된적이 있음 */
	m_bExitProc	= TRUE;

}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgE90::InitCtrl()
{
	TCHAR tzLocID[CIM_SUBSTRATE_LOC_ID]	= {NULL};
	CComboBox *pCmb	= (CComboBox*)GetDlgItem(IDC_E90_CMB_SUBST_LOC);
	pCmb->SetCurSel(0);

	/* Location Setup 여부 */
	m_lst_ctl[0].InsertColumn(1, L"Location ID", LVCFMT_LEFT, 110);
	m_lst_ctl[0].InsertColumn(2, L"State", LVCFMT_CENTER, 85);
	m_lst_ctl[0].SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	wmemset(tzLocID, 0x00, CIM_SUBSTRATE_LOC_ID);
	pCmb->GetLBText(0, tzLocID);	m_lst_ctl[0].InsertItem(0, tzLocID);	m_lst_ctl[0].SetItemText(0, 1, L"No State");
	pCmb->GetLBText(1, tzLocID);	m_lst_ctl[0].InsertItem(1, tzLocID);	m_lst_ctl[0].SetItemText(1, 1, L"No State");
	pCmb->GetLBText(2, tzLocID);	m_lst_ctl[0].InsertItem(2, tzLocID);	m_lst_ctl[0].SetItemText(2, 1, L"No State");
	pCmb->GetLBText(3, tzLocID);	m_lst_ctl[0].InsertItem(3, tzLocID);	m_lst_ctl[0].SetItemText(3, 1, L"No State");

	/* Substrate Setup 여부 */
	m_lst_ctl[1].InsertColumn(1, L"Location ID", LVCFMT_LEFT, 110);
	m_lst_ctl[1].InsertColumn(2, L"Slot ID", LVCFMT_LEFT, 100);
	m_lst_ctl[1].InsertColumn(3, L"Loc.State", LVCFMT_CENTER, 100);
	m_lst_ctl[1].InsertColumn(4, L"Proc.State", LVCFMT_CENTER, 150);
	m_lst_ctl[1].InsertColumn(5, L"Transfer.State", LVCFMT_CENTER, 120);
	m_lst_ctl[1].InsertColumn(6, L"Reader(ID).State", LVCFMT_CENTER, 160);
	m_lst_ctl[1].SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	((CComboBox*)GetDlgItem(IDC_E90_CMB_PROC_STATE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_E90_CMB_TRANS_STATE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_E90_CMB_READ_STATE))->SetCurSel(0);
}

/*
 desc : 주기적으로 호출되는 함수
 parm : None
 retn : None
*/
VOID CDlgE90::UpdateCtrl()
{
	BOOL bEnable		= TRUE;
	INT32 i	= 0;
	TCHAR tzMesg[512]	= {NULL}, tzCarrID[CIM_CARRIER_ID_SIZE]	= {NULL};
	CEdit *pEdit		= {NULL};

}

/*
 desc : 이벤트 정보가 수신된 경우 (부모에 의해서 호출됨)
 parm : event	- [in]  이벤트 정보가 저장된 구조체 포인터
 retn : None
*/
VOID CDlgE90::UpdateCtrl(STG_CCED *event)
{
	CEdit *pEdit= NULL;
#if 0
	switch (event->exx_id)
	{
	case en_essc_e87_cms	:
		switch (event->cbf_id)
		{
		default			: ;	break;
		}
		break;
	}
#endif
}

void CDlgE90::OnBnClickedE90BtnRemoteCtrlGet()
{
	BOOL bEnable	= FALSE;
	TCHAR tzTxt[2][32]	= { L"Disable", L"Enable" };

	CEdit *pEdit	= (CEdit*)GetDlgItem(IDC_E90_EDT_REMOTE_CTRL);
	if (!uvCIMLib_E90STS_GetAllowRemoteControl(bEnable))	return;
	pEdit->SetWindowText(tzTxt[bEnable]);
}

void CDlgE90::OnBnClickedE90BtnRemoteCtrlEnable()
{
	uvCIMLib_E90STS_SetAllowRemoteControl(TRUE);
}
void CDlgE90::OnBnClickedE90BtnRemoteCtrlDisable()
{
	uvCIMLib_E90STS_SetAllowRemoteControl(FALSE);
}

VOID CDlgE90::UpdateSubstrateLocID()
{
	BOOL bSucc	= FALSE;
	INT32 i	= 0, j = 0, i32Item;
	TCHAR tzSubLoc[CIM_SUBSTRATE_LOC_ID];
	E90_SSLS enLocState;
	E90_SSPS enProcState;
	E90_SSTS enTransState;
	E90_SISM enReadState;
	CStringArray arrPRJobID;
	CStringArray arrSlotID;

	/* Update to Substrate ID */
	m_lst_ctl[1].SetRedraw(FALSE);
	m_lst_ctl[1].DeleteAllItems();
#if 0
	m_lst_ctl[1].InsertColumn(1, L"Location ID", LVCFMT_LEFT, 110);
	m_lst_ctl[1].InsertColumn(2, L"Slot ID", LVCFMT_LEFT, 100);
	m_lst_ctl[1].InsertColumn(3, L"State", LVCFMT_CENTER, 90);
#endif
	bSucc	= uvCIMLib_E40PJM_GetProcessAllJobId(arrPRJobID);
	for (i=0; bSucc && i<arrPRJobID.GetCount(); i++)
	{
		if (uvCIMLib_E40PJM_GetPRJobToAllSlotID(arrPRJobID[i].GetBuffer(), arrSlotID))
		{
			for (j=0; j<arrSlotID.GetCount(); j++)
			{
				wmemset(tzSubLoc, 0x00, CIM_SUBSTRATE_LOC_ID);
				if (uvCIMLib_E90STS_GetSubstrateLocationID(arrSlotID[j].GetBuffer(),
															tzSubLoc, CIM_SUBSTRATE_LOC_ID))
				{
					i32Item	= m_lst_ctl[1].GetItemCount();
					m_lst_ctl[1].InsertItem(i32Item, tzSubLoc);
					m_lst_ctl[1].SetItemText(i32Item, 1, arrSlotID[j].GetBuffer());
					/* Location State */
					enLocState	= E90_SSLS::en_no_state;
					if (uvCIMLib_E90STS_GetSubstrateLocationStateName(tzSubLoc, enLocState))
					{
						switch (enLocState)
						{
						case E90_SSLS::en_no_state	:	m_lst_ctl[1].SetItemText(i32Item, 2, L"No State");		break;
						case E90_SSLS::en_occupied	:	m_lst_ctl[1].SetItemText(i32Item, 2, L"Occupied");		break;
						case E90_SSLS::en_unoccupied:	m_lst_ctl[1].SetItemText(i32Item, 2, L"Unoccupied");	break;
						}
					}

					/* Processing State */
					enProcState	= E90_SSPS::en_no_state;
					if (uvCIMLib_E90STS_GetSubstrateProcessingState(arrSlotID[j].GetBuffer(),enProcState))
					{
						switch (enProcState)
						{
						case E90_SSPS::en_no_state			:	m_lst_ctl[1].SetItemText(i32Item, 3, L"no_state");			break;
						case E90_SSPS::en_needs_processing	:	m_lst_ctl[1].SetItemText(i32Item, 3, L"needs_processing");	break;
						case E90_SSPS::en_in_process		:	m_lst_ctl[1].SetItemText(i32Item, 3, L"in_process");		break;
						case E90_SSPS::en_processed			:	m_lst_ctl[1].SetItemText(i32Item, 3, L"processed");			break;
						case E90_SSPS::en_aborted			:	m_lst_ctl[1].SetItemText(i32Item, 3, L"aborted");			break;
						case E90_SSPS::en_stopped			:	m_lst_ctl[1].SetItemText(i32Item, 3, L"stopped");			break;
						case E90_SSPS::en_rejected			:	m_lst_ctl[1].SetItemText(i32Item, 3, L"rejected");			break;
						case E90_SSPS::en_lost				:	m_lst_ctl[1].SetItemText(i32Item, 3, L"lost");				break;
						case E90_SSPS::en_skipped			:	m_lst_ctl[1].SetItemText(i32Item, 3, L"skipped");			break;
						}
					}

					/* Transfer State */
					enTransState	= E90_SSTS::en_no_state;
					if (uvCIMLib_E90STS_GetSubstrateTransportStateName(arrSlotID[j].GetBuffer(),enTransState))
					{
						switch (enTransState)
						{
						case E90_SSTS::en_no_state		:	m_lst_ctl[1].SetItemText(i32Item, 4, L"no_state");		break;
						case E90_SSTS::en_source		:	m_lst_ctl[1].SetItemText(i32Item, 4, L"source");		break;
						case E90_SSTS::en_work			:	m_lst_ctl[1].SetItemText(i32Item, 4, L"work");			break;
						case E90_SSTS::en_destination	:	m_lst_ctl[1].SetItemText(i32Item, 4, L"destination");	break;
						}
					}

					/* Read State */
					enReadState	= E90_SISM::en_no_state;
					if (uvCIMLib_E90STS_GetSubstrateIDStatus(arrSlotID[j].GetBuffer(),enReadState))
					{
						switch (enReadState)
						{
						case E90_SISM::en_no_state				:	m_lst_ctl[1].SetItemText(i32Item, 5, L"no_state");				break;
						case E90_SISM::en_not_confirmed			:	m_lst_ctl[1].SetItemText(i32Item, 5, L"not_confirmed");			break;
						case E90_SISM::en_waiting_for_host		:	m_lst_ctl[1].SetItemText(i32Item, 5, L"waiting_for_host");		break;
						case E90_SISM::en_confirmed				:	m_lst_ctl[1].SetItemText(i32Item, 5, L"confirmed");				break;
						case E90_SISM::en_confirmation_failed	:	m_lst_ctl[1].SetItemText(i32Item, 5, L"confirmation_failed");	break;
						}
					}
				}
			}
			arrSlotID.RemoveAll();
		}
	}
	arrPRJobID.RemoveAll();

	m_lst_ctl[1].SetRedraw(TRUE);
	m_lst_ctl[1].Invalidate(FALSE);


	m_lst_ctl[0].SetRedraw(FALSE);

	/* Update to Substrate Location */
	for (i=0; bSucc && i<m_lst_ctl[0].GetItemCount(); i++)
	{
		wmemset(tzSubLoc, 0x00, CIM_SUBSTRATE_LOC_ID);
		m_lst_ctl[0].GetItemText(i, 0, tzSubLoc, CIM_SUBSTRATE_LOC_ID);

		if (!uvCIMLib_E90STS_GetSubstrateLocationStateName(tzSubLoc, enLocState))
		{
			m_lst_ctl[0].SetItemText(i, 1, L"No State");
		}
		else
		{
			switch (enLocState)
			{
			case E90_SSLS::en_no_state	:	m_lst_ctl[0].SetItemText(i, 1, L"No State");	break;
			case E90_SSLS::en_occupied	:	m_lst_ctl[0].SetItemText(i, 1, L"Occupied");	break;
			case E90_SSLS::en_unoccupied:	m_lst_ctl[0].SetItemText(i, 1, L"Unoccupied");	break;
			}
		}
	}

	m_lst_ctl[0].SetRedraw(TRUE);
	m_lst_ctl[0].Invalidate(FALSE);
}

void CDlgE90::OnBnClickedE90BtnSubstLocAdd()
{
	TCHAR tzSubstLoc[CIM_SUBSTRATE_LOC_ID]	= {NULL};
	CComboBox* pCmb	= (CComboBox*)GetDlgItem(IDC_E90_CMB_SUBST_LOC);
	pCmb->GetLBText(pCmb->GetCurSel(), tzSubstLoc);
	uvCIMLib_E90STS_SetAddSubstrateLocation(tzSubstLoc);
	UpdateSubstrateLocID();
}
void CDlgE90::OnBnClickedE90BtnSubstLocAdds()
{
	INT32 i = 0;
	TCHAR tzSubstLoc[CIM_SUBSTRATE_LOC_ID]	= {NULL};
	CComboBox* pCmb	= (CComboBox*)GetDlgItem(IDC_E90_CMB_SUBST_LOC);
	for (; i<pCmb->GetCount(); i++)
	{
		wmemset(tzSubstLoc, 0x00, CIM_SUBSTRATE_LOC_ID);
		pCmb->GetLBText(i, tzSubstLoc);
		uvCIMLib_E90STS_SetAddSubstrateLocation(tzSubstLoc);
	}	
	UpdateSubstrateLocID();
}

void CDlgE90::OnBnClickedE90BtnSubstLocDel()
{
	TCHAR tzSubstLoc[CIM_SUBSTRATE_LOC_ID]	= {NULL};
	CComboBox* pCmb	= (CComboBox*)GetDlgItem(IDC_E90_CMB_SUBST_LOC);
	pCmb->GetLBText(pCmb->GetCurSel(), tzSubstLoc);
	uvCIMLib_E90STS_SetRemoveSubstrateLocation(tzSubstLoc);
	UpdateSubstrateLocID();
}void CDlgE90::OnBnClickedE90BtnSubstLocDels()
{
	INT32 i = 0;
	TCHAR tzSubstLoc[CIM_SUBSTRATE_LOC_ID]	= {NULL};
	CComboBox* pCmb	= (CComboBox*)GetDlgItem(IDC_E90_CMB_SUBST_LOC);
	for (; i<pCmb->GetCount(); i++)
	{
		wmemset(tzSubstLoc, 0x00, CIM_SUBSTRATE_LOC_ID);
		pCmb->GetLBText(i, tzSubstLoc);
		uvCIMLib_E90STS_SetRemoveSubstrateLocation(tzSubstLoc);
	}
	UpdateSubstrateLocID();
}


void CDlgE90::OnBnClickedE90BtnSubstIdAdd()
{
	INT32 i	= 0, i32Count = 0;
	TCHAR tzSubstLoc[CIM_SUBSTRATE_LOC_ID]	= {NULL};
	TCHAR tzSubstID[CIM_SUBSTRATE_ID]		= {NULL};
	TCHAR tzSubstCount[32]	= {NULL};
	TCHAR tzSubstIDName[128]	= {NULL};
	CComboBox* pCmb	= (CComboBox*)GetDlgItem(IDC_E90_CMB_SUBST_LOC);
	pCmb->GetLBText(pCmb->GetCurSel(), tzSubstLoc);
	((CEdit*)GetDlgItem(IDC_E90_EDT_SUBST_ID))->GetWindowText(tzSubstID, CIM_SUBSTRATE_LOC_ID);
	((CEdit*)GetDlgItem(IDC_E90_EDT_SUBST_COUNT))->GetWindowText(tzSubstCount, 32);
	i32Count	= _wtoi(tzSubstCount);
	if (i32Count > 25)	i32Count	= 25;
	for (i=0; i<i32Count; i++)
	{
		wmemset(tzSubstIDName, 128, 0x00);
		swprintf_s(tzSubstIDName, 128, L"%s_%02d", tzSubstID, i+1);
		uvCIMLib_E90STS_RegisterSubstrateName(tzSubstIDName, tzSubstLoc);
	}
	UpdateSubstrateLocID();
}
void CDlgE90::OnBnClickedE90BtnSubstIdDel()
{
	TCHAR tzSubstID[CIM_SUBSTRATE_ID]	= {NULL};
	((CEdit*)GetDlgItem(IDC_E90_EDT_SUBST_ID))->GetWindowText(tzSubstID, CIM_SUBSTRATE_LOC_ID);
	if (wcslen(tzSubstID))	uvCIMLib_E90STS_SetRemoveSubstrate(tzSubstID);
	else
	{
		INT32 i, i32SelLst	= -1;
		for (i=0; i<m_lst_ctl[1].GetItemCount(); i++)
		{
			if (LVIS_SELECTED == (LVIS_SELECTED & m_lst_ctl[1].GetItemState(i, LVIS_SELECTED)))
			{
				i32SelLst	= i;
				break;
			}
		}
		m_lst_ctl[1].GetItemText(i32SelLst, 1, tzSubstID, CIM_SUBSTRATE_ID);
		if (wcslen(tzSubstID))
		{
			uvCIMLib_E90STS_SetRemoveSubstrate(tzSubstID);
		}
	}

	UpdateSubstrateLocID();
}


void CDlgE90::OnBnClickedE90BtnSubstLocUpdate()
{
	UpdateSubstrateLocID();
}
void CDlgE90::OnBnClickedE90BtnSubstIdUpdate()
{
	UpdateSubstrateLocID();
}

void CDlgE90::OnBnClickedE90BtnSubstLocState()
{
	INT32 i, i32SelPos	= -1, i32State = -1;

	CComboBox *pCmb	= (CComboBox*)GetDlgItem(IDC_E90_CMB_SUBST_LOC_STATE);

	for (i=0; i<m_lst_ctl[0].GetItemCount(); i++)
	{
		if (LVIS_SELECTED == (LVIS_SELECTED & m_lst_ctl[0].GetItemState(i, LVIS_SELECTED)))
		{
			i32SelPos	= i;
			break;
		}
	}
	if (i == m_lst_ctl[0].GetItemCount())	return;

	/* 현재 선택된 LocationID 값으로 콤보 상자에 있는 값으로 변경 */
	i32State	= pCmb->GetCurSel();
}

void CDlgE90::OnBnClickedE90BtnSubstLocStates()
{
	INT32 i32Value[4]	= {NULL};
	INT32 i, i32SelLst = -1;
	TCHAR tzSubstID[CIM_SUBSTRATE_ID]	= {NULL};
	TCHAR tzLocID[CIM_SUBSTRATE_LOC_ID]	= {NULL};
	/* Get random value for Processing State */
	i32Value[0]	= uvCmn_GetRandRangeNumer(INT32(E90_SSPS::en_no_state), INT32(E90_SSPS::en_skipped));
	/* Get random value for Reader State */
	i32Value[1]	= uvCmn_GetRandRangeNumer(INT32(E90_SISM::en_no_state), INT32(E90_SISM::en_confirmation_failed));
	/* Get random value for Transport State */
	i32Value[2]	= uvCmn_GetRandRangeNumer(INT32(E90_SSTS::en_no_state), INT32(E90_SSTS::en_destination));
	/* Get random value for Processing State */
	i32Value[3]	= uvCmn_GetRandRangeNumer(INT32(0), INT32(3));
	m_lst_ctl[0].GetItemText(i32Value[3], 0, tzLocID, CIM_SUBSTRATE_LOC_ID);

	for (i=0; i<m_lst_ctl[1].GetItemCount(); i++)
	{
		if (LVIS_SELECTED == (LVIS_SELECTED & m_lst_ctl[1].GetItemState(i, LVIS_SELECTED)))
		{
			i32SelLst	= i;
			break;
		}
	}
	m_lst_ctl[1].GetItemText(i32SelLst, 1, tzSubstID, CIM_SUBSTRATE_ID);

	uvCIMLib_E90STS_SetSubstrateAnyLocStates(tzSubstID, tzLocID,
											 E90_SISM(i32Value[1]),
											 E90_SSPS(i32Value[0]),
											 E90_SSTS(i32Value[2]));
	UpdateSubstrateLocID();
}

void CDlgE90::OnBnClickedE90BtnSubstIdProc()
{
	TCHAR tzSubstID[CIM_SUBSTRATE_ID]	= {NULL};
	INT32 i, i32SelLst = -1, i32SelCmb	= ((CComboBox*)GetDlgItem(IDC_E90_CMB_PROC_STATE))->GetCurSel();
	if (i32SelCmb < 0)	return;

	for (i=0; i<m_lst_ctl[1].GetItemCount(); i++)
	{
		if (LVIS_SELECTED == (LVIS_SELECTED & m_lst_ctl[1].GetItemState(i, LVIS_SELECTED)))
		{
			i32SelLst	= i;
			break;
		}
	}
	if (i32SelLst < 0)	return;
	i32SelCmb	-= 1;
	m_lst_ctl[1].GetItemText(i32SelLst, 1, tzSubstID, CIM_SUBSTRATE_ID);
	uvCIMLib_E90STS_SetChangeSubstrateProcessingState(tzSubstID, E90_SSPS(i32SelCmb));
	UpdateSubstrateLocID();
}

void CDlgE90::OnBnClickedE90BtnSubstIdTrans()
{
	TCHAR tzSubstID[CIM_SUBSTRATE_ID]	= {NULL};
	INT32 i, i32SelLst = -1, i32SelCmb	= ((CComboBox*)GetDlgItem(IDC_E90_CMB_TRANS_STATE))->GetCurSel();
	if (i32SelCmb < 0)	return;

	for (i=0; i<m_lst_ctl[1].GetItemCount(); i++)
	{
		if (LVIS_SELECTED == (LVIS_SELECTED & m_lst_ctl[1].GetItemState(i, LVIS_SELECTED)))
		{
			i32SelLst	= i;
			break;
		}
	}
	if (i32SelLst < 0)	return;
	i32SelCmb	-= 1;
	m_lst_ctl[1].GetItemText(i32SelLst, 1, tzSubstID, CIM_SUBSTRATE_ID);
	uvCIMLib_E90STS_SetSubstrateTransportState(tzSubstID, E90_SSTS(i32SelCmb));
	UpdateSubstrateLocID();
}

void CDlgE90::OnBnClickedE90BtnSubstIdRead()
{
	TCHAR tzSubstID[CIM_SUBSTRATE_ID]	= {NULL};
	INT32 i, i32SelLst = -1, i32SelCmb	= ((CComboBox*)GetDlgItem(IDC_E90_CMB_READ_STATE))->GetCurSel();
	if (i32SelCmb < 0)	return;

	for (i=0; i<m_lst_ctl[1].GetItemCount(); i++)
	{
		if (LVIS_SELECTED == (LVIS_SELECTED & m_lst_ctl[1].GetItemState(i, LVIS_SELECTED)))
		{
			i32SelLst	= i;
			break;
		}
	}
	if (i32SelLst < 0)	return;
	i32SelCmb	-= 1;
	m_lst_ctl[1].GetItemText(i32SelLst, 1, tzSubstID, CIM_SUBSTRATE_ID);
	uvCIMLib_E90STS_SetSubstrateIDStatus(tzSubstID, E90_SISM(i32SelCmb));
	UpdateSubstrateLocID();
}
