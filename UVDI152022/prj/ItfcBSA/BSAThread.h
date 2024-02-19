
#pragma once

#include "../../inc/comm/ClientThread.h"

class CSendQue;

class CBSAThread : public CClientThread
{
public:
	CBSAThread(LPG_CTCS vision, LPG_RBVC shmem);
	virtual ~CBSAThread();

/* 가상함수 */
protected:
	virtual BOOL		Connected();
	virtual VOID		DoWork();
	virtual BOOL		RecvData();

/* 로컬 구조체
protected:


/* 로컬 변수 */
protected:

	BOOL				m_bInitUpdate;		/* 트리거 보드에 연결되고 난 이후 초기 값 설정 여부  */
	BOOL				m_bNotiEvent;		/* Event 발생 여부 */

	UINT64				m_u64RecvTime;		/* 데이터 수신된 시간 저장 */
	UINT64				m_u64ReqTime;		/* 주기적으로 요청한 시간 (단위: 밀리초) */
	UINT64				m_u64SendTime;		/* 짧은 시간 내에 많은 데이터를 송신하지 못하도록 하기 위함 */
	UINT64				m_u64PeriodTime;	/* 이전에 데이터 전송한 시간 저장 */

	PUINT8				m_pPktRecvData;		/* 수신 받은 데이터 임시 저장 */
	PUINT8				m_pPktRecvFlag;		/* Packet 수신되었는지 여부 (0x00: None, 0x01 : Sended, 0x02 : Received) */
	PTCHAR				m_pEventMsg;		/* Evnet Noti Message 발생 여부 */
	PTCHAR				m_pLogMsg;

	LPG_RBVC			m_pstShMemVisi;		/* 공유 메모리 영역 (통신 수신 데이터 및 기타 데이터 임시 저장) */

	CMySection			m_syncSend;			/* 송신 동기화 객체 */
	CSendQue			*m_pSendQue;		/* 송신 큐 보내기 */

/* 로컬 함수 */
protected:

	BOOL				PktAnalysis(LPG_PSRB data);

	BOOL				PopSendData();
	BOOL				IsSendData();
	BOOL				IsPopEmptyData();

	VOID				CloseSocket(SOCKET sd, UINT32 time_out=0);

	VOID				SetSockData(UINT8 flag, LPG_PSRB data);
	VOID				SetSendData(LPG_PSRB data);
	VOID				SetRecvData(LPG_PSRB data);

	VOID				SetNotiMsg(PTCHAR mesg);
	VOID				SetNotiData(LPG_PSRB data);

	VOID				SetAlignGrab(PUINT8 data);


/* 공용 함수 */
public:

	BOOL				IsInitRecvData()	{ return m_bInitRecv; };
	BOOL				IsRecvData(ENG_VCPC cmd);

	BOOL				SendData(ENG_VCPC cmd, PUINT8 data = NULL, UINT16 size = 0x0000);
	BOOL				SendCenteringData(ENG_VCPC cmd, UINT8 cam_no, UINT8 unit);
	BOOL				SendFocusData(ENG_VCPC cmd, UINT8 cam_no, UINT8 unit);	
	BOOL				SendGrabData(ENG_VCPC cmd, UINT8 point, UINT8 mark, UINT8 pattern, INT32 pos_x, INT32 pos_y);
	BOOL				SendViewData(ENG_VCPC cmd, UINT8 mode);
	BOOL				SendIllumination(ENG_VCPC cmd, UINT8 mark, UINT8 onoff);
	BOOL				SendJoblist(ENG_VCPC cmd, UINT64 job_mark_1, UINT64 job_mark_2);

	BOOL				IsUpdateStateTime(UINT64 time);
	VOID				SetUpdateStateTime()			{	m_pstShMemVisi->update_state_time = GetTickCount64(); }

	BOOL				IsVisionNotiEvent()				{	return m_bNotiEvent;	};
	VOID				ResetVisionNotiEvent()			{	m_bNotiEvent = FALSE;	};
	PTCHAR				GetVisionNotiEventMsg()			{	return m_pEventMsg;		};
};



