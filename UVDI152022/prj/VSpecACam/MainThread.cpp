
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
	m_u64TickGrabbed	= 0;
	m_u8WorkStep		= 0x00;
	m_bRunSpec			= FALSE;
	m_enWorkState		= ENG_JWNS::en_none;
	m_u64PeridPktTime	= 0;
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
	if (m_syncSpec.Enter())
	{
		/* 포커스 작업 수행 */
		if (m_bRunSpec)	RunSpec();
		else			ReqPeriodPkt();

		/* 동기 해제 */
		m_syncSpec.Leave();
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
 parm : flag	- [in]  TRUE : 모든 작업 완료, FALSE : 1 회 측정 작업 완료
		timeout	- [in]  SendMessag Lifetime (unit: msec)
 retn : None
*/
VOID CMainThread::SendMesgResult(BOOL flag, UINT32 timeout)
{
	WPARAM wParam		= flag ? MSG_ID_CALI_COMPLETE : MSG_ID_CALI_RESULT;
	DWORD_PTR dwResult	= 0;
	LRESULT lResult		= 0;

	/* 부모에게 이벤트 메시지 전달 */
	lResult	= ::SendMessageTimeout(m_hParent, WM_MAIN_THREAD, wParam, 0L,
								   SMTO_NORMAL, timeout, &dwResult);
	if (0 == lResult)
	{
#if 0
		/* 현재 발생된 에러 값 저장 */
		TRACE("MainThread : SendMessage Time out <MesgResult> = %d \n", GetLastError());
#endif
	}
}

/*
 desc : 부모에게 메시지 전달 (SendMessage) - Step Result 결과 값
 parm : flag	- [in]  TRUE : 모든 작업 완료, FALSE : 1 회 측정 작업 완료
		timeout	- [in]  SendMessag Lifetime (unit: msec)
 retn : None
*/
VOID CMainThread::SendMesgGrabView(UINT32 timeout)
{
	DWORD_PTR dwResult	= 0;
	LRESULT lResult		= 0;

	/* 부모에게 이벤트 메시지 전달 */
	lResult	= ::SendMessageTimeout(m_hParent, WM_MAIN_THREAD, MSG_ID_GRAB_VIEW, 0L,
								   SMTO_NORMAL, timeout, &dwResult);
	if (0 == lResult)
	{
#if 0
		/* 현재 발생된 에러 값 저장 */
		TRACE("MainThread : SendMessage Time out <GrabView> = %d \n", GetLastError());
#endif
	}
}


/*
 desc : Camera Specification 측정 관련 매개 변수 설정 후 동작 진행
 parm : run_mode- [in]  측정 모드 값 (0x00 : 카메라 회전 각도 값 측정, 0x01 : 카메라 픽셀 크기 값 구하기)
		cam_id	- [in]  카메라 인덱스 1 or Later
		repeat	- [in]  총 반복 측정 횟수
 retn : None
*/
VOID CMainThread::StartSpec(UINT8 run_mode, UINT8 cam_id, UINT32 repeat)
{
	/* 동기 진입 */
	if (m_syncSpec.Enter())
	{
		m_u8RunMode		= run_mode == 0x00 ? 0xfe /*각도 측정 수행*/: 0xff /*각도 측정하지 않음*/;
		m_u8ACamID		= cam_id;
		m_u32RepeatTotal= repeat;
		m_u32RepeatCount= 0;
		m_dbRateLast	= 0.0f;
		m_u8WorkTotal	= 0x10;
		m_u8WorkStep	= 0x01;
		/* 무조건 Quartz가 Vision Camera의 중심 위치로 오도록 처리 */
		m_u8GrabbedCount= 0x00;	/* 각도 측정을 위한 이미지 (Quartz Mark) 측정 횟수 증가 */
		m_u64DelayTime	= GetTickCount64();
		/* 동작 시작 */
		m_bRunSpec		= TRUE;

		/* 동기 해제 */
		m_syncSpec.Leave();
	}
}

/*
 desc : 작업 강제로 중지
 parm : None
 retn : None
*/
VOID CMainThread::StopSpec()
{
	/* 동기 진입 */
	if (m_syncSpec.Enter())
	{
		m_bRunSpec	= FALSE;
		/* 동기 해제 */
		m_syncSpec.Leave();
	}
}

/*
 desc : Specification 측정 작업 수행
 parm : None
 retn : None
*/
VOID CMainThread::RunSpec()
{
	/* 작업 단계 별로 동작 처리 */
	switch (m_u8WorkStep)
	{
	/* 나머지 Align Camera를 한쪽으로 이동하기 */
	case 0x01	:	m_enWorkState	= SetACamMovingSide();			break;
	case 0x02	:	m_enWorkState	= IsACamMovedSide();			break;
	/* Quartz에 맞게 Z Axis Focusing */
	case 0x03	:	m_enWorkState	= SetACamMovingZAxisQuartz();	break;
	case 0x04	:	m_enWorkState	= IsACamMovedZAxisQuartz();		break;
	/* Quartz가 있는 곳으로 Align Camera 이동하기 */
	case 0x05	:	m_enWorkState	= SetACamMovingQuartz();		break;
	case 0x06	:	m_enWorkState	= IsACamMovedQuartz();			break;
	/* Trigger Enabled */
	case 0x07	:	m_enWorkState	= SetTrigEnabled();				break;
	case 0x08	:	m_enWorkState	= IsTrigEnabled();				break;
	/* Align Camera를 통해 이미지 Grabbed & 오차 값 추출 */
	case 0x09	:	m_enWorkState	= GetGrabQuartzData();			break;
	case 0x0a	:	m_enWorkState	= IsGrabbQuartzData();			break;
	/* Quartz를 Align Camera의 중심으로 이동하도록 조정 */
	case 0x0b	:	m_enWorkState	= SetMotionMoving();			break;
	case 0x0c	:	m_enWorkState	= IsMotionMoved();				break;
	/* Quartz의 Mark가 Vision Camera의 왼쪽(or 오른쪽)에 오도록 Stage X 이동 처리 */
	case 0x0d	:	m_enWorkState	= SetMotionMovingX();			break;
	case 0x0e	:	m_enWorkState	= IsMotionMovedX();				break;
	/* Quartz의 Mark Image를 Grabbed */
	case 0x0f	:	m_enWorkState	= GetGrabQuartzData();			break;
	case 0x10	:	m_enWorkState	= IsGrabbQuartzMoveX();			break;
	}

	/* 다음 작업 진행 여부 판단 */
	SetWorkNext();

	/* 장시간 동안 동일 위치를 반복 수행한다면 에러 처리 */
	if (IsWorkTimeOut())
	{
		if (m_syncSpec.Enter())
		{
			m_bRunSpec		= FALSE;
			m_enWorkState	= ENG_JWNS::en_error;

			m_syncSpec.Leave();
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
		m_bRunSpec		= FALSE;
		return;
	}

	if (ENG_JWNS::en_next == m_enWorkState)
	{
#if 1
		if (m_u8WorkStep > 0x0a/* && m_u8WorkStep < 0x10*/)	SendMesgGrabView();
#endif
		if (m_u8WorkTotal == m_u8WorkStep)
		{
#if 0	/* 다른 함수에서 사용됨 (ReqPeriodPkt 함수에서 호출됨) */
			/* 현재 광학계 높이 값 요청 */
			uvEng_Luria_ReqGetMotorAbsPositionAll();
#endif
			if (++m_u32RepeatCount == m_u32RepeatTotal)
			{
				m_u8WorkStep	= 0x00;
				m_enWorkState	= ENG_JWNS::en_comp;
				m_bRunSpec		= FALSE;
				CalcACamSpec(TRUE);		/* 현재 측정된 Align Camera의 Angle과 Pixel Size 구하기 */
			}
			else
			{
				m_u8GrabbedCount= 0x00;
				m_u8WorkStep	= 0x05;	/* QuartZ 중심 위치로 이동 */
				m_dbRateLast	= 0.0f;
				CalcACamSpec(FALSE);	/* 현재 측정된 Align Camera의 Angle과 Pixel Size 구하기 */
			}
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
 desc : 현재 이동하고자 하는 카메라가 아닌 경우, 맨 왼쪽 or 오른쪽 끝으로 이동 처리
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetACamMovingSide()
{
	DOUBLE dbMovePos	= 0.0f, dbMoveVelo = 0.0f;
	ENG_MMDI enACamDrv	= ENG_MMDI::en_align_cam1;
	LPG_CMSI pstCfgMC2	= &uvEng_GetConfig()->mc2_svc;
	LPG_MDSM pstMemMC2	= uvEng_ShMem_GetMC2();

	/* 외장형 카메라 타입인 경우 */
	if (ENG_VCPK::en_camera_basler_ipv4 == (ENG_VCPK)uvEng_GetConfig()->set_comn.align_camera_kind)
	{
		/* 측정 대상이 아닌 Align Camera Drive ID가 이동하고자 하는 위치 얻기 */
		if (m_u8ACamID == 0x01)	enACamDrv = ENG_MMDI::en_align_cam2;
		/* Align Camera 마다 사이드로 이동하고자 하는 위치 값이 다름 */
		if (enACamDrv == ENG_MMDI::en_align_cam2)
		{
			dbMovePos	= pstCfgMC2->max_dist[(UINT8)enACamDrv];
		}
		/* Align Camera 1 */
		else
		{
			dbMovePos	= pstCfgMC2->min_dist[(UINT8)enACamDrv];
		}
		/* Speed */
		dbMoveVelo= pstCfgMC2->max_velo[(UINT8)enACamDrv];

		/* 현재 이동하고자 하는 모션의 Toggled 값 얻기 */
		uvCmn_MC2_GetDrvDoneToggled(enACamDrv);

		/* 맨 우측 혹은 맨 좌측으로 측정 대상이 아닌 카메라 이동 시키기 */
		if (!uvEng_MC2_SendDevAbsMove(enACamDrv, dbMovePos, dbMoveVelo))
		{
			return ENG_JWNS::en_error;
		}
	}
	/* 내장형 카메라일 경우 */
	else
	{
		/* 아무런 작업을 수행하지 않음 */
		return ENG_JWNS::en_next;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : 맨 끝으로 이동했는지 여부 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsACamMovedSide()
{
	ENG_MMDI enACamDrv	= m_u8ACamID == 0x01 ? ENG_MMDI::en_align_cam2 : ENG_MMDI::en_align_cam1;
	/* 모터가 멈추었는지 확인 */
	if (!uvCmn_MC2_IsDrvDoneToggled(enACamDrv))	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : Align Camera를 Quartz가 잘 보이도록 Focusing을 위해 Z AXis 위치 이동
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetACamMovingZAxisQuartz()
{
	/* Align Camera Z Axis 이동 */
	uvEng_MCQ_SetACamMovePosZ(m_u8ACamID, 0x00, uvEng_GetConfig()->acam_spec.acam_z_focus[m_u8ACamID-1]);
	return ENG_JWNS::en_next;
}

/*
 desc : Quartz 의 Z Axis이 모두 이동 했는지 여부 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsACamMovedZAxisQuartz()
{
	DOUBLE dbACamZPos	= 0.0f, dbACamZSet = 0.0f;

	/* 이동하고자 하는 카메라의 Z 축 Focus 위치 */
	dbACamZSet	= uvEng_GetConfig()->acam_spec.acam_z_focus[m_u8ACamID-1];
	/* 현재 얼라인 카메라의 위치 값 얻기 */
	dbACamZPos	= uvCmn_MCQ_GetACamCurrentPosZ(m_u8ACamID);
	/* 둘다 소숫점 이해 4자리까지는 유효하게 만듬 */
	dbACamZSet	= ROUNDED(dbACamZSet, 4);
	dbACamZPos	= ROUNDED(dbACamZPos, 4);

	/* 이동하고자 하는 위치와 이동한 위치의 오차 값이 um 범위 내에 동일한지 여부 */
	if (dbACamZSet != dbACamZPos)	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : Align Camera를 Quartz가 있는 위치로 이동
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetACamMovingQuartz()
{
	ENG_MMDI enACamDrv	= m_u8ACamID == 0x01 ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;
	DOUBLE dbMoveACamPos= 0, dbMoveXPos = 0, dbMoveYPos = 0;
	DOUBLE dbSpeedACam	= 0, dbMoveXSpeed = 0, dbMoveYSpeed;

	/* Quartz Position */
	dbMoveXPos		= uvEng_GetConfig()->acam_spec.quartz_stage_x;
	dbMoveYPos		= uvEng_GetConfig()->acam_spec.quartz_stage_y[m_u8ACamID-1];
	dbMoveACamPos	= uvEng_GetConfig()->acam_spec.quartz_acam[m_u8ACamID-1];

	/* Speed */
	dbSpeedACam		= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)enACamDrv];
	dbMoveXSpeed	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_x];
	dbMoveYSpeed	= uvEng_GetConfig()->mc2_svc.max_velo[(UINT8)ENG_MMDI::en_stage_y];

	/* 현재 이동하고자 하는 모션의 Toggled 값 얻기 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
	uvCmn_MC2_GetDrvDoneToggled(enACamDrv);

	/* Motion 이동 시키기 */
	if (!uvEng_MC2_SendDevAbsMove(enACamDrv,			dbMoveACamPos,	dbSpeedACam))	return ENG_JWNS::en_error;
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x,	dbMoveXPos,		dbMoveXSpeed))	return ENG_JWNS::en_error;
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y,	dbMoveYPos,		dbMoveYSpeed))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : Quartz 위치로 모두 이동이 완료 되었는지 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsACamMovedQuartz()
{
	ENG_MMDI enACamDrv	= m_u8ACamID == 0x01 ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;
	if (!uvCmn_MC2_IsDrvDoneToggled(enACamDrv))			return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_x))	return ENG_JWNS::en_wait;
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : 현재 Trigger와 Strobe 동작을 Enable 시킨다.
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetTrigEnabled()
{
	/* 현재 Trigger Mode가 Enable 상태인지 확인 */
	if (uvCmn_Trig_IsStrobEnable())	return ENG_JWNS::en_next;
	/* Trigger Mode (Trigger & Strobe)를 강제로 Enable 시킴 */
	if (!uvEng_Trig_ReqTriggerStrobe(TRUE))
	{
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : 현재 Trigger와 Strobe가 Enable 상태인지 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsTrigEnabled()
{
	/* 현재 Trigger Mode가 Enable 상태인지 확인 */
	if (uvCmn_Trig_IsStrobEnable())	return ENG_JWNS::en_next;

	return ENG_JWNS::en_wait;
}

/*
 desc : 현재 위치에서 Mark Image를 Grab !!!
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::GetGrabQuartzData()
{
	UINT8 u8ChNo		= m_u8ACamID;	/* Camera Number가 Channel Number */
	ENG_MMDI enACamDrv	= m_u8ACamID == 0x01 ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;
	UINT32 u32Model		= UINT32(ENG_MMDT::en_circle);
	DOUBLE dbParam[2]	= { 256.0f,
							(DOUBLE)ROUNDED(uvEng_GetConfig()->acam_spec.in_ring_size * 1000.0f, 0) / 2.0 };
#if 0	/* 프로그램 초기 실행할 때, 자동으로 등록 됨 */
	/* Quartz 내 Mark를 인식할 Mark Model 등록 */
	if (!uvEng_Camera_SetModelDefineEx(m_u8ACamID,
									   uvEng_GetConfig()->mark_find.model_speed,	/* Medium */
									   uvEng_GetConfig()->mark_find.model_smooth,
									   &u32Model,		/* circle */
									   &dbParam[0],		/* BLACK (256) or WHITE (128) */
									   &dbParam[1],		/* Width or Radius or Diameter, ... */
									   NULL, NULL, NULL, 1,
									   ))
	{
		return ENG_JWNS::en_error;
	}
#endif
	/* 기존 Live & Edge & Calibration 데이터 초기화 */
	uvEng_Camera_ResetGrabbedImage();

	/* Trigger 1개 발생 */
	if (!uvEng_Trig_ReqTrigOutOneOnly(u8ChNo))
	{
		return ENG_JWNS::en_error;
	}

	/* Image Grabbed 시작한 시간 저장 */
	m_u64TickGrabbed	= GetTickCount64();

	return ENG_JWNS::en_next;
}

/*
 desc : Left 쪽 마크 이미지를 Grabbed 했는지 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsGrabbQuartzData()
{
	ENG_MMDI enACamDrv	= m_u8ACamID == 0x01 ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;
	LPG_ACGR pstGrab	= NULL;

	/* Grabbed Image가 존재하는지 확인 */
	pstGrab = uvEng_Camera_RunModelCali(m_u8ACamID, 0xff);	/* 이미지 중심으로 이동하기 때문에 무조건 0xff 값 */
	if (pstGrab && 0x00 != pstGrab->marked)
	{
		m_dbDiffCamX	= pstGrab->move_mm_x;	/* Axis X (Align Camera) Error Value */
		m_dbDiffStageY	= pstGrab->move_mm_y;	/* Axis Y (Stage) Error Value */
#if 0
		TRACE(L"m_dbDiffCamX = %9.4f m_dbDiffStageY = %9.4f\n", m_dbDiffCamX, m_dbDiffStageY);
#endif
		/* !!! 중요. 만약 오차 값이 0.2 um 이내이면, 이동 종료 !!! */
		if (abs(m_dbDiffCamX) < MOTION_CALI_CENTERING_MIN &&
			abs(m_dbDiffStageY) < MOTION_CALI_CENTERING_MIN)
		{
			/* Quartz Mark 중심을 Left or Right 이동 후 측정하기 위한 루틴으로 Skip */
			m_u8WorkStep	= 0x0c;
		}
		/* 다음 단계로 이동 */
		return ENG_JWNS::en_next;
	}

	/* 3 초 동안 Grabbed Image가 없는 경우, 다시 트리거 발생 시키기 위해 뒤로 이동 */
	if (m_u64TickGrabbed+3000 < GetTickCount64())
	{
		m_u8WorkStep	-= 2;
		return ENG_JWNS::en_next;
	}

	return ENG_JWNS::en_wait;
}

/*
 desc : 현재 위치에서 주어진 오차 값만 모션 미세 위치 조정
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetMotionMoving()
{
	UINT8 u8InstAngle	= uvEng_GetConfig()->set_cams.acam_inst_angle;
	ENG_MDMD enDirect	= ENG_MDMD::en_move_left;
	ENG_MMDI enACamDrv	= m_u8ACamID == 0x01 ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;
	DOUBLE dbACamPosX	= 0.0f, dbStagePosY = 0.0f;
	DOUBLE dbStepVelo	= uvEng_GetConfig()->mc2_svc.step_velo;

	/* 현재 위치 얻기 */
	dbACamPosX	= uvCmn_MC2_GetDrvAbsPos(enACamDrv);
	dbStagePosY	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);

	/* Align Camera 이동 */
	if (abs(m_dbDiffCamX) > MOTION_CALI_CENTERING_MIN)
	{
		uvCmn_MC2_GetDrvDoneToggled(enACamDrv);
#if 0
		if (m_dbDiffCamX < 0.0f)	enDirect = ENG_MDMD::en_move_left;	/* Mark 측정 후 오차 값이 양수 값이면 */
		else						enDirect = ENG_MDMD::en_move_right;	/* Mark 측정 후 오차 값이 음수 값이면 */
		if (!uvEng_MC2_SendDevRefMove(ENG_MMMM::en_move_step,
									  enACamDrv, enDirect, abs(m_dbDiffCamX)))
		{
			return ENG_JWNS::en_error;
		}
#else
		/* Align Camera의 X 위치가 QuartZ의 중앙 위치로 이동 */
		if (u8InstAngle)	dbACamPosX	-= m_dbDiffCamX;
		else				dbACamPosX	+= m_dbDiffCamX;
		if (!uvEng_MC2_SendDevAbsMove(enACamDrv, dbACamPosX, dbStepVelo))
			return ENG_JWNS::en_error;
#endif
	}
	/* Stage Y 이동 */
	if (abs(m_dbDiffStageY) > MOTION_CALI_CENTERING_MIN)
	{
		uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_y);
#if 0
		/* Stage Y Axis 이동 */
		if (m_dbDiffStageY < 0.0f)	enDirect = ENG_MDMD::en_move_down;	/* Mark 측정 후 오차 값이 양수 값이면 */
		else						enDirect = ENG_MDMD::en_move_up;	/* Mark 측정 후 오차 값이 음수 값이면 */
		/* 특정 위치로 이동 */
		if (!uvEng_MC2_SendDevRefMove(ENG_MMMM::en_move_step,
									  ENG_MMDI::en_stage_y, enDirect, abs(m_dbDiffStageY)))
		{
			return ENG_JWNS::en_error;
		}
#else
		/* Align Camera의 X 위치가 QuartZ의 중앙 위치로 이동 */
		if (u8InstAngle)	dbStagePosY	+= m_dbDiffStageY;
		else				dbStagePosY	-= m_dbDiffStageY;
		if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_y, dbStagePosY, dbStepVelo))
			return ENG_JWNS::en_error;
#endif
	}

	return ENG_JWNS::en_next;
}

/*
 desc : 원하는 위치까지 이동이 완료 되었는지 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsMotionMoved()
{
	ENG_MMDI enACamDrv	= m_u8ACamID == 0x01 ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;
	/* 미세 조정으로 이동하고 있는 Align Camera와 Stage Y Axis이 모두 이동되었는지 확인 */
	if (abs(m_dbDiffCamX) > MOTION_CALI_CENTERING_MIN && !uvCmn_MC2_IsDrvDoneToggled(enACamDrv))
	{
		return ENG_JWNS::en_wait;
	}
	if (abs(m_dbDiffStageY) > MOTION_CALI_CENTERING_MIN && !uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_y))
	{
		return ENG_JWNS::en_wait;
	}
	/* Quartz 중심 이동 하기 위해 다시 측정 진행 */
	m_u8WorkStep	= 0x08;

	return ENG_JWNS::en_next;
}

/*
 desc : 현재 선택된 카메라 기준으로, Quartz의 Mark 중심 위치를 왼쪽 or 오른쪽으로 이동하기 위해
		일정 거리만큼 Stage X 좌표 이동
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetMotionMovingX()
{
	DOUBLE dbMovePos	= 0.0f, dbDist[2]	= {NULL}, dbMoveSpeed	= 0.0f;
	DOUBLE dbPointDist	= uvEng_GetConfig()->acam_spec.point_dist;

	/* 이동 거리 입력 */
	dbDist[0]	= -dbPointDist / 2.0f;
	dbDist[1]	= dbPointDist;

	/* 현재 선택된 Align Camera */
	dbMovePos	= uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x) + dbDist[m_u8GrabbedCount];
	dbMoveSpeed	= 50;	/* Speed : 50 mm/sec */

	/* 현재 이동하고자 하는 모션의 Toggled 값 얻기 */
	uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI::en_stage_x);
	/* 사이드로 이동 시키기 */
	if (!uvEng_MC2_SendDevAbsMove(ENG_MMDI::en_stage_x, dbMovePos, dbMoveSpeed))
	{
		return ENG_JWNS::en_error;
	}

	return ENG_JWNS::en_next;
}

/*
 desc : 현재 선택된 카메라 기준으로, Quartz의 Mark 중심 위치를 왼쪽 or 오른쪽으로 이동하기 위해
		일정 거리만큼 Stage X 좌표 이동 했는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsMotionMovedX()
{
	if (!uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI::en_stage_x))	return ENG_JWNS::en_wait;

	return ENG_JWNS::en_next;
}

/*
 desc : 원하는 이미지를 Grabbed 했는지 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsGrabbQuartzMoveX()
{
	LPG_ACGR pstGrab= NULL;

	/* Grabbed Image가 존재하는지 확인 */
	pstGrab = uvEng_Camera_RunModelCali(m_u8ACamID, m_u8RunMode);
	if (pstGrab && 0x00 != pstGrab->marked)
	{
		/* Grabbed Image의 Mark 중심 좌표 */
		m_stGrabCentXY[m_u8GrabbedCount].x	= pstGrab->mark_cent_px_x;
		m_stGrabCentXY[m_u8GrabbedCount].y	= pstGrab->mark_cent_px_y;
		m_stGrabMoveXY[m_u8GrabbedCount].x	= pstGrab->move_px_x;
		m_stGrabMoveXY[m_u8GrabbedCount].y	= pstGrab->move_px_y;
#if 0
		TRACE(L"cam_id[%u]  id[%u]  cent_px_xy = %.6f / %.5f  move_px_xy = %.6f / %.6f\n",
			  m_u8ACamID, m_u8GrabbedCount,
			  pstGrab->mark_cent_px_x, pstGrab->mark_cent_px_y,
			  pstGrab->move_px_x, pstGrab->move_px_y);
#endif
		/* 어느 작업으로 Skip 수행할지 결정 */
		if (0 == m_u8GrabbedCount)
		{
			m_u8GrabbedCount++;
			m_u8WorkStep	= 0x0c;	/* 2 번째 (Right) 측정하기 위해 이동 조치 */
			/* 다음 단계로 이동 */
			return ENG_JWNS::en_next;
		}
		return ENG_JWNS::en_next;
	}

	/* 3 초 동안 Grabbed Image가 없는 경우, 다시 트리거 발생 시키기 위해 뒤로 이동 */
	if (m_u64TickGrabbed+3000 < GetTickCount64())
	{
		m_u8WorkStep	-= 2;
		return ENG_JWNS::en_next;
	}

	return ENG_JWNS::en_wait;
}

/*
 desc : Align Camera의 Angle과 Pixel Resolution 구하기
 parm : flag	- [in]  TRUE : 모든 작업 완료, FALSE : 측정 1회 완료
 retn : None
*/
VOID CMainThread::CalcACamSpec(BOOL flag)
{
	DOUBLE dbRadian		= 0.0f, dbDegree = 0.0f, dbPixelSize = 0.0f, dbCrossDist = 0.0f;
	DOUBLE dbMoveDist	= uvEng_GetConfig()->acam_spec.point_dist * 1000.0f;	/* unit : um */
	ENG_MMDI enACamDrv	= m_u8ACamID == 0x01 ? ENG_MMDI::en_align_cam1 : ENG_MMDI::en_align_cam2;

	/* Align Camera : Angle */
	dbRadian	= atan2((m_stGrabCentXY[0].y - m_stGrabCentXY[1].y), (m_stGrabCentXY[0].x - m_stGrabCentXY[1].x));
	dbDegree	= (dbRadian * 180.0f) / M_PI;
	TRACE(L"ACamNo = %d  Degree = %.4f\n", m_u8ACamID, dbDegree);
	if (uvEng_GetConfig()->set_cams.acam_inst_angle)
	{
		uvEng_GetConfig()->acam_spec.spec_angle[m_u8ACamID-1]	= dbDegree - 180.0f;	/* unit : degree */
	}
	else
	{
		uvEng_GetConfig()->acam_spec.spec_angle[m_u8ACamID-1]	= dbDegree;	/* unit : degree */
	}
	/* Align Camera : Pixel Resolution */
	dbCrossDist	= sqrt(pow(m_stGrabCentXY[0].x - m_stGrabCentXY[1].x, 2) + pow(m_stGrabCentXY[0].y - m_stGrabCentXY[1].y, 2));
	dbPixelSize = dbMoveDist / dbCrossDist;
	uvEng_GetConfig()->acam_spec.spec_pixel_um[m_u8ACamID-1]	= dbPixelSize;	/* unit : um */
#if 0
	TRACE(L"Align Camera [Degree / PixelSize] = %.8f (degree:%.8f), %.8f\n", 
		  uvEng_GetConfig()->acam_spec.spec_angle[m_u8ACamID-1], dbDegree, dbPixelSize);
#endif

	/* 결과 정보 부모에게 알림 */
	SendMesgResult(flag);
}

/*
 desc : 현재 작업 진행율 확인하기
 parm : None
 retn : 작업 진행률 값 (단위: Percent)
*/
DOUBLE CMainThread::GetStepRate()
{
	DOUBLE dbRate	= 0.0f;

	dbRate	= ((m_u8WorkStep * 1.0f) / DOUBLE(m_u8WorkTotal)) * 100.0f;
	if (dbRate > m_dbRateLast)	m_dbRateLast	= dbRate;

	/* 가장 최근에 진행률 값이 큰 기준 값만 반환 */
	return m_dbRateLast;
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