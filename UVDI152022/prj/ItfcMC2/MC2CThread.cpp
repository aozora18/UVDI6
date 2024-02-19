
/* desc : Luria Server Thread ��, CMPS: Client, Luria: Server */

#include "pch.h"
#include "MC2CThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

/*
 desc : ������
 parm : config		- [in]  ȯ�� ���� ����
		shmem		- [in]  MC2 Service ���� ���� �޸�
		source_ip	- [in]  Local IPv4
		target_ip	- [in]  Remote IPv4
		serial_no	- [in]  Serial number of the device or 0
		mc2_port	- [in]  TCP/IP Port (MC2�� �����͸� �۽��ϱ� ���� ��Ʈ)
		cmps_port	- [in]  MC2�κ��� CMPS�� ����ޱ� ���� ��� ��Ʈ
		port_buff	- [in]  TCP/IP Port Buffer Size
		recv_buff	- [in]  Send or Recv�� ��Ŷ �� ���� ū ��Ŷ�� ũ�� (����: Bytes)
		ring_buff	- [in]	Max Recv Packet Size
		idle_time	- [in]  �� �ð�(����:�и���) ���� MC2�� ��� �۾�(�۽� or ����)�� ���� ���
							Client�� ���� ��� ������ �ݰ� ������ ����
							�� ���� 0 �� ���, �� ����� ������� �ʴ´�.
		sock_time	- [in]  ���� �̺�Ʈ (��/����) ���� ��� �ð� (����: �и���)
							�ش� �־��� ��(�ð�)��ŭ�� ���� �̺�Ʈ ������ ���� �ܰ�� ��ƾ �̵�
 retn : None
*/
CMC2CThread::CMC2CThread(LPG_CMSI config,
						 LPG_MDSM shmem,
						 UINT16 mc2_port,
						 UINT16 cmps_port,
						 UINT32 source_ip,
						 UINT32 target_ip,
						 UINT32 serial_no,
						 UINT32 port_buff,
						 UINT32 recv_buff,
						 UINT32 ring_buff,
						 UINT32 idle_time,
						 UINT32 sock_time)
	: CMC2Thread(shmem,
				 mc2_port,
				 cmps_port,
				 source_ip,
				 target_ip,
				 port_buff,
				 recv_buff,
				 ring_buff,
				 sock_time)
{
	m_pstConfig		= config;
	m_u16ReqIndex	= 0x0000;
	m_bSendPeriodPkt= TRUE;
	m_u32SerialNo	= serial_no;
	m_u32IdleTime	= idle_time;
	m_u64PeriodTime	= 250;	/* �⺻ ������ ������ �ֱ������� ��û */
	m_u64RecvTime	= 0;	/* �ݵ�� 0 ������ �ʱ�ȭ */
	m_u64SendTime	= 0;	/* �ݵ�� ���� �ð����� ���� */
	m_bConnected	= FALSE;
	m_bInitSocket	= FALSE;
	m_pPktReadBuff	= NULL;
	m_pPktReqWrite	= NULL;
	m_pstReqHead	= NULL;
	m_pstReqRead	= NULL;
	/* Check whether it operates in demo mode */
	if (GetConfig()->IsRunDemo())	m_enSysID = ENG_EDIC::en_demo;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CMC2CThread::~CMC2CThread()
{
	/* ��Ŷ ���� �޸� ���� */
	delete [] m_pPktReadBuff;
	delete [] m_pPktReqWrite;

	/* ���� �۽� ��Ŷ �޸� ���� */
	m_lstSendPkt.RemoveAll();
}

/*
 desc : ������ �ʱ� �����, ���� �ѹ� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::StartWork()
{
	/* ��Ŷ �۽� ���� �Ҵ� */
	m_pPktReadBuff	= new UINT8 [PKT_SIZE_READ_OBJECT+1];	/* fixed : 26 bytes */
	memset(m_pPktReadBuff, 0x00, PKT_SIZE_READ_OBJECT+1);
	m_pPktReqWrite	= new UINT8 [512];	/* SD2S�� 8 �� ����̺꿡 ���� ��� ������ �о���µ� ��� */
	memset(m_pPktReqWrite, 0x00, 512);

	/* ��Ŷ �۽� ���ۿ� ����ü ������ ���� */
	m_pstReqHead	= (LPG_PUCH)m_pPktReadBuff;
	m_pstReqRead	= (LPG_PQRD)(m_pPktReadBuff + 10);

	/* UDP Header �⺻ �� ���� */
	m_pstReqHead->return_port	= m_u16CMPSPort;					/* UDP target port of the client */
	m_pstReqHead->udp_msg_type	= (UINT8)ENG_MUMT::en_gen3_dnc;		/* read request -> fixed */
	m_pstReqHead->short_cycle	= 0;								/* 0 - Long Cycle, 1 = Short cycle ���� ���� (?) */
	m_pstReqHead->u16_reserved	= 0;
	m_pstReqHead->send_cmd_cnt	= 0;								/* �������� �۽��� ��� Ƚ�� or ������ (Client�� Server�� ���� ��ɾ��̸�, ���� ���� ��, �ĺ��ϱ� ����) */
	/* Reading Object */
	m_pstReqRead->e_gen3_dnc_cmd= (UINT8)ENG_DCG3::en_req_read_obj;	/* read object value index -> fixed */
	m_pstReqRead->serial_no		= m_u32SerialNo;					/* MC2 Serial Number */

	return CMC2Thread::StartWork();
}

/*
 desc : �ֱ������� ȣ���
 parm : None
 retn : None
*/
VOID CMC2CThread::RunWork()
{
	BOOL bIsConnect	= FALSE;
	UINT64 u64Tick	= GetTickCount64();

	/* ���� ���� */
	if (m_syncPeriodPkt.Enter())
	{
		/* MC�� ����(?) �� ���� �ֱٿ� �۽��� �ð��� ���ŵ� �ð� ������ ũ��, ��� ���������� �� */
		/* ��, �۽ŵ� �ð� ���� 5�� �̳��� ���ŵ� �ð��� ���ٸ�, ��� ���������� �� */
		if (!m_bConnected)
		{
			if (m_u32IdleTime && (m_u64RecvTime + m_u32IdleTime < u64Tick))	bIsConnect	= TRUE;
			else if (u64Tick > m_u64RecvTime + 10000 /* 10 �� */)			bIsConnect	= TRUE;
			if (bIsConnect)
			{
				/* ------------------------------------------------- */
				/* ���� �۾��� ���� Client ������ �ʱ�ȭ �ߴٰ� �˸� */
				/* ------------------------------------------------- */
				m_bInitSocket	= InitSocket(u64Tick);	/* ������ �ʱ�ȭ �Ǿ��ٰ� ���� */
			}
		}
		/* --------------------- */
		/* Socket Event ����ó�� */
		/* --------------------- */
 		if (0x02 == PopSockEvent())
 		{
 			TCHAR tzMesg[128]	= {NULL};
			CUniToChar csCnv1, csCnv2;
 			swprintf_s(tzMesg, 128,
					   L"The remote system has been disconnected (src:%s tgt:%s / err_cd:0x%x)",
					   m_tzSourceIPv4, m_tzTargetIPv4, m_i32SockError);
 			if (!IsRunDemo())	LOG_ERROR(m_enSysID, tzMesg);
 		}

		/* �ֱ������� �۽ŵ� ��Ŷ�� �ִ��� ���� Ȯ�� */
		if (m_bInitSocket)	SendPeriodPkt(u64Tick);

		/* ���� ���� */
		m_syncPeriodPkt.Leave();
	}
}

/*
 desc : �ֱ������� ��Ŷ �۽��ϴ� ���
 parm : tick	- [in]  ���� CPU �ð�
 retn : None
 note : ��Ŷ �۽ſ� ���� �ּ� ���� �ð��� 100 ms , ������ ������ 150 ms ��
*/
VOID CMC2CThread::SendPeriodPkt(UINT64 tick)
{
	BOOL bSended	= FALSE;

	/* ���� ���� */
	if (m_syncPktSend.Enter())
	{
		/* ------------------------------------------------------------------- */
		/* �ſ� �߿�. 200 msec (�ʴ� 5��  �̻�)���Ϸ� ������ ��û�ϸ� MC2 ���� */
		/* ------------------------------------------------------------------- */
		/* �ֱٿ� ���ŵ� �ð��� Period Time ���� ũ�ٸ�, ������ ���� ��Ŷ �۽� */
		/* �ֱٿ� �۽ŵ� ��Ŷ�� ������ �ּ� 200ms �̳��� ���ٸ� ���� ��Ŷ �۽� */
		/* ------------------------------------------------------------------- */
		/* ~���� �߿�~ MC2�� ������ ������ ���� ���� ���� ��Ŷ�� �۽��ؾ� �� */
		/* ������ Ȯ������ �ʰ�, ���� ����� �۽��ϸ�, ���� �۽� ����� ���õ� */
		/* ------------------------------------------------------------------- */
		if (m_u64SendTime+m_u64PeriodTime < tick)
		{
			/* ���� �ֱٿ� �۽��� �ð� ���� */
			m_u64SendTime	= tick;
			/* ���� ��ϵ� ��Ŷ���� �۽� */
			if (m_lstSendPkt.GetCount() > 0)
			{
				/* ��Ŷ �������� */
				STM_PSDS stPktData	= m_lstSendPkt.GetHead();
				bSended = ReSended(stPktData.data, stPktData.size, 3, 100);
				if (bSended && stPktData.mode && GetConfig()->set_comn.comm_log_mc2)
				{
					TCHAR tzMesg[128]	= {NULL};
					PUINT8 pPktData		= stPktData.data;
					LPG_PUCH pstHead	= (LPG_PUCH)pPktData;	pPktData	+= sizeof(STG_PUCH);
					LPG_PQWD pstWrite	= (LPG_PQWD)pPktData;
					swprintf_s(tzMesg, 128,
							   L"UVDI15 vs. MC2 [Send] [called_func=SendPeriodPkt]"
							   L"[send_cmd_cnt=0x%08x,e_gen3_dnc_cmd=0x%02x][sub_id=0x%02x][data_len=%d]",
							   pstHead->send_cmd_cnt, pstWrite->e_gen3_dnc_cmd, pstWrite->sub_id, pstWrite->data_len);
					LOG_MESG(m_enSysID, tzMesg);
				}
				/* �۽ŵ� ��Ŷ �޸� ���� */
				m_lstSendPkt.RemoveHead();
			}
			else
			{
				/* �ֱ����� ��Ŷ ������ �۽� */
				ReadReqPacket();
			}
		}
		/* �������� */
		m_syncPktSend.Leave();
	}
}

/*
 desc : Release tcp/ip socket
 parm : sd		- [in]  Socket descriptor
		time_out- [in]  Please refer to the LINGER structure (= linger time)
						If the value is 0, I disconnected immediately without any delay
 retn : None
*/
VOID CMC2CThread::CloseSocket(SOCKET sd, UINT32 time_out)
{
	/* --------------------------------------------------------- */
	/* �θ� ���� CloseSocket �Լ� ȣ���ϱ� ���� ������ ���� ���� */
	/* --------------------------------------------------------- */
	m_bConnected	= FALSE;
	/* ���� ���� ���� */
	UpdateLinkTime(0x00);

	// ���� �θ� �Լ� ȣ��
	CMC2Thread::CloseSocket(sd, time_out);
	// Socket ������ ���� �ʱ�ȭ ��� �ٸ��� ó��
	if (sd == m_sdClient)	m_sdClient	= INVALID_SOCKET;
}

/*
 desc : Init Socket
 parm : tick	- [in]  CPU ���� �ð�
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::InitSocket(UINT64 tick)
{
	INT32 i32Option		= 1;
	TCHAR tzMesg[128]	= {NULL};
	LONG32 l32Event		= FD_READ;
	SOCKADDR_IN stAddrIn= {NULL};

	/* ������ �ʱ�ȭ ���� �ʾҴٰ� ���� */
	m_bInitSocket	= FALSE;
	// Remove existing client socket exists
	if (INVALID_SOCKET != m_sdClient)
	{
		CloseSocket(m_sdClient);
	}

	// ������ �õ��ϹǷ� �÷��� ����
	m_enSocket		= ENG_TPCS::en_linking;
	m_u32AddrSize	= sizeof(SOCKADDR_IN);
	memset(&m_stSrvAddr, 0x00, m_u32AddrSize);

	// Create a client socket
	m_sdClient = socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == m_sdClient)	return FALSE;

	/* ----------------------- */
	/* Setup the server socket */
	/* ----------------------- */
	stAddrIn.sin_family			= AF_INET;
	stAddrIn.sin_port			= htons(m_u16CMPSPort);
	InetPtonW(AF_INET, m_tzSourceIPv4, &stAddrIn.sin_addr.s_addr);

	/* ----------------------- */
	/* Setup the client socket */
	/* ----------------------- */
	m_stSrvAddr.sin_family		= AF_INET;
	m_stSrvAddr.sin_port		= htons(m_u16MC2Port);	// In this case, the port number is a value of 0 at all times
	InetPtonW(AF_INET, m_tzTargetIPv4, &m_stSrvAddr.sin_addr.s_addr);

	/* Setup socketoption - reuse IPv4 */
	setsockopt(m_sdClient, SOL_SOCKET, SO_REUSEADDR, (char *)&i32Option, sizeof(i32Option));
	/* Socket binding ... */
	if (SOCKET_ERROR == ::bind(m_sdClient, (LPSOCKADDR)&stAddrIn, sizeof(SOCKADDR_IN)))
	{
		m_i32SockError = WSAGetLastError();
		swprintf_s(tzMesg, 128,
				   L"Socket binding error : 0x%x (Source IPv4=%s)", m_i32SockError, m_tzSourceIPv4);
		LOG_ERROR(m_enSysID, tzMesg);
		return FALSE;
	}

	// Setup socket event (FD_READ)
	if (SOCKET_ERROR == WSAEventSelect(m_sdClient, m_wsaClient, l32Event))	return FALSE;
	swprintf_s(tzMesg, 128,
			   L"Connected to UDP Port (src:%s tgt:%s)", m_tzSourceIPv4, m_tzTargetIPv4);
	LOG_MESG(m_enSysID, tzMesg);


	/* ----------------------------------------- */
	/* !!! Important !!! Send / Recv Time �ʱ�ȭ */
	/* ----------------------------------------- */
	m_u64RecvTime	= tick;	/* ���� CPU �ð����� ���� (��-������ ���� ����) */
	m_u64SendTime	= 0;	/* ���� �ʱ�ȭ ��, �ٷ� �۽��ϱ� ���� 0 ������ �ʱ�ȭ */

	return TRUE;
}

/*
 desc : ���� �̺�Ʈ ���� ���
 parm : None
 retn : 0x00 - ����, 0x01 - ����, 0x02 - ����, 0x03 - ������ ����
*/
UINT8 CMC2CThread::PopSockEvent()
{
	UINT8 u8RetCode	= 0x00;
	INT32 i32WsaRet	= 0;
	UINT32 u32WsaRet= 0;
	WSANETWORKEVENTS wsaNet;

	/* ----------------------------------------------------------------------------------------- */
	/* �̺�Ʈ ��ü�� ��ȣ ���°� �� ������ ����Ѵ�. WSAWaitForMultipleEvents() �Լ��� ��ȯ ���� */
	/* ��ȣ ���°� �� �̺�Ʈ ��ü�� �迭 �ε��� + WSA_WAIT_EVENT_0 ���̴�. �׷��Ƿ�, ���� �ε��� */
	/* ���� �������� WSA_WAIT_EVENT_0 (WAIT_BOJECT_0) ���� ���� �Ѵ�.                            */
	/* ----------------------------------------------------------------------------------------- */
	u32WsaRet	= WSAWaitForMultipleEvents(1,				// ���� �̺�Ʈ ���� ����: 1 ��
										   &m_wsaClient,	// ���� �̺�Ʈ �ڵ�
										   FALSE,			// TRUE : ��� ���� �̺�Ʈ ��ü�� ��ȣ ����, FALSE : 1�� �̺�Ʈ ��ü�� ��ȣ �����̸� ��� ����
										   m_u32SockTime,	// ���� �ð� (�и���) �̳��� ������ ������ ����
										   FALSE);			// WSAEventSelect������ �׻� FALSE
#if 1
	if (u32WsaRet == WSA_WAIT_TIMEOUT || u32WsaRet == WSA_WAIT_FAILED)
#else
	if (u32WsaRet == WSA_WAIT_TIMEOUT)
#endif
	{
#if 1
		if (u32WsaRet == WSA_WAIT_FAILED)
		{
			TCHAR tzMesg[256]	= {NULL};
			swprintf_s(tzMesg, 256, L"The return value of the WSAWaitForMultipleEvents failed. "
									L"(time out) (tgt_ipv4=%s)", m_tzTargetIPv4);
			if (m_enSysID != ENG_EDIC::en_demo)	LOG_MESG(m_enSysID, tzMesg);
		}
#endif
		SetCycleTime(NORMAL_THREAD_SPEED);
		return 0x00;
	}
	// ��ü���� Network Event �˾Ƴ���
	u32WsaRet	-= WSA_WAIT_EVENT_0;
	i32WsaRet	= WSAEnumNetworkEvents(m_sdClient,			// ��� ����
									   m_wsaClient,			// ��� ���ϰ� ¦�� �̷� ��ü �ڵ�
									   &wsaNet);			// �̺�Ʈ ������ ����� ����ü ������
	// ������ �������� Ȯ��
	if (i32WsaRet == SOCKET_ERROR)
	{
		SetCycleTime(NORMAL_THREAD_SPEED);
		return 0x00;
	}

	/* ----------------------------------------------------------------- */
	/* ���� ������ ���� �ӵ��� ���̱� ���� ó�� (������ ���Ÿ���� ���) */
	/* ----------------------------------------------------------------- */
	SetCycleTime(0);
	/* ------------------------------ */
	/* Socket Event �� �и� ó�� ���� */
	/* ------------------------------ */
	if (FD_READ == (FD_READ & wsaNet.lNetworkEvents))
	{
		/* ���� ���� */
		if (m_syncPktRecv.Enter())
		{
			// ��� ��Ʈ�κ��� ������ �б� �õ�
			if (wsaNet.iErrorCode[FD_READ_BIT] == 0)
			{
				if (Received(m_sdClient))
				{
					u8RetCode = 0x03;
					/* !!! ���ŵ� �ð� ���� !!! */
					m_u64RecvTime	= GetTickCount64();
					/* ���ŵ� �����Ͱ� ������, ����� ������ ���� */
					m_bConnected	= TRUE;
					/* ���� �Ϸ� ���� */
					UpdateLinkTime(0x01);
				}
			}
			/* ���� ���� */
			m_syncPktRecv.Leave();
		}
	}
	// ������ ������ ���� Ȥ�� ��Ÿ �̺�Ʈ (Send or Connected)�� ���, ���� �������� ó��
#if 1
	else if (FD_CLOSE == (FD_CLOSE & wsaNet.lNetworkEvents))
#else
	else
#endif
	{
		CloseSocket(m_sdClient);
		u8RetCode	= 0x02;
	}

	return u8RetCode;
}

/*
 desc : ���� ������ �۽�
 parm : data	- [in]  ������ �����Ͱ� ����� ���� ������
		size	- [in]  'data' ���ۿ� ����� �������� ũ��
		mode	- [in]  �ֱ������� ��û�ϴ� ��� (0x00), �ܺ� (����)���� ��û�� ��� (0x01)
 retn : TRUE - �۽� ����(��� �۽ŵ� �����Ͱ� ������),
		FALSE - �۽� ����(���� ����... ������ �����ؾ� ��)
*/
BOOL CMC2CThread::AddPktData(PUINT8 data, UINT32 size, UINT8 mode)
{
	BOOL bSucc			= FALSE;
	STM_PSDS stPktData	= {NULL};

	/* ���� ���� */
	if (m_syncPktSend.Enter())
	{
		if (m_lstSendPkt.GetCount() > MAX_MC2_PACKET_COUNT)
		{
			/* �� ���� ��ϵ� ��Ŷ ���� */
			m_lstSendPkt.RemoveHead();
			LOG_WARN(m_enSysID,
					 L"Older packets were removed because the number of packets was exceeded");
		}
		if (size < MAX_MC2_PACKET_SIZE)
		{
			/* ���ο� ��Ŷ ��� */
			memcpy(stPktData.data, data, size);
			stPktData.size	= size;
			stPktData.mode	= mode;
			m_lstSendPkt.AddTail(stPktData);

			/* ���� ��Ŷ �۽��� ���� ��ɾ� Ƚ�� �� ���� ó�� : Server�� Client ���� ��/���� �ĺ� ���� */
			m_pstReqHead->NextCmdCount();

			bSucc	= TRUE;
		}
		/* ���� ���� */
		m_syncPktSend.Leave();
	}

	return bSucc;
}

/*
 desc : �ֱ������� ȣ��Ǵ� �Լ�
 parm : None
 retn : None
*/
VOID CMC2CThread::ReadReqPacket()
{
	/* �ֱ������� MC2�� ���� ���� ��û */
	switch (m_u16ReqIndex)
	{
	/* ------------------------------------------------------------- */
	/* PDOs (Process Data Objects) Reference Value Collective Object */
	/* ------------------------------------------------------------- */
	case 0x0000	:	ReadObjectValueByIndex(0, MAX_WANTED_READ_SIZE, 0, (UINT32)ENG_PSOI::en_ref_value_collective);	break;
	case 0x0001	:	ReadObjectValueByIndex(0, MAX_WANTED_READ_SIZE, 0, (UINT32)ENG_PSOI::en_act_value_collective);	break;
	}

	/* ��Ŷ ������ �۽� */
	if (AddPktData(m_pPktReadBuff, PKT_SIZE_READ_OBJECT, 0x00))
	{
	}
	/* ���� ��ɾ� ��û�� ���� �ε��� ���� ó�� */
	if (++m_u16ReqIndex == 2)	m_u16ReqIndex	= 0x0000;
}

/*
 desc : E_G3�� �б� ��ɾ� ��û
		Reads the value of an object.
		The object is selected per object index.
		The response always includes the max. possible number of data bytes.
		The length of the response is limited by:
			-. the size of the object
			-. the length of user data of a UDP telegram of 1280 bytes
			-. the definition of the parameter <SizeWanted>.
 parm : sub_id		- [in]  SubID of the SD3 device (Drive ID)
		read_size	- [in]  Number of bytes to be read / 0 = all available
							This parameter limits the length of user data of a UDP telegram of 1280 bytes.
							If, beginning from the SubIn-dex, more data are included 
							in the object than defined in the parameter <SizeWanted>,
							only the data bytes defined in SizeWanted are read.
							If the value 0 or a value greater than 1280 is defined for SizeWanted,
							this limitation will be inactive and max. 1280 bytes will be read per read command.
		sub_index	- [in]  Reading shall be started at this SubIndex (Offset)
		index		- [in]  Index of the object to be read (Object ID)
 retn : None
*/
VOID CMC2CThread::ReadObjectValueByIndex(UINT8 sub_id,
										 UINT16 read_size,
										 UINT32 sub_index,
										 UINT32 index)
{
	STG_MPSD stPktSend			= {NULL};
	/* �б� ���� ä��� */
	m_pstReqRead->sub_id		= sub_id;
	m_pstReqRead->size_wanted	= read_size;
	m_pstReqRead->sub_index		= sub_index;
	m_pstReqRead->index			= index;

	/* �̸� �۽ŵ� ��Ŷ ���� ���� */
	stPktSend.cmd				= m_pstReqRead->e_gen3_dnc_cmd;
	stPktSend.obj_index			= m_pstReqRead->index;
	stPktSend.sub_id			= m_pstReqRead->sub_id;
	stPktSend.sub_index			= m_pstReqRead->sub_index;
	/* �޸� �ʿ� ��� */
	memcpy(&m_pstPktSend[m_pstReqHead->send_cmd_cnt], &stPktSend, sizeof(STG_MPSD));
}

/*
 desc : ���� ��ɾ� �޸� �ʿ� ��� (Object 0x3801, PDO Reference Value Collective Object)
 parm : sub_id		- [in]  SubID of the SD3 device (Drive ID)
		sub_index	- [in]  Writing shall be started at this SubIndex (Offset)
		index		- [in]  Index of the object to be read (Object ID)
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::WriteObjectValueByIndex(UINT8 sub_id, UINT32 sub_index, UINT32 index)
{
	STG_MPSD stPktSend	= {NULL};
#if 0
	/* ���� MC2 State Bit �� ���� Busy �����̰ų� Busy ���°� �ƴ� ���, Ư�� �ð� ���� ��� �� ó�� */
	if (!m_pObjAct->GetDrive(sub_id)->IsStopped())	return FALSE;
	/* Done Toggled Flag ���� �ٲ� ������ ���� �ð� ��� (0.5 �� ���) */
	CMC2Thread::Sleep(500);
#endif
	/* �̸� �۽ŵ� ��Ŷ ���� ���� */
	stPktSend.cmd		= (UINT8)ENG_DCG3::en_req_write_obj;
	stPktSend.obj_index	= index;
	stPktSend.sub_id	= sub_id;
	stPktSend.sub_index	= sub_index;
	/* �޸� �ʿ� ��� */
	memcpy(&m_pstPktSend[m_pstReqHead->send_cmd_cnt], &stPktSend, sizeof(STG_MPSD));

	return TRUE;
}

/*
 desc : Command E_G3_WriteObjectValueByIndex (Object 0x3801, PDO Reference Value Collective Object)
		PDOs (Process Data Objects) : PDO Reference Value Collective Object (address. 0x3801)
 parm : drv_head	- [in]  �� 8���� ����̺꿡 ���������� ����Ǵ� Header ����
		drv_data	- [in]  �� 8���� ������ Drive Data (Control/Reserved/Parameter-1~3)
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::WriteObjectValueByIndex(LPG_RVCH drv_head, LPG_RVCD drv_data)
{
	BOOL bSucc			= FALSE;
	UINT32 u32Size		= 0;
	LPG_PQWD pstWrite	= NULL;
	PUINT8 pData	= NULL;	/* STG_RVCH + STG_RVCD (8 set) */

	/* ���� ���� */
	if (m_syncPktSend.Enter())
	{
		/* ��ü �۽� ��Ŷ ũ�� ���ϱ� */
		u32Size		= sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8);
		u32Size		+= sizeof(STG_RVCH) + sizeof(STG_RVCD) * MAX_MC2_DRIVE;

		m_pPktReqWrite[u32Size]	= 0x00;
		/* UDP ��� ���� ���� */
		memcpy(m_pPktReqWrite, m_pstReqHead, sizeof(STG_PUCH));
		/* ���� �� ������ ���� */
		pstWrite	= (LPG_PQWD)(m_pPktReqWrite + sizeof(STG_PUCH));
		pData		= m_pPktReqWrite + sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8);

		/* �⺻ �۽� ��� ���� */
		pstWrite->e_gen3_dnc_cmd= (UINT8)ENG_DCG3::en_req_write_obj;
		pstWrite->sub_id		= 0x00/* fixed. SubID of the SD2 device */;
		/* ������ ��Ŷ ���� */
		pstWrite->serial_no		= m_u32SerialNo;
		pstWrite->index			= (UINT32)ENG_PSOI::en_ref_value_collective;
		pstWrite->sub_index		= 0x00000000;	/* Offset */
		pstWrite->data_len		= (UINT16)(sizeof(STG_RVCH) + sizeof(STG_RVCD) * MAX_MC2_DRIVE);
		/* �ִ� 8���� Reference Drive Data ���� */
		memcpy(pData, drv_head, sizeof(STG_RVCH));
		pData	+= (UINT32)sizeof(STG_RVCH);
		memcpy(pData, drv_data, sizeof(STG_RVCD) * MAX_MC2_DRIVE);

		/* �޸� �㿡 ��� */
		bSucc	= WriteObjectValueByIndex(0x00, 0x0000, pstWrite->index);
		if (bSucc)
		{
			/* MC2 ��Ŷ �۽� */
			bSucc	= AddPktData(m_pPktReqWrite, u32Size, 0x01);
		}

		/* ���� ���� */
		m_syncPktSend.Leave();
	}

	return bSucc;
}

/*
 desc : ���(Step) ��ġ�� �̵� (Up, Down, Left, Bottom) ��ġ �� ��ȯ
 parm : drv_id	- [in]  Motor Drive ID (SD2 Device ID) ��, 0x00, 0x01, 0x04, 0x05
		direct	- [in]  �̵� ���� (Up, Down, Left, Right)
		dist	- [in]  �̵� �Ÿ� (����: 0.1 um or 100 nm)
 retn : ���� ��ġ �� ��ȯ (����: 0.1 um or 100 nm)
*/
INT32 CMC2CThread::GetDevStepMovePos(ENG_MMDI drv_id, ENG_MDMD direct, INT32 dist)
{
	INT32 i32MaxPos	= (INT32)ROUNDED(m_pstConfig->max_dist[(UINT8)drv_id] * 10000.0f, 0);
	INT32 i32MinPos = (INT32)ROUNDED(m_pstConfig->min_dist[(UINT8)drv_id] * 10000.0f, 0);
	INT32 i32ActPos	= m_pstShMemMC2->act_data[(UINT8)drv_id].real_position;
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15_LLS10 ||\
	DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15_LLS06)
	switch (direct)
	{
	/* ���� or ���� �̵� */
	case ENG_MDMD::en_move_down	:
	case ENG_MDMD::en_move_left	:	if (i32ActPos + dist > i32MaxPos)	i32ActPos	= i32MaxPos;
									else								i32ActPos	-= dist;		break;
	/* ���� or ���� �̵� */
	case ENG_MDMD::en_move_up	:	if (i32ActPos - dist < 0)			i32ActPos	= i32MinPos;
									else								i32ActPos	-= dist;		break;
	case ENG_MDMD::en_move_right:	if (i32ActPos - dist < i32MinPos)	i32ActPos	= i32MinPos;
									else								i32ActPos	+= dist;		break;
	}
#else
	AfxMessageBox(L"It is not currently supported (CMC2CThread::GetDevStepMovePos Function)", MB_ICONSTOP|MB_TOPMOST);
#endif
	return i32ActPos;
}

/*
 desc : Ư��(Step) ��ġ�� �̵� (Up, Down, Left, Bottom) ��ġ �� ��ȯ
 parm : drv_id	- [in]  Motor Drive ID (SD2 Device ID) ��, 0x00, 0x01, 0x04, 0x05
		pos		- [in]  ���� ��ġ (����: 0.1 um or 100 nm)
 retn : ���� ��ġ �� ��ȯ (����: 0.1 um or 100 nm)
*/
INT32 CMC2CThread::GetDevAbsMovePos(ENG_MMDI drv_id, INT32 pos)
{
	INT32 i32MaxPos	= 0;
	INT32 i32ActPos	= pos;

	i32MaxPos	= (INT32)ROUNDED(m_pstConfig->max_dist[(UINT8)drv_id] * 10000.0f, 0);
#if 0
	if (drv_id == ENG_MMDI::en_align_cam1 || drv_id == ENG_MMDI::en_align_cam2)
	{
		i32ActPos	+= (INT32)ROUNDED(m_pstConfig->min_dist[(UINT8)drv_id] * 10000.0f, 0);
	}
#endif
	/* ���� �̵��� �Ÿ��� �̵� */
	if (i32ActPos > i32MaxPos)	i32ActPos = i32MaxPos;

	return i32ActPos;
}

/*
 desc : ����(Jog) ��ġ�� �̵� (Up, Down, Left, Bottom) ��ġ �� ��ȯ
 parm : drv_id	- [in]  Motor Drive ID (SD2 Device ID) ��, 0x00, 0x01, 0x04, 0x05
		direct	- [in]  �̵� ���� (Up, Down, Left, Right)
 retn : ���� ��ġ �� ��ȯ (����: 0.1 um or 100 nm)
*/
INT32 CMC2CThread::GetDevJogMovePos(ENG_MMDI drv_id, ENG_MDMD direct)
{
	INT32 i32ActPos = 0;
	switch (direct)
	{
	/* ���� or ���� �̵� */
	case ENG_MDMD::en_move_down	:
	case ENG_MDMD::en_move_left	:	i32ActPos	= (INT32)ROUNDED(m_pstConfig->max_dist[(UINT8)drv_id] * 10000.0f, 0);		break;
	/* ���� or ���� �̵� */
	case ENG_MDMD::en_move_up	:
	case ENG_MDMD::en_move_right:	i32ActPos	= (INT32)ROUNDED(m_pstConfig->min_dist[(UINT8)drv_id] * 10000.0f, 0);		break;
	}

	return i32ActPos;
}

/*
 desc : �������� ��� �̵� (Up, Down, Left, Bottom) - Reference Drive
 parm : method	- [in]  ENG_MMMM (Step or Jog)
		drv_id	- [in]  Motor Drive ID (SD2 Device ID) ��, 0x00, 0x01, 0x04, 0x05
		direct	- [in]  �̵� ���� (Up, Down, Left, Right)
		dist	- [in]  �̵� �Ÿ� (����: 0.1 um or 100 nm)
		velo	- [in]  �̵� �ӵ� (����: 0.1 um or 100 nm / sec)
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::WriteDevMoveRef(ENG_MMMM method, ENG_MMDI drv_id, ENG_MDMD direct,
								  INT32 dist, INT32 velo)
{
	BOOL bSucc			= FALSE;
	INT32 i32ActPos		= 0;
	UINT32 u32Size		= sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8) + sizeof(STG_RVCD);
	LPG_PQWD pstWrite	= NULL;
	LPG_RVCD pstData	= NULL;

	/* ���� ���� */
	if (m_syncPktSend.Enter())
	{
		m_pPktReqWrite[u32Size]	= 0x00;
		/* UDP ��� ���� ���� */
		memcpy(m_pPktReqWrite, m_pstReqHead, sizeof(STG_PUCH));
		/* ���� �� ������ ���� */
		pstWrite	= (LPG_PQWD)(m_pPktReqWrite + sizeof(STG_PUCH));
		pstData		= (LPG_RVCD)(m_pPktReqWrite + sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8));

		/* �⺻ �۽� ��� ���� */
		pstWrite->e_gen3_dnc_cmd= (UINT8)ENG_DCG3::en_req_write_obj;
		pstWrite->sub_id		= (UINT8)drv_id;
#if (ROTATED_CAMERA_IN_PHOTOHEAD)
		switch (direct)
		{
		case ENG_MDMD::en_move_up	:	direct	= ENG_MDMD::en_move_down;	break;
		case ENG_MDMD::en_move_down	:	direct	= ENG_MDMD::en_move_up;		break;
		}
#endif
		/* Ư�� �Ÿ���ŭ �̵����� Jog�� ���� ��ġ���� �̵��ϴ� ������ */
		if (method == ENG_MMMM::en_move_step)
		{
			i32ActPos	= GetDevStepMovePos(drv_id, direct, dist);
		}
		else
		{
			i32ActPos	= GetDevJogMovePos(drv_id, direct);
		}
		/* ������ ��Ŷ ���� */
		pstWrite->serial_no		= m_u32SerialNo;
		pstWrite->index			= (UINT32)ENG_PSOI::en_sdo_ref_drive;
		pstWrite->sub_index		= 0x00000000;		/* Offset */
		pstWrite->data_len		= (UINT16)sizeof(STG_RVCD);
		pstData->ctrl_cmd		= (UINT8)ENG_MCCN::en_go_position;
		pstData->ctrl_cmd_toggle= !m_pstShMemMC2->GetDriveRef((UINT8)drv_id)->ctrl_cmd_toggle;
		pstData->param_0		= 0;			/* ??? */
		pstData->param_1		= i32ActPos;	/* Target Position */
		pstData->param_2		= velo;			/* Moving Speed */
		pstData->param_3		= 0;

		/* �޸� �㿡 ��� */
		if (WriteObjectValueByIndex(UINT8(drv_id), 0x0000, pstWrite->index))
		{
			/* MC2 ��Ŷ �۽� */
			bSucc	= AddPktData(m_pPktReqWrite, u32Size, 0x01);
		}
		/* ���� ���� */
		m_syncPktSend.Leave();
	}

	return bSucc;
}

/*
 desc : �������� ���� �̵� (Up, Down, Left, Bottom) - Reference Drive
 parm : drv_id	- [in]  Motor Drive ID (SD2 Device ID) ��, 0x00, 0x01, 0x04, 0x05
		pos		- [in]  ���������� �̵��� ��ġ (����: 0.1 um or 100 nm)
		velo	- [in]  �̵� �ӵ� (����: 0.1 um or 100 nm / sec)
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::WriteDevMoveAbs(ENG_MMDI drv_id, INT32 pos, UINT32 velo)
{
	BOOL bSucc			= FALSE;
	INT32 i32ActPos		= 0;
	UINT32 u32Size		= sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8) + sizeof(STG_RVCD);
	LPG_PQWD pstWrite	= NULL;
	LPG_RVCD pstData	= NULL;

	/* ���� ���� */
	if (m_syncPktSend.Enter())
	{
		m_pPktReqWrite[u32Size]	= 0x00;
		/* UDP ��� ���� ���� */
		memcpy(m_pPktReqWrite, m_pstReqHead, sizeof(STG_PUCH));
		/* ���� �� ������ ���� */
		pstWrite	= (LPG_PQWD)(m_pPktReqWrite + sizeof(STG_PUCH));
		pstData		= (LPG_RVCD)(m_pPktReqWrite + sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8));

		/* �⺻ �۽� ��� ���� */
		pstWrite->e_gen3_dnc_cmd= (UINT8)ENG_DCG3::en_req_write_obj;
		pstWrite->sub_id		= (UINT8)drv_id;

		/* Ư�� �Ÿ���ŭ �̵����� Jog�� ���� ��ġ���� �̵��ϴ� ������ */
		i32ActPos	= GetDevAbsMovePos(drv_id, pos);
		/* ������ ��Ŷ ���� */
		pstWrite->serial_no		= m_u32SerialNo;
		pstWrite->index			= (UINT32)ENG_PSOI::en_sdo_ref_drive;
		pstWrite->sub_index		= 0x00000000;		/* Offset */
		pstWrite->data_len		= (UINT16)sizeof(STG_RVCD);
		pstData->ctrl_cmd		= (UINT8)ENG_MCCN::en_go_position;
		pstData->ctrl_cmd_toggle= !m_pstShMemMC2->GetDriveRef((UINT8)drv_id)->ctrl_cmd_toggle;
		pstData->param_0		= 0;			/* ??? */
		pstData->param_1		= i32ActPos;	/* Target Position */
		pstData->param_2		= velo;			/* Moving Speed */
		pstData->param_3		= 0;

		/* �޸� �㿡 ��� (Object 0x3801, PDO Reference Value Collective Object)*/
		if (WriteObjectValueByIndex(UINT8(drv_id), 0x0000, pstWrite->index))
		{
			/* MC2 ��Ŷ �۽� */
			bSucc	= AddPktData(m_pPktReqWrite, u32Size, 0x01);
		}
		/* ���� ���� */
		m_syncPktSend.Leave();
	}

	return bSucc;
}

/*
 desc : ���������� Vector �� �̵� - Reference Drive
 parm : m_drv_id	- [in]  Vector �̵� �����̵Ǵ� Master Drive ID ��
		s_drv_id	- [in]  Vector �̵� ����� Slave Driver ID ��
		m_drv_pos	- [in]  ���������� ���� �̵��� 1 �� ���� ��ġ (����: 0.1 um or 100 nm)
		s_drv_pos	- [in]  ���������� ���� �̵��� 2 �� ���� ��ġ (����: 0.1 um or 100 nm)
		velo		- [in]  Vector �̵� �ӵ� �� (����: 0.1 um or 100 nm / sec)
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::WriteStageMoveVec(ENG_MMDI m_drv_id, ENG_MMDI s_drv_id,
									INT32 m_drv_pos, INT32 s_drv_pos, INT32 velo)
{
	BOOL bSucc			= FALSE;
	UINT32 u32Size		= sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8) + sizeof(STG_RVCD);
	LPG_PQWD pstWrite	= NULL;
	LPG_RVCD pstData	= NULL;

	/* ���� ���� */
	if (m_syncPktSend.Enter())
	{
		m_pPktReqWrite[u32Size]	= 0x00;
		/* UDP ��� ���� ���� */
		memcpy(m_pPktReqWrite, m_pstReqHead, sizeof(STG_PUCH));
		/* ���� �� ������ ���� */
		pstWrite	= (LPG_PQWD)(m_pPktReqWrite + sizeof(STG_PUCH));
		pstData		= (LPG_RVCD)(m_pPktReqWrite + sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8));

		/* �⺻ �۽� ��� ���� */
		pstWrite->e_gen3_dnc_cmd= (UINT8)ENG_DCG3::en_req_write_obj;
		pstWrite->sub_id		= UINT8(m_drv_id);
		/* ������ ��Ŷ ���� */
		pstWrite->serial_no		= m_u32SerialNo;
		pstWrite->index			= (UINT32)ENG_PSOI::en_sdo_ref_drive;
		pstWrite->sub_index		= 0x00000000;		/* Offset */
		pstWrite->data_len		= (UINT16)sizeof(STG_RVCD);
		pstData->ctrl_cmd		= (UINT8)ENG_MCCN::en_do_vector;

		/* Master Drive �� �������� Vector �̵��� ��� */
		pstData->ctrl_cmd_toggle= !m_pstShMemMC2->GetDriveRef(UINT8(m_drv_id))->ctrl_cmd_toggle;
		pstData->param_0		= UINT8(s_drv_id);	/* Slave Drive */
		pstData->param_1		= m_drv_pos;		/* Master Target Position */
		pstData->param_2		= s_drv_pos;		/* Slave Target Position */
		pstData->param_3		= velo;

		/* �޸� �㿡 ��� (Object 0x3801, PDO Reference Value Collective Object) */
		bSucc	= WriteObjectValueByIndex(UINT8(m_drv_id), 0x0000, pstWrite->index);
		if (bSucc)
		{
			/* MC2 ��Ŷ �۽� */
			bSucc	= AddPktData(m_pPktReqWrite, u32Size, 0x01);
		}

		/* ���� ���� */
		m_syncPktSend.Leave();
	}

	return bSucc;
}

/*
 desc : Device Control - Reference Drive
 parm : drv_id	- [in] Axis ���� (0, 1, 4, 5)
		cmd		- [in]  ENG_MCCN
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::WriteDevCtrl(ENG_MMDI drv_id, ENG_MCCN cmd)
{
	BOOL bSucc	= TRUE;

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
	if (cmd == ENG_MCCN::en_do_homing)
	{
		if (ENG_MMST(m_pstShMemMC2->ref_head.mc_en_stop) == ENG_MMST::en_stop_yes)	bSucc = FALSE;
	}
	else
	{
		if (ENG_MMST(m_pstShMemMC2->ref_head.mc_en_stop) != ENG_MMST::en_stop_yes)	bSucc = FALSE;
	}
	if (!bSucc)	LOG_WARN(m_enSysID,
						 L"The command was canceled because of an MControl::EN_STOP condition");
#endif

	return bSucc && WriteSdoRefDrive(drv_id, cmd, 0, 0, 0, 0);
}

/*
 desc : ���� �ӵ� ����
 parm : drv_id	- [in]  Axis ���� (0, 1, 4, 5)
		velo	- [in]  �̵� �ӵ� (����: mm/sec)
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::WriteDevGoVelo(ENG_MMDI drv_id, UINT32 velo)
{
	return WriteSdoRefDrive(drv_id, ENG_MCCN::en_go_velocity, 0, INT32(velo), 0, 0);
}

/*
 desc : ���� �⺻ �ӵ� �� ���ӵ� ����
 parm : drv_id	- [in]  Axis ���� (0, 1, 4, 5)
		velo	- [in]  �̵� �ӵ� (����: mm/sec)
		acce	- [in]  �̵� ���ӵ� (����: mm^2)
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::WriteDevBaseVeloAcce(ENG_MMDI drv_id, UINT32 velo, UINT32 acce)
{
	if (!WriteSdoRefDrive(drv_id, ENG_MCCN::en_set_velocity, 0, INT32(velo), 0, 0))		return FALSE;
	if (!WriteSdoRefDrive(drv_id, ENG_MCCN::en_set_acceleration, 0, INT32(acce), 0, 0))	return FALSE;
	return TRUE;
}

/*
 desc : Luria Expose Mode�� ����
 parm : drv_id	- [in]  �뱤 ������ �� Drive ID
		mode	- [in]  Area (Align) mode or Expose mode
						Area Mode		: ���� ���������� �������� ����
						Expo Mode		: ���� ��ġ ������ Min ���� ������, �������� ������, �������� Min ������ �̵� ��
		pixel	- [in]  Pixel Resolution (5.4 or 10.8 or 21.6 um) (unit: pm; picometer = 1/1000 nm)
						<LLS 10>
						Scroll mode 1	:  5.4310344828	um
						Scroll mode 2	: 10.8620689655	um
						Scroll mode 3	: 16.2931034483	um
						Scroll mode 4	: 21.724137931	um
						<LLS 25>
						Scroll mode 1	: 10.8620689655	um
						Scroll mode 2	: 21.724137931	um
						Scroll mode 3	: 32.5862068966	um
						Scroll mode 4	: 43.4482758621	um
		begin	- [in]  �뱤 ���� ��ġ (����: 0.1 um or 100 nm)
		end		- [in]  �뱤 ���� ��ġ (����: 0.1 um or 100 nm)
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::WriteDevLuriaMode(ENG_MMDI drv_id, ENG_MTAE mode,
									UINT64 pixel, INT32 begin, INT32 end)
{
	ENG_MCCN enCmd[2]	= { ENG_MCCN::en_set_trig_mode_v1, ENG_MCCN::en_set_trig_mode_v2 };
	/* ù ��° �Ķ���Ϳ� �� �� ���� : �߿� */
	UINT8 u8Param0	= ENG_MTAE::en_expo == mode ? 0x00 : 0x02;
	if (ENG_MTAE::en_area == mode)
	{
		begin = 17;
		end = 0;
	}
	return WriteSdoRefDrive(drv_id, ENG_MCCN::en_set_trig_mode_v1,
							u8Param0, INT32(pixel), begin, end);
}

/*
 desc : Device Control - Reference Header <PDO>
 parm : drv_id	- [in]  Axis ���� (0, 1, 4, 5)
		mode	- [in]  ENG_MMSM (0x00:Only Homing operation or 0x01: Other operation)
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::WritePdoRefHead(ENG_PSOI obj_index, UINT64 data)
{
	BOOL bSucc			= FALSE;
	UINT32 u32Size		= sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8) + sizeof(STG_RVCH);
	LPG_PQWD pstWrite	= NULL;
	LPG_RVCH pstHead	= NULL;

	/* ���� ���� */
	if (m_syncPktSend.Enter())
	{
		/* �۽� ��Ŷ �޸� �Ҵ� */
		m_pPktReqWrite[u32Size]	= 0x00;
		/* UDP ��� ���� ���� */
		memcpy(m_pPktReqWrite, m_pstReqHead, sizeof(STG_PUCH));
		/* ���� �� ������ ���� */
		pstWrite	= (LPG_PQWD)(m_pPktReqWrite + sizeof(STG_PUCH));
		pstHead		= (LPG_RVCH)(m_pPktReqWrite + sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8));

		/* �⺻ �۽� ��� ���� */
		pstWrite->e_gen3_dnc_cmd= (UINT8)ENG_DCG3::en_req_write_obj;
		pstWrite->sub_id		= 0x00/*UINT8(drv_id)*/;
		/* ������ ��Ŷ ���� */
		pstWrite->serial_no		= m_u32SerialNo;
		pstWrite->index			= UINT32(obj_index)/*ENG_PSOI::en_pdo_ref_head_mcontrol*/;
		pstWrite->sub_index		= 0x00000000;	/* MC Control : Offset */
		pstWrite->data_len		= (UINT16)sizeof(STG_RVCH);
		/* Reference Header Part -> ���� �⺻ �� �����ϰ� */
		memcpy(pstHead, &m_pstShMemMC2->ref_head, sizeof(STG_RVCH));
		/* Object Type�� ���� ���� �ٸ��� ���� ���� */
		switch (obj_index)
		{
		case ENG_PSOI::en_pdo_ref_head_mcontrol		:
			memcpy(pstHead, &data, 1);				break;	/*pstHead->mc_en_stop	= UINT8(data);	break;*/
		case ENG_PSOI::en_pdo_ref_head_v_scale		:
			pstHead->v_scale		= UINT8(data);	break;;
		case ENG_PSOI::en_pdo_ref_head_d_outputs	:
			pstHead->digital_outputs= UINT64(data);	break;;
		}

		/* �޸� �㿡 ��� (Object 0x3801, PDO Reference Value Collective Object) */
		if (WriteObjectValueByIndex(pstWrite->sub_id, 0x0000, pstWrite->index))
		{
			/* MC2 ��Ŷ �۽� */
			bSucc	= AddPktData(m_pPktReqWrite, u32Size, 0x01);
		}
		/* ���� ���� */
		m_syncPktSend.Leave();
	}

	return bSucc;
}

/*
 desc : Device Control - Reference Drive <SDO> (0x3808)
 parm : drv_id	- [in]  Axis ���� (0, 1, 4, 5)
		cmd		- [in]  ENG_MCCN
		param0	- [in]  Parameter 0
		param1	- [in]  Parameter 1
		param2	- [in]  Parameter 2
		param3	- [in]  Parameter 3
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::WriteSdoRefDrive(ENG_MMDI drv_id, ENG_MCCN cmd,
								   UINT8 param0, INT32 param1, INT32 param2, INT32 param3)
{
	BOOL bSucc			= FALSE;
	UINT32 u32Size		= sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8) + sizeof(STG_RVCD);
	LPG_PQWD pstWrite	= NULL;
	LPG_RVCD pstData	= NULL;

	/* ���� ���� */
	if (m_syncPktSend.Enter())
	{
		/* �۽� ��Ŷ �޸� �Ҵ� */
		m_pPktReqWrite[u32Size]	= 0x00;
		/* UDP ��� ���� ���� */
		memcpy(m_pPktReqWrite, m_pstReqHead, sizeof(STG_PUCH));
		/* ���� �� ������ ���� */
		pstWrite	= (LPG_PQWD)(m_pPktReqWrite + sizeof(STG_PUCH));
		pstData		= (LPG_RVCD)(m_pPktReqWrite + sizeof(STG_PUCH) + sizeof(STG_PQWD) - sizeof(PUINT8));

		/* �⺻ �۽� ��� ���� */
		pstWrite->e_gen3_dnc_cmd= (UINT8)ENG_DCG3::en_req_write_obj;
		pstWrite->sub_id		= UINT8(drv_id);
		/* ������ ��Ŷ ���� */
		pstWrite->serial_no		= m_u32SerialNo;
		pstWrite->index			= (UINT32)ENG_PSOI::en_sdo_ref_drive;
		pstWrite->sub_index		= 0x00000000;	/* MC Control : Offset */
		pstWrite->data_len		= (UINT16)sizeof(STG_RVCD);
		/* Reference Data Part */
		pstData->ctrl_cmd		= (UINT8)cmd;
		pstData->ctrl_cmd_toggle= !m_pstShMemMC2->GetDriveRef((UINT8)drv_id)->ctrl_cmd_toggle;
		pstData->param_0		= param0;		/* Base Axis */
		pstData->param_1		= param1;		/* Target Position */
		pstData->param_2		= param2;		/* Target Position - Slave */
		pstData->param_3		= param3;

		/* �޸� �㿡 ��� (Object 0x3801, PDO Reference Value Collective Object) */
		if (WriteObjectValueByIndex(pstWrite->sub_id, 0x0000, pstWrite->index))
		{
			/* MC2 ��Ŷ �۽� */
			bSucc	= AddPktData(m_pPktReqWrite, u32Size, 0x01);
		}
		/* ���� ���� */
		m_syncPktSend.Leave();
	}

	return bSucc;
}

/*
 desc : Luria�� ���� �뱤 (Printing) ������ ���ο� ����,
		MC2���� �ֱ������� ��� ������ ��û ���� ����
 parm : enable	- [in]  TRUE : ��û ����, FALSE : ��û ���� (�ƿ� ��û ������ �ƴ�)
 retn : None
*/
VOID CMC2CThread::SetSendPeriodPkt(BOOL enable)
{
	/* ���� ���� */
	if (m_syncPeriodPkt.Enter())
	{
		/* �ֱ������� ������ ��û ���� */
		m_bSendPeriodPkt	= enable;
		if (enable)	m_u64PeriodTime	= 250;	/* ���� ������ �ֱ������� ��û */
		else		m_u64PeriodTime	= 500;	/* ���� ������ �ֱ������� ��û */

		/* ���� ���� */
		m_syncPeriodPkt.Leave();
	}
}

/*
 desc : ���� ���� ���� ����
 parm : flag	- [in]  0x00: ���� ���� ����, 0x01: ����� ����
 retn : None
*/
VOID CMC2CThread::UpdateLinkTime(UINT8 flag)
{
	BOOL bUpate	= FALSE;

	/* ���� ���� ���� �����̰�, ������ ����� ���¶�� */
	if (!flag && m_pstShMemMC2->link.is_connected)			bUpate	= TRUE;
	/* ���� ���� �����̰�, ������ ������ ���¶�� */
	else if (flag && !m_pstShMemMC2->link.is_connected)	bUpate	= TRUE;

	/* ���� ���¿� �ٸ� �����, ���� ���� */
	if (bUpate)
	{
		m_pstShMemMC2->link.is_connected	= flag;
		m_pstShMemMC2->link.link_time[0]	= m_stLinkTime.wYear;
		m_pstShMemMC2->link.link_time[1]	= m_stLinkTime.wMonth;
		m_pstShMemMC2->link.link_time[2]	= m_stLinkTime.wDay;
		m_pstShMemMC2->link.link_time[3]	= m_stLinkTime.wHour;
		m_pstShMemMC2->link.link_time[4]	= m_stLinkTime.wMinute;
		m_pstShMemMC2->link.link_time[5]	= m_stLinkTime.wSecond;
		m_pstShMemMC2->link.last_sock_error	= GetSockLastError();
	}
}

/*
 desc : Reference Data ��ü ��û
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::ReadReqPktRefAll()
{
	/* Reference Data ��ü ��û ��Ŷ ���� */
	ReadObjectValueByIndex(0, MAX_WANTED_READ_SIZE, 0, (UINT32)ENG_PSOI::en_ref_value_collective);
	/* ��Ŷ ������ �۽� */
	return AddPktData(m_pPktReadBuff, PKT_SIZE_READ_OBJECT, 0x01);
}

/*
 desc : Active Data ��ü ��û
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMC2CThread::ReadReqPktActAll()
{
	/* Actvie Data ��ü ��û ��Ŷ ���� */
	ReadObjectValueByIndex(0, MAX_WANTED_READ_SIZE, 0, (UINT32)ENG_PSOI::en_act_value_collective);
	/* ��Ŷ ������ �۽� */
	return AddPktData(m_pPktReadBuff, PKT_SIZE_READ_OBJECT, 0x01);
}
