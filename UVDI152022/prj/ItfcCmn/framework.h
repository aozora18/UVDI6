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


/* Define Header */
#include "../../inc/conf/global.h"
#include "../../inc/conf/define.h"
#include "../../inc/conf/enum.h"

/* Class Header */
#include "../../inc/comn/UniToChar.h"

/* Use the CxImage */
#include "../../../Common/CxImage-7.02/include/ximage.h"
#ifdef _DEBUG
#pragma comment(lib, "../../../Common/CxImage-7.02/lib/debug/CxImagex64D.lib")
#pragma comment(lib, "../../../Common/CxImage-7.02/lib/debug/jasperx64D.lib")
#pragma comment(lib, "../../../Common/CxImage-7.02/lib/debug/jbigx64D.lib")
#pragma comment(lib, "../../../Common/CxImage-7.02/lib/debug/jpegx64D.lib")
#pragma comment(lib, "../../../Common/CxImage-7.02/lib/debug/libdcrx64D.lib")
#pragma comment(lib, "../../../Common/CxImage-7.02/lib/debug/libpsdx64D.lib")
#pragma comment(lib, "../../../Common/CxImage-7.02/lib/debug/mngx64D.lib")
#pragma comment(lib, "../../../Common/CxImage-7.02/lib/debug/pngx64D.lib")
#pragma comment(lib, "../../../Common/CxImage-7.02/lib/debug/tiffx64D.lib")
#pragma comment(lib, "../../../Common/CxImage-7.02/lib/debug/zlibx64D.lib")
#else
#pragma comment(lib, "../../../Common/CxImage-7.02/lib/release/CxImagex64.lib")
#pragma comment(lib, "../../../Common/CxImage-7.02/lib/release/jasperx64.lib")
#pragma comment(lib, "../../../Common/CxImage-7.02/lib/release/jbigx64.lib")
#pragma comment(lib, "../../../Common/CxImage-7.02/lib/release/jpegx64.lib")
#pragma comment(lib, "../../../Common/CxImage-7.02/lib/release/libdcrx64.lib")
#pragma comment(lib, "../../../Common/CxImage-7.02/lib/release/libpsdx64.lib")
#pragma comment(lib, "../../../Common/CxImage-7.02/lib/release/mngx64.lib")
#pragma comment(lib, "../../../Common/CxImage-7.02/lib/release/pngx64.lib")
#pragma comment(lib, "../../../Common/CxImage-7.02/lib/release/tiffx64.lib")
#pragma comment(lib, "../../../Common/CxImage-7.02/lib/release/zlibx64.lib")
#endif
