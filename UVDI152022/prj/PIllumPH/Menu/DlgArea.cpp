
/*
 desc : Initial Position Search
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgArea.h"

#include "../Work/WorkArea.h"
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
CDlgArea::CDlgArea(UINT32 id, LPG_MICL recipe, UINT8 menu_id, CWnd* parent)
	: CDlgMenu(id, recipe, menu_id, parent)
{
	m_pWorkArea		= NULL;
	m_u32Width[0]	= 54;
	m_u32Width[1]	= 68;
	m_u32Width[2]	= 61;
}

/*
 desc : destructor
 parm : None
 retn : None
*/
CDlgArea::~CDlgArea()
{
}

/*
 desc : window id mapping
 parm : dx	- object for data exchange and validation
 retn : None
*/
VOID CDlgArea::DoDataExchange(CDataExchange* dx)
{
	CDlgMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Groupbox - Normal */
	u32StartID	= IDC_AREA_GRP_ILLUM_PWR;
	for (i=0; i<10; i++)	DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Button - Normal */
	u32StartID	= IDC_AREA_BTN_FILTER_RATE;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Checkbox - Normal */
	u32StartID	= IDC_AREA_CHK_ILLUM_PWR;
	for (i=0; i<4; i++)		DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
	/* Combobox - Normal */
	u32StartID	= IDC_AREA_CMB_PARM_PH_NO;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_cmb_ctl[i]);
	/* Static - Normal */
	u32StartID	= IDC_AREA_TXT_PARAM_LED_IDX;
	for (i=0; i<22; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Editbox - Integer */
	u32StartID	= IDC_AREA_EDT_MAT_ROW;
	for (i=0; i<7; i++)		DDX_Control(dx, u32StartID+i,	m_edt_int[i]);
	/* Editbox - Float */
	u32StartID	= IDC_AREA_EDT_CORRECT_MIN;
	for (i=0; i<13; i++)	DDX_Control(dx, u32StartID+i,	m_edt_flt[i]);
}

BEGIN_MESSAGE_MAP(CDlgArea, CDlgMenu)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_AREA_BTN_FILTER_RATE,	IDC_AREA_BTN_FILTER_RATE,	OnBtnClicked)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_AREA_CHK_ILLUM_PWR,	IDC_AREA_CHK_FILTER_EXIST,	OnChkClicked)
	ON_NOTIFY(GVN_COLUMNCLICK, IDC_GRID_AREA_PWR, OnGridColumnClicked)
	ON_NOTIFY(GVN_COLUMNCLICK, IDC_GRID_AREA_CNT, OnGridColumnClicked)
	ON_NOTIFY(GVN_COLUMNCLICK, IDC_GRID_AREA_AVG, OnGridColumnClicked)
	ON_NOTIFY(GVN_COLUMNCLICK, IDC_GRID_AREA_DIF, OnGridColumnClicked)
END_MESSAGE_MAP()

/*
 desc : PreTranslateMessage
 parm : msg	- Message Structure Pointer
 retn : TRUE or FALSE
*/
BOOL CDlgArea::PreTranslateMessage(MSG* msg)
{

	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : When the windows is running, it is initially called once
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgArea::OnInitDlg()
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
VOID CDlgArea::OnExitDlg()
{
	CDlgMenu::OnExitDlg();

	UINT8 i	= 0x00;

	/* Delete the grid control */
	for (; i<4; i++)
	{
		if (m_pGridArea[i])
		{
			if (m_pGridArea[i]->GetSafeHwnd())	m_pGridArea[i]->DestroyWindow();
			delete m_pGridArea[i];
		}
	}
}

/*
 desc : Called whenever the window is updated
 parm : dc	- device context
 retn : None
*/
VOID CDlgArea::OnPaintDlg(CDC *dc)
{
}

/*
 desc : Initialize the controls
 parm : None
 retn : None
*/
VOID CDlgArea::InitCtrl()
{
	INT32 i;

	/* Groupbox - Normal */
	for (i=0; i<10; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* Static - Normal */
	for (i=0; i<22; i++)
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
		if (i < 6)			m_edt_int[i].SetMinMaxNum(UINT16(1), UINT16(32));
		else if (i == 6)	m_edt_int[i].SetMinMaxNum(UINT16(0), UINT16(MAX_LED_POWER));
	}
	/* Edit box - Float */
	for (i=0; i<13; i++)
	{
		m_edt_flt[i].SetEditFont(&g_lf);
		m_edt_flt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_flt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_flt[i].SetInputType(ENM_DITM::en_double);
		m_edt_flt[i].SetUseMinMax(TRUE);
		m_edt_flt[i].SetInputSign(ENM_MNUS::en_sign);

		if (i < 4 || i > 10)	m_edt_flt[i].SetInputPoint(6);
		else if (i < 10)		m_edt_flt[i].SetInputPoint(4);
		if (i < 4 || i > 7)
		{
			m_edt_flt[i].SetReadOnly(TRUE);
			m_edt_flt[i].SetMouseClick(FALSE);
		}
		else
		{
			m_edt_flt[i].SetReadOnly(FALSE);
			m_edt_flt[i].SetMouseClick(TRUE);
		}
	}
	/* Combobox - Normal */
	for (i=0; i<2; i++)	m_cmb_ctl[i].SetLogFont(&g_lf);
	for (i=0; i<uvEng_GetConfig()->luria_svc.ph_count; i++)		m_cmb_ctl[0].AddNumber(UINT64(i+1));
	for (i=0; i<uvEng_GetConfig()->luria_svc.led_count; i++)	m_cmb_ctl[1].AddNumber(UINT64(i+1));
	if (m_cmb_ctl[0].GetCount() > 0)	m_cmb_ctl[0].SetCurSel(m_pstRecipe->ph_no-1);
	if (m_cmb_ctl[1].GetCount() > 0)	m_cmb_ctl[1].SetCurSel(m_pstRecipe->led_no-1);
	/* Normal - Button */
	for (i=0; i<1; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
	}
	/* Checkbox - Normal */
	for (i=0; i<4; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf);
		if (0x00 == i)	m_chk_ctl[i].SetBgColor(RGB(255, 255, 255));
		else			m_chk_ctl[i].SetBgColor(g_clrBtnColor);
	}
	/*  It is not currently supported */
	m_chk_ctl[1].EnableWindow(FALSE);
	/* Set to the default */
	m_chk_ctl[2].SetCheck(1);
}

/*
 desc : Set the initial value in the controls
 parm : None
 retn : None
*/
VOID CDlgArea::InitData()
{
	UINT8 i	= 0x00;
	m_edt_int[0].SetTextToNum(UINT8(3));	/* Axis-Y (Vert; Row) Movement Count */
	m_edt_int[1].SetTextToNum(UINT8(3));	/* Axis-X (Horz; Column) Movement Count */
	m_edt_int[2].SetTextToNum(UINT8(1));	/* This is a count to ignore when measuring LEDs */
	m_edt_int[3].SetTextToNum(UINT8(5));	/* Effective count when measuring LEDs */
	m_edt_int[4].SetTextToNum(UINT8(1));	/* The minimum number of discarded values among measured values */
	m_edt_int[5].SetTextToNum(UINT8(1));	/* The maximum number of discarded values among measured values */
	m_edt_int[6].SetTextToNum(UINT16(200));	/* LED Power Index (0 ~ 4095) */

	for (i=0; i<4; i++)	m_edt_flt[i].SetTextToNum(0.0f, 6);
	for (i=4; i<6; i++)	m_edt_flt[i].SetTextToNum(0.0f, 4);
	for (i=6; i<8; i++)	m_edt_flt[i].SetTextToNum(0.25f, 4);
	m_edt_flt[8].SetTextToNum(uvEng_GetConfig()->mc2_svc.max_dist[0], 4);
	m_edt_flt[9].SetTextToNum(uvEng_GetConfig()->mc2_svc.max_dist[1], 4);

	/* Select the number of PH and LED */
	m_cmb_ctl[0].SetCurSel(0);
	m_cmb_ctl[1].SetCurSel(0);
}

/*
 desc : Create a grid control for Measurement Information
 parm : None
 retn : None
*/
VOID CDlgArea::InitGrid()
{
	UINT32 i, u32ID	= IDC_GRID_AREA_PWR;
	UINT32 dwStyle	= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	GV_ITEM stGV	= { NULL };
	CRect rGrid[4]	= {NULL};

	/* Last Recv. Packet / PDO Header / PDO Data (Values) */
	for (i=0; i<4; i++)
	{
		::GetWindowRect(m_grp_ctl[5+i].GetSafeHwnd(), rGrid[i]);
		ScreenToClient(rGrid[i]);
		rGrid[i].left	+= 13;
		rGrid[i].right	-= 13;
		rGrid[i].top	+= 20;
		rGrid[i].bottom	-= 13;
	}

	/* Basic properties of gird controls */
	stGV.mask	= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr= RGB(32, 32, 32);
	stGV.crBkClr= RGB(214, 214, 214);
	stGV.lfFont	= g_lf;
	stGV.nFormat= DT_VCENTER | DT_SINGLELINE | DT_CENTER;
	stGV.row	= 0;
	stGV.col	= 0;
	stGV.strText= L"Y/X";

	/* Create a object */
	for (i=0; i<4; i++)
	{
		m_pGridArea[i]	= new CGridCtrl;
		ASSERT(m_pGridArea[i]);
		m_pGridArea[i]->SetModifyStyleEx(WS_EX_STATICEDGE);
		m_pGridArea[i]->SetDrawScrollBarHorz(TRUE);
		m_pGridArea[i]->SetDrawScrollBarVert(TRUE);
		if (!m_pGridArea[i]->Create(rGrid[i], this, u32ID+i, dwStyle))
		{
			delete m_pGridArea[i];
			m_pGridArea[i] = NULL;
		}
		else
		{
			/* Set default values */
			m_pGridArea[i]->SetLogFont(g_lf);
// 			m_pGridArea[i]->SetRowCount(1);
// 			m_pGridArea[i]->SetColumnCount(1);
			m_pGridArea[i]->SetFixedRowCount(1);
			m_pGridArea[i]->SetFixedColumnCount(1);
			m_pGridArea[i]->SetRowHeight(0, 25);
			m_pGridArea[i]->SetColumnWidth(0, m_u32Width[0]);
			m_pGridArea[i]->SetBkColor(RGB(255, 255, 255));
			m_pGridArea[i]->SetFixedColumnSelection(0);

			/* Set the common control */
			stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
			stGV.crFgClr = RGB(0, 0, 0);

			/* Header Control (First row) */
			m_pGridArea[i]->SetItem(&stGV);
//			m_pGridArea[i]->SetBaseGridProp(1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0);
			m_pGridArea[i]->SetBaseGridProp(1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
			m_pGridArea[i]->UpdateGrid();
		}
	}
}

/*
 desc : Updates the controls
 parm : None
 retn : None
*/
VOID CDlgArea::UpdateCtrl()
{
	/* Enable or Disable for Controls */
	EnableCtrl();
	/* Updates the illuminance value (mW) */
	if (uvEng_PM100D_IsRunMeasure())
	{
		UpdateRealTimeIllum();
	}
}

/*
 desc : Enable or Disable for Controls
 parm : None
 retn : None
*/
VOID CDlgArea::EnableCtrl()
{
	UINT8 i;
	BOOL bWorkIn	= !m_pWorkArea ? FALSE : !m_pWorkArea->IsEnded();
	BOOL bPM100D	= uvEng_PM100D_IsConnected();
	BOOL bInited	= uvEng_Luria_IsServiceInited();
	BOOL bLoaded	= m_pstRecipe->IsValidArea();
	BOOL bActive	= m_chk_ctl[0].GetCheck() == 1 ? TRUE : FALSE;
	BOOL bRunDemo	= uvEng_GetConfig()->IsRunDemo();
	BOOL bEnabled	= bRunDemo || bPM100D && !bWorkIn;

	/* Checkbox */
	m_chk_ctl[0].EnableWindow(bEnabled);	/* Quad Illum. */
	/* Combobox */
	m_cmb_ctl[0].EnableWindow(bEnabled);	/* Photohead Number */
	m_cmb_ctl[1].EnableWindow(bEnabled);	/* Led Number */
	/* Editbox - Integer */
	for (i=0x00; i<7; i++)	m_edt_int[i].EnableWindow(bEnabled);
	/* Editbox - Float */
	for (i=0x00; i<8; i++)	m_edt_flt[i].EnableWindow(bEnabled);
#if 0	/* It is not currently supported */
	m_chk_ctl[1].EnableWindow(bRunDemo || bPM100D && bLoaded && !bWorkIn && bInited && !bActive);	/* Correct */
#endif
}

/*
 desc : Called when the button is clicked
 parm : id	- [in]  checkbox id clicked
 retn : None
*/
VOID CDlgArea::OnBtnClicked(UINT32 id)
{
	if (IDC_AREA_BTN_FILTER_RATE == id)	UpdateFilterRate();
}

/*
 desc : Called when the checkbox is clicked
 parm : id	- [in]  checkbox id clicked
 retn : None
*/
VOID CDlgArea::OnChkClicked(UINT32 id)
{
	UINT8 u8PhNo	= (UINT8)m_cmb_ctl[0].GetLBTextToNum();
	UINT8 u8LedNo	= (UINT8)m_cmb_ctl[1].GetLBTextToNum();
	INT16 i16Freq	= 365;
	UINT16 u16Amp	= (UINT16)m_edt_int[6].GetTextToNum();
	BOOL bCheck		= FALSE;	
	TCHAR tzFile[_MAX_PATH]	= {NULL};

	/* Set the frequency */
	switch (u8LedNo)
	{
	case 0x01 :	i16Freq = 365;	break;
	case 0x02 :	i16Freq = 385;	break;
	case 0x03 :	i16Freq = 395;	break;
	case 0x04 :	i16Freq = 405;	break;
	}

	switch (id)
	{
	case IDC_AREA_CHK_ILLUM_PWR	:
		bCheck = (m_chk_ctl[0].GetCheck() == 1) ? TRUE : FALSE;
		if (bCheck)
		{
			/* Initializes the min/max/avg */
			m_edt_flt[0].SetTextToStr(L"");
			m_edt_flt[1].SetTextToStr(L"");
			m_edt_flt[2].SetTextToStr(L"");
			m_edt_flt[3].SetTextToStr(L"");
			uvEng_PM100D_SetQueryWaveLength(i16Freq);
		}
		/* Turn on or off the LED on the photo head */
//		uvEng_Luria_ReqSetLedLightOnOff(u8PhNo, ENG_LLPI(u8LedNo), UINT8(bCheck));
		/* Start measuring the frequency */
		uvEng_PM100D_RunMeasure(bCheck);
		if (bCheck)
		{
			swprintf_s(tzFile, _MAX_PATH, L"%s\\illu\\real.csv", g_tzWorkDir);
			uvCmn_DeleteForceFile(tzFile);
		}
		break;

	case IDC_AREA_CHK_CORRECT	:
		AfxMessageBox(L"This feature is not currently supported", MB_ICONSTOP|MB_TOPMOST);
		break;

	case IDC_AREA_CHK_FILTER_NONE	:
	case IDC_AREA_CHK_FILTER_EXIST	:
		m_chk_ctl[2].SetCheck(0);
		m_chk_ctl[3].SetCheck(0);
		m_chk_ctl[id-IDC_AREA_CHK_FILTER_NONE].SetCheck(1);	break;
	}
}

/*
 desc : Create the work job
 parm : None
 retn : None
*/
VOID CDlgArea::CreateWork()
{
	ReleaseWork();

	/* Initalize the maximum value in each grid control area */
	memset(m_stMaxPwr, 0x00, sizeof(STM_GMMV) * 2);
	/* Creates a working secinario */
	m_pWorkArea	= new CWorkArea(m_pstRecipe, m_u8RunMode, m_dbMaxMin);
	ASSERT(m_pWorkArea);
	m_pWorkArea->InitWork();
}

/*
 desc : Release the work job
 parm : None
 retn : None
*/
VOID CDlgArea::ReleaseWork()
{
	if (m_pWorkArea)
	{
		m_pWorkArea->StopWork();
		delete m_pWorkArea;
		m_pWorkArea	= NULL;
	}
}

/*
 desc : Initializes for LED Measurement Area
 parm : run_mode- [in]  0x00 : Low Speed (Safe Mode), 0x01 : High Speed (Incomplete Mode)
		max_min	- [in]  Max-Min Condition
						If the difference between the maximum and minimum values of the data stored
						in the queue buffer is less than this value, collection is possible
 retn : TRUE or FALSE
*/
BOOL CDlgArea::StartWork(UINT8 run_mode, DOUBLE max_min)
{
	UINT8 u8Valid		= 0x00;
	TCHAR tzOut[4][8]	= { L"Left", L"Right", L"Top", L"Bottom" };
	LPG_CMSI pstMC2		= &uvEng_GetConfig()->mc2_svc;

	if (!CDlgMenu::StartWork(run_mode, max_min))	return FALSE;

	/* Check whether is active mode */
	if (IsActiveMode())
	{
		AfxMessageBox(L"Currently in Active Mode.\r\nDisable Active mode", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* Check whether the set values are valid */
	if (!m_pstRecipe->IsValidArea())
	{
		AfxMessageBox(L"The set value (area) is not valid", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}	
	/* Check if the measurement position is out of range */
	u8Valid	= m_pstRecipe->IsValidAreaRange(pstMC2->min_dist[UINT8(ENG_MMDI::en_stage_x)],
											pstMC2->min_dist[UINT8(ENG_MMDI::en_stage_y)],
											pstMC2->max_dist[UINT8(ENG_MMDI::en_stage_x)],
											pstMC2->max_dist[UINT8(ENG_MMDI::en_stage_y)]);
	if (0x00 != u8Valid)
	{
		TCHAR tzMesg[128]	= {NULL};
		swprintf_s(tzMesg, 128, L"It's out of the measurement range of stage (%s)", tzOut[u8Valid-1]);
		AfxMessageBox(tzMesg, MB_ICONSTOP|MB_TOPMOST);
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
VOID CDlgArea::StopWork()
{
	if (m_pWorkArea)	ReleaseWork();
}

/*
 desc : Whether it's currently being executed.
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgArea::IsWorkBusy()
{
	if (!m_pWorkArea)	return FALSE;
	return m_pWorkArea->IsBusy();
}

/*
 desc : Search the location for LED Power measurement
 parm : None
 retn : None
*/
VOID CDlgArea::DoWork()
{
	/* If the work is idle */
	if (!m_pWorkArea)	return;
	/* Check if everything is done... */
	if (!m_pWorkArea->IsEnded())
	{
		m_pWorkArea->DoWork();
		if (m_pWorkArea->IsUpdatePower())	UpdateWork(0x00);
		if (m_pWorkArea->IsUpdatedCalc())	UpdateWork(0x01);
	}
}

/*
 desc : Rebuild the grid control
 parm : None
 retn : None
*/
VOID CDlgArea::RebuildGrid()
{
	INT16 i, j, k;
	INT16 i16Rows	= (INT16)m_pstRecipe->GetCountRowCol(0x00);
	INT16 i16Cols	= (INT16)m_pstRecipe->GetCountRowCol(0x01);
	UINT32 u32Width	= 0;
	GV_ITEM stGV	= { NULL };

	/* Basic properties of grid controls */
	stGV.mask	= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr= m_clrGridFg;
	stGV.crBkClr= m_clrGridBg;
	stGV.lfFont	= g_lf;
	stGV.nFormat= DT_VCENTER | DT_CENTER | DT_SINGLELINE;

	for (i=0; i<4; i++)
	{
		/* Reset the grid control */
		m_pGridArea[i]->SetRedraw(FALSE);
		m_pGridArea[i]->DeleteNonFixedRows();
		m_pGridArea[i]->SetRedraw(TRUE);

		/* Set the rows and columns of grid control */
		m_pGridArea[i]->SetRedraw(FALSE);

		m_pGridArea[i]->SetRowCount(i16Rows+1);
		m_pGridArea[i]->SetColumnCount(i16Cols+1);

		if (i < 2)	u32Width	= m_u32Width[1];
		else		u32Width	= m_u32Width[2];
		/* Set the title of columns */
		stGV.row	= 0;
		m_pGridArea[i]->SetColumnWidth(0, m_u32Width[0]);
		for (j=1,k=-INT16(i16Cols/2); j<=i16Cols; j++,k++)
		{
			m_pGridArea[i]->SetColumnWidth(j, u32Width);
			stGV.col	= j;
			stGV.strText.Format(L"%.2f", m_pstRecipe->GetPosXY(0x10, k));
			m_pGridArea[i]->SetItem(&stGV);
		}

		/* Set the title of rows */
		stGV.col	= 0;
		m_pGridArea[i]->SetRowHeight(0, 25);
		for (j=1,k=-INT16(i16Rows/2); j<=i16Rows; j++,k++)
		{
			m_pGridArea[i]->SetRowHeight(j, 25);
			stGV.row	= j;
			stGV.strText.Format(L"%.2f", m_pstRecipe->GetPosXY(0x11, k));
			m_pGridArea[i]->SetItem(&stGV);
		}

		m_pGridArea[i]->SetRedraw(TRUE, TRUE);
		m_pGridArea[i]->Invalidate(TRUE);
	}
}

/*
 desc : Get the recipe file
 parm : None
 retn : None
*/
BOOL CDlgArea::GetData()
{
	/* Get the recipe value */
	m_pstRecipe->ph_no			= (UINT8)m_cmb_ctl[0].GetLBTextToNum();		/* Photohead Number (0x01 ~ 0x08) */
	m_pstRecipe->led_no			= (UINT8)m_cmb_ctl[1].GetLBTextToNum();		/* The measured LED Number (0x01 ~ 0x04, en_365nm ~ en_...) */
	m_pstRecipe->mat_rows		= (UINT8)m_edt_int[0].GetTextToNum();		/* Axis-Y (Vert; Row) Movement Count */
	m_pstRecipe->mat_cols		= (UINT8)m_edt_int[1].GetTextToNum();		/* Axis-X (Horz; Column) Movement Count */
	m_pstRecipe->pass_cnt[0]	= (UINT16)m_edt_int[2].GetTextToNum();		/* This is a count to ignore when measuring LEDs */
	m_pstRecipe->valid_cnt[0]	= (UINT16)m_edt_int[3].GetTextToNum();		/* Effective count when measuring LEDs */
	m_pstRecipe->drop_min_cnt[0]= (UINT16)m_edt_int[4].GetTextToNum();		/* The minimum number of discarded values among measured values */
	m_pstRecipe->drop_max_cnt[0]= (UINT16)m_edt_int[5].GetTextToNum();		/* The maximum number of discarded values among measured values */
	m_pstRecipe->amp_idx[0]		= (UINT16)m_edt_int[6].GetTextToNum();		/* LED Power Index (0 ~ 4095) */
	m_pstRecipe->correct		= (DOUBLE)m_edt_flt[2].GetTextToDouble();	/* Illuminance value collected from sensor in idle sate */
	m_pstRecipe->cent_pos_x		= (DOUBLE)m_edt_flt[4].GetTextToDouble();	/*uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);*/		/* measurement start X position (unit: mm) */
	m_pstRecipe->cent_pos_y		= (DOUBLE)m_edt_flt[5].GetTextToDouble();	/*uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);*/		/* measurement start Y position (unit: mm) */
	m_pstRecipe->move_dist_row	= (DOUBLE)m_edt_flt[6].GetTextToDouble();	/* Axis-Y (Vert; Row) Movement interval (unit: mm) */
	m_pstRecipe->move_dist_col	= (DOUBLE)m_edt_flt[7].GetTextToDouble();	/* Axis-X (Horz; Column) Movement interval (unit: mm) */

	if (!m_pstRecipe->IsValidArea())
	{
		AfxMessageBox(L"Invalid value exists [AREA]", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : Update measurement data and controls
 parm : None
 retn : None
*/
VOID CDlgArea::SetData()
{
	if (!m_pstRecipe->IsValidArea())	return;

	/* Update the controls */
	m_cmb_ctl[0].SetCurSel(m_pstRecipe->ph_no-1);				/* Photohead Number (0x01 ~ 0x08) */
	m_cmb_ctl[1].SetCurSel(m_pstRecipe->led_no-1);				/* The measured LED Number (0x01 ~ 0x04, en_365nm ~ en_...) */
	m_edt_int[0].SetTextToNum(m_pstRecipe->mat_rows);			/* Axis-Y (Vert; Row) Movement Count */
	m_edt_int[1].SetTextToNum(m_pstRecipe->mat_cols);			/* Axis-X (Horz; Column) Movement Count */
	m_edt_int[4].SetTextToNum(m_pstRecipe->drop_min_cnt[0]);	/* The minimum number of discarded values among measured values */
	m_edt_int[5].SetTextToNum(m_pstRecipe->drop_max_cnt[0]);	/* The maximum number of discarded values among measured values */
	m_edt_int[2].SetTextToNum(m_pstRecipe->pass_cnt[0]);		/* This is a count to ignore when measuring LEDs */
	m_edt_int[3].SetTextToNum(m_pstRecipe->valid_cnt[0]);		/* Effective count when measuring LEDs */
	m_edt_int[6].SetTextToNum(m_pstRecipe->amp_idx[0]);			/* LED Power Index (0 ~ 4095) */
	m_edt_flt[2].SetTextToNum(m_pstRecipe->correct, 6);			/* Illuminance value collected from sensor in idle sate */
	m_edt_flt[4].SetTextToNum(m_pstRecipe->cent_pos_x, 3);		/* Axis-Y Center Position (unit: mm) */
	m_edt_flt[5].SetTextToNum(m_pstRecipe->cent_pos_y, 3);		/* Axis-X Center Position (unit: mm) */
	m_edt_flt[6].SetTextToNum(m_pstRecipe->move_dist_row, 4);	/* Axis-Y (Vert; Row) Movement interval (unit: mm) */
	m_edt_flt[7].SetTextToNum(m_pstRecipe->move_dist_col, 4);	/* Axis-X (Horz; Column) Movement interval (unit: mm) */

	/* Rebuild the grid control */
	RebuildGrid();
}

/*
 desc : Updates the currently measured illuminance value
 parm : None
 retn : None
*/
VOID CDlgArea::UpdateRealTimeIllum()
{
	TCHAR tzMeas[32]= {NULL};
	INT32 i32Size	= 0;
	DOUBLE dbPower	= 0.0f, dbLast = 0.0f;	/* Unit : mW */
	vector <DOUBLE>::iterator itPwr;

	/* Get the current power of illuminance */
	LPG_PGDV pstGet	= uvEng_PM100D_GetValue();
	if (!pstGet)	return;

	/* Apply the magnification value of the illuminance filter */
	dbPower	= pstGet->average * 1000.0f;			/* unit: mW */
	dbLast	= uvEng_PM100D_GetCurPower() * 1000.0f;	/* unit: mW */
	swprintf_s(tzMeas, 32, L"%.7f", dbPower);	m_edt_flt[2].SetTextToStr(tzMeas);
	swprintf_s(tzMeas, 32, L"%.7f", dbLast);	m_edt_flt[3].SetTextToStr(tzMeas);

	/* Output the average value */
	if (m_chk_ctl[2].GetCheck() == 1)	m_edt_flt[10].SetTextToNum(dbPower, 7);	/* Naked (Orginal) */
	else								m_edt_flt[11].SetTextToNum(dbPower, 7);	/* Install (Filtered) */
#if 1
	/* Stores real-time measurement values as history */
	if (m_chk_ctl[0].GetCheck())
	{
		CHAR szData[64]	= {NULL}, szFile[_MAX_PATH] = {NULL};
		errno_t eRet	= 0;
		FILE *fpIllum	= NULL;
		SYSTEMTIME stTm	= {NULL};
		CUniToChar csCnv;
		/* Get the current time */
		GetLocalTime(&stTm);
		/* Createa file for storing data */
		sprintf_s(szFile, _MAX_PATH, "%s\\illu\\real.csv", csCnv.Uni2Ansi(g_tzWorkDir));
		eRet = fopen_s(&fpIllum, szFile, "at+");
		if (0 == eRet)
		{
			sprintf_s(szData, 64, "%04u-%02u-%02u %02u:%02u:%02u,%.7f,%.7f\n",
					  stTm.wYear, stTm.wMonth, stTm.wDay, stTm.wHour, stTm.wMinute, stTm.wSecond,
					  dbPower, dbLast);
			/* Move to end of file */
			fseek(fpIllum, 0, SEEK_END);
			/* Saves the measurement value */
			fputs(szData, /*sizeof(CHAR), strlen(szData), */fpIllum);
			/* Close the file */
			fclose(fpIllum);
		}
	}
#endif
}

/*
 desc : Update the measurement information in the grid control
 parm : mode	- [in]  0x00 : Real-time data, 0x01 : Calculated data
 retn : None
*/
VOID CDlgArea::UpdateWork(UINT8 mode)
{
	UINT16 u16Ignore	= m_pWorkArea->GetIgnoreCount();
	UINT16 u16Valid		= m_pWorkArea->GetValidCount();
	BOOL bEnded			= FALSE;
	DOUBLE dbPower[2]	= {DBL_MAX, DBL_MIN};	/* 0x00 : Max - Min, 0x01 : Average (unit: mW) */
	GV_ITEM stGV		= {NULL};
	CString strVal;

	/* Basic properties of gird controls */
	stGV.mask	= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr= RGB(16, 16, 16);
	stGV.crBkClr= RGB(255, 255, 255);
	stGV.lfFont	= g_lf;
	stGV.row	= m_pWorkArea->GetMoveY();
	stGV.col	= m_pWorkArea->GetMoveX();

	if (0x00 == mode)
	{
		/* Current Power */
		stGV.nFormat= DT_VCENTER | DT_RIGHT | DT_SINGLELINE;
		stGV.strText.Format(L"%.5f", m_pWorkArea->GetLastPower() * 1000.0f);
		m_pGridArea[0]->SetItem(&stGV);
		m_pGridArea[0]->InvalidateRowCol(stGV.row, stGV.col);
		/* Ignore / Valid Count */
		stGV.nFormat= DT_VCENTER | DT_CENTER | DT_SINGLELINE;
		stGV.strText.Format(L"%3u,%3u", u16Ignore, u16Valid);
		m_pGridArea[2]->SetItem(&stGV);
		m_pGridArea[2]->InvalidateRowCol(stGV.row, stGV.col);
	}
	else
	{
		/* Max - Min (mW) */
		stGV.nFormat= DT_VCENTER | DT_RIGHT | DT_SINGLELINE;
		dbPower[0]	= m_pWorkArea->GetMaxMinDiff() * 1000.0f;
		stGV.strText.Format(L"%.5f", dbPower[0]);
		m_pGridArea[3]->SetItem(&stGV);
		m_pGridArea[3]->InvalidateRowCol(stGV.row, stGV.col);
		/* Average (mW) */
		dbPower[1]	= m_pWorkArea->GetAverage() * 1000.0f;
		stGV.strText.Format(L"%.5f", dbPower[1]);
		m_pGridArea[1]->SetItem(&stGV);
		m_pGridArea[1]->InvalidateRowCol(stGV.row, stGV.col);
		if (m_stMaxPwr[0].row > 0)
		{
			/* The different (Max - Min) Power (Set the Minimum Power) */
			if (m_stMaxPwr[0].power > dbPower[0])
			{
				/* Resets the previous minimum value */
				m_pGridArea[3]->SetItemFgColour(m_stMaxPwr[0].row, m_stMaxPwr[0].col, m_clrGridFg);
				m_pGridArea[3]->InvalidateRowCol(m_stMaxPwr[0].row, m_stMaxPwr[0].col);
				/* Updates the new minimum value */
				m_stMaxPwr[0].row	= stGV.row;
				m_stMaxPwr[0].col	= stGV.col;
				m_stMaxPwr[0].power	= dbPower[0];
				m_pGridArea[3]->SetItemFgColour(m_stMaxPwr[0].row, m_stMaxPwr[0].col, RGB(255, 0, 0));
				m_pGridArea[3]->InvalidateRowCol(m_stMaxPwr[0].row, m_stMaxPwr[0].col);
			}
			/* The average power (Set the Maximum Power) */
			if (m_stMaxPwr[1].power < dbPower[1])
			{
				/* Resets the previous maximum value */
				m_pGridArea[1]->SetItemFgColour(m_stMaxPwr[1].row, m_stMaxPwr[1].col, m_clrGridFg);
				m_pGridArea[1]->InvalidateRowCol(m_stMaxPwr[1].row, m_stMaxPwr[1].col);
				/* Updates the new maximum value */
				m_stMaxPwr[1].row	= stGV.row;
				m_stMaxPwr[1].col	= stGV.col;
				m_stMaxPwr[1].power	= dbPower[1];
				m_pGridArea[1]->SetItemFgColour(m_stMaxPwr[1].row, m_stMaxPwr[1].col, RGB(255, 0, 0));
				m_pGridArea[1]->InvalidateRowCol(m_stMaxPwr[1].row, m_stMaxPwr[1].col);
			}
		}
		else
		{
			/* The different (Max - Min) Power */
			m_stMaxPwr[0].row	= stGV.row;
			m_stMaxPwr[0].col	= stGV.col;
			m_stMaxPwr[0].power	= dbPower[0];
			m_pGridArea[3]->SetItemFgColour(m_stMaxPwr[0].row, m_stMaxPwr[0].col, RGB(255, 0, 0));
			m_pGridArea[3]->InvalidateRowCol(m_stMaxPwr[0].row, m_stMaxPwr[0].col);
			/* The average power */
			m_stMaxPwr[1].row	= stGV.row;
			m_stMaxPwr[1].col	= stGV.col;
			m_stMaxPwr[1].power	= dbPower[1];
			m_pGridArea[1]->SetItemFgColour(m_stMaxPwr[1].row, m_stMaxPwr[1].col, RGB(255, 0, 0));
			m_pGridArea[1]->InvalidateRowCol(m_stMaxPwr[1].row, m_stMaxPwr[1].col);
		}
	}
}

/*
 desc : OnNotifyGridSelechanged Event
 parm : nmhdr	- [in] the handle and ID of the control sending the message and the notification code
		result	- [in]  A pointer to the LRESULT variable to store the result code if the message has been handled.
 retn : None
*/
VOID CDlgArea::OnGridColumnClicked(NMHDR *nmhdr, LRESULT *result)
{
	INT32 i32GridID		= 0;
	NM_GRIDVIEW *pItem	= (NM_GRIDVIEW*)nmhdr;
	CString strPosX, strPosY;
	if (!pItem)	return;

	i32GridID	= UINT32(pItem->hdr.idFrom) - IDC_GRID_AREA_PWR;
	if (i32GridID < 0)	return;

	strPosX.Empty();
	strPosY.Empty();
	if (pItem->iRow < 0 || pItem->iColumn < 0)
	{
		m_pGridArea[i32GridID]->SetSelectedRange(-1, -1, -1, -1);
		m_pGridArea[i32GridID]->SetFocusCell(-1, -1);
		m_pGridArea[i32GridID]->Invalidate();
		if (i32GridID == 1)
		{
			strPosX.Format(L"%.2f", m_pstRecipe->cent_pos_x);
			strPosY.Format(L"%.2f", m_pstRecipe->cent_pos_y);
		}
	}
	/* In the average's grid control */
	else if (i32GridID == 1)
	{
		/* Get the center position (X / Y) */
		strPosX	= m_pGridArea[1]->GetItemText(0, pItem->iColumn);
		strPosY	= m_pGridArea[1]->GetItemText(pItem->iRow, 0);
	}

	/* Update the value to control windows */
	CString strPwr	= m_pGridArea[1]->GetItemText(pItem->iRow, pItem->iColumn);
	if (i32GridID == 1 && strPwr.GetLength() > 0)
	{
		m_edt_flt[4].SetTextToStr(strPosX.GetBuffer());
		m_edt_flt[5].SetTextToStr(strPosY.GetBuffer());
	}
}

/*
 desc : Save the grid's data to a file
 parm : None
 retn : None
*/
VOID CDlgArea::SaveGrid()
{
	PTCHAR ptzFile	= GetGridFile(L"area");
	if (ptzFile && m_pGridArea[1]->Save(ptzFile))	/* Average */
	{
		AfxMessageBox(L"Succeeded in saving the file [AREA]", MB_OK);
	}
}

/*
 desc : Get the photohead number
 parm : None
 retn : 0x01 ~ 0x08 (succ) or 0x00 (fail)
*/
UINT8 CDlgArea::GetPhNo()
{
	INT64 i64PhNo	= m_cmb_ctl[0].GetLBTextToNum();
	if (i64PhNo < 1)	return 0x00;
	return UINT8(i64PhNo);
}

/*
 desc : Whether in active mode
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgArea::IsActiveMode()
{
	return m_chk_ctl[0].GetCheck() == 1 ? TRUE : FALSE;
}

/*
 desc : Get the current step name
 parm : None
 retn : Step Name or L""
*/
PTCHAR CDlgArea::GetStepName()
{
	if (!m_pWorkArea)	return L"";
	return m_pWorkArea->GetStepName();
}

/*
 desc : Calculate the filter rate
 parm : None
 retn : None
*/
VOID CDlgArea::UpdateFilterRate()
{
	DOUBLE dbRate	= m_edt_flt[10].GetTextToDouble() /* Naked */ / m_edt_flt[11].GetTextToDouble() /* Install */;
	m_edt_flt[12].SetTextToNum(dbRate, 6);
}