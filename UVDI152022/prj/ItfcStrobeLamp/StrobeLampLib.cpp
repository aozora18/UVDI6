
/*
 desc : TCP/IP Client Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "StrobeLampLib.h"
#include <condition_variable>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

std::mutex CStrobeLampLib::lockMutex;


CStrobeLampLib::CStrobeLampLib()
{
	for (int i = 0; i < _countof(m_evt_comm_wait); i++)
	{
		m_evt_comm_wait[i] = NULL;
	}

	m_u64RecvTime = GetTickCount64();
	m_u64SendTime = GetTickCount64();

// 	m_vReplyMsg.push_back(eSTROBE_LAMP_CHANNEL_DELAY_CONTROL);
// 	m_vReplyMsg.push_back(eSTROBE_LAMP_CHANNEL_STROBE_CONTROL);
// 	m_vReplyMsg.push_back(eSTROBE_LAMP_CHANNEL_WRITE);
// 	m_vReplyMsg.push_back(eSTROBE_LAMP_PAGE_DATA_WRITE);
// 	m_vReplyMsg.push_back(eSTROBE_LAMP_PAGE_DATA_READ);
// 	m_vReplyMsg.push_back(eSTROBE_LAMP_PAGE_LOOP_DATA_WRITE);
// 	m_vReplyMsg.push_back(eSTROBE_LAMP_PAGE_LOOP_DATA_READ);
// 	m_vReplyMsg.push_back(eSTROBE_LAMP_STROBE_MODE);
// 	m_vReplyMsg.push_back(eSTROBE_LAMP_TRIGGER_MODE);
 	m_vReplyMsg.push_back(eSTROBE_LAMP_DATA_SAVE);
 	m_vReplyMsg.push_back(eSTROBE_LAMP_DATA_LOAD);
// 	m_vReplyMsg.push_back(eSTROBE_LAMP_STROBE_TRIGGER_MODE_READ);

	for (int i = 0; i < (int)m_vReplyMsg.size(); i++)
	{
		m_evt_comm_wait[i] = new CMclEvent(TRUE);
	}

	m_bRun = FALSE;
	m_apNetMonitorThread = NULL;
	m_uiDataPos = 0;
	m_uiDataLength = 0;
	
	m_bAutoReconnection = TRUE;
	m_vRecvData.reserve(DEF_PACKET_MAX_SIZE);
	m_SocketClient.SetInterface(this);

	m_nUniqueID = 1;

	m_bProve = TRUE;
}

/*
 desc : Destructor
 parm : None
*/
CStrobeLampLib::~CStrobeLampLib()
{
	/* StrobeLamp Server와 연결 강제 해제 */
	m_bRun = FALSE;
	m_apNetMonitorThread->Terminate(0L);

	Sleep(500);

	m_SocketClient->Close();
	m_SocketClient.Terminate();
	
	m_vRecvData.clear();
	m_vReplyMsg.clear();
	m_lResultStruct.clear();
	while (!m_qRecvStruct.empty())
	{
		m_qRecvStruct.pop();
	}
	
}

BOOL CStrobeLampLib::Init(HWND hwnd, LPG_CTCS pComm, LPG_CCGS pComn)
{
	m_bAutoReconnection = FALSE;
	m_SocketClient->Close();

	m_hMainWnd = hwnd;
	m_pComm = pComm;
	m_pComn = pComn;

	/* IP 주소 변환: DWORD -> String 변환 */
	TCHAR	tzSourceIPv4[IPv4_LENGTH];
	TCHAR	tzTargetIPv4[IPv4_LENGTH];
	IN_ADDR stInAddr = { NULL };

	wmemset(tzSourceIPv4, 0x00, IPv4_LENGTH);
	memcpy(&stInAddr.s_addr, &m_pComm->source_ipv4, sizeof(UINT32));
	InetNtopW(AF_INET, (CONST VOID*) & stInAddr, tzSourceIPv4, IPv4_LENGTH);

	wmemset(tzTargetIPv4, 0x00, IPv4_LENGTH);
	memcpy(&stInAddr.s_addr, &m_pComm->target_ipv4, sizeof(UINT32));
	InetNtopW(AF_INET, (CONST VOID*) & stInAddr, tzTargetIPv4, IPv4_LENGTH);

	m_str_local_ip = tzSourceIPv4;
	m_str_svr_ip = tzTargetIPv4;
	m_nPortNum = m_pComm->tcp_port;

	if (FALSE == m_bRun)
	{
		m_bRun = TRUE;
		m_apNetMonitorThread = new CMclThread(this);
	}

	m_bAutoReconnection = TRUE;
	return TRUE;
}


unsigned CStrobeLampLib::ThreadHandlerProc()
{
	Sleep(1000);

	TCHAR szPort[16] = { 0, };

	while (INFINITE)
	{
 		if (!m_bRun) break;

		if (!m_bAutoReconnection)
		{
			Sleep(500);
			continue;
		}

		if (IsConnect())
		{
			PopPktData();
			Sleep(5);
			continue;
		}

		swprintf_s(szPort, _countof(szPort), _T("%d"), m_nPortNum);
		if (m_SocketClient.StartClient(m_str_local_ip, m_str_svr_ip, szPort, AF_INET, SOCK_STREAM) == true)
		{

			continue;
		}

		if (!m_bRun) break;
		Sleep(1000);
		if (!m_bRun) break;
		Sleep(1000);
		if (!m_bRun) break;
		Sleep(1000);
		if (!m_bRun) break;

	} // Main Thread Main Loop End

	return NO_ERROR;
}

int CStrobeLampLib::GetUniqueID()
{
	if (m_nUniqueID == 99999999)
		m_nUniqueID = 1;

	return m_nUniqueID++;
}




void CStrobeLampLib::OnDataReceived(CSocketHandle* pSH, const BYTE* pbData, DWORD dwCount, const SockAddrIn& addr)
{
	STROBELOCK;
	int cmd = pbData[1];
	auto bodySize = sizeof(Header) + STG_SLPR_RECV::GetBodyResponseSize(cmd) + sizeof(Tail);
	for (DWORD i = 0; i < bodySize; ++i)
	{
		m_vRecvData.push_back(pbData[i]);

		if (m_vRecvData.size() >= sizeof(Tail))
		{
			size_t checksum_pos = m_vRecvData.size() - sizeof(Tail);

			if (FIX_DATA_TAIL_1 == m_vRecvData[checksum_pos + 1] &&
				FIX_DATA_TAIL_2 == m_vRecvData[checksum_pos + 2])
			{
				STG_SLPR_RECV stRecv;
				memcpy(&stRecv, m_vRecvData.data(), m_vRecvData.size());
				stRecv.tail.checksum = m_vRecvData[checksum_pos];
				AddPktData(stRecv);
				m_vRecvData.clear();
			}
		}
	}
}

void CStrobeLampLib::OnThreadBegin(CSocketHandle* pSH)
{

}

void CStrobeLampLib::OnThreadExit(CSocketHandle* pSH)
{

}

void CStrobeLampLib::OnConnectionDropped(CSocketHandle* pSH)
{
	ASSERT(pSH == m_SocketClient);
	(pSH);

	m_SocketClient.Close();
	m_bAutoReconnection = TRUE;
}

void CStrobeLampLib::OnConnectionError(CSocketHandle* pSH, DWORD dwError)
{
	ASSERT(pSH == m_SocketClient);
	(pSH);

	m_SocketClient.Close();
	m_bAutoReconnection = TRUE;

}

BOOL CStrobeLampLib::Open(LPCTSTR Ip, int nPort)
{
	m_syncSend.Enter();

	m_str_local_ip.Empty();
	m_str_svr_ip = Ip;
	m_nPortNum = nPort;

	TCHAR szPort[16] = { 0, };
	swprintf_s(szPort, _countof(szPort), _T("%d"), nPort);
	BOOL bRet = FALSE;

	bRet = m_SocketClient.StartClient(NULL, Ip, szPort, AF_INET, SOCK_STREAM);

	if (bRet)
	{
		m_u64SendTime = GetTickCount64();
		m_u64RecvTime = GetTickCount64();
	}

	m_syncSend.Leave();
	return bRet;
}

BOOL CStrobeLampLib::Open(LPCTSTR LocalIp, LPCTSTR Ip, int nPort)
{
	m_syncSend.Enter();

	m_str_local_ip = LocalIp;
	m_str_svr_ip = Ip;
	m_nPortNum = nPort;

	TCHAR szPort[16] = { 0, };
	swprintf_s(szPort, _countof(szPort), _T("%d"), nPort);
	BOOL bRet = FALSE;

	bRet = m_SocketClient.StartClient(LocalIp, Ip, szPort, AF_INET, SOCK_STREAM);

	if (bRet)
	{
		m_u64SendTime = GetTickCount64();
		m_u64RecvTime = GetTickCount64();
	}

	m_syncSend.Leave();
	return bRet;
}

BOOL CStrobeLampLib::Open()
{
	m_syncSend.Enter();


	TCHAR szPort[16] = { 0, };
	swprintf_s(szPort, _countof(szPort), _T("%d"), m_nPortNum);
	BOOL bRet = FALSE;

	bRet = m_SocketClient.StartClient(m_str_local_ip, m_str_svr_ip, szPort, AF_INET, SOCK_STREAM);

	if (bRet)
	{
		m_u64SendTime = GetTickCount64();
		m_u64RecvTime = GetTickCount64();
	}

	m_syncSend.Leave();
	return bRet;
}

BOOL CStrobeLampLib::IsConnect()
{
	return (m_SocketClient.IsOpen() ? TRUE : FALSE);
}

BOOL CStrobeLampLib::Close()
{
	m_bAutoReconnection = TRUE;
	m_SocketClient.Close();

	return true;
}

int CStrobeLampLib::Send_ChannelDelayControl(const uint8_t page, const uint8_t channel, const uint16_t  delayValue, int nTimeout /*= 10000*/)
{
	if (IsConnect() == FALSE)
	{
		return eSTROBE_LAMP_ERR_DISCONNECT;
	}

	/* 데이터 범위 인터락 */
	ERR_CODE_STROBE eEcode = eSTROBE_LAMP_ERR_OK;
	if ((eSTROBE_LAMP_ERR_OK != (eEcode = IsValidPage(page))) ||
		(eSTROBE_LAMP_ERR_OK != (eEcode = IsValidChannel(channel))) ||
		(eSTROBE_LAMP_ERR_OK != (eEcode = IsValidDelayValue(delayValue))))
	{
		return eEcode;
	}

	/* 메세지 선언 */
	stStrobeLampMessage stMsg(eSTROBE_LAMP_CHANNEL_DELAY_CONTROL);

	/* 메세지 바디 입력 */
	uint8_t delayHighValue = (delayValue >> 8) & 0xFF;	// High byte
	uint8_t delayLowValue  = delayValue & 0xFF;         // Low byte

	stMsg.body.channelDelayControl.page = page;
	stMsg.body.channelDelayControl.channel = channel;
	stMsg.body.channelDelayControl.delayHighValue = delayHighValue;
	stMsg.body.channelDelayControl.delayLowValue = delayLowValue;

	/* 메세지 체크섬 얻기 */
	finalizeMessage(stMsg);

	/* 메세지 전송 */
	if (SendData(stMsg, (int)stMsg.GetSize(), nTimeout))
	{
		return eSTROBE_LAMP_ERR_OK;
	}

	return eSTROBE_LAMP_ERR_PKT_BUFFER_FULL;
}

int CStrobeLampLib::Send_ChannelStrobeControl(const uint8_t page, const uint8_t channel, const uint16_t strobeValue, int nTimeout /*= 10000*/)
{
	if (IsConnect() == FALSE)
	{
		return eSTROBE_LAMP_ERR_DISCONNECT;
	}

	/* 데이터 범위 인터락 */
	ERR_CODE_STROBE eEcode = eSTROBE_LAMP_ERR_OK;
	if ((eSTROBE_LAMP_ERR_OK != (eEcode = IsValidPage(page))) ||
		(eSTROBE_LAMP_ERR_OK != (eEcode = IsValidChannel(channel))) ||
		(eSTROBE_LAMP_ERR_OK != (eEcode = IsValidStrobeValue(strobeValue))))
	{
		return eEcode;
	}

	/* 메세지 선언 */
	stStrobeLampMessage stMsg(eSTROBE_LAMP_CHANNEL_STROBE_CONTROL);

	/* 메세지 바디 입력 */
	uint8_t strobeHighValue = (strobeValue >> 8) & 0xFF;  // High byte
	uint8_t strobeLowValue = strobeValue & 0xFF;          // Low byte

	stMsg.body.channelStrobeControl.page = page;
	stMsg.body.channelStrobeControl.channel = channel;
	stMsg.body.channelStrobeControl.strobeHighValue = strobeHighValue;
	stMsg.body.channelStrobeControl.strobeLowValue = strobeLowValue;

	/* 메세지 체크섬 얻기 */
	finalizeMessage(stMsg);

	/* 메세지 전송 */
	if (SendData(stMsg, (int)stMsg.GetSize(), nTimeout))
	{
		return eSTROBE_LAMP_ERR_OK;
	}

	return eSTROBE_LAMP_ERR_PKT_BUFFER_FULL;
}

int CStrobeLampLib::Send_ChannelWrite(const uint8_t page, const uint8_t channel, const uint16_t delayValue, const uint16_t strobeValue, int nTimeout /*= 10000*/)
{
	if (IsConnect() == FALSE)
	{
		return eSTROBE_LAMP_ERR_DISCONNECT;
	}

	/* 데이터 범위 인터락 */
	ERR_CODE_STROBE eEcode = eSTROBE_LAMP_ERR_OK;
	if ((eSTROBE_LAMP_ERR_OK != (eEcode = IsValidPage(page))) ||
		(eSTROBE_LAMP_ERR_OK != (eEcode = IsValidChannel(channel))) ||
		(eSTROBE_LAMP_ERR_OK != (eEcode = IsValidDelayValue(delayValue))) ||
		(eSTROBE_LAMP_ERR_OK != (eEcode = IsValidStrobeValue(strobeValue))))
	{
		return eEcode;
	}

	/* 메세지 선언 */
	stStrobeLampMessage stMsg(eSTROBE_LAMP_CHANNEL_WRITE);

	/* 메세지 바디 입력 */
	uint8_t delayHighValue = (delayValue >> 8) & 0xFF;		// High byte
	uint8_t delayLowValue = delayValue & 0xFF;				// Low byte
	uint8_t strobeHighValue = (strobeValue >> 8) & 0xFF;	// High byte
	uint8_t strobeLowValue = strobeValue & 0xFF;			// Low byte

	stMsg.body.channelWrite.page = page;
	stMsg.body.channelWrite.channel = channel;
	stMsg.body.channelWrite.delayHighValue = delayHighValue;
	stMsg.body.channelWrite.delayLowValue = delayLowValue;
	stMsg.body.channelWrite.strobeHighValue = strobeHighValue;
	stMsg.body.channelWrite.strobeLowValue = strobeLowValue;

	/* 메세지 체크섬 얻기 */
	finalizeMessage(stMsg);

	/* 메세지 전송 */
	if (SendData(stMsg, (int)stMsg.GetSize(), nTimeout))
	{
		return eSTROBE_LAMP_ERR_OK;
	}

	return eSTROBE_LAMP_ERR_PKT_BUFFER_FULL;
}

int CStrobeLampLib::Send_PageDataWrite(const uint8_t page, const uint8_t channelCount, const uint16_t* delayValues, const uint16_t* strobeValues, int nTimeout /*= 10000*/)
{
	if (IsConnect() == FALSE)
	{
		return eSTROBE_LAMP_ERR_DISCONNECT;
	}

	/* 데이터 범위 인터락 */
	ERR_CODE_STROBE eEcode = eSTROBE_LAMP_ERR_OK;
	if (eSTROBE_LAMP_ERR_OK != (eEcode = IsValidPage(page)))
	{
		return eEcode;
	}
	if (channelCount > DEF_MAX_ARRAY_SIZE)
	{
		return eSTROBE_LAMP_ERR_INVALID_SIZE;
	}
	for (int i = 0; i < channelCount; ++i)
	{
		if (eSTROBE_LAMP_ERR_OK != (eEcode = IsValidDelayValue(delayValues[i])))
		{
			return eEcode;
		}

		if (eSTROBE_LAMP_ERR_OK != (eEcode = IsValidStrobeValue(strobeValues[i])))
		{
			return eEcode;
		}
	}

	/* 메세지 선언 */
	stStrobeLampMessage stMsg(eSTROBE_LAMP_PAGE_DATA_WRITE);

	/* 메세지 바디 입력 */
	stMsg.body.pageDataWrite.page = page;
	for (int ch = 0; ch < channelCount; ++ch)
	{
		uint8_t delayHighValue  = (delayValues[ch] >> 8) & 0xFF;	// High byte
		uint8_t delayLowValue   = delayValues[ch] & 0xFF;			// Low byte
		uint8_t strobeHighValue = (strobeValues[ch] >> 8) & 0xFF;	// High byte
		uint8_t strobeLowValue  = strobeValues[ch] & 0xFF;			// Low byte

		stMsg.body.pageDataWrite.delayHighValue[ch] = delayHighValue;
		stMsg.body.pageDataWrite.delayLowValue[ch] = delayLowValue;
		stMsg.body.pageDataWrite.strobeHighValue[ch] = strobeHighValue;
		stMsg.body.pageDataWrite.strobeLowValue[ch] = strobeLowValue;
	}

	/* 메세지 체크섬 얻기 */
	finalizeMessage(stMsg);

	/* 메세지 전송 */
	if (SendData(stMsg, (int)stMsg.GetSize(), nTimeout))
	{
		return eSTROBE_LAMP_ERR_OK;
	}

	return eSTROBE_LAMP_ERR_PKT_BUFFER_FULL;
}

int CStrobeLampLib::Send_PageDataReadRequest(const uint8_t page, int nTimeout /*= 10000*/)
{
	if (IsConnect() == FALSE)
	{
		return eSTROBE_LAMP_ERR_DISCONNECT;
	}

	/* 데이터 범위 인터락 */
	ERR_CODE_STROBE eEcode = eSTROBE_LAMP_ERR_OK;
	if ((eSTROBE_LAMP_ERR_OK != (eEcode = IsValidPage(page))))
	{
		return eEcode;
	}

	/* 메세지 선언 */
	stStrobeLampMessage stMsg(eSTROBE_LAMP_PAGE_DATA_READ);

	/* 메세지 바디 입력 */
	stMsg.body.pageDataReadRequest.page = page;

	/* 메세지 체크섬 얻기 */
	finalizeMessage(stMsg);

	/* 메세지 전송 */
	if (SendData(stMsg, (int)stMsg.GetSize(), nTimeout))
	{
		return eSTROBE_LAMP_ERR_OK;
	}

	return eSTROBE_LAMP_ERR_PKT_BUFFER_FULL;
}

int CStrobeLampLib::Send_PageLoopDataWrite(const uint8_t page, const uint8_t loopDelay3Value, const uint8_t loopDelay2Value, const uint8_t loopDelay1Value, const uint8_t loopDelay0Value, const uint8_t loopCount, int nTimeout /*= 10000*/)
{
	if (IsConnect() == FALSE)
	{
		return eSTROBE_LAMP_ERR_DISCONNECT;
	}

	/* 데이터 범위 인터락 */
	ERR_CODE_STROBE eEcode = eSTROBE_LAMP_ERR_OK;
	if ((eSTROBE_LAMP_ERR_OK != (eEcode = IsValidPage(page))))
	{
		return eEcode;
	}

	/* 메세지 선언 */
	stStrobeLampMessage stMsg(eSTROBE_LAMP_PAGE_LOOP_DATA_WRITE);

	/* 메세지 바디 입력 */
	stMsg.body.pageLoopDataWrite.page = page;
	stMsg.body.pageLoopDataWrite.loopDelay3Value = loopDelay3Value;
	stMsg.body.pageLoopDataWrite.loopDelay2Value = loopDelay2Value;
	stMsg.body.pageLoopDataWrite.loopDelay1Value = loopDelay1Value;
	stMsg.body.pageLoopDataWrite.loopDelay0Value = loopDelay0Value;
	stMsg.body.pageLoopDataWrite.loopCount = loopCount;

	/* 메세지 체크섬 얻기 */
	finalizeMessage(stMsg);

	/* 메세지 전송 */
	if (SendData(stMsg, (int)stMsg.GetSize(), nTimeout))
	{
		return eSTROBE_LAMP_ERR_OK;
	}

	return eSTROBE_LAMP_ERR_PKT_BUFFER_FULL;
}

int CStrobeLampLib::Send_PageLoopDataRead(const uint8_t page, int nTimeout /*= 10000*/)
{
	if (IsConnect() == FALSE)
	{
		return eSTROBE_LAMP_ERR_DISCONNECT;
	}

	/* 데이터 범위 인터락 */
	ERR_CODE_STROBE eEcode = eSTROBE_LAMP_ERR_OK;
	if ((eSTROBE_LAMP_ERR_OK != (eEcode = IsValidPage(page))))
	{
		return eEcode;
	}

	/* 메세지 선언 */
	stStrobeLampMessage stMsg(eSTROBE_LAMP_PAGE_LOOP_DATA_READ);

	/* 메세지 바디 입력 */
	stMsg.body.pageLoopDataReadRequest.page = page;

	/* 메세지 체크섬 얻기 */
	finalizeMessage(stMsg);

	/* 메세지 전송 */
	if (SendData(stMsg, (int)stMsg.GetSize(), nTimeout))
	{
		return eSTROBE_LAMP_ERR_OK;
	}

	return eSTROBE_LAMP_ERR_PKT_BUFFER_FULL;
}

int CStrobeLampLib::Send_StrobeMode(const uint8_t mode, int nTimeout /*= 10000*/)
{
	if (IsConnect() == FALSE)
	{
		return eSTROBE_LAMP_ERR_DISCONNECT;
	}

	/* 데이터 범위 인터락 */
	ERR_CODE_STROBE eEcode = eSTROBE_LAMP_ERR_OK;
	if ((eSTROBE_LAMP_ERR_OK != (eEcode = IsValidStrobeMode(mode))))
	{
		return eEcode;
	}

	/* 메세지 선언 */
	stStrobeLampMessage stMsg(eSTROBE_LAMP_STROBE_MODE);

	/* 메세지 바디 입력 */
	stMsg.body.strobeMode.mode = mode;

	/* 메세지 체크섬 얻기 */
	finalizeMessage(stMsg);
	
	/* 메세지 전송 */
	if (SendData(stMsg, (int)stMsg.GetSize(), nTimeout))
	{
		return eSTROBE_LAMP_ERR_OK;
	}

	return eSTROBE_LAMP_ERR_PKT_BUFFER_FULL;
}

int CStrobeLampLib::Send_TriggerMode(const uint8_t mode, int nTimeout /*= 10000*/)
{
	if (IsConnect() == FALSE)
	{
		return eSTROBE_LAMP_ERR_DISCONNECT;
	}

	/* 메세지 선언 */
	stStrobeLampMessage stMsg(eSTROBE_LAMP_TRIGGER_MODE);

	/* 메세지 바디 입력 */
	stMsg.body.triggerMode.mode = mode;

	/* 메세지 체크섬 얻기 */
	finalizeMessage(stMsg);

	/* 메세지 전송 */
	if (SendData(stMsg, (int)stMsg.GetSize(), nTimeout))
	{
		return eSTROBE_LAMP_ERR_OK;
	}

	return eSTROBE_LAMP_ERR_PKT_BUFFER_FULL;
}

int CStrobeLampLib::Send_DataSave(int nTimeout /*= 10000*/)
{
	if (IsConnect() == FALSE)
	{
		return eSTROBE_LAMP_ERR_DISCONNECT;
	}

	/* 메세지 선언 */
	stStrobeLampMessage stMsg(eSTROBE_LAMP_DATA_SAVE);

	/* 메세지 체크섬 얻기 */
	finalizeMessage(stMsg);

	/* 메세지 전송 */
	if (SendData(stMsg, (int)stMsg.GetSize(), nTimeout))
	{
		return eSTROBE_LAMP_ERR_OK;
	}

	return eSTROBE_LAMP_ERR_PKT_BUFFER_FULL;
}

int CStrobeLampLib::Send_DataLoad(int nTimeout /*= 10000*/)
{
	if (IsConnect() == FALSE)
	{
		return eSTROBE_LAMP_ERR_DISCONNECT;
	}

	/* 메세지 선언 */
	stStrobeLampMessage stMsg(eSTROBE_LAMP_DATA_LOAD);

	/* 메세지 체크섬 얻기 */
	finalizeMessage(stMsg);

	/* 메세지 전송 */
	if (SendData(stMsg, (int)stMsg.GetSize(), nTimeout))
	{
		return eSTROBE_LAMP_ERR_OK;
	}

	return eSTROBE_LAMP_ERR_PKT_BUFFER_FULL;
}

int CStrobeLampLib::Send_StrobeTriggerModeRead(int nTimeout /*= 10000*/)
{
	if (IsConnect() == FALSE)
	{
		return eSTROBE_LAMP_ERR_DISCONNECT;
	}

	/* 메세지 선언 */
	stStrobeLampMessage stMsg(eSTROBE_LAMP_STROBE_TRIGGER_MODE_READ);

	/* 메세지 체크섬 얻기 */
	finalizeMessage(stMsg);

	/* 메세지 전송 */
	if (SendData(stMsg, (int)stMsg.GetSize(), nTimeout))
	{
		return eSTROBE_LAMP_ERR_OK;
	}

	return eSTROBE_LAMP_ERR_PKT_BUFFER_FULL;
}

/*
 desc : 데이터가 수신된 경우, 호출됨
 parm : None
 retn : TRUE - 성공적으로 분석 (Ring Buffer) 처리
		FALSE- 분석 처리하지 않음 (아직 완벽하지 않은 패킷임)
*/
BOOL CStrobeLampLib::RecvData(STG_SLPR_RECV& stRecv)
{
	BOOL	bIsRequest = TRUE;
	int		nEventIndex = 0;

	uint8_t calculatedChecksum = CalculateChecksum(stRecv, stRecv.GetSize(false));
	if (calculatedChecksum != stRecv.tail.checksum)
	{
		TRACE(_T("RECV CMD : %d, LENGTH : %d, CHECKSUM : %d \n"), stRecv.header.command, stRecv.GetSize(), stRecv.tail.checksum);
		AfxMessageBox(L"STROBE LAMP - RECV DATA CHECKSUM ERROR");
		return FALSE;
	}

	m_syncResult.Enter();
	m_lResultStruct.push_back(stRecv);

	for (int i = 0; i < m_vReplyMsg.size(); i++)
	{
		if (stRecv.header.command == m_vReplyMsg.at(i))
		{
			nEventIndex = i;
			bIsRequest = FALSE;
			break;
		}
	}

	TRACE(_T("RECV CMD : %d, LENGTH : %d, CHECKSUM : %d, COUNT : %d \n"), stRecv.header.command, stRecv.GetSize(), stRecv.tail.checksum, (int)m_lResultStruct.size());

	if (bIsRequest)
	{
		/* Requset 일 경우 MainDlg 쪽으로 Packet을 날린다. */
		::PostMessage(m_hMainWnd, WM_STROBE_LAMP_MSG_THREAD, (WPARAM)&m_lResultStruct.back(), 0);
	}
	else
	{
		m_evt_comm_wait[nEventIndex]->Set();
	}

	m_syncResult.Leave();

	/* --------------------------------------------------------- */
	/* 가장 최근에 수신된 시간 저장. 장시간 수신 없으면 연결 해제*/
	/* --------------------------------------------------------- */
	m_u64RecvTime = GetTickCount64();

	return TRUE;
}

void CStrobeLampLib::AddPktData(STG_SLPR_RECV& stData)
{
	m_syncRecv.Enter();
	m_qRecvStruct.push(stData);
	m_syncRecv.Leave();
}

void CStrobeLampLib::PopPktData()
{
	if (m_qRecvStruct.empty())
	{
		return;
	}

	RecvData(m_qRecvStruct.front());

	m_syncRecv.Enter();
	m_qRecvStruct.pop();
	m_syncRecv.Leave();
	conditionValue.notify_all();
}

/*
 desc : 즉시 송신 작업 수행
 parm : None
 retn : None
*/
BOOL CStrobeLampLib::SendData(STG_SLPR_SEND& stSend, int nSize, int nTimeout)
{
	BOOL bSucc = FALSE;
	int		nEventIndex = -1;
	/* 동기 진입 */

	std::unique_lock<std::mutex> lock(lockMutex);
	conditionValue.wait(lock, [&] { return m_qRecvStruct.empty(); }); //만약 처리중인 (수신된) 메세지가 있을경우 스레드 팬딩.

	if (m_syncSend.Enter())
	{
		if (IsConnect())
		{

			m_u64SendTime = GetTickCount64();
			
			TRACE(_T("SEND CMD : %d, LENGTH : %d, CHECKSUM : %d, TIMEOUT : %d \n"), stSend.header.command, nSize, stSend.tail.checksum, nTimeout);

			/* 데이터 전송 */
			//std::vector<BYTE> serializedData = SerializeMassege(stSend);
			//BYTE* bpData = serializedData.data();
			//DWORD dwSendCount = m_SocketClient.Write(bpData, (DWORD)nSize, NULL);

			/* 데이터 전송 */

			BYTE* bpData = SerializeMessage(stSend);
			DWORD dwSendCount = m_SocketClient.Write(bpData, (DWORD)nSize, NULL);
			delete[] bpData; // 메모리 해제

			/* 송신 실패이면 ... */
			if (dwSendCount != nSize)
			{
				LOG_ERROR(ENG_EDIC::en_strobe_lamp, L"Failed to send the packet data [Strobe Lamp Info]");
				/* 동기 해제 */
				m_syncSend.Leave();
				return FALSE;
			}

			for (int i = 0; i < m_vReplyMsg.size(); i++)
			{
				if (stSend.header.command == m_vReplyMsg.at(i))
				{
					m_evt_comm_wait[i]->Reset();
					nEventIndex = i;

					break;
				}
			}

			bSucc = TRUE;
		}
		else
		{
			if (g_pstConfig->set_comn.run_emulate_mode && IsConnect() == FALSE)
			{
				bSucc = TRUE;
			}
		}
	}

	/* 동기 해제 */
	m_syncSend.Leave();

	if (0 <= nEventIndex)
	{
		if (FALSE == WaitReply(nEventIndex, nTimeout))
		{
			return eSTROBE_LAMP_ERR_ACK_TIMEOUT;
		}
	}

	return bSucc;
}

BOOL CStrobeLampLib::WaitReply(int nIndex, int nTimeout)
{
	if (IsConnect() == FALSE)
	{
		if (g_pstConfig->set_comn.run_emulate_mode)
		{
			return TRUE;
		} // _SIMUL
	}

	if (0 == nTimeout)
	{
		return TRUE;
	}

	DWORD dwResult = m_evt_comm_wait[nIndex]->Wait(nTimeout);

	if (dwResult != WAIT_OBJECT_0)
		return FALSE;

	return TRUE;
}

uint8_t CStrobeLampLib::CalculateChecksum(const stStrobeLampMessage& msg, size_t length)
{
	uint8_t checksum = msg.header.command;

	const uint8_t* data = reinterpret_cast<const uint8_t*>(&msg.body);

	// 체크섬 계산: 바디에서만 수행합니다.
	for (size_t i = 0; i < length - sizeof(Header) - sizeof(Tail); ++i)
	{
		checksum ^= data[i];
	}

	return checksum;
}

void CStrobeLampLib::finalizeMessage(stStrobeLampMessage& msg)
{
	msg.tail.checksum = CalculateChecksum(msg, msg.GetSize());
}

ERR_CODE_STROBE CStrobeLampLib::IsValidPage(uint8_t page)
{
	if (page > DEF_MAX_PAGE_NUMBER)
	{
		return eSTROBE_LAMP_ERR_INVALID_PAGE;
	}
	return eSTROBE_LAMP_ERR_OK;
}

ERR_CODE_STROBE CStrobeLampLib::IsValidChannel(uint8_t channel)
{
	if (channel > DEF_MAX_CHANNEL_NUMBER)
	{
		return eSTROBE_LAMP_ERR_INVALID_CHANNEL;
	}
	return eSTROBE_LAMP_ERR_OK;
}

ERR_CODE_STROBE CStrobeLampLib::IsValidDelayValue(uint16_t delayValue)
{
	if (delayValue > DEF_MAX_DELAY_VALUE) // 0x1770 in decimal
	{
		return eSTROBE_LAMP_ERR_INVALID_DELAY_VALUE;
	}
	return eSTROBE_LAMP_ERR_OK;
}

ERR_CODE_STROBE CStrobeLampLib::IsValidStrobeValue(uint16_t strobeValue)
{
	if (strobeValue > DEF_MAX_STROBE_VALUE)	// 0x03E8 in decimal
	{
		return eSTROBE_LAMP_ERR_INVALID_STROBE_VALUE;
	}
	return eSTROBE_LAMP_ERR_OK;
}

ERR_CODE_STROBE CStrobeLampLib::IsValidStrobeMode(uint16_t strobeMode)
{
	if (strobeMode > DEF_MAX_STROBE_MODE_NUMBER)
	{
		return eSTROBE_LAMP_ERR_INVALID_STROBE_MODE;
	}
	return eSTROBE_LAMP_ERR_OK;
}