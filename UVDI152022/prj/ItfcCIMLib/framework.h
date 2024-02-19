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


/* atl header */
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <atlsafe.h>
#include <atlconv.h>
#include <atlcoll.h>
#include <comutil.h>

/* CIMLib. 내부에 WorkThread 사용 여부  */
#define	USE_WORK_THREAD		0
#define USE_SIMULATION_CODE	0	/* CIMETRIX Korea 측에서 만들어 준 시뮬레이션 동작 코드 동작 여부 */

/* User Header */
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
