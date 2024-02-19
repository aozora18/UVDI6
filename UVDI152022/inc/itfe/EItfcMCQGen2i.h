
/*
 desc : The Interface Library for MITSUBISH's MELSEC-Q PLC Equipment
*/

#pragma once

#include "../conf/global.h"
#include "../conf/define.h"
#include "../conf/mcdrv.h"
#include "../conf/plc_addr_gen2i.h"

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

/* --------------------------------------------------------------------------------------------- */
/*                         외부 함수 - < PLC - Melsec Q > < for Common >                         */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Check whether the current PLC is connected
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MCQ_IsConnected();
/*
 desc : Returns the most recently occurred Error Code (Status) value.
 parm : None
 retn : Error Code 값
*/
API_IMPORT UINT16 uvCmn_MCQ_GetLastErrorCode();
/*
 desc : Initialize the most recently occurred Error Code (Status)
 parm : None
 retn : None
*/
API_IMPORT VOID uvCmn_MCQ_ResetLastErrorCode();
/*
 desc : MC Protocol - 특정 위치의 Bit 데이터 값 반환
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소) (4 Bytes이지만, 3 Bytes 값만 유효)
 retn : 비트 값 (0 or 1) 반환
*/
API_IMPORT UINT8 uvCmn_MCQ_GetBitsValue(UINT32 addr);
/*
 desc : MC Protocol - 임의 주소의 Word 데이터 값 반환
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소) (4 Bytes이지만, 3 Bytes 값만 유효)
 retn : Word 값 반환
*/
API_IMPORT UINT16 uvCmn_MCQ_GetWordValue(UINT32 addr);
/*
 desc : MC Protocol - 메모리에 저장된 Word 데이터 값 반환
 parm : index	- [in]  Word 단위의 메모리의 인덱스 위치 값 (0 or Later)
 retn : Word 값 반환
*/
API_IMPORT UINT16 uvCmn_MCQ_GetWordMemory(UINT16 index);
/*
 desc : MC Protocol - 메모리에 저장된 DWord 데이터 값 반환
 parm : index	- [in]  Word 단위의 메모리의 인덱스 위치 값 (0 or Later)
 retn : Word 값 반환
*/
API_IMPORT UINT32 uvCmn_MCQ_GetDWordMemory(UINT16 index);
/*
 desc : MC Protocol - 임의 주소의 DWord 데이터 값 반환
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소) (4 Bytes이지만, 3 Bytes 값만 유효)
 retn : Word 값 반환
*/
API_IMPORT UINT32 uvCmn_MCQ_GetDWordValue(UINT32 addr);

/* --------------------------------------------------------------------------------------------- */
/*           !!! C# 쪽에 전달을 하기 위한 목적으로 만들어진 외부 라이브러리 함수 임 !!!          */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : PLC에 1 개 이상의 알람 값이 존재하는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MCQ_IsAlarm();
/*
 desc : PLC에 1 개 이상의 경고 값이 존재하는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCmn_MCQ_IsWarn();
/*
 desc : MC Protocol - 임의 위치의 Bit 데이터 값 반환
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소)
 retn : 비트 값 (0 or 1) 반환
*/
API_IMPORT UINT8 uvCmn_MCQ_GetBitsValueEx(ENG_PIOA addr);
/*
 desc : MC Protocol - 특정 위치의 Bit 데이터 쓰기
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소)
		value	- [in]  Bit 1개 값이 저장된 버퍼
 retn : TRUE or FALSE (에러 코드가 저장됨 ?)
*/
API_IMPORT BOOL uvEng_MCQ_WriteBitsValueEx(ENG_PIOA addr, UINT8 value);
/*
 desc : MC Protocol - 특정 위치의 Bit 데이터 반전 후 쓰기
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소)
		value	- [in]  Bit 1개 값이 저장된 버퍼
 retn : TRUE or FALSE (에러 코드가 저장됨 ?)
*/
API_IMPORT BOOL uvEng_MCQ_WriteBitsInvertEx(ENG_PIOA addr);
/*
 desc : MC Protocol - 임의 주소의 Word 데이터 값 반환
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소)
 retn : Word 값 반환
*/
API_IMPORT UINT16 uvCmn_MCQ_GetWordValueEx(ENG_PIOA addr);
/*
 desc : MC Protocol - Word 데이터 쓰기
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소)
		value	- [in]  Word 1개 값이 저장된 버퍼
 retn : TRUE or FALSE (에러 코드가 저장됨 ?)
*/
API_IMPORT BOOL uvEng_MCQ_WriteWordValueEx(ENG_PIOA addr, UINT16 value);
/*
 desc : MC Protocol - 임의 주소의 DWord 데이터 값 반환
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소)
 retn : Word 값 반환
*/
API_IMPORT UINT32 uvCmn_MCQ_GetDWordValueEx(ENG_PIOA addr);
/*
 desc : MC Protocol - DWord 데이터 쓰기
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소)
		value	- [in]  DWord 1개 값이 저장된 버퍼
 retn : TRUE or FALSE (에러 코드가 저장됨 ?)
*/
API_IMPORT BOOL uvEng_MCQ_WriteDWordValueEx(ENG_PIOA addr, UINT32 value);

/* --------------------------------------------------------------------------------------------- */
/*                      외부 Utility - < PLC - Melsec Q > < for C# or GUI >                      */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : MC Protocol - Tower (Lamp On or Off) or (All On or OFF)
 parm : color	- [in]  0x01 : green, 0x02 : yellow, 0x03 : red, 0x04 : all (ENG_TLI)
		power	- [in]  0x00 : OFF, 0x01 : ON
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_WriteTowerLampOnOff(ENG_TLCI color, UINT8 power);
/*
 desc : MC Protocol - Photohead Z Axis <Cooling> All On or Off
 parm : power	- [in]  0x00 : OFF, 0x01 : ON
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_WritePhZAxisCoolingAll(UINT8 power);
/*
 desc : MC Protocol - Photohead Z Axis <Cylinder> All On or Off
 parm : power	- [in]  0x00 : OFF, 0x01 : ON
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_WritePhZAxisCyclinderAll(UINT8 power);
/*
 desc : MC Protocol - Vacuum On or Off
 parm : chuck	- [in]  Wafer Chuck Vacuum (0x00 : OFF, 0x01 : ON)
		robot	- [in]  Robot Vacuum (0x00 : OFF, 0x01 : ON)
		aligner	- [in]  Aligner Vacuum (0x00 : OFF, 0x01 : ON)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_WriteVacuumControl(UINT8 chuck, UINT8 robot, UINT8 aligner);
/*
 desc : MC Protocol - Photohead Power On or Off (특수하게 처리해야 됨)
 parm : ph1_power	- [in]  Photohead 1st Power (0x00 : OFF, 0x01 : ON)
		ph2_power	- [in]  Photohead 2nd Power (0x00 : OFF, 0x01 : ON)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_WritePhPower(UINT8 ph1_power, UINT8 ph2_power);
/*
 desc : MC Protocol - MC2 Power On or Off (특수하게 처리해야 됨)
 parm : power	- [in]  MC2 Power (0x00 : OFF, 0x01 : ON)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_WriteMC2Power(UINT8 power);
/*
 desc : MC Protocol - Light On or Off for Illumination
 parm : None
 retn : TRUE or FALSE
 note : It is performed internally in reverse
*/
API_IMPORT BOOL uvEng_MCQ_WriteLightIllumInvert();
/*
 desc : MC Protocol - Light On or Off for Halogen
 parm : h_ring	- [in]  Halogen Ring Power (0x00 : OFF, 0x01 : ON)
		h_coax	- [in]  Halogen Coaxial Power (0x00 : OFF, 0x01 : ON)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_WriteLightControl(UINT8 h_ring, UINT8 h_coax);
/*
 desc : MC Protocol - XY Stage Move Prohibit Release or OFF
 parm : release	- [in]  0x00 : OFF, 0x01 : RELEASE
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_WriteXYStageMoveProhibitRelease(UINT8 release);
/*
 desc : MC Protocol - Linked.Litho : EXP-INK ~ EXP-RDY (Exposure Inline ~ Exposure Receive To Ready)
 parm : type	- [in]  exp-ink (0x01), exp-rej (0x02), exp-snd (0x03), exp-rdy (0x04)
		mode	- [in]  0x00 : reset, 0x01 : set
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_WriteLithoExposureToTrack(ENG_LLET type, UINT8 mode);
/*
 desc : Halogen Light Power (소숫점 2자리까지 유효)
 parm : type	- [in]  0x00 : Ring, 0x01 : Coaxial
		volt	- [in]  Light Power (0.00 ~ 12.00 volt)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_WriteLightPowerVolt(ENG_HLPT type, DOUBLE volt);
/*
 desc : Lift Pin or BSA Axis 제어
 parm : type	- [in]  0:en_pin_x_axis, 1:en_pin_z_axis, 2:en_bas_z_axis
		mode	- [in]  Control Mode (1:Enable or Disable,2:Reset, 3:Stop)
		value	- [in]  0x00 (Disable or OFF)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_WriteLiftPinAxisControl(ENG_LPAT type, ENG_LPAC mode, UINT8 value);
/*
 desc : Lift Pin or BSA Axis 동작 처리
 parm : type	- [in]  0:en_pin_x_axis, 1:en_pin_z_axis, 2:en_bas_z_axis
		mode	- [in]  Action Mode (1:jog+move, 2:jog-move, 3:home position, 4:position, 5:alarm reset)
		value	- [in]  0x00 (Disable or OFF)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_WriteLiftPinAxisAction(ENG_LPAT type, ENG_LPAA mode, UINT8 value);
/*
 desc : Lift Pin or BSA Axis : Jog Speed 설정
 parm : type	- [in]  0:en_pin_x_axis, 1:en_pin_z_axis, 2:en_bas_z_axis
		value	- [in]  Jog Speed Value (unit : mm/min)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_WriteLiftPinAxisJogSpeed(ENG_LPAT type, DOUBLE value);
/*
 desc : Lift Pin or BSA Axis : Position Speed 설정
 parm : type	- [in]  0:en_pin_x_axis, 1:en_pin_z_axis, 2:en_bas_z_axis
		value	- [in]  Jog Speed Value (unit : mm/min)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_WriteLiftPinAxisPosSpeed(ENG_LPAT type, DOUBLE value);
/*
 desc : Lift Pin or BSA Axis : Position 설정
 parm : type	- [in]  0:en_pin_x_axis, 1:en_pin_z_axis, 2:en_bas_z_axis
		value	- [in]  Jog Speed Value (unit : um)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_MCQ_WriteLiftPinAxisPos(ENG_LPAT type, DOUBLE value);
#ifdef __cplusplus
}
#endif