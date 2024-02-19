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
#include <afxdtctl.h>					// Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxsock.h>

/* Global Header */
#include "../../inc/conf/global.h"
#include "../../inc/conf/define.h"
#include "../../inc/conf/enum.h"
//#include "../../inc/conf/conf_uvdi15.h"
#include "../../inc/conf/conf_uvdi15.h"
#include "../../inc/conf/recipe_uvdi15.h"
#include "../../inc/conf/plc_addr_uvdi15.h"
#include "../../inc/conf/philhmi_uvdi15.h"
#include "../../inc/conf/strobe_lamp_uvdi15.h"

/* Library Header */
#include "../../inc/conf/luria.h"
#include "../../inc/conf/mc2.h"
#include "../../inc/conf/mcdrv.h"

/* Common Class Header */
#include "../../inc/comn/MyMutex.h"
#include "../../inc/comn/UniToChar.h"
#include "../../inc/comn/MySection.h"

/* Engine Class Header */
#include "../../inc/engine/ConfComn.h"		/* 환경 파일 for Common */
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15_LLS06)
#include "../../inc/engine/ConfUvdi15.h"		/* 환경 파일 for UVDI15 */
#include "../../inc/engine/MemUvdi15.h"		/* Shared Memory Objects */
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15|| \
DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
#include "../../inc/engine/ConfUVDI15.h"	/* 환경 파일 for UVDI15 */
#include "../../inc/engine/MemUVDI15.h"		/* Shared Memory Objects */
#endif
#include "../../inc/engine/MainThread.h"	/* Main Thread */

/* User Class Header */
#include "../../inc/recipe/LedPower.h"
#include "../../inc/recipe/ACamCali.h"
#include "../../inc/recipe/ThickCali.h"
#include "../../inc/recipe/RecipeUVDI15.h"
#include "../../inc/recipe/PhStep.h"
#include "../../inc/recipe/CorrectY.h"
#include "../../inc/recipe/MarkUVDI15.h"
#include "../../inc/mesg/CodeToStr.h"

/* Interface Header */
#include "../../inc/itfc/ItfcCmn.h"
#include "../../inc/itfc/ItfcLogs.h"
#include "../../inc/itfc/ItfcLuria.h"
#include "../../inc/itfc/ItfcMC2.h"
#include "../../inc/itfc/ItfcBasler.h"
//#include "../../inc/itfc/ItfcTrig.h"
#include "../../inc/itfc/ItfcCalc.h"

#include "../../inc/itfc/ItfcGentec.h"
#include "../../inc/itfc/ItfcMvenc.h"
#include "../../inc/itfc/ItfcPhilhmi.h"
#include "../../inc/itfc/ItfcStrobeLamp.h"
#include "../../inc/itfc/ItfcKeyenceLDS.h"


/* Load Library */
#ifdef _DEBUG
#pragma comment(lib, "../../lib/debug/ItfcGentecx64D.lib")
#else
#pragma comment(lib, "../../lib/release/ItfcGentecx64.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "../../lib/debug/ItfcMvencx64D.lib")
#else
#pragma comment(lib, "../../lib/release/ItfcMvencx64.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "../../lib/debug/ItfcPhilhmix64D.lib")
#else
#pragma comment(lib, "../../lib/release/ItfcPhilhmix64.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "../../lib/debug/ItfcStrobeLampx64D.lib")
#else
#pragma comment(lib, "../../lib/release/ItfcStrobeLampx64.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "../../lib/debug/ItfcKeyenceLDSx64D.lib")
#else
#pragma comment(lib, "../../lib/release/ItfcKeyenceLDSx64.lib")
#endif