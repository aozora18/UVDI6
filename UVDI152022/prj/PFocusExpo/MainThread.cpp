
/*
 desc : Main Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "MainThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : parent	- [in]  자신을 호출한 부모 윈도 핸들
 retn : None
*/
CMainThread::CMainThread(HWND parent)
	: CThinThread()
{
	m_hParent			= parent;
	m_u8WorkStep		= 0x00;
	m_bRunPrint			= FALSE;
	m_enWorkState		= ENG_JWNS::en_none;
	m_u64PeridPktTime	= GetTickCount64();
	m_bReqPeriodMC2		= FALSE;
}

/*
 desc : 파괴자
 parm : None
*/
CMainThread::~CMainThread()
{
}

/*
 desc : 스레드 동작시에 초기 한번 호출됨
 parm : None
 retn : DI_TRUE or DI_FALSE
*/
BOOL CMainThread::StartWork()
{
	/* 무조건 1 초 정도 쉬고 동작하도록 함 */
	CThinThread::Sleep(1000);

	return TRUE;
}

/*
 desc : 스레드 동작시에 주기적으로 호출됨
 parm : None
 retn : None
*/
VOID CMainThread::RunWork()
{
	/* 일반적인 모니터링 호출 */
	SendMesgNormal(100);
	/* 동기 진입 */
	if (m_syncPrint.Enter())
	{
		/* 포커스 작업 수행 */
		if (m_bRunPrint)	RunPrint();
		else				ReqPeriodPkt();

		/* 동기 해제 */
		m_syncPrint.Leave();
	}
}

/*
 desc : 스레드 종료시에 마지막으로 한번 호출됨
 parm : None
 retn : None
*/
VOID CMainThread::EndWork()
{
}

/*
 desc : 부모에게 메시지 전달 (SendMessage)
 parm : timeout	- [in]  SendMessag Lifetime (unit: msec)
 retn : None
*/
VOID CMainThread::SendMesgNormal(UINT32 timeout)
{
	DWORD_PTR dwResult	= 0;
	LRESULT lResult		= 0;

	/* 부모에게 이벤트 메시지 전달 */
	lResult	= ::SendMessageTimeout(m_hParent, WM_MAIN_THREAD, MSG_ID_NORMAL, 0L,
								   SMTO_NORMAL, timeout, &dwResult);
	if (0 == lResult)
	{
#if 0
		/* 현재 발생된 에러 값 저장 */
		TRACE("MainThread : SendMessage Time out <Normal> = %d \n", GetLastError());
#endif
	}
}

/*
 desc : 부모에게 메시지 전달 (SendMessage)
 parm : timeout	- [in]  SendMessag Lifetime (unit: msec)
		flag	- [in]  FALSE : 노광 1회 완료, TRUE : 노광 전체 완료
 retn : None
*/
VOID CMainThread::SendMesgPrint(BOOL flag, UINT32 timeout)
{
	WPARAM wParam		= flag ? MSG_ID_PRINT_COMPLETE : MSG_ID_PRINT_ONE;
	DWORD_PTR dwResult	= 0;
	LRESULT lResult		= 0;

	/* 부모에게 이벤트 메시지 전달 */
	lResult	= ::SendMessageTimeout(m_hParent, WM_MAIN_THREAD, wParam, 0L,
								   SMTO_NORMAL, timeout, &dwResult);
	if (0 == lResult)
	{
#if 0
		/* 현재 발생된 에러 값 저장 */
		TRACE("MainThread : SendMessage Time out <Normal> = %d \n", GetLastError());
#endif
	}
}

/*
 desc : 노광 진행
 parm : expo_x	- [in]  노광 시작 위치 (단위: um)
		expo_y	- [in]  노광 시작 위치 (단위: um)
		period_z- [in]  광학계 Z 축 이동 간격 (단위: um)
		period_y- [in]  스테이지 Y 축 이동 간격 (단위: 0.1 um or 100 nm)
		repeat	- [in]  총 반복 노광 횟수
 retn : None
*/
VOID CMainThread::StartPrint(DOUBLE expo_x, DOUBLE expo_y,
							 DOUBLE period_z, DOUBLE period_y, UINT16 repeat)
{
	UINT8 i	= 0x00;

	/* 동기 진입 */
	if (m_syncPrint.Enter())
	{
		m_u16PrintTotal	= repeat;
		m_u16PrintCount	= 0;
		m_dbPeriodZ		= period_z;
		m_dbPeriodY		= period_y;
		m_u8WorkStep	= 0x01;
		m_u8WorkTotal	= 0x0e;
		/* 노광 시작 위치 얻기 */
		m_dbExpoX		= expo_x/*uvEng_ShMem_GetLuria()->machine.table_expo_start_xy[0].x*/;
		m_dbExpoY		= expo_y/*uvEng_ShMem_GetLuria()->machine.table_expo_start_xy[0].y*/;
		/* 현재 모든 Photohead의 Z Axis 위치 (높이) 값 저장 (단위: um) */
		for (i=0x00; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
		{
			m_dbPhBaseZ[i]	= uvEng_ShMem_GetLuria()->directph.focus_motor_move_abs_position[i] / 1000.0f;
		}
		/* 현재 작업 시작한 시점 시간 저장 */
		m_u64DelayTime		= GetTickCount64();
		/* 동작 시작 */
		m_bRunPrint			= TRUE;

		/* 동기 해제 */
		m_syncPrint.Leave();
	}
}

/*
 desc : 작업 강제로 중지
 parm : None
 retn : None
*/
VOID CMainThread::StopPrint()
{
	/* 동기 진입 */
	if (m_syncPrint.Enter())
	{
		m_bRunPrint	= FALSE;
		/* 동기 해제 */
		m_syncPrint.Leave();
	}
}

/*
 desc : Specification 측정 작업 수행
 parm : None
 retn : None
*/
VOID CMainThread::RunPrint()
{
	/* 작업 단계 별로 동작 처리 */
	switch (m_u8WorkStep)
	{
	/* Luria 서비스에 노광 시작 위치 설정 */
	case 0x01	:	m_enWorkState	= SetExpoStartXY();		break;
	/* 노광 시작 위치에 설정 값이 변경되는데, 최소 대기 시간 */
	case 0x02	:	m_enWorkState	= IsWorkWaitTime();		break;
	/* Luria 서비스에 노광 시작 위치 설정 했는지 확인 */
	case 0x03	:	m_enWorkState	= IsExpoStartXY();		break;
	/* 모든 Photohead Z Axis Homing (Reset) (Move to Mid-Position) */
	case 0x04	:	m_enWorkState	= SetPhZAxisHoming();	break;
	/* 모든 Photohead Z Axis이 현재 Homing 상태 중인지 확인 */
	case 0x05	:	m_enWorkState	= IsPhZAxisHoming();	break;
	/* 모든 Photohead Z Axis Homing (Reset) 완료 했는지 확인 */
	case 0x06	:	m_enWorkState	= IsPhZAxisHomed();		break;
	/* 모든 Photohead Z Axis 노광 위치로 이동 */
	case 0x07	:	m_enWorkState	= SetPhZAxisMoving();	break;
	/* 모든 Photohead Z Axis이 이동하는데 최소 대기 시간만큼 기다림 */
	case 0x08	:	m_enWorkState	= IsWorkWaitTime();		break;
	/* 모든 Photohead Z Axis이 노광 위치로 이동 했는지 확인 */
	case 0x09	:	m_enWorkState	= IsPhZAxisMoved();		break;
	/* 노광 준비 */
	case 0x0a	:	m_enWorkState	= SetExpoReady();		break;
	/* 노광 준비 되었는지 확인 */
	case 0x0b	:	m_enWorkState	= IsExpoReady();		break;
	/* 노광 진행 */
	case 0x0c	:	m_enWorkState	= SetExpoPrinting();	break;
	/* 노광 시작이 수행되기 까지, 잠시 대기 */
	case 0x0d	:	m_enWorkState	= IsWorkWaitTime();		break;
	/* 노광 완료 했는지 여부 확인 */
	case 0x0e	:	m_enWorkState	= IsExpoPrinted();		break;
	}

	/* 다음 작업 진행 여부 판단 */
	SetWorkNext();

	/* 장시간 동안 동일 위치를 반복 수행한다면 에러 처리 */
	if (IsWorkTimeOut())
	{
		if (m_syncPrint.Enter())
		{
			m_bRunPrint		= FALSE;
			m_enWorkState	= ENG_JWNS::en_error;

			m_syncPrint.Leave();
		}
	}
}

/*
 desc : 다음 단계로 이동하기 위한 처리
 parm : None
 retn : None
*/
VOID CMainThread::SetWorkNext()
{
	/* 작업이 에러이거나 종료되었을 때 */
	if (ENG_JWNS::en_error == m_enWorkState || ENG_JWNS::en_comp == m_enWorkState)
	{
		m_u8WorkStep	= 0x00;
		m_bRunPrint		= FALSE;
		return;
	}

	if (ENG_JWNS::en_next == m_enWorkState)
	{
		if (m_u8WorkTotal == m_u8WorkStep)
		{
			if (++m_u16PrintCount == m_u16PrintTotal)
			{
				m_u8WorkStep	= 0x00;
				m_enWorkState	= ENG_JWNS::en_comp;
				m_bRunPrint		= FALSE;
				SendMesgPrint(TRUE);
			}
			else
			{
				m_u8WorkStep	= 0x01;
				SendMesgPrint(FALSE);
			}
#if 0	/* ReqPeriodPkt() 함수에서 주기적으로 호출 */
			/* 현재 광학계 높이 값 요청 */
			uvEng_Luria_ReqGetMotorAbsPositionAll();
#endif
		}
		else
		{
			/* 다음 작업 단계로 이동 */
			m_u8WorkStep++;
		}
		/* 가장 최근에 Waiting 한 시간 저장 */
		m_u64DelayTime	= GetTickCount64();
	}
}

/*
 desc : 일정 시간 동안 작업을 대기하기 위함
 parm : time	- [in]  최소한의 대기에 필요한 시간 (단위: msec)
 retn : None
*/
VOID CMainThread::SetWorkWaitTime(UINT64 time)
{
	m_u64WaitTime	= GetTickCount64() + time;
}

/*
 desc : 일정 시간 동안 작업이 대기 완료 했는지 여부
 parm : title	- [in]  작업 (단계) 정보 출력 여부
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsWorkWaitTime()
{
	return GetTickCount64() > m_u64WaitTime ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : 장시간 동일한 위치에서 대기 중인지 여부
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainThread::IsWorkTimeOut()
{
#ifdef _DEBUG
	return	GetTickCount64() > (m_u64DelayTime + 60000);	/* 30 초 이상 지연되면 */
#else
	return	GetTickCount64() > (m_u64DelayTime + 10000);	/* 10 초 이상 지연되면 */
#endif
}

/*
 desc : 작업이 완료 되었는지 확인
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainThread::IsCompleted()
{
	return m_enWorkState == ENG_JWNS::en_comp;
}

/*
 desc : 현재 Photohead Z Axis이 원하는 위치에 도달했는지 여부
 parm : ph_no	- [in]  Photohead Number (1 ~ 8)
		pos		- [in]  최종 이동될 위치 값 (단위: mm)
 retn : TRUE or FALSE
*/
BOOL CMainThread::IsPhZAxisMoved(UINT8 ph_no, DOUBLE pos)
{
	UINT8 u8Drv		= UINT8(ENG_MMDI::en_axis_ph1);
	DOUBLE dbZAxis	= 0.0f;

	/* 최종 이동될 위치와 현재 이동된 위치의 오차가 +/- 0.001 mm 즉, 3 um 미만이면 성공 */
	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		dbZAxis	= uvCmn_Luria_GetPhZAxisPosition(ph_no);
	}
	else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		dbZAxis	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI(u8Drv+ph_no-1));
	}

	return (abs(pos - dbZAxis) < 0.003 /* less than 3 um */);
}

/*
 desc : 현재 작업 진행률 반환
 parm : None
 retn : 진행률 (단위: %; Percent)
*/
UINT16 CMainThread::GetStepCount()
{
	return  m_u8WorkStep + (m_u8WorkTotal * m_u16PrintCount);
}

/*
 desc : 노광 시작 위치 설정
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetExpoStartXY()
{
	BOOL bSucc	= FALSE;
	DOUBLE dbStartX, dbStartY;
#if 0
	TRACE(L"Repeat Count = %d\n", m_u16PrintCount);
#endif
	/* 단위: um (Luria Service에 등록해 줘야 되므로) */
	dbStartX	= m_dbExpoX;									/* X 축은 변함이 없이 고정 값 */
	dbStartY	= m_dbExpoY + (m_dbPeriodY * m_u16PrintCount);	/* Y 축은 노광 횟수에 따라 증/감 하는 변경 값 */
	/* 노광 시작 위치를 Luria (Photohead) Service에 설정 */
	bSucc		= uvEng_Luria_ReqSetTableExposureStartPos(0x01 /* fixed */,
														  (UINT32)ROUNDED(dbStartX * 1000.0f, 0),
														  (UINT32)ROUNDED(dbStartY * 1000.0f, 0), TRUE);
	/* 설정 응답에 따른 최소 대기 시간 */
	SetWorkWaitTime(1000);

	return bSucc ? ENG_JWNS::en_next : ENG_JWNS::en_error;
}

/*
 desc : 노광 시작 위치 설정 했는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsExpoStartXY()
{
	DOUBLE dbStartX, dbStartY;
	LPG_I4XY pstStart	= &uvEng_ShMem_GetLuria()->machine.table_expo_start_xy[0];

	/* 단위: um (Luria Service에 등록해 줘야 되므로) */
	dbStartX	= m_dbExpoX;									/* X 축은 변함이 없이 고정 값 */
	dbStartY	= m_dbExpoY + (m_dbPeriodY * m_u16PrintCount);	/* Y 축은 노광 횟수에 따라 증/감 하는 변경 값 */

	/* Luria에 노광 시작 위치 값이 갱신 되었는지 여부 확인 */
	if (!(pstStart->x == (INT32)ROUNDED(dbStartX * 1000.0f, 0) &&
		  pstStart->y == (INT32)ROUNDED(dbStartY * 1000.0f, 0)))	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : 모든 Photohead Z Axis Homing (Reset) (Move to Mid-Position)
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetPhZAxisHoming()
{
	BOOL bSucc	= FALSE;
	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* 모든 광학계 Mid-pos 값이 초기화 안되었다고 값 재설정 */
		uvCmn_Luria_ResetAllPhZAxisMidPosition();
		/* 모든 광학계 Mid-Pos 즉, Homing 작업 수행하라고 설정 */
		bSucc	= uvEng_Luria_ReqSetMotorPositionInitAll(0x01);
	}
	else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* 현재 Motion의 Toggle 값 저장 */
		uvCmn_MC2_GetDrvDoneToggledAll();
		/* Photohead 2대만 Homing 수행 */
		uvEng_MC2_SendDevHoming(ENG_MMDI::en_axis_ph1);
		uvEng_MC2_SendDevHoming(ENG_MMDI::en_axis_ph2);
	}
	/* 가장 최근 명령 전달 시간 초기화 */
	m_u64SendTime	= 0;
	m_u64StepTime	= GetTickCount64();	/* 해당 명령을 실행한 시간 저장 */

	return bSucc ? ENG_JWNS::en_next : ENG_JWNS::en_wait;
}

/*
 desc : 모든 Photohead Z Axis 중 1개 이상이 Homing 상태인지 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsPhZAxisHoming()
{
	/* !!! 중요 !!! 작업 대기 시간 값 갱신 */
	m_u64DelayTime	= GetTickCount64();

	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* Photohead Z Axis의 모터 상태 요청 */
		if (m_u64SendTime+500 < GetTickCount64())
		{
#if 0
			TRACE(L"Homing Check........\n");
#endif
			m_u64SendTime = GetTickCount64();
			if (!uvEng_Luria_ReqGetMotorStateAll())	return ENG_JWNS::en_error;
		}
		/* 약 10초 동안 Homing 작업을 수행하지 않으면, 다시 Homing 명령 수행 */
		if (m_u64StepTime+10000 < m_u64DelayTime)
		{
			m_u8WorkStep	-= 2;
			return ENG_JWNS::en_next;
		}
		/* 임의 Photohead의 Z Axis이 Homing 상태인지 여부 확인 */
		if (!uvCmn_Luria_IsAnyPhZAxisHoming())	return ENG_JWNS::en_wait;
	}
	else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* 약 10초 동안 Homing 작업을 수행하지 않으면, 다시 Homing 명령 수행 */
		if (m_u64StepTime+10000 < m_u64DelayTime)
		{
			m_u8WorkStep	-= 2;
			return ENG_JWNS::en_next;
		}
		/* Photohead 2대 Homing 작업 완료 했는지 여부 */
		if (!(uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_axis_ph1) &&
			  uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_axis_ph2)))	return ENG_JWNS::en_wait;
	}

	m_u64StepTime	= GetTickCount64();	/* 해당 명령을 실행한 시간 저장 */

	return ENG_JWNS::en_next;	/* Homing 상태인 경우 */
}

/*
 desc : 모든 Photohead Z Axis Homing (Reset) 되었는지 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsPhZAxisHomed()
{
	/* !!! 중요 !!! 작업 대기 시간 값 갱신 */
	m_u64DelayTime	= GetTickCount64();

	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* Photohead Z Axis의 모터 상태 요청 */
		if (m_u64SendTime+500 < GetTickCount64())
		{
			m_u64SendTime = GetTickCount64();
			if (!uvEng_Luria_ReqGetMotorStateAll())	return ENG_JWNS::en_error;
			return ENG_JWNS::en_wait;
		}
		/* 약 15초 동안 Homing 작업을 수행하지 않으면, 다시 Homing 명령 수행 */
		if (m_u64StepTime+15000 < m_u64DelayTime)
		{
			m_u8WorkStep	-= 3;
			return ENG_JWNS::en_next;
		}
		/* 현재 모든 Photohead의 Mid 상태가 완료 되었는지 확인 */
		if (!uvCmn_Luria_IsAllPhZAxisMidPosition())	return ENG_JWNS::en_wait;
		/* 현재 모든 Phothead의 Z Axis 상태가 Idle 상태인지 확인 */
		if (!uvCmn_Luria_IsAllPhZAxisIdle())		return ENG_JWNS::en_wait;
	}
	else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* 약 3초 동안 Homing 작업을 수행하지 않으면, 다시 Homing 명령 수행 */
		if (m_u64StepTime+3000 < m_u64DelayTime)
		{
			m_u8WorkStep	-= 3;
			return ENG_JWNS::en_next;
		}
		/* Photohead 2대 Homing 작업 완료 했는지 여부 */
		if (!(uvCmn_MC2_IsDriveHomed(ENG_MMDI::en_axis_ph1) &&
			  uvCmn_MC2_IsDriveHomed(ENG_MMDI::en_axis_ph2)))	return ENG_JWNS::en_wait;
	}
	return ENG_JWNS::en_next;
}

/*
 desc : 모든 Photohead Z Axis을 원하는 노광 위치로 이동
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetPhZAxisMoving()
{
	UINT8 i			= 0x00, u8Drv = UINT8(ENG_MMDI::en_axis_ph1);
	BOOL bSucc		= FALSE;
	DOUBLE dbZAxis	= 0, *pVelo = uvEng_GetConfig()->mc2_svc.max_velo;
	
	/* Photohead 개수 만큼 노광 위치로 이동 시킴 */
	for (; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		dbZAxis	= m_dbPhBaseZ[i] + (m_dbPeriodZ * m_u16PrintCount);
#if 0
		TRACE(L"1st. Set PH Z Axis Pos [%d] = %.3f mm\n", i+1, dbZAxis);
#endif
		/* 현재 Photohead의 Z 축 위치 이동 */
		if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
		{
			if (!uvEng_Luria_ReqSetMotorAbsPosition(i+1, dbZAxis))	return ENG_JWNS::en_error;
		}
		else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
		{
			uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI(u8Drv+i));
			if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI(u8Drv+i), dbZAxis, pVelo[u8Drv+i]))
				return ENG_JWNS::en_error;
		}
	}

	/* Luria에서 모든 Photohead가 노광 위치로 이동하는데 최소한의 대기 시간 */
	SetWorkWaitTime(1000);

	/* 가장 최근 명령 전달 시간 초기화 */
	m_u64SendTime	= 0;
	m_u64StepTime	= GetTickCount64();	/* 해당 명령을 실행한 시간 저장 */

	return ENG_JWNS::en_next;
}

/*
 desc : 모든 Photohead Z Axis Homing (Reset) 되었는지 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsPhZAxisMoved()
{
	UINT8 i, u8Moved= 0x00, u8Drv = UINT8(ENG_MMDI::en_axis_ph1);
	BOOL bIsMoved	= FALSE;
	DOUBLE dbZAxis	= 0;

	/* !!! 중요 !!! 작업 대기 시간 값 갱신 */
	m_u64DelayTime	= GetTickCount64();

	/* 약 10초 동안 원하는 위치로 이동하지 않았다면, 다시 이동 명령 수행 */
	if (m_u64StepTime+10000 < m_u64DelayTime)
	{
		m_u8WorkStep	-= 2;
		return ENG_JWNS::en_next;
	}
	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* 모든 Photohead의 Z Axis이 정상적으로 작업할 준비가 되어 있는지 확인 */
		if (!uvCmn_Luria_IsAllPhZAxisIdle())	return	ENG_JWNS::en_wait;
	}
	else if (0x03 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* Photohead 개수 만큼 노광 위치로 이동 시킴 */
		for (i=0x00; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
		{
			if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI(u8Drv+i)))	return ENG_JWNS::en_wait;
		}
	}

	/* 모든 Photohead의 Z Axis 위치 값이 원하는 위치에 도달 했는지 여부 */
	for (i=0x00; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		dbZAxis	= m_dbPhBaseZ[i] + (m_dbPeriodZ * m_u16PrintCount);
#if 0
		TRACE(L"Current PH Z Axis Pos [%d] = [set] %.3f mm  == [cur] %.3f mm\n", i+1,
			  dbZAxis, uvCmn_Luria_GetPhZAxisPosition(i+1));
#endif
		/* 최종 이동될 위치와 현재 이동된 위치의 오차가 +/- 0.001 mm 즉, 1 um 이내이면 성공 */
		if (IsPhZAxisMoved(i+1, dbZAxis))	u8Moved++;
	}

	/* 모두 이동이 완료 되었는지 여부 및 모든 광학계의 Z Axis의 상태가 Idle 상태인지 확인 */
	if (u8Moved == uvEng_GetConfig()->luria_svc.ph_count)	return ENG_JWNS::en_next;

	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* 현재 모든 Photohead의 Z Axis 위치 값 요청 */
		if (m_u64SendTime+500 < GetTickCount64())
		{
			/* Photohead 개수 만큼 노광 위치로 이동 시킴 */
			for (i=0x00; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
			{
				dbZAxis	= m_dbPhBaseZ[i] + (m_dbPeriodZ * m_u16PrintCount);
				TRACE(L"2nd. Set PH Z Axis Pos [%d] = %.3f mm\n", i+1, dbZAxis);
				/* 현재 위치와 최종 이동될 위치기 +/- 오차 범위에 있으면, 더 이상 이동하지 말라고 함 */
				if (!IsPhZAxisMoved(i+1, dbZAxis))
				{
					/* 현재 Photohead의 Z 축 위치 이동 */
					if (!uvEng_Luria_ReqSetMotorAbsPosition(i+1, dbZAxis))	return ENG_JWNS::en_error;
				}
			}

			m_u64SendTime = GetTickCount64();
			if (!uvEng_Luria_ReqGetMotorAbsPositionAll())	return ENG_JWNS::en_error;
		}
	}
	return ENG_JWNS::en_wait;
}

/*
 desc : Luria Service에게 노광할 준비가 되어 있다고 알림
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetExpoReady()
{
	/* 가장 최근 명령 전달 시간 초기화 */
	m_u64SendTime	= 0;
	m_u64StepTime	= GetTickCount64();	/* 해당 명령을 실행한 시간 저장 */

	if (!uvEng_Luria_ReqSetPrintOpt(0x00))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : 모든 Photohead Z Axis Homing (Reset) 되었는지 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsExpoReady()
{
	/* 약 5초 동안 원하는 위치로 이동하지 않았다면, 다시 이동 명령 수행 */
	if (m_u64StepTime+5000 < GetTickCount64())
	{
		m_u8WorkStep	-= 2;
		return ENG_JWNS::en_next;
	}

	/* 현재 노광이 준비된 상태인지 여부 확인 */
	if (uvCmn_Luria_IsExposeStateSuccess(ENG_LCEP::en_pre_print))	return ENG_JWNS::en_next;

	/* 현재 Luria Service의 노광 상태 (진행) 요청 */
	if (m_u64SendTime+500 < GetTickCount64())
	{
		m_u64SendTime = GetTickCount64();
		if (!uvEng_Luria_ReqGetExposureState())	return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_wait;
}

/*
 desc : Luria Service에게 노광 실행하라고 알림
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetExpoPrinting()
{
	/* 가장 최근 명령 전달 시간 초기화 */
	m_u64SendTime	= 0;

	if (!uvEng_Luria_ReqSetPrintOpt(0x01))	return ENG_JWNS::en_error;

	/* 노광 시작하기 위해 준비하는 대기 시간 값 설정 */
	SetWorkWaitTime(1000);

	return ENG_JWNS::en_next;
}

/*
 desc : 모든 Photohead Z Axis Homing (Reset) 되었는지 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsExpoPrinted()
{
	/* 현재 노광이 완료된 상태인지 여부 확인 */
	if (uvCmn_Luria_IsExposeStateSuccess(ENG_LCEP::en_print))	return ENG_JWNS::en_next;

	/* 현재 노광이 진행 중인 경우 */
	if (uvCmn_Luria_IsExposeStateRunning(ENG_LCEP::en_print))
	{
		/* !!! 중요 !!! 작업 대기 시간 값 갱신 */
		m_u64DelayTime	= GetTickCount64();
	}

	/* 현재 Luria Service의 노광 상태 (진행) 요청 */
	if (m_u64SendTime+500 < GetTickCount64())
	{
		m_u64SendTime = GetTickCount64();
		if (!uvEng_Luria_ReqGetExposureState())	return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_wait;
}

/*
 desc : 노광 작업이 없을 때, 주기적으로 요청 (주로 Luria Side)
 parm : None
 retn : None
*/
VOID CMainThread::ReqPeriodPkt()
{
	UINT64 u64Tick	= GetTickCount64();

	/* 일정 주기마다 호출하도록 하기 위함 (1초에 n번 횟수마다 송신하기 위함) */
	if (u64Tick < m_u64PeridPktTime + 1000/4)	return;

	/* 가장 최근에 호출한 시간 저장 */
	m_u64PeridPktTime	= u64Tick;
#if 0
	/* Motion Position 요청 */
	if (m_bReqPeriodMC2)	uvEng_MC2_ReadReqPktActAll();
	else					uvEng_MC2_ReadReqPktRefAll();
	m_bReqPeriodMC2	= !m_bReqPeriodMC2;
#endif
	if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
	{
		/* Luria State 요청 */
		uvEng_Luria_ReqGetMotorAbsPositionAll();
	}
}