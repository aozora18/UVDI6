
#pragma once

#include "../../inc/comn/ThinThread.h"
#include "Mvsenc.h"
#include "mvsencenum.h"
#define TRIG_RECV_OUTPUT	0	/* Trigger Board로부터 수신된 데이터 출력 */

class CSendQue;

class CMvencThread : public CThinThread
{
/* 생성자/파괴자 */
public:

	CMvencThread(BYTE byPort, LPG_CTSP conf, LPG_TPQR shmem,
				LPG_DLSM link, LPG_CCGS comn);
	virtual ~CMvencThread();

	// 가상함수 재정의
protected:
	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();

/* 로컬 구조체 */
protected:


/* 로컬 변수 */
protected:
	HANDLE					m_handle; 
	BYTE					m_byPort;
	MVS_ENC_HT_TRGCONTROL	m_stTrigCtrl[MAX_TRIG_CHANNEL];

	BOOL					m_bInitRecv;

	/* 얼라인 카메라의 트리거 출력 위치 저장 (가장 최근 값. 덮어씀) */
	UINT32					m_u32SendTrigPos[MAX_TRIG_CHANNEL][MAX_REGIST_TRIG_POS_COUNT];	/* (단위: 100 nm or 0.1 um) */
	UINT64					m_u64SendTime;			/* 짧은 시간 내에 많은 데이터를 송신하지 못하도록 하기 위함 */
#if (TRIG_RECV_OUTPUT)
	UINT64					m_u64TraceTime;
#endif

	LPG_CTSP				m_pstConfTrig;			/* Trigger Configuration */
	LPG_TPQR				m_pstShMemTrig;			/* Trigger Shared Memory */
	LPG_DLSM				m_pstShMemLink;			/* 통신 연결 상태 */
	LPG_CCGS				m_pstConfComn;			/* 전체 공통 환경 정보 */

	CMySection				m_syncSend;				/* 송신 동기화 객체 */


/* 로컬 함수 */
protected:
	VOID				SetTrigEvent(BOOL direct, LPG_TPTS trig_set);
	VOID				SetTrigPos(LPG_TPTS trig_set,
								   UINT8 start1, UINT8 count1, PINT32 pos1,
								   UINT8 start2, UINT8 count2, PINT32 pos2);

/* 공용 함수 */
public:
	BOOL				IsConnected();
	void				CloseHandle();

	UINT8				GetTrigChNo(UINT8 cam_id);
	BOOL				ReqInitUpdate();
	BOOL				ReqEEPROMCtrl(UINT8 mode);
	BOOL				ReqReadSetup();
	BOOL				ResetTrigPosAll();
	BOOL				ReqWriteBoardReset();
	BOOL				ReqWriteEncoderOut(UINT32 enc_out);
	BOOL				ReqWriteTrigOutOne(UINT32 enc_out);
	BOOL				ReqWriteTrigOutOne_(UINT32 channelBit, int trigTime=20);

	BOOL				ReqWriteTriggerStrobe(BOOL enable);
	BOOL				ReqWriteAreaTrigPos(BOOL direct,
											UINT8 start1, UINT8 count1, PINT32 pos1,
											UINT8 start2, UINT8 count2, PINT32 pos2,
											ENG_TEED enable, BOOL clear);
	BOOL				ReqWriteAreaTrigPosCh(UINT8 cam_id, UINT8 start, UINT8 count, PINT32 pos,
											  ENG_TEED enable, BOOL clear);
	BOOL				ReqResetTrigCount();
	BOOL				IsInitRecvData() { return m_bInitRecv; };
	BOOL				IsTrigPosEqual(UINT8 cam_id, UINT8 index, UINT32 pos);

	/*UVDI15*/
	BOOL				ReqWirteTrigLive(UINT8 cam_id, BOOL enable);
	BOOL				SetLiveMode();
	BOOL				SetPositionTrigMode(ENG_TEED enable);
};