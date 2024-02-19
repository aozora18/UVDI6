
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
	m_hParent		= parent;
	m_u8WorkStep	= 0x00;
	m_bRunFocus		= FALSE;
	m_u8RetryTrig	= 0x00;
	/* 측정된 엣지 정보를 분석 및 저장 관리하는 자식 다이얼로그 객체 */
	m_pMeasure		= new CMeasure();
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

	m_u8RetryTrig	= 0x00;

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
 parm : timeout	- [in]  SendMessag Lifetime (unit: msec)
 retn : None
*/
VOID CMainThread::SendMesgResult(UINT32 timeout)
{
	DWORD_PTR dwResult	= 0;
	LRESULT lResult		= 0;

	/* 부모에게 이벤트 메시지 전달 */
	lResult	= ::SendMessageTimeout(m_hParent, WM_MAIN_THREAD, MSG_ID_STEP_RESULT, 0L,
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
 parm : cam_id	- [in]  카메라 인덱스 1 or Later
		step	- [in]  측정 타켓 (포토헤드 혹은 카메라 Z 축) 이동 간격 (단위: mm)
		skip	- [in]  엣지 측정을 위해 LED 안정화하는데 필요한 횟수 (실제 분석하지 않고 버림)
		repeat	- [in]  한 Step 당 반복 측정 횟수 (여러 번 측정 후 평균 값을 구하기 위함)
		total	- [in]  총 Step 이동 횟수 즉, step 크기 만큼 상/하 이동 횟수 (일단, Min/Max 값이 벗어나지 않도록 유의)
 retn : None
*/
VOID CMainThread::StartFocus(UINT8 cam_id,
							 DOUBLE step,
							 UINT32 skip,
							 UINT32 repeat,
							 UINT32 total)
{
	/* 동기 진입 */
	if (m_syncFocus.Enter())
	{
		m_u8ACamID		= cam_id;
		m_dbInitPosZ	= uvCmn_MCQ_GetACamCurrentPosZ(cam_id);	/* 현재 모터 Z 축 위치 값 저장 */
		m_dbStepSize	= step;		/* UP : 양수 값, DOWN : 음수 값 */
		m_u32SkipCount	= skip;
		m_u32RepCount	= repeat;
		m_u32TotCount	= total;

		/* 내부 멤버 초기화 */
		m_bRunFocus		= TRUE;
		m_u8WorkStep	= 0x01;
		m_u8WorkTotal	= 0x07;
		m_u32MeasRep	= 0;
		m_u32MeasTot	= 0;
		m_u32MeasSkip	= 0;
		m_u64DelayTime	= GetTickCount64();

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
	/* Align Camera Z : Homing */
	case 0x01 : m_enWorkState = ACamZHoming();			break;
	/* Align Camera Z : IsHomed */
	case 0x02 : m_enWorkState = IsACamZHomed();			break;
	/* 측정 위치로 이동할지 결정 */
	case 0x03 : m_enWorkState = SetMoveStep();			break;
	/* 측정 위치로 이동했는지 확인 */
	case 0x04 : m_enWorkState = IsMovedStep();			break;
	/* Trigger 1개 발생 */
	case 0x05 : m_enWorkState = PutOneTrigger();		break;
	/* Grabbed Image 수집 및 Edge Detect 수행 */
	case 0x06 : m_enWorkState = ProcDetectImage();		break;
	/* 결과 데이터 수집 후 분석 진행 */
	case 0x07 : m_enWorkState = ResultDetectImage();	break;
	}

	/* 다음 작업 진행 여부 판단 */
	SetWorkNext();
	/* 장시간 동안 동일 위치를 반복 수행한다면 에러 처리 */
	IsWorkTimeOut();
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
		m_bRunFocus		= FALSE;
		return;
	}

	if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* 모든 작업을 끝낸 경우인지 */
		if (m_u8WorkTotal != m_u8WorkStep)
		{
			/* 현재 Skip 처리 단계인지 여부 */
			if (0x06 == m_u8WorkStep && 0 == m_u32MeasRep)
			{
				m_u8WorkStep	= 0x05;	/* 시험 측정 위치로 이동 */
			}
			else	m_u8WorkStep++;	/* 다음 작업 단계로 이동 */
		}
		/* 모든 작업이 완료된 경우 */
		else
		{
			/* 다시 현재 위치에서 반복 측정하라고 Step 값 조정 */
			if (m_u32MeasRep != m_u32RepCount)
			{
				m_u8WorkStep	= 0x05;
			}
			/* 현재 위치에서 반복 측정 횟수가 모두 측정이 되었는지 */
			else
			{
				m_u32MeasRep	= 0x00;	/* 현재 위치에서 반복 측정 횟수 초기화 */
				m_u32MeasSkip	= 0x00;	/* 현재 위치에서 반복 시험 횟수 초기화 */
				m_u32MeasTot++;			/* 전체 측정 횟수 증가 처리 */
				/* 전체 측정 작업 횟수만큼 동작 했는지 */
				if (IsCompleted())
				{
					m_enWorkState	= ENG_JWNS::en_comp;
					m_bRunFocus		= FALSE;
				}
				else
				{
					/* 다시 처음으로 이동하기 위함 */
					m_u8WorkStep= 0x01;
				}
			}
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
	if (!m_bRunFocus || (m_u32TotCount == m_u32MeasTot))	return TRUE;

	return FALSE;
}

/*
 desc : 얼라인 카메라 Z 축 Homing 요청
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::ACamZHoming()
{
	if (!uvEng_MCQ_SetACamHomingZAxis(m_u8ACamID))	return ENG_JWNS::en_error;
	return ENG_JWNS::en_next;
}

/*
 desc : 얼라인 카메라 Z 축 Homing 완료 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsACamZHomed()
{
	/* Homing 하는데 최소 필요한 동작 시간 줘야 됨 */
#ifdef _DEBUG
	if (GetTickCount64() < m_u64DelayTime + 2000)	return ENG_JWNS::en_wait;
#else
	if (GetTickCount64() < m_u64DelayTime + 1000)	return ENG_JWNS::en_wait;
#endif
	if (!uvCmn_MCQ_IsACamZReady(m_u8ACamID))	return ENG_JWNS::en_wait;

	/* 모터가 안정화될 때까지 약간 대기 */
	uvCmn_uSleep(500000);	/* 약 0.5 초 */

	return ENG_JWNS::en_next;
}

/*
 desc : 측정 위치로 모터 이동
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::SetMoveStep()
{
	/* 현재 이동해야 할 위치 계산 (최초 시작 위치 + 현재 이동한 횟수 * 이동 크기) */
	m_dbMeasPosZ	= m_dbInitPosZ + (m_u32MeasTot * m_dbStepSize);
	/* Align Camera Z 축 이동 */
	if (!uvEng_MCQ_SetACamMovePosZ(m_u8ACamID, 0x00, m_dbMeasPosZ))	return ENG_JWNS::en_error;

	return ENG_JWNS::en_next;
}

/*
 desc : 이동을 완료 했는지 여부
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::IsMovedStep()
{
	DOUBLE dbPosZ	= 0.0f, dbValidMin, dbValidMax;

	/* 현재 Z Axis 위치 값 얻기 */
	dbPosZ	= uvCmn_MCQ_GetACamCurrentPosZ(m_u8ACamID);

	/* Z Axis 위치가 있어야 할 유효 범위 값 설정 */
	dbValidMax	= dbPosZ + 0.001 /* 1 um */;
	dbValidMin	= dbPosZ - 0.001 /* 1 um */;

	/* 측정하고자 하는 위치와 동일한지 여부 (+/- 10 um 정도 오차는 인정) */
	if (m_dbMeasPosZ > dbValidMin && m_dbMeasPosZ < dbValidMax)
	{
		return ENG_JWNS::en_next;	/* +/- 1.0 um 이내에 현재 Z 축 위치가 이동된 상태임 */
	}

	/* 모터가 안정화될 때까지 약간 대기 */
	uvCmn_uSleep(500000);	/* 약 0.5 초 */

	return ENG_JWNS::en_wait;	/* 아직 이동이 완료되지 않았음 */
}

/*
 desc : 트리거 1개 발생 시킴
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::PutOneTrigger()
{
	UINT8 u8ChNo	= m_u8ACamID;	/* MAX 4 */

	/* 현재 트리거 명령 버퍼에 임의 개수 이하가 대기 중인지 여부 */
	/* Trigger 송신 버퍼에 과도하게 많은 명령어가 저장되는 것 방지 */
	if (!uvEng_Trig_IsSendCmdCountUnder(3))	return ENG_JWNS::en_wait;

	/* Align Camera is Edge Detection Mode */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_edge_mode);

	/* 기존 Live & Edge & Calibration 데이터 초기화 */
	uvEng_Camera_ResetGrabbedImage();

	/* Trigger 강제로 (수동으로) 발생 */
	if (!uvEng_Trig_ReqTrigOutOne(u8ChNo, TRUE))
	{
		AfxMessageBox(L"Failed to send the event for trigger", MB_ICONSTOP|MB_TOPMOST);
		return ENG_JWNS::en_error;
	}

	/* 이미지 Grabbed 확인 시간 갱신 */
	m_u64GrabTime	= GetTickCount64();

	return ENG_JWNS::en_next;
}

/*
 desc : 현재 Grabbed Image 즉, Edge Detection된 이미지가 있는지 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CMainThread::ProcDetectImage()
{
	/* Grabbed Image가 있으며, Edge 결과가 있는지 여부 */
	if (!uvEng_Camera_RunEdgeDetect(m_u8ACamID))
	{
		/* 3 초 동안 Grabbed Image가 없는 경우, 다시 트리거 발생 시키기 위해 뒤로 이동 */
		if (!(m_u64GrabTime+3000 < GetTickCount64()))	return ENG_JWNS::en_wait;
		/* 다시 이미지 Grbbaed 작업 수행하라고 지시 */
		m_u8WorkStep	-= 2;
		return ENG_JWNS::en_next;
	}
	/* Grabbed Image가 있지만, Edge 결과 후 검색 실패인지 여부 */
	if (!uvEng_Camera_GetEdgeDetectResults(m_u8ACamID)[0].set_data)
	{
		/* 다시 이미지 Grbbaed 작업 수행하라고 지시 */
		if (m_u8RetryTrig < 0x03)
		{
			m_u8WorkStep	-= 2;
			return ENG_JWNS::en_next;
		}
		AfxMessageBox(L"Failed to detect the grabbed image", MB_ICONSTOP|MB_TOPMOST);
		return ENG_JWNS::en_error;
	}

	/* 트리거 발생 횟수 초기화 */
	m_u8RetryTrig	= 0x00;
	/* Edge Detect 수행을 정상적으로 했으면, Skip Count 증가 처리 */
	if (m_u32MeasSkip < m_u32SkipCount)	m_u32MeasSkip++;
	else								m_u32MeasRep++;

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
	if (!m_pMeasure->SetResult(m_u8ACamID))	return ENG_JWNS::en_error;
	/* 상위 부모에게 알림 */
	SendMesgResult();

	TRACE(L"m_u32MeasRep = %d  [%d]\n", m_u32MeasRep, m_pMeasure->GetCount());

	return ENG_JWNS::en_next;
}

/*
 desc : 현재 처리 결과 등록된 개수 반환
 parm : None
 retn : 개수 반환
*/
UINT32 CMainThread::GetGrabCount()
{
	return m_pMeasure->GetCount();
}

/*
 desc : 현재까지 측정된 데이터 반환
 parm : index	- [in]  가져오고자 하는 메모리 위치 (Zero-based)
 retn : index 위치에 저장된 측정 값
*/
LPG_ZAAL CMainThread::GetGrabData(UINT32 index)
{
	LPG_ZAAL pstData	= NULL;

	/* 동기 진입 */
	if (m_syncFocus.Enter())
	{
		pstData	= m_pMeasure->GetResult(index);

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
 desc : 현재 적재된 Edge Image의 측정 값 포인터 반환
 parm : None
 retn : 측정 값 구조체 포인터 반환
*/
LPG_ZAAL CMainThread::GetGrabSel()
{
	return m_pMeasure->GetDataSel();
}

/*
 desc : 측정된 Min / Max 값 반환
 parm : mode	- [in]  0x00: Min, 0x01: Max
		type	- [in]  0x00 : strength, 0x01 : length, 0x02 : Feret, 0x03 : Diameter
 retn : 값 반환
*/
DOUBLE CMainThread::GetMinMax(UINT8 mode, UINT8 type)
{
	return m_pMeasure->GetMinMax(mode, type);
}

/*
 desc : 현재까지 작업 진행율 반환
 parm : None
 retn : 작업 진행율 반환 (단위: percent)
*/
DOUBLE CMainThread::GetWorkStepRate()
{
	DOUBLE dbTotal	= m_u32SkipCount * m_u32RepCount * m_u32TotCount * 1.0f;
	DOUBLE dbCurrent= (m_u32SkipCount * m_u32RepCount * m_u32MeasTot) * 1.0f +
					  (m_u32MeasRep + m_u32MeasSkip) * 1.0f;

	return dbCurrent / dbTotal * 100.0f;
}