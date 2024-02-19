// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"

/* --------------------------------------------------------------------------------------------- */
/*                                           전역 변수                                           */
/* --------------------------------------------------------------------------------------------- */

extern TCHAR				g_tzWorkDir[MAX_PATH_LEN];
extern LOGFONT				g_lf;

/* 엔진 초기화 되었는지 여부에 따라 */
extern BOOL					g_bEngineInited;
extern TCHAR				g_tzPEngine[MAX_FILE_LEN];


/* MC2 Global Parameter */
extern UINT8				g_u8DrvDoneToggled[MAX_MC2_DRIVE];
extern UINT8				g_u8DrvCmdToggled[MAX_MC2_DRIVE];

/* Software Running Mode  */
extern ENG_ERVM				g_enRunMode;

/* Data File */
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
extern CLedPower			*g_pLedPower;
extern CThickCali			*g_pThickCali;
extern CRecipe				*g_pRecipe;
extern CMark				*g_pMark;
#endif
extern CPhStep				* g_pPhStep;
extern CCodeToStr			*g_pCodeToStr;

/* Shared Memory Object */
extern CMemConf				*g_pMemConf;
extern CMemLuria			*g_pMemLuria;
extern CMemMC2				*g_pMemMC2;
extern CMemPLC				*g_pMemPLC;
//extern CMemPM100D			*g_pMemPM100D;
extern CMemMC2b				* g_pMemMC2b;
extern CMemMvenc			* g_pMemMvenc;

extern CMemTrig				*g_pMemTrig;
extern CMemVisi				*g_pMemVisi;
extern CMemPhStep			*g_pMemPhStep;

#if 0	/* It is only used in PODIS project */
/* Main Thread */
extern CMainThread			*g_pMainThread;
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                           전역 함수                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 엔진 내부 환경 파일 구조체 포인터 반환
 parm : None
 retn : 구조체 포인터 (공유 메모리 영역)
*/
extern LPG_CIEA GetConfig();
extern LPG_LDSM GetShMemLuria();
extern LPG_MDSM GetShMemMC2();
extern LPG_MDSM GetShMemMC2b();
extern LPG_PDSM GetShMemPLC();
extern LPG_PMRW GetShMemStruct();

//extern LPG_PMDV GetShMemStruct();
#endif


extern LPG_DLSM GetShMemTrigLink();
extern LPG_VDSM GetShMemVisi();



//#endif //PCH_H
