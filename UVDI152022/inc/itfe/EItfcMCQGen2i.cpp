
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
	/* Bit 위치까지 포함된 주소를 Word 주소와 Bit 위치 값 분리하여 반환 */
	GetAddrWordBits(addr, u32Addr, u8BitNo);
	/* 입력된 주소 값이 유효한지 여부 확인 */
	if (!IsValidAddr(u32Addr))	return FALSE;

	/* WORD 2 개에 나눠서 저장 */
	memcpy(u16Value, &value, 4);

	if (GetConfig()->set_comn.comm_log_plc)
	{
		TCHAR tzMesg[128]	= {NULL};
		swprintf_s(tzMesg, 128,
					L"PODIS vs. PLC [Set] [called_func=uvEng_MCQ_WriteDWord]"
					L"[addr=%08x, value=%u]", addr, value);
		LOG_MESG(ENG_EDIC::en_engine, tzMesg);
	}

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
	UINT8 u8BitNo;
	UINT32 u32Addr;

	/* 데모 동작 모드인지 여부에 따라 */
	if (GetConfig()->IsRunDemo())	return TRUE;
	/* Bit 위치까지 포함된 주소를 Word 주소와 Bit 위치 값 분리하여 반환 */
	GetAddrWordBits(addr, u32Addr, u8BitNo);

	/* 입력된 주소 값이 유효한지 여부 확인 */
	if (!IsValidAddr(u32Addr))	return FALSE;

	if (GetConfig()->set_comn.comm_log_plc)
	{
		TCHAR tzMesg[128]	= {NULL};
		swprintf_s(tzMesg, 128,
					L"PODIS vs. PLC [Set] [called_func=uvEng_MCQ_WriteWord]"
					L"[addr=%08x, value=%u]", addr, value);
		LOG_MESG(ENG_EDIC::en_engine, tzMesg);
	}

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
	/* Bit 위치까지 포함된 주소를 Word 주소와 Bit 위치 값 분리하여 반환 */
	GetAddrWordBits(addr, u32Addr, u8BitNo);

	/* 입력된 주소 값이 유효한지 여부 확인 */
	if (!IsValidAddr(u32Addr))	return FALSE;
	/* 해당 메모리 영역의 1 Word 값 */
	if (!g_pMemPLC->GetWordValue(u32Addr, u16Value))		return FALSE;

	/* 특정 위치의 비트 값 변경 */
	if (value)	u16Value	|= (1 << u8BitNo);	/* 특정 위치의 비트 값을 1 로 변경 */
	else		u16Value	&= ~(1 << u8BitNo);	/* 특정 위치의 비트 값을 0 로 변경 */

	if (GetConfig()->set_comn.comm_log_plc)
	{
		TCHAR tzMesg[128]	= {NULL};
		swprintf_s(tzMesg, 128,
					L"PODIS vs. PLC [Set] [called_func=uvEng_MCQ_WriteBits]"
					L"[addr=%08x, value=%u]", addr, value);
		LOG_MESG(ENG_EDIC::en_engine, tzMesg);
	}

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
	/* Bit 위치까지 포함된 주소를 Word 주소와 Bit 위치 값 분리하여 반환 */
	GetAddrWordBits(addr, u32Addr, u8BitNo);

	/* 입력된 주소 값이 유효한지 여부 확인 */
	if (!IsValidAddr(u32Addr))	return FALSE;
	/* 해당 메모리 영역의 1 Word 값 */
	if (!g_pMemPLC->GetWordValue(u32Addr, u16Value))		return FALSE;

	/* 특정 위치의 비트 값 반전 후 변경 */
	u16Value	^= (1 << u8BitNo);	/* 1 -> 0, 0 -> 1*/

	if (GetConfig()->set_comn.comm_log_plc)
	{
		TCHAR tzMesg[128]	= {NULL};
		swprintf_s(tzMesg, 128,
					L"PODIS vs. PLC [Set] [called_func=uvEng_MCQ_WriteBitsInvert]"
					L"[addr=%08x, value=%u]", addr, u16Value);
		LOG_MESG(ENG_EDIC::en_engine, tzMesg);
	}

	return uvMCQDrv_WriteWord(ENG_DNTC::en_d, u32Addr, 0x0001, &u16Value);
}

/*
 desc : MC Protocol - 특정 위치의 Bit 데이터 반전 후 쓰기
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소)
		value	- [in]  Bit 1개 값이 저장된 버퍼
 retn : TRUE or FALSE (에러 코드가 저장됨 ?)
*/
API_EXPORT BOOL uvEng_MCQ_WriteBitsInvertEx(ENG_PIOA addr)
{
	return uvEng_MCQ_WriteBitsInvert(UINT32(addr));
}


/* --------------------------------------------------------------------------------------------- */
/*                         외부 함수 - < PLC - Melsec Q > < for Common >                         */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Check whether the current PLC is connected
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCmn_MCQ_IsConnected()
{
	return GetShMemPLC()->link->is_connected;
}

/*
 desc : Returns the most recently occurred Error Code (Status) value.
 parm : None
 retn : Error Code 값
*/
API_EXPORT UINT16 uvCmn_MCQ_GetLastErrorCode()
{
	return g_pMemPLC->GetMemMap()->link->u16_error_code;
}

/*
 desc : Initialize the most recently occurred Error Code (Status)
 parm : None
 retn : None
*/
API_EXPORT VOID uvCmn_MCQ_ResetLastErrorCode()
{
	g_pMemPLC->GetMemMap()->link->u16_error_code	= 0x000;
}

/*
 desc : MC Protocol - 임의 위치의 Bit 데이터 값 반환
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소) (4 Bytes이지만, 3 Bytes 값만 유효)
 retn : 비트 값 (0 or 1) 반환
*/
API_EXPORT UINT8 uvCmn_MCQ_GetBitsValue(UINT32 addr)
{
	UINT8 u8BitNo	= 0x00;
	UINT16 u16Value	= 0x0000;
	UINT32 u32Addr	= 0x00000000;

	/* Bit 위치까지 포함된 주소를 Word 주소와 Bit 위치 값 분리하여 반환 */
	GetAddrWordBits(addr, u32Addr, u8BitNo);
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
API_EXPORT UINT16 uvCmn_MCQ_GetWordValue(UINT32 addr)
{
	UINT8 u8BitNo	= 0x00;
	UINT16 u16Value	= 0x0000;
	UINT32 u32Addr	= 0x00000000;

	/* Bit 위치까지 포함된 주소를 Word 주소와 Bit 위치 값 분리하여 반환 */
	GetAddrWordBits(addr, u32Addr, u8BitNo);
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
API_EXPORT UINT16 uvCmn_MCQ_GetWordMemory(UINT16 index)
{
	UINT16 u16Value	= 0;

	/* 해당 메모리 영역의 1 Word 값 */
	if (!g_pMemPLC->GetWordValue(index, u16Value))	return 0;

	/* 특정 위치의 비트 값 반환 */
	return u16Value;
}

/*
 desc : MC Protocol - 메모리에 저장된 DWord 데이터 값 반환
 parm : index	- [in]  Word 단위의 메모리의 인덱스 위치 값 (0 or Later)
 retn : Word 값 반환
*/
API_EXPORT UINT32 uvCmn_MCQ_GetDWordMemory(UINT16 index)
{
	UINT32 u32Value	= 0;

	/* 해당 메모리 영역의 1 Word 값 */
	if (!g_pMemPLC->GetDWordValue(index, u32Value))	return 0;

	/* 특정 위치의 비트 값 반환 */
	return u32Value;
}

/*
 desc : MC Protocol - 임의 주소의 DWord 데이터 값 반환
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소) (4 Bytes이지만, 3 Bytes 값만 유효)
 retn : Word 값 반환
*/
API_EXPORT UINT32 uvCmn_MCQ_GetDWordValue(UINT32 addr)
{
	UINT8 u8BitNo	= 0x00;
	UINT32 u32Value	= 0x00000000;
	UINT32 u32Addr	= 0x00000000;

	/* Bit 위치까지 포함된 주소를 Word 주소와 Bit 위치 값 분리하여 반환 */
	GetAddrWordBits(addr, u32Addr, u8BitNo);
	/* 입력된 주소 값이 유효한지 여부 확인 */
	if (!IsValidAddr(u32Addr))	return 0;
	/* 해당 메모리 영역의 1 Word 값 */
	if (!g_pMemPLC->GetDWordValue(u32Addr, u32Value))	return 0;

	/* 특정 위치의 비트 값 반환 */
	return u32Value;
}

/* --------------------------------------------------------------------------------------------- */
/*           !!! C# 쪽에 전달을 하기 위한 목적으로 만들어진 외부 라이브러리 함수 임 !!!          */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : PLC에 1 개 이상의 알람 값이 존재하는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCmn_MCQ_IsAlarm()
{
	PUINT64 pAlarm	= NULL;
	if (!g_pMemPLC)	return FALSE;

	/* Alarm 영역 4 Word 즉, 8 Bytes 값 가져오기 */
	pAlarm	= (PUINT64)g_pMemPLC->GetMemMap()->data;
	/* 8 Bytes 영역에 0 값이 1 개 이상 존재하면, <알람> 임 */
	return (*pAlarm == 0xffffffffffffffff);

#if 0
	LPG_PMDV pstMem	= NULL;
	pstMem	= (LPG_PMDV)g_pMemPLC->GetMemMap()->data;
#endif
}

/*
 desc : PLC에 1 개 이상의 경고 값이 존재하는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCmn_MCQ_IsWarn()
{
	PUINT32 pWarn	= NULL;
	if (!g_pMemPLC)	return FALSE;

	/* Alarm 영역 4 Word 즉, 8 Bytes 값 가져오기 */
	pWarn	= (PUINT32)g_pMemPLC->GetMemMap()->data;
	pWarn	+= 2;	/* skip 8 bytes (sizeof(UINT32) * 2) */
	/* 4 Bytes 영역에 0 값이 1 개 이상 존재하면, <경고> 임 */
	return (*pWarn == 0xffffffff);

#if 0
	LPG_PMDV pstMem	= NULL;
	pstMem	= (LPG_PMDV)g_pMemPLC->GetMemMap()->data;
#endif
}

/*
 desc : MC Protocol - 임의 위치의 Bit 데이터 값 반환
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소)
 retn : 비트 값 (0 or 1) 반환
*/
API_EXPORT UINT8 uvCmn_MCQ_GetBitsValueEx(ENG_PIOA addr)
{
	return uvCmn_MCQ_GetBitsValue(UINT32(addr));
}

/*
 desc : MC Protocol - 특정 위치의 Bit 데이터 쓰기
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소)
		value	- [in]  Bit 1개 값이 저장된 버퍼
 retn : TRUE or FALSE (에러 코드가 저장됨 ?)
*/
API_EXPORT BOOL uvEng_MCQ_WriteBitsValueEx(ENG_PIOA addr, UINT8 value)
{
	return uvEng_MCQ_WriteBits(UINT32(addr), value);
}

/*
 desc : MC Protocol - 임의 주소의 Word 데이터 값 반환
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소)
 retn : Word 값 반환
*/
API_EXPORT UINT16 uvCmn_MCQ_GetWordValueEx(ENG_PIOA addr)
{
	return uvCmn_MCQ_GetWordValue(UINT32(addr));
}

/*
 desc : MC Protocol - Word 데이터 쓰기
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소)
		value	- [in]  Word 1개 값이 저장된 버퍼
 retn : TRUE or FALSE (에러 코드가 저장됨 ?)
*/
API_EXPORT BOOL uvEng_MCQ_WriteWordValueEx(ENG_PIOA addr, UINT16 value)
{
	return uvEng_MCQ_WriteWord(UINT32(addr), value);
}

/*
 desc : MC Protocol - 임의 주소의 DWord 데이터 값 반환
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소)
 retn : Word 값 반환
*/
API_EXPORT UINT32 uvCmn_MCQ_GetDWordValueEx(ENG_PIOA addr)
{
	return uvCmn_MCQ_GetDWordValue(UINT32(addr));
}

/*
 desc : MC Protocol - DWord 데이터 쓰기
 parm : addr	- [in]  디바이스 시작 주소 값 (bit 위치까지 포함된 주소)
		value	- [in]  DWord 1개 값이 저장된 버퍼
 retn : TRUE or FALSE (에러 코드가 저장됨 ?)
*/
API_EXPORT BOOL uvEng_MCQ_WriteDWordValueEx(ENG_PIOA addr, UINT32 value)
{
	return uvEng_MCQ_WriteDWord(UINT32(addr), value);
}

/* --------------------------------------------------------------------------------------------- */
/*                      외부 Utility - < PLC - Melsec Q > < for C# or GUI >                      */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : MC Protocol - Tower (Lamp On or Off) or (All On or OFF)
 parm : color	- [in]  0x01 : green, 0x02 : yellow, 0x03 : red, 0x04 : all (ENG_TLI)
		power	- [in]  0x00 : OFF, 0x01 : ON
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_WriteTowerLampOnOff(ENG_TLCI color, UINT8 power)
{
	UINT16 u16Value	= 0x0000;
	ENG_PIOA enAddr	= ENG_PIOA::en_safety_reset_cmd;

	/* 해당 메모리 영역의 1 Word 값 가져오기 */
	if (!g_pMemPLC->GetWordValue(enAddr, u16Value))	return FALSE;

	/* Tower Lamp Green:1, Yellow:2, Red:3, Buzzer:4 */
	if (power)
	{
		switch (color)
		{
		case ENG_TLCI::en_green	:	u16Value |= 0x0002;	/* 0000 0000 0000 0010 */	break;
		case ENG_TLCI::en_yellow:	u16Value |= 0x0004;	/* 0000 0000 0000 0100 */	break;
		case ENG_TLCI::en_red	:	u16Value |= 0x0018;	/* 0000 0000 0001 1000 */	break;	/* Included Buzzer */
		case ENG_TLCI::en_all	:	u16Value |= 0x0018;	/* 0000 0000 0000 1110 */	break;	/* Not included Buzzer */
		}
	}
	else
	{
		switch (color)
		{
		case ENG_TLCI::en_green	:	u16Value &= 0xfffd;	/* 1111 1111 1111 1101 */	break;
		case ENG_TLCI::en_yellow:	u16Value &= 0xfffb;	/* 1111 1111 1111 1011 */	break;
		case ENG_TLCI::en_red	:	u16Value &= 0xffe7;	/* 1111 1111 1110 0111 */	break;	/* Included Buzzer */
		case ENG_TLCI::en_all	:	u16Value &= 0xffe1;	/* 1111 1111 1110 0001 */	break;	/* Included Buzzer */
		}
	}

	return uvEng_MCQ_WriteWord(UINT32(enAddr), u16Value);
}

/*
 desc : MC Protocol - Photohead Z Axis <Cooling> All On or Off
 parm : power	- [in]  0x00 : OFF, 0x01 : ON
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_WritePhZAxisCoolingAll(UINT8 power)
{
	UINT16 u16Value	= 0x0000;
	ENG_PIOA enAddr	= ENG_PIOA::en_safety_reset_cmd;

	/* 해당 메모리 영역의 1 Word 값 가져오기 */
	if (!g_pMemPLC->GetWordValue(enAddr, u16Value))	return FALSE;

	/* The bit position of PH Z Axis Cooling : 5, 6, 7 */
	if (power)	u16Value |= 0x00E0;	/* 0000 0000 1110 0000 */
	else		u16Value &= 0xFF1F;	/* 1111 1111 0001 1111 */

	return uvEng_MCQ_WriteWord(UINT32(enAddr), u16Value);
}

/*
 desc : MC Protocol - Photohead Z Axis <Cylinder> All On or Off
 parm : power	- [in]  0x00 : OFF, 0x01 : ON
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_WritePhZAxisCyclinderAll(UINT8 power)
{
	UINT16 u16Value	= 0x0000;
	ENG_PIOA enAddr	= ENG_PIOA::en_safety_reset_cmd;

	/* 해당 메모리 영역의 1 Word 값 가져오기 */
	if (!g_pMemPLC->GetWordValue(enAddr, u16Value))	return FALSE;

	/* The bit position of PH Z Axis Cylinder : 8, 9, A */
	if (power)	u16Value |= 0x0700;	/* 0000 0111 0000 0000 */
	else		u16Value &= 0xF8FF;	/* 1111 1000 1111 1111 */

	return uvEng_MCQ_WriteWord(UINT32(enAddr), u16Value);
}

/*
 desc : MC Protocol - Vacuum On or Off
 parm : chuck	- [in]  Wafer Chuck Vacuum (0x00 : OFF, 0x01 : ON)
		robot	- [in]  Robot Vacuum (0x00 : OFF, 0x01 : ON)
		aligner	- [in]  Aligner Vacuum (0x00 : OFF, 0x01 : ON)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_WriteVacuumControl(UINT8 chuck, UINT8 robot, UINT8 aligner)
{
	UINT16 u16Value	= 0x0000;
	ENG_PIOA enAddr	= ENG_PIOA::en_safety_reset_cmd;

	/* 해당 메모리 영역의 1 Word 값 가져오기 */
	if (!g_pMemPLC->GetWordValue(enAddr, u16Value))	return FALSE;

	/* Wafer Chuck (11) */
	if (chuck)	u16Value |= 0x0800;	/* 0000 1000 0000 0000 */
	else		u16Value &= 0xf7ff;	/* 1111 0111 1111 1111 */
	/* Robot (13) */	 
	if (robot)	u16Value |= 0x2000;	/* 0010 0000 0000 0000 */
	else		u16Value &= 0xdfff;	/* 1101 1111 1111 1111 */
	/* Aligner (14) */	 
	if (aligner)u16Value |= 0x4000;	/* 0100 0000 0000 0000 */
	else		u16Value &= 0xbfff;	/* 1011 1111 1111 1111 */

	return uvEng_MCQ_WriteWord(UINT32(enAddr), u16Value);
}

/*
 desc : MC Protocol - Photohead Power On or Off (특수하게 처리해야 됨)
 parm : ph1_power	- [in]  Photohead 1st Power (0x00 : OFF, 0x01 : ON)
		ph2_power	- [in]  Photohead 2nd Power (0x00 : OFF, 0x01 : ON)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_WritePhPower(UINT8 ph1_power, UINT8 ph2_power)
{
	UINT16 u16Value	= 0x0000;
	ENG_PIOA enAddr1= ENG_PIOA::en_safety_reset_cmd;
	ENG_PIOA enAddr2= ENG_PIOA::en_photohead_2_power_off_cmd;

	/* 해당 메모리 영역의 1 Word 값 가져오기 */
	if (!g_pMemPLC->GetWordValue(enAddr1, u16Value))	return FALSE;
	/* PH [1] Power (15) (!!! 다른 것과 반대로 처리해야 됨 !!!) */
	if (ph1_power)	u16Value &= 0x7fff;	/* 0111 1111 1111 1111 (ON) */
	else			u16Value |= 0x8000;	/* 1000 0000 0000 0000 (OFF) */
	if (!uvEng_MCQ_WriteWord(UINT32(enAddr1), u16Value))	return FALSE;

	/* 해당 메모리 영역의 1 Word 값 가져오기 */
	if (!g_pMemPLC->GetWordValue(enAddr2, u16Value))	return FALSE;
	/* PH [1] Power (15) (!!! 다른 것과 반대로 처리해야 됨 !!!) */
	if (ph2_power)	u16Value &= 0xfffe;	/* 1111 1111 1111 1110 (ON) */
	else			u16Value |= 0x0001;	/* 0000 0000 0000 0001 (OFF) */
	return uvEng_MCQ_WriteWord(UINT32(enAddr2), u16Value);
}

/*
 desc : MC Protocol - MC2 Power On or Off (특수하게 처리해야 됨)
 parm : power	- [in]  MC2 Power (0x00 : OFF, 0x01 : ON)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_WriteMC2Power(UINT8 power)
{
	UINT16 u16Value	= 0x0000;
	ENG_PIOA enAddr	= ENG_PIOA::en_xy_stage_move_prohibit;

	/* 해당 메모리 영역의 1 Word 값 가져오기 */
	if (!g_pMemPLC->GetWordValue(enAddr, u16Value))	return FALSE;
	/* MC2 Power (!!! 다른 것과 반대로 처리해야 됨 !!!) */
	if (power)	u16Value &= 0xfffb;	/* 1111 1111 1111 1011 (ON) */
	else		u16Value |= 0x0004;	/* 0000 0000 0000 0100 (OFF) */

	return uvEng_MCQ_WriteWord(UINT32(enAddr), u16Value);
}

/*
 desc : MC Protocol - Light On or Off for Illumination
 parm : None
 retn : TRUE or FALSE
 note : It is performed internally in reverse
*/
API_EXPORT BOOL uvEng_MCQ_WriteLightIllumInvert()
{
	UINT16 u16Value	= 0x0000;
	ENG_PIOA enAddr	= ENG_PIOA::en_photohead_2_power_off_cmd;

	/* 해당 메모리 영역의 1 Word 값 가져오기 */
	if (!g_pMemPLC->GetWordValue(enAddr, u16Value))	return FALSE;

	/* Illumination Sensor Power (1) */
	/* 이 값은 항상 1 값으로만  설정 */
	/* PLC가 처리 후 다시 0 값으로 변경 */
	u16Value |= 0x0002;	/* 0000 0000 0000 0010 */

	return uvEng_MCQ_WriteWord(UINT32(enAddr), u16Value);
}

/*
 desc : MC Protocol - Light On or Off for Halogen
 parm : h_ring	- [in]  Halogen Ring Power (0x00 : OFF, 0x01 : ON)
		h_coax	- [in]  Halogen Coaxial Power (0x00 : OFF, 0x01 : ON)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_WriteLightControl(UINT8 h_ring, UINT8 h_coax)
{
	UINT16 u16Value	= 0x0000;
	ENG_PIOA enAddr	= ENG_PIOA::en_photohead_2_power_off_cmd;

	/* 해당 메모리 영역의 1 Word 값 가져오기 */
	if (!g_pMemPLC->GetWordValue(enAddr, u16Value))	return FALSE;

	/* Halogen Ring (2) */
	if (h_ring)	u16Value |= 0x0004;	/* 0000 0000 0000 0100 */
	else		u16Value &= 0xfffb;	/* 1111 1111 1111 1011 */
	/* Halogen Coaxial (3) */
	if (h_coax)	u16Value |= 0x0008;	/* 0000 0000 0000 1000 */
	else		u16Value &= 0xfff7;	/* 1111 1111 1111 0111 */

	return uvEng_MCQ_WriteWord(UINT32(enAddr), u16Value);
}

/*
 desc : MC Protocol - XY Stage Move Prohibit Release or OFF
 parm : release	- [in]  0x00 : OFF, 0x01 : RELEASE
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_WriteXYStageMoveProhibitRelease(UINT8 release)
{
	UINT16 u16Value	= 0x0000;
	ENG_PIOA enAddr	= ENG_PIOA::en_xy_stage_move_prohibit;

	/* 해당 메모리 영역의 1 Word 값 가져오기 */
	if (!g_pMemPLC->GetWordValue(enAddr, u16Value))	return FALSE;

	/* The bit position of xy stage move prohibit release (1) */
	if (release)	u16Value |= 0x0002;	/* 0000 0000 0000 0010 */
	else			u16Value &= 0xfffd;	/* 1111 1111 1111 1101 */

	return uvEng_MCQ_WriteWord(UINT32(enAddr), u16Value);
}

/*
 desc : MC Protocol - Linked.Litho : EXP-INK ~ EXP-RDY (Exposure Inline ~ Exposure Receive To Ready)
 parm : type	- [in]  exp-ink (0x01), exp-rej (0x02), exp-snd (0x03), exp-rdy (0x04)
		mode	- [in]  0x00 : reset, 0x01 : set
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_WriteLithoExposureToTrack(ENG_LLET type, UINT8 mode)
{
	UINT16 u16Value	= 0x0000;
	ENG_PIOA enAddr	= ENG_PIOA::en_exp_ink_exposure_inline;

	/* 해당 메모리 영역의 1 Word 값 가져오기 */
	if (!g_pMemPLC->GetWordValue(enAddr, u16Value))	return FALSE;

	switch (type)
	{
	/* The bit position of exp-ink (exposure_inline) (0) */
	case ENG_LLET::en_exp_ink	:
		if (mode)	u16Value |= 0x0001;	/* 0000 0000 0000 0001 */
		else		u16Value &= 0xfffe;	/* 1111 1111 1111 1110 */
		break;
	/* The bit position of exp-rej (exposure_reject) (1) */
	case ENG_LLET::en_exp_rej	:
		if (mode)	u16Value |= 0x0002;	/* 0000 0000 0000 0010 */
		else		u16Value &= 0xfffd;	/* 1111 1111 1111 1101 */
		break;
	/* The bit position of exp-snd (exposure_send_to_ready) (2) */
	case ENG_LLET::en_exp_snd	:
		if (mode)	u16Value |= 0x0004;	/* 0000 0000 0000 0100 */
		else		u16Value &= 0xfffb;	/* 1111 1111 1111 1011 */
		break;
	/* The bit position of exp-rdy (exposure_recv_to_ready) (3) */
	case ENG_LLET::en_exp_rdy	:
		if (mode)	u16Value |= 0x0008;	/* 0000 0000 0000 1000 */
		else		u16Value &= 0xfff7;	/* 1111 1111 1111 0111 */
		break;
	}

	return uvEng_MCQ_WriteWord(UINT32(enAddr), u16Value);
}

/*
 desc : Halogen Light Power (소숫점 2자리까지 유효)
 parm : type	- [in]  0x00 : Ring, 0x01 : Coaxial
		volt	- [in]  Light Power (0.00 ~ 12.00 volt)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_WriteLightPowerVolt(ENG_HLPT type, DOUBLE volt)
{
	/* 입력된 값이 0 ~ 12.00 범주에 속하지 않으면 에러 처리 */
	if (volt < 0.0f || volt > 12.0f)
	{
		LOG_ERROR(ENG_EDIC::en_engine,
				  L"The entered Light Power value is not valid (valid range : 0.00 ~ 12.00 volt)");
		return FALSE;
	}
	UINT16 u16Value	= (UINT16)ROUNDED(volt / 0.12f, 0);
	ENG_PIOA enAddr	= ENG_PIOA::en_halogen_ring_light_power;
	if (ENG_HLPT::en_coaxial == type)	enAddr = ENG_PIOA::en_halogen_coaxial_light_power;

	return uvEng_MCQ_WriteWord(UINT32(enAddr), u16Value);
}

/*
 desc : Lift Pin or BSA Axis 제어
 parm : type	- [in]  0:en_pin_x_axis, 1:en_pin_z_axis, 2:en_bas_z_axis
		mode	- [in]  Control Mode (1:Enable or Disable,2:Reset, 3:Stop)
		value	- [in]  0x00 (Disable or OFF)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_WriteLiftPinAxisControl(ENG_LPAT type, ENG_LPAC mode, UINT8 value)
{
	UINT16 u16Value	= 0x0000;
	ENG_PIOA enAddr	= ENG_PIOA::en_lift_pin_xo_axis_enable_disable;

	switch (type)
	{
	case ENG_LPAT::en_pin_x_axis: enAddr = ENG_PIOA::en_lift_pin_xo_axis_enable_disable;	break;
	case ENG_LPAT::en_pin_z_axis: enAddr = ENG_PIOA::en_lift_pin_zo_axis_enable_disable;	break;
	case ENG_LPAT::en_bsa_z_axis: enAddr = ENG_PIOA::en_bsa_camera_zo_axis_enable_disable;	break;
	default						: return FALSE;
	}

	/* 해당 메모리 영역의 1 Word 값 가져오기 */
	if (!g_pMemPLC->GetWordValue(enAddr, u16Value))	return FALSE;

	/* 1 or 0 */
	if (value)
	{
		switch (mode)
		{
		case ENG_LPAC::en_mode	: u16Value |= 0x0001;	break;	/* 0000 0000 0000 0001 */
		case ENG_LPAC::en_reset	: u16Value |= 0x0002;	break;	/* 0000 0000 0000 0010 */
		case ENG_LPAC::en_stop	: u16Value |= 0x0004;	break;	/* 0000 0000 0000 0100 */
		default					: return FALSE;
		}
	}
	else
	{
		switch (mode)
		{
		case ENG_LPAC::en_mode	: u16Value &= 0xfffe;	break;	/* 1111 1111 1111 1110 */
		case ENG_LPAC::en_reset	: u16Value &= 0xfffd;	break;	/* 1111 1111 1111 1101 */
		case ENG_LPAC::en_stop	: u16Value &= 0xfffb;	break;	/* 1111 1111 1111 1011 */
		default					: return FALSE;
		}
	}

	return uvEng_MCQ_WriteWord(UINT32(enAddr), u16Value);
}

/*
 desc : Lift Pin or BSA Axis 동작 처리
 parm : type	- [in]  0:en_pin_x_axis, 1:en_pin_z_axis, 2:en_bas_z_axis
		mode	- [in]  Action Mode (1:jog+move, 2:jog-move, 3:home position, 4:position, 5:alarm reset)
		value	- [in]  0x00 (Disable or OFF)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_WriteLiftPinAxisAction(ENG_LPAT type, ENG_LPAA mode, UINT8 value)
{
	UINT16 u16Value	= 0x0000;
	ENG_PIOA enAddr	= ENG_PIOA::en_lift_pin_xo_axis_enable_disable;

	switch (type)
	{
	case ENG_LPAT::en_pin_x_axis: enAddr = ENG_PIOA::en_lift_pin_xo_axis_enable_disable;	break;
	case ENG_LPAT::en_pin_z_axis: enAddr = ENG_PIOA::en_lift_pin_zo_axis_enable_disable;	break;
	case ENG_LPAT::en_bsa_z_axis: enAddr = ENG_PIOA::en_bsa_camera_zo_axis_enable_disable;	break;
	default						: return FALSE;
	}

	/* 해당 메모리 영역의 1 Word 값 가져오기 */
	if (!g_pMemPLC->GetWordValue(enAddr, u16Value))	return FALSE;

	/* 1 or 0 */
	if (value)
	{
		switch (mode)
		{
		case ENG_LPAA::en_jog_move_plus	: u16Value |= 0x0010;	break;	/* 0000 0000 0001 0000 */
		case ENG_LPAA::en_jog_move_minus: u16Value |= 0x0020;	break;	/* 0000 0000 0010 0000 */
		case ENG_LPAA::en_home_position	: u16Value |= 0x0040;	break;	/* 0000 0000 0100 0000 */
		case ENG_LPAA::en_move_position	: u16Value |= 0x0080;	break;	/* 0000 0000 1000 0000 */
		case ENG_LPAA::en_alarm_reset	: u16Value |= 0x0100;	break;	/* 0000 0001 0000 0000 */
		default							: return FALSE;
		}
	}
	else
	{
		switch (mode)
		{
		case ENG_LPAA::en_jog_move_plus	: u16Value &= 0xffef;	break;	/* 1111 1111 1110 1111 */
		case ENG_LPAA::en_jog_move_minus: u16Value &= 0xffdf;	break;	/* 1111 1111 1101 1111 */
		case ENG_LPAA::en_home_position	: u16Value &= 0xffbf;	break;	/* 1111 1111 1011 1111 */
		case ENG_LPAA::en_move_position	: u16Value &= 0xff7f;	break;	/* 1111 1111 0111 1111 */
		case ENG_LPAA::en_alarm_reset	: u16Value &= 0xfeff;	break;	/* 1111 1110 1111 1111 */
		default							: return FALSE;
		}
	}

	return uvEng_MCQ_WriteWord(UINT32(enAddr), u16Value);
}

/*
 desc : Lift Pin or BSA Axis : Jog Speed 설정
 parm : type	- [in]  0:en_pin_x_axis, 1:en_pin_z_axis, 2:en_bas_z_axis
		value	- [in]  Jog Speed Value (unit : mm/min)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_WriteLiftPinAxisJogSpeed(ENG_LPAT type, DOUBLE value)
{
	BOOL bValid		= FALSE;
	UINT32 u32Value	= 0x00000000;
	ENG_PIOA enAddr	= ENG_PIOA::en_lift_pin_xo_axis_jog_speed_setting;

	switch (type)
	{
	case ENG_LPAT::en_pin_x_axis: enAddr = ENG_PIOA::en_lift_pin_xo_axis_jog_speed_setting;		break;
	case ENG_LPAT::en_pin_z_axis: enAddr = ENG_PIOA::en_lift_pin_zo_axis_jog_speed_setting;		break;
	case ENG_LPAT::en_bsa_z_axis: enAddr = ENG_PIOA::en_bsa_camera_zo_axis_jog_speed_setting;	break;
	default						: return FALSE;
	}

	/* 속도 값 계산 */
	u32Value = (UINT32)ROUNDED(value / 0.01f, 0);
	/* 속도 범위에 유효한지 확인 */
	switch (type)
	{
	case ENG_LPAT::en_pin_x_axis:
	case ENG_LPAT::en_pin_z_axis:	bValid = (u32Value > 0 && u32Value <= 60000);	break;
	case ENG_LPAT::en_bsa_z_axis:	bValid = (u32Value > 0 && u32Value <= 10000);	break;
	}

	if (!bValid)
	{
		LOG_ERROR(ENG_EDIC::en_engine,
				  L"Pin Axis Jog Speed Settings : Out of speed range value.");
		return FALSE;
	}

	return uvEng_MCQ_WriteDWord(UINT32(enAddr), u32Value);
}

/*
 desc : Lift Pin or BSA Axis : Position Speed 설정
 parm : type	- [in]  0:en_pin_x_axis, 1:en_pin_z_axis, 2:en_bas_z_axis
		value	- [in]  Jog Speed Value (unit : mm/min)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_WriteLiftPinAxisPosSpeed(ENG_LPAT type, DOUBLE value)
{
	BOOL bValid		= FALSE;
	UINT32 u32Value	= 0x00000000;
	ENG_PIOA enAddr	= ENG_PIOA::en_lift_pin_xo_axis_position_speed_setting;

	switch (type)
	{
	case ENG_LPAT::en_pin_x_axis: enAddr = ENG_PIOA::en_lift_pin_xo_axis_position_speed_setting;	break;
	case ENG_LPAT::en_pin_z_axis: enAddr = ENG_PIOA::en_lift_pin_zo_axis_position_speed_setting;	break;
	case ENG_LPAT::en_bsa_z_axis: enAddr = ENG_PIOA::en_bsa_camera_zo_axis_position_speed_setting;	break;
	default						: return FALSE;
	}

	/* 속도 값 계산 */
	u32Value = (UINT32)ROUNDED(value / 0.01f, 0);
	/* 속도 범위에 유효한지 확인 */
	switch (type)
	{
	case ENG_LPAT::en_pin_x_axis:
	case ENG_LPAT::en_pin_z_axis:	bValid = (u32Value > 0 && u32Value <= 180000);	break;
	case ENG_LPAT::en_bsa_z_axis:	bValid = (u32Value > 0 && u32Value <= 21840);	break;
	}

	if (!bValid)
	{
		LOG_ERROR(ENG_EDIC::en_engine,
				  L"Pin Axis Position Speed Settings : Out of speed range value.");
		return FALSE;
	}

	return uvEng_MCQ_WriteDWord(UINT32(enAddr), u32Value);
}

/*
 desc : Lift Pin or BSA Axis : Position 설정
 parm : type	- [in]  0:en_pin_x_axis, 1:en_pin_z_axis, 2:en_bas_z_axis
		value	- [in]  Jog Speed Value (unit : um)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_MCQ_WriteLiftPinAxisPos(ENG_LPAT type, DOUBLE value)
{
	BOOL bValid		= FALSE;
	INT32 i32Value	= 0x00000000;
	ENG_PIOA enAddr	= ENG_PIOA::en_lift_pin_xo_axis_position_setting;

	switch (type)
	{
	case ENG_LPAT::en_pin_x_axis: enAddr = ENG_PIOA::en_lift_pin_xo_axis_position_setting;		break;
	case ENG_LPAT::en_pin_z_axis: enAddr = ENG_PIOA::en_lift_pin_zo_axis_position_setting;		break;
	case ENG_LPAT::en_bsa_z_axis: enAddr = ENG_PIOA::en_bsa_camera_zo_axis_position_setting;	break;
	default						: return FALSE;
	}

	/* 속도 값 계산 */
	i32Value = (INT32)ROUNDED(value / 0.1f, 0);
	/* 속도 범위에 유효한지 확인 */
	switch (type)
	{
	case ENG_LPAT::en_pin_x_axis:	bValid = (i32Value >= 0 && i32Value <= 2460000);	break;
	case ENG_LPAT::en_pin_z_axis:	bValid = (i32Value >= 0 && i32Value <= 257000);		break;
	case ENG_LPAT::en_bsa_z_axis:	bValid = (i32Value >= -10400 && i32Value <= 30500);	break;
	}

	if (!bValid)
	{
		LOG_ERROR(ENG_EDIC::en_engine,
				  L"Pin Axis Position Settings : Out of speed range value.");
		return FALSE;
	}

	return uvEng_MCQ_WriteDWord(UINT32(enAddr), UINT32(i32Value));
}

#ifdef __cplusplus
}
#endif