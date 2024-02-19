#pragma once

#define MOTION_CALI_CENTERING_MIN	0.002f	/* ����: mm. 0.5 um */

class CWork
{
/* ������ & �ı��� */
public:

	CWork();
	virtual ~CWork();

/* �����Լ� ������ */
protected:
public:

/* ���� ���� */
protected:

	TCHAR				m_tzFile[MAX_PATH_LEN];

	UINT8				m_u8ACamID;			/* 1 or 2 */
	UINT8				m_u8StepIt;			/* ���� �۾� �ܰ� */
	UINT8				m_u8WorkTotal;		/* ��ü �۾� �ܰ� */
	UINT8				m_u8TrigRetry;		/* Put On Trigger : Retry Counting */

	UINT16				m_u16MoveRows;		/* ������� �̵��� Rows ���� */
	UINT16				m_u16MoveCols;		/* ������� �̵��� Cols ���� */
	UINT16				m_u16Thickness;		/* ��� ���� �뱤�� ������ �β� (����: um) */

	UINT32				m_u32GrabSize;		/* Grabbed Image Size (����: Bytes) */

	UINT64				m_u64WorkTime;		/* Save the last time you worked */
	UINT64				m_u64WaitTime;
	UINT64				m_u64StartTime;		/* �۾� ���� �ð� (����: msec) */
	UINT64				m_u64GrabbedTick;	/* ���� �ð� ���� Grabbed Image�� ���� ���, �ٽ� ���� �õ� */
	UINT64				m_u64MotionTime;	/* Save the time the motion operated */
	UINT64				m_u64MovedTick;		/* Motion �̵� �� ����ȭ �� ������ �ּ� ��� �ð� 1.5�� ���� ����� �� */

	DOUBLE				m_dbTotalRate;		/* ��ü ���� Ƚ�� */

	DOUBLE				m_dbOffsetX;		/* ���� X (����: mm) */
	DOUBLE				m_dbOffsetY;		/* ���� Y (����: mm) */

	ENG_JWNS			m_enWorkState;

	FILE				*m_fpCali;

	LPG_CACC			m_pstCali;

	LPG_ACGR			m_pstGrabbedResult;


/* ���� �Լ� */
protected:

	VOID				InitWork(UINT8 cam_id);
	VOID				CloseCaliFile();

	VOID				SetWorkWaitTime(UINT64 time);
	BOOL				IsWorkWaitTime();

	BOOL				IsWorkTimeout();

	ENG_JWNS			SetMovingInit();
	ENG_JWNS			IsMovedInited();

	VOID				SetMotionTime()	{	m_u64MotionTime = GetTickCount64();	}
	BOOL				IsMotionTimeout(UINT64 delay)	{	return GetTickCount64() > (m_u64MotionTime + delay);	}


/* ���� �Լ� */
public:

	UINT8				GetWorkStep()		{	return m_u8StepIt;				}

	UINT16				GetCurRows()		{	return m_u16MoveRows;			};
	UINT16				GetCurCols()		{	return m_u16MoveCols;			};

	UINT64				GetWorkTime()		{	return (m_u64WorkTime - m_u64StartTime);	};

	DOUBLE				GetProcRate();

	DOUBLE				GetOffsetX()		{	return m_dbOffsetX;				}
	DOUBLE				GetOffsetY()		{	return m_dbOffsetY;				}

	ENG_JWNS			GetWorkState()		{	return m_enWorkState;			};

	VOID				ResetWorkState()	{	m_enWorkState	= ENG_JWNS::en_none;	};

	BOOL				IsCompleted()		{	return m_enWorkState == ENG_JWNS::en_comp;	}
};