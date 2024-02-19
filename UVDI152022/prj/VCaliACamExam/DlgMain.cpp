
/*
 desc : Main Dialog
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgMain.h"

#include "MainThread.h"

#define ONE_SCAN_WIDTH	20.6f	/* unit : mm */

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
	m_bDrawBG		= 0x01;
	m_hIcon			= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_u8ViewMode	= 0x00;
	m_pMainThread	= NULL;
	m_bSetThick		= FALSE;
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
	u32StartID	= IDC_MAIN_GRP_VIEW;
	for (i=0; i<13; i++)	DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* For Image */
	u32StartID	= IDC_MAIN_PIC_VIEW;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_pic_ctl[i]);
	/* Check - Normal */
	u32StartID	= IDC_MAIN_CHK_MC2_DRV_0;
	for (i=0; i<10; i++)	DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
	/* Button - Normal */
	u32StartID	= IDC_MAIN_BTN_MC2_MOVE_UP;
	for (i=0; i<17; i++)	DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Static - Normal */
	u32StartID	= IDC_MAIN_TXT_MC2_MOVE;
	for (i=0; i<25; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* edit - float */
	u32StartID	= IDC_MAIN_EDT_MC2_DIST;
	for (i=0; i<16; i++)	DDX_Control(dx, u32StartID+i,	m_edt_flt[i]);
	/* Edit - integer */
	u32StartID	= IDC_MAIN_EDT_COUNT_ROWS;
	for (i=0; i<6; i++)		DDX_Control(dx, u32StartID+i,	m_edt_int[i]);
}

BEGIN_MESSAGE_MAP(CDlgMain, CMyDialog)
	ON_MESSAGE(WM_MAIN_THREAD,	OnMsgMainThread)
	ON_WM_SYSCOMMAND()
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_BTN_MC2_MOVE_UP, IDC_MAIN_BTN_EXIT,	OnBtnClick)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_CHK_MC2_DRV_0, IDC_MAIN_CHK_ACAM_NO,	OnChkClick)
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

	/* ���̺귯�� �ʱ�ȭ */
	if (!InitLib())		return FALSE;

	InitCtrl();		/* ���� ��Ʈ�� �ʱ�ȭ */
	LoadParam();	/* ���� �ֱٿ� ������ �� �ҷ����� */
	InitGrid();		/* �׸��� �ʱ�ȭ */
	InitSetup();

	/* Main Thread ���� (!!! �߿�. InitLib �� �����ϸ�, �ݵ�� �����ؾ� �� !!!) */
	m_pMainThread	= new CMainThread(m_hWnd);
	ASSERT(m_pMainThread);
	if (!m_pMainThread->CreateThread(0, NULL, 100/*NORMAL_THREAD_SPEED*/))
	{
		delete m_pMainThread;
		m_pMainThread	= NULL;
	}

	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgMain::OnExitDlg()
{
	UINT32 i	= 0;
	/* �⺻ ���� ������ �޸� ���� */
	if (m_pMainThread)
	{
		m_pMainThread->Stop();
		while (m_pMainThread->IsBusy())	m_pMainThread->Sleep();
		delete m_pMainThread;
		m_pMainThread	= NULL;
	}
	/* ���̺귯�� ���� */
	CloseLib();
	/* Grid Control �޸� ����*/
	if (m_pGridMeas)
	{
		for (i=0; i<2; i++)	m_pGridMeas[i].DestroyWindow();
		delete [] m_pGridMeas;
	}
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgMain::OnPaintDlg(CDC *dc)
{
	ENG_VCCM enMode	= uvEng_Camera_GetCamMode();
	RECT rDraw;

	/* �̹����� ��µ� ���� ���� */
	m_pic_ctl[0].GetWindowRect(&rDraw);
	ScreenToClient(&rDraw);

	/* ���� Calibration Mode�� ��� */
	if (enMode == ENG_VCCM::en_cali_mode)
	{
		uvEng_Camera_DrawGrabExamBitmap(dc->m_hDC, rDraw,
										m_pMainThread->GetGrabbedResult(0x00),
										m_pMainThread->GetGrabbedResult(0x01));
	}
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
 desc : Library Init
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::InitLib()
{
	return uvEng_Init(ENG_ERVM::en_cali_exam);
}

/*
 desc : Library Close
 parm : None
 retn : None
*/
VOID CDlgMain::CloseLib()
{
	uvEng_Close();
}

/*
 desc : ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgMain::InitCtrl()
{
	INT32 i;

	/* group box */
	for (i=0; i<13; i++)	m_grp_ctl[i].SetFont(&g_lf);

	/* text - normal */
	for (i=0; i<25; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* button - normal */
	for (i=0; i<17; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
	}
	/* checkbox */
	for (i=0; i<10; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf);
		m_chk_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
	/* edit - integer */
	for (i=0; i<6; i++)
	{
		m_edt_int[i].SetEditFont(&g_lf);
		m_edt_int[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_int[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_int[i].SetInputType(ENM_DITM::en_int8);
	}
	/* edit - float */
	for (i=0; i<16; i++)
	{
		m_edt_flt[i].SetEditFont(&g_lf);
		m_edt_flt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_flt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_flt[i].SetInputType(ENM_DITM::en_float);
	}
}

/*
 desc : �ʱ� �� ����
 parm : None
 retn : None
*/
VOID CDlgMain::InitSetup()
{
	UINT32 i	= 0;
	LPG_CMSI pstMC2	= &uvEng_GetConfig()->mc2_svc;
	LPG_CSCI pstCAM	= &uvEng_GetConfig()->set_cams;

	/* �ϴ� ��� Disable */
	for (i=0x00; i<MAX_MC2_DRIVE; i++)	m_chk_ctl[i].EnableWindow(FALSE);
#ifdef _DEBUG
	/* for MC2 - üũ ��ư Ȱ��ȭ ó�� */
	for (i=0; i<pstMC2->drive_count; i++)
	{
		m_chk_ctl[pstMC2->axis_id[i]].EnableWindow(TRUE);
	}
#endif
	/* Read Only */
//	for (i=2; i<5; i++)		m_edt_int[i].SetMouseClick(FALSE);
//	for (i=2; i<9; i++)		m_edt_flt[i].SetMouseClick(FALSE);
//	for (i=10; i<12; i++)	m_edt_flt[i].SetMouseClick(FALSE);

	/* ���� ȯ�濡 �´� �˻� �� ��� */
	SetRegistModel();
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
	CRect rGrid[2];

	// ���� ���� Client ũ��
	m_grp_ctl[1].GetWindowRect(rGrid[0]);
	ScreenToClient(rGrid[0]);
	rGrid[0].left	+= 15;
	rGrid[0].right	-= 15;
	rGrid[0].top	+= 24;
	rGrid[0].bottom	-= 15;
	rGrid[1].bottom	= rGrid[0].bottom;
	rGrid[0].bottom	= rGrid[0].top + (rGrid[0].Height() / 2 - 6);
	rGrid[1].left	= rGrid[0].left;
	rGrid[1].right	= rGrid[0].right;
	rGrid[1].top	+= rGrid[0].bottom + 12;

	// �׸��� ��Ʈ�� �⺻ ���� �Ӽ�
	stGV.mask	= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr= RGB(32, 32, 32);
	/* ��ü ���� */
	m_pGridMeas	= new CGridCtrl[2];
	ASSERT(m_pGridMeas);
	for (i=0; i<2; i++)
	{
		m_pGridMeas[i].SetModifyStyleEx(WS_EX_STATICEDGE);
		m_pGridMeas[i].SetDrawScrollBarVert(TRUE);
		m_pGridMeas[i].SetDrawScrollBarHorz(TRUE);
		if (!m_pGridMeas[i].Create(rGrid[i], this, IDC_MAIN_GRID_OFFSET_X+i, dwStyle))
		{
			delete m_pGridMeas;
			m_pGridMeas = NULL;
			return;
		}
		/* ��ü �⺻ ���� */
		m_pGridMeas[i].SetLogFont(g_lf);
		m_pGridMeas[i].SetRowCount(1);
		m_pGridMeas[i].SetColumnCount(1);
		m_pGridMeas[i].SetRowHeight(0, 25);
		m_pGridMeas[i].SetColumnWidth(0, 41);
		m_pGridMeas[i].SetBkColor(RGB(255, 255, 255));
		m_pGridMeas[i].SetFixedColumnSelection(0);

		m_pGridMeas[i].SetBaseGridProp(1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	}

	/* Updates the row and column of grid control */
	ReBuildGridXY();
}

/*
 desc : �̹��� �� ���� ����
 parm : None
 retn : None
*/
VOID CDlgMain::InvalidateView()
{
	CRect rView;
	m_pic_ctl[0].GetWindowRect(rView);
	ScreenToClient(rView);
	InvalidateRect(rView, TRUE);
}

/*
 desc : Main Thread���� �߻��� �̺�Ʈ ó��
 parm : wparam	- [in]  Event ID
		lparam	- [in]  Event Data
 retn : 0L
*/
LRESULT CDlgMain::OnMsgMainThread(WPARAM wparam, LPARAM lparam)
{
	/* �ֱ������� ȣ���� */
	UpdatePeriod();
	/* Drive Position */
	UpdateDrvPos();
	/* Update Cali XY */
	switch (wparam)
	{
	case MSG_ID_WORK_COMP	:	WorkComplete();
	case MSG_ID_MEAS_MARK	:
	case MSG_ID_MEAS_CALI	:
	case MSG_ID_EXAM_SHUT	:	InvalidateView();
		switch (wparam)
		{
		case MSG_ID_MEAS_CALI	:
		case MSG_ID_EXAM_SHUT	:	UpdateCaliXY();
									UpdateWorkTime();	break;
		}
	}

#if 1
	/* ��� ��Ʈ���� �����̴� ���ο� ���� ��ư Enable / Disable */
	EnableCtrl();
#endif
	return 0L;
}

/*
 desc : �θ� �����忡 ���� �ֱ������� ȣ���
 parm : None
 retn : None
*/
VOID CDlgMain::UpdatePeriod()
{
	/* Grabbed Image Display */
	if (m_chk_ctl[8].GetCheck())	/* Live Mode */
	{
		UpdateLiveView();
		if (m_u8ViewMode == 0x00)	m_u8ViewMode	= 0x01;	/* Live Mode ���� */
	}
	else	/* Grabbed Mode */
	{
		/* ��� ������ Live Mode���ٰ� Grab Mode�� �ٲ� ��� */
		if (0x01 == m_u8ViewMode && !m_pMainThread->IsRunWork())
		{
			InvalidateView();
			m_u8ViewMode	= 0x00;	/* Grab Mode ���� */
		}
	}

	/* ���� �۾� (�ó�����) ����� */
	if (m_pMainThread->IsRunWork())
	{
		m_edt_flt[14].SetTextToNum(m_pMainThread->GetProcRate(), 3);
	}
#if 0
	/* ���� �ֱٿ� �뱤�� �뱤 �ӵ� ��, Material Thickness �� ��� */
	if (m_edt_int[4].GetTextToNum() != uvEng_ShMem_GetLuria()->exposure.frame_rate_factor)
	{
		m_edt_int[4].SetTextToNum(uvEng_ShMem_GetLuria()->exposure.frame_rate_factor);
	}
#endif
}

/*
 desc : Drive Position ���
 parm : None
 retn : TRUE or FALSE
*/
VOID CDlgMain::UpdateDrvPos()
{
	m_edt_flt[2].SetTextToNum(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x),		4, TRUE);
	m_edt_flt[3].SetTextToNum(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y),		4, TRUE);
	m_edt_flt[4].SetTextToNum(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam1),	4, TRUE);
	m_edt_flt[5].SetTextToNum(uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam2),	4, TRUE);
	UINT8 u8ACamID = GetCheckACam();
	m_edt_flt[15].SetTextToNum(uvCmn_MCQ_GetACamCurrentPosZ(u8ACamID),	4, TRUE);
}

/*
 desc : Align Camera Calibration X / Y ����
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateCaliXY()
{
	UINT8 u8Color[2][2]	= {NULL};
	INT32 i32Row		= 1, i32Col = 1;
	DOUBLE dbOffsetX	= 0.0f, dbOffsetY = 0.0f;
	TCHAR tzValue[2][32]= {NULL};
	COLORREF clrFg[2]	= { RGB(0, 0, 0), RGB(255, 0, 0) };
	COLORREF clrBk[2]	= { RGB(255, 255, 255), RGB(192, 255, 192) };

	/* ���� ��ġ ��� */
	m_pMainThread->GetCurRowCol(i32Row, i32Col);
#if 1
	i32Row	+= 1;
	i32Col	+= 1;
#endif
	/* ���� �� ��� */
	m_pMainThread->GetCurOffset(dbOffsetX, dbOffsetY);

	/* ���� (����)�� um �� ���� */
	dbOffsetX	= ROUNDED(dbOffsetX * 1000.0f, 1);
	dbOffsetY	= ROUNDED(dbOffsetY * 1000.0f, 1);
	swprintf_s(tzValue[0], 32, L"%+.1f", dbOffsetX);
	swprintf_s(tzValue[1], 32, L"%+.1f", dbOffsetY);
	/* ���� ���� */
	if (dbOffsetX < 0.0f)	u8Color[0][0]	= 0x01;
	if (dbOffsetY < 0.0f)	u8Color[0][1]	= 0x01;
	if (abs(dbOffsetX) > uvEng_GetConfig()->acam_cali.GetErrLimitValUm())	u8Color[1][0] = 0x01;
	if (abs(dbOffsetY) > uvEng_GetConfig()->acam_cali.GetErrLimitValUm())	u8Color[1][1] = 0x01;

	/* �׸��� ������ �� ���� (����: um) */
	m_pGridMeas[0].SetItemText(i32Row, i32Col, tzValue[0]);
	m_pGridMeas[1].SetItemText(i32Row, i32Col, tzValue[1]);
	m_pGridMeas[0].SetItemFgColour(i32Row, i32Col, clrFg[u8Color[0][0]]);
	m_pGridMeas[1].SetItemFgColour(i32Row, i32Col, clrFg[u8Color[0][1]]);
	m_pGridMeas[0].SetItemBkColour(i32Row, i32Col, clrBk[u8Color[1][0]]);
	m_pGridMeas[1].SetItemBkColour(i32Row, i32Col, clrBk[u8Color[1][1]]);
	m_pGridMeas[0].InvalidateRowCol(i32Row, i32Col);
	m_pGridMeas[1].InvalidateRowCol(i32Row, i32Col);
}

/*
 desc : Align Camera Calibration X / Y ����
 parm : row	- [in]  ������ ���� �־���� ��ġ ���� (�ڵ��� ���, 0)
		col	- [in]  ������ ���� �־���� ��ġ ���� (�ڵ��� ���, 0)
 retn : None
*/
VOID CDlgMain::UpdateCaliXY(INT32 row, INT32 col, LPG_ACGR result)
{
	/* �׸��� ������ �� ���� (����: um) */
	m_pGridMeas[0].SetItemDouble(row, col, result->move_mm_x*1000.0f, 1);	/* mm -> um */
	m_pGridMeas[1].SetItemDouble(row, col, result->move_mm_y*1000.0f, 1);	/* mm -> um */
	m_pGridMeas[0].InvalidateRowCol(row, col);
	m_pGridMeas[1].InvalidateRowCol(row, col);
}

/*
 desc : Align Camera Calibration X / Y ����
 parm : row	- [in]  ������ ���� �־���� ��ġ ���� (�ڵ��� ���, 0)
		col	- [in]  ������ ���� �־���� ��ġ ���� (�ڵ��� ���, 0)
 retn : None
*/
VOID CDlgMain::UpdateCaliXY(INT32 row, INT32 col, DOUBLE offset_x, DOUBLE offset_y)
{
	/* �׸��� ������ �� ���� (����: um) */
	m_pGridMeas[0].SetItemDouble(row, col, offset_x*1000.0f, 1);	/* mm -> um */
	m_pGridMeas[1].SetItemDouble(row, col, offset_y*1000.0f, 1);	/* mm -> um */
	m_pGridMeas[0].InvalidateRowCol(row, col);
	m_pGridMeas[1].InvalidateRowCol(row, col);
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
	case IDC_MAIN_BTN_STAGE_X		:
	case IDC_MAIN_BTN_STAGE_Y		:
	case IDC_MAIN_BTN_ACAM_X		:	SetMotionPos(id);					break;

	case IDC_MAIN_BTN_EXIT			:	PostMessage(WM_CLOSE, 0, 0L);		break;
	case IDC_MAIN_BTN_MC2_RESET		:	MC2Recovery();						break;
	case IDC_MAIN_BTN_ACAM_Z_MOVE	:	ACamMoveAbs();						break;
	case IDC_MAIN_BTN_MOVE_UNLOAD	:	UnloadPosition();					break;
	case IDC_MAIN_BTN_SAVE_OFFSET	:	SaveAsExcelFile();					break;
	case IDC_MAIN_BTN_MC2_MOVE_UP	:	MotionMove(ENG_MDMD::en_move_up);	break;
	case IDC_MAIN_BTN_MC2_MOVE_DN	:	MotionMove(ENG_MDMD::en_move_down);	break;
	case IDC_MAIN_BTN_WORK_REBUILD	:	ReBuildGridXY();					break;
	case IDC_MAIN_BTN_MOVE_MEASURE	:	InitMeasureMove();					break;
	case IDC_MAIN_BTN_SAVE_MEASURE	:	SaveMeasureMove();					break;
	case IDC_MAIN_BTN_WORK_START	:	StartCalibration();					break;
	case IDC_MAIN_BTN_WORK_STOP		:	StopCalibration();					break;
	case IDC_MAIN_BTN_TEST_MATCH	:	RunTestMatch();						break;
	case IDC_MAIN_BTN_THICK_DOF		:	SetDofFilmThick();					break;
	}
}

/*
 desc : üũ ��ư �̺�Ʈ ó��
 parm : id	- [in]  Click�� ��ư ID
 retn : None
*/
VOID CDlgMain::OnChkClick(UINT32 id)
{
	switch (id)
	{
	case IDC_MAIN_CHK_LIVE_VIEW	:	SetLiveView();	break;
	case IDC_MAIN_CHK_ACAM_NO	:	ChangeACamNo();	break;
	}
}

/*
 desc : �׸��� ���� �缳��
 parm : None
 retn : None
*/
VOID CDlgMain::ReBuildGridXY()
{
	UINT8 i, j, k;
	TCHAR tzTitle[2][8]	= { L"[ X ]", L"[ Y ]" };
	INT32 i32Rows, i32Cols;
	GV_ITEM stGV	= { NULL };

	/* ���� ��� ���� ���� */
	i32Rows	= (INT32)m_edt_int[0].GetTextToNum();
	i32Cols	= (INT32)m_edt_int[1].GetTextToNum();
	/* ��� ���� ������ �ּ� 2�� �̻��� �־�� �� */
	if (i32Rows < 2 || i32Cols < 2)
	{
		AfxMessageBox(L"There must be at least two rows and columns", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* ��� ���� ������ �ʹ� ���� ��� */
	if (i32Rows > uvEng_GetConfig()->acam_cali.max_rows_count ||
		i32Cols > uvEng_GetConfig()->acam_cali.max_cols_count)
	{
		AfxMessageBox(L"Too many rows and columns", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	for (k=0; k<2; k++)
	{
		/* �׸��� ���� ���� ��Ȱ��ȭ */
		m_pGridMeas[k].SetRedraw(FALSE);

		/* ������ ���� ��� ���� */
		m_pGridMeas[k].DeleteNonFixedRows();
	
		/* ��� ���� ���� ���� */
		m_pGridMeas[k].SetRowCount(i32Rows+1);
		m_pGridMeas[k].SetColumnCount(i32Cols+1);

		/* Ÿ��Ʋ ���� */
		stGV.mask	= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
		stGV.crFgClr= RGB(32, 32, 32);
		stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
		stGV.crBkClr= RGB(214, 214, 214);
		stGV.row	= 0;
		stGV.col	= 0;
		stGV.strText= tzTitle[k];
		stGV.lfFont	= g_lf;
		m_pGridMeas[k].SetItem(&stGV);

		/* Ÿ��Ʋ ��� �� ���� */
		for (i=1,stGV.col=0; i<=i32Rows; i++)
		{
			stGV.row	= i;
			stGV.strText.Format(L"%02u", i);
			m_pGridMeas[k].SetItem(&stGV);
			m_pGridMeas[k].SetRowHeight(i, 24);
		}
		for (i=1,stGV.row=0; i<=i32Cols; i++)
		{
			stGV.col	= i;
			stGV.strText.Format(L"%02u", i);
			m_pGridMeas[k].SetItem(&stGV);
			m_pGridMeas[k].SetColumnWidth(i, 38);
		}

		/* ���� ��� �� ���� */
		stGV.mask	= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
		stGV.crFgClr= RGB(16, 16, 16);
		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.crBkClr= RGB(255, 255, 255);
		for (i=1; i<=i32Rows; i++)
		{
			for (j=1; j<=i32Cols; j++)
			{
				stGV.row	= i;
				stGV.col	= j;
				stGV.strText.Format(L"+0.0");
				m_pGridMeas[k].SetItem(&stGV);
			}
		}

		/* �׸��� ���� ���� Ȱ��ȭ */
		m_pGridMeas[k].SetRedraw(TRUE);
		m_pGridMeas[k].Invalidate(FALSE);
	}

	/* �뱤 ���� ��� */
	m_edt_flt[10].SetTextToNum(m_edt_flt[12].GetTextToDouble() * (m_edt_int[0].GetTextToNum()-1), 4);
	m_edt_flt[11].SetTextToNum(m_edt_flt[13].GetTextToDouble() * (m_edt_int[1].GetTextToNum()-1), 4);
}

/*
 desc : Align Camera ��ȣ ����
 parm : None
 retn : None
*/
VOID CDlgMain::ChangeACamNo()
{
	TCHAR tzNum[8]	= {NULL};

	m_chk_ctl[9].GetWindowText(tzNum, 8);
	if (0 == wcscmp(tzNum, L"1"))	m_chk_ctl[9].SetWindowText(L"2");
	else							m_chk_ctl[9].SetWindowText(L"1");
	/* �� �ٽ� ��� */
	SetRegistModel();
	/* ī�޶� ���� �ο��� ���� ��ġ ���� */
	LoadMeasPos();
}

/*
 desc : ���� ���õ� ����� ī�޶� ��ȣ ��ȯ (1 or 2)
 parm : None
 retn : 1 or 2
*/
UINT8 CDlgMain::GetCheckACam()
{
	TCHAR tzNum[8]	= {NULL};

	m_chk_ctl[9].GetWindowText(tzNum, 8);
	return (UINT8)_wtoi(tzNum);
}

/*
 desc : Live Mode ����
 parm : None
 retn : None
*/
VOID CDlgMain::SetLiveView()
{
	BOOL bSucc		= FALSE;
	UINT32 u32OnOff	= 0x00000000;

	/* ���� äũ�� �����̸� */
	if (m_chk_ctl[8].GetCheck())	/* Live View */
	{
		/* Align Camera is Live Mode */
		uvEng_Camera_SetCamMode(ENG_VCCM::en_live_mode);
		/* Trigger & Strobe : Enabled */
		bSucc = uvEng_Trig_ReqTriggerStrobe(TRUE);
		if (bSucc)
		{
			bSucc = uvEng_Trig_ReqEncoderLive(GetCheckACam());
		}
	}
	else
	{
		/* Trigger Disabled */
		bSucc = uvEng_Trig_ReqEncoderOutReset();
		/* Trigger & Strobe : Disabled */
		if (bSucc)
		{
			bSucc = uvEng_Trig_ReqTriggerStrobe(FALSE);
			UINT64 u64Tick = GetTickCount64();

			do	{

				/* Trigger Board�κ��� ������ ���� ���� ���� �Դ��� Ȯ�� */
				if (0 == uvEng_ShMem_GetTrig()->enc_out_val)
				{
					/* �ִ� 1 �� ���� ��� �� ���� �������� */
					if (GetTickCount64() > (u64Tick + 1000))	break;
				}
				else
				{
					/* �ִ� 2 �� ���� ��� �� ���� ������, ���� �������� */
					if (GetTickCount64() > (u64Tick + 2000))	break;
				}
				Sleep(10);

			}	while (1);
			/* Align Camera is Calibration Mode */
			uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
		}
	}
	Invalidate(TRUE);
}

/*
 desc : ���ο� �˻��� ���� �˻� ����� �� ���
 parm : None
 retn : Non
*/
VOID CDlgMain::SetRegistModel()
{
	UINT8 u8ACamID		= GetCheckACam();
	UINT32 u32Model[2]	= { (UINT32)ENG_MMDT::en_circle, (UINT32)ENG_MMDT::en_ring }, u32Find = 2;
	DOUBLE dbMSize1[2]	= {NULL} /* um */, dbMColor[2] = {NULL} /* 256:Black, 128:White */;
	DOUBLE dbMSize2[2]	= {NULL};
	LPG_CIEA pstCfg		= uvEng_GetConfig();

	/* Register the search target related model */
	u32Model[0]	= (UINT32)ENG_MMDT::en_ring;
	dbMSize1[0]	= pstCfg->acam_cali.model_ring_size1 * 1000.0f;
	dbMSize2[0]	= pstCfg->acam_cali.model_ring_size2 * 1000.0f;
	dbMColor[0]	= pstCfg->acam_cali.model_ring_color;
	u32Model[1]	= (UINT32)ENG_MMDT::en_circle;
	dbMSize1[1]	= pstCfg->acam_cali.model_shut_size * 1000.0f;
	dbMColor[1]	= pstCfg->acam_cali.model_shut_color;

	if (!uvEng_Camera_SetModelDefineEx(u8ACamID,
									   pstCfg->mark_find.model_speed,
									   pstCfg->mark_find.detail_level,
									   pstCfg->mark_find.model_smooth,
									   u32Model, dbMColor, dbMSize1,
									   dbMSize2, NULL, NULL, u32Find))
	{
		AfxMessageBox(L"Failed to register the model for search target", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
}

/*
 desc : ��� (Motor Drive) �̵�
 parm : direct	- [in]  Up or Down
 retn : None
*/
VOID CDlgMain::MotionMove(ENG_MDMD direct)
{
	UINT8 i	= 0x00, u8DrvNo = 0x00;	/* 0x00 ~ 0x07 */
	BOOL bCheck		= FALSE;
	DOUBLE dbDrvPos	= 0, dbMinPos, dbMaxPos, dbDist, dbVelo;
	LPG_CMSI pstCfg	= &uvEng_GetConfig()->mc2_svc;
	LPG_MDSM pstMC2	= uvEng_ShMem_GetMC2();

	/* �̵��ϰ��� �ϴ� Drive�� üũ �Ǿ� �ִ��� Ȯ�� */
	for (i=0; i<pstCfg->drive_count && !bCheck; i++)
	{
		bCheck	= m_chk_ctl[pstCfg->axis_id[i]].GetCheck() > 0;
	}
	if (!bCheck)
	{
		AfxMessageBox(L"The drive to be moved is not checked", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* �̵��ϰ��� �ϴ� �ӵ��� �Ÿ� ���� �ԷµǾ� �ִ��� Ȯ�� (���⼭�� Step �̵��� �ش� ��) */
	dbDist	= m_edt_flt[0].GetTextToDouble();
	dbVelo	= m_edt_flt[1].GetTextToDouble();
	if (dbDist < 0.0001f || dbVelo < 1.0f)
	{
		AfxMessageBox(L"Check the move distance and speed values", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* ���� ��ġ���� �̵� �Ÿ� ���� Min or Max ���� �ʰ� �ߴٸ�, Min or Max �� ��ŭ �̵� */
	for (i=0; i<pstCfg->drive_count; i++)
	{
		/* ����̺갡 üũ �Ǿ� �ִ��� ���� Ȯ�� */
		u8DrvNo	= pstCfg->axis_id[i];
		if (!m_chk_ctl[u8DrvNo].GetCheck())	continue;

		/* ���� ����̺��� ��ġ Ȯ�� */
		dbDrvPos	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(u8DrvNo));
		dbMinPos	= pstCfg->min_dist[u8DrvNo];
		dbMaxPos	= pstCfg->max_dist[u8DrvNo];

		/* ���� Ȥ�� ���� ������ �̵��ϸ�, ���� ��ġ�� �̵� �Ÿ� ����ŭ ���� �ֱ� */
		if (direct == ENG_MDMD::en_move_up || direct == ENG_MDMD::en_move_right)
		{
			dbDrvPos	+= dbDist;	/* �̵��� �Ÿ� �� ��� */
			if (dbDrvPos > dbMaxPos)	dbDrvPos	= dbMaxPos;	/* �ִ� �̵� �Ÿ� �Ѿ��, �ִ� ������ ��ü */
		}
		else
		{
			dbDrvPos	-= dbDist;	/* �̵��� �Ÿ� �� ��� */
			if (dbDrvPos < dbMinPos)	dbDrvPos	= dbMinPos;	/* �ּ� �̵� �Ÿ� �Ѿ��, �ִ� ������ ��ü */
		}

		/* Motion Drive �̵� */
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI(u8DrvNo), dbDrvPos, dbVelo))
		{
			AfxMessageBox(L"Failed to move the motion drive", MB_ICONSTOP|MB_TOPMOST);
			return;
		}
	}
}

/*
 desc : MC2�� ��� Motor �ʱ�ȭ (����) (Homing)
 parm : None
 retn : None
*/
VOID CDlgMain::MC2Recovery()
{
	UINT8 i	= 0x00;
#if 1
	for (i=0; i<MAX_MC2_DRIVE; i++)
	{
		if (!m_chk_ctl[i].GetCheck())	continue;
		if (!uvEng_MC2_SendDevHoming(ENG_MMDI(i)))
		{
			AfxMessageBox(L"The home operation of all drives has failed", MB_ICONSTOP|MB_TOPMOST);
			return;
		}
	}
#else
	if (!uvEng_MC2_SendDevHomingAll())
	{
		AfxMessageBox(L"The home operation of all drives has failed", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
#endif
}

/*
 desc : Align Camera (Motor Drive) �̵� (���� ��ġ�� �̵�)
 parm : None
 retn : None
*/
VOID CDlgMain::ACamMoveAbs()
{
	UINT8 u8ACamID	= GetCheckACam();
	DOUBLE dbPosZ	= 0.0f;	/* unit : mm */

	/* Photohead or Align Camera Z Axis �̵� */
	dbPosZ	= m_edt_flt[9].GetTextToDouble();
	/* ���ο� ��ġ�� �̵� */
	uvEng_MCQ_SetACamMovePosZ(u8ACamID, 0x00 /* Absolute Moving */, dbPosZ);
}

/*
 desc : Save the data in the grid control to a file
 parm : None
 retn : None
*/
VOID CDlgMain::SaveAsExcelFile()
{
	TCHAR tzTempFile[MAX_PATH_LEN]	= {NULL};
	SYSTEMTIME stTm	= {NULL};

	/* Get the current time */
	GetLocalTime(&stTm);

	/* ��ϵ� �����Ͱ� ������ Ȯ�� */
	if (m_pGridMeas->GetRowCount() < 2 || m_pGridMeas->GetColumnCount() < 2)	return;

	/* ���� �ӽ� ���Ϸ� ���� �� ���� */
	swprintf_s(tzTempFile, MAX_PATH_LEN, L"%s\\xy2d\\exam_x_acam(%u)_thick(%u)_%04d%02d%02d_%02d%02d.csv",
			   g_tzWorkDir, GetCheckACam(), (UINT16)m_edt_int[4].GetTextToNum(),
			   stTm.wYear, stTm.wMonth, stTm.wDay, stTm.wHour, stTm.wMinute);
	m_pGridMeas[0].Save(tzTempFile);

	/* ���� �ӽ� ���Ϸ� ���� �� ���� */
	swprintf_s(tzTempFile, MAX_PATH_LEN, L"%s\\xy2d\\exam_y_acam(%u)_thick(%u)_%04d%02d%02d_%02d%02d.csv",
			   g_tzWorkDir, GetCheckACam(), (UINT16)m_edt_int[4].GetTextToNum(),
			   stTm.wYear, stTm.wMonth, stTm.wDay, stTm.wHour, stTm.wMinute);
	m_pGridMeas[1].Save(tzTempFile);
}

/*
 desc : �ʱ� ���� ��ġ�� �̵�
 parm : None
 retn : None
*/
VOID CDlgMain::InitMeasureMove()
{
	UINT8 i	= 0x00, u8ACamID = GetCheckACam(), u8DrvPh = UINT8(ENG_MMDI::en_axis_ph1);
	DOUBLE dbDist	= 0.0f, dbVelo = 0.0f, dbPosZ;
	ENG_MMDI enACam, enSide;

	/* ī�޶� 2 ���� (�ܺ� Ÿ��) ��� */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)uvEng_GetConfig()->set_comn.align_camera_kind)
	{
		if (u8ACamID == 1)
		{
			enACam	= ENG_MMDI::en_align_cam1;
			enSide	= ENG_MMDI::en_align_cam2;
			dbDist	= uvEng_GetConfig()->mc2_svc.max_dist[(UINT8)enSide];
		}
		else
		{
			enACam	= ENG_MMDI::en_align_cam2;
			enSide	= ENG_MMDI::en_align_cam1;
			dbDist	= uvEng_GetConfig()->mc2_svc.min_dist[(UINT8)enSide];
		}

		/* Camera 2�� �̵� */
		dbVelo	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)enSide];
		uvEng_MC2_SendDevAbsMove(enSide, dbDist, dbVelo);
		/* Camera 1�� �̵� */
		dbVelo	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)enACam];
		uvEng_MC2_SendDevAbsMove(enACam, uvEng_GetConfig()->acam_cali.mark_acam[u8ACamID-1], dbVelo);
#if 0
		/* ī�޶� Z Axis �̵� */
		uvEng_MCQ_SetACamMovePosZ(u8ACamID, 0x00 /* Absolute Moving */, uvEng_GetConfig()->acam_focus.acam_z_focus[u8ACamID-1]);
#endif
	}
#if 0
	else
	{
		/* ī�޶� Z Axis �̵� */
		uvEng_MCQ_SetACamMovePosZ(0x01, 0x00 /* Absolute Moving */, uvEng_GetConfig()->acam_focus.acam_z_focus[0]);
	}
#endif
	/* Stage X �̵� */
	dbVelo	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_x];
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, uvEng_GetConfig()->acam_cali.mark_stage_x, dbVelo);
	/* Stage Y �̵� */
	dbVelo	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_y];
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, uvEng_GetConfig()->acam_cali.mark_stage_y[u8ACamID-1], dbVelo);

	for (i=0; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		/* ���а� Z �� Focus �� ��� */
		dbPosZ	= uvEng_GetConfig()->luria_svc.ph_z_focus[i];
		/* �̵��ϰ��� �ϴ� ���� ������ ������� ���� */
		if (!uvCmn_Luria_IsValidPhMoveZRange(dbPosZ))
		{
			AfxMessageBox(L"Out of moving range area", MB_ICONSTOP|MB_TOPMOST);
			return;
		}

		/* ���а� Z �� �̵� */
		if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
		{
			uvEng_Luria_ReqSetMotorAbsPosition(i+1, dbPosZ);
		}
		else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
		{
			dbVelo = uvEng_GetConfig()->mc2_svc.max_velo[u8DrvPh+i];
			uvEng_MC2_SendDevAbsMove(ENG_MMDI(u8DrvPh+i), dbPosZ, dbVelo);
		}
	}
}

/*
 desc : ���� ���� ���� ��ġ ����
 parm : None
 retn : None
*/
VOID CDlgMain::SaveMeasureMove()
{
	UINT8 u8ACam	= GetCheckACam();
	LPG_CACC pstCfg	= &uvEng_GetConfig()->acam_cali;

	pstCfg->mark_stage_x			= m_edt_flt[6].GetTextToDouble();
	pstCfg->mark_stage_y[u8ACam-1]	= m_edt_flt[7].GetTextToDouble();
	pstCfg->mark_acam[u8ACam-1]		= m_edt_flt[8].GetTextToDouble();

	/* Init ���Ϸ� ���� */
	uvEng_SaveACamCali();
	/* ȭ�鿡 �ٽ� ��� */
	LoadMeasPos();
}

/*
 desc : �������� ��ġ�� �۾��� ��ġ�� �̵�
 parm : None
 retn : None
*/
VOID CDlgMain::UnloadPosition()
{
	DOUBLE dbMoveX	= uvEng_GetConfig()->set_align.table_unloader_xy[0][0];
	DOUBLE dbMoveY	= uvEng_GetConfig()->set_align.table_unloader_xy[0][1];

	/* Vacuum�� Shutter�� ���� ���� ���� */
	uvEng_MCQ_LedVacuumShutter(0x00, 0x01, 0x00);

	/* ��� (��������) �̵� */
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, dbMoveX, m_edt_flt[1].GetTextToDouble());
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dbMoveY, m_edt_flt[1].GetTextToDouble());
}

/*
 desc : ���� ȯ�� ���Ͽ� ����� ȸ�� �� �ҷ�����
 parm : None
 retn : None
*/
VOID CDlgMain::LoadParam()
{
	/* Measurement Row and Column count */
	m_edt_int[0].SetTextToNum(UINT16(26));	/* Full 52 */
	m_edt_int[1].SetTextToNum(UINT16(49));
	/* The size of Grabbed Image */
	m_edt_int[2].SetTextToNum(uvEng_GetConfig()->set_cams.ioi_size[0]);
	m_edt_int[3].SetTextToNum(uvEng_GetConfig()->set_cams.ioi_size[1]);
	m_edt_int[4].SetTextToNum(UINT16(960));	/* Material Thick */
	/* Move Velo and Distance */
	m_edt_flt[0].SetTextToNum(100.0f,	4);
	m_edt_flt[1].SetTextToNum(uvEng_GetConfig()->mc2_svc.move_velo,	4);
	/* Up or Down For camera z axis */
	m_edt_flt[9].SetTextToNum(1.0f, 4);
	/* Measurement Row and Column Period Size (mm) */
	m_edt_flt[12].SetTextToNum(20.0f, 4);
	m_edt_flt[13].SetTextToNum(10.368f, 4);
	/* ���� ī�޶��� ���� ��ġ ���� */
	LoadMeasPos();
}

/*
 desc : ���� ��ġ ����
 parm : None
 retn : None
*/
VOID CDlgMain::LoadMeasPos()
{
	UINT8 u8ACamID	= GetCheckACam();

	m_edt_flt[6].SetTextToNum(uvEng_GetConfig()->acam_cali.mark_stage_x, 4);
	m_edt_flt[7].SetTextToNum(uvEng_GetConfig()->acam_cali.mark_stage_y[u8ACamID-1], 4);
	m_edt_flt[8].SetTextToNum(uvEng_GetConfig()->acam_cali.mark_acam[u8ACamID-1], 4);
}

/*
 desc : ��ư �� ���� ��Ʈ�� Enable or Disable ó��
 parm : None
 retn : None
*/
VOID CDlgMain::EnableCtrl()
{
	UINT8 i;
	BOOL bStopped	= TRUE, bRunCali = m_pMainThread->IsRunWork();
	BOOL bConnected	= TRUE, bEnable	= TRUE, bRunDemo = uvEng_GetConfig()->IsRunDemo();
	LPG_CMSI pstMC2Svc	= &uvEng_GetConfig()->mc2_svc;

	/* �ʿ��� ��ư�� Enable or Disable ó�� */
	bConnected	= uvCmn_MC2_IsConnected() && uvCmn_Camera_IsConnected() &&
				  uvCmn_Trig_IsConnected()&& uvCmn_MCQ_IsConnected();
	/* ���� ���õ� ī�޶� �̵� �������� ���ο� ���� */
	bStopped	= uvEng_ShMem_GetMC2()->GetDriveAct((UINT8)ENG_MMDI::en_stage_x)->IsStopped()	&&
				  uvEng_ShMem_GetMC2()->GetDriveAct((UINT8)ENG_MMDI::en_stage_y)->IsStopped()	&&
				  uvEng_ShMem_GetMC2()->GetDriveAct((UINT8)ENG_MMDI::en_align_cam1)->IsStopped()&&
				  uvEng_ShMem_GetMC2()->GetDriveAct((UINT8)ENG_MMDI::en_align_cam2)->IsStopped();

	/* ��ư Ȱ��ȭ ���� */
	bEnable	= bStopped && bConnected && !bRunCali || bRunDemo;

	for (i=0; i<4; i++)
	{
		if (m_btn_ctl[i].IsWindowEnabled() == bEnable)	continue;
		m_btn_ctl[i].EnableWindow(bEnable);
	}
	for (i=5; i<14; i++)
	{
		if (m_btn_ctl[i].IsWindowEnabled() == bEnable)	continue;
		m_btn_ctl[i].EnableWindow(bEnable);
	}
	if (bRunDemo)
	{
		m_btn_ctl[14].EnableWindow(bEnable);
	}
	else
	{
		if (m_btn_ctl[14].IsWindowEnabled() != (!bEnable))
		{
			m_btn_ctl[14].EnableWindow(!bEnable);
		}
	}
	m_btn_ctl[15].EnableWindow(bEnable);

	/* MC2 Drive Check Button */
	for (i=0; i<uvEng_GetConfig()->mc2_svc.drive_count; i++)
	{
		m_chk_ctl[pstMC2Svc->axis_id[i]].EnableWindow(bEnable);
	}
	for (i=8; i<11; i++)	m_chk_ctl[i].EnableWindow(bEnable);
}

/*
 desc : Calibration Start
 parm : None
 retn : None
*/
VOID CDlgMain::StartCalibration()
{
	/* ���� �β� ���� ���ؼ� ���� ��ư�� �������� ���� */
	if (!m_bSetThick)
	{
		AfxMessageBox(L"No material thickness value set", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* �׸��� ��Ʈ�� ������ */
	ReBuildGridXY();

	/* Trigger ���� �Ӽ� �� ���� (Trigger OnTime / Strobe OnTime / Trigger Delay Time */
	if (!uvEng_Trig_ReqInitUpdate())
	{
		AfxMessageBox(L"Failed to set the trigger attribute for LED Lamp", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* ���� ���� ���� ���� ������ ���� */
	uvEng_GetConfig()->acam_cali.set_rows_count	= (UINT16)m_edt_int[0].GetTextToNum();
	uvEng_GetConfig()->acam_cali.set_cols_count	= (UINT16)m_edt_int[1].GetTextToNum();
	uvEng_GetConfig()->acam_cali.period_row_size= (DOUBLE)m_edt_flt[12].GetTextToDouble();
	uvEng_GetConfig()->acam_cali.period_col_size= (DOUBLE)m_edt_flt[13].GetTextToDouble();
	/* Material Thickness �� ���� ���� */
	if (m_edt_int[4].GetTextToNum() < 1)
	{
		AfxMessageBox(L"The material thickness is not entered", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* �۾� ���� */
	m_pMainThread->StartCali(GetCheckACam(), (UINT16)m_edt_int[4].GetTextToNum());
}

/*
 desc : Calibration Stop
 parm : None
 retn : None
*/
VOID CDlgMain::StopCalibration()
{
	UINT64 u64Tick = GetTickCount64();

	m_pMainThread->StopCali();	/* �۾� ���� */

	do {

		uvCmn_uSleep(100);

		/* 10 �� �̻� ����ϸ� ���� ���������� */
		if (u64Tick+10000 < GetTickCount64())
		{
			break;
		}

	} while (m_pMainThread->IsRunWork());
}

/*
 desc : ���� ��� ��ġ ���� ����Ʈ �ڽ��� ���
 parm : id	- [in]  Button ID
 retn : None
*/
VOID CDlgMain::SetMotionPos(UINT32 id)
{
	UINT8 u8EdtID	= 0x06 + UINT8(id - IDC_MAIN_BTN_STAGE_X);
	UINT8 u8ACamID	= GetCheckACam();
	DOUBLE dbPos	= 0.0f;
	ENG_MMDI enACam	= u8ACamID == 1 ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;

	switch (id)
	{
	case IDC_MAIN_BTN_STAGE_X	:	dbPos	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);	break;
	case IDC_MAIN_BTN_STAGE_Y	:	dbPos	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);	break;
	case IDC_MAIN_BTN_ACAM_X	:	dbPos	= uvCmn_MC2_GetDrvAbsPos(enACam);				break;
	}
	m_edt_flt[u8EdtID].SetTextToNum(dbPos, 4);
}

/*
 desc : ���������� �����ϴ��� ��Ī Ȯ��
 parm : None
 ��ȭ : None
*/
VOID CDlgMain::RunTestMatch()
{
	UINT8 u8ACamID		= GetCheckACam();
	BOOL bFinded		= FALSE;
	UINT64 u64TickLoop	= 0, u64StartTick;
	LPG_ACGR pstResult	= NULL;

	/* ���� Live Mode ������� Ȯ�� */
	if (m_chk_ctl[8].GetCheck())
	{
		AfxMessageBox(L"Currently running in <live mode>", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* ���� ��ϵ� ���� �����ϴ��� ���� */
	if (!uvEng_Camera_IsSetMarkModel(0x01, u8ACamID))
	{
		AfxMessageBox(L"There are no registered models", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* Align Camera is Calibration Mode */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);

	/* ù�� ��� �� �κ� �� �ʱ�ȭ */
	m_pGridMeas[0].SetItemText(1, 1, L"+0.0");
	m_pGridMeas[1].SetItemText(1, 1, L"+0.0");
	m_pGridMeas[0].InvalidateRowCol(1, 1);
	m_pGridMeas[1].InvalidateRowCol(1, 1);

	/* ���� �˻��� Grabbed Data & Image ���� */
	uvEng_Camera_ResetGrabbedImage();

	/* �۾� ��û ��� �ð� */
	u64StartTick	= GetTickCount64();

	/* Trigger ������ (��������) �߻� */
	if (!uvEng_Trig_ReqTrigOutOne(u8ACamID, TRUE))
	{
		uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
		AfxMessageBox(L"Failed to send the event for trigger", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* �۾� ��û ��� �ð� */
	u64StartTick	= GetTickCount64();
	/* 1 ~ 2 �� ���� ��� */
	do {

		if (u64StartTick+2000 < GetTickCount64())	break;
		uvCmn_uSleep(100);

	} while (1);

	/* �˻� ��� Ȯ�� */
	RunTestExam();

	/* ȭ�� ���� */
	InvalidateView();
	/* Camera ���� ��� ���� */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
}

/*
 desc : ���� �߽ɰ� Circle�� �߽� ���� ������ �󸶳� �߻� �ߴ��� Ȯ��
 parm : None
 ��ȭ : None
*/
VOID CDlgMain::RunTestExam()
{
	UINT32 u32GrabSize	= 0;
	DOUBLE dbOffsetXY[2]= {NULL};
	LPG_ACGR pstResult	= NULL;

	/* Grabbed Image ũ�� ��� (����: bytes) */
	u32GrabSize	= uvEng_GetConfig()->set_cams.ioi_size[0] * uvEng_GetConfig()->set_cams.ioi_size[1];
	/* Grabbed Image ���� �Ҵ� */
	pstResult	= new STG_ACGR[2];
	ASSERT(pstResult);
	memset(pstResult, 0x00, (sizeof(STG_ACGR)-sizeof(PUINT8)) * 2);
	pstResult[0].grab_data	= (PUINT8)::Alloc(u32GrabSize + 1);
	pstResult[1].grab_data	= (PUINT8)::Alloc(u32GrabSize + 1);
	pstResult[0].grab_data[u32GrabSize]	= 0x00;
	pstResult[1].grab_data[u32GrabSize]	= 0x00;

	/* �˻� ��� Ȯ�� */
	if (!uvEng_Camera_RunModelExam(pstResult))
	{
		AfxMessageBox(L"Failed to analyze the grabbed image", MB_ICONSTOP|MB_TOPMOST);
		delete [] pstResult;
		return;
	}

	/* �˻��� 2���� �߽� ���� ���� �� �� */
	dbOffsetXY[0]	= pstResult[0].mark_cent_mm_x - pstResult[1].mark_cent_mm_x;
	dbOffsetXY[1]	= pstResult[0].mark_cent_mm_y - pstResult[1].mark_cent_mm_y;

	/* �׸��� ��Ʈ�� ���� */
	UpdateCaliXY(1, 1, dbOffsetXY[0], dbOffsetXY[1]);

	/* �޸� ���� */
	delete [] pstResult;
}

/*
 desc : Calibration Material Thick ���� ��ŭ ī�޶� Z �� ��� �̵� ��Ŵ
 parm : None
 retn : Non
*/
VOID CDlgMain::SetDofFilmThick()
{
	DOUBLE dbSetThick	= (DOUBLE)m_edt_int[4].GetTextToNum() / 1000.0f;	/* mm */
	DOUBLE dbBaseThick	= uvEng_GetConfig()->set_align.dof_film_thick;
	DOUBLE dbDiffThick	= dbSetThick - dbBaseThick;
	DOUBLE *pdbACamFocus= uvEng_GetConfig()->acam_focus.acam_z_focus;

	if (dbSetThick < 0.05f /* 50 um �����̸� ó������ ���� */)	return;

	/* ���� �β� ��� ���̸�ŭ Align Camera Z �� �̵� ��Ŵ */
	uvEng_MCQ_SetACamMovePosZ(0x01, 0x00, pdbACamFocus[0]+dbDiffThick);
	uvEng_MCQ_SetACamMovePosZ(0x02, 0x00, pdbACamFocus[1]+dbDiffThick);

	/* �β� ���� �����ߴٰ� �÷��� ���� */
	m_bSetThick	= TRUE;
}

/*
 desc : ��� �۾��� �Ϸ�� ���
 parm : None
 retn : None
*/
VOID CDlgMain::WorkComplete()
{
#if 0
	TCHAR tzTime[32]	= {NULL};
	UINT64 u64JobTime	= m_pMainThread->GetJobTime();

	swprintf_s(tzTime, 32, L"%.3f min", (u64JobTime/1000.0f) / 60.0f);
	m_edt_int[5].SetTextToStr(tzTime, TRUE);
#endif
}

/*
 ���� : ���� �۾� �ð� ����
 ���� : None
 ��ȯ : None
*/
VOID CDlgMain::UpdateWorkTime()
{
	if (m_pMainThread && m_pMainThread->IsRunWork())
	{
		TCHAR tzTime[32]	= {NULL};
		UINT32 u32TimeSec	= (UINT32)(m_pMainThread->GetWorkTime()/1000);
		UINT32 u32TimeMin	= (UINT32)(u32TimeSec / 60);
		swprintf_s(tzTime, 32, L"%u m %u s", u32TimeMin, u32TimeSec%60);
		m_edt_int[5].SetTextToStr(tzTime, TRUE);
	}
}

/*
 desc : Updates the live image
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateLiveView()
{
	HDC hDC	= NULL;
	RECT rDraw;

	/* �̹����� ��µ� ���� ���� */
	m_pic_ctl[0].GetClientRect(&rDraw);
	hDC	= ::GetDC(m_pic_ctl[0].m_hWnd);
	if (hDC)
	{
		uvEng_Camera_DrawLiveBitmap(hDC, rDraw, GetCheckACam());
		::ReleaseDC(m_pic_ctl[0].m_hWnd, hDC);
	}
}
