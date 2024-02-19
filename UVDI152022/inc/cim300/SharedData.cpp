
/*
 desc : CIMConnect & CIM300 데이터 공유
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
 desc : 생성자
 parm : conn_cnt	- [in]  CONNECTIONx 개수
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
	m_enRecipeLoadAck	= E30_HPLA::en_rg_reset;	/* Recipe Load에 대한 요청이 없는 상태 */
	m_enRecipeSendAck	= E30_SRHA::en_ra_reset;	/* 레시피 데이터 송신에 대해 요청이 없는 상태 */
	m_enTermMsgAck		= E30_STHA::en_t_reset;		/* 터미널 메시지 송신에 대해 요청이 없는 상태 */
	m_enRecipeFileAck	= E30_RMJC::en_reset;		/* 레시피 파일 송신에 대해 요청이 없는 상태 */

	/* 해쉬맵 테이블 크기 설정 */
	m_mapCommState.InitHashTable(conn_cnt);
	m_mapCtrlState.InitHashTable(conn_cnt);
	m_mapSpoolState.InitHashTable(conn_cnt);
	/* 메모리 할당 : Host로부터 요청받은 Transaction 요청 정보 임시 저장 */
	m_stTransaction.Init();
	/* 메모리 할당 : Load Port 전체 상태 정보 임시 저장 (Carrier ID 포함) */
	m_pstLoadPortState	= (LPG_CLPI)::Alloc(sizeof(STG_CLPI) * UINT32(MAX_FOUP_COUNT));
	ASSERT(m_pstLoadPortState);
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CSharedData::~CSharedData()
{
	POSITION pPos		= NULL;
	CPRJobID *pPRJobID	= NULL;
	CCRJobID *pCRJobID	= NULL;

	/* 현재 남아 있는 이벤트 모두 제거 */
	RemoveEventAll();
	/* 레시피 목록 (파일) 메모리 반환 */
	RecipeRemoveAll();
	/* 동기화 객체 해제 */
	m_mtxEvent.Close();
	m_mtxRecipe.Close();
	m_mtxPRJobIDs.Close();
	m_mtxCRJobIDs.Close();

	/* 해쉬맵 메모리 해제 */
	m_mapCommState.RemoveAll();
	m_mapCtrlState.RemoveAll();
	m_mapSpoolState.RemoveAll();
	m_lstRecipe.RemoveAll();

	/* Process Job ID 제거 */
	pPos	= m_lstPRJobID.GetHeadPosition();
	while (pPos)
	{
		pPRJobID	= m_lstPRJobID.GetNext(pPos);
		if (pPRJobID)	delete pPRJobID;
	}
	m_lstPRJobID.RemoveAll();

	/* Control Job ID 제거 */
	pPos	= m_lstCRJobID.GetHeadPosition();
	while (pPos)
	{
		pCRJobID	= m_lstCRJobID.GetNext(pPos);
		if (pCRJobID)	delete pCRJobID;
	}
	m_lstCRJobID.RemoveAll();

	/* 메모리 해제 : Host로부터 요청받은 Transaction 요청 정보 임시 저장 */
	m_stTransaction.Close();
	/* 메모리 해제 : Load Port 전체 상태 정보 임시 저장 (Carrier ID 포함) */
	if (m_pstLoadPortState)	::Free(m_pstLoadPortState);

	/* SECS-II Message 메모리 해제 */
	RemoveAllSecsMessage();
}

/*
 desc : 모든 CIM300 Event 제거
 parm : None
 retn : None
*/
VOID CSharedData::RemoveEventAll()
{
	POSITION pPos	= NULL;
	STG_CCED stEvent= {NULL};

	CE40CmdParam *pE40CmdParam	= NULL;

	/* 동기 진입 */
	if (m_mtxEvent.Lock())
	{
		pPos	= m_lstEvent.GetHeadPosition();
		while (pPos)
		{
			stEvent	= m_lstEvent.GetNext(pPos);
			if (stEvent.str_data2)	::Free(stEvent.str_data2);
			if (stEvent.str_data1)	::Free(stEvent.str_data1);

			/* 각 Class Object 메모리 해제 */
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
		/* 이벤트 정보 메모리 해제 */
		m_lstEvent.RemoveAll();
		/* 동기 해제 */
		m_mtxEvent.Unlock();
	}
}

/*
 desc : CIM300Cbk로부터 발생된 이벤트 처리
 parm : exx_id		- [in]  Event Main ID (ENG_ESSC)
		cbf_id		- [in]  각 Main ID의 Sub ID 즉, Callback Function ID
		evt_data	- [in]  Event Data가 저장된 구조체 포인터
		str_data	- [in]  문자열 버퍼가 저장된 버퍼 포인터
		id_data1,2	- [in]  string_data1 / string_data2 구분 값 (ENG_GCPI)
		str_data1,2	- [in]  문자열 버퍼가 저장된 버퍼 포인터
 retn : None
*/
VOID CSharedData::PushEvent(ENG_ESSC exx_id, UINT8 cbf_id, LNG_CCED evt_data)
{
	/* 동기화 진입 */
	if (m_mtxEvent.Lock())
	{
		STG_CCED stEvent= {NULL};
		/* 이벤트 정보 설정 */
		stEvent.exx_id	= UINT16(exx_id);
		stEvent.cbf_id	= cbf_id;
		stEvent.id_data1= UINT8(ENG_GCPI::en_empty_param);
		stEvent.id_data2= UINT8(ENG_GCPI::en_empty_param);
		memcpy(&stEvent.evt_data, evt_data, sizeof(UNG_CCED));
		/* 리스트 버퍼에 등록 */
		m_lstEvent.AddTail(stEvent);

		/* 동기화 해제 */
		m_mtxEvent.Unlock();
	}
}
VOID CSharedData::PushEvent(ENG_ESSC exx_id, UINT8 cbf_id, LNG_CCED evt_data, ENG_GCPI id_data, PTCHAR data)
{
	/* 동기화 진입 */
	if (m_mtxEvent.Lock())
	{
		UINT32 u32Size	= 0;
		STG_CCED stEvent= {NULL};

		/* 이벤트 정보 설정 */
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
		/* 리스트 버퍼에 등록 */
		m_lstEvent.AddTail(stEvent);

		/* 동기화 해제 */
		m_mtxEvent.Unlock();
	}
}
VOID CSharedData::PushEvent(ENG_ESSC exx_id, UINT8 cbf_id, LNG_CCED evt_data,
							ENG_GCPI id_data1, ENG_GCPI id_data2, PTCHAR data1, PTCHAR data2)
{
	/* 동기화 진입 */
	if (m_mtxEvent.Lock())
	{
		UINT32 u32Size	= 0;
		STG_CCED stEvent= {NULL};

		/* 이벤트 정보 설정 */
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
		/* 리스트 버퍼에 등록 */
		m_lstEvent.AddTail(stEvent);

		/* 동기화 해제 */
		m_mtxEvent.Unlock();
	}
}


/*
 desc : CIM300Cbk로부터 등록된 이벤트 반환
 parm : data	- [in]  반환된 이벤트 정보가 저장된 구조체 포인터
 retn : TRUE (반환 데이터 있음) or FALSE (반환 데이터 없음)
*/
BOOL CSharedData::PopEvent(STG_CCED &event)
{
	BOOL bPoped	= FALSE;

	/* 동기화 진입 */
	if (m_mtxEvent.Lock())
	{
		if (m_lstEvent.GetCount())
		{
			/* 가장 오래된 이벤트 항목부터 반환 */
			event	= m_lstEvent.GetHead();
			/* 반환된 데이터 리스트에서 제거  */
			m_lstEvent.RemoveHead();
			/* 반환될 데이터 존재 */
			bPoped	= TRUE;
		}
		/* 동기화 해제 */
		m_mtxEvent.Unlock();
	}

	return bPoped;	/* 반환 성공 여부 */
}

/*
 desc : Event Data가 비어 있는지 여부
 parm : None
 retn : TRUE or FALSE
*/
BOOL CSharedData::IsEmptyEvent()
{
	BOOL bEmpty	= FALSE;

	/* 동기화 진입 */
	if (m_mtxEvent.Lock())
	{
		bEmpty	= m_lstEvent.GetCount() < 1;	/* 비어 있는지 여부 */
		/* 동기화 해제 */
		m_mtxEvent.Unlock();
	}

	return bEmpty;
}

/* --------------------------------------------------------------------------------------------- */
/*                                            Common                                             */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Load Port의 Docked or Undocked Location Name 반환
 parm : port_id	- [in]  Load Port ID (1 or 2)
		docked	- [in]  0x00 : Docked, 0x01 : Undocked
 retn : Docked or Undocked Location Name 반환 (실패일 경우, NULL)
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
 desc : Host에서 요청한 Transaction 정보 값 설정
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
 desc : State Machine 상태 값 반환
 parm : machine	- [in]  발생된 위치 (E30_SSMS)
		conn_id	- [in] 0 or Later (공통 적용을 위해서는 0 값 사용)
 retn : state (음수인 경우, 실패 값이 확률?이 높다)
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
 desc : State Machine 상태 값 갱신
 parm : machine	- [in]  발생된 위치 (E30_SSMS)
		conn_id	- [in] 0 or Later (공통 적용을 위해서는 0 값 사용)
		state	- [in]  발생 값 (E87_CDSM 값에 따라, E87_LPTS ~ E87_LPAM
 retn : None
*/
VOID CSharedData::E30_PushState(E30_SSMS machine, LONG conn_id, LONG state)
{
	UNG_CCED unEvent	= {NULL};

	/* Event 설정 */
	unEvent.e30_sc.machine	= UINT8(machine);
	unEvent.e30_sc.conn_id	= conn_id;
	unEvent.e30_sc.new_state= state;

	switch (machine)
	{
	case E30_SSMS::en_commstate	:	m_mapCommState.SetAt(conn_id, state);	break;
	case E30_SSMS::en_control	:	m_mapCtrlState.SetAt(conn_id, state);	break;
	case E30_SSMS::en_spooling	:	m_mapSpoolState.SetAt(conn_id, state);	break;
	}

	/* Event 영역에 등록 */
	PushEvent(ENG_ESSC::en_e30_gem, (UINT8)E30_CEFI::en_state_change, &unEvent);
}

/*
 desc : Variable Value 등록
 retn : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		var_id	- [in]  Variable ID
		var_name- [in]  Variable Name
 retn : None
*/
VOID CSharedData::E30_ValueChanged(LONG conn_id, LONG var_id, PTCHAR var_name)
{
	UNG_CCED unEvent	= {NULL};

	/* Event 설정 */
	unEvent.e30_vc.conn_id	= conn_id;
	unEvent.e30_vc.var_id	= var_id;

	/* Event 영역에 등록 */
	PushEvent(ENG_ESSC::en_e30_gem, (UINT8)E30_CEFI::en_value_changed1,
			  &unEvent, ENG_GCPI::en_e30_variable_name, var_name);
}

/*
 desc : EC Value 등록
 retn : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		var_name- [in]  Variable Name
 retn : None
*/
VOID CSharedData::E30_ECChanged(LONG conn_id, PTCHAR var_name)
{
	UNG_CCED unEvent	= {NULL};

	/* Event 설정 */
	unEvent.e30_ec.conn_id	= conn_id;
	unEvent.e30_ec.var_id	= 0;

	/* Event 영역에 등록 */
	PushEvent(ENG_ESSC::en_e30_gem, (UINT8)E30_CEFI::en_ec_changed,
			  &unEvent, ENG_GCPI::en_e30_variable_name, var_name);
}

/*
 desc : Alarm 등록
 retn : alarm_id- [in]  Alarm ID (Common이므로, Host ID가 필요 없음)
		alarm_cd- [in]  Alarm Code
 retn : None
*/
VOID CSharedData::E30_AlarmChanged(LONG alarm_id, LONG alarm_cd)
{
	UNG_CCED unEvent	= {NULL};

	/* Event 설정 */
	unEvent.e30_ac.alarm_id	= alarm_id;
	unEvent.e30_ac.alarm_cd	= alarm_cd;
	/* Event 영역에 등록 */
	PushEvent(ENG_ESSC::en_e30_gem, (UINT8)E30_CEFI::en_alarm_change, &unEvent);
}

/*
 desc : Process Program Data 관련 Host 요청 등록
 parm : cbf_id		- [in]  Host로부터 요청된 Callback Function ID
		conn_id		- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
 retn : recipe_name	- [in]  Recipe Name (TEL의 경우, Recipe File Name)
*/
VOID CSharedData::E30_PPData(E30_CEFI cbf_id, LONG conn_id, PTCHAR recipe_name)
{
	UNG_CCED unEvent	= {NULL};

	/* Event 설정 */
	unEvent.e30_pp.conn_id	= conn_id;
	/* Event 영역에 등록 */
	PushEvent(ENG_ESSC::en_e30_gem, (UINT8)E30_CEFI::en_pp_request, &unEvent,
			  ENG_GCPI::en_e30_pp_recipe_name, recipe_name);
}

/*
 desc : Terminal Message 등록
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		term_id	- [in]  Terminal ID, Common의 경우, 무조건 0 값
		mesg	- [in]  Terminal Message가 저장된 객체
 retn : None
*/
VOID CSharedData::TerminalMsgRcvd(LONG conn_id, LONG term_id, PTCHAR mesg)
{
	UNG_CCED unEvent	= {NULL};

	/* Event 설정 */
	unEvent.e30_tm.conn_id	= conn_id;
	unEvent.e30_tm.term_id	= term_id;
	/* Event 영역에 등록 */
	PushEvent(ENG_ESSC::en_e30_gem, (UINT8)E30_CEFI::en_terminal_msg_rcvd,
			  &unEvent, ENG_GCPI::en_e30_terminal_mesg, mesg);
}

/*
 desc : User Defined Message 등록
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		msg_id	- [in]  Message ID (S1 ~ , F1 ~ )
		reply	- [in]  Reply 값 (mNotHandled:0, mReplyLater:1, mReply:2, mNoReply:3)
 retn : None
*/
VOID CSharedData::MessageReceived(LONG conn_id, LONG mesg_id, LONG reply)
{
	UNG_CCED unEvent	= {NULL};

	/* Event 설정 */
	unEvent.e30_mr.conn_id	= conn_id;
	unEvent.e30_mr.mesg_id	= mesg_id;
	unEvent.e30_mr.reply	= reply;
	/* Event 영역에 등록 */
	PushEvent(ENG_ESSC::en_e30_gem, (UINT8)E30_CEFI::en_message_received, &unEvent);
}

/*
 desc : 원격 명령어 (Remote Command) 등록
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		rcmd_id	- [in]  수신된 원격 명령 ID
		rcmd_str- [in]  원격 명령어 (문자열. 혹시 몰라서)
 retn : None
*/
VOID CSharedData::CommandCalled(LONG conn_id, E30_GRCI rcmd_id, PTCHAR rcmd_str)
{
	UNG_CCED unEvent	= {NULL};

	/* Event 설정 */
	unEvent.e30_rc.conn_id	= conn_id;
	unEvent.e30_rc.rcmd_id	= LONG(rcmd_id);
	/* Event 영역에 등록 */
	PushEvent(ENG_ESSC::en_e30_gem, (UINT8)E30_CEFI::en_command_called,
			  &unEvent, ENG_GCPI::en_e30_remote_command, rcmd_str);
}

/* --------------------------------------------------------------------------------------------- */
/*                                        ICxE40PJM Area                                         */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : EC Value 등록
 retn : prj_id	- [in]  Object identifier of Process Job for the command
		state	- [in]  Process Job State (E40_PPJS)
 retn : None
*/
VOID CSharedData::E40_PRStateChange(PTCHAR job_id, LONG state)
{
	UNG_CCED unEvent	= {NULL};
	/* Event 설정 */
	unEvent.e40_ps.state= state;
	/* Event 영역에 등록 */
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

	/* Event 영역에 등록 */
	PushEvent(ENG_ESSC::en_e40_pjm, (UINT8)cbf_id, &unEvent, ENG_GCPI::en_e40_prcommand_callback, job_id);
}
VOID CSharedData::E40_PushCommnad(E40_CEFI cbf_id, E40_PCCC cmd, PTCHAR job_id, PVOID param_cmd)
{
	UNG_CCED unEvent	= {NULL};

	unEvent.e40_pc.cmd		= UINT8(cmd);
	unEvent.e40_pc.param_cmd= param_cmd;

	/* Event 영역에 등록 */
	PushEvent(ENG_ESSC::en_e40_pjm, (UINT8)cbf_id, &unEvent, ENG_GCPI::en_e40_prcommand_callback, job_id);
}

/* --------------------------------------------------------------------------------------------- */
/*                                          ICxE87CMS Area                                       */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : State Machine 상태 값 갱신
 parm : machine	- [in]  발생된 위치 (E87_CDSM)
		state	- [in]  발생 값 (E87_CDSM 값에 따라, E87_LPTS ~ E87_LPAM
		port_id	- [in]  The load port identifier (0 if port ID is not known)
		carr_id	- [in]  The carrier ID (may be an empty string)
 retn : None
*/
VOID CSharedData::E87_PushState(UINT8 machine, LONG state, INT16 port_id, PTCHAR carr_id)
{
	UNG_CCED unEvent	= {NULL};

	/* Event 설정 */
	unEvent.e87_sc.machine	= machine;
	unEvent.e87_sc.port_id	= port_id;
	unEvent.e87_sc.state	= state;
	/* Event 영역에 등록 */
	PushEvent(ENG_ESSC::en_e87_cms, (UINT8)E87_CEFI::en_state_change, &unEvent,
			  ENG_GCPI::en_e87_carrier_id, carr_id);
}

/*
 desc : E87 대다수 콜백 이벤트 값 갱신
 parm : cbf_id	- [in]  Callback Function ID
		state	- [in]  발생 값 (E87_CDSM 값에 따라, E87_LPTS ~ E87_LPAM
		port_id	- [in]  The load port identifier (0 if port ID is not known)
		carr_id	- [in]  The carrier ID (may be an empty string)
		loc_id	- [in]  The ID for the location of the carrier
 retn : None
*/
VOID CSharedData::E87_PushEvent(E87_CEFI cbf_id, LONG state,
								INT16 port_id, PTCHAR carr_id, PTCHAR loc_id)
{
	UNG_CCED unEvent	= {NULL};

	/* Event 설정 */
	unEvent.e87_cf.port_id	= port_id;
	unEvent.e87_cf.state	= state;
	/* Event 영역에 등록 */
	PushEvent(ENG_ESSC::en_e87_cms, (UINT8)cbf_id, &unEvent,
			  ENG_GCPI::en_e87_carrier_id, ENG_GCPI::en_e87_location_id, carr_id, loc_id);
}

/*
 desc : E87 기타 콜백 이벤트 값 갱신
 parm : cbf_id	- [in]  Callback Function ID
		state	- [in]  발생 값 (E87_CDSM 값에 따라, E87_LPTS ~ E87_LPAM
		port_id	- [in]  The load port identifier (0 if port ID is not known)
		type1	- [in]  기타 문자열 1 종류 값 (ENG_GCPI)
		type2	- [in]  기타 문자열 2 종류 값 (ENG_GCPI)
		data1	- [in]  기타 문자열 1
		data2	- [in]  기타 문자열 2
 retn : None
*/
VOID CSharedData::E87_PushEvent(E87_CEFI cbf_id, LONG state, INT16 port_id,
								ENG_GCPI type1, ENG_GCPI type2, PTCHAR data1, PTCHAR data2)
{
	UNG_CCED unEvent	= {NULL};

	/* Event 설정 */
	unEvent.e87_cf.port_id	= port_id;
	unEvent.e87_cf.state	= state;
	/* Event 영역에 등록 */
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

	/* Event 영역에 등록 */
	PushEvent(ENG_ESSC::en_e90_sts, (UINT8)E90_CEFI::en_command_callback, &unEvent,
			  ENG_GCPI::en_e90_substrate_id, subst_id);
}

/*
 desc : E90 대다수 콜백 이벤트 값 갱신
 parm : cbf_id		- [in]  Callback Function ID
		machine		- [in]  발생 값 (E90_VSMC 값에 따라)
		new_state	- [in]  The new state in the state model
							이 값은 machine (SubstrateStateMachineEnum) 값에 따라 달라짐
							switch (machine)
							{
							case sisNO_SM					:	break;
							case sisSUBSTRATE_TRANSPORT_SM	:	SubstrateTransportStateEnum;	break;
							case sisSUBSTRATE_PROCESSING_SM	:	SubstrateProcessingStateEnum;	break;
							case sisSUBSTRATE_READER_SM		:	?
							case sisSUBSTRATE_LOCATION_SM	:	SubstrateLocationStateEnum;		break;
							case sisBATCH_LOCATION_SM		:	break;	// 이 값은 무시
							}
		sub_obj		- [in]  The substrate object
		subst_id	- [in]  The substrate ID
 retn : None
*/
VOID CSharedData::E90_PushEvent(E90_CEFI cbf_id, E90_VSMC machine, LONG new_state, PTCHAR subst_id)
{
	UNG_CCED unEvent	= {NULL};

	/* Event 설정 */
	unEvent.e90_sc.machine	= UINT8(machine);
	unEvent.e90_sc.state	= new_state;
	/* Event 영역에 등록 */
	PushEvent(ENG_ESSC::en_e90_sts, UINT8(cbf_id), &unEvent, ENG_GCPI::en_e90_substrate_id, subst_id);
}

/* --------------------------------------------------------------------------------------------- */
/*                                          ICxE94CJM Area                                       */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : E94 기타 콜백 이벤트 값 갱신
 parm : cbf_id	- [in]  Callback Function ID
		type	- [in]  기타 문자열 1 종류 값 (ENG_GCPI)
		data	- [in]  기타 문자열 (보통 Process Job ID or Control Job ID)
 retn : None
*/
VOID CSharedData::E94_PushEvent(E94_CEFI cbf_id, ENG_GCPI type, PTCHAR data)
{
	UNG_CCED unEvent	= {NULL};

	/* Event 설정 (딱히 없음) */

	/* Event 영역에 등록 */
	PushEvent(ENG_ESSC::en_e94_cjm, UINT8(cbf_id), &unEvent, type, data);
}

/*
 desc : GetNextPrJobID Event
 parm : cbf_id	- [in]  Callback Function ID
		type	- [in]  E94_CPOM 값 (Process Job Order Management)
		job_id	- [in]  Control Job ID 
 retn : None
*/
VOID CSharedData::E94_PushEvent(E94_CEFI cbf_id, E94_CPOM type, PTCHAR job_id)
{
	UNG_CCED unEvent	= {NULL};

	/* Event 설정 (딱히 없음) */
	unEvent.e94_po.state	= LONG(type);
	/* Event 영역에 등록 */
	PushEvent(ENG_ESSC::en_e94_cjm, UINT8(cbf_id), &unEvent, ENG_GCPI::en_e94_process_job_id, job_id);
}

/*
 desc : Control Job State 상태 값 갱신
 parm : state	- [in]  E94CJState (E94_CCJS)
		ctrl_id	- [in]  Control Job ID
 retn : None
*/
VOID CSharedData::E94_PushCJState(E94_CCJS state, PTCHAR ctrl_id)
{
	UNG_CCED unEvent	= {NULL};

	/* Event 설정 */
	unEvent.e94_sc.state	= LONG(state);
	/* Event 영역에 등록 */
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

	/* Event 영역에 등록 */
	PushEvent(ENG_ESSC::en_e94_cjm, UINT8(cbf_id), &unEvent, ENG_GCPI::en_e94_cjcommand_callback, job_id);
}
VOID CSharedData::E94_PushCommnad(E94_CEFI cbf_id, E94_CCMD cmd, PTCHAR job_id)
{
	UNG_CCED unEvent	= {NULL};

	unEvent.e94_pc.cmd		= UINT8(cmd);

	/* Event 영역에 등록 */
	PushEvent(ENG_ESSC::en_e94_cjm, UINT8(cbf_id), &unEvent, ENG_GCPI::en_e94_cjcommand_callback, job_id);
}

/*
 desc : 레시피 (프로세스 프로그램; Process Program == Recipe) 등록
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		file	- [in]  레시피 이름 (전체 경로가 포함된 파일)
 retn : TRUE or FALSE
*/
BOOL CSharedData::RecipeAppend(LONG conn_id, PTCHAR file)
{
	BOOL bSucc	= FALSE;
	STG_CRFD stRecipe;

	/* 동기 진입 */
	if (m_mtxRecipe.Lock())
	{
		/* 등록 가능한 레시피의 개수를 초과 했는지 여부 */
		if (m_lstRecipe.GetCount() < MAX_RECIPE_COUNT)
		{
			stRecipe.conn_id= UINT32(conn_id);
			stRecipe.file	= (PTCHAR)::Alloc(sizeof(TCHAR) * MAX_PATH_LEN);
			wmemset(stRecipe.file, 0x00, MAX_PATH_LEN);
			wcscpy_s(stRecipe.file, MAX_PATH_LEN, file);
			m_lstRecipe.AddTail(stRecipe);
			bSucc	= TRUE;
		}

		/* 동기 해제 */
		m_mtxRecipe.Unlock();
	}

	return bSucc;
}

/*
 desc : 레시피 (프로세스 프로그램; Process Program == Recipe) 삭제
 parm : conn_id	- [in]  연결된 장비 ID, Common의 경우, 무조건 0 값
		file	- [in]  레시피 이름 (전체 경로가 포함된 파일)
 retn : TRUE or FALSE
*/
BOOL CSharedData::RecipeRemove(LONG conn_id, PTCHAR file)
{
	BOOL bSucc		= FALSE;
	POSITION pPos	= NULL, pPrePos;
	STG_CRFD stRecipe;

	/* 동기 진입 */
	if (m_mtxRecipe.Lock())
	{
		/* 전체 개수만큼 반복하여 동일한 레시피 이름이 있는지 검색 */
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

		/* 동기 해제 */
		m_mtxRecipe.Unlock();
	}

	return bSucc;
}

/*
 desc : 모든 레시피 (프로세스 프로그램; Process Program == Recipe) 삭제
 parm : None
 retn : None
*/
VOID CSharedData::RecipeRemoveAll()
{
	POSITION pPos	= NULL;
	STG_CRFD stRecipe;

	/* 동기 진입 */
	if (m_mtxRecipe.Lock())
	{
		pPos	= m_lstRecipe.GetHeadPosition();
		while (pPos)
		{
			stRecipe	= m_lstRecipe.GetNext(pPos);
			if (stRecipe.file)	::Free(stRecipe.file);
		}
		m_lstRecipe.RemoveAll();
		/* 동기 해제 */
		m_mtxRecipe.Unlock();
	}
}

/*
 desc : Host ID와 일치되는 레시피 반환
 parm : conn_id	- [in]  0 값이면, 모든 레시피 반환
		recipe	- [in]  레시피 리스트가 저장될 버퍼
 retn : TRUE or FALSE
*/
BOOL CSharedData::GetRecipeList(LONG conn_id, CStringArray &recipe)
{
	POSITION pPos	= NULL;
	STG_CRFD stRecipe;

	/* 동기 진입 */
	if (m_mtxRecipe.Lock())
	{
		pPos	= m_lstRecipe.GetHeadPosition();
		while (pPos)
		{
			stRecipe	= m_lstRecipe.GetNext(pPos);
			/* 레시피 이름 등록 */
			if (stRecipe.file && ((conn_id == 0x00 || stRecipe.conn_id == conn_id))
			{
				recipe.Add(stRecipe.file);
			}
		}
		/* 동기 해제 */
		m_mtxRecipe.Unlock();
	}

	return recipe.GetCount() > 0;
}

/*
 desc : Process Program (=Recipe) 저장 경로 설정
 parm : path	- [in]  레시피 경로명 (맨 마지막 '\' 제외)
 retn : TRUE or FALSE
*/
BOOL CSharedData::SetRecipePath(PTCHAR path)
{
	UINT32 u32Size	= 0;
	if (!path)	return FALSE;

	/* 경로 길이 */
	u32Size	= (UINT32)wcslen(path);
	if (u32Size < 3 /* ex> d:\ */)	return FALSE;
	/* 맨 마지막 문자에 '\' 포함되어 있는지 */
	if (path[u32Size-1] == L'\\' || path[u32Size-1] == L'/')	return FALSE;

	/* 문자열 복사 */
	swprintf_s(m_tzRecipePath, MAX_FILE_LEN, L"%s", path);

	return TRUE;
}

/*
 desc : Process Program (=Recipe)의 확장자 설정
 parm : ext	- [in]  파일의 확장자
 retn : TRUE or FALSE
*/
BOOL CSharedData::SetRecipeExtension(PTCHAR ext)
{
	if (!ext)	return FALSE;

	/* 경로 길이 */
	swprintf_s(m_tzRecipeExt, MAX_FILE_LEN, L"%s", ext);
	return TRUE;
}

/*
 desc : PRJobID에 해당된 정보 및 Substate ID 정보 등록 혹은 관리
 parm : cmd		- [in]  Command Code
		pj_id	- [in]  Process Job ID
		carr_id	- [in]  Process Job ID에 등록된 Carrier ID 리스트
		slot_no	- [in]  Process Job ID에 등록된 Slot Number 리스트
 retn : None
*/
VOID CSharedData::SetPRJobToSubstID(E40_PCCC cmd, PTCHAR pj_id, CStringArray *carr_id, CByteArray *slot_no)
{
	BOOL bFind			= FALSE;
	INT32 i	= 0;
	POSITION pPos		= NULL;
	CPRJobID *pPRJobID	= NULL;

	/* 동기화 진입 */
	if (m_mtxPRJobIDs.Lock())
	{
		/* 기존 Process Job ID가 있으면 메모리 해제 */
		for (; i<m_lstPRJobID.GetCount() && !bFind; i++)
		{
			/* 객체 Position 얻기 */
			pPos	= m_lstPRJobID.FindIndex(i);
			if (pPos)
			{
				/* Process Job ID 객체 얻기 */
				pPRJobID= (CPRJobID *)m_lstPRJobID.GetAt(pPos);
				/* 비교 후, 동일한 PRJob ID가 존재한다면 */
				if (pPRJobID && (0 == wcscmp(pPRJobID->GetPRJobID(), pj_id)))
				{
					bFind	= TRUE;
				}
			}
		}

		/* 새로 생성인 경우 */
		if (cmd == E40_PCCC::en_create)
		{
			if (bFind)
			{
				delete pPRJobID;
				m_lstPRJobID.RemoveAt(pPos);
			}
			/* 새로운 PRJob ID 정보 등록 */
			pPRJobID	= new CPRJobID(pj_id, cmd);
			ASSERT(pPRJobID);
			if (!pPRJobID->AddSubstState(carr_id, slot_no))	delete pPRJobID;
			else	m_lstPRJobID.AddTail(pPRJobID);
		}
		/* Process Command Code만 변경된 경우 */
		else
		{
			if (bFind)	pPRJobID->UpdateCmdCode(cmd);
		}

		/* 동기화 해제 */
		m_mtxPRJobIDs.Unlock();
	}
}

/*
 desc : Control Job ID 제거
 변수 :	pj_id	- [in]  Process Job ID
 retn : None
*/
BOOL CSharedData::RemoveProcessJobID(PTCHAR pj_id)
{
	BOOL bFind			= FALSE;
	INT32 i	= 0;
	POSITION pPos		= NULL;
	CPRJobID *pPRJobID	= NULL;

	/* 동기화 진입 */
	if (m_mtxPRJobIDs.Lock())
	{
		/* 기존 Process Job ID가 있으면 메모리 해제 */
		for (; i<m_lstPRJobID.GetCount() && !bFind; i++)
		{
			/* 객체 Position 얻기 */
			pPos	= m_lstPRJobID.FindIndex(i);
			if (pPos)
			{
				/* Process Job ID 객체 얻기 */
				pPRJobID= (CPRJobID *)m_lstPRJobID.GetAt(pPos);
				/* 비교 후, 동일한 PRJob ID가 존재한다면 */
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

		/* 동기화 해제 */
		m_mtxPRJobIDs.Unlock();
	}

	return bFind;
}

/*
 desc : E40PJM의 PRJobID 속성 (상세) 정보 제거
 parm : prj_id	- [in]  Process Job ID
 retn : TRUE or FALSE
*/
BOOL CSharedData::RemovePRJobToSubstID(PTCHAR pj_id)
{
	BOOL bDeleted			= FALSE;
	POSITION pPos, pPrevPos	= NULL;
	CPRJobID *pPRJobID		= NULL;

	/* 동기화 진입 */
	if (m_mtxPRJobIDs.Lock())
	{
		pPos	= m_lstPRJobID.GetHeadPosition();
		while (pPos && !bDeleted)
		{
			pPrevPos	= pPos;
			pPRJobID	= m_lstPRJobID.GetNext(pPos);
			/* 삭제 대상의 PRJobID인지 확인 */
			if (pPRJobID && (0 == wcscmp(pPRJobID->GetPRJobID(), pj_id)))
			{
				bDeleted	= TRUE;
				delete pPRJobID;
				/* 검색된 위치 항목 제거 */
				m_lstPRJobID.RemoveAt(pPrevPos);
			}
		}

		/* 동기화 해제 */
		m_mtxPRJobIDs.Unlock();
	}

	return bDeleted;
}

/*
 desc : E40PJM::PRCommandCbk 호출될 때, 사용하기 위해 Substate IDs의 상태 정보 갱신
		pj_id	- [in]  Process Job ID
		machine	- [in]  Substrate ID의 상태 정보를 갱신할 Machine Type 구분 값
		state	- [in]  변경하려는 Substate ID의 상태 값
 retn : TRUE or FALSE
*/
BOOL CSharedData::UpdatePRJobToSubstID(PTCHAR st_id, E90_VSMC machine, INT32 state)
{
	INT32 i32Index			= -1;
	BOOL bIsUpdated			= FALSE;
	POSITION pPos, pPrevPos	= NULL;
	CPRJobID *pPRJobID		= NULL;

	/* 동기화 진입 */
	if (m_mtxPRJobIDs.Lock())
	{
		pPos	= m_lstPRJobID.GetHeadPosition();
		while (pPos && !bIsUpdated)
		{
			pPrevPos	= pPos;
			pPRJobID	= m_lstPRJobID.GetNext(pPos);
			if (pPRJobID)
			{
				/* 동일한 Substate ID가 존재하면, 새로운 상태 값 갱신 */
				i32Index	= pPRJobID->IsFindSlotID(st_id);
				if (i32Index)
				{
					/* Machine Type에 따라 상태 정보 갱신 */
					pPRJobID->UpdateSubstState(i32Index, machine, state);
					bIsUpdated	= TRUE;
				}
			}
		}

		/* 동기화 해제 */
		m_mtxPRJobIDs.Unlock();
	}

	return bIsUpdated;
}

/*
 desc : E40PJM::PRCommandCbk 호출될 때, 사용하기 위해 Substate IDs의 상태 정보 갱신
		pj_id		- [in]  Process Job ID
		auto_start	- [in]  Process Job ID의 동작 방법 값
		pj_state	- [in]  Process Job ID의 상태 값
		lst_state	- [in]  변경하려는 Substate ID의 상태 값이 저장된 리스트 버퍼
 retn : TRUE or FALSE
*/
BOOL CSharedData::UpdatePRJobToSubstID(PTCHAR pj_id, BOOL auto_start, E40_PPJS pj_state,
									   CAtlList <STG_CSDI> *lst_state)
{
	INT32 i32Index		= -1;
	BOOL bIsUpdated		= FALSE;
	POSITION pPos		= NULL;
	CPRJobID *pPRJobID	= NULL;

	/* 동기화 진입 */
	if (m_mtxPRJobIDs.Lock())
	{
		pPos	= m_lstPRJobID.GetHeadPosition();
		while (pPos && !bIsUpdated)
		{
			pPRJobID	= m_lstPRJobID.GetNext(pPos);
			/* 동일한 Substate ID가 존재하면, 새로운 상태 값 갱신 */
			if (0 == wcscmp(pj_id, pPRJobID->GetPRJobID()))
			{
				/* Machine Type에 따라 상태 정보 갱신 */
				pPRJobID->UpdateSubstStateAll(auto_start, pj_state, lst_state);
				bIsUpdated	= TRUE;
			}
		}

		/* 동기화 해제 */
		m_mtxPRJobIDs.Unlock();
	}

	return bIsUpdated;
}

/*
 desc : 현재 등록된 Process Job ID 내의 Substrate IDs의 상태가 모두 입력된 상태와 동일한지 여부
 parm : pj_id	- [in]  검색 대상의 Process Job ID
		machine	- [in]  Substate Machine Type 값
		state	- [in]  비교 대상의 Substrate Processing State 값
 retn : 입력된 state 값과 모두 동일하다면 TRUE, 한 개 이상 다른 값이 존재한다면 FALSE
 */
BOOL CSharedData::IsCheckedAllPRJobSubstState(PTCHAR pj_id, E90_VSMC machine, INT32 state)
{
	BOOL bIsChecked		= TRUE;	/* 모두 동일한 상태 값이라고 설정 */
	POSITION pPos		= NULL;
	CPRJobID *pPRJobID	= NULL;
	
	/* 등록된 개수가 1개라도 없는지 */
	if (m_lstPRJobID.GetCount() < 1)	return FALSE;

	/* 동기화 진입 */
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

		/* 동기화 해제 */
		m_mtxPRJobIDs.Unlock();
	}

	return bIsChecked;
}

/*
 desc : 현재 등록된 Process Job ID 내의 Substrate IDs의 상태가 입력된 상태와 동일한 값이 1개라도 있는지 여부
 parm : pj_id	- [in]  검색 대상의 Process Job ID
		machine	- [in]  Substate Machine Type 값
		state	- [in]  비교 대상의 Substrate Processing State 값
 retn : 입력된 state 값과 동일한 상태 값이 한 개라도 존재하면 TRUE
		입력된 state 값과 동일한 상태 값이 한 개라도 없다면 FALSE
 */
BOOL CSharedData::IsCheckedOnePRJobSubstState(PTCHAR pj_id, E90_VSMC machine, INT32 state)
{
	BOOL bIsChecked		= FALSE;	/* 모두 동일한 상태 값이 없다고 설정 */
	POSITION pPos		= NULL;
	CPRJobID *pPRJobID	= NULL;
	
	/* 등록된 개수가 1개라도 없는지 */
	if (m_lstPRJobID.GetCount() < 1)	return FALSE;

	/* 동기화 진입 */
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

		/* 동기화 해제 */
		m_mtxPRJobIDs.Unlock();
	}

	return bIsChecked;
}

/*
 desc : 현재 장치에 등록된 PRJob IDs에 등록된 Substrate IDs의 모든 상태 정보들 반환
 parm : pj_id	- [in]  검색 대상의 Process Job ID
		subst	- [out] Substrate ID의 상태 및 기타 정보들이 저장될 리스트 메모리 (PRCommandCbk 함수에서 주로 사용됨?)
 retn : TRUE or FALSE
*/
BOOL CSharedData::GetSubstIDStateAll(PTCHAR pj_id, CAtlList <STG_CSDI> &lst_state)
{
	BOOL bIsChecked		= FALSE;
	POSITION pPos		= NULL;
	CPRJobID *pPRJobID	= NULL;

	if (!pj_id)	return FALSE;

	/* 동기화 진입 */
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

		/* 동기화 해제 */
		m_mtxPRJobIDs.Unlock();
	}

	return TRUE;
}

/*
 desc : PRJob ID 내에 등록된 Subsatre ID 개수 반환
 parm : pj_id	- [in]  검색 대상의 Process Job ID
		count	- [out] 개수 값 반환 (Zero-based)
 retn : TRUE or FALSE
*/
BOOL CSharedData::GetCountSubstID(PTCHAR pj_id, UINT32 &count)
{
	BOOL bIsChecked		= FALSE;
	POSITION pPos		= NULL;
	CPRJobID *pPRJobID	= NULL;

	if (!pj_id)	return FALSE;

	/* 동기화 진입 */
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

		/* 동기화 해제 */
		m_mtxPRJobIDs.Unlock();
	}

	return bIsChecked;
}

/*
 desc : PRJob ID 내의 작업 상태 값 반환
 parm : pj_id	- [in]  검색 대상의 Process Job ID
		state	- [out] 작업 상태가 반환될 참조 변수
 retn : TRUE or FALSE
*/
BOOL CSharedData::GetPRJobState(PTCHAR pj_id, E40_PPJS &state)
{
	BOOL bIsChecked		= FALSE;
	POSITION pPos		= NULL;
	CPRJobID *pPRJobID	= NULL;

	if (!pj_id)	return FALSE;

	/* 동기화 진입 */
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

		/* 동기화 해제 */
		m_mtxPRJobIDs.Unlock();
	}

	return bIsChecked;
}

/*
 desc : Process Job IDs에 등록된 모든 Substrate Information 얻기 (상태 및 Carrier ID)
 parm : lst_subst	- [out] 모든 Substrate Information 정보가 저장될 리스트 버퍼 참조 변수
 retn : TRUE or FALSE
*/
BOOL CSharedData::GetSubstrateInfoAll(CAtlList <STG_CSDI> &lst_subst)
{
	BOOL bIsChecked		= FALSE;
	POSITION pPos		= NULL;
	CPRJobID *pPRJobID	= NULL;
	CAtlList <STG_CSDI> lstSubstAll;

	/* 동기화 진입 */
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
				/* 기존 임시 리스트 버퍼 메모리 해제 */
				lstSubstAll.RemoveAll();
			}
		}

		/* 동기화 해제 */
		m_mtxPRJobIDs.Unlock();
	}

	return TRUE;
}

/*
 desc : Process Job IDs에 등록된 임의 Substrate Information 얻기 (상태 및 Carrier ID)
 parm : subst_id	- [in]  검색하고자 하는 Substrate ID
		subst_info	- [out] Substrate Information 정보가 저장될 참조 변수
 retn : TRUE or FALSE
*/
BOOL CSharedData::GetSubstrateInfo(PTCHAR subst_id, STG_CSDI &subst_info)
{
	BOOL bIsChecked		= FALSE;
	POSITION pPos		= NULL;
	CPRJobID *pPRJobID	= NULL;

	/* 동기화 진입 */
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

		/* 동기화 해제 */
		m_mtxPRJobIDs.Unlock();
	}

	return bIsChecked;
}

/*
 desc : PRJobID에 해당된 정보 및 Substate ID 정보 등록 혹은 관리
 parm : state	- [in]  Control Job State
		cj_id	- [in]  Control Job ID
		carr_id	- [in]  Control Job ID에 등록된 Carrier ID 리스트
		pj_id	- [in]  Control Job ID에 등록된 Process Job ID 리스트
 retn : None
*/
VOID CSharedData::SetCRJobToProperty(E94_CCJS state, PTCHAR cj_id,
									 CStringArray *carr_id, CStringArray *pj_id)
{
	BOOL bIsFind		= FALSE;
	INT32 i	= 0;
	POSITION pPos		= NULL;
	CCRJobID *pCRJobID	= NULL;

	/* 동기화 진입 */
	if (m_mtxCRJobIDs.Lock())
	{
		/* 기존 Process Job ID가 있으면 메모리 해제 */
		for (; i<m_lstCRJobID.GetCount() && !bIsFind; i++)
		{
			/* 객체 Position 얻기 */
			pPos	= m_lstCRJobID.FindIndex(i);
			/* Process Job ID 객체 얻기 */
			pCRJobID= (CCRJobID *)m_lstCRJobID.GetAt(pPos);
			/* 비교 후, 동일한 PRJob ID가 존재한다면 */
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

		/* 새로운 PRJob ID 정보 등록 */
		pCRJobID	= new CCRJobID(cj_id);
		ASSERT(pCRJobID);
		pCRJobID->AddProperty(pj_id, carr_id);
		m_lstCRJobID.AddTail(pCRJobID);

		/* 동기화 해제 */
		m_mtxCRJobIDs.Unlock();
	}
}

/*
 desc : Control Job ID 제거
 변수 :	cj_id	- [in]  Control Job ID
 retn : None
*/
BOOL CSharedData::RemoveControlJobID(PTCHAR cj_id)
{
	BOOL bIsFind		= FALSE;
	INT32 i	= 0;
	POSITION pPos		= NULL;
	CCRJobID *pCRJobID	= NULL;

	/* 동기화 진입 */
	if (m_mtxCRJobIDs.Lock())
	{
		/* 기존 Process Job ID가 있으면 메모리 해제 */
		for (; i<m_lstCRJobID.GetCount() && !bIsFind; i++)
		{
			/* 객체 Position 얻기 */
			pPos	= m_lstCRJobID.FindIndex(i);
			/* Process Job ID 객체 얻기 */
			pCRJobID= (CCRJobID *)m_lstCRJobID.GetAt(pPos);
			/* 비교 후, 동일한 PRJob ID가 존재한다면 */
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

		/* 동기화 해제 */
		m_mtxCRJobIDs.Unlock();
	}

	return bFind;
}

/*
 desc : E94CJM::Callback 호출될 때, 사용하기 위한 속성 정보 갱신
 변수 :	cj_id		- [in]  Control Job ID
		auto_start	- [in]  Control Job ID의 AutoStart 동작 여부
		cj_state	- [in]  Control Job ID의 상태 값
		pj_id		- [in]  Process Job ID가 저장된 배열 포인터
		carr_id		- [in]  Control Job ID 내에 Carrier ID가 저장된 배열 포인터
 retn : TRUE or FALSE
*/
BOOL CSharedData::UpdateCRJobToProperty(PTCHAR cj_id, E94_CCJS cj_state, BOOL auto_start,
										CStringArray *pj_id, CStringList *carr_id)
{
	INT32 i32Index		= -1;
	BOOL bIsUpdated		= FALSE;
	POSITION pPos		= NULL;
	CCRJobID *pCRJobID	= NULL;

	/* 동기화 진입 */
	if (m_mtxCRJobIDs.Lock())
	{
		pPos	= m_lstCRJobID.GetHeadPosition();
		while (pPos && !bIsUpdated)
		{
			pCRJobID	= m_lstCRJobID.GetNext(pPos);
			/* 동일한 Substate ID가 존재하면, 새로운 상태 값 갱신 */
			if (0 == wcscmp(cj_id, pCRJobID->GetCRJobID()))
			{
				/* Machine Type에 따라 상태 정보 갱신 */
				pCRJobID->UpdateProperty(cj_state, auto_start, pj_id, carr_id);
				bIsUpdated	= TRUE;
			}
		}

		/* 동기화 해제 */
		m_mtxCRJobIDs.Unlock();
	}

	return bIsUpdated;
}

/*
 desc : 임의 인덱스 (Zero-based) 위치에 있는 Process Job ID or Carrier ID 값 반환
 parm : cj_id	- [in]  검색 대상의 Control Job ID
		method	- [in]  0x00: Process Job ID, 0x01: Carrier ID
		index	- [in]  Zero-based Index 값 (Process Job ID or Carrier ID가 저장된 배열 인덱스)
 retn : Process Job ID or Carrier ID 값 반환 (NULL이면, 더 이상 존재하지 않음)
*/
PTCHAR CSharedData::GetCRJobToSubID(PTCHAR cj_id, UINT8 method, UINT32 index)
{
	INT32 i32Index		= -1;
	BOOL bIsUpdated		= FALSE;
	PTCHAR ptzObjID		= NULL;
	POSITION pPos		= NULL;
	CCRJobID *pCRJobID	= NULL;

	/* 동기화 진입 */
	if (m_mtxCRJobIDs.Lock())
	{
		pPos	= m_lstCRJobID.GetHeadPosition();
		while (pPos && !bIsUpdated)
		{
			pCRJobID	= m_lstCRJobID.GetNext(pPos);
			/* 동일한 Substate ID가 존재하면, 새로운 상태 값 갱신 */
			if (0 == wcscmp(cj_id, pCRJobID->GetCRJobID()))
			{
				if (method)	ptzObjID = pCRJobID->GetCarrID(index);
				else		ptzObjID = pCRJobID->GetPRJobID(index);
				bIsUpdated	= TRUE;
			}
		}
		/* 동기화 해제 */
		m_mtxCRJobIDs.Unlock();
	}

	return ptzObjID;
}

/*
 desc : Control Job ID에 속한 모든 Process Job ID or Carrier ID 반환
 parm : cj_id	- [in]  검색 대상의 Control Job ID
		method	- [in]  0x00: Process Job ID, 0x01: Carrier ID
		obj_ids	- [out] Process Job IDs or Carrier IDs가 반환되어 저장될 배열 리스트 버퍼
 retn : None
*/
VOID CSharedData::GetCRJobToSubIDs(PTCHAR cj_id, UINT8 method, CStringArray &obj_ids)
{
	INT32 i32Index		= -1;
	BOOL bIsUpdated		= FALSE;
	POSITION pPos		= NULL;
	CCRJobID *pCRJobID	= NULL;

	/* 동기화 진입 */
	if (m_mtxCRJobIDs.Lock())
	{
		pPos	= m_lstCRJobID.GetHeadPosition();
		while (pPos && !bIsUpdated)
		{
			pCRJobID	= m_lstCRJobID.GetNext(pPos);
			/* 동일한 Substate ID가 존재하면, 새로운 상태 값 갱신 */
			if (0 == wcscmp(cj_id, pCRJobID->GetCRJobID()))
			{
				/* Control Job ID에 등록된 모든 Carrier IDs 가져오기 */
				if (method)	pCRJobID->GetCarrIDs(obj_ids);
				else		pCRJobID->GetPRJobIDs(obj_ids);
				bIsUpdated	= TRUE;
			}
		}
		/* 동기화 해제 */
		m_mtxCRJobIDs.Unlock();
	}
}

/*
설명 : 모든 Process Job ID 리스트 반환
변수 : 
obj_ids	- [out] Process Job IDs or Carrier IDs가 반환되어 저장될 배열 리스트 버퍼
반환 : None
*/
VOID CSharedData::GetAllPRJobIDs(CStringArray &obj_ids)
{
	POSITION pPos	= NULL;
	CStringArray arrPRJobIDs;

	/* 동기화 진입 */
	if (m_mtxCRJobIDs.Lock())
	{
		pPos = m_lstCRJobID.GetHeadPosition();
		while (pPos)
		{
			CCRJobID *pCRJobID = NULL;
			pCRJobID = m_lstCRJobID.GetNext(pPos);

			/* CRJob ID에 속한 PRJob IDs 얻기 */
			pCRJobID->GetPRJobIDs(arrPRJobIDs);
			if (arrPRJobIDs.GetCount() > 0)
			{
				obj_ids.Append(arrPRJobIDs);
				arrPRJobIDs.RemoveAll();	/* 기존 항목 제거 */
			}
		}
		/* 동기화 해제 */
		m_mtxCRJobIDs.Unlock();
	}
}

/*
 desc : 임의 인덱스 (Zero-based) 위치에 있는 Process Job ID의 동작 상태 값 반환
 parm : cj_id	- [in]  검색 대상의 Control Job ID
		index	- [in]  Zero-based Index 값 (Process Job ID가 저장된 배열 인덱스)
		method	- [out] Process Job ID의 동작 상태 값이 반환될 참조 변수
 retn : TRUE (반환 성공) or FALSE (반환 실패. 검색 실패)
*/
BOOL CSharedData::GetAutoStart(PTCHAR cj_id, UINT32 index, BOOL &method)
{
	INT32 i32Index		= -1;
	BOOL bIsUpdated		= FALSE;
	PTCHAR ptzPRJobID	= NULL;
	POSITION pPos		= NULL;
	CCRJobID *pCRJobID	= NULL;

	/* 동기화 진입 */
	if (m_mtxCRJobIDs.Lock())
	{
		pPos	= m_lstCRJobID.GetHeadPosition();
		while (pPos && !bIsUpdated)
		{
			pCRJobID	= m_lstCRJobID.GetNext(pPos);
			/* 동일한 Substate ID가 존재하면, 새로운 상태 값 갱신 */
			if (0 == wcscmp(cj_id, pCRJobID->GetCRJobID()))
			{
				bIsUpdated	= pCRJobID->GetAutoStart(index, method);
			}
		}
		/* 동기화 해제 */
		m_mtxCRJobIDs.Unlock();
	}

	return bIsUpdated;
}

/*
 desc : Load Port의 동작 State (ENG_LPOS) 값 갱신
 parm : port_no	- [in]  Load Port Number (1-based)
		state	- [in]  Load Port State 값 (ENG_LPOS)
 retn : TRUE or FALSE
*/
BOOL CSharedData::SetLoadPortState(UINT16 port_id, ENG_LPOS state)
{
	if (port_id < 1 || port_id > MAX_FOUP_COUNT)	return FALSE;

	/* State 정보 갱신 */
	m_enLoadPortState[port_id-1]	= state;

	return TRUE;
}

/*
 desc : 현재 Load Port의 동작 State (ENG_LPOS) 값 반환
 parm : port_no	- [in]  Load Port Number (1-based)
 retn : 상태 값
*/
ENG_LPOS CSharedData::GetLoadPortState(UINT16 port_id)
{
	if (port_id < 1 || port_id > MAX_FOUP_COUNT)	return ENG_LPOS::en_unknown;

	return m_enLoadPortState[port_id-1];
}

/*
 desc : Load Port/Carrier ID와 관련된 정보 저장
 parm : port_no	- [in]  Load Port Number (1-based)
		data	- [in]  8가지 정도의 동작 상태 및 Carrier ID 등의 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
BOOL CSharedData::UpdateLoadPortDataAll(UINT16 port_id, LPG_CLPI data)
{
	if (port_id < 1 || port_id > MAX_FOUP_COUNT)	return FALSE;
	/* Load Port Number 기준으로 Load Port 및 Carrier ID 정보 갱신 */
	memcpy(&m_pstLoadPortState[port_id-1], data, sizeof(STG_CLPI));

	return TRUE;
}

/*
 desc : Load Port Number와 관련된 정보 반환
 parm : port_no	- [in]  Load Port Number (1-based)
		data	- [in]  8가지 정도의 동작 상태 및 Carrier ID 등의 정보가 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
BOOL CSharedData::GetLoadPortDataAll(UINT16 port_id, STG_CLPI &data)
{
	if (port_id < 1 || port_id > MAX_FOUP_COUNT)	return FALSE;
	/* Load Port Number 기준으로 Load Port 및 Carrier ID 정보 반환 */
	memcpy(&data, &m_pstLoadPortState[port_id-1], sizeof(STG_CLPI));

	return TRUE;
}

/*
 desc : Carrier ID와 관련된 정보 반환
 parm : carr_id	- [in]  검색 대상의 Carrier ID
		data	- [in]  8가지 정도의 동작 상태 및 Carrier ID 등의 정보가 저장된 구조체 포인터
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
 desc : Carrier ID 제거
 parm : carr_id	- [in]  제거 대상인 Carrier ID
 retn : TRUE (제거 성공), FALSE (제거 실패)
*/
BOOL CSharedData::RemoveSharedCarrID(PTCHAR carr_id)
{
	INT32 i	= 0;
	POSITION pPos	= NULL;
	CPRJobID *pPRJobID;
	CCRJobID *pCRJobID;

	/* ---------------------------------------- */
	/* Carrier ID와 관련된 Process Job IDs 제거 */
	/* ---------------------------------------- */

	/* 동기화 진입 */
	if (m_mtxPRJobIDs.Lock())
	{
		/* 기존 Process Job ID가 있으면 메모리 해제 */
		for (i=0; i<m_lstPRJobID.GetCount(); i++)
		{
			/* 객체 Position 얻기 */
			pPos	= m_lstPRJobID.FindIndex(i);
			/* Process Job ID 객체 얻기 */
			pPRJobID= (CPRJobID *)m_lstPRJobID.GetAt(pPos);
			/* Process Job ID에 속해 있는 Carrier가 존재하면 Process Job ID 제거 */
			if (pPRJobID && pPRJobID->IsFindCarrID(carr_id))
			{
				delete pPRJobID;
				m_lstPRJobID.RemoveAt(pPos);
				/* 다시 처음부터 검색해야 됨 */
				i = 0;	/* !!! important !!! */
			}
		}

		/* 동기화 해제 */
		m_mtxPRJobIDs.Unlock();
	}

	/* ---------------------------------------- */
	/* Carrier ID와 관련된 Control Job IDs 제거 */
	/* ---------------------------------------- */
	/* 동기화 진입 */
	if (m_mtxCRJobIDs.Lock())
	{
		/* 기존 Process Job ID가 있으면 메모리 해제 */
		for (i=0; i<m_lstCRJobID.GetCount(); i++)
		{
			/* 객체 Position 얻기 */
			pPos	= m_lstCRJobID.FindIndex(i);
			/* Process Job ID 객체 얻기 */
			pCRJobID= (CCRJobID *)m_lstCRJobID.GetAt(pPos);
			/* Control Job ID에 속해 있는 Carrier가 존재하면 Contron Job ID 제거 */
			if (pCRJobID && pCRJobID->IsFindCarrID(carr_id))
			{
				delete pCRJobID;
				m_lstCRJobID.RemoveAt(pPos);
				/* 다시 처음부터 검색해야 됨 */
				i = 0;	/* !!! important !!! */
			}
		}

		/* 동기화 해제 */
		m_mtxCRJobIDs.Unlock();
	}

	return TRUE;
}

/*
 desc : SECS-II Message 등록 (덮어쓰기 즉, 기존 등록된 정보가 있으면, 제거 후 새로 등록)
 parm : mesg	- [in]  등록될 SECS-II Message가 저장된 구조체 참조
 retn : TRUE (새로 등록) or FALSE (기존 메시지 삭제 후 등록)
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
		/* 만약 동일한 SECS-II Message인 경우, 기존 메시지 제거 */
		if (stMesg.mesg_id == mesg.mesg_id)
		{
			/* 기존 데이터 제거 */
			if (stMesg.value)	::Free(stMesg.value);
			m_lstSecsMesg.RemoveAt(pPrePos);

			/* 새로 수신된 메시지 등록 */
			m_lstSecsMesg.AddTail(mesg);

			/* 기존 등록된 SECS-II Message 삭제 후 새로 수신된 메시지로 등록 */
			return FALSE;
		}
	}

	/* 새롭게 SECS-II Message 등록 */
	m_lstSecsMesg.AddTail(mesg);

	return TRUE;
}

/*
 desc : 기존 등록된 SECS-II Message 반환
 parm : mesg_id	- [in]  검색 대상의 Message ID 값 (Stream/Function으로 구성된 16진수 Hexa-Decimal 값)
		remove	- [in]  검색된 대상을 반환 후 메모리에서 제거할지 여부
		item	- [out] 검색된 데이터가 반환될 리스트 버퍼
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
		/* 만약 동일한 SECS-II Message인 경우, 기존 메시지 제거 */
		if (stMesg.mesg_id == mesg_id)
		{
			/* 검색된 데이터 반환 */
			for (i=0; i<stMesg.count; i++)
			{
				memcpy(&stVals, &stMesg.value[i], sizeof(STG_TSMV));
				item.AddTail(stVals);
			}

			/* 기존 데이터 제거 */
			if (stMesg.value)	::Free(stMesg.value);
			m_lstSecsMesg.RemoveAt(pPrePos);

			/* 반환 성공 */
			return TRUE;
		}
	}

	/* 반환 실패 */
	return FALSE;
}

/* 
 desc : SECS-II Message 메모리 해제
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

