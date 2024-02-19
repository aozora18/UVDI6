
#pragma once

#include "EBase.h"

class CE94CJM : public CEBase
{
public:
	CE94CJM(CLogData *logs, CSharedData *share);
	~CE94CJM();

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

	BOOL				Init(LONG equip_id, LONG conn_id);
	VOID				Close();
	BOOL				SetLogLevel(ENG_ILLV level);

	ICxE94CJMPtr		GetE94CJMPtr()					{	return m_pICxE94CJMPtr;	}

	/* ----------------------------------------------------------------------------------------- */
	/*                                     Philoptics Area                                       */
	/* ----------------------------------------------------------------------------------------- */

	BOOL				SetProcessJobComplete(PTCHAR cj_id, PTCHAR pj_id);

	BOOL				CreateJob(PTCHAR cj_id, PTCHAR carr_id, CStringArray *pj_id,
								  E94_CPOM job_order, BOOL start);

	/* Control Job ID and States */
	BOOL				GetControlJobList(CStringArray &list_id, CAtlList<E94_CCJS> &list_state,
										  E94_CLCJ filter);
	BOOL				GetControlJobList(CStringArray &list_cj, CAtlList<E94_CCJS> &list_state);
	BOOL				GetControlJobList(CStringArray &list, E94_CLCJ filter);
	BOOL				GetControlJobFirstID(E94_CCJS filter, PTCHAR cj_id, UINT32 size);
	BOOL				GetControlJobState(PTCHAR cj_id, E94_CCJS &state);
	BOOL				SetSelectedControlJob(PTCHAR cj_id);
	BOOL				SetDeselectedControlJob(PTCHAR cj_id);
	/* Process Job ID */
	INT32				GetProcessJobCount(PTCHAR cj_id);
	BOOL				GetProcessJobList(PTCHAR cj_id, CStringArray &list,
										  E94_CLPJ filter=E94_CLPJ::en_list_all);

	BOOL				GetAllowRemoteControl(BOOL &enable);
	BOOL				SetAllowRemoteControl(BOOL enable);

	/* ----------------------------------------------------------------------------------------- */
	/*                                      Cimetrix Area                                        */
	/* ----------------------------------------------------------------------------------------- */

	BOOL				GetControlJobCarrierInputSpec(PTCHAR cj_id, CStringList &list_carr);
	BOOL				GetControlJobCarrierOutputSpec(PTCHAR cj_id, CStringList &list_carr);
	BOOL				GetControlJobProcessJobList(PTCHAR cj_id, CStringList &list_pj);
	BOOL				GetControlJobStartMethod(PTCHAR cj_id, BOOL &auto_start);
	BOOL				GetControlJobCompletedTime(PTCHAR cj_id, PTCHAR date_time, UINT32 size);
	BOOL				GetControlJobMaterialArrived(PTCHAR cj_id, BOOL &arrived);

	BOOL				SetStartControlJob(PTCHAR cj_id);
	BOOL				SetStopControlJob(PTCHAR cj_id, E94_CCJA action);
	BOOL				SetAbortControlJob(PTCHAR cj_id, E94_CCJA action);
	BOOL				SetPauseControlJob(PTCHAR cj_id);
	BOOL				SetResumeControlJob(PTCHAR cj_id);
	BOOL				SetCancelControlJob(PTCHAR cj_id, E94_CCJA action);
	BOOL				SetHOQControlJob(PTCHAR cj_id);

	BOOL				CJGetJob(PTCHAR ctrl_id, ICxE94ControlJobPtr cj);

	BOOL				StartNextProcessJob(PTCHAR cj_id);
	BOOL				StartNextControlJob();
};

