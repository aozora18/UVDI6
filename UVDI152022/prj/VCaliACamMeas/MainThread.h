
#pragma once

#include "../../inc/comn/ThinThread.h"

class CWorkMeas;

class CMainThread : public CThinThread
{
/* 생성자 / 파괴자 */
public:
	CMainThread(HWND parent);
	virtual ~CMainThread();

/* 가상 함수 재정의 */
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();

/* 로컬 변수 */
protected:

	BOOL				m_bRunCali;		/* 동작 여부 */
	BOOL				m_bWorking;		/* 작업 진행 상태인지 */

	CWorkMeas			*m_pWorkMeas;	/* Align Calibration 2D XY 오차 측정 */

	CMySection			m_syncCali;

/* 로컬 함수 */
protected:

	VOID				SendMesgParent(UINT16 msg_id, UINT32 timeout=100);

	VOID				DoWorkMeas();

/* 공용 함수 */
public:

	VOID				StartCali(UINT8 cam_id, UINT16 thick, LPG_ACCS meas);
	VOID				StopCali();
	/* The GrabbedResult for inspection mode */
	LPG_ACGR			GetGrabbedResult(UINT8 index);
	VOID				GetCurRowCol(INT32 &row, INT32 &col);
	VOID				GetCurOffset(DOUBLE &offset_x, DOUBLE &offset_y);

	BOOL				IsRunWork()	{	return m_bWorking;	}

	DOUBLE				GetProcRate();
	UINT64				GetWorkTime();
};