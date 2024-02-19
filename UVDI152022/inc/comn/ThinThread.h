
#pragma once

#include <afxmt.h>

class CThinThread
{

public:

	CThinThread(UINT32 exit_msg=NULL);
	virtual ~CThinThread();

/* 가상 함수 */
protected:

	virtual BOOL		StartWork()	= 0;
	virtual VOID		RunWork()	= 0;
	virtual VOID		EndWork()	= 0;

/* 로컬 변수 */
protected:

	BOOL				m_u8IsBusy;
	BOOL				m_u8IsStop;

	UINT32				m_u32CycleTime;
	UINT32				m_u32ExitMsg;		/* 부모에게 자신이 종료되었을 때, 알릴수 있는 이벤트 메시지 ID */

	HANDLE				m_hThread;
	HWND				m_hParent;			/* 자신을 호출한 부모 윈도 핸들 */

	CEvent				*m_pEventWait;		/* 스레드 대기 이벤트 */
	CEvent				*m_pEventSleep;		/* 스레드 정지 이벤트 */


/* 로컬 함수 */
protected:

	VOID				Run();

	static UINT32 __stdcall Start(VOID *pv);

/* 공용 함수 */
public:

	UINT32				GetCycleTime() const;

	BOOL				CreateThread(UINT32 statck=0, LPSECURITY_ATTRIBUTES sec=NULL, UINT32 msec=1000);
	BOOL				IsBusy();

	HANDLE				GetHandle()	{	return m_hThread;	}
	
	VOID				Stop(UINT32 cycle_time=100);
	VOID				SetCycleTime(UINT32 msec=100);

	/* --------------------------------------- */
	/* 스레드 대기 함수 (Sync or Message Pump) */
	/* --------------------------------------- */
	VOID				Sleep(UINT32 msec=100);		/* CPU 자체가 멈추면서 대기 */
};