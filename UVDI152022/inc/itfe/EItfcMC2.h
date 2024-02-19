
/*
 desc : The Interface Library for Sieb & Meyer's MC2 Motion Equipment
*/

#pragma once

#include "../conf/global.h"
#include "../conf/define.h"
#include "../conf/mc2.h"


#ifdef __cplusplus
extern "C"
{
#endif

/* --------------------------------------------------------------------------------------------- */
/*                              외부 함수 - < MC2 > < for Engine >                               */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 스테이지 X / Y를 Vector 로 이동
 parm : drv_x	- [in]  Vector 이동 대상 축의 Drive ID 1
		drv_y	- [in]  Vector 이동 대상 축의 Drive ID 2
		pos_x	- [in]  스테이지가 최종 이동될 X 축의 위치 (단위: 0.1 um or 100 nm)
		pos_y	- [in]  스테이지가 최종 이동될 Y 축의 위치 (단위: 0.1 um or 100 nm)
		velo	- [in]  스테이지 이동 속도 값
		axis	- [out] Vector 이동할 경우, Master Axis (기준 축) 값 반환
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MC2_SendDevMoveVectorXY(ENG_MMDI drv_x, ENG_MMDI drv_y,
											  DOUBLE pos_x, DOUBLE pos_y, DOUBLE velo, ENG_MMDI &axis);
/*
 desc : 기존 시스템 접속 해제 후 재접속 진행
 parm : None
 retn : TRUE or FALSE
 note : EN_STOP 모드가 적용되지 않는 곳에 사용
*/
API_IMPORT BOOL uvEng_MC2_Reconnected();
/*
 desc : Device Stopped
 parm : drv_id	- [in]  SD2 Driver ID 즉, 0x00, 0x01, 0x04, 0x05
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MC2_SendDevStopped(ENG_MMDI drv_id);
/*
 desc : 현재 모든 Drive의 Motor 중지
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MC2_SendDevStoppedAll();
/*
 desc : Device Locked (On or OFF)
 parm : drv_id	- [in]  SD2 Driver ID
		flag	- [in]  TRUE (Servo On) or FALSE(Servo Off) Command
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MC2_SendDevLocked(ENG_MMDI drv_id, BOOL flag);
/*
 desc : All Device Locked (On or OFF)
 parm : flag	- [in]  TRUE (Servo On) or FALSE(Servo Off) Command
 retn : TRUE (All Servo On) or FALSE (All Servo Off)
*/
API_IMPORT BOOL uvCmn_MC2_SendDevLockedAll(BOOL flag);
/*
 desc : Device Fault Reset (Error Init)
 parm : drv_id	- [in]  Motor Drive ID (SD2 Device ID) 즉, 0x00, 0x01, 0x04, 0x05
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MC2_SendDevFaultReset(ENG_MMDI drv_id);
/*
 desc : 모든 Device Fault Reset All (Sequential) (0x3808)
 parm : None
 retn : TRUE or FALSE
 note : EN_STOP 모드가 적용되지 않는 곳에 사용
*/
API_IMPORT BOOL uvEng_MC2_SendDevFaultResetAll();
/*
 desc : 모든 Device Fault Reset All (Concurrency : Used where EN_STOP mode is enabled.) (0x3801)
 parm : None
 retn : TRUE or FALSE
 note : EN_STOP 모드가 적용된 곳에 사용
*/
API_IMPORT BOOL uvEng_MC2_SendDevFaultResetAllEx();
/*
 desc : Motor Drive에 대해 Homing
 parm : drv_id	- [in]  MC2 (SDS) Drive ID
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MC2_SendDevHoming(ENG_MMDI drv_id);
/*
 desc : 모든 Device Homing All (Sequential) (0x3808)
 parm : None
 retn : TRUE or FALSE
 note : EN_STOP 모드가 적용되지 않는 곳에 사용
*/
API_IMPORT BOOL uvEng_MC2_SendDevHomingAll();
/*
 desc : 모든 Device Homing All (Concurrency : Used where EN_STOP mode is enabled.) (0x3808)
 parm : None
 retn : TRUE or FALSE
 note : EN_STOP 모드가 적용된 곳에 사용
*/
API_IMPORT BOOL uvEng_MC2_SendDevHomingAllEx();
/*
 desc : All Device Fault Reset (Error Init) - Only drives in which errors occur sequentially are reset.
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MC2_SendDevFaultResetCheckAll();
/*
 desc : Luria가 현재 노광 (Printing) 중인지 여부에 따라,
		MC2에게 주기적으로 통신 데이터 요청 여부 설정
 parm : enable	- [in]  TRUE : 요청 진행, FALSE : 요청 중지 (아예 요청 중지가 아님)
 retn : None
*/
API_IMPORT VOID uvEng_MC2_SetSendPeriodPkt(BOOL enable);
/*
 desc : MC2에게 주기적으로 패킷 데이터를 요청하는지 여부 반환
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MC2_IsSendPeriodPkt();
/*
 desc : Move the motor to absoule position (Up, Down, Left, Bottom)
 parm : drv_id	- [in]  Motor Drive ID (SD2 Device ID) 즉, 0x00, 0x01, 0x04, 0x05
		move	- [in]  position value to move the motor (unit: mm) (It is valid up to 4 decimal places)
		velo	- [in]  Speed value (unit: mm /sec) (It is valid up to 4 decimal places)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MC2_SendDevAbsMove(ENG_MMDI drv_id, DOUBLE move, DOUBLE velo);
API_IMPORT BOOL uvEng_MC2_SendDevAbsMoveExt(ENG_MMDI drv_id, DOUBLE move);
/*
 desc : Move the motor to a relative position (Up, Down, Left, Bottom)
 parm : method	- [in]  ENG_MMMM (Jog, Step, Abs: Absolute Position)
		drv_id	- [in]  Motor Drive ID (SD2 Device ID) 즉, 0x00, 0x01, 0x04, 0x05
		direct	- [in]  Move direct (Up / Down / Left / Right ...)
		dist	- [in]  Move distance (unit: mm) (It is valid up to 4 decimal places)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MC2_SendDevRefMove(ENG_MMMM method, ENG_MMDI drv_id,
										 ENG_MDMD direct, DOUBLE dist);
/*
 desc : Focus Motor Move Absolute Position (모든 Photohead의 Z Axis을 기본 위치로 이동)
 parm : drv_id	- [in]  Motor Drive ID (SD2 Device ID) 즉, 0x00, 0x01, 0x04, 0x05
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MC2_ReqSetPhMotorFocusMove(ENG_MMDI drv_id);
/*
 desc : 모든 Photohead Z 축이 Focus 위치로 이동 했는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MC2_IsAllPhMotorFocusMoved();
/*
 desc : Luria Expose Mode로 설정
 parm : drv_id	- [in]  노광 방향의 축 Drive ID
		mode	- [in]  Area (Align) mode or Expose mode (ENG_MTAE)
						Area Mode			: 절대 스테이지가 움직이지 않음
						Expo Mode			: 현재 위치 값보다 Min 값이 작으면, 움직이지 않으며, 나머지는 Min 값으로 이동 함
		pixel	- [in]  Pixel Size (unit: mm)
		begin	- [in]  노광 시작 위치 (단위: mm)
		end		- [in]  노광 종료 위치 (단위: mm)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MC2_SendDevLuriaMode(ENG_MMDI drv_id, ENG_MTAE mode,
										   DOUBLE pixel=0.0f, DOUBLE begin=0.0f, DOUBLE end=0.0f);
/*
 desc : Reference Data 전체 요청
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MC2_ReadReqPktRefAll();
/*
 desc : Active Data 전체 요청
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MC2_ReadReqPktActAll();
/*
 desc : Device Control - Reference Header - for EN_STOP and HEART_BEAT
 parm : en_stop	- [in]  ENG_MMSM (0x00:Only Homing operation or 0x01: Other operation)
		beat	- [in]  Toggle (0 or 1) (Enter the opposite value based on the current value)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MC2_SetRefHeadEnStop(ENG_MMST en_stop, UINT8 beat=0x00);
API_IMPORT BOOL uvEng_MC2_SetRefHeadEnStopEx(ENG_MMST en_stop);

/* --------------------------------------------------------------------------------------------- */
/*                              외부 함수 - < MC2 > < for Common >                               */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : MC2 Server에 연결 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MC2_IsConnected();
/*
 desc : 현재 모든 Drive의 Motor가 중지 상태인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MC2_IsMotorDriveStopAll();
/*
 desc : MC2에서 동작 중인 Drive (SD2S) 중 한 개라도 에러가 발생 했는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MC2_IsAnyDriveError();
/*
 desc : 현재 Motor Drive의 EN_STOP 값 반환
 parm : None
 retn : TRUE (EN_STOP == yes) or FALSE (EN_STOP == no)
*/
API_IMPORT BOOL uvCmn_MC2_IsDriveEnStop();
/*
 desc : 현재 Motor Drive의 Heart Beat 값 반환
 parm : None
 retn : 0 or 1
*/
API_IMPORT UINT8 uvCmn_MC2_GetRefHeartBeat();
/*
 desc : 현재 Motor Drive의 V_SCALE 값 반환
 parm : None
 retn : scale 값 (0 ~ 100 %)
*/
API_IMPORT UINT8 uvCmn_MC2_GetRefVScale();
/*
 desc : 현재 Motor Drive의 OUTPUT S값 반환
 parm : None
 retn : scale 값 (0 ~ 100 %)
*/
API_IMPORT UINT64 uvCmn_MC2_GetRefOutputs();
/*
 desc : Check whether an error has occurred in the any motor drive
 parm : drv_id	- [in]  Motor Drive ID (SD2 Device ID) that is, 0x00, 0x01, 0x04, 0x05
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MC2_IsDriveError(ENG_MMDI drv_id);
API_IMPORT BOOL uvCmn_MC2_IsDriveErrorAll();
/*
 desc : MC2에서 DNC Error가 발생 했는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MC2_IsDNCError();
/*
 desc : MC2에서 동작 중인 Drive (SD2S) 중 한 개라도 BUSY 상태인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MC2_IsAnyDriveBusy();
/*
 desc : MC2에서 동작 중인 임의 Drive (SD2S)가 Busy 상태인지 여부
		drv_id	- [in]  Motor Drive ID (SD2 Device ID) 즉, 0x00, 0x01, 0x04, 0x05
 retn : TRUE (Busy) or FALSE (Idle)
*/
API_IMPORT BOOL uvCmn_MC2_IsDriveBusy(ENG_MMDI drv_id);
/*
 desc : 현재 임의 모션 드라이브가 이동 중인지 여부
 parm : drv_id	- [in]  Motor Drive ID (SD2 Device ID) 즉, 0x00, 0x01, 0x04, 0x05
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MC2_IsDriveMoving(ENG_MMDI drv_id);
/*
 desc : 현재 MC2에서 동작 (Busy) 중인 Drive의 번호 반환
 parm : None
 retn : drive number (0 ~ 7), 없다면 0xff
 note : 여러 개가 Busy 상태일지라도, 가장 먼저 Busy인 드라이브 번호만 반환
*/
API_IMPORT UINT8 uvCmn_MC2_GetFirstBusyDriveNo();
/*
 desc : 현재 임의 Motor Drive의 토글 값 얻기
 parm : drv_id	- [in]  Toggle 값 얻기 위한 드라이브 ID (0x00 ~ 0x07)
 retn : None
*/
API_IMPORT VOID uvCmn_MC2_GetDrvDoneToggled(ENG_MMDI drv_id);
/*
 desc : 임의 Motor Drive의 Toggled 값이 이전 Toggled 값과 다른지 여부 비교
 parm : drv_id	- [in]  Toggle 값 얻기 위한 드라이브 ID (0x00 ~ 0x07)
 retn : TRUE (이전 값과 다르다) or FALSE (이전 값과 동일하다)
*/
API_IMPORT BOOL uvCmn_MC2_IsDrvDoneToggled(ENG_MMDI drv_id);
/*
 desc : 모든 Motor Drive의 토글 값 얻기
 parm : None
 retn : None
*/
API_IMPORT VOID uvCmn_MC2_GetDrvDoneToggledAll();
/*
 desc : 모든 Motor Drive의 Toggled 값이 이전 Toggled 값과 다른지 여부 비교
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MC2_IsDrvDoneToggledAll();
/*
 desc : Returns the absolute position of the current motion drive
 parm : drv_id	- [in]  Motion drive ID (0x00 ~ 0x07)
 retn : position (unit: mm)
*/
API_IMPORT DOUBLE uvCmn_MC2_GetDrvAbsPos(ENG_MMDI drv_id);
/*
 desc : 모든 드라이브의 상태 값 저장 (에러 포함)
 parm : None
 retn : None
*/
API_IMPORT VOID uvCmn_MC2_SaveAllDriveState();
/*
 desc : Check whether the motor drive is in homing state
 parm : drv_id	- [in]  motor drive id
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MC2_IsDriveHomed(ENG_MMDI drv_id);
API_IMPORT BOOL uvCmn_MC2_IsDriveHomedAll();
/*
 desc : 특정 모션 드라이브의 축에 해당되는 에러 값 반환
 parm : drv_id	- [in]  모션 드라이브 ID
 retn : 에러 코드 값 반환
*/
API_IMPORT UINT16 uvCmn_MC2_GetDriveError(ENG_MMDI drv_id);
/*
 desc : 특정 모션 드라이브의 축에 해당되는 Result 값 반환
 parm : drv_id	- [in]  모션 드라이브 ID
		flag	- [in]  0x01 : Result1, 0x02 : Result2
 retn : Result 1 or 2 값 반환
*/
API_IMPORT INT32 uvCmn_MC2_GetDriveResult(ENG_MMDI drv_id, UINT8 flag);
/*
 desc : MC2에서 동작 중인 Drive (SD2S)의 상태 값 반환
 parm : drv_id	- [in]  모션 드라이브 ID
		flag	- [in]  요청 플래그 값
 retn : 상태 값 반환
*/
API_IMPORT UINT8 uvCmn_MC2_GetDriveStatus(ENG_MMDI drv_id, ENG_SADS flag);
/*
 desc : MC2에서 동작 중인 Drive (SD2S)에서 Actual Header중 MState 값 반환
 parm : heart_no	- [in]  Heart Beat Number (1 0r 2)
 retn : 0 or 1 값 반환
*/
API_IMPORT UINT8 uvCmn_MC2_GetActState(UINT8 heart_no);
/*
 desc : MC2에서 동작 중인 Drive (SD2S)에서 Actual Header중 MError 값 반환
 parm : None
 retn : 0 or Error 값 반환
*/
API_IMPORT UINT16 uvCmn_MC2_GetActError();
/*
 desc : MC2에서 동작 중인 Drive (SD2S)에서 Actual Header중 Inputs or Output 값 반환
 parm : type	- [in]  0x00 : Inputs, 0x01 : Outputs
 retn : 값 반환
*/
API_IMPORT UINT64 uvCmn_MC2_GetActInOut(UINT8 type);
/*
 desc : 각 드라이브의 서보의 Locked 상태 반환 (On or Off)
 parm : drv_id	- [in]  SD2 Driver ID
 retn : TRUE (Servo On) or FALSE (Servo Off)
*/
API_IMPORT BOOL uvCmn_MC2_IsDevLocked(ENG_MMDI drv_id);
/*
 desc : Returns whether the servo status of all motor drives is locked
 parm : None
 retn : TRUE (All Servo On) or FALSE (All Servo Off)
*/
API_IMPORT BOOL uvCmn_MC2_IsAllDevLocked();

#ifdef __cplusplus
}
#endif