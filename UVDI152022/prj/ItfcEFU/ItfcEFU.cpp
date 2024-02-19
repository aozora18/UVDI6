
/*
 desc : EFU Library for Sinsung
*/

#include "pch.h"
#include "MainApp.h"

#include "EFUThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/* ------------------------------------------------------------------------------------ */
/*                                      전역 변수                                       */
/* ------------------------------------------------------------------------------------ */

LPG_EPAS		g_pstShMemEFU		= NULL;
CEFUThread		*g_pEFUThread		= NULL;		/* 작업 스레드 */


/* ------------------------------------------------------------------------------------ */
/*                                      내부 함수                                       */
/* ------------------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------------------ */
/*                                      외부 함수                                       */
/* ------------------------------------------------------------------------------------ */

#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : EFU Library 초기화
 parm : config	- [in]  환경 설정 정보
		shmem	- [in]  EFU 관련 공유 메모리
 retn : None
*/
API_EXPORT BOOL uvEFU_Init(LPG_CIEA config, LPG_EPAS shmem)
{
	/* 공유 메모리 연결 */
	g_pstShMemEFU	= shmem;
	/* 전역 환경 파일 연결 */
	g_pstConfig		= config;

	/* 시리얼 통신 스레드 생성 */
	g_pEFUThread	= new CEFUThread(config->efu.port_no,
									 config->efu.baud_rate,
									 config->efu.port_buff,
									 config->efu.ext_param_1,
									 shmem);
	ASSERT(g_pEFUThread);
	if (!g_pEFUThread->CreateThread(0, NULL, 100))	return FALSE;

	return TRUE;
}

/*
 desc : PreAlinger Library 해제
 parm : None
 retn : None
*/
API_EXPORT VOID uvEFU_Close()
{
	/* 작업 스레드 메모리 해제 */
	if (g_pEFUThread)
	{
		if (g_pEFUThread->IsBusy())		g_pEFUThread->Stop();
		while (g_pEFUThread->IsBusy())	g_pEFUThread->Sleep();
		delete g_pEFUThread;
		g_pEFUThread	= NULL;
	}
}

/*
 desc : 현재 EFU와 연결되어 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFU_IsConnected()
{
	if (!g_pEFUThread->IsBusy())	return FALSE;
	return g_pEFUThread->IsOpenPort();
}

/*
 desc : 통신 응답 대기 시간 값 설정
 parm : wait	- [in]  통신 (명령) 전송 후 응답이 올 때까지 최대 기다리는 시간 (단위: msec)
 retn : None
*/
API_EXPORT VOID uvEFU_SetAckTimeout(UINT64 wait)
{
	g_pEFUThread->SetAckTimeout(wait);
}

/*
 desc : 기존 통신 관련 송/수신 데이터 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvEFU_ResetCommData()
{
	g_pEFUThread->ResetCommData();
}

/*
 desc : EFU 설정 (Speed) 요청
 parm : bl_id	- [in]  EFU ID 즉, 0x01 ~ 0x20 (1 ~ 32), 0x00 - ALL
		speed	- [in]  Setting Value 즉, 0 ~ 140 (0 ~ 1400) RPM (1 = 10 RPM 의미)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFU_ReqSetSpeed(UINT8 bl_id, UINT8 speed)
{
	if (g_pEFUThread->IsGetAutoState())
	{
		LOG_WARN(ENG_EDIC::en_efu_ss, L"This is the current automatic status information request mode");
		return FALSE;
	}
	return g_pEFUThread->ReqSetSpeed(bl_id, speed);
}

/*
 desc : EFU 설정 가능 모드인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFU_IsSetSpeed()
{
	return !g_pEFUThread->IsGetAutoState();
}

/*
 desc : EFU 상태 요청
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFU_ReqGetState()
{
	return g_pEFUThread->ReqGetState();
}

/*
 desc : 명령어 전송해도 되는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEFU_IsSendIdle()
{
	return g_pEFUThread->IsSendIdle();
}

/*
 desc : 주기적으로 EFU의 상태 정보 요청할 것인지 여부 설정
 parm : enable	- [in]  주기적으로 상태 정보 요청 (TRUE) or 요청하지 않음 (FALSE)
 retn : None
 note : enable 값이 TRUE이면, EFU의 RPM 설정 명령이 수행되지 않습니다.
		EFU의 RPM 설정을 하려면, 반드시 이 값을 Disable (FALSE) 시켜야 됩니다.
*/
API_EXPORT VOID uvEFU_SetPeriodState(BOOL enable)
{
	g_pEFUThread->SetAutoEnable(enable);
}

#ifdef __cplusplus
}
#endif