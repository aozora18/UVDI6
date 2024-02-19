
#pragma once

#include "../../inc/comn/ThinThread.h"

class CMeasure;

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

	UINT8				m_u8WorkStep;		/* 작업 순서 */
	UINT8				m_u8WorkTotal;
	UINT8				m_u8PhNext;			/* 현재 측정하고 있는 Phothead Number (1 ~ 8) */

	UINT16				m_u16RetryGrab;		/* Grabbed Image 캡처 횟수 */
	UINT16				m_u16StepYTotal;	/* 총 Y 축으로 이동하는 횟수 */
	UINT16				m_u16StepYNext;		/* 현재 측정 중이 Y 축 구간 인덱스 (1 ~ N) */
	UINT16				m_u16RepeatTotal;	/* 한번 측정할 때마다, 연속적으로 측정하는 반복 횟수 (1 ~ N) */
	UINT16				m_u16RepeatNext;	/* 현재 반복 측정하고 있는 횟수 (1 ~ N) */
	
	BOOL				m_bRunFocus;		/* Focus 측정 진행 여부 플래그 */

	UINT64				m_u64DelayTime;		/* 현재 작업이 완료되는데까지 대기하는 시간 (단위: msec) */
	UINT64				m_u64GrabTime;		/* 검색 시작한 시간 저장 */
	UINT64				m_u64WaitTime;
	UINT64				m_u64PeridPktTime;

	ENG_JWNS			m_enWorkState;		/* 0x00 : none, 0x01 : wait, 0x02 : next, 0x03 : complete, 0xff : error */

	CMySection			m_syncFocus;
	CMeasure			*m_pMeasure;		/* 측정 데이터가 임시 저장될 객체 */


/* 로컬 함수 */
protected:

	VOID				SetWorkWaitTime(UINT64 time);
	ENG_JWNS			IsWorkWaitTime();

	VOID				SendMesgNormal(UINT32 timeout=100);
	VOID				SendMesgResult(WPARAM result, UINT32 timeout=100);

	VOID				RunFocus();
	VOID				SetWorkNext();
	BOOL				IsWorkTimeOut();
	VOID				ReqPeriodPkt();

	ENG_JWNS			SetMeasureMoving();
	ENG_JWNS			IsMeasureMoved();

	ENG_JWNS			PutOneTrigger();
	ENG_JWNS			ProcFindModel();
	ENG_JWNS			ResultDetectImage();


/* 공용 함수 */
public:

	/* Focus 측정 관련 매개 변수 값 설정 */
	VOID				StartFocus(UINT16 step_s, UINT16 repeat, DOUBLE step_y);
	VOID				StopFocus();
	BOOL				IsCompleted();

	LPG_ZAAL			GetGrabData(UINT8 ph_no, UINT32 index);
	LPG_ZAAL			GetLastGrabData();
	CMeasure			*GetMeasure();

	/* Focus 측정 진행 여부 */
	BOOL				IsRunFocus()	{	return m_bRunFocus;	}

	/* 현재까지 작업 진행율 반환 */
	DOUBLE				GetWorkStepRate();
};