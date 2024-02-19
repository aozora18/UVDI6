
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
/* 생성자 & 파괴자 */
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

/* 가상 함수 */
protected:

	virtual VOID		RunWork();


/* 로컬 변수 */
protected:

	UINT32				m_u32IdleTime;		// Client로부터 주어진 시간동안 응답이 없는 경우, Listen Socket을 재초기화하는 주기 (단위: 밀리초)
	UINT32				m_u32AnnounceCount;

	SOCKET				m_sdListen;			// Listen Socket Descriptor
	WSAEVENT			m_wsaListen;		// Listen Socket Event

	CLuriaCThread		*m_pClient;
	/* The objects of user data family */
	CJobManagement		*m_pPktJM;
	CExposure			*m_pPktEP;
	CSystem				*m_pPktSS;
	CComManagement		*m_pPktCM;


/* 로컬 함수 */
protected:

	BOOL				InitListen();		// 소켓 초기화
	BOOL				PktAnalysis(LPG_PCLR data);

	UINT8				PopSockEvent();		// Socket Event 정보 추출
	VOID				Accepted(SOCKET sd);
	VOID				CloseSocket(SOCKET sd, UINT32 time_out=0);

	BOOL				ReqSystemStatus();
	BOOL				ReqAnnouncementStatus();


/* 공용 함수 */
public:

	VOID				CloseClient();		// 현재 연결된 Client Socket 연결 해제 처리
	VOID				CloseServer();		// 현재 Server Socket 연결 해제 처리
};
