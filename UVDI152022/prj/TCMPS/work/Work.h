
#pragma once

class CWork
{
/* 생성자 & 파괴자 */
public:

	CWork();
	virtual ~CWork();

/* 가상함수 재정의 */
protected:
public:

	virtual VOID		RunWork()	= 0;


/* 열거형 */
protected:


/* 로컬 변수 */
protected:

	TCHAR				m_tzLastWorkJob[MAX_WORK_JOB_NAME];

	UINT8				m_u8ACamID;			/* 1 or 2 */
	UINT8				m_u8WorkStep;		/* 현재 작업 단계 */
	UINT8				m_u8WorkLast;		/* 가장 최근의 작업 단계 임시 저장 */
	UINT8				m_u8WorkTotal;		/* 전체 작업 단계 */
	UINT8				m_u8TrigRetry;		/* Put On Trigger : Retry Counting */
	UINT8				m_u8PrintStripe;	/* 가장 최근에 Printing 할 때, 노광된 Stripe 횟수 */

	UINT16				m_u16MarkNo;		/* Global Mark Number (0x01 ~ 0x04)*/

	UINT64				m_u64DelayTime;
	UINT64				m_u64WaitTime;
	UINT64				m_u64ReqSendTime;	/* 주기적으로 요청하는 패킷의 송신 간격 (단위: msec) */
	UINT64				m_u64StripeTime;	/* 가장 최근에 노광 1 줄 (Stripe) 진행한 시간 (단위: msec) */

	DOUBLE				m_dbAlignMoveEndY;	/* Align Mark 검색할 때, 최종 도착될 위치 임시 저장 (단위: mm) */
	DOUBLE				m_dbTrigPos[MAX_ALIGN_CAMERA][2];	/* 얼라인 카메라의 트리거 위치 즉, Y 축 스테이지의 트리거 위치 값 (단위: mm) */

	ENG_AOEM			m_enAlignMode;		/* 0x01 : Direct, 0x02 : Align, 0x03 : Align & Calibration */

	ENG_MMDI			m_enVecMoveAxis;	/* Vector 방식으로 이동할 때, 기준이 되는 Drive 축 */

	ENG_JWNS			m_enWorkState;
	ENG_BWOK			m_enWorkJobID;		/* 작업 종류 */

	LPG_ACGR			m_pstGrabResult;	/* 임시 Model Find 후 저장된 결과 구조체 포인터 */


/* 로컬 함수 */
protected:

	VOID				IsWorkTimeOut();

	VOID				InitWork();
	VOID				SetWorkWaitTime(UINT64 time);

	BOOL				IsVacuumOnOff(UINT8 mode);
	BOOL				IsChillerRunning();
	/* 가장 최근의 작업 정보가 동일하게 반복하고 있는지 여부 */
	BOOL				IsWorkRepeat()	{	return m_u8WorkStep == m_u8WorkLast;	}

	VOID				SetWorkJobName(PTCHAR job_name);

	ENG_JWNS			IsWorkWaitTime();
	ENG_JWNS			IsMotorDriveStopAll();
	ENG_JWNS			SetTrigDisabled();
	ENG_JWNS			IsTrigDisabled(BOOL check=TRUE);
	ENG_JWNS			IsRegistGerberCheck();
	ENG_JWNS			SetMovingAlignMark();
	ENG_JWNS			IsMovedAlignMark();
	ENG_JWNS			PutOneTrigger();
	ENG_JWNS			GetGrabResult();
	ENG_JWNS			SetAlignMovingInit();
	ENG_JWNS			IsAlignMovedInit();
	ENG_JWNS			SetLuriaAreaMode();
	ENG_JWNS			IsLuriaAreaMode();
	ENG_JWNS			SetTrigCalcGlobal4();
	ENG_JWNS			SetTrigRegistGlobal4();
	ENG_JWNS			IsTrigRegistedGlobal4();
	ENG_JWNS			SetAlignMovingGlobal4();
	ENG_JWNS			IsSetMarkValidGlobal4();
	ENG_JWNS			IsAlignMovedGlobal();
	ENG_JWNS			SetMovingUnloader();
	ENG_JWNS			IsMovedUnloader();
	ENG_JWNS			SetAlignMarkRegist();
	ENG_JWNS			IsAlignMarkRegisted();
	ENG_JWNS			SetPrePrinting();
	ENG_JWNS			IsPrePrinted();
	ENG_JWNS			SetPrinting();
	ENG_JWNS			IsPrinted();
	ENG_JWNS			SetExpoReady();
	ENG_JWNS			SetHomingPHAll();
	ENG_JWNS			IsHomedPHAll();


/* 공용 함수 */
public:

	VOID				SetAlignMode(ENG_AOEM mode)	{	m_enAlignMode	= mode;					}
	UINT8				GetWorkStep()				{	return m_u8WorkStep;					};

	ENG_JWNS			GetWorkState()				{	return m_enWorkState;					};
	ENG_BWOK			GetWorkJobID()				{	return m_enWorkJobID;					};

	VOID				ResetWorkState()			{	m_enWorkState	= ENG_JWNS::en_none;		};
	BOOL				IsCompleted()				{	return m_enWorkState == ENG_JWNS::en_comp;	};
	BOOL				IsWorkError()				{	return m_enWorkState == ENG_JWNS::en_error;	};

	BOOL				IsExistGerberMark();

	LPG_ACGR			GetLastGrabResult()			{	return m_pstGrabResult;					};

	PTCHAR				GetLastWorkJobName()		{	return m_tzLastWorkJob;					}

	DOUBLE				GetWorkRate();
};