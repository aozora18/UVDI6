
/*
 desc : LSPA (PreAligner) Library
*/

#pragma once

#include "../conf/conf_gen2i.h"
#include "../conf/milara.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : PreAlinger Library 초기화
 parm : config	- [in]  환경 설정 정보
		shmem	- [in]  Prealigner 관련 공유 메모리
 retn : None
*/
API_IMPORT BOOL uvMPA_Init(LPG_CIEA config, LPG_MPDV shmem);
/*
 desc : PreAlinger Library 해제
 parm : None
 retn : None
*/
API_IMPORT VOID uvMPA_Close();
/*
 desc : 현재 Logosol PreAlinger와 연결되어 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_IsConnected();
/*
 desc : MPA 가 초기화 완료 되었는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_IsInitialized();
/*
 desc : 기존 통신 관련 송/수신 데이터 초기화
 parm : None
 retn : None
*/
API_IMPORT VOID uvMPA_ResetCommData();
/*
 desc : 가장 최근에 송신한 명령어에 대한 응답 상태 값 반환
 parm : None
 retn : 0x00 - FAIL, 0x01 - SUCC, 0xff - Not defined (Not received)
*/
API_IMPORT ENG_MSAS uvMPA_GetLastSendAckStatus();
/*
 desc : 최근에 수신된 명령어 크기 (단위: Bytes)
 parm : None
 retn : 최근에 수신된 명령어 크기 (단위: Bytes)
*/
API_IMPORT UINT32 uvMPA_GetRecvSize();
/*
 desc : 명령어 송신에 대한 응답 모드 값 설정 및 반환
 parm : mode	- [in]  응답 모드 값
						0: 동기 방식 (엄격한 Master-Slave 방식) - 응답이 올 때까지 다른 명령 송신할 수 없음
						   보통 1초 이내에 응답이 올 때, 사용하는 방법
						1: 비동기 방식 (16진수 응답 값 반환)
						2: 비동기 방식 (단지,'>' or '?' 값만 반환)
						3: 비동기 방식 (1과 비슷하지만 숫자가 아닌 문자열 반환 받을 때 사용)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetInfoMode(UINT32 mode);
API_IMPORT BOOL uvMPA_GetInfoMode();
/*
 desc : 모션 축 (T:Rotation, R:Horizontal, Z:Vertical)의 위치 정보 얻기
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetAxisPos();
/*
 desc : 현재 Prealigner의 장비 진단 상태 얻기
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetDiagState();
/*
 desc : 현재 Prealigner의 모션 동작 중에 발생된 에러 상태 얻기
 parm : None
 retn : TRUE or FALSE
 note : 장비의 가장 최근 에러에 대한 상세 정보 즉, 모션 동작 중에 에러가 발생한 경우
*/
API_IMPORT BOOL uvMPA_GetMotionState();
/*
 desc : 현재 Prealigner의 직전 (이전)에 발생된 에러 원인 정보 얻기
 parm : None
 retn : TRUE or FALSE
 note : Prealigner의 동작 중에 에러가 발생 했다면, 발생 원인을 텍스트 형식으로 제공
*/
API_IMPORT BOOL uvMPA_GetErrorReason();
/*
 desc : Prealinger의 모든 축에 대한 상태 (Homing 했는지 즉, Active 여부) 값 반환
 parm : None
 retn : TRUE or FALSE
 note : 각 축이 Homing 할 때, Home과 Limit 쪽 스위치를 인지 했는지 여부
*/
API_IMPORT BOOL uvMPA_GetAllSwitchState();
/*
 desc : Prealigner의 컨트롤러 상태 정보 반환 (코드 값으로 반환)
 parm : None
 retn : TRUE or FALSE
 note : 강제로 값 출력 모드를 INF 0 (1 값이 아님에 주의) 설정 함
*/
API_IMPORT BOOL uvMPA_GetSystemStatus();
/*
 desc : Prealigner내에 동작되는 각 모듈들의 전체 펌웨어 버전 반환
 parm : None
 retn : TRUE or FALSE
 note : 반환 예> MCL Code Version 2.0.1.6
				 System-1.3.2.9 PreAl-1.1.1.37 Calib-1.1.1.18 PA-B1.2.1.8
*/
API_IMPORT BOOL uvMPA_GetVerModuleAll();

/*
 desc : 각 축 (R, Z, T)들의 Home Position 값 반환
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetAxisHomePosition();
/*
 desc : 각 축(R, Z, T) 중 임의 Home Position 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		value	- [in]  Axis에 따른 Home Position 값
						입력 범위는 음수/양수 값 모두 가능 ? (개발사로부터 답변이 옴)
						(unit: R/Z: um, T: degree)
 retn : TRUE or FALSE
 */
API_IMPORT BOOL uvMPA_SetAxisHomePosition(ENG_PANC axis, DOUBLE value);
/*
 desc : 각 축(R, Z, T) 중 현재 위치를 Home Position 값으로 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
 retn : TRUE or FALSE
 */
API_IMPORT BOOL uvMPA_SetAxisCurHomePosition(ENG_PANC axis);
/*
 desc : 각 축 (R, Z, T)들의 전류 제한 (레벨) 값 반환
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetAxisCurrentLimit();
/*
 desc : 각 축(R, Z, T) 중 임의 전류 제한 (레벨) 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		value	- [in]  Axis에 따른 Current Limit 값
 retn : TRUE or FALSE
 */
API_IMPORT BOOL uvMPA_SetAxisCurrentLimit(ENG_PANC axis, UINT8 value);
/*
 desc : 각 축 (R, Z, T)들의 Position Limit Error 값 반환
 parm : None
 retn : TRUE or FALSE
 note : Motor (Servo)가 가지고 있는 민감도에 따라, 다르게 설정되어 있음
		특정 환경마다 설치된 Servo Motor의 Tuning 계수를 설정해야 됨
		Motor 마다 설정된 이 에러 값 기준으로, 이동할 때마다 Motor가 벗어나면 자동으로 멈추게 됨
*/
API_IMPORT BOOL uvMPA_GetAxisPositionLimitError();
/*
 desc : 각 축(R, Z, T) 중 Position Limit Error 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		value	- [in]  Axis에 따른 Position Limit Error 값 (1 ~ 65535)
						Encoder Counts 값 (주위! um 값이 아님!)
 retn : TRUE or FALSE
 note : 설정된 값 보다, Motor가 그 이상으로 동작 했다면, Prealinger 내부의 펌웨어서 자동 멈춤
 */
API_IMPORT BOOL uvMPA_SetAxisPositionLimitError(ENG_PANC axis, UINT16 value);
/*
 desc : 각 축 (R, Z, T)들의 Forward or Reversion Direction에 대한 Limit 값 반환
 parm : direct	- [in]  Axis Direction (축의 이동 방향 즉, 정방향 or 역방향)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetAxisDirectLimit(ENG_MAMD direct);
/*
 desc : 각 축 (R, Z, T)들의 Forward or Reversion Direction에 대한 Limit 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		direct	- [in]  Axis Direction (축의 이동 방향 즉, 정방향 or 역방향)
		value	- [in]  Axis에 따른 Home Position 값
						입력 범위는 음수/양수 값 모두 가능 ? (개발사로부터 답변이 옴)
						(unit: R/Z: um, T: degree)
 retn : TRUE or FALSE
 */
API_IMPORT BOOL uvMPA_SetAxisDirectLimit(ENG_PANC axis, ENG_MAMD direct, DOUBLE value);
/*
 desc : 각 축 (R, Z, T)들의 Home Offset 값 반환
 parm : None
 retn : TRUE or FALSE
 note : 설정된 Home Position으로 Homing 했을 때, 실제 이동한 위치 값이 0이 아닌 경우,
		0 으로 바뀌기 위한 오차 값
*/
API_IMPORT BOOL uvMPA_GetAxisHomeOffset();
/*
 desc : 각 축(R, Z, T) 중 임의 Home Offset 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		value	- [in]  Axis에 따른 Home Position 값
						입력 범위는 음수/양수 값 모두 가능 ? (개발사로부터 답변이 옴)
						(unit: R/Z: um, T: degree)
 retn : TRUE or FALSE
 note : 개발 쪽에서는 거의 사용 안함. (생산자 쪽에서 사용하는 명령어)
 */
API_IMPORT BOOL uvMPA_SetAxisHomeOffset(ENG_PANC axis, DOUBLE value);
/*
 desc : 각 축(R, Z, T) 중 현재 위치를 Home Offset 값으로 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
 retn : TRUE or FALSE
 */
API_IMPORT BOOL uvMPA_SetAxisCurHomeOffset(ENG_PANC axis);
/*
 desc : 각 축 (R, Z, T)들의 Acceleration or Deceleration 값 반환
 parm : type	- [in]  0x00: Acceleration, 0x01: Deceleration
 retn : TRUE or FALSE
 note : 각 축의 순간 속력 (가속력 혹은 감속력) 값을 초당 얼마만큼 설정되어 있는지 반환
*/
API_IMPORT BOOL uvMPA_GetAxisInstSpeed(ENG_MAST type);
/*
 desc : 각 축 (R, Z, T)들의 Acceleration or Deceleration 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		type	- [in]  0x00: Acceleration, 0x01: Deceleration
		value	- [in]  Axis에 따른 Acceleration or Decleration 값
						(unit: R/Z: um / sec^2, T: degree / sec^2)
 retn : TRUE or FALSE
 */
API_IMPORT BOOL uvMPA_SetAxisInstSpeed(ENG_PANC axis, ENG_MAST type, DOUBLE value);
/*
 desc : 각 축 (R, Z, T)들의 Acceleration or Deceleration for JERK (가가속도 or 가감속도) 값 반환
 parm : type	- [in]  0x00: Acceleration, 0x01: Deceleration
 retn : TRUE or FALSE
 note : 각 축의 순간 속력 (가가속력 혹은 가감속력) 값을 초당 얼마만큼 설정되어 있는지 반환
		!!! Peraligner의 성능에는 영향을 주지 않음 !!!
*/
API_IMPORT BOOL uvMPA_GetAxisJerkSpeed(ENG_MAST type);
/*
 desc : 각 축 (R, Z, T)들의 Acceleration or Deceleration for JERK (가가속도 or 가감속도) 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		type	- [in]  0x00: Acceleration, 0x01: Deceleration
		value	- [in]  Axis에 따른 Acceleration or Decleration for JERK 값
						(unit: R/Z: um / sec^2, T: degree / sec^2)
 retn : TRUE or FALSE
 note : !!! Peraligner의 성능에는 영향을 주지 않음 !!!
 */
API_IMPORT BOOL uvMPA_SetAxisJerkSpeed(ENG_PANC axis, ENG_MAST type, DOUBLE value);
/*
 desc : 각 축 (R, Z, T)들의 Velocity Profile 값 반환
 parm : None
 retn : TRUE or FALSE
 note : 0 : Trapezoidal velocity profile	(Web Search)
		1 : Bezier curve velocity profile	(Web Search)
		2 : Spline curve velocity profile	(Web Search)
		3 : S-Curved velocity profile		(Web Search)
		9 : Device velocity profile			(Web Search)
*/
API_IMPORT BOOL uvMPA_GetVelocityProfile();
/*
 desc : 각 축 (R, Z, T)들의 Velocity Profile 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		profile	- [in]  Profile Value (0, 1, 2, 3, 9 중 1개)
 retn : TRUE or FALSE
 note : 0 : Trapezoidal velocity profile	(Web Search)
		1 : Bezier curve velocity profile	(Web Search)
		2 : Spline curve velocity profile	(Web Search)
		3 : S-Curved velocity profile		(Web Search)
		9 : Device velocity profile			(Web Search)
*/
API_IMPORT BOOL uvMPA_SetVelocityProfile(ENG_PANC axis, UINT8 profile);
/*
 desc : 각 축 (R, Z, T)들의 Speed 값 반환
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetAxisSpeed();
/*
 desc : 각 축 (R, Z, T)들의 Speed 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		speed	- [in]  Speed Value
						(unit: R/Z: um / sec^2, T: degree / sec^2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetAxisSpeed(ENG_PANC axis, DOUBLE speed);
/*
 desc : Prealigner의 모든 축들에 대해 초기화 (Homing...)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetAxisAllHoming();
/*
 desc : 각 축 (R, Z, T)들의 Speed 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_MoveHomePosition(ENG_PANC axis);
/*
 desc : 각 축 (R, Z, T)들의 위치를 이동 (Move to Position)
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		type	- [in]  절대 이동 (0x00)인지, 상대 이동 (0x01)인지 여부
		value	- [in]  Axis에 따른 이동 거리 값 (상대 이동일 경우, 음수도 입력 가능?)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_MoveToAxis(ENG_PANC axis, ENG_PAMT type, DOUBLE value);
/*
 desc : 각 축 (R, Z, T)들의 Servo Control ON or OFF
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		type	- [in]  Control Operation 즉, OFF (0x00) or ON (0x01)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_ServoControl(ENG_PANC axis, ENG_PSOO type);
/*
 desc : 임의 (선택된) 축 혹은 전체 축에 대해서 동작을 중지 시킴
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical)), 3: All
 retn : TRUE or FALSE
 note : 현재 실행 중인 Macros에 대해서도 중지 (취소) 시킴
*/
API_IMPORT BOOL uvMPA_StopToAxis(ENG_PANC axis);
/*
 desc : Prealigner에 제출된 모든 명령 (Macros)들과 작업 (Procedure)들의 실행(동작)이 완료될
		때까지 대기하고 특정 (임의) 축의 이동은 중지 시킴
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
 retn : TRUE or FALSE
 참고 :	가령 WMC Axis1: Axis1의 이동이 멈출 때까지 대기하고, 모든 Macros이 실행이 완료 됨
*/
API_IMPORT BOOL uvMPA_WaitPrealigner(ENG_PANC axis);
/*
 desc : Chuck으로부터 Wafer를 제거하기 위해 Chuck Vacuum을 Close (OFF) or Open (ON) 수행
 parm : type	- [in]  Control Operation 즉, OFF (0x00) or ON (0x01)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_ChuckVacuum(ENG_PSOO type);
/*
 desc : Pins으로부터 Wafer를 제거하기 위해 Pins Vacuum을 Close (OFF) or Open (ON) 수행
 parm : type	- [in]  Control Operation 즉, OFF (0x00) or ON (0x01)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_PinsVacuum(ENG_PSOO type);
/*
 desc : Wafer Align (정렬) 수행 : Notch 검색
 parm : bal_mode	- [in]  Balance Mode (0, 2 ~ 5, if 1 값인 경우는 해당 사항 없음)
		max_offset	- [in]  Alignment Offset (unit: um)
		offset_try	- [in]  Offset Retry Count
		max_angle	- [in]  Notch Angle (unit: degree)
		angle_try	- [in]  Angle Retry Count
		orient_notch- [in]  Orient Notch 적용 여부 (0 or 1)
							if 1, Notch가 항상 Chuck에서 동일한 방식으로 배치되도록 Pin 아래 Chuck 회전
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_FindNotchAngle(DOUBLE max_offset=0.0f, UINT8 offset_try=0.0f,
									 DOUBLE max_angle=0.0f, UINT8 angle_try=0.0f, UINT8 orient_notch=0.0f);
API_IMPORT BOOL uvMPA_FindNotchAngleExt(UINT8 mode);
/*
 desc : CCD Sensor (Image Sensor)로부터 수집 (Grab)된 이미지의 Pixel 개수 반환
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetCCDSensorPixels();
/*
 desc : CCD Sensor (Image Sensor)로부터 수집 (Grab)된 이미지의 Pixel 개수 설정
 parm : rate	- [in]  CCD Sensor가 Sampling 할 때, 읽어들 일 (캡처;Grab) 이미지의 개수
						기본 값은 10으로 설정되어 있음
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetCCDSensorPixels(UINT16 value);
/*
 desc : Wafer가 놓여 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_CheckWaferLoaded();
/*
 desc : Wafer Alignment 검색 성공 후 Wafer 회전을 얼마만큼 진행할지 회전 각도 값 설정
 parm : angle	- [in]  Rotation Angle (degree)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetRotationAfterAlign(DOUBLE angle);
/*
 desc : Wafer Alignment 검색 후 Wafer 회전을 얼마만큼 진행할지 회전 각도 값 반환
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetRotationAfterAlign();
/*
 desc : Wafer가 Prealigner에 놓여질 유형 (장소) 설정
 parm : type	- [in]  PINS (0x00), CHCUK(0x01)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetWaferPlaceType(ENG_PWPT type);
/*
 desc : Wafer가 Prealigner에 놓여질 유형 (장소) 값 반환
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetWaferPlaceType();
/*
 desc : 현재 Prealigner가 Wafer Alignment 성공 혹은 실패한 개수 얻기
 parm : type	- [in]  fail (0x00), succ (0x01)
 retn : TRUE or FALSE
 note : 이 함수 호출하기에 앞서, uvMPA_FindNotchAngle(...) 호출이 선행되어야 함
*/
API_IMPORT BOOL uvMPA_GetWaferNotchCount(ENG_PASR type);
/*
 desc : 현재 Prealigner가 Wafer Alignment 검색 후 성공 및 실패에 개수 통틀어 각각의 깊이와 폭 값 요청
 parm : index	- [in]  Range (0 ~ Total: -1)
 retn : TRUE or FALSE
 note : 주의!!! 앞 부분에는 검색 (BAL) 후 Good 정보 (깊이/폭), 뒷 부분에는 Bad 정보 (깊이/폭)
		이 함수 호출하기에 앞서, uvMPA_GetWaferNotchCount(...) 호출이 선행되어야 함
*/
API_IMPORT BOOL uvMPA_GetWaferAlignDepthWidth(INT16 index);
/*
 desc : Prealigner의 Notch 유형 (Depth, Width) 별 검색 조건 (Min & Max) 값 얻기
 parm : type	- [in] 0x00: Depth, 0x01: Width
		kind	- [in] 0x00: Min, 0x01: Max
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetWaferFindNotchRange(ENG_PNTI type, UINT8 kind);
/*
 desc : Prealigner의 Notch 유형 (Depth, Width) 별 검색 조건 (Min & Max) 값 얻기
 parm : type	- [in] 0x00: Depth, 0x01: Width
		kind	- [in] 0x00: Min, 0x01: Max
		range	- [in] Range Value 값
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetWaferFindNotchRange(ENG_PNTI type, UINT8 kind, UINT32 range);
/*
 desc : CCD Sampling의 개수(장수) 얻기 (이 값에 따라, Notch 검색하는데 소요되는 시간 결정?)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetMinCCDSampleCount();
/*
 desc : 최소 CCD Sampling의 개수(장수) 설정 (GOOD 판정한데 필요한 최소 Sample 개수)
 parm : samples	- [in]  Notch 검색하는데 필요한 CCD Sampling 개수 (10 ~ 2500)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetMinCCDSampleCount(UINT16 samples);
/*
 desc : 현재 Prealinger의 작업 가능한 Wafer Size 설정
 parm : type	- [in]  Min / Max 구분 값
		kind	- [in]  Wafer Size의 구분 (종류) 값 (2, 3, 4, 5, 6, 8, 12, 18 중 1개 값)
						50mm(2″), 75mm(3″), 100mm(4″), 125mm(5″), 150mm(6″),
						200mm(8″), 300mm(12″), 450mm(18″)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetWaferMinMaxSize(ENG_MMMR type, ENG_MWKT kind);
/*
 desc : 현재 Prealinger에 설정된 작업 가능한 Wafer Size 얻기
 parm : type	- [in]  Min / Max 구분 값
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetWaferMinMaxSize(ENG_MMMR type);
/*
 desc : 현재 Prealinger에서 작업하려는 Wafer Size 설정
 parm : kind	- [in]  Wafer Size의 구분 (종류) 값 (2, 3, 4, 5, 6, 8, 12, 18 중 1개 값)
						50mm(2″), 75mm(3″), 100mm(4″), 125mm(5″), 150mm(6″),
						200mm(8″), 300mm(12″), 450mm(18″)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetWaferWorkSize(ENG_MWKT kind);
/*
 desc : 현재 Prealinger에서 작업하려는 Wafer Size 얻기
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetWaferWorkSize();
/*
 desc : 현재 Prealinger에서 Wafer Notch 인식하기 위한 Light Source 설정
 parm : light	- [in]  0: 첫 번째 Light Source, 1: 두 번째 Light Source
 retn : TRUE or FALSE
 note : Some prealigners have two different light sources per wafer size.
		For example,
			LPA1218 has two light sources for the 12 inch sensor – for 300 and 310 mm and
			two light sources for the 18 inch sensor – for 450 and 460 mm wafers.
			This command sets or reports the selected light source.
*/
API_IMPORT BOOL uvMPA_SetWaferWorkLight(UINT8 light);
/*
 desc : 현재 Prealinger에서 Wafer Notch 인식하기 위한 Light Source 반환
 parm : None
 retn : TRUE or FALSE
 note : Some prealigners have two different light sources per wafer size.
		For example,
			LPA1218 has two light sources for the 12 inch sensor – for 300 and 310 mm and
			two light sources for the 18 inch sensor – for 450 and 460 mm wafers.
			This command sets or reports the selected light source.
*/
API_IMPORT BOOL uvMPA_GetWaferWorkLight();
/*
 desc : Wafer의 중심 위치와 방향 정보 등을 반환
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetWaferCenterOrientation();
/*
 desc : Wafer Size를 자동으로 감지할지 여부 설정 (내부적으로 Wafer Size를 설정하지 않은 경우 사용)
 parm : type	- [in]  0x00: Auto Detection 사용 안함, 0x01: Wafer Size를 자동으로 감지하는 모드 사용
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetAutoDetectWaferSize(ENG_PWSD type);
/*
 desc : Wafer Size를 자동으로 감지 여부 값 얻기
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetAutoDetectWaferSize();
/*
 desc : 정렬 작업 (명령) 즉, Wafer Notch 검색이 실패한 경우,
		Wafer Edge의 추가적인 검사를 할지 여부 설정
 parm : type	- [in]  Auto Scan Again 관련 플래그 동작 활성화 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetAutoScanAgain(ENG_MFED type);
/*
 desc : 정렬 작업 (명령) 즉, Wafer Notch 검색이 실패한 경우,
		Wafer Edge의 추가적인 검사를 할지 여부 값 얻기
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetAutoScanAgain();
/*
 desc : "BAL" 명령 (정렬 작업) 완료 후 Chuck 위에 Wafer의 Vacuum 상태를 On으로 할지 Off로 할지 설정
 parm : type	- [in]  Vacuum On (0x01) or Off (0x00)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetVacuumSetAfterBAL(ENG_PSOO type);
/*
 desc : "BAL" 명령 (정렬 작업) 완료 후 Chuck 위에 Wafer의 Vacuum 상태 설정 값 반환
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetVacuumSetAfterBAL();
/*
 desc : Wafer의 재질이 투명한지 여부 설정
 parm : type	- [in]  Transparent (0x01) or Non-transparent (0x00)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetWaferGlassType(ENG_PWGO type);
/*
 desc : Wafer의 재질이 투명한지 여부 값 얻기
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetWaferGlassType();
/*
 desc : Prealigner에 Wafer가 놓여질 때 또는 Wafer를 가져갈 때 (가져 올 때), 위치 값 얻기
 parm : type	- [in] 0x00: Loading Down Position, 0x01: Loading Up Position
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetWaferLoadPosition(ENG_PWLP type);
/*
 desc : Prealigner에 Wafer가 놓여질 때 또는 Wafer를 가져갈 때 (가져 올 때), 위치 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		type	- [in] 0x00: Loading Down Position, 0x01: Loading Up Position
		pos		- [in] Position 값 (unit: um)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetWaferLoadPosition(ENG_PANC axis, ENG_PWLP type, DOUBLE pos);
/*
 desc : 측정 단계 동안 T (Rotation) 축, Chuck 및 Pins 사이의 Wafer 이동 동안
		R (Horzontal)과 Z (Vertical) 축의 가속도 / 감속도 값 얻기
 parm : type	- [in] 0x00: 가속도 (Acceleration), 0x01: 감속도 (Deceleration)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetMeasureAccDcl(ENG_MAST type);
/*
 desc : 측정 단계 동안 T (Rotation) 축, Chuck 및 Pins 사이의 Wafer 이동 동안
		R (Horzontal)과 Z (Vertical) 축의 가속도 / 감속도 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		type	- [in]  0x00: Loading Down Position, 0x01: Loading Up Position
		value	- [in]  이동하는데 필요한 각도의 가속도 혹은 이동하는데 가속도 값 등
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetMeasureAccDcl(ENG_PANC axis, ENG_MAST type, DOUBLE value);
/*
 desc : 측정 단계 동안 T (Rotation) 축, Chuck 및 Pins 사이의 Wafer 이동 동안
		R (Horzontal)과 Z (Vertical) 축의 속도 값 얻기
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetMeasureSpeedAxis();
/*
 desc : 측정 단계 동안 T (Rotation) 축, Chuck 및 Pins 사이의 Wafer 이동 동안
		R (Horzontal)과 Z (Vertical) 축의 속도 값 설정
 parm : axis	- [in]  Axis Name (0:T (Angle), 1:R (Horizontal), 2:Z (Vertical))
		value	- [in]  이동하는데 필요한 각도의 가속도 혹은 이동하는데 가속도 값 등
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetMeasureSpeedAxis(ENG_PANC axis, DOUBLE value);
/*
 desc : Notch의 Depth 해상도의 모드 값 얻기
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetNotchDepthMode();
/*
 desc : Notch의 Depth 해상도의 모드 값 설정
 parm : enable	- [in] Notch Depth 해상도를 표준 (기본) 값보다 더 높이 할것인지 여부
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetNotchDepthMode(bool enable);
/*
 desc : Wafer Alignment 동작될 때, Prealinger의 축(T/R/Z)의 위치 이동 (Measureing Position)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetWaferAlignmentMeasuringPos();
/*
 desc : Wafer Alignment 동작될 때, Prealinger의 축(T/R/Z)의 위치를 반환 (Loading Position)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetWaferAlignmentLoadingPos();
/*
 desc : MTP 명령 이후에 Z (Veritcal)축의 최종 위치 모드 값 얻기
 parm : None
 retn : TRUE or FALSE
 note : MTP Command: Wafer를 Pin으로 이동 시작하라는 명령어
*/
API_IMPORT BOOL uvMPA_GetModeZAfterMovePins();
/*
 desc : MTP 명령 이후에 Z (Veritcal)축의 최종 위치 모드 값 얻기
 parm : mode	- [in]  _LD (Load Down Position; 0), _LU (Load Up Position; 1)
 retn : TRUE or FALSE
 note : MTP Command: Wafer를 Pin으로 이동 시작하라는 명령어
		_LD Command: Wafer가 올려지는 Pins의 Load Down Position
		_LU Command: Wafer가 올려지는 Pins의 Load Up Position
*/
API_IMPORT BOOL uvMPA_SetModeZAfterMovePins(ENG_PWLP mode);
/*
 desc : Chuck과 Pins의 수평인 Z (Vertical) 축의 위치 (Position) 값 얻기
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetLevelChcukToPinsPosZ();
/*
 desc : Chuck과 Pins의 수평인 Z (Vertical) 축의 위치 (Position) 값 얻기
 parm : value	- [in]  Z (Vertical) 축 높이 값 (단위: um)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetLevelChcukToPinsPosZ(DOUBLE value);
/*
 desc : Wafer가 Chuck에서 Pins로 이송될 때, Up/Down 오차 (Backlash)를 허용하는 Offset 값 얻기
 parm : type	- [in]  0x00: Down Postion Offset, 0x01: Up Position Offset
 retn : TRUE or FALSE
 note : 얇게 휘어진 Wafer를 Chuck To Pins으로의 이송에 대한 신뢰성 개선을 위한 Offset 값
		Up에서 Down / Up으로 Pins이 이동할 때, 발생할 수 있는 오차 (Backlash? 현상) 부분을
		허용하기 위한 Offset (오차) 값
*/
API_IMPORT BOOL uvMPA_GetOffsetChcukToPinsPosZ(ENG_PWLP type);
/*
 desc : Wafer가 Chuck에서 Pins로 이송될 때, Up/Down 오차 (Backlash)를 허용하는 Offset 값 설정
 parm : type	- [in]  0x00: Down Postion Offset, 0x01: Up Position Offset
		value	- [in]  Z (Vertical) 축 Offset 값 (단위: um)
						Pins Position offset when moving down
 retn : TRUE or FALSE
 note : 얇게 휘어진 Wafer를 Chuck To Pins으로의 이송에 대한 신뢰성 개선을 위한 Offset 값
		Up에서 Down / Up으로 Pins이 이동할 때, 발생할 수 있는 오차 (Backlash? 현상) 부분을
		허용하기 위한 Offset (오차) 값
*/
API_IMPORT BOOL uvMPA_SetOffsetChcukToPinsPosZ(ENG_PWLP type, DOUBLE value);
/*
 desc : CCD Sensor와 R (Horizontal) 축 간의 각도 값 출력
 parm : None
 retn : TRUE or FALSE
 note : 주의!!! Wafer Size를 고려하지 않음
*/
API_IMPORT BOOL uvMPA_GetAngleRAxisToCCD();
/*
 desc : CCD Sensor와 R (Horizontal) 축 간의 각도 값 설정
 parm : value	- [in]  R (horizontal) 축과 CCD Sensor 사이의 각도 (degrees)
 retn : TRUE or FALSE
 note : 주의!!! Wafer Size를 고려하지 않음
*/
API_IMPORT BOOL uvMPA_SetAngleRAxisToCCD(DOUBLE value);
/*
 desc : 현재 샘플 평균화 모드를 출력
		만약 모드 값이 3 or 5로 설정되어 있다면, 처음 측정된 Notch/Flat에 대해,
		차후 (이후부터) 검색되는 나머지 Wafer들의 Notch/Flat를 검색하는 기준 (평균)이 됨
 parm : None
 retn : TRUE or FALSE
 note : 이 명령을 사용하면, 임의 Wafer에 대한 Notch/flag 등을 Sampling 후에 평균치로 저장
		나머지 Wafer들을 검색할 때는 다시 모드 값을 3 혹은 5 이외의 값으로 설정해 놓아야 됨
*/
API_IMPORT BOOL uvMPA_GetNotchAverageMode();
/*
 desc : 현재 샘플 평균화 모드를 설정
		만약 모드 값이 3 or 5로 설정되어 있다면, 처음 측정된 Notch/Flat에 대해,
		차후 (이후부터) 검색되는 나머지 Wafer들의 Notch/Flat를 검색하는 기준 (평균)이 됨
 parm : enable	- [in]  TRUE: 처음 측정되는 Wafer의 Notch/flag을 평균화 값으로 적용
						FALSE: 평균화 값 적용 안함
 retn : TRUE or FALSE
 note : 이 명령을 사용하면, 임의 Wafer에 대한 Notch/flag 등을 Sampling 후에 평균치로 저장
		나머지 Wafer들을 검색할 때는 다시 모드 값을 3 혹은 5 이외의 값으로 설정해 놓아야 됨
*/
API_IMPORT BOOL uvMPA_SetNotchAverageMode(BOOL enable);
/*
 desc : CCD Sensor의 중심 설정 혹은 출력
		Wafer 크기 기준으로 Wafer Notch / flat 부분에 해당되는 위치 (단위: pixel) (개인적인 판단)
 parm : None
 retn : TRUE or FALSE
 note : Wafer Size는 고려 대상이 아님 (Philoptics Library Part)
*/
API_IMPORT BOOL uvMPA_GetCCDSensorCenter();
/*
 desc : CCD Sensor의 중심 설정 혹은 출력
		Wafer 크기 기준으로 Wafer Notch / flat 부분에 해당되는 위치 (단위: pixel) (개인적인 판단)
 parm : pixel	- [in]  Wafer Size 기준으로 Wafer Notch or Flat 부분의 위치 값 (단위: pixel)
 retn : TRUE or FALSE
 note : Wafer Size는 고려 대상이 아님 (Philoptics Library Part)
*/
API_IMPORT BOOL uvMPA_SetCCDSensorCenter(UINT32 pixel);
/*
 desc : Sampling Mode의 값을 출력
		즉, 이 모드는 회전 당 샘플링 된 샘플 수와 검사 속도 (“_MS”)를 설정하고
		“_MS”  명령어에 의해 반환된 값을 내부적으로 설정
		(즉, “_MS” 명령에 의해 설정된 속도 값을 기준으로 모드 값에 따라 다르게 동작 시킴)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetRotationSampleMode();
/*
 desc : Sampling Mode의 값을 설정
		즉, 이 모드는 회전 당 샘플링 된 샘플 수와 검사 속도 (“_MS”)를 설정하고
		“_MS”  명령어에 의해 반환된 값을 내부적으로 설정
		(즉, “_MS” 명령에 의해 설정된 속도 값을 기준으로 모드 값에 따라 다르게 동작 시킴)
 parm : mode	- [in]  Sampling Mode 값 (0, 1, 2, 3)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetRotationSampleMode(ENG_PRSM mode);
/*
 desc : Wafer 이송 중에 Chuck 모션을 Pins Position 지점에서 멈출게 할지 여부 제어 값 반환
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetStopOnPins();
/*
 desc : Wafer 이송 중에 Chuck 모션을 Pins Position 지점에서 멈출게 할지 여부 제어 값 설정
 parm : mode	- [in]  Sampling Mode 값 (0, 1)
						1: Chuck에서 Pins로 Wafer 이송 중에 Pins Position에서 멈추고,
						   다시 돌아갑니다. 이 Pins position에서, Chuck Vacuum이 Off되기 전에
						   Pins Vacuum은 On이 되어짐 (현재 수행단계까지 진행 후 멈춤?)
						0: Chuck은 Pins Position에서 정지하지 않고, Pins Vacuum이 On되기 전에
						   Chuck Vacuum은 Off 되어짐 (바로 멈춤?)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetStopOnPins(UINT8 mode);
/*
 desc : 펌웨어에서 Substrate의 가장자리 코너 부분으로부터 가져올 수 있는 CCD Data의
		최소 / 최대 값을 출력
 parm : mode	- [in]  0x00: Min, 0x01: Max
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetSuqreValueOfCCD(ENG_MMMR mode);
/*
 desc : 펌웨어에서 Substrate의 가장자리 코너 부분으로부터 가져올 수 있는 CCD Data의
		최소 / 최대 값을 설정
 parm : mode	- [in]  0x00: Min, 0x01: Max
		area	- [in]  가져올 영역의 크기 (Square 즉, 가로/세로 동일한 크기) (단위: pixels)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetSuqreValueOfCCD(ENG_MMMR mode, UINT32 area);
/*
 desc : Chuck이 Pins에 방해받지 않도록 R (Horizontal) 축을 이동하는데,
		안전한 지점인 Pins 아래쪽 위치를 출력
		또는
		Chuck의 Wafer가 Pins 위에 있도록 R (Horizontal) 축을 이동하는데,
		안전한 지점인 Pins 위쪽 위치를 출력 (Wafer를 가지고 가기 위한 안전한 지점)
 parm : mode	- [in]  0x00: Transfer Down, 0x01: Transfer Up
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetWaferTranserSafePos(ENG_PWLP mode);
/*
 desc : Chuck이 Pins에 방해받지 않도록 R (Horizontal) 축을 이동하는데,
		안전한 지점인 Pins 아래쪽 위치를 설정
		또는
		Chuck의 Wafer가 Pins 위에 있도록 R (Horizontal) 축을 이동하는데,
		안전한 지점인 Pins 위쪽 위치를 설정 (Wafer를 가지고 가기 위한 안전한 지점)
 parm : mode	- [in]  0x00: Transfer Down, 0x01: Transfer Up
		value	- [in]  Chuck의 위치 값 (단위: um)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetWaferTranserSafePos(ENG_PWLP mode, DOUBLE value);
/*
 desc : Prealigner 동작 관련된 여타 다른 부분들의 동작 타임 아웃 값을 출력
 parm : param	- [in]  Parameter Number (0 ~ 10) (ENUM 형으로 정의하지 못했음. 넘 난해 함)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetParamTimeOut(UINT8 param);
/*
 desc : Prealigner 동작 관련된 여타 다른 부분들의 동작 타임 아웃 값을 설정
 parm : param	- [in]  Parameter Number (0 ~ 10) (ENUM 형으로 정의하지 못했음. 넘 난해 함)
		value	- [in]  Timeout or Delay 값 (msec)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetParamTimeOut(UINT8 param, UINT32 value);

/*
 desc : 선택한 웨이퍼 크기와 함께 자동으로 사용할 매개 변수 집합 할당 (설정; Assigned)
		Wafer 크기가 변경될 때마다 기존 할당된 세트의 파라미터들이 작업 파라미터로 적재 됨
 parm : set		- [in]  Set Number (-1 or 1 or Later)
		wafer	- [in]  Wafer Size (2, 3, 4, 5, 6, 8, 12, 18)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetAssingAutoLoadWafer(INT8 set, ENG_MWKT wafer);
/*
 desc : Prealigner 동작 관련된 여타 다른 부분들의 동작 타임 아웃 값을 출력
 parm : param	- [in]  Parameter Number (0 ~ 10) (ENUM 형으로 정의하지 못했음. 넘 난해 함)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetWaferType();
/*
 desc : Prealigner 동작 관련된 여타 다른 부분들의 동작 타임 아웃 값을 설정
 parm : type	- [in]  Wafer Type (-2, -1, 0, 1, 2)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetWaferType(ENG_PGWT type);
/*
 desc : Prealigner에 저장되어 있는 "Performance Set"을 파일 ("perform.ini")에서 읽어들인 후
		기존 "Performance Set"에 덮어쓰고, 활성화 시킴
 parm : set_no	- [in]  Performance Set Number
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_RestorePerformSetFile(UINT8 set_no);
/*
 desc : 현재 Prealigner에 설정된 모든 파라미터들의 설정된 값을 파일 ("paramset.ini")에 저장
		기존 파일에 덮어쓰는 구조 임
 parm : set_no	- [in]  Performance Set Number
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_WritePerformSetFile(UINT8 set_no);
/*
 desc : 이전 Prealigner에 저장된 모든 파라미터들의 설정된 값을 파일 ("paramset.ini")에서 불러옴
		기존 설정된 파라미터들의 값은 모두 사라짐
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_RestoreParamSetFile();
/*
 desc : 현재 Prealigner에 설정된 모든 파라미터들의 설정된 값을 파일 ("paramset.ini")에 저장
		기존 파일에 덮어쓰는 구조 임
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SaveParamSetFile();
/*
 desc : 이전 Prealigner에 저장된 모든 파라미터들의 설정된 값을 비메모리 ("Non-volatile") 영역에서 불러옴
		기존 설정된 파라미터들의 값은 모두 사라짐
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_RestoreParamSetMemory();
/*
 desc : 현재 Prealigner에 설정된 모든 파라미터들의 설정된 값을 비메모리 ("Non-volatile") 영역에 저장
		기존에 저장된 정보는 사라짐 (덮어써 버림)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SaveParamSetMemory();
/*
 desc : 현재 Prealigner의 시스템 시간 얻기
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetSystemTime();
/*
 desc : 현재 Prealigner의 시스템 시간 설정 (현재 시간 설정)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetSystemTime();
/*
 desc : 원점 복귀 궤적 매개 변수를 모든 축에 할당 즉, 속도, 가속 및 감속도 설정
 parm : mode	- [in]  0x00:HSA (Homing Mode), 0x01:WSA (Working Mode)
 retn : TRUE or FALSE
 note : Homing 할 때, 적용됨 (?: QnA 보냈지만, 답변 없음)
		현재 각 축들의 속도 관련 설정된 정보 값을 그대로 반영
		(SPD 명령을 통해서 현재 각 축들의 속도 정보 확인할 수 있음)
*/
API_IMPORT BOOL uvMPA_RestoreTrajectorySpeedAcc(UINT8 mode);
/*
 desc : 현재 Prealigner의 장비를 초기화 (Reset; Turn off -> Turn on)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_ResetPrealigner();
/*
 desc : Prealigner가 정렬 (BAL Command) 작업이 실패한 경우, CCD Data를 포함하고 있는 파일을
		자동으로 생성할지 여부 값 얻기
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetAlignFailAutoDumps();
/*
 desc : Prealigner가 정렬 (BAL Command) 작업이 실패한 경우, CCD Data를 포함하고 있는 파일을
		자동으로 생성할지 여부 값 얻기
 parm : mode	- [in]  0x00: 자동 생성 안함, 0x01: 자동 생성 함
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetAlignFailAutoDumps(UINT8 mode);
/*
 desc : “_AUTODUMPS” 모드 값이 “1”로 설정된 경우, 유효한 명령어
		생성하고자 하는 CCD Samples 파일에 대한 File Numbering 값 반환
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetAlignFailAutoDumpFile();
/*
 desc : “_AUTODUMPS” 모드 값이 “1”로 설정된 경우, 유효한 명령어
		생성하고자 하는 CCD Samples 파일에 대한 File Numbering 값 설정
 parm : file_no	- [in]  File Numbering (0 ~ 9)
 retn : TRUE or FALSE
 note : 파일 이름은 “AUTOS”와 File Number로 구성되고, 확장자는 “CSV”로 구성
		ex> AUTOS1.CSV, AUTOS2.CSV, …, AUTOS9.CSV
		파일이 생성되고 난 이후, File Number는 자동으로 증가.
		즉, 정렬 (BAL Command) 명령 후 Align 결과가 실패한 경우, File Number가 자동으로 증가
		만약 File Number가 9 넘어가면, 자동으로 0으로 바뀌는지는 모름?  문의 ?
		그래서, “BAL” Command어 실행하기 전에 항상 “_AUTODUMPFS 0” 명령어 호출해줘야 됨?
*/
API_IMPORT BOOL uvMPA_SetAlignFailAutoDumpFile(UINT8 file_no);
/*
 desc : Main Prealigner Calibration 절차 초기화 (개시) 진행
		Chuck 혹은 Pins의 교체 (재배치)와 같이 Prealigner에서 기구(기계)적인 변경
		(ex> 교체, 위치 조정 혹은 수정 등)가 있었을 경우, 이 명령어 사용
 parm : None
 retn : TRUE or FALSE
 note : Prealigner에 의해 지원되는 비-투명 3, 4, 8, 12 혹은 18 인치를 이용하여,
		이 명령을 실행해야 함
*/
API_IMPORT BOOL uvMPA_SetPhysicalChangeCalibration();
/*
 desc : CCD 보상 계수 (“_SH” and “_SL”) 그리고 현재 Wafer에 대한
		CCD 중심의 보상 (Calibration)을 초기화 (시작) 수행
		Light House 높이가 변경 혹은 Z 축 측정 위치 (“_MP Z”)가 변경된 경우, 이 명령을 수행 추천
 parm : mode1	- [in]  use small iteration steps (알고리즘 성격 정의)
						0x00: 빠른 보정 알고리즘
						0x01: 느린 보정 알고리즘 (대신 정밀한 검사 수행)
		mode2	- [in]  CCD 중심 검색 여부
 retn : TRUE or FALSE
 note : Wafer가 Prealigner에 놓여져 있는 상태에서 수행되며
		Calibration된 파라미터 값들이 저장되지 않으므로, “SMP” 명령을 통해서 수동으로 저장
*/
API_IMPORT BOOL uvMPA_SetPhysicalChangeCompensation(UINT8 mode1, UINT8 mode2);
/*
 desc : CCD Samples를 마지막 검사에서 “samples.BAL”인 파일로 저장
		(파일 이름 끝에 임의의 Numbering이 붙음)
 parm : file_no	- [in]  File Numbering (0 ~ 9)
 retn : TRUE or FALSE
 note : 이 파일은 제조 쪽에 Alignment Algorithm의 상세 문제 (Troubleshooting)에 대해
		알리고자 할 때 사용됨 (이 명령을 실행하기 앞서, “DRPS” 명령이 실행되어져야 함)
*/
API_IMPORT BOOL uvMPA_SetDetailTroubleFileByManufacture(UINT8 file_no);
/*
 desc : CCD Samples를 마지막 검사에서 “samples.CSV”인 파일로 저장
		(파일 이름 끝에 임의의 Numbering이 붙음)
 parm : file_no	- [in]  File Numbering (0 ~ 9)
 retn : TRUE or FALSE
 note : 이 파일은 제조 쪽 혹은 서비스 직원에 Alignment Algorithm의 기본 문제 (Troubleshooting)에 대해
		알리고자 할 때 사용됨
*/
API_IMPORT BOOL uvMPA_SetBasicTroubleFileByManufacture(UINT8 file_no);
/*
 desc : 이 명령은 마지막 데이터 수집 실행 중에 얻어진 samples를 내부 메모리로 읽어들이고,
		 “DDPF” 명령을 사용하기 위해 samples를 준비해 놓음 즉, 읽어들인 샘플의 개수 반환
 parm : None
 retn : TRUE or FALSE
 */
API_IMPORT BOOL uvMPA_SetTroubleDumpToMemory();
/*
 desc : 현재 Prealigner의 Edge 조작 여부 (Edge Handling 여부)
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetEdgeHandleType();
/*
 desc : Target -> Chuck
		Edge Handling Prealigner의 척이 Sensor를 덮을 때, 최대 CCD 값 지정
		CCD Sensor가 검사 (Scanning) 중일 때, 이 파라미터 (CCD Values)
		하위 (아래)에 있는 값일 경우 Wafer가 없는 것으로 판단
		(Sensing 거리 값. Wafer를 감지하기 위한 분포 값)
 parm : type	- [in]  0x00: 일반 Wafer Size, 0x01: Smaller Wafer Size, 0x02: Greater Wafer Size
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetCCDChuckSensingDist(UINT8 type);
/*
 desc : Target -> Chuck
		Edge Handling Prealigner의 척이 Sensor를 덮을 때, 최대 CCD 값 지정
		CCD Sensor가 검사 (Scanning) 중일 때, 이 파라미터 (CCD Values)
		하위 (아래)에 있는 값일 경우 Wafer가 없는 것으로 판단
		(Sensing 거리 값. Wafer를 감지하기 위한 분포 값)
 parm : type	- [in]  0x00: 일반 Wafer Size, 0x01: Smaller Wafer Size, 0x02: Greater Wafer Size
		pixel	- [in]  Sensing 거리 값 (단위: pixels)
 retn : TRUE or FALSE
 참고 :	이 명령을 설정된 파라미터 값은 “SMP” 명령어에 의해 내부 메모리에 저장 됨
*/
API_IMPORT BOOL uvMPA_SetCCDChuckSensingDist(UINT16 pixel, UINT8 type);
/*
 desc : Target -> Chuck Pins
		Edge Handling Prealigner의 척이 Sensor를 덮을 때, 최대 CCD 값 지정
		CCD Sensor가 검사 (Scanning) 중일 때, 이 파라미터 (CCD Values)
		하위 (아래)에 있는 값일 경우 Wafer가 없는 것으로 판단
		(Sensing 거리 값. Wafer를 감지하기 위한 분포 값)
 parm : type	- [in]  0x00: 일반 Wafer Size, 0x01: Smaller Wafer Size, 0x02: Greater Wafer Size
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetCCDPinsSensingDist(UINT8 type);
/*
 desc : Target -> Chuck Pins
		Edge Handling Prealigner의 척이 Sensor를 덮을 때, 최대 CCD 값 지정
		CCD Sensor가 검사 (Scanning) 중일 때, 이 파라미터 (CCD Values)
		하위 (아래)에 있는 값일 경우 Wafer가 없는 것으로 판단
		(Sensing 거리 값. Wafer를 감지하기 위한 분포 값)
 parm : type	- [in]  0x00: 일반 Wafer Size, 0x01: Smaller Wafer Size, 0x02: Greater Wafer Size
		pixel	- [in]  Sensing 거리 값 (단위: pixels)
 retn : TRUE or FALSE
 참고 :	이 명령을 설정된 파라미터 값은 “SMP” 명령어에 의해 내부 메모리에 저장 됨
*/
API_IMPORT BOOL uvMPA_SetCCDPinsSensingDist(UINT16 pixel, UINT8 type);
/*
 desc : Edge Handling Prealigner의 Chuck 위에서 수직 축을 따라 고정된 Pins 위에 안전하게
		회전할 수 있는 위치를 출력
		즉, Wafer가 안전하게 회전할 수 있는 수직 (Z; Vertical)의 위치를 설정 내지 출력해 줌
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_GetSafeRotateAxisZ();
/*
 desc : Edge Handling Prealigner의 Chuck 위에서 수직 축을 따라 고정된 Pins 위에 안전하게
		회전할 수 있는 위치를 설정
		즉, Wafer가 안전하게 회전할 수 있는 수직 (Z; Vertical)의 위치를 설정 내지 출력해 줌
 parm : value	- [in]  안전하게 회전할 Z 축 위치 (단위: um)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_SetSafeRotateAxisZ(DOUBLE value);

/* --------------------------------------------------------------------------------------------- */
/*                                            Common                                             */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 가장 최근에 송신한 명령에 대해 수신된 패킷이 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_IsLastSendAckRecv();
/*
 desc : 가장 최근에 송신한 명령에 대해 수신 응답이 성공 했는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvMPA_IsLastSendAckSucc();
/*
 desc : 시스템 상태 값에 대한 에러 메시지 반환
 parm : mesg	- [out] 에러 코드에 대한 상세 설명이 저장될 반환 버퍼
		size	- [in]  'mesg' 버퍼 크기
 retn : FALSE (에러 없음), TRUE (에러 있음)
 note : "mesg" 버퍼를 최소 1024만큼 설정해 줘야 됨
*/
API_IMPORT BOOL uvMPA_GetSystemStatusMesg(TCHAR *mesg, UINT32 size);
/*
 desc : 이전에 송신된 명령어의 에러 값 (코드)에 대한 상세 문자열 정보 반환
 parm : mesg	- [out] 에러 코드에 대한 상세 설명이 저장될 반환 버퍼
		size	- [in]  'mesg' 버퍼 크기
 retn : FALSE (에러 없음), TRUE (에러 있음)
*/
API_IMPORT BOOL uvMPA_GetPreviousError(TCHAR *mesg, UINT32 size);
/*
 desc : 가장 최근에 수신된 데이터가 있는지 여부
 parm : cmd		- [in]  검사 대상 명령어
		r_cmd	- [out] 가장 최근에 수신된 명령어가 저장될 버퍼
		size	- [in]  'r_cmd' 버퍼의 크기
 retn : TRUE or FALSE
 note : 만약, 'cmd' 값에 값을 입력하지 않으면, 가장 최근에 송신한 명령어에 대해, 수신된 값이 존재하는지 여부
*/
API_IMPORT BOOL uvMPA_IsRecvCmdData(ENG_PSCC cmd=ENG_PSCC::psc_none);
API_IMPORT BOOL uvMPA_IsRecvCmdLast();
API_IMPORT BOOL uvMPA_GetRecvCmdLast(PTCHAR r_cmd, UINT32 size);
/*
 desc : 장비 (시스템) 내부에 에러가 발생 했는지 여부
 parm : Non
 retn : TRUE
*/
API_IMPORT BOOL uvMPA_IsSystemError();


#ifdef __cplusplus
}
#endif