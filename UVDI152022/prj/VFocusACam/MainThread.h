
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

	UINT8				m_u8ACamID;			/* 1 or Later */
	UINT8				m_u8WorkStep;		/* 작업 순서 */
	UINT8				m_u8WorkTotal;
	UINT8				m_u8RetryTrig;		/* 트리거 발생 재시도 횟수 */
	
	BOOL				m_bRunFocus;		/* Focus 측정 진행 여부 플래그 */

	UINT32				m_u32SkipCount;		/* 이동 후 LED가 안정화 될 때까지 테스트로 측정하는 횟수 (분석 값에 포함 안됨) */
	UINT32				m_u32RepCount;		/* 이동 및 안정화 후부터 정상적으로 측정이 반복되는 횟수 (평균 값을 구하기 위함) */
	UINT32				m_u32TotCount;		/* 총 이동 횟수 즉, Step (Up/Down)이 몇 번 이동해야할지 횟수 값 */
	
	UINT32				m_u32MeasRep;		/* 현재 반복 측정 횟수 */
	UINT32				m_u32MeasTot;		/* 현재 Step (Up/Down)이 몇 번 이동 했는지 횟수 */
	UINT32				m_u32MeasSkip;		/* 안정화 상태 횟수 즉, Skip 진행된 횟수 */

	UINT64				m_u64GrabTime;		/* 검색 시작한 시간 저장 */
	UINT64				m_u64DelayTime;		/* 매 단계 처리 시점마다 시작 시간 저장 */

	DOUBLE				m_dbInitPosZ;		/* Z Axis 측정 최초 시작 위치 (단위: mm) */
	DOUBLE				m_dbStepSize;		/* 이동 간격 (unit: 100 nm or 0.1 um) */
	DOUBLE				m_dbMeasPosZ;		/* 다음에 이동해야 할 위치 */

	ENG_MDMD			m_enUpDown;			/* 상/하 이동 방향 값 (1:Up or 2:Down) */
	ENG_JWNS			m_enWorkState;		/* 0x00 : none, 0x01 : wait, 0x02 : next, 0x03 : complete, 0xff : error */

	CMySection			m_syncFocus;
	CMeasure			*m_pMeasure;		/* 측정 데이터가 임시 저장될 객체 */


/* 로컬 함수 */
protected:

	VOID				SendMesgNormal(UINT32 timeout=100);
	VOID				SendMesgResult(UINT32 timeout=100);

	VOID				RunFocus();
	VOID				SetWorkNext();
	BOOL				IsWorkTimeOut();

	ENG_JWNS			ACamZHoming();
	ENG_JWNS			IsACamZHomed();
	ENG_JWNS			PutOneTrigger();
	ENG_JWNS			ProcDetectImage();
	ENG_JWNS			ResultDetectImage();
	ENG_JWNS			SetMoveStep();
	ENG_JWNS			IsMovedStep();


/* 공용 함수 */
public:

	/* Focus 측정 관련 매개 변수 값 설정 */
	VOID				StartFocus(UINT8 cam_id,
								   DOUBLE step,
								   UINT32 skip,
								   UINT32 repeat,
								   UINT32 total);
	VOID				StopFocus();
	BOOL				IsCompleted();

	/* 현재까지 측정된 정보 반환 */
	UINT32				GetGrabCount();
	LPG_ZAAL			GetGrabData(UINT32 index);
	LPG_ZAAL			GetGrabSel();

	CMeasure			*GetMeasure();
	DOUBLE				GetMinMax(UINT8 mode, UINT8 type);

	/* Focus 측정 진행 여부 */
	BOOL				IsRunFocus()	{	return m_bRunFocus;	}

	/* 현재까지 작업 진행율 반환 */
	DOUBLE				GetWorkStepRate();
};