
#pragma once

#include "../WorkStep.h"

class CWorkRecipeUnload : public CWorkStep
{
/* ������ & �ı��� */
public:

	CWorkRecipeUnload();
	virtual ~CWorkRecipeUnload();

/* �����Լ� ������ */
protected:
public:

	virtual	BOOL		InitWork();
	virtual VOID		DoWork();


/* ���� ���� */
protected:


/* ���� �Լ� */
protected:

	ENG_JWNS			IsRecvJobLists();


/* ���� �Լ� */
public:

};
