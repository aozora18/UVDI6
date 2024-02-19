
#pragma once

#include "../WorkStep.h"

class CWorkRecipeUnload : public CWorkStep
{
/* 생성자 & 파괴자 */
public:

	CWorkRecipeUnload();
	virtual ~CWorkRecipeUnload();

/* 가상함수 재정의 */
protected:
public:

	virtual	BOOL		InitWork();
	virtual VOID		DoWork();


/* 로컬 변수 */
protected:


/* 로컬 함수 */
protected:

	ENG_JWNS			IsRecvJobLists();


/* 공용 함수 */
public:

};
