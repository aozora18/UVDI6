
#pragma once

#include "Work.h"

class CWorkMarkMove : public CWork
{
/* 생성자 & 파괴자 */
public:

	CWorkMarkMove();
	virtual ~CWorkMarkMove();

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

	BOOL				InitWork(UINT16 mark_no);

};
