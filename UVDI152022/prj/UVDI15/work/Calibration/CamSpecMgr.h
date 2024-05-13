#pragma once
#include "../../../../inc/conf/vision_uvdi15.h"

#define DEF_MOTION_CALI_CENTERING_MIN	0.001f	/* ����: mm. 1 um */

enum EnCamSpecRunMode
{
	eMODE_CAM_SPEC_ANGLE = 0,
	eMODE_CAM_SPEC_PIXEL_SIZE,
	eMODE_CAM_SPEC_MAX
};

class CCamSpecMgr
{

	/* ������ */
public:

	enum EnMeasureStep
	{
		eMEASURE_MOVE_OTHER_HEAD_OUTWORD = 0,
		eMEASURE_MOVE_HEAD_Z_POS,
		eMEASURE_MOVE_QUARTZ_POS,
		eMEASURE_READY_TRIGGER,
		eMEASURE_GRAB_QUARTZ,
		eMEASURE_MOVE_CENTER,
		eMEASURE_MOVE_X_POS,
		eMEASURE_GRAB_QUARTZ_X,
		eMEASURE_MAX
	};

	/* �����ѹ� ��� */
	enum EnCamSpec
	{
		eMODEL_FIND_COUNT = 2,
		eCOUNT_OF_CAMERA = 2,
	};



/* ������ & �ı��� */
public:

	CCamSpecMgr();
	virtual ~CCamSpecMgr();

	/* Singleton pattern */
	static CCamSpecMgr* GetInstance()
	{
		static CCamSpecMgr _inst;
		return &_inst;
	}

/* ���� ���� */
protected:

	BOOL				m_bStop;				/* ���� �÷��� */
	BOOL				m_bRunMode;				/* FALSE : ī�޶� ȸ�� ���� �� ����, TRUE : ī�޶� �ȼ� ũ�� �� ���� */
	UINT8				m_u8ACamID;				/* 1 or Later */

	CWinThread*			m_pMeasureThread;		/* ���� ������ */
	BOOL				m_bRunnigThread;		/* ������ ���� ���� �ľ�*/

	UINT32				m_u32RepeatTotal;		/* �� �ݺ��� Ƚ�� */


/* ���� �Լ� */
protected:

	VOID Wait(int nTime);

	BOOL ACamMovingSide(int nTimeOut = 60000, double dDiffDistance = 0.001);
	BOOL ACamMovingZAxisQuartz();
	BOOL ACamMovingQuartz(int nTimeOut = 60000, double dDiffDistance = 0.001);
	BOOL WaitTrigEnabled(int nTimeOut = 60000);
	BOOL GrabQuartzData(STG_ACGR& stGrab, BOOL bRunMode, int nRetryCount = 3);
	BOOL MotionMoving(double dMoveX, double dMoveY, int nTimeOut = 60000, double dDiffDistance = DEF_MOTION_CALI_CENTERING_MIN);
	BOOL MotionMovingX(int nGrabCount, int nTimeOut = 60000, double dDiffDistance = 0.001);

	VOID CalcACamSpec(STG_DBXY stCenter1, STG_DBXY stCenter2);
	

/* ���� �Լ� */
public:
	/* ���� ��⵿������ ���� �����带 ������ UI���� ȣ���� �����Ѵ�. */
	BOOL Measurement(HWND hHwnd = NULL);
	////////////////////////////////////////////////////////////////////

	VOID Initialize();
	VOID Terminate();

	BOOL SetRegistModel();

	BOOL IsProcessWorking() { return m_bRunnigThread; }

	VOID SetRunMode(BOOL bMode) { m_bRunMode = bMode; }
	BOOL GetRunMode() { return m_bRunMode; }

	VOID SetRepeatCount(UINT32 u32Count) { m_u32RepeatTotal = u32Count; }
	UINT32 GetRepeatCount() { return m_u32RepeatTotal; }

	VOID SetCamID(UINT8 u8Count) { m_u8ACamID = u8Count; }
	UINT8 GetCurCamID() { return m_u8ACamID; }
	

	VOID MeasureStart(HWND hHwnd = NULL);
	VOID SetStopProcess() { m_bStop = TRUE; }
};

UINT SpecMeasureThread(LPVOID lpParam);