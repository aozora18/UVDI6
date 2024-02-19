
/*
 desc : Log Files & Contents
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgConf_OLD.h"

#include "../mesg/DlgMesg.h"

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgConf::CDlgConf(UINT32 id, CWnd* parent)
	: CDlgMenu(id, parent)
{
	/* 자신의 다이얼로그 ID 설정 */
	m_enDlgID = ENG_CMDI::en_menu_conf;
}

CDlgConf::~CDlgConf()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgConf::DoDataExchange(CDataExchange* dx)
{
	CDlgMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* groupbox - normal */
	u32StartID = IDC_CONF_GRP_COMMON;
	for (i = 0; i < eCONF_GRP_MAX; i++)		DDX_Control(dx, u32StartID + i, m_grp_ctl[i]);
	/* button - normal */
	u32StartID = IDC_CONF_BTN_FILE_SELECT;
	for (i = 0; i < eCONF_BTN_MAX; i++)		DDX_Control(dx, u32StartID + i, m_btn_ctl[i]);
	/* checkbox - normal */
	u32StartID = IDC_CONF_CHK_LAMP_TYPE_1;
	for (i = 0; i < eCONF_CHK_MAX; i++)		DDX_Control(dx, u32StartID + i, m_chk_ctl[i]);
	/* text - text */
	u32StartID = IDC_CONF_TXT_ACAM_SPEC_HORZ;
	for (i = 0; i < eCONF_TXT_MAX; i++)		DDX_Control(dx, u32StartID + i, m_txt_ctl[i]);
	/* edit - text */
	u32StartID = IDC_CONF_EDT_GERBER_PATH;
	for (i = 0; i < eCONF_EDT_MAX; i++)		DDX_Control(dx, u32StartID + i, m_edt_txt[i]);
	/* edit - integer */
	u32StartID = IDC_CONF_EDT_ACAM_SPEC_HORZ;
	for (i = 0; i < eCONF_EDT_INT_MAX; i++)	DDX_Control(dx, u32StartID + i, m_edt_int[i]);
	/* edit - float */
	u32StartID = IDC_CONF_EDT_ACAM_POINT_DIST;
	for (i = 0; i < eCONF_EDT_FLOAT_MAX; i++)	DDX_Control(dx, u32StartID + i, m_edt_flt[i]);
}

BEGIN_MESSAGE_MAP(CDlgConf, CDlgMenu)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CONF_BTN_FILE_SELECT, IDC_CONF_BTN_HOT_AIR, OnBtnClick)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CONF_CHK_LAMP_TYPE_1, IDC_CONF_CHK_GLOBAL_OFFSET, OnChkClick)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgConf::PreTranslateMessage(MSG* msg)
{
	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgConf::OnInitDlg()
{
	/* 컨트롤 초기화 */
	InitCtrl();
	if (!InitGrid())	return FALSE;

	/* Load Config */
	LoadConfig();

	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgConf::OnExitDlg()
{
	CloseGrid();
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgConf::OnPaintDlg(CDC* dc)
{

	/* 자식 윈도 호출 */
	CDlgMenu::DrawOutLine();
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgConf::OnResizeDlg()
{
}

/*
 desc : 부모 스레드에 의해 주기적으로 호출됨
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgConf::UpdatePeriod(UINT64 tick, BOOL is_busy)
{

	/* Control Enable or Disable */
	UpdateControl(tick, is_busy);
}

/*
 desc : 컨트롤
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgConf::UpdateControl(UINT64 tick, BOOL is_busy)
{
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgConf::InitCtrl()
{
	CResizeUI clsResizeUI;

	INT32 i = 0;
	INT64 i64Min[eCONF_EDT_INT_MAX] = { 320,  240,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,   0,   0 };
	INT64 i64Max[eCONF_EDT_INT_MAX] = { 1280, 1024, 99999, 99999, 99999, 99999, 99999, 99999, 99999, 99999, 99999, 99999, 255, 255 };
	DOUBLE dbMin[eCONF_EDT_FLOAT_MAX] = { 6.0000f, 1.0001f, 0.0001f, 1.0000f, 0.0001f, 1.0000f, 0.0001f,
							1.0000f, 0.0001f,  0.0000f, 0.0000f,
							0.001, 0.000000f, 0.000000f, 0.000000f, 0.000000f };
	DOUBLE dbMax[eCONF_EDT_FLOAT_MAX] = { 500.0000f, 30.0000f, 50.0000f, 50.0000f, 50.0000f, 50.0000f, 50.0000f,
							50.0000f, 50.0000f, 999.0000f, 999.0000f,
							359.999, 999.999999f, 999.999999f, 999.999999f, 999.999999f };

	/* group box */
	for (i = 0; i < eCONF_GRP_MAX; i++)
	{
		m_grp_ctl[i].SetLogFont(&g_lf[eFONT_LEVEL2_BOLD]);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_grp_ctl[i]);
		// by sysandj : Resize UI
	}
	/* static - Normal */
	for (i = 0; i < eCONF_TXT_MAX; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_txt_ctl[i]);
		// by sysandj : Resize UI
	}

	for (i = 0; i < eCONF_EDT_MAX; i++)
	{
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_edt_txt[i]);
		// by sysandj : Resize UI
	}

	/* checkbox */
	for (i = 0; i < eCONF_CHK_MAX; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_chk_ctl[i].SetBgColor(RGB(255, 255, 255));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_chk_ctl[i]);
		// by sysandj : Resize UI
	}
	/* button - normal */
	for (i = 0; i < eCONF_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
		// by sysandj : Resize UI
	}
	/* edit - integer */
	for (i = 0; i < eCONF_EDT_INT_MAX; i++)
	{
		m_edt_int[i].SetEditFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_edt_int[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_int[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_int[i].SetReadOnly(TRUE);
		m_edt_int[i].SetInputType(ENM_DITM::en_uint32);
		m_edt_int[i].SetInputSign(ENM_MNUS::en_unsign);
		m_edt_int[i].SetMinMaxNum(i64Min[i], i64Max[i]);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_edt_int[i]);
		// by sysandj : Resize UI
	}
	/* 현재 사용하지 않음 */
	m_edt_int[eCONF_EDT_INT_TRIG_PLUS_2].EnableWindow(FALSE);
	m_edt_int[eCONF_EDT_INT_TRIG_PLUS_2].SetDisableKBDN();
	m_edt_int[eCONF_EDT_INT_TRIG_MINUS_2].EnableWindow(FALSE);
	m_edt_int[eCONF_EDT_INT_TRIG_MINUS_2].SetDisableKBDN();
	/* edit - float */
	for (i = 0; i < eCONF_EDT_FLOAT_MAX; i++)
	{
		m_edt_flt[i].SetEditFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_edt_flt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_flt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_flt[i].SetReadOnly(TRUE);
		m_edt_flt[i].SetInputType(ENM_DITM::en_double);
		if (i == eCONF_EDT_FLOAT_TRANS_GBL_SCALE_X || i == eCONF_EDT_FLOAT_TRANS_GBL_SCALE_Y)	m_edt_flt[i].SetInputSign(ENM_MNUS::en_sign);
		else					m_edt_flt[i].SetInputSign(ENM_MNUS::en_unsign);
		m_edt_flt[i].SetMinMaxNum(dbMin[i], dbMax[i]);
		if (i < eCONF_EDT_FLOAT_EXPO_START_X)			m_edt_flt[i].SetInputPoint(1);
		else if (i < eCONF_EDT_FLOAT_TRANS_GBL_ROTATE)	m_edt_flt[i].SetInputPoint(4);
		else if (i == eCONF_EDT_FLOAT_TRANS_GBL_ROTATE)	m_edt_flt[i].SetInputPoint(3);
		else				m_edt_flt[i].SetInputPoint(6);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_edt_flt[i]);
		// by sysandj : Resize UI
	}
	/* Transformation Recipe 사용 여부에 따라 */
	if (0x00 == uvEng_GetConfig()->global_trans.use_trans)
	{
		for (i = eCONF_CHK_GLOBAL_RECT; i < eCONF_CHK_MAX; i++)	m_chk_ctl[i].EnableWindow(FALSE);
		for (i = eCONF_EDT_FLOAT_TRANS_GBL_ROTATE; i < eCONF_EDT_FLOAT_MAX; i++)	m_edt_flt[i].EnableWindow(FALSE);
	}
}

/*
 desc : GridControl 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgConf::InitGrid()
{

	return TRUE;
}

/*
 desc : GridControl 해제
 parm : None
 retn : None
*/
VOID CDlgConf::CloseGrid()
{
}

/*
 desc : 객체 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgConf::InitObject()
{

	return TRUE;
}

/*
 desc : 객체 해제
 parm : None
 retn : None
*/
VOID CDlgConf::CloseObject()
{
}

/*
 desc : 일반 버튼 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgConf::OnBtnClick(UINT32 id)
{
	switch (id)
	{
	case IDC_CONF_BTN_FILE_SELECT:	SelectPath();		break;
	case IDC_CONF_BTN_CONF_LOAD:	LoadConfig();		break;
	case IDC_CONF_BTN_CONF_SAVE:	SaveConfig();		break;
	case IDC_CONF_BTN_START_XY:	SetExpoStartXY();	break;
	case IDC_CONF_BTN_ANALOG_GAIN:	SetAnalogGain();	break;
	case IDC_CONF_BTN_HOT_AIR:	SetTempHotAir();	break;
	}
}

/*
 desc : 체크 버튼 클릭한 경우
 parm : id	- [in]  체크 버튼 ID
 retn : None
*/
VOID CDlgConf::OnChkClick(UINT32 id)
{
	BOOL bEnable = FALSE;
	switch (id)
	{
	case IDC_CONF_CHK_GLOBAL_ROTATION:
		bEnable = m_chk_ctl[eCONF_CHK_GLOBAL_ROTATION].GetCheck() == 1;
		if (!bEnable)	m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_ROTATE].SetWindowTextW(L"0.000");
		m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_ROTATE].EnableWindow(bEnable);
		break;
	case IDC_CONF_CHK_GLOBAL_SCALE:
		bEnable = m_chk_ctl[eCONF_CHK_GLOBAL_SCALE].GetCheck() == 1;
		if (!bEnable)
		{
			m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_SCALE_X].SetWindowTextW(L"0.000000");
			m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_SCALE_Y].SetWindowTextW(L"0.000000");
		}
		m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_SCALE_X].EnableWindow(bEnable);
		m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_SCALE_Y].EnableWindow(bEnable);
		break;
	case IDC_CONF_CHK_GLOBAL_OFFSET:
		bEnable = m_chk_ctl[eCONF_CHK_GLOBAL_OFFSET].GetCheck() == 1;
		if (!bEnable)
		{
			m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_OFFSET_X].SetWindowTextW(L"0.000000");
			m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_OFFSET_Y].SetWindowTextW(L"0.000000");
		}
		m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_OFFSET_X].EnableWindow(bEnable);
		m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_OFFSET_Y].EnableWindow(bEnable);
		break;
	}
}

/*
 desc : 거버 경로 설정
 parm : None
 retn : None
*/
VOID CDlgConf::SelectPath()
{
	TCHAR tzPath[MAX_PATH_LEN] = { NULL };
	if (!uvCmn_GetSelectPath(m_hWnd, tzPath))	return;
	m_edt_txt[eCONF_EDT_GERBER_PATH].SetWindowTextW(tzPath);
}

/*
 desc : 환경 설정 파일 적재
 parm : None
 retn : None
*/
VOID CDlgConf::LoadConfig()
{
	LPG_CIEA pstCfg = uvEng_GetConfig();

	/* 파일에 저장된 환경 파일 부터 적재 */
	uvEng_ReLoadConfig();
	/* Calibration file 재적재 */
	uvEng_ReloadCaliFile();

	LoadCommon(pstCfg);
	LoadAlignCamera(pstCfg);
	LoadTempRange(pstCfg);
	LoadEtc(pstCfg);
	LoadLuria(pstCfg);
	LoadTrigGrab(pstCfg);
	LoadTransformation(pstCfg);
#if 0	/* Not currently in use */
	LoadMarkFind(pstCfg);
#endif
}

/*
 desc : 환경 설정 파일 저장
 parm : None
 retn : None
*/
VOID CDlgConf::SaveConfig()
{
	LPG_CIEA pstCfg = uvEng_GetConfig();
	SaveCommon(pstCfg);
	SaveAlignCamera(pstCfg);
	SaveTempRange(pstCfg);
	SaveEtc(pstCfg);
	SaveLuria(pstCfg);
	SaveTrigGrab(pstCfg);
	SaveTransformation(pstCfg);
#if 0	/* Not currently in use */
	SaveMarkFind(pstCfg);
#endif
	/* 전체 환경 파일 저장 */
	uvEng_SaveConfig();
}

/*
 desc : 환경 설정 파일 적재 - Common
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConf::LoadCommon(LPG_CIEA conf)
{
	UINT8 u8Check = 0x00;
	LPG_CCGS pstConf = &conf->set_comn;
	LPG_CIPC pstPodis = &conf->set_podis;

	/* Gerber path */
	m_edt_txt[eCONF_EDT_GERBER_PATH].SetTextToStr(pstConf->gerber_path, TRUE);
	/* Strobe Type */
	m_chk_ctl[eCONF_CHK_LAMP_TYPE_1].SetCheck(!pstConf->strobe_lamp_type);
	m_chk_ctl[eCONF_CHK_LAMP_TYPE_2].SetCheck(pstConf->strobe_lamp_type);
	/* Align Camera Type */
	u8Check = ENG_VCPK(pstConf->align_camera_kind) == ENG_VCPK::en_camera_basler_ipv4 ? 1 : 0;
	m_chk_ctl[eCONF_CHK_CAMERA_BASLER].SetCheck(u8Check);
	u8Check = ENG_VCPK(pstConf->align_camera_kind) == ENG_VCPK::en_camera_ids_ph ? 1 : 0;
	m_chk_ctl[eCONF_CHK_CAMERA_IDS].SetCheck(u8Check);
	/* Whether to save mark grabbed file */
	m_chk_ctl[eCONF_CHK_GRAB_MARK_SAVE].SetCheck(pstConf->grab_mark_saved);
	/* Whether to save log file */
	m_chk_ctl[eCONF_CHK_LOG_FILE_SAVE].SetCheck(pstConf->log_file_saved);
	/* Run Recipe Homing */
	m_chk_ctl[eCONF_CHK_RUN_RECIPE_HOMING].SetCheck(pstPodis->load_recipe_homing);
	/* Material Detecting */
	m_chk_ctl[eCONF_CHK_MATERIAL_DETECT].SetCheck(pstConf->run_material_detect);
}

/*
 desc : 환경 설정 파일 저장 - Common
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConf::SaveCommon(LPG_CIEA conf)
{
	UINT8 u8Check = 0x00;
	LPG_CCGS pstConf = &conf->set_comn;
	LPG_CIPC pstPodis = &conf->set_podis;

	/* Gerber path */
	wmemset(pstConf->gerber_path, 0x00, MAX_PATH_LEN);
	m_edt_txt[eCONF_EDT_GERBER_PATH].GetWindowTextW(pstConf->gerber_path, MAX_PATH_LEN);
	/* Strobe Type */
	pstConf->strobe_lamp_type = (UINT8)m_chk_ctl[eCONF_CHK_LAMP_TYPE_2].GetCheck();
	/* Align Camera Type */
	pstConf->align_camera_kind = 0x00;
	if (m_chk_ctl[eCONF_CHK_CAMERA_BASLER].GetCheck())	pstConf->align_camera_kind = (UINT8)ENG_VCPK::en_camera_basler_ipv4;
	if (m_chk_ctl[eCONF_CHK_CAMERA_IDS].GetCheck())	pstConf->align_camera_kind = (UINT8)ENG_VCPK::en_camera_ids_ph;
	/* Whether to save mark grabbed file */
	pstConf->grab_mark_saved = (UINT8)m_chk_ctl[eCONF_CHK_GRAB_MARK_SAVE].GetCheck();
	/* Whether to save log file */
	pstConf->log_file_saved = (UINT8)m_chk_ctl[eCONF_CHK_LOG_FILE_SAVE].GetCheck();
	/* Run recipe homing */
	pstPodis->load_recipe_homing = (UINT8)m_chk_ctl[eCONF_CHK_RUN_RECIPE_HOMING].GetCheck();
	/* Material Detecting */
	pstConf->run_material_detect = (UINT8)m_chk_ctl[eCONF_CHK_MATERIAL_DETECT].GetCheck();
}

/*
 desc : 환경 설정 파일 설정 - Align Camera의 Gain 값 저장 후, 카메라 파라미터 다시 적용
 parm : None
 retn : None
*/
VOID CDlgConf::SetAnalogGain()
{
#if 0
	LPG_CCDB pstBasler = &uvEng_GetConfig()->set_basler;
	/* Analog Gain */
	pstBasler->cam_gain_level[0] = (UINT8)m_edt_int[eCONF_EDT_INT_ACAM_ANALOG_GAIN_1].GetTextToNum();
	pstBasler->cam_gain_level[1] = (UINT8)m_edt_int[eCONF_EDT_INT_ACAM_ANALOG_GAIN_2].GetTextToNum();

	/* 얼라인 카메라의 파라미터 값 다시 적용하기 위한 호출 */
	if (!uvEng_Camera_UpdateSetParam())
	{
		AfxMessageBox(L"Failed to set the parameter for align camera", MB_ICONSTOP | MB_TOPMOST);
	}
#else
	uvEng_Camera_SetGainLevel(0x01, (UINT8)m_edt_int[eCONF_EDT_INT_ACAM_ANALOG_GAIN_1].GetTextToNum());
	uvEng_Camera_SetGainLevel(0x02, (UINT8)m_edt_int[eCONF_EDT_INT_ACAM_ANALOG_GAIN_2].GetTextToNum());
#endif
}

/*
 desc : 온조기 온도 제어
 parm : None
 retn : None
*/
VOID CDlgConf::SetTempHotAir()
{
	uvEng_MCQ_SetHotAirTempSetting(m_edt_flt[eCONF_EDT_FLOAT_TEMP_HOTAIR_SET].GetTextToDouble());
}

/*
 desc : 환경 설정 파일 적재 - Align Camera
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConf::LoadAlignCamera(LPG_CIEA conf)
{
	LPG_CSCI pstCams = &conf->set_cams;
	LPG_CASI pstSpec = &conf->acam_spec;
	LPG_CCDB pstBasler = &conf->set_basler;
	/* ACam Width / Height */
	m_edt_int[eCONF_EDT_INT_ACAM_SPEC_HORZ].SetTextToNum(pstCams->acam_size[0]);
	m_edt_int[eCONF_EDT_INT_ACAM_SPEC_VERT].SetTextToNum(pstCams->acam_size[1]);
	/* 스펙 검사를 위한 좌/우 이동 거리 */
	m_edt_flt[eCONF_EDT_FLOAT_ACAM_POINT_DIST].SetTextToNum(pstSpec->point_dist, 4);
	/* Install Angle */
	m_chk_ctl[eCONF_CHK_ACAM_ROTATE_INST].SetCheck(pstCams->acam_inst_angle);
	/* Analog Gain */
	m_edt_int[eCONF_EDT_INT_ACAM_ANALOG_GAIN_1].SetTextToNum(pstBasler->cam_gain_level[0]);
	m_edt_int[eCONF_EDT_INT_ACAM_ANALOG_GAIN_2].SetTextToNum(pstBasler->cam_gain_level[1]);
}

/*
 desc : 환경 설정 파일 저장 - Align Camera
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConf::SaveAlignCamera(LPG_CIEA conf)
{
	LPG_CSCI pstCams = &conf->set_cams;
	LPG_CASI pstSpec = &conf->acam_spec;
	LPG_CCDB pstBasler = &conf->set_basler;
	/* ACam Width / Height */
	pstCams->acam_size[0] = (UINT16)m_edt_int[eCONF_EDT_INT_ACAM_SPEC_HORZ].GetTextToNum();
	pstCams->acam_size[1] = (UINT16)m_edt_int[eCONF_EDT_INT_ACAM_SPEC_VERT].GetTextToNum();
	/* 스펙 검사를 위한 좌/우 이동 거리 */
	pstSpec->point_dist = m_edt_flt[eCONF_EDT_FLOAT_ACAM_POINT_DIST].GetTextToDouble();
	/* Install Angle */
	pstCams->acam_inst_angle = (UINT8)m_chk_ctl[eCONF_CHK_ACAM_ROTATE_INST].GetCheck();
	/* Analog Gain */
	pstBasler->cam_gain_level[0] = (UINT8)m_edt_int[eCONF_EDT_INT_ACAM_ANALOG_GAIN_1].GetTextToNum();
	pstBasler->cam_gain_level[1] = (UINT8)m_edt_int[eCONF_EDT_INT_ACAM_ANALOG_GAIN_2].GetTextToNum();
}

/*
 desc : 환경 설정 파일 적재 - Temperature & Range
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConf::LoadTempRange(LPG_CIEA conf)
{
	LPG_DITR pstConf = &conf->temp_range;
	m_edt_flt[eCONF_EDT_FLOAT_TEMP_HOTAIR_SET].SetTextToNum(pstConf->hot_air[0], 1);
	m_edt_flt[eCONF_EDT_FLOAT_TEMP_HOTAIR_RANGE].SetTextToNum(pstConf->hot_air[1], 1);
	m_edt_flt[eCONF_EDT_FLOAT_TEMP_DI_SET].SetTextToNum(pstConf->di_internal[0], 1);
	m_edt_flt[eCONF_EDT_FLOAT_TEMP_DI_RANGE].SetTextToNum(pstConf->di_internal[1], 1);
	m_edt_flt[eCONF_EDT_FLOAT_TEMP_LED_SET].SetTextToNum(pstConf->optic_led[0], 1);
	m_edt_flt[eCONF_EDT_FLOAT_TEMP_LED_RANGE].SetTextToNum(pstConf->optic_led[1], 1);
	m_edt_flt[eCONF_EDT_FLOAT_TEMP_BD_SET].SetTextToNum(pstConf->optic_board[0], 1);
	m_edt_flt[eCONF_EDT_FLOAT_TEMP_BD_RANGE].SetTextToNum(pstConf->optic_board[1], 1);
}

/*
 desc : 환경 설정 파일 저장 - Temperature & Range
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConf::SaveTempRange(LPG_CIEA conf)
{
	LPG_DITR pstConf = &conf->temp_range;
	pstConf->hot_air[0] = m_edt_flt[eCONF_EDT_FLOAT_TEMP_HOTAIR_SET].GetTextToDouble();
	pstConf->hot_air[1] = m_edt_flt[eCONF_EDT_FLOAT_TEMP_HOTAIR_RANGE].GetTextToDouble();
	pstConf->di_internal[0] = m_edt_flt[eCONF_EDT_FLOAT_TEMP_DI_SET].GetTextToDouble();
	pstConf->di_internal[1] = m_edt_flt[eCONF_EDT_FLOAT_TEMP_DI_RANGE].GetTextToDouble();
	pstConf->optic_led[0] = m_edt_flt[eCONF_EDT_FLOAT_TEMP_LED_SET].GetTextToDouble();
	pstConf->optic_led[1] = m_edt_flt[eCONF_EDT_FLOAT_TEMP_LED_RANGE].GetTextToDouble();
	pstConf->optic_board[0] = m_edt_flt[eCONF_EDT_FLOAT_TEMP_BD_SET].GetTextToDouble();
	pstConf->optic_board[1] = m_edt_flt[eCONF_EDT_FLOAT_TEMP_BD_RANGE].GetTextToDouble();
}

/*
 desc : 환경 설정 파일 적재 - ETC
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConf::LoadEtc(LPG_CIEA conf)
{
}

/*
 desc : 환경 설정 파일 저장 - ETC
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConf::SaveEtc(LPG_CIEA conf)
{
}

/*
 desc : 환경 설정 파일 적재 - Luria
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConf::LoadLuria(LPG_CIEA conf)
{
	LPG_LDSM pstLuria = uvEng_ShMem_GetLuria();
	m_edt_flt[eCONF_EDT_FLOAT_EXPO_START_X].SetTextToNum(pstLuria->machine.table_expo_start_xy[0].x / 1000.0f, 3);
	m_edt_flt[eCONF_EDT_FLOAT_EXPO_START_Y].SetTextToNum(pstLuria->machine.table_expo_start_xy[0].y / 1000.0f, 3);
	m_edt_flt[eCONF_EDT_FLOAT_EXPO_START_X].Invalidate();
	m_edt_flt[eCONF_EDT_FLOAT_EXPO_START_Y].Invalidate();
}

/*
 desc : 환경 설정 파일 저장 - Luria
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConf::SaveLuria(LPG_CIEA conf)
{
	LPG_LDSM pstLuria = uvEng_ShMem_GetLuria();
	conf->luria_svc.table_expo_start_xy[0][0] = pstLuria->machine.table_expo_start_xy[0].x / 1000.0f;
	conf->luria_svc.table_expo_start_xy[0][1] = pstLuria->machine.table_expo_start_xy[0].y / 1000.0f;
}

/*
 desc : 노광 시작 위치 설정
 parm : None
 retn : None
*/
VOID CDlgConf::SetExpoStartXY()
{
	DOUBLE dbStartX = m_edt_flt[eCONF_EDT_FLOAT_EXPO_START_X].GetTextToDouble();
	DOUBLE dbStartY = m_edt_flt[eCONF_EDT_FLOAT_EXPO_START_Y].GetTextToDouble();

	/* 노광 시작 위치 설정 */
	uvEng_Luria_ReqSetTableExposureStartPos(0x01,
		(UINT32)ROUNDED(dbStartX * 1000.0f, 0),
		(UINT32)ROUNDED(dbStartY * 1000.0f, 0));
}

/*
 desc : 환경 설정 파일 적재 - Trigger Grab
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConf::LoadTrigGrab(LPG_CIEA conf)
{
	UINT8 i = 0x03;

	/* Trigger On Time */
	m_edt_int[eCONF_EDT_INT_TRIG_ON_TIME_1].SetTextToNum((UINT32)conf->trig_grab.trig_on_time[0]);
	m_edt_int[eCONF_EDT_INT_TRIG_ON_TIME_2].SetTextToNum((UINT32)conf->trig_grab.trig_on_time[1]);
	/* Strobe On Time */
	m_edt_int[eCONF_EDT_INT_STROBE_ON_TIME_1].SetTextToNum((UINT32)conf->trig_grab.strob_on_time[0]);
	m_edt_int[eCONF_EDT_INT_STROBE_ON_TIME_2].SetTextToNum((UINT32)conf->trig_grab.strob_on_time[1]);
	/* Trigger Delay Time */
	m_edt_int[eCONF_EDT_INT_TRIG_DELAY_TIME_1].SetTextToNum((UINT32)conf->trig_grab.trig_delay_time[0]);
	m_edt_int[eCONF_EDT_INT_TRIG_DELAY_TIME_2].SetTextToNum((UINT32)conf->trig_grab.trig_delay_time[1]);
	/* Trigger Plus Time */
#if 0
	m_edt_int[eCONF_EDT_INT_TRIG_PLUS_1].SetTextToNum((UINT32)conf->trig_set.trig_plus[0]);
	m_edt_int[eCONF_EDT_INT_TRIG_PLUS_2].SetTextToNum((UINT32)conf->trig_grab.trig_plus[1]);
#else
	m_edt_int[eCONF_EDT_INT_TRIG_PLUS_1].SetTextToNum(conf->trig_grab.trig_forward);
	//	m_edt_int[eCONF_EDT_INT_TRIG_PLUS_2].SetTextToNum(conf->trig_grab.trig_forward);
#endif
	/* Trigger Minus Time */
#if 0
	m_edt_int[eCONF_EDT_INT_TRIG_MINUS_1].SetTextToNum(conf->trig_grab.trig_plus[0]);
	//	m_edt_int[eCONF_EDT_INT_TRIG_MINUS_2].SetTextToNum(conf->trig_grab.trig_plus[1]);
#else
	m_edt_int[eCONF_EDT_INT_TRIG_MINUS_1].SetTextToNum(conf->trig_grab.trig_backward);
	//	m_edt_int[eCONF_EDT_INT_TRIG_MINUS_2].SetTextToNum(conf->trig_grab.trig_backward);
#endif

	for (; i < eCONF_EDT_INT_MAX; i++)	m_edt_int[i].Invalidate();
}

/*
 desc : 환경 설정 파일 저장 - Trigger Grab
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConf::SaveTrigGrab(LPG_CIEA conf)
{
	/* Trigger On Time */
	conf->trig_grab.trig_on_time[0] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_ON_TIME_1].GetTextToNum();
	conf->trig_grab.trig_on_time[1] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_ON_TIME_2].GetTextToNum();
	/* Strobe On Time */
	conf->trig_grab.strob_on_time[0] = (UINT32)m_edt_int[eCONF_EDT_INT_STROBE_ON_TIME_1].GetTextToNum();
	conf->trig_grab.strob_on_time[1] = (UINT32)m_edt_int[eCONF_EDT_INT_STROBE_ON_TIME_2].GetTextToNum();
	/* Trigger Delay Time */
	conf->trig_grab.trig_delay_time[0] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_DELAY_TIME_1].GetTextToNum();
	conf->trig_grab.trig_delay_time[1] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_DELAY_TIME_2].GetTextToNum();
#if 0
	/* Trigger Plus Time */
	conf->trig_grab.trig_plus[0] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_PLUS_1].GetTextToNum();
	conf->trig_grab.trig_plus[1] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_PLUS_2].GetTextToNum();
	conf->trig_grab.trig_plus[2] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_PLUS_1].GetTextToNum();
	conf->trig_grab.trig_plus[3] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_PLUS_2].GetTextToNum();
	/* Trigger Minus Time */
	conf->trig_grab.trig_minus[0] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_MINUS_1].GetTextToNum();
	conf->trig_grab.trig_minus[1] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_MINUS_2].GetTextToNum();
	conf->trig_grab.trig_minus[2] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_MINUS_1].GetTextToNum();
	conf->trig_grab.trig_minus[3] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_MINUS_2].GetTextToNum();
#else
	/* Trigger Plus count */
	conf->trig_grab.trig_forward = (INT32)m_edt_int[eCONF_EDT_INT_TRIG_PLUS_1].GetTextToNum();
	/* Trigger Minus count */
	conf->trig_grab.trig_backward = (INT32)m_edt_int[eCONF_EDT_INT_TRIG_MINUS_1].GetTextToNum();
#endif
}

/*
 설명 : Panel의 Transformation 값 읽어들이기
 변수 : conf	- [in]  환경 파일 구조체 포인터
 반환 : None
*/
VOID CDlgConf::LoadTransformation(LPG_CIEA conf)
{
	BOOL bEnable = TRUE;

	m_chk_ctl[eCONF_CHK_GLOBAL_RECT].SetCheck(conf->global_trans.use_rectangle);
	m_chk_ctl[eCONF_CHK_GLOBAL_ROTATION].SetCheck(conf->global_trans.use_rotation_mode);
	m_chk_ctl[eCONF_CHK_GLOBAL_SCALE].SetCheck(conf->global_trans.use_scaling_mode);
	m_chk_ctl[eCONF_CHK_GLOBAL_OFFSET].SetCheck(conf->global_trans.use_offset_mode);
	m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_ROTATE].SetTextToNum(conf->global_trans.rotation / 1000.0f, 3);
	m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_SCALE_X].SetTextToNum(conf->global_trans.scaling[0] / 1000000.0f, 6);
	m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_SCALE_Y].SetTextToNum(conf->global_trans.scaling[1] / 1000000.0f, 6);
	m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_OFFSET_X].SetTextToNum(conf->global_trans.offset[0] / 1000000.0f, 6);
	m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_OFFSET_Y].SetTextToNum(conf->global_trans.offset[1] / 1000000.0f, 6);

	bEnable = m_chk_ctl[eCONF_CHK_GLOBAL_ROTATION].GetCheck() == 1;
	m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_ROTATE].EnableWindow(bEnable);
	bEnable = m_chk_ctl[eCONF_CHK_GLOBAL_SCALE].GetCheck() == 1;
	m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_SCALE_X].EnableWindow(bEnable);
	m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_SCALE_Y].EnableWindow(bEnable);
	bEnable = m_chk_ctl[eCONF_CHK_GLOBAL_OFFSET].GetCheck() == 1;
	m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_OFFSET_X].EnableWindow(bEnable);
	m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_OFFSET_Y].EnableWindow(bEnable);
}

/*
 설명 : Panel의 Transformation 값 저장하기
 변수 : conf	- [in]  환경 파일 구조체 포인터
 반환 : None
*/
VOID CDlgConf::SaveTransformation(LPG_CIEA conf)
{
	conf->global_trans.use_rectangle = (UINT8)m_chk_ctl[eCONF_CHK_GLOBAL_RECT].GetCheck();
	conf->global_trans.use_rotation_mode = (UINT8)m_chk_ctl[eCONF_CHK_GLOBAL_ROTATION].GetCheck();
	conf->global_trans.use_scaling_mode = (UINT8)m_chk_ctl[eCONF_CHK_GLOBAL_SCALE].GetCheck();
	conf->global_trans.use_offset_mode = (UINT8)m_chk_ctl[eCONF_CHK_GLOBAL_OFFSET].GetCheck();

	conf->global_trans.rotation = (INT32)ROUNDED(m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_ROTATE].GetTextToDouble() * 1000.0f, 0);
	conf->global_trans.scaling[0] = (UINT32)ROUNDED(m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_SCALE_X].GetTextToDouble() * 1000000.0f, 0);
	conf->global_trans.scaling[1] = (UINT32)ROUNDED(m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_SCALE_Y].GetTextToDouble() * 1000000.0f, 0);
	conf->global_trans.offset[0] = (UINT32)ROUNDED(m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_OFFSET_X].GetTextToDouble() * 1000000.0f, 0);
	conf->global_trans.offset[1] = (UINT32)ROUNDED(m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_OFFSET_Y].GetTextToDouble() * 1000000.0f, 0);
}

#if 0	/* Not currently in use */
/*
 설명 : Mark Find 값 읽어들이기
 변수 : conf	- [in]  환경 파일 구조체 포인터
 반환 : None
*/
VOID CDlgConf::LoadMarkFind(LPG_CIEA conf)
{
	LPG_CVMF pstConf = &conf->mark_find;

	m_edt_int[14].SetTextToNum(pstConf->max_mark_regist);
	m_edt_int[15].SetTextToNum(pstConf->max_mark_grab);
	m_edt_int[16].SetTextToNum(pstConf->max_mark_find);
	m_edt_int[17].SetTextToNum(UINT8(ROUNDED(pstConf->model_smooth, 0)));
	m_edt_int[18].SetTextToNum(pstConf->detail_level);
}

/*
 설명 : Mark Find 값 저장하기
 변수 : conf	- [in]  환경 파일 구조체 포인터
 반환 : None
*/
VOID CDlgConf::SaveMarkFind(LPG_CIEA conf)
{
	LPG_CVMF pstConf = &conf->mark_find;

	pstConf->max_mark_regist = (UINT8)m_edt_int[14].GetTextToNum();
	pstConf->max_mark_grab = (UINT8)m_edt_int[15].GetTextToNum();
	pstConf->max_mark_find = (UINT8)m_edt_int[16].GetTextToNum();
	pstConf->model_smooth = (DOUBLE)m_edt_int[17].GetTextToNum();
	pstConf->detail_level = (UINT8)m_edt_int[18].GetTextToNum();
}
#endif