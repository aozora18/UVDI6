
/*
 desc : Monitoring - PLC Value
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgPlcQ.h"

#include "PLC/DlgPSet.h"


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
CDlgPlcQ::CDlgPlcQ(UINT32 id, CWnd *parent)
	: CDlgChild(id, parent)
{
	m_pGrid		= NULL;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDlgPlcQ::~CDlgPlcQ()
{
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgPlcQ::DoDataExchange(CDataExchange* dx)
{
	CDlgChild::DoDataExchange(dx);

}

BEGIN_MESSAGE_MAP(CDlgPlcQ, CDlgChild)
	ON_BN_CLICKED(IDC_GRID_PLCQ_DATA,	OnGridClicked)
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgPlcQ::PreTranslateMessage(MSG* msg)
{

	return CDlgChild::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgPlcQ::OnInitDlg()
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
VOID CDlgPlcQ::OnExitDlg()
{
	/* �׸��� ��Ʈ�� �޸� ���� */
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
VOID CDlgPlcQ::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgPlcQ::OnResizeDlg()
{
}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgPlcQ::InitCtrl()
{

	return TRUE;
}

/*
 desc : �׸��� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgPlcQ::InitGrid()
{
	TCHAR tzTitle[3][40]= { L"D Address",
							L"F E D C  B A 9 8  7 6 5 4  3 2 1 0",
							L"Value (16 : 32 bit)" };
	TCHAR tzDAddr[32]	= {NULL};
	INT32 i32Width[3]	= { 90, 240, 145 }, i;
	INT32 i32ColCnt		= 3, i32RowCnt = (INT32)uvEng_Conf_GetPLC()->addr_d_count;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	UINT32 dwFormat[2]	= { DT_VCENTER | DT_SINGLELINE | DT_CENTER,
							DT_VCENTER | DT_SINGLELINE | DT_RIGHT };
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* ���� ���� ���� ���, �׸��� ũ�� ���� */
	GetClientRect(rGrid);
	rGrid.left	+= 10;
	rGrid.right	-= 10;
	rGrid.top	+= 13;
	rGrid.bottom-= 10;

	/* �׸��� ��Ʈ�� �⺻ ���� �Ӽ� */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* ��ü ���� */
	m_pGrid = new CGridCtrl;
	ASSERT(m_pGrid);
	m_pGrid->SetDrawScrollBar(1);
	m_pGrid->SetDrawScrollBarVert(1);
	m_pGrid->SetDrawScrollBarHorz(1);
	if (!m_pGrid->Create(rGrid, this, IDC_GRID_PLCQ_DATA, dwStyle))
	{
		delete m_pGrid;
		m_pGrid = NULL;
		return FALSE;
	}
	/* ��ü �⺻ ���� */
	m_pGrid->SetLogFont(g_lf);
	m_pGrid->SetRowCount(i32RowCnt+1);
	m_pGrid->SetColumnCount(i32ColCnt);
	m_pGrid->SetFixedRowCount(1);
	m_pGrid->SetRowHeight(0, 25);
	m_pGrid->SetFixedColumnCount(0);
	m_pGrid->SetBkColor(RGB(255, 255, 255));
	m_pGrid->SetFixedColumnSelection(0);

	/* Ÿ��Ʋ (ù ��° ��) */
	stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	stGV.crBkClr = RGB(214, 214, 214);
	stGV.crFgClr = RGB(0, 0, 0);
	for (i=0; i<i32ColCnt; i++)
	{
		stGV.nFormat= dwFormat[0];
		stGV.row = 0;
		stGV.col = i;
		stGV.strText = tzTitle[i];
		m_pGrid->SetItem(&stGV);
		m_pGrid->SetColumnWidth(i, i32Width[i]);
//		m_pGrid->SetItemFont(0, i, &g_lf);
	}

	/* ���� (1 ~ Rows) ���� / �ּ� ���� (���� ���� �ӵ� ������ �Ұ����ϰ� ���� for �� ��� ����) */
	stGV.crBkClr = RGB(255, 255, 255);
	for (i=1; i<=i32RowCnt; i++)
	{
		m_pGrid->SetRowHeight(i, 25);

		stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
		stGV.strText.Format(L"%05d", uvEng_Conf_GetPLC()->start_d_addr+(i-1));
		stGV.row	= i;
		stGV.col	= 0;
		m_pGrid->SetItem(&stGV);
//		m_pGrid->SetItemFont(i, 0, &g_lf);

		stGV.strText= L"";
		stGV.row	= i;
		stGV.col	= 1;
		m_pGrid->SetItem(&stGV);
//		m_pGrid->SetItemFont(i, 1, &g_lf);

		stGV.nFormat = DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.strText= L"";
		stGV.row	= i;
		stGV.col	= 2;
		m_pGrid->SetItem(&stGV);
//		m_pGrid->SetItemFont(i, 2, &g_lf);
	}

	m_pGrid->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGrid->UpdateGrid();

	return TRUE;
}

/*
 desc : ���� ���� ��Ʈ�� ���� ����
 parm : None
 retn : None
*/
VOID CDlgPlcQ::UpdateCtrl()
{
	/* PLC Memory Data ���� */
	UpdatePLCValue();
}

/*
 desc : PLC Memory Data (Value) ����
 parm : None
 retn : None
*/
VOID CDlgPlcQ::UpdatePLCValue()
{
	TCHAR tzVal[64]	= {NULL}, tzBin[2][4] = { L"0 ", L"1 " };
	UINT16 u16Val	= {NULL};
	UINT32 u32Begin, u32End, u32Val = 0;
	PUINT16 pPlcMem	= (PUINT16)uvEng_ShMem_GetPLC()->data;	/* PLC Shared Memory */

	/* ���� ��ũ�� ���� ��ġ ��� */
	if (!m_pGrid->GetViewRowIndex(u32Begin, u32End))	return;

	/* ȭ�� ���� ���� */
	m_pGrid->SetRedraw(FALSE);

	/* ȭ�鿡 �� ���� */
	for (; u32Begin<=u32End; u32Begin++)
	{
		/* �ݵ�� �ʱ�ȭ */
		wmemset(tzVal, 0x00, 64);

		/* ���� ��ġ�� �� �ӽ� ���� */
		u16Val	= pPlcMem[u32Begin-1];

		/* ���ڿ� ���ۿ� �߰� */
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >> 15 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >> 14 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >> 13 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >> 12 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), L" ");
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >> 11 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >> 10 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >>  9 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >>  8 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), L" ");
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >>  7 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >>  6 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >>  5 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >>  4 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), L" ");
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >>  3 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >>  2 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >>  1 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val & 0x0001)]);
		/* ������ �������� ������ NULL ������ ���� */
		tzVal[34]	= 0x00;
		/* ���ڿ��� �� ��� */
		m_pGrid->SetItemText(u32Begin, 1, tzVal);
		/* 16 ���� �� ��� */
		if ((u32Begin % 2) == 1)
		{
			memcpy(&u32Val, &pPlcMem[u32Begin-1], 4);
			swprintf_s(tzVal, 64, L"%5u : %10u ", u16Val, u32Val);
			m_pGrid->SetItemText(u32Begin, 2, tzVal);
		}
		else
		{
			swprintf_s(tzVal, 64, L"%5u :            ", u16Val);
			m_pGrid->SetItemText(u32Begin, 2, tzVal);
		}
	}

	/* ȭ�� ���� Ȱ��ȭ */
	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate(FALSE);
}

/*
 desc : GridCtrl ��, PLC Memory �����Ͱ� ��µ� ���� �� ���� �� (Row)�� ���� Ŭ���� ���
 parm : None
 retn : None
*/
VOID CDlgPlcQ::OnGridClicked()
{
	UINT16 u16Addr;
	CDlgPSet dlgPSet;

	/* ���� ���õ� ��� ���� ��ȣ ��� */
	PPOINT ptSelPos	= m_pGrid->GetSelPos();
	if (!ptSelPos)	return;

	/* �޸� �ּ� �� ��� */
	u16Addr	= (UINT16)m_pGrid->GetItemTextToInt(ptSelPos->x, 0);
	/* �� ���� ��ȭ ���� ��� */
	dlgPSet.MyDoModal(u16Addr);
}