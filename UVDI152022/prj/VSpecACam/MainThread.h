
#pragma once

#include "../../inc/comn/ThinThread.h"

#define MOTION_CALI_CENTERING_MIN	0.0002f	/* ����: mm. 0.2 um */

class CMainThread : public CThinThread
{
/* ������ / �ı��� */
public:
	CMainThread(HWND parent);
	virtual ~CMainThread();

/* ���� �Լ� ������ */
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();

/* ���� ���� */
protected:

	UINT8				m_u8RunMode;		/* 0 : ī�޶� ȸ�� ���� �� ����, 1 : ī�޶� �ȼ� ũ�� �� ���� */
	UINT8				m_u8ACamID;			/* 1 or Later */
	UINT8				m_u8WorkStep;		/* ���� �۾� ���� */
	UINT8				m_u8WorkTotal;		/* ��ü �۾� ���� */
	UINT8				m_u8GrabbedCount;	/* ���� ���� Ƚ�� ���� */

	UINT32				m_u32RepeatTotal;	/* �� �ݺ��� Ƚ�� */
	UINT32				m_u32RepeatCount;	/* ���� �ݺ� ���� Ƚ�� */

	BOOL				m_bRunSpec;			/* Focus ���� ���� ���� �÷��� */

	DOUBLE				m_dbDiffCamX;		/* Mark �̹����� �߽ɿ��� ������ Align Camera ���� (unit: mm) */
	DOUBLE				m_dbDiffStageY;		/* Mark �̹����� �߽ɿ��� ������ Stage Y ���� (unit: mm) */

	UINT64				m_u64TickGrabbed;	/* �˻� ������ �ð� ���� */
	UINT64				m_u64DelayTime;		/* �� �ܰ� ó�� �������� ���� �ð� ���� */
	UINT64				m_u64PeridPktTime;

	DOUBLE				m_dbRateLast;		/* ���� �ֱ� ����� �� �ӽ� ���� */

	ENG_JWNS			m_enWorkState;		/* 0x00 : none, 0x01 : wait, 0x02 : next, 0x03 : complete, 0xff : error */

	/* Align Camera 1/2 ���� ��/�� �̵��� �ȼ� ��ǥ �� */
	STG_DBXY			m_stGrabCentXY[2];	/* �� ī�޶� ���� 2 ���� Grabbed Image�� Mark �߽� ��ǥ�� ����� ����ü */
	STG_DBXY			m_stGrabMoveXY[2];	/* �� ī�޶� ���� 2 ���� Grabbed Image�� Mark �̵� ��ǥ�� ����� ����ü */

	CMySection			m_syncSpec;


/* ���� �Լ� */
protected:

	VOID				SendMesgNormal(UINT32 timeout=100);
	VOID				SendMesgResult(BOOL flag, UINT32 timeout=100);
	VOID				SendMesgGrabView(UINT32 timeout=100);

	VOID				RunSpec();
	VOID				SetWorkNext();
	BOOL				IsWorkTimeOut();
	VOID				ReqPeriodPkt();

	ENG_JWNS			SetACamMovingSide();
	ENG_JWNS			IsACamMovedSide();

	ENG_JWNS			SetACamMovingZAxisQuartz();
	ENG_JWNS			IsACamMovedZAxisQuartz();

	ENG_JWNS			SetACamMovingQuartz();
	ENG_JWNS			IsACamMovedQuartz();

	ENG_JWNS			SetTrigEnabled();
	ENG_JWNS			IsTrigEnabled();

	ENG_JWNS			GetGrabQuartzData();
	ENG_JWNS			IsGrabbQuartzData();

	ENG_JWNS			SetMotionMoving();
	ENG_JWNS			IsMotionMoved();

	ENG_JWNS			SetMotionMovingX();
	ENG_JWNS			IsMotionMovedX();

	ENG_JWNS			IsGrabbQuartzMoveX();

	VOID				CalcACamSpec(BOOL flag);


/* ���� �Լ� */
public:

	/* Focus ���� ���� �Ű� ���� �� ���� */
	VOID				StartSpec(UINT8 run_mode, UINT8 cam_id, UINT32 repeat);
	VOID				StopSpec();
	BOOL				IsCompleted();
	BOOL				IsRunSpec()		{	return m_bRunSpec;	}

	DOUBLE				GetStepRate();

	/* ������� ������ ���� ��ȯ */
	UINT32				GetGrabCount();

	DOUBLE				GetMinMax(UINT8 mode, UINT8 type);

	UINT32				GetMeasureCount()	{	return m_u32RepeatCount+1;	}
};