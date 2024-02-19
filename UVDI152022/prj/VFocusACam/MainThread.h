
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

	UINT8				m_u8ACamID;			/* 1 or Later */
	UINT8				m_u8WorkStep;		/* �۾� ���� */
	UINT8				m_u8WorkTotal;
	UINT8				m_u8RetryTrig;		/* Ʈ���� �߻� ��õ� Ƚ�� */
	
	BOOL				m_bRunFocus;		/* Focus ���� ���� ���� �÷��� */

	UINT32				m_u32SkipCount;		/* �̵� �� LED�� ����ȭ �� ������ �׽�Ʈ�� �����ϴ� Ƚ�� (�м� ���� ���� �ȵ�) */
	UINT32				m_u32RepCount;		/* �̵� �� ����ȭ �ĺ��� ���������� ������ �ݺ��Ǵ� Ƚ�� (��� ���� ���ϱ� ����) */
	UINT32				m_u32TotCount;		/* �� �̵� Ƚ�� ��, Step (Up/Down)�� �� �� �̵��ؾ����� Ƚ�� �� */
	
	UINT32				m_u32MeasRep;		/* ���� �ݺ� ���� Ƚ�� */
	UINT32				m_u32MeasTot;		/* ���� Step (Up/Down)�� �� �� �̵� �ߴ��� Ƚ�� */
	UINT32				m_u32MeasSkip;		/* ����ȭ ���� Ƚ�� ��, Skip ����� Ƚ�� */

	UINT64				m_u64GrabTime;		/* �˻� ������ �ð� ���� */
	UINT64				m_u64DelayTime;		/* �� �ܰ� ó�� �������� ���� �ð� ���� */

	DOUBLE				m_dbInitPosZ;		/* Z Axis ���� ���� ���� ��ġ (����: mm) */
	DOUBLE				m_dbStepSize;		/* �̵� ���� (unit: 100 nm or 0.1 um) */
	DOUBLE				m_dbMeasPosZ;		/* ������ �̵��ؾ� �� ��ġ */

	ENG_MDMD			m_enUpDown;			/* ��/�� �̵� ���� �� (1:Up or 2:Down) */
	ENG_JWNS			m_enWorkState;		/* 0x00 : none, 0x01 : wait, 0x02 : next, 0x03 : complete, 0xff : error */

	CMySection			m_syncFocus;
	CMeasure			*m_pMeasure;		/* ���� �����Ͱ� �ӽ� ����� ��ü */


/* ���� �Լ� */
protected:

	VOID				SendMesgNormal(UINT32 timeout=100);
	VOID				SendMesgResult(UINT32 timeout=100);

	VOID				RunFocus();
	VOID				SetWorkNext();
	BOOL				IsWorkTimeOut();

	ENG_JWNS			ACamZHoming();
	ENG_JWNS			IsACamZHomed();
	ENG_JWNS			PutOneTrigger();
	ENG_JWNS			ProcDetectImage();
	ENG_JWNS			ResultDetectImage();
	ENG_JWNS			SetMoveStep();
	ENG_JWNS			IsMovedStep();


/* ���� �Լ� */
public:

	/* Focus ���� ���� �Ű� ���� �� ���� */
	VOID				StartFocus(UINT8 cam_id,
								   DOUBLE step,
								   UINT32 skip,
								   UINT32 repeat,
								   UINT32 total);
	VOID				StopFocus();
	BOOL				IsCompleted();

	/* ������� ������ ���� ��ȯ */
	UINT32				GetGrabCount();
	LPG_ZAAL			GetGrabData(UINT32 index);
	LPG_ZAAL			GetGrabSel();

	CMeasure			*GetMeasure();
	DOUBLE				GetMinMax(UINT8 mode, UINT8 type);

	/* Focus ���� ���� ���� */
	BOOL				IsRunFocus()	{	return m_bRunFocus;	}

	/* ������� �۾� ������ ��ȯ */
	DOUBLE				GetWorkStepRate();
};