
#pragma once

#include "EBase.h"

class CE157MPT : public CEBase
{
public:
	CE157MPT(CLogData *logs, CSharedData *share);
	~CE157MPT();

/* ����ü */
protected:

/* ���� �Լ� */
protected:
public:


/* ���� ���� */
protected:



/* ���� �Լ� */
protected:

	BOOL				InitE115MPT();
	VOID				CloseE115MPT();
	BOOL				InitE115HW();
	VOID				CloseE115HW();


/* ���� �Լ� */
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

