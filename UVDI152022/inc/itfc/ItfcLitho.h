
/*
 desc : LSPA (PreAligner) Library
*/

#pragma once

#include "../conf/conf_gen2i.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : Linked Litho Library 초기화
 parm : config	- [in]  환경 설정 정보
		shmem	- [in]  PLC D Memory 영역을 가리키는 포인터
 retn : None
*/
API_IMPORT BOOL uvLitho_Init(LPG_CIEA config, PUINT8 shmem);
/*
 desc : Linked Litho Library 해제
 parm : None
 retn : None
*/
API_IMPORT VOID uvLitho_Close();
/*
 desc : 현재 Track 장비 내의 상태 값 반환
 parm : None
 retn : Track 장비 내의 상태 정보가 저장된 구조체 포인터
*/
API_IMPORT LPG_ODET uvLitho_GetStateTrack();
/*
 desc : 현재 Scanner 장비 내의 상태 값 반환
 parm : None
 retn : Track 장비 내의 상태 정보가 저장된 구조체 포인터
*/
API_IMPORT LPG_IDTE uvLitho_GetStateScanner();
/*
 desc : PIO History 초기화
 parm : None
 retn : None
*/
API_IMPORT VOID uvLitho_ResetHistPIO();
/*
 desc : 특정 위치의 PIO 상태 값 반환 (가장 최근부터 이전에 발생된 상태 값 가져오기)
 parm : count	- [in]  최소 1 값 (가장 최근에 발생된 순으로 값 가져오기 위한 개수 값)
		state	- [out] PIO 상태 값 저장 ('count' 개수만큼 저장될 구조체 버퍼 포인터)
 retn : TRUE or FALSE (버퍼가 비어 있거나, 해당 위치 (Index)에 값이 없거나 ...)
*/
API_IMPORT BOOL uvLitho_GetHistPIO(UINT8 count, LPG_STSE state);
/*
 desc : EXP-INL (Scanner Inline) 값 제어 (ON or OFF)
 parm : signal	- [in]  ON (0x01) or OFF (0x00)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLitho_SetScanInline(bool signal);
/*
 desc : EXP-RDY (Scanner Received to Ready) 값 제어 (ON or OFF)
 parm : signal	- [in]  ON (0x01) or OFF (0x00)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLitho_SetScanRecvToReady(bool signal);
/*
 desc : EXP_SND (Scanner Send to Ready) 값 제어 (ON or OFF)
 parm : signal	- [in]  ON (0x01) or OFF (0x00)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLitho_SetScanSendToReady(bool signal);
/*
 desc : EXP-REJ (Scanner Reject) 값 제어 (ON or OFF)
 parm : signal	- [in]  ON (0x01) or OFF (0x00)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLitho_SetScanReject(bool signal);
/*
 desc : Track 에서 Scanner로 Wafer가 이송 중인지 여부 (Transporting from Track To Scanner)
 parm : None
 retn : TRUE or FALSE
 note : 정상적인 이송인지 여부
*/
API_IMPORT BOOL uvLitho_IsMovingWaferFromTrackToScanner();
/*
 desc : 현재 Track에서 Scanner로 Wafer를 이송 가능한지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLitho_IsMoveWaferFromTrackToScanner();
/*
 desc : 현재 Track에서 Scanner로 Wafer (in Lot)가 이동이 완료되었는지 여부
 parm : None
 retn : TRUE or FALSE (이동이 완료되지 않은 상태... 즉, 다른 상태 임)
*/
API_IMPORT BOOL uvLitho_IsMovedWaferFromTrackToScanner();
/*
 desc : 현재 Track에서 Wafer 이송 동작 작업을 멈추게 할 수 있는지 여부
		즉, Wafer 이송 동작을 멈추게 하는 명령 (제어)이 가능한지 여부
		현재 Track에서 Wafer가 이송 중일 때는 멈추게 할 수 없기 때문임
 parm : None
 retn : TRUE or FALSE
 note : 현재 Track의 EFEM 이 동작 중이라는 가정 (설정)임
*/
API_IMPORT BOOL uvLitho_IsMoveStopWaferOnTrack();
/*
 desc : 현재 Lot Wafer의 마지막 Wafer 이동이 Track에서 Scanner로 이동이 취소 되었는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLitho_IsAbortedEndOfLotWaferFromTrackToScanner();
/*
 desc : Scanner 에서 Track로 Wafer가 이송 완료 되었는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLitho_IsMovingWaferFromScannerToTrack();

/*
 desc : Scanner 에서 Track로 Wafer가 이송 중인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLitho_IsMovedWaferFromScannerToTrack();
/*
 desc : Scanner 에서 Track로 Wafer가 이송 거부 중인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLitho_IsRejectingWaferFromScannerToTrack();
/*
 desc : Scanner 에서 Track로 Wafer가 이송 거부 완료 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLitho_IsRejectedWaferFromScannerToTrack();
/*
 desc : Scanner 에서 Wafer가 제거 (삭제) 중인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLitho_IsDeletingWaferInScanner();
/*
 desc : Scanner 에서 Wafer가 제거 (삭제) 중인지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvLitho_IsDeletedWaferInScanner();


#ifdef __cplusplus
}
#endif