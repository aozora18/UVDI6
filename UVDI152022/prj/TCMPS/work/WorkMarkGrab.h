
#pragma once

#include "Work.h"

class CWorkMarkGrab : public CWork
{
/* ������ & �ı��� */
public:

	CWorkMarkGrab();
	virtual ~CWorkMarkGrab();

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

	BOOL				InitWork(UINT8 cam_id);

};
