
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
 desc : ������
 parm : exit_msg	- [in]  �����尡 ����Ǿ��� ��, �θ𿡰� ������ �� �ִ� �̺�Ʈ �޽��� ID ��
 retn : None
*/
CThinThread::CThinThread(UINT32 exit_msg /* NULL */)
{
	m_u8IsBusy		= 0x00;	/* �ݵ�� �ʱ�ȭ �ʿ� */
	m_hParent		= NULL;
	m_u32ExitMsg	= exit_msg;

	/* �̺�Ʈ ���� �� ���� */
	m_pEventWait	= new CEvent(FALSE, FALSE);	/* non-signaled,	auto   reset */
	m_pEventSleep	= new CEvent(FALSE, TRUE);	/* non-signaled,	manual reset */
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CThinThread::~CThinThread() 
{
	/* �ڵ� ���� ó�� */
	if (m_pEventSleep)
	{
		m_pEventSleep->SetEvent();
		delete m_pEventSleep;
		m_pEventSleep = NULL;
	}
	/* ���� ���� ó�� */
	if (m_pEventWait)
	{
		m_pEventWait->SetEvent();
		delete m_pEventWait;
		m_pEventWait = NULL;
	}
}

/*
 desc : ������ ����
 parm : stack	- [in]  ���� ũ��
		sec		- [in]  ���� �Ӽ�
		msec	- [in]  milli-seconds ��, 1/1000 �� (�ּ� 10 �̻� ���̾�� ��)
 retn : TRUE - ���� ����, FALSE - ����
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
 desc : ������ �ʱ�ȭ �� �ݺ� ���� �׸��� ���� ��ƾ�� ����ִ� �κ�
 parm : None
 retn : None
*/
VOID CThinThread::Run()
{
	/* ������ ���� ȣ�� ���� ����ȭ ��ü ���� */
	CSingleLock cslWait(m_pEventWait);

	/* ���� �ʱ�ȭ */
	if (StartWork())
	{
		/* ���� ���� ����... */
		while (!m_u8IsStop)
		{
			/* ���� ���� */
			RunWork();

			/* ���� �ð� ��� */
			if (m_u32CycleTime)
			{
				cslWait.Lock(m_u32CycleTime);
				cslWait.Unlock();
			}
		}
	}

	/* ���� �۾� ���� */
	EndWork();

	/* �ڽ��� �����尡 ����Ǿ��� �� �˸��� ���� (�θ𿡰� �̺�Ʈ �޽����� ...) */
	if (m_u32ExitMsg && m_hParent)
	{
		::PostMessage(m_hParent, m_u32ExitMsg, 0, 0L);
	}

	/* -------------------------- */
	/* �����尡 ����Ǿ��ٰ� ���� */
	/* -------------------------- */
	m_u8IsBusy = 0x00;

	/* ������ �ڵ� ��ü ���� �� ����ó�� */
	CloseHandle(m_hThread);
	m_hThread = NULL;
	_endthreadex(0);
}

/*
 desc : ������ ������ ���� ȣ��Ǵ� �ݹ� ��
 parm : pv	- [in]  ���� ������
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
 desc : ������ ���� ����(����) ȣ��
 parm : cycle_time	- [in]  ������ �����ϱ� ���� ���� �ӵ��� �󸶳� ������ �� ������ (����: �и���)
 retn : None
*/
VOID CThinThread::Stop(UINT32 cycle_time)
{
	/* �����带 �����ϱ� ����, �������� ������ ������ ��. ���� �̺�Ʈ ó�� �ӵ��� �Ѱ� ������ */
	SetCycleTime(cycle_time);
	/* ����ڿ� ���� ������ �����Լ��� ȣ��� ��� �÷��� ���� (�ݹ��Լ� ������...) */
	m_u8IsStop = 0x01;
}

/*
 desc : ������ ���� ���� �ð� ����
 parm : msec	- [in]  �и� �� ��, 1 / 1000 ��
*/
VOID CThinThread::SetCycleTime(UINT32 msec)
{
	/* 10 ���� ���� Windows �ü������ ���� ����(?) -> �ذ� : ��Ƽ�̵�� Ÿ�� ��, CPU Ŭ���� �̿��� �ذ� */
	if (msec != 0 && msec < 10)	msec	= 10;
	/* �������� ���� �ð��� ������ ���, ���� �ð� ��⸦ ������~ ���������� ������ �ȴ� (?) */
	m_u32CycleTime = msec;
	/* �̷��� �ؾ� Thread���� SendMessage ȣ�� �� ������ ���� ������ѵ� ���� �ʴ� ������ ���ֱ� ���� */
	CThinThread::Sleep(msec);
}
UINT32 CThinThread::GetCycleTime() const
{
 	return m_u32CycleTime;
}

/*
 desc : ������ ���� ���� Ȯ��
 parm : None
 retn : TRUE - ������ ���� �� / FALSE - ������ ���� �Ϸ�
*/
BOOL CThinThread::IsBusy()
{
	return m_u8IsBusy;
}

/*
 desc : ���� �ð�(�и���)��ŭ ��޸��� - ����ȭ ��ü ���
 parm : msec	- [in]  ��� �ð� (����: �и� ��) / �⺻ �� 300 �и���
 retn : None
*/
VOID CThinThread::Sleep(UINT32 msec)
{
	CSingleLock cslSleep(m_pEventSleep);
	cslSleep.Lock(msec);
	cslSleep.Unlock();
}
