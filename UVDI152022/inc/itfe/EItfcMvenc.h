
/*
 desc : The Interface Library for Philoptics's Trigger Device
*/

#pragma once

#include "../conf/global.h"
#include "../conf/define.h"
#if (CUSTOM_CODE_UVDI15_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_UVDI15_LLS06 == DELIVERY_PRODUCT_ID)
#include "../../inc/conf/trig_uvdi15.h"
#elif (CUSTOM_CODE_GEN2I == DELIVERY_PRODUCT_ID)
#elif (CUSTOM_CODE_UVDI15 == DELIVERY_PRODUCT_ID || \
CUSTOM_CODE_HDDI6== DELIVERY_PRODUCT_ID)
#include "../../inc/conf/trig_uvdi15.h"

#endif


#ifdef __cplusplus
extern "C"
{
#endif

/* --------------------------------------------------------------------------------------------- */
/*                             외부 함수 - < Trigger >  < for Engine >                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Trigger & Strobe Enable or Disable
 parm : enable	- [in]  TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Mvenc_ReqTriggerStrobe(BOOL enable);
/*
 desc : 내부 트리거 설정
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Mvenc_ReqEncoderLive(UINT8 cam_id, UINT8 lamp_type);
/*
 desc : 내부 트리거 설정 초기화
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Mvenc_ReqEncoderOutReset();
/*
 desc : 트리거 1개 발생 시킴 (Only Trigger Event)
 parm : ch_no	- [in]  채널 번호 (1 or 2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Mvenc_ReqTrigOutOneOnly(UINT8 ch_no, UINT8 lamp_type);
/*
 desc : 트리거 1개 발생 시킴 (Trigger Enable -> Trigger Event -> Trigger Disable)
 parm : ch_no	- [in]  채널 번호 (1 or 2)
		enable	- [in]  트리거 내보낸 이후 Disable 시킬지 여부
						트리거 1개 발생 후, 곧바로 트리거 Disable 할지 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Mvenc_ReqTrigOutOne(UINT8 channelBit);
API_IMPORT BOOL uvEng_Mvenc_ReqTrigOutOneUseReset(int channelBit);

API_IMPORT BOOL uvEng_Mvenc_ReqTrigDelay(UINT8 channel, int delay);
#if 1
/*
 desc : Trigger Board에 처음 연결되고 난 이후, 환경 파일에 설정된 값으로 초기화 진행
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Mvenc_ReqInitUpdate();
#endif
/*
 desc : 기존 등록된 4개 채널에 대한 Trigger Position 값 초기화 (최대 값) 수행
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Mvenc_ResetTrigPosAll();
/*
 desc : 현재 트리거의 위치 등록 값이 초기화 되었는지 여부
 parm : cam_id	- [in]  Align Camera Index (0x01 or 0x02)
		count	- [in]  채널 번호에 해당되는 검사할 트리거 등록 개수 (MAX: 16)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Mvenc_IsTrigPosReset(UINT8 cam_id, UINT8 count=MAX_REGIST_TRIG_POS_COUNT);
/*
 desc : 현재 트리거에 송신될 명령 버퍼의 대기 개수가 임의 개수 이하인지 여부
 parm : count	- [in]  이 개수 미만이면 TRUE, 이상이면 FALSE
 retn : count 개수 미만 (미포함)이면 TRUE, 이상 (포함)이면 FALSE 반환
*/
API_IMPORT BOOL uvEng_Mvenc_IsSendCmdCountUnder(UINT16 count);
/*
 desc : 채널 별로 트리거 위치 등록
 parm : direct	- [in]  TRUE: 정방향 (전진), FALSE: 역방향 (후진)
		start	- [in]  트리거 저장 시작 위치 (0 ~ 15)
		count	- [in]  트리거 등록 개수 (1 ~ 16)
		pos		- [in]  트리거 등록 위치가 저장되어 있는 배열 포인터 (unit : 100 nm or 0.1 um)
		enable	- [in]  트리거 Enable or Disable
		clear	- [in]  트리거 위치 등록하면서 현재 Trigger Encoder 값을
						Clear (TRUE)할것인지 혹은 읽기 모드 (FALSE)를 유지할 것인지 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Mvenc_ReqWriteAreaTrigPos(BOOL direct,
											   UINT8 start1, UINT8 count1, PINT32 pos1,
											   UINT8 start2, UINT8 count2, PINT32 pos2,
											   ENG_TEED enable, BOOL clear);
/*
 desc : 특정 채널에 트리거 위치 등록
 parm : cam_id	- [in]  Align Camera Index (0x01 or 0x02)
		start	- [in]  트리거 저장 시작 위치 (0 ~ 15)
		count	- [in]  트리거 등록 개수 (1 ~ 16)
		pos		- [in]  트리거 등록 위치가 저장되어 있는 배열 포인터 (unit : 100 nm or 0.1 um)
		enable	- [in]  트리거 Enable or Disable
		clear	- [in]  트리거 위치 등록하면서 현재 Trigger Encoder 값을
						Clear (TRUE)할것인지 혹은 읽기 모드 (FALSE)를 유지할 것인지 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Mvenc_ReqWriteAreaTrigPosCh(UINT8 cam_id, UINT8 start, UINT8 count, PINT32 pos,
												 ENG_TEED enable, BOOL clear);
/*
 desc : 기존에 입력된 Trigger 위치 값과 Trigger Board로부터 수신된 입력 값 비교
 parm : cam_id	- [in]  Align Camera Index (0x01 or 0x02)
		index	- [in]  트리거 저장 위치 (0x000 ~ 0x0f)
		pos		- [in]  트리거 값 (단위: 100 nm or 0.1 um)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Mvenc_IsTrigPosEqual(UINT8 cam_id, UINT8 index, UINT32 pos);

/*
 desc : 카메라 별 가상 트리거 발생하여 Live 모드로 진행
 parm : cam_id	- [in]  Align Camera Index (0x01 or 0x02)
		enable	- [in]  TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Mvenc_ReqWirteTrigLive(UINT8 cam_id, BOOL enable);


/* --------------------------------------------------------------------------------------------- */
/*                             외부 함수 - < Trigger >  < for Common >                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 현재 PLC가 연결되어 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Mvenc_IsConnected();
/*
 desc : Trigger 내의 Strobe이 Enable or Disable 상태 값 반환
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Mvenc_IsStrobEnable();
/*
 desc : Trigger 내의 Strobe이 Enable or Disable 상태 값 반환
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_Mvenc_IsStrobDisable();

#ifdef __cplusplus
}
#endif