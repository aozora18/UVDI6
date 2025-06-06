﻿// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"

/* ---------------------------------------------------------------------------------------------- */
/*                                           전역 메시지                                          */
/* ---------------------------------------------------------------------------------------------- */

#define WM_MAIN_THREAD							WM_USER + 1000


/* ---------------------------------------------------------------------------------------------- */
/*                                            전역 상수                                           */
/* ---------------------------------------------------------------------------------------------- */



/* ---------------------------------------------------------------------------------------------- */
/*                                             윈도 ID                                            */
/* ---------------------------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------------------------- */
/*                                            일반 문자                                           */
/* ---------------------------------------------------------------------------------------------- */



/* ---------------------------------------------------------------------------------------------- */
/*                                            문서 제목                                           */
/* ---------------------------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------------------------- */
/*                                            전역 변수                                           */
/* ---------------------------------------------------------------------------------------------- */

extern	TCHAR									g_tzWorkDir[MAX_PATH_LEN];
extern	LOGFONT									g_lf;


/* ---------------------------------------------------------------------------------------------- */
/*                                             열거형                                             */
/* ---------------------------------------------------------------------------------------------- */

/* 작업 종류 */
typedef enum class __en_base_work_operate_kind__ : UINT8
{
	en_work_none	= 0x00,
	en_mark_move	= 0x01,		/* Align Mark 위치로 이동 */
	en_mark_grab	= 0x02,		/* Align Mark 위치에서 마크 인식 */
	en_mark_test	= 0x04,		/* Align Mark 검사 */
	en_expo_only	= 0x05,		/* Only 노광 진행 */
	en_expo_mark	= 0x06,		/* Align Mark 진행 후 노광 진행 */
	en_expo_cali	= 0x07,		/* Align Mark 및 보정 적용 후 노광 진행 */

}	ENG_BWOK;


/* ---------------------------------------------------------------------------------------------- */
/*                                             구조체                                             */
/* ---------------------------------------------------------------------------------------------- */
#pragma pack (push, 8)


#pragma pack (pop)

/* ---------------------------------------------------------------------------------------------- */
/*                                            전역 함수                                           */
/* ---------------------------------------------------------------------------------------------- */


#endif //PCH_H
