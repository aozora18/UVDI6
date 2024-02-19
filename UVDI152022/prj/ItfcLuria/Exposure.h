
#pragma once

#include "BaseFamily.h"

class CExposure : public CBaseFamily
{
/* 생성자 & 파괴자 */
public:

	CExposure(LPG_LDSM shmem, CCodeToStr *error);
	virtual ~CExposure();


/* 가상 함수 */
protected:

public:

	virtual VOID		SetRecvPacket(UINT8 cmd, PUINT8 data, UINT32 size);


/* 로컬 변수 */
protected:

	LPG_LDEW			m_pstExposure;


/* 로컬 함수 */
protected:

	VOID				SetRecvExposureEtcBytes(UINT8 uid, PUINT8 data, UINT32 size);
	VOID				SaveLightSourceStatusMulti();


/* 공용 함수 */
public:

	/* Get or Set LED Amplitude */
	PUINT8				GetPktLightAmplitude(ENG_LPGS flag, UINT8 count, UINT16 (*amp)[4]=NULL);		/* uid = 0x01 */
	PUINT8				GetPktLightAmplitude(ENG_LPGS flag, UINT8 ph_no, ENG_LLPI led_no, UINT16 amp=0);
	PUINT8				GetPktScrollStepSize(ENG_LPGS flag, UINT8 step=0);								/* uid = 0x02 */
	PUINT8				GetPktFrameRateFactor(ENG_LPGS flag, UINT16 rate=0);							/* uid = 0x03 */
	PUINT8				GetPktLedDutyCycle(ENG_LPGS flag, UINT8 duty=0);								/* uid = 0x04 */
	PUINT8				GetPktPrePrint();																/* uid = 0x05 */
	PUINT8				GetPktPrint();																	/* uid = 0x06 */
	PUINT8				GetPktAbort();																	/* uid = 0x07 */
	PUINT8				GetPktGetExposureState();														/* uid = 0x08 */
	PUINT8				GetPktGetExposureSpeed();														/* uid = 0x09 */
	PUINT8				GetPktResetExposureState();														/* uid = 0x0a */
	PUINT8				GetPktEnableAfCopyLastStrip(ENG_LPGS flag, UINT8 enable=0);						/* uid = 0x0b */
	PUINT8				GetPktSimulatePrint();															/* uid = 0x0c */
	PUINT8				GetPktMoveTableToExposureStartPos();											/* uid = 0x0d */
	PUINT8				GetPktGetNumberOfLightSources(UINT8 ph_no);										/* uid = 0x0e */
	PUINT8				GetPktGetLightSourceStatus(UINT8 ph_no, ENG_LLPI led_no);						/* uid = 0x0f */
	PUINT8				GetPktDisableAfFirstStrip(ENG_LPGS flag, UINT8 disable=0);						/* uid = 0x10 */
	PUINT8				GetPktAFSensor(ENG_LPGS flag, UINT8 type=0, UINT8 agc=0, UINT16 pwm=0);			/* uid = 0x11 */
	PUINT8				GetPktLightRampUpDown();														/* uid = 0x12 */
	PUINT8				GetPktAFSensorMeasureLaserPWM();												/* uid = 0x13 */
	PUINT8				GetPktGetLightSourceStatusMulti();												/* uid = 0x14 */

	VOID				SetExposureState(PUINT8 data);
};
