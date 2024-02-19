
#pragma once

#include "../../inc/comn/ThinThread.h"

#define MOTION_CALI_CENTERING_MIN	2	/* ����: 0.1 um or 100 nm, ex> 2 --> 0.2 um */

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

	UINT8				m_u8WorkStep;			/* ���� �۾� ���� */
	UINT8				m_u8WorkTotal;			/* ��ü �۾� ���� */
	UINT16				m_u16PrintCount;		/* �뱤 Ƚ�� ���� */
	UINT16				m_u16PrintTotal;		/* �뱤 ��ü Ƚ�� */

	BOOL				m_bReqPeriodMC2;
	BOOL				m_bRunPrint;			/* �뱤 ���� ���� ���� �÷��� */

	UINT64				m_u64PeridPktTime;

	DOUBLE				m_dbExpoX;				/* �뱤 ���� ��ġ (��� ��ġ) (����: um) */
	DOUBLE				m_dbExpoY;				/* �뱤 ���� ��ġ (��� ��ġ) (����: um) */
	DOUBLE				m_dbPeriodY;			/* �Ź� �뱤 ������ Y �� �������� �̵� �Ÿ� (unit: um) */
	DOUBLE				m_dbPeriodZ;			/* �Ź� �뱤 ������ PH Z �� Up/Down �̵� �Ÿ� (unit: um) */

	DOUBLE				m_dbPhBaseZ[MAX_PH];	/* ��� Photohead Z ���� �⺻ ���� ��ġ (����: um) (range: 63000 ~ 85000) */

	UINT64				m_u64DelayTime;			/* �� �ܰ� ó�� �������� ���� �ð� ���� */
	UINT64				m_u64WaitTime;			/* ���� �۾��� ���� �Ϸ�� ������ �ּ����� ��� �ð� �� (����: ms) */
	UINT64				m_u64SendTime;			/* �ֱ������� ��ɾ� �����ϴµ�, ���� �ֱٿ� ��ɾ� ������ �ð� ���� (����: ms) */
	UINT64				m_u64StepTime;			/* Ư�� �ð� ���� �ش� ��ɾ ���������� ���۵��� �ʴ´ٸ�, �� �ð� �������� �ٽ� �ش� ����� �����ϱ� ���� */

	ENG_JWNS			m_enWorkState;			/* 0x00 : none, 0x01 : wait, 0x02 : next, 0x03 : complete, 0xff : error */

	CMySection			m_syncPrint;


/* ���� �Լ� */
protected:

	VOID				SendMesgNormal(UINT32 timeout=100);
	VOID				SendMesgPrint(BOOL flag, UINT32 timeout=100);

	VOID				RunPrint();
	VOID				SetWorkNext();
	BOOL				IsWorkTimeOut();
	BOOL				IsPhZAxisMoved(UINT8 ph_no, DOUBLE pos);

	VOID				ReqPeriodPkt();

	VOID				SetWorkWaitTime(UINT64 time);
	ENG_JWNS			IsWorkWaitTime();

	ENG_JWNS			SetExpoStartXY();
	ENG_JWNS			IsExpoStartXY();

	ENG_JWNS			SetPhZAxisHoming();
	ENG_JWNS			IsPhZAxisHoming();
	ENG_JWNS			IsPhZAxisHomed();

	ENG_JWNS			SetPhZAxisMoving();
	ENG_JWNS			IsPhZAxisMoved();

	ENG_JWNS			SetExpoReady();
	ENG_JWNS			IsExpoReady();

	ENG_JWNS			SetExpoPrinting();
	ENG_JWNS			IsExpoPrinted();


/* ���� �Լ� */
public:

	/* Focus ���� ���� �Ű� ���� �� ���� */
	VOID				StartPrint(DOUBLE expo_x, DOUBLE expo_y,
								   DOUBLE period_z, DOUBLE period_y, UINT16 repeat);
	VOID				StopPrint();
	BOOL				IsCompleted();
	BOOL				IsRunPrint()		{	return m_bRunPrint;	}

	UINT16				GetStepCount();
	UINT16				GetPrintCount()		{	return m_u16PrintCount;	}
	UINT16				GetPrintTotal()		{	return m_u16PrintTotal;	}
	UINT16				GetPrintRest()		{	return m_u16PrintTotal - m_u16PrintCount;	}
	UINT8				GetStepTotal()		{	return m_u8WorkTotal;	}
};