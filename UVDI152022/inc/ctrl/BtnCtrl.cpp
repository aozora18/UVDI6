
/*
 desc : Button Control
*/

#include "pch.h"
#include "BtnCtrl.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : 생성자
 parm : None
 retn : None
*/
CBtnCtrl::CBtnCtrl()
{
	m_bLButtonDnClicked	= FALSE;
	m_wMsgBtnPull		= 0xffffffff;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CBtnCtrl::~CBtnCtrl()
{
}

BEGIN_MESSAGE_MAP(CBtnCtrl, CMacButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

/*
 desc : 마우스 왼쪽 버튼 클릭 Down
 parm : flags	- [in]  
		point	- [in]  
 retn : None
*/
VOID CBtnCtrl::OnLButtonDown(UINT32 flags, CPoint point)
{
	/* 윈도 혹은 다이얼로그 어디 영역에서나 현재 Click Event 먹히기 위함 */
	SetCapture();
	/* 왼쪽 마우스 버튼 클릭 다운 가상 함수 호출 */
	m_bLButtonDnClicked = LButtonDn();
	if (!m_bLButtonDnClicked)
	{
		/* Capture Release */
		ReleaseCapture();
		return;
	}

	CMacButton::OnLButtonDown(flags, point);
}

/*
 desc : 마우스 왼쪽 버튼 클릭 UP
 parm : flags	- [in]  
		point	- [in]  
 retn : None
*/
VOID CBtnCtrl::OnLButtonUp(UINT32 flags, CPoint point)
{
	/* 왼쪽 마우스 버튼 클릭 업 가상 함수 호출 */
	LButtonUp();
	/* Capture Release */
	if (m_bLButtonDnClicked)	ReleaseCapture();

	CMacButton::OnLButtonUp(flags, point);
}

/*
 desc : 버튼 눌림 이벤트
 parm : None
 retn : None
*/
VOID CBtnCtrl::PushBtnEvent()
{
	GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)GetSafeHwnd());
}

/*
 desc : 버튼 팝업 이벤트
 parm : None
 retn : None
*/
VOID CBtnCtrl::PullBtnEvent()
{
	GetParent()->PostMessage(m_wMsgBtnPull, NULL, (LPARAM)GetSafeHwnd());
}

/* --------------------------------------------------------------------------------------------- */
/*                                      Button Numbering                                         */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 마우스 왼쪽 버튼 클릭 UP
 parm : flags	- [in]  
		point	- [in]  
 retn : TRUE or FALSE
*/
BOOL CBtnNum::LButtonUp()
{
	TCHAR tzNum[32]	= {NULL};
	if (m_i64Max <= m_i64Num)	m_i64Num	= m_i64Min;
	else						m_i64Num++;

	/* 버튼 출력 */
	swprintf_s(tzNum, 32, L"%I64d", m_i64Num);
	SetWindowTextW(tzNum);

	/* 상위 부모에게 버튼 이벤트로 알림 */
	PushBtnEvent();

	return TRUE;
}

/*
 desc : 초기화 설정
 parm : start	- [in]  값 초기 시작 값
		max_i	- [in]  최대 값
		min_i	- [in]  최소 값
 retn : None
*/
VOID CBtnNum::SetInit(INT64 start, INT64 max_i, INT64 min_i)
{
	TCHAR tzNum[32]	= {NULL};

	m_i64Num	= start;
	m_i64Max	= max_i;
	m_i64Min	= min_i;

	/* 버튼 출력 */
	swprintf_s(tzNum, 32, L"%I64d", start);
	SetWindowTextW(tzNum);
}

/* --------------------------------------------------------------------------------------------- */
/*                                 Melsec PLC Switch On / Off                                    */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Sub Classing
 parm : None
 retn : None
*/
VOID CBtnPLCOnOff::PreSubclassWindow() 
{
	CBtnCtrl::PreSubclassWindow();
	GetWindowTextW(m_tzTitle, 64);
}

/*
 desc : 마우스 왼쪽 버튼 클릭 UP
 parm : flags	- [in]  
		point	- [in]  
 retn : TRUE or FALSE
*/
BOOL CBtnPLCOnOff::LButtonUp()
{
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
	UINT8 u8Vacuum		= 0x00, u8Shutter = 0x00;
	/* Vacuum & Shutter 현재 값 저장 */
	u8Vacuum	= uvData_PLC_GetMemStruct()->vacuum_status;
	u8Shutter	= uvData_PLC_GetMemStruct()->shutter_status;
	switch (m_u32Addr)
	{
	case en_padr_trigger_board_power_reset	:	uvPLC_TriggerReset();		break;
	case en_padr_shutter_open_close			:	uvPLC_VacuumShutterOnOff(u8Vacuum, !u8Shutter);	break;
	case en_padr_vacuum_on_off				:	uvPLC_VacuumShutterOnOff(!u8Vacuum, u8Shutter);	break;
	case en_padr_safety_reset_cmd			:	uvPLC_SafetyReset();		break;
	case en_padr_chiller_run_cmd			:	uvPLC_ChillerRunStop();		break;
	case en_padr_vacuum_controller_run_cmd	:	uvPLC_VacuumRunStop();		break;
	case en_padr_temp_controller_run_cmd	:	uvPLC_TempCtrlRunStop();	break;
	case en_padr_top_fan_on_off_1			:	uvPLC_TopFanOnOff(1);		break;
	case en_padr_top_fan_on_off_2			:	uvPLC_TopFanOnOff(2);		break;
	case en_padr_top_fan_on_off_3			:	uvPLC_TopFanOnOff(3);		break;
	default									:	return FALSE;
	}
#elif (CUSTOM_CODE_HANWHA_QCELL == DELIVERY_PRODUCT_ID)
	/* 만약 Feeding Roll Down 작업일 경우, 반드시 Tension Control이 On 상태인지 확인 */
	if (m_u32Addr == en_pard_w_feeding_roll_up_down &&
		uvData_PLC_GetMemStruct()->IsRollUp(en_dsii_feeding_roll_up_down) &&
		uvData_PLC_GetMemStruct()->IsWinderRun())
	{
		/* 상위 부모에게 버튼 이벤트로 알림 */
		PushBtnEvent();
	}
	/* 현재 PLC의 상태 값 반전 */
	else
	{
#if 0
		if (!uvPLC_WriteBitInvert(m_u32Addr))	return FALSE;
#else
		UINT8 u8Value		= 0x00;
		LPG_PMRW pstPLCMem	= uvData_PLC_GetMemStruct();
		/* 변경된 값에 따라, 타이틀 변경 */
		switch (m_u32Addr)
		{
		case en_pard_w_unwinder_epc_auto_mode		:	u8Value	= pstPLCMem->unwinder_epc_auto_mode_status;			break;
		case en_pard_w_rewinder_epc_auto_mode		:	u8Value	= pstPLCMem->rewinder_epc_auto_mode_status;			break;
		case en_pard_w_unwinder_epc_center			:	u8Value	= pstPLCMem->unwinder_epc_center_on_status;			break;
		case en_pard_w_rewinder_epc_center			:	u8Value	= pstPLCMem->rewinder_epc_center_on_status;			break;
		case en_pard_w_unwinder_run					:	u8Value	= pstPLCMem->unwinder_run_status;					break;
		case en_pard_w_rewinder_run					:	u8Value	= pstPLCMem->rewinder_run_status;					break;
		case en_pard_w_unwinder_forward_backward	:	u8Value	= pstPLCMem->unwinder_inv_forward_backward_status;	break;
		case en_pard_w_rewinder_forward_backward	:	u8Value	= pstPLCMem->rewinder_inv_forward_backward_status;	break;
		case en_pard_w_unwinder_shaft_lock			:	u8Value	= pstPLCMem->unwinder_shaft_lock;					break;
		case en_pard_w_rewinder_shaft_lock			:	u8Value	= pstPLCMem->rewinder_shaft_lock;					break;

		case en_pard_w_left_right_guide_roll_up_down:
			if (pstPLCMem->IsRollUpDownWaiting(en_dsii_left_guide_roll_up_down) ||
				pstPLCMem->IsRollUpDownWaiting(en_dsii_right_guide_roll_up_down))	u8Value	= 0x02;	/* Waiting */
			else									u8Value	= pstPLCMem->IsGuideRollUpDownAll(0x01) ? 0x01 : 0x00;	break;
		case en_pard_w_feeding_roll_up_down			:
			if (pstPLCMem->IsRollUpDownWaiting(en_dsii_feeding_roll_up_down))	u8Value	= 0x02;	/* Waiting */
			else						u8Value = pstPLCMem->IsRollUp(en_dsii_feeding_roll_up_down) ? 0x01 : 0x00;	break;

		case en_pard_w_ion_bar_run					:	u8Value	= pstPLCMem->IsIonizerRun() ? 0x01 : 0x00;			break;

		case en_pard_w_unwinder_left_clamp_up_down	:	u8Value	= pstPLCMem->IsWinderClampWaitingAll(0x00, 0x00) ? 2 : 0;
														if (!u8Value)	u8Value	= pstPLCMem->unwinder_left_clamp_up_sensor;		break;
		case en_pard_w_unwinder_right_clamp_up_down	:	u8Value	= pstPLCMem->IsWinderClampWaitingAll(0x00, 0x01) ? 2 : 0;
														if (!u8Value)	u8Value	= pstPLCMem->unwinder_right_clamp_up_sensor;	break;
		case en_pard_w_rewinder_left_clamp_up_down	:	u8Value	= pstPLCMem->IsWinderClampWaitingAll(0x01, 0x00) ? 2 : 0;
														if (!u8Value)	u8Value	= pstPLCMem->rewinder_left_clamp_up_sensor;		break;
		case en_pard_w_rewinder_right_clamp_up_down	:	u8Value	= pstPLCMem->IsWinderClampWaitingAll(0x01, 0x01) ? 2 : 0;
														if (!u8Value)	u8Value	= pstPLCMem->rewinder_right_clamp_up_sensor;	break;
		case en_pard_w_chiller_run					:	u8Value	= pstPLCMem->chiller_run_status;					break;
		/* Vacuum Control */
		case en_pard_w_vacuum_1_on					:	u8Value	= pstPLCMem->IsVacuumRun(0x01) ? 0x01 : 0x00;		break;
		case en_pard_w_vacuum_2_on					:	u8Value	= pstPLCMem->IsVacuumRun(0x02) ? 0x01 : 0x00;		break;
		case en_pard_w_vacuum_3_on					:	u8Value	= pstPLCMem->IsVacuumRun(0x03) ? 0x01 : 0x00;		break;
		case en_pard_w_vacuum_4_on					:	u8Value	= pstPLCMem->IsVacuumRun(0x04) ? 0x01 : 0x00;		break;
		/* Blower Control */
		case en_pard_w_blower_1_on					:	u8Value	= pstPLCMem->IsBlowerRun(0x01) ? 0x01 : 0x00;		break;
		case en_pard_w_blower_2_on					:	u8Value	= pstPLCMem->IsBlowerRun(0x02) ? 0x01 : 0x00;		break;
		case en_pard_w_blower_3_on					:	u8Value	= pstPLCMem->IsBlowerRun(0x03) ? 0x01 : 0x00;		break;
		case en_pard_w_blower_4_on					:	u8Value	= pstPLCMem->IsBlowerRun(0x04) ? 0x01 : 0x00;		break;

		case en_pard_w_uv_curing_ctrl				:	u8Value	= pstPLCMem->uv_curing_cont;						break;
		case en_pard_w_cleaning_machine_run			:	u8Value	= pstPLCMem->cleaning_machine_run;					break;
		case en_pard_w_optics_1_2_3_smps_power		:	u8Value	= !pstPLCMem->IsOpticsPowerOn();					break;
		case en_pard_w_local_button_ignore_cmd		:	u8Value	= pstPLCMem->local_button_ignore_cmd;				break;
		case en_pard_w_xy_motor_air_on				:	u8Value	= pstPLCMem->xy_motor_air_on;						break;
		default										:	u8Value	= 0xff;												break;
		}

		/* 현재 등록되지 않는 제어인 경우인지 여부 확인 */
		if (0xff == u8Value || 0x02 == u8Value)
		{
			if (uvLang_IsLangKorean())
			{
				switch (u8Value)
				{
				case 0x02	:	AfxMessageBox(L"응답 대기 중 입니다. 잠시만 ...", MB_ICONSTOP|MB_TOPMOST);	break;
				case 0xff	:	AfxMessageBox(L"알 수 없는 제어 명령 입니다.", MB_ICONSTOP|MB_TOPMOST);	break;
				}
			}
			else
			{
				switch (u8Value)
				{
				case 0x02	:	AfxMessageBox(L"Wait for response. Wait a minute  ...", MB_ICONSTOP|MB_TOPMOST);	break;
				case 0xff	:	AfxMessageBox(L"Unknown control command", MB_ICONSTOP|MB_TOPMOST);					break;
				}
			}
		}
		else
		{
			/* 만약 UV Curing 제어 모드일 경우 */
			if (m_u32Addr == en_pard_w_uv_curing_ctrl)
			{
				/* 현재 경화기 (UV Curing)가 Off되어 있는 경우이고, 거버가 적재되어 있지 않다면, ON 못하게 */
				if (0x00 == u8Value && uvData_Luria_IsRegistRecipe())
				{
					if (uvLang_IsLangKorean())	AfxMessageBox(L"등록된 레시피가 없습니다.", MB_ICONSTOP|MB_TOPMOST);
					else						AfxMessageBox(L"There is no registered recipe.", MB_ICONSTOP|MB_TOPMOST);
					return FALSE;
				}
			}
#if (USE_LIB_PLC)
			if (!uvPLC_WriteBit(m_u32Addr, !u8Value))	return FALSE;
#endif
		}
#endif
	}
#endif

	return TRUE;
}

/*
 desc : On / Off
 parm : value	- [in]  0x00 or 0x01
 retn : None
*/
VOID CBtnPLCOnOff::SetOnOff(UINT8 value)
{
	/* 현재 설정된 값과 새로 설정할 값이 다른지 */
	if (value == m_u8OnOff)	return;
	/* 함수 부하를 줄이기 위해서 문자 버퍼 선언 및 초기화 */
	TCHAR tzTitle[128]	= {NULL};
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
	/* 버튼 타이틀 변경 */
	if (value)	swprintf_s(tzTitle, 128, L"%s.On", m_tzTitle);
	else		swprintf_s(tzTitle, 128, L"%s.Off", m_tzTitle);
#elif (CUSTOM_CODE_HANWHA_QCELL == DELIVERY_PRODUCT_ID)
#endif
	/* 현재 값 변경 */
	SetWindowTextW(tzTitle);
	m_u8OnOff	= value;
}

/*
 desc : Update On or Off / Auto or Manual / ....
 parm : disable	- [in]  강제로 Disable 여부
 retn : None
*/
VOID CBtnPLCOnOff::UpdateValue(BOOL disable)
{
	UINT8 u8Value	= 0x00;

#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
#elif (CUSTOM_CODE_HANWHA_QCELL == DELIVERY_PRODUCT_ID)
	LPG_PMRW pstPLCMem	= uvData_PLC_GetMemStruct();

	/* 가장 최근에 보낸 명령어가 있으면, 컨트롤 Disable 처리 */
	if (m_u32Addr == en_pard_w_unwinder_epc_center || m_u32Addr == en_pard_w_rewinder_epc_center)
	{
		if (pstPLCMem->unwinder_epc_auto_mode_status || pstPLCMem->rewinder_epc_auto_mode_status)
		{
			EnableWindow(disable&&FALSE);
		}
		else
		{
#if (USE_LIB_PLC)
			EnableWindow(disable&&uvPLC_IsSendTimeValid());
#endif
		}
	}
	else
	{
#if (USE_LIB_PLC)
		EnableWindow(disable&&uvPLC_IsSendTimeValid());
#endif
	}

	/* 변경된 값에 따라, 타이틀 변경 */
	switch (m_u32Addr)
	{
	case en_pard_w_unwinder_epc_auto_mode		:	u8Value	= pstPLCMem->unwinder_epc_auto_mode_status;			break;
	case en_pard_w_rewinder_epc_auto_mode		:	u8Value	= pstPLCMem->rewinder_epc_auto_mode_status;			break;
	case en_pard_w_unwinder_epc_center			:	u8Value	= pstPLCMem->unwinder_epc_center_on_status;			break;
	case en_pard_w_rewinder_epc_center			:	u8Value	= pstPLCMem->rewinder_epc_center_on_status;			break;
	case en_pard_w_unwinder_run					:	u8Value	= pstPLCMem->unwinder_run_status;					break;
	case en_pard_w_rewinder_run					:	u8Value	= pstPLCMem->rewinder_run_status;					break;
	case en_pard_w_unwinder_forward_backward	:	u8Value	= pstPLCMem->unwinder_inv_forward_backward_status;	break;
	case en_pard_w_rewinder_forward_backward	:	u8Value	= pstPLCMem->rewinder_inv_forward_backward_status;	break;
	case en_pard_w_unwinder_shaft_lock			:	u8Value	= pstPLCMem->unwinder_shaft_lock;					break;
	case en_pard_w_rewinder_shaft_lock			:	u8Value	= pstPLCMem->rewinder_shaft_lock;					break;

	case en_pard_w_left_right_guide_roll_up_down:
		if (pstPLCMem->IsRollUpDownWaiting(en_dsii_left_guide_roll_up_down) ||
			pstPLCMem->IsRollUpDownWaiting(en_dsii_right_guide_roll_up_down))	u8Value	= 0x02;	/* Waiting */
		else									u8Value	= pstPLCMem->IsGuideRollUpDownAll(0x01) ? 0x01 : 0x00;	break;
	case en_pard_w_feeding_roll_up_down			:
		if (pstPLCMem->IsRollUpDownWaiting(en_dsii_feeding_roll_up_down))	u8Value	= 0x02;	/* Waiting */
		else						u8Value = pstPLCMem->IsRollUp(en_dsii_feeding_roll_up_down) ? 0x01 : 0x00;	break;

	case en_pard_w_ion_bar_run					:	u8Value	= pstPLCMem->IsIonizerRun() ? 0x01 : 0x00;			break;

	case en_pard_w_unwinder_left_clamp_up_down	:	u8Value	= pstPLCMem->IsWinderClampWaitingAll(0x00, 0x00) ? 2 : 0;
													if (!u8Value)	u8Value	= pstPLCMem->unwinder_left_clamp_up_sensor;		break;
	case en_pard_w_unwinder_right_clamp_up_down	:	u8Value	= pstPLCMem->IsWinderClampWaitingAll(0x00, 0x01) ? 2 : 0;
													if (!u8Value)	u8Value	= pstPLCMem->unwinder_right_clamp_up_sensor;	break;
	case en_pard_w_rewinder_left_clamp_up_down	:	u8Value	= pstPLCMem->IsWinderClampWaitingAll(0x01, 0x00) ? 2 : 0;
													if (!u8Value)	u8Value	= pstPLCMem->rewinder_left_clamp_up_sensor;		break;
	case en_pard_w_rewinder_right_clamp_up_down	:	u8Value	= pstPLCMem->IsWinderClampWaitingAll(0x01, 0x01) ? 2 : 0;
													if (!u8Value)	u8Value	= pstPLCMem->rewinder_right_clamp_up_sensor;	break;
	case en_pard_w_chiller_run					:	u8Value	= pstPLCMem->chiller_run_status;					break;
	/* Vacuum Control */
	case en_pard_w_vacuum_1_on					:	u8Value	= pstPLCMem->IsVacuumRun(0x01) ? 0x01 : 0x00;		break;
	case en_pard_w_vacuum_2_on					:	u8Value	= pstPLCMem->IsVacuumRun(0x02) ? 0x01 : 0x00;		break;
	case en_pard_w_vacuum_3_on					:	u8Value	= pstPLCMem->IsVacuumRun(0x03) ? 0x01 : 0x00;		break;
	case en_pard_w_vacuum_4_on					:	u8Value	= pstPLCMem->IsVacuumRun(0x04) ? 0x01 : 0x00;		break;
	/* Blower Control */
	case en_pard_w_blower_1_on					:	u8Value	= pstPLCMem->IsBlowerRun(0x01) ? 0x01 : 0x00;		break;
	case en_pard_w_blower_2_on					:	u8Value	= pstPLCMem->IsBlowerRun(0x02) ? 0x01 : 0x00;		break;
	case en_pard_w_blower_3_on					:	u8Value	= pstPLCMem->IsBlowerRun(0x03) ? 0x01 : 0x00;		break;
	case en_pard_w_blower_4_on					:	u8Value	= pstPLCMem->IsBlowerRun(0x04) ? 0x01 : 0x00;		break;

	case en_pard_w_uv_curing_ctrl				:	u8Value	= pstPLCMem->uv_curing_cont;						break;
	case en_pard_w_cleaning_machine_run			:	u8Value	= pstPLCMem->cleaning_machine_run;					break;
	case en_pard_w_optics_1_2_3_smps_power		:	u8Value	= pstPLCMem->IsOpticsPowerOn() ?0x01 : 0x00;		break;
	case en_pard_w_local_button_ignore_cmd		:	u8Value	= pstPLCMem->local_button_ignore_cmd;				break;
	case en_pard_w_xy_motor_air_on				:	u8Value	= pstPLCMem->xy_motor_air_on;						break;
	}

	/* 현재 설정된 값과 새로 설정할 값이 다른지 */
	if (u8Value == m_u8OnOff && m_u32Addr == m_u32AddrLast)	return;

	/* 컨트롤 비활성화 */
	SetRedraw(FALSE);

	/* 버튼 컨트롤에 해당되는 기본 문자열 얻기 */
	PTCHAR pText	= uvLang_FindString(GetWindowLong(m_hWnd, GWL_ID));
	if (!pText)
	{
		switch (m_u32Addr)
		{
		case en_pard_w_vacuum_1_on	:
		case en_pard_w_vacuum_2_on	:
		case en_pard_w_vacuum_3_on	:
		case en_pard_w_vacuum_4_on	:
			switch (GetUserDefaultUILanguage())
			{
			case 0x0412	:	pText	= L"베큠";		break;	/* 한국 */
			default		:	pText	= L"Vacuum";	break;	/* 영어 */
			}	break;
		case en_pard_w_blower_1_on	:
		case en_pard_w_blower_2_on	:
		case en_pard_w_blower_3_on	:
		case en_pard_w_blower_4_on	:
			switch (GetUserDefaultUILanguage())
			{
			case 0x0412	:	pText	= L"블로어";	break;	/* 한국 */
			default		:	pText	= L"Blower";	break;	/* 영어 */
			}	break;
		}
	}

	/* 버튼 문자열 임시 저장 버퍼 */
	TCHAR tzSubj[128]= {NULL}, tzValue[32]	= {NULL};

	/* 최근 변경된 상태 값 저장 */
	m_u8OnOff		= u8Value;
	m_u32AddrLast	= m_u32Addr;
	/* 변경된 값에 따라, 타이틀 변경 */
	switch (m_u32Addr)
	{
	case en_pard_w_unwinder_epc_auto_mode			:
	case en_pard_w_rewinder_epc_auto_mode			:
		if (u8Value)				swprintf_s(tzSubj, 128, L"%s\n(AUTO)",		pText);
		else						swprintf_s(tzSubj, 128, L"%s\n(MANUAL)",	pText);		break;
	case en_pard_w_unwinder_epc_center			:
	case en_pard_w_rewinder_epc_center			:
		if (u8Value)				swprintf_s(tzSubj, 128, L"%s\n(CENTER)",	pText);
		else						swprintf_s(tzSubj, 128, L"%s\n(STOP)",		pText);		break;
	case en_pard_w_unwinder_run					:
	case en_pard_w_rewinder_run					:
	case en_pard_w_ion_bar_run					:
	case en_pard_w_cleaning_machine_run			:
	case en_pard_w_unwinder_shaft_lock			:
	case en_pard_w_rewinder_shaft_lock			:
	case en_pard_w_chiller_run					:
		if (u8Value)				swprintf_s(tzSubj, 128, L"%s\n(RUN)",		pText);
		else						swprintf_s(tzSubj, 128, L"%s\n(STOP)",		pText);		break;
	case en_pard_w_unwinder_forward_backward	:
	case en_pard_w_rewinder_forward_backward	:
		if (u8Value)				swprintf_s(tzSubj, 128, L"%s\n(FORWARD)",	pText);
		else						swprintf_s(tzSubj, 128, L"%s\n(BACKWARD)",	pText);		break;
	case en_pard_w_left_right_guide_roll_up_down:
	case en_pard_w_feeding_roll_up_down			:
	case en_pard_w_unwinder_left_clamp_up_down	:
	case en_pard_w_unwinder_right_clamp_up_down	:
	case en_pard_w_rewinder_left_clamp_up_down	:
	case en_pard_w_rewinder_right_clamp_up_down	:
		if (0x01 == u8Value)		swprintf_s(tzSubj, 128, L"%s\n(UP)",		pText);
		else if (0x00 == u8Value)	swprintf_s(tzSubj, 128, L"%s\n(DOWN)",		pText);
		else						swprintf_s(tzSubj, 128, L"%s\n(WAIT)",		pText);		break;
	case en_pard_w_uv_curing_ctrl				:
	case en_pard_w_optics_1_2_3_smps_power		:
	case en_pard_w_xy_motor_air_on				:
		if (u8Value)				swprintf_s(tzSubj, 128, L"%s\n(ON)",		pText);
		else						swprintf_s(tzSubj, 128, L"%s\n(OFF)",		pText);		break;	
	/* Vacuum Mode */
	case en_pard_w_vacuum_1_on					:
	case en_pard_w_vacuum_2_on					:
	case en_pard_w_vacuum_3_on					:
	case en_pard_w_vacuum_4_on					:
		if (u8Value)	swprintf_s(tzSubj, 128, L"%s %d (ON)",	pText, m_u32Addr-en_pard_w_vacuum_1_on+1);
		else			swprintf_s(tzSubj, 128, L"%s %d (OFF)",	pText, m_u32Addr-en_pard_w_vacuum_1_on+1);	break;
	/* Blower Mode */
	case en_pard_w_blower_1_on					:
	case en_pard_w_blower_2_on					:
	case en_pard_w_blower_3_on					:
	case en_pard_w_blower_4_on					:
		if (u8Value)	swprintf_s(tzSubj, 128, L"%s %d (ON)",	pText, m_u32Addr-en_pard_w_blower_1_on+1);
		else			swprintf_s(tzSubj, 128, L"%s %d (OFF)",	pText, m_u32Addr-en_pard_w_blower_1_on+1);	break;
	case en_pard_w_local_button_ignore_cmd		:
		if (u8Value)	swprintf_s(tzSubj, 128, L"%s\n(Disable)",	pText);
		else			swprintf_s(tzSubj, 128, L"%s\n(Enable)",	pText);	break;
	}

	/* 버튼 타이틀 설정 */
	if (wcslen(tzSubj))	SetWindowTextW(tzSubj);
	/* 색상 변경 */
	if (u8Value)	SetBgColor(RGB(128, 255, 128));
	else			SetBgColor(RGB(255, 201, 14));

	/* 컨트롤 활성화 */
	SetRedraw(TRUE);
	Invalidate(TRUE);
#endif

}

/* --------------------------------------------------------------------------------------------- */
/*                                     Stage Homing X and Y                                      */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 마우스 왼쪽 버튼 클릭 UP
 parm : flags	- [in]  
		point	- [in]  
 retn : TRUE or FALSE
*/
BOOL CBtnDevAll::LButtonUp()
{
	BOOL bFlag	= TRUE;

#if (USE_LIB_MC2)
	switch (m_enCtrl)
	{
	/* Stop 명령 송신 (Camera and Stage) */
	case en_mccw_device_stop	:	uvMC2_SendDevStopped(ENG_MMDI::en_stage_x);
									uvMC2_SendDevStopped(ENG_MMDI::en_stage_y);
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
									uvMC2_SendDevStopped(ENG_MMDI::en_align_cam1);
									uvMC2_SendDevStopped(ENG_MMDI::en_align_cam2);
#endif
									break;
	/* Homing 명령 송신 */
	case en_mccw_device_home	:	uvMC2_SendDevHoming(ENG_MMDI::en_stage_x);
									uvMC2_SendDevHoming(ENG_MMDI::en_stage_y);
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
									uvMC2_SendDevHoming(ENG_MMDI::en_align_cam1);
									uvMC2_SendDevHoming(ENG_MMDI::en_align_cam2);
#endif
									break;
	/* Lock 명령 송신 */
	case en_mccw_device_lock	:	bFlag	= uvData_MC2_GetData()->IsServoOff(ENG_MMDI::en_stage_x);
									uvMC2_SendDevLocked(ENG_MMDI::en_stage_x,	bFlag);
									uvMC2_SendDevLocked(ENG_MMDI::en_stage_y,	bFlag);
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
									uvMC2_SendDevLocked(ENG_MMDI::en_align_cam1,	bFlag);
									uvMC2_SendDevLocked(ENG_MMDI::en_align_cam2,	bFlag);
#endif
									break;
	/* 에러 초기화 */
	case en_mccw_device_reset	:	uvMC2_SendDevFaultReset(ENG_MMDI::en_stage_x);
									uvMC2_SendDevFaultReset(ENG_MMDI::en_stage_y);
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
									uvMC2_SendDevFaultReset(ENG_MMDI::en_align_cam1);
									uvMC2_SendDevFaultReset(ENG_MMDI::en_align_cam2);
#endif
									break;
	}
#endif
	return TRUE;
}


/* --------------------------------------------------------------------------------------------- */
/*                                           MC2 Homing                                          */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 마우스 왼쪽 버튼 클릭 UP
 parm : flags	- [in]  
		point	- [in]  
 retn : TRUE or FALSE
*/
BOOL CBtnDevHome::LButtonUp()
{
#if (USE_LIB_MC2)
	if (0xff != m_enDrvID)	uvMC2_SendDevHoming(m_enDrvID);
#endif
	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                      Stage Moving - XY                                        */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 마우스 왼쪽 버튼 클릭 Down
 parm : flags	- [in]  
		point	- [in]  
 retn : TRUE or FALSE
*/
BOOL CBtnDevMove::LButtonDn()
{
	BOOL bSucc	= FALSE;

	/* MC2 */
	if (m_enSysID == ENG_EDIC::en_mc2)
	{
		/* Stage의 X or Y 축이 이동 중이거나, 완전히 멈추지 않은 경우인지 확인 */
		if (m_enDrvID == ENG_MMDI::en_stage_x || m_enDrvID == ENG_MMDI::en_stage_y)
		{
			bSucc	= uvData_MC2_GetData()->act_data[ENG_MMDI::en_stage_x].IsStopped() &&
					  uvData_MC2_GetData()->act_data[ENG_MMDI::en_stage_y].IsStopped();
		}
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
		/* Camera 1 or 2 축이 이동 중이거나, 완전히 멈추지 않은 경우인지 확인 */
		else if (m_enDrvID == ENG_MMDI::en_align_cam1 || m_enDrvID == ENG_MMDI::en_align_cam2)
		{
			bSucc	= uvData_MC2_GetData()->act_data[ENG_MMDI::en_align_cam1].IsStopped() &&
					  uvData_MC2_GetData()->act_data[ENG_MMDI::en_align_cam2].IsStopped();
		}
#endif
	}
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_HANWHA_QCELL)
	/* PLC (Roll Feeding) */
	else if (m_enSysID == ENG_EDIC::en_melsec)
	{
		bSucc	= uvData_PLC_GetMemStruct()->roll_feeding_axis_status ? FALSE/*Busy*/ : TRUE/*Ready*/;
	}
#endif
	/* 중지 상태가 아닌 경우인지 확인 */
	if (!bSucc)
	{
#ifdef _DEBUG
		AfxMessageBox(L"The current device (axis) is moving", MB_ICONSTOP|MB_TOPMOST);
#endif
		return FALSE;
	}

	/* Step (일정 거리 만큼)으로 이동 내지 절대 위치 이동할 때, LButtonDn 함수 수행하지 않음 */
	if (m_bJogEnable && m_enMethod == ENG_MMMM::en_move_jog)
	{
		/* Jog 이동 명령 송신 */
		if (m_enSysID == ENG_EDIC::en_mc2)
		{
#if (USE_LIB_MC2)
			if (!uvMC2_SendDevRefMove(ENG_MMMM::en_move_jog, m_enDrvID, m_enDirect))	return FALSE;
#endif
		}
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_HANWHA_QCELL)
		else if (m_enSysID == ENG_EDIC::en_melsec)
		{
			PushBtnEvent();
		}
#endif
	}

	return TRUE;
}

/*
 desc : 마우스 왼쪽 버튼 클릭 UP
 parm : flags	- [in]  
		point	- [in]  
 retn : TRUE or FALSE
*/
BOOL CBtnDevMove::LButtonUp()
{
	if (m_enMethod == ENG_MMMM::en_move_jog)
	{
#if (USE_LIB_MC2)
		/* 해당 Driver의 이동 중지 (기존 명령 취소) */
		if (m_enSysID == ENG_EDIC::en_mc2)
		{
			if (m_bJogEnable)	uvMC2_SendDevStopped(m_enDrvID);
		}
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_HANWHA_QCELL)
		else if (m_enSysID == ENG_EDIC::en_melsec)
		{
			if (m_bJogEnable)	uvPLC_SetRollFeedingJog(en_mdmd_none);
		}
#endif
#endif
	}
	else if (m_enMethod == ENG_MMMM::en_move_step)
	{
		/* 이동 명령 송신 */
		if (m_enSysID == ENG_EDIC::en_mc2)
		{
			if (m_enDirect != en_mdmd_pos_align_start && m_enDirect != en_mdmd_pos_expo_start)
			{
#if (USE_LIB_MC2)
				uvMC2_SendDevRefMove(ENG_MMMM::en_move_step, m_enDrvID, m_enDirect);
#endif
			}
			else
			{
				INT32 i32StartX, i32StartY, i32Velo;	/* 0.1 um or 100 nm */
				/* 현재 X 축에 설정된 스테이지 이동 속도 */
				i32Velo	= uvData_MC2_GetDrvVelo();
				/* 노광 혹은 얼라인 시작 위치 얻기 */
				uvData_Luria_GetStartXY(m_enDirect, i32StartX, i32StartY);
#if (USE_LIB_MC2)
				/* 설정된 위치로 이동 */
				uvMC2_SendDevMoveVector(i32StartX, i32StartY, i32Velo);
#endif
			}
		}
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_HANWHA_QCELL)
		else if (m_enSysID == ENG_EDIC::en_melsec)	PushBtnEvent();
#endif
	}
	else if (m_enMethod == ENG_MMMM::en_move_act)
	{
		INT32 i32Dist	= uvData_MC2_GetDrvDist();	/* 0.1 um or 100 nm */
		UINT32 u32Velo	= uvData_MC2_GetDrvVelo();	/* 0.1 um or 100 nm / sec */
#if (USE_LIB_MC2)
		if (!uvMC2_SendDevAbsMove(m_enDrvID, i32Dist, u32Velo))
		{
			return FALSE;
		}
#endif
	}

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                    ACam Moving - Up / Down                                    */
/* --------------------------------------------------------------------------------------------- */

#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
/*
 desc : 마우스 왼쪽 버튼 클릭 Down
 parm : flags	- [in]  
		point	- [in]  
 retn : TRUE or FALSE
*/
BOOL CBtnACamUpDn::LButtonDn()
{
	BOOL bSucc	= FALSE;

	if (m_enDrvID == ENG_MMDI::en_align_cam1)
	{
		bSucc	= (BOOL)uvData_PLC_GetMemStruct()->camera1_z_axis_status;
	}
	else if (m_enDrvID == ENG_MMDI::en_align_cam2)
	{
		bSucc	= (BOOL)uvData_PLC_GetMemStruct()->camera2_z_axis_status;
	}
	/* 중지 상태가 아닌 경우인지 확인 */
	if (bSucc)
	{
#ifdef _DEBUG
		AfxMessageBox(L"The current device (axis) is moving", MB_ICONSTOP|MB_TOPMOST);
#endif
		return FALSE;
	}
	
	/* 상위 부모에게 버튼 이벤트로 알림 */
	PushBtnEvent();

	/* 현재 Z Axis 높이 값 가져오기 */
	UINT8 u8CamNo		= UINT8(m_enDrvID - ENG_MMDI::en_align_cam1) + 1;
	INT32 i32AxisPos	= uvData_PLC_GetZAxisPosition(u8CamNo);
	UINT32 u32AxisMove	= uvData_PLC_GetAxisMoveRef();
	/* 현재 Up or Down 일 때, 주어 간 값만큼 상대 값으로 이동 */
	if (en_mdmd_move_axis_down == m_enDirect)
	{
		i32AxisPos	-= u32AxisMove;
	}
	else if (en_mdmd_move_axis_up == m_enDirect)
	{
		i32AxisPos	+= u32AxisMove;
	}
	/* 설정된 위치 만큼 이동 */
	uvPLC_CameraZAxisPosition(u8CamNo, i32AxisPos);

	return TRUE;
}
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                       Button String                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 마우스 왼쪽 버튼 클릭 UP
 parm : flags	- [in]  
		point	- [in]  
 retn : TRUE or FALSE
*/
BOOL CBtnString::LButtonUp()
{
	if (m_u8Max <= m_u8Num)	m_u8Num	= m_u8Min;
	else					m_u8Num++;

	SetWindowTextW(m_arrText.GetAt(m_u8Num-1));

	/* 상위 부모에게 버튼 이벤트로 알림 */
	PushBtnEvent();

	return TRUE;
}

/*
 desc : 초기화 설정
 parm : start	- [in]  값 초기 시작 값
		max_i	- [in]  최대 값
		min_i	- [in]  최소 값
 retn : None
*/
VOID CBtnString::SetInit()
{
	m_u8Num	= 1;
	m_u8Max	= (UINT8)m_arrText.GetCount();
	m_u8Min	= 1;

	/* 버튼 출력 */
	SetWindowTextW(m_arrText.GetAt(0));
}

/*
 desc : 화면에 출력되는 문자열 저장
 parm : title	- [in]  문자열 버퍼
 retn : None
*/
VOID CBtnString::PutString(TCHAR *title)
{
	m_arrText.Add(title);
}

/* --------------------------------------------------------------------------------------------- */
/*                                 Button Align Camera Number                                    */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 마우스 왼쪽 버튼 클릭 UP
 parm : flags	- [in]  
		point	- [in]  
 retn : TRUE or FALSE
*/
BOOL CBtnACamNum::LButtonUp()
{
	TCHAR tzACam[32]	= {NULL};
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
	if ((m_u8ACamNum+1) > uvData_Info_GetData()->common->acam_count)	SetTextACam(0x01);
	else																SetTextACam(m_u8ACamNum+1);
#else
	SetTextACam(0x01);
#endif

	/* 상위 부모에게 버튼 이벤트로 알림 */
	PushBtnEvent();

	return TRUE;
}

/*
 desc : 카메라 번호 갱신
 parm : cam_no	- [in]  Camera Number (1 or 2)
 retn : None
*/
VOID CBtnACamNum::SetTextACam(UINT8 cam_no)
{
	TCHAR tzACam[32]	= {NULL};
	m_u8ACamNum = cam_no;
	swprintf_s(tzACam, 32, L"ACam.%d", cam_no);
	SetWindowTextW(tzACam);
}

#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
/*
 desc : 카메라 번호 강제로 변경
 parm : cam_no	- [in]  Camera Number (1 or 2)
 retn : None
*/
VOID CBtnACamNum::SelectACam(UINT8 cam_no)
{
	if (cam_no < 1 || uvData_Info_GetData()->common->acam_count < cam_no)	return;
	SetTextACam(cam_no);
}
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                 Button Align Camera Number                                    */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 마우스 왼쪽 버튼 클릭 UP
 parm : flags	- [in]  
		point	- [in]  
 retn : TRUE or FALSE
*/
BOOL CBtnMarkIdx::LButtonUp()
{
	TCHAR tzTitle[8]	= {NULL};

	if (m_u8MarkIndex == MAX_REGIST_MODEL_COUNT)	m_u8MarkIndex = 0x01;
	else											m_u8MarkIndex++;

	swprintf_s(tzTitle, 8, L"Index.%d", m_u8MarkIndex);
	SetWindowTextW(tzTitle);

	/* 상위 부모에게 버튼 이벤트로 알림 */
	PushBtnEvent();

	return TRUE;
}


/* --------------------------------------------------------------------------------------------- */
/*                                      Button Mark Model                                        */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 마우스 왼쪽 버튼 클릭 UP
 parm : flags	- [in]  
		point	- [in]  
 retn : TRUE or FALSE
*/
BOOL CBtnMarkModel::LButtonUp()
{
	switch (m_u32MarkModel)
	{
	case 8		:	m_u32MarkModel	= 16;
					SetWindowTextW(L"Ellipse");		break;
	case 16		:	m_u32MarkModel	= 32;
					SetWindowTextW(L"Square");		break;
	case 32		:	m_u32MarkModel	= 64;
					SetWindowTextW(L"Rectangle");	break;
	case 64		:	m_u32MarkModel	= 256;
					SetWindowTextW(L"Ring");		break;
	case 256	:	m_u32MarkModel	= 8192;
					SetWindowTextW(L"Cross (+)");	break;
	case 8192	:	m_u32MarkModel	= 65536;
					SetWindowTextW(L"Triangle");	break;
	case 65536	:	m_u32MarkModel	= 8;
					SetWindowTextW(L"Circle");		break;
	}

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                       Button JOb TYpe                                         */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 마우스 왼쪽 버튼 클릭 UP
 parm : flags	- [in]  
		point	- [in]  
 retn : TRUE or FALSE
*/
BOOL CBtnJobType::LButtonUp()
{
	if (m_arrJobType.GetCount() - 1 <= m_u8JobId)	m_u8JobId	= 0x00;
	else											m_u8JobId++;
	SetWindowText(m_arrJobType[m_u8JobId]);

	/* 상위 부모에게 버튼 이벤트로 알림 */
	PushBtnEvent();
	return TRUE;
}

/*
 desc : Job Type Name 등록
 parm : name	- [in]  Job Type Name이 등록된 버퍼
 retn : TRUE or FALSE
*/
BOOL CBtnJobType::AddJobType(TCHAR *name)
{
	if (m_arrJobType.GetCount() > 31)	return FALSE;	/* 더이상 등록 못함 */
	if (wcslen(name) < 1)	return FALSE;
	/* 등록 작업 진행 */
	m_arrJobType.Add(name);

	return TRUE;
}

/*
 desc : Job Index 설정
 parm : job_id	- [in]  1 ~ 32 값 중 하나임
 retn : TRUE or FALSE
*/
BOOL CBtnJobType::SetJobType(UINT8 job_id)
{
	if (job_id < 1 || m_arrJobType.GetCount() < job_id)	return FALSE;
	m_u8JobId	= job_id - 1;
	SetWindowText(m_arrJobType[m_u8JobId]);
	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                  Button Mark Parameter 1                                      */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 마우스 왼쪽 버튼 클릭 UP
 parm : flags	- [in]  
		point	- [in]  
 retn : TRUE or FALSE
*/
BOOL CBtnMarkParam1::LButtonUp()
{
	if (m_u16Param1 == 256)
	{
		m_u16Param1	= 128;
		SetWindowTextW(L"White");
	}
	else
	{
		m_u16Param1	= 256;
		SetWindowTextW(L"Black");
	}

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                 Button Mode (Step) 1 or 2                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 마우스 왼쪽 버튼 클릭 UP
 parm : flags	- [in]  
		point	- [in]  
 retn : TRUE or FALSE
*/
BOOL CBtnStepMode::LButtonUp()
{
	if (m_u8Mode == 1)
	{
		m_u8Mode	= 2;
		SetWindowTextW(L"Mode 2");
	}
	else
	{
		m_u8Mode	= 1;
		SetWindowTextW(L"Mode 1");
	}

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                              Align Mode (Move or Step) 1 or 2                                 */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 마우스 왼쪽 버튼 클릭 UP
 parm : flags	- [in]  
		point	- [in]  
 retn : TRUE or FALSE
*/
BOOL CBtnAlignMode::LButtonUp()
{
	if (m_u8Mode == 1)
	{
		m_u8Mode	= 2;
		SetWindowTextW(L"Align\n[ Step ]");
	}
	else
	{
		m_u8Mode	= 1;
		SetWindowTextW(L"Align\n[ Move ]");
	}

	return TRUE;
}

/*
 desc : 현재 초기 모드에 따른 타이틀 출력
 parm : None
 retn : None
*/
VOID CBtnAlignMode::SetInit()
{
	if (m_u8Mode == 1)	SetWindowTextW(L"Align\n[ Move ]");
	else				SetWindowTextW(L"Align\n[ Step ]");
}

/* --------------------------------------------------------------------------------------------- */
/*                                  Check Box Number (1 ~ 255)                                   */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Number 설정
 parm : num	- [in]  입력할 값 (0x00 ~ 0xff)
		tot	- [in]  전체 개수 (0 값이면, 설정하지 않음)
 retn : None
*/
VOID CBtnNumber::PutNum(UINT32 num, UINT32 tot)
{
	TCHAR tzTitle[256]	= {NULL};
	m_u32Num	= num;
	if (tot)	m_u32Tot = tot;
	if (wcslen(m_tzTitle))
	{
		swprintf_s(tzTitle, 256, L"%s\n%u / %u", m_tzTitle, m_u32Num, m_u32Tot);
		SetWindowTextW(tzTitle);
		Invalidate(TRUE);
	}
}

/*
 desc : 마우스 왼쪽 버튼 클릭 UP
 parm : flags	- [in]  
		point	- [in]  
 retn : TRUE or FALSE
*/
BOOL CBtnNumber::LButtonUp()
{
	if (m_u32Tot)
	{
		if (m_u32Num < m_u32Tot)	m_u32Num++;
		else						m_u32Num=1;
	}
	PutNum(m_u32Num);

	/* 상위 부모에게 알림 */
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)GetSafeHwnd());

	return TRUE;
}