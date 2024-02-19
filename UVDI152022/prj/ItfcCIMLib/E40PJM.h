
#pragma once

#include "EBase.h"

class CE40PJM : public CEBase
{
public:
	CE40PJM(CLogData *logs, CSharedData *share);
	~CE40PJM();

/* 구조체 */
protected:


/* 가상 함수 */
protected:
public:


/* 로컬 변수 */
protected:



/* 로컬 함수 */
protected:

	BOOL				InitE40PJM();
	VOID				CloseE40PJM();


/* 공용 함수 */
public:

	BOOL				Init(LONG equip_id, LONG conn_id);
	VOID				Close();
	BOOL				SetLogLevel(ENG_ILLV level);

	ICxE40PJMPtr		GetE40PJMPtr()						{	return m_pICxE40PJMPtr;		}

	BOOL				SetPRJobStartProcess(PTCHAR pj_id);
	BOOL				SetPRJobComplete(PTCHAR pj_id);
	BOOL				SetPRJobCompleteOnly(PTCHAR pj_id);
	BOOL				SetPRJobAbortComplete(PTCHAR pj_id);
	BOOL				SetPRJobProcessComplete(PTCHAR pj_id);
	BOOL				SetProcessJobQueueToSetup(PTCHAR pj_id);
	BOOL				SetProcessJobStoppingToStopped(PTCHAR pj_id);
	BOOL				SetProcessJobRemove(PTCHAR pj_id);
	BOOL				SetProcessJobDequeue(PTCHAR pj_id);
	BOOL				SetProcessJobCreate(PTCHAR pj_id, PTCHAR carr_id, PTCHAR recipe_name,
											E40_PPMT mtrl_type, BOOL recipe_only, BOOL proc_start,
											CAtlList <UINT8> *slot_no, BOOL enhanced=TRUE);
	BOOL				GetControlJobID(PTCHAR pj_id, PTCHAR cj_id, UINT32 size);
	BOOL				SetControlJobID(PTCHAR pj_id, PTCHAR cj_id);
	BOOL				GetProcessJobState(PTCHAR pj_id, E40_PPJS &state);
	BOOL				GetProcessAllJobId(CStringArray &list_job);
	BOOL				GetProcessAllJobIdState(CStringArray &list_job, CAtlList <E40_PPJS> &list_state);
	BOOL				GetProcessJobAllSubstID(PTCHAR pj_id, CStringArray &subst_id, CStringArray &carr_id);
	BOOL				GetProcessJobAllSlotNo(PTCHAR pj_id, CByteArray &slot_no, CStringArray &carr_id);
	BOOL				GetProcessJobQueue(CStringArray &list_job);
	BOOL				GetProcessJobSpaceCount(UINT32 &count);
	BOOL				GetPRJobProcessSlotID(PTCHAR pj_id, PTCHAR carr_id, CAtlList <UINT8> &slot_no);
	BOOL				GetProcessJobFirstID(E40_PPJS filter, PTCHAR pj_id, UINT32 size);

	BOOL				GetAllowRemoteControl(BOOL &enable);
	BOOL				SetAllowRemoteControl(BOOL enable);

	BOOL				GetPRMtlNameList(PTCHAR pj_id, CStringArray &carr_id, CByteArray &slot_no);

	BOOL				SetPRJobStateControl(PTCHAR pj_id, E40_PPJC control);

	BOOL				SetProcessJobState(PTCHAR pj_id, E40_PPJS state);
	BOOL				SetProcessJobAborted(PTCHAR pj_id);
	BOOL				SetPRJobCommand(PTCHAR pj_id, PTCHAR command);

	BOOL				GetPRMaterialOrder(E94_CPOM &order);
	BOOL				SetPRMaterialOrder(E94_CPOM order);

	BOOL				SetEnablePauseEvent(BOOL enable);
	BOOL				GetEnablePauseEvent(BOOL &enable);

	BOOL				GetNotificationType(E40_PENT &type);
	BOOL				SetNotificationType(E40_PENT type);

	BOOL				SetPRJobAlert(PTCHAR pj_id);
	BOOL				SetPRJobEvent(PTCHAR pj_id, E40_PSEO evt_id,
									  CAtlList <UINT32> *list_vid,
									  CAtlList <E30_GPVT> *list_type,
									  CStringArray *list_value);
	BOOL				SetPRJobSetStartMethod(CStringArray *list_jobs, BOOL start);

	BOOL				GetPRMtlType(PTCHAR pj_id, E40_PPMT &type);
	BOOL				SetPRMtlType(PTCHAR pj_id, E40_PPMT type);

	BOOL				GetPRWaitForAllMaterial(PTCHAR pj_id, BOOL &value);
	BOOL				SetPRWaitForAllMaterial(PTCHAR pj_id, BOOL value);

	BOOL				GetRecipeID(PTCHAR pj_id, PTCHAR rp_id, UINT32 size);
	BOOL				GetProcessStartMethod(PTCHAR pj_id, BOOL &method);
};

