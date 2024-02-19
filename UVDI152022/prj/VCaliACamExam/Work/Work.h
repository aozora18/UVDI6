#pragma once

#define MOTION_CALI_CENTERING_MIN	0.002f	/* 단위: mm. 0.5 um */

class CWork
{
/* 생성자 & 파괴자 */
public:

	CWork();
	virtual ~CWork();

/* 가상함수 재정의 */
protected:
public:

/* 로컬 변수 */
protected:

	TCHAR				m_tzFile[MAX_PATH_LEN];

	UINT8				m_u8ACamID;			/* 1 or 2 */
	UINT8				m_u8WorkStep;		/* 현재 작업 단계 */
	UINT8				m_u8WorkTotal;		/* 전체 작업 단계 */
	UINT8				m_u8TrigRetry;		/* Put On Trigger : Retry Counting */

	UINT16				m_u16MoveRows;		/* 현재까지 이동된 Rows 개수 */
	UINT16				m_u16MoveCols;		/* 현재까지 이동된 Cols 개수 */
	UINT16				m_u16Thickness;		/* 방금 전에 노광한 소재의 두께 (um) */

	UINT32				m_u32GrabSize;		/* Grabbed Image Size (단위: Bytes) */

	UINT64				m_u64DelayTime;
	UINT64				m_u64WaitTime;
	UINT64				m_u64JobTime;		/* 총 작업하는데 소요된 시간 저장 (단위: msec) */
	UINT64				m_u64StartTime;		/* 작업 시작 시간 (단위: msec) */
	UINT64				m_u64GrabbedTick;	/* 임의 시간 동안 Grabbed Image가 없는 경우, 다시 측정 시도 */

	DOUBLE				m_dbTotalRate;		/* 전체 진행 횟수 */

	DOUBLE				m_dbOffsetX;		/* 단차 X (단위: mm) */
	DOUBLE				m_dbOffsetY;		/* 단차 Y (단위: mm) */

	ENG_JWNS			m_enWorkState;

	FILE				*m_fpCali;

	LPG_CACC			m_pstCali;

	LPG_ACGR			m_pstGrabbedResult;


/* 로컬 함수 */
protected:

	BOOL				IsWorkTimeOut();

	VOID				InitWork(UINT8 cam_id);
	VOID				SetWorkWaitTime(UINT64 time);
	VOID				CloseCaliFile();

	ENG_JWNS			SetMovingInit();
	ENG_JWNS			IsMovedInited();


/* 공용 함수 */
public:

	UINT8				GetWorkStep()		{	return m_u8WorkStep;			}

	UINT16				GetCurRows()		{	return m_u16MoveRows;			};
	UINT16				GetCurCols()		{	return m_u16MoveCols;			};

	UINT64				GetJobTime()		{	return m_u64JobTime;			};
	UINT64				GetWorkTime()		{	return GetTickCount64() - m_u64StartTime;	}

	DOUBLE				GetProcRate();

	DOUBLE				GetOffsetX()		{	return m_dbOffsetX;				}
	DOUBLE				GetOffsetY()		{	return m_dbOffsetY;				}

	ENG_JWNS			GetWorkState()		{	return m_enWorkState;			};

	VOID				ResetWorkState()	{	m_enWorkState	= ENG_JWNS::en_none;	};

	BOOL				IsCompleted()		{	return m_enWorkState == ENG_JWNS::en_comp;	}
};