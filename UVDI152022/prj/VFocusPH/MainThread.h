
#pragma once

#include "../../inc/comn/ThinThread.h"

class CMeasure;

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

	UINT8				m_u8WorkStep;		/* �۾� ���� */
	UINT8				m_u8WorkTotal;
	UINT8				m_u8PhNext;			/* ���� �����ϰ� �ִ� Phothead Number (1 ~ 8) */

	UINT16				m_u16RetryGrab;		/* Grabbed Image ĸó Ƚ�� */
	UINT16				m_u16StepYTotal;	/* �� Y ������ �̵��ϴ� Ƚ�� */
	UINT16				m_u16StepYNext;		/* ���� ���� ���� Y �� ���� �ε��� (1 ~ N) */
	UINT16				m_u16RepeatTotal;	/* �ѹ� ������ ������, ���������� �����ϴ� �ݺ� Ƚ�� (1 ~ N) */
	UINT16				m_u16RepeatNext;	/* ���� �ݺ� �����ϰ� �ִ� Ƚ�� (1 ~ N) */
	
	BOOL				m_bRunFocus;		/* Focus ���� ���� ���� �÷��� */

	UINT64				m_u64DelayTime;		/* ���� �۾��� �Ϸ�Ǵµ����� ����ϴ� �ð� (����: msec) */
	UINT64				m_u64GrabTime;		/* �˻� ������ �ð� ���� */
	UINT64				m_u64WaitTime;
	UINT64				m_u64PeridPktTime;

	ENG_JWNS			m_enWorkState;		/* 0x00 : none, 0x01 : wait, 0x02 : next, 0x03 : complete, 0xff : error */

	CMySection			m_syncFocus;
	CMeasure			*m_pMeasure;		/* ���� �����Ͱ� �ӽ� ����� ��ü */


/* ���� �Լ� */
protected:

	VOID				SetWorkWaitTime(UINT64 time);
	ENG_JWNS			IsWorkWaitTime();

	VOID				SendMesgNormal(UINT32 timeout=100);
	VOID				SendMesgResult(WPARAM result, UINT32 timeout=100);

	VOID				RunFocus();
	VOID				SetWorkNext();
	BOOL				IsWorkTimeOut();
	VOID				ReqPeriodPkt();

	ENG_JWNS			SetMeasureMoving();
	ENG_JWNS			IsMeasureMoved();

	ENG_JWNS			PutOneTrigger();
	ENG_JWNS			ProcFindModel();
	ENG_JWNS			ResultDetectImage();


/* ���� �Լ� */
public:

	/* Focus ���� ���� �Ű� ���� �� ���� */
	VOID				StartFocus(UINT16 step_s, UINT16 repeat, DOUBLE step_y);
	VOID				StopFocus();
	BOOL				IsCompleted();

	LPG_ZAAL			GetGrabData(UINT8 ph_no, UINT32 index);
	LPG_ZAAL			GetLastGrabData();
	CMeasure			*GetMeasure();

	/* Focus ���� ���� ���� */
	BOOL				IsRunFocus()	{	return m_bRunFocus;	}

	/* ������� �۾� ������ ��ȯ */
	DOUBLE				GetWorkStepRate();
};