#ifndef __MVS_ENC_H__
#define __MVS_ENC_H__

#ifndef HANDLE
typedef void *HANDLE;
#endif

#ifndef USHORT
typedef unsigned short USHORT;
#endif

#ifndef UINT
typedef unsigned int UINT;
#endif // !UINT

typedef void(__stdcall *MvsEncSendCallBack)(unsigned char*, int);
typedef void(__stdcall *MvsEncRecvCallBack)(unsigned char*, int);
// 사용 순서 핸들러 생성 -> 포트 열기 -> Read, Write -> 포트 닫기 -> 핸들러 제거 -> Release()

#pragma region 핸들러, 포트열기, Read, Write



//핸들러 생성, 해제, 유효성 검사
extern "C"   HANDLE __stdcall MvsEncCreateHandler();
extern "C"   void __stdcall MvsEncCloseHandler(HANDLE handle);
extern "C"   bool __stdcall MvsEncIsValidHandle(HANDLE handle);

//Open, Close, Open Check
extern "C"   bool __stdcall MvsEncOpen(HANDLE handle, const char* lpszDevice);
extern "C"   void __stdcall MvsEncClose(HANDLE handle);
extern "C"   bool __stdcall MvsEncIsOpen(HANDLE handle);
extern "C"   bool __stdcall MvsEncPurge(HANDLE handle);

//Read, Write
extern "C"   UINT __stdcall MvsEncSendOnly(HANDLE handle, char command, USHORT address, UINT data = 0);
extern "C"   UINT __stdcall MvsEncSendData(HANDLE handle, char command, USHORT address, UINT data = 0, UINT *out = nullptr);
extern "C"   UINT __stdcall MvsEncSendRead(HANDLE handle, USHORT address, UINT data, UINT *out = nullptr);
extern "C"   UINT __stdcall MvsEncSendWrite(HANDLE handle, USHORT address, UINT data, UINT *out = nullptr);
extern "C"   UINT __stdcall MvsEncRead(HANDLE handle, void* buffer, int offset, int count, bool *cancel);
extern "C"	 UINT __stdcall MvsEncReadUntil(HANDLE handle, void* buffer, unsigned char etx, bool * cancel);

//해제
extern "C"  void __stdcall MvsEncRelease();
#pragma endregion

#pragma region Event
//Send, receive Event
extern "C"   UINT __stdcall MvsEncRegisterSendEvent(HANDLE handle, void* func);
extern "C"   UINT __stdcall MvsEncRegisterRecvEvent(HANDLE handle, void* func);
extern "C"   UINT __stdcall MvsEncRegisterErrorEvent(HANDLE handle, void* func);
#pragma endregion

#pragma region Enc함수 Return Status
//Trigger Controls
extern "C"   UINT __stdcall MvsEncSetTriggerControlsEachTrigger(HANDLE handle, int triggerID, int type, int channel, int multi, int conditionFactor, int outputOperator, int directionFactor, int positionDirection, int correction, int triggerBase);
extern "C"   UINT __stdcall MvsEncSetTriggerControls(HANDLE handle, int triggerID, const void* ctrl);
extern "C"   UINT __stdcall MvsEncGetTriggerControls(HANDLE handle, int triggerID, void* ctrl);

extern "C"  UINT __stdcall MvsEncSetTriggerGenerator(HANDLE handle, int triggerID, int cycle, int pulseHigh);
extern "C"  UINT __stdcall MvsEncGetTriggerGenerator(HANDLE handle, int triggerID, int* cycle, int* pulseHigh);


//ROLL VERSION 전용
extern "C"   UINT __stdcall MvsEncSetTriggerGeneratorRoll(HANDLE handle, int triggerID, unsigned int cycle);
extern "C"   UINT __stdcall MvsEncGetTriggerGeneratorRoll(HANDLE handle, int triggerID, int* cycle);

//아날로그 채배율 설정
extern "C"  UINT __stdcall MvsEncSetEncoderMultiForAnalog(HANDLE handle, int triggerID, int AnalogMulti);
extern "C"  UINT __stdcall MvsEncGetEncoderMultiForAnalog(HANDLE handle, int triggerID, int* AnalogMulti);

//아날로그 엔코더 전용 함수
//A상과 B상을 바꿈.
extern "C"   UINT __stdcall MvsEncSetEncoderInversion(HANDLE handle, int triggerID, int inversion);
extern "C"   UINT __stdcall MvsEncGetEncoderInversion(HANDLE handle, int triggerID, int* inversion);

//아날로그 엔코더 캘리브레이션
extern "C"  UINT __stdcall MvsEncStartCalibration(HANDLE handle, int triggerID);
extern "C"  UINT __stdcall MvsEncStopCalibration(HANDLE handle, int triggerID);

//트리거 출력 구간 설정
//count 값으로 설정(start)
extern "C"  UINT __stdcall MvsEncSetTriggerPosition0(HANDLE handle, int triggerID, int position);
//um 값으로 설정
extern "C"  UINT __stdcall MvsEncSetTriggerPositionMicrometer0(HANDLE handle, int triggerID, int position, int multi, double resolution, int unit = 0);
extern "C"  UINT __stdcall MvsEncGetTriggerPosition0(HANDLE handle, int triggerID, int* position);
//count 값으로 설정(end)
extern "C"  UINT __stdcall MvsEncSetTriggerPosition1(HANDLE handle, int triggerID, int position);
//um 값으로 설정
extern "C"  UINT __stdcall MvsEncSetTriggerPositionMicrometer1(HANDLE handle, int triggerID, int position, int multi, double resolution, int unit = 0);
extern "C"  UINT __stdcall MvsEncGetTriggerPosition1(HANDLE handle, int triggerID, int *position);

extern "C"  UINT __stdcall MvsEncSetEncoderDirection(HANDLE handle, int triggerID, int direction);
extern "C"  UINT __stdcall MvsEncGetEncoderDirection(HANDLE handle, int triggerID, int *direction);

//트리거 출력 모드 설정
extern "C"  UINT __stdcall MvsEncSetTriggerOutputMode(HANDLE handle, int triggerID, int mode);
extern "C"  UINT __stdcall MvsEncGetTriggerOutputMode(HANDLE handle, int triggerID, int *mode);

extern "C"  UINT __stdcall MvsEncSetTriggerPulseWidth(HANDLE handle, int triggerID, double pulseWidth);
extern "C"  UINT __stdcall MvsEncGetTriggerPulseWidth(HANDLE handle, int triggerID, double *pulseWidth);

extern "C"  UINT __stdcall MvsEncSetTriggerDelay(HANDLE handle, int triggerID, double delay);
extern "C"  UINT __stdcall MvsEncGetTriggerDelay(HANDLE handle, int triggerID, double *delay);

extern "C"  UINT __stdcall MvsEncSetEncoderResetValue(HANDLE handle, int triggerID, int position);
extern "C"  UINT __stdcall MvsEncGetEncoderResetValue(HANDLE handle, int triggerID, int *position);

//가상 엔코더, 주파수 설정
extern "C" UINT __stdcall MvsEncSetVirtualEncoderFrequency(HANDLE handle, int virtualEncoderID, int frequency);
extern "C" UINT __stdcall MvsEncGetVirtualEncoderFrequency(HANDLE handle, int virtualEncoderID, int *frequency);


//디지털입력 값 가져오기
extern "C"  UINT __stdcall MvsEncGetDI(HANDLE handle, int* di0, int* di1, int* di2, int* di3, int* di4, int* di5, int* di6);
extern "C"  UINT __stdcall MvsEncGetDIState(HANDLE handle, int diID, int *value);

//디지털입력 카운트 값 가져오기
extern "C"  UINT __stdcall MvsEncGetDiCount(HANDLE handle, int diID, UINT *dicount);
//트리거출력 카운트 값 가져오기
extern "C"  UINT __stdcall MvsEncGetTriggerCount(HANDLE handle, int triggerID, UINT *Triggercount);
//현재위치 가져오기 count
extern "C"  UINT __stdcall MvsEncGetEncoderPosition(HANDLE handle, int encoderID, int *encoderposition);
//디지털 현재위치 가져오기 um
extern "C"  UINT __stdcall MvsEncGetEncoderPositionForAnalog(HANDLE handle, int encoderID, int multi, double resolution, int *encoderposition);
//에러 카운트 값 가져오기
extern "C"  UINT __stdcall MvsEncGetTriggerErrorCount(HANDLE handle, int triggerID,UINT *errorCount);

//펌웨어버전과 보드타입 가져오기
extern "C"  UINT __stdcall MvsEncGetFirmwareVersion(HANDLE handle, double *version);
//로직버전 가져오기
extern "C"  UINT __stdcall MvsEncGetLogicVersion(HANDLE handle, double *version);

//보드초기화
extern "C"  UINT __stdcall MvsEncLoadDefaultParameters(HANDLE handle);
//플래시메모리에 설정값 저장
extern "C"  UINT __stdcall MvsEncSaveFlash(HANDLE handle);
//플래시메모리에서 설정값 가져오기
extern "C"  UINT __stdcall MvsEncLoadFlash(HANDLE handle);
//모든 카운트 초기화
extern "C"  UINT __stdcall MvsEncClearAll(HANDLE handle);
//디지털입력 카운트 초기화
extern "C"  UINT __stdcall MvsEncClearDiAll(HANDLE handle);
//트리거출력 카운트 초기화
extern "C"  UINT __stdcall MvsEncClearTriggerAll(HANDLE handle);
//현재위치 초기화
extern "C"  UINT __stdcall MvsEncClearEncoderPositionAll(HANDLE handle);
//에러 카운트 초기화
extern "C"  UINT __stdcall MvsEncClearErrorCountAll(HANDLE handle);
//디지털입력 카운트 초기화 개별
extern "C"  UINT __stdcall MvsEncClearDiCount(HANDLE handle, int diID);
//트리거출력 카운트 초기화 개별
extern "C"  UINT __stdcall MvsEncClearTriggerCount(HANDLE handle, int triggerID);
//현재위치 초기화 개별
extern "C"  UINT __stdcall MvsEncClearEncoderPosition(HANDLE handle, int encoderID);
//에러 카운트 초기화 개별
extern "C"  UINT __stdcall MvsEncClearErrorCount(HANDLE handle, int id);

//엔코더 분해능 설정
//resolution (um)
extern "C"  UINT __stdcall  MvsEncSetEncoderResolution(HANDLE handle, double resolution);
extern "C"  UINT __stdcall MvsEncGetEncoderResolution(HANDLE handle, double *resolution);

///////////////////////JOE ADDED 2023.02.09
extern "C"  UINT __stdcall MvsEncSetIndexTriggerLastIndex(HANDLE handle, UINT triggerID, UINT lastIndex);
extern "C"  UINT __stdcall MvsEncSetIndexTriggerPosition(HANDLE handle, UINT triggerID, UINT positionIndex, int position);
extern "C"  UINT __stdcall MvsEncGetIndexTriggerLastIndex(HANDLE handle, UINT triggerID, UINT * lastIndex);
extern "C"  UINT __stdcall MvsEncGetIndexTriggerPosition(HANDLE handle, UINT triggerID, UINT positionIndex, int* position);

extern "C" UINT __stdcall MvsEncSetPositiveRun(HANDLE handle, UINT runCHbit);
extern "C" UINT __stdcall MvsEncGetPositiveRun(HANDLE handle, UINT * runCHbit);
extern "C" UINT __stdcall MvsEncSetNegativeRun(HANDLE handle, UINT runCHbit);
extern "C" UINT __stdcall MvsEncGetNegativeRun(HANDLE handle, UINT * runCHbit);
///////////////////////JOE ADDED 2023.02.09 end

///////////////////////JOE ADDED 2023.02.16
extern "C" UINT __stdcall MvsEncSetTrgMode(HANDLE handle, UINT runCHbit);
extern "C" UINT __stdcall MvsEncGetTrgMode(HANDLE handle, UINT * runCHbit);
///////////////////////JOE ADDED 2023.02.16 end

#pragma endregion







#endif