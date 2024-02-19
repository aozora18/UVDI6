
/*
 desc : TCP/IP Client Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "PhilhmiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif



CPhilhmiLib::CPhilhmiLib()
{
	for (int i = 0; i < _countof(m_evt_comm_wait); i++)
	{
		m_evt_comm_wait[i] = NULL;
	}

	m_u64RecvTime = GetTickCount64();
	m_u64SendTime = GetTickCount64();

	m_vReplyMsg.push_back(ePHILHMI_P2C_RECIPE_CREATE);
	m_vReplyMsg.push_back(ePHILHMI_P2C_RECIPE_DELETE);
	m_vReplyMsg.push_back(ePHILHMI_P2C_RECIPE_MODIFY);
	m_vReplyMsg.push_back(ePHILHMI_P2C_RECIPE_SELECT);
	m_vReplyMsg.push_back(ePHILHMI_P2C_IO_STATUS);
	m_vReplyMsg.push_back(ePHILHMI_P2C_OUTPUT_ONOFF);
	m_vReplyMsg.push_back(ePHILHMI_P2C_ABS_MOVE);
	m_vReplyMsg.push_back(ePHILHMI_P2C_ABS_MOVE_COMPLETE);
	m_vReplyMsg.push_back(ePHILHMI_P2C_REL_MOVE);
	m_vReplyMsg.push_back(ePHILHMI_P2C_REL_MOVE_COMPLETE);
	m_vReplyMsg.push_back(ePHILHMI_P2C_CHAR_MOVE);
	m_vReplyMsg.push_back(ePHILHMI_P2C_CHAR_MOVE_COMPLETE);
	m_vReplyMsg.push_back(ePHILHMI_P2C_PROCESS_STEP);
	m_vReplyMsg.push_back(ePHILHMI_P2C_PROCESS_COMPLETE);
	m_vReplyMsg.push_back(ePHILHMI_P2C_SUB_PROCESS_COMPLETE);
	m_vReplyMsg.push_back(ePHILHMI_P2C_EC_MODIFY);
	m_vReplyMsg.push_back(ePHILHMI_P2C_INITIAL_COMPLETE);
	m_vReplyMsg.push_back(ePHILHMI_P2C_ALARM_OCCUR);
	m_vReplyMsg.push_back(ePHILHMI_P2C_EVENT_NOTIFY);
	m_vReplyMsg.push_back(ePHILHMI_P2C_INTERRUPT_STOP);

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
}

/*
 desc : Destructor
 parm : None
*/
CPhilhmiLib::~CPhilhmiLib()
{
	/* Philhmi Server와 연결 강제 해제 */
	Close();
	
}

BOOL CPhilhmiLib::Init(HWND hwnd, LPG_CTCS pComm, LPG_CCGS pComn)
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


unsigned CPhilhmiLib::ThreadHandlerProc()
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

int CPhilhmiLib::GetUniqueID()
{
	if (m_nUniqueID == 99999999)
		m_nUniqueID = 1;

	return m_nUniqueID++;
}


void CPhilhmiLib::OnDataReceived(CSocketHandle* pSH, const BYTE* pbData, DWORD dwCount, const SockAddrIn& addr)
{
	for (int i = 0; i < (int)dwCount; i++)
	{
		m_vRecvData.push_back(pbData[i]);

		if (m_vRecvData.size() == sizeof(STG_PPH))
		{
			STG_PPH stHeader;
			memcpy(&stHeader, m_vRecvData.data(), sizeof(STG_PPH));
			m_uiDataLength = stHeader.ulDataLen + sizeof(STG_PPH);
		}

		if (m_uiDataLength == m_vRecvData.size())
		{
			STG_PP_PACKET_RECV stRecv;
			memcpy(&stRecv, m_vRecvData.data(), m_vRecvData.size());
			AddPktData(stRecv);
			m_vRecvData.clear();
			m_uiDataLength = 0;
		}
	}
}

void CPhilhmiLib::OnThreadBegin(CSocketHandle* pSH)
{

}

void CPhilhmiLib::OnThreadExit(CSocketHandle* pSH)
{

}

void CPhilhmiLib::OnConnectionDropped(CSocketHandle* pSH)
{
	ASSERT(pSH == m_SocketClient);
	(pSH);

	m_SocketClient.Close();
	m_bAutoReconnection = TRUE;
}

void CPhilhmiLib::OnConnectionError(CSocketHandle* pSH, DWORD dwError)
{
	ASSERT(pSH == m_SocketClient);
	(pSH);

	m_SocketClient.Close();
	m_bAutoReconnection = TRUE;

}

BOOL CPhilhmiLib::Open(LPCTSTR Ip, int nPort)
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

BOOL CPhilhmiLib::Open(LPCTSTR LocalIp, LPCTSTR Ip, int nPort)
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

BOOL CPhilhmiLib::Open()
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

BOOL CPhilhmiLib::IsConnect()
{
	return (m_SocketClient.IsOpen() ? TRUE : FALSE);
}

BOOL CPhilhmiLib::Close()
{
	m_syncResult.Enter();
	m_bRun = FALSE;
	m_bAutoReconnection = FALSE;

	m_apNetMonitorThread->Terminate(0L);

	Sleep(500);

	m_SocketClient.Close();
	m_SocketClient.Terminate();

	m_vRecvData.clear();
	m_vReplyMsg.clear();
	m_lResultStruct.clear();
	while (!m_qRecvStruct.empty())
	{
		m_qRecvStruct.pop();
	}
	m_syncResult.Leave();
	
	return true;
}

// 주의! 이 전의 DATA를 가져올 수 있음. UniqueID 권장
BOOL CPhilhmiLib::GetRecvDataFromCommand(int nCommand, STG_PP_PACKET_RECV* pstRecv)
{
	m_syncResult.Enter();
	for (auto datapos = m_lResultStruct.begin(); datapos != m_lResultStruct.end(); datapos++)
	{
		if (datapos->st_header.nCommand == nCommand)
		{
			memcpy(pstRecv, &(*datapos), datapos->st_header.ulDataLen + sizeof(STG_PPH));
			m_lResultStruct.erase(datapos);
			m_syncResult.Leave();
			return TRUE;
		}
	}
	m_syncResult.Leave();
	return FALSE;
}

BOOL CPhilhmiLib::GetRecvDataFromUniqueID(unsigned int nUniqueID, STG_PP_PACKET_RECV* pstRecv)
{
	m_syncResult.Enter();
	for (auto datapos = m_lResultStruct.begin(); datapos != m_lResultStruct.end(); datapos++)
	{
		if (datapos->st_header.ulUniqueID == nUniqueID)
		{
			memcpy(pstRecv, &(*datapos), datapos->st_header.ulDataLen + sizeof(STG_PPH));
			m_lResultStruct.erase(datapos);
			m_syncResult.Leave();
			return TRUE;
		}
	}

	m_syncResult.Leave();

	return FALSE;
}

BOOL CPhilhmiLib::GetRecvWaitFromUniqueID(unsigned int nUniqueID, STG_PP_PACKET_RECV* pstRecv, int nTimeout)
{
	for (int i = 0; i < m_vReplyMsg.size(); i++)
	{
		if (pstRecv->st_header.nCommand == m_vReplyMsg.at(i))
		{
			if (FALSE == WaitReply(i, nTimeout))
			{
				return ePHILHMI_ERR_ACK_TIMEOUT;
			}

			break;
		}
	}

	return GetRecvDataFromUniqueID(nUniqueID, pstRecv);
}

BOOL CPhilhmiLib::DeleteRecvDataFromUniqueID(unsigned int nUniqueID)
{
	m_syncResult.Enter();
	for (auto datapos = m_lResultStruct.begin(); datapos != m_lResultStruct.end(); datapos++)
	{
		if (datapos->st_header.ulUniqueID == nUniqueID)
		{
			m_lResultStruct.erase(datapos);
			m_syncResult.Leave();
			return TRUE;
		}
	}

	m_syncResult.Leave();
	return FALSE;
}

/*
 desc : 데이터가 수신된 경우, 호출됨
 parm : None
 retn : TRUE - 성공적으로 분석 (Ring Buffer) 처리
		FALSE- 분석 처리하지 않음 (아직 완벽하지 않은 패킷임)
*/
BOOL CPhilhmiLib::RecvData(STG_PP_PACKET_RECV& stRecv)
{
	BOOL	bIsRequest = TRUE;
	int		nEventIndex = 0;

	m_syncResult.Enter();
	if (FALSE == m_bRun)
	{
		m_syncResult.Leave();
		return FALSE;
	}

	m_lResultStruct.push_back(stRecv);

	for (int i = 0; i < m_vReplyMsg.size(); i++)
	{
		if (stRecv.st_header.nCommand == m_vReplyMsg.at(i))
		{
			nEventIndex = i;
			bIsRequest = FALSE;
			break;
		}
	}

	TRACE(_T("RECV CMD : %d, LENGTH : %d, UNIQU : %d, COUNT : %d \n"), stRecv.st_header.nCommand, stRecv.st_header.ulDataLen, stRecv.st_header.ulUniqueID, (int)m_lResultStruct.size());

	if (FALSE == m_bRun)
	{
		m_syncResult.Leave();
		return FALSE;
	}

	if (bIsRequest)
	{
		/* Requset 일 경우 MainDlg 쪽으로 Packet을 날린다. */
		::PostMessage(m_hMainWnd, WM_PHILHMI_MSG_THREAD, (WPARAM)&m_lResultStruct.back(), m_lResultStruct.back().st_header.ulDataLen + sizeof(STG_PPH));
	}
	else
	{
		m_evt_comm_wait[nEventIndex]->Set();
	}

	if (DEF_COUNT_OF_MAX_RECVDATA < m_lResultStruct.size())
	{
		
		m_lResultStruct.erase(m_lResultStruct.begin());
	}
	m_syncResult.Leave();

	/* --------------------------------------------------------- */
	/* 가장 최근에 수신된 시간 저장. 장시간 수신 없으면 연결 해제*/
	/* --------------------------------------------------------- */
	m_u64RecvTime = GetTickCount64();

	return TRUE;
}

void CPhilhmiLib::AddPktData(STG_PP_PACKET_RECV& stData)
{
	m_syncRecv.Enter();
	m_qRecvStruct.push(stData);
	m_syncRecv.Leave();
}

void CPhilhmiLib::PopPktData()
{
	if (m_qRecvStruct.empty())
	{
		return;
	}

	RecvData(m_qRecvStruct.front());

	m_syncRecv.Enter();
	m_qRecvStruct.pop();
	m_syncRecv.Leave();
}

int CPhilhmiLib::Send_P2C_RCP_CREATE(STG_PP_P2C_RCP_CREATE& stSend, STG_PP_P2C_RCP_CREATE_ACK& stRecv, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	stSend.ulUniqueID = GetUniqueID();
	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		if (g_pstConfig->set_comn.run_emulate_mode && IsConnect() == FALSE)
		{
			memcpy(&m_stRecvData, &stSend, sizeof(STG_PPH));
			m_stRecvData.st_header.usErrorCode = ePHILHMI_ERR_OK;
		}

		if (0 < nTimeout)
		{
			stRecv.usErrorCode = ePHILHMI_ERR_NOT_EXIST_RECVDATA;
			m_syncResult.Enter();
			for (auto datapos = m_lResultStruct.begin(); datapos != m_lResultStruct.end(); datapos++)
			{
				if (datapos->st_header.nCommand == stSend.nCommand && datapos->st_header.ulUniqueID == stSend.ulUniqueID)
				{
					memcpy(&stRecv, &(*datapos), sizeof(stRecv));
					m_lResultStruct.erase(datapos);
					break;
				}
			}
			m_syncResult.Leave();

			// Error Code
			if (stRecv.usErrorCode != ePHILHMI_ERR_OK)
			{
				return stRecv.usErrorCode;
			}

			if (stSend.ulUniqueID != stRecv.ulUniqueID)
			{
				return ePHILHMI_ERR_PKT_UNIQUEID_MISSMATCH;
			}
		}

		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_ACK_TIMEOUT;
}

int CPhilhmiLib::Send_P2C_RCP_DELETE(STG_PP_P2C_RCP_DELETE& stSend, STG_PP_P2C_RCP_DELETE_ACK& stRecv, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	stSend.ulUniqueID = GetUniqueID();
	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		if (g_pstConfig->set_comn.run_emulate_mode && IsConnect() == FALSE)
		{
			memcpy(&m_stRecvData, &stSend, sizeof(STG_PPH));
			m_stRecvData.st_header.usErrorCode = ePHILHMI_ERR_OK;
		}

		if (0 < nTimeout)
		{
			stRecv.usErrorCode = ePHILHMI_ERR_NOT_EXIST_RECVDATA;
			m_syncResult.Enter();
			for (auto datapos = m_lResultStruct.begin(); datapos != m_lResultStruct.end(); datapos++)
			{
				if (datapos->st_header.nCommand == stSend.nCommand && datapos->st_header.ulUniqueID == stSend.ulUniqueID)
				{
					memcpy(&stRecv, &(*datapos), sizeof(stRecv));
					m_lResultStruct.erase(datapos);
					break;
				}
			}
			m_syncResult.Leave();

			// Error Code
			if (stRecv.usErrorCode != ePHILHMI_ERR_OK)
			{
				return stRecv.usErrorCode;
			}

			if (stSend.ulUniqueID != stRecv.ulUniqueID)
			{
				return ePHILHMI_ERR_PKT_UNIQUEID_MISSMATCH;
			}
		}

		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_ACK_TIMEOUT;
}

int CPhilhmiLib::Send_P2C_RCP_MODIFY(STG_PP_P2C_RCP_MODIFY& stSend, STG_PP_P2C_RCP_MODIFY_ACK& stRecv, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	stSend.ulUniqueID = GetUniqueID();
	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		if (g_pstConfig->set_comn.run_emulate_mode && IsConnect() == FALSE)
		{
			memcpy(&m_stRecvData, &stSend, sizeof(STG_PPH));
			m_stRecvData.st_header.usErrorCode = ePHILHMI_ERR_OK;
		}

		if (0 < nTimeout)
		{
			stRecv.usErrorCode = ePHILHMI_ERR_NOT_EXIST_RECVDATA;
			m_syncResult.Enter();
			for (auto datapos = m_lResultStruct.begin(); datapos != m_lResultStruct.end(); datapos++)
			{
				if (datapos->st_header.nCommand == stSend.nCommand && datapos->st_header.ulUniqueID == stSend.ulUniqueID)
				{
					memcpy(&stRecv, &(*datapos), sizeof(stRecv));
					m_lResultStruct.erase(datapos);
					break;
				}
			}
			m_syncResult.Leave();

			// Error Code
			if (stRecv.usErrorCode != ePHILHMI_ERR_OK)
			{
				return stRecv.usErrorCode;
			}

			if (stSend.ulUniqueID != stRecv.ulUniqueID)
			{
				return ePHILHMI_ERR_PKT_UNIQUEID_MISSMATCH;
			}
		}

		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_ACK_TIMEOUT;
}

int CPhilhmiLib::Send_P2C_RCP_SELECT(STG_PP_P2C_RCP_SELECT& stSend, STG_PP_P2C_RCP_SELECT_ACK& stRecv, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	stSend.ulUniqueID = GetUniqueID();
	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		if (g_pstConfig->set_comn.run_emulate_mode && IsConnect() == FALSE)
		{
			memcpy(&m_stRecvData, &stSend, sizeof(STG_PPH));
			m_stRecvData.st_header.usErrorCode = ePHILHMI_ERR_OK;
		}

		if (0 < nTimeout)
		{
			stRecv.usErrorCode = ePHILHMI_ERR_NOT_EXIST_RECVDATA;
			m_syncResult.Enter();
			for (auto datapos = m_lResultStruct.begin(); datapos != m_lResultStruct.end(); datapos++)
			{
				if (datapos->st_header.nCommand == stSend.nCommand && datapos->st_header.ulUniqueID == stSend.ulUniqueID)
				{
					memcpy(&stRecv, &(*datapos), sizeof(stRecv));
					m_lResultStruct.erase(datapos);
					break;
				}
			}
			m_syncResult.Leave();

			// Error Code
			if (stRecv.usErrorCode != ePHILHMI_ERR_OK)
			{
				return stRecv.usErrorCode;
			}

			if (stSend.ulUniqueID != stRecv.ulUniqueID)
			{
				return ePHILHMI_ERR_PKT_UNIQUEID_MISSMATCH;
			}
		}

		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_ACK_TIMEOUT;
}

int CPhilhmiLib::Send_P2C_IO_STATUS(STG_PP_P2C_IO_STATUS& stSend, STG_PP_P2C_IO_STATUS_ACK& stRecv, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	stSend.ulUniqueID = GetUniqueID();
	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		if (g_pstConfig->set_comn.run_emulate_mode && IsConnect() == FALSE)
		{
			memcpy(&m_stRecvData, &stSend, sizeof(STG_PPH));
			m_stRecvData.st_header.usErrorCode = ePHILHMI_ERR_OK;
			m_stRecvData.st_p2c_ack_io_status.Reset();
			m_stRecvData.st_p2c_ack_io_status.byInputCount = 8;
			m_stRecvData.st_p2c_ack_io_status.byOutputCount = 4;

			for (int i = 0; i < m_stRecvData.st_p2c_ack_io_status.byInputCount; i++)
			{
				m_stRecvData.st_p2c_ack_io_status.usInputData[i] = rand() % 65535;
			}

			for (int i = 0; i < m_stRecvData.st_p2c_ack_io_status.byOutputCount; i++)
			{
				m_stRecvData.st_p2c_ack_io_status.usOutputData[i] = rand() % 65535;
			}
		}

		if (0 < nTimeout)
		{
			stRecv.usErrorCode = ePHILHMI_ERR_NOT_EXIST_RECVDATA;
			m_syncResult.Enter();
			for (auto datapos = m_lResultStruct.begin(); datapos != m_lResultStruct.end(); datapos++)
			{
				if (datapos->st_header.nCommand == stSend.nCommand && datapos->st_header.ulUniqueID == stSend.ulUniqueID)
				{
					memcpy(&stRecv, &(*datapos), sizeof(stRecv));
					m_lResultStruct.erase(datapos);
					break;
				}
			}
			m_syncResult.Leave();

			// Error Code
			if (stRecv.usErrorCode != ePHILHMI_ERR_OK)
			{
				return stRecv.usErrorCode;
			}

			if (stSend.ulUniqueID != stRecv.ulUniqueID)
			{
				return ePHILHMI_ERR_PKT_UNIQUEID_MISSMATCH;
			}
		}

		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_ACK_TIMEOUT;
}

int CPhilhmiLib::Send_P2C_OUTPUT_ONOFF(STG_PP_P2C_OUTPUT_ONOFF& stSend, STG_PP_P2C_OUTPUT_ONOFF_ACK& stRecv, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	stSend.ulUniqueID = GetUniqueID();
	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		if (g_pstConfig->set_comn.run_emulate_mode && IsConnect() == FALSE)
		{
			memcpy(&m_stRecvData, &stSend, sizeof(STG_PPH));
			m_stRecvData.st_header.usErrorCode = ePHILHMI_ERR_OK;
		}

		if (0 < nTimeout)
		{
			stRecv.usErrorCode = ePHILHMI_ERR_NOT_EXIST_RECVDATA;
			m_syncResult.Enter();
			for (auto datapos = m_lResultStruct.begin(); datapos != m_lResultStruct.end(); datapos++)
			{
				if (datapos->st_header.nCommand == stSend.nCommand && datapos->st_header.ulUniqueID == stSend.ulUniqueID)
				{
					memcpy(&stRecv, &(*datapos), sizeof(stRecv));
					m_lResultStruct.erase(datapos);
					break;
				}
			}
			m_syncResult.Leave();

			// Error Code
			if (stRecv.usErrorCode != ePHILHMI_ERR_OK)
			{
				return stRecv.usErrorCode;
			}

			if (stSend.ulUniqueID != stRecv.ulUniqueID)
			{
				return ePHILHMI_ERR_PKT_UNIQUEID_MISSMATCH;
			}
		}

		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_ACK_TIMEOUT;
}

int CPhilhmiLib::Send_P2C_ABS_MOVE(STG_PP_P2C_ABS_MOVE& stSend, STG_PP_P2C_ABS_MOVE_ACK& stRecv, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	stSend.ulUniqueID = GetUniqueID();
	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		if (g_pstConfig->set_comn.run_emulate_mode && IsConnect() == FALSE)
		{
			memcpy(&m_stRecvData, &stSend, sizeof(STG_PPH));
			m_stRecvData.st_header.usErrorCode = ePHILHMI_ERR_OK;
		}

		if (0 < nTimeout)
		{
			stRecv.usErrorCode = ePHILHMI_ERR_NOT_EXIST_RECVDATA;
			m_syncResult.Enter();
			for (auto datapos = m_lResultStruct.begin(); datapos != m_lResultStruct.end(); datapos++)
			{
				if (datapos->st_header.nCommand == stSend.nCommand && datapos->st_header.ulUniqueID == stSend.ulUniqueID)
				{
					memcpy(&stRecv, &(*datapos), sizeof(stRecv));
					m_lResultStruct.erase(datapos);
					break;
				}
			}
			m_syncResult.Leave();

			// Error Code
			if (stRecv.usErrorCode != ePHILHMI_ERR_OK)
			{
				return stRecv.usErrorCode;
			}

			if (stSend.ulUniqueID != stRecv.ulUniqueID)
			{
				return ePHILHMI_ERR_PKT_UNIQUEID_MISSMATCH;
			}
		}

		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_ACK_TIMEOUT;
}

int CPhilhmiLib::Send_P2C_ABS_MOVE_COMP(STG_PP_P2C_ABS_MOVE_COMP& stSend, STG_PP_P2C_ABS_MOVE_COMP_ACK& stRecv, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	stSend.ulUniqueID = GetUniqueID();
	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		if (g_pstConfig->set_comn.run_emulate_mode && IsConnect() == FALSE)
		{
			memcpy(&m_stRecvData, &stSend, sizeof(STG_PPH));
			m_stRecvData.st_header.usErrorCode = ePHILHMI_ERR_OK;
		}

		if (0 < nTimeout)
		{
			stRecv.usErrorCode = ePHILHMI_ERR_NOT_EXIST_RECVDATA;
			m_syncResult.Enter();
			for (auto datapos = m_lResultStruct.begin(); datapos != m_lResultStruct.end(); datapos++)
			{
				if (datapos->st_header.nCommand == stSend.nCommand && datapos->st_header.ulUniqueID == stSend.ulUniqueID)
				{
					memcpy(&stRecv, &(*datapos), sizeof(stRecv));
					m_lResultStruct.erase(datapos);
					break;
				}
			}
			m_syncResult.Leave();

			// Error Code
			if (stRecv.usErrorCode != ePHILHMI_ERR_OK)
			{
				return stRecv.usErrorCode;
			}

			if (stSend.ulUniqueID != stRecv.ulUniqueID)
			{
				return ePHILHMI_ERR_PKT_UNIQUEID_MISSMATCH;
			}
		}

		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_ACK_TIMEOUT;
}

int CPhilhmiLib::Send_P2C_REL_MOVE(STG_PP_P2C_REL_MOVE& stSend, STG_PP_P2C_REL_MOVE_ACK& stRecv, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	stSend.ulUniqueID = GetUniqueID();
	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		if (g_pstConfig->set_comn.run_emulate_mode && IsConnect() == FALSE)
		{
			memcpy(&m_stRecvData, &stSend, sizeof(STG_PPH));
			m_stRecvData.st_header.usErrorCode = ePHILHMI_ERR_OK;
		}

		if (0 < nTimeout)
		{
			stRecv.usErrorCode = ePHILHMI_ERR_NOT_EXIST_RECVDATA;
			m_syncResult.Enter();
			for (auto datapos = m_lResultStruct.begin(); datapos != m_lResultStruct.end(); datapos++)
			{
				if (datapos->st_header.nCommand == stSend.nCommand && datapos->st_header.ulUniqueID == stSend.ulUniqueID)
				{
					memcpy(&stRecv, &(*datapos), sizeof(stRecv));
					m_lResultStruct.erase(datapos);
					break;
				}
			}
			m_syncResult.Leave();

			// Error Code
			if (stRecv.usErrorCode != ePHILHMI_ERR_OK)
			{
				return stRecv.usErrorCode;
			}

			if (stSend.ulUniqueID != stRecv.ulUniqueID)
			{
				return ePHILHMI_ERR_PKT_UNIQUEID_MISSMATCH;
			}
		}

		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_ACK_TIMEOUT;
}

int CPhilhmiLib::Send_P2C_REL_MOVE_COMP(STG_PP_P2C_REL_MOVE_COMP& stSend, STG_PP_P2C_REL_MOVE_COMP_ACK& stRecv, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	stSend.ulUniqueID = GetUniqueID();
	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		if (g_pstConfig->set_comn.run_emulate_mode && IsConnect() == FALSE)
		{
			memcpy(&m_stRecvData, &stSend, sizeof(STG_PPH));
			m_stRecvData.st_header.usErrorCode = ePHILHMI_ERR_OK;
		}

		if (0 < nTimeout)
		{
			stRecv.usErrorCode = ePHILHMI_ERR_NOT_EXIST_RECVDATA;
			m_syncResult.Enter();
			for (auto datapos = m_lResultStruct.begin(); datapos != m_lResultStruct.end(); datapos++)
			{
				if (datapos->st_header.nCommand == stSend.nCommand && datapos->st_header.ulUniqueID == stSend.ulUniqueID)
				{
					memcpy(&stRecv, &(*datapos), sizeof(stRecv));
					m_lResultStruct.erase(datapos);
					break;
				}
			}
			m_syncResult.Leave();

			// Error Code
			if (stRecv.usErrorCode != ePHILHMI_ERR_OK)
			{
				return stRecv.usErrorCode;
			}

			if (stSend.ulUniqueID != stRecv.ulUniqueID)
			{
				return ePHILHMI_ERR_PKT_UNIQUEID_MISSMATCH;
			}
		}

		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_ACK_TIMEOUT;
}

int CPhilhmiLib::Send_P2C_CHAR_MOVE(STG_PP_P2C_CHAR_MOVE& stSend, STG_PP_P2C_CHAR_MOVE_ACK& stRecv, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	stSend.ulUniqueID = GetUniqueID();
	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		if (g_pstConfig->set_comn.run_emulate_mode && IsConnect() == FALSE)
		{
			memcpy(&m_stRecvData, &stSend, sizeof(STG_PPH));
			m_stRecvData.st_header.usErrorCode = ePHILHMI_ERR_OK;
		}

		if (0 < nTimeout)
		{
			stRecv.usErrorCode = ePHILHMI_ERR_NOT_EXIST_RECVDATA;
			m_syncResult.Enter();
			for (auto datapos = m_lResultStruct.begin(); datapos != m_lResultStruct.end(); datapos++)
			{
				if (datapos->st_header.nCommand == stSend.nCommand && datapos->st_header.ulUniqueID == stSend.ulUniqueID)
				{
					memcpy(&stRecv, &(*datapos), sizeof(stRecv));
					m_lResultStruct.erase(datapos);
					break;
				}
			}
			m_syncResult.Leave();

			// Error Code
			if (stRecv.usErrorCode != ePHILHMI_ERR_OK)
			{
				return stRecv.usErrorCode;
			}

			if (stSend.ulUniqueID != stRecv.ulUniqueID)
			{
				return ePHILHMI_ERR_PKT_UNIQUEID_MISSMATCH;
			}
		}

		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_ACK_TIMEOUT;
}

int CPhilhmiLib::Send_P2C_CHAR_MOVE_COMP(STG_PP_P2C_CHAR_MOVE_COMP& stSend, STG_PP_P2C_CHAR_MOVE_COMP_ACK& stRecv, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	stSend.ulUniqueID = GetUniqueID();
	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		if (g_pstConfig->set_comn.run_emulate_mode && IsConnect() == FALSE)
		{
			memcpy(&m_stRecvData, &stSend, sizeof(STG_PPH));
			m_stRecvData.st_header.usErrorCode = ePHILHMI_ERR_OK;
		}

		if (0 < nTimeout)
		{
			stRecv.usErrorCode = ePHILHMI_ERR_NOT_EXIST_RECVDATA;
			m_syncResult.Enter();
			for (auto datapos = m_lResultStruct.begin(); datapos != m_lResultStruct.end(); datapos++)
			{
				if (datapos->st_header.nCommand == stSend.nCommand && datapos->st_header.ulUniqueID == stSend.ulUniqueID)
				{
					memcpy(&stRecv, &(*datapos), sizeof(stRecv));
					m_lResultStruct.erase(datapos);
					break;
				}
			}
			m_syncResult.Leave();

			// Error Code
			if (stRecv.usErrorCode != ePHILHMI_ERR_OK)
			{
				return stRecv.usErrorCode;
			}

			if (stSend.ulUniqueID != stRecv.ulUniqueID)
			{
				return ePHILHMI_ERR_PKT_UNIQUEID_MISSMATCH;
			}
		}

		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_ACK_TIMEOUT;
}

int CPhilhmiLib::Send_P2C_PROCESS_STEP(STG_PP_P2C_PROCESS_STEP& stSend, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	stSend.ulUniqueID = GetUniqueID();
	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		//if (g_pstConfig->set_comn.run_emulate_mode && IsConnect() == FALSE)
		//{
		//	memcpy(&m_stRecvData, &stSend, sizeof(STG_PPH));
		//	m_stRecvData.st_header.usErrorCode = ePHILHMI_ERR_OK;
		//}
		//
		//if (0 < nTimeout)
		//{
		//	stRecv.usErrorCode = ePHILHMI_ERR_NOT_EXIST_RECVDATA;
		//	m_syncResult.Enter();
		//	for (auto datapos = m_lResultStruct.begin(); datapos != m_lResultStruct.end(); datapos++)
		//	{
		//		if (datapos->st_header.nCommand == stSend.nCommand && datapos->st_header.ulUniqueID == stSend.ulUniqueID)
		//		{
		//			memcpy(&stRecv, &(*datapos), sizeof(stRecv));
		//			m_lResultStruct.erase(datapos);
		//			break;
		//		}
		//	}
		//	m_syncResult.Leave();

		//	// Error Code
		//	if (stRecv.usErrorCode != ePHILHMI_ERR_OK)
		//	{
		//		return stRecv.usErrorCode;
		//	}

		//	if (stSend.ulUniqueID != stRecv.ulUniqueID)
		//	{
		//		return ePHILHMI_ERR_PKT_UNIQUEID_MISSMATCH;
		//	}
		//}

		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_ACK_TIMEOUT;
}

int CPhilhmiLib::Send_P2C_PROCESS_COMP(STG_PP_P2C_PROCESS_COMP& stSend, STG_PP_P2C_PROCESS_COMP_ACK& stRecv, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	stSend.ulUniqueID = GetUniqueID();
	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		if (g_pstConfig->set_comn.run_emulate_mode && IsConnect() == FALSE)
		{
			memcpy(&m_stRecvData, &stSend, sizeof(STG_PPH));
			m_stRecvData.st_header.usErrorCode = ePHILHMI_ERR_OK;
		}

		if (0 < nTimeout)
		{
			stRecv.usErrorCode = ePHILHMI_ERR_NOT_EXIST_RECVDATA;
			m_syncResult.Enter();
			for (auto datapos = m_lResultStruct.begin(); datapos != m_lResultStruct.end(); datapos++)
			{
				if (datapos->st_header.nCommand == stSend.nCommand && datapos->st_header.ulUniqueID == stSend.ulUniqueID)
				{
					memcpy(&stRecv, &(*datapos), sizeof(stRecv));
					m_lResultStruct.erase(datapos);
					break;
				}
			}
			m_syncResult.Leave();

			// Error Code
			if (stRecv.usErrorCode != ePHILHMI_ERR_OK)
			{
				return stRecv.usErrorCode;
			}

			if (stSend.ulUniqueID != stRecv.ulUniqueID)
			{
				return ePHILHMI_ERR_PKT_UNIQUEID_MISSMATCH;
			}
		}

		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_ACK_TIMEOUT;
}

int CPhilhmiLib::Send_P2C_SUBPROCESS_COMP(STG_PP_P2C_SUBPROCESS_COMP& stSend, STG_PP_P2C_SUBPROCESS_COMP_ACK& stRecv, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	stSend.ulUniqueID = GetUniqueID();
	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		if (g_pstConfig->set_comn.run_emulate_mode && IsConnect() == FALSE)
		{
			memcpy(&m_stRecvData, &stSend, sizeof(STG_PPH));
			m_stRecvData.st_header.usErrorCode = ePHILHMI_ERR_OK;
		}

		if (0 < nTimeout)
		{
			stRecv.usErrorCode = ePHILHMI_ERR_NOT_EXIST_RECVDATA;
			m_syncResult.Enter();
			for (auto datapos = m_lResultStruct.begin(); datapos != m_lResultStruct.end(); datapos++)
			{
				if (datapos->st_header.nCommand == stSend.nCommand && datapos->st_header.ulUniqueID == stSend.ulUniqueID)
				{
					memcpy(&stRecv, &(*datapos), sizeof(stRecv));
					m_lResultStruct.erase(datapos);
					break;
				}
			}
			m_syncResult.Leave();

			// Error Code
			if (stRecv.usErrorCode != ePHILHMI_ERR_OK)
			{
				return stRecv.usErrorCode;
			}

			if (stSend.ulUniqueID != stRecv.ulUniqueID)
			{
				return ePHILHMI_ERR_PKT_UNIQUEID_MISSMATCH;
			}
		}

		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_ACK_TIMEOUT;
}

int CPhilhmiLib::Send_P2C_EC_MODIFY(STG_PP_P2C_EC_MODIFY& stSend, STG_PP_P2C_EC_MODIFY_ACK& stRecv, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	stSend.ulUniqueID = GetUniqueID();
	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		if (g_pstConfig->set_comn.run_emulate_mode && IsConnect() == FALSE)
		{
			memcpy(&m_stRecvData, &stSend, sizeof(STG_PPH));
			m_stRecvData.st_header.usErrorCode = ePHILHMI_ERR_OK;
		}

		if (0 < nTimeout)
		{
			stRecv.usErrorCode = ePHILHMI_ERR_NOT_EXIST_RECVDATA;
			m_syncResult.Enter();
			for (auto datapos = m_lResultStruct.begin(); datapos != m_lResultStruct.end(); datapos++)
			{
				if (datapos->st_header.nCommand == stSend.nCommand && datapos->st_header.ulUniqueID == stSend.ulUniqueID)
				{
					memcpy(&stRecv, &(*datapos), sizeof(stRecv));
					m_lResultStruct.erase(datapos);
					break;
				}
			}
			m_syncResult.Leave();

			// Error Code
			if (stRecv.usErrorCode != ePHILHMI_ERR_OK)
			{
				return stRecv.usErrorCode;
			}

			if (stSend.ulUniqueID != stRecv.ulUniqueID)
			{
				return ePHILHMI_ERR_PKT_UNIQUEID_MISSMATCH;
			}
		}

		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_ACK_TIMEOUT;
}

int CPhilhmiLib::Send_P2C_INITIAL_COMPLETE(STG_PP_P2C_INITIAL_COMPLETE& stSend, STG_PP_P2C_INITIAL_COMPLETE_ACK& stRecv, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	stSend.ulUniqueID = GetUniqueID();
	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		if (g_pstConfig->set_comn.run_emulate_mode && IsConnect() == FALSE)
		{
			memcpy(&m_stRecvData, &stSend, sizeof(STG_PPH));
			m_stRecvData.st_header.usErrorCode = ePHILHMI_ERR_OK;
		}

		if (0 < nTimeout)
		{
			stRecv.usErrorCode = ePHILHMI_ERR_NOT_EXIST_RECVDATA;
			m_syncResult.Enter();
			for (auto datapos = m_lResultStruct.begin(); datapos != m_lResultStruct.end(); datapos++)
			{
				if (datapos->st_header.nCommand == stSend.nCommand && datapos->st_header.ulUniqueID == stSend.ulUniqueID)
				{
					memcpy(&stRecv, &(*datapos), sizeof(stRecv));
					m_lResultStruct.erase(datapos);
					break;
				}
			}
			m_syncResult.Leave();

			// Error Code
			if (stRecv.usErrorCode != ePHILHMI_ERR_OK)
			{
				return stRecv.usErrorCode;
			}

			if (stSend.ulUniqueID != stRecv.ulUniqueID)
			{
				return ePHILHMI_ERR_PKT_UNIQUEID_MISSMATCH;
			}
		}

		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_ACK_TIMEOUT;
}

int CPhilhmiLib::Send_P2C_ALARM_OCCUR(STG_PP_P2C_ALARM_OCCUR& stSend, STG_PP_P2C_ALARM_OCCUR_ACK& stRecv, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	stSend.ulUniqueID = GetUniqueID();
	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		if (g_pstConfig->set_comn.run_emulate_mode && IsConnect() == FALSE)
		{
			memcpy(&m_stRecvData, &stSend, sizeof(STG_PPH));
			m_stRecvData.st_header.usErrorCode = ePHILHMI_ERR_OK;
		}

		if (0 < nTimeout)
		{
			stRecv.usErrorCode = ePHILHMI_ERR_NOT_EXIST_RECVDATA;
			m_syncResult.Enter();
			for (auto datapos = m_lResultStruct.begin(); datapos != m_lResultStruct.end(); datapos++)
			{
				if (datapos->st_header.nCommand == stSend.nCommand && datapos->st_header.ulUniqueID == stSend.ulUniqueID)
				{
					memcpy(&stRecv, &(*datapos), sizeof(stRecv));
					m_lResultStruct.erase(datapos);
					break;
				}
			}
			m_syncResult.Leave();

			// Error Code
			if (stRecv.usErrorCode != ePHILHMI_ERR_OK)
			{
				return stRecv.usErrorCode;
			}

			if (stSend.ulUniqueID != stRecv.ulUniqueID)
			{
				return ePHILHMI_ERR_PKT_UNIQUEID_MISSMATCH;
			}
		}

		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_ACK_TIMEOUT;
}

int CPhilhmiLib::Send_P2C_EVENT_NOTIFY(STG_PP_P2C_EVENT_NOTIFY& stSend, STG_PP_P2C_EVENT_NOTIFY_ACK& stRecv, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	stSend.ulUniqueID = GetUniqueID();
	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		if (g_pstConfig->set_comn.run_emulate_mode && IsConnect() == FALSE)
		{
			memcpy(&m_stRecvData, &stSend, sizeof(STG_PPH));
			m_stRecvData.st_header.usErrorCode = ePHILHMI_ERR_OK;
		}

		if (0 < nTimeout)
		{
			stRecv.usErrorCode = ePHILHMI_ERR_NOT_EXIST_RECVDATA;
			m_syncResult.Enter();
			for (auto datapos = m_lResultStruct.begin(); datapos != m_lResultStruct.end(); datapos++)
			{
				if (datapos->st_header.nCommand == stSend.nCommand && datapos->st_header.ulUniqueID == stSend.ulUniqueID)
				{
					memcpy(&stRecv, &(*datapos), sizeof(stRecv));
					m_lResultStruct.erase(datapos);
					break;
				}
			}
			m_syncResult.Leave();

			// Error Code
			if (stRecv.usErrorCode != ePHILHMI_ERR_OK)
			{
				return stRecv.usErrorCode;
			}

			if (stSend.ulUniqueID != stRecv.ulUniqueID)
			{
				return ePHILHMI_ERR_PKT_UNIQUEID_MISSMATCH;
			}
		}

		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_ACK_TIMEOUT;
}

int CPhilhmiLib::Send_P2C_INTERRUPT_STOP(STG_PP_P2C_INTERRUPT_STOP& stSend, STG_PP_P2C_INTERRUPT_STOP_ACK& stRecv, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	stSend.ulUniqueID = GetUniqueID();
	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		if (g_pstConfig->set_comn.run_emulate_mode && IsConnect() == FALSE)
		{
			memcpy(&m_stRecvData, &stSend, sizeof(STG_PPH));
			m_stRecvData.st_header.usErrorCode = ePHILHMI_ERR_OK;
		}

		if (0 < nTimeout)
		{
			stRecv.usErrorCode = ePHILHMI_ERR_NOT_EXIST_RECVDATA;
			m_syncResult.Enter();
			for (auto datapos = m_lResultStruct.begin(); datapos != m_lResultStruct.end(); datapos++)
			{
				if (datapos->st_header.nCommand == stSend.nCommand && datapos->st_header.ulUniqueID == stSend.ulUniqueID)
				{
					memcpy(&stRecv, &(*datapos), sizeof(stRecv));
					m_lResultStruct.erase(datapos);
					break;
				}
			}
			m_syncResult.Leave();

			// Error Code
			if (stRecv.usErrorCode != ePHILHMI_ERR_OK)
			{
				return stRecv.usErrorCode;
			}

			if (stSend.ulUniqueID != stRecv.ulUniqueID)
			{
				return ePHILHMI_ERR_PKT_UNIQUEID_MISSMATCH;
			}
		}

		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_ACK_TIMEOUT;
}

int CPhilhmiLib::Send_C2P_RCP_CREATE_ACK(STG_PP_C2P_RCP_CREATE_ACK& stSend, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_PKT_BUFFER_FULL;
}

int CPhilhmiLib::Send_C2P_RCP_DELETE_ACK(STG_PP_C2P_RCP_DELETE_ACK& stSend, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_PKT_BUFFER_FULL;
}

int CPhilhmiLib::Send_C2P_RCP_INFO_ACK(STG_PP_C2P_RCP_INFO_ACK& stSend, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_PKT_BUFFER_FULL;
}

int CPhilhmiLib::Send_C2P_RCP_LIST_ACK(STG_PP_C2P_RCP_LIST_ACK& stSend, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_PKT_BUFFER_FULL;
}

int CPhilhmiLib::Send_C2P_RCP_SELECT_ACK(STG_PP_C2P_RCP_SELECT_ACK& stSend, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_PKT_BUFFER_FULL;
}

int CPhilhmiLib::Send_C2P_RCP_MODIFY_ACK(STG_PP_C2P_RCP_MODIFY_ACK& stSend, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_PKT_BUFFER_FULL;
}

int CPhilhmiLib::Send_C2P_ABS_MOVE_ACK(STG_PP_C2P_ABS_MOVE_ACK& stSend, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_PKT_BUFFER_FULL;
}

int CPhilhmiLib::Send_C2P_ABS_MOVE_COMP_ACK(STG_PP_C2P_ABS_MOVE_COMP_ACK& stSend, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_PKT_BUFFER_FULL;
}

int CPhilhmiLib::Send_C2P_REL_MOVE_ACK(STG_PP_C2P_REL_MOVE_ACK& stSend, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_PKT_BUFFER_FULL;
}

int CPhilhmiLib::Send_C2P_REL_MOVE_COMP_ACK(STG_PP_C2P_REL_MOVE_COMP_ACK& stSend, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_PKT_BUFFER_FULL;
}

int CPhilhmiLib::Send_C2P_CHAR_MOVE_ACK(STG_PP_C2P_CHAR_MOVE_ACK& stSend, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_PKT_BUFFER_FULL;
}

int CPhilhmiLib::Send_C2P_CHAR_MOVE_COMP_ACK(STG_PP_C2P_CHAR_MOVE_COMP_ACK& stSend, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_PKT_BUFFER_FULL;
}

int CPhilhmiLib::Send_C2P_INITIAL_EXECUTE_ACK(STG_PP_C2P_INITIAL_EXECUTE_ACK& stSend, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_PKT_BUFFER_FULL;
}

int CPhilhmiLib::Send_C2P_PROCESS_EXECUTE_ACK(STG_PP_C2P_PROCESS_EXECUTE_ACK& stSend, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_PKT_BUFFER_FULL;
}

int CPhilhmiLib::Send_C2P_SUBPROCESS_EXECUTE_ACK(STG_PP_C2P_SUBPROCESS_EXECUTE_ACK& stSend, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_PKT_BUFFER_FULL;
}

int CPhilhmiLib::Send_C2P_STATUS_VALUE_ACK(STG_PP_C2P_STATUS_VALUE_ACK& stSend, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_PKT_BUFFER_FULL;
}

int CPhilhmiLib::Send_C2P_MODE_CHANGE_ACK(STG_PP_C2P_MODE_CHANGE_ACK& stSend, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_PKT_BUFFER_FULL;
}

int CPhilhmiLib::Send_C2P_EVENT_NOTIFY_ACK(STG_PP_C2P_EVENT_NOTIFY_ACK& stSend, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_PKT_BUFFER_FULL;
}

int CPhilhmiLib::Send_C2P_EVENT_STATUS_ACK(STG_PP_C2P_EVENT_STATUS_ACK& stSend, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_PKT_BUFFER_FULL;
}

int CPhilhmiLib::Send_C2P_TIME_SYNC_ACK(STG_PP_C2P_TIME_SYNC_ACK& stSend, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_PKT_BUFFER_FULL;
}

int CPhilhmiLib::Send_C2P_INTERRUPT_STOP_ACK(STG_PP_C2P_INTERRUPT_STOP_ACK& stSend, int nTimeout /*= 3000*/)
{
	if (false == g_pstConfig->set_comn.run_emulate_mode)
	{
		if (IsConnect() == FALSE)
		{
			return ePHILHMI_ERR_DISCONNECT;
		}
	}

	STG_PP_PACKET_SEND stUnionSend;
	memcpy(stUnionSend.stData, &stSend, sizeof(stSend));
	if (SendData(stUnionSend, stSend.ulDataLen + sizeof(STG_PPH), nTimeout))
	{
		return ePHILHMI_ERR_OK;
	}

	return ePHILHMI_ERR_PKT_BUFFER_FULL;
}

/*
 desc : 즉시 송신 작업 수행
 parm : None
 retn : None
*/
BOOL CPhilhmiLib::SendData(STG_PP_PACKET_SEND &stSend, int nSize, int nTimeout)
{
	BOOL bSucc = FALSE;
	int		nEventIndex = -1;
	/* 동기 진입 */
	if (m_syncSend.Enter())
	{
		if (IsConnect())
		{
			/* !!! 반드시 송신 시간 정보를 여기에 기입. !!! 가장 최근에 송신한 시간 저장 */
			m_u64SendTime = GetTickCount64();
			/* !!! 중요 !!! -> 마지막 Link 크기 제외 */
			/* 데이터 전송 */
			if (0 > stSend.st_header.nCommand)
			{
				TRACE(_T("SEND CMD : %d, LENGTH : %d, UNIQU : %d \n"), stSend.st_header.nCommand, stSend.st_header.ulDataLen, stSend.st_header.ulUniqueID);
			}

			TRACE(_T("SEND CMD : %d, LENGTH : %d, UNIQU : %d \n"), stSend.st_header.nCommand, stSend.st_header.ulDataLen, stSend.st_header.ulUniqueID);
			DWORD dwSendCount = m_SocketClient.Write((BYTE*)stSend.stData, (DWORD)nSize, NULL);

			/* 송신 실패이면 ... */
			if (dwSendCount != nSize)
			{
				LOG_ERROR(ENG_EDIC::en_philhmi, L"Failed to send the packet data [Philhmi Info]");
				/* 동기 해제 */
				m_syncSend.Leave();
				return FALSE;
			}

			for (int i = 0; i < m_vReplyMsg.size(); i++)
			{
				if (stSend.st_header.nCommand == m_vReplyMsg.at(i))
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
			return ePHILHMI_ERR_ACK_TIMEOUT;
		}
	}

	return bSucc;
}

BOOL CPhilhmiLib::WaitReply(int nIndex, int nTimeout)
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