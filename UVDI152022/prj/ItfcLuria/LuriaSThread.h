
#pragma once

#include "LuriaThread.h"

class CLuriaCThread;
/* The objects of user data family */
class CJobManagement;
class CExposure;
class CSystem;
class CComManagement;

class CLuriaSThread : public CLuriaThread
{
/* ������ & �ı��� */
public:

	CLuriaSThread(LPG_CTCS luria,
				  LPG_CLSI conf,
				  LPG_LDSM shmem,
				  CLuriaCThread *client,
				  CJobManagement *job,
				  CExposure *exposure,
				  CSystem *system,
				  CComManagement *commgt);
	virtual ~CLuriaSThread();

/* ���� �Լ� */
protected:

	virtual VOID		RunWork();


/* ���� ���� */
protected:

	UINT32				m_u32IdleTime;		// Client�κ��� �־��� �ð����� ������ ���� ���, Listen Socket�� ���ʱ�ȭ�ϴ� �ֱ� (����: �и���)
	UINT32				m_u32AnnounceCount;

	SOCKET				m_sdListen;			// Listen Socket Descriptor
	WSAEVENT			m_wsaListen;		// Listen Socket Event

	CLuriaCThread		*m_pClient;
	/* The objects of user data family */
	CJobManagement		*m_pPktJM;
	CExposure			*m_pPktEP;
	CSystem				*m_pPktSS;
	CComManagement		*m_pPktCM;


/* ���� �Լ� */
protected:

	BOOL				InitListen();		// ���� �ʱ�ȭ
	BOOL				PktAnalysis(LPG_PCLR data);

	UINT8				PopSockEvent();		// Socket Event ���� ����
	VOID				Accepted(SOCKET sd);
	VOID				CloseSocket(SOCKET sd, UINT32 time_out=0);

	BOOL				ReqSystemStatus();
	BOOL				ReqAnnouncementStatus();


/* ���� �Լ� */
public:

	VOID				CloseClient();		// ���� ����� Client Socket ���� ���� ó��
	VOID				CloseServer();		// ���� Server Socket ���� ���� ó��
};
