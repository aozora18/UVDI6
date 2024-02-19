/*
 desc : Gerber Recipe Grid Control
 */

#include "pch.h"
#include "../../MainApp.h"
#include "GridMotor.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : ������
 parm : parent	- [in]  �θ� ���� �ڵ�
		grid	- [in]  Grid�� ��µ� ���� �ڵ�
 retn : None
*/
CGridMotor::CGridMotor(HWND parent, HWND grid)
{
	m_pGrid			= NULL;
	m_pParent		= CWnd::FromHandle(parent);
	m_pCtrl			= CWnd::FromHandle(grid);
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CGridMotor::~CGridMotor()
{
	if (m_pGrid)
	{
		if (m_pGrid->GetSafeHwnd())	m_pGrid->DestroyWindow();
		delete m_pGrid;
		m_pGrid	= NULL;
	}
}

/*
 desc : Grid Control�� ���� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CGridMotor::InitGrid()
{
	TCHAR tzCols[3][16]	= { L"Item", L"State", L"Pos (mm)" };
	//TCHAR tzRows[4][32]	= { L"STAGE X", L"STAGE Y", L"ACam.X1", L"ACAM.X2" };
	TCHAR tzRows[6][32] = { L"STAGE X", L"STAGE Y", L"PH1 Z", L"PH2 Z", L"ACam.X1", L"ACAM.X2" };
	INT32 i32Width[3]	= { 68, 61, 80 }, i, j=1;
	INT32 i32ColCnt		= 3, i32RowCnt = 0;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	LPG_CIEA pstCfg		= uvEng_GetConfig();
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* ���� ���� Client ũ�� */
	m_pCtrl->GetWindowRect(rGrid);
	m_pParent->ScreenToClient(rGrid);

	CResizeUI clsResizeUI;
	/* Grid ũ�� �缳�� */
	rGrid.left	+= (long)(15 * clsResizeUI.GetRateX());
	rGrid.top	+= (long)(22 * clsResizeUI.GetRateY());
	rGrid.right	-= (long)(15 * clsResizeUI.GetRateX());
	rGrid.bottom-= (long)(15 * clsResizeUI.GetRateY());

	// by sysandj : Grid Size ����
	int nWidth = 0;
	int nResizeWidth = 0;
	for (int i = 0; i < _countof(i32Width); i++)
	{
		nWidth += i32Width[i];
	}

	for (int i = 0; i < _countof(i32Width); i++)
	{
		i32Width[i] += (rGrid.Width() - nWidth) / _countof(i32Width);
		nResizeWidth += i32Width[i];
	}
	i32Width[0] += rGrid.Width() - nResizeWidth - 2;
	// by sysandj : Grid Size ����

	/* �� ��µ� �� (Row)�� ���� ���ϱ� */
	i32RowCnt	= pstCfg->luria_svc.ph_count + pstCfg->mc2_svc.drive_count;
	/* Basler ī�޶��� ��� Z Axis�� ��ġ (����) �� ��� ���� */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)pstCfg->set_comn.align_camera_kind)
	{
		/* Motion Left / Right ��ġ�� Up / Down ���� ��ġ */
		i32RowCnt	+= pstCfg->set_cams.acam_count;
	}

	/* �׸��� ��Ʈ�� �⺻ ���� �Ӽ� */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* ��ü ���� */
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

	/* ��ü �⺻ ���� */
	m_pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	m_pGrid->SetRowCount(i32RowCnt+1);
	m_pGrid->SetColumnCount(i32ColCnt);
	// by sysandj
	//m_pGrid->SetFixedRowCount(1);
	m_pGrid->SetRowHeight(0, 24);
	m_pGrid->SetFixedColumnCount(0);
	m_pGrid->SetBkColor(RGB(255, 255, 255));
	m_pGrid->SetFixedColumnSelection(0);

	/* Ÿ��Ʋ (ù ��° ��) */
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

	/* ���� ���� */
	stGV.crBkClr = RGB(255, 255, 255);

	/* ���� ���� - for Motion Drive Position */
	for (i=0; i<pstCfg->mc2_svc.drive_count; i++,j++)
	{
		m_pGrid->SetRowHeight(j, 25);

		/* for Subject */
		stGV.strText= tzRows[i];	/*.Format(L"MC %d", pstCfg->mc2_svc.axis_id[i]);*/
		stGV.nFormat= DT_LEFT | DT_VCENTER | DT_SINGLELINE;
		stGV.row	= j;
		stGV.col	= 0;
		m_pGrid->SetItem(&stGV);

		/* for State */
		stGV.strText= L"Idle";
		stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
		stGV.col	= 1;
		m_pGrid->SetItem(&stGV);

		/* for Value */
		stGV.strText= L"0.0000";
		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.col	= 2;
		m_pGrid->SetItem(&stGV);

		// by sysandj
		m_pGrid->SetItemBkColour(j, 0, DEF_COLOR_BTN_MENU_NORMAL);
		m_pGrid->SetItemFgColour(j, 0, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
		// by sysandj
	}

// 	/* ���� ���� - for Photohead Position */
// 	for (i=0; i<pstCfg->luria_svc.ph_count; i++,j++)
// 	{
// 		m_pGrid->SetRowHeight(j, 25);
// 
// 		/* for Subject */
// 		stGV.strText.Format(L"PH Z%d", i+1);
// 		stGV.nFormat= DT_LEFT | DT_VCENTER | DT_SINGLELINE;
// 		stGV.row	= j;
// 		stGV.col	= 0;
// 		m_pGrid->SetItem(&stGV);
// 
// 		/* for State */
// 		stGV.strText= L"Idle";
// 		stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
// 		stGV.col	= 1;
// 		m_pGrid->SetItem(&stGV);
// 
// 		/* for Value */
// 		stGV.strText= L"0.0000";
// 		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
// 		stGV.col	= 2;
// 		m_pGrid->SetItem(&stGV);
// 
// 		// by sysandj
// 		m_pGrid->SetItemBkColour(j, 0, DEF_COLOR_BTN_MENU_NORMAL);
// 		m_pGrid->SetItemFgColour(j, 0, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
// 		// by sysandj
// 	}
// 
// 	/* ���� ���� - for Basler Camera Position (Z Axis Up / Down Postion) */
// 	for (i=0; i<pstCfg->set_cams.acam_count; i++,j++)
// 	{
// 		m_pGrid->SetRowHeight(j, 25);
// 
// 		/* for Subject */
// 		stGV.strText.Format(L"ACAM Z%d", i+1);
// 		//stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
// 		// by sysandj : Grid ���� ����
// 		stGV.nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
// 		stGV.row	= j;
// 		stGV.col	= 0;
// 		m_pGrid->SetItem(&stGV);
// 
// 		/* for State */
// 		stGV.strText= L"Idle";
// 		stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
// 		stGV.col	= 1;
// 		m_pGrid->SetItem(&stGV);
// 
// 		/* for Value */
// 		stGV.strText= L"0.0000";
// 		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
// 		stGV.col	= 2;
// 		m_pGrid->SetItem(&stGV);
// 
// 		// by sysandj
// 		m_pGrid->SetItemBkColour(j, 0, DEF_COLOR_BTN_MENU_NORMAL);
// 		m_pGrid->SetItemFgColour(j, 0, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
// 		// by sysandj
// 	}

	/* �⺻ �Ӽ� �� ���� */
	m_pGrid->SetBaseGridProp(0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGrid->Refresh();
}

/*
 desc : ���� ��ġ �� ���� ���� ����
 parm : None
 retn : None
*/
VOID CGridMotor::UpdateValue()
{
	UINT8 u8DrvID		= 0x00, i, j = 0x01, u8Fg = 0x00;
	GV_ITEM stGV		= { NULL };
	LPG_MDSM pstMC2		= uvEng_ShMem_GetMC2();
	LPG_CIEA pstCfg		= uvEng_GetConfig();
	LPG_LDSM pstPH		= uvEng_ShMem_GetLuria();
	COLORREF clrFg[3]	= {RGB(0, 0, 0), RGB(0, 0, 255), RGB(255, 0, 0) };

	/* ȭ�� ���� ��Ȱ��ȭ */
	m_pGrid->SetRedraw(FALSE);

	/* �׸��� ��Ʈ�� �⺻ ���� �Ӽ� */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crBkClr = RGB(255, 255, 255);
	stGV.crFgClr = RGB(0, 0, 0);
	stGV.lfFont = g_lf[eFONT_LEVEL3_BOLD];

	if (0x01 == pstCfg->luria_svc.z_drive_type)
	{
		/* ���� ���� - for Motion Drive Position */
		for (i=0; i<pstCfg->mc2_svc.drive_count; i++,j++)
		{
			/* for State */
			u8DrvID	= pstCfg->mc2_svc.axis_id[i];
			if (pstMC2->IsDriveError(u8DrvID))		{	stGV.strText= L"Error";	u8Fg = 0x02;	}
			else if (pstMC2->IsDriveBusy(u8DrvID))	{	stGV.strText= L"Busy";	u8Fg = 0x01;	}
			else									{	stGV.strText= L"Idle";	u8Fg = 0x00;	}
			stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
			stGV.row	= j;
			stGV.col	= 1;
			stGV.crFgClr= clrFg[u8Fg];
			m_pGrid->SetItem(&stGV);

			/* for Value */
			stGV.strText.Format(L"%.4f", pstMC2->GetDrivePos(u8DrvID));
			stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
			stGV.col	= 2;
			stGV.crFgClr= clrFg[0];
			m_pGrid->SetItem(&stGV);
		}
		/* ���� ���� - for Photohead Position */
		for (i=0; i<pstCfg->luria_svc.ph_count; i++,j++)
		{
			/* for State */
			if (pstPH->directph.IsMotorMiddleInited(i+1))	{	stGV.strText= L"Inited";	u8Fg = 0x00;	}
			else											{	stGV.strText= L"None";		u8Fg = 0x02;	}
			stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
			stGV.row	= j;
			stGV.col	= 1;
			stGV.crFgClr= clrFg[u8Fg];
			m_pGrid->SetItem(&stGV);

			/* for Value */
			stGV.strText.Format(L"%.3f", pstPH->directph.focus_motor_move_abs_position[i]/1000.0f);
			stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
			stGV.col	= 2;
			stGV.crFgClr= clrFg[0];
			m_pGrid->SetItem(&stGV);
		}
	}
	else if (0x03 == pstCfg->luria_svc.z_drive_type)
	{
		/* ���� ���� - for Motion Drive Position */
		for (i=0; i<pstCfg->mc2_svc.drive_count; i++,j++)
		{
			/* for State */
			u8DrvID	= pstCfg->mc2_svc.axis_id[i];
			if (pstMC2->IsDriveError(u8DrvID))		{	stGV.strText= L"Error";	u8Fg = 0x02;	}
			else if (pstMC2->IsDriveBusy(u8DrvID))	{	stGV.strText= L"Busy";	u8Fg = 0x01;	}
			else									{	stGV.strText= L"Idle";	u8Fg = 0x00;	}
			stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
			stGV.row	= j;
			stGV.col	= 1;
			stGV.crFgClr= clrFg[u8Fg];
			m_pGrid->SetItem(&stGV);

			/* for Value */
			stGV.strText.Format(L"%.4f", pstMC2->GetDrivePos(u8DrvID));
			stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
			stGV.col	= 2;
			stGV.crFgClr= clrFg[0];
			m_pGrid->SetItem(&stGV);
		}
	}
// 	/* ���� ���� - for Basler Camera Position (Z Axis Up / Down Postion) */
// 	for (i=0; i<pstCfg->set_cams.acam_count; i++,j++)
// 	{
// 		/* for State */
// 		if (uvCmn_MCQ_IsACamZReady(i+1))	{	stGV.strText= L"Ready";	u8Fg = 0x00;	}
// 		else								{	stGV.strText= L"Busy";	u8Fg = 0x01;	}
// 		stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
// 		stGV.row	= j;
// 		stGV.col	= 1;
// 		stGV.crFgClr= clrFg[u8Fg];
// 		m_pGrid->SetItem(&stGV);
// 
// 		/* for Value */
// 		stGV.strText.Format(L"%.4f", uvCmn_MCQ_GetACamCurrentPosZ(i+1));
// 		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
// 		stGV.col	= 2;
// 		stGV.crFgClr= clrFg[0];
// 		m_pGrid->SetItem(&stGV);
// 	}

	/* ȭ�� ���� Ȱ��ȭ */
	m_pGrid->SetRedraw(TRUE, TRUE);
	/* �׸��� ��Ʈ�� ���� */
	m_pGrid->Invalidate();
}
