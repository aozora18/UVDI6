
/*
 desc : 전체 MAIN THREAD
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
 desc : 생성자
 parm : parent	- [in]  자신을 호출한 부모 윈도 핸들
 retn : None
*/
CMainThread::CMainThread(HWND parent)
	: CThinThread()
{
	m_hParent	= parent;
}


/*
 desc : 파괴자
 parm : None
 retn : None
*/
CMainThread::~CMainThread()
{
}

/*
 desc : 스레드 동작시에 초기 한번 호출됨
 parm : None
 retn : DI_TRUE or DI_FALSE
*/
BOOL CMainThread::StartWork()
{
	/* 무조건 1 초 정도 쉬고 동작하도록 함 */
	CThinThread::Sleep(1000);

	return TRUE;
}

/*
 desc : 스레드 동작시에 주기적으로 호출됨
 parm : None
 retn : None
*/
VOID CMainThread::RunWork()
{
	/* 주기적으로 부모에게 이벤트 메시지 전달 */
	::PostMessage(m_hParent, WM_MAIN_THREAD, 0, 0L);
}

/*
 desc : 스레드 종료시에 마지막으로 한번 호출됨
 parm : None
 retn : None
*/
VOID CMainThread::EndWork()
{
}
