
/*
 desc : Monitoring - MC2 Value
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgMC2.h"


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
CDlgMC2::CDlgMC2(UINT32 id, CWnd *parent)
	: CDlgChild(id, parent)
{
	m_pGrid[0]	= NULL;
	m_pGrid[1]	= NULL;
	m_pGrid[2]	= NULL;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDlgMC2::~CDlgMC2()
{
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgMC2::DoDataExchange(CDataExchange* dx)
{
	CDlgChild::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Group box */
	u32StartID	= IDC_MC2_GRP_RECV;
	for (i=0; i<3; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Static */
	u32StartID	= IDC_MC2_TXT_CHNO;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Combo box */
	u32StartID	= IDC_MC2_CMB_CHNO;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_cmb_ctl[i]);
	/* Check box */
	u32StartID	= IDC_MC2_CHK_REF;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgMC2, CDlgChild)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_MC2_CHK_REF, IDC_MC2_CHK_ACT, OnChkClicked)
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgMC2::PreTranslateMessage(MSG* msg)
{

	return CDlgChild::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMC2::OnInitDlg()
{
	if (!CDlgChild::OnInitDlg())	return FALSE;

	/* ��Ʈ�� �ʱ�ȭ */
	if (!InitCtrl())	return FALSE;
	/* GridControl �ʱ�ȭ */
	if (!InitGrid())	return FALSE;

	/* ���� ��ü ȣ�� */
	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgMC2::OnExitDlg()
{
	UINT8 i	= 0x00;
	/* �׸��� ��Ʈ�� �޸� ���� */
	for (; i<3; i++)
	{
		if (m_pGrid[i])
		{
			if (m_pGrid[i]->GetSafeHwnd())	m_pGrid[i]->DestroyWindow();
			delete m_pGrid[i];
		}
	}
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgMC2::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgMC2::OnResizeDlg()
{
}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMC2::InitCtrl()
{
	INT32 i;

	/* Groupbox */
	for (i=0; i<3; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* Combobox */
	for (i=0; i<1; i++)
	{
		m_cmb_ctl[i].SetLogFont(&g_lf);
	}
	/* text - normal */
	for (i=0; i<1; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* check box - normal */
	for (i=0; i<2; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf);
		m_chk_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}

	/* ��Ʈ�� �ʱ�ȭ */
	m_cmb_ctl[0].SetCurSel(0);
	m_chk_ctl[0].SetCheck(1);

	return TRUE;
}

/*
 desc : �׸��� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMC2::InitGrid()
{
	TCHAR tzTitle[2][16]= { L"Item", L"Value" };
	INT32 i32Width[3][2]= { {95, 74}, {95, 74}, {129, 100} }, i, j;
	INT32 i32ColCnt		= 2, i32RowCnt[3] = { 5, 5, 15 }, i32Scroll[3] = { 0, 0, 1 };
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	UINT32 u32GridID[3]	= { IDC_GRID_MC2_RECV, IDC_GRID_MC2_HEAD, IDC_GRID_MC2_DATA};
	GV_ITEM stGV		= { NULL };
	CRect rGrid[3];

	/* Last Recv. Packet / PDO Header / PDO Data (Values) */
	for (i=0; i<3; i++)
	{
		::GetWindowRect(m_grp_ctl[i].GetSafeHwnd(), rGrid[i]);
		ScreenToClient(rGrid[i]);
		rGrid[i].left	+= 15;
		rGrid[i].right	-= 15;
		rGrid[i].top	+= 22;
		rGrid[i].bottom	-= 13;
	}

	/* �׸��� ��Ʈ�� �⺻ ���� �Ӽ� */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* ��ü ���� */
	for (i=0; i<3; i++)
	{
		m_pGrid[i] = new CGridCtrl;
		ASSERT(m_pGrid[i]);
		m_pGrid[i]->SetDrawScrollBar(i32Scroll[i]);
		m_pGrid[i]->SetDrawScrollBarHorz(0);
		m_pGrid[i]->SetDrawScrollBarVert(i32Scroll[i]);
		if (!m_pGrid[i]->Create(rGrid[i], this, u32GridID[i], dwStyle))
		{
			delete m_pGrid[i];
			m_pGrid[i] = NULL;
			return FALSE;
		}
		/* ��ü �⺻ ���� */
		m_pGrid[i]->SetLogFont(g_lf);
		m_pGrid[i]->SetRowCount(i32RowCnt[i]+1);
		m_pGrid[i]->SetColumnCount(2);
		m_pGrid[i]->SetFixedRowCount(1);
		m_pGrid[i]->SetRowHeight(0, 24);
		m_pGrid[i]->SetFixedColumnCount(0);
		m_pGrid[i]->SetBkColor(RGB(255, 255, 255));
		m_pGrid[i]->SetFixedColumnSelection(0);
	}

	/* Ÿ��Ʋ (ù ��° ��) */
	stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	stGV.crBkClr = RGB(214, 214, 214);
	stGV.crFgClr = RGB(0, 0, 0);
	for (i=0; i<3; i++)
	{
		for (j=0; j<i32ColCnt; j++)
		{
			stGV.row	= 0;
			stGV.col	= j;
			stGV.strText= tzTitle[j];
			m_pGrid[i]->SetItem(&stGV);
			m_pGrid[i]->SetColumnWidth(j, i32Width[i][j]);
//			m_pGrid[i]->SetItemFont(0, j, &g_lf);
		}
	}
	/* ���� (1 ~ Rows) ���� ���� */
	stGV.crBkClr = RGB(255, 255, 255);
	for (i=0; i<3; i++)
	{
		for (j=1; j<=i32RowCnt[i]; j++)
		{
			m_pGrid[i]->SetRowHeight(j, 24);

			stGV.nFormat= DT_LEFT | DT_VCENTER | DT_SINGLELINE;
			stGV.strText= L"";
			stGV.row	= j;
			stGV.col	= 0;
			m_pGrid[i]->SetItem(&stGV);
//			m_pGrid[i]->SetItemFont(j, 0, &g_lf);

			stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
			stGV.col	= 1;
			m_pGrid[i]->SetItem(&stGV);
//			m_pGrid[i]->SetItemFont(j, 1, &g_lf);
		}

		m_pGrid[i]->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
		m_pGrid[i]->UpdateGrid();
	}

	/* ���� �ֱٿ� ���Ź��� ��Ŷ ������ ���� */
	m_pGrid[0]->SetItemText(1, 0, L"MSG Type");
	m_pGrid[0]->SetItemText(2, 0, L"DNC Command");
	m_pGrid[0]->SetItemText(3, 0, L"DNC Error");
	m_pGrid[0]->SetItemText(4, 0, L"Sub Index");
	m_pGrid[0]->SetItemText(5, 0, L"Object Index");

	return TRUE;
}

/*
 desc : ���� ���� ��Ʈ�� ���� ����
 parm : None
 retn : None
*/
VOID CDlgMC2::UpdateCtrl()
{
	/* Trigger Memory Data ���� */
	UpdateMC2Value();
}

/*
 desc : MC2 Data (Value) ����
 parm : None
 retn : None
*/
VOID CDlgMC2::UpdateMC2Value()
{
	UINT8 u8ChNo		= (UINT8)m_cmb_ctl[0].GetCurSel(), i;
	BOOL bIsActive		= m_chk_ctl[1].GetCheck() > 0 ? TRUE : FALSE;
	TCHAR tzData[128]	= {NULL}, tzValue[32] = {NULL};
	LPG_MDSM pstMC2		= uvEng_ShMem_GetMC2();

	/* Change the title for a group box */
	if (!bIsActive)	swprintf_s(tzData, 128, L"PDO Ref.Head");
	else			swprintf_s(tzData, 128, L"PDO Act.Head");
	m_grp_ctl[1].SetText(tzData, FALSE);
	if (!bIsActive)	swprintf_s(tzData, 128, L"PDO Ref.Value [Drv.No : %d]", u8ChNo);
	else			swprintf_s(tzData, 128, L"PDO Act.Value [Drv.No : %d]", u8ChNo);
	m_grp_ctl[2].SetText(tzData, FALSE);

	/* ------------------------------------------------------------------------ */
	/* �׸��� ��ü �������� ����. �� �÷� ������ ���� ���� (ȭ�� ������ �ּ�ȭ) */
	/* ------------------------------------------------------------------------ */

	/* ���� �ֱ� ���� ���� ��Ŷ ���� ��� */
	swprintf_s(tzValue, 32, L"0x%02x", pstMC2->last_recv.msg_type);
	m_pGrid[0]->SetItemText(1, 1, tzValue);	m_pGrid[0]->InvalidateRowCol(1, 1);
	swprintf_s(tzValue, 32, L"0x%02x", pstMC2->last_recv.dnc_cmd);
	m_pGrid[0]->SetItemText(2, 1, tzValue);	m_pGrid[0]->InvalidateRowCol(2, 1);
	swprintf_s(tzValue, 32, L"0x%02x", pstMC2->last_recv.dnc_err);
	m_pGrid[0]->SetItemText(3, 1, tzValue);	m_pGrid[0]->InvalidateRowCol(3, 1);
	swprintf_s(tzValue, 32, L"0x%02x", pstMC2->last_recv.sub_id);
	m_pGrid[0]->SetItemText(4, 1, tzValue);	m_pGrid[0]->InvalidateRowCol(4, 1);
	swprintf_s(tzValue, 32, L"0x%08x", pstMC2->last_recv.obj_index);
	m_pGrid[0]->SetItemText(5, 1, tzValue);	m_pGrid[0]->InvalidateRowCol(5, 1);

	/* Reference or Active �׸�� �� ���� */
	if (!bIsActive)
	{
		/* Header */
		m_pGrid[1]->SetItemText(1, 0, L"EN_STOP");										m_pGrid[1]->InvalidateRowCol(1, 0);
		m_pGrid[1]->SetItemText(1, 1, UINT(pstMC2->ref_head.mc_en_stop));				m_pGrid[1]->InvalidateRowCol(1, 1);
		m_pGrid[1]->SetItemText(2, 0, L"HEART-BEAT");									m_pGrid[1]->InvalidateRowCol(2, 0);
		m_pGrid[1]->SetItemText(2, 1, UINT(pstMC2->ref_head.mc_heart_beat));			m_pGrid[1]->InvalidateRowCol(2, 1);
		m_pGrid[1]->SetItemText(3, 0, L"V_SCALE (%)");									m_pGrid[1]->InvalidateRowCol(3, 0);
		m_pGrid[1]->SetItemText(3, 1, UINT(pstMC2->ref_head.v_scale));					m_pGrid[1]->InvalidateRowCol(3, 1);
		m_pGrid[1]->SetItemText(4, 0, L"Outputs");										m_pGrid[1]->InvalidateRowCol(4, 1);
		swprintf_s(tzValue, 32, L"%I64u", pstMC2->ref_head.digital_outputs);
		m_pGrid[1]->SetItemText(4, 1, tzValue);											m_pGrid[1]->InvalidateRowCol(4, 1);
		m_pGrid[1]->SetItemText(5, 0, L"");												m_pGrid[1]->InvalidateRowCol(5, 1);
		m_pGrid[1]->SetItemText(5, 1, L"");												m_pGrid[1]->InvalidateRowCol(5, 1);
		/* Values */
		m_pGrid[2]->SetItemText(1, 0, L"Control Command");								m_pGrid[2]->InvalidateRowCol(1, 0);
		m_pGrid[2]->SetItemText(1, 1, UINT(pstMC2->ref_data[u8ChNo].ctrl_cmd));			m_pGrid[2]->InvalidateRowCol(1, 1);
		m_pGrid[2]->SetItemText(2, 0, L"Command Toggle");								m_pGrid[2]->InvalidateRowCol(2, 0);
		m_pGrid[2]->SetItemText(2, 1, UINT(pstMC2->ref_data[u8ChNo].ctrl_cmd_toggle));	m_pGrid[2]->InvalidateRowCol(2, 1);
		m_pGrid[2]->SetItemText(3, 0, L"Param 0");										m_pGrid[2]->InvalidateRowCol(3, 0);
		m_pGrid[2]->SetItemText(3, 1, UINT(pstMC2->ref_data[u8ChNo].param_0));			m_pGrid[2]->InvalidateRowCol(3, 1);
		m_pGrid[2]->SetItemText(4, 0, L"Param 1");										m_pGrid[2]->InvalidateRowCol(4, 0);
		m_pGrid[2]->SetItemText(4, 1, UINT(pstMC2->ref_data[u8ChNo].param_1));			m_pGrid[2]->InvalidateRowCol(4, 1);
		m_pGrid[2]->SetItemText(5, 0, L"Param 2");										m_pGrid[2]->InvalidateRowCol(5, 0);
		m_pGrid[2]->SetItemText(5, 1, UINT(pstMC2->ref_data[u8ChNo].param_2));			m_pGrid[2]->InvalidateRowCol(5, 1);
		m_pGrid[2]->SetItemText(6, 0, L"Param 3");										m_pGrid[2]->InvalidateRowCol(6, 0);
		m_pGrid[2]->SetItemText(6, 1, UINT(pstMC2->ref_data[u8ChNo].param_3));			m_pGrid[2]->InvalidateRowCol(6, 1);
		for (i=7; i<16; i++)
		{
			m_pGrid[2]->SetItemText(i, 0, L"");											m_pGrid[2]->InvalidateRowCol(i, 0);
			m_pGrid[2]->SetItemText(i, 1, L"");											m_pGrid[2]->InvalidateRowCol(i, 1);
		}
	}
	else
	{
		/* Header */
		m_pGrid[1]->SetItemText(1, 0, L"HEART-BEAT 1");									m_pGrid[1]->InvalidateRowCol(1, 0);
		m_pGrid[1]->SetItemText(1, 1, UINT(pstMC2->act_head.ms_heart_beat1));			m_pGrid[1]->InvalidateRowCol(1, 1);
		m_pGrid[1]->SetItemText(2, 0, L"HEART-BEAT 2");									m_pGrid[1]->InvalidateRowCol(2, 0);
		m_pGrid[1]->SetItemText(2, 1, UINT(pstMC2->act_head.ms_heart_beat2));			m_pGrid[1]->InvalidateRowCol(2, 1);
		m_pGrid[1]->SetItemText(3, 0, L"Error");										m_pGrid[1]->InvalidateRowCol(3, 0);
		m_pGrid[1]->SetItemText(3, 1, UINT(pstMC2->act_head.merror));					m_pGrid[1]->InvalidateRowCol(3, 1);
		m_pGrid[1]->SetItemText(4, 0, L"Inputs");										m_pGrid[1]->InvalidateRowCol(4, 1);
		swprintf_s(tzValue, 32, L"%I64u", pstMC2->act_head.inputs);
		m_pGrid[1]->SetItemText(4, 1, tzValue);											m_pGrid[1]->InvalidateRowCol(4, 1);
		m_pGrid[1]->SetItemText(5, 0, L"Outputs");										m_pGrid[1]->InvalidateRowCol(5, 1);
		swprintf_s(tzValue, 32, L"%I64u", pstMC2->act_head.outputs);
		m_pGrid[1]->SetItemText(5, 1, tzValue);											m_pGrid[1]->InvalidateRowCol(5, 1);
		/* Values */
		m_pGrid[2]->SetItemText(1, 0, L"Command Done");											m_pGrid[2]->InvalidateRowCol(1, 0);
		m_pGrid[2]->SetItemText(1, 1, UINT(pstMC2->act_data[u8ChNo].flag_done));				m_pGrid[2]->InvalidateRowCol(1, 1);
		m_pGrid[2]->SetItemText(2, 0, L"Drive Calibrated");										m_pGrid[2]->InvalidateRowCol(2, 0);
		m_pGrid[2]->SetItemText(2, 1, UINT(pstMC2->act_data[u8ChNo].flag_cal));					m_pGrid[2]->InvalidateRowCol(2, 1);
		m_pGrid[2]->SetItemText(3, 0, L"Power Stage On");										m_pGrid[2]->InvalidateRowCol(3, 0);
		m_pGrid[2]->SetItemText(3, 1, UINT(pstMC2->act_data[u8ChNo].flag_on));					m_pGrid[2]->InvalidateRowCol(3, 1);
		m_pGrid[2]->SetItemText(4, 0, L"Error Exist");											m_pGrid[2]->InvalidateRowCol(4, 0);
		m_pGrid[2]->SetItemText(4, 1, UINT(pstMC2->act_data[u8ChNo].flag_err));					m_pGrid[2]->InvalidateRowCol(4, 1);
		m_pGrid[2]->SetItemText(5, 0, L"Message Code");											m_pGrid[2]->InvalidateRowCol(5, 0);
		m_pGrid[2]->SetItemText(5, 1, UINT(pstMC2->act_data[u8ChNo].flag_msg));					m_pGrid[2]->InvalidateRowCol(5, 1);
		m_pGrid[2]->SetItemText(6, 0, L"Command Busy");											m_pGrid[2]->InvalidateRowCol(6, 0);
		m_pGrid[2]->SetItemText(6, 1, UINT(pstMC2->act_data[u8ChNo].flag_busy));				m_pGrid[2]->InvalidateRowCol(6, 1);
		m_pGrid[2]->SetItemText(7, 0, L"Velocity Zero");										m_pGrid[2]->InvalidateRowCol(7, 0);
		m_pGrid[2]->SetItemText(7, 1, UINT(pstMC2->act_data[u8ChNo].flag_zero));				m_pGrid[2]->InvalidateRowCol(7, 1);
		m_pGrid[2]->SetItemText(8, 0, L"Position Reached");										m_pGrid[2]->InvalidateRowCol(8, 0);
		m_pGrid[2]->SetItemText(8, 1, UINT(pstMC2->act_data[u8ChNo].flag_inpos));				m_pGrid[2]->InvalidateRowCol(8, 1);
		m_pGrid[2]->SetItemText(9, 0, L"Velocity Reached");										m_pGrid[2]->InvalidateRowCol(9, 0);
		m_pGrid[2]->SetItemText(9, 1, UINT(pstMC2->act_data[u8ChNo].flag_varc));				m_pGrid[2]->InvalidateRowCol(9, 1);
		m_pGrid[2]->SetItemText(10,0, L"Drive Initied");										m_pGrid[2]->InvalidateRowCol(10, 0);
		m_pGrid[2]->SetItemText(10,1, UINT(pstMC2->act_data[u8ChNo].flag_init));				m_pGrid[2]->InvalidateRowCol(10, 1);
		m_pGrid[2]->SetItemText(11,0, L"Done Toggled");											m_pGrid[2]->InvalidateRowCol(11, 0);
		m_pGrid[2]->SetItemText(11,1, UINT(pstMC2->act_data[u8ChNo].flag_done_toggle));			m_pGrid[2]->InvalidateRowCol(11, 1);
		m_pGrid[2]->SetItemText(12,0, L"Error Code");											m_pGrid[2]->InvalidateRowCol(12, 0);
		m_pGrid[2]->SetItemText(12,1, UINT(pstMC2->act_data[u8ChNo].error));					m_pGrid[2]->InvalidateRowCol(12, 1);
		m_pGrid[2]->SetItemText(13,0, L"Real Position (mm)");									m_pGrid[2]->InvalidateRowCol(13, 0);
		m_pGrid[2]->SetItemText(13,1, DOUBLE(pstMC2->act_data[u8ChNo].real_position/10000.0f));	m_pGrid[2]->InvalidateRowCol(13, 1);
		m_pGrid[2]->SetItemText(14,0, L"Result 1");												m_pGrid[2]->InvalidateRowCol(14, 0);
		m_pGrid[2]->SetItemText(14,1, UINT(pstMC2->act_data[u8ChNo].result1));					m_pGrid[2]->InvalidateRowCol(14, 1);
		m_pGrid[2]->SetItemText(15,0, L"Result 2");												m_pGrid[2]->InvalidateRowCol(15, 0);
		m_pGrid[2]->SetItemText(15,1, UINT(pstMC2->act_data[u8ChNo].result2));					m_pGrid[2]->InvalidateRowCol(15, 1);
	}
}

/*
 desc : üũ �ڽ� �̺�Ʈ
 parm : id	- [in]  Check box ID
 retn : None
*/
VOID CDlgMC2::OnChkClicked(UINT32 id)
{
	m_chk_ctl[id-IDC_MC2_CHK_REF].SetCheck(1);
	if (id == IDC_MC2_CHK_REF)	m_chk_ctl[1].SetCheck(0);
	else						m_chk_ctl[0].SetCheck(0);

	/* ȭ�� ���� */
	UpdateMC2Value();
}