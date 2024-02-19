
/* ----------------------- */
/* desc : ��� �⺻ ������ */
/* ----------------------- */

#pragma once

#ifndef IPv4_LENGTH
#define LEN_IPV4				16
#endif // !IPv4_LENGTH

/* -------------------------------- */
/* ��Ŷ �۽� �õ� �߿�, ��۽� Ƚ�� */
/* -------------------------------- */
#ifndef TCP_SEND_TRY_COUNT
#define TCP_SEND_TRY_COUNT		3
#endif // !TCP_SEND_TRY_COUNT

/* ---------------------------------------- */
/* ��Ŷ �۽� �õ� �߿�, �۽� ��� ���� �ð� */
/* ---------------------------------------- */
#ifndef TCP_SEND_TRY_TIMEOUT
#define TCP_SEND_TRY_TIMEOUT	30	// ����: �и���
#endif // !TCP_SEND_TRY_TIMEOUT

/* ---------------------------------------- */
/* ��Ŷ �۽� �õ� �߿�, ��ü ��� ���� �ð� */
/* ---------------------------------------- */
#ifndef TCP_SEND_TOTAL_TIMEOUT
#define TCP_SEND_TOTAL_TIMEOUT	1000	// ����: �и���
#endif // !TCP_SEND_TOTAL_TIMEOUT

/* ---------------------------------------- */
/* ��Ŷ ���� �õ� �߿�, ��ü ��� ���� �ð� */
/* ---------------------------------------- */
#ifndef TCP_RECV_TOTAL_TIMEOUT
#define TCP_RECV_TOTAL_TIMEOUT	3000	// ����: �и���
#endif // !TCP_SEND_TOTAL_TIMEOUT

#define	PACKET_FAIL_PERIOD_TIME	300	/* ��Ŷ ��� ���� ���� �޽��� ����� ���޵Ǵ� ���� ���� ���� */

/* Log Macro */
#define LOG_TCPIP(app_id, level, mesg)	\
	do {	\
		uvLogs_SaveLogs(ENG_EDIC(app_id), ENG_LNWE(level), mesg, WFILE, WFUNC, WLINE);	\
	} while (0)
#define LOG_TCPIP2(app_id, level, mesg)	\
	do {	\
		uvLogs_SaveLogs2(ENG_EDIC(app_id), ENG_LNWE(level), mesg, WFILE, WFUNC, WLINE);	\
	} while (0)

/* --------------------------------------------------------------------------------------------- */
/*                                           Log Macro                                           */
/* --------------------------------------------------------------------------------------------- */

#include "../comn/ThinThread.h"
#include "../conf/tcpip.h"
#include "RingBuff.h"
#include "PktQue.h"

class CTcpThread : public CThinThread
{
// ������ & �İ���
public:
	CTcpThread(ENG_EDIC sys_id,					/* System ID */
			   UINT16 th_id,					// Thread ID
			   UINT16 tcp_port,					// TCP/IP Port
			   UINT32 source_ip,				// Local IPv4
			   UINT32 target_ip,				// Remote IPv4
			   UINT32 port_buff = 1024*8,		// TCP/IP Port Buffer Size (����: KBytes, About 8 KBytes)
			   UINT32 recv_buff	= 1024*1024*1,	// Packet ������ �ִ� ũ�� (����: Bytes. About 1 MBytes)
			   UINT32 ring_buff	= 1024*1024*2,	// Max Recv Packet Size (����: Bytes, About 2 MBytes)
			   UINT32 sock_time	= 30,
			   UINT32 recv_count= 32);			// �⺻ 30 �и��� ���� ���� �̺�Ʈ ������ ���� ��� (����: �и���)
	virtual ~CTcpThread();

// ���� �Լ�
protected:

	// ���� ���� �Լ�
	virtual VOID		CloseSocket(SOCKET sd, UINT32 time_out=0);
	// ���� �Լ� ����
	virtual BOOL		StartWork();
	virtual VOID		EndWork();
	virtual BOOL		RecvData()	= 0;


// ��� ����
protected:

	BOOL				m_bIsInited;		// ��� ������ �ʱ�ȭ ����
	BOOL				m_bIsLinked;		// ���� (Connected) or ���� (Accepted)�Ǿ����� ����
	BOOL				m_bIsStopSend;		/* ���� �۽� ���� ��, ���� ó�� */

	UINT16				m_u16ThreadID;		// Zero-based

	// ũ�� ����: KBytes
	UINT32				m_u32PortBuff;		// ��Ʈ ���� ũ�� (��Ʈ �� �۽� or ���� �κ� ���� ũ��)
	UINT32				m_u32RecvBuff;		// ���� ���� ũ�� (�۽� or ���� ��Ŷ �� ���� ū ��Ŷ�� ũ��)
	UINT32				m_u32RingBuff;		// ���� ���� ũ�� (���ŵ� ��Ŷ�� ������ ��뷮 ����)
	INT32				m_i32SockError;		// ���� �ֱٿ� �߻��� ���� ����
	INT32				m_i32LastSendPktSize;

	// ���� �ð� (����: �и���)
	UINT32				m_u32SockTime;		// ���� �̺�Ʈ ���� ��� ���� �ð� (����: �и���)
	UINT64				m_u64WorkTime;		// ���� �ֱٿ� ����� �ð� ���� (����: �и���)

	UINT64				m_u64ConnectTime;

	SYSTEMTIME			m_stWorkTime;		// ���� �ֱٿ� ����� �ð� ���� (����: ����� �ú���)
	SYSTEMTIME			m_stLinkTime;		// ���� �ý��� ����� �ð� ���� (����: ����� �ú���)

	// Socket Information
	TCHAR				m_tzTCPInfo[128];
	TCHAR				m_tzSourceIPv4[IPv4_LENGTH];
	TCHAR				m_tzTargetIPv4[IPv4_LENGTH];
	UINT16				m_u16TcpPort;		// TCP Port
	SOCKET				m_sdClient;			// Client Socket Descriptor
	WSAEVENT			m_wsaClient;		// Client Socket Event
	/* ---------------------------------------------------------------------------- */
	/* ��Ŷ ���� �̺�Ʈ �߻��� ��, �б� �õ� Ƚ�� (������ �׳� �ʱ� �� 0 ���� ����) */
	/* ---------------------------------------------------------------------------- */
	UINT32				m_u32RecvRepeat;	// �б� �ݺ� �õ� Ƚ��
	UINT32				m_u32RecvSleep;		// �б� �ݺ� �õ� ���� ����ϴ� �ð� (����: �и���)

	// ���� �ֱٿ� ����� ����
	ENG_TPCS			m_enSocket;			// ���� �ֱ��� ���� ����
	ENG_EDIC			m_enSysID;			/* System ID */

	// ���� ����
	PUINT8				m_pPktRecvBuff;		// �ѹ� ���ŵ� ��, ����� �ӽ� ����
	CRingBuff			*m_pPktRingBuff;	// ������� ���ŵ� �����͸� ������ ��뷮 ����
	CPktQue				*m_pRecvQue;		// �ӽ� ��Ŷ ���� ���� ����� ���� ����


// ��� �Լ�
protected:

	// �ʱ�ȭ �Լ�
	BOOL				InitData();
	BOOL				InitMemory();

	/* ------------------------------ */
	/* ����� ������ �پ��� �ɼ� ���� */
	/* ------------------------------ */
	BOOL				SetSockOptBuffSize(UINT8 flag, SOCKET sd);
	BOOL				SetSockOptTimeout(UINT8 flag, SOCKET sd, INT32 time);
	BOOL				SetSockOptSpeed(INT32 optval, SOCKET sd);
	BOOL				SetSockOptAlive(INT32 optval, SOCKET sd);
	BOOL				SetSockOptRoute(INT32 optval, SOCKET sd);

	VOID				UpdateWorkTime();

	// ������ ���� ó��
	INT32				ReceiveAllData(SOCKET sd);

	BOOL				IsTcpWorkIdle(UINT32 time_gap);
	BOOL				IsTcpWorkConnect(UINT32 time_gap);
	VOID				SetForceClose();	// ������ ��� ����

	// ��Ŷ ������ �۽�/����/����
	BOOL				Sended(PUINT8 packet, UINT32 &size);
	BOOL				ReSended(PUINT8 packet,
								 UINT32 size,
								 UINT32 retry=TCP_SEND_TRY_COUNT,
								 UINT32 wait=TCP_SEND_TRY_TIMEOUT);
	BOOL				ReSended(PUINT8 packet,
								 UINT32 size,
								 UINT64 time_out=500,
								 UINT32 wait=TCP_SEND_TRY_TIMEOUT);
	BOOL				Received(SOCKET sd);

	// ���� ������ �ð����� ����� ������ �ֱ������� Alive Check ���� �۽�
	BOOL				SendAliveTime();

	VOID				SocketError(PTCHAR mesg);


// ���� �Լ�
public:


	BOOL				IsEmptyPktBuff();	// ���� ���ۿ� ���ŵ� �����Ͱ� �ִ��� ��ȸ

	/* ------------------------------------- */
	/* �ܼ��� ���� ����(����)�� ��� �� ��ȯ */
	/* ------------------------------------- */
	UINT16				GetThreadID()				{	return m_u16ThreadID;	}
	BOOL				IsConnected()				{	return m_bIsLinked;		}
	UINT16				GetPort()					{	return m_u16TcpPort;	}
	UINT64				GetWorkTcpTick()			{	return m_u64WorkTime;	}
	LPSYSTEMTIME		GetWorkTcpTime()			{	return &m_stWorkTime;	}
	LPSYSTEMTIME		GetLinkTime()				{	return &m_stLinkTime;	}
	INT32				GetSockLastError()			{	return m_i32SockError;	}

	PTCHAR				GetSourceIPv4()				{	return m_tzSourceIPv4;	}
	PTCHAR				GetTargetIPv4()				{	return m_tzTargetIPv4;	}
};