#pragma once
#include "../../../../inc/conf/vision_uvdi15.h"
class CDlgMain;
class CFlushErrorMgr
{
/* 생성자 & 파괴자 */
public:

	CFlushErrorMgr();
	virtual ~CFlushErrorMgr();

	/* Singleton pattern */
	static CFlushErrorMgr* GetInstance()
	{
		static CFlushErrorMgr _inst;
		return &_inst;
	}

/* 열거형 */
protected:

	enum EnMeasureStep : int
	{
		eMEASURE_LOAD_RECIPE = -7,
		eMEASURE_IS_LOAD_COMPLETE = -6,
		eMEASURE_SET_EXPO_POS = -5,
		eMEASURE_PREPRINT = -4,
		eMEASURE_IS_PREPRINT = -3,
		eMEASURE_PRINT = -2,
		eMEASURE_IS_PRINT=-1,
		eMEASURE_GRABBED_IMAGE = 0,
		eMEASURE_MOVE_MOTION,
		eMEASURE_MAX
	};

	/* 매직넘버 대용 */
	enum EnFlushError
	{
		eMODEL_FIND_COUNT = 2
// 		eCOUNT_OF_PROCESS = 5,
// 		eMAX_PROGRESS = 13,
// 
// 		eLAGRANGE_DATA_COUNT = 3
	};


/* 로컬 변수 */
protected:

	BOOL				m_bStop;				/* 정지 플래그 */
	UINT8				m_u8CurStep;
	UINT8				m_u8MaxStep;
	int measureCount=1;
	int completedCount = 0;
	BOOL				m_bResultSucc;
	LPG_ACGR			m_pstGrabData;

	vector<double>		m_dVctStripeWidth;

	CWinThread*			m_pMeasureThread;		/* 측정 쓰레드 */
	BOOL				m_bRunnigThread;		/* 쓰레드 동작 유무 파악*/



/* 로컬 함수 */
protected:

	VOID Wait(int nTime);
	
	BOOL GrabbedImage(double& dCenterMove);
	BOOL MoveMotion(double dCenterMove, int nTimeOut = 30000, double dDiffDistance = 0.001);
	

/* 공용 함수 */
public:
	/* 내부 대기동작으로 인해 쓰레드를 제외한 UI에서 호출을 금지한다. */
	BOOL Measurement(HWND hHwnd, int count, CDlgMain* dlgMain);
	////////////////////////////////////////////////////////////////////

	VOID Initialize();
	VOID Terminate();
	BOOL SetRegistModel();
	BOOL LoadTable();

	LPG_ACGR GetGrabData() { return m_pstGrabData; }

	VOID SetCurStep(UINT8 u8SetStep) { m_u8CurStep = (u8SetStep > 0x01) ? u8SetStep : 0x01; }
	VOID SetMaxStep(UINT8 u8SetStep) { m_u8MaxStep = (u8SetStep > 0x01) ? u8SetStep : 0x01; }

	BOOL IsProcessWorking() { return m_bRunnigThread; }

	VOID MeasureStart(HWND hHwnd ,int count, CDlgMain* dlgMain);
	VOID SetStopProcess() { m_bStop = TRUE; }
// 	VOID MeasureEnd();
};

UINT FlushMeasureThread(LPVOID lpParam);