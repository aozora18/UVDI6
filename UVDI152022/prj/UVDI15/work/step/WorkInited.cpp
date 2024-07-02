
/*
 desc : ��ü ��� Homing
*/

#include "pch.h"
#include "../../MainApp.h"
#include "WorkInited.h"


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
CWorkInited::CWorkInited()
	: CWorkStep()
{
	m_enWorkJobID	= ENG_BWOK::en_work_init;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CWorkInited::~CWorkInited()
{
}

/*
 desc : �ʱ� �۾� ����
 parm : None
 retn : None
*/
BOOL CWorkInited::InitWork()
{
	/* ���� ��� ���� �� �ʱ�ȭ */
	if (!CWork::InitWork())	return FALSE;

	/* ��ü �۾� �ܰ� */
	//m_u8StepTotal	= 0x1b;
	m_u8StepTotal = 0x17;
	/* �� �糪�������� ó������ ����ϹǷ�, Ÿ��Ʋ ǥ���� ����� �� */
	SetStepName(L"Initialize");

	return TRUE;
}

/*
 desc : �ֱ������� �۾� ����
 parm : None
 retn : None
*/
VOID CWorkInited::DoWork()
{
	/* �۾� �ܰ� ���� ���� ó�� */
	switch (m_u8StepIt)
	{

	case 0x01 : m_enWorkState = CheckSafetyIO();						break;
	/* -------------------------------------------------------------------- */
	/*                      System Device - Init Part                       */
	/* -------------------------------------------------------------------- */
	case 0x02 : m_enWorkState = IsLuriaConnected();						break;
	/* -------------------------------------------------------------------- */
	/*                     Motion Drive - Homing Part                       */
	/* -------------------------------------------------------------------- */
	case 0x03 : m_enWorkState = InitDriveErrorReset();					break;
	case 0x04 : m_enWorkState = IsDriveErrorReseted();					break;
	/* ��� ����̺갡 Homing �����ϴµ� �ּҷ� �ʿ��� ��� �ð� */
	case 0x05 : m_enWorkState = SetWorkWaitTime(1000);					break;
	case 0x06 : m_enWorkState = IsWorkWaitTime();						break;
	/* Stage X / Y / ACam1/2 and PH1/2 Drive�� ���ؼ� Homing ���� */
	case 0x07 : m_enWorkState = DoDriveHomingAll();						break;
	/* Stage X / Y / ACam1/2 and PH1/2 Drive�� Homing �ϴµ� �ּ� ��� (���� �� �ʿ� ������...) */
	case 0x08 : m_enWorkState = SetWorkWaitTime(4000);					break;
	case 0x09 : m_enWorkState = IsWorkWaitTime();						break;
	/* ��� ����̺갡 Homing �Ǿ������� Ȯ�� (ACam2�� ���� ����) */
	case 0x0a : m_enWorkState = IsDrivedHomedAll();						break;
	/* -------------------------------------------------------------------- */
	/*                      Luria Service - Init Part                       */
	/* -------------------------------------------------------------------- */
	/* Luria Service ����� ���Ŀ� H/W �ʱ�ȭ�� �ݵ�� �ؾ��ϰ� */
	/* �׸��� ��� �ʱ�ȭ ���Ŀ��� H/W �ʱ�ȭ�� �ݵ�� �ؾ��ϰ� */
	/* Luria Service �ʱ�ȭ ���� �ּ� ��� �ð� */
	case 0x0b : m_enWorkState = SetWorkWaitTime(2000);					break;
	case 0x0c : m_enWorkState = IsWorkWaitTime();						break;
	case 0x0d: m_enWorkState = InitializeHWInit();						break;
	//case 0x0e: m_enWorkState = IsLuriaValid();							break;
	case 0x0e : m_enWorkState = SetAllPhotoheads();						break;
	case 0x0f : m_enWorkState = IsAllPhotoheads();						break;
	case 0x10 : m_enWorkState = SetMotionControl();						break;
	case 0x11 : m_enWorkState = IsMotionControl();						break;
	case 0x12 : m_enWorkState = SetMotionType1();						break;
	case 0x13 : m_enWorkState = IsMotionType1();						break;
	case 0x14 : m_enWorkState = SetTableSettings();						break;
	case 0x15 : m_enWorkState = IsTableSettings();						break;
	case 0x16 : m_enWorkState = SetSystemSettings();					break;
	case 0x17 : m_enWorkState = IsSystemSettings();						break;
	case 0x18 : m_enWorkState = SetAllPhMotorMoving();					break;
	/* ��� Photohead�� �ش� ��ġ�� �̵��ϴµ� �ʿ��� �ּ� ��� �ð� */
	case 0x19 : m_enWorkState = SetWorkWaitTime(3000);					break;
	case 0x1a : m_enWorkState = IsWorkWaitTime();						break;
	case 0x1b : m_enWorkState = IsAllPhMotorMoved();					break;

	}

	/* ���� �۾� ���� ���� �Ǵ� */
	CWorkInited::SetWorkNext();
	/* ��ð� ���� ���� ��ġ�� �ݺ� �����Ѵٸ� ���� ó�� */
	CWork::CheckWorkTimeout();
}


/*
 desc : ���� �ܰ�� �̵��ϱ� ���� ó��
 parm : None
 retn : None
*/
VOID CWorkInited::SetWorkNext()
{
	UINT64 u64JobTime = GetTickCount64() - m_u64StartTime;

	/* �� �۾� �������� �ð� �� ���� ó�� */
	uvEng_UpdateJobWorkTime(u64JobTime);

	/* ��� �۾��� ���� �Ǿ����� ���� */
	if (ENG_JWNS::en_error == m_enWorkState)
	{
		/*Philhmi�� �ʱ�ȭ ���� ��ȣ ������*/
		PhilInitComp(0x00);

		TCHAR tzMesg[128] = { NULL };
		swprintf_s(tzMesg, 128, L"Work <Error Step It = 0x%02x>", m_u8StepIt);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);

		m_u8StepIt = 0x00;
	}
	else if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* �۾��� ��� �� �ӽ� ���� */
		CalcStepRate();
		/* �۾��� �Ϸ� �Ǿ����� ���� */
		if (m_u8StepTotal == m_u8StepIt)
		{
			m_enWorkState = ENG_JWNS::en_comp;
			/* ���� �޽��� �ʱ�ȭ */
			uvCmn_Logs_ResetErrorMesg();
			/*Philhmi�� �ʱ�ȭ �Ϸ� ��ȣ ������*/
			PhilInitComp(0x01);
			/* �׻� ȣ��*/
			CWork::EndWork();
		}
		else
		{
			/* ���� �۾� �ܰ�� �̵� */
			m_u8StepIt++;
		}
		/* ���� �ֱٿ� Next ������ �ð� ���� */
		m_u64DelayTime = GetTickCount64();
	}
}

/*
 desc : Philhmi�� �ʱ�ȭ �Ϸ� ��ȣ ������
 parm : None
 retn : None
*/
VOID CWorkInited::PhilInitComp(UINT8 state)
{
	STG_PP_P2C_INITIAL_COMPLETE		stInitialComp;
	STG_PP_P2C_INITIAL_COMPLETE_ACK stInitialCompAck;

	stInitialComp.Reset();
	stInitialCompAck.Reset();


	if (state == 0x01)
	{
	}
	if (stInitialCompAck.usErrorCode)
	{
		stInitialComp.usErrorCode = ePHILHMI_ERR_STATUS_INIT;
		//LOG_ERROR(ENG_EDIC::en_uvdi15, L"Philhmi Failure to signal completion of initialization operation");
	}
	uvEng_Philhmi_Send_P2C_INITIAL_COMPLETE(stInitialComp, stInitialCompAck);
}


/*
 desc : �ʱ�ȭ ������ ���� IO üũ
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::CheckSafetyIO()
{

	return ENG_JWNS::en_next;
}



/*
 desc : Safety PLC �ʱ�ȭ (Reset)
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::ResetSafetyPLC()
{
// 	if (!uvCmn_MCQ_IsConnected())
// 	{
// 		LOG_ERROR(ENG_EDIC::en_uvdi15, L"PLC not connected");
// 		return ENG_JWNS::en_error;
// 	}
// 
// 	/* ���� �۾� Step Name ���� */
// 	SetStepName(L"Reset.Safety.PLC");
// 	/* Reset ��ɾ� �۽� */
// 	if (!uvEng_MCQ_SafetyReset())
// 	{
// 		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SafetyReset)");
// 		return ENG_JWNS::en_error;
// 	}

	return ENG_JWNS::en_next;
}

/*
 desc : Luria System Check
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::IsLuriaConnected()
{
	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Luria.Connected");
	return uvCmn_Luria_IsConnected() ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : Luria System Check
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::IsLuriaValid()
{
	TCHAR tzError[64]	= {NULL};

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Luria.Valid");

	/* ���� Luria System�� ������ ������ ���� */
	if (uvCmn_Luria_IsErrorPH() || uvCmn_Luria_IsErrorPC())
	{
#if 0	/* ItfcLuria Library ���� �ֱ������� ��û�ϹǷ� ���� ���⼭ ȣ���� �ʿ� ���� */
		/* �ʹ� ���� ��û�ϸ� �ȵǹǷ� ... */
		if ((m_u64ReqSendTime + 500) < GetTickCount64())
		{
			/* �ֱ� ���� ��û ��� ������ �ð� ���� */
			m_u64ReqSendTime = GetTickCount64();
			/* �ý��� ���� ���� ��û */
			if (!uvLuria_ReqGetSystemStatus())	return ENG_JWNS::en_error;
		}
#endif
		/* Log �̷� ���� */
		swprintf_s(tzError, 64, L"motor <%u> optic <%u> pc <%u>",
				   uvEng_ShMem_GetLuria()->system.get_system_status[0],
				   uvEng_ShMem_GetLuria()->system.get_system_status[1],
				   uvEng_ShMem_GetLuria()->system.get_system_status[2]);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzError);

		return ENG_JWNS::en_wait;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : Luria ���� Machine Config - All Photohead ���� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::SetAllPhotoheads()
{
	LPG_CLSI pstLuriaSvc	= &uvEng_GetConfig()->luria_svc;
	LPG_LDMC pstMemMachine	= &uvEng_ShMem_GetLuria()->machine;

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.All.Optic.Heads");
	/* Luria Info �ʱ�ȭ */
	pstMemMachine->ResetAllPHInfo();	
	/* Photohead �⺻ ���� ���� ��û */
	if (!uvEng_Luria_ReqSetAllPhotoheads(pstLuriaSvc->ph_count,
										 pstLuriaSvc->ph_pitch,
										 pstLuriaSvc->scroll_rate,
										 pstLuriaSvc->ph_rotate))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetAllPhotoheads)");
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : Luria ���� Machine Config - All Photoheads ���� ��� ���� �Ǿ����� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::IsAllPhotoheads()
{
	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.All.Photoheads");
	return uvEng_ShMem_GetLuria()->machine.IsSetAllPHInfo() ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : Luria ���� Machine Config - Spec.Photoheads (Only IPv4) ���� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::SetSpecPhotoheadsIPv4()
{
	LPG_CLSI pstLuriaSvc	= &uvEng_GetConfig()->luria_svc;
	LPG_LDMC pstMemMachine	= &uvEng_ShMem_GetLuria()->machine;

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.Spec.Photoheads.IPv4");

	/* ���� Spec. Photohead IPv4 �ʱ�ȭ */
	memset(pstMemMachine->ph_ipv4[m_u8SetPhNo], 0x00, 4);

	/* Photohead ���� IPv4 �ּ� ���� ��û */
	if (!uvEng_Luria_ReqSetSpecPhotoHeadIPv4(m_u8SetPhNo+1, pstLuriaSvc->ph_ipv4[m_u8SetPhNo]))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetSpecPhotoHeadIPv4)");
		return ENG_JWNS::en_error;
	}
	/* ���� Photohead�� IP �ּҸ� �����ϱ� ���� */
	m_u8SetPhNo++;

	return ENG_JWNS::en_next;
}

/*
 desc : Luria ���� Machine Config - pec.Photoheads ���� ��� ���� �Ǿ����� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::IsSpecPhotoheadsIPv4()
{
	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Spec.Photoheads.IPv4");
	/* ���� Photohead ���� IPv4 �ּҰ� �����Ǿ� �ִ��� ���� */
	return uvEng_ShMem_GetLuria()->machine.ph_ipv4[m_u8SetPhNo] ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : Luria ���� Machine Config - Motion Control ���� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::SetMotionControl()
{
	LPG_CLSI pstLuriaSvc	= &uvEng_GetConfig()->luria_svc;
	LPG_LDMC pstMemMachine	= &uvEng_ShMem_GetLuria()->machine;

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.Motion.Control");
	/* Motion Control �ʱ�ȭ */
	pstMemMachine->ResetMotionControl();

	/* Luria ���񽺿�Motion Control ���� ���� */
	if (!uvEng_Luria_ReqSetMotionControl(0x01/* Sieb&Meyer */,
										 pstLuriaSvc->motion_control_ip,
										 pstLuriaSvc->max_y_motion_speed,
										 pstLuriaSvc->x_motion_speed))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetMotionControl)");
		return ENG_JWNS::en_error;
	}
	return ENG_JWNS::en_next;
}

/*
 desc : Luria ���� Machine Config - Motion Control ���� ��� ���� �Ǿ����� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::IsMotionControl()
{
	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Motion.Control");
	return uvEng_ShMem_GetLuria()->machine.IsMSetotionControl() ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : Luria ���� Machine Config - Motion Control Type1 ���� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::SetMotionType1()
{
	UINT32 u32AccDist		= 0;
	LPG_CLSI pstLuriaSvc	= &uvEng_GetConfig()->luria_svc;
	LPG_LDMC pstMemMachine	= &uvEng_ShMem_GetLuria()->machine;

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.Motion.Type1");
	/* ���� ���� �ʱ�ȭ */
	pstMemMachine->ResetMotionType1();
	/* Motion Type ���� ���� ���� */
	u32AccDist	= (UINT32)ROUNDED(pstLuriaSvc->y_acceleration_distance * 1000.0f, 0);
	if (!uvEng_Luria_ReqSetMotionType1(u32AccDist, pstLuriaSvc->active_table_no))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetMotionType1)");
		return ENG_JWNS::en_error;
	}
	return ENG_JWNS::en_next;
}

/*
 desc : Luria ���� Machine Config - Motion Control Type1 ���� ��� ���� �Ǿ����� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::IsMotionType1()
{
	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Spec.Photoheads.IPv4");
	return uvEng_ShMem_GetLuria()->machine.IsMotionType1() ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : Luria ���� Machine Config - Table Settings ���� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::SetTableSettings()
{
	LPG_CLSI pstLuriaSvc	= &uvEng_GetConfig()->luria_svc;
	LPG_LDMC pstMemMachine	= &uvEng_ShMem_GetLuria()->machine;

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.Table.Settings");
	/* Table Setting �� �ʱ�ȭ */
	pstMemMachine->ResetTableSetting(0x01);
	/* Table Settings ���� ���� */
	if (!uvEng_Luria_ReqSetTableSettings(0x01, pstLuriaSvc->paralleogram_motion_adjust[0x00],
										 pstLuriaSvc->table_1_print_direction))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetTableSettings)");
		return ENG_JWNS::en_error;
	}
	/* �۽� �۾� ���� : Table Settings - Table Position Limits */
	if (!uvEng_Luria_ReqSetTablePositionLimit(0x01,
											  (INT32)ROUNDED(pstLuriaSvc->table_limit_max_xy[0][0]*1000.0f, 0),
											  (INT32)ROUNDED(pstLuriaSvc->table_limit_max_xy[0][1]*1000.0f, 0),
											  (INT32)ROUNDED(pstLuriaSvc->table_limit_min_xy[0][0]*1000.0f, 0),
											  (INT32)ROUNDED(pstLuriaSvc->table_limit_min_xy[0][1]*1000.0f, 0)))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetTablePositionLimit)");
		return ENG_JWNS::en_error;
	}
	return ENG_JWNS::en_next;
}

/*
 desc : Luria ���� Machine Config - Table Settings ���� ��� ���� �Ǿ����� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::IsTableSettings()
{
	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Table.Settings");
	/*return uvCmn_Luria_IsTableSetting(0x01) ? ENG_JWNS::en_next : ENG_JWNS::en_wait;*/
	return ENG_JWNS::en_next;
}

/*
 desc : Luria ���� Machine Config - System Settings ���� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::SetSystemSettings()
{
	UINT8 u8Reset			= uvEng_GetConfig()->IsRunDemo() ? 0x01 : 0x00;
	LPG_CLSI pstLuriaSvc	= &uvEng_GetConfig()->luria_svc;
	LPG_LDMC pstMemMachine	= &uvEng_ShMem_GetLuria()->machine;

	/* ���� �۾� Step Name ���� */
	SetStepName(L"System.Settings");
	/* System Settings ���� �ʱ�ȭ */
	pstMemMachine->ResetSystemSetting(u8Reset);

	/* System Settings - Photohead Rotated */
	if (!uvEng_Luria_ReqSetRotatePhotoheads(pstLuriaSvc->ph_rotate))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetRotatePhotoheads)");
		return ENG_JWNS::en_error;
	}
	/* System Settings - Emulated */
	if (!uvEng_Luria_ReqSetSystemSettings(pstLuriaSvc->emulate_motion,
										  pstLuriaSvc->emulate_ph,
										  pstLuriaSvc->emulate_trigger,
										  pstLuriaSvc->debug_print_level))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetSystemSettings)");
		return ENG_JWNS::en_error;
	}
	return ENG_JWNS::en_next;
}

/*
 desc : Luria ���� Machine Config - System Settings ���� ��� ���� �Ǿ����� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::IsSystemSettings()
{
	UINT8 u8Reset	= uvEng_GetConfig()->IsRunDemo() ? 0x01 : 0x00;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.System.Settings");
	return uvEng_ShMem_GetLuria()->machine.IsSystemSetting(u8Reset) ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : ��� Align Camera�� Z Axis ���� ���� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::SetAllACamMotorReset()
{
	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.All.ACam.Motor.Reset");

	/* Align Camera 1 & 2���� �࿡ �߻��� ���� ���� �ʱ�ȭ */
	// by sysandj : MCQ��ü �߰� �ʿ�
// 	if (!uvEng_MCQ_SetACamResetZAxis(0x03))
// 	{
// 		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SetACamResetZAxis (All))");
// 		return ENG_JWNS::en_error;
// 	}

	return ENG_JWNS::en_next;
}

/*
 desc : ��� Align Camera�� Z Axis ���Ͱ� ���� �Ϸ� �Ǿ����� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::IsAllACamMotorReseted()
{
	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.All.ACam.Motor.Reseted");
	/* ���� Align Camera�� Z Axis ���Ͱ� Homing �Ϸ� �Ǿ����� ���� Ȯ�� */
	// by sysandj : MCQ��ü �߰� �ʿ�
	//if (uvCmn_MCQ_IsACamZAlarm(0x01))	return ENG_JWNS::en_wait;
	//if (uvCmn_MCQ_IsACamZAlarm(0x02))	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : ��� Align Camera�� Z Axis ���� �ʱ�ȭ (Homing)
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::SetAllACamMotorInit()
{
	/* ���� �۾� Step Name ���� */
	SetStepName(L"Set.All.ACam.Motor.Init");

	// by sysandj : MCQ��ü �߰� �ʿ�
// 	if (!uvEng_MCQ_SetACamZHomePosZ(0x01))
// 	{
// 		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SetACamZHomePosZ (0x01))");
// 		return ENG_JWNS::en_error;
// 	}
// 	if (!uvEng_MCQ_SetACamZHomePosZ(0x02))
// 	{
// 		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SetACamZHomePosZ (0x02))");
// 		return ENG_JWNS::en_error;
// 	}

	return ENG_JWNS::en_next;
}

/*
 desc : ��� Align Camera�� Z Axis ���Ͱ� �ʱ�ȭ �Ϸ� �Ǿ����� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::IsAllACamMotorInited()
{
	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.All.ACam.Motor.Inited");
	/* ���� Align Camera�� Z Axis ���Ͱ� Homing �Ϸ� �Ǿ����� ���� Ȯ�� */
	// by sysandj : MCQ��ü �߰� �ʿ�
// 	if (0 != uvCmn_MCQ_GetACamCurrentPosZ(0x01))	return ENG_JWNS::en_wait;
// 	if (0 != uvCmn_MCQ_GetACamCurrentPosZ(0x02))	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : ��� Align Camera�� Z Axis�� �⺻ ��ġ�� �̵� �ϱ�
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::SetAllACamMotorMoving()
{
	/* ���� �۾� Step Name ���� */
	SetStepName(L"All.ACam.Motor.Moving");

	// by sysandj : MCQ��ü �߰� �ʿ�
// 	if (!uvEng_MCQ_SetACamMovePosZ(0x01, 0x00, uvEng_GetConfig()->acam_spec.acam_z_focus[0]))
// 	{
// 		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ACamMovePosZ (0x01))");
// 		return ENG_JWNS::en_error;
// 	}
// 	if (!uvEng_MCQ_SetACamMovePosZ(0x02, 0x00, uvEng_GetConfig()->acam_spec.acam_z_focus[1]))
// 	{
// 		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ACamMovePosZ (0x02))");
// 		return ENG_JWNS::en_error;
// 	}
	return ENG_JWNS::en_next;
}

/*
 desc : ��� Align Camera�� Z Axis�� �⺻ ��ġ�� �̵� �ߴ��� ���� Ȯ��
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::IsAllACamMotorMoved()
{
	INT32 i32PosSet	= 0, i32PosGet = 0;

	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.All.ACam.Motor.Moved");
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

	/* ���� ��ġ�� ���� ��ġ �� �� */
	i32PosSet	= (INT32)ROUNDED(uvEng_GetConfig()->acam_spec.acam_z_focus[0] * 1000.0f, 0);	/* mm -> um */
	i32PosGet = 0;// by sysandj : MCQ��ü �߰� �ʿ� : (INT32)ROUNDED(uvCmn_MCQ_GetACamCurrentPosZ(0x01) * 1000.0f, 0);				/* mm -> um */
	if (i32PosGet != i32PosSet)	return ENG_JWNS::en_wait;
	/* ���� ��ġ�� ���� ��ġ �� �� */
	i32PosSet	= (INT32)ROUNDED(uvEng_GetConfig()->acam_spec.acam_z_focus[1] * 1000.0f, 0);	/* mm -> um */
	i32PosGet = 0;// by sysandj : MCQ��ü �߰� �ʿ� : (INT32)ROUNDED(uvCmn_MCQ_GetACamCurrentPosZ(0x02) * 1000.0f, 0);				/* mm -> um */
	if (i32PosGet != i32PosSet)	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : ������ �µ� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::SetHotAirTemp()
{
	/* ���� �۾� Step Name ���� */
	SetStepName(L"Hot.Air.Temp");

	// by sysandj : MCQ��ü �߰� �ʿ�
// 	if (!uvEng_MCQ_SetHotAirTempSetting(uvEng_GetConfig()->temp_range.hot_air[0]))
// 	{
// 		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SetHotAirTempSetting)");
// 		return ENG_JWNS::en_error;
// 	}
	return ENG_JWNS::en_next;
}

/*
 desc : ����� ���� �µ� ���� �������� ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::IsHotAirTemped()
{
	/* ���� �۾� Step Name ���� */
	if (!IsWorkRepeat())	SetStepName(L"Is.Hot.Air.Temp");
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;
	/*  ������ ������ ���� ���� ������ �µ� ���� �������� .... */
	// by sysandj : MCQ��ü �߰� �ʿ�
// 	if ((UINT16)ROUNDED(uvEng_MCQ_GetHotAirTempSetting(), 0) != 
// 		(UINT16)ROUNDED(uvEng_GetConfig()->temp_range.hot_air[0], 0))
// 	{
// 		return ENG_JWNS::en_wait;;
// 	}

	return ENG_JWNS::en_next;
}

/*
 desc : �ʱ�ȭ ����
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::CheckEquipState()
{
	ENG_JWNS enState	= ENG_JWNS::en_next;

	/* ���� �۾� Step Name ���� */
	SetStepName(L"Check.Equipment.State");
	if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x01)
	{
		/* ���а� ���� ���� ���� ��û */
		if (!uvEng_Luria_ReqGetMotorStateAll())	return ENG_JWNS::en_error;
	}
	/* ��� ��� ����̺갡 ���� �������� ���� */
	if (uvCmn_MC2_IsMotorDriveStopAll())	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}
