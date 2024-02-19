
#pragma once

#include "../../inc/comn/ThinThread.h"

#define MOTION_CALI_CENTERING_MIN	0.0002f	/* 단위: mm. 0.2 um */

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

	UINT8				m_u8RunMode;		/* 0 : 카메라 회전 각도 값 측정, 1 : 카메라 픽셀 크기 값 측정 */
	UINT8				m_u8ACamID;			/* 1 or Later */
	UINT8				m_u8WorkStep;		/* 현재 작업 개수 */
	UINT8				m_u8WorkTotal;		/* 전체 작업 개수 */
	UINT8				m_u8GrabbedCount;	/* 각도 측정 횟수 증가 */

	UINT32				m_u32RepeatTotal;	/* 총 반복될 횟수 */
	UINT32				m_u32RepeatCount;	/* 현재 반복 측정 횟수 */

	BOOL				m_bRunSpec;			/* Focus 측정 진행 여부 플래그 */

	DOUBLE				m_dbDiffCamX;		/* Mark 이미지의 중심에서 떨어진 Align Camera 오차 (unit: mm) */
	DOUBLE				m_dbDiffStageY;		/* Mark 이미지의 중심에서 떨어진 Stage Y 오차 (unit: mm) */

	UINT64				m_u64TickGrabbed;	/* 검색 시작한 시간 저장 */
	UINT64				m_u64DelayTime;		/* 매 단계 처리 시점마다 시작 시간 저장 */
	UINT64				m_u64PeridPktTime;

	DOUBLE				m_dbRateLast;		/* 가장 최근 진행률 값 임시 저장 */

	ENG_JWNS			m_enWorkState;		/* 0x00 : none, 0x01 : wait, 0x02 : next, 0x03 : complete, 0xff : error */

	/* Align Camera 1/2 번이 좌/우 이동한 픽셀 좌표 값 */
	STG_DBXY			m_stGrabCentXY[2];	/* 각 카메라 별로 2 개의 Grabbed Image의 Mark 중심 좌표가 저장될 구조체 */
	STG_DBXY			m_stGrabMoveXY[2];	/* 각 카메라 별로 2 개의 Grabbed Image의 Mark 이동 좌표가 저장될 구조체 */

	CMySection			m_syncSpec;


/* 로컬 함수 */
protected:

	VOID				SendMesgNormal(UINT32 timeout=100);
	VOID				SendMesgResult(BOOL flag, UINT32 timeout=100);
	VOID				SendMesgGrabView(UINT32 timeout=100);

	VOID				RunSpec();
	VOID				SetWorkNext();
	BOOL				IsWorkTimeOut();
	VOID				ReqPeriodPkt();

	ENG_JWNS			SetACamMovingSide();
	ENG_JWNS			IsACamMovedSide();

	ENG_JWNS			SetACamMovingZAxisQuartz();
	ENG_JWNS			IsACamMovedZAxisQuartz();

	ENG_JWNS			SetACamMovingQuartz();
	ENG_JWNS			IsACamMovedQuartz();

	ENG_JWNS			SetTrigEnabled();
	ENG_JWNS			IsTrigEnabled();

	ENG_JWNS			GetGrabQuartzData();
	ENG_JWNS			IsGrabbQuartzData();

	ENG_JWNS			SetMotionMoving();
	ENG_JWNS			IsMotionMoved();

	ENG_JWNS			SetMotionMovingX();
	ENG_JWNS			IsMotionMovedX();

	ENG_JWNS			IsGrabbQuartzMoveX();

	VOID				CalcACamSpec(BOOL flag);


/* 공용 함수 */
public:

	/* Focus 측정 관련 매개 변수 값 설정 */
	VOID				StartSpec(UINT8 run_mode, UINT8 cam_id, UINT32 repeat);
	VOID				StopSpec();
	BOOL				IsCompleted();
	BOOL				IsRunSpec()		{	return m_bRunSpec;	}

	DOUBLE				GetStepRate();

	/* 현재까지 측정된 정보 반환 */
	UINT32				GetGrabCount();

	DOUBLE				GetMinMax(UINT8 mode, UINT8 type);

	UINT32				GetMeasureCount()	{	return m_u32RepeatCount+1;	}
};