
#pragma once

// Intel 경고 무시하기
#if 0
#pragma warning(disable : 1885)	// #pragma region unclosed at end of file
#else
#pragma warning(disable : 4996)	// Winsock2 
#endif

/* -------------------- */
/* Concurrency library  */
/* -------------------- */
#include <ppl.h>
#if 1800 == _MSC_VER
using namespace concurrency;
#else
using namespace Concurrency;
#endif

// MFC Header
#include <afxmt.h>
#include <atlcoll.h>

/* Internal Header */
#include <cmath>
#include <vector>
#include <algorithm>

/* Windows Socket Header */
#include <winsock.h>
#include <WS2tcpip.h>

/* <!!! Important !!!>*/
#include "define.h"

/* use the standard library */
using namespace std;


/* --------------------------------------------------------------------------------------------- */
/*                                           열거형                                              */
/* --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- */
/*                                           구조체                                              */
/* --------------------------------------------------------------------------------------------- */
class CDPoint
{
public:
	double x, y;
	CDPoint()
	{
		x = y = 0.0;
	}
};

#pragma pack (push, 8)

/* Value Structure : UINT32 X/Y */
typedef struct __st_coord_unsigned16_x_y__
{
	UINT16				x, y;
	UINT16				u16_reserved[2];

	VOID SWAP()
	{
		x	= SWAP16(x);
		y	= SWAP16(y);
	}

}	STG_U16XY,	*LPG_U16XY;

/* Position Structure : INT32 X/Y */
typedef struct __st_coord_integer32_x_y__
{
	INT32				x, y;

	VOID SWAP()
	{
		x	= SWAP32(x);
		y	= SWAP32(y);
	}

}	STG_I32XY,	*LPG_I32XY;

/* Position Structure : UINT32 X/Y */
typedef struct __st_coord_unsigned32_x_y__
{
	UINT32				x, y;

	VOID SWAP()
	{
		x	= SWAP32(x);
		y	= SWAP32(y);
	}

}	STG_U32XY,	*LPG_U32XY;

/* Position Structure : DOUBLE X/Y */
typedef struct __st_coord_double_x_y__
{
	DOUBLE				x, y;

}	STG_DBXY,	*LPG_DBXY;
/* 좌표 구조체 : DOUBLE Width / Height */
typedef struct __st_coord_double_width_height__
{
	DOUBLE				cx, cy;

}	STG_DBWH,	*LPG_DBWH;

/* 트레이 아이콘 사라지게 하기 위한 구조체 */
typedef struct __st_tray_icon_refresh_data__
{
	UINT32				id;
	UINT32				cb_msg;
	// Handle Sizes : 8 Bytes
	HWND				hwnd;
	HICON				hicon;

}	STG_TIRD,	*LPG_TIRD;

#pragma pack (pop)	/* 8 bytes order */