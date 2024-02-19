
/*
 desc : Measure the illuminance of the photohead (LED)
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgMeas.h"

#include "../Work/WorkMeas.h"
#include "../Data/Recipe.h"


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
CDlgMeas::CDlgMeas(UINT32 id, LPG_MICL recipe, UINT8 menu_id, CWnd* parent)
	: CDlgMenu(id, recipe, menu_id, parent)
{
	m_pWorkMeas	= NULL;
	m_pGridMeas	= NULL;
}

/*
 desc : destructor
 parm : None
 retn : None
*/
CDlgMeas::~CDlgMeas()
{
}

/*
 desc : window id mapping
 parm : dx	- object for data exchange and validation
 retn : None
*/
VOID CDlgMeas::DoDataExchange(CDataExchange* dx)
{
	CDlgMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Groupbox - Normal */
	u32StartID	= IDC_MEAS_GRP_MEAS;
	for (i=0; i<5; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Static - Normal */
	u32StartID	= IDC_MEAS_TXT_INDEX_BEGIN;
	for (i=0; i<8; i++)		DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Editbox - Integer */
	u32StartID	= IDC_MEAS_EDT_INDEX_BEGIN;
	for (i=0; i<7; i++)		DDX_Control(dx, u32StartID+i,	m_edt_int[i]);
	/* Combobox - Normal */
	u32StartID	= IDC_MEAS_CMB_PARM_PH_NO;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_cmb_ctl[i]);
	/* Checkbox - Frequence */
	u32StartID	= IDC_MEAS_CHK_LED_365;
	for (i=0; i<4; i++)		DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgMeas, CDlgMenu)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_MEAS_CHK_LED_365,	IDC_MEAS_CHK_LED_405,	OnChkClicked)
END_MESSAGE_MAP()

/*
 desc : PreTranslateMessage
 parm : msg	- Message Structure Pointer
 retn : TRUE or FALSE
*/
BOOL CDlgMeas::PreTranslateMessage(MSG* msg)
{

	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : When the windows is running, it is initially called once
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMeas::OnInitDlg()
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
 desc : When the window is stopping, it is called once at the last
 parm : None
 retn : None
*/
VOID CDlgMeas::OnExitDlg()
{
	CDlgMenu::OnExitDlg();

	/* Delete the grid control */
	if (m_pGridMeas)
	{
		if (m_pGridMeas->GetSafeHwnd())	m_pGridMeas->DestroyWindow();
		delete m_pGridMeas;
	}
}

/*
 desc : Called whenever the window is updated
 parm : dc	- device context
 retn : None
*/
VOID CDlgMeas::OnPaintDlg(CDC *dc)
{
}

/*
 desc : Initialize the controls
 parm : None
 retn : None
*/
VOID CDlgMeas::InitCtrl()
{
	INT32 i;

	/* Groupbox - Normal */
	for (i=0; i<5; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* Static - Normal */
	for (i=0; i<8; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(TRUE);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(255, 255, 255), 0, RGB(0, 0, 0));
	}
	/* Edit box - Integer (Unsigned) */
	for (i=0; i<7; i++)
	{
		m_edt_int[i].SetEditFont(&g_lf);
		m_edt_int[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_int[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_int[i].SetInputType(ENM_DITM::en_uint16);
		m_edt_int[i].SetUseMinMax(TRUE);
		m_edt_int[i].SetInputSign(ENM_MNUS::en_unsign);
		m_edt_int[i].SetReadOnly(FALSE);
		m_edt_int[i].SetMouseClick(TRUE);

		if (i < 2)		m_edt_int[i].SetMinMaxNum(UINT16(0), UINT16(MAX_LED_POWER));
		else if (i < 5)	m_edt_int[i].SetMinMaxNum(UINT16(1), UINT16(100));
		else if (i < 7)	m_edt_int[i].SetMinMaxNum(UINT16(1), UINT16(32));
	}
	/* Combobox - Normal */
	for (i=0; i<1; i++)	m_cmb_ctl[i].SetLogFont(&g_lf);
	for (i=0; i<uvEng_GetConfig()->luria_svc.ph_count; i++)	m_cmb_ctl[0].AddNumber(UINT64(i+1));
	if (m_cmb_ctl[0].GetCount() > 0)	m_cmb_ctl[0].SetCurSel(m_pstRecipe->ph_no-1);
	m_cmb_ctl[0].EnableWindow(FALSE);
	/* Checkbox - Frequence */
	for (i=0; i<4; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf);
		m_chk_ctl[i].SetBgColor(g_clrBtnColor);
	}
	m_chk_ctl[0].SetCheck(1);
}

/*
 desc : Set the initial value in the controls
 parm : None
 retn : None
*/
VOID CDlgMeas::InitData()
{
	m_edt_int[0].SetTextToNum(UINT32(100));		/* Index - Begin */
	m_edt_int[1].SetTextToNum(UINT32(23));		/* Index - Count */
	m_edt_int[2].SetTextToNum(UINT32(50));		/* Index - Interval */
	m_edt_int[3].SetTextToNum(UINT32(0));		/* Measurement - Pass */
	m_edt_int[4].SetTextToNum(UINT32(5));		/* Measurement - Valid */
	m_edt_int[5].SetTextToNum(UINT32(1));		/* Ignore - Min */
	m_edt_int[6].SetTextToNum(UINT32(1));		/* Ignore - Max */
}

/*
 desc : Create a grid control for Measurement Information
 parm : None
 retn : None
*/
VOID CDlgMeas::InitGrid()
{
	TCHAR tzCols[8][16]	= { L"Index", L"Count", L"Last (mW)", L"Max-Min (mW)", L"Avg (mW)", L"Std (mW)" };
	INT32 i32Width[6]	= { 50, 110, 110, 110, 110, 110 }, i, j=1;
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

	/* Create a object */
	m_pGridMeas	= new CGridCtrl;
	ASSERT(m_pGridMeas);
	m_pGridMeas->SetDrawScrollBarHorz(FALSE);
	m_pGridMeas->SetDrawScrollBarVert(TRUE);
	if (!m_pGridMeas->Create(rGrid, this, IDC_GRID_MEAS_RET, dwStyle))
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
	m_pGridMeas->SetRowHeight(0, 28);
	m_pGridMeas->SetFixedColumnCount(0);
	m_pGridMeas->SetBkColor(RGB(255, 255, 255));
	m_pGridMeas->SetFixedColumnSelection(0);

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
		m_pGridMeas->SetItem(&stGV);
		m_pGridMeas->SetColumnWidth(i, i32Width[i]);
	}

	m_pGridMeas->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGridMeas->UpdateGrid();
}

/*
 desc : Updates the controls
 parm : None
 retn : None
*/
VOID CDlgMeas::UpdateCtrl()
{
	/* Enable or Disable for Controls */
	EnableCtrl();
}

/*
 desc : Enable or Disable for Controls
 parm : None
 retn : None
*/
VOID CDlgMeas::EnableCtrl()
{
	UINT8 i	= 0x00;
	BOOL bWorkInM	= !m_pWorkMeas ? FALSE : m_pWorkMeas->IsBusy();

	/* Edit box - Integer */
	for (i=0x00; i<7; i++)	m_edt_int[i].EnableWindow(!bWorkInM);
}

/*
 desc : Create the work job
 parm : None
 retn : None
*/
VOID CDlgMeas::CreateWork()
{
	UINT8 u8LedNo	= GetLedNo();
	UINT8 i	= 0x00, u8PhCnt	= uvEng_GetConfig()->luria_svc.ph_count;
	UINT16 u16Drop[2], u16Meas[2], u16Index[3];

	ReleaseWork();

	/* Get the index of LED Power*/
	u16Index[0]	= (UINT16)m_edt_int[0].GetTextToNum();
	u16Index[1]	= (UINT16)m_edt_int[1].GetTextToNum();
	u16Index[2]	= (UINT16)m_edt_int[2].GetTextToNum();
	if (u16Index[0] < 1 || u16Index[1] < 1 || u16Index[2] < 1)
	{
		AfxMessageBox(L"The index count is not valid", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* Get the drop count */
	u16Drop[0]	= (UINT16)m_edt_int[5].GetTextToNum();
	u16Drop[1]	= (UINT16)m_edt_int[6].GetTextToNum();
	if (u16Drop[0] < 1 || u16Drop[1] < 1)
	{
		AfxMessageBox(L"The drop count is not valid", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* Get the measurement count */
	u16Meas[0]	= (UINT16)m_edt_int[3].GetTextToNum();
	u16Meas[1]	= (UINT16)m_edt_int[4].GetTextToNum();
	if (/*u16Meas[0] < 1 || */u16Meas[1] < 1)
	{
		AfxMessageBox(L"The measurement count is not valid", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* Check whether the value between the number of DROPs and the number of MEASUREMENTs is correct. */
	if (u16Meas[1] < (u16Drop[0]+u16Drop[1]))
	{
		AfxMessageBox(L"The number of <drop> and <measurements> are incompatible", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* Initalize the maximum value in each grid control area */
	memset(m_stMinPwr, 0x00, sizeof(STM_GMMV) * 3);
	/* Start the Job */
	m_pWorkMeas	= new CWorkMeas(u8LedNo, u16Index, m_pstRecipe, m_u8RunMode, m_dbMaxMin);
	ASSERT(m_pWorkMeas);
	m_pWorkMeas->InitWork();
}

/*
 desc : Release the work job
 parm : None
 retn : None
*/
VOID CDlgMeas::ReleaseWork()
{
	if (m_pWorkMeas)
	{
		m_pWorkMeas->StopWork();
		delete m_pWorkMeas;
		m_pWorkMeas	= NULL;
	}
}

/*
 desc : Initializes for LED Power measurement
 parm : run_mode- [in]  0x00 : Low Speed (Safe Mode), 0x01 : High Speed (Incomplete Mode)
		max_min	- [in]  Max-Min Condition
						If the difference between the maximum and minimum values of the data stored
						in the queue buffer is less than this value, collection is possible
 retn : TRUE or FALSE
*/
BOOL CDlgMeas::StartWork(UINT8 run_mode, DOUBLE max_min)
{
	UINT8 u8LedNo	= GetLedNo();
	UINT16 i		= 0x01;
	UINT16 u16Begin	= (UINT16)m_edt_int[0].GetTextToNum();
	UINT16 u16End	= (UINT16)m_edt_int[1].GetTextToNum();
	UINT16 u16Intvl	= (UINT16)m_edt_int[2].GetTextToNum();
	UINT16 u16Rows	= (u16End - u16Begin) / u16Intvl + 1;
	GV_ITEM stGV	= { NULL };

	if (!CDlgMenu::StartWork(run_mode, max_min))	return FALSE;

	/* Check whether the set values are valid */
	if (!m_pstRecipe->IsValidMeas())
	{
		AfxMessageBox(L"The set value (measurement) is not valid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	if (!IsValidParam())
	{
		AfxMessageBox(L"Invalid parameter values for measurement", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

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
VOID CDlgMeas::StopWork()
{
	ReleaseWork();
}

/*
 desc : Whether it's currently being executed.
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMeas::IsWorkBusy()
{
	if (m_pWorkMeas)
	{
		if (m_pWorkMeas)	return m_pWorkMeas->IsBusy();
	}

	return FALSE;
}

/*
 desc : It is called periodically
 parm : None
 retn : None
*/
VOID CDlgMeas::DoWork()
{
	/* If the work is idle */
	if (!m_pWorkMeas)	return;
	/* Check if everything is done... */
	if (!m_pWorkMeas->IsEnded())
	{
		m_pWorkMeas->DoWork();
		if (m_pWorkMeas->IsUpdatePower())	UpdateWork(0x00);
		if (m_pWorkMeas->IsUpdatedCalc())	UpdateWork(0x01);
	}
}

/*
 desc : Rebuild the grid control
 parm : None
 retn : None
*/
VOID CDlgMeas::RebuildGrid()
{
	INT32 i, i32Rows= (INT32)m_edt_int[1].GetTextToNum();
	INT32 i32Begin	= (INT32)m_edt_int[0].GetTextToNum();
	INT32 i32Intvl	= (INT32)m_edt_int[2].GetTextToNum();
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
	m_pGridMeas->SetRedraw(FALSE);
	m_pGridMeas->DeleteNonFixedRows();
	m_pGridMeas->SetRedraw(TRUE);

	/* Set the rows and columns of grid control */
	m_pGridMeas->SetRedraw(FALSE);
	m_pGridMeas->SetRowCount(i32Rows+1);

	for (i=0; i<i32Rows; i++)
	{
		stGV.row	= i + 1;
		stGV.strText.Format(L"%u", i32Begin + i * i32Intvl);
		m_pGridMeas->SetItem(&stGV);
		m_pGridMeas->SetRowHeight(i+1, 25);
	}

	m_pGridMeas->SetRedraw(TRUE, TRUE);
	m_pGridMeas->Invalidate(TRUE);
}

/*
 desc : Get the recipe file
 parm : None
 retn : None
*/
BOOL CDlgMeas::GetData()
{
	m_pstRecipe->pass_cnt[2]	= (UINT16)m_edt_int[3].GetTextToNum();	/* This is a count to ignore when measuring LEDs */
	m_pstRecipe->valid_cnt[2]	= (UINT16)m_edt_int[4].GetTextToNum();	/* Effective count when measuring LEDs */
	m_pstRecipe->drop_min_cnt[2]= (UINT16)m_edt_int[5].GetTextToNum();	/* The minimum number of discarded values among measured values */
	m_pstRecipe->drop_max_cnt[2]= (UINT16)m_edt_int[6].GetTextToNum();	/* The maximum number of discarded values among measured values */

	if (!m_pstRecipe->IsValidMeas())
	{
		AfxMessageBox(L"Invalid value exists [MEAS]", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : Inspect parameters for measurement
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMeas::IsValidParam()
{
	if (m_edt_int[0].GetTextToNum() < 1 ||
		m_edt_int[1].GetTextToNum() < 1 ||
		m_edt_int[2].GetTextToNum() < 1)	return FALSE;
	return TRUE;
}

/*
 desc : Update measurement data and controls
 parm : None
 retn : None
*/
VOID CDlgMeas::SetData()
{
	if (!IsValidParam())				return;
	if (!m_pstRecipe->IsValidMeas())	return;

	/* Update the controls */
	m_cmb_ctl[0].SetCurSel(m_pstRecipe->ph_no-1);				/* Photohead Number (0x01 ~ 0x08) */
	m_edt_int[3].SetTextToNum(m_pstRecipe->pass_cnt[2]);		/* This is a count to ignore when measuring LEDs */
	m_edt_int[4].SetTextToNum(m_pstRecipe->valid_cnt[2]);		/* Effective count when measuring LEDs */
	m_edt_int[5].SetTextToNum(m_pstRecipe->drop_min_cnt[2]);	/* The minimum number of discarded values among measured values */
	m_edt_int[6].SetTextToNum(m_pstRecipe->drop_max_cnt[2]);	/* The maximum number of discarded values among measured values */

	/* Rebuild the grid control */
	RebuildGrid();
}

/*
 desc : Called when the checkbox is clicked
 parm : id	- [in]  checkbox id clicked
 retn : None
*/
VOID CDlgMeas::OnChkClicked(UINT32 id)
{
	UINT8 i	= 0x00;
	INT16 i16Freq[4]	= { 365, 385, 395, 405 };

	if (id <= IDC_MEAS_CHK_LED_405)
	{
		for (; i<4; i++)	m_chk_ctl[i].SetCheck(0);
		m_chk_ctl[id - IDC_MEAS_CHK_LED_365].SetCheck(1);
		/* Set the frequency value to be measured in the PM100D */
		if (uvEng_PM100D_IsConnected())
		{
			if (!uvEng_PM100D_SetQueryWaveLength(i16Freq[id - IDC_MEAS_CHK_LED_365]))
			{
				AfxMessageBox(L"Failed to set the frequency value", MB_ICONSTOP|MB_TOPMOST);
				return;
			}
		}
	}
	else
	{
		m_chk_ctl[id - IDC_MEAS_CHK_LED_365].SetCheck(1);
	}
}

/*
 desc : Save the grid's data to a file
 parm : None
 retn : None
*/
VOID CDlgMeas::SaveGrid()
{
	PTCHAR ptzFile	= GetGridFile(L"meas");
	if (ptzFile && m_pGridMeas->Save(ptzFile))
	{
		AfxMessageBox(L"Succeeded in saving the file [MEAS]", MB_OK);
	}
}

/*
 desc : Get the led number
 parm : None
 retn : 0x01 ~ 0x08 (succ) or 0x00 (fail)
*/
UINT8 CDlgMeas::GetLedNo()
{
	UINT8 i	= 0x00;
	for (; i<4; i++)
	{
		if (1 == m_chk_ctl[i].GetCheck())	return (i+1);
	}
	return 0x00;
}

/*
 desc : Get the current step name
 parm : None
 retn : Step Name or L""
*/
PTCHAR CDlgMeas::GetStepName()
{
	if (!m_pWorkMeas)	return L"";
	return m_pWorkMeas->GetStepName();
}

/*
 desc : Update the measurement information in the grid control
 parm : mode	- [in]  0x00 : Real-time data, 0x01 : Calculated data
 retn : None
*/
VOID CDlgMeas::UpdateWork(UINT8 mode)
{
	UINT16 u16Ignore	= m_pWorkMeas->GetIgnoreCount();
	UINT16 u16Valid		= m_pWorkMeas->GetValidCount();
	BOOL bEnded			= FALSE;
	DOUBLE dbPower[3]	= {DBL_MAX, DBL_MAX, DBL_MAX};
	GV_ITEM stGV		= {NULL};
	CString strVal;

	/* Basic properties of gird controls */
	stGV.mask	= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr= RGB(16, 16, 16);
	stGV.crBkClr= RGB(255, 255, 255);
	stGV.lfFont	= g_lf;
	stGV.row	= m_pWorkMeas->GetMeasCount()+1;

	if (0x00 == mode)
	{
		/* Ignore / Valid Count */
		stGV.nFormat= DT_VCENTER | DT_CENTER | DT_SINGLELINE;
		stGV.strText.Format(L"%4u,%4u", u16Ignore, u16Valid);
		stGV.col	= 1;
		m_pGridMeas->SetItem(&stGV);
		m_pGridMeas->InvalidateRowCol(stGV.row, stGV.col);
		/* Current Power */
		stGV.nFormat= DT_VCENTER | DT_RIGHT | DT_SINGLELINE;
		stGV.strText.Format(L"%.6f", m_pWorkMeas->GetLastPower() * 1000.0f);
		stGV.col	= 2;
		m_pGridMeas->SetItem(&stGV);
		m_pGridMeas->InvalidateRowCol(stGV.row, stGV.col);
	}
	else
	{
		/* Max - Min (mW) */
		stGV.nFormat= DT_VCENTER | DT_RIGHT | DT_SINGLELINE;
		dbPower[0]	= m_pWorkMeas->GetMaxMinDiff() * 1000.0f;
		stGV.strText.Format(L"%.6f", dbPower[0]);
		stGV.col	= 3;
		m_pGridMeas->SetItem(&stGV);
		m_pGridMeas->InvalidateRowCol(stGV.row, stGV.col);
		/* Average (mW) */
		dbPower[1]	= m_pWorkMeas->GetAverage() * 1000.0f;
		stGV.strText.Format(L"%.6f", dbPower[1]);
		stGV.col	= 4;
		m_pGridMeas->SetItem(&stGV);
		m_pGridMeas->InvalidateRowCol(stGV.row, stGV.col);
		/* Standard Deviation (mW) */
		dbPower[2]	= m_pWorkMeas->GetStdDev() * 1000.0f;
		stGV.strText.Format(L"%.6f", dbPower[2]);
		stGV.col	= 5;
		m_pGridMeas->SetItem(&stGV);
		m_pGridMeas->InvalidateRowCol(stGV.row, stGV.col);
#if 0
		if (m_stMinPwr[0].row > 0)
		{
			/* The different (Max - Min) Power (Set the Minimum Power) */
			if (m_stMinPwr[0].power > dbPower[0])
			{
				/* Resets the previous minimum value */
				m_pGridMeas->SetItemFgColour(m_stMinPwr[0].row, m_stMinPwr[0].col, m_clrGridFg);
				m_pGridMeas->InvalidateRowCol(m_stMinPwr[0].row, 3);
				/* Updates the new minimum value */
				m_stMinPwr[0].row	= stGV.row;
				m_stMinPwr[0].col	= 3;
				m_stMinPwr[0].power	= dbPower[0];
				m_pGridMeas->SetItemFgColour(m_stMinPwr[0].row, m_stMinPwr[0].col, RGB(255, 0, 0));
				m_pGridMeas->InvalidateRowCol(m_stMinPwr[0].row, m_stMinPwr[0].col);
			}
			/* The average power (Set the Minimum Power) */
			if (m_stMinPwr[1].power > dbPower[1])
			{
				/* Resets the previous maximum value */
				m_pGridMeas->SetItemFgColour(m_stMinPwr[1].row, m_stMinPwr[1].col, m_clrGridFg);
				m_pGridMeas->InvalidateRowCol(m_stMinPwr[1].row, m_stMinPwr[1].col);
				/* Updates the new maximum value */
				m_stMinPwr[1].row	= stGV.row;
				m_stMinPwr[1].col	= 4;
				m_stMinPwr[1].power	= dbPower[1];
				m_pGridMeas->SetItemFgColour(m_stMinPwr[1].row, m_stMinPwr[1].col, RGB(255, 0, 0));
				m_pGridMeas->InvalidateRowCol(m_stMinPwr[1].row, m_stMinPwr[1].col);
			}
			/* The std_dev power (Set the Minimum Power) */
			if (m_stMinPwr[2].power > dbPower[2])
			{
				/* Resets the previous maximum value */
				m_pGridMeas->SetItemFgColour(m_stMinPwr[2].row, m_stMinPwr[2].col, m_clrGridFg);
				m_pGridMeas->InvalidateRowCol(m_stMinPwr[2].row, m_stMinPwr[2].col);
				/* Updates the new maximum value */
				m_stMinPwr[2].row	= stGV.row;
				m_stMinPwr[2].col	= 5;
				m_stMinPwr[2].power	= dbPower[1];
				m_pGridMeas->SetItemFgColour(m_stMinPwr[2].row, m_stMinPwr[2].col, RGB(255, 0, 0));
				m_pGridMeas->InvalidateRowCol(m_stMinPwr[2].row, m_stMinPwr[2].col);
			}
		}
		else
		{
			/* The different (Max - Min) Power */
			m_stMinPwr[0].row	= stGV.row;
			m_stMinPwr[0].col	= 3;
			m_stMinPwr[0].power	= dbPower[0];
			m_pGridMeas->SetItemFgColour(m_stMinPwr[0].row, m_stMinPwr[0].col, RGB(255, 0, 0));
			m_pGridMeas->InvalidateRowCol(m_stMinPwr[0].row, m_stMinPwr[0].col);
			/* The average power */
			m_stMinPwr[1].row	= stGV.row;
			m_stMinPwr[1].col	= 4;
			m_stMinPwr[1].power	= dbPower[1];
			m_pGridMeas->SetItemFgColour(m_stMinPwr[1].row, m_stMinPwr[1].col, RGB(255, 0, 0));
			m_pGridMeas->InvalidateRowCol(m_stMinPwr[1].row, m_stMinPwr[1].col);
			/* The average power */
			m_stMinPwr[2].row	= stGV.row;
			m_stMinPwr[2].col	= 5;
			m_stMinPwr[2].power	= dbPower[1];
			m_pGridMeas->SetItemFgColour(m_stMinPwr[2].row, m_stMinPwr[2].col, RGB(255, 0, 0));
			m_pGridMeas->InvalidateRowCol(m_stMinPwr[2].row, m_stMinPwr[2].col);
		}
#endif
	}
}
