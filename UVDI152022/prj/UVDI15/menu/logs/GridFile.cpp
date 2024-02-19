/*
 ���� : Logs Grid Control
 */

#include "pch.h"
#include "../../MainApp.h"
#include "GridFile.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                       Base Grid Control                                       */
/* --------------------------------------------------------------------------------------------- */

/*
 ���� : �ı���
 ���� : None
 ��ȯ : None
*/
CGridFile::~CGridFile()
{
	if (m_pGrid)
	{
		if (m_pGrid->GetSafeHwnd())	m_pGrid->DestroyWindow();
		delete m_pGrid;
	}
}

/*
 ���� : �ʱ�ȭ (����)
 ���� : None
 ��ȯ : TRUE or FALSE
*/
BOOL CGridFile::Init()
{
	/* ���� ���� Client ũ�� */
	m_pCtrl->GetWindowRect(m_rGrid);
	m_pParent->ScreenToClient(m_rGrid);

	CResizeUI clsResizeUI;
	/* Grid ũ�� �缳�� */
	m_rGrid.left	+= (long)(15 *	clsResizeUI.GetRateX());
	m_rGrid.top		+= (long)(22 *	clsResizeUI.GetRateX());
	m_rGrid.right	-= (long)(15 *	clsResizeUI.GetRateX());
	m_rGrid.bottom	-= (long)(136 * clsResizeUI.GetRateX());

	// by sysandj : Grid Size ����
	INT32 i32Width[2] = { 121, 45};
	int nWidth = 0;
	int nResizeWidth = 0;
	for (int i = 0; i < _countof(i32Width); i++)
	{
		nWidth += i32Width[i];
	}

	for (int i = 0; i < _countof(i32Width); i++)
	{
		i32Width[i] += (m_rGrid.Width() - nWidth - ::GetSystemMetrics(SM_CXVSCROLL)) / _countof(i32Width);
		nResizeWidth += i32Width[i];
	}
	i32Width[0] += m_rGrid.Width() - ::GetSystemMetrics(SM_CXVSCROLL) - nResizeWidth - 2;
	// by sysandj : Grid Size ����

	/* ��ü ���� */
	m_pGrid = new CGridCtrl;
	ASSERT(m_pGrid);
	m_pGrid->SetDrawScrollBarVert(TRUE);
	m_pGrid->SetDrawScrollBarHorz(FALSE);
	if (!m_pGrid->Create(m_rGrid, m_pParent, IDC_GRID_GERB_GERBER_LIST, m_u32Style))
	{
		delete m_pGrid;
		return FALSE;
	}

	/* ��ü �⺻ ���� */
	m_pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	m_pGrid->SetColumnCount(2);
	m_pGrid->SetBkColor(RGB(255, 255, 255));

	/* ���� */
	// by sysandj : Grid Size ����
	//m_pGrid->SetColumnWidth(0, 121);
	//m_pGrid->SetColumnWidth(1, 45);
	m_pGrid->SetColumnWidth(0, i32Width[0]);
	m_pGrid->SetColumnWidth(1, i32Width[1]);
	// by sysandj : Grid Size ����

	/* �⺻ �Ӽ� �� ���� */
	m_pGrid->SetBaseGridProp(0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGrid->Refresh();

	/* �Ź� ������ ���� */
	LoadFiles();

	return TRUE;
}

/*
 ���� : ���� ���õ� ���� �̸� ��ȯ
 ���� : None
 ��ȯ : ���ϸ� ��ȯ
*/
PTCHAR CGridFile::GetSelect()
{
	UINT32 i	= 0, u32State = 0;

	wmemset(m_tzFile, 0x00, MAX_FILE_LEN);

	for (; i<(UINT32)m_pGrid->GetRowCount(); i++)
	{
		/* ���� ��ġ �׸� �Ӽ� �� ��� */
		u32State	= m_pGrid->GetItemState(i, 0);
		if (GVIS_SELECTED == (u32State & GVIS_SELECTED))
		{
			swprintf_s(m_tzFile, MAX_FILE_LEN, L"%s.log", m_pGrid->GetItemText(i, 0).GetBuffer());
			return m_tzFile;
		}
	}

	return NULL;
}

/*
 ���� : ���� ���õ� �׸��� �ִ��� ����
 ���� : None
 ��ȯ : TRUE or FALSE
*/
BOOL CGridFile::IsSelected()
{
	UINT32 i	= 0;
	for (; i<(UINT32)m_pGrid->GetRowCount(); i++)
	{
		/* ���� ��ġ �׸� �Ӽ� �� ��� */
		if (GVIS_SELECTED == (m_pGrid->GetItemState(i, 0) & GVIS_SELECTED))	return TRUE;
	}

	return FALSE;
}

/*
 ���� : �α� ���� ����
 ���� : None
 ��ȯ : None
*/
VOID CGridFile::LoadFiles()
{
	UINT16 i		= 0;
	INT32 i32Files	= 0, i32Size = 0;
	TCHAR tzDir[MAX_PATH_LEN]	= {NULL}, tzFile[MAX_PATH_LEN] = {NULL};
	CString strFile;
	CStringArray arrFiles;

	/* Search the log files in log directory */
	swprintf_s(tzDir, MAX_PATH_LEN, L"%s\\logs", g_tzWorkDir);
	uvCmn_SearchFile(tzDir, L"*.log", arrFiles);
	/* �˻��� ���� ������ ���ٸ� */
	i32Files	= (INT32)arrFiles.GetCount();
	/* ȭ�� ���� ��Ȱ��ȭ */
	m_pGrid->SetRedraw(FALSE);
	if (i32Files > 0)
	{
		/* �׸� ������ŭ Row ���� */
		m_pGrid->SetRowCount(i32Files);
		/* ��ϵ� �׸� �ٽ� ���� */
		for (; i<i32Files; i++)
		{
			m_pGrid->SetRowHeight(i, 24);

			/* ���� ��� */
			strFile = arrFiles.GetAt(i);
			/* File Name */
			m_pGrid->SetItemText(i, 0, strFile.Left(strFile.GetLength() - 4 /* .log */));
			m_pGrid->SetItemFgColour(i, 0, m_stGV.crFgClr);
			m_pGrid->SetItemBkColour(i, 0, m_stGV.crBkClr);
			m_pGrid->SetItemFormat(i, 0, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			/* File Size (KBytes) */
			swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\logs\\%s", g_tzWorkDir, strFile.GetBuffer());
			i32Size = (INT32)ROUNDED(uvCmn_GetFileSize(tzFile) / 1024.0f, 0);
			m_pGrid->SetItemText(i, 1, i32Size, 1);
			m_pGrid->SetItemFgColour(i, 1, m_stGV.crFgClr);
			m_pGrid->SetItemBkColour(i, 1, m_stGV.crBkClr);
			m_pGrid->SetItemFormat(i, 1, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		}
	}
	/* ȭ�� ���� Ȱ��ȭ */
	m_pGrid->SetRedraw(TRUE, TRUE);
	/* �׸��� ��Ʈ�� ���� */
	m_pGrid->Invalidate();
}

/*
 ���� : ���� ���õ� ���� ���� ����
 ���� : None
 ��ȯ : None
*/
VOID CGridFile::DeleteFiles()
{
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};

	if (!IsSelected())	return;

	/* ȭ�� ���� ��Ȱ��ȭ */
	m_pGrid->SetRedraw(FALSE);

	/* ���� ���� */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\logs\\%s", g_tzWorkDir, GetSelect());
	DeleteFile(tzFile);

	/* ȭ�� ���� Ȱ��ȭ */
	m_pGrid->SetRedraw(TRUE, TRUE);
	/* �׸��� ��Ʈ�� ���� */
	m_pGrid->Invalidate();

	/* �ٽ� �α� ���� ���� */
	LoadFiles();
}
