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

/* Class Header */
#include "../../inc/comn/UniToChar.h"
#include "../../inc/comn/MySection.h"

/* Library Header */
#include "../../inc/itfc/ItfcCmn.h"
#include "../../inc/itfc/ItfcTOOLS.h"
#include "../../inc/conf/vision_uvdi15.h"

/* Load the library */
#ifdef _DEBUG
	#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
		 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID|| \
		 CUSTOM_CODE_PODIS_LLS15 == DELIVERY_PRODUCT_ID)
		#pragma comment(lib, "../../lib/debug/ItfcTOOLSx64D.lib")
	#elif (CUSTOM_CODE_GEN2I == DELIVERY_PRODUCT_ID)
		#pragma comment(lib, "../../lib/debug/ItfcTOOLSTwcx64D.lib")
	#endif
#else
	#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
		 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID|| \
		 CUSTOM_CODE_PODIS_LLS15 == DELIVERY_PRODUCT_ID)
		#pragma comment(lib, "../../lib/release/ItfcTOOLSx64.lib")
	#elif (CUSTOM_CODE_GEN2I == DELIVERY_PRODUCT_ID)
		#pragma comment(lib, "../../lib/release/ItfcTOOLSTwcx64.lib")
	#endif
#endif



/* Color Define */
#define	DEF_FONT_NAME						_T("Noto Sans")
#define	DEF_COLOR_BTN_MENU_NORMAL			RGB(70,		130,	180)
#define	DEF_COLOR_BTN_MENU_NORMAL_TEXT		RGB(230, 230, 230)
#define	DEF_COLOR_BTN_MENU_SELECT			RGB(0, 45, 116)
#define	DEF_COLOR_BTN_MENU_SELECT_TEXT		RGB(230, 230, 230)
#define	DEF_COLOR_BTN_PAGE_NORMAL			RGB(112,	128,	144)
#define	DEF_COLOR_BTN_PAGE_NORMAL_TEXT		RGB(230, 230, 230)
#define	DEF_COLOR_BTN_PAGE_SELECT			RGB(0, 45, 116)
#define	DEF_COLOR_BTN_PAGE_SELECT_TEXT		RGB(230, 230, 230)
//=============================================================================
//  Color Define
//=============================================================================
#define	LIGHT_PINK				RGB(255,	182,	193)
#define	PINK					RGB(255,	192,	203)
#define	CRIMSON					RGB(220,	20,		60)
#define	LAVENDER_BLUSH			RGB(255,	240,	245)
#define	PALE_VILOT_RED			RGB(219,	112,	147)
#define	HOT_PINK				RGB(255,	105,	180)
#define	DEEP_PINK				RGB(255,	20,		147)
#define	MEDIUM_VILOT_RED		RGB(199,	21,		133)
#define	ORCHID					RGB(218,	112,	214)
#define	THISTLE					RGB(216,	191,	216)
#define	PLUM					RGB(221,	160,	221)
#define	VIIOLET					RGB(238,	130,	238)
#define	MAGENTA					RGB(255,	0,		255)
#define	FUCHSIA					RGB(255,	0,		255)
#define	PURPLE					RGB(128,	0,		128)
#define	MEDIUM_ORCHID			RGB(186,	85,		211)
#define	DARK_VIOLET				RGB(148,	0,		211)
#define	DARK_ORCHID				RGB(153,	50,		204)
#define	INDIGO					RGB(75,		0,		130)
#define	BLUE_VIOLET				RGB(138,	43,		226)
#define	MEDIUM_PURPLE			RGB(147,	112,	219)
#define	MEDIUM_SLATE_BLUE		RGB(123,	104,	238)
#define	SLATE_BLUE				RGB(106,	90,		205)
#define	DARK_SLATE_BLUE			RGB(72,		61,		139)
#define	LAVENDER				RGB(230,	230,	250)
#define	GHOST_WHITE				RGB(248,	248,	255)
#define	MEDIUM_BLUE				RGB(0,		0,		205)
#define	MIDNIGHT_BLUE			RGB(25,		25,		112)
#define	NAVY					RGB(0,		0,		128)
#define	ROYAL_BLUE				RGB(62,		105,	225)
#define	CORNFLOWER_BLUE			RGB(100,	149,	237)
#define	LIGHT_STEEL_BLUE		RGB(176,	196,	222)
#define	LIGHT_SLATE_GRAY		RGB(119,	136,	153)
#define	SLATE_GRAY				RGB(112,	128,	144)
#define	DODGER_BLUE				RGB(30,		144,	255)
#define	ALICE_BLUE				RGB(240,	248,	255)
#define	STEEL_BLUE				RGB(70,		130,	180)
#define	LIGHT_SKY_BLUE			RGB(135,	206,	250)
#define	SKY_BLUE				RGB(135,	206,	235)
#define	DEEP_SKY_BLUE			RGB(0,		191,	255)
#define	POWEDER_BLUE			RGB(176,	224,	230)
#define	CADET_BLUE				RGB(176,	158,	160)
#define	AZURE					RGB(240,	255,	255)
#define	LIGHT_CYAN				RGB(224,	255,	255)
#define	PALE_TURQUOOISE			RGB(175,	238,	238)
#define	CYAN					RGB(0,		255,	255)
#define	AQUA					RGB(0,		255,	255)
#define	DARK_TURQUOISE			RGB(0,		206,	209)
#define	DARK_SLATE_GRAY			RGB(47,		79,		79)
#define	TEAL					RGB(0,		128,	128)
#define	MEDIUM_TURQUOISE		RGB(72,		209,	204)
#define	LIGHT_SEA_GREEN			RGB(32,		178,	170)
#define	TURQUOISE				RGB(64,		224,	208)
#define	AQUAMARINE				RGB(127,	255,	212)
#define	MEDIUM_AQUAMARINE		RGB(102,	205,	170)
#define	MEDIUM_SPRING_GREEN		RGB(0,		250,	154)
#define	MINT_CREAM				RGB(245,	255,	250)
#define	SPRING_GREEN			RGB(0,		255,	127)
#define	MEDIUM_SEA_GREEN		RGB(60,		179,	113)
#define	SEA_GREEN				RGB(46,		139,	87)
#define	HONEYDEW				RGB(240,	255,	240)
#define	PALE_GREEN				RGB(152,	251,	152)
#define	DARK_SEA_FREEN			RGB(143,	188,	143)
#define	LIME_GREEN				RGB(50,		205,	50)
#define	LIME					RGB(0,		255,	0)
#define	FOREST_GREEN			RGB(34,		139,	34)
#define	CHARTREUSE				RGB(127,	255,	0)
#define	LAWN_GREEN				RGB(124,	252,	0)
#define	GREEN_YELLOW			RGB(173,	255,	47)
#define GREEN_					RGB(  0,	127,	0)
#define	DARK_OLIVE_GREEN		RGB(85,		107,	47)
#define	YELLOW_GREEN			RGB(154,	205,	50)
#define BRIGHT_YELLOW			RGB(255,	255,	235)
#define	OLIVE_DRAB				RGB(107,	142,	35)
#define	BEIGE					RGB(245,	245,	220)
#define	LIGHT_GOLDENROD_YELLOW	RGB(250,	250,	210)
#define	IVORY					RGB(255,	255,	240)
#define	LIGHT_YELLOW			RGB(255,	255,	224)
#define	YELLOW					RGB(255,	255,	0)
#define	OLIVE					RGB(128,	128,	0)
#define	DARK_KHAKI				RGB(128,	128,	0)
#define	LEMON_CHIFFON			RGB(255,	250,	205)
#define	PALE_GOLDENROD			RGB(238,	232,	170)
#define	KHAKI					RGB(240,	230,	170)
#define	GOLD					RGB(255,	215,	0)
#define	CORNSILK				RGB(255,	248,	220)
#define	GOLDENROD				RGB(218,	165,	32)
#define	DARK_GOLDENROD			RGB(184,	134,	11)
#define	FLORAL_WHITE			RGB(255,	250,	240)
#define	OLD_LACE				RGB(253,	245,	230)
#define	WHEAT					RGB(245,	222,	179)
#define	MOCCASIN				RGB(255,	228,	181)
#define	ORANGE					RGB(255,	165,	0)
#define	PAPAYA_WHIP				RGB(255,	239,	213)
#define	BLANCHED_ALMOND			RGB(255,	235,	205)
#define	NAVAJO_WHITE			RGB(255,	222,	173)
#define	ANTIQUE_WHITE			RGB(250,	235,	215)
#define	TAN						RGB(210,	180,	140)
#define	BURLY_WOOD				RGB(222,	180,	135)
#define	BISQUE					RGB(255,	228,	196)
#define	DARK_ORANGE				RGB(255,	140,	230)
#define	LINEN					RGB(250,	240,	230)
#define	PERU					RGB(205,	133,	63)
#define	PEACH_PUFF				RGB(255,	218,	185)
#define	SANDY_BROWN				RGB(244,	164,	96)
#define	CHOCOLATE				RGB(210,	105,	30)
#define	SADDLE_BROWN			RGB(244,	164,	96)
#define	SEASHELL				RGB(255,	245,	238)
#define	SIENNA					RGB(160,	82,		45)
#define	LIGHT_SALMON			RGB(255,	160,	122)
#define	CORAL					RGB(255,	127,	80)
#define	ORANGE_RED				RGB(255,	69,		0)
#define	DARK_SALMON				RGB(233,	150,	122)
#define	TOMATO					RGB(255,	99,		71)
#define	MISTY_ROSE				RGB(255,	228,	225)
#define	SALMON					RGB(250,	128,	114)
#define	SNOW					RGB(255,	250,	250)
#define	LIGHT_CORAL				RGB(240,	128,	128)
#define	ROSY_BROWN				RGB(188,	143,	143)
#define	INDIAN_RED				RGB(205,	92,		92)
#define	BROWN					RGB(165,	42,		42)
#define	FIRE_BRICK				RGB(178,	34,		34)
#define	MAROON					RGB(128,	0,		0)
#define	WHITE_					RGB(255,	255,	255)
#define	WHITE_SMOKE				RGB(245,	245,	245)
#define	GAINSBORO				RGB(220,	220,	220)
#define	SILVER					RGB(192,	192,	192)
#define	DARKBLUE				RGB(000,	000,	128)
#define	DARK_GREEN				RGB(000,	064,	000)
#define	DARK_CYAN				RGB(000,	064,	064)	
#define	DARK_RED				RGB(064,	000,	000)
#define	DARK_MAGENTA			RGB(128,	000,	128)
#define	DARK_GRAY				RGB(128,	128,	128)
#define	LIGHT_GRAY				RGB(192,	192,	192)
#define	LIGHT_BLUE				RGB(000,	000,	255)
#define	LIGHT_GREEN				RGB(000,	255,	000)
#define	LIGHT_RED				RGB(255,	000,	000)
#define	LIGHT_MAGENTA			RGB(255,	000,	255)
#define SCRIPT					RGB(176,	6,		002)
#define	BLACK_					RGB(0,		0,		0)
#define	FORD_GRAPHITE			RGB(65,		65,		65)