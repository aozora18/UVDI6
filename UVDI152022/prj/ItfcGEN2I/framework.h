#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE 클래스입니다.
#include <afxodlgs.h>       // MFC OLE 대화 상자 클래스입니다.
#include <afxdisp.h>        // MFC 자동화 클래스입니다.
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC 데이터베이스 클래스입니다.
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO 데이터베이스 클래스입니다.
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxsock.h>


/* Global Header */
#include "../../inc/conf/global.h"
#include "../../inc/conf/define.h"
#include "../../inc/conf/enum.h"

/* Library Header */
#include "../../inc/conf/conf_gen2i.h"
#include "../../inc/conf/luria.h"
#include "../../inc/conf/mc2.h"
#include "../../inc/conf/mcdrv.h"
#include "../../inc/conf/milara.h"
#include "../../inc/conf/recipe_gen2i.h"
#include "../../inc/conf/plc_addr_gen2i.h"
#include "../../inc/conf/bsa.h"
#include "../../inc/conf/pm100.h"
#include "../../inc/conf/wafer.h"
#include "../../inc/conf/efu.h"

/* Common Class Header */
#include "../../inc/comn/MyMutex.h"
#include "../../inc/comn/UniToChar.h"

/* Engine Class Header */
#include "../../inc/engine/ConfComn.h"		/* 환경 파일 for Common */
#include "../../inc/engine/ConfGen2i.h"		/* 환경 파일 for GEN2i */
#include "../../inc/engine/ConfTeaching.h"	/* 환경 파일 for GEN2i (Only Teaching) */
#include "../../inc/engine/ConfTracking.h"	/* 환경 파일 for GEN2i (Only Wafer Tracking) */
#include "../../inc/engine/ConfExpoParam.h"	/* 환경 파일 for GEN2i (Only Teaching) */
#include "../../inc/engine/MemGen2i.h"		/* Shared Memory Objects */
#include "../../inc/engine/MainThread.h"	/* Main Thread */

/* User Class Header */
#include "../../inc/mesg/CodeToStr.h"
#include "../../inc/recipe/LedPower.h"
#include "../../inc/recipe/RecipeGen2i.h"

/* Interface Header */
#include "../../inc/itfc/ItfcCmn.h"
#include "../../inc/itfc/ItfcLogs.h"
#include "../../inc/itfc/ItfcLuria.h"
#include "../../inc/itfc/ItfcMC2.h"
#include "../../inc/itfc/ItfcMCQDrv.h"
#include "../../inc/itfc/ItfcMPA.h"
#include "../../inc/itfc/ItfcMDR.h"
#include "../../inc/itfc/ItfcEFU.h"
#include "../../inc/itfc/ItfcLitho.h"
#include "../../inc/itfc/ItfcBSA.h"

#if (USE_PREPORCESSING_LIB)
/* PreProcess Library */
namespace PreProApi {
	#include "../../inc/itfc/ItfcPrePro.h"
}
#ifdef _DEBUG
	#pragma comment(lib, "c:\\luria\\PreProcess\\PreProcessApi.lib")
#else
	#pragma comment(lib, "c:\\luria\\PreProcess\\PreProcessApi.lib")
#endif
#endif
