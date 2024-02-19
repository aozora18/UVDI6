
/*
 desc : EFU 시리얼 통신 스레드
*/

#include "pch.h"
#include "EFUThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : port	- [in]  통신 포트 번호 (1 ~ 255)
		baud	- [in]  통신 포트 속도 (CBR_100 ~ CBR_CBR_256000 中 입력)
						300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 1152000
		buff	- [in]  통신 포트의 송/수신 버퍼 크기
		efu_cnt	- [in]  EFU 설치 개수
		shmem	- [in]  공유 메모리
 retn : None
*/
CEFUThread::CEFUThread(UINT8 port, UINT32 baud, UINT32 buff, UINT8 efu_cnt, LPG_EPAS shmem)
{
	m_u8Port	= port;
	/* 전역 객체 생성 */
	m_pComm		= new CSerialComm(port, baud, buff, ENG_SHOT::en_shot_handshake_off);
	ASSERT(m_pComm);
	m_pstState	= shmem;
	m_u8EFUCount= efu_cnt;

	m_pRecvData			= (PUINT8)::Alloc(MAX_CMD_EFU_RECV);
	ASSERT(m_pRecvData);
#ifdef _DEBUG
	m_bAutoStateGet		= TRUE;		/* 기본적으로, EFU 자동으로 상태 요청 값을 요청하지 않습니다. */
#else
	m_bAutoStateGet		= FALSE;	/* 기본적으로, EFU 자동으로 상태 요청 값을 요청하지 않습니다. */
#endif
	m_bIsPktRecv		= TRUE;
	m_u64AckTimeWait	= 1000;		/* 송신 후 1 초 이내에 응답이 꼭 와야 함 */
	m_u64SendPktTime	= 0;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CEFUThread::~CEFUThread()
{
	if (m_pComm)
	{
		/* 시리얼 통신 포트 해제 */
		CommClosed();
		/* 시리얼 통신 객체 해제 */
		delete m_pComm;
		m_pComm	= NULL;
	}
	if (m_pRecvData)	::Free(m_pRecvData);
	m_pRecvData	= NULL;
}

/*
 desc : 스레드 시작할 때, 최초 한번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CEFUThread::StartWork()
{

	return TRUE;
}

/*
 desc : 스레드 동작 중, 지속적으로 호출됨
 parm : None
 retn : None
*/
VOID CEFUThread::RunWork()
{
	/* 현재 통신 포트가 열려져 있는지 여부를 확인 후 열려져 있지 않으면, 해제 후 다시 연결 시도 */
	if (!m_pComm->IsOpenPort())
	{
		if (!m_pComm->OpenExt(ENG_CRTB::en_crtb_read_timeout_blocking, FALSE))
			CommClosed();
		/* 통신 포트 다시 열기 때문에, 일정 시간 대기 */
		Sleep(500);
	}
	else
	{
		/* 주기적으로 데이터 수신이 있는지 읽기 시도 */
		if (m_syncComm.Enter())
		{
			/* 통신 포트로부터 데이터 읽기 */
			if (m_pComm->ReadData())
			{
				ParseAnalysis();
				m_bIsPktRecv	= TRUE;
			}
			else
			{
				/* 현재 수신 가능한 상태인지 확인 */
				SetRecvEnable();
				/* 이전에 수신된 데이터가 있고, 자동으로 상태 정보 요청하는 모드인지 확인 */
				if (m_bIsPktRecv && m_bAutoStateGet)
					ReqGetState();
			}
			/* 동기 해제 */
			m_syncComm.Leave();
		}
	}
}

/*
 desc : 스레드 중지(종료)될 때, 마지막으로 한 번 호출됨
 parm : None
 retn : None
*/
VOID CEFUThread::EndWork()
{
}

/*
 desc : 스레드 동작을 중지하고자 할 때, 통신 이벤트 대기 상태를 탈출한 후, 중지 작업 진행
 parm : None
 retn : None
*/
VOID CEFUThread::Stop()
{
#if 0
	/* 반드시 통신 이벤트 감시 기능을 해제한 후, 중지 작업 수행 */
	if (m_pComm)	m_pComm->WaitCommEventBreak();
#endif
	/* 스레드 동작을 중지 시킴 */
	CThinThread::Stop();
}

/*
 desc : 현재 시리얼 통신 포트가 열려져 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
BOOL CEFUThread::IsOpenPort()
{
	return m_pComm->IsOpenPort();
}

/*
 desc : 현재 통신 포트의 환경 설정 값 반환
 parm : data	- [out] 반환되어 저장될 구조체 참조
 retn : TRUE or FALSE
*/
BOOL CEFUThread::GetConfigPort(STG_SSCI &data)
{
	return m_pComm->GetConfigPort(data);
}

/*
 desc : 현재 통신 포트의 환결 설정 값 변경
 parm : data	- [in]  변경하고자 하는 값이 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
BOOL CEFUThread::SetConfigPort(LPG_SSCI data)
{
	return m_pComm->SetConfigPort(data);
}

/*
 desc : 주기적으로 수신된 데이터가 있는지 값 요청
 parm : port	- [in]  검색 대상의 통신 포트 번호
		data	- [in]  수신된 데이터가 저장될 버퍼 포인터
		size	- [out] 수신된 데이터가 저장될 'data' 버퍼의 크기
						실제로 수신된 데이터의 크기
 retn : TRUE or FALSE
*/
BOOL CEFUThread::PopCommData(PUINT8 data, UINT32 &size)
{
	if (!m_pComm)	return FALSE;

	/* 현재 스레드가 동작 중인지 / 아닌지 확인 */
	if (!IsBusy())
	{
		m_pComm->PutErrMsg(L"The comm thread is stopped");
		return FALSE;
	}
	/* 수신된 데이터 가져오고, 가져온 크기 저장 */
	size = m_pComm->PopReadData(data, size);

	return TRUE;
}

/*
 desc : 현재 통신 상태 값 반환
 parm : None
 retn : ENG_SCSC
*/
ENG_SCSC CEFUThread::GetCommStatus()
{
	return m_pComm->GetCommStatus();
}

/*
 desc : 현재 통신 연결 해제 처리
 parm : None
 retn : None
*/
VOID CEFUThread::CommClosed()
{
	if (m_pComm)	m_pComm->Close();
	ResetCommData();
}

/*
 desc : 수신된 임시 버퍼의 데이터 분석
 parm : None
 retn : None
*/
VOID CEFUThread::ParseAnalysis()
{
	UINT8 u8Pkt[8]	= {NULL}, u8ChkSum = 0x00, i;
	UINT32 u32Read	= 0, u32Pkts = 7;
	PUINT8 pRecvData= m_pRecvData;

	do {

		/* 현재 버퍼에 저장된 데이터 크기 */
		u32Read	= m_pComm->GetReadSize();
		/* 최소한 6 bytes (stx+mode1+mode2+lv32_id+dpu_id+...+etx) 이상이 수신되었는지 여부 */
		if (u32Read < 0x06)	return;
		/* 수신된 데이터의 버퍼가 너무 크면, 읽어서 버림 */
		if (u32Read >= MAX_CMD_EFU_RECV)
		{
			TCHAR tzMesg[256]	= {NULL};
			swprintf_s(tzMesg, 256, L"RS232_PORT[%d], The receive buffer was too full, "
									L"so all existing received data was read\n", m_u8Port);
			LOG_ERROR(ENG_EDIC::en_efu_ss, tzMesg);
			m_pComm->ReadAllData();
			return;
		}

		/* 최소한 6 bytes (stx+mode1+mode2+lv32_id+dpu_id+...+etx) 이상이 수신되었는지 여부 */
		u32Read	= m_pComm->GetReadSize();
		if (u32Read < 0x06)	return;

		/* 5 bytes 읽기 */
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
		/* 앞으로 더 수신될 데이터가 있는지 여부 */
		if (u32Read < u32Pkts)	return;

		/* 수신된 버퍼에 있는 모든 내용 가져오기 (!!! Pops !!!) */
		memset(m_pRecvData, 0x00, MAX_CMD_EFU_RECV);
		m_pComm->PopReadData(pRecvData, u32Pkts);
		/* Start / End Bit 비교 */
		if (!(EFU_PKT_STX == pRecvData[0] && EFU_PKT_ETX == pRecvData[u32Pkts-1]))
		{
			TCHAR tzMesg[256]	= {NULL};
			swprintf_s(tzMesg, 256, L"RS232_PORT[%d], The first (STX:%02x) and last (ETX:%02x) of the packet are incorrect, "
									L"so all existing received data was read\n", m_u8Port, m_pRecvData[0], m_pRecvData[u32Pkts-1]);
			LOG_ERROR(ENG_EDIC::en_efu_ss, tzMesg);
			m_pComm->ReadAllData();
			return;
		}
		/* 패킷의 Check Sum 비교 */
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
		/* 데이터 부가 있는 위치로 주소 이동 */
		pRecvData	+= 5;	/* DPU_ID 이후... */
		/* In case of response BL500 ID & PV & Alarm & SV */
		if (0x8A == u8Pkt[1] && 0x87 == u8Pkt[2])
		{
			/* 모두 들어 있으면 정상 */
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

		/* 수신받은 패킷 문자열로 임시 저장 (16진수) */
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
 desc : 기존 통신 관련 송/수신 데이터 초기화
 parm : None
 retn : None
*/
VOID CEFUThread::ResetCommData()
{
	/* 동기 진입 */
	if (m_syncComm.Enter())
	{
		/* 통신 포트로부터 데이터 읽기 시도 */
		m_pComm->ReadData();
		/* 기존 수신된 버퍼 모두 읽어서 버리기 */
		m_pComm->ReadAllData();

		/* 동기 해제 */
		m_syncComm.Leave();
	}
}

/*
 desc : 통신 응답 대기 시간 값 설정
 parm : wait	- [in]  통신 (명령) 전송 후 응답이 올 때까지 최대 기다리는 시간 (단위: msec)
 retn : None
*/
VOID CEFUThread::SetAckTimeout(UINT64 wait)
{
	m_u64AckTimeWait = wait;
}
#if 0
/*
 desc : 현재 EFU 쪽에 데이터를 송신해도 되는지 여부
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
 desc : 원격 시스템으로부터 데이터를 수신받을 수 있는 조건 설정 확인
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
 desc : 명령어 송신 (파라미터 포함)
 parm : cmd	- [in]  파라미터가 포함된 문자열 (ASCII Code)
		size- [in]  'cmd' 에 저장된 문자열 (버퍼) 크기
 retn : TRUE or FALSE
*/
BOOL CEFUThread::SendData(PUINT8 cmd, UINT32 size)
{
	BOOL bSended	= FALSE;
	UINT64 u64Tick	= GetTickCount64();

	/* 현재 통신이 연결된 상태인지 여부 */
	if (!m_pComm->IsOpenPort())
	{
		m_pComm->PutErrMsg(L"Serial communication port did not open", 0x00, FALSE);
		return FALSE;
	}
	/* -------------------------------------------------------------- */
	/* 만약 이전에 송신된 데이터가 있는지 그리고 응답이 오지 않았는지 */
	/* -------------------------------------------------------------- */
	if (!m_bIsPktRecv && u64Tick < (m_u64SendPktTime + m_u64AckTimeWait))
	{
		LOG_WARN(ENG_EDIC::en_efu_ss, L"There is no response from EFU for previously sent packets");
		return FALSE;
	}
	/* --------------------------------------------------------------------- */
	/* 가장 최근에 송신된 패킷의 시간 저장 및 아직 수신된 패킷이 없다고 설정 */
	/* --------------------------------------------------------------------- */
	m_u64SendPktTime= GetTickCount64();
	m_bIsPktRecv	= FALSE;

	/* 데이터 송신 */
	bSended	= m_pComm->WriteData(cmd, size);
	if (!bSended)
	{
		SaveError(0x00, cmd);
		/* 만약 송신이 실패하면, 응답 패킷과 패킷 송신 시간 재설정 */
		m_bIsPktRecv	= TRUE;
		m_u64SendPktTime= 0;
	}

	return bSended;
}

/*
 desc : 로그 출력 (Only 시리얼 명령 송/수신 실패)
 parm : type	- [in]  0x00: Send Command, 0x01: Recv Command, 0x02 : Recv Data
		buff	- [in]  송신 혹은 수신 내용 (명령)이 저장된 버퍼 혹은 수신된 메시지 내용
 retn : None
*/
VOID CEFUThread::SaveError(UINT8 type, PUINT8 buff)
{
	TCHAR tzMesg[1024]	= {NULL}, tzType[3][8] = { L"Send", L"Recv", L"Data" };
	swprintf_s(tzMesg, 1024, L"EFU [%s]: mode1=%02x : mode2=%02x", tzType[type], buff[1], buff[2]);
	LOG_ERROR(ENG_EDIC::en_efu_ss, tzMesg);
}

/*
 desc : EFU 설정 (Speed) 요청
 parm : bl_id	- [in]  EFU ID 즉, 0x01 ~ 0x20 (1 ~ 32), 0x00 - ALL
		speed	- [in]  Setting Value 즉, 0 ~ 140 (0 ~ 1400) RPM (1 = 10 RPM 의미)
 retn : TRUE or FALSE
*/
BOOL CEFUThread::ReqSetSpeed(UINT8 bl_id, UINT8 speed)
{
	UINT8 i, u8Pkt[16]	= {NULL};
	BOOL bSended	= FALSE;

	/* 기존 설정 정보 초기화 */
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

	/* 동기 진입 */
	if (m_syncComm.Enter())
	{
		bSended	= SendData(u8Pkt, 10);
		/* 동기 해제 */
		m_syncComm.Leave();
	}

	return bSended;
}

/*
 desc : EFU 상태 요청
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

	/* 동기 진입 */
	if (m_syncComm.Enter())
	{
		bSended	= SendData(u8Pkt, 9);
		/* 동기 해제 */
		m_syncComm.Leave();
	}

	return bSended;
}
