
#pragma once

#include "Work.h"

class CWorkMarkMove : public CWork
{
/* ������ & �ı��� */
public:

	CWorkMarkMove();
	virtual ~CWorkMarkMove();

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

	BOOL				InitWork(UINT16 mark_no);

};
