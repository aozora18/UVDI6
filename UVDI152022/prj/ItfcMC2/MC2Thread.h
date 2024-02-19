
/* ----------------------- */
/* desc : ��� �⺻ ������ */
/* ----------------------- */

#pragma once

#ifndef IPv4_LENGTH
#define LEN_IPV4	16
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


#define MAX_WANTED_READ_SIZE	160	/* head (32) + drive_data (16) * drive_count (8) */
#define	PACKET_FAIL_PERIOD_TIME	300	/* ��Ŷ ��� ���� ���� �޽��� ����� ���޵Ǵ� ���� ���� ���� */

#include <winsock.h>
#include <WS2tcpip.h>

#include "../../inc/comn/ThinThread.h"
#include "../../inc/comm/RingBuff.h"

class CMC2Thread : public CThinThread
{
// ������ & �İ���
public:
	CMC2Thread(LPG_MDSM shmem,
			   UINT16 mc2_port,
			   UINT16 cmps_port,
			   UINT32 source_ip,				// Local IPv4
			   UINT32 target_ip,				// Remote IPv4
			   UINT32 port_buff	= 1024*8,		// TCP/IP Port Buffer Size (����: KBytes, About 8 KBytes)
			   UINT32 recv_buff	= 1024*1024*1,	// Packet ������ �ִ� ũ�� (����: Bytes. About 1 MBytes)
			   UINT32 ring_buff	= 1024*1024*2,	// Max Recv Packet Size (����: Bytes, About 2 MBytes)
			   UINT32 sock_time	= 30);			// ���� : �и���
	virtual ~CMC2Thread();

// ���� �Լ�
protected:

	// ���� ���� �Լ�
	virtual VOID		CloseSocket(SOCKET sd, UINT32 time_out=0) = 0;
	// ���� �Լ� ����
	virtual BOOL		StartWork();
	virtual VOID		EndWork();


// ��� ����
protected:

	BOOL				m_bIsInited;		// ��� ������ �ʱ�ȭ ����
	BOOL				m_bConnected;		// ���� ����

	ENG_EDIC			m_enSysID;

	UINT8				m_u8DncError;
	UINT16				m_u16DrvError[MAX_MC2_DRIVE];

	UINT16				m_u16ThreadID;		// Zero-based

	// ũ�� ����: KBytes
	UINT32				m_u32PortBuff;		// ��Ʈ ���� ũ�� (��Ʈ �� �۽� or ���� �κ� ���� ũ��)
	UINT32				m_u32RecvBuff;		// ���� ���� ũ�� (�۽� or ���� ��Ŷ �� ���� ū ��Ŷ�� ũ��)
	UINT32				m_u32RingBuff;		// ���� ���� ũ�� (���ŵ� ��Ŷ�� ������ ��뷮 ����)
	INT32				m_i32SockError;		// ���� �ֱٿ� �߻��� ���� ����

	// ���� �ð� (����: �и���)
	UINT32				m_u32SockTime;		// ���� �̺�Ʈ ���� ��� ���� �ð� (����: �и���)
	UINT64				m_u64WorkTime;		// ���� �ֱٿ� ����� �ð� ���� (����: �и���)
	UINT64				m_u64SendTime;		/* ���� �ֱٿ� �۽��� �ð� ���� (����: �и���) */

	UINT64				m_u64TCPFailTime;	/* ��Ŷ �۽� ���и� �θ𿡰� �˸� �ð� ���� */

	SYSTEMTIME			m_stWorkTime;		// ���� �ֱٿ� ����� �ð� ���� (����: ����� �ú���)
	SYSTEMTIME			m_stLinkTime;		// ���� �ý��� ����� �ð� ���� (����: ����� �ú���)

	// Socket Information
	TCHAR				m_tzSourceIPv4[IPv4_LENGTH];
	TCHAR				m_tzTargetIPv4[IPv4_LENGTH];
	UINT16				m_u16MC2Port;		// TCP Port
	UINT16				m_u16CMPSPort;

	UINT32				m_u32AddrSize;
	SOCKET				m_sdClient;			// Client Socket Descriptor
	SOCKADDR_IN			m_stSrvAddr;
	WSAEVENT			m_wsaClient;		// Client Socket Event

	/* ---------------------------------------------------------------------------- */
	/* ��Ŷ ���� �̺�Ʈ �߻��� ��, �б� �õ� Ƚ�� (������ �׳� �ʱ� �� 0 ���� ����) */
	/* ---------------------------------------------------------------------------- */
	UINT32				m_u32RecvRepeat;	// �б� �ݺ� �õ� Ƚ��
	UINT32				m_u32RecvSleep;		// �б� �ݺ� �õ� ���� ����ϴ� �ð� (����: �и���)

	/* MC2 Service ���� �޸� (Actual / Reference �ǽð� ���� ����) */
	LPG_MDSM			m_pstShMemMC2;

	// ���� �ֱٿ� ����� ����
	ENG_TPCS			m_enSocket;			// ���� �ֱ��� ���� ����

	// ���� ����
	PUINT8				m_pPktRecvBuff;		// �ѹ� ���ŵ� ��, ����� �ӽ� ����
	CRingBuff			*m_pPktRingBuff;	// ������� ���ŵ� �����͸� ������ ��뷮 ����

	/* �۽� ���� ���� �ӽ� ���� */
	LPG_MPSD			m_pstPktSend;

	CMySection			m_syncPktSend;		// ��Ŷ ������ �۽� ����ȭ ��ü
	CMySection			m_syncPktRecv;		// ��Ŷ ������ �۽� ����ȭ ��ü


// ��� �Լ�
protected:

	// �ʱ�ȭ �Լ�
	BOOL				InitData();
	BOOL				InitMemory();

	BOOL				RecvData();

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

	VOID				SetForceClose();	// ������ ��� ����

	// ��Ŷ ������ �۽�/����/����
	BOOL				Sended(PUINT8 packet, UINT32 &size);
	BOOL				ReSended(PUINT8 packet,
								 UINT32 size,
								 UINT32 retry=TCP_SEND_TRY_COUNT,
								 UINT32 wait=TCP_SEND_TRY_TIMEOUT);
	BOOL				Received(SOCKET sd);

	VOID				SetRecvPacket(ENG_TPCS type, LPG_MPRD data);
	VOID				SetRecvData(UINT32 obj_id, PUINT8 data, UINT32 size);
	VOID				AnalysisError();


// ���� �Լ�
public:

	UINT8				GetDNCError()				{	return m_u8DncError;		}
	VOID				ResetError();

	VOID				Closed(SOCKET sd);
	BOOL				IsEmptyPktBuff();	// ���� ���ۿ� ���ŵ� �����Ͱ� �ִ��� ��ȸ
	BOOL				IsConnected()				{	return m_bConnected;		}

	/* ------------------------------------- */
	/* �ܼ��� ���� ����(����)�� ��� �� ��ȯ */
	/* ------------------------------------- */
	UINT16				GetThreadID()				{	return m_u16ThreadID;		}
	UINT16				GetMC2Port()				{	return m_u16MC2Port;		}
	UINT16				GetCMPSPort()				{	return m_u16CMPSPort;		}
	UINT64				GetWorkTcpTick()			{	return m_u64WorkTime;		}
	LPSYSTEMTIME		GetWorkTcpTime()			{	return &m_stWorkTime;		}
	INT32				GetSockLastError()			{	return m_i32SockError;		}

	PTCHAR				GetSourceIPv4()				{	return m_tzSourceIPv4;		}
	PTCHAR				GetTargetIPv4()				{	return m_tzTargetIPv4;		}
};