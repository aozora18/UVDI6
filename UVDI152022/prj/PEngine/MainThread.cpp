
/*
 desc : Main Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "MainThread.h"
#include "DlgMain.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : parent	- [in]  �ڽ��� ȣ���� �θ� ���� �ڵ�
 retn : None
*/
CMainThread::CMainThread(HWND parent)
	: CThinThread()
{
	m_hParent		= parent;
	m_u64TickNormal	= 0;
}

/*
 desc : �ı���
 parm : None
*/
CMainThread::~CMainThread()
{
}

/*
 desc : ������ ���۽ÿ� �ʱ� �ѹ� ȣ���
 parm : None
 retn : DI_TRUE or DI_FALSE
*/
BOOL CMainThread::StartWork()
{
	/* ������ 1 �� ���� ���� �����ϵ��� �� */
	CThinThread::Sleep(1000);

	return TRUE;
}

/*
 desc : ������ ���۽ÿ� �ֱ������� ȣ���
 parm : None
 retn : None
*/
VOID CMainThread::RunWork()
{
	/* ���� �޸𸮰� �ʱ�ȭ �Ǿ����� ���ο� ���� */
	if (!uvEng_IsInitedEngine())	return;

	UINT64 u64Tick	= GetTickCount64();
	/* �ֱ������� ȣ�� */
	if (u64Tick > m_u64TickNormal + 100)
	{
		m_u64TickNormal	= u64Tick;
		SendMesgNormal();
	}
}

/*
 desc : ������ ����ÿ� ���������� �ѹ� ȣ���
 parm : None
 retn : None
*/
VOID CMainThread::EndWork()
{
}

/*
 desc : �θ𿡰� �޽��� ���� (SendMessage)
 parm : timeout	- [in]  SendMessag Lifetime (unit: msec)
 retn : None
*/
VOID CMainThread::SendMesgNormal(UINT32 timeout)
{
	DWORD_PTR dwResult	= 0;
	LRESULT lResult		= 0;

	/* �θ𿡰� �̺�Ʈ �޽��� ���� */
	lResult	= ::SendMessageTimeout(m_hParent, WM_MAIN_THREAD, 0, 0L,
								   SMTO_NORMAL, timeout, &dwResult);
	if (0 == lResult)
	{
#if 0
		/* ���� �߻��� ���� �� ���� */
		TRACE("MainThread : SendMessage Time out <Normal> = %d \n", GetLastError());
#endif
	}
}

