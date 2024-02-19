
/*
 desc : The Interface Library for Shinsung EFU
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
 desc : Return full EFU information
 parm : None
 retn : Structure pointer that stores all EFU information
*/
LPG_EPAS GetShMemEFU()
{
	if (!g_pMemEFU)	return NULL;
	return (LPG_EPAS)g_pMemEFU->GetMemMap();
}


#ifdef __cplusplus
extern "C"
{
#endif

/* --------------------------------------------------------------------------------------------- */
/*                              외부 함수 - < EFU > < for Engine >                               */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Return full EFU information
 parm : None
 retn : Structure pointer that stores all EFU information
*/
API_EXPORT LPG_EPAS uvEng_EFU_GetShMemMC2()
{
	return GetShMemEFU();
}

/*
 desc : Check whether it is currently connected with the EFU
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_EFU_IsConnected()
{
	return uvEFU_IsConnected();
}

/*
 desc : Set the waiting time value for receiving to the remote system.
 parm : wait	- [in]  maximum waiting time (unit: msec)
 retn : None
*/
API_EXPORT VOID uvEng_EFU_SetAckTimeout(UINT64 wait)
{
	uvEFU_SetAckTimeout(wait);
}

/*
 desc : Reset the existing connected communication
 parm : None
 retn : None
*/
API_EXPORT VOID uvEng_EFU_ResetCommData()
{
	uvEFU_ResetCommData();
}

/*
 desc : Set the specific (=Speed. RPM) status value of EFU
 parm : bl_id	- [in]  EFU ID (In other words, 0x01 ~ 0x20 (1 ~ 32), 0x00 - ALL)
		speed	- [in]  Setting Value (0 ~ 140 (0 ~ 1400) RPM (1 = 10 RPM))
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_EFU_ReqSetSpeed(UINT8 bl_id, UINT8 speed)
{
	return uvEFU_ReqSetSpeed(bl_id, speed);
}

/*
 desc : Whether in EFU configurable mode
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_EFU_IsSetSpeed()
{
	return uvEFU_IsSetSpeed();
}

/*
 desc : Request the current status value of EFU.
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_EFU_ReqGetState()
{
	return uvEFU_ReqGetState();
}

/*
 desc : Whether the command can be sent
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_EFU_IsSendIdle()
{
	return uvEFU_IsSendIdle();
}

/*
 desc : Returns a value by ID for BL500.
 parm : bl_id	- [in]  BL500 ID (0x01 ~ 0x032)
		type	- [in]  value kind
 retn : 1 byte value (In case of failure, the value is 0xff)
*/
API_EXPORT UINT8 uvEng_EFU_GetDataValue(UINT8 bl_id, ENG_SEBT type)
{
	LPG_EPAS pstShMem	= g_pMemEFU->GetMemMap();
	if (bl_id < 1 || GetConfig()->efu.ext_param_1 < bl_id)	return 0xff;
	switch (type)
	{
	case ENG_SEBT::en_pv	:	return pstShMem[bl_id-1].pv;
	case ENG_SEBT::en_alarm	:	return pstShMem[bl_id-1].alarm;
	case ENG_SEBT::en_sv	:	return pstShMem[bl_id-1].sv;
	}
	return 0xff;
}

/*
 desc : Setting whether to periodically request status information of EFU
 parm : enable	- [in]  Periodically request status information (TRUE) or not request (FALSE)
 retn : None
 note : If the enable value is TRUE, the RPM setting command of EFU is not executed.
		To set the RPM of EFU, you must disable (FALSE) this value.
*/
API_EXPORT VOID uvEng_EFU_SetPeriodState(BOOL enable)
{
	uvEFU_SetPeriodState(enable);
}

/* --------------------------------------------------------------------------------------------- */
/*                              외부 함수 - < EFU > < for Common >                               */
/* --------------------------------------------------------------------------------------------- */


#ifdef __cplusplus
}
#endif