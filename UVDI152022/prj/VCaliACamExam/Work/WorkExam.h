#pragma once

#include "Work.h"

class CWorkExam : public CWork
{
/* ������ & �ı��� */
public:

	CWorkExam();
	virtual ~CWorkExam();

/* �����Լ� ���� */
protected:
public:

/* ���� ���� */
protected:

	UINT64				m_u64MovedTick;		/* Motion �̵� �� ����ȭ �� ������ �ּ� ��� �ð� 1.5�� ���� ����� �� */


/* ���� �Լ� */
protected:

	VOID				SetWorkNext();

	ENG_JWNS			PutOneTrigger();
	ENG_JWNS			GetGrabResult();
	ENG_JWNS			SetMovingCols();
	ENG_JWNS			IsMovedCols();
	ENG_JWNS			SetMovingRows();
	ENG_JWNS			IsMovedRows();


/* ���� �Լ� */
public:

	BOOL				InitWork();
	VOID				DoWork();

	LPG_ACGR			GetGrabbedResult(UINT8 index);
};