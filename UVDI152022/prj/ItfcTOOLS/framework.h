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
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
#include "../../inc/conf/conf_podis.h"
#include "../../inc/conf/recipe_podis.h"
#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
#include "../../inc/conf/conf_gen2i.h"
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
#endif
#include "../../inc/conf/conf_uvdi15.h"

/* Library Header */
#include "../../inc/conf/luria.h"
#include "../../inc/conf/mc2.h"
#include "../../inc/conf/mcdrv.h"
#include "../../inc/conf/philhmi_podis.h"
#include "../../inc/conf/plc_addr_podis.h"
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID|| \
	 CUSTOM_CODE_PODIS_LLS15 == DELIVERY_PRODUCT_ID)

#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
//#include "../../inc/conf/plc_addr_gen2i.h"
#include "../../inc/conf/wafer.h"
#include "../../inc/conf/milara.h"
#include "../../inc/conf/pm100.h"
#include "../../inc/conf/bsa.h"
#include "../../inc/conf/efu.h"
#endif

/* Common Class Header */
#include "../../inc/comn/MyMutex.h"
#include "../../inc/comn/UniToChar.h"

/* Engine Class Header */
#include "../../inc/engine/ConfComn.h"		/* 환경 파일 for Common */
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
#include "../../inc/engine/ConfPodis.h"		/* 환경 파일 for PODIS Tools */
#include "../../inc/engine/MemPodis.h"		/* Shared Memory Objects */
#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
#include "../../inc/engine/ConfGen2i.h"		/* 환경 파일 for PODIS Tools */
#include "../../inc/engine/MemGen2i.h"		/* Shared Memory Objects */
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
#include "../../inc/engine/ConfUVDI15.h"	/* 환경 파일 for UVDI15 */
#include "../../inc/engine/MemUVDI15.h"		/* Shared Memory Objects */
#endif
#include "../../inc/engine/MainThread.h"	/* Main Thread */

/* User Class Header */
#include "../../inc/mesg/CodeToStr.h"
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
#include "../../inc/recipe/LedPower.h"
#include "../../inc/recipe/ThickCali.h"
#include "../../inc/recipe/Recipe.h"
#include "../../inc/recipe/Mark.h"
#endif
#include "../../inc/recipe/PhStep.h"

/* Interface Header */
#include "../../inc/itfc/ItfcCmn.h"
#include "../../inc/itfc/ItfcLogs.h"
#include "../../inc/itfc/ItfcLuria.h"
#include "../../inc/itfc/ItfcMC2.h"
#include "../../inc/itfc/ItfcMCQDrv.h"
#include "../../inc/itfc/ItfcPM100D.h"
#include "../../inc/itfc/ItfcCalc.h"
#include "../../inc/itfc/ItfcBasler.h"

#include "../../inc/itfc/ItfcMvenc.h"
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID|| \
	 CUSTOM_CODE_PODIS_LLS15 == DELIVERY_PRODUCT_ID)
#include "../../inc/itfc/ItfcTrig.h"
#endif

#ifdef _DEBUG
#pragma comment(lib, "../../lib/debug/ItfcMvencx64D.lib")
#else
#pragma comment(lib, "../../lib/release/ItfcMvencx64.lib")
#endif


/* 최대 등록 가능한 Photohead Step 개수 (레시피에 등록 가능한 개수) */
#define MAX_PH_STEP_LINES	255	/* 1 바이트로 환경 (구조체) 파일에 설정 되어 있으므로 */
