
#pragma once

#include "../../inc/comn/ThinThread.h"
#include "../../inc/comm/SerialComm.h"

class CEFUThread : public CThinThread
{
/* 생성자 & 파괴자 */
public:

	CEFUThread(UINT8 port, UINT32 baud, UINT32 buff, UINT8 efu_cnt, LPG_EPAS shmem);
	virtual ~ CEFUThread();

/* 가상함수 재선언 */
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();

public:



/* 로컬 변수 */
protected:

	/* 시리얼 통신 포트 (1 ~ 255) */
	UINT8				m_u8Port;
	UINT8				m_u8EFUCount;

	BOOL				m_bIsPktRecv;		/* Packet 송신에 대한 응답 여부 */
	BOOL				m_bAutoStateGet;	/* 주기적으로 EFU 상태 요청인지 여부 */

	UINT64				m_u64SendPktTime;	/* 통신 시간 저장 (송신)*/
	UINT64				m_u64AckTimeWait;	/* 통신 송신 후 응답이 올 때까지 최대 대기하는 시간 */

	STG_EBSS			m_stSpeed;			/* 가장 최근에 설정한 속도 정보 */

	PUINT8				m_pRecvData;		/* 수신데이터 임시 저장 */
	LPG_EPAS			m_pstState;			/* Shared Memory (EFU 상태 정보) */

	CMySection			m_syncComm;
	CSerialComm			*m_pComm;			/* 시리얼 통신 객체 */


/* 로컬 함수 */
protected:

	/* 통신 데이터 전송 */
	BOOL				SendData(PUINT8 cmd, UINT32 size);

	/* 수신 데이터 반환 */
	BOOL				PopCommData(PUINT8 data, UINT32 &size);

	VOID				ParseAnalysis();
	VOID				SetRecvEnable();
	VOID				CommClosed();	/* 시리얼 통신 해제 */

	VOID				SaveError(UINT8 type, PUINT8 buff);


/* 공용 함수 */
public:

	VOID				Stop();

	VOID				SetAckTimeout(UINT64 wait);
	VOID				SetAutoEnable(BOOL enable)	{ m_bAutoStateGet = enable;	}
	BOOL				IsGetAutoState()			{	return m_bAutoStateGet;	}

	UINT8				GetPort()		{	return m_u8Port;	}

	/* 시리얼 통신 포트 환경 설정 및 얻기 */
	BOOL				GetConfigPort(STG_SSCI &data);
	BOOL				SetConfigPort(LPG_SSCI data);

	BOOL				IsOpenPort();
	BOOL				IsSendIdle()	{	return m_bIsPktRecv;	}

	BOOL				ReqSetSpeed(UINT8 bl_id, UINT8 speed);
	BOOL				ReqGetState();

	ENG_SCSC			GetCommStatus();	/* 현재 통신 상태 반환 */
	VOID				ResetCommData();	/* 기존 송신 및 수신 관련 통신 데이터 초기화 */

	LPG_EBSS			GetSpeedValue()	{	return &m_stSpeed;	}
};