#ifndef __AXT_AXDEV_H__
#define __AXT_AXDEV_H__


#define WIN32_LEAN_AND_MEAN

#include <tchar.h>

#include "AXHD.h"

typedef TCHAR* PTCHAR;

#ifdef __cplusplus
extern "C" {
#endif    //__cplusplus

    // Board Number를 이용하여 Board Address 찾기
    DWORD    __stdcall AxlGetBoardAddress(long lBoardNo, DWORD *upBoardAddress);
    // Board Number를 이용하여 Board ID 찾기
    DWORD    __stdcall AxlGetBoardID(long lBoardNo, DWORD *upBoardID);
    // Board Number를 이용하여 Board Version 찾기
    DWORD    __stdcall AxlGetBoardVersion(long lBoardNo, DWORD *upBoardVersion);
    // Board Number와 Module Position을 이용하여 Module ID 찾기
    DWORD    __stdcall AxlGetModuleID(long lBoardNo, long lModulePos, DWORD *upModuleID);
    // Board Number와 Module Position을 이용하여 Module Version 찾기
    DWORD    __stdcall AxlGetModuleVersion(long lBoardNo, long lModulePos, DWORD *upModuleVersion);
    // Board Number와 Module Position을 이용하여 Network Node 정보 확인
    DWORD    __stdcall AxlGetModuleNodeInfo(long lBoardNo, long lModulePos, long *upNetNo, DWORD *upNodeAddr);

    // Board에 내장된 범용 Data Flash Write (PCI-R1604[RTEX master board]전용)
    // lPageAddr(0 ~ 199)
    // lByteNum(1 ~ 120)
    // 주의) Flash에 데이타를 기입할 때는 일정 시간(최대 17mSec)이 소요되기때문에 연속 쓰기시 지연 시간이 필요함.
    DWORD    __stdcall AxlSetDataFlash(long lBoardNo, long lPageAddr, long lBytesNum, BYTE *bpSetData);

    // Board에 내장된 ESTOP 외부 입력 신호를 이용한 InterLock 기능 사용 유무 및 디지털 필터 상수값 정의 (PCI-Rxx00[MLIII master board]전용)
    // 1. 사용 유무
    //	  설명: 기능 사용 설정후 외부에서 ESTOP 신호 인가시 보드에 연결된 모션 제어 노드에 대해서 ESTOP 구동 명령 실행	
    //    0: 기능 사용하지 않음(기본 설정값)
    //    1: 기능 사용
    // 2. 디지털 필터 값
    //      입력 필터 상수 설정 범위 1 ~ 40, 단위 통신 Cyclic time
	// Board 에 dwInterLock, dwDigFilterVal을 이용하여 EstopInterLock 기능 설정
	DWORD    __stdcall AxlSetEStopInterLock(long lBoardNo, DWORD dwInterLock, DWORD dwDigFilterVal);
	// Board에 설정된 dwInterLock, dwDigFilterVal 정보를 가져오기
	DWORD    __stdcall AxlGetEStopInterLock(long lBoardNo, DWORD *dwInterLock, DWORD  *dwDigFilterVal);
	// Board에 입력된 EstopInterLock 신호를 읽는다.
	DWORD    __stdcall AxlReadEStopInterLock(long lBoardNo, DWORD *dwInterLock);
	
    // Board에 내장된 범용 Data Flash Read(PCI-R1604[RTEX master board]전용)
    // lPageAddr(0 ~ 199)
    // lByteNum(1 ~ 120)
    DWORD    __stdcall AxlGetDataFlash(long lBoardNo, long lPageAddr, long lBytesNum, BYTE *bpGetData);

    // Board Number와 Module Position을 이용하여 AIO Module Number 찾기
    DWORD    __stdcall AxaInfoGetModuleNo(long lBoardNo, long lModulePos, long *lpModuleNo);
    // Board Number와 Module Position을 이용하여 DIO Module Number 찾기
    DWORD    __stdcall AxdInfoGetModuleNo(long lBoardNo, long lModulePos, long *lpModuleNo);

    // 지정 축에 IPCOMMAND Setting
    DWORD    __stdcall AxmSetCommand(long lAxisNo, IPCOMMAND sCommand);
    // 지정 축에 8bit IPCOMMAND Setting
    DWORD    __stdcall AxmSetCommandData08(long lAxisNo, IPCOMMAND sCommand, DWORD uData);
    // 지정 축에 8bit IPCOMMAND 가져오기
    DWORD    __stdcall AxmGetCommandData08(long lAxisNo, IPCOMMAND sCommand, DWORD *upData);
    // 지정 축에 16bit IPCOMMAND Setting
    DWORD    __stdcall AxmSetCommandData16(long lAxisNo, IPCOMMAND sCommand, DWORD uData);
    // 지정 축에 16bit IPCOMMAND 가져오기
    DWORD    __stdcall AxmGetCommandData16(long lAxisNo, IPCOMMAND sCommand, DWORD *upData);
    // 지정 축에 24bit IPCOMMAND Setting
    DWORD    __stdcall AxmSetCommandData24(long lAxisNo, IPCOMMAND sCommand, DWORD uData);
    // 지정 축에 24bit IPCOMMAND 가져오기
    DWORD    __stdcall AxmGetCommandData24(long lAxisNo, IPCOMMAND sCommand, DWORD *upData);
    // 지정 축에 32bit IPCOMMAND Setting
    DWORD    __stdcall AxmSetCommandData32(long lAxisNo, IPCOMMAND sCommand, DWORD uData);
    // 지정 축에 32bit IPCOMMAND 가져오기
    DWORD    __stdcall AxmGetCommandData32(long lAxisNo, IPCOMMAND sCommand, DWORD *upData);

    // 지정 축에 QICOMMAND Setting
    DWORD    __stdcall AxmSetCommandQi(long lAxisNo, QICOMMAND sCommand);
    // 지정 축에 8bit QICOMMAND Setting
    DWORD    __stdcall AxmSetCommandData08Qi(long lAxisNo, QICOMMAND sCommand, DWORD uData);
    // 지정 축에 8bit QICOMMAND 가져오기
    DWORD    __stdcall AxmGetCommandData08Qi(long lAxisNo, QICOMMAND sCommand, DWORD *upData);
    // 지정 축에 16bit QICOMMAND Setting
    DWORD    __stdcall AxmSetCommandData16Qi(long lAxisNo, QICOMMAND sCommand, DWORD uData);
    // 지정 축에 16bit QICOMMAND 가져오기
    DWORD    __stdcall AxmGetCommandData16Qi(long lAxisNo, QICOMMAND sCommand, DWORD *upData);
    // 지정 축에 24bit QICOMMAND Setting
    DWORD    __stdcall AxmSetCommandData24Qi(long lAxisNo, QICOMMAND sCommand, DWORD uData);
    // 지정 축에 24bit QICOMMAND 가져오기
    DWORD    __stdcall AxmGetCommandData24Qi(long lAxisNo, QICOMMAND sCommand, DWORD *upData);
    // 지정 축에 32bit QICOMMAND Setting
    DWORD    __stdcall AxmSetCommandData32Qi(long lAxisNo, QICOMMAND sCommand, DWORD uData);
    // 지정 축에 32bit QICOMMAND 가져오기
    DWORD    __stdcall AxmGetCommandData32Qi(long lAxisNo, QICOMMAND sCommand, DWORD *upData);

    // 지정 축에 Port Data 가져오기 - IP
    DWORD    __stdcall AxmGetPortData(long lAxisNo,  WORD wOffset, DWORD *upData);
    // 지정 축에 Port Data Setting - IP
    DWORD    __stdcall AxmSetPortData(long lAxisNo, WORD wOffset, DWORD dwData);
    // 지정 축에 Port Data 가져오기 - QI
    DWORD    __stdcall AxmGetPortDataQi(long lAxisNo, WORD byOffset, WORD *wData);
    // 지정 축에 Port Data Setting - QI
    DWORD    __stdcall AxmSetPortDataQi(long lAxisNo, WORD byOffset, WORD wData);

    // 지정 축에 스크립트를 설정한다. - IP
    // sc    : 스크립트 번호 (1 - 4)
    // event : 발생할 이벤트 SCRCON 을 정의한다.
    //         이벤트 설정 축갯수설정, 이벤트 발생할 축, 이벤트 내용 1,2 속성 설정한다.
    // cmd   : 어떤 내용을 바꿀것인지 선택 SCRCMD를 정의한다.
    // data  : 어떤 Data를 바꿀것인지 선택
    DWORD    __stdcall AxmSetScriptCaptionIp(long lAxisNo, long sc, DWORD event, DWORD data);
    // 지정 축에 스크립트를 반환한다. - IP
    DWORD    __stdcall AxmGetScriptCaptionIp(long lAxisNo, long sc, DWORD *event, DWORD *data);

    // 지정 축에 스크립트를 설정한다. - QI
    // sc    : 스크립트 번호 (1 - 4)
    // event : 발생할 이벤트 SCRCON 을 정의한다.
    //         이벤트 설정 축갯수설정, 이벤트 발생할 축, 이벤트 내용 1,2 속성 설정한다.
    // cmd   : 어떤 내용을 바꿀것인지 선택 SCRCMD를 정의한다.
    // data  : 어떤 Data를 바꿀것인지 선택
    DWORD    __stdcall AxmSetScriptCaptionQi(long lAxisNo, long sc, DWORD event, DWORD cmd, DWORD data);
    // 지정 축에 스크립트를 반환한다. - QI
    DWORD    __stdcall AxmGetScriptCaptionQi(long lAxisNo, long sc, DWORD *event, DWORD *cmd, DWORD *data);

    // 지정 축에 스크립트 내부 Queue Index를 Clear 시킨다.
    // uSelect IP. 
    // uSelect(0): 스크립트 Queue Index 를 Clear한다.
    //        (1): 캡션 Queue를 Index Clear한다.
    // uSelect QI. 
    // uSelect(0): 스크립트 Queue 1 Index 을 Clear한다.
    //        (1): 스크립트 Queue 2 Index 를 Clear한다.
    DWORD    __stdcall AxmSetScriptCaptionQueueClear(long lAxisNo, DWORD uSelect);

    // 지정 축에 스크립트 내부 Queue의 Index 반환한다. 
    // uSelect IP
    // uSelect(0): 스크립트 Queue Index를 읽어온다.
    //        (1): 캡션 Queue Index를 읽어온다.
    // uSelect QI. 
    // uSelect(0): 스크립트 Queue 1 Index을 읽어온다.
    //        (1): 스크립트 Queue 2 Index를 읽어온다.
    DWORD    __stdcall AxmGetScriptCaptionQueueCount(long lAxisNo, DWORD *updata, DWORD uSelect);

    // 지정 축에 스크립트 내부 Queue의 Data갯수 반환한다. 
    // uSelect IP
    // uSelect(0): 스크립트 Queue Data 를 읽어온다.
    //        (1): 캡션 Queue Data를 읽어온다.
    // uSelect QI.
    // uSelect(0): 스크립트 Queue 1 Data 읽어온다.
    //        (1): 스크립트 Queue 2 Data 읽어온다.
    DWORD    __stdcall AxmGetScriptCaptionQueueDataCount(long lAxisNo, DWORD *updata, DWORD uSelect);

    // 내부 데이타를 읽어온다.
    DWORD    __stdcall AxmGetOptimizeDriveData(long lAxisNo, double dMinVel, double dVel, double dAccel, double  dDecel, 
            WORD *wRangeData, WORD *wStartStopSpeedData, WORD *wObjectSpeedData, WORD *wAccelRate, WORD *wDecelRate);

    // 보드내에 레지스터를 Byte단위로 설정 및 확인한다.
    DWORD    __stdcall AxmBoardWriteByte(long lBoardNo, WORD wOffset, BYTE byData);
    DWORD    __stdcall AxmBoardReadByte(long lBoardNo, WORD wOffset, BYTE *byData);

    // 보드내에 레지스터를 Word단위로 설정 및 확인한다.
    DWORD    __stdcall AxmBoardWriteWord(long lBoardNo, WORD wOffset, WORD wData);
    DWORD    __stdcall AxmBoardReadWord(long lBoardNo, WORD wOffset, WORD *wData);

    // 보드내에 레지스터를 DWord단위로 설정 및 확인한다.
    DWORD    __stdcall AxmBoardWriteDWord(long lBoardNo, WORD wOffset, DWORD dwData);
    DWORD    __stdcall AxmBoardReadDWord(long lBoardNo, WORD wOffset, DWORD *dwData);

    // 보드내에 모듈에 레지스터를 Byte설정 및 확인한다.
    DWORD    __stdcall AxmModuleWriteByte(long lBoardNo, long lModulePos, WORD wOffset, BYTE byData);
    DWORD    __stdcall AxmModuleReadByte(long lBoardNo, long lModulePos, WORD wOffset, BYTE *byData);

    // 보드내에 모듈에 레지스터를 Word설정 및 확인한다.
    DWORD    __stdcall AxmModuleWriteWord(long lBoardNo, long lModulePos, WORD wOffset, WORD wData);
    DWORD    __stdcall AxmModuleReadWord(long lBoardNo, long lModulePos, WORD wOffset, WORD *wData);

    // 보드내에 모듈에 레지스터를 DWord설정 및 확인한다.
    DWORD    __stdcall AxmModuleWriteDWord(long lBoardNo, long lModulePos, WORD wOffset, DWORD dwData);
    DWORD    __stdcall AxmModuleReadDWord(long lBoardNo, long lModulePos, WORD wOffset, DWORD *dwData);

    // 외부 위치 비교기에 값을 설정한다.(Pos = Unit)
    DWORD    __stdcall AxmStatusSetActComparatorPos(long lAxisNo, double dPos);
    // 외부 위치 비교기에 값을 반환한다.(Positon = Unit)
    DWORD    __stdcall AxmStatusGetActComparatorPos(long lAxisNo, double *dpPos);

    // 내부 위치 비교기에 값을 설정한다.(Pos = Unit)
    DWORD    __stdcall AxmStatusSetCmdComparatorPos(long lAxisNo, double dPos);
    // 내부 위치 비교기에 값을 반환한다.(Pos = Unit)
    DWORD    __stdcall AxmStatusGetCmdComparatorPos(long lAxisNo, double *dpPos);
	// ABS Position 을 Flash 에 설정한다.
    DWORD    __stdcall AxmStatusSetFlashAbsOffset(long lAxisNo, long dPosition);
	// Flash 에 저장 된 ABS Position 을 반환한다.
	// dReadType  : Value in Flash Memory (0), Real used Value in memory(1)
    DWORD    __stdcall AxmStatusGetFlashAbsOffset(long lAxisNo, long* dPosition, DWORD dReadType);
	// 사용자가 Flash 에 ABS Position 저장할 수 있는 옵션을 설정한다.
	DWORD	 __stdcall AxmStatusSetAbsOffsetWriteEnable(long lAxisNo, bool bStatus);
	// ABS Position 설정 옵션의 상태를 반환한다.
	DWORD	 __stdcall AxmStatusGetAbsOffsetWriteEnable(long lAxisNo, bool *bpStatus);	

//========== 추가 함수 =========================================================================================================
    // 직선 보간 을 속도만 가지고 무한대로 증가한다.
    // 속도 비율대로 거리를 넣어주어야 한다. 
    DWORD    __stdcall AxmLineMoveVel(long lCoord, double dVel, double dAccel, double dDecel);

//========= 센서 위치 구동 함수( 필독: IP만가능 , QI에는 기능없음)==============================================================
    // 지정 축의 Sensor 신호의 사용 유무 및 신호 입력 레벨을 설정한다.
    // 사용 유무 LOW(0), HIGH(1), UNUSED(2), USED(3)
    DWORD    __stdcall AxmSensorSetSignal(long lAxisNo, DWORD uLevel);
    // 지정 축의 Sensor 신호의 사용 유무 및 신호 입력 레벨을 반환한다.
    DWORD    __stdcall AxmSensorGetSignal(long lAxisNo, DWORD *upLevel);
    // 지정 축의 Sensor 신호의 입력 상태를 반환한다
    DWORD    __stdcall AxmSensorReadSignal(long lAxisNo, DWORD *upStatus);

    // 지정 축의 설정된 속도와 가속율로 센서 위치 드라이버를 구동한다.
    // Sensor 신호의 Active level입력 이후 상대 좌표로 설정된 거리만큼 구동후 정지한다.
    // 펄스가 출력되는 시점에서 함수를 벗어난다.
    // lMethod :  0 - 일반 구동, 1 - 센서 신호 검출 전은 저속 구동. 신호 검출 후 일반 구동
    //            2 - 저속 구동
    DWORD    __stdcall AxmSensorMovePos(long lAxisNo, double dPos, double dVel, double dAccel, double dDecel, long lMethod);

    // 지정 축의 설정된 속도와 가속율로 센서 위치 드라이버를 구동한다.
    // Sensor 신호의 Active level입력 이후 상대 좌표로 설정된 거리만큼 구동후 정지한다.
    // 펄스 출력이 종료되는 시점에서 함수를 벗어난다.
    DWORD    __stdcall AxmSensorStartMovePos(long lAxisNo, double dPos, double dVel, double dAccel, double dDecel, long lMethod);

    // 원점검색 진행스탭 변화의 기록을 반환한다.
    // *lpStepCount      : 기록된 Step의 개수
    // *upMainStepNumber : 기록된 MainStepNumber 정보의 배열포인트
    // *upStepNumber     : 기록된 StepNumber 정보의 배열포인트
    // *upStepBranch     : 기록된 Step별 Branch 정보의 배열포인트
    // 주의: 배열개수는 50개로 고정
    DWORD    __stdcall AxmHomeGetStepTrace(long lAxisNo, long *lpStepCount, DWORD *upMainStepNumber, DWORD *upStepNumber, DWORD *upStepBranch);

//=======추가 홈 서치 (PI-N804/404에만 해당됨.)=================================================================================
    // 사용자가 지정한 축의 홈설정 파라메타를 설정한다.(QI칩 전용 레지스터 이용).
    // uZphasCount : 홈 완료후에 Z상 카운트(0 - 15)
    // lHomeMode   : 홈 설정 모드( 0 - 12)
    // lClearSet   : 위치 클리어 , 잔여펄스 클리어 사용 선택 (0 - 3)
    //               0: 위치클리어 사용않함, 잔여펄스 클리어 사용 안함
    //                 1: 위치클리어 사용함, 잔여펄스 클리어 사용 안함
    //               2: 위치클리어 사용안함, 잔여펄스 클리어 사용함
    //               3: 위치클리어 사용함, 잔여펄스 클리어 사용함.
    // dOrgVel : 홈관련 Org  Speed 설정 
    // dLastVel: 홈관련 Last Speed 설정 
    DWORD    __stdcall AxmHomeSetConfig(long lAxisNo, DWORD uZphasCount, long lHomeMode, long lClearSet, double dOrgVel, double dLastVel, double dLeavePos);
    // 사용자가 지정한 축의 홈설정 파라메타를 반환한다.
    DWORD    __stdcall AxmHomeGetConfig(long lAxisNo, DWORD *upZphasCount, long *lpHomeMode, long *lpClearSet, double *dpOrgVel, double *dpLastVel, double *dpLeavePos); //KKJ(070215)

    // 사용자가 지정한 축의 홈 서치를 시작한다.
    // lHomeMode 사용시 설정 : 0 - 5 설정 (Move Return후에 Search를  시작한다.)
    // lHomeMode -1로 그대로 사용시 HomeConfig에서 사용한대로 그대로 설정됨.
    // 구동방향      : Vel값이 양수이면 CW, 음수이면 CCW.
    DWORD    __stdcall AxmHomeSetMoveSearch(long lAxisNo, double dVel, double dAccel, double dDecel);

    // 사용자가 지정한 축의 홈 리턴을 시작한다.
    // lHomeMode 사용시 설정 : 0 - 12 설정 
    // lHomeMode -1로 그대로 사용시 HomeConfig에서 사용한대로 그대로 설정됨.
    // 구동방향      : Vel값이 양수이면 CW, 음수이면 CCW.
    DWORD    __stdcall AxmHomeSetMoveReturn(long lAxisNo, double dVel, double dAccel, double dDecel);
    
    // 사용자가 지정한 축의 홈 이탈을 시작한다.
    // 구동방향      : Vel값이 양수이면 CW, 음수이면 CCW.
    DWORD    __stdcall AxmHomeSetMoveLeave(long lAxisNo, double dVel, double dAccel, double dDecel);

    // 사용자가 지정한 다축의 홈 서치을 시작한다.
    // lHomeMode 사용시 설정 : 0 - 5 설정 (Move Return후에 Search를  시작한다.)
    // lHomeMode -1로 그대로 사용시 HomeConfig에서 사용한대로 그대로 설정됨.
    // 구동방향      : Vel값이 양수이면 CW, 음수이면 CCW.
    DWORD    __stdcall AxmHomeSetMultiMoveSearch(long lArraySize, long *lpAxesNo, double *dpVel, double *dpAccel, double *dpDecel);

    //지정된 좌표계의 구동 속도 프로파일 모드를 설정한다.
    // (주의점 : 반드시 축맵핑 하고 사용가능)
    // ProfileMode : '0' - 대칭 Trapezode
    //               '1' - 비대칭 Trapezode
    //               '2' - 대칭 Quasi-S Curve
    //               '3' - 대칭 S Curve
    //               '4' - 비대칭 S Curve
    DWORD    __stdcall AxmContiSetProfileMode(long lCoord, DWORD uProfileMode);
    // 지정된 좌표계의 구동 속도 프로파일 모드를 반환한다.
    DWORD    __stdcall AxmContiGetProfileMode(long lCoord, DWORD *upProfileMode);

    //========== DIO 인터럽트 플래그 레지스트 읽기
    // 지정한 입력 접점 모듈, Interrupt Flag Register의 Offset 위치에서 bit 단위로 인터럽트 발생 상태 값을 읽음
    DWORD    __stdcall AxdiInterruptFlagReadBit(long lModuleNo, long lOffset, DWORD *upValue);
    // 지정한 입력 접점 모듈, Interrupt Flag Register의 Offset 위치에서 byte 단위로 인터럽트 발생 상태 값을 읽음
    DWORD    __stdcall AxdiInterruptFlagReadByte(long lModuleNo, long lOffset, DWORD *upValue);
    // 지정한 입력 접점 모듈, Interrupt Flag Register의 Offset 위치에서 word 단위로 인터럽트 발생 상태 값을 읽음
    DWORD    __stdcall AxdiInterruptFlagReadWord(long lModuleNo, long lOffset, DWORD *upValue);
    // 지정한 입력 접점 모듈, Interrupt Flag Register의 Offset 위치에서 double word 단위로 인터럽트 발생 상태 값을 읽음
    DWORD    __stdcall AxdiInterruptFlagReadDword(long lModuleNo, long lOffset, DWORD *upValue);
    // 전체 입력 접점 모듈, Interrupt Flag Register의 Offset 위치에서 bit 단위로 인터럽트 발생 상태 값을 읽음
    DWORD    __stdcall AxdiInterruptFlagRead(long lOffset, DWORD *upValue);

    //========= 로그 관련 함수 ==========================================================================================
    // 현재 자동으로 설정됨.
    // 설정 축의 함수 실행 결과를 EzSpy에서 모니터링 할 수 있도록 설정 또는 해제하는 함수이다.
    // uUse : 사용 유무 => DISABLE(0), ENABLE(1)
    DWORD    __stdcall AxmLogSetAxis(long lAxisNo, DWORD uUse);
    
    // EzSpy에서의 설정 축 함수 실행 결과 모니터링 여부를 확인하는 함수이다.
    DWORD    __stdcall AxmLogGetAxis(long lAxisNo, DWORD *upUse);

//=========== 로그 출력 관련 함수
    //지정한 입력 채널의 EzSpy에 로그 출력 여부를 설정한다.
    DWORD    __stdcall AxaiLogSetChannel(long lChannelNo, DWORD uUse);
    //지정한 입력 채널의 EzSpy에 로그 출력 여부를 확인한다.
    DWORD    __stdcall AxaiLogGetChannel(long lChannelNo, DWORD *upUse);

//==지정한 출력 채널의 EzSpy 로그 출력 
    //지정한 출력 채널의 EzSpy에 로그 출력 여부를 설정한다.
    DWORD    __stdcall AxaoLogSetChannel(long lChannelNo, DWORD uUse);
    //지정한 출력 채널의 EzSpy에 로그 출력 여부를 확인한다.
    DWORD    __stdcall AxaoLogGetChannel(long lChannelNo, DWORD *upUse);

//==Log
    // 지정한 모듈의 EzSpy에 로그 출력 여부 설정
    DWORD    __stdcall AxdLogSetModule(long lModuleNo, DWORD uUse);
    // 지정한 모듈의 EzSpy에 로그 출력 여부 확인
    DWORD    __stdcall AxdLogGetModule(long lModuleNo, DWORD *upUse);

    // 지정한 보드가 RTEX 모드일 때 그 보드의 firmware 버전을 확인한다.
    DWORD    __stdcall AxlGetFirmwareVersion(long lBoardNo, PTCHAR szVersion);
    // 지정한 보드로 Firmware를 전송 한다.
    DWORD    __stdcall AxlSetFirmwareCopy(long lBoardNo, WORD *wData, WORD *wCmdData);
    // 지정한 보드로 Firmware Update를 수행한다.
    DWORD    __stdcall AxlSetFirmwareUpdate(long lBoardNo);
    // 지정한 보드의 현재 RTEX 초기화 상태를 확인 한다.
    DWORD    __stdcall AxlCheckStatus(long lBoardNo, DWORD* dwStatus);
    // 지정한 축에 RTEX Master board에 범용 명령을 실행 합니다.
    DWORD    __stdcall AxlRtexUniversalCmd(long lBoardNo, WORD wCmd, WORD wOffset, WORD *wData);
    // 지정한 축의 RTEX 통신 명령을 실행한다.
    DWORD    __stdcall AxmRtexSlaveCmd(long lAxisNo, DWORD dwCmdCode, DWORD dwTypeCode, DWORD dwIndexCode, DWORD dwCmdConfigure, DWORD dwValue);
    // 지정한 축에 실행한 RTEX 통신 명령의 결과값을 확인한다.
    DWORD    __stdcall AxmRtexGetSlaveCmdResult(long lAxisNo, DWORD* dwIndex, DWORD *dwValue);
	// 지정한 축에 실행한 RTEX 통신 명령의 결과값을 확인한다. PCIE-Rxx04-RTEX 전용
	DWORD    __stdcall AxmRtexGetSlaveCmdResultEx(long lAxisNo, DWORD* dwpCommand, DWORD* dwpType, DWORD* dwpIndex, DWORD *dwpValue);
    // 지정한 축에 RTEX 상태 정보를 확인한다.
    DWORD   __stdcall AxmRtexGetAxisStatus(long lAxisNo, DWORD *dwStatus);
    // 지정한 축에 RTEX 통신 리턴 정보를 확인한다.(Actual position, Velocity, Torque)
    DWORD   __stdcall AxmRtexGetAxisReturnData(long lAxisNo,  DWORD *dwReturn1, DWORD *dwReturn2, DWORD *dwReturn3);
    // 지정한 축에 RTEX Slave 축의 현재 상태 정보를 확인한다.(mechanical, Inposition and etc)
    DWORD   __stdcall AxmRtexGetAxisSlaveStatus(long lAxisNo,  DWORD *dwStatus);
    
    // 지정한 축에 MLII Slave 축에 범용 네트웍 명령어를 기입한다.
    DWORD   __stdcall AxmSetAxisCmd(long lAxisNo, DWORD* tagCommand);
    // 지정한 축에 MLII Slave 축에 범용 네트웍 명령의 결과를 확인한다.
    DWORD   __stdcall AxmGetAxisCmdResult(long lAxisNo, DWORD* tagCommand);

    // 지정한 SIIIH Slave 모듈에 네트웍 명령의 결과를 기입하고 반환 한다.
    DWORD   __stdcall AxdSetAndGetSlaveCmdResult(long lModuleNo, DWORD* tagSetCommand, DWORD* tagGetCommand);
    DWORD   __stdcall AxaSetAndGetSlaveCmdResult(long lModuleNo, DWORD* tagSetCommand, DWORD* tagGetCommand);
    DWORD   __stdcall AxcSetAndGetSlaveCmdResult(long lModuleNo, DWORD* tagSetCommand, DWORD* tagGetCommand);
    
    // DPRAM 데이터를 확인한다.
    DWORD   __stdcall AxlGetDpRamData(long lBoardNo, WORD wAddress, DWORD* dwpRdData);
    // DPRAM 데이터를 Word단위로 확인한다.
    DWORD   __stdcall AxlBoardReadDpramWord(long lBoardNo, WORD wOffset, DWORD* dwpRdData);
	// DPRAM 데이터를 Word단위로 설정한다.
	DWORD   __stdcall AxlBoardWriteDpramWord(long lBoardNo, WORD wOffset, DWORD dwWrData);

    // 각 보드의 각 SLAVE별로 명령을 전송한다.
    DWORD   __stdcall AxlSetSendBoardEachCommand(long lBoardNo, DWORD dwCommand, DWORD* dwpSendData, DWORD dwLength);
    // 각 보드로 명령을 전송한다.
    DWORD   __stdcall AxlSetSendBoardCommand(long lBoardNo, DWORD dwCommand, DWORD* dwpSendData, DWORD dwLength);
    // 각 보드의 응답을 확인한다.
    DWORD   __stdcall AxlGetResponseBoardCommand(long lBoardNo, DWORD* dwpReadData);

    // Network Type Master 보드에서 Slave 들의 Firmware Version을 읽어 오는 함수.
    // ucaFirmwareVersion unsigned char 형의 Array로 선언하고 크기가 4이상이 되도록 선언 해야 한다.
    DWORD   __stdcall AxmInfoGetFirmwareVersion(long lAxisNo, unsigned char* ucaFirmwareVersion);
    DWORD   __stdcall AxaInfoGetFirmwareVersion(long lModuleNo, unsigned char* ucaFirmwareVersion);
    DWORD   __stdcall AxdInfoGetFirmwareVersion(long lModuleNo, unsigned char* ucaFirmwareVersion);
    DWORD   __stdcall AxcInfoGetFirmwareVersion(long lModuleNo, unsigned char* ucaFirmwareVersion);
    
    //======== PCI-R1604-MLII 전용 함수=========================================================================== 
    // INTERPOLATE and LATCH Command의 Option Field의 Torq Feed Forward의 값을 설정 하도록 합니다.
    // 기본값은 MAX로 설정되어 있습니다.
    // 설정값은 0 ~ 4000H까지 설정 할 수 있습니다.
    // 설정값은 4000H이상으로 설정하면 설정은 그 이상으로 설정되나 동작은 4000H값이 적용 됩니다.
    DWORD   __stdcall AxmSetTorqFeedForward(long lAxisNo, DWORD dwTorqFeedForward);
    
    // INTERPOLATE and LATCH Command의 Option Field의 Torq Feed Forward의 값을 읽어오는 함수 입니다.
    // 기본값은 MAX로 설정되어 있습니다.
    DWORD   __stdcall AxmGetTorqFeedForward(long lAxisNo, DWORD* dwpTorqFeedForward);
    
    // INTERPOLATE and LATCH Command의 VFF Field의 Velocity Feed Forward의 값을 설정 하도록 합니다.
    // 기본값은 '0'로 설정되어 있습니다.
    // 설정값은 0 ~ FFFFH까지 설정 할 수 있습니다.
    DWORD   __stdcall AxmSetVelocityFeedForward(long lAxisNo, DWORD dwVelocityFeedForward);
    
    // INTERPOLATE and LATCH Command의 VFF Field의 Velocity Feed Forward의 값을 읽어오는 함수 입니다.
    DWORD   __stdcall AxmGetVelocityFeedForward(long lAxisNo, DWORD* dwpVelocityFeedForward);

    // Encoder type을 설정한다.
    // 기본값은 0(TYPE_INCREMENTAL)로 설정되어 있습니다.
    // 설정값은 0 ~ 1까지 설정 할 수 있습니다.
    // 설정값 : 0(TYPE_INCREMENTAL), 1(TYPE_ABSOLUTE).
    DWORD   __stdcall AxmSignalSetEncoderType(long lAxisNo, DWORD dwEncoderType);

    // Encoder type을 확인한다.
    DWORD   __stdcall AxmSignalGetEncoderType(long lAxisNo, DWORD* dwpEncoderType);
    //======================================================================================================== 

    // Slave Firmware Update를 위해 추가
    //DWORD   __stdcall AxmSetSendAxisCommand(long lAxisNo, WORD wCommand, WORD* wpSendData, WORD wLength);

    //======== PCI-R1604-RTEX, RTEX-PM 전용 함수============================================================== 
    // 범용 입력 2,3번 입력시 JOG 구동 속도를 설정한다. 
    // 구동에 관련된 모든 설정(Ex, PulseOutMethod, MoveUnitPerPulse 등)들이 완료된 이후 한번만 실행하여야 한다.
    DWORD    __stdcall AxmMotSetUserMotion(long lAxisNo, double dVelocity, double dAccel, double dDecel);

    // 범용 입력 2,3번 입력시 JOG 구동 동작 사용 가부를 설정한다.
    // 설정값 :  0(DISABLE), 1(ENABLE)
    DWORD    __stdcall AxmMotSetUserMotionUsage(long lAxisNo, DWORD dwUsage);

    // MPGP 입력을 사용하여 Load/UnLoad 위치를 자동으로 이동하는 기능 설정. 
    DWORD    __stdcall  AxmMotSetUserPosMotion(long lAxisNo, double dVelocity, double dAccel, double dDecel, double dLoadPos, double dUnLoadPos, DWORD dwFilter, DWORD dwDelay);

    // MPGP 입력을 사용하여 Load/UnLoad 위치를 자동으로 이동하는 기능 설정. 
    // 설정값 :  0(DISABLE), 1(Position 기능 A 사용), 2(Position 기능 B 사용)
    DWORD    __stdcall  AxmMotSetUserPosMotionUsage(long lAxisNo, DWORD dwUsage);
    //======================================================================================================== 

    //======== SIO-CN2CH/HPC4, 절대 위치 트리거 기능 모듈 전용 함수================================================ 
    // 메모리 데이터 쓰기 함수
    DWORD  __stdcall AxcKeWriteRamDataAddr(long lChannelNo, DWORD dwAddr, DWORD dwData);
    // 메모리 데이터 읽기 함수
    DWORD  __stdcall AxcKeReadRamDataAddr(long lChannelNo, DWORD dwAddr, DWORD* dwpData);
    // 메모리 초기화 함수
    DWORD  __stdcall AxcKeResetRamDataAll(long lModuleNo, DWORD dwData);
    // 트리거 타임 아웃 설정 함수
    DWORD  __stdcall AxcTriggerSetTimeout(long lChannelNo, DWORD dwTimeout);
    // 트리거 타임 아웃 확인 함수
    DWORD  __stdcall AxcTriggerGetTimeout(long lChannelNo, DWORD* dwpTimeout);
    // 트리거 대기 상태 확인 함수
    DWORD  __stdcall AxcStatusGetWaitState(long lChannelNo, DWORD* dwpState);
    // 트리거 대기 상태 설정 함수
    DWORD  __stdcall AxcStatusSetWaitState(long lChannelNo, DWORD dwState);

    // 지정 채널에 명령어 기입.
    DWORD  __stdcall AxcKeSetCommandData32(long lChannelNo, DWORD dwCommand, DWORD dwData);
    // 지정 채널에 명령어 기입.
    DWORD  __stdcall AxcKeSetCommandData16(long lChannelNo, DWORD dwCommand, WORD wData);
    // 지정 채널의 레지스터 확인.
    DWORD  __stdcall AxcKeGetCommandData32(long lChannelNo, DWORD dwCommand, DWORD *dwpData);
    // 지정 채널의 레지스터 확인.
    DWORD  __stdcall AxcKeGetCommandData16(long lChannelNo, DWORD dwCommand, WORD *wpData);
    //======================================================================================================== 

    //======== PCI-N804/N404 전용, Sequence Motion ===================================================================
    // Sequence Motion의 축 정보를 설정 합니다. (최소 1축)
    // lSeqMapNo : 축 번호 정보를 담는 Sequence Motion Index Point
    // lSeqMapSize : 축 번호 갯수
    // long* LSeqAxesNo : 축 번호 배열
    DWORD   __stdcall AxmSeqSetAxisMap(long lSeqMapNo, long lSeqMapSize, long* lSeqAxesNo);
    DWORD   __stdcall AxmSeqGetAxisMap(long lSeqMapNo, long* lSeqMapSize, long* lSeqAxesNo);

    // Sequence Motion의 기준(Master) 축을 설정 합니다. 
    // 반드시 AxmSeqSetAxisMap(...) 에 설정된 축 내에서 설정하여야 합니다.
    DWORD   __stdcall AxmSeqSetMasterAxisNo(long lSeqMapNo, long lMasterAxisNo);

    // Sequence Motion의 Node 적재 시작을 라이브러리에 알립니다.
    DWORD   __stdcall AxmSeqBeginNode(long lSeqMapNo);

    // Sequence Motion의 Node 적재 종료를 라이브러리에 알립니다.
    DWORD   __stdcall AxmSeqEndNode(long lSeqMapNo);

    // Sequence Motion의 구동을 시작 합니다.
    DWORD   __stdcall AxmSeqStart(long lSeqMapNo, DWORD dwStartOption);

    // Sequence Motion의 각 Profile Node 정보를 라이브러리에 입력 합니다.
    // 만약 1축 Sequence Motion을 사용하더라도, *dPosition는 1개의 Array로 지정하여 주시기 바랍니다.
    DWORD   __stdcall AxmSeqAddNode(long lSeqMapNo, double* dPosition, double dVelocity, double dAcceleration, double dDeceleration, double dNextVelocity);

    // Sequence Motion이 구동 시 현재 실행 중인 Node Index를 알려 줍니다.
    DWORD   __stdcall AxmSeqGetNodeNum(long lSeqMapNo, long* lCurNodeNo);

    // Sequence Motion의 총 Node Count를 확인 합니다.
    DWORD   __stdcall AxmSeqGetTotalNodeNum(long lSeqMapNo, long* lTotalNodeCnt);

    // Sequence Motion이 현재 구동 중인지 확인 합니다.
    // dwInMotion : 0(구동 종료), 1(구동 중)
    DWORD   __stdcall AxmSeqIsMotion(long lSeqMapNo, DWORD* dwInMotion);

    // Sequence Motion의 Memory를 Clear 합니다.
    // AxmSeqSetAxisMap(...), AxmSeqSetMasterAxisNo(...) 에서 설정된 값은 유지됩니다.
    DWORD   __stdcall AxmSeqWriteClear(long lSeqMapNo);

    // Sequence Motion의 구동을 종료 합니다.
    // dwStopMode : 0(EMERGENCY_STOP), 1(SLOWDOWN_STOP) 
    DWORD   __stdcall AxmSeqStop(long lSeqMapNo, DWORD dwStopMode);
    //======================================================================================================== 


	//======== PCIe-Rxx04-SIIIH 전용 함수==========================================================================
	// (SIIIH, MR_J4_xxB, Para : 0 ~ 8) ==
	//     [0] : Command Position
	//     [1] : Actual Position
	//     [2] : Actual Velocity
	//     [3] : Mechanical Signal
	//     [4] : Regeneration load factor(%)
	//     [5] : Effective load factor(%)
	//     [6] : Peak load factor(%)
	//     [7] : Current Feedback
	//     [8] : Command Velocity
	DWORD	__stdcall AxmStatusSetMon(long lAxisNo, DWORD dwParaNo1, DWORD dwParaNo2, DWORD dwParaNo3, DWORD dwParaNo4, DWORD dwUse);
	DWORD	__stdcall AxmStatusGetMon(long lAxisNo, DWORD *dwpParaNo1, DWORD *dwpParaNo2, DWORD *dwpParaNo3, DWORD *dwpParaNo4, DWORD *dwpUse);
	DWORD	__stdcall AxmStatusReadMon(long lAxisNo, DWORD *dwpParaNo1, DWORD *dwpParaNo2, DWORD *dwpParaNo3, DWORD *dwpParaNo4, DWORD *dwDataVaild);
	DWORD	__stdcall AxmStatusReadMonEx(long lAxisNo, long *lpDataCnt, DWORD *dwpReadData);
	//=============================================================================================================
		
    //======== PCI-R32IOEV-RTEX 전용 함수=========================================================================== 
    // I/O 포트로 할당된 HPI register 를 읽고 쓰기위한 API 함수.
    // I/O Registers for HOST interface.
    // I/O 00h Host status register (HSR)
    // I/O 04h Host-to-DSP control register (HDCR)
    // I/O 08h DSP page register (DSPP)
    // I/O 0Ch Reserved
    DWORD   __stdcall AxlSetIoPort(long lBoardNo, DWORD dwAddr, DWORD dwData);
    DWORD   __stdcall AxlGetIoPort(long lBoardNo, DWORD dwAddr, DWORD* dwpData);

    //======== PCI-R3200-MLIII 전용 함수=========================================================================== 
/*
    // M-III Master 보드 펌웨어 업데이트 기본 정보 설정 함수
    DWORD   __stdcall AxlM3SetFWUpdateInit(long lBoardNo, DWORD dwTotalPacketSize);
    // M-III Master 보드 펌웨어 업데이트 기본 정보 설정 결과 확인 함수
    DWORD   __stdcall AxlM3GetFWUpdateInit(long lBoardNo, DWORD *dwTotalPacketSize);
    // M-III Master 보드 펌웨어 업데이트 자료 전달 함수
    DWORD   __stdcall AxlM3SetFWUpdateCopy(long lBoardNo, DWORD *lFWUpdataData, DWORD dwLength);
    // M-III Master 보드 펌웨어 업데이트 자료 전달 결과 확인 함수
    DWORD   __stdcall AxlM3GetFWUpdateCopy(long lBoardNo, BYTE bCrcData, DWORD *lFWUpdataResult);
    // M-III Master 보드 펌웨어 업데이트 실행
    DWORD   __stdcall AxlM3SetFWUpdate(long lBoardNo, DWORD dwSectorNo);
    // M-III Master 보드 펌웨어 업데이트 실행 결과 확인
    DWORD   __stdcall AxlM3GetFWUpdate(long lBoardNo, DWORD *dwSectorNo, DWORD *dwIsDone);
*/
    // M-III Master 보드 펌웨어 업데이트 기본 정보 설정 함수
    DWORD   __stdcall AxlM3SetFWUpdateInit(long lBoardNo, DWORD dwTotalPacketSize, DWORD dwProcTotalStepNo);
    // M-III Master 보드 펌웨어 업데이트 기본 정보 설정 결과 확인 함수
    DWORD   __stdcall AxlM3GetFWUpdateInit(long lBoardNo, DWORD *dwTotalPacketSize, DWORD *dwProcTotalStepNo);

    // M-III Master 보드 펌웨어 업데이트 자료 전달 함수
    DWORD   __stdcall AxlM3SetFWUpdateCopy(long lBoardNo, DWORD *pdwPacketData, DWORD dwPacketSize);
    // M-III Master 보드 펌웨어 업데이트 자료 전달 결과 확인 함수
    DWORD   __stdcall AxlM3GetFWUpdateCopy(long lBoardNo, DWORD *dwPacketSize);

    // M-III Master 보드 펌웨어 업데이트 실행
    DWORD   __stdcall AxlM3SetFWUpdate(long lBoardNo, DWORD dwFlashBurnStepNo);
    // M-III Master 보드 펌웨어 업데이트 실행 결과 확인
    DWORD   __stdcall AxlM3GetFWUpdate(long lBoardNo, DWORD *dwFlashBurnStepNo, DWORD *dwIsFlashBurnDone);

    // M-III Master 보드 EEPROM 데이터 설정 함수
    DWORD   __stdcall AxlM3SetCFGData(long lBoardNo, DWORD *pCmdData, DWORD CmdDataSize);
    // M-III Master 보드 EEPROM 데이터 가져오기 함수
    DWORD   __stdcall AxlM3GetCFGData(long lBoardNo, DWORD *pCmdData, DWORD CmdDataSize);

    // M-III Master 보드 CONNECT PARAMETER 기본 정보 설정 함수
    DWORD   __stdcall AxlM3SetMCParaUpdateInit(long lBoardNo, WORD wCh0Slaves, WORD wCh1Slaves, DWORD dwCh0CycTime, DWORD dwCh1CycTime, DWORD dwChInfoMaxRetry);
    // M-III Master 보드 CONNECT PARAMETER 기본 정보 설정 결과 확인 함수
    DWORD   __stdcall AxlM3GetMCParaUpdateInit(long lBoardNo, WORD *wCh0Slaves, WORD *wCh1Slaves, DWORD *dwCh0CycTime, DWORD *dwCh1CycTime, DWORD *dwChInfoMaxRetry);
    // M-III Master 보드 CONNECT PARAMETER 기본 정보 전달 함수
    DWORD   __stdcall AxlM3SetMCParaUpdateCopy(long lBoardNo, WORD wIdx, WORD wChannel, WORD wSlaveAddr, DWORD dwProtoCalType, DWORD dwTransBytes, DWORD dwDeviceCode);
    // M-III Master 보드 CONNECT PARAMETER 기본 정보 전달 결과 확인 함수
    DWORD   __stdcall AxlM3GetMCParaUpdateCopy(long lBoardNo, WORD wIdx, WORD *wChannel, WORD *wSlaveAddr, DWORD *dwProtoCalType, DWORD *dwTransBytes, DWORD *dwDeviceCode);

    // M-III Master 보드내에 레지스터를 DWord단위로 확인 함수
    DWORD   __stdcall AxlBoardReadDWord(long lBoardNo, WORD wOffset, DWORD *dwData);
    // M-III Master 보드내에 레지스터를 DWord단위로 설정 함수
    DWORD   __stdcall AxlBoardWriteDWord(long lBoardNo, WORD wOffset, DWORD dwData);

    // 보드내에 확장 레지스터를 DWord단위로 설정 및 확인한다.
    DWORD    __stdcall AxlBoardReadDWordEx(long lBoardNo, DWORD dwOffset, DWORD *dwData);
    DWORD    __stdcall AxlBoardWriteDWordEx(long lBoardNo, DWORD dwOffset, DWORD dwData);

    // 서보를 정지 모드로 설정 함수
    DWORD   __stdcall AxmM3ServoSetCtrlStopMode(long lAxisNo, BYTE bStopMode);
    // 서보를 Lt 선택 상태로 설정 함수
    DWORD   __stdcall AxmM3ServoSetCtrlLtSel(long lAxisNo, BYTE bLtSel1, BYTE bLtSel2);
    // 서보의 IO 입력 상태를 확인 함수
    DWORD   __stdcall AxmStatusReadServoCmdIOInput(long lAxisNo, DWORD *upStatus);    
    // 서보의 보간 구동 함수
    DWORD   __stdcall AxmM3ServoExInterpolate(long lAxisNo, DWORD dwTPOS, DWORD dwVFF, DWORD dwTFF, DWORD dwTLIM, DWORD dwExSig1, DWORD dwExSig2);
    // 서보 엑츄레이터 바이어스 설정 함수
    DWORD   __stdcall AxmM3ServoSetExpoAccBias(long lAxisNo, WORD wBias);
    // 서보 엑츄레이터 시간 설정 함수
    DWORD   __stdcall AxmM3ServoSetExpoAccTime(long lAxisNo, WORD wTime);
    // 서보의 이동 시간을 설정 함수
    DWORD   __stdcall AxmM3ServoSetMoveAvrTime(long lAxisNo, WORD wTime);
    // 서보의 Acc 필터 설정 함수
    DWORD   __stdcall AxmM3ServoSetAccFilter(long lAxisNo, BYTE bAccFil);
    // 서보의 상태 모니터1 설정 함수
    DWORD   __stdcall AxmM3ServoSetCprmMonitor1(long lAxisNo, BYTE bMonSel);
    // 서보의 상태 모니터2 설정 함수
    DWORD   __stdcall AxmM3ServoSetCprmMonitor2(long lAxisNo, BYTE bMonSel);
    // 서보의 상태 모니터1 확인 함수
    DWORD   __stdcall AxmM3ServoStatusReadCprmMonitor1(long lAxisNo, DWORD *upStatus);
    // 서보의 상태 모니터2 확인 함수
    DWORD   __stdcall AxmM3ServoStatusReadCprmMonitor2(long lAxisNo, DWORD *upStatus);
    // 서보 엑츄레이터 Dec 설정 함수
    DWORD   __stdcall AxmM3ServoSetAccDec(long lAxisNo, WORD wAcc1, WORD wAcc2, WORD wAccSW, WORD wDec1, WORD wDec2, WORD wDecSW);
    // 서보 정지 설정 함수
    DWORD   __stdcall AxmM3ServoSetStop(long lAxisNo, long lMaxDecel);

//========== 표준 I/O 기기 공통 커맨드 =========================================================================
    // Network제품 각 슬레이브 기기의 파라미터 설정 값을 반환하는 함수
    DWORD   __stdcall AxlM3GetStationParameter(long lBoardNo, long lModuleNo, WORD wNo, BYTE bSize, BYTE bModuleType, BYTE *pbParam);
    // Network제품 각 슬레이브 기기의 파라미터 값을 설정하는 함수
    DWORD   __stdcall AxlM3SetStationParameter(long lBoardNo, long lModuleNo, WORD wNo, BYTE bSize, BYTE bModuleType, BYTE *pbParam);
    // Network제품 각 슬레이브 기기의 ID값을 반환하는 함수
    DWORD   __stdcall AxlM3GetStationIdRd(long lBoardNo, long lModuleNo, BYTE bIdCode, BYTE bOffset, BYTE bSize, BYTE bModuleType, BYTE *pbParam);
    // Network제품 각 슬레이브 기기의 무효 커맨드로 사용하는 함수
    DWORD   __stdcall AxlM3SetStationNop(long lBoardNo, long lModuleNo, BYTE bModuleType);
    // Network제품 각 슬레이브 기기의 셋업을 실시하는 함수
    DWORD   __stdcall AxlM3SetStationConfig(long lBoardNo, long lModuleNo, BYTE bConfigMode, BYTE bModuleType);
    // Network제품 각 슬레이브 기기의 알람 및 경고 상태 값을 반환하는 함수
    DWORD   __stdcall AxlM3GetStationAlarm(long lBoardNo, long lModuleNo, WORD wAlarmRdMod, WORD wAlarmIndex, BYTE bModuleType, WORD *pwAlarmData);
    // Network제품 각 슬레이브 기기의 알람 및 경고 상태를 해제하는 함수
    DWORD   __stdcall AxlM3SetStationAlarmClear(long lBoardNo, long lModuleNo, WORD wAlarmClrMod, BYTE bModuleType);
    // Network제품 각 슬레이브 기기와의 동기통신을 설정하는 함수
    DWORD   __stdcall AxlM3SetStationSyncSet(long lBoardNo, long lModuleNo, BYTE bModuleType);
    // Network제품 각 슬레이브 기기와의 연결을 설정하는 함수
    DWORD   __stdcall AxlM3SetStationConnect(long lBoardNo, long lModuleNo, BYTE bVer, BYTE bComMode, BYTE bComTime, BYTE bProfileType, BYTE bModuleType);
    // Network제품 각 슬레이브 기기와의 연결 끊음을 설정하는 함수
    DWORD   __stdcall AxlM3SetStationDisConnect(long lBoardNo, long lModuleNo, BYTE bModuleType);
    // Network제품 각 슬레이브 기기의 비휘발성 파라미터 설정 값을 반환하는 함수
    DWORD   __stdcall AxlM3GetStationStoredParameter(long lBoardNo, long lModuleNo, WORD wNo, BYTE bSize, BYTE bModuleType, BYTE *pbParam);
    // Network제품 각 슬레이브 기기의 비휘발성 파라미터 값을 설정하는 함수
    DWORD   __stdcall AxlM3SetStationStoredParameter(long lBoardNo, long lModuleNo, WORD wNo, BYTE bSize, BYTE bModuleType, BYTE *pbParam);
    // Network제품 각 슬레이브 기기의 메모리 설정 값을 반환하는 함수
    DWORD   __stdcall AxlM3GetStationMemory(long lBoardNo, long lModuleNo, WORD wSize, DWORD dwAddress, BYTE bModuleType, BYTE bMode, BYTE bDataType, BYTE *pbData);
    // Network제품 각 슬레이브 기기의 메모리 값을 설정하는 함수
    DWORD   __stdcall AxlM3SetStationMemory(long lBoardNo, long lModuleNo, WORD wSize, DWORD dwAddress, BYTE bModuleType, BYTE bMode, BYTE bDataType, BYTE *pbData);

//========== 표준 I/O 기기 커넥션 커맨드 =========================================================================
    // Network제품 각 재정열된 슬레이브 기기의 자동 억세스 모드 값을 설정하는 함수
    DWORD   __stdcall AxlM3SetStationAccessMode(long lBoardNo, long lModuleNo, BYTE bModuleType, BYTE bRWSMode);
    // Network제품 각 재정열된 슬레이브 기기의 자동 억세스 모드 설정값을 반환하는 함수
    DWORD   __stdcall AxlM3GetStationAccessMode(long lBoardNo, long lModuleNo, BYTE bModuleType, BYTE *bRWSMode);
    // Network제품 각 슬레이브 기기의 동기 자동 연결 모드를 설정하는 함수
    DWORD   __stdcall AxlM3SetAutoSyncConnectMode(long lBoardNo, long lModuleNo, BYTE bModuleType, DWORD dwAutoSyncConnectMode);
    // Network제품 각 슬레이브 기기의 동기 자동 연결 모드 설정값을 반환하는 함수
    DWORD   __stdcall AxlM3GetAutoSyncConnectMode(long lBoardNo, long lModuleNo, BYTE bModuleType, DWORD *dwpAutoSyncConnectMode);
    // Network제품 각 슬레이브 기기에 대한 단일 동기화 연결을 설정하는 함수
    DWORD   __stdcall AxlM3SyncConnectSingle(long lBoardNo, long lModuleNo, BYTE bModuleType);
    // Network제품 각 슬레이브 기기에 대한 단일 동기화 연결 끊음을 설정하는 함수
    DWORD   __stdcall AxlM3SyncDisconnectSingle(long lBoardNo, long lModuleNo, BYTE bModuleType);
    // Network제품 각 슬레이브 기기와의 연결 상태를 확인하는 함수
    DWORD   __stdcall AxlM3IsOnLine(long lBoardNo, long lModuleNo, DWORD *dwData);

//========== 표준 I/O 프로파일 커맨드 =========================================================================
    // Network제품 각 동기화 상태의 슬레이브 I/O 기기에 대한 데이터 설정값을 반환하는 함수
    DWORD   __stdcall AxlM3GetStationRWS(long lBoardNo, long lModuleNo, BYTE bModuleType, DWORD *pdwParam, BYTE bSize);
    // Network제품 각 동기화 상태의 슬레이브 I/O 기기에 대한 데이터값을 설정하는 함수
    DWORD   __stdcall AxlM3SetStationRWS(long lBoardNo, long lModuleNo, BYTE bModuleType, DWORD *pdwParam, BYTE bSize);
    // Network제품 각 비동기화 상태의 슬레이브 I/O 기기에 대한 데이터 설정값을 반환하는 함수
    DWORD   __stdcall AxlM3GetStationRWA(long lBoardNo, long lModuleNo, BYTE bModuleType, DWORD *pdwParam, BYTE bSize);
    // Network제품 각 비동기화 상태의 슬레이브 I/O 기기에 대한 데이터값을 설정하는 함수
    DWORD   __stdcall AxlM3SetStationRWA(long lBoardNo, long lModuleNo, BYTE bModuleType, DWORD *pdwParam, BYTE bSize);

    // MLIII adjustment operation을 설정 한다.
    // dwReqCode == 0x1005 : parameter initialization : 20sec
    // dwReqCode == 0x1008 : absolute encoder reset   : 5sec
    // dwReqCode == 0x100E : automatic offset adjustment of motor current detection signals  : 5sec
    // dwReqCode == 0x1013 : Multiturn limit setting  : 5sec
    DWORD   __stdcall AxmM3AdjustmentOperation(long lAxisNo, DWORD dwReqCode);

    // M3 전용 원점 검색 진행 상태 진단용 함수이다.
    DWORD    __stdcall AxmHomeGetM3FWRealRate(long lAxisNo, DWORD *upHomeMainStepNumber, DWORD *upHomeSubStepNumber, DWORD *upHomeLastMainStepNumber, DWORD *upHomeLastSubStepNumber);
	// M3 전용 원점 검색시 센서존에서 탈출시 보정되는 위치 값을 반환하는 함수이다.
    DWORD   __stdcall AxmHomeGetM3OffsetAvoideSenArea(long lAxisNo, double *dPos);
    // M3 전용 원점 검색시 센서존에서 탈출시 보정되는 위치 값을 설정하는 함수이다.
	// dPos 설정 값이 0이면 자동으로 탈출시 보정되는 위치 값은 자동으로 설정된다.
	// dPos 설정 값은 양수의 값만 입력한다.
	DWORD   __stdcall AxmHomeSetM3OffsetAvoideSenArea(long lAxisNo, double dPos);

	// M3 전용, 절대치 엔코더 사용 기준, 원점검색 완료 후 CMD/ACT POS 초기화 여부 설정
	// dwSel: 0, 원점 검색후 CMD/ACTPOS 0으로 설정됨.[초기값]
	// dwSel: 1, 원점 검색후 CMD/ACTPOS 값이 설정되지 않음.
	DWORD   __stdcall AxmM3SetAbsEncOrgResetDisable(long lAxisNo, DWORD dwSel);

	// M3 전용, 절대치 엔코더 사용 기준, 원점검색 완료 후 CMD/ACT POS 초기화 여부 설정값 가져오기
	// upSel: 0, 원점 검색후 CMD/ACTPOS 0으로 설정됨.[초기값]
	// upSel: 1, 원점 검색후 CMD/ACTPOS 값이 설정되지 않음.
	DWORD   __stdcall AxmM3GetAbsEncOrgResetDisable(long lAxisNo, DWORD *upSel);

	// M3 전용, 슬레이브 OFFLINE 전환시 알람 유지 기능 사용 유무 설정
	// dwSel: 0, ML3 슬레이브 ONLINE->OFFLINE 알람 처리 사용하지 않음.[초기값]
	// dwSel: 1, ML3 슬레이브 ONLINE->OFFLINE 알람 처리 사용

	DWORD   __stdcall AxmM3SetOfflineAlarmEnable(long lAxisNo, DWORD dwSel);
	// M3 전용, 슬레이브 OFFLINE 전환시 알람 유지 기능 사용 유무 설정 값 가져오기
	// upSel: 0, ML3 슬레이브 ONLINE->OFFLINE 알람 처리 사용하지 않음.[초기값]
	// upSel: 1, ML3 슬레이브 ONLINE->OFFLINE 알람 처리 사용

	DWORD   __stdcall AxmM3GetOfflineAlarmEnable(long lAxisNo, DWORD *upSel);

	// M3 전용, 슬레이브 OFFLINE 전환 여부 상태 값 가져오기
	// upSel: 0, ML3 슬레이브 ONLINE->OFFLINE 전환되지 않음
	// upSel: 1, ML3 슬레이브 ONLINE->OFFLINE 전환되었음.
	DWORD   __stdcall AxmM3ReadOnlineToOfflineStatus(long lAxisNo, DWORD *upStatus);

    // Network 제품의 Configuration Lock 상태를 설정한다.
    // wLockMode  : DISABLE(0), ENABLE(1)
    DWORD   __stdcall AxlSetLockMode(long lBoardNo, WORD wLockMode);

    // Lock 정보를 설정
    DWORD   __stdcall AxlSetLockData(long lBoardNo, DWORD dwTotalNodeNum, DWORD *dwpNodeNo, DWORD *dwpNodeID, DWORD *dwpLockData);

	DWORD   __stdcall AxmMoveStartPosWithAVC(long lAxisNo, double dPosition, double dMaxVelocity, double dMaxAccel, double dMinJerk, double *dpMoveVelocity, double *dpMoveAccel, double *dpMoveJerk);
    // 카운터 모듈의 2-D 절대위치 트리거 기능을 위해 필요한 트리거 위치 정보를 설정한다.
    // lChannelNo : 0,1 channel 일 경우 0, 2,3 channel 의 경우 2 를 설정. 
    // nDataCnt : 
    //  nDataCnt > 0 : 데이터 등록, nDataCnt <= 0 : 등록된 데이터 초기화.
    // dwOption : Reserved.
    // dpPatternData : (X1, Y1)
    DWORD  __stdcall AxcTriggerSetPatternData(long lChannelNo, long nDataCnt,  DWORD dwOption, double* dpPatternData);
    // 카운터 모듈의 2-D 절대위치 트리거 기능을 위해 필요한 트리거 위치 정보를 확인한다.
    DWORD  __stdcall AxcTriggerGetPatternData(long lChannelNo, long* npDataCnt,  DWORD* dwpOption, double* dpPatternData);

	//연속 보간 관련하여 AxmContiEndNode 함수내에서 보간구동관련 Node 를 Data Queue 에 미리 채워넣을 수 있도록하는 기능을 활성화 한다.
	//bPushPrevContiQueue : 1 // 해당 기능 활성화 
	//bPushPrevContiQueue : 0 // 해당 기능 비활성화 	
	DWORD __stdcall AxmContiSetPushPrevContiQueueEnable(long lCoordinate,BOOL bPushPrevContiQueue);
	//설정해둔 AxmContiSetPushPrevContiQueueEnable Flag값을 반환한다.	
	DWORD __stdcall AxmContiGetPushPrevContiQueueEnable(long lCoordinate,BOOL *bPushPrevContiQueue);

	// 연속보간 구동 시 Data Queue 에 Node 정보가 적재되었는지 상태를 반환한다.
	// AxmContiSetPushPrevContiQueueEnable(long lCoordinate, 1) 로 설정되어있을 경우만 유효
	// bPushPrevContiQueueComplete : 1 // Node Data 적재 완료
	// bPushPrevContiQueueComplete : 0 // Node Data 적재 되어있지않음
	DWORD __stdcall AxmContiGetPushPrevContiQueueComplete(long lCoordinate, BOOL * bPushPrevContiQueueComplete);
	
    // 연속보간 구동 시 첫 노드 시작 및 마지막 노드 시작 시 일정시간 이후 지정된 좌표계의 마스터 축의 OutputBit On/Off 제어
	// AxmContiBeginNode 앞에 호출해야 한다. 한번 구동하면 Flag가 초기화되어 다시 호출해야 사용할 수 있다.
    // StartTime/EndTime 단위는 [Sec]이며, 0 ~ 6.5초까지 설정 가능하다.
    // uOnoff	: 0 - 시작 위치에서 Bit On 종료 위치에서 Bit Off
    //          : 1 - 시작 위치에서 Bit Off 종료 위치에서 Bit On
    // lEndMode : 0 - 마지막 노드 구동 종료 후 즉시 OutputBit Off/On
    //			: 1 - 마지막 노드 구동 시작 후 입력한 EndTime 이후 OutputBit Off/On
    //			: 2 - 구동 시작 시 OutputBit On/Off 및 입력한 EndTime 이후 OutputBit Off/On
	DWORD __stdcall AxmContiSetWriteOutputBit(long lCoordinate, double dStartTime, double dEndTime, long lBitNo, long uOnoff, long lEndMode);

	// AxmContiSetWriteOutputBit로 설정한 값들을 반환한다.
	DWORD __stdcall	AxmContiGetWriteOutputBit(long lCoordinate, double* dpStartTime, double* dpEndTime, long* lpBitNo, long* lpOnoff, long* lpEndMode);

	// AxmContiSetWriteOutputBit로 설정한 값들을 리셋한다.
	DWORD __stdcall	AxmContiResetWriteOutputBit(long lCoordinate);

	// AxmMoveTorqueStop 함수로 토크 구동 정지 시 CmdPos 값을 ActPos 값과 일치시키는 시점까지의 대기 시간을 설정한다.
	// dwSettlingTime
	//  1) 단위: [msec]
	//  2) 입력 가능 범위: 0 ~ 10000
	//  *참고* AxmMoveTorqueSetStopSettlingTime 함수로 대기 시간을 설정하지 않으면, dafault 값인 10[msec]가 적용된다.
	DWORD __stdcall AxmMoveTorqueSetStopSettlingTime(long lAxisNo, DWORD dwSettlingTime);
	// AxmMoveTorqueStop 함수로 토크 구동 정지 시 CmdPos 값을 ActPos 값과 일치시키는 시점까지의 대기 시간을 반환한다.
	DWORD __stdcall AxmMoveTorqueGetStopSettlingTime(long lAxisNo, DWORD* dwpSettlingTime);

	//////////////////////////////////////////////////////////////////////////
	// Monitor  
	// 데이터를 수집을 진행할 항목을 추가합니다.
	DWORD  __stdcall AxlMonitorSetItem(long lBoardNo, long lItemIndex, DWORD dwSignalType, long lSignalNo, long lSubSignalNo);

	// 데이터 수집을 진행할 항목들에 관한 정보를 가져옵니다.
	DWORD  __stdcall AxlMonitorGetIndexInfo(long lBoardNo, long* lpItemSize, long* lpItemIndex);

	// 데이터 수집을 진행할 각 항목의 세부 설정을 가져옵니다.
	DWORD  __stdcall AxlMonitorGetItemInfo(long lBoardNo, long lItemIndex, DWORD* dwpSignalType, long* lpSignalNo, long* lpSubSignalNo);

	// 모든 데이터 수집 항목의 설정을 초기화합니다.
	DWORD  __stdcall AxlMonitorResetAllItem(long lBoardNo);

	// 선택된 데이터 수집 항목의 설정을 초기화합니다.
	DWORD  __stdcall AxlMonitorResetItem(long lBoardNo, long lItemIndex);

	// 데이터 수집의 트리거 조건을 설정합니다.
	DWORD  __stdcall AxlMonitorSetTriggerOption(long lBoardNo, DWORD dwSignalType, long lSignalNo, long lSubSignalNo, DWORD dwOperatorType, double dValue1, double dValue2);

	// 데이터 수집의 트리거 조건을 가져옵니다.
	//DWORD  __stdcall AxlMonitorGetTriggerOption(DWORD* dwpSignalType, long* lpSignalNo, long* lpSubSignalNo, DWORD* dwpOperatorType, double* dpValue1, double* dpValue2);

	// 데이터 수집의 트리거 조건을 초기화합니다.
	DWORD  __stdcall AxlMonitorResetTriggerOption(long lBoardNo);

	// 데이터 수집을 시작합니다.
	DWORD  __stdcall AxlMonitorStart(long lBoardNo, DWORD dwStartOption, DWORD dwOverflowOption);

	// 데이터 수집을 정지합니다.
	DWORD  __stdcall AxlMonitorStop(long lBoardNo);

	// 수집된 데이터를 가져옵니다.
	DWORD  __stdcall AxlMonitorReadData(long lBoardNo, long* lpItemSize, long* lpDataCount, double* dpReadData);

	// 데이터 수집의 주기를 가져옵니다.
	DWORD  __stdcall AxlMonitorReadPeriod(long lBoardNo, DWORD* dwpPeriod);
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// MonitorEx  
	// 데이터를 수집을 진행할 항목을 추가합니다.
	DWORD  __stdcall AxlMonitorExSetItem(long lItemIndex, DWORD dwSignalType, long lSignalNo, long lSubSignalNo);

	// 데이터 수집을 진행할 항목들에 관한 정보를 가져옵니다.
	DWORD  __stdcall AxlMonitorExGetIndexInfo(long* lpItemSize, long* lpItemIndex);

	// 데이터 수집을 진행할 각 항목의 세부 설정을 가져옵니다.
	DWORD  __stdcall AxlMonitorExGetItemInfo(long lItemIndex, DWORD* dwpSignalType, long* lpSignalNo, long* lpSubSignalNo);

	// 모든 데이터 수집 항목의 설정을 초기화합니다.
	DWORD  __stdcall AxlMonitorExResetAllItem();

	// 선택된 데이터 수집 항목의 설정을 초기화합니다.
	DWORD  __stdcall AxlMonitorExResetItem(long lItemIndex);

	// 데이터 수집의 트리거 조건을 설정합니다.
	DWORD  __stdcall AxlMonitorExSetTriggerOption(DWORD dwSignalType, long lSignalNo, long lSubSignalNo, DWORD dwOperatorType, double dValue1, double dValue2);

	// 데이터 수집의 트리거 조건을 가져옵니다.
	//DWORD  __stdcall AxlMonitorExGetTriggerOption(DWORD* dwpSignalType, long* lpSignalNo, long* lpSubSignalNo, DWORD* dwpOperatorType, double* dpValue1, double* dpValue2);

	// 데이터 수집의 트리거 조건을 초기화합니다.
	DWORD  __stdcall AxlMonitorExResetTriggerOption();

	// 데이터 수집을 시작합니다.
	DWORD  __stdcall AxlMonitorExStart(DWORD dwStartOption, DWORD dwOverflowOption);

	// 데이터 수집을 정지합니다.
	DWORD  __stdcall AxlMonitorExStop();

	// 수집된 데이터를 가져옵니다.
	DWORD  __stdcall AxlMonitorExReadData(long* lpItemSize, long* lpDataCount, double* dpReadData);

	// 데이터 수집의 주기를 가져옵니다.
	DWORD  __stdcall AxlMonitorExReadPeriod(DWORD* dwpPeriod);
	//////////////////////////////////////////////////////////////////////////

	// X2, Y2 축에 대한 Offset 위치 정보를 포함한 2축 직선 보간 #01.
	DWORD  __stdcall AxmLineMoveDual01(long lCoordNo, double* dpEndPosition, double dVelocity, double dAccel, double dDecel, double dOffsetLength, double dTotalLength, double* dpStartOffsetPosition, double* dpEndOffsetPosition);
	// X2, Y2 축에 대한 Offset 위치 정보를 포함한 2축 원호 보간 #01.
	DWORD  __stdcall AxmCircleCenterMoveDual01(long lCoordNo, long* lpAxes, double* dpCenterPosition, double* dpEndPosition, double dVelocity, double dAccel, double dDecel, DWORD dwCWDir, double dOffsetLength, double dTotalLength, double* dpStartOffsetPosition, double* dpEndOffsetPosition);

	// 해당보드의 connect mode 를 반환한다.
	// dpMode : 1 Auto Connect Mode
	// dpMode : 0 Manual Connect Mode
	DWORD	__stdcall AxlGetBoardConnectMode(long lBoardNo, DWORD *dwpMode);
	// 해당보드의 connect mode 를 설정한다.
	// dMode : 1 Auto Connect Mode
	// dMode : 0 Manual Connect Mode	
	DWORD	__stdcall AxlSetBoardConnectMode(long lBoardNo, DWORD dwMode);
	
	//지정된 축의 Command Queue 를 초기화 한다.
	DWORD	__stdcall AxmStatusSetCmdQueueClear(long lAxisNo);
	
	// 지정된 축의 통신 프로토콜관련 Data 를 확인한다.	
	DWORD	__stdcall AxmStatusGetControlBits(long lAxisNo, DWORD* dwpTxData, DWORD* dwpRxData);

	// 사용 중인 AXL이 있는지 확인(Shared Memory가 존재하는지 확인)
	BOOL	__stdcall AxlIsUsing();
	DWORD	__stdcall AxlRescanExternalDevice();
	DWORD	__stdcall AxlGetExternalDeviceInfo(long lBoardNo, void* devInfo);  //void* -> ExternalDeviceInfo*

	// EzConfig가 제공하는 확장 기능 사용을 위한 함수
	// (PCIe-Rxx05-MLIII, dwFunction: 0) ==
	//		[0]: ActOffset 저장 기능 활성화 여부(dwValue: [0] Disable, [1] Enable)
	DWORD	__stdcall AxlSetExFunction(DWORD dwFunction, DWORD dwValue);
	DWORD	__stdcall AxlGetExFunction(DWORD dwFunction, DWORD *dwpValue);

#ifdef __cplusplus
}
#endif    //__cplusplus

#endif    //__AXT_AXDEV_H__
