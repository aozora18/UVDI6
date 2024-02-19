
/*
 desc : The Interface Library for Sieb & Meyer's MC2 Motion Equipment
*/

#pragma once

#include "../conf/global.h"
#include "../conf/define.h"
#include "../conf/efu.h"


#ifdef __cplusplus
extern "C"
{
#endif

/* --------------------------------------------------------------------------------------------- */
/*                          External Function - < EFU > < for Engine >                           */
/* --------------------------------------------------------------------------------------------- */
/*
 desc : Return full EFU information
 parm : None
 retn : Structure pointer that stores all EFU information
*/
API_IMPORT LPG_EPAS uvEng_EFU_GetShMemMC2();
/*
 desc : Check whether it is currently connected with the EFU
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_EFU_IsConnected();
/*
 desc : Set the waiting time value for receiving to the remote system.
 parm : wait	- [in]  maximum waiting time (unit: msec)
 retn : None
*/
API_IMPORT VOID uvEng_EFU_SetAckTimeout(UINT64 wait);
/*
 desc : Reset the existing connected communication
 parm : None
 retn : None
*/
API_IMPORT VOID uvEng_EFU_ResetCommData();
/*
 desc : Set the specific (=Speed. RPM) status value of EFU
 parm : bl_id	- [in]  EFU ID (In other words, 0x01 ~ 0x20 (1 ~ 32), 0x00 - ALL)
		speed	- [in]  Setting Value (0 ~ 140 (0 ~ 1400) RPM (1 = 10 RPM))
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_EFU_ReqSetSpeed(UINT8 bl_id, UINT8 speed);
/*
 desc : Whether in EFU configurable mode
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_EFU_IsSetSpeed();
/*
 desc : Request the current status value of EFU.
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_EFU_ReqGetState();
/*
 desc : Whether the command can be sent
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_EFU_IsSendIdle();
/*
 desc : Returns a value by ID for BL500.
 parm : bl_id	- [in]  BL500 ID (0x01 ~ 0x032)
		type	- [in]  value kind
 retn : 1 byte value (In case of failure, the value is 0xff)
*/
API_IMPORT UINT8 uvEng_EFU_GetDataValue(UINT8 bl_id, ENG_SEBT type);
/*
 desc : Setting whether to periodically request status information of EFU
 parm : enable	- [in]  Periodically request status information (TRUE) or not request (FALSE)
 retn : None
 note : If the enable value is TRUE, the RPM setting command of EFU is not executed.
		To set the RPM of EFU, you must disable (FALSE) this value.
*/
API_IMPORT VOID uvEng_EFU_SetPeriodState(BOOL enable);

/* --------------------------------------------------------------------------------------------- */
/*                          External Function - < EFU > < for Common >                           */
/* --------------------------------------------------------------------------------------------- */


#ifdef __cplusplus
}
#endif