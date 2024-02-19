// pch.cpp: 미리 컴파일된 헤더에 해당하는 소스 파일

#include "pch.h"

/* --------------------------------------------------------------------------------------------- */
/*                                           전역 변수                                           */
/* --------------------------------------------------------------------------------------------- */

TCHAR				g_tzWorkDir[MAX_PATH_LEN]			= {NULL};
LOGFONT				g_lf								= {NULL};

/* 엔진 초기화 되었는지 여부에 따라 */
BOOL				g_bEngineInited						= FALSE;
TCHAR				g_tzPEngine[MAX_FILE_LEN]			= {NULL};

/* MC2 Global Parameter */
UINT8				g_u8DrvDoneToggled[MAX_MC2_DRIVE]	= {NULL};
UINT8				g_u8DrvCmdToggled[MAX_MC2_DRIVE]	= {NULL};

/* Software Running Mode  */
ENG_ERVM			g_enRunMode		= ENG_ERVM::en_cmps_sw;

/* Data File */
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
CLedPower			*g_pLedPower	= NULL;	/* LED Power */
CThickCali			*g_pThickCali	= NULL;	/* Material Thick Calibration for Align Camera */
CRecipe				*g_pRecipe		= NULL;	/* Recipe Data */
CMark				*g_pMark		= NULL;	/* Mark Recipe */
#endif
CPhStep				* g_pPhStep = NULL;	/* Photohead Step Calibration */
CCodeToStr			*g_pCodeToStr	= NULL;	/* Error Code -> String (Message) */

/* Shared Memory Object */
CMemConf			*g_pMemConf		= NULL;
CMemLuria			*g_pMemLuria	= NULL;
CMemMC2				*g_pMemMC2		= NULL;
CMemPLC				*g_pMemPLC		= NULL;
//CMemPM100D			*g_pMemPM100D	= NULL;
CMemMC2b			* g_pMemMC2b = NULL;
CMemMvenc			* g_pMemMvenc = NULL;
CMemTrig			*g_pMemTrig		= NULL;
CMemVisi			*g_pMemVisi		= NULL;
CMemPhStep			*g_pMemPhStep	= NULL;

#if 0	/* It is used only in PODIS project */
/* Main Thread */
CMainThread			*g_pMainThread	= NULL;
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                           전역 함수                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Return the shared memory structure pointer
 parm : None
 retn : shared memory
*/
LPG_CIEA GetConfig()
{
	if (!g_pMemConf)	return NULL;
	return g_pMemConf->GetMemMap();
}
LPG_LDSM GetShMemLuria()
{
	if (!g_pMemLuria)	return NULL;
	return g_pMemLuria->GetMemMap();
}
LPG_MDSM GetShMemMC2()
{
	if (!g_pMemMC2)	return NULL;
	return (LPG_MDSM)g_pMemMC2->GetMemMap();
}
LPG_MDSM GetShMemMC2b()
{
	if (!g_pMemMC2b)	return NULL;
	return g_pMemMC2b->GetMemMap();
}
LPG_PDSM GetShMemPLC()
{
	if (!g_pMemPLC)	return NULL;
	return g_pMemPLC->GetMemMap();
}
//#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
//	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
//	LPG_PMRW GetShMemStruct();
//
//#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
//	LPG_PMDV GetShMemStruct();
//
//#endif
//	if (!g_pMemPLC)	return NULL;
//
//#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
//	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
//	return (LPG_PMRW)g_pMemPLC->GetMemMap()->data;
//#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
//	return (LPG_PMDV)g_pMemPLC->GetMemMap()->data;
//#endif
//}

LPG_PMRW GetShMemStruct()
{
	if (!g_pMemPLC)	return NULL;
	return (LPG_PMRW)g_pMemPLC->GetMemMap()->data;
}

LPG_DLSM GetShMemTrigLink()
{
	if (!g_pMemTrig)	return NULL;
	return (LPG_DLSM)g_pMemTrig->GetMemLink();
}
LPG_VDSM GetShMemVisi()
{
	if (!g_pMemVisi)	return NULL;
	return (LPG_VDSM)g_pMemVisi->GetMemMap();
}


