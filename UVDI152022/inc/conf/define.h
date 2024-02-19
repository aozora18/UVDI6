/*
 desc : ���� Define �� ����
*/

#pragma once

#include <math.h>

/* --------------------------------------------------------------------------------------------- */
/*                                    Ignore certain warnings                                    */
/* --------------------------------------------------------------------------------------------- */
#pragma warning(disable: 6387)
#pragma warning(disable: 26451)

/* --------------------------------------------------------------------------------------------- */
/*      ���� ȣȯ ������ ��� �޽��� ��� �κ��� ������ ��µ��� �ʵ��� ó���ϱ� ���� ��ũ��     */
/* --------------------------------------------------------------------------------------------- */
#define	_WINSOCK_DEPRECATED_NO_WARNINGS

/* Luria�� Ư�� ���� ���� �ڵ����� ȣ�� ���� */
#define USE_PREPORCESSING_LIB					0	/* PreProcessing Library ��� ���� */

/* --------------------------------------------------------------------------------------------- */
/*                                 �ܺ� �������̽� ȣ�� ������                                   */
/* --------------------------------------------------------------------------------------------- */

#define	API_EXPORT								__declspec(dllexport)
#define	API_IMPORT								__declspec(dllimport)

/* --------------------------------------------------------------------------------------------- */
/*                                  ��Ÿ ���� �� �Լ� ������                                     */
/* --------------------------------------------------------------------------------------------- */

#define WIDEN(x)								L ## x
#define	WIDEN2(x)								WIDEN(x)
#define WFILE									WIDEN2(__FILE__)
#define WFUNC									WIDEN2(__FUNCTION__)
#define	WLINE									__LINE__

#define PDOUBLE									DOUBLE*

/* --------------------------------------------------------------------------------------------- */
/*                                    ���� �޸� ���� �̸�                                      */
/* --------------------------------------------------------------------------------------------- */

#define	SH_MEM_CONFIG							L"BRainX1.Shared.Memory.Config"
#define	SH_MEM_CONFIG_TEACH						L"BRainX1.Shared.Memory.Config.Teaching"
#define	SH_MEM_CONFIG_EXPO_PARAM				L"BRainX1.Shared.Memory.Config.ExpoParam"
#define	SH_MEM_EFEM								L"BRainX1.Shared.Memory.EFEM"
#define	SH_MEM_LOGS								L"BRainX1.Shared.Memory.Logs"
#define	SH_MEM_LURIA							L"BRainX1.Shared.Memory.Luria"
#define	SH_MEM_PLC								L"BRainX1.Shared.Memory.PLC"
#define	SH_MEM_MC2								L"BRainX1.Shared.Memory.MC2"
#define	SH_MEM_MC2B								L"BRainX1.Shared.Memory.MC2b"
#define	SH_MEM_TRIG								L"BRainX1.Shared.Memory.Trigger"
#define	SH_MEM_VISI								L"BRainX1.Shared.Memory.Vision"
#define	SH_MEM_BSA								L"BRainX1.Shared.Memory.BSA"
#define	SH_MEM_PM100D							L"BRainX1.Shared.Memory.PM100D"
#define	SH_MEM_PH_STEP							L"BRainX1.Shared.Memory.PhStepY"
#define	SH_MEM_PH_CORRECT_Y						L"BRainX1.Shared.Memory.PhCorrectionY"
#define	SH_MEM_LSPA								L"BRainX1.Shared.Memory.PreAligner"
#define	SH_MEM_LSDR								L"BRainX1.Shared.Memory.DiamondRobot"
#define	SH_MEM_EFU_SS							L"BRainX1.Shared.Memory.EFUShinsung"
#define	SH_MEM_TRACKING							L"BRainX1.Shared.Memory.WaferTracking"
#define	SH_MEM_PHILHMI							L"BRainX1.Shared.Memory.Philhmi"
#define	SH_MEM_STROBE_LAMP						L"BRainX1.Shared.Memory.StrobeLamp"

/* --------------------------------------------------------------------------------------------- */
/*                                          ��� �� ����                                         */
/* --------------------------------------------------------------------------------------------- */

#define MAX_PATH_COUNT							9
#define MAX_PATH_LEN							512
#define MAX_FILE_LEN							256
#define IPv4_LENGTH								16

#define	BYTE_SIZE_K								(1024.0)											/* KBytes -> Bytes */
#define	BYTE_SIZE_M								(1024.0 * 1024.0)									/* MBytes -> Bytes */
#define	BYTE_SIZE_G								(1000.0 * 1024.0 * 1024.0)							/* GBytes -> Bytes */

#define	BMP_HEAD_SIZE							62													/* Bitmap Header ���� ũ�� */

#define	LOG_MESG_SIZE							512

/* --------------------------------------------------------------------------------------------- */
/*                                       ������Ʈ ���� ID                                        */
/* --------------------------------------------------------------------------------------------- */

/* ������Ʈ ��ǰ�� �ڵ� �� */
#define	CUSTOM_CODE_PODIS_LLS10					0x1110	/* PH Z Axis Motor : Stepper */
#define	CUSTOM_CODE_PODIS_LLS06					0x1106	/* PH Z Axis Motor : Linear Motor */
#define	CUSTOM_CODE_GEN2I						0x2000
#define	CUSTOM_CODE_HANWHA_QCELL				0x3000
#define	CUSTOM_CODE_UTRONIX						0x4000
#define CUSTOM_CODE_UVDI15						0x5000 /*Absoilst  UVDI 15um*/
#define CUSTOM_CODE_HDDI6						0x6000 /*Absoilst  HDDI 6um*/

//#define DELIVERY_PRODUCT_ID					CUSTOM_CODE_PODIS_LLS10
#define DELIVERY_PRODUCT_ID						CUSTOM_CODE_UVDI15


#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
#define	CUSTOM_DATA_CONFIG					L"data"
#define	CUSTOM_DATA_CONFIG2					"data"
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
#define	CUSTOM_DATA_CONFIG					L"HDDI6_data"
#define	CUSTOM_DATA_CONFIG2					"HDDI6_data"
#endif

/* Align Camera 2D Calibration ��� ���� */
//#define USE_ALIGN_CAMERA_2D_CALI_VER			0x10	/* 0x10 or 0x20 or 0x21 */
#define USE_ALIGN_CAMERA_2D_CALI_VER			0x20	/* 0x10 or 0x20 or 0x21 */
//#define USE_ALIGN_CAMERA_2D_CALI_VER			0x21	/* 0x10 or 0x20 or 0x21 */
#define CUSTOM_CODE_TEST_PODIS					0		
#define CUSTOM_CODE_WAIT_JOB					1		/*���� �۾� �ʿ�*/
/* --------------------------------------------------------------------------------------------- */
/*                                     �ֿ� ��� ��� ����                                       */
/* --------------------------------------------------------------------------------------------- */

#if (TOUCH_KBD)
	#define USE_KEYBOARD_TYPE_TOUCH				1
#else
	#define USE_KEYBOARD_TYPE_TOUCH				0
#endif

#define USE_IO_LINK_PHILHMI						1	/*PHILHMI�� IO ��Ʈ�� ���*/
#define MC2_DRIVE_2SET							1	/*MC2 Drive 2Set ���*/
/* --------------------------------------------------------------------------------------------- */
/*                                      ������ ���� �ӵ� ��                                      */
/* --------------------------------------------------------------------------------------------- */

#define	HIGH_THREAD_SPEED						1
#ifndef _DEBUG
	#define	MEDIUM_THREAD_SPEED					10
	#define	NORMAL_THREAD_SPEED					50
	#define	SLOW_THREAD_SPEED					100
#else
	#define	MEDIUM_THREAD_SPEED					50
	#define	NORMAL_THREAD_SPEED					100
	#define	SLOW_THREAD_SPEED					250
#endif


/* --------------------------------------------------------------------------------------------- */
/*                                           Data Count                                          */
/* --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- */
/*                                          Data Length                                          */
/* --------------------------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------------------------- */
/*                                         Constant Value                                        */
/* --------------------------------------------------------------------------------------------- */

#define M_PI									3.14159265358979323846
#define M_PI_2									1.57079632679489661923
#define M_PI_4     								0.785398163397448309616
#define M2_PI      								(M_PI+M_PI)
#define DRIVEDIVIDE								0x06		/* mc2 ��⿡�� ���������� �ִ� ũ�� */

/* --------------------------------------------------------------------------------------------- */
/*                                         String Value                                          */
/* --------------------------------------------------------------------------------------------- */

#define ENCRYPT_PWD_DATA						"BlueRainX1"

/* --------------------------------------------------------------------------------------------- */
/*                                          ��ũ�� ����                                          */
/* --------------------------------------------------------------------------------------------- */

// ��Ʈ�� �̹��� ũ�� �� ���̿� ���� ũ�� ���� (4�� ����� �Ǿ�� ��)
#define	WIDTHBYTES(bpp, width)					(((bpp * width) + 31) / 32 * 4)
// �뱤�Ǵ� ���� �������� ũ��
#define	FRAME_SIZE_BYTES(width, Height)			((WIDTHBYTES(1, width)) * Height)
#define XGA_FRAME_BYTES							FRAME_SIZE_BYTES(1024, 768)

/* �Ҽ��� Ư�� �ڸ����� �ݿø� or �ݳ��� ��, ������ �Ҽ����� ���� */
#define ROUNDED(x, dig)							(floor(x * pow(double(10), dig) + 0.5f) / pow(double(10), dig))
/* �Ҽ��� Ư�� �ڸ����� ������ �ݿø� ��, ������ �Ҽ����� ���� */
#define ROUNDUP(x, dig)							(ceil(x * pow(double(10), dig)) / pow(double(10), dig))
/* �Ҽ��� Ư�� �ڸ����� ������ �ݳ��� ��, ������ �Ҽ����� ���� */
#define ROUNDDN(x, dig)							(floor(x * pow(double(10), dig)) / pow(double(10), dig))

// �յ� ����Ʈ ���� ��ġ ��ȯ
#define SWAP16(endian)							(((((endian) & 0xff) << 8) | (((endian) >> 8) & 0xff)))  
#define SWAP32(endian)							((										\
													(((endian) & 0xff000000) >> 24)	|	\
													(((endian) & 0x00ff0000) >> 8)	|	\
													(((endian) & 0x0000ff00) << 8)	|	\
													(((endian) & 0x000000ff) << 24)		\
												))
// ���� ���� 1 byte ���ۿ� ����
#define GET_BYTE_NUM2HEX(value, pos)			(((value) >> ((pos) << 3)) & 0xff)
// ���ڿ� ���̸� UINT32 ũ��� ��ȯ
#define STR_LEN(str)							UINT32(_tcslen(str))

// GetKeyState �� ����ϸ� �Ǵµ�, �����ϰ� ����϶�� �Ʒ��� ���� �������� ^^
#define	IsShiftKeyClick()						(0x8000 == (GetKeyState(VK_SHIFT)   & 0x8000))
#define	IsCtrlKeyClick()						(0x8000 == (GetKeyState(VK_CONTROL) & 0x8000))
#define IsAltKeyClik()							(0x8000 == (GetKeyState(VK_MENU)	& 0x8000))

/* ------------------------------------------------------------- */
/* ��Ʈ ������ ó���ϴ� ��ũ�� : �ڿ������� ��ȣ�� ���۵Ǵ� ��� */
/* ------------------------------------------------------------- */
#define CLEAR_BIT_R(data, pos)					((data) &= ~(0x01 << (pos)))	// �� bit Clear
#define CLEAR_BITS_R(data, area, pos)			((data) &= ~((area) << (pos)))	// ���ӵ� bit Clear
#define SET_BIT_R(data, pos)					((data) |= (0x01 << (pos)))		// �� bit Set
#define SET_BITS_R(data, pos)					((data) |= ((area) << (pos)))	// ���ӵ� bit
#define INVERT_BIT_R(data, pos)					((data) ^= (0x01 << (pos)))		// �� bit ����
#define INVERT_BITS_R(data, area, pos)			((data) ^= ((area) << (pos)))	// ���ӵ� bit ����
#define CHECK_BIT_R(data, pos)					((data) & (0x01 << (pos)))		// ��Ʈ �˻�
#define EXTRACT_BITS_R(data, area, pos)			(((data) >> (pos)) & (area))	// ��Ʈ ����
/* ------------------------------------------------------------- */
/* ��Ʈ ������ ó���ϴ� ��ũ�� : �տ������� ��ȣ�� ���۵Ǵ� ��� */
/* ------------------------------------------------------------- */
#define CLEAR_BIT_L(data, pos)					((data) &= ~(0x80 >> (pos)))	// �� bit Clear
#define CLEAR_BITS_L(data, area, pos)			((data) &= ~((area) >> (pos)))	// ���ӵ� bit Clear
#define SET_BIT_L(data, pos)					((data) |= (0x80 >> (pos)))		// �� bit Set
#define SET_BITS_L(data, pos)					((data) |= ((area) >> (pos)))	// ���ӵ� bit
#define INVERT_BIT_L(data, pos)					((data) ^= (0x80 >> (pos)))		// �� bit ����
#define INVERT_BITS_L(data, area, pos)			((data) ^= ((area) >> (pos)))	// ���ӵ� bit ����
#define CHECK_BIT_L(data, pos)					((data) & (0x80 >> (pos)))		// ��Ʈ �˻�
#define EXTRACT_BITS_L(data, area, pos)			(((data) << (pos)) & (area))	// ��Ʈ ����

/* --------------------------------------------------------------------------------------------- */
/*                                     �α� ���̺귯�� ����                                      */
/* --------------------------------------------------------------------------------------------- */

/* Log Macro */
#define LOG_SAVED(apps, level, mesg)	\
	do {	\
		uvLogs_SaveLogs(ENG_EDIC(apps), ENG_LNWE(level), mesg, WFILE, WFUNC, WLINE);	\
	} while (0)
#define LOG_ERROR(apps, mesg)	\
	do {	\
		uvLogs_SaveLogs(ENG_EDIC(apps), ENG_LNWE::en_error, mesg, WFILE, WFUNC, WLINE);	\
	} while (0)
#define LOG_WARN(apps, mesg)	\
	do {	\
		uvLogs_SaveLogs(ENG_EDIC(apps), ENG_LNWE::en_warning, mesg, WFILE, WFUNC, WLINE);	\
	} while (0)
#define LOG_MESG(apps, mesg)	\
	do {	\
		uvLogs_SaveLogs(ENG_EDIC(apps), ENG_LNWE::en_normal, mesg, WFILE, WFUNC, WLINE);	\
	} while (0)

/* --------------------------------------------------------------------------------------------- */
/*                                     ���̺귯�� ��� ����                                      */
/* --------------------------------------------------------------------------------------------- */

#define USE_LIB_LURIA							1
#define USE_LIB_MC2								1
#define USE_LIB_PLC								1
#define USE_LIB_TRIG							1
#define USE_LIB_BASLER							1
#define	USE_LIB_IDS								1
#define	USE_STROBE_DEV							0

/* --------------------------------------------------------------------------------------------- */
/*                                          VISION ����                                          */
/* --------------------------------------------------------------------------------------------- */
#define	DISP_TYPE_EXPO							0
#define	DISP_TYPE_MARK							1
#define	DISP_TYPE_MARK_LIVE						2
#define	DISP_TYPE_MARKSET						3
#define	DISP_TYPE_MMPM							4
#define	DISP_TYPE_CALB_CAMSPEC					5
#define	DISP_TYPE_CALB_EXPO						6
#define	DISP_TYPE_CALB_ACCR						7

#define ACA2440_SIZE_X	2448
#define ACA2440_SIZE_Y	2048
#define ACA1600_SIZE_X	1600
#define ACA1600_SIZE_Y	1200
#define ACA1920_SIZE_X 1920
#define ACA1920_SIZE_Y 1200

#define VISION_FONT_TEXT	_T("���� ���")
#define VISION_FONT_BOLD	_T("���� ��� Bold")

// CColorStatic 
#define COLOR_SILVER			RGB(192, 192, 192)
#define COLOR_LIGHT_GRAY		RGB(150, 150, 150)
#define COLOR_GRAY				RGB(128, 128, 128)
#define COLOR_BLACK				RGB(  1,   1,   1)
#define COLOR_GREEN				RGB(  1, 255,   1)
#define COLOR_RED				RGB(255,   1,   1)
#define COLOR_BLUE				RGB(  1,   1, 255)
#define COLOR_WHITE				RGB(255, 255, 255)
#define COLOR_ORANGE			RGB(230, 160,  63)

// ALG
#define ALG_PAT		0
#define ALG_MOD		1

#define MARK_BUF_SIZE_X		700
#define MARK_BUF_SIZE_Y		700

//#define CAM_CNT				2
#define MARKDISP_CNT		4
//#define GRABMARK_CNT		4	// GRAB �ϴ� ��ũ ����
#define PART_CNT			8   // EXPO, MARK, LIVE, MARKSET, MMPM, CALB_CAM, CALB_EXPO, CALB_ACCR (DISP TYPE)

#define	GLOBAL_MARK			0
#define	LOCAL_MARK			1
#define	TMP_MARK			2
