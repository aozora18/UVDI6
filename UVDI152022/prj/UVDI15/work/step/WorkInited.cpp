
/*
 desc : 전체 장비 Homing
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
 desc : 생성자
 parm : None
 retn : None
*/
CWorkInited::CWorkInited()
	: CWorkStep()
{
	m_enWorkJobID	= ENG_BWOK::en_work_init;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CWorkInited::~CWorkInited()
{
}

/*
 desc : 초기 작업 수행
 parm : None
 retn : None
*/
BOOL CWorkInited::InitWork()
{
	/* 내부 멤버 변수 값 초기화 */
	if (!CWork::InitWork())	return FALSE;

	/* 전체 작업 단계 */
	//m_u8StepTotal	= 0x1b;
	m_u8StepTotal = 0x17;
	/* 이 사나리에서는 처음부터 대기하므로, 타이틀 표현을 해줘야 함 */
	SetStepName(L"Initialize");

	return TRUE;
}

/*
 desc : 주기적으로 작업 수행
 parm : None
 retn : None
*/
VOID CWorkInited::DoWork()
{
	/* 작업 단계 별로 동작 처리 */
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
	/* 모든 드라이브가 Homing 시작하는데 최소로 필요한 대기 시간 */
	case 0x05 : m_enWorkState = SetWorkWaitTime(1000);					break;
	case 0x06 : m_enWorkState = IsWorkWaitTime();						break;
	/* Stage X / Y / ACam1/2 and PH1/2 Drive에 대해서 Homing 수행 */
	case 0x07 : m_enWorkState = DoDriveHomingAll();						break;
	/* Stage X / Y / ACam1/2 and PH1/2 Drive의 Homing 하는데 최소 대기 (굳이 할 필요 없지만...) */
	case 0x08 : m_enWorkState = SetWorkWaitTime(4000);					break;
	case 0x09 : m_enWorkState = IsWorkWaitTime();						break;
	/* 모든 드라이브가 Homing 되었는지를 확인 (ACam2가 제일 늦음) */
	case 0x0a : m_enWorkState = IsDrivedHomedAll();						break;
	/* -------------------------------------------------------------------- */
	/*                      Luria Service - Init Part                       */
	/* -------------------------------------------------------------------- */
	/* Luria Service 재시작 이후에 H/W 초기화를 반드시 해야하고 */
	/* 그리고 모션 초기화 이후에도 H/W 초기화를 반드시 해야하고 */
	/* Luria Service 초기화 응답 최소 대기 시간 */
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
	/* 모든 Photohead가 해당 위치로 이동하는데 필요한 최소 대기 시간 */
	case 0x19 : m_enWorkState = SetWorkWaitTime(3000);					break;
	case 0x1a : m_enWorkState = IsWorkWaitTime();						break;
	case 0x1b : m_enWorkState = IsAllPhMotorMoved();					break;

	}

	/* 다음 작업 진행 여부 판단 */
	CWorkInited::SetWorkNext();
	/* 장시간 동안 동일 위치를 반복 수행한다면 에러 처리 */
	CWork::CheckWorkTimeout();
}


/*
 desc : 다음 단계로 이동하기 위한 처리
 parm : None
 retn : None
*/
VOID CWorkInited::SetWorkNext()
{
	UINT64 u64JobTime = GetTickCount64() - m_u64StartTime;

	/* 매 작업 구간마다 시간 값 증가 처리 */
	uvEng_UpdateJobWorkTime(u64JobTime);

	/* 모든 작업이 종료 되었는지 여부 */
	if (ENG_JWNS::en_error == m_enWorkState)
	{
		/*Philhmi에 초기화 실패 신호 보내기*/
		PhilInitComp(0x00);

		TCHAR tzMesg[128] = { NULL };
		swprintf_s(tzMesg, 128, L"Work <Error Step It = 0x%02x>", m_u8StepIt);
		LOG_ERROR(ENG_EDIC::en_uvdi15, tzMesg);

		m_u8StepIt = 0x00;
	}
	else if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* 작업률 계산 후 임시 저장 */
		CalcStepRate();
		/* 작업이 완료 되었는지 여부 */
		if (m_u8StepTotal == m_u8StepIt)
		{
			m_enWorkState = ENG_JWNS::en_comp;
			/* 에러 메시지 초기화 */
			uvCmn_Logs_ResetErrorMesg();
			/*Philhmi에 초기화 완료 신호 보내기*/
			PhilInitComp(0x01);
			/* 항상 호출*/
			CWork::EndWork();
		}
		else
		{
			/* 다음 작업 단계로 이동 */
			m_u8StepIt++;
		}
		/* 가장 최근에 Next 수행한 시간 저장 */
		m_u64DelayTime = GetTickCount64();
	}
}

/*
 desc : Philhmi에 초기화 완료 신호 보내기
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
 desc : 초기화 가능한 상태 IO 체크
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::CheckSafetyIO()
{

	return ENG_JWNS::en_next;
}



/*
 desc : Safety PLC 초기화 (Reset)
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
// 	/* 현재 작업 Step Name 설정 */
// 	SetStepName(L"Reset.Safety.PLC");
// 	/* Reset 명령어 송신 */
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
	/* 현재 작업 Step Name 설정 */
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

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Luria.Valid");

	/* 현재 Luria System에 문제가 없는지 여부 */
	if (uvCmn_Luria_IsErrorPH() || uvCmn_Luria_IsErrorPC())
	{
#if 0	/* ItfcLuria Library 에서 주기적으로 요청하므로 굳이 여기서 호출할 필요 없음 */
		/* 너무 자주 요청하면 안되므로 ... */
		if ((m_u64ReqSendTime + 500) < GetTickCount64())
		{
			/* 최근 상태 요청 명령 전송한 시간 저장 */
			m_u64ReqSendTime = GetTickCount64();
			/* 시스템 상태 정보 요청 */
			if (!uvLuria_ReqGetSystemStatus())	return ENG_JWNS::en_error;
		}
#endif
		/* Log 이력 저장 */
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
 desc : Luria 내의 Machine Config - All Photohead 정보 설정
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::SetAllPhotoheads()
{
	LPG_CLSI pstLuriaSvc	= &uvEng_GetConfig()->luria_svc;
	LPG_LDMC pstMemMachine	= &uvEng_ShMem_GetLuria()->machine;

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.All.Optic.Heads");
	/* Luria Info 초기화 */
	pstMemMachine->ResetAllPHInfo();	
	/* Photohead 기본 정보 설정 요청 */
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
 desc : Luria 내의 Machine Config - All Photoheads 쪽이 모두 수신 되었는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::IsAllPhotoheads()
{
	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.All.Photoheads");
	return uvEng_ShMem_GetLuria()->machine.IsSetAllPHInfo() ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : Luria 내의 Machine Config - Spec.Photoheads (Only IPv4) 정보 설정
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::SetSpecPhotoheadsIPv4()
{
	LPG_CLSI pstLuriaSvc	= &uvEng_GetConfig()->luria_svc;
	LPG_LDMC pstMemMachine	= &uvEng_ShMem_GetLuria()->machine;

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.Spec.Photoheads.IPv4");

	/* 기존 Spec. Photohead IPv4 초기화 */
	memset(pstMemMachine->ph_ipv4[m_u8SetPhNo], 0x00, 4);

	/* Photohead 별로 IPv4 주소 설정 요청 */
	if (!uvEng_Luria_ReqSetSpecPhotoHeadIPv4(m_u8SetPhNo+1, pstLuriaSvc->ph_ipv4[m_u8SetPhNo]))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetSpecPhotoHeadIPv4)");
		return ENG_JWNS::en_error;
	}
	/* 다음 Photohead의 IP 주소를 설정하기 위함 */
	m_u8SetPhNo++;

	return ENG_JWNS::en_next;
}

/*
 desc : Luria 내의 Machine Config - pec.Photoheads 쪽이 모두 수신 되었는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::IsSpecPhotoheadsIPv4()
{
	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Spec.Photoheads.IPv4");
	/* 현재 Photohead 마다 IPv4 주소가 설정되어 있는지 여부 */
	return uvEng_ShMem_GetLuria()->machine.ph_ipv4[m_u8SetPhNo] ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : Luria 내의 Machine Config - Motion Control 정보 설정
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::SetMotionControl()
{
	LPG_CLSI pstLuriaSvc	= &uvEng_GetConfig()->luria_svc;
	LPG_LDMC pstMemMachine	= &uvEng_ShMem_GetLuria()->machine;

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.Motion.Control");
	/* Motion Control 초기화 */
	pstMemMachine->ResetMotionControl();

	/* Luria 서비스에Motion Control 정보 설정 */
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
 desc : Luria 내의 Machine Config - Motion Control 쪽이 모두 수신 되었는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::IsMotionControl()
{
	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Motion.Control");
	return uvEng_ShMem_GetLuria()->machine.IsMSetotionControl() ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : Luria 내의 Machine Config - Motion Control Type1 정보 설정
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::SetMotionType1()
{
	UINT32 u32AccDist		= 0;
	LPG_CLSI pstLuriaSvc	= &uvEng_GetConfig()->luria_svc;
	LPG_LDMC pstMemMachine	= &uvEng_ShMem_GetLuria()->machine;

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.Motion.Type1");
	/* 기존 정보 초기화 */
	pstMemMachine->ResetMotionType1();
	/* Motion Type 정보 새로 설정 */
	u32AccDist	= (UINT32)ROUNDED(pstLuriaSvc->y_acceleration_distance * 1000.0f, 0);
	if (!uvEng_Luria_ReqSetMotionType1(u32AccDist, pstLuriaSvc->active_table_no))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetMotionType1)");
		return ENG_JWNS::en_error;
	}
	return ENG_JWNS::en_next;
}

/*
 desc : Luria 내의 Machine Config - Motion Control Type1 쪽이 모두 수신 되었는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::IsMotionType1()
{
	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Spec.Photoheads.IPv4");
	return uvEng_ShMem_GetLuria()->machine.IsMotionType1() ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : Luria 내의 Machine Config - Table Settings 정보 설정
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::SetTableSettings()
{
	LPG_CLSI pstLuriaSvc	= &uvEng_GetConfig()->luria_svc;
	LPG_LDMC pstMemMachine	= &uvEng_ShMem_GetLuria()->machine;

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.Table.Settings");
	/* Table Setting 값 초기화 */
	pstMemMachine->ResetTableSetting(0x01);
	/* Table Settings 정보 설정 */
	if (!uvEng_Luria_ReqSetTableSettings(0x01, pstLuriaSvc->paralleogram_motion_adjust[0x00],
										 pstLuriaSvc->table_1_print_direction))
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetTableSettings)");
		return ENG_JWNS::en_error;
	}
	/* 송신 작업 진행 : Table Settings - Table Position Limits */
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
 desc : Luria 내의 Machine Config - Table Settings 쪽이 모두 수신 되었는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::IsTableSettings()
{
	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Table.Settings");
	/*return uvCmn_Luria_IsTableSetting(0x01) ? ENG_JWNS::en_next : ENG_JWNS::en_wait;*/
	return ENG_JWNS::en_next;
}

/*
 desc : Luria 내의 Machine Config - System Settings 정보 설정
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::SetSystemSettings()
{
	UINT8 u8Reset			= uvEng_GetConfig()->IsRunDemo() ? 0x01 : 0x00;
	LPG_CLSI pstLuriaSvc	= &uvEng_GetConfig()->luria_svc;
	LPG_LDMC pstMemMachine	= &uvEng_ShMem_GetLuria()->machine;

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"System.Settings");
	/* System Settings 정보 초기화 */
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
 desc : Luria 내의 Machine Config - System Settings 쪽이 모두 수신 되었는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::IsSystemSettings()
{
	UINT8 u8Reset	= uvEng_GetConfig()->IsRunDemo() ? 0x01 : 0x00;

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.System.Settings");
	return uvEng_ShMem_GetLuria()->machine.IsSystemSetting(u8Reset) ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : 모든 Align Camera의 Z Axis 모터 에러 리셋
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::SetAllACamMotorReset()
{
	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.All.ACam.Motor.Reset");

	/* Align Camera 1 & 2번의 축에 발생된 에러 값을 초기화 */
	// by sysandj : MCQ대체 추가 필요
// 	if (!uvEng_MCQ_SetACamResetZAxis(0x03))
// 	{
// 		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SetACamResetZAxis (All))");
// 		return ENG_JWNS::en_error;
// 	}

	return ENG_JWNS::en_next;
}

/*
 desc : 모든 Align Camera의 Z Axis 모터가 리셋 완료 되었는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::IsAllACamMotorReseted()
{
	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.All.ACam.Motor.Reseted");
	/* 현재 Align Camera의 Z Axis 모터가 Homing 완료 되었는지 여부 확인 */
	// by sysandj : MCQ대체 추가 필요
	//if (uvCmn_MCQ_IsACamZAlarm(0x01))	return ENG_JWNS::en_wait;
	//if (uvCmn_MCQ_IsACamZAlarm(0x02))	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : 모든 Align Camera의 Z Axis 모터 초기화 (Homing)
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::SetAllACamMotorInit()
{
	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Set.All.ACam.Motor.Init");

	// by sysandj : MCQ대체 추가 필요
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
 desc : 모든 Align Camera의 Z Axis 모터가 초기화 완료 되었는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::IsAllACamMotorInited()
{
	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.All.ACam.Motor.Inited");
	/* 현재 Align Camera의 Z Axis 모터가 Homing 완료 되었는지 여부 확인 */
	// by sysandj : MCQ대체 추가 필요
// 	if (0 != uvCmn_MCQ_GetACamCurrentPosZ(0x01))	return ENG_JWNS::en_wait;
// 	if (0 != uvCmn_MCQ_GetACamCurrentPosZ(0x02))	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : 모든 Align Camera의 Z Axis을 기본 위치로 이동 하기
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::SetAllACamMotorMoving()
{
	/* 현재 작업 Step Name 설정 */
	SetStepName(L"All.ACam.Motor.Moving");

	// by sysandj : MCQ대체 추가 필요
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
 desc : 모든 Align Camera의 Z Axis이 기본 위치로 이동 했는지 여부 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::IsAllACamMotorMoved()
{
	INT32 i32PosSet	= 0, i32PosGet = 0;

	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.All.ACam.Motor.Moved");
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;

	/* 현재 위치와 설정 위치 값 비교 */
	i32PosSet	= (INT32)ROUNDED(uvEng_GetConfig()->acam_spec.acam_z_focus[0] * 1000.0f, 0);	/* mm -> um */
	i32PosGet = 0;// by sysandj : MCQ대체 추가 필요 : (INT32)ROUNDED(uvCmn_MCQ_GetACamCurrentPosZ(0x01) * 1000.0f, 0);				/* mm -> um */
	if (i32PosGet != i32PosSet)	return ENG_JWNS::en_wait;
	/* 현재 위치와 설정 위치 값 비교 */
	i32PosSet	= (INT32)ROUNDED(uvEng_GetConfig()->acam_spec.acam_z_focus[1] * 1000.0f, 0);	/* mm -> um */
	i32PosGet = 0;// by sysandj : MCQ대체 추가 필요 : (INT32)ROUNDED(uvCmn_MCQ_GetACamCurrentPosZ(0x02) * 1000.0f, 0);				/* mm -> um */
	if (i32PosGet != i32PosSet)	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : 온조기 온도 설정
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::SetHotAirTemp()
{
	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Hot.Air.Temp");

	// by sysandj : MCQ대체 추가 필요
// 	if (!uvEng_MCQ_SetHotAirTempSetting(uvEng_GetConfig()->temp_range.hot_air[0]))
// 	{
// 		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (SetHotAirTempSetting)");
// 		return ENG_JWNS::en_error;
// 	}
	return ENG_JWNS::en_next;
}

/*
 desc : 현재와 이전 온도 값이 동일한지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::IsHotAirTemped()
{
	/* 현재 작업 Step Name 설정 */
	if (!IsWorkRepeat())	SetStepName(L"Is.Hot.Air.Temp");
	if (uvEng_GetConfig()->IsRunDemo())	return ENG_JWNS::en_next;
	/*  이전에 설정한 값과 현재 설정된 온도 값이 동일한지 .... */
	// by sysandj : MCQ대체 추가 필요
// 	if ((UINT16)ROUNDED(uvEng_MCQ_GetHotAirTempSetting(), 0) != 
// 		(UINT16)ROUNDED(uvEng_GetConfig()->temp_range.hot_air[0], 0))
// 	{
// 		return ENG_JWNS::en_wait;;
// 	}

	return ENG_JWNS::en_next;
}

/*
 desc : 초기화 진행
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkInited::CheckEquipState()
{
	ENG_JWNS enState	= ENG_JWNS::en_next;

	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Check.Equipment.State");
	if (uvEng_GetConfig()->luria_svc.z_drive_type == 0x01)
	{
		/* 광학계 모터 상태 정보 요청 */
		if (!uvEng_Luria_ReqGetMotorStateAll())	return ENG_JWNS::en_error;
	}
	/* 모든 모션 드라이브가 중지 상태인지 여부 */
	if (uvCmn_MC2_IsMotorDriveStopAll())	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}
