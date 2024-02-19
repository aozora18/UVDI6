
#pragma once

class CWork
{
/* ������ & �ı��� */
public:

	CWork();
	virtual ~CWork();

/* �����Լ� ������ */
protected:
public:

	virtual VOID		RunWork()	= 0;


/* ������ */
protected:


/* ���� ���� */
protected:

	TCHAR				m_tzLastWorkJob[MAX_WORK_JOB_NAME];

	UINT8				m_u8ACamID;			/* 1 or 2 */
	UINT8				m_u8WorkStep;		/* ���� �۾� �ܰ� */
	UINT8				m_u8WorkLast;		/* ���� �ֱ��� �۾� �ܰ� �ӽ� ���� */
	UINT8				m_u8WorkTotal;		/* ��ü �۾� �ܰ� */
	UINT8				m_u8TrigRetry;		/* Put On Trigger : Retry Counting */
	UINT8				m_u8PrintStripe;	/* ���� �ֱٿ� Printing �� ��, �뱤�� Stripe Ƚ�� */

	UINT16				m_u16MarkNo;		/* Global Mark Number (0x01 ~ 0x04)*/

	UINT64				m_u64DelayTime;
	UINT64				m_u64WaitTime;
	UINT64				m_u64ReqSendTime;	/* �ֱ������� ��û�ϴ� ��Ŷ�� �۽� ���� (����: msec) */
	UINT64				m_u64StripeTime;	/* ���� �ֱٿ� �뱤 1 �� (Stripe) ������ �ð� (����: msec) */

	DOUBLE				m_dbAlignMoveEndY;	/* Align Mark �˻��� ��, ���� ������ ��ġ �ӽ� ���� (����: mm) */
	DOUBLE				m_dbTrigPos[MAX_ALIGN_CAMERA][2];	/* ����� ī�޶��� Ʈ���� ��ġ ��, Y �� ���������� Ʈ���� ��ġ �� (����: mm) */

	ENG_AOEM			m_enAlignMode;		/* 0x01 : Direct, 0x02 : Align, 0x03 : Align & Calibration */

	ENG_MMDI			m_enVecMoveAxis;	/* Vector ������� �̵��� ��, ������ �Ǵ� Drive �� */

	ENG_JWNS			m_enWorkState;
	ENG_BWOK			m_enWorkJobID;		/* �۾� ���� */

	LPG_ACGR			m_pstGrabResult;	/* �ӽ� Model Find �� ����� ��� ����ü ������ */


/* ���� �Լ� */
protected:

	VOID				IsWorkTimeOut();

	VOID				InitWork();
	VOID				SetWorkWaitTime(UINT64 time);

	BOOL				IsVacuumOnOff(UINT8 mode);
	BOOL				IsChillerRunning();
	/* ���� �ֱ��� �۾� ������ �����ϰ� �ݺ��ϰ� �ִ��� ���� */
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


/* ���� �Լ� */
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