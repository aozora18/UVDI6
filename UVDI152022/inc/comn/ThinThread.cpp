
#include "pch.h"
#include "ThinThread.h"
#include <Process.h>
#include <WinSock.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 desc : 생성자
 parm : exit_msg	- [in]  스레드가 종료되었을 때, 부모에게 전달할 수 있는 이벤트 메시지 ID 값
 retn : None
*/
CThinThread::CThinThread(UINT32 exit_msg /* NULL */)
{
	m_u8IsBusy		= 0x00;	/* 반드시 초기화 필요 */
	m_hParent		= NULL;
	m_u32ExitMsg	= exit_msg;

	/* 이벤트 생성 및 설정 */
	m_pEventWait	= new CEvent(FALSE, FALSE);	/* non-signaled,	auto   reset */
	m_pEventSleep	= new CEvent(FALSE, TRUE);	/* non-signaled,	manual reset */
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CThinThread::~CThinThread() 
{
	/* 자동 리셋 처리 */
	if (m_pEventSleep)
	{
		m_pEventSleep->SetEvent();
		delete m_pEventSleep;
		m_pEventSleep = NULL;
	}
	/* 수동 리셋 처리 */
	if (m_pEventWait)
	{
		m_pEventWait->SetEvent();
		delete m_pEventWait;
		m_pEventWait = NULL;
	}
}

/*
 desc : 스레드 생성
 parm : stack	- [in]  스택 크기
		sec		- [in]  보안 속성
		msec	- [in]  milli-seconds 즉, 1/1000 초 (최소 10 이상 값이어야 함)
 retn : TRUE - 생성 성공, FALSE - 실패
*/
BOOL CThinThread::CreateThread(UINT32 statck, LPSECURITY_ATTRIBUTES sec, UINT32 msec)
{
	UINT32 u32ThreadID	= 0;
	m_hThread			= NULL;
	m_u8IsBusy			= 1;
	m_u8IsStop			= 0;

	m_hThread = (HANDLE)(_beginthreadex(sec,  statck, Start, this, 1, &u32ThreadID));
#if 0
	m_u32CycleTime		= msec;
#else
	SetCycleTime(msec);
#endif
	/* modified by jskang21. 2004/03/23 */
	return m_hThread ? TRUE : FALSE;
//	return m_hThread ? reinterpret_cast<unsigned long>(m_hThread) : FALSE;
}

/*
 desc : 스레드 초기화 및 반복 수행 그리고 종료 루틴이 들어있는 부분
 parm : None
 retn : None
*/
VOID CThinThread::Run()
{
	/* 스레드 종료 호출 여부 동기화 객체 정의 */
	CSingleLock cslWait(m_pEventWait);

	/* 시작 초기화 */
	if (StartWork())
	{
		/* 무한 루프 돌기... */
		while (!m_u8IsStop)
		{
			/* 본문 수행 */
			RunWork();

			/* 일정 시간 대기 */
			if (m_u32CycleTime)
			{
				cslWait.Lock(m_u32CycleTime);
				cslWait.Unlock();
			}
		}
	}

	/* 종료 작업 수행 */
	EndWork();

	/* 자신의 스레드가 종료되었을 때 알리기 위함 (부모에게 이벤트 메시지로 ...) */
	if (m_u32ExitMsg && m_hParent)
	{
		::PostMessage(m_hParent, m_u32ExitMsg, 0, 0L);
	}

	/* -------------------------- */
	/* 스레드가 종료되었다고 설정 */
	/* -------------------------- */
	m_u8IsBusy = 0x00;

	/* 스레드 핸들 객체 해제 및 종료처리 */
	CloseHandle(m_hThread);
	m_hThread = NULL;
	_endthreadex(0);
}

/*
 desc : 스레드 생성에 의해 호출되는 콜백 함
 parm : pv	- [in]  전역 포인터
 retn : ???
*/
UINT32 __stdcall CThinThread::Start(VOID *pv)
{
	CThinThread *pMT = static_cast<CThinThread *>(pv);
	if (!pMT)	return 0;

	pMT->Run();

	return 1;
}

/*
 desc : 스레드 동작 중지(정지) 호출
 parm : cycle_time	- [in]  스레드 정지하기 전에 동작 속도를 얼마나 느리게 할 것인지 (단위: 밀리초)
 retn : None
*/
VOID CThinThread::Stop(UINT32 cycle_time)
{
	/* 스레드를 중지하기 위해, 스레드의 동작을 느리게 함. 윈도 이벤트 처리 속도의 한계 때문에 */
	SetCycleTime(cycle_time);
	/* 사용자에 의해 스레드 종료함수가 호출된 경우 플래그 설정 (콜백함수 때문에...) */
	m_u8IsStop = 0x01;
}

/*
 desc : 스레드 동작 제어 시간 설정
 parm : msec	- [in]  밀리 초 즉, 1 / 1000 초
*/
VOID CThinThread::SetCycleTime(UINT32 msec)
{
	/* 10 이하 값은 Windows 운영체제에서 감지 못함(?) -> 해결 : 멀티미디어 타임 즉, CPU 클럭을 이용한 해결 */
	if (msec != 0 && msec < 10)	msec	= 10;
	/* 스레드의 동작 시간을 변경할 경우, 일정 시간 대기를 가져야~ 정상적으로 변경이 된다 (?) */
	m_u32CycleTime = msec;
	/* 이렇게 해야 Thread에서 SendMessage 호출 후 스레드 강제 종료시켜도 죽지 않는 현상을 없애기 위함 */
	CThinThread::Sleep(msec);
}
UINT32 CThinThread::GetCycleTime() const
{
 	return m_u32CycleTime;
}

/*
 desc : 스레드 종료 여부 확인
 parm : None
 retn : TRUE - 스레드 동작 중 / FALSE - 스레드 종료 완료
*/
BOOL CThinThread::IsBusy()
{
	return m_u8IsBusy;
}

/*
 desc : 일정 시간(밀리초)만큼 기달리기 - 동기화 객체 사용
 parm : msec	- [in]  대기 시간 (단위: 밀리 초) / 기본 값 300 밀리초
 retn : None
*/
VOID CThinThread::Sleep(UINT32 msec)
{
	CSingleLock cslSleep(m_pEventSleep);
	cslSleep.Lock(msec);
	cslSleep.Unlock();
}
