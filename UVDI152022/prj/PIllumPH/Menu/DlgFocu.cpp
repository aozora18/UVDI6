
/*
 desc : Initial Position Search
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgFocu.h"

#include "../Work/WorkFocus.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : constructor
 parm : id		- [in]  dialog id
		recipe	- [in]  Recipe Information
		parent	- [in]  parent object pointer
 retn : None
*/
CDlgFocu::CDlgFocu(UINT32 id,LPG_MICL recipe, UINT8 menu_id, CWnd* parent)
	: CDlgMenu(id, recipe, menu_id, parent)
{
	m_pWorkFocus= NULL;
	m_pGridFocus= NULL;
}

/*
 desc : destructor
 parm : None
 retn : None
*/
CDlgFocu::~CDlgFocu()
{
}

/*
 desc : window id mapping
 parm : dx	- object for data exchange and validation
 retn : None
*/
VOID CDlgFocu::DoDataExchange(CDataExchange* dx)
{
	CDlgMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Groupbox - Normal */
	u32StartID	= IDC_FOCU_GRP_MEAS;
	for (i=0; i<6; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Combobox - Normal */
	u32StartID	= IDC_FOCU_CMB_PARM_PH_NO;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_cmb_ctl[i]);
	/* Static - Normal */
	u32StartID	= IDC_FOCU_TXT_COUNT;
	for (i=0; i<10; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Editbox - Integer */
	u32StartID	= IDC_FOCU_EDT_LED_IDX;
	for (i=0; i<6; i++)		DDX_Control(dx, u32StartID+i,	m_edt_int[i]);
	/* Editbox - Float */
	u32StartID	= IDC_FOCU_EDT_FOCUS_Z;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_edt_flt[i]);
}

BEGIN_MESSAGE_MAP(CDlgFocu, CDlgMenu)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID_FOCU_RET, OnGridSelChanged)
END_MESSAGE_MAP()

/*
 desc : PreTranslateMessage
 parm : msg	- Message Structure Pointer
 retn : TRUE or FALSE
*/
BOOL CDlgFocu::PreTranslateMessage(MSG* msg)
{

	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : When the windows is running, it is initially called once
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgFocu::OnInitDlg()
{
	if (!CDlgMenu::OnInitDlg())	return FALSE;

	/* Initialize controls for windows */
	InitCtrl();
	/* Set the initial value in the control */
	InitData();
	/* Initialize control for grid */
	InitGrid();

	/* return a fixed value (TRUE) */
	return TRUE;
}

/*
 desc : Called once during the last shutdown
 parm : None
 retn : None
*/
VOID CDlgFocu::OnExitDlg()
{
	CDlgMenu::OnExitDlg();

	/* Delete the grid control */
	if (m_pGridFocus)
	{
		if (m_pGridFocus->GetSafeHwnd())	m_pGridFocus->DestroyWindow();
		delete m_pGridFocus;
	}
}

/*
 desc : Called whenever the window is updated
 parm : dc	- device context
 retn : None
*/
VOID CDlgFocu::OnPaintDlg(CDC *dc)
{
}

/*
 desc : Initialize the controls
 parm : None
 retn : None
*/
VOID CDlgFocu::InitCtrl()
{
	INT32 i;

	/* Groupbox - Normal */
	for (i=0; i<6; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* Static - Normal */
	for (i=0; i<10; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(TRUE);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(255, 255, 255), 0, RGB(0, 0, 0));
	}
	/* Edit box - Integer (Unsigned) */
	for (i=0; i<6; i++)
	{
		m_edt_int[i].SetEditFont(&g_lf);
		m_edt_int[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_int[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_int[i].SetInputType(ENM_DITM::en_uint16);
		m_edt_int[i].SetUseMinMax(TRUE);
		m_edt_int[i].SetInputSign(ENM_MNUS::en_unsign);
		m_edt_int[i].SetReadOnly(FALSE);
		m_edt_int[i].SetMouseClick(TRUE);
		if (i < 5)		m_edt_int[i].SetMinMaxNum(UINT16(1), UINT16(100));
		else if (i < 6)	m_edt_int[i].SetMinMaxNum(UINT16(0), UINT16(MAX_LED_POWER));
	}
	/* Edit box - Float */
	for (i=0; i<2; i++)
	{
		m_edt_flt[i].SetEditFont(&g_lf);
		m_edt_flt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_flt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_flt[i].SetInputType(ENM_DITM::en_double);
		m_edt_flt[i].SetUseMinMax(TRUE);
		m_edt_flt[i].SetInputSign(ENM_MNUS::en_sign);
		m_edt_flt[i].SetInputPoint(4);
		if (0x00 == i)	m_edt_flt[i].SetMinMaxNum(-0.500f, 24.000f);
		else			m_edt_flt[i].SetMinMaxNum(0.001f, 5.000f);
		m_edt_flt[i].SetReadOnly(FALSE);
		m_edt_flt[i].SetMouseClick(TRUE);
	}
	/* Combobox - Normal */
	for (i=0; i<1; i++)	m_cmb_ctl[i].SetLogFont(&g_lf);
	for (i=0; i<uvEng_GetConfig()->luria_svc.ph_count; i++)		m_cmb_ctl[0].AddNumber(UINT64(i+1));
	for (i=0; i<uvEng_GetConfig()->luria_svc.led_count; i++)	m_cmb_ctl[1].AddNumber(UINT64(i+1));
	if (m_cmb_ctl[0].GetCount() > 0)	m_cmb_ctl[0].SetCurSel(m_pstRecipe->ph_no-1);
	if (m_cmb_ctl[1].GetCount() > 0)	m_cmb_ctl[1].SetCurSel(m_pstRecipe->led_no-1);
	m_cmb_ctl[0].EnableWindow(FALSE);
	m_cmb_ctl[1].EnableWindow(FALSE);
	if (m_cmb_ctl[0].GetCount() > 0)	m_cmb_ctl[0].SetCurSel(0);
	if (m_cmb_ctl[1].GetCount() > 0)	m_cmb_ctl[1].SetCurSel(0);
}

/*
 desc : Set the initial value in the controls
 parm : None
 retn : None
*/
VOID CDlgFocu::InitData()
{
	UINT8 i	= 0x00;

	m_edt_int[0].SetTextToNum(UINT16(500));	/* LED Power Index (0 ~ 4095) */
	m_edt_int[1].SetTextToNum(UINT8(12));	/* This is a count to ignore when measuring LEDs */
	m_edt_int[2].SetTextToNum(UINT8(32));	/* Effective count when measuring LEDs */
	m_edt_int[3].SetTextToNum(UINT8(4));	/* The minimum number of discarded values among measured values */
	m_edt_int[4].SetTextToNum(UINT8(4));	/* The maximum number of discarded values among measured values */
	m_edt_int[5].SetTextToNum(UINT8(5));	/* Axis-Z (UpDown) Movement Count */

	m_edt_flt[0].SetTextToNum(0.0f, 4);		/* Axis-Z Focus Position */
	m_edt_flt[1].SetTextToNum(0.0f, 4);		/* Axis-Z Focus Move Interval */
}

/*
 desc : Create a grid control for Measurement Information
 parm : None
 retn : None
*/
VOID CDlgFocu::InitGrid()
{
	TCHAR tzCols[6][16]	= { L"PH Z (mm)", L"Count", L"Last (mW)", L"Max-Min (mW)", L"Avg (mW)", L"Std (mW)" };
	INT32 i32Width[6]	= { 80, 90, 100, 100, 100, 100 }, i, j=1;
	INT32 i32ColCnt		= 6, i32RowCnt = 0;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	LPG_CIEA pstCfg		= uvEng_GetConfig();
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* Last Recv. Packet / PDO Header / PDO Data (Values) */
	::GetWindowRect(m_grp_ctl[0].GetSafeHwnd(), rGrid);
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
	m_pGridFocus	= new CGridCtrl;
	ASSERT(m_pGridFocus);
	m_pGridFocus->SetDrawScrollBarHorz(FALSE);
	m_pGridFocus->SetDrawScrollBarVert(TRUE);
	if (!m_pGridFocus->Create(rGrid, this, IDC_GRID_FOCU_RET, dwStyle))
	{
		delete m_pGridFocus;
		m_pGridFocus = NULL;
		return;
	}

	/* Set default values */
	m_pGridFocus->SetLogFont(g_lf);
	m_pGridFocus->SetRowCount(i32RowCnt+1);
	m_pGridFocus->SetColumnCount(i32ColCnt);
	m_pGridFocus->SetFixedRowCount(1);
	m_pGridFocus->SetRowHeight(0, 28);
	m_pGridFocus->SetFixedColumnCount(0);
	m_pGridFocus->SetBkColor(RGB(255, 255, 255));
	m_pGridFocus->SetFixedColumnSelection(0);

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
		m_pGridFocus->SetItem(&stGV);
		m_pGridFocus->SetColumnWidth(i, i32Width[i]);
	}

	m_pGridFocus->SetBaseGridProp(1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
	m_pGridFocus->UpdateGrid();
}

/*
 desc : Updates the controls
 parm : None
 retn : None
*/
VOID CDlgFocu::UpdateCtrl()
{
	/* Enable or Disable for Controls */
	EnableCtrl();
}

/*
 desc : Enable or Disable for Controls
 parm : None
 retn : None
*/
VOID CDlgFocu::EnableCtrl()
{
	UINT8 i;
	BOOL bWorkIn	= !m_pWorkFocus ? FALSE : !m_pWorkFocus->IsEnded();
	BOOL bPM100D	= uvEng_PM100D_IsConnected();
	BOOL bInited	= uvEng_Luria_IsServiceInited();
	BOOL bLoaded	= m_pstRecipe->IsValidFocus();
	BOOL bRunDemo	= uvEng_GetConfig()->IsRunDemo();
	BOOL bEnabled	= bRunDemo || bPM100D && !bWorkIn;

	for (i=0x00; i<6; i++)	m_edt_int[i].EnableWindow(bRunDemo || bEnabled);
	for (i=0x00; i<2; i++)	m_edt_flt[i].EnableWindow(bRunDemo || bEnabled);
}

/*
 desc : Create the work job
 parm : None
 retn : None
*/
VOID CDlgFocu::CreateWork()
{
	ReleaseWork();

	/* Initalize the maximum value in each grid control area */
	memset(m_stMaxPwr, 0x00, sizeof(STM_GMMV) * 3);
	/* Creates a working secinario */
	m_pWorkFocus	= new CWorkFocus(m_pstRecipe, m_u8RunMode, m_dbMaxMin);
	ASSERT(m_pWorkFocus);
	m_pWorkFocus->InitWork();
}

/*
 desc : Release the work job
 parm : None
 retn : None
*/
VOID CDlgFocu::ReleaseWork()
{
	if (m_pWorkFocus)
	{
		m_pWorkFocus->StopWork();
		delete m_pWorkFocus;
		m_pWorkFocus	= NULL;
	}
}

/*
 desc : Initializes for LED Focus measurement
 parm : run_mode- [in]  0x00 : Low Speed (Safe Mode), 0x01 : High Speed (Incomplete Mode)
		max_min	- [in]  Max-Min Condition
						If the difference between the maximum and minimum values of the data stored
						in the queue buffer is less than this value, collection is possible
 retn : TRUE or FALSE
*/
BOOL CDlgFocu::StartWork(UINT8 run_mode, DOUBLE max_min)
{
	if (!CDlgMenu::StartWork(run_mode, max_min))	return FALSE;

	/* Check whether the set values are valid */
	if (!m_pstRecipe->IsValidFocus())
	{
		AfxMessageBox(L"The set value (focus) is not valid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* Check if the focus position is out of focus */
	if (!IsValidFocusPH())	return FALSE;
	/* Rebuild the grid control */
	RebuildGrid();
	/* Creates a object for area measurement */
	CreateWork();

	return TRUE;
}

/*
 desc : Stop the search work (operation)
 parm : None
 retn : None
*/
VOID CDlgFocu::StopWork()
{
	if (m_pWorkFocus)	ReleaseWork();
}

/*
 desc : Whether it's currently being executed.
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgFocu::IsWorkBusy()
{
	if (!m_pWorkFocus)	return FALSE;
	return m_pWorkFocus->IsBusy();
}

/*
 desc : It is called periodically
 parm : None
 retn : None
*/
VOID CDlgFocu::DoWork()
{
	/* If the work is idle */
	if (!m_pWorkFocus)	return;
	/* Check if everything is done... */
	if (!m_pWorkFocus->IsEnded())
	{
		m_pWorkFocus->DoWork();
		if (m_pWorkFocus->IsUpdatePower())	UpdateWork(0x00);
		if (m_pWorkFocus->IsUpdatedCalc())	UpdateWork(0x01);
	}
}

/*
 desc : Rebuild the grid control
 parm : None
 retn : None
*/
VOID CDlgFocu::RebuildGrid()
{
	INT16 i, i16Rows= (INT16)m_pstRecipe->GetCountUpDown();
	DOUBLE dbPosZ	= m_pstRecipe->GetPosZ(0x00);
	GV_ITEM stGV	= {NULL};

	/* Basic properties of grid controls */
	stGV.mask	= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.nFormat= DT_VCENTER | DT_RIGHT | DT_SINGLELINE;
	stGV.crFgClr= RGB(32, 32, 32);
	stGV.crBkClr= RGB(250, 250, 250);
	stGV.lfFont	= g_lf;
	stGV.col	= 0;

	/* Reset the grid control */
	m_pGridFocus->SetRedraw(FALSE);
	m_pGridFocus->DeleteNonFixedRows();
	m_pGridFocus->SetRedraw(TRUE);

	/* Set the rows and columns of grid control */
	m_pGridFocus->SetRedraw(FALSE);
	m_pGridFocus->SetRowCount(i16Rows+1);

	/* Measurement Position */
	for (i=0; i<i16Rows; i++)
	{
		stGV.row	= i + 1;
		stGV.strText.Format(L"%.4f", dbPosZ - i * m_pstRecipe->move_dist_z);
		m_pGridFocus->SetItem(&stGV);
		m_pGridFocus->SetRowHeight(i+1, 25);
	}

	m_pGridFocus->SetRedraw(TRUE, TRUE);
	m_pGridFocus->Invalidate(TRUE);
}

/*
 desc : Get the recipe file
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgFocu::GetData()
{
	/* Get the value on the controls */
 	m_pstRecipe->amp_idx[1]		= (UINT16)m_edt_int[0].GetTextToNum();		/* LED Power Index (0 ~ 4095) */
	m_pstRecipe->pass_cnt[1]	= (UINT16)m_edt_int[1].GetTextToNum();		/* This is a count to ignore when measuring LEDs */
	m_pstRecipe->valid_cnt[1]	= (UINT16)m_edt_int[2].GetTextToNum();		/* Effective count when measuring LEDs */
	m_pstRecipe->drop_min_cnt[1]= (UINT16)m_edt_int[3].GetTextToNum();		/* The minimum number of discarded values among measured values */
	m_pstRecipe->drop_max_cnt[1]= (UINT16)m_edt_int[4].GetTextToNum();		/* The maximum number of discarded values among measured values */
	m_pstRecipe->mat_updn		= (UINT8)m_edt_int[5].GetTextToNum();		/* Axis-Z (UpDown) Movement Count */
	m_pstRecipe->focus_pos_z	= m_edt_flt[0].GetTextToDouble();			/* Axis-Z (UpDown) Focus Position (unit: mm) */
	m_pstRecipe->move_dist_z	= m_edt_flt[1].GetTextToDouble();			/* Axis-Z (UpDown) Movement interval (unit: mm) */

	if (!m_pstRecipe->IsValidFocus())
	{
		AfxMessageBox(L"Invalid value exists [FOCUS]", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : Update measurement data and controls
 parm : None
 retn : None
*/
VOID CDlgFocu::SetData()
{
	if (!m_pstRecipe->IsValidFocus())	return;

	/* Update the controls */
	m_cmb_ctl[0].SetCurSel(m_pstRecipe->ph_no-1);				/* Photohead Number (0x01 ~ 0x08) */
	m_cmb_ctl[1].SetCurSel(m_pstRecipe->led_no-1);				/* The measured LED Number (0x01 ~ 0x04, en_365nm ~ en_...) */
	m_edt_int[0].SetTextToNum(m_pstRecipe->amp_idx[1]);			/* LED Power Index (0 ~ 4095) */
	m_edt_int[1].SetTextToNum(m_pstRecipe->pass_cnt[1]);		/* This is a count to ignore when measuring LEDs */
	m_edt_int[2].SetTextToNum(m_pstRecipe->valid_cnt[1]);		/* Effective count when measuring LEDs */
	m_edt_int[3].SetTextToNum(m_pstRecipe->drop_min_cnt[1]);	/* The minimum number of discarded values among measured values */
	m_edt_int[4].SetTextToNum(m_pstRecipe->drop_max_cnt[1]);	/* The maximum number of discarded values among measured values */
	m_edt_int[5].SetTextToNum(m_pstRecipe->mat_updn);			/* Axis-Z (UpDown) Movement Count */
	m_edt_flt[0].SetTextToNum(m_pstRecipe->focus_pos_z, 4);		/* Axis-Z (UpDown) Focus Position (unit: mm) */
	m_edt_flt[1].SetTextToNum(m_pstRecipe->move_dist_z, 4);		/* Axis-Z (UpDown) Movement interval (unit: mm) */

	/* Rebuild the grid control */
	RebuildGrid();
}

/*
 desc : Update the measurement information in the grid control
 parm : mode	- [in]  0x00 : Real-time data, 0x01 : Calculated data
 retn : None
*/
VOID CDlgFocu::UpdateWork(UINT8 mode)
{
	UINT16 u16Ignore	= m_pWorkFocus->GetIgnoreCount();
	UINT16 u16Valid		= m_pWorkFocus->GetValidCount();
	GV_ITEM stGV		= {NULL};
	DOUBLE dbPower[3]	= {DBL_MAX, DBL_MAX, DBL_MAX};

	/* Basic properties of grid controls */
	stGV.mask	= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr= RGB(32, 32, 32);
	stGV.crBkClr= RGB(250, 250, 250);
	stGV.lfFont	= g_lf;
	stGV.row	= m_pWorkFocus->GetMoveTotal() + 1;

	if (0x00 == mode)
	{
		/* Ignore / Valid Count */
		stGV.nFormat= DT_VCENTER | DT_CENTER | DT_SINGLELINE;
		stGV.col	= 1;
		stGV.strText.Format(L"%4u   %4u", u16Ignore, u16Valid);
		m_pGridFocus->SetItem(&stGV);
		m_pGridFocus->InvalidateRowCol(stGV.row, stGV.col);
		/* Current mWatt */
		stGV.nFormat= DT_VCENTER | DT_RIGHT | DT_SINGLELINE;
		stGV.col	= 2;
		stGV.strText.Format(L"%.5f", m_pWorkFocus->GetLastPower() * 1000.0f);
		m_pGridFocus->SetItem(&stGV);
		m_pGridFocus->InvalidateRowCol(stGV.row, stGV.col);
	}
	else
	{
		/* Max - Min (mW) */
		stGV.nFormat= DT_VCENTER | DT_RIGHT | DT_SINGLELINE;
		dbPower[0]	= m_pWorkFocus->GetMaxMinDiff() * 1000.0f;
		stGV.col	= 3;
		stGV.strText.Format(L"%.5f", dbPower[0]);
		m_pGridFocus->SetItem(&stGV);
		m_pGridFocus->InvalidateRowCol(stGV.row, stGV.col);

		/* Average (mW) */
		dbPower[1]	= m_pWorkFocus->GetAverage() * 1000.0f;
		stGV.col	= 4;
		stGV.strText.Format(L"%.6f", dbPower[1]);
		m_pGridFocus->SetItem(&stGV);
		m_pGridFocus->InvalidateRowCol(stGV.row, stGV.col);

		/* Standard Deviation (mW) */
		dbPower[2]	= m_pWorkFocus->GetStdDev() * 1000.0f;
		stGV.col	= 5;
		stGV.strText.Format(L"%.6f", dbPower[2]);
		m_pGridFocus->SetItem(&stGV);
		m_pGridFocus->InvalidateRowCol(stGV.row, stGV.col);

		/* Change the color to the largest value */
		if (m_stMaxPwr[0].row > 0)
		{
			/* The different (Max - Min) Power (Set the Minimum Power) */
			if (m_stMaxPwr[0].power > dbPower[0])
			{
				/* Resets the previous minimum value */
				m_pGridFocus->SetItemFgColour(m_stMaxPwr[0].row, 3, m_clrGridFg);
				m_pGridFocus->InvalidateRowCol(m_stMaxPwr[0].row, 3);
				/* Updates the new minimum value */
				m_stMaxPwr[0].row	= stGV.row;
				m_stMaxPwr[0].col	= 3;
				m_stMaxPwr[0].power	= dbPower[0];
				m_pGridFocus->SetItemFgColour(m_stMaxPwr[0].row, 3, RGB(255, 0, 0));
				m_pGridFocus->InvalidateRowCol(m_stMaxPwr[0].row, 3);
			}
			/* The average power (Set the Minimum Power) */
			if (m_stMaxPwr[1].power > dbPower[1])
			{
				/* Resets the previous minimum value */
				m_pGridFocus->SetItemFgColour(m_stMaxPwr[1].row, 4, m_clrGridFg);
				m_pGridFocus->InvalidateRowCol(m_stMaxPwr[1].row, 4);
				/* Updates the new minimum value */
				m_stMaxPwr[1].row	= stGV.row;
				m_stMaxPwr[1].col	= 4;
				m_stMaxPwr[1].power	= dbPower[1];
				m_pGridFocus->SetItemFgColour(m_stMaxPwr[1].row, 4, RGB(255, 0, 0));
				m_pGridFocus->InvalidateRowCol(m_stMaxPwr[1].row, 4);
			}
			/* The standard devipation power (Set the Minimum Power) */
			if (m_stMaxPwr[2].power > dbPower[2])
			{
				/* Resets the previous minimum value */
				m_pGridFocus->SetItemFgColour(m_stMaxPwr[2].row, 5, m_clrGridFg);
				m_pGridFocus->InvalidateRowCol(m_stMaxPwr[2].row, 5);
				/* Updates the new minimum value */
				m_stMaxPwr[2].row	= stGV.row;
				m_stMaxPwr[2].col	= 5;
				m_stMaxPwr[2].power	= dbPower[2];
				m_pGridFocus->SetItemFgColour(m_stMaxPwr[2].row, 5, RGB(255, 0, 0));
				m_pGridFocus->InvalidateRowCol(m_stMaxPwr[2].row, 5);
			}
		}
		else
		{
			/* The different (Max - Min) Power */
			m_stMaxPwr[0].row	= stGV.row;
			m_stMaxPwr[0].col	= stGV.col;
			m_stMaxPwr[0].power	= dbPower[0];
			m_pGridFocus->SetItemFgColour(m_stMaxPwr[0].row, 3, RGB(255, 0, 0));
			m_pGridFocus->InvalidateRowCol(m_stMaxPwr[0].row, 3);
			/* The average power */
			m_stMaxPwr[1].row	= stGV.row;
			m_stMaxPwr[1].col	= stGV.col;
			m_stMaxPwr[1].power	= dbPower[1];
			m_pGridFocus->SetItemFgColour(m_stMaxPwr[1].row, 4, RGB(255, 0, 0));
			m_pGridFocus->InvalidateRowCol(m_stMaxPwr[1].row, 4);
			/* The standard deviation power */
			m_stMaxPwr[2].row	= stGV.row;
			m_stMaxPwr[2].col	= stGV.col;
			m_stMaxPwr[2].power	= dbPower[2];
			m_pGridFocus->SetItemFgColour(m_stMaxPwr[1].row, 5, RGB(255, 0, 0));
			m_pGridFocus->InvalidateRowCol(m_stMaxPwr[1].row, 5);
		}
	}
}

/*
 desc : Save the grid's data to a file
 parm : None
 retn : None
*/
VOID CDlgFocu::SaveGrid()
{
	PTCHAR ptzFile	= GetGridFile(L"focus");
	if (ptzFile && m_pGridFocus->Save(ptzFile))
	{
		AfxMessageBox(L"Succeeded in saving the file [FOCUS]", MB_OK);
	}
}

/*
 desc : Get the photohead number
 parm : None
 retn : 0x01 ~ 0x08 (succ) or 0x00 (fail)
*/
UINT8 CDlgFocu::GetPhNo()
{
	INT64 i64PhNo	= m_cmb_ctl[0].GetLBTextToNum();
	if (i64PhNo < 1)	return 0x00;
	return UINT8(i64PhNo);
}

/*
 desc : Get the current step name
 parm : None
 retn : Step Name or L""
*/
PTCHAR CDlgFocu::GetStepName()
{
	if (!m_pWorkFocus)	return L"";
	return m_pWorkFocus->GetStepName();
}

/*
 desc : Check if the focus position is out of focus
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgFocu::IsValidFocusPH()
{
	UINT8 u8Valid		= 0x00;
	UINT8 u8PhNo		= (UINT8)m_cmb_ctl[0].GetLBTextToNum();
	TCHAR tzOut[2][8]	= { L"Down", L"Up" }, tzMesg[128] = {NULL};
	ENG_MMDI enDrv		= ENG_MMDI(UINT8(ENG_MMDI::en_axis_ph1)+u8PhNo-0x01);
	LPG_CMSI pstMC2		= &uvEng_GetConfig()->mc2_svc;

	/* Check if the measurement position is out of range */
	u8Valid	= m_pstRecipe->IsValidFocusRange(pstMC2->min_dist[UINT8(enDrv)], pstMC2->max_dist[UINT8(enDrv)]);
	if (!uvEng_GetConfig()->IsRunDemo() && 0x00 != u8Valid)
	{
		
		swprintf_s(tzMesg, 128, L"It's out of the measurement range of photohead (%s)", tzOut[u8Valid-1]);
		AfxMessageBox(tzMesg, MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : OnNotifyGridSelechanged Event
 parm : nmhdr	- [in] the handle and ID of the control sending the message and the notification code
		result	- [in]  A pointer to the LRESULT variable to store the result code if the message has been handled.
 retn : None
*/
VOID CDlgFocu::OnGridSelChanged(NMHDR *nmhdr, LRESULT *result)
{
	NM_GRIDVIEW *pItem	= (NM_GRIDVIEW*)nmhdr;
	CString strPosZ;
	if (!pItem)	return;

	strPosZ.Empty();
	if (pItem->iRow < 0 || pItem->iColumn != 4)
	{
		m_pGridFocus->SetSelectedRange(-1, -1, -1, -1);
		m_pGridFocus->SetFocusCell(-1, -1);
		m_pGridFocus->Invalidate();
		strPosZ.Format(L"%.2f", m_pstRecipe->focus_pos_z);
	}
	/* In the average's grid control */
	else
	{
		/* Get the center position (Z) */
		strPosZ	= m_pGridFocus->GetItemText(pItem->iRow, 0);
	}

	/* Update the value to control windows */
	CString strPwr	= m_pGridFocus->GetItemText(pItem->iRow, pItem->iColumn);
	if (strPwr.GetLength() > 0)
	{
		m_edt_flt[0].SetTextToStr(strPosZ.GetBuffer());
	}
}
