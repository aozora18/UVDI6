#pragma once

#include <process.h>
#include <deque>

using namespace std;

#define eSOCK_MAX__BUFFER 65535

class CSocketCComm
{
public:
	CSocketCComm(void);
	~CSocketCComm(void);

private:
	long m_lSleepUnit;

public:
	void SLEEP(long lmsec);

// Critical Section
private:
	CRITICAL_SECTION m_csSocket;			// Socket ����
	CRITICAL_SECTION m_csSendMsgQ;			// Msg Q Processing�� ���		
	CRITICAL_SECTION m_csRecvMsgQ;			// Msg Q Processing�� ���		

// Message Q ó�� ����========================================================================
private:
	deque<BYTE>	*m_pbySend;
	deque<BYTE>	*m_pbyRecv;

public:
	long PushSendQ(BYTE *byData, unsigned int nSize);
	long PopSendQ(BYTE *byData, unsigned int nSize);
	long FrontSendQ(BYTE *pbyData);
	long SizeOfSendQ();
	void ClearSendQ();

	long PushRecvQ(BYTE *byData, unsigned int nSize);
	long PopRecvQ(BYTE *byData, unsigned int nSize);
	long FrontRecvQ(BYTE *pbyData);
	long SizeOfRecvQ();
	void ClearRecvQ();
//============================================================================================

// Socket Config ����=========================================================================
private:
	bool m_bErrorChk;					// Send / Recv�� Error Check�� �� �� �ΰ�?
	unsigned short m_unMaxBufSize;		// Recv ó���� �ѹ��� �޾Ƴ� Buffer Size ����
	unsigned short m_unPortNum;			// Socket Port Number
	char m_szLocalIPAddr[32];			// Socket Local IP Address
	char m_szRemoteIPAddr[32];			// Socket Remote IP Address

public:
	void SetConfigErrorCheck(bool bErrChk);
	void SetConfigMaxBufSize(unsigned short unSize);
	void SetConfigPortNumber(unsigned short unPort);
	void SetConfigLocalIPAddr(const char *pszIPAddr);
	void SetConfigRemoteIPAddr(const char *pszIPAddr);
	bool GetConfigErrorCheck();
	unsigned short GetConfigMaxBufSize();
	unsigned short GetConfigPortNumber();
	void GetConfigLocalIPAddr(char *pszIPAddr);
	void GetConfigRemoteIPAddr(char *pszIPAddr);
//============================================================================================

// Socket Flag ����===========================================================================
private:
	bool m_bNetOpen;					// Net Open ���� �� TRUE, Net Close ��, FALSE
	bool m_bConnected;					// Connect Flag
	bool m_bExitThread;					// Thread ���� ��, Thread ���� Flag
	bool m_bRetryConnect;				// �ڵ� Retry ó��...
	bool m_bSendAble;
	bool m_bUseMasterThread;
	bool m_bUseSlaveThread;

public:
	void SetFlagOpen(bool bOpen);
	void SetFlagConnected(bool bConnected);
	void SetFlagExitThread(bool bExit);
	void SetFlagRetryConnect(bool bReconnect);
	void SetFlagSendAble(bool bSendAble);
	void SetFlagUseMasterThread(bool bUse);
	void SetFlagUseSlaveThread(bool bUse);
	bool GetFlagOpen();
	bool GetFlagConnected();
	bool GetFlagExitThread();
	bool GetFlagRetryConnect();
	bool GetFlagSendAble();
	bool GetFlagMasterThread();
	bool GetFlagSlaveThread();
//============================================================================================

// Socket ����================================================================================
private:
	WSADATA	m_wsaData;
	SOCKET m_wSocket;
	SOCKADDR_IN m_NetAddress;

	void SendToSocket(long lSize, BYTE *pbyData);
	void RecvFromSocket();

	BOOL DoConnect(SOCKET wSock, const char *szHost, int nPort, int nTimeout = 1);

public:
	bool IsConnect();
	bool IsOpen();
	bool NetOpen(const char *szLocalAddr, const char *szRemoteAddr, unsigned short nPortNum, unsigned short nBufSize = eSOCK_MAX__BUFFER, bool bChkError = true);
	bool NetClose(bool bRetry = false);
	void Send(long lSize, BYTE *pbyData);
//============================================================================================

// Thread ó�� ����
private:
	typedef struct _Thread
	{
		CSocketCComm* _this;
	} _Thread;

	_Thread *m_pEvent;
	_Thread *m_pRecv;
	_Thread *m_pSend;
	_Thread *m_pMaster;
	_Thread *m_pSlave;

	HANDLE m_hEvent;
	HANDLE m_hRecv;
	HANDLE m_hSend;
	HANDLE m_hMaster;
	HANDLE m_hSlave;

	UINT m_unEventThreadID;
	UINT m_unRecvThreadID;
	UINT m_unSendThreadID;
	UINT m_unMasterThreadID;
	UINT m_unSlaveThreadID;

	static unsigned int __stdcall ThreadEvent(void *lpParam);
	static unsigned int __stdcall ThreadRecv(void *lpParam);
	static unsigned int __stdcall ThreadSend(void *lpParam);
	static unsigned int __stdcall ThreadMasterReq(void *lpParam);
	static unsigned int __stdcall ThreadSlaveReq(void *lpParam);

public:
	virtual long RecvProcess(long lSize, BYTE *pbyData)							{	return 0;		}
	virtual void MasterRequest()												{	return;			}
	virtual void SlaveRequest()													{	return;			}
	virtual void OnConnect()													{	return;			}
	virtual void OnDisconnect()													{	return;			}
	//Edited by SCH[20170428] : Recv �Լ� ���ϰ��� Error�ϰ�� ����ó�� �� �α� �߰�
	virtual void SOCKET_PRINT_F(LPCSTR szFmt, ...)								{	return;			}
};

class CSocketSComm
{
public:
	CSocketSComm(void);
	~CSocketSComm(void);

private:
	long m_lSleepUnit;

public:
	void SLEEP(long lmsec);

// Critical Section
private:
	CRITICAL_SECTION m_csSocket;				// Socket ����
	CRITICAL_SECTION m_csSendMsgQ;				// Msg Q Processing�� ���		
	CRITICAL_SECTION m_csRecvMsgQ;				// Msg Q Processing�� ���		

// Message Q ó�� ����========================================================================
private:
	deque<BYTE> *m_pbySend;
	deque<BYTE> *m_pbyRecv;

public:
	long PushSendQ(BYTE *byData, unsigned int nSize);
	long PopSendQ(BYTE *byData, unsigned int nSize);
	long FrontSendQ(BYTE *pbyData);
	long SizeOfSendQ();
	void ClearSendQ();

	long PushRecvQ(BYTE *byData, unsigned int nSize);
	long PopRecvQ(BYTE *byData, unsigned int nSize);
	long FrontRecvQ(BYTE *pbyData);
	long SizeOfRecvQ();
	void ClearRecvQ();
//============================================================================================


// Socket Config ����=========================================================================
private:
	bool m_bErrorChk;						// Send / Recv�� Error Check�� �� �� �ΰ�?
	unsigned short m_unMaxBufSize;			// Recv ó���� �ѹ��� �޾Ƴ� Buffer Size ����
	unsigned short m_unPortNum;				// Socket Port Number
	char m_szIPAddress[32];					// Socket IP Address

public:
	void SetConfigErrorCheck(bool bErrChk);
	void SetConfigMaxBufSize(unsigned short unSize);
	void SetConfigPortNumber(unsigned short unPort);
	void SetConfigIPAddress(const char *pszIPAddr);
	bool GetConfigErrorCheck();
	unsigned short GetConfigMaxBufSize();
	unsigned short GetConfigPortNumber();
	void GetConfigIPAddress(char *pszIPAddr);
//============================================================================================

// Socket Flag ����===========================================================================
private:
	bool m_bNetOpen;						// Net Open ���� �� TRUE, Net Close ��, FALSE
	bool m_bConnected;						// Connect Flag
	bool m_bExitThread;						// Thread ���� ��, Thread ���� Flag
	bool m_bRetryConnect;					// �ڵ� Retry ó��...
	bool m_bSendAble;
	bool m_bUseMasterThread;
	bool m_bUseSlaveThread;

public:
	void SetFlagOpen(bool bOpen);
	void SetFlagConnected(bool bConnected);
	void SetFlagExitThread(bool bExit);
	void SetFlagRetryConnect(bool bReconnect);
	void SetFlagSendAble(bool bSendAble);
	void SetFlagUseMasterThread(bool bUse);
	void SetFlagUseSlaveThread(bool bUse);
	bool GetFlagOpen();
	bool GetFlagConnected();
	bool GetFlagExitThread();
	bool GetFlagRetryConnect();
	bool GetFlagSendAble();
	bool GetFlagMasterThread();
	bool GetFlagSlaveThread();
//============================================================================================

// Socket ����================================================================================
private:
	WSADATA	m_wsaData;
	SOCKET m_wSocket;
	SOCKET m_DataSocket;
	SOCKADDR_IN	m_NetAddress;

	void SendToSocket(long lSize, BYTE *pbyData);
	void RecvFromSocket();

public:
	bool IsConnect();
	bool IsOpen();
	bool NetOpen(const char *szAddress, unsigned short nPortNum, unsigned short nBufSize = eSOCK_MAX__BUFFER, bool bChkError = true);
	bool NetClose(bool bRetry = false);
	void Send(long lSize, BYTE *pbyData);
//============================================================================================

// Thread ó�� ����
private:
	typedef struct _Thread
	{
		CSocketSComm* _this;
	} _Thread;

	_Thread *m_pEvent;
	_Thread *m_pRecv;
	_Thread *m_pSend;
	_Thread *m_pMaster;
	_Thread *m_pSlave;

	HANDLE m_hEvent;
	HANDLE m_hRecv;
	HANDLE m_hSend;
	HANDLE m_hMaster;
	HANDLE m_hSlave;

	UINT m_unEventThreadID;
	UINT m_unRecvThreadID;
	UINT m_unSendThreadID;
	UINT m_unMasterThreadID;
	UINT m_unSlaveThreadID;

	static unsigned int __stdcall ThreadEvent(void *lpParam);
	static unsigned int __stdcall ThreadRecv(void *lpParam);
	static unsigned int __stdcall ThreadSend(void *lpParam);
	static unsigned int __stdcall ThreadMasterReq(void *lpParam);
	static unsigned int __stdcall ThreadSlaveReq(void *lpParam);

public:
	virtual long RecvProcess(long lSize, BYTE *pbyData)						{ return 0; }
	virtual void MasterRequest()											{ return;	}
	virtual void SlaveRequest()												{ return;	}
	virtual void OnConnect()												{ return;	}
	virtual void OnDisconnect()												{ return;	}
};
