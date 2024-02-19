
/* desc : Client TCP Thread */

#pragma once

#include "TcpThread.h"


class CClientThread : public CTcpThread
{
/* ������ & �ı��� */
public:

	CClientThread(ENG_EDIC sys_id,				/* System ID */
				  UINT16 th_id,					/* Thread ID */
				  UINT16 tcp_port,				// TCP/IP Port
				  UINT32 source_ip,				// Local IPv4
				  UINT32 target_ip,				// Remote IPv4
				  UINT32 port_buff=1024*8,		// TCP/IP Port Buffer Size (About 8 Kbytes)
				  UINT32 recv_buff=1024*1024,	// Packet ������ �ִ� ũ�� (����: Bytes. About 1 MBytes)
				  UINT32 ring_buff=1024*1024*2,	// Max Recv Packet Size (����: Bytes, About 1 MBytes)
				  UINT32 idle_time=0,			// �־��� �ð�(����: �и���)���� �ƹ��� ��� �۾��� ���� ���, Alive Message ����
												// 0 ������ �־��� ���, Alive Check �������� ����
				  UINT32 link_time=5*1000,		// �־��� �ð����� ������ �������ϴ� �� (����: �и���)
				  UINT32 sock_time=30,			// �⺻ 30 �и��� ���� ���� �̺�Ʈ ������ ���� ���
				  UINT32 live_time=2000);
	virtual ~CClientThread();

/* ���� �Լ� */
protected:

	/* ���� ���� �Լ� */
	virtual BOOL		Connected()	= 0;
	virtual VOID		DoWork() = 0;
	/* ���� �Լ� ���� */
	virtual VOID		RunWork();

/* ���� ���� */
protected:

	BOOL				m_bInitRecv;			/* �ʱ� 1�� ���ŵ� �����Ͱ� �����ϴ��� ����  */

	UINT32				m_u32LinkTime;			/* ������ �ֱ� (����: �и���) */
	UINT32				m_u32IdleTime;			/* ���� �ð� ��� ������ ���� ���, ��� ���� �ð� */
	UINT32				m_u32LiveTime;			/* Alive Check Time (����: �и���) */

/* ���� �Լ� */
protected:

	BOOL				IsLinkEvent();
	BOOL				DoConnect();
	BOOL				BindSock();
	VOID				Connected(SOCKET sd);	/* ����� ��, �ʱ� �ѹ� ȣ��� */
	VOID				CloseSocket(SOCKET sd, UINT32 time_out=0);
	UINT8				PopSockEvent();			/* Socket Event ���� ��� */


/* ���� �Լ� */
public:

	/* �������� ���� ������ ���� */
	VOID				CloseClient()			{	CloseSocket(m_sdClient);	}
};