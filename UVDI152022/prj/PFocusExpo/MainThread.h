
#pragma once

#include "../../inc/comn/ThinThread.h"

#define MOTION_CALI_CENTERING_MIN	2	/* 단위: 0.1 um or 100 nm, ex> 2 --> 0.2 um */

class CMainThread : public CThinThread
{
/* 생성자 / 파괴자 */
public:
	CMainThread(HWND parent);
	virtual ~CMainThread();

/* 가상 함수 재정의 */
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();

/* 로컬 변수 */
protected:

	UINT8				m_u8WorkStep;			/* 현재 작업 개수 */
	UINT8				m_u8WorkTotal;			/* 전체 작업 개수 */
	UINT16				m_u16PrintCount;		/* 노광 횟수 증가 */
	UINT16				m_u16PrintTotal;		/* 노광 전체 횟수 */

	BOOL				m_bReqPeriodMC2;
	BOOL				m_bRunPrint;			/* 노광 측정 진행 여부 플래그 */

	UINT64				m_u64PeridPktTime;

	DOUBLE				m_dbExpoX;				/* 노광 시작 위치 (모션 위치) (단위: um) */
	DOUBLE				m_dbExpoY;				/* 노광 시작 위치 (모션 위치) (단위: um) */
	DOUBLE				m_dbPeriodY;			/* 매번 노광 때마다 Y 축 스테이지 이동 거리 (unit: um) */
	DOUBLE				m_dbPeriodZ;			/* 매번 노광 때마다 PH Z 축 Up/Down 이동 거리 (unit: um) */

	DOUBLE				m_dbPhBaseZ[MAX_PH];	/* 모든 Photohead Z 축의 기본 시작 위치 (단위: um) (range: 63000 ~ 85000) */

	UINT64				m_u64DelayTime;			/* 매 단계 처리 시점마다 시작 시간 저장 */
	UINT64				m_u64WaitTime;			/* 이전 작업이 동작 완료될 때까지 최소한의 대기 시간 값 (단위: ms) */
	UINT64				m_u64SendTime;			/* 주기적으로 명령어 전송하는데, 가장 최근에 명령어 전송한 시간 저장 (단위: ms) */
	UINT64				m_u64StepTime;			/* 특정 시간 동안 해당 명령어가 정상적으로 동작되지 않는다면, 이 시간 기준으로 다시 해당 명령을 실행하기 위함 */

	ENG_JWNS			m_enWorkState;			/* 0x00 : none, 0x01 : wait, 0x02 : next, 0x03 : complete, 0xff : error */

	CMySection			m_syncPrint;


/* 로컬 함수 */
protected:

	VOID				SendMesgNormal(UINT32 timeout=100);
	VOID				SendMesgPrint(BOOL flag, UINT32 timeout=100);

	VOID				RunPrint();
	VOID				SetWorkNext();
	BOOL				IsWorkTimeOut();
	BOOL				IsPhZAxisMoved(UINT8 ph_no, DOUBLE pos);

	VOID				ReqPeriodPkt();

	VOID				SetWorkWaitTime(UINT64 time);
	ENG_JWNS			IsWorkWaitTime();

	ENG_JWNS			SetExpoStartXY();
	ENG_JWNS			IsExpoStartXY();

	ENG_JWNS			SetPhZAxisHoming();
	ENG_JWNS			IsPhZAxisHoming();
	ENG_JWNS			IsPhZAxisHomed();

	ENG_JWNS			SetPhZAxisMoving();
	ENG_JWNS			IsPhZAxisMoved();

	ENG_JWNS			SetExpoReady();
	ENG_JWNS			IsExpoReady();

	ENG_JWNS			SetExpoPrinting();
	ENG_JWNS			IsExpoPrinted();


/* 공용 함수 */
public:

	/* Focus 측정 관련 매개 변수 값 설정 */
	VOID				StartPrint(DOUBLE expo_x, DOUBLE expo_y,
								   DOUBLE period_z, DOUBLE period_y, UINT16 repeat);
	VOID				StopPrint();
	BOOL				IsCompleted();
	BOOL				IsRunPrint()		{	return m_bRunPrint;	}

	UINT16				GetStepCount();
	UINT16				GetPrintCount()		{	return m_u16PrintCount;	}
	UINT16				GetPrintTotal()		{	return m_u16PrintTotal;	}
	UINT16				GetPrintRest()		{	return m_u16PrintTotal - m_u16PrintCount;	}
	UINT8				GetStepTotal()		{	return m_u8WorkTotal;	}
};