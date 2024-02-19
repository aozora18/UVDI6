
/*
 desc : Main Dialog
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgMain.h"
#include "MainThread.h"

#include "./Work/WorkCent.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : constructor
 parm : parent	- [in]  parent object pointer
 retn : None
*/
CDlgMain::CDlgMain(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgMain::IDD, parent)
{
	m_bDrawBG		= TRUE;
	m_hIcon			= AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_bDofThick		= FALSE;
	m_bLiveMode		= FALSE;
	m_pWorkCent		= NULL;
	m_u8ErrIdx		= 0x00;
	m_pMainThread	= NULL;
	m_u64TickMesg	= GetTickCount64();
	m_u64TickMesg	= m_u64TickMesg;
	m_u64TickCtrl	= m_u64TickMesg;
	m_u64TickText	= m_u64TickMesg;
	m_u64TickLogs	= m_u64TickMesg;
	m_u64TickLive	= m_u64TickMesg;
}

/*
 desc : window id mapping
 parm : dx	- object for data exchange and validation
 retn : None
*/
VOID CDlgMain::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Groupbox - Normal */
	u32StartID	= IDC_MAIN_GRP_MC2;
	for (i=0; i<11; i++)	DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Picture - Normal */
	u32StartID	= IDC_MAIN_PIC_ACAM;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_pic_ctl[i]);
	/* Combobox - Normal */
	u32StartID	= IDC_MAIN_CMB_ACAM;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_cmb_ctl[i]);
	/* Static - Normal */
	u32StartID	= IDC_MAIN_TXT_STEP;
	for (i=0; i<16; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Editbox - integer */
	u32StartID	= IDC_MAIN_EDT_MEAS_COUNT;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_edt_int[i]);
	/* Editbox - float */
	u32StartID	= IDC_MAIN_EDT_DIST;
	for (i=0; i<13; i++)	DDX_Control(dx, u32StartID+i,	m_edt_flt[i]);
	/* Editbox - Text (String) */
	u32StartID	= IDC_MAIN_EDT_ERROR;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_edt_txt[i]);
	/* Button - Normal */
	u32StartID	= IDC_MAIN_BTN_EXIT;
	for (i=0; i<15; i++)	DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Checkbox - Normal */
	u32StartID	= IDC_MAIN_CHK_LIVE;
	for (i=0; i<4; i++)		DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgMain, CMyDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MAIN_THREAD,	OnMsgThread)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_BTN_EXIT,	IDC_MAIN_BTN_MATCH_STOP,	OnBtnClicked)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_CHK_LIVE,	IDC_MAIN_CHK_MEAS_TARGET,	OnChkClicked)
	ON_NOTIFY_RANGE(GVN_COLUMNCLICK, IDC_MAIN_GRID_MC2, IDC_MAIN_GRID_MEAS,		OnGridSelChanged)
	ON_EN_CHANGE(IDC_MAIN_EDT_MEAS_COUNT, OnEnChangeSetMeasVal)
	ON_EN_CHANGE(IDC_MAIN_EDT_MEAS_INTVL, OnEnChangeSetMeasVal)
END_MESSAGE_MAP()

/*
 desc : System Control Event
		the user selects a command from the Control menu,
		or when the user selects the Maximize or the Minimize button
 parm : id		- Specifies the type of system command requested
		lparam	- If a Control-menu command is chosen with the mouse,
				  lParam contains the cursor coordinates.
				  The low-order word contains the x coordinate,
				  and the high-order word contains the y coordinate.
				  Otherwise this parameter isn't used
 retn : None
*/
VOID CDlgMain::OnSysCommand(UINT32 id, LPARAM lparam)
{

	CMyDialog::OnSysCommand(id, lparam);
}

/*
 desc : When the windows is running, it is initially called once
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::OnInitDlg()
{
	UINT32 u32Speed	= NORMAL_THREAD_SPEED;
	/* Set whether to ouput to the sub-monitor */
	/*uvCmn_MoveSubMonitor(m_hWnd);*/

	/* Initialize the Engine library */
	if (!InitLib())	return FALSE;

	/* Initlaize the window and grid controls */
	InitCtrl();
	InitGridMC2();
	InitGridMeas();
	InitData();
	if (uvEng_GetConfig()->IsRunDemo())	InitDemo();

	/* Create the work step object */
	m_pWorkCent	= new CWorkCent(&m_stSearch);
	ASSERT(m_pWorkCent);
	memset(&m_stSearch, 0x00, sizeof(STG_SCIM));
	/* Creates the main-thread (After initialing the Engine library) */
	if (uvEng_GetConfig()->IsRunDemo())	u32Speed	= 10;
	m_pMainThread	= new CMainThread(m_hWnd);
	ASSERT(m_pMainThread);
	if (!m_pMainThread->CreateThread(0, NULL, u32Speed))
	{
		delete m_pMainThread;
		m_pMainThread	= NULL;
	}

	return TRUE;
}

/*
 desc : When the window is stopping, it is called once at the last
 parm : None
 retn : None
*/
VOID CDlgMain::OnExitDlg()
{
	/* Release the memory for main-thread */
	if (m_pMainThread)
	{
		m_pMainThread->Stop();
		while (m_pMainThread->IsBusy())	m_pMainThread->Sleep();
		delete m_pMainThread;
	}
	/* Release the memory for work step object */
	if (m_pWorkCent)
	{
		delete m_pWorkCent;
		m_pWorkCent	= NULL;
	}
	/* Only used in the demo version */
	if (uvEng_GetConfig()->IsRunDemo())
	{
		m_vMarkX.clear();
		m_vMarkY.clear();
	}
	/* Remove all event messages */
	uvCmn_WaitPeekMessageQueue(500);
	/* Release the Engine library (It should be written last!) */
	CloseLib();
	/* Release the grid control */
	CloseGrid();
}

/*
 desc : Called whenever the window is updated
 parm : dc	- device context
 retn : None
*/
VOID CDlgMain::OnPaintDlg(CDC *dc)
{
}

/*
 desc : When the window is running and resizing, It is initially once called
 parm : None
 retn : None
*/
VOID CDlgMain::OnResizeDlg()
{
}

/*
 desc : Initialize the controls
 parm : None
 retn : None
*/
VOID CDlgMain::InitCtrl()
{
	INT32 i;

	/* Normal - Groupbox */
	for (i=0; i<11; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* Normal - Static */
	for (i=0; i<16; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(TRUE);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* Edit box - Integer */
	for (i=0; i<1; i++)
	{
		m_edt_int[i].SetEditFont(&g_lf);
		m_edt_int[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_int[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_int[i].SetInputType(ENM_DITM::en_uint16);
		m_edt_int[i].SetUseMinMax(TRUE);
		m_edt_int[i].SetInputSign(ENM_MNUS::en_unsign);
		m_edt_int[i].SetMinMaxNum(UINT16(2), UINT16(1024));
	}
	/* Edit box - Float */
	for (i=0; i<13; i++)
	{
		m_edt_flt[i].SetEditFont(&g_lf);
		m_edt_flt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_flt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_flt[i].SetInputType(ENM_DITM::en_float);
		m_edt_flt[i].SetUseMinMax(TRUE);
		m_edt_flt[i].SetMinMaxNum(0.0f, 1000.0f);
		m_edt_flt[i].SetInputSign(ENM_MNUS::en_unsign);
		m_edt_flt[i].SetInputPoint(4);

		if (i < 4)
		{
			m_edt_flt[i].SetReadOnly(FALSE);
			m_edt_flt[i].SetMouseClick(TRUE);
		}
		else
		{
			m_edt_flt[i].SetReadOnly(TRUE);
			m_edt_flt[i].SetMouseClick(FALSE);
		}
	}

	/* Edit box - String */
	for (i=0; i<1; i++)
	{
		m_edt_txt[i].SetEditFont(&g_lf);
		m_edt_txt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_txt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_txt[i].SetInputType(ENM_DITM::en_string);
		m_edt_txt[i].SetInputMaxStr(1024);
		m_edt_txt[i].SetReadOnly(TRUE);
		m_edt_txt[i].SetMouseClick(FALSE);
	}
	/* Normal - Button */
	for (i=0; i<15; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
	}
	/* Normal - Checkbox */
	for (i=0; i<4; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf);
		m_chk_ctl[i].SetBgColor(g_clrBtnColor);
		m_chk_ctl[i].SetReadOnly(FALSE);
	}
	m_chk_ctl[0].SetCheck(0);
//	m_chk_ctl[1].SetCheck(1);

	/* Combobox - Normal */
	for (i=0; i<1; i++)	m_cmb_ctl[i].SetLogFont(&g_lf);
	for (i=0; i<uvEng_GetConfig()->set_cams.acam_count; i++)	m_cmb_ctl[0].AddNumber(UINT64(i+1));
	m_cmb_ctl[0].SetCurSel(0);
}

/*
 desc : Create a grid control
 parm : None
 retn : None
*/
VOID CDlgMain::InitGridMC2()
{
	TCHAR tzCols[4][16]	= { L"Item", L"Pos (mm)", L"Servo", L"Error" }, tzRows[32] = {NULL};
	INT32 i32Width[4]	= { 52, 70, 58, 53 }, i, j=1;
	INT32 i32ColCnt		= 4, i32RowCnt = 0;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	LPG_CIEA pstCfg		= uvEng_GetConfig();
	GV_ITEM stGV		= {NULL};
	CRect rGrid;

	/* Last Recv. Packet / PDO Header / PDO Data (Values) */
	::GetWindowRect(m_grp_ctl[0].GetSafeHwnd(), rGrid);
	ScreenToClient(rGrid);
	rGrid.left	+= 13;
	rGrid.right	-= 13;
	rGrid.top	+= 80;
	rGrid.bottom-= 74;

	/* Get the total number of rows to be printed */
	if (0x01 == pstCfg->luria_svc.z_drive_type)
	{
		i32RowCnt	= pstCfg->luria_svc.ph_count + pstCfg->mc2_svc.drive_count;
	}
	else if (0x03 == pstCfg->luria_svc.z_drive_type)
	{
		i32RowCnt	= pstCfg->mc2_svc.drive_count;
	}
	i32RowCnt		+=pstCfg->set_cams.acam_count;

	/* Basic properties of gird controls */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* Create a object */
	m_pGridMC2	= new CGridCtrl;
	ASSERT(m_pGridMC2);
	m_pGridMC2->SetDrawScrollBarHorz(FALSE);
	m_pGridMC2->SetDrawScrollBarVert(FALSE);
	if (!m_pGridMC2->Create(rGrid, this, IDC_MAIN_GRID_MC2, dwStyle))
	{
		delete m_pGridMC2;
		m_pGridMC2 = NULL;
		return;
	}

	/* Set default values */
	m_pGridMC2->SetLogFont(g_lf);
	m_pGridMC2->SetRowCount(i32RowCnt+1);
	m_pGridMC2->SetColumnCount(i32ColCnt);
	m_pGridMC2->SetFixedRowCount(1);
	m_pGridMC2->SetRowHeight(0, 28);
	m_pGridMC2->SetFixedColumnCount(0);
	m_pGridMC2->SetBkColor(RGB(255, 255, 255));
	m_pGridMC2->SetFixedColumnSelection(0);

	/* Set the common control */
	stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	stGV.crFgClr = RGB(0, 0, 0);

	/* Header Control (First row) */
	stGV.crBkClr = RGB(214, 214, 214);
	stGV.lfFont	= g_lf;
	for (i=0; i<i32ColCnt; i++)
	{
		stGV.nFormat= DT_VCENTER | DT_SINGLELINE | DT_CENTER;
		stGV.row	= 0;
		stGV.col	= i;
		stGV.strText= tzCols[i];
		m_pGridMC2->SetItem(&stGV);
		m_pGridMC2->SetColumnWidth(i, i32Width[i]);
	}

	/* body background color */
	stGV.crBkClr = RGB(255, 255, 255);

	/* body setting - for Motor Position / Servo / Error */
	if (0x01 == pstCfg->luria_svc.z_drive_type)
	{
		for (i=0; i<pstCfg->luria_svc.ph_count; i++,j++)
		{
			m_pGridMC2->SetRowHeight(j, 24);
			stGV.row	= j;

			/* for Item */
			stGV.strText.Format(L"PH  %d", i+1);
			stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
			stGV.col	= 0;
			m_pGridMC2->SetItem(&stGV);

			/* for Position */
			stGV.strText= L"0.0000";
			stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
			stGV.row	= j;
			stGV.col	= 1;
			m_pGridMC2->SetItem(&stGV);

			/* for Servo State & error */
			stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;

			stGV.strText= L"Locked";
			stGV.col	= 2;
			m_pGridMC2->SetItem(&stGV);

			stGV.strText= L"0";
			stGV.col	= 3;
			m_pGridMC2->SetItem(&stGV);
		}
	}
	/* body setting - for Motion Drive Position */
	for (i=0; i<pstCfg->mc2_svc.drive_count; i++,j++)
	{
			m_pGridMC2->SetRowHeight(j, 24);
			stGV.row	= j;

			/* for Item */
			stGV.strText.Format(L"MC2 %d", uvEng_GetConfig()->mc2_svc.axis_id[i]);
			stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
			stGV.col	= 0;
			m_pGridMC2->SetItem(&stGV);

			/* for Position */
			stGV.strText= L"0.0000";
			stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
			stGV.row	= j;
			stGV.col	= 1;
			m_pGridMC2->SetItem(&stGV);

			/* for Servo State & error */
			stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;

			stGV.strText= L"Locked";
			stGV.col	= 2;
			m_pGridMC2->SetItem(&stGV);

			stGV.strText= L"0";
			stGV.col	= 3;
			m_pGridMC2->SetItem(&stGV);
	}
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
	/* body setting - for Align Camera Z Axis */
	for (i=0; i<pstCfg->set_cams.acam_count; i++,j++)
	{
			m_pGridMC2->SetRowHeight(j, 24);
			stGV.row	= j;

			/* for Item */
			stGV.strText.Format(L"ACAM %d", i+1);
			stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
			stGV.col	= 0;
			m_pGridMC2->SetItem(&stGV);

			/* for Position */
			stGV.strText= L"0.0000";
			stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
			stGV.row	= j;
			stGV.col	= 1;
			m_pGridMC2->SetItem(&stGV);

			/* for error */
			stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;

			stGV.strText= L"None";
			stGV.col	= 2;
			m_pGridMC2->SetItem(&stGV);

			stGV.strText= L"0";
			stGV.col	= 3;
			m_pGridMC2->SetItem(&stGV);
	}
#endif
	m_pGridMC2->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1);
	m_pGridMC2->UpdateGrid();
}

/*
 desc : Create a grid control
 parm : None
 retn : None
*/
VOID CDlgMain::InitGridMeas()
{
	TCHAR tzCols[7][16]	= { L"YYYY-MM-DD", L"Stage X", L"Stage Y", L"Diff. X", L"Diff. Y", L"Offset X", L"Offset Y" };
	INT32 i32Width[7]	= { 90, 66, 66, 65, 65, 75, 75 }, i, j=1;
	INT32 i32ColCnt		= 7, i32RowCnt = 0;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	LPG_CIEA pstCfg		= uvEng_GetConfig();
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* Last Recv. Packet / PDO Header / PDO Data (Values) */
	::GetWindowRect(m_grp_ctl[1].GetSafeHwnd(), rGrid);
	ScreenToClient(rGrid);
	rGrid.left	+= 13;
	rGrid.right	-= 13;
	rGrid.top	+= 20;
	rGrid.bottom-= 13;

	/* Basic properties of gird controls */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);
	stGV.crBkClr = RGB(214, 214, 214);
	stGV.lfFont	= g_lf;

	/* Create a object */
	m_pGridMeas	= new CGridCtrl;
	ASSERT(m_pGridMeas);
	m_pGridMeas->SetModifyStyleEx(WS_EX_STATICEDGE);
	m_pGridMeas->SetDrawScrollBarHorz(FALSE);
	m_pGridMeas->SetDrawScrollBarVert(TRUE);
	if (!m_pGridMeas->Create(rGrid, this, IDC_MAIN_GRID_MEAS, dwStyle))
	{
		delete m_pGridMeas;
		m_pGridMeas = NULL;
		return;
	}

	/* Set default values */
	m_pGridMeas->SetLogFont(g_lf);
	m_pGridMeas->SetRowCount(i32RowCnt+1);
	m_pGridMeas->SetColumnCount(i32ColCnt);
	m_pGridMeas->SetFixedRowCount(1);
	m_pGridMeas->SetRowHeight(0, 25);
	m_pGridMeas->SetFixedColumnCount(0);
	m_pGridMeas->SetBkColor(RGB(255, 255, 255));
	m_pGridMeas->SetFixedColumnSelection(0);

	/* Set the common control */
	stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	stGV.crFgClr = RGB(0, 0, 0);

	/* Header Control (First row) */
	for (i=0; i<i32ColCnt; i++)
	{
		stGV.nFormat= DT_VCENTER | DT_SINGLELINE | DT_CENTER;
		stGV.row	= 0;
		stGV.col	= i;
		stGV.strText= tzCols[i];
		m_pGridMeas->SetItem(&stGV);
		m_pGridMeas->SetColumnWidth(i, i32Width[i]);
	}

	m_pGridMeas->SetBaseGridProp(1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
	m_pGridMeas->UpdateGrid();
}

/*
 desc : Release the grid control
 parm : None
 retn : None
*/
VOID CDlgMain::CloseGrid()
{
	if (m_pGridMC2)
	{
		if (m_pGridMC2->GetSafeHwnd())	m_pGridMC2->DestroyWindow();
		delete m_pGridMC2;
	}
	if (m_pGridMeas)
	{
		if (m_pGridMeas->GetSafeHwnd())	m_pGridMeas->DestroyWindow();
		delete m_pGridMeas;
	}
}

/*
 desc : Sets the default value for the controls
 parm : None
 retn : None
*/
VOID CDlgMain::InitData()
{
	m_edt_int[0].SetTextToNum(135);				/* Measurement Count */

	m_edt_flt[0].SetTextToNum(100.0f, 4);		/* Distance */
	m_edt_flt[1].SetTextToNum(100.0f, 4);		/* Velocity */
	m_edt_flt[2].SetTextToNum(4.0f, 4);			/* Measurement Interval */
	m_edt_flt[3].SetTextToNum(8.1f, 4);			/* Material Thicknesss (Soda Glass) */

	m_edt_flt[4].SetTextToNum(9.9998f, 4);		/* Measurement Start X position */
	m_edt_flt[5].SetTextToNum(1304.8800, 4);	/* Measurement Start Y position */
	m_edt_flt[6].SetTextToNum(410.9998, 4);		/* Measurement ACam X position */

	if (uvEng_GetConfig()->IsRunDemo())
	{
		/* Start : Measurement Offset XY */
		m_edt_flt[9].SetTextToNum(0.0f, 4);
		m_edt_flt[10].SetTextToNum(0.0f, 4);
		/* Stop : Measurement Offset XY */
		m_edt_flt[11].SetTextToNum(0.0f, 4);
		m_edt_flt[12].SetTextToNum(0.0f, 4);
	}
}

/*
 desc : Only used in the demo version
 parm : None
 retn : None
*/
VOID CDlgMain::InitDemo()
{
	m_vMarkX.push_back(-0.2);		m_vMarkY.push_back(0.1);
	m_vMarkX.push_back(-1.6);		m_vMarkY.push_back(0.2);
	m_vMarkX.push_back(-2.9);		m_vMarkY.push_back(0.6);
	m_vMarkX.push_back(-2.9);		m_vMarkY.push_back(1.0);
	m_vMarkX.push_back(-3.0);		m_vMarkY.push_back(1.0);
	m_vMarkX.push_back(-3.2);		m_vMarkY.push_back(1.4);
	m_vMarkX.push_back(-2.2);		m_vMarkY.push_back(1.4);
	m_vMarkX.push_back(-3.0);		m_vMarkY.push_back(1.4);
	m_vMarkX.push_back(-4.0);		m_vMarkY.push_back(1.7);
	m_vMarkX.push_back(-4.9);		m_vMarkY.push_back(2.2);
	m_vMarkX.push_back(-6.7);		m_vMarkY.push_back(2.3);
	m_vMarkX.push_back(-9.5);		m_vMarkY.push_back(2.8);
	m_vMarkX.push_back(-11.6);		m_vMarkY.push_back(2.8);
	m_vMarkX.push_back(-13.8);		m_vMarkY.push_back(3.5);
	m_vMarkX.push_back(-16.5);		m_vMarkY.push_back(4.0);
	m_vMarkX.push_back(-18.7);		m_vMarkY.push_back(4.1);
	m_vMarkX.push_back(-20.6);		m_vMarkY.push_back(5.0);
	m_vMarkX.push_back(-22.2);		m_vMarkY.push_back(5.4);
	m_vMarkX.push_back(-23.1);		m_vMarkY.push_back(6.0);
	m_vMarkX.push_back(-23.7);		m_vMarkY.push_back(6.0);
	m_vMarkX.push_back(-23.3);		m_vMarkY.push_back(6.2);
	m_vMarkX.push_back(-22.6);		m_vMarkY.push_back(6.1);
	m_vMarkX.push_back(-23.3);		m_vMarkY.push_back(6.4);
	m_vMarkX.push_back(-22.8);		m_vMarkY.push_back(6.5);
	m_vMarkX.push_back(-22.8);		m_vMarkY.push_back(6.6);
	m_vMarkX.push_back(-23.4);		m_vMarkY.push_back(6.9);
	m_vMarkX.push_back(-24.1);		m_vMarkY.push_back(7.1);
	m_vMarkX.push_back(-24.7);		m_vMarkY.push_back(7.8);
	m_vMarkX.push_back(-25.5);		m_vMarkY.push_back(8.0);
	m_vMarkX.push_back(-25.8);		m_vMarkY.push_back(8.7);
	m_vMarkX.push_back(-26.6);		m_vMarkY.push_back(9.4);
	m_vMarkX.push_back(-26.8);		m_vMarkY.push_back(10.0);
	m_vMarkX.push_back(-26.3);		m_vMarkY.push_back(10.5);
	m_vMarkX.push_back(-25.5);		m_vMarkY.push_back(10.8);
	m_vMarkX.push_back(-24.0);		m_vMarkY.push_back(10.6);
	m_vMarkX.push_back(-21.6);		m_vMarkY.push_back(10.9);
	m_vMarkX.push_back(-18.4);		m_vMarkY.push_back(11.1);
	m_vMarkX.push_back(-15.9);		m_vMarkY.push_back(11.2);
	m_vMarkX.push_back(-13.7);		m_vMarkY.push_back(11.1);
	m_vMarkX.push_back(-12.0);		m_vMarkY.push_back(11.4);
	m_vMarkX.push_back(-10.8);		m_vMarkY.push_back(11.7);
	m_vMarkX.push_back(-9.5);		m_vMarkY.push_back(12.2);
	m_vMarkX.push_back(-8.9);		m_vMarkY.push_back(12.8);
	m_vMarkX.push_back(-7.9);		m_vMarkY.push_back(12.8);
	m_vMarkX.push_back(-7.0);		m_vMarkY.push_back(13.7);
	m_vMarkX.push_back(-6.1);		m_vMarkY.push_back(14.3);
	m_vMarkX.push_back(-5.3);		m_vMarkY.push_back(14.5);
	m_vMarkX.push_back(-3.5);		m_vMarkY.push_back(15.3);
	m_vMarkX.push_back(-2.0);		m_vMarkY.push_back(15.4);
	m_vMarkX.push_back(-0.1);		m_vMarkY.push_back(15.6);
	m_vMarkX.push_back(2.4);		m_vMarkY.push_back(15.6);
	m_vMarkX.push_back(4.4);		m_vMarkY.push_back(15.5);
	m_vMarkX.push_back(6.0);		m_vMarkY.push_back(15.7);
	m_vMarkX.push_back(7.3);		m_vMarkY.push_back(15.1);
	m_vMarkX.push_back(8.0);		m_vMarkY.push_back(15.2);
	m_vMarkX.push_back(8.1);		m_vMarkY.push_back(15.6);
	m_vMarkX.push_back(7.7);		m_vMarkY.push_back(15.8);
	m_vMarkX.push_back(7.3);		m_vMarkY.push_back(15.7);
	m_vMarkX.push_back(7.0);		m_vMarkY.push_back(16.2);
	m_vMarkX.push_back(6.4);		m_vMarkY.push_back(16.4);
	m_vMarkX.push_back(5.7);		m_vMarkY.push_back(17.0);
	m_vMarkX.push_back(3.8);		m_vMarkY.push_back(17.3);
	m_vMarkX.push_back(2.5);		m_vMarkY.push_back(18.1);
	m_vMarkX.push_back(1.9);		m_vMarkY.push_back(18.2);
	m_vMarkX.push_back(1.7);		m_vMarkY.push_back(18.6);
	m_vMarkX.push_back(1.7);		m_vMarkY.push_back(19.1);
	m_vMarkX.push_back(2.2);		m_vMarkY.push_back(18.6);
	m_vMarkX.push_back(2.4);		m_vMarkY.push_back(18.6);
	m_vMarkX.push_back(1.3);		m_vMarkY.push_back(18.4);
	m_vMarkX.push_back(0.5);		m_vMarkY.push_back(18.6);
	m_vMarkX.push_back(-1.1);		m_vMarkY.push_back(19.1);
	m_vMarkX.push_back(-3.8);		m_vMarkY.push_back(19.3);
	m_vMarkX.push_back(-4.9);		m_vMarkY.push_back(19.1);
	m_vMarkX.push_back(-6.7);		m_vMarkY.push_back(19.5);
	m_vMarkX.push_back(-7.8);		m_vMarkY.push_back(20.1);
	m_vMarkX.push_back(-8.4);		m_vMarkY.push_back(20.6);
	m_vMarkX.push_back(-9.3);		m_vMarkY.push_back(20.9);
	m_vMarkX.push_back(-9.4);		m_vMarkY.push_back(21.1);
	m_vMarkX.push_back(-8.8);		m_vMarkY.push_back(21.7);
	m_vMarkX.push_back(-7.9);		m_vMarkY.push_back(21.5);
	m_vMarkX.push_back(-5.9);		m_vMarkY.push_back(21.7);
	m_vMarkX.push_back(-4.8);		m_vMarkY.push_back(21.3);
	m_vMarkX.push_back(-4.5);		m_vMarkY.push_back(21.4);
	m_vMarkX.push_back(-4.0);		m_vMarkY.push_back(21.7);
	m_vMarkX.push_back(-4.5);		m_vMarkY.push_back(21.6);
	m_vMarkX.push_back(-5.4);		m_vMarkY.push_back(21.9);
	m_vMarkX.push_back(-6.6);		m_vMarkY.push_back(22.1);
	m_vMarkX.push_back(-8.4);		m_vMarkY.push_back(22.7);
	m_vMarkX.push_back(-10.1);		m_vMarkY.push_back(23.1);
	m_vMarkX.push_back(-11.3);		m_vMarkY.push_back(23.6);
	m_vMarkX.push_back(-12.9);		m_vMarkY.push_back(23.9);
	m_vMarkX.push_back(-14.6);		m_vMarkY.push_back(24.5);
	m_vMarkX.push_back(-14.7);		m_vMarkY.push_back(25.3);
	m_vMarkX.push_back(-14.4);		m_vMarkY.push_back(25.7);
	m_vMarkX.push_back(-13.8);		m_vMarkY.push_back(26.1);
	m_vMarkX.push_back(-12.5);		m_vMarkY.push_back(25.9);
	m_vMarkX.push_back(-11.3);		m_vMarkY.push_back(25.6);
	m_vMarkX.push_back(-11.2);		m_vMarkY.push_back(25.9);
	m_vMarkX.push_back(-10.5);		m_vMarkY.push_back(26.3);
	m_vMarkX.push_back(-10.2);		m_vMarkY.push_back(26.0);
	m_vMarkX.push_back(-11.0);		m_vMarkY.push_back(26.3);
	m_vMarkX.push_back(-11.6);		m_vMarkY.push_back(26.6);
	m_vMarkX.push_back(-11.7);		m_vMarkY.push_back(27.3);
	m_vMarkX.push_back(-12.8);		m_vMarkY.push_back(27.5);
	m_vMarkX.push_back(-13.7);		m_vMarkY.push_back(28.1);
	m_vMarkX.push_back(-14.2);		m_vMarkY.push_back(28.5);
	m_vMarkX.push_back(-14.3);		m_vMarkY.push_back(29.2);
	m_vMarkX.push_back(-13.5);		m_vMarkY.push_back(29.8);
	m_vMarkX.push_back(-12.3);		m_vMarkY.push_back(30.0);
	m_vMarkX.push_back(-10.9);		m_vMarkY.push_back(30.6);
	m_vMarkX.push_back(-8.4);		m_vMarkY.push_back(30.3);
	m_vMarkX.push_back(-5.7);		m_vMarkY.push_back(30.1);
	m_vMarkX.push_back(-4.4);		m_vMarkY.push_back(30.1);
	m_vMarkX.push_back(-3.3);		m_vMarkY.push_back(30.4);
	m_vMarkX.push_back(-2.7);		m_vMarkY.push_back(29.9);
	m_vMarkX.push_back(-3.3);		m_vMarkY.push_back(30.5);
	m_vMarkX.push_back(-3.9);		m_vMarkY.push_back(30.6);
	m_vMarkX.push_back(-4.4);		m_vMarkY.push_back(31.3);
	m_vMarkX.push_back(-4.8);		m_vMarkY.push_back(32.0);
	m_vMarkX.push_back(-6.0);		m_vMarkY.push_back(32.5);
	m_vMarkX.push_back(-5.4);		m_vMarkY.push_back(33.0);
	m_vMarkX.push_back(-5.8);		m_vMarkY.push_back(33.2);
	m_vMarkX.push_back(-5.9);		m_vMarkY.push_back(34.0);
	m_vMarkX.push_back(-5.4);		m_vMarkY.push_back(33.9);
	m_vMarkX.push_back(-3.8);		m_vMarkY.push_back(34.6);
	m_vMarkX.push_back(-1.6);		m_vMarkY.push_back(34.8);
	m_vMarkX.push_back(-0.2);		m_vMarkY.push_back(34.6);
	m_vMarkX.push_back(0.6);		m_vMarkY.push_back(34.4);
	m_vMarkX.push_back(0.2);		m_vMarkY.push_back(34.6);
	m_vMarkX.push_back(-0.1);		m_vMarkY.push_back(35.2);
	m_vMarkX.push_back(-0.8);		m_vMarkY.push_back(34.9);
	m_vMarkX.push_back(-2.0);		m_vMarkY.push_back(35.1);
	m_vMarkX.push_back(-3.1);		m_vMarkY.push_back(35.5);
	m_vMarkX.push_back(-4.2);		m_vMarkY.push_back(35.9);
	m_vMarkX.push_back(-4.3);		m_vMarkY.push_back(36.1);

	m_edt_flt[9].SetTextToNum(-0.0002f, 4);
	m_edt_flt[10].SetTextToNum(0.0001f, 4);
	m_edt_flt[11].SetTextToNum(-0.0043f, 4);
	m_edt_flt[12].SetTextToNum(0.0361f, 4);
}

/*
 desc : Initialize the Engine library
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::InitLib()
{
	return uvEng_InitEx(ENG_ERVM::en_stage_st, FALSE);
//	return uvEng_InitEx(ENG_ERVM::en_cmps_sw, FALSE);
}

/*
 desc : Release the Engine library
 parm : None
 retn : None
*/
VOID CDlgMain::CloseLib()
{
	uvEng_Close();
}

/*
 desc : Process events generated from the main-thread
 parm : wparam	- [in]  Event ID
		lparam	- [in]  Event Data
 retn : 0L
*/
LRESULT CDlgMain::OnMsgThread(WPARAM wparam, LPARAM lparam)
{
	if (!uvEng_GetConfig())	return 0L;

	UpdatePeriod();
	UpdateWorkStep();

	return 0L;
}

/*
 desc : Update item values periodically
 parm : None
 retn : None
*/
VOID CDlgMain::UpdatePeriod()
{
	UINT64 u64Tick	= GetTickCount64();	/* Get the current time of CPU */

	if (m_u64TickCtrl+500 < u64Tick)	{	m_u64TickCtrl = u64Tick;	EnableControls();	}
	if (m_u64TickText+450 < u64Tick)	{	m_u64TickText = u64Tick;	UpdateControls();	}
	if (m_u64TickLogs+2500< u64Tick)	{	m_u64TickLogs = u64Tick;	UpdateLogMesg();	}
	if (m_u64TickLive+600 < u64Tick)	{	m_u64TickLive = u64Tick;	UpdateLiveView();	}
}

/*
 desc : Enable or Disable for Controls
 parm : None
 retn : None
*/
VOID CDlgMain::EnableControls()
{
	UINT8 i = 0, j	= 0x00;
	BOOL bIsValid	= FALSE, bIsBusy = m_pWorkCent->IsBusy() || uvCmn_MC2_IsAnyDriveBusy();
	BOOL bLiveMode	= m_chk_ctl[0].GetCheck() == 1;
	BOOL bMC2		= uvCmn_MC2_IsConnected();
	BOOL bPLC		= uvCmn_MCQ_IsConnected();
	BOOL bLuria		= uvCmn_Luria_IsConnected();
	BOOL bLuiraInit	= uvEng_Luria_IsServiceInited();
	BOOL bEmulated	= uvEng_GetConfig()->luria_svc.IsRunEmulated();
	BOOL bRunDemo	= uvEng_GetConfig()->IsRunDemo();
	BOOL bSelectMC2	= m_pGridMC2->GetSelectedCount() > 0;
	LPG_LDDP pstDP	= &uvEng_ShMem_GetLuria()->directph;		/* Get the sahred memory of luria's direct_photohead */

	m_btn_ctl[1].EnableWindow((bPLC && bMC2 || bRunDemo) && !bIsBusy && !bLiveMode);		/* Start */
	m_btn_ctl[2].EnableWindow((bPLC && bMC2 || bRunDemo) && bIsBusy && !bLiveMode);			/* Stop */
	m_btn_ctl[3].EnableWindow(!bIsBusy && !bLiveMode);										/* Save the measured grid data */
	m_btn_ctl[4].EnableWindow((!bIsBusy && bMC2 || bRunDemo) && !bIsBusy && !bLiveMode);	/* Unload the stage */
	m_btn_ctl[5].EnableWindow(!bIsBusy);													/* Thickness */
//	m_btn_ctl[6].EnableWindow(bRunDemo);													/* Reset the Error */
	m_btn_ctl[6].EnableWindow(bSelectMC2 && bMC2 && !bIsBusy);								/* Move Up */
	m_btn_ctl[7].EnableWindow(bSelectMC2 && bMC2 && !bIsBusy);								/* Move Down */
	m_btn_ctl[9].EnableWindow(!bIsBusy && bMC2 && !bIsBusy);								/* Home All */
	m_btn_ctl[10].EnableWindow(!bIsBusy && bMC2 && !bIsBusy);								/* Reset All */
	m_btn_ctl[11].EnableWindow(!bIsBusy && bMC2 && !bIsBusy);								/* Go to measurement start position */
	m_btn_ctl[12].EnableWindow(!bIsBusy && bMC2 && !bIsBusy);								/* Go to measurement position */
	m_btn_ctl[13].EnableWindow((!bIsBusy && bMC2 || bRunDemo) && !bIsBusy && !bLiveMode);	/* Match Start - Measurement Test */
	m_btn_ctl[14].EnableWindow((!bIsBusy && bMC2 || bRunDemo) && !bIsBusy && !bLiveMode);	/* Match Stop - Measurement Test */

	m_chk_ctl[0].EnableWindow(!bIsBusy);													/* Live view mode */
	m_chk_ctl[1].EnableWindow(!bIsBusy);													/* Measurement X */
	m_chk_ctl[2].EnableWindow(!bIsBusy);													/* Measurement Y */
	m_chk_ctl[3].EnableWindow(!bIsBusy);													/* Measurement Target */
	m_edt_int[0].EnableWindow(!bIsBusy);													/* Measurement Count */
	m_edt_flt[2].EnableWindow(!bIsBusy);													/* Measurement Interval */
	m_edt_flt[3].EnableWindow(!bIsBusy);													/* Measurement Thickness */
}

/*
 desc : Update the log message
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateControls()
{
	/* Update the step name of the work */
	UpdateStepName();
	/* Updates the motion position & errors */
	if (uvCmn_MC2_IsConnected())	UpdateMotion();
	/* Update the air-pressure ... */
	if (uvCmn_MCQ_IsConnected())	UpdateAirPressure();
}

/*
 desc : OnNotifyGridSelechanged Event
 parm : id		- [in] Grid Control ID
		nmhdr	- [in] the handle and ID of the control sending the message and the notification code
		result	- [in]  A pointer to the LRESULT variable to store the result code if the message has been handled.
 retn : None
*/
VOID CDlgMain::OnGridSelChanged(UINT32 id, NMHDR *nmhdr, LRESULT *result)
{
	BOOL bEnabled, bMC2	= uvCmn_MC2_IsConnected();
	INT32 i32Row, i32Col=0, i32Drv = uvEng_GetConfig()->mc2_svc.drive_count + uvEng_GetConfig()->luria_svc.ph_count;
	NM_GRIDVIEW *pItem	= (NM_GRIDVIEW*)nmhdr;
	CString strDrv;

	/* Return the index of the selected row and column */
	i32Row	= pItem->iRow;
	i32Col	= pItem->iColumn;
	bEnabled= i32Row > 0;

	if (IDC_MAIN_GRID_MC2 == id)
	{
		m_btn_ctl[4].EnableWindow(bEnabled);
		m_btn_ctl[5].EnableWindow(bEnabled);
		if (!bEnabled)
		{
			m_pGridMC2->SetSelectedRange(-1, -1, -1, -1);
			m_pGridMC2->SetFocusCell(-1, -1);
			m_pGridMC2->Invalidate();
			m_edt_flt[1].SetTextToNum(0.0f, 4);
		}
		else
		{
			/* In case of PH */
			if (i32Row <= uvEng_GetConfig()->luria_svc.ph_count)
			{
				m_edt_flt[1].SetTextToNum(0.0f, 4);
			}
			/* In case of MC2 */
			else if (i32Row <= 6)
			{
				strDrv	= m_pGridMC2->GetItemText(i32Row, 0);
				i32Drv	= strDrv.GetAt(4) - L'0';
				m_edt_flt[1].SetTextToNum(uvEng_GetConfig()->mc2_svc.max_velo[i32Drv], 4);
			}
			/* In case of ACam Z Axis */
			else
			{
				m_edt_flt[1].SetTextToNum(0.0f, 4);
			}
		}
	}
	else if (IDC_MAIN_GRID_MEAS == id)
	{
	}

	*result = 0;	/* fixed */
}

/*
 desc : Called when the button is clicked
 parm : id	- [in]  button id clicked
 retn : None
*/
VOID CDlgMain::OnBtnClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_MAIN_BTN_EXIT			:	PostMessage(WM_CLOSE, 0, 0L);		break;
	case IDC_MAIN_BTN_ERROR			:	ResetErroMessage();					break;
	case IDC_MAIN_BTN_HOMED_ALL		:	uvEng_MC2_SendDevHomingAll();		break;
	case IDC_MAIN_BTN_RESET_ALL		:	uvEng_MC2_SendDevFaultResetAll();	break;
	case IDC_MAIN_BTN_UNLOAD		:	UnloadPosition();					break;
	case IDC_MAIN_BTN_THICK			:	SetDofFilmThick();					break;
	/* Move the Motor (or Stage) */
	case IDC_MAIN_BTN_MOVE_UP		:	MoveToMotor(0x00);					break;
	case IDC_MAIN_BTN_MOVE_DOWN		:	MoveToMotor(0x01);					break;
	/* Set the start position of stage xy */
	case IDC_MAIN_BTN_MEAS_START	:	SetStartXY();						break;
	/* Start/Stop the work step */
	case IDC_MAIN_BTN_RUN_START		:	StartWork();						break;
	case IDC_MAIN_BTN_RUN_STOP		:	StopWork();							break;
	/* Move to the start / Stop Postion */
	case IDC_MAIN_BTN_MEAS_MOVE		:	MoveToMeas();						break;
	/* Test the mark inspection */
	case IDC_MAIN_BTN_MATCH_START	:	SetBaseMatch(0x00);					break;
	case IDC_MAIN_BTN_MATCH_STOP	:	SetBaseMatch(0x01);					break;
	case IDC_MAIN_BTN_SAVE			:	SaveGridFile();						break;
	}
}

/*
 desc : Called when the checkbox is clicked
 parm : id	- [in]  checkbox id clicked
 retn : None
*/
VOID CDlgMain::OnChkClicked(UINT32 id)
{
	if (m_chk_ctl[1].GetCheck() == 1 &&
		m_chk_ctl[2].GetCheck() == 1)
	{
		m_chk_ctl[3].SetCheck(0);
		m_chk_ctl[3].SetWindowTextW(L"Stage X");
	}
	switch (id)
	{
	case IDC_MAIN_CHK_LIVE			:	SetLiveView();	break;
	case IDC_MAIN_CHK_MEAS_X		:
	case IDC_MAIN_CHK_MEAS_Y		:	CalcMeasXY();	break;
	case IDC_MAIN_CHK_MEAS_TARGET	:	ChangeTarget();
										CalcMeasXY();	break;
	}
}

/*
 desc : Reset the error message
 parm : None
 retn : None
*/
VOID CDlgMain::ResetErroMessage()
{
	uvCmn_Luria_ResetLastErrorStatus();
	uvCmn_Logs_ResetErrorMesg();
	m_edt_txt[0].SetWindowTextW(L"");
}

/*
 desc : This is an event that occurs when the value of edit box changes
 parm : None
 retn : None
*/
VOID CDlgMain::OnEnChangeSetMeasVal()
{
	CalcMeasXY();
}

/*
 desc : Calculates the measurement position
 parm : None
 retn : None
*/
VOID CDlgMain::CalcMeasXY()
{
	DOUBLE dbMoveX	= 0.0f, dbMoveY = 0.0f, dbDist = 0.0f;

	if (m_edt_int[0].GetTextToNum() < 1)
	{
		m_edt_flt[7].SetTextToNum(0.0f, 4);
		m_edt_flt[8].SetTextToNum(0.0f, 4);
	}
	else
	{
		dbMoveX	= m_edt_flt[4].GetTextToDouble();
		if (m_chk_ctl[3].GetCheck() == 1)	dbMoveX	= m_edt_flt[6].GetTextToDouble();
		dbMoveY	= m_edt_flt[5].GetTextToDouble();
		dbDist	= m_edt_flt[2].GetTextToDouble() * DOUBLE(m_edt_int[0].GetTextToNum() - 1);

		/* Calculate the move x/y */
		if (1 == m_chk_ctl[1].GetCheck())	dbMoveX	+= dbDist;
		if (1 == m_chk_ctl[2].GetCheck())	dbMoveY	+= dbDist;

		/* Update the move position */
		m_edt_flt[7].SetTextToNum(dbMoveX, 4);
		m_edt_flt[8].SetTextToNum(dbMoveY, 4);
	}
}

/*
 desc : Move the motor (or stage)
 parm : direct	- [in]  The movement direct value (0x00: : up, 0x01: down)
 retn : None
*/
VOID CDlgMain::MoveToMotor(UINT8 direct)
{
	ENG_MMDI enDrv	= ENG_MMDI::en_axis_none;
	DOUBLE dbDist	= m_edt_flt[0].GetTextToDouble();
	DOUBLE dbVelo	= m_edt_flt[1].GetTextToDouble();
	DOUBLE dbMove	= 0.0f, dbSign = 1.0f;
	LPG_CMSI pstMC2	= &uvEng_GetConfig()->mc2_svc;

	/* Get the selected item (drive number) */
	LPPOINT ptSel	= m_pGridMC2->GetSelPos();
	if (!ptSel)	return;
	if (ptSel->x < 1)	return;

	/* Get the motor number according to the direction of movement of the stage */
	switch (ptSel->x)
	{
	/* In case of PH, ACam 1/2 Z */
	case 0x01 :
	case 0x02 :
	case 0x07 :
	case 0x08 :
		switch (direct)
		{
		case 0x00 : dbSign = 1.0f;	break;	/* Up */
		case 0x01 : dbSign = -1.0f;	break;	/* Down */
		}	break;
	/* In case of MC2 */
	case 0x03 :
	case 0x05 :
	case 0x06 :
		/* In case of Stage X, ACam1/2 X */
		switch (direct)
		{
		case 0x00 : dbSign = -1.0f;	break;	/* Left */
		case 0x01 : dbSign = 1.0f;	break;	/* Right */
		}	break;
		/* In case of Stage Y */
	case 0x04 :
		switch (direct)
		{
		case 0x00 : dbSign = -1.0f;	break;	/* Down */
		case 0x01 : dbSign = 1.0f;	break;	/* Up */
		}	break;
	}

	if (ptSel->x < 3)
	{
		dbMove	= uvCmn_Luria_GetPhZAxisPosition(UINT8(ptSel->x)) + dbDist * dbSign;
		uvEng_Luria_ReqSetMotorAbsPosition(UINT8(ptSel->x), dbMove);
	}
	else if (ptSel->x < 7)
	{
		/* Get the current position of the motor (MC2) + Move the distance value */
		switch (ptSel->x)
		{
		case 0x03	:	enDrv	= ENG_MMDI::en_stage_x;		break;
		case 0x04	:	enDrv	= ENG_MMDI::en_stage_y;		break;
		case 0x05	:	enDrv	= ENG_MMDI::en_align_cam1;	break;
		case 0x06	:	enDrv	= ENG_MMDI::en_align_cam2;	break;
		}
		dbMove	= uvCmn_MC2_GetDrvAbsPos(enDrv) + dbDist * dbSign;
		if (pstMC2->min_dist[UINT8(enDrv)] > dbMove)	dbMove = pstMC2->min_dist[UINT8(enDrv)];
		if (pstMC2->max_dist[UINT8(enDrv)] < dbMove)	dbMove = pstMC2->max_dist[UINT8(enDrv)];
		/* Check if the maximum shift value is out of range */
		if (!uvEng_MC2_SendDevAbsMove(enDrv, dbMove, dbVelo))	return;
	}
	else
	{
		dbMove	= uvCmn_MCQ_GetACamCurrentPosZ(UINT8(ptSel->x - 0x06)) + dbDist * dbSign;
		uvEng_MCQ_SetACamMovePosZ(UINT8(ptSel->x - 0x06), 0x00, dbMove);
	}
}

/*
 desc : Update the air-pressure
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateAirPressure()
{
	UINT8 u8Alarm		= 0x00;
	TCHAR tzAir[32]		= {NULL};	/* Unit : kPa */
	UINT16 u16AirPress	= 0;

#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
	swprintf_s(tzAir, 32, L"--- kPa");
#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
	u8Alarm		= uvCmn_MCQ_GetBitsValueEx(ENG_PIOA::en_z1_z3_air_pressure_low_alarm);
	u16AirPress	= uvCmn_MCQ_GetWordValueEx(ENG_PIOA::en_z1_z3_axis_cylinder_air_pressure);
	swprintf_s(tzAir, 32, L"%u kPa", u16AirPress);
#endif
	COLORREF clrTxt[2]	= { RGB(255, 0, 0), RGB(0, 0, 0) };
	if (!m_txt_ctl[1].IsCompareText(tzAir))
	{
		m_txt_ctl[1].SetRedraw(FALSE);
		m_txt_ctl[1].SetWindowText(tzAir);
		m_txt_ctl[1].SetTextColor(clrTxt[u8Alarm]);
		m_txt_ctl[1].SetRedraw(TRUE);
		m_txt_ctl[1].Invalidate(FALSE);
	}
}

/*
 desc : Update the log message
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateLogMesg()
{
	if (!uvCmn_Logs_IsErrorMesg())
	{
		if (m_edt_txt[0].GetTextToLen() > 0)	m_edt_txt[0].SetWindowTextW(L"");
	}
	else
	{
		/* get the error message from log buffer */
		PTCHAR ptzMsg = uvCmn_Logs_GetErrorMesg(m_u8ErrIdx);
		if (!ptzMsg)
		{
			ptzMsg = uvCmn_Logs_GetErrorMesg(0x00);
			m_u8ErrIdx	= 0x00;
		}
		m_edt_txt[0].SetWindowTextW(ptzMsg);
		m_u8ErrIdx++;
	}
}

/*
 desc : Update the motion positions and erros
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateMotion()
{
	UINT8 i, j		= 0x01, u8Drv;
	TCHAR tzVal[32]	= {NULL}, tzLock[2][8] = {L"Unlock", L"Locked" };
	TCHAR tzReady[2][8]	= {L"Busy", L"Ready" }, tzAlarm[2][8] = {L"None", L"Alarm" };
	LPG_CMSI pstMC2	= &uvEng_GetConfig()->mc2_svc;

	/* Disable the Redraw */
	m_pGridMC2->SetRedraw(FALSE);

	/* Photohead Information */
	for (i=0x00; i<uvEng_GetConfig()->luria_svc.ph_count; i++,j++)
	{
		/* Position */
		swprintf_s(tzVal, 32, L"%.4f", uvCmn_Luria_GetPhZAxisPosition(i+1));
		m_pGridMC2->SetItemText(j, 1, tzVal);
		/* Servo Lock & Error */
		if (0x01 == uvEng_ShMem_GetLuria()->machine.z_drive_type)	/* In case of stepper motor */
		{
			m_pGridMC2->SetItemText(j, 3, L"");
			if (uvEng_ShMem_GetLuria()->system.IsErrorPH(i+1))	m_pGridMC2->SetItemText(j, 3, L"Error");
			else												m_pGridMC2->SetItemText(j, 3, L"None");
		}
		else if (0x03 == uvEng_ShMem_GetLuria()->machine.z_drive_type)	/* In case of linear motor */
		{
			u8Drv	= uvEng_GetConfig()->luria_svc.z_drive_sd2s_ph[i];
			m_pGridMC2->SetItemText(j, 2, tzLock[uvCmn_MC2_IsDevLocked(ENG_MMDI(u8Drv))]);
			swprintf_s(tzVal, 32, L"0x%04x", uvCmn_MC2_GetDriveError(ENG_MMDI(u8Drv)));
			m_pGridMC2->SetItemText(j, 3, tzVal);
		}
	}

	for (i=0x00; i<uvEng_GetConfig()->mc2_svc.drive_count; i++,j++)
	{
		/* Position */
		swprintf_s(tzVal, 32, L"%.4f", uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(pstMC2->axis_id[i])));
		m_pGridMC2->SetItemText(j, 1, tzVal);
		/* Servo Lock */
		m_pGridMC2->SetItemText(j, 2, tzLock[uvCmn_MC2_IsDevLocked(ENG_MMDI(pstMC2->axis_id[i]))]);
		/* Error */
		swprintf_s(tzVal, 32, L"0x%04x", uvCmn_MC2_GetDriveError(ENG_MMDI(pstMC2->axis_id[i])));
		m_pGridMC2->SetItemText(j, 3, tzVal);
	}

#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
	for (i=0x00; i<uvEng_GetConfig()->set_cams.acam_count; i++,j++)
	{
		/* Position */
		swprintf_s(tzVal, 32, L"%.4f", uvCmn_MCQ_GetACamCurrentPosZ(i+1));
		m_pGridMC2->SetItemText(j, 1, tzVal);
		/* IsBusy */
		m_pGridMC2->SetItemText(j, 2, tzReady[uvCmn_MCQ_IsACamZReady(i+1)]);
		/* IsAlarm */
		m_pGridMC2->SetItemText(j, 3, tzAlarm[uvCmn_MCQ_IsACamZAlarm(i+1)]);
	}
#endif
	/* Enable the Redraw */
	m_pGridMC2->SetRedraw(TRUE, TRUE);
	m_pGridMC2->Invalidate(FALSE);
}

/*
 desc : Output the step name
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateStepName()
{
	m_txt_ctl[0].SetRedraw(FALSE);

	if (!m_pWorkCent->IsBusy())
	{
		if (!m_txt_ctl[0].IsCompareText(L"Ready"))
		{
			m_txt_ctl[0].SetWindowTextW(L"Ready");
		}
	}
	else
	{
		/* Get the current step name */
		PTCHAR ptzStepName	= m_pWorkCent->GetStepName();
		if (!m_txt_ctl[0].IsCompareText(ptzStepName))
		{
			m_txt_ctl[0].SetWindowTextW(ptzStepName);
		}
	}
	m_txt_ctl[0].SetRedraw(TRUE);
	m_txt_ctl[0].Invalidate(FALSE);
}

/*
 desc : Updates the work step
 parm: None
 retn : None
*/
VOID CDlgMain::UpdateWorkStep()
{
	if (!m_pWorkCent->IsBusy())	return;
	/* Called periodically */
	m_pWorkCent->DoWork();
	/* Verify that the mark data has been measured */
	if (m_pWorkCent->IsUpdatedCalc())
	{
		UpdateGrabImage();
		UpdateGridData();
	}
	/* Verify that the measurement is completed */
	if (m_pWorkCent->IsCompleted())
	{
		UpdateGridTrendLines();
	}
}

/*
 desc : Update the grabbed image
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateGrabImage()
{
	HWND hWnd	= m_pic_ctl[0].GetSafeHwnd();
	HDC hDC		= NULL;
	CRect rGrab;

	/* Get the Device Handle */
	hDC = ::GetDC(hWnd);
	/* Get the output area */
	m_pic_ctl[0].GetClientRect(rGrab);
	/* Outputs the captured image to the specified area */
	uvEng_Camera_DrawCaliBitmap(hDC, rGrab);
	/* Release the device context */
	::ReleaseDC(hWnd, hDC);
}

/*
 desc : Update the grid data
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateGridData()
{
	TCHAR tzTime[32]= {NULL};
	INT32 i32Row	= m_pWorkCent->GetMeasCount();
	DOUBLE dbMotionX, dbMotionY;
	DOUBLE dbDiffX	= 0.0f, dbDiffY = 0.0f, dbSumX = 0.0f, dbSumY = 0.0f;
	DOUBLE dbSqrt	= 0.0f, dbCos = 0.0f, dbSin = 0.0f, dbAtan1 = 0.0f, dbAtan2 = 0.0f;
	DOUBLE dbLineX	= 0.0f, dbLineY = 0.0f, dbDist = m_stSearch.interval * (i32Row - 1);
	SYSTEMTIME tmCur= {NULL};

	if (uvEng_GetConfig()->IsRunDemo())
	{
		if (m_stSearch.target_x == 0x00)	dbMotionX	= m_edt_flt[4].GetTextToDouble();
		else								dbMotionX	= m_edt_flt[6].GetTextToDouble();
		dbMotionX	+= m_edt_flt[2].GetTextToDouble() * (i32Row - 1);
		dbMotionY	= m_edt_flt[5].GetTextToDouble();
	}
	else
	{
		if (m_stSearch.target_x == 0x0000)	dbMotionX	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);
		else
		{
			if (m_stSearch.acam_id == 1)	dbMotionX	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam1);
			else							dbMotionX	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam2);
		}
		dbMotionY	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);
	}
	/* Get the current time */
	GetLocalTime(&tmCur);
	swprintf_s(tzTime, 32, L"%02u:%02u:%02u.%03u",
			   tmCur.wHour, tmCur.wMinute, tmCur.wSecond, tmCur.wMilliseconds);

	if (uvEng_GetConfig()->IsRunDemo())
	{
#if 0
		dbDiffX	= uvCmn_GetRandNumerI32(0x01, 2) / 10000.0f /* mm -> 100 nm */;
		dbDiffY	= uvCmn_GetRandNumerI32(0x01, 2) / 10000.0f /* mm -> 100 nm */;
#else
		dbDiffX	= m_vMarkX[m_pWorkCent->GetMeasCount() - 1] / 1000.0f;
		dbDiffY	= m_vMarkY[m_pWorkCent->GetMeasCount() - 1] / 1000.0f;
#endif
	}
	else
	{
		dbDiffX	= m_pWorkCent->GetResult()->move_mm_x;
		dbDiffY	= m_pWorkCent->GetResult()->move_mm_y;
	}
#if 0
#if 0	
	if (i32Row > 1)
	{
		dbOffsetX = dbDiffX - m_pGridMeas->GetItemTextToFloat(i32Row-1, 3);
		dbOffsetY = dbDiffY - m_pGridMeas->GetItemTextToFloat(i32Row-1, 4);
	}
#else
	if (i32Row > 0)
	{
		dbSumX	= dbMotionX + dbDiffX;
		dbSumY	= dbMotionY + dbDiffY;

		/* Straightness X */
		dbLineX	= 0.0f;
		dbSqrt	= sqrt(pow(dbSumX - m_dbMatchXY[0][0], 2) + pow(dbSumY - m_dbMatchXY[0][1], 2));
		if ((dbSumX - m_dbMatchXY[0][0]) != 0.0f)
		{
			dbAtan1	= atan((dbSumY - m_dbMatchXY[0][1]) / (dbSumX - m_dbMatchXY[0][0]));
			dbAtan2	= atan((m_dbMatchXY[1][1] - m_dbMatchXY[0][1]) / (m_dbMatchXY[1][0] - m_dbMatchXY[0][0]));
			dbCos	= cos(dbAtan1-dbAtan2);
			dbLineX	= dbSqrt * dbCos - dbDist;
		}
		/* Straightness Y */
		dbLineY	= 0.0f;
		if ((dbSumX - m_dbMatchXY[0][0]) != 0.0f)
		{
			dbSqrt	= sqrt(pow(dbSumX - m_dbMatchXY[0][0], 2) + pow(dbSumY - m_dbMatchXY[0][1], 2));
			dbAtan1	= atan((dbSumY - m_dbMatchXY[0][1]) / (dbSumX - m_dbMatchXY[0][0]));
			dbAtan2	= atan((m_dbMatchXY[1][1] - m_dbMatchXY[0][1]) / (m_dbMatchXY[1][0] - m_dbMatchXY[0][0]));
			dbSin	= sin(dbAtan1-dbAtan2);
			dbLineY	= dbSqrt * dbSin;
		}
	}
#endif
#endif
	m_pGridMeas->SetItemText(i32Row, 0, tzTime);
	m_pGridMeas->SetItemDouble(i32Row, 1, dbMotionX, 4);
	m_pGridMeas->SetItemDouble(i32Row, 2, dbMotionY, 4);
	m_pGridMeas->SetItemDouble(i32Row, 3, dbDiffX, 4);
	m_pGridMeas->SetItemDouble(i32Row, 4, dbDiffY, 4);
#if 0
 	m_pGridMeas->SetItemDouble(i32Row, 5, dbLineX, 4);
 	m_pGridMeas->SetItemDouble(i32Row, 6, dbLineY, 4);
#endif
	m_pGridMeas->InvalidateRow(i32Row);
}

/*
 desc : Update the grid data for trend lines
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateGridTrendLines()
{
	INT32 i, i32Row	= m_pGridMeas->GetRowCount();
	DOUBLE dbDiffX, dbDiffY;
	vector <DOUBLE> vInDiffX, vInDiffY;
	vector <DOUBLE> vOutDiffX, vOutDiffY;

	if (i32Row < 2)	return;

	/* Get the DiffXY data */
	for (i=1; i<i32Row; i++)
	{
		dbDiffX	= m_pGridMeas->GetItemTextToFloat(i, 3) * 1000.0f;	/* mm -> um */
		dbDiffY	= m_pGridMeas->GetItemTextToFloat(i, 4) * 1000.0f;	/* mm -> um */
		vInDiffX.push_back(dbDiffX);
		vInDiffY.push_back(dbDiffY);
	}

	/* Calcualtes the trend lines */
	uvEng_Calc_GetStraightness(vInDiffX, vOutDiffX);
	uvEng_Calc_GetStraightness(vInDiffY, vOutDiffY);

	/* Updates the grid data */
	m_pGridMeas->SetRedraw(FALSE);
	for (i=1; i<i32Row; i++)
	{
 		m_pGridMeas->SetItemDouble(i, 5, vOutDiffX[i-1] / 1000.0f, 6);
 		m_pGridMeas->SetItemDouble(i, 6, vOutDiffY[i-1] / 1000.0f, 6);
	}
	m_pGridMeas->SetRedraw(TRUE, TRUE);
	m_pGridMeas->Invalidate(TRUE);

	/* Release the vector data */
	vInDiffX.clear();
	vInDiffY.clear();
	vOutDiffX.clear();
	vOutDiffY.clear();
}

/*
 desc : Updates the live image
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateLiveView()
{
	HDC hDC	= NULL;
	CRect rGrab;

	/* Get the output area */
	m_pic_ctl[0].GetClientRect(&rGrab);

	/* In live mode */
	if (1 == m_chk_ctl[0].GetCheck())
	{
		hDC	= ::GetDC(m_pic_ctl[0].m_hWnd);
		if (hDC)
		{
			uvEng_Camera_DrawLiveBitmap(hDC, rGrab, (UINT8)m_cmb_ctl[0].GetLBTextToNum());
			::ReleaseDC(m_pic_ctl[0].m_hWnd, hDC);
		}
		if (!m_bLiveMode)	m_bLiveMode	= TRUE;
	}
	/* If not in live mode */
	else if (m_bLiveMode)
	{
		m_bLiveMode	= FALSE;
		m_pic_ctl[0].InvalidateRect(rGrab);
	}
}

/*
 desc : Set the start xy position
 parm : None
 retn : None
*/
VOID CDlgMain::SetStartXY()
{
	ENG_MMDI enDrv	= ENG_MMDI::en_align_cam1;
	DOUBLE dbStageX	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);
	DOUBLE dbStageY	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);
	if (m_cmb_ctl[0].GetLBTextToNum() == 2)	enDrv = ENG_MMDI::en_align_cam2;
	DOUBLE dbACamX	= uvCmn_MC2_GetDrvAbsPos(enDrv);
	if (uvEng_GetConfig()->IsRunDemo())
	{
		m_edt_flt[4].SetTextToNum(10.0f, 4);
		m_edt_flt[5].SetTextToNum(50.0f, 4);
		m_edt_flt[6].SetTextToNum(100.0f, 4);
	}
	else
	{
		m_edt_flt[4].SetTextToNum(dbStageX, 4);
		m_edt_flt[5].SetTextToNum(dbStageY, 4);
		m_edt_flt[6].SetTextToNum(dbACamX, 4);
	}
}

/*
 desc : Set the live view
 parm : None
 retn : None
*/
VOID CDlgMain::SetLiveView()
{
	UINT8 u8CamID		= (UINT8)m_cmb_ctl[0].GetLBTextToNum();
	BOOL bSucc, bLive	= m_chk_ctl[0].GetCheck() == 1;	/* Verify that the live is active */
	UINT64 u64Tick		= GetTickCount64();
	ENG_VCCM enMode[2]	= { ENG_VCCM::en_none, ENG_VCCM::en_live_mode };

	/* Set the camera operation mode */
	uvEng_Camera_SetCamMode(enMode[m_chk_ctl[0].GetCheck()]);
	/* Enable or disable the operation of trigger board */
	uvEng_Trig_ReqTriggerStrobe(bLive);
	if (bLive)
	{
		bSucc	= uvEng_Trig_ReqEncoderLive(u8CamID);
	}
	else
	{
		/* Reset the output of trigger board */
		bSucc = uvEng_Trig_ReqEncoderOutReset();
		if (bSucc && !uvEng_GetConfig()->IsRunDemo())
		{
			do {

				/* Verify that a response has been received from trigger board */
				if (0 != uvEng_ShMem_GetTrig()->enc_out_val)	break;
				/* Verfy that thime is over */
				if (GetTickCount64() > (u64Tick + 2000))
				{
					bSucc	= FALSE;
					break;
				}

			} while (1);
		}
	}

	if (!bSucc)	AfxMessageBox(L"Failed to Enable or Disable the live mode ", MB_ICONSTOP|MB_TOPMOST);
}

/*
 desc : Move to the measurement position
		(Move to the starting or stop position)
 parm : None
 retn : None
*/
VOID CDlgMain::MoveToMeas()
{
	ENG_MMDI enDrv	= ENG_MMDI::en_stage_x;
	DOUBLE dbLastX	= m_edt_flt[7].GetTextToDouble();
	DOUBLE dbLastY	= m_edt_flt[8].GetTextToDouble();

	/* Whether it is a stage x-axis movement or a camera x-axis movement */
	if (m_chk_ctl[3].GetCheck() == 1)	/* In case of camera x-axis movement */
	{
		enDrv	= ENG_MMDI::en_align_cam1;
		if (m_cmb_ctl[0].GetLBTextToNum() == 2)	enDrv = ENG_MMDI::en_align_cam2;
	}

	/* Check whether the location of motion you wnat to move is valid */
	if (dbLastX < uvEng_GetConfig()->mc2_svc.min_dist[UINT8(enDrv)] ||
		dbLastX < uvEng_GetConfig()->mc2_svc.min_dist[UINT8(enDrv)] ||
		dbLastY > uvEng_GetConfig()->mc2_svc.max_dist[UINT8(ENG_MMDI::en_stage_y)] ||
		dbLastY > uvEng_GetConfig()->mc2_svc.max_dist[UINT8(ENG_MMDI::en_stage_y)])
	{
		AfxMessageBox(L"It is a location that can't be moved", MB_ICONINFORMATION);
		return;
	}

	/* 모션 (스테이지) 이동 */
	uvEng_MC2_SendDevAbsMoveExt(enDrv, dbLastX);
	uvEng_MC2_SendDevAbsMoveExt(ENG_MMDI::en_stage_y, dbLastY);
}

/*
 desc : Start the work step
 parm : None
 retn : None
*/
VOID CDlgMain::StartWork()
{
	/* Whether the axis to be measured is selected */
	if ((0 == m_chk_ctl[1].GetCheck()) && (0 == m_chk_ctl[2].GetCheck()))
	{
		AfxMessageBox(L"Measurement axis is not selected", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* Positions and error values of the first and last points */
	m_dbMatchXY[0][0] = m_edt_flt[4].GetTextToDouble() + m_edt_flt[9].GetTextToDouble();
	m_dbMatchXY[0][1] = m_edt_flt[5].GetTextToDouble() + m_edt_flt[10].GetTextToDouble();
	m_dbMatchXY[1][0] = m_edt_flt[7].GetTextToDouble() + m_edt_flt[11].GetTextToDouble();
	m_dbMatchXY[1][1] = m_edt_flt[8].GetTextToDouble() + m_edt_flt[12].GetTextToDouble();
	if (m_chk_ctl[3].GetCheck())
	{
		m_dbMatchXY[0][0] = m_edt_flt[6].GetTextToDouble() + m_edt_flt[9].GetTextToDouble();
	}
	/* Check that the set value is valid */
	if (!(m_dbMatchXY[0][0] > 0.0f && m_dbMatchXY[0][1] > 0.0f &&
		  m_dbMatchXY[1][0] > 0.0f && m_dbMatchXY[1][1] > 0.0f &&
		  (abs(m_dbMatchXY[0][0] - m_dbMatchXY[1][0]) > m_edt_flt[2].GetTextToDouble() ||
		   abs(m_dbMatchXY[0][1] - m_dbMatchXY[1][1]) > m_edt_flt[2].GetTextToDouble())))
	{
		AfxMessageBox(L"The start and stop setting value is not valid", MB_ICONSTOP);
		return;
	}

	/* Reset the work information */
	memset(&m_stSearch, 0x00, sizeof(STG_SCIM));
	/* Set the model for search */
	if (!SetRegistModel())	return;
	/* Set the search paramter */
	if (m_chk_ctl[1].GetCheck())	m_stSearch.direct |= 0x01;
	if (m_chk_ctl[2].GetCheck())	m_stSearch.direct |= 0x02;
	if (m_chk_ctl[3].GetCheck())	m_stSearch.target_x= 0x01;
	m_stSearch.acam_id	= (UINT16)m_cmb_ctl[0].GetLBTextToNum();
	m_stSearch.count	= (UINT16)m_edt_int[0].GetTextToNum();
	if (m_chk_ctl[3].GetCheck())	m_stSearch.start_x	= m_edt_flt[6].GetTextToDouble();
	else							m_stSearch.start_x	= m_edt_flt[4].GetTextToDouble();
	m_stSearch.start_y	= m_edt_flt[5].GetTextToDouble();
	m_stSearch.interval	= m_edt_flt[2].GetTextToDouble();
	/* Verify that the value is valid */
	if (!m_stSearch.IsValid())
	{
		AfxMessageBox(L"The setting value is not valid", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* Redraw the grid control */
	ResetGridCtrl();
	/* Enable the trigger for strobe */
	uvEng_Trig_ReqTriggerStrobe(TRUE);
	/* Set the camera operation mode */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);
	/* Start working */
	m_pWorkCent->InitWork();
}

/*
 desc : Stop the work step
 parm : None
 retn : None
*/
VOID CDlgMain::StopWork()
{
	if (m_pWorkCent->IsBusy())	m_pWorkCent->StopWork();
}

/*
 desc : Test the mark inspection
 parm : mode	- [in]  0x00-Start Position, 0x01-Stop Position
 retn : None
*/
VOID CDlgMain::SetBaseMatch(UINT8 mode)
{
	UINT8 u8ACamID		= (UINT8)m_cmb_ctl[0].GetLBTextToNum(), u8No = 0x09;
	BOOL bFinded		= FALSE;
	UINT64 u64TickLoop	= 0, u64StartTick;
	LPG_ACGR pstResult	= NULL;

	if (uvEng_GetConfig()->IsRunDemo())
	{
	}
	else
	{
		/* Set the model for search */
		if (!SetRegistModel())	return;
		/* Align Camera is Calibration Mode */
		uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);
		/* Reset the matching results */
		if (0x00 != mode)	u8No	= 0x0b;
		m_edt_flt[u8No].SetWindowTextW(L"+0.0000");
		m_edt_flt[u8No+1].SetWindowTextW(L"+0.0000");
		/* 기존 검색된 Grabbed Data & Image 제거 */
		uvEng_Camera_ResetGrabbedImage();
		/* Save the current working time */
		u64StartTick	= GetTickCount64();
		/* Forced triggering */
		if (!uvEng_Trig_ReqTrigOutOne(u8ACamID, TRUE))
		{
			uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
			AfxMessageBox(L"Failed to send the event for trigger", MB_ICONSTOP|MB_TOPMOST);
			return;
		}
		/* Save the working time */
		u64StartTick	= GetTickCount64();
		/* 1 ~ 2 초 정도 대기 */
		do {

			if (u64StartTick+2000 < GetTickCount64())	break;
			uvCmn_uSleep(100);

		} while (1);

		/* Get the search result */
		pstResult	= uvEng_Camera_RunModelCali(u8ACamID, 0xff);
		if (!pstResult || !pstResult->marked)
		{
			AfxMessageBox(L"Failed to analyze the grabbed image", MB_ICONSTOP|MB_TOPMOST);
		}
		/* Output the different value (error xy) */
		m_edt_flt[u8No].SetTextToNum(pstResult->move_mm_x, 4);
		m_edt_flt[u8No+1].SetTextToNum(pstResult->move_mm_y, 4);
		/* Update the grabbed image */
		UpdateGrabImage();
		/* Camera 동작 모드 설정 */
		uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
	}
}

/*
 desc : Register model for search
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::SetRegistModel()
{
	UINT8 u8ACamID	= (UINT8)m_cmb_ctl[0].GetLBTextToNum();
	UINT32 u32Model	= (UINT32)ENG_MMDT::en_circle;
	DOUBLE dbMSize	= 0.0f /* um */, dbMColor = 0.0f/* 256:Black, 128:White */;
	LPG_CIEA pstCfg	= uvEng_GetConfig();

	/* Regist a model for search target */
	dbMSize	= pstCfg->stage_st.model_meas_size * 1000.0f;
	dbMColor= pstCfg->stage_st.model_meas_color;

	if (!uvEng_Camera_SetModelDefineEx(u8ACamID,
									   pstCfg->mark_find.model_speed,
									   pstCfg->mark_find.detail_level,
									   pstCfg->mark_find.model_smooth,
									   &u32Model, &dbMColor, &dbMSize,
									   NULL, NULL, NULL, 1))
	{
		AfxMessageBox(L"Failed to setup the model for search target", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	return TRUE;
}

/*
 desc : Redraw (Rebuild) the grid control
 parm : None
 retn : None
*/
VOID CDlgMain::ResetGridCtrl()
{
	TCHAR tzDate[32]= {NULL};
	UINT32 i, j;
	GV_ITEM stGV	= {NULL};
	SYSTEMTIME tmCur= {NULL};

	/* Get the current date */
	GetLocalTime(&tmCur);
	swprintf_s(tzDate, 32, L"%04u-%02u-%02u", tmCur.wYear, tmCur.wMonth, tmCur.wDay);

	/* Basic properties of gird controls */
	stGV.mask	= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.lfFont	= g_lf;
	stGV.crFgClr= RGB(0, 0, 0);
	stGV.crBkClr= RGB(250, 250, 250);
	stGV.nFormat= DT_VCENTER | DT_SINGLELINE | DT_CENTER;

	/*  Remove all the existing grid data */
	m_pGridMeas->DeleteNonFixedRows();
	/* Disable updates of grid control */
	m_pGridMeas->SetRedraw(FALSE);
	/* Set the row of grid control */
	m_pGridMeas->SetRowCount(m_stSearch.count+1);
	/* Set the current date */
	m_pGridMeas->SetItemText(0, 0, tzDate);

	for (i=1; i<=m_stSearch.count; i++)
	{
		stGV.nFormat= DT_VCENTER | DT_SINGLELINE | DT_CENTER;
		stGV.strText.Format(L"00:00:00.000");
		stGV.row	= i;
		stGV.col	= 0;
		m_pGridMeas->SetItem(&stGV);
		m_pGridMeas->SetRowHeight(i, 24);

		stGV.nFormat= DT_VCENTER | DT_SINGLELINE | DT_RIGHT;
		for (j=1; j<7; j++)
		{
			stGV.col= j;
			stGV.strText.Format(L"");
			m_pGridMeas->SetItem(&stGV);
		}
	}

	/* Enable updates of grid control */
	m_pGridMeas->SetRedraw(TRUE, TRUE);
	m_pGridMeas->Invalidate(TRUE);
}

/*
 desc : 스테이지 위치를 작업자 위치로 이동
 parm : None
 retn : None
*/
VOID CDlgMain::UnloadPosition()
{
	DOUBLE dbMoveX	= uvEng_GetConfig()->set_align.table_unloader_xy[0][0];
	DOUBLE dbMoveY	= uvEng_GetConfig()->set_align.table_unloader_xy[0][1];

	/* 모션 (스테이지) 이동 */
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, dbMoveX,
							 uvEng_GetConfig()->mc2_svc.max_velo[UINT8(ENG_MMDI::en_stage_x)]);;
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dbMoveY,
							 uvEng_GetConfig()->mc2_svc.max_velo[UINT8(ENG_MMDI::en_stage_x)]);
}

/*
 desc : Calibration Material Thick 설정 만큼 카메라 Z 축 모두 이동 시킴
 parm : None
 retn : Non
*/
VOID CDlgMain::SetDofFilmThick()
{
	DOUBLE dbSetThick	= (DOUBLE)m_edt_flt[3].GetTextToDouble();	/* mm */
	DOUBLE dbBaseThick	= uvEng_GetConfig()->set_align.dof_film_thick;
	DOUBLE dbDiffThick	= dbSetThick - dbBaseThick;
	DOUBLE *pdbACamFocus= uvEng_GetConfig()->acam_focus.acam_z_focus;

	m_bDofThick	= FALSE;
	if (dbSetThick < 0.001f /* 1 um 이하이면 처리하지 못함 */)
	{
		AfxMessageBox(L"Check the thick value", MB_ICONSTOP);
		return;
	}

	/* 기준 두께 대비 차이만큼 Align Camera Z 축 이동 시킴 */
	if (!uvEng_MCQ_SetACamMovePosZ(0x01, 0x00, pdbACamFocus[0]+dbDiffThick))	return;
	if (!uvEng_MCQ_SetACamMovePosZ(0x02, 0x00, pdbACamFocus[1]+dbDiffThick))	return;

	/* The thickness of material has been set */
	m_bDofThick	= TRUE;
}

/*
 desc : Change the name of target x (stage x or align camera x)
 parm : None
 retn : None
*/
VOID CDlgMain::ChangeTarget()
{
	if (m_chk_ctl[3].GetCheck())	m_chk_ctl[3].SetWindowTextW(L"ACam X");
	else							m_chk_ctl[3].SetWindowTextW(L"Stage X");
}

/*
 desc : 
 parm : None
 retn : None
*/
VOID CDlgMain::SaveGridFile()
{
	TCHAR tzFile[_MAX_PATH]	= {NULL};
	TCHAR tzTarget[2][8]	= { L"stage_y", L"cam_x" };
	SYSTEMTIME stTm			= {NULL};

	GetLocalTime(&stTm);

	swprintf_s(tzFile, _MAX_PATH, L"%s\\stst\\stst_%04d%02d%02d_%02d%02d%02d_%s_acam%d.csv",
			   g_tzWorkDir, stTm.wYear, stTm.wMonth, stTm.wDay,
			   stTm.wHour, stTm.wMinute, stTm.wSecond,
			   tzTarget[m_chk_ctl[3].GetCheck()], (INT32)m_cmb_ctl[0].GetLBTextToNum());

	m_pGridMeas->Save(tzFile);
}