
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
 desc : ������
 parm : None
 retn : None
*/
CEditText::CEditText()
{
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CEditText::~CEditText()
{
}

BEGIN_MESSAGE_MAP(CEditText, CMyEdit)
END_MESSAGE_MAP()

/*
 desc : ��Ʈ���� ����� ��, �ʱ� ȣ���
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
 desc : ��ɾ� �Է¿� ���� ���ڿ� ���
 parm : cmd_num	- [in]  ��ɾ� �� (0 ~ 25)
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
 desc : ��ɾ� �Է¿� ���� ���ڿ� ���
 parm : cmd	- [in]  Command Number
		pos	- [in]  Parameter ��ġ ��ȣ (0 
		val	- [in]  1 byte ��
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

	/* ��� */
	SetWindowTextW(tzValue);
}

/* --------------------------------------------------------------------------------------------- */
/*                        Object 3802 PDO (Actual Value Collective Object)                       */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : �ǽð� Axis Position
 parm : val	- [in]  ��ġ ��
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
	/* �� ���� */
	m_dbPos	= val;
}

/*
 desc : �ǽð� Flag On or Off
 parm : val	- [in]  0 or 1
 retn : None
*/
VOID CEditMC2ActFlag::PutOnOff(UINT8 val)
{
	TCHAR tzValue[2][8]	= {L"Off", L"On"};
	SetWindowTextW(tzValue[val]);
	/* �� ���� */
	m_u8OnOff	= val;
}

/*
 desc : �ǽð� Result
 parm : val	- [in]  Result ?
 retn : None
*/
VOID CEditMC2ActResult::PutResult(INT32 val)
{
	TCHAR tzValue[32]	= {NULL};
	swprintf_s(tzValue, 32, L"%d", val);
	SetWindowTextW(tzValue);
	/* �� ���� */
	m_i32Result	= val;
}

/*
 desc : �ǽð� Error
 parm : val	- [in]  Error Code
 retn : None
*/
VOID CEditMC2ActError::PutError(UINT16 val)
{
	TCHAR tzValue[32]= {NULL};
	swprintf_s(tzValue, 32, L"%d", val);
	SetWindowTextW(tzValue);
	/* �� ���� */
	m_i16Error	= val;
}

/* --------------------------------------------------------------------------------------------- */
/*                                      Object Position Error                                    */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ��ġ ���� �� �ʱ�ȭ
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
 desc : ���� ��ġ ���� �� ��� (���� ���� �� �� ũ�� ���)
 parm : None
 retn : None
*/
VOID CEditMC2PosErr::UpdatePos()
{
	TCHAR tzErr[32]	= {L"0.0 um"};
	/* ���� ����̺��� ��ġ ���� */
	INT32 i32NowPos	= uvData_MC2_GetViewDrvPos(m_enDrvID);
	/* ���� ���� ���� ��ġ ���� ���� ��� */
	UINT32 u32PosErr= abs(INT32(i32NowPos - m_i32PrvPos));

	if (INT32_MAX != m_i32PrvPos)
	{
		/* ��� ���� ������ ��ġ�� ���� ������ ��ġ�� ���� ���� ������ �Էµ� �� �� */
		if (u32PosErr > m_u32MaxErr)
		{
			m_u32MaxErr	= u32PosErr;
			/* ��ġ ���� �� ��� */
			swprintf_s(tzErr, 32, L"%.1f um", DOUBLE(m_u32MaxErr / 10.0f));
			SetWindowTextW(tzErr);
		}
	}

	/* ���� ������ ��ġ �� ���� */
	m_i32PrvPos	= i32NowPos;
}
