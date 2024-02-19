
/*
 desc : Input Values
*/

#include "stdafx.h"
#include "EditText.h"

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
CEditText::CEditText()
{
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CEditText::~CEditText()
{
}

BEGIN_MESSAGE_MAP(CEditText, CMyEdit)
END_MESSAGE_MAP()

/*
 desc : 컨트롤이 수행될 때, 초기 호출됨
 parm : None
 retn : None
*/
VOID CEditText::PreSubclassWindow()
{

	CMyEdit::PreSubclassWindow();
}

/* --------------------------------------------------------------------------------------------- */
/*                       Object 3801 PDO (Reference Value Collective Object)                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 명령어 입력에 따른 문자열 출력
 parm : cmd_num	- [in]  명령어 값 (0 ~ 25)
 retn : None
*/
VOID CEditMC2RefCmd::PutCmd(UINT8 cmd_num)
{
	switch (cmd_num)
	{
	case en_mccn_do_nothing				 :	SetWindowTextW(L"nothing");					break;
	case en_mccn_do_homing				 :	SetWindowTextW(L"homing");					break;
	case en_mccn_set_position_limits	 :	SetWindowTextW(L"set_position_limits");		break;
	case en_mccn_set_velocity			 :	SetWindowTextW(L"set_velocity");			break;
	case en_mccn_set_acceleration		 :	SetWindowTextW(L"set_acceleration");		break;
	case en_mccn_go_position			 :	SetWindowTextW(L"go_position");				break;
	case en_mccn_cancel_command			 :	SetWindowTextW(L"cancel_command");			break;
	case en_mccn_do_power_on			 :	SetWindowTextW(L"power_on");				break;
	case en_mccn_do_power_off			 :	SetWindowTextW(L"power_off");				break;
	case en_mccn_set_jerk				 :	SetWindowTextW(L"set_jerk");				break;
	case en_mccn_set_position_window	 :	SetWindowTextW(L"set_position_window");		break;
	case en_mccn_set_puls_output		 :	SetWindowTextW(L"set_puls_output");			break;
	case en_mccn_do_arc					 :	SetWindowTextW(L"arc");						break;
	case en_mccn_do_vector				 :	SetWindowTextW(L"vector");					break;
	case en_mccn_set_gear_mode			 :	SetWindowTextW(L"set_gear_mode");			break;
	case en_mccn_restart_drive			 :	SetWindowTextW(L"restart_drive");			break;
	case en_mccn_set_cal_off_set		 :	SetWindowTextW(L"set_cal_off_set");			break;
	case en_mccn_go_velocity			 :	SetWindowTextW(L"go_velocity");				break;
	case en_mccn_fault_reset			 :	SetWindowTextW(L"fault_reset");				break;
	case en_mccn_messure_cutting_edge	 :	SetWindowTextW(L"messure_cutting_edge");	break;
	}
}

/*
 desc : 명령어 입력에 따른 문자열 출력
 parm : cmd	- [in]  Command Number
		pos	- [in]  Parameter 위치 번호 (0 
		val	- [in]  1 byte 값
 retn : None
*/
VOID CEditMC2RefParam::PutParam(UINT8 cmd, UINT8 pos, INT32 val)
{
	TCHAR tzValue[32]	= {NULL};

	switch (cmd)
	{
	case en_mccn_do_homing				 :
		if (pos == 1)			swprintf_s(tzValue, 32, L"%d", val);						break;
	case en_mccn_set_position_limits	 :
		if (pos > 0 && pos < 3)	swprintf_s(tzValue, 32, L"%.1f", val / 10.0f);				break;
	case en_mccn_set_velocity			:
		if (pos == 1)			swprintf_s(tzValue, 32, L"%d", val);						break;
	case en_mccn_set_acceleration		 :
		if (pos == 1)			swprintf_s(tzValue, 32, L"%d", val);						break;
	case en_mccn_go_position			 :
		if (pos == 1)			swprintf_s(tzValue, 32, L"%.4f mm ", val / 10000.0f);		break;
	case en_mccn_set_jerk				 :
		if (pos == 1)			swprintf_s(tzValue, 32, L"%d", val);						break;
	case en_mccn_set_position_window	 :
		if (pos == 1)			swprintf_s(tzValue, 32, L"%d", val);						break;
	case en_mccn_set_puls_output		 :
		if (pos == 1)
		{
			if (val == 0)		swprintf_s(tzValue, 32, L"Off");
			else				swprintf_s(tzValue, 32, L"On");
		}
		break;
	case en_mccn_do_arc					 :
		if (pos == 0)			swprintf_s(tzValue, 32, L"S.Drv. : %d", val);
		else if (pos == 1)		swprintf_s(tzValue, 32, L"Rad. : %.1f", val / 10.0f);
		else if (pos == 2)		swprintf_s(tzValue, 32, L"Start Ang. : %.1f", val / 10.0f);
		else if (pos == 3)		swprintf_s(tzValue, 32, L"End Ang. : %.1f", val / 10.0f);
		break;
	case en_mccn_do_vector				 :
		if (pos == 0)			swprintf_s(tzValue, 32, L"S.Drv. : %d", val);
		else if (pos == 1)		swprintf_s(tzValue, 32, L"Tgt Pos. : %.1f", val / 10.0f);
		else if (pos == 2)		swprintf_s(tzValue, 32, L"Tgt Pos.S. : %.1f", val / 10.0f);
		else if (pos == 3)		swprintf_s(tzValue, 32, L"End Ang. : %d", val);
		break;
	case en_mccn_set_gear_mode			 :
		if (pos == 0)			swprintf_s(tzValue, 32, L"M.Drv. : %d", val);
		else if (pos == 1)
		{
			if (val == 0)		swprintf_s(tzValue, 32, L"Off");
			else				swprintf_s(tzValue, 32, L"On");
		}
		else if (pos == 2)		swprintf_s(tzValue, 32, L"Cycle.Len. : %.1f", val / 10.0f);
		else if (pos == 3)		swprintf_s(tzValue, 32, L"M.Cycle Len. : %.1f", val / 10.0f);
		break;
	case en_mccn_set_cal_off_set		 :
		if (pos == 1)			swprintf_s(tzValue, 32, L"Cal.Offset : %d", val);			break;
	case en_mccn_go_velocity			 :
		if (pos == 1)			swprintf_s(tzValue, 32, L"Tgt.Velo. : %.1f", val / 10.0f);	break;

	case en_mccn_do_nothing				 :	wcscpy_s(tzValue, 32, L"nothing");		break;
	case en_mccn_cancel_command			 :	wcscpy_s(tzValue, 32, L"cancel");		break;
	case en_mccn_do_power_on			 :	wcscpy_s(tzValue, 32, L"power_on");		break;
	case en_mccn_do_power_off			 :	wcscpy_s(tzValue, 32, L"power_off");	break;
	case en_mccn_restart_drive			 :	wcscpy_s(tzValue, 32, L"restart");		break;
	case en_mccn_fault_reset			 :	wcscpy_s(tzValue, 32, L"fault_reset");	break;
	case en_mccn_messure_cutting_edge	 :	wcscpy_s(tzValue, 32, L"cutting_edge");	break;
	}

	/* 출력 */
	SetWindowTextW(tzValue);
}

/* --------------------------------------------------------------------------------------------- */
/*                        Object 3802 PDO (Actual Value Collective Object)                       */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 실시간 Axis Position
 parm : val	- [in]  위치 값
 retn : None
*/
VOID CEditMC2ActPos::PutPos(DOUBLE val)
{
	TCHAR tzValue[32]	= {NULL};
#if 0
	swprintf_s(tzValue, 32, L"%.4f mm ", val);
#else
	swprintf_s(tzValue, 32, L"%.4f ", val);
#endif
	SetWindowTextW(tzValue);
	/* 값 저장 */
	m_dbPos	= val;
}

/*
 desc : 실시간 Flag On or Off
 parm : val	- [in]  0 or 1
 retn : None
*/
VOID CEditMC2ActFlag::PutOnOff(UINT8 val)
{
	TCHAR tzValue[2][8]	= {L"Off", L"On"};
	SetWindowTextW(tzValue[val]);
	/* 값 저장 */
	m_u8OnOff	= val;
}

/*
 desc : 실시간 Result
 parm : val	- [in]  Result ?
 retn : None
*/
VOID CEditMC2ActResult::PutResult(INT32 val)
{
	TCHAR tzValue[32]	= {NULL};
	swprintf_s(tzValue, 32, L"%d", val);
	SetWindowTextW(tzValue);
	/* 값 저장 */
	m_i32Result	= val;
}

/*
 desc : 실시간 Error
 parm : val	- [in]  Error Code
 retn : None
*/
VOID CEditMC2ActError::PutError(UINT16 val)
{
	TCHAR tzValue[32]= {NULL};
	swprintf_s(tzValue, 32, L"%d", val);
	SetWindowTextW(tzValue);
	/* 값 저장 */
	m_i16Error	= val;
}

/* --------------------------------------------------------------------------------------------- */
/*                                      Object Position Error                                    */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 위치 오차 값 초기화
 parm : None
 retn : None
*/
VOID CEditMC2PosErr::ResetPos()
{
	m_i32PrvPos	= INT32_MAX;
	m_u32MaxErr	= 0;
	SetWindowTextW(L"0.0 um");
}

/*
 desc : 현재 위치 오차 값 계산 (이전 값과 비교 후 크면 출력)
 parm : None
 retn : None
*/
VOID CEditMC2PosErr::UpdatePos()
{
	TCHAR tzErr[32]	= {L"0.0 um"};
	/* 현재 드라이브의 위치 정보 */
	INT32 i32NowPos	= uvData_MC2_GetViewDrvPos(m_enDrvID);
	/* 기존 값과 현재 위치 값의 차이 계산 */
	UINT32 u32PosErr= abs(INT32(i32NowPos - m_i32PrvPos));

	if (INT32_MAX != m_i32PrvPos)
	{
		/* 방금 전에 수집된 위치와 현재 수집된 위치의 차이 값과 기존에 입력된 값 비교 */
		if (u32PosErr > m_u32MaxErr)
		{
			m_u32MaxErr	= u32PosErr;
			/* 위치 오차 값 출력 */
			swprintf_s(tzErr, 32, L"%.1f um", DOUBLE(m_u32MaxErr / 10.0f));
			SetWindowTextW(tzErr);
		}
	}

	/* 현재 수집된 위치 값 저장 */
	m_i32PrvPos	= i32NowPos;
}
