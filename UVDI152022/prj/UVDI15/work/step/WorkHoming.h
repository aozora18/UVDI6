
#pragma once

#include "../WorkStep.h"

class CWorkHoming : public CWorkStep
{
/* 생성자 & 파괴자 */
public:

	CWorkHoming();
	virtual ~CWorkHoming();

/* 가상함수 재정의 */
protected:
public:

	virtual	BOOL		InitWork();
	virtual VOID		DoWork();


/* 로컬 변수 */
protected:



/* 로컬 함수 */
protected:


/* 공용 함수 */
public:


};
