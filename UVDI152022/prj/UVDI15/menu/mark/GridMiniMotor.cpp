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
 desc : ������
 parm : parent	- [in]  �θ� ���� �ڵ�
		grid	- [in]  Grid�� ��µ� ���� �ڵ�
 retn : None
*/
CGridMiniMotor::CGridMiniMotor(HWND parent, HWND grid)
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
 desc : Grid Control�� ���� �ʱ�ȭ
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

	/* ���� ���� Client ũ�� */
	m_pCtrl->GetWindowRect(rGrid);
	m_pParent->ScreenToClient(rGrid);

	CResizeUI clsResizeUI;
	/* Grid ũ�� �缳�� */
	// by sys&j : ȭ�� ������ ���� ����
	rGrid.left	+= (long)(185 * clsResizeUI.GetRateX());
	rGrid.top	+= (long)(23  * clsResizeUI.GetRateY());
	rGrid.right	-= (long)(14  * clsResizeUI.GetRateX());
	rGrid.bottom-= (long)(135 * clsResizeUI.GetRateY());
	// by sys&j : ȭ�� ������ ���� ����
	
	// by sysandj : Grid Size ����
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

	/* ���� ���� - for Photohead Position */
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

	/* ���� ���� - for Motion Drive Position */
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

	/* ���� ���� - for Basler Camera Position (Z Axis Up / Down Postion) */
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

	/* �⺻ �Ӽ� �� ���� */
	m_pGrid->SetBaseGridProp(0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGrid->Refresh();
}

/*
 desc : ���� ��ġ �� ���� ���� ����
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

	/* ȭ�� ���� ��Ȱ��ȭ */
	m_pGrid->SetRedraw(FALSE);

	/* �׸��� ��Ʈ�� �⺻ ���� �Ӽ� */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crBkClr = RGB(255, 255, 255);
	stGV.crFgClr = RGB(0, 0, 0);
	stGV.lfFont = g_lf[eFONT_LEVEL2_BOLD];

	/* ���� ���� - for Photohead Position */
	for (i=0; i<pstCfg->luria_svc.ph_count; i++,j++)
	{
		/* for Value */
		stGV.strText.Format(L"%.3f", pstPH->directph.focus_motor_move_abs_position[i]/1000.0f);
		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.row	= j;
		stGV.col	= 1;
		m_pGrid->SetItem(&stGV);
	}

	/* ���� ���� - for Motion Drive Position */
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

	/* ���� ���� - for Basler Camera Position (Z Axis Up / Down Postion) */
	for (i=0; i<pstCfg->set_cams.acam_count; i++,j++)
	{
		/* for Value */
		//stGV.strText.Format(L"%.4f", uvCmn_MCQ_GetACamCurrentPosZ(i+1));
		double dCamPos = 0; // by sysandj : MCQ��ü �߰� �ʿ�
		stGV.strText.Format(L"%.4f", dCamPos);
		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.row	= j;
		stGV.col	= 1;
		m_pGrid->SetItem(&stGV);
	}

	/* ȭ�� ���� Ȱ��ȭ */
	m_pGrid->SetRedraw(TRUE, TRUE);
	/* �׸��� ��Ʈ�� ���� */
	m_pGrid->Invalidate();
}
