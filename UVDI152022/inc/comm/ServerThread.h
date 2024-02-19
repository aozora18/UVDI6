
#pragma once

#include "TcpThread.h"


class CServerThread : public CTcpThread
{
// ������ & �ı���
public:

	CServerThread(ENG_EDIC sys_id,
				  UINT16 th_id,					// Thread ID
				  UINT16 tcp_port,				// TCP/IP Port
				  UINT32 source_ip,				// Local IPv4
				  UINT32 target_ip,				// Remote IPv4
				  UINT32 port_buff=1024*8,		// TCP/IP Port Buffer Size (About 8 Kbytes)
				  UINT32 recv_buff=1024*1024,	// Packet ������ �ִ� ũ�� (����: Bytes. About 1 MBytes)
				  UINT32 ring_buff=1024*1024*2,	// Max Recv Packet Size (����: Bytes, About 1 MBytes)
				  UINT32 idle_time=10,			// �־��� �ð� ���� ������ ���� ���, ���� ���� ���ʱ�ȭ
				  UINT32 sock_time=10);			// �⺻ 10 �и��� ���� ���� �̺�Ʈ ������ ���� ��� (����: �и���)
	virtual ~CServerThread();

// ���� �Լ�
protected:
	// ���� ���� �Լ�
	virtual BOOL		Accepted() = 0;
	virtual VOID		DoWork() = 0;
	// ���� �Լ� ����
	virtual VOID		RunWork();


// ���� ����
protected:

	UINT64				m_u64IdleTime;		// Client�κ��� �־��� �ð����� ������ ���� ���, Listen Socket�� ���ʱ�ȭ�ϴ� �ֱ� (����: �и���)

	SOCKET				m_sdListen;			// Listen Socket Descriptor
	WSAEVENT			m_wsaListen;		// Listen Socket Event

// ���� �Լ�
protected:

	BOOL				InitListen();		// ���� �ʱ�ȭ
	UINT8				PopSockEvent();		// Socket Event ���� ����
	VOID				Accepted(SOCKET sd);
	VOID				CloseSocket(SOCKET sd, UINT32 time_out=0);

// ���� �Լ�
public:

	VOID				CloseClient();		// ���� ����� Client Socket ���� ���� ó��
	VOID				CloseServer();		// ���� Server Socket ���� ���� ó��
};
