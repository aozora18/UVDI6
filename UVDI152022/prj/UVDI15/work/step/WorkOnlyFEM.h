
#pragma once

#include "../WorkStep.h"

class CWorkOnlyFEM : public CWorkStep
{
/* ������ & �ı��� */
public:

	CWorkOnlyFEM();
	virtual ~CWorkOnlyFEM();

/* �����Լ� ������ */
protected:
public:

	virtual	BOOL		InitWork();
	virtual VOID		DoWork();


/* ���� ���� */
protected:


	UINT8				m_u8WaitLoaded;

	UINT64				m_u64TickLoaded;

	// by sysandj
	unsigned long		m_lastUniqueID;

/* ���� �Լ� */
protected:

	VOID				SetWorkNext();
	VOID				CalcStepRate();

// #if 1
// 	ENG_JWNS			IsGerberJobLoaded();
// #endif

/* ���� �Լ� */
public:

};
