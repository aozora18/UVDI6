
/*
 desc : Main Dialog
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgMain.h"
#include "MainThread.h"

#include "./Menu/DlgMenu.h"
#include "./Menu/DlgExpo.h"
#include "./Menu/DlgEdge.h"
#include "./Menu/DlgFind.h"
#include "./Menu/DlgHist.h"
#include "./Menu/DlgLive.h"

#include "./Meas/Measure.h"


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
	m_i32ACamZAxisPos	= 0;
	m_pDlgLast			= NULL;
	m_pDlgExpo			= NULL;
	m_pDlgEdge			= NULL;
	m_pDlgFind			= NULL;
	m_pDlgHist			= NULL;
	m_pDlgLive			= NULL;
	m_pGridPos			= NULL;

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

	/* Normal - Group box */
	u32StartID	= IDC_MAIN_GRP_MC2;
	for (i=0; i<6; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Normal - Text */
	u32StartID	= IDC_MAIN_TXT_MC2_MOVE;
	for (i=0; i<19; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Normal - Button */
	u32StartID	= IDC_MAIN_BTN_MC2_MOVE_UP;
	for (i=0; i<11; i++)	DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	u32StartID	= IDC_MAIN_TAB_RESULT;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_tab_ctl[i]);

	/* for MC2 */
	u32StartID	= IDC_MAIN_CHK_MC2_DRV_0;
	for (i=0; i<8; i++)		DDX_Control(dx, u32StartID+i,	m_chk_mc2[i]);
	u32StartID	= IDC_MAIN_EDT_MC2_DIST;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_edt_mc2[i]);

	/* for Align Camera */
	u32StartID	= IDC_MAIN_CHK_ACAM_NO_1;
	for (i=0; i<5; i++)		DDX_Control(dx, u32StartID+i,	m_chk_cam[i]);
	u32StartID	= IDC_MAIN_EDT_ACAM_ABS;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_edt_cam[i]);

	/* for Focusing Option */
	u32StartID	= IDC_MAIN_CHK_OPTION_UP;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_chk_opt[i]);
	u32StartID	= IDC_MAIN_EDT_OPTION_SKIP;
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
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_CHK_ACAM_NO_1,	IDC_MAIN_CHK_ACAM_LIVE,	OnChkClickACam)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_CHK_OPTION_UP,	IDC_MAIN_CHK_OPTION_DN,	OnChkClickOption)
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
	InitTab();
	InitSetup();
	InitGrid();

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

	/* �� �ڽ� ���� ���� */
	CloseTab();
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
	for (i=0; i<6; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* Normal - Text */
	for (i=0; i<19; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(TRUE);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* Normal - Button */
	for (i=0; i<11; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
	}
	/* Normal - Tab Control */
	for (i=0; i<1; i++)
	{
		m_tab_ctl[i].SetFontInfo(&g_lf);
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
	for (i=0; i<5; i++)
	{
		m_chk_cam[i].SetLogFont(g_lf);
		m_chk_cam[i].SetBgColor(RGB(0xff, 0xff, 0xff));
		m_chk_cam[i].SetReadOnly(FALSE);
		m_chk_cam[i].EnableWindow(FALSE);
	}
	/* ������ 1�� ī�޶�� Check �ؾ� �� */
	m_chk_cam[0].SetCheck(1);

	for (i=0; i<1; i++)
	{
		m_edt_cam[i].SetEditFont(&g_lf);
		m_edt_cam[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_cam[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_cam[i].SetInputType(ENM_DITM::en_float);
	}

	/* for Focusing Option */
	for (i=0; i<2; i++)
	{
		m_chk_opt[i].SetLogFont(g_lf);
		m_chk_opt[i].SetBgColor(RGB(0xff, 0xff, 0xff));
		m_chk_opt[i].SetReadOnly(FALSE);
	}
	/* ������ Down�� Check �ؾ� �� */
	m_chk_opt[1].SetCheck(1);

	for (i=0; i<5; i++)
	{
		m_edt_opt[i].SetEditFont(&g_lf);
		m_edt_opt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_opt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		if (i != 3)	m_edt_opt[i].SetInputType(ENM_DITM::en_hex10);
		else		m_edt_opt[i].SetInputType(ENM_DITM::en_float);
	}

	/* for Trigger Time */
	for (i=0; i<3; i++)
	{
		m_edt_trg[i].SetEditFont(&g_lf);
		m_edt_trg[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_trg[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_trg[i].SetInputType(ENM_DITM::en_hex10);
	}

	/* for Work status */
	for (i=0; i<1; i++)
	{
		m_edt_pgr[i].SetEditFont(&g_lf);
		m_edt_pgr[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_pgr[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_pgr[i].SetInputType(ENM_DITM::en_float);
		m_edt_pgr[i].SetReadOnly(TRUE);
		m_edt_pgr[i].SetMouseClick(FALSE);
	}
}

/*
 desc : Tab Control�� �÷��� �ڽ� ���̾�α� ���� �� ����
 parm : None
 retn : None
*/
VOID CDlgMain::InitTab()
{
	/* Setup the Tab Control */
	m_tab_ctl[0].SetTabWndBackColor(RGB(255, 255, 255));	/* �� ��� ���� */
	m_tab_ctl[0].SetTabBtnBackColor(RGB(255, 255, 255));	/* �� ��ư ���� */
	m_tab_ctl[0].SetNormalColor(RGB(96, 96, 96));			/* �� ���� ���� ���� */
	m_tab_ctl[0].SetSelectedColor(RGB(0, 0, 0));			/* �� ����   ���� ���� */
	m_tab_ctl[0].SetMouseOverColor(RGB(0, 32, 255));		/* �� ���콺 ���� ���� */
	m_tab_ctl[0].SetTopLeftCorner(CPoint(28, 63));
	m_tab_ctl[0].SetTabSize(100, 25);
	m_tab_ctl[0].SetMinTabWidth(50);

	/* Expo View (Printing) */
	m_pDlgExpo	= new CDlgExpo(IDD_EXPO, this);
	m_pDlgExpo->Create();
	m_tab_ctl[0].AddTab(m_pDlgExpo, L"Printing");
	/* Live View */
	m_pDlgLive	= new CDlgLive(IDD_LIVE, this);
	m_pDlgLive->Create();
	m_tab_ctl[0].AddTab(m_pDlgLive, L"Live View");
	/* Edge Find */
	m_pDlgFind	= new CDlgFind(IDD_FIND, this);
	m_pDlgFind->Create();
	m_tab_ctl[0].AddTab(m_pDlgFind, L"Edge Find");
	/* Edge Find */
	m_pDlgEdge	= new CDlgEdge(IDD_EDGE, this);
	m_pDlgEdge->Create();
	m_tab_ctl[0].AddTab(m_pDlgEdge, L"Edge Draw");
	/* Histogram View */
	m_pDlgHist	= new CDlgHist(IDD_HIST, this);
	m_pDlgHist->Create();
	m_tab_ctl[0].AddTab(m_pDlgHist, L"Histogram");

	/* ���� ���õ� ��ü ���� */
	m_tab_ctl[0].SelectTab(0);
}

/*
 desc : Tab Control�� �÷��� �ڽ� ���̾�α� ����
 parm : None
 retn : None
*/
VOID CDlgMain::CloseTab()
{
	m_tab_ctl[0].DeleteAllTabs();
	KillChild(m_pDlgExpo);
	KillChild(m_pDlgEdge);
	KillChild(m_pDlgFind);
	KillChild(m_pDlgHist);
	KillChild(m_pDlgLive);
}

/*
 desc : �ڽ� ���� �޸� ����
 parm : dlg	- [in]  ���̾�α� ��ü
 retn : None
*/
VOID CDlgMain::KillChild(CDlgMenu *dlg)
{
	if (dlg)
	{
		if (dlg->GetSafeHwnd())	dlg->DestroyWindow();
		delete dlg;
	}
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

	/* for Align Camera - üũ ��ư Ȱ��ȭ ó�� */
	for (i=0; i<pstCAM->acam_count; i++)
	{
		m_chk_cam[i].EnableWindow(TRUE);
	}
	/* for Align Camera - ����Ʈ ��ư Ȱ��ȭ ó�� */
	m_edt_mc2[1].EnableWindow(FALSE);

	/* Auto Focus ������ ���� Step Size �ʱ� �� (����: um) */
	m_edt_opt[0].SetTextToNum(UINT32(2));
	m_edt_opt[1].SetTextToNum(UINT32(15));
	m_edt_opt[2].SetTextToNum(UINT32(10));
	m_edt_opt[4].SetTextToNum(0.025, 4);
}

/*
 desc : �׸��� ��Ʈ�� ����
 parm : None
 retn : None
*/
VOID CDlgMain::InitGrid()
{
	TCHAR tzCols[3][16]	= { L"Item", L"Pos (mm)" }, tzRows[32] = {NULL};
	INT32 i32Width[3]	= { 44, 71 }, i, j=1;
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
	i32RowCnt	= pstCfg->luria_svc.ph_count + pstCfg->mc2_svc.drive_count;
	/* Basler ī�޶��� ��� Z Axis�� ��ġ (����) �� ��� ���� */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)pstCfg->set_comn.align_camera_kind)
	{
		/* Align Camera�� Z Axis ���� ���� ��ġ �� ���� */
		m_u32ACamZAxisRow	= i32RowCnt;
		/* Motion Left / Right ��ġ�� Up / Down ���� ��ġ */
		i32RowCnt	+= pstCfg->set_cams.acam_count;
	}

	/* �׸��� ��Ʈ�� �⺻ ���� �Ӽ� */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* ��ü ���� */
	m_pGridPos	= new CGridCtrl;
	ASSERT(m_pGridPos);
	m_pGridPos->SetModifyStyleEx(WS_EX_STATICEDGE);
	m_pGridPos->SetDrawScrollBarHorz(FALSE);
	m_pGridPos->SetDrawScrollBarVert(TRUE);
	if (!m_pGridPos->Create(rGrid, this, IDC_MAIN_GRID_MOTOR_POS, dwStyle))
	{
		delete m_pGridPos;
		m_pGridPos = NULL;
		return;
	}
	/* ��ü �⺻ ���� */
	m_pGridPos->SetLogFont(g_lf);
	m_pGridPos->SetRowCount(i32RowCnt+1);
	m_pGridPos->SetColumnCount(i32ColCnt);
	m_pGridPos->SetFixedRowCount(1);
	m_pGridPos->SetRowHeight(0, 24);
	m_pGridPos->SetFixedColumnCount(0);
	m_pGridPos->SetBkColor(RGB(255, 255, 255));
	m_pGridPos->SetFixedColumnSelection(0);

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
		m_pGridPos->SetItem(&stGV);
		m_pGridPos->SetColumnWidth(i, i32Width[i]);
	}

	/* ���� ���� */
	stGV.crBkClr = RGB(255, 255, 255);

	/* ���� ���� - for Photohead Position */
	for (i=0; i<pstCfg->luria_svc.ph_count; i++,j++)
	{
		m_pGridPos->SetRowHeight(j, 24);

		/* for Subject */
		stGV.strText.Format(L"PH %d", i+1);
		stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
		stGV.row	= j;
		stGV.col	= 0;
		m_pGridPos->SetItem(&stGV);

		/* for Subject */
		stGV.strText= L"0.0000";
		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.col	= 1;
		m_pGridPos->SetItem(&stGV);
	}

	/* ���� ���� - for Motion Drive Position */
	for (i=0; i<pstCfg->mc2_svc.drive_count; i++,j++)
	{
		m_pGridPos->SetRowHeight(j, 24);

		/* for Subject */
		stGV.strText.Format(L"MC %d", pstCfg->mc2_svc.axis_id[i]);
		stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
		stGV.row	= j;
		stGV.col	= 0;
		m_pGridPos->SetItem(&stGV);

		/* for Subject */
		stGV.strText= L"0.0000";
		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.col	= 1;
		m_pGridPos->SetItem(&stGV);
	}

	/* ���� ���� - for Basler Camera Position */
	if (pstCfg->set_comn.align_camera_kind == (UINT8)ENG_VCPK::en_camera_basler_ipv4)
	{
		/* Z Axis Up / Down Postion */
		for (i=0; i<pstCfg->set_cams.acam_count; i++,j++)
		{
			m_pGridPos->SetRowHeight(j, 24);

			/* for Subject */
			stGV.strText.Format(L"AZ %d", i+1);
			stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
			stGV.row	= j;
			stGV.col	= 0;
			m_pGridPos->SetItem(&stGV);

			/* for Subject */
			stGV.strText= L"0.0000";
			stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
			stGV.col	= 1;
			m_pGridPos->SetItem(&stGV);
		}
	}

	m_pGridPos->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGridPos->UpdateGrid();
}

/*
 desc : �׸��� ��Ʈ�� ����
 parm : None
 retn : None
*/
VOID CDlgMain::CloseGrid()
{
	/* for Motor Position */
	if (m_pGridPos)
	{
		if (m_pGridPos->GetSafeHwnd())	m_pGridPos->DestroyWindow();
		delete m_pGridPos;
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
	case IDC_MAIN_BTN_MC2_MOVE_UP	:	MotionMove(ENG_MDMD::en_move_up);	break;
	case IDC_MAIN_BTN_MC2_MOVE_DN	:	MotionMove(ENG_MDMD::en_move_down);	break;
	case IDC_MAIN_BTN_ACAM_MOVE		:	ACamMoveAbs();						break;
	case IDC_MAIN_BTN_START			:	StartFocus();						break;
	case IDC_MAIN_BTN_STOP			:	StopFocus();						break;
	case IDC_MAIN_BTN_MOVE			:	MovePosMeasure();					break;
	case IDC_MAIN_BTN_SAVE			:	SaveDataToFile();					break;
	case IDC_MAIN_BTN_ACAM_HOMED	:	ACamZAxisRecovery();				break;
	case IDC_MAIN_BTN_CH			:	SetTriggerCh();						break;
	}
}

/*
 desc : üũ (for Align Camera) Ŭ�� �̺�Ʈ
 parm : id	- [in]  Click ID
 retn : None
*/
VOID CDlgMain::OnChkClickACam(UINT32 id)
{
	UINT8 i	= 0x00;

	switch (id)
	{
	case IDC_MAIN_CHK_ACAM_NO_1	:
	case IDC_MAIN_CHK_ACAM_NO_2	:
	case IDC_MAIN_CHK_ACAM_NO_3	:
	case IDC_MAIN_CHK_ACAM_NO_4	:
		for (i=0; i<4; i++)	m_chk_cam[i].SetCheck(0);
		m_chk_cam[id-IDC_MAIN_CHK_ACAM_NO_1].SetCheck(1);
		break;
	case IDC_MAIN_CHK_ACAM_LIVE	:	SetLiveView();	break;
	}
}

/*
 desc : üũ (for Focusing Option) Ŭ�� �̺�Ʈ
 parm : id	- [in]  Click ID
 retn : None
*/
VOID CDlgMain::OnChkClickOption(UINT32 id)
{
	UINT8 i	= 0x00;

	switch (id)
	{
	case IDC_MAIN_CHK_OPTION_UP	:
	case IDC_MAIN_CHK_OPTION_DN	:
		for (i=0; i<2; i++)	m_chk_opt[i].SetCheck(0);
		m_chk_opt[id-IDC_MAIN_CHK_OPTION_UP].SetCheck(1);
		break;
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
	case MSG_ID_NORMAL		: UpdatePeriod();		break;
	case MSG_ID_STEP_RESULT	: UpdateStepResult();	break;
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
	/* for Result (Live View / Edge Histogram / Edge Object) */
	UpdateResult();
	/* for Motor Position (mm) */
	UpdateMotorPos();
	/* for PLC Value */
	UpdatePLCVal();
	/* Control : Enable/Disable */
	UpdateEnableCtrl();
	/* Update Child Dialog */
	UpdateChildCtrl();
}

/*
 desc : Focus ���� ��� ����
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateStepResult()
{
	m_pDlgFind->RefreshData();
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

	/* ���� ���õ� Tab Position */
	i32Tab	= m_tab_ctl[0].GetCurSel();
	if (i32Tab < 0)	return;

	/* ����� ī�޶� ����Ǿ� ���� ������ */
	if (!uvCmn_Camera_IsConnected())	bEnable	= FALSE;
	/* ����� ī�޶� ����Ǿ� �ִٸ� */
	else
	{
		/* ���� Live View ȭ���� �ƴϸ� */
		if (i32Tab != 1)	bEnable	= FALSE;
		/* ���� Live View ȭ���̸�, ���� ���� ������� ���ο� ���� */
		else				bEnable	= !m_pMainThread->IsRunFocus();
	}

	/* Live Check ��ư Ȱ��ȭ �� üũ ���� ó�� */
	m_chk_cam[4].EnableWindow(bEnable);
	if (!bEnable)
	{
		if (m_chk_cam[4].GetCheck())	m_chk_cam[4].SetCheck(0);
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
	CDlgMenu *pDlgMenu	= NULL;

	/* ���� ���� ������ ���� */
	if (!m_pMainThread->IsRunFocus())	return;

	/* ���� ���õ� Tab�� �ڽ� ���� ��� */
	i32Tab	= m_tab_ctl[0].GetCurSel();
	if (i32Tab < 0)	return;

	/* Grid Control ���� �� ���� */
	pDlgMenu	= (CDlgMenu*)m_tab_ctl[0].GetTabWnd(i32Tab);
	if (pDlgMenu)	pDlgMenu->UpdateCtrl();

	/* ������� ����� ��� */
	swprintf_s(tzVal, 64, L"%.2f %%", m_pMainThread->GetWorkStepRate());
	m_edt_pgr[0].SetTextToStr(tzVal, TRUE);
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

	/* Luria Photohead ��ġ */
	for (i=0; i<pstCfg->luria_svc.ph_count; i++, j++)
	{
		dbPos[0] = m_pGridPos->GetItemTextToFloat(j, 1);
		dbPos[1] = (DOUBLE)ROUNDED(pstDP->focus_motor_move_abs_position[i]/1000.0f, 3);
		if (dbPos[0] != dbPos[1])
		{
			m_pGridPos->SetItemText(j, 1, dbPos[1]);
			m_pGridPos->InvalidateRowCol(j, 1);
		}
	}

	/* MC2 SD2S Drive ��ġ (Basler Camera ����) */
	for (i=0; i<pstCfg->mc2_svc.drive_count; i++, j++)
	{
		dbPos[0] = m_pGridPos->GetItemTextToFloat(j, 1);
		dbPos[1] = (DOUBLE)ROUNDED(pstMC2->act_data[pstCfg->mc2_svc.axis_id[i]].real_position/10000.0f, 4);
		if (dbPos[0] != dbPos[1])
		{
			m_pGridPos->SetItemText(j, 1, dbPos[1]);
			m_pGridPos->InvalidateRowCol(j, 1);
		}
	}

	/* Align Camera ��ġ (Basler ī�޶��� ��츸 �ش�) */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)pstCfg->set_comn.align_camera_kind)
	{
		for (i=0; i<pstCfg->set_cams.acam_count; i++, j++)
		{
			dbPos[0] = m_pGridPos->GetItemTextToFloat(j, 1);
			dbPos[1] = (DOUBLE)ROUNDED(uvCmn_MCQ_GetACamCurrentPosZ(i+1), 4);
			if (dbPos[0] != dbPos[1])
			{
				m_pGridPos->SetItemText(j, 1, dbPos[1]);
				m_pGridPos->InvalidateRowCol(j, 1);
			}
		}
	}
}

/*
 desc : ���� ���� (üũ)�� ����� ī�޶� ��ȣ ���
 parm : None
 retn : ���õ� ����� ī�޶� ��ȣ (1-based), ������ ��� 0xff
*/
UINT8 CDlgMain::GetCheckACam()
{
	UINT8 i	= 0x00;

	for (; i<uvEng_GetConfig()->set_cams.acam_count; i++)
	{
		if (m_chk_cam[i].GetCheck())	return (i+1);
	}

	return 0xff;
}

/*
 desc : ���� ��� ���� PLC �� ����
 parm : None
 retn : None
*/
VOID CDlgMain::UpdatePLCVal()
{
	UINT8 u8ACamNo	= GetCheckACam();
	INT32 i32PosZ	= 0 /* 0.1 um or 100 nm */;
	UINT32 u32Velo	= 0;
	LPG_CIEA pstCfg	= uvEng_GetConfig();

	/* ���õ� ī�޶� ���� ��� */
	if (0xff == u8ACamNo)	return;
	/* Align Camera (for Basler) - Z Axis Move Step & Speed */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)pstCfg->set_comn.align_camera_kind)
	{
		/* ���� Align Camera�� Z Axis �̵� �ӵ� �� ���� ��ġ (Position) �� ��� */
		if (u8ACamNo == 0x01)	u32Velo	= uvCmn_MCQ_GetShMemStruct()->w_camera1_z_axis_position_speed_setting;
		else					u32Velo	= uvCmn_MCQ_GetShMemStruct()->w_camera2_z_axis_position_speed_setting;
		i32PosZ	= (INT32)ROUNDED(uvCmn_MCQ_GetACamCurrentPosZ(u8ACamNo) * 10000.0f, 0);
		if (m_i32ACamZAxisPos != i32PosZ)
		{
			TCHAR tzVal[32]	= {NULL};
			swprintf_s(tzVal, 32, L"%.4f", i32PosZ / 10000.0f);
			m_pGridPos->SetItemText(m_u32ACamZAxisRow, 1, tzVal);	/* Position */
			m_i32ACamZAxisPos = i32PosZ;
		}
	}
}

/*
 desc : �ڽ� ���̾�α� ���� ����
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateChildCtrl()
{
	INT32 i32Tab	= -1;
	CDlgMenu *pDlgMenu	= NULL;

	/* ���� ���õ� Tab�� �ڽ� ���� ��� */
	i32Tab	= m_tab_ctl[0].GetCurSel();
	if (i32Tab < 0)	return;

	/* Grid Control ���� �� ���� */
	pDlgMenu	= (CDlgMenu*)m_tab_ctl[0].GetTabWnd(i32Tab);
	if (pDlgMenu)	pDlgMenu->UpdateCtrl();
}

/*
 desc : ���� ��Ʈ�� Enable or Disable
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateEnableCtrl()
{
	UINT8 i	= 0x00;
	BOOL bBusyMotion	= FALSE, bBusyRunFocus = FALSE;
	LPG_CMSI pstMC2Svc	= &uvEng_GetConfig()->mc2_svc;

	/* Focus ���� ���� ���� */
	bBusyRunFocus	= m_pMainThread->IsRunFocus();
	/* Motion Busy ���� */
	bBusyMotion		= uvCmn_MC2_IsAnyDriveBusy();
	/* MC2/Align Camera Up/Down Moving Button */
	for (i=0x00; i<3; i++)
	{
		m_btn_ctl[i].EnableWindow(!(bBusyMotion || bBusyRunFocus));
	}
	/* MC2 Drive Check Button */
	for (i=0; i<uvEng_GetConfig()->mc2_svc.drive_count; i++)
	{
		m_chk_mc2[pstMC2Svc->axis_id[i]].EnableWindow(!(bBusyMotion || bBusyRunFocus));
	}
	/* Align Camera Check Button */
	for (i=0; i<uvEng_GetConfig()->set_cams.acam_count; i++)
	{
		m_chk_cam[i].EnableWindow(!(bBusyMotion || bBusyRunFocus));
	}
	m_chk_opt[0].EnableWindow(!(bBusyMotion || bBusyRunFocus));
	m_chk_opt[1].EnableWindow(!(bBusyMotion || bBusyRunFocus));

	/* Normal Button */
	m_btn_ctl[3].EnableWindow(!bBusyRunFocus);					/* Reset */
	m_btn_ctl[4].EnableWindow(!(bBusyMotion || bBusyRunFocus));	/* Move */
	m_btn_ctl[5].EnableWindow(!(bBusyMotion || bBusyRunFocus));	/* Start */
	m_btn_ctl[6].EnableWindow(bBusyRunFocus);					/* Stop */
	m_btn_ctl[7].EnableWindow(!(bBusyMotion || bBusyRunFocus));	/* Save */
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
 desc : ��� (Motor Drive) �̵�
 parm : direct	- [in]  Up or Down
 retn : None
*/
VOID CDlgMain::MotionMove(ENG_MDMD direct)
{
	UINT8 i	= 0x00, u8DrvNo = 0x00;	/* 0x00 ~ 0x07 */
	DOUBLE dbDrvPos	= 0, dbMinPos, dbMaxPos, dbDist, dbVelo;
	BOOL bCheck		= FALSE;
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
	if (dbDist < 0.001f /* 1 um */ || dbVelo < 0.1f /* 100 um/sec */)
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
 desc : Align Camera (Motor Drive) �̵� (���� ��ġ�� �̵�)
 parm : None
 retn : None
*/
VOID CDlgMain::ACamMoveAbs()
{
	UINT8 u8ACamNo	= GetCheckACam()/*, u8PhNo*/;
	DOUBLE dbPosZ	= 0.0f;

	/* ���õ� ī�޶� ���� ��� */
	if (0xff == u8ACamNo)	return;

	/* Photohead or Align Camera Z Axis �̵� */
	dbPosZ	= m_edt_cam[0].GetTextToDouble();
	/* ���ο� ��ġ�� �̵� */
	uvEng_MCQ_SetACamMovePosZ(u8ACamNo, 0x00, dbPosZ);
}

/*
 desc : ���� ��ġ�� �̵�
 parm : None
 retn : None
*/
VOID CDlgMain::MovePosMeasure()
{
	UINT8 i			= 0x00;
	UINT8 u8ACamNo	= GetCheckACam();
	LPG_CIEA pstCfg	= (LPG_CIEA)uvEng_GetConfig();
	if (!pstCfg)	return;

	/* ���õ� ī�޶� ���� ��� */
	if (0xff == u8ACamNo)	return;
	/* �̵� �ӵ��� ������ ���� �ʰų� �ʹ� �����ٸ� */
	if (m_edt_mc2[1].GetTextToDouble() < 10.0f)
	{
		AfxMessageBox(L"The moving speed value of the motion is too small", MB_ICONSTOP|MB_TOPMOST);
		return;	/* 10 mm/mm �̵� �ӵ� ������ ��� */
	}

	/* ��� X �� �̵� */
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x,
							 pstCfg->acam_focus.mark2_stage_x,
							 m_edt_mc2[1].GetTextToDouble());
	/* ��� Y �� �̵� */
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y,
							 pstCfg->acam_focus.mark2_stage_y[0],
							 m_edt_mc2[1].GetTextToDouble());

	/* ��� Camera �� �̵� */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)pstCfg->set_comn.align_camera_kind)
	{
		/* 2 �� ī�޶� ������ ��� */
		if (0x02 == u8ACamNo)
		{
			/* 1 ��° ī�޶� �� �������� �̵� ��Ŵ */
			uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1,
									 0.0f,
									 m_edt_mc2[1].GetTextToDouble());
			/* 2 ��° ī�޶� ���� ��ġ�� �̵� ��Ŵ */
			uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2,
									 pstCfg->acam_focus.mark2_acam[1],
									 m_edt_mc2[1].GetTextToDouble());
		}
		else
		{
			/* 2 ��° ī�޶� �� ���������� �̵� ��Ŵ */
			uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam2,
									 pstCfg->mc2_svc.max_dist[(UINT8)ENG_MMDI::en_align_cam2],
									 m_edt_mc2[1].GetTextToDouble());
			/* 1 ��° ī�޶� ���� ��ġ�� �̵� ��Ŵ */
			uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1,
									 pstCfg->acam_focus.mark2_acam[0],
									 m_edt_mc2[1].GetTextToDouble());
		}
	}

	/* ����� ī�޶� Z �� �̵� */
	uvEng_MCQ_SetACamMovePosZ(u8ACamNo, 0x00, pstCfg->acam_focus.acam_z_focus[u8ACamNo-1]);
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
 desc : ��Ŀ�� ���� ����
 parm : None
 retn : None
*/
VOID CDlgMain::StartFocus()
{
	UINT8 u8ACamNo	= GetCheckACam();
	UINT8 u8ChNo	= u8ACamNo == 1 ? 0x00 : 0x02;
	BOOL bValid		= TRUE;
	ENG_MDMD enUpDown;
	UINT32 u32Skip, u32Repeat, u32Total;
	DOUBLE dbStep	= 0.0f, dbPosZ = 0.0f, dbSkipTotal;
	LPG_CIEA pstCfg	= uvEng_GetConfig();

	/* ���� ������ ������ �� ���� �ʱ�ȭ */
	m_pDlgEdge->ResetData();
	m_pDlgFind->ResetData();
	m_pDlgHist->ResetData();
	m_pDlgLive->ResetData();

	/* ���� ��Ŀ�� ���� ��� ���� �� ���� �ʱ�ȭ */
	ResetFocus();

	/* ���õ� ī�޶� ���� ��� */
	if (0xff == u8ACamNo)	return;
	/* �����忡 ���� ���� �Ķ���� ���� */
	enUpDown	= m_chk_opt[0].GetCheck() > 0 ? ENG_MDMD::en_move_up : ENG_MDMD::en_move_down;
	dbStep		= m_edt_opt[4].GetTextToDouble();
	u32Skip		= (UINT32)m_edt_opt[0].GetTextToNum();
	u32Repeat	= (UINT32)m_edt_opt[1].GetTextToNum();
	u32Total	= (UINT32)m_edt_opt[2].GetTextToNum();

	/* �̵� ���⿡ ����, �̵� ���� ũ�⿡ ��ȣ (��� or ����) �� ���� */
	if (enUpDown == ENG_MDMD::en_move_up)	dbStep	*= 1.0f;
	else								dbStep	*= -1.0f;

	/* --------------------------------------- */
	/* ���� �Էµ� �Ķ���� ���� ��ȿ���� �Ǵ� */
	/* --------------------------------------- */

	/* Step ũ�⳪ Skip ���� �׸��� Total ���� 0 ���� ū�� */
	if (0.0f == ROUNDED(dbStep, 4))
	{
		AfxMessageBox(L"Please enter a valid value", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* ��� Step�� Total Ƚ����ŭ �̵� ���� ��, ���Կ� ������ �� �ִ��� Ȯ�� */
	dbPosZ		= uvCmn_MCQ_GetACamCurrentPosZ(u8ACamNo);
	dbSkipTotal	= dbStep * (u32Total - 1);	/* unit: mm */
	dbPosZ		+= dbSkipTotal;

	/* Z Axis Up/Down Limit ���� */
#if 1
	bValid	= uvCmn_Camera_IsZPosUpDownLimit(dbPosZ);
#else
	/* In case of basler type */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)pstCfg->set_comn.align_camera_kind)
	{
		if (pstCfg->set_basler.z_axis_limit[0] > dbPosZ)	bValid	= FALSE;
		if (pstCfg->set_basler.z_axis_limit[1] < dbPosZ)	bValid	= FALSE;
	}
	/* In case of ids type */
	else
	{
		if (pstCfg->set_ids.z_axis_limit[0] > dbPosZ)	bValid	= FALSE;
		if (pstCfg->set_ids.z_axis_limit[1] < dbPosZ)	bValid	= FALSE;
	}
#endif
	if (!bValid)
	{
		AfxMessageBox(L"Z-Axis position is out of bounds", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* Trigger ���� �Ӽ� �� ���� (Trigger OnTime / Strobe OnTime / Trigger Delay Time */
	UpdateTrigParam();
	if (!uvEng_Trig_ReqInitUpdate())
	{
		AfxMessageBox(L"Failed to set the trigger attribute for LED Lamp", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* �����忡 �� ���� �� ���� ��Ŵ*/
	m_pMainThread->StartFocus(u8ACamNo, dbStep, u32Skip, u32Repeat, u32Total);
}

/*
 desc : ���� �˻��� ���� ��� ����
 parm : None
 retn : None
*/
VOID CDlgMain::ResetFocus()
{
	TCHAR tzPath[MAX_PATH_LEN]	= {NULL};
	/* ���� Calibration �� ���� ���� */
	swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\vdof", g_tzWorkDir);
	uvCmn_DeleteAllFiles(tzPath);
}

/*
 desc : ���� ó�� ��� ��ϵ� ���� ��ȯ
 parm : None
 retn : ���� ��ȯ
*/
UINT32 CDlgMain::GetGrabCount()
{
	return m_pMainThread->GetGrabCount();
}

/*
 desc : ������� ������ ������ ��ȯ
 parm : index	- [in]  ���������� �ϴ� �޸� ��ġ (Zero-based)
 retn : index ��ġ�� ����� ���� ��
*/
LPG_ZAAL CDlgMain::GetGrabData(UINT32 index)
{
	return m_pMainThread->GetGrabData(index);
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
	if (m_chk_cam[4].GetCheck())	/* Live View */
	{
		/* Trigger Parameter ���� */
		UpdateTrigParam();
		/* Live ��� ��� */
		m_pDlgLive->SetLive(TRUE);
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
		/* Live ��� ���� */
		m_pDlgLive->SetLive(FALSE);

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
 desc : ���� Edge ó�� ��ü ������ ��ȯ
 parm : None
 retn : Edge ó�� ��� ��ü ������
*/
CMeasure *CDlgMain::GetMeasure()
{
	return m_pMainThread->GetMeasure();
}

/*
 desc : ���� ����� Edge Image�� ���� �� ������ ��ȯ
 parm : None
 retn : ���� �� ����ü ������ ��ȯ
*/
LPG_ZAAL CDlgMain::GetGrabSel()
{
	return m_pMainThread->GetGrabSel();
}

/*
 desc : ������ Min / Max �� ��ȯ
 parm : mode	- [in]  0x00: Min, 0x01: Max
		type	- [in]  0x00 : strength, 0x01 : length, 0x02 : Feret, 0x03 : Diameter
 retn : �� ��ȯ
*/
DOUBLE CDlgMain::GetMinMax(UINT8 mode, UINT8 type)
{
	return m_pMainThread->GetMinMax(type, type);
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

	/* Focus�� �����Ǵ� ���� �β� */
	m_edt_opt[3].SetTextToNum(pstCfg->set_align.dof_film_thick, 4);
}

/*
 desc : ī�޶��� ���� Ÿ�Կ� ���� Ʈ���� ������ ä�� ��ȣ �� ��ȯ
 parm : None
 retn : 0 or Later
*/
UINT8 CDlgMain::GetTrigChNo()
{
	UINT8 u8ChNo	= 0x00;
	UINT8 u8ACamNo	= GetCheckACam();
	LPG_CIEA pstCfg	= uvEng_GetConfig();

	if (u8ACamNo == 0xff)	return u8ChNo;

	switch (pstCfg->set_comn.strobe_lamp_type)
	{
	case 0x00	:	u8ChNo	= u8ACamNo == 1 ? 0x00 : 0x02;	break;	/* Amber */
	case 0x01	:	u8ChNo	= u8ACamNo == 1 ? 0x01 : 0x03;	break;	/* IR */
	}

	return u8ChNo;
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

	swprintf_s(tzChNo, 32, L"CH [ %u ]", u8ChNo);
	m_btn_ctl[9].SetWindowTextW(tzChNo);

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
 desc : ���� ������ ���� ȯ�� ���Ͽ� �����
 parm : None
 retn : None
*/
VOID CDlgMain::SaveDataToFile()
{
	UINT8 u8ACamNo	= GetCheckACam(), u8ChNo = GetTrigChNo();
	LPG_CIEA pstCfg	= uvEng_GetConfig();

	if (0xff == u8ACamNo)
	{
		AfxMessageBox(L"No camera is selected", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* ���� MC2�� ������ �̵� �Ÿ� �� �ӵ� �� ���� */
	pstCfg->mc2_svc.move_dist	= m_edt_mc2[0].GetTextToDouble();
	pstCfg->mc2_svc.move_velo	= m_edt_mc2[1].GetTextToDouble();

	/* Align Camera�� ������ �̵� �ܰ� �Ÿ� �� ���� */
	pstCfg->acam_focus.step_move_z		= m_edt_opt[4].GetTextToDouble();
	/* Focus�� �����Ǵ� ���� �β� */
	pstCfg->set_align.dof_film_thick	= m_edt_opt[3].GetTextToDouble();

	/* Trigger Time */
	pstCfg->trig_step.trig_on_time[u8ChNo]		= (UINT32)m_edt_trg[0].GetTextToNum();
	pstCfg->trig_step.strob_on_time[u8ChNo]		= (UINT32)m_edt_trg[1].GetTextToNum();
	pstCfg->trig_step.trig_delay_time[u8ChNo]	= (UINT32)m_edt_trg[2].GetTextToNum();

	/* ȯ�� ���� ���� ���Ϸ� ���� */
	if (!uvEng_SaveConfig())	return;
#ifndef _DEBUG
	AfxMessageBox(L"Save completed successfully", MB_OK);
#endif
}

/*
 desc : Align Camera - Recovery (Error ����)
 parm : None
 retn : None
*/
VOID CDlgMain::ACamZAxisRecovery()
{
	/* Align Camera Z Axis Homing */
	uvEng_MCQ_SetACamHomingZAxis(1);
	uvEng_MCQ_SetACamHomingZAxis(2);
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
	UINT8 u8ACamNo	= GetCheckACam();
	UINT8 u8ChNo	= u8ACamNo == 1 ? 0x00 : 0x02;
	LPG_CIEA pstCfg	= uvEng_GetConfig();

	/* Trigger ���� �Ӽ� �� ���� (Trigger OnTime / Strobe OnTime / Trigger Delay Time */
	pstCfg->trig_step.trig_on_time[u8ChNo]		= (UINT32)m_edt_trg[0].GetTextToNum();
	pstCfg->trig_step.strob_on_time[u8ChNo]		= (UINT32)m_edt_trg[1].GetTextToNum();
	pstCfg->trig_step.trig_delay_time[u8ChNo]	= (UINT32)m_edt_trg[2].GetTextToNum();
}