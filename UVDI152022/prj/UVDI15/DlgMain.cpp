
/*
 desc : Main Dialog
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgMain.h"

#include "./imgs/Images.h"
#include "MainThread.h"

/* �ڽ� �޴� ȭ�� */
#include "GlobalVariables.h"
#include "./menu/DlgAuto.h"
#include "./menu/DlgExpo.h"
#include "./menu/DlgGerb.h"
#include "./menu/DlgJob.h"
#include "./menu/DlgMark.h"
#include "./menu/DlgCtrl.h"
#include "./menu/DlgLogs.h"
#include "./menu/DlgStat.h"
#include "./menu/DlgConf.h"
#include "./menu/DlgCalb.h" //  by sysandj : Calibration Dialog �߰�
#include "./menu/DlgManual.h"
#include "./menu/DlgPhilhmi.h"
#include "./menu/DlgMMPMAutoCenter.h"
#include "./mesg/DlgMesg.h"
#include "./pops/DlgWait.h"
#include "./param/RecipeManager.h"
#include "./param/IOManager.h"
#include "./param/LogManager.h"
#include "./param/InterLockManager.h"
#include "./param/RecvPhil.h"
#include "./db/DBMgr.h"
#include "stuffs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	USE_ENGINE_LIB	1	/* ���� ���̺귯�� ���� ���� */
Stuffs Stuffs::stuffUtils; // �ʱ�ȭ

void updatePender()
{
	auto updateCnt = GlobalVariables::GetInstance()->GetCount("mainUpdate");

	GlobalVariables::GetInstance()->Waiter([&]()->bool
		{
			return updateCnt != GlobalVariables::GetInstance()->GetCount("mainUpdate");
		});
}


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

	/* �ΰ� �̹��� */
	m_pImgPhil		= NULL;
	m_pImgCustomer		= NULL;
	/* �ڽ� �޴� ȭ�� */
	m_pDlgMenu		= NULL;
	m_pDlgWait		= NULL;

//#ifdef _DEBUG
	m_bDragMoveWnd	= TRUE;
	m_bMagnetic		= TRUE;
//#endif
	m_u64TickPeriod	= 0;
	/* ���� �ֱٿ� ���õ� �޴� ���̾�α� ID */
	m_enDlgID		= ENG_CMDI::en_menu_none;
	/* ���� �ֱ��� ���� ���� ��û �ð� �ӽ� ���� */
	m_u8NextReqCmd	= 0x00;
	m_u64ReqCmdTime	= GetTickCount64();
	m_u8MsgLogNext	= 0x00;
	m_u64MsgLogTime	= 0;	/* �ٷ� �����ϱ� ���� */

	/*Philhmi�� ���� ��� ��� ��ü��*/
	g_u8Romote = en_menu_phil_mode_none;
	m_stExpoLog.Init();
	m_stPhilStatus.Init();
	
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
	u32StartID	= IDC_MAIN_GRP_GERB_RECIPE;
	for (i=0; i< eMAIN_GRP_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* static - picture */
	u32StartID	= IDC_MAIN_PIC_CUSTOMER;
	for (i=0; i< eMAIN_PIC_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_pic_ctl[i]);
	/* static - normal */
	u32StartID	= IDC_MAIN_TXT_STATUS;
	for (i=0; i< eMAIN_TXT_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* button - normal */
	u32StartID	= IDC_MAIN_BTN_EXIT;
	for (i=0; i< eMAIN_BTN_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgMain, CMyDialog)
	ON_MESSAGE(WM_MAIN_THREAD,			OnMsgMainThread)
	ON_MESSAGE(WM_MAIN_CHILD,			OnMsgMainChild)
	ON_MESSAGE(WM_MAIN_RECIPE_UPDATE,	OnMsgMainRecipeUpdate)
	ON_MESSAGE(WM_MAIN_PROCESS_UPDATE,	OnMsgMainProcessUpdate)
	ON_MESSAGE(WM_PHILHMI_MSG_THREAD,	OnMsgMainPHILHMI)
	ON_MESSAGE(WM_STROBE_LAMP_MSG_THREAD, OnMsgMainStrobeLamp)
	ON_MESSAGE(WM_MAIN_RECIPE_CREATE,	OnMsgMainRecipeCreate)
	ON_MESSAGE(WM_MAIN_RECIPE_DELETE,	OnMsgMainRecipeDelete)
	ON_MESSAGE(WM_MAIN_RECIPE_CHANGE,	OnMsgMainPhilMsg)
	ON_MESSAGE(WM_MAIN_PHIL_MSG,		OnMsgMainRecipeChange)
	ON_WM_SYSCOMMAND()
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_BTN_EXIT, IDC_MAIN_BTN_EXIT + eMAIN_BTN_MAX,	OnBtnClicked)
	ON_MESSAGE(eMSG_MAIN_OPEN_CONSOLE, OnOpenMotorConsole)
	ON_MESSAGE(eMSG_MAIN_LOGIN_CONSOLE, OnOpenLoginConsole)
	ON_MESSAGE(eMSG_MAIN_ADMIN_CONSOLE, OnOpenAdminConsole)
	ON_MESSAGE(eMSG_MAIN_MANAGER_CONSOLE, OnOpenManagerConsole)
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
	
	UINT32 u32Size	= 0, i = 0;
#if (USE_ENGINE_LIB)
	/* ���̺귯�� �ʱ�ȭ */
	if (!InitLib())		return FALSE;
#endif
	//Stuffs::GetStuffs().RemoveOldFiles();
	/*���� �Ⱓ�� ���� Save File ���� */
	Stuffs::GetStuffs().RemoveOldFiles(
		uvEng_GetConfig()->monitor.bmp_delect_day, uvEng_GetConfig()->monitor.log_delect_day);
	
	uvEng_Luria_SetAlignMotionPtr(GlobalVariables::GetInstance()->GetAlignMotion());
	uvEng_Mark_SetAlignMotionPtr(GlobalVariables::GetInstance()->GetAlignMotion());
	uvEng_Camera_SetAlignMotionPtr(GlobalVariables::GetInstance()->GetAlignMotion());
	
	// by sysandj : ��Ʈ ����
	CreateUserFont();
	// by sysandj : ��Ʈ ����


	// by sysandj : recipe
	CRecipeManager::GetInstance()->Init(this->GetSafeHwnd());
	CIOManager::GetInstance()->Init(this->GetSafeHwnd());
	CRecvPhil::GetInstance()->Init(this->GetSafeHwnd());
	// by sysandj : recipe

	// by sysandj : log
	CLogManager::GetInstance()->InitManager(this->GetSafeHwnd());
	// by sysandj : log

	// DBMgr Ŭ���� �߰�
	CDBMgr::GetInstance()->Create(this);

	LPG_RAAF pstAlign = uvEng_Mark_GetSelectAlignRecipe();

	if (nullptr != pstAlign)	// 20230814 mhbaek Add : ���˻� �߰�
	{
		for (int i = 0; i < uvEng_GetConfig()->set_cams.acam_count; i++) {
			for (int j = 0; j < 2; j++) { // Global, Local 2��
				uvCmn_Camera_SetMarkFindMode(i + 1, pstAlign->mark_type, j);
			}
		}
	}

	/* ���� ��Ʈ�� �ʱ�ȭ */
	InitCtrl();
	/* �ΰ� ��� �̹��� �ʱ�ȭ */
	if (!LoadImages())	return FALSE;

	/* Main Thread ���� (!!! �߿�. InitLib �� �����ϸ�, �ݵ�� �����ؾ� �� !!!) */
	m_pMainThread	= new CMainThread(m_hWnd);
	ASSERT(m_pMainThread);
	if (!m_pMainThread->CreateThread(0, NULL, NORMAL_THREAD_SPEED))
	{
		delete m_pMainThread;
		m_pMainThread	= NULL;
	}

	/* �⺻ ȭ�� ��� */
	if (!CreateMenu(IDC_MAIN_BTN_AUTO))	return FALSE;

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
	LOG_MESG(ENG_EDIC::en_uvdi15, L"The program (uvdi15) has started!!!");
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	LOG_MESG(ENG_EDIC::en_uvdi15, L"The program (hddi 6) has started!!!");
#endif


	m_pDlgMotor = new CDlgMotr(this);
	m_pDlgMotor->Create();
	m_pDlgMotor->ShowWindow(SW_HIDE);

	m_pDlgLoginMain = new CDlgLoginMain(this);
	//m_pDlgLoginMain->Create();
	//m_pDlgLoginMain->ShowWindow(SW_HIDE);

	m_pDlgLoginAdmin = new CDlgLoginAdmin(this);
	m_pDlgLoginAdmin->Create();
	m_pDlgLoginAdmin->ShowWindow(SW_HIDE);

	m_pDlgLoginMgr = new CDlgLoginManager(this);
	m_pDlgLoginMgr->Create();
	m_pDlgLoginMgr->ShowWindow(SW_HIDE);

	/* Manager Class �ʱ�ȭ */
	CPowerMeasureMgr::GetInstance()->Initialize();
	CFlushErrorMgr::GetInstance()->Initialize();
	CCamSpecMgr::GetInstance()->Initialize();
	
	CInterLockManager::GetInstance()->LoadInterlcok();
	m_stExpoLog.Init();
	/* Set the title */
	
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
	SetWindowTextW(L"UVDI15 LLS15");
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	SetWindowTextW(L"HDDI6 LLS06");
#endif

	LPG_RJAF pstJob = uvEng_JobRecipe_GetSelectRecipe();
	CUniToChar csCnv;

	CString strReicpe, strExpo, strAlign;
	/* �Ź� ������ ��� */
	//pText	= (CMyStatic *)CWnd::FromHandle(m_hGerb);
	//if (!pstGerb)	pText->SetWindowTextW(L"No recipe selected");
	//else			pText->SetWindowTextW(csCnv.Ansi2Uni(pstGerb->recipe_name));
	//pText->Invalidate(TRUE);
	if (!pstJob)	strReicpe = L"No recipe selected";
	else			strReicpe = csCnv.Ansi2Uni(pstJob->job_name);

	if (!pstJob)	strExpo = L"No recipe selected";
	else			strExpo = csCnv.Ansi2Uni(pstJob->expo_recipe);

	if (!pstJob)	strAlign = L"No recipe selected";
	else			strAlign = csCnv.Ansi2Uni(pstJob->align_recipe);

	m_txt_ctl[eMAIN_TXT_GERB_RECIPE].SetTextToStr(strReicpe.GetBuffer());
	m_txt_ctl[eMAIN_TXT_EXPOSE_RECIPE].SetTextToStr(strExpo.GetBuffer());
	m_txt_ctl[eMAIN_TXT_ALIGN_RECIPE].SetTextToStr(strAlign.GetBuffer());

	strReicpe.ReleaseBuffer();
	strExpo.ReleaseBuffer();
	strAlign.ReleaseBuffer();

	GetDlgItem(IDC_MAIN_BTN_PHILHMI)->ShowWindow(SW_HIDE);


	//�߰�. 



	

	
	GlobalVariables::GetInstance()->GetAlignMotion().DoInitial(uvEng_GetConfig());

	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgMain::OnExitDlg()
{
	m_vUILevel.clear();

	/* Manager Class ���� */
	CRecipeManager::GetInstance()->Destroy();
	CPowerMeasureMgr::GetInstance()->Terminate();
	CFlushErrorMgr::GetInstance()->Terminate();
	CCamSpecMgr::GetInstance()->Terminate();
	CIOManager::GetInstance()->Destroy();
	CLogManager::GetInstance()->Destroy();

	/* �⺻ ���� ������ �޸� ���� */
	if (m_pMainThread)
	{
		m_pMainThread->Stop();
		while (m_pMainThread->IsBusy())	m_pMainThread->Sleep();
		delete m_pMainThread;
		m_pMainThread	= NULL;
	}
	/* �ΰ� �̹��� ��ü ���� */
	if (m_pImgPhil)		delete m_pImgPhil;
	if (m_pImgCustomer)		delete m_pImgCustomer;

	/* �ݵ�� ���̺귯�� ���� ���� �ۼ� */
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
	LOG_MESG(ENG_EDIC::en_uvdi15, L"The program (uvdi15) has ended!!!");
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	LOG_MESG(ENG_EDIC::en_uvdi15, L"The program (hddi6) has ended!!!");
#endif

	/* �ڽ� (�޴�) ���� �޸� ���� */
	DeleteMenu();
#if (USE_ENGINE_LIB)
	/* ���̺귯�� ���� */
	CloseLib();
#endif

	GlobalVariables::GetInstance()->Destroy();

	/* ���α׷� ���� ��� �˸� */
	if (m_pDlgWait)
	{
		if (m_pDlgWait->GetSafeHwnd())	m_pDlgWait->DestroyWindow();
		delete m_pDlgWait;
	}
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgMain::OnPaintDlg(CDC *dc)
{
	/* �ΰ� �̹��� ��� */
	if (m_pic_ctl[eMAIN_PIC_CUSTOMER].GetSafeHwnd())		m_pImgCustomer->DrawImage();
	//if (m_pic_ctl[eMAIN_PIC_PHIL].GetSafeHwnd())	m_pImgPhil->DrawImage();
	/* �ڽ� (�޴�) ���̾�α� ���� */
	if (m_pDlgMenu)	m_pDlgMenu->Invalidate();
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
 desc : UI ��Ʈ�ѷ� ���� ����
 parm : None
 retn : None
*/
void CDlgMain::RegisterUILevel()
{
	/* ----------------------------------------------------------------------------	*/
	/* ���� ������ ����� ��ư�� ���� �α��εǱ� ������ ��Ȱ��ȭ �����̴�.			*/
	/* ���� ������ ���� ���� ��ư�� �ڵ������� Ȱ��ȭ ���°� �ȴ�.					*/
	/* ----------------------------------------------------------------------------	*/


	//	���� �ڵ�.
	m_vUILevel.push_back(ST_UI_LEVEL(eLOGIN_LEVEL_OPERATOR, &m_btn_ctl[eMAIN_BTN_MANUAL]));		//	OPERATOR / ENGINEER / ADMIN ������ �α��� ��, �ش� ��ư�� Ȱ��ȭ�ȴ�.
	m_vUILevel.push_back(ST_UI_LEVEL(eLOGIN_LEVEL_ENGINEER, &m_btn_ctl[eMAIN_BTN_EXPO]));		//	           ENGINEER / ADMIN ������ �α��� ��, �ش� ��ư�� Ȱ��ȭ�ȴ�.
	m_vUILevel.push_back(ST_UI_LEVEL(eLOGIN_LEVEL_ADMIN   , &m_btn_ctl[eMAIN_BTN_MARK]));		//	                      ADMIN ������ �α��� ��, �ش� ��ư�� Ȱ��ȭ�ȴ�.
}

/*
 desc : Library Init
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::InitLib()
{
	return uvEng_Init(this->GetSafeHwnd(), ENG_ERVM::en_cmps_sw);
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
	// by sysandj : Resize UI
	CResizeUI clsResizeUI;
	clsResizeUI.ResizeControl(this, this);
	//MoveWindow(CRect(0, 0, clsResizeUI.GetSizeX(), clsResizeUI.GetSizeY()));
	// by sysandj : Resize UI

	INT32 i = 0;

	/* group box */
	for (i = 0; i < eMAIN_GRP_MAX; i++)
	{
		m_grp_ctl[i].SetLogFont(&g_lf[eFONT_LEVEL2_BOLD]);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_grp_ctl[i]);
		// by sysandj : Resize UI
	}

	for (i = 0; i < eMAIN_PIC_MAX; i++)
	{
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_pic_ctl[i]);
		// by sysandj : Resize UI
	}

	/* button - normal */
	for (i=0; i< eMAIN_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		/* ������ ��ư ������ �˻��ϴ� ��ƾ�� �߰� �� */
//		m_btn_ctl[i].SetBgColor(RGB(0xf4, 0xf4, 0xf4));
		m_btn_ctl[i].SetTextColor(DEF_COLOR_BTN_MENU_NORMAL_TEXT);
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
		// by sysandj : Resize UI
	}

	/* text - normal */
	for (i=0; i< eMAIN_TXT_MAX; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_txt_ctl[i]);
		// by sysandj : Resize UI
	}

	for (i = eMAIN_TXT_SERVER; i < eMAIN_TXT_MAX; i++)
	{
		m_txt_ctl[i].SetBgColor(SEA_GREEN);
		m_txt_ctl[i].SetTextColor(WHITE_);
	}

#if 1
	/* ���� ���� ��� */
	TCHAR tzVer[64] = {NULL};
	CUniToChar csCnv;
	swprintf_s(tzVer, 64, L"%s", csCnv.Ansi2Uni(STR_VERSION_APP_UVDI15));
	m_txt_ctl[eMAIN_TXT_VERSION].SetWindowTextW(tzVer);
#endif
}

/*
 desc : �ΰ� �̹��� ��ü ���� �� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::LoadImages()
{
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	CDlgMesg dlgMesg;

	/* ���� �ΰ� ��ü ���� �� ���� */
	m_pImgCustomer	= new CImgCustomer(m_pic_ctl[eMAIN_PIC_CUSTOMER].GetSafeHwnd());
	ASSERT(m_pImgCustomer);
	if (!m_pImgCustomer->LoadFile())
	{
		dlgMesg.MyDoModal(L"Failed to load the file of customer.jpg", 0x01);
		return FALSE;
	}
	/* PHILOPTICS �ΰ� ��ü ���� �� ���� */
	//m_pImgPhil	= new CImgPhil(m_pic_ctl[eMAIN_PIC_PHIL].GetSafeHwnd());
// 	ASSERT(m_pImgPhil);
// 	if (!m_pImgPhil->LoadFile())
// 	{
// 		dlgMesg.MyDoModal(L"Failed to load the file of phil.jpg", 0x01);
// 		return FALSE;
// 	}

	return TRUE;
}

/*
 desc : Main Thread���� �߻��� �̺�Ʈ ó��
 parm : wparam	- [in]  Event ID
		lparam	- [in]  Event Data
 retn : 0L
*/
LRESULT CDlgMain::OnMsgMainThread(WPARAM wparam, LPARAM lparam)
{
	BOOL bBusy		= LONG(lparam) == 1 ? TRUE : FALSE;
	ENG_BWOK enWork	= ENG_BWOK(wparam);
	UINT64 u64Tick	= GetTickCount64();



	/* �ֱ������� ���� (500 msec ���� ȣ��) */
	if (wparam == MSG_MAIN_THREAD_PERIOD)
	{
		//�� �޼��� ������� 100ms ������ ȣ��ȴ�. 100ms���� ȣ���� �����̸� ���� ���ǰ��ʿ����. 
		UpdateLDSMeasure();/*LSD ���� ���� ��ġ�� �´��� Ȯ��*/

		if (u64Tick > m_u64TickPeriod + 200)
		{
			UpdatePeriod(u64Tick, bBusy);
			m_bMainBusy = bBusy;/*��� ���� ��*/
			m_u64TickPeriod = u64Tick;
			GlobalVariables::GetInstance()->IncCount("mainUpdate");
		}
	}

	/* ���� �ڽ� ȭ���� Expose�� ���, ���� �̺�Ʈ ó�� */
	if (m_pDlgMenu && m_pDlgMenu->GetDlgID() == ENG_CMDI::en_menu_expo)
	{
		switch (enWork)
		{
		case ENG_BWOK::en_mesg_init	: ((CDlgExpo*)m_pDlgMenu)->Invalidate(FALSE);	break;
		case ENG_BWOK::en_mesg_mark	: ((CDlgExpo*)m_pDlgMenu)->DrawMarkData();		break;
		}
	}

	/* ���� �ڽ� ȭ���� Auto�� ���, ���� �̺�Ʈ ó�� */
	if (m_pDlgMenu && m_pDlgMenu->GetDlgID() == ENG_CMDI::en_menu_auto)
	{
		switch (enWork)
		{
		case ENG_BWOK::en_work_init:	RunWorkJob(ENG_BWOK::en_work_init);			break;
		}
	}

	/* ���α׷� ���� �̺�Ʈ �޾Ҵ��� ���� */
	if (enWork == ENG_BWOK::en_app_exit)	PostMessage(WM_CLOSE, 0, 0L);

	return 0L;
}

/*
 desc : Child Dialog���� �߻��� �̺�Ʈ ó��
 parm : wparam	- [in]  ȭ�鿡 ��µ� Dialog ID (������)
		lparam	- [in]  Event Data
 retn : 0L
*/
LRESULT CDlgMain::OnMsgMainChild(WPARAM wparam, LPARAM lparam)
{
	UINT32 u32ChildID	= 0;

	/* �ش� �ڽ� (�޴�) ���� �޸� �Ҵ� */
	switch (ENG_CMDI(wparam))
	{
	case ENG_CMDI::en_menu_auto:	u32ChildID = IDC_MAIN_BTN_AUTO;		break;
	case ENG_CMDI::en_menu_manual:	u32ChildID = IDC_MAIN_BTN_MANUAL;	break;
	case ENG_CMDI::en_menu_expo	:	u32ChildID = IDC_MAIN_BTN_EXPO;	break;
	case ENG_CMDI::en_menu_gerb	:	u32ChildID = IDC_MAIN_BTN_JOB;	break;
	case ENG_CMDI::en_menu_mark	:	u32ChildID = IDC_MAIN_BTN_MARK;	break;
	case ENG_CMDI::en_menu_ctrl	:	u32ChildID = IDC_MAIN_BTN_CTRL;	break;
	case ENG_CMDI::en_menu_logs	:	u32ChildID = IDC_MAIN_BTN_LOGS;	break;
	case ENG_CMDI::en_menu_conf	:	u32ChildID = IDC_MAIN_BTN_CONF;	break;
	case ENG_CMDI::en_menu_calb:	u32ChildID = IDC_MAIN_BTN_CALB;	break;
	case ENG_CMDI::en_menu_philhmi:	u32ChildID = IDC_MAIN_BTN_PHILHMI;	break;
	}

	/* �ڽ� ���̾�α� ID ��� */
	if (u32ChildID)	CreateMenu(u32ChildID);

	return 0L;
}

/*
desc : Export Dialog���� �߻��� Recipe �̺�Ʈ ó��
parm : wparam - [in]  Gerb Recipe String
	   lparam - [in]  Mark Recipe String
retn : 0L
*/
LRESULT CDlgMain::OnMsgMainRecipeUpdate(WPARAM wparam, LPARAM lparam)
{
	CString* pstrRecipe = (CString*)lparam;

	switch (EnMainGrp((int)wparam))
	{
	case eMAIN_GRP_JOB_RECIPE:
		if (NULL != pstrRecipe)
		{
			m_txt_ctl[eMAIN_TXT_GERB_RECIPE].SetTextToStr(pstrRecipe->GetBuffer());
		}
		break;
	case eMAIN_GRP_EXPOSE_RECIPE:
		if (NULL != pstrRecipe)
		{
			m_txt_ctl[eMAIN_TXT_EXPOSE_RECIPE].SetTextToStr(pstrRecipe->GetBuffer());
		}
		break;
	case eMAIN_GRP_ALIGN_RECIPE:
		if (NULL != pstrRecipe)
		{
			m_txt_ctl[eMAIN_TXT_ALIGN_RECIPE].SetTextToStr(pstrRecipe->GetBuffer());
			
		}
		break;
	}
	
	pstrRecipe->ReleaseBuffer();

	return 0L;
}

/*
desc : Export Dialog���� �߻��� Process �̺�Ʈ ó��
parm : wparam - [in]  Work Process Step String
	   lparam - [in]  NULL
retn : 0L
*/
LRESULT CDlgMain::OnMsgMainProcessUpdate(WPARAM wparam, LPARAM lparam)
{
	


	return 0L;
}

/*
desc : PhilHMI�� ���۵� �����͸� PhilDlg�� Log ��� �̺�Ʈ ó��
parm : wparam - [in]  Work Process Step String
	   lparam - [in]  NULL
retn : 0L
*/
LRESULT CDlgMain::OnMsgMainPhilMsg(WPARAM wparam, LPARAM lparam)
{
	TCHAR* szTemp = (TCHAR*)wparam;
	//STG_SLPR_RECV* pstRecv = reinterpret_cast<STG_SLPR_RECV*>(wparam);

	/*DlgPhilhmi ���� �޽��� Ȯ��*/
	if (m_pDlgMenu && m_pDlgMenu->GetDlgID() == ENG_CMDI::en_menu_philhmi)
	{
		((CDlgPhilhmi*)m_pDlgMenu)->AddListBox(szTemp);
	}

	return 0L;
}


// by sysandj : philhmi ���� �޼��� ��û
LRESULT CDlgMain::OnMsgMainPHILHMI(WPARAM wparam, LPARAM lparam)
{
	if (uvEng_Terminated())
		return 0L;

	STG_PP_PACKET_RECV* pstPhil = (STG_PP_PACKET_RECV*)wparam; 
	CUniToChar csCnv1;
	CString strAxis;


	BOOL bBusy = LONG(lparam) == 1 ? TRUE : FALSE;
	BOOL bLoaded = uvCmn_Luria_IsJobNameLoaded();

	if (nullptr == pstPhil)
	{
		return 0L;
	}

	try
	{
		while (uvEng_uvPhilhmi_IsSyncResultLocked())
			Sleep(10);

		if (pstPhil->st_header.nCommand < 0)
		{
			return 0L;
		}
	}
	catch(exception e)
	{
		return 0L;
	}
	

	UINT8 u8Offset = 0xff;
	UINT16 tempCommand = pstPhil->st_header.nCommand;
	TCHAR szTemp[512] = { NULL };

	swprintf_s(szTemp, 512, L"nCommand = %d  ulUniqueID = %d",
		pstPhil->st_header.nCommand, pstPhil->st_header.ulUniqueID);
	LOG_SAVED(ENG_EDIC::en_philhmi, ENG_LNWE::en_job_work, szTemp);

	switch (pstPhil->st_header.nCommand)
	{
	/// <summary>
	/// PHILHMI ��û
	/// </summary>
	case (int)ENG_PHPC::ePHILHMI_C2P_RECIPE_CREATE:
		CRecvPhil::GetInstance()->PhilSendCreateRecipe(pstPhil);
		break;

	case (int)ENG_PHPC::ePHILHMI_C2P_RECIPE_DELETE:
		CRecvPhil::GetInstance()->PhilSendDelectRecipe(pstPhil);
		break;

	case (int)ENG_PHPC::ePHILHMI_C2P_RECIPE_MODIFY:
		CRecvPhil::GetInstance()->PhilSendModifyRecipe(pstPhil);
		break;

	case (int)ENG_PHPC::ePHILHMI_C2P_RECIPE_SELECT:

		updatePender();
		CRecvPhil::GetInstance()->PhilSendSelectRecipe(pstPhil, this, m_bMainBusy);
	break;

	case (int)ENG_PHPC::ePHILHMI_C2P_RECIPE_LIST:
		CRecvPhil::GetInstance()->PhilSendListRecipe(pstPhil);
		break;
	case (int)ENG_PHPC::ePHILHMI_C2P_RECIPE_INFORMATION:
		CRecvPhil::GetInstance()->PhilSendInfoRecipe(pstPhil);
		break;

	case (int)ENG_PHPC::ePHILHMI_C2P_ABS_MOVE:

		updatePender();
		
		/*Recv ���� ���� Ȯ��*/
		if (PhilSendMoveRecvAck(pstPhil, m_bMainBusy))
		{
			for (int i = 0; i < pstPhil->st_c2p_abs_move.usCount; i++)
			{
				PhilSendMove(pstPhil, i);
			}
		}
		break;

	case (int)ENG_PHPC::ePHILHMI_C2P_ABS_MOVE_COMPLETE:
		PhilSendMoveComplete(pstPhil);
		break;

	case (int)ENG_PHPC::ePHILHMI_C2P_REL_MOVE:
		/*Recv ���� ���� Ȯ��*/

		updatePender();


		if (PhilSendMoveRecvAck(pstPhil, m_bMainBusy))
		{
			for (int i = 0; i < pstPhil->st_c2p_rel_move.usCount; i++)
			{
				PhilSendMove(pstPhil, i);
			}
		}
		break;

	case (int)ENG_PHPC::ePHILHMI_C2P_REL_MOVE_COMPLETE:
		PhilSendMoveComplete(pstPhil);
		break;

	case (int)ENG_PHPC::ePHILHMI_C2P_CHAR_MOVE:
		/*Recv ���� ���� Ȯ��*/

		updatePender();


		if (PhilSendMoveRecvAck(pstPhil, m_bMainBusy))
		{
			for (int i = 0; i < pstPhil->st_c2p_char_move.usCount; i++)
			{
				PhilSendMove(pstPhil, i);
			}
		}
		break;


	case (int)ENG_PHPC::ePHILHMI_C2P_CHAR_MOVE_COMPLETE	 :
		PhilSendMoveComplete(pstPhil);
		break;

	case (int)ENG_PHPC::ePHILHMI_C2P_PROCESS_EXECUTE		 :

		updatePender();
		CRecvPhil::GetInstance()->PhilSendProcessExecute(pstPhil, this, m_bMainBusy);
		break; 

	case (int)ENG_PHPC::ePHILHMI_C2P_SUB_PROCESS_EXECUTE	 :
		PhilSendSubProcessExecute(pstPhil);
		break;

	case (int)ENG_PHPC::ePHILHMI_C2P_STATUS_VALUE:
		CRecvPhil::GetInstance()->PhilSendStatusValue(pstPhil); 
		break;

	case (int)ENG_PHPC::ePHILHMI_C2P_MODE_CHANGE			 :
		CRecvPhil::GetInstance()->PhilSendChageMode(pstPhil, this);
		//PhilSendChageMode(pstPhil);
		break;
	case (int)ENG_PHPC::ePHILHMI_C2P_INITIAL_EXECUTE:
		
		updatePender();

		CRecvPhil::GetInstance()->PhilSendInitialExecute(pstPhil, this, m_bMainBusy);
		break;
	case (int)ENG_PHPC::ePHILHMI_C2P_EVENT_STATUS:
		PhilSendEventStatus(pstPhil);
		break;

	case (int)ENG_PHPC::ePHILHMI_C2P_EVENT_NOTIFY:
		PhilSendEventNotify(pstPhil);
		break;

	case (int)ENG_PHPC::ePHILHMI_C2P_TIME_SYNC:
		CRecvPhil::GetInstance()->PhilSendTimeSync(pstPhil, this);
		break;

	case (int)ENG_PHPC::ePHILHMI_C2P_INTERRUPT_STOP:
		CRecvPhil::GetInstance()->PhilSendInterruptStop(pstPhil, this);
		break;

	/// <summary>
	/// ���� ��ɿ� ���� ACK
	/// </summary>
// 	case ePHILHMI_P2C_RECIPE_CREATE			 :
// 		STG_PP_P2C_RCP_CREATE			stCreate;
// 		STG_PP_P2C_RCP_CREATE_ACK		stCreateAck;
// 		stCreate.Reset();
// 		stCreateAck.Reset();
// 		//pstPhil->st_p2c_ack_rcp_create;
// 		uvEng_Philhmi_Send_P2C_RCP_CREATE(stCreate, stCreateAck);
// 		break;
// 	case ePHILHMI_P2C_RECIPE_DELETE			 :
// 		STG_PP_P2C_RCP_DELETE			stDelete;
// 		STG_PP_P2C_RCP_DELETE_ACK		stDeleteAck;
// 		stDelete.Reset();
// 		stDeleteAck.Reset();
// 		uvEng_Philhmi_Send_P2C_RCP_DELETE(stDelete, stDeleteAck);
// 		break;
// 	case ePHILHMI_P2C_RECIPE_MODIFY			 :
// 		STG_PP_P2C_RCP_MODIFY			stModify;
// 		STG_PP_P2C_RCP_MODIFY_ACK		stModifyAck;
// 		stModify.Reset();
// 		stModifyAck.Reset();
// 		uvEng_Philhmi_Send_P2C_RCP_MODIFY(stModify, stModifyAck);
// 		break;
// 	case ePHILHMI_P2C_RECIPE_SELECT:
// 		STG_PP_P2C_RCP_SELECT			stSelect;
// 		STG_PP_P2C_RCP_SELECT_ACK		stSelectAck;
// 		stModify.Reset();
// 		stModifyAck.Reset();
// 		uvEng_Philhmi_Send_P2C_RCP_SELECT(stSelect, stSelectAck);
// 		break;
// 	case ePHILHMI_P2C_IO_STATUS				 :
// 		STG_PP_P2C_IO_STATUS			stIOStatus;
// 		STG_PP_P2C_IO_STATUS_ACK		stIOStatusAck;
// 		stModify.Reset();
// 		stModifyAck.Reset();
// 		uvEng_Philhmi_Send_P2C_IO_STATUS(stIOStatus, stIOStatusAck);
// 		break;
// 	case ePHILHMI_P2C_OUTPUT_ONOFF			 :
// 		STG_PP_P2C_OUTPUT_ONOFF			stOutputOnoff;
// 		STG_PP_P2C_OUTPUT_ONOFF_ACK		stOutputOnoffAck;
// 		uvEng_Philhmi_Send_P2C_OUTPUT_ONOFF(stOutputOnoff, stOutputOnoffAck);
// 		break;
// 	case ePHILHMI_P2C_ABS_MOVE				 :
// 		STG_PP_P2C_ABS_MOVE				stAbsMove;
// 		STG_PP_P2C_ABS_MOVE_ACK			stAbsMoveAck;
// 		uvEng_Philhmi_Send_P2C_ABS_MOVE(stAbsMove, stAbsMoveAck);
// 		break;
// 	case ePHILHMI_P2C_ABS_MOVE_COMPLETE		 :
// 		STG_PP_P2C_ABS_MOVE_COMP		stAbsMoveComp;
// 		STG_PP_P2C_ABS_MOVE_COMP_ACK	stAbsMoveCompAck;
// 		uvEng_Philhmi_Send_P2C_ABS_MOVE_COMP(stAbsMoveComp, stAbsMoveCompAck);
// 		break;
// 	case ePHILHMI_P2C_REL_MOVE				 :
// 		STG_PP_P2C_REL_MOVE				stRelMove;
// 		STG_PP_P2C_REL_MOVE_ACK			stRelMoveAck;
// 		uvEng_Philhmi_Send_P2C_REL_MOVE(stRelMove, stRelMoveAck);
// 		break;
// 	case ePHILHMI_P2C_REL_MOVE_COMPLETE		 :
// 		STG_PP_P2C_REL_MOVE_COMP		stRelMoveComp;
// 		STG_PP_P2C_REL_MOVE_COMP_ACK	stRelMoveCompAck;
// 		uvEng_Philhmi_Send_P2C_REL_MOVE_COMP(stRelMoveComp, stRelMoveCompAck);
// 		break;
// 	case ePHILHMI_P2C_CHAR_MOVE				 :
// 		STG_PP_P2C_CHAR_MOVE			stCharMove;
// 		STG_PP_P2C_CHAR_MOVE_ACK		stCharMoveAck;
// 		uvEng_Philhmi_Send_P2C_CHAR_MOVE(stCharMove, stCharMoveAck);
// 		break;
// 	case ePHILHMI_P2C_CHAR_MOVE_COMPLETE	 :
// 		STG_PP_P2C_CHAR_MOVE_COMP		stCharMoveComp;
// 		STG_PP_P2C_CHAR_MOVE_COMP_ACK	stCharMoveCompAck;
// 		uvEng_Philhmi_Send_P2C_CHAR_MOVE_COMP(stCharMoveComp, stCharMoveCompAck);
// 		break;
// 	case ePHILHMI_P2C_PROCESS_STEP			 :
// 		STG_PP_P2C_PROCESS_STEP			stProcessStep;
// 		STG_PP_P2C_PROCESS_STEP_ACK		stProcessStepAck;
// 		uvEng_Philhmi_Send_P2C_PROCESS_STEP(stProcessStep, stProcessStepAck);
// 		break;
// 	case ePHILHMI_P2C_PROCESS_COMPLETE		 :
// 		STG_PP_P2C_PROCESS_COMP			stProcessComp;
// 		STG_PP_P2C_PROCESS_COMP_ACK		stProcessCompAck;
// 		uvEng_Philhmi_Send_P2C_PROCESS_COMP(stProcessComp, stProcessCompAck);
// 		break;
// 	case ePHILHMI_P2C_SUB_PROCESS_COMPLETE	 :
// 		STG_PP_P2C_SUBPROCESS_COMP		stSubProcessComp;
// 		STG_PP_P2C_SUBPROCESS_COMP_ACK stSubProcessCompAck;
// 		uvEng_Philhmi_Send_P2C_SUBPROCESS_COMP(stSubProcessComp, stSubProcessCompAck);
// 		break;
// 	case ePHILHMI_P2C_EC_MODIFY			 :
// 		STG_PP_P2C_EC_MODIFY			stP2CStatusValue;
// 		STG_PP_P2C_EC_MODIFY_ACK		stStatusValueAck;
// 		uvEng_Philhmi_Send_P2C_EC_MODIFY(stP2CStatusValue, stStatusValueAck);
// 		break;
// 	case ePHILHMI_P2C_INITIAL_COMPLETE		 :
// 		STG_PP_P2C_INITIAL_COMPLETE		stInitialComp;
// 		STG_PP_P2C_INITIAL_COMPLETE_ACK stInitialCompAck;
// 
// 		uvEng_Philhmi_Send_P2C_INITIAL_COMPLETE(stInitialComp, stInitialCompAck);
// 		break;
// 	case ePHILHMI_P2C_ALARM_OCCUR:
// 		STG_PP_P2C_ALARM_OCCUR			stAlarmOccur;
// 		STG_PP_P2C_ALARM_OCCUR_ACK		stAlarmOccurAck;
// 		uvEng_Philhmi_Send_P2C_ALARM_OCCUR(stAlarmOccur, stAlarmOccurAck);
// 		break;
// 	case ePHILHMI_P2C_EVENT_NOTIFY			 :
// 		STG_PP_P2C_EVENT_NOTIFY			stP2CEventNotify;
// 		STG_PP_P2C_EVENT_NOTIFY_ACK		stEventNotifyAck;
// 		uvEng_Philhmi_Send_P2C_EVENT_NOTIFY(stP2CEventNotify, stEventNotifyAck);
// 		break;
// 	case ePHILHMI_P2C_INTERRUPT_STOP		 :
// 		STG_PP_P2C_INTERRUPT_STOP		stP2CInterruptStop;
// 		STG_PP_P2C_INTERRUPT_STOP_ACK	stInterruptStopAck;
// 		uvEng_Philhmi_Send_P2C_INTERRUPT_STOP(stP2CInterruptStop, stInterruptStopAck);
// 		break;
	default:
		// ���ǵ��� ���� ��û
		break;
	}

	if (pstPhil)
	{
		uvEng_Philhmi_DeleteRecvDataFromUniqueID(pstPhil->st_header.ulUniqueID);
	}
	
	/*���� Philhmi �޼��� ����*/
	g_u16PhilCommand = tempCommand;
	
	
	return 0L;
}

LRESULT CDlgMain::OnMsgMainStrobeLamp(WPARAM wparam, LPARAM lparam)
{
	STG_SLPR_RECV*	pstRecv	= reinterpret_cast<STG_SLPR_RECV*>(wparam);
	DWORD			dwCount	= static_cast<DWORD>(lparam);

	uint8_t recvPage;
	uint8_t recvChannel;
	
	uint16_t recvDelayValue;
	//uint8_t recvDelayLowValue;

	uint16_t recvStrobeValue;
	//uint8_t recvStrobeLowValue;
	uint8_t recvMode;
//	uint8_t	recvReserved;
	uint8_t recvLoopCount;
	uint8_t recvLoopDelayValues[4];
	uint8_t recvMode1;
	uint8_t recvMode2;
	/*uint8_t recvDelayHighValues[8];
	uint8_t recvDelayLowValues[8];
	uint8_t recvStrobeHighValues[8];
	uint8_t recvStrobeLowValues[8];*/

	uint16_t recvDelayValues[8];
	uint16_t recvStrobeValues[8];

	const int ITEMS_OF_CHANNEL = 4;
	switch (pstRecv->header.command)
	{
	case eSTROBE_LAMP_CHANNEL_DELAY_CONTROL:	// ���� ä�� Delay ����
		recvPage			= pstRecv->body.channelDelayControl.page;
		recvChannel			= pstRecv->body.channelDelayControl.channel;
		recvDelayValue		= pstRecv->body.channelDelayControl.delayHighValue << 8 | pstRecv->body.channelDelayControl.delayLowValue;
		//recvDelayHighValue	= pstRecv->body.channelDelayControl.delayHighValue;
		//recvDelayLowValue	= pstRecv->body.channelDelayControl.delayLowValue;
		break;

	case eSTROBE_LAMP_CHANNEL_STROBE_CONTROL:	// ���� ä�� Strobe ����
		recvPage			= pstRecv->body.channelStrobeControl.page;
		recvChannel			= pstRecv->body.channelStrobeControl.channel;
		recvStrobeValue = pstRecv->body.channelStrobeControl.strobeHighValue << 8 | pstRecv->body.channelStrobeControl.strobeLowValue;
		GlobalVariables::GetInstance()->IncCount("strobeRecved");

		//recvStrobeHighValue = pstRecv->body.channelStrobeControl.strobeHighValue;
		//recvStrobeLowValue	= pstRecv->body.channelStrobeControl.strobeLowValue;
		break;

	case eSTROBE_LAMP_CHANNEL_WRITE:			// Individual Channel Write
		recvPage			= pstRecv->body.channelWrite.page;
		recvChannel			= pstRecv->body.channelWrite.channel;
		recvDelayValue = pstRecv->body.channelWrite.delayHighValue << 8 | pstRecv->body.channelWrite.delayLowValue;
		recvStrobeValue = pstRecv->body.channelWrite.strobeHighValue << 8 | pstRecv->body.channelWrite.strobeLowValue;
			
		//recvDelayHighValue	= pstRecv->body.channelWrite.delayHighValue;
		//recvDelayLowValue	= pstRecv->body.channelWrite.delayLowValue;
		//recvStrobeHighValue = pstRecv->body.channelWrite.strobeHighValue;
		//recvStrobeLowValue	= pstRecv->body.channelWrite.strobeLowValue;
		break;

	case eSTROBE_LAMP_PAGE_DATA_WRITE:			// Page Data Write
		recvPage = pstRecv->body.pageDataWrite.page;

		for (int i = 0, j = 0; i < DEF_MAX_ARRAY_SIZE; i++)
		{
			recvDelayValues[i] = pstRecv->body.pageDataReadResponse.rawBody[j] << 8 | pstRecv->body.pageDataReadResponse.rawBody[j + 1];
			recvStrobeValues[i] = pstRecv->body.pageDataReadResponse.rawBody[j + 2] << 8 | pstRecv->body.pageDataReadResponse.rawBody[j + 3];
			j += ITEMS_OF_CHANNEL;
		}
		break;

	case eSTROBE_LAMP_PAGE_DATA_READ:			// Page Data Read Request or Response
		recvPage = pstRecv->body.pageDataReadResponse.page;

		for (int i = 0,j=0; i < DEF_MAX_ARRAY_SIZE; i++)
		{
			recvDelayValues[i] = pstRecv->body.pageDataReadResponse.rawBody[j] << 8 | pstRecv->body.pageDataReadResponse.rawBody[j+1];
			recvStrobeValues[i] = pstRecv->body.pageDataReadResponse.rawBody[j+2] << 8 | pstRecv->body.pageDataReadResponse.rawBody[j+3];
			j += ITEMS_OF_CHANNEL;

			/*��Ʈ�κ꿡�� ���� �� �ӽ�����*/
			uvEng_GetConfig()->set_strobe_lamp.u16BufferValue[i] = recvStrobeValues[i];
		}
		GlobalVariables::GetInstance()->IncCount("strobeRecved");

		//memcpy(recvDelayHighValues,  pstRecv->body.pageDataReadResponse.delayHighValue, 8);
		//memcpy(recvDelayLowValues,   pstRecv->body.pageDataReadResponse.delayLowValue, 8);
		//memcpy(recvStrobeHighValues, pstRecv->body.pageDataReadResponse.strobeHighValue, 8);
		//memcpy(recvStrobeLowValues,  pstRecv->body.pageDataReadResponse.strobeLowValue, 8);
		break;

	case eSTROBE_LAMP_PAGE_LOOP_DATA_WRITE:		// Page Loop Data Write
		recvPage			   = pstRecv->body.pageLoopDataWrite.page;
		recvLoopDelayValues[0] = pstRecv->body.pageLoopDataWrite.loopDelay3Value;
		recvLoopDelayValues[1] = pstRecv->body.pageLoopDataWrite.loopDelay2Value;
		recvLoopDelayValues[2] = pstRecv->body.pageLoopDataWrite.loopDelay1Value;
		recvLoopDelayValues[3] = pstRecv->body.pageLoopDataWrite.loopDelay0Value;
		recvLoopCount		   = pstRecv->body.pageLoopDataWrite.loopCount;
		break;

	case eSTROBE_LAMP_PAGE_LOOP_DATA_READ:		// Page Loop Data Read or Response
		recvPage			   = pstRecv->body.pageLoopDataReadResponse.page;
		recvLoopDelayValues[0] = pstRecv->body.pageLoopDataReadResponse.loopDelay3Value;
		recvLoopDelayValues[1] = pstRecv->body.pageLoopDataReadResponse.loopDelay2Value;
		recvLoopDelayValues[2] = pstRecv->body.pageLoopDataReadResponse.loopDelay1Value;
		recvLoopDelayValues[3] = pstRecv->body.pageLoopDataReadResponse.loopDelay0Value;
		recvLoopCount		   = pstRecv->body.pageLoopDataReadResponse.loopCount;
	break;

	case eSTROBE_LAMP_STROBE_MODE:				// Strobe Mode
		recvMode				= pstRecv->body.strobeMode.mode;
		break;

	case eSTROBE_LAMP_TRIGGER_MODE:				// Trigger Mode
		recvMode				= pstRecv->body.triggerMode.mode;
		break;

	case eSTROBE_LAMP_DATA_SAVE:				// Data Save
		//recvReserved			= pstRecv->body.dataSave.reserved;
		break;

	case eSTROBE_LAMP_DATA_LOAD:				// Data Load
		//recvReserved			= pstRecv->body.dataLoad.reserved;
		break;

	case eSTROBE_LAMP_STROBE_TRIGGER_MODE_READ: // Strobe/Trigger Mode Read or Response
		recvMode1				= pstRecv->body.strobeTriggerModeReadResponse.mode1;
		recvMode2				= pstRecv->body.strobeTriggerModeReadResponse.mode2;
		break;

	default:
		AfxMessageBox(L"Unknown command"); // static_cast<int>(pstRecv->header.command)
	}

	return 0L;
}

LRESULT CDlgMain::OnMsgMainRecipeCreate(WPARAM wparam, LPARAM lparam)
{
	return 0L;
}

LRESULT CDlgMain::OnMsgMainRecipeDelete(WPARAM wparam, LPARAM lparam)
{
	return 0L;
}

LRESULT CDlgMain::OnMsgMainRecipeChange(WPARAM wparam, LPARAM lparam)
{
	return 0L;
}

// by sysandj : philhmi ���� �޼��� ��û

/*
 desc : �θ� �����忡 ���� �ֱ������� ȣ���
 parm : tick	- [in]  ���� CPU �ð�
		busy	- [in]  Busy ����
 retn : None
*/
VOID CDlgMain::UpdatePeriod(UINT64 tick, BOOL busy)
{
	/* ���� ��µ� �ڽ� (�޴�) ���� ���� ȣ�� */
	if (m_pDlgMenu && m_pDlgMenu->GetSafeHwnd())
	{
		UpdateControl(tick, busy);
		m_pDlgMenu->UpdatePeriod(tick, busy/*m_pMainThread->IsBusyWorkJob()*/);
	}
	/* �ֱ������� ��� ���� ��û */
	UpdateReqState(tick, busy/*m_pMainThread->IsBusyWorkJob()*/);
	/* ���� ���� �޽��� ��� */
	UpdateErrorMessage(tick);

	UpdateCommState();
	
	/*WorkStage�� SafePos ��ġ �ߴ��� Ȯ��*/
	UpdateSafePosCheck();
	/*PhilHMI���� ��û�� Move ���� �Ϸ� Ȯ��*/
	UpdatePhilState();
}

/*
 desc : ���� ���õ� �ڽ� (�޴�) ���̾�α� ID�� ���õ� ��ư ���� ����
 parm : tick	- [in]  ���� CPU �ð�
		busy	- [in]  Busy ����
 retn : None
*/
VOID CDlgMain::UpdateControl(UINT64 tick, BOOL busy)
{
	UINT8 i	= 0x00;
	BOOL bUpdate	= TRUE, bLoadedJob = FALSE/*, bExposeDlg = FALSE*/;

	/* If the expose screen is currently displayed... */
	/*bExposeDlg	= m_enDlgID == ENG_CMDI::en_menu_expo;*/
	/* If a gerber is currently being loaded... */
	bLoadedJob	= m_pMainThread->GetWorkJobID() == ENG_BWOK::en_gerb_load;

	if (!bLoadedJob && uvEng_IsEngineAlarm())
	{
		if (!bUpdate)
		{
			m_txt_ctl[eMAIN_TXT_STATUS].SetWindowTextW(L"ALARM");
			m_txt_ctl[eMAIN_TXT_STATUS].SetBgColor(RGB(255, 0, 0));
			m_txt_ctl[eMAIN_TXT_STATUS].SetTextColor(g_clrBtnTextColor);
			m_txt_ctl[eMAIN_TXT_STATUS].Invalidate(TRUE);
		}
		/* �汤�� - �˶� ���� */
	}
	else if (!bLoadedJob && uvEng_IsEngineWarn())
	{
		if (!bUpdate)
		{
			m_txt_ctl[eMAIN_TXT_STATUS].SetWindowTextW(L"WARNING");
			m_txt_ctl[eMAIN_TXT_STATUS].SetBgColor(RGB(255, 255, 128));
			m_txt_ctl[eMAIN_TXT_STATUS].SetTextColor(g_clrBtnTextColor);
			m_txt_ctl[eMAIN_TXT_STATUS].Invalidate(TRUE);
		}
		/* �汤�� - ��� ���� */
	}
	else
	{
		
		if (!bUpdate)
		{
			m_txt_ctl[eMAIN_TXT_STATUS].SetWindowTextW(L"Normal");
			m_txt_ctl[eMAIN_TXT_STATUS].SetBgColor(RGB(255, 255, 128));
			m_txt_ctl[eMAIN_TXT_STATUS].Invalidate(TRUE);
		}
		/* �汤�� - ���� ���� */
	}

	/* ���� ���� ���� �޴� ���̾�α� ID�� �ƴϸ�, ���� ��ư���� �˶� ���ο� ���� ���� ǥ�� */
	if (m_enDlgID != m_pDlgMenu->GetDlgID())
	{
		for (i=0; i< eMAIN_BTN_MAX; i++)
		{
			m_btn_ctl[i].SetBgColor( DEF_COLOR_BTN_MENU_NORMAL);
			m_btn_ctl[i].SetTextColor(DEF_COLOR_BTN_MENU_NORMAL_TEXT);
			//m_btn_ctl[i].SetBgColor(RGB(0xf0, 0xf0, 0xf0));
			m_btn_ctl[i].Invalidate(TRUE);
		}

		/* ���� ���õ� �޴� (�ڽ�) ���̾�α� ID ���� */
		m_enDlgID	= m_pDlgMenu->GetDlgID();
		/* ���� ���õ� ��ư�� ���� ���� �ٸ��� ��� */
		i = 0xff;
		switch (m_enDlgID)
		{
		case ENG_CMDI::en_menu_auto: i = eMAIN_BTN_AUTO;	break;
		case ENG_CMDI::en_menu_manual : i = eMAIN_BTN_MANUAL;	break;
		case ENG_CMDI::en_menu_expo: i = eMAIN_BTN_EXPO;	break;
		case ENG_CMDI::en_menu_gerb	: i = eMAIN_BTN_GERB;	break;
		case ENG_CMDI::en_menu_mark	: i = eMAIN_BTN_MARK;	break;
		case ENG_CMDI::en_menu_ctrl	: i = eMAIN_BTN_CTRL;	break;
		case ENG_CMDI::en_menu_logs	: i = eMAIN_BTN_LOGS;	break;
//		case ENG_CMDI::en_menu_stat	: i = eMAIN_BTN_STAT;	break;
		case ENG_CMDI::en_menu_conf	: i = eMAIN_BTN_CONF;	break;
		case ENG_CMDI::en_menu_calb : i = eMAIN_BTN_CALB;	break;
		case ENG_CMDI::en_menu_philhmi: i = eMAIN_BTN_PHILHMI;	break;
//		case ENG_CMDI::en_menu_test	: i = 0x08;	break;
		}
		if (i != 0xff)
		{
			//m_btn_ctl[i].SetBgColor(RGB(0x80, 0xff, 0x00));
			m_btn_ctl[i].SetBgColor(DEF_COLOR_BTN_MENU_SELECT);
			m_btn_ctl[i].Invalidate(TRUE);
		}
	}
//#ifndef _DEBUG
//	/* ���� �۾� ���̰ų�, ��� ���� �ý����� ������� �ʾҰų� ... */
//	if (busy/*m_pMainThread->IsBusyWorkJob()*/ || uvEng_IsEngineAlarm())
//	{
//		for (i= eMAIN_BTN_EXPO; i< eMAIN_BTN_MAX; i++)
//		{
//			if (i == eMAIN_BTN_CONF)	continue;
//			if (i != eMAIN_BTN_EXPO /* expo */ && i != eMAIN_BTN_STOP /* stop */ && i != eMAIN_BTN_CTRL/* ctrl */)
//			{
//				if (m_btn_ctl[i].IsWindowEnabled())	m_btn_ctl[i].EnableWindow(FALSE);
//			}
//			else
//			{
//				if (!m_btn_ctl[i].IsWindowEnabled())	m_btn_ctl[i].EnableWindow(TRUE);
//			}
//		}
//	}
//	else
//	{
//		for (i= eMAIN_BTN_EXPO; i< eMAIN_BTN_CONF; i++)
//		{
//			if (!m_btn_ctl[i].IsWindowEnabled())	m_btn_ctl[i].EnableWindow(TRUE);
//		}
//
//		/* �뱤 ȭ���� �������� ���ο� ����, ������ ��ư�� Enable Disable ó�� */
//		BOOL bEnable = ENG_CMDI::en_menu_expo == m_enDlgID;
//		if (m_btn_ctl[eMAIN_BTN_STOP].IsWindowEnabled() != bEnable)	m_btn_ctl[eMAIN_BTN_STOP].EnableWindow(bEnable);
//	}
//#endif
}

/*
 desc : �ֱ������� ���� ���� ��û
 parm : is_busy	- [in]  ���� Busy �������� ����
 retn : None
*/
VOID CDlgMain::UpdateReqState(UINT64 tick, BOOL is_busy)
{
	if (0x01 == uvEng_ShMem_GetLuria()->machine.z_drive_type)
	{
		/* Update PH Z �� ��ġ ���� ��û */
		if (!is_busy && (m_u64ReqCmdTime + 1000 < tick))
		{
			m_u64ReqCmdTime = tick;
			switch (m_u8NextReqCmd)
			{
			case 0x00	: uvEng_Luria_ReqGetMotorAbsPositionAll();	break;
			case 0x01	: uvEng_Luria_ReqGetMotorStateAll();		break;
			}
			if (0x01 == m_u8NextReqCmd)	m_u8NextReqCmd	= 0x00;
			else						m_u8NextReqCmd++;
		}
	}
}

/*
 desc : ��ư �̺�Ʈ
 parm : None
 retn : None
*/
VOID CDlgMain::OnBtnClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_MAIN_BTN_EXIT	:	ExitApp();	break;
	case IDC_MAIN_BTN_STOP	:	WorkStop();	break;
	case IDC_MAIN_BTN_LOGIN	:	::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), eMSG_MAIN_LOGIN_CONSOLE, NULL, NULL); /*�α���*/	break;
	case IDC_MAIN_BTN_MODE	:	/*AUTO���*/	break;
	case IDC_MAIN_BTN_AUTO	:
	case IDC_MAIN_BTN_MANUAL:
	case IDC_MAIN_BTN_EXPO	:
	case IDC_MAIN_BTN_JOB	:
	case IDC_MAIN_BTN_MARK	:
	case IDC_MAIN_BTN_CTRL	:
	case IDC_MAIN_BTN_LOGS	:	
	case IDC_MAIN_BTN_CONF	:	
	case IDC_MAIN_BTN_CALB	:	
	case IDC_MAIN_BTN_PHILHMI:
		CreateMenu(id);			break;
	case IDC_MAIN_BTN_RESET	:	ResetErrorMessage();	break;
	}

	if (id == IDC_MAIN_BTN_MARK) {
		uvEng_Camera_SetDispType(DISP_TYPE_MARK);
	}
	else if(id == IDC_MAIN_BTN_MANUAL){
		uvEng_Camera_SetDispType(DISP_TYPE_MMPM);
	}
	else {
		uvEng_Camera_SetDispType(99);
	}
}

/*
 desc : �۾� (���) �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgMain::WorkInit()
{
	CDlgMesg dlgMesg;

	/* ���� ���� �޽��� �ʱ�ȭ */
	ResetErrorMessage();

	/* �Ź� ������ ��ϵǾ� �ִ��� ���� Ȯ�� */
	if (uvCmn_Luria_IsLoadedJobName())
	{
		dlgMesg.MyDoModal(L"There is recipe loaded.\nUnload recipes !", 0x01);
		return;
	}
	if (IDOK != dlgMesg.MyDoModal(L"Do you want to reset your equipment ?"))	return;
	/* ��� �ʱ�ȭ ���� ��� ���� */
	if (IDOK == dlgMesg.MyDoModal(L"Only Homing (Yes) or Full Inited (No)", 0x02))
	{
		m_pMainThread->RunWorkJob(ENG_BWOK::en_work_home);
	}
	else
	{
		m_pMainThread->RunWorkJob(ENG_BWOK::en_work_init);
	}
}

/*
 desc : �۾� (���) ���� ����
 parm : None
 retn : None
*/
VOID CDlgMain::WorkStop()
{
	CDlgMesg dlgMesg;

	/* ���� ���� �޽��� �ʱ�ȭ */
	ResetErrorMessage();

	/* ���� �۾� ���� ���, �ѹ� �� ����� ��ȭ ���� ��� */
	if (m_pMainThread->IsBusyWorkJob())
	{
		if (IDOK != dlgMesg.MyDoModal(L"Do you want to stop the current work ?"))	return;
	}

	/* ���� �������� ���� �۾� ��û�� �����.... Printing �μ� ���� Ȯ������ �ʰ�, �ٷ� ���� ��Ŵ */
	if (m_pMainThread->GetWorkJobID() == ENG_BWOK::en_work_stop)
	{
		UINT8 u8Forced	= 0x01;
		m_pMainThread->RunWorkJob(ENG_BWOK::en_work_stop, PUINT64(&u8Forced));
	}
	else
	{
		m_pMainThread->RunWorkJob(ENG_BWOK::en_work_stop);
	}
}

/*
 desc : MainThread�� RunWorkJob �Լ� ȣ��
 parm : job_id	- [in]  Job �ĺ� ID �� (ENG_BWOK)
		data	- [in]  Job �� ������ ��
 retn : TRUE or FALSE
*/
BOOL CDlgMain::RunWorkJob(ENG_BWOK job_id, PUINT64 data)
{
	CDlgMesg dlgMesg;

	if (!m_pMainThread)	return FALSE;
#if 1
	if (!uvEng_GetConfig()->IsRunDemo())
	{
		/* ���� �α׿� ���� �޽����� �����ϸ� �� �̻� �۾� �� �� ������ ó�� */
		if (job_id != ENG_BWOK::en_work_stop &&
			job_id != ENG_BWOK::en_work_home &&
			job_id != ENG_BWOK::en_work_init)
		{
#if 0

			if (uvCmn_Luria_IsLastError() || uvCmn_Logs_IsErrorMesg())
			{
				dlgMesg.MyDoModal(L"An error message exists.\nCheck the error before proceeding.", 0x01);
				return FALSE;
			}
#endif
		}
	}
#else
	/* ���� ���� �޽��� �ʱ�ȭ */
	ResetErrorMessage();
#endif
	/* �۾� ���� */
	return m_pMainThread->RunWorkJob(job_id, data);
}

BOOL CDlgMain::IsBusyWorkJob()
{
	return m_pMainThread->IsBusyWorkJob();
}

ENG_BWOK CDlgMain::GetWorkJobID()
{
	
	return m_pMainThread->GetWorkJobID();
}

/*
 desc : ���� ����� �ڽ� ���� �޸� ����
 parm : None
 retn : None
*/
VOID CDlgMain::DeleteMenu()
{
	if (!m_pDlgMenu)	return;

	try
	{
		if (m_pDlgMenu->GetSafeHwnd())
			m_pDlgMenu->DestroyWindow();
		
	}
	catch (CMemoryException* e)
	{
		
	}
	catch (CFileException* e)
	{
	}
	catch (CException* e)
	{
	}
	delete m_pDlgMenu;
	m_pDlgMenu = NULL;
}

/*
 desc : �ڽ� �޴� ȭ�� ����
 parm : id	- [in]  ��ư ��Ʈ�� ID
 retn : TRUE or FALSE
*/
BOOL CDlgMain::CreateMenu(UINT32 id)
{
	UINT32 u32DlgID	= 0;

	if (m_pDlgMenu && m_pDlgMenu->GetDlgID() == ENG_CMDI::en_menu_logs)
	{
		//((CDlgLogs*)m_pDlgMenu)->SetStop();

		if (eLOGS_STEP_INIT != ((CDlgLogs*)m_pDlgMenu)->GetLogStep())
		{
			AfxMessageBox(L"���Ͽ��� ������ �ҷ����� ���Դϴ�.");
			return FALSE;
		}
	}

	/* ���� ����� �ڽ� ������ �ִٸ� �޸� ���� */
	DeleteMenu();

	/* �����ϰ��� �ϴ� ���� (�޴� ���̾�α�) ID ���� */
	u32DlgID	= IDD_AUTO + (id - IDC_MAIN_BTN_AUTO);
	/* �ش� �ڽ� (�޴�) ���� �޸� �Ҵ� */
	switch (id)
	{
	case IDC_MAIN_BTN_AUTO		: m_pDlgMenu	= new CDlgAuto		(u32DlgID, this);	break;
	case IDC_MAIN_BTN_MANUAL	: m_pDlgMenu	= new CDlgManual	(u32DlgID, this);	break;
	case IDC_MAIN_BTN_EXPO		: m_pDlgMenu	= new CDlgExpo		(u32DlgID, this);	break;
	case IDC_MAIN_BTN_JOB		: m_pDlgMenu	= new CDlgJob		(u32DlgID, this);	break;
	case IDC_MAIN_BTN_MARK		: m_pDlgMenu	= new CDlgMark		(u32DlgID, this);	break;
	case IDC_MAIN_BTN_CTRL		: m_pDlgMenu	= new CDlgCtrl		(u32DlgID, this);	break;
	case IDC_MAIN_BTN_LOGS		: m_pDlgMenu	= new CDlgLogs		(u32DlgID, this);	break;
	case IDC_MAIN_BTN_CONF		: m_pDlgMenu	= new CDlgConf		(u32DlgID, this);	break;
	case IDC_MAIN_BTN_CALB		: m_pDlgMenu	= new CDlgCalb		(u32DlgID, this);	break;
	case IDC_MAIN_BTN_PHILHMI	: m_pDlgMenu	= new CDlgPhilhmi	(u32DlgID, this);	break;
	}

	/* �ڽ� (�޴�) ���� ���� */
	if (!m_pDlgMenu->Create(m_pic_ctl[eMAIN_PIC_MENU].GetSafeHwnd()))
	{
		delete m_pDlgMenu;
		m_pDlgMenu	= NULL;
		return FALSE;
	}

	return TRUE;
}

/*
 desc : ���� �߻��� ���� �޽��� ���
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateErrorMessage(UINT64 tick)
{
	PTCHAR ptzMesg	= NULL;

	/* ���� �޽��� ��� �ð��� �Ǿ����� Ȯ�� */
#ifdef _DEBUG
	if (tick < m_u64MsgLogTime + 2000)	return;
#else
	if (tick < m_u64MsgLogTime + 5000)	return;
#endif
	/* ���� ����� �ð� ���� */
	m_u64MsgLogTime	= tick;

	/* ���� �ε��� ��ġ���� �߻��� ���� �޽��� ��� */
	ptzMesg	= uvCmn_Logs_GetErrorMesg(m_u8MsgLogNext);
	/* ������ ���� �����, �ϴ� �ٽ� ó�� �޽��� �о�;� �� */
	if (!ptzMesg && m_u8MsgLogNext)
	{
		m_u8MsgLogNext	= 0x00;
		ptzMesg	= uvCmn_Logs_GetErrorMesg(m_u8MsgLogNext);
	}
	/* ��Ʈ�� ���� ��Ȱ��ȭ */
	m_txt_ctl[eMAIN_TXT_MESSAGE].SetRedraw(FALSE);
	/* ��Ʈ�� ������ ��輱 �� ���� */
	m_txt_ctl[eMAIN_TXT_MESSAGE].SetBgColor(RGB(253, 253, 254));
	m_txt_ctl[eMAIN_TXT_MESSAGE].SetBorderColor(RGB(192, 192, 192));
	if (!ptzMesg)
	{
		m_txt_ctl[eMAIN_TXT_MESSAGE].SetTextColor(RGB(128, 128, 128));
		m_txt_ctl[eMAIN_TXT_MESSAGE].SetWindowTextW(L"Ready");
		if (m_u8MsgLogNext != 0x00)	m_u8MsgLogNext	= 0x00;
	}
	else
	{
		m_txt_ctl[eMAIN_TXT_MESSAGE].SetTextColor(RGB(255, 32, 0));
		m_txt_ctl[eMAIN_TXT_MESSAGE].SetWindowTextW(ptzMesg);
		m_u8MsgLogNext++;
	}
	/* ��Ʈ�� ���� Ȱ��ȭ */
	m_txt_ctl[eMAIN_TXT_MESSAGE].SetRedraw(TRUE);
	m_txt_ctl[eMAIN_TXT_MESSAGE].Invalidate(FALSE);
}

/*
 desc : ���� �߻��� ���� �޽��� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgMain::ResetErrorMessage()
{
	/* Luria Error Code ���� */
	uvCmn_Luria_ResetLastErrorStatus();
	/* ���� �޽��� �ʱ�ȭ */
	uvCmn_Logs_ResetErrorMesg();
	m_u8MsgLogNext	= 0x00;	/* �ݵ�� 0x00���� �ʱ�ȭ */

	/* ��Ʈ�� ���� ��Ȱ��ȭ */
	m_txt_ctl[eMAIN_TXT_MESSAGE].SetRedraw(FALSE);
	m_txt_ctl[eMAIN_TXT_MESSAGE].SetBgColor(RGB(253, 253, 254));
	m_txt_ctl[eMAIN_TXT_MESSAGE].SetBorderColor(RGB(192, 192, 192));
	m_txt_ctl[eMAIN_TXT_MESSAGE].SetTextColor(RGB(128, 128, 128));
	m_txt_ctl[eMAIN_TXT_MESSAGE].SetWindowTextW(L"Ready");
	/* ��Ʈ�� ���� Ȱ��ȭ */
	m_txt_ctl[eMAIN_TXT_MESSAGE].SetRedraw(TRUE);
	m_txt_ctl[eMAIN_TXT_MESSAGE].Invalidate(FALSE);

}

VOID CDlgMain::UpdateCommState()
{
	int nConnectionState = 0;
	for (int i = eMAIN_TXT_SERVER; i < eMAIN_TXT_MAX; i++)
	{
		nConnectionState = 0;
		switch (i)
		{
		case eMAIN_TXT_SERVER:
			nConnectionState += uvCmn_Luria_IsConnected();
			nConnectionState += uvEng_Luria_IsServiceInited();
			break;
		case eMAIN_TXT_MOTION:
			nConnectionState += uvCmn_MC2_IsConnected();
			if (nConnectionState) nConnectionState++;
			break;
		case eMAIN_TXT_TRIGGER:
			nConnectionState += uvCmn_Mvenc_IsConnected();
			if (nConnectionState) nConnectionState++;
			break;
		case eMAIN_TXT_PHILHMI:
			nConnectionState += uvEng_Philhmi_IsConnected();
			if (nConnectionState) nConnectionState++;
			break;
		case eMAIN_TXT_CAM1:
			nConnectionState += uvCmn_Camera_IsConnectedACam(1);
			if (nConnectionState) nConnectionState++;
			break;
		case eMAIN_TXT_CAM2:
			nConnectionState += uvCmn_Camera_IsConnectedACam(2);
			if (nConnectionState) nConnectionState++;
			break;
		case eMAIN_TXT_GENTEC:
			nConnectionState += uvEng_Gentec_IsConnected();
			if (nConnectionState) nConnectionState++;
			break;
		case eMAIN_TXT_STROBE_LAMP:
			nConnectionState += uvEng_StrobeLamp_IsConnected();
			if (nConnectionState) nConnectionState++;
			break;
		}
		

		switch (nConnectionState)
		{
		case 0: // Failed
			m_txt_ctl[i].SetBgColor(TOMATO);
			break;
		case 1: // Warrning
			m_txt_ctl[i].SetBgColor(ORANGE);
			break;
		case 2: // Success
			m_txt_ctl[i].SetBgColor(SEA_GREEN);
			break;
		}

		m_txt_ctl[i].Invalidate();
	}
}

/*
 desc : LDS ���� ����
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateLDSMeasure()
{
	double dCurY=0, dValue=0;
	
	dCurY = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);

	if (uvEng_GetConfig()->measure_flat.bThieckOnOff == false)
		return;
	
	if (dCurY < uvEng_GetConfig()->measure_flat.dStartYPos ||
		dCurY > uvEng_GetConfig()->measure_flat.dEndYPos)
	{
		if (uvEng_GetConfig()->measure_flat.GetThickMeasure() != 0)
			uvEng_GetConfig()->measure_flat.bThieckOnOff = FALSE;
		
		return;
	}
	
	TCHAR tzMsg[256] = { NULL };

	dValue = uvEng_KeyenceLDS_Measure(0);
	
	if (dValue < -5 || dValue > 5)
		return;

	uvEng_GetConfig()->measure_flat.SetThickMeasureResult(((dValue * 10000) / 10000));

	swprintf_s(tzMsg, 256, L"Stage_y=%.4ff LDS=%.4f", dCurY, dValue);
	LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);

	/*Log ���*/
	m_strLog.Format(tzMsg);
	txtWrite(m_strLog);

	
}

/*
 desc : SaferPos ��ġ Ȯ�� 
 parm : None
 retn : None
*/
VOID CDlgMain::UpdateSafePosCheck()
{
	double dCurX, dCurY;
	double dMoveX, dMoveY;
	TCHAR tzMsg[256] = { NULL };

	dCurX = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);
	dCurY = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);

	dMoveX = abs(dCurX - uvEng_GetConfig()->set_align.table_unloader_xy[0][0]);
	dMoveY = abs(dCurY - uvEng_GetConfig()->set_align.table_unloader_xy[0][1]);

	if (dMoveX < 0.01 && dMoveY < 0.01)
	{
		/*���� �̺�Ʈ�� SafePos �Ǿ� ������ PhilHMI ���� �ʿ� ����*/
		if (m_stPhilStatus.safe_pos == FALSE)
		{
			m_stPhilStatus.safe_pos = TRUE;

			swprintf_s(tzMsg, 256, L"Safe Pos Stage_x=%.4ff Stage_y=%.4f \n", dCurX, dCurY);
			LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);

			STG_PP_P2C_EVENT_NOTIFY			stP2CEventNotify;
			STG_PP_P2C_EVENT_NOTIFY_ACK		stEventNotifyAck;

			stP2CEventNotify.Reset();
			stEventNotifyAck.Reset();

			sprintf_s(stP2CEventNotify.szEventName, DEF_MAX_EVENT_NAME_LENGTH, "SAFEPOS");
			stP2CEventNotify.bEventFlag = m_stPhilStatus.safe_pos;


			uvEng_Philhmi_Send_P2C_EVENT_NOTIFY(stP2CEventNotify, stEventNotifyAck);
		}

	}
	else
	{
		/*���� �̺�Ʈ�� SafePos �Ǿ� ������ PhilHMI ���� �ʿ� ����*/
		if (m_stPhilStatus.safe_pos == TRUE)
		{
			m_stPhilStatus.safe_pos = FALSE;

			swprintf_s(tzMsg, 256, L"NO Safe Pos Stage_x=%.4ff Stage_y=%.4f \n", dCurX, dCurY);
			LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);

			STG_PP_P2C_EVENT_NOTIFY			stP2CEventNotify;
			STG_PP_P2C_EVENT_NOTIFY_ACK		stEventNotifyAck;

			stP2CEventNotify.Reset();
			stEventNotifyAck.Reset();

			sprintf_s(stP2CEventNotify.szEventName, DEF_MAX_EVENT_NAME_LENGTH, "SAFEPOS");
			stP2CEventNotify.bEventFlag = m_stPhilStatus.safe_pos;


			uvEng_Philhmi_Send_P2C_EVENT_NOTIFY(stP2CEventNotify, stEventNotifyAck);
		}
	}

	/*Recipe Comp ���� �Ǿ� �ִٸ� ������ Ÿ�� ����*/
	if (m_stPhilStatus.recipe_comp == TRUE)
	{
		m_stPhilStatus.recipe_comp = uvCmn_Luria_IsJobNameLoaded();
	}
}

VOID CDlgMain::txtWrite(CString msg)
{
	CStdioFile	imsifile;

	TCHAR tzFile[_MAX_PATH] = { NULL };
	SYSTEMTIME stTm = { NULL };

	GetLocalTime(&stTm);

	TCHAR tzPath[_MAX_PATH];
	swprintf_s(tzPath, _MAX_PATH, L"%s\\logs\\expo\\%04d-%02d-%02d LDS_Thick.dat", g_tzWorkDir, stTm.wYear, stTm.wMonth, stTm.wDay);


	imsifile.Open(tzPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite);

	imsifile.SeekToEnd();

	swprintf_s(tzFile, L"[%04d-%02d-%02d %02d:%02d:%02d.%03d]  ",
		(UINT16)stTm.wYear, (UINT16)stTm.wMonth, (UINT16)stTm.wDay,
		(UINT16)stTm.wHour, (UINT16)stTm.wMinute, (UINT16)stTm.wSecond, (UINT16)stTm.wMilliseconds);

	imsifile.WriteString(tzFile);
	imsifile.WriteString(msg);

	imsifile.Close();

}

/*
 desc : Philhmi ���� ����
 parm : None
 retn : None
*/
VOID CDlgMain::UpdatePhilState()
{
	DOUBLE MovePos = 0;

	/*Phil���� ���� ��� �� �̵� �Ϸ� ��ȣ*/
	if (m_stPhilStatus.phil_move > en_menu_phil_move_none)
	{
		m_u64DelayTimeHMI = GetTickCount64() - m_u64StartTimeHMI;
		/*��� �ð��� 60�� ������ ���� ó��*/
		if (m_u64DelayTimeHMI > 60000)
		{
			if (m_stPhilStatus.phil_move == en_menu_phil_move_abs)
			{
				STG_PP_P2C_ABS_MOVE_COMP stAbsMove;
				STG_PP_P2C_ABS_MOVE_COMP_ACK stAbsMoveAck;
				stAbsMove.Reset();
				stAbsMoveAck.Reset();

				stAbsMove.usErrorCode = ePHILHMI_ERR_MOTOR_RUN_TIMEOUT;
				stAbsMove.ulUniqueID = m_stPhilStatus.unique_id;
				uvEng_Philhmi_Send_P2C_ABS_MOVE_COMP(stAbsMove, stAbsMoveAck);
			}
			else if (m_stPhilStatus.phil_move == en_menu_phil_move_rel)
			{
				STG_PP_P2C_REL_MOVE_COMP stRelMove;
				STG_PP_P2C_REL_MOVE_COMP_ACK stRelMoveAck;
				stRelMove.Reset();
				stRelMoveAck.Reset();

				stRelMove.usErrorCode = ePHILHMI_ERR_MOTOR_RUN_TIMEOUT;
				stRelMove.ulUniqueID = m_stPhilStatus.unique_id;
				uvEng_Philhmi_Send_P2C_REL_MOVE_COMP(stRelMove, stRelMoveAck);
			}
			else if (m_stPhilStatus.phil_move == en_menu_phil_move_char)
			{
				STG_PP_P2C_CHAR_MOVE_COMP stCharMove;
				STG_PP_P2C_CHAR_MOVE_COMP_ACK stCharMoveAck;
				stCharMove.Reset();
				stCharMoveAck.Reset();

				stCharMove.usErrorCode = ePHILHMI_ERR_MOTOR_RUN_TIMEOUT;
				stCharMove.ulUniqueID = m_stPhilStatus.unique_id;
				uvEng_Philhmi_Send_P2C_CHAR_MOVE_COMP(stCharMove, stCharMoveAck);
			}

			m_stPhilStatus.phil_move = en_menu_phil_move_none;
		}
		else
		{
			MovePos = uvCmn_MC2_GetDrvAbsPos((ENG_MMDI)m_stPhilStatus.drive_id);
			if (0.01 > fabs(MovePos - m_stPhilStatus.move_dist))
			{
				if (m_stPhilStatus.phil_move == en_menu_phil_move_abs)
				{
					STG_PP_P2C_ABS_MOVE_COMP stAbsMove;
					STG_PP_P2C_ABS_MOVE_COMP_ACK stAbsMoveAck;
					stAbsMove.Reset();
					stAbsMoveAck.Reset();

					stAbsMove.ulUniqueID = m_stPhilStatus.unique_id;
					uvEng_Philhmi_Send_P2C_ABS_MOVE_COMP(stAbsMove, stAbsMoveAck);
				}
				else if (m_stPhilStatus.phil_move == en_menu_phil_move_rel)
				{
					STG_PP_P2C_REL_MOVE_COMP stRelMove;
					STG_PP_P2C_REL_MOVE_COMP_ACK stRelMoveAck;
					stRelMove.Reset();
					stRelMoveAck.Reset();

					stRelMove.ulUniqueID = m_stPhilStatus.unique_id;
					uvEng_Philhmi_Send_P2C_REL_MOVE_COMP(stRelMove, stRelMoveAck);
				}
				else if (m_stPhilStatus.phil_move == en_menu_phil_move_char)
				{
					STG_PP_P2C_CHAR_MOVE_COMP stCharMove;
					STG_PP_P2C_CHAR_MOVE_COMP_ACK stCharMoveAck;
					stCharMove.Reset();
					stCharMoveAck.Reset();

					stCharMove.ulUniqueID = m_stPhilStatus.unique_id;
					uvEng_Philhmi_Send_P2C_CHAR_MOVE_COMP(stCharMove, stCharMoveAck);
				}
				m_stPhilStatus.phil_move = en_menu_phil_move_none;
			}
		}

	}

}


/*
 desc : ���α׷� ����
 parm : None
 retn : None
*/
VOID CDlgMain::ExitApp()
{
#ifndef _DEBUG
	CDlgMesg dlgMesg;
	if (IDOK != dlgMesg.MyDoModal(L"Do you want to terminate the program ?"))	return;
#endif

	if (m_pDlgMotor->GetSafeHwnd())	m_pDlgMotor->DestroyWindow();
	delete m_pDlgMotor;
	m_pDlgMotor = NULL;

	if (m_pDlgLoginMain->GetSafeHwnd())	m_pDlgLoginMain->DestroyWindow();
	delete m_pDlgLoginMain;
	m_pDlgLoginMain = NULL;

	if (m_pDlgLoginAdmin->GetSafeHwnd())	m_pDlgLoginAdmin->DestroyWindow();
	delete m_pDlgLoginAdmin;
	m_pDlgLoginAdmin = NULL;

	if (m_pDlgLoginMgr->GetSafeHwnd()) m_pDlgLoginMgr->DestroyWindow();
	delete m_pDlgLoginMgr;
	m_pDlgLoginMgr = NULL;


	/* ���α׷� ���� ��� ���� ������ */
	if (!m_pDlgWait)
	{
		m_pDlgWait	= new CDlgWait(this);
		ASSERT(m_pDlgWait);
		if (m_pDlgWait->Create())
		{
			m_pDlgWait->Invalidate(TRUE);
			m_pDlgWait->ShowWindow(SW_SHOW);
		}
	}
	ShowWindow(SW_HIDE);
#if 0
	PostMessage(WM_CLOSE, 0, 0L);
#else
	m_pMainThread->SetExitApp(1000);
#endif
}

/*
 desc : Window �̺�Ʈ�� ������ MotorConsole â�� ����.
 parm : None
 retn : None
*/
LRESULT CDlgMain::OnOpenMotorConsole(WPARAM wParam, LPARAM lParam)
{
	if (m_pDlgMotor != nullptr)
	{
		m_pDlgMotor->ShowWindow(SW_SHOW);
	}

	return 0;
}

LRESULT CDlgMain::OnOpenLoginConsole(WPARAM wParam, LPARAM lParam)
{
	if (m_pDlgLoginMain != nullptr)
	{
		m_pDlgLoginMain->DoModal();
	}

	UpdateUILevel();
 	m_pDlgMenu->UpdateUILevel();
	return 0;
}

LRESULT CDlgMain::OnOpenAdminConsole(WPARAM wParam, LPARAM lParam)
{
	if (m_pDlgLoginAdmin != nullptr)
	{
		m_pDlgLoginAdmin->ShowWindow(SW_SHOW);
	}

	return 0;
}

LRESULT CDlgMain::OnOpenManagerConsole(WPARAM wParam, LPARAM lParam)
{
	if (m_pDlgLoginMgr != nullptr)
	{
		m_pDlgLoginMgr->ShowWindow(SW_SHOW);
	}

	return 0;
}

#define DEF_UI_DEFAULT_FONT_LEVEL1					22
#define DEF_UI_DEFAULT_FONT_LEVEL2					18
#define DEF_UI_DEFAULT_FONT_LEVEL3					14

void CDlgMain::CreateUserFont()
{
	// by sysandj : ��Ʈ ����
	for (int i = 0; i < eFONT_MAX; i++)
	{
		g_lf[i].lfHeight = (int)((double)DEF_UI_DEFAULT_FONT_LEVEL1);
		g_lf[i].lfWidth = 0;
		g_lf[i].lfEscapement = 0;
		g_lf[i].lfOrientation = 0;
		g_lf[i].lfItalic = false;
		g_lf[i].lfUnderline = false;
		g_lf[i].lfStrikeOut = false;
		g_lf[i].lfCharSet = DEFAULT_CHARSET;
		g_lf[i].lfOutPrecision = OUT_DEFAULT_PRECIS;
		g_lf[i].lfClipPrecision = CLIP_DEFAULT_PRECIS;
		g_lf[i].lfQuality = CLEARTYPE_NATURAL_QUALITY;
		g_lf[i].lfPitchAndFamily = DEFAULT_PITCH;
		StrCpy(g_lf[i].lfFaceName, DEF_FONT_NAME);
		g_lf[i].lfWeight = FW_BOLD;
	}

	g_lf[eFONT_LEVEL2_BOLD].lfHeight = (int)((double)DEF_UI_DEFAULT_FONT_LEVEL2);
	g_lf[eFONT_LEVEL3_BOLD].lfHeight = (int)((double)DEF_UI_DEFAULT_FONT_LEVEL3);
	g_lf[eFONT_LEVEL1].lfHeight = (int)((double)DEF_UI_DEFAULT_FONT_LEVEL2);
	g_lf[eFONT_LEVEL1].lfWeight = FW_NORMAL;
	g_lf[eFONT_LEVEL2].lfHeight = (int)((double)DEF_UI_DEFAULT_FONT_LEVEL2);
	g_lf[eFONT_LEVEL2].lfWeight = FW_NORMAL;
	g_lf[eFONT_LEVEL3].lfHeight = (int)((double)DEF_UI_DEFAULT_FONT_LEVEL3);
	g_lf[eFONT_LEVEL3].lfWeight = FW_NORMAL;
		
	for (int i = 0; i < eFONT_MAX; i++)
	{
		g_font[i].DeleteObject();
		g_font[i].CreateFontIndirect(&g_lf[i]);
	}
}

BOOL CDlgMain::PhilSendMoveRecvAck(STG_PP_PACKET_RECV* stRecv, BOOL is_busy)
{
	BOOL Succ = TRUE;
	unsigned short	ErrorCode = ePHILHMI_ERR_OK;
	g_u16PhilCommand = stRecv->st_header.nCommand;

	//Sleep(1000);//�ϴ� �����ô�.

	if (is_busy)
	{
		ErrorCode = ePHILHMI_ERR_STATUS_BUSY;
		Succ = FALSE;
	}

	/*ABS �̵� ���� ��ȣ�� ���� ���� �Ǿ��ٴ� Ȯ�� ��ȣ*/
	if (g_u16PhilCommand == (int)ENG_PHPC::ePHILHMI_C2P_ABS_MOVE)
	{
		STG_PP_C2P_ABS_MOVE_ACK stAbsAck;
		stAbsAck.Reset();
		stAbsAck.ulUniqueID = stRecv->st_c2p_abs_move.ulUniqueID;
		stAbsAck.usErrorCode = ErrorCode;

		uvEng_Philhmi_Send_C2P_ABS_MOVE_ACK(stAbsAck);
	}
	/*REL �̵� ���� ��ȣ�� ���� ���� �Ǿ��ٴ� Ȯ�� ��ȣ*/
	else if (g_u16PhilCommand == (int)ENG_PHPC::ePHILHMI_C2P_REL_MOVE)
	{
		STG_PP_C2P_REL_MOVE_ACK stRelAck;
		stRelAck.Reset();
		stRelAck.ulUniqueID = stRecv->st_c2p_rel_move.ulUniqueID;
		stRelAck.usErrorCode = ErrorCode;

		uvEng_Philhmi_Send_C2P_REL_MOVE_ACK(stRelAck);
	}
	/*CHAR �̵� ���� ��ȣ�� ���� ���� �Ǿ��ٴ� Ȯ�� ��ȣ*/
	else if (g_u16PhilCommand == (int)ENG_PHPC::ePHILHMI_C2P_CHAR_MOVE)
	{
		STG_PP_C2P_CHAR_MOVE_ACK stCharAck;
		stCharAck.Reset();
		stCharAck.ulUniqueID = stRecv->st_c2p_char_move.ulUniqueID;
		stCharAck.usErrorCode = ErrorCode;

		uvEng_Philhmi_Send_C2P_CHAR_MOVE_ACK(stCharAck);
	}

	return Succ;
}




VOID CDlgMain::PhilSendMove(STG_PP_PACKET_RECV* stRecv, int AxisCount)
{
	TCHAR szTemp[512] = { NULL };
	CUniToChar csCnv;
	CString strAxis;
	ENG_MMDI drv_id = ENG_MMDI::en_all;
	double dTargetPos = 0, dSpeed = 0;
	unsigned short	ErrorCode = ePHILHMI_ERR_OK;


	/*�̵� ���� ����*/
	g_u16PhilCommand = stRecv->st_header.nCommand;
	if (g_u16PhilCommand == (int)ENG_PHPC::ePHILHMI_C2P_ABS_MOVE)
	{
		/*Axis �̸�*/
		strAxis.Format(_T("%s"), csCnv.Ansi2Uni(stRecv->st_c2p_abs_move.stMove[AxisCount].szAxisName));
		dTargetPos = stRecv->st_c2p_abs_move.stMove[AxisCount].dPosition;
		dSpeed = stRecv->st_c2p_abs_move.stMove[AxisCount].dSpeed;

		if (_T("STAGE_X") == strAxis)			drv_id = ENG_MMDI::en_stage_x;
		else if (_T("STAGE_Y") == strAxis)		drv_id = ENG_MMDI::en_stage_y;
		else if (_T("Cam1_X") == strAxis)		drv_id = ENG_MMDI::en_align_cam1;
		else if (_T("Cam2_X") == strAxis)		drv_id = ENG_MMDI::en_align_cam2;
		else if (_T("PH1_Z") == strAxis)		drv_id = ENG_MMDI::en_axis_ph1;
		else if (_T("PH2_Z") == strAxis)		drv_id = ENG_MMDI::en_axis_ph2;
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
		else if (_T("PH3_Z") == strAxis)		drv_id = ENG_MMDI::en_axis_ph3;
		else if (_T("PH4_Z") == strAxis)		drv_id = ENG_MMDI::en_axis_ph4;
		else if (_T("PH5_Z") == strAxis)		drv_id = ENG_MMDI::en_axis_ph5;
		else if (_T("PH6_Z") == strAxis)		drv_id = ENG_MMDI::en_axis_ph6;
#endif
		/*�Ϸ� Ȯ���� ���� ����*/
		m_stPhilStatus.phil_move = en_menu_phil_move_abs;
		m_stPhilStatus.unique_id = stRecv->st_c2p_abs_move.ulUniqueID;
		m_stPhilStatus.drive_id = (UINT8)drv_id;
		m_stPhilStatus.move_dist = dTargetPos;
	}
	//else if (mode == en_menu_phil_move_rel)
	else if (g_u16PhilCommand == (int)ENG_PHPC::ePHILHMI_C2P_REL_MOVE)
	{
		strAxis.Format(_T("%s"), csCnv.Ansi2Uni(stRecv->st_c2p_rel_move.stMove[AxisCount].szAxisName));
		dTargetPos = stRecv->st_c2p_rel_move.stMove[AxisCount].dPosition;
		dSpeed = stRecv->st_c2p_rel_move.stMove[AxisCount].dSpeed;

		if (_T("STAGE_X") == strAxis)			drv_id = ENG_MMDI::en_stage_x;
		else if (_T("STAGE_Y") == strAxis)		drv_id = ENG_MMDI::en_stage_y;
		else if (_T("Cam1_X") == strAxis)		drv_id = ENG_MMDI::en_align_cam1;
		else if (_T("Cam2_X") == strAxis)		drv_id = ENG_MMDI::en_align_cam2;
		else if (_T("PH1_Z") == strAxis)		drv_id = ENG_MMDI::en_axis_ph1;
		else if (_T("PH2_Z") == strAxis)		drv_id = ENG_MMDI::en_axis_ph2;
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
		else if (_T("PH3_Z") == strAxis)		drv_id = ENG_MMDI::en_axis_ph3;
		else if (_T("PH4_Z") == strAxis)		drv_id = ENG_MMDI::en_axis_ph4;
		else if (_T("PH5_Z") == strAxis)		drv_id = ENG_MMDI::en_axis_ph5;
		else if (_T("PH6_Z") == strAxis)		drv_id = ENG_MMDI::en_axis_ph6;
#endif
		else                                    drv_id = ENG_MMDI::en_axis_none;

		// ��� �̵��� ���
		dTargetPos = uvCmn_MC2_GetDrvAbsPos(drv_id) + dTargetPos;

		/*�Ϸ� Ȯ���� ���� ����*/
		m_stPhilStatus.phil_move = en_menu_phil_move_rel;
		m_stPhilStatus.unique_id = stRecv->st_c2p_rel_move.ulUniqueID;
		m_stPhilStatus.drive_id = (UINT8)drv_id;
		m_stPhilStatus.move_dist = dTargetPos;
	}

	else if (g_u16PhilCommand == (int)ENG_PHPC::ePHILHMI_C2P_CHAR_MOVE)
	{
		//EFEM_POS
		strAxis.Format(_T("%s"), csCnv.Ansi2Uni(stRecv->st_c2p_char_move.stMoveTeach[AxisCount].szAxisName));
		stRecv->st_c2p_char_move.stMoveTeach[AxisCount].szPositionName;

		if (_T("STAGE_X") == strAxis)			drv_id = ENG_MMDI::en_stage_x;
		else if (_T("STAGE_Y") == strAxis)		drv_id = ENG_MMDI::en_stage_y;
		else if (_T("Cam1_X") == strAxis)		drv_id = ENG_MMDI::en_align_cam1;
		else if (_T("Cam2_X") == strAxis)		drv_id = ENG_MMDI::en_align_cam2;
		else if (_T("PH1_Z") == strAxis)		drv_id = ENG_MMDI::en_axis_ph1;
		else if (_T("PH2_Z") == strAxis)		drv_id = ENG_MMDI::en_axis_ph2;
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
		else if (_T("PH3_Z") == strAxis)		drv_id = ENG_MMDI::en_axis_ph3;
		else if (_T("PH4_Z") == strAxis)		drv_id = ENG_MMDI::en_axis_ph4;
		else if (_T("PH5_Z") == strAxis)		drv_id = ENG_MMDI::en_axis_ph5;
		else if (_T("PH6_Z") == strAxis)		drv_id = ENG_MMDI::en_axis_ph6;
#endif
		else                                    drv_id = ENG_MMDI::en_axis_none;

		if (memcmp(stRecv->st_c2p_char_move.stMoveTeach[AxisCount].szPositionName, L"EFEM_POS", DEF_MAX_POSITION_NAME_LENGTH))
		{
			if (drv_id == ENG_MMDI::en_stage_x)				dTargetPos = uvEng_GetConfig()->set_align.table_unloader_xy[0][0];
			else if (drv_id == ENG_MMDI::en_stage_y)		dTargetPos = uvEng_GetConfig()->set_align.table_unloader_xy[0][1];
			else                                            ErrorCode = ePHILHMI_ERR_MOTOR_NOT_DEFINE_INPUT;
		}
		else if (memcmp(stRecv->st_c2p_char_move.stMoveTeach[AxisCount].szPositionName, L"MANUAL_POS", DEF_MAX_POSITION_NAME_LENGTH))
		{
			if (drv_id == ENG_MMDI::en_stage_x)				dTargetPos = uvEng_GetConfig()->mc2b_svc.max_dist[(UINT8)drv_id] - 10;
			else if (drv_id == ENG_MMDI::en_stage_y)		dTargetPos = uvEng_GetConfig()->mc2b_svc.max_dist[(UINT8)drv_id] - 10;
			else                                            ErrorCode = ePHILHMI_ERR_MOTOR_NOT_DEFINE_INPUT;
		}

		/*�Ϸ� Ȯ���� ���� ����*/
		m_stPhilStatus.phil_move = en_menu_phil_move_char;
		m_stPhilStatus.unique_id = stRecv->st_c2p_char_move.ulUniqueID;
		m_stPhilStatus.drive_id = (UINT8)drv_id;
		m_stPhilStatus.move_dist = dTargetPos;
	}

	/*Mc2 ����*/
	if (drv_id > ENG_MMDI::en_axis_acam1)
	{
		ErrorCode = ePHILHMI_ERR_MOTOR_NOT_DEFINE;
	}

	//���� on ���� Ȯ��
	else if (FALSE == uvCmn_MC2_IsDevLocked(drv_id))
	{
		//ErrorCode = ePHILHMI_ERR_MOTOR_OFF_LOCK;
		ErrorCode = ePHILHMI_ERR_MOTOR_ALARM;
	}
	// ���� ���� Ȯ��
	else if (TRUE == uvCmn_MC2_IsDriveError(drv_id))
	{
		ErrorCode = ePHILHMI_ERR_MOTOR_ALARM;

	}
	// Busy ��ȣ Ȯ��
	else if (TRUE == uvCmn_MC2_IsDriveBusy(drv_id))
	{
		ErrorCode = ePHILHMI_ERR_MOTOR_MOVING;
	}
	//�������� ���ۿ� 
	else if (CInterLockManager::GetInstance()->CheckMoveInterlock(drv_id, dTargetPos))
	{
		//ErrorCode = ePHILHMI_ERR_MOTOR_IO_CHECK;
		ErrorCode = ePHILHMI_ERR_MOTOR_ALARM;
	}

	/*���� �Ÿ� Ȯ�� �Ͽ� Max, Min Ȯ��*/
	INT32 i32MinDist, i32MaxDist;
	UINT8 u8drv_id;
	if (drv_id < (ENG_MMDI)DRIVEDIVIDE)
	{
		i32MinDist = (INT32)ROUNDED(uvEng_GetConfig()->mc2_svc.min_dist[UINT8(drv_id)] * 10000.0f, 0);
		i32MaxDist = (INT32)ROUNDED(uvEng_GetConfig()->mc2_svc.max_dist[UINT8(drv_id)] * 10000.0f, 0);

	}
	else
	{
		u8drv_id = (UINT8)drv_id - DRIVEDIVIDE;
		i32MinDist = (INT32)ROUNDED(uvEng_GetConfig()->mc2b_svc.min_dist[u8drv_id] * 10000.0f, 0);
		i32MaxDist = (INT32)ROUNDED(uvEng_GetConfig()->mc2b_svc.max_dist[u8drv_id] * 10000.0f, 0);
	}
	if (i32MaxDist < (dTargetPos * 10000.0f))
	{
		ErrorCode = ePHILHMI_ERR_MOTOR_PLM;
	}
	else if (i32MinDist > (dTargetPos * 10000.0f))
	{
		ErrorCode = ePHILHMI_ERR_MOTOR_NLM;
	}


	/*���� ��Ȳ�� ���� ��� ���� ����*/
	if (ErrorCode == ePHILHMI_ERR_OK)
	{
		/*������ ���� ����*/
		uvEng_MC2_SendDevAbsMove(drv_id, dTargetPos, dSpeed);
		/*�̵� ���� ���� �ð�*/
		m_u64StartTimeHMI = GetTickCount64();
	}
	/*������ ������ Ȯ���� �Ǹ� ���� ���� ó��*/
	else
	{
		if (m_stPhilStatus.phil_move == en_menu_phil_move_abs)
		{
			STG_PP_P2C_ABS_MOVE_COMP stAbsMove;
			STG_PP_P2C_ABS_MOVE_COMP_ACK stAbsMoveAck;
			stAbsMove.Reset();
			stAbsMoveAck.Reset();
			stAbsMove.ulUniqueID = m_stPhilStatus.unique_id;
			stAbsMove.usErrorCode = ErrorCode;

			uvEng_Philhmi_Send_P2C_ABS_MOVE_COMP(stAbsMove, stAbsMoveAck);
		}
		else if (m_stPhilStatus.phil_move == en_menu_phil_move_rel)
		{
			STG_PP_P2C_REL_MOVE_COMP stRelMove;
			STG_PP_P2C_REL_MOVE_COMP_ACK stRelMoveAck;
			stRelMove.Reset();
			stRelMoveAck.Reset();
			stRelMove.ulUniqueID = m_stPhilStatus.unique_id;
			stRelMove.usErrorCode = ErrorCode;

			uvEng_Philhmi_Send_P2C_REL_MOVE_COMP(stRelMove, stRelMoveAck);
		}
		else if (m_stPhilStatus.phil_move == en_menu_phil_move_char)
		{
			STG_PP_P2C_CHAR_MOVE_COMP stCharMove;
			STG_PP_P2C_CHAR_MOVE_COMP_ACK stCharMoveAck;
			stCharMove.Reset();
			stCharMoveAck.Reset();
			stCharMove.ulUniqueID = m_stPhilStatus.unique_id;
			stCharMove.usErrorCode = ErrorCode;

			uvEng_Philhmi_Send_P2C_CHAR_MOVE_COMP(stCharMove, stCharMoveAck);
		}

		m_stPhilStatus.phil_move = en_menu_phil_move_none;
	}
}


VOID CDlgMain::PhilSendMoveComplete(STG_PP_PACKET_RECV* stRecv)
{
	unsigned short	ErrorCode = ePHILHMI_ERR_OK;
	g_u16PhilCommand = stRecv->st_header.nCommand;

	/*ABS �̵� �Ϸ� ���� ��ȣ*/
	if (g_u16PhilCommand == (int)ENG_PHPC::ePHILHMI_C2P_ABS_MOVE_COMPLETE)
	{
		STG_PP_C2P_ABS_MOVE_COMP_ACK stAbsComplete;
		stAbsComplete.Reset();
		stAbsComplete.ulUniqueID = stRecv->st_c2p_abs_move.ulUniqueID;
		stAbsComplete.usErrorCode = ErrorCode;

		uvEng_Philhmi_Send_C2P_ABS_MOVE_COMP_ACK(stAbsComplete);
	}
	/*REL �̵� �Ϸ� ���� ��ȣ*/
	else if (g_u16PhilCommand == (int)ENG_PHPC::ePHILHMI_C2P_REL_MOVE_COMPLETE)
	{
		STG_PP_C2P_REL_MOVE_COMP_ACK stRelComplete;
		stRelComplete.Reset();
		stRelComplete.ulUniqueID = stRecv->st_c2p_rel_move.ulUniqueID;
		stRelComplete.usErrorCode = ErrorCode;

		uvEng_Philhmi_Send_C2P_REL_MOVE_COMP_ACK(stRelComplete);
	}
	else if (g_u16PhilCommand == (int)ENG_PHPC::ePHILHMI_C2P_CHAR_MOVE_COMPLETE)
	{
		STG_PP_C2P_CHAR_MOVE_COMP_ACK stCharComplete;
		stCharComplete.Reset();
		stCharComplete.ulUniqueID = stRecv->st_c2p_rel_move.ulUniqueID;
		stCharComplete.usErrorCode = ErrorCode;

		uvEng_Philhmi_Send_C2P_CHAR_MOVE_COMP_ACK(stCharComplete);
	}
}
VOID CDlgMain::PhilSendProcessExecute(STG_PP_PACKET_RECV* stRecv, BOOL is_busy)
{
	STG_PP_C2P_PROCESS_EXECUTE_ACK stProcessExecute;
	stProcessExecute.Reset();
	stProcessExecute.ulUniqueID = stRecv->st_c2p_process_execute.ulUniqueID;

	TCHAR szTemp[512] = { NULL };
	/*DlgPhilhmi ���� �޽��� Ȯ��*/
	if (m_pDlgMenu && m_pDlgMenu->GetDlgID() == ENG_CMDI::en_menu_philhmi)
	{
		((CDlgPhilhmi*)m_pDlgMenu)->AddListBox(_T("C2P Process Execute"));
		swprintf_s(szTemp, 512, L"Process Execute : %S - %S  ",
			stRecv->st_c2p_process_execute.szRecipeName, stRecv->st_c2p_process_execute.szGlassID);
		((CDlgPhilhmi*)m_pDlgMenu)->AddListBox(szTemp);

	}
	/*�뱤 ���� ����*/
	m_stExpoLog.Init();
	memcpy(m_stExpoLog.recipe_name, stRecv->st_c2p_process_execute.szRecipeName, DEF_MAX_RECIPE_NAME_LENGTH);
	memcpy(m_stExpoLog.glassID, stRecv->st_c2p_process_execute.szGlassID, DEF_MAX_GLASS_NAME_LENGTH);
	/*���� �Ķ���Ͱ� ������*/
	memcpy(stProcessExecute.szRecipeName, stRecv->st_c2p_process_execute.szRecipeName, DEF_MAX_RECIPE_NAME_LENGTH);
	memcpy(stProcessExecute.szGlassID, stRecv->st_c2p_process_execute.szGlassID, DEF_MAX_GLASS_NAME_LENGTH);

	/*�������� ���� ����*/
	BOOL bSelect = uvEng_JobRecipe_GetSelectRecipe() ? TRUE : FALSE;
	/*������ ��� ����*/
	BOOL bLoaded = uvCmn_Luria_IsJobNameLoaded();
	/*�����ǿ� ��ũ ��� ����*/
	BOOL bMarked = uvEng_Luria_IsMarkGlobal();


	if ((bSelect && bLoaded && !is_busy))
	{
		RunWorkJob(ENG_BWOK::en_expo_only, PUINT64(&m_stExpoLog));
	}
	else
	{
		stProcessExecute.usErrorCode = ePHILHMI_ERR_STATUS_BUSY;
		return;
	}


	uvEng_Philhmi_Send_C2P_PROCESS_EXECUTE_ACK(stProcessExecute);
}

VOID CDlgMain::PhilSendChageMode(STG_PP_PACKET_RECV* stRecv)
{
	STG_PP_C2P_MODE_CHANGE_ACK stModeChange;
	stModeChange.Reset();

	/*��� ����*/
	stModeChange.usMode = stRecv->st_c2p_mode_change.usMode;
	/*���� ����*/
	g_u8Romote = stRecv->st_c2p_mode_change.usMode;

	if (stRecv->st_c2p_mode_change.usMode == en_menu_phil_mode_auto)
	{
		CreateMenu(IDC_MAIN_BTN_AUTO);
	}
	else if (stRecv->st_c2p_mode_change.usMode == en_menu_phil_mode_stop)
	{
		/*��� ���� ����*/
		RunWorkJob(ENG_BWOK::en_work_stop);
		//CreateMenu(IDC_MAIN_BTN_MANUAL);
	}
	else if (stRecv->st_c2p_mode_change.usMode == en_menu_phil_mode_pause)
	{
		/*��� ���� ����*/
		RunWorkJob(ENG_BWOK::en_work_stop);
	}
	else if (stRecv->st_c2p_mode_change.usMode == en_menu_phil_mode_alarm)
	{
	}
	else if (stRecv->st_c2p_mode_change.usMode == en_menu_phil_mode_pm)
	{
	}

	stModeChange.ulUniqueID = stRecv->st_c2p_mode_change.ulUniqueID;
	uvEng_Philhmi_Send_C2P_MODE_CHANGE_ACK(stModeChange);
}

VOID CDlgMain::PhilSendInitialExecute(STG_PP_PACKET_RECV* stRecv, BOOL is_busy)
{
	STG_PP_C2P_INITIAL_EXECUTE_ACK	stInitialExecuteAck;
	stInitialExecuteAck.Reset();

	stInitialExecuteAck.ulUniqueID = stRecv->st_c2p_initial_execute.ulUniqueID;

	if (is_busy)
	{
		/*��� �ʱ�ȭ ����*/
		RunWorkJob(ENG_BWOK::en_work_init);
	}
	else
	{
		stInitialExecuteAck.usErrorCode = ePHILHMI_ERR_STATUS_BUSY;
	}

	uvEng_Philhmi_Send_C2P_INITIAL_EXECUTE_ACK(stInitialExecuteAck);
}

VOID CDlgMain::PhilSendSubProcessExecute(STG_PP_PACKET_RECV* stRecv)
{
	STG_PP_C2P_SUBPROCESS_EXECUTE_ACK stSubProcessExecute;
	stSubProcessExecute.Reset();
	stSubProcessExecute.ulUniqueID = stRecv->st_c2p_subprocess_execute.ulUniqueID;

	TCHAR szTemp[512] = { NULL };
	/*DlgPhilhmi ���� �޽��� Ȯ��*/
	if (m_pDlgMenu && m_pDlgMenu->GetDlgID() == ENG_CMDI::en_menu_philhmi)
	{
		((CDlgPhilhmi*)m_pDlgMenu)->AddListBox(_T("C2P Sub Process Execute"));
		swprintf_s(szTemp, 512, L"Sub Process Execute : %S",
			stRecv->st_c2p_subprocess_execute.szAckSubProcessName);
		((CDlgPhilhmi*)m_pDlgMenu)->AddListBox(szTemp);

	}
	/*���� �Ķ���Ͱ� ������*/
	memcpy(stSubProcessExecute.szAckSubProcessName, stRecv->st_c2p_subprocess_execute.szAckSubProcessName, DEF_MAX_SUBPROCESS_NAME_LENGTH);
	uvEng_Philhmi_Send_C2P_SUBPROCESS_EXECUTE_ACK(stSubProcessExecute);
}

VOID CDlgMain::PhilSendEventStatus(STG_PP_PACKET_RECV* stRecv)
{
	STG_PP_C2P_EVENT_STATUS_ACK		stEventStatus;
	stEventStatus.Reset();
	stEventStatus.ulUniqueID = stRecv->st_c2p_event_status.ulUniqueID;
	stEventStatus.bEventFlag = 0;


	memcpy(stEventStatus.szEventName, stRecv->st_c2p_event_status.szEventName, DEF_MAX_EVENT_NAME_LENGTH);

	// 20240217 mhbaek Modify : ���ڿ� �񱳹����� ����
	if (0 == strcmp(stEventStatus.szEventName, "MPSTART"))
	{
		stEventStatus.bEventFlag = m_stPhilStatus.mp_start;
	}
	else if (0 == strcmp(stEventStatus.szEventName, "SAFEPOS"))
	{
		stEventStatus.bEventFlag = m_stPhilStatus.safe_pos;
	}
	else if (0 == strcmp(stEventStatus.szEventName, "RECIPE_COMP"))
	{
		stEventStatus.bEventFlag = m_stPhilStatus.recipe_comp;
		//stEventStatus.bEventFlag = uvCmn_Luria_IsJobNameLoaded();
	}


	uvEng_Philhmi_Send_C2P_EVENT_STATUS_ACK(stEventStatus);
}

VOID CDlgMain::PhilSendEventNotify(STG_PP_PACKET_RECV* stRecv)
{
	STG_PP_C2P_EVENT_NOTIFY_ACK		stEventNotify;
	stEventNotify.Reset();
	stEventNotify.ulUniqueID = stRecv->st_c2p_event_notify.ulUniqueID; 

	TCHAR szTemp[512] = { NULL };
	if (m_pDlgMenu && m_pDlgMenu->GetDlgID() == ENG_CMDI::en_menu_philhmi)
	{
		((CDlgPhilhmi*)m_pDlgMenu)->AddListBox(_T("C2P Event Notify"));
		swprintf_s(szTemp, 512, L"Event : %S", stRecv->st_c2p_event_notify.szEventName);
		((CDlgPhilhmi*)m_pDlgMenu)->AddListBox(szTemp);
	}
	uvEng_Philhmi_Send_C2P_EVENT_NOTIFY_ACK(stEventNotify);
}
VOID CDlgMain::PhilSendTimeSync(STG_PP_PACKET_RECV* stRecv)
{
	STG_PP_C2P_TIME_SYNC_ACK		stTimeSync;
	stTimeSync.Reset();
	stTimeSync.ulUniqueID = stRecv->st_c2p_time_sync.ulUniqueID;

	/*���� �ð�*/
	SYSTEMTIME stTm = { NULL };
	GetLocalTime(&stTm);
	char szSyncTime[DEF_TIME_LENGTH + 1] = { 0, };
	sprintf_s(stRecv->st_c2p_time_sync.szSyncTime, DEF_TIME_LENGTH + 1, "%04d%02d%02d%02d%02d%02d",
		stTm.wYear, stTm.wMonth, stTm.wDay, stTm.wHour, stTm.wMinute, stTm.wSecond);
	memcpy(stRecv->st_c2p_time_sync.szSyncTime, szSyncTime, DEF_TIME_LENGTH);

	TCHAR szTemp[512] = { NULL };
	if (m_pDlgMenu && m_pDlgMenu->GetDlgID() == ENG_CMDI::en_menu_philhmi)
	{
		((CDlgPhilhmi*)m_pDlgMenu)->AddListBox(_T("C2P Time sync"));
		swprintf_s(szTemp, 512, L"Time : %S", stRecv->st_c2p_time_sync.szSyncTime);
		((CDlgPhilhmi*)m_pDlgMenu)->AddListBox(szTemp);
	}
	uvEng_Philhmi_Send_C2P_TIME_SYNC_ACK(stTimeSync);
}

VOID CDlgMain::PhilSendInterruptStop(STG_PP_PACKET_RECV* stRecv)
{
	STG_PP_C2P_INTERRUPT_STOP_ACK	stInterruptStop;
	stInterruptStop.Reset();
	stInterruptStop.ulUniqueID = stRecv->st_c2p_interrupt_stop.ulUniqueID;

	if (m_pDlgMenu && m_pDlgMenu->GetDlgID() == ENG_CMDI::en_menu_philhmi)
	{
		((CDlgPhilhmi*)m_pDlgMenu)->AddListBox(_T("C2P Interrupt Stop"));
	}
	/*��� ���� ����*/
	RunWorkJob(ENG_BWOK::en_work_stop);
	uvEng_Philhmi_Send_C2P_INTERRUPT_STOP_ACK(stInterruptStop);
}
