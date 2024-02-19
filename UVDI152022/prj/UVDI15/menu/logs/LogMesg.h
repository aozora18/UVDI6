#pragma once

#include "../../../../inc/comn/MyRich.h"

class CLogMesg : public CMyRich
{
/* 생성자 & 파괴자 */
public:

	CLogMesg();
	virtual ~CLogMesg();

/* 로컬 변수 */
protected:

/* 로컬 함수 */
protected:

/* 공용 함수 */
public:

	VOID				LoadFile(PTCHAR file, ENG_LNWE type);
};