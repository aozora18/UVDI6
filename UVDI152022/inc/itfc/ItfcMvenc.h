
/*
 desc : Trigger Communication Libarary
*/

#pragma once

#include "../conf/global.h"
#include "../conf/trig_uvdi15.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : PLC Server와 통신을 담당하는 Client 스레드 생성 및 실행
 parm : config	- [in]  환경 설정 정보
		shmem	- [in]  Trigger Service 관련 공유 메모리
		link	- [in]  통신 연결 공유 메모리
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMvenc_Init(BYTE byPort, LPG_CIEA config, LPG_TPQR shmem, LPG_DLSM link);
/*
 desc : PLC Server와 통신을 담당하는 Client 스레드 생성 및 실행
 parm : config	- [in]  환경 설정 정보
		shmem	- [in]  Trigger Service 관련 공유 메모리
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMvenc_Open(LPG_CIEA config, LPG_TPQR shmem);
/*
 desc : 라이브러리 해제
 parm : None
 retn : None
*/
API_IMPORT VOID uvMvenc_Close();
/*
 desc : 시스템 재접속 수행
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMvenc_Reconnected();
/*
 desc : 현재 PLC가 연결되어 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMvenc_IsConnected();
/*
 desc : 초기 연결되고 난 이후 수신된 데이터가 존재하는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMvenc_IsInitRecv();
#if 1
/*
 desc : Trigger Board에 처음 연결되고 난 이후, 환경 파일에 설정된 값으로 초기화 진행
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMvenc_ReqInitUpdate();
#endif
/*
 desc : 기존 등록된 4개 채널에 대한 Trigger Position 값 초기화 (최대 값) 수행
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMvenc_ResetTrigPosAll();
/*
 desc : 현재 트리거의 위치 등록 값이 초기화 되었는지 여부
 parm : cam_id	- [in]  Align Camera Index (0x01 or 0x02)
		count	- [in]  채널 번호에 해당되는 검사할 트리거 등록 개수 (MAX: 16)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMvenc_IsTrigPosReset(UINT8 cam_id, UINT8 count=MAX_REGIST_TRIG_POS_COUNT);
/*
 desc : 채널 별로 트리거 위치 등록
 parm : direct	- [in]  TRUE: 정방향 (전진), FALSE: 역방향 (후진)
		start	- [in]  트리거 저장 위치 (0 ~ 15)
		count	- [in]  트리거 등록 개수 (1 ~ 16)
		pos		- [in]  트리거 등록 위치가 저장되어 있는 배열 포인터 (unit : 100 nm or 0.1 um)
		enable	- [in]  트리거 Enable or Disable
		clear	- [in]  트리거 위치 등록하면서 현재 Trigger Encoder 값을
						Clear (TRUE)할것인지 혹은 읽기 모드 (FALSE)를 유지할 것인지 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMvenc_ReqWriteAreaTrigPos(BOOL direct,
										   UINT8 start1, UINT8 count1, PINT32 pos1,
										   UINT8 start2, UINT8 count2, PINT32 pos2,
										   ENG_TEED enable, BOOL clear);
/*
 desc : 특정 채널에 트리거 위치 등록
 parm : cam_id	- [in]  Align Camera Index (0x01 or 0x02)
		start	- [in]  트리거 저장 위치 (0 ~ 15)
		count	- [in]  트리거 등록 개수 (1 ~ 16)
		pos		- [in]  트리거 등록 위치가 저장되어 있는 배열 포인터 (unit : 100 nm or 0.1 um)
		enable	- [in]  트리거 Enable or Disable
		clear	- [in]  트리거 위치 등록하면서 현재 Trigger Encoder 값을
						Clear (TRUE)할것인지 혹은 읽기 모드 (FALSE)를 유지할 것인지 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMvenc_ReqWriteAreaTrigPosCh(UINT8 cam_id, UINT8 start, UINT8 count, PINT32 pos,
											 ENG_TEED enable, BOOL clear);
/*
 desc : Trigger & Strobe Enable or Disable
 parm : enable	- [in]  TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMvenc_ReqTriggerStrobe(BOOL enable);
/*
 desc : IP 변경하기
 parm : ip_addr	- [in]  패킷 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMvenc_ReqIP4Addr(PUINT8 ip_addr);
/*
 desc : EEPROM Write
 parm : mode	- [in]  0x01 - Read, 0x02 - Write
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMvenc_ReqEEPROMCtrl(UINT8 mode);
/*
 desc : Board S/W Reset
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMvenc_ReqBoardReset();
/*
 desc : 내부 트리거 설정
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
		lamp_type	- [in]  램프 조명 타입 (0:Ring or 1:Coaxial)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMvenc_ReqEncoderLive(UINT8 cam_id, UINT8 lamp_type);
/*
 desc : 내부 트리거 설정 초기화
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMvenc_ReqEncoderOutReset();
/*
 desc : 트리거 1개 발생 시킴 (Only Trigger Event)
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
		lamp_type	- [in]  램프 조명 타입 (0:Ring or 1:Coaxial)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMvenc_ReqTrigOutOneOnly(UINT8 cam_id, UINT8 lamp_type);

API_IMPORT BOOL uvMvenc_ReqTrigOutOne_(UINT8 channelBit);

/*
 desc : 트리거 1개 발생 시킴 (Trigger Enable -> Trigger Event -> Trigger Disable)
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
		lamp_type	- [in]  램프 조명 타입 (0:Ring or 1:Coaxial)
		enable	- [in]  트리거 내보낸 이후 Disable 시킬지 여부
						트리거 1개 발생 후, 곧바로 트리거 Disable 할지 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMvenc_ReqTrigOutOne(UINT8 cam_id, UINT8 lamp_type, BOOL enable=TRUE);
/*
 desc : 기존에 입력된 Trigger 위치 값과 Trigger Board로부터 수신된 입력 값 비교
 parm : ch_no	- [in]  채널 번호 (0x01 ~ 0x04)
		index	- [in]  트리거 저장 위치 (0x000 ~ 0x0f)
		pos		- [in]  트리거 값
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMvenc_IsTrigPosEqual(UINT8 ch_no, UINT8 index, UINT32 pos);
/*
 desc : 현재 트리거에 송신될 명령 버퍼의 대기 개수가 임의 개수 이하인지 여부
 parm : count	- [in]  이 개수 미만이면 TRUE, 이상이면 FALSE
 retn : count 개수 미만 (미포함)이면 TRUE, 이상 (포함)이면 FALSE 반환
*/
API_IMPORT BOOL uvMvenc_IsSendCmdCountUnder(UINT16 count);


/*
 desc : 카메라 별 가상 트리거 발생하여 Live 모드로 진행
 parm : cam_id	- [in]  Align Camera Index (0x01 or 0x02)
		enable	- [in]  TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE

*/
API_IMPORT BOOL uvMvenc_ReqWirteTrigLive(UINT8 cam_id, BOOL enable);

#ifdef __cplusplus
}
#endif