
#pragma once

#include "EBase.h"

class CE116EPT : public CEBase
{
public:
	CE116EPT(CLogData *logs, CSharedData *share);
	~CE116EPT();

/* 구조체 */
protected:

/* 가상 함수 */
protected:
public:


/* 로컬 변수 */
protected:



/* 로컬 함수 */
protected:

	BOOL				InitE116EPT();
	VOID				CloseE116EPT();
	BOOL				InitE116HW();
	VOID				CloseE116HW();


/* 공용 함수 */
public:

	BOOL				Init(LONG equip_id, LONG conn_id);
	VOID				Close();

	ICxE116ObjectPtr	GetE116ObjPtr()						{	return m_pICxE116ObjPtr;	}

	/* ----------------------------------------------------------------------------------------- */
	/*                                     Philoptics Area                                       */
	/* ----------------------------------------------------------------------------------------- */

	BOOL				SetBusy(PTCHAR mod_name, PTCHAR task_name, E116_ETBS task_type);
	BOOL				SetIdle(PTCHAR mod_name);

	/* ----------------------------------------------------------------------------------------- */
	/*                                      Cimetrix Area                                        */
	/* ----------------------------------------------------------------------------------------- */

};

