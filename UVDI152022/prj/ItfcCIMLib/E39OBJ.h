
#pragma once

#include "EBase.h"

class CE39OBJ : public CEBase
{
public:
	/* ������ & �ı��� */
	CE39OBJ(CLogData *logs, CSharedData *share);
	~CE39OBJ();

/* ����ü */
protected:

/* ���� �Լ� */
protected:
public:


/* ���� ���� */
protected:



/* ���� �Լ� */
protected:

/* ���� �Լ� */
public:

	virtual BOOL		Init(LONG equip_id, LONG conn_id);
	virtual VOID		Close();

	ICxE39Ptr			GetE39Ptr()							{	return m_pICxE39Ptr;		}
	ICxE39ObjPtr		GetE39ObjPtr()						{	return m_pICxE39ObjPtr;		}


};
