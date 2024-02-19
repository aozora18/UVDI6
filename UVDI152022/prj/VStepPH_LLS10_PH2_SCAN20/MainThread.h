
#pragma once

#include "../../inc/comn/ThinThread.h"

#define MOTION_CALI_CENTERING_MIN	0.0002f	/* ����: mm. 0.2 um */

class CMainThread : public CThinThread
{
/* ������ / �ı��� */
public:
	CMainThread(HWND parent, LPG_ACGR grab);
	virtual ~CMainThread();

/* ���� �Լ� ������ */
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();

/* ���� ���� */
protected:

	UINT8				m_u8Step;
	UINT8				m_u8GridCheck;
	UINT8				m_u8GridCount;

	BOOL				m_bResultSucc;
	BOOL				m_bRunMeasure;

	UINT64				m_u64WorkTime;
	UINT64				m_u64WaitTime;
	UINT64				m_u64StepTime;
	UINT64				m_u64ReqTime;

	DOUBLE				m_dbCentMove;

	LPG_ACGR			m_pstGrabData;

	CMySection			m_syncRun;

/* ���� �Լ� */
protected:

	BOOL				PutOneTrigger();
	BOOL				GrabbedImage();
	BOOL				MotionMoving();
	BOOL				IsMotionMoved();

	BOOL				DoMeasure();

	VOID				SendMesgNormal(UINT32 timeout=100);
	VOID				SendMesgResult(BOOL result, UINT32 timeout=100);

	VOID				SetWaitTime(UINT64 wait);


/* ���� �Լ� */
public:

	BOOL				IsWorking()	{	return m_bRunMeasure;	}
	VOID				RunMeasure(BOOL run);
	VOID				ResetMeasure()	{	m_u8GridCount	= 0x00;	}
	VOID				SetGridCheck(UINT8 check)	{	m_u8GridCheck = check; }
};