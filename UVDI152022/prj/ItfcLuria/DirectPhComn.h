
#pragma once

#include "BaseFamily.h"

class CDirectPhComn  : public CBaseFamily
{
/* 생성자 & 파괴자 */
public:

	CDirectPhComn(LPG_LDSM shmem, CCodeToStr *error);
	virtual ~CDirectPhComn();


/* 가상 함수 */
protected:

public:

	virtual VOID		SetRecvPacket(UINT8 cmd, PUINT8 data, UINT32 size);


/* 로컬 변수 */
protected:

	LPG_LDDP			m_pstShMemDP;
	LPG_LDPF			m_pstShMemPF;


/* 로컬 함수 */
protected:

	PUINT8				GetPktData(UINT16 msg_size,
								   UINT8 ph_no,
								   UINT16 rec_id,
								   PUINT8 data=NULL,
								   UINT8 size=0);

	INT32				SetRecvDirectPh(UINT8 ph_no, UINT16 rec_id, PUINT8 data);

	INT32				SetRecvReplyAck(PUINT8 data);
	INT32				SetRecvMotorStatus(UINT8 ph_no, PUINT8 data);
	INT32				SetRecvMotorAbsPosition(UINT8 ph_no, PUINT8 data);
	INT32				SetRecvTemp(UINT8 ph_no, PUINT8 data);
	INT32				SetRecvAutofocusPosition(UINT8 ph_no, PUINT8 data);
	INT32				SetRecvLedOnTime(UINT8 ph_no, PUINT8 data);
	INT32				SetRecvOverallError(UINT8 ph_no, PUINT8 data);
	INT32				SetRecvLightSourceInfo(UINT8 ph_no, PUINT8 data);
	INT32				SetRecvCameraIllumLED(UINT8 ph_no, PUINT8 data);
	INT32				SetRecvPlotEnable(UINT8 ph_no, PUINT8 data);
	INT32				SetRecvFocusPosition(UINT8 ph_no, PUINT8 data);



	INT32				SetRecvLedPower(UINT8 ph_no, PUINT8 data);
	INT32				SetRecvLedDriveStatus(UINT8 ph_no, PUINT8 data);
	INT32				SetRecvLedLightOnOff(UINT8 ph_no, PUINT8 data);
	INT32				SetRecvFlatnessMaskOn(UINT8 ph_no, PUINT8 data);
	INT32				SetRecvLoadInternalImage(UINT8 ph_no, PUINT8 data);
	INT32				SetRecvActiveSeqFile(UINT8 ph_no, PUINT8 data);
	INT32				SetRecvDmdFlipOutput(UINT8 ph_no, PUINT8 data);
	INT32				SetRecvDmdMirrorShake(UINT8 ph_no, PUINT8 data);
	INT32				SetRecvSequencerState(UINT8 ph_no, PUINT8 data);


/* 공용 함수 */
public:

	/* Focus Motor Parts */
	PUINT8				GetPktMotorStateAll();
	PUINT8				GetPktMotorPositionInit(UINT8 ph_no, UINT8 action);
	PUINT8				GetPktMotorPositionInitAll(UINT8 action);
	PUINT8				GetPktMotorAbsPositionAll();
	PUINT8				GetPktPhLedOnTimeAll(UINT16 freq_no);
	PUINT8				GetPktPhLedTempAll(UINT16 freq_no);
	PUINT8				SetPktMotorAbsPosition(UINT8 ph_no, UINT32 pos);
	PUINT8				GetPktFocusMotorMeasureDist(UINT8 ph_no);
	PUINT8				SetPktFocusMotorMeasureDist(UINT8 ph_no, UINT32 dist);

	PUINT8				GetPktImageType(UINT8 ph_no);

	PUINT8				GetPktFlatnessMaskOn(UINT8 ph_no);
	PUINT8				SetPktFlatnessMaskOn(UINT8 ph_no, UINT8 enable);

	PUINT8				SetPktLoadInternalImage(UINT8 ph_no, UINT8 img_no);
	PUINT8				GetPktLoadInternalImage(UINT8 ph_no);

	PUINT8				SetPktActiveSequence(UINT8 ph_no, UINT8 type);
	PUINT8				GetPktActiveSequence(UINT8 ph_no);

	PUINT8				SetPktLedLightOnOff(UINT8 ph_no, ENG_LLPI led_no, UINT8 onoff);
	PUINT8				GetPktLedLightOnOff(UINT8 ph_no, ENG_LLPI led_no);

	PUINT8				SetPktSequnceState(UINT8 ph_no, UINT8 seq_cmd, UINT8 enable);
	PUINT8				GetPktSequnceState(UINT8 ph_no, UINT8 seq_cmd);
	/* LED Driver Amplitude (LedAmplitude 함수와 성격이 다름. 이함수는 Spot 용도) */
	PUINT8				SetPktLightAmplitude(UINT8 ph_no, ENG_LLPI led_no, UINT16 amp_idx);
	PUINT8				GetPktLightAmplitude(UINT8 ph_no, ENG_LLPI led_no);
	/* Get the Light Source Driver Status */
	PUINT8				GetPktLedSourceDriverStatus(UINT8 ph_no, ENG_LLPI led_no);
	/* 임의 광학계 에러 정보 요청 */
	PUINT8				GetPktOverallErrorStatus(UINT8 ph_no);
	/* Clear Error Status */
	PUINT8				SetPktClearErrorStatus(UINT8 ph_no);
	/* Light Source Info. */
	PUINT8				GetPktLightSource(UINT8 ph_no);
	/* Camera Illumination Light Value */
	PUINT8				SetPktCamIllumValue(UINT8 ph_no, UINT8 value);
	PUINT8				GetPktCamIllumValue(UINT8 ph_no);
	/* Line Sensor Data */
	PUINT8				SetPktLineSensorPlot(UINT8 ph_no, UINT8 enable);
	PUINT8				GetPktLineSensorPlot(UINT8 ph_no);
	/* Current Autofocus Position */
	PUINT8				GetPktCurrentAutofocusPosition(UINT8 ph_no);
	/* DMD Flip */
	PUINT8				SetPktDmdFlipOutput(UINT8 ph_no, UINT8 flip);
	PUINT8				GetPktDmdFlipOutput(UINT8 ph_no);
	/* DMD Mirror Shake */
	PUINT8				SetPktDmdMirrorShake(UINT8 ph_no, UINT8 enable);
	PUINT8				GetPktDmdMirrorShake(UINT8 ph_no);
	/* Light Pulse Duration*/
	PUINT8				SetPktLightPulseDuration(UINT8 ph_no, UINT16 value);
	PUINT8				GetPktLightPulseDuration(UINT8 ph_no);

	//신규추가.

	PUINT8 SetPktStoredAutofocusPosition(UINT8 ph_no, UINT16 setPos); // stored af 값 쓰기
	PUINT8 GetPktStoredAutofocusPosition(UINT8 ph_no); //읽기
	INT32 SetRecvStoredAutofocusPosition(UINT8 phNo, PUINT8 data); //결과
};