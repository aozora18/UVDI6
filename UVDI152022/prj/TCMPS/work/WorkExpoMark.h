
#pragma once

#include "Work.h"

class CWorkExpoMark : public CWork
{
/* ������ & �ı��� */
public:

	CWorkExpoMark(ENG_AOEM mode);
	virtual ~CWorkExpoMark();

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
