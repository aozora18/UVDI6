
#pragma once

#include "Work.h"

class CWorkMarkTest : public CWork
{
/* ������ & �ı��� */
public:

	CWorkMarkTest();
	virtual ~CWorkMarkTest();

/* �����Լ� ������ */
protected:
public:
	virtual VOID		RunWork();


/* ���� ���� */
protected:


/* ���� �Լ� */
protected:

	VOID				SetWorkNext();



/* ���� �Լ� */
public:

	BOOL				InitWork();

};
