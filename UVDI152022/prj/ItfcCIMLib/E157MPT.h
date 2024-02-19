
#pragma once

#include "EBase.h"

class CE157MPT : public CEBase
{
public:
	CE157MPT(CLogData *logs, CSharedData *share);
	~CE157MPT();

/* 구조체 */
protected:

/* 가상 함수 */
protected:
public:


/* 로컬 변수 */
protected:



/* 로컬 함수 */
protected:

	BOOL				InitE115MPT();
	VOID				CloseE115MPT();
	BOOL				InitE115HW();
	VOID				CloseE115HW();


/* 공용 함수 */
public:

	BOOL				Init(LONG equip_id, LONG conn_id);
	VOID				Close();

	ICxE157ObjectPtr	GetE157ObjPtr()					{	return m_pICxE157ObjPtr;	}

	/* ----------------------------------------------------------------------------------------- */
	/*                                     Philoptics Area                                       */
	/* ----------------------------------------------------------------------------------------- */

	BOOL				MPTExecutionStarted(PTCHAR mod_name, PTCHAR pj_id, PTCHAR rp_id, PTCHAR st_id);
	BOOL				MPTExecutionCompleted(PTCHAR mod_name, BOOL exec_ok);
	BOOL				MPTStepStarted(PTCHAR mod_name, PTCHAR step_id);
	BOOL				MPTStepCompleted(PTCHAR mod_name, BOOL step_ok);

	/* ----------------------------------------------------------------------------------------- */
	/*                                      Cimetrix Area                                        */
	/* ----------------------------------------------------------------------------------------- */

};

