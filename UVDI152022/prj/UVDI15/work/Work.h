
#pragma once

#include <atomic>
class CMainThread;
class CWork
{
/* ������ & �ı��� */
public:

	CWork();
	virtual ~CWork();
	
	void SetProcessComplete() { workProcessCompleted = true; }
	static void SetAbort(bool set) 
	{

		aborted.store(set);
		
	}

	static bool GetAbort()
	{
		return aborted.load();
	}


	static void SetonExternalWork(bool set)
	{
		onExternalWork.store(set);
	}

	static bool GetonExternalWork()
	{
		return onExternalWork.load();
	}

	UINT32 SetActionRequest(ENG_RIJA action)
	{
		requestActionCode = (UINT32)action;
		return requestActionCode;
	}

/* �����Լ� ������ */
protected:
public:
	volatile static atomic<bool> aborted;
	volatile static atomic<bool> onExternalWork;

	void SetMainthreadPtr(CMainThread* ptr)
	{
		mainthreadPtr = ptr;
	}

/* ������ */
protected:
	CMainThread* mainthreadPtr = nullptr;

/* ���� ���� */
protected:
	UINT32				requestActionCode = 0;
	UINT8				m_u8SetPhNo;			/* ���� Photohead Number (0-based) */

	UINT8				m_u8RetryCount;			/* ������ Step ������ �ݺ� Ƚ�� */
	UINT8				m_u8RetryStep;			/* ������ Step ������ �ݺ� ��ġ */

	UINT8				m_u8StepIt;				/* ���� �۾� �ܰ� */
	UINT8				m_u8StepTotal;			/* ��ü �۾� �ܰ� */
	UINT8				m_u8StepLast;			/* ���� �ֱ��� �۾� �ܰ� �ӽ� ���� */

	TCHAR				m_tzWorkName[WORK_NAME_LEN];
	TCHAR				m_tzStepName[STEP_NAME_LEN];
#if 0
	BOOL				m_bWorkAbortSet;		/* �۾� ��� ��û ���� */
#endif
	UINT64				m_u64DelayTime;
	UINT64				m_u64WaitTime;
	UINT64				m_u64ReqSendTime;		/* �ó����� ��� ���� �� �ֱ������� ���� Ȯ���ϴ� ��� �۽� �ֱ� ������ ���ؼ� ... */
	UINT64				m_u64StartTime;			/* �۾� ���� �ð� ���� */

	DOUBLE				m_dbPosACam[2];			/* Max 4 ��. Global Mark Mark1=0, Mark2=2, Mark3=1, Mark4=3 */
	DOUBLE				m_dbAlignStageY;		/* Align Mark �˻��� ���� �����  ���� �� �� �̵� ��ġ �� (����: mm) */

	bool uiRefresh = false;
	ENG_JWNS			m_enWorkState;			/* �۾� ���� */
	ENG_BWOK			m_enWorkJobID;			/* �۾� ���� */
#if 0
	CMySection			m_csSyncAbortSet;		/* �۾� ��� ���� ����ȭ */
#endif

private:
	

/* ���� �Լ� */
protected:
	

	BOOL				InitWork();
	VOID				EndWork();

	VOID				SetWorkName();

	VOID				CalcStepRate();
	VOID				SetWorkNext();
	VOID				CheckWorkTimeout();

	VOID				SaveWorkLogs(PTCHAR mesg);

	VOID				SetStepName(PTCHAR name);

	BOOL				GetLocalLeftRightTopMarkIndex(UINT8 scan, UINT8 &left, UINT8 &right);
	BOOL				GetLocalLeftRightBottomMarkIndex(UINT8 scan, UINT8 &left, UINT8 &right);

	BOOL				SetTrigPosCalcSaved(INT32 acam_diff_y/*, INT32 mark_diff_y*/);
	BOOL				SetAlignACamCaliX();
	void				LocalTrigRegist();//����Ʈ���� ���
	BOOL				ResetExpoReady();

	BOOL				IsSendCmdTime(UINT32 wait);
	BOOL				IsWorkRepeat()				{	return m_u8StepIt == m_u8StepLast;	}	/* ���� �ֱ��� �۾� ������ �����ϰ� �ݺ��ϰ� �ִ��� ���� */
	BOOL				IsMC2ErrorCheck();
	BOOL				IsSelectedRecipeValid();
	BOOL				IsLuriaStatusError();
	BOOL				IsGerberMarkValidCheck();
	BOOL				IsValidMarkCount();
	BOOL				IsAlignMarkRegisted();
	BOOL				IsGerberCheck(UINT8 flag);
	BOOL				IsChillerRunning();
	BOOL				IsVacuumRunning();
	BOOL				IsRecipeGerberCheck();
	BOOL				IsMarkTypeOnlyGlobal();

	INT32				GetLeftRightBottomMarkPosY(ENG_AMTF mark, UINT8 scan=0x00, UINT8 direct=0x00);
	INT32				GetGlobalMarkMotionPosY(UINT8 mark_no);
	INT32				GetLocalMarkMotionPosY(UINT8 index);
	INT32				GetACam1Mark2MotionX(UINT8 mark_no);

	VOID				SetSendCmdTime()	{	m_u64ReqSendTime = GetTickCount64();	}

	VOID				GetACamCentDistXY(INT32 &dist_x, INT32 &dist_y);
	VOID				GetACamCentDistXY(DOUBLE &dist_x, DOUBLE &dist_y);
	VOID				GetGlobalMarkIndex(UINT8 mark_no, UINT8 &cam_id, UINT8 &img_id);
#if 0
	VOID				GetMovePosGlobalMark(UINT8 mark_no, INT32 &acam_x, INT32 &stage_y);
#endif
	VOID				GetMovePosGlobalMarkACam1(UINT8 mark_no, INT32 &acam_x, INT32 &stage_y);

	/* �ֱ� �۾� ��� �ð��� ���� �ð����� ���� */
	VOID				UpdateWaitingTime()			{	m_u64DelayTime = GetTickCount64();	}

	DOUBLE				GetACamMark2MotionX(UINT16 mark_no);
	BOOL				GetGlobalMarkMoveXY(UINT16 mark_no, DOUBLE &acam_x, DOUBLE &stage_y);

	BOOL				IsTrigPosResetAll();


/* ���� �Լ� */
public:

	UINT8				GetStepIt()			{	return m_u8StepIt;		}
	PTCHAR				GetStepName()		{	return m_tzStepName;	}
	ENG_BWOK			GetWorkJobID()		{	return m_enWorkJobID;	}
	ENG_JWNS			GetWorkState()		{	return m_enWorkState;	}

	void				SetUIRefresh(bool set) 
	{ 
		uiRefresh = set; 
	}

	bool				GetUIRefresh() 
	{ 
		return uiRefresh; 
	}


	

	UINT32 GetActionRequest() 
	{
		

		return requestActionCode;
	}
	

	bool workProcessCompleted = false;
	BOOL				IsWorkCompleted()	{	return m_enWorkState == ENG_JWNS::en_comp;	}
	BOOL				IsWorkStopped();
	BOOL				IsWorkError();
#if 0
	VOID				SetWorkStop();
	BOOL				IsWorkAbort()		{	return m_bWorkAbortSet;	}
#endif
};