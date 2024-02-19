#include "pch.h"
#include "MainApp.h"
#include "BSAThread.h"
#include "SendQue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

#define MAX_RECV_WAIT_TIME	500

/*
 desc : ������
 parm : vision	- [in]  Vision Server ��� ����
		shmem	- [in]  Luria Shared Memory
 retn : None
*/
CBSAThread::CBSAThread(LPG_CTCS vision, LPG_RBVC shmem)
	: CClientThread(ENG_EDIC::en_bsa, 0x0000, vision->tcp_port,
					vision->source_ipv4,	/* CMPS Local (Client) IPv4 */
					vision->target_ipv4,	/* PLS Remote (Server) IPv4 */
					vision->port_buff, vision->recv_buff, vision->ring_buff,
					vision->idle_time, vision->link_time, vision->sock_time)
{
	m_bInitUpdate	= FALSE;
	m_u64ReqTime	= GetTickCount64();
	m_u64RecvTime	= GetTickCount64();
	m_u64SendTime	= GetTickCount64();
	m_u64PeriodTime	= 0;
	m_bNotiEvent	= FALSE;
	/* ���� �޸� ���� */
	m_pstShMemVisi	= shmem;

	/* ���� ���� ������ �ӽ� ���� */
	m_pPktRecvData = (PUINT8)::Alloc(1024);
	memset(m_pPktRecvData, 0x00, 1024);
	/* ��Ŷ �۽ſ� ���� ���� ���� ���� �÷��� */
	m_pPktRecvFlag	= (PUINT8)::Alloc(32);
	memset(m_pPktRecvFlag, 0x00, 32);
	/* Event Message ���� �Ҵ� */
	m_pEventMsg	= (PTCHAR)::Alloc(sizeof(TCHAR) * 1024+1);
	wmemset(m_pEventMsg, 0x00, 1024+1);

	/* �����(���)�� ����� �ӽ÷� ������ �۽� ������ ��ü ���� */
	m_pSendQue = new CSendQue(256);
	ASSERT(m_pSendQue);
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CBSAThread::~CBSAThread()
{
	/* Trigger Server�� ���� ���� ���� */
	CloseClient();
	/* Event Message ���� ���� */
	::Free(m_pEventMsg);
	/* ��Ŷ �۽ſ� ���� ���� �÷��� ���� */
	::Free(m_pPktRecvFlag);
	/* �۽� ť �޸� ���� */
	delete m_pSendQue;
	::Free(m_pPktRecvData);
}

/*
 desc : ������ ����� �ֱ��� ȣ���
 parm : None
 retn : None
*/
VOID CBSAThread::DoWork()
{
	/* �ֱ������� �۽� Queue ���� */
	if (!m_pSendQue->IsQueEmpty())	PopSendData();

	/* Vision State�� Ư�� �ð����� ���ŵ��� ������ ������ ���� ���� */
#ifdef _DEBUG
	if(!IsUpdateStateTime(10000))
#else
	if (!IsUpdateStateTime(5000))
#endif
	{
		/* ���� �ֱ� ���ŵ� �۾� �ð� ���� */
		m_u64RecvTime = GetTickCount64();
		/* ������ ���� ���� ���� */
		CloseClient();

		LOG_WARN(ENG_EDIC::en_bsa, L"5 sec no response close socket");
		return;
	}

	/* ���ŵ� �����Ͱ� �����ϴ��� �ֱ������� Ȯ�� */
	LPG_PSRB pstPktData	= m_pRecvQue->PopPktData();
	if (pstPktData)	PktAnalysis(pstPktData);
}

/*
 desc : ������ �����̵� ���
 parm : sd	- ���� �����
 retn : TRUE or FALSE
*/
BOOL CBSAThread::Connected()
{
	/* ����Ǿ��� ��, �� �ð� ��� ���� */
	m_u64SendTime	= GetTickCount64();
	m_u64RecvTime	= GetTickCount64();
	m_u64ReqTime	= GetTickCount64();

	/* State Time ���� */
	SetUpdateStateTime();

	return TRUE;
}

/*
 desc : �����κ��� ���� ������ �� ���
 parm : None
 retn : Nonek
*/
VOID CBSAThread::CloseSocket(SOCKET sd, UINT32 time_out/* =0 */)
{
	// ���� ��ü ȣ��
	CClientThread::CloseSocket(sd, time_out);
}

/*
 desc : ���ŵ� ��Ŷ ��ɾ ���� ó�� ����
 parm : data	- [in]	���ŵ� �����Ͱ� ����� ����ü ������
 retn : TRUE - ������ ���� ����, FALSE - ������ ���� ����
*/
BOOL CBSAThread::PktAnalysis(LPG_PSRB data)
{
	/* Alive Check Event�� ������� �ʴ´�. �ӵ� ���� ���� ������ */
	if ((UINT16)ENG_PCUC::en_comn_alive_check != data->cmd)
	{
		/* ���� ó���� ���� �ݹ� �Լ� ȣ�� */
		SetSockData(0x02, data);
	}

	return TRUE;
}

/*
 desc : �����Ͱ� ���ŵ� ���, ȣ���
 parm : None
 retn : TRUE - ���������� �м� (Ring Buffer) ó��
		FALSE- �м� ó������ ���� (���� �Ϻ����� ���� ��Ŷ��)
*/
BOOL CBSAThread::RecvData()
{
	TCHAR tzMesg[64]	= { NULL };
	PUINT8 pPktRead/*, pPktData*/;
	UINT32 u32HeadTail	= sizeof(STG_UTPH) + sizeof(STG_UTPT);
	UINT64 u64SavedBytes;
	STG_UTPH stPktHead	= { NULL };
	STG_UTPT stPktTail	= { NULL };

	// �⺻������ ���ŵǾ��ٰ� ����
	m_enSocket = ENG_TPCS::en_recved;
	do {
		/* ------------------------------------------------------------------------------- */
		/* ���� ���ŵ� ������ ũ�⸦ ���� �� ��Ŷ ��� ũ��(8 Bytes) ���� ���� ���� ��� */
		/* ������ �� �о�� ������ �� �̻� �۾��� �������� �ʵ��� ó�� �Ѵ�                */
		/* ------------------------------------------------------------------------------- */
		u64SavedBytes = m_pPktRingBuff->GetReadSize();
		if (u64SavedBytes < u32HeadTail)
		{
			m_enSocket = ENG_TPCS::en_recving;
			return TRUE;
		}
		// ��� ���� ����
		m_pPktRingBuff->CopyBinary((UINT8*)&stPktHead, sizeof(STG_UTPH));
		stPktHead.SwapNetworkToHost();
		// �߸��� ��Ŷ�� ��� ��ó ó��
		if (stPktHead.length < 1)
		{
			swprintf_s(tzMesg, 64, L"head:%02x / cmd:%02x / length:%d",
					   stPktHead.head, stPktHead.cmd, stPktHead.length);
			LOG_ERROR(ENG_EDIC::en_bsa, tzMesg);
			return FALSE;
		}
		/* --------------------------------------------------------------------------------- */
		/* ���� ���ŵ� ������ ũ�⸦ ���� �� ��Ŷ ���� ũ��(Tail ����) ���� ���� ���� ��� */
		/* ������ �� �о�� ������ �� �̻� �۾��� �������� �ʵ��� ó�� �Ѵ�                  */
		/* --------------------------------------------------------------------------------- */
		if (u64SavedBytes < stPktHead.length)
		{
			m_enSocket = ENG_TPCS::en_recving;
			return TRUE;
		}
		//////////////////////////////////////////////////////////////////////////
		/* ----------------------------------------------------------------------- */
		/* ���ŵ� ��Ŷ ������ 1���� ������ ���� �Ҵ�                               */
		/* ----------------------------------------------------------------------- */
		m_pPktRecvData[stPktHead.length] = 0x00;
		pPktRead = m_pPktRecvData;

		/* ----------------------------------------------------------------------- */
		/* ���ۿ� �ϳ� �̻��� ��Ŷ�� ����Ǿ� �ִٸ� ������ �����͸� �о�� �Ѵ� */
		/* ----------------------------------------------------------------------- */
		m_pPktRingBuff->ReadBinary(pPktRead, stPktHead.length);
		// ���� �� ���۵Ǵ� ��ġ�� ���� ����Ʈ �̵�
		pPktRead += sizeof(STG_UTPH);
		/* ----------------------------------------------------------------------- */
		/* ���� ���ŵ� ��Ŷ �����͸� ���� ť ���ۿ� ��� �۾��� �����ϵ��� ó�� �� */
		/* ----------------------------------------------------------------------- */
		if (!m_pRecvQue->PushPktData(stPktHead.cmd, stPktHead.length - u32HeadTail, pPktRead))
		{
#ifdef _DEBUG
			TRACE(L"Recv Queue is Full\n");
#endif
		}
		/* ------------------------------------------------------------------------------- */
		/* ���� ��Ŷ �ϳ��� �а� �� ����, ���ۿ� ���� �ִ� �������� ũ�Ⱑ ��Ŷ�� �����ϴ� */
		/* �ּ��� ��Ŷ ũ�⸸ŭ ���� ������, ������ �ѹ� �� �����ϰ�, �ּ� ��Ŷ ũ�⸸ŭ   */
		/* ���� ���� ������ �� �о������ �ش� ������ ������������ ó�� �Ѵ�               */
		/* ------------------------------------------------------------------------------- */
	} while (!m_pRecvQue->IsQueFull() && (u64SavedBytes - stPktHead.length) >= u32HeadTail);

	/* !!! Important !!! - ���ŵǸ�, �۽Žð� ������ �ʱ�ȭ (0 ��) */
	m_u64SendTime = 0;

	return TRUE;
}

/*
 desc : �۽��ص� �Ǵ��� Ȯ�� (������ �۽ſ� ���� ������ ���� �ߴ��� Ȯ��)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CBSAThread::IsSendData()
{
	/* �۽� �� Ư�� �ð� ���� ������ ������ Ȯ�� */
	return (GetTickCount64() - m_u64SendTime) > MAX_RECV_WAIT_TIME ? TRUE : FALSE;
}

/*
 desc : ���� �۽ŵ� ��Ŷ�� �����ϴ��� ���� Ȯ��
 parm : None
 retn : FALSE - POP �� �����Ͱ� �ִ�. TRUE - POP �� �����Ͱ� ����
*/
BOOL CBSAThread::IsPopEmptyData()
{
	// Queue�� ��� �ִٸ� TRUE�̹Ƿ�, POP �� �� ����.
	return m_pSendQue->IsQueEmpty();
}

/*
 desc : �۽��� �����Ͱ� ������, Queue�κ��� ������ �ϳ� ������ �� �۽� �۾� ����
 parm : None
 retn : TRUE or FALSE (������ ��Ŷ ������ ���� ���)
*/
BOOL CBSAThread::PopSendData()
{
	BOOL bSucc = FALSE;
	LPG_PBSR pstQue = NULL;
	STG_UTPH stHead = { NULL };
	STG_PSRB stData = { NULL };

	/* �۽ſ� ���� ������ ���� ���, �� �̻� �۽� ���� ó�� */
	if (!IsSendData())
	{
		LOG_ERROR(ENG_EDIC::en_bsa, L"There is no response to a previously sent command");
		return FALSE;
	}

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		/* ---------------------------------- */
		/* ť�� ���� �����Ͱ� �����ϴ��� Ȯ�� */
		/* ---------------------------------- */
		pstQue = m_pSendQue->PopData();
		if (pstQue)
		{
			// ���� �۽��� ������ �ð� ����
			m_u64SendTime = GetTickCount64();

			/* �޽��� ��� ���� */
			memcpy(&stHead, pstQue->data, sizeof(STG_UTPH));
			stHead.SwapNetworkToHost();
			stData.cmd = stHead.cmd;
			stData.len = stHead.length - sizeof(STG_UTPH) - sizeof(STG_UTPT);
			stData.data = pstQue->data + sizeof(STG_UTPH);

			/* �۽� �޽��� �˸� */
			SetSockData(0x01, &stData);

			/* ������ ���� */
			bSucc = ReSended(pstQue->data, UINT32(pstQue->size), UINT32(3), UINT32(30));
			/* ���� ������ �۽��� ���� �ߴٸ� */
			if (!bSucc)
			{
				/* �۽� FAIL �޽��� �˸� */
				SetSockData(0xf1, &stData);
			}
		}
		/* ���� ���� */
		m_syncSend.Leave();
	}

	/* �۽��� ��Ŷ ������ �ִ��� ����(TRUE-�۽ŵ� ��Ŷ�� �ִ�? FALSE-�۽ŵ� ��Ŷ�� ����) */
	return bSucc;
}

//////////////////////////////////////////////////////////////////////////
/*
 desc : ������ �۽�
 parm : cmd		- [in]  ��ɾ�
		data	- [in]  �۽� �����Ͱ� ����� ���� ������
 retn : TRUE or FALSE
*/
BOOL CBSAThread::SendData(ENG_VCPC cmd, PUINT8 data, UINT16 size)
{
	BOOL bSucc = FALSE;

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		bSucc = m_pSendQue->PushData(UINT16(cmd), data, size);

		/* ���� ���� */
		m_syncSend.Leave();
	}

	return bSucc;
}

/*
 desc : Vision Centering Command Data Send
 parm :	cmd		- [in] command
		cam_no	- [in] ī�޶� ���� (0x01: bsa , 0x02: up)
		unit	- [in] ���� ��ġ����(0x01: quaz, 0x02: mark1, 0x03: mark2)
 retn : TRUE or FALSE
*/
BOOL CBSAThread::SendCenteringData(ENG_VCPC cmd, UINT8 cam_no, UINT8 unit)
{
	UINT8 u8Pkt[2]	= { cam_no, unit };
	return SendData(cmd, u8Pkt, 2);
}

/*
 desc : Vision Centering Command Data Send
 parm :	cmd		- [in] command
		cam_no	- [in] ī�޶� ���� (0x01: bsa , 0x02: up)
		unit	- [in] ���� ��ġ����(0x01: quaz, 0x02: mark1, 0x03: mark2)
 retn : TRUE or FALSE
*/
BOOL CBSAThread::SendFocusData(ENG_VCPC cmd, UINT8 cam_no, UINT8 unit)
{
	UINT8 u8Pkt[2]	= { cam_no, unit };
	return SendData(cmd, u8Pkt, 2);
}

/*
 desc : Vision Focus Command Data Send
 parm :	cmd		- [in] command
		point	- [in] ���� ���� (0x01: ����1, 0x02: ����2)
		mark	- [in] Mark ��ȣ (0x01: mark1, 0x02: mark2)
		pattern	- [in] pattern ��ȣ 
		pos_x	- [in] ���� Stage X�� ��ġ
		pos_y	- [in] ���� Stage Y�� ��ġ
 retn : TRUE or FALSE
*/
BOOL CBSAThread::SendGrabData(ENG_VCPC cmd, UINT8 point, UINT8 mark, UINT8 pattern,
							  INT32 pos_x, INT32 pos_y)
{
	UINT8 u8Cmd[3]	= { point, mark, pattern }, u8Pkt[16] = {NULL};
	UINT32 u32Cmd[2]= { SWAP32(UINT32(pos_x)), SWAP32(UINT32(pos_y)) };

	memcpy(u8Pkt, u8Cmd, 3);
	memcpy(u8Pkt+3, u32Cmd, 8);

	return SendData(cmd, u8Pkt, 11);
}

/*
 desc : Vision Centering Command Data Send
 parm :	cmd		- [in] command
		mode	- [in] ?
 retn : TRUE or FALSE
*/
BOOL CBSAThread::SendViewData(ENG_VCPC cmd, UINT8 mode)
{
	return SendData(cmd, (PUINT8)&mode, 1);
}

/*
 desc : Vision Centering Command Data Send
 parm :	cmd		- [in] command
		mark	- [in] Mark ��ȣ (0x01: mark1, 0x02: mark2)
		unit	- [in] ���� ��ġ����(0x01: quaz, 0x02: mark1, 0x03: mark2)
 retn : TRUE or FALSE
*/
BOOL CBSAThread::SendIllumination(ENG_VCPC cmd, UINT8 mark, UINT8 onoff)
{
	UINT8 u8Pkt[2]	= { mark, onoff };
	return SendData(cmd, u8Pkt, 2);
}

/*
 desc : Vision Joblist Command Data Send
 parm :	cmd			- [in] command
 retn : TRUE or FALSE
*/
BOOL CBSAThread::SendJoblist(ENG_VCPC cmd, UINT64 mark_1, UINT64 mark_2)
{
	UINT32 i, u32Pkt[4]	= { NULL };
	u32Pkt[0]	= (mark_1 & 0xffffffff00000000) >> 32;
	u32Pkt[1]	= (mark_1 & 0x00000000ffffffff);
	u32Pkt[2]	= (mark_2 & 0xffffffff00000000) >> 32;
	u32Pkt[3]	= (mark_2 & 0x00000000ffffffff);
	for (i=0; i<4; i++)	u32Pkt[i]	= SWAP32(u32Pkt[i]);

 	return SendData(cmd, (PUINT8)u32Pkt, sizeof(UINT32));
}

/*
 desc : �ݹ� �Լ� ó��
 parm : flag	- [in]  send_succ:0x01, recv_succ:0x02, send_fail_0xf1, recv_fail:0xf2
		data	- [in]  �۽� or ���ŵ� �����Ͱ� ����� ����ü ������
 retn : None
*/
VOID CBSAThread::SetSockData(UINT8 flag, LPG_PSRB data)
{
	switch (flag)
	{
	case 0x01 : SetSendData(data);	break;
	case 0x02 : SetRecvData(data);	break;
	case 0xf1 :
	case 0xf2 :
		{
			TCHAR tzErr[2][16]	= { L"send", L"recv" };
			TCHAR tzMesg[128]	= { NULL };
			swprintf_s(tzMesg, 128, L"Failed to %s the packet (cmd:%02x) for vision_tcp/ip",
					   tzErr[flag-0xf1], data->cmd);
			LOG_ERROR(ENG_EDIC::en_bsa, tzMesg);
		}
		break;
	}
}

/*
 desc : ���ŵ� �����Ͱ� �ִ��� ���� ��ȯ
 parm : cmd	- [in]  ��ɾ�
 retn : TRUE or FALS
*/
BOOL CBSAThread::IsRecvData(ENG_VCPC cmd)
{
	UINT8 u8Cmd	= 0x00;

	/* ��Ŷ �۽ſ� ���� �÷��� ���� */
	u8Cmd	= (UINT16(cmd) & 0x00ff);
	if (u8Cmd > 32)
	{
		TCHAR tzMesg[64]	= {NULL};
		swprintf_s(tzMesg, 64, L"Invalid command call (cmd : %02x)", UINT8(cmd));
		LOG_WARN(ENG_EDIC::en_bsa, tzMesg);
		return FALSE;
	}

	return m_pPktRecvFlag[u8Cmd] == 0x02;
}

/*
 desc : �۽� ������ ó��
 parm : data	- [in]  ���ŵ� �����Ͱ� ����� ����ü ������
 retn : None
*/
VOID CBSAThread::SetSendData(LPG_PSRB data)
{
	UINT8 u8Cmd	= 0x00;
	TCHAR tzMesg[1024]	= { NULL };
	TCHAR tzData[512]	= { NULL };
	STG_SBVC stPktSend	= { NULL };

	/* ��Ŷ �۽ſ� ���� �÷��� ���� */
	u8Cmd	= (data->cmd & 0x00ff);
	if (u8Cmd < 32)	m_pPktRecvFlag[u8Cmd]	= 0x01;	/* Sended */

	switch (data->cmd)
	{
	case ENG_VCPC::en_vision_req_alive			:
	case ENG_VCPC::en_vision_req_state			:
	case ENG_VCPC::en_vision_req_error			:
	case ENG_VCPC::en_vision_req_reset			:
	case ENG_VCPC::en_vision_req_stop			:
	case ENG_VCPC::en_vision_req_cam_offset		:
	case ENG_VCPC::en_vision_req_marklist		:
	case ENG_VCPC::en_vision_req_job_pat_list	:
	case ENG_VCPC::en_vision_req_align_result	:	swprintf_s(tzData, 512, L"null");		break;
	case ENG_VCPC::en_vision_req_centering		:
		memcpy(&stPktSend.centering, data->data, sizeof(STG_SVCV));
		swprintf_s(tzData, 512, L"cam:%x, unit:%x",
				   stPktSend.centering.cam_num, stPktSend.centering.unit);					break;
	case ENG_VCPC::en_vision_req_focus			:
		memcpy(&stPktSend.focus, data->data, sizeof(STG_SVFV));
		swprintf_s(tzData, 512, L"cam:%x, unit:%x",
				   stPktSend.focus.cam_num, stPktSend.focus.unit);							break;
	case ENG_VCPC::en_vision_req_align_grab		:
		memcpy(&stPktSend.align, data->data, sizeof(STG_SAVV));
		stPktSend.align.SwapData();
		swprintf_s(tzData, 512, L"pt:%x, Mark:%x, Pat:%x, pos_x:%d, pos_y:%d",
				   stPktSend.align.point_num, stPktSend.align.mark_num, stPktSend.align.pattern_num,
				   stPktSend.align.pos_x, stPktSend.align.pos_y);							break;
	case ENG_VCPC::en_vision_req_illumination	:
		memcpy(&stPktSend.illum, data->data, sizeof(STG_SIMO));
		swprintf_s(tzData, 512, L"mark:%x, onoff:%d",
				   stPktSend.illum.mark, stPktSend.illum.onoff);							break;
	case ENG_VCPC::en_vision_req_view			:
		memcpy(&stPktSend.view, data->data, sizeof(STG_SVVM));
		swprintf_s(tzData, 512, L"unit : %x", stPktSend.view.mode);							break;
	}

	/* Cmd + Data */
	swprintf_s(tzMesg, 1024, L"SEND :[cmd=%x],[%s]", data->cmd, tzData);
	SetNotiMsg(tzMesg);
}

/*
 desc : ���Ź��� ������ ó��
 parm : data	- [in]  ���ŵ� �����Ͱ� ����� ����ü ������
 retn : None
*/
VOID CBSAThread::SetRecvData(LPG_PSRB data)
{
	UINT8 u8Cmd	= 0x00;
	PUINT8 pData= data->data;

	/* ��Ŷ ���ſ� ���� �÷��� ���� */
	if (data->cmd != (UINT16)ENG_VCPC::en_vision_res_state)
	{
		u8Cmd	= (data->cmd & 0x00ff);
		if (u8Cmd < 32)	m_pPktRecvFlag[u8Cmd]	= 0x02;	/* Received */
	}

	switch (data->cmd)
	{
	case ENG_VCPC::en_vision_res_alive			:
	case ENG_VCPC::en_vision_res_stop			:
	case ENG_VCPC::en_vision_res_reset			:
	case ENG_VCPC::en_vision_res_illumination	:	break;;

	case ENG_VCPC::en_vision_res_state			:	m_pstShMemVisi->vision_state	= pData[0];
													m_pstShMemVisi->update_state_time	= GetTickCount64();			break;
	case ENG_VCPC::en_vision_res_error			:	memcpy(&m_pstShMemVisi->vision_error, pData, sizeof(STG_RVEI));	break;
	case ENG_VCPC::en_vision_res_centering		:	memcpy(&m_pstShMemVisi->centering, pData, sizeof(STG_RVCV));
													m_pstShMemVisi->centering.SwapData();							break;
	case ENG_VCPC::en_vision_res_cam_offset		:	memcpy(&m_pstShMemVisi->cam_offset, pData, sizeof(STG_RVCO));
													m_pstShMemVisi->cam_offset.SwapData();							break;

	case ENG_VCPC::en_vision_res_focus			:	memcpy(&m_pstShMemVisi->focus, pData, sizeof(STG_RVFV));
													m_pstShMemVisi->focus.SwapData();								break;
	case ENG_VCPC::en_vision_res_marklist		:	memcpy(&m_pstShMemVisi->pattern, pData, sizeof(STG_RVPM));
													m_pstShMemVisi->pattern.SwapData();								break;
	case ENG_VCPC::en_vision_res_align_grab		:	SetAlignGrab(pData);										break;
	case ENG_VCPC::en_vision_res_align_result	:	memcpy(&m_pstShMemVisi->align_info.align_result, pData, sizeof(STG_RVAR));
													m_pstShMemVisi->align_info.align_result.SwapData();				break;
	case ENG_VCPC::en_vision_res_job_pat_list	:	m_pstShMemVisi->job_list.result	= pData[0];						break;
	
	case ENG_VCPC::en_vision_res_view			:	m_pstShMemVisi->view_info.result	= pData[0];					break;
	}

	/* NotiEvnet Msg ���� */
	SetNotiData(data);
}

/*
 desc : Align�� Mark Grab ��/�� ����
 parm : data	- [in]  ���ŵ� �����Ͱ� ����� ����ü ������
 retn : None
*/
VOID CBSAThread::SetAlignGrab(PUINT8 data)
{
	/* Mark��ȣ(data[1]), Grab������(data[2]) ������ ��ȿ�� �˻� */
	if (data[1] < 0 || data[2] < 0)			return;
	/* Align Mark �� Grab ������ ���� */
	switch (data[0])
	{
	case 0x01 : m_pstShMemVisi->align_info.mark_grab.mark_1_grab = data[2];	break;	/* Mark 1 */
	case 0x02 : m_pstShMemVisi->align_info.mark_grab.mark_2_grab = data[2];	break;	/* Mark 2 */
	}
}

/*
 desc : NotiEvnet Msg ����
 parm : mesg	- [in]  Noti Mesage
 retn : None
*/
VOID CBSAThread::SetNotiMsg(PTCHAR mesg)
{
	TCHAR tzMesg[1024]	= {NULL};

 	m_bNotiEvent= TRUE;
	CTime cTime	= CTime::GetCurrentTime();	/* ���� �ý������κ��� ��¥ �� �ð��� ��� �´�. */
	swprintf_s(m_pEventMsg, 1024, L"(%02d:%02d:%02d) %s",
			   cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond(), mesg);

	/* �α� ���Ͽ� ���� */
	swprintf_s(tzMesg, 1024, L"Vision set log data <%s>", mesg);
	LOG_MESG(ENG_EDIC::en_bsa, tzMesg);
}

/*
 desc : View Control �� ���� ��� ����
 parm : data	- [in]  ���ŵ� �����Ͱ� ����� ����ü ������
 retn : None
*/
VOID CBSAThread::SetNotiData(LPG_PSRB data)
{
	TCHAR tzMesg[1024]		= { NULL };
	TCHAR tzData[512]		= { NULL };
	TCHAR tzstatus[4][32]	= { L"READY", L"BUSY", L"MANUAL", L"ERROR" };
	TCHAR tzunit[3][32]		= { L"NONE", L"BSA", L"UP" };
	TCHAR tzMark[3][32]		= { L"NONE", L"MARK1", L"MARK2" };
	TCHAR tzgrab[2][32]		= { L"GRAB", L"FAIL" };

	switch (data->cmd)
	{
	case ENG_VCPC::en_vision_res_alive			:
	case ENG_VCPC::en_vision_res_stop			:
	case ENG_VCPC::en_vision_res_reset			:	
	case ENG_VCPC::en_vision_res_illumination	:	swprintf_s(tzData, 512, L"null");				break;
	case ENG_VCPC::en_vision_res_state			:
		swprintf_s(tzData, 512, L"status:%s", tzstatus[m_pstShMemVisi->vision_state]);				break;
	case ENG_VCPC::en_vision_res_error			:
		swprintf_s(tzData, 512, L"unit:%x, error:%x", m_pstShMemVisi->vision_error.unit,
				   m_pstShMemVisi->vision_error.error);												break;
	case ENG_VCPC::en_vision_res_centering		:
		if (m_pstShMemVisi->centering.common.unit > 3 || m_pstShMemVisi->centering.common.unit < 0)	return;
		swprintf_s(tzData, 512, L"cam:%s, result:%x, dx:%d, dy:%d",
				   tzunit[m_pstShMemVisi->centering.common.unit], m_pstShMemVisi->centering.common.result,
				   m_pstShMemVisi->centering.move_x, m_pstShMemVisi->centering.move_y);				break;
	case ENG_VCPC::en_vision_res_cam_offset		:
		swprintf_s(tzData, 512, L"result:%x, ofx:%d, ofy:%d", m_pstShMemVisi->cam_offset.result,
				   m_pstShMemVisi->cam_offset.offset_x, m_pstShMemVisi->cam_offset.offset_y);		break;
	case ENG_VCPC::en_vision_res_focus			:
		if (m_pstShMemVisi->focus.common.unit > 3 || m_pstShMemVisi->focus.common.unit < 0)			return;
		swprintf_s(tzData, 512, L"cam:%s, result:%x, data:%d, dz:%d",
				   tzunit[m_pstShMemVisi->focus.common.unit], m_pstShMemVisi->focus.common.result,
				   m_pstShMemVisi->focus.focus_data, m_pstShMemVisi->focus.move_z);					break;
	case ENG_VCPC::en_vision_res_marklist		:	swprintf_s(tzData, 512, L"MarkList");			break;
	case ENG_VCPC::en_vision_res_align_grab		:
		if (data->data[1] < 0 || data->data[1] > 4)	 return;
		if (data->data[2] < 0 || data->data[2] > 4)	 return;
		swprintf_s(tzData, 512, L"Mark:%s, result:%s",
				   tzMark[data->data[1]], tzgrab[data->data[2]]);									break;
	case ENG_VCPC::en_vision_res_align_result	:
		swprintf_s(tzData, 512, L"result:%x, m1x:%d, m1y:%d, m2x:%d, m2y:%d, sfx:%d, sfy:%d, sfa:%d",
			m_pstShMemVisi->align_info.align_result.result,
			m_pstShMemVisi->align_info.align_result.mark_1_pos_x,
			m_pstShMemVisi->align_info.align_result.mark_1_pos_y,
			m_pstShMemVisi->align_info.align_result.mark_2_pos_x,
			m_pstShMemVisi->align_info.align_result.mark_2_pos_y,
			m_pstShMemVisi->align_info.align_result.shfit_x,
			m_pstShMemVisi->align_info.align_result.shfit_y,
			m_pstShMemVisi->align_info.align_result.shfit_angle);									break;
	case ENG_VCPC::en_vision_res_job_pat_list	:
		swprintf_s(tzData, 512, L"result:%x", m_pstShMemVisi->job_list.result);						break;
	case ENG_VCPC::en_vision_res_view			:
		swprintf_s(tzData, 512, L"result:%x", m_pstShMemVisi->view_info.result);					break;
	}

	swprintf_s(tzMesg, 1024, L"RECV :[cmd=%x],[%s]", data->cmd, tzData);
	SetNotiMsg(tzMesg);
}

/*
 desc : Vision State ���°� �־��� �ð� �ȿ� ���ŵǾ����� Ȯ��
 parm : time	- [in]  �� �ð� �̳��� ���ŵǾ����� Ȯ�� �ð� (����: msec)
 retn : TRUE (�־��� �ð����� ª�� ���ŵǾ���)
		FALSE (�־��� �ð� �̳��� ���ŵ��� �ʾҴ�. ��, ���ŵ� �����Ͱ� ���� ����.)
*/
BOOL CBSAThread::IsUpdateStateTime(UINT64 time)
{
	/* ���� �ð� ���, �ֱٿ� ������ �ð��� ����, �־��� �ð����� ū�� ���� */
	return (GetTickCount64() > (m_pstShMemVisi->update_state_time + time)) ? FALSE : TRUE;
}
