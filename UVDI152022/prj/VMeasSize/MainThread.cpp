
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
 desc : Constructor
 parm : parent	- [in]  자신을 호출한 부모 윈도 핸들
 retn : None
*/
CMainThread::CMainThread(HWND parent)
	: CThinThread()
{
	m_hParent	= parent;
}

/*
 desc : Destructor
 parm : None
*/
CMainThread::~CMainThread()
{
}

/*
 desc : Called first when thread is starts
 parm : None
 retn : DI_TRUE or DI_FALSE
*/
BOOL CMainThread::StartWork()
{

	return TRUE;
}

/*
 desc : Called periodically by the thread
 parm : None
 retn : None
*/
VOID CMainThread::RunWork()
{
	/* Enter the sysnc. */
	if (m_syncCali.Enter())
	{
		/* It periodically sends an event message to the parent */
		SendMesgParent(MSG_ID_NORMAL, 100);

		/* Leave the sync. */
		m_syncCali.Leave();
	}
}

/*
 desc : Called last when thread is stopped
 parm : None
 retn : None
*/
VOID CMainThread::EndWork()
{
}

/*
 desc : It periodically sends an event message to the parent
 parm : msg_id	- [in]  message id (0 or later)
		timeout	- [in]  SendMessag Lifetime (unit: msec)
 retn : None
*/
VOID CMainThread::SendMesgParent(UINT16 msg_id, UINT32 timeout)
{
	DWORD_PTR dwResult	= 0;
	LRESULT lResult		= 0;

	/* 부모에게 이벤트 메시지 전달 */
#if 1
	lResult	= ::SendMessageTimeout(m_hParent, WM_MAIN_THREAD, msg_id, 0L,
								   SMTO_NORMAL, timeout, &dwResult);
	if (0 == lResult)
	{
		/* 현재 발생된 에러 값 저장 */
		TRACE("MainThread : SendMessage Time out <Normal> = %d \n", GetLastError());
	}
#else
	::PostMessage(m_hParent, WM_MAIN_THREAD, msg_id, 0L);
#endif
}
