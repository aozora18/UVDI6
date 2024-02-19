
/*
 desc : Main Dialog
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgMain.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : �ڽ��� ���� ID, �θ� ���� ��ü
 retn : None
*/
CDlgMain::CDlgMain(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgMain::IDD, parent)
{
	m_bDrawBG	= 0x01;
	m_hIcon		= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgMain::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;
	/* group box */
	u32StartID	= IDC_MAIN_GRP_RESULT;
	for (i=0; i<6; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Button - Normal */
	u32StartID	= IDC_MAIN_BTN_EXIT;
	for (i=0; i<4; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Checkbox - Normal */
	u32StartID	= IDC_MAIN_CHK_ACAM1;
	for (i=0; i<4; i++)		DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
	/* Static - Normal */
	u32StartID	= IDC_MAIN_TXT_MEAS_PERIOD_ROWS;
	for (i=0; i<7; i++)		DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Edit - integer */
	u32StartID	= IDC_MAIN_EDT_MEAS_COUNT_ROWS;
	for (i=0; i<3; i++)		DDX_Control(dx, u32StartID+i,	m_edt_int[i]);
	/* edit - float */
	u32StartID	= IDC_MAIN_EDT_MEAS_PERIOD_ROWS;
	for (i=0; i<5; i++)		DDX_Control(dx, u32StartID+i,	m_edt_flt[i]);
}

BEGIN_MESSAGE_MAP(CDlgMain, CMyDialog)
	ON_WM_SYSCOMMAND()
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_BTN_EXIT, IDC_MAIN_BTN_SAVE,	OnBtnClick)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_CHK_ACAM1, IDC_MAIN_CHK_CALI_Y,OnChkClick)
END_MESSAGE_MAP()

/*
 desc : �ý��� ��ɾ� ����
 parm : id		- ���õ� �׸� ID ��
		lparam	- ???
 retn : 1 - ���� / 0 - ����
*/
VOID CDlgMain::OnSysCommand(UINT32 id, LPARAM lparam)
{
//	switch (id)
//	{
//	}

	CMyDialog::OnSysCommand(id, lparam);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::OnInitDlg()
{
	UINT32 u32Size	= 0, i = 0;

	InitCtrl();		/* ���� ��Ʈ�� �ʱ�ȭ */
	InitGrid();		/* �׸��� �ʱ�ȭ */

	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgMain::OnExitDlg()
{
	/* Grid Control �޸� ����*/
	if (m_pGrid)
	{
		m_pGrid->DestroyWindow();
		delete m_pGrid;
	}
	/* XY Calibration Data �޸� ���� */
	if (m_stCali.pos_xy)	::Free(m_stCali.pos_xy);
	m_stCali.pos_xy	= NULL;
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgMain::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgMain::OnResizeDlg()
{
}

/*
 desc : ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgMain::InitCtrl()
{
	UINT8 i	= 0x00;
#if 0
	UINT32 u32Min[6]	= { 10, 10,  10,  10,    40,    40 };
	UINT32 u32Max[6]	= { 50, 50, 100, 100, 10000, 10000 };
	DOUBLE dbMin[2]		= {   0.0f,   0.0f };
	DOUBLE dbMax[2]		= { 100.0f, 100.0f };
#endif
	/* group box */
	for (i=0; i<6; i++)	m_grp_ctl[i].SetFont(&g_lf);

	/* text - normal */
	for (i=0; i<7; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* button - normal */
	for (i=0; i<4; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
	}
	/* checkbox - normal */
	for (i=0; i<4; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf);
		m_chk_ctl[i].SetBgColor(g_clrBtnColor);
	}
	m_chk_ctl[0].SetCheck(1);
	m_chk_ctl[2].SetCheck(1);
	/* edit - integer */
	for (i=0; i<3; i++)
	{
		m_edt_int[i].SetEditFont(&g_lf);
		m_edt_int[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_int[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_int[i].SetInputType(ENM_DITM::en_int8);
		if (i != 2)	m_edt_int[i].SetReadOnly(TRUE);
	}
	/* edit - float */
	for (i=0; i<5; i++)
	{
		m_edt_flt[i].SetEditFont(&g_lf);
		m_edt_flt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_flt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_flt[i].SetInputType(ENM_DITM::en_float);
	}
	m_edt_flt[4].SetTextToNum(5.0f, 1);
}

/*
 desc : �׸��� ��Ʈ�� ���� - Step XY
 parm : None
 retn : None
*/
VOID CDlgMain::InitGrid()
{
	UINT32 dwStyle	= WS_CHILD | WS_TABSTOP | WS_VISIBLE, i = 0;

	GV_ITEM stGV = { NULL };
	CRect rGrid;

	// ���� ���� Client ũ��
	m_grp_ctl[0].GetWindowRect(rGrid);
	ScreenToClient(rGrid);
	rGrid.left	+= 15;
	rGrid.right	-= 15;
	rGrid.top	+= 24;
	rGrid.bottom-= 15;

	// �׸��� ��Ʈ�� �⺻ ���� �Ӽ�
	stGV.mask	= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr= RGB(32, 32, 32);
	/* ��ü ���� */
	m_pGrid	= new CGridCtrl;
	ASSERT(m_pGrid);
	m_pGrid->SetModifyStyleEx(WS_EX_STATICEDGE);
	m_pGrid->SetDrawScrollBarVert(TRUE);
	m_pGrid->SetDrawScrollBarHorz(TRUE);
	if (!m_pGrid->Create(rGrid, this, IDC_MAIN_GRID_CALI_TOOL, dwStyle))
	{
		delete m_pGrid;
		m_pGrid = NULL;
		return;
	}
	/* ��ü �⺻ ���� */
	m_pGrid->SetLogFont(g_lf);
	m_pGrid->SetRowCount(1);
	m_pGrid->SetColumnCount(1);
	m_pGrid->SetRowHeight(0, 24);
	m_pGrid->SetColumnWidth(0, 38);
	m_pGrid->SetBkColor(RGB(255, 255, 255));
	m_pGrid->SetFixedColumnSelection(0);

	m_pGrid->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
}

/*
 desc : �Ϲ� ��ư �̺�Ʈ ó��
 parm : id	- [in]  Click�� ��ư ID
 retn : None
*/
VOID CDlgMain::OnBtnClick(UINT32 id)
{
	switch (id)
	{
	case IDC_MAIN_BTN_EXIT			:	PostMessage(WM_CLOSE, 0, 0L);	break;
	case IDC_MAIN_BTN_DOF_FILE		:	OpenFile();						break;
	case IDC_MAIN_BTN_REFRESH		:	DrawGrid();	DrawData();			break;
	case IDC_MAIN_BTN_SAVE			:	SaveFile();						break;
	}
}

/*
 desc : �Ϲ� üũ �̺�Ʈ ó��
 parm : id	- [in]  Click�� ��ư ID
 retn : None
*/
VOID CDlgMain::OnChkClick(UINT32 id)
{
	switch (id)
	{
	case IDC_MAIN_CHK_ACAM1		:
	case IDC_MAIN_CHK_ACAM2		:
		m_chk_ctl[0].SetCheck(0);
		m_chk_ctl[1].SetCheck(0);
		m_chk_ctl[id-IDC_MAIN_CHK_ACAM1].SetCheck(1);
		break;
	case IDC_MAIN_CHK_CALI_X	:
	case IDC_MAIN_CHK_CALI_Y	:
		m_chk_ctl[2].SetCheck(0);
		m_chk_ctl[3].SetCheck(0);
		m_chk_ctl[id-IDC_MAIN_CHK_ACAM1].SetCheck(1);
		break;
	}
}

/*
 desc : ���� �׸��� ��Ʈ�ѿ� ��µ� ���� Data ���Ϸ� ���� (Camera 1 or Camera 2)
 parm : None
 retn : None
*/
VOID CDlgMain::SaveFile()
{
	UINT8 u8ACamID	= m_chk_ctl[0].GetCheck() == 1 ? 0x01 : 0x02;
	UINT16 i, j, u16Pos;
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL}, tzVal[32] = {NULL}, tzMsg[256] = {NULL};
	DOUBLE dbDiffX	= 0.0f, dbDiffY = 0.0f;
	errno_t eRet	= NULL;
	FILE *fpCali	= {NULL};
	CUniToChar csCnv;

	/* ���� �β� ���� �����Ǿ� �ִ��� ���� */
	if (m_edt_int[2].GetTextToNum() < 1)
	{
		AfxMessageBox(L"The material thickness value is not enterend", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* ���ϸ� ���� (acam_cali_thick_acam_no) */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\xy2d\\acam_cali_%04u_%u.dat",
			   g_tzWorkDir, (UINT16)m_edt_int[2].GetTextToNum(), u8ACamID);

	/* ������ ������ ������ �������� ���ϵ��� �� */
	if (uvCmn_FindFile(tzFile))
	{
		if (IDYES != AfxMessageBox(L"The same file exists\n"
								  L"Shoud I overwite the file?", MB_YESNO))	return;
		else
		{
			/* �б� ���� ���� ������ �����ϱ� ���� */
			if (!uvCmn_DeleteForceFile(tzFile))
			{
				AfxMessageBox(L"Failed to delete the file", MB_ICONSTOP|MB_TOPMOST);
				return;
			}
		}
	}

	/* Open the file */
	eRet = fopen_s(&fpCali, csCnv.Uni2Ansi(tzFile), "wt");
	if (0 != eRet)
	{
		swprintf_s(tzMsg, 256, L"Failed to open the file (err:%d)", eRet);
		AfxMessageBox(tzMsg, MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* ���� ���� ��ġ ���� ���� */
	wmemset(tzVal, 0x00, 32);	swprintf_s(tzVal, 32, L"%.4f\n", m_stCali.acam_x/10000.0f);
	fputws(tzVal, fpCali);
	wmemset(tzVal, 0x00, 32);	swprintf_s(tzVal, 32, L"%.4f\n", m_stCali.stage_x/10000.0f);
	fputws(tzVal, fpCali);
	wmemset(tzVal, 0x00, 32);	swprintf_s(tzVal, 32, L"%.4f\n", m_stCali.stage_y/10000.0f);
	fputws(tzVal, fpCali);
	/* �����ϰ��� �ϴ� �������� ���� (��� ��) ���� */
	wmemset(tzVal, 0x00, 32);	swprintf_s(tzVal, 32, L"%.4f\n", m_stCali.row_gap/10000.0f);
	fputws(tzVal, fpCali);
	wmemset(tzVal, 0x00, 32);	swprintf_s(tzVal, 32, L"%.4f\n", m_stCali.col_gap/10000.0f);
	fputws(tzVal, fpCali);
	/* �����ϰ��� �ϴ� �������� ���� (��� ��) ���� */
	wmemset(tzVal, 0x00, 32);	swprintf_s(tzVal, 32, L"%u\n", m_stCali.row_cnt);
	fputws(tzVal, fpCali);
	wmemset(tzVal, 0x00, 32);	swprintf_s(tzVal, 32, L"%u\n", m_stCali.col_cnt);
	fputws(tzVal, fpCali);

	/* ���� �β��� ���� X / Y ��ġ ���� �� ��� */
	dbDiffX	= m_edt_flt[2].GetTextToDouble() / 1000.0f;	/* um */
	dbDiffY	= m_edt_flt[3].GetTextToDouble() / 1000.0f;	/* um */

	for (i=0; i<m_stCali.row_cnt; i++)
	{
		for (j=0; j<m_stCali.col_cnt; j++)
		{
			u16Pos	= i * m_stCali.row_cnt + j;
			swprintf_s(tzVal, 32, L"%+.4f,%+.4f\n",
					   m_stCali.pos_xy[u16Pos].x/10000.0f + dbDiffX,
					   m_stCali.pos_xy[u16Pos].y/10000.0f + dbDiffY);
			if (EOF == fputws(tzVal, fpCali))
			{
				AfxMessageBox(L"Failed to save to the calibration file", MB_ICONSTOP|MB_TOPMOST);
				if (0 == eRet)	fclose(fpCali);
				return;
			}
		}
	}

	/* ���� �ݱ�  */
	if (0 == eRet)	fclose(fpCali);
}

/*
 desc : Calibration �����Ͱ� ����� Data ���� ����
 parm : None
 retn : None
*/
VOID CDlgMain::OpenFile()
{
	TCHAR tzInit[MAX_PATH_LEN]	= {NULL};
	PTCHAR ptzFile	= NULL;

	/* �ʱ� ���� ���� */
	ptzFile	= (PTCHAR)::Alloc(sizeof(TCHAR)*MAX_PATH_LEN);
	wmemset(ptzFile, 0x00, MAX_PATH_LEN);
	swprintf_s(tzInit, MAX_PATH_LEN, L"%s\\xy2d", g_tzWorkDir);
	if (uvCmn_GetSelectFile(L"Cali File (*.dat) | *.dat", MAX_PATH_LEN, ptzFile,
							L"XY2D Calibration File", tzInit))
	{
		/* ���� ���� ���� */
		if (!LoadData(ptzFile))	return;
	}
	::Free(ptzFile);
	/* �׸��� ���� */
	DrawGrid();
	/* �׸��忡 ������ �� ���� */
	DrawData();
}

/*
 desc : DOF Film ������ ����� Data ������ �׸��忡 ����
 parm : file	- [in]  2D Calibration Data File
 retn : TRUE or FALSE
*/
BOOL CDlgMain::LoadData(PTCHAR file)
{
	TCHAR *ptzVal, tzVal[32], tzFile[MAX_PATH_LEN] = {NULL}, *ptzContext;
	INT32 i, i32Total;
	errno_t eRet	= {NULL};
	LPG_I32XY pstXY	= NULL;
	FILE *fpCali	= {NULL};
	CUniToChar csCnv;

	/* XY Calibration Data �޸� ���� */
	if (m_stCali.pos_xy)	::Free(m_stCali.pos_xy);
	m_stCali.pos_xy	= NULL;

	/* Open the file */
	eRet = fopen_s(&fpCali, csCnv.Uni2Ansi(file), "rt");
	if (0 != eRet)	return FALSE;

	/* Align Camera X : Motion Position */
	wmemset(tzVal, 0x00, 32);	fgetws(tzVal, 32, fpCali);
	m_stCali.acam_x	= (INT32)ROUNDED(_wtof(tzVal) * 10000.0f, 0);
	/* Stage X : Motion Position */
	wmemset(tzVal, 0x00, 32);	fgetws(tzVal, 32, fpCali);
	m_stCali.stage_x	= (INT32)ROUNDED(_wtof(tzVal) * 10000.0f, 0);
	/* Stage Y : Motion Position */
	wmemset(tzVal, 0x00, 32);	fgetws(tzVal, 32, fpCali);
	m_stCali.stage_y	= (INT32)ROUNDED(_wtof(tzVal) * 10000.0f, 0);
	/* ��� ���� ���� ���� */
	wmemset(tzVal, 0x00, 32);	fgetws(tzVal, 32, fpCali);
	m_stCali.row_gap	= (INT32)ROUNDED(_wtof(tzVal) * 10000.0f, 0);
	wmemset(tzVal, 0x00, 32);	fgetws(tzVal, 32, fpCali);
	m_stCali.col_gap	= (INT32)ROUNDED(_wtof(tzVal) * 10000.0f, 0);
	/* ��� ���� ���� ���� */
	wmemset(tzVal, 0x00, 32);	fgetws(tzVal, 32, fpCali);
	m_stCali.row_cnt	= (UINT32)_wtoi(tzVal);
	wmemset(tzVal, 0x00, 32);	fgetws(tzVal, 32, fpCali);
	m_stCali.col_cnt	= (UINT32)_wtof(tzVal);

	/* �޸� �Ҵ� */
	i32Total		= UINT32(m_stCali.col_cnt) * UINT32(m_stCali.row_cnt);
	m_stCali.pos_xy= (LPG_I32XY)::Alloc(i32Total * sizeof(STG_I32XY));
	ASSERT(m_stCali.pos_xy);
	memset(m_stCali.pos_xy, 0x00, sizeof(STG_I32XY) * i32Total);
	/* ���� ���� ����� ����ü ������ ���� */
	pstXY	= m_stCali.pos_xy;
	/* �� ���� ī�޶��� Calibration ��ġ �� ��� */
	for (i=0; i<i32Total; i++)
	{
		wmemset(tzVal, 0x00, 32);
		/* ���پ� �б� (�о���̰��� �ϴ� ������ �� + 1 (/n; ���๮�� ������) */
		if (!fgetws(tzVal, 32, fpCali))	break;
		/* �ƹ��� �����Ͱ� �������� �ʴ´��� Ȯ�� */
		if (wcslen(tzVal) < 1)	break;
		/* Align Camera X, Stage Y ���� �� ���� */
		ptzVal		= wcstok_s(tzVal, L",", &ptzContext);
		pstXY[i].x	= (INT32)ROUNDED(_wtof(ptzVal) * 10000.0f, 0);
		ptzVal		= wcstok_s(NULL, L",", &ptzContext);
		pstXY[i].y	= (INT32)ROUNDED(_wtof(ptzVal) * 10000.0f, 0);

		/* ������ ������ �о�鿴���� */
		if (feof(fpCali))	break;
	}

	/* ���������� ��� �о�鿴���� ���� */
	return (i == i32Total);
}

/*
 desc : �׸��� ���� �缳��
 parm : None
 retn : None
*/
VOID CDlgMain::DrawGrid()
{
	UINT8 i;
	GV_ITEM stGV= { NULL };

	/* ���� ��� ���� �̵� �Ÿ� */
	m_edt_flt[0].SetTextToNum(m_stCali.row_gap/10000.0f, 4);
	m_edt_flt[1].SetTextToNum(m_stCali.col_cnt/10000.0f, 4);
	/* ���� ��� ���� ���� ���� */
	m_edt_int[0].SetTextToNum(m_stCali.row_cnt);
	m_edt_int[1].SetTextToNum(m_stCali.col_cnt);
	/* ��� ���� ������ �ּ� 2�� �̻��� �־�� �� */
	if (m_stCali.row_cnt < 2 || m_stCali.col_cnt < 2)
	{
		AfxMessageBox(L"There must be at least two rows and columns", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* �׸��� ���� ���� ��Ȱ��ȭ */
	m_pGrid->SetRedraw(FALSE);
	/* ������ ���� ��� ���� */
	m_pGrid->DeleteNonFixedRows();
	
	/* ��� ���� ���� ���� */
	m_pGrid->SetRowCount(m_stCali.row_cnt+1);
	m_pGrid->SetColumnCount(m_stCali.col_cnt+1);

	/* Ÿ��Ʋ ���� */
	stGV.mask	= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr= RGB(32, 32, 32);
	stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	stGV.crBkClr= RGB(214, 214, 214);
	stGV.row	= 0;
	stGV.col	= 0;
	stGV.strText= L"X / Y";
	stGV.lfFont	= g_lf;
	m_pGrid->SetItem(&stGV);
	m_pGrid->SetColumnWidth(0, 30);

	/* Ÿ��Ʋ ��� �� ���� */
	for (i=1,stGV.col=0; i<=m_stCali.row_cnt; i++)
	{
		stGV.row	= i;
		stGV.strText.Format(L"%02u", i);
		m_pGrid->SetItem(&stGV);
		m_pGrid->SetRowHeight(i, 22);
	}
	for (i=1,stGV.row=0; i<=m_stCali.col_cnt; i++)
	{
		stGV.col	= i;
		stGV.strText.Format(L"%02u", i);
		m_pGrid->SetItem(&stGV);
		m_pGrid->SetColumnWidth(i, 45);
	}

	/* �׸��� ���� ���� Ȱ��ȭ */
	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate(FALSE);
}

/*
 desc : �׸��� ������ �� ����
 parm : None
 retn : None
*/
VOID CDlgMain::DrawData()
{
	UINT8 u8XY		= m_chk_ctl[2].GetCheck() == 1 ? 0x00 : 0x01;
	UINT16 i, j, u16Pos;
	DOUBLE dbPosXY	= 0.0f, dbDiffXY = 0.0f;
	TCHAR tzVal[32]	= { NULL };
	GV_ITEM stGV	= { NULL };

	/* Offset XY �� ���� */
	if (0x00 == u8XY)	dbDiffXY = m_edt_flt[2].GetTextToDouble();
	else				dbDiffXY = m_edt_flt[3].GetTextToDouble();

	/* ���� ��� �� ���� */
	stGV.mask	= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr= RGB(16, 16, 16);
	stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
	stGV.crBkClr= RGB(255, 255, 255);

	/* �׸��� ���� ���� ��Ȱ��ȭ */
	m_pGrid->SetRedraw(FALSE);

	for (i=1; i<=m_stCali.row_cnt; i++)
	{
		for (j=1; j<=m_stCali.col_cnt; j++)
		{
			stGV.row= i;
			stGV.col= j;
			u16Pos	= (i-1) * m_stCali.col_cnt + (j-1);
			
			wmemset(tzVal, 0x00, 32);
			if (0x00 == u8XY)	dbPosXY	= (m_stCali.pos_xy[u16Pos].x / 10.0f) + dbDiffXY;
			else				dbPosXY	= (m_stCali.pos_xy[u16Pos].y / 10.0f) + dbDiffXY;

			if (abs(dbPosXY) > m_edt_flt[4].GetTextToDouble())
			{
				stGV.crFgClr	= RGB(255, 128, 0);
			}
			else
			{
				stGV.crFgClr	= RGB(0, 0, 0);
			}
			swprintf_s(tzVal, 32, L"%+.1f", dbPosXY);
			stGV.strText	= tzVal;
			m_pGrid->SetItem(&stGV);
		}
	}

	/* �׸��� ���� ���� Ȱ��ȭ */
	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate(FALSE);
}