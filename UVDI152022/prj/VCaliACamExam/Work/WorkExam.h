#pragma once

#include "Work.h"

class CWorkExam : public CWork
{
/* 생성자 & 파괴자 */
public:

	CWorkExam();
	virtual ~CWorkExam();

/* 가상함수 선언 */
protected:
public:

/* 로컬 변수 */
protected:

	UINT64				m_u64MovedTick;		/* Motion 이동 후 안정화 될 때까지 최소 대기 시간 1.5초 정도 쉬어야 됨 */


/* 로컬 함수 */
protected:

	VOID				SetWorkNext();

	ENG_JWNS			PutOneTrigger();
	ENG_JWNS			GetGrabResult();
	ENG_JWNS			SetMovingCols();
	ENG_JWNS			IsMovedCols();
	ENG_JWNS			SetMovingRows();
	ENG_JWNS			IsMovedRows();


/* 공용 함수 */
public:

	BOOL				InitWork();
	VOID				DoWork();

	LPG_ACGR			GetGrabbedResult(UINT8 index);
};