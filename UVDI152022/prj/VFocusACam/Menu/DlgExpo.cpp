
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
 desc : ������
 parm : id		- [in]  �ڽ��� ���� ID
		parent	- [in]  �ڽ��� ȣ���� �θ� ���� Ŭ���� ������
 retn : None
*/
CDlgExpo::CDlgExpo(UINT32 id,CWnd* parent)
	: CDlgMenu(id, parent)
{
	m_pGridLed				= NULL;
	/* ���� �ֱ� �� �ӽ� ���� */
	m_u64TickLoadState		= 0;
	m_u64TickExpoState		= 0;
	m_bIsExpoUpdate			= FALSE;
	m_u32ExpoSpeed			= 0;
	m_pLedAmplitue			= NULL;
	m_enExpoStatus			= ENG_LPES::en_not_defined;
	m_enLastJobLoadStatus	= ENG_LSLS::en_load_none;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDlgExpo::~CDlgExpo()
{
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
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
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgExpo::PreTranslateMessage(MSG* msg)
{

	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgExpo::OnInitDlg()
{
	if (!CDlgMenu::OnInitDlg())	return FALSE;

	/* �޸� �ʱ�ȭ */
	InitMem();
	/* ��Ʈ�� �ʱ�ȭ */
	InitCtrl();
	/* ��Ʈ�� �ʱ�ȭ */
	InitGrid();

	/* ���� ��ü ȣ�� */
	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgExpo::OnExitDlg()
{
	CDlgMenu::OnExitDlg();

	UINT8 i	= 0x00;

	/* GridCtrl ���� */
	if (m_pGridLed)
	{
		if (m_pGridLed->GetSafeHwnd())	m_pGridLed->DestroyWindow();
		delete m_pGridLed;
	}

	/* �޸� ���� */
	for (; i<uvEng_GetConfig()->luria_svc.ph_count; i++)	::Free(m_pLedAmplitue[i]);
	::Free(m_pLedAmplitue);
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgExpo::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgExpo::OnResizeDlg()
{
}

/*
 desc : �޸� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgExpo::InitMem()
{
	UINT8 i		= 0x00;
	UINT8 u8PHs	= uvEng_GetConfig()->luria_svc.ph_count;
	UINT8 u8Led	= uvEng_GetConfig()->luria_svc.led_count;

	/* Photohead LED Power ���� �ӽ� ���� */
	m_pLedAmplitue	= (UINT16**)::Alloc(u8PHs * sizeof(PUINT16));
	for (; i<u8PHs; i++)
	{
		m_pLedAmplitue[i]	= (PUINT16)::Alloc(sizeof(UINT16) * u8Led);
		memset(m_pLedAmplitue[i], 0x00, sizeof(UINT16) * u8Led);
	}
}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
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
 desc : ��Ʈ�� �ʱ�ȭ
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

	/* �׸��� ��Ʈ�� �⺻ ���� �Ӽ� */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* ��ü ���� */
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
	/* ��ü �⺻ ���� */
	m_pGridLed->SetLogFont(g_lf);
	m_pGridLed->SetRowCount(i32RowCnt+1);
	m_pGridLed->SetColumnCount(i32ColCnt);
	m_pGridLed->SetFixedRowCount(1);
	m_pGridLed->SetRowHeight(0, 25);
	m_pGridLed->SetFixedColumnCount(0);
	m_pGridLed->SetBkColor(RGB(255, 255, 255));
	m_pGridLed->SetFixedColumnSelection(0);

	/* Ÿ��Ʋ (ù ��° ��) */
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

	/* Ÿ��Ʋ (ù ��° ��) */
	for (i=1; i<=i32RowCnt; i++)
	{
		stGV.row	= i;
		stGV.col	= 0;
		stGV.strText.Format(L"PH %d", i);
		m_pGridLed->SetItem(&stGV);
		m_pGridLed->SetRowHeight(i, 24);
	}

	/* ���� */
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
 desc : ���� ���� ��Ʈ�� ���� ����
 parm : None
 retn : None
*/
VOID CDlgExpo::UpdateCtrl()
{
	/* Job List ���� */
	UpdateJobList();
	/* Job Load ���� */
	UpdateJobLoad();
	/* Exposure ���� */
	UpdateExposure();
	/* LED Power ���� */
	UpdateLedAmpl();
}

/*
 desc : Job List ����
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

	/* �޸𸮿� ��ϵ� ���� �����鼭, ����Ʈ�� ��ϵǰ��� ������ Skip */
	if (pstJobMgt->job_regist_count < 1 && i32JobList < 1)	return;
	/* �޸𸮿� ��ϵ� ������ ����Ʈ �ڽ��� ��ϵ� ������ �ٸ��� Ȯ�� */
	if (i32JobList != pstJobMgt->job_regist_count)	bUpdateJobs	= TRUE;
	/* ���� �޸𸮿� ����Ʈ�� ��ϵ� ������ ������, Ȥ�� �Ź� �̸��� �ٸ��� �� �����Ƿ� */
	if (!bUpdateJobs)
	{
		for (i=0; i<pstJobMgt->job_regist_count; i++)
		{
			/* List Box�� ��ϵ� Gerber Name �������� */
			wmemset(tzGerberName, 0x00, MAX_GERBER_NAME);
			m_box_ctl[0].GetText(i, tzGerberName);
			/* ������ �׸��� ã������... ã�� ���� ���� ��Ű�� */
			if (pstJobMgt->IsJobOnlyFinded(csCnv.Uni2Ansi(tzGerberName)))	u8Find++;
		}
		/* ��� ������ �׸����� Ȯ�� / �������� ������, ����Ʈ �ڽ� ���� �ʿ� */
		if (u8Find != pstJobMgt->job_regist_count)	bUpdateJobs	= TRUE;
	}
	/* ������ �ʿ� ���ٸ�, ���� */
	if (!bUpdateJobs)	return;

	/* ------------------------ */
	/* Job List ��� Ȥ�� ����  */
	/* ------------------------ */

	/* ���� ��Ȱ��ȭ */
	m_box_ctl[0].SetRedraw(FALSE);
	/* ���� �׸� ��� ����� */
	m_box_ctl[0].ResetContent();
	/* ���� �޸𸮿� �ִ� �׸� ��� */
	for (i=0; i<pstJobMgt->job_regist_count; i++)
	{
		i32AddIndex	= m_box_ctl[0].AddString(csCnv.Ansi2Uni(pstJobMgt->job_name_list[i]));
		/* ���� ���õ� �׸����� Ȯ�� ��, ���õ� �׸��̸�, ���� �۾� ���� */
		if (0 == strcmp(pstJobMgt->job_name_list[i], pstJobMgt->selected_job_name))
		{
			m_box_ctl[0].SetCurSel(i32AddIndex);
		}
	}

	/* ���� Ȱ��ȭ */
	m_box_ctl[0].SetRedraw(TRUE);
	m_box_ctl[0].Invalidate(FALSE);
}

/*
 desc : Job Load ���� ����
 parm : None
 retn : None
*/
VOID CDlgExpo::UpdateJobLoad()
{
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;

	/* ���� ���õ� Job Name�� �����ϴ��� ���� */
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

	/* ���� ���°� Completed�̰� ���� (�ֱ�) ���°� Completed�� �ƴ϶�� ... */
	if (ENG_LSLS(pstJobMgt->selected_job_load_state) == ENG_LSLS::en_load_completed &&
		ENG_LSLS(pstJobMgt->selected_job_load_state) != m_enLastJobLoadStatus)
	{
		/* XML ���� ���� ���� */
		if (!uvEng_Luria_LoadSelectJobXML())
		{
			AfxMessageBox(L"Failed to load the currently selected gerber file", MB_ICONSTOP|MB_TOPMOST);
		}
	}

	/* ����, ���� �Ź��� ���� ���� ������ ���, ���������� Job Load State ��û */
	if ((pstJobMgt->selected_job_load_state == (UINT8)ENG_LSLS::en_load_not_started ||
		 pstJobMgt->selected_job_load_state == (UINT8)ENG_LSLS::en_loading) &&
		 (GetTickCount64() > m_u64TickLoadState + 250))
	{
		/* ���� ��û �ֱ� ������ �ð� �� �ӽ� ���� */
		m_u64TickLoadState	= GetTickCount64();
		/* Job Load State ��û */
		uvEng_Luria_ReqGetSelectedJobLoadState();
	}
}

/*
 desc : Exposure ���� ���� (Print Speed, Step Size, Frame Rate, Duty Cycle, Printing, Status, ...)
 parm : None
 retn : None
*/
VOID CDlgExpo::UpdateExposure()
{
	LPG_LDEW pstExpo	= &uvEng_ShMem_GetLuria()->exposure;
	UINT8 u8StateMask	= pstExpo->get_exposure_state[0] & 0x0f;

	/* ���� ���� �����ϸ�, �� �̻� ó������ ���� */
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

		/* ���� �ֱ� �� �ӽ� ���� */
		m_enExpoStatus	= (ENG_LPES)pstExpo->get_exposure_state[0];
	}
//	TRACE(L"cur=%d, tot=%d\n", pstExpo->get_exposure_state[1], pstExpo->get_exposure_state[2]);
	/* ���� ���� Running (Printing) �����̸�, Progress Bar ���� */
	if (0xA0 != (pstExpo->get_exposure_state[0] & 0xA0) &&
		0x01 == (pstExpo->get_exposure_state[0] & 0x01))
	{
		/* Progress Range ���� */
		m_pgs_ctl[0].SetRange(0, pstExpo->get_exposure_state[2]);
		/* ���� ����� �� ���� */
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

		/* ���� �ֱ� �� �ӽ� ���� */
		m_u32ExpoSpeed = pstExpo->get_exposure_speed;
	}

	/* ���� �뱤 ���°� �� �̻� ������ �ʿ䰡 ������ ���� */
	if (u8StateMask == 0x02 || u8StateMask == 0x0f)	m_bIsExpoUpdate	= FALSE;	/* �� �̻� �뱤 ���� ��û���� ���ƶ� */
	/* ���� �뱤 ���� ���� ��û ����̸�, ���� ���� ��û */
	if (m_bIsExpoUpdate)
	{
		if (m_u64TickExpoState + 250 < GetTickCount64())
		{
			/* ���� �뱤 ���� ���� ���� ��û */
			uvEng_Luria_ReqGetExposureState();
			/* ���� �ֱٿ� ��û�� �ð� �ӽ� ���� */
			m_u64TickExpoState	= GetTickCount64();
		}
	}
}

/*
 desc : LED Power ����
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

	/* ���� ���� �����ϸ� �������� ���� */
	for (; i<u8PHs && !bUpdate; i++)
	{
		if (0 != memcmp(m_pLedAmplitue[i], pstExpo->led_amplitude_power[i], sizeof(UINT16) * u8Led))
		{
			bUpdate = TRUE;
		}
	}
	/* ����� ���� ���ٸ� ... */
	if (!bUpdate)	return;

	/* ���� LED Power ���� �ٸ��� ���� */
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
 desc : List Box���� �Ź��� ������ ���, Luria Service���� �ش� ���õ� �Ź��� ���õǵ��� ó��
 parm : None
 retn : None
*/
VOID CDlgExpo::OnBoxSelchangeJobList()
{
	INT32 i32CurSel	= 0;
	TCHAR tzJobName[MAX_PATH_LEN]	= {NULL};
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;
	CUniToChar csCnv;

	/* List Box���� ���õ� �׸� �������� */
	i32CurSel	= m_box_ctl[0].GetCurSel();
	if (i32CurSel < 0)
	{
		AfxMessageBox(L"The selected item does not exist", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* ���õ� �׸��� ��ȿ���� �˻� */
	m_box_ctl[0].GetText(i32CurSel, tzJobName);
	if (wcslen(tzJobName) < 1)	return;
	/* ���� ���õ� Job Name�� �������� ���� Ȯ�� */
	if (0 == wcscmp(tzJobName, csCnv.Ansi2Uni(pstJobMgt->selected_job_name)))
	{
		AfxMessageBox(L"Job Name already selected", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* Luria�� ���õ� �Ź��� ���õǵ��� ��ɾ� ���� */
	if (!uvEng_Luria_ReqSelectedJobName(tzJobName, 0x00))
	{
		AfxMessageBox(L"Failed to selected the Job Name", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : ���ο� Job Name ���
 parm : None
 retn : None
*/
VOID CDlgExpo::JobMgtAdd()
{
	TCHAR tzJobName[MAX_PATH_LEN]	= {NULL}, tzRLTFiles[MAX_PATH_LEN] = {NULL};
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;
	CUniToChar csCnv;

	/* Job Name ���丮 ���� */
	if (!uvCmn_RootSelectPath(m_hWnd, uvEng_GetConfig()->set_comn.gerber_path, tzJobName))	return;
	/* Job Name ���� ���� ���丮�� �����ϴ��� Ȯ�� */
	if (uvCmn_GetChildPathCount(tzJobName) != 0)
	{
		AfxMessageBox(L"This is not a gerber directory", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* �ش� ���丮 ���� rlt_settings.xml ������ �����ϴ��� Ȯ�� */
	swprintf_s(tzRLTFiles, MAX_PATH_LEN, L"%s\\rlt_settings.xml", tzJobName);
	if (!uvCmn_FindFile(tzRLTFiles))
	{
		AfxMessageBox(L"File (rlt_settings.xml) does not exist", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* ������ ��ϵ� �Ź� ���� �߿� ������ �Ź��� �ִ��� ���� Ȯ�� */
	if (pstJobMgt->IsJobOnlyFinded(csCnv.Uni2Ansi(tzJobName)))
	{
		AfxMessageBox(L"Job Name already registered exist", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* Luria Service�� ��� �۾� ���� */
	if (!uvEng_Luria_ReqAddJobList(tzJobName))
	{
		AfxMessageBox(L"Failed to register Job Name", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : ���� Job Name ����
 parm : None
 retn : None
*/
VOID CDlgExpo::JobMgtDel()
{
	INT32 i32CurSel	= 0;
	TCHAR tzJobName[MAX_PATH_LEN]	= {NULL};
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;
	CUniToChar csCnv;

	/* List Box���� ���õ� �׸� �������� */
	i32CurSel	= m_box_ctl[0].GetCurSel();
	if (i32CurSel < 0)
	{
		AfxMessageBox(L"The selected item does not exist", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* ���õ� �׸��� ��ȿ���� �˻� */
	m_box_ctl[0].GetText(i32CurSel, tzJobName);
	if (wcslen(tzJobName) < 1)	return;
	if (0 != wcscmp(tzJobName, csCnv.Ansi2Uni(pstJobMgt->selected_job_name)))
	{
		AfxMessageBox(L"The gerber for removal is not selected in the optics", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* ���õ� �׸��� �Ź����� ���� */
	if (!uvEng_Luria_ReqSetDeleteSelectedJob())
	{
		AfxMessageBox(L"Failed to delete the selected Gerber Name", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* ���� ���µ� Job List ��û */
	uvEng_Luria_ReqGetJobList();
}

/*
 desc : ���� ���µ� Job Name ���� ����
 parm : None
 retn : None
*/
VOID CDlgExpo::JobMgtLoad()
{
	INT32 i32CurSel	= 0;
	TCHAR tzJobName[MAX_PATH_LEN]	= {NULL};
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;
	CUniToChar csCnv;

	/* List Box���� ���õ� �׸� �������� */
	i32CurSel	= m_box_ctl[0].GetCurSel();
	if (i32CurSel < 0)
	{
		AfxMessageBox(L"The selected item does not exist", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* ���õ� �׸��� ��ȿ���� �˻� */
	m_box_ctl[0].GetText(i32CurSel, tzJobName);
	if (wcslen(tzJobName) < 1)	return;
	if (0 != wcscmp(tzJobName, csCnv.Ansi2Uni(pstJobMgt->selected_job_name)))
	{
		AfxMessageBox(L"The gerber is not selected in the optics", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* ���� ���õ� Job�� ���� ���� ��û */
	if (!uvEng_Luria_ReqSetLoadSelectedJob())
	{
		AfxMessageBox(L"Failed to load the Gerber Name", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* ���� �뱤 ���� ���� ��û */
	if (!uvEng_Luria_ReqGetExposureFactor())
	{
		AfxMessageBox(L"Failed to request the Exposure Factor", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : Exposure (Print) ���� ����
 parm : None
 retn : None
*/
VOID CDlgExpo::ExpoParamSet()
{
	UINT8 u8StepSize	= (UINT8)m_edt_ctl[3].GetTextToNum();
	UINT8 u8DutyCycle	= (UINT8)m_edt_ctl[5].GetTextToNum();
	UINT16 u16FrameRate	= (UINT16)ROUNDED(m_edt_ctl[4].GetTextToDouble() * 1000.0f, 0);
	LPG_LDEW pstExpo	= &uvEng_ShMem_GetLuria()->exposure;

	/* ���� ���� ���� ���� ���� �ٸ��� �������� ���� */
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
 parm : mode	- [in]  Print ���� ��ɾ�
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

	/* ���� ���� �� ���� (Loaded)�� Job�� �ִ��� ��ȸ */
	if (!(pstJobMgt->IsJobNameSelected() && pstJobMgt->IsJobNameLoadCompleted()))
	{
		AfxMessageBox(L"Job Name is not selected or not loaded", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* �뱤 ���� */
	if (!uvEng_Luria_ReqSetPrintOpt(mode))
	{
		AfxMessageBox(L"Failed to set the print for exposure", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* �뱤 ���¸� �����϶�� �÷��� ���� */
	m_bIsExpoUpdate	= TRUE;
}

/*
 desc : ���� Photohead�� LED�� ������ Level Index �� ��û
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
 desc : Photohead�� LED�� ���ο� Level Index �� ����
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
