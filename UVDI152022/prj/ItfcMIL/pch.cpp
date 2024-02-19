// pch.cpp: 미리 컴파일된 헤더에 해당하는 소스 파일

#include "pch.h"

/* --------------------------------------------------------------------------------------------- */
/*                                        Global parameter                                       */
/* --------------------------------------------------------------------------------------------- */

TCHAR		g_tzWorkDir[MAX_PATH_LEN]	= {NULL};
LPG_CIEA	g_pstConfig					= NULL;

/* --------------------------------------------------------------------------------------------- */
/*                                        Global function                                        */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Returns the value of the entire envirnment
 parm : None
 retn : Structure pointer
*/
LPG_CIEA GetConfig()
{
	return g_pstConfig;
}

/*
 desc : Check that the engine is running
 parm : None
 retn : TRUE or FALSE
*/
BOOL IsRunDemo()
{
	if (!g_pstConfig)	return TRUE;
	return g_pstConfig->IsRunDemo();
}
