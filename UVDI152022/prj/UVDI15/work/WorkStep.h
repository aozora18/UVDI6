
#pragma once

#include "Work.h"

class CWorkStep : public CWork
{
/* ������ & �ı��� */
public:

	CWorkStep();
	virtual ~CWorkStep();

	vector<int> selCamNum;
/* �����Լ� ������ */
protected:
public:

	virtual	BOOL		InitWork()	= 0;
	virtual VOID		DoWork()	= 0;


/* ������ */
protected:


/* ���� ���� */
protected:

	UINT8				m_u8LastJobCount;		/* ���� �ֱٱ��� ��ϵ� Job Name�� ��� ���� (�ӽ� ���� �� T.T) */
	UINT8				m_u8PrintStripNum;		/* ���� �������� �뱤�� ���� ���� */
	UINT8				m_u8LastLoadStrip;		/* ���� �ֱٿ� �Ź��� ����� ����� �� */

	INT32				m_i32PrintingStageY;	/* ���� �뱤 ���� Stage Y ���� ��� �� (����: um) */
	INT32				m_i32ACamZAxisSet[2];	/* Basler Camera Z Axis ���� �� �ӽ� ���� (����: 0.1 um or 100 nm) */
	UINT64				m_u64TickPrintStrip;	/* ���� �ֱٿ� �뱤�� Ƚ���� ����� �ð� �ӽ� ���� */

	DOUBLE				m_dbLastPrintPosY;		/* ���� �ֱٿ� �뱤�� Y �� ���� �뱤 ��� ��ġ */
	DOUBLE				m_dbPhZAxisSet[MAX_PH];	/* ���� �ֱٿ� ������ ���а� Z �� ���� �� �ӽ� ���� (����: mm) */

	ENG_MMDI			m_enDrvACamID;			/* ���� ���õ� Motion Drive �� Align Camera ID */
	ENG_MMDI			m_enVectMoveDrv;		/* ���� �ֱٿ� Vector�� �������� �̵��� ���� �� */


/* ���� �Լ� */
protected:

	ENG_JWNS			SetWorkWaitTime(UINT64 time);
	ENG_JWNS			IsWorkWaitTime();
#if 1
	ENG_JWNS			IsStepRepeatTimeout(UINT8 back_step, UINT64 time_delay,
											UINT8 retry_cnt=0x03);
#endif
	/* 2���� ������ ������ �ó����� �Լ� */
	ENG_JWNS			SetTrigEnable(BOOL enable);
	ENG_JWNS			IsTrigEnabled(BOOL enable) { return ENG_JWNS::en_next; }

	ENG_JWNS			SetMovingUnloader();
	ENG_JWNS			IsMovedUnloader();

	ENG_JWNS			IsLoadedGerberCheck();
	ENG_JWNS			IsMotorDriveStopAll();

	ENG_JWNS			SetAlignMeasMode();
	ENG_JWNS			IsAlignMeasMode();

	ENG_JWNS			SetJobNameSelecting();
	ENG_JWNS			IsJobNameSelected();

	ENG_JWNS			SetJobNameLoading();
	ENG_JWNS			IsJobNameLoaded();

	ENG_JWNS			SetTrigRegistGlobal();
	ENG_JWNS			IsTrigRegistGlobal();
	ENG_JWNS			SetTrigRegistLocal(UINT8 scan);
	ENG_JWNS			IsTrigRegistLocal(UINT8 scan);

	ENG_JWNS			SetAlignMovingInit();
	ENG_JWNS			IsAlignMovedInit(function<bool()> callback = nullptr);
	ENG_JWNS			SetAlignMovingGlobal();
	ENG_JWNS			IsAlignMovedGlobal();
	ENG_JWNS			SetAlignMovingLocal(UINT8 mode, UINT8 scan);
	ENG_JWNS			IsAlignMovedLocal(UINT8 mode, UINT8 scan);

	ENG_JWNS			SetFinish();
	ENG_JWNS			IsFinish();

	ENG_JWNS			SetLedVacuumShutterIn();
	ENG_JWNS			SetLedVacuumShutterOut();
	ENG_JWNS			SetVacuumShutterOnOff(UINT8 vacuum, UINT8 shutter);
	ENG_JWNS			SetLedVacuumShutterOnOff(UINT8 start_led, UINT8 vacuum, UINT8 shutter);
	ENG_JWNS			SetLedShutterOnOff(UINT8 start_led, UINT8 shutter);
	ENG_JWNS			IsVacuumWait();
	ENG_JWNS			IsShutterWait();
	ENG_JWNS			IsShutterVacuumWait();

	ENG_JWNS			SetPrePrinting();
	ENG_JWNS			IsPrePrinted();
	ENG_JWNS			SetPrinting();
	ENG_JWNS			IsPrinted();

	ENG_JWNS			SetAlignMarkRegist();
	ENG_JWNS			SetAlignMarkRegistforStatic();
	ENG_JWNS			IsAlignMarkRegist();

	ENG_JWNS			SetPhZAxisMovingAll();
	ENG_JWNS			IsPhZAxisMovedAll();

	ENG_JWNS			SetACamZAxisMovingAll(unsigned long& lastuniqueID);
	ENG_JWNS			IsACamZAxisMovedAll(unsigned long& lastuniqueID);

	ENG_JWNS			IsSetMarkValid(ENG_AMTF type, UINT8 scan);
	ENG_JWNS			IsSetMarkValidAll(UINT8 mode,int* camNum=nullptr);

	ENG_JWNS			SetGerberRegist();
	ENG_JWNS			IsGerberRegisted();

	ENG_JWNS			SetStepDutyFrame();
	ENG_JWNS			IsStepDutyFrame();

	ENG_JWNS			SetExposeStartXY();
	ENG_JWNS			IsExposeStartXY();

	ENG_JWNS			SetLedAmplitude();
	ENG_JWNS			IsLedAmplituded();

	ENG_JWNS			SetAllPhMotorMoving();
	ENG_JWNS			IsAllPhMotorMoved();

	ENG_JWNS			SetAllPhMotorHoming();
	ENG_JWNS			IsAllPhMotorHomed();

	ENG_JWNS			InitDriveErrorReset();
	ENG_JWNS			IsDriveErrorReseted(UINT8 back_step=0x01);

	ENG_JWNS			DoDriveHoming(ENG_MMDI drv_id);
	ENG_JWNS			DoDriveHomingAll();
	ENG_JWNS			IsDrivedHomed(ENG_MMDI drv_id);
	ENG_JWNS			IsDrivedHomedAll();

	ENG_JWNS			SetACamMovingZAxisQuartz();
	ENG_JWNS			IsACamMovedZAxisQuartz();

	ENG_JWNS			SetACamMovingSide();
	ENG_JWNS			IsACamMovedSide();

	ENG_JWNS			SetACamMovingQuartz();
	ENG_JWNS			IsACamMovedQuartz();

	ENG_JWNS			SetMovingSideACam2();
	ENG_JWNS			IsMovedSideACam2();

	ENG_JWNS			SetMovingMarkACam1(UINT8 mark_no);
	ENG_JWNS			IsMovedMarkACam1();

	ENG_JWNS			SetSelectedJobIndex(UINT8 index=0);
	ENG_JWNS			IsSelectedJobIndex();

	ENG_JWNS			SetDeleteSelectedJobName(UINT32 time);
	ENG_JWNS			IsDeleteSelectedJobName(UINT8 back_step=0x01);

	ENG_JWNS			SetHysteresis(UINT8 offset=0xff);
	ENG_JWNS			IsSetHysteresis();

	

	/* ���Ϸ� ���۵Ǵ� �ô϶�� �Լ� */
	ENG_JWNS			SetTrigPosReset();
	ENG_JWNS			SetTrigPosCalcSaved();
	ENG_JWNS			SetMovePrintXY();
	ENG_JWNS			WaitACamError(UINT8 cam_no);
	ENG_JWNS			ACam2HomingStop();
	ENG_JWNS			InitializeHWInit();
	ENG_JWNS			SetExposeReady(BOOL in_mark, BOOL detect, BOOL vaccum, UINT32 count);
	ENG_JWNS			IsGrabbedImageCount(UINT16 count, UINT64 delay,int* camNum=nullptr);
	ENG_JWNS			SetTrigOutOneACam(UINT8 cam_id);
	ENG_JWNS			GetJobLists(UINT32 time);

	bool SetAutoFocusFeatures();
	bool MoveCamToSafetypos(ENG_MMDI callbackAxis = ENG_MMDI::en_axis_none, double pos = 0);
	BOOL MoveAxis(ENG_MMDI axis, bool absolute, double pos, bool waiting);

	ENG_JWNS			CheckValidRecipe();
	ENG_JWNS			IsSetTrigPosResetAll();

	ENG_JWNS			ResetErrorMC2();			/* Reset the MC2 */
	ENG_JWNS			CameraSetCamMode(ENG_VCCM mode);

};