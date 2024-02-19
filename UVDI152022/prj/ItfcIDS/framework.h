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


/* Structure Header (2개의 define.h 문의 순서와 enum.h 보다 먼저 와야 됨) */
#include "../../inc/conf/global.h"
#include "../../inc/conf/define.h"
#include "../../inc/conf/enum.h"
#include "../../inc/conf/vision.h"
#include "../../inc/conf/tcpip.h"

/* Class Header */
#include "../../inc/comn/UniToChar.h"
#include "../../inc/comn/MySection.h"

/* Library Header */
#include "../../inc/itfc/ItfcCmn.h"
#include "../../inc/itfc/ItfcLogs.h"
#include "../../inc/itfc/ItfcMIL.h"

/* Load Library */
/* Include files to use the IDS API. */
#include <uEye.h>
#pragma comment (lib, "uEye_api_64.lib")

#ifdef _DEBUG
	#pragma comment(lib, "../../lib/debug/ItfcMILx64D.lib")
#else
	#pragma comment(lib, "../../lib/release/ItfcMILx64.lib")
#endif
