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
extern BOOL terminated;
#if (MC2_DRIVE_2SET == 0)
/* MC2 Global Parameter */
extern UINT8				g_u8DrvDoneToggled[MAX_MC2_DRIVE];
extern UINT8				g_u8DrvCmdToggled[MAX_MC2_DRIVE];
#elif(MC2_DRIVE_2SET == 1)
/* MC2 Global Parameter */
extern UINT8				g_u8DrvDoneToggled[MAX_MC2_DRIVE*2];
extern UINT8				g_u8DrvCmdToggled[MAX_MC2_DRIVE*2];
#endif


/* Software Running Mode  */
extern ENG_ERVM				g_enRunMode;

/* Data File */
extern CLedPower			*g_pLedPower;
extern CACamCali			*g_pACamCali;
extern CThickCali			*g_pThickCali;
extern CPhStep				*g_pPhStep;
extern CCorrectY			*g_pCorrectY;
extern CCodeToStr			*g_pCodeToStr;

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15_LLS06)
extern CRecipeUVDI15		*g_pRecipe;
extern CMark				*g_pMark;
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15|| \
DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
extern CRecipeUVDI15		*g_pRecipe;
extern CMarkUVDI15			*g_pMark;
#endif


/* Shared Memory Object */
extern CMemConf				*g_pMemConf;
extern CMemLuria			*g_pMemLuria;
extern CMemMC2				*g_pMemMC2;
//extern CMemPLC				*g_pMemPLC;
//extern CMemTrig				*g_pMemTrig;
extern CMemVisi				*g_pMemVisi;
extern CMemPhStep			*g_pMemPhStep;
extern CMemPhCorrectY		*g_pMemPhCorrectY;

extern CMemGentec			*g_pMemGentec;
extern CMemMvenc			*g_pMemMvenc;
extern CMemPhilhmi			*g_pMemPhilhmi;
extern CMemStrobeLamp		*g_pMemStrobeLamp;
extern CMemMC2b				*g_pMemMC2b;

/* Main Thread */
extern CMainThread			*g_pMainThread;

/* --------------------------------------------------------------------------------------------- */
/*                                           전역 함수                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Return the shared memory structure pointer
 parm : None
 retn : shared memory
*/
extern LPG_CIEA GetConfig();
extern LPG_LDSM GetShMemLuria();
extern LPG_VDSM GetShMemVisi();
extern LPG_MDSM GetShMemMC2();
extern LPG_MDSM GetShMemMC2b();
extern LPG_PDSM GetShMemPLC();
extern LPG_PMRW GetShMemStruct();
extern LPG_DLSM GetShMemTrigLink();
extern LPG_DLSM GetShMemPhilhmiLink();
extern LPG_PPR GetShMemPhilhmi();
extern LPG_DLSM GetShMemStrobeLampLink();
extern LPG_SLPR GetShMemStrobeLamp();
#endif //PCH_H
