
/*
 desc : Main Dialog
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgMain.h"

#include "MainThread.h"
#include "./Meas/Measure.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
	m_dbACamZAxisPos[0]	= -100.0f;
	m_dbACamZAxisPos[1]	= -100.0f;
	m_pMainThread		= NULL;
	m_bTriggerOn		= FALSE;
	m_u8ACamNo			= 0x01;
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

	/* Normal - Group box */
	u32StartID	= IDC_MAIN_GRP_MC2;
	for (i=0; i<8; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Normal - Text */
	u32StartID	= IDC_MAIN_TXT_MC2_MOVE;
	for (i=0; i<21; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Normal - Button */
	u32StartID	= IDC_MAIN_BTN_MC2_MOVE_UP;
	for (i=0; i<18; i++)	DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);

	/* Picture */
	u32StartID	= IDC_MAIN_PIC_VIEW;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_pic_ctl[i]);

	/* for MC2 */
	u32StartID	= IDC_MAIN_CHK_MC2_DRV_0;
	for (i=0; i<8; i++)		DDX_Control(dx, u32StartID+i,	m_chk_mc2[i]);
	u32StartID	= IDC_MAIN_EDT_MC2_DIST;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_edt_mc2[i]);

	/* for Align Camera */
	u32StartID	= IDC_MAIN_CHK_ACAM_LIVE;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_chk_cam[i]);
	u32StartID	= IDC_MAIN_EDT_ACAM_ABS;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_edt_cam[i]);

	/* for Move Size */
	u32StartID	= IDC_MAIN_EDT_MOVEUP_SIZE;
	for (i=0; i<5; i++)		DDX_Control(dx, u32StartID+i,	m_edt_opt[i]);

	/* for Trigger Time */
	u32StartID	= IDC_MAIN_EDT_TRIG_ONTIME;
	for (i=0; i<3; i++)		DDX_Control(dx, u32StartID+i,	m_edt_trg[i]);

	/* for Work status */
	u32StartID	= IDC_MAIN_EDT_STEP;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_edt_pgr[i]);

}

BEGIN_MESSAGE_MAP(CDlgMain, CMyDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MAIN_THREAD,	OnMsgThread)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_BTN_MC2_MOVE_UP,	IDC_MAIN_BTN_EXIT,		OnBtnClicked)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_CHK_ACAM_LIVE,	IDC_MAIN_CHK_ACAM_LIVE,	OnChkClickACam)
END_MESSAGE_MAP()

/*
 desc : �ý��� ��ɾ� ����
 parm : id		- ���õ� �׸� ID ��
		lparam	- ???
 retn : 1 - ���� / 0 - ����
*/
VOID CDlgMain::OnSysCommand(UINT32 id, LPARAM lparam)
{

	CMyDialog::OnSysCommand(id, lparam);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::OnInitDlg()
{
	/* ����Ǵ� ���α׷��� Sub Monitor�� ��� */
	/*uvCmn_MoveSubMonitor(m_hWnd);*/

	/* ���̺귯�� �ʱ�ȭ */
	if (!InitLib())	return FALSE;

	/* ���� ��Ʈ�� �ʱ�ȭ */
	InitCtrl();
	InitGridMotor();
	InitGridMeas();
	InitSetup();

	/* �⺻ ȯ�� ���� ���� ���� */
	LoadDataConfig();
	LoadTriggerCh();

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
	/* �⺻ ���� ������ �޸� ���� */
	if (m_pMainThread)
	{
		m_pMainThread->Stop();
		while (m_pMainThread->IsBusy())	m_pMainThread->Sleep();
		delete m_pMainThread;
		m_pMainThread	= NULL;
	}

	/* GridControl ���� */
	CloseGrid();
	/* ���̺귯�� ���� !!! �� �������� �ڵ� !!! */
	CloseLib();
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
		uvEng_Camera_DrawCaliBitmap(dc->m_hDC, rDraw);
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
 desc : ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgMain::InitCtrl()
{
	INT32 i;

	/* Normal - Group box */
	for (i=0; i<8; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* Normal - Text */
	for (i=0; i<21; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(TRUE);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* Normal - Button */
	for (i=0; i<18; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
	}
	/* for MC2 */
	for (i=0; i<8; i++)
	{
		m_chk_mc2[i].SetLogFont(g_lf);
		m_chk_mc2[i].SetBgColor(RGB(0xff, 0xff, 0xff));
		m_chk_mc2[i].SetReadOnly(FALSE);
		m_chk_mc2[i].EnableWindow(FALSE);
	}
	for (i=0; i<2; i++)
	{
		m_edt_mc2[i].SetEditFont(&g_lf);
		m_edt_mc2[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_mc2[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_mc2[i].SetInputType(ENM_DITM::en_float);
	}
	/* for Align Camera */
	for (i=0; i<1; i++)
	{
		m_chk_cam[i].SetLogFont(g_lf);
		m_chk_cam[i].SetBgColor(RGB(0xff, 0xff, 0xff));
		m_chk_cam[i].SetReadOnly(FALSE);
		m_chk_cam[i].EnableWindow(FALSE);
	}

	for (i=0; i<1; i++)
	{
		m_edt_cam[i].SetEditFont(&g_lf);
		m_edt_cam[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_cam[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_cam[i].SetInputType(ENM_DITM::en_float);
	}
	/* for Trigger Time */
	for (i=0; i<3; i++)
	{
		m_edt_trg[i].SetEditFont(&g_lf);
		m_edt_trg[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_trg[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_trg[i].SetInputType(ENM_DITM::en_hex10);
	}
	/* for Work Status */
	for (i=0; i<1; i++)
	{
		m_edt_pgr[i].SetEditFont(&g_lf);
		m_edt_pgr[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_pgr[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_pgr[i].SetInputType(ENM_DITM::en_float);
	}
	/* for Measurement Option */
	for (i=0; i<5; i++)
	{
		m_edt_opt[i].SetEditFont(&g_lf);
		m_edt_opt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_opt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		if (i < 0x03)	m_edt_opt[i].SetInputType(ENM_DITM::en_float);
		else			m_edt_opt[i].SetInputType(ENM_DITM::en_int8);
	}

	/* Align Camera Focus���� ������ ���� �������Ƿ� ... */
	m_edt_opt[1].SetReadOnly(TRUE);
	m_edt_opt[1].SetMouseClick(FALSE);
}

/*
 desc : �ʱ� ��Ʈ�� ����
 parm : None
 retn : None
*/
VOID CDlgMain::InitSetup()
{
	UINT32 i	= 0;
	LPG_CMSI pstMC2	= &uvEng_GetConfig()->mc2_svc;
	LPG_CSCI pstCAM	= &uvEng_GetConfig()->set_cams;

	/* for MC2 - üũ ��ư Ȱ��ȭ ó�� */
	for (i=0; i<pstMC2->drive_count; i++)
	{
		m_chk_mc2[pstMC2->axis_id[i]].EnableWindow(TRUE);
	}

	/* for Align Camera - ����Ʈ ��ư Ȱ��ȭ ó�� */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)uvEng_GetConfig()->set_comn.align_camera_kind)
	{
		/* Velo ���� ��ư�� Disable */
		m_edt_mc2[1].EnableWindow(FALSE);
	}
}

/*
 desc : �׸��� ��Ʈ�� ���� (Motor Position)
 parm : None
 retn : None
*/
VOID CDlgMain::InitGridMotor()
{
	TCHAR tzCols[2][16]	= { L"Item", L"Pos (mm)" }, tzRows[32] = {NULL};
	INT32 i32Width[2]	= { 54, 75 }, i, j=1;
	INT32 i32ColCnt		= 2, i32RowCnt = 0;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	LPG_CIEA pstCfg		= uvEng_GetConfig();
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* Last Recv. Packet / PDO Header / PDO Data (Values) */
	::GetWindowRect(m_grp_ctl[3].GetSafeHwnd(), rGrid);
	ScreenToClient(rGrid);
	rGrid.left	+= 15;
	rGrid.right	-= 15;
	rGrid.top	+= 22;
	rGrid.bottom-= 13;

	/* ------------------ */
	/* for Motor Position */
	/* ------------------ */

	/* �� ��µ� �� (Row)�� ���� ���ϱ� */
	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		i32RowCnt	= pstCfg->luria_svc.ph_count + pstCfg->mc2_svc.drive_count;
	}
	else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		i32RowCnt	= pstCfg->mc2_svc.drive_count;
	}
	/* Basler ī�޶��� ��� Z Axis�� ��ġ (����) �� ��� ���� */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)pstCfg->set_comn.align_camera_kind)
	{
		/* Align Camera�� Z Axis ���� ���� ��ġ �� ���� */
		m_u32ACamZAxisRow[0]	= i32RowCnt+1;
		m_u32ACamZAxisRow[1]	= i32RowCnt+2;
		/* Motion Left / Right ��ġ�� Up / Down ���� ��ġ */
		i32RowCnt	+= pstCfg->set_cams.acam_count;
	}

	/* �׸��� ��Ʈ�� �⺻ ���� �Ӽ� */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* ��ü ���� */
	m_pGridMotor	= new CGridCtrl;
	ASSERT(m_pGridMotor);
	m_pGridMotor->SetDrawScrollBarHorz(FALSE);
	m_pGridMotor->SetDrawScrollBarVert(TRUE);
	if (!m_pGridMotor->Create(rGrid, this, IDC_MAIN_GRID_MOTOR_POS, dwStyle))
	{
		delete m_pGridMotor;
		m_pGridMotor = NULL;
		return;
	}
	/* ��ü �⺻ ���� */
	m_pGridMotor->SetLogFont(g_lf);
	m_pGridMotor->SetRowCount(i32RowCnt+1);
	m_pGridMotor->SetColumnCount(i32ColCnt);
	m_pGridMotor->SetFixedRowCount(1);
	m_pGridMotor->SetRowHeight(0, 24);
	m_pGridMotor->SetFixedColumnCount(0);
	m_pGridMotor->SetBkColor(RGB(255, 255, 255));
	m_pGridMotor->SetFixedColumnSelection(0);

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
		m_pGridMotor->SetItem(&stGV);
		m_pGridMotor->SetColumnWidth(i, i32Width[i]);
	}

	/* ���� ���� */
	stGV.crBkClr = RGB(255, 255, 255);
	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* ���� ���� - for Photohead Position */
		for (i=0; i<pstCfg->luria_svc.ph_count; i++,j++)
		{
			m_pGridMotor->SetRowHeight(j, 24);

			/* for Subject */
			stGV.strText.Format(L"PH %d", i+1);
			stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
			stGV.row	= j;
			stGV.col	= 0;
			m_pGridMotor->SetItem(&stGV);

			/* for Subject */
			stGV.strText= L"0.0000";
			stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
			stGV.col	= 1;
			m_pGridMotor->SetItem(&stGV);
		}
	}
	/* ���� ���� - for Motion Drive Position */
	for (i=0; i<pstCfg->mc2_svc.drive_count; i++,j++)
	{
		m_pGridMotor->SetRowHeight(j, 24);

		/* for Subject */
		stGV.strText.Format(L"MC %d", pstCfg->mc2_svc.axis_id[i]);
		stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
		stGV.row	= j;
		stGV.col	= 0;
		m_pGridMotor->SetItem(&stGV);

		/* for Subject */
		stGV.strText= L"0.0000";
		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.col	= 1;
		m_pGridMotor->SetItem(&stGV);
	}

	/* ���� ���� - for Basler Camera Position */
	if (pstCfg->set_comn.align_camera_kind == (UINT8)ENG_VCPK::en_camera_basler_ipv4)
	{
		/* Z Axis Up / Down Postion */
		for (i=0; i<pstCfg->set_cams.acam_count; i++,j++)
		{
			m_pGridMotor->SetRowHeight(j, 24);

			/* for Subject */
			stGV.strText.Format(L"AZ %d", i+1);
			stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
			stGV.row	= j;
			stGV.col	= 0;
			m_pGridMotor->SetItem(&stGV);

			/* for Subject */
			stGV.strText= L"0.0000";
			stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
			stGV.col	= 1;
			m_pGridMotor->SetItem(&stGV);
		}
	}

	m_pGridMotor->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGridMotor->UpdateGrid();
}

/*
 desc : �׸��� ��Ʈ�� ���� (Measurement Result)
 parm : None
 retn : None
*/
VOID CDlgMain::InitGridMeas()
{
	TCHAR tzCols[8][16]	= { L"PH", L"Step", L"Score(%)", L"Scale(%)", 
							L"Coverage(%)", L"Fit Error", L"Repeat", L"ACam.Z(mm)" }, tzRows[32] = {NULL};
	INT32 i32Width[8]	= { 50, 48, 70, 70, 85, 65, 50, 90 }, i, j=1;
	INT32 i32ColCnt		= 8, i32RowCnt = 0;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	LPG_CIEA pstCfg		= uvEng_GetConfig();
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* Last Recv. Packet / PDO Header / PDO Data (Values) */
	::GetWindowRect(m_grp_ctl[7].GetSafeHwnd(), rGrid);
	ScreenToClient(rGrid);
	rGrid.left	+= 15;
	rGrid.right	-= 15;
	rGrid.top	+= 22;
	rGrid.bottom-= 13;

	/* �׸��� ��Ʈ�� �⺻ ���� �Ӽ� */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* ��ü ���� */
	m_pGridMeas	= new CGridCtrl;
	ASSERT(m_pGridMeas);
	m_pGridMeas->SetDrawScrollBarHorz(FALSE);
	m_pGridMeas->SetDrawScrollBarVert(TRUE);
	if (!m_pGridMeas->Create(rGrid, this, IDC_MAIN_GRID_MEAS_DATA, dwStyle))
	{
		delete m_pGridMeas;
		m_pGridMeas = NULL;
		return;
	}
	/* ��ü �⺻ ���� */
	m_pGridMeas->SetLogFont(g_lf);
	m_pGridMeas->SetRowCount(i32RowCnt+1);
	m_pGridMeas->SetColumnCount(i32ColCnt);
	m_pGridMeas->SetFixedRowCount(1);
	m_pGridMeas->SetRowHeight(0, 25);
	m_pGridMeas->SetFixedColumnCount(0);
	m_pGridMeas->SetBkColor(RGB(255, 255, 255));
	m_pGridMeas->SetFixedColumnSelection(0);

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
		m_pGridMeas->SetItem(&stGV);
		m_pGridMeas->SetColumnWidth(i, i32Width[i]);
	}

	m_pGridMeas->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGridMeas->UpdateGrid();
}

/*
 desc : �׸��� ��Ʈ�� ����
 parm : None
 retn : None
*/
VOID CDlgMain::CloseGrid()
{
	/* for Motor Position */
	if (m_pGridMotor)
	{
		if (m_pGridMotor->GetSafeHwnd())	m_pGridMotor->DestroyWindow();
		delete m_pGridMotor;
	}
	/* for Motor Position */
	if (m_pGridMeas)
	{
		if (m_pGridMeas->GetSafeHwnd())		m_pGridMeas->DestroyWindow();
		delete m_pGridMeas;
	}
}

/*
 desc : Library Init
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::InitLib()
{
	if (!uvEng_Init(ENG_ERVM::en_cali_vdof))	return FALSE;

	return TRUE;
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
 desc : ��ư Ŭ�� �̺�Ʈ
 parm : id	- [in]  Click�� ��ư ID
 retn : None
*/
VOID CDlgMain::OnBtnClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_MAIN_BTN_EXIT			:	PostMessage(WM_CLOSE, 0, 0L);		break;
	case IDC_MAIN_BTN_MC2_RESET		:	MC2Recovery();						break;
	case IDC_MAIN_BTN_NEXT			:	NextStepMoveY();					break;
	case IDC_MAIN_BTN_ACAM_MOVE		:	ACamMoveAbs();						break;
	case IDC_MAIN_BTN_ACAM_HOMED	:	ACamHoming();						break;
	case IDC_MAIN_BTN_START			:	StartFocus();						break;
	case IDC_MAIN_BTN_STOP			:	StopFocus();						break;
	case IDC_MAIN_BTN_UNLOAD		:	UnloadStageXY();					break;
	case IDC_MAIN_BTN_MOVE			:	MovePosMeasure();					break;
	case IDC_MAIN_BTN_SAVE			:	SaveDataToFile();					break;
	case IDC_MAIN_BTN_MC2_MOVE_UP	:	MotionMove(ENG_MDMD::en_move_up);	break;
	case IDC_MAIN_BTN_MC2_MOVE_DN	:	MotionMove(ENG_MDMD::en_move_down);	break;
	case IDC_MAIN_BTN_MODEL_FIND	:	RunFindModel();						break;
	case IDC_MAIN_BTN_MODEL_RESET	:	ResetFindModel();					break;
	case IDC_MAIN_BTN_TRIG_SET		:	SetTriggerParam();					break;
	case IDC_MAIN_BTN_TRIG_CH		:	SetTriggerCh();						break;
	case IDC_MAIN_BTN_FILE_XLS		:	SaveAsExcelFile();					break;
	case IDC_MAIN_BTN_ACAM			:	SetACamNo();						break;
	}
}

/*
 desc : üũ (for Align Camera) Ŭ�� �̺�Ʈ
 parm : id	- [in]  Click ID
 retn : None
*/
VOID CDlgMain::OnChkClickACam(UINT32 id)
{
	switch (id)
	{
	case IDC_MAIN_CHK_ACAM_LIVE	:	SetLiveView();	break;
	}
}

/*
 desc : Main Thread���� �߻��� �̺�Ʈ ó��
 parm : wparam	- [in]  Event ID
		lparam	- [in]  Event Data
 retn : 0L
*/
LRESULT CDlgMain::OnMsgThread(WPARAM wparam, LPARAM lparam)
{
	switch (wparam)
	{
	case MSG_ID_NORMAL			:	UpdatePeriod();			break;
	case MSG_ID_STEP_RESULT		:	UpdateStepResult();
									UpdateMatchDraw();		break;
	case MSG_ID_STEP_GRAB_FAIL	:	DisplayGrabError(0x01);	break;
	case MSG_ID_STEP_FIND_FAIL	:	DisplayGrabError(0x02);	break;
	}

	/* Ʈ���Ű� �߻��ǰ� 2�� ���� �����ٸ�, �� �˻� ��� ȣ�� */
	if (m_csSyncTrig.Enter())
	{
		if (m_bTriggerOn)
		{
			if (m_u64TrigTime + 2000 < GetTickCount64())
			{
				m_bTriggerOn	= FALSE;
				UpdateGrabResult();
			}
		}
		/* ���� ���� */
		m_csSyncTrig.Leave();
	}

	return 0L;
}

/*
 desc : �ֱ������� ���ŵǴ� �׸�
 parm : None
 retn : None
*/
VOID CDlgMain::UpdatePeriod()
{
	/* for Align Camera */
	UpdateCtrlACam();
	/* for Image */
	UpdateResult();
	/* for Motor Position (mm) */
	UpdateMotorPos();
	/* for PLC Value */
	UpdatePLCVal();
	/* Control : Enable/Disable */
	UpdateEnableCtrl();
}

/*
 desc : Focus ���� ��� ����
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateStepResult()
{
	TCHAR tzValue[128]	= {NULL};
	UINT8 u8PhNo		= 0x00;	/* Zero-based */
	INT32 i32Item		= 1;
	UINT32 u32Count		= 0, u32Format	= DT_VCENTER | DT_SINGLELINE;
	LPG_ZAAL pstData	= NULL;
	CMeasure *pMeasure	= m_pMainThread->GetMeasure();

	/* ���� ��ϵ� ���� ���� ��� */
	pstData	= m_pMainThread->GetLastGrabData();
	u8PhNo	= pstData->ph_no - 1;
	/* ��ϵ� ��ġ ��� */
	i32Item	= u8PhNo * m_u16StepCount + pstData->steps;

	/* Photohead Number */
	wmemset(tzValue, 0x00, 128);
	swprintf_s(tzValue, 128, L"PH_%u", pstData->ph_no);
	m_pGridMeas->SetItemText(i32Item,	0, tzValue);
	m_pGridMeas->SetItemFormat(i32Item,	0, u32Format|DT_CENTER);
	if (0 == (pstData->ph_no % 2))
	{
		m_pGridMeas->SetItemBkColour(i32Item, 0, RGB(255, 225, 240));
	}
	m_pGridMeas->InvalidateRowCol(i32Item, 0);
	/* Measurement Step Number */
	wmemset(tzValue, 0x00, 128);
	swprintf_s(tzValue, 128, L"%03u", pstData->steps);
	m_pGridMeas->SetItemText(i32Item,	1, tzValue);
	m_pGridMeas->SetItemFormat(i32Item,	1, u32Format|DT_CENTER);
	m_pGridMeas->InvalidateRowCol(i32Item, 1);
	/* Score Rate (100 % �� ����� ���� ����) */
	wmemset(tzValue, 0x00, 128);
	swprintf_s(tzValue, 128, L"%.3f", pstData->avg_value[0]);
	m_pGridMeas->SetItemText(i32Item,	2, tzValue);
	m_pGridMeas->SetItemFormat(i32Item,	2, u32Format|DT_RIGHT);
	if (pstData->set_value[0])
	{
		/* ���� �� ��� ���� ��� */
		if (m_i32SetValue[u8PhNo][0] != -1)
		{
			m_pGridMeas->SetItemBkColour(m_i32SetValue[u8PhNo][0], 1, RGB(255, 255, 255));
			m_pGridMeas->SetItemBkColour(m_i32SetValue[u8PhNo][0], 2, RGB(255, 255, 255));
			m_pGridMeas->InvalidateRowCol(m_i32SetValue[u8PhNo][0], 1);
			m_pGridMeas->InvalidateRowCol(m_i32SetValue[u8PhNo][0], 2);
		}
		m_pGridMeas->SetItemBkColour(i32Item, 1, RGB(224, 224, 244));
		m_pGridMeas->SetItemBkColour(i32Item, 2, RGB(224, 224, 244));
		m_i32SetValue[u8PhNo][0]	= i32Item;
	}
	/* Scale Rate (100 % �� ����� ���� ����) */
	wmemset(tzValue, 0x00, 128);
	swprintf_s(tzValue, 128, L"%.3f", pstData->avg_value[1]);
	m_pGridMeas->SetItemText(i32Item,	3, tzValue);
	m_pGridMeas->SetItemFormat(i32Item,	3, u32Format|DT_RIGHT);
	if (pstData->set_value[1])
	{
		/* ���� �� ��� ���� ��� */
		if (m_i32SetValue[u8PhNo][1] != -1)
		{
			m_pGridMeas->SetItemBkColour(m_i32SetValue[u8PhNo][1], 3, RGB(255, 255, 255));
			m_pGridMeas->InvalidateRowCol(m_i32SetValue[u8PhNo][1], 3);
		}
		m_i32SetValue[u8PhNo][1]	= i32Item;
		/* ���ο� ��� ���� ���� */
		m_pGridMeas->SetItemBkColour(i32Item, 3, RGB(224, 224, 244));
	}
	/* Coverage Rate (100 % �� ����� ���� ����) */
	wmemset(tzValue, 0x00, 128);
	swprintf_s(tzValue, 128, L"%.3f", pstData->avg_value[2]);
	m_pGridMeas->SetItemText(i32Item,	4, tzValue);
	m_pGridMeas->SetItemFormat(i32Item,	4, u32Format|DT_RIGHT);
	if (pstData->set_value[2])
	{
		/* ���� �� ��� ���� ��� */
		if (m_i32SetValue[u8PhNo][2] != -1)
		{
			m_pGridMeas->SetItemBkColour(m_i32SetValue[u8PhNo][2], 4, RGB(255, 255, 255));
			m_pGridMeas->InvalidateRowCol(m_i32SetValue[u8PhNo][2], 4);
		}
		m_i32SetValue[u8PhNo][2]	= i32Item;
		/* ���ο� ��� ���� ���� */
		m_pGridMeas->SetItemBkColour(i32Item, 4, RGB(224, 224, 244));
	}
	/* Fit Error Rate (0.00 �� ����� ���� ����) */
	wmemset(tzValue, 0x00, 128);
	swprintf_s(tzValue, 128, L"%.5f", pstData->avg_value[3]);
	m_pGridMeas->SetItemText(i32Item,	5, tzValue);
	m_pGridMeas->SetItemFormat(i32Item,	5, u32Format|DT_RIGHT);
	if (pstData->set_value[3])
	{
		/* ���� �� ��� ���� ��� */
		if (m_i32SetValue[u8PhNo][3] != -1)
		{
			m_pGridMeas->SetItemBkColour(m_i32SetValue[u8PhNo][3], 5, RGB(255, 255, 255));
			m_pGridMeas->InvalidateRowCol(m_i32SetValue[u8PhNo][3], 5);
		}
		m_i32SetValue[u8PhNo][3]	= i32Item;
		/* ���ο� ��� ���� ���� */
		m_pGridMeas->SetItemBkColour(i32Item, 5, RGB(224, 224, 244));
	}
	/* Repeat Count (�ִ� 255) */
	m_pGridMeas->SetItemText(i32Item,	6, (UINT32)pstData->arrValue[0].GetCount());
	m_pGridMeas->SetItemFormat(i32Item,	6, u32Format|DT_CENTER);
	/* Align Camera Z Position (mm) */
#if 0
	m_pGridMeas->SetItemText(i32Item, 7, uvCmn_MCQ_GetACamCurrentPosZ(0x01));
	m_pGridMeas->SetItemFormat(i32Item,	7, u32Format|DT_RIGHT);
	m_pGridMeas->SetItemText(i32Item, 8, uvCmn_MCQ_GetACamCurrentPosZ(0x02));
	m_pGridMeas->SetItemFormat(i32Item,	8, u32Format|DT_RIGHT);
#else
	m_pGridMeas->SetItemText(i32Item, 7, uvCmn_MCQ_GetACamCurrentPosZ(m_u8ACamNo));
	m_pGridMeas->SetItemFormat(i32Item,	7, u32Format|DT_RIGHT);
#endif
	/* ���� �� ���� */
	m_pGridMeas->InvalidateRow(i32Item);
}

/*
 desc : Match�� ���� ��� �̹��� ���
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateMatchDraw()
{
	InvalidateView();
}

/*
 desc : Grabbed Image �˻� ���� ���� �޽���
 parm : flag	- [in]  0x01 : �� �˻� ����, 0x02 : �� �м� ����
 retn : None
*/
VOID CDlgMain::DisplayGrabError(UINT8 flag)
{
	if (0x01 == flag)	AfxMessageBox(L"Failed to find the mark model", MB_ICONSTOP|MB_TOPMOST);
	else 				AfxMessageBox(L"Failed to analyze the mark model", MB_ICONSTOP|MB_TOPMOST);
}

/*
 desc : Align Camera ���� Control ����
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateCtrlACam()
{
	INT32 i32Tab	= 0;
	BOOL bEnable	=FALSE;

	/* ����� ī�޶� ����Ǿ� ���� ������ */
	if (!uvCmn_Camera_IsConnected())	bEnable	= FALSE;
	/* ����� ī�޶� ����Ǿ� �ִٸ� */
	else
	{
		/* ���� Live View ȭ���� �ƴϸ� */
		if (i32Tab != 0)	bEnable	= FALSE;
		/* ���� Live View ȭ���̸�, ���� ���� ������� ���ο� ���� */
		else				bEnable	= !m_pMainThread->IsRunFocus();
	}

	/* Live Check ��ư Ȱ��ȭ �� üũ ���� ó�� */
	m_chk_cam[0].EnableWindow(bEnable);
	if (!bEnable)
	{
		if (m_chk_cam[0].GetCheck())	m_chk_cam[0].SetCheck(0);
	}
}

/*
 desc : Result (Live View / Edge Histogram / Edge Object) ����
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateResult()
{
	TCHAR tzVal[64]	= {NULL};
	INT32 i32Tab	= 0;

	/* ���� ���� ������ ���� */
	if (!m_pMainThread->IsRunFocus())
	{
		if (m_chk_cam[0].GetCheck())	UpdateLiveView();
	}
	else
	{
		/* ���� Grabbed Image ��� */
		InvalidateView();
		/* ������� ����� ��� */
		swprintf_s(tzVal, 64, L"%.2f %%", m_pMainThread->GetWorkStepRate());
		m_edt_pgr[0].SetTextToStr(tzVal, TRUE);
	}
}

/*
 desc : ���� ��� ���� Motor Position (mm) �� ����
 parm : None
 retn : None
 note : Photohead Z Axis, Motion Drive, Align Camera Z Axis ���
*/
VOID CDlgMain::UpdateMotorPos()
{
	UINT8 i	= 0x00, j = 1;
	DOUBLE dbPos[2]	= {NULL};
	LPG_CIEA pstCfg	= uvEng_GetConfig();
	LPG_LDDP pstDP	= &uvEng_ShMem_GetLuria()->directph;
	LPG_MDSM pstMC2	= uvEng_ShMem_GetMC2();

	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* Luria Photohead ��ġ */
		for (i=0; i<pstCfg->luria_svc.ph_count; i++, j++)
		{
			dbPos[0] = m_pGridMotor->GetItemTextToFloat(j, 1);
			dbPos[1] = (DOUBLE)ROUNDED(pstDP->focus_motor_move_abs_position[i]/1000.0f, 3);
			if (dbPos[0] != dbPos[1])
			{
				m_pGridMotor->SetItemText(j, 1, dbPos[1]);
				m_pGridMotor->InvalidateRowCol(j, 1);
			}
		}
	}
	/* MC2 SD2S Drive ��ġ (Basler Camera ����) */
	for (i=0; i<pstCfg->mc2_svc.drive_count; i++, j++)
	{
		dbPos[0] = m_pGridMotor->GetItemTextToFloat(j, 1);
		dbPos[1] = (DOUBLE)ROUNDED(pstMC2->act_data[pstCfg->mc2_svc.axis_id[i]].real_position/10000.0f, 4);
		if (dbPos[0] != dbPos[1])
		{
			m_pGridMotor->SetItemText(j, 1, dbPos[1]);
			m_pGridMotor->InvalidateRowCol(j, 1);
		}
	}
}

/*
 desc : ���� ��� ���� PLC �� ����
 parm : None
 retn : None
*/
VOID CDlgMain::UpdatePLCVal()
{
	DOUBLE dbPosZ	= 0.0f;
	LPG_CIEA pstCfg	= uvEng_GetConfig();

	/* Align Camera (for Basler) - Z Axis Move Step & Speed */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)pstCfg->set_comn.align_camera_kind)
	{
		/* ���� Align Camera�� Z Axis �̵� �ӵ� �� ���� ��ġ (Position) �� ��� */
		dbPosZ	= uvCmn_MCQ_GetACamCurrentPosZ(0x01);
		if (m_dbACamZAxisPos[0] != dbPosZ)
		{
			TCHAR tzVal[32]	= {NULL};
			swprintf_s(tzVal, 32, L"%.4f", dbPosZ);
			m_pGridMotor->SetItemText(m_u32ACamZAxisRow[0], 1, tzVal);	/* Position */
			m_pGridMotor->InvalidateRowCol(m_u32ACamZAxisRow[0], 1);
			m_dbACamZAxisPos[0] = dbPosZ;
		}
		dbPosZ	= uvCmn_MCQ_GetACamCurrentPosZ(0x02);
		if (m_dbACamZAxisPos[1] != dbPosZ)
		{
			TCHAR tzVal[32]	= {NULL};
			swprintf_s(tzVal, 32, L"%.4f", dbPosZ);
			m_pGridMotor->SetItemText(m_u32ACamZAxisRow[1], 1, tzVal);	/* Position */
			m_pGridMotor->InvalidateRowCol(m_u32ACamZAxisRow[1], 1);
			m_dbACamZAxisPos[1] = dbPosZ;
		}
	}
	/* Align Camera (for IDS) - Only Z Axis Step */
	else
	{
	}
}

/*
 desc : ���� ��Ʈ�� Enable or Disable
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateEnableCtrl()
{
	UINT8 i	= 0x00;
	BOOL bBusyMotion	= FALSE, bBusyRunFocus = FALSE, bValidFocusPos = FALSE;
	LPG_CMSI pstMC2Svc	= &uvEng_GetConfig()->mc2_svc;

	/* Focus ���� ���� ���� */
	bBusyRunFocus	= m_pMainThread->IsRunFocus();
	/* Motion Busy ���� */
	bBusyMotion		= uvCmn_MC2_IsAnyDriveBusy();
	/* MC2 Drive Check Button */
	for (; i<uvEng_GetConfig()->mc2_svc.drive_count; i++)
	{
		m_chk_mc2[pstMC2Svc->axis_id[i]].EnableWindow(!(bBusyMotion || bBusyRunFocus));
	}
	/* Align Camera Check Button */
	m_chk_cam[0].EnableWindow(!(bBusyMotion || bBusyRunFocus));

	/* ���� ����� ī�޶��� Z �� ��ġ�� ȯ�� ���Ͽ� ����� Focus Z ��ġ�� �������� ���� */
	bValidFocusPos	= uvCmn_MCQ_GetACamCurrentPosZ(m_u8ACamNo) != uvEng_GetConfig()->acam_focus.acam_z_focus[m_u8ACamNo-1];

	/* Normal Button */
	m_btn_ctl[0].EnableWindow(!bBusyRunFocus);										/* MC2 Move Up */
	m_btn_ctl[1].EnableWindow(!bBusyRunFocus);										/* MC2 Move Down */
	m_btn_ctl[2].EnableWindow(!bBusyRunFocus);										/* ACam Move */
	m_btn_ctl[3].EnableWindow(!bBusyRunFocus);										/* Reset */
	m_btn_ctl[4].EnableWindow(!(bBusyMotion || bBusyRunFocus));						/* Move */
#ifndef _DEBUG
	m_btn_ctl[5].EnableWindow(!(bBusyMotion || bBusyRunFocus || bValidFocusPos));	/* Start */
	m_btn_ctl[6].EnableWindow(bBusyRunFocus);										/* Stop */
#endif
	m_btn_ctl[7].EnableWindow(bBusyRunFocus);										/* Next */
	m_btn_ctl[8].EnableWindow(!(bBusyMotion || bBusyRunFocus));						/* Save */
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
		if (!m_chk_mc2[i].GetCheck())	continue;
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
	/* Align Camera Z Axis �̵� */
	uvEng_MCQ_SetACamMovePosZ(m_u8ACamNo, 0x00, m_edt_cam[0].GetTextToDouble());
}

/*
 desc : Align Camera (Motor Drive) Homing
 parm : None
 retn : None
*/
VOID CDlgMain::ACamHoming()
{
	/* ���ο� ��ġ�� �̵� */
	uvEng_MCQ_SetACamHomingZAxis(m_u8ACamNo);
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
		bCheck	= m_chk_mc2[pstCfg->axis_id[i]].GetCheck() > 0;
	}
	if (!bCheck)
	{
		AfxMessageBox(L"The drive to be moved is not checked", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* �̵��ϰ��� �ϴ� �ӵ��� �Ÿ� ���� �ԷµǾ� �ִ��� Ȯ�� (���⼭�� Step �̵��� �ش� ��) */
	dbDist	= m_edt_mc2[0].GetTextToDouble();
	dbVelo	= m_edt_mc2[1].GetTextToDouble();
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
		if (!m_chk_mc2[u8DrvNo].GetCheck())	continue;

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
 desc : ���� ��ġ�� �̵�
 parm : None
 retn : None
*/
VOID CDlgMain::MovePosMeasure()
{
	UINT8 i			= 0x00;
	DOUBLE dbPos	= 0.0f, dbVelo = m_edt_mc2[1].GetTextToDouble();	/* mm/sec */
	LPG_CIEA pstCfg	= (LPG_CIEA)uvEng_GetConfig();
	if (!pstCfg)	return;

	/* �̵� �ӵ��� ������ ���� �ʰų� �ʹ� �����ٸ� */
	if (dbVelo < 10.0f)	return;	/* 10 mm/sec �̵� �ӵ� ������ ��� */

	/* ��� X �� �̵� */
	dbPos	= pstCfg->ph_focus.mark2_stage[0];
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, dbPos, dbVelo);

	/* ��� Y �� �̵� */
	dbPos	= pstCfg->ph_focus.mark2_stage[1];
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dbPos, dbVelo);

	/* ��� Camera �� �̵� */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)pstCfg->set_comn.align_camera_kind)
	{
		if (0x01 == m_u8ACamNo)
		{
			/* 2 ��° ī�޶� �� ���������� �̵� ��Ŵ */
			dbPos	= pstCfg->mc2_svc.max_dist[(UINT8)ENG_MMDI::en_align_cam2];
			uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2, dbPos, dbVelo);
			/* 1 ��° ī�޶� ���� ��ġ�� �̵� ��Ŵ */
			dbPos	= pstCfg->ph_focus.mark2_acam_x[0];
			uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, dbPos, dbVelo);
		}
		else
		{
			/* 1 ��° ī�޶� �� �������� �̵� ��Ŵ */
			dbPos	= pstCfg->mc2_svc.min_dist[(UINT8)ENG_MMDI::en_align_cam1];
			uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, dbPos, dbVelo);
			/* 2 ��° ī�޶� ���� ��ġ�� �̵� ��Ŵ */
			dbPos	= pstCfg->ph_focus.mark2_acam_x[1];
			uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2, dbPos, dbVelo);
		}
		/* ����� ī�޶� Z �� �̵� */
		uvEng_MCQ_SetACamMovePosZ(1/* fixed */, 0x00, pstCfg->acam_focus.acam_z_focus[0]);
	}
#if 0
	else
	{
		/* ���а� Z �� �̵� */
		uvEng_Luria_ReqSetMotorAbsPosition(pstCfg->set_ids.ph_no,
										   pstCfg->acam_focus.acam_z_focus[0]);
	}
#endif
}

/*
 desc : ��Ŀ�� ���� ����
 parm : None
 retn : None
*/
VOID CDlgMain::StopFocus()
{
	if (m_pMainThread->IsRunFocus())	m_pMainThread->StopFocus();
}

/*
 desc : �������� Unload ��ġ�� �̵�
 parm : None
 retn : None
*/
VOID CDlgMain::UnloadStageXY()
{
	DOUBLE dbMoveX	= uvEng_GetConfig()->set_align.table_unloader_xy[0][0];
	DOUBLE dbMoveY	= uvEng_GetConfig()->set_align.table_unloader_xy[0][1];
	DOUBLE dbVelo	= uvEng_GetConfig()->mc2_svc.move_velo;

	if (m_pMainThread->IsRunFocus())
	{
		AfxMessageBox(L"It is currently being measured\nPlease stop measuring first", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* ��� (��������) �̵� */
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, dbMoveX, dbVelo);
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dbMoveY, dbVelo);
}

/*
 desc : ���� ���� ��ġ�� �̵� (Y �� �������θ� �̵�)
 parm : None
 retn : None
*/
VOID CDlgMain::NextStepMoveY()
{
}

/*
 desc : Ʈ���� ä�� ��ȣ ����
 parm : None
 retn : None
*/
VOID CDlgMain::LoadTriggerCh()
{
	UINT8 u8ChNo		= 0x01;
	TCHAR tzChNo[32]	= {NULL};
	LPG_CTSP pstTrig	= &uvEng_GetConfig()->trig_step;

	u8ChNo	= uvEng_GetConfig()->set_comn.strobe_lamp_type == 0x00 ? 0x01 : 0x02;	/* 0x00: Coaxial : 0x01 : Ring */

	swprintf_s(tzChNo, 32, L"CH\n[ %u ]", u8ChNo);
	m_btn_ctl[15].SetWindowTextW(tzChNo);

	/* Trigger Time �� ��-���� */
	m_edt_trg[0].SetTextToNum(pstTrig->trig_on_time[u8ChNo-1]);
	m_edt_trg[1].SetTextToNum(pstTrig->strob_on_time[u8ChNo-1]);
	m_edt_trg[2].SetTextToNum(pstTrig->trig_delay_time[u8ChNo-1]);

	/* Trigger ���� �Ӽ� �� ���� (Trigger OnTime / Strobe OnTime / Trigger Delay Time */
	if (!uvEng_Trig_ReqInitUpdate())
	{
		AfxMessageBox(L"Failed to set the trigger attribute for LED Lamp", MB_ICONSTOP|MB_TOPMOST);
	}
}

/*
 desc : Ʈ���� ä�� ��ȣ ����
 parm : None
 retn : None
*/
VOID CDlgMain::SetTriggerCh()
{
	UINT8 u8ChNo	= uvEng_GetConfig()->set_comn.strobe_lamp_type;
	if (!u8ChNo)	uvEng_GetConfig()->set_comn.strobe_lamp_type = 0x01;	/* Ring */
	else			uvEng_GetConfig()->set_comn.strobe_lamp_type = 0x00;	/* Coaxial */

	LoadTriggerCh();
}

/*
 desc : Align Camera ��ȣ ����
 parm : None
 retn : None
*/
VOID CDlgMain::SetACamNo()
{
	TCHAR tzNo[32]	= {NULL};

	m_u8ACamNo	= m_u8ACamNo == 0x01 ? 0x02 : 0x01;
	swprintf_s(tzNo, 32, L"ACam\n[ %u ]", m_u8ACamNo);
	m_btn_ctl[16].SetWindowTextW(tzNo);
}

/*
 desc : �� �˻� ����
 parm : None
 retn : None
*/
VOID CDlgMain::SetTriggerParam()
{
	LPG_CTSP pstTrig	= &uvEng_GetConfig()->trig_step;
	/* �� �޸𸮿� ���� */
	pstTrig->SetData(m_u8ACamNo,
					 (UINT32)m_edt_trg[0].GetTextToNum(),
					 (UINT32)m_edt_trg[1].GetTextToNum(),
					 (UINT32)m_edt_trg[2].GetTextToNum());
	/* Trigger �ʿ� ��� ���� */
	if (!uvEng_Trig_ReqInitUpdate())
	{
		AfxMessageBox(L"Failed to send the command to trigger", MB_ICONSTOP|MB_TOPMOST);
	}
}

/*
 desc : �˻� ����� �� ���� �Ķ���� ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::SetModelParam()
{
	UINT32 u32Model	= (UINT32)ENG_MMDT::en_circle;
	DOUBLE dbMSize	= 0.0f /* um */, dbMColor = 256 /* 256:Black, 128:White */;
	LPG_CIEA pstCfg	= uvEng_GetConfig();

	/* �˻� ��� ���� �� ��� */
	dbMSize	= pstCfg->ph_focus.model_dia_size * 1000.0f;
	dbMColor= pstCfg->ph_focus.model_color;
	if (!uvEng_Camera_SetModelDefineEx(m_u8ACamNo,
									   pstCfg->mark_find.detail_level,
									   pstCfg->mark_find.model_speed,
									   pstCfg->mark_find.model_smooth,
									   &u32Model, &dbMColor, &dbMSize,
									   NULL, NULL, NULL, 1))
	{
		AfxMessageBox(L"Failed to setup the model for search target", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	return TRUE;
}


/*
 desc : �� �˻� ����
 parm : None
 retn : None
*/
VOID CDlgMain::RunFindModel()
{
	UINT8 u8Ch	= m_u8ACamNo==0x01 ? 0x01 : 0x02;
	/* Camera ���� ��� ���� */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);
	/* �˻� ��� �� ��� */
	if (!SetModelParam())	return;

	/* ���� �˻��� Grabbed Data & Image ���� */
	uvEng_Camera_ResetGrabbedImage();

	/* Trigger ������ (��������) �߻� */
	if (!uvEng_Trig_ReqTrigOutOne(u8Ch, TRUE))
	{
		AfxMessageBox(L"Failed to send the event for trigger", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* Ʈ���� �߻��ߴٰ� �÷��� ���� */
	if (m_csSyncTrig.Enter())
	{
		m_u64TrigTime	= GetTickCount64();
		m_bTriggerOn	= TRUE;

		m_csSyncTrig.Leave();
	}
}

/*
 desc : �� �˻� ���� ��� ���
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateGrabResult()
{
	TCHAR tzValue[128]	= {NULL};
	INT32 i32Item		= 0;
	UINT32 u32Count		= 0, u32Format	= DT_VCENTER | DT_SINGLELINE;;
	LPG_ACGR pstResult	= NULL;

	/* �˻��� ����� �ִ��� ���� */
	pstResult	= uvEng_Camera_RunModelCali(m_u8ACamNo, 0xff);
	if (!pstResult)
	{
		AfxMessageBox(L"There is no analysis result", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* �˻��� �̹��� �м� ��� Ȯ�� */
	if (pstResult->marked != 0x01)
	{
		AfxMessageBox(L"Failed to analyze the grabbed image", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* Camera ���� ��� ���� */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_none);

	/* �׸��� ��Ʈ�� ��Ȱ��ȭ */
	m_pGridMeas->SetRedraw(FALSE);
	/* ���� ��ϵ� ���� ��� */
	i32Item	= m_pGridMeas->GetRowCount();
	m_pGridMeas->InsertRow(i32Item);
	/* Photohead Number */
	m_pGridMeas->SetItemText(i32Item, 0, L"PH_XX");
	m_pGridMeas->SetItemFormat(i32Item, 0, u32Format|DT_CENTER);
	/* Measurement Step Number */
	m_pGridMeas->SetItemText(i32Item, 1, L"XXX");
	m_pGridMeas->SetItemFormat(i32Item, 1, u32Format|DT_CENTER);
	/* Score Rate (100 % �� ����� ���� ����) */
	wmemset(tzValue, 0x00, 128);
	swprintf_s(tzValue, 128, L"%.4f", pstResult->score_rate);
	m_pGridMeas->SetItemText(i32Item, 2, tzValue);
	m_pGridMeas->SetItemFormat(i32Item, 2, u32Format|DT_RIGHT);
	m_pGridMeas->SetItemBkColour(i32Item, 2, RGB(224, 224, 244));
	/* Scale Rate (100 % �� ����� ���� ����) */
	wmemset(tzValue, 0x00, 128);
	swprintf_s(tzValue, 128, L"%.4f", pstResult->scale_rate);
	m_pGridMeas->SetItemText(i32Item, 3, tzValue);
	m_pGridMeas->SetItemFormat(i32Item, 3, u32Format|DT_RIGHT);
	m_pGridMeas->SetItemBkColour(i32Item, 3, RGB(224, 224, 244));
	/* Coverage Rate (100 % �� ����� ���� ����) */
	wmemset(tzValue, 0x00, 128);
	swprintf_s(tzValue, 128, L"%.4f", pstResult->coverage_rate);
	m_pGridMeas->SetItemText(i32Item, 4, tzValue);
	m_pGridMeas->SetItemFormat(i32Item, 4, u32Format|DT_RIGHT);
	m_pGridMeas->SetItemBkColour(i32Item, 4, RGB(224, 224, 244));
	/* Fit Error Rate (0.000 % �� ����� ���� ����) */
	wmemset(tzValue, 0x00, 128);
	swprintf_s(tzValue, 128, L"%.5f", pstResult->fit_error);
	m_pGridMeas->SetItemText(i32Item, 5, tzValue);
	m_pGridMeas->SetItemFormat(i32Item, 5, u32Format|DT_RIGHT);
	m_pGridMeas->SetItemBkColour(i32Item, 5, RGB(224, 224, 244));
	/* Repeat Count (�ִ� 255) */
	m_pGridMeas->SetItemText(i32Item, 6, L"XXX");
	m_pGridMeas->SetItemFormat(i32Item, 6, u32Format|DT_CENTER);
	/* Align Camera Z Position (mm) */
#if 0
	m_pGridMeas->SetItemText(i32Item, 7, uvCmn_MCQ_GetACamCurrentPosZ(0x01));
	m_pGridMeas->SetItemFormat(i32Item, 7, u32Format|DT_RIGHT);
	m_pGridMeas->SetItemText(i32Item, 8, uvCmn_MCQ_GetACamCurrentPosZ(0x02));
	m_pGridMeas->SetItemFormat(i32Item, 8, u32Format|DT_RIGHT);
#else
	m_pGridMeas->SetItemText(i32Item, 7, uvCmn_MCQ_GetACamCurrentPosZ(m_u8ACamNo));
	m_pGridMeas->SetItemFormat(i32Item, 7, u32Format|DT_RIGHT);
#endif
	/* �׸��� ��Ʈ�� Ȱ��ȭ */
	m_pGridMeas->SetRedraw(TRUE);
	m_pGridMeas->Invalidate(FALSE);

	/* ���� Grabbed Image ��� ��� */
	UpdateMatchDraw();
}

/*
 desc : �� �˻� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgMain::ResetFindModel()
{
	INT32 i, j, i32Rows	= (INT32)m_edt_opt[4].GetTextToNum() * uvEng_GetConfig()->luria_svc.ph_count + 1;
	/* Grid Control ��� ���� */
	m_pGridMeas->DeleteNonFixedRows();
#if 0
	/* �׸��� ���� ���� ���� */
	m_pGridMeas->SetRowCount(i32Rows);
#endif
	/* ���� �˻��� Edge File ���� */
	uvEng_RemoveAllFiles(0x00);
	/* ���� �˻��� Grabbed Data & Image ���� */
	uvEng_Camera_ResetGrabbedImage();
	uvEng_Camera_SetCamMode(ENG_VCCM::en_none);

	/* ������ ��� ���� ����� �׸��� ��� ���� ��ġ �ʱ�ȭ */
	for (i=0; i<MAX_PH; i++)
	{
		for (j=0; j<COLLECT_DATA_COUNT; j++)	m_i32SetValue[i][j]	= -1;
	}
}

/*
 desc : ��Ŀ�� ���� ����
 parm : None
 retn : None
*/
VOID CDlgMain::StartFocus()
{
	BOOL bValid		= TRUE;
	UINT32 u32Model	= (UINT32)ENG_MMDT::en_circle;
	LPG_CIEA pstCfg	= uvEng_GetConfig();

	/* ���� ��Ŀ�� ���� ��� ���� �� ���� �ʱ�ȭ */
	ResetFindModel();

	/* --------------------------------------- */
	/* ���� �Էµ� �Ķ���� ���� ��ȿ���� �Ǵ� */
	/* --------------------------------------- */

	/* Trigger ���� �Ӽ� �� ���� (Trigger OnTime / Strobe OnTime / Trigger Delay Time */
	UpdateTrigParam();
	if (!uvEng_Trig_ReqInitUpdate())
	{
		AfxMessageBox(L"Failed to set the trigger attribute for LED Lamp", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* ��ü ���� ���� �� �ӽ� ���� */
	m_u16StepCount	= (UINT16)m_edt_opt[4].GetTextToNum();
	/* ���������� ���� �Է� �Ǿ� �ִ��� Ȯ�� */
	if (m_edt_opt[3].GetTextToNum() < 1)
	{
		AfxMessageBox(L"Enter the measurement repeat value", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	if (m_u16StepCount < 1)
	{
		AfxMessageBox(L"Enter the measurement step value", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	if (m_edt_opt[0].GetTextToDouble() < 1.0f)
	{
		AfxMessageBox(L"Enter the value for the step of moving the section", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* �� ��� */
	if (!SetModelParam())	return;

	/* Auto Focus ������ ���� Step Y Move Size �� (����: mm) */
	pstCfg->ph_focus.step_move_y	= m_edt_opt[0].GetTextToDouble();
	/* Focus�� �����Ǵ� ���� �β� */
	pstCfg->set_align.dof_film_thick= m_edt_opt[1].GetTextToDouble();
	/* PH ���� ������ �Ÿ� ��, Mark ���� ���� ���� ���� (Distance. mm) */
	pstCfg->ph_focus.mark_period	= m_edt_opt[2].GetTextToDouble();
	/* �� �������� �����Ǵ� �ݺ� Ƚ�� */
	pstCfg->ph_focus.repeat_count	= (UINT16)m_edt_opt[3].GetTextToNum();

	/* �����忡 �� ���� �� ���� ��Ŵ*/
	m_pMainThread->StartFocus((UINT16)m_edt_opt[4].GetTextToNum(),
							  (UINT16)m_edt_opt[3].GetTextToNum(),
							  m_edt_opt[0].GetTextToDouble());
}

/*
 desc : Live View ��� ���� ����
 parm : None
 retn : None
*/
VOID CDlgMain::SetLiveView()
{
	BOOL bSucc		= FALSE;
	UINT32 u32OnOff	= 0x00000000;

	/* Live View ��� ���� Ȯ�� */
	if (m_chk_cam[0].GetCheck())	/* Live View */
	{
		/* Trigger Parameter ���� */
		UpdateTrigParam();
		/* Align Camera is Live Mode */
		uvEng_Camera_SetCamMode(ENG_VCCM::en_live_mode);
		/* Trigger & Strobe : Enabled */
		bSucc = uvEng_Trig_ReqTriggerStrobe(TRUE);
		if (bSucc)
		{
			bSucc = uvEng_Trig_ReqEncoderLive(m_u8ACamNo);
		}
	}
	else
	{
		if (uvEng_Camera_GetCamMode() != ENG_VCCM::en_live_mode)	return;
		/* Trigger Disabled */
		bSucc = uvEng_Trig_ReqEncoderOutReset();
		if (bSucc)
		{
			/* Trigger & Strobe : Disabled */
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
					if (GetTickCount64() > (u64Tick + 2500))	break;
				}
				Sleep(10);

			}	while (1);

			/* Align Camera is Calibration Mode */
			uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
		}
	}
}

/*
 desc : �⺻������ ȯ�� ���Ͽ� ������ ���� ����
 parm : None
 retn : None
*/
VOID CDlgMain::LoadDataConfig()
{
	LPG_CIEA pstCfg	= uvEng_GetConfig();

	/* ���� MC2�� ������ �̵� �Ÿ� �� �ӵ� �� ��� */
	m_edt_mc2[0].SetTextToNum(pstCfg->mc2_svc.move_dist, 4);
	m_edt_mc2[1].SetTextToNum(pstCfg->mc2_svc.move_velo, 4);

	/* Auto Focus ������ ���� Step Y Move Size �ʱ� �� (����: mm) */
	m_edt_opt[0].SetTextToNum(pstCfg->ph_focus.step_move_y, 4);
	/* Focus�� �����Ǵ� ���� �β� */
	m_edt_opt[1].SetTextToNum(pstCfg->set_align.dof_film_thick, 4);
	/* PH ���� ������ �Ÿ� ��, Mark ���� ���� ���� ���� (Distance. mm) */
	m_edt_opt[2].SetTextToNum(pstCfg->ph_focus.mark_period, 4);
	/* �� �������� �����Ǵ� �ݺ� Ƚ�� */
	m_edt_opt[3].SetTextToNum(pstCfg->ph_focus.repeat_count);
	/* Y �� Step �̵� Ƚ�� */
	m_edt_opt[4].SetTextToNum(pstCfg->ph_focus.step_y_count);
}

/*
 desc : ���� ������ ���� ȯ�� ���Ͽ� �����
 parm : None
 retn : None
*/
VOID CDlgMain::SaveDataToFile()
{
	LPG_CIEA pstCfg	= uvEng_GetConfig();

	/* ���� MC2�� ������ �̵� �Ÿ� �� �ӵ� �� ���� */
	pstCfg->mc2_svc.move_dist			= m_edt_mc2[0].GetTextToDouble();
	pstCfg->mc2_svc.move_velo			= m_edt_mc2[1].GetTextToDouble();

	/* Auto Focus ������ ���� Step Y Move Size �ʱ� �� (����: mm) */
	pstCfg->ph_focus.step_move_y		= m_edt_opt[0].GetTextToDouble();
#if 0	/* �� Tool������ �������� �ʴ´�. (Align Camera Focus���� ������ ���� �������Ƿ� ...) */
	/* Focus�� �����Ǵ� ���� �β� */
	pstCfg->set_align.dof_film_thick	= m_edt_opt[1].GetTextToDouble();
#endif
	/* PH ���� ������ �Ÿ� (Mark ���� ������ �� �Ÿ�) */
	pstCfg->ph_focus.mark_period		= m_edt_opt[2].GetTextToDouble();
	/* �� �����Ǵ� �������� �ݺ� ���� ���� */
	pstCfg->ph_focus.repeat_count		= (UINT16)m_edt_opt[3].GetTextToNum();
#if 0
	/* Trigger Time */
	pstCfg->trig_set.trig_on_time[0]	= (UINT32)m_edt_trg[0].GetTextToNum();
	pstCfg->trig_set.strob_on_time[0]	= (UINT32)m_edt_trg[1].GetTextToNum();
	pstCfg->trig_set.trig_delay_time[0]	= (UINT32)m_edt_trg[2].GetTextToNum();
#endif
	/* ȯ�� ���Ͽ� ����� ��ũ ó�� ���� ��ġ�� ���� ����� ��ġ�� �ٸ��� �������� ���� ���� */
	if (pstCfg->ph_focus.mark2_stage[0] != uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x) ||
		pstCfg->ph_focus.mark2_stage[1] != uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y) ||
		pstCfg->ph_focus.mark2_acam_x[0] != uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam1) ||
		pstCfg->ph_focus.mark2_acam_x[1] != uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam2))
	{
		if (IDYES == AfxMessageBox(L"Mark Inspection start position has been changed\n"
								   "Shall we change it to a new position value?", MB_YESNO))
		{
			pstCfg->ph_focus.mark2_stage[0]	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);
			pstCfg->ph_focus.mark2_stage[1]	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);
			if (0x01 == m_u8ACamNo)
			{
				pstCfg->ph_focus.mark2_acam_x[0]	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam1);
			}
			else
			{
				pstCfg->ph_focus.mark2_acam_x[1]	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam2);
			}
		}
	}

	/* ȯ�� ���� ���� ���Ϸ� ���� */
	uvEng_SaveConfig();
#ifndef _DEBUG
	AfxMessageBox(L"Save completed successfully", MB_OK);
#endif
}

/*
 desc : ���� �ڵ� Focusing �۾� �������� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::IsRunFocus()
{
	return m_pMainThread->IsRunFocus();
}

/*
 desc : Trigger Parameter ����
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateTrigParam()
{
	LPG_CIEA pstCfg	= uvEng_GetConfig();

	/* Trigger ���� �Ӽ� �� ���� (Trigger OnTime / Strobe OnTime / Trigger Delay Time */
	pstCfg->trig_step.trig_on_time[0]	= (UINT32)m_edt_trg[0].GetTextToNum();
	pstCfg->trig_step.strob_on_time[0]	= (UINT32)m_edt_trg[1].GetTextToNum();
	pstCfg->trig_step.trig_delay_time[0]	= (UINT32)m_edt_trg[2].GetTextToNum();
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
	if (m_pGridMeas->GetRowCount() < 2)	return;

	/* ���� �ð� */
	GetLocalTime(&stTm);
	/* ���� �ӽ� ���Ϸ� ���� �� ���� */
	swprintf_s(tzTempFile, MAX_PATH_LEN, L"%s\\vdof\\pdof_%02u%02u%02u_%02u%02u%02u.csv",
			   g_tzWorkDir, stTm.wYear-2000, stTm.wMonth, stTm.wDay,
			   stTm.wHour, stTm.wMinute, stTm.wSecond);

	/* ���Ϸ� ���� */
	m_pGridMeas->Save(tzTempFile);

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
	CRect rView;
	m_pic_ctl[0].GetWindowRect(rView);
	ScreenToClient(rView);
	InvalidateRect(rView, TRUE);
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
		uvEng_Camera_DrawLiveBitmap(hDC, rDraw, m_u8ACamNo);
		::ReleaseDC(m_pic_ctl[0].m_hWnd, hDC);
	}
}