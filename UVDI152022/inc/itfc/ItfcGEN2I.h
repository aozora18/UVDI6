
/*
 desc : Common Library
*/

#pragma once

/* Enum & Struct File */
#include "../conf/global.h"
#include "../conf/define.h"
#include "../conf/conf_gen2i.h"
#include "../conf/wafer.h"
#include "../conf/efu.h"
#include "../conf/milara.h"

/* Interface File - Normal */
#include "../itfe/EItfcLitho.h"
#include "../itfe/EItfcLogs.h"
#include "../itfe/EItfcLuria.h"
#include "../itfe/EItfcMC2.h"
#include "../itfe/EItfcMCQGen2i.h"
#include "../itfe/EItfcMDR.h"
#include "../itfe/EItfcMPA.h"
#include "../itfe/EItfcEFU.h"
#include "../itfe/EItfcBSA.h"
#include "../itfe/EItfcPM100D.h"
#if 0 /* Not used (It is managed by the QWrite project) */
#include "../itfe/EItfcMCQGen2i.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                    외부 함수 - < Shared >                                     */
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
API_IMPORT LPG_PMDV uvEng_ShMem_GetPLCStruct();
/*
 desc : Prealigner for Logosol 전체 정보 반환
 parm : None
 retn : 전체 정보가 저장된 구조체 포인터
*/
API_IMPORT LPG_MPDV uvEng_ShMem_GetLSPA();
/*
 desc : Diamond Robot for Logosol 전체 정보 반환
 parm : None
 retn : 전체 정보가 저장된 구조체 포인터
*/
API_IMPORT LPG_MRDV uvEng_ShMem_GetLSDR();
/*
 desc : BSA 전체 정보 반환
 parm : None
 retn : 전체 정보가 저장된 구조체 포인터
*/
API_IMPORT LPG_RBVC uvEng_ShMem_GetBSA()
/*
 desc : Wafer Tracking 전체 정보 반환
 parm : None
 retn : 전체 정보가 저장된 구조체 포인터
*/
API_IMPORT LPG_CWTI uvEng_ShMem_GetWaferTracking();
/*
 desc : Exposure Parmeter 전체 정보 반환
 parm : None
 retn : 전체 정보가 저장된 구조체 포인터
*/
API_IMPORT LPG_CTEI uvEng_ShMem_GetExpoParam();
/*
 desc : Teaching Position 전체 정보 반환
 parm : None
 retn : 전체 정보가 저장된 구조체 포인터
*/
API_IMPORT LPG_CTPI uvEng_ShMem_GetTeachingPosition();
/*
 desc : Edge or DOF (vdof) 폴더 제거
 parm : type	- [in]  0x00: Edge, 0x01: vdof
 retn : None
*/
API_IMPORT VOID uvEng_RemoveAllFiles(UINT8 type);
/*
 desc : Get the size of Shared Memory
 parm : dev_id	- [in]  
 retn : structure size (unit: bytes)
*/
API_IMPORT UINT32 uvEng_GetShMemSizeOfStruct(ENG_EDIC dev_id);

/* --------------------------------------------------------------------------------------------- */
/*                                    외부 함수 - < Config >                                     */
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
/*
 desc : PLC 환경 정보
 parm : None
 retn : 환경 정보가 저장된 구조체 포인터 반환
*/
API_IMPORT LPG_CPSI uvEng_Conf_GetPLC();
/*
 desc : Teaching 환경 정보
 parm : None
 retn : 환경 정보가 저장된 구조체 포인터 반환
*/
API_IMPORT LPG_CTPI uvEng_GetConfigTeaching();

/* --------------------------------------------------------------------------------------------- */
/*                                외부 함수 - < Config - Teaching>                               */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Teach Centering Data 저장
 변수 :	cam_no	- [in] 카메라 설정 (0x01: bsa , 0x02: up)
		point	- [in] Centering Point (QuartZ(0x01), Mark1(0x02), Mark2(0x03)
		dx		- [in] Centering Point에 대한 X Axis Position
		dy		- [in] Centering Point에 대한 Y Axis Position
 retn : None
*/
API_IMPORT VOID uvEng_Conf_Teaching_SetCenteringPos(UINT8 cam_no, UINT8 point, DOUBLE dx, DOUBLE dy);
/*
 desc : Teach focus Data 저장
 변수 :	cam_no	- [in] 카메라 설정 (0x01: bsa , 0x02: up)
		point	- [in] Centering Point (QuartZ(0x01), Mark1(0x02), Mark2(0x03)
		dz		- [in] Centering Point에 대한 X Axis Position
 retn : None
*/
API_IMPORT VOID uvEng_Conf_Teaching_SetFocusPos(UINT8 cam_no, UINT8 point, DOUBLE dz);
/*
 desc : Teach Camoffset Data 저장
 parm : dx	- [in] Camera offset X
		dy	- [in] Camera offset Y
 retn : None
*/
API_IMPORT VOID uvEng_Conf_Teaching_SetCamOffset(DOUBLE dx, DOUBLE dy);
/*
 desc : Teach Camoffset Data 저장
 parm : dx	- [in] Camera offset X
		dy	- [in] Camera offset Y
 retn : None
*/
API_IMPORT VOID uvEng_Conf_Teaching_SetQuartzCentOffset(DOUBLE dx, DOUBLE dy);
/*
 desc : Teach Camoffset Data 저장
 parm : dx	- [in] Camera offset X
		dy	- [in] Camera offset Y
 retn : None
*/
API_IMPORT VOID uvEng_Conf_Teaching_SetBSAOffset(DOUBLE dx, DOUBLE dy);
/*
 desc : 조도 위치 정보 설정
 parm : x	- [in]  Axis X
		x	- [in]  Axis Y
 retn : None
*/
API_IMPORT VOID uvEng_Conf_Teaching_SetIllumPos(DOUBLE x, DOUBLE y);
/*
 desc : LD Sensor 위치 정보 설정
 parm : x		- [in]  Axis X
		y		- [in]  Axis Y
		loader	- [in]  Axis Loader
 retn : None
*/
API_IMPORT VOID uvEng_Conf_Teaching_SetLDSensorValue(DOUBLE point1, DOUBLE point2, DOUBLE point3);

/* --------------------------------------------------------------------------------------------- */
/*                           외부 함수 - < Config - Exposure Parameter>                          */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 노광 대상 거버 등록
 parm : index	- [in]  등록될 위치  (0 ~ 4)
		gerber	- [in]  등록 대상 거버 이름 (전체 경로 포함?)
 retn : None
*/
API_IMPORT VOID uvEng_Conf_ExpoParam_AddExpoGerber(UINT8 index, PTCHAR gerber);
/*
 desc : 측정 대상 거버 등록
 parm : index	- [in]  등록될 위치  (0 ~ 9)
		gerber	- [in]  등록 대상 거버 이름 (전체 경로 포함?)
 retn : None
*/
API_IMPORT VOID uvEng_Conf_ExpoParam_AddMeasGerber(UINT8 index, PTCHAR gerber);
/*
 desc : Teach Equipment 파일 적재
 parm : index	- [in]  등록될 위치  (0 ~ 1)
		gid		- [in]  Gerber Index (1 ~ 25)
		mark1,2	- [in]  Mark 설정 1, 2
		thick	- [in]  Material Thickness (unit: 100 nm or 0.1 um)
		energy	- [in]  최종 노광될 광량 (단위: mj)
		speed	- [in]  최종 노광될 속도 (단위: mm/sec)
 retn : None
*/
API_IMPORT VOID uvEng_Conf_ExpoParam_AddExpoRecipe(UINT8 index, UINT8 gid, UINT8 mark1, UINT8 mark2,
												   UINT32 thick, DOUBLE energy, DOUBLE speed);
/*
 desc : 현재 저장(등록)되어 있는 FOUP의 레시피 (GET/PUT/GID/Energy) 정보 반환
 parm : index	- [in]  저장되어 있는 위치 (0 ~ 1)
 retn : Recipe가 저장된 구조체 참조
*/
API_IMPORT LPG_EFWG uvEng_Conf_ExpoParam_GetExpoRecipe(UINT8 index);
/*
 desc : 현재 등록되어 있는 거버 이름 반환
 parm : index	- [in]  저장되어 있는 위치 (0 ~ 4)
 retn : NULL or String
*/
API_IMPORT PTCHAR uvEng_Conf_ExpoParam_GetExpoGerber(UINT8 index);
/*
 desc : 현재 등록되어 있는 거버 이름 반환
 parm : index	- [in]  저장되어 있는 위치 (0 ~ 9)
 retn : NULL or String
*/
API_IMPORT PTCHAR uvEng_Conf_ExpoParam_GetMeasGerber(UINT8 index);
/*
설명 : 현재 등록되어 있는 Matrix 거버 이름 반환
변수 : type	- [in]  1번: EXPO_FOCUS_GERBER, 2번:EXPO_OFFSET_GERBER 
반환 : NULL or String
*/
API_IMPORT PTCHAR uvEng_Conf_ExpoParam_GetMatrixGerber(UINT8 type);
/*
 desc : 등록된 거버 초기화
 parm : None
 retn : None
*/
API_IMPORT VOID uvEng_Conf_ExpoParam_ResetExpoGerber();
/*
 desc : 등록된 거버 초기화
 parm : None
 retn : None
*/
API_IMPORT VOID uvEng_Conf_ExpoParam_ResetMeasGerber();
/*
 desc : 등록된 Recipe 초기화
 parm : None
 retn : None
*/
API_IMPORT VOID uvEng_Conf_ExpoParam_ResetExpoRecipe();

/* --------------------------------------------------------------------------------------------- */
/*                                    외부 함수 - < Engine >                                     */
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
						engine 값이 FALSE이면, mode 값은 의미 없음
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Init(ENG_ERVM e_mode=ENG_ERVM::en_cmps_sw);
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
/*
 desc : 환경 파일 저장
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_SaveConfig();
/*
 desc : 환경 파일 저장 <Only Teaching>
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_SaveConfigTeaching(LPG_CTPI data=NULL);
/*
 desc : 환경 파일 저장 <Only ExpoParameter>
 parm : data	- [in]  외부로부터 입력된 데이터
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_SaveConfigExpoParamter(LPG_CTEI data=NULL);


#ifdef __cplusplus
}
#endif