
/*
 desc : TCP/IP Server Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "GFSSThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : th_id	- [in]  thread id
		server	- [in]  Client ���� ����
 retn : None
*/
CGFSSThread::CGFSSThread(UINT16 th_id, LPG_CSID config)
	: CServerThread(ENG_EDIC::en_gfss,
					th_id,
					config->server->tcp_port,
					config->server->source_ipv4,	// CMPS Local (Client) IPv4
					config->server->target_ipv4,	// PLS Remote (Server) IPv4
					config->server->port_buff,
					config->server->recv_buff,
					config->server->ring_buff,
					config->server->idle_time,
					config->server->sock_time)
{
	/* ���� �ֱٿ� ���Ź��� �Ź� ���� ������ ���� */
	m_u32TotalFiles		= 0;
	m_u32FileCount		= 0;
	m_u32TotalSizes		= 0;

	/* Live Check �۽� ���� ���� (Idle Time ��� �������� ����) */
	m_u64PeriodLiveTime	= m_u64IdleTime / 2;
	m_u64LastLiveTime	= 0;

	wmemset(m_tzLastNewDir, 0x00, MAX_PATH_LEN);
	wmemset(m_tzLastDelDir, 0x00, MAX_PATH_LEN);
	wmemset(m_tzLastSaveDir, 0x00, MAX_PATH_LEN);
	/* ȯ�� ���� ���� */
	m_pstConfig			= config;
	/* ���� ���� �ʱ�ȭ (��뷮 ���� ���� ����) */
	UINT32 u32Size		= 1024 * 1024 * 64;
	m_pRecvBuff			= (PUINT8)::Alloc(u32Size+1);
	ASSERT(m_pRecvBuff);
	memset(m_pRecvBuff, 0x00, u32Size+1);
	/* ���� �ֱٿ� ���Źޱ� ������ �ð� �ʱ�ȭ */
	m_u64RecvStartTime	= GetTickCount64();
	m_bIsRecvCompleted	= FALSE;
}

/*
 desc : Destructor
 parm : None
*/
CGFSSThread::~CGFSSThread()
{
	/* ���� ���� ���� (��뷮 ���� ���� ����)*/
	if (m_pRecvBuff)	::Free(m_pRecvBuff);
	m_pRecvBuff	= NULL;

	/* ��Ŷ ���� �ӽ� ��� �޸� ���� */
	if (m_syncPktCmd.Enter())
	{
		m_lstPktCmd.RemoveAll();
		m_syncPktCmd.Leave();
	}
}

/*
 desc : ������ ����� �ֱ��� ȣ���
 parm : None
 retn : None
*/
VOID CGFSSThread::DoWork()
{
	/* ������ �ȵ� �������� Ȯ��*/
	if (!IsConnected())	return;

	/* Idle Time ���� 0���� ū ��츸 ���� */
	if (GetTickCount64() > (m_u64LastLiveTime + m_u64PeriodLiveTime))
	{
		/* Live Time �۽� */
		SetAliveTime();
		/* ���� �ֱٿ� �۽��� �ð� ���� */
		m_u64LastLiveTime	= GetTickCount64();
	}
}

/*
 desc : Ŭ���̾�Ʈ ������ ���� ���
 parm : sd	- ���� �����
 retn : TRUE or FALSE
*/
BOOL CGFSSThread::Accepted()
{
	m_u32TotalFiles		= 0;
	m_u32FileCount		= 0;
	m_u32TotalSizes		= 0;
	m_u32TotalZipSize	= 0;

	return TRUE;
}

/*
 desc : �����κ��� ���� ������ �� ���
 parm : None
 retn : None
*/
VOID CGFSSThread::CloseSocket(SOCKET sd, UINT32 time_out/* =0 */)
{
	/* ���� ��ü ȣ�� */
	CServerThread::CloseSocket(sd, time_out);
}

/*
 desc : ���� ������ �۽�
 parm : data	- [in]  ������ �����Ͱ� ����� ���� ������
		size	- [in]  'data' ���ۿ� ����� �������� ũ��
		time_out- [in]  Timeout �� ����. ����: �и��� (0�̸�, ��� ����)
 retn : TRUE - �۽� ����(��� �۽ŵ� �����Ͱ� ������),
		FALSE - �۽� ����(���� ����... ������ �����ؾ� ��)
*/
BOOL CGFSSThread::SendData(PUINT8 data, UINT32 size, UINT64 time_out)
{
	BOOL bSucc			= FALSE;
	TCHAR tzMesg[128]	= {NULL};

	/* ������ �۽� �� ���� ���� */
	m_enSocket	= ENG_TPCS::en_sending;
	if (0 == time_out)	bSucc = ReSended(data, size, UINT32(3), UINT32(100));
#ifdef _DEBUG
	else				bSucc = ReSended(data, size, UINT32_MAX);
#else
	else				bSucc = ReSended(data, size, time_out);
#endif
	if (!bSucc)
	{
		swprintf_s(tzMesg, 128, L"Failed to send the data (size = %d) (sock_err = %d)",
				   size, WSAGetLastError());
		m_enLastErrorLevel	= ENG_LNWE::en_error;
		LOG_ERROR(ENG_EDIC::en_gfss, tzMesg);
		m_enSocket	= ENG_TPCS::en_send_failed;
	}
	else
	{
		m_enSocket	= ENG_TPCS::en_sended;
	}

	return bSucc;
}

/*
 desc : ���� ��Ŷ ����
 parm : cmd		- [in]  ��Ŷ ��ɾ�
		data	- [in]  ��Ŷ ���� �����Ͱ� ����� ����
		size	- [in]  'data' ���� �� ����� ������ ���� ũ��
		time_out- [in]  Timeout �� ����. ����: �и��� (0�̸�, ��� ����)
 retn : TRUE or FALSE
*/
BOOL CGFSSThread::SendData(ENG_PCGG cmd, PUINT8 data, UINT32 size, UINT64 time_out)
{
	UINT32 u32Size	= sizeof(STG_UTPH) + size + sizeof(STG_UTPT);	/* ��ü �۽ŵ� ��Ŷ ũ�� ���� */
	STG_UTPH stHead	= {NULL};
	STG_UTPT stTail	= {NULL};
	PUINT8 pPktBuff	= (PUINT8)::Alloc(u32Size+1), pPktData;
	ASSERT(pPktBuff);
	pPktData	= pPktBuff;

	/* �޸� �� �κ� �ʱ�ȭ */
	pPktBuff[u32Size]	= 0x00;

	/* ��Ŷ ��� �� ���� �κ� �⺻ �� ���� */
	stHead.head		= SWAP16(UINT16(ENG_PHSI::en_head_gfss_vs_gfsc));
	stHead.cmd		= SWAP16(UINT16(cmd));
	stHead.length	= SWAP32(u32Size);
	stTail.crc		= 0x0000;
	stTail.tail		= SWAP16(UINT16(ENG_PHSI::en_comn_tail));

	/* ��Ŷ �۽� ũ�� Ȯ�� */
	memcpy(pPktData, &stHead, sizeof(STG_UTPH));			pPktData += sizeof(STG_UTPH);
	if (data && size > 0)	memcpy(pPktData, data, size);	pPktData += size;
	memcpy(pPktData, &stTail, sizeof(STG_UTPT));

	/* �۽� ��Ŷ ��� */
	stHead.SwapNetworkToHost();
	PushPktHead(&stHead);

	return SendData(pPktBuff, u32Size, time_out);
}

/*
 desc : �����Ͱ� ���ŵ� ���, ȣ���
 parm : None
 retn : TRUE - ���������� �м� (Ring Buffer) ó��
		FALSE- �м� ó������ ���� (���� �Ϻ����� ���� ��Ŷ��)
*/
BOOL CGFSSThread::RecvData()
{
	BOOL bIsTimeout	= FALSE;
	BOOL bRecvState	= TRUE;
	BOOL bRecvAlive	= FALSE;	/* Alive Check Packet ���� ����  */
	UINT32 u32Size	= 0, u32Read;
	STG_UTPH stHead	= {NULL};
	PUINT8 pPktBuff	= NULL;

	/* ���� ���� �ð� �ʱ�ȭ */
	uvGFSP_ResetLoopTime();

	/* Recv to packet */
	m_enSocket	= ENG_TPCS::en_recving;


	while (1)
	{
		/* ���� ��뷮(?) ���� ������ ���� */
		pPktBuff= m_pRecvBuff;

		/* ���� ���ۿ� ���� �ִ� �������� ũ�� Ȯ�� */
		u32Read	= m_pPktRingBuff->GetReadSize();

		/* ���� ���ŵ� �����Ͱ� �⺻ ��� ���� ũ�⸸ŭ ����Ǿ� �ִ��� Ȯ�� */
		if (u32Read < sizeof(STG_UTPH))	break;	// !!! ���� �ƴ� !!!

		/* ���ۿ� ������ ��Ŷ�� ��� ����Ǿ� �ִ��� Ȯ�� or �����ϴٸ� ��� */
		m_pPktRingBuff->CopyBinary((PUINT8)&stHead, sizeof(STG_UTPH));
		/* ���� ���ŵ� �������� ũ�� */
		u32Size = SWAP32(stHead.length) - sizeof(STG_UTPH);
		/* �Ź� ���� ũ�⸸ŭ ����Ǿ� �ִ��� Ȯ�� */
		if (u32Read < u32Size)	break;	// !!! ���� �ƴ� !!!
		/* ���� ������ Copy�� ���� ��� �κ��� ���۷κ��� ���� �� ���� */
		m_pPktRingBuff->PopBinary(sizeof(STG_UTPH));
		/* ��� �κ��� ������ �������� ����Ǿ� ũ�� �ٽ� ��� */
		memcpy(pPktBuff, &stHead, sizeof(STG_UTPH));
		pPktBuff+= sizeof(STG_UTPH);

		/* �� ���ŵǾ���� ��ŭ �����Ͱ� �� ���ۿ� ����Ǿ� �ִ��� �б� ��û */
		/* u32Size�� ũ�Ⱑ 0 Bytes �� ���, ���� �����Ͱ� ���ٰ� �Ǵ� �Ѵ�. */
		if (u32Size)	m_pPktRingBuff->ReadBinary(pPktBuff, u32Size);

		/* Alive Check Packet �������� ���� */
		if (SWAP16(stHead.cmd) == (UINT16)ENG_PCUC::en_comn_alive_check)	bRecvAlive	= TRUE;
		else
		{
			/* ���ŵ� ������ ���� */
			bRecvState	= RecvData(m_pRecvBuff, u32Size+sizeof(STG_UTPH));
			/* ���ŵ� ��Ŷ ��� (��ɾ�) �κ� �ӽ� ���� */
			if (bRecvState)
			{
				stHead.SwapNetworkToHost();
				PushPktHead(&stHead);
			}
		}
		/* ���� ������ ��� ������ ���� Ȯ�� */
#if (USE_CHECK_LIMIT_LOOP_TIME)
		if (uvGFSP_IsLoopTimeOut(WFILE, WLINE))	break;
#else
		if (uvGFSP_IsLoopTimeOut())	break;
#endif
		{
			LOG_ERROR(ENG_EDIC::en_gfss, L"GFSS : A timeout occurred while receiving data");
			bIsTimeout	= TRUE;
			break;
		}
	}

	/* Alive Check�� ���� ���� �۽� */
	if (bRecvAlive)	bRecvState = SendAliveTime();

	/* Recv to packet */
	if (!bIsTimeout)	m_enSocket	= ENG_TPCS::en_recved;
	else				m_enSocket	= ENG_TPCS::en_recv_failed;

	return bRecvState;
}

/*
 desc : ���̺귯�� ������ ó��
 parm : data	- [in]  �̺�Ʈ �����Ͱ� ����� ���� ������
		size	- [in]  'data' ���ۿ� ����� �������� ũ��
 retn : TRUE or FALSE (���� ����? �߸��� �������� ������ ���ŵ�)
*/
BOOL CGFSSThread::RecvData(PUINT8 data, UINT32 size)
{
	BOOL bSucc		= FALSE;
	UINT32 u32Body	= size - (sizeof(STG_UTPH) + sizeof(STG_UTPT));
	PUINT8 pPktBody	= data + sizeof(STG_UTPH);
	LPG_UTPH pstHead= LPG_UTPH(data);

	/* packet byte order */
	pstHead->SwapNetworkToHost();

	switch (pstHead->cmd)
	{
	case ENG_PCGG::en_pcgg_new_gerber_path	:	bSucc = NewGerberPath(pPktBody, u32Body);	break;
	case ENG_PCGG::en_pcgg_del_gerber_path	:	bSucc = DelGerberPath(pPktBody, u32Body);	break;
	case ENG_PCGG::en_pcgg_cnt_gerber_files	:	bSucc = SetGerberCount(pPktBody, u32Body);	break;
	case ENG_PCGG::en_pcgg_send_gerber_files:	bSucc = SetGerberFiles(pPktBody);			break;
	case ENG_PCGG::en_pcgg_send_gerber_comp	:	bSucc = SetGerberComp();					break;
	case ENG_PCGG::en_pcgg_get_recipe_files	:	bSucc = GetRecipeFiles();					break;
	case ENG_PCGG::en_pcgg_set_recipe_files	:	bSucc = SetRecipeFiles(pPktBody, u32Body);	break;
	}

	return bSucc;
}

/*
 desc : �Ź� ���� (�Ź� ���� �� ��Ÿ ���� ����) �� ������ ��� �� ����
 parm : data	- [in]  ��Ŷ ���� (head & tail ����) ������ ����� ���� ������
		size	- [in]  'data' ���ۿ� ����� ��Ŷ�� ũ��
 retn : TRUE or FALSE
*/
BOOL CGFSSThread::NewGerberPath(PUINT8 data, UINT32 size)
{
	UINT8 u8Flag		= 0x02;	/* ���� ���� ���з� �ʱ� �� ���� */
	CHAR szGPath[GERBER_PATH_NAME]	= {NULL};	/* �Ź� (�Ź� ���� �� ��Ÿ ���� ����) ���� �� */
	TCHAR tzMesg[128]	= {NULL};
	CUniToChar csCnv;

	/* �ϴ�, �⺻ ������ ���� ������ ���� */
	m_enLastErrorLevel	= ENG_LNWE::en_error;

	if (size > GERBER_PATH_NAME /* Max Gerber Path */)
	{
		swprintf_s(tzMesg, 128, L"Invalid packet information [cmd: generate gerber path]");
		if (wcslen(tzMesg))	LOG_SAVED(ENG_EDIC::en_gfss, m_enLastErrorLevel, tzMesg);
	}
	else
	{
		/* Gerber Name ���� */
		memcpy_s(szGPath, GERBER_PATH_NAME, data, size);
#if 0
		/* ��ü ��� ���� : �⺻ ��� + �Ź� �̸� (��) */
		swprintf_s(tzGPath, MAX_PATH_LEN, L"%s\\%s", m_pstConfig->gerber_path->gfss, csCnv.Ansi2Uni(szGPath));
		/* ���� �Ź� (�Ź� ���� �� ��Ÿ ���� ����) ������ �����ϸ�, ������ ���� */
		if (uvCmn_FindPath(tzGPath))
		{
			if (!uvCmn_RemoveDirectory(tzGPath))	u8Flag	= 0x01;	/* ���� �Ź� ������ �ְ�, �� ������ ����µ� ������ ��� */
		}
		/* ���ο� �Ź� (�Ź� ���� �� ��Ÿ ���� ����) ��� (���丮) ���� */
		if (0x00 == u8Flag)
		{
			/* ���� ���丮�� ������, �˾Ƽ� ������ */
			if (!uvCmn_CreateDirectory(tzGPath))	u8Flag	= 0x02;	/* �Ź� (�Ź� ���� �� ��Ÿ ���� ����) ��ο� Ư�� ���� �� ��Ÿ ��ȣ�� ���Ե� ��� */
		}

		/* �α� ��� */
		switch (u8Flag)
		{
		case 0x00 :	m_enLastErrorLevel	= ENG_LNWE::en_normal;
					swprintf_s(tzMesg, LOG_MESG_SIZE, L"Succeeded to create the gerber directory [%s]", csCnv.Ansi2Uni(szGPath));	break;
		case 0x01 :	swprintf_s(tzMesg, LOG_MESG_SIZE, L"Failed to delete the gerber directory [%s]", csCnv.Ansi2Uni(szGPath));		break;
		case 0x02 :	swprintf_s(tzMesg, LOG_MESG_SIZE, L"Failed to create the gerber directory [%s]", csCnv.Ansi2Uni(szGPath));		break;
		}
#else
		u8Flag	= NewGerberPath(csCnv.Ansi2Uni(szGPath));
#endif
	}

	if (wcslen(tzMesg))	LOG_SAVED(ENG_EDIC::en_gfss, m_enLastErrorLevel, tzMesg);

	/* ���� ��Ŷ ���� */
	return SendData(ENG_PCGG::en_pcgg_new_gerber_path_ack, &u8Flag, 1);
}

/*
 desc : �Ź� ������ ������ ��� �� ����
 parm : data	- [in]  ��Ŷ ���� (head & tail ����) ������ ����� ���� ������
		size	- [in]  'data' ���ۿ� ����� ��Ŷ�� ũ��
 retn : TRUE or FALSE
*/
BOOL CGFSSThread::DelGerberPath(PUINT8 data, UINT32 size)
{
	UINT8 u8Flag	= 0x00;
	CHAR szGPath[GERBER_PATH_NAME]	= {NULL};	/* �Ź� ���� �� */
	TCHAR tzPath[MAX_PATH_LEN]		= {NULL};	/* ��ü ��� (����̺� ����) */
	TCHAR tzMesg[LOG_MESG_SIZE]		= {NULL};
	CUniToChar csCnv;

	/* �ϴ�, �⺻ ������ ���� ������ ���� */
	m_enLastErrorLevel	= ENG_LNWE::en_error;

	if (size > GERBER_PATH_NAME /* Max Gerber Path */)
	{
		swprintf_s(tzMesg, LOG_MESG_SIZE, L"Invalid packet information [cmd: delete gerber path]");
	}
	else
	{
		/* Gerber Name ���� */
		memcpy_s(szGPath, GERBER_PATH_NAME, data, size);
		/* ��ü ��� ���� : �⺻ ��� + �Ź� �̸� (��) */
		swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\%s",
				   m_pstConfig->gerber_path->gfss, csCnv.Ansi2Uni(szGPath));
		/* ���� �Ź� ������ �����ϸ�, ������ ���� */
		if (!uvCmn_FindPath(tzPath))				u8Flag	= 0x01;	/* ��� (���丮)�� �������� ���� */
		else if (!uvCmn_RemoveDirectory(tzPath))	u8Flag	= 0x02;	/* ���� �Ź� ������ �ְ�, �� ������ ����µ� ������ ��� */

		/* �α� ��� */
		switch (u8Flag)
		{
		case 0x00 :	m_enLastErrorLevel	= ENG_LNWE::en_normal;
					swprintf_s(tzMesg, LOG_MESG_SIZE, L"Succeeded to delete the gerber directory [%s]", csCnv.Ansi2Uni(szGPath));	break;
		case 0x01 :	swprintf_s(tzMesg, LOG_MESG_SIZE, L"Failed to find the gerber directory [%s]", csCnv.Ansi2Uni(szGPath));		break;
		case 0x02 :	swprintf_s(tzMesg, LOG_MESG_SIZE, L"Failed to delete the gerber directory [%s]", csCnv.Ansi2Uni(szGPath));		break;
		}
	}

	if (wcslen(tzMesg))	LOG_SAVED(ENG_EDIC::en_gfss, m_enLastErrorLevel, tzMesg);

	/* ���� �ֱٿ� ������ ��� �� ���� */
	wmemset(m_tzLastDelDir, 0x00, MAX_PATH_LEN);
	wcscpy_s(m_tzLastDelDir, MAX_PATH_LEN, csCnv.Ansi2Uni(szGPath));

	/* ���� ��Ŷ ���� */
	return SendData(ENG_PCGG::en_pcgg_del_gerber_path_ack, &u8Flag, 1);
}

/*
 desc : �� ���� ���� �Ź� ������ ����
 parm : data	- [in]  ��Ŷ ���� (head & tail ����) ������ ����� ���� ������
		size	- [in]  'data' ���ۿ� ����� ��Ŷ�� ũ��
 retn : TRUE or FALSE
*/
BOOL CGFSSThread::SetGerberCount(PUINT8 data, UINT32 size)
{
	TCHAR tzMesg[128]	= {NULL};
	UINT32 u32SendTotal	= 0;

	/* �ϴ�, �⺻ ������ ���� ������ ���� */
	m_enLastErrorLevel	= ENG_LNWE::en_error;

	/* ������ ���Ź��� �Ź� ������ ���� �ʱ�ȭ */
	m_u32TotalFiles		= 0;
	m_u32FileCount		= 0;
	m_u32TotalSizes		= 0;
	m_u32TotalZipSize	= 0;
	if (size == 4)
	{
		/* �Ź� ������ ���� ���� */
		memcpy(&m_u32TotalFiles, data, size);
		u32SendTotal		= m_u32TotalFiles;
		m_u32TotalFiles		= SWAP32(u32SendTotal);
		m_enLastErrorLevel	= ENG_LNWE::en_normal;
	}
	swprintf_s(tzMesg, 128, L"The number of gerber files to receive is [ %d ]", m_u32TotalFiles);
	LOG_SAVED(ENG_EDIC::en_gfss, m_enLastErrorLevel, tzMesg);

	/* ���� ��Ŷ ���� */
	return SendData(ENG_PCGG::en_pcgg_cnt_gerber_files_ack, (PUINT8)&u32SendTotal, 4);
}

/*
 desc : �Ź� ������ ���ŵ� ���
 parm : data	- [in]  ��Ŷ ���� (head & tail ����) ������ ����� ���� ������
		size	- [in]  'data' ���ۿ� ����� ��Ŷ�� ũ��
 retn : TRUE or FALSE
*/
BOOL CGFSSThread::SetGerberFiles(PUINT8 data)
{
	CHAR szMesg[LOG_MESG_SIZE]	= {NULL};
	UINT8 u8GFileFlag	= 0x00;
	UINT32 u32GFile		= 0;
	size_t lGUnzipedSize, lGUnzipReserved;
	snappy_status enStatus;
	PUINT8 pData		= data;
	PUINT8 pGUnzipOrg, pGUnzipData;
	STG_PGFP stGFile	= {NULL};
	LPG_CSID pstConfig	= uvGFSP_Info_GetData();
	CUniToChar csCnv1, csCnv2;

	/* ���ŵ� ������ �Ź� ���� ��ȣ */
	memcpy(&u32GFile, pData, 4);		pData	+= 4;
	/* ���ŵ� �Ź� ������ ó�� / �߰� / �� �κ� �� ���? */
	u8GFileFlag	= pData[0];				pData	+= 1;
	/* ����� ������ ũ�� */
	memcpy (&lGUnzipedSize, pData, 4);	pData	+= 4;		/* SWAP32 ȿ�� */
	/* �� ��ȯ */
	u32GFile		= SWAP32(u32GFile);
	lGUnzipedSize	= SWAP32(lGUnzipedSize);
	/* �Ź� ���� ���� ���� �ð� �ʱ�ȭ */
	if (u32GFile == 1)
	{
		m_u64RecvStartTime	= GetTickCount64();
		m_bIsRecvCompleted	= FALSE;
	}

	/* �ϴ�, �⺻ ������ ���� ������ ���� */
	m_enLastErrorLevel	= ENG_LNWE::en_error;

	/* ���� ���� ���ο� ���� */
	lGUnzipReserved	= lGUnzipedSize;				/* �ϴ�, �������� �ʾҴٴ� �����Ͽ� �Ź� ���� ũ�� �ӽ� ���� */
	m_u32TotalZipSize	+= (UINT32)lGUnzipedSize;	/* ������� ���ŵ� ����� ������ �� ũ�� (����: Bytes) */
	if (pstConfig->common->use_zip_file)
	{
		/* ���� ���� ���� ���� ũ�� */
		enStatus	= snappy_uncompressed_length((PCHAR)pData, lGUnzipedSize, &lGUnzipReserved);
		if ((enStatus != SNAPPY_OK) || (lGUnzipReserved < 1))
		{
			LOG_SAVED(ENG_EDIC::en_gfss, m_enLastErrorLevel, L"Invalid zip file size");
			return FALSE;
		}
	}

	/* ���� ���� �� ����� ���� �޸� �Ҵ� */
	pGUnzipOrg	= (PUINT8)::Alloc(lGUnzipReserved+1);
	memset(pGUnzipOrg, 0x00, lGUnzipReserved+1);
	if (pstConfig->common->use_zip_file)
	{
		/* ���� ���� */
		enStatus	= snappy_uncompress((PCHAR)pData, lGUnzipedSize, (PCHAR)pGUnzipOrg, &lGUnzipReserved);
		if ((enStatus != SNAPPY_OK) || (lGUnzipReserved < 1))
		{
			LOG_ERROR(ENG_EDIC::en_gfss, L"Failed to decompress the gerber file");
			return FALSE;
		}
	}
	/* �������� ���� ���, ���Ź��� ���� �Ź� ���� ���� �ӽ� ���ۿ� ���� ���� */
	else
	{
		memcpy(pGUnzipOrg, pData, lGUnzipReserved);
	}

	/* �ӽ� ������ ���� */
	pGUnzipData	= pGUnzipOrg;
	/* �Ź� ���� ���� �κ� �� ��� ���� �κ� ���� */
	memcpy(&stGFile, pGUnzipData, sizeof(STG_PGFP));	pGUnzipData	+= sizeof(STG_PGFP);

	/* ��� ���� Network Byte Order ��ȯ */
	stGFile.SwapData();
	sprintf_s(szMesg, LOG_MESG_SIZE, "[%02d] [%s] [%s] [%d bytes]",
			  stGFile.file_no, stGFile.path, stGFile.file, stGFile.s_size);
	LOG_MESG(ENG_EDIC::en_gfss, csCnv1.Ansi2Uni(szMesg));

#ifdef _DEBUG
	TRACE(L"Recv file number = [%d] %d [%s]\n",
		  u8GFileFlag, u32GFile, csCnv2.Ansi2Uni(stGFile.file));
#endif

	/* �Ź� ���� ���� ũ�� ���ϱ� */
	lGUnzipReserved	-= sizeof(STG_PGFP);
	if (lGUnzipReserved != stGFile.s_size)
	{
		sprintf_s(szMesg, LOG_MESG_SIZE, "The original and uncompressed files are different sizes "
										 "Source[%I32u] / Unzip[%d]",
				  stGFile.s_size, (INT32)lGUnzipReserved);
		LOG_ERROR(ENG_EDIC::en_gfss, csCnv1.Ansi2Uni(szMesg));
		m_enLastErrorLevel	= ENG_LNWE::en_error;
		return FALSE;
	}

	/* ������ ó������ �ƴ��� ���ο� ���� ���� �������� ���� Ȯ�� */
	if (u8GFileFlag == 0x00)
	{
		if (0X00 != NewGerberPath(csCnv1.Ansi2Uni(stGFile.path)))
		{
			return FALSE;
		}
	}
	/* �Ź� ���� ���� */
	if (!SaveGerberFile(csCnv1.Ansi2Uni(stGFile.path), csCnv2.Ansi2Uni(stGFile.file),
						pGUnzipData, (UINT32)lGUnzipReserved, u32GFile))
	{
		return FALSE;
	}

	/* ���������� ��� ó���� ���, ���� ���ٰ� �÷��� ���� */
	m_enLastErrorLevel	= ENG_LNWE::en_normal;

	return TRUE;
}

/*
 desc : ���ο� �Ź� (�Ź� ���� Ȥ�� ���� ��Ÿ ����) ���� ����
 parm : path	- [in]  ���� ������ �Ź� �� (��ü ��� ������)
 retn : 0x00 - ���ο� �Ź� ���� ���� ����
		0x01 - ���� �Ź� ���� ���� ����
		0x02 - ���ο� �Ź� ���� ���� ����
*/
UINT8 CGFSSThread::NewGerberPath(PTCHAR path)
{
	UINT8 u8Flag		= 0x00;
	TCHAR tzMesg[LOG_MESG_SIZE]	= {NULL}, tzGPath[MAX_PATH_LEN] = {NULL};
	LPG_CSID pstConfig	= uvGFSP_Info_GetData();

	/* ���� �����Ϸ��� ���� ���� ��, ���� ���� ��ġ�� �����ϴ��� Ȯ�� */
	swprintf_s(tzGPath, MAX_PATH_LEN, L"%s\\%s", pstConfig->gerber_path->gfss, path);
#if 0
	/* ���� �����Ϸ��� ���� �˻� ��, �����ϸ�, ���� ���� ���� */
	if (!uvCmn_FindPath(tzGPath))
	{
		if (!uvCmn_CreateDirectory(tzGPath))
		{
			swprintf_s(tzMesg, LOG_MESG_SIZE, L"Failed to create the gerber path [%s]", tzGPath);
			enLevel	= ENG_LNWE::en_error;
		}
		else
		{
			swprintf_s(tzMesg, LOG_MESG_SIZE, L"Succeeded to create the gerber path [%s]", tzGPath);
		}
	}
	else
	{
		swprintf_s(tzMesg, LOG_MESG_SIZE, L"This directory already exists [%s]", tzGPath);
	}
#else
	/* ���� �Ź� (�Ź� ���� �� ��Ÿ ���� ����) ������ �����ϸ�, ������ ���� */
	if (uvCmn_FindPath(tzGPath))
	{
		if (!uvCmn_RemoveDirectory(tzGPath))	u8Flag	= 0x01;	/* ���� �Ź� ������ �ְ�, �� ������ ����µ� ������ ��� */
	}
	/* ���ο� �Ź� (�Ź� ���� �� ��Ÿ ���� ����) ��� (���丮) ���� */
	if (0x00 == u8Flag)
	{
		/* ���� ���丮�� ������, �˾Ƽ� ������ */
		if (!uvCmn_CreateDirectory(tzGPath))	u8Flag	= 0x02;	/* �Ź� (�Ź� ���� �� ��Ÿ ���� ����) ��ο� Ư�� ���� �� ��Ÿ ��ȣ�� ���Ե� ��� */
	}

	/* �α� ��� */
	switch (u8Flag)
	{
	case 0x00 :	m_enLastErrorLevel	= ENG_LNWE::en_normal;
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"Succeeded to create the gerber directory [%s]", tzGPath);	break;
	case 0x01 :	swprintf_s(tzMesg, LOG_MESG_SIZE, L"Failed to delete the gerber directory [%s]", tzGPath);		break;
	case 0x02 :	swprintf_s(tzMesg, LOG_MESG_SIZE, L"Failed to create the gerber directory [%s]", tzGPath);		break;
	}
	if (wcslen(tzMesg))	LOG_SAVED(ENG_EDIC::en_gfss, m_enLastErrorLevel, tzMesg);
#endif
	/* ���� �ֱٿ� ������ ��� �� ���� */
	wmemset(m_tzLastNewDir, 0x00, MAX_PATH_LEN);
	wcscpy_s(m_tzLastNewDir, MAX_PATH_LEN, tzGPath);

	return u8Flag;
}

/*
 desc : ���ο� �Ź� (�Ź� ���� Ȥ�� ���� ��Ÿ ����) ���� ����
 parm : path	- [in]  ���ο� �Ź� ������ ����� ���
		file	- [in]  �Ź� (�Ź� ���� Ȥ�� ���� ��Ÿ ����) ���� (Ȯ���� ����)
		data	- [in]  �Ź� ������ ������ ����� ���� ������
		size	- [in]  'data' ���ۿ� ����� ������ ũ��
		f_no	- [in]  ���� ����Ǵ� ������ ��ȣ
 retn : TRUE or FALSE
*/
BOOL CGFSSThread::SaveGerberFile(PTCHAR path, PTCHAR file, PUINT8 data, UINT32 size, UINT32 f_no)
{
	TCHAR tzMesg[128]	= {NULL}, tzGFile[MAX_PATH_LEN] = {NULL};
	ENG_LNWE enLevel	= ENG_LNWE::en_normal;
	LPG_CSID pstConfig	= uvGFSP_Info_GetData();
	CUniToChar csCnv;

	/* ���� �����Ϸ��� ���� ����, ���� �� �ݱ� */
	swprintf_s(tzGFile, MAX_PATH_LEN, L"%s\\%s\\%s", pstConfig->gerber_path->gfss, path, file);
	if (!uvCmn_SaveBinFile(data, size, csCnv.Uni2Ansi(tzGFile)))
	{
		swprintf_s(tzMesg, 128, L"Failed to save the gerber file [%d]", f_no);
		enLevel	= ENG_LNWE::en_error;
	}
	else
	{
		swprintf_s(tzMesg, 128, L"Succeeded to save the gerber file [%d]", f_no);
	}
	/* ���� ���Ź��� �Ź� ������ ���� (��ȣ) �� �ӽ� ���� */
	m_u32FileCount	= f_no;
	/* ���ݱ��� ���Ź��� �Ź� ���� (�Ź� ���� ��Ÿ ���� ����)�� ��ü ũ�� (����: Bytes) ��� */
	m_u32TotalSizes	+= size;

	/* ���� �ֱٿ� ����� ��� �� ���� */
	wmemset(m_tzLastSaveDir, 0x00, MAX_PATH_LEN);
	wcscpy_s(m_tzLastSaveDir, MAX_PATH_LEN, path);

	LOG_SAVED(ENG_EDIC::en_gfss, enLevel, tzMesg);

	return TRUE;
}

/*
 desc : ��� �Ź� ���ϵ��� ���� �Ϸ�� ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CGFSSThread::SetGerberComp()
{
	LOG_MESG(ENG_EDIC::en_gfss, L"Completed to receive the gerber files");

	/* ���� �ֱٿ� ���Ź޴µ� �� �ҿ�� �ð� ���� (����: msec) */
	m_u64RecvTotalTime	= GetTickCount64() - m_u64RecvStartTime;
	m_bIsRecvCompleted	= TRUE;
	/* ���� �ƹ��� �۾��� ���� (����μ� ���� �� �ʿ䰡 ����) */
	return SendData(ENG_PCGG::en_pcgg_send_gerber_comp_ack, NULL, 0);
}

/*
 desc : Client�κ��� Server�� ���� (����)�� ������ ������ ��û ���� ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CGFSSThread::GetRecipeFiles()
{
	BOOL bSended		= FALSE;
	LPG_CSBP pstRecipe	= uvGFSP_Info_GetData()->recipe_path;
	CStringArray arrRecipeFiles;

	LOG_MESG(ENG_EDIC::en_gfss, L"Request to recipe files");

	/* Server ���� Recipe Files �˻� */
	uvCmn_SearchRecurseFile(pstRecipe->gfss, L".dat", arrRecipeFiles, 0x00);
	/* ���� �˻��� �����ǰ� ���ٸ� ... */
	if (arrRecipeFiles.GetCount() < 1)
	{
		UINT16 u16Count	= 0x0000;
		bSended	= SendData(ENG_PCGG::en_pcgg_get_recipe_files_ack, (PUINT8)&u16Count, 2);
	}
	else
	{
		bSended	= SetRecipeFiles(&arrRecipeFiles);
	}

	return bSended;
}

/*
 ����: Server�κ��� ���ŵ� ������ ���� ����
 parm : data	- [in]  ��Ŷ ���� (head & tail ����) ������ ����� ���� ������
		size	- [in]  'data' ���ۿ� ����� ��Ŷ�� ũ��
 retn : TRUE or FALSE
*/
BOOL CGFSSThread::SetRecipeFiles(PUINT8 data, UINT32 size)
{
	CHAR szFile[RECIPE_FILE_NAME]	= {NULL};
	CHAR szRecipe[MAX_PATH_LEN]		= {NULL};
	UINT8 u8Mode	= 0x00,	/* 0x00 : Text File, 0x01 : Binary File */ u8Flag = 0x00;
	UINT16 u16Count	= 0x0000, i		= 0;
	UINT32 u32Size	= 0;
	PUINT8 pPktBuff	= data, pPktData = pPktBuff;
	LPG_CSBP pstRecipe	= uvGFSP_Info_GetData()->recipe_path;
	CUniToChar csCnv;

	/* �߸��� ��Ŷ ���� */
	if (size < 2)	return FALSE;

	/* ������ ����Ǿ� �ִ� ���� ��� ���� */
	uvCmn_DeleteAllFiles(pstRecipe->gfss);

	/* �ϴ� ���ŵ� ���� ���� Ȯ�� */
	memcpy(&u16Count, pPktData, 2);	pPktData += 2;
	u16Count	= SWAP16(u16Count);
	/* ���ŵ� ���� ���� */
	m_u32TotalFiles	= u16Count;
	m_u32TotalSizes	= 0;
	if (u16Count > 0)
	{
		u8Flag	= 0x01;	/* ������Ʈ ���� */
		/* ���� ������ŭ �ݺ� */
		for (i=0; i<u16Count; i++)
		{
			/* ������ ���� ��� */
			memset(szFile, 0x00, RECIPE_FILE_NAME);
			memcpy(szFile, pPktData, RECIPE_FILE_NAME);				pPktData += RECIPE_FILE_NAME;
			sprintf_s(szRecipe, MAX_PATH_LEN, "%s\\%s", csCnv.Uni2Ansi(pstRecipe->gfss), szFile);
			/* ������ ���� ũ�� ��� */
			memcpy(&u32Size, pPktData, 4);							pPktData += 4;
			u32Size	= SWAP32(u32Size);
			/* �� ���ŵ� ���� ũ�� ���� */
			m_u32TotalSizes	+= u32Size;
			/* ���� ���� ��� */
			u8Mode	= pPktData[0];									pPktData += 1;
			/* ���� ���� */
			uvCmn_SaveBinFile(pPktData, u32Size, szRecipe);			pPktData += u32Size;
		}
	}

	/* ���� �۽� */
	return SendData(ENG_PCGG::en_pcgg_set_recipe_files_ack, &u8Flag, 1);
}

/*
 desc : Client�κ��� ���ŵ� ��Ŷ ��� �κ� �ӽ� ��� (App. �ʿ��� � ��ɾ���� ���ŵǾ����� �˱� ����)
 parm : head	- [in]  ���ŵ� ��Ŷ ��� ������ ����ü ������
 retn : None
*/
VOID CGFSSThread::PushPktHead(LPG_UTPH head)
{
	/* ����ȭ ���� */
	if (m_syncPktCmd.Enter())
	{
		/* ������ ��ϵ� ���� (����)�� �ʰ��Ǹ� ���� ������ �׸� ���� */
		if (m_lstPktCmd.GetCount() > MAX_PKT_LAST_RECV_SIZE)
			m_lstPktCmd.RemoveHead();
		/* ���� �߻��� ��Ŷ ��� ��� */
		m_lstPktCmd.AddTail(*head);

		/* ����ȭ ���� */
		m_syncPktCmd.Leave();
	}
}

/*
 desc : Client�κ��� ���ŵ� ��Ŷ ��� �κ� ��ȯ
 parm : head	- [out] ��ȯ�Ǿ� ����� ��Ŷ ��� ����ü ����
 retn : TRUE (��ȯ�� ������ ����) or FALSE (��ȯ�� ��Ŷ ��� ����)
*/
BOOL CGFSSThread::PopPktHead(STG_UTPH &head)
{
	BOOL bPopped	= FALSE;

	/* ����ȭ ���� */
	if (m_syncPktCmd.Enter())
	{
		if (m_lstPktCmd.GetCount() > 0)
		{
			/* ���� ������ ��Ŷ ��� ������ Popped !!! */
			head = m_lstPktCmd.GetHead();
			/* ���� ������ ��Ŷ ��� ���� */
			m_lstPktCmd.RemoveHead();
			/* ���������� Popped �ߴٰ� �÷��� ���� */
			bPopped	= TRUE;
		}

		/* ����ȭ ���� */
		m_syncPktCmd.Leave();
	}

	return bPopped;
}

/*
 desc : ������� �Ź� ���� (�Ź� ���� Ȥ�� �Ź� ���� ��Ÿ ����)�� ������ ���� �� ��ȯ
 parm : None
 retn : ���ŷ� ��
*/
DOUBLE CGFSSThread::GetTotalFilesRate()
{
	if (m_u32FileCount < 1 || m_u32TotalFiles < 1)	return 0.0f;
	return (DOUBLE(m_u32FileCount) / DOUBLE(m_u32TotalFiles) * 100.0f);
}

/*
 desc : ���� �ֱٿ� ���� / ���� / ���� (�Ź� ���� ���ϵ�) �� ���� ��ȯ
 parm : type	- [in]  0x00:����, 0x01:����, 0x02:����
 retn : ���� (��ü ��� ������)
*/
PTCHAR CGFSSThread::GetLastDir(UINT8 type)
{
	switch (type)
	{
	case 0x00	:	return m_tzLastNewDir;
	case 0x01	:	return m_tzLastDelDir;
	case 0x02	:	return m_tzLastSaveDir;
	}
	return L"";
}

/*
 desc : Server�� ����� ������ ���� �۽�
 parm : files	- [in]  ���� �۽ŵ� ������ ���ϸ��� ����Ǿ� �ִ� ���ڿ� �迭 ����
 retn : TRUE or FALSE
*/
BOOL CGFSSThread::SetRecipeFiles(CStringArray *files)
{
	BOOL bSended	= TRUE;
	UINT16 u16Count	= 0, i;
	UINT32 u32Total	= 0, u32Size, u32Read, u32Recipe;
	PUINT8 pPktBuff	= NULL, pPktData = NULL;
	PCHAR pszRecipe	= NULL;
	CAtlList <UINT32> lstFileSize;
	CUniToChar csCnv;

	m_u32TotalFiles	= 0;
	m_u32FileCount	= 0;
	m_u32TotalSizes	= 0;

	/* �迭�� ����� ������ ���ϵ��� ��ü ũ�� ���ϱ� */
	u16Count		= (UINT16)files->GetCount();
	m_u32TotalFiles	= u16Count;
	for (i=0; i<u16Count; i++)
	{
		/* ���� ������ ũ�� ��� (����: Bytes) */
		u32Size	= uvCmn_GetFileSize(CString(files->GetAt(i)).GetBuffer(), L"rt");
		/* ���� ������ ũ�� �ӽ� ���� */
		lstFileSize.AddTail(u32Size);

		/* �� �Ҵ�� ��Ŷ ���� ũ�⸦ ���ϱ� ���ؼ� */
		u32Total+= u32Size;
		u32Total+= RECIPE_FILE_NAME;
		u32Total+= 4;	/* recipe file size */
		u32Total+= 1;	/* file mode: 0x00 (text file) or 0x01 (binary file) */

		/* ��ü �۽ŵ� ���� ������ �� ũ�� */
		m_u32TotalSizes	+= u32Size;
	}

	/* ������ �⺻ ��� ���� */
	u32Recipe	= (UINT32)wcslen(uvGFSP_Info_GetData()->recipe_path->gfss);
	/* ��ü ������ �۽��� ���� �Ҵ� */
	pPktBuff	= (PUINT8)::Alloc(u32Total+1);
	memset(pPktBuff, 0x00, u32Total+1);
	pPktData	= pPktBuff;

	/* ��Ŷ ���� */
	u16Count	= SWAP16(u16Count);
	memcpy(pPktData, &u16Count, 2);	pPktData	+= 2;
	for (i=0; i<SWAP16(u16Count); i++)
	{
		/* ������ ���� (���� ����) */
		pszRecipe	= csCnv.Uni2Ansi(CString(files->GetAt(i)).GetBuffer());
		pszRecipe	+= (u32Recipe + 1);
		memcpy(pPktData, pszRecipe, strlen(pszRecipe));	pPktData += RECIPE_FILE_NAME;
		/* ���� ũ�� */
		u32Size	= lstFileSize.GetAt(lstFileSize.FindIndex(i));
		u32Size	= SWAP32(u32Size);
		memcpy(pPktData, &u32Size, 4);					pPktData += 4;
		u32Size	= SWAP32(u32Size);	/* �ٽ� ���� */
		/* ���� ��� (������ �ؽ�Ʈ) */
		pPktData[0]	= 0x00;	/* Text Files */			pPktData += 1;
		/* ������ ���� ���� (�ؽ�Ʈ ���� ������ �о���� ���� �� ���̳ʸ� ���� �о��, �������� ũ�⸦ �� �� ���� */
		u32Read	= uvCmn_LoadFile(CString(files->GetAt(i)).GetBuffer(), pPktData, u32Size, 0x01);
		if (u32Read < 1 || u32Read != u32Size)
		{
			bSended	= FALSE;
			break;
		}
		pPktData += u32Read;	/* ���� ���� ��ġ�� �̵� */
	}

	/* �뷮�� ũ�Ƿ�, �۽��� ��, ���� �ð� Time out �� �� �ο� */
	if (!bSended)
	{
		LOG_ERROR(ENG_EDIC::en_gfss, L"Failed to create packet for sending recipe file");
	}
	else
	{
		bSended	= SendData(ENG_PCGG::en_pcgg_get_recipe_files_ack, pPktBuff, u32Total, 5000);
	}

	/* �޸� ���� */
	::Free(pPktBuff);
	lstFileSize.RemoveAll();

	return bSended;
}

/*
 desc : Client �ʿ� �ֱ������� Alive Check ��� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CGFSSThread::SetAliveTime()
{
	/* ��Ŷ �۽� */
	return SendData(ENG_PCGG(ENG_PCUC::en_comn_alive_check), NULL, 0);
}
