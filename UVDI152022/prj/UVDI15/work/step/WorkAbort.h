
#pragma once

#include "../WorkStep.h"

class CWorkAbort : public CWorkStep
{
/* ������ & �ı��� */
public:

	CWorkAbort(UINT8 forced=0x00);
	virtual ~CWorkAbort();

/* �����Լ� ������ */
protected:
public:

	virtual	BOOL		InitWork();
	virtual VOID		DoWork();


/* ���� ���� */
protected:

	UINT8				m_u8AbortForced;

	UINT64				m_u64TickAbortWait;	/* ��ð� ��⸦ ���߱� ���� */


/* ���� �Լ� */
protected:

	ENG_JWNS			StopPrinting();		/* ���� ���� �뱤 ���̸�, ������ �뱤 ���� ��Ŵ */
	ENG_JWNS			IsStopPrinting();	/* ���� �뱤 ������ ���� Ȯ�� */


/* ���� �Լ� */
public:
	VOID				PhilInterrupsStop();

};
