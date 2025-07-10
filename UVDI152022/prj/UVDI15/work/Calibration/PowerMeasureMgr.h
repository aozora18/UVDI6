#pragma once

// #define LED_SIMUL				1
// #define	MOTOR_SIMUL				1
#define MOTOR_MOVE_DELAY		500
#define LIGHT_PULSE_DURATION	500


typedef struct stMeasureParam
{
	UINT8 u8Row;
	UINT8 u8Col;
	double dPower;

} ST_MEASURE_PARAM;
typedef struct stMovePos
{
	double dX;
	double dY;
	double dZ;

	stMovePos(double _dX = 0, double _dY = 0, double _dZ = 0)
	{
		dX = _dX;
		dY = _dY;
		dZ = _dZ;
	}
} ST_MOVE_POS;
typedef struct stLedValue
{
	UINT16 u16Index;
	double dPower;

	stLedValue()
	{
		u16Index = 0;
		dPower = 0;
	}
} ST_LED_VALUE;
typedef struct stWorkProgress
{
	int nRate;
	CString strDescription;

	stWorkProgress()
	{
		nRate = 0;
		strDescription = _T("");
	}
} ST_WORK_PROGRESS;

typedef std::vector<ST_LED_VALUE> VCT_LED_TABLE;
typedef std::vector<VCT_LED_TABLE> VCT_HEAD_TABLE;
typedef std::vector<VCT_HEAD_TABLE> VCT_POWER_TABLE;


class CPowerMeasureMgr
{
/* ������ & �ı��� */
public:

	CPowerMeasureMgr();
	virtual ~CPowerMeasureMgr();

	/* Singleton pattern */
	static CPowerMeasureMgr* GetInstance()
	{
		static CPowerMeasureMgr _inst;
		return &_inst;
	}

/* ������ */
protected:

	/* �����ѹ� ��� */
	enum EnPowerMeasure
	{
		ePOS_X = 0,
		ePOS_Y,
		ePOS_Z,

		eCOUNT_OF_PROCESS = 5,
		eMAX_PROGRESS = 13,

		eLAGRANGE_DATA_COUNT = 3
	};


/* ���� ���� */
protected:

	BOOL				m_bStop;				/* ���� �÷��� */

	UINT8				m_u8StartHead;			/* ���� ���� �� PhotoHead */
	UINT8				m_u8StartLed;			/* ���� ���� �� LED */

	UINT8				m_u8Head;				/* ���õ� PhotoHead */
	UINT8				m_u8Led;				/* ���õ� LED */
	UINT16				m_u16PowerIndex;		/* LED On/Off Index �� */

	BOOL				m_bUseAllPH;			/* ��� ��� ���� ���� */
	BOOL				m_bUseAllWave;			/* ��� ���� ���� ���� */
	BOOL				m_bUseMaxIll;			/* �ִ� ���� ��ġ �̵���� ��뿩�� */
	UINT8				m_u8MaxPoints;			/* ���� �ִ� ����Ʈ�� �� */
	double				m_dPitchXY;				/* �� ����Ʈ�� ������ �Ÿ� �� */
	double				m_dPitchZ;				/* �� ����Ʈ�� ������ �Ÿ� �� */

	VCT_LED_TABLE		m_stVctMeasureTable;	/* ���� ���̺� */

	CWinThread*			m_pMeasureThread;		/* ���� ������ */
	BOOL				m_bRunnigThread;		/* ������ ���� ���� �ľ�*/
	UINT16				m_u16Index;				/* ���� ���� �ܰ� */

	VCT_POWER_TABLE		m_stVctPowerTable;		/* ��ü ���� ���̺� [HEAD NO][LED NO][MEASURE STEP]*/

	int					m_nRecipeNum;			/* ������ ���� �� ������ ��ȣ */

/* ���� �Լ� */
protected:

	VOID Wait(int nTime);
	BOOL BoundsCheckingPowerTable(UINT8 u8Head, UINT8 u8Led);

	VOID LoadTableFile(UINT8 u8Head);
	BOOL SaveHeadTableFile(UINT8 u8Head);
	BOOL MeasureToPowerTable();
	VOID ClearPowerTable();

	ST_MOVE_POS CalcXYMovePos(double dStandardX, double dStandardY, UINT8 u8Row, UINT8 u8Col, UINT8 u8MaxPoint, double dPitch);
	BOOL StageMoveXY(double dX, double dY, double dSpeed);
	BOOL IsStageMovedXY(double dTargetX, double dTargetY, int nTimeOut = 60000, double dDiffDistance = 0.005);

	BOOL HeadMoveZ(UINT8 u8Head, double dZ, double dSpeed);
	BOOL IsHeadMovedZ(UINT8 u8Head, double dTargetZ, int nTimeOut = 60000, double dDiffDistance = 0.005);
	BOOL HeadMovedZWhileMeasure(UINT8 u8Head, double dTargetZ, double &dZ, int nSamplingTime = 10, int nTimeOut = 60000, double dDiffDistance = 0.005);

	int GetProgressPercent(UINT16 u16Index);

	BOOL FindMaxIllumPos(HWND hHwnd = NULL);
	BOOL FindFocusPos(HWND hHwnd = NULL);
	BOOL PowerMeasure(HWND hHwnd = NULL);
	BOOL RecipeMeasure(int nHead, HWND hHwnd = NULL);

/* ���� �Լ� */
public:

	VOID Initialize();
	VOID Terminate();

	VOID SetStopProcess() { m_bStop = TRUE; }
	BOOL IsProcessWorking() { return m_bRunnigThread; }

	VOID SetMoveAllPH(BOOL bUseAllPH) { m_bUseAllPH = bUseAllPH; }
	BOOL GetMoveAllPH() { return m_bUseAllPH; }
	VOID SetMoveAllWave(BOOL bUseAllWave) { m_bUseAllWave = bUseAllWave; }
	BOOL GetMoveAllWave() { return m_bUseAllWave; }
	VOID SetMoveMaxIll(BOOL bUseMaxIll) { m_bUseMaxIll = bUseMaxIll; }
	BOOL GetMoveMaxIll() { return m_bUseMaxIll; }

	VOID SetManualIndexPower(UINT16 u16PowerIndex) { m_u16PowerIndex = u16PowerIndex; }
	UINT16 GetManualIndexPower() { return m_u16PowerIndex; }

	VOID SetPowerTable(UINT16 u16Count, UINT16 u16StartIndex, UINT16 u16StepIndex);
	VOID SetPhotoHeadOption(UINT8 u8Head, UINT8 u8Led);

	VOID SetFindPosParam(UINT8 u8MaxPoints, double dPitchXY, double dPitchZ);

	VCT_LED_TABLE* GetMeasureTable() { return &m_stVctMeasureTable; }

	BOOL PhotoLedOnOff(UINT8 u8Head, UINT8 u8Led, UINT16 u16PwrIndex, BOOL bOn);
	VOID AllPhotoLedOff();

	VOID SetRecipeNum(BOOL nRecipeNum) { m_nRecipeNum = nRecipeNum; }
	BOOL GetRecipeNum() { return m_nRecipeNum; }

	/* ���� ��⵿������ ���� �����带 ������ UI���� ȣ���� �����Ѵ�. */
	BOOL Measurement(HWND hHwnd = NULL);
	BOOL CheckRecipe(HWND hHwnd = NULL);
	////////////////////////////////////////////////////////////////////

	BOOL MeasureStart(HWND hHwnd = NULL);
	VOID MeasureEnd();
	void SaveAllHeadtableFile();

	BOOL RecipeCheckStart(HWND hHwnd = NULL);

	/////////////////////////////////////////////////////////////////////////////////////
	UINT16 GetPowerIndex(UINT8 u8Head, UINT8 u8Led, double dPower);

	double GetStableValue(UINT8 u8Led, double dAccuracy = 0.001, int nTimeOut = 60000);

	BOOL GetMaxValue(UINT8 u8Head, UINT8 u8Led, UINT16 & u16Index, double &dPower);
	VCT_POWER_TABLE	GetPowerTable() { return m_stVctPowerTable; }
};

UINT PowerMeasureThread(LPVOID lpParam);
UINT RecipeCheckThread(LPVOID lpParam);