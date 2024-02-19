
#pragma once

#include "../WorkStep.h"

class CWorkInited : public CWorkStep
{
/* 생성자 & 파괴자 */
public:

	CWorkInited();
	virtual ~CWorkInited();

/* 가상함수 재정의 */
protected:
public:

	virtual	BOOL		InitWork();
	virtual VOID		DoWork();


/* 로컬 변수 */
protected:



/* 로컬 함수 */
protected:
	VOID				SetWorkNext();
	VOID				PhilInitComp(UINT8 state);

	ENG_JWNS			ResetSafetyPLC();			/* Reset the Safety PLC */
	ENG_JWNS			CheckSafetyIO();			/* 초기화 가능한 상태 IO 체크 */

	ENG_JWNS			IsLuriaConnected();			/* Luria 연결 여부 */
	ENG_JWNS			IsLuriaValid();				/* Luria System Check */

	ENG_JWNS			SetAllPhotoheads();			/* Photoheads 정보 설정 */
	ENG_JWNS			IsAllPhotoheads();			/* Photoheads 설정 여부 */

	ENG_JWNS			SetSpecPhotoheadsIPv4();	/* Photohead Spec 설정 */
	ENG_JWNS			IsSpecPhotoheadsIPv4();		/* Photohead Spec 설정 여부 */

	ENG_JWNS			SetMotionControl();			/* Motion Control 기본 설정 */
	ENG_JWNS			IsMotionControl();			/* Motion Control 기본 설정 여부 */

	ENG_JWNS			SetMotionType1();			/* Motion Control TYpe1 기본 설정 */
	ENG_JWNS			IsMotionType1();			/* Motion Control TYpe1 기본 설정 여부 */

	ENG_JWNS			SetTableSettings();			/* Table Settings 기본 설정 */
	ENG_JWNS			IsTableSettings();			/* Table Settings 기본 설정 여부 */

	ENG_JWNS			SetSystemSettings();		/* System Settings & Etc 기본 설정 */
	ENG_JWNS			IsSystemSettings();			/* System Settings & Etc 기본 설정 여부 */

	ENG_JWNS			SetAllACamMotorReset();		/* Align Camera Z Axis Motor - Motor Reseting */
	ENG_JWNS			IsAllACamMotorReseted();	/* Align Camera Z Axis Motor - Motor Reset 완료 여부 */

	ENG_JWNS			SetAllACamMotorInit();		/* Align Camera Z Axis Motor - Focus Inited */
	ENG_JWNS			IsAllACamMotorInited();		/* Align Camera Z Axis Motor - 초기화 완료 여부 */

	ENG_JWNS			SetAllACamMotorMoving();	/* Align Camera의 모든 Z Axis Motor - Set Positioning */
	ENG_JWNS			IsAllACamMotorMoved();		/* Align Camera의 모든 Z Axis Motor - 위치 이동 완료 여부 */

	ENG_JWNS			SetHotAirTemp();			/* 온조기 - 온도 설정 */
	ENG_JWNS			IsHotAirTemped();			/* 온조기 - 온도 설정 완료 여부 */

	ENG_JWNS			CheckEquipState();
};
