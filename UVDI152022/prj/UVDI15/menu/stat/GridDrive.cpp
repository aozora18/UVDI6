/*
 desc : Motion Controller Status
 */

#include "pch.h"
#include "../../MainApp.h"
#include "GridDrive.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                       Base Grid Control                                       */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : parent	- [in]  자신을 호출한 부모 핸들
		grid	- [in]  Grid Control이 출력될 윈도 핸들
		g_id	- [in]  Grid Control ID
		rows	- [in]  행 (Item)의 개수
		cols	- [in]  열 (Item)의 개수
 retn : None
*/
CGridDrive::CGridDrive(HWND parent, HWND grid, UINT32 g_id, UINT32 rows, UINT32 cols)
{
	m_pGrid			= NULL;
	m_pParent		= CWnd::FromHandle(parent);
	m_pCtrl			= CWnd::FromHandle(grid);
	m_u32GridID		= g_id;

	/* 그리드 컨트롤 기본 공통 속성 */
	m_u32Style		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	m_stGV.mask		= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	m_stGV.nFormat	= DT_LEFT | DT_VCENTER | DT_SINGLELINE;
	m_stGV.crFgClr	= RGB(32, 32, 32);
	m_stGV.crBkClr	= RGB(240, 240, 240);
	m_stGV.lfFont = g_lf[eFONT_LEVEL2_BOLD];

	/* Grid Control의 왼쪽에 출력될 항목이 저장될 버퍼 생성 */
	m_u32Rows		= rows;
	m_u32Cols		= cols;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CGridDrive::~CGridDrive()
{
	if (m_pGrid)
	{
		if (m_pGrid->GetSafeHwnd())	m_pGrid->DestroyWindow();
		delete m_pGrid;
	}
}

/*
 desc : 초기화 (생성)
 parm : vert	- [in]  수직 스크롤 생성 여부
 retn : TRUE or FALSE
*/
BOOL CGridDrive::Init(BOOL vert)
{
	/* 현재 윈도 Client 크기 */
	m_pCtrl->GetWindowRect(m_rGrid);
	m_pParent->ScreenToClient(m_rGrid);

	CResizeUI clsResizeUI;
	/* Grid 크기 재설정 */
	m_rGrid.left	+= (long)(15 * clsResizeUI.GetRateX());
	m_rGrid.top		+= (long)(23 * clsResizeUI.GetRateY());
	m_rGrid.right	-= (long)(15 * clsResizeUI.GetRateX());
	m_rGrid.bottom	-= (long)(15 * clsResizeUI.GetRateY());

	/* 객체 생성 */
	m_pGrid = new CGridCtrl;
	ASSERT(m_pGrid);
	m_pGrid->SetDrawScrollBarVert(vert);
	m_pGrid->SetDrawScrollBarHorz(FALSE);
	if (!m_pGrid->Create(m_rGrid, m_pParent, m_u32GridID, m_u32Style))
	{
		delete m_pGrid;
		return FALSE;
	}

	/* 객체 기본 설정 */
	m_pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	m_pGrid->SetRowCount(m_u32Rows);
	m_pGrid->SetColumnCount(m_u32Cols);
	//m_pGrid->SetFixedRowCount(1);
	m_pGrid->SetBkColor(RGB(255, 255, 255));

	/* 본문 */
	m_pGrid->SetColumnWidth(0, m_rGrid.Width()-20);

	/* 기본 속성 및 갱신 */
	m_pGrid->SetBaseGridProp(0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGrid->Refresh();

	/* 거버 레시피 적재 */
	if (m_u32Rows > 0 && m_u32Cols > 0)
	{
		Load();
		Update();
	}

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                       MC2 Drive Status                                        */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : MC2 Drive Status
 parm : None
 retn : None
*/
VOID CGridDriveStat::Load()
{
	TCHAR tzCols[11][16]= { L"drive", L"done", L"cali", L"pwr_on", L"err",
							L"busy", L"zero", L"inpos", L"velo", L"init", L"error" };
	TCHAR tzRows[4][8]	= { L"Stage X", L"Stage Y", L"ACAM 1", L"ACAM 2" };
	INT32 i32Width[11]	= { 60, 51, 51, 51, 51, 53, 53, 53, 53, 53, 55 };
	UINT32 i;

	// by sysandj : Grid Size 맞춤
	int nWidth = 0;
	int nResizeWidth = 0;
	for (int i = 0; i < _countof(i32Width); i++)
	{
		nWidth += i32Width[i];
	}

	for (int i = 0; i < _countof(i32Width); i++)
	{
		i32Width[i] += (m_rGrid.Width() - nWidth) / _countof(i32Width);
		nResizeWidth += i32Width[i];
	}
	i32Width[0] += m_rGrid.Width() - nResizeWidth - 2;

	m_rGrid.bottom += (m_u32Rows * 25) - m_rGrid.Height();
	m_pGrid->MoveWindow(m_rGrid);
	// by sysandj : Grid Size 맞춤

	/* 화면 갱신 비활성화 */
	m_pGrid->SetRedraw(FALSE);

	/* 타이틀 (첫 번째 행) */
	m_stGV.nFormat	= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	m_stGV.crBkClr	= RGB(214, 214, 214);
	m_stGV.crFgClr	= RGB(0, 0, 0);
	for (i=0; i<m_u32Cols; i++)
	{
		m_stGV.row = 0;
		m_stGV.col = i;
		m_stGV.strText = tzCols[i];
		m_pGrid->SetItem(&m_stGV);
		m_pGrid->SetColumnWidth(i, i32Width[i]);
		// by sysandj
		m_pGrid->SetItemBkColour(0, i, DEF_COLOR_BTN_MENU_NORMAL);
		m_pGrid->SetItemFgColour(0, i, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
		// by sysandj
		
// 		m_pGrid->SetItemFont(0, i, &g_lf);
	}

	/* 타이틀 (첫 번째 열) */
	m_stGV.nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
	m_stGV.crBkClr = RGB(214, 214, 214);
	m_stGV.crFgClr = RGB(0, 0, 0);

	m_pGrid->SetRowHeight(0, 25);
	for (i=1; i<m_u32Rows; i++)
	{
		m_stGV.row = i;
		m_stGV.col = 0;
		m_stGV.strText = tzRows[i-1];
		m_pGrid->SetItem(&m_stGV);
		m_pGrid->SetRowHeight(i, 25);
// 		m_pGrid->SetItemFont(i, 0, &g_lf);

		// by sysandj
		m_pGrid->SetItemBkColour(i, 0, DEF_COLOR_BTN_MENU_NORMAL);
		m_pGrid->SetItemFgColour(i, 0, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
		// by sysandj
	}
	/* 화면 갱신 활성화 */
	m_pGrid->SetRedraw(TRUE);
	/* 그리드 컨트롤 갱신 */
	m_pGrid->Invalidate();
}

/*
 desc : 상태 값 갱신
 parm : None
 retn : 선택된 정보가 저장된 구조체 포인터
*/
VOID CGridDriveStat::Update()
{
	TCHAR tzVal[32]		= {NULL};
	UINT32 i = 0, j		= 0;
	COLORREF clrVal[2]	= { RGB (0, 0, 0), RGB (255, 0, 0) };
	LPG_MDSM pstVal		= uvEng_ShMem_GetMC2();
	LPG_AVCD pstAct		= NULL;

	m_stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	m_stGV.crBkClr = RGB(255, 255, 255);
	m_stGV.crFgClr = RGB(0, 0, 0);

	/* 화면 갱신 비활성화 */
	m_pGrid->SetRedraw(FALSE);
	for (i=0; i<uvEng_GetConfig()->mc2_svc.drive_count; i++)
	{
		/* 현재 드라이버의 상태 값 얻기 */
		pstAct	= pstVal->GetDriveAct(uvEng_GetConfig()->mc2_svc.axis_id[i]);
		
		/* 출력되는 행 및 에러 여부에 따라 배경색 설정 */
		m_stGV.row		= i+1;
		m_stGV.crFgClr	= clrVal[pstAct->flag_err];

		m_stGV.col= 1;	m_stGV.strText.Format(L"%u",	pstAct->flag_done);		m_pGrid->SetItem(&m_stGV);
		m_stGV.col++;	m_stGV.strText.Format(L"%u",	pstAct->flag_cal);		m_pGrid->SetItem(&m_stGV);
		m_stGV.col++;	m_stGV.strText.Format(L"%u",	pstAct->flag_on);		m_pGrid->SetItem(&m_stGV);
		m_stGV.col++;	m_stGV.strText.Format(L"%u",	pstAct->flag_err);		m_pGrid->SetItem(&m_stGV);
		m_stGV.col++;	m_stGV.strText.Format(L"%u",	pstAct->flag_msg);		m_pGrid->SetItem(&m_stGV);
		m_stGV.col++;	m_stGV.strText.Format(L"%u",	pstAct->flag_busy);		m_pGrid->SetItem(&m_stGV);
		m_stGV.col++;	m_stGV.strText.Format(L"%u",	pstAct->flag_zero);		m_pGrid->SetItem(&m_stGV);
		m_stGV.col++;	m_stGV.strText.Format(L"%u",	pstAct->flag_inpos);	m_pGrid->SetItem(&m_stGV);
		m_stGV.col++;	m_stGV.strText.Format(L"%u",	pstAct->flag_init);		m_pGrid->SetItem(&m_stGV);
		m_stGV.col++;	m_stGV.strText.Format(L"0x%04x",pstAct->error);			m_pGrid->SetItem(&m_stGV);
	}
	/* 화면 갱신 활성화 */
	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate();
}

/* --------------------------------------------------------------------------------------------- */
/*                                     SD2 Drive Error Message                                   */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : SD2 Drive Error Code Message
 parm : None
 retn : None
*/
VOID CGridSD2Error::Load()
{
	TCHAR tzCols[2][32]	= { L"Code", L"Error Message" };
	INT32 i32Width[2]	= { 50, 515 };
	UINT32 i = 0;

	// by sysandj : Grid Size 맞춤
	int nWidth = 0;
	int nResizeWidth = 0;
	for (int i = 0; i < _countof(i32Width); i++)
	{
		nWidth += i32Width[i];
	}

	for (int i = 0; i < _countof(i32Width); i++)
	{
		i32Width[i] += (m_rGrid.Width() - ::GetSystemMetrics(SM_CXVSCROLL) - nWidth) / _countof(i32Width);
		nResizeWidth += i32Width[i];
	}
	i32Width[0] += m_rGrid.Width() - ::GetSystemMetrics(SM_CXVSCROLL) - nResizeWidth - 2;

	//m_rGrid.bottom += (m_u32Rows * 25) - m_rGrid.Height();
	//m_pGrid->MoveWindow(m_rGrid);
	// by sysandj : Grid Size 맞춤

	/* 화면 갱신 비활성화 */
	m_pGrid->SetRedraw(FALSE);

	/* 타이틀 (첫 번째 행) */
	m_stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	m_stGV.crBkClr = RGB(214, 214, 214);
	m_stGV.crFgClr = RGB(0, 0, 0);
	for (i=0; i<m_u32Cols; i++)
	{
		m_stGV.row = 0;
		m_stGV.col = i;
		m_stGV.strText = tzCols[i];
		m_pGrid->SetItem(&m_stGV);
		m_pGrid->SetColumnWidth(i, i32Width[i]);

		// by sysandj
		m_pGrid->SetItemBkColour(0, i, DEF_COLOR_BTN_MENU_NORMAL);
		m_pGrid->SetItemFgColour(0, i, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
		// by sysandj
	}

	/* 타이틀 (첫 번째 열) */
	m_stGV.nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
	m_stGV.crBkClr = RGB(214, 214, 214);
	m_stGV.crFgClr = RGB(0, 0, 0);

	m_pGrid->SetRowHeight(0, 25);
	for (i=1; i<m_u32Rows; i++)
	{
		m_stGV.row = i;
		m_stGV.col = 0;
		m_stGV.strText.Empty();
		m_pGrid->SetItem(&m_stGV);
		m_pGrid->SetRowHeight(i, 25);

		// by sysandj
		m_pGrid->SetItemBkColour(i, 0, DEF_COLOR_BTN_MENU_NORMAL);
		m_pGrid->SetItemFgColour(i, 0, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
		// by sysandj
	}
	/* 화면 갱신 활성화 */
	m_pGrid->SetRedraw(TRUE, TRUE);
	/* 그리드 컨트롤 갱신 */
	m_pGrid->Invalidate();
}

/*
 desc : 상태 값 갱신
 parm : None
 retn : 선택된 정보가 저장된 구조체 포인터
*/
VOID CGridSD2Error::Update()
{
	UINT32 i	= 1;

	/* 화면 갱신 비활성화 */
	m_pGrid->SetRedraw(FALSE);

	m_stGV.nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
	m_stGV.crBkClr = RGB(255, 255, 255);
	m_stGV.crFgClr = RGB(0, 0, 0);

	m_pGrid->SetItemText(i, 0, L"0x0003");	m_pGrid->SetItemText(i, 1, L"Interpolation error (interpolated position control)");		i++;
	m_pGrid->SetItemText(i, 0, L"0x0005");	m_pGrid->SetItemText(i, 1, L"Error caused by warning");									i++;
	m_pGrid->SetItemText(i, 0, L"0x0006");	m_pGrid->SetItemText(i, 1, L"Digital Input 'External Hardware'");						i++;
	m_pGrid->SetItemText(i, 0, L"0x0007");	m_pGrid->SetItemText(i, 1, L"Error in internal hardware (FC2)");						i++;
	m_pGrid->SetItemText(i, 0, L"0x0009");	m_pGrid->SetItemText(i, 1, L"Hiperface / EnDat OEM data incorrect");					i++;
	m_pGrid->SetItemText(i, 0, L"0x000A");	m_pGrid->SetItemText(i, 1, L"drive-setup-tool heartbeat");								i++;
	m_pGrid->SetItemText(i, 0, L"0x000B");	m_pGrid->SetItemText(i, 1, L"Communication / bus system error");						i++;
	m_pGrid->SetItemText(i, 0, L"0x000C");	m_pGrid->SetItemText(i, 1, L"Mains 'Ready for operation' is missing");					i++;
	m_pGrid->SetItemText(i, 0, L"0x000F");	m_pGrid->SetItemText(i, 1, L"Endat / Hiperface communication faulty");					i++;
	m_pGrid->SetItemText(i, 0, L"0x0011");	m_pGrid->SetItemText(i, 1, L"FPGA power output stage shut‐ down");						i++;
	m_pGrid->SetItemText(i, 0, L"0x0012");	m_pGrid->SetItemText(i, 1, L"Error in spindle selection");								i++;
	m_pGrid->SetItemText(i, 0, L"0x0019");	m_pGrid->SetItemText(i, 1, L"Power supply load too high");								i++;
	m_pGrid->SetItemText(i, 0, L"0x001A");	m_pGrid->SetItemText(i, 1, L"Motor temperature too high");								i++;
	m_pGrid->SetItemText(i, 0, L"0x001B");	m_pGrid->SetItemText(i, 1, L"Ambient temperature too high");							i++;
	m_pGrid->SetItemText(i, 0, L"0x001C");	m_pGrid->SetItemText(i, 1, L"Power output stage temperature too high");					i++;
	m_pGrid->SetItemText(i, 0, L"0x001D");	m_pGrid->SetItemText(i, 1, L"Motor load too high (Motor I²t)");							i++;
	m_pGrid->SetItemText(i, 0, L"0x001E");	m_pGrid->SetItemText(i, 1, L"Power output stage load too high (I²t)");					i++;
	m_pGrid->SetItemText(i, 0, L"0x001F");	m_pGrid->SetItemText(i, 1, L"Speed error or slip too great");							i++;
	m_pGrid->SetItemText(i, 0, L"0x0021");	m_pGrid->SetItemText(i, 1, L"Power supply load monitoring -> mains voltage too high");	i++;
	m_pGrid->SetItemText(i, 0, L"0x0022");	m_pGrid->SetItemText(i, 1, L"Power supply load monitoring -> mains voltage too low");	i++;
	m_pGrid->SetItemText(i, 0, L"0x0023");	m_pGrid->SetItemText(i, 1, L"Error in external power supply unit");						i++;
	m_pGrid->SetItemText(i, 0, L"0x0024");	m_pGrid->SetItemText(i, 1, L"Encoder 0 monitoring");									i++;
	m_pGrid->SetItemText(i, 0, L"0x0025");	m_pGrid->SetItemText(i, 1, L"Ballast circuit load (I²t ballast resistor)");				i++;
	m_pGrid->SetItemText(i, 0, L"0x0025");	m_pGrid->SetItemText(i, 1, L"DC DC converter overload (only 0362161xy)");				i++;
	m_pGrid->SetItemText(i, 0, L"0x0026");	m_pGrid->SetItemText(i, 1, L"Actual speed value greater than overspeed threshold");		i++;
	m_pGrid->SetItemText(i, 0, L"0x0027");	m_pGrid->SetItemText(i, 1, L"Tracking error monitoring and motor slowdown");			i++;
	m_pGrid->SetItemText(i, 0, L"0x0028");	m_pGrid->SetItemText(i, 1, L"Motor feedback");											i++;
	m_pGrid->SetItemText(i, 0, L"0x0029");	m_pGrid->SetItemText(i, 1, L"Motor phase lost");										i++;
	m_pGrid->SetItemText(i, 0, L"0x002A");	m_pGrid->SetItemText(i, 1, L"Overvoltage in DC link");									i++;
	m_pGrid->SetItemText(i, 0, L"0x002B");	m_pGrid->SetItemText(i, 1, L"Undervoltage in DC link");									i++;
	m_pGrid->SetItemText(i, 0, L"0x002C");	m_pGrid->SetItemText(i, 1, L"Commutation lost");										i++;
	m_pGrid->SetItemText(i, 0, L"0x002D");	m_pGrid->SetItemText(i, 1, L"Short circuit in power output stage");						i++;
	m_pGrid->SetItemText(i, 0, L"0x002E");	m_pGrid->SetItemText(i, 1, L"Safety circuit (Safety X10)");								i++;
	m_pGrid->SetItemText(i, 0, L"0x002F");	m_pGrid->SetItemText(i, 1, L"Drive parameters not activated");							i++;
	m_pGrid->SetItemText(i, 0, L"0x0037");	m_pGrid->SetItemText(i, 1, L"Firmware stopped by ESC");									i++;
	m_pGrid->SetItemText(i, 0, L"0x0038");	m_pGrid->SetItemText(i, 1, L"Device configuration");									i++;
	m_pGrid->SetItemText(i, 0, L"0x0039");	m_pGrid->SetItemText(i, 1, L"Faulty or no firmware");									i++;
	m_pGrid->SetItemText(i, 0, L"0x003A");	m_pGrid->SetItemText(i, 1, L"FPGA watchdog triggered");									i++;
	m_pGrid->SetItemText(i, 0, L"0x003B");	m_pGrid->SetItemText(i, 1, L"No drive parameters loaded");								i++;
	m_pGrid->SetItemText(i, 0, L"0x003C");	m_pGrid->SetItemText(i, 1, L"Drive parameters incorrect");								i++;
	m_pGrid->SetItemText(i, 0, L"0x003D");	m_pGrid->SetItemText(i, 1, L"Logic coding missing or incorrect");						i++;
	m_pGrid->SetItemText(i, 0, L"0x003E");	m_pGrid->SetItemText(i, 1, L"Error in electronic type plate");							i++;
	m_pGrid->SetItemText(i, 0, L"0x0043");	m_pGrid->SetItemText(i, 1, L"Software Quick Stop");										i++;
	m_pGrid->SetItemText(i, 0, L"0x0044");	m_pGrid->SetItemText(i, 1, L"Input Quick Stop");										i++;
	m_pGrid->SetItemText(i, 0, L"0x0047");	m_pGrid->SetItemText(i, 1, L"Negative Position Limit software");						i++;
	m_pGrid->SetItemText(i, 0, L"0x0048");	m_pGrid->SetItemText(i, 1, L"Positive Position Limit software");						i++;
	m_pGrid->SetItemText(i, 0, L"0x004B");	m_pGrid->SetItemText(i, 1, L"Negative Position Limit switch");							i++;
	m_pGrid->SetItemText(i, 0, L"0x004C");	m_pGrid->SetItemText(i, 1, L"Positive Position Limit switch");							i++;

	/* 화면 갱신 활성화 */
	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate();
}

/* --------------------------------------------------------------------------------------------- */
/*                                     MC2 Drive Error Message                                   */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : MC2 Drive Error Code Message
 parm : None
 retn : None
*/
VOID CGridMC2Error::Load()
{
	TCHAR tzCols[2][32]	= { L"Code", L"Error Message" };
	INT32 i32Width[2]	= { 50, 515 };
	UINT32 i = 0;

	// by sysandj : Grid Size 맞춤
	int nWidth = 0;
	int nResizeWidth = 0;
	for (int i = 0; i < _countof(i32Width); i++)
	{
		nWidth += i32Width[i];
	}

	for (int i = 0; i < _countof(i32Width); i++)
	{
		i32Width[i] += (m_rGrid.Width() - ::GetSystemMetrics(SM_CXVSCROLL) - nWidth) / _countof(i32Width);
		nResizeWidth += i32Width[i];
	}
	i32Width[0] += m_rGrid.Width() - ::GetSystemMetrics(SM_CXVSCROLL) - nResizeWidth - 2;

	//m_rGrid.bottom += (m_u32Rows * 25) - m_rGrid.Height();
	//m_pGrid->MoveWindow(m_rGrid);
	// by sysandj : Grid Size 맞춤

	/* 화면 갱신 비활성화 */
	m_pGrid->SetRedraw(FALSE);

	/* 타이틀 (첫 번째 행) */
	m_stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	m_stGV.crBkClr = RGB(214, 214, 214);
	m_stGV.crFgClr = RGB(0, 0, 0);
	for (i=0; i<m_u32Cols; i++)
	{
		m_stGV.row = 0;
		m_stGV.col = i;
		m_stGV.strText = tzCols[i];
		m_pGrid->SetItem(&m_stGV);
		m_pGrid->SetColumnWidth(i, i32Width[i]);

		// by sysandj
		m_pGrid->SetItemBkColour(0, i, DEF_COLOR_BTN_MENU_NORMAL);
		m_pGrid->SetItemFgColour(0, i, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
		// by sysandj
	}

	/* 타이틀 (첫 번째 열) */
	m_stGV.nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
	m_stGV.crBkClr = RGB(214, 214, 214);
	m_stGV.crFgClr = RGB(0, 0, 0);

	m_pGrid->SetRowHeight(0, 25);
	for (i=1; i<m_u32Rows; i++)
	{
		m_stGV.row = i;
		m_stGV.col = 0;
		m_stGV.strText.Empty();
		m_pGrid->SetItem(&m_stGV);
		m_pGrid->SetRowHeight(i, 25);

		// by sysandj
		m_pGrid->SetItemBkColour(i, 0, DEF_COLOR_BTN_MENU_NORMAL);
		m_pGrid->SetItemFgColour(i, 0, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
		// by sysandj
	}
	/* 화면 갱신 활성화 */
	m_pGrid->SetRedraw(TRUE, TRUE);
	/* 그리드 컨트롤 갱신 */
	m_pGrid->Invalidate();
}

/*
 desc : 상태 값 갱신
 parm : None
 retn : 선택된 정보가 저장된 구조체 포인터
*/
VOID CGridMC2Error::Update()
{
	UINT32 i	= 1;

	/* 화면 갱신 비활성화 */
	m_pGrid->SetRedraw(FALSE);

	m_stGV.nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
	m_stGV.crBkClr = RGB(255, 255, 255);
	m_stGV.crFgClr = RGB(0, 0, 0);

	m_pGrid->SetItemText(i, 0, L"0x010a");	m_pGrid->SetItemText(i, 1, L"Wrong Parameter Value");		i++;
	m_pGrid->SetItemText(i, 0, L"0x010b");	m_pGrid->SetItemText(i, 1, L"Object Not Found  ");			i++;
	m_pGrid->SetItemText(i, 0, L"0x010c");	m_pGrid->SetItemText(i, 1, L"Wrong Object Size");			i++;
	m_pGrid->SetItemText(i, 0, L"0x0115");	m_pGrid->SetItemText(i, 1, L"Wrong Command ");				i++;
	m_pGrid->SetItemText(i, 0, L"0x0117");	m_pGrid->SetItemText(i, 1, L"Command Cancel");				i++;
	m_pGrid->SetItemText(i, 0, L"0x0118");	m_pGrid->SetItemText(i, 1, L"Machine Stop  ");				i++;
	m_pGrid->SetItemText(i, 0, L"0x0140");	m_pGrid->SetItemText(i, 1, L"Not Calibrated");				i++;
	m_pGrid->SetItemText(i, 0, L"0x0141");	m_pGrid->SetItemText(i, 1, L"Profile Error");				i++;
	m_pGrid->SetItemText(i, 0, L"0x0143");	m_pGrid->SetItemText(i, 1, L"SG Cmd Busy or slave busy ");	i++;
	m_pGrid->SetItemText(i, 0, L"0x0144");	m_pGrid->SetItemText(i, 1, L"Not Operation Enabled");		i++;
	m_pGrid->SetItemText(i, 0, L"0x0145");	m_pGrid->SetItemText(i, 1, L"Illegal Cal Method");			i++;
	m_pGrid->SetItemText(i, 0, L"0x0146");	m_pGrid->SetItemText(i, 1, L"Illegal Drive No");			i++;
	m_pGrid->SetItemText(i, 0, L"0x0148");	m_pGrid->SetItemText(i, 1, L"Not Ready To Switch On");		i++;
	m_pGrid->SetItemText(i, 0, L"0x014b");	m_pGrid->SetItemText(i, 1, L"Velocity Out Of Limits");		i++;
	m_pGrid->SetItemText(i, 0, L"0x014d");	m_pGrid->SetItemText(i, 1, L"Change On The Fly");			i++;
	m_pGrid->SetItemText(i, 0, L"0x014e");	m_pGrid->SetItemText(i, 1, L"Not In Gantry Mode");			i++;
	m_pGrid->SetItemText(i, 0, L"0x014f");	m_pGrid->SetItemText(i, 1, L"Wrong Master");				i++;
	m_pGrid->SetItemText(i, 0, L"0x0150");	m_pGrid->SetItemText(i, 1, L"Emergency Stop");				i++;
	m_pGrid->SetItemText(i, 0, L"0x0151");	m_pGrid->SetItemText(i, 1, L"Limit Switch Positive");		i++;
	m_pGrid->SetItemText(i, 0, L"0x0151");	m_pGrid->SetItemText(i, 1, L"Limit Switch Negative");		i++;
	m_pGrid->SetItemText(i, 0, L"0x0153");	m_pGrid->SetItemText(i, 1, L"Position Out Of Limits");		i++;
	m_pGrid->SetItemText(i, 0, L"0x0154");	m_pGrid->SetItemText(i, 1, L"Parameter Out Of Limits");		i++;
	m_pGrid->SetItemText(i, 0, L"0x015a");	m_pGrid->SetItemText(i, 1, L"Power Stage Off");				i++;
	m_pGrid->SetItemText(i, 0, L"0x015b");	m_pGrid->SetItemText(i, 1, L"Gantry Positions Not Equal");	i++;
	m_pGrid->SetItemText(i, 0, L"0x015c");	m_pGrid->SetItemText(i, 1, L"Not In Hold Mode");			i++;
	m_pGrid->SetItemText(i, 0, L"0x015d");	m_pGrid->SetItemText(i, 1, L"Power Stage On");				i++;
	m_pGrid->SetItemText(i, 0, L"0x015e");	m_pGrid->SetItemText(i, 1, L"Acceleration Out Of Limits");	i++;
	m_pGrid->SetItemText(i, 0, L"0x015f");	m_pGrid->SetItemText(i, 1, L"Jerk Out Of Limits");			i++;
	m_pGrid->SetItemText(i, 0, L"0x0160");	m_pGrid->SetItemText(i, 1, L"Tracking Error Gantry");		i++;

	/* 화면 갱신 활성화 */
	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate();
}
