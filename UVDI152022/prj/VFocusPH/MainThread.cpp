
/*
 desc : Main Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "MainThread.h"
#include "./Meas/Measure.h"


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
	m_bRunFocus			= FALSE;
	m_u64PeridPktTime	= 0;
	/* 측정된 엣지 정보를 분석 및 저장 관리하는 자식 다이얼로그 객체 */
	m_pMeasure			= new CMeasure();
	ASSERT(m_pMeasure);
}

/*
 desc : 파괴자
 parm : None
*/
CMainThread::~CMainThread()
{
	/* 기존 측정된 데이터 메모리 해제 */
	if (m_pMeasure)	delete m_pMeasure;
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
	if (m_syncFocus.Enter())
	{
		/* 포커스 작업 수행 */
		if (m_bRunFocus)	RunFocus();
		else				ReqPeriodPkt();

		/* 동기 해제 */
		m_syncFocus.Leave();
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
 desc : 부모에게 메시지 전달 (SendMessage) - Step Result 결과 값
 parm : result	- [in]  결과 코드 값
		timeout	- [in]  SendMessag Lifetime (unit: msec)
 retn : None
*/
VOID CMainThread::SendMesgResult(WPARAM result, UINT32 timeout)
{
	DWORD_PTR dwResult	= 0;
	LRESULT lResult		= 0;

	/* 부모에게 이벤트 메시지 전달 */
	lResult	= ::SendMessageTimeout(m_hParent, WM_MAIN_THREAD, result, 0L,
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
 desc : 포커스 측정 관련 매개 변수 설정 후 동작 진행
 parm : step_s	- [in]  Y 축 이동 횟수
		repeat	- [in]  측정 구간마다 반복 측정되는 횟수
		step_y	- [in]  Y 축 모션 이동 간격 (UP) (단위: mm)
 retn : None
*/
VOID CMainThread::StartFocus(UINT16 step_s, UINT16 repeat, DOUBLE step_y)
{
	/* 동기 진입 */
	if (m_syncFocus.Enter())
	{
		m_u16RetryGrab	= 0;

		m_u8PhNext		= 0x01;
		m_u16StepYNext	= 0x0001;
		m_u16RepeatNext	= 0x0001;

		m_u16StepYTotal	= step_s;
		m_u16RepeatTotal= repeat;

		m_u8WorkStep	= 0x01;
		m_u8WorkTotal	= 0x05;

		m_bRunFocus		= TRUE;

		/* 동기 해제 */
		m_syncFocus.Leave();
	}

	/* 기존 측정된 데이터 메모리 해제 */
	m_pMeasure->ResetResult();
}

/*
 desc : 작업 강제로 중지
 parm : None
 retn : None
*/
VOID CMainThread::StopFocus()
{
	/* 동기 진입 */
	if (m_syncFocus.Enter())
	{
		m_bRunFocus	= FALSE;
		/* 동기 해제 */
		m_syncFocus.Leave();
	}
}

/*
 desc : 포커스 작업 수행
 parm : None
 retn : None
*/
VOID CMainThread::RunFocus()
{
	switch (m_u8WorkStep)
	{
	/* 측정 위치로 이동 */
	case 0x01	:	m_enWorkState = SetMeasureMoving();		break;
	case 0x02	:	m_enWorkState = IsMeasureMoved();		break;
	/* Trigger 1개 발생 */
	case 0x03	:	m_enWorkState = PutOneTrigger();		break;
	/* Grabbed Image 수집 및 Edge Detect 수행 */
	case 0x04	:	m_enWorkState = ProcFindModel();		break;
	/* 결과 데이터 수집 후 분석 진행 */
	case 0x05	:	m_enWorkState = ResultDetectImage();	break;
	}

	/* 다음 작업 진행 여부 판단 */
	SetWorkNext();
	/* 장시간 동안 동일 위치를 반복 수행한다면 에러 처리 */
	if (IsWorkTimeOut())
	{
		if (m_syncFocus.Enter())
		{
			m_bRunFocus		= FALSE;
			m_enWorkState	= ENG_JWNS::en_error;
			/* 동기 해제 */
			m_syncFocus.Leave();
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
	if (ENG_JWNS::en_error == m_enWorkState)
	{
		m_u8WorkStep	= 0x00;
		m_bRunFocus		= FALSE;
		return;
	}

	if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* 모든 작업이 끝나지 않은 경우 */
		if (m_u8WorkTotal != m_u8WorkStep)
		{
			m_u8WorkStep++;	/* 다음 작업 단계로 이동 */
		}
		/* 모든 작업이 완료된 경우 */
		else
		{
#if 0	/* ReqPeriodPkt 함수에서 주기적으로 호출함 */
			if (0x01 == uvEng_GetConfig()->luria_svc.z_drive_type)
			{
				/* 현재 광학계 높이 값 요청 */
				uvEng_Luria_ReqGetMotorAbsPositionAll();
			}
#endif
			/* 현재 Step 구간에서 반복 측정을 모두 하였는지 여부 */
			if (m_u16RepeatNext != m_u16RepeatTotal)
			{
				m_u16RepeatNext++;
				m_u8WorkStep	= 0x03;	/* Trigger 발생 위치로 이동 */
			}
			/* 현재 광학계의 Y 축 스텝 (UP) 이동이 모두 완료 되었는지 여부 */
			else if (m_u16StepYNext != m_u16StepYTotal)
			{
				m_u16StepYNext++;
				m_u16RepeatNext	= 0x01;
				m_u8WorkStep	= 0x01;
			}
			/* 광학계 (Photohead) 개수만큼 측정이 다 되었는지 여부 */
			else if (m_u8PhNext != uvEng_GetConfig()->luria_svc.ph_count)
			{
				m_u8PhNext++;
				m_u16RepeatNext	= 0x01;
				m_u16StepYNext	= 0x01;
				m_u8WorkStep	= 0x01;
			}
			else
			{
				m_enWorkState	= ENG_JWNS::en_comp;
				m_u8WorkStep	= 0x00;
				m_bRunFocus		= FALSE;
			}
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
	return	GetTickCount64() > (m_u64DelayTime + 60000);	/* 60 초 이상 지연되면 */
#else
	return	GetTickCount64() > (m_u64DelayTime + 20000);	/* 20 초 이상 지연되면 */
#endif
}

/*
 desc : Focus 작업이 완료 되었는지 확인
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainThread::IsCompleted()
{
	/* 어째든 멈춘거니까... 완료된 것으로 판단 */
	if (!m_bRunFocus && (m_u8WorkTotal == m_u8WorkStep) &&
		(m_u8PhNext == uvEng_GetConfig()->luria_svc.ph_count))	return TRUE;

	return FALSE;
}

/*
 desc : 현재까지 측정된 데이터 반환
 parm : ph_no	- [in]  Photohead Number (1 ~ 8)
		index	- [in]  가져오고자 하는 메모리 위치 (Zero-based)
 retn : index 위치에 저장된 측정 값
*/
LPG_ZAAL CMainThread::GetGrabData(UINT8 ph_no, UINT32 index)
{
	LPG_ZAAL pstData	= NULL;

	/* 동기 진입 */
	if (m_syncFocus.Enter())
	{
		pstData	= m_pMeasure->GetResult(ph_no, index);

		/* 동기 해제 */
		m_syncFocus.Leave();
	}

	return pstData;
}

/*
 desc : 가장 최근에 측정된 데이터 반환
 parm : None
 retn : 결과 데이터 반환
*/
LPG_ZAAL CMainThread::GetLastGrabData()
{
	LPG_ZAAL pstData	= NULL;

	/* 동기 진입 */
	if (m_syncFocus.Enter())
	{
		pstData	= m_pMeasure->GetLastResult();

		/* 동기 해제 */
		m_syncFocus.Leave();
	}

	return pstData;
}

/*
 desc : 현재 적재된 Edge Image의 CImage 객체 포인터 반환
 parm : None
 retn : CImage 객체 포인터
*/
CMeasure *CMainThread::GetMeasure()
{
	return m_pMeasure;
}

/*
 desc : 현재까지 작업 진행율 반환
 parm : None
 retn : 작업 진행율 반환 (단위: percent)
*/
DOUBLE CMainThread::GetWorkStepRate()
{
#if 1
	DOUBLE dbBase	= (m_u16RepeatTotal + m_u8WorkTotal * 1.0f) * m_u16StepYTotal;
	
	DOUBLE dbTotal	= dbBase * uvEng_GetConfig()->luria_svc.ph_count;
	DOUBLE dbCurrent= (dbBase * (m_u8PhNext - 1)) +
					  (m_u16RepeatTotal + m_u8WorkTotal * 1.0f) * (m_u16StepYNext - 1) +
					  (m_u16RepeatNext + m_u8WorkStep * 1.0f);
#else
	DOUBLE dbTotal	= (m_u16RepeatTotal + m_u8WorkTotal * 1.0f) * m_u16StepYTotal;
	DOUBLE dbCurrent= (m_u16RepeatTotal + m_u8WorkTotal * 1.0f) * (m_u16StepYNext - 1) +
					  (m_u16RepeatNext + m_u8WorkStep * 1.0f);
#endif
	return dbCurrent / dbTotal * 100.0f;
}

/*
 desc : 마크 측정 위치로 이동
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetMeasureMoving()
{
	DOUBLE dbStageX, dbStageY, dbACamX;
	DOUBLE dbVelo		= uvEng_GetConfig()->mc2_svc.move_velo;
	LPG_CPFI pstPhFocus	= &uvEng_GetConfig()->ph_focus;

	/* 기존 Motion X/Y, Align Camera X 1 번 등 Toggle 값 임시 저장 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
	/* 얼라인 카메라 1번 마크 측정 위치로 이동 */
	dbACamX		= pstPhFocus->mark2_acam_x[0];
	/* 현재 측정 기준으로 값으로 모션 X / Y 위치 재계산 */
	dbStageX	= pstPhFocus->mark2_stage[0] +
				  ((m_u8PhNext - 1) * pstPhFocus->mark_period);
	dbStageY	= pstPhFocus->mark2_stage[1] +
				  ((m_u16StepYNext - 1) * pstPhFocus->step_move_y);

	/* 모션 이동 수행 */
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x,	dbStageX,	dbVelo);
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y,	dbStageY,	dbVelo);
	uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1,dbACamX,	dbVelo);

	return ENG_JWNS::en_next;
}

/*
 desc : 마크 측정 위치로 이동했는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsMeasureMoved()
{
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_x))		return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))		return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1))	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : 트리거 1개 발생 시킴
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::PutOneTrigger()
{
	/* 현재 트리거 명령 버퍼에 임의 개수 이하가 대기 중인지 여부 */
	/* Trigger 송신 버퍼에 과도하게 많은 명령어가 저장되는 것 방지 */
	if (!uvEng_Trig_IsSendCmdCountUnder(3))	return ENG_JWNS::en_wait;

	/* Align Camera is Edge Detection Mode */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);

	/* 기존 Live & Edge & Calibration 데이터 초기화 */
	uvEng_Camera_ResetGrabbedImage();

	/* Trigger 강제로 (수동으로) 발생 */
	if (!uvEng_Trig_ReqTrigOutOne(0x01/*fixed*/, TRUE))
	{
		AfxMessageBox(L"Failed to send the event for trigger", MB_ICONSTOP|MB_TOPMOST);
//		return ENG_JWNS::en_error;
		/* 현재 시간으로 갱신 (Timeout 발생되지 않도록 하기 위함) */
		m_u64DelayTime	= GetTickCount64();
		/* 다시 트리거 발생시키기 위함 */
		m_u8WorkStep	-= 1;
		return ENG_JWNS::en_next;
	}

	/* 이미지 Grabbed 확인 시간 갱신 */
	m_u64GrabTime	= GetTickCount64();

	return ENG_JWNS::en_next;
}

/*
 desc : 현재 Grabbed Image 즉, Model Find된 이미지가 있는지 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::ProcFindModel()
{
	/* Grabbed Image가 있으며, 매칭된 결과가 있는지 여부 */
	if (!uvEng_Camera_RunModelFind(0x01))
	{
		/* 3 초 동안 Grabbed Image가 없는 경우, 다시 트리거 발생 시키기 위해 뒤로 이동 */
		if (!(m_u64GrabTime+1000 < GetTickCount64()))	return ENG_JWNS::en_wait;
		/* 측정 하지 못한 횟수 증가 처리 */
		m_u16RetryGrab++;
		/* 다시 이미지 Grbbaed 작업 수행하라고 지시 */
		m_u8WorkStep	-= 2;
		/* 일정 횟수 이상 인식 못했으면 에러 처리 */
		if (m_u16RetryGrab > 10)
		{
			SendMesgResult(MSG_ID_STEP_FIND_FAIL);
			return ENG_JWNS::en_error;
		}
		return ENG_JWNS::en_next;
	}

	/* Grabbed Image가 있지만, Edge 결과 후 검색 실패인지 여부 */
	if (!uvEng_Camera_GetLastGrabbedMark())
	{
		SendMesgResult(MSG_ID_STEP_GRAB_FAIL);
		return ENG_JWNS::en_error;
	}

	/* 정상적으로 처리 했으면 초기화 */
	m_u16RetryGrab	= 0x00;

	return ENG_JWNS::en_next;
}

/*
 desc : 수집 및 Edge 인식된 결과 정보 분석
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::ResultDetectImage()
{
	/* 엣지 검색 결과 처리 */
	if (!m_pMeasure->SetResult(0x01, m_u8PhNext, m_u16StepYNext))	return ENG_JWNS::en_error;
	/* 상위 부모에게 알림 */
	SendMesgResult(MSG_ID_STEP_RESULT);
#if 0
	TRACE(L"PH [%d], Step Y [%d], Count [%d]\n", m_u8PhNext, m_u16StepYNext, m_pMeasure->GetCount(m_u8PhNext));
#endif
	return ENG_JWNS::en_next;
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