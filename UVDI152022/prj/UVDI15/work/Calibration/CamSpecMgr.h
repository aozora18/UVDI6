#pragma once
#include "../../../../inc/conf/vision_uvdi15.h"

#define DEF_MOTION_CALI_CENTERING_MIN	0.001f	/* 단위: mm. 1 um */

enum EnCamSpecRunMode
{
	eMODE_CAM_SPEC_ANGLE = 0,
	eMODE_CAM_SPEC_PIXEL_SIZE,
	eMODE_CAM_SPEC_MAX
};

class CCamSpecMgr
{

	/* 열거형 */
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

	/* 매직넘버 대용 */
	enum EnCamSpec
	{
		eMODEL_FIND_COUNT = 2,
		eCOUNT_OF_CAMERA = 2,
	};



/* 생성자 & 파괴자 */
public:

	CCamSpecMgr();
	virtual ~CCamSpecMgr();

	/* Singleton pattern */
	static CCamSpecMgr* GetInstance()
	{
		static CCamSpecMgr _inst;
		return &_inst;
	}

/* 로컬 변수 */
protected:

	BOOL				m_bStop;				/* 정지 플래그 */
	BOOL				m_bRunMode;				/* FALSE : 카메라 회전 각도 값 측정, TRUE : 카메라 픽셀 크기 값 측정 */
	UINT8				m_u8ACamID;				/* 1 or Later */

	CWinThread*			m_pMeasureThread;		/* 측정 쓰레드 */
	BOOL				m_bRunnigThread;		/* 쓰레드 동작 유무 파악*/

	UINT32				m_u32RepeatTotal;		/* 총 반복될 횟수 */


/* 로컬 함수 */
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
	

/* 공용 함수 */
public:
	/* 내부 대기동작으로 인해 쓰레드를 제외한 UI에서 호출을 금지한다. */
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