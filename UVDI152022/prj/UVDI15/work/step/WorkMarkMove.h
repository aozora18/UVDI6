
#pragma once

#include "../WorkStep.h"

class CWorkMarkMove : public CWorkStep
{
/* 생성자 & 파괴자 */
public:

	CWorkMarkMove(UINT8 mark_no);
	virtual ~CWorkMarkMove();

/* 가상함수 재정의 */
protected:
public:

	virtual	BOOL		InitWork();
	virtual VOID		DoWork();


/* 로컬 변수 */
protected:

	UINT8				m_u8MarkNo;		/* 이동하려는 Global Mark Number 위치 (Zero-based) */
	UINT8				m_u8ACamID;		/* 1 or 2 */


/* 로컬 함수 */
protected:

	/* 1번째 Align Mark로 위치로 이동 */
	ENG_JWNS			SetMovingAlignMark();
	ENG_JWNS			IsMovedAlignMark();

/* 공용 함수 */
public:

};
