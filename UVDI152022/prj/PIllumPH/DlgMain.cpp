
/*
 desc : Main Dialog
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgMain.h"
#include "MainThread.h"

#include "./Menu/DlgArea.h"
#include "./Menu/DlgFocu.h"
#include "./Menu/DlgMeas.h"

#include "./Data/Recipe.h"


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

	m_bInited		= FALSE;
	m_u64TickInit	= 0;

	m_pMainThread	= NULL;
	m_pGridMC2		= NULL;
	m_pGridPH		= NULL;

	m_u8ChkID		= 0x00;	/* By default value, set the initial dialog to be selected */
	m_u8ErrIdx		= 0x00;	/* Index of error message (zero-based) */
	m_u64TickMsg	= GetTickCount64();

	memset(&m_stRecipe, 0x00, sizeof(STG_MICL));
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
	for (i=0; i<7; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Picture - Normal */
	u32StartID	= IDC_MAIN_PIC_MENU;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_pic_ctl[i]);
	/* Static - Normal */
	u32StartID	= IDC_MAIN_TXT_STEP_NAME;
	for (i=0; i<10; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Editbox - integer (Unsigned) */
	u32StartID	= IDC_MAIN_EDT_PH_AMP_IDX;
	for (i=0; i<4; i++)		DDX_Control(dx, u32StartID+i,	m_edt_int[i]);
	/* Editbox - float */
	u32StartID	= IDC_MAIN_EDT_DIST;
	for (i=0; i<7; i++)		DDX_Control(dx, u32StartID+i,	m_edt_flt[i]);
	/* Editbox - Text (String) */
	u32StartID	= IDC_MAIN_EDT_ERROR;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_edt_txt[i]);
	/* Button - Normal */
	u32StartID	= IDC_MAIN_BTN_EXIT;
	for (i=0; i<18; i++)	DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Checkbox - Normal */
	u32StartID	= IDC_MAIN_CHK_INIT;
	for (i=0; i<4; i++)		DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgMain, CMyDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MAIN_THREAD,	OnMsgThread)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_BTN_EXIT,	IDC_MAIN_BTN_PH_LED_OFF,	OnBtnClicked)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_CHK_INIT,	IDC_MAIN_CHK_RUN_MODE,		OnChkClicked)
	ON_NOTIFY_RANGE(GVN_COLUMNCLICK, IDC_GRID_MAIN_MC2, IDC_GRID_MAIN_PH,		OnGridColumnClick)
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
	InitGridPH();
	InitData();

	/* Creates the main-thread (After initialing the Engine library) */
	if (uvEng_GetConfig()->IsRunDemo())	u32Speed	= 10;
	m_pMainThread	= new CMainThread(m_hWnd);
	ASSERT(m_pMainThread);
	if (!m_pMainThread->CreateThread(0, NULL, u32Speed))
	{
		delete m_pMainThread;
		m_pMainThread	= NULL;
	}

	/* By default, select the initial dialog box */
	m_u8ChkID	= 0x00;
	InitChild();
	ShowChild(m_u8ChkID);

	return TRUE;
}

/*
 desc : When the window is stopping, it is called once at the last
 parm : None
 retn : None
*/
VOID CDlgMain::OnExitDlg()
{
	/* Power Off All LEDs */
	ResetPHLedPowerOff();
	uvCmn_uSleep(3000);

	/* Release the memory for main-thread */
	if (m_pMainThread)
	{
		m_pMainThread->Stop();
		while (m_pMainThread->IsBusy())	m_pMainThread->Sleep();
		delete m_pMainThread;
	}
	/* Remove all event messages */
	uvCmn_WaitPeekMessageQueue(500);
	/* Release the child window */
	KillChild();
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
	for (i=0; i<7; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* Normal - Static */
	for (i=0; i<10; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(TRUE);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* Edit box - Integer (Unsigned) */
	for (i=0; i<4; i++)
	{
		m_edt_int[i].SetEditFont(&g_lf);
		m_edt_int[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_int[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_int[i].SetInputType(ENM_DITM::en_uint16);
		m_edt_int[i].SetUseMinMax(TRUE);
		m_edt_int[i].SetInputSign(ENM_MNUS::en_unsign);
		switch (i)
		{
		case 0x00	:
		case 0x03	:
			switch (i)
			{
			case 0x00	:	m_edt_int[i].SetMinMaxNum(UINT16(0), UINT16(MAX_LED_POWER));	break;
			case 0x03	:	m_edt_int[i].SetMinMaxNum(UINT16(1), UINT16(5));				break;
			}
			m_edt_int[i].SetReadOnly(FALSE);
			m_edt_int[i].SetMouseClick(TRUE);
			break;

		case 0x01	:
		case 0x02	:
			switch (i)
			{
			case 0x01	:	m_edt_int[i].SetMinMaxNum(UINT16(1), UINT16(MAX_PH));	break;
			case 0x02	:	m_edt_int[i].SetMinMaxNum(UINT16(1), UINT16(4));		break;
			}
			m_edt_int[i].SetReadOnly(TRUE);
			m_edt_int[i].SetMouseClick(FALSE);
			break;
		}
	}
	/* Edit box - Float */
	for (i=0; i<7; i++)
	{
		m_edt_flt[i].SetEditFont(&g_lf);
		m_edt_flt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_flt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_flt[i].SetInputType(ENM_DITM::en_float);
		m_edt_flt[i].SetUseMinMax(TRUE);
		m_edt_flt[i].SetMinMaxNum(0.0f, 1000.0f);
		m_edt_flt[i].SetInputSign(ENM_MNUS::en_unsign);
		if (i < 2)	m_edt_flt[i].SetInputPoint(4);
		else		m_edt_flt[i].SetInputPoint(3);
		m_edt_flt[i].SetReadOnly(FALSE);
		m_edt_flt[i].SetMouseClick(TRUE);
	}
	/* Edit box - String */
	for (i=0; i<2; i++)
	{
		m_edt_txt[i].SetEditFont(&g_lf);
		m_edt_txt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_txt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_txt[i].SetInputType(ENM_DITM::en_string);
		if (i == 0)
		{
			m_edt_txt[i].SetInputMaxStr(1024);
			m_edt_txt[i].SetReadOnly(TRUE);
			m_edt_txt[i].SetMouseClick(FALSE);
		}
		else
		{
			m_edt_txt[i].SetInputMaxStr(32);
			m_edt_txt[i].SetReadOnly(FALSE);
			m_edt_txt[i].SetMouseClick(TRUE);
		}
	}
	/* Normal - Button */
	for (i=0; i<18; i++)
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
	m_chk_ctl[3].SetCheck(1);
}

/*
 desc : Create a grid control for MC2
 parm : None
 retn : None
*/
VOID CDlgMain::InitGridMC2()
{
	TCHAR tzCols[4][16]	= { L"Item", L"Pos (mm)", L"Servo", L"Error" }, tzRows[32] = {NULL};
	INT32 i32Width[4]	= { 50, 71, 58, 52 }, i, j=1;
	INT32 i32ColCnt		= 4, i32RowCnt = 0;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	LPG_CIEA pstCfg		= uvEng_GetConfig();
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* Last Recv. Packet / PDO Header / PDO Data (Values) */
	::GetWindowRect(m_grp_ctl[0].GetSafeHwnd(), rGrid);
	ScreenToClient(rGrid);
	rGrid.left	+= 13;
	rGrid.right	-= 13;
	rGrid.top	+= 55;
	rGrid.bottom-= 75;

	/* Get the total number of rows to be printed */
	if (0x01 == pstCfg->luria_svc.z_drive_type)
	{
		i32RowCnt	= pstCfg->luria_svc.ph_count + pstCfg->mc2_svc.drive_count;
	}
	else if (0x03 == pstCfg->luria_svc.z_drive_type)
	{
		i32RowCnt	= pstCfg->mc2_svc.drive_count;
	}

	/* Basic properties of gird controls */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* Create a object */
	m_pGridMC2	= new CGridCtrl;
	ASSERT(m_pGridMC2);
	m_pGridMC2->SetDrawScrollBarHorz(FALSE);
	m_pGridMC2->SetDrawScrollBarVert(FALSE);
	if (!m_pGridMC2->Create(rGrid, this, IDC_GRID_MAIN_MC2, dwStyle))
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

	m_pGridMC2->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1);
	m_pGridMC2->UpdateGrid();
}

/*
 desc : Create a grid control for Photohead
 parm : None
 retn : None
*/
VOID CDlgMain::InitGridPH()
{
	INT32 i32Width[5]	= { 45, 47, 47, 47, 47 }, i, j=1;
	INT32 i32ColCnt		= 5, i32RowCnt = uvEng_GetConfig()->luria_svc.ph_count + 1;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* Last Recv. Packet / PDO Header / PDO Data (Values) */
	::GetWindowRect(m_grp_ctl[1].GetSafeHwnd(), rGrid);
	ScreenToClient(rGrid);
	rGrid.left	+= 13;
	rGrid.right	-= 13;
	rGrid.top	+= 20;
	rGrid.bottom-= 122;

	/* Basic properties of gird controls */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* Create a object */
	m_pGridPH	= new CGridCtrl;
	ASSERT(m_pGridPH);
	m_pGridPH->SetDrawScrollBarHorz(FALSE);
	m_pGridPH->SetDrawScrollBarVert(FALSE);
	if (!m_pGridPH->Create(rGrid, this, IDC_GRID_MAIN_PH, dwStyle))
	{
		delete m_pGridPH;
		m_pGridPH = NULL;
		return;
	}

	/* Set default values */
	m_pGridPH->SetLogFont(g_lf);
	m_pGridPH->SetRowCount(i32RowCnt);
	m_pGridPH->SetColumnCount(i32ColCnt);
	m_pGridPH->SetFixedRowCount(1);
	m_pGridPH->SetFixedColumnCount(1);
	m_pGridPH->SetRowHeight(0, 28);
	m_pGridPH->SetFixedColumnCount(0);
	m_pGridPH->SetBkColor(RGB(255, 255, 255));
	m_pGridPH->SetFixedColumnSelection(0);

	/* Set the common control */
	stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	stGV.crFgClr = RGB(0, 0, 0);

	/* Common Properties */
	stGV.crBkClr= RGB(214, 214, 214);
	stGV.lfFont	= g_lf;
	stGV.nFormat= DT_VCENTER | DT_SINGLELINE | DT_CENTER;

	/* Header Control (First row, column) */
	stGV.row	= 0;
	stGV.col	= 0;
	stGV.strText= L"Item";
	m_pGridPH->SetItem(&stGV);
	m_pGridPH->SetColumnWidth(0, i32Width[0]);

	/* Header Control (First row) */
	for (i=1; i<i32ColCnt; i++)
	{
		stGV.row	= 0;
		stGV.col	= i;
		stGV.strText.Format(L"LED %u", i);
		m_pGridPH->SetItem(&stGV);
		m_pGridPH->SetColumnWidth(i, i32Width[i]);
	}
	/* Header Control (First Column) */
	for (i=1; i<i32RowCnt; i++)
	{
		stGV.row	= i;
		stGV.col	= 0;
		stGV.strText.Format(L"PH %u", i);
		m_pGridPH->SetItem(&stGV);
		m_pGridPH->SetRowHeight(i, 24);
	}

	/* body background color */
	stGV.crBkClr = RGB(255, 255, 255);

	/* body setting - for Photohead Led Power Index */
	for (i=1; i<i32RowCnt; i++)
	{
		for (j=1; j<i32ColCnt; j++)
		{
			stGV.row	= i;
			stGV.col	= j;
			stGV.strText= L"0";
			stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
			m_pGridPH->SetItem(&stGV);
		}
	}

	m_pGridPH->SetBaseGridProp(1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1);
	m_pGridPH->UpdateGrid();
}

/*
 desc : Release the grid control
 parm : None
 retn : None
*/
VOID CDlgMain::CloseGrid()
{
	/* for Motor Position and Error */
	if (m_pGridMC2)
	{
		if (m_pGridMC2->GetSafeHwnd())	m_pGridMC2->DestroyWindow();
		delete m_pGridMC2;
	}
	/* for Photohead Light Power Index */
	if (m_pGridPH)
	{
		if (m_pGridPH->GetSafeHwnd())	m_pGridPH->DestroyWindow();
		delete m_pGridPH;
	}
}

/*
 desc : Sets the default value for the controls
 parm : None
 retn : None
*/
VOID CDlgMain::InitData()
{
#ifdef _DEBUG
//	m_edt_txt[1].SetTextToStr(L"gen2i_1_220405_ph2led1");
	m_edt_txt[1].SetTextToStr(L"gen2i_1_220217_ph1led1");
#endif
	m_edt_int[0].SetTextToNum(UINT16(200));	/* LED Power */
	m_edt_int[1].SetTextToNum(UINT16(1));	/* PH No */
	m_edt_int[2].SetTextToNum(UINT16(1));	/* LED No */
	m_edt_int[3].SetTextToNum(UINT16(3));	/* DMD Image No */

	m_edt_flt[2].SetTextToNum(1.0f, 3);		/* Illum. Filter Rate */
	m_edt_flt[3].SetTextToNum(1.1f, 3);		/* Illum. Filter Rate */
	m_edt_flt[4].SetTextToNum(1.2f, 3);		/* Illum. Filter Rate */
	m_edt_flt[5].SetTextToNum(1.3f, 3);		/* Illum. Filter Rate */
	m_edt_flt[6].SetTextToNum(0.005f, 3);	/* Max-Min Condition */
}

/*
 desc : Create a child dialog (with modaless)
 parm : None
 retn : None
*/
VOID CDlgMain::InitChild()
{
	UINT8 i	= 0x00;
	CRect rChild;

	/* Move the child dialog to the output area */
	m_pic_ctl[0].GetWindowRect(rChild);
	ScreenToClient(rChild);
	/* Creates the modaless dialog */
	m_pDlgMenu[0] = new CDlgArea(IDD_AREA, &m_stRecipe, 0x00, this);
	m_pDlgMenu[1] = new CDlgFocu(IDD_FOCU, &m_stRecipe, 0x01, this);
	m_pDlgMenu[2] = new CDlgMeas(IDD_MEAS, &m_stRecipe, 0x02, this);
	ASSERT(m_pDlgMenu[0] && m_pDlgMenu[1]);
	for (; i<0x03; i++)
	{
		if (!m_pDlgMenu[i]->Create())	return;
		m_pDlgMenu[i]->MoveWindow(rChild.left, rChild.top, rChild.Width(), rChild.Height());
#if 0
		/* Show the child dialog */
		m_pDlgMenu[i]->ShowWindow(SW_SHOW);
#endif
	}
}

/*
 desc : Show the child dialog (with modaless)
 parm : index	- [in]  Dialog Index (0x00 ~ ...)
 retn : None
*/
VOID CDlgMain::ShowChild(UINT8 index)
{
	/* Changes the state of existing checkbox */
	m_chk_ctl[m_u8ChkID].SetCheck(0);

	if (m_pDlgLast)	m_pDlgLast->ShowWindow(SW_HIDE);
	m_pDlgLast = m_pDlgMenu[index];
	/* Update the controls in current child dialog */
	m_pDlgLast->SetData();
	m_pDlgLast->ShowWindow(SW_SHOW);

	/* Changes the state of selected checkbox */
	m_chk_ctl[index].SetCheck(1);
	/* Backup the clicked checkbox id */
	m_u8ChkID	= index;
}

/*
 desc : Release the child window
 parm : None
 retn : None
*/
VOID CDlgMain::KillChild()
{
	UINT8 i	= 0x00;
	for (; i<3; i++)
	{
		if (m_pDlgMenu[i]->GetSafeHwnd())	m_pDlgMenu[i]->DestroyWindow();
		delete m_pDlgMenu[i];
		m_pDlgMenu[i]	= NULL;
	}
}

/*
 desc : Initialize the Engine library
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::InitLib()
{
#if 0
	if (!uvEng_Init(ENG_ERVM::en_led_power))	return FALSE;
#else
	BOOL bInit	= FALSE;
	if (IDYES == AfxMessageBox(L"Do you want to reset the Engine?", MB_YESNO))	bInit	= TRUE;
	if (!uvEng_InitEx(ENG_ERVM::en_led_power, bInit))	return FALSE;
#endif

	return TRUE;
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
	UpdatePeriod();
	if (m_pDlgLast && m_pDlgLast->IsWorkBusy())
	{
		m_pDlgLast->DoWork();
		UpdateStepName();
	}
	return 0L;
}

/*
 desc : Update item values periodically
 parm : None
 retn : None
*/
VOID CDlgMain::UpdatePeriod()
{
	if (uvEng_GetConfig())
	{
		EnableCtrl();
		UpdateControls();
		UpdateLogMesg();
	}
}

/*
 desc : Enable or Disable for Controls
 parm : None
 retn : None
*/
VOID CDlgMain::EnableCtrl()
{
	UINT8 i = 0, j	= 0x00;
	BOOL bIsValid	= FALSE, bIsBusy = FALSE, bIsLedOn = FALSE, bIsName = FALSE;
	BOOL bMC2		= uvCmn_MC2_IsConnected();
	BOOL bLuria		= uvCmn_Luria_IsConnected();
	BOOL bLuiraInit	= uvEng_Luria_IsServiceInited();
	BOOL bEmulated	= uvEng_GetConfig()->luria_svc.IsRunEmulated();
	BOOL bRunDemo	= uvEng_GetConfig()->IsRunDemo();
	LPG_LDDP pstDP	= &uvEng_ShMem_GetLuria()->directph;		/* Get the sahred memory of luria's direct_photohead */

	if (m_pDlgLast)
	{
		bIsValid	= m_stRecipe.IsValid(m_pDlgLast->GetMenuID());
		if (m_pDlgLast)	bIsBusy	= m_pDlgLast->IsWorkBusy();
	}

	for (i=0x00; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		for (j=0x00; j<uvEng_GetConfig()->luria_svc.led_count; j++)
		{
			if (pstDP->IsPhLedLightOn(i+1, ENG_LLPI(j+1)))
			{
				bIsLedOn	= TRUE;
				break;
			}
		}
	}
	if (m_edt_txt[1].GetTextToLen() > 0)	bIsName = TRUE;

	/* Controls Enabled or Disabled */
	m_chk_ctl[0].EnableWindow(bRunDemo || bMC2 && bLuria && bLuiraInit && !bIsBusy);	/* Photohead Area Menu */
	m_chk_ctl[1].EnableWindow(bRunDemo || bMC2 && bLuria && bLuiraInit && !bIsBusy);	/* Photohead Focus Menu */
	m_chk_ctl[2].EnableWindow(bRunDemo || bMC2 && bLuria && bLuiraInit && !bIsBusy);	/* LED Power Measurement Menu */
	m_chk_ctl[3].EnableWindow(!bIsBusy);												/* Measurement Speed */

	m_btn_ctl[1].EnableWindow(bMC2 && bLuria && bLuiraInit && !bIsBusy);	/* Move UP or Left */
	m_btn_ctl[2].EnableWindow(bMC2 && bLuria && bLuiraInit && !bIsBusy);	/* Move Down or Right */
	m_btn_ctl[3].EnableWindow(FALSE/*bMC2 && bLuria && bLuiraInit*/);		/* Home All (!!! Important. It's dangerous !!!) */
	m_btn_ctl[4].EnableWindow(bMC2 && bLuria && bLuiraInit);				/* Reset All */

	m_btn_ctl[5].EnableWindow(bLuria && bLuiraInit && !bIsBusy);	/* Set the amplitude of the photohead */
	m_btn_ctl[6].EnableWindow(bLuria && bLuiraInit && !bIsBusy);	/* Reset the amplitude of all the photohead */
//	m_btn_ctl[7].EnableWindow();									/* Reset the error message */
	m_btn_ctl[8].EnableWindow(bLuria && bLuiraInit && !bIsBusy);	/* Initialize the hardware_init of luria */
	m_btn_ctl[9].EnableWindow(bLuria && bLuiraInit && !bIsBusy);	/* Select the number of image for the photohead */

 	m_btn_ctl[10].EnableWindow(!bIsBusy);				/* Load the recipe */
 	m_btn_ctl[11].EnableWindow(!bIsBusy);				/* Reload the recipe */
 	m_btn_ctl[12].EnableWindow(bIsName && !bIsBusy);	/* Save the recipe */
	m_btn_ctl[13].EnableWindow(bMC2 && bLuria && bLuiraInit && bIsValid && !bIsBusy);							/* Move the starting position */
	m_btn_ctl[14].EnableWindow(bRunDemo || (bMC2 && bLuria && bLuiraInit && bIsValid && !bIsBusy && !bIsLedOn));/* Start the job */
	m_btn_ctl[15].EnableWindow(bRunDemo || (bMC2 && bLuria && bLuiraInit && bIsValid && bIsBusy && !bIsLedOn));	/* Stop the job */
	m_btn_ctl[17].EnableWindow(bLuria && bLuiraInit);			/* Reset the amplitude of led in photohead */
}

/*
 desc : Update the log message
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateControls()
{
	/* Updates the current child dialog */
	if (m_pDlgLast)	m_pDlgLast->UpdateCtrl();
	/* Updates the motion position & errors */
	if (uvCmn_MC2_IsConnected())	UpdateMotion();
	/* Update the photohead */
	if (uvCmn_Luria_IsConnected())	UpdatePH();
	/* Update the air-pressure ... */
	UpdateAirPressure();
}

/*
 desc : OnNotifyGridColumnClicked Event
 parm : id		- [in] Grid Control ID
		nmhdr	- [in] the handle and ID of the control sending the message and the notification code
		result	- [in]  A pointer to the LRESULT variable to store the result code if the message has been handled.
 retn : None
*/
VOID CDlgMain::OnGridColumnClick(UINT32 id, NMHDR *nmhdr, LRESULT *result)
{
	INT32 i32Row, i32Col=0;
	NM_GRIDVIEW *pItem	= (NM_GRIDVIEW*)nmhdr;
	if (!pItem)	return;

	/* Return the index of the selected row and column */
	i32Row	= pItem->iRow;
	i32Col	= pItem->iColumn;
	if (i32Row < 0 || i32Col < 0)	return;

	if (IDC_GRID_MAIN_MC2 == id)
	{
		m_btn_ctl[1].EnableWindow(BOOL(i32Row > 0 && i32Row <= MAX_PH));
		m_btn_ctl[2].EnableWindow(BOOL(i32Row > 0 && i32Row <= MAX_PH));
	}
	else if (IDC_GRID_MAIN_PH == id)
	{
		m_edt_int[1].SetTextToNum(UINT16(i32Row));
		m_edt_int[2].SetTextToNum(UINT16(i32Col));
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
	case IDC_MAIN_BTN_EXIT				:	PostMessage(WM_CLOSE, 0, 0L);				break;
	case IDC_MAIN_BTN_ERROR				:	ResetErroMessage();							break;
	case IDC_MAIN_BTN_PH_AMP_SET		:	SetPHAmplitude();							break;
	case IDC_MAIN_BTN_PH_AMP_GET_ALL	:	GetPHAmplitudeAll();						break;
	case IDC_MAIN_BTN_PH_LED_OFF		:	ResetPHLedPowerOff();						break;
	/* Move the Motor (or Stage) */
	case IDC_MAIN_BTN_MOVE_UP			:	MoveStage(0x00);							break;
	case IDC_MAIN_BTN_MOVE_DOWN			:	MoveStage(0x01);							break;
	/* Inited the luria and focus */
	case IDC_MAIN_BTN_HWINIT			:	SetLuriaInit();								break;
	/* Inited the mode the for illuminace measurement */
	case IDC_MAIN_BTN_IMG_NO			:	SetLoadImageNo();							break;
	/* Set the homing or reset for motion drive */
	case IDC_MAIN_BTN_HOMED_ALL			:	;	break;	/* Caution : It's dangerous, so don't use it */
	case IDC_MAIN_BTN_RESET_ALL			:	uvEng_MC2_SendDevFaultResetAll();			break;

	/* Load / Reload / Save the recipe */
	case IDC_MAIN_BTN_RECIPE_LOAD		:	LoadRecipe();								break;
	case IDC_MAIN_BTN_RECIPE_RELOAD		:	ReloadRecipe();								break;
	case IDC_MAIN_BTN_RECIPE_SAVE		:	SaveRecipe();								break;
	/* Move to the starting position */
	case IDC_MAIN_BTN_MOVE				:	if (m_pDlgLast)	MoveStart();				break;

	/* Start or Stop the operation */
	case IDC_MAIN_BTN_START				:	StartWork();								break;
	case IDC_MAIN_BTN_STOP				:	if (m_pDlgLast)	m_pDlgLast->StopWork();		break;
	case IDC_MAIN_BTN_SAVE				:	if (m_pDlgLast)	m_pDlgLast->SaveGrid();		break;
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
 desc : Called when the checkbox is clicked
 parm : id	- [in]  checkbox id clicked
 retn : None
*/
VOID CDlgMain::OnChkClicked(UINT32 id)
{
	UINT8 u8Id		= UINT8(id-IDC_MAIN_CHK_INIT);
	BOOL bChecked	= FALSE;
#ifndef _DEBUG
	if (!m_bInited)
	{
		AfxMessageBox(L"HW initialize has not been executed", MB_ICONSTOP|MB_TOPMOST);
		m_chk_ctl[3].SetCheck(0);
		return;
	}
	else if ((m_u64TickInit + 10000) > GetTickCount64())
	{
		AfxMessageBox(L"Please wait a moment\nInitialization is being completed", MB_ICONSTOP|MB_TOPMOST);
		m_chk_ctl[3].SetCheck(0);
		return;
	}
#endif
	/* Release an existing child window */
	if (IDC_MAIN_CHK_RUN_MODE != id)	ShowChild(UINT8(id-IDC_MAIN_CHK_INIT));
	else if (IDC_MAIN_CHK_RUN_MODE == id)
	{
		if (m_chk_ctl[3].GetCheck() == 1)	m_chk_ctl[3].SetWindowTextW(L"High Speed\nIncompete Mode");
		else								m_chk_ctl[3].SetWindowTextW(L"Low Speed\nSafe Mode");
	}
}

/*
 desc : Move the motor (or stage)
 parm : direct	- [in]  The movement direct value (0x00: : up, 0x01: down)
 retn : None
*/
VOID CDlgMain::MoveStage(UINT8 direct)
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
	enDrv	= ENG_MMDI(pstMC2->axis_id[ptSel->x-1]);

	/* Get the motor number according to the direction of movement of the stage */
	switch (direct)
	{
	case 0x00 : dbSign = 1.0f;	break;	/* Up */
	case 0x01 : dbSign = -1.0f;	break;	/* Down */
	}

	/* Get the current position of the motor (MC2) + Move the distance value */
	dbMove	= uvCmn_MC2_GetDrvAbsPos(enDrv);
	dbMove	= uvCmn_MC2_GetDrvAbsPos(enDrv) + dbDist * dbSign;
	if (pstMC2->min_dist[UINT8(enDrv)] > dbMove)	dbMove = pstMC2->min_dist[UINT8(enDrv)];
	if (pstMC2->max_dist[UINT8(enDrv)] < dbMove)	dbMove = pstMC2->max_dist[UINT8(enDrv)];
	if (pstMC2->max_velo[UINT8(enDrv)] < dbVelo)	dbVelo = pstMC2->max_velo[UINT8(enDrv)];

	/* Check if the maximum shift value is out of range */
	if (!uvEng_MC2_SendDevAbsMove(enDrv, dbMove, dbVelo))	return;
}

/*
 desc : Inited the HW and Focus
 parm : None
 retn : None
*/
VOID CDlgMain::SetLuriaInit()
{
	if (!uvEng_Luria_IsServiceInited())
	{
		AfxMessageBox(L"The service is being initialized\nPlease wait a moment", MB_ICONINFORMATION);
		return;
	}
	if (!uvEng_Luria_ReqSetHWInit())
	{
		AfxMessageBox(L"Failed to initialize the HW Init of Service", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	if (!uvEng_Luria_ReqSetMotorPositionInitAll())
	{
		AfxMessageBox(L"Failed to initialize the Z-Axis Motor of Luria", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* !!! Important !!! */
	m_bInited		= TRUE;
	m_u64TickInit	= GetTickCount64();
}

/*
 desc : Load the inernal image
 parm : None
 retn : None
*/
VOID CDlgMain::SetLoadImageNo()
{
	UINT8 u8PhNo	= (UINT8)m_edt_int[1].GetTextToNum();
	UINT8 u8ImgNo	= (UINT8)m_edt_int[3].GetTextToNum();
#if 0
	if (!uvEng_Luria_ReqSetHWInit())
	{
		AfxMessageBox(L"Failed to initialize the HW Init of Service", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
#endif
	if (!uvEng_Luria_ReqSetLoadInternalTestImage(u8PhNo, u8ImgNo))
	{
		AfxMessageBox(L"Failed to LoadInteralTestImage", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : Set the amplitude of the photohead's LEDs
 parm : None
 retn : None
*/
VOID CDlgMain::SetPHAmplitude()
{
	UINT8 u8PhNo	= (UINT8)m_edt_int[1].GetTextToNum();
	UINT8 u8LedNo	= (UINT8)m_edt_int[2].GetTextToNum();
	UINT8 u8ImgNo	= (UINT8)m_edt_int[3].GetTextToNum();
	UINT16 u16Amp	= (UINT16)m_edt_int[0].GetTextToNum();
	UINT16 u16Freq	= uvEng_Luria_GetLedNoToFreq(u8LedNo);

	if (u8PhNo < 0x01 || u8LedNo < 0x01)
	{
		AfxMessageBox(L"The PH and LED number is not entered", MB_ICONINFORMATION);
		return;
	}
	/* Set the image number */
	if (!uvEng_Luria_ReqSetLoadInternalTestImage(u8PhNo, u8ImgNo))
	{
		AfxMessageBox(L"Failed to LoadInteralTestImage", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
#if 0
	/* Reset the all LED power */
	uvEng_Luria_ReqSetLightIntensityAll(0xff);
#endif
	/* Set the frequency of PM100D */
	uvEng_PM100D_SetQueryWaveLength(u16Freq);
	/* Set the LED Power */
	uvEng_Luria_ReqSetLightIntensity(u8PhNo, ENG_LLPI(u8LedNo), u16Amp);
	GetPHAmplitudeAll();
}

/*
 desc : Get the amplitude of all the photohead
 parm : None
 retn : None
*/
VOID CDlgMain::GetPHAmplitudeAll()
{
#if 0
	uvEng_Luria_ReqGetLedPowerAll();
#else
//	UINT8 u8PhNo	= (UINT8)m_edt_int[1].GetTextToNum();
	UINT8 u8LedNo	= (UINT8)m_edt_int[2].GetTextToNum();
	UINT8 i	= 0;
	for (; i<4; i++)	uvEng_Luria_ReqGetLedPowerLedAll(ENG_LLPI(i+1));
#endif
}

/*
 desc : Turn off the power of the photohead's LED
 parm : None
 retn : None
*/
VOID CDlgMain::ResetPHLedPowerOff()
{
	UINT8 i	= 0x00;
	/* You must use the HWInit command */
	uvEng_Luria_ReqSetHWInit();
	for (; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		uvEng_Luria_ReqSetLightIntensityAll(i+1);
	}
	GetPHAmplitudeAll();
}

/*
 desc : Load the recipe file
 parm : None
 retn : None
*/
VOID CDlgMain::LoadRecipe()
{
	TCHAR tzInit[_MAX_PATH]	= {NULL};
	STG_MICL stRecipe		= {NULL};
	PTCHAR ptzPath, ptzFile;
	CString strFile;
	CRecipe csRecipe;

	/* Set the path where the recipe file is stored */
	swprintf_s(tzInit, _MAX_PATH, L"%s\\data\\recipe", g_tzWorkDir);
	/* Open the comoon file dialog for file selection */
	ptzPath	= (PTCHAR)::Alloc(sizeof(TCHAR)*MAX_PATH_LEN);
	wmemset(ptzPath, 0x00, MAX_PATH_LEN);
	if (uvCmn_GetSelectFile(L"Illum File (*.im) | *.im", MAX_PATH_LEN,
							 ptzPath, L"Please select a recipe file", tzInit))
	{
		if (wcslen(ptzPath) > 0)
		{
			ptzFile	= uvCmn_GetFileName(ptzPath, (UINT32)wcslen(ptzPath));
			if (ptzFile)
			{
				/* Load the recipe data */
				strFile.Format(L"%s", ptzFile);
				if (csRecipe.LoadRecipe(ptzPath, &stRecipe))
				{
					/* Update the recipe file in the edit_box */
					m_edt_txt[1].SetTextToStr(strFile.Left(strFile.GetLength()-3).GetBuffer());
					/* Filter Magnification for illuminance measurement (Normal : 10x -> 1/10) */
					m_edt_flt[2].SetTextToNum(stRecipe.sensor_rate[0], 3);
					m_edt_flt[3].SetTextToNum(stRecipe.sensor_rate[1], 3);
					m_edt_flt[4].SetTextToNum(stRecipe.sensor_rate[2], 3);
					m_edt_flt[5].SetTextToNum(stRecipe.sensor_rate[3], 3);
					/* Set the results */
					memcpy(&m_stRecipe, &stRecipe, sizeof(STG_MICL));
					/* Update the controls in current child dialog */
					m_pDlgMenu[0]->SetData();
					m_pDlgMenu[1]->SetData();
					m_pDlgMenu[2]->SetData();
				}
			}
		}
	}
	::Free(ptzPath);
}

/*
 desc : Reload the recipe file
 parm : None
 retn : None
*/
VOID CDlgMain::ReloadRecipe()
{
	TCHAR tzRecipe[32]	= {NULL};
	STG_MICL stRecipe	= {NULL};
	CRecipe csRecipe;

	/* Update the recipe file in the edit_box */
	m_edt_txt[1].GetWindowText(tzRecipe, 32);
	if (wcslen(tzRecipe) < 1)
	{
		AfxMessageBox(L"The recipe name is not entered", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* Load the recipe data */
	if (!csRecipe.LoadRecipe(tzRecipe, &stRecipe))
	{
		AfxMessageBox(L"Failed to load the recipe file", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* Filter Magnification for illuminance measurement (Normal : 10x -> 1/10) */
	m_edt_flt[2].SetTextToNum(stRecipe.sensor_rate[0], 3);
	m_edt_flt[3].SetTextToNum(stRecipe.sensor_rate[1], 3);
	m_edt_flt[4].SetTextToNum(stRecipe.sensor_rate[2], 3);
	m_edt_flt[5].SetTextToNum(stRecipe.sensor_rate[3], 3);
	/* Set the results */
	memcpy(&m_stRecipe, &stRecipe, sizeof(STG_MICL));
	/* Update the controls in current child dialog */
	m_pDlgMenu[0]->SetData();
	m_pDlgMenu[1]->SetData();
	m_pDlgMenu[2]->SetData();
}

/*
 desc : Save the recipe file
 parm : None
 retn : None
*/
VOID CDlgMain::SaveRecipe()
{
	TCHAR tzRecipe[32]	= {NULL};
	CRecipe csRecipe;

	/* Get the recipe data */
	if (!m_pDlgMenu[0]->GetData())	return;
	if (!m_pDlgMenu[1]->GetData())	return;
	if (!m_pDlgMenu[2]->GetData())	return;

	/* Update the recipe file in the edit_box */
	m_edt_txt[1].GetWindowText(tzRecipe, 32);
	if (wcslen(tzRecipe) < 1)
	{
		AfxMessageBox(L"The recipe name is not entered", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* Filter Magnification for illuminance measurement (Normal : 10x -> 1/10) */
	m_stRecipe.sensor_rate[0]	= (DOUBLE)m_edt_flt[2].GetTextToDouble();
	m_stRecipe.sensor_rate[1]	= (DOUBLE)m_edt_flt[3].GetTextToDouble();
	m_stRecipe.sensor_rate[2]	= (DOUBLE)m_edt_flt[4].GetTextToDouble();
	m_stRecipe.sensor_rate[3]	= (DOUBLE)m_edt_flt[5].GetTextToDouble();
	/* Save the recipe */
	csRecipe.SaveRecipe(tzRecipe, &m_stRecipe);
}

/*
 desc : Move to the starting position
 parm : None
 retn : None
*/
VOID CDlgMain::MoveStart()
{
	LPG_CMSI pstMC2	= &uvEng_GetConfig()->mc2_svc;
	BOOL bRunDemo	= uvEng_GetConfig()->IsRunDemo();

	/* Check whether the motion is currently moving */
	if (!bRunDemo && uvCmn_MC2_IsAnyDriveBusy())
	{
		AfxMessageBox(L"The motion is currently moving", MB_ICONINFORMATION);
		return;
	}
	/* Check whether the structure information for illuminance measurement is valid */
	if (!bRunDemo && !m_stRecipe.IsValidArea())
	{
		AfxMessageBox(L"The measurement info. of illuminace is not valid", MB_ICONINFORMATION);
		return;
	}

	/* Check whether the location of motion you wnat to move is valid */
	if (!(m_stRecipe.cent_pos_x != 0.0f && m_stRecipe.cent_pos_y != 0.0f &&
		  m_stRecipe.cent_pos_x < pstMC2->max_dist[UINT8(ENG_MMDI::en_stage_x)] &&
		  m_stRecipe.cent_pos_y < pstMC2->max_dist[UINT8(ENG_MMDI::en_stage_y)]) && !bRunDemo)
	{
		AfxMessageBox(L"It is a location that can't be moved", MB_ICONINFORMATION);
		return;
	}
	if (!bRunDemo)
	{
		/* Move to the starting position (Axis-X/Y) */
		uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, m_stRecipe.cent_pos_x, pstMC2->max_velo[UINT8(ENG_MMDI::en_stage_x)]);
		uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, m_stRecipe.cent_pos_y, pstMC2->max_velo[UINT8(ENG_MMDI::en_stage_y)]);
		/* Move to the starting position (Axis-Z) */
#if 0
		uvEng_Luria_ReqSetMotorAbsPosition(m_stRecipe.ph_no, m_stRecipe.focus_pos_z);
#else
		uvEng_Luria_ReqSetMotorAbsPosition(m_pDlgLast->GetPhNo(), m_stRecipe.focus_pos_z);
#endif
	}
}

/*
 desc : Update the grid control
 parm : None
 retn : None
*/
VOID CDlgMain::UpdatePH()
{
	UINT8 i = 0x00,j= 0x00;
	UINT8 u8PhCnt	= uvEng_GetConfig()->luria_svc.ph_count;
	UINT8 u8LedCnt	= uvEng_GetConfig()->luria_svc.led_count;
	LPG_LDDP pstDP	= &uvEng_ShMem_GetLuria()->directph;
	LPG_LDEW pstEP	= &uvEng_ShMem_GetLuria()->exposure;

	m_pGridPH->SetRedraw(FALSE);

	for (; i<u8PhCnt; i++)
	{
		for (j=0x00; j<u8LedCnt; j++)
		{
#if 1
			m_pGridPH->SetItemText(i+1, j+1, (UINT32)pstDP->light_source_driver_amplitude[i][j]);
#else
			m_pGridPH->SetItemText(i+1, j+1, (UINT32)pstEP->led_amplitude_power[i][j]);
#endif
		}
	}

	m_pGridPH->SetRedraw(TRUE);
	m_pGridPH->Invalidate(FALSE);
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
#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
	u8Alarm		= uvCmn_MCQ_GetBitsValueEx(ENG_PIOA::en_z1_z3_air_pressure_low_alarm);
	u16AirPress	= uvCmn_MCQ_GetWordValueEx(ENG_PIOA::en_z1_z3_axis_cylinder_air_pressure);
#endif
	COLORREF clrTxt[2]	= { RGB(255, 0, 0), RGB(0, 0, 0) };
	swprintf_s(tzAir, 32, L"%u kPa", u16AirPress);
	m_txt_ctl[1].SetRedraw(FALSE);
	m_txt_ctl[1].SetWindowText(tzAir);
	m_txt_ctl[1].SetTextColor(clrTxt[u8Alarm]);
	m_txt_ctl[1].SetRedraw(TRUE);
	m_txt_ctl[1].Invalidate(FALSE);
}

/*
 desc : Update the log message
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateLogMesg()
{
	UINT64 u64Tick	= GetTickCount64();
	/* Update the error message */
	if (u64Tick > m_u64TickMsg + 5000)
	{
		m_u64TickMsg	= u64Tick;

		PTCHAR ptzMsg = uvCmn_Logs_GetErrorMesg(m_u8ErrIdx);
		if (ptzMsg)
		{
			m_edt_txt[0].SetWindowTextW(ptzMsg);
			m_u8ErrIdx++;
		}
		else	m_u8ErrIdx	= 0x00;
	}
}

/*
 desc : Update the motion positions and erros
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateMotion()
{
	UINT8 i, u8DrvCnt	= uvEng_GetConfig()->mc2_svc.drive_count;
	TCHAR tzVal[32]		= {NULL}, tzLock[2][8] = {L"Unlock", L"Locked" };
	LPG_CMSI pstMC2		= &uvEng_GetConfig()->mc2_svc;

	/* Disable the Redraw */
	m_pGridMC2->SetRedraw(FALSE);

	for (i=0x00; i<u8DrvCnt; i++)
	{
		/* Position */
		swprintf_s(tzVal, 32, L"%.4f", uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(pstMC2->axis_id[i])));
		m_pGridMC2->SetItemText(i+1, 1, tzVal);
		/* Servo Lock */
		m_pGridMC2->SetItemText(i+1, 2, tzLock[uvCmn_MC2_IsDevLocked(ENG_MMDI(pstMC2->axis_id[i]))]);
		/* Error */
		swprintf_s(tzVal, 32, L"0x%04x", uvCmn_MC2_GetDriveError(ENG_MMDI(pstMC2->axis_id[i])));
		m_pGridMC2->SetItemText(i+1, 3, tzVal);
	}

	/* Enable the Redraw */
	m_pGridMC2->SetRedraw(TRUE);
	m_pGridMC2->Invalidate(FALSE);
}

/*
 desc : Output the step name
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateStepName()
{
	/* Get the current step name */
	PTCHAR ptzStepName	= m_pDlgLast->GetStepName();
	if (!m_txt_ctl[0].IsCompareText(ptzStepName))
	{
		m_txt_ctl[0].SetRedraw(FALSE);
		m_txt_ctl[0].SetWindowTextW(ptzStepName);
		m_txt_ctl[0].SetRedraw(TRUE);
		m_txt_ctl[0].Invalidate(FALSE);
	}
}

/*
 desc : Start working
 parm : None
 retn : None
*/
VOID CDlgMain::StartWork()
{
	if (m_pDlgLast)
	{
		UINT8 u8RunMode = m_chk_ctl[3].GetCheck() == 1 ? 0x01 : 0x00;
		m_pDlgLast->StartWork(u8RunMode, m_edt_flt[6].GetTextToDouble());
	}
}