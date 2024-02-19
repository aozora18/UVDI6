
/*
 desc : EFU Library
*/

#pragma once

#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
#include "../conf/conf_podis.h"
#elif (CUSTOM_CODE_GEN2I == DELIVERY_PRODUCT_ID)
#include "../conf/conf_gen2i.h"
#endif

#include "../conf/efu.h"


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
API_IMPORT BOOL uvEFU_Init(LPG_CIEA config, LPG_EPAS shmem);
/*
 desc : PreAlinger Library 해제
 parm : None
 retn : None
*/
API_IMPORT VOID uvEFU_Close();
/*
 desc : 현재 EFU와 연결되어 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEFU_IsConnected();
/*
 desc : 통신 응답 대기 시간 값 설정
 parm : wait	- [in]  통신 (명령) 전송 후 응답이 올 때까지 최대 기다리는 시간 (단위: msec)
 retn : None
*/
API_IMPORT VOID uvEFU_SetAckTimeout(UINT64 wait);
/*
 desc : 기존 통신 관련 송/수신 데이터 초기화
 parm : None
 retn : None
*/
API_IMPORT VOID uvEFU_ResetCommData();
/*
 desc : EFU 설정 가능 모드인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEFU_IsSetSpeed();
/*
 desc : EFU 설정 (Speed) 요청
 parm : bl_id	- [in]  EFU ID 즉, 0x01 ~ 0x20 (1 ~ 32), 0x00 - ALL
		speed	- [in]  Setting Value 즉, 0 ~ 140 (0 ~ 1400) RPM (1 = 10 RPM 의미)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEFU_ReqSetSpeed(UINT8 bl_id, UINT8 speed);
/*
 desc : EFU 상태 요청
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEFU_ReqGetState();
/*
 desc : 명령어 전송해도 되는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEFU_IsSendIdle();
/*
 desc : 주기적으로 EFU의 상태 정보 요청할 것인지 여부 설정
 parm : enable	- [in]  주기적으로 상태 정보 요청 (TRUE) or 요청하지 않음 (FALSE)
 retn : None
 note : enable 값이 TRUE이면, EFU의 RPM 설정 명령이 수행되지 않습니다.
		EFU의 RPM 설정을 하려면, 반드시 이 값을 Disable (FALSE) 시켜야 됩니다.
*/
API_IMPORT VOID uvEFU_SetPeriodState(BOOL enable);


#ifdef __cplusplus
}
#endif