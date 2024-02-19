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
#include "../../inc/conf/pm100.h"
#include "../../inc/conf/conf_comn.h"

/* Class Header */
#include "../../inc/comn/UniToChar.h"
#include "../../inc/comn/MySection.h"

/* Library Header */
#include "../../inc/itfc/ItfcCmn.h"
#include "../../inc/itfc/ItfcTOOLS.h"
#include "../../inc/itfc/ItfcLogs.h"

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

/* The Job ID */
typedef enum class __en_work_job_operation_id__ : UINT8
{
	en_none		= 0x00,
	en_area		= 0x01,
	en_focus	= 0x02,
	en_power	= 0x03,
	en_meas		= 0x04,

}	ENG_WJOI;

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

/* The measurement information for finding a <central> location */
typedef struct __st_measure_info_center_location__
{
	UINT8				ph_no;				/* Photohead Number (0x01 ~ 0x08) */
	UINT8				led_no;				/* The measured LED Number (0x01 ~ 0x04, en_365nm ~ en_...) : 0x00 - Area, 0x01 - Focus */
	UINT8				mat_rows;			/* Axis-Y (Horz) Movement Count */
	UINT8				mat_cols;			/* Axis-X (Vert) Movement Count */
	UINT8				mat_updn;			/* AXIS-Z (UpDown) Movement Count */
	UINT8				u8_reserved[3];

	UINT16				drop_min_cnt[3];	/* The minimum number of discarded values among measured values : 0x00-Area, 0x01-Focus, 0x02 - Power */
	UINT16				drop_max_cnt[3];	/* The maximum number of discarded values among measured values : 0x00-Area, 0x01-Focus, 0x02 - Power */
	UINT16				pass_cnt[3];		/* This is a count to ignore when measuring LEDs : 0x00-Area, 0x01-Focus, 0x02 - Power */
	UINT16				valid_cnt[3];		/* Effective count when measuring LEDs : 0x00-Area, 0x01-Focus, 0x02 - Power */
	UINT16				amp_idx[2];			/* LED Power Index (0 ~ 4095) : 0x00 - Area, 0x01-Focus */
	UINT16				u16_reserved[2];

	DOUBLE				correct;		/* Illuminance value collected from sensor in idle sate */
	DOUBLE				cent_pos_x;		/* measurement start X position (unit: mm) */
	DOUBLE				cent_pos_y;		/* measurement start Y position (unit: mm) */
	DOUBLE				focus_pos_z;	/* measurement focs Z position (unit: mm) */
	DOUBLE				sensor_rate[4];	/* The Sensor Filter Magnification (Factor:365 ~ 405nm, 4ea) for illuminance measurement (Normal : 10x -> 1/10) */
	DOUBLE				move_dist_z;	/* The moving size interval of the z axis (unit: mm) */
	DOUBLE				move_dist_row;	/* Axis-Y Vert Movement interval (unit: mm) */
	DOUBLE				move_dist_col;	/* Axis-X Horz Movement interval (unit: mm) */

	/*
	 desc : The measurement position is obtained for each direction
	 parm : direct	- [in]  0x00 : Left, 0x01 : Right, 0x02 : Top, 0x03 : Bottom
							0x10 : Cent X, 0x20 : Cent Y
	 retn : Motion Position (Absolute value. unit: mm)
	*/
	DOUBLE GetPosXY(UINT8 direct)
	{
		switch (direct)
		{
		case 0x00 :	return (cent_pos_x - mat_cols * move_dist_col);	break;	/* Left */
		case 0x01 :	return (cent_pos_x + mat_cols * move_dist_col);	break;	/* Right */
		case 0x02 :	return (cent_pos_y - mat_rows * move_dist_row);	break;	/* Top */
		case 0x03 :	return (cent_pos_y + mat_rows * move_dist_row);	break;	/* Bottom */
		case 0x10 :	return (cent_pos_x);							break;	/* Center X */
		case 0x11 :	return (cent_pos_y);							break;	/* Center Y */
		}

		return DBL_MIN;
	}

	/*
	 desc : The measurement position is obtained for each direction
	 parm : direct	- [in]  0x00 : Left, 0x01 : Right, 0x02 : Top, 0x03 : Bottom
							0x10 : Cent X, 0x20 : Cent Y
			index	- [in]  Measurement Sequence (Range : INT16_MIN ~ INT16_MAX)
	 retn : Motion Position (Absolute value. unit: mm)
	*/
	DOUBLE GetPosXY(UINT8 direct, INT16 index)
	{
		switch (direct)
		{
		case 0x00 :																		/* Left */
		case 0x01 :	return (GetPosXY(direct) + move_dist_col * DOUBLE(index));	break;	/* Right */
		case 0x02 :																		/* Top */
		case 0x03 :	return (GetPosXY(direct) + move_dist_row * DOUBLE(index));	break;	/* Bottom */
		case 0x10 :	return (cent_pos_x + move_dist_col * DOUBLE(index));		break;	/* Center X */
		case 0x11 :	return (cent_pos_y + move_dist_row * DOUBLE(index));		break;	/* Center Y */
		}
		return DBL_MIN;
	}

	/*
	 desc : Get the number of rows / columns measured
	 parm : type	- [in]  0x00 : rows, 0x01 : columns
	 retn : The number of rows or columns
	*/
	UINT16 GetCountRowCol(UINT8 type)
	{
		if (0x00 == type)	return (mat_rows * 2 + 1);
		return (mat_cols * 2 + 1);
	}

	/*
	 desc : Get the number of measured
	 parm : None
	 retn : The number of UpDown
	*/
	UINT16 GetCountUpDown()
	{
		return (mat_updn * 2 + 1);
	}

	/*
	 desc : Calculates the start position
	 parm : direct	- [in]  0x00 : Up, 0x01 : Down
	 retn : TRUE or FALSE
	*/
	DOUBLE GetPosZ(UINT8 direct)
	{
		if (0x00 == direct) return (focus_pos_z + (mat_updn * move_dist_z));
		return (focus_pos_z - (mat_updn * move_dist_z));
	}

	/*
	 desc : Check if it is outside the measurement range
	 parm : min_x/y	- [in]  Area range min (unit: mm)
			max_x/y	- [in]  Area range max (unit: mm)
	 retn : 0x00 (Valid)
			0x01 (Left   X : Out)
			0x02 (Right  X : Out)
			0x03 (Top    Y : Out)			
			0x04 (Bottom Y : Out)
	*/
	UINT8 IsValidAreaRange(DOUBLE min_x, DOUBLE min_y, DOUBLE max_x, DOUBLE max_y)
	{
		if (GetPosXY(0x00) < min_x)	return 0x01;
		if (GetPosXY(0x01) > max_x)	return 0x02;
		if (GetPosXY(0x02) < min_y)	return 0x03;
		if (GetPosXY(0x03) > max_y)	return 0x04;
		return 0x00;
	}
	/*
	 desc : Check if it is outside the measurement range
	 parm : min_dn	- [in]  Focus range min (unit: mm)
			max_up	- [in]  Focus range max (unit: mm)
	 retn : 0x00 (Valid)
			0x01 (Min down : Out)
			0x02 (Max   up : Out)
	*/
	UINT8 IsValidFocusRange(DOUBLE min_dn, DOUBLE max_up)
	{
		if (focus_pos_z - (mat_updn * move_dist_z) < min_dn)	return 0x01;
		if (focus_pos_z + (mat_updn * move_dist_z) > max_up)	return 0x02;
		return 0x00;
	}

	/*
	 desc : Total number of repeated measurement
	 parm : type	- [in]  0x00 : Area, 0x01 : Focus, 0x02 : Power
	 retn : Total Repeated Count
	*/
	UINT16 GetTotalMeasCount(UINT8 type)
	{
		return valid_cnt[type]+pass_cnt[type];
	}

	/*
	 desc : Check whether the values are valid
	 parm : menu_id	- [in]  0x00 (Area), 0x01 (Focus)
	 retn : TRUE or FALSE
	*/
	BOOL IsValidArea()
	{
		return (ph_no > 0 && led_no > 0 && mat_cols > 0 && mat_rows > 0 && valid_cnt[0] > 0 &&
				amp_idx[0] > 0 && move_dist_col > 0.0f && move_dist_row > 0.0f && 
				sensor_rate[0] != 0.0f && sensor_rate[1] != 0.0f && sensor_rate[2] != 0.0f &&
				sensor_rate[3] != 0.0f && cent_pos_x > 0.0f && cent_pos_y > 0.0f &&
				valid_cnt[0] > ((drop_max_cnt[0]+drop_min_cnt[0])));
	}
	BOOL IsValidFocus()
	{
		return (ph_no > 0 && led_no > 0 && valid_cnt[1] > 0 && amp_idx[1] > 0 &&
				/*cent_pos_z > 0.0f && */mat_updn > 0 && move_dist_z > 0.0f &&
				sensor_rate[0] != 0.0f && sensor_rate[1] != 0.0f && sensor_rate[2] != 0.0f &&
				sensor_rate[3] != 0.0f && valid_cnt[1] > ((drop_max_cnt[1]+drop_min_cnt[1])));
	}
	BOOL IsValidMeas()
	{
		return (valid_cnt[2] > 0 &&
				sensor_rate[0] != 0.0f && sensor_rate[1] != 0.0f && sensor_rate[2] != 0.0f &&
				sensor_rate[3] != 0.0f && valid_cnt[2] > ((drop_max_cnt[2]+drop_min_cnt[2])));
	}
	BOOL IsValid(UINT8 menu_id)
	{
		BOOL bIsValid	= FALSE;
		switch (menu_id)
		{
		case 0x00 : bIsValid = IsValidArea();	break;
		case 0x01 : bIsValid = IsValidFocus();	break;
		case 0x02 : bIsValid = IsValidMeas();	break;
		}
		return bIsValid;
	}

}	STG_MICL,	*LPG_MICL;

#pragma pack (pop)	/* 8 bytes */
