/****************************************************************************
*****************************************************************************
**
** File Name
** ----------
**
** AXL.H
**
** COPYRIGHT (c) AJINEXTEK Co., LTD
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
** Ajinextek Library Header File
** 
**
*****************************************************************************
*****************************************************************************
**
** Source Change Indices
** ---------------------
** 
** (None)
**
**
*****************************************************************************
*****************************************************************************
**
** Website
** ---------------------
**
** http://www.ajinextek.com
**
*****************************************************************************
*****************************************************************************
*/

#ifndef __AXT_AXL_H__
#define __AXT_AXL_H__

#include "AXHS.h"
//#include "AXA.h"
//#include "AXD.h"
//#include "AXM.h"

#ifdef __cplusplus
extern "C" {
#endif    //__cplusplus

//========== 라이브러리 초기화 =========================================================================

    // 라이브러리 초기화
    DWORD   __stdcall AxlOpen(long lIrqNo);

    // 라이브러리 초기화시 하드웨어 칩에 리셋을 하지 않음.
    DWORD   __stdcall AxlOpenNoReset(long lIrqNo);

    // 라이브러리 사용을 종료
    BOOL    __stdcall AxlClose();

    // 라이브러리가 초기화 되어 있는 지 확인
    BOOL    __stdcall AxlIsOpened();

    // 인터럽트를 사용한다.
    DWORD   __stdcall AxlInterruptEnable();

    // 인터럽트를 사용안한다.
    DWORD   __stdcall AxlInterruptDisable();

//========== 라이브러리 및 베이스 보드 정보 ============================================================
    // 등록된 베이스 보드의 개수 확인
    DWORD   __stdcall AxlGetBoardCount(long *lpBoardCount);
    // 라이브러리 버전 확인, szVersion[64]
    DWORD   __stdcall AxlGetLibVersion(char *szVersion);

    // Network제품의 각 모듈별 연결상태를 확인하는 함수
    DWORD   __stdcall AxlGetModuleNodeStatus(long lBoardNo, long lModulePos);

    // 해당 보드가 제어 가능한 상태인지 반환한다.
    DWORD   __stdcall AxlGetBoardStatus(long lBoardNo);

    // Network 제품의 Configuration Lock 상태를 반환한다.
    // *wpLockMode  : DISABLE(0), ENABLE(1)
    DWORD __stdcall AxlGetLockMode(long lBoardNo, WORD* wpLockMode);

	// 반환값에 대한 설명을 반환한다.
	DWORD   __stdcall AxlGetReturnCodeInfo(DWORD dwReturnCode, long lReturnInfoSize, long* lpRecivedSize, char* szReturnInfo);

//========== 로그 레벨 =================================================================================
    // EzSpy에 출력할 메시지 레벨 설정
    // uLevel : 0 - 3 설정
    // LEVEL_NONE(0)    : 모든 메시지를 출력하지 않는다.
    // LEVEL_ERROR(1)   : 에러가 발생한 메시지만 출력한다.
    // LEVEL_RUNSTOP(2) : 모션에서 Run / Stop 관련 메시지를 출력한다.
    // LEVEL_FUNCTION(3): 모든 메시지를 출력한다.
    DWORD   __stdcall AxlSetLogLevel(DWORD uLevel);
    // EzSpy에 출력할 메시지 레벨 확인
    DWORD   __stdcall AxlGetLogLevel(DWORD *upLevel);

//========== MLIII =================================================================================
    // Network제품의 각 모듈을 검색을 시작하는 함수
    DWORD   __stdcall AxlScanStart(long lBoardNo, long lNet);
    // Network제품 각 보드의 모든 모듈을 connect하는 함수
    DWORD   __stdcall AxlBoardConnect(long lBoardNo, long lNet);
    // Network제품 각 보드의 모든 모듈을 Disconnect하는 함수
    DWORD   __stdcall AxlBoardDisconnect(long lBoardNo, long lNet);

//========== SIIIH =================================================================================
    // SIIIH 마스터 보드에 연결된 모듈에 대한 검색을 시작하는 함수(SIIIH 마스터 보드 전용)
    DWORD   __stdcall AxlScanStartSIIIH(SCAN_RESULT* pScanResult = NULL);

//========== Fan Control ===============================================================================
    // 보드에 장착되어 있는 Fan의 속도(rpm)를 확인한다.
    DWORD   __stdcall AxlReadFanSpeed(long lBoardNo, double *dpFanSpeed);

//========== EzSpy =================================================================================
	// EzSpy User Log(Max length : 200 Bytes)
	DWORD   __stdcall AxlEzSpyUserLog(char* szUserLog);

#ifdef __cplusplus
}
#endif    //__cplusplus

#endif    //__AXT_AXL_H__
