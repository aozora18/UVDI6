
/*
 desc : Main Dialog
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgMain.h"

#include "DlgStep.h"
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
	m_bDrawBG			= 0x01;
	m_hIcon				= AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_u64DrawTime		= 0;
	m_u8ViewMode		= 0x00;	/* Grab Mode ���� */
	m_u8LastCamZAxis	= 0;
	m_u8FocusCount		= 0;
	m_dbLastACamPos		= 0.0f;
	m_bEnableLast		= TRUE;
	m_bReqPhOffset		= FALSE;
	m_pstGrabData		= NULL;
	m_pGridStep[0]		= NULL;
	m_pGridStep[1]		= NULL;
	m_pGridStep[2]		= NULL;
	m_pMainThread		= NULL;
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
	u32StartID	= IDC_MAIN_GRP_MARK_1;
	for (i=0; i<13; i++)	DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Check - Normal */
	u32StartID	= IDC_MAIN_CHK_MC2_DRV_0;
	for (i=0; i<10; i++)	DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
	/* Button - Normal */
	u32StartID	= IDC_MAIN_BTN_MC2_MOVE_UP;
	for (i=0; i<17; i++)	DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Static - Normal */
	u32StartID	= IDC_MAIN_TXT_MC2_MOVE;
	for (i=0; i<24; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* edit - integer */
	u32StartID	= IDC_MAIN_EDT_MARK_COUNT;
	for (i=0; i<4; i++)		DDX_Control(dx, u32StartID+i,	m_edt_int[i]);
	/* Edit - Distance */
	u32StartID	= IDC_MAIN_EDT_MC2_DIST;
	for (i=0; i<18; i++)	DDX_Control(dx, u32StartID+i,	m_edt_flt[i]);
	/* For Image */
	u32StartID	= IDC_MAIN_PIC_VIEW_1;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_pic_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgMain, CMyDialog)
	ON_MESSAGE(WM_MAIN_THREAD,	OnMsgMainThread)
	ON_WM_SYSCOMMAND()
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_BTN_MC2_MOVE_UP, IDC_MAIN_BTN_EXIT,	OnBtnClick)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_CHK_MC2_DRV_0, IDC_MAIN_CHK_MARK_SAVE,OnChkClick)
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

	InitCtrl();	/* ���� ��Ʈ�� �ʱ�ȭ */
	InitGridView();	/* �׸��� �ʱ�ȭ */
	InitGridData();	/* �׸��� �ʱ�ȭ */
	InitSetup();

	/* Grabbed Image ���� �Ҵ� */
	m_pstGrabData	= new STG_ACGR[2];
	ASSERT(m_pstGrabData);
	memset(m_pstGrabData, 0x00, sizeof(STG_ACGR) * 2);
	/* Grabbed Image Buffer �Ҵ�*/
	u32Size	= uvEng_GetConfig()->set_cams.soi_size[0] *
			  uvEng_GetConfig()->set_cams.soi_size[1];
	m_pstGrabData[0].grab_data	= (PUINT8)::Alloc(u32Size + 1);
	m_pstGrabData[1].grab_data	= (PUINT8)::Alloc(u32Size + 1);
	m_pstGrabData[0].grab_data[u32Size]	= 0x00;
	m_pstGrabData[1].grab_data[u32Size]	= 0x00;

	/* ���� �ֱٿ� ������ �� �ҷ����� */
	LoadParam();
	/* Main Thread ���� (!!! �߿�. InitLib �� �����ϸ�, �ݵ�� �����ؾ� �� !!!) */
	m_pMainThread	= new CMainThread(m_hWnd);
	ASSERT(m_pMainThread);
	if (!m_pMainThread->CreateThread(0, NULL, NORMAL_THREAD_SPEED))
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
	/* Grabbed Image ���� �޸� ���� */
	if (m_pstGrabData)
	{
		if (m_pstGrabData[0].grab_data)	::Free(m_pstGrabData[0].grab_data);
		if (m_pstGrabData[1].grab_data)	::Free(m_pstGrabData[1].grab_data);
		delete [] m_pstGrabData;
	}
	for (i=0; i<3; i++)
	{
		if (m_pGridStep[i])
		{
			m_pGridStep[i]->DestroyWindow();
			delete m_pGridStep[i];
		}
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

	/* ���� Calibration Mode�� ��� */
	if (uvEng_Camera_GetCamMode() == ENG_VCCM::en_cali_mode)
	{
		/* ���� �̹����� ��µ� ���� ���� */
		m_pic_ctl[0].GetWindowRect(&rDraw);
		ScreenToClient(&rDraw);
		uvEng_Camera_DrawMarkData(dc->m_hDC, rDraw, &m_pstGrabData[0], TRUE);

		/* ������ �̹����� ��µ� ���� ���� */
		m_pic_ctl[1].GetWindowRect(&rDraw);
		ScreenToClient(&rDraw);
		uvEng_Camera_DrawMarkData(dc->m_hDC, rDraw, &m_pstGrabData[1], TRUE);
	}
	else if (enMode == ENG_VCCM::en_live_mode)
	{
		/* ���� �̹����� ��µ� ���� ���� */
		m_pic_ctl[0].GetWindowRect(&rDraw);
		ScreenToClient(&rDraw);
		uvEng_Camera_DrawLive(dc->m_hDC, rDraw, 0x01);
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
	return uvEng_Init(ENG_ERVM::en_cali_step);
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
	for (i=0; i<13; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* text - normal */
	for (i=0; i<24; i++)
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
	for (i=0; i<4; i++)
	{
		m_edt_int[i].SetEditFont(&g_lf);
		m_edt_int[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_int[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_int[i].SetInputType(EN_DITM::en_int);
	}
	/* edit - float */
	for (i=0; i<18; i++)
	{
		m_edt_flt[i].SetEditFont(&g_lf);
		m_edt_flt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_flt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_flt[i].SetInputType(EN_DITM::en_float);
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
	/* for MC2 - üũ ��ư Ȱ��ȭ ó�� */
	for (i=0; i<pstMC2->drive_count; i++)
	{
		m_chk_ctl[pstMC2->axis_id[i]].EnableWindow(TRUE);
	}

	/* Grid Control �� �ʱ�ȭ */
	ResetMeasureXY();
}

/*
 desc : �׸��� ��Ʈ�� ���� - Step XY
 parm : None
 retn : None
*/
VOID CDlgMain::InitGridView()
{
	TCHAR tzRows[5][16]	= { L"No", L"Left", L"Right", L"Step.X", L"Step.Y" }, tzCols[32] = {NULL};
	INT32 i, j, k, l, m, iColCnt = 26, iRowCnt = 5;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;

	GV_ITEM stGV = { NULL };
	CRect rGrid;

	// ���� ���� Client ũ��
	m_grp_ctl[8].GetWindowRect(rGrid);
	ScreenToClient(rGrid);
	rGrid.left	+= 15;
	rGrid.right	-= 100;
	rGrid.top	+= 24;
	rGrid.bottom-= 15;

	// �׸��� ��Ʈ�� �⺻ ���� �Ӽ�
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);
	/* ��ü ���� */
	m_pGridStep[2] = new CGridCtrl;
	ASSERT(m_pGridStep);
	m_pGridStep[2]->SetDrawScrollBarVert(FALSE);
	m_pGridStep[2]->SetDrawScrollBarHorz(FALSE);
	if (!m_pGridStep[2]->Create(rGrid, this, IDC_MAIN_GRID_STEP_XY, dwStyle))
	{
		delete m_pGridStep[2];
		m_pGridStep[2] = NULL;
		return;
	}
	/* ��ü �⺻ ���� */
	m_pGridStep[2]->SetLogFont(g_lf);
	m_pGridStep[2]->SetRowCount(iRowCnt);
	m_pGridStep[2]->SetColumnCount(iColCnt);
	m_pGridStep[2]->SetFixedRowCount(3);
	m_pGridStep[2]->SetRowHeight(0, 26);
	m_pGridStep[2]->SetRowHeight(1, 26);
	m_pGridStep[2]->SetRowHeight(2, 26);
	m_pGridStep[2]->SetRowHeight(3, 26);
	m_pGridStep[2]->SetRowHeight(4, 26);
	m_pGridStep[2]->SetFixedColumnCount(1);
	m_pGridStep[2]->SetColumnWidth(0, 57);
	m_pGridStep[2]->SetBkColor(RGB(255, 255, 255));
	m_pGridStep[2]->SetFixedColumnSelection(0);

	/* Ÿ��Ʋ (ù ��° ��) */
	stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	stGV.crBkClr = RGB(214, 214, 214);
	stGV.crFgClr = RGB(0, 0, 0);
	for (i=1, j=1, k=1, l=1, m=2; i<iColCnt-1; i++)
	{
		stGV.row = 0;
		stGV.col = i;
		stGV.strText.Format(L"%02d", i);
		m_pGridStep[2]->SetItem(&stGV);
		m_pGridStep[2]->SetColumnWidth(i, 47);
//		m_pGridStep[2]->SetItemFont(0, i, &g_lf);

		stGV.row = 1;
		stGV.strText.Format(L"%d-%d", j, k);
		m_pGridStep[2]->SetItem(&stGV);
//		m_pGridStep[2]->SetItemFont(1, i, &g_lf);
		k++;
		if (k > 5)
		{
			j++;	k = 1;
		}
		stGV.row = 2;
		stGV.strText.Format(L"%d-%d", l, m);
		m_pGridStep[2]->SetItem(&stGV);
//		m_pGridStep[2]->SetItemFont(2, i, &g_lf);
		m++;
		if (m > 5)
		{
			l++;	m = 1;
		}
	}
	stGV.row	= 0;
	stGV.col	= iColCnt - 1;
	stGV.strText= L"Result";
	m_pGridStep[2]->SetItem(&stGV);
	m_pGridStep[2]->SetColumnWidth(stGV.col, 80);
//	m_pGridStep[2]->SetItemFont(0, stGV.col, &g_lf);
	stGV.row	= 1;
	stGV.strText= L"Avg.Step.Y";
	m_pGridStep[2]->SetItem(&stGV);
//	m_pGridStep[2]->SetItemFont(0, stGV.col, &g_lf);
	stGV.row	= 2;
	stGV.strText= L"POS.Delay";
	m_pGridStep[2]->SetItem(&stGV);
//	m_pGridStep[2]->SetItemFont(1, stGV.col, &g_lf);
	/* Ÿ��Ʋ (ù ��° ��) */
	for (i=0; i<5; i++)
	{
		stGV.row	= i;
		stGV.col	= 0;
		stGV.strText= tzRows[i];
		m_pGridStep[2]->SetItem(&stGV);
//		m_pGridStep[2]->SetItemFont(i, 0, &g_lf);
	}
	/* ���� �� ��� �κ� �ʱ�ȭ */
	for (i=3; i<5; i++)
	{
		for (j=1; j<iColCnt; j++)
		{
			stGV.row	= i;
			stGV.col	= j;
			stGV.strText= L"";
			stGV.nFormat = DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
			stGV.crBkClr = RGB(255, 255, 255);
			stGV.crFgClr = RGB(0, 0, 0);
			m_pGridStep[2]->SetItem(&stGV);
//			m_pGridStep[2]->SetItemFont(i, j, &g_lf);
		}
	}

	/* ���� ��� ���� ���� ���� �� ���� ���� */
	for (i=5; i<iColCnt; i+=5)
	{
		stGV.row	= 3;
		stGV.col	= i;
		stGV.crBkClr= RGB(215, 240, 135);
		stGV.crFgClr = RGB(0, 0, 0);
		m_pGridStep[2]->SetItem(&stGV);

		stGV.row	= 4;
		stGV.col	= i;
		stGV.crBkClr= RGB(215, 240, 135);
		stGV.crFgClr = RGB(0, 0, 0);
		m_pGridStep[2]->SetItem(&stGV);
	}

	m_pGridStep[2]->SetBaseGridProp(3, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGridStep[2]->UpdateGrid();
}

/*
 desc : �׸��� ��Ʈ�� ����
 parm : None
 retn : None
*/
VOID CDlgMain::InitGridData()
{
	TCHAR tzCols[5][16]	= { L"Item", L"PH.2", L"PH.3", L"PH.4", L"PH.5" };
	TCHAR tzRows[3][16]	= { L"Prev", L"This", L"Save" };
	INT32 i, i32ColCnt	= 5, i32RowCnt = 4, k, j;
	INT32 i32Width[5]	= { 50, 69, 69, 69, 69 };
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;

	GV_ITEM stGV = { NULL };
	CRect rGrid;

	for (k=0; k<2; k++)
	{
		// ���� ���� Client ũ��
		m_grp_ctl[9+k].GetWindowRect(rGrid);
		ScreenToClient(rGrid);
		rGrid.left	+= 15;
		rGrid.right	-= 15;
		rGrid.top	+= 24;
		rGrid.bottom-= 15;

		// �׸��� ��Ʈ�� �⺻ ���� �Ӽ�
		stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
		/* ��ü ���� */
		m_pGridStep[k] = new CGridCtrl;
		ASSERT(m_pGridStep[k]);
		m_pGridStep[k]->SetDrawScrollBarVert(FALSE);
		m_pGridStep[k]->SetDrawScrollBarHorz(FALSE);
		if (!m_pGridStep[k]->Create(rGrid, this, IDC_MAIN_GRID_RESULT_X+k, dwStyle))
		{
			delete m_pGridStep[k];
			m_pGridStep[k] = NULL;
		}
		/* ��ü �⺻ ���� */
		m_pGridStep[k]->SetLogFont(g_lf);
		m_pGridStep[k]->SetRowCount(i32RowCnt);
		m_pGridStep[k]->SetColumnCount(i32ColCnt);
		m_pGridStep[k]->SetFixedRowCount(1);
		m_pGridStep[k]->SetRowHeight(0, 26);
		m_pGridStep[k]->SetFixedColumnCount(0);
		m_pGridStep[k]->SetBkColor(RGB(255, 255, 255));
		m_pGridStep[k]->SetFixedColumnSelection(0);

		/* Ÿ��Ʋ (ù ��° ��) */
		stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
		stGV.crBkClr = RGB(214, 214, 214);
		stGV.crFgClr = RGB(0, 0, 0);
		stGV.lfFont	= g_lf;
		for (i=0; i<i32ColCnt; i++)
		{
			stGV.row	= 0;
			stGV.col	= i;
			stGV.strText= tzCols[i];
			m_pGridStep[k]->SetItem(&stGV);
			m_pGridStep[k]->SetColumnWidth(i, i32Width[i]);
		}
		/* Ÿ��Ʋ (ù ��° ��) */
		stGV.crBkClr = RGB(214, 214, 214);
		stGV.lfFont	= g_lf;
		for (i=1; i<i32RowCnt; i++)
		{
			stGV.row	= i;
			stGV.col	= 0;
			stGV.strText= tzRows[i-1];
			m_pGridStep[k]->SetItem(&stGV);
			m_pGridStep[k]->SetRowHeight(i, 26);
		}

		/* ���� */
		stGV.nFormat = DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.crBkClr = RGB(255, 255, 255);
		stGV.lfFont	= g_lf;
		for (i=1; i<i32RowCnt; i++)
		{
			for (j=1; j<i32ColCnt; j++)
			{
				stGV.row	= i;
				stGV.col	= j;
				stGV.strText= L"";
				m_pGridStep[k]->SetItem(&stGV);
			}
		}

		m_pGridStep[k]->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
		m_pGridStep[k]->UpdateGrid();
	}
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
	/* Update Step X */
	UpdateStepXY();
#ifndef _DEBUG
	/* ��� ��Ʈ���� �����̴� ���ο� ���� ��ư Enable / Disable */
	EnableCtrl();
#endif
	/* Photohead Offset ��û */
	if (!m_bReqPhOffset && uvCmn_Luria_IsConnected())
	{
		m_bReqPhOffset	= uvEng_Luria_ReqGetPhotoheadOffInfo(uvEng_GetConfig()->ph_step.scroll_mode);
	}
	else
	{
		/* Update the Photohead Offset */
		UpdatePhOffset();
	}

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
		if (GetTickCount64() > (m_u64DrawTime+100))
		{
			/* ���� ��µ� ������ �ð� ���� */
			m_u64DrawTime	= GetTickCount64();
			InvalidateView();
		}
		m_u8ViewMode	= 0x01;	/* Live Mode ���� */
	}
	else	/* Grabbed Mode */
	{
		/* ��� ������ Live Mode���ٰ� Grab Mode�� �ٲ� ��� */
		if (0x01 == m_u8ViewMode)
		{
			InvalidateView();
			m_u8ViewMode	= 0x00;	/* Grab Mode ���� */
		}
	}
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
	m_edt_flt[5].SetTextToNum(uvCmn_MCQ_GetACamCurrentPosZ(0x01),				4, TRUE);
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
	case IDC_MAIN_BTN_MC2_RESET			:	MC2Recovery();							break;
	case IDC_MAIN_BTN_ACAM_Z_MOVE		:	ACamMoveAbs();							break;
	case IDC_MAIN_BTN_MARK_MATCH		:	m_btn_ctl[4].EnableWindow(FALSE);
											SetMatchModel();
											m_btn_ctl[4].EnableWindow(TRUE);		break;
	case IDC_MAIN_BTN_MARK_CHANGE		:	SetChangeVal();							break;
	case IDC_MAIN_BTN_MEASURE_MOVE		:	InitMeasureMove();						break;
	case IDC_MAIN_BTN_MEASURE_SAVE		:	SaveMeasureMove();						break;
	case IDC_MAIN_BTN_MOVE_UNLOAD		:	UnloadPosition();						break;
	case IDC_MAIN_BTN_SAVE_OFFSET		:	SavePhOffset();							break;
	case IDC_MAIN_BTN_STEP_RESET		:	ResetMeasureXY();						break;
	case IDC_MAIN_BTN_STEP_FILE			:	SaveAsExcelFile();						break;
	case IDC_MAIN_BTN_GET_SIZE			:	GetGrabSize();							break;
	case IDC_MAIN_BTN_SET_SIZE			:	SetGrabSize();							break;
	case IDC_MAIN_BTN_MODEL_ADD			:	SetRegistModel();						break;
	case IDC_MAIN_BTN_MC2_MOVE_UP		:	MotionMove(ENG_MDMD::en_move_up);		break;
	case IDC_MAIN_BTN_MC2_MOVE_DN		:	MotionMove(ENG_MDMD::en_move_down);		break;
	case IDC_MAIN_BTN_ACAM_HOMING		:	ACamZAxisHoming();						break;
	case IDC_MAIN_BTN_EXIT				:	PostMessage(WM_CLOSE, 0, 0L);			break;
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
	case IDC_MAIN_CHK_LIVE_VIEW		:	SetLiveView();	break;
	case IDC_MAIN_CHK_MARK_SAVE		:	break;
	}
}

/*
 desc : Ʈ���� 1�� �߻� ��Ŵ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::PutOneTrigger()
{
	UINT8 u8ChNo= 0x01;
	BOOL bSucc	= FALSE;

	/* ���� Live & Edge & Calibration ������ �ʱ�ȭ */
	uvEng_Camera_ResetGrabbedMark();
	bSucc	= uvEng_Trig_ReqTrigOutOne(u8ChNo, FALSE);

	if (!bSucc)	AfxMessageBox(L"Failed to send the event for trigger", MB_ICONSTOP|MB_TOPMOST);

	return bSucc;
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
			bSucc = uvEng_Trig_ReqEncoderLive(0x01);
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
	UINT32 u32Model	= (UINT32)ENG_MMDT::en_circle;
	DOUBLE dbMSize	= 0.0f /* um */, dbMColor = 256 /* 256:Black, 128:White */;
	LPG_CIEA pstCfg	= uvEng_GetConfig();
	/* �˻� ��� ���� �� ��� */
	dbMSize	= pstCfg->ph_step.model_dia_size * 1000.0f;
	dbMColor= pstCfg->ph_step.model_color;
	if (!uvEng_Camera_SetModelDefineEx(0x01,
									   pstCfg->mark_find.model_speed,
									   pstCfg->mark_find.model_smooth,
									   &u32Model, &dbMColor, &dbMSize,
									   NULL, NULL, NULL, 1))	/* ����Ϸ��� ���� ������ �ݵ�� 1�� */
	{
		AfxMessageBox(L"Failed to setup the model for search target", MB_ICONSTOP|MB_TOPMOST);
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
 desc : Align Camera - Homing
 parm : None
 retn : None
*/
VOID CDlgMain::ACamZAxisHoming()
{
	uvEng_MCQ_SetACamHomingZAxis(0x01);
	uvEng_MCQ_SetACamHomingZAxis(0x02);
}

/*
 desc : Align Camera (Motor Drive) �̵� (���� ��ġ�� �̵�)
 parm : None
 retn : None
*/
VOID CDlgMain::ACamMoveAbs()
{
	DOUBLE dbPosZ	= 0.0f;	/* unit : mm */

	/* Photohead or Align Camera Z Axis �̵� */
	dbPosZ	= m_edt_flt[16].GetTextToDouble();
	/* ���ο� ��ġ�� �̵� */
	uvEng_MCQ_SetACamMovePosZ(0x01, 0x00, dbPosZ);
}

/*
 desc : Mark �˻� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::SetMatchModel()
{
	TCHAR tzDist[32]	= {NULL};
	BOOL bIsChanged		= FALSE;
	UINT16 u16Toggle	= 0;
	DOUBLE dbAxis		= 0.0f, dbVelo = 0.0f, dbTemp = 0.0f;
	UINT64 u64TickLoop	= 0, u64TickEnter = GetTickCount64();
	DOUBLE dbMirrorSize	= uvEng_GetConfig()->luria_svc.pixel_size;

	/* ���� ��� �� �ʱ�ȭ */
	m_edt_flt[4].SetWindowTextW(L"+0.0000");
	m_edt_flt[5].SetWindowTextW(L"+0.0000");
	m_edt_flt[6].SetWindowTextW(L"+0.0000");
	m_edt_flt[7].SetWindowTextW(L"+0.0000");
	m_edt_flt[8].SetWindowTextW(L"+0.0000");
	m_edt_flt[9].SetWindowTextW(L"+0.0000");
	/* ��ü ȭ�� ���� */
	Invalidate(TRUE);

	/* ���� Live Mode ������� Ȯ�� */
	if (m_chk_ctl[5].GetCheck())
	{
		AfxMessageBox(L"Currently running in <live mode>", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* ���� ��ϵ� ���� �����ϴ��� ���� */
	if (!uvEng_Camera_IsSetMarkModel(0x01, 0x01))
	{
		AfxMessageBox(L"There are no registered models", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* Align Camera is Edge Detection Mode */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);

	do {

		/* Ʈ���� 1�� �߻� */
		PutOneTrigger();
		/* Trigger �߻��� �ð� ���� */
		u64TickLoop	= GetTickCount64();

		/* Grabbed Image�� ��Ī ����� �����ϴ��� ���� Ȯ�� */
		do {

			/* Grabbed Image�� �����ϴ��� Ȯ�� */
			m_pstGrabData[0].marked	= 0x00;
			m_pstGrabData[1].marked	= 0x00;
			if (uvEng_Camera_RunModelStep(0x01, MODEL_FIND_COUNT, FALSE, m_pstGrabData))
			{
				/* ����� ī�޶� ȸ���Ǿ� �ִٸ� ... (180�� �̻�) */
				if (uvEng_GetConfig()->set_cams.acam_inst_angle)
				{
					bIsChanged	= m_pstGrabData[0].mark_cent_mm_x < m_pstGrabData[1].mark_cent_mm_x;
				}
				else
				{
					bIsChanged	= m_pstGrabData[0].mark_cent_mm_x > m_pstGrabData[1].mark_cent_mm_x;
				}
				/* X ��ǥ ���� ���� ������ ���ġ */
				if (bIsChanged)
				{
					STG_ACGR stTemp	= {NULL};
					memcpy(&stTemp,				&m_pstGrabData[0],	sizeof(STG_ACGR));
					memcpy(&m_pstGrabData[0],	&m_pstGrabData[1],	sizeof(STG_ACGR));
					memcpy(&m_pstGrabData[1],	&stTemp,			sizeof(STG_ACGR));
				}
				break;
			}
			/* ���� �ð� ���� ������ ������ ���� ���� ���� */
			if (u64TickLoop+3000 < GetTickCount64())
			{
				AfxMessageBox(L"Could not get the image from the alignment camera", MB_ICONSTOP|MB_TOPMOST);
				return FALSE;
			}
			Sleep(100);

		} while (1);

		if (m_pstGrabData[0].marked && m_pstGrabData[1].marked)
		{
			/* Trigger Board�� Trigger Disable ó�� */
			uvEng_Trig_ReqTriggerStrobe(FALSE);
			/* ���� Grabbed Image 2���� �߽� ���� ������ ����� ���ϰ�, 1��° ī�޶��� ���� �̵� �Ÿ� ���� */
			dbTemp	= (m_pstGrabData[0].mark_cent_mm_dist + m_pstGrabData[1].mark_cent_mm_dist) / 2.0f;
			dbTemp	= dbTemp - m_pstGrabData[0].mark_cent_mm_dist;
			if (abs(m_pstGrabData[0].mark_cent_mm_dist - m_pstGrabData[1].mark_cent_mm_dist) < dbMirrorSize)	/* 1.8 um �̳��� �ִ��� ���� */
			{
				/* ��Ī�� ��� �� ��� */
				UpdateMatchVal();
				/* ��ü �̹����� ũ�⿡�� ù��° ��ü�� �ι�° ��ü ���̿� �̹����� �߽��� ������ ��� �̵� */
				swprintf_s(tzDist, 32, L"%.4f", m_pstGrabData[0].mark_cent_mm_dist);
				m_edt_flt[14].SetWindowTextW(tzDist);
				swprintf_s(tzDist, 32, L"%.4f", m_pstGrabData[1].mark_cent_mm_dist);
				m_edt_flt[15].SetWindowTextW(tzDist);
				break;	/* ���� ��ũ�� �̹��� �߽����� ���� �ּ� 0.5 mm ���Ϸ� ������ ������, ���� ���� ���� */
			}
			/* ���� ���õ� ����� ī�޶� �̵��ϱ� ���� �� ���� */
			dbAxis	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam1) + dbTemp;
			/* ���� ����� ī�޶��� Toggle �� ���� */
			uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
			/* ���� ���õ� ����� ī�޶��� �̵� �ӵ� �� ��� */
			dbVelo	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_align_cam1];
			/* ī�޶� ������ ��ġ�� �̵� */
			if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, dbAxis, dbVelo))
			{
				AfxMessageBox(L"Failed to move the motion drive of alignment camera", MB_ICONSTOP|MB_TOPMOST);
				return FALSE;
			}
			TRACE(L"Align Carmera Moving = %.4f\n", dbAxis);
		}

		/* ���������� �̵��� �Ϸ�Ǿ��ٸ� */
		u64TickLoop	= GetTickCount64();
		while (1)
		{
			/* ���������� �̵��� �Ϸ� �Ǿ��ٸ� ���� ���� ���� */
			if (uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1))	break;
			/* Ÿ�� �ƿ��� �߻� �ߴٸ� ���� ǥ�� */
			if (u64TickLoop+2000 < GetTickCount64())
			{
				AfxMessageBox(L"Failed to move the motion drive of alignment camera", MB_ICONSTOP|MB_TOPMOST);
				return FALSE;
			}
			Sleep(100);
		}

		/* ���� ���� ���� �����ߴٸ� */
		if (u64TickEnter+10000 < GetTickCount64())
		{
			AfxMessageBox(L"Measurement operation timed out.", MB_ICONSTOP|MB_TOPMOST);
			break;
		}

	} while (1);

	/* �ڵ����� ���� ��ġ�� �̵�. Set the grid üũ ��ư�� üũ �� ��츸 �ش��. */
	if (m_chk_ctl[9].GetCheck())
	{
		uvEng_MC2_SendDevRefMove(ENG_MMMM::en_move_step, ENG_MMDI::en_align_cam1,
								 ENG_MDMD::en_move_right, uvEng_GetConfig()->ph_step.stripe_width);
	}

	return TRUE;
}

/*
 desc : ���� ������ �� ��ȯ ����
 parm : None
 retn : None
*/
VOID CDlgMain::SetChangeVal()
{
	STG_ACGR stTemp	= {NULL};

	/* 1 �� �� �ӽÿ� ���� */
	memcpy(&stTemp, &m_pstGrabData[0], sizeof(STG_ACGR));
	/* 2 �� �� 1 �� ���� ����� */
	memcpy(&m_pstGrabData[0], &m_pstGrabData[1], sizeof(STG_ACGR));
	/* �ӽ� �� 2 �� ���� ����� */
	memcpy(&m_pstGrabData[1], &stTemp, sizeof(STG_ACGR));
	/* �� ���� */
	UpdateMatchVal();
}

/*
 desc : ���� ������ �� ����
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateMatchVal()
{
	TCHAR tzVal[64]	= {NULL};
	DOUBLE dbStepX	= 0.0f, dbStepY = 0.0f, dbRotate = 1.0f;

	/* ���а� ȸ�� ��ġ ���ο� ���� */
	dbRotate= uvEng_GetConfig()->luria_svc.ph_rotate == 1 ? -1.0f : 1.0f;
	/* X1 & Y1 / X2 & Y2�� ���� �˻��� Mark�� �߽� ��ǥ �� */
	if (0x00 != m_pstGrabData[0].marked)
	{
		swprintf_s(tzVal, 64, L"%.4f", m_pstGrabData[0].mark_cent_mm_x);
		m_edt_flt[6].SetWindowTextW(tzVal);
		swprintf_s(tzVal, 64, L"%.4f", m_pstGrabData[0].mark_cent_mm_y);
		m_edt_flt[7].SetWindowTextW(tzVal);
	}
	if (0x00 != m_pstGrabData[1].marked)
	{
		swprintf_s(tzVal, 64, L"%.4f", m_pstGrabData[1].mark_cent_mm_x);
		m_edt_flt[8].SetWindowTextW(tzVal);
		swprintf_s(tzVal, 64, L"%.4f", m_pstGrabData[1].mark_cent_mm_y);
		m_edt_flt[9].SetWindowTextW(tzVal);
	}
	/* �⺻ X / Y ������ �Ÿ� ���� (����: �ȼ�) */
	dbStepX	= m_pstGrabData[1].mark_cent_px_x - m_pstGrabData[0].mark_cent_px_x;
	dbStepY	= -(m_pstGrabData[1].mark_cent_px_y - m_pstGrabData[0].mark_cent_px_y);
	/* ������ ���� (Pixel)�� ���� �� 1 �ȼ��� ũ�� (um)�� ���ϸ�, ���� �غ��� ���� (mm) ��� */
	dbStepX	= sqrt(pow(dbStepX, 2.0) - pow(dbStepY, 2.0)) * (uvEng_GetConfig()->acam_spec.spec_pixel_um[0] / 1000.0f);
	/* ���� ������ ���� (mm)���� ���� �������� �� ���� (mark_period: mm)�� ���� ���� (mm)�� ���ϰ�, ���⼭ 1000.0f�� ���ϸ� um ����  */
	dbStepX	= dbStepX - (uvEng_GetConfig()->ph_step.mark_period);
	dbStepY	= dbStepY * (uvEng_GetConfig()->acam_spec.spec_pixel_um[0] / 1000.0f) * dbRotate /* ���а� ȸ�� ���ο� ���� */;
	/* Grid ���� ���� */
	UpdateMatchGrid(dbStepX, dbStepY);

	/* ��� �� ��� */
	swprintf_s(tzVal, 64, L"%+.4f", dbStepX);	/* X ��ǥ�� �ڿ��� �տ����� ���� �� */
	m_edt_flt[10].SetWindowTextW(tzVal);
	swprintf_s(tzVal, 64, L"%+.4f", dbStepY);	/* Y ��ǥ�� �տ��� �ڿ����� ���� �� */
	m_edt_flt[11].SetWindowTextW(tzVal);

	/* Grabbed Image ���� */
	InvalidateView();
}

/*
 desc : �׸��� ������ �� �����ϱ�
 parm : step_x	- [in]  ���� X ��ǥ �� (����: mm)
		step_y	- [in]  ���� Y ��ǥ �� (����: mm)
 retn : None
*/
VOID CDlgMain::UpdateMatchGrid(DOUBLE step_x, DOUBLE step_y)
{
	TCHAR tzValue[32]	= {NULL};
	UINT8 i				= 0, u8GridPos	= (UINT8)m_edt_int[0].GetTextToNum();
	DOUBLE dbValue		= 0.0f;
	ENG_MMDI enDrvID	= ENG_MMDI::en_align_cam1;
	
	if (1 != m_chk_ctl[9].GetCheck() || u8GridPos > 24)
	{
		/* �׸��� ���� ���� ��Ȱ��ȭ */
		m_pGridStep[2]->SetRedraw(FALSE);
		/* ���� ��ġ�� ���� Y �� ��� */
		m_pGridStep[2]->SetItemDouble(3, 1, step_x * 1000.0f, 1);
		m_pGridStep[2]->SetItemDouble(4, 1, step_y * 1000.0f, 1);
		/* �׸��� ���� ���� Ȱ��ȭ */
		m_pGridStep[2]->SetRedraw(TRUE);
		m_pGridStep[2]->Invalidate(FALSE);
		return;
	}

	/* ���� ī�޶��� ���� ��ġ ���� ������ ������ ��ġ �� ���ؼ� �ٸ� ��츸 (5 um ���ؿ��� ��) */
	dbValue	= ROUNDED(m_dbLastACamPos, 3) - ROUNDED(uvCmn_MC2_GetDrvAbsPos(enDrvID), 3);
	if (abs(dbValue) < 0.005)	return;

	/* �׸��� ���� ���� ��Ȱ��ȭ */
	m_pGridStep[2]->SetRedraw(FALSE);

	/* ���� ��ġ�� ���� Y �� ��� */
	m_pGridStep[2]->SetItemDouble(3, u8GridPos, step_x * 1000.0f, 1);
	m_pGridStep[2]->SetItemDouble(4, u8GridPos, step_y * 1000.0f, 1);
	/* ���� X �� ��� */
	switch (u8GridPos)
	{
	case 05	:	/* Step X / Y */
				dbValue	= m_pGridStep[2]->GetItemTextToFloat(3, 5);
				m_pGridStep[0]->SetItemDouble(2, 1, dbValue, 1);	m_pGridStep[0]->InvalidateRowCol(2, 1);
				dbValue	= m_pGridStep[2]->GetItemTextToFloat(4, 5);
				m_pGridStep[1]->SetItemDouble(2, 1, dbValue, 1);	m_pGridStep[1]->InvalidateRowCol(2, 1);
				break;
	case 10	:	/* Step X / Y */
				dbValue	= m_pGridStep[2]->GetItemTextToFloat(3, 5) + m_pGridStep[2]->GetItemTextToFloat(3, 10);
				m_pGridStep[0]->SetItemDouble(2, 2, dbValue, 1);	m_pGridStep[0]->InvalidateRowCol(2, 2);
				dbValue	= m_pGridStep[2]->GetItemTextToFloat(4, 5) + m_pGridStep[2]->GetItemTextToFloat(4, 10);
				m_pGridStep[1]->SetItemDouble(2, 2, dbValue, 1);	m_pGridStep[1]->InvalidateRowCol(2, 2);
				break;
	case 15	:	/* Step X / Y */
				dbValue	= m_pGridStep[2]->GetItemTextToFloat(3, 5) + m_pGridStep[2]->GetItemTextToFloat(3, 10)+
						  m_pGridStep[2]->GetItemTextToFloat(3, 15);
				m_pGridStep[0]->SetItemDouble(2, 3, dbValue, 1);	m_pGridStep[0]->InvalidateRowCol(2, 3);
				dbValue	= m_pGridStep[2]->GetItemTextToFloat(4, 5) + m_pGridStep[2]->GetItemTextToFloat(4, 10)+
						  m_pGridStep[2]->GetItemTextToFloat(4, 15);
				m_pGridStep[1]->SetItemDouble(2, 3, dbValue, 1);	m_pGridStep[1]->InvalidateRowCol(2, 3);
				break;
	case 20	:	/* Step X / Y */
				dbValue	= m_pGridStep[2]->GetItemTextToFloat(3, 5) + m_pGridStep[2]->GetItemTextToFloat(3, 10)+
						  m_pGridStep[2]->GetItemTextToFloat(3, 15)+ m_pGridStep[2]->GetItemTextToFloat(3, 20);
				m_pGridStep[0]->SetItemDouble(2, 4, dbValue, 1);	m_pGridStep[0]->InvalidateRowCol(2, 4);
				dbValue	= m_pGridStep[2]->GetItemTextToFloat(4, 5) + m_pGridStep[2]->GetItemTextToFloat(4, 10)+
						  m_pGridStep[2]->GetItemTextToFloat(4, 15)+ m_pGridStep[2]->GetItemTextToFloat(4, 20);
				m_pGridStep[1]->SetItemDouble(2, 4, dbValue, 1);	m_pGridStep[1]->InvalidateRowCol(2, 4);
				break;
	}
	/* ������ ����� ��ġ�� �̵� ��Ŵ */
	m_edt_int[0].SetTextToNum(u8GridPos+1);
	/* ��ü ������ �ߴٸ�, ��� Y ������ Positive (or Negative) �� ��� �� ��� */
	if (u8GridPos == 24)
	{
		/* ��� Y ���� �� ��� */
		for (i=01; i<05; i++)	dbValue	+= abs(m_pGridStep[2]->GetItemTextToFloat(4, i));
		for (i=06; i<10; i++)	dbValue	+= abs(m_pGridStep[2]->GetItemTextToFloat(4, i));
		for (i=11; i<15; i++)	dbValue	+= abs(m_pGridStep[2]->GetItemTextToFloat(4, i));
		for (i=16; i<20; i++)	dbValue	+= abs(m_pGridStep[2]->GetItemTextToFloat(4, i));
		for (i=21; i<25; i++)	dbValue	+= abs(m_pGridStep[2]->GetItemTextToFloat(4, i));
		m_pGridStep[2]->SetItemDouble(3, 25, dbValue/20.0f, 2);
	}
	/* ���� Align Camera ��ġ �� ���� */
	m_dbLastACamPos	= uvCmn_MC2_GetDrvAbsPos(enDrvID);
	
	/* �׸��� ���� ���� Ȱ��ȭ */
	m_pGridStep[2]->SetRedraw(TRUE);
	m_pGridStep[2]->Invalidate(FALSE);
}

/*
 desc : �׸��� ���� ���� �ʱ�ȭ (����)
 parm : None
 retn : None
*/
VOID CDlgMain::ResetMeasureXY()
{
	UINT8 i, j, k;

	m_dbLastACamPos	= 0.0f;

	m_edt_int[0].SetTextToNum(1);
	/* �׸��� ���� ���� ��Ȱ��ȭ */
	m_pGridStep[2]->SetRedraw(FALSE);
	/* �׸��� ���� �ʱ�ȭ */
	for (i=3; i<5; i++)
	{
		for (j=1; j<25; j++)	m_pGridStep[2]->SetItemText(i, j, L"0.0");
	}
	/* �׸��� ���� ���� Ȱ��ȭ */
	m_pGridStep[2]->SetRedraw(TRUE);
	m_pGridStep[2]->Invalidate(FALSE);

	for (k=0; k<2; k++)
	{
		/* �׸��� ���� ���� ��Ȱ��ȭ */
		m_pGridStep[k]->SetRedraw(FALSE);
		/* �׸��� ���� �ʱ�ȭ */
		for (i=1; i<4; i++)
		{
			for (j=1; j<5; j++)
			{
				m_pGridStep[k]->SetItemText(i, j, L"0.0");
			}
		}
		/* �׸��� ���� ���� Ȱ��ȭ */
 		m_pGridStep[k]->SetRedraw(TRUE);
 		m_pGridStep[k]->SetRedraw(TRUE);
	}

#if 0
	m_pGridStep[2]->SetItemFloat(3, 1, -1.3f, 1);	m_pGridStep[2]->SetItemFloat(4, 1, 8.8f, 1);
	m_pGridStep[2]->SetItemFloat(3, 2, 0.7f, 1);	m_pGridStep[2]->SetItemFloat(4, 2, -12.7f, 1);
	m_pGridStep[2]->SetItemFloat(3, 3, 0.7f, 1);	m_pGridStep[2]->SetItemFloat(4, 3, 8.5f, 1);
	m_pGridStep[2]->SetItemFloat(3, 4, 0.2f, 1);	m_pGridStep[2]->SetItemFloat(4, 4, -12.4f, 1);
	m_pGridStep[2]->SetItemFloat(3, 5, -141.2f, 1);	m_pGridStep[2]->SetItemFloat(4, 5, -56.6f, 1);
	m_pGridStep[2]->SetItemFloat(3, 6, 3.6f, 1);	m_pGridStep[2]->SetItemFloat(4, 6, 13.0f, 1);
	m_pGridStep[2]->SetItemFloat(3, 7, 2.8f, 1);	m_pGridStep[2]->SetItemFloat(4, 7, -8.6f, 1);
	m_pGridStep[2]->SetItemFloat(3, 8, 2.3f, 1);	m_pGridStep[2]->SetItemFloat(4, 8, 12.6f, 1);
	m_pGridStep[2]->SetItemFloat(3, 9, 3.2f, 1);	m_pGridStep[2]->SetItemFloat(4, 9, -8.7f, 1);

	m_pGridStep[0]->SetItemFloat(2, 1, -141.2f, 1);	m_pGridStep[1]->SetItemFloat(2, 1, -56.6f, 1);	
	m_edt_flt[17].SetTextToNum(0.900f, 3);
	m_edt_int[3].SetTextToNum(1);
	m_edt_int[0].SetTextToNum(10);
#endif
}

/*
 desc : �׸��� ��Ʈ�ѿ� �ִ� �����͸� ���Ϸ� ����
 parm : None
 retn : None
*/
VOID CDlgMain::SaveAsExcelFile()
{
	TCHAR tzTempFile[MAX_PATH_LEN]	= {NULL};
	SYSTEMTIME stTm	= {NULL};

	/* ��ϵ� �����Ͱ� ������ Ȯ�� */
	if (m_pGridStep[2]->GetRowCount() < 2)	return;

	/* ���� �ð� */
	GetLocalTime(&stTm);
	/* ���� �ӽ� ���Ϸ� ���� �� ���� */
	swprintf_s(tzTempFile, MAX_PATH_LEN, L"%s\\vstep\\vstep_%2u%2u%2u_%2u%2u%2u.csv",
			   g_tzWorkDir, stTm.wYear-2000, stTm.wMonth, stTm.wDay,
			   stTm.wHour, stTm.wMinute, stTm.wSecond);

	/* ���Ϸ� ���� */
	m_pGridStep[2]->Save(tzTempFile);

	/* ����� ���� ���� */
	ShellExecute(NULL, _T("open"), _T("excel.exe"), tzTempFile, NULL, SW_SHOW);
}
/*
 desc : �̹��� �� ���� ����
 parm : None
 retn : None
*/
VOID CDlgMain::InvalidateView()
{
	UINT8 i	= 0x00;
	CRect rView;

	for (i=0; i<2; i++)
	{
		m_grp_ctl[i].GetWindowRect(rView);
		ScreenToClient(rView);
		InvalidateRect(rView, TRUE);
	}
}

/*
 desc : �ʱ� ���� ��ġ�� �̵�
 parm : None
 retn : None
*/
VOID CDlgMain::InitMeasureMove()
{
	DOUBLE dbDist	= 0.0f, dbVelo = 0.0f;

	/* ī�޶� 2 ���� (�ܺ� Ÿ��) ��� */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)uvEng_GetConfig()->set_comn.align_camera_kind)
	{
		/* Camera 2�� �̵� */
		dbVelo	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_align_cam2];
		dbDist	= uvEng_GetConfig()->mc2_svc.max_dist[(UINT8)ENG_MMDI::en_align_cam2];
		uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2, dbDist, dbVelo);
		/* Camera 1�� �̵� */
		dbVelo	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_align_cam1];
		uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, uvEng_GetConfig()->ph_step.start_acam_x, dbVelo);
	}

	/* ī�޶� Z Axis �̵� */
	uvEng_MCQ_SetACamMovePosZ(0x01, 0x00, uvEng_GetConfig()->acam_focus.acam_z_focus[0x00]);

	/* Stage X �̵� */
	dbVelo	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_x];
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, uvEng_GetConfig()->ph_step.start_xy[0x00], dbVelo);

	/* Stage Y �̵� */
	dbVelo	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_y];
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, uvEng_GetConfig()->ph_step.start_xy[0x01], dbVelo);
}

/*
 desc : ���� ���� ���� ��ġ ����
 parm : None
 retn : None
*/
VOID CDlgMain::SaveMeasureMove()
{
	uvEng_GetConfig()->ph_step.start_acam_x		= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam1);
	uvEng_GetConfig()->ph_step.start_xy[0x00]	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);
	uvEng_GetConfig()->ph_step.start_xy[0x01]	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);

	/* Init ���Ϸ� ���� */
	uvEng_SaveConfig();
}

/*
 desc : ���� ������ ���а� ���� ���� �� ����
 parm : None
 retn : None
 note : �˰��� - �������� (Hysterisis)_v0.31.xlsx ���� ����
*/
VOID CDlgMain::SavePhOffset()
{
	UINT8 i	= 0x00, j	= 0x01, k = 0x01, l, u8Scan, u8Sub, u8Div, u8PhCnt;
	INT16 i16Offset		= 0;
	UINT32 u32HysAvgY /* nsec */;
	DOUBLE dbStepY[2]	= {NULL}, dbTotCalcY = 0.0f, dbTotAvgY = 0.0f /* ��ü Y �� ���� ũ�� (����: um) */;
	DOUBLE dbFirst, dbCalcY, dbSpeed /* �뱤 �ӵ� (����: mm/s) */;
	DOUBLE dbPixelSize	= uvEng_GetConfig()->luria_svc.pixel_size, dbOffset, dbMaxSpeed;
	STG_OSSD stStep		= {NULL};
	CDlgStep dlgStep;
	CAtlList <DOUBLE> lstCalc;

	/* ��ϵ� �Ź� ������ �ִ��� ���� */
	if (uvCmn_Luria_GetJobCount())
	{
		AfxMessageBox(L"Registered gerber file exists\nPlease remove all the gerber files", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* Scroll Mode�� Frame Rate ���� �����Ǿ� �ִ��� ���� */
	if (m_edt_flt[17].GetTextToDouble() < 0.001 || m_edt_int[3].GetTextToNum() < 1)
	{
		AfxMessageBox(L"No [Scroll Mode] or [Frame Rate] values are entered", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* �޸� ���� �� �ʱ�ȭ */
	stStep.Reset();

	/* ���а� �뱤 Scan Ƚ�� */
	u8Scan	= uvEng_GetConfig()->ph_step.ph_scan;
	u8Div	= uvEng_GetConfig()->ph_step.ph_scan / 2;
	u8PhCnt	= uvEng_GetConfig()->luria_svc.ph_count;

	/* ���а� ���� X (��/��) ���� �� ��� */
	for (i=1; i<u8PhCnt; i++)
	{
		stStep.step_x_nm[i]	= (UINT32)ROUNDED(m_pGridStep[0]->GetItemTextToFloat(3, i) * 1000.0f, 0);
		stStep.step_y_nm[i]	= (INT32)ROUNDED(m_pGridStep[1]->GetItemTextToFloat(3, i) * 1000.0f, 0);
	}
	/* Frame Rate �� Scroll �� ��� */
	stStep.frame_rate	= (UINT32)ROUNDED(m_edt_flt[17].GetTextToDouble() * 1000.0f, 0);
	stStep.scroll_mode	= (UINT8)m_edt_int[3].GetTextToNum();

	/* ��/�� Ƚ�� */
	u8Sub	= u8Scan % 2;
	/* ���а� �� ��/�� ��ĵ ���� �� ���� �� ��� ���ϰ�, ��ü ��� �ӽ� ���� */
	for (i=0x00,k=0x01; i<u8PhCnt; i++, k+=u8Scan)
	{
		dbStepY[0]	= dbStepY[1] = 0.0f;
		/* ������帶�� ���� ��/�� Ƚ���� Ȧ���� ���, ������ �ѹ��� ��� ���� */
		/* ���� ���� j ���� 2 �� ��� ���̸�, ���� ���ϰ� ����, �� �տ� ���ϱ� */
		for (j=0x00,l=k; j<u8Div; j++,l+=2)
		{
			dbStepY[0]	+= m_pGridStep[2]->GetItemTextToFloat(4, l);
			dbStepY[1]	+= m_pGridStep[2]->GetItemTextToFloat(4, l+1);
		}
		/* ���а� ������ ����� ���� �� */
		dbTotAvgY	+= (dbStepY[1] / DOUBLE(u8Div) - dbStepY[0] / DOUBLE(u8Div)) / 2.0f;
	}
	/* ���а� ��ü ��� */
	dbTotAvgY	/= DOUBLE(u8PhCnt);
	/* 1�� ���Ŀ� ���� Negative Offset �� ���ϱ� */
	dbFirst	= m_pGridStep[2]->GetItemTextToFloat(4, 1);
	if (dbFirst >= 0.0f)
	{
		i16Offset	= (INT16)(dbTotAvgY / dbPixelSize);	/* �Ҽ��� ���ϴ� ������ ������ ���� ���� Casting ���� */
	}
	else
	{
		i16Offset	= 1 + (INT16)(dbTotAvgY / dbPixelSize);	/* �Ҽ��� ���ϴ� ������ ������ ���� ���� Casting ���� */
	}

	/* �߰� �ȼ� ũ�� ���ϱ� */
	dbOffset	= abs(i16Offset) * dbPixelSize;
	/* �߰� ���� �� ���ϱ� (���� ���� ������ ����ϱ� ���� �߰� ��) */
	for (i=0x00,k=0x01; i<u8PhCnt; i++, k+=u8Scan)
	{
		dbCalcY = 0.0f;
		/* ������帶�� ���� ��/�� Ƚ���� Ȧ���� ���, ������ �ѹ��� ��� ���� */
		/* ���� ���� j ���� 2 �� ��� ���̸�, ���� ���ϰ� ����, �� �տ� ���ϱ� */
		for (j=0x00; j<u8Scan; j++)
		{
			/* Scan Ƚ���� Ȧ���� ���, �� ���а��� ������ ���� ó������ ���� */
			if ((u8Scan % 2) != 0 && (j == (u8Scan - 1)))	continue;
			if (0 == (j % 2))
			{
				if (dbFirst <= 0.0f)	dbCalcY	= m_pGridStep[2]->GetItemTextToFloat(4, k+j) + dbOffset;
				else					dbCalcY	= m_pGridStep[2]->GetItemTextToFloat(4, k+j) - dbOffset;
			}
			else
			{
				if (dbFirst > 0.0f)		dbCalcY	= m_pGridStep[2]->GetItemTextToFloat(4, k+j) + dbOffset;
				else					dbCalcY	= m_pGridStep[2]->GetItemTextToFloat(4, k+j) - dbOffset;
			}
			lstCalc.AddTail(dbCalcY);
		}

		/* �߰� ���� �� ��� ���ϱ� */
		dbStepY[0]	= dbStepY[1] = 0.0f;
		for (j=0; j<lstCalc.GetCount(); j+=2)
		{
			dbStepY[0]	+= lstCalc.GetAt(lstCalc.FindIndex(j));
			dbStepY[1]	+= lstCalc.GetAt(lstCalc.FindIndex(j+1));
		}
		/* ��� ���ϱ� */
		u8Div		= UINT8(lstCalc.GetCount() / 2);
		dbTotCalcY	+= -1.0f * (dbStepY[1] / DOUBLE(u8Div) - dbStepY[0] / DOUBLE(u8Div)) / 2.0f;

		/* ���� ���� ��� ����� */
		lstCalc.RemoveAll();
	}

	/* ��ü Y �� ��/�� �뱤 ������ �̵��ؾ� �� Hysterisis �� �� �̵� ũ�� (����: um) ��� */
	dbTotAvgY	= dbTotCalcY / DOUBLE(u8PhCnt);
	/* �뱤 �ӵ� ��� (�ִ� �ӵ� * Frame Rate) (����: mm/sec) */
#if 1
	dbMaxSpeed	= uvCmn_Luria_GetExposeMaxSpeed(1/*(UINT8)m_edt_int[3].GetTextToNum()*/);
	dbSpeed		= dbMaxSpeed * (m_edt_flt[17].GetTextToDouble());
#else
	dbSpeed		= uvEng_GetConfig()->luria_svc.max_print_speed * (m_edt_flt[17].GetTextToDouble());
	dbSpeed		= uvEng_GetConfig()->luria_svc.max_print_speed * (m_edt_flt[17].GetTextToDouble() / 1000.0f);
#endif

	/* ���� Negative�� �̵��ؾ� �� �ȼ� ũ�� �� (���� �ȼ� ���� �� ��) */
	stStep.nega_offset_px	= i16Offset	+ uvEng_ShMem_GetLuria()->machine.hysteresis_type1.negative_offset;
	/* Hysterisis Negative Delay �� ���ϱ� (nsec ���ϱ� ���� 1000000.0f ���ϱ�) */
	u32HysAvgY	= (UINT32)ROUNDED((dbTotAvgY / (dbSpeed / 1000.0f)) * 1000.0f, 0);
	stStep.delay_posi_nsec	= u32HysAvgY / 2;
	stStep.delay_nega_nsec	= u32HysAvgY - stStep.delay_posi_nsec;

	/* ������ ��� �۾� ���� */
	if (IDOK != dlgStep.MyDoModal(&stStep))	return;
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
	DOUBLE dbRadian = 0.0f, dbDegree = 0.0f, dbPixelSize = 0.0f;

	/* ���� 1�� ī�޶� �������� ���� �� ��� */
	dbDegree	= uvEng_GetConfig()->acam_spec.spec_angle[0];
	dbPixelSize	= uvEng_GetConfig()->acam_spec.spec_pixel_um[0];
	/* Convert Degree to Radian */
	dbRadian	= (dbDegree * M_PI) / 180.0f;
	/* ��Ʈ�ѿ� ��� */
	m_edt_flt[12].SetTextToNum(dbDegree, 7);
	/* Align Camera Pixel Size ���� */
	m_edt_flt[13].SetTextToNum(dbPixelSize, 7);
	m_edt_int[0].SetTextToNum((UINT32)1);
	/* The size of Grabbed Image */
	m_edt_int[1].SetTextToNum(uvEng_GetConfig()->set_cams.soi_size[0]);
	m_edt_int[2].SetTextToNum(uvEng_GetConfig()->set_cams.soi_size[1]);
	/* Move Velo and Distance */
	m_edt_flt[0].SetTextToNum(uvEng_GetConfig()->ph_step.stripe_width,	4);
	m_edt_flt[1].SetTextToNum(uvEng_GetConfig()->mc2_svc.move_velo,		4);
}

/*
 desc : ��ư �� ���� ��Ʈ�� Enable or Disable ó��
 parm : None
 retn : None
*/
VOID CDlgMain::EnableCtrl()
{
	UINT8 i;
	BOOL bStopped	= TRUE;
	LPG_CMSI pstMC2Svc	= &uvEng_GetConfig()->mc2_svc;

	/* �ʿ��� ��ư�� Enable or Disable ó�� */
	if (!uvCmn_MC2_IsConnected()	||
		!uvCmn_Camera_IsConnected()	||
		!uvCmn_Trig_IsConnected()	||
		!uvCmn_MCQ_IsConnected())
	{
		for (i=0; i<15; i++)	m_btn_ctl[i].EnableWindow(FALSE);
		for (i=0; i<8; i++)		m_chk_ctl[i].EnableWindow(FALSE);
		m_bEnableLast	= FALSE;
		return;
	}

	/* ���� ���õ� ī�޶� �̵� �������� ���ο� ���� */
	bStopped	= uvEng_ShMem_GetMC2()->GetDriveAct((UINT8)ENG_MMDI::en_stage_x)->IsStopped()	&&
				  uvEng_ShMem_GetMC2()->GetDriveAct((UINT8)ENG_MMDI::en_stage_y)->IsStopped()	&&
				  uvEng_ShMem_GetMC2()->GetDriveAct((UINT8)ENG_MMDI::en_align_cam1)->IsStopped()&&
				  uvEng_ShMem_GetMC2()->GetDriveAct((UINT8)ENG_MMDI::en_align_cam2)->IsStopped();
	/* ���� �ֱ� ���¿� �������� ���� */
	if (bStopped == m_bEnableLast)	return;
	/* ���� �ֱ� ���� ���� */
	m_bEnableLast	= bStopped;

	for (i=0; i<15; i++)	m_btn_ctl[i].EnableWindow(bStopped);
	/* MC2 Drive Check Button */
	for (i=0; i<uvEng_GetConfig()->mc2_svc.drive_count; i++)
	{
		m_chk_ctl[pstMC2Svc->axis_id[i]].EnableWindow(bStopped);
	}
}

/*
 desc : Step X / Y (Photohead ��) ����
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateStepXY()
{
	UINT8 i	= 0;
	DOUBLE dbPrev, dbThis;

	for (i=1; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		dbPrev	= m_pGridStep[0]->GetItemTextToFloat(1, i);
		dbThis	= m_pGridStep[0]->GetItemTextToFloat(2, i);
		m_pGridStep[0]->SetItemDouble(3, i, dbPrev+dbThis, 1);
		m_pGridStep[0]->InvalidateRowCol(3, i);

		dbPrev	= m_pGridStep[1]->GetItemTextToFloat(1, i);
		dbThis	= m_pGridStep[1]->GetItemTextToFloat(2, i);
		m_pGridStep[1]->SetItemDouble(3, i, dbPrev+dbThis, 1);
		m_pGridStep[1]->InvalidateRowCol(3, i);
	}
}

/*
 desc : Photohead Offset �� ����
 parm : None
 retn : None
*/
VOID CDlgMain::UpdatePhOffset()
{
	TCHAR tzValue[32]	= {NULL};
	INT32 i			= 0;
	DOUBLE dbStepY	= 0.0f, dbStepX = 0.0f;
	LPG_MCPO pStepXY= uvEng_ShMem_GetLuria()->machine.ph_offset_xy;

	/* ���� ���� �����ϸ� ���� ���� �ʱ� */
	for (i=1; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		dbStepX	= m_pGridStep[0]->GetItemTextToFloat(1, i);	/* um */
		dbStepY	= m_pGridStep[1]->GetItemTextToFloat(1, i);	/* um */

		if (dbStepX != ROUNDED(pStepXY[i].pos_offset_x / 1000.0f, 1) /* nm -> um */)
		{
			swprintf_s(tzValue, 32, L"%.1f", pStepXY[i].pos_offset_x / 1000.0f);
			m_pGridStep[0]->SetItemText(1, i, tzValue);
			m_pGridStep[0]->InvalidateRowCol(1, i);
		}
		if (dbStepY != ROUNDED(pStepXY[i].pos_offset_y / 1000.0f, 1) /* nm -> um */)
		{
			swprintf_s(tzValue, 32, L"%.1f", pStepXY[i].pos_offset_y / 1000.0f);
			m_pGridStep[1]->SetItemText(1, i, tzValue);
			m_pGridStep[1]->InvalidateRowCol(1, i);
		}
	}
}

/*
 desc : ���� Grabbed Size ���� ���
 parm : None
 retn : None
*/
VOID CDlgMain::GetGrabSize()
{
	m_edt_int[1].SetTextToNum(uvEng_GetConfig()->set_cams.soi_size[0]);
	m_edt_int[2].SetTextToNum(uvEng_GetConfig()->set_cams.soi_size[1]);
}

/*
 desc : ���� Grabbed Size ���� ����
 parm : None
 retn : None
*/
VOID CDlgMain::SetGrabSize()
{
	if (AfxMessageBox(L"Do you want to reconnect the camera?", MB_YESNO) != IDYES)	return;

	uvEng_GetConfig()->set_cams.soi_size[0]	= (UINT32)m_edt_int[1].GetTextToNum();
	uvEng_GetConfig()->set_cams.soi_size[1]	= (UINT32)m_edt_int[2].GetTextToNum();
	uvEng_SaveConfig();
	uvEng_Camera_Reconnect();
	GetGrabSize();
}

