
/*
 desc : All Status
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgStat.h"

#include "./stat/GridLuria.h"		/* Grid Control */
#include "./stat/GridDrive.h"		/* Grid Control */
#include "./stat/GridTempDI.h"		/* Grid Control */


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgStat::CDlgStat(UINT32 id, CWnd* parent)
	: CDlgMenu(id, parent)
{
	/* 자신의 다이얼로그 ID 설정 */
	m_enDlgID		= ENG_CMDI::en_menu_none;//ENG_CMDI::en_menu_stat;
	m_u64ReqTime	= 0;
	m_u8NextRefresh	= 0x00;
	m_u8NextCmd		= 0x00;
}

CDlgStat::~CDlgStat()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgStat::DoDataExchange(CDataExchange* dx)
{
	CDlgMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* groupbox - normal */
	u32StartID	= IDC_STAT_GRP_PH_STATUS;
	for (i=0; i< eSTATE_GRP_MAX; i++)	DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* button - normal */
	u32StartID	= IDC_STAT_BTN_PH_NO_UP;
	for (i=0; i< eSTATE_BTN_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* static - normal */
	u32StartID	= IDC_STAT_TXT_HDD_TOTAL;
	for (i=0; i< eSTATE_TXT_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* edit - integer */
	u32StartID	= IDC_STAT_EDT_PH_NO;
	for (i=0; i< eSTATE_EDT_INT_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_edt_int[i]);
	/* edit - integer */
	u32StartID	= IDC_STAT_EDT_HDD_TOTAL;
	for (i=0; i< eSTATE_EDT_FLOAT_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_edt_flt[i]);
}

BEGIN_MESSAGE_MAP(CDlgStat, CDlgMenu)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_STAT_BTN_PH_NO_UP, IDC_STAT_BTN_REFRESH, OnBtnClick)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgStat::PreTranslateMessage(MSG* msg)
{
	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgStat::OnInitDlg()
{
	/* 컨트롤 초기화 */
	InitCtrl();
	if (!InitGrid())	return FALSE;
#if 0
	/* HDD Information */
	AllRefresh();
#endif
	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgStat::OnExitDlg()
{
	CloseGrid();
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgStat::OnPaintDlg(CDC *dc)
{

	/* 자식 윈도 호출 */
	CDlgMenu::DrawOutLine();
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgStat::OnResizeDlg()
{
}

/*
 desc : 부모 스레드에 의해 주기적으로 호출됨
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgStat::UpdatePeriod(UINT64 tick, BOOL is_busy)
{
	/* Motor Drive Status */
	if (m_pGridDriveStat)	m_pGridDriveStat->Update();
	/* Control Enable or Disable */
	UpdateControl(tick, is_busy);
	UpdateRefresh();
}

/*
 desc : 컨트롤
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgStat::UpdateControl(UINT64 tick, BOOL is_busy)
{
	/* 현재 작업 중이지 않다면, 주기적으로 상태 정보 요청 */
	if (!is_busy)
	{
		/* 임의 주기마다 값 요청 */
#ifdef _DEBUG
		if (m_u64ReqTime+1000 > GetTickCount64())	return;
#else
		if (m_u64ReqTime+500 > GetTickCount64())	return;
#endif
		/* 가장 최근에 요청한 시간 갱신 */
		m_u64ReqTime	= GetTickCount64();

		switch (m_u8NextCmd)
		{
//		case 0x00 :	uvEng_Luria_GetPktLightSourceStatusMulti();	break;	/* LED Status */
 		case 0x01 :	uvEng_Luria_ReqGetPhLedOnTimeAll();			break;	/* PH Led On Time All */
 		case 0x02 :	uvEng_Luria_ReqGetPhLedTempAll();			break;	/* PH Main Temperature */
		case 0x03 :	uvEng_Luria_ReqGetSystemStatus();			break;	/* Luria System Status */
		}
		if (++m_u8NextCmd == 0x04)	m_u8NextCmd	= 0x00;
	}
}

/*
 desc : 정보 갱신
 parm : None
 retn : None
*/
VOID CDlgStat::UpdateRefresh()
{
	switch (m_u8NextRefresh)
	{
	case 0x00	: m_pGridLuriaMC2->Update();		 break;
	case 0x01	: m_pGridLuriaPC->Update();			 break;
	case 0x02	: m_pGridLuriaOverall->Update();	 break;
	case 0x03	: m_pGridLuriaLedSource->Update();	 break;
	case 0x04	: m_pGridLuriaPHStat->Update();		 break;
	case 0x05	: m_pGridLuriaJob->Update();		 break;
	case 0x06	: m_pGridLuriaPHInfo->Update();		 break;
	case 0x07	: m_pGridDriveStat->Update();		 break;
	case 0x08	: m_pGridSD2Error->Update();		 break;
	case 0x09	: m_pGridMC2Error->Update();		 break;
	case 0x0a	: m_pGridTempDI->Update();			 break;
	}
	if (0x0a == m_u8NextRefresh)	m_u8NextRefresh	= 0x00;
	else							m_u8NextRefresh++;
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgStat::InitCtrl()
{
	CResizeUI clsResizeUI;
	INT32 i = 0;

	/* group box */
	for (i = 0; i < eSTATE_GRP_MAX; i++)
	{
		m_grp_ctl[i].SetLogFont(&g_lf[eFONT_LEVEL2_BOLD]);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_grp_ctl[i]);
		// by sysandj : Resize UI
	}
	/* static - Normal */
	for (i=0; i< eSTATE_TXT_MAX; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_txt_ctl[i]);
		// by sysandj : Resize UI
	}
	/* edit - integer */
	for (i=0; i< eSTATE_EDT_INT_MAX; i++)
	{
		m_edt_int[i].SetEditFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_edt_int[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_int[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_int[i].SetReadOnly(TRUE);
		m_edt_int[i].SetInputType(ENM_DITM::en_uint32);
		m_edt_int[i].SetInputSign(ENM_MNUS::en_unsign);
		m_edt_int[i].SetDisableKBDN();

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_edt_int[i]);
		// by sysandj : Resize UI
	}
	/* edit - double */
	for (i=0; i< eSTATE_EDT_FLOAT_MAX; i++)
	{
		m_edt_flt[i].SetEditFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_edt_flt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_flt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_flt[i].SetReadOnly(TRUE);
		m_edt_flt[i].SetInputType(ENM_DITM::en_double);
		m_edt_flt[i].SetInputSign(ENM_MNUS::en_unsign);
		m_edt_flt[i].SetInputPoint(3);
		m_edt_flt[i].SetDisableKBDN();

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_edt_flt[i]);
		// by sysandj : Resize UI
	}
	m_edt_int[eSTATE_EDT_PH_NO].SetTextToNum(INT64(1));
	/* button - normal */
	for (i=0; i< eSTATE_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
		// by sysandj : Resize UI
	}
}

/*
 desc : GridControl 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgStat::InitGrid()
{
	/* Photohead - Motion Status */
	m_pGridLuriaMC2 = new CGridLuriaMC2(m_hWnd, m_grp_ctl[eSTATE_GRP_PH_MOTOR].GetSafeHwnd(), IDC_GRID_STAT_PH_MOTION_STATUS);
	ASSERT(m_pGridLuriaMC2);
	if (!m_pGridLuriaMC2->Init())		return FALSE;
	/* Photohead - PC Status */
	m_pGridLuriaPC = new CGridLuriaPC(m_hWnd, m_grp_ctl[eSTATE_GRP_PH_PC].GetSafeHwnd(), IDC_GRID_STAT_LURIA_PC_STATUS);
	ASSERT(m_pGridLuriaPC);
	if (!m_pGridLuriaPC->Init())		return FALSE;
	/* Photohead - OverallErrorStatusMulti */
	m_pGridLuriaOverall = new CGridLuriaOverall(m_hWnd, m_grp_ctl[eSTATE_GRP_PH_OVERALL].GetSafeHwnd(), IDC_GRID_STAT_LURIA_OVERALL);
	ASSERT(m_pGridLuriaOverall);
	if (!m_pGridLuriaOverall->Init())	return FALSE;
	/* Photohead - GetLightSourceStatusMulti for LED */
	m_pGridLuriaLedSource = new CGridLuriaLedSource(m_hWnd, m_grp_ctl[eSTATE_GRP_PH_LIGHT].GetSafeHwnd(), IDC_GRID_STAT_LURIA_OVERALL);
	ASSERT(m_pGridLuriaLedSource);
	if (!m_pGridLuriaLedSource->Init())	return FALSE;
	/* Photohead - Init & Communication Status */
	m_pGridLuriaPHStat = new CGridLuriaPHStat(m_hWnd, m_grp_ctl[eSTATE_GRP_PH_COMM].GetSafeHwnd(), IDC_GRID_STAT_LURIA_PH_STAT);
	ASSERT(m_pGridLuriaPHStat);
	if (!m_pGridLuriaPHStat->Init())	return FALSE;
	/* Photohead - Job Load / Mark Regist / Expose Status */
	m_pGridLuriaJob = new CGridLuriaJob(m_hWnd, m_grp_ctl[eSTATE_GRP_JOB_STATUS].GetSafeHwnd(), IDC_GRID_STAT_LURIA_JOB);
	ASSERT(m_pGridLuriaJob);
	if (!m_pGridLuriaJob->Init())		return FALSE;
	/* Photohead - Job Load / Mark Regist / Expose Status */
	m_pGridLuriaPHInfo = new CGridLuriaPHInfo(m_hWnd, m_grp_ctl[eSTATE_GRP_PH_INFO].GetSafeHwnd(), IDC_GRID_STAT_LURIA_PH_INFO);
	ASSERT(m_pGridLuriaPHInfo);
	if (!m_pGridLuriaPHInfo->Init())	return FALSE;
	/* MC2 - Drive Information */
	m_pGridDriveStat = new CGridDriveStat(m_hWnd, m_grp_ctl[eSTATE_GRP_DRIVE_STATUS].GetSafeHwnd(), IDC_GRID_STAT_MC2_DRIVE_STAT);
	ASSERT(m_pGridDriveStat);
	if (!m_pGridDriveStat->Init(FALSE))	return FALSE;
	/* MC2 - SD2 Drive Error Message */
	m_pGridSD2Error = new CGridSD2Error(m_hWnd, m_grp_ctl[eSTATE_GRP_SD2_ERROR].GetSafeHwnd(), IDC_GRID_STAT_MC2_SD2_ERROR);
	ASSERT(m_pGridSD2Error);
	if (!m_pGridSD2Error->Init())		return FALSE;
	/* MC2 - MC2 Drive Error Message */
	m_pGridMC2Error = new CGridMC2Error(m_hWnd, m_grp_ctl[eSTATE_GRP_MC2_ERROR].GetSafeHwnd(), IDC_GRID_STAT_MC2_MC2_ERROR);
	ASSERT(m_pGridMC2Error);
	if (!m_pGridMC2Error->Init())		return FALSE;
	/* PLC - PLC Temperature DI */
	m_pGridTempDI = new CGridTempDI(m_hWnd, m_grp_ctl[eSTATE_GRP_PLC_DI_TEMP].GetSafeHwnd());
	ASSERT(m_pGridTempDI);
	if (!m_pGridTempDI->Init())		return FALSE;

	return TRUE;
}

/*
 desc : GridControl 해제
 parm : None
 retn : None
*/
VOID CDlgStat::CloseGrid()
{
	if (m_pGridLuriaMC2)		delete m_pGridLuriaMC2;
	if (m_pGridLuriaPC)			delete m_pGridLuriaPC;
	if (m_pGridLuriaOverall)	delete m_pGridLuriaOverall;
	if (m_pGridLuriaLedSource)	delete m_pGridLuriaLedSource;
	if (m_pGridLuriaPHStat)		delete m_pGridLuriaPHStat;
	if (m_pGridLuriaJob)		delete m_pGridLuriaJob;
	if (m_pGridLuriaPHInfo)		delete m_pGridLuriaPHInfo;
	if (m_pGridDriveStat)		delete m_pGridDriveStat;
	if (m_pGridSD2Error)		delete m_pGridSD2Error;
	if (m_pGridMC2Error)		delete m_pGridMC2Error;
	if (m_pGridTempDI)			delete m_pGridTempDI;
}

/*
 desc : 객체 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgStat::InitObject()
{

	return TRUE;
}

/*
 desc : 객체 해제
 parm : None
 retn : None
*/
VOID CDlgStat::CloseObject()
{
}

/*
 desc : 일반 버튼 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgStat::OnBtnClick(UINT32 id)
{
	UINT8 i	= 0x00;
	switch (id)
	{
	case IDC_STAT_BTN_PH_NO_UP	:	PHRefresh(1);	break;
	case IDC_STAT_BTN_PH_NO_DN	:	PHRefresh(-1);	break;
	case IDC_STAT_BTN_REFRESH	:	HDDRefresh();	break;
	}
}

/*
 desc : PH 정보 갱신
 parm : fact	- [in]  -1 or 1
 retn : None
*/
VOID CDlgStat::PHRefresh(INT8 fact)
{
	INT8 i8PhNo	= (INT8)m_edt_int[eSTATE_EDT_PH_NO].GetTextToNum() + fact;
	if (i8PhNo < 1)	i8PhNo	= 1;
	if (i8PhNo > uvEng_GetConfig()->luria_svc.ph_count)	i8PhNo = uvEng_GetConfig()->luria_svc.ph_count;

	/* PH Number 갱신 */
	m_edt_int[0].SetTextToNum(INT8(i8PhNo));
	/* 각종 그리드 컨트롤 갱신 */
	i8PhNo--;
	m_pGridLuriaOverall->Update(i8PhNo);
	m_pGridLuriaLedSource->Update(i8PhNo);
	m_pGridLuriaPHInfo->Update(i8PhNo);
}

/*
 desc : 정보 갱신
 parm : None
 retn : None
*/
VOID CDlgStat::HDDRefresh()
{
	UINT64 u64Total, u64Free;
	DOUBLE dbGB	= 1024.0f * 1024.0f * 1024.0f;

	/* HDD 정보 갱신 */
	if (!uvCmn_GetHDDInfo(u64Total, u64Free, L"C:\\"))
	{
		m_edt_flt[eSTATE_EDT_HDD_TOTAL].SetTextToNum(DOUBLE(0), 3);
		m_edt_flt[eSTATE_EDT_HDD_SPARE].SetTextToNum(DOUBLE(0), 3);
	}
	else
	{
		m_edt_flt[eSTATE_EDT_HDD_TOTAL].SetTextToNum(DOUBLE(u64Total / dbGB), 3);
		m_edt_flt[eSTATE_EDT_HDD_SPARE].SetTextToNum(DOUBLE(u64Free / dbGB), 3);
	}
}
