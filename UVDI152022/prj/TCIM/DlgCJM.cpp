
/*
 desc : CIM300
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgCJM.h"


IMPLEMENT_DYNAMIC(CDlgCJM, CDialogEx)

/*
 desc : 생성자
 parm : None
 retn : None
*/
CDlgCJM::CDlgCJM(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgCJM::IDD, pParent)
{
	m_bExitProc	= FALSE;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgCJM::~CDlgCJM()
{
}

/* 컨트롤 연결 */
void CDlgCJM::DoDataExchange(CDataExchange* dx)
{
	CDialogEx::DoDataExchange(dx);

	UINT32 i, u32StartID;

	u32StartID	= IDC_CJM_LST_CJSTATUS;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_lst_ctl[i]);

}

/* 메시지 맵 */
BEGIN_MESSAGE_MAP(CDlgCJM, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CJM_BTN_CLEAR_SELECTION, &CDlgCJM::OnBnClickedCjmBtnClearSelection)
	ON_BN_CLICKED(IDC_CJM_BTN_START, &CDlgCJM::OnBnClickedCjmBtnStart)
	ON_BN_CLICKED(IDC_CJM_BTN_STOP, &CDlgCJM::OnBnClickedCjmBtnStop)
	ON_BN_CLICKED(IDC_CJM_BTN_ABORT, &CDlgCJM::OnBnClickedCjmBtnAbort)
	ON_BN_CLICKED(IDC_CJM_BTN_PAUSE, &CDlgCJM::OnBnClickedCjmBtnPause)
	ON_BN_CLICKED(IDC_CJM_BTN_RESUME, &CDlgCJM::OnBnClickedCjmBtnResume)
	ON_BN_CLICKED(IDC_CJM_BTN_SELECT, &CDlgCJM::OnBnClickedCjmBtnSelect)
	ON_BN_CLICKED(IDC_CJM_BTN_DESELECT, &CDlgCJM::OnBnClickedCjmBtnDeselect)
	ON_BN_CLICKED(IDC_CJM_BTN_HOQ, &CDlgCJM::OnBnClickedCjmBtnHoq)
	ON_BN_CLICKED(IDC_CJM_BTN_CANCEL, &CDlgCJM::OnBnClickedCjmBtnCancel)
	ON_NOTIFY(NM_CLICK, IDC_CJM_LST_CJSTATUS, &CDlgCJM::OnNMClickCjmLstCjstatus)
	ON_BN_CLICKED(IDC_CJM_BTN_RUN, &CDlgCJM::OnBnClickedCjmBtnRun)
	ON_BN_CLICKED(IDC_CJM_BTN_UPDATE, &CDlgCJM::OnBnClickedCjmBtnUpdate)
END_MESSAGE_MAP()

/*
 desc : 다이얼로그 생성 후 초기화
*/
BOOL CDlgCJM::OnInitDialog()
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
VOID CDlgCJM::OnClose()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnClose();
}
VOID CDlgCJM::OnDestroy()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnDestroy();
}
VOID CDlgCJM::ExitProc()
{
	/* 이미 이전에 호출된적이 있음 */
	m_bExitProc	= TRUE;

}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgCJM::InitCtrl()
{
	/* Control Job IDs */
	m_lst_ctl[0].InsertColumn(1, L"Sel",				LVCFMT_CENTER, 30);	//  for checkbox 
	m_lst_ctl[0].InsertColumn(2, L"No",					LVCFMT_CENTER, 30);
	m_lst_ctl[0].InsertColumn(3, L"Id",					LVCFMT_CENTER, 100);
	m_lst_ctl[0].InsertColumn(4, L"State",				LVCFMT_CENTER, 100);
	m_lst_ctl[0].InsertColumn(5, L"CarrierInputSpec",	LVCFMT_CENTER, 100);
	m_lst_ctl[0].InsertColumn(6, L"ProcessJobs",		LVCFMT_CENTER, 100);
	m_lst_ctl[0].InsertColumn(7, L"AutoStart",			LVCFMT_CENTER, 100);
	m_lst_ctl[0].InsertColumn(8, L"Completed Time",		LVCFMT_CENTER, 100);
	m_lst_ctl[0].InsertColumn(9, L"Material Arrived",	LVCFMT_CENTER, 100);

	m_lst_ctl[0].SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES |
								  LVS_EX_SINGLEROW | LVS_EX_SIMPLESELECT | LVS_EX_ONECLICKACTIVATE);
}

/*
 desc : 주기적으로 호출되는 함수
 parm : None
 retn : None
*/
VOID CDlgCJM::UpdateCtrl()
{

}

/*
 desc : 이벤트 정보가 수신된 경우 (부모에 의해서 호출됨)
 parm : event	- [in]  이벤트 정보가 저장된 구조체 포인터
 retn : None
*/
VOID CDlgCJM::UpdateCtrl(STG_CCED *event)
{
	if ((E87_CEFI)event->cbf_id == E87_CEFI::en_slot_map_verified)
	{
		uvCIMLib_Mixed_UpdateMtrlOutSpecSubstrateDestinationByE87(event->str_data1);
		return;
	}
	else if ((E94_CEFI)event->cbf_id == E94_CEFI::en_attr_change_noti)
	{
		uvCIMLib_Mixed_UpdateMtrlOutSpecSubstrateDestinationAll();
		return;
	}

	// Anderson 0515: cj state change
	UNG_CCED evt_data = event->evt_data;
	E94_CCJS state1 = (E94_CCJS) (&evt_data)->e94_sc.state;
	PTCHAR cj_id = event->str_data1;
	if (state1 == E94_CCJS::en_paused)
	{
		uvCIMLib_Mixed_PauseProcessJobs(cj_id);
	}
	else if (state1 == E94_CCJS::en_executing)
	{
		uvCIMLib_Mixed_ResumePausedProcessJobs(cj_id);
	}
	else if (state1 == E94_CCJS::en_queued)
	{
		uvCIMLib_Mixed_UpdateMtrlOutSpecSubstrateDestination(cj_id);
	}

	/* ControlJob ID 전체 획득 */
	CStringArray arrCtrlJobIDs;			// ControlJob IDs
	E94_CLCJ filter = E94_CLCJ::en_list_all;
	if (uvCIMLib_E94CJM_GetControlJobListId(arrCtrlJobIDs, filter))
	{
	}

	m_lst_ctl[0].SetRedraw(FALSE);

	// row count
	int nowCount = m_lst_ctl[0].GetItemCount();
	int tobeCount = (int) arrCtrlJobIDs.GetCount();
	if (nowCount > tobeCount)
	{
		int differ = nowCount - tobeCount;
		int start = nowCount - 1;
		for (int row = start; row >= tobeCount; row--)
		{
			m_lst_ctl[0].DeleteItem(row);
		}
	}
	else if (nowCount < tobeCount)
	{
		int differ = tobeCount - nowCount;
		for (int row = nowCount; row < tobeCount; row++)
		{
			m_lst_ctl[0].InsertItem(row, L"");
		}
	}

	// get data and display
	for (int row = 0; row < arrCtrlJobIDs.GetCount(); row++)
	{
		// id
		PTCHAR cjId = arrCtrlJobIDs[row].GetBuffer(0);
		m_lst_ctl[0].SetItemText(row, (INT32)E94_COL::Id, cjId);

		// ControlJob State 얻기
		E94_CCJS state;
		if (uvCIMLib_E94CJM_GetControlJobState(cjId, state))
		{
		}
		CString tempState = E94_CCJS_ToString(state);
		LPCTSTR txtState = (LPCTSTR)tempState;
		m_lst_ctl[0].SetItemText(row, (INT32)E94_COL::State, txtState);

		// Completed Time
		TCHAR szDateTime[32];		// 실제 출력할떄 mm:dd:yy-hh:mm:ss 형태로 바꿀것
		if (uvCIMLib_E94CJM_GetControlJobCompletedTime(cjId, szDateTime, 32))
		{
			m_lst_ctl[0].SetItemText(row, (INT32)E94_COL::CompletedTime, szDateTime);
		}

		// material arrived
		if (state != E94_CCJS::en_completed)
		{
			BOOL bArrived;
			if (uvCIMLib_E94CJM_GetControlJobMaterialArrived(cjId, bArrived))
			{
				LPCTSTR txtMaterial = L"N";
				if (bArrived)	txtMaterial = L"Y";
				m_lst_ctl[0].SetItemText(row, (INT32)E94_COL::MaterialArraived, txtMaterial);
			}
		}

		// Carrier Input Spec (can be empty)
		// source carrier (+destination carrier if not uni-carrier
		CStringList carrierList;
		if (uvCIMLib_E94CJM_GetControlJobCarrierInputSpec(cjId, carrierList))
		{
			CString tempC = GetStringFromList(carrierList);
			LPCTSTR txtCarrier = (LPCTSTR)tempC;
			m_lst_ctl[0].SetItemText(row, (INT32)E94_COL::CarrierInputSpec, txtCarrier);
		}

		// PRJobStatusList
		CStringList pjList;
		if (uvCIMLib_E94CJM_GetControlJobProcessJobList(cjId, pjList))
		{
			CString tempP = GetStringFromList(pjList);
			LPCTSTR txtPjList = (LPCTSTR)tempP;
			m_lst_ctl[0].SetItemText(row, (INT32)E94_COL::ProcessJobs, txtPjList);
		}

		// StartMethod
		BOOL bAutoStart = FALSE;
		if (uvCIMLib_E94CJM_GetControlJobStartMethod(cjId, bAutoStart))
		{
			LPCTSTR txtAutoStart = L"N";
			if (bAutoStart == TRUE)	txtAutoStart = L"Y";
			m_lst_ctl[0].SetItemText(row, (INT32)E94_COL::AutoStart, txtAutoStart);
		}
	}

	// row number
	int count = m_lst_ctl[0].GetItemCount();
	for (int row = 0; row < count; row++)
	{
		int a = row + 1;
		CString b;
		b.Format(_T("%d"), a);
		LPCTSTR c = (LPCTSTR)b;
		m_lst_ctl[0].SetItemText(row, (INT32)E94_COL::No, c);
	}

	m_lst_ctl[0].SetRedraw(TRUE);
	m_lst_ctl[0].Invalidate(FALSE);

	UpdateCJAction();
}

CString CDlgCJM::GetStringFromList(CStringList & listValue)
{
	CString ret;
	POSITION pos = listValue.GetHeadPosition();
	while (pos)
	{
		CString one = listValue.GetNext(pos);
		ret += one;
		ret += L",";
	}
	return ret;
}


INT32 CDlgCJM::GetSelectedRow()
{
	INT32 ret = -1;
	for (INT32 row = 0; row < m_lst_ctl[0].GetItemCount(); row++)
	{
		if (IsSelected(row) == TRUE)
		{
			ret = row;
			break;
		}
	}
	return ret;
}

BOOL CDlgCJM::IsSelected(INT32 row)
{
	BOOL ret = m_lst_ctl[0].GetCheck(row);
	return ret;
}

E94_CCJS CDlgCJM::GetState(INT32 row)
{
	CString txt = m_lst_ctl[0].GetItemText(row, (INT32)E94_COL::State);
	E94_CCJS ret = ToStateEnum(txt);
	return ret;
}

E94_CCJS CDlgCJM::ToStateEnum(CString text)
{
	E94_CCJS ret	= E94_CCJS::en_unknown;

	if (text == "")					ret = E94_CCJS::en_unknown;
	if (text == "QUEUED")			ret = E94_CCJS::en_queued;
	if (text == "SELECTED")			ret = E94_CCJS::en_selected;
	if (text == "WAITINGFORSTART")	ret = E94_CCJS::en_waitingforstart;
	if (text == "EXECUTING")		ret = E94_CCJS::en_executing;
	if (text == "PAUSED")			ret = E94_CCJS::en_paused;
	if (text == "COMPLETED")		ret = E94_CCJS::en_completed;

	return ret;
}

CString CDlgCJM::E94_CCJS_ToString(E94_CCJS state)
{
	CString result;

	switch (state)
	{
	case E94_CCJS::en_unknown:			result = "";				break;
	case E94_CCJS::en_queued:			result = "QUEUED";			break;
	case E94_CCJS::en_selected:			result = "SELECTED";		break;
	case E94_CCJS::en_waitingforstart:	result = "WAITINGFORSTART";	break;
	case E94_CCJS::en_executing:		result = "EXECUTING";		break;
	case E94_CCJS::en_paused:			result = "PAUSED";			break;
	case E94_CCJS::en_completed:		result = "COMPLETED";		break;
	}

	return result;
}


VOID CDlgCJM::UpdateCJAction()
{

	/*

	HOQ				E94_CCJS::en_queued - QUEUD 인 ControlJob 2개 이상일경우 하위 ControlJob 들에 대해서만 사용가능, 첫번째 QUEUE 인 ControlJob 엔 사용 불가

	Start		Stop		Abort		Pause		Resume		Cancel		Select		Deselect		HOQ
	E94_CCJS::en_unknown:
	E94_CCJS::en_queued:								O			O									O			O							O
	E94_CCJS::en_selected:								O			O															O
	E94_CCJS::en_waitingforstart:		O				O			O
	E94_CCJS::en_executing:								O			O			O
	E94_CCJS::en_paused:								O			O						O
	E94_CCJS::en_completed:
	*/

	// 버튼 컨트롤 가져오기
	CWnd *p_wndButtonStart;
	CWnd *p_wndButtonStop;
	CWnd *p_wndButtonAbort;
	CWnd *p_wndButtonPause;
	CWnd *p_wndButtonResume;
	CWnd *p_wndButtonCancel;
	CWnd *p_wndButtonSelect;
	CWnd *p_wndButtonDeselect;
	CWnd *p_wndButtonHOQ;

	p_wndButtonStart	= GetDlgItem(IDC_CJM_BTN_START);
	p_wndButtonStop		= GetDlgItem(IDC_CJM_BTN_STOP);
	p_wndButtonAbort	= GetDlgItem(IDC_CJM_BTN_ABORT);
	p_wndButtonPause	= GetDlgItem(IDC_CJM_BTN_PAUSE);
	p_wndButtonResume	= GetDlgItem(IDC_CJM_BTN_RESUME);
	p_wndButtonCancel	= GetDlgItem(IDC_CJM_BTN_CANCEL);
	p_wndButtonSelect	= GetDlgItem(IDC_CJM_BTN_SELECT);
	p_wndButtonDeselect	= GetDlgItem(IDC_CJM_BTN_DESELECT);
	p_wndButtonHOQ		= GetDlgItem(IDC_CJM_BTN_HOQ);

	int row = GetSelectedRow();
	if (row >= 0 && row < m_lst_ctl[0].GetItemCount())
	{
		p_wndButtonStart->EnableWindow(FALSE);
		p_wndButtonStop->EnableWindow(FALSE);
		p_wndButtonAbort->EnableWindow(FALSE);
		p_wndButtonPause->EnableWindow(FALSE);
		p_wndButtonResume->EnableWindow(FALSE);
		p_wndButtonCancel->EnableWindow(FALSE);
		p_wndButtonSelect->EnableWindow(FALSE);
		p_wndButtonDeselect->EnableWindow(FALSE);
		p_wndButtonHOQ->EnableWindow(FALSE);

		E94_CCJS state = GetState(row);
		switch (state)
		{
		case E94_CCJS::en_unknown:
			break;
		case E94_CCJS::en_queued:
			p_wndButtonStop->EnableWindow(TRUE);
			p_wndButtonAbort->EnableWindow(TRUE);
			p_wndButtonCancel->EnableWindow(TRUE);
			p_wndButtonSelect->EnableWindow(TRUE);
			p_wndButtonHOQ->EnableWindow(TRUE);
			break;
#pragma region Active
		case E94_CCJS::en_selected:
			p_wndButtonDeselect->EnableWindow(TRUE);
			p_wndButtonStop->EnableWindow(TRUE);
			p_wndButtonAbort->EnableWindow(TRUE);
			break;
		case E94_CCJS::en_waitingforstart:
			p_wndButtonStart->EnableWindow(TRUE);
			p_wndButtonStop->EnableWindow(TRUE);
			p_wndButtonAbort->EnableWindow(TRUE);
			break;
		case E94_CCJS::en_executing:
			p_wndButtonPause->EnableWindow(TRUE);
			p_wndButtonStop->EnableWindow(TRUE);
			p_wndButtonAbort->EnableWindow(TRUE);
			break;
		case E94_CCJS::en_paused:
			p_wndButtonResume->EnableWindow(TRUE);
			p_wndButtonStop->EnableWindow(TRUE);
			p_wndButtonAbort->EnableWindow(TRUE);
			break;
#pragma endregion Active

		case E94_CCJS::en_completed:
			break;
		}
	}
	else
	{
		p_wndButtonStart->EnableWindow(FALSE);
		p_wndButtonStop->EnableWindow(FALSE);
		p_wndButtonAbort->EnableWindow(FALSE);
		p_wndButtonPause->EnableWindow(FALSE);
		p_wndButtonResume->EnableWindow(FALSE);
		p_wndButtonCancel->EnableWindow(FALSE);
		p_wndButtonSelect->EnableWindow(FALSE);
		p_wndButtonDeselect->EnableWindow(FALSE);
		p_wndButtonHOQ->EnableWindow(FALSE);
	}
}

void CDlgCJM::ClearSelection()
{
	for (int i=0; i<m_lst_ctl[0].GetItemCount(); i++)
	{
		m_lst_ctl[0].SetCheck(i, FALSE);
	}
}

void CDlgCJM::OnBnClickedCjmBtnClearSelection()
{
	ClearSelection();
	UpdateCJAction();
}


void CDlgCJM::OnBnClickedCjmBtnStart()
{
	PTCHAR ptCtrlID;
	CString strBuffer;
	int iIndex;

	iIndex = GetSelectedRow();
	if (iIndex >= 0)
	{
		strBuffer = m_lst_ctl[0].GetItemText(iIndex, 2);
		ptCtrlID = strBuffer.GetBuffer(0);

		uvCIMLib_E94CJM_SetStartControlJob(ptCtrlID);
	}
}

void CDlgCJM::OnBnClickedCjmBtnStop()
{
	PTCHAR ptCtrlID = NULL;
	CString strBuffer;
	int iIndex;

	iIndex = GetSelectedRow();
	if (iIndex >= 0)
	{
		strBuffer	= m_lst_ctl[0].GetItemText(iIndex, 2);
		ptCtrlID	= strBuffer.GetBuffer(0);
		uvCIMLib_E94CJM_SetStopControlJob(ptCtrlID, E94_CCJA::en_remove_jobs);
	}
}

void CDlgCJM::OnBnClickedCjmBtnAbort()
{
	PTCHAR ptCtrlID;
	CString strBuffer;
	int iIndex;

	iIndex = GetSelectedRow();
	if (iIndex >= 0)
	{
		strBuffer = m_lst_ctl[0].GetItemText(iIndex, 2);
		ptCtrlID = strBuffer.GetBuffer(0);

		uvCIMLib_E94CJM_SetAbortControlJob(ptCtrlID, E94_CCJA::en_remove_jobs);
	}
}

void CDlgCJM::OnBnClickedCjmBtnPause()
{
	PTCHAR ptCtrlID;
	CString strBuffer;
	int iIndex;

	iIndex = GetSelectedRow();
	if (iIndex >= 0)
	{
		strBuffer = m_lst_ctl[0].GetItemText(iIndex, 2);
		ptCtrlID = strBuffer.GetBuffer(0);

		uvCIMLib_E94CJM_SetPauseControlJob(ptCtrlID);
	}
}

void CDlgCJM::OnBnClickedCjmBtnResume()
{
	PTCHAR ptCtrlID;
	CString strBuffer;
	int iIndex;

	iIndex = GetSelectedRow();
	if (iIndex >= 0)
	{
		strBuffer = m_lst_ctl[0].GetItemText(iIndex, 2);
		ptCtrlID = strBuffer.GetBuffer(0);

		uvCIMLib_E94CJM_SetResumeControlJob(ptCtrlID);
	}
}

void CDlgCJM::OnBnClickedCjmBtnSelect()
{
	PTCHAR ptCtrlID;
	CString strBuffer;
	int iIndex;

	iIndex = GetSelectedRow();
	if (iIndex >= 0)
	{
		strBuffer = m_lst_ctl[0].GetItemText(iIndex, 2);
		ptCtrlID = strBuffer.GetBuffer(0);

		uvCIMLib_E94CJM_SetSelectedControlJob(ptCtrlID);
	}
}

void CDlgCJM::OnBnClickedCjmBtnDeselect()
{
	PTCHAR ptCtrlID;
	CString strBuffer;
	int iIndex;

	iIndex = GetSelectedRow();
	if (iIndex >= 0)
	{
		strBuffer = m_lst_ctl[0].GetItemText(iIndex, 2);
		ptCtrlID = strBuffer.GetBuffer(0);

		uvCIMLib_E94CJM_SetDeselectedControlJob(ptCtrlID);
	}
}

void CDlgCJM::OnBnClickedCjmBtnHoq()
{
	PTCHAR ptCtrlID;
	CString strBuffer;
	int iIndex;

	iIndex = GetSelectedRow();
	if (iIndex >= 0)
	{
		strBuffer = m_lst_ctl[0].GetItemText(iIndex, 2);
		ptCtrlID = strBuffer.GetBuffer(0);

		uvCIMLib_E94CJM_SetHOQControlJob(ptCtrlID);
	}
}

void CDlgCJM::OnBnClickedCjmBtnCancel()
{
	PTCHAR ptCtrlID;
	CString strBuffer;
	int iIndex;

	iIndex = GetSelectedRow();
	if (iIndex >= 0)
	{
		strBuffer = m_lst_ctl[0].GetItemText(iIndex, 2);
		ptCtrlID = strBuffer.GetBuffer(0);

		uvCIMLib_E94CJM_SetCancelControlJob(ptCtrlID, E94_CCJA::en_remove_jobs);
	}
}


void CDlgCJM::OnNMClickCjmLstCjstatus(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// single row select only
	int selectedIndex = -1;
	selectedIndex = m_lst_ctl[0].GetNextItem(-1, LVNI_SELECTED);

	ClearSelection();

	if (selectedIndex >= 0)
	{
		m_lst_ctl[0].SetCheck(selectedIndex, true);
	}

	// ControlJob Item 클릭시 해당 ControlJob State 에 따라 기능 버튼 Enable/Disable
	UpdateCJAction();

	*pResult = 0;
}

void CDlgCJM::OnBnClickedCjmBtnRun()
{
	/// get cj id
	CString strCtrlID;
	int iIndex;

	iIndex = GetSelectedRow();
	if (iIndex >= 0)
	{
		strCtrlID = m_lst_ctl[0].GetItemText(iIndex, 2);
	}

	/// get pj list
	CStringList pjList;
	if (uvCIMLib_E94CJM_GetControlJobProcessJobList(strCtrlID.GetBuffer(), pjList)==FALSE)
	{
		TRACE("FAILED: uvCIMLib_E94CJM_GetProcessjobList");
		return;
	}

	/// for pj 1~n
	POSITION pos1 = pjList.GetHeadPosition();
	while (pos1)
	{
		// get one pj id
		CString one = pjList.GetNext(pos1);

		/// set  - start process
		if (uvCIMLib_E40PJM_SetPRJobStartProcess(one.GetBuffer())==FALSE)
		{
			TRACE("FAILED: uvCIMLib_E40PJM_SetPRJobStartProcess=%s", one.GetBuffer());
			return;
		}

		/// simul processing
//		uvCIMLib_SimulateProcessing(one.GetBuffer());
	}

	// get carrier input spec
	CStringList carrierList;
	if (uvCIMLib_E94CJM_GetControlJobCarrierInputSpec(strCtrlID.GetBuffer(), carrierList) == FALSE)
	{
		TRACE("FAILED: uvCIMLib_E94CJM_GetCarrierInputSpec=%s", strCtrlID.GetBuffer());
		return;
	}

	// close carrier
	POSITION pos2 = carrierList.GetHeadPosition();
	while (pos2)
	{
		CString one = carrierList.GetNext(pos2);
		if (uvCIMLib_E87CMS_SetAccessCarrier(0x00, one.GetBuffer(), 0x01,
											  E87_CCAS::en_carrier_complete) == FALSE)
		{
			TRACE("FAILED: uvCIMLib_Close_Carrier=%s", one);
		}
		else
		{
			uvCIMLib_E87CMS_SetCarrierEvent(0x00, one.GetBuffer(), 0x01, E87_CCES::en_carrier_closed);
		}
	}
}

void CDlgCJM::OnBnClickedCjmBtnUpdate()
{
}
