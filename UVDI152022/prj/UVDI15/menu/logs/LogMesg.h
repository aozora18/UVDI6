#pragma once

#include "../../../../inc/comn/MyRich.h"

class CLogMesg : public CMyRich
{
/* ������ & �ı��� */
public:

	CLogMesg();
	virtual ~CLogMesg();

/* ���� ���� */
protected:

/* ���� �Լ� */
protected:

/* ���� �Լ� */
public:

	VOID				LoadFile(PTCHAR file, ENG_LNWE type);
};