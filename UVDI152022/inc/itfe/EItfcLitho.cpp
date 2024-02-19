
/*
 desc : Engine Library (Shared Memory & Support Library)
*/

#include "pch.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


#ifdef __cplusplus
extern "C"
{
#endif

/* --------------------------------------------------------------------------------------------- */
/*                        외부 함수 - Linked Litho for TEL < for Engine >                        */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Returns the status value within the current Track device
 parm : None
 retn : Structure pointer where status information in Track equipment is stored
*/
API_EXPORT LPG_ODET uvEng_Litho_GetStateTrack()
{
	return uvLitho_GetStateTrack();
}

/*
 desc : Returns the status value in the current Scanner device
 parm : None
 retn : Structure pointer where status information in Track equipment is stored
*/
API_EXPORT LPG_IDTE uvEng_Litho_GetStateScanner()
{
	return uvLitho_GetStateScanner();
}

/*
 desc : Initialize PIO History
 parm : None
 retn : None
*/
API_EXPORT VOID uvEng_Litho_ResetHistPIO()
{
	return uvLitho_ResetHistPIO();
}

/*
 desc : Return the PIO state value of a specific location (get the most recent to the previous state value)
 parm : count	- [in]  Minimum 1 value (count values to get values in order of most recent occurrence)
		state	- [out] Store PIO state value (struct buffer pointer to be saved as many as 'count')
 retn : TRUE or FALSE (Either the buffer is empty, there is no value at that position (Index), or ...)
*/
API_EXPORT BOOL uvEng_Litho_GetHistPIO(UINT8 count, LPG_STSE state)
{
	return uvLitho_GetHistPIO(count, state);
}

/*
 desc : EXP-INL (Scanner Inline) Value Control (ON or OFF)
 parm : signal	- [in]  ON (0x01) or OFF (0x00)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Litho_SetScanInline(bool signal)
{
	return uvLitho_SetScanInline(signal);
}

/*
 desc : EXP-RDY (Scanner Received to Ready) Value Control (ON or OFF)
 parm : signal	- [in]  ON (0x01) or OFF (0x00)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Litho_SetScanRecvToReady(bool signal)
{
	return uvLitho_SetScanRecvToReady(signal);
}

/*
 desc : EXP_SND (Scanner Send to Ready) Value Control (ON or OFF)
 parm : signal	- [in]  ON (0x01) or OFF (0x00)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Litho_SetScanSendToReady(bool signal)
{
	return uvLitho_SetScanSendToReady(signal);
}

/*
 desc : EXP-REJ (Scanner Reject) Value Control (ON or OFF)
 parm : signal	- [in]  ON (0x01) or OFF (0x00)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Litho_SetScanReject(bool signal)
{
	return uvLitho_SetScanReject(signal);
}

/*
 desc : Whether the wafer is being transferred from the track to the scanner (Transporting from Track To Scanner)
 parm : None
 retn : TRUE or FALSE
 note : 정상적인 이송인지 여부 (Normal Wafer Transfer)
*/
API_EXPORT BOOL uvEng_Litho_IsMovingWaferFromTrackToScanner()
{
	return uvLitho_IsMovingWaferFromTrackToScanner();
}

/*
 desc : Whether it is possible to transfer the wafer from the current track to the scanner
		In other words, whether the Scanner is ready to receive a Wafer
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Litho_IsMoveWaferFromTrackToScanner()
{
	return uvLitho_IsMoveWaferFromTrackToScanner();
}

/*
 desc : Whether the wafer (in lot) has been moved from the current track to the scanner
 parm : None
 retn : TRUE or FALSE (The move is not complete... i.e. it is in a different state.)
*/
API_EXPORT BOOL uvEng_Litho_IsMovedWaferFromTrackToScanner()
{
	return uvLitho_IsMovedWaferFromTrackToScanner();
}

/*
 desc : Whether it is possible to stop the wafer transfer motion operation on the current track
		That is, whether a command (control) to stop the wafer transfer operation is possible.
		This is because it cannot be stopped while the wafer is being transferred in the current track.
 parm : None
 retn : TRUE or FALSE
 note : It is assumed (setting) that EFEM of the current track is in operation.
*/
API_EXPORT BOOL uvEng_Litho_IsMoveStopWaferOnTrack()
{
	return uvLitho_IsMoveStopWaferOnTrack();
}

/*
 desc : Whether the last wafer movement of the current lot wafer has been canceled from the track to the scanner
 parm : None
 retn : TRUE or FALSE
 note : End of Lot Wafer Aborted
*/
API_EXPORT BOOL uvEng_Litho_IsAbortedEndOfLotWaferFromTrackToScanner()
{
	return uvLitho_IsAbortedEndOfLotWaferFromTrackToScanner();
}

/*
 desc : Whether the wafer has been transferred from the scanner to the track
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Litho_IsMovingWaferFromScannerToTrack()
{
	return uvLitho_IsMovingWaferFromScannerToTrack();
}

/*
 desc : Whether the wafer is being transferred from the scanner to the track
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Litho_IsMovedWaferFromScannerToTrack()
{
	return uvLitho_IsMovedWaferFromScannerToTrack();
}

/*
 desc : Whether the wafer is refusing to transfer from Scanner to Track
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Litho_IsRejectingWaferFromScannerToTrack()
{
	return uvLitho_IsRejectingWaferFromScannerToTrack();
}

/*
 desc : Whether or not the transfer has been rejected by the wafer from the scanner to the track
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Litho_IsRejectedWaferFromScannerToTrack()
{
	return uvLitho_IsRejectedWaferFromScannerToTrack();
}

/*
 desc : Whether the Wafer is being removed (deleted) by the Scanner
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Litho_IsDeletingWaferInScanner()
{
	return uvLitho_IsDeletingWaferInScanner();
}

/*
 desc : Whether the Wafer is being removed (deleted) by the Scanner
 parm : None
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvEng_Litho_IsDeletedWaferInScanner()
{
	return uvLitho_IsDeletedWaferInScanner();
}

/* --------------------------------------------------------------------------------------------- */
/*                        외부 함수 - Linkid Litho for TEL < for Common >                        */
/* --------------------------------------------------------------------------------------------- */


#ifdef __cplusplus
}
#endif