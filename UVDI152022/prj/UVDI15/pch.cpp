// pch.cpp: 미리 컴파일된 헤더에 해당하는 소스 파일

#include "pch.h"

/* ---------------------------------------------------------------------------------------------- */
/*                                            전역 변수                                           */
/* ---------------------------------------------------------------------------------------------- */
TCHAR									g_tzWorkDir[MAX_PATH_LEN]	= {NULL};
LOGFONT									g_lf[eFONT_MAX] = {NULL};
CFont									g_font[eFONT_MAX];

COLORREF								g_clrBtnColor				= DEF_COLOR_BTN_PAGE_NORMAL;
COLORREF								g_clrBtnTextColor			= DEF_COLOR_BTN_PAGE_NORMAL_TEXT;
COLORREF								g_clrBtnSelColor			= DEF_COLOR_BTN_PAGE_SELECT;
COLORREF								g_clrBtnSelTextColor		= DEF_COLOR_BTN_PAGE_SELECT_TEXT;
UINT									g_u8Romote					= 0;
UINT16									g_u16PhilCommand			= 0;
#ifdef _DEBUG
UINT									g_loginLevel				= (UINT)EN_LOGIN_LEVEL::eLOGIN_LEVEL_ADMIN;
#else
UINT									g_loginLevel = (UINT)EN_LOGIN_LEVEL::eLOGIN_LEVEL_UNKNOWN;  
#endif