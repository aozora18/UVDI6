/*
 desc : Mini Motor Grid Control
 */

#include "pch.h"
#include "../../MainApp.h"
#include "GridMiniMotor.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : 생성자
 parm : parent	- [in]  부모 윈도 핸들
		grid	- [in]  Grid가 출력될 윈도 핸들
 retn : None
*/
CGridMiniMotor::CGridMiniMotor(HWND parent, HWND grid)
{
	m_pGrid			= NULL;
	m_pParent		= CWnd::FromHandle(parent);
	m_pCtrl			= CWnd::FromHandle(grid);
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CGridMiniMotor::~CGridMiniMotor()
{
	if (m_pGrid)
	{
		if (m_pGrid->GetSafeHwnd())	m_pGrid->DestroyWindow();
		delete m_pGrid;
		m_pGrid	= NULL;
	}
}

/*
 desc : Grid Control이 공통 초기화
 parm : None
 retn : None
*/
VOID CGridMiniMotor::InitGrid()
{
	TCHAR tzCols[2][16]	= { L"Item", L"Pos (mm)" }, tzRows[32] = {NULL};
	INT32 i32Width[2]	= { 65, 95, }, i, j=1;
	INT32 i32ColCnt		= 2, i32RowCnt = 0;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	LPG_CIEA pstCfg		= uvEng_GetConfig();
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* 현재 윈도 Client 크기 */
	m_pCtrl->GetWindowRect(rGrid);
	m_pParent->ScreenToClient(rGrid);

	CResizeUI clsResizeUI;
	/* Grid 크기 재설정 */
	// by sys&j : 화면 맞춤을 위해 수정
	rGrid.left	+= (long)(185 * clsResizeUI.GetRateX());
	rGrid.top	+= (long)(23  * clsResizeUI.GetRateY());
	rGrid.right	-= (long)(14  * clsResizeUI.GetRateX());
	rGrid.bottom-= (long)(135 * clsResizeUI.GetRateY());
	// by sys&j : 화면 맞춤을 위해 수정
	
	// by sysandj : Grid Size 맞춤
	int nWidth = 0;
	int nResizeWidth = 0;
	for (int i = 0; i < _countof(i32Width); i++)
	{
		nWidth += i32Width[i];
	}

	for (int i = 0; i < _countof(i32Width); i++)
	{
		i32Width[i] += (rGrid.Width() - nWidth - ::GetSystemMetrics(SM_CXVSCROLL)) / _countof(i32Width);
		nResizeWidth += i32Width[i];
	}
	i32Width[0] += rGrid.Width() - ::GetSystemMetrics(SM_CXVSCROLL) - nResizeWidth - 2;
	// by sysandj : Grid Size 맞춤

	/* 총 출력될 행 (Row)의 개수 구하기 */
	i32RowCnt	= pstCfg->luria_svc.ph_count + pstCfg->mc2_svc.drive_count;
	/* Basler 카메라의 경우 Z Axis의 위치 (높이) 값 얻기 위함 */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)pstCfg->set_comn.align_camera_kind)
	{
		/* Motion Left / Right 위치와 Up / Down 높이 위치 */
		i32RowCnt	+= pstCfg->set_cams.acam_count;
	}

	/* 그리드 컨트롤 기본 공통 속성 */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* 객체 생성 */
	m_pGrid = new CGridCtrl;
	ASSERT(m_pGrid);
	m_pGrid->SetDrawScrollBarVert(1);
	m_pGrid->SetDrawScrollBarHorz(0);
	if (!m_pGrid->Create(rGrid, m_pParent, IDC_GRID_CTRL_MOTOR_DRIVE, dwStyle))
	{
		delete m_pGrid;
		m_pGrid	= NULL;
		return;
	}

	/* 객체 기본 설정 */
	m_pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	m_pGrid->SetRowCount(i32RowCnt+1);
	m_pGrid->SetColumnCount(i32ColCnt);
	//m_pGrid->SetFixedRowCount(1);
	m_pGrid->SetRowHeight(0, 24);
	m_pGrid->SetFixedColumnCount(0);
	m_pGrid->SetBkColor(RGB(255, 255, 255));
	m_pGrid->SetFixedColumnSelection(0);

	/* 타이틀 (첫 번째 행) */
	stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	stGV.crBkClr = RGB(214, 214, 214);
	stGV.crFgClr = RGB(0, 0, 0);
	stGV.lfFont = g_lf[eFONT_LEVEL3_BOLD];
	for (i=0; i<i32ColCnt; i++)
	{
		stGV.nFormat= DT_VCENTER | DT_SINGLELINE | DT_CENTER;
		stGV.row	= 0;
		stGV.col	= i;
		stGV.strText= tzCols[i];
		m_pGrid->SetItem(&stGV);
		m_pGrid->SetColumnWidth(i, i32Width[i]);
		// by sysandj
		m_pGrid->SetItemBkColour(0, i, DEF_COLOR_BTN_MENU_NORMAL);
		m_pGrid->SetItemFgColour(0, i, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
		// by sysandj
	}

	/* 본문 배경색 */
	stGV.crBkClr = RGB(255, 255, 255);

	/* 본문 설정 - for Photohead Position */
	for (i=0; i<pstCfg->luria_svc.ph_count; i++,j++)
	{
		m_pGrid->SetRowHeight(j, 24);

		/* for Subject */
		stGV.strText.Format(L"PH %d", i+1);
		stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
		stGV.row	= j;
		stGV.col	= 0;
		m_pGrid->SetItem(&stGV);

		/* for Value */
		stGV.strText= L"0.0000";
		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.col	= 1;
		m_pGrid->SetItem(&stGV);
	}

	/* 본문 설정 - for Motion Drive Position */
	for (i=0; i<pstCfg->mc2_svc.drive_count; i++,j++)
	{
		m_pGrid->SetRowHeight(j, 24);

		/* for Subject */
		stGV.strText.Format(L"MC %d", pstCfg->mc2_svc.axis_id[i]);
		stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
		stGV.row	= j;
		stGV.col	= 0;
		m_pGrid->SetItem(&stGV);

		/* for Value */
		stGV.strText= L"0.0000";
		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.col	= 1;
		m_pGrid->SetItem(&stGV);
	}

	/* 본문 설정 - for Basler Camera Position (Z Axis Up / Down Postion) */
	for (i=0; i<pstCfg->set_cams.acam_count; i++,j++)
	{
		m_pGrid->SetRowHeight(j, 24);

		/* for Subject */
		stGV.strText.Format(L"AZ %d", i+1);
		stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
		stGV.row	= j;
		stGV.col	= 0;
		m_pGrid->SetItem(&stGV);

		/* for Value */
		stGV.strText= L"0.0000";
		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.col	= 1;
		m_pGrid->SetItem(&stGV);
	}

	/* 기본 속성 및 갱신 */
	m_pGrid->SetBaseGridProp(0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGrid->Refresh();
}

/*
 desc : 모터 위치 및 상태 정보 갱신
 parm : None
 retn : None
*/
VOID CGridMiniMotor::UpdateValue()
{
	UINT8 u8DrvID	= 0x00, i, j = 0x01;
	GV_ITEM stGV	= { NULL };
	LPG_MDSM pstMC2	= uvEng_ShMem_GetMC2();
	LPG_CIEA pstCfg	= uvEng_GetConfig();
	LPG_LDSM pstPH	= uvEng_ShMem_GetLuria();

	/* 화면 갱신 비활성화 */
	m_pGrid->SetRedraw(FALSE);

	/* 그리드 컨트롤 기본 공통 속성 */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crBkClr = RGB(255, 255, 255);
	stGV.crFgClr = RGB(0, 0, 0);
	stGV.lfFont = g_lf[eFONT_LEVEL2_BOLD];

	/* 본문 설정 - for Photohead Position */
	for (i=0; i<pstCfg->luria_svc.ph_count; i++,j++)
	{
		/* for Value */
		stGV.strText.Format(L"%.3f", pstPH->directph.focus_motor_move_abs_position[i]/1000.0f);
		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.row	= j;
		stGV.col	= 1;
		m_pGrid->SetItem(&stGV);
	}

	/* 본문 설정 - for Motion Drive Position */
	for (i=0; i<pstCfg->mc2_svc.drive_count; i++,j++)
	{
		/* for Value */
		u8DrvID	= uvEng_GetConfig()->mc2_svc.axis_id[i];
		stGV.strText.Format(L"%.4f", pstMC2->GetDrivePos(u8DrvID));
		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.row	= j;
		stGV.col	= 1;
		m_pGrid->SetItem(&stGV);
	}

	/* 본문 설정 - for Basler Camera Position (Z Axis Up / Down Postion) */
	for (i=0; i<pstCfg->set_cams.acam_count; i++,j++)
	{
		/* for Value */
		//stGV.strText.Format(L"%.4f", uvCmn_MCQ_GetACamCurrentPosZ(i+1));
		double dCamPos = 0; // by sysandj : MCQ대체 추가 필요
		stGV.strText.Format(L"%.4f", dCamPos);
		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.row	= j;
		stGV.col	= 1;
		m_pGrid->SetItem(&stGV);
	}

	/* 화면 갱신 활성화 */
	m_pGrid->SetRedraw(TRUE, TRUE);
	/* 그리드 컨트롤 갱신 */
	m_pGrid->Invalidate();
}
