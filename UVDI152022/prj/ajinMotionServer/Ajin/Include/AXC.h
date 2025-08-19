/****************************************************************************
*****************************************************************************
**
** File Name
** ----------
**
** AXC.H
**
** COPYRIGHT (c) AJINEXTEK Co., LTD
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
** Ajinextek Counter Library Header File
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

#ifndef __AXT_AXC_H__
#define __AXT_AXC_H__

#include "AXHS.h"
#include "AXHD.h"

#ifdef __cplusplus
extern "C" {
#endif    //__cplusplus

//========== 보드 및 모듈 정보 
    // CNT 모듈이 있는지 확인
    DWORD __stdcall AxcInfoIsCNTModule(DWORD *upStatus);
    
    // CNT 모듈 No 확인
    DWORD __stdcall AxcInfoGetModuleNo(long lBoardNo, long lModulePos, long *lpModuleNo);
    
    // CNT 입출력 모듈의 개수 확인
    DWORD __stdcall AxcInfoGetModuleCount(long *lpModuleCount);

    // 지정한 모듈의 카운터 입력 채널 개수 확인
    DWORD __stdcall AxcInfoGetChannelCount(long lModuleNo, long *lpCount);
    
    // 시스템에 장착된 카운터의 전 채널 수를 확인
    DWORD __stdcall AxcInfoGetTotalChannelCount(long *lpChannelCount);    

    // 지정한 모듈 번호로 베이스 보드 번호, 모듈 위치, 모듈 ID 확인
    DWORD __stdcall AxcInfoGetModule(long lModuleNo, long *lpBoardNo, long *lpModulePos, DWORD *upModuleID);

    // 해당 모듈이 제어가 가능한 상태인지 반환한다.
    DWORD __stdcall AxcInfoGetModuleStatus(long lModuleNo);

    DWORD __stdcall AxcInfoGetFirstChannelNoOfModuleNo(long lModuleNo, long *lpChannelNo);
	DWORD __stdcall AxcInfoGetModuleNoOfChannelNo(long lChannelNo, long *lpModuleNo);

    // 카운터 모듈의 Encoder 입력 방식을 설정 합니다.
    // dwMethod --> 0x00 : Sign and pulse, x1 multiplication
    // dwMethod --> 0x01 : Phase-A and phase-B pulses, x1 multiplication
    // dwMethod --> 0x02 : Phase-A and phase-B pulses, x2 multiplication
    // dwMethod --> 0x03 : Phase-A and phase-B pulses, x4 multiplication
    // dwMethod --> 0x08 : Sign and pulse, x2 multiplication
    // dwMethod --> 0x09 : Increment and decrement pulses, x1 multiplication
    // dwMethod --> 0x0A : Increment and decrement pulses, x2 multiplication
    // SIO-CN2CH/HPC4의 경우
    // dwMethod --> 0x00 : Up/Down 방식, A phase : 펄스, B phase : 방향
    // dwMethod --> 0x01 : Phase-A and phase-B pulses, x1 multiplication
    // dwMethod --> 0x02 : Phase-A and phase-B pulses, x2 multiplication
    // dwMethod --> 0x03 : Phase-A and phase-B pulses, x4 multiplication
    // SIO-LCM4의 경우
    // dwMethod --> 0x01 : Phase-A and phase-B pulses, x1 multiplication
    // dwMethod --> 0x02 : Phase-A and phase-B pulses, x2 multiplication
    // dwMethod --> 0x03 : Phase-A and phase-B pulses, x4 multiplication    
    DWORD __stdcall AxcSignalSetEncInputMethod(long lChannelNo, DWORD dwMethod);

    // 카운터 모듈의 Encoder 입력 방식을 설정 합니다.
    // *dwpUpMethod --> 0x00 : Sign and pulse, x1 multiplication
    // *dwpUpMethod --> 0x01 : Phase-A and phase-B pulses, x1 multiplication
    // *dwpUpMethod --> 0x02 : Phase-A and phase-B pulses, x2 multiplication
    // *dwpUpMethod --> 0x03 : Phase-A and phase-B pulses, x4 multiplication
    // *dwpUpMethod --> 0x08 : Sign and pulse, x2 multiplication
    // *dwpUpMethod --> 0x09 : Increment and decrement pulses, x1 multiplication
    // *dwpUpMethod --> 0x0A : Increment and decrement pulses, x2 multiplication
    // SIO-CN2CH/HPC4의 경우
    // dwMethod --> 0x00 : Up/Down 방식, A phase : 펄스, B phase : 방향
    // dwMethod --> 0x01 : Phase-A and phase-B pulses, x1 multiplication
    // dwMethod --> 0x02 : Phase-A and phase-B pulses, x2 multiplication
    // dwMethod --> 0x03 : Phase-A and phase-B pulses, x4 multiplication
    // SIO-LCM4의 경우
    // dwMethod --> 0x01 : Phase-A and phase-B pulses, x1 multiplication
    // dwMethod --> 0x02 : Phase-A and phase-B pulses, x2 multiplication
    // dwMethod --> 0x03 : Phase-A and phase-B pulses, x4 multiplication       
    DWORD __stdcall AxcSignalGetEncInputMethod(long lChannelNo, DWORD* dwpUpMethod);

    // 카운터 모듈의 트리거를 설정 합니다.
    // dwMode -->  0x00 : Latch
    // dwMode -->  0x01 : State
    // dwMode -->  0x02 : Special State    --> SIO-CN2CH 전용
    // SIO-CN2CH의 경우
    // dwMode -->  0x00 : 절대 위치 트리거 또는 주기 위치 트리거.
    // 주의 : 제품마다 기능이 서로 다르기 때문에 구별하여 사용 필요.
    // dwMode -->  0x01 : 시간 주기 트리거(AxcTriggerSetFreq로 설정)
    // SIO-HPC4의 경우
    // dwMode -->  0x00 : timer mode with counter & frequncy.
    // dwMode -->  0x01 : timer mode.
    // dwMode -->  0x02 : absolute mode[with fifo].
    // dwMode -->  0x03 : periodic mode.[Default]
    DWORD __stdcall AxcTriggerSetFunction(long lChannelNo, DWORD dwMode);

    // 카운터 모듈의 트리거 설정을 확인 합니다.
    // *dwMode -->  0x00 : Latch
    // *dwMode -->  0x01 : State
    // *dwMode -->  0x02 : Special State
    // SIO-CN2CH의 경우
    // *dwMode -->  0x00 : 절대 위치 트리거 또는 주기 위치 트리거.
    // 주의 : 제품마다 기능이 서로 다르기 때문에 구별하여 사용 필요.
    // *dwMode -->  0x01 : 시간 주기 트리거(AxcTriggerSetFreq로 설정)
    // SIO-HPC4의 경우
    // *dwMode -->  0x00 : timer mode with counter & frequncy.
    // *dwMode -->  0x01 : timer mode.
    // *dwMode -->  0x02 : absolute mode[with fifo].
    // *dwMode -->  0x03 : periodic mode.[Default]
    DWORD __stdcall AxcTriggerGetFunction(long lChannelNo, DWORD* dwpMode);

    // dwUsage --> 0x00 : Trigger Not use
    // dwUsage --> 0x01 : Trigger use
    DWORD __stdcall AxcTriggerSetNotchEnable(long lChannelNo, DWORD dwUsage);

    // *dwUsage --> 0x00 : Trigger Not use
    // *dwUsage --> 0x01 : Trigger use
    DWORD __stdcall AxcTriggerGetNotchEnable(long lChannelNo, DWORD* dwpUsage);
    

    // 카운터 모듈의 Capture 극성을 설정 합니다.(External latch input polarity)
    // dwCapturePol --> 0x00 : Signal OFF -> ON
    // dwCapturePol --> 0x01 : Signal ON -> OFF
    DWORD __stdcall AxcSignalSetCaptureFunction(long lChannelNo, DWORD dwCapturePol);

    // 카운터 모듈의 Capture 극성 설정을 확인 합니다.(External latch input polarity)
    // *dwCapturePol --> 0x00 : Signal OFF -> ON
    // *dwCapturePol --> 0x01 : Signal ON -> OFF
    DWORD __stdcall AxcSignalGetCaptureFunction(long lChannelNo, DWORD* dwpCapturePol);

    // 카운터 모듈의 Capture 위치를 확인 합니다.(External latch)
    // *dbpCapturePos --> Capture position 위치
    DWORD __stdcall AxcSignalGetCapturePos(long lChannelNo, double* dbpCapturePos);

    // 카운터 모듈의 카운터 값을 확인 합니다.
    // *dbpActPos --> 카운터 값
    DWORD __stdcall AxcStatusGetActPos(long lChannelNo, double* dbpActPos);

    // 카운터 모듈의 카운터 값을 dbActPos 값으로 설정 합니다.
    DWORD __stdcall AxcStatusSetActPos(long lChannelNo, double dbActPos);

    // 카운터 모듈의 트리거 위치를 설정합니다.
    // 카운터 모듈의 트리거 위치는 2개까지만 설정 할 수 있습니다.
    DWORD __stdcall AxcTriggerSetNotchPos(long lChannelNo, double dbLowerPos, double dbUpperPos);

    // 카운터 모듈의 설정한 트리거 위치를 확인 합니다.
    DWORD __stdcall AxcTriggerGetNotchPos(long lChannelNo, double* dbpLowerPos, double* dbpUpperPos);

    // 카운터 모듈의 트리거 출력을 강제로 합니다.
    // dwOutVal --> 0x00 : 트리거 출력 '0'
    // dwOutVal --> 0x01 : 트리거 출력 '1'
    DWORD __stdcall AxcTriggerSetOutput(long lChannelNo, DWORD dwOutVal);

    // 카운터 모듈의 상태를 확인합니다.
    // Bit '0' : Carry (카운터 현재치가 덧셈 펄스에 의해 카운터 상한치를 넘어 0로 바뀌었을 때 1스캔만 ON으로 합니다.)
    // Bit '1' : Borrow (카운터 현재치가 뺄셈 펄스에 의해 0을 넘어 카운터 상한치로 바뀌었을 때 1스캔만 ON으로 합니다.)
    // Bit '2' : Trigger output status
    // Bit '3' : Latch input status
    DWORD __stdcall AxcStatusGetChannel(long lChannelNo, DWORD* dwpChannelStatus);


    // SIO-CN2CH 전용 함수군
    //
    // 카운터 모듈의 위치 단위를 설정한다.
    // 실제 위치 이동량에 대한 펄스 갯수를 설정하는데,
    // ex) 1mm 이동에 1000가 필요하다면dMoveUnitPerPulse = 0.001로 입력하고,
    //     이후 모든 함수의 위치와 관련된 값을 mm 단위로 설정하면 된다.
    DWORD __stdcall AxcMotSetMoveUnitPerPulse(long lChannelNo, double dMoveUnitPerPulse);

    // 카운터 모듈의 위치 단위를 확인한다.
    DWORD __stdcall AxcMotGetMoveUnitPerPulse(long lChannelNo, double* dpMoveUnitPerPuls);

    // 카운터 모듈의 엔코더 입력 카운터를 반전 기능을 설정한다.
    // dwReverse --> 0x00 : 반전하지 않음.
    // dwReverse --> 0x01 : 반전.
    DWORD __stdcall AxcSignalSetEncReverse(long lChannelNo, DWORD dwReverse);

    // 카운터 모듈의 엔코더 입력 카운터를 반전 기능을 설정을 확인한다.
    DWORD __stdcall AxcSignalGetEncReverse(long lChannelNo, DWORD* dwpReverse);

    // 카운터 모듈의 Encoder 입력 신호를 선택한다.
    // dwSource -->  0x00 : 2(A/B)-Phase 신호.
    // dwSource -->  0x01 : Z-Phase 신호.(방향성 없음.)
    DWORD  __stdcall AxcSignalSetEncSource(long lChannelNo, DWORD dwSource);

    // 카운터 모듈의 Encoder 입력 신호 선택 설정을 확인한다.
    DWORD  __stdcall AxcSignalGetEncSource(long lChannelNo, DWORD* dwpSource);

    // 카운터 모듈의 트리거 출력 범위 중 하한 값을 설정한다.
    // 위치 주기 트리거 제품의 경우 위치 주기로 트리거 출력을 발생시킬 범위 중 하한 값을 설정한다.
    // 절대 위치 트리거 제품의 경우 Ram 시작 번지의 트리거 정보의 적용 기준 위치를 설정한다.
    // 단위 : AxcMotSetMoveUnitPerPulse로 설정한 단위.
    // Note : 하한값은 상한값보다 작은값을 설정하여야 합니다.
    DWORD  __stdcall AxcTriggerSetBlockLowerPos(long lChannelNo, double dLowerPosition);

    // 카운터 모듈의 트리거 출력 범위 중 하한 값을 확인한다.
    DWORD  __stdcall AxcTriggerGetBlockLowerPos(long lChannelNo, double* dpLowerPosition);

    // 카운터 모듈의 트리거 출력 범위 중 상한 값을 설정한다.
    // 위치 주기 트리거 제품의 경우 위치 주기로 트리거 출력을 발생시킬 범위 중 상한 값을 설정한다.
    // 절대 위치 트리거 제품의 경우 트리거 정보가 설정된 Ram 의 마지막 번지의 트리거 정보가 적용되는 위치로 사용된다.
    // 단위 : AxcMotSetMoveUnitPerPulse로 설정한 단위.
    // Note : 상한값은 하한값보다 큰값을 설정하여야 합니다.
    DWORD  __stdcall AxcTriggerSetBlockUpperPos(long lChannelNo, double dUpperPosition);
    // 카운터 모듈의 트리거 출력 범위 중 하한 값을 설정한다.
    DWORD  __stdcall AxcTriggerGetBlockUpperPos(long lChannelNo, double* dpUpperrPosition);

    // 카운터 모듈의 위치 주기 모드 트리거에 사용되는 위치 주기를 설정한다.
    // 단위 : AxcMotSetMoveUnitPerPulse로 설정한 단위.
    DWORD  __stdcall AxcTriggerSetPosPeriod(long lChannelNo, double dPeriod);

    // 카운터 모듈의 위치 주기 모드 트리거에 사용되는 위치 주기를 확인한다.
    DWORD  __stdcall AxcTriggerGetPosPeriod(long lChannelNo, double* dpPeriod);

    // 카운터 모듈의 위치 주기 모드 트리거 사용시 위치 증감에 대한 유효기능을 설정한다.
    // dwDirection -->  0x00 : 카운터의 증/감에 대하여 트리거 주기 마다 출력.
    // dwDirection -->  0x01 : 카운터가 증가 할때만 트리거 주기 마다 출력.
    // dwDirection -->  0x01 : 카운터가 감소 할때만 트리거 주기 마다 출력.
    DWORD  __stdcall AxcTriggerSetDirectionCheck(long lChannelNo, DWORD dwDirection);
    // 카운터 모듈의 위치 주기 모드 트리거 사용시 위치 증감에 대한 유효기능 설정을 확인한다.
    DWORD  __stdcall AxcTriggerGetDirectionCheck(long lChannelNo, DWORD* dwpDirection);

    // 카운터 모듈의 위치 주기 모드 트리거 기능의 범위, 위치 주기를 한번에 설정한다.
    // 위치 설정 단위 :  AxcMotSetMoveUnitPerPulse로 설정한 단위.
    DWORD  __stdcall AxcTriggerSetBlock(long lChannelNo, double dLower, double dUpper, double dABSod);

    // 카운터 모듈의 위치 주기 모드 트리거 기능의 범위, 위치 주기를 설정을 한번에 확인한다.
    DWORD  __stdcall AxcTriggerGetBlock(long lChannelNo, double *dpLower, double *dpUpper, double *dpABSod);

    // 카운터 모듈의 트리거 출력 펄스 폭을 설정한다.
    // 단위 : uSec
    DWORD  __stdcall AxcTriggerSetTime(long lChannelNo, double dTrigTime);

    // 카운터 모듈의 트리거 출력 펄스 폭 설정을 확인한다.
    DWORD  __stdcall AxcTriggerGetTime(long lChannelNo, double* dpTrigTime);

    // 카운터 모듈의 트리거 출력 펄스의 출력 레벨을 설정한다.
    // dwLevel -->  0x00 : 트리거 출력시 'Low' 레벨 출력.
    // dwLevel -->  0x01 : 트리거 출력시 'High' 레벨 출력.
    DWORD  __stdcall AxcTriggerSetLevel(long lChannelNo, DWORD dwLevel);
    // 카운터 모듈의 트리거 출력 펄스의 출력 레벨 설정을 확인한다.
    DWORD  __stdcall AxcTriggerGetLevel(long lChannelNo, DWORD* dwpLevel);

    // 카운터 모듈의 주파수 트리거 출력 기능에 필요한 주파수를 설정한다.
    // 단위 : Hz, 범위 : 1Hz ~ 500 kHz
    DWORD  __stdcall AxcTriggerSetFreq(long lChannelNo, DWORD dwFreqency);
    // 카운터 모듈의 주파수 트리거 출력 기능에 필요한 주파수를 설정을 확인한다.
    DWORD  __stdcall AxcTriggerGetFreq(long lChannelNo, DWORD* dwpFreqency);

    // 카운터 모듈의 지정 채널에 대한 범용 출력 값을 설정한다.
    // dwOutput 범위 : 0x00 ~ 0x0F, 각 채널당 4개의 범용 출력
    DWORD  __stdcall AxcSignalWriteOutput(long lChannelNo, DWORD dwOutput);

    // 카운터 모듈의 지정 채널에 대한 범용 출력 값을 확인한다.
    DWORD  __stdcall AxcSignalReadOutput(long lChannelNo, DWORD* dwpOutput);

    // 카운터 모듈의 지정 채널에 대한 범용 출력 값을 비트 별로 설정한다.
    // lBitNo 범위 : 0 ~ 3, 각 채널당 4개의 범용 출력
    DWORD  __stdcall AxcSignalWriteOutputBit(long lChannelNo, long lBitNo, DWORD uOnOff);
    // 카운터 모듈의 지정 채널에 대한 범용 출력 값을 비트 별로 확인 한다.
    // lBitNo 범위 : 0 ~ 3
    DWORD  __stdcall AxcSignalReadOutputBit(long lChannelNo,long lBitNo, DWORD *upOnOff);

    // 카운터 모듈의 지정 채널에 대한 범용 입력 값을 확인한다.
    DWORD  __stdcall AxcSignalReadInput(long lChannelNo, DWORD* dwpInput);

    // 카운터 모듈의 지정 채널에 대한 범용 입력 값을 비트 별로 확인 한다.
    // lBitNo 범위 : 0 ~ 3
    DWORD  __stdcall AxcSignalReadInputBit(long lChannelNo, long lBitNo, DWORD *upOnOff);

    // 카운터 모듈의 트리거 출력을 활성화 한다.
    // 현재 설정된 기능에 따라 트리거 출력이 최종적으로 출력할 것인지 설정한다.
    DWORD  __stdcall AxcTriggerSetEnable(long lChannelNo, DWORD dwUsage);

    // 카운터 모듈의 트리거 출력 활설화 설정 내용을 확인하다.
    DWORD  __stdcall AxcTriggerGetEnable(long lChannelNo, DWORD* dwpUsage);

    // 카운터 모듈의 절대위치 트리거 기능을 위해 설정된 RAM 내용을 확인한다.
    // dwAddr 범위 : 0x0000 ~ 0x1FFFF;
    DWORD  __stdcall AxcTriggerReadAbsRamData(long lChannelNo, DWORD dwAddr, DWORD* dwpData);

    // 카운터 모듈의 절대위치 트리거 기능을 위해 필요한 RAM 내용을 설정한다.
    // dwAddr 범위 : 0x0000 ~ 0x1FFFF;
    DWORD  __stdcall AxcTriggerWriteAbsRamData(long lChannelNo, DWORD dwAddr, DWORD dwData);

	// 지정 CNT 채널의 절대 위치 트리거 기능을 위한 DWORD형 위치 정보를 설정한다.
	//----------------------------------------------------------------------------------------------------------------------------------
	// 1. AXT_SIO_CN2CH인 경우
	// dwTrigNum	--> 131072(=0x20000) 개까지 설정 가능
	// dwTrigPos	--> DWORD형 Data 입력 가능
	// dwDirection	--> 0x0(default) : dwTrigPos[0], dwTrigPos[1] ..., dwTrigPos[dwTrigNum - 1] 순으로 Data를 Write 한다.
	//					0x1			 : dwTrigPos[dwTrigNum - 1], dwTrigPos[dwTrigNum - 2], ..., dwTrigPos[0] 순으로 Data를 Write 한다.
	// *참고* 1) dwDirection: Data Write 순서만 다를 뿐 기능상의 차이 없음
	//		  2) AXC Manual의 AxcTriggerSetAbs - Description을 참고하여 data를 가공 후 사용해야 함
	//----------------------------------------------------------------------------------------------------------------------------------
	// 2. AXT_SIO_HPC4인 경우
	// dwTrigNum	--> 500 개까지 설정 가능
	// dwTrigPos	--> DWORD형 Data 입력 가능
	// dwDirection	--> 0x0(default) : 사용되지 않는 변수로, 입력하지 않아도 된다.
	//----------------------------------------------------------------------------------------------------------------------------------
	// 3. AXT_SIO_RCNT2RTEX, AXT_SIO_RCNT2MLIII, AXT_SIO_RCNT2SIIIH, AXT_SIO_RCNT2SIIIH_R인 경우
	// dwTrigNum	--> 0x200(=512) 개까지 설정 가능
	// dwTrigPos	--> DWORD형 data 입력 가능
	// dwDirection	--> 0x0(default) : 사용되지 않는 변수로, 입력하지 않아도 된다.
	//----------------------------------------------------------------------------------------------------------------------------------
    DWORD  __stdcall AxcTriggerSetAbs(long lChannelNo, DWORD dwTrigNum, DWORD* dwTrigPos, DWORD dwDirection = 0);


	// 지정 CNT 채널의 절대 위치 트리거 기능을 위한 double형 위치 정보를 설정한다.
	//----------------------------------------------------------------------------------------------------------------------------------
	// 1. AXT_SIO_CN2CH인 경우
	// dwTrigNum	--> 4194304(=0x20000*32) 개까지 설정 가능
	// dTrigPos		--> double형 data 입력 가능
	// dwDirection	--> 0x0(default) : dTrigPos[0], dTrigPos[1] ..., dTrigPos[dwTrigNum - 1] 순으로 Data를 Write 한다.
	//					0x1			 : dTrigPos[dwTrigNum - 1], dTrigPos[dwTrigNum - 2], ..., dTrigPos[0] 순으로 Data를 Write 한다.
	// *참고* 1) dwDirection: Data Write 순서만 다를 뿐 기능상의 차이 없음
	//----------------------------------------------------------------------------------------------------------------------------------
	// 2. AXT_SIO_RCNT2RTEX, AXT_SIO_RCNT2MLIII, AXT_SIO_RCNT2SIIIH_R인 경우
	// dwTrigNum	--> 0x200(=512) 개까지 설정 가능
	// dTrigPos		--> double형 data 입력 가능
	// dwDirection	--> 0x0(default) : 사용되지 않는 변수로, 입력하지 않아도 된다.
	//----------------------------------------------------------------------------------------------------------------------------------
    DWORD  __stdcall AxcTriggerSetAbsDouble(long lChannelNo, DWORD dwTrigNum, double* dTrigPos, DWORD dwDirection = 0);

	////////////////// LCM4_10_Version/////////////////////////////////////////////////////////////

	// 카운터 모듈의 PWM 출력을 활성화한다.
	DWORD __stdcall AxcTriggerSetPwmEnable(long lChannelNo, bool bEnable);
	// 카운터 모듈의 PWM 출력 활성화 상태를 확인한다.
	DWORD __stdcall AxcTriggerGetPwmEnable(long lChannelNo, bool *bEnable);
	// 카운터 모듈의 PWM 출력모드를 설정한다.
	// dwMode : PWM 출력모드
	// [0] : Manual (Manual로 설정된 PWM Data)
	// [1] : Auto (속도 Table)
	DWORD __stdcall AxcTriggerSetPwmOutMode(long lChannelNo, DWORD dwMode); 
	// 카운터 모듈의 PWM 출력모드를 확인한다.
	// dwMode : PWM 출력모드
	// [0] : Manual (Manual로 설정된 PWM Data)
	// [1] : Auto (속도 Table)
	DWORD __stdcall AxcTriggerGetPwmOutMode(long lChannelNo, DWORD* dwpMode);	

	// 카운터 모듈의 각 테이블에 2-D 절대속도에서 PWM 신호를 출력하기 위해 필요한 속도 정보를 설정한다.
	// dMinVel : dMinVel
	// dMaxVel : dMaxVel 
	// dVelInterval : 속도 테이블에서의 구간별 속도 Interval
	// ※주의 : dMinVel부터 dVelInterval 간격으로 최대 5000개의 속두구간이 나뉜다.  
	//          (((dMaxVel-dMinVel) / dVelInterval) <= 5000)이 만족하여야 한다. 
	DWORD __stdcall AxcTriggerSetPwmVelInfo(long lChannelNo, double dMinVel, double dMaxVel, double dVelInterval);
	// 카운터 모듈의 각 테이블에 2-D 절대속도에서 PWM 신호를 출력하기 위해 필요한 속도 정보를 확인한다.
	DWORD __stdcall AxcTriggerGetPwmVelInfo(long lChannelNo, double* dpMinVel, double* dpMaxVel, double* dpVelInterval);
	//	카운터 모듈의 PWM 출력에서 Pulse 폭 제어방식을 설정한다.
	// dwMode : Pulse 폭 제어 
	// [0] : DutyRatio
	// [1] : PulseWidth
	DWORD __stdcall AxcTriggerSetPwmPulseControl(long lChannelNo, DWORD dwMode);
	// 	카운터 모듈의 PWM 출력에서 Pulse 조절방식을 확인다.
	// dwpMode : Pulse 폭 제어 
	// [0] : DutyRatio
	// [1] : PulseWidth	
	DWORD __stdcall AxcTriggerGetPwmPulseControl(long lChannelNo, DWORD *dwpMode);

	// 카운터 모듈의 각 테이블에 2-D 절대속도에서 PWM 신호를 출력하기 위해 필요한 정보를 설정한다.
	// pwm 출력모드가 Manual 인 경우에만 유효하다
	// dFrequency :  (0.017 ~ 1M) 까지 설정이 가능하다. (Hz 단위)
	// dData : Pulse 폭 관련 Data 를 입력하며 Pulse Control 방식에 따라 Data 형식이 다르다.
	// Pulse 폭 제어방식이 DutyRatio인 경우 DutyRatio 
	// Pulse 폭 제어방식이 PulseWidth 인 경우 PulseWidth (us단위) 
	DWORD __stdcall AxcTriggerSetPwmManualData(long lChannelNo,double dFrequency, double dData);
	// 카운터 모듈의 각 테이블에 2-D 절대속도에서 PWM 신호를 출력하기 위해 필요한 정보를 확인한다.
	DWORD __stdcall AxcTriggerGetPwmManualData(long lChannelNo,double* dpFrequency, double* dpData);
	// 카운터 모듈의 각 테이블에 2-D 절대속도에서 PWM 신호를 출력하기 위해 필요한 정보를 설정한다.
	// lDataCnt : 설정 할 트리거 정보의 전체 개수
	// dpVel : dpVel[0],dpVel[1]....dpVel[DataCnt -1] 순서로 입력 가능
	// dwpFrequency : dwpFrequency[0],dwpFrequency[1]....dwpFrequency[DataCnt-1] 순서로 입력 가능(0.017 ~ 1M) 까지 설정이 가능하다.
	// dData : Pulse 폭 관련 Data 를 입력하며 Pulse Control 방식에 따라 Data 형식이 다르다.
	// Pulse 폭 제어방식이 DutyRatio인 경우 DutyRatio 
	// Pulse 폭 제어방식이 PulseWidth 인 경우 PulseWidth (us단위) 
	// ※주의 : 
	//    1) dpVel, dwpFrequency, dwpDutyRatio 의 배열 개수를 주의하여 사용해야한다.
	//		- 속도가 0인 구간에서는 PWM 출력이 불가하다.
	//    3) PWM Enable 상태에서는 사용할 수 없다.
	DWORD __stdcall AxcTriggerSetPwmPatternData(long lChannelNo, long lDataCnt,  double *dpVel, double *dpFrequency, double *dpData);
	// 카운터 모듈의 각 테이블에 2-D 절대속도에서 PWM 신호를 출력하기 위해 필요한 정보를 설정한다.
	DWORD __stdcall AxcTriggerSetPwmData(long lChannelNo, double dVel, double dFrequency, double dData);
	// 카운터 모듈의 각 테이블에 2-D 절대속도에서 PWM 신호를 출력하기 위해 필요한 정보를 확인한다.
	DWORD __stdcall AxcTriggerGetPwmData(long lChannelNo, double dVel, double *dpFrequency, double *dpData);
	// 카운터 모듈의 속도 값을 확인 합니다.
	DWORD __stdcall AxcStatusReadActVel(long lChannelNo, double* dpActVel);
	// 카운터 모듈의 2D 속도 값을 확인 합니다.
	DWORD __stdcall AxcStatusRead2DActVel(long lChannelNo, double* dpActVel);
	// 카운터 모듈의 Position 값을 초기화 한다.
	DWORD __stdcall AxcStatusSetActPosClear(long lChannelNo);	
	////////////////// HPC4_30_Version
	// 카운터 모듈의 각 테이블에 할당된 트리거 출력의 레벨을 설정한다.
	// uLevel : 트리거 출력 신호의 Active Level
	//   [0]  : 트리거 출력시 'Low' 레벨 출력.
	//   [1]  : 트리거 출력시 'High' 레벨 출력.
    DWORD  __stdcall AxcTableSetTriggerLevel(long lModuleNo, long lTablePos, DWORD uLevel);
	// 카운터 모듈의 각 테이블에 지정된 트리거 출력의 레벨 설정값을 확인한다.
    DWORD  __stdcall AxcTableGetTriggerLevel(long lModuleNo, long lTablePos, DWORD* upLevel);

	// 카운터 모듈의 각 테이블에 할당된 트리거 출력의 펄스 폭을 설정한다.
	// dTriggerTimeUSec : [Default 500ms], us단위로 지정
	DWORD  __stdcall AxcTableSetTriggerTime(long lModuleNo, long lTablePos, double dTriggerTimeUSec);
	// 카운터 모듈의 각 테이블에 지정된 트리거 출력의 펄스 폭 설정값을 확인한다.
    DWORD  __stdcall AxcTableGetTriggerTime(long lModuleNo, long lTablePos, double* dpTriggerTimeUSec);

	// 카운터 모듈의 각 테이블에 할당 할 2개의 엔코더 입력 신호를 설정한다.
	// uEncoderInput1 [0-3]: 카운터 모듈에 입력되는 4개의 엔코더 신호중 하나
	// uEncoderInput2 [0-3]: 카운터 모듈에 입력되는 4개의 엔코더 신호중 하나
	DWORD  __stdcall AxcTableSetEncoderInput(long lModuleNo, long lTablePos, DWORD uEncoderInput1, DWORD uEncoderInput2);
	// 카운터 모듈의 각 테이블에 할당 된 2개의 엔코더 입력 신호를 확인한다.
	DWORD  __stdcall AxcTableGetEncoderInput(long lModuleNo, long lTablePos, DWORD* upEncoderInput1, DWORD* upEncoderInput2);

	// 카운터 모듈의 각 테이블에 할당 할 트리거 출력 포트를 설정한다.
	// uTriggerOutport [0x0-0xF]: Bit0: 트리거 출력 0, Bit1: 트리거 출력 1, Bit2: 트리거 출력 2, Bit3: 트리거 출력 3 
	// Ex) 0x3(3)   : 출력 0, 1에 트리거 신호를 출력하는 경우
	//     0xF(255) : 출력 0, 1, 2, 3에 트리거 신호를 출력하는 경우
	DWORD  __stdcall AxcTableSetTriggerOutport(long lModuleNo, long lTablePos, DWORD uTriggerOutport);
	// 카운터 모듈의 각 테이블에 할당 된 트리거 출력 포트를 확인한다.
	DWORD  __stdcall AxcTableGetTriggerOutport(long lModuleNo, long lTablePos, DWORD* upTriggerOutport);

	// 카운터 모듈의 각 테이블에 설정된 트리거 위치에 대한 허용 오차 범위를 설정한다.
	// dErrorRange  : 지정 축의 Unit단위로 트리거 위치에 대한 허용 오차 범위를 설정
	DWORD  __stdcall AxcTableSetErrorRange(long lModuleNo, long lTablePos, double dErrorRange);
	// 카운터 모듈의 각 테이블에 설정된 트리거 위치에 대한 허용 오차 범위를 확인한다.
	DWORD  __stdcall AxcTableGetErrorRange(long lModuleNo, long lTablePos, double* dpErrorRange);

	// 카운터 모듈의 각 테이블에 설정된 정보로(트리거 출력 Port, 트리거 펄스 폭) 트리거를 1개 발생시킨다.
	// ※ 주의 : 1) 트리거가 Disable되어 있으면 이 함수는 자동으로 Enable시켜 트리거를 발생시킴 
	//           2) Trigger Mode가 HPC4_PATTERN_TRIGGER 모드일 경우 이 함수는 자동으로 트리거 모드를 HPC4_RANGE_TRIGGER로 변경 함(하나의 트리거만 발생시키기 위해)
	DWORD  __stdcall AxcTableTriggerOneShot(long lModuleNo, long lTablePos);

	// 카운터 모듈의 각 테이블에 설정된 정보로(트리거 출력 Port, 트리거 펄스 폭), 지정한 개수만큼 설정한 주파수로 트리거를 발생시킨다.
    // lTriggerCount     : 지정한 주파수를 유지하며 발생시킬 트리거 출력 개수
	// uTriggerFrequency : 트리거를 발생시킬 주파수	
	// ※ 주의 : 1) 트리거가 Disable되어 있으면 이 함수는 자동으로 Enable시켜 패턴을 가진 트리거를 발생시킴 
	//           2) Trigger Mode가 HPC4_PATTERN_TRIGGER 모드가 아닐 경우 이 함수는 자동으로 트리거 모드를 HPC4_PATTERN_TRIGGER로 변경 함
	DWORD  __stdcall AxcTableTriggerPatternShot(long lModuleNo, long lTablePos, long lTriggerCount, DWORD uTriggerFrequency);
	// 카운터 모듈의 각 테이블에 설정된 패턴 트리거 설정 정보를(주파수, 카운터) 확인한다.
	DWORD  __stdcall AxcTableGetPatternShotData(long lModuleNo, long lTablePos, long* lpTriggerCount, DWORD* upTriggerFrequency);

	// 카운터 모듈의 각 테이블에 트리거를 출력하는 방식을 설정한다.
	// uTrigMode : 트리거를 출력하는 방식을 설정한다.
	//   [0] HPC4_RANGE_TRIGGER   : 지정한 트리거 위치에 설정한 허용 범위안에 위치할 때 트리거를 출력하는 모드
	//   [1] HPC4_VECTOR_TRIGGER  : 지한 트리거 위치에 설정한 허용 범위와 벡터 방향이 일치할 때 트리거를 출력하는 모드
	//   [3] HPC4_PATTERN_TRIGGER : 위치와 무관하게 지정한 개수만큼 설정한 주파수로 트리거를 출력하는 모드
	DWORD  __stdcall AxcTableSetTriggerMode(long lModuleNo, long lTablePos, DWORD uTrigMode);
	// 카운터 모듈의 각 테이블에 설정된 트리거를 출력하는 방식을 확인한다
	DWORD  __stdcall AxcTableGetTriggerMode(long lModuleNo, long lTablePos, DWORD* upTrigMode);		
	// 카운터 모듈의 각 테이블 별로 출력된 누적 트리거 갯수를 초기화 한다.
	DWORD  __stdcall AxcTableSetTriggerCountClear(long lModuleNo, long lTablePos);

	// 카운터 모듈의 각 테이블에 2-D 절대위치에서 트리거 신호를 출력하기 위해 필요한 정보를 설정한다.
	// lTriggerDataCount : 설정 할 트리거 정보의 전체 개수
    //   [-1, 0]         : 등록된 트리거 정보 데이타 초기화
	// dpTriggerData     : 2-D 절대위치 트리거 정보(해당 배열의 개수는 lTriggerDataCount * 2가 되어야됨)
	//   *[0, 1]         : X[0], Y[0] 
	// lpTriggerCount    : 입력한 2-D 절대 트리거 위치에서 트리거 조건 만족 시 발생시킬 트리거 갯수를 배열로 설정(해당 배열의 개수는 lTriggerDataCount)
	// dpTriggerInterval : TriggerCount 만큼 연속해서 트리거를 발생시킬때 유지 할 간격을 주파수 단위로 설정(해당 배열의 개수는 lTriggerDataCount)
	// ※주의 : 
	//    1) 각 전달인자의 배열 개수를 주의하여 사용해야됩니다. 내부에서 사용되는 인자 보다 적은 배열을 지정하면 메모리 참조 오류가 발생 될 수 있음.
	//    2) Trigger Mode는 HPC4_RANGE_TRIGGER로 자동 변경됨
	//    3) 함수 내부에서 Trigger를 Disable한 후 모든 설정을 진행하며 완료 후 다시 Enable 시킴
	DWORD  __stdcall AxcTableSetTriggerData(long lModuleNo, long lTablePos, long lTriggerDataCount, double* dpTriggerData, long* lpTriggerCount, double* dpTriggerInterval);
	// 카운터 모듈의 각 테이블에 트리거 신호를 출력하기 위해 설정한 트리거 설정 정보를 확인한다.
	// ※ 주의 : 각 테이블에 등록된 최대 트리거 데이타 개수를 모를 때는 아래와 같이 트리거 데이타 개수를 미리 파악한 후 사용하십시요.
	// Ex)      1) AxcTableGetTriggerData(lModuleNo, lTablePos, &lTriggerDataCount, NULL, NULL, NULL);
	//          2) dpTriggerData     = new double[lTriggerDataCount * 2];
	//          3) lpTriggerCount    = new long[lTriggerDataCount];
	//          4) dpTriggerInterval = new double[lTriggerDataCount];  
	DWORD  __stdcall AxcTableGetTriggerData(long lModuleNo, long lTablePos, long* lpTriggerDataCount, double* dpTriggerData, long* lpTriggerCount, double* dpTriggerInterval);
	
	// 카운터 모듈의 각 테이블에 2-D 절대위치에서 트리거 신호를 출력하기 위해 필요한 정보를 AxcTableSetTriggerData함수와 다른 방식으로 설정한다.
	// lTriggerDataCount : 설정 할 트리거 정보의 전체 개수
	// uOption : dpTriggerData 배열의 데이타 입력 방식을 지정 
	//   [0]   : dpTriggerData 배열에 X Pos[0], Y Pos[0], X Pos[1], Y Pos[1] 순서로 입력
	//   [1]   : dpTriggerData 배열에 X Pos[0], Y Pos[0], Count, Inteval, X Pos[1], Y Pos[1], Count, Inteval 순서로 입력
	// ※주의 : 
	//    1) dpTriggerData의 배열 개수를 주의하여 사용해야됩니다. 내부에서 사용되는 인자 보다 적은 배열을 지정하면 메모리 참조 오류가 발생 될 수 있음.
	//    2) Trigger Mode는 HPC4_RANGE_TRIGGER로 자동 변경됨
	//    3) 함수 내부에서 Trigger를 Disable한 후 모든 설정을 진행하며 완료 후 다시 Enable 시킴
    DWORD  __stdcall AxcTableSetTriggerDataEx(long lModuleNo, long lTablePos, long lTriggerDataCount, DWORD uOption, double* dpTriggerData);
	// 카운터 모듈의 각 테이블에 트리거 신호를 출력하기 위해 설정한 트리거 설정 정보를 확인한다.
	// ※ 주의 : 각 테이블에 등록된 최대 트리거 데이타 개수를 모를 때는 아래와 같이 트리거 데이타 개수를 미리 파악한 후 사용.
	// Ex)      1) AxcTableGetTriggerDataEx(lModuleNo, lTablePos, &lTriggerDataCount, &uOption, NULL);
	//          2) if(uOption == 0) : dpTriggerData     = new double[lTriggerDataCount * 2];
	//          3) if(uOption == 1) : dpTriggerData     = new double[lTriggerDataCount * 4];
	//          4) dpTriggerInterval = new double[lTriggerDataCount];  
    DWORD  __stdcall AxcTableGetTriggerDataEx(long lModuleNo, long lTablePos, long* lpTriggerDataCount,  DWORD* upOption, double* dpTriggerData);
  
	// 카운터 모듈의 지정한 테이블에 설정된 모든 트리거 데이타와 H/W FIFO의 데이타를 모두 삭제한다.
	DWORD  __stdcall AxcTableSetTriggerDataClear(long lModuleNo, long lTablePos);

	// 카운터 모듈의 지정한 테이블의 트리거 출력 기능을 동작시킴.
	// uEnable : 트리거를 출력 기능의 사용여부를 설정
	// ※ 주의 : 1) 트리거 출력 중 DISABLE하면 출력이 바로 멈춤
	//           2) AxcTableTriggerOneShot, AxcTableGetPatternShotData,AxcTableSetTriggerData, AxcTableGetTriggerDataEx 함수 호출시 자동으로 ENABLE됨
	DWORD  __stdcall AxcTableSetEnable(long lModuleNo, long lTablePos, DWORD uEnable);
	// 카운터 모듈의 지정한 테이블의 트리거 출력 기능의 동작 여부를 확인함.
	DWORD  __stdcall AxcTableGetEnable(long lModuleNo, long lTablePos, DWORD* upEnable);		

	// 카운터 모듈의 지정한 테이블을 이용해 발생된 트리거 개수를 확인.
	// lpTriggerCount : 현재까지 출력된 트리거 출력 개수를 반환, AxcTableSetTriggerCountClear 함수로 초기화
	DWORD  __stdcall AxcTableReadTriggerCount(long lModuleNo, long lTablePos, long* lpTriggerCount);	

	// 카운터 모듈의 지정한 테이블에 할당된 H/W 트리거 데이타 FIFO의 상태를 확인.
	// lpCount1[0~500] : 2D 트리거 위치 데이타 중 첫번째(X) 위치를 저정하고 있는 H/W FIFO에 입력된 데이타 개수
	// upStatus1 : 2D 트리거 위치 데이타 중 첫번째(X) 위치를 저정하고 있는 H/W FIFO의 상태
	//   [Bit 0] : Data Empty
	//   [Bit 1] : Data Full
	//   [Bit 2] : Data Valid
	// lpCount2[0~500] : 2D 트리거 위치 데이타 중 두번째(Y) 위치를 저정하고 있는 H/W FIFO에 입력된 데이타 개수
	// upStatus2 : 2D 트리거 위치 데이타 중 두번째(Y) 위치를 저정하고 있는 H/W FIFO의 상태
	//   [Bit 0] : Data Empty
	//   [Bit 1] : Data Full
	//   [Bit 2] : Data Valid
	DWORD  __stdcall AxcTableReadFifoStatus(long lModuleNo, long lTablePos, long* lpCount1, DWORD* upStatus1, long* lpCount2, DWORD* upStatus2);

	// 카운터 모듈의 지정한 테이블에 할당된 H/W 트리거 데이타 FIFO의 현재 위치 데이타값을 확인.
	// dpTopData1 : 2D H/W 트리거 데이타 FIFO의 현재 데이타 중 첫번째(X) 위치 데이타를 확인 함 
	// dpTopData1 : 2D H/W 트리거 데이타 FIFO의 현재 데이타 중 두번째(Y) 위치 데이타를 확인 함 
	DWORD  __stdcall AxcTableReadFifoData(long lModuleNo, long lTablePos, double* dpTopData1, double* dpTopData2);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
}
#endif    //__cplusplus

#endif    //__AXT_AXC_H__
