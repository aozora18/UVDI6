
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
 desc : 생성자
 parm : th_id	- [in]  thread id
		server	- [in]  Client 연결 정보
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
	/* 가장 최근에 수신받을 거버 관련 파일의 개수 */
	m_u32TotalFiles		= 0;
	m_u32FileCount		= 0;
	m_u32TotalSizes		= 0;

	/* Live Check 송신 간격 설정 (Idle Time 대비 절반으로 설정) */
	m_u64PeriodLiveTime	= m_u64IdleTime / 2;
	m_u64LastLiveTime	= 0;

	wmemset(m_tzLastNewDir, 0x00, MAX_PATH_LEN);
	wmemset(m_tzLastDelDir, 0x00, MAX_PATH_LEN);
	wmemset(m_tzLastSaveDir, 0x00, MAX_PATH_LEN);
	/* 환경 파일 저장 */
	m_pstConfig			= config;
	/* 수신 버퍼 초기화 (대용량 수신 버퍼 적용) */
	UINT32 u32Size		= 1024 * 1024 * 64;
	m_pRecvBuff			= (PUINT8)::Alloc(u32Size+1);
	ASSERT(m_pRecvBuff);
	memset(m_pRecvBuff, 0x00, u32Size+1);
	/* 가장 최근에 수신받기 시작한 시간 초기화 */
	m_u64RecvStartTime	= GetTickCount64();
	m_bIsRecvCompleted	= FALSE;
}

/*
 desc : Destructor
 parm : None
*/
CGFSSThread::~CGFSSThread()
{
	/* 수신 버퍼 해제 (대용량 수신 버퍼 해제)*/
	if (m_pRecvBuff)	::Free(m_pRecvBuff);
	m_pRecvBuff	= NULL;

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
VOID CGFSSThread::DoWork()
{
	/* 연결이 안된 상태인지 확인*/
	if (!IsConnected())	return;

	/* Idle Time 값이 0보다 큰 경우만 동작 */
	if (GetTickCount64() > (m_u64LastLiveTime + m_u64PeriodLiveTime))
	{
		/* Live Time 송신 */
		SetAliveTime();
		/* 가장 최근에 송신한 시간 저장 */
		m_u64LastLiveTime	= GetTickCount64();
	}
}

/*
 desc : 클라이언트 연결이 들어온 경우
 parm : sd	- 소켓 기술자
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
 desc : 서버로부터 연결 해제가 된 경우
 parm : None
 retn : None
*/
VOID CGFSSThread::CloseSocket(SOCKET sd, UINT32 time_out/* =0 */)
{
	/* 상위 객체 호출 */
	CServerThread::CloseSocket(sd, time_out);
}

/*
 desc : 소켓 데이터 송신
 parm : data	- [in]  전송할 데이터가 저장된 버퍼 포인터
		size	- [in]  'data' 버퍼에 저장된 데이터의 크기
		time_out- [in]  Timeout 값 지정. 단위: 밀리초 (0이면, 사용 안함)
 retn : TRUE - 송신 성공(비록 송신된 데이터가 없더라도),
		FALSE - 송신 실패(소켓 에러... 연결을 해제해야 함)
*/
BOOL CGFSSThread::SendData(PUINT8 data, UINT32 size, UINT64 time_out)
{
	BOOL bSucc			= FALSE;
	TCHAR tzMesg[128]	= {NULL};

	/* 소켓을 송신 중 모드로 변경 */
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
 desc : 응답 패킷 전달
 parm : cmd		- [in]  패킷 명령어
		data	- [in]  패킷 본문 데이터가 저장된 버퍼
		size	- [in]  'data' 버퍼 에 저장된 데이터 본문 크기
		time_out- [in]  Timeout 값 지정. 단위: 밀리초 (0이면, 사용 안함)
 retn : TRUE or FALSE
*/
BOOL CGFSSThread::SendData(ENG_PCGG cmd, PUINT8 data, UINT32 size, UINT64 time_out)
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
BOOL CGFSSThread::RecvData()
{
	BOOL bIsTimeout	= FALSE;
	BOOL bRecvState	= TRUE;
	BOOL bRecvAlive	= FALSE;	/* Alive Check Packet 수신 여부  */
	UINT32 u32Size	= 0, u32Read;
	STG_UTPH stHead	= {NULL};
	PUINT8 pPktBuff	= NULL;

	/* 무한 루프 시간 초기화 */
	uvGFSP_ResetLoopTime();

	/* Recv to packet */
	m_enSocket	= ENG_TPCS::en_recving;


	while (1)
	{
		/* 현재 대용량(?) 버퍼 포인터 연결 */
		pPktBuff= m_pRecvBuff;

		/* 현재 버퍼에 남아 있는 데이터의 크기 확인 */
		u32Read	= m_pPktRingBuff->GetReadSize();

		/* 현재 수신된 데이터가 기본 헤더 정보 크기만큼 저장되어 있는지 확인 */
		if (u32Read < sizeof(STG_UTPH))	break;	// !!! 에러 아님 !!!

		/* 버퍼에 완전한 패킷이 모두 저장되어 있는지 확인 or 부족하다면 대기 */
		m_pPktRingBuff->CopyBinary((PUINT8)&stHead, sizeof(STG_UTPH));
		/* 현재 수신될 데이터의 크기 */
		u32Size = SWAP32(stHead.length) - sizeof(STG_UTPH);
		/* 거버 파일 크기만큼 저장되어 있는지 확인 */
		if (u32Read < u32Size)	break;	// !!! 에러 아님 !!!
		/* 먼저 기존에 Copy된 응답 헤더 부분을 버퍼로부터 읽은 후 버림 */
		m_pPktRingBuff->PopBinary(sizeof(STG_UTPH));
		/* 헤더 부분을 제외한 나머지가 저장되어 크기 다시 계산 */
		memcpy(pPktBuff, &stHead, sizeof(STG_UTPH));
		pPktBuff+= sizeof(STG_UTPH);

		/* 총 수신되어야할 만큼 데이터가 링 버퍼에 저장되어 있는지 읽기 요청 */
		/* u32Size의 크기가 0 Bytes 인 경우, 본문 데이터가 없다고 판단 한다. */
		if (u32Size)	m_pPktRingBuff->ReadBinary(pPktBuff, u32Size);

		/* Alive Check Packet 수신인지 여부 */
		if (SWAP16(stHead.cmd) == (UINT16)ENG_PCUC::en_comn_alive_check)	bRecvAlive	= TRUE;
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
		{
			LOG_ERROR(ENG_EDIC::en_gfss, L"GFSS : A timeout occurred while receiving data");
			bIsTimeout	= TRUE;
			break;
		}
	}

	/* Alive Check에 대해 응답 송신 */
	if (bRecvAlive)	bRecvState = SendAliveTime();

	/* Recv to packet */
	if (!bIsTimeout)	m_enSocket	= ENG_TPCS::en_recved;
	else				m_enSocket	= ENG_TPCS::en_recv_failed;

	return bRecvState;
}

/*
 desc : 라이브러리 데이터 처리
 parm : data	- [in]  이벤트 데이터가 저장된 버퍼 포인터
		size	- [in]  'data' 버퍼에 저장된 데이터의 크기
 retn : TRUE or FALSE (연결 해제? 잘못된 데이터의 성격이 수신됨)
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
 desc : 거버 파일 (거버 파일 및 기타 관련 파일) 을 저장할 경로 명 생성
 parm : data	- [in]  패킷 본문 (head & tail 제외) 내용이 저장된 버퍼 포인터
		size	- [in]  'data' 버퍼에 저장된 패킷의 크기
 retn : TRUE or FALSE
*/
BOOL CGFSSThread::NewGerberPath(PUINT8 data, UINT32 size)
{
	UINT8 u8Flag		= 0x02;	/* 폴더 생성 실패로 초기 값 설정 */
	CHAR szGPath[GERBER_PATH_NAME]	= {NULL};	/* 거버 (거버 파일 및 기타 관련 파일) 폴더 명 */
	TCHAR tzMesg[128]	= {NULL};
	CUniToChar csCnv;

	/* 일단, 기본 값으로 에러 레벨로 설정 */
	m_enLastErrorLevel	= ENG_LNWE::en_error;

	if (size > GERBER_PATH_NAME /* Max Gerber Path */)
	{
		swprintf_s(tzMesg, 128, L"Invalid packet information [cmd: generate gerber path]");
		if (wcslen(tzMesg))	LOG_SAVED(ENG_EDIC::en_gfss, m_enLastErrorLevel, tzMesg);
	}
	else
	{
		/* Gerber Name 추출 */
		memcpy_s(szGPath, GERBER_PATH_NAME, data, size);
#if 0
		/* 전체 경로 구성 : 기본 경로 + 거버 이름 (명) */
		swprintf_s(tzGPath, MAX_PATH_LEN, L"%s\\%s", m_pstConfig->gerber_path->gfss, csCnv.Ansi2Uni(szGPath));
		/* 기존 거버 (거버 파일 및 기타 관련 파일) 폴더가 존재하면, 무조건 삭제 */
		if (uvCmn_FindPath(tzGPath))
		{
			if (!uvCmn_RemoveDirectory(tzGPath))	u8Flag	= 0x01;	/* 기존 거버 폴더가 있고, 이 폴더를 지우는데 실패한 경우 */
		}
		/* 새로운 거버 (거버 파일 및 기타 관련 파일) 경로 (디렉토리) 생성 */
		if (0x00 == u8Flag)
		{
			/* 하위 디렉토리도 있으면, 알아서 생성됨 */
			if (!uvCmn_CreateDirectory(tzGPath))	u8Flag	= 0x02;	/* 거버 (거버 파일 및 기타 관련 파일) 경로에 특수 문자 및 기타 기호가 포함된 경우 */
		}

		/* 로그 등록 */
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

	/* 응답 패킷 전달 */
	return SendData(ENG_PCGG::en_pcgg_new_gerber_path_ack, &u8Flag, 1);
}

/*
 desc : 거버 파일을 저장할 경로 명 생성
 parm : data	- [in]  패킷 본문 (head & tail 제외) 내용이 저장된 버퍼 포인터
		size	- [in]  'data' 버퍼에 저장된 패킷의 크기
 retn : TRUE or FALSE
*/
BOOL CGFSSThread::DelGerberPath(PUINT8 data, UINT32 size)
{
	UINT8 u8Flag	= 0x00;
	CHAR szGPath[GERBER_PATH_NAME]	= {NULL};	/* 거버 폴더 명 */
	TCHAR tzPath[MAX_PATH_LEN]		= {NULL};	/* 전체 경로 (드라이브 포함) */
	TCHAR tzMesg[LOG_MESG_SIZE]		= {NULL};
	CUniToChar csCnv;

	/* 일단, 기본 값으로 에러 레벨로 설정 */
	m_enLastErrorLevel	= ENG_LNWE::en_error;

	if (size > GERBER_PATH_NAME /* Max Gerber Path */)
	{
		swprintf_s(tzMesg, LOG_MESG_SIZE, L"Invalid packet information [cmd: delete gerber path]");
	}
	else
	{
		/* Gerber Name 추출 */
		memcpy_s(szGPath, GERBER_PATH_NAME, data, size);
		/* 전체 경로 구성 : 기본 경로 + 거버 이름 (명) */
		swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\%s",
				   m_pstConfig->gerber_path->gfss, csCnv.Ansi2Uni(szGPath));
		/* 기존 거버 폴더가 존재하면, 무조건 삭제 */
		if (!uvCmn_FindPath(tzPath))				u8Flag	= 0x01;	/* 경로 (디렉토리)가 존재하지 않음 */
		else if (!uvCmn_RemoveDirectory(tzPath))	u8Flag	= 0x02;	/* 기존 거버 폴더가 있고, 이 폴더를 지우는데 실패한 경우 */

		/* 로그 등록 */
		switch (u8Flag)
		{
		case 0x00 :	m_enLastErrorLevel	= ENG_LNWE::en_normal;
					swprintf_s(tzMesg, LOG_MESG_SIZE, L"Succeeded to delete the gerber directory [%s]", csCnv.Ansi2Uni(szGPath));	break;
		case 0x01 :	swprintf_s(tzMesg, LOG_MESG_SIZE, L"Failed to find the gerber directory [%s]", csCnv.Ansi2Uni(szGPath));		break;
		case 0x02 :	swprintf_s(tzMesg, LOG_MESG_SIZE, L"Failed to delete the gerber directory [%s]", csCnv.Ansi2Uni(szGPath));		break;
		}
	}

	if (wcslen(tzMesg))	LOG_SAVED(ENG_EDIC::en_gfss, m_enLastErrorLevel, tzMesg);

	/* 가장 최근에 삭제된 경로 명 저장 */
	wmemset(m_tzLastDelDir, 0x00, MAX_PATH_LEN);
	wcscpy_s(m_tzLastDelDir, MAX_PATH_LEN, csCnv.Ansi2Uni(szGPath));

	/* 응답 패킷 전달 */
	return SendData(ENG_PCGG::en_pcgg_del_gerber_path_ack, &u8Flag, 1);
}

/*
 desc : 총 수신 받을 거버 파일의 개수
 parm : data	- [in]  패킷 본문 (head & tail 제외) 내용이 저장된 버퍼 포인터
		size	- [in]  'data' 버퍼에 저장된 패킷의 크기
 retn : TRUE or FALSE
*/
BOOL CGFSSThread::SetGerberCount(PUINT8 data, UINT32 size)
{
	TCHAR tzMesg[128]	= {NULL};
	UINT32 u32SendTotal	= 0;

	/* 일단, 기본 값으로 에러 레벨로 설정 */
	m_enLastErrorLevel	= ENG_LNWE::en_error;

	/* 이전에 수신받을 거버 파일의 개수 초기화 */
	m_u32TotalFiles		= 0;
	m_u32FileCount		= 0;
	m_u32TotalSizes		= 0;
	m_u32TotalZipSize	= 0;
	if (size == 4)
	{
		/* 거버 파일의 개수 설정 */
		memcpy(&m_u32TotalFiles, data, size);
		u32SendTotal		= m_u32TotalFiles;
		m_u32TotalFiles		= SWAP32(u32SendTotal);
		m_enLastErrorLevel	= ENG_LNWE::en_normal;
	}
	swprintf_s(tzMesg, 128, L"The number of gerber files to receive is [ %d ]", m_u32TotalFiles);
	LOG_SAVED(ENG_EDIC::en_gfss, m_enLastErrorLevel, tzMesg);

	/* 응답 패킷 전달 */
	return SendData(ENG_PCGG::en_pcgg_cnt_gerber_files_ack, (PUINT8)&u32SendTotal, 4);
}

/*
 desc : 거버 파일이 수신된 경우
 parm : data	- [in]  패킷 본문 (head & tail 제외) 내용이 저장된 버퍼 포인터
		size	- [in]  'data' 버퍼에 저장된 패킷의 크기
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

	/* 수신된 데이터 거버 파일 번호 */
	memcpy(&u32GFile, pData, 4);		pData	+= 4;
	/* 수신된 거버 파일의 처음 / 중간 / 끝 부분 중 어디? */
	u8GFileFlag	= pData[0];				pData	+= 1;
	/* 압축된 데이터 크기 */
	memcpy (&lGUnzipedSize, pData, 4);	pData	+= 4;		/* SWAP32 효과 */
	/* 값 변환 */
	u32GFile		= SWAP32(u32GFile);
	lGUnzipedSize	= SWAP32(lGUnzipedSize);
	/* 거버 파일 수신 시작 시간 초기화 */
	if (u32GFile == 1)
	{
		m_u64RecvStartTime	= GetTickCount64();
		m_bIsRecvCompleted	= FALSE;
	}

	/* 일단, 기본 값으로 에러 레벨로 설정 */
	m_enLastErrorLevel	= ENG_LNWE::en_error;

	/* 파일 압축 여부에 따라 */
	lGUnzipReserved	= lGUnzipedSize;				/* 일단, 압축하지 않았다는 가정하에 거버 파일 크기 임시 저장 */
	m_u32TotalZipSize	+= (UINT32)lGUnzipedSize;	/* 현재까지 수신된 압축된 파일의 총 크기 (단위: Bytes) */
	if (pstConfig->common->use_zip_file)
	{
		/* 압축 해제 예상 버퍼 크기 */
		enStatus	= snappy_uncompressed_length((PCHAR)pData, lGUnzipedSize, &lGUnzipReserved);
		if ((enStatus != SNAPPY_OK) || (lGUnzipReserved < 1))
		{
			LOG_SAVED(ENG_EDIC::en_gfss, m_enLastErrorLevel, L"Invalid zip file size");
			return FALSE;
		}
	}

	/* 압축 해제 후 저장될 버퍼 메모리 할당 */
	pGUnzipOrg	= (PUINT8)::Alloc(lGUnzipReserved+1);
	memset(pGUnzipOrg, 0x00, lGUnzipReserved+1);
	if (pstConfig->common->use_zip_file)
	{
		/* 압축 해제 */
		enStatus	= snappy_uncompress((PCHAR)pData, lGUnzipedSize, (PCHAR)pGUnzipOrg, &lGUnzipReserved);
		if ((enStatus != SNAPPY_OK) || (lGUnzipReserved < 1))
		{
			LOG_ERROR(ENG_EDIC::en_gfss, L"Failed to decompress the gerber file");
			return FALSE;
		}
	}
	/* 압축하지 않은 경우, 수신받은 원본 거버 파일 내용 임시 버퍼에 복사 수행 */
	else
	{
		memcpy(pGUnzipOrg, pData, lGUnzipReserved);
	}

	/* 임시 포인터 저장 */
	pGUnzipData	= pGUnzipOrg;
	/* 거버 압축 파일 부분 중 헤더 파일 부분 복사 */
	memcpy(&stGFile, pGUnzipData, sizeof(STG_PGFP));	pGUnzipData	+= sizeof(STG_PGFP);

	/* 헤더 정보 Network Byte Order 변환 */
	stGFile.SwapData();
	sprintf_s(szMesg, LOG_MESG_SIZE, "[%02d] [%s] [%s] [%d bytes]",
			  stGFile.file_no, stGFile.path, stGFile.file, stGFile.s_size);
	LOG_MESG(ENG_EDIC::en_gfss, csCnv1.Ansi2Uni(szMesg));

#ifdef _DEBUG
	TRACE(L"Recv file number = [%d] %d [%s]\n",
		  u8GFileFlag, u32GFile, csCnv2.Ansi2Uni(stGFile.file));
#endif

	/* 거버 파일 원본 크기 구하기 */
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

	/* 파일의 처음인지 아닌지 여부에 따라 폴더 생성할지 여부 확인 */
	if (u8GFileFlag == 0x00)
	{
		if (0X00 != NewGerberPath(csCnv1.Ansi2Uni(stGFile.path)))
		{
			return FALSE;
		}
	}
	/* 거버 파일 저장 */
	if (!SaveGerberFile(csCnv1.Ansi2Uni(stGFile.path), csCnv2.Ansi2Uni(stGFile.file),
						pGUnzipData, (UINT32)lGUnzipReserved, u32GFile))
	{
		return FALSE;
	}

	/* 정상적으로 모두 처리한 경우, 에러 없다고 플래그 설정 */
	m_enLastErrorLevel	= ENG_LNWE::en_normal;

	return TRUE;
}

/*
 desc : 새로운 거버 (거버 파일 혹은 관련 기타 파일) 폴더 생성
 parm : path	- [in]  새로 생성될 거버 명 (전체 경로 미포함)
 retn : 0x00 - 새로운 거버 폴더 생성 성공
		0x01 - 기존 거버 폴더 삭제 실패
		0x02 - 새로운 거버 폴더 생성 실패
*/
UINT8 CGFSSThread::NewGerberPath(PTCHAR path)
{
	UINT8 u8Flag		= 0x00;
	TCHAR tzMesg[LOG_MESG_SIZE]	= {NULL}, tzGPath[MAX_PATH_LEN] = {NULL};
	LPG_CSID pstConfig	= uvGFSP_Info_GetData();

	/* 현재 저장하려는 폴더 구성 후, 로컬 저장 장치에 존재하는지 확인 */
	swprintf_s(tzGPath, MAX_PATH_LEN, L"%s\\%s", pstConfig->gerber_path->gfss, path);
#if 0
	/* 현재 저장하려는 폴더 검색 후, 실패하면, 폴더 생성 진행 */
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
	/* 기존 거버 (거버 파일 및 기타 관련 파일) 폴더가 존재하면, 무조건 삭제 */
	if (uvCmn_FindPath(tzGPath))
	{
		if (!uvCmn_RemoveDirectory(tzGPath))	u8Flag	= 0x01;	/* 기존 거버 폴더가 있고, 이 폴더를 지우는데 실패한 경우 */
	}
	/* 새로운 거버 (거버 파일 및 기타 관련 파일) 경로 (디렉토리) 생성 */
	if (0x00 == u8Flag)
	{
		/* 하위 디렉토리도 있으면, 알아서 생성됨 */
		if (!uvCmn_CreateDirectory(tzGPath))	u8Flag	= 0x02;	/* 거버 (거버 파일 및 기타 관련 파일) 경로에 특수 문자 및 기타 기호가 포함된 경우 */
	}

	/* 로그 등록 */
	switch (u8Flag)
	{
	case 0x00 :	m_enLastErrorLevel	= ENG_LNWE::en_normal;
				swprintf_s(tzMesg, LOG_MESG_SIZE, L"Succeeded to create the gerber directory [%s]", tzGPath);	break;
	case 0x01 :	swprintf_s(tzMesg, LOG_MESG_SIZE, L"Failed to delete the gerber directory [%s]", tzGPath);		break;
	case 0x02 :	swprintf_s(tzMesg, LOG_MESG_SIZE, L"Failed to create the gerber directory [%s]", tzGPath);		break;
	}
	if (wcslen(tzMesg))	LOG_SAVED(ENG_EDIC::en_gfss, m_enLastErrorLevel, tzMesg);
#endif
	/* 가장 최근에 생성된 경로 명 저장 */
	wmemset(m_tzLastNewDir, 0x00, MAX_PATH_LEN);
	wcscpy_s(m_tzLastNewDir, MAX_PATH_LEN, tzGPath);

	return u8Flag;
}

/*
 desc : 새로운 거버 (거버 파일 혹은 관련 기타 파일) 파일 저장
 parm : path	- [in]  새로운 거버 파일이 저장될 경로
		file	- [in]  거버 (거버 파일 혹은 관련 기타 파일) 파일 (확장자 포함)
		data	- [in]  거버 파일의 내용이 저장된 버퍼 포인터
		size	- [in]  'data' 버퍼에 저장된 데이터 크기
		f_no	- [in]  현재 저장되는 파일의 번호
 retn : TRUE or FALSE
*/
BOOL CGFSSThread::SaveGerberFile(PTCHAR path, PTCHAR file, PUINT8 data, UINT32 size, UINT32 f_no)
{
	TCHAR tzMesg[128]	= {NULL}, tzGFile[MAX_PATH_LEN] = {NULL};
	ENG_LNWE enLevel	= ENG_LNWE::en_normal;
	LPG_CSID pstConfig	= uvGFSP_Info_GetData();
	CUniToChar csCnv;

	/* 현재 저장하려는 파일 열고, 저장 후 닫기 */
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
	/* 현재 수신받은 거버 파일의 순서 (번호) 값 임시 저장 */
	m_u32FileCount	= f_no;
	/* 지금까지 수신받은 거버 파일 (거버 관련 기타 파일 포함)의 전체 크기 (단위: Bytes) 계산 */
	m_u32TotalSizes	+= size;

	/* 가장 최근에 저장된 경로 명 저장 */
	wmemset(m_tzLastSaveDir, 0x00, MAX_PATH_LEN);
	wcscpy_s(m_tzLastSaveDir, MAX_PATH_LEN, path);

	LOG_SAVED(ENG_EDIC::en_gfss, enLevel, tzMesg);

	return TRUE;
}

/*
 desc : 모든 거버 파일들이 수신 완료된 경우
 parm : None
 retn : TRUE or FALSE
*/
BOOL CGFSSThread::SetGerberComp()
{
	LOG_MESG(ENG_EDIC::en_gfss, L"Completed to receive the gerber files");

	/* 가장 최근에 수신받는데 총 소요된 시간 저장 (단위: msec) */
	m_u64RecvTotalTime	= GetTickCount64() - m_u64RecvStartTime;
	m_bIsRecvCompleted	= TRUE;
	/* 아직 아무런 작업도 없음 (현재로선 굳이 할 필요가 없음) */
	return SendData(ENG_PCGG::en_pcgg_send_gerber_comp_ack, NULL, 0);
}

/*
 desc : Client로부터 Server에 저장 (관리)된 레시피 파일을 요청 받은 경우
 parm : None
 retn : TRUE or FALSE
*/
BOOL CGFSSThread::GetRecipeFiles()
{
	BOOL bSended		= FALSE;
	LPG_CSBP pstRecipe	= uvGFSP_Info_GetData()->recipe_path;
	CStringArray arrRecipeFiles;

	LOG_MESG(ENG_EDIC::en_gfss, L"Request to recipe files");

	/* Server 내에 Recipe Files 검색 */
	uvCmn_SearchRecurseFile(pstRecipe->gfss, L".dat", arrRecipeFiles, 0x00);
	/* 만약 검색된 레시피가 없다면 ... */
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
 설명: Server로부터 수신된 레시피 파일 저장
 parm : data	- [in]  패킷 본문 (head & tail 제외) 내용이 저장된 버퍼 포인터
		size	- [in]  'data' 버퍼에 저장된 패킷의 크기
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

	/* 잘못된 패킷 검증 */
	if (size < 2)	return FALSE;

	/* 기존에 저장되어 있던 파일 모두 삭제 */
	uvCmn_DeleteAllFiles(pstRecipe->gfss);

	/* 일단 수신된 파일 개수 확인 */
	memcpy(&u16Count, pPktData, 2);	pPktData += 2;
	u16Count	= SWAP16(u16Count);
	/* 수신된 파일 개수 */
	m_u32TotalFiles	= u16Count;
	m_u32TotalSizes	= 0;
	if (u16Count > 0)
	{
		u8Flag	= 0x01;	/* 업데이트 성공 */
		/* 파일 개수만큼 반복 */
		for (i=0; i<u16Count; i++)
		{
			/* 레시피 파일 얻기 */
			memset(szFile, 0x00, RECIPE_FILE_NAME);
			memcpy(szFile, pPktData, RECIPE_FILE_NAME);				pPktData += RECIPE_FILE_NAME;
			sprintf_s(szRecipe, MAX_PATH_LEN, "%s\\%s", csCnv.Uni2Ansi(pstRecipe->gfss), szFile);
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

	/* 응답 송신 */
	return SendData(ENG_PCGG::en_pcgg_set_recipe_files_ack, &u8Flag, 1);
}

/*
 desc : Client로부터 수신된 패킷 헤더 부분 임시 등록 (App. 쪽에서 어떤 명령어들이 수신되었는지 알기 위함)
 parm : head	- [in]  수신된 패킷 헤더 정보의 구조체 포인터
 retn : None
*/
VOID CGFSSThread::PushPktHead(LPG_UTPH head)
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
BOOL CGFSSThread::PopPktHead(STG_UTPH &head)
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
 desc : 현재까지 거버 파일 (거버 파일 혹은 거버 관련 기타 파일)을 수신한 비율 값 반환
 parm : None
 retn : 수신률 값
*/
DOUBLE CGFSSThread::GetTotalFilesRate()
{
	if (m_u32FileCount < 1 || m_u32TotalFiles < 1)	return 0.0f;
	return (DOUBLE(m_u32FileCount) / DOUBLE(m_u32TotalFiles) * 100.0f);
}

/*
 desc : 가장 최근에 생성 / 삭제 / 저장 (거버 관련 파일들) 된 폴더 반환
 parm : type	- [in]  0x00:생성, 0x01:삭제, 0x02:저장
 retn : 폴더 (전체 경로 미포함)
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
 desc : Server에 저장된 레시피 파일 송신
 parm : files	- [in]  현재 송신될 레시피 파일명이 저장되어 있는 문자열 배열 버퍼
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
	u32Recipe	= (UINT32)wcslen(uvGFSP_Info_GetData()->recipe_path->gfss);
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
		LOG_ERROR(ENG_EDIC::en_gfss, L"Failed to create packet for sending recipe file");
	}
	else
	{
		bSended	= SendData(ENG_PCGG::en_pcgg_get_recipe_files_ack, pPktBuff, u32Total, 5000);
	}

	/* 메모리 해제 */
	::Free(pPktBuff);
	lstFileSize.RemoveAll();

	return bSended;
}

/*
 desc : Client 쪽에 주기적으로 Alive Check 명령 전달
 parm : None
 retn : TRUE or FALSE
*/
BOOL CGFSSThread::SetAliveTime()
{
	/* 패킷 송신 */
	return SendData(ENG_PCGG(ENG_PCUC::en_comn_alive_check), NULL, 0);
}
