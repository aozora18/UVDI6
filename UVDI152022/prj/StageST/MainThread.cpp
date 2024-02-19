
/*
 desc : Main Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "MainThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : Contstructure
 parm : parent	- [in]  The parent handle that called itself
 retn : None
*/
CMainThread::CMainThread(HWND parent)
	: CThinThread()
{
	m_hParent	= parent;
	m_u64Tick	= GetTickCount64();
#ifdef _DEBUG
	m_u64Wait	= 100;
#else
	m_u64Wait	= 50;
#endif
	if (uvEng_GetConfig()->IsRunDemo())	m_u64Wait	= 1;
}

/*
 desc : Destructure
 parm : None
*/
CMainThread::~CMainThread()
{
}

/*
 desc : When the thread is running, it is initially called once
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainThread::StartWork()
{

	return TRUE;
}

/*
 desc : Called periodically while the thread is running
 parm : None
 retn : None
*/
VOID CMainThread::RunWork()
{
	UINT64 u64Tick	= GetTickCount64();

	/* It is not connected the MC2 and Luria, No more update (refresh) */
	if (!uvEng_GetConfig()->IsRunDemo())
	{
		if (!uvCmn_Luria_IsConnected() ||
			!uvCmn_MC2_IsConnected() ||
			!uvCmn_MCQ_IsConnected())	return;
	}
	/* notify parent of the events periodically */
	if (u64Tick > (m_u64Tick + m_u64Wait))
	{
		/* Save the periodic update (refresh) time */
		m_u64Tick	= u64Tick;
		/* Notify parent */
		SendMesgParent(0x00, 100);
	}
}

/*
 desc : When the thread is stopping, It is called once at the end
 parm : None
 retn : None
*/
VOID CMainThread::EndWork()
{
}

/*
 desc : Send an event message to the parent (SendMessage) - Normal or Result
 parm : type	- [in]  Message Type (0x00: Normal, 0x01 : Result)
		timeout	- [in]  SendMessag Lifetime (unit: msec)
 retn : None
*/
VOID CMainThread::SendMesgParent(UINT8 type, UINT32 timeout)
{
	DWORD_PTR dwResult	= 0;
	LRESULT lResult		= 0;

	/* 부모에게 이벤트 메시지 전달 */
	lResult	= ::SendMessageTimeout(m_hParent, WM_MAIN_THREAD, 0, 0L,
								   SMTO_NORMAL, timeout, &dwResult);
	if (0 == lResult)
	{
#if 0
		/* 현재 발생된 에러 값 저장 */
		TRACE("MainThread : SendMessage Time out <Normal> = %d\n", GetLastError());
#endif
	}
}
