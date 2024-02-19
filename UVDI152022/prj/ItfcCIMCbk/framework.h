#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED

#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 일부 CString 생성자는 명시적으로 선언됩니다.

#include <afxwin.h>
#include <afxext.h>
#include <afxole.h>
#include <afxodlgs.h>
#include <afxrich.h>
#include <afxhtml.h>
#include <afxcview.h>
#include <afxwinappex.h>
#include <afxframewndex.h>
#include <afxmdiframewndex.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdisp.h>        // MFC 자동화 클래스입니다.
#endif // _AFX_NO_OLE_SUPPORT

#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#include "resource.h"

/* atl header */
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <atlsafe.h>
#include <atlconv.h>
#include <atlcoll.h>

/* User header */
#include "../../inc/conf/global.h"
#include "../../inc/conf/define.h"
#include "../../inc/conf/cim300.h"

/* Class Header */
#include "../../inc/comn/UniToChar.h"
#include "../../inc/comn/MyMutex.h"
#include "../../inc/comn/MySection.h"
#include "../../inc/comn/ThinThread.h"
#include "../../inc/cim300/LogData.h"
#include "../../inc/cim300/SharedData.h"

/* Interface Header */
#include "../../inc/itfc/ItfcCmn.h"
