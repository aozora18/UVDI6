
/*
 desc : Align Camera - Edge Find ���
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgFind.h"

#include "../Meas/Measure.h"

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
CDlgFind::CDlgFind(UINT32 id, CWnd* parent)
	: CDlgMenu(id, parent)
{
	m_pGrid	= NULL;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDlgFind::~CDlgFind()
{
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgFind::DoDataExchange(CDataExchange* dx)
{
	CDlgMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Button */
	u32StartID	= IDC_FIND_BTN_REFRESH;
	for (i=0; i<3; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgFind, CDlgMenu)
	ON_BN_CLICKED(IDC_MENU_GRID_EDGE_RESULT,	OnGridClicked)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_FIND_BTN_REFRESH, IDC_FIND_BTN_SAVE, OnBnClicked)
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgFind::PreTranslateMessage(MSG* msg)
{

	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgFind::OnInitDlg()
{
	if (!CDlgMenu::OnInitDlg())	return FALSE;

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
VOID CDlgFind::OnExitDlg()
{
	CDlgMenu::OnExitDlg();

	/* GridCtrl ���� */
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
VOID CDlgFind::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgFind::OnResizeDlg()
{
}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgFind::InitCtrl()
{
	INT32 i;

	/* Button - normal */
	for (i=0; i<3; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
	}
}
/*
 desc : ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgFind::InitGrid()
{
	TCHAR tzCols[7][16]	= { L"Z (mm)", L"Strength", L"Length", L"Diameter", L"Files", L"Find", L"Rep" }, tzRows[32] = {NULL};
	INT32 i32Width[7]	= { 60, 85, 75, 80, 70, 40, 35 }, i, j=1;
	INT32 i32ColCnt		= 7, i32RowCnt = 0;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	LPG_CIEA pstCfg		= uvEng_GetConfig();
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* Last Recv. Packet / PDO Header / PDO Data (Values) */
	::GetWindowRect(m_hWnd, rGrid);
	ScreenToClient(rGrid);
	rGrid.left	+= 5;
	rGrid.right	-= 5;
	rGrid.top	+= 5;
	rGrid.bottom-= 60;

	/* �׸��� ��Ʈ�� �⺻ ���� �Ӽ� */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* ��ü ���� */
	m_pGrid	= new CGridCtrl;
	ASSERT(m_pGrid);
	m_pGrid->SetDrawScrollBarHorz(FALSE);
	m_pGrid->SetDrawScrollBarVert(TRUE);
	if (!m_pGrid->Create(rGrid, this, IDC_MENU_GRID_EDGE_RESULT, dwStyle))
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
	m_pGrid->SetRowHeight(0, 25);
	m_pGrid->SetFixedColumnCount(0);
	m_pGrid->SetBkColor(RGB(255, 255, 255));
	m_pGrid->SetFixedColumnSelection(0);

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
		m_pGrid->SetItem(&stGV);
		m_pGrid->SetColumnWidth(i, i32Width[i]);
	}

	m_pGrid->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGrid->UpdateGrid();
}

/*
 desc : ���� ���� ��Ʈ�� ���� ����
 parm : None
 retn : None
*/
VOID CDlgFind::UpdateCtrl()
{
	UINT8 i	= 0x00;
	BOOL bEnable	= m_pDlgMain->IsRunFocus();
	/* ��ư ��Ʈ�� */
	for (i=0x00; i<3; i++)	m_btn_ctl[i].EnableWindow(!bEnable);
}

/*
 desc : GridCtrl Ŭ�� �̺�Ʈ �߻��� ���
 parm : None
 retn : None
*/
VOID CDlgFind::OnGridClicked()
{
	TCHAR tzFile[MAX_FILE_LEN]	= {NULL};
	CMeasure *pMeasure	= m_pDlgMain->GetMeasure();

	/* ���� ���õ� ��� ���� ��ȣ ��� */
	PPOINT ptPos	= m_pGrid->GetSelPos();
	if (!ptPos)	return;

	/* 4 ��° �÷��� ���� �̸� ��� */
	swprintf_s(tzFile, MAX_FILE_LEN, L"%s", m_pGrid->GetItemText(ptPos->x, 4).GetBuffer());
	if (wcslen(tzFile) > 0)	pMeasure->SelectEdgeFile(tzFile);
}

/*
 desc : ���� ��µ� ������ ��� ����
 parm : None
 retn : None
*/
VOID CDlgFind::ResetData()
{
	/* ���� ��Ȱ��ȭ */
	m_pGrid->SetRedraw(FALSE);

	/* Ÿ��Ʋ �κи� �����ϰ� ��� ���� */
	m_pGrid->DeleteNonFixedRows();

	/* ���� Ȱ��ȭ */
	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate(FALSE);
}

/*
 desc : ��ư �̺�Ʈ ID
 parm : id	- [in]  ��ư ��Ʈ�� ID
 retn : None
*/
VOID CDlgFind::OnBnClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_FIND_BTN_REFRESH	:	RefreshData();	break;
	case IDC_FIND_BTN_FIND		:	GetEdgeData();	break;
	case IDC_FIND_BTN_SAVE		:	SaveToOpen();	break;
	}
}

/*
 desc : ������� ������ ��� ���� ��������
 parm : None
 retn : None
*/
VOID CDlgFind::RefreshData()
{
	TCHAR tzValue[128]	= {NULL};
	INT32 i32Item		= 0;
	UINT32 i, u32Count	= 0, u32Format	= DT_VCENTER | DT_SINGLELINE;;
	LPG_ZAAL pstData	= NULL;
	CMeasure *pMeasure	= m_pDlgMain->GetMeasure();

	/* ���� ��Ȱ��ȭ */
	m_pGrid->SetRedraw(FALSE);

	/* Ÿ��Ʋ �κи� �����ϰ� ��� ���� */
	m_pGrid->DeleteNonFixedRows();

	/* ���� ��ϵ� ���� ���� ��� */
	u32Count = pMeasure->GetCount();
	for (i=0; i<u32Count; i++)
	{
		pstData	= m_pDlgMain->GetGrabData(i);
		if (!pstData)	break;

		/* Z �� ���� �� */
		i32Item	= m_pGrid->GetRowCount();
		m_pGrid->InsertRow(i32Item);
		m_pGrid->SetItemDouble(i32Item, 0, pstData->z_pos / 10000.0f, 4);
		m_pGrid->SetItemFormat(i32Item, 0, u32Format|DT_RIGHT);

		/* Strength */
		wmemset(tzValue, 0x00, 128);
		swprintf_s(tzValue, 128, L"%.3f", pstData->avg_value[0]);
//		swprintf_s(tzData, 128, L"%s (%.2f)", tzValue, pstData->std_value[0]);
		m_pGrid->SetItemText(i32Item, 1, tzValue);
		m_pGrid->SetItemFormat(i32Item, 1, u32Format|DT_RIGHT);
		if (pstData->set_value[0])	m_pGrid->SetItemBkColour(i32Item, 1, RGB(224, 224, 244));

		/* Length */
		wmemset(tzValue, 0x00, 128);
		swprintf_s(tzValue, 128, L"%.3f", pstData->avg_value[1]);
//		swprintf_s(tzData, 128, L"%s (%.3f)", tzValue, pstData->std_value[1]);
		m_pGrid->SetItemText(i32Item, 2, tzValue);
		m_pGrid->SetItemFormat(i32Item, 2, u32Format|DT_RIGHT);
		if (pstData->set_value[1])	m_pGrid->SetItemBkColour(i32Item, 2, RGB(224, 224, 244));

		/* Feret */
		wmemset(tzValue, 0x00, 128);
		swprintf_s(tzValue, 128, L"%.3f", pstData->avg_value[2]);
//		swprintf_s(tzData, 128, L"%s (%.3f)", tzValue, pstData->std_value[2]);
		m_pGrid->SetItemText(i32Item, 3, tzValue);
		m_pGrid->SetItemFormat(i32Item, 3, u32Format|DT_RIGHT);
		if (pstData->set_value[2])	m_pGrid->SetItemBkColour(i32Item, 3, RGB(224, 224, 244));

		m_pGrid->SetItemText(i32Item, 4, pstData->file);
		m_pGrid->SetItemFormat(i32Item, 4, u32Format|DT_CENTER);

		m_pGrid->SetItemText(i32Item, 5, (INT32)pstData->find_count);
		m_pGrid->SetItemFormat(i32Item, 5, u32Format|DT_RIGHT);

		m_pGrid->SetItemText(i32Item, 6, (UINT32)pstData->arrValue[0].GetCount());
		m_pGrid->SetItemFormat(i32Item, 6, u32Format|DT_CENTER);
	}

	/* ���� Ȱ��ȭ */
	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate(FALSE);
}

/*
 desc : ���� ������ ���� (�׽�Ʈ)
 parm : None
 retn : None
*/
VOID CDlgFind::GetEdgeData()
{
	UINT8 u8ACamNo		= m_pDlgMain->GetCheckACam();
	UINT64 u64Tick		= 0;
	LPG_EDFR pstResult	= NULL;
	CMeasure *pMeasure	= m_pDlgMain->GetMeasure();

	/* ���õ� ī�޶� ������ Ȯ�� */
	if (u8ACamNo < 1)	return;

	/* Trigger ���� ���� */
	m_pDlgMain->UpdateTrigParam();

	/* ���� Grabbed Image ��� �ʱ�ȭ */
	uvEng_Camera_ResetGrabbedImage();
	/* ����� ī�޶� ��� ���� */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_edge_mode);
	/* Ʈ���� 1�� �߻� */
	if (!uvEng_Trig_ReqTrigOutOne(u8ACamNo, FALSE))
	{
		AfxMessageBox(L"Failed to generate the trigger event", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* ���� ��� ���� �ð� ��� ���� */
	u64Tick	= GetTickCount64();

	/* ���� �ð� ���� ������ �����Ͱ� �ִ��� Ȯ�� */
	do {
		/* �� 3�� ���� ������ ������ ���� ������ */
#ifdef _DEBUG
		if (GetTickCount64() > u64Tick + 10000)	break;
#else
		if (GetTickCount64() > u64Tick + 3000)	break;
#endif
		/* ���� ���� ó�� ���� (������ �̹��� ������ ����) */
		if (uvEng_Camera_RunEdgeDetect(u8ACamNo))
		{
			/* ���� ���� ���� */
			pstResult	= uvEng_Camera_GetEdgeDetectResults(u8ACamNo);
			break;	/* �ٷ� ���������� */
		}

	} while (1);
	
	/* �˻��� ���� ���� ����� ... */
	if (!pstResult)
	{
		AfxMessageBox(L"Failed to find the edge detection", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* ���� ������ �м� */
	if (!pMeasure->SetResult(u8ACamNo))
	{
		AfxMessageBox(L"Failed to analyze edge detection", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* ���� �ֱ� ���� �����ϵ��� �� */
	pMeasure->SelectLastEdgeFile();

	/* ��� ������ ��� */
	RefreshData();
}

/*
 desc : �׸��� ��Ʈ�ѿ� �ִ� �����͸� ���Ϸ� ����
 parm : None
 retn : None
*/
VOID CDlgFind::SaveToOpen()
{
	TCHAR tzTempFile[MAX_PATH_LEN]	= {NULL};
	SYSTEMTIME stTm	= {NULL};

	/* ��ϵ� �����Ͱ� ������ Ȯ�� */
	if (m_pGrid->GetRowCount() < 2)	return;

	/* ���� �ð� */
	GetLocalTime(&stTm);
	/* ���� �ӽ� ���Ϸ� ���� �� ���� */
	swprintf_s(tzTempFile, MAX_PATH_LEN, L"%s\\vdof\\vdof_%02u%02u%02u_%02u%02u%02u.csv",
			   g_tzWorkDir, stTm.wYear-2000, stTm.wMonth, stTm.wDay,
			   stTm.wHour, stTm.wMinute, stTm.wSecond);

	/* ���Ϸ� ���� */
	m_pGrid->Save(tzTempFile);

	/* ����� ���� ���� */
	ShellExecute(NULL, _T("open"), _T("excel.exe"), tzTempFile, NULL, SW_SHOW);
}