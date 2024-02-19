
/*
 desc : The Interface Library for MITSUBISH's MELSEC-Q PLC Equipment
*/

#include "pch.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/* --------------------------------------------------------------------------------------------- */
/*                                           내부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 입력된 주소 값이 유효한지 여부
 parm : addr	- [in]  주소 값
 retn : TRUE or FALSE
*/
BOOL IsValidAddr(UINT32 addr)
{
	LPG_CPSI pstPLC = &GetConfig()->melsec_q_svc;

	if ((pstPLC->start_d_addr > addr) ||
		(pstPLC->start_d_addr + pstPLC->addr_d_count) < addr)
	{
		TCHAR tzMesg[256]	= {NULL};
		swprintf_s(tzMesg, 256, L"PLC Address [%08x] is incorrect", addr);
		LOG_ERROR(ENG_EDIC::en_engine, tzMesg);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : PLC 기본 주소를 WORD 주소와 BIT 위치 값으로 분리 후 반환
 parm : addr	- [in]  PLC I/O 주소
		w_th	- [out] Word 주소가 저장될 참조 변수
		b_th	- [out] Word 주소의 비트 위치 (0 ~ F)가 저장될 참조 변수
 retn : None
*/
VOID GetAddrWordBits(UINT32 addr, UINT32 &w_th, UINT8 &b_th)
{
	UINT32 u32Addr	= UINT32(addr);

	/* WORD 주소 내 Bit 위치 값 (0 ~ F) */
	b_th	= (u32Addr & 0x0000000F);
	/* WORD 주소 값 */
	w_th	= (u32Addr >> 4);
}

/*
 desc : LED, Vacuum 그리고 Shutter를 동시 제어 수행
 parm : start_led	- [in]  0x01: On, 0x00: Off
		vacuum	- [in]  0x01: On, 0x00: Off
		shutter	- [in]  0x01: Open, 0x00: Close
 retn : TRUE or FALSE
*/
BOOL WriteLedVacuumShutter(UINT8 start_led, UINT8 vacuum, UINT8 shutter)
{
	UINT8 u8BitNo[3]	= {NULL};
	UINT16 u16Value		= 0;
	UINT32 u32Addr		= 0;
	UINT32 u32AddrOrg[3]= { UINT32(ENG_PIOA::en_start_lamp),
							UINT32(ENG_PIOA::en_vacuum_on_off),
							UINT32(ENG_PIOA::en_shutter_open_close) };
	/* Vacuum과 Shutter 그리고 Start LED 버튼의 Word 주소는 동일 */
	GetAddrWordBits(u32AddrOrg[0], u32Addr, u8BitNo[0]);	/* Start Lamp */
	GetAddrWordBits(u32AddrOrg[1], u32Addr, u8BitNo[1]);	/* Vacuum */
	GetAddrWordBits(u32AddrOrg[2], u32Addr, u8BitNo[2]);	/* Shutter */
	/* 입력된 주소 값이 유효한지 여부 확인 */
	if (!IsValidAddr(u32Addr))	return FALSE;	/* Start Lamp, Vacuum, Shutter가 동일 Word 주소 0x4B0~ */
	/* 해당 메모리 영역의 1 Word 값 */
	if (!g_pMemPLC->GetWordValue(u32Addr, u16Value))	return FALSE;	/* Start Lamp, Vacuum, Shutter가 동일 Word 주소 0x4B0~ */
	/* Start LED, Vacuum, Shutter의 값이 있는 비트 영역 부분을 0 으로 초기화 */
	u16Value	= (u16Value & 0xFFE3);	/* 1111 1111 1110 0011 */
	/* 특정 위치의 비트 값 변경 for Start LED Button */
	if (start_led)	u16Value	|= (0x01 << u8BitNo[0]);	/* 특정 위치의 비트 값을 1 로 변경 */
	/* 특정 위치의 비트 값 변경 for Vacuum */
	if (vacuum)		u16Value	|= (0x01 << u8BitNo[1]);	/* 특정 위치의 비트 값을 1 로 변경 */
	/* 특정 위치의 비트 값 변경 for Shutter */
	if (shutter)	u16Value	|= (0x01 << u8BitNo[2]);	/* 특정 위치의 비트 값을 1 로 변경 */

															/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;
	return uvMCQDrv_WriteWord(ENG_DNTC::en_d, u32Addr, 0x0001, &u16Value);
}

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
API_EXPORT BOOL uvEng_MCQ_WriteDWord(UINT32 addr, UINT32 value)
{
	UINT8 u8BitNo		= 0x00;
	UINT16 u16Value[2]	= {NULL};
	UINT32 u32Addr		= 0x00000000;

	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;
	if (!uvMCQDrv_IsConnected())	return FALSE;
	/* Bit 위치까지 포함된 주소를 Word 주소와 Bit 위치 값 분리하여 반환 */
	GetAddrWordBits(addr, u32Addr, u8BitNo);
	/* 입력된 주소 값이 유효한지 여부 확인 */
	if (!IsValidAddr(u32Addr))	return FALSE;

	/* WORD 2 개에 나눠서 저장 */
	memcpy(u16Value, &value, 4);

	return uvMCQDrv_WriteWord(ENG_DNTC::en_d, u32Addr, 0x0002, u16Value);
}

/*
 desc : MC Protocol - Word 데이터 쓰기
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소) (4 Bytes이지만, 3 Bytes 값만 유효)
		value	- [in]  Word 1개 값이 저장된 버퍼
 retn : TRUE or FALSE (에러 코드가 저장됨 ?)
*/
API_EXPORT BOOL uvEng_MCQ_WriteWord(UINT32 addr, UINT16 value)
{
	UINT8 u8BitNo	= 0x00;
	UINT16 u16Value	= 0x0000;
	UINT32 u32Addr	= 0x00000000;

	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;
	if (!uvMCQDrv_IsConnected())	return FALSE;
	/* Bit 위치까지 포함된 주소를 Word 주소와 Bit 위치 값 분리하여 반환 */
	GetAddrWordBits(addr, u32Addr, u8BitNo);

	/* 입력된 주소 값이 유효한지 여부 확인 */
	if (!IsValidAddr(u32Addr))	return FALSE;

	return uvMCQDrv_WriteWord(ENG_DNTC::en_d, u32Addr, 0x0001, &value);
}

/*
 desc : MC Protocol - 특정 위치의 Bit 데이터 쓰기
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소) (4 Bytes이지만, 3 Bytes 값만 유효)
		value	- [in]  Bit 1개 값이 저장된 버퍼
 retn : TRUE or FALSE (에러 코드가 저장됨 ?)
*/
API_EXPORT BOOL uvEng_MCQ_WriteBits(UINT32 addr, UINT8 value)
{
	UINT8 u8BitNo	= 0x00;
	UINT16 u16Value	= 0x0000;
	UINT32 u32Addr	= 0x00000000;

	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;
	if (!uvMCQDrv_IsConnected())	return FALSE;
	/* Bit 위치까지 포함된 주소를 Word 주소와 Bit 위치 값 분리하여 반환 */
	GetAddrWordBits(addr, u32Addr, u8BitNo);

	/* 입력된 주소 값이 유효한지 여부 확인 */
	if (!IsValidAddr(u32Addr))	return FALSE;
	/* 해당 메모리 영역의 1 Word 값 */
	if (!g_pMemPLC->GetWordValue(u32Addr, u16Value))		return FALSE;

	/* 특정 위치의 비트 값 변경 */
	if (value)	u16Value	|= (1 << u8BitNo);	/* 특정 위치의 비트 값을 1 로 변경 */
	else		u16Value	&= ~(1 << u8BitNo);	/* 특정 위치의 비트 값을 0 로 변경 */

	return uvMCQDrv_WriteWord(ENG_DNTC::en_d, u32Addr, 0x0001, &u16Value);
}

/*
 desc : MC Protocol - 특정 위치의 Bit 데이터 반전 후 쓰기
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소) (4 Bytes이지만, 3 Bytes 값만 유효)
		value	- [in]  Bit 1개 값이 저장된 버퍼
 retn : TRUE or FALSE (에러 코드가 저장됨 ?)
*/
API_EXPORT BOOL uvEng_MCQ_WriteBitsInvert(UINT32 addr)
{
	UINT8 u8BitNo	= 0x00;
	UINT16 u16Value	= 0x0000;
	UINT32 u32Addr	= 0x00000000;

	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;
	if (!uvMCQDrv_IsConnected())	return FALSE;

	/* Bit 위치까지 포함된 주소를 Word 주소와 Bit 위치 값 분리하여 반환 */
	GetAddrWordBits(addr, u32Addr, u8BitNo);

	/* 입력된 주소 값이 유효한지 여부 확인 */
	if (!IsValidAddr(u32Addr))	return FALSE;
	/* 해당 메모리 영역의 1 Word 값 */
	if (!g_pMemPLC->GetWordValue(u32Addr, u16Value))		return FALSE;

	/* 특정 위치의 비트 값 반전 후 변경 */
	u16Value	^= (1 << u8BitNo);	/* 1 -> 0, 0 -> 1*/

	return uvMCQDrv_WriteWord(ENG_DNTC::en_d, u32Addr, 0x0001, &u16Value);
}

/* --------------------------------------------------------------------------------------------- */
/*                          외부 함수 - < PLC - Melsec Q > < for PODIS >                         */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Align Camera Z 축 Homing
 parm : cam_id	- [in]  1 or 2 (if is 0x03, All)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_SetACamHomingZAxis(UINT8 cam_id)
{
	UINT8 u8BitNo[2]	= {NULL};
	UINT16 u16Value[2]	= {NULL};
	UINT32 u32Addr[2]	= {NULL};
	UINT32 u32AddrOrg[2]= { UINT32(ENG_PIOA::en_camera1_z_axis_home_position),
							UINT32(ENG_PIOA::en_camera2_z_axis_home_position) };

	if (cam_id > 3)	return FALSE;

	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;
	if (cam_id == 0x03)
	{
		GetAddrWordBits(u32AddrOrg[0], u32Addr[0], u8BitNo[0]);
		GetAddrWordBits(u32AddrOrg[1], u32Addr[1], u8BitNo[1]);
		/* 입력된 주소 값이 유효한지 여부 확인 */
		if (!IsValidAddr(u32Addr[0]))	return FALSE;
		if (!IsValidAddr(u32Addr[1]))	return FALSE;
		/* 해당 메모리 영역의 1 Word 값 */
		if (!g_pMemPLC->GetWordValue(u32Addr[0], u16Value[0]))	return FALSE;
		if (!g_pMemPLC->GetWordValue(u32Addr[1], u16Value[1]))	return FALSE;
		/* 특정 위치의 비트 값 변경 */
		u16Value[0]	|= (1 << u8BitNo[0]);	/* 특정 위치의 비트 값을 1 로 변경 */
		u16Value[1]	|= (1 << u8BitNo[1]);	/* 특정 위치의 비트 값을 1 로 변경 */
		u16Value[0]	|= u16Value[1];

		return uvMCQDrv_WriteWord(ENG_DNTC::en_d, u32Addr[0], 0x0001, &u16Value[0]);
	}
	else
	{
		/* Bit 위치까지 포함된 주소를 Word 주소와 Bit 위치 값 분리하여 반환 */
		GetAddrWordBits(u32AddrOrg[cam_id-1], u32Addr[cam_id-1], u8BitNo[cam_id-1]);
		/* 입력된 주소 값이 유효한지 여부 확인 */
		if (!IsValidAddr(u32Addr[cam_id-1]))			return FALSE;
		/* 해당 메모리 영역의 1 Word 값 */
		if (!g_pMemPLC->GetWordValue(u32Addr[cam_id-1], u16Value[cam_id-1]))	return FALSE;
		/* 특정 위치의 비트 값 변경 */
		u16Value[cam_id-1]	|= (1 << u8BitNo[cam_id-1]);	/* 특정 위치의 비트 값을 1 로 변경 */
	
		return uvMCQDrv_WriteWord(ENG_DNTC::en_d, u32Addr[cam_id-1], 0x0001, &u16Value[cam_id-1]);
	}
}

/*
 desc : Align Camera Z 축 Reset
 parm : cam_id	- [in]  1 or 2 (if is 0x03, All)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_SetACamResetZAxis(UINT8 cam_id)
{
	UINT8 u8BitNo[2]	= {NULL};
	UINT16 u16Value[2]	= {NULL};
	UINT32 u32Addr[2]	= {NULL};
	UINT32 u32AddrOrg[2]= { UINT32(ENG_PIOA::en_camera1_z_axis_reset),
							UINT32(ENG_PIOA::en_camera2_z_axis_reset) };

	if (cam_id > 3)	return FALSE;

	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;

	if (cam_id == 0x03)
	{
		GetAddrWordBits(u32AddrOrg[0], u32Addr[0], u8BitNo[0]);
		GetAddrWordBits(u32AddrOrg[1], u32Addr[1], u8BitNo[1]);
		/* 입력된 주소 값이 유효한지 여부 확인 */
		if (!IsValidAddr(u32Addr[0]))	return FALSE;
		if (!IsValidAddr(u32Addr[1]))	return FALSE;
		/* 해당 메모리 영역의 1 Word 값 */
		if (!g_pMemPLC->GetWordValue(u32Addr[0], u16Value[0]))	return FALSE;
		if (!g_pMemPLC->GetWordValue(u32Addr[1], u16Value[1]))	return FALSE;
		/* 특정 위치의 비트 값 변경 */
		u16Value[0]	|= (1 << u8BitNo[0]);	/* 특정 위치의 비트 값을 1 로 변경 */
		u16Value[1]	|= (1 << u8BitNo[1]);	/* 특정 위치의 비트 값을 1 로 변경 */
		u16Value[0]	|= u16Value[1];

		return uvMCQDrv_WriteWord(ENG_DNTC::en_d, u32Addr[0], 0x0001, &u16Value[0]);
	}
	else
	{
		/* Bit 위치까지 포함된 주소를 Word 주소와 Bit 위치 값 분리하여 반환 */
		GetAddrWordBits(u32AddrOrg[cam_id-1], u32Addr[cam_id-1], u8BitNo[cam_id-1]);
		/* 입력된 주소 값이 유효한지 여부 확인 */
		if (!IsValidAddr(u32Addr[cam_id-1]))			return FALSE;
		/* 해당 메모리 영역의 1 Word 값 */
		if (!g_pMemPLC->GetWordValue(u32Addr[cam_id-1], u16Value[cam_id-1]))	return FALSE;
		/* 특정 위치의 비트 값 변경 */
		u16Value[cam_id-1]	|= (1 << u8BitNo[cam_id-1]);	/* 특정 위치의 비트 값을 1 로 변경 */
	
		return uvMCQDrv_WriteWord(ENG_DNTC::en_d, u32Addr[cam_id-1], 0x0001, &u16Value[cam_id-1]);
	}
}

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
API_EXPORT BOOL uvEng_MCQ_SetACamMovePosZ(UINT8 cam_id, UINT8 method, DOUBLE move, UINT8 up_down)
{
	INT32 i32MotorPosZ	= 0 /* 0.1 um or 100 nm */;
	INT32 i32MoveZ		= 0 /* 0.1 um or 100 nm */;
	UINT32 u32Addr[2];

	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;

	/* 현재 Align Camera ID와 Z 축 위치 얻기 */
	if (0x01 == cam_id)
	{
		i32MotorPosZ= (INT32)GetShMemStruct()->r_camera1_z_axis_position_display;
		u32Addr[0]	= (UINT32)ENG_PIOA::en_camera1_z_axis_position_setting;
		u32Addr[1]	= (UINT32)ENG_PIOA::en_camera1_z_axis_position;
	}
	else
	{
		i32MotorPosZ= (INT32)GetShMemStruct()->r_camera2_z_axis_position_display;
		u32Addr[0]	= (UINT32)ENG_PIOA::en_camera2_z_axis_position_setting;
		u32Addr[1]	= (UINT32)ENG_PIOA::en_camera2_z_axis_position;
	}

	/* 입력된 속도 및 이동 거리 얻기 */
	i32MoveZ	= (INT32)ROUNDED(move * 10000.0f, 0);	/* Moving (mm -> 0.1 um or 100 nm) */
	/* 상대 이동 거리인 경우, 현재 위치에 이동 거리 값만큼 합산해 줌 */
	if (0x01 == method)	/* 0:Absolute, 1:Relative */
	{
		/* 상대 이동할 때, Top 내지 Bottom인지에 따라 음수 여부 결정 */
		if (0x01 == up_down)	i32MotorPosZ += i32MoveZ;	/* UP */
		else					i32MotorPosZ -= i32MoveZ;	/* DOWN */
	}
	else
	{
		i32MotorPosZ	= i32MoveZ;
	}

	/* 최소 값보다 작으면 최소 값만큼 이동 */
	if (i32MotorPosZ < (INT32)(ROUNDED(GetConfig()->set_basler.z_axis_move_min * 10000.0f, 0)))
	{
		i32MotorPosZ	= (INT32)(ROUNDED(GetConfig()->set_basler.z_axis_move_min * 10000.0f, 0));
	}
	/* 최대 값보다 크면 최대 값만큼 이동 */
	if (i32MotorPosZ > (INT32)(ROUNDED(GetConfig()->set_basler.z_axis_move_max * 10000.0f, 0)))
	{
		i32MotorPosZ	= (INT32)(ROUNDED(GetConfig()->set_basler.z_axis_move_max * 10000.0f, 0));
	}

	/* 절대 위치로 이동될 위치 저장 */
	if (!uvEng_MCQ_WriteDWord(u32Addr[0], UINT32(i32MotorPosZ)))	return FALSE;
	/* 설정된 절대 위치로 이동 */
	return uvEng_MCQ_WriteBits(u32Addr[1], 0x01);
}

/*
 desc : Align Camera Z Axis 이동 for All
 parm : move	- [in]  절대 이동 위치 값 (단위: 0.1 um or 100 nm)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_SetACamMoveAbsAllZ(PINT32 move)
{
	UINT i	= 0x00;
	UINT32 u32Addr[2][2]= { { (UINT32)ENG_PIOA::en_camera1_z_axis_position_setting,
							  (UINT32)ENG_PIOA::en_camera1_z_axis_position},
						    { (UINT32)ENG_PIOA::en_camera2_z_axis_position_setting,
							  (UINT32)ENG_PIOA::en_camera2_z_axis_position } };

	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;

	for (; i<0x02; i++)
	{
		/* 최소 값보다 작으면 최소 값만큼 이동 */
		if (move[i] < (INT32)(ROUNDED(GetConfig()->set_basler.z_axis_move_min * 10000.0f, 0)))
		{
			move[i]	= (INT32)(ROUNDED(GetConfig()->set_basler.z_axis_move_min * 10000.0f, 0));
		}
		/* 최대 값보다 크면 최대 값만큼 이동 */
		if (move[i] > (INT32)(ROUNDED(GetConfig()->set_basler.z_axis_move_max * 10000.0f, 0)))
		{
			move[i]	= (INT32)(ROUNDED(GetConfig()->set_basler.z_axis_move_max * 10000.0f, 0));
		}

		/* 절대 위치로 이동될 위치 저장 */
		if (!uvEng_MCQ_WriteDWord(u32Addr[i][0], UINT32(move[i])))	return FALSE;
		/* 설정된 절대 위치로 이동 */
		if (!uvEng_MCQ_WriteBits(u32Addr[i][1], 0x01))	return FALSE;
	}

	return TRUE;
}

/*
 desc : Align Camera Z Axis : Move To Home Position
 parm : cam_id	- [in]  얼라인 카메라 ID (1 or Later)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_SetACamZHomePosZ(UINT8 cam_id)
{
	UINT32 u32Addr	= (UINT32)ENG_PIOA::en_camera1_z_axis_home_position;
	if (0x02 == cam_id)	u32Addr	= (UINT32)ENG_PIOA::en_camera2_z_axis_home_position;

	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;
	if (!uvMCQDrv_IsConnected())	return FALSE;
	return uvEng_MCQ_WriteBits(u32Addr, 0x01);
}

/*
 desc : Align Camera Z Axis : 상태가 준비 모드인지 여부
 parm : cam_id	- [in]  얼라인 카메라 ID (1 or Later)
 retn : TRUE (Ready) or FALSE (Busy)
*/
API_EXPORT BOOL uvCmn_MCQ_IsACamZReady(UINT8 cam_id)
{
	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;
	if (!uvMCQDrv_IsConnected())	return FALSE;
	if (1 == cam_id)	return GetShMemStruct()->r_camera1_z_axis_status ? FALSE : TRUE;
	return GetShMemStruct()->r_camera2_z_axis_status ? FALSE : TRUE;
}

/*
 desc : Align Camera Z Axis : 알람 발생 여부
 parm : cam_id	- [in]  얼라인 카메라 ID (1 or Later)
 retn : TRUE (Alarm) or FALSE (No Alarm)
*/
API_EXPORT BOOL uvCmn_MCQ_IsACamZAlarm(UINT8 cam_id)
{
	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return FALSE;
	if (!uvMCQDrv_IsConnected())	return FALSE;
	if (1 == cam_id)	return GetShMemStruct()->r_camera1_z_axis_alarm ? FALSE : TRUE;
	return GetShMemStruct()->r_camera2_z_axis_alarm ? FALSE : TRUE;
}

/*
 desc : Vacuum 제어 수행
 parm : vacuum	- [in]  0x01: On, 0x00: Off
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_SetVacuumOnOff(UINT8 vacuum)
{
	/* 현재 Start LED 값 얻기 */
	UINT8 u8StartLed= GetShMemStruct()->r_start_button;
	UINT8 u8Shutter	= GetShMemStruct()->r_shutter_status;
	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;
	/* 현재 Shutter나 Vacuum이 Open (On) or Close (Off) 상태가 아닌 경우 */
	if (GetShMemStruct()->r_vacuum_vac_atm_waiting)	return FALSE;

	return WriteLedVacuumShutter(u8StartLed, vacuum, u8Shutter);
}

/*
 desc : Shutter 제어 수행
 parm : shutter	- [in]  0x01: Open, 0x00: Close
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_SetShutterOnOff(UINT8 shutter)
{
	/* 현재 Start LED 값 얻기 */
	UINT8 u8StartLed= GetShMemStruct()->r_start_button;
	UINT8 u8Vacuum	= GetShMemStruct()->r_vacuum_status;
	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;
	/* 현재 Shutter나 Vacuum이 Open (On) or Close (Off) 상태가 아닌 경우 */
	if (GetShMemStruct()->r_shutter_open_close_waiting)	return FALSE;

	return WriteLedVacuumShutter(u8StartLed, u8Vacuum, shutter);
}

/*
 desc : Vacuum과 Shutter를 동시 제어 수행
 parm : vacuum	- [in]  0x01: On, 0x00: Off
		shutter	- [in]  0x01: Open, 0x00: Close
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_SetVacuumShutterOnOff(UINT8 vacuum, UINT8 shutter)
{
	/* 현재 Start LED 값 얻기 */
	UINT8 u8StartLed= GetShMemStruct()->r_start_button;
	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;
	/* 현재 Shutter나 Vacuum이 Open (On) or Close (Off) 상태가 아닌 경우 */
	if (GetShMemStruct()->r_shutter_open_close_waiting ||
		GetShMemStruct()->r_vacuum_vac_atm_waiting)
	{
		return FALSE;
	}
	return WriteLedVacuumShutter(u8StartLed, vacuum, shutter);
}

/*
 desc : Start Lamp / Vacuum / Shutter 제어 
 parm : start_led	- [in]  0x01: On, 0x00: Off
		vacuum		- [in]  0x01: On, 0x00: Off
		shutter		- [in]  0x01: Open, 0x00: Close
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_LedVacuumShutter(UINT8 start_led, UINT8 vacuum, UINT8 shutter)
{
	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;

	/* 현재 Shutter나 Vacuum이 Open (On) or Close (Off) 상태가 아닌 경우 */
	if (GetShMemStruct()->r_shutter_open_close_waiting ||
		GetShMemStruct()->r_vacuum_vac_atm_waiting)
	{
		return FALSE;
	}
	return WriteLedVacuumShutter(start_led, vacuum, shutter);
}

/*
 desc : Vacuum Controller Run or Stop
 parm : run	- [in]  0x01:Run, 0x00:Stop
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_VacuumRunStop(UINT8 run)
{
	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;
	return uvEng_MCQ_WriteBits(UINT32(ENG_PIOA::en_vacuum_controller_run_cmd), run);
}

/*
 desc : Chiller Controller Run or Stop
 parm : run	- [in]  0x01:Run, 0x00:Stop
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_ChillerRunStop(UINT8 run)
{
	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;
	return uvEng_MCQ_WriteBits(UINT32(ENG_PIOA::en_chiller_run_cmd), run);
}

/*
 desc : Trigger Board Reset (Rebooting)
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_TriggerReset()
{
	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;
	return uvEng_MCQ_WriteBits(UINT32(ENG_PIOA::en_trigger_board_power_reset), 0x01);
}

/*
 desc : Start Lamp (Start 램프 신호 명령으로 사용자에게 노광 진행 중임을 알림)
 parm : enable	- [in]  0x00 - Stop, 0x01 - Start
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_StartLampEnable(UINT8 enable)
{
	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;
	return uvEng_MCQ_WriteBits(UINT32(ENG_PIOA::en_start_lamp), enable);
}

/*
 desc : Buzzer 제어 수행
 parm : buzzer	- [in]  0x01: On, 0x00: Off
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_SetBuzzerOnOff(UINT8 buzzer)
{
	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;
	return uvEng_MCQ_WriteBits(UINT32(ENG_PIOA::en_tower_buzzer_1_cmd), buzzer);
}

/*
 desc : Lamp 제어 수행
 parm : type	- [in]  확인 대상 값 즉, 0x00 : Normal, 0x01 : Warn, 0x02 : Alarm
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_SetLamp(UINT8 type)
{
	UINT8 u8BitNo[4]	= {NULL};
	UINT16 u16Value		= 0;
	UINT32 u32Addr		= 0;
	UINT32 u32AddrOrg[4]= { UINT32(ENG_PIOA::en_tower_green_lamp_cmd),
							UINT32(ENG_PIOA::en_tower_yellow_lamp_flashing_cmd),
							UINT32(ENG_PIOA::en_tower_red_lamp_flashing_cmd),
							UINT32(ENG_PIOA::en_tower_buzzer_1_cmd) };
	/* Vacuum과 Shutter 그리고 Start LED 버튼의 Word 주소는 동일 */
	GetAddrWordBits(u32AddrOrg[0], u32Addr, u8BitNo[0]);	/* Green Lamp */
	GetAddrWordBits(u32AddrOrg[1], u32Addr, u8BitNo[1]);	/* Yellow Lamp */
	GetAddrWordBits(u32AddrOrg[2], u32Addr, u8BitNo[2]);	/* Red Lamp */
	GetAddrWordBits(u32AddrOrg[3], u32Addr, u8BitNo[3]);	/* Buzzer 1 */
	/* 입력된 주소 값이 유효한지 여부 확인 */
	if (!IsValidAddr(u32Addr))	return FALSE;	/* Start Lamp, Vacuum, Shutter가 동일 Word 주소 0x4B0~ */
	/* 해당 메모리 영역의 1 Word 값 */
	if (!g_pMemPLC->GetWordValue(u32Addr, u16Value))	return FALSE;	/* Start Lamp, Vacuum, Shutter가 동일 Word 주소 0x4B0~ */
	/* Green, Yellow, Red, Buzzer의 값이 있는 비트 영역 부분을 0 으로 초기화 */
	u16Value	= (u16Value & 0x1E1F);	/* 0001 1110 0001 1111 */
	switch (type)
	{
	case 0x00 : u16Value |= (0x01 << u8BitNo[0]);		break;	/* Green Lamp */
	case 0x01 : u16Value |= (0x01 << u8BitNo[1]);		break;	/* Yellow Lamp */
	case 0x02 : u16Value |= (0x01 << u8BitNo[2]);				/* Red Lamp */
				/*u16Value |= (0x01 << u8BitNo[3]);*/	break;	/* Buzzer On */
	}

	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;

	return uvMCQDrv_WriteWord(ENG_DNTC::en_d, u32Addr, 0x0001, &u16Value);
}

/*
 desc : Safety Reset Command (안전 커버 및 EMS가 눌린 경우, 장비 원복 후 RESET 누를 경우 사용)
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_SafetyReset()
{
	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;
	return uvEng_MCQ_WriteWord(UINT32(ENG_PIOA::en_safety_reset_cmd), 0x0001);
}

/*
 desc : Camera Z Axis Position Speed Setting
 parm : value	- [in]  온도 값 (1500 ~ 2500 즉, 15.00 ℃ ~ 25.00 ℃)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_SetHotAirTempSetting(DOUBLE value)
{
	UINT16 u16Temp	= (UINT16)ROUNDED(value * 100.0f, 0);
	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo() || !GetConfig()->IsCheckHotAir())	return TRUE;
	return uvEng_MCQ_WriteWord(UINT32(ENG_PIOA::en_temp_controller_temp_setting), u16Temp);
}

/* --------------------------------------------------------------------------------------------- */
/*                         외부 함수 - < PLC - Melsec Q > < for Common >                         */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 현재 PLC가 연결되어 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCmn_MCQ_IsConnected()
{
	if (!GetConfig())	return FALSE;
	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;
	return GetShMemPLC()->link->is_connected;
}

/*
 desc : Melsec Q PLC 데이터 정보 반환 (구조체)
 parm : None
 retn : PLC 전체 정보가 저장된 구조체 포인터
*/
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID|| \
	 CUSTOM_CODE_UVDI15 == DELIVERY_PRODUCT_ID)
API_EXPORT LPG_PMRW uvCmn_MCQ_GetShMemStruct()
{
	PUINT8 pMemPLC	= NULL;
	if (!g_pMemPLC)	return NULL;
	pMemPLC	= g_pMemPLC->GetMemMap()->data;
	return LPG_PMRW(pMemPLC);
}
#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
API_EXPORT LPG_PMDV uvCmn_MCQ_GetShMemStruct()
{
	PUINT8 pMemPLC	= NULL;
	if (!g_pMemPLC)	return NULL;
	pMemPLC	= g_pMemPLC->GetMemMap()->data;
	return LPG_PMDV(pMemPLC);
}
#endif

/*
 desc : 가장 최근에 발생된 Error Code (Status) 값 반환
 parm : None
 retn : Error Code 값
*/
API_EXPORT UINT16 uvCmn_MCQ_GetLastErrorCode()
{
	return g_pMemPLC->GetMemMap()->link->u16_error_code;
}

/*
 desc : 가장 최근에 발생된 Error Code (Status) 초기화
 parm : None
 retn : None
*/
API_EXPORT VOID uvCmn_MCQ_ResetLastErrorCode()
{
	g_pMemPLC->GetMemMap()->link->u16_error_code	= 0x000;
}

/*
 desc : MC Protocol - 특정 위치의 Bit 데이터 값 반환
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소) (4 Bytes이지만, 3 Bytes 값만 유효)
 retn : 비트 값 (0 or 1) 반환
*/
API_EXPORT UINT8 uvCmn_MCQ_ReadBitsValue(UINT32 addr)
{
	UINT8 u8BitNo	= 0x00;
	UINT16 u16Value	= 0x0000;
	UINT32 u32Addr	= 0x00000000;

	/* Bit 위치까지 포함된 주소를 Word 주소와 Bit 위치 값 분리하여 반환 */
	GetAddrWordBits(addr, u32Addr, u8BitNo);

	if (!uvMCQDrv_IsConnected())	return 0;
	/* 입력된 주소 값이 유효한지 여부 확인 */
	if (!IsValidAddr(u32Addr))	return 0;
	/* 해당 메모리 영역의 1 Word 값 */
	if (!g_pMemPLC->GetWordValue(u32Addr, u16Value))	return 0;

	/* 특정 위치의 비트 값 반환 */
	return UINT8((u16Value >> u8BitNo) & 0x0001);
}

/*
 desc : MC Protocol - 임의 주소의 Word 데이터 값 반환
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소) (4 Bytes이지만, 3 Bytes 값만 유효)
 retn : Word 값 반환
*/
API_EXPORT UINT16 uvCmn_MCQ_ReadWordValue(UINT32 addr)
{
	UINT8 u8BitNo	= 0x00;
	UINT16 u16Value	= 0x0000;
	UINT32 u32Addr	= 0x00000000;

	/* Bit 위치까지 포함된 주소를 Word 주소와 Bit 위치 값 분리하여 반환 */
	GetAddrWordBits(addr, u32Addr, u8BitNo);

	if (!uvMCQDrv_IsConnected())	return 0;
	/* 입력된 주소 값이 유효한지 여부 확인 */
	if (!IsValidAddr(u32Addr))	return 0;
	/* 해당 메모리 영역의 1 Word 값 */
	if (!g_pMemPLC->GetWordValue(u32Addr, u16Value))	return 0;

	/* 특정 위치의 비트 값 반환 */
	return u16Value;
}

/*
 desc : MC Protocol - 메모리에 저장된 Word 데이터 값 반환
 parm : index	- [in]  Word 단위의 메모리의 인덱스 위치 값 (0 or Later)
 retn : Word 값 반환
*/
API_EXPORT UINT16 uvCmn_MCQ_ReadWordMemory(UINT16 index)
{
	UINT16 u16Value	= 0;

	if (!uvMCQDrv_IsConnected())	return 0;
	/* 해당 메모리 영역의 1 Word 값 */
	if (!g_pMemPLC->GetWordValue(index, u16Value))	return 0;

	/* 특정 위치의 비트 값 반환 */
	return u16Value;
}

/*
 desc : MC Protocol - 임의 주소의 DWord 데이터 값 반환
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소) (4 Bytes이지만, 3 Bytes 값만 유효)
 retn : Word 값 반환
*/
API_EXPORT UINT32 uvCmn_MCQ_ReadDWordValue(UINT32 addr)
{
	UINT8 u8BitNo	= 0x00;
	UINT32 u32Value	= 0x00000000;
	UINT32 u32Addr	= 0x00000000;

	/* Bit 위치까지 포함된 주소를 Word 주소와 Bit 위치 값 분리하여 반환 */
	GetAddrWordBits(addr, u32Addr, u8BitNo);

	if (!uvMCQDrv_IsConnected())	return 0;
	/* 입력된 주소 값이 유효한지 여부 확인 */
	if (!IsValidAddr(u32Addr))	return 0;
	/* 해당 메모리 영역의 1 Word 값 */
	if (!g_pMemPLC->GetDWordValue(u32Addr, u32Value))	return 0;

	/* 특정 위치의 비트 값 반환 */
	return u32Value;
}

/*
 desc : 현재 얼라인 카메라의 Z Axis 높이 값 반환
 parm : cam_id	- [in]  Align Camera Id (Number) (1 or Later)
 retn : Z Axis 높이 값 반환 (단위: mm) (소수점 4자리까지 유효)
*/
API_EXPORT DOUBLE uvCmn_MCQ_GetACamCurrentPosZ(UINT8 cam_id)
{
	INT32 i32PosZ	= 0;

	if (GetConfig()->IsRunDemo())	return 0.0f;
	if (!uvMCQDrv_IsConnected())	return 0.0f;
	if (1 == cam_id)	i32PosZ = GetShMemStruct()->r_camera1_z_axis_position_display;
	else				i32PosZ = GetShMemStruct()->r_camera2_z_axis_position_display;

	return DOUBLE(i32PosZ/10000.0f);
}

/*
 desc : 워크 테이블에 노광 대상 소재가 놓여있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCmn_MCQ_IsMaterialDetected()
{
	if (GetConfig()->IsRunDemo() || !GetConfig()->IsCheckMaterial())	return TRUE;
	/* 소재 감지할지 여부 확인 기능 */
	if (GetConfig()->set_comn.run_material_detect == 0x00)	return TRUE;	/* 소재 감지 기능 사용하지 않으면 무조건 TRUE */
	return GetShMemStruct()->r_glass_detecting_sensor ? TRUE : FALSE;
}

/*
 desc : 장비 전면에 설치된 Start Button 2개에 불이 들어와 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCmn_MCQ_IsStartButtonLedOn()
{
	if (GetConfig()->IsRunDemo() || !GetConfig()->IsCheckStartLed())	return TRUE;
	return GetShMemStruct()->r_start_button ? TRUE : FALSE;
}

/*
 desc : 공조기 설정 온도 값 반환 (현재 값이 아님)
 parm : None
 retn : 온도 값 (1500 ~ 2500 즉, 15.00 ℃ ~ 25.00 ℃)
*/
API_EXPORT DOUBLE uvEng_MCQ_GetHotAirTempSetting()
{
#ifdef _DEBUG
	LPG_PMRW pstPLC	= GetShMemStruct();
#endif
	return GetShMemStruct()->r_temp_controller_current_setting_temp / 100.0f;
}

/*
 desc : 공조기 현재 온도 값
 parm : None
 retn : 온도 값 (1500 ~ 2500 즉, 15.00 ℃ ~ 25.00 ℃)
*/
API_EXPORT DOUBLE uvEng_MCQ_GetHotAirTemp()
{
	return GetShMemStruct()->r_temp_controller_current_temp / 100.0f;
}

/*
 desc : DI 내부 온도 값
 parm : index	- [in]  4 곳 중 1 곳 선택 (0x00 ~ 0x03)
 retn : 온도 값 (-200000 ~ +850000 즉, -200.000 ℃ ~ +850.000 ℃)
*/
API_EXPORT DOUBLE uvEng_MCQ_GetTempDI(UINT8 index)
{
	switch (index)
	{
	case 0x00 :	return GetShMemStruct()->r_internal_monitoring_temp / 1000.0f;
	case 0x01 :	return GetShMemStruct()->r_internal_monitoring_temp_2 / 1000.0f;
	case 0x02 :	return GetShMemStruct()->r_internal_monitoring_temp_3 / 1000.0f;
	case 0x03 :	return GetShMemStruct()->r_internal_monitoring_temp_4 / 1000.0f;
	}
	return 0.0f;
}

/*
 desc : Buzzer 제어 상태가 On인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_IsSetBuzzerOn()
{
	return GetShMemStruct()->r_signal_tower_buzzer_1 ? TRUE : FALSE;
}

/*
 desc : Lamp 제어 상태가 Alarm or Warn or Normal 인지 여부 (적색 / 노란색 깜박임인지 혹은 녹색 점등인지 여부)
 parm : type	- [in]  확인 대상 값 즉, 0x00 : Normal, 0x01 : Warn, 0x02 : Alarm
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_IsSetLampOn(UINT8 type)
{
	UINT8 u8OnOff	= 0x00;
	switch (type)
	{
	case 0x00 : u8OnOff = GetShMemStruct()->r_signal_tower_green_lamp;				break;
	case 0x01 : u8OnOff = GetShMemStruct()->r_signal_tower_yellow_lamp_flashing;	break;
	case 0x02 : u8OnOff = GetShMemStruct()->r_signal_tower_red_lamp_flashing;		break;
	}
	return u8OnOff ? TRUE : FALSE;
}

/*
 desc : 현재 Vacuum Pressure (kPa) 값 반환
 parm : None
 retn : 2 bytes pressure 값 반환 (단위: kPa) (범위: 0 ~ -101 kPa)
*/
API_EXPORT INT16 uvCmn_MCQ_GetVacuumPress()
{
	UINT8 u8BitNo	= 0x00;
	UINT16 u16Value	= 0x0000;
	UINT32 u32Addr	= 0x0000, u32AddrFreq = (UINT32)ENG_PIOA::en_vacuum_controller_current_pressure;

	/* Bit 위치까지 포함된 주소를 Word 주소와 Bit 위치 값 분리하여 반환 */
	GetAddrWordBits(u32AddrFreq, u32Addr, u8BitNo);

	if (!uvMCQDrv_IsConnected())	return 0;
	/* 입력된 주소 값이 유효한지 여부 확인 */
	if (!IsValidAddr(u32Addr))	return 0;
	/* 해당 메모리 영역의 1 Word 값 */
	if (!g_pMemPLC->GetWordValue(u32Addr, u16Value))	return 0;

	/* 특정 위치의 비트 값 반환 */
	return (INT16)u16Value;
}

/*
 desc : 현재 Vacuum Frequency (Hz) 값 반환
 parm : None
 retn : 2 bytes frequency 값 반환 (단위: Hz) (범위: 0 ~ -101 hz)
*/
API_EXPORT INT16 uvCmn_MCQ_GetVacuumFreq()
{
	UINT8 u8BitNo	= 0x00;
	UINT16 u16Value	= 0x0000;
	UINT32 u32Addr	= 0x0000, u32AddrFreq = (UINT32)ENG_PIOA::en_vacuum_controller_current_frequnency;

	/* Bit 위치까지 포함된 주소를 Word 주소와 Bit 위치 값 분리하여 반환 */
	GetAddrWordBits(u32AddrFreq, u32Addr, u8BitNo);

	if (!uvMCQDrv_IsConnected())	return 0;
	/* 입력된 주소 값이 유효한지 여부 확인 */
	if (!IsValidAddr(u32Addr))	return 0;
	/* 해당 메모리 영역의 1 Word 값 */
	if (!g_pMemPLC->GetWordValue(u32Addr, u16Value))	return 0;

	/* 특정 위치의 비트 값 반환 */
	return (INT16)u16Value;
}


#ifdef __cplusplus
}
#endif