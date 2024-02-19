
/*
 desc : EFU �ø��� ��� ������
*/

#include "pch.h"
#include "EFUThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : port	- [in]  ��� ��Ʈ ��ȣ (1 ~ 255)
		baud	- [in]  ��� ��Ʈ �ӵ� (CBR_100 ~ CBR_CBR_256000 �� �Է�)
						300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 1152000
		buff	- [in]  ��� ��Ʈ�� ��/���� ���� ũ��
		efu_cnt	- [in]  EFU ��ġ ����
		shmem	- [in]  ���� �޸�
 retn : None
*/
CEFUThread::CEFUThread(UINT8 port, UINT32 baud, UINT32 buff, UINT8 efu_cnt, LPG_EPAS shmem)
{
	m_u8Port	= port;
	/* ���� ��ü ���� */
	m_pComm		= new CSerialComm(port, baud, buff, ENG_SHOT::en_shot_handshake_off);
	ASSERT(m_pComm);
	m_pstState	= shmem;
	m_u8EFUCount= efu_cnt;

	m_pRecvData			= (PUINT8)::Alloc(MAX_CMD_EFU_RECV);
	ASSERT(m_pRecvData);
#ifdef _DEBUG
	m_bAutoStateGet		= TRUE;		/* �⺻������, EFU �ڵ����� ���� ��û ���� ��û���� �ʽ��ϴ�. */
#else
	m_bAutoStateGet		= FALSE;	/* �⺻������, EFU �ڵ����� ���� ��û ���� ��û���� �ʽ��ϴ�. */
#endif
	m_bIsPktRecv		= TRUE;
	m_u64AckTimeWait	= 1000;		/* �۽� �� 1 �� �̳��� ������ �� �;� �� */
	m_u64SendPktTime	= 0;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CEFUThread::~CEFUThread()
{
	if (m_pComm)
	{
		/* �ø��� ��� ��Ʈ ���� */
		CommClosed();
		/* �ø��� ��� ��ü ���� */
		delete m_pComm;
		m_pComm	= NULL;
	}
	if (m_pRecvData)	::Free(m_pRecvData);
	m_pRecvData	= NULL;
}

/*
 desc : ������ ������ ��, ���� �ѹ� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CEFUThread::StartWork()
{

	return TRUE;
}

/*
 desc : ������ ���� ��, ���������� ȣ���
 parm : None
 retn : None
*/
VOID CEFUThread::RunWork()
{
	/* ���� ��� ��Ʈ�� ������ �ִ��� ���θ� Ȯ�� �� ������ ���� ������, ���� �� �ٽ� ���� �õ� */
	if (!m_pComm->IsOpenPort())
	{
		if (!m_pComm->OpenExt(ENG_CRTB::en_crtb_read_timeout_blocking, FALSE))
			CommClosed();
		/* ��� ��Ʈ �ٽ� ���� ������, ���� �ð� ��� */
		Sleep(500);
	}
	else
	{
		/* �ֱ������� ������ ������ �ִ��� �б� �õ� */
		if (m_syncComm.Enter())
		{
			/* ��� ��Ʈ�κ��� ������ �б� */
			if (m_pComm->ReadData())
			{
				ParseAnalysis();
				m_bIsPktRecv	= TRUE;
			}
			else
			{
				/* ���� ���� ������ �������� Ȯ�� */
				SetRecvEnable();
				/* ������ ���ŵ� �����Ͱ� �ְ�, �ڵ����� ���� ���� ��û�ϴ� ������� Ȯ�� */
				if (m_bIsPktRecv && m_bAutoStateGet)
					ReqGetState();
			}
			/* ���� ���� */
			m_syncComm.Leave();
		}
	}
}

/*
 desc : ������ ����(����)�� ��, ���������� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CEFUThread::EndWork()
{
}

/*
 desc : ������ ������ �����ϰ��� �� ��, ��� �̺�Ʈ ��� ���¸� Ż���� ��, ���� �۾� ����
 parm : None
 retn : None
*/
VOID CEFUThread::Stop()
{
#if 0
	/* �ݵ�� ��� �̺�Ʈ ���� ����� ������ ��, ���� �۾� ���� */
	if (m_pComm)	m_pComm->WaitCommEventBreak();
#endif
	/* ������ ������ ���� ��Ŵ */
	CThinThread::Stop();
}

/*
 desc : ���� �ø��� ��� ��Ʈ�� ������ �ִ��� ���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL CEFUThread::IsOpenPort()
{
	return m_pComm->IsOpenPort();
}

/*
 desc : ���� ��� ��Ʈ�� ȯ�� ���� �� ��ȯ
 parm : data	- [out] ��ȯ�Ǿ� ����� ����ü ����
 retn : TRUE or FALSE
*/
BOOL CEFUThread::GetConfigPort(STG_SSCI &data)
{
	return m_pComm->GetConfigPort(data);
}

/*
 desc : ���� ��� ��Ʈ�� ȯ�� ���� �� ����
 parm : data	- [in]  �����ϰ��� �ϴ� ���� ����� ����ü ������
 retn : TRUE or FALSE
*/
BOOL CEFUThread::SetConfigPort(LPG_SSCI data)
{
	return m_pComm->SetConfigPort(data);
}

/*
 desc : �ֱ������� ���ŵ� �����Ͱ� �ִ��� �� ��û
 parm : port	- [in]  �˻� ����� ��� ��Ʈ ��ȣ
		data	- [in]  ���ŵ� �����Ͱ� ����� ���� ������
		size	- [out] ���ŵ� �����Ͱ� ����� 'data' ������ ũ��
						������ ���ŵ� �������� ũ��
 retn : TRUE or FALSE
*/
BOOL CEFUThread::PopCommData(PUINT8 data, UINT32 &size)
{
	if (!m_pComm)	return FALSE;

	/* ���� �����尡 ���� ������ / �ƴ��� Ȯ�� */
	if (!IsBusy())
	{
		m_pComm->PutErrMsg(L"The comm thread is stopped");
		return FALSE;
	}
	/* ���ŵ� ������ ��������, ������ ũ�� ���� */
	size = m_pComm->PopReadData(data, size);

	return TRUE;
}

/*
 desc : ���� ��� ���� �� ��ȯ
 parm : None
 retn : ENG_SCSC
*/
ENG_SCSC CEFUThread::GetCommStatus()
{
	return m_pComm->GetCommStatus();
}

/*
 desc : ���� ��� ���� ���� ó��
 parm : None
 retn : None
*/
VOID CEFUThread::CommClosed()
{
	if (m_pComm)	m_pComm->Close();
	ResetCommData();
}

/*
 desc : ���ŵ� �ӽ� ������ ������ �м�
 parm : None
 retn : None
*/
VOID CEFUThread::ParseAnalysis()
{
	UINT8 u8Pkt[8]	= {NULL}, u8ChkSum = 0x00, i;
	UINT32 u32Read	= 0, u32Pkts = 7;
	PUINT8 pRecvData= m_pRecvData;

	do {

		/* ���� ���ۿ� ����� ������ ũ�� */
		u32Read	= m_pComm->GetReadSize();
		/* �ּ��� 6 bytes (stx+mode1+mode2+lv32_id+dpu_id+...+etx) �̻��� ���ŵǾ����� ���� */
		if (u32Read < 0x06)	return;
		/* ���ŵ� �������� ���۰� �ʹ� ũ��, �о ���� */
		if (u32Read >= MAX_CMD_EFU_RECV)
		{
			TCHAR tzMesg[256]	= {NULL};
			swprintf_s(tzMesg, 256, L"RS232_PORT[%d], The receive buffer was too full, "
									L"so all existing received data was read\n", m_u8Port);
			LOG_ERROR(ENG_EDIC::en_efu_ss, tzMesg);
			m_pComm->ReadAllData();
			return;
		}

		/* �ּ��� 6 bytes (stx+mode1+mode2+lv32_id+dpu_id+...+etx) �̻��� ���ŵǾ����� ���� */
		u32Read	= m_pComm->GetReadSize();
		if (u32Read < 0x06)	return;

		/* 5 bytes �б� */
		m_pComm->CopyReadData(u8Pkt, 5);
		/* In case of response BL500 ID & PV & Alarm & SV */
		if (0x8A == u8Pkt[1] && 0x87 == u8Pkt[2])
		{
			u32Pkts	+= 4 /* bl500_id + pv + alarm + sv */ * m_u8EFUCount;
		}
		/* In case of response BL500 speed */
		else if (0x89 == u8Pkt[1] && 0x84 == u8Pkt[2])
		{
			u32Pkts	+= 3 /* StartID+EndID+FlagOK */;
		}
		/* ������ �� ���ŵ� �����Ͱ� �ִ��� ���� */
		if (u32Read < u32Pkts)	return;

		/* ���ŵ� ���ۿ� �ִ� ��� ���� �������� (!!! Pops !!!) */
		memset(m_pRecvData, 0x00, MAX_CMD_EFU_RECV);
		m_pComm->PopReadData(pRecvData, u32Pkts);
		/* Start / End Bit �� */
		if (!(EFU_PKT_STX == pRecvData[0] && EFU_PKT_ETX == pRecvData[u32Pkts-1]))
		{
			TCHAR tzMesg[256]	= {NULL};
			swprintf_s(tzMesg, 256, L"RS232_PORT[%d], The first (STX:%02x) and last (ETX:%02x) of the packet are incorrect, "
									L"so all existing received data was read\n", m_u8Port, m_pRecvData[0], m_pRecvData[u32Pkts-1]);
			LOG_ERROR(ENG_EDIC::en_efu_ss, tzMesg);
			m_pComm->ReadAllData();
			return;
		}
		/* ��Ŷ�� Check Sum �� */
		for (i=1; i<u32Pkts-2; i++)	u8ChkSum	+= pRecvData[i];
		if (u8ChkSum != pRecvData[u32Pkts-2])
		{
			TCHAR tzMesg[256]	= {NULL};
			swprintf_s(tzMesg, 256, L"RS232_PORT[%d], The original [%02x] and received packets [%02x] have different CheckSum values, "
									L"so all existing received data was read\n", m_u8Port, pRecvData[u32Read-2], u8ChkSum);
			LOG_ERROR(ENG_EDIC::en_efu_ss, tzMesg);
			m_pComm->ReadAllData();
			return;
		}
		/* ������ �ΰ� �ִ� ��ġ�� �ּ� �̵� */
		pRecvData	+= 5;	/* DPU_ID ����... */
		/* In case of response BL500 ID & PV & Alarm & SV */
		if (0x8A == u8Pkt[1] && 0x87 == u8Pkt[2])
		{
			/* ��� ��� ������ ���� */
			for (i=0; i<m_u8EFUCount; i++)
			{
				memcpy(&m_pstState[i], pRecvData, 4);	pRecvData	+= 0x04;
			}
		}
		/* In case of response BL500 speed */
		else if (0x89 == u8Pkt[1] && 0x84 == u8Pkt[2])
		{
			memcpy(&m_stSpeed, pRecvData, 3);
		}
		/* Unknown data */
		else
		{
			TCHAR tzMesg[256]	= {NULL};
			swprintf_s(tzMesg, 256, L"RS232_PORT[%d], An unknown command [mode1:%02x][mode2:%0x02] was received, "
									L"so all existing received data was read\n", m_u8Port, pRecvData[1], pRecvData[2]);
			LOG_ERROR(ENG_EDIC::en_efu_ss, tzMesg);
			m_pComm->ReadAllData();
			return;
		}

		/* ���Ź��� ��Ŷ ���ڿ��� �ӽ� ���� (16����) */
		if (GetConfig()->set_comn.comm_log_efu)
		{
			TCHAR tzPkts[256]	= {NULL}, tzHex[4] = {NULL};
			if (u32Pkts > 64)	u32Pkts = 64;
			swprintf_s(tzPkts, 256, L"PODIS vs. MILARA [Recv] [called_func=ParseAnalysis]"
									L"[comm_port=%d][size=%d]: ", m_u8Port, u32Pkts);
			for (i=0x00; i<u32Pkts; i++)
			{
				swprintf_s(tzHex, 4, L"%02x ", m_pRecvData[i]);
				wcscat_s(tzPkts, 256, tzHex);
			}
			LOG_MESG(ENG_EDIC::en_efu_ss, tzPkts);
		}

	} while (1);
}

/*
 desc : ���� ��� ���� ��/���� ������ �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CEFUThread::ResetCommData()
{
	/* ���� ���� */
	if (m_syncComm.Enter())
	{
		/* ��� ��Ʈ�κ��� ������ �б� �õ� */
		m_pComm->ReadData();
		/* ���� ���ŵ� ���� ��� �о ������ */
		m_pComm->ReadAllData();

		/* ���� ���� */
		m_syncComm.Leave();
	}
}

/*
 desc : ��� ���� ��� �ð� �� ����
 parm : wait	- [in]  ��� (���) ���� �� ������ �� ������ �ִ� ��ٸ��� �ð� (����: msec)
 retn : None
*/
VOID CEFUThread::SetAckTimeout(UINT64 wait)
{
	m_u64AckTimeWait = wait;
}
#if 0
/*
 desc : ���� EFU �ʿ� �����͸� �۽��ص� �Ǵ��� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CEFUThread::IsSendIdle()
{
	if (m_bIsPktRecv)	return TRUE;
#ifdef _DEBUG
	return (GetTickCount64() > (m_u64SendPktTime + 1000*120/*m_u64AckTimeWait*/));
#else
	return (GetTickCount64() > (m_u64SendPktTime + m_u64AckTimeWait));
#endif
}
#endif
/*
 desc : ���� �ý������κ��� �����͸� ���Ź��� �� �ִ� ���� ���� Ȯ��
 parm : None
 retn : None
*/
VOID CEFUThread::SetRecvEnable()
{
	if (m_bIsPktRecv)	return;

#ifdef _DEBUG
	m_bIsPktRecv = (GetTickCount64() > (m_u64SendPktTime + 1000*5/*m_u64AckTimeWait*/));
#else
	m_bIsPktRecv = (GetTickCount64() > (m_u64SendPktTime + m_u64AckTimeWait));
#endif
}

/*
 desc : ��ɾ� �۽� (�Ķ���� ����)
 parm : cmd	- [in]  �Ķ���Ͱ� ���Ե� ���ڿ� (ASCII Code)
		size- [in]  'cmd' �� ����� ���ڿ� (����) ũ��
 retn : TRUE or FALSE
*/
BOOL CEFUThread::SendData(PUINT8 cmd, UINT32 size)
{
	BOOL bSended	= FALSE;
	UINT64 u64Tick	= GetTickCount64();

	/* ���� ����� ����� �������� ���� */
	if (!m_pComm->IsOpenPort())
	{
		m_pComm->PutErrMsg(L"Serial communication port did not open", 0x00, FALSE);
		return FALSE;
	}
	/* -------------------------------------------------------------- */
	/* ���� ������ �۽ŵ� �����Ͱ� �ִ��� �׸��� ������ ���� �ʾҴ��� */
	/* -------------------------------------------------------------- */
	if (!m_bIsPktRecv && u64Tick < (m_u64SendPktTime + m_u64AckTimeWait))
	{
		LOG_WARN(ENG_EDIC::en_efu_ss, L"There is no response from EFU for previously sent packets");
		return FALSE;
	}
	/* --------------------------------------------------------------------- */
	/* ���� �ֱٿ� �۽ŵ� ��Ŷ�� �ð� ���� �� ���� ���ŵ� ��Ŷ�� ���ٰ� ���� */
	/* --------------------------------------------------------------------- */
	m_u64SendPktTime= GetTickCount64();
	m_bIsPktRecv	= FALSE;

	/* ������ �۽� */
	bSended	= m_pComm->WriteData(cmd, size);
	if (!bSended)
	{
		SaveError(0x00, cmd);
		/* ���� �۽��� �����ϸ�, ���� ��Ŷ�� ��Ŷ �۽� �ð� �缳�� */
		m_bIsPktRecv	= TRUE;
		m_u64SendPktTime= 0;
	}

	return bSended;
}

/*
 desc : �α� ��� (Only �ø��� ��� ��/���� ����)
 parm : type	- [in]  0x00: Send Command, 0x01: Recv Command, 0x02 : Recv Data
		buff	- [in]  �۽� Ȥ�� ���� ���� (���)�� ����� ���� Ȥ�� ���ŵ� �޽��� ����
 retn : None
*/
VOID CEFUThread::SaveError(UINT8 type, PUINT8 buff)
{
	TCHAR tzMesg[1024]	= {NULL}, tzType[3][8] = { L"Send", L"Recv", L"Data" };
	swprintf_s(tzMesg, 1024, L"EFU [%s]: mode1=%02x : mode2=%02x", tzType[type], buff[1], buff[2]);
	LOG_ERROR(ENG_EDIC::en_efu_ss, tzMesg);
}

/*
 desc : EFU ���� (Speed) ��û
 parm : bl_id	- [in]  EFU ID ��, 0x01 ~ 0x20 (1 ~ 32), 0x00 - ALL
		speed	- [in]  Setting Value ��, 0 ~ 140 (0 ~ 1400) RPM (1 = 10 RPM �ǹ�)
 retn : TRUE or FALSE
*/
BOOL CEFUThread::ReqSetSpeed(UINT8 bl_id, UINT8 speed)
{
	UINT8 i, u8Pkt[16]	= {NULL};
	BOOL bSended	= FALSE;

	/* ���� ���� ���� �ʱ�ȭ */
	memset(&m_stSpeed, 0x00, sizeof(STG_EBSS));

	/* STX, ETX */
	u8Pkt[0]	= EFU_PKT_STX;
	u8Pkt[9]	= EFU_PKT_ETX;
	u8Pkt[1]	= 0x89;			/* MODE 1 */
	u8Pkt[2]	= 0x84;			/* MODE 2 */
	u8Pkt[3]	= EFU_LV32_ID;	/* LV32 ID */
	u8Pkt[4]	= EFU_DPU_ID;
	/* BL500 ID */
	if (0x00 == bl_id)
	{
		u8Pkt[5]= 0x01;
		u8Pkt[6]= m_u8EFUCount;
		m_stSpeed.bl_start	= 0x01;
		m_stSpeed.bl_end	= m_u8EFUCount;
	}
	else
	{
		u8Pkt[5]= bl_id;
		u8Pkt[6]= bl_id;
		m_stSpeed.bl_start	= bl_id;
		m_stSpeed.bl_end	= bl_id;
	}
	u8Pkt[7]		= speed;	/* Setting Value (Speed. 0 ~ 140. 1 = 10 speed) */
	m_stSpeed.speed	= speed;
	for (i=1; i<8; i++)	u8Pkt[8] += u8Pkt[i];	/* Check Sum */

	/* ���� ���� */
	if (m_syncComm.Enter())
	{
		bSended	= SendData(u8Pkt, 10);
		/* ���� ���� */
		m_syncComm.Leave();
	}

	return bSended;
}

/*
 desc : EFU ���� ��û
 parm : None
 retn : TRUE or FALSE
*/
BOOL CEFUThread::ReqGetState()
{
	UINT8 i, u8Pkt[16]	= {NULL};
	BOOL bSended	= FALSE;

	/* STX, ETX */
	u8Pkt[0]	= EFU_PKT_STX;
	u8Pkt[8]	= EFU_PKT_ETX;
	u8Pkt[1]	= 0x8A;			/* MODE 1 */
	u8Pkt[2]	= 0x87;			/* MODE 2 */
	u8Pkt[3]	= EFU_LV32_ID;	/* LV32 ID */
	u8Pkt[4]	= EFU_DPU_ID;
	/* BL500 ID */
	u8Pkt[5]	= 0x01;
	u8Pkt[6]	= m_u8EFUCount;
	for (i=1; i<7; i++)	u8Pkt[7] += u8Pkt[i];	/* Check Sum */

	/* ���� ���� */
	if (m_syncComm.Enter())
	{
		bSended	= SendData(u8Pkt, 9);
		/* ���� ���� */
		m_syncComm.Leave();
	}

	return bSended;
}
