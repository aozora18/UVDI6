
#pragma once

#include "../WorkStep.h"

class CWorkMarkMove : public CWorkStep
{
/* ������ & �ı��� */
public:

	CWorkMarkMove(UINT8 mark_no);
	virtual ~CWorkMarkMove();

/* �����Լ� ������ */
protected:
public:

	virtual	BOOL		InitWork();
	virtual VOID		DoWork();


/* ���� ���� */
protected:

	UINT8				m_u8MarkNo;		/* �̵��Ϸ��� Global Mark Number ��ġ (Zero-based) */
	UINT8				m_u8ACamID;		/* 1 or 2 */


/* ���� �Լ� */
protected:

	/* 1��° Align Mark�� ��ġ�� �̵� */
	ENG_JWNS			SetMovingAlignMark();
	ENG_JWNS			IsMovedAlignMark();

/* ���� �Լ� */
public:

};
