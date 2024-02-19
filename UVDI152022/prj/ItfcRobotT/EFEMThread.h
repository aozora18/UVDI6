
#pragma once

#include "../../inc/comm/ClientThread.h"
#include "../../inc/comn/MySection.h"

class CSendQue;

class CEFEMThread : public CClientThread
{
/* 생성자/파괴자 */
public:

	CEFEMThread(LPG_EDSM shmem, UINT16 th_id, LPG_CTCS efem);
	virtual ~CEFEMThread();

/* 가상 함수 */
protected:

	virtual BOOL		Connected();
	virtual VOID		DoWork();
	/* -------------------------------------------------------------------------- */
	/* 공통 통신 수신 모듈이 아니기 때문에, 별도로 가상함수를 통해 함수 별도 수행 */
	/* -------------------------------------------------------------------------- */
	virtual BOOL		RecvData();


/* 로컬 변수 */
protected:

	UINT8				m_u8SendData[8];	/* 수신에 대한 바로 송신할 때, 임시로 패킷 저장 */
#if 0
	UINT8				m_u8LastEfemState;	/* 가장 최근의 EFEM 상태 : Ready or Busy or Error */
	UINT8				m_u8LastWaferLoad;	/* 가장 최근의 Lot (Wafer)가 놓여진 위치 정보 (Combined) */
#endif
	UINT8				m_u8AliveCount;		/* Alive 송신 시도 횟수. 응답이 없으면 계속 증가됨 */

	SYSTEMTIME			m_tmAliveTime;		/* Alive 수신 받은 시간 저장 */

	BOOL				m_bSendRecv;		/* DI_TRUE - Sended, DI_FALSE - Recved */
	BOOL				m_bSetAreaMode;		/* 트리거 보드가 Area 모드 상태인지 여부 */
	BOOL				m_bSetFoupChange;	/* EFEM으로부터 Attach 이벤트를 받은 경우 */
	BOOL				m_bIsExposeMode;	/* 현재 노광 모드인지 여부 */

	UINT64				m_u64SendTime;		/* 데이터 전송 시간 (단위: 밀리초) */
	UINT64				m_u64PeriodTime;	/* 이전에 데이터 전송한 시간 저장 */
	UINT64				m_u64StateTime;		/* EFEM State 요청한 시간 저장 */
	UINT64				m_u64StateTick;		/* EFEM State 요청 주기 (단위: msec) */

	PUINT8				m_pPktRecvData;

	LPG_EDSM			m_pstShMemEFEM;		/* 공유 메모리 */

	CSendQue			*m_pSendQue;		/* 송신 큐 보내기 */
	CMySection			m_syncSend;			/* 송신 동기화 객체 */


/* 로컬 함수 */
protected:

	BOOL				InitSendData();
	BOOL				PktAnalysis(LPG_PSRB data);
	BOOL				PopSendData();
	BOOL				IsSendData();
	BOOL				ReqRefreshData(UINT16 cmd, PUINT8 data);
	BOOL				ReqEfemState();

	VOID				CloseSocket(SOCKET sd, UINT32 time_out=0);
	VOID				SetAliveData();
	VOID				SetRecvPacket(LPG_PSRB data);

	VOID				SaveErrorLogs(UINT16 code);

	VOID				UpdateLinkTime(UINT8 flag);


/* 공용 함수 */
public:

	BOOL				SendData(ENG_ECPC cmd, PUINT8 data=NULL, UINT16 size=0x0000);

	LPSYSTEMTIME		GetAliveTime()		{	return &m_tmAliveTime;	};

	VOID				SetExposeMode(BOOL enable);
	/* 처음 에러가 발생된 에러 코드 값 처리 */
	VOID				ResetErrorCode()			{	m_pstShMemEFEM->link.u16_error_code	= 0x0000;	}
	UINT16				GetErrorCode()				{	return m_pstShMemEFEM->link.u16_error_code;		}

	/* FOUP Lot List 요청 */
	BOOL				ReqFoupLotList();
};