
#pragma once

#include "../WorkStep.h"

class CWorkOnlyFEM : public CWorkStep
{
/* 생성자 & 파괴자 */
public:

	CWorkOnlyFEM();
	virtual ~CWorkOnlyFEM();

/* 가상함수 재정의 */
protected:
public:

	virtual	BOOL		InitWork();
	virtual VOID		DoWork();


/* 로컬 변수 */
protected:


	UINT8				m_u8WaitLoaded;

	UINT64				m_u64TickLoaded;

	// by sysandj
	unsigned long		m_lastUniqueID;

/* 로컬 함수 */
protected:

	VOID				SetWorkNext();
	VOID				CalcStepRate();

// #if 1
// 	ENG_JWNS			IsGerberJobLoaded();
// #endif

/* 공용 함수 */
public:

};
