
/*
 desc : Monitoring - EFEM Robot
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgEFEM.h"


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
CDlgEFEM::CDlgEFEM(UINT32 id, CWnd *parent)
	: CDlgChild(id, parent)
{
	m_pGrid	= NULL;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDlgEFEM::~CDlgEFEM()
{
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgEFEM::DoDataExchange(CDataExchange* dx)
{
	CDlgChild::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Group box */
	u32StartID	= IDC_EFEM_GRP_BASE;
	for (i=0; i<3; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Static */
	u32StartID	= IDC_EFEM_TXT_FOUP;
	for (i=0; i<11; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Edit box */
	u32StartID	= IDC_EFEM_EDT_LOAD;
	for (i=0; i<10; i++)	DDX_Control(dx, u32StartID+i,	m_edt_ctl[i]);
	/* Combo box */
	u32StartID	= IDC_EFEM_CMB_FOUP;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_cmb_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgEFEM, CDlgChild)
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgEFEM::PreTranslateMessage(MSG* msg)
{

	return CDlgChild::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgEFEM::OnInitDlg()
{
	if (!CDlgChild::OnInitDlg())	return FALSE;

	/* ��Ʈ�� �ʱ�ȭ */
	InitCtrl();
	InitGrid();

	/* ���� ��ü ȣ�� */
	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgEFEM::OnExitDlg()
{
	if (m_pGrid)
	{
		if (m_pGrid->GetSafeHwnd())	m_pGrid->DestroyWindow();
		delete m_pGrid;
	}
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgEFEM::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgEFEM::OnResizeDlg()
{
}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgEFEM::InitCtrl()
{
	INT32 i;

	/* Groupbox */
	for (i=0; i<3; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* text - normal */
	for (i=0; i<11; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* Edit box */
	for (i=0; i<10; i++)
	{
		m_edt_ctl[i].SetEditFont(&g_lf);
		m_edt_ctl[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_ctl[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_ctl[i].SetInputType(EN_DITM::en_string);
		m_edt_ctl[i].SetReadOnly(TRUE);
		m_edt_ctl[i].SetMouseClick(FALSE);
	}
	/* Combo box - normal */
	for (i=0; i<1; i++)
	{
		m_cmb_ctl[i].SetLogFont(&g_lf);
	}

	/* Combo Box ���� */
	m_cmb_ctl[0].ResetContent();
	for (i=0; i<uvEng_Conf_GetEFEM()->foup_count; i++)
	{
		m_cmb_ctl[0].AddNumber(UINT64(i+1));
	}
	if (i > 0)	m_cmb_ctl[0].SetCurSel(0);
}

/*
 desc : �׸��� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgEFEM::InitGrid()
{
	TCHAR tzCols[4][8]	= { L"Item", L"loaded", L"exposed", L"error" };
	INT32 i32Width[4]	= { 44, 50, 55, 49 }, i, j;
	INT32 i32ColCnt		= 4, i32RowCnt = uvEng_Conf_GetEFEM()->foup_lots;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* Last Recv. Packet / PDO Header / PDO Data (Values) */
	::GetWindowRect(m_grp_ctl[2].GetSafeHwnd(), rGrid);
	ScreenToClient(rGrid);
	rGrid.left	+= 14;
	rGrid.right	-= 14;
	rGrid.top	+= 56;
	rGrid.bottom-= 14;

	/* �׸��� ��Ʈ�� �⺻ ���� �Ӽ� */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* ��ü ���� */
	m_pGrid = new CGridCtrl;
	ASSERT(m_pGrid);
	m_pGrid->SetDrawScrollBar(1);
	m_pGrid->SetDrawScrollBarHorz(0);
	m_pGrid->SetDrawScrollBarVert(1);
	if (!m_pGrid->Create(rGrid, this, IDC_GRID_FOUP_INFO, dwStyle))
	{
		delete m_pGrid;
		m_pGrid = NULL;
		return;
	}
	/* ��ü �⺻ ���� */
	m_pGrid->SetLogFont(g_lf);
	m_pGrid->SetRowCount(i32RowCnt+1);
	m_pGrid->SetColumnCount(i32ColCnt);
	m_pGrid->SetFixedRowCount(1);
	m_pGrid->SetRowHeight(0, 24);
	m_pGrid->SetFixedColumnCount(0);
	m_pGrid->SetBkColor(RGB(255, 255, 255));
	m_pGrid->SetFixedColumnSelection(0);

	/* Ÿ��Ʋ (ù ��° ��) */
	stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	stGV.crBkClr = RGB(214, 214, 214);
	stGV.crFgClr = RGB(0, 0, 0);
	for (i=0; i<4; i++)
	{
		stGV.nFormat= DT_VCENTER | DT_SINGLELINE | DT_CENTER;
		stGV.row	= 0;
		stGV.col	= i;
		stGV.strText= tzCols[i];
		m_pGrid->SetItem(&stGV);
		m_pGrid->SetColumnWidth(i, i32Width[i]);
//		m_pGrid->SetItemFont(0, i, &g_lf);
	}
	/* ���� (1 ~ Rows) ���� ���� */
	stGV.crBkClr = RGB(255, 255, 255);
	for (i=1; i<=i32RowCnt; i++)
	{
		m_pGrid->SetRowHeight(i, 23);
		for (j=0; j<4; j++)
		{
			stGV.nFormat= DT_VCENTER | DT_SINGLELINE;
			if (j == 0)
			{
				stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
				stGV.strText.Format(L"%02d", i);
			}
			else
			{
				stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
				stGV.strText= L"";
			}
			stGV.row	= i;
			stGV.col	= j;
			m_pGrid->SetItem(&stGV);
//			m_pGrid->SetItemFont(i, j, &g_lf);
		}
	}

	m_pGrid->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGrid->UpdateGrid();
}

/*
 desc : ���� ���� ��Ʈ�� ���� ����
 parm : None
 retn : None
*/
VOID CDlgEFEM::UpdateCtrl()
{
	/* Update Log Data */
	UpdateMoni();
}

/*
 desc : PLC Memory Data (Value) ����
 parm : None
 retn : None
*/
VOID CDlgEFEM::UpdateMoni()
{
	TCHAR tzVals[64]	= {NULL};
	UINT8 i, u8Foups	= uvEng_Conf_GetEFEM()->foup_count, u8Event;
	INT32 i32Sel		= m_cmb_ctl[0].GetCurSel();
	LPG_EDSM pstShMem	= uvEng_ShMem_GetEFEM();
	LPG_LWDS pstFoup	= NULL;

	/* ���� ��ȿ���� ���� Ȯ�� */
	if (i32Sel < 0 || u8Foups < 1 || !pstShMem)	return;

	/* FOUP Load ���� */
	swprintf_s(tzVals, 64, L"FOUP1:%d | FOUP2:%d", pstShMem->foup_1_load, pstShMem->foup_2_load);
	m_edt_ctl[0].SetTextToStr(tzVals, TRUE);
	/* EFEM State */
	switch (pstShMem->efem_state)
	{
	case ENG_ECSV::en_ready	:	m_edt_ctl[1].SetTextToStr(L"Ready", TRUE);	break;
	case ENG_ECSV::en_busy	:	m_edt_ctl[1].SetTextToStr(L"Busy", TRUE);	break;
	case ENG_ECSV::en_error	:	m_edt_ctl[1].SetTextToStr(L"Error", TRUE);	break;
	}
	/* EFEM Wafer */
	switch (pstShMem->efem_state)
	{
	case ENG_EWLI::en_wafer_in_foup_new	:
	case ENG_EWLI::en_wafer_in_foup_old	:	m_edt_ctl[2].SetTextToStr(L"Empty", TRUE);		break;
	case ENG_EWLI::en_wafer_in_header	:	m_edt_ctl[2].SetTextToStr(L"Header", TRUE);		break;
	case ENG_EWLI::en_wafer_in_aligner	:	m_edt_ctl[2].SetTextToStr(L"Aligner", TRUE);	break;
	case ENG_EWLI::en_wafer_in_buffer	:	m_edt_ctl[2].SetTextToStr(L"Buffer", TRUE);		break;
	}
	/* CMPS & EFEM Event */
	for (i=0; i<2; i++)
	{
		u8Event	= i == 0 ? pstShMem->cmps_event : pstShMem->efem_event;
		switch (u8Event)
		{
		case ENG_EFEC::en_none						:	m_edt_ctl[3+i].SetTextToStr(L"None", TRUE);				break;

		case ENG_EFEC::en_cmps_from_foup_to_aligner	:
		case ENG_EFEC::en_efem_from_foup_to_aligner	:	m_edt_ctl[3+i].SetTextToStr(L"FOUP To Aligner", TRUE);	break;

		case ENG_EFEC::en_cmps_from_buffer_to_foup	:
		case ENG_EFEC::en_efem_from_buffer_to_foup	:	m_edt_ctl[3+i].SetTextToStr(L"Buffer To FOUP", TRUE);	break;

		case ENG_EFEC::en_cmps_open_shutter			:	m_edt_ctl[3+i].SetTextToStr(L"OPEN Shutter", TRUE);		break;
		case ENG_EFEC::en_efem_load_completed		:	m_edt_ctl[3+i].SetTextToStr(L"Load Completed", TRUE);	break;

		case ENG_EFEC::en_cmps_complete_to_expose	:	m_edt_ctl[3+i].SetTextToStr(L"Print Completed", TRUE);	break;
		case ENG_EFEC::en_efem_load_on_buffer		:	m_edt_ctl[3+i].SetTextToStr(L"Load On Buffer", TRUE);	break;
		}
	}
	/* Recovery Rate */
	swprintf_s(tzVals, 64, L"%u %%", pstShMem->efem_rate);
	m_edt_ctl[5].SetTextToStr(tzVals);
	/* Reset Check */
	if (pstShMem->efem_reset)	m_edt_ctl[6].SetTextToStr(L"Reset received", TRUE);
	else						m_edt_ctl[6].SetTextToStr(L"Reset waiting", TRUE);
	/* Last Error Code */
	swprintf_s(tzVals, 64, L"0x%04x", pstShMem->error_last);
	m_edt_ctl[7].SetTextToStr(tzVals);
	/* Recent Error Code */
	if (pstShMem->IsQueEmpty() && pstShMem->err_front == 0x00)
	{
		m_edt_ctl[8].SetTextToStr(L"0x0000", TRUE);
		m_edt_ctl[9].SetTextToStr(L"0000-00-00 00:00:00", TRUE);
	}
	else
	{
		/* Que�� �� �̻� ���ŵ� ���� �ڵ尡 ������ Ȯ�� */
		if (!pstShMem->IsQueEmpty())
		{
			LPG_EATD pstInfo	= pstShMem->GetErrorTime();
			if (pstInfo)
			{
				m_edt_ctl[8].SetTextToHex(UINT16(pstInfo->error), TRUE);
				swprintf_s(tzVals, 64, L"%04d-%02d-%02d %02d:%02d:%02d",
						   pstInfo->year, pstInfo->month, pstInfo->day,
						   pstInfo->hour, pstInfo->minute, pstInfo->second);
				m_edt_ctl[9].SetTextToStr(tzVals, TRUE);
			}
		}
	}

	/* FOUP Information */
	m_pGrid->SetRedraw(FALSE);

	pstFoup	= i32Sel == 0 ? pstShMem->foup_1_list : pstShMem->foup_2_list;
	for (i=1; i<=uvEng_Conf_GetEFEM()->foup_lots; i++)
	{
		m_pGrid->SetItemText(i, 1, pstFoup[i].loaded ? L"Exist" : L"Empty");
		m_pGrid->SetItemText(i, 2, pstFoup[i].expose ? L"Old" : L"New");
		m_pGrid->SetItemText(i, 3, pstFoup[i].error ? L"Error" : L"Normal");
	}

	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate(FALSE);
}
