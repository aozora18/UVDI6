
#pragma once

#include "../WorkStep.h"

class CWorkInited : public CWorkStep
{
/* ������ & �ı��� */
public:

	CWorkInited();
	virtual ~CWorkInited();

/* �����Լ� ������ */
protected:
public:

	virtual	BOOL		InitWork();
	virtual VOID		DoWork();


/* ���� ���� */
protected:



/* ���� �Լ� */
protected:
	VOID				SetWorkNext();
	VOID				PhilInitComp(UINT8 state);

	ENG_JWNS			ResetSafetyPLC();			/* Reset the Safety PLC */
	ENG_JWNS			CheckSafetyIO();			/* �ʱ�ȭ ������ ���� IO üũ */

	ENG_JWNS			IsLuriaConnected();			/* Luria ���� ���� */
	ENG_JWNS			IsLuriaValid();				/* Luria System Check */

	ENG_JWNS			SetAllPhotoheads();			/* Photoheads ���� ���� */
	ENG_JWNS			IsAllPhotoheads();			/* Photoheads ���� ���� */

	ENG_JWNS			SetSpecPhotoheadsIPv4();	/* Photohead Spec ���� */
	ENG_JWNS			IsSpecPhotoheadsIPv4();		/* Photohead Spec ���� ���� */

	ENG_JWNS			SetMotionControl();			/* Motion Control �⺻ ���� */
	ENG_JWNS			IsMotionControl();			/* Motion Control �⺻ ���� ���� */

	ENG_JWNS			SetMotionType1();			/* Motion Control TYpe1 �⺻ ���� */
	ENG_JWNS			IsMotionType1();			/* Motion Control TYpe1 �⺻ ���� ���� */

	ENG_JWNS			SetTableSettings();			/* Table Settings �⺻ ���� */
	ENG_JWNS			IsTableSettings();			/* Table Settings �⺻ ���� ���� */

	ENG_JWNS			SetSystemSettings();		/* System Settings & Etc �⺻ ���� */
	ENG_JWNS			IsSystemSettings();			/* System Settings & Etc �⺻ ���� ���� */

	ENG_JWNS			SetAllACamMotorReset();		/* Align Camera Z Axis Motor - Motor Reseting */
	ENG_JWNS			IsAllACamMotorReseted();	/* Align Camera Z Axis Motor - Motor Reset �Ϸ� ���� */

	ENG_JWNS			SetAllACamMotorInit();		/* Align Camera Z Axis Motor - Focus Inited */
	ENG_JWNS			IsAllACamMotorInited();		/* Align Camera Z Axis Motor - �ʱ�ȭ �Ϸ� ���� */

	ENG_JWNS			SetAllACamMotorMoving();	/* Align Camera�� ��� Z Axis Motor - Set Positioning */
	ENG_JWNS			IsAllACamMotorMoved();		/* Align Camera�� ��� Z Axis Motor - ��ġ �̵� �Ϸ� ���� */

	ENG_JWNS			SetHotAirTemp();			/* ������ - �µ� ���� */
	ENG_JWNS			IsHotAirTemped();			/* ������ - �µ� ���� �Ϸ� ���� */

	ENG_JWNS			CheckEquipState();
};
