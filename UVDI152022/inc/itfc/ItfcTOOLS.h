
/*
 desc : Common Library
*/

#pragma once

/* Enum & Struct File */
#include "../conf/global.h"
#include "../conf/define.h"
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
#include "../conf/conf_podis.h"
#elif (CUSTOM_CODE_GEN2I == DELIVERY_PRODUCT_ID)
//#include "../conf/conf_gen2i.h"
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
#include "../conf/recipe_uvdi15.h"
#endif
#include "../conf/plc_addr_podis.h"


#include "../../inc/conf/conf_uvdi15.h"


/* Interface File - Normal */
#include "../itfe/EItfcLogs.h"
#include "../itfe/EItfcLuria.h"
#include "../itfe/EItfcMC2.h"
#include "../itfe/EItfcPM100D.h"
#include "../itfe/EItfcCalc.h"

#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS15 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
//#include "../itfe/EItfcCamera.h"
//#include "../itfe/EItfcTrig.h"
//#include "../itfe/EItfcMvenc.h"
#else
#include "../itfe/EItfcCamera.h"
//#include "../itfe/EItfcTrig.h"
#include "../itfe/EItfcMvenc.h"
#include "../itfe/EItfcMCQPodis.h"
#endif
/* Interface File - Data */
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
#include "../itfe/EItfcACamCali.h"
#include "../itfe/EItfcLedPower.h"
#include "../itfe/EItfcPhStep.h"
#endif


#ifdef __cplusplus
extern "C"
{
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                   External API - < Shared >                                   */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Luria 전체 정보 반환
 parm : None
 retn : Luria 전체 정보가 저장된 구조체 포인터
*/
API_IMPORT LPG_LDSM uvEng_ShMem_GetLuria();
/*
 desc : MC2 전체 정보 반환
 parm : None
 retn : MC2 전체 정보가 저장된 구조체 포인터
*/
API_IMPORT LPG_MDSM uvEng_ShMem_GetMC2();
/*
 desc : Melsec Q PLC 전체 정보 반환
 parm : None
 retn : PLC 전체 정보가 저장된 구조체 포인터
*/
API_IMPORT LPG_PDSM uvEng_ShMem_GetPLC();
/*
 desc : Melsec Q PLC data information return (struct)
 parm : None
 retn : Structure pointer where all PLC information is stored
*/
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
API_IMPORT LPG_PMRW uvEng_ShMem_GetPLCStruct();
#elif (CUSTOM_CODE_GEN2I == DELIVERY_PRODUCT_ID)
API_IMPORT LPG_PMDV uvEng_ShMem_GetPLCStruct();
#endif
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
/*
 desc : Trigger Board 전체 정보 반환
 parm : None
 retn : Trigger Board 전체 정보가 저장된 구조체 포인터
*/
API_IMPORT LPG_TPQR uvEng_ShMem_GetTrig();
/*
 desc : Trigger Board 통신 정보 반환
 parm : None
 retn : Trigger Board 전체 정보가 저장된 구조체 포인터
*/
API_IMPORT LPG_DLSM uvEng_ShMem_GetTrigLink();
/*
 desc : Vision 전체 정보 반환
 parm : None
 retn : Vision 전체 정보가 저장된 구조체 포인터
*/
API_IMPORT LPG_VDSM uvEng_ShMem_GetVisi();
/*
 desc : Recipe (Photohead Offset; Step) 전체 정보 반환
 parm : None
 retn : PH Step 전체 정보가 저장된 구조체 포인터
*/
API_IMPORT LPG_OSSD uvEng_ShMem_GetPhStep();
#endif
/*
 desc : Edge or DOF (vdof) 폴더 제거
 parm : type	- [in]  0x00: Edge, 0x01: vdof
 retn : None
*/
API_IMPORT VOID uvEng_RemoveAllFiles(UINT8 type);


/* --------------------------------------------------------------------------------------------- */
/*                                   External API - < Config >                                   */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Luria 환경 정보 얻기
 parm : None
 retn : 환경 정보가 저장된 구조체 포인터 반환
*/
API_IMPORT LPG_CLSI uvEng_Conf_GetLuria();
/*
 desc : 통신 환경 정보
 parm : None
 retn : 환경 정보가 저장된 구조체 포인터 반환
*/
API_IMPORT LPG_CCSI uvEng_Conf_GetComm();
API_IMPORT LPG_CCGS uvEng_Conf_GetComn();
/*
 desc : PLC 환경 정보
 parm : None
 retn : 환경 정보가 저장된 구조체 포인터 반환
*/
API_IMPORT LPG_CPSI uvEng_Conf_GetPLC();

/* --------------------------------------------------------------------------------------------- */
/*                                   External API - < Config >                                   */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : PODIS Engine DI 초기화
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
		e_mode	- [in]  엔진 동작 모드 (engine 값이 FALSE이면, 무조건 0 값)
						0 : None (Engine Monitoring)
						1 : CMPS (노광 제어 SW)
						2 : Step (광학계 단차 측정)
						3 : VDOF (Align Camera DOF (Depth of Focus)
						4 : Exam (Align Shutting Inspection or Measurement)
		init	- [in]  Whether each service (module) is initialized or not
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Init(ENG_ERVM e_mode=ENG_ERVM::en_monitoring);
API_IMPORT BOOL uvEng_InitEx(ENG_ERVM e_mode, BOOL init);
/*
 desc : PODIS Engine DI 해제
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT VOID uvEng_Close();
/*
 desc : 환경 파일 다시 적재 진행
 parm : None
 retn : TRUE or FALSE
 note : Engine만 수행할 수 있는 함수
*/
API_IMPORT BOOL uvEng_ReLoadConfig();
/*
 desc : 전체 환경 파일 반환
 parm : None
 retn : 환경 파일 정보가 저장된 구조체 포인터
*/
API_IMPORT LPG_CIEA uvEng_GetConfig();

/*
 desc : 공유 메모리가 초기화 되었는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_IsInitedEngine();
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
/*
 desc : 환경 파일 저장 - PH Step 부분만 저장
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_SavePhStep();
/*
 desc : 환경 파일 저장 - ACam Cali 부분만 저장
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_SaveACamCali();
#endif
/*
 desc : 환경 파일 저장
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_SaveConfig();


/* --------------------------------------------------------------------------------------------- */
/*                                외부 함수 - < Luria for PODIS >                                */
/* --------------------------------------------------------------------------------------------- */

#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
/*
 desc : 기준 Mark 2 정보 기준으로, 현재 Mark 번호에 대한 이동해야 할 카메라 X 위치 반환
 parm : mark_no	- [in]  Mark Number (0x01 or Later)
 retn : 상수 값 (mm)
*/
API_IMPORT DOUBLE uvEng_Luria_GetACamMark2MotionX(UINT16 mark_no);
/*
 desc : 현재 적재된 거버의 임의 Mark Number에 대해, 이동하고자 하는 모션의 Stage X / Y 위치 반환
 parm : mark_no	- [in]  Global Mark Number (1 or Later)
		acam_x	- [out] Motion 이동 위치 반환 (X : mm)
		stage_y	- [out] Motion 이동 위치 반환 (Y : mm)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Luria_GetGlobalMarkMoveXY(UINT16 mark_no, DOUBLE &acam_x, DOUBLE &stage_y);
#endif

#ifdef __cplusplus
}
#endif