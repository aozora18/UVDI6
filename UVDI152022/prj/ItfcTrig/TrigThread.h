
#pragma once

#include "../../inc/comm/ClientThread.h"

#define TRIG_RECV_OUTPUT	0	/* Trigger Board로부터 수신된 데이터 출력 */

class CSendQue;

class CTrigThread : public CClientThread
{
/* 생성자/파괴자 */
public:

	CTrigThread(LPG_CTCS trig, LPG_CTSP conf, LPG_TPQR shmem,
				LPG_DLSM link, LPG_CCGS comn);
	virtual ~CTrigThread();

/* 가상함수 */
protected:
	virtual BOOL		Connected();
	virtual VOID		DoWork();
	virtual BOOL		RecvData();


/* 로컬 구조체 */
protected:


/* 로컬 변수 */
protected:

	BOOL				m_bInitRecv;			/* 초기 1번 수신된 데이터가 존재하는지 여부  */
	BOOL				m_bInitUpdate;			/* 트리거 보드에 연결되고 난 이후 초기 값 설정 여부  */

	/* 얼라인 카메라의 트리거 출력 위치 저장 (가장 최근 값. 덮어씀) */
	UINT32				m_u32SendTrigPos[MAX_TRIG_CHANNEL][MAX_REGIST_TRIG_POS_COUNT];	/* (단위: 100 nm or 0.1 um) */

	UINT64				m_u64RecvTime;			/* 데이터 수신된 시간 저장 */
	UINT64				m_u64SendTime;			/* 짧은 시간 내에 많은 데이터를 송신하지 못하도록 하기 위함 */
#if (TRIG_RECV_OUTPUT)
	UINT64				m_u64TraceTime;
#endif

	LPG_CTSP			m_pstConfTrig;			/* Trigger Configuration */
	LPG_TPQR			m_pstShMemTrig;			/* Trigger Shared Memory */
	LPG_DLSM			m_pstShMemLink;			/* 통신 연결 상태 */
	LPG_CCGS			m_pstConfComn;			/* 전체 공통 환경 정보 */

	CMySection			m_syncSend;				/* 송신 동기화 객체 */
	CSendQue			*m_pSendQue;			/* 송신 큐 보내기 */


/* 로컬 함수 */
protected:

	VOID				CloseSocket(SOCKET sd, UINT32 time_out=0);

	VOID				UpdateLinkTime(UINT8 flag);

	VOID				PopSendData();
	BOOL				IsPopEmptyData();

	VOID				SetRecvPacket(ENG_TPCS type, PUINT8 data, UINT32 size);
	VOID				SetTrigEvent(BOOL direct, LPG_TPTS trig_set);
	VOID				SetTrigPos(LPG_TPTS trig_set,
								   UINT8 start1, UINT8 count1, PUINT32 pos1,
								   UINT8 start2, UINT8 count2, PUINT32 pos2);


/* 공용 함수 */
public:

	UINT8				GetTrigChNo(UINT8 cam_id);

	UINT16				GetQueCount();
	BOOL				ReqInitUpdate();
	BOOL				ReqEEPROMCtrl(UINT8 mode);
	BOOL				ReqReadSetup();
	BOOL				ResetTrigPosAll();
	BOOL				ReqWriteBoardReset();
	BOOL				ReqWriteIP4Addr(PUINT8 ip_addr);
	BOOL				ReqWriteEncoderOut(UINT32 enc_out);
	BOOL				ReqWriteTrigOutOne(UINT32 enc_out);
	BOOL				ReqWriteTriggerStrobe(BOOL enable);
	BOOL				ReqWriteAreaTrigPos(BOOL direct,
											UINT8 start1, UINT8 count1, PUINT32 pos1,
											UINT8 start2, UINT8 count2, PUINT32 pos2,
											ENG_TEED enable, BOOL clear);
	BOOL				ReqWriteAreaTrigPosCh(UINT8 cam_id, UINT8 start, UINT8 count, PUINT32 pos,
											  ENG_TEED enable, BOOL clear);
	BOOL				ReqResetTrigCount();

	BOOL				IsInitRecvData()	{	return m_bInitRecv;	};
	BOOL				IsTrigPosEqual(UINT8 cam_id, UINT8 index, UINT32 pos);
};