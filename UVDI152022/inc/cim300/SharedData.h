
#pragma once

class CPRJobID;
class CCRJobID;

class CSharedData
{
public:
	/* ������ & �ı��� */
	CSharedData(LONG conn_cnt);
	virtual ~CSharedData();
 
/* ����ü */
protected:

#pragma pack (push, 8)


#pragma pack (pop)

/* ������ */
protected:

/* ����ü */
protected:

#pragma pack (push, 8)

	/* Global Interface Table Cookie */
	typedef struct __st_global_interface_table_cookie__
	{
		UINT32			clientapp_cookie;
		UINT32			e39oss_cookie;
		UINT32			e40pjm_cookie;
		UINT32			e87cms_cookie;
		UINT32			e90sts_cookie;
		UINT32			e94cjm_cookie;
		UINT32			e116obj_cookie;
		UINT32			e157obj_cookie;

	}	STM_GITC,	*LPM_GITC;

	/* PRCommandCbk �Լ��� ȣ��� ��, ����ϱ� ���� Process Job ID ���� ����� �ε��� ��ġ ��� ���� */
	typedef struct __st_prcommand_job_index_position__
	{
		UINT8			begin;	/* Zero-based */
		UINT8			end;	/* Max 24. */
		UINT8			u8_reserved[6];

	}	STM_PJIP,	*LPM_PJIP;

#pragma pack (pop)

/* ���� ���� */
protected:

	/* �ӽ÷� ��� */
	TCHAR				m_tzDockLocName[64];
	TCHAR				m_tzUndockLocName[64];

	/* Process Program (=Recipe) ��� ��ġ */
	TCHAR				m_tzRecipePath[MAX_PATH_LEN];
	TCHAR				m_tzRecipeExt[MAX_FILE_LEN];	/* ������ ������ Ȯ���� */

	/* ���� �ֱٿ� SECS-II Message ���� �� ������ Transaction ID �� �ӽ� ���� */
	LONG				m_lLastSECSMsgTransID;

	/* Load Port State */
	ENG_LPOS			m_enLoadPortState[MAX_FOUP_COUNT];
	LPG_CLPI			m_pstLoadPortState;

	/* Process State */
	E30_GPSV			m_enProcessState;
	/* Common */
	STM_GITC			m_stCookie;

	/* Reply */
	E30_HPLA			m_enRecipeLoadAck;	/* Host�κ��� Recipe�� ���� ��û�� ���� ���� */
	E30_SRHA			m_enRecipeSendAck;	/* Host�κ��� Recipe ������ �۽� ��û�� ���� ���� */
	E30_STHA			m_enTermMsgAck;		/* Host�κ��� Terminal ������ �۽� ��û�� ���� ���� */
	E30_RMJC			m_enRecipeFileAck;	/* Host�κ��� Recipe ���� �۽� ��û�� ���� ���� */

	STG_CHCT			m_stTransaction;	/* ���� �ֱ� Host�κ��� ��û ���� Transaction ���� �ӽ� ���� */

	/* ������ ����ȭ ��ü */
	CMyMutex			m_mtxEvent;
	CMyMutex			m_mtxRecipe;
	CMyMutex			m_mtxPRJobIDs;		/* E40PJM::Callback ȣ��� ��, ���� �� ���� ����ȭ ��ü */
	CMyMutex			m_mtxCRJobIDs;		/* E94PJM::Callback ȣ��� ��, ���� �� ���� ����ȭ ��ü */

	CAtlList <STG_CCED>	m_lstEvent;			/*  Event */

	/* ���� �ֱ��� Change State �� �ӽ� ���� */
	CAtlMap <LONG, LONG>m_mapCommState;		/* Key = conn_id, Value = State */
	CAtlMap <LONG, LONG>m_mapCtrlState;		/* Key = conn_id, Value = State */
	CAtlMap <LONG, LONG>m_mapSpoolState;	/* Key = conn_id, Value = State */

	CAtlList <STG_CRFD>	m_lstRecipe;		/* ���� ��� ��� (����)�� ������ ��� (����) ���� */
	CAtlList <CPRJobID*>m_lstPRJobID;		/* PRJobID ���� */
	CAtlList <CCRJobID*>m_lstCRJobID;		/* CRJobID ���� */

	CAtlList <STG_TSMD>	m_lstSecsMesg;		/* SECS-II Message ���� */


/* ���� �Լ� */
protected:

	/* For Event */
	VOID				PushEvent(ENG_ESSC exx_id, UINT8 cbf_id, LNG_CCED evt_data);
	VOID				PushEvent(ENG_ESSC exx_id, UINT8 cbf_id, LNG_CCED evt_data,
								  ENG_GCPI id_data, PTCHAR data);
	VOID				PushEvent(ENG_ESSC exx_id, UINT8 cbf_id, LNG_CCED evt_data,
								  ENG_GCPI id_data1, ENG_GCPI id_data2, PTCHAR data1, PTCHAR data2);

/* ���� �Լ� */
public:

	/* ���� E30_State Change ���� �� ��ȯ */
	LONG				E30_GetState(E30_SSMS machine, LONG conn_id);

	/* ���μ��� ���� ���� (CIMConnect) */
	E30_GPSV			GetProcessState()				{	return m_enProcessState;		}
	VOID				SetProcessState(E30_GPSV state)	{	m_enProcessState	= state;	}

	/* Transaction ���� ���� �� ��ȯ */
	LPG_CHCT			GetTransactionInfo()	{	return &m_stTransaction;	}
	VOID				SetTransactionInfo(PTCHAR loc_id, PTCHAR carr_id, PTCHAR data_seg,
										   LONG data_len, LONG transaction_id);

	/* Common */
	PTCHAR				GetLoadPortDockedName(UINT16 port_id, UINT8 docked);
	LPM_GITC			GetGITCookie()	{	return &m_stCookie;		}

	/* Event */
	VOID				RemoveEventAll();
	BOOL				PopEvent(STG_CCED &event);
	BOOL				IsEmptyEvent();

	VOID				TerminalMsgRcvd(LONG conn_id, LONG term_id, PTCHAR mesg=NULL);
	VOID				MessageReceived(LONG conn_id, LONG mseg_id, LONG reply);
	VOID				CommandCalled(LONG conn_id, E30_GRCI rcmd_id, PTCHAR rcmd_str=NULL);

	VOID				E30_ValueChanged(LONG conn_id, LONG var_id, PTCHAR var_name=NULL);
	VOID				E30_ECChanged(LONG conn_id, PTCHAR var_name=NULL);
	VOID				E30_AlarmChanged(LONG alarm_id, LONG alarm_cd);
	VOID				E30_PushState(E30_SSMS machine, LONG conn_id, LONG state);
	VOID				E30_PPData(E30_CEFI cbf_id, LONG conn_id, PTCHAR recipe_name);

	VOID				E40_PRStateChange(PTCHAR job_id, LONG state);
	VOID				E40_PushCommnad(E40_CEFI cbf_id, E40_PECS cmd_src, E40_PCCC cmd,
										PTCHAR job_id, PVOID param_list);
	VOID				E40_PushCommnad(E40_CEFI cbf_id, E40_PCCC cmd, PTCHAR job_id,
										PVOID param_list);

	VOID				E87_PushState(UINT8 machine, LONG state, INT16 port_id, PTCHAR carr_id=NULL);
	VOID				E87_PushEvent(E87_CEFI cbf_id, LONG state,
									  INT16 port_id, PTCHAR carr_id=NULL, PTCHAR loc_id=NULL);
	VOID				E87_PushEvent(E87_CEFI cbf_id, LONG state, INT16 port_id,
									  ENG_GCPI type1, ENG_GCPI type2, PTCHAR data1, PTCHAR data2);

	VOID				E90_PushCommnad(E90_SPSC cmd, PTCHAR subst_id);
	VOID				E90_PushEvent(E90_CEFI cbf_id, E90_VSMC machine, LONG new_state, PTCHAR subst_id);

	VOID				E94_PushCJState(E94_CCJS state, PTCHAR ctrl_id=NULL);
	VOID				E94_PushEvent(E94_CEFI cbf_id, ENG_GCPI type, PTCHAR data=NULL);
	VOID				E94_PushEvent(E94_CEFI cbf_id, E94_CPOM type, PTCHAR job_id);
	VOID				E94_PushCommnad(E94_CEFI cbf_id, E94_PECS cmd_src, E94_CCMD cmd, PTCHAR job_id);
	VOID				E94_PushCommnad(E94_CEFI cbf_id, E94_CCMD cmd, PTCHAR job_id);

	/* ���� �ֱٿ� �۽ŵ� SECS-II Message�� Transaction ID �� */
	LONG				GetLastSECSMessageTransID()			{	return m_lLastSECSMsgTransID;	}
	VOID				SetLastSECSMessageTransID(LONG id)	{	m_lLastSECSMsgTransID = id;		}

	/* ������ ��û�� ���� ���� �� ���� �� ��ȯ */
	VOID				ResetPPLoadRecipeAck()	{	m_enRecipeLoadAck	= E30_HPLA::en_rg_reset;	}
	E30_HPLA			GetPPLoadRecipeAck()	{	return m_enRecipeLoadAck;	}
	VOID				SetPPLoadRecipeAck(E30_HPLA reply)	{	m_enRecipeLoadAck = reply;	}
	BOOL				IsPPLoadRecipeAckOk()	{	return (m_enRecipeLoadAck == E30_HPLA::en_rg_ok || m_enRecipeLoadAck == E30_HPLA::en_rg_have);	}
	/* ������ �۽� (����)�� ���� ���� �� ���� �� ��ȯ */
	VOID				ResetPPSendRecipeAck()	{	m_enRecipeSendAck	= E30_SRHA::en_ra_reset;	}
	E30_SRHA			GetPPSendRecipeAck()	{	return m_enRecipeSendAck;	}
	VOID				SetPPSendRecipeAck(E30_SRHA reply)	{	m_enRecipeSendAck = reply;	}
	BOOL				IsPPSendRecipeAckOk()	{	return (m_enRecipeSendAck == E30_SRHA::en_ra_accepted);	}
	/* �͹̳� �޽��� �۽� (����)�� ���� ���� �� ���� �� ��ȯ */
	VOID				ResetTermMsgAck()		{	m_enTermMsgAck	= E30_STHA::en_t_reset;	}
	E30_STHA			GetTermMsgAck()			{	return m_enTermMsgAck;	}
	VOID				SetTermMsgAck(E30_STHA reply)	{	m_enTermMsgAck = reply;	}
	BOOL				IsTermMsgAckOk()		{	return (m_enTermMsgAck == E30_STHA::en_t_accepted);	}
	/* ���� �ֱٿ� Host�κ��� ���Ź��� ������ ���� (����) �� ���� �� ��ȯ (Process Program; Recipe) */
	VOID				ResetRecipeFileAck()	{	m_enRecipeFileAck	= E30_RMJC::en_reset;	}
	E30_RMJC			GetRecipeFileAck()		{	return m_enRecipeFileAck;	}
	VOID				SetRecipeFileAck(E30_RMJC reply)	{	m_enRecipeFileAck = reply;	}

	/* ������ ��� (����) ���� �Լ� */
	BOOL				RecipeAppend(LONG conn_id, PTCHAR file);
	BOOL				RecipeRemove(LONG conn_id, PTCHAR file);
	BOOL				GetRecipeList(LONG conn_id, CStringArray &recipe);
	VOID				RecipeRemoveAll();

	/* ������ ����/��ȯ */
	BOOL				SetRecipePath(PTCHAR path);
	BOOL				SetRecipeExtension(PTCHAR ext);
	PTCHAR				GetRecipePath()			{	return m_tzRecipePath;	};
	PTCHAR				GetRecipeExtension()	{	return m_tzRecipeExt;	};

	/* E40PJM::PRCommandCbk �Լ� ȣ��� ��, ����ϱ� ���� Substrate IDs�� ���� �� ���� */
	VOID				SetPRJobToSubstID(E40_PCCC cmd, PTCHAR pj_id, CStringArray *carr_id, CByteArray *slot_no);
	BOOL				RemoveProcessJobID(PTCHAR pj_id);
	BOOL				RemovePRJobToSubstID(PTCHAR pj_id);
	BOOL				UpdatePRJobToSubstID(PTCHAR st_id, E90_VSMC machine, INT32 state);
	BOOL				UpdatePRJobToSubstID(PTCHAR pj_id, BOOL auto_start, E40_PPJS pj_state,
											 CAtlList <STG_CSDI> *lst_state);
	BOOL				IsCheckedAllPRJobSubstState(PTCHAR pj_id, E90_VSMC machine, INT32 state);
	BOOL				IsCheckedOnePRJobSubstState(PTCHAR pj_id, E90_VSMC machine, INT32 state);
	BOOL				GetSubstIDStateAll(PTCHAR pj_id, CAtlList <STG_CSDI> &lst_state);
	BOOL				GetCountSubstID(PTCHAR pj_id, UINT32 &count);
	BOOL				GetPRJobState(PTCHAR pj_id, E40_PPJS &state);

	/* E90STS::Substrate ���� ���� ��� */
	BOOL				GetSubstrateInfoAll(CAtlList <STG_CSDI> &lst_subst);
	BOOL				GetSubstrateInfo(PTCHAR subst_id, STG_CSDI &subst_info);

	/* E94CJM::Callback �Լ� ȣ��� ��, ����ϱ� ���� ���� Ȥ�� ���� �� ���� */
	VOID				SetCRJobToProperty(E94_CCJS state, PTCHAR cj_id,
										   CStringArray *carr_id, CStringArray *pj_id);
	BOOL				UpdateCRJobToProperty(PTCHAR cj_id, E94_CCJS cj_state, BOOL auto_start,
											  CStringArray *pj_id, CStringList *carr_id);
	PTCHAR				GetCRJobToSubID(PTCHAR cj_id, UINT8 method, UINT32 index);
	VOID				GetCRJobToSubIDs(PTCHAR cj_id, UINT8 method, CStringArray &carr_ids);
	VOID				GetAllPRJobIDs(CStringArray &obj_ids);
	BOOL				GetAutoStart(PTCHAR cj_id, UINT32 index, BOOL &method);
	BOOL				RemoveControlJobID(PTCHAR cj_id);

	/* Load Port State */
	BOOL				SetLoadPortState(UINT16 port_id, ENG_LPOS state);
	ENG_LPOS			GetLoadPortState(UINT16 port_id);
	BOOL				UpdateLoadPortDataAll(UINT16 port_no, LPG_CLPI data);
	BOOL				GetLoadPortDataAll(UINT16 port_id, STG_CLPI &data);
	BOOL				GetCarrierIDDataAll(PTCHAR carr_id, STG_CLPI &data);

	/* Carrier ID ���� Process Job & Control Job IDs ��� ���� */
	BOOL				RemoveSharedCarrID(PTCHAR carr_id);

	/* SECS-II Message ��� (����) �۾� */
	BOOL				AddSecsMessage(STG_TSMD &mesg);
	BOOL				GetSecsMessage(LONG mesg_id, BOOL remove, CAtlList <STG_TSMV> &item);
	VOID				RemoveAllSecsMessage();
};