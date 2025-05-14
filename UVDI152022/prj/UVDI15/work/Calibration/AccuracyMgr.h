#pragma once
#include "../../../../inc/conf/vision_uvdi15.h"
#include <map>
#define DEF_DIFF_POINTS	2.0F

typedef struct stAccuracyMeasureParam
{
	double dMotorX;
	double dMotorY;
	double dValueX;
	double dValueY;

	double dGbrX;
	double dGbrY;

	stAccuracyMeasureParam()
	{
		dGbrX = 0;
		dGbrY = 0;
		dMotorX = 0;
		dMotorY = 0;
		dValueX = 0;
		dValueY = 0;
		
	}

} ST_ACCR_PARAM;

typedef struct stField
{
	UINT32 u32Row;
	UINT32 u32Col;

	stField()
	{
		u32Row = 0;
		u32Col = 0;
	}
} ST_FIELD;

typedef std::vector<ST_ACCR_PARAM> VCT_ACCR_TABLE;

enum EN_CAL_FIELD_PARAM
{
	eCAL_PARAM_START_POINT_LB = 0,
	eCAL_PARAM_START_POINT_LT,
	eCAL_PARAM_START_POINT_RB,
	eCAL_PARAM_START_POINT_RT,

	eCAL_PARAM_DIR_X = 0,
	eCAL_PARAM_DIR_Y,
};

class DoubleMarkAccurcytest
{
public:
	bool RunDoublemarkTestExam(int camNum, double* pdErrX, double* pdErrY);
	bool RegistMultiMark();

};

class CAccuracyMgr
{
public:
	/* 생성자 & 파괴자 */
	CAccuracyMgr();
	virtual ~CAccuracyMgr();
	map<int, string> timeMap;
	STG_ACGR stGrab;
	
	/* Singleton pattern */
	static CAccuracyMgr* GetInstance()
	{
		static CAccuracyMgr _inst;
		return &_inst;
	}

	enum SearchMode
	{
		single,
		multi,
	};

	void SetExpoAreaMeasure(bool set)
	{
		expoAreaMeasure = set;
	}

	bool IsGbrpadPosSetted()
	{
		return gbrPadInitialPosX != -1 && gbrPadInitialPosY != -1;
	}

	void SetGbrPadInitialPos(double x, double y)
	{

		gbrPadInitialPosX = x;
		gbrPadInitialPosY = y;
	}

	void SetSearchMode(SearchMode mode)
	{
		serchmode = mode;
	}

	SearchMode GetSearchMode()
	{
		return serchmode;
	}

	bool RegistMultiMark()
	{
		return doubleMarkAccurtest.RegistMultiMark();
	}

	bool RunDoublemarkTestExam(int camNum, double* pdErrX=nullptr, double* pdErrY = nullptr)
	{
		return doubleMarkAccurtest.RunDoublemarkTestExam(camNum, pdErrX, pdErrY);
	}

	/* 열거형 */
public:
	enum EN_REPORT_LIST
	{
		e2DCAL_REPORT_OK = 0,
		e2DCAL_REPORT_MOTION_FIAL,
		e2DCAL_REPORT_GRAB_FIAL,
		e2DCAL_REPORT_MAX
	};

	enum MeasureRegion
	{
		align,
		expo,
	};


protected:

	DoubleMarkAccurcytest doubleMarkAccurtest;
	SearchMode serchmode = SearchMode::single;
	bool expoAreaMeasure = false;

	enum EnMeasureStep
	{
		eMEASURE_MOVE = 0,
		eMEASURE_GRAB,

		eMEASURE_MAX
	};

	/* 매직넘버 대용 */
	enum EnCamSpec
	{
		eMODEL_FIND_COUNT = 2,
		eCOUNT_OF_CAMERA = 2,
	};


	MeasureRegion measureRegion = MeasureRegion::align;



	/* 로컬 변수 */
protected:

	BOOL				m_bStop;				/* 정지 플래그 */

	CWinThread* m_pMeasureThread;		/* 측정 쓰레드 */
	BOOL				m_bRunnigThread;		/* 쓰레드 동작 유무 파악*/

	BOOL				m_bUseCalData;
	BOOL				m_bUseCamDrv;

	UINT8				m_u8ACamID = 1;

	int					m_nStartIndex;
	
	VCT_ACCR_TABLE		m_stVctTable;

	double gbrPadInitialPosX = -1;
	double gbrPadInitialPosY = -1;



	/* 로컬 함수 */
protected:

	VOID Wait(int nTime);
	BOOL Move(ENG_MMDI eMotor, double dPos, int nTimeOut = 60000, double dDiffDistance = DEF_MOTION_CALI_CENTERING_MIN);
	BOOL MotionCalcMoving(double dMoveX, double dMoveY, double dVel = -1, int nTimeOut = 60000, double dDiffDistance = DEF_MOTION_CALI_CENTERING_MIN);
	BOOL GrabData(STG_ACGR& stGrab, BOOL bRunMode, int nRetryCount);

	/* 공용 변수 */
public:

	/* 공용 함수 */
public:
	////////////////////////////////////////////////////////////////////
	VOID SortField(VCT_ACCR_TABLE& stVctField);
	ST_FIELD ClacField(VCT_ACCR_TABLE stVctField);

	BOOL LoadMeasureField(CString strPath);
	BOOL MakeMeasureField(CString strPath, CDPoint dpStartPos, UINT8 u8StartPoint, UINT8 u8Dir, double dAngle, double dPitch, CPoint cpPoint, bool toXDirection,bool turnBack);

	BOOL SaveCaliFile(CString strFileName);
	bool MoveTillArrive(double x, double y, double spd);

	VOID SetUseCalData(BOOL bSet) { m_bUseCalData = bSet; }
	BOOL GetUseCalData() { return m_bUseCalData; }

	VOID SetUseCamDrv(BOOL bUseCamDrv) { m_bUseCamDrv = bUseCamDrv; }
	BOOL GetCurXDrv() { return m_bUseCamDrv; }

	VOID SetCamID(UINT8 u8Count)
	{
		m_u8ACamID = u8Count;
	}

	VOID SetMeasureMode(MeasureRegion region) { measureRegion = region; }
	VOID SetStartIndex(int nStartNum) { m_nStartIndex = nStartNum; }
	int GetStartIndex() { return m_bUseCamDrv; }


	

	////////////////////////////////////////////////////////////////////

	CPoint CalcPoint(UINT32 u32Index, UINT8 u8StartPoint, UINT8 u8Dir, CPoint cpMaxGridSize);
	CDPoint CalcPosition(CDPoint dpStartPos, double dAngle, double dPitch, CPoint cpPoint);

	////////////////////////////////////////////////////////////////////

	/* 내부 대기동작으로 인해 쓰레드를 제외한 UI에서 호출을 금지한다. */
	BOOL Measurement(HWND hHwnd = NULL);
	////////////////////////////////////////////////////////////////////

	VOID Initialize();
	VOID Terminate();

	BOOL SetRegistModel();

	BOOL IsProcessWorking() { return m_bRunnigThread; }

	VOID MeasureStart(HWND hHwnd = NULL);
	VOID SetStopProcess() { m_bStop = TRUE; }
	BOOL IsStopProcess() { return m_bStop; }

	int GetPointCount() { return (int)m_stVctTable.size(); }

	BOOL SetPointErrValue(int nIndex, double dX, double dY);
	BOOL GetPointErrValue(int nIndex, double& dX, double& dY);
	BOOL GetPointPosition(int nIndex, double& dX, double& dY);
	BOOL GetPointParam(int nIndex, ST_ACCR_PARAM& stParam);
};

UINT MeasureThread(LPVOID lpParam);