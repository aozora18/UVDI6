#pragma once
#include "../../../../inc/conf/vision_uvdi15.h"

class CFlushErrorMgr
{
/* ������ & �ı��� */
public:

	CFlushErrorMgr();
	virtual ~CFlushErrorMgr();

	/* Singleton pattern */
	static CFlushErrorMgr* GetInstance()
	{
		static CFlushErrorMgr _inst;
		return &_inst;
	}

/* ������ */
protected:

	enum EnMeasureStep
	{
		eMEASURE_GRABBED_IMAGE = 0,
		eMEASURE_MOVE_MOTION,
		eMEASURE_MAX
	};

	/* �����ѹ� ��� */
	enum EnFlushError
	{
		eMODEL_FIND_COUNT = 2
// 		eCOUNT_OF_PROCESS = 5,
// 		eMAX_PROGRESS = 13,
// 
// 		eLAGRANGE_DATA_COUNT = 3
	};


/* ���� ���� */
protected:

	BOOL				m_bStop;				/* ���� �÷��� */
	UINT8				m_u8CurStep;
	UINT8				m_u8MaxStep;
	BOOL				m_bResultSucc;
	LPG_ACGR			m_pstGrabData;

	vector<double>		m_dVctStripeWidth;

	CWinThread*			m_pMeasureThread;		/* ���� ������ */
	BOOL				m_bRunnigThread;		/* ������ ���� ���� �ľ�*/


/* ���� �Լ� */
protected:

	VOID Wait(int nTime);
	
	BOOL GrabbedImage(double& dCenterMove);
	BOOL MoveMotion(double dCenterMove, int nTimeOut = 30000, double dDiffDistance = 0.001);
	

/* ���� �Լ� */
public:
	/* ���� ��⵿������ ���� �����带 ������ UI���� ȣ���� �����Ѵ�. */
	BOOL Measurement(HWND hHwnd = NULL);
	////////////////////////////////////////////////////////////////////

	VOID Initialize();
	VOID Terminate();
	BOOL SetRegistModel();
	BOOL LoadTable();

	LPG_ACGR GetGrabData() { return m_pstGrabData; }

	VOID SetCurStep(UINT8 u8SetStep) { m_u8CurStep = (u8SetStep > 0x01) ? u8SetStep : 0x01; }
	VOID SetMaxStep(UINT8 u8SetStep) { m_u8MaxStep = (u8SetStep > 0x01) ? u8SetStep : 0x01; }

	BOOL IsProcessWorking() { return m_bRunnigThread; }

	VOID MeasureStart(HWND hHwnd = NULL);
	VOID SetStopProcess() { m_bStop = TRUE; }
// 	VOID MeasureEnd();
};

UINT FlushMeasureThread(LPVOID lpParam);