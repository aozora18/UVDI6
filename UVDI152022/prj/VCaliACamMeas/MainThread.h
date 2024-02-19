
#pragma once

#include "../../inc/comn/ThinThread.h"

class CWorkMeas;

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

	BOOL				m_bRunCali;		/* ���� ���� */
	BOOL				m_bWorking;		/* �۾� ���� �������� */

	CWorkMeas			*m_pWorkMeas;	/* Align Calibration 2D XY ���� ���� */

	CMySection			m_syncCali;

/* ���� �Լ� */
protected:

	VOID				SendMesgParent(UINT16 msg_id, UINT32 timeout=100);

	VOID				DoWorkMeas();

/* ���� �Լ� */
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