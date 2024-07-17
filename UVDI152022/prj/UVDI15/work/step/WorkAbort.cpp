
/*
 desc : 모든 동작을 강제로 중지처리
*/

#include "pch.h"
#include "../../MainApp.h"
#include "WorkAbort.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : 생성자
 parm : forced	- [in]  강제로 노광 중지 기능 실행 즉, 노광 중이지 않는 상태에서, Luria로부터
						노광 중지 완료 응답을 받지 않고, 모든 축의 모터가 중지만 되어 있다면
						강제로 노광 중지 작업을 실행 시킴
 retn : None
*/
CWorkAbort::CWorkAbort(UINT8 forced)
	: CWorkStep()
{
	m_enWorkJobID	= ENG_BWOK::en_work_stop;
	m_u8AbortForced	= forced;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CWorkAbort::~CWorkAbort()
{
}

/*
 desc : 초기 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkAbort::InitWork()
{
	/* 내부 멤버 변수 값 초기화 */
	if (!CWork::InitWork())	return FALSE;

	/* 전체 작업 단계 */
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
	m_u8StepTotal = 0x07;
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	m_u8StepTotal	= 0x0a;
#endif

	//m_u8StepTotal = 0x07;
	/*Philhmi에 장비 정지 신호 보내기*/
	//PhilInterrupsStop();
	return TRUE;
}

/*
 desc : 주기적으로 작업 수행
 parm : None
 retn : None
*/
VOID CWorkAbort::DoWork()
{
	/* 작업 단계 별로 동작 처리 */
	switch (m_u8StepIt)
	{
		case 0x01 : m_enWorkState = StopPrinting();							break;
		/* GetExposeState 요청에 대한 응답이 올 때까지 일정 시간 대기 */
		case 0x02 : m_enWorkState = SetWorkWaitTime(1000);					break;
		case 0x03 : m_enWorkState = IsWorkWaitTime();						break;
		case 0x04 : m_enWorkState = IsStopPrinting();						break;
		/* 스테이지가 멈출 때까지 일정 시간 대기 */
		case 0x05 : m_enWorkState = SetWorkWaitTime(2000);					break;
		case 0x06 : m_enWorkState = IsWorkWaitTime();						break;
		case 0x07 : m_enWorkState = SetTrigEnable(FALSE);					break;
		case 0x08 : m_enWorkState = SetMovingUnloader();					break;
		case 0x09: m_enWorkState = IsTrigEnabled(FALSE);					break;
		case 0x0a: m_enWorkState = IsMovedUnloader();						break;
	}

	
	/* 다음 작업 진행 여부 판단 */
	CWork::SetWorkNext();
	/* 장시간 동안 동일 위치를 반복 수행한다면 에러 처리 */
	CWork::CheckWorkTimeout();
}

/*
 desc : 만약 현재 노광 중이면, 강제로 노광 해제 시킴
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkAbort::StopPrinting()
{
	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Stop.Printing");

	/* 노광 시작을 알리는 램프 (전면 2개의 버튼 스위치) 끄기 */
	// by sysandj : MCQ대체 추가 필요 : uvEng_MCQ_StartLampEnable(0x00);

	if (!uvCmn_Luria_IsExposeIdle())
	{
		if (!uvEng_Luria_ReqSetPrintAbort())
		{
			LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetPrintAbort)");
			return ENG_JWNS::en_error;
		}
		if (!uvEng_GetConfig()->luria_svc.use_announcement)
		{
			/* 명령 송신 시간 갱신 */
			SetSendCmdTime();

			/* Printing Status 정보 요청 */
			if (0x00 == uvEng_Luria_ReqGetExposureState())
			{
				LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetExposureState)");
				return ENG_JWNS::en_error;
			}
		}
	}

	/* 장시간 대기하기를 방지하기 위함 */
	m_u64TickAbortWait	= GetTickCount64();

	return ENG_JWNS::en_next;
}

/*
 desc : 현재 노광 중인지 여부 확인
 parm : None
 retn : wait, error, complete or next
*/
ENG_JWNS CWorkAbort::IsStopPrinting()
{
	/* 현재 작업 Step Name 설정 */
	SetStepName(L"Is.Stop.Printing");

	/* 만약 Printing 중이라면, 강제로 중지 */
	if (0x00 == m_u8AbortForced)
	{
		/* 현재 Luria가 노광 대기 모드 상태인지 여부 */
		if (uvCmn_Luria_IsExposeIdle())
		{
			return ENG_JWNS::en_next;
		}

		if (!uvEng_GetConfig()->luria_svc.use_announcement)
		{
			/* 다시 명령 보내도 되는 시간인지 판단 */
			if (IsSendCmdTime(250))
			{
				/* Printing Status 정보 요청 */
				if (0x00 == uvEng_Luria_ReqGetExposureState())
				{
					LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqGetExposureState)");
					return ENG_JWNS::en_error;
				}
			}
		}
		/* 다시 명령 보내도 되는 시간인지 판단 */
		if (IsSendCmdTime(250))
		{
			/* 송신 시간 저장 */
			SetSendCmdTime();
			/* 강제로 Printing 종료 */
			if (!uvEng_Luria_ReqSetPrintAbort())	return ENG_JWNS::en_next;
		}
		/* 5 초 동안 응답이 없으면, 강제로 다음 작업으로 Skip 처리 */
		if (GetTickCount64() > m_u64TickAbortWait + 5000)
		{
			/* 모든 모터가 멈춘 상태인지 재차 확인 */
			if (uvCmn_MC2_IsMotorDriveStopAll())
			{
				m_u8StepIt		= 0x00;
				m_u8AbortForced	= 0x01;
				return ENG_JWNS::en_next;
			}
		}
	}
	/* 모든 축의 Motion이 중지된 상태라면, 바로 다음 작업으로 Skip 처리 */
	else
	{
		/* 모든 모터가 멈춘 상태인지 재차 확인 */
		if (uvCmn_MC2_IsMotorDriveStopAll())	
			return ENG_JWNS::en_next;
	}

	return ENG_JWNS::en_wait;
}

/*
 desc : Philhmi에 장비 정지 신호 보내기
 parm : None
 retn : None
*/
VOID CWorkAbort::PhilInterrupsStop()
{
	STG_PP_P2C_INTERRUPT_STOP		stP2CInterruptStop;
	STG_PP_P2C_INTERRUPT_STOP_ACK	stInterruptStopAck;

	stP2CInterruptStop.Reset();
	stInterruptStopAck.Reset();

	uvEng_Philhmi_Send_P2C_INTERRUPT_STOP(stP2CInterruptStop, stInterruptStopAck);



	if (stInterruptStopAck.usErrorCode)
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Philhmi Failure Equipment stop signal");
	}
}