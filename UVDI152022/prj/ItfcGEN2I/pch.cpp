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
CCodeToStr			*g_pCodeToStr		= NULL;	/* Error Code -> String (Message) */
CLedPower			*g_pLedPower		= NULL;	/* Recipe Data */
CRecipeGen2i		*g_pRecipe			= NULL;	/* Recipe Data */

/* Shared Memory Object */
CMemConf			*g_pMemConf			= NULL;
CMemConfTeaching	*g_pMemConfTeaching	= NULL;
CMemConfExpoParam	*g_pMemConfExpoParam= NULL;
CMemConfTracking	*g_pMemConfTracking	= NULL;
CMemLuria			*g_pMemLuria		= NULL;
CMemMC2				*g_pMemMC2			= NULL;
CMemPLC				*g_pMemPLC			= NULL;
CMemMPA				*g_pMemMPA			= NULL;
CMemMDR				*g_pMemMDR			= NULL;
CMemEFU				*g_pMemEFU			= NULL;
CMemBSA				*g_pMemBSA			= NULL;


/* --------------------------------------------------------------------------------------------- */
/*                                           전역 함수                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 엔진 내부 환경 파일 구조체 포인터 반환
 parm : None
 retn : 구조체 포인터 (공유 메모리 영역)
*/
LPG_CIEA GetConfig()
{
	return g_pMemConf ? g_pMemConf->GetMemMap() : NULL;
}
LPG_CTPI GetConfigTeaching()
{
	return g_pMemConfTeaching ? g_pMemConfTeaching->GetMemMap() : NULL;
}
LPG_CTEI GetConfigExpoParam()
{
	return g_pMemConfExpoParam ? g_pMemConfExpoParam->GetMemMap() : NULL;
}
LPG_CWTI GetConfigTracking()
{
	return g_pMemConfTracking ? g_pMemConfTracking->GetMemMap() : NULL;
}
LPG_LDSM GetShMemLuria()
{
	if (!g_pMemLuria)	return NULL;
	return g_pMemLuria->GetMemMap();
}
LPG_MDSM GetShMemMC2()
{
	if (!g_pMemMC2)	return NULL;
	return g_pMemMC2->GetMemMap();
}
LPG_PDSM GetShMemPLC()
{
	if (!g_pMemPLC)	return NULL;
	return g_pMemPLC->GetMemMap();
}
LPG_PDSM GetShMemStruct()
{
	if (!g_pMemPLC)	return NULL;
	return (LPG_PDSM)g_pMemPLC->GetMemMap();
}

/*
 desc : Check whether to use it
 parm : id	- [in]  Library Identifier
 retn : TRUE or FALSE
*/
BOOL IsUseLib(ENG_EDIC id)
{
	switch (id)
	{
	case ENG_EDIC::en_lspa		:	if (g_pMemMPA)	return TRUE;	break;	/* Prealigner Library */
	case ENG_EDIC::en_lsdr		:	if (g_pMemMDR)	return TRUE;	break;	/* Diamond Robot Library */
	case ENG_EDIC::en_efu_ss	:	if (g_pMemEFU)	return TRUE;	break;	/* EFU Library */
	}

	AfxMessageBox(L"The library is inactive mode\r\nCheck the config file (gen2i.inf)", MB_ICONSTOP|MB_TOPMOST);

	return FALSE;
}
