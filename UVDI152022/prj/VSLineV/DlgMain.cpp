
/*
 desc : Main Dialog
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgMain.h"

#include "MainThread.h"

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
#if 0
	UINT32 i, u32StartID;
	/* Normal - Group box */
	u32StartID	= IDC_MAIN_GRP_MC2;
	for (i=0; i<8; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Normal - Text */
	u32StartID	= IDC_MAIN_TXT_MC2_MOVE;
	for (i=0; i<21; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Normal - Button */
	u32StartID	= IDC_MAIN_BTN_MC2_MOVE_UP;
	for (i=0; i<15; i++)	DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);

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
	u32StartID	= IDC_MAIN_EDT_STROBE_ONTIME;
	for (i=0; i<3; i++)		DDX_Control(dx, u32StartID+i,	m_edt_trg[i]);

	/* for Work status */
	u32StartID	= IDC_MAIN_EDT_STEP;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_edt_pgr[i]);
#endif
}

BEGIN_MESSAGE_MAP(CDlgMain, CMyDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MAIN_THREAD,	OnMsgThread)
#if 0
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_BTN_MC2_MOVE_UP,	IDC_MAIN_BTN_EXIT,		OnBtnClicked)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_CHK_ACAM_LIVE,	IDC_MAIN_CHK_ACAM_LIVE,	OnChkClickACam)
#endif
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
#if 0
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
	for (i=0; i<15; i++)
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
		m_edt_mc2[i].SetInputType(EN_DITM::en_float);
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
		m_edt_cam[i].SetInputType(EN_DITM::en_float);
	}
	/* for Trigger Time */
	for (i=0; i<3; i++)
	{
		m_edt_trg[i].SetEditFont(&g_lf);
		m_edt_trg[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_trg[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_trg[i].SetInputType(EN_DITM::en_hex10);
	}
	/* for Work Status */
	for (i=0; i<1; i++)
	{
		m_edt_pgr[i].SetEditFont(&g_lf);
		m_edt_pgr[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_pgr[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_pgr[i].SetInputType(EN_DITM::en_float);
	}
	/* for Measurement Option */
	for (i=0; i<5; i++)
	{
		m_edt_opt[i].SetEditFont(&g_lf);
		m_edt_opt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_opt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		if (0x03 < i)	m_edt_opt[i].SetInputType(EN_DITM::en_float);
		else			m_edt_opt[i].SetInputType(EN_DITM::en_int);
	}

	/* Align Camera Focus���� ������ ���� �������Ƿ� ... */
	m_edt_opt[1].SetReadOnly(TRUE);
	m_edt_opt[1].SetMouseClick(FALSE);
#endif
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
#if 0
	switch (id)
	{
	case IDC_MAIN_BTN_EXIT			:	PostMessage(WM_CLOSE, 0, 0L);		break;
	case IDC_MAIN_BTN_MC2_RESET		:	MC2Recovery();	ACamZAxisHome();	break;
	case IDC_MAIN_BTN_NEXT			:	NextStepMoveY();					break;
	case IDC_MAIN_BTN_ACAM_MOVE		:	ACamMoveAbs();						break;
	case IDC_MAIN_BTN_ACAM_HOMED	:	ACamHoming();						break;
	case IDC_MAIN_BTN_START			:	StartFocus();						break;
	case IDC_MAIN_BTN_STOP			:	StopFocus();						break;
	case IDC_MAIN_BTN_MOVE			:	MovePosMeasure();					break;
	case IDC_MAIN_BTN_SAVE			:	SaveDataToFile();					break;
	case IDC_MAIN_BTN_MC2_MOVE_UP	:	MotionMove(ENG_MDMD::en_move_up);		break;
	case IDC_MAIN_BTN_MC2_MOVE_DN	:	MotionMove(ENG_MDMD::en_move_down);		break;
	case IDC_MAIN_BTN_MODEL_FIND	:	RunFindModel();						break;
	case IDC_MAIN_BTN_MODEL_RESET	:	ResetFindModel();					break;
	case IDC_MAIN_BTN_TRIG_SET		:	SetTriggerParam();					break;
	case IDC_MAIN_BTN_FILE_XLS		:	SaveAsExcelFile();					break;
	}
#endif
}

/*
 desc : Main Thread���� �߻��� �̺�Ʈ ó��
 parm : wparam	- [in]  Event ID
		lparam	- [in]  Event Data
 retn : 0L
*/
LRESULT CDlgMain::OnMsgThread(WPARAM wparam, LPARAM lparam)
{
#if 0
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
#endif
	return 0L;
}

/*
 desc : �ֱ������� ���ŵǴ� �׸�
 parm : None
 retn : None
*/
VOID CDlgMain::UpdatePeriod()
{
}
