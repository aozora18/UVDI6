
/*
 desc : CIMConnect & CIM300 ������ ����
*/

#include "pch.h"
#include "SharedData.h"

#include "E40CmdParam.h"
#include "PRJobID.h"
#include "CRJobID.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 desc : ������
 parm : conn_cnt	- [in]  CONNECTIONx ����
 retn : None
*/
CSharedData::CSharedData(LONG conn_cnt)
{
	m_lLastSECSMsgTransID	= 0;

	memset(&m_stCookie, 0x00, sizeof(STM_GITC));
	wmemset(m_tzRecipePath, 0x00, MAX_PATH_LEN);
	wmemset(m_tzRecipeExt, 0x00, MAX_FILE_LEN);

	if (!m_mtxEvent.Open(MUTEX_SHARED_EVENT))			AfxMessageBox(L"Failed to create the [Shared Mutex:Event]", MB_ICONSTOP|MB_TOPMOST);
	if (!m_mtxRecipe.Open(MUTEX_SHARED_RECIPE))			AfxMessageBox(L"Failed to create the [Shared Mutex:Recipe]", MB_ICONSTOP|MB_TOPMOST);
	if (!m_mtxPRJobIDs.Open(MUTEX_E40_CALLBACK))		AfxMessageBox(L"Failed to create the [Shared Mutex:CRCallback]", MB_ICONSTOP|MB_TOPMOST);
	if (!m_mtxCRJobIDs.Open(MUTEX_E94_CALLBACK))		AfxMessageBox(L"Failed to create the [Shared Mutex:CRCallback]", MB_ICONSTOP|MB_TOPMOST);

	m_enProcessState	= E30_GPSV::en_idle;		/* Init Process State */
	m_enRecipeLoadAck	= E30_HPLA::en_rg_reset;	/* Recipe Load�� ���� ��û�� ���� ���� */
	m_enRecipeSendAck	= E30_SRHA::en_ra_reset;	/* ������ ������ �۽ſ� ���� ��û�� ���� ���� */
	m_enTermMsgAck		= E30_STHA::en_t_reset;		/* �͹̳� �޽��� �۽ſ� ���� ��û�� ���� ���� */
	m_enRecipeFileAck	= E30_RMJC::en_reset;		/* ������ ���� �۽ſ� ���� ��û�� ���� ���� */

	/* �ؽ��� ���̺� ũ�� ���� */
	m_mapCommState.InitHashTable(conn_cnt);
	m_mapCtrlState.InitHashTable(conn_cnt);
	m_mapSpoolState.InitHashTable(conn_cnt);
	/* �޸� �Ҵ� : Host�κ��� ��û���� Transaction ��û ���� �ӽ� ���� */
	m_stTransaction.Init();
	/* �޸� �Ҵ� : Load Port ��ü ���� ���� �ӽ� ���� (Carrier ID ����) */
	m_pstLoadPortState	= (LPG_CLPI)::Alloc(sizeof(STG_CLPI) * UINT32(MAX_FOUP_COUNT));
	ASSERT(m_pstLoadPortState);
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CSharedData::~CSharedData()
{
	POSITION pPos		= NULL;
	CPRJobID *pPRJobID	= NULL;
	CCRJobID *pCRJobID	= NULL;

	/* ���� ���� �ִ� �̺�Ʈ ��� ���� */
	RemoveEventAll();
	/* ������ ��� (����) �޸� ��ȯ */
	RecipeRemoveAll();
	/* ����ȭ ��ü ���� */
	m_mtxEvent.Close();
	m_mtxRecipe.Close();
	m_mtxPRJobIDs.Close();
	m_mtxCRJobIDs.Close();

	/* �ؽ��� �޸� ���� */
	m_mapCommState.RemoveAll();
	m_mapCtrlState.RemoveAll();
	m_mapSpoolState.RemoveAll();
	m_lstRecipe.RemoveAll();

	/* Process Job ID ���� */
	pPos	= m_lstPRJobID.GetHeadPosition();
	while (pPos)
	{
		pPRJobID	= m_lstPRJobID.GetNext(pPos);
		if (pPRJobID)	delete pPRJobID;
	}
	m_lstPRJobID.RemoveAll();

	/* Control Job ID ���� */
	pPos	= m_lstCRJobID.GetHeadPosition();
	while (pPos)
	{
		pCRJobID	= m_lstCRJobID.GetNext(pPos);
		if (pCRJobID)	delete pCRJobID;
	}
	m_lstCRJobID.RemoveAll();

	/* �޸� ���� : Host�κ��� ��û���� Transaction ��û ���� �ӽ� ���� */
	m_stTransaction.Close();
	/* �޸� ���� : Load Port ��ü ���� ���� �ӽ� ���� (Carrier ID ����) */
	if (m_pstLoadPortState)	::Free(m_pstLoadPortState);

	/* SECS-II Message �޸� ���� */
	RemoveAllSecsMessage();
}

/*
 desc : ��� CIM300 Event ����
 parm : None
 retn : None
*/
VOID CSharedData::RemoveEventAll()
{
	POSITION pPos	= NULL;
	STG_CCED stEvent= {NULL};

	CE40CmdParam *pE40CmdParam	= NULL;

	/* ���� ���� */
	if (m_mtxEvent.Lock())
	{
		pPos	= m_lstEvent.GetHeadPosition();
		while (pPos)
		{
			stEvent	= m_lstEvent.GetNext(pPos);
			if (stEvent.str_data2)	::Free(stEvent.str_data2);
			if (stEvent.str_data1)	::Free(stEvent.str_data1);

			/* �� Class Object �޸� ���� */
			switch (stEvent.exx_id)	/* GEM300 Standard Code */
			{
			case ENG_ESSC::en_e40_pjm	:
				switch (stEvent.cbf_id)
				{
				case E40_CEFI::en_pr_command_callback	:
				case E40_CEFI::en_pr_command_callback2	:
					pE40CmdParam	= (CE40CmdParam*)stEvent.evt_data.e40_pc.param_cmd;
					if (pE40CmdParam)	delete pE40CmdParam;
					break;
				}
				break;
			}
		}
		/* �̺�Ʈ ���� �޸� ���� */
		m_lstEvent.RemoveAll();
		/* ���� ���� */
		m_mtxEvent.Unlock();
	}
}

/*
 desc : CIM300Cbk�κ��� �߻��� �̺�Ʈ ó��
 parm : exx_id		- [in]  Event Main ID (ENG_ESSC)
		cbf_id		- [in]  �� Main ID�� Sub ID ��, Callback Function ID
		evt_data	- [in]  Event Data�� ����� ����ü ������
		str_data	- [in]  ���ڿ� ���۰� ����� ���� ������
		id_data1,2	- [in]  string_data1 / string_data2 ���� �� (ENG_GCPI)
		str_data1,2	- [in]  ���ڿ� ���۰� ����� ���� ������
 retn : None
*/
VOID CSharedData::PushEvent(ENG_ESSC exx_id, UINT8 cbf_id, LNG_CCED evt_data)
{
	/* ����ȭ ���� */
	if (m_mtxEvent.Lock())
	{
		STG_CCED stEvent= {NULL};
		/* �̺�Ʈ ���� ���� */
		stEvent.exx_id	= UINT16(exx_id);
		stEvent.cbf_id	= cbf_id;
		stEvent.id_data1= UINT8(ENG_GCPI::en_empty_param);
		stEvent.id_data2= UINT8(ENG_GCPI::en_empty_param);
		memcpy(&stEvent.evt_data, evt_data, sizeof(UNG_CCED));
		/* ����Ʈ ���ۿ� ��� */
		m_lstEvent.AddTail(stEvent);

		/* ����ȭ ���� */
		m_mtxEvent.Unlock();
	}
}
VOID CSharedData::PushEvent(ENG_ESSC exx_id, UINT8 cbf_id, LNG_CCED evt_data, ENG_GCPI id_data, PTCHAR data)
{
	/* ����ȭ ���� */
	if (m_mtxEvent.Lock())
	{
		UINT32 u32Size	= 0;
		STG_CCED stEvent= {NULL};

		/* �̺�Ʈ ���� ���� */
		stEvent.exx_id	= UINT16(exx_id);
		stEvent.cbf_id	= cbf_id;
		stEvent.id_data1= UINT8(id_data);
		stEvent.id_data2= UINT8(ENG_GCPI::en_empty_param);
		memcpy(&stEvent.evt_data, evt_data, sizeof(UNG_CCED));
		if (data && wcslen(data) > 0)
		{
			u32Size	= (UINT32)wcslen(data);
			stEvent.str_data1			= (PTCHAR)::Alloc(u32Size*sizeof(TCHAR)+sizeof(TCHAR));
			stEvent.str_data1[u32Size]	= 0x00;
			wmemcpy_s(stEvent.str_data1, u32Size, data, (UINT32)wcslen(stEvent.str_data1));
		}
		/* ����Ʈ ���ۿ� ��� */
		m_lstEvent.AddTail(stEvent);

		/* ����ȭ ���� */
		m_mtxEvent.Unlock();
	}
}
VOID CSharedData::PushEvent(ENG_ESSC exx_id, UINT8 cbf_id, LNG_CCED evt_data,
							ENG_GCPI id_data1, ENG_GCPI id_data2, PTCHAR data1, PTCHAR data2)
{
	/* ����ȭ ���� */
	if (m_mtxEvent.Lock())
	{
		UINT32 u32Size	= 0;
		STG_CCED stEvent= {NULL};

		/* �̺�Ʈ ���� ���� */
		stEvent.exx_id	= UINT16(exx_id);
		stEvent.cbf_id	= cbf_id;
		stEvent.id_data1= UINT8(id_data1);
		stEvent.id_data2= UINT8(id_data2);
		memcpy(&stEvent.evt_data, evt_data, sizeof(UNG_CCED));
		/* For data1 */
		if (data1 && wcslen(data1) > 0)
		{
			u32Size	= (UINT32)wcslen(data1);
			stEvent.str_data1			= (PTCHAR)::Alloc(u32Size*sizeof(TCHAR)+sizeof(TCHAR));
			stEvent.str_data1[u32Size]	= 0x00;
			wmemcpy_s(stEvent.str_data1, u32Size, data1, (UINT32)wcslen(stEvent.str_data1));

			/* For data2 */
			if (data2 && wcslen(data2) > 0)
			{
				u32Size	= (UINT32)wcslen(data2);
				stEvent.str_data2			= (PTCHAR)::Alloc(u32Size*sizeof(TCHAR)+sizeof(TCHAR));
				stEvent.str_data2[u32Size]	= 0x00;
				wmemcpy_s(stEvent.str_data2, u32Size, data2, (UINT32)wcslen(stEvent.str_data2));
			}
		}
		/* ����Ʈ ���ۿ� ��� */
		m_lstEvent.AddTail(stEvent);

		/* ����ȭ ���� */
		m_mtxEvent.Unlock();
	}
}


/*
 desc : CIM300Cbk�κ��� ��ϵ� �̺�Ʈ ��ȯ
 parm : data	- [in]  ��ȯ�� �̺�Ʈ ������ ����� ����ü ������
 retn : TRUE (��ȯ ������ ����) or FALSE (��ȯ ������ ����)
*/
BOOL CSharedData::PopEvent(STG_CCED &event)
{
	BOOL bPoped	= FALSE;

	/* ����ȭ ���� */
	if (m_mtxEvent.Lock())
	{
		if (m_lstEvent.GetCount())
		{
			/* ���� ������ �̺�Ʈ �׸���� ��ȯ */
			event	= m_lstEvent.GetHead();
			/* ��ȯ�� ������ ����Ʈ���� ����  */
			m_lstEvent.RemoveHead();
			/* ��ȯ�� ������ ���� */
			bPoped	= TRUE;
		}
		/* ����ȭ ���� */
		m_mtxEvent.Unlock();
	}

	return bPoped;	/* ��ȯ ���� ���� */
}

/*
 desc : Event Data�� ��� �ִ��� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CSharedData::IsEmptyEvent()
{
	BOOL bEmpty	= FALSE;

	/* ����ȭ ���� */
	if (m_mtxEvent.Lock())
	{
		bEmpty	= m_lstEvent.GetCount() < 1;	/* ��� �ִ��� ���� */
		/* ����ȭ ���� */
		m_mtxEvent.Unlock();
	}

	return bEmpty;
}

/* --------------------------------------------------------------------------------------------- */
/*                                            Common                                             */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Load Port�� Docked or Undocked Location Name ��ȯ
 parm : port_id	- [in]  Load Port ID (1 or 2)
		docked	- [in]  0x00 : Docked, 0x01 : Undocked
 retn : Docked or Undocked Location Name ��ȯ (������ ���, NULL)
*/
PTCHAR CSharedData::GetLoadPortDockedName(UINT16 port_id, UINT8 docked)
{
	PTCHAR ptzNamed	= NULL;

	if (port_id < 1 || port_id > 2)	return NULL;

	if (0x00 == docked)
	{
		wmemset(m_tzDockLocName, 0x00, 64);
		swprintf_s(m_tzDockLocName, 64, L"LP%dDocked", port_id);
		ptzNamed = m_tzDockLocName;
	}
	else
	{
		wmemset(m_tzUndockLocName, 0x00, 64);
		swprintf_s(m_tzUndockLocName, 64, L"LP%dUndocked", port_id);
		ptzNamed = m_tzUndockLocName;
	}

	return ptzNamed;
}

/*
 desc : Host���� ��û�� Transaction ���� �� ����
 parm : loc_id			- [in]  The ID for the location of the carrier
		carr_id			- [in]  The ID of the carrier
		data_seg		- [in]  The DataSeg parameter
		data_len		- [in]  The DataLength parameter
		transaction_id	- [in]  The transaction ID of S3F29 (Carrier Tag Read Request), pass it in to CCxE87CMS::AsyncCarrierTagReadComplete
 retn : None
*/
VOID CSharedData::SetTransactionInfo(PTCHAR loc_id, PTCHAR carr_id, PTCHAR data_seg,
									 LONG data_len, LONG transaction_id)
{
	m_stTransaction.transaction_id	= transaction_id;
	m_stTransaction.data_len		= data_len;
	swprintf_s(m_stTransaction.carr_id, CIM_CARRIER_ID_SIZE, L"%s", carr_id);
	swprintf_s(m_stTransaction.data_seg, CIM_DATA_SEGMENT_SIZE, L"%s", data_seg);
	swprintf_s(m_stTransaction.loc_id, CIM_CARRIER_LOC_SIZE, L"%s", loc_id);
}

/* --------------------------------------------------------------------------------------------- */
/*                                           ICxE30GEM                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : State Machine ���� �� ��ȯ
 parm : machine	- [in]  �߻��� ��ġ (E30_SSMS)
		conn_id	- [in] 0 or Later (���� ������ ���ؼ��� 0 �� ���)
 retn : state (������ ���, ���� ���� Ȯ��?�� ����)
*/
LONG CSharedData::E30_GetState(E30_SSMS machine, LONG conn_id)
{
	BOOL bFind	= FALSE;
	LONG lState	= -1;

	switch (machine)
	{
	case E30_SSMS::en_commstate	:	bFind = m_mapCommState.Lookup(conn_id, lState);		break;
	case E30_SSMS::en_control	:	bFind = m_mapCtrlState.Lookup(conn_id, lState);		break;
	case E30_SSMS::en_spooling	:	bFind = m_mapSpoolState.Lookup(conn_id, lState);	break;
	}
	return bFind ? lState : -1L;
}

/*
 desc : State Machine ���� �� ����
 parm : machine	- [in]  �߻��� ��ġ (E30_SSMS)
		conn_id	- [in] 0 or Later (���� ������ ���ؼ��� 0 �� ���)
		state	- [in]  �߻� �� (E87_CDSM ���� ����, E87_LPTS ~ E87_LPAM
 retn : None
*/
VOID CSharedData::E30_PushState(E30_SSMS machine, LONG conn_id, LONG state)
{
	UNG_CCED unEvent	= {NULL};

	/* Event ���� */
	unEvent.e30_sc.machine	= UINT8(machine);
	unEvent.e30_sc.conn_id	= conn_id;
	unEvent.e30_sc.new_state= state;

	switch (machine)
	{
	case E30_SSMS::en_commstate	:	m_mapCommState.SetAt(conn_id, state);	break;
	case E30_SSMS::en_control	:	m_mapCtrlState.SetAt(conn_id, state);	break;
	case E30_SSMS::en_spooling	:	m_mapSpoolState.SetAt(conn_id, state);	break;
	}

	/* Event ������ ��� */
	PushEvent(ENG_ESSC::en_e30_gem, (UINT8)E30_CEFI::en_state_change, &unEvent);
}

/*
 desc : Variable Value ���
 retn : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		var_id	- [in]  Variable ID
		var_name- [in]  Variable Name
 retn : None
*/
VOID CSharedData::E30_ValueChanged(LONG conn_id, LONG var_id, PTCHAR var_name)
{
	UNG_CCED unEvent	= {NULL};

	/* Event ���� */
	unEvent.e30_vc.conn_id	= conn_id;
	unEvent.e30_vc.var_id	= var_id;

	/* Event ������ ��� */
	PushEvent(ENG_ESSC::en_e30_gem, (UINT8)E30_CEFI::en_value_changed1,
			  &unEvent, ENG_GCPI::en_e30_variable_name, var_name);
}

/*
 desc : EC Value ���
 retn : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		var_name- [in]  Variable Name
 retn : None
*/
VOID CSharedData::E30_ECChanged(LONG conn_id, PTCHAR var_name)
{
	UNG_CCED unEvent	= {NULL};

	/* Event ���� */
	unEvent.e30_ec.conn_id	= conn_id;
	unEvent.e30_ec.var_id	= 0;

	/* Event ������ ��� */
	PushEvent(ENG_ESSC::en_e30_gem, (UINT8)E30_CEFI::en_ec_changed,
			  &unEvent, ENG_GCPI::en_e30_variable_name, var_name);
}

/*
 desc : Alarm ���
 retn : alarm_id- [in]  Alarm ID (Common�̹Ƿ�, Host ID�� �ʿ� ����)
		alarm_cd- [in]  Alarm Code
 retn : None
*/
VOID CSharedData::E30_AlarmChanged(LONG alarm_id, LONG alarm_cd)
{
	UNG_CCED unEvent	= {NULL};

	/* Event ���� */
	unEvent.e30_ac.alarm_id	= alarm_id;
	unEvent.e30_ac.alarm_cd	= alarm_cd;
	/* Event ������ ��� */
	PushEvent(ENG_ESSC::en_e30_gem, (UINT8)E30_CEFI::en_alarm_change, &unEvent);
}

/*
 desc : Process Program Data ���� Host ��û ���
 parm : cbf_id		- [in]  Host�κ��� ��û�� Callback Function ID
		conn_id		- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
 retn : recipe_name	- [in]  Recipe Name (TEL�� ���, Recipe File Name)
*/
VOID CSharedData::E30_PPData(E30_CEFI cbf_id, LONG conn_id, PTCHAR recipe_name)
{
	UNG_CCED unEvent	= {NULL};

	/* Event ���� */
	unEvent.e30_pp.conn_id	= conn_id;
	/* Event ������ ��� */
	PushEvent(ENG_ESSC::en_e30_gem, (UINT8)E30_CEFI::en_pp_request, &unEvent,
			  ENG_GCPI::en_e30_pp_recipe_name, recipe_name);
}

/*
 desc : Terminal Message ���
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		term_id	- [in]  Terminal ID, Common�� ���, ������ 0 ��
		mesg	- [in]  Terminal Message�� ����� ��ü
 retn : None
*/
VOID CSharedData::TerminalMsgRcvd(LONG conn_id, LONG term_id, PTCHAR mesg)
{
	UNG_CCED unEvent	= {NULL};

	/* Event ���� */
	unEvent.e30_tm.conn_id	= conn_id;
	unEvent.e30_tm.term_id	= term_id;
	/* Event ������ ��� */
	PushEvent(ENG_ESSC::en_e30_gem, (UINT8)E30_CEFI::en_terminal_msg_rcvd,
			  &unEvent, ENG_GCPI::en_e30_terminal_mesg, mesg);
}

/*
 desc : User Defined Message ���
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		msg_id	- [in]  Message ID (S1 ~ , F1 ~ )
		reply	- [in]  Reply �� (mNotHandled:0, mReplyLater:1, mReply:2, mNoReply:3)
 retn : None
*/
VOID CSharedData::MessageReceived(LONG conn_id, LONG mesg_id, LONG reply)
{
	UNG_CCED unEvent	= {NULL};

	/* Event ���� */
	unEvent.e30_mr.conn_id	= conn_id;
	unEvent.e30_mr.mesg_id	= mesg_id;
	unEvent.e30_mr.reply	= reply;
	/* Event ������ ��� */
	PushEvent(ENG_ESSC::en_e30_gem, (UINT8)E30_CEFI::en_message_received, &unEvent);
}

/*
 desc : ���� ��ɾ� (Remote Command) ���
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		rcmd_id	- [in]  ���ŵ� ���� ��� ID
		rcmd_str- [in]  ���� ��ɾ� (���ڿ�. Ȥ�� ����)
 retn : None
*/
VOID CSharedData::CommandCalled(LONG conn_id, E30_GRCI rcmd_id, PTCHAR rcmd_str)
{
	UNG_CCED unEvent	= {NULL};

	/* Event ���� */
	unEvent.e30_rc.conn_id	= conn_id;
	unEvent.e30_rc.rcmd_id	= LONG(rcmd_id);
	/* Event ������ ��� */
	PushEvent(ENG_ESSC::en_e30_gem, (UINT8)E30_CEFI::en_command_called,
			  &unEvent, ENG_GCPI::en_e30_remote_command, rcmd_str);
}

/* --------------------------------------------------------------------------------------------- */
/*                                        ICxE40PJM Area                                         */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : EC Value ���
 retn : prj_id	- [in]  Object identifier of Process Job for the command
		state	- [in]  Process Job State (E40_PPJS)
 retn : None
*/
VOID CSharedData::E40_PRStateChange(PTCHAR job_id, LONG state)
{
	UNG_CCED unEvent	= {NULL};
	/* Event ���� */
	unEvent.e40_ps.state= state;
	/* Event ������ ��� */
	PushEvent(ENG_ESSC::en_e40_pjm, (UINT8)E40_CEFI::en_pr_state_change, &unEvent,
			  ENG_GCPI::en_e40_process_job_id, job_id);
}

/*
 desc : Notification of a PRCommand service request
 parm : cbf_id		- [in]  Callback Function ID
		cmd_src		- [in]  Enumeration identifying the source of the command execution
		cmd			- [in]  Callback command code
		prj_id		- [in]  Object identifier of Process Job for the command
		param_cmd	- [in]  List of CmdParameter L {L {A CPNAME1}{CPVAL1}} ... {L {A CPNAMEn}{CPVALn}}.
							If command is pjcbCREATE, pjcbSETUP, or pjcbWAITINGFORSTART,
							this parameter is not used and the value will be set equal to NULL.
							If command is pjcbSTART and the ProcessJob state transition is
							from SETTING UP to WAITING FOR START or PROCESSING, this parameter is
							not used and the value will be set equal to NULL.
*/
VOID CSharedData::E40_PushCommnad(E40_CEFI cbf_id, E40_PECS cmd_src, E40_PCCC cmd, PTCHAR job_id,
								  PVOID param_cmd)
{
	UNG_CCED unEvent	= {NULL};

	unEvent.e40_pc.cmd_src	= UINT8(cmd_src);
	unEvent.e40_pc.cmd		= UINT8(cmd);
	unEvent.e40_pc.param_cmd= param_cmd;

	/* Event ������ ��� */
	PushEvent(ENG_ESSC::en_e40_pjm, (UINT8)cbf_id, &unEvent, ENG_GCPI::en_e40_prcommand_callback, job_id);
}
VOID CSharedData::E40_PushCommnad(E40_CEFI cbf_id, E40_PCCC cmd, PTCHAR job_id, PVOID param_cmd)
{
	UNG_CCED unEvent	= {NULL};

	unEvent.e40_pc.cmd		= UINT8(cmd);
	unEvent.e40_pc.param_cmd= param_cmd;

	/* Event ������ ��� */
	PushEvent(ENG_ESSC::en_e40_pjm, (UINT8)cbf_id, &unEvent, ENG_GCPI::en_e40_prcommand_callback, job_id);
}

/* --------------------------------------------------------------------------------------------- */
/*                                          ICxE87CMS Area                                       */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : State Machine ���� �� ����
 parm : machine	- [in]  �߻��� ��ġ (E87_CDSM)
		state	- [in]  �߻� �� (E87_CDSM ���� ����, E87_LPTS ~ E87_LPAM
		port_id	- [in]  The load port identifier (0 if port ID is not known)
		carr_id	- [in]  The carrier ID (may be an empty string)
 retn : None
*/
VOID CSharedData::E87_PushState(UINT8 machine, LONG state, INT16 port_id, PTCHAR carr_id)
{
	UNG_CCED unEvent	= {NULL};

	/* Event ���� */
	unEvent.e87_sc.machine	= machine;
	unEvent.e87_sc.port_id	= port_id;
	unEvent.e87_sc.state	= state;
	/* Event ������ ��� */
	PushEvent(ENG_ESSC::en_e87_cms, (UINT8)E87_CEFI::en_state_change, &unEvent,
			  ENG_GCPI::en_e87_carrier_id, carr_id);
}

/*
 desc : E87 ��ټ� �ݹ� �̺�Ʈ �� ����
 parm : cbf_id	- [in]  Callback Function ID
		state	- [in]  �߻� �� (E87_CDSM ���� ����, E87_LPTS ~ E87_LPAM
		port_id	- [in]  The load port identifier (0 if port ID is not known)
		carr_id	- [in]  The carrier ID (may be an empty string)
		loc_id	- [in]  The ID for the location of the carrier
 retn : None
*/
VOID CSharedData::E87_PushEvent(E87_CEFI cbf_id, LONG state,
								INT16 port_id, PTCHAR carr_id, PTCHAR loc_id)
{
	UNG_CCED unEvent	= {NULL};

	/* Event ���� */
	unEvent.e87_cf.port_id	= port_id;
	unEvent.e87_cf.state	= state;
	/* Event ������ ��� */
	PushEvent(ENG_ESSC::en_e87_cms, (UINT8)cbf_id, &unEvent,
			  ENG_GCPI::en_e87_carrier_id, ENG_GCPI::en_e87_location_id, carr_id, loc_id);
}

/*
 desc : E87 ��Ÿ �ݹ� �̺�Ʈ �� ����
 parm : cbf_id	- [in]  Callback Function ID
		state	- [in]  �߻� �� (E87_CDSM ���� ����, E87_LPTS ~ E87_LPAM
		port_id	- [in]  The load port identifier (0 if port ID is not known)
		type1	- [in]  ��Ÿ ���ڿ� 1 ���� �� (ENG_GCPI)
		type2	- [in]  ��Ÿ ���ڿ� 2 ���� �� (ENG_GCPI)
		data1	- [in]  ��Ÿ ���ڿ� 1
		data2	- [in]  ��Ÿ ���ڿ� 2
 retn : None
*/
VOID CSharedData::E87_PushEvent(E87_CEFI cbf_id, LONG state, INT16 port_id,
								ENG_GCPI type1, ENG_GCPI type2, PTCHAR data1, PTCHAR data2)
{
	UNG_CCED unEvent	= {NULL};

	/* Event ���� */
	unEvent.e87_cf.port_id	= port_id;
	unEvent.e87_cf.state	= state;
	/* Event ������ ��� */
	PushEvent(ENG_ESSC::en_e87_cms, UINT8(cbf_id), &unEvent, type1, type2, data1, data2);
}

/* --------------------------------------------------------------------------------------------- */
/*                                          ICxE90STS Area                                       */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Notification of a Command service request
 parm : cmd		- [in]  The command to be executed
		subst_id- [in]  The object ID for the substrate
*/
VOID CSharedData::E90_PushCommnad(E90_SPSC cmd, PTCHAR subst_id)
{
	UNG_CCED unEvent	= {NULL};

	unEvent.e90_cc.cmd	= UINT8(cmd);

	/* Event ������ ��� */
	PushEvent(ENG_ESSC::en_e90_sts, (UINT8)E90_CEFI::en_command_callback, &unEvent,
			  ENG_GCPI::en_e90_substrate_id, subst_id);
}

/*
 desc : E90 ��ټ� �ݹ� �̺�Ʈ �� ����
 parm : cbf_id		- [in]  Callback Function ID
		machine		- [in]  �߻� �� (E90_VSMC ���� ����)
		new_state	- [in]  The new state in the state model
							�� ���� machine (SubstrateStateMachineEnum) ���� ���� �޶���
							switch (machine)
							{
							case sisNO_SM					:	break;
							case sisSUBSTRATE_TRANSPORT_SM	:	SubstrateTransportStateEnum;	break;
							case sisSUBSTRATE_PROCESSING_SM	:	SubstrateProcessingStateEnum;	break;
							case sisSUBSTRATE_READER_SM		:	?
							case sisSUBSTRATE_LOCATION_SM	:	SubstrateLocationStateEnum;		break;
							case sisBATCH_LOCATION_SM		:	break;	// �� ���� ����
							}
		sub_obj		- [in]  The substrate object
		subst_id	- [in]  The substrate ID
 retn : None
*/
VOID CSharedData::E90_PushEvent(E90_CEFI cbf_id, E90_VSMC machine, LONG new_state, PTCHAR subst_id)
{
	UNG_CCED unEvent	= {NULL};

	/* Event ���� */
	unEvent.e90_sc.machine	= UINT8(machine);
	unEvent.e90_sc.state	= new_state;
	/* Event ������ ��� */
	PushEvent(ENG_ESSC::en_e90_sts, UINT8(cbf_id), &unEvent, ENG_GCPI::en_e90_substrate_id, subst_id);
}

/* --------------------------------------------------------------------------------------------- */
/*                                          ICxE94CJM Area                                       */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : E94 ��Ÿ �ݹ� �̺�Ʈ �� ����
 parm : cbf_id	- [in]  Callback Function ID
		type	- [in]  ��Ÿ ���ڿ� 1 ���� �� (ENG_GCPI)
		data	- [in]  ��Ÿ ���ڿ� (���� Process Job ID or Control Job ID)
 retn : None
*/
VOID CSharedData::E94_PushEvent(E94_CEFI cbf_id, ENG_GCPI type, PTCHAR data)
{
	UNG_CCED unEvent	= {NULL};

	/* Event ���� (���� ����) */

	/* Event ������ ��� */
	PushEvent(ENG_ESSC::en_e94_cjm, UINT8(cbf_id), &unEvent, type, data);
}

/*
 desc : GetNextPrJobID Event
 parm : cbf_id	- [in]  Callback Function ID
		type	- [in]  E94_CPOM �� (Process Job Order Management)
		job_id	- [in]  Control Job ID 
 retn : None
*/
VOID CSharedData::E94_PushEvent(E94_CEFI cbf_id, E94_CPOM type, PTCHAR job_id)
{
	UNG_CCED unEvent	= {NULL};

	/* Event ���� (���� ����) */
	unEvent.e94_po.state	= LONG(type);
	/* Event ������ ��� */
	PushEvent(ENG_ESSC::en_e94_cjm, UINT8(cbf_id), &unEvent, ENG_GCPI::en_e94_process_job_id, job_id);
}

/*
 desc : Control Job State ���� �� ����
 parm : state	- [in]  E94CJState (E94_CCJS)
		ctrl_id	- [in]  Control Job ID
 retn : None
*/
VOID CSharedData::E94_PushCJState(E94_CCJS state, PTCHAR ctrl_id)
{
	UNG_CCED unEvent	= {NULL};

	/* Event ���� */
	unEvent.e94_sc.state	= LONG(state);
	/* Event ������ ��� */
	PushEvent(ENG_ESSC::en_e94_cjm, (UINT8)E94_CEFI::en_cj_state_change, &unEvent,
			  ENG_GCPI::en_e94_control_job_id, ctrl_id);
}

/*
 desc : 
 parm : cbf_id	- [in]  Callback Function ID
		cmd_src	- [in]  Enumeration identifying the source of the command execution
		cmd		- [in]  Callback command code
		job_id	- [in]  Object identifier of Control Job for the command
*/
VOID CSharedData::E94_PushCommnad(E94_CEFI cbf_id, E94_PECS cmd_src, E94_CCMD cmd, PTCHAR job_id)
{
	UNG_CCED unEvent	= {NULL};

	unEvent.e94_pc.cmd_src	= UINT8(cmd_src);
	unEvent.e94_pc.cmd		= UINT8(cmd);

	/* Event ������ ��� */
	PushEvent(ENG_ESSC::en_e94_cjm, UINT8(cbf_id), &unEvent, ENG_GCPI::en_e94_cjcommand_callback, job_id);
}
VOID CSharedData::E94_PushCommnad(E94_CEFI cbf_id, E94_CCMD cmd, PTCHAR job_id)
{
	UNG_CCED unEvent	= {NULL};

	unEvent.e94_pc.cmd		= UINT8(cmd);

	/* Event ������ ��� */
	PushEvent(ENG_ESSC::en_e94_cjm, UINT8(cbf_id), &unEvent, ENG_GCPI::en_e94_cjcommand_callback, job_id);
}

/*
 desc : ������ (���μ��� ���α׷�; Process Program == Recipe) ���
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		file	- [in]  ������ �̸� (��ü ��ΰ� ���Ե� ����)
 retn : TRUE or FALSE
*/
BOOL CSharedData::RecipeAppend(LONG conn_id, PTCHAR file)
{
	BOOL bSucc	= FALSE;
	STG_CRFD stRecipe;

	/* ���� ���� */
	if (m_mtxRecipe.Lock())
	{
		/* ��� ������ �������� ������ �ʰ� �ߴ��� ���� */
		if (m_lstRecipe.GetCount() < MAX_RECIPE_COUNT)
		{
			stRecipe.conn_id= UINT32(conn_id);
			stRecipe.file	= (PTCHAR)::Alloc(sizeof(TCHAR) * MAX_PATH_LEN);
			wmemset(stRecipe.file, 0x00, MAX_PATH_LEN);
			wcscpy_s(stRecipe.file, MAX_PATH_LEN, file);
			m_lstRecipe.AddTail(stRecipe);
			bSucc	= TRUE;
		}

		/* ���� ���� */
		m_mtxRecipe.Unlock();
	}

	return bSucc;
}

/*
 desc : ������ (���μ��� ���α׷�; Process Program == Recipe) ����
 parm : conn_id	- [in]  ����� ��� ID, Common�� ���, ������ 0 ��
		file	- [in]  ������ �̸� (��ü ��ΰ� ���Ե� ����)
 retn : TRUE or FALSE
*/
BOOL CSharedData::RecipeRemove(LONG conn_id, PTCHAR file)
{
	BOOL bSucc		= FALSE;
	POSITION pPos	= NULL, pPrePos;
	STG_CRFD stRecipe;

	/* ���� ���� */
	if (m_mtxRecipe.Lock())
	{
		/* ��ü ������ŭ �ݺ��Ͽ� ������ ������ �̸��� �ִ��� �˻� */
		pPos	= m_lstRecipe.GetHeadPosition();
		while (pPos)
		{
			pPrePos	= pPos;
			stRecipe= m_lstRecipe.GetNext(pPos);
			if (stRecipe.file && (0 == wcscmp(stRecipe.file, file) && stRecipe.conn_id == conn_id))
			{
				::Free(stRecipe.file);
				m_lstRecipe.RemoveAt(pPos);
				bSucc	= TRUE;
				break;
			}
		}

		/* ���� ���� */
		m_mtxRecipe.Unlock();
	}

	return bSucc;
}

/*
 desc : ��� ������ (���μ��� ���α׷�; Process Program == Recipe) ����
 parm : None
 retn : None
*/
VOID CSharedData::RecipeRemoveAll()
{
	POSITION pPos	= NULL;
	STG_CRFD stRecipe;

	/* ���� ���� */
	if (m_mtxRecipe.Lock())
	{
		pPos	= m_lstRecipe.GetHeadPosition();
		while (pPos)
		{
			stRecipe	= m_lstRecipe.GetNext(pPos);
			if (stRecipe.file)	::Free(stRecipe.file);
		}
		m_lstRecipe.RemoveAll();
		/* ���� ���� */
		m_mtxRecipe.Unlock();
	}
}

/*
 desc : Host ID�� ��ġ�Ǵ� ������ ��ȯ
 parm : conn_id	- [in]  0 ���̸�, ��� ������ ��ȯ
		recipe	- [in]  ������ ����Ʈ�� ����� ����
 retn : TRUE or FALSE
*/
BOOL CSharedData::GetRecipeList(LONG conn_id, CStringArray &recipe)
{
	POSITION pPos	= NULL;
	STG_CRFD stRecipe;

	/* ���� ���� */
	if (m_mtxRecipe.Lock())
	{
		pPos	= m_lstRecipe.GetHeadPosition();
		while (pPos)
		{
			stRecipe	= m_lstRecipe.GetNext(pPos);
			/* ������ �̸� ��� */
			if (stRecipe.file && ((conn_id == 0x00 || stRecipe.conn_id == conn_id))
			{
				recipe.Add(stRecipe.file);
			}
		}
		/* ���� ���� */
		m_mtxRecipe.Unlock();
	}

	return recipe.GetCount() > 0;
}

/*
 desc : Process Program (=Recipe) ���� ��� ����
 parm : path	- [in]  ������ ��θ� (�� ������ '\' ����)
 retn : TRUE or FALSE
*/
BOOL CSharedData::SetRecipePath(PTCHAR path)
{
	UINT32 u32Size	= 0;
	if (!path)	return FALSE;

	/* ��� ���� */
	u32Size	= (UINT32)wcslen(path);
	if (u32Size < 3 /* ex> d:\ */)	return FALSE;
	/* �� ������ ���ڿ� '\' ���ԵǾ� �ִ��� */
	if (path[u32Size-1] == L'\\' || path[u32Size-1] == L'/')	return FALSE;

	/* ���ڿ� ���� */
	swprintf_s(m_tzRecipePath, MAX_FILE_LEN, L"%s", path);

	return TRUE;
}

/*
 desc : Process Program (=Recipe)�� Ȯ���� ����
 parm : ext	- [in]  ������ Ȯ����
 retn : TRUE or FALSE
*/
BOOL CSharedData::SetRecipeExtension(PTCHAR ext)
{
	if (!ext)	return FALSE;

	/* ��� ���� */
	swprintf_s(m_tzRecipeExt, MAX_FILE_LEN, L"%s", ext);
	return TRUE;
}

/*
 desc : PRJobID�� �ش�� ���� �� Substate ID ���� ��� Ȥ�� ����
 parm : cmd		- [in]  Command Code
		pj_id	- [in]  Process Job ID
		carr_id	- [in]  Process Job ID�� ��ϵ� Carrier ID ����Ʈ
		slot_no	- [in]  Process Job ID�� ��ϵ� Slot Number ����Ʈ
 retn : None
*/
VOID CSharedData::SetPRJobToSubstID(E40_PCCC cmd, PTCHAR pj_id, CStringArray *carr_id, CByteArray *slot_no)
{
	BOOL bFind			= FALSE;
	INT32 i	= 0;
	POSITION pPos		= NULL;
	CPRJobID *pPRJobID	= NULL;

	/* ����ȭ ���� */
	if (m_mtxPRJobIDs.Lock())
	{
		/* ���� Process Job ID�� ������ �޸� ���� */
		for (; i<m_lstPRJobID.GetCount() && !bFind; i++)
		{
			/* ��ü Position ��� */
			pPos	= m_lstPRJobID.FindIndex(i);
			if (pPos)
			{
				/* Process Job ID ��ü ��� */
				pPRJobID= (CPRJobID *)m_lstPRJobID.GetAt(pPos);
				/* �� ��, ������ PRJob ID�� �����Ѵٸ� */
				if (pPRJobID && (0 == wcscmp(pPRJobID->GetPRJobID(), pj_id)))
				{
					bFind	= TRUE;
				}
			}
		}

		/* ���� ������ ��� */
		if (cmd == E40_PCCC::en_create)
		{
			if (bFind)
			{
				delete pPRJobID;
				m_lstPRJobID.RemoveAt(pPos);
			}
			/* ���ο� PRJob ID ���� ��� */
			pPRJobID	= new CPRJobID(pj_id, cmd);
			ASSERT(pPRJobID);
			if (!pPRJobID->AddSubstState(carr_id, slot_no))	delete pPRJobID;
			else	m_lstPRJobID.AddTail(pPRJobID);
		}
		/* Process Command Code�� ����� ��� */
		else
		{
			if (bFind)	pPRJobID->UpdateCmdCode(cmd);
		}

		/* ����ȭ ���� */
		m_mtxPRJobIDs.Unlock();
	}
}

/*
 desc : Control Job ID ����
 ���� :	pj_id	- [in]  Process Job ID
 retn : None
*/
BOOL CSharedData::RemoveProcessJobID(PTCHAR pj_id)
{
	BOOL bFind			= FALSE;
	INT32 i	= 0;
	POSITION pPos		= NULL;
	CPRJobID *pPRJobID	= NULL;

	/* ����ȭ ���� */
	if (m_mtxPRJobIDs.Lock())
	{
		/* ���� Process Job ID�� ������ �޸� ���� */
		for (; i<m_lstPRJobID.GetCount() && !bFind; i++)
		{
			/* ��ü Position ��� */
			pPos	= m_lstPRJobID.FindIndex(i);
			if (pPos)
			{
				/* Process Job ID ��ü ��� */
				pPRJobID= (CPRJobID *)m_lstPRJobID.GetAt(pPos);
				/* �� ��, ������ PRJob ID�� �����Ѵٸ� */
				if (pPRJobID && (0 == wcscmp(pPRJobID->GetPRJobID(), pj_id)))
				{
					bFind	= TRUE;
				}
			}
		}
		if (bFind)
		{
			delete pPRJobID;
			m_lstPRJobID.RemoveAt(pPos);
		}

		/* ����ȭ ���� */
		m_mtxPRJobIDs.Unlock();
	}

	return bFind;
}

/*
 desc : E40PJM�� PRJobID �Ӽ� (��) ���� ����
 parm : prj_id	- [in]  Process Job ID
 retn : TRUE or FALSE
*/
BOOL CSharedData::RemovePRJobToSubstID(PTCHAR pj_id)
{
	BOOL bDeleted			= FALSE;
	POSITION pPos, pPrevPos	= NULL;
	CPRJobID *pPRJobID		= NULL;

	/* ����ȭ ���� */
	if (m_mtxPRJobIDs.Lock())
	{
		pPos	= m_lstPRJobID.GetHeadPosition();
		while (pPos && !bDeleted)
		{
			pPrevPos	= pPos;
			pPRJobID	= m_lstPRJobID.GetNext(pPos);
			/* ���� ����� PRJobID���� Ȯ�� */
			if (pPRJobID && (0 == wcscmp(pPRJobID->GetPRJobID(), pj_id)))
			{
				bDeleted	= TRUE;
				delete pPRJobID;
				/* �˻��� ��ġ �׸� ���� */
				m_lstPRJobID.RemoveAt(pPrevPos);
			}
		}

		/* ����ȭ ���� */
		m_mtxPRJobIDs.Unlock();
	}

	return bDeleted;
}

/*
 desc : E40PJM::PRCommandCbk ȣ��� ��, ����ϱ� ���� Substate IDs�� ���� ���� ����
		pj_id	- [in]  Process Job ID
		machine	- [in]  Substrate ID�� ���� ������ ������ Machine Type ���� ��
		state	- [in]  �����Ϸ��� Substate ID�� ���� ��
 retn : TRUE or FALSE
*/
BOOL CSharedData::UpdatePRJobToSubstID(PTCHAR st_id, E90_VSMC machine, INT32 state)
{
	INT32 i32Index			= -1;
	BOOL bIsUpdated			= FALSE;
	POSITION pPos, pPrevPos	= NULL;
	CPRJobID *pPRJobID		= NULL;

	/* ����ȭ ���� */
	if (m_mtxPRJobIDs.Lock())
	{
		pPos	= m_lstPRJobID.GetHeadPosition();
		while (pPos && !bIsUpdated)
		{
			pPrevPos	= pPos;
			pPRJobID	= m_lstPRJobID.GetNext(pPos);
			if (pPRJobID)
			{
				/* ������ Substate ID�� �����ϸ�, ���ο� ���� �� ���� */
				i32Index	= pPRJobID->IsFindSlotID(st_id);
				if (i32Index)
				{
					/* Machine Type�� ���� ���� ���� ���� */
					pPRJobID->UpdateSubstState(i32Index, machine, state);
					bIsUpdated	= TRUE;
				}
			}
		}

		/* ����ȭ ���� */
		m_mtxPRJobIDs.Unlock();
	}

	return bIsUpdated;
}

/*
 desc : E40PJM::PRCommandCbk ȣ��� ��, ����ϱ� ���� Substate IDs�� ���� ���� ����
		pj_id		- [in]  Process Job ID
		auto_start	- [in]  Process Job ID�� ���� ��� ��
		pj_state	- [in]  Process Job ID�� ���� ��
		lst_state	- [in]  �����Ϸ��� Substate ID�� ���� ���� ����� ����Ʈ ����
 retn : TRUE or FALSE
*/
BOOL CSharedData::UpdatePRJobToSubstID(PTCHAR pj_id, BOOL auto_start, E40_PPJS pj_state,
									   CAtlList <STG_CSDI> *lst_state)
{
	INT32 i32Index		= -1;
	BOOL bIsUpdated		= FALSE;
	POSITION pPos		= NULL;
	CPRJobID *pPRJobID	= NULL;

	/* ����ȭ ���� */
	if (m_mtxPRJobIDs.Lock())
	{
		pPos	= m_lstPRJobID.GetHeadPosition();
		while (pPos && !bIsUpdated)
		{
			pPRJobID	= m_lstPRJobID.GetNext(pPos);
			/* ������ Substate ID�� �����ϸ�, ���ο� ���� �� ���� */
			if (0 == wcscmp(pj_id, pPRJobID->GetPRJobID()))
			{
				/* Machine Type�� ���� ���� ���� ���� */
				pPRJobID->UpdateSubstStateAll(auto_start, pj_state, lst_state);
				bIsUpdated	= TRUE;
			}
		}

		/* ����ȭ ���� */
		m_mtxPRJobIDs.Unlock();
	}

	return bIsUpdated;
}

/*
 desc : ���� ��ϵ� Process Job ID ���� Substrate IDs�� ���°� ��� �Էµ� ���¿� �������� ����
 parm : pj_id	- [in]  �˻� ����� Process Job ID
		machine	- [in]  Substate Machine Type ��
		state	- [in]  �� ����� Substrate Processing State ��
 retn : �Էµ� state ���� ��� �����ϴٸ� TRUE, �� �� �̻� �ٸ� ���� �����Ѵٸ� FALSE
 */
BOOL CSharedData::IsCheckedAllPRJobSubstState(PTCHAR pj_id, E90_VSMC machine, INT32 state)
{
	BOOL bIsChecked		= TRUE;	/* ��� ������ ���� ���̶�� ���� */
	POSITION pPos		= NULL;
	CPRJobID *pPRJobID	= NULL;
	
	/* ��ϵ� ������ 1���� ������ */
	if (m_lstPRJobID.GetCount() < 1)	return FALSE;

	/* ����ȭ ���� */
	if (m_mtxPRJobIDs.Lock())
	{
		pPos	= m_lstPRJobID.GetHeadPosition();
		while (pPos && bIsChecked)
		{
			pPRJobID	= m_lstPRJobID.GetNext(pPos);
			if (pPRJobID && !pPRJobID->IsFindState(machine, state))
			{
				bIsChecked	= FALSE;
			}
		}

		/* ����ȭ ���� */
		m_mtxPRJobIDs.Unlock();
	}

	return bIsChecked;
}

/*
 desc : ���� ��ϵ� Process Job ID ���� Substrate IDs�� ���°� �Էµ� ���¿� ������ ���� 1���� �ִ��� ����
 parm : pj_id	- [in]  �˻� ����� Process Job ID
		machine	- [in]  Substate Machine Type ��
		state	- [in]  �� ����� Substrate Processing State ��
 retn : �Էµ� state ���� ������ ���� ���� �� ���� �����ϸ� TRUE
		�Էµ� state ���� ������ ���� ���� �� ���� ���ٸ� FALSE
 */
BOOL CSharedData::IsCheckedOnePRJobSubstState(PTCHAR pj_id, E90_VSMC machine, INT32 state)
{
	BOOL bIsChecked		= FALSE;	/* ��� ������ ���� ���� ���ٰ� ���� */
	POSITION pPos		= NULL;
	CPRJobID *pPRJobID	= NULL;
	
	/* ��ϵ� ������ 1���� ������ */
	if (m_lstPRJobID.GetCount() < 1)	return FALSE;

	/* ����ȭ ���� */
	if (m_mtxPRJobIDs.Lock())
	{
		pPos = m_lstPRJobID.GetHeadPosition();
		while (pPos && bIsChecked)
		{
			pPRJobID = m_lstPRJobID.GetNext(pPos);
			if (pPRJobID && pPRJobID->IsFindState(machine, state))
			{
				bIsChecked	= TRUE;
			}
		}

		/* ����ȭ ���� */
		m_mtxPRJobIDs.Unlock();
	}

	return bIsChecked;
}

/*
 desc : ���� ��ġ�� ��ϵ� PRJob IDs�� ��ϵ� Substrate IDs�� ��� ���� ������ ��ȯ
 parm : pj_id	- [in]  �˻� ����� Process Job ID
		subst	- [out] Substrate ID�� ���� �� ��Ÿ �������� ����� ����Ʈ �޸� (PRCommandCbk �Լ����� �ַ� ����?)
 retn : TRUE or FALSE
*/
BOOL CSharedData::GetSubstIDStateAll(PTCHAR pj_id, CAtlList <STG_CSDI> &lst_state)
{
	BOOL bIsChecked		= FALSE;
	POSITION pPos		= NULL;
	CPRJobID *pPRJobID	= NULL;

	if (!pj_id)	return FALSE;

	/* ����ȭ ���� */
	if (m_mtxPRJobIDs.Lock())
	{
		pPos	= m_lstPRJobID.GetHeadPosition();
		while (pPos)
		{
			pPRJobID	= m_lstPRJobID.GetNext(pPos);
			if (0 == wcscmp(pj_id, pPRJobID->GetPRJobID()))
			{
				bIsChecked	= pPRJobID->GetSubstIDStateAll(lst_state);
				break;
			}
		}

		/* ����ȭ ���� */
		m_mtxPRJobIDs.Unlock();
	}

	return TRUE;
}

/*
 desc : PRJob ID ���� ��ϵ� Subsatre ID ���� ��ȯ
 parm : pj_id	- [in]  �˻� ����� Process Job ID
		count	- [out] ���� �� ��ȯ (Zero-based)
 retn : TRUE or FALSE
*/
BOOL CSharedData::GetCountSubstID(PTCHAR pj_id, UINT32 &count)
{
	BOOL bIsChecked		= FALSE;
	POSITION pPos		= NULL;
	CPRJobID *pPRJobID	= NULL;

	if (!pj_id)	return FALSE;

	/* ����ȭ ���� */
	if (m_mtxPRJobIDs.Lock())
	{
		pPos	= m_lstPRJobID.GetHeadPosition();
		while (pPos)
		{
			pPRJobID	= m_lstPRJobID.GetNext(pPos);
			if (0 == wcscmp(pj_id, pPRJobID->GetPRJobID()))
			{
				count		= pPRJobID->GetCountSubstID();
				bIsChecked	= TRUE;
				break;
			}
		}

		/* ����ȭ ���� */
		m_mtxPRJobIDs.Unlock();
	}

	return bIsChecked;
}

/*
 desc : PRJob ID ���� �۾� ���� �� ��ȯ
 parm : pj_id	- [in]  �˻� ����� Process Job ID
		state	- [out] �۾� ���°� ��ȯ�� ���� ����
 retn : TRUE or FALSE
*/
BOOL CSharedData::GetPRJobState(PTCHAR pj_id, E40_PPJS &state)
{
	BOOL bIsChecked		= FALSE;
	POSITION pPos		= NULL;
	CPRJobID *pPRJobID	= NULL;

	if (!pj_id)	return FALSE;

	/* ����ȭ ���� */
	if (m_mtxPRJobIDs.Lock())
	{
		pPos	= m_lstPRJobID.GetHeadPosition();
		while (pPos)
		{
			pPRJobID	= m_lstPRJobID.GetNext(pPos);
			if (0 == wcscmp(pj_id, pPRJobID->GetPRJobID()))
			{
				state		= pPRJobID->GetPRJobState();
				bIsChecked	= TRUE;
				break;
			}
		}

		/* ����ȭ ���� */
		m_mtxPRJobIDs.Unlock();
	}

	return bIsChecked;
}

/*
 desc : Process Job IDs�� ��ϵ� ��� Substrate Information ��� (���� �� Carrier ID)
 parm : lst_subst	- [out] ��� Substrate Information ������ ����� ����Ʈ ���� ���� ����
 retn : TRUE or FALSE
*/
BOOL CSharedData::GetSubstrateInfoAll(CAtlList <STG_CSDI> &lst_subst)
{
	BOOL bIsChecked		= FALSE;
	POSITION pPos		= NULL;
	CPRJobID *pPRJobID	= NULL;
	CAtlList <STG_CSDI> lstSubstAll;

	/* ����ȭ ���� */
	if (m_mtxPRJobIDs.Lock())
	{
		pPos	= m_lstPRJobID.GetHeadPosition();
		while (pPos)
		{
			pPRJobID	= m_lstPRJobID.GetNext(pPos);
			if (pPRJobID)
			{
				pPRJobID->GetSubstIDStateAll(lstSubstAll);
				lst_subst.AddTailList(&lstSubstAll);
				/* ���� �ӽ� ����Ʈ ���� �޸� ���� */
				lstSubstAll.RemoveAll();
			}
		}

		/* ����ȭ ���� */
		m_mtxPRJobIDs.Unlock();
	}

	return TRUE;
}

/*
 desc : Process Job IDs�� ��ϵ� ���� Substrate Information ��� (���� �� Carrier ID)
 parm : subst_id	- [in]  �˻��ϰ��� �ϴ� Substrate ID
		subst_info	- [out] Substrate Information ������ ����� ���� ����
 retn : TRUE or FALSE
*/
BOOL CSharedData::GetSubstrateInfo(PTCHAR subst_id, STG_CSDI &subst_info)
{
	BOOL bIsChecked		= FALSE;
	POSITION pPos		= NULL;
	CPRJobID *pPRJobID	= NULL;

	/* ����ȭ ���� */
	if (m_mtxPRJobIDs.Lock())
	{
		pPos	= m_lstPRJobID.GetHeadPosition();
		while (pPos && !bIsChecked)
		{
			pPRJobID	= m_lstPRJobID.GetNext(pPos);
			if (pPRJobID && pPRJobID->GetSubstIDState(subst_id, subst_info))
			{
				bIsChecked	= TRUE;
			}
		}

		/* ����ȭ ���� */
		m_mtxPRJobIDs.Unlock();
	}

	return bIsChecked;
}

/*
 desc : PRJobID�� �ش�� ���� �� Substate ID ���� ��� Ȥ�� ����
 parm : state	- [in]  Control Job State
		cj_id	- [in]  Control Job ID
		carr_id	- [in]  Control Job ID�� ��ϵ� Carrier ID ����Ʈ
		pj_id	- [in]  Control Job ID�� ��ϵ� Process Job ID ����Ʈ
 retn : None
*/
VOID CSharedData::SetCRJobToProperty(E94_CCJS state, PTCHAR cj_id,
									 CStringArray *carr_id, CStringArray *pj_id)
{
	BOOL bIsFind		= FALSE;
	INT32 i	= 0;
	POSITION pPos		= NULL;
	CCRJobID *pCRJobID	= NULL;

	/* ����ȭ ���� */
	if (m_mtxCRJobIDs.Lock())
	{
		/* ���� Process Job ID�� ������ �޸� ���� */
		for (; i<m_lstCRJobID.GetCount() && !bIsFind; i++)
		{
			/* ��ü Position ��� */
			pPos	= m_lstCRJobID.FindIndex(i);
			/* Process Job ID ��ü ��� */
			pCRJobID= (CCRJobID *)m_lstCRJobID.GetAt(pPos);
			/* �� ��, ������ PRJob ID�� �����Ѵٸ� */
			if (pCRJobID && (0 == wcscmp(pCRJobID->GetCRJobID(), cj_id)))
			{
				bIsFind	= TRUE;
			}
		}

		if (bIsFind)
		{
			delete pCRJobID;
			m_lstCRJobID.RemoveAt(pPos);
		}

		/* ���ο� PRJob ID ���� ��� */
		pCRJobID	= new CCRJobID(cj_id);
		ASSERT(pCRJobID);
		pCRJobID->AddProperty(pj_id, carr_id);
		m_lstCRJobID.AddTail(pCRJobID);

		/* ����ȭ ���� */
		m_mtxCRJobIDs.Unlock();
	}
}

/*
 desc : Control Job ID ����
 ���� :	cj_id	- [in]  Control Job ID
 retn : None
*/
BOOL CSharedData::RemoveControlJobID(PTCHAR cj_id)
{
	BOOL bIsFind		= FALSE;
	INT32 i	= 0;
	POSITION pPos		= NULL;
	CCRJobID *pCRJobID	= NULL;

	/* ����ȭ ���� */
	if (m_mtxCRJobIDs.Lock())
	{
		/* ���� Process Job ID�� ������ �޸� ���� */
		for (; i<m_lstCRJobID.GetCount() && !bIsFind; i++)
		{
			/* ��ü Position ��� */
			pPos	= m_lstCRJobID.FindIndex(i);
			/* Process Job ID ��ü ��� */
			pCRJobID= (CCRJobID *)m_lstCRJobID.GetAt(pPos);
			/* �� ��, ������ PRJob ID�� �����Ѵٸ� */
			if (pCRJobID && (0 == wcscmp(pCRJobID->GetCRJobID(), cj_id)))
			{
				bIsFind	= TRUE;
				break;
			}
		}
		if (bIsFind)
		{
			delete pCRJobID;
			m_lstCRJobID.RemoveAt(pPos);
		}

		/* ����ȭ ���� */
		m_mtxCRJobIDs.Unlock();
	}

	return bFind;
}

/*
 desc : E94CJM::Callback ȣ��� ��, ����ϱ� ���� �Ӽ� ���� ����
 ���� :	cj_id		- [in]  Control Job ID
		auto_start	- [in]  Control Job ID�� AutoStart ���� ����
		cj_state	- [in]  Control Job ID�� ���� ��
		pj_id		- [in]  Process Job ID�� ����� �迭 ������
		carr_id		- [in]  Control Job ID ���� Carrier ID�� ����� �迭 ������
 retn : TRUE or FALSE
*/
BOOL CSharedData::UpdateCRJobToProperty(PTCHAR cj_id, E94_CCJS cj_state, BOOL auto_start,
										CStringArray *pj_id, CStringList *carr_id)
{
	INT32 i32Index		= -1;
	BOOL bIsUpdated		= FALSE;
	POSITION pPos		= NULL;
	CCRJobID *pCRJobID	= NULL;

	/* ����ȭ ���� */
	if (m_mtxCRJobIDs.Lock())
	{
		pPos	= m_lstCRJobID.GetHeadPosition();
		while (pPos && !bIsUpdated)
		{
			pCRJobID	= m_lstCRJobID.GetNext(pPos);
			/* ������ Substate ID�� �����ϸ�, ���ο� ���� �� ���� */
			if (0 == wcscmp(cj_id, pCRJobID->GetCRJobID()))
			{
				/* Machine Type�� ���� ���� ���� ���� */
				pCRJobID->UpdateProperty(cj_state, auto_start, pj_id, carr_id);
				bIsUpdated	= TRUE;
			}
		}

		/* ����ȭ ���� */
		m_mtxCRJobIDs.Unlock();
	}

	return bIsUpdated;
}

/*
 desc : ���� �ε��� (Zero-based) ��ġ�� �ִ� Process Job ID or Carrier ID �� ��ȯ
 parm : cj_id	- [in]  �˻� ����� Control Job ID
		method	- [in]  0x00: Process Job ID, 0x01: Carrier ID
		index	- [in]  Zero-based Index �� (Process Job ID or Carrier ID�� ����� �迭 �ε���)
 retn : Process Job ID or Carrier ID �� ��ȯ (NULL�̸�, �� �̻� �������� ����)
*/
PTCHAR CSharedData::GetCRJobToSubID(PTCHAR cj_id, UINT8 method, UINT32 index)
{
	INT32 i32Index		= -1;
	BOOL bIsUpdated		= FALSE;
	PTCHAR ptzObjID		= NULL;
	POSITION pPos		= NULL;
	CCRJobID *pCRJobID	= NULL;

	/* ����ȭ ���� */
	if (m_mtxCRJobIDs.Lock())
	{
		pPos	= m_lstCRJobID.GetHeadPosition();
		while (pPos && !bIsUpdated)
		{
			pCRJobID	= m_lstCRJobID.GetNext(pPos);
			/* ������ Substate ID�� �����ϸ�, ���ο� ���� �� ���� */
			if (0 == wcscmp(cj_id, pCRJobID->GetCRJobID()))
			{
				if (method)	ptzObjID = pCRJobID->GetCarrID(index);
				else		ptzObjID = pCRJobID->GetPRJobID(index);
				bIsUpdated	= TRUE;
			}
		}
		/* ����ȭ ���� */
		m_mtxCRJobIDs.Unlock();
	}

	return ptzObjID;
}

/*
 desc : Control Job ID�� ���� ��� Process Job ID or Carrier ID ��ȯ
 parm : cj_id	- [in]  �˻� ����� Control Job ID
		method	- [in]  0x00: Process Job ID, 0x01: Carrier ID
		obj_ids	- [out] Process Job IDs or Carrier IDs�� ��ȯ�Ǿ� ����� �迭 ����Ʈ ����
 retn : None
*/
VOID CSharedData::GetCRJobToSubIDs(PTCHAR cj_id, UINT8 method, CStringArray &obj_ids)
{
	INT32 i32Index		= -1;
	BOOL bIsUpdated		= FALSE;
	POSITION pPos		= NULL;
	CCRJobID *pCRJobID	= NULL;

	/* ����ȭ ���� */
	if (m_mtxCRJobIDs.Lock())
	{
		pPos	= m_lstCRJobID.GetHeadPosition();
		while (pPos && !bIsUpdated)
		{
			pCRJobID	= m_lstCRJobID.GetNext(pPos);
			/* ������ Substate ID�� �����ϸ�, ���ο� ���� �� ���� */
			if (0 == wcscmp(cj_id, pCRJobID->GetCRJobID()))
			{
				/* Control Job ID�� ��ϵ� ��� Carrier IDs �������� */
				if (method)	pCRJobID->GetCarrIDs(obj_ids);
				else		pCRJobID->GetPRJobIDs(obj_ids);
				bIsUpdated	= TRUE;
			}
		}
		/* ����ȭ ���� */
		m_mtxCRJobIDs.Unlock();
	}
}

/*
���� : ��� Process Job ID ����Ʈ ��ȯ
���� : 
obj_ids	- [out] Process Job IDs or Carrier IDs�� ��ȯ�Ǿ� ����� �迭 ����Ʈ ����
��ȯ : None
*/
VOID CSharedData::GetAllPRJobIDs(CStringArray &obj_ids)
{
	POSITION pPos	= NULL;
	CStringArray arrPRJobIDs;

	/* ����ȭ ���� */
	if (m_mtxCRJobIDs.Lock())
	{
		pPos = m_lstCRJobID.GetHeadPosition();
		while (pPos)
		{
			CCRJobID *pCRJobID = NULL;
			pCRJobID = m_lstCRJobID.GetNext(pPos);

			/* CRJob ID�� ���� PRJob IDs ��� */
			pCRJobID->GetPRJobIDs(arrPRJobIDs);
			if (arrPRJobIDs.GetCount() > 0)
			{
				obj_ids.Append(arrPRJobIDs);
				arrPRJobIDs.RemoveAll();	/* ���� �׸� ���� */
			}
		}
		/* ����ȭ ���� */
		m_mtxCRJobIDs.Unlock();
	}
}

/*
 desc : ���� �ε��� (Zero-based) ��ġ�� �ִ� Process Job ID�� ���� ���� �� ��ȯ
 parm : cj_id	- [in]  �˻� ����� Control Job ID
		index	- [in]  Zero-based Index �� (Process Job ID�� ����� �迭 �ε���)
		method	- [out] Process Job ID�� ���� ���� ���� ��ȯ�� ���� ����
 retn : TRUE (��ȯ ����) or FALSE (��ȯ ����. �˻� ����)
*/
BOOL CSharedData::GetAutoStart(PTCHAR cj_id, UINT32 index, BOOL &method)
{
	INT32 i32Index		= -1;
	BOOL bIsUpdated		= FALSE;
	PTCHAR ptzPRJobID	= NULL;
	POSITION pPos		= NULL;
	CCRJobID *pCRJobID	= NULL;

	/* ����ȭ ���� */
	if (m_mtxCRJobIDs.Lock())
	{
		pPos	= m_lstCRJobID.GetHeadPosition();
		while (pPos && !bIsUpdated)
		{
			pCRJobID	= m_lstCRJobID.GetNext(pPos);
			/* ������ Substate ID�� �����ϸ�, ���ο� ���� �� ���� */
			if (0 == wcscmp(cj_id, pCRJobID->GetCRJobID()))
			{
				bIsUpdated	= pCRJobID->GetAutoStart(index, method);
			}
		}
		/* ����ȭ ���� */
		m_mtxCRJobIDs.Unlock();
	}

	return bIsUpdated;
}

/*
 desc : Load Port�� ���� State (ENG_LPOS) �� ����
 parm : port_no	- [in]  Load Port Number (1-based)
		state	- [in]  Load Port State �� (ENG_LPOS)
 retn : TRUE or FALSE
*/
BOOL CSharedData::SetLoadPortState(UINT16 port_id, ENG_LPOS state)
{
	if (port_id < 1 || port_id > MAX_FOUP_COUNT)	return FALSE;

	/* State ���� ���� */
	m_enLoadPortState[port_id-1]	= state;

	return TRUE;
}

/*
 desc : ���� Load Port�� ���� State (ENG_LPOS) �� ��ȯ
 parm : port_no	- [in]  Load Port Number (1-based)
 retn : ���� ��
*/
ENG_LPOS CSharedData::GetLoadPortState(UINT16 port_id)
{
	if (port_id < 1 || port_id > MAX_FOUP_COUNT)	return ENG_LPOS::en_unknown;

	return m_enLoadPortState[port_id-1];
}

/*
 desc : Load Port/Carrier ID�� ���õ� ���� ����
 parm : port_no	- [in]  Load Port Number (1-based)
		data	- [in]  8���� ������ ���� ���� �� Carrier ID ���� ������ ����� ����ü ������
 retn : TRUE or FALSE
*/
BOOL CSharedData::UpdateLoadPortDataAll(UINT16 port_id, LPG_CLPI data)
{
	if (port_id < 1 || port_id > MAX_FOUP_COUNT)	return FALSE;
	/* Load Port Number �������� Load Port �� Carrier ID ���� ���� */
	memcpy(&m_pstLoadPortState[port_id-1], data, sizeof(STG_CLPI));

	return TRUE;
}

/*
 desc : Load Port Number�� ���õ� ���� ��ȯ
 parm : port_no	- [in]  Load Port Number (1-based)
		data	- [in]  8���� ������ ���� ���� �� Carrier ID ���� ������ ����� ����ü ������
 retn : TRUE or FALSE
*/
BOOL CSharedData::GetLoadPortDataAll(UINT16 port_id, STG_CLPI &data)
{
	if (port_id < 1 || port_id > MAX_FOUP_COUNT)	return FALSE;
	/* Load Port Number �������� Load Port �� Carrier ID ���� ��ȯ */
	memcpy(&data, &m_pstLoadPortState[port_id-1], sizeof(STG_CLPI));

	return TRUE;
}

/*
 desc : Carrier ID�� ���õ� ���� ��ȯ
 parm : carr_id	- [in]  �˻� ����� Carrier ID
		data	- [in]  8���� ������ ���� ���� �� Carrier ID ���� ������ ����� ����ü ������
 retn : TRUE or FALSE
*/
BOOL CSharedData::GetCarrierIDDataAll(PTCHAR carr_id, STG_CLPI &data)
{
	UINT16 i	= 0x00;
	BOOL bFind	= FALSE;

	for (; i<MAX_FOUP_COUNT; i++)
	{
		if (0 == wcscmp(m_pstLoadPortState[i].carr_id, carr_id))
		{
			memcpy(&data, &m_pstLoadPortState[i], sizeof(STG_CLPI));
			bFind	= TRUE;
		}
	}

	return bFind;
}

/*
 desc : Carrier ID ����
 parm : carr_id	- [in]  ���� ����� Carrier ID
 retn : TRUE (���� ����), FALSE (���� ����)
*/
BOOL CSharedData::RemoveSharedCarrID(PTCHAR carr_id)
{
	INT32 i	= 0;
	POSITION pPos	= NULL;
	CPRJobID *pPRJobID;
	CCRJobID *pCRJobID;

	/* ---------------------------------------- */
	/* Carrier ID�� ���õ� Process Job IDs ���� */
	/* ---------------------------------------- */

	/* ����ȭ ���� */
	if (m_mtxPRJobIDs.Lock())
	{
		/* ���� Process Job ID�� ������ �޸� ���� */
		for (i=0; i<m_lstPRJobID.GetCount(); i++)
		{
			/* ��ü Position ��� */
			pPos	= m_lstPRJobID.FindIndex(i);
			/* Process Job ID ��ü ��� */
			pPRJobID= (CPRJobID *)m_lstPRJobID.GetAt(pPos);
			/* Process Job ID�� ���� �ִ� Carrier�� �����ϸ� Process Job ID ���� */
			if (pPRJobID && pPRJobID->IsFindCarrID(carr_id))
			{
				delete pPRJobID;
				m_lstPRJobID.RemoveAt(pPos);
				/* �ٽ� ó������ �˻��ؾ� �� */
				i = 0;	/* !!! important !!! */
			}
		}

		/* ����ȭ ���� */
		m_mtxPRJobIDs.Unlock();
	}

	/* ---------------------------------------- */
	/* Carrier ID�� ���õ� Control Job IDs ���� */
	/* ---------------------------------------- */
	/* ����ȭ ���� */
	if (m_mtxCRJobIDs.Lock())
	{
		/* ���� Process Job ID�� ������ �޸� ���� */
		for (i=0; i<m_lstCRJobID.GetCount(); i++)
		{
			/* ��ü Position ��� */
			pPos	= m_lstCRJobID.FindIndex(i);
			/* Process Job ID ��ü ��� */
			pCRJobID= (CCRJobID *)m_lstCRJobID.GetAt(pPos);
			/* Control Job ID�� ���� �ִ� Carrier�� �����ϸ� Contron Job ID ���� */
			if (pCRJobID && pCRJobID->IsFindCarrID(carr_id))
			{
				delete pCRJobID;
				m_lstCRJobID.RemoveAt(pPos);
				/* �ٽ� ó������ �˻��ؾ� �� */
				i = 0;	/* !!! important !!! */
			}
		}

		/* ����ȭ ���� */
		m_mtxCRJobIDs.Unlock();
	}

	return TRUE;
}

/*
 desc : SECS-II Message ��� (����� ��, ���� ��ϵ� ������ ������, ���� �� ���� ���)
 parm : mesg	- [in]  ��ϵ� SECS-II Message�� ����� ����ü ����
 retn : TRUE (���� ���) or FALSE (���� �޽��� ���� �� ���)
*/
BOOL CSharedData::AddSecsMessage(STG_TSMD &mesg)
{
	POSITION pPos, pPrePos;
	STG_TSMD stMesg	= {NULL};

	pPos	= m_lstSecsMesg.GetHeadPosition();
	while (pPos)
	{
		pPrePos = pPos;
		stMesg	= m_lstSecsMesg.GetNext(pPos);
		/* ���� ������ SECS-II Message�� ���, ���� �޽��� ���� */
		if (stMesg.mesg_id == mesg.mesg_id)
		{
			/* ���� ������ ���� */
			if (stMesg.value)	::Free(stMesg.value);
			m_lstSecsMesg.RemoveAt(pPrePos);

			/* ���� ���ŵ� �޽��� ��� */
			m_lstSecsMesg.AddTail(mesg);

			/* ���� ��ϵ� SECS-II Message ���� �� ���� ���ŵ� �޽����� ��� */
			return FALSE;
		}
	}

	/* ���Ӱ� SECS-II Message ��� */
	m_lstSecsMesg.AddTail(mesg);

	return TRUE;
}

/*
 desc : ���� ��ϵ� SECS-II Message ��ȯ
 parm : mesg_id	- [in]  �˻� ����� Message ID �� (Stream/Function���� ������ 16���� Hexa-Decimal ��)
		remove	- [in]  �˻��� ����� ��ȯ �� �޸𸮿��� �������� ����
		item	- [out] �˻��� �����Ͱ� ��ȯ�� ����Ʈ ����
 retn : TRUE or FALSE
*/
BOOL CSharedData::GetSecsMessage(LONG mesg_id, BOOL remove, CAtlList <STG_TSMV> &item)
{
	LONG i;
	POSITION pPos, pPrePos;
	STG_TSMV stVals	= {NULL};
	STG_TSMD stMesg	= {NULL};

	pPos	= m_lstSecsMesg.GetHeadPosition();
	while (pPos)
	{
		pPrePos = pPos;
		stMesg	= m_lstSecsMesg.GetNext(pPos);
		/* ���� ������ SECS-II Message�� ���, ���� �޽��� ���� */
		if (stMesg.mesg_id == mesg_id)
		{
			/* �˻��� ������ ��ȯ */
			for (i=0; i<stMesg.count; i++)
			{
				memcpy(&stVals, &stMesg.value[i], sizeof(STG_TSMV));
				item.AddTail(stVals);
			}

			/* ���� ������ ���� */
			if (stMesg.value)	::Free(stMesg.value);
			m_lstSecsMesg.RemoveAt(pPrePos);

			/* ��ȯ ���� */
			return TRUE;
		}
	}

	/* ��ȯ ���� */
	return FALSE;
}

/* 
 desc : SECS-II Message �޸� ����
 parm : None
 retn : None
*/
VOID CSharedData::RemoveAllSecsMessage()
{
	POSITION pPos		= NULL;
	STG_TSMD stSecsMesg	= {NULL};

	pPos = m_lstSecsMesg.GetHeadPosition();
	while (pPos)
	{
		stSecsMesg	= m_lstSecsMesg.GetNext(pPos);
		if (stSecsMesg.value)	::Free(stSecsMesg.value);
	}
	m_lstSecsMesg.RemoveAll();
}

