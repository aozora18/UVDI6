/****************************************************************************
*****************************************************************************
**
** File Name
** ----------
**
** AXD.H
**
** COPYRIGHT (c) AJINEXTEK Co., LTD
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
** Ajinextek Digital Library Header File
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

#ifndef __AXT_AXD_H__
#define __AXT_AXD_H__

#include "AXHS_SHIM.h"

#ifdef __cplusplus
extern "C" {
#endif    //__cplusplus

//========== 보드 및 모듈 정보 
    // DIO 모듈이 있는지 확인
    DWORD    __stdcall AxdInfoIsDIOModule(DWORD *upStatus);
    
    // DIO 모듈 No 확인
    DWORD    __stdcall AxdInfoGetModuleNo(long lBoardNo, long lModulePos, long *lpModuleNo);
    
    // DIO 입출력 모듈의 개수 확인
    DWORD    __stdcall AxdInfoGetModuleCount(long *lpModuleCount);

    // 지정한 모듈의 입력 접점 개수 확인
    DWORD    __stdcall AxdInfoGetInputCount(long lModuleNo, long *lpCount);
    
    // 지정한 모듈의 출력 접점 개수 확인
    DWORD    __stdcall AxdInfoGetOutputCount(long lModuleNo, long *lpCount);
    
    // 지정한 모듈 번호로 베이스 보드 번호, 모듈 위치, 모듈 ID 확인
    DWORD    __stdcall AxdInfoGetModule(long lModuleNo, long *lpBoardNo, long *lpModulePos, DWORD *upModuleID);

    // 해당 모듈이 제어가 가능한 상태인지 반환한다.
    DWORD    __stdcall AxdInfoGetModuleStatus(long lModuleNo);

//========== 인터럽트 설정 확인 
    // 지정한 모듈에 인터럽트 메시지를 받아오기 위하여 윈도우 메시지, 콜백 함수 또는 이벤트 방식을 사용
    //========= 인터럽트 관련 함수 ======================================================================================
    // 콜백 함수 방식은 이벤트 발생 시점에 즉시 콜백 함수가 호출 됨으로 가장 빠르게 이벤트를 통지받을 수 있는 장점이 있으나
    // 콜백 함수가 완전히 종료 될 때까지 메인 프로세스가 정체되어 있게 된다.
    // 즉, 콜백 함수 내에 부하가 걸리는 작업이 있을 경우에는 사용에 주의를 요한다. 
    // 이벤트 방식은 쓰레드등을 이용하여 인터럽트 발생여부를 지속적으로 감시하고 있다가 인터럽트가 발생하면 
    // 처리해주는 방법으로, 쓰레드 등으로 인해 시스템 자원을 점유하고 있는 단점이 있지만
    // 가장 빠르게 인터럽트를 검출하고 처리해줄 수 있는 장점이 있다.
    // 일반적으로는 많이 쓰이지 않지만, 인터럽트의 빠른처리가 주요 관심사인 경우에 사용된다. 
    // 이벤트 방식은 이벤트의 발생 여부를 감시하는 특정 쓰레드를 사용하여 메인 프로세스와 별개로 동작되므로
    // MultiProcessor 시스템등에서 자원을 가장 효율적으로 사용할 수 있게 되어 특히 권장하는 방식이다.
    // 인터럽트 메시지를 받아오기 위하여 윈도우 메시지 또는 콜백 함수를 사용한다.
    // (메시지 핸들, 메시지 ID, 콜백함수, 인터럽트 이벤트)
    //    hWnd    : 윈도우 핸들, 윈도우 메세지를 받을때 사용. 사용하지 않으면 NULL을 입력.
    //    uMessage: 윈도우 핸들의 메세지, 사용하지 않거나 디폴트값을 사용하려면 0을 입력.
    //    proc    : 인터럽트 발생시 호출될 함수의 포인터, 사용하지 않으면 NULL을 입력.
    //    pEvent  : 이벤트 방법사용시 이벤트 핸들
    // Ex)
    // AxdiInterruptSetModule(0, Null, 0, AxtInterruptProc, NULL);
    // void __stdcall AxtInterruptProc(long lActiveNo, DWORD uFlag){
    //     ... ;
    // }
    DWORD    __stdcall AxdiInterruptSetModule(long lModuleNo, HWND hWnd, DWORD uMessage, AXT_INTERRUPT_PROC pProc, HANDLE *pEvent);
    
    // 지정한 모듈의 인터럽트 사용 유무 설정
    //======================================================//
    // uUse    : DISABLE(0)    // 인터럽트 해제
    //         : ENABLE(1)     // 인터럽트 설정
    //======================================================//
    DWORD    __stdcall AxdiInterruptSetModuleEnable(long lModuleNo, DWORD uUse);
    
    // 지정한 모듈의 인터럽트 사용 유무 확인
    //======================================================//
    // *upUse  : DISABLE(0)    // 인터럽트 해제
    //         : ENABLE(1)     // 인터럽트 설정
    //======================================================//
    DWORD    __stdcall AxdiInterruptGetModuleEnable(long lModuleNo, DWORD *upUse);
    
    // 이벤트 방식 인터럽트 사용시 인터럽트 발생 위치 확인
    DWORD    __stdcall AxdiInterruptRead(long *lpModuleNo, DWORD *upFlag);

//========== 인터럽트 상승 / 하강 에지 설정 확인 
    // 지정한 입력 접점 모듈, Interrupt Rising / Falling Edge register의 Offset 위치에서 bit 단위로 상승 또는 하강 에지 값을 설정
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // uMode       : DOWN_EDGE(0)
    //             : UP_EDGE(1)
    // uValue      : DISABLE(0)
    //             : ENABLE(1)
    //===============================================================================================//
    DWORD    __stdcall AxdiInterruptEdgeSetBit(long lModuleNo, long lOffset, DWORD uMode, DWORD uValue);
    
    // 지정한 입력 접점 모듈, Interrupt Rising / Falling Edge register의 Offset 위치에서 byte 단위로 상승 또는 하강 에지 값을 설정
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // uMode       : DOWN_EDGE(0)
    //             : UP_EDGE(1)
    // uValue      : 0x00 ~ 0x0FF ('1'로 Setting 된 부분 인터럽트 설정)
    //===============================================================================================//
    DWORD    __stdcall AxdiInterruptEdgeSetByte(long lModuleNo, long lOffset, DWORD uMode, DWORD uValue);
    
    // 지정한 입력 접점 모듈, Interrupt Rising / Falling Edge register의 Offset 위치에서 word 단위로 상승 또는 하강 에지 값을 설정
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // uMode       : DOWN_EDGE(0)
    //             : UP_EDGE(1)
    // uValue      : 0x00 ~ 0x0FFFF ('1'로 Setting 된 부분 인터럽트 설정)
    //===============================================================================================//
    DWORD    __stdcall AxdiInterruptEdgeSetWord(long lModuleNo, long lOffset, DWORD uMode, DWORD uValue);
    
    // 지정한 입력 접점 모듈, Interrupt Rising / Falling Edge register의 Offset 위치에서 double word 단위로 상승 또는 하강 에지 값을 설정
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // uMode       : DOWN_EDGE(0)
    //             : UP_EDGE(1)
    // uValue      : 0x00 ~ 0x0FFFFFFFF ('1'로 Setting 된 부분 인터럽트 설정)
    //===============================================================================================//
    DWORD    __stdcall AxdiInterruptEdgeSetDword(long lModuleNo, long lOffset, DWORD uMode, DWORD uValue);

    // 지정한 입력 접점 모듈, Interrupt Rising / Falling Edge register의 Offset 위치에서 bit 단위로 상승 또는 하강 에지 값을 확인
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // uMode       : DOWN_EDGE(0)
    //             : UP_EDGE(1)
    // *upValue    : 0x00 ~ 0x0FF ('1'로 Setting 된 부분 인터럽트 설정)
    //===============================================================================================//
    DWORD    __stdcall AxdiInterruptEdgeGetBit(long lModuleNo, long lOffset, DWORD uMode, DWORD *upValue);
    
    // 지정한 입력 접점 모듈, Interrupt Rising / Falling Edge register의 Offset 위치에서 byte 단위로 상승 또는 하강 에지 값을 확인
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // uMode       : DOWN_EDGE(0)
    //             : UP_EDGE(1)
    // *upValue    : 0x00 ~ 0x0FF ('1'로 Setting 된 부분 인터럽트 설정)
    //===============================================================================================//
    DWORD    __stdcall AxdiInterruptEdgeGetByte(long lModuleNo, long lOffset, DWORD uMode, DWORD *upValue);
    
    // 지정한 입력 접점 모듈, Interrupt Rising / Falling Edge register의 Offset 위치에서 word 단위로 상승 또는 하강 에지 값을 확인
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // uMode       : DOWN_EDGE(0)
    //             : UP_EDGE(1)
    // *upValue    : 0x00 ~ 0x0FFFFFFFF ('1'로 Setting 된 부분 인터럽트 설정)
    //===============================================================================================//
    DWORD    __stdcall AxdiInterruptEdgeGetWord(long lModuleNo, long lOffset, DWORD uMode, DWORD *upValue);
    
    // 지정한 입력 접점 모듈, Interrupt Rising / Falling Edge register의 Offset 위치에서 double word 단위로 상승 또는 하강 에지 값을 확인
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // uMode       : DOWN_EDGE(0)
    //             : UP_EDGE(1)
    // *upValue    : 0x00 ~ 0x0FFFFFFFF ('1'로 Setting 된 부분 인터럽트 설정)
    //===============================================================================================//
    DWORD    __stdcall AxdiInterruptEdgeGetDword(long lModuleNo, long lOffset, DWORD uMode, DWORD *upValue);
    
    // 전체 입력 접점 모듈, Interrupt Rising / Falling Edge register의 Offset 위치에서 bit 단위로 상승 또는 하강 에지 값을 설정
    //===============================================================================================//
    // lOffset     : 입력 접점에 대한 Offset 위치
    // uMode       : DOWN_EDGE(0)
    //             : UP_EDGE(1)
    // uValue      : DISABLE(0)
    //             : ENABLE(1)
    //===============================================================================================//
    DWORD    __stdcall AxdiInterruptEdgeSet(long lOffset, DWORD uMode, DWORD uValue);
    
    // 전체 입력 접점 모듈, Interrupt Rising / Falling Edge register의 Offset 위정에서 bit 단위로 상승 또는 하강 에지 값을 확인
    //===============================================================================================//
    // lOffset     : 입력 접점에 대한 Offset 위치
    // uMode       : DOWN_EDGE(0)
    //             : UP_EDGE(1)
    // *upValue    : DISABLE(0)
    //             : ENABLE(1)
    //===============================================================================================//
    DWORD    __stdcall AxdiInterruptEdgeGet(long lOffset, DWORD uMode, DWORD *upValue);

//========== 입출력 레벨 설정 확인 
//==입력 레벨 설정 인
    // 지정한 입력 접점 모듈의 Offset 위치에서 bit 단위로 데이터 레벨을 설정
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // uLevel      : LOW(0)
    //             : HIGH(1)
    //===============================================================================================//
    DWORD    __stdcall AxdiLevelSetInportBit(long lModuleNo, long lOffset, DWORD uLevel);

    // 지정한 입력 접점 모듈의 Offset 위치에서 byte 단위로 데이터 레벨을 설정
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // uLevel      : 0x00 ~ 0x0FF('1'로 설정 된 비트는 HIGH, '0'으로 설정 된 비트는 LOW)
    //===============================================================================================//
    DWORD    __stdcall AxdiLevelSetInportByte(long lModuleNo, long lOffset, DWORD uLevel);
    
    // 지정한 입력 접점 모듈의 Offset 위치에서 word 단위로 데이터 레벨을 설정
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // uLevel      : 0x00 ~ 0x0FFFF('1'로 설정 된 비트는 HIGH, '0'으로 설정 된 비트는 LOW)
    //===============================================================================================//
    DWORD    __stdcall AxdiLevelSetInportWord(long lModuleNo, long lOffset, DWORD uLevel);
    
    // 지정한 입력 접점 모듈의 Offset 위치에서 double word 단위로 데이터 레벨을 설정
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // uLevel      : 0x00 ~ 0x0FFFFFFFF('1'로 설정 된 비트는 HIGH, '0'으로 설정 된 비트는 LOW)
    //===============================================================================================//
    DWORD    __stdcall AxdiLevelSetInportDword(long lModuleNo, long lOffset, DWORD uLevel);
    
    // 지정한 입력 접점 모듈의 Offset 위치에서 bit 단위로 데이터 레벨을 확인
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // *upLevel    : LOW(0)
    //             : HIGH(1)
    //===============================================================================================//
    DWORD    __stdcall AxdiLevelGetInportBit(long lModuleNo, long lOffset, DWORD *upLevel);

    // 지정한 입력 접점 모듈의 Offset 위치에서 byte 단위로 데이터 레벨을 확인
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // *upLevel    : 0x00 ~ 0x0FF('1'로 읽힌 비트는 HIGH, '0'으로 읽힌 비트는 LOW)
    //===============================================================================================//
    DWORD    __stdcall AxdiLevelGetInportByte(long lModuleNo, long lOffset, DWORD *upLevel);
    
    // 지정한 입력 접점 모듈의 Offset 위치에서 word 단위로 데이터 레벨을 확인
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // *upLevel    : 0x00 ~ 0x0FFFF('1'로 읽힌 비트는 HIGH, '0'으로 읽힌 비트는 LOW)
    //===============================================================================================//
    DWORD    __stdcall AxdiLevelGetInportWord(long lModuleNo, long lOffset, DWORD *upLevel);
    
    // 지정한 입력 접점 모듈의 Offset 위치에서 double word 단위로 데이터 레벨을 확인
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // *upLevel    : 0x00 ~ 0x0FFFFFFFF('1'로 읽힌 비트는 HIGH, '0'으로 읽힌 비트는 LOW)
    //===============================================================================================//
    DWORD    __stdcall AxdiLevelGetInportDword(long lModuleNo, long lOffset, DWORD *upLevel);
    
    // 전체 입력 접점 모듈의 Offset 위치에서 bit 단위로 데이터 레벨을 설정
    //===============================================================================================//
    // lOffset     : 입력 접점에 대한 Offset 위치
    // uLevel      : LOW(0)
    //             : HIGH(1)
    //===============================================================================================//
    DWORD    __stdcall AxdiLevelSetInport(long lOffset, DWORD uLevel);
    
    // 전체 입력 접점 모듈의 Offset 위치에서 bit 단위로 데이터 레벨을 확인
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // *upLevel    : LOW(0)
    //             : HIGH(1)
    //===============================================================================================//
    DWORD    __stdcall AxdiLevelGetInport(long lOffset, DWORD *upLevel);

//==출력 레벨 설정 확인
    // 지정한 출력 접점 모듈의 Offset 위치에서 bit 단위로 데이터 레벨을 설정
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치
    // uLevel      : LOW(0)
    //             : HIGH(1)
    //===============================================================================================//
    DWORD    __stdcall AxdoLevelSetOutportBit(long lModuleNo, long lOffset, DWORD uLevel);

    // 지정한 출력 접점 모듈의 Offset 위치에서 byte 단위로 데이터 레벨을 설정
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치
    // uLevel      : 0x00 ~ 0x0FF('1'로 설정 된 비트는 HIGH, '0'으로 설정 된 비트는 LOW)
    //===============================================================================================//
    DWORD    __stdcall AxdoLevelSetOutportByte(long lModuleNo, long lOffset, DWORD uLevel);
    
    // 지정한 출력 접점 모듈의 Offset 위치에서 word 단위로 데이터 레벨을 설정
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치
    // uLevel      : 0x00 ~ 0x0FFFF('1'로 설정 된 비트는 HIGH, '0'으로 설정 된 비트는 LOW)
    //===============================================================================================//
    DWORD    __stdcall AxdoLevelSetOutportWord(long lModuleNo, long lOffset, DWORD uLevel);
    
    // 지정한 출력 접점 모듈의 Offset 위치에서 double word 단위로 데이터 레벨을 설정
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치
    // uLevel      : 0x00 ~ 0x0FFFFFFFF('1'로 설정 된 비트는 HIGH, '0'으로 설정 된 비트는 LOW)
    //===============================================================================================//
    DWORD    __stdcall AxdoLevelSetOutportDword(long lModuleNo, long lOffset, DWORD uLevel);
    
    // 지정한 출력 접점 모듈의 Offset 위치에서 bit 단위로 데이터 레벨을 확인
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치
    // *upLevel    : LOW(0)
    //             : HIGH(1)
    //===============================================================================================//
    DWORD    __stdcall AxdoLevelGetOutportBit(long lModuleNo, long lOffset, DWORD *upLevel);

    // 지정한 출력 접점 모듈의 Offset 위치에서 byte 단위로 데이터 레벨을 확인
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치
    // uLevel      : 0x00 ~ 0x0FF('1'로 읽힌 비트는 HIGH, '0'으로 읽힌 비트는 LOW)
    //===============================================================================================//
    DWORD    __stdcall AxdoLevelGetOutportByte(long lModuleNo, long lOffset, DWORD *upLevel);
    
    // 지정한 출력 접점 모듈의 Offset 위치에서 word 단위로 데이터 레벨을 확인
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치
    // uLevel      : 0x00 ~ 0x0FFFF('1'로 읽힌 비트는 HIGH, '0'으로 읽힌 비트는 LOW)
    //===============================================================================================//
    DWORD    __stdcall AxdoLevelGetOutportWord(long lModuleNo, long lOffset, DWORD *upLevel);
    
    // 지정한 출력 접점 모듈의 Offset 위치에서 double word 단위로 데이터 레벨을 확인
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치
    // uLevel      : 0x00 ~ 0x0FFFFFFFF('1'로 읽힌 비트는 HIGH, '0'으로 읽힌 비트는 LOW)
    //===============================================================================================//
    DWORD    __stdcall AxdoLevelGetOutportDword(long lModuleNo, long lOffset, DWORD *upLevel);
    
    // 전체 출력 접점 모듈의 Offset 위치에서 bit 단위로 데이터 레벨을 설정
    //===============================================================================================//
    // lOffset     : 출력 접점에 대한 Offset 위치
    // uLevel      : LOW(0)
    //             : HIGH(1)
    //===============================================================================================//
    DWORD    __stdcall AxdoLevelSetOutport(long lOffset, DWORD uLevel);
    
    // 전체 출력 접점 모듈의 Offset 위치에서 bit 단위로 데이터 레벨을 확인
    //===============================================================================================//
    // lOffset     : 출력 접점에 대한 Offset 위치
    // *upLevel    : LOW(0)
    //             : HIGH(1)
    //===============================================================================================//
    DWORD    __stdcall AxdoLevelGetOutport(long lOffset, DWORD *upLevel);
    
//========== 입출력 포트 쓰기 읽기 
//==출력 포트 쓰기
    // 전체 출력 접점 모듈의 Offset 위치에서 bit 단위로 데이터를 출력
    //===============================================================================================//
    // lOffset     : 출력 접점에 대한 Offset 위치
    // uLevel      : LOW(0)
    //             : HIGH(1)
    //===============================================================================================//
    DWORD    __stdcall AxdoWriteOutport(long lOffset, DWORD uValue);
    
    // 지정한 출력 접점 모듈의 Offset 위치에서 bit 단위로 데이터를 출력
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치
    // uLevel      : LOW(0)
    //             : HIGH(1)
    //===============================================================================================//
    DWORD    __stdcall AxdoWriteOutportBit(long lModuleNo, long lOffset, DWORD uValue);

    // 지정한 출력 접점 모듈의 Offset 위치에서 byte 단위로 데이터를 출력
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치
    // uValue      : 0x00 ~ 0x0FF('1'로 설정 된 비트는 HIGH, '0'으로 설정 된 비트는 LOW)
    //===============================================================================================//
    DWORD    __stdcall AxdoWriteOutportByte(long lModuleNo, long lOffset, DWORD uValue);
    
    // 지정한 출력 접점 모듈의 Offset 위치에서 word 단위로 데이터를 출력
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치
    // uValue      : 0x00 ~ 0x0FFFF('1'로 설정 된 비트는 HIGH, '0'으로 설정 된 비트는 LOW)
    //===============================================================================================//
    DWORD    __stdcall AxdoWriteOutportWord(long lModuleNo, long lOffset, DWORD uValue);
    
    // 지정한 출력 접점 모듈의 Offset 위치에서 double word 단위로 데이터를 출력
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치
    // uValue      : 0x00 ~ 0x0FFFFFFFF('1'로 설정 된 비트는 HIGH, '0'으로 설정 된 비트는 LOW)
    //===============================================================================================//
    DWORD    __stdcall AxdoWriteOutportDword(long lModuleNo, long lOffset, DWORD uValue);
    
//==출력 포트 읽기
    // 전체 출력 접점 모듈의 Offset 위치에서 bit 단위로 데이터를 읽기
    //===============================================================================================//
    // lOffset     : 출력 접점에 대한 Offset 위치
    // *upLevel    : LOW(0)
    //             : HIGH(1)
    //===============================================================================================//
    DWORD    __stdcall AxdoReadOutport(long lOffset, DWORD *upValue);
    
    // 지정한 출력 접점 모듈의 Offset 위치에서 bit 단위로 데이터를 읽기
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치
    // *upLevel    : LOW(0)
    //             : HIGH(1)
    //===============================================================================================//
    DWORD    __stdcall AxdoReadOutportBit(long lModuleNo, long lOffset, DWORD *upValue);

    // 지정한 출력 접점 모듈의 Offset 위치에서 byte 단위로 데이터를 읽기
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치
    // *upValue    : 0x00 ~ 0x0FF('1'로 읽힌 비트는 HIGH, '0'으로 읽힌 비트는 LOW)
    //===============================================================================================//
    DWORD    __stdcall AxdoReadOutportByte(long lModuleNo, long lOffset, DWORD *upValue);
    
    // 지정한 출력 접점 모듈의 Offset 위치에서 word 단위로 데이터를 읽기
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치
    // *upValue    : 0x00 ~ 0x0FFFF('1'로 읽힌 비트는 HIGH, '0'으로 읽힌 비트는 LOW)
    //===============================================================================================//
    DWORD    __stdcall AxdoReadOutportWord(long lModuleNo, long lOffset, DWORD *upValue);
    
    // 지정한 출력 접점 모듈의 Offset 위치에서 double word 단위로 데이터를 읽기
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치
    // *upValue    : 0x00 ~ 0x0FFFFFFFF('1'로 읽힌 비트는 HIGH, '0'으로 읽힌 비트는 LOW)
    //===============================================================================================//
    DWORD    __stdcall AxdoReadOutportDword(long lModuleNo, long lOffset, DWORD *upValue);
    
//==입력 포트 읽기
    // 전체 입력 접점 모듈의 Offset 위치에서 bit 단위로 데이터를 읽기
    //===============================================================================================//
    // lOffset     : 입력 접점에 대한 Offset 위치
    // *upValue    : LOW(0)
    //             : HIGH(1)
    //===============================================================================================//
    DWORD    __stdcall AxdiReadInport(long lOffset, DWORD *upValue);
    
    // 지정한 입력 접점 모듈의 Offset 위치에서 bit 단위로 데이터를 읽기
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // *upValue    : LOW(0)
    //             : HIGH(1)
    //===============================================================================================//
    DWORD    __stdcall AxdiReadInportBit(long lModuleNo, long lOffset, DWORD *upValue);

    // 지정한 입력 접점 모듈의 Offset 위치에서 byte 단위로 데이터를 읽기
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // *upValue    : 0x00 ~ 0x0FF('1'로 읽힌 비트는 HIGH, '0'으로 읽힌 비트는 LOW)
    //===============================================================================================//
    DWORD    __stdcall AxdiReadInportByte(long lModuleNo, long lOffset, DWORD *upValue);
    
    // 지정한 입력 접점 모듈의 Offset 위치에서 word 단위로 데이터를 읽기
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // *upValue    : 0x00 ~ 0x0FFFF('1'로 읽힌 비트는 HIGH, '0'으로 읽힌 비트는 LOW)
    //===============================================================================================//
    DWORD    __stdcall AxdiReadInportWord(long lModuleNo, long lOffset, DWORD *upValue);
    
    // 지정한 입력 접점 모듈의 Offset 위치에서 double word 단위로 데이터를 읽기
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // *upValue    : 0x00 ~ 0x0FFFFFFFF('1'로 읽힌 비트는 HIGH, '0'으로 읽힌 비트는 LOW)
    //===============================================================================================//
    DWORD    __stdcall AxdiReadInportDword(long lModuleNo, long lOffset, DWORD *upValue);

    
//== MLII 용 M-Systems DIO(R7 series) 전용 함수.
    // 지정한 모듈에 장착된 입력 접점용 확장 기능 모듈의 Offset 위치에서 bit 단위로 데이터를 읽기
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치(0~15)
    // *upValue    : LOW(0)
    //             : HIGH(1)
    //===============================================================================================//
    DWORD   __stdcall AxdReadExtInportBit(long lModuleNo, long lOffset, DWORD *upValue);

    // 지정한 모듈에 장착된 입력 접점용 확장 기능 모듈의 Offset 위치에서 byte 단위로 데이터를 읽기
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치(0~1)
    // *upValue    : 0x00 ~ 0x0FF('1'로 읽힌 비트는 HIGH, '0'으로 읽힌 비트는 LOW)
    //===============================================================================================//
    DWORD   __stdcall AxdReadExtInportByte(long lModuleNo, long lOffset, DWORD *upValue);

    // 지정한 모듈에 장착된 입력 접점용 확장 기능 모듈의 Offset 위치에서 word 단위로 데이터를 읽기
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치(0)
    // *upValue    : 0x00 ~ 0x0FFFF('1'로 읽힌 비트는 HIGH, '0'으로 읽힌 비트는 LOW)
    //===============================================================================================//
    DWORD   __stdcall AxdReadExtInportWord(long lModuleNo, long lOffset, DWORD *upValue);

    // 지정한 모듈에 장착된 입력 접점용 확장 기능 모듈의 Offset 위치에서 dword 단위로 데이터를 읽기
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치(0)
    // *upValue    : 0x00 ~ 0x00000FFFF('1'로 읽힌 비트는 HIGH, '0'으로 읽힌 비트는 LOW)
    //===============================================================================================//
    DWORD   __stdcall AxdReadExtInportDword(long lModuleNo, long lOffset, DWORD *upValue);

    // 지정한 모듈에 장착된 출력 접점용 확장 기능 모듈의 Offset 위치에서 bit 단위로 데이터를 읽기
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치(0~15)
    // *upValue    : LOW(0)
    //             : HIGH(1)
    //===============================================================================================//
    DWORD   __stdcall AxdReadExtOutportBit(long lModuleNo, long lOffset, DWORD *upValue);

    // 지정한 모듈에 장착된 출력 접점용 확장 기능 모듈의 Offset 위치에서 byte 단위로 데이터를 읽기
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치(0~1)
    // *upValue    : 0x00 ~ 0x0FF('1'로 읽힌 비트는 HIGH, '0'으로 읽힌 비트는 LOW)
    //===============================================================================================//
    DWORD   __stdcall AxdReadExtOutportByte(long lModuleNo, long lOffset, DWORD *upValue);

    // 지정한 모듈에 장착된 출력 접점용 확장 기능 모듈의 Offset 위치에서 word 단위로 데이터를 읽기
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치(0)
    // *upValue    : 0x00 ~ 0x0FFFF('1'로 읽힌 비트는 HIGH, '0'으로 읽힌 비트는 LOW)
    //===============================================================================================//
    DWORD   __stdcall AxdReadExtOutportWord(long lModuleNo, long lOffset, DWORD *upValue);

    // 지정한 모듈에 장착된 출력 접점용 확장 기능 모듈의 Offset 위치에서 dword 단위로 데이터를 읽기
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치(0)
    // *upValue    : 0x00 ~ 0x00000FFFF('1'로 읽힌 비트는 HIGH, '0'으로 읽힌 비트는 LOW)
    //===============================================================================================//
    DWORD   __stdcall AxdReadExtOutportDword(long lModuleNo, long lOffset, DWORD *upValue);

    // 지정한 모듈에 장착된 출력 접점용 확장 기능 모듈의 Offset 위치에서 bit 단위로 데이터 출력
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치
    // uValue      : LOW(0)
    //             : HIGH(1)
    //===============================================================================================//
    DWORD   __stdcall AxdWriteExtOutportBit(long lModuleNo, long lOffset, DWORD uValue);

    // 지정한 모듈에 장착된 출력 접점용 확장 기능 모듈의 Offset 위치에서 byte 단위로 데이터 출력
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치(0~1)
    // uValue      : 0x00 ~ 0x0FF('1'로 읽힌 비트는 HIGH, '0'으로 읽힌 비트는 LOW)
    //===============================================================================================//
    DWORD   __stdcall AxdWriteExtOutportByte(long lModuleNo, long lOffset, DWORD uValue);

    // 지정한 모듈에 장착된 출력 접점용 확장 기능 모듈의 Offset 위치에서 word 단위로 데이터 출력
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치(0)
    // uValue    : 0x00 ~ 0x0FFFF('1'로 읽힌 비트는 HIGH, '0'으로 읽힌 비트는 LOW)
    //===============================================================================================//
    DWORD   __stdcall AxdWriteExtOutportWord(long lModuleNo, long lOffset, DWORD uValue);

    // 지정한 모듈에 장착된 출력 접점용 확장 기능 모듈의 Offset 위치에서 dword 단위로 데이터 출력
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치(0)
    // uValue    : 0x00 ~ 0x00000FFFF('1'로 읽힌 비트는 HIGH, '0'으로 읽힌 비트는 LOW)
    //===============================================================================================//
    DWORD   __stdcall AxdWriteExtOutportDword(long lModuleNo, long lOffset, DWORD uValue);

    // 지정한 모듈에 장착된 입/출력 접점용 확장 기능 모듈의 Offset 위치에서 bit 단위로 데이터 레벨을 설정
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치(0~15)
    // uLevel      : LOW(0)
    //             : HIGH(1)
    //===============================================================================================//
    DWORD   __stdcall AxdLevelSetExtportBit(long lModuleNo, long lOffset, DWORD uLevel);

    // 지정한 모듈에 장착된 입/출력 접점용 확장 기능 모듈의 Offset 위치에서 byte 단위로 데이터 레벨을 설정
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치(0~1)
    // uLevel      : 0x00 ~ 0xFF('1'로 설정 된 비트는 HIGH, '0'으로 설정 된 비트는 LOW)
    //===============================================================================================//
    DWORD   __stdcall AxdLevelSetExtportByte(long lModuleNo, long lOffset, DWORD uLevel);

    // 지정한 모듈에 장착된 입/출력 접점용 확장 기능 모듈의 Offset 위치에서 word 단위로 데이터 레벨을 설정
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치(0)
    // uLevel      : 0x00 ~ 0xFFFF('1'로 설정 된 비트는 HIGH, '0'으로 설정 된 비트는 LOW)
    //===============================================================================================//
    DWORD   __stdcall AxdLevelSetExtportWord(long lModuleNo, long lOffset, DWORD uLevel);

    // 지정한 모듈에 장착된 입/출력 접점용 확장 기능 모듈의 Offset 위치에서 dword 단위로 데이터 레벨을 설정
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치(0)
    // uLevel      : 0x00 ~ 0x0000FFFF('1'로 설정 된 비트는 HIGH, '0'으로 설정 된 비트는 LOW)
    //===============================================================================================//
    DWORD   __stdcall AxdLevelSetExtportDword(long lModuleNo, long lOffset, DWORD uLevel);

    // 지정한 모듈에 장착된 입/출력 접점용 확장 기능 모듈의 Offset 위치에서 bit 단위로 데이터 레벨 확인
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치(0~15)
    // *upLevel      : LOW(0)
    //             : HIGH(1)
    //===============================================================================================//
    DWORD   __stdcall AxdLevelGetExtportBit(long lModuleNo, long lOffset, DWORD *upLevel);

    // 지정한 모듈에 장착된 입/출력 접점용 확장 기능 모듈의 Offset 위치에서 byte 단위로 데이터 레벨 확인
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치(0~1)
    // *upLevel      : 0x00 ~ 0xFF('1'로 설정 된 비트는 HIGH, '0'으로 설정 된 비트는 LOW)
    //===============================================================================================//
    DWORD   __stdcall AxdLevelGetExtportByte(long lModuleNo, long lOffset, DWORD *upLevel);

    // 지정한 모듈에 장착된 입/출력 접점용 확장 기능 모듈의 Offset 위치에서 word 단위로 데이터 레벨 확인
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치(0)
    // *upLevel      : 0x00 ~ 0xFFFF('1'로 설정 된 비트는 HIGH, '0'으로 설정 된 비트는 LOW)
    //===============================================================================================//
    DWORD   __stdcall AxdLevelGetExtportWord(long lModuleNo, long lOffset, DWORD *upLevel);

    // 지정한 모듈에 장착된 입/출력 접점용 확장 기능 모듈의 Offset 위치에서 dword 단위로 데이터 레벨 확인
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치(0)
    // *upLevel      : 0x00 ~ 0x0000FFFF('1'로 설정 된 비트는 HIGH, '0'으로 설정 된 비트는 LOW)
    //===============================================================================================//
    DWORD   __stdcall AxdLevelGetExtportDword(long lModuleNo, long lOffset, DWORD *upLevel);
    
//========== 고급 함수 
    // 지정한 입력 접점 모듈의 Offset 위치에서 신호가 Off에서 On으로 바뀌었는지 확인
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // *upValue    : FALSE(0)
    //             : TRUE(1)
    //===============================================================================================//
    DWORD    __stdcall AxdiIsPulseOn(long lModuleNo, long lOffset, DWORD *upValue);

    // 지정한 입력 접점 모듈의 Offset 위치에서 신호가 On에서 Off으로 바뀌었는지 확인
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // *upValue    : FALSE(0)
    //             : TRUE(1)
    //===============================================================================================//
    DWORD    __stdcall AxdiIsPulseOff(long lModuleNo, long lOffset, DWORD *upValue);

    // 지정한 입력 접점 모듈의 Offset 위치에서 신호가 count 만큼 호출될 동안 On 상태로 유지하는지 확인
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 입력 접점에 대한 Offset 위치
    // lCount      : 0 ~ 0x7FFFFFFF(2147483647)
    // *upValue    : FALSE(0)
    //             : TRUE(1)
    // lStart      : 1(최초 호출)
    //             : 0(반복 호출)
    //===============================================================================================//
    DWORD    __stdcall AxdiIsOn(long lModuleNo, long lOffset, long lCount, DWORD *upValue, long lStart);
    
    // 지정한 입력 접점 모듈의 Offset 위치에서 신호가 count 만큼 호출될 동안 Off 상태로 유지하는지 확인
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치
    // lCount      : 0 ~ 0x7FFFFFFF(2147483647)
    // *upValue    : FALSE(0)
    //             : TRUE(1)
    // lStart      : 1(최초 호출)
    //             : 0(반복 호출)
    //===============================================================================================//
    DWORD    __stdcall AxdiIsOff(long lModuleNo, long lOffset, long lCount, DWORD *upValue, long lStart);
    
    // 지정한 출력 접점 모듈의 Offset 위치에서 설정한 mSec동안 On을 유지하다가 Off 시킴
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치
    // lCount      : 0 ~ 0x7FFFFFFF(2147483647)
    // lmSec       : 1 ~ 30000
    //===============================================================================================//
    DWORD    __stdcall AxdoOutPulseOn(long lModuleNo, long lOffset, long lmSec);

    // 지정한 출력 접점 모듈의 Offset 위치에서 설정한 mSec동안 Off를 유지하다가 On 시킴
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치
    // lCount      : 0 ~ 0x7FFFFFFF(2147483647)
    // lmSec       : 1 ~ 30000
    //===============================================================================================//
    DWORD    __stdcall AxdoOutPulseOff(long lModuleNo, long lOffset, long lmSec);

    // 지정한 출력 접점 모듈의 Offset 위치에서 설정한 횟수, 설정한 간격으로 토글한 후 원래의 출력상태를 유지함
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치
    // lInitState  : Off(0)
    //             : On(1)
    // lmSecOn     : 1 ~ 30000
    // lmSecOff    : 1 ~ 30000
    // lCount      : 1 ~ 0x7FFFFFFF(2147483647)
    //             : -1 무한 토글
    //===============================================================================================//
    DWORD    __stdcall AxdoToggleStart(long lModuleNo, long lOffset, long lInitState, long lmSecOn, long lmSecOff, long lCount);
    
    // 지정한 출력 접점 모듈의 Offset 위치에서 토글중인 출력을 설정한 신호 상태로 정지 시킴
    //===============================================================================================//
    // lModuleNo   : 모듈 번호
    // lOffset     : 출력 접점에 대한 Offset 위치
    // uOnOff      : Off(0)
    //             : On(1)
    //===============================================================================================//
    DWORD   __stdcall AxdoToggleStop(long lModuleNo, long lOffset, DWORD uOnOff);

    // 지정한 출력 모듈의 Network이 끊어 졌을 경우 출력 상태를 출력 Byte 단위로 설정한다.
    //===============================================================================================//
    // lModuleNo   : 모듈 번호(분산형 슬레이브 제품만 지원 함)
    // dwSize      : 설정 할 Byte 수(ex. RTEX-DB32 : 2, RTEX-DO32 : 4)
    // dwaSetValue : 설정 할 변수 값(Default는 Network 끊어 지기 전 상태 유지)
    //             : 0 --> Network 끊어 지기 전 상태 유지
    //             : 1 --> On
    //             : 2 --> Off
    //             : 3 --> User Value, (Default user value는 Off로 설정됨, AxdoSetNetworkErrorUserValue() 함수로 변경가능)
    //===============================================================================================//
    DWORD   __stdcall AxdoSetNetworkErrorAct(long lModuleNo, DWORD dwSize, DWORD* dwaSetValue);

	// 지정한 출력 모듈의 Network이 끊어 졌을 경우 출력 값을 사용자가 정의한 출력값을 Byte 단위로 설정한다.
	//===============================================================================================//
	// lModuleNo   : 모듈 번호(분산형 슬레이브 제품만 지원 함)
	// dwOffset    : 출력 접점에 대한 Offset 위치, BYTE 단위로 증가(지정범위:0, 1, 2, 3)
	// dwValue     : 출력 접점 값(00 ~ FFH)
	//             : AxdoSetNetworkErrorAct() 함수로 해당 Offset에 대해서 User Value 로 설정되어야 동작한다.
	//===============================================================================================//
	DWORD   __stdcall AxdoSetNetworkErrorUserValue(long lModuleNo, DWORD dwOffset, DWORD dwValue);

    // 지정한 모듈의 연결 Number를 설정한다.
    DWORD   __stdcall AxdSetContactNum(long lModuleNo, DWORD dwInputNum, DWORD dwOutputNum);

    // 지정한 모듈의 연결 Number를 확인한다.
    DWORD   __stdcall AxdGetContactNum(long lModuleNo, DWORD* dwpInputNum, DWORD* dwpOutputNum);
    
#ifdef __cplusplus
}
#endif    //__cplusplus

#endif    //__AXT_AXD_H__
