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


#if (MC2_DRIVE_2SET == 0)
/* MC2 Global Parameter */
UINT8				g_u8DrvDoneToggled[MAX_MC2_DRIVE] = { NULL };
UINT8				g_u8DrvCmdToggled[MAX_MC2_DRIVE] = { NULL };
#elif(MC2_DRIVE_2SET == 1)
/* MC2 Global Parameter */
UINT8				g_u8DrvDoneToggled[MAX_MC2_DRIVE*2] = { NULL };
UINT8				g_u8DrvCmdToggled[MAX_MC2_DRIVE*2] = { NULL };
#endif


/* Software Running Mode  */
ENG_ERVM			g_enRunMode			= ENG_ERVM::en_cmps_sw;

/* Data File */
CLedPower			*g_pLedPower		= NULL;	/* LED Power */
CACamCali			*g_pACamCali		= NULL;	/* Camera Calibration */
CThickCali			*g_pThickCali		= NULL;	/* Material Thick Calibration for Align Camera */
CPhStep				*g_pPhStep			= NULL;	/* Photohead Step Calibration */
CCorrectY			*g_pCorrectY		= NULL;	/* Photohead Stripe Calibration */

CCodeToStr			*g_pCodeToStr		= NULL;	/* Error Code -> String (Message) */

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15_LLS06)
CRecipeUVDI15*		g_pRecipe	= NULL;			/* Recipe Data */
CMark*				g_pMark		= NULL;			/* Mark Recipe */
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15|| \
DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
CRecipeUVDI15*	g_pRecipe	= NULL;
CMarkUVDI15*	g_pMark = NULL;
#endif



/* Shared Memory Object */
CMemConf			*g_pMemConf			= NULL;
CMemLuria			*g_pMemLuria		= NULL;
CMemMC2				*g_pMemMC2			= NULL;
//CMemPLC				*g_pMemPLC			= NULL;
//CMemTrig			*g_pMemTrig			= NULL;
CMemVisi			*g_pMemVisi			= NULL;
CMemPhStep			*g_pMemPhStep		= NULL;
CMemPhCorrectY		*g_pMemPhCorrectY	= NULL;

CMemGentec			*g_pMemGentec = NULL;
CMemMvenc			*g_pMemMvenc = NULL;
CMemPhilhmi			*g_pMemPhilhmi = NULL;
CMemStrobeLamp		*g_pMemStrobeLamp = NULL;
CMemMC2b			*g_pMemMC2b = NULL;

/* Main Thread */
CMainThread			*g_pMainThread	= NULL;


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
LPG_VDSM GetShMemVisi()
{
	if (!g_pMemVisi)	return NULL;
	return (LPG_VDSM)g_pMemVisi->GetMemMap();
}
LPG_MDSM GetShMemMC2()
{
	if (!g_pMemMC2)	return NULL;
	return g_pMemMC2->GetMemMap();
}
LPG_MDSM GetShMemMC2b()
{
	if (!g_pMemMC2b)	return NULL;
	return g_pMemMC2b->GetMemMap();
}

LPG_DLSM GetShMemTrigLink()
{
	if (!g_pMemMvenc)	return NULL;
	return (LPG_DLSM)g_pMemMvenc->GetMemLink();
}

LPG_DLSM GetShMemPhilhmiLink()
{
	if (!g_pMemPhilhmi)	return NULL;
	return (LPG_DLSM)g_pMemPhilhmi->GetMemLink();
}

LPG_PPR GetShMemPhilhmi()
{
	if (!g_pMemPhilhmi)	return NULL;
	return (LPG_PPR)g_pMemPhilhmi->GetMemMap();
}

LPG_DLSM GetShMemStrobeLampLink()
{
	if (!g_pMemStrobeLamp)	return NULL;
	return (LPG_DLSM)g_pMemStrobeLamp->GetMemLink();
}

LPG_SLPR GetShMemStrobeLamp()
{
	if (!g_pMemStrobeLamp)	return NULL;
	return (LPG_SLPR)g_pMemStrobeLamp->GetMemMap();
}
