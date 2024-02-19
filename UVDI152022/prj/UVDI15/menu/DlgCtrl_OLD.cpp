
/*
 desc : Motion & PLC Monitoring
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgCtrl.h"

#include "./ctrl/GridMotor.h"
#include "./ctrl/GridPLC.h"
#include <bitset>


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
CDlgCtrl::CDlgCtrl(UINT32 id, CWnd* parent)
	: CDlgMenu(id, parent)
{
	m_pGridMotor	= NULL;
	m_pGridPLC		= NULL;
	/* 자신의 다이얼로그 ID 설정 */
	m_enDlgID		= ENG_CMDI::en_menu_ctrl;
}

CDlgCtrl::~CDlgCtrl()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgCtrl::DoDataExchange(CDataExchange* dx)
{
	CDlgMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;
	/* groupbox - normal */
	u32StartID	= IDC_CTRL_GRP_MC2;
	for (i=0; i< eCTRL_GRP_MAX; i++)	DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* static - normal */
	u32StartID	= IDC_CTRL_TXT_MC2_DIST;
	for (i=0; i< eCTRL_TXT_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* edit - integer */
	u32StartID	= IDC_CTRL_EDT_VACUUM_FREQ;
	for (i=0; i< eCTRL_EDT_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_edt_txt[i]);
	/* edit - float */
	u32StartID	= IDC_CTRL_EDT_MC2_SPEED;
	for (i=0; i< eCTRL_EDT_FLOAT_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_edt_flt[i]);
	/* button - normal */
	u32StartID	= IDC_CTRL_BTN_RESET_TRIGGER;
	for (i=0; i< eCTRL_BTN_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* button - homing */
	u32StartID	= IDC_CTRL_BTN_HOMING_STAGE_X;
	for (i=0; i< eCTRL_BTN_HOMING_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_btn_hos[i]);
	/* button - homing */
	u32StartID	= IDC_CTRL_BTN_HOMING_PH_1;
	for (i=0; i< eCTRL_BTN_HOMING_PH_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_btn_hop[i]);
	/* button - homing - z axis for camera */
	u32StartID	= IDC_CTRL_BTN_HOMING_ACAM_Z1;
	for (i=0; i< eCTRL_BTN_HOMING_ACAM_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_btn_hoz[i]);
	/* button - reset - z axis for camera */
	u32StartID	= IDC_CTRL_BTN_RESET_ACAM_Z1;
	for (i=0; i< eCTRL_BTN_RESET_ACAM_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_btn_rsz[i]);
	/* button - motion move */
	u32StartID	= IDC_CTRL_BTN_MC2_LEFT;
	for (i=0; i< eCTRL_BTN_MOV_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_btn_mov[i]);
	/* button - Motor All (Stop, Homing, Reset) */
	u32StartID	= IDC_CTRL_BTN_MOTOR_STOP;
	for (i=0; i< eCTRL_BTN_MOTOR_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_btn_mot[i]);
	/* button - PLC */
	u32StartID	= IDC_CTRL_BTN_PLC_PREV;
	for (i=0; i< eCTRL_BTN_PLC_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_btn_plc[i]);
	/* check - Motor Axis */
	u32StartID	= IDC_CTRL_CHK_STAGE_X;
	for (i=0; i< eCTRL_CHK_AXS_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_chk_axs[i]);
	/* check - Motor Axis for Z */
	u32StartID	= IDC_CTRL_CHK_ACAM_Z1;
	for (i=0; i< eCTRL_CHK_ACAM_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_chk_axz[i]);
	/* check - Motor Axis */
	u32StartID	= IDC_CTRL_CHK_PH_1;
	for (i=0; i< eCTRL_CHK_PH_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_chk_mph[i]);
	/* check - Motor Servo */
	u32StartID	= IDC_CTRL_CHK_SERVO_STAGE_X;
	for (i=0; i< eCTRL_CHK_SERVO_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_chk_svs[i]);
	/* check - Motor Servo */
	u32StartID	= IDC_CTRL_CHK_SERVO_PH_1;
	for (i=0; i< eCTRL_CHK_SERVO_PH_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_chk_svp[i]);
	/* check - Motion Moving Type : Absolute or Relative */
	u32StartID	= IDC_CTRL_CHK_MOVE_METHOD_MC2;
	for (i=0; i< eCTRL_CHK_MOVE_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_chk_mvt[i]);
	/* check - Device ON or OFF / Open or Close / etc */
	u32StartID	= IDC_CTRL_CHK_VACUUM_BLOW;
	for (i=0; i< eCTRL_CHK_MAX; i++)	DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgCtrl, CDlgMenu)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_CTRL_BTN_MC2_LEFT, IDC_CTRL_BTN_PLC_UPDATE, OnBtnClick)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_CTRL_CHK_STAGE_X, IDC_CTRL_CHK_BUZZER_ON, OnChkClick)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgCtrl::PreTranslateMessage(MSG* msg)
{
	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgCtrl::OnInitDlg()
{
	/* 컨트롤 초기화 */
	InitCtrl();
	if (!InitGrid())	return FALSE;
	InitValue();

	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgCtrl::OnExitDlg()
{
	CloseGrid();
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgCtrl::OnPaintDlg(CDC *dc)
{

	/* 자식 윈도 호출 */
	CDlgMenu::DrawOutLine();
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgCtrl::OnResizeDlg()
{
}

/*
 desc : 부모 스레드에 의해 주기적으로 호출됨
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgCtrl::UpdatePeriod(UINT64 tick, BOOL is_busy)
{
	LPG_PMRW pstPLC	= uvEng_ShMem_GetPLCExt();

	if (m_pGridMotor)	m_pGridMotor->UpdateValue();
	/* Status : Hor Air Power, Vacuum Power, Chiller Power, Vacuum Control, Shutter Control */
	UpdateControlError(pstPLC);
	UpdateServoStatus();
	UpdateCheckStatus();
	/* Control Enable or Disable */
	UpdateControl(tick, is_busy);
}

/*
 desc : 컨트롤
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgCtrl::UpdateControl(UINT64 tick, BOOL is_busy)
{
	if (m_pGridMotor)	m_pGridMotor->UpdateValue();
	/* Vacuum Frequency */
	m_edt_txt[eCTRL_EDT_VACUUM_FREQ].SetRedraw(FALSE);
	TCHAR tzVac[64] = {NULL};
	swprintf_s(tzVac, 64, L"%d kPa / %.2f Hz",
			   uvCmn_MCQ_GetVacuumPress(), uvCmn_MCQ_GetVacuumFreq() / 100.0f);
	m_edt_txt[eCTRL_EDT_VACUUM_FREQ].SetTextToStr(tzVac);
	m_edt_txt[eCTRL_EDT_VACUUM_FREQ].SetRedraw(TRUE);
}

/*
 desc : Chiller Power / Vacuum Power / Hot Air Power / Vacuum Control / Shutter Control
 parm : None
 retn : None
*/
VOID CDlgCtrl::UpdateCheckStatus()
{
	LPG_PMRW pstPLC	= uvEng_ShMem_GetPLCExt();

	
	m_chk_ctl[eCTRL_CHK_VACUUM_BLOW		].SetCheck(!pstPLC->r_vacuum_status);/* Vacuum Control (Blow or Vacuum) */
	m_chk_ctl[eCTRL_CHK_VACUUM_VAC		].SetCheck(pstPLC->r_vacuum_status);
	m_chk_ctl[eCTRL_CHK_SHUTTER_CLOSE	].SetCheck(!pstPLC->r_shutter_status);/* Shutter Control (CLose or Open) */
	m_chk_ctl[eCTRL_CHK_SHUTTER_OPEN	].SetCheck(pstPLC->r_shutter_status);
	m_chk_ctl[eCTRL_CHK_CHILLER_OFF		].SetCheck(!pstPLC->r_chiller_controller_running);/* Chiller Power (Off / On) */
	m_chk_ctl[eCTRL_CHK_CHILLER_ON		].SetCheck(pstPLC->r_chiller_controller_running);
	m_chk_ctl[eCTRL_CHK_VACUUM_OFF		].SetCheck(!pstPLC->r_vacuum_controller_running);/* Vacuum Power (Off / On) */
	m_chk_ctl[eCTRL_CHK_VACUUM_ON		].SetCheck(pstPLC->r_vacuum_controller_running);
	m_chk_ctl[eCTRL_CHK_HOTAIR_OFF		].SetCheck(!pstPLC->r_temp_controller_running);/* Hot Air Power (Off / On) */
	m_chk_ctl[eCTRL_CHK_HOTAIR_ON		].SetCheck(pstPLC->r_temp_controller_running);
	m_chk_ctl[eCTRL_CHK_BUZZER_OFF		].SetCheck(!pstPLC->r_signal_tower_buzzer_1);/* Buzzer (Off / On) */
	m_chk_ctl[eCTRL_CHK_BUZZER_ON		].SetCheck(pstPLC->r_signal_tower_buzzer_1);
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgCtrl::InitCtrl()
{
	// by sysandj : Resize UI
	CResizeUI clsResizeUI;
	// by sysandj : Resize UI

	INT32 i = 0;
	//INT64 i64MinMax[2]	= { -200, 0 };
	DOUBLE dbMin[eCTRL_EDT_FLOAT_MAX]		= {   0.0001f,    0.0001f, -15.0000f };
	DOUBLE dbMax[eCTRL_EDT_FLOAT_MAX]		= { 499.9999f, 1650.9999f, 15.0000f };
	/* group box */
	for (i = 0; i < eCTRL_GRP_MAX; i++)
	{
		m_grp_ctl[i].SetLogFont(&g_lf[eFONT_LEVEL2_BOLD]);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_grp_ctl[i]);
		// by sysandj : Resize UI
	}
	/* static - Normal */
	for (i=0; i< eCTRL_TXT_MAX; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_txt_ctl[i]);
		// by sysandj : Resize UI
	}
	/* edit - integer */
	for (i=0; i< eCTRL_EDT_MAX; i++)
	{
		m_edt_txt[i].SetEditFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_edt_txt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_txt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_txt[i].SetReadOnly(TRUE);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_edt_txt[i]);
		// by sysandj : Resize UI
	}
	/* edit - float */
	for (i=0; i< eCTRL_EDT_FLOAT_MAX; i++)
	{
		m_edt_flt[i].SetEditFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_edt_flt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_flt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_flt[i].SetReadOnly(TRUE);
		m_edt_flt[i].SetInputType(ENM_DITM::en_float);
		if (i < eCTRL_EDT_FLOAT_PLC_DIST)	m_edt_flt[i].SetInputSign(ENM_MNUS::en_unsign);
		else		m_edt_flt[i].SetInputSign(ENM_MNUS::en_sign);
		m_edt_flt[i].SetMinMaxNum(dbMin[i], dbMax[i]);
		m_edt_flt[i].SetInputPoint(4);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_edt_flt[i]);
		// by sysandj : Resize UI
	}
	/* button - normal */
	for (i=0; i< eCTRL_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
		// by sysandj : Resize UI
	}
#if 1	/* UTRONIX equipment operates, but PODIS 4th equipment does not (4호기 (태성이가 만든 것) 것은 안됨) */
	m_btn_ctl[eCTRL_BTN_RESET_TRIGGER].EnableWindow(FALSE);
#endif
#if 1	/* The safety lock operation is not supported */
	m_btn_ctl[eCTRL_BTN_RESET_SAFELOCK].EnableWindow(FALSE);
#endif
	/* button - homing - stage */
	for (i=0; i< eCTRL_BTN_HOMING_MAX; i++)
	{
		m_btn_hos[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_hos[i].SetBgColor(g_clrBtnColor);
		m_btn_hos[i].SetTextColor(g_clrBtnTextColor);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_hos[i]);
		// by sysandj : Resize UI
	}
	/* button - homing - photohead */
	for (i=0; i< eCTRL_BTN_HOMING_PH_MAX; i++)
	{
		m_btn_hop[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_hop[i].SetBgColor(g_clrBtnColor);
		m_btn_hop[i].SetTextColor(g_clrBtnTextColor);
		if (i >= uvEng_GetConfig()->luria_svc.ph_count)	m_btn_hop[i].EnableWindow(FALSE);
		//if (0x01 != uvEng_GetConfig()->luria_svc.z_drive_type)	m_btn_hop[i].EnableWindow(FALSE);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_hop[i]);
		// by sysandj : Resize UI
	}
	/* button - homing / reset - z axis for camera */
	for (i = 0; i < eCTRL_BTN_HOMING_ACAM_MAX; i++)
	{
		m_btn_hoz[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_hoz[i].SetBgColor(g_clrBtnColor);
		m_btn_hoz[i].SetTextColor(g_clrBtnTextColor);
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_hoz[i]);
		// by sysandj : Resize UI
	}

	for (i = 0; i < eCTRL_BTN_RESET_ACAM_MAX; i++)
	{
		m_btn_rsz[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_rsz[i].SetBgColor(g_clrBtnColor);
		m_btn_rsz[i].SetTextColor(g_clrBtnTextColor);
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_rsz[i]);
		// by sysandj : Resize UI
	}
	/* button - motion move */
	for (i=0; i< eCTRL_BTN_MOV_MAX; i++)
	{
		m_btn_mov[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_mov[i].SetBgColor(g_clrBtnColor);
		m_btn_mov[i].SetTextColor(g_clrBtnTextColor);
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_mov[i]);
		// by sysandj : Resize UI
	}
	/* button - Motor All / PLC */
	for (i = 0; i < eCTRL_BTN_MOTOR_MAX; i++)
	{
		m_btn_mot[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_mot[i].SetBgColor(g_clrBtnColor);
		m_btn_mot[i].SetTextColor(g_clrBtnTextColor);
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_mot[i]);
		// by sysandj : Resize UI
	}

	for (i = 0; i < eCTRL_BTN_PLC_MAX; i++)
	{
		m_btn_plc[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_plc[i].SetBgColor(g_clrBtnColor);
		m_btn_plc[i].SetTextColor(g_clrBtnTextColor);
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_plc[i]);
		// by sysandj : Resize UI
	}
	/* Prev Button Disable */
	m_btn_plc[eCTRL_BTN_PLC_PREV].EnableWindow(FALSE);

	/* check - Motor Axis */
	for (i=0; i< eCTRL_CHK_AXS_MAX; i++)
	{
		m_chk_axs[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_chk_axs[i].SetBgColor(RGB(255, 255, 255));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_chk_axs[i]);
		// by sysandj : Resize UI
	}
	/* check - Motor Axis for Z */
	for (i=0; i< eCTRL_CHK_ACAM_MAX; i++)
	{
		m_chk_axz[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_chk_axz[i].SetBgColor(RGB(255, 255, 255));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_chk_axz[i]);
		// by sysandj : Resize UI
	}
	/* check - Motor Photohead */
	for (i=0; i< eCTRL_CHK_PH_MAX; i++)
	{
		m_chk_mph[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_chk_mph[i].SetBgColor(RGB(255, 255, 255));
		if (i >= uvEng_GetConfig()->luria_svc.ph_count)	m_chk_mph[i].EnableWindow(FALSE);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_chk_mph[i]);
		// by sysandj : Resize UI
	}
	/* check - Motor Servo - stage */
	for (i=0; i< eCTRL_CHK_SERVO_MAX; i++)
	{
		m_chk_svs[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_chk_svs[i].SetBgColor(RGB(255, 255, 255));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_chk_svs[i]);
		// by sysandj : Resize UI
	}
	/* check - Motor Servo - photohead */
	for (i=0; i< eCTRL_CHK_SERVO_PH_MAX; i++)
	{
		m_chk_svp[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_chk_svp[i].SetBgColor(RGB(255, 255, 255));
#if 0	/* 본 장비에서 지원 함 */
		if (i >= uvEng_GetConfig()->luria_svc.ph_count)	m_chk_svp[i].EnableWindow(FALSE);
#else
		m_chk_svp[i].EnableWindow(FALSE);
#endif

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_chk_svp[i]);
		// by sysandj : Resize UI
	}
	/* check - Motion Moving Type : Absolute or Relative */
	for (i=0; i< eCTRL_CHK_MOVE_MAX; i++)
	{
		m_chk_mvt[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_chk_mvt[i].SetBgColor(RGB(255, 255, 255));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_chk_mvt[i]);
		// by sysandj : Resize UI
	}
	/* check - Device ON or OFF / Open or Close / etc */
	for (i=0; i< eCTRL_CHK_MAX; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_chk_ctl[i].SetBgColor(RGB(255, 255, 255));
		m_chk_ctl[i].SetCheck(1);	/* 무조건 모두 선택 */

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_chk_ctl[i]);
		// by sysandj : Resize UI
	}
}

/*
 desc : GridControl 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgCtrl::InitGrid()
{
	/* Motor Grid */
	m_pGridMotor= new CGridMotor(m_hWnd, m_grp_ctl[eCTRL_GRP_MOTOR_POS].GetSafeHwnd());
	ASSERT(m_pGridMotor);
	m_pGridMotor->InitGrid();
	/* PLC Grid */
	m_pGridPLC	= new CGridPLC(m_hWnd, m_grp_ctl[eCTRL_GRP_PLC_IO].GetSafeHwnd());
	ASSERT(m_pGridPLC);
	m_pGridPLC->InitGrid();

	return TRUE;
}

/*
 desc : GridControl 해제
 parm : None
 retn : None
*/
VOID CDlgCtrl::CloseGrid()
{
	if (m_pGridMotor)	delete m_pGridMotor;
	if (m_pGridPLC)		delete m_pGridPLC;
}

/*
 desc : 초기 값 설정
 parm : None
 retn : None
*/
VOID CDlgCtrl::InitValue()
{
	/* Motion Distance / Speed */
	m_edt_flt[eCTRL_EDT_FLOAT_MC2_SPEED].SetTextToNum(DOUBLE(uvEng_GetConfig()->mc2_svc.move_velo), 4);
	m_edt_flt[eCTRL_EDT_FLOAT_MC2_DIST].SetTextToNum(DOUBLE(uvEng_GetConfig()->mc2_svc.move_dist), 4);
}

/*
 desc : 일반 버튼 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgCtrl::OnBtnClick(UINT32 id)
{
	switch (id)
	{
	case IDC_CTRL_BTN_PLC_PREV			:	SetPageRead(0x00);	break;
	case IDC_CTRL_BTN_PLC_UPDATE		:	SetPageRead(0x02);	break;
	case IDC_CTRL_BTN_PLC_NEXT			:	SetPageRead(0x01);	break;
	case IDC_CTRL_BTN_PH_INIT			:	SetLuriaPHInited();	break;
	/* Motion Moving for stage */
	case IDC_CTRL_BTN_MC2_LEFT			:	SetMotionMoving(ENG_MDMD::en_move_left);	break;
	case IDC_CTRL_BTN_MC2_RIGHT			:	SetMotionMoving(ENG_MDMD::en_move_right);	break;
	case IDC_CTRL_BTN_MC2_UP			:	SetMotionMoving(ENG_MDMD::en_move_up);		break;
	case IDC_CTRL_BTN_MC2_DOWN			:	SetMotionMoving(ENG_MDMD::en_move_down);	break;
	/* Motion Moving for camera z */
	case IDC_CTRL_BTN_PLC_UP			:	SetPLCMoving(0x01);	break;
	case IDC_CTRL_BTN_PLC_DOWN			:	SetPLCMoving(0x02);	break;
	/* Motion Homing - Each */
	case IDC_CTRL_BTN_HOMING_STAGE_X	:	SetMotionHoming(ENG_MMDI::en_stage_x);		break;
	case IDC_CTRL_BTN_HOMING_STAGE_Y	:	SetMotionHoming(ENG_MMDI::en_stage_y);		break;
	case IDC_CTRL_BTN_HOMING_ACAM_X1	:	SetMotionHoming(ENG_MMDI::en_align_cam1);	break;
	case IDC_CTRL_BTN_HOMING_ACAM_X2	:	SetMotionHoming(ENG_MMDI::en_align_cam2);	break;
	case IDC_CTRL_BTN_HOMING_PH_1		:
	case IDC_CTRL_BTN_HOMING_PH_2		:
	case IDC_CTRL_BTN_HOMING_PH_3		:
	case IDC_CTRL_BTN_HOMING_PH_4		:
	case IDC_CTRL_BTN_HOMING_PH_5		:
		if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
		{
			switch (id)
			{
			case IDC_CTRL_BTN_HOMING_PH_1	:
			case IDC_CTRL_BTN_HOMING_PH_2	:
			case IDC_CTRL_BTN_HOMING_PH_3	:
			case IDC_CTRL_BTN_HOMING_PH_4	:
			case IDC_CTRL_BTN_HOMING_PH_5	:	SetPhHoming(id - IDC_CTRL_BTN_HOMING_PH_1 + 1);	break;
			}
		}
		else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
		{
			switch (id)
			{
			case IDC_CTRL_BTN_HOMING_PH_1		:	SetMotionHoming(ENG_MMDI::en_axis_ph1);		break;
			case IDC_CTRL_BTN_HOMING_PH_2		:	SetMotionHoming(ENG_MMDI::en_axis_ph2);		break;
			}
		}
		break;
	case IDC_CTRL_BTN_MOTOR_HOMING		:	SetMotionHoming(ENG_MMDI::en_all);			break;
	case IDC_CTRL_BTN_MOTOR_STOP		:	uvEng_MC2_SendDevStoppedAll();				break;
	case IDC_CTRL_BTN_MOTOR_RESET		:	uvEng_MC2_SendDevFaultResetAll();			break;
	/* Motor Homing - z axis for camera */
	case IDC_CTRL_BTN_HOMING_ACAM_Z1	:
	case IDC_CTRL_BTN_HOMING_ACAM_Z2	:
	case IDC_CTRL_BTN_HOMING_ACAM_Z_ALL	:	SetACamZHoming(id - IDC_CTRL_BTN_HOMING_ACAM_Z1 + 1);	break;
	/* Motor Reset - z axis for camera */
	case IDC_CTRL_BTN_RESET_ACAM_Z1		:
	case IDC_CTRL_BTN_RESET_ACAM_Z2		:
	case IDC_CTRL_BTN_RESET_ACAM_Z_ALL	:	SetACamZReset(id - IDC_CTRL_BTN_RESET_ACAM_Z1 + 1);		break;
	/* Trigger Power Reset */
	case IDC_CTRL_BTN_RESET_TRIGGER		:	SetTriggerReset();	break;
	}
}

/*
 desc : 일반 체크 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgCtrl::OnChkClick(UINT32 id)
{
	UINT8 i		= 0x00;
	BOOL bOnOff	= FALSE;
	BOOL bEnable= FALSE, bEnableZ = m_chk_mvt[1].GetCheck() ? TRUE : FALSE;

	switch (id)
	{
	/* Move Type */
	case IDC_CTRL_CHK_MOVE_METHOD_MC2	:
	case IDC_CTRL_CHK_MOVE_METHOD_ACAM	:
		i= UINT8(id - IDC_CTRL_CHK_MOVE_METHOD_MC2);
		if (m_chk_mvt[i].GetCheck())	m_chk_mvt[i].SetWindowTextW(L"MODE\n[ R ]");
		else							m_chk_mvt[i].SetWindowTextW(L"MODE\n[ A ]");
		break;
	/* Select stage or align camera */
	case IDC_CTRL_CHK_STAGE_X			:
	case IDC_CTRL_CHK_STAGE_Y			:
	case IDC_CTRL_CHK_ACAM_X1			:
	case IDC_CTRL_CHK_ACAM_X2			:
		for (i=0x00; i< eCTRL_CHK_AXS_MAX; i++)	m_chk_axs[i].SetCheck(0);
		for (i = 0x00; i < eCTRL_CHK_PH_MAX; i++)	m_chk_mph[i].SetCheck(0);
		if (m_chk_mvt[eCTRL_CHK_MOVE_METHOD_ACAM].GetCheck())
		{
			m_chk_axs[id - IDC_CTRL_CHK_STAGE_X].SetCheck(1);
		}
		else
		{
			if (id == IDC_CTRL_CHK_STAGE_Y)	bEnable	= TRUE;
			m_btn_mov[eCTRL_BTN_MOV_MC2_LEFT	].EnableWindow(!bEnable);	/* Move : Left */
			m_btn_mov[eCTRL_BTN_MOV_MC2_RIGHT	].EnableWindow(!bEnable);	/* Move : Right */
			m_btn_mov[eCTRL_BTN_MOV_MC2_UP		].EnableWindow(bEnable);	/* Move : Up */
			m_btn_mov[eCTRL_BTN_MOV_MC2_DOWN	].EnableWindow(bEnable);	/* Move : Down */
			// by sys&j : out of range
			//m_edt_flt[4].SetEnableKBDN();			/* Speed */
			m_edt_flt[eCTRL_EDT_FLOAT_MC2_SPEED].SetEnableKBDN();			/* Speed */
			m_chk_axs[id - IDC_CTRL_CHK_STAGE_X].SetCheck(1);
		}
		break;
	/* Select align camera for z axis */
	case IDC_CTRL_CHK_ACAM_Z1			:
	case IDC_CTRL_CHK_ACAM_Z2			:
		for (i=0x00; i< eCTRL_CHK_ACAM_MAX; i++)	m_chk_axz[i].SetCheck(0);
		m_chk_axz[id - IDC_CTRL_CHK_ACAM_Z1].SetCheck(1);
		break;
	/* PHOTOHEAD */
	case IDC_CTRL_CHK_PH_1				:
	case IDC_CTRL_CHK_PH_2				:
	case IDC_CTRL_CHK_PH_3				:
	case IDC_CTRL_CHK_PH_4				:
	case IDC_CTRL_CHK_PH_5				:
		for (i=0x00; i< eCTRL_CHK_AXS_MAX; i++)	m_chk_axs[i].SetCheck(0);
		for (i=0x00; i< eCTRL_CHK_PH_MAX; i++)	m_chk_mph[i].SetCheck(0);
		m_btn_mov[eCTRL_BTN_MOV_MC2_LEFT].EnableWindow(FALSE);	/* Move : Left */
		m_btn_mov[eCTRL_BTN_MOV_MC2_RIGHT].EnableWindow(FALSE);	/* Move : Right */
		//m_edt_flt[eCTRL_EDT_FLOAT_MC2_SPEED].SetDisableKBDN();		/* Speed */
		m_edt_flt[eCTRL_EDT_FLOAT_MC2_SPEED].SetEnableKBDN();		/* Speed */
		m_chk_mph[id - IDC_CTRL_CHK_PH_1].SetCheck(1);
		break;
	/* Servo Control (On or Off) */
	case IDC_CTRL_CHK_SERVO_STAGE_X		:
	case IDC_CTRL_CHK_SERVO_STAGE_Y		:
	case IDC_CTRL_CHK_SERVO_ACAM_X1		:
	case IDC_CTRL_CHK_SERVO_ACAM_X2		:
		bOnOff	= m_chk_svs[id-IDC_CTRL_CHK_SERVO_STAGE_X].GetCheck() ? TRUE : FALSE;
		uvEng_MC2_SendDevLocked(ENG_MMDI(id - IDC_CTRL_CHK_SERVO_STAGE_X), bOnOff); break;
	case IDC_CTRL_CHK_SERVO_PH_1		:
	case IDC_CTRL_CHK_SERVO_PH_2		:
		if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
		{
			bOnOff	= m_chk_svs[id-IDC_CTRL_CHK_SERVO_STAGE_X].GetCheck() ? TRUE : FALSE;
			uvEng_MC2_SendDevLocked(ENG_MMDI(id - IDC_CTRL_CHK_SERVO_STAGE_X), bOnOff);
		}
		break;
	/* Vacuum Blow or Vac */
	case IDC_CTRL_CHK_VACUUM_BLOW	:
	case IDC_CTRL_CHK_VACUUM_VAC	:	SetVacuumCtrl(id - IDC_CTRL_CHK_VACUUM_BLOW);		break;
	/* Shutter Open or Close */
#if 1
	case IDC_CTRL_CHK_SHUTTER_CLOSE	:
	case IDC_CTRL_CHK_SHUTTER_OPEN	:	SetShutterCtrl(id - IDC_CTRL_CHK_SHUTTER_CLOSE);	break;
#else
	case IDC_CTRL_CHK_SHUTTER_CLOSE	:	uvEng_MCQ_LedVacuumShutter(0x00, 0x01, 0x00);	break;
	case IDC_CTRL_CHK_SHUTTER_OPEN	:	uvEng_MCQ_LedVacuumShutter(0x00, 0x01, 0x01);	break;
#endif
	/* Chiller On or Off */
	case IDC_CTRL_CHK_CHILLER_OFF	:
	case IDC_CTRL_CHK_CHILLER_ON	:	SetChillerPower(id - IDC_CTRL_CHK_CHILLER_OFF);	break;
	/* Vacuum On or Off */
	case IDC_CTRL_CHK_VACUUM_OFF	:
	case IDC_CTRL_CHK_VACUUM_ON		:	SetVacuumPower(id - IDC_CTRL_CHK_VACUUM_OFF);	break;
	/* Buzzer On or Off */
	case IDC_CTRL_CHK_BUZZER_OFF	:
	case IDC_CTRL_CHK_BUZZER_ON		:	SetBuzzerCtrl(id - IDC_CTRL_CHK_BUZZER_OFF);	break;
	}
}

/*
 desc : Vacuum On or Off
 parm : enable	- [in]  0:Off, 1:On
 retn : None
*/
VOID CDlgCtrl::SetVacuumCtrl(UINT8 enable)
{
	/* Vacuum 과 Shutter 동시 제어 */
	uvEng_MCQ_SetVacuumOnOff(enable);
}

/*
 desc : Shutter Open or Close
 parm : enable	- [in]  0:Closed, 1:Opened
 retn : None
*/
VOID CDlgCtrl::SetShutterCtrl(UINT8 enable)
{
	/* Vacuum 과 Shutter 동시 제어 */
	uvEng_MCQ_SetShutterOnOff(enable);
}

/*
 desc : Chiller Off or On
 parm : enable	- [in]  0:Closed, 1:Opened
 retn : None
*/
VOID CDlgCtrl::SetChillerPower(UINT8 enable)
{
	/* Vacuum 과 Shutter 동시 제어 */
	uvEng_MCQ_ChillerRunStop(enable);
}

/*
 desc : Vacuum Off or On
 parm : enable	- [in]  0:Closed, 1:Opened
 retn : None
*/
VOID CDlgCtrl::SetVacuumPower(UINT8 enable)
{
	/* Vacuum 과 Shutter 동시 제어 */
	uvEng_MCQ_VacuumRunStop(enable);
}

/*
 desc : Buzzer On or Off
 parm : enable	- [in]  0:Off, 1:On
 retn : None
*/
VOID CDlgCtrl::SetBuzzerCtrl(UINT8 enable)
{
	/* Vacuum 과 Shutter 동시 제어 */
	uvEng_MCQ_SetBuzzerOnOff(enable);
}

/*
 desc : PLC Read 영역 페이지 전환
 parm : flag	- [in]  0x00 - 이전, 0x01 - 다음, 0x02 - 갱신
 retn : None
*/
VOID CDlgCtrl::SetPageRead(UINT8 flag)
{
	/* 이전 페이지로 이동하는 경우 */
	if (0x00 == flag)		m_pGridPLC->SetPagePrev();
	/* 다음 페이지로 이동하는 경우 */
	else if (0x01 == flag)	m_pGridPLC->SetPageNext();

	/* Prev & Next Button Enable 처리 */
	m_btn_plc[eCTRL_BTN_PLC_PREV].EnableWindow(!m_pGridPLC->IsPageFirst());
	m_btn_plc[eCTRL_BTN_PLC_NEXT].EnableWindow(!m_pGridPLC->IsPageLast());

	/* 바로 갱신 */
	// 기존
	//m_pGridPLC->UpdatePeriod();
	// by sysandj : 안보혁수석님 sample 코드
	// by sysandj : philhmi 통신
	STG_PP_P2C_IO_STATUS		stStatus;
	STG_PP_P2C_IO_STATUS_ACK	stStatusAck;
	stStatus.Reset();
	stStatusAck.Reset();
	std::vector <std::vector <bool>>	vInputGroup;
	std::vector <std::vector <bool>>	vOutputGroup;
	if (uvEng_Philhmi_Send_P2C_IO_STATUS(stStatus, stStatusAck))
	{
		for (int i = 0; i < stStatusAck.byInputCount; i++)
		{
			std::vector <bool>	vInput;
			std::bitset<16>		bits(stStatusAck.usInputData[i]);
			for (int i = 0; i < 16; i++) 
			{
				vInput.push_back(bits[i]); // 각 비트의 값을 bool 벡터에 추가
			}

			vInputGroup.push_back(vInput);
		}

		for (int i = 0; i < stStatusAck.byOutputCount; i++)
		{
			std::vector <bool>	vOutput;
			std::bitset<16>		bits(stStatusAck.usOutputData[i]);
			for (int i = 0; i < 16; i++)
			{
				vOutput.push_back(bits[i]); // 각 비트의 값을 bool 벡터에 추가
			}

			vOutputGroup.push_back(vOutput);
		}
	}

	m_pGridPLC->UpdatePeriod(vInputGroup);

	vInputGroup.clear();
	vOutputGroup.clear();
	// by sysandj : 안보혁수석님 sample 코드
}

/*
 desc : Motion 이동
 parm : direct	- [in]  ENG_MDMD 값
 retn : None
*/
VOID CDlgCtrl::SetMotionMoving(ENG_MDMD direct)
{
	DOUBLE dbMotorSpeed	= 0.0f;
	DOUBLE dbMotorDist	= 0.0f;
	DOUBLE dbMotorPos	= 0.0f;	/* StageX, StageY, ACam X1, ACamX2 */
	ENG_MMDI enDrvID	= ENG_MMDI::en_axis_none;

	/* 현재 선택된 드라이브 정보 얻기 */
	if		(m_chk_axs[eCTRL_CHK_AXS_STAGE_X].GetCheck())	enDrvID = ENG_MMDI::en_stage_x;
	else if (m_chk_axs[eCTRL_CHK_AXS_STAGE_Y].GetCheck())	enDrvID = ENG_MMDI::en_stage_y;

	else if (m_chk_mph[eCTRL_CHK_PH_1].GetCheck())			enDrvID = ENG_MMDI::en_axis_ph1;
	else if (m_chk_mph[eCTRL_CHK_PH_2].GetCheck())			enDrvID = ENG_MMDI::en_axis_ph2;
	
	else if (m_chk_axs[eCTRL_CHK_AXS_ACAM_X1].GetCheck())	enDrvID = ENG_MMDI::en_align_cam1;
	else if (m_chk_axs[eCTRL_CHK_AXS_ACAM_X2].GetCheck())	enDrvID	= ENG_MMDI::en_align_cam2;
	if (enDrvID == ENG_MMDI::en_axis_none)	return;

	/* 현재 Motion 위치 얻기 */
	dbMotorPos	= uvCmn_MC2_GetDrvAbsPos(enDrvID);
	/* 입력된 속도 및 이동 거리 얻기 */
	dbMotorSpeed	= m_edt_flt[eCTRL_EDT_FLOAT_MC2_SPEED].GetTextToDouble();	/* Speed */
	dbMotorDist		= m_edt_flt[eCTRL_EDT_FLOAT_MC2_DIST].GetTextToDouble();	/* Distance */

	/* 상대 이동 여부에 따라 */
	if (m_chk_mvt[eCTRL_BTN_MOTOR_STOP].GetCheck())
	{
		switch (enDrvID)
		{
		case ENG_MMDI::en_stage_x		:
			if (direct == ENG_MDMD::en_move_left)	dbMotorPos	+= dbMotorDist;			/* left */
			else									dbMotorPos	-= dbMotorDist;	break;	/* right */
// 		case ENG_MMDI::en_stage_y		:
// 			if (direct == ENG_MDMD::en_move_up)		dbMotorPos	+= dbMotorDist;			/* up */
// 			else									dbMotorPos	-= dbMotorDist;	break;	/* down */
		case ENG_MMDI::en_stage_y:
		case ENG_MMDI::en_axis_ph1:
		case ENG_MMDI::en_axis_ph2:
			if (direct == ENG_MDMD::en_move_up)		dbMotorPos += dbMotorDist;			/* up */
			else									dbMotorPos -= dbMotorDist;	break;	/* down */
		case ENG_MMDI::en_align_cam1	:
		case ENG_MMDI::en_align_cam2	:
			if (direct == ENG_MDMD::en_move_left)	dbMotorPos	-= dbMotorDist;			/* left */
			else									dbMotorPos	+= dbMotorDist;	break;	/* right */
		}
	}
	/* 절대 이동인 경우 */
	else
	{
		dbMotorPos	= dbMotorDist;
	}

	/* 노광 이동 위치 값이 Min / Max 벗어나면 안되므로, Min / Mxwax 값으로 자동 변경 */
	if (dbMotorPos < uvEng_GetConfig()->mc2_svc.min_dist[UINT8(enDrvID)])
	{
		dbMotorPos	= uvEng_GetConfig()->mc2_svc.min_dist[UINT8(enDrvID)];
	}
	if (dbMotorPos > uvEng_GetConfig()->mc2_svc.max_dist[UINT8(enDrvID)])
	{
		dbMotorPos	= uvEng_GetConfig()->mc2_svc.max_dist[UINT8(enDrvID)];
	}

	/* 절대 위치로 이동 */
	uvEng_MC2_SendDevAbsMove(enDrvID, dbMotorPos, dbMotorSpeed);
}

/*
 desc : Align Camera Z Axis 이동
 parm : up_down	- [in]  0x01:Up, 0x02:Down 값
 retn : None
*/
VOID CDlgCtrl::SetPLCMoving(UINT8 up_down)
{
	UINT8 u8ACamID = m_chk_axz[eCTRL_CHK_ACAM_Z1].GetCheck() == 1 ? 0x01 : 0x02;
	uvEng_MCQ_SetACamMovePosZ(u8ACamID,
							  (UINT8)m_chk_mvt[eCTRL_CHK_MOVE_METHOD_ACAM].GetCheck(),	/* 0:Absolute, 1:Relative */
							  m_edt_flt[eCTRL_EDT_FLOAT_PLC_DIST].GetTextToDouble(),
							  up_down);
}

/*
 desc : Button Error
		즉, Trigger Board 통신 에러 (전원 리셋), 안전락 (EMU) 해제
 parm : data	- [in]  PLC Memory Data
 retn : None
*/
VOID CDlgCtrl::UpdateControlError(LPG_PMRW data)
{
	COLORREF clrBg[2]	= { RGB(255, 128, 0), g_clrBtnColor };

	/* Trigger Board */
	if (clrBg[data->r_trigger_board_power_line] != m_btn_ctl[eCTRL_BTN_RESET_TRIGGER].GetBgColor())
	{
		m_btn_ctl[eCTRL_BTN_RESET_TRIGGER].SetRedraw(FALSE);
		m_btn_ctl[eCTRL_BTN_RESET_TRIGGER].SetBgColor(clrBg[data->r_trigger_board_power_line]);
		m_btn_ctl[eCTRL_BTN_RESET_TRIGGER].SetRedraw(TRUE);
		m_btn_ctl[eCTRL_BTN_RESET_TRIGGER].Invalidate();
	}
#if 0	/* Safety Status 값은 현재 PLC에서 받아올 수 없음 */
	/* Safety Lock */
	if (clrBg[data->r_safety_line_power] != m_btn_ctl[eCTRL_BTN_RESET_TRIGGER].GetBgColor())
	{
		m_btn_ctl[eCTRL_BTN_RESET_SAFELOCK].SetRedraw(FALSE);
		m_btn_ctl[eCTRL_BTN_RESET_SAFELOCK].SetBgColor(clrBg[data->r_safety_line_power]);
		m_btn_ctl[eCTRL_BTN_RESET_SAFELOCK].SetRedraw(TRUE);
		m_btn_ctl[eCTRL_BTN_RESET_SAFELOCK].Invalidate();
	}
#endif
}

/*
 desc : Servo Status 갱신
 parm : None
 retn : None
*/
VOID CDlgCtrl::UpdateServoStatus()
{
	UINT8 i			= 0x00;
	INT32 i32Servo	= 0;
	LPG_CMSI pstCfg	= &uvEng_GetConfig()->mc2_svc;
	
	for (; i<pstCfg->drive_count; i++)
	{
		i32Servo = uvEng_ShMem_GetMC2()->IsServoOff(pstCfg->axis_id[i]) ? 0 : 1;
		if (i32Servo != m_chk_svs[i].GetCheck())
		{
			m_chk_svs[i].SetRedraw(FALSE);
			m_chk_svs[i].SetCheck(i32Servo);
			m_chk_svs[i].SetRedraw(TRUE);
			m_chk_svs[i].Invalidate();
		}
	}
#if 0	/* 본 장비에서 지원함 */
	/* Photohead Servo 처리 */
#endif
}

/*
 desc : Motion Homing
 parm : drv_id	- [in]  Drive ID
 retn : None
*/
VOID CDlgCtrl::SetMotionHoming(ENG_MMDI drv_id)
{
	if (drv_id == ENG_MMDI::en_all)	uvEng_MC2_SendDevHomingAll();
	else							uvEng_MC2_SendDevHoming(drv_id);
}

/*
 desc : Photohead Homing
 parm : drv_id	- [in]  Drive ID (1-based)
 retn : None
*/
VOID CDlgCtrl::SetPhHoming(UINT8 drv_id)
{
	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		if (drv_id)	uvEng_Luria_ReqSetMotorPositionInit(drv_id);
		else		uvEng_Luria_ReqSetMotorPositionInitAll();
	}
}

/*
 desc : Homing for Align Camera Z Axis
 parm : cam_id	- [in]  Align Camera ID
 retn : None
*/
VOID CDlgCtrl::SetACamZHoming(UINT8 cam_id)
{
	uvEng_MCQ_SetACamHomingZAxis(cam_id);
}

/*
 desc : Reset for Align Camera Z Axis
 parm : cam_id	- [in]  Align Camera ID
 retn : None
*/
VOID CDlgCtrl::SetACamZReset(UINT8 cam_id)
{
	uvEng_MCQ_SetACamResetZAxis(cam_id);
}

/*
 desc : Trigger Power Reset
 parm : None
 retn : None
*/
VOID CDlgCtrl::SetTriggerReset()
{
	uvEng_MCQ_TriggerReset();
}

/*
 desc : Luria PH HW Inited
 parm : None
 retn : None
*/
VOID CDlgCtrl::SetLuriaPHInited()
{
	uvEng_Luria_ReqSetHWInit();
}