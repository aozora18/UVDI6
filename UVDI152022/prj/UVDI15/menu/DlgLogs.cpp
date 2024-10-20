
/*
 desc : Log Files & Contents
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgLogs.h"

#include "../mesg/DlgMesg.h"
#include "./logs/GridFile.h"	/* Grid Control */
#include <thread>


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif



#define WM_USER_SYNCSCROLL			(WM_USER + 1)


/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgLogs::CDlgLogs(UINT32 id, CWnd* parent)
	: CDlgMenu(id, parent)
{
	/* 자신의 다이얼로그 ID 설정 */
	m_enDlgID	= ENG_CMDI::en_menu_logs;

	m_pPgr = NULL;

	m_nChecked = 0x01;

	m_eSTEP = eLOGS_STEP_INIT;
	m_bStop = FALSE;
}

CDlgLogs::~CDlgLogs()
{
	StopLogThread();
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgLogs::DoDataExchange(CDataExchange* dx)
{
	CDlgMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	
	/* groupbox - normal */
	u32StartID	= IDC_LOGS_GRP_LIST;
	for (i=0; i< eLOGS_GRP_MAX; i++)		DDX_Control(dx, u32StartID + i,	m_grp_ctl[i]);
	/* Title - Normal */
	u32StartID = IDC_LOGS_TTL_CONTENT1;
	for (i=0; i< eLOGS_TTL_MAX; i++)		DDX_Control(dx, u32StartID + i, m_ttl_ctl[i]);
	/* rich - normal */
	u32StartID	= IDC_LOGS_RCH_CONTENT;
	for (i=0; i< eLOGS_RCH_MAX; i++)		DDX_Control(dx, u32StartID + i,	m_rch_ctl[i]);
	/* checkbox - log_level */
	u32StartID	= IDC_LOGS_CHK_FILT_SERVICE;
	for (i=0; i< eLOGS_TYPE_MAX; i++)		DDX_Control(dx, u32StartID + i,	m_chk_flt[i]);
	/* checkbox - option */
	u32StartID = IDC_LOGS_CHK_VIEW_SYNC_SCROLL;
	for (i=0; i< eLOGS_VOP_MAX; i++)		DDX_Control(dx, u32StartID + i, m_chk_vop[i]);
	/* button - normal */
	u32StartID	= IDC_LOGS_BTN_TODAY;
	for (i=0; i< eLOGS_BTN_MAX; i++)		DDX_Control(dx, u32StartID + i,	m_btn_ctl[i]);
	/* grid - search */
	u32StartID = IDC_LOGS_GRD_SEARCH;
	for (i=0; i< eLOGS_GRD_MAX; i++)		DDX_Control(dx, u32StartID + i, m_grd_ctl[i]);
	/* progress bar - normal */
	u32StartID = IDC_LOGS_PGR_LOADING;
	for (i=0; i< eLOGS_PGR_MAX; i++)		DDX_Control(dx, u32StartID + i, m_pgr_bar[i]);
}

BEGIN_MESSAGE_MAP(CDlgLogs, CDlgMenu)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_LOGS_BTN_TODAY, IDC_LOGS_BTN_TODAY + eLOGS_BTN_MAX, OnBtnClick)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_LOGS_CHK_FILT_SERVICE, IDC_LOGS_CHK_FILT_SERVICE + eLOGS_TYPE_MAX, OnChkClick)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_LOGS_CHK_VIEW_SYNC_SCROLL, IDC_LOGS_CHK_VIEW_SYNC_SCROLL + eLOGS_VOP_MAX, OnOptClick)
	ON_MESSAGE(WM_USER_SYNCSCROLL, &CDlgLogs::OnSyncScroll)
ON_WM_VSCROLL()
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgLogs::PreTranslateMessage(MSG* msg)
{
 	if (m_chk_vop[eLOGS_VOP_SYNC_SCROLL].GetCheck())
 	{
 		if (msg->message == WM_MOUSEWHEEL)
 		{
 			// 휠 메시지를 각 Rich Edit 컨트롤에 전달
 			for (UINT i = 0; i < eLOGS_RCH_MAX; i++)
 			{
 				m_rch_ctl[i].SendMessage(WM_MOUSEWHEEL, msg->wParam, msg->lParam);
 			}
 
 			return TRUE;  // 메시지가 처리되었음을 나타냅니다.
 		}
 	}

	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLogs::OnInitDlg()
{
	/* 컨트롤 초기화 */
	InitCtrl();
	if (!InitGrid())	return FALSE;

	StartLogThread();

	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgLogs::OnExitDlg()
{
	SetStop();
	StopLogThread();

	if (m_pPgr)
	{
		delete m_pPgr;
		m_pPgr = NULL;
	}

	CloseGrid();
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgLogs::OnPaintDlg(CDC *dc)
{

	/* 자식 윈도 호출 */
	CDlgMenu::DrawOutLine();
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgLogs::OnResizeDlg()
{
}

/*
 desc : 부모 스레드에 의해 주기적으로 호출됨
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgLogs::UpdatePeriod(UINT64 tick, BOOL is_busy)
{
	/* Control Enable or Disable */
	UpdateControl(tick, is_busy);
}

/*
 desc : 컨트롤
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgLogs::UpdateControl(UINT64 tick, BOOL is_busy)
{
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgLogs::InitCtrl()
{
	CResizeUI clsResizeUI;

	INT32 i = 0;

	/* group box */
	for (i = 0; i < eLOGS_GRP_MAX; i++)
	{
		m_grp_ctl[i].SetLogFont(&g_lf[eFONT_LEVEL2_BOLD]);

		clsResizeUI.ResizeControl(this, &m_grp_ctl[i]);
	}

	/* checkbox - error_level */
	for (i=0; i< eLOGS_TYPE_MAX; i++)
	{
		m_chk_flt[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_chk_flt[i].SetBgColor(RGB(255, 255, 255));

		clsResizeUI.ResizeControl(this, &m_chk_flt[i]);
	}
	m_chk_flt[eLOGS_TYPE_SERVICE].SetCheck(1);

	/* checkbox - view_option */
	for (i = 0; i < eLOGS_VOP_MAX; i++)
	{
		//m_chk_vop[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		//m_chk_vop[i].SetBgColor(RGB(255, 255, 255));

		clsResizeUI.ResizeControl(this, &m_chk_vop[i]);
	}

	/* button - normal */
	for (i=0; i< eLOGS_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);

		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
	}

	/* static title */
	for (i = 0; i < eLOGS_TTL_MAX; i++)
	{
		m_ttl_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_ttl_ctl[i].SetBaseProp(0, 1, 0, 0, FORD_GRAPHITE, 0, WHITE_);

		clsResizeUI.ResizeControl(this, &m_ttl_ctl[i]);

		m_ttl_ctl[i].GetWindowRect(m_rtOriginTTL[i]);
	}

 	/* rich edit */
 	for (i=0; i< eLOGS_RCH_MAX; i++)
 	{
 		m_rch_ctl[i].SetRichFont(g_lf[eFONT_LEVEL2_BOLD].lfFaceName, g_lf[eFONT_LEVEL2_BOLD].lfWeight);
		m_rch_ctl[i].SetKeyInputEnable(FALSE);

		clsResizeUI.ResizeControl(this, &m_rch_ctl[i]);

		m_rch_ctl[i].GetWindowRect(m_rtOriginRCH[i]);
 	}

	/* grid ctrl */
	for (int i = 0; i < _countof(m_grd_ctl); i++)
	{
		clsResizeUI.ResizeControl(this, &m_grd_ctl[i]);
	}


	/* Progress Bar*/
	clsResizeUI.ResizeControl(this, &m_pgr_bar[eLOGS_PGR_LOADING]);

 	CRect rtPrg;
	m_pgr_bar[eLOGS_PGR_LOADING].GetWindowRect(rtPrg);
 	this->ScreenToClient(rtPrg);
 
 	m_pPgr = new CProgressCtrlX();
	if (m_pPgr->Create(WS_BORDER | WS_VISIBLE, rtPrg, this, IDC_LOGS_PGR_LOADING))
	{
 		m_pPgr->SetBkColor(ALICE_BLUE);
 		m_pPgr->SetTextColor(BLACK_);
 		m_pPgr->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
 		m_pPgr->SetShowPercent(TRUE);
 		m_pPgr->SetRange(0, 100);
	}
	else
	{
		delete m_pPgr;
		m_pPgr = nullptr;
	}

}

/*
 desc : GridControl 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLogs::InitGrid()
{
	CRect rtGrd;
	CGridCtrl* pGrid = &m_grd_ctl[eLOGS_GRD_SEARCH];
	pGrid->GetWindowRect(rtGrd);
	this->ScreenToClient(rtGrd);

	double dCellRatio[2] = { 0.3, 0.7 };	/* 그리드 비율 */
	int nMaxCol = 2;
	int nMaxRow = 2;
	int nHeight = 0;	/* 셀 높이 */
	
	nHeight = rtGrd.Height() / nMaxRow;

	ST_GRID_LOGS_SEARCH	stLogsSearch[2] =
	{
		{ _T("FROM"),			DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS,	(int)((rtGrd.Width() * dCellRatio[0]))},
		{ _T("TO"),				DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS,	(int)((rtGrd.Width() * dCellRatio[1]))},
	};

	pGrid->SetRowCount(nMaxRow);
	pGrid->SetColumnCount(nMaxCol);

	for (int nRow = 0; nRow < nMaxRow; nRow++)
	{
		pGrid->SetRowHeight(nRow, nHeight - 1);

		for (int nCol = 0; nCol < nMaxCol; nCol++)
		{
			pGrid->SetColumnWidth(nCol, stLogsSearch[nCol].nWidth);
			pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS);

			if (0 == nCol)
			{
				pGrid->SetItemState(nRow, nCol, pGrid->GetItemState(nRow, nCol) | GVIS_READONLY);
				pGrid->SetItemText(nRow, nCol, stLogsSearch[nRow].strTitle);
				pGrid->SetItemBkColour(nRow, nCol, ALICE_BLUE);
				pGrid->SetItemFgColour(nRow, nCol, BLACK_);
			}
			else
			{
				pGrid->SetCellType(nRow, nCol, RUNTIME_CLASS(CGridCellDateTime));
				CGridCellDateTime* pGridDate = (CGridCellDateTime*)pGrid->GetCell(nRow, nCol);
				pGridDate->Init(DTS_SHORTDATEFORMAT);
			}
		}
	}

	pGrid->SetColumnResize(FALSE);
	pGrid->SetRowResize(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
	pGrid->Refresh();

	return TRUE;
}

/*
 desc : GridControl 해제
 parm : None
 retn : None
*/
VOID CDlgLogs::CloseGrid()
{
	
}

/*
 desc : GridPartition
 parm : None
 retn : None
*/
VOID CDlgLogs::PartitionObject(int nChecked)
{
	std::array<CWnd*, eLOGS_RCH_MAX> wndTitles = { &m_ttl_ctl[0], &m_ttl_ctl[1], &m_ttl_ctl[2], &m_ttl_ctl[3] };
	std::array<CWnd*, eLOGS_RCH_MAX> wndContents = { &m_rch_ctl[0], &m_rch_ctl[1], &m_rch_ctl[2], &m_rch_ctl[3] };
	std::array<CRect, eLOGS_RCH_MAX> rtTitles = { m_rtOriginTTL[0], m_rtOriginTTL[1], m_rtOriginTTL[2], m_rtOriginTTL[3] };
	std::array<CRect, eLOGS_RCH_MAX> rtContents = { m_rtOriginRCH[0], m_rtOriginRCH[1], m_rtOriginRCH[2], m_rtOriginRCH[3] };

	CResizeUI clsResizeUI;

	clsResizeUI.PartitionTitle(wndTitles, rtTitles, nChecked);
	clsResizeUI.PartitionContent(wndContents, rtContents, nChecked);
}

/*
 desc : 객체 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLogs::InitObject()
{

	return TRUE;
}

/*
 desc : 객체 해제
 parm : None
 retn : None
*/
VOID CDlgLogs::CloseObject()
{
}

/*
 desc : 일반 버튼 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgLogs::OnBtnClick(UINT32 id)
{
	switch (id)
	{
	case IDC_LOGS_BTN_TODAY:		SetGridTextToday();		break;
	case IDC_LOGS_BTN_THIS_WEEK:	SetGridTextThisWeek();	break;
	case IDC_LOGS_BTN_SEARCH:		UpdateList();			break;
	case IDC_LOGS_BTN_STOP:			SetStop();				break;
	}
}

/*
 desc : 체크 버튼 클릭한 경우
 parm : id	- [in]  체크 버튼 ID
 retn : None
*/
VOID CDlgLogs::OnChkClick(UINT32 id)
{
	UINT8 i	= 0x00;
	m_nChecked = 0x00;
	for (; i < eLOGS_TYPE_MAX; i++)
	{
		if (m_chk_flt[i].GetCheck())
		{
			m_nChecked++;
		}
	}
	
	CMacCheckBox* chk = &m_chk_flt[id - IDC_LOGS_CHK_FILT_SERVICE];

	//	4개 이상 선택 불가
	if (eLOGS_RCH_MAX < m_nChecked)
	{
		//	상태 복구
		chk->SetCheck(0);
		return;
	}

	//	화면 갱신 중인지 확인
	if (eLOGS_STEP_INIT != m_eSTEP)
	{
		//	상태 복구
		if (chk->GetCheck())
			chk->SetCheck(0);
		else
			chk->SetCheck(1);

		AfxMessageBox(L"파일에서 내용을 불러오는 중입니다.");
		return;
	}

	

	/* 필터 적용 후 관련 로그만 출력 */
	UpdateList();
}


VOID CDlgLogs::OnOptClick(UINT32 id)
{
	/* 포커스 초기화 */
	InitListFirstLine();
}



/*
 desc : 파일로부터 로그 데이터 얻기
 parm : None
 retn : None
*/
VOID CDlgLogs::GetLogInfo(vST_LOG_MSG* vLogs)
{
	/* 설정한 날짜 얻기 */
	COleDateTime tStart;
	COleDateTime tEnd;
	GetGridTextData(tStart, tEnd);


	/* LOG 파일 불러오기 */
	for (int nType = 0; nType < eLOGS_TYPE_MAX; nType++)
	{
		CLogManager::GetInstance()->InitLog((EN_LOG_TYPE)nType);

		if (m_chk_flt[nType].GetCheck())
		{
			m_eSTEP = eLOGS_STEP_FILE_LOAD;

			CLogManager::GetInstance()->LoadFiles((EN_LOG_TYPE)nType, tStart, tEnd);			
		}
	}


	/* LOG 정보 얻기 */
	BOOL bSync = ( 1 < m_nChecked ) ? TRUE : FALSE;

	for (int nType = 0; nType < eLOGS_TYPE_MAX; nType++)
	{
		if (m_chk_flt[nType].GetCheck())
		{
			vLogs[nType] = CLogManager::GetInstance()->GetLog((EN_LOG_TYPE)nType, bSync);
		}
	}

	/* 스텝 화면에 뿌리기 */
	m_eSTEP = eLOGS_STEP_VIEW_UPDATE;
}



/*
 desc : 로그 데이터를 화면에 뿌리기
 parm : None
 retn : None
*/
VOID CDlgLogs::UpdateList()
{
	/* 화면 초기화 */
	InitTitles();
	InitContents();

	/* 타이틀 화면 표시 */
	UpdateLogTitles();

	//	화면 재배분
	PartitionObject(m_nChecked);

	/* 로그 정보 얻기 */
	GetLogInfo(m_vLogs);
}

/*
 desc : 현재 선택된 파일 삭제 수행
 parm : None
 retn : None
*/
VOID CDlgLogs::DeleteFile()
{
	CDlgMesg dlgMesg;

}

VOID CDlgLogs::SetGridTextToday()
{
	CGridCtrl* pGrid = &m_grd_ctl[eLOGS_GRD_SEARCH];

	CTime time(CTime::GetCurrentTime());

	CGridCellDateTime* pGridDate_From = (CGridCellDateTime*)pGrid->GetCell(0, 1);
	pGridDate_From->Init(DTS_SHORTDATEFORMAT);
	CGridCellDateTime* pGridDate_To = (CGridCellDateTime*)pGrid->GetCell(1, 1);
	pGridDate_To->Init(DTS_SHORTDATEFORMAT);

	pGrid->Refresh();
}

VOID CDlgLogs::SetGridTextThisWeek()
{
	CGridCtrl* pGrid = &m_grd_ctl[eLOGS_GRD_SEARCH];

	CTime time(CTime::GetCurrentTime());
	
	//	컨셉1) 1주일 전 날짜 구하기	
	//CTimeSpan spanWeek(6, 23, 59, 59);

	//	컨셉2) 1주의 시작 날짜 구하기
	int dayOfWeek = time.GetDayOfWeek();			// 현재 요일 구하기 (1 = 일요일, 2 = 월요일, ..., 7 = 토요일)
	CTimeSpan spanWeek(dayOfWeek - 2, 0, 0, 0);		//	2 = 월요일,

	CGridCellDateTime* pGridDate_From = (CGridCellDateTime*)pGrid->GetCell(0, 1);
	pGridDate_From->SetTime(time - spanWeek);
	CGridCellDateTime* pGridDate_To = (CGridCellDateTime*)pGrid->GetCell(1, 1);
	pGridDate_To->Init(DTS_SHORTDATEFORMAT);

	pGrid->Refresh();
}

VOID CDlgLogs::GetGridTextData(COleDateTime& tStart, COleDateTime& tEnd)
{
	CGridCtrl* pGrid = &m_grd_ctl[eLOGS_GRD_SEARCH];
	CGridCellDateTime* pGridFromDate = (CGridCellDateTime*)pGrid->GetCell(0, 1);
	CGridCellDateTime* pGridToDate = (CGridCellDateTime*)pGrid->GetCell(1, 1);

	CTime ttmpStart(pGridFromDate->GetTime()->GetYear(), pGridFromDate->GetTime()->GetMonth(), pGridFromDate->GetTime()->GetDay(), 0, 0, 0);
	CTime ttmpEnd(pGridToDate->GetTime()->GetYear(), pGridToDate->GetTime()->GetMonth(), pGridToDate->GetTime()->GetDay(), 0, 0, 0);

	tStart	= ttmpStart.GetTime();
	tEnd	= ttmpEnd.GetTime();
}

VOID CDlgLogs::SetStop()
{
	m_bStop = TRUE;
}

VOID CDlgLogs::InitTitles()
{
	for (int rch = 0; rch < eLOGS_RCH_MAX; rch++)
		m_ttl_ctl[rch].SetWindowTextW(L"");
}

VOID CDlgLogs::InitContents()
{
	//	화면 초기화
	for (int i = 0; i < eLOGS_RCH_MAX; i++)
	{
		m_rch_ctl[i].SetClearAll(TRUE);
	}
}

VOID CDlgLogs::UpdateLogTitles()
{
	int nRch = 0, nType = 0;
	while (nRch < eLOGS_RCH_MAX && nType < eLOGS_TYPE_MAX)
	{
		if (m_chk_flt[nType].GetCheck())
		{
			m_ttl_ctl[nRch].SetWindowTextW(m_tzType[nType]);
			m_ttl_ctl[nRch].SetRedraw(TRUE);
			m_ttl_ctl[nRch].Invalidate(FALSE);

			nRch++;
		}

		nType++;
	}
}

VOID CDlgLogs::UpdateLogContents()
{
	if (eLOGS_STEP_VIEW_UPDATE != m_eSTEP)
	{
		return;
	}

	int nTotal = 0;
	int nCount = 0;

	/* 전체 개수 얻기 */	
	for (int nType = 0; nType < eLOGS_TYPE_MAX; nType++)
	{
		if (m_chk_flt[nType].GetCheck())
		{
			nTotal += (int)m_vLogs[nType].size();
		}
	}

	if (nTotal == 0)
	{
		m_eSTEP = eLOGS_STEP_INIT;
		m_bStop = FALSE;
		return;
	}	

	/* 개별 개수 얻기 */
	int nRch = 0, nType = 0;
	while (nRch < eLOGS_RCH_MAX && nType < eLOGS_TYPE_MAX)
	{
		if (m_chk_flt[nType].GetCheck())
		{
			if (m_vLogs[nType].size() > 0)
			{
				for (auto log : m_vLogs[nType])
				{
					if (m_bStop)
						break;

					m_rch_ctl[nRch].Append(log.strLog.GetBuffer(), RGB(0, 0, 0), FALSE);
					log.strLog.ReleaseBuffer();

					if (m_pPgr)
						m_pPgr->SetPos(_CAL_PERCENTAGE_(++nCount, nTotal));

				}

				m_vLogs[nType].clear();
			}

			nRch++;
		}

		nType++;
	}

	m_eSTEP = eLOGS_STEP_INIT;
	m_bStop = FALSE;
}

VOID CDlgLogs::InitListFirstLine()
{
	int nFirstVisibleLine = 0;
	for (int i = 0; i < eLOGS_RCH_MAX; i++)
	{
		nFirstVisibleLine = m_rch_ctl[i].GetFirstVisibleLine();
		if (nFirstVisibleLine > 0)
		{
			m_rch_ctl[i].LineScroll(-nFirstVisibleLine);
		}
	}
}

UINT CDlgLogs::LogThreadProc()
{
	while (m_bStopThread.load() == false)
	{
		CDlgLogs* pDlg = (CDlgLogs*)this;
		// 로그 업데이트 주기 (0.1초)
		Sleep(100);
		pDlg->UpdateLogContents();
	}
	return 0;
}

void CDlgLogs::StartLogThread()
{
	m_bStopThread.store(false);
	m_pLogThread = std::thread([=]() {LogThreadProc(); });  //AfxBeginThread(LogThreadProc, this);
}

void CDlgLogs::StopLogThread()
{
	m_bStopThread.store(true);
	Sleep(200);
	if (m_pLogThread.joinable())
		m_pLogThread.join();

}

EN_LOG_STEP CDlgLogs::GetLogStep()
{
	return m_eSTEP;
}

LRESULT CDlgLogs::OnSyncScroll(WPARAM wParam, LPARAM lParam)
{
	if (m_chk_vop[eLOGS_VOP_SYNC_SCROLL].GetCheck())
	{
		CMyRich* triggeredControl = (CMyRich*)wParam;
		int nLinesToScroll = static_cast<int>(lParam);  // 스크롤된 라인 수

		for (auto& control : m_rch_ctl)
		{
			if (&control != triggeredControl)
			{
				control.LineScroll(nLinesToScroll);
			}
		}
	}

	return 0;
}
