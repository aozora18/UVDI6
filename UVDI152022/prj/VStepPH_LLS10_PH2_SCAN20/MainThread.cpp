
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
CMainThread::CMainThread(HWND parent, LPG_ACGR grab)
	: CThinThread()
{
	m_hParent		= parent;
	m_bRunMeasure	= FALSE;
	m_pstGrabData	= grab;
	m_u8GridCheck	= 0x00;
	m_u8GridCount	= 0x00;
	m_u64WorkTime	= GetTickCount64();
	m_u64ReqTime	= GetTickCount64();
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

	/* 측정 작업 진행 */
	if (m_bRunMeasure)
	{
		m_bRunMeasure = DoMeasure();
	}
	else
	{
		/* 거버 파일이 등록되어 있는지 여부를 확인하기 위해 주기적으로 요청 ? */
		if (GetTickCount64()+5000)
		{
			/* 가장 최근 요청한 시간 저장 */
			m_u64ReqTime = GetTickCount64();
			uvEng_Luria_ReqGetJobList();
		}
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
 desc : Grabbed Result 전달
 parm : result	- [in]  성공 여부
		timeout	- [in]  SendMessag Lifetime (unit: msec)
 retn : None
*/
VOID CMainThread::SendMesgResult(BOOL result, UINT32 timeout)
{
	DWORD_PTR dwResult	= 0;
	LRESULT lResult		= 0;
	WPARAM wParam		= result ? MSG_ID_RESULT_SUCC : MSG_ID_RESULT_FAIL;

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
 desc : 측정 작업 수행
 parm : run	- [in]  TRUE or FALSE
 retn : None
*/
VOID CMainThread::RunMeasure(BOOL run)
{
	if (m_syncRun.Enter())
	{
		m_u8Step		= 0x01;
		m_bRunMeasure	= run;
		m_u64StepTime	= GetTickCount64();

		m_syncRun.Leave();
	}
}

/*
 desc : 작업 대기 시간 설정
 parm : wait	- [in]  대기 시간 (단위: msec)
 retn : None
*/
VOID CMainThread::SetWaitTime(UINT64 wait)
{
	m_u64WorkTime	= (UINT32)GetTickCount();
	m_u64WaitTime	= wait;
}

/*
 desc : 측정
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainThread::DoMeasure()
{
	BOOL bSucc	= FALSE;

	/* 일정 시간 대기 */
	if (!(GetTickCount64() > m_u64StepTime + 100))	return TRUE;
	m_u64StepTime	= GetTickCount64();

	switch (m_u8Step)
	{
	case 0x01	: bSucc = PutOneTrigger();	break;
	case 0x02	: bSucc = GrabbedImage();	break;
	case 0x03	: bSucc = MotionMoving();	break;
	case 0x04	: bSucc = IsMotionMoved();	break;
	default		: return FALSE;
	}

	if (bSucc)
	{
		if (m_u8Step == 0x04)
		{
			if (!m_bResultSucc)	m_u8Step	= 0x00;
			else				SendMesgResult(TRUE);	/* 부모에게 알림 */
		}

		m_u8Step++;
	}
	else
	{
		/* 일정 시간 동안 응답이 없는 경우, 에러 처리 */
		bSucc	= TRUE;
		if ((UINT32)GetTickCount() > (m_u64WorkTime+m_u64WaitTime))
		{
			bSucc	= FALSE;
			SendMesgResult(FALSE);
		}
		uvCmn_uSleep(500000);	/* 0.5 초 정도 대기 */
	}

	return bSucc;
}

/*
 desc : 트리거 1개 발생 시킴
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainThread::PutOneTrigger()
{
	UINT8 u8ChNo= 0x01;

	/* 기존 Live & Edge & Calibration 데이터 초기화 */
	uvEng_Camera_ResetGrabbedImage();
	if (!uvEng_Trig_ReqTrigOutOne(0x01, FALSE))
	{
		AfxMessageBox(L"Failed to send the event for trigger", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* Grabbed Image가 처리되는 시간 대기 */
	SetWaitTime(5000);

	return TRUE;
}

/*
 desc : Grabbed Image 처리
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainThread::GrabbedImage()
{
	BOOL bIsChanged		= FALSE;
	DOUBLE dbCentOffset = uvEng_GetConfig()->ph_step.center_offset;

	/* 작업 완료 하지 못함으로 설정 */
	m_bResultSucc	= FALSE;
	/* Grabbed Image가 존재하는지 확인 */
	m_pstGrabData[0].marked	= 0x00;
	m_pstGrabData[1].marked	= 0x00;
	if (!uvEng_Camera_RunModelStep(0x01, MODEL_FIND_COUNT, FALSE, m_pstGrabData))
	{
		return FALSE;
	}

	/* 얼라인 카메라가 회전되어 있다면 ... (180도 이상) */
	if (uvEng_GetConfig()->set_cams.acam_inst_angle)
	{
		bIsChanged	= m_pstGrabData[0].mark_cent_mm_x < m_pstGrabData[1].mark_cent_mm_x;
	}
	else
	{
		bIsChanged	= m_pstGrabData[0].mark_cent_mm_x > m_pstGrabData[1].mark_cent_mm_x;
	}
	/* X 좌표 값이 작은 순으로 재배치 */
	if (bIsChanged)
	{
		STG_ACGR stTemp	= {NULL};
		memcpy(&stTemp,				&m_pstGrabData[0],	sizeof(STG_ACGR));
		memcpy(&m_pstGrabData[0],	&m_pstGrabData[1],	sizeof(STG_ACGR));
		memcpy(&m_pstGrabData[1],	&stTemp,			sizeof(STG_ACGR));
	}
	/* 정상 검출 안됐다면 ... */
	if (!(m_pstGrabData[0].marked && m_pstGrabData[1].marked))
	{
		AfxMessageBox(L"Failed to find the mark info", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 현재 Grabbed Image 2개의 중심 간의 떨어진 평균을 구하고, 1번째 카메라의 최종 이동 거리 구함 */
	m_dbCentMove = m_pstGrabData[0].mark_cent_mm_dist - m_pstGrabData[1].mark_cent_mm_dist;
#if 1
	TRACE(L"m_dbCentMove = %.4f, dbCentOffset = %.4f mm\n", m_dbCentMove, dbCentOffset);
#endif
	if (abs(m_dbCentMove) < dbCentOffset)	/* 1.0 mm 이내에 있는지 여부 */
	{
		/* 작업 성공 */
		m_bResultSucc	= TRUE;
	}

	return TRUE;
}

/*
 desc : 이미지가 중심에 오도록 모션 이동
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainThread::MotionMoving()
{
	DOUBLE dbMove	= 0.0f, dbVelo;
#if 0
	/* 현재 위치 */
	TRACE(L"[start] acam_1 =  %.4f\n", uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam1));
#endif
	/* 모션 위치 정밀 조정 */
	if (!m_bResultSucc)
	{
		/* 현재 선택된 얼라인 카메라를 이동하기 위한 값 설정 */
		dbMove	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_align_cam1) - m_dbCentMove/2.0f;
		/* 현재 얼라인 카메라의 Toggle 값 저장 */
		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
		/* 현재 선택된 얼라인 카메라의 이동 속도 값 얻기 */
		dbVelo	= uvEng_GetConfig()->mc2_svc.step_velo;

		/* 카메라를 정해진 위치로 이동 */
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_align_cam1, dbMove, dbVelo))
		{
			AfxMessageBox(L"Failed to move the motion drive of alignment camera", MB_ICONSTOP|MB_TOPMOST);
			return FALSE;
		}
	}
	/* 다음 측정 위치로 이동 */
	else
	{
		/* 자동으로 다음 위치로 이동. Set the grid 체크 버튼이 체크된 경우만 해당됨. */
		if (m_u8GridCheck)
		{
			dbMove	= uvEng_GetConfig()->ph_step.stripe_width;
#if 0
			if (++m_u8GridCount == 0x06)
			{
				dbMove	+= uvEng_GetConfig()->ph_step.stripe_width / 3;
			}
#else
			m_u8GridCount++;
#endif
			/* 현재 얼라인 카메라의 Toggle 값 저장 */
			uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_align_cam1);
			/* 다음 측정 위치로 이동 */
			uvEng_MC2_SendDevRefMove(ENG_MMMM::en_move_step, ENG_MMDI::en_align_cam1,
									 ENG_MDMD::en_move_right, dbMove);
		}
	}

	/* 모션 도착 예정 대기 시간 */
	SetWaitTime(5000);

	return TRUE;
}

/*
 desc : 정상적으로 이동이 완료 되었는지 확인
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainThread::IsMotionMoved()
{
	if (!m_u8GridCheck)	return TRUE;
	/* 정상적으로 이동이 완료 되었다면 루프 빠져 나감 */
	return (uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_align_cam1));
}
