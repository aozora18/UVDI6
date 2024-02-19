
/*
 desc : MILARA �ø��� ��� ���� ������
*/

#include "pch.h"
#include "MilaraThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : apps	- [in]  Prealigner or Robot
		port	- [in]  ��� ��Ʈ ��ȣ (1 ~ 255)
		baud	- [in]  ��� ��Ʈ �ӵ� (CBR_100 ~ CBR_CBR_256000 �� �Է�)
						300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 1152000
		buff	- [in]  ��� ��Ʈ�� ��/���� ���� ũ��
 retn : None
*/
CMilaraThread::CMilaraThread(ENG_EDIC apps, UINT8 port, UINT32 baud, UINT32 buff)
{
	m_enAppsID		= apps;
	m_u8Port		= port;
	m_u8ReplySkip	= 0x00;

	/* �Ʒ� 2���� ������ �ø��� ��� ��Ʈ ���� �� �ݵ�� �ʱ�ȭ�� �� ���� */
	m_bIsInited		= FALSE;	/* �� ���� FALSE �� ���� ��� ��쿡�� �۽��� ���� ���� */
	m_u8InitStep	= 0x00;

	/* ���� ��ü ���� */
	m_pComm			= new CSerialComm(port, baud, buff, ENG_SHOT::en_shot_handshake_off);
	ASSERT(m_pComm);

	/* ������ ���Ե� ��/���� ���� �޸� �Ҵ� */
	m_pCmdSend		= (PUINT8)::Alloc(MAX_CMD_MILARA_SEND + 1);
	m_pCmdRecv		= (PUINT8)::Alloc(MAX_CMD_MILARA_RECV + 1);
	m_pszLastPkt	= (PCHAR)::Alloc(MAX_CMD_MILARA_RECV * 3 + 1);	/* 2 bytes Hexa-String + 1 bytes space */
	memset(m_pCmdSend, 0x00, MAX_CMD_MILARA_SEND+1);
	memset(m_pCmdRecv, 0x00, MAX_CMD_MILARA_RECV+1);
	memset(m_pszLastPkt, 0x00, MAX_CMD_MILARA_RECV*3+1);
	m_u32RecvSize	= 0;
	m_bIsTechMode	= FALSE;	/* Teching Mode���� ���� (�ʱ�. Teaching Mode �ƴ϶�� ����) */
	m_enInfoMode	= ENG_MRIM::en_rim_2;	/* �񵿱� ���� �����ϸ鼭, ���� ���� ������ ������Ʈ�� �ޱ�� �� */

	m_u64AckTimeWait= 1000;	/* �۽� �� 1 �� �̳��� ������ �� �;� �� */
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CMilaraThread::~CMilaraThread()
{
	if (m_pComm)
	{
		/* �ø��� ��� ��Ʈ ���� */
		CommClosed();
		/* �ø��� ��� ��ü ���� */
		delete m_pComm;
		m_pComm	= NULL;
	}
	/* �۽� ���� ���� */
	if (m_pCmdSend)		::Free(m_pCmdSend);
	if (m_pCmdRecv)		::Free(m_pCmdRecv);
	if (m_pszLastPkt)	::Free(m_pszLastPkt);
}

/*
 desc : ������ ������ ��, ���� �ѹ� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMilaraThread::StartWork()
{

	return TRUE;
}

/*
 desc : ������ ���� ��, ���������� ȣ���
 parm : None
 retn : None
*/
VOID CMilaraThread::RunWork()
{
	/* ���� ��� ��Ʈ�� ������ �ִ��� ���θ� Ȯ�� */
	/* ������ ���� ������, ���� �� �ٽ� ���� �õ� */
	if (!m_pComm->IsOpenPort())
	{
		/* ��� ��Ʈ �ٽ� ���� ���ؼ�, ���� �ð� ��� */
		Sleep(500);
		if (!m_pComm->OpenExt(ENG_CRTB::en_crtb_read_timeout_blocking, FALSE))
		{
			CommClosed();
		}
#if 0
		else
		{
			SendData("INF 1");
			SendData("INF");
		}
#endif
		/* ��� ��Ʈ �ٽ� ���� ������, ���� �ð� ��� */
		Sleep(500);
		return;
	}

	/* �ֱ������� ������ ������ �ִ��� �б� �õ� */
	if (m_syncComm.Enter())
	{
		if (!m_bIsInited)	Initialized();
		/* ��� ��Ʈ�κ��� ������ �б� */
		m_pComm->ReadData();
		/* ���ۿ� ���ŵ� �����Ͱ� �ִٸ� */
		if (m_pComm->GetReadSize())
		{
			ParseAnalysis();	/* �ӽ� ���ۿ� �ִ� ������ ������ �м��ϱ� */
		}

		/* ���� ���� */
		m_syncComm.Leave();
	}
}

/*
 desc : ������ ����(����)�� ��, ���������� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CMilaraThread::EndWork()
{
}

/*
 desc : ������ ������ �����ϰ��� �� ��,
		��� �̺�Ʈ ��� ���¸� Ż���ϵ��� �� ��, ���� �۾��� �����ؾ� �Ѵ�.
 parm : None
 retn : None
*/
VOID CMilaraThread::Stop()
{
#if 0
	/* �ݵ�� ��� �̺�Ʈ ���� ����� ������ ��, ���� �۾� ���� */
	if (m_pComm)	m_pComm->WaitCommEventBreak();
#endif
	// ������ ������ ���� ��Ŵ
	CThinThread::Stop();
}

/*
 desc : ���� �ø��� ��� ��Ʈ�� ������ �ִ��� ���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMilaraThread::IsOpenPort()
{
	if (!m_pComm)	return FALSE;
	return m_pComm->IsOpenPort();
}

/*
 desc : ���� �ֱٿ� �߻��� ���� �޽��� ����
 parm : msg	- [in]  ���� ������ ����� ���ڿ� ������
 retn : None
*/
VOID CMilaraThread::PutErrMsg(TCHAR *msg)
{
	if (m_pComm)	m_pComm->PutErrMsg(msg);
}

/*
 desc : ���� ��� ��Ʈ�� ȯ�� ���� �� ��ȯ
 parm : data	- [out] ��ȯ�Ǿ� ����� ����ü ����
 retn : TRUE or FALSE
*/
BOOL CMilaraThread::GetConfigPort(STG_SSCI &data)
{
	if (!m_pComm)	return FALSE;
	return m_pComm->GetConfigPort(data);
}

/*
 desc : ���� ��� ��Ʈ�� ȯ�� ���� �� ����
 parm : data	- [in]  �����ϰ��� �ϴ� ���� ����� ����ü ������
 retn : TRUE or FALSE
*/
BOOL CMilaraThread::SetConfigPort(LPG_SSCI data)
{
	if (!m_pComm)	return FALSE;
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
BOOL CMilaraThread::PopReadData(PUINT8 data, UINT32 &size)
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
 desc : ���ŵ� �����Ͱ� �ִ��� �� ��û (���� �����͸� ���縸 �ؼ� ������)
 parm : port	- [in]  �˻� ����� ��� ��Ʈ ��ȣ
		data	- [in]  ���ŵ� �����Ͱ� ����� ���� ������
		size	- [out] ���ŵ� �����Ͱ� ����� 'data' ������ ũ��
						������ ���ŵ� �������� ũ��
 retn : TRUE or FALSE
*/
BOOL CMilaraThread::CopyReadData(PUINT8 data, UINT32 &size)
{
	if (!m_pComm)	return FALSE;
	/* ���� �����尡 ���� ������ / �ƴ��� Ȯ�� */
	if (!IsBusy())
	{
		m_pComm->PutErrMsg(L"The comm thread is stopped");
		return FALSE;
	}
	/* ���ŵ� ������ ��������, ������ ũ�� ���� */
	size	= m_pComm->CopyReadData(data, size);

	return TRUE;
}

/*
 desc : ���� ��� ���� �� ��ȯ
 parm : None
 retn : ENG_SCSC
*/
ENG_SCSC CMilaraThread::GetCommStatus()
{
	return m_pComm->GetCommStatus();
}

/*
 desc : ���� ��� ���� ���� ó��
 parm : None
 retn : None
*/
VOID CMilaraThread::CommClosed()
{
	m_bIsInited	= FALSE;
	if (m_pComm)	m_pComm->Close();
	ResetCommData();
}

/*
 desc : ���ŵ� �ӽ� ������ ������ �м�
 parm : None
 retn : None
*/
VOID CMilaraThread::ParseAnalysis()
{
	BOOL bIsFindA		= FALSE, bIsFindN = FALSE;
	UINT32 u32FindA		= 0 /* Ack */, u32FindN = 0 /* Not Ack */;
	UINT32 u32Read		= 0;

	do {

		/* ���� ���ۿ� ����� ������ ũ�� */
		u32Read	= m_pComm->GetReadSize();
		/* ���ŵ� �������� ���۰� �ʹ� ũ��, �о ���� */
		if (u32Read >= MAX_CMD_MILARA_RECV)
		{
			TCHAR tzMesg[128]	= {NULL};
			swprintf_s(tzMesg, 128, L"RS232_PORT[%d], The receive buffer was too full, "
									L"so all existing received data was read\n", m_u8Port);
			LOG_ERROR(ENG_EDIC::en_milara, tzMesg);
			m_pComm->ReadAllData();
			return;
		}
		/* �ּ��� 3 bytes ('>' or '?' + 'cr' + 'lf') �̻��� ���ŵǾ����� ���� */
		if (u32Read < UINT32(m_u8ReplySkip+0x03))	return;

		/* ���ŵ� ���ڿ� �߿��� '>' Ȥ�� '?'�� �ִ��� Ȯ�� */
		bIsFindA = m_pComm->FindChar(UINT8('\n'), UINT8('>'), u32FindA);
		bIsFindN = m_pComm->FindChar(UINT8('\n'), UINT8('?'), u32FindN);

		/* ��� �˻� ������ ��� */
		if (!u32FindA && !bIsFindN)	return;
		/* '>' or '?' �տ� line feed <lf>�� �����ϴ��� Ȯ�� */

		/* 2 ���� ���� �˻� ��� �߿��� 1 ���� �����ϴ� ��� */
		if (!(u32FindA && bIsFindN))
		{
			u32Read	= u32FindA > u32FindN ? u32FindA : u32FindN;
		}
		/* 2 ���� ���� �˻� ��� �߿��� 2 �� ��� �����ϴ� ��� */
		else
		{
			u32Read	= u32FindA > u32FindN ? u32FindN : u32FindA;
		}
		/* !!! ������ �˻� ��ġ�� Zero-based �̹Ƿ� !!! */
		u32Read++;

		/* !!! �ݵ��, ���� ���� �ʱ�ȭ !!! */
		memset(m_pCmdRecv, 0x00, MAX_CMD_MILARA_RECV);
		/* ���ŵ� ���۷κ��� ������ (��Ŷ) �б� ('\r\n' 2�� ���� �����ؼ� �о���̱�) */
		if (!m_pComm->PopReadData((PUINT8)m_pCmdRecv, u32Read))
		{
			TCHAR tzMesg[128]	= {NULL};
			swprintf_s(tzMesg, 128,
					   L"RS232_PORT[%d], Failed to read data from communication buffer\n", m_u8Port);
			LOG_ERROR(ENG_EDIC::en_milara, tzMesg);
			return;
		}

		/* !!! ���� �ֱٿ� ���ŵ� ��ɾ� ('cr'�� '>' or '?' ����) ũ�� �ӽ� ���� !!! */
		m_u32RecvSize	= u32Read;

		/* ���� �ֱٿ� ������ ��ɾ��� ���� ���� �� ���� */
		if (m_pCmdRecv[u32Read-0x01] == 0x3e /* '>' */)	m_enLastSendAck = ENG_MSAS::succ;
		else											m_enLastSendAck	= ENG_MSAS::fail;

		/* ���ŵ� �������� ũ�Ⱑ 3 bytes �����̸�, '>\r\n' or '?\r\n' �̸�, ������ ���� ó�� ���� ���� */
		if (m_u32RecvSize > UINT32(m_u8ReplySkip+0x03) &&
			m_enLastSendAck == ENG_MSAS::succ)
		{
			RecvData();
		}

		/* ��ư ���ŵ� �����Ͱ� �ִ�. (�����̵� �����̵�) */
		m_bIsRecv	= TRUE;

	} while (1);
}

/*
 desc : ���� ��� ���� ��/���� ������ �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CMilaraThread::ResetCommData()
{
	/* ���� ���� */
	if (m_syncComm.Enter())
	{
		/* ���� �ֱ� �۽� ��ɾ� (�Ķ���� ����) �ʱ�ȭ */
		m_enLastSendAck	= ENG_MSAS::none;
		/* ��� ��Ʈ�κ��� ������ �б� �õ� */
		m_pComm->ReadData();
		/* ���� ���ŵ� ���� ��� �о ������ */
		m_pComm->ReadAllData();

		/* ���� ���� */
		m_syncComm.Leave();
	}
}

/*
 desc : ��ɾ� �۽� (�Ķ���� ����)
 parm : cmd	- [in]  �Ķ���Ͱ� ���Ե� ���ڿ� (ASCII Code)
		size- [in]  'cmd' �� ����� ���ڿ� (����) ũ��
 retn : TRUE or FALSE
*/
BOOL CMilaraThread::SendData(PCHAR cmd, UINT32 size)
{
	BOOL bSended	= FALSE;
	UINT32 u32Size	= size + 1 /* because of carriage return */;
	PUINT8 pCmdSend	= m_pCmdSend;

	/* ���� �۽� ��ɿ� ���� ���� ���°� �������� �ʾҴٶ�� ���� */
	m_enLastSendAck	= ENG_MSAS::none;
	/* ���� ����� ����� �������� ���� */
	if (!m_pComm->IsOpenPort())
	{
		m_pComm->PutErrMsg(L"Serial communication port did not open", 0x00, FALSE);
		return FALSE;
	}

	/* �۽� �������� ���۰� ��� �ִ� ������� */
	if (strlen(cmd) < 1 || size < 1 || MAX_CMD_MILARA_SEND < u32Size)
	{
		LOG_WARN(ENG_EDIC::en_milara, L"Send command length is too short or too large");
		return FALSE;
	}

	/* �۽� ���� �ʱ�ȭ */
	memset(m_pCmdSend, 0x00, MAX_CMD_MILARA_SEND);

	/* ���� ���� */
	if (m_syncComm.Enter())
	{
		/* �۽� ���ۿ� ���� */
		memcpy(pCmdSend, cmd, size);	/* ��� ���� */
		pCmdSend	+= size;
		*pCmdSend	= 0x0d;	/* Carriage Return */

		/* ���� �ֱ� ���ŵ� ������ �ʱ�ȭ */
		memset(m_pCmdRecv, 0x00, MAX_CMD_MILARA_RECV);
		m_u32RecvSize	= 0;

		/* ������ �۽� */
		bSended	= m_pComm->WriteData(m_pCmdSend, u32Size);

		/* ���� ���� */
		m_syncComm.Leave();
	}

	/* �۽ŵ� ��Ŷ ���ڿ��� �ӽ� ���� (16����) */
	if (bSended && GetConfig()->set_comn.comm_log_milara)
	{
		UINT8 i	= 0x00;
		TCHAR tzPkts[256]	= {NULL}, tzHex[4] = {NULL};
		if (u32Size > 64)	u32Size = 64;
		swprintf_s(tzPkts, 256, L"PODIS vs. MILARA [Send] [called_func=SendData]"
								L"[comm_port=%d][size=%d]: ", m_u8Port, u32Size);
		for (; i<u32Size; i++)
		{
			swprintf_s(tzHex, 4, L"%02x ", m_pCmdSend[i]);
			wcscat_s(tzPkts, 256, tzHex);
		}
		LOG_MESG(m_enAppsID, tzPkts);
	}

	return bSended;
}

/*
 desc : ��� ���� ��� �ð� �� ����
 parm : wait	- [in]  ��� (���) ���� �� ������ �� ������ �ִ� ��ٸ��� �ð� (����: msec)
 retn : None
*/
VOID CMilaraThread::SetAckTimeout(UINT64 wait)
{
	m_u64AckTimeWait = wait;
}

/*
 desc : ��� (���) �۽� �� ������ �� ������ ���� �ð� ��� �ϵ��� �ϱ� ����
 parm : None
 retn : None
*/
VOID CMilaraThread::WaitTimeToRecv()
{
#ifndef _DEBUG
	UINT64 u64Wait	= GetTickCount64() + m_u64AckTimeWait;
#endif
	do {
#ifdef _DEBUG
		if (GetRecvSize())	break;
#else
		/* ��� (Prealigner)�κ��� ������ �� ������ �ּ��� ��⸦ �ߴ��� ���� */
		if (GetTickCount64() > u64Wait)	break;
#endif
		uvCmn_uSleep(10);	/* ��� ���� */

	} while (1);
}

/*
 desc : �α� ��� (Only �ø��� ��� ��/���� ����)
 parm : type	- [in]  0x00: Send Command, 0x01: Recv Command, 0x02 : Recv Data
		buff	- [in]  �۽� Ȥ�� ���� ���� (���)�� ����� ���� Ȥ�� ���ŵ� �޽��� ����
 retn : None
*/
VOID CMilaraThread::SaveErrorToSerial(UINT8 type, PCHAR buff)
{
	TCHAR tzMesg[1024]	= {NULL}, tzType[3][8] = { L"Send", L"Recv", L"Data" };
	CUniToChar csCnv;

	swprintf_s(tzMesg, 1024, L"Prealinger or Robot: %s : %s", tzType[type], csCnv.Ansi2Uni(buff));
	LOG_ERROR(ENG_EDIC::en_milara, tzMesg);
}

/*
 desc : ��� �۽� �� ���� �ޱ�
 parm : cmd	- [in]  Send ��ɾ ����� ����
 retn : Null or ���ŵ� �����Ͱ� ����� ���� ���� ������
*/
BOOL CMilaraThread::SendData(PCHAR cmd)
{
	/* ���ŵ� �����Ͱ� ���ٰ� �÷��� ���� */
	m_bIsRecv	= FALSE;

	/* ��� ���� */
	if (!SendData(cmd, (UINT32)strlen(cmd)))
	{
		SaveErrorToSerial(0x00, cmd);
		m_bIsSend	= FALSE;	/* ������ �۽� ���� */
		return FALSE;
	}
	/* ������ �۽� ���� ó�� */
	m_bIsSend	= TRUE;

	return TRUE;
}

/*
 desc : �޸� (Comma)�� ������ ���ڿ� (��� ���� ������) �м� (���ڿ� ���ۿ� ���)
 parm : vals	- [out] �޸��� �и��� ������ ������ ���ڿ��� ����� ���� ��
 retn : None
*/
VOID CMilaraThread::GetValues(vector <string> &vals)
{
	UINT32 u32Find	= 0, i = 0, u32Recv = m_u32RecvSize - 0x03 /* = '>' or '?' + '/r' + '/n' */;
	PCHAR pszVal	= NULL, pszRecv = (PCHAR)m_pCmdRecv + m_u8ReplySkip /* Skip : > or ? */;

	/* �޸� �Ҵ� */
	pszVal	= (PCHAR)::Alloc(MAX_CMD_MILARA_RECV);
	ASSERT(pszVal);

	if (u32Recv > 0)
	{
		/* �޸� ������ �ձ��� �� �и� �� ���� */
		for (; i<u32Recv; i++)
		{
			if (pszRecv[i] == ',')
			{
				/* �˻��� �� ���� (���) */
				memset(pszVal, 0x00, MAX_CMD_MILARA_RECV);
				memcpy(pszVal, pszRecv+u32Find, i-u32Find);
				vals.push_back(pszVal);

				/* ���� �ֱٿ� �˻��� ��ġ �� �ӽ� ���� */
				u32Find = i + 1;	/* �޸� ���� ���ڷ� ��ġ �缳�� */
			}
		}

		/* �� ������ �� ���� */
		memset(pszVal, 0x00, MAX_CMD_MILARA_RECV);
		memcpy(pszVal, pszRecv+u32Find, i-u32Find);
		vals.push_back(pszVal);
#if 0
		/* �α� ���ڿ��� ��� */
		CHAR szMesg[512]= {NULL};
		CUniToChar csCnv;
		for (i=0; i<vals.size(); i++)
		{
			strcat_s(szMesg, 512, vals[i].c_str());
			strcat_s(szMesg, 512, " ");
		}
		LOG_MESG(m_enAppsID, csCnv.Ansi2Uni(szMesg));
#endif
	}

	/* �޸� ���� */
	::Free(pszVal);
}

/*
 desc : �޸� (Comma)�� ���Ե� ���� ���ڿ����� �޸� ����
 parm : None
 retn : ���ڰ� ��ȯ
*/
UINT32 CMilaraThread::RleaseCommaToNum()
{
	string strBuff((PCHAR)m_pCmdRecv);

	/* ���ڿ� �߿� Comma (',') ���� ���� : remove(strBuff.begin(), strBuff.end(), ',')�� ���� ������ ������ ���� ��ȯ */
	if (strBuff.find(',') != string::npos)
	{
		strBuff.erase(remove(strBuff.begin(), strBuff.end(), ','), strBuff.end());
	}

	return (UINT32)atol(strBuff.c_str());
}

/*
 desc : �� ��(R, Z, T) �� �Էµ� ���� ���α׷��� �����Ǵ� ������ ��ȯ
 parm : p_type	- [in]  Point (�Ҽ���) �ڸ� ���� (00, 000, 000, ...)
		value	- [in]  ���� R�� Z�� ���, inch
						���� T�� ���, degree
 retn : �� ��ȯ (������ inch or degree --> �Ǽ��� um or degree)
 */
DOUBLE CMilaraThread::GetInchToPos(ENG_RITU p_type, INT32 value)
{
	DOUBLE dbValue		= 0.0f;
	DOUBLE dbInchToUm	= GetInchToUm();

	/* �ԷµǴ� ���� ���� ���� ��ȯ�� �ٸ� */
	switch (p_type)
	{
	case ENG_RITU::itu_00	: dbValue = (value / 100.0f) * dbInchToUm;		break;	/* 1 --> 0.01 inch, 0.01 inc = 254.0 um */
	case ENG_RITU::itu_000	: dbValue = (value / 1000.0f) * dbInchToUm;		break;	/* 1 --> 0.01 inch, 0.01 inc = 254.0 um */
	case ENG_RITU::itu_0000	: dbValue = (value / 10000.0f) * dbInchToUm;	break;	/* 1 --> 0.0001 inch = 2.54 um */
	}

	return dbValue;
}

/*
 desc : �Ҽ��� �Է� ������ ���� ��� �����Ǵ� ������ ��ȯ
 parm : p_type	- [in]  Point (�Ҽ���) �ڸ� ���� (00, 000, 000, ...)
		value	- [in]  User Input Value (unit : um)
 retn : �� ��ȯ (�Ǽ��� um or degree --> ������ inch or degree)
 */
INT32 CMilaraThread::GetPosToInch(ENG_RUTI p_type, DOUBLE value)
{
	DOUBLE dbValue		= 0.0f;
	DOUBLE dbInchToUm	= GetInchToUm()/*2.54 / * 1 inch -> 2.54 cm * / * 10000 / * cm -> um * /*/;

	/* �ԷµǴ� ���� ���� ���� ��ȯ�� �ٸ� */
	switch (p_type)
	{
	case ENG_RUTI::uti_00	:	dbValue	= (value / dbInchToUm) /* inch */ * 100.0f   /* 1 --> 0.01 inch */;		break;
	case ENG_RUTI::uti_000	:	dbValue	= (value / dbInchToUm) /* inch */ * 1000.0f  /* 1 --> 0.01 inch */;		break;
	case ENG_RUTI::uti_0000	:	dbValue	= (value / dbInchToUm) /* inch */ * 10000.0f /* 1 --> 0.0001 inch */;	break;
	}

	return (INT32)ROUNDED(dbValue, 0);
}

/*
 desc : �� ��(R, Z, T) �� �Էµ� ���� ���α׷��� �����Ǵ� ������ ��ȯ
 parm : axis	- [in]  Axis Name (R : Horizontal, Z : Vertical, T : Angle)
		value	- [in]  ���� R�� Z�� ���, inch
						���� T�� ���, degree
 retn : �� ��ȯ (������ inch or degree --> �Ǽ��� um or degree)
 */
DOUBLE CMilaraThread::GetAxisPosApp(ENG_RANC axis, INT32 value)
{
	DOUBLE dbValue		= 0.0f;
	DOUBLE dbInchToUm	= GetInchToUm();

	/* �ԷµǴ� ���� ���� ���� ��ȯ�� �ٸ� */
	switch (axis)
	{
	case ENG_RANC::rac_t	: dbValue = value * 0.01;						break;	/* 1 --> 0.01�� */
	case ENG_RANC::rac_r1	: 
	case ENG_RANC::rac_r2	: 
	case ENG_RANC::rac_z	: dbValue = (value / 1000.0f) * dbInchToUm;		break;	/* 1 --> 0.001 inch, 0.001 inc = 25.4 um */
	}

	return dbValue;
}

/*
 desc : ������ �� ��ȯ �� ����
 parm : host_val- [in]  ����� Host�� Double/UINT32/UINT16/UINT8/INT8 �迭 ���� ������
		vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMilaraThread::SetValues(DOUBLE *host_val, vector <string> &vals)
{
	UINT32 i = 0, u32Count = (UINT32)vals.size();
	if (u32Count > 4)	u32Count	= 4;	/* �ϴ�, K �� ���� �ʿ� ����, ���ڿ��� ��ȯ�Ǳ� ������ ... */
	for (; i<u32Count; i++)	host_val[i] = atof(vals[i].c_str());
}
VOID CMilaraThread::SetValues(UINT32 *host_val, vector <string> &vals)
{
	UINT32 i = 0, u32Count = (UINT32)vals.size();
	if (u32Count > 4)	u32Count	= 4;	/* �ϴ�, K �� ���� �ʿ� ����, ���ڿ��� ��ȯ�Ǳ� ������ ... */
	for (; i<u32Count; i++)	host_val[i] = (UINT32)atoi(vals[i].c_str());
}
VOID CMilaraThread::SetValues(INT32 *host_val, vector <string> &vals)
{
	UINT32 i = 0, u32Count = (UINT32)vals.size();
	if (u32Count > 4)	u32Count	= 4;	/* �ϴ�, K �� ���� �ʿ� ����, ���ڿ��� ��ȯ�Ǳ� ������ ... */
	for (; i<u32Count; i++)	host_val[i] = atoi(vals[i].c_str());
}
VOID CMilaraThread::SetValues(UINT16 *host_val, vector <string> &vals)
{
	UINT32 i = 0, u32Count = (UINT32)vals.size();
	if (u32Count > 4)	u32Count	= 4;	/* �ϴ�, K �� ���� �ʿ� ����, ���ڿ��� ��ȯ�Ǳ� ������ ... */
	for (; i<u32Count; i++)	host_val[i] = (UINT16)atoi(vals[i].c_str());
}
VOID CMilaraThread::SetValues(UINT8 *host_val, vector <string> &vals)
{
	UINT32 i = 0, u32Count = (UINT32)vals.size();
	if (u32Count > 4)	u32Count	= 4;	/* �ϴ�, K �� ���� �ʿ� ����, ���ڿ��� ��ȯ�Ǳ� ������ ... */
	for (; i<u32Count; i++)	host_val[i] = (UINT8)atoi(vals[i].c_str());
}
VOID CMilaraThread::SetValues(INT8 *host_val, vector <string> &vals)
{
	UINT32 i = 0, u32Count = (UINT32)vals.size();
	if (u32Count > 4)	u32Count	= 4;	/* �ϴ�, K �� ���� �ʿ� ����, ���ڿ��� ��ȯ�Ǳ� ������ ... */
	for (; i<u32Count; i++)	host_val[i] = (INT8)atoi(vals[i].c_str());
}
VOID CMilaraThread::SetValues(INT16 *host_val, vector <string> &vals)
{
	UINT32 i = 0, u32Count = (UINT32)vals.size();
	if (u32Count > 4)	u32Count	= 4;	/* �ϴ�, K �� ���� �ʿ� ����, ���ڿ��� ��ȯ�Ǳ� ������ ... */
	for (; i<u32Count; i++)	host_val[i] = (INT16)atoi(vals[i].c_str());
}

/*
 desc : Inch �� ��ȯ �� ����
 parm : host_val- [in]  ����� Host�� Double/UINT32/UINT16/UINT8/INT8 �迭 ���� ������
		vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMilaraThread::SetInch01Value(DOUBLE &host_val, vector <string> &vals)
{
	host_val = GetInchToPos(ENG_RITU::itu_00, atoi(vals[0].c_str()));
}
VOID CMilaraThread::SetInch001Value(DOUBLE &host_val, vector <string> &vals)
{
	host_val = GetInchToPos(ENG_RITU::itu_000, atoi(vals[0].c_str()));
}
VOID CMilaraThread::SetInch0001Value(DOUBLE &host_val, vector <string> &vals)
{
	host_val = GetInchToPos(ENG_RITU::itu_0000, atoi(vals[0].c_str()));
}
VOID CMilaraThread::SetInch0001Values(DOUBLE *host_val, vector <string> &vals)
{
	UINT32 i = 0, u32Count = (UINT32)vals.size();
	if (u32Count > 4)	u32Count	= 4;	/* �ϴ�, K �� ���� �ʿ� ����, ���ڿ��� ��ȯ�Ǳ� ������ ... */
	for (; i<u32Count; i++)	host_val[i] = GetInchToPos(ENG_RITU::itu_0000, atoi(vals[i].c_str()));
}

/*
 desc : ������ �� ��ȯ �� ����
 parm : host_val- [in]  ����� Host�� Double/UINT32/UINT16/UINT8/INT8 �迭 ���� ������
		vals	- [in]  ���ŵ� �߿����� ���� ����� ���ڿ� ���� ������
 retn : None
*/
VOID CMilaraThread::SetValue(DOUBLE &host_val, vector <string> &vals)
{
	host_val = atof(vals[0].c_str());
}
VOID CMilaraThread::SetValue(INT32 &host_val, vector <string> &vals)
{
	host_val = (UINT32)atoi(vals[0].c_str());
}
VOID CMilaraThread::SetValue(UINT32 &host_val, vector <string> &vals)
{
	host_val = (UINT32)atoi(vals[0].c_str());
}
VOID CMilaraThread::SetValue(INT16 &host_val, vector <string> &vals)
{
	host_val = (INT16)atoi(vals[0].c_str());
}
VOID CMilaraThread::SetValue(UINT16 &host_val, vector <string> &vals)
{
	host_val = (UINT16)atoi(vals[0].c_str());
}
VOID CMilaraThread::SetValue(UINT8 &host_val, vector <string> &vals)
{
	host_val = (UINT8)atoi(vals[0].c_str());
}
VOID CMilaraThread::SetValue(INT8 &host_val, vector <string> &vals)
{
	host_val = (INT8)atoi(vals[0].c_str());
}
VOID CMilaraThread::SetValue(CHAR &host_val, vector <string> &vals)
{
	host_val = vals[0].c_str()[0];
}
VOID CMilaraThread::SetValueHex(UINT32 &host_val, vector <string> &vals)
{
	host_val = strtol(vals[0].c_str(), NULL, 16);
}
VOID CMilaraThread::SetValueHex(UINT16 &host_val, vector <string> &vals)
{
	host_val = (UINT16)strtol(vals[0].c_str(), NULL, 16);
}
