
/*
 desc : TCP/IP Client Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "TrigThread.h"
#include "SendQue.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : trig	- [in]  Trigger Server ��� ����
		conf	- [in]  Trigger ȯ�� ����
		shmem	- [in]  Trigger Shared Memory
		comn	- [in]  ��ü ���� ȯ�� ����
 retn : None
*/
CTrigThread::CTrigThread(LPG_CTCS trig, LPG_CTSP conf, LPG_TPQR shmem,
						 LPG_DLSM link, LPG_CCGS comn)
	: CClientThread(ENG_EDIC::en_trig,
					0x0000,
					trig->tcp_port,
					trig->source_ipv4,	// CMPS Local (Client) IPv4
					trig->target_ipv4,	// PLS Remote (Server) IPv4
					trig->port_buff,
					trig->recv_buff,
					trig->ring_buff,
					trig->idle_time,
					trig->link_time,
					trig->sock_time)
{
	/* ------------------------------------------- */
	/* RunWork �Լ� ������ �ٷ� ���� �ϵ��� ó���� */
	/* ------------------------------------------- */
	m_bInitRecv		= FALSE;	/* �ʱ� ���ŵ� �����Ͱ� �������� ���� */
	m_bInitUpdate	= FALSE;
	m_u64RecvTime	= GetTickCount64();
	m_u64SendTime	= GetTickCount64();
	m_pstConfTrig	= conf;
	m_pstShMemTrig	= shmem;
	m_pstShMemLink	= link;
	m_pstConfComn	= comn;

	/* Check whether it operates in demo mode */
	if (GetConfig()->IsRunDemo())	m_enSysID = ENG_EDIC::en_demo;

	/* ----------------------------------------------------------- */
	/* �����(���)�� ����� �ӽ÷� ������ �۽� ������ ��ü ���� */
	/* ----------------------------------------------------------- */
	m_pSendQue		= new CSendQue(256);
	ASSERT(m_pSendQue);
	/* ���� �ֱ� �� �ʱ�ȭ */
	UINT8 i, j;
	for (i=0; i<MAX_TRIG_CHANNEL; i++)
	{
		for (j=0; j<MAX_TRIG_CHANNEL; j++)
		{
			m_u32SendTrigPos[i][j]	= MAX_TRIG_POS;
		}
	}

#if (TRIG_RECV_OUTPUT)
	m_u64TraceTime	= 0;
#endif
}

/*
 desc : Destructor
 parm : None
*/
CTrigThread::~CTrigThread()
{
	/* Trigger Server�� ���� ���� ���� */
	CloseClient();
	/* �۽� ť �޸� ���� */
	delete m_pSendQue;
}

/*
 desc : ������ ����� �ֱ��� ȣ���
 parm : None
 retn : None
*/
VOID CTrigThread::DoWork()
{
	BOOL bSendData	= FALSE;
	/* ���� �۾� �ð� */
	UINT64 u64Tick	= GetTickCount64();
#if 1
	/* ���� �ð� ���� ���ŵ� �����Ͱ� ���ٸ� �������� ó�� */
	if (m_u32IdleTime && ((m_u64RecvTime + UINT64(m_u32IdleTime)) < u64Tick))
	{
		/* ������ ���� ���� �Ǿ��ٰ� �α� ���� */
		TCHAR tzMesg[LOG_MESG_SIZE]	= {NULL};
		swprintf_s(tzMesg, LOG_MESG_SIZE,
				   L"Alive Time-out (idle time-out : recv_time:%I64u + idle_time:%I32u < cur_time:%I64u)",
				   m_u64RecvTime, m_u32IdleTime, u64Tick);
		LOG_ERROR(ENG_EDIC::en_trig, tzMesg);
		/* ���� �ֱ� �۾� �ð� ���� */
		m_u64RecvTime	= u64Tick;
		/* ������ ���� ���� ���� */
		CloseClient();
		return;
	}
#endif
	/* �ʱ� �۽ſ� ���� ������ �ְ�, ť�� �۽ŵ� ��ɰ� ���ٸ�, �۽� �۾� ���� */
	if (IsPopEmptyData())
	{
		/* ------------------------------------------------------------------ */
		/* �ֱ������� Trigger�� ���� ���� �о���� �ֱ�� 1000 msec ���� ���� */
		/* ------------------------------------------------------------------ */
		if (u64Tick > (m_u64SendTime + 250))
		{
			bSendData	= TRUE;	/* Trigger ���� �� ��û ��Ŷ �۽� */
			ReqReadSetup();
		}
	}
	else
	{
		/* ---------------------------------------------------------------------- */
		/* ��� ���� ���ŵ� �����Ͱ� ������ 50 msec ���Ŀ�, �۽� �����ϰ�, ������ */
		/* �۽ŵ� �����Ϳ� ����, 250 msec ���ð� ���� ���ŵ��� ������ �۽� ���� */
		/* ---------------------------------------------------------------------- */
		if (u64Tick > (m_u64SendTime + 250) || u64Tick > (m_u64RecvTime + 50))
		{
			bSendData	= TRUE;		/* ���� ��Ŷ ������ �۽� */
		}
	}

	/* ��Ŷ ������ �۽� ���� */
	if (bSendData && (u64Tick > (m_u64SendTime + 250)))
	{
		PopSendData();
	}
}

/*
 desc : Camera Index (1 or 2)�� ���� ä�� ��ȣ �� ��ȯ
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
 retn : Camera Index�� �ش�Ǵ� Trigger Board�� Channel ID (0x01 ~ 0x04) �� ��ȯ
		0x00 ���̸� ���� ó��
*/
UINT8 CTrigThread::GetTrigChNo(UINT8 cam_id)
{
#if 1
	UINT8 u8Type	= m_pstConfComn->strobe_lamp_type;
	if (0x01 == cam_id)
	{
		if (!u8Type)	return 0x01;
		else			return 0x02;
	}
	else
	{
		if (!u8Type)	return 0x03;
		else			return 0x04;
	}
#else	/* Trigger Event�� ������ 1 or 3 CH������ �߻����Ѿ� �� (���� DI ��� ������ ����) */
	if (0x01 == cam_id)	return 0x01;
	else				return 0x03;
#endif

	return 0x00;
}

/*
 desc : �����Ͱ� ���ŵ� ���, ȣ���
 parm : None
 retn : TRUE - ���������� �м� (Ring Buffer) ó��
		FALSE- �м� ó������ ���� (���� �Ϻ����� ���� ��Ŷ��)
*/
BOOL CTrigThread::RecvData()
{
	UINT32 u32Read	= 0;
	STG_TPQR stData	= {NULL};		// �� �ǽð� ���ŵǴ� ������ �ӽ� ����

	/* ���� ��Ŷ ���� ����� ���� */
	m_enSocket	= ENG_TPCS::en_recving;

	do	{

		/* ----------------------------------------------------------------- */
		/* ���� ���ŵ� �����Ͱ� �⺻ ��� ���� ũ�⸸ŭ ����Ǿ� �ִ��� Ȯ�� */
		/* ----------------------------------------------------------------- */
		u32Read	= m_pPktRingBuff->GetReadSize();
		if (u32Read < sizeof(STG_TPQR))	break;
		/* ----------------------------------------------------------- */
		/* ���� ������ Copy�� ���� ��� �κ��� ���۷κ��� ���� �� ���� */
		/* ----------------------------------------------------------- */
		m_pPktRingBuff->ReadBinary((PUINT8)&stData, sizeof(STG_TPQR));
		/* ----------------------------------------------------------- */
		/* �Ʒ� NotiEvent �Լ����� STG_MFPR�� ũ�� ���� ū �ǹ̰� ���� */
		/* ----------------------------------------------------------- */
		SetRecvPacket(ENG_TPCS::en_recved, (PUINT8)&stData, sizeof(STG_TPQR));
		/* �ʱ� �����Ͱ� ���ŵǾ��ٰ� ���� */
		if (!m_bInitRecv)	m_bInitRecv	= TRUE;

	} while (1);

	/* --------------------------------------------------------- */
	/* ���� �ֱٿ� ���ŵ� �ð� ����. ��ð� ���� ������ ���� ����*/
	/* --------------------------------------------------------- */
	m_u64RecvTime	= GetTickCount64();

	return TRUE;
}

/*
 desc : ��Ŷ ���� ó��
 parm : type	- [in]  �̺�Ʈ ���� (ENG_TPCS)
		data	- [in]  �۽� or ���ŵ� �����Ͱ� ����� ����ü ������
		size	- [in]  'data' ���ۿ� ����� ũ��
 retn : None
*/
VOID CTrigThread::SetRecvPacket(ENG_TPCS type, PUINT8 data, UINT32 size)
{
	LPG_TPQR pstData	= LPG_TPQR(data);
	/* ���ŵ� ������ Big -> Little Endian */
	pstData->SWAP();

	/* Shared Memory ���� */
	if (ENG_TPCS::en_recved == type)
	{
		memcpy(m_pstShMemTrig, data, size);
	}
#if (TRIG_RECV_OUTPUT)
	if (pstData->trig_strob_ctrl == 0x01 && m_u64TraceTime+1000 <GetTickCount64())
	{
		m_u64TraceTime	= GetTickCount64();
		LPG_TPQR d = pstData;
		for (UINT8 i=0; i<MAX_TRIG_CHANNEL; i++)
		{
			LPG_TPTS t = &pstData->trig_set[i];
			TRACE(L"EncCounter = %10d TrigStrobCtrl = %1u EncOutVal = %10u "
				  L"CH = <%1u> AreaOnTime.Trig/Strobe/Delay = <%5u> <%5u> <%4u> "
				  L"AreaTrigPos.Plus/Minus = <%5u> <%5u> Trig.Pos = [%10u] [%10u] [%10u] [%10u]\n",
				  d->enc_counter, d->trig_strob_ctrl, d->enc_out_val,
				  i+1, t->area_trig_ontime, t->area_strob_ontime, t->area_strob_trig_delay,
				  t->area_trig_pos_plus, t->area_trig_pos_minus,
				  t->area_trig_pos[0], t->area_trig_pos[1],
				  t->area_trig_pos[2], t->area_trig_pos[3]);
		}
		TRACE("\n");
	}
#endif
}

/*
 desc : ������ �����̵� ���
 parm : sd	- ���� �����
 retn : TRUE or FALSE
*/
BOOL CTrigThread::Connected()
{
	/* ����Ǿ��� ��, �� �ð� ��� ���� */
	m_u64SendTime	= GetTickCount64();
	m_u64RecvTime	= GetTickCount64();
	/* ����ǰ� �� ���� �ʱ�ȭ ��û���� �ʾҴٰ� ���� */
	m_bInitRecv		= FALSE;	/* �ʱ� ����ǰ� �� ���� ���ŵ� �����Ͱ� ���ٰ� ���� */
	/* ���� �Ϸ� ���� */
	UpdateLinkTime(0x01);
#if 1
	/* �ʱ� �� ���� ��ɾ� �۽� */
	return ReqInitUpdate();
#else
	return TRUE;
#endif
}

/*
 desc : �����κ��� ���� ������ �� ���
 parm : None
 retn : None
*/
VOID CTrigThread::CloseSocket(SOCKET sd, UINT32 time_out/* =0 */)
{

	// ���� ��ü ȣ��
	CClientThread::CloseSocket(sd, time_out);
}

/*
 desc : ���� �۽ŵ� ��Ŷ�� �����ϴ��� ���� Ȯ��
 parm : None
 retn : FALSE - POP �� �����Ͱ� �ִ�. TRUE - POP �� �����Ͱ� ����
*/
BOOL CTrigThread::IsPopEmptyData()
{
	BOOL bIsEmpty	= TRUE;

	/* ����ȭ ���� */
	if (m_syncSend.Enter())
	{
		// Queue�� ��� �ִٸ� TRUE�̹Ƿ�, POP �� �� ����.
		bIsEmpty = m_pSendQue->IsQueEmpty();

		/* ����ȭ ���� */
		m_syncSend.Leave();
	}

	return bIsEmpty;
}

/*
 desc : �۽��� �����Ͱ� ������, Queue�κ��� ������ �ϳ� ������ �� �۽� �۾� ����
 parm : None
 retn : None
*/
VOID CTrigThread::PopSendData()
{
	BOOL bSucc		= FALSE;
	LPG_TPQR pstQue	= NULL;

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		/* ---------------------------------- */
		/* ť�� ���� �����Ͱ� �����ϴ��� Ȯ�� */
		/* ---------------------------------- */
		pstQue	= m_pSendQue->PopData();
		if (pstQue)
		{
			/* !!! �ݵ�� �۽� �ð� ������ ���⿡ ����. !!! ���� �ֱٿ� �۽��� �ð� ���� */
			m_u64SendTime	= GetTickCount64();
			/* !!! �߿� !!! -> ������ Link ũ�� ���� */
			/* ������ ���� */
			bSucc = ReSended((PUINT8)pstQue, sizeof(STG_TPQR), UINT32(3), 30);
			/* �۽� �����̸� ... */
			if (!bSucc)
			{
				LOG_ERROR(ENG_EDIC::en_trig, L"Failed to send the packet data [Trigger Info]");
			}
		}

		/* ���� ���� */
		m_syncSend.Leave();
	}
}

/*
 desc : Trigger Board�� ������ �� ��û
 parm : None
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ReqReadSetup()
{
	BOOL bSucc			= FALSE;
	STG_TPQR stPktData	= {NULL};

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		/* �ʱ⿡ EEPROM�� ����� ���� �������� */
		stPktData.command	= htonl(UINT32(ENG_TBPC::en_read));	/* SwapData �Լ� ȣ���� ���, ���ϰ� ũ�Ƿ� */
		/* �۽ŵ� ������ ���ۿ� ��� */
		bSucc = m_pSendQue->PushData(&stPktData);
		if (!bSucc)
		{
			LOG_WARN(ENG_EDIC::en_trig, L"The send packet buffer is full");
		}
		/* ���� ���� */
		m_syncSend.Leave();
	}

	return bSucc;
}

#if 0
/*
 desc : Trigger Board�� ó�� ����ǰ� �� ����, ȯ�� ���Ͽ� ������ ������ �ʱ�ȭ ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ReqInitUpdate()
{
	BOOL bSucc			= FALSE;
	UINT32 i = 0, j		= 0;
	STG_TPQR stPktData	= {NULL};

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		/* ���� ���ŵ� ������ �ʱ�ȭ */
		m_pstShMemTrig->ResetPktData();

		/* 1th : Base Setup */
		stPktData.command			= UINT32(ENG_TBPC::en_write);
		stPktData.enc_counter		= INT32(ENG_TECM::en_clear);
		stPktData.trig_strob_ctrl	= UINT32(ENG_TEED::en_disable);
		/* �ʱ� �� ���� */
		for (i=0x00; i<0x04; i++)
		{
			stPktData.trig_set[i].area_trig_ontime		= m_pstConfTrig->trig_on_time[i];
			stPktData.trig_set[i].area_strob_ontime		= m_pstConfTrig->strob_on_time[i];
			stPktData.trig_set[i].area_trig_pos_plus	= m_pstConfTrig->trig_plus[i];
			stPktData.trig_set[i].area_trig_pos_minus	= m_pstConfTrig->trig_minus[i];
			stPktData.trig_set[i].area_strob_trig_delay	= m_pstConfTrig->trig_delay_time[i];
			/* �ʱ� Ʈ���� �߻� ��ġ ���� 0�� �ƴ� MAX ������ ���� */
			for (j=0; j<MAX_REGIST_TRIG_POS_COUNT; j++)
			{
				stPktData.trig_set[i].area_trig_pos[j]	= MAX_TRIG_POS;
			}
		}
		/* Network Bytes Ordering */
		stPktData.SWAP();
		bSucc = m_pSendQue->PushData(&stPktData);

		/* ���� ���� */
		m_syncSend.Leave();
	}

	/* 2th : Internal Trigger - Disabled */
	ReqWriteEncoderOut(0x00000000);

	return bSucc ? ReqReadSetup() : FALSE;
}
#else
/*
 desc : Ư���� ��쿡�� ���. (Ʈ���ź����� Ʈ���Ŵ� 2�� �ۿ� �߻��ȵǱ� ������...)
 parm : None
 retn : TRUE or FALSE
 note : Ʈ���Ŵ� ������ 1, 3 �� ī�޶�(����;ä��)�� �߻��ϱ� ������...
		2, 4�� ī�޶� (����;ä��)�� �߻���Ű�� ���� ���
*/
BOOL CTrigThread::ReqInitUpdate()
{
	BOOL bSucc			= FALSE;
	UINT8 u8LampType	= m_pstConfComn->strobe_lamp_type;	/* 0x00: Coaxial, 0x01: Ring */
	UINT32 i = 0, j		= 0;
	STG_TPQR stPktData	= {NULL};

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		/* ���� ���ŵ� ������ �ʱ�ȭ */
		m_pstShMemTrig->ResetPktData();

		/* 1th : Base Setup */
		stPktData.command			= UINT32(ENG_TBPC::en_write);
		stPktData.enc_counter		= INT32(ENG_TECM::en_clear);
		stPktData.trig_strob_ctrl	= UINT32(ENG_TEED::en_disable);
		/* Ʈ���� �̺�Ʈ ���� */
		SetTrigEvent(TRUE, stPktData.trig_set);
		/* Network Bytes Ordering */
		stPktData.SWAP();
		bSucc = m_pSendQue->PushData(&stPktData);

		/* ���� ���� */
		m_syncSend.Leave();
	}
#if 0
	/* 2th : Internal Trigger - Disabled */
	ReqWriteEncoderOut(0x00000000);
#endif
	return bSucc ? ReqReadSetup() : FALSE;
}
#endif

/*
 desc : ���� ��ϵ� 4�� ä�ο� ���� Trigger Position �� �ʱ�ȭ (�ִ� ��) ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ResetTrigPosAll()
{
	BOOL bSucc			= FALSE;
	UINT32 i = 0, j		= 0;
	STG_TPQR stPktData	= {NULL};

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		stPktData.command	= (UINT32)ENG_TBPC::en_write;
		/* ���� �� ���� */
		memcpy(&stPktData, m_pstShMemTrig, sizeof(STG_TPQR));
		/* �ʱ� �� ���� */
		for (; i<MAX_TRIG_CHANNEL; i++)
		{
			/* �ʱ� Ʈ���� �߻� ��ġ ���� 0�� �ƴ� MAX ������ ���� */
			for (j=0; j<MAX_REGIST_TRIG_POS_COUNT; j++)	stPktData.trig_set[i].area_trig_pos[j]	= MAX_TRIG_POS;
		}
		/* �ֱ� Ʈ���� ��ġ �� �ʱ�ȭ (�ݵ�� ��Ŷ ���ۿ� �ִ°ͺ��� �տ� �����ؾ� ��) */
		memset(m_u32SendTrigPos, MAX_TRIG_POS, sizeof(UINT32)*MAX_TRIG_CHANNEL*MAX_REGIST_TRIG_POS_COUNT);
		/* Network Bytes Ordering */
		stPktData.SWAP();
		bSucc = m_pSendQue->PushData(&stPktData);

		/* ���� ���� */
		m_syncSend.Leave();
	}

	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : Trigger Board�� ����ǰ� �� ���� EEPROM ������ �а�/���� ����
 parm : mode	- [in]  0x01 - Read, 0x02 - Write
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ReqEEPROMCtrl(UINT8 mode)
{
	BOOL bSucc			= FALSE;
	UINT32 u32Cmd		= mode == 0x01 ? UINT32(ENG_TBPC::en_eeprom_read) : UINT32(ENG_TBPC::en_eeprom_write);
	STG_TPQR stPktData	= {NULL};

	/* ���� EEPROM���κ��� �б� ����� ���, ���� �� ��� ����� */
	if (0x01 == mode)	m_pstShMemTrig->ResetPktData();

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		/* �ʱ⿡ EEPROM�� ����� ���� �������� */
		stPktData.command	= htonl(u32Cmd);	/* SwapData �Լ� ȣ���� ���, ���ϰ� ũ�Ƿ� */
		bSucc = m_pSendQue->PushData(&stPktData);

		/* ���� ���� */
		m_syncSend.Leave();
	}

	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : Board Reset
 parm : None
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ReqWriteBoardReset()
{
	BOOL bSucc			= FALSE;
	STG_TPQR stPktData	= {NULL};

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		/* ��ɾ� ���� */
		stPktData.command	= htonl(UINT32(ENG_TBPC::en_reset));
		/* �۽ŵ� ������ ���ۿ� ��� */
		bSucc = m_pSendQue->PushData(&stPktData);
		/* ���� ���� */
		m_syncSend.Leave();
	}
#if 0
	/* ���� ���ŵ� ������ �ʱ�ȭ */
	m_pstShMemTrig->ResetPktData();
#endif
#if 0	/* SW Reset�̸�, �ٷ� ������ �����Ƿ�, �б� ��û�ص� ���ǹ� */
	/* ������ 1�� �� ��û (������ �ޱ� ����) */
	return bSucc ? ReqReadSetup() : FALSE;
#else
#if 0	/* ���߿� Trigger Board���� ���� ���� �ñ׳��� ���� ������ �ȵ� */
	if (bSucc)
	{
		/* �������� �������Ƿ�, ���⼭�� ���� Close ���� ? */
		CloseSocket(m_sdClient);
	}
#endif
	return bSucc;
#endif
}

/*
 desc : Area Trigger Event ����
 parm : direct	- [in]  TRUE: ������, FALSE: ������
		trig_set- [out] Trigger Event ���� ���� �� ��ȯ
 retn : None
 note : Trigger Event�� �޴� ���� ä�� 1/3 �����̰�, Strobe Event�� �޴� ���� ä�� 1/2/3/4 ��� ����
		ä�� 2/4���� ���� �̹����� �������� ä�� 1/3������ Trigger �̺�Ʈ �߻� ���Ѿ� ��
*/
VOID CTrigThread::SetTrigEvent(BOOL direct, LPG_TPTS trig_set)
{
	UINT8 u8LampType= m_pstConfComn->strobe_lamp_type;	/* 0x00: Coaxial, 0x01: Ring */
	UINT32 i = 0;
#if 0
	/* �ʱ� �� ���� */
	for (i=0x00; i<0x04; i++)
	{
		/* Camera 1 / 2 - Coaxial Lamp (for 1 & 3 ch) */
		if (0x00 == u8LampType)
		{
			/* Coaxial Lamp Ȱ��ȭ ���� */
			if (i == 0x00 || i == 0x02)
			{
				trig_set[i].area_trig_ontime				= m_pstConfTrig->trig_on_time[i];
				trig_set[i].area_strob_ontime				= m_pstConfTrig->strob_on_time[i];
				trig_set[i].area_strob_trig_delay			= m_pstConfTrig->trig_delay_time[i];
				if (direct)	trig_set[i].area_trig_pos_plus	= m_pstConfTrig->trig_plus[i];
				else		trig_set[i].area_trig_pos_minus	= m_pstConfTrig->trig_minus[i];
			}
		}
		/* Camera 1 / 2 - Ring Lamp (for 2 & 4 ch) (DI ��� ������ ������ ���� T.T) */
		else
		{
			/* 1 & 3 CH�� ���, Trigger�� �߻���Ű��, Strobe �� �߻����� �ʵ��� �ؾ� �� */
			if (i == 0x00 || i == 0x02)
			{
				/* Trigger On Time�� ���, 1 & 3 CH�� ���, 2 & 4 CH�� ���� �����ǵ��� �� */
				trig_set[i].area_trig_ontime				= m_pstConfTrig->trig_on_time[i+1];
				if (direct)	trig_set[i].area_trig_pos_plus	= m_pstConfTrig->trig_plus[i+1];
				else 		trig_set[i].area_trig_pos_minus	= m_pstConfTrig->trig_minus[i+1];
			}
			/* 2 & 4 ch�� ���, Trigger�� �߻��ص� �ǹ� ���� (HW ������ ����) Strobe �� �߻��ϵ��� ���� */
			else
			{
				trig_set[i].area_strob_ontime		= m_pstConfTrig->strob_on_time[i];
				trig_set[i].area_strob_trig_delay	= m_pstConfTrig->trig_delay_time[i];
			}
		}
	}
#else
#if 0
	/* �ʱ� �� ���� */
	for (i=0x00; i<0x04; i++)
	{
		/* Camera 1 / 2 - Coaxial Lamp (for 1 & 3 ch) */
		if (0x00 == u8LampType)
		{
			/* Coaxial Lamp Ȱ��ȭ ���� */
			if (i == 0x00 || i == 0x02)
			{
				trig_set[i].area_trig_ontime		= m_pstConfTrig->trig_on_time[i];
				trig_set[i].area_strob_ontime		= m_pstConfTrig->strob_on_time[i];
				trig_set[i].area_strob_trig_delay	= m_pstConfTrig->trig_delay_time[i];
				if (direct)
				{
					trig_set[i].area_trig_pos_plus	= m_pstConfTrig->trig_plus[i];
					trig_set[i].area_trig_pos_minus	= m_pstConfTrig->trig_minus[i];
				}
				else
				{
					trig_set[i].area_trig_pos_plus	= m_pstConfTrig->trig_minus[i];
					trig_set[i].area_trig_pos_minus	= m_pstConfTrig->trig_plus[i];
				}
			}
		}
		/* Camera 1 / 2 - Ring Lamp (for 2 & 4 ch) (DI ��� ������ ������ ���� T.T) */
		else
		{
			/* 1 & 3 CH�� ���, Trigger�� �߻���Ű��, Strobe �� �߻����� �ʵ��� �ؾ� �� */
			if (i == 0x00 || i == 0x02)
			{
				/* Trigger On Time�� ���, 1 & 3 CH�� ���, 2 & 4 CH�� ���� �����ǵ��� �� */
				trig_set[i].area_trig_ontime		= m_pstConfTrig->trig_on_time[i+1];
				if (direct)
				{
					trig_set[i].area_trig_pos_plus	= m_pstConfTrig->trig_plus[i+1];
					trig_set[i].area_trig_pos_minus	= m_pstConfTrig->trig_minus[i+1];
				}
				else 
				{
					trig_set[i].area_trig_pos_plus	= m_pstConfTrig->trig_minus[i+1];
					trig_set[i].area_trig_pos_minus	= m_pstConfTrig->trig_plus[i+1];
				}
			}
			/* 2 & 4 ch�� ���, Trigger�� �߻��ص� �ǹ� ���� (HW ������ ����) Strobe �� �߻��ϵ��� ���� */
			else
			{
				trig_set[i].area_strob_ontime		= m_pstConfTrig->strob_on_time[i];
				trig_set[i].area_strob_trig_delay	= m_pstConfTrig->trig_delay_time[i];
			}
		}
	}
#else
	/* �ʱ� �� ���� */
	for (i=0x00; i<0x04; i++)
	{
		trig_set[i].area_trig_ontime		= m_pstConfTrig->trig_on_time[i];
		trig_set[i].area_strob_ontime		= m_pstConfTrig->strob_on_time[i];
		trig_set[i].area_strob_trig_delay	= m_pstConfTrig->trig_delay_time[i];
#if 0
		if (direct)
		{
			trig_set[i].area_trig_pos_plus	= 0/*m_pstConfTrig->trig_plus[i]*/;
			trig_set[i].area_trig_pos_minus	= 0/*m_pstConfTrig->trig_minus[i]*/;
		}
		else
		{
			trig_set[i].area_trig_pos_plus	= 0/*m_pstConfTrig->trig_minus[i]*/;
			trig_set[i].area_trig_pos_minus	= 0/*m_pstConfTrig->trig_plus[i]*/;
		}
#else
		trig_set[i].area_trig_pos_plus		= 0/*m_pstConfTrig->trig_plus[i]*/;
		trig_set[i].area_trig_pos_minus		= 0/*m_pstConfTrig->trig_minus[i]*/;
#endif
	}
#endif
#endif
}

/*
 desc : Area Trigger Position ����
 parm : trig_set- [in]  Trigger Position ����
		start	- [in]  Ʈ���� ���� ��ġ (0 ~ 15)
		count	- [in]  Ʈ���� ��� ���� (1 ~ 16)
		pos		- [in]  Ʈ���� ��� ��ġ�� ����Ǿ� �ִ� �迭 ������ (unit : 100 nm or 0.1 um)
 note : Trigger Event�� �޴� ���� ä�� 1/3 �����̰�, Strobe Event�� �޴� ���� ä�� 1/2/3/4 ��� ����
		ä�� 2/4���� ���� �̹����� �������� ä�� 1/3������ Trigger �̺�Ʈ �߻� ���Ѿ� ��
*/
VOID CTrigThread::SetTrigPos(LPG_TPTS trig_set,
							 UINT8 start1, UINT8 count1, PUINT32 pos1,
							 UINT8 start2, UINT8 count2, PUINT32 pos2)
{
	UINT8 u8LampType= m_pstConfComn->strobe_lamp_type;	/* 0x00: Coaxial, 0x01: Ring */
	UINT32 i = 0;
	UINT8 u8ChNo[2]	= {NULL};

	/* �� Align Camera�� ���� ä�� ��ȣ ��� */
	u8ChNo[0]	= GetTrigChNo(0x01) - 1;
	u8ChNo[1]	= GetTrigChNo(0x02) - 1;

	/* Camera 1 / 2 - Coaxial Trigger (for 1 & 3 ch) */
	if (0x00 == u8LampType)
	{
		/* ���ο� �� ���� ����, ���� ������ �� �ӽ� ���� */
		memcpy(&trig_set[u8ChNo[0]].area_trig_pos+start1, pos1, sizeof(UINT32) * count1);
		memcpy(&trig_set[u8ChNo[1]].area_trig_pos+start2, pos2, sizeof(UINT32) * count2);
		memcpy(&m_u32SendTrigPos[u8ChNo[0]]+start1, pos1, sizeof(UINT32) * count1);
		memcpy(&m_u32SendTrigPos[u8ChNo[1]]+start2, pos2, sizeof(UINT32) * count2);
	}
	/* Camera 1 / 2 - Ring Trigger (for 2 & 4 ch) (DI ��� ������ ������ ���� T.T) */
	else
	{
		/* �ϴ� ������ 1/3 ä�ο��� Trigger ��ġ ���� (ī�޶� �̺�Ʈ �߻� ��Ű�� ����) */
		memcpy(&trig_set[u8ChNo[0]-1].area_trig_pos+start1, pos1, sizeof(UINT32) * count1);
		memcpy(&trig_set[u8ChNo[1]-1].area_trig_pos+start2, pos2, sizeof(UINT32) * count2);
		/* ���ο� �� ���� ����, ���� ������ �� �ӽ� ���� */
		memcpy(&trig_set[u8ChNo[0]].area_trig_pos+start1, pos1, sizeof(UINT32) * count1);
		memcpy(&trig_set[u8ChNo[1]].area_trig_pos+start2, pos2, sizeof(UINT32) * count2);
		memcpy(&m_u32SendTrigPos[u8ChNo[0]]+start1, pos1, sizeof(UINT32) * count1);
		memcpy(&m_u32SendTrigPos[u8ChNo[1]]+start2, pos2, sizeof(UINT32) * count2);
	}
}

/*
 desc : ä�� ���� Ʈ���� ��ġ ���
 parm : direct	- [in]  TRUE: ������, FALSE: ������
		start	- [in]  Ʈ���� ���� ��ġ (0 ~ 15)
		count	- [in]  Ʈ���� ��� ���� (1 ~ 16)
		pos		- [in]  Ʈ���� ��� ��ġ�� ����Ǿ� �ִ� �迭 ������ (unit : 100 nm or 0.1 um)
		enable	- [in]  Ʈ���� Enable or Disable
		clear	- [in]  Ʈ���� ��ġ ����ϸ鼭 ���� Trigger Encoder ����
						Clear (TRUE)�Ұ����� Ȥ�� �б� ��� (FALSE)�� ������ ������ ����
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ReqWriteAreaTrigPos(BOOL direct,
									  UINT8 start1, UINT8 count1, PUINT32 pos1,
									  UINT8 start2, UINT8 count2, PUINT32 pos2,
									  ENG_TEED enable, BOOL clear)
{
	UINT8 i1 = 0, i2	= 0, u3MaxTrig = MAX_REGIST_TRIG_POS_COUNT;
	UINT8 u8ChNo[2]		= {NULL};
	BOOL bSucc			= FALSE;
	STG_TPQR stPktData	= {NULL};

	/* �� Align Camera�� ���� ä�� ��ȣ ��� */
	u8ChNo[0]	= GetTrigChNo(0x01);
	u8ChNo[1]	= GetTrigChNo(0x02);

	if ((u8ChNo[0] < 1 || u8ChNo[0] > 4 || !(start1 >= 0 && start1 < MAX_REGIST_TRIG_POS_COUNT) || !pos1 ||
		 (start1 + count1) > MAX_REGIST_TRIG_POS_COUNT) ||
		(u8ChNo[1] < 1 || u8ChNo[1] > 4 || !(start2 >= 0 && start2 < MAX_REGIST_TRIG_POS_COUNT) || !pos2 ||
		 (start2 + count2) > MAX_REGIST_TRIG_POS_COUNT))
	{
		TCHAR tzMesg[LOG_MESG_SIZE]	= {NULL};
		swprintf_s(tzMesg, LOG_MESG_SIZE,
				   L"Trigger Invalid Parameter (ch_no1:%d ch_no2:%d start1:%d count1:%d start2:%d count2:%d)",
				   u8ChNo[0], u8ChNo[1], start1, count1, start2, count2);
		LOG_ERROR(ENG_EDIC::en_trig, tzMesg);
		return FALSE;
	}
	u8ChNo[0]--;
	u8ChNo[1]--;

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		stPktData.command			= (UINT32)ENG_TBPC::en_write;									/* Write */
		stPktData.enc_counter		= clear ? INT32(ENG_TECM::en_clear) : INT32(ENG_TECM::en_read);	/* Clear to encoder */
		stPktData.trig_strob_ctrl	= (UINT32)enable;														/* Trigger Enable or Disable */
		/* ���� Area Trigger �� ��� ���� */
		memcpy(stPktData.trig_set, m_pstShMemTrig->trig_set, sizeof(STG_TPTS) * 4 /* Max 4 channel */);
		/* Ʈ���� �̺�Ʈ ���� */
		SetTrigEvent(direct, stPktData.trig_set);
		/* Ʈ���� ��ġ ���� */
		SetTrigPos(stPktData.trig_set, start1, count1, pos1, start2, count2, pos2);
		/* ������ Ʈ���� ��� ��ġ�� �ִ� �� ���� */
		for (i1=0; i1<start1; i1++)					stPktData.trig_set[u8ChNo[0]].area_trig_pos[i1] = MAX_TRIG_POS;
		for (i1=start1+count1; i1<u3MaxTrig; i1++)	stPktData.trig_set[u8ChNo[0]].area_trig_pos[i1] = MAX_TRIG_POS;
		for (i2=0; i2<start2; i2++)					stPktData.trig_set[u8ChNo[1]].area_trig_pos[i2] = MAX_TRIG_POS;
		for (i2=start2+count1; i2<u3MaxTrig; i2++)	stPktData.trig_set[u8ChNo[1]].area_trig_pos[i2] = MAX_TRIG_POS;

		stPktData.SWAP();

		/* �۽ŵ� ������ ���ۿ� ��� */
		bSucc = m_pSendQue->PushData(&stPktData);

		/* ���� ���� */
		m_syncSend.Leave();
	}

	/* ���� ���� �� �б� ��û */
	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : Ư�� ä�ο� Ʈ���� ��ġ ���
 parm : cam_id	- [in]  Align Camera Index (0x01 or 0x02)
		start	- [in]  Ʈ���� ���� ��ġ (0 ~ 15)
		count	- [in]  Ʈ���� ��� ���� (1 ~ 16)
		pos		- [in]  Ʈ���� ��� ��ġ�� ����Ǿ� �ִ� �迭 ������ (unit : 100 nm or 0.1 um)
		enable	- [in]  Ʈ���� Enable or Disable
		clear	- [in]  Ʈ���� ��ġ ����ϸ鼭 ���� Trigger Encoder ����
						Clear (TRUE)�Ұ����� Ȥ�� �б� ��� (FALSE)�� ������ ������ ����
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ReqWriteAreaTrigPosCh(UINT8 cam_id, UINT8 start, UINT8 count, PUINT32 pos,
										ENG_TEED enable, BOOL clear)
{
	UINT8 i =0 ,j		= 0, u8ChNo;
	BOOL bSucc			= FALSE;
	STG_TPQR stPktData	= {NULL};

	/* �� Align Camera�� ���� ä�� ��ȣ ��� */
	u8ChNo	= GetTrigChNo(cam_id);

	if (u8ChNo < 1 || u8ChNo > 4 || !(start >= 0 && start < MAX_REGIST_TRIG_POS_COUNT) || !pos ||
		(start + count) > MAX_REGIST_TRIG_POS_COUNT)
	{
		TCHAR tzMesg[LOG_MESG_SIZE]	= {NULL};
		swprintf_s(tzMesg, LOG_MESG_SIZE,
				   L"Trigger Invalid Parameter (ch_no:%d start:%d count:%d)",
				   u8ChNo, start, count);
		LOG_ERROR(ENG_EDIC::en_trig, tzMesg);
		return FALSE;
	}
	u8ChNo--;

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		stPktData.command			= (UINT32)ENG_TBPC::en_write;									/* Write */
		stPktData.enc_counter		= clear ? (INT32)ENG_TECM::en_clear : (INT32)ENG_TECM::en_read;	/* Clear to encoder */
		stPktData.trig_strob_ctrl	= (UINT32)enable;												/* Trigger Enable or Disable */
		/* Ʈ���� �̺�Ʈ ���� */
		SetTrigEvent(TRUE, stPktData.trig_set);
		/* ���ο� ���� ���� �� */
		memcpy(&stPktData.trig_set[u8ChNo].area_trig_pos+start, pos, sizeof(UINT32) * count);
		/* ���� �ֱ��� Ʈ���� �۽� ��ġ �� ���� */
		memcpy(&m_u32SendTrigPos[u8ChNo]+start, pos, sizeof(UINT32) * count);
		stPktData.SWAP();
		/* �۽ŵ� ������ ���ۿ� ��� */
		bSucc = m_pSendQue->PushData(&stPktData);
		/* ���� ���� */
		m_syncSend.Leave();
	}

	/* ���� ���� �� �б� ��û */
	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : Trigger & Strobe Enable or Disable
 parm : enable	- [in]  TRUE : Enable, FALSE : Disable
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ReqWriteTriggerStrobe(BOOL enable)
{
	UINT8 i	= 0x00;
	BOOL bSucc			= FALSE;
	STG_TPQR stPktData	= {NULL};

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		/* Encoder Counter Reset Command */
		stPktData.command			= (UINT32)ENG_TBPC::en_write;
		stPktData.enc_counter		= (INT32)ENG_TECM::en_clear;
		stPktData.trig_strob_ctrl	= enable ? (UINT32)ENG_TEED::en_enable : (UINT32)ENG_TEED::en_disable;
#if 1
		/* Ʈ���� �̺�Ʈ ���� */
		if (enable)	SetTrigEvent(TRUE, stPktData.trig_set);
#else	
		/* <!!! �߿� !!!> ���� Area Trigger �� ��� ���� */
		for (; i<MAX_TRIG_CHANNEL; i++)
		{
			stPktData.trig_set[i].area_trig_ontime		= m_pstConfTrig->trig_on_time[i];
			stPktData.trig_set[i].area_strob_ontime		= m_pstConfTrig->strob_on_time[i];
			stPktData.trig_set[i].area_strob_trig_delay	= m_pstConfTrig->trig_delay_time[i];
		}
#endif
		/* --------------------------------------------------------------------------------- */
		/* Triger & Strobe Enable ���� Disable�� ���� ��ϵ� Trigger Position �� ��� �ʱ�ȭ */
		/* --------------------------------------------------------------------------------- */
		/* �ִ� �� ���� */
		stPktData.ResetTrigPos(0);
		stPktData.ResetTrigPos(1);
		stPktData.ResetTrigPos(2);
		stPktData.ResetTrigPos(3);
		/* Network Bytes Ordering */
		stPktData.SWAP();
		/* �۽ŵ� ������ ���ۿ� ��� */
		bSucc = m_pSendQue->PushData(&stPktData);
		/* ���� ���� */
		m_syncSend.Leave();
	}

	/* ���� ���� �� �б� ��û */
	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : IP �����ϱ�
 parm : ip_addr	- [in]  ��Ŷ ������ ����� ����ü ������
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ReqWriteIP4Addr(PUINT8 ip_addr)
{
	BOOL bSucc			= TRUE;
	STG_TPQR stPktData	= {NULL};

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		/* ���� ��ϵ� IP �ּ� �ʱ�ȭ */
		m_pstShMemTrig->ResetIPv4();
		/* IP ������ ���� ��ɾ� ���� */
		if (bSucc)
		{
			stPktData.command	= htonl((UINT32)ENG_TBPC::en_ipaddr_write);
			memcpy(&stPktData.ip_addr, ip_addr, 4);
			bSucc = m_pSendQue->PushData(&stPktData);
		}
		if (bSucc)
		{
			/* ������ IP�� ��� ���� ��ɾ� ���� */
			stPktData.command	= htonl((UINT32)ENG_TBPC::en_ipaddr_read);
			bSucc = m_pSendQue->PushData(&stPktData);
		}
		/* ���� ���� */
		m_syncSend.Leave();
	}

	/* ���� ���� �� �б� ��û */
	return bSucc;	/* IP�� �ٲ�� ������ ����������, ������ �ϴ��� �ؾ� �� */
}

/*
 desc : ���� Ʈ���� ����
 parm : onoff	- [in]  1 ����Ʈ �� �ǹ̰� ���� (4 Bytes �̹Ƿ�, �� 4ä��)
						�ʴ� 10 frame ���
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ReqWriteEncoderOut(UINT32 enc_out)
{
	UINT8 i				= 0x00;
	BOOL bSucc			= FALSE;
	STG_TPQR stPktData	= {NULL};

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		/* ��ɾ� ���� */
		stPktData.command		= htonl((UINT32)ENG_TBPC::en_trig_internal);
		stPktData.enc_out_val	= htonl(enc_out);
		for (; i<4; i++)
		{
#if 0
			stPktData.trig_set[i].area_trig_ontime	= 2500;
			stPktData.trig_set[i].area_strob_ontime	= 2500;
#else	/* ���� DI ��� Ư�� ������ (Trigger�� ������ 1 & 3 CH ������ �߻� ���Ѿ� ��) */
			if (i == 0x00 || i == 0x02)	stPktData.trig_set[i].area_trig_ontime	= 2500;
			else						stPktData.trig_set[i].area_trig_ontime	= 0;
			stPktData.trig_set[0].area_strob_ontime	= 2500;
#endif
		}
		bSucc = m_pSendQue->PushData(&stPktData);
		/* ���� ���� */
		m_syncSend.Leave();
	}

	/* ���� ���� �� �б� ��û */
	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : ���� ä�� ���� Ʈ���� 1���� �߻�
 parm : enc_out	- [in]  1 ����Ʈ �� �ǹ̰� ���� (4 Bytes �̹Ƿ�, �� 4ä��)
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ReqWriteTrigOutOne(UINT32 enc_out)
{
	BOOL bSucc			= FALSE;
	STG_TPQR stPktData	= {NULL};

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		/* ��ɾ� ���� */
		stPktData.command		= htonl((UINT32)ENG_TBPC::en_trig_out_one);
		stPktData.enc_out_val	= htonl(enc_out);
		bSucc = m_pSendQue->PushData(&stPktData);
		if (!bSucc)
		{
			LOG_ERROR(ENG_EDIC::en_trig, L"The send packet buffer is full");
		}
		/* ���� ���� */
		m_syncSend.Leave();
	}

	/* ���� ���� �� �б� ��û */
	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : Reset Trigger Count
 parm : None
 retn : TRUE or FALSE
*/
BOOL CTrigThread::ReqResetTrigCount()
{
	BOOL bSucc			= FALSE;
	STG_TPQR stPktData	= {NULL};

	/* ���� ���� */
	if (m_syncSend.Enter())
	{
		/* Encoder Counter Reset Command */
		stPktData.command		= (UINT32)ENG_TBPC::en_write;
		stPktData.enc_counter	= (INT32)ENG_TECM::en_clear;	/* 0x03 (Clear),  0x02 (Read) */
		/* Network Bytes Ordering */
		stPktData.SWAP();

		/* �۽ŵ� ������ ���ۿ� ��� */
		bSucc = m_pSendQue->PushData(&stPktData);
		/* ���� ���� */
		m_syncSend.Leave();
	}

	/* ���� ���� �� �б� ��û */
	return bSucc ? ReqReadSetup() : FALSE;
}

/*
 desc : ������ �Էµ� Trigger ��ġ ���� Trigger Board�κ��� ���ŵ� �Է� �� ��
 parm : cam_id	- [in]  Align Camera Index (0x01 or 0x02)
		index	- [in]  Ʈ���� ���� ��ġ (0x000 ~ 0x0f)
		pos		- [in]  Ʈ���� �� (���Ϸ��� �Էµ� Ʈ���� ��ġ ��)
 retn : TRUE or FALSE
*/
BOOL CTrigThread::IsTrigPosEqual(UINT8 cam_id, UINT8 index, UINT32 pos)
{
	/* �� Align Camera�� ���� ä�� ��ȣ ��� */
	UINT8 u8ChNo	= GetTrigChNo(cam_id);
	if (!u8ChNo)	return FALSE;
	u8ChNo--;

	return (m_pstShMemTrig->trig_set[u8ChNo].area_trig_pos[index] == pos);
}

/*
 desc : ���� ���� ���� ����
 parm : flag	- [in]  0x00: ���� ���� ����, 0x01: ����� ����
 retn : None
*/
VOID CTrigThread::UpdateLinkTime(UINT8 flag)
{
	BOOL bUpate	= FALSE;

	/* ���� ���� ���� �����̰�, ������ ����� ���¶�� */
	if (!flag && m_pstShMemLink->is_connected)		bUpate	= TRUE;
	/* ���� ���� �����̰�, ������ ������ ���¶�� */
	else if (flag && !m_pstShMemLink->is_connected)	bUpate	= TRUE;

	/* ���� ���¿� �ٸ� �����, ���� ���� */
	if (bUpate)
	{
		SYSTEMTIME stLinkTime;
		GetLocalTime(&stLinkTime);
		m_pstShMemLink->is_connected	= flag;
		m_pstShMemLink->link_time[0]	= stLinkTime.wYear;
		m_pstShMemLink->link_time[1]	= stLinkTime.wMonth;
		m_pstShMemLink->link_time[2]	= stLinkTime.wDay;
		m_pstShMemLink->link_time[3]	= stLinkTime.wHour;
		m_pstShMemLink->link_time[4]	= stLinkTime.wMinute;
		m_pstShMemLink->link_time[5]	= stLinkTime.wSecond;
		m_pstShMemLink->last_sock_error	= GetSockLastError();
	}
}

/*
 desc : �۽ŵ� ��ɾ ����� ������ ���� ��ȯ
 parm : None
 retn : ���� (ť)�� ����� ����
*/
UINT16 CTrigThread::GetQueCount()
{
	return m_pSendQue->GetQueCount();
}