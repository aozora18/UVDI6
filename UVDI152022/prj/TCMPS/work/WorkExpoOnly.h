
#pragma once

#include "Work.h"

class CWorkExpoOnly : public CWork
{
/* ������ & �ı��� */
public:

	CWorkExpoOnly();
	virtual ~CWorkExpoOnly();

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
