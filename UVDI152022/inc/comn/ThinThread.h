
#pragma once

#include <afxmt.h>

class CThinThread
{

public:

	CThinThread(UINT32 exit_msg=NULL);
	virtual ~CThinThread();

/* ���� �Լ� */
protected:

	virtual BOOL		StartWork()	= 0;
	virtual VOID		RunWork()	= 0;
	virtual VOID		EndWork()	= 0;

/* ���� ���� */
protected:

	BOOL				m_u8IsBusy;
	BOOL				m_u8IsStop;

	UINT32				m_u32CycleTime;
	UINT32				m_u32ExitMsg;		/* �θ𿡰� �ڽ��� ����Ǿ��� ��, �˸��� �ִ� �̺�Ʈ �޽��� ID */

	HANDLE				m_hThread;
	HWND				m_hParent;			/* �ڽ��� ȣ���� �θ� ���� �ڵ� */

	CEvent				*m_pEventWait;		/* ������ ��� �̺�Ʈ */
	CEvent				*m_pEventSleep;		/* ������ ���� �̺�Ʈ */


/* ���� �Լ� */
protected:

	VOID				Run();

	static UINT32 __stdcall Start(VOID *pv);

/* ���� �Լ� */
public:

	UINT32				GetCycleTime() const;

	BOOL				CreateThread(UINT32 statck=0, LPSECURITY_ATTRIBUTES sec=NULL, UINT32 msec=1000);
	BOOL				IsBusy();

	HANDLE				GetHandle()	{	return m_hThread;	}
	
	VOID				Stop(UINT32 cycle_time=100);
	VOID				SetCycleTime(UINT32 msec=100);

	/* --------------------------------------- */
	/* ������ ��� �Լ� (Sync or Message Pump) */
	/* --------------------------------------- */
	VOID				Sleep(UINT32 msec=100);		/* CPU ��ü�� ���߸鼭 ��� */
};