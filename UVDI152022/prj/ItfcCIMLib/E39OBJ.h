
#pragma once

#include "EBase.h"

class CE39OBJ : public CEBase
{
public:
	/* 생성자 & 파괴자 */
	CE39OBJ(CLogData *logs, CSharedData *share);
	~CE39OBJ();

/* 구조체 */
protected:

/* 가상 함수 */
protected:
public:


/* 로컬 변수 */
protected:



/* 로컬 함수 */
protected:

/* 공용 함수 */
public:

	virtual BOOL		Init(LONG equip_id, LONG conn_id);
	virtual VOID		Close();

	ICxE39Ptr			GetE39Ptr()							{	return m_pICxE39Ptr;		}
	ICxE39ObjPtr		GetE39ObjPtr()						{	return m_pICxE39ObjPtr;		}


};
