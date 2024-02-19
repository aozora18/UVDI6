
/*
 desc : Luria - Printing (Exposure)
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgExpo.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgExpo::CDlgExpo(UINT32 id,CWnd* parent)
	: CDlgMenu(id, parent)
{
	m_pGridLed				= NULL;
	/* 가장 최근 값 임시 저장 */
	m_u64TickLoadState		= 0;
	m_u64TickExpoState		= 0;
	m_bIsExpoUpdate			= FALSE;
	m_u32ExpoSpeed			= 0;
	m_pLedAmplitue			= NULL;
	m_enExpoStatus			= ENG_LPES::en_not_defined;
	m_enLastJobLoadStatus	= ENG_LSLS::en_load_none;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgExpo::~CDlgExpo()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgExpo::DoDataExchange(CDataExchange* dx)
{
	CDlgMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Group box */
	u32StartID	= IDC_EXPO_GRP_JOBS;
	for (i=0; i<4; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Process */
	u32StartID	= IDC_EXPO_PGR_PRINT_RATE;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_pgs_ctl[i]);
	/* List box */
	u32StartID	= IDC_EXPO_BOX_JOBS_LIST;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_box_ctl[i]);
	/* Button */
	u32StartID	= IDC_EXPO_BTN_JOBS_ADD;
	for (i=0; i<10; i++)	DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Static */
	u32StartID	= IDC_EXPO_TXT_JOBS_LOAD;
	for (i=0; i<11; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Edit box */
	u32StartID	= IDC_EXPO_EDT_JOBS_STATUS;
	for (i=0; i<6; i++)		DDX_Control(dx, u32StartID+i,	m_edt_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgExpo, CDlgMenu)
	ON_LBN_SELCHANGE(IDC_EXPO_BOX_JOBS_LIST, OnBoxSelchangeJobList)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_EXPO_BTN_JOBS_ADD, IDC_EXPO_BTN_LED_SET, OnBnClicked)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgExpo::PreTranslateMessage(MSG* msg)
{

	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgExpo::OnInitDlg()
{
	if (!CDlgMenu::OnInitDlg())	return FALSE;

	/* 메모리 초기화 */
	InitMem();
	/* 컨트롤 초기화 */
	InitCtrl();
	/* 컨트롤 초기화 */
	InitGrid();

	/* 상위 객체 호출 */
	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgExpo::OnExitDlg()
{
	CDlgMenu::OnExitDlg();

	UINT8 i	= 0x00;

	/* GridCtrl 해제 */
	if (m_pGridLed)
	{
		if (m_pGridLed->GetSafeHwnd())	m_pGridLed->DestroyWindow();
		delete m_pGridLed;
	}

	/* 메모리 해제 */
	for (; i<uvEng_GetConfig()->luria_svc.ph_count; i++)	::Free(m_pLedAmplitue[i]);
	::Free(m_pLedAmplitue);
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgExpo::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgExpo::OnResizeDlg()
{
}

/*
 desc : 메모리 초기화
 parm : None
 retn : None
*/
VOID CDlgExpo::InitMem()
{
	UINT8 i		= 0x00;
	UINT8 u8PHs	= uvEng_GetConfig()->luria_svc.ph_count;
	UINT8 u8Led	= uvEng_GetConfig()->luria_svc.led_count;

	/* Photohead LED Power 정보 임시 저장 */
	m_pLedAmplitue	= (UINT16**)::Alloc(u8PHs * sizeof(PUINT16));
	for (; i<u8PHs; i++)
	{
		m_pLedAmplitue[i]	= (PUINT16)::Alloc(sizeof(UINT16) * u8Led);
		memset(m_pLedAmplitue[i], 0x00, sizeof(UINT16) * u8Led);
	}
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgExpo::InitCtrl()
{
	INT32 i;

	/* Groupbox */
	for (i=0; i<4; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* Progress */
	for (i=0; i<1; i++)
	{
		/* Progress - normal */
		m_pgs_ctl[i].SetColor(RGB(160, 255, 0));
	}
	/* List box - normal */
	for (i=0; i<1; i++)
	{
		m_box_ctl[i].SetLogFont(&g_lf);
	}
	/* Button - normal */
	for (i=0; i<10; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
	}
	/* text - normal */
	for (i=0; i<11; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* Edit box */
	for (i=0; i<6; i++)
	{
		m_edt_ctl[i].SetEditFont(&g_lf);
		m_edt_ctl[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_ctl[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_ctl[i].SetInputType(ENM_DITM::en_float);
		if (i < 3)
		{
			m_edt_ctl[i].SetMouseClick(FALSE);
			m_edt_ctl[i].SetReadOnly(TRUE);
		}
	}
}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgExpo::InitGrid()
{
	TCHAR tzCols[5][8]	= { L"Item", L"LED1", L"LED2", L"LED3", L"LED4" }, tzRows[32] = {NULL};
	INT32 i32Width[5]	= { 40, 40, 40, 40, 40 }, i, j;
	INT32 i32ColCnt		= 5, i32RowCnt = uvEng_GetConfig()->luria_svc.ph_count;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* Last Recv. Packet / PDO Header / PDO Data (Values) */
	m_grp_ctl[3].GetWindowRect(rGrid);
	ScreenToClient(rGrid);
	rGrid.left	+= 14;
	rGrid.right	-= 78;
	rGrid.top	+= 24;
	rGrid.bottom-= 16;

	/* 그리드 컨트롤 기본 공통 속성 */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* 객체 생성 */
	m_pGridLed	= new CGridCtrl;
	ASSERT(m_pGridLed);
	m_pGridLed->SetDrawScrollBarHorz(FALSE);
	m_pGridLed->SetDrawScrollBarVert(TRUE);
	if (!m_pGridLed->Create(rGrid, this, IDC_MENU_GRID_EXPO_AMPL, dwStyle))
	{
		delete m_pGridLed;
		m_pGridLed = NULL;
		return;
	}
	/* 객체 기본 설정 */
	m_pGridLed->SetLogFont(g_lf);
	m_pGridLed->SetRowCount(i32RowCnt+1);
	m_pGridLed->SetColumnCount(i32ColCnt);
	m_pGridLed->SetFixedRowCount(1);
	m_pGridLed->SetRowHeight(0, 25);
	m_pGridLed->SetFixedColumnCount(0);
	m_pGridLed->SetBkColor(RGB(255, 255, 255));
	m_pGridLed->SetFixedColumnSelection(0);

	/* 타이틀 (첫 번째 행) */
	stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	stGV.crBkClr = RGB(214, 214, 214);
	stGV.crFgClr = RGB(0, 0, 0);
	stGV.lfFont	= g_lf;
	for (i=0; i<i32ColCnt; i++)
	{
		stGV.row	= 0;
		stGV.col	= i;
		stGV.strText= tzCols[i];
		m_pGridLed->SetItem(&stGV);
		m_pGridLed->SetColumnWidth(i, i32Width[i]);
	}

	/* 타이틀 (첫 번째 열) */
	for (i=1; i<=i32RowCnt; i++)
	{
		stGV.row	= i;
		stGV.col	= 0;
		stGV.strText.Format(L"PH %d", i);
		m_pGridLed->SetItem(&stGV);
		m_pGridLed->SetRowHeight(i, 24);
	}

	/* 본문 */
	stGV.nFormat = DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
	stGV.crBkClr = RGB(255, 255, 255);
	for (i=1; i<=i32RowCnt; i++)
	{
		for (j=1; j<i32ColCnt; j++)
		{
			stGV.row	= i;
			stGV.col	= j;
			stGV.strText= L"";
			m_pGridLed->SetItem(&stGV);
		}
	}

	m_pGridLed->SetBaseGridProp(1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0);
	m_pGridLed->UpdateGrid();
}

/*
 desc : 현재 윈도 컨트롤 영역 갱신
 parm : None
 retn : None
*/
VOID CDlgExpo::UpdateCtrl()
{
	/* Job List 갱신 */
	UpdateJobList();
	/* Job Load 갱신 */
	UpdateJobLoad();
	/* Exposure 갱신 */
	UpdateExposure();
	/* LED Power 갱신 */
	UpdateLedAmpl();
}

/*
 desc : Job List 갱신
 parm : None
 retn : None
*/
VOID CDlgExpo::UpdateJobList()
{
	UINT8 i, u8Find		= 0x00;
	TCHAR tzGerberName[MAX_GERBER_NAME]	= {NULL};
	BOOL bUpdateJobs	= FALSE;
	INT32 i32JobList	= m_box_ctl[0].GetCount(), i32AddIndex;
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;
	CUniToChar csCnv;

	/* 메모리에 등록된 것이 없으면서, 리스트에 등록되것이 없으면 Skip */
	if (pstJobMgt->job_regist_count < 1 && i32JobList < 1)	return;
	/* 메모리에 등록된 개수와 리스트 박스에 등록된 개수가 다른지 확인 */
	if (i32JobList != pstJobMgt->job_regist_count)	bUpdateJobs	= TRUE;
	/* 공유 메모리와 리스트에 등록된 개수는 같지만, 혹시 거버 이름이 다를수 도 있으므로 */
	if (!bUpdateJobs)
	{
		for (i=0; i<pstJobMgt->job_regist_count; i++)
		{
			/* List Box에 등록된 Gerber Name 가져오기 */
			wmemset(tzGerberName, 0x00, MAX_GERBER_NAME);
			m_box_ctl[0].GetText(i, tzGerberName);
			/* 동일한 항목을 찾았으면... 찾은 개수 증가 시키기 */
			if (pstJobMgt->IsJobOnlyFinded(csCnv.Uni2Ansi(tzGerberName)))	u8Find++;
		}
		/* 모두 동일한 항목인지 확인 / 동일하지 않으면, 리스트 박스 갱신 필요 */
		if (u8Find != pstJobMgt->job_regist_count)	bUpdateJobs	= TRUE;
	}
	/* 갱신할 필요 없다면, 리턴 */
	if (!bUpdateJobs)	return;

	/* ------------------------ */
	/* Job List 등록 혹은 갱신  */
	/* ------------------------ */

	/* 갱신 비활성화 */
	m_box_ctl[0].SetRedraw(FALSE);
	/* 기존 항목 모두 지우기 */
	m_box_ctl[0].ResetContent();
	/* 공유 메모리에 있는 항목 등록 */
	for (i=0; i<pstJobMgt->job_regist_count; i++)
	{
		i32AddIndex	= m_box_ctl[0].AddString(csCnv.Ansi2Uni(pstJobMgt->job_name_list[i]));
		/* 현재 선택된 항목인지 확인 후, 선택된 항목이면, 선택 작업 진행 */
		if (0 == strcmp(pstJobMgt->job_name_list[i], pstJobMgt->selected_job_name))
		{
			m_box_ctl[0].SetCurSel(i32AddIndex);
		}
	}

	/* 갱신 활성화 */
	m_box_ctl[0].SetRedraw(TRUE);
	m_box_ctl[0].Invalidate(FALSE);
}

/*
 desc : Job Load 상태 갱신
 parm : None
 retn : None
*/
VOID CDlgExpo::UpdateJobLoad()
{
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;

	/* 현재 선택된 Job Name이 존재하는지 여부 */
	if (!pstJobMgt->IsJobNameSelected())
	{
		m_edt_ctl[0].SetTextToStr(L"None",	TRUE);
		return;
	}

	switch (pstJobMgt->selected_job_load_state)
	{
	case ENG_LSLS::en_load_none			:	m_edt_ctl[0].SetTextToStr(L"None",		TRUE);	break;
	case ENG_LSLS::en_load_not_started	:	m_edt_ctl[0].SetTextToStr(L"Ready",		TRUE);	break;
	case ENG_LSLS::en_loading			:	m_edt_ctl[0].SetTextToStr(L"Loading",	TRUE);	break;
	case ENG_LSLS::en_load_completed	:	m_edt_ctl[0].SetTextToStr(L"Loaded",	TRUE);	break;
	case ENG_LSLS::en_load_failed		:	m_edt_ctl[0].SetTextToStr(L"Failed",	TRUE);	break;
	}

	/* 현재 상태가 Completed이고 이전 (최근) 상태가 Completed가 아니라면 ... */
	if (ENG_LSLS(pstJobMgt->selected_job_load_state) == ENG_LSLS::en_load_completed &&
		ENG_LSLS(pstJobMgt->selected_job_load_state) != m_enLastJobLoadStatus)
	{
		/* XML 파일 적재 진행 */
		if (!uvEng_Luria_LoadSelectJobXML())
		{
			AfxMessageBox(L"Failed to load the currently selected gerber file", MB_ICONSTOP|MB_TOPMOST);
		}
	}

	/* 만약, 현재 거버가 적재 중인 상태인 경우, 지속적으로 Job Load State 요청 */
	if ((pstJobMgt->selected_job_load_state == (UINT8)ENG_LSLS::en_load_not_started ||
		 pstJobMgt->selected_job_load_state == (UINT8)ENG_LSLS::en_loading) &&
		 (GetTickCount64() > m_u64TickLoadState + 250))
	{
		/* 다음 요청 주기 결정할 시간 값 임시 저장 */
		m_u64TickLoadState	= GetTickCount64();
		/* Job Load State 요청 */
		uvEng_Luria_ReqGetSelectedJobLoadState();
	}
}

/*
 desc : Exposure 상태 갱신 (Print Speed, Step Size, Frame Rate, Duty Cycle, Printing, Status, ...)
 parm : None
 retn : None
*/
VOID CDlgExpo::UpdateExposure()
{
	LPG_LDEW pstExpo	= &uvEng_ShMem_GetLuria()->exposure;
	UINT8 u8StateMask	= pstExpo->get_exposure_state[0] & 0x0f;

	/* 이전 값과 동일하면, 더 이상 처리하지 않음 */
	if ((UINT8)m_enExpoStatus != pstExpo->get_exposure_state[0])
	{
		/* Exposure State */
		switch (pstExpo->get_exposure_state[0])
		{
		case ENG_LPES::en_not_defined		:	m_edt_ctl[1].SetTextToStr(L"Not defined", TRUE);	break;
		case ENG_LPES::en_idle				:	m_edt_ctl[1].SetTextToStr(L"Idle", TRUE);			break;

		case ENG_LPES::en_preprint_running	:	m_edt_ctl[1].SetTextToStr(L"Pre-Print Running");	break;
		case ENG_LPES::en_preprint_success	:	m_edt_ctl[1].SetTextToStr(L"Pre-Print Success");	break;
		case ENG_LPES::en_preprint_failed	:	m_edt_ctl[1].SetTextToStr(L"Pre-Print Failed");		break;	

		case ENG_LPES::en_print_running		:	m_edt_ctl[1].SetTextToStr(L"Print Running");		break;
		case ENG_LPES::en_print_success		:	m_edt_ctl[1].SetTextToStr(L"Print Success");		break;
		case ENG_LPES::en_print_failed		:	m_edt_ctl[1].SetTextToStr(L"Print Failed");			break;
#if 0
		case ENG_LPES::en_simul_running		:	m_edt_ctl[1].SetTextToStr(L"Simulate Running");		break;
		case ENG_LPES::en_simul_success		:	m_edt_ctl[1].SetTextToStr(L"Simulate Success");		break;
		case ENG_LPES::en_simul_failed		:	m_edt_ctl[1].SetTextToStr(L"Simulate Failed");		break;
#endif
		case ENG_LPES::en_abort_in_progress	:	m_edt_ctl[1].SetTextToStr(L"Abort Running");		break;
		case ENG_LPES::en_abort_success		:	m_edt_ctl[1].SetTextToStr(L"Abort Success");		break;
		case ENG_LPES::en_abort_failed		:	m_edt_ctl[1].SetTextToStr(L"Abort Failed");			break;
		}

		/* 가장 최근 값 임시 저장 */
		m_enExpoStatus	= (ENG_LPES)pstExpo->get_exposure_state[0];
	}
//	TRACE(L"cur=%d, tot=%d\n", pstExpo->get_exposure_state[1], pstExpo->get_exposure_state[2]);
	/* 만약 현재 Running (Printing) 상태이면, Progress Bar 갱신 */
	if (0xA0 != (pstExpo->get_exposure_state[0] & 0xA0) &&
		0x01 == (pstExpo->get_exposure_state[0] & 0x01))
	{
		/* Progress Range 설정 */
		m_pgs_ctl[0].SetRange(0, pstExpo->get_exposure_state[2]);
		/* 현재 진행된 값 설정 */
		m_pgs_ctl[0].SetPos(pstExpo->get_exposure_state[1]);
	}
	else if ((pstExpo->get_exposure_state[0] & 0xf2) == 0x22 ||
			 (pstExpo->get_exposure_state[0] & 0xf2) == 0x32)
	{
		m_pgs_ctl[0].SetPos(pstExpo->get_exposure_state[2]);
	}

	/* Exposure Speed / Step Size / Frame Rate / Duty Cycle */
	if (m_u32ExpoSpeed != pstExpo->get_exposure_speed)
	{
		/* Exposure Speed */
		m_edt_ctl[2].SetTextToNum(DOUBLE(pstExpo->get_exposure_speed/1000.0f), 3, TRUE);
		/* Step Size */
		m_edt_ctl[3].SetTextToNum(pstExpo->scroll_step_size, TRUE);
		/* Frame Rate */
		m_edt_ctl[4].SetTextToNum(DOUBLE(pstExpo->frame_rate_factor/1000.0f), 3, TRUE);
		/* Duty Cycle */
		m_edt_ctl[5].SetTextToNum(pstExpo->led_duty_cycle, TRUE);

		/* 가장 최근 값 임시 저장 */
		m_u32ExpoSpeed = pstExpo->get_exposure_speed;
	}

	/* 만약 노광 상태가 더 이상 갱신할 필요가 없는지 여부 */
	if (u8StateMask == 0x02 || u8StateMask == 0x0f)	m_bIsExpoUpdate	= FALSE;	/* 더 이상 노광 상태 요청하지 말아라 */
	/* 현재 노광 상태 정보 요청 모드이면, 상태 정보 요청 */
	if (m_bIsExpoUpdate)
	{
		if (m_u64TickExpoState + 250 < GetTickCount64())
		{
			/* 현재 노광 진행 상태 정보 요청 */
			uvEng_Luria_ReqGetExposureState();
			/* 가장 최근에 요청한 시간 임시 저장 */
			m_u64TickExpoState	= GetTickCount64();
		}
	}
}

/*
 desc : LED Power 갱신
 parm : None
 retn : None
*/
VOID CDlgExpo::UpdateLedAmpl()
{
	UINT8 i		= 0x00;
	UINT8 u8PHs	= uvEng_GetConfig()->luria_svc.ph_count;
	UINT8 u8Led	= uvEng_GetConfig()->luria_svc.led_count;
	BOOL bUpdate	= FALSE;
	LPG_LDEW pstExpo= &uvEng_ShMem_GetLuria()->exposure;

	/* 기존 값과 동일하면 갱신하지 않음 */
	for (; i<u8PHs && !bUpdate; i++)
	{
		if (0 != memcmp(m_pLedAmplitue[i], pstExpo->led_amplitude_power[i], sizeof(UINT16) * u8Led))
		{
			bUpdate = TRUE;
		}
	}
	/* 변경된 값이 없다면 ... */
	if (!bUpdate)	return;

	/* 만약 LED Power 값이 다르면 갱신 */
	m_pGridLed->SetRedraw(FALSE);
	for (i=0x00; i<u8PHs; i++)
	{
		m_pGridLed->SetItemText(i+1, 1, UINT32(pstExpo->led_amplitude_power[i][0]));
		m_pGridLed->SetItemText(i+1, 2, UINT32(pstExpo->led_amplitude_power[i][1]));
		m_pGridLed->SetItemText(i+1, 3, UINT32(pstExpo->led_amplitude_power[i][2]));
		m_pGridLed->SetItemText(i+1, 4, UINT32(pstExpo->led_amplitude_power[i][3]));
	}
	m_pGridLed->SetRedraw(TRUE);
	m_pGridLed->Invalidate(FALSE);
}

/*
 desc : Button Event
 parm : id	- [in]  Button ID
 retn : None
*/
VOID CDlgExpo::OnBnClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_EXPO_BTN_JOBS_ADD		:	JobMgtAdd();		break;
	case IDC_EXPO_BTN_JOBS_DEL		:	JobMgtDel();		break;
	case IDC_EXPO_BTN_JOBS_LOAD		:	JobMgtLoad();		break;
	case IDC_EXPO_BTN_EXPO_SET		:	ExpoParamSet();		break;
	case IDC_EXPO_BTN_EXPO_READY	:	ExpoStart(0x00);	break;
	case IDC_EXPO_BTN_EXPO_START	:	ExpoStart(0x01);	break;
	case IDC_EXPO_BTN_EXPO_ABORT	:	ExpoStart(0x03);	break;
	case IDC_EXPO_BTN_EXPO_RAMPUP	:	ExpoStart(0x06);	break;
	case IDC_EXPO_BTN_LED_GET		:	GetLedLightLvl();	break;
	case IDC_EXPO_BTN_LED_SET		:	SetLedLightLvl();	break;
	}
}

/*
 desc : List Box에서 거버를 선택한 경우, Luria Service에서 해당 선택된 거버로 선택되도록 처리
 parm : None
 retn : None
*/
VOID CDlgExpo::OnBoxSelchangeJobList()
{
	INT32 i32CurSel	= 0;
	TCHAR tzJobName[MAX_PATH_LEN]	= {NULL};
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;
	CUniToChar csCnv;

	/* List Box에서 선택된 항목 가져오기 */
	i32CurSel	= m_box_ctl[0].GetCurSel();
	if (i32CurSel < 0)
	{
		AfxMessageBox(L"The selected item does not exist", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 선택된 항목이 유효한지 검사 */
	m_box_ctl[0].GetText(i32CurSel, tzJobName);
	if (wcslen(tzJobName) < 1)	return;
	/* 현재 선택된 Job Name가 동일하지 여부 확인 */
	if (0 == wcscmp(tzJobName, csCnv.Ansi2Uni(pstJobMgt->selected_job_name)))
	{
		AfxMessageBox(L"Job Name already selected", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* Luria에 선택된 거버가 선택되도록 명령어 전달 */
	if (!uvEng_Luria_ReqSelectedJobName(tzJobName, 0x00))
	{
		AfxMessageBox(L"Failed to selected the Job Name", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : 새로운 Job Name 등록
 parm : None
 retn : None
*/
VOID CDlgExpo::JobMgtAdd()
{
	TCHAR tzJobName[MAX_PATH_LEN]	= {NULL}, tzRLTFiles[MAX_PATH_LEN] = {NULL};
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;
	CUniToChar csCnv;

	/* Job Name 디렉토리 선택 */
	if (!uvCmn_RootSelectPath(m_hWnd, uvEng_GetConfig()->set_comn.gerber_path, tzJobName))	return;
	/* Job Name 내에 하위 디렉토리가 존재하는지 확인 */
	if (uvCmn_GetChildPathCount(tzJobName) != 0)
	{
		AfxMessageBox(L"This is not a gerber directory", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 해당 디렉토리 내에 rlt_settings.xml 파일이 존재하는지 확인 */
	swprintf_s(tzRLTFiles, MAX_PATH_LEN, L"%s\\rlt_settings.xml", tzJobName);
	if (!uvCmn_FindFile(tzRLTFiles))
	{
		AfxMessageBox(L"File (rlt_settings.xml) does not exist", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 기존에 등록된 거버 파일 중에 동일한 거버가 있는지 여부 확인 */
	if (pstJobMgt->IsJobOnlyFinded(csCnv.Uni2Ansi(tzJobName)))
	{
		AfxMessageBox(L"Job Name already registered exist", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* Luria Service에 등록 작업 진행 */
	if (!uvEng_Luria_ReqAddJobList(tzJobName))
	{
		AfxMessageBox(L"Failed to register Job Name", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : 기존 Job Name 제거
 parm : None
 retn : None
*/
VOID CDlgExpo::JobMgtDel()
{
	INT32 i32CurSel	= 0;
	TCHAR tzJobName[MAX_PATH_LEN]	= {NULL};
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;
	CUniToChar csCnv;

	/* List Box에서 선택된 항목 가져오기 */
	i32CurSel	= m_box_ctl[0].GetCurSel();
	if (i32CurSel < 0)
	{
		AfxMessageBox(L"The selected item does not exist", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 선택된 항목이 유효한지 검사 */
	m_box_ctl[0].GetText(i32CurSel, tzJobName);
	if (wcslen(tzJobName) < 1)	return;
	if (0 != wcscmp(tzJobName, csCnv.Ansi2Uni(pstJobMgt->selected_job_name)))
	{
		AfxMessageBox(L"The gerber for removal is not selected in the optics", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 선택된 항목을 거버에서 삭제 */
	if (!uvEng_Luria_ReqSetDeleteSelectedJob())
	{
		AfxMessageBox(L"Failed to delete the selected Gerber Name", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 현재 상태된 Job List 요청 */
	uvEng_Luria_ReqGetJobList();
}

/*
 desc : 현재 상태된 Job Name 적재 진행
 parm : None
 retn : None
*/
VOID CDlgExpo::JobMgtLoad()
{
	INT32 i32CurSel	= 0;
	TCHAR tzJobName[MAX_PATH_LEN]	= {NULL};
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;
	CUniToChar csCnv;

	/* List Box에서 선택된 항목 가져오기 */
	i32CurSel	= m_box_ctl[0].GetCurSel();
	if (i32CurSel < 0)
	{
		AfxMessageBox(L"The selected item does not exist", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 선택된 항목이 유효한지 검사 */
	m_box_ctl[0].GetText(i32CurSel, tzJobName);
	if (wcslen(tzJobName) < 1)	return;
	if (0 != wcscmp(tzJobName, csCnv.Ansi2Uni(pstJobMgt->selected_job_name)))
	{
		AfxMessageBox(L"The gerber is not selected in the optics", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 현재 선택된 Job의 적재 진행 요청 */
	if (!uvEng_Luria_ReqSetLoadSelectedJob())
	{
		AfxMessageBox(L"Failed to load the Gerber Name", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 현재 노광 조건 정보 요청 */
	if (!uvEng_Luria_ReqGetExposureFactor())
	{
		AfxMessageBox(L"Failed to request the Exposure Factor", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : Exposure (Print) 조건 설정
 parm : None
 retn : None
*/
VOID CDlgExpo::ExpoParamSet()
{
	UINT8 u8StepSize	= (UINT8)m_edt_ctl[3].GetTextToNum();
	UINT8 u8DutyCycle	= (UINT8)m_edt_ctl[5].GetTextToNum();
	UINT16 u16FrameRate	= (UINT16)ROUNDED(m_edt_ctl[4].GetTextToDouble() * 1000.0f, 0);
	LPG_LDEW pstExpo	= &uvEng_ShMem_GetLuria()->exposure;

	/* 현재 설정 값과 이전 값이 다르면 갱신하지 않음 */
	if (u8StepSize != pstExpo->scroll_step_size ||
		u8DutyCycle != pstExpo->led_duty_cycle ||
		u16FrameRate != pstExpo->frame_rate_factor)
	{
		if (!uvEng_Luria_ReqSetExposureFactor(u8StepSize, u8DutyCycle, u16FrameRate/1000.0f))
		{
			AfxMessageBox(L"Failed to set the value required for exposure", MB_ICONSTOP|MB_TOPMOST);
			return;
		}
	}
}

/*
 desc : Exposure Start
 parm : mode	- [in]  Print 관련 명령어
						0 : Pre-Print
						1 : Print
						2 : Simulation Print
						3 : Abort
						4 : Reset Exposure State
						5 : Move Start Position
 retn : None
*/
VOID CDlgExpo::ExpoStart(UINT8 mode)
{
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;
	LPG_LDEW pstExpo	= &uvEng_ShMem_GetLuria()->exposure;

	/* 현재 선택 및 적재 (Loaded)된 Job이 있는지 조회 */
	if (!(pstJobMgt->IsJobNameSelected() && pstJobMgt->IsJobNameLoadCompleted()))
	{
		AfxMessageBox(L"Job Name is not selected or not loaded", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* 노광 제어 */
	if (!uvEng_Luria_ReqSetPrintOpt(mode))
	{
		AfxMessageBox(L"Failed to set the print for exposure", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* 노광 상태를 갱신하라고 플래그 설정 */
	m_bIsExpoUpdate	= TRUE;
}

/*
 desc : 현재 Photohead의 LED에 설정된 Level Index 값 요청
 parm : None
 retn : None
*/
VOID CDlgExpo::GetLedLightLvl()
{
	UINT8 i	= 0x01;
	BOOL bSucc	= TRUE;

#if 0
	for (; i<=uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		if (bSucc)	bSucc	= uvEng_Luria_ReqGetLedPower(i, 0);	/* 365 */
		if (bSucc)	bSucc	= uvEng_Luria_ReqGetLedPower(i, 1);	/* 385 */
		if (bSucc)	bSucc	= uvEng_Luria_ReqGetLedPower(i, 2);	/* 395 */
		if (bSucc)	bSucc	= uvEng_Luria_ReqGetLedPower(i, 3);	/* 405 */
	}
#else
	bSucc	= uvEng_Luria_ReqGetLedAmplitude();
#endif
	if (!bSucc)
	{
		AfxMessageBox(L"Failed to request the LED level value for photohead", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : Photohead의 LED에 새로운 Level Index 값 설정
 parm : None
 retn : None
*/
VOID CDlgExpo::SetLedLightLvl()
{
	UINT8 i	= 0x01;
	UINT16 u16Level	= 0;
	BOOL bSucc	= TRUE;

	for (; i<=uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		u16Level	= (UINT16)m_pGridLed->GetItemTextToInt(i, 1);
		if (bSucc)	bSucc	= uvEng_Luria_ReqSetLedAmplitudeOne(i, ENG_LLPI(0), u16Level);	/* 365 */
		u16Level	= (UINT16)m_pGridLed->GetItemTextToInt(i, 2);
		if (bSucc)	bSucc	= uvEng_Luria_ReqSetLedAmplitudeOne(i, ENG_LLPI(1), u16Level);	/* 385 */
		u16Level	= (UINT16)m_pGridLed->GetItemTextToInt(i, 3);
		if (bSucc)	bSucc	= uvEng_Luria_ReqSetLedAmplitudeOne(i, ENG_LLPI(2), u16Level);	/* 395 */
		u16Level	= (UINT16)m_pGridLed->GetItemTextToInt(i, 4);
		if (bSucc)	bSucc	= uvEng_Luria_ReqSetLedAmplitudeOne(i, ENG_LLPI(3), u16Level);	/* 405 */
	}
	if (!bSucc)
	{
		AfxMessageBox(L"Failed to request the LED level value for photohead", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}
