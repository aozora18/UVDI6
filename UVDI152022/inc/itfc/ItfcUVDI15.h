
/*
 desc : Common Library
*/

#pragma once

/* Enum & Struct File */
#include "../conf/global.h"	/* define.h 문의 순서와 enum.h 보다 먼저 와야 됨 */
#include "../conf/define.h"
#include "../conf/enum.h"
#include "../conf/conf_uvdi15.h"
#include "../conf/recipe_uvdi15.h"
#include "../conf/plc_addr_uvdi15.h"

/* Interface File - Normal */
#include "../itfe/EItfcCamera.h"
#include "../itfe/EItfcLogs.h"
#include "../itfe/EItfcLuria.h"
#include "../itfe/EItfcMC2.h"
#include "../itfe/EItfcGentec.h"
#include "../itfe/EItfcKeyenceLDS.h"

//#include "../itfe/EItfcTrig.h"
#include "../itfe/EItfcMvenc.h"
#include "../itfe/EItfcPhilhmi.h"
#include "../itfe/EItfcStrobeLamp.h"

/* Interface File - Data */
#include "../itfe/EItfcACamCali.h"
#include "../itfe/EItfcLedPower.h"
#include "../itfe/EItfcThickCali.h"
#include "../itfe/EItfcPhStep.h"
#include "../itfe/EItfcMarkUVDI15.h"
#include "../itfe/EItfcRcpUVDI15.h"


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
// API_IMPORT LPG_PDSM uvEng_ShMem_GetPLC();
// API_IMPORT LPG_PMRW uvEng_ShMem_GetPLCExt();
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
 desc : Photohead 단차 정보 반환
 parm : None
 retn : 단차 정보가 저장된 구조체 포인터
*/
API_IMPORT LPG_OSSD uvEng_ShMem_GetPhStep();
/*
 desc : Photohead Stripe Correct Table Y 정보 반환
 parm : None
 retn : 단차 정보가 저장된 구조체 포인터
*/
API_IMPORT LPG_OSCY uvEng_ShMem_GetPhCorrectY();

/*
 desc : Philhmi 전체 정보 반환
 parm : None
 retn : Philhmi 전체 정보가 저장된 구조체 포인터
*/
API_IMPORT LPG_PPR uvEng_ShMem_GetPhilhmi();
/*
 desc : Philhmi 통신 정보 반환
 parm : None
 retn : Philhmi 전체 정보가 저장된 구조체 포인터
*/
API_IMPORT LPG_DLSM uvEng_ShMem_GetPhilhmiLink();

/*
 desc : Strobe Lamp 전체 정보 반환
 parm : None
 retn : Strobe Lamp 전체 정보가 저장된 구조체 포인터
*/
API_IMPORT LPG_SLPR uvEng_ShMem_GetStrobeLamp();
/*
 desc : Strobe Lamp 통신 정보 반환
 parm : None
 retn : Strobe Lamp 전체 정보가 저장된 구조체 포인터
*/
API_IMPORT LPG_DLSM uvEng_ShMem_GetStrobeLampLink();


/*
 desc : Edge or DOF (vdof) 폴더 제거
 parm : type	- [in]  0x00: Edge, 0x01: vdof
 retn : None
*/
API_IMPORT VOID uvEng_RemoveAllFiles(UINT8 type);


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
API_IMPORT LPG_CCGS uvEng_Conf_GetComn();
/*
 desc : PLC 환경 정보
 parm : None
 retn : 환경 정보가 저장된 구조체 포인터 반환
*/
API_IMPORT LPG_CPSI uvEng_Conf_GetPLC();

/* --------------------------------------------------------------------------------------------- */
/*                                    외부 함수 - < Common >                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : UVDI15 Engine DI 초기화
 parm : engine	- [in]  Engine 여부 (TRUE: Engine 에서 호출, FALSE: 일반 제어UI S/W에서 호출)
		e_mode	- [in]  엔진 동작 모드 (engine 값이 FALSE이면, 무조건 0 값)
						0 : None (Engine Monitoring)
						1 : CMPS (노광 제어 SW)
						2 : Step (광학계 단차 측정)
						3 : VDOF (Align Camera DOF (Depth of Focus)
						4 : Exam (Align Shutting Inspection or Measurement)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Init(HWND hWnd, ENG_ERVM e_mode=ENG_ERVM::en_monitoring);
/*
 desc : UVDI15 Engine DI 해제
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT VOID uvEng_Close();
/*
 desc : UVDI15 Engine DI 해제여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Terminated();

/*
 desc : 환경 파일 다시 적재 진행
 parm : None
 retn : TRUE or FALSE
 note : Engine만 수행할 수 있는 함수
*/
API_IMPORT BOOL uvEng_ReLoadConfig();
/*
 desc : 데이터 파일 다시 적재
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_ReloadCaliFile();
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

API_IMPORT BOOL uvEng_Camera_SetModelDefine_tot(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
	LPG_CMPV model, UINT8 fi_No, TCHAR* file,
	DOUBLE scale_min = 0.0f, DOUBLE scale_max = 0.0f,
	DOUBLE score_min = 0.0f, DOUBLE score_tgt = 0.0f);

API_IMPORT BOOL uvEng_IsInitedEngine();

// by sysandj : recipe 저장
API_EXPORT BOOL		uvEng_SetRecipeConfig(PTCHAR name);
API_EXPORT PTCHAR	uvEng_GetRecipeConfig();
// by sysandj : recipe 저장

/*
 desc : 환경 파일 저장
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_SaveConfig();

/*
 desc : 모든 장비가 연결되어 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_IsConnectedAll();
/*
 desc : PLC, Motion, Luria 및 기타 모듈로부터 DI 장비에 문제 (알람)가 발생했는지 여부 반환
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_IsEngineAlarm();
/*
 desc : PLC, Motion, Luria 및 기타 모듈로부터 DI 장비에 문제 (경고)가 발생했는지 여부 반환
 parm : None
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_IsEngineWarn();
/*
 desc : 작업 이름 설정
 parm : name	- [in]  작업 (STEP 포함) 이름
 retn : None
*/
API_IMPORT VOID uvEng_SetWorkStepName(PTCHAR name);
API_IMPORT VOID uvEng_SetWorkOptionalText(PTCHAR text);
API_IMPORT PTCHAR uvEng_GetWorkOptionalText();

/*
 desc : 현재 작업 이름 반환
 parm : None
 retn : 작업 이름 (STEP 포함) 반환
*/
API_IMPORT PTCHAR uvEng_GetWorkStepName();
/*
 desc : 전체 작업 진행률 설정 및 반환
 parm : rate	- [in]  작업 진행률 (unit: percent;%)
 retn : None or 진행률 값 (단위: %)
*/
API_IMPORT VOID uvEng_SetWorkStepRate(DOUBLE rate);
API_IMPORT DOUBLE uvEng_GetWorkStepRate();
/*
 desc : 현재 작업 동작 중인데 소요되고 있는 시간 설정 및 반환
 parm : job_id	- [in]  작업 코드 정보 (유일한 코드 값임)
		w_time	- [in]  현재 작업하는데 소요되고 있는 시간 (완료 시간이기도 함) (단위: msec)
		e_time	- [in]  작업이 완료되는 시점의 소요된 시간
 retn : None or 작업 소요 시간 (단위:  msec)
*/
API_IMPORT VOID uvEng_SetJobWorkInfo(UINT8 job_id, UINT64 e_time);
API_IMPORT VOID uvEng_UpdateJobWorkTime(UINT64 w_time);
API_IMPORT UINT64 uvEng_GetJobWorkTimeAverage();
API_IMPORT UINT64 uvEng_GetJobWorkTime();
/*
 desc : 동일한 작업의 반복 횟수 값 설정 및 반환
 parm : None
 retn : None or 동일 작업 개수
*/
API_IMPORT UINT32 uvEng_GetJobWorkCount();
/*
 desc : 각종 작업 관련 시간 및 통계 정보 초기화
 parm : None
 retn : None
*/
API_IMPORT VOID uvEng_ResetJobWorkInfo();

/* --------------------------------------------------------------------------------------------- */
/*                              외부 함수 - < Trigger for UVDI15 >                                */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Trigger가 발생될 Motion Position 값 등록
 parm : mark	- [in]  Mark Type (Global or Local)
		cam_id	- [in]  Align Camera Index (1 or 2)
		index	- [in]  Array Index (Zero-based)
		trig	- [in]  Trigger Position (Unit : 100 nm or 0.1 um)
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvEng_Trig_SetTrigger(ENG_AMTF mark, UINT8 cam_id, UINT8 index, INT32 trig);
/*
 desc : Trigger가 발생될 Motion Position 값 반환
 parm : mark	- [in]  Mark Type (Global or Local)
		cam_id	- [in]  Align Camera Index (1 or 2)
 retn : 배열 포인터 반환
*/
API_IMPORT PINT32 uvEng_Trig_GetTrigger(ENG_AMTF mark, UINT8 cam_id);

/* --------------------------------------------------------------------------------------------- */
/*                                 외부 함수 - < PLC for UVDI15 >                                 */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : DI 내부의 현재 온도 반환
 parm : index	- [in]  0x00 ~ 0x03 (TEMP Sensor가 설치된 4곳 중 1 곳)
 retn : 온도 값 반환
*/
API_IMPORT DOUBLE uvEng_PLC_GetTempDI(UINT8 index);
/*
 desc : DI 내부의 평균 온도 반환
 parm : index	- [in]  0x00 ~ 0x03 (TEMP Sensor가 설치된 4곳 중 1 곳)
 retn : 온도 값 반환
*/
API_IMPORT DOUBLE uvEng_PLC_GetTempDIAvg(UINT8 index);
/*
 desc : DI 내부의 최소 온도 반환
 parm : index	- [in]  0x00 ~ 0x03 (TEMP Sensor가 설치된 4곳 중 1 곳)
 retn : 온도 값 반환
*/
API_IMPORT DOUBLE uvEng_PLC_GetTempDIMin(UINT8 index);
/*
 desc : DI 내부의 최대 온도 반환
 parm : index	- [in]  0x00 ~ 0x03 (TEMP Sensor가 설치된 4곳 중 1 곳)
 retn : 온도 값 반환
*/
API_IMPORT DOUBLE uvEng_PLC_GetTempDIMax(UINT8 index);

#ifdef __cplusplus
}
#endif