
#pragma once

#include "../../inc/comn/ThinThread.h"

class CWork;
class CWorkExpoOnly;

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

	BOOL				m_bRunWork;			/* 동작 여부 */
	BOOL				m_bReqPeriodMC2;	/* 한번은 Active, 한번은 Reference 호출하기 위함 */

	UINT64				m_u64PeridPktTime;

	CMySection			m_syncCali;

	CWork				*m_pWorkJob;
	CWorkExpoOnly		*m_pExpoOnly;

/* 로컬 함수 */
protected:

	VOID				SendMesgParent(UINT16 msg_id, UINT32 timeout=100);
#if 1
	VOID				StartWorkJob();
	VOID				RunWorkJob();
#endif
#if (USE_LUIRA_REQ_PERIOD_PKT)
	VOID				PauseWorkJob();
#else
	VOID				ReqPeriodPkt();
#endif

/* 공용 함수 */
public:

	BOOL				IsRunWork()		{	return m_bRunWork;	}
	DOUBLE				GetWorkRate();

	VOID				RemoveWorkJob();
	VOID				StopWorkJob();

	BOOL				SetWorkExpoOnly();

	ENG_BWOK			GetWorkJobID();

	PTCHAR				GetErrorJobName();
};
