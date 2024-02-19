
/*
 desc : CIM300
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgPJM.h"


IMPLEMENT_DYNAMIC(CDlgPJM, CDialogEx)

/*
 desc : 생성자
 parm : None
 retn : None
*/
CDlgPJM::CDlgPJM(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgPJM::IDD, pParent)
{
	m_bExitProc	= FALSE;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgPJM::~CDlgPJM()
{
}

/* 컨트롤 연결 */
void CDlgPJM::DoDataExchange(CDataExchange* dx)
{
	CDialogEx::DoDataExchange(dx);

}

/* 메시지 맵 */
BEGIN_MESSAGE_MAP(CDlgPJM, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_PJM_BTN_PJ_STATE_UPDATE, &CDlgPJM::OnBnClickedPjmBtnPjStateUpdate)
	ON_BN_CLICKED(IDC_PJM_BTN_PJ_LIST_GET, &CDlgPJM::OnBnClickedPjmBtnPjListGet)
END_MESSAGE_MAP()

/*
 desc : 다이얼로그 생성 후 초기화
*/
BOOL CDlgPJM::OnInitDialog()
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
VOID CDlgPJM::OnClose()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnClose();
}
VOID CDlgPJM::OnDestroy()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnDestroy();
}
VOID CDlgPJM::ExitProc()
{
	/* 이미 이전에 호출된적이 있음 */
	m_bExitProc	= TRUE;

}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgPJM::InitCtrl()
{
}

/*
 desc : 주기적으로 호출되는 함수
 parm : None
 retn : None
*/
VOID CDlgPJM::UpdateCtrl()
{

}

/*
 desc : 이벤트 정보가 수신된 경우 (부모에 의해서 호출됨)
 parm : event	- [in]  이벤트 정보가 저장된 구조체 포인터
 retn : None
*/
VOID CDlgPJM::UpdateCtrl(STG_CCED *event)
{

}


VOID CDlgPJM::OnBnClickedPjmBtnPjStateUpdate()
{
	TCHAR tzPRJobID[CIM_PROCESS_JOB_ID_SIZE]	= {NULL};
	TCHAR tzState[256]	= {NULL};
	CListBox *pBoxPRJob = (CListBox*)GetDlgItem(IDC_PJM_BOX_PJ_LIST);
	CListBox *pBoxSubst = (CListBox*)GetDlgItem(IDC_PJM_BOX_PJ_STATE_UPDATE);
	CAtlList <STG_CSDI> lstState;
	STG_CSDI stState;

	INT32 i32Sel	= pBoxPRJob->GetCurSel();
	if (i32Sel < 0)	return;
	pBoxPRJob->GetText(i32Sel, tzPRJobID);
	if (wcslen(tzPRJobID) < 1)	return;

	pBoxSubst->SetRedraw(FALSE);
	pBoxSubst->ResetContent();

	uvCIMLib_GetSubstIDStateAll(tzPRJobID, lstState);
	for (INT32 i=0; i<lstState.GetCount(); i++)
	{
		stState	= lstState.GetAt(lstState.FindIndex(i));
		swprintf_s(tzState, 256, L"PRJobID:%s.CarrierID:%s.Process:%d.Transport:%d",
				   tzPRJobID, stState.carr_id, stState.proc_state, stState.trans_state);
	}
	lstState.RemoveAll();

	pBoxSubst->SetRedraw(TRUE);
	pBoxSubst->Invalidate(TRUE);
}

VOID CDlgPJM::OnBnClickedPjmBtnPjListGet()
{
	INT32 i	= 0;
	CListBox *pBox = (CListBox*)GetDlgItem(IDC_PJM_BOX_PJ_LIST);
	CStringArray arrPRJobID;

	pBox->SetRedraw(FALSE);
	pBox->ResetContent();

	if (uvCIMLib_E40PJM_GetProcessAllJobId(arrPRJobID))
	{
		for (; i<arrPRJobID.GetCount(); i++)	pBox->AddString(arrPRJobID[i].GetBuffer());
	}

	pBox->SetRedraw(TRUE);
	pBox->Invalidate(TRUE);
}
