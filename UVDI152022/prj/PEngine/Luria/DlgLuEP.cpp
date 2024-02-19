
/*
 desc : Monitoring - Luria Service - Exposure (Printing)
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgLuEP.h"


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
CDlgLuEP::CDlgLuEP(UINT32 id, CWnd *parent)
	: CDlgLuria(id, parent)
{
	m_pGridAmpl	= NULL;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDlgLuEP::~CDlgLuEP()
{
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgLuEP::DoDataExchange(CDataExchange* dx)
{
	CDlgLuria::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Group box */
	u32StartID	= IDC_LUEP_GRP_PRINT_STATUS;
	for (i=0; i<5; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Static */
	u32StartID	= IDC_LUEP_TXT_PRINT_PROGRESS;
	for (i=0; i<7; i++)		DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Edit box */
	u32StartID	= IDC_LUEP_EDT_PRINT_STATUS;
	for (i=0; i<8; i++)		DDX_Control(dx, u32StartID+i,	m_edt_ctl[i]);
	/* Check box */
	u32StartID	= IDC_LUEP_CHK_SHARED_LOCAL;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
	/* Progress */
	u32StartID	= IDC_LUEP_PGS_PRINT;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_pgs_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgLuEP, CDlgLuria)
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgLuEP::PreTranslateMessage(MSG* msg)
{

	return CDlgLuria::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLuEP::OnInitDlg()
{
	if (!CDlgLuria::OnInitDlg())	return FALSE;

	/* ��Ʈ�� �ʱ�ȭ */
	InitCtrl();
	InitGrid();

#ifdef _DEBUG
	uvEng_Luria_ReqGetExposureFactor();
	uvEng_Luria_ReqGetLedAmplitude();
	uvEng_Luria_ReqGetLedDutyFrameRate();
	uvEng_Luria_ReqGetExposureState();
#endif
	/* ���� ��ü ȣ�� */
	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgLuEP::OnExitDlg()
{
	CDlgLuria::OnExitDlg();

	/* Grid Control �޸� ���� */
	if (m_pGridAmpl)
	{
		if (m_pGridAmpl->GetSafeHwnd())	m_pGridAmpl->DestroyWindow();
		delete m_pGridAmpl;
	}
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgLuEP::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgLuEP::OnResizeDlg()
{
}

/*
 desc : ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgLuEP::InitCtrl()
{
	UINT8 i	= 0x00;

	/* Groupbox */
	for (i=0; i<5; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* text - normal */
	for (i=0; i<7; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* Edit box */
	for (i=0; i<8; i++)
	{
		m_edt_ctl[i].SetEditFont(&g_lf);
		m_edt_ctl[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_ctl[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_ctl[i].SetInputType(EN_DITM::en_string);
		m_edt_ctl[i].SetReadOnly(TRUE);
		m_edt_ctl[i].SetMouseClick(FALSE);
	}
	/* Progress - normal */
	m_pgs_ctl[0].SetColor(RGB(160, 255, 0));
	/* check box - normal */
	for (i=0; i<2; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf);
		m_chk_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
		m_chk_ctl[i].SetReadOnly(TRUE);
	}
}

/*
 desc : �׸��� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgLuEP::InitGrid()
{
	TCHAR tzCols[5][8]	= { L"Item", L"365", L"385", L"395", L"405" };
	TCHAR tzRows[8][8]	= { L"PH 1", L"PH 2", L"PH 3", L"PH 4", L"PH 5", L"PH 6", L"PH 7", L"PH 8" };
	INT32 i32Width[5]	= { 46, 44, 44, 44, 44 }, i, j;
	INT32 i32ColCnt		= 5, i32RowCnt = uvEng_Conf_GetLuria()->ph_count;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* Last Recv. Packet / PDO Header / PDO Data (Values) */
	::GetWindowRect(m_grp_ctl[3].GetSafeHwnd(), rGrid);
	ScreenToClient(rGrid);
	rGrid.left	+= 15;
	rGrid.right	-= 15;
	rGrid.top	+= 22;
	rGrid.bottom-= 13;

	/* �׸��� ��Ʈ�� �⺻ ���� �Ӽ� */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* ��ü ���� */
	m_pGridAmpl = new CGridCtrl;
	ASSERT(m_pGridAmpl);
	m_pGridAmpl->SetDrawScrollBar(1);
	m_pGridAmpl->SetDrawScrollBarHorz(0);
	m_pGridAmpl->SetDrawScrollBarVert(1);
	if (!m_pGridAmpl->Create(rGrid, this, IDC_GRID_EXPO_AMPL, dwStyle))
	{
		delete m_pGridAmpl;
		m_pGridAmpl = NULL;
		return;
	}
	/* ��ü �⺻ ���� */
	m_pGridAmpl->SetLogFont(g_lf);
	m_pGridAmpl->SetRowCount(i32RowCnt+1);
	m_pGridAmpl->SetColumnCount(i32ColCnt);
	m_pGridAmpl->SetFixedRowCount(1);
	m_pGridAmpl->SetRowHeight(0, 24);
	m_pGridAmpl->SetFixedColumnCount(0);
	m_pGridAmpl->SetBkColor(RGB(255, 255, 255));
	m_pGridAmpl->SetFixedColumnSelection(0);

	/* Ÿ��Ʋ (ù ��° ��) */
	stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	stGV.crBkClr = RGB(214, 214, 214);
	stGV.crFgClr = RGB(0, 0, 0);
	for (i=0; i<i32ColCnt; i++)
	{
		stGV.nFormat= DT_VCENTER | DT_SINGLELINE | DT_CENTER;
		stGV.row	= 0;
		stGV.col	= i;
		stGV.strText= tzCols[i];
		m_pGridAmpl->SetItem(&stGV);
		m_pGridAmpl->SetColumnWidth(i, i32Width[i]);
//		m_pGridAmpl->SetItemFont(0, i, &g_lf);
	}
	/* ���� (1 ~ Rows) ���� ���� */
	stGV.crBkClr = RGB(255, 255, 255);
	for (i=1; i<=i32RowCnt; i++)
	{
		m_pGridAmpl->SetRowHeight(i, 24);
		for (j=0; j<5; j++)
		{
			stGV.nFormat= DT_VCENTER | DT_SINGLELINE;
			if (j == 0)
			{
				stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
				stGV.strText= tzRows[i-1];
			}
			else
			{
				stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
				stGV.strText= L"";
			}
			stGV.row	= i;
			stGV.col	= j;
			m_pGridAmpl->SetItem(&stGV);
//			m_pGridAmpl->SetItemFont(i, j, &g_lf);
		}
	}

	m_pGridAmpl->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGridAmpl->UpdateGrid();
}

/*
 desc : ���� ���� ��Ʈ�� ���� ����
 parm : None
 retn : None
*/
VOID CDlgLuEP::UpdateCtrl()
{
	UINT8 i	= 0x00, j;
	TCHAR tzVal[64]	= {NULL}, tzStr[64] = {NULL}, tzEdt[1024] = {NULL};
	LPG_LDEW pstEW	= &uvEng_ShMem_GetLuria()->exposure;	/* Luria Shared Memory */
	LPG_LDJM pstJM	= &uvEng_ShMem_GetLuria()->jobmgt;		/* Luria Shared Memory */
	LPG_LDPP pstPP	= &uvEng_ShMem_GetLuria()->panel;		/* Luria Shared Memory */
	LPG_LDMC pstMC	= &uvEng_ShMem_GetLuria()->machine;		/* Luria Shared Memory */
	CUniToChar csCnv;

	/* Expose (Printing) Rate */
	switch (pstEW->get_exposure_state[0])
	{
	case ENG_LPES::en_print_running		:
	case ENG_LPES::en_simul_running		:	m_pgs_ctl[0].SetRange(1, pstEW->get_exposure_state[2]);
										m_pgs_ctl[0].SetPos(pstEW->get_exposure_state[1]);
										m_edt_ctl[0].SetTextToStr(L"Print running", TRUE);		break;
	case ENG_LPES::en_print_success		:
	case ENG_LPES::en_simul_success		:	m_pgs_ctl[0].SetRange(1, pstEW->get_exposure_state[2]);
										m_pgs_ctl[0].SetPos(pstEW->get_exposure_state[2]);
										m_edt_ctl[0].SetTextToStr(L"Print successful", TRUE);	break;
	case ENG_LPES::en_print_failed		:
	case ENG_LPES::en_simul_failed		:	m_edt_ctl[0].SetTextToStr(L"Print failed", TRUE);		break;
	case ENG_LPES::en_abort_in_progress	:	m_edt_ctl[0].SetTextToStr(L"Print aboring", TRUE);		break;
	case ENG_LPES::en_abort_success		:	m_edt_ctl[0].SetTextToStr(L"Abort success", TRUE);		break;
	case ENG_LPES::en_abort_failed		:	m_edt_ctl[0].SetTextToStr(L"Abort failed", TRUE);		break;
	}

	/* Shared Local Zone & Global Rectangle Lock */
	m_chk_ctl[0].SetCheck(pstPP->use_shared_local_zones);
	m_chk_ctl[1].SetCheck(pstPP->global_rectangle_lock);

	/* Job Path */
	m_edt_ctl[1].SetTextToStr(csCnv.Ansi2Uni(pstJM->selected_job_name));

	/* Registration Points - Global / Local */
	if (pstJM->selected_job_fiducial.g_coord_count > 0)
	{
		for (i=0; i<pstJM->selected_job_fiducial.g_coord_count-1; i++)
		{
			swprintf_s(tzVal, 64, L"%7.4f", pstJM->selected_job_fiducial.g_coord_xy[i].x / 1000000.0f);
			swprintf_s(tzStr, 64, L"%9s", tzVal);	wcscat_s(tzEdt, 1024, tzStr);	wcscat_s(tzEdt, 1024, L" |");
			swprintf_s(tzVal, 64, L"%7.4f",	pstJM->selected_job_fiducial.g_coord_xy[i].y / 1000000.0f);
			swprintf_s(tzStr, 64, L"%9s", tzVal);	wcscat_s(tzEdt, 1024, tzStr);	wcscat_s(tzEdt, 1024, L" mm\r\n");
		}
		if (pstJM->selected_job_fiducial.g_coord_count > 0)
		{
			swprintf_s(tzVal, 64, L"%7.4f", pstJM->selected_job_fiducial.g_coord_xy[i].x / 1000000.0f);
			swprintf_s(tzStr, 64, L"%9s", tzVal);	wcscat_s(tzEdt, 1024, tzStr);	wcscat_s(tzEdt, 1024, L" |");
			swprintf_s(tzVal, 64, L"%7.4f",	pstJM->selected_job_fiducial.g_coord_xy[i].y / 1000000.0f);
			swprintf_s(tzStr, 64, L"%9s", tzVal);	wcscat_s(tzEdt, 1024, tzStr);	wcscat_s(tzEdt, 1024, L" mm");
			if (pstJM->selected_job_fiducial.l_coord_count > 0)	wcscat_s(tzEdt, 1024, L"\r\n");
		}
	}
	if (pstJM->selected_job_fiducial.l_coord_count > 0)
	{
		for (i=0; i<pstJM->selected_job_fiducial.l_coord_count-1; i++)
		{
			swprintf_s(tzVal, 64, L"%7.4f", pstJM->selected_job_fiducial.l_coord_xy[i].x / 1000000.0f);
			swprintf_s(tzStr, 64, L"%9s", tzVal);	wcscat_s(tzEdt, 1024, tzStr);	wcscat_s(tzEdt, 1024, L" |");
			swprintf_s(tzVal, 64, L"%7.4f",	pstJM->selected_job_fiducial.l_coord_xy[i].y / 1000000.0f);
			swprintf_s(tzStr, 64, L"%9s", tzVal);	wcscat_s(tzEdt, 1024, tzStr);	wcscat_s(tzEdt, 1024, L" mm\r\n");
		}
		if (pstJM->selected_job_fiducial.l_coord_count > 0)
		{
			swprintf_s(tzVal, 64, L"%7.4f", pstJM->selected_job_fiducial.l_coord_xy[i].x / 1000000.0f);
			swprintf_s(tzStr, 64, L"%9s", tzVal);	wcscat_s(tzEdt, 1024, tzStr);	wcscat_s(tzEdt, 1024, L" |");
			swprintf_s(tzVal, 64, L"%7.4f",	pstJM->selected_job_fiducial.l_coord_xy[i].y / 1000000.0f);
			swprintf_s(tzStr, 64, L"%9s", tzVal);	wcscat_s(tzEdt, 1024, tzStr);	wcscat_s(tzEdt, 1024, L" mm");
		}
	}
	m_edt_ctl[2].SetTextToStr(tzEdt, TRUE);

	/* Print Speed */
	swprintf_s(tzVal, 64, L"%.3f mm/s", pstEW->get_exposure_speed/1000.0f);
	m_edt_ctl[3].SetTextToStr(tzVal, TRUE);
	/* Scroll Step Size */
	m_edt_ctl[4].SetTextToNum(pstEW->scroll_step_size, TRUE);
	/* Frame Rate Factor */
	m_edt_ctl[5].SetTextToNum(pstEW->frame_rate_factor/1000.0f, 3, TRUE);
	/* LED Duty Cycle */
	swprintf_s(tzVal, 64, L"%u %%", pstEW->led_duty_cycle);
	m_edt_ctl[6].SetTextToStr(tzVal, TRUE);

	/* Job Status */
	switch (pstJM->selected_job_load_state)
	{
	case ENG_LSLS::en_load_none			:	m_edt_ctl[7].SetTextToStr(L"None",			TRUE);	break;
	case ENG_LSLS::en_load_not_started	:	m_edt_ctl[7].SetTextToStr(L"Not started",	TRUE);	break;
	case ENG_LSLS::en_loading			:	m_edt_ctl[7].SetTextToStr(L"Loading",		TRUE);	break;
	case ENG_LSLS::en_load_completed	:	m_edt_ctl[7].SetTextToStr(L"Job loaded",	TRUE);	break;
	case ENG_LSLS::en_load_failed		:	m_edt_ctl[7].SetTextToStr(L"Failed",		TRUE);	break;
	}

	/* LED Power Index */
	m_pGridAmpl->SetRedraw(FALSE);
	for (i=0; i<uvEng_Conf_GetLuria()->ph_count; i++)
	{
		for (j=0; j<uvEng_Conf_GetLuria()->led_count; j++)
		{
			m_pGridAmpl->SetItemText(i+1, j+1, UINT(pstEW->led_amplitude_power[i][j]));
		}
	}
	m_pGridAmpl->SetRedraw(TRUE);
	m_pGridAmpl->Invalidate(FALSE);
}
