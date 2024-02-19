
/*
 desc : MILARA 시리얼 통신 공통 스레드
*/

#include "pch.h"
#include "MilaraThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : apps	- [in]  Prealigner or Robot
		port	- [in]  통신 포트 번호 (1 ~ 255)
		baud	- [in]  통신 포트 속도 (CBR_100 ~ CBR_CBR_256000 中 입력)
						300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 1152000
		buff	- [in]  통신 포트의 송/수신 버퍼 크기
 retn : None
*/
CMilaraThread::CMilaraThread(ENG_EDIC apps, UINT8 port, UINT32 baud, UINT32 buff)
{
	m_enAppsID		= apps;
	m_u8Port		= port;
	m_u8ReplySkip	= 0x00;

	/* 아래 2개의 변수는 시리얼 통신 포트 열린 후 반드시 초기화할 때 사용됨 */
	m_bIsInited		= FALSE;	/* 이 값이 FALSE 일 때는 어떠한 경우에도 송신이 되지 않음 */
	m_u8InitStep	= 0x00;

	/* 전역 객체 생성 */
	m_pComm			= new CSerialComm(port, baud, buff, ENG_SHOT::en_shot_handshake_off);
	ASSERT(m_pComm);

	/* 내용이 포함된 송/수신 버퍼 메모리 할당 */
	m_pCmdSend		= (PUINT8)::Alloc(MAX_CMD_MILARA_SEND + 1);
	m_pCmdRecv		= (PUINT8)::Alloc(MAX_CMD_MILARA_RECV + 1);
	m_pszLastPkt	= (PCHAR)::Alloc(MAX_CMD_MILARA_RECV * 3 + 1);	/* 2 bytes Hexa-String + 1 bytes space */
	memset(m_pCmdSend, 0x00, MAX_CMD_MILARA_SEND+1);
	memset(m_pCmdRecv, 0x00, MAX_CMD_MILARA_RECV+1);
	memset(m_pszLastPkt, 0x00, MAX_CMD_MILARA_RECV*3+1);
	m_u32RecvSize	= 0;
	m_bIsTechMode	= FALSE;	/* Teching Mode인지 여부 (초기. Teaching Mode 아니라고 설정) */
	m_enInfoMode	= ENG_MRIM::en_rim_2;	/* 비동기 모드로 동작하면서, 응답 값은 무조건 프롬프트만 받기로 함 */

	m_u64AckTimeWait= 1000;	/* 송신 후 1 초 이내에 응답이 꼭 와야 함 */
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CMilaraThread::~CMilaraThread()
{
	if (m_pComm)
	{
		/* 시리얼 통신 포트 해제 */
		CommClosed();
		/* 시리얼 통신 객체 해제 */
		delete m_pComm;
		m_pComm	= NULL;
	}
	/* 송신 버퍼 해제 */
	if (m_pCmdSend)		::Free(m_pCmdSend);
	if (m_pCmdRecv)		::Free(m_pCmdRecv);
	if (m_pszLastPkt)	::Free(m_pszLastPkt);
}

/*
 desc : 스레드 시작할 때, 최초 한번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMilaraThread::StartWork()
{

	return TRUE;
}

/*
 desc : 스레드 동작 중, 지속적으로 호출됨
 parm : None
 retn : None
*/
VOID CMilaraThread::RunWork()
{
	/* 현재 통신 포트가 열려져 있는지 여부를 확인 */
	/* 열려져 있지 않으면, 해제 후 다시 연결 시도 */
	if (!m_pComm->IsOpenPort())
	{
		/* 통신 포트 다시 열기 위해서, 일정 시간 대기 */
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
		/* 통신 포트 다시 열기 때문에, 일정 시간 대기 */
		Sleep(500);
		return;
	}

	/* 주기적으로 데이터 수신이 있는지 읽기 시도 */
	if (m_syncComm.Enter())
	{
		if (!m_bIsInited)	Initialized();
		/* 통신 포트로부터 데이터 읽기 */
		m_pComm->ReadData();
		/* 버퍼에 수신된 데이터가 있다면 */
		if (m_pComm->GetReadSize())
		{
			ParseAnalysis();	/* 임시 버퍼에 있는 데이터 가져와 분석하기 */
		}

		/* 동기 해제 */
		m_syncComm.Leave();
	}
}

/*
 desc : 스레드 중지(종료)될 때, 마지막으로 한 번 호출됨
 parm : None
 retn : None
*/
VOID CMilaraThread::EndWork()
{
}

/*
 desc : 스레드 동작을 중지하고자 할 때,
		통신 이벤트 대기 상태를 탈출하도록 한 후, 중지 작업을 진행해야 한다.
 parm : None
 retn : None
*/
VOID CMilaraThread::Stop()
{
#if 0
	/* 반드시 통신 이벤트 감시 기능을 해제한 후, 중지 작업 수행 */
	if (m_pComm)	m_pComm->WaitCommEventBreak();
#endif
	// 스레드 동작을 중지 시킴
	CThinThread::Stop();
}

/*
 desc : 현재 시리얼 통신 포트가 열려져 있는지 여부 확인
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMilaraThread::IsOpenPort()
{
	if (!m_pComm)	return FALSE;
	return m_pComm->IsOpenPort();
}

/*
 desc : 가장 최근에 발생된 에러 메시지 저장
 parm : msg	- [in]  에러 정보가 저장된 문자열 포인터
 retn : None
*/
VOID CMilaraThread::PutErrMsg(TCHAR *msg)
{
	if (m_pComm)	m_pComm->PutErrMsg(msg);
}

/*
 desc : 현재 통신 포트의 환경 설정 값 반환
 parm : data	- [out] 반환되어 저장될 구조체 참조
 retn : TRUE or FALSE
*/
BOOL CMilaraThread::GetConfigPort(STG_SSCI &data)
{
	if (!m_pComm)	return FALSE;
	return m_pComm->GetConfigPort(data);
}

/*
 desc : 현재 통신 포트의 환결 설정 값 변경
 parm : data	- [in]  변경하고자 하는 값이 저장된 구조체 포인터
 retn : TRUE or FALSE
*/
BOOL CMilaraThread::SetConfigPort(LPG_SSCI data)
{
	if (!m_pComm)	return FALSE;
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
BOOL CMilaraThread::PopReadData(PUINT8 data, UINT32 &size)
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
 desc : 수신된 데이터가 있는지 값 요청 (수신 데이터를 복사만 해서 가져감)
 parm : port	- [in]  검색 대상의 통신 포트 번호
		data	- [in]  수신된 데이터가 저장될 버퍼 포인터
		size	- [out] 수신된 데이터가 저장될 'data' 버퍼의 크기
						실제로 수신된 데이터의 크기
 retn : TRUE or FALSE
*/
BOOL CMilaraThread::CopyReadData(PUINT8 data, UINT32 &size)
{
	if (!m_pComm)	return FALSE;
	/* 현재 스레드가 동작 중인지 / 아닌지 확인 */
	if (!IsBusy())
	{
		m_pComm->PutErrMsg(L"The comm thread is stopped");
		return FALSE;
	}
	/* 수신된 데이터 가져오고, 가져온 크기 저장 */
	size	= m_pComm->CopyReadData(data, size);

	return TRUE;
}

/*
 desc : 현재 통신 상태 값 반환
 parm : None
 retn : ENG_SCSC
*/
ENG_SCSC CMilaraThread::GetCommStatus()
{
	return m_pComm->GetCommStatus();
}

/*
 desc : 현재 통신 연결 해제 처리
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
 desc : 수신된 임시 버퍼의 데이터 분석
 parm : None
 retn : None
*/
VOID CMilaraThread::ParseAnalysis()
{
	BOOL bIsFindA		= FALSE, bIsFindN = FALSE;
	UINT32 u32FindA		= 0 /* Ack */, u32FindN = 0 /* Not Ack */;
	UINT32 u32Read		= 0;

	do {

		/* 현재 버퍼에 저장된 데이터 크기 */
		u32Read	= m_pComm->GetReadSize();
		/* 수신된 데이터의 버퍼가 너무 크면, 읽어서 버림 */
		if (u32Read >= MAX_CMD_MILARA_RECV)
		{
			TCHAR tzMesg[128]	= {NULL};
			swprintf_s(tzMesg, 128, L"RS232_PORT[%d], The receive buffer was too full, "
									L"so all existing received data was read\n", m_u8Port);
			LOG_ERROR(ENG_EDIC::en_milara, tzMesg);
			m_pComm->ReadAllData();
			return;
		}
		/* 최소한 3 bytes ('>' or '?' + 'cr' + 'lf') 이상이 수신되었는지 여부 */
		if (u32Read < UINT32(m_u8ReplySkip+0x03))	return;

		/* 수신된 문자열 중에서 '>' 혹은 '?'가 있는지 확인 */
		bIsFindA = m_pComm->FindChar(UINT8('\n'), UINT8('>'), u32FindA);
		bIsFindN = m_pComm->FindChar(UINT8('\n'), UINT8('?'), u32FindN);

		/* 모두 검색 실패인 경우 */
		if (!u32FindA && !bIsFindN)	return;
		/* '>' or '?' 앞에 line feed <lf>가 존재하는지 확인 */

		/* 2 개의 문자 검색 대상 중에서 1 개만 존재하는 경우 */
		if (!(u32FindA && bIsFindN))
		{
			u32Read	= u32FindA > u32FindN ? u32FindA : u32FindN;
		}
		/* 2 개의 문자 검색 대상 중에서 2 개 모두 존재하는 경우 */
		else
		{
			u32Read	= u32FindA > u32FindN ? u32FindN : u32FindA;
		}
		/* !!! 마지막 검색 위치는 Zero-based 이므로 !!! */
		u32Read++;

		/* !!! 반드시, 기존 버퍼 초기화 !!! */
		memset(m_pCmdRecv, 0x00, MAX_CMD_MILARA_RECV);
		/* 수신된 버퍼로부터 데이터 (패킷) 읽기 ('\r\n' 2개 문자 포함해서 읽어들이기) */
		if (!m_pComm->PopReadData((PUINT8)m_pCmdRecv, u32Read))
		{
			TCHAR tzMesg[128]	= {NULL};
			swprintf_s(tzMesg, 128,
					   L"RS232_PORT[%d], Failed to read data from communication buffer\n", m_u8Port);
			LOG_ERROR(ENG_EDIC::en_milara, tzMesg);
			return;
		}

		/* !!! 가장 최근에 수신된 명령어 ('cr'와 '>' or '?' 포함) 크기 임시 저장 !!! */
		m_u32RecvSize	= u32Read;

		/* 가장 최근에 수신한 명령어의 응답 상태 값 갱신 */
		if (m_pCmdRecv[u32Read-0x01] == 0x3e /* '>' */)	m_enLastSendAck = ENG_MSAS::succ;
		else											m_enLastSendAck	= ENG_MSAS::fail;

		/* 수신된 데이터의 크기가 3 bytes 이하이면, '>\r\n' or '?\r\n' 이면, 데이터 수신 처리 하지 않음 */
		if (m_u32RecvSize > UINT32(m_u8ReplySkip+0x03) &&
			m_enLastSendAck == ENG_MSAS::succ)
		{
			RecvData();
		}

		/* 암튼 수신된 데이터가 있다. (성공이든 실패이든) */
		m_bIsRecv	= TRUE;

	} while (1);
}

/*
 desc : 기존 통신 관련 송/수신 데이터 초기화
 parm : None
 retn : None
*/
VOID CMilaraThread::ResetCommData()
{
	/* 동기 진입 */
	if (m_syncComm.Enter())
	{
		/* 가장 최근 송신 명령어 (파라미터 제외) 초기화 */
		m_enLastSendAck	= ENG_MSAS::none;
		/* 통신 포트로부터 데이터 읽기 시도 */
		m_pComm->ReadData();
		/* 기존 수신된 버퍼 모두 읽어서 버리기 */
		m_pComm->ReadAllData();

		/* 동기 해제 */
		m_syncComm.Leave();
	}
}

/*
 desc : 명령어 송신 (파라미터 포함)
 parm : cmd	- [in]  파라미터가 포함된 문자열 (ASCII Code)
		size- [in]  'cmd' 에 저장된 문자열 (버퍼) 크기
 retn : TRUE or FALSE
*/
BOOL CMilaraThread::SendData(PCHAR cmd, UINT32 size)
{
	BOOL bSended	= FALSE;
	UINT32 u32Size	= size + 1 /* because of carriage return */;
	PUINT8 pCmdSend	= m_pCmdSend;

	/* 현재 송신 명령에 대한 응답 상태가 정해지지 않았다라고 설정 */
	m_enLastSendAck	= ENG_MSAS::none;
	/* 현재 통신이 연결된 상태인지 여부 */
	if (!m_pComm->IsOpenPort())
	{
		m_pComm->PutErrMsg(L"Serial communication port did not open", 0x00, FALSE);
		return FALSE;
	}

	/* 송신 데이터의 버퍼가 비어 있는 경우인지 */
	if (strlen(cmd) < 1 || size < 1 || MAX_CMD_MILARA_SEND < u32Size)
	{
		LOG_WARN(ENG_EDIC::en_milara, L"Send command length is too short or too large");
		return FALSE;
	}

	/* 송신 버퍼 초기화 */
	memset(m_pCmdSend, 0x00, MAX_CMD_MILARA_SEND);

	/* 동기 진입 */
	if (m_syncComm.Enter())
	{
		/* 송신 버퍼에 복사 */
		memcpy(pCmdSend, cmd, size);	/* 명령 본문 */
		pCmdSend	+= size;
		*pCmdSend	= 0x0d;	/* Carriage Return */

		/* 가장 최근 수신된 데이터 초기화 */
		memset(m_pCmdRecv, 0x00, MAX_CMD_MILARA_RECV);
		m_u32RecvSize	= 0;

		/* 데이터 송신 */
		bSended	= m_pComm->WriteData(m_pCmdSend, u32Size);

		/* 동기 해제 */
		m_syncComm.Leave();
	}

	/* 송신된 패킷 문자열로 임시 저장 (16진수) */
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
 desc : 통신 응답 대기 시간 값 설정
 parm : wait	- [in]  통신 (명령) 전송 후 응답이 올 때까지 최대 기다리는 시간 (단위: msec)
 retn : None
*/
VOID CMilaraThread::SetAckTimeout(UINT64 wait)
{
	m_u64AckTimeWait = wait;
}

/*
 desc : 통신 (명령) 송신 후 응답이 올 때까지 일정 시간 대기 하도록 하기 위함
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
		/* 장비 (Prealigner)로부터 응답이 올 때까지 최소한 대기를 했는지 여부 */
		if (GetTickCount64() > u64Wait)	break;
#endif
		uvCmn_uSleep(10);	/* 잠시 쉬고 */

	} while (1);
}

/*
 desc : 로그 출력 (Only 시리얼 명령 송/수신 실패)
 parm : type	- [in]  0x00: Send Command, 0x01: Recv Command, 0x02 : Recv Data
		buff	- [in]  송신 혹은 수신 내용 (명령)이 저장된 버퍼 혹은 수신된 메시지 내용
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
 desc : 명령 송신 후 응답 받기
 parm : cmd	- [in]  Send 명령어가 저장된 버퍼
 retn : Null or 수신된 데이터가 저장된 전역 버퍼 포인터
*/
BOOL CMilaraThread::SendData(PCHAR cmd)
{
	/* 수신된 데이터가 없다고 플래그 설정 */
	m_bIsRecv	= FALSE;

	/* 명령 전달 */
	if (!SendData(cmd, (UINT32)strlen(cmd)))
	{
		SaveErrorToSerial(0x00, cmd);
		m_bIsSend	= FALSE;	/* 데이터 송신 실패 */
		return FALSE;
	}
	/* 데이터 송신 성공 처리 */
	m_bIsSend	= TRUE;

	return TRUE;
}

/*
 desc : 콤마 (Comma)로 구성된 문자열 (통신 수신 데이터) 분석 (문자열 버퍼에 등록)
 parm : vals	- [out] 콤마로 분리된 각각의 값들이 문자열로 저장된 벡터 값
 retn : None
*/
VOID CMilaraThread::GetValues(vector <string> &vals)
{
	UINT32 u32Find	= 0, i = 0, u32Recv = m_u32RecvSize - 0x03 /* = '>' or '?' + '/r' + '/n' */;
	PCHAR pszVal	= NULL, pszRecv = (PCHAR)m_pCmdRecv + m_u8ReplySkip /* Skip : > or ? */;

	/* 메모리 할당 */
	pszVal	= (PCHAR)::Alloc(MAX_CMD_MILARA_RECV);
	ASSERT(pszVal);

	if (u32Recv > 0)
	{
		/* 콤마 구분자 앞까지 값 분리 후 저장 */
		for (; i<u32Recv; i++)
		{
			if (pszRecv[i] == ',')
			{
				/* 검색된 값 복사 (등록) */
				memset(pszVal, 0x00, MAX_CMD_MILARA_RECV);
				memcpy(pszVal, pszRecv+u32Find, i-u32Find);
				vals.push_back(pszVal);

				/* 가장 최근에 검색한 위치 값 임시 저장 */
				u32Find = i + 1;	/* 콤마 이후 문자로 위치 재설정 */
			}
		}

		/* 맨 마지막 값 복사 */
		memset(pszVal, 0x00, MAX_CMD_MILARA_RECV);
		memcpy(pszVal, pszRecv+u32Find, i-u32Find);
		vals.push_back(pszVal);
#if 0
		/* 로그 문자열로 출력 */
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

	/* 메모리 해제 */
	::Free(pszVal);
}

/*
 desc : 콤마 (Comma)가 포함된 숫자 문자열에서 콤마 제거
 parm : None
 retn : 숫자값 반환
*/
UINT32 CMilaraThread::RleaseCommaToNum()
{
	string strBuff((PCHAR)m_pCmdRecv);

	/* 문자열 중에 Comma (',') 문자 제거 : remove(strBuff.begin(), strBuff.end(), ',')은 최종 지원진 마지막 원소 반환 */
	if (strBuff.find(',') != string::npos)
	{
		strBuff.erase(remove(strBuff.begin(), strBuff.end(), ','), strBuff.end());
	}

	return (UINT32)atol(strBuff.c_str());
}

/*
 desc : 각 축(R, Z, T) 중 입력된 값을 프로그램에 설정되는 값으로 변환
 parm : p_type	- [in]  Point (소숫점) 자리 개수 (00, 000, 000, ...)
		value	- [in]  축이 R와 Z인 경우, inch
						축이 T인 경우, degree
 retn : 값 반환 (정수형 inch or degree --> 실수형 um or degree)
 */
DOUBLE CMilaraThread::GetInchToPos(ENG_RITU p_type, INT32 value)
{
	DOUBLE dbValue		= 0.0f;
	DOUBLE dbInchToUm	= GetInchToUm();

	/* 입력되는 값에 따라 단위 변환이 다름 */
	switch (p_type)
	{
	case ENG_RITU::itu_00	: dbValue = (value / 100.0f) * dbInchToUm;		break;	/* 1 --> 0.01 inch, 0.01 inc = 254.0 um */
	case ENG_RITU::itu_000	: dbValue = (value / 1000.0f) * dbInchToUm;		break;	/* 1 --> 0.01 inch, 0.01 inc = 254.0 um */
	case ENG_RITU::itu_0000	: dbValue = (value / 10000.0f) * dbInchToUm;	break;	/* 1 --> 0.0001 inch = 2.54 um */
	}

	return dbValue;
}

/*
 desc : 소숫점 입력 개수에 따라 장비에 설정되는 값으로 변환
 parm : p_type	- [in]  Point (소숫점) 자리 개수 (00, 000, 000, ...)
		value	- [in]  User Input Value (unit : um)
 retn : 값 반환 (실수형 um or degree --> 정수형 inch or degree)
 */
INT32 CMilaraThread::GetPosToInch(ENG_RUTI p_type, DOUBLE value)
{
	DOUBLE dbValue		= 0.0f;
	DOUBLE dbInchToUm	= GetInchToUm()/*2.54 / * 1 inch -> 2.54 cm * / * 10000 / * cm -> um * /*/;

	/* 입력되는 값에 따라 단위 변환이 다름 */
	switch (p_type)
	{
	case ENG_RUTI::uti_00	:	dbValue	= (value / dbInchToUm) /* inch */ * 100.0f   /* 1 --> 0.01 inch */;		break;
	case ENG_RUTI::uti_000	:	dbValue	= (value / dbInchToUm) /* inch */ * 1000.0f  /* 1 --> 0.01 inch */;		break;
	case ENG_RUTI::uti_0000	:	dbValue	= (value / dbInchToUm) /* inch */ * 10000.0f /* 1 --> 0.0001 inch */;	break;
	}

	return (INT32)ROUNDED(dbValue, 0);
}

/*
 desc : 각 축(R, Z, T) 중 입력된 값을 프로그램에 설정되는 값으로 변환
 parm : axis	- [in]  Axis Name (R : Horizontal, Z : Vertical, T : Angle)
		value	- [in]  축이 R와 Z인 경우, inch
						축이 T인 경우, degree
 retn : 값 반환 (정수형 inch or degree --> 실수형 um or degree)
 */
DOUBLE CMilaraThread::GetAxisPosApp(ENG_RANC axis, INT32 value)
{
	DOUBLE dbValue		= 0.0f;
	DOUBLE dbInchToUm	= GetInchToUm();

	/* 입력되는 값에 따라 단위 변환이 다름 */
	switch (axis)
	{
	case ENG_RANC::rac_t	: dbValue = value * 0.01;						break;	/* 1 --> 0.01° */
	case ENG_RANC::rac_r1	: 
	case ENG_RANC::rac_r2	: 
	case ENG_RANC::rac_z	: dbValue = (value / 1000.0f) * dbInchToUm;		break;	/* 1 --> 0.001 inch, 0.001 inc = 25.4 um */
	}

	return dbValue;
}

/*
 desc : 복수의 값 변환 후 저장
 parm : host_val- [in]  저장될 Host의 Double/UINT32/UINT16/UINT8/INT8 배열 버퍼 포인터
		vals	- [in]  수신된 펌웨어의 값이 저장된 문자열 벡터 포인터
 retn : None
*/
VOID CMilaraThread::SetValues(DOUBLE *host_val, vector <string> &vals)
{
	UINT32 i = 0, u32Count = (UINT32)vals.size();
	if (u32Count > 4)	u32Count	= 4;	/* 일단, K 축 값은 필요 없고, 문자열로 반환되기 때문에 ... */
	for (; i<u32Count; i++)	host_val[i] = atof(vals[i].c_str());
}
VOID CMilaraThread::SetValues(UINT32 *host_val, vector <string> &vals)
{
	UINT32 i = 0, u32Count = (UINT32)vals.size();
	if (u32Count > 4)	u32Count	= 4;	/* 일단, K 축 값은 필요 없고, 문자열로 반환되기 때문에 ... */
	for (; i<u32Count; i++)	host_val[i] = (UINT32)atoi(vals[i].c_str());
}
VOID CMilaraThread::SetValues(INT32 *host_val, vector <string> &vals)
{
	UINT32 i = 0, u32Count = (UINT32)vals.size();
	if (u32Count > 4)	u32Count	= 4;	/* 일단, K 축 값은 필요 없고, 문자열로 반환되기 때문에 ... */
	for (; i<u32Count; i++)	host_val[i] = atoi(vals[i].c_str());
}
VOID CMilaraThread::SetValues(UINT16 *host_val, vector <string> &vals)
{
	UINT32 i = 0, u32Count = (UINT32)vals.size();
	if (u32Count > 4)	u32Count	= 4;	/* 일단, K 축 값은 필요 없고, 문자열로 반환되기 때문에 ... */
	for (; i<u32Count; i++)	host_val[i] = (UINT16)atoi(vals[i].c_str());
}
VOID CMilaraThread::SetValues(UINT8 *host_val, vector <string> &vals)
{
	UINT32 i = 0, u32Count = (UINT32)vals.size();
	if (u32Count > 4)	u32Count	= 4;	/* 일단, K 축 값은 필요 없고, 문자열로 반환되기 때문에 ... */
	for (; i<u32Count; i++)	host_val[i] = (UINT8)atoi(vals[i].c_str());
}
VOID CMilaraThread::SetValues(INT8 *host_val, vector <string> &vals)
{
	UINT32 i = 0, u32Count = (UINT32)vals.size();
	if (u32Count > 4)	u32Count	= 4;	/* 일단, K 축 값은 필요 없고, 문자열로 반환되기 때문에 ... */
	for (; i<u32Count; i++)	host_val[i] = (INT8)atoi(vals[i].c_str());
}
VOID CMilaraThread::SetValues(INT16 *host_val, vector <string> &vals)
{
	UINT32 i = 0, u32Count = (UINT32)vals.size();
	if (u32Count > 4)	u32Count	= 4;	/* 일단, K 축 값은 필요 없고, 문자열로 반환되기 때문에 ... */
	for (; i<u32Count; i++)	host_val[i] = (INT16)atoi(vals[i].c_str());
}

/*
 desc : Inch 값 변환 후 저장
 parm : host_val- [in]  저장될 Host의 Double/UINT32/UINT16/UINT8/INT8 배열 버퍼 포인터
		vals	- [in]  수신된 펌웨어의 값이 저장된 문자열 벡터 포인터
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
	if (u32Count > 4)	u32Count	= 4;	/* 일단, K 축 값은 필요 없고, 문자열로 반환되기 때문에 ... */
	for (; i<u32Count; i++)	host_val[i] = GetInchToPos(ENG_RITU::itu_0000, atoi(vals[i].c_str()));
}

/*
 desc : 단일의 값 변환 후 저장
 parm : host_val- [in]  저장될 Host의 Double/UINT32/UINT16/UINT8/INT8 배열 버퍼 포인터
		vals	- [in]  수신된 펌웨어의 값이 저장된 문자열 벡터 포인터
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
