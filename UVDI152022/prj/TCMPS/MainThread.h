
#pragma once

#include "../../inc/comn/ThinThread.h"

class CWork;
class CWorkExpoOnly;

class CMainThread : public CThinThread
{
/* ������ / �ı��� */
public:
	CMainThread(HWND parent);
	virtual ~CMainThread();

/* ���� �Լ� ������ */
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();

/* ���� ���� */
protected:

	BOOL				m_bRunWork;			/* ���� ���� */
	BOOL				m_bReqPeriodMC2;	/* �ѹ��� Active, �ѹ��� Reference ȣ���ϱ� ���� */

	UINT64				m_u64PeridPktTime;

	CMySection			m_syncCali;

	CWork				*m_pWorkJob;
	CWorkExpoOnly		*m_pExpoOnly;

/* ���� �Լ� */
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

/* ���� �Լ� */
public:

	BOOL				IsRunWork()		{	return m_bRunWork;	}
	DOUBLE				GetWorkRate();

	VOID				RemoveWorkJob();
	VOID				StopWorkJob();

	BOOL				SetWorkExpoOnly();

	ENG_BWOK			GetWorkJobID();

	PTCHAR				GetErrorJobName();
};
