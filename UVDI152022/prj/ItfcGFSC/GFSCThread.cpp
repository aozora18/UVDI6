
/*
 desc : TCP/IP Client Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "GFSCThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : th_id	- [in]  thread id
		client	- [in]  Server ���� ����
 retn : None
*/
CGFSCThread::CGFSCThread(UINT16 th_id, LPG_CSID config)
	: CClientThread(ENG_EDIC::en_gfsc,
					th_id,
					config->client->tcp_port,
					config->client->source_ipv4,	// CMPS Local (Client) IPv4
					config->client->target_ipv4,	// PLS Remote (Server) IPv4
					config->client->port_buff,
					config->client->recv_buff,
					config->client->ring_buff,
					config->client->idle_time,
					config->client->link_time,
					config->client->sock_time,
					config->client->live_time)
{
	m_bSendPktGFiles	= FALSE;
	m_u32FileCount		= 0;
	m_u32TotalFiles		= 0;

	/* ȯ�� ���� ���� */
	m_pstConfig			= config;
	/* ���� ���� �ʱ�ȭ (��뷮 ���� ���� ����) */
#ifdef _DEBUG
	UINT32 u32Size		= 1024 * 1024 * 32;
#else
	UINT32 u32Size		= 1024 * 1024 * 32;
#endif // _DEBUG
	m_pRecvBuff			= new UINT8 [u32Size+1];
	ASSERT(m_pRecvBuff);
	memset(m_pRecvBuff, 0x00, u32Size+1);
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CGFSCThread::~CGFSCThread()
{
	/* ���� ���� ���� (��뷮 ���� ���� ����)*/
	if (m_pRecvBuff)	delete [] m_pRecvBuff;
	m_pRecvBuff	= NULL;

	/* �ӽ� �Ź� ���� �۽� ��Ŷ �޸� ���� */
	m_lstPktGFiles.RemoveAll();

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
VOID CGFSCThread::DoWork()
{
#if 0
	/* ������ �ȵ� �������� Ȯ��*/
	if (!IsConnected())	return;
#endif

	/* ���� �ð� (�� 3 ��) ���� ��� ��, ������ ���Ÿ��� ���� */
	if (m_u32IdleTime && IsTcpWorkIdle(m_u32IdleTime))
	{
		TCHAR tzMesg[128]	= {NULL};
		swprintf_s(tzMesg, 128, L"GFSC communication wait timeout : %d", m_u32IdleTime);
		LOG_ERROR(ENG_EDIC::en_gfsc, tzMesg);

		/* ���� ������ ���Ҵ� ��� ��Ŷ�� ��� */
		ReceiveAllData(m_sdClient);
		/* ���� ������ ���� ��Ŵ */
		CloseSocket(m_sdClient);
	}
	else
	{
		/* ����� ���¿��� �Ź� ���� �۽� */
		if (m_syncPktCtrl.Enter())
		{
			if (m_bSendPktGFiles)	SetGerberFiles();
			m_syncPktCtrl.Leave();
		}
	}
}

/*
 desc : ������ �����̵� ���
 parm : sd	- ���� �����
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::Connected()
{
	return TRUE;
}

/*
 desc : �����κ��� ���� ������ �� ���
 parm : None
 retn : None
*/
VOID CGFSCThread::CloseSocket(SOCKET sd, UINT32 time_out/* =0 */)
{
	/* ���� ��ü ȣ�� */
	CClientThread::CloseSocket(sd, time_out);
}

/*
 desc : ���� ������ �۽�
 parm : data	- [in]  ������ �����Ͱ� ����� ���� ������
		size	- [in]  'data' ���ۿ� ����� �������� ũ��
		time_out- [in]  Timeout �� ����. ����: �и��� (0�̸�, ��� ����)
 retn : TRUE - �۽� ����(��� �۽ŵ� �����Ͱ� ������),
		FALSE - �۽� ����(���� ����... ������ �����ؾ� ��)
*/
BOOL CGFSCThread::SendData(PUINT8 data, UINT32 size, UINT64 time_out)
{
	BOOL bSucc			= FALSE;
	TCHAR tzMesg[128]	= {NULL};

	/* ---------------------------------------- */
	/* MC Protocol�� �°� ������ ��Ŷ ���� ���� */
	/* ---------------------------------------- */
	/* ���� �۽� �� ���� ���� */
	m_enSocket	= ENG_TPCS::en_sending;
	if (0 == time_out)	bSucc = ReSended(data, size, UINT32(3), UINT32(100));
#ifdef _DEBUG
	else				bSucc = ReSended(data, size, UINT32_MAX);
#else
	else				bSucc = ReSended(data, size, time_out);
#endif
	if (!bSucc)
	{
		/* ������ ��Ŷ �۽� ������ �ð����� �ֱⰡ �ʹ� ª���� �θ𿡰� �˸��� ���� */
		swprintf_s(tzMesg, 128, L"Failed to send data (size=%d) (sock_err = %d)",
				   size, WSAGetLastError());
		LOG_ERROR(ENG_EDIC::en_gfsc, tzMesg);
		m_enSocket	= ENG_TPCS::en_send_failed;	/* ���� �۽� ���� ��� ���� */
	}
	else
	{
		m_enSocket	= ENG_TPCS::en_sended;	/* ���� �۽� �Ϸ� ��� ���� */
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
BOOL CGFSCThread::SendData(ENG_PCGG cmd, PUINT8 data, UINT32 size, UINT64 time_out)
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
BOOL CGFSCThread::RecvData()
{
	BOOL bRecvState	= TRUE;
	UINT32 u32Size	= 0, u32Read;
	STG_UTPH stHead	= {NULL};
	PUINT8 pPktBuff	= NULL;

	/* ---------------------------------------------------------------- */
	/* 'm_pRecvBuff' ��ġ �� ��� ���� �κ��� ����Ű�� ������ ��ġ ���� */
	/* ---------------------------------------------------------------- */

	/* ���� ���� �ð� �ʱ�ȭ */
	uvGFSP_ResetLoopTime();

	while (1)
	{
		/* ���� ��뷮(?) ���� ������ ���� */
		pPktBuff	= m_pRecvBuff;

		/* ���� ���ۿ� ���� �ִ� �������� ũ�� Ȯ�� */
		u32Read		= m_pPktRingBuff->GetReadSize();

		/* ���� ���ŵ� �����Ͱ� �⺻ ��� ���� ũ�⸸ŭ ����Ǿ� �ִ��� Ȯ�� */
		if (u32Read < sizeof(STG_UTPH))	break;	// !!! ���� �ƴ� !!!

		/* ���ۿ� ������ ��Ŷ�� ��� ����Ǿ� �ִ��� Ȯ�� or �����ϴٸ� ��� */
		m_pPktRingBuff->CopyBinary((PUINT8)&stHead, sizeof(STG_UTPH));
		if (u32Read < (SWAP32(stHead.length) - sizeof(STG_UTPH)))	break;	// !!! ���� �ƴ� !!!

		/* ���� ������ Copy�� ���� ��� �κ��� ���۷κ��� ���� �� ���� */
		m_pPktRingBuff->PopBinary(sizeof(STG_UTPH));
		/* ��� �κ��� ������ �������� ����Ǿ� ũ�� �ٽ� ��� */
		u32Size = SWAP32(stHead.length) - sizeof(STG_UTPH);
		memcpy(pPktBuff, &stHead, sizeof(STG_UTPH));
		pPktBuff+= sizeof(STG_UTPH);

		/* �� ���ŵǾ���� ��ŭ �����Ͱ� �� ���ۿ� ����Ǿ� �ִ��� �б� ��û */
		/* u32Size�� ũ�Ⱑ 0 Bytes �� ���, ���� �����Ͱ� ���ٰ� �Ǵ� �Ѵ�. */
		if (u32Size)	m_pPktRingBuff->ReadBinary(pPktBuff, u32Size);
		/* Alive Check Packet �������� ���� */
		if (SWAP16(stHead.cmd) == (UINT16)ENG_PCUC::en_comn_alive_check)
		{
			m_bValidAliveTime	= TRUE;
		}
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
	}
#ifdef _DEBUG
	/* Alive Check�� ���� ���� �۽� */
	if (m_bValidAliveTime)
		TRACE(L"A [ Alive check packet ] was received [%.3f sec]\n",
			  (GetTickCount64()-m_u64WorkTime)/1000.0f);
#endif
	/* Recv to packet */
	m_enSocket	= ENG_TPCS::en_recved;

	return bRecvState;
}

/*
 desc : ���̺귯�� ������ ó��
 parm : data	- [in]  �̺�Ʈ �����Ͱ� ����� ���� ������
		size	- [in]  'data' ���ۿ� ����� �������� ũ��
 retn : TRUE or FALSE (���� ����? �߸��� �������� ������ ���ŵ�)
*/
BOOL CGFSCThread::RecvData(PUINT8 data, UINT32 size)
{
	UINT32 u32Body	= size - (sizeof(STG_UTPH) + sizeof(STG_UTPT));
	PUINT8 pPktBody	= data+sizeof(STG_UTPH);
	LPG_UTPH pstHead= LPG_UTPH(data);

	/* packet byte order */
	pstHead->SwapNetworkToHost();

	switch (pstHead->cmd)
	{
	case ENG_PCGG::en_pcgg_new_gerber_path_ack	:	NewGerberPath(pPktBody, u32Body);	break;
	case ENG_PCGG::en_pcgg_cnt_gerber_files_ack	:	CntGerberFiles(pPktBody, u32Body);	break;
	case ENG_PCGG::en_pcgg_del_gerber_path_ack	:	DelGerberPath(pPktBody, u32Body);	break;
	case ENG_PCGG::en_pcgg_get_recipe_files_ack	:	GetRecipeFiles(pPktBody, u32Body);	break;
	case ENG_PCGG::en_pcgg_set_recipe_files_ack	:	SetRecipeFiles(pPktBody, u32Body);	break;
	}

	return TRUE;
}

/*
 desc : �Ź� ������ �۽��ϱ� ���� �ӽ� ���� ����Ʈ�� ����
 parm : gfile	- [in]  �۽� ����� �Ź� ���� ������ ����� ����ü
 retn : None
*/
VOID CGFSCThread::AddPktGFiles(STG_PGFP gfile)
{
	if (m_lstPktGFiles.GetCount() > MAX_PKT_SEND_GFILES)	return;
	/* �ӽ� ���� */
	m_lstPktGFiles.AddTail(gfile);
}

/*
 desc : ���� �Ź� ���� �۽� �ӽ� ���� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CGFSCThread::ResetPktGfiles()
{
	if (m_syncPktCtrl.Enter())
	{
		m_bSendPktGFiles	= FALSE;
		m_syncPktCtrl.Leave();
	}
	m_lstPktGFiles.RemoveAll();
}

/*
 desc : �ӽ� �޸𸮿� ��ϵ� �Ź� ���� ������ �۽��϶�� �÷��� ����
 parm : None
 retn : None
*/
VOID CGFSCThread::SetSetupGFile()
{
	BOOL bSuccSended	= FALSE;
	TCHAR tzMesg[128]	= {NULL};
	UINT32 u32SendTotal	= 0;
	ENG_LNWE enLevel	= ENG_LNWE::en_normal;

	/* ������� �۽ŵ� �Ź� ������ ���� ���� �ʱ�ȭ */
	m_u32FileCount	= 0;
	m_u32TotalFiles	= (UINT32)m_lstPktGFiles.GetCount();
	u32SendTotal	= SWAP32(m_u32TotalFiles);

	/* ���� �۽��Ϸ��� �Ź� ������ ���� �� ���� ���� */
	bSuccSended	= SendData(ENG_PCGG::en_pcgg_cnt_gerber_files, (PUINT8)&u32SendTotal, 4);
	if (!bSuccSended)
	{
		swprintf_s(tzMesg, 128,
				   L"Failed to send the gerber info [gfile_count = %d]", m_u32TotalFiles);
		LOG_ERROR(ENG_EDIC::en_gfsc, tzMesg);
	}
	else
	{
		swprintf_s(tzMesg, 128,
				   L"Succeeded to send the gerber info [gfile_count = %d]", m_u32TotalFiles);
		LOG_MESG(ENG_EDIC::en_gfsc, tzMesg);
	}

	/* �� �������� ��� */
	if (m_syncPktCtrl.Enter())
	{
		m_bSendPktGFiles	= bSuccSended;
		m_syncPktCtrl.Leave();
	}
}

/*
 desc : ������ Alive Check ��� ����
 parm : logs	- [in]  �α� ���� ���� ����
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::SetAliveTime(BOOL logs)
{
	if (logs)
	{
		TCHAR tzMesg[128]	= {NULL};
		swprintf_s(tzMesg, 128, L"Send Alive Time (Incr = %u)", m_u32AliveCount);
		LOG_MESG(ENG_EDIC::en_gfsc, tzMesg);

		if (++m_u32AliveCount == UINT32_MAX)	m_u32AliveCount	= 1;
	}

	/* ���� �޾Ҵٴ� �÷��� �ʱ�ȭ */
	m_bValidAliveTime	= FALSE;

	/* ��Ŷ �۽� */
	return SendData(ENG_PCGG(ENG_PCUC::en_comn_alive_check), NULL, 0);
}

/*
 desc : �Ź� ���� ��� �� ���� ��� ����
 parm : g_name	- [in]  ���� ������ �Ź� �� (��ü ��δ� ������. �⺻ ��� ������ ������ �Ź� ���� ��)
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::SetNewGerberPath(PTCHAR g_name)
{
	TCHAR tzMesg[128]	= {NULL};
	PCHAR pszGName		= NULL;
	CUniToChar csCnv;

	if (!g_name)	return FALSE;
	if (wcslen(g_name) < 1)	return FALSE;

	/* Ansi Code ��ȯ */
	pszGName	= csCnv.Uni2Ansi(g_name);
	if (!pszGName)	return FALSE;

	swprintf_s(tzMesg, 128, L"Create a gerber path (%s)", g_name);
	LOG_MESG(ENG_EDIC::en_gfsc, tzMesg);

	/* ��Ŷ �۽� */
	return SendData(ENG_PCGG::en_pcgg_new_gerber_path, PUINT8(pszGName), UINT32(strlen(pszGName)));
}

/*
 desc : �Ź� ���� ��� �� ���� ��� ����
 parm : g_name	- [in]  ������ �Ź� �� (��ü ��δ� ������. �⺻ ��� ������ ������ �Ź� ���� ��)
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::SetDelGerberPath(PTCHAR g_name)
{
	TCHAR tzMesg[128] = {NULL};
	PCHAR pszGName	= NULL;
	CUniToChar csCnv;

	if (!g_name)	return FALSE;
	if (wcslen(g_name) < 1)	return FALSE;

	/* Ansi Code ��ȯ */
	pszGName	= csCnv.Uni2Ansi(g_name);
	if (!pszGName)	return FALSE;

	swprintf_s(tzMesg, 128, L"Delete the gerber path (%s)", g_name);
	LOG_MESG(ENG_EDIC::en_gfsc, tzMesg);

	/* ��Ŷ �۽� */
	return SendData(ENG_PCGG::en_pcgg_del_gerber_path, PUINT8(pszGName), UINT32(strlen(pszGName)));
}

/*
 desc : �ӽ� �޸𸮿� ��� (����)�� �Ź� ������ ���� �а�, �۽� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::SetGerberFiles()
{
	BOOL bSucc			= FALSE;
	TCHAR tzGFile[MAX_PATH_LEN]	= {NULL}, tzMesg[128] = {NULL};
	size_t lGFileSize	= 0, lGZipedSize;
	UINT32 u32Head		= 9;	/* f_no(4)+flag(1)+size(4) */
	UINT32 u32SendSize	= 0, u32SendFiles;
	PUINT8 pGFileOrg, pGFileData;
	PUINT8 pGZipOrg, pGZipData;
	POSITION pPos		= NULL;
	STG_PGFP stGFile	= {NULL};
	LPG_CSID pstConfig	= uvGFSP_Info_GetData();
	CUniToChar csCnv1, csCnv2;

	/* �ϴ�, ������ ���� ��� ���� */
	if (m_syncPktCtrl.Enter())
	{
		m_bSendPktGFiles	= FALSE;
		m_syncPktCtrl.Leave();
	}

	/* ���� �۽��ϰ��� �ϴ� �Ź� ������ �ε��� ���� ��ü �������� ũ�ų� ���ٸ�, ��� ���� �Ϸ�� ���� */
	if (m_u32FileCount >= m_u32TotalFiles)
	{
		/* �Ź� (�Ź� ���� Ȥ�� ���� ��Ÿ ����) ���� ��� ���� �Ϸ� �ߴٰ� �˸� */
		SendData(ENG_PCGG::en_pcgg_send_gerber_comp, NULL, 0);
		return FALSE;
	}

	/* ���� �۽��� �Ź� ���� ���� ��� */
	pPos	= m_lstPktGFiles.FindIndex(m_u32FileCount);
	if (!pPos)
	{
		swprintf_s(tzMesg, 128,
				   L"There are currently no Gerber files [%d th] to transfer", m_u32FileCount);
		LOG_ERROR(ENG_EDIC::en_gfsc, tzMesg);
		return FALSE;
	}

	stGFile	= m_lstPktGFiles.GetAt(pPos);

	/* �Ź� ���� �̸� ���� �� �˻��� ���� */
	swprintf_s(tzGFile, MAX_PATH_LEN, L"%s\\%s\\%s",
			   pstConfig->gerber_path->gfsc,
			   csCnv1.Ansi2Uni(stGFile.path),
			   csCnv2.Ansi2Uni(stGFile.file));

	/* �Ź� ���� ���縦 ���� �޸� �Ҵ� */
	lGFileSize	= stGFile.s_size + sizeof(STG_PGFP);
	pGFileOrg	= (PUINT8)::Alloc(lGFileSize + 1);
	memset(pGFileOrg, 0x00, lGFileSize + 1);
	pGFileData	= pGFileOrg + sizeof(STG_PGFP);
	/* ������ �Ź� ���� �о �ӽ� �޸𸮿� �ֱ� */
	if (!uvCmn_LoadFile(tzGFile, pGFileData, stGFile.s_size))
	{
		::Free(pGFileOrg);
		swprintf_s(tzMesg, 128, L"Failed to load the [file:%s] [size:%d]",
				   tzGFile, stGFile.s_size);
		LOG_ERROR(ENG_EDIC::en_gfsc, tzMesg);
		return FALSE;
	}
	/* ��� �κ��� ���� ���ο� ���� ���� ���� ũ�� ���� �޶����Ƿ� ��� �Ŀ� ���� �۾� ���� */
	stGFile.SwapData();
	memcpy(pGFileOrg, &stGFile, sizeof(STG_PGFP));

	/* �������� ���, �ִ� ���� ���� ũ�� �� ��ȯ */
	lGZipedSize	= snappy_max_compressed_length(lGFileSize);
	/* ���� ���ο� ���� �۽��ؾ� �ϹǷ� (!!! �� ���� ��Ŷ 7 ����Ʈ ���� ���� �߰� Ȯ�� !!!) */
	pGZipOrg	= (PUINT8)::Alloc(lGZipedSize + u32Head);	/* !!! ���� ������ ���� ���� ��� �ϴ� 2��� Ȯ�� !!! */

	ASSERT(pGZipOrg);
	/* !!! ��Ŷ ���� 7 ����Ʈ �� �κк��� ����� �Ź� ���� (����ü ����)�� ���ԵǾ�� �� !!! */
	pGZipData	= pGZipOrg + u32Head;
	/* ��� �κ� ���� */
	if (!pstConfig->common->use_zip_file)
	{
		/* ���� ���� �״�� ���� */
		memcpy(pGZipData, pGFileOrg, lGFileSize);
		lGZipedSize	= lGFileSize;
		u32SendSize	= UINT32(lGZipedSize) + u32Head;	/* �ϴ�, ���� ũ�⸦ ���� ���� ũ��� ���� */
	}
	else
	{
		/* ���� ���� ���� �۾� ���� */
		snappy_status enStatus = snappy_compress((PCHAR)pGFileOrg, lGFileSize, (PCHAR)pGZipData, &lGZipedSize);
		if (enStatus != SNAPPY_OK || lGZipedSize < 1)
		{
			swprintf_s(tzMesg, 128,
					   L"Failed to compress the [file:%s] [size:%d] [zip:%d] [status=%d]",
					   tzGFile, (INT32)lGFileSize, (INT32)lGZipedSize, (INT32)enStatus);
			LOG_ERROR(ENG_EDIC::en_gfsc, tzMesg);
			m_enLastErrorLevel	= ENG_LNWE::en_error;
			::Free(pGFileOrg);
			::Free(pGZipOrg);

			return FALSE;
		}

		u32SendSize	= UINT32(lGZipedSize) + u32Head;	/* �ϴ�, ���� ũ�⸦ ���� ���� ũ��� ���� */
	}

	/* ��� �κ� ���� */
	u32SendFiles	= SWAP32(m_u32FileCount + 1);	/* 1-based ���� �۽��ؾ� �ϱ� ������ */
	memcpy(pGZipOrg, &u32SendFiles, 4);
	if (stGFile.file_no == 0x0000)					pGZipOrg[4]	= 0x00;
	else if (m_u32FileCount == (m_u32TotalFiles-1))	pGZipOrg[4]	= 0x02;
	else											pGZipOrg[4]	= 0x01;
	lGZipedSize	= SWAP32(lGZipedSize);	/* ��ü �۽ŵǴ� ��Ŷ�� ũ�� */
	memcpy(pGZipOrg+5, &lGZipedSize, 4);

	/* ���� ���� Ȥ�� ������ �Ź� ���� �۽� ���� */
	bSucc	= SendData(ENG_PCGG::en_pcgg_send_gerber_files, pGZipOrg, u32SendSize, 10000);
	if (!bSucc)
	{
		swprintf_s(tzMesg, 128, L"Failed to send the [%s]", tzGFile);
		m_enLastErrorLevel	= ENG_LNWE::en_error;
		LOG_ERROR(ENG_EDIC::en_gfsc, tzMesg);
	}
	else
	{
		swprintf_s(tzMesg, 128, L"Succeeded to send the [%s]", tzGFile);
		m_enLastErrorLevel	= ENG_LNWE::en_normal;
		LOG_MESG(ENG_EDIC::en_gfsc, tzMesg);
	}

	/* �޸� ���� */
	::Free(pGFileOrg);
	::Free(pGZipOrg);

	/* ���� �۽��� �����Ͱ� ���� �ִٸ� ... */
	m_u32FileCount++;
	if (m_syncPktCtrl.Enter())
	{
		m_bSendPktGFiles	= TRUE;
		m_syncPktCtrl.Leave();
	}
#ifdef _DEBUG
	TRACE(L"The number of files transfered = %d / %d\n", m_u32FileCount, m_u32TotalFiles);
#endif

	return bSucc;
}

/*
 desc : �������� Recipe Files ��û
 parm : None
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::GetRecipeFiles()
{
	LOG_MESG(ENG_EDIC::en_gfsc, L"Request the recipe files");
	/* ��Ŷ �۽� */
	return SendData(ENG_PCGG::en_pcgg_get_recipe_files, NULL, 0);
}

/*
 desc : Server�κ��� Client�� ���� (����)�� ������ ������ ��û ���� ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::SetRecipeFiles()
{
	BOOL bSended		= FALSE;
	LPG_CSBP pstRecipe	= uvGFSP_Info_GetData()->recipe_path;
	CStringArray arrRecipeFiles;

	LOG_MESG(ENG_EDIC::en_gfsc, L"Request to recipe files");

	/* Server ���� Recipe Files �˻� */
	uvCmn_SearchRecurseFile(pstRecipe->gfsc, L".dat", arrRecipeFiles, 0x00);
	/* ���� �˻��� �����ǰ� ���ٸ� ... */
	if (arrRecipeFiles.GetCount() < 1)
	{
		UINT16 u16Count	= 0x0000;
		bSended	= SendData(ENG_PCGG::en_pcgg_set_recipe_files, (PUINT8)&u16Count, 2);
	}
	else
	{
		bSended	= SetRecipeFiles(&arrRecipeFiles);
	}

	return bSended;
}

/*
 desc : ���� (Client) �ý����� ���� ��ο� �Ź� ��� ���� ��û�� ���� ���� ó��
 parm : data	- [in]  ��Ŷ ���� (head & tail ����) ������ ����� ���� ������
		size	- [in]  'data' ���ۿ� ����� ��Ŷ�� ũ��
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::NewGerberPath(PUINT8 data, UINT32 size)
{
	TCHAR tzMesg[128]	= {NULL};

	if (size != 0x01)	data[0]	= 0xfe;

	/* �ϴ�, �⺻ ������ ���� ������ ���� */
	m_enLastErrorLevel	= ENG_LNWE::en_error;

	/* �α� ��� */
	switch (data[0])
	{
	case 0x00 :	m_enLastErrorLevel	= ENG_LNWE::en_normal;
				swprintf_s(tzMesg, 128, L"succeeded to create the gerber directory");				break;
	case 0x01 :	swprintf_s(tzMesg, 128, L"failed to delete the gerber directory");					break;
	case 0x02 :	swprintf_s(tzMesg, 128, L"failed to create the gerber directory");					break;
	case 0xfe :	swprintf_s(tzMesg, 128, L"invalid packet size [size (body only): %d]", size);		break;
	case 0xff :	swprintf_s(tzMesg, 128, L"invalid packet information [cmd: generate gerber path]");	break;
	}

	if (wcslen(tzMesg))	LOG_SAVED(ENG_EDIC::en_gfsc, m_enLastErrorLevel, tzMesg);

	return TRUE;
}

/*
 desc : ���� (Client) �ý����� ���� ��ο� �Ź� ��� ���� ��û�� ���� ���� ó��
 parm : data	- [in]  ��Ŷ ���� (head & tail ����) ������ ����� ���� ������
		size	- [in]  'data' ���ۿ� ����� ��Ŷ�� ũ��
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::DelGerberPath(PUINT8 data, UINT32 size)
{
	TCHAR tzMesg[128]		= {NULL};

	/* �ϴ�, �⺻ ������ ���� ������ ���� */
	m_enLastErrorLevel	= ENG_LNWE::en_error;

	if (size != 0x01)	data[0]	= 0xfe;

	/* �α� ��� */
	switch (data[0])
	{
	case 0x00 :	m_enLastErrorLevel	= ENG_LNWE::en_normal;
				swprintf_s(tzMesg, 128, L"Succeeded to delete the gerber directory");				break;
	case 0x01 :	swprintf_s(tzMesg, 128, L"Failed to find the gerber directory");					break;
	case 0x02 :	swprintf_s(tzMesg, 128, L"Failed to delete the gerber directory");					break;
	case 0xfe :	swprintf_s(tzMesg, 128, L"Invalid packet size [size (body only): %d]", size);		break;
	case 0xff :	swprintf_s(tzMesg, 128, L"Invalid packet information [cmd: delete gerber path]");	break;
	}

	if (wcslen(tzMesg))	LOG_SAVED(ENG_EDIC::en_gfsc, m_enLastErrorLevel, tzMesg);

	return TRUE;
}

/*
 desc : ���� (Client) �ý��ۿ� ���޵� �Ź� ���� ���� ��û�� ���� ���� ó��
 parm : data	- [in]  ��Ŷ ���� (head & tail ����) ������ ����� ���� ������
		size	- [in]  'data' ���ۿ� ����� ��Ŷ�� ũ��
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::CntGerberFiles(PUINT8 data, UINT32 size)
{
	UINT8 u8Flag		= 0x00;
	TCHAR tzMesg[128]	= {NULL};
	UINT32 u32PktAck	= 0x0000;

	/* �ϴ�, �⺻ ������ ���� ������ ���� */
	m_enLastErrorLevel	= ENG_LNWE::en_error;

	if (size != 0x04)	u8Flag	= 0xfe;
	else
	{
		memcpy(&u32PktAck, data, size);
		u32PktAck	= SWAP32(u32PktAck);
		if (m_u32TotalFiles != u32PktAck)	u8Flag	= 0xff;
	}

	/* �α� ��� */
	switch (data[0])
	{
	case 0x00 :	m_enLastErrorLevel	= ENG_LNWE::en_normal;
				swprintf_s(tzMesg, 128, L"Checked the number of gerber files to received");		break;
	case 0xfe :	swprintf_s(tzMesg, 128, L"Invalid packet size [size (body only): %d]", size);	break;
	case 0xff :	swprintf_s(tzMesg, 128, L"The number of send and receive files does not match");break;
	}

	if (wcslen(tzMesg))	LOG_SAVED(ENG_EDIC::en_gfsc, m_enLastErrorLevel, tzMesg);

	return TRUE;
}

/*
 ����: Server�κ��� ���ŵ� ������ ���� ����
 parm : data	- [in]  ��Ŷ ���� (head & tail ����) ������ ����� ���� ������
		size	- [in]  'data' ���ۿ� ����� ��Ŷ�� ũ��
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::GetRecipeFiles(PUINT8 data, UINT32 size)
{
	CHAR szFile[RECIPE_FILE_NAME]	= {NULL};
	CHAR szRecipe[MAX_PATH_LEN]		= {NULL};
	UINT8 u8Mode	= 0x00;	/* 0x00 : Text File, 0x01 : Binary File */
	UINT16 u16Count	= 0x0000, i		= 0;
	UINT32 u32Size	= 0;
	PUINT8 pPktBuff	= data, pPktData = pPktBuff;
	LPG_CSBP pstRecipe	= uvGFSP_Info_GetData()->recipe_path;
	CUniToChar csCnv;

	/* �߸��� ��Ŷ ���� */
	if (size < 2)	return FALSE;

	/* ������ ����Ǿ� �ִ� ���� ��� ���� */
	uvCmn_DeleteAllFiles(pstRecipe->gfsc);

	/* �ϴ� ���ŵ� ���� ���� Ȯ�� */
	memcpy(&u16Count, pPktData, 2);	pPktData += 2;
	u16Count	= SWAP16(u16Count);
	/* ���ŵ� ���� ���� */
	m_u32TotalFiles	= u16Count;
	m_u32TotalSizes	= 0;
	if (u16Count < 1)
	{
		LOG_WARN(ENG_EDIC::en_gfsc, L"There are no recipe files on server system");
	}
	else
	{
		/* ���� ������ŭ �ݺ� */
		for (i=0; i<u16Count; i++)
		{
			/* ������ ���� ��� */
			memset(szFile, 0x00, RECIPE_FILE_NAME);
			memcpy(szFile, pPktData, RECIPE_FILE_NAME);				pPktData += RECIPE_FILE_NAME;
			sprintf_s(szRecipe, MAX_PATH_LEN, "%s\\%s", csCnv.Uni2Ansi(pstRecipe->gfsc), szFile);
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

	return TRUE;
}

/*
 ����: Server�κ��� ���ŵ� ������ ���� ���� ��
 parm : data	- [in]  ��Ŷ ���� (head & tail ����) ������ ����� ���� ������
		size	- [in]  'data' ���ۿ� ����� ��Ŷ�� ũ��
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::SetRecipeFiles(PUINT8 data, UINT32 size)
{
	/* �߸��� ��Ŷ ���� */
	if (size < 1)	return FALSE;

	/* �ϴ� ���ŵ� ���� ���� Ȯ�� */
	if (data[0] != 0x01)
	{
		LOG_ERROR(ENG_EDIC::en_gfsc, L"Failed to update recipe file on the server");
		return FALSE;
	}
	else
	{
		LOG_MESG(ENG_EDIC::en_gfsc, L"Succeeded to update recipe file on the server");
	}

	return TRUE;
}

/*
 desc : Client�κ��� ���ŵ� ��Ŷ ��� �κ� �ӽ� ��� (App. �ʿ��� � ��ɾ���� ���ŵǾ����� �˱� ����)
 parm : head	- [in]  ���ŵ� ��Ŷ ��� ������ ����ü ������
 retn : None
*/
VOID CGFSCThread::PushPktHead(LPG_UTPH head)
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
BOOL CGFSCThread::PopPktHead(STG_UTPH &head)
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
 desc : Client���� Server�� �Ź� ���� (�Ź� ���� Ȥ�� �Ź� ���� ��Ÿ ����)�� ��� �۽��ߴ��� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::IsSendGFileCompleted()
{
	if (m_u32TotalFiles < 1 || m_u32FileCount < 1)	return FALSE;
	return (m_u32FileCount == m_u32TotalFiles);
}

/*
 desc : ������� �Ź� ���� (�Ź� ���� Ȥ�� �Ź� ���� ��Ÿ ����)�� ���۵� ���� �� ��ȯ
 parm : None
 retn : ���۷� ��
*/
DOUBLE CGFSCThread::GetSendGFilesRate()
{
	if (m_u32TotalFiles < 1 || m_u32FileCount < 1)	return 0.0f;

	return (DOUBLE(m_u32FileCount) / DOUBLE(m_u32TotalFiles) * 100);
}

/*
 desc : ���� �۽� ���̸� ���� ó��
 parm : None
 retn : None
*/
VOID CGFSCThread::StopGerberSendFile()
{
	if (m_syncPktCtrl.Enter())
	{
		if (m_bSendPktGFiles)	m_bSendPktGFiles	= FALSE;

		m_syncPktCtrl.Leave();
	}
}

/*
 desc : Server�� ����� ������ ���� �۽�
 parm : files	- [in]  ���� �۽ŵ� ������ ���ϸ��� ����Ǿ� �ִ� ���ڿ� �迭 ����
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::SetRecipeFiles(CStringArray *files)
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
	u32Recipe	= (UINT32)wcslen(uvGFSP_Info_GetData()->recipe_path->gfsc);
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
		LOG_ERROR(ENG_EDIC::en_gfsc, L"Failed to create packet for sending recipe file");
	}
	else
	{
		bSended	= SendData(ENG_PCGG::en_pcgg_set_recipe_files, pPktBuff, u32Total, 5000);
	}

	/* �޸� ���� */
	::Free(pPktBuff);
	lstFileSize.RemoveAll();

	return bSended;
}

