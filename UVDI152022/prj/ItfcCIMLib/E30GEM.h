
#pragma once

#include "EBase.h"

class CE30GEM : public CEBase
{
public:
	/* 생성자 & 파괴자 */
	CE30GEM(CLogData *logs, CSharedData *share, LONG conn_cnt);
	~CE30GEM();

/* 구조체 */
protected:

	/* 이벤트 등록 정보 : conn_id, variables_id */
	typedef struct __st_register_event_handle_info__
	{
		INT32			conn_id;	/* Equipment Connection ID */
		INT32			evt_id;		/* Event (Var or Event or Cmd) ID */

	}	STM_REHI,	*LPM_REHI;

	/* 이벤트 등록 정보 : command, description, message */
	typedef struct __st_register_event_command_info__
	{
		PTCHAR			cmd;

		/* 할당된 메모리 해제 */
		VOID Free()
		{
			if (cmd)	::Free(cmd);
		}

	}	STM_RECI,	*LPM_RECI;


/* 가상 함수 */
protected:
public:


/* 로컬 변수 */
protected:

	BOOL				m_bIsInitConnected;		/* CIM Connect 초기화 수행 여부 */
	BOOL				m_bIsInitCompleted;		/* CIM Connect 초기화 완료 여부 */
	BOOL				m_bIsLoadProject;

	LONG				m_lConnectionCount;		/* Connection 개수 (Host 개수) */


	CStringArray		m_arrEquipLocName;		/* Tracking System에 등록된 Module (Location) Name */
	CStringArray		m_arrEquipProcName;		/* Tracking System에 등록된 Processing Name */

	ICxValueDispPtr		m_pICxSECSMesg;			/* The interface for SECS-II Message */

	CAtlList <LPM_REHI>	m_lstEvtValC;			/* Callback for ValueChange */
	CAtlList <LPM_REHI>	m_lstEvtValG;			/* Callback for GetValue*/
	CAtlList <LPM_REHI>	m_lstEvtMesg;			/* Callback for Message */
	CAtlList <LPM_RECI>	m_lstEvtCmds;			/* Callback for Command */


/* 로컬 함수 */
protected:

	BOOL				InitE30GEM(LONG equip_id);
	VOID				CloseE30GEM();


	BOOL				SetDispAppendValue(ICxValueDisp *disp, LNG_CVVT value, E30_GPVT type);

	BOOL				GetValue(LONG conn_id, LONG var_id, E30_GPVT type, LNG_CVVT value,
								 LONG handle=0, LONG index=0);
	BOOL				GetValueDispPtr(LONG handle, LONG index, UINT8 flag,
										ICxValueDispPtr disp, LPG_CEVI data);

	BOOL				RegisterGetValueID(LONG conn_id, LONG var_id, PTCHAR mesg=NULL);
	BOOL				UnregisterGetValueID();

	BOOL				RegisterValueChangeID(LONG conn_id, LONG var_id, PTCHAR mesg=NULL);
	BOOL				UnregisterValueChangeID();

	BOOL				RegisterCommonID();
	BOOL				UnregisterCommonID();

	BOOL				RegisterCommandID();
	BOOL				RegisterCommandID(CComBSTR cmd, CComBSTR desc, PTCHAR mesg=NULL);
	BOOL				UnregisterCommandID();

	BOOL				RegisterMessageID(LONG conn_id, UINT8 mesg_sid, UINT8 mesg_fid, PTCHAR mesg);
	BOOL				UnregisterMessageID();


/* 공용 함수 */
public:

	BOOL				Init(LONG equip_id, LONG conn_id);
	VOID				Close();
	BOOL				Finalized(LONG equip_id, LONG conn_id);
	VOID				ResetInitFlag();

	BOOL				IsServiceRun();
	BOOL				LoadProject(PTCHAR epj_file);
	BOOL				SaveProjetsToEPJ(PTCHAR epj_file);

	BOOL				WaitOnEMServiceIdle(UINT32 time_out=50000);

	BOOL				SendTerminalMessage(LONG conn_id, LONG term_id, PTCHAR mesg);
	BOOL				SendTerminalAcknowledge(LONG conn_id);

	BOOL				S2F17DateAndTimeRequest(LONG conn_id);
	BOOL				OperatorCommand(LONG conn_id, PTCHAR name, LONG handle);

	BOOL				GetValue(LONG conn_id, LONG var_id, UNG_CVVT &value, LONG handle=0, LONG index=0);
	BOOL				GetValueI1(LONG conn_id, LONG var_id, PINT8 value, LONG handle=0, LONG index=0);
	BOOL				GetValueI2(LONG conn_id, LONG var_id, PINT16 value, LONG handle=0, LONG index=0);
	BOOL				GetValueI4(LONG conn_id, LONG var_id, PINT32 value, LONG handle=0, LONG index=0);
	BOOL				GetValueI8(LONG conn_id, LONG var_id, PINT64 value, LONG handle=0, LONG index=0);
	BOOL				GetValueU1(LONG conn_id, LONG var_id, PUINT8 value, LONG handle=0, LONG index=0);
	BOOL				GetValueU2(LONG conn_id, LONG var_id, PUINT16 value, LONG handle=0, LONG index=0);
	BOOL				GetValueU4(LONG conn_id, LONG var_id, PUINT32 value, LONG handle=0, LONG index=0);
	BOOL				GetValueU8(LONG conn_id, LONG var_id, PUINT64 value, LONG handle=0, LONG index=0);
	BOOL				GetValueF4(LONG conn_id, LONG var_id, PFLOAT value, LONG handle=0, LONG index=0);
	BOOL				GetValueF8(LONG conn_id, LONG var_id, DOUBLE *value, LONG handle=0, LONG index=0);
	BOOL				GetValueBo(LONG conn_id, LONG var_id, BOOL *value, LONG handle=0, LONG index=0);
	BOOL				GetValueBi(LONG conn_id, LONG var_id, PUINT8 value, LONG handle=0, LONG index=0);
	BOOL				GetValueType(LONG conn_id, LONG var_id, E30_GPVT &type);
	BOOL				GetVarType(LONG conn_id, LONG var_id, E30_GVTC &type);

	/* Validation releated Functions */
	BOOL				IsInitConnected(BOOL logs=FALSE);
	BOOL				IsInitCompleted()	{	return m_bIsInitCompleted;	}
	BOOL				IsLoadedProject()	{	return m_bIsLoadProject;	}
	BOOL				IsInitedAll(BOOL logs=FALSE);
	BOOL				IsCommEnabled(LONG conn_id, BOOL logs=TRUE);
	BOOL				IsCommCommunicating(LONG conn_id, BOOL logs=TRUE);
	BOOL				IsControlOnline(LONG conn_id, BOOL logs=TRUE);
	BOOL				IsControlRemote(LONG conn_id, BOOL logs=TRUE);

	/* Related the [Control] */
	BOOL				SetControlOnline(LONG conn_id, BOOL state);
	BOOL				SetControlRemote(LONG conn_id, BOOL state);
	BOOL				SetCommunication(LONG conn_id, BOOL enable);

	/* Related the [Spool] */
	BOOL				SetSpoolState(LONG conn_id, BOOL enable);
	BOOL				SetSpoolOverwrite(LONG conn_id, BOOL overwrite);
	BOOL				SetSpoolPurge(LONG conn_id);

	/* Related the [Alarm] */
	BOOL				SetAlarmsEnabled(LONG conn_id, LONG id, BOOL state);
	BOOL				AlarmClearAll();
	BOOL				AlarmClear(LONG var_id);
	BOOL				AlarmSet(LONG var_id);
	BOOL				AlarmSet(LONG var_id, PTCHAR text);
	BOOL				GetAlarmList(CAtlList <LONG> &ids, CAtlList <LONG> &states, BOOL alarm);
	BOOL				SetDeleteAlarm(LONG id, BOOL logs=FALSE);
	BOOL				GetAlarmID(LONG conn_id, PTCHAR name, LONG &id, BOOL logs=FALSE);
	BOOL				GetAlarmState(LONG id, LONG &state, BOOL logs=FALSE);

	/* Trigger Event Func... */
	BOOL				SetTrigEvent(LONG conn_id, LONG evt_id);
	BOOL				SetTrigEventValue(LONG conn_id, LONG evt_id, LONG var_id,
										  LNG_CVVT value, E30_GPVT type);
	BOOL				SetTrigEventValues(LONG conn_id, LONG evt_id,
										   CAtlList<LONG> &var_ids, CAtlList<UNG_CVVT> &values,
										   CAtlList<E30_GPVT> &types);
	BOOL				SetTrigEventList(LONG conn_id, LONG evt_id, LONG var_id,
										 CAtlList<UNG_CVVT> &values, CAtlList<E30_GPVT> &types);
	BOOL				SetUpdateProcessState(E30_GPSV state);

	/* Commands */
	BOOL				GetCommandList(CAtlList <LONG> &ids, CStringArray &names);
	/* EPJ (Project) Files */
	BOOL				GetEPJList(CStringArray &files, CAtlList <LONG> &sizes);

	/* Communications */
	BOOL				GetCommSetupInfo(LONG conn_id, PTCHAR comm_pgid, UINT32 size_pgid,
										 PTCHAR comm_sets, UINT32 size_sets);
	BOOL				SetCommSetupInfo(LONG conn_id, PTCHAR comm_pgid, PTCHAR comm_sets);
	/* Related the Recipes (Process Program) */
	BOOL				GetRecipes(CStringArray &names, CAtlList <LONG> &sizes);
	BOOL				QueryRecipeUpload(LONG conn_id, PTCHAR name, UINT32 size);
	BOOL				QueryRecipeRequest(LONG conn_id, PTCHAR name);
	BOOL				QueryRecipeSend(LONG conn_id, PTCHAR name);

	/* Etc. Utility */
	BOOL				UpdateCIM300State();

	BOOL				SetSynchronizeTime(UINT64 t_secs);
	BOOL				SetClearProject(BOOL logs=FALSE);
	BOOL				SetDeleteAllRequests(BOOL logs=FALSE);
	BOOL				GetCommState(LONG conn_id, E30_GCSS &state, E30_GCSH &substate, BOOL logs=FALSE);
	BOOL				GetConnectionName(LONG conn_id, PTCHAR name, UINT32 size, BOOL logs=FALSE);
	BOOL				GetCurrentProjectName(PTCHAR name, UINT32 size, BOOL logs=FALSE);
	BOOL				GetDefaultProjectName(PTCHAR name, UINT32 size, BOOL logs=FALSE);
	BOOL				GetHostToEquipID(LONG &equip_id, BOOL logs=FALSE);
#if 0
	BOOL				GetVarMinMax(LONG conn_id, LONG id, E30_GPVT &type, PVOID min, PVOID max);
#else
	BOOL				GetVarMinMax(LONG conn_id, LONG id, E30_GPVT &type, UNG_CVVT &min, UNG_CVVT &max);
#endif
	BOOL				GetHostVariables(LONG conn_id, E30_GVTC type, CAtlList <STG_CEVI> &lst_data);
	BOOL				IsLogging(BOOL &enable);
	BOOL				SetLogEnable(BOOL enable);

	BOOL				GetReports(LONG conn_id, CAtlList <LONG> &ids, CStringArray &names);
	BOOL				GetReportsEvents(LONG conn_id, CAtlList <LONG> &r_ids, CAtlList <LONG> &e_ids);
	BOOL				GetListTraces(LONG conn_id, CAtlList <LONG> &t_ids, CStringArray &t_names);
	BOOL				GetListVariables(LONG conn_id, E30_GVTC type, CAtlList <LONG> &ids, CStringArray &names);
	BOOL				GetListMessageIDs(LONG conn_id, CAtlList <LONG> &ids);

	BOOL				SetLoggingConfigSave(BOOL enable, BOOL logs=FALSE);
	BOOL				GetLoggingConfigSave(BOOL &enable, BOOL logs=FALSE);

	BOOL				SendPrimaryMessage(LONG conn_id, UINT8 mesg_sid, UINT8 mesg_fid, 
										   PTCHAR str_msg, UINT8 term_id=0, BOOL reply=FALSE);

	BOOL				GetAppName(PTCHAR name, UINT32 size);
	BOOL				SetAppName(PTCHAR name);

	BOOL				GetBuildDateTime(PTCHAR date_time, UINT32 size);
	BOOL				GetBuildVersion(PTCHAR version, UINT32 size);
	BOOL				GetBuildVersion(LONG &major, LONG &minor, LONG &patch, LONG &build);
	BOOL				GetListApplications(LONG equip_id, CAtlList <LONG> &ids, CStringArray &names);
	BOOL				GetListConnections(LONG equip_id, CAtlList <LONG> &ids, CStringArray &names);
	BOOL				GetListEquipments(CAtlList <LONG> &ids, CStringArray &names);

	/* Tracking System에 등록된 이름 반환 */
	PTCHAR				GetEquipLocationName(ENG_TSSI index);
	PTCHAR				GetEquipProcessingName(E90_SSPS index);

	/* EPJ 프로젝트 파일 내에 값 (숫자, 문자열?)이 변경될 때, 상위 프로그램 (PCIMLib)에게 알리기 위함 */
	BOOL				RegisterValueChangeID(CAtlList <STG_TVCI> *lst_val);
	BOOL				RegisterMessageID(CAtlList <STG_TSMI> *lst_mesg);

	ICxEMServicePtr		GetEMServicePtr()					{	return m_pICxEMService;		}
	ICxClientApplicationPtr	GetClientAppPtr()				{	return m_pICxClientApp;		}
	ICxClientToolPtr	GetClientToolPtr()					{	return m_pICxClientTool;	}
	ICxClientDataPtr	GetClientDataPtr()					{	return m_pICxClientData;	}

	ICxEMAppCallbackPtr	GetEMAppCallbackPtr()				{	return m_pICxEMAppCbk;		}

	/* SECS-II Message 전달 */
	BOOL				ResetSecsMesgList();
	BOOL				AddSecsMesgRoot(E30_GPVT type, PVOID value, UINT32 size);
	LONG				AddSecsMesgList(LONG handle, BOOL clear=FALSE);
	BOOL				AddSecsMesgValue(LONG handle, E30_GPVT type, PVOID value, UINT32 size);
	BOOL				SendSecsMessage(UINT8 conn_id, UINT8 mesg_sid, UINT8 mesg_fid,
										BOOL is_reply, LONG &trans_id, BOOL is_body);
	BOOL				ListAtItem(ICxValueDisp *mesg, LONG handle, LONG index, LONG level,
								   CAtlList <STG_CLHC> &item);
};
