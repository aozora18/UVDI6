
/*
 desc : ���а� ���� ����
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgStep.h"

#include "../../inc/grid/GridCtrl.h"

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : ������
 parm : id		- [in]  �ڽ��� ���� ID
		parent	- [in]  �ڽ��� ȣ���� �θ� ���� Ŭ���� ������
 retn : None
*/
CDlgStep::CDlgStep(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgStep::IDD, parent)
{
	m_bDrawBG	= TRUE;
	m_bTooltip	= TRUE;
	m_clrBg		= RGB(255, 255, 255);
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgStep::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Groupbox - Normal */
	u32StartID	= IDC_STEP_GRP_STEP_XY;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Button - Normal */
	u32StartID	= IDC_STEP_BTN_APPLY;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Static - Normal */
	u32StartID	= IDC_STEP_TXT_NEGA_OFFSET;
	for (i=0; i<6; i++)		DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Edit - Normal */
	u32StartID	= IDC_STEP_EDT_NEGA_OFFSET;
	for (i=0; i<3; i++)		DDX_Control(dx, u32StartID+i,	m_edt_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgStep, CMyDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_STEP_BTN_APPLY, IDC_STEP_BTN_CANCEL, OnBtnClicked)
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgStep::PreTranslateMessage(MSG* msg)
{

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgStep::OnInitDlg()
{
	/* ��Ʈ�� �ʱ�ȭ */
	InitCtrl();
	InitGrid();

	/* Load Data */
	LoadStepXY();

	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgStep::OnExitDlg()
{
	CloseGrid();
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgStep::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgStep::OnResizeDlg()
{
}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgStep::InitCtrl()
{
	INT32 i, i32Min[5] = {1, 0, 0, 0, 1};
	PTCHAR pText	= NULL;

	/* Normal - Group box */
	for (i=0; i<1; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* button - normal */
	for (i=0; i<2; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
	}
	/* text - normal */
	for (i=0; i<6; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* edit - Normal Value */
	for (i=0; i<3; i++)
	{
		m_edt_ctl[i].SetEditFont(&g_lf);
		m_edt_ctl[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_ctl[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_ctl[i].SetInputType(ENM_DITM::en_int8);
		m_edt_ctl[i].SetMouseClick(FALSE);
		m_edt_ctl[i].SetReadOnly(TRUE);
	}
}

/*
 desc : �׸��� ��Ʈ�� ����
 parm : None
 retn : None
*/
VOID CDlgStep::CloseGrid()
{
	if (m_pGridStepXY)
	{
		m_pGridStepXY->DestroyWindow();
		delete m_pGridStepXY;
	}
}

/*
 desc : ���� ���� ����
 parm : None
 retn : None
*/
VOID CDlgStep::InitGrid()
{
	TCHAR tzCols[3][16]	= { L"Item", L"StepX (um)", L"StepY (um)" }, tzRows[32] = {NULL};
	INT32 i32Width[3]	= { 65, 80, 80 }, i, j=1;
	INT32 i32ColCnt		= 3, i32RowCnt = 0;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	LPG_CIEA pstCfg		= uvEng_GetConfig();
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* Last Recv. Packet / PDO Header / PDO Data (Values) */
	::GetWindowRect(m_grp_ctl[0].GetSafeHwnd(), rGrid);
	ScreenToClient(rGrid);
	rGrid.left	+= 15;
	rGrid.right	-= 15;
	rGrid.top	+= 22;
	rGrid.bottom-= 13;

	/* �� ��µ� �� (Row)�� ���� ���ϱ� */
	i32RowCnt	= pstCfg->luria_svc.ph_count;
	/* �׸��� ��Ʈ�� �⺻ ���� �Ӽ� */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* ��ü ���� */
	m_pGridStepXY	= new CGridCtrl;
	ASSERT(m_pGridStepXY);
	m_pGridStepXY->SetDrawScrollBarHorz(FALSE);
	m_pGridStepXY->SetDrawScrollBarVert(FALSE);
	if (!m_pGridStepXY->Create(rGrid, this, IDC_STEP_GRID_STEP_XY, dwStyle))
	{
		delete m_pGridStepXY;
		m_pGridStepXY = NULL;
		return;
	}
	/* ��ü �⺻ ���� */
	m_pGridStepXY->SetLogFont(g_lf);
	m_pGridStepXY->SetRowCount(i32RowCnt);
	m_pGridStepXY->SetColumnCount(i32ColCnt);
	m_pGridStepXY->SetFixedRowCount(1);
	m_pGridStepXY->SetRowHeight(0, 24);
	m_pGridStepXY->SetFixedColumnCount(0);
	m_pGridStepXY->SetBkColor(RGB(255, 255, 255));
	m_pGridStepXY->SetFixedColumnSelection(0);

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
		m_pGridStepXY->SetItem(&stGV);
		m_pGridStepXY->SetColumnWidth(i, i32Width[i]);
	}

	/* ���� ���� */
	stGV.crBkClr = RGB(255, 255, 255);

	/* ���� ���� - for Photohead Position */
	for (i=0x01,j=0x01; i<pstCfg->luria_svc.ph_count; i++,j++)
	{
		m_pGridStepXY->SetRowHeight(j, 24);

		/* for Subject */
		stGV.strText.Format(L"PH %d", i+1);
		stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
		stGV.row	= j;
		stGV.col	= 0;
		m_pGridStepXY->SetItem(&stGV);

		/* for Subject */
		stGV.strText= L"0.000";
		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.col	= 1;
		m_pGridStepXY->SetItem(&stGV);

		/* for Subject */
		stGV.col	= 2;
		m_pGridStepXY->SetItem(&stGV);
	}

	m_pGridStepXY->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGridStepXY->UpdateGrid();
}

/*
 desc : ���� �߻��� ���� ���� ���
 parm : None
 retn : None
*/
VOID CDlgStep::OnBtnClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_STEP_BTN_APPLY		:	ApplyStepXY();			break;
	case IDC_STEP_BTN_CANCEL	:	CMyDialog::OnCancel();	break;
	}
}

/*
 desc : DoModal Override
 parm : data	- [in]  ���� ���а� ���� ������ ����� ����ü ������
 retn : ?
*/
INT_PTR CDlgStep::MyDoModal(LPG_OSSD data)
{
	memcpy(&m_stStep, data, sizeof(STG_OSSD));

	return DoModal();
}

/*
 desc : Model ��� �۾� ����
 parm : None
 retn : None
*/
VOID CDlgStep::ApplyStepXY()
{
	UINT8 i		= 0x01;
	BOOL bSucc	= FALSE;

	/* �� ��ȿ�� Ȯ�� */
	if (abs(INT32(m_stStep.delay_posi_nsec) - INT32(m_stStep.delay_nega_nsec)) > 1)
	{
		AfxMessageBox(L"The value entered is not valid", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
#if 0	/* ���� ����� �Ź��� ������ ���� �ȵǹǷ�, ȯ�� ���Ͽ��� ������Ʈ ���� */
	/* �ٷ� ���а� ������ ������ ������ ���� */
	if (IDYES == AfxMessageBox(L"Shall we apply the PH Offset values right away?", MB_YESNO))
	{
		if (!uvEng_Luria_ReqSetMotionType1Hysteresis(m_stStep.scroll_mode, m_stStep.nega_offset_px,
													 m_stStep.delay_posi_nsec, m_stStep.delay_nega_nsec))
		{
			AfxMessageBox(L"Failed to apply the PH Offset values", MB_ICONSTOP|MB_TOPMOST);
			return;
		}
		for (i=1; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
		{
			uvEng_Luria_ReqSetSpecPhotoHeadOffset(i+1,
												  m_stStep.step_x_nm[i], m_stStep.step_y_nm[i]);
		}
	}
#endif
	/* ȯ�� ���Ͽ� ���а� ���� �� ���� */
	for (i=0x01; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		uvEng_GetConfig()->luria_svc.ph_offset_x[i]	= m_stStep.step_x_nm[i]/1000000.0f;
		uvEng_GetConfig()->luria_svc.ph_offset_y[i]	= m_stStep.step_y_nm[i]/1000000.0f;
	}
	uvEng_GetConfig()->luria_svc.hys_type_1_scroll_mode		= m_stStep.scroll_mode;
	uvEng_GetConfig()->luria_svc.hys_type_1_negative_offset	= m_stStep.nega_offset_px;
	uvEng_GetConfig()->luria_svc.hys_type_1_delay_offset[0]	= m_stStep.delay_posi_nsec/1000000.0f;
	uvEng_GetConfig()->luria_svc.hys_type_1_delay_offset[1]	= m_stStep.delay_nega_nsec/1000000.0f;
	/* ȯ�� ���� ���� */
	uvEng_SaveConfig();
#if 1
	/* ������ �����ϴ� ���̸� ���� ������ ���� ���� */
	if (uvEng_PhStep_RecipeFind(m_stStep.frame_rate))
	{
		uvEng_PhStep_RecipeDelete(m_stStep.frame_rate);
	}
	/* ���� ���ŵ� ������ �߰� */
	if (!uvEng_PhStep_RecipeAppend(&m_stStep))
	{
		AfxMessageBox(L"Failed to save the offset value for PH", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* ���Ϸ� ���� */
	uvEng_PhStep_RecipeSave();
#endif
	/* ���� �۾� ���� */
	CMyDialog::OnOK();
}

/*
 desc : Edge Find Result Data ����
 parm : None
 retn : None
*/
VOID CDlgStep::LoadStepXY()
{
	UINT8 i;

	for (i=0x01; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		m_pGridStepXY->SetItemDouble(i, 1, m_stStep.step_x_nm[i]/1000.0f, 3);
		m_pGridStepXY->SetItemDouble(i, 2, m_stStep.step_y_nm[i]/1000.0f, 3);
	}

	/* ���� �����Ǿ� �ִ� Hysteresis �� �Է� */
	m_edt_ctl[0].SetTextToNum(m_stStep.nega_offset_px);
	m_edt_ctl[1].SetTextToNum(m_stStep.delay_posi_nsec);
	m_edt_ctl[2].SetTextToNum(m_stStep.delay_nega_nsec);
}
