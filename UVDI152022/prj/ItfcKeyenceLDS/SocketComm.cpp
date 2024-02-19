#include "pch.h"
#include "SocketComm.h"
#include "stdlib.h"
#include "stdio.h"
#include "Ws2tcpip.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CSocketCComm::CSocketCComm()
{
	m_lSleepUnit = 1;

	::InitializeCriticalSection(&m_csSocket);
	::InitializeCriticalSection(&m_csSendMsgQ);
	::InitializeCriticalSection(&m_csRecvMsgQ);

	m_pbySend = new deque<BYTE>;
	m_pbyRecv = new deque<BYTE>;

	SetConfigErrorCheck(true);
	SetConfigMaxBufSize(eSOCK_MAX__BUFFER);
	SetConfigPortNumber(2014);
	SetConfigLocalIPAddr("127.0.0.1");
	SetConfigRemoteIPAddr("127.0.0.1");

	SetFlagOpen(false);
	SetFlagConnected(false);
	SetFlagExitThread(false);
	SetFlagRetryConnect(false);
	SetFlagSendAble(false);
	SetFlagUseMasterThread(true);
	SetFlagUseSlaveThread(true);

	m_hEvent = NULL;
	m_hRecv = NULL;
	m_hSend = NULL;
	m_hMaster = NULL;
	m_hSlave = NULL;
}

CSocketCComm::~CSocketCComm()
{
	m_lSleepUnit = 0;
	SetFlagExitThread(true);

	ClearSendQ();
	ClearRecvQ();
	delete m_pbySend;
	delete m_pbyRecv;

	SLEEP(100);

	::DeleteCriticalSection(&m_csSocket);
	::DeleteCriticalSection(&m_csSendMsgQ);
	::DeleteCriticalSection(&m_csRecvMsgQ);
}

void CSocketCComm::SLEEP(long lmsec)
{
	Sleep(m_lSleepUnit*lmsec);
}

//忙式式式式式式式式式式式式式式式式式成式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式忖
//弛 PushSendQ		弛 雖薑脹 Size曖 Data蒂 Send Q薹縑 Push и棻					弛
//弛					弛															弛
//戍式式式式式式式式式式式式式式式式式托式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式扣
//弛 1. Input		弛 BYTE			*pbyData	: Q縑 厥擊 Data Pointer			弛
//弛					弛 unsigned int	nSize		: Q縑 厥擊 Data Size				弛
//弛 2. Output		弛 nothing													弛
//弛 3. return		弛 return		nSize		: Q縑 厥擎 Data Size				弛
//戌式式式式式式式式式式式式式式式式式扛式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式戎
long CSocketCComm::PushSendQ(BYTE *pbyData, unsigned int nSize)
{
	::EnterCriticalSection(&m_csSendMsgQ);

	unsigned int i;
	for (i = 0; i < nSize; i++)
	{
		m_pbySend->push_back(*pbyData++);
	}

	::LeaveCriticalSection(&m_csSendMsgQ);

	return nSize;
}

//忙式式式式式式式式式式式式式式式式式成式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式忖
//弛 PopSendQ		弛 雖薑脹 Size曖 Data蒂 Send Q縑憮 疏頂螞棻					弛
//弛					弛															弛
//戍式式式式式式式式式式式式式式式式式托式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式扣
//弛 1. Input		弛 BYTE			*pbyData	: Q縑憮 疏頂螢 Data Pointer		弛
//弛					弛 unsigned int	nSize		: Q縑憮 疏頂螢 Data Size			弛
//弛 2. Output		弛 nothing													弛
//弛 3. return		弛 return		nSize		: Q縑憮 疏頂螞 Data Size			弛
//戌式式式式式式式式式式式式式式式式式扛式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式戎
long CSocketCComm::PopSendQ(BYTE *pbyData, unsigned int nSize)
{
	::EnterCriticalSection(&m_csSendMsgQ);

	unsigned int i = 0;
	while (i < nSize && ((long)m_pbySend->size())>0)
	{
		pbyData[i++] = m_pbySend->front();
		m_pbySend->pop_front();
	}
	::LeaveCriticalSection(&m_csSendMsgQ);

	return i;
}

//忙式式式式式式式式式式式式式式式式式成式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式忖
//弛 FrontSendQ		弛 1Byte曖 Data蒂 Send Q縑憮 檗橫螞棻							弛
//弛					弛															弛
//戍式式式式式式式式式式式式式式式式式托式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式扣
//弛 1. Input		弛 nothing													弛
//弛 2. Output		弛 BYTE			*pbyData	: Q縑憮 檗橫螢 Data Pointer		弛
//弛 3. return		弛 return		nSize		: Q Size						弛
//戌式式式式式式式式式式式式式式式式式扛式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式戎
long CSocketCComm::FrontSendQ(BYTE *pbyData)
{
	::EnterCriticalSection(&m_csSendMsgQ);
	long lQSize = (long)m_pbySend->size();
	if (lQSize > 0)
	{
		*pbyData = m_pbySend->front();
	}
	::LeaveCriticalSection(&m_csSendMsgQ);

	return lQSize;
}

//忙式式式式式式式式式式式式式式式式式成式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式忖
//弛 SizeOfSendQ		弛 Q縑 氈朝 Data Size蒂 奩�納挬�								弛
//弛					弛															弛
//戍式式式式式式式式式式式式式式式式式托式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式扣
//弛 1. Input		弛 nothing													弛
//弛 2. Output		弛 nothing													弛
//弛 3. return		弛 return		nSize		: Q縑 菟橫 氈朝 Data Size		弛
//戌式式式式式式式式式式式式式式式式式扛式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式戎
long CSocketCComm::SizeOfSendQ()
{
	long lSize = 0;
	::EnterCriticalSection(&m_csSendMsgQ);
	lSize = (long)m_pbySend->size();
	::LeaveCriticalSection(&m_csSendMsgQ);

	return lSize;
}

//忙式式式式式式式式式式式式式式式式式成式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式忖
//弛 ClearSendQ		弛 Q縑 氈朝 Data蒂 賅舒 Reset и棻								弛
//弛					弛															弛
//戍式式式式式式式式式式式式式式式式式托式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式扣
//弛 1. Input		弛 nothing													弛
//弛 2. Output		弛 nothing													弛
//弛 3. return		弛 void														弛
//戌式式式式式式式式式式式式式式式式式扛式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式戎
void CSocketCComm::ClearSendQ()
{
	::EnterCriticalSection(&m_csSendMsgQ);

	m_pbySend->clear();

	::LeaveCriticalSection(&m_csSendMsgQ);
}

//忙式式式式式式式式式式式式式式式式式成式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式忖
//弛 PushRecvQ		弛 雖薑脹 Size曖 Data蒂 Send Q薹縑 Push и棻					弛
//弛					弛															弛
//戍式式式式式式式式式式式式式式式式式托式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式扣
//弛 1. Input		弛 BYTE			*pbyData	: Q縑 厥擊 Data Pointer			弛
//弛					弛 unsigned int	nSize		: Q縑 厥擊 Data Size				弛
//弛 2. Output		弛 nothing													弛
//弛 3. return		弛 return		nSize		: Q縑 厥擎 Data Size				弛
//戌式式式式式式式式式式式式式式式式式扛式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式戎
long CSocketCComm::PushRecvQ(BYTE *pbyData, unsigned int nSize)
{
	::EnterCriticalSection(&m_csRecvMsgQ);

	unsigned int i;
	for (i = 0; i < nSize; i++)
	{
		m_pbyRecv->push_back(*pbyData++);
	}

	::LeaveCriticalSection(&m_csRecvMsgQ);

	return nSize;
}

//忙式式式式式式式式式式式式式式式式式成式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式忖
//弛 PopRecvQ		弛 雖薑脹 Size曖 Data蒂 Send Q縑憮 疏頂螞棻					弛
//弛					弛															弛
//戍式式式式式式式式式式式式式式式式式托式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式扣
//弛 1. Input		弛 BYTE			*pbyData	: Q縑憮 疏頂螢 Data Pointer		弛
//弛					弛 unsigned int	nSize		: Q縑憮 疏頂螢 Data Size			弛
//弛 2. Output		弛 nothing													弛
//弛 3. return		弛 return		nSize		: Q縑憮 疏頂螞 Data Size			弛
//戌式式式式式式式式式式式式式式式式式扛式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式戎
long CSocketCComm::PopRecvQ(BYTE *pbyData, unsigned int nSize)
{
	::EnterCriticalSection(&m_csRecvMsgQ);

	unsigned int i = 0;
	while (i < nSize && ((long)m_pbyRecv->size())>0)
	{
		pbyData[i++] = m_pbyRecv->front();
		m_pbyRecv->pop_front();
	}
	::LeaveCriticalSection(&m_csRecvMsgQ);

	return i;
}

//忙式式式式式式式式式式式式式式式式式成式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式忖
//弛 FrontRecvQ		弛 1Byte曖 Data蒂 Send Q縑憮 檗橫螞棻							弛
//弛					弛															弛
//戍式式式式式式式式式式式式式式式式式托式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式扣
//弛 1. Input		弛 nothing													弛
//弛 2. Output		弛 BYTE			*pbyData	: Q縑憮 檗橫螢 Data Pointer		弛
//弛 3. return		弛 return		nSize		: Q Size						弛
//戌式式式式式式式式式式式式式式式式式扛式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式戎
long CSocketCComm::FrontRecvQ(BYTE *pbyData)
{
	::EnterCriticalSection(&m_csRecvMsgQ);
	long lQSize = (long)m_pbyRecv->size();
	if (lQSize > 0)
	{
		*pbyData = m_pbyRecv->front();
	}
	::LeaveCriticalSection(&m_csRecvMsgQ);

	return lQSize;
}

//忙式式式式式式式式式式式式式式式式式成式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式忖
//弛 SizeOfRecvQ		弛 Q縑 氈朝 Data Size蒂 奩�納挬�								弛
//弛					弛															弛
//戍式式式式式式式式式式式式式式式式式托式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式扣
//弛 1. Input		弛 nothing													弛
//弛 2. Output		弛 nothing													弛
//弛 3. return		弛 return		nSize		: Q縑 菟橫 氈朝 Data Size		弛
//戌式式式式式式式式式式式式式式式式式扛式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式戎
long CSocketCComm::SizeOfRecvQ()
{
	long lSize = 0;
	::EnterCriticalSection(&m_csRecvMsgQ);
	lSize = (long)m_pbyRecv->size();
	::LeaveCriticalSection(&m_csRecvMsgQ);

	return lSize;
}

//忙式式式式式式式式式式式式式式式式式成式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式忖
//弛 ClearRecvQ		弛 Q縑 氈朝 Data蒂 賅舒 Reset и棻								弛
//弛					弛															弛
//戍式式式式式式式式式式式式式式式式式托式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式扣
//弛 1. Input		弛 nothing													弛
//弛 2. Output		弛 nothing													弛
//弛 3. return		弛 void														弛
//戌式式式式式式式式式式式式式式式式式扛式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式戎
void CSocketCComm::ClearRecvQ()
{
	::EnterCriticalSection(&m_csRecvMsgQ);

	m_pbyRecv->clear();

	::LeaveCriticalSection(&m_csRecvMsgQ);
}

// Socket Config
void CSocketCComm::SetConfigErrorCheck(bool bErrChk)
{
	m_bErrorChk = bErrChk;
}

void CSocketCComm::SetConfigMaxBufSize(unsigned short unSize)
{
	m_unMaxBufSize = unSize;
}

void CSocketCComm::SetConfigPortNumber(unsigned short unPort)
{
	m_unPortNum = unPort;
}

void CSocketCComm::SetConfigLocalIPAddr(const char *pszIPAddr)
{
	long lLen = (long)strlen(pszIPAddr);
	if (lLen < 7 || lLen > 15)
		return;

	sprintf_s(m_szLocalIPAddr, 16, "%s", pszIPAddr);
	m_szLocalIPAddr[lLen] = 0x00;
}

void CSocketCComm::SetConfigRemoteIPAddr(const char *pszIPAddr)
{
	long lLen = (long)strlen(pszIPAddr);
	if (lLen < 7 || lLen > 15)
		return;

	sprintf_s (m_szRemoteIPAddr, 16, "%s", pszIPAddr);
	m_szRemoteIPAddr[lLen] = 0x00;
}

bool CSocketCComm::GetConfigErrorCheck()
{
	return m_bErrorChk;
}

unsigned short CSocketCComm::GetConfigMaxBufSize()
{
	return m_unMaxBufSize;
}

unsigned short CSocketCComm::GetConfigPortNumber()
{
	return m_unPortNum;
}

void CSocketCComm::GetConfigLocalIPAddr(char *pszIPAddr)
{
	long lLen = (long)strlen(m_szLocalIPAddr);
	sprintf_s(pszIPAddr, 16, "%s", m_szLocalIPAddr);
	m_szLocalIPAddr[lLen] = 0x00;
}

void CSocketCComm::GetConfigRemoteIPAddr(char *pszIPAddr)
{
	long lLen = (long)strlen(m_szRemoteIPAddr);
	sprintf_s(pszIPAddr, 16, "%s", m_szRemoteIPAddr);
	m_szRemoteIPAddr[lLen] = 0x00;
}

void CSocketCComm::SetFlagOpen(bool bOpen)
{
	m_bNetOpen = bOpen;
}

void CSocketCComm::SetFlagConnected(bool bConnected)
{
	m_bConnected = bConnected;
}

void CSocketCComm::SetFlagExitThread(bool bExit)
{
	m_bExitThread = bExit;
}

void CSocketCComm::SetFlagRetryConnect(bool bReconnect)
{
	m_bRetryConnect = bReconnect;
}

void CSocketCComm::SetFlagSendAble(bool bSendAble)
{
	m_bSendAble = bSendAble;
}

void CSocketCComm::SetFlagUseMasterThread(bool bUse)
{
	m_bUseMasterThread = bUse;
}

void CSocketCComm::SetFlagUseSlaveThread(bool bUse)
{
	m_bUseSlaveThread = bUse;
}

bool CSocketCComm::GetFlagOpen()
{
	return m_bNetOpen;
}

bool CSocketCComm::GetFlagConnected()
{
	return m_bConnected;
}

bool CSocketCComm::GetFlagExitThread()
{
	return m_bExitThread;
}

bool CSocketCComm::GetFlagRetryConnect()
{
	return m_bRetryConnect;
}

bool CSocketCComm::GetFlagSendAble()
{
	return m_bSendAble;
}

bool CSocketCComm::GetFlagMasterThread()
{
	return m_bUseMasterThread;
}

bool CSocketCComm::GetFlagSlaveThread()
{
	return m_bUseSlaveThread;
}

bool CSocketCComm::ChkConnect()
{
	bool bOpen = GetFlagOpen();
	bool bConnect = GetFlagConnected();

	return (bOpen && bConnect);
}

bool CSocketCComm::IsOpen()
{
	return GetFlagOpen();
}

void CSocketCComm::SendToSocket(long lSendSize, BYTE *pbyData)
{
	//int nResult = send(m_wSocket, (char*)pbyData, lSendSize, 0);

	//if (GetConfigErrorCheck() == true)
	//{
	//	if (nResult == SOCKET_ERROR)
	//	{
	//		int nCode = WSAGetLastError();

	//		if (nCode == WSAEWOULDBLOCK)
	//		{
	//			Send(lSendSize, pbyData);
	//		}
	//	}
	//}

	bool bAccept = false;
	int nSendPos = 0;
	int nResult	= 0;
	int nBuffSize = (int)lSendSize;

	while (!bAccept)
	{
		nResult = send(m_wSocket, (char*)pbyData + nSendPos, static_cast<int>(nBuffSize - nSendPos), 0);

		if (nResult == SOCKET_ERROR)
		{
			int eCode = WSAGetLastError();

			if (eCode != WSAEWOULDBLOCK)
			{
				// send socket error occurred WSAError
				break;
			}

			nResult = 0;
		}

		if (nResult != nBuffSize)
		{
			if (nResult > nBuffSize)
			{
				// send socket error limit buff size
				break;
			}

			nSendPos += nResult;

			if (nSendPos == nBuffSize)
			{
				bAccept = true;
				break;
			}
		}
		else
		{
			bAccept = true;
			break;
		}
	}
}

void CSocketCComm::RecvFromSocket()
{
	int nRecvSize = 0;
	unsigned short unMaxSize = GetConfigMaxBufSize();
	char* pszBuf = new char[unMaxSize];
	memset(pszBuf, 0x00, unMaxSize);

	//if (recv(m_wSocket, pszBuf, unMaxSize, MSG_PEEK) <= 0)
	//{
	//	delete[] pszBuf;
	//	return;
	//}

	nRecvSize = recv(m_wSocket, pszBuf, unMaxSize, 0);

	if (nRecvSize == SOCKET_ERROR)
	{
		int nCode = WSAGetLastError();
		switch (nCode)
		{
		case WSANOTINITIALISED:		SOCKET_PRINT_F("There is no successful WSAStartup Fucntion call before using this function.");									break;
		case WSAENETDOWN:			SOCKET_PRINT_F("Error in Network Sub System.");																					break;
		case WSAEFAULT:				SOCKET_PRINT_F("Parameter buf is not appropriate format.");																		break;
		case WSAENOTCONN:			SOCKET_PRINT_F("Socket is not accessed.");																						break;
		case WSAEINTR:				SOCKET_PRINT_F("Blocking call is canceled in WSACancelBlockingCall function.");													break;
		case WSAEINPROGRESS:		SOCKET_PRINT_F("Blocking Winsock v1.1 is in progress now, or service provider is still handling callback fucntion.");			break;
		case WSAENETRESET:			SOCKET_PRINT_F("Access is blocked during progressing operation.");																break;
		case WSAENOTSOCK:			SOCKET_PRINT_F("Descriptor is not socket.");																					break;
		case WSAEOPNOTSUPP:			SOCKET_PRINT_F("Although MSG_OOB is specified, socket is not same stream format as SOCK_STREAM.");								break;
		case WSAESHUTDOWN:			SOCKET_PRINT_F("Socket is shutdown.");																							break;
		case WSAEWOULDBLOCK:		SOCKET_PRINT_F("Socket is marking as non-blocking(asyncronous), and receiving operation is blocking.");							break;
		case WSAEMSGSIZE:			SOCKET_PRINT_F("Received messeage has loss because the message is too big to save at the assigned buffer.");					break;
		case WSAEINVAL:				SOCKET_PRINT_F("Socket is not binded by bind fucntion, unknown flag used,");														
									SOCKET_PRINT_F("MSG_OOB flag is specified for the socket with the SO_OOBININE opion enabled,");										
									SOCKET_PRINT_F("or parameter len is 0 or less than 0.");																		break;
		case WSAECONNABORTED:		SOCKET_PRINT_F("Virtual Connection Circuit Network is timeout or disconnected by other failure.");								break;
		case WSAETIMEDOUT:			SOCKET_PRINT_F("The access is dropped because of network failure or response failure of the other system.");					break;
		case WSAECONNRESET:			SOCKET_PRINT_F("Virtual Connection Circuit Network is reset by performing end of hard or abortive on a remote location.");		break;
		default:					SOCKET_PRINT_F("Socket Receive Error (Error Code = %d)", nCode);																break;
		}
		delete[] pszBuf;
		return;
	}

	//Edited by SM[20170428] : nRecvSize陛 -1檣唳辦 蛤幗斜 嫦儅
	if (ChkConnect() == false)
	{
		ClearRecvQ();
	}
	else
	{
		if (nRecvSize > 0)
		{
			PushRecvQ((BYTE *)pszBuf, (unsigned int)nRecvSize);
		}
	}
	delete[] pszBuf;
}

// Socket擊 non-blocking 戲煎 虜萇棻擠 Timeout擊 輿堅 connect �� select л熱縑憮 翱唸 唸婁蒂 馬雖и棻.
BOOL CSocketCComm::DoConnect(SOCKET wSock, const char *szHost, int nPort, int nTimeout)
{
	TIMEVAL Timeout;
	Timeout.tv_sec = nTimeout;
	Timeout.tv_usec = 0;

	struct sockaddr_in address;

	inet_pton(AF_INET, szHost, &address.sin_addr.s_addr);
	address.sin_port = htons(nPort);
	address.sin_family = AF_INET;

	//set the socket in non-blocking
	unsigned long unMode = 1;
	int nResult = ioctlsocket(wSock, FIONBIO, &unMode);

	if (nResult != NO_ERROR)
	{
		return FALSE;
	}

	if (connect(wSock, (struct sockaddr *)&address, sizeof(address)) == false)
	{
		return FALSE;
	}

	// restart the socket mode
	unMode = 0;
	nResult = ioctlsocket(wSock, FIONBIO, &unMode);

	if (NO_ERROR != nResult)
	{
		return FALSE;
	}

	fd_set fdWrite, fdErr;
	FD_ZERO(&fdWrite);
	FD_ZERO(&fdErr);
	FD_SET(wSock, &fdWrite);
	FD_SET(wSock, &fdErr);

	select(0, NULL, &fdWrite, &fdErr, &Timeout);

	if (FD_ISSET(wSock, &fdWrite))
	{
		return TRUE;
	}

	return FALSE;
}

bool CSocketCComm::NetOpen(const char *szLocalAddr, const char *szRemoteAddr, unsigned short nPortNum, unsigned short nBufSize, bool bChkError)
{
	if (nullptr == this)
	{
		return false;
	}

	if (ChkConnect() == true)
	{
		// 檜嘐 翱唸腎橫 氈朝 status縑憮朝 TRUE 奩��
		SOCKET_PRINT_F("IP = %s Port = %d: Socket is already connected.", szRemoteAddr, nPortNum);
		return true;
	}
//	SOCKET_PRINT_F("IP = %s Port = %d: Net Open Start", szRemoteAddr, nPortNum);

	// Net Open衛 濠翕戲煎 Retry 撲薑擊 ж紫煙 ж濠.
	SetFlagOpen(true);
	SetFlagRetryConnect(true);

	SetConfigMaxBufSize(nBufSize);
	SetConfigLocalIPAddr(szLocalAddr);
	SetConfigRemoteIPAddr(szRemoteAddr);
	SetConfigPortNumber(nPortNum);
	SetConfigErrorCheck(bChkError);

	if (m_hEvent == NULL)
	{
		m_pEvent = new _Thread;
		m_pEvent->_this = this;
		SOCKET_PRINT_F("IP = %s Port = %d: Net Open EventThread", szRemoteAddr, nPortNum);
		if ((m_hEvent = (HANDLE)_beginthreadex(NULL, 0, ThreadEvent, m_pEvent, 0, &m_unEventThreadID)) == NULL)
		{
			m_bNetOpen = FALSE;
			delete m_pEvent;
			m_pEvent = NULL;

			NetClose();
			return false;
		}
	}

	SetFlagConnected(false);
	SetFlagExitThread(false);

	m_hRecv = NULL;
	m_hSend = NULL;
	m_hMaster = NULL;
	m_hSlave = NULL;

	ClearRecvQ();
	ClearSendQ();

	if (WSAStartup(0x202, &m_wsaData) == SOCKET_ERROR)
	{
		WSACleanup();
		SOCKET_PRINT_F("IP = %s Port = %d: WSAStartup(0x202, &m_wsaData) = SOCKET_ERROR", szRemoteAddr, nPortNum);

		return false;
	}

	m_wSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (m_wSocket == INVALID_SOCKET)
	{
		WSACleanup();
		SOCKET_PRINT_F("IP = %s Port = %d: socket(AF_INET, SOCK_STREAM, 0) = INVALID_SOCKET", szRemoteAddr, nPortNum);

		return false;
	}

	m_NetAddress.sin_family = AF_INET;
	inet_pton(AF_INET, szLocalAddr, &m_NetAddress.sin_addr.s_addr);
	m_NetAddress.sin_port = htons(0);
	if (SOCKET_ERROR == ::bind(m_wSocket, (struct sockaddr*)&m_NetAddress, sizeof(m_NetAddress)))
	{
		closesocket(m_wSocket);
		WSACleanup();
		return false;
	}
	 
	int nError = DoConnect(m_wSocket, szRemoteAddr, nPortNum);
	if (!nError)
	{
		closesocket(m_wSocket);
		WSACleanup();
		SOCKET_PRINT_F("IP = %s Port = %d: connect(m_wSocket, (struct sockaddr*)&m_NetAddress, sizeof(m_NetAddress)) = %d", szRemoteAddr, nPortNum, nError);

		return false;
	}

	// Send Thread諦 Recv Thread蒂 儅撩и棻.
	m_pSend = new _Thread;
	m_pSend->_this = this;

	if ((m_hSend = (HANDLE)_beginthreadex(NULL, 0, ThreadSend, m_pSend, 0, &m_unSendThreadID)) == NULL)
	{
		delete m_pSend;
		m_pSend = NULL;

		NetClose(TRUE);
		SOCKET_PRINT_F("IP = %s Port = %d: ThreadSend Thread create failed", szRemoteAddr, nPortNum);
		return false;
	}

	m_pRecv = new _Thread;
	m_pRecv->_this = this;

	if ((m_hRecv = (HANDLE)_beginthreadex(NULL, 0, ThreadRecv, m_pRecv, 0, &m_unRecvThreadID)) == NULL)
	{
		delete m_pRecv;
		m_pRecv = NULL;

		NetClose(TRUE);
		SOCKET_PRINT_F("IP = %s Port = %d: ThreadRecv Thread create failed", szRemoteAddr, nPortNum);
		return false;
	}

	if (GetFlagMasterThread() == true)
	{
		// Master Req Thread諦 Slave Req Thread蒂 儅撩и棻.
		m_pMaster = new _Thread;
		m_pMaster->_this = this;

		if ((m_hMaster = (HANDLE)_beginthreadex(NULL, 0, ThreadMasterReq, m_pMaster, 0, &m_unMasterThreadID)) == NULL)
		{
			delete m_pMaster;
			m_pMaster = NULL;
			NetClose(TRUE);
			SOCKET_PRINT_F("IP = %s Port = %d: ThreadMasterReq Thread create failed", szRemoteAddr, nPortNum);
			return false;
		}
	}

	if (GetFlagSlaveThread() == true)
	{
		m_pSlave = new _Thread;
		m_pSlave->_this = this;

		if ((m_hSlave = (HANDLE)_beginthreadex(NULL, 0, ThreadSlaveReq, m_pSlave, 0, &m_unSlaveThreadID)) == NULL)
		{
			delete m_pSlave;
			m_pSlave = NULL;
			NetClose(TRUE);
			SOCKET_PRINT_F("IP = %s Port = %d: ThreadSlaveReq Thread create failed", szRemoteAddr, nPortNum);
			return false;
		}
	}

	SOCKET_PRINT_F("IP = %s Port = %d: Net Open success", szRemoteAddr, nPortNum);
	return true;
}

bool CSocketCComm::NetClose(bool bRetry)
{
	if (nullptr == this)
	{
		return false;
	}

	// Close 衛, NetOpen擊 濠翕戲煎 Retry й 匙檣雖蒂 雖薑.
	SetFlagRetryConnect(bRetry);

	DWORD dwExitCode;
	SetFlagExitThread(true);

	if (ChkConnect() == true)
	{
		OnDisconnect();
	}

	// Retry Connection 籀葬 ж雖 彊擊 唳辦縑虜  Event Thread蒂 謙猿 衛鑑棻.
	if (bRetry == false)
	{
		if (m_hEvent != NULL)
		{
			SetFlagOpen (false);
			while (true)
			{
				SetFlagOpen(false);
				SetFlagExitThread(true);
				GetExitCodeThread(m_hEvent, &dwExitCode);
				if (dwExitCode != STILL_ACTIVE)
				{
					CloseHandle(m_hEvent);
					m_hEvent = NULL;
					break;
				}
				SLEEP(10);
			}
		}
	}

	if (m_hSend != NULL)
	{
		while (true)
		{
			GetExitCodeThread(m_hSend, &dwExitCode);
			if (dwExitCode != STILL_ACTIVE)
			{
				CloseHandle(m_hSend);
				m_hSend = NULL;
				break;
			}
			SLEEP(10);
		}
	}

	if (m_hRecv != NULL)
	{
		while (true)
		{
			GetExitCodeThread(m_hRecv, &dwExitCode);
			if (dwExitCode != STILL_ACTIVE)
			{
				CloseHandle(m_hRecv);
				m_hRecv = NULL;
				break;
			}
			SLEEP(10);
		}
	}

	if (m_hMaster != NULL)
	{
		while (true)
		{
			GetExitCodeThread(m_hMaster, &dwExitCode);
			if (dwExitCode != STILL_ACTIVE)
			{
				CloseHandle(m_hMaster);
				m_hMaster = NULL;
				break;
			}
			SLEEP(10);
		}
	}

	if (m_hSlave != NULL)
	{
		while (true)
		{
			GetExitCodeThread(m_hSlave, &dwExitCode);
			if (dwExitCode != STILL_ACTIVE)
			{
				CloseHandle(m_hSlave);
				m_hSlave = NULL;
				break;
			}
			SLEEP(10);
		}
	}

	closesocket(m_wSocket);
	WSACleanup();

	SetFlagConnected(false);

	SOCKET_PRINT_F("Net Close Complete");
	return true;
}

void CSocketCComm::Send(long lSize, BYTE *pbyData)
{
	if (ChkConnect() == true)
	{
		if (GetFlagSendAble() == true)
		{
			PushSendQ(pbyData, (unsigned int)lSize);
		}
	}
	// Connect 腎雖 彊擎 唳辦
	else
	{
		if (SizeOfSendQ() > 0)
		{
			ClearSendQ();
		}
	}
}

unsigned int __stdcall CSocketCComm::ThreadEvent(void *lpParam)
{
	_Thread* pThis = (_Thread*)lpParam;

	WSANETWORKEVENTS WorkEvents;
	WSAEVENT hRecvEvent = WSACreateEvent();

	while (1)
	{
		pThis->_this->SLEEP(10);

		if ((pThis->_this->GetFlagExitThread() == true) && (pThis->_this->GetFlagOpen() == false))
		{
			break;
		}

		memset(&WorkEvents, 0x00, sizeof(WorkEvents));

		WSAEventSelect(pThis->_this->m_wSocket, hRecvEvent, FD_READ | FD_CLOSE | FD_WRITE | FD_CONNECT);
		WSAEnumNetworkEvents(pThis->_this->m_wSocket, hRecvEvent, &WorkEvents);

		if ((WorkEvents.lNetworkEvents & FD_READ) == FD_READ)
		{
			pThis->_this->RecvFromSocket();
		}

		if ((WorkEvents.lNetworkEvents & FD_WRITE) == FD_WRITE)
		{
			pThis->_this->SetFlagSendAble(true);
		}

		if ((WorkEvents.lNetworkEvents & FD_CONNECT) == FD_CONNECT)
		{
			pThis->_this->SetFlagConnected(true);
			// �挫� в蹂й 蛭..
			pThis->_this->SetFlagSendAble(true);
			pThis->_this->OnConnect();
		}

		if ((WorkEvents.lNetworkEvents & FD_CLOSE) == FD_CLOSE)
		{
			::EnterCriticalSection(&pThis->_this->m_csSocket);

			pThis->_this->SetFlagSendAble(false);
			pThis->_this->ClearSendQ();
			pThis->_this->ClearRecvQ();

			::LeaveCriticalSection(&pThis->_this->m_csSocket);

			pThis->_this->NetClose(true);
			pThis->_this->SOCKET_PRINT_F("Net Close");
			continue;
		}

		// Retry 褻勒 衛, 蕾樓 衛紫
		if ((pThis->_this->GetFlagConnected() == false) && (pThis->_this->GetFlagRetryConnect() == true))
		{	// Retry 衛紫
			char			szLocalIP[32]	= {0,};
			char			szRemoteIP[32]	= {0,};
			unsigned short	unPortNum		= 2014;
			unsigned short	unMaxBufSize	= eSOCK_MAX__BUFFER;
			bool			bErrorCheck		= true;
			pThis->_this->GetConfigLocalIPAddr(szLocalIP);
			pThis->_this->GetConfigRemoteIPAddr(szRemoteIP);
			unPortNum = pThis->_this->GetConfigPortNumber();
			unMaxBufSize = pThis->_this->GetConfigMaxBufSize();
			bErrorCheck	= pThis->_this->GetConfigErrorCheck();
//			pThis->_this->SOCKET_PRINT_F("IP = %s Port = %d: Before Net Open Retry", szIP, unPortNum);
			pThis->_this->NetOpen(szLocalIP, szRemoteIP, unPortNum, unMaxBufSize, bErrorCheck);
			pThis->_this->SOCKET_PRINT_F("IP = %s Port = %d: After Net Open Retry", szRemoteIP, unPortNum);
		}
	}

	closesocket(pThis->_this->m_wSocket);
	WSACloseEvent(hRecvEvent);

	delete pThis;

	return TRUE;
}

unsigned int __stdcall CSocketCComm::ThreadRecv(void *lpParam)
{
	_Thread* pThis = (_Thread*)lpParam;

	while (1)
	{
		pThis->_this->SLEEP(10);

		if (pThis->_this->GetFlagOpen() == false ||
			pThis->_this->GetFlagExitThread() == true)
		{
			break;
		}

		long lSize = pThis->_this->SizeOfRecvQ();
		if (lSize > 0)
		{
			BYTE* pbyBuf = new BYTE[lSize];
			pThis->_this->PopRecvQ(pbyBuf, (unsigned int)lSize);
			if (pThis->_this->RecvProcess(lSize, pbyBuf) != 0)
			{
				//
			}
			delete[] pbyBuf;
		}
	}

	delete pThis;

	return true;
}

unsigned int __stdcall CSocketCComm::ThreadSend(void *lpParam)
{
	_Thread* pThis = (_Thread*)lpParam;

	while (1)
	{
		pThis->_this->SLEEP(10);

		if (pThis->_this->GetFlagOpen() == false ||
			pThis->_this->GetFlagExitThread() == true)
		{
			break;
		}

		::EnterCriticalSection(&pThis->_this->m_csSocket);

		if (pThis->_this->GetFlagSendAble() != true)
		{
			::LeaveCriticalSection(&pThis->_this->m_csSocket);
			continue;
		}

		long lSize = pThis->_this->SizeOfSendQ();
		if (lSize > 0)
		{
			BYTE* pbyBuf = new BYTE[lSize];
			long lSendSize = pThis->_this->PopSendQ(pbyBuf, lSize);
			pThis->_this->SendToSocket(lSendSize, pbyBuf);
			delete[] pbyBuf;
		}

		::LeaveCriticalSection(&pThis->_this->m_csSocket);
	}

	delete pThis;

	return true;
}


unsigned int __stdcall CSocketCComm::ThreadMasterReq(void *lpParam)
{
	_Thread* pThis = (_Thread*)lpParam;

	while (1)
	{
		pThis->_this->SLEEP(10);

		if (pThis->_this->GetFlagOpen() == false ||
			pThis->_this->GetFlagExitThread() == true)
		{
			break;
		}

		pThis->_this->MasterRequest();
	}

	delete pThis;

	return true;
}

unsigned int __stdcall CSocketCComm::ThreadSlaveReq(void *lpParam)
{
	_Thread* pThis = (_Thread*)lpParam;

	while (1)
	{
		pThis->_this->SLEEP(10);

		if (pThis->_this->GetFlagOpen() == false ||
			pThis->_this->GetFlagExitThread() == true)
		{
			break;
		}

		pThis->_this->SlaveRequest();
	}

	delete pThis;

	return true;
}