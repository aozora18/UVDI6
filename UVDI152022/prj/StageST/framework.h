#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.
#include <afxdisp.h>        // MFC 자동화 클래스입니다.

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC의 리본 및 컨트롤 막대 지원

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

/* MFC Header */
#include <atlimage.h>

/* Structure Header (2개의 define.h 문의 순서와 enum.h 보다 먼저 와야 됨) */
#include "../../inc/conf/global.h"
#include "../../inc/conf/define.h"
#include "../../inc/conf/enum.h"
#include "../../inc/conf/conf_comn.h"

/* Class Header */
#include "../../inc/comn/UniToChar.h"
#include "../../inc/comn/MySection.h"

/* Library Header */
#include "../../inc/itfc/ItfcCmn.h"
#include "../../inc/itfc/ItfcLogs.h"
#include "../../inc/itfc/ItfcTOOLS.h"

/* Load the library */
#ifdef _DEBUG
	#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
		 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
		#pragma comment(lib, "../../lib/debug/ItfcTOOLSx64D.lib")
	#elif (CUSTOM_CODE_GEN2I == DELIVERY_PRODUCT_ID)
		#pragma comment(lib, "../../lib/debug/ItfcTOOLSTwcx64D.lib")
	#endif
#else
	#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
		 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
		#pragma comment(lib, "../../lib/release/ItfcTOOLSx64.lib")
	#elif (CUSTOM_CODE_GEN2I == DELIVERY_PRODUCT_ID)
		#pragma comment(lib, "../../lib/release/ItfcTOOLSTwcx64.lib")
	#endif
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                        Global Messages                                        */
/* --------------------------------------------------------------------------------------------- */

#define WM_MAIN_THREAD							WM_USER + 1001


/* --------------------------------------------------------------------------------------------- */
/*                                        Global Constants                                       */
/* --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- */
/*                                          Window IDs                                           */
/* --------------------------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------------------------- */
/*                                           Enumerate                                           */
/* --------------------------------------------------------------------------------------------- */

/* The Job States */
typedef enum class __en_work_job_state_id__ : UINT8
{
	en_init		= 0x00,	/* Default value */
	en_next		= 0x01,
	en_wait		= 0x02,
	en_comp		= 0x03,
	en_fail		= 0x04,
	en_tout		= 0x05,	/* A timeout occurred */
	en_stop		= 0x06,	/* Forcibly stop the job */

}	ENG_WJSI;


/* --------------------------------------------------------------------------------------------- */
/*                                           Structure                                           */
/* --------------------------------------------------------------------------------------------- */
#pragma pack (push, 8)

/* The search conditicon information for measurement */
typedef struct __st_search_condition_info_measurement__
{
	UINT16				target_x:1;			/* 0 : Stage X, 1 : Align Camera X */
	UINT16				acam_id:2;			/* Align Camera Number (1 or 2) */
	UINT16				direct:2;			/* 1:Move the stage to the X axis, 2:Move the stage to the Y axis, 3:Move the stage to the XY axis (Vector) */
	UINT16				count:10;			/* Measurement Count (Max: 1023) */
	UINT16				u16bit_reserved:1;
	UINT16				u16_reserved[3];
	DOUBLE				interval;			/* Search interval (unit: mm) */
	DOUBLE				start_x;			/* Start position (X) (unit: mm) */
	DOUBLE				start_y;			/* Start position (Y) (unit: mm) */

	/*
	 desc : Verify that the value is valid
	 parm : None
	 retn : None
	*/
	BOOL IsValid()
	{
		return (acam_id > 0 && direct > 0 && count > 0 && interval >= 1.0f &&
				start_x >= 0.0f && start_x < DBL_MAX &&
				start_y >= 0.0f && start_y < DBL_MAX);
	}

}	STG_SCIM,	*LPG_SCIM;

#pragma pack (pop)	/* 8 bytes */
