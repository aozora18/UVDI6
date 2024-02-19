#include "pch.h"
#include "SerialComm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CSerialComm::CSerialComm(void)
{
	ResetSerialParam();

	m_bOpen = FALSE;
	SetUseThread(FALSE);
	SetExitThread(FALSE);

	m_bThRecvRunnig = FALSE;
	m_bThUserRunnig = FALSE;

	// 20210623 by jyoon : 초기화 추가
	m_pstRecvArg = NULL;
	m_pstUserArg = NULL;

	::InitializeCriticalSection(&m_csLock);
}

CSerialComm::~CSerialComm(void)
{
	SetExitThread(TRUE);
	PortClose();

	::DeleteCriticalSection(&m_csLock);
}

BOOL CSerialComm::PortOpen(ST_SERIAL_PARAM *stParam)
{
	if (nullptr == this)
	{
		return FALSE;
	}

	DWORD dwError = 0;

	if (IsOpen() == TRUE)
	{
		return TRUE;
	}

	if (stParam != nullptr)
	{
		SetSerialParam(*stParam);
	}
	else
	{
		stParam = &GetSerialParam();
	}

	memset(&m_osRead, 0, sizeof(OVERLAPPED));
	m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_osRead.hEvent == FALSE)
	{
		dwError = GetLastError();
		return FALSE;
	}

	memset(&m_osWrite, 0, sizeof(OVERLAPPED));
	m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_osWrite.hEvent == FALSE)
	{
		dwError = GetLastError();
		return FALSE;
	}

	TCHAR chPort[32] = { 0, };
	_stprintf_s(chPort, 32, _T("\\\\.\\COM%d"), stParam->byPort);

	m_hComm = CreateFile(chPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		dwError = GetLastError();
		return FALSE;
	}

	// 포트 EVENT 설정
	if (ResetEvent() == FALSE)
	{
		dwError = GetLastError();
		return FALSE;
	}

	// InQueue, OutQueue 크기 설정.
	if (SetupComm(m_hComm, __SIZE_OF_MAX_SERIAL_BUFFER_, __SIZE_OF_MAX_SERIAL_BUFFER_) == FALSE)
	{
		dwError = GetLastError();
		return FALSE;
	}

	// buffer data abort&clear
	PortClear();

	// timeout setting
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = __SERIAL_INTERVAL_TIMEOUT_;
	timeouts.ReadTotalTimeoutMultiplier = __SIZE_OF_COMM_BUFFER_;
	timeouts.ReadTotalTimeoutConstant = __SERIAL_INTERVAL_TIMEOUT_;
	timeouts.WriteTotalTimeoutMultiplier = 2 * CBR_9600 / stParam->dwBaud;
	timeouts.WriteTotalTimeoutConstant = 0;
	if (SetCommTimeouts(m_hComm, &timeouts) == FALSE)
	{
		dwError = GetLastError();
		return FALSE;
	}

	//// dcb setting
	DCB	dcb;
	dcb.DCBlength = sizeof(DCB);
	GetCommState(m_hComm, &dcb);
	dcb.BaudRate = stParam->dwBaud;
	dcb.ByteSize = stParam->byByteSize;
	dcb.Parity = stParam->byParity;
	dcb.StopBits = stParam->byStopBits;
	dcb.fInX = FALSE;
	dcb.fOutX = FALSE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	dcb.XonChar = ASCII_XON;
	dcb.XoffChar = ASCII_XOFF;
	dcb.XonLim = 0xFF;
	dcb.XoffLim = 0xFF;
	dcb.EvtChar = ASCII_CR;

	if (SetCommState(m_hComm, &dcb) == FALSE)
	{
		dwError = GetLastError();
		return FALSE;
	}

	m_bOpen = TRUE;

	SetExitThread(FALSE);

	if (GetUseThread() == TRUE)
	{
		m_pstRecvArg = new tagThreadArg;
		m_pstRecvArg->_this = this;
		m_pThRecv = AfxBeginThread(RecvThread, m_pstRecvArg);
		if (m_pThRecv == NULL)
		{
			delete m_pstRecvArg;
			m_pstRecvArg = NULL;

			PortClose();
			return FALSE;
		}

		m_pstUserArg = new tagThreadArg;
		m_pstUserArg->_this = this;
		m_pThUser = AfxBeginThread(UserThread, m_pstUserArg);
		if (m_pThUser == NULL)
		{
			delete m_pstUserArg;
			m_pstUserArg = NULL;

			PortClose();
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CSerialComm::PortClose()
{
	if (nullptr == this)
	{
		return FALSE;
	}

	DWORD dwExitCode = 0x00;

	SetExitThread(TRUE);

	// 쓰레드 종료까지 대기
	while (TRUE == IsThreadRunnig())
	{
		Sleep(10);
	}

	ResetEvent();

	if (IsOpen() == TRUE)
	{
		EscapeCommFunction(m_hComm, CLRDTR);
		PurgeComm(m_hComm, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
		CloseHandle(m_hComm);
		CloseHandle(m_osRead.hEvent);
		CloseHandle(m_osWrite.hEvent);

		m_bOpen = FALSE;
	}

	if (NULL != m_pstRecvArg)
	{
		delete m_pstRecvArg;
		m_pstRecvArg = NULL;
	}
	if (NULL != m_pstUserArg)
	{
		delete m_pstUserArg;
		m_pstUserArg = NULL;
	}

	return TRUE;
}

unsigned int __stdcall CSerialComm::RecvThread(LPVOID lpParam)
{
	tagThreadArg* pThis = (tagThreadArg*)lpParam;

	pThis->_this->m_bThRecvRunnig = TRUE;

	while (FALSE == pThis->_this->GetExitThread())
	{
		Sleep(10);

		::EnterCriticalSection(&pThis->_this->m_csLock);
		long lSize = pThis->_this->ReadCommInBuffer(pThis->_this->m_byRecvMsg);
		if (lSize > 0)
		{
			if (pThis->_this->RecvProcess(lSize, pThis->_this->m_byRecvMsg) != 0)
			{
				// 무슨 처리를 할까?
			}
		}
		::LeaveCriticalSection(&pThis->_this->m_csLock);
	}

	pThis->_this->m_bThRecvRunnig = FALSE;
// 	delete pThis;

	return 0;
}

unsigned int __stdcall CSerialComm::UserThread(LPVOID lpParam)
{
	tagThreadArg* pThis = (tagThreadArg*)lpParam;

	pThis->_this->m_bThUserRunnig = TRUE;

	while (FALSE == pThis->_this->GetExitThread())
	{
		Sleep(10);

		::EnterCriticalSection(&pThis->_this->m_csLock);
		pThis->_this->UserProcess();
		::LeaveCriticalSection(&pThis->_this->m_csLock);
	}

	pThis->_this->m_bThUserRunnig = FALSE;
// 	delete pThis;

	return 0;
}

void CSerialComm::ResetSerialParam()
{
	m_stSerialParam.Initialize();
}

void CSerialComm::SetSerialParam(ST_SERIAL_PARAM stData)
{
	m_stSerialParam = stData;
}

ST_SERIAL_PARAM CSerialComm::GetSerialParam()
{
	return m_stSerialParam;
}

BOOL CSerialComm::ResetEvent()
{
	BOOL bRet = SetCommMask(m_hComm, 0x00);
	if (bRet == TRUE)
	{
		m_dwEventData = 0x00;
	}

	return bRet;
}

BOOL CSerialComm::SetEvent(DWORD dwEvent)
{
	BOOL bRet = SetCommMask(m_hComm, dwEvent);
	if (bRet == TRUE)
	{
		m_dwEventData = dwEvent;
	}

	return bRet;
}

DWORD CSerialComm::GetEvent()
{
	return m_dwEventData;
}

BOOL CSerialComm::AddEvent(DWORD dwEvent)
{
	DWORD dwPreEvent = GetEvent();
	DWORD dwEventMask = dwEvent | dwPreEvent;

	return SetEvent(dwEventMask);
}

BOOL CSerialComm::RemoveEvent(DWORD dwEvent)
{
	DWORD dwPreEvent = GetEvent();
	DWORD dwEventMask = dwPreEvent & (0xFFFFFFFF ^ dwEvent);

	return SetEvent(dwEventMask);
}

long CSerialComm::GetInBufferCount()
{
	long lCount = 0;

	if (IsOpen() == FALSE)
	{
		return lCount;
	}

	DWORD dwRead = 0;
	DWORD dwError = 0;
	DWORD dwErrorFlags = 0;
	COMSTAT	comstat;

	if (ClearCommError(m_hComm, &dwErrorFlags, &comstat) == FALSE)
	{
		return eSERIAL_ERROR_UNDEFINED;
	}

	lCount = long(comstat.cbInQue);

	return lCount;
}

long CSerialComm::GetOutBufferCount()
{
	long lCount = 0;

	if (IsOpen() == FALSE)
	{
		return lCount;
	}

	DWORD dwRead = 0;
	DWORD dwError = 0;
	DWORD dwErrorFlags = 0;
	COMSTAT comstat;

	if (ClearCommError(m_hComm, &dwErrorFlags, &comstat) == FALSE)
	{
		return eSERIAL_ERROR_UNDEFINED;
	}

	lCount = long(comstat.cbOutQue);

	return lCount;
}

long CSerialComm::FindByteSize(long lSize, BYTE *pbyMain, BYTE *pbySep)
{
	BYTE *pAddr = pbyMain;

	long lSepLen = (long)strlen((char*)pbySep);

	long i;
	for (i = 0; i < lSize; i++)
	{
		if (memcmp(pAddr, pbySep, lSepLen) == 0)
		{
			return (int)(pAddr - pbyMain);
		}
		pAddr++;
	}

	return -1;
}

BOOL CSerialComm::PortClear()
{
	BOOL bRet = FALSE;
	bRet = PurgeComm(m_hComm, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

	return bRet;
}

long CSerialComm::ReadCommInBuffer(BYTE *pbyBuff, long lMaxSize)
{
	long lSize = 0;
	DWORD dwInterval = GetTickCount();
	while (true)
	{
		// 지정 길이 만큼 읽어야 하는 경우
		if (lSize >= lMaxSize)
		{
			break;
		}

		if (ReadComm1Byte(&pbyBuff[lSize]) == 1)
		{
			lSize++;
			dwInterval = GetTickCount();
		}
		else if (GetTickCount() - dwInterval > __SERIAL_INTERVAL_TIMEOUT_)
		{
			break;
		}

		// 210625 Add
		if (0 == lSize)
		{
			break;
		}
	}

	return lSize;
}

long CSerialComm::ReadComm1Byte(BYTE *pbyBuff)
{
	DWORD dwRead = DWORD(GetInBufferCount());

	if (dwRead <= 0)
	{
		return dwRead;
	}

	DWORD dwError = 0;
	DWORD dwErrorFlags = 0;
	COMSTAT comstat;

	if (dwRead > 0)
	{
		if (ReadFile(m_hComm, pbyBuff, 1, &dwRead, &m_osRead) == FALSE)
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				while (!GetOverlappedResult(m_hComm, &m_osRead, &dwRead, TRUE))
				{
					dwError = GetLastError();
					if (dwError != ERROR_IO_INCOMPLETE)
					{
						ClearCommError(m_hComm, &dwErrorFlags, &comstat);
						break;
					}
				}
			}
			else
			{
				dwRead = 0;
				ClearCommError(m_hComm, &dwErrorFlags, &comstat);
			}
		}
	}

	return dwRead;
}

long CSerialComm::ReadComm(BYTE *pbyData, BYTE *pbyETX, long lSize, long lTimeOut)
{
	if (IsOpen() == FALSE)
	{
		return -1;
	}

	if (lSize > __SIZE_OF_MAX_SERIAL_BUFFER_)
	{
		return -1;
	}

	// Timeout이 설정되지 않았다면, 현재 Buffer에 있는 Data만 읽도록 한다.
	if (lTimeOut <= 0)
	{
		return ReadCommInBuffer(pbyData, lSize);
	}
	else if (lTimeOut % 10 != 0)
	{
		lTimeOut /= 10;
		lTimeOut *= 10;
		lTimeOut += 10;
	}

	long lReadCount = 0;
	long lElapsedTime = 0;
	while (lElapsedTime < lTimeOut)
	{
		if (ReadComm1Byte((pbyData + lReadCount)) == 1)
		{
			// 1 byte가 읽혀진 경우
			lReadCount++;
			if (FindByteSize(lReadCount, pbyData, pbyETX) >= 0)
			{
				return lReadCount;
			}

			if (lReadCount >= lSize)
			{
				return -1;
			}

			continue;
		}

		Sleep(10);
		lElapsedTime += 10;
	}

	return -1;
}

long CSerialComm::WriteComm(BYTE *pbyData, long lSize)
{
	PortClear();

	DWORD dwWritten = 0;;
	DWORD dwError = 0;;
	DWORD dwErrorFlags = 0;;
	COMSTAT comstat;

	BOOL bRet = WriteFile(m_hComm, pbyData, (DWORD)lSize, &dwWritten, &m_osWrite);
	if (bRet == FALSE)
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			// 읽을 문자가 남아 있거나 전송할 문자가 남아 있을 경우 Overapped IO의
			// 특성에 따라 ERROR_IO_PENDING 에러 메시지가 전달된다.
			// commtimeouts에 정해준 시간만큼 기다려준다.
			while (!GetOverlappedResult(m_hComm, &m_osWrite, &dwWritten, TRUE))
			{
				dwError = GetLastError();
				if (dwError != ERROR_IO_INCOMPLETE)
				{
					ClearCommError(m_hComm, &dwErrorFlags, &comstat);
					break;
				}
			}
		}
		else
		{
			dwWritten = 0;
			ClearCommError(m_hComm, &dwErrorFlags, &comstat);
		}
	}

	return (long)dwWritten;
}

long CSerialComm::WriteComm(BYTE *pbyData)
{
	long lSize = (long)_tcslen((LPCTSTR)pbyData);
	long lBytes = lSize * sizeof(TCHAR);

	return WriteComm(pbyData, lBytes);
}
