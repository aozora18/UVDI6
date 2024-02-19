
/*
 desc : The Interface Library for MITSUBISH's MELSEC-Q PLC Equipment
*/

#pragma once

#include "../conf/global.h"
#include "../conf/define.h"
#include "../conf/mcdrv.h"
#include "../conf/plc_addr_podis.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* --------------------------------------------------------------------------------------------- */
/*                         외부 함수 - < PLC - Melsec Q > < for Engine >                         */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : MC Protocol - DWord 데이터 쓰기
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소) (4 Bytes이지만, 3 Bytes 값만 유효)
		value	- [in]  DWord 1개 값이 저장된 버퍼
 retn : TRUE or FALSE (에러 코드가 저장됨 ?)
*/
API_IMPORT BOOL uvEng_MCQ_WriteDWord(UINT32 addr, UINT32 value);
/*
 desc : MC Protocol - Word 데이터 쓰기
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소) (4 Bytes이지만, 3 Bytes 값만 유효)
		value	- [in]  Word 1개 값이 저장된 버퍼
 retn : TRUE or FALSE (에러 코드가 저장됨 ?)
*/
API_IMPORT BOOL uvEng_MCQ_WriteWord(UINT32 addr, UINT16 value);
/*
 desc : MC Protocol - Bits 데이터 쓰기
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소) (4 Bytes이지만, 3 Bytes 값만 유효)
		value	- [in]  Bit 1개 값이 저장된 버퍼
 retn : TRUE or FALSE (에러 코드가 저장됨 ?)
*/
API_IMPORT BOOL uvEng_MCQ_WriteBits(UINT32 addr, UINT8 value);
/*
 desc : MC Protocol - 특정 위치의 Bit 데이터 반전 후 쓰기
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소) (4 Bytes이지만, 3 Bytes 값만 유효)
 retn : TRUE or FALSE (에러 코드가 저장됨 ?)
*/
API_IMPORT BOOL uvEng_MCQ_WriteBitsInvert(UINT32 addr);
/*
 desc : Align Camera Z 축 Homing
 parm : cam_id	- [in]  1 or 2
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_SetACamHomingZAxis(UINT8 cam_id);
/*
 desc : Align Camera Z 축 Reset
 parm : cam_id	- [in]  1 or 2 (if is 0x03, All)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_SetACamResetZAxis(UINT8 cam_id);
/*
 desc : Align Camera Z Axis 이동
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
		method	- [in]  0x00: 절대 이동, 0x01: 상대 이동
		move	- [in]  이동 거리 값 (단위: mm)
						method==0x00 -> 최종 이동할 위치
						method==0x01 -> 현재 위치에서 이동할 거리 값
		up_down	- [in]  0x00:None, 0x01:Up, 0x02:Down 값 (method 값이 0x01일 경우만 이 플래그 유효)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_SetACamMovePosZ(UINT8 cam_id, UINT8 method, DOUBLE move, UINT8 up_down=0x00);
/*
 desc : Align Camera Z Axis 이동 for All
 parm : move	- [in]  절대 이동 위치 값 (단위: 0.1 um or 100 nm)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_SetACamMoveAbsAllZ(PINT32 move);
/*
 desc : Align Camera Z Axis : Move To Home Position
 parm : cam_id	- [in]  얼라인 카메라 ID (1 or Later)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_SetACamZHomePosZ(UINT8 cam_id);
/*
 desc : Align Camera Z Axis : 상태가 준비 모드인지 여부
 parm : cam_id	- [in]  얼라인 카메라 ID (1 or Later)
 retn : TRUE (Ready) or FALSE (Busy)
*/
API_IMPORT BOOL uvCmn_MCQ_IsACamZReady(UINT8 cam_id);
/*
 desc : Align Camera Z Axis : 알람 발생 여부
 parm : cam_id	- [in]  얼라인 카메라 ID (1 or Later)
 retn : TRUE (Alarm) or FALSE (No Alarm)
*/
API_IMPORT BOOL uvCmn_MCQ_IsACamZAlarm(UINT8 cam_id);
/*
 desc : Vacuum 제어 수행
 parm : vacuum	- [in]  0x01: On, 0x00: Off
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_SetVacuumOnOff(UINT8 vacuum);
/*
 desc : Shutter 제어 수행
 parm : shutter	- [in]  0x01: Open, 0x00: Close
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_SetShutterOnOff(UINT8 shutter);
/*
 desc : Vacuum과 Shutter를 동시 제어 수행
 parm : vacuum	- [in]  0x01: On, 0x00: Off
		shutter	- [in]  0x01: Open, 0x00: Close
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_SetVacuumShutterOnOff(UINT8 vacuum, UINT8 shutter);
/*
 desc : Start Lamp / Vacuum / Shutter 제어 
 parm : start_led	- [in]  0x01: On, 0x00: Off
		vacuum		- [in]  0x01: On, 0x00: Off
		shutter		- [in]  0x01: Open, 0x00: Close
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_LedVacuumShutter(UINT8 start_led, UINT8 vacuum, UINT8 shutter);
/*
 desc : Vacuum Controller Run or Stop
 parm : run	- [in]  0x01:Run, 0x00:Stop
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_VacuumRunStop(UINT8 run);
/*
 desc : Chiller Controller Run or Stop
 parm : run	- [in]  0x01:Run, 0x00:Stop
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_ChillerRunStop(UINT8 run);
/*
 desc : Trigger Board Reset (Rebooting)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_TriggerReset();
/*
 desc : Start Lamp (Start 램프 신호 명령으로 사용자에게 노광 진행 중임을 알림)
 parm : enable	- [in]  0x00 - Stop, 0x01 - Start
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_StartLampEnable(UINT8 enable);
/*
 desc : Buzzer 제어 수행
 parm : buzzer	- [in]  0x01: On, 0x00: Off
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_SetBuzzerOnOff(UINT8 buzzer);
/*
 desc : Lamp 제어 수행
 parm : type	- [in]  확인 대상 값 즉, 0x00 : Normal, 0x01 : Warn, 0x02 : Alarm
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_SetLamp(UINT8 type);
/*
 desc : Safety Reset Command (안전 커버 및 EMS가 눌린 경우, 장비 원복 후 RESET 누를 경우 사용)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_SafetyReset();
/*
 desc : Camera Z Axis Position Speed Setting
 parm : value	- [in]  온도 값 (1500 ~ 2500 즉, 15.00 ℃ ~ 25.00 ℃)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_SetHotAirTempSetting(DOUBLE value);

/* --------------------------------------------------------------------------------------------- */
/*                         외부 함수 - < PLC - Melsec Q > < for Common >                         */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 현재 PLC가 연결되어 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MCQ_IsConnected();
/*
 desc : Melsec Q PLC 데이터 정보 반환 (구조체)
 parm : None
 retn : PLC 전체 정보가 저장된 구조체 포인터
*/
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID|| \
	 CUSTOM_CODE_UVDI15 == DELIVERY_PRODUCT_ID)
API_IMPORT LPG_PMRW uvCmn_MCQ_GetShMemStruct();
#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
API_IMPORT LPG_PMDV uvCmn_MCQ_GetShMemStruct();
#endif
/*
 desc : 가장 최근에 발생된 Error Code (Status) 값 반환
 parm : None
 retn : Error Code 값
*/
API_IMPORT UINT16 uvCmn_MCQ_GetLastErrorCode();
/*
 desc : 가장 최근에 발생된 Error Code (Status) 초기화
 parm : None
 retn : None
*/
API_IMPORT VOID uvCmn_MCQ_ResetLastErrorCode();
/*
 desc : MC Protocol - 특정 위치의 Bit 데이터 값 반환
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소) (4 Bytes이지만, 3 Bytes 값만 유효)
 retn : 비트 값 (0 or 1) 반환
*/
API_IMPORT UINT8 uvCmn_MCQ_ReadBitsValue(UINT32 addr);
/*
 desc : MC Protocol - 임의 주소의 Word 데이터 값 반환
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소) (4 Bytes이지만, 3 Bytes 값만 유효)
 retn : Word 값 반환
*/
API_IMPORT UINT16 uvCmn_MCQ_ReadWordValue(UINT32 addr);
/*
 desc : MC Protocol - 메모리에 저장된 Word 데이터 값 반환
 parm : index	- [in]  Word 단위의 메모리의 인덱스 위치 값 (0 or Later)
 retn : Word 값 반환
*/
API_IMPORT UINT16 uvCmn_MCQ_ReadWordMemory(UINT16 index);
/*
 desc : MC Protocol - 임의 주소의 DWord 데이터 값 반환
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소) (4 Bytes이지만, 3 Bytes 값만 유효)
 retn : Word 값 반환
*/
API_IMPORT UINT32 uvCmn_MCQ_ReadDWordValue(UINT32 addr);
/*
 desc : 현재 얼라인 카메라의 Z Axis 높이 값 반환
 parm : cam_id	- [in]  Align Camera Id (Number) (1 or Later)
 retn : Z Axis 높이 값 반환 (단위: mm) (소수점 4자리까지 유효)
*/
API_IMPORT DOUBLE uvCmn_MCQ_GetACamCurrentPosZ(UINT8 cam_id);
/*
 desc : 워크 테이블에 노광 대상 소재가 놓여있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MCQ_IsMaterialDetected();
/*
 desc : 장비 전면에 설치된 Start Button 2개에 불이 들어와 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MCQ_IsStartButtonLedOn();
/*
 desc : 공조기 설정 온도 값 반환 (현재 값이 아님)
 parm : None
 retn : 온도 값 (1500 ~ 2500 즉, 15.00 ℃ ~ 25.00 ℃)
*/
API_IMPORT DOUBLE uvEng_MCQ_GetHotAirTempSetting();
/*
 desc : 공조기 현재 온도 값
 parm : None
 retn : 온도 값 (1500 ~ 2500 즉, 15.00 ℃ ~ 25.00 ℃)
*/
API_IMPORT DOUBLE uvEng_MCQ_GetHotAirTemp();
/*
 desc : DI 내부 온도 값
 parm : index	- [in]  4 곳 중 1 곳 선택 (0x00 ~ 0x03)
 retn : 온도 값 (-200000 ~ +850000 즉, -200.000 ℃ ~ +850.000 ℃)
*/
API_IMPORT DOUBLE uvEng_MCQ_GetTempDI(UINT8 index);
/*
 desc : Buzzer 제어 상태가 On인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_IsSetBuzzerOn();
/*
 desc : Lamp 제어 상태가 Alarm or Warn or Normal 인지 여부 (적색 / 노란색 깜박임인지 혹은 녹색 점등인지 여부)
 parm : type	- [in]  확인 대상 값 즉, 0x00 : Normal, 0x01 : Warn, 0x02 : Alarm
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_IsSetLampOn(UINT8 type);
/*
 desc : 현재 Vacuum Pressure (kPa) 값 반환
 parm : None
 retn : 2 bytes pressure 값 반환 (단위: kPa) (범위: 0 ~ -101 kPa)
*/
API_IMPORT INT16 uvCmn_MCQ_GetVacuumPress();
/*
 desc : 현재 Vacuum Frequency (Hz) 값 반환
 parm : None
 retn : 2 bytes frequency 값 반환 (단위: Hz) (범위: 0 ~ -101 hz)
*/
API_IMPORT INT16 uvCmn_MCQ_GetVacuumFreq();


#ifdef __cplusplus
}
#endif