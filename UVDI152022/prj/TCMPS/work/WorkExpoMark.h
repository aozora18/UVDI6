
#pragma once

#include "Work.h"

class CWorkExpoMark : public CWork
{
/* 생성자 & 파괴자 */
public:

	CWorkExpoMark(ENG_AOEM mode);
	virtual ~CWorkExpoMark();

/* 가상함수 재정의 */
protected:
public:
	virtual VOID		RunWork();


/* 로컬 변수 */
protected:



/* 로컬 함수 */
protected:

	VOID				SetWorkNext();



/* 공용 함수 */
public:

	BOOL				InitWork();

};
