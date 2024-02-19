#pragma once

#define	ASCII_LF 0x0A
#define	ASCII_CR 0x0D
#define	ASCII_XON 0x11
#define	ASCII_XOFF 0x13
#define	ASCII_STX 0x02  
#define	ASCII_ETX 0x03

#define __SIZE_OF_MAX_SERIAL_BUFFER_ 65536
#define __SIZE_OF_COMM_BUFFER_ 256

#define __SERIAL_INTERVAL_TIMEOUT_ 16
#define __SERIAL_DEFAULT_TIMEOUT_ __SIZE_OF_COMM_BUFFER_ * __SERIAL_INTERVAL_TIMEOUT_

enum EN_SERIAL_ERROR
{
	eSERIAL_ERROR_SUCCESS,
	eSERIAL_ERROR_UNDEFINED,
	eSERIAL_ERROR_TIMEOUT,
	eSERIAL_ERROR_PORT_NOT_OPEN,
	eSERIAL_ERROR_READ,
	eSERIAL_ERROR_WRITE,
};

typedef struct ST_SERIAL_PARAM
{
	BYTE byPort;
	DWORD dwBaud;
	BYTE byByteSize;
	BYTE byParity;
	BYTE byStopBits;

	void ST_SERIAL_PARAM::Initialize()
	{
		byPort = 8;
		dwBaud = CBR_9600;
		byByteSize = 8;
		byStopBits = ONESTOPBIT;
		byParity = NOPARITY;
	};

	ST_SERIAL_PARAM::ST_SERIAL_PARAM()
	{
		Initialize();
	};

	void SetPort(BYTE byData)			{ byPort = byData;		}
	void SetBaudRate(DWORD dwData)		{ dwBaud = dwData;		}
	void SetDataBits(BYTE byData)		{ byByteSize = byData;	}
	void SetStopBits(BYTE byData)		{ byStopBits = byData;	}
	void SetParityBits(BYTE byData)		{ byParity = byData;	}

	BYTE GetPort()						{ return byPort;		}
	DWORD GetBaudRate()					{ return dwBaud;		}
	BYTE GetDataBits()					{ return byByteSize;	}
	BYTE GetStopBits()					{ return byStopBits;	}
	BYTE GetParityBits()				{ return byParity;		}
} ST_SERIAL_PARAM_;

class CSerialComm
{
public:
	CSerialComm(void);
	virtual ~CSerialComm(void);

private:
	ST_SERIAL_PARAM m_stSerialParam;

	BOOL		m_bOpen;				
	HANDLE		m_hComm;				
	OVERLAPPED	m_osRead;		
	OVERLAPPED	m_osWrite;		
	DWORD		m_dwEventData;

	BOOL		m_bUseThread;			
	BOOL		m_bExitThread;			

public:
	BOOL PortOpen(ST_SERIAL_PARAM *stParam = nullptr);
	BOOL PortClose();

	void ResetSerialParam();
	void SetSerialParam(ST_SERIAL_PARAM stData);
	ST_SERIAL_PARAM GetSerialParam();

	BOOL IsOpen()							{ return m_bOpen;			}

protected:
	void SetUseThread(BOOL bData)			{ m_bUseThread = bData;		}
	BOOL GetUseThread()						{ return m_bUseThread;		}
	void SetExitThread(BOOL bData)			{ m_bExitThread = bData;	}
	BOOL GetExitThread()					{ return m_bExitThread;		}

	BOOL IsThreadRunnig()					{ return (m_bThRecvRunnig | m_bThUserRunnig);	}

	BOOL ResetEvent();
	BOOL SetEvent(DWORD dwEvent);
	DWORD GetEvent();
	BOOL AddEvent(DWORD dwEvent);
	BOOL RemoveEvent(DWORD dwEvent);

	long GetInBufferCount();
	long GetOutBufferCount();
	long FindByteSize(long lSize, BYTE *pbyMain, BYTE *pbySep);

	BOOL PortClear();
	long ReadCommInBuffer(BYTE *pbyBuff, long lMaxSize = __SIZE_OF_MAX_SERIAL_BUFFER_);
	long ReadComm1Byte(BYTE *pbyBuff);

	long ReadComm(BYTE *pbyData, BYTE *pbyETX, long lSize, long lTimeOut = __SERIAL_DEFAULT_TIMEOUT_);
	long WriteComm(BYTE *pbyData, long lSize);
	long WriteComm(BYTE *pbyData);

private:
	typedef struct tagThreadArg
	{
		CSerialComm *_this;
	} ST_THREAD_ARGUMENTS_;

	tagThreadArg *m_pstRecvArg;
	tagThreadArg *m_pstUserArg;

	CWinThread *m_pThRecv;
	CWinThread *m_pThUser;

	BOOL m_bThRecvRunnig;
	BOOL m_bThUserRunnig;

	CRITICAL_SECTION m_csLock;

	static unsigned int __stdcall RecvThread(LPVOID lpParam);
	static unsigned int __stdcall UserThread(LPVOID lpParam);

	long m_lRecvSize;
	long m_lSendSize;
	BYTE m_byRecvMsg[__SIZE_OF_COMM_BUFFER_ + 1];
	BYTE m_bySendMsg[__SIZE_OF_COMM_BUFFER_ + 1];

protected:
	virtual long RecvProcess(long lSize, BYTE *pbyData)		{ return 0;		}
	virtual void UserProcess()								{ return;		}
};
