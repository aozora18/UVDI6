
/*
 desc : Main Dialog
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgMain.h"

#include "MainThread.h"

#define ONE_SCAN_WIDTH	20.6f	/* unit : mm */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : �ڽ��� ���� ID, �θ� ���� ��ü
 retn : None
*/
CDlgMain::CDlgMain(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgMain::IDD, parent)
{
	m_bDrawBG				= 0x01;
	m_hIcon					= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_u8ViewMode			= 0x00;
	m_u64DrawTime			= 0;
	m_bMotionStop			= FALSE;
	m_bLiveSaved			= FALSE;
	m_pMainThread			= NULL;
	m_u64TickLoadState		= 0;
	m_pLedAmplitue			= NULL;
	m_enLastJobLoadStatus	= ENG_LSLS::en_load_none;
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgMain::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;
	/* group box */
	u32StartID	= IDC_MAIN_GRP_MARK_VIEW;
	for (i=0; i<13; i++)	DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* list box */
	u32StartID	= IDC_MAIN_BOX_JOBS_LIST;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_box_ctl[i]);
	/* For Image */
	u32StartID	= IDC_MAIN_PIC_MARK_VIEW_11;
	for (i=0; i<5; i++)		DDX_Control(dx, u32StartID+i,	m_pic_ctl[i]);
	/* Check - acam no */
	u32StartID	= IDC_MAIN_CHK_ACAM_NO_11;
	for (i=0; i<4; i++)		DDX_Control(dx, u32StartID+i,	m_chk_cam[i]);
	/* Check - moving to mark */
	u32StartID	= IDC_MAIN_CHK_MARK_1;
	for (i=0; i<4; i++)		DDX_Control(dx, u32StartID+i,	m_chk_mak[i]);
	/* Check - mc2 */
	u32StartID	= IDC_MAIN_CHK_MC2_DRV_0;
	for (i=0; i<8; i++)		DDX_Control(dx, u32StartID+i,	m_chk_drv[i]);
	/* Check - Align Method */
	u32StartID	= IDC_MAIN_CHK_EXPO_DIRECT;
	for (i=0; i<3; i++)		DDX_Control(dx, u32StartID+i,	m_chk_mtd[i]);
	/* Check - live */
	u32StartID	= IDC_MAIN_CHK_ACAM_LIVE;
	for (i=0; i<3; i++)		DDX_Control(dx, u32StartID+i,	m_chk_liv[i]);
	/* Button - Normal */
	u32StartID	= IDC_MAIN_BTN_MOVE_ACAM;
	for (i=0; i<37; i++)	DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Static - Normal */
	u32StartID	= IDC_MAIN_TXT_ACAM_ABS;
	for (i=0; i<17; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Edit - integer */
	u32StartID	= IDC_MAIN_EDT_EXPO_STEP;
	for (i=0; i<4; i++)		DDX_Control(dx, u32StartID+i,	m_edt_int[i]);
	/* edit - float */
	u32StartID	= IDC_MAIN_EDT_MC2_DIST;
	for (i=0; i<6; i++)		DDX_Control(dx, u32StartID+i,	m_edt_flt[i]);
	/* edit - float */
	u32StartID	= IDC_MAIN_EDT_MARK_RESULT_11;
	for (i=0; i<4; i++)		DDX_Control(dx, u32StartID+i,	m_edt_rlt[i]);
	/* edit - text */
	u32StartID	= IDC_MAIN_EDT_JOBS_STATUS;
	for (i=0; i<3; i++)		DDX_Control(dx, u32StartID+i,	m_edt_txt[i]);
}

BEGIN_MESSAGE_MAP(CDlgMain, CMyDialog)
	ON_MESSAGE(WM_MAIN_THREAD,	OnMsgMainThread)
	ON_WM_SYSCOMMAND()
	ON_LBN_SELCHANGE(IDC_MAIN_BOX_JOBS_LIST, OnBoxSelchangeJobList)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_BTN_MOVE_ACAM, IDC_MAIN_BTN_APP_EXIT,		OnBtnClicked)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_CHK_ACAM_NO_11, IDC_MAIN_CHK_ACAM_LIVE,	OnChkClicked)
END_MESSAGE_MAP()

/*
 desc : �ý��� ��ɾ� ����
 parm : id		- ���õ� �׸� ID ��
		lparam	- ???
 retn : 1 - ���� / 0 - ����
*/
VOID CDlgMain::OnSysCommand(UINT32 id, LPARAM lparam)
{
//	switch (id)
//	{
//	}

	CMyDialog::OnSysCommand(id, lparam);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::OnInitDlg()
{
	UINT32 u32Size	= 0, i = 0;

	/* ���̺귯�� �ʱ�ȭ */
	if (!InitLib())		return FALSE;

	InitMem();		/* �ʱ� �޸� �Ҵ� */
	InitCtrl();		/* ���� ��Ʈ�� �ʱ�ȭ */
	LoadParam();	/* ���� �ֱٿ� ������ �� �ҷ����� */
	InitGridPos();	/* �׸��� �ʱ�ȭ */
	InitGridLed();	/* �׸��� �ʱ�ȭ */
	InitSetup();

	/* Main Thread ���� (!!! �߿�. InitLib �� �����ϸ�, �ݵ�� �����ؾ� �� !!!) */
	m_pMainThread	= new CMainThread(m_hWnd);
	ASSERT(m_pMainThread);
	if (!m_pMainThread->CreateThread(0, NULL, NORMAL_THREAD_SPEED))
	{
		delete m_pMainThread;
		m_pMainThread	= NULL;
	}

	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgMain::OnExitDlg()
{
	UINT32 i	= 0;
	/* �⺻ ���� ������ �޸� ���� */
	if (m_pMainThread)
	{
		m_pMainThread->Stop();
		while (m_pMainThread->IsBusy())	m_pMainThread->Sleep();
		delete m_pMainThread;
		m_pMainThread	= NULL;
	}
	/* �޸� ���� */
	if (m_pLedAmplitue)
	{
		for (; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
		{
			::Free(m_pLedAmplitue[i]);
		}
		::Free(m_pLedAmplitue);
	}
	/* ���̺귯�� ���� */
	CloseLib();
	/* Grid Control �޸� ����*/
	CloseGrid();
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgMain::OnPaintDlg(CDC *dc)
{
	RECT rDraw;
	ENG_VCCM enMode	= uvEng_Camera_GetCamMode();

	/* �̹����� ��µ� ���� ���� */
	m_pic_ctl[4].GetWindowRect(&rDraw);
	ScreenToClient(&rDraw);

	/* ���� ��ũ ���� �˻� ����� ��� */
	switch (enMode)
	{
	case ENG_VCCM::en_live_mode	:	uvEng_Camera_DrawLive(dc->m_hDC, rDraw, FALSE);	break;
	case ENG_VCCM::en_grab_mode	:	uvEng_Camera_DrawGrab(dc->m_hDC, rDraw, GetCheckACam(), TRUE);	break;
	}
#if 0
	if (uvEng_Camera_GetGrabbedCount() == 0x04)
	{
		ENG_BWOK enJobID	= m_pMainThread->GetWorkJobID();
		switch (enJobID)
		{
		case ENG_BWOK::en_mark_test	:
		case ENG_BWOK::en_expo_mark	:
		case ENG_BWOK::en_expo_cali	:	UpdateMarkExpoResult();	break;
		}
	}
#endif
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgMain::OnResizeDlg()
{
}

/*
 desc : Library Init
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::InitLib()
{
	return uvEng_Init(ENG_ERVM::en_cmps_sw);
}

/*
 desc : Library Close
 parm : None
 retn : None
*/
VOID CDlgMain::CloseLib()
{
	uvEng_Close();
}

/*
 desc : �޸� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgMain::InitMem()
{
	UINT8 i		= 0x00;
	UINT8 u8PHs	= uvEng_GetConfig()->luria_svc.ph_count;
	UINT8 u8LED	= uvEng_GetConfig()->luria_svc.led_count;

	/* Photohead LED Power ���� �ӽ� ���� */
	m_pLedAmplitue	= (UINT16**)::Alloc(u8PHs * sizeof(PUINT16));
	for (; i<u8PHs; i++)
	{
		m_pLedAmplitue[i]	= (PUINT16)::Alloc(sizeof(UINT16) * u8LED);
		memset(m_pLedAmplitue[i], 0x00, sizeof(UINT16) * u8LED);
	}
}

/*
 desc : ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgMain::InitCtrl()
{
	INT32 i;

	/* group box */
	for (i=0; i<13; i++)	m_grp_ctl[i].SetFont(&g_lf);
	/* List box - normal */
	for (i=0; i<1; i++)		m_box_ctl[i].SetLogFont(&g_lf);
	/* text - normal */
	for (i=0; i<17; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* button - normal */
	for (i=0; i<37; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
	}
	/* checkbox */
	for (i=0; i<4; i++)
	{
		m_chk_cam[i].SetLogFont(g_lf);
		m_chk_cam[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
	for (i=0; i<4; i++)
	{
		m_chk_mak[i].SetLogFont(g_lf);
		m_chk_mak[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
	for (i=0; i<8; i++)
	{
		m_chk_drv[i].SetLogFont(g_lf);
		m_chk_drv[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
	for (i=0; i<4; i++)
	{
		m_chk_mtd[i].SetLogFont(g_lf);
		m_chk_mtd[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
	for (i=0; i<1; i++)
	{
		m_chk_liv[i].SetLogFont(g_lf);
		m_chk_liv[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
	/* edit - integer */
	for (i=0; i<4; i++)
	{
		m_edt_int[i].SetEditFont(&g_lf);
		m_edt_int[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_int[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_int[i].SetInputType(EN_DITM::en_int);
	}
	/* edit - float */
	for (i=0; i<6; i++)
	{
		m_edt_flt[i].SetEditFont(&g_lf);
		m_edt_flt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_flt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_flt[i].SetInputType(EN_DITM::en_float);
	}
	/* edit - float */
	for (i=0; i<4; i++)
	{
		m_edt_rlt[i].SetEditFont(&g_lf);
		m_edt_rlt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_rlt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_rlt[i].SetInputType(EN_DITM::en_float);
	}
	/* edit - text */
	for (i=0; i<3; i++)
	{
		m_edt_txt[i].SetEditFont(&g_lf);
		m_edt_txt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_txt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_txt[i].SetInputType(EN_DITM::en_string);
	}
}

/*
 desc : �ʱ� �� ����
 parm : None
 retn : None
*/
VOID CDlgMain::InitSetup()
{
	UINT32 i	= 0;
	LPG_CMSI pstMC2	= &uvEng_GetConfig()->mc2_svc;
	LPG_CSCI pstCAM	= &uvEng_GetConfig()->set_cams;

	/* �ϴ� ��� Disable */
	for (i=0x00; i<MAX_MC2_DRIVE; i++)		m_chk_drv[i].EnableWindow(FALSE);
	/* for MC2 - üũ ��ư Ȱ��ȭ ó�� */
	for (i=0; i<pstMC2->drive_count; i++)	m_chk_drv[pstMC2->axis_id[i]].EnableWindow(TRUE);
	/* Align Camera */
	for (i=0; i<pstCAM->acam_count; i++)	m_chk_cam[pstCAM->acam_count+i].EnableWindow(FALSE);
	/* Read Only */
//	for (i=0; i<3; i++)	m_edt_int[i].SetMouseClick(FALSE);
//	for (i=0; i<6; i++)	m_edt_flt[i].SetMouseClick(FALSE);
	for (i=0; i<4; i++)	m_edt_rlt[i].SetMouseClick(FALSE);
	for (i=0; i<1; i++)	m_edt_txt[i].SetMouseClick(FALSE);

	/* �뱤 ��� ���� */
	m_chk_mtd[0].SetCheck(1);
	for (i=0; i<3; i++)	m_chk_mtd[i].EnableWindow(FALSE);
	/* ��ũ �̵� ��ȣ ���� */
	m_chk_mak[0].SetCheck(1);
	/* ����� ī�޶� ���� */
	m_chk_cam[0].SetCheck(1);

	/* �⺻ �� ���� (�ӵ�, �Ÿ�) */
	m_edt_flt[0].SetTextToNum(uvEng_GetConfig()->mc2_svc.move_dist, 4);
	m_edt_flt[1].SetTextToNum(uvEng_GetConfig()->mc2_svc.move_velo, 4);

	/* ���� ȯ�濡 �´� �˻� �� ��� */
	SetRegistModel();

	/* ���� �뱤 ���� ���� */
	uvEng_Camera_SetAlignMode(ENG_AOEM::en_direct_expose);
}

/*
 desc : �׸��� ��Ʈ�� ����
 parm : None
 retn : None
*/
VOID CDlgMain::InitGridPos()
{
	TCHAR tzCols[3][16]	= { L"Item", L"Pos (mm)" }, tzRows[32] = {NULL};
	INT32 i32Width[3]	= { 55, 85 }, i, j=1;
	INT32 i32ColCnt		= 2, i32RowCnt = 0;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	LPG_CIEA pstCfg		= uvEng_GetConfig();
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* Last Recv. Packet / PDO Header / PDO Data (Values) */
	::GetWindowRect(m_grp_ctl[2].GetSafeHwnd(), rGrid);
	ScreenToClient(rGrid);
	rGrid.left	+= 15;
	rGrid.right	-= 15;
	rGrid.top	+= 22;
	rGrid.bottom-= 13;

	/* ------------------ */
	/* for Motor Position */
	/* ------------------ */

	/* �� ��µ� �� (Row)�� ���� ���ϱ� */
	i32RowCnt	= pstCfg->luria_svc.ph_count + pstCfg->mc2_svc.drive_count;
	/* Basler ī�޶��� ��� Z Axis�� ��ġ (����) �� ��� ���� */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)pstCfg->set_comn.align_camera_kind)
	{
		/* Align Camera�� Z Axis ���� ���� ��ġ �� ���� */
		m_u32ACamZAxisRow	= i32RowCnt;
		/* Motion Left / Right ��ġ�� Up / Down ���� ��ġ */
		i32RowCnt	+= pstCfg->set_cams.acam_count;
	}

	/* �׸��� ��Ʈ�� �⺻ ���� �Ӽ� */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* ��ü ���� */
	m_pGridPos	= new CGridCtrl;
	ASSERT(m_pGridPos);
	m_pGridPos->SetModifyStyleEx(WS_EX_STATICEDGE);
	m_pGridPos->SetDrawScrollBarHorz(FALSE);
	m_pGridPos->SetDrawScrollBarVert(TRUE);
	if (!m_pGridPos->Create(rGrid, this, IDC_MAIN_GRID_MOTOR_POS, dwStyle))
	{
		delete m_pGridPos;
		m_pGridPos = NULL;
		return;
	}
	/* ��ü �⺻ ���� */
	m_pGridPos->SetLogFont(g_lf);
	m_pGridPos->SetRowCount(i32RowCnt+1);
	m_pGridPos->SetColumnCount(i32ColCnt);
	m_pGridPos->SetFixedRowCount(1);
	m_pGridPos->SetRowHeight(0, 24);
	m_pGridPos->SetFixedColumnCount(0);
	m_pGridPos->SetBkColor(RGB(255, 255, 255));
	m_pGridPos->SetFixedColumnSelection(0);

	/* Ÿ��Ʋ (ù ��° ��) */
	stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	stGV.crBkClr = RGB(214, 214, 214);
	stGV.crFgClr = RGB(0, 0, 0);
	stGV.lfFont	= g_lf;
	for (i=0; i<i32ColCnt; i++)
	{
		stGV.nFormat= DT_VCENTER | DT_SINGLELINE | DT_CENTER;
		stGV.row	= 0;
		stGV.col	= i;
		stGV.strText= tzCols[i];
		m_pGridPos->SetItem(&stGV);
		m_pGridPos->SetColumnWidth(i, i32Width[i]);
	}

	/* ���� ���� */
	stGV.crBkClr = RGB(255, 255, 255);

	/* ���� ���� - for Photohead Position */
	for (i=0; i<pstCfg->luria_svc.ph_count; i++,j++)
	{
		m_pGridPos->SetRowHeight(j, 24);

		/* for Subject */
		stGV.strText.Format(L"PH %d", i+1);
		stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
		stGV.row	= j;
		stGV.col	= 0;
		m_pGridPos->SetItem(&stGV);

		/* for Subject */
		stGV.strText= L"0.0000";
		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.col	= 1;
		m_pGridPos->SetItem(&stGV);
	}

	/* ���� ���� - for Motion Drive Position */
	for (i=0; i<pstCfg->mc2_svc.drive_count; i++,j++)
	{
		m_pGridPos->SetRowHeight(j, 24);

		/* for Subject */
		stGV.strText.Format(L"MC %d", pstCfg->mc2_svc.axis_id[i]);
		stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
		stGV.row	= j;
		stGV.col	= 0;
		m_pGridPos->SetItem(&stGV);

		/* for Subject */
		stGV.strText= L"0.0000";
		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.col	= 1;
		m_pGridPos->SetItem(&stGV);
	}

	/* ���� ���� - for Basler Camera Position */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)pstCfg->set_comn.align_camera_kind)
	{
		/* Z Axis Up / Down Postion */
		for (i=0; i<pstCfg->set_cams.acam_count; i++,j++)
		{
			m_pGridPos->SetRowHeight(j, 24);

			/* for Subject */
			stGV.strText.Format(L"AZ %d", i+1);
			stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
			stGV.row	= j;
			stGV.col	= 0;
			m_pGridPos->SetItem(&stGV);

			/* for Subject */
			stGV.strText= L"0.0000";
			stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
			stGV.col	= 1;
			m_pGridPos->SetItem(&stGV);
		}
	}

	m_pGridPos->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGridPos->UpdateGrid();
}

/*
 desc : ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgMain::InitGridLed()
{
	TCHAR tzCols[5][8]	= { L"Item", L"LED1", L"LED2", L"LED3", L"LED4" }, tzRows[32] = {NULL};
	INT32 i32Width[5]	= { 48, 48, 48, 48, 48 }, i, j;
	INT32 i32ColCnt		= 5, i32RowCnt = uvEng_GetConfig()->luria_svc.ph_count;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* Last Recv. Packet / PDO Header / PDO Data (Values) */
	m_grp_ctl[6].GetWindowRect(rGrid);
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
	m_pGridLed->SetModifyStyleEx(WS_EX_STATICEDGE);
	m_pGridLed->SetDrawScrollBarHorz(FALSE);
	m_pGridLed->SetDrawScrollBarVert(TRUE);
	if (!m_pGridLed->Create(rGrid, this, IDC_MAIN_GRID_PH_LED, dwStyle))
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
	stGV.crBkClr = RGB(239, 239, 239);
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
 desc : �׸��� ��Ʈ�� ����
 parm : None
 retn : None
*/
VOID CDlgMain::CloseGrid()
{
	if (m_pGridPos)
	{
		if (m_pGridPos->GetSafeHwnd())	m_pGridPos->DestroyWindow();
		delete m_pGridPos;
	}
	if (m_pGridLed)
	{
		if (m_pGridLed->GetSafeHwnd())	m_pGridLed->DestroyWindow();
		delete m_pGridLed;
	}
}

/*
 desc : List Box���� �Ź��� ������ ���, Luria Service���� �ش� ���õ� �Ź��� ���õǵ��� ó��
 parm : None
 retn : None
*/
VOID CDlgMain::OnBoxSelchangeJobList()
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
		AfxMessageBox(L"Job Name already selected", MB_ICONINFORMATION);
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
 desc : Main Thread���� �߻��� �̺�Ʈ ó��
 parm : wparam	- [in]  Event ID
		lparam	- [in]  Event Data
 retn : 0L
*/
LRESULT CDlgMain::OnMsgMainThread(WPARAM wparam, LPARAM lparam)
{
	/* �ֱ������� ȣ���� */
	switch (wparam)
	{
	case MSG_WORK_PERIOD		:	UpdatePeriod();		break;
#if 0
	case MSG_WORK_COMPLETED		:	WorkCompleted();	break;
	case MSG_WORK_ALIGN_RESULT	:	WorkAlignResult();	break;
	case MSG_WORK_ERROR			:	WorkErrorJobName();	break;
#endif
	}

	return 0L;
}

/*
 desc : �θ� �����忡 ���� �ֱ������� ȣ���
 parm : None
 retn : None
*/
VOID CDlgMain::UpdatePeriod()
{
	/* Drive Position */
	UpdateMotorPos();
	/* Job List ���� */
	UpdateJobList();
	/* Job Load ���� */
	UpdateJobLoad();
	/* Exposure ���� */
	UpdateExposure();
	/* LED Power ���� */
	UpdateLedAmpl();
	/* Update Live ���� */
	UpdateLiveView();
	/* ��� ��Ʈ���� �����̴� ���ο� ���� ��ư Enable / Disable */
//	EnableCtrl();
	/* Update Job Rate */
//	UpdateJobRate();
}
#if 0
/*
 desc : �۾� �Ϸ� ������ ���
 parm : None
 retn : None
*/
VOID CDlgMain::WorkCompleted()
{
	switch (m_pMainThread->GetWorkJobID())
	{
	case ENG_BWOK::en_mark_move	:	;	break;
	case ENG_BWOK::en_mark_grab	:	UpdateMarkGrabResult();	break;
	case ENG_BWOK::en_mark_test	:
	case ENG_BWOK::en_expo_mark	:
	case ENG_BWOK::en_expo_cali	:	;	break;
	}
	m_edt_txt[1].SetTextToStr(L"Completed", TRUE);
}

/*
 desc : ����� �۾� �Ϸ� ������ ���
 parm : None
 retn : None
*/
VOID CDlgMain::WorkAlignResult()
{
	switch (m_pMainThread->GetWorkJobID())
	{
	case ENG_BWOK::en_mark_test	:
	case ENG_BWOK::en_expo_mark	:
	case ENG_BWOK::en_expo_cali	:	UpdateMarkExpoResult();	break;
	}
}

/*
 desc : �۾� ���� ������ ���
 parm : None
 retn : None
*/
VOID CDlgMain::WorkErrorJobName()
{
	if (m_pMainThread->GetErrorJobName())
	{
		m_edt_txt[1].SetTextToStr(m_pMainThread->GetErrorJobName(), TRUE);
	}
}
#endif
/*
 desc : ���� ��� ���� Motor Position (mm) �� ����
 parm : None
 retn : None
 note : Photohead Z Axis, Motion Drive, Align Camera Z Axis ���
*/
VOID CDlgMain::UpdateMotorPos()
{
	UINT8 i	= 0x00, j = 1;
	DOUBLE dbPos[2]	= {NULL};
	LPG_CIEA pstCfg	= uvEng_GetConfig();
	LPG_LDDP pstDP	= &uvEng_ShMem_GetLuria()->directph;
	LPG_MDSM pstMC2	= uvEng_ShMem_GetMC2();

	/* Luria Photohead ��ġ */
	for (i=0; i<pstCfg->luria_svc.ph_count; i++, j++)
	{
		dbPos[0] = m_pGridPos->GetItemTextToFloat(j, 1);
		dbPos[1] = (DOUBLE)ROUNDED(pstDP->focus_motor_move_abs_position[i]/1000.0f, 3);
		if (dbPos[0] != dbPos[1])
		{
			m_pGridPos->SetItemText(j, 1, dbPos[1]);
			m_pGridPos->InvalidateRowCol(j, 1);
		}
	}

	/* MC2 SD2S Drive ��ġ (Basler Camera ����) */
	for (i=0; i<pstCfg->mc2_svc.drive_count; i++, j++)
	{
		dbPos[0] = m_pGridPos->GetItemTextToFloat(j, 1);
		dbPos[1] = (DOUBLE)ROUNDED(pstMC2->act_data[pstCfg->mc2_svc.axis_id[i]].real_position/10000.0f, 4);
		if (dbPos[0] != dbPos[1])
		{
			m_pGridPos->SetItemText(j, 1, dbPos[1]);
			m_pGridPos->InvalidateRowCol(j, 1);
		}
	}

	/* Align Camera ��ġ (Basler ī�޶��� ��츸 �ش�) */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)pstCfg->set_comn.align_camera_kind)
	{
		for (i=0; i<pstCfg->set_cams.acam_count; i++, j++)
		{
			dbPos[0] = m_pGridPos->GetItemTextToFloat(j, 1);
			dbPos[1] = (DOUBLE)ROUNDED(uvCmn_MCQ_GetACamCurrentPosZ(i+1), 4);
			if (dbPos[0] != dbPos[1])
			{
				m_pGridPos->SetItemText(j, 1, dbPos[1]);
				m_pGridPos->InvalidateRowCol(j, 1);
			}
		}
	}
}

/*
 desc : Job List ����
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateJobList()
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
VOID CDlgMain::UpdateJobLoad()
{
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;

	/* ���� ���õ� Job Name�� �����ϴ��� ���� */
	if (!pstJobMgt->IsJobNameSelected())
	{
		m_edt_txt[0].SetTextToStr(L"None",	TRUE);
		return;
	}

	switch (pstJobMgt->selected_job_load_state)
	{
	case ENG_LSLS::en_load_none			:	m_edt_txt[0].SetTextToStr(L"None",		TRUE);	break;
	case ENG_LSLS::en_load_not_started	:	m_edt_txt[0].SetTextToStr(L"Ready",		TRUE);	break;
	case ENG_LSLS::en_loading			:	m_edt_txt[0].SetTextToStr(L"Loading",	TRUE);	break;
	case ENG_LSLS::en_load_completed	:	m_edt_txt[0].SetTextToStr(L"Loaded",	TRUE);	break;
	case ENG_LSLS::en_load_failed		:	m_edt_txt[0].SetTextToStr(L"Failed",	TRUE);	break;
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
	/* ���� �ֱٿ� Job Load ���� �� ���� */
	m_enLastJobLoadStatus	= ENG_LSLS(pstJobMgt->selected_job_load_state);

	/* Luria�κ��� ���� ���񽺸� �޴��� ���ο� ���� */
	if (!uvEng_GetConfig()->luria_svc.use_announcement)
	{
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
}

/*
 desc : LED Power ����
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateLedAmpl()
{
	UINT8 i		= 0x00;
	UINT8 u8PHs	= uvEng_GetConfig()->luria_svc.ph_count;
	UINT8 u8LED	= uvEng_GetConfig()->luria_svc.led_count;
	BOOL bUpdate	= FALSE;
	LPG_LDEW pstExpo= &uvEng_ShMem_GetLuria()->exposure;

	/* ���� ���� �����ϸ� �������� ���� */
	for (; i<u8PHs && !bUpdate; i++)
	{
		if (0 != memcmp(m_pLedAmplitue[i], pstExpo->led_amplitude_power[i], sizeof(UINT16) * u8LED))
		{
			memcpy(m_pLedAmplitue[i], pstExpo->led_amplitude_power[i], sizeof(UINT16) * u8LED);
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
 desc : Exposure ���� ���� (Print Speed, Step Size, Frame Rate, Duty Cycle, Printing, Status, ...)
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateExposure()
{
	UINT32 u32Speed		= (UINT32)ROUNDED(m_edt_flt[3].GetTextToDouble() * 1000.0f, 0);	/* ���� �뱤 �ӵ� �� ��� */
	LPG_LDEW pstExpo	= &uvEng_ShMem_GetLuria()->exposure;
	LPG_LDMC pstMachine	= &uvEng_ShMem_GetLuria()->machine;

	/* Exposure Speed / Step Size / Frame Rate / Duty Cycle */
	if (u32Speed != pstExpo->get_exposure_speed)
	{
		/* Exposure Speed */
		m_edt_flt[3].SetTextToNum(DOUBLE(pstExpo->get_exposure_speed/1000.0f), 3, TRUE);
		/* Step Size */
		m_edt_int[0].SetTextToNum(pstExpo->scroll_step_size, TRUE);
		/* Duty Cycle */
		m_edt_int[1].SetTextToNum(pstExpo->led_duty_cycle, TRUE);
		/* Frame Rate */
		m_edt_int[2].SetTextToNum(pstExpo->frame_rate_factor, TRUE);
	}
}
#if 0
/*
 desc : Exposure ���� ���� (Print Speed, Step Size, Frame Rate, Duty Cycle, Printing, Status, ...)
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateJobRate()
{
	if (m_pMainThread->IsRunWork())
	{
		TCHAR tzRate[32]	= {NULL};
		swprintf_s(tzRate, 32, L"%.02f %%", m_pMainThread->GetWorkRate());
		m_edt_txt[2].SetTextToStr(tzRate, TRUE);
	}
}
#endif
/*
 desc : Live Grabbed Image ���
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateLiveView()
{
	/* Grabbed Image Display */
	if (m_chk_liv[0].GetCheck())	/* Live Mode */
	{
		if (GetTickCount64() > (m_u64DrawTime+100))
		{
			/* ���� ��µ� ������ �ð� ���� */
			m_u64DrawTime	= GetTickCount64();
			InvalidateView();
		}
		m_u8ViewMode	= 0x01;	/* Live Mode ���� */
	}
	else	/* Grabbed Mode */
	{
		/* ��� ������ Live Mode���ٰ� Grab Mode�� �ٲ� ��� */
		if (0x01 == m_u8ViewMode)
		{
			m_u8ViewMode	= 0x00;	/* Grab Mode ���� */
		}
	}
}
#if 0
/*
 desc : Mark Grabbed Image�� ����� ���� ���� ���
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateMarkGrabResult()
{
	TCHAR tzResult[128]	= {NULL};
	UINT8 u8ACamID		= GetCheckACam();
	UINT8 u8MarkNo		= GetCheckMarkNo();
	LPG_ACGR pstResult	= m_pMainThread->GetLastGrabResult();
	CMyEdit *pEdit		= NULL;
	CMyStatic *pPic		= NULL;

	if (!u8ACamID)
	{
		AfxMessageBox(L"There is no camera selected!", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* ���õ� Mark�� ���� ��� */
	if (0x00 == u8MarkNo)	return;
	pEdit	= &m_edt_rlt[u8MarkNo-1];
	pPic	= &m_pic_ctl[u8MarkNo-1];

	/* ��� �� ��� */
	swprintf_s(tzResult, 128, L"[%.1f,%.1f um] [%.1f %%] [%.1f %%]",
			   (DOUBLE)ROUNDED(pstResult->move_mm_x * 1000.0f, 1),
			   (DOUBLE)ROUNDED(pstResult->move_mm_y * 1000.0f, 1),
			   pstResult->score_rate, pstResult->scale_rate);
	pEdit->SetTextToStr(tzResult, TRUE);
	/* ȭ�� ���� */
	uvEng_Camera_DrawCali(pPic->GetSafeHwnd());
}

/*
 desc : Mark Grabbed Image�� ����� ���� ���� ���
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateMarkExpoResult()
{
	TCHAR tzResult[128]	= {NULL};
	UINT8 i, u8CamID, u8ImgID;
	LPG_ACGR pstMark	= NULL;
	CMyEdit *pEdit		= NULL;
	CMyStatic *pPic		= NULL;

	for (i=0x00; i<4; i++)
	{
		u8CamID	= i < 2 ? 0x01 : 0x02;
		u8ImgID	= i % 2;
		pstMark	= uvEng_Camera_GetGrabbedMark(u8CamID, u8ImgID);
		if (!pstMark)	continue;
		/* �˻� ��� ��� */
		pEdit	= &m_edt_rlt[i];
		/* ��� �� ��� */
		swprintf_s(tzResult, 128, L"[%.1f,%.1f um] [%.1f %%] [%.1f %%]",
				   (DOUBLE)ROUNDED(pstMark->move_mm_x * 1000.0f, 1),
				   (DOUBLE)ROUNDED(pstMark->move_mm_y * 1000.0f, 1),
				   pstMark->score_rate, pstMark->scale_rate);
		pEdit->SetTextToStr(tzResult, TRUE);
		/* �˻��� �̹��� ��� */
		pPic	= &m_pic_ctl[i];
		uvEng_Camera_DrawMarkData(pPic->GetSafeHwnd(), pstMark, pstMark->marked);
	}
}
#endif
/*
 desc : �Ϲ� ��ư �̺�Ʈ ó��
 parm : id	- [in]  Click�� ��ư ID
 retn : None
*/
VOID CDlgMain::OnBtnClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_MAIN_BTN_APP_EXIT		:	PostMessage(WM_CLOSE, 0, 0L);		break;
	case IDC_MAIN_BTN_MC2_MOVE_UP	:	MotionMove(ENG_MDMD::en_move_up);	break;
	case IDC_MAIN_BTN_MC2_MOVE_DN	:	MotionMove(ENG_MDMD::en_move_down);	break;
	case IDC_MAIN_BTN_MC2_RESET		:	MC2Recovery();						break;
	case IDC_MAIN_BTN_MOVE_ACAM		:	ACamMoveAbs();						break;
	case IDC_MAIN_BTN_MOVE_UNLOAD	:	UnloadPosition();					break;
	case IDC_MAIN_BTN_JOBS_ADD		:	JobMgtAdd();						break;
	case IDC_MAIN_BTN_JOBS_DEL		:	JobMgtDel();						break;
	case IDC_MAIN_BTN_JOBS_LIST		:	JobMgtList();						break;
	case IDC_MAIN_BTN_JOBS_LOAD		:	JobMgtLoad();						break;
	case IDC_MAIN_BTN_EXPO_BASE		:	SetExpoParamBase();					break;
	case IDC_MAIN_BTN_EXPO_SET		:	SetExpoParam();						break;
	case IDC_MAIN_BTN_EXPO_GET		:	GetExpoParam();						break;
	case IDC_MAIN_BTN_LED_GET		:	GetLedLightLvl();					break;
	case IDC_MAIN_BTN_LED_SET		:	SetLedLightLvl();					break;
	case IDC_MAIN_BTN_LED_DEFAULT	:	SetLedLightLevel(0x01);				break;
	case IDC_MAIN_BTN_LED_MIN		:	SetLedLightLevel(0x00);				break;
	case IDC_MAIN_BTN_START_GET		:	ExpoStartXY(0);						break;
	case IDC_MAIN_BTN_START_SET		:	ExpoStartXY(1);						break;
	case IDC_MAIN_BTN_START_MOVE	:	uvEng_Luria_ReqSetPrintStartMove();	break;
	case IDC_MAIN_BTN_PH_HOME		:	PhZAxisInitAll();					break;
	case IDC_MAIN_BTN_MOTOR_FOCUS	:	MovingMotorFocus();					break;
	case IDC_MAIN_BTN_LOAD_CONFIG	:	uvEng_ReLoadConfig();				break;
//	case IDC_MAIN_BTN_WORK_STOP		:	WorkStop();							break;
	case IDC_MAIN_BTN_SHUTER_OPEN	:	ShutterCtrl(1);						break;
	case IDC_MAIN_BTN_SHUTER_CLOSE	:	ShutterCtrl(0);						break;
	case IDC_MAIN_BTN_VACUUM_ON		:	VacuumCtrl(1);						break;
	case IDC_MAIN_BTN_VACUUM_OFF	:	VacuumCtrl(0);						break;
	case IDC_MAIN_BTN_JOB_MATERIAL	:	PhZAxisMove();						break;
	case IDC_MAIN_BTN_CALC_ENERGY	:	CalcEnergy();						break;
	case IDC_MAIN_BTN_GRAB_SAVE		:	Grabbed2Saved();					break;
#if 0
	case IDC_MAIN_BTN_MARK_MOVE		:
	case IDC_MAIN_BTN_MARK_GRAB		:
	case IDC_MAIN_BTN_MARK_TEST		:
	case IDC_MAIN_BTN_WORK_START	:	m_edt_txt[1].SetTextToStr(L"", TRUE);
										m_edt_txt[2].SetTextToStr(L"0.00 %%", TRUE);
		switch (id)
		{
		case IDC_MAIN_BTN_MARK_MOVE		:	MoveToMarkPos();				break;
		case IDC_MAIN_BTN_MARK_GRAB		:	MarkGrab();						break;
		case IDC_MAIN_BTN_MARK_TEST		:	MarkTest();						break;
//		case IDC_MAIN_BTN_WORK_START	:	WorkStart();					break;
		}
#endif
	}
}

/*
 desc : üũ ��ư �̺�Ʈ ó��
 parm : id	- [in]  Click�� ��ư ID
 retn : None
*/
VOID CDlgMain::OnChkClicked(UINT32 id)
{
	UINT8 i	= 0x00;

	switch (id)
	{
	case IDC_MAIN_CHK_ACAM_LIVE			:	SetLiveView();		break;

	case IDC_MAIN_CHK_EXPO_DIRECT		:
	case IDC_MAIN_CHK_EXPO_ALIGN_ONLY	:
	case IDC_MAIN_CHK_EXPO_ALIGN_CALI	:
		for (i=0; i<3; i++)	m_chk_mtd[i].SetCheck(0);
		m_chk_mtd[id-IDC_MAIN_CHK_EXPO_DIRECT].SetCheck(1);
		uvEng_Camera_SetAlignMode(ENG_AOEM(id-IDC_MAIN_CHK_EXPO_DIRECT));	break;

	case IDC_MAIN_CHK_MARK_1			:
	case IDC_MAIN_CHK_MARK_2			:
	case IDC_MAIN_CHK_MARK_3			:
	case IDC_MAIN_CHK_MARK_4			:
		for (i=0; i<4; i++)	m_chk_mak[i].SetCheck(0);
		m_chk_mak[id-IDC_MAIN_CHK_MARK_1].SetCheck(1);		break;

	case IDC_MAIN_CHK_ACAM_NO_11		:
	case IDC_MAIN_CHK_ACAM_NO_12		:
	case IDC_MAIN_CHK_ACAM_NO_21		:
	case IDC_MAIN_CHK_ACAM_NO_22		:
		for (i=0; i<uvEng_GetConfig()->set_cams.acam_count; i++)	m_chk_cam[i].SetCheck(0);
		m_chk_cam[id-IDC_MAIN_CHK_ACAM_NO_11].SetCheck(1);		break;
	}
}

/*
 desc : ���� ���õ� ����� ī�޶� ��ȣ ��ȯ (1 or 2)
 parm : None
 retn : 1 or 2 (0x00 ���̸� ���õ� ���� ����)
*/
UINT8 CDlgMain::GetCheckACam()
{
	TCHAR tzNum[8]	= {NULL};

	if (m_chk_cam[0].GetCheck())	return 0x01;
	if (m_chk_cam[1].GetCheck())	return 0x02;
	if (m_chk_cam[2].GetCheck())	return 0x03;
	if (m_chk_cam[3].GetCheck())	return 0x04;
	return 0x00;
}

/*
 desc : ���� ���õ� Mark Number �� ��ȯ
 parm : None
 retn : ���õ� Mark Number �� (0x01 ~ 0x04)
*/
UINT8 CDlgMain::GetCheckMarkNo()
{
	UINT8 u8MarkNo	= 0x00;

	/* ���� ���õ� ��ũ ��ȣ Ȯ�� */
	if (m_chk_mak[0].GetCheck())	u8MarkNo = 0x01;
	if (m_chk_mak[1].GetCheck())	u8MarkNo = 0x02;
	if (m_chk_mak[2].GetCheck())	u8MarkNo = 0x03;
	if (m_chk_mak[3].GetCheck())	u8MarkNo = 0x04;

	return u8MarkNo;
}

/*
 desc : ���� ���õ� Mark Number �� ��ȯ
 parm : None
 retn : ���õ� Job Type �� (0x01 : Direct Expose, 0x02 : Align Expose, 0x03 : Align & Calibration Expose)
*/
UINT8 CDlgMain::GetCheckMethod()
{
	if (m_chk_mtd[1].GetCheck())	return 0x02;	/* Only Align Expose */
	if (m_chk_mtd[2].GetCheck())	return 0x03;

	return 0x01;	/* Only Direct Expose*/
}

/*
 desc : Live Mode ����
 parm : None
 retn : None
*/
VOID CDlgMain::SetLiveView()
{
	UINT8 u8ACamID	= GetCheckACam();
	BOOL bSucc		= FALSE;
	UINT32 u32OnOff	= 0x00000000;

	if (!u8ACamID)
	{
		AfxMessageBox(L"There is no camera selected!", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* �ϴ� Live Image ���� ���� ���� */
	m_bLiveSaved	= FALSE;

	/* ���� äũ�� �����̸� */
	if (m_chk_liv[0].GetCheck())	/* Live View */
	{
		/* Align Camera is Live Mode */
		uvEng_Camera_SetCamMode(ENG_VCCM::en_live_mode);
		/* Trigger & Strobe : Enabled */
		bSucc = uvEng_Trig_ReqTriggerStrobe(TRUE);
		if (bSucc)
		{
			bSucc = uvEng_Trig_ReqEncoderLive(u8ACamID);
#if 0
			/* Live Image ���� ���� */
			if (IDYES == AfxMessageBox(L"Would you like to save the live image to a file?", MB_YESNO))
				m_bLiveSaved = TRUE;
#endif
		}
	}
	else
	{
		/* Trigger Disabled */
		bSucc = uvEng_Trig_ReqEncoderOutReset();
		/* Trigger & Strobe : Disabled */
		if (bSucc)
		{
			bSucc = uvEng_Trig_ReqTriggerStrobe(FALSE);
			UINT64 u64Tick = GetTickCount64();

			do	{

				/* Trigger Board�κ��� ������ ���� ���� ���� �Դ��� Ȯ�� */
				if (0 == uvEng_ShMem_GetTrig()->enc_out_val)
				{
					/* �ִ� 1 �� ���� ��� �� ���� �������� */
					if (GetTickCount64() > (u64Tick + 1000))	break;
				}
				else
				{
					/* �ִ� 2 �� ���� ��� �� ���� ������, ���� �������� */
					if (GetTickCount64() > (u64Tick + 2000))	break;
				}
				Sleep(10);

			}	while (1);
			/* Align Camera is Calibration Mode */
			uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
		}
	}
	Invalidate(TRUE);
}

/*
 desc : ���ο� �˻��� ���� �˻� ����� �� ���
 parm : None
 retn : Non
*/
VOID CDlgMain::SetRegistModel()
{
	UINT32 u32Model	= (UINT32)ENG_MMDT::en_circle, i = 0;
	DOUBLE dbMSize	= 1.0f /* um */, dbMColor = 256.0f /* 256:Black, 128:White */;
	LPG_CIEA pstCfg	= uvEng_GetConfig();
	/* �˻� ��� ���� �� ��� */
	dbMSize	= pstCfg->cmps_test.model_dia_size * 1000.0f;
	dbMColor= pstCfg->cmps_test.model_mark_color;

	for (i=0; i<pstCfg->set_cams.acam_count; i++)
	{
		if (!uvEng_Camera_SetModelDefineEx(i+1,
										   pstCfg->mark_find.model_speed,
										   pstCfg->mark_find.model_smooth,
										   &u32Model, &dbMColor, &dbMSize,
										   NULL, NULL, NULL, 1))
		{
			AfxMessageBox(L"Failed to setup the model for search target", MB_ICONSTOP|MB_TOPMOST);
			return;
		}
	}
}

/*
 desc : ��� (Motor Drive) �̵�
 parm : direct	- [in]  Up or Down
 retn : None
*/
VOID CDlgMain::MotionMove(ENG_MDMD direct)
{
	UINT8 i	= 0x00, u8DrvNo = 0x00;	/* 0x00 ~ 0x07 */
	BOOL bCheck		= FALSE;
	DOUBLE dbDrvPos	= 0, dbMinPos, dbMaxPos, dbDist, dbVelo;
	LPG_CMSI pstCfg	= &uvEng_GetConfig()->mc2_svc;
	LPG_MDSM pstMC2	= uvEng_ShMem_GetMC2();

	/* �̵��ϰ��� �ϴ� Drive�� üũ �Ǿ� �ִ��� Ȯ�� */
	for (i=0; i<pstCfg->drive_count && !bCheck; i++)
	{
		bCheck	= m_chk_drv[pstCfg->axis_id[i]].GetCheck() > 0;
	}
	if (!bCheck)
	{
		AfxMessageBox(L"The drive to be moved is not checked", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* �̵��ϰ��� �ϴ� �ӵ��� �Ÿ� ���� �ԷµǾ� �ִ��� Ȯ�� (���⼭�� Step �̵��� �ش� ��) */
	dbDist	= m_edt_flt[0].GetTextToDouble();
	dbVelo	= m_edt_flt[1].GetTextToDouble();
	if (dbDist < 0.0001f || dbVelo < 1.0f)
	{
		AfxMessageBox(L"Check the move distance and speed values", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* ���� ��ġ���� �̵� �Ÿ� ���� Min or Max ���� �ʰ� �ߴٸ�, Min or Max �� ��ŭ �̵� */
	for (i=0; i<pstCfg->drive_count; i++)
	{
		/* ����̺갡 üũ �Ǿ� �ִ��� ���� Ȯ�� */
		u8DrvNo	= pstCfg->axis_id[i];
		if (!m_chk_drv[u8DrvNo].GetCheck())	continue;

		/* ���� ����̺��� ��ġ Ȯ�� */
		dbDrvPos	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(u8DrvNo));
		dbMinPos	= pstCfg->min_dist[u8DrvNo];
		dbMaxPos	= pstCfg->max_dist[u8DrvNo];

		/* ���� Ȥ�� ���� ������ �̵��ϸ�, ���� ��ġ�� �̵� �Ÿ� ����ŭ ���� �ֱ� */
		if (direct == ENG_MDMD::en_move_up || direct == ENG_MDMD::en_move_right)
		{
			dbDrvPos	+= dbDist;	/* �̵��� �Ÿ� �� ��� */
			if (dbDrvPos > dbMaxPos)	dbDrvPos	= dbMaxPos;	/* �ִ� �̵� �Ÿ� �Ѿ��, �ִ� ������ ��ü */
		}
		else
		{
			dbDrvPos	-= dbDist;	/* �̵��� �Ÿ� �� ��� */
			if (dbDrvPos < dbMinPos)	dbDrvPos	= dbMinPos;	/* �ּ� �̵� �Ÿ� �Ѿ��, �ִ� ������ ��ü */
		}

		/* Motion Drive �̵� */
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI(u8DrvNo), dbDrvPos, dbVelo))
		{
			AfxMessageBox(L"Failed to move the motion drive", MB_ICONSTOP|MB_TOPMOST);
			return;
		}
	}
}

/*
 desc : MC2�� ��� Motor �ʱ�ȭ (����) (Homing)
 parm : None
 retn : None
*/
VOID CDlgMain::MC2Recovery()
{
	UINT8 i	= 0x00;
#if 1
	for (i=0; i<MAX_MC2_DRIVE; i++)
	{
		if (!m_chk_drv[i].GetCheck())	continue;
		if (!uvEng_MC2_SendDevHoming(ENG_MMDI(i)))
		{
			AfxMessageBox(L"The home operation of all drives has failed", MB_ICONSTOP|MB_TOPMOST);
			return;
		}
	}
#else
	if (!uvEng_MC2_SendDevHomingAll())
	{
		AfxMessageBox(L"The home operation of all drives has failed", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
#endif
}

/*
 desc : Align Camera (Motor Drive) �̵� (���� ��ġ�� �̵�)
 parm : None
 retn : None
*/
VOID CDlgMain::ACamMoveAbs()
{
	DOUBLE dbPosZ	= 0.0f;	/* unit : mm */
	UINT8 u8ACamID	= GetCheckACam();
	if (!u8ACamID)
	{
		AfxMessageBox(L"There is no camera selected!", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* Photohead or Align Camera Z Axis �̵� */
	dbPosZ	= m_edt_flt[2].GetTextToDouble();
	/* ���ο� ��ġ�� �̵� */
	uvEng_MCQ_SetACamMovePosZ(u8ACamID, 0x00 /* Absolute Moving */, dbPosZ);
}

/*
 desc : �������� ��ġ�� �۾��� ��ġ�� �̵�
 parm : None
 retn : None
*/
VOID CDlgMain::UnloadPosition()
{
	DOUBLE dbMoveX	= uvEng_GetConfig()->set_align.table_unloader_xy[0][0];
	DOUBLE dbMoveY	= uvEng_GetConfig()->set_align.table_unloader_xy[0][1];

	/* ��� (��������) �̵� */
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, dbMoveX, m_edt_flt[1].GetTextToDouble());
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dbMoveY, m_edt_flt[1].GetTextToDouble());
}

/*
 desc : ���� ȯ�� ���Ͽ� ����� ȸ�� �� �ҷ�����
 parm : None
 retn : None
*/
VOID CDlgMain::LoadParam()
{
#if 0
	/* Measurement Row and Column count */
	m_edt_int[0].SetTextToNum(UINT16(10));
	m_edt_int[1].SetTextToNum(UINT16(40));
	/* The size of Grabbed Image */
	m_edt_int[2].SetTextToNum(uvEng_GetConfig()->set_cams.ioi_size[0]);
	m_edt_int[3].SetTextToNum(uvEng_GetConfig()->set_cams.ioi_size[1]);
	/* Move Velo and Distance */
	m_edt_flt[0].SetTextToNum(100.0f,	4);
	m_edt_flt[1].SetTextToNum(uvEng_GetConfig()->mc2_svc.move_velo,	4);
	/* Up or Down For camera z axis */
	m_edt_flt[9].SetTextToNum(1.0f, 4);
	/* Measurement Row and Column Period Size (mm) */
	m_edt_flt[12].SetTextToNum(5.0f, 4);
	m_edt_flt[13].SetTextToNum(5.0f, 4);
#endif
}
#if 0
/*
 desc : ��ư �� ���� ��Ʈ�� Enable or Disable ó��
 parm : None
 retn : None
*/
VOID CDlgMain::EnableCtrl()
{
	UINT8 i;
	BOOL bStopped	= TRUE, bRunExpo = m_pMainThread->IsRunWork();
	LPG_CMSI pstMC2Svc	= &uvEng_GetConfig()->mc2_svc;

	/* �ʿ��� ��ư�� Enable or Disable ó�� */
	if (!uvCmn_MC2_IsConnected()	||
		!uvCmn_Camera_IsConnected()	||
		!uvCmn_Trig_IsConnected()	||
		!uvCmn_MCQ_IsConnected())
	{
		return;
	}

	/* ���� ���õ� ī�޶� �̵� �������� ���ο� ���� */
	bStopped	= uvEng_ShMem_GetMC2()->GetDriveAct((UINT8)ENG_MMDI::en_stage_x)->IsStopped()	&&
				  uvEng_ShMem_GetMC2()->GetDriveAct((UINT8)ENG_MMDI::en_stage_y)->IsStopped()	&&
				  uvEng_ShMem_GetMC2()->GetDriveAct((UINT8)ENG_MMDI::en_align_cam1)->IsStopped()&&
				  uvEng_ShMem_GetMC2()->GetDriveAct((UINT8)ENG_MMDI::en_align_cam2)->IsStopped();
	/* ���� �ֱ� ���� ���� */
	m_bMotionStop	= !(!bStopped || bRunExpo);

#if 1
	for (i=0; i<9; i++)		m_btn_ctl[i].EnableWindow(m_bMotionStop);
	for (i=10;i<29; i++)	m_btn_ctl[i].EnableWindow(m_bMotionStop);
	m_btn_ctl[5].EnableWindow(!m_bMotionStop);	/* Stop Button */
#if 0
	/* MC2 Drive Check Button */
	for (i=0; i<uvEng_GetConfig()->mc2_svc.drive_count; i++)
	{
		m_chk_drv[pstMC2Svc->axis_id[i]].EnableWindow(m_bMotionStop);
	}
#endif
	for (i=0; i<uvEng_GetConfig()->set_cams.acam_count; i++)
	{
		m_chk_cam[i].EnableWindow(m_bMotionStop);
	}
	for (i=0; i<4; i++)	m_chk_mak[i].EnableWindow(m_bMotionStop);
	for (i=0; i<4; i++)	m_chk_mtd[i].EnableWindow(m_bMotionStop);
	for (i=0; i<1; i++)	m_chk_liv[i].EnableWindow(m_bMotionStop);
#endif
}
#endif
/*
 desc : ���������� �����ϴ��� ��Ī Ȯ��
 parm : None
 ��ȭ : None
*/
VOID CDlgMain::RunTestMatch()
{
	UINT8 u8ACamID		= GetCheckACam();
	BOOL bFinded		= FALSE;
	UINT64 u64TickLoop	= 0, u64StartTick;
	LPG_ACGR pstResult	= NULL;

	if (!u8ACamID)
	{
		AfxMessageBox(L"There is no camera selected!", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* ���� Live Mode ������� Ȯ�� */
	if (m_chk_liv[0].GetCheck())
	{
		AfxMessageBox(L"Currently running in <live mode>", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* ���� ��ϵ� ���� �����ϴ��� ���� */
	if (!uvEng_Camera_IsSetMarkModel(0x01, u8ACamID))
	{
		AfxMessageBox(L"There are no registered models", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* Align Camera is Edge Detection Mode */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);
#if 0
	/* ù�� ��� �� �κ� �� �ʱ�ȭ */
	m_pGridMeas[0].SetItemText(1, 1, L"+0.0");
	m_pGridMeas[1].SetItemText(1, 1, L"+0.0");
	m_pGridMeas[0].InvalidateRowCol(1, 1);
	m_pGridMeas[1].InvalidateRowCol(1, 1);
#endif
	/* ���� �˻��� Grabbed Data & Image ���� */
	uvEng_Camera_ResetGrabbedMark();
	uvEng_Camera_ResetGrabbedImage();

	/* Trigger ������ (��������) �߻� */
	if (!uvEng_Trig_ReqTrigOutOne(u8ACamID, TRUE))
	{
		uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
		AfxMessageBox(L"Failed to send the event for trigger", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* �۾� ��û ��� �ð� */
	u64StartTick	= GetTickCount64();
	/* 1�� ���� ��� */
	do {
		if (u64StartTick+2000 < GetTickCount64())	break;
		uvCmn_uSleep(100);
	} while (1);

	/* �˻��� ����� �ִ��� ���� */
	pstResult	= uvEng_Camera_RunModelCali(u8ACamID, 0xff);
	if (!pstResult->marked)
	{
		AfxMessageBox(L"Failed to analyze the grabbed image", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

#if 0
	/* ȭ�� ���� */
	DrawGrabbedImage();
	/* �׸��� ��Ʈ�� ���� */
	UpdateCaliXY(1, 1, pstResult);
	/* Camera ���� ��� ���� */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
#endif
}

/*
 desc : ���ο� Job Name ���
 parm : None
 retn : None
*/
VOID CDlgMain::JobMgtAdd()
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
VOID CDlgMain::JobMgtDel()
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
 desc : ���� ���µ� Job List ��û
 parm : None
 retn : None
*/
VOID CDlgMain::JobMgtList()
{
	uvEng_Luria_ReqGetJobList();
}

/*
 desc : ���� ���µ� Job Name ���� ����
 parm : None
 retn : None
*/
VOID CDlgMain::JobMgtLoad()
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
 desc : Exposure (Print) ���� ���� (�⺻ ��)
 parm : None
 retn : None
*/
VOID CDlgMain::SetExpoParamBase()
{
	LPG_CCTV pstCMPS	= &uvEng_GetConfig()->cmps_test;
#if 0
	m_edt_int[0].SetTextToNum(pstCMPS->expo_step_size);
	m_edt_int[1].SetTextToNum(pstCMPS->expo_duty_cycle);
	m_edt_int[2].SetTextToNum(pstCMPS->expo_frame_rate);
#endif
	if (!uvEng_Luria_ReqSetExposureFactor(pstCMPS->expo_step_size,
										  pstCMPS->expo_duty_cycle,
										  pstCMPS->expo_frame_rate/1000.0f))
	{
		AfxMessageBox(L"Failed to set the value required for exposure", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : Exposure (Print) ���� ����
 parm : None
 retn : None
*/
VOID CDlgMain::SetExpoParam()
{
	UINT8 u8StepSize	= (UINT8)m_edt_int[0].GetTextToNum();
	UINT8 u8DutyCycle	= (UINT8)m_edt_int[1].GetTextToNum();
	UINT16 u16FrameRate	= (UINT16)m_edt_int[2].GetTextToNum();
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
 desc : Exposure (Print) ���� ���
 parm : None
 retn : None
*/
VOID CDlgMain::GetExpoParam()
{
	if (!uvEng_Luria_ReqGetExposureFactor())
	{
		AfxMessageBox(L"Failed to get the value required for exposure", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : ���� Photohead�� LED�� ������ Level Index �� ��û
 parm : None
 retn : None
*/
VOID CDlgMain::GetLedLightLvl()
{
	UINT8 i		= 0x00;
	UINT8 u8PHs	= uvEng_GetConfig()->luria_svc.ph_count;
	UINT8 u8LED	= uvEng_GetConfig()->luria_svc.led_count;

	/* ���� �� �ʱ�ȭ */
	for (; i<u8PHs; i++)
	{
		memset(m_pLedAmplitue[i], 0xff, sizeof(UINT16) * u8LED);
	}

	/* ���� �����Ǿ� �ִ� �� ��û */
	if (!uvEng_Luria_ReqGetLedAmplitude())
	{
		AfxMessageBox(L"Failed to get the LED level value for photohead", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : Photohead�� LED�� ���ο� Level Index �� ����
 parm : None
 retn : None
*/
VOID CDlgMain::SetLedLightLvl()
{
	UINT8 i	= 0x01;
	UINT16 u16Level	= 0;
	BOOL bSucc	= TRUE;

	for (; i<=uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		u16Level	= (UINT16)m_pGridLed->GetItemTextToInt(i, 1);
		if (bSucc)	bSucc	= uvEng_Luria_ReqSetLedAmplitudeOne(i, 1, u16Level);	/* 365 */
		u16Level	= (UINT16)m_pGridLed->GetItemTextToInt(i, 2);
		if (bSucc)	bSucc	= uvEng_Luria_ReqSetLedAmplitudeOne(i, 2, u16Level);	/* 385 */
		u16Level	= (UINT16)m_pGridLed->GetItemTextToInt(i, 3);
		if (bSucc)	bSucc	= uvEng_Luria_ReqSetLedAmplitudeOne(i, 3, u16Level);	/* 395 */
		u16Level	= (UINT16)m_pGridLed->GetItemTextToInt(i, 4);
		if (bSucc)	bSucc	= uvEng_Luria_ReqSetLedAmplitudeOne(i, 4, u16Level);	/* 405 */
	}
	if (!bSucc)
	{
		AfxMessageBox(L"Failed to set the LED level value for photohead", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : Photohead�� LED�� ���ο� Level Index ���� �ּ� Ȥ�� ������ ������ ����
 parm : mode	- [in] 0x00 : �ּ� �� ��, 0 �� �Է�, 0x01 : ������ �� �Է�
 retn : None
*/
VOID CDlgMain::SetLedLightLevel(UINT8 mode)
{
	UINT8 i	= 0x01, j = 0;
	UINT16 u16Level	= 0;
	BOOL bSucc	= TRUE;

	UINT16 (*pLedId)[MAX_LED]	= uvEng_GetConfig()->cmps_test.ph_led_power_id;

	for (; i<=uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
#if 1
		for (j=1; (bSucc && j<=uvEng_GetConfig()->luria_svc.led_count); j++)
		{
			/* 365, 385, 395, 405 */
			if (mode)	u16Level	= pLedId[i-1][j-1];
			uvEng_Luria_ReqSetLedAmplitudeOne(i, j, u16Level);
		}
#else
		if (mode)	u16Level	= pLedId[i-1][0];
		uvEng_Luria_ReqSetLedAmplitudeOne(i, 1, u16Level);	/* 365 */
		if (mode)	u16Level	= pLedId[i-1][1];
		uvEng_Luria_ReqSetLedAmplitudeOne(i, 2, u16Level);	/* 385 */
		if (mode)	u16Level	= pLedId[i-1][2];
		uvEng_Luria_ReqSetLedAmplitudeOne(i, 3, u16Level);	/* 395 */
		if (mode)	u16Level	= pLedId[i-1][3];
		uvEng_Luria_ReqSetLedAmplitudeOne(i, 4, u16Level);	/* 405 */
#endif
	}
	if (!bSucc)
	{
		AfxMessageBox(L"Failed to set the LED level value for photohead", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : �뱤 ���� ��ġ ��� Ȥ�� ����
 parm : flag	- [in]  0x00 : ���, 0x01 : ����
 retn : None
*/
VOID CDlgMain::ExpoStartXY(UINT8 flag)
{
	/* �뱤�� ���۵� ��ġ (���� ��� ���� ��ġ�� �ƴϰ�, Luria�� �ԷµǴ� ��ġ) */
	LPG_I4XY pstXY	= &uvEng_ShMem_GetLuria()->machine.table_expo_start_xy[0];
	if (0x00 == flag)
	{
		m_edt_flt[4].SetTextToNum(pstXY->x / 1000.0f, 3, TRUE);	/* ����: um */
		m_edt_flt[5].SetTextToNum(pstXY->y / 1000.0f, 3, TRUE);	/* ����: um */
	}
	else
	{
		uvEng_Luria_ReqSetTableExposureStartPos(0x01,
												(UINT32)ROUNDED(m_edt_flt[4].GetTextToDouble() * 1000.0f, 0),	/* unit: mm */
												(UINT32)ROUNDED(m_edt_flt[5].GetTextToDouble() * 1000.0f, 0),	/* unit: mm */
												TRUE);
	}
}

/*
 desc : ���а� Z �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgMain::PhZAxisInitAll()
{
	/* �߰� ��ġ�� �̵� */
	uvEng_Luria_ReqSetMotorPositionInitAll(0x01);
}

/*
 desc : ��� ���а� �� ����� ī�޶� ���� �β� ��� �뱤/��ũ �ν� ��Ŀ�� ��ġ�� �̵�
 parm : None
 retn : None
*/
VOID CDlgMain::MovingMotorFocus()
{
	UINT8 i			= 0x00;
	DOUBLE dbOffset	= 0.0f;
	DOUBLE dbPosZ	= 0.0f;	/* ����: mm */

	/* ���� ���� �뱤 ���� �β� ���, ���� �뱤 ���� �β� ���� ���ϰ� */
	dbOffset	= uvEng_GetConfig()->cmps_test.expo_film_thick - uvEng_GetConfig()->set_align.dof_film_thick;
	/* ���а� ��ġ�� ���� �뱤 ��Ŀ�� ��� �� ���̸�ŭ �̵��ϴ� �۾��� */
	for (i=0; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		/* ���а� Z �� Focus �� ��� */
		dbPosZ	= uvEng_GetConfig()->luria_svc.ph_z_focus[i] + dbOffset /* ���� �β� ���̸�ŭ �� Ȥ�� �� �̵� */;
		/* �̵��ϰ��� �ϴ� ���� ������ ������� ���� */
		if (!uvCmn_Luria_IsValidPhMoveZRange(dbPosZ))
		{
			AfxMessageBox(L"Out of moving range area", MB_ICONSTOP|MB_TOPMOST);
			return;
		}
		/* ���а� Z �� �̵� */
		uvEng_Luria_ReqSetMotorAbsPosition(i+1, dbPosZ);
	}

	/* ����� ī�޶� ��Ŀ���� �̵� (Basler Camera�� �ش��) */
	if (ENG_VCPK::en_camera_basler_ipv4 != (ENG_VCPK)uvEng_GetConfig()->set_comn.align_camera_kind)	return;
	for (i=0; i<uvEng_GetConfig()->set_cams.acam_count; i++)
	{
		/* ����� ī�޶� Z �� Focus �� ��� */
		dbPosZ	= uvEng_GetConfig()->acam_focus.acam_z_focus[i] + dbOffset /* ���� �β� ���̸�ŭ �� Ȥ�� �� �̵� */;
		uvEng_MCQ_SetACamMovePosZ(i+1, 0x00, dbPosZ);
	}
}

/*
 desc : �뱤 �۾� ����
 parm : None
 retn : None
*/
VOID CDlgMain::WorkStart()
{
	UINT8 u8Work	= GetCheckMethod();

	/* ������ ���� ������ Ȯ�� */
	if (m_pMainThread->IsRunWork())	return;

	/* ���� �˻��� Grabbed Data & Image ���� */
	uvEng_Camera_ResetGrabbedMark();
	uvEng_Camera_ResetGrabbedImage();
	/* Align Mark Grab.Mode */
	if (0x01 != u8Work)	uvEng_Camera_SetCamMode(ENG_VCCM::en_grab_mode);
	else				uvEng_Camera_SetCamMode(ENG_VCCM::en_none);

	/* Align Camera�� Calibration File�� �ִ� ���� ������ �� ���� */
	if (0x03 == u8Work)
	{
#if 0
		if (!uvEng_ACamCali_LoadFile(UINT16(m_edt_int[2].GetTextToNum())))
		{
			AfxMessageBox(L"There is no calibration data", MB_ICONSTOP|MB_TOPMOST);
			return;
		}
#endif
	}

	m_pMainThread->SetWorkExpoOnly();
}

/*
 desc : �뱤 �۾� ����
 parm : None
 retn : None
*/
VOID CDlgMain::WorkStop()
{
	/* �۾� ���� ���� ���� ���μ��� ���� */
#if 1
	m_pMainThread->StopWorkJob();
#else
#endif
}

/*
 desc : Shutter ����/�ݱ�
 parm : None
 retn : None
*/
VOID CDlgMain::ShutterCtrl(UINT8 onoff)
{
	UINT32 u32Addr = (UINT32)ENG_PIOA::en_shutter_open_close;
	/* Write */
	uvEng_MCQ_WriteBits(u32Addr, onoff);
}

/*
 desc : Vacuum On/Off
 parm : None
 retn : None
*/
VOID CDlgMain::VacuumCtrl(UINT8 onoff)
{
	UINT32 u32Addr = (UINT32)ENG_PIOA::en_vacuum_on_off;
	/* Write */
	uvEng_MCQ_WriteBits(u32Addr, onoff);
}

/*
 desc : ���� ���� �β� ���, �뱤 ���� �β� ���̸�ŭ ���а� Up or Down
 parm : None
 retn : None
*/
VOID CDlgMain::PhZAxisMove()
{
	UINT32 i;
	DOUBLE dbPhPos[MAX_PH] = {NULL}, dbBase, dbPos;
	UINT32 u32Thick  = (UINT32)m_edt_int[3].GetTextToNum();
	LPG_CIEA pstCfg = uvEng_GetConfig();

	/* ���� ���� �β� */
	dbBase	= (u32Thick / 1000.0f) - pstCfg->set_align.dof_film_thick;

	/* ���� ���а� ���̿��� ���� �β� ���� ��ŭ ���̸� ... ���� Ȥ�� ���� */
	for (i=0; i<pstCfg->luria_svc.ph_count; i++)
	{
		dbPos	= pstCfg->luria_svc.ph_z_focus[i] + dbBase;
		uvEng_Luria_ReqSetMotorAbsPosition(i+1, dbPos);
	}
}

/*
 desc : ���� ������ �Ķ���� ���� �뱤 ������ �� ���
 parm : None
 retn : None
*/
VOID CDlgMain::CalcEnergy()
{
	TCHAR tzMesg[128]	= {NULL};
	UINT8 i	= 0;
	DOUBLE dbTotal		= 0.0f;
	LPG_LDEW pstExpo	= &uvEng_ShMem_GetLuria()->exposure;

	for (; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		dbTotal	+= uvCmn_Luria_GetExposeEnergy(pstExpo->scroll_step_size,
											   pstExpo->led_duty_cycle,
											   pstExpo->frame_rate_factor,
											   uvEng_GetConfig()->cmps_test.ph_led_power_wt[i]);
	}

	swprintf_s(tzMesg, 128, L"Average Energy = %.3f\n", dbTotal / DOUBLE(i));
	AfxMessageBox(tzMesg, MB_ICONINFORMATION);
}

/*
 desc : ���� Trigger Board ���� (Trig On Time, Strobe On Time, Trig Delay Time)�� ����, �̹����� Grab �� ����
 parm : None
 retn : None
*/
VOID CDlgMain::Grabbed2Saved()
{
	/* �ݵ�� �ʱ�ȭ ����� �� */
	uvEng_Camera_ResetGrabbedImage();

	uvEng_Camera_SetCamMode(ENG_VCCM::en_grab_mode);
	uvEng_Trig_ReqTrigOutOne(GetCheckACam(), TRUE);	/* Ʈ���� ������ �� �ٷ� Disable ó�� */

	/* Ʈ���� �߻� �� ���� �ð� ���� */
	UINT64 u64Tick	= GetTickCount64();
	do {

		uvCmn_uSleep(100);
		if ((u64Tick+3000) < GetTickCount64())	return;
		if (uvEng_Camera_GetGrabbedCount())	break;

	} while (1);

	InvalidateView();
}

/*
 desc : �̹��� �� ���� ����
 parm : None
 retn : None
*/
VOID CDlgMain::InvalidateView()
{
	CRect rView;
	m_pic_ctl[4].GetWindowRect(rView);
	ScreenToClient(rView);
	InvalidateRect(rView, TRUE);
}
