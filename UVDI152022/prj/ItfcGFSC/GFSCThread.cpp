
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
 desc : 생성자
 parm : th_id	- [in]  thread id
		client	- [in]  Server 연결 정보
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

	/* 환경 파일 저장 */
	m_pstConfig			= config;
	/* 수신 버퍼 초기화 (대용량 수신 버퍼 적용) */
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
	/* 수신 버퍼 해제 (대용량 수신 버퍼 해제)*/
	if (m_pRecvBuff)	delete [] m_pRecvBuff;
	m_pRecvBuff	= NULL;

	/* 임시 거버 파일 송신 패킷 메모리 해제 */
	m_lstPktGFiles.RemoveAll();

	/* 패킷 수신 임시 등록 메모리 해제 */
	if (m_syncPktCmd.Enter())
	{
		m_lstPktCmd.RemoveAll();
		m_syncPktCmd.Leave();
	}
}

/*
 desc : 스레드 실행시 주기적 호출됨
 parm : None
 retn : None
*/
VOID CGFSCThread::DoWork()
{
#if 0
	/* 연결이 안된 상태인지 확인*/
	if (!IsConnected())	return;
#endif

	/* 일정 시간 (약 3 초) 동안 대기 후, 강제로 수신모드로 변경 */
	if (m_u32IdleTime && IsTcpWorkIdle(m_u32IdleTime))
	{
		TCHAR tzMesg[128]	= {NULL};
		swprintf_s(tzMesg, 128, L"GFSC communication wait timeout : %d", m_u32IdleTime);
		LOG_ERROR(ENG_EDIC::en_gfsc, tzMesg);

		/* 기존 저장해 놓았던 모든 패킷을 비움 */
		ReceiveAllData(m_sdClient);
		/* 연결 강제로 해제 시킴 */
		CloseSocket(m_sdClient);
	}
	else
	{
		/* 연결된 상태에서 거버 파일 송신 */
		if (m_syncPktCtrl.Enter())
		{
			if (m_bSendPktGFiles)	SetGerberFiles();
			m_syncPktCtrl.Leave();
		}
	}
}

/*
 desc : 서버에 연결이된 경우
 parm : sd	- 소켓 기술자
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::Connected()
{
	return TRUE;
}

/*
 desc : 서버로부터 연결 해제가 된 경우
 parm : None
 retn : None
*/
VOID CGFSCThread::CloseSocket(SOCKET sd, UINT32 time_out/* =0 */)
{
	/* 상위 객체 호출 */
	CClientThread::CloseSocket(sd, time_out);
}

/*
 desc : 소켓 데이터 송신
 parm : data	- [in]  전송할 데이터가 저장된 버퍼 포인터
		size	- [in]  'data' 버퍼에 저장된 데이터의 크기
		time_out- [in]  Timeout 값 지정. 단위: 밀리초 (0이면, 사용 안함)
 retn : TRUE - 송신 성공(비록 송신된 데이터가 없더라도),
		FALSE - 송신 실패(소켓 에러... 연결을 해제해야 함)
*/
BOOL CGFSCThread::SendData(PUINT8 data, UINT32 size, UINT64 time_out)
{
	BOOL bSucc			= FALSE;
	TCHAR tzMesg[128]	= {NULL};

	/* ---------------------------------------- */
	/* MC Protocol에 맞게 적용한 패킷 전송 진행 */
	/* ---------------------------------------- */
	/* 소켓 송신 중 모드로 변경 */
	m_enSocket	= ENG_TPCS::en_sending;
	if (0 == time_out)	bSucc = ReSended(data, size, UINT32(3), UINT32(100));
#ifdef _DEBUG
	else				bSucc = ReSended(data, size, UINT32_MAX);
#else
	else				bSucc = ReSended(data, size, time_out);
#endif
	if (!bSucc)
	{
		/* 이전에 패킷 송신 실패한 시간보다 주기가 너무 짧으면 부모에게 알리지 않음 */
		swprintf_s(tzMesg, 128, L"Failed to send data (size=%d) (sock_err = %d)",
				   size, WSAGetLastError());
		LOG_ERROR(ENG_EDIC::en_gfsc, tzMesg);
		m_enSocket	= ENG_TPCS::en_send_failed;	/* 소켓 송신 실패 모드 변경 */
	}
	else
	{
		m_enSocket	= ENG_TPCS::en_sended;	/* 소켓 송신 완료 모드 변경 */
	}

	return bSucc;
}

/*
 desc : 응답 패킷 전달
 parm : cmd		- [in]  패킷 명령어
		data	- [in]  패킷 본문 데이터가 저장된 버퍼
		size	- [in]  'data' 버퍼 에 저장된 데이터 본문 크기
		time_out- [in]  Timeout 값 지정. 단위: 밀리초 (0이면, 사용 안함)
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::SendData(ENG_PCGG cmd, PUINT8 data, UINT32 size, UINT64 time_out)
{
	UINT32 u32Size	= sizeof(STG_UTPH) + size + sizeof(STG_UTPT);	/* 전체 송신될 패킷 크기 설정 */
	STG_UTPH stHead	= {NULL};
	STG_UTPT stTail	= {NULL};
	PUINT8 pPktBuff	= (PUINT8)::Alloc(u32Size+1), pPktData;
	ASSERT(pPktBuff);
	pPktData	= pPktBuff;

	/* 메모리 끝 부분 초기화 */
	pPktBuff[u32Size]	= 0x00;

	/* 패킷 헤더 및 테일 부분 기본 값 설정 */
	stHead.head		= SWAP16(UINT16(ENG_PHSI::en_head_gfss_vs_gfsc));
	stHead.cmd		= SWAP16(UINT16(cmd));
	stHead.length	= SWAP32(u32Size);
	stTail.crc		= 0x0000;
	stTail.tail		= SWAP16(UINT16(ENG_PHSI::en_comn_tail));

	/* 패킷 송신 크기 확인 */
	memcpy(pPktData, &stHead, sizeof(STG_UTPH));			pPktData += sizeof(STG_UTPH);
	if (data && size > 0)	memcpy(pPktData, data, size);	pPktData += size;
	memcpy(pPktData, &stTail, sizeof(STG_UTPT));

	/* 송신 패킷 등록 */
	stHead.SwapNetworkToHost();
	PushPktHead(&stHead);

	return SendData(pPktBuff, u32Size, time_out);
}

/*
 desc : 데이터가 수신된 경우, 호출됨
 parm : None
 retn : TRUE - 성공적으로 분석 (Ring Buffer) 처리
		FALSE- 분석 처리하지 않음 (아직 완벽하지 않은 패킷임)
*/
BOOL CGFSCThread::RecvData()
{
	BOOL bRecvState	= TRUE;
	UINT32 u32Size	= 0, u32Read;
	STG_UTPH stHead	= {NULL};
	PUINT8 pPktBuff	= NULL;

	/* ---------------------------------------------------------------- */
	/* 'm_pRecvBuff' 위치 중 헤더 이후 부분을 가리키는 포인터 위치 설정 */
	/* ---------------------------------------------------------------- */

	/* 무한 루프 시간 초기화 */
	uvGFSP_ResetLoopTime();

	while (1)
	{
		/* 현재 대용량(?) 버퍼 포인터 연결 */
		pPktBuff	= m_pRecvBuff;

		/* 현재 버퍼에 남아 있는 데이터의 크기 확인 */
		u32Read		= m_pPktRingBuff->GetReadSize();

		/* 현재 수신된 데이터가 기본 헤더 정보 크기만큼 저장되어 있는지 확인 */
		if (u32Read < sizeof(STG_UTPH))	break;	// !!! 에러 아님 !!!

		/* 버퍼에 완전한 패킷이 모두 저장되어 있는지 확인 or 부족하다면 대기 */
		m_pPktRingBuff->CopyBinary((PUINT8)&stHead, sizeof(STG_UTPH));
		if (u32Read < (SWAP32(stHead.length) - sizeof(STG_UTPH)))	break;	// !!! 에러 아님 !!!

		/* 먼저 기존에 Copy된 응답 헤더 부분을 버퍼로부터 읽은 후 버림 */
		m_pPktRingBuff->PopBinary(sizeof(STG_UTPH));
		/* 헤더 부분을 제외한 나머지가 저장되어 크기 다시 계산 */
		u32Size = SWAP32(stHead.length) - sizeof(STG_UTPH);
		memcpy(pPktBuff, &stHead, sizeof(STG_UTPH));
		pPktBuff+= sizeof(STG_UTPH);

		/* 총 수신되어야할 만큼 데이터가 링 버퍼에 저장되어 있는지 읽기 요청 */
		/* u32Size의 크기가 0 Bytes 인 경우, 본문 데이터가 없다고 판단 한다. */
		if (u32Size)	m_pPktRingBuff->ReadBinary(pPktBuff, u32Size);
		/* Alive Check Packet 수신인지 여부 */
		if (SWAP16(stHead.cmd) == (UINT16)ENG_PCUC::en_comn_alive_check)
		{
			m_bValidAliveTime	= TRUE;
		}
		else
		{
			/* 수신된 데이터 저장 */
			bRecvState	= RecvData(m_pRecvBuff, u32Size+sizeof(STG_UTPH));
			/* 수신된 패킷 헤드 (명령어) 부분 임시 저장 */
			if (bRecvState)
			{
				stHead.SwapNetworkToHost();
				PushPktHead(&stHead);
			}
		}
		/* 무한 루프에 들어 갔는지 여부 확인 */
#if (USE_CHECK_LIMIT_LOOP_TIME)
		if (uvGFSP_IsLoopTimeOut(WFILE, WLINE))	break;
#else
		if (uvGFSP_IsLoopTimeOut())	break;
#endif
	}
#ifdef _DEBUG
	/* Alive Check에 대해 응답 송신 */
	if (m_bValidAliveTime)
		TRACE(L"A [ Alive check packet ] was received [%.3f sec]\n",
			  (GetTickCount64()-m_u64WorkTime)/1000.0f);
#endif
	/* Recv to packet */
	m_enSocket	= ENG_TPCS::en_recved;

	return bRecvState;
}

/*
 desc : 라이브러리 데이터 처리
 parm : data	- [in]  이벤트 데이터가 저장된 버퍼 포인터
		size	- [in]  'data' 버퍼에 저장된 데이터의 크기
 retn : TRUE or FALSE (연결 해제? 잘못된 데이터의 성격이 수신됨)
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
 desc : 거버 파일을 송신하기 위한 임시 버퍼 리스트에 저장
 parm : gfile	- [in]  송신 대상의 거버 파일 정보가 저장된 구조체
 retn : None
*/
VOID CGFSCThread::AddPktGFiles(STG_PGFP gfile)
{
	if (m_lstPktGFiles.GetCount() > MAX_PKT_SEND_GFILES)	return;
	/* 임시 저장 */
	m_lstPktGFiles.AddTail(gfile);
}

/*
 desc : 기존 거버 파일 송신 임시 버퍼 초기화
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
 desc : 임시 메모리에 등록된 거버 파일 정보를 송신하라고 플래그 설정
 parm : None
 retn : None
*/
VOID CGFSCThread::SetSetupGFile()
{
	BOOL bSuccSended	= FALSE;
	TCHAR tzMesg[128]	= {NULL};
	UINT32 u32SendTotal	= 0;
	ENG_LNWE enLevel	= ENG_LNWE::en_normal;

	/* 현재까지 송신된 거버 파일의 개수 정보 초기화 */
	m_u32FileCount	= 0;
	m_u32TotalFiles	= (UINT32)m_lstPktGFiles.GetCount();
	u32SendTotal	= SWAP32(m_u32TotalFiles);

	/* 현재 송신하려는 거버 파일의 개수 및 정보 전달 */
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

	/* 맨 마지막에 기술 */
	if (m_syncPktCtrl.Enter())
	{
		m_bSendPktGFiles	= bSuccSended;
		m_syncPktCtrl.Leave();
	}
}

/*
 desc : 서버에 Alive Check 명령 전달
 parm : logs	- [in]  로그 파일 저장 여부
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

	/* 응답 받았다는 플래그 초기화 */
	m_bValidAliveTime	= FALSE;

	/* 패킷 송신 */
	return SendData(ENG_PCGG(ENG_PCUC::en_comn_alive_check), NULL, 0);
}

/*
 desc : 거버 파일 경로 명 생성 명령 전달
 parm : g_name	- [in]  새로 생성될 거버 명 (전체 경로는 미포함. 기본 경로 하위에 생성될 거버 폴더 명)
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::SetNewGerberPath(PTCHAR g_name)
{
	TCHAR tzMesg[128]	= {NULL};
	PCHAR pszGName		= NULL;
	CUniToChar csCnv;

	if (!g_name)	return FALSE;
	if (wcslen(g_name) < 1)	return FALSE;

	/* Ansi Code 변환 */
	pszGName	= csCnv.Uni2Ansi(g_name);
	if (!pszGName)	return FALSE;

	swprintf_s(tzMesg, 128, L"Create a gerber path (%s)", g_name);
	LOG_MESG(ENG_EDIC::en_gfsc, tzMesg);

	/* 패킷 송신 */
	return SendData(ENG_PCGG::en_pcgg_new_gerber_path, PUINT8(pszGName), UINT32(strlen(pszGName)));
}

/*
 desc : 거버 파일 경로 명 삭제 명령 전달
 parm : g_name	- [in]  삭제될 거버 명 (전체 경로는 미포함. 기본 경로 하위의 삭제될 거버 폴더 명)
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::SetDelGerberPath(PTCHAR g_name)
{
	TCHAR tzMesg[128] = {NULL};
	PCHAR pszGName	= NULL;
	CUniToChar csCnv;

	if (!g_name)	return FALSE;
	if (wcslen(g_name) < 1)	return FALSE;

	/* Ansi Code 변환 */
	pszGName	= csCnv.Uni2Ansi(g_name);
	if (!pszGName)	return FALSE;

	swprintf_s(tzMesg, 128, L"Delete the gerber path (%s)", g_name);
	LOG_MESG(ENG_EDIC::en_gfsc, tzMesg);

	/* 패킷 송신 */
	return SendData(ENG_PCGG::en_pcgg_del_gerber_path, PUINT8(pszGName), UINT32(strlen(pszGName)));
}

/*
 desc : 임시 메모리에 등록 (저장)된 거버 파일의 정보 읽고, 송신 작업 수행
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

	/* 일단, 무조건 실패 라고 설정 */
	if (m_syncPktCtrl.Enter())
	{
		m_bSendPktGFiles	= FALSE;
		m_syncPktCtrl.Leave();
	}

	/* 현재 송신하고자 하는 거버 파일의 인덱스 값이 전체 개수보다 크거나 같다면, 모두 전송 완료된 것임 */
	if (m_u32FileCount >= m_u32TotalFiles)
	{
		/* 거버 (거버 파일 혹은 관련 기타 파일) 파일 모두 전송 완료 했다고 알림 */
		SendData(ENG_PCGG::en_pcgg_send_gerber_comp, NULL, 0);
		return FALSE;
	}

	/* 현재 송신할 거버 파일 정보 얻기 */
	pPos	= m_lstPktGFiles.FindIndex(m_u32FileCount);
	if (!pPos)
	{
		swprintf_s(tzMesg, 128,
				   L"There are currently no Gerber files [%d th] to transfer", m_u32FileCount);
		LOG_ERROR(ENG_EDIC::en_gfsc, tzMesg);
		return FALSE;
	}

	stGFile	= m_lstPktGFiles.GetAt(pPos);

	/* 거버 파일 이름 설정 및 검색해 보기 */
	swprintf_s(tzGFile, MAX_PATH_LEN, L"%s\\%s\\%s",
			   pstConfig->gerber_path->gfsc,
			   csCnv1.Ansi2Uni(stGFile.path),
			   csCnv2.Ansi2Uni(stGFile.file));

	/* 거버 파일 적재를 위한 메모리 할당 */
	lGFileSize	= stGFile.s_size + sizeof(STG_PGFP);
	pGFileOrg	= (PUINT8)::Alloc(lGFileSize + 1);
	memset(pGFileOrg, 0x00, lGFileSize + 1);
	pGFileData	= pGFileOrg + sizeof(STG_PGFP);
	/* 나머지 거버 파일 읽어서 임시 메모리에 넣기 */
	if (!uvCmn_LoadFile(tzGFile, pGFileData, stGFile.s_size))
	{
		::Free(pGFileOrg);
		swprintf_s(tzMesg, 128, L"Failed to load the [file:%s] [size:%d]",
				   tzGFile, stGFile.s_size);
		LOG_ERROR(ENG_EDIC::en_gfsc, tzMesg);
		return FALSE;
	}
	/* 헤더 부분은 압축 여부에 따라 압축 파일 크기 값이 달라지므로 계산 후에 복사 작업 진행 */
	stGFile.SwapData();
	memcpy(pGFileOrg, &stGFile, sizeof(STG_PGFP));

	/* 압축했을 경우, 최대 압축 공간 크기 값 반환 */
	lGZipedSize	= snappy_max_compressed_length(lGFileSize);
	/* 압축 여부에 따라 송신해야 하므로 (!!! 꼭 본문 패킷 7 바이트 공간 여부 추가 확보 !!!) */
	pGZipOrg	= (PUINT8)::Alloc(lGZipedSize + u32Head);	/* !!! 압축 공간은 원본 파일 대비 일단 2배로 확보 !!! */

	ASSERT(pGZipOrg);
	/* !!! 패킷 본문 7 바이트 뒷 부분부터 압축된 거버 정보 (구조체 포함)가 포함되어야 함 !!! */
	pGZipData	= pGZipOrg + u32Head;
	/* 헤더 부분 복사 */
	if (!pstConfig->common->use_zip_file)
	{
		/* 원본 파일 그대로 복사 */
		memcpy(pGZipData, pGFileOrg, lGFileSize);
		lGZipedSize	= lGFileSize;
		u32SendSize	= UINT32(lGZipedSize) + u32Head;	/* 일단, 원본 크기를 압축 파일 크기로 저장 */
	}
	else
	{
		/* 원본 파일 압축 작업 진행 */
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

		u32SendSize	= UINT32(lGZipedSize) + u32Head;	/* 일단, 원본 크기를 압축 파일 크기로 저장 */
	}

	/* 헤더 부분 설정 */
	u32SendFiles	= SWAP32(m_u32FileCount + 1);	/* 1-based 부터 송신해야 하기 때문에 */
	memcpy(pGZipOrg, &u32SendFiles, 4);
	if (stGFile.file_no == 0x0000)					pGZipOrg[4]	= 0x00;
	else if (m_u32FileCount == (m_u32TotalFiles-1))	pGZipOrg[4]	= 0x02;
	else											pGZipOrg[4]	= 0x01;
	lGZipedSize	= SWAP32(lGZipedSize);	/* 전체 송신되는 패킷의 크기 */
	memcpy(pGZipOrg+5, &lGZipedSize, 4);

	/* 현재 압축 혹은 비압축된 거버 파일 송신 진행 */
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

	/* 메모리 해제 */
	::Free(pGFileOrg);
	::Free(pGZipOrg);

	/* 아직 송신할 데이터가 남아 있다면 ... */
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
 desc : 서버에게 Recipe Files 요청
 parm : None
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::GetRecipeFiles()
{
	LOG_MESG(ENG_EDIC::en_gfsc, L"Request the recipe files");
	/* 패킷 송신 */
	return SendData(ENG_PCGG::en_pcgg_get_recipe_files, NULL, 0);
}

/*
 desc : Server로부터 Client에 저장 (관리)된 레시피 파일을 요청 받은 경우
 parm : None
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::SetRecipeFiles()
{
	BOOL bSended		= FALSE;
	LPG_CSBP pstRecipe	= uvGFSP_Info_GetData()->recipe_path;
	CStringArray arrRecipeFiles;

	LOG_MESG(ENG_EDIC::en_gfsc, L"Request to recipe files");

	/* Server 내에 Recipe Files 검색 */
	uvCmn_SearchRecurseFile(pstRecipe->gfsc, L".dat", arrRecipeFiles, 0x00);
	/* 만약 검색된 레시피가 없다면 ... */
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
 desc : 원격 (Client) 시스템의 임의 경로에 거버 경로 생성 요청에 대한 응답 처리
 parm : data	- [in]  패킷 본문 (head & tail 제외) 내용이 저장된 버퍼 포인터
		size	- [in]  'data' 버퍼에 저장된 패킷의 크기
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::NewGerberPath(PUINT8 data, UINT32 size)
{
	TCHAR tzMesg[128]	= {NULL};

	if (size != 0x01)	data[0]	= 0xfe;

	/* 일단, 기본 값으로 에러 레벨로 설정 */
	m_enLastErrorLevel	= ENG_LNWE::en_error;

	/* 로그 등록 */
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
 desc : 원격 (Client) 시스템의 임의 경로에 거버 경로 삭제 요청에 대한 응답 처리
 parm : data	- [in]  패킷 본문 (head & tail 제외) 내용이 저장된 버퍼 포인터
		size	- [in]  'data' 버퍼에 저장된 패킷의 크기
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::DelGerberPath(PUINT8 data, UINT32 size)
{
	TCHAR tzMesg[128]		= {NULL};

	/* 일단, 기본 값으로 에러 레벨로 설정 */
	m_enLastErrorLevel	= ENG_LNWE::en_error;

	if (size != 0x01)	data[0]	= 0xfe;

	/* 로그 등록 */
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
 desc : 원격 (Client) 시스템에 전달될 거버 파일 개수 요청에 대한 응답 처리
 parm : data	- [in]  패킷 본문 (head & tail 제외) 내용이 저장된 버퍼 포인터
		size	- [in]  'data' 버퍼에 저장된 패킷의 크기
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::CntGerberFiles(PUINT8 data, UINT32 size)
{
	UINT8 u8Flag		= 0x00;
	TCHAR tzMesg[128]	= {NULL};
	UINT32 u32PktAck	= 0x0000;

	/* 일단, 기본 값으로 에러 레벨로 설정 */
	m_enLastErrorLevel	= ENG_LNWE::en_error;

	if (size != 0x04)	u8Flag	= 0xfe;
	else
	{
		memcpy(&u32PktAck, data, size);
		u32PktAck	= SWAP32(u32PktAck);
		if (m_u32TotalFiles != u32PktAck)	u8Flag	= 0xff;
	}

	/* 로그 등록 */
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
 설명: Server로부터 수신된 레시피 파일 저장
 parm : data	- [in]  패킷 본문 (head & tail 제외) 내용이 저장된 버퍼 포인터
		size	- [in]  'data' 버퍼에 저장된 패킷의 크기
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

	/* 잘못된 패킷 검증 */
	if (size < 2)	return FALSE;

	/* 기존에 저장되어 있던 파일 모두 삭제 */
	uvCmn_DeleteAllFiles(pstRecipe->gfsc);

	/* 일단 수신된 파일 개수 확인 */
	memcpy(&u16Count, pPktData, 2);	pPktData += 2;
	u16Count	= SWAP16(u16Count);
	/* 수신된 파일 개수 */
	m_u32TotalFiles	= u16Count;
	m_u32TotalSizes	= 0;
	if (u16Count < 1)
	{
		LOG_WARN(ENG_EDIC::en_gfsc, L"There are no recipe files on server system");
	}
	else
	{
		/* 파일 개수만큼 반복 */
		for (i=0; i<u16Count; i++)
		{
			/* 레시피 파일 얻기 */
			memset(szFile, 0x00, RECIPE_FILE_NAME);
			memcpy(szFile, pPktData, RECIPE_FILE_NAME);				pPktData += RECIPE_FILE_NAME;
			sprintf_s(szRecipe, MAX_PATH_LEN, "%s\\%s", csCnv.Uni2Ansi(pstRecipe->gfsc), szFile);
			/* 레시피 파일 크기 얻기 */
			memcpy(&u32Size, pPktData, 4);							pPktData += 4;
			u32Size	= SWAP32(u32Size);
			/* 총 수신된 파일 크기 저장 */
			m_u32TotalSizes	+= u32Size;
			/* 파일 저장 방식 */
			u8Mode	= pPktData[0];									pPktData += 1;
			/* 파일 저장 */
			uvCmn_SaveBinFile(pPktData, u32Size, szRecipe);			pPktData += u32Size;
		}
	}

	return TRUE;
}

/*
 설명: Server로부터 수신된 레시피 성공 여부 값
 parm : data	- [in]  패킷 본문 (head & tail 제외) 내용이 저장된 버퍼 포인터
		size	- [in]  'data' 버퍼에 저장된 패킷의 크기
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::SetRecipeFiles(PUINT8 data, UINT32 size)
{
	/* 잘못된 패킷 검증 */
	if (size < 1)	return FALSE;

	/* 일단 수신된 파일 개수 확인 */
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
 desc : Client로부터 수신된 패킷 헤더 부분 임시 등록 (App. 쪽에서 어떤 명령어들이 수신되었는지 알기 위함)
 parm : head	- [in]  수신된 패킷 헤더 정보의 구조체 포인터
 retn : None
*/
VOID CGFSCThread::PushPktHead(LPG_UTPH head)
{
	/* 동기화 진입 */
	if (m_syncPktCmd.Enter())
	{
		/* 기존에 등록된 개수 (버퍼)가 초과되면 가장 오래된 항목 제거 */
		if (m_lstPktCmd.GetCount() > MAX_PKT_LAST_RECV_SIZE)
			m_lstPktCmd.RemoveHead();
		/* 새로 발생된 패킷 헤더 등록 */
		m_lstPktCmd.AddTail(*head);

		/* 동기화 해제 */
		m_syncPktCmd.Leave();
	}
}

/*
 desc : Client로부터 수신된 패킷 헤더 부분 반환
 parm : head	- [out] 반환되어 저장될 패킷 헤더 구조체 참조
 retn : TRUE (반환된 데이터 있음) or FALSE (반환될 패킷 헤더 없음)
*/
BOOL CGFSCThread::PopPktHead(STG_UTPH &head)
{
	BOOL bPopped	= FALSE;

	/* 동기화 진입 */
	if (m_syncPktCmd.Enter())
	{
		if (m_lstPktCmd.GetCount() > 0)
		{
			/* 가장 오래된 패킷 헤더 데이터 Popped !!! */
			head = m_lstPktCmd.GetHead();
			/* 가장 오래된 패킷 헤더 제거 */
			m_lstPktCmd.RemoveHead();
			/* 정상적으로 Popped 했다고 플래그 설정 */
			bPopped	= TRUE;
		}

		/* 동기화 해제 */
		m_syncPktCmd.Leave();
	}

	return bPopped;
}

/*
 desc : Client에서 Server로 거버 파일 (거버 파일 혹은 거버 관련 기타 파일)을 모두 송신했는지 여부
 parm : None
 retn : TRUE or FALSE
*/
BOOL CGFSCThread::IsSendGFileCompleted()
{
	if (m_u32TotalFiles < 1 || m_u32FileCount < 1)	return FALSE;
	return (m_u32FileCount == m_u32TotalFiles);
}

/*
 desc : 현재까지 거버 파일 (거버 파일 혹은 거버 관련 기타 파일)이 전송된 비율 값 반환
 parm : None
 retn : 전송률 값
*/
DOUBLE CGFSCThread::GetSendGFilesRate()
{
	if (m_u32TotalFiles < 1 || m_u32FileCount < 1)	return 0.0f;

	return (DOUBLE(m_u32FileCount) / DOUBLE(m_u32TotalFiles) * 100);
}

/*
 desc : 현재 송신 중이면 중지 처리
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
 desc : Server에 저장된 레시피 파일 송신
 parm : files	- [in]  현재 송신될 레시피 파일명이 저장되어 있는 문자열 배열 버퍼
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

	/* 배열에 저장된 레시피 파일들의 전체 크기 구하기 */
	u16Count		= (UINT16)files->GetCount();
	m_u32TotalFiles	= u16Count;
	for (i=0; i<u16Count; i++)
	{
		/* 현재 파일의 크기 얻기 (단위: Bytes) */
		u32Size	= uvCmn_GetFileSize(CString(files->GetAt(i)).GetBuffer(), L"rt");
		/* 현재 파일의 크기 임시 저장 */
		lstFileSize.AddTail(u32Size);

		/* 총 할당될 패킷 버퍼 크기를 구하기 위해서 */
		u32Total+= u32Size;
		u32Total+= RECIPE_FILE_NAME;
		u32Total+= 4;	/* recipe file size */
		u32Total+= 1;	/* file mode: 0x00 (text file) or 0x01 (binary file) */

		/* 전체 송신될 파일 개수의 총 크기 */
		m_u32TotalSizes	+= u32Size;
	}

	/* 레시피 기본 경로 길이 */
	u32Recipe	= (UINT32)wcslen(uvGFSP_Info_GetData()->recipe_path->gfsc);
	/* 전체 파일을 송신할 버퍼 할당 */
	pPktBuff	= (PUINT8)::Alloc(u32Total+1);
	memset(pPktBuff, 0x00, u32Total+1);
	pPktData	= pPktBuff;

	/* 패킷 구성 */
	u16Count	= SWAP16(u16Count);
	memcpy(pPktData, &u16Count, 2);	pPktData	+= 2;
	for (i=0; i<SWAP16(u16Count); i++)
	{
		/* 레시피 파일 (길이 고정) */
		pszRecipe	= csCnv.Uni2Ansi(CString(files->GetAt(i)).GetBuffer());
		pszRecipe	+= (u32Recipe + 1);
		memcpy(pPktData, pszRecipe, strlen(pszRecipe));	pPktData += RECIPE_FILE_NAME;
		/* 파일 크기 */
		u32Size	= lstFileSize.GetAt(lstFileSize.FindIndex(i));
		u32Size	= SWAP32(u32Size);
		memcpy(pPktData, &u32Size, 4);					pPktData += 4;
		u32Size	= SWAP32(u32Size);	/* 다시 원복 */
		/* 파일 모드 (무조건 텍스트) */
		pPktData[0]	= 0x00;	/* Text Files */			pPktData += 1;
		/* 레시피 내용 복사 (텍스트 파일 일지라도 읽어들일 때는 꼭 바이너리 모드로 읽어야, 정상적인 크기를 알 수 있음 */
		u32Read	= uvCmn_LoadFile(CString(files->GetAt(i)).GetBuffer(), pPktData, u32Size, 0x01);
		if (u32Read < 1 || u32Read != u32Size)
		{
			bSended	= FALSE;
			break;
		}
		pPktData += u32Read;	/* 다음 저장 위치로 이동 */
	}

	/* 용량이 크므로, 송신할 때, 일정 시간 Time out 값 꼭 부여 */
	if (!bSended)
	{
		LOG_ERROR(ENG_EDIC::en_gfsc, L"Failed to create packet for sending recipe file");
	}
	else
	{
		bSended	= SendData(ENG_PCGG::en_pcgg_set_recipe_files, pPktBuff, u32Total, 5000);
	}

	/* 메모리 해제 */
	::Free(pPktBuff);
	lstFileSize.RemoveAll();

	return bSended;
}

