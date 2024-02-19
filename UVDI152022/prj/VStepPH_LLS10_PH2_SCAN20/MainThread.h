
#pragma once

#include "../../inc/comn/ThinThread.h"

#define MOTION_CALI_CENTERING_MIN	0.0002f	/* 단위: mm. 0.2 um */

class CMainThread : public CThinThread
{
/* 생성자 / 파괴자 */
public:
	CMainThread(HWND parent, LPG_ACGR grab);
	virtual ~CMainThread();

/* 가상 함수 재정의 */
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();

/* 로컬 변수 */
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

/* 로컬 함수 */
protected:

	BOOL				PutOneTrigger();
	BOOL				GrabbedImage();
	BOOL				MotionMoving();
	BOOL				IsMotionMoved();

	BOOL				DoMeasure();

	VOID				SendMesgNormal(UINT32 timeout=100);
	VOID				SendMesgResult(BOOL result, UINT32 timeout=100);

	VOID				SetWaitTime(UINT64 wait);


/* 공용 함수 */
public:

	BOOL				IsWorking()	{	return m_bRunMeasure;	}
	VOID				RunMeasure(BOOL run);
	VOID				ResetMeasure()	{	m_u8GridCount	= 0x00;	}
	VOID				SetGridCheck(UINT8 check)	{	m_u8GridCheck = check; }
};