
/*
 desc : Frame Rate ���� ��ȭ ����
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgSped.h"

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
CDlgSped::CDlgSped(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgSped::IDD, parent)
{
	m_bDrawBG	= TRUE;
	m_bTooltip	= TRUE;
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgSped::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Group - Normal */
	u32StartID	= IDC_SPED_GRP_FRAME_RATE;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Button - Normal */
	u32StartID	= IDC_SPED_BTN_APPLY;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgSped, CMyDialog)
	ON_NOTIFY(NM_CLICK, IDC_GRID_SPED_FRAME_RATE, OnGridFrameRate)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_SPED_BTN_APPLY, IDC_SPED_BTN_CANCEL, OnBtnClicked)
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgSped::PreTranslateMessage(MSG* msg)
{

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgSped::OnInitDlg()
{
	/* ��Ʈ�� �ʱ�ȭ */
	if (!InitCtrl())	return FALSE;
	if (!InitGrid())	return FALSE;

	/* TOP_MOST & Center */
	CenterParentTopMost();
	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgSped::OnExitDlg()
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
VOID CDlgSped::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgSped::OnResizeDlg()
{
}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgSped::InitCtrl()
{
	INT32 i;
	PTCHAR pText	= NULL;

	/* group box - normal */
	for (i=0; i<1; i++)	m_grp_ctl[i].SetLogFont(&g_lf[eFONT_LEVEL2_BOLD]);
	/* button - normal */
	for (i=0; i<2; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);
	}

	return TRUE;
}

/*
 desc : �ʱ�ȭ (����)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgSped::InitGrid()
{
	CRect rGrid;
	GV_ITEM	stGV;

	/* Grid Control �Ӽ� �� */
	stGV.mask		= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.nFormat	= DT_LEFT | DT_VCENTER | DT_SINGLELINE;
	stGV.crFgClr	= RGB(32, 32, 32);
	stGV.crBkClr	= RGB(255, 255, 255);

	/* ���� ���� Client ũ�� */
	m_grp_ctl[0].GetWindowRect(rGrid);
	ScreenToClient(rGrid);

	CResizeUI clsResizeUI;
	/* Grid ũ�� �缳�� */
	rGrid.left		+= (long)(15 * clsResizeUI.GetRateX());
	rGrid.top		+= (long)(22 * clsResizeUI.GetRateY());
	rGrid.right		-= (long)(15 * clsResizeUI.GetRateX());
	rGrid.bottom	-= (long)(15 * clsResizeUI.GetRateY());

	/* ��ü ���� */
	m_pGrid = new CGridCtrl;
	ASSERT(m_pGrid);
	m_pGrid->SetDrawScrollBarVert(TRUE);
	m_pGrid->SetDrawScrollBarHorz(FALSE);
	if (!m_pGrid->Create(rGrid, this, IDC_GRID_SPED_FRAME_RATE, WS_CHILD|WS_TABSTOP|WS_VISIBLE))
	{
		delete m_pGrid;
		return FALSE;
	}

	/* ��ü �⺻ ���� */
	m_pGrid->SetItem(&stGV);
	m_pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	m_pGrid->SetColumnCount(1);
	m_pGrid->SetBkColor(RGB(255, 255, 255));

	/* ���� */
	m_pGrid->SetColumnWidth(0, rGrid.Width()-20);

	/* �⺻ �Ӽ� �� ���� */
	m_pGrid->SetBaseGridProp(0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGrid->Refresh();

	/* �Ź� ������ ���� */
	LoadFrameRate();

	return TRUE;
}

/*
 desc : �ӵ� ���� ���� ���� ����
 parm : None
 retn : None
*/
VOID CDlgSped::LoadFrameRate()
{
	INT32 i32RowCnt		= 0;
	TCHAR tzValue[32]	= {NULL};
	LPG_OSSD pstPhStep	= NULL;

	/* ȭ�� ���� ��Ȱ��ȭ */
	m_pGrid->SetRedraw(FALSE);
	/* ���� �׸� ��� ���� */
	m_pGrid->DeleteNonFixedRows();

	do {

		/* �׸� �������� */
		pstPhStep	= uvEng_PhStep_GetRecipeIndex(i32RowCnt);
		if (!pstPhStep)	break;
		/* Frame Speed �� ���� ���� */
		if (pstPhStep->frame_rate < 1)	break;

		/* �׸� ������ŭ Row ���� */
		m_pGrid->SetRowCount(i32RowCnt+1);
		m_pGrid->SetRowHeight(i32RowCnt, 30);
		m_pGrid->SetItemFormat(i32RowCnt, 0, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		swprintf_s(tzValue, 32, L" %.3f", pstPhStep->frame_rate/1000.0f);
		m_pGrid->SetItemText(i32RowCnt, 0, tzValue);
		/* ���� ��ġ�� �׸� ���� */
		i32RowCnt++;

	} while (1);

	/* ���� ���� ���� */
	m_pGrid->SortItems(0, TRUE);
	/* ȭ�� ���� Ȱ��ȭ */
	m_pGrid->SetRedraw(TRUE, TRUE);
	/* �׸��� ��Ʈ�� ���� */
	m_pGrid->Invalidate(TRUE);
}


/*
 desc : ���� �߻��� ���� ���� ���
 parm : None
 retn : None
*/
VOID CDlgSped::OnBtnClicked(UINT32 id)
{
	if (id == IDC_SPED_BTN_APPLY)	CMyDialog::EndDialog(IDOK);
	else							CMyDialog::EndDialog(IDCANCEL);
}

/*
 desc : DoModal Override �Լ�
 parm : None
 retn : 0L
*/
INT_PTR CDlgSped::MyDoModal()
{
	m_dbFrameRate	= 0.0f;

	return DoModal();
}

/*
 desc : Gerber Recipe �׸��� ������ ���
 parm : nm_hdr	- [in]  Grid Control ������ ����� ����ü ������
		result	- [in]  fixed value (0)
 retn : None
*/
VOID CDlgSped::OnGridFrameRate(NMHDR *nm_hdr, LRESULT *result)
{
	NM_GRIDVIEW *pstGrid= (NM_GRIDVIEW*)nm_hdr;
	m_dbFrameRate	= m_pGrid->GetItemTextToFloat(pstGrid->iRow, 0);
}
