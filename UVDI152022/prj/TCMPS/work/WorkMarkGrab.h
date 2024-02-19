
#pragma once

#include "Work.h"

class CWorkMarkGrab : public CWork
{
/* 생성자 & 파괴자 */
public:

	CWorkMarkGrab();
	virtual ~CWorkMarkGrab();

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

	BOOL				InitWork(UINT8 cam_id);

};
