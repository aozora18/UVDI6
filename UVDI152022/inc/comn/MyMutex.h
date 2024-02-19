
#pragma once

#include <afxwin.h>

class CMyMutex
{

// ������ & �ı���
public:

	/*
	 desc : ������
	 parm : share_name	- [in]  �����ϰ��� �ϴ� ������ ��Ī
	 retn : None
	*/
	CMyMutex()
	{
		m_hMutex	= NULL;
		wmemset(m_tzName, 0x00, 1024);
	}
	CMyMutex(TCHAR *share_name)
	{
		// ���� Mutex ����� ������ ������ ���ο� Mutex ����?
		m_hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, share_name);
		if (!m_hMutex)
		{
			m_hMutex= CreateMutex(NULL, FALSE, share_name);
		}
		if (m_hMutex)	swprintf_s(m_tzName, 1024, _T("%s"), share_name);
	}
	/*
	 desc : �ı���
	 parm : None
	 retn : None
	*/
	virtual ~CMyMutex()
	{
		if (m_hMutex)	CloseHandle(m_hMutex);
		m_hMutex	= NULL;
	}

// ���� ����
protected:

	TCHAR				m_tzName[1024];
	HANDLE				m_hMutex;


// ���� �Լ�
protected:


// ���� �Լ�
public:

	/*
	 desc : ���ؽ� �ڵ� ��ȯ
	 parm : None
	 retn : �ڵ�
	*/
	HANDLE GetHandle()
	{
		return m_hMutex;
	}

	/*
	 desc : ���ؽ� ����
	 parm : share_name	- [in]  �����ϰ��� �ϴ� ������ ��Ī
	 retn : TRUE or FALSE
	*/
	BOOL Open(TCHAR *share_name)
	{
		m_hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, share_name);
		if (!m_hMutex)
		{
			m_hMutex	= CreateMutex(NULL, FALSE, share_name);
		}
		if (m_hMutex)	swprintf_s(m_tzName, 1024, _T("%s"), share_name);
		return m_hMutex ? TRUE : FALSE;
	}

	/*
	 desc : ���ؽ� �ݱ�
	 parm : None
	 retn : None
	*/
	VOID Close()
	{
		if (m_hMutex)
		{
			CloseHandle(m_hMutex);
		}
		m_hMutex	= NULL;
	}

	/*
	 desc : Lock
	 parm : time	- [in]  Lock�� Ǯ�� ������ �ִ� ��� �ð� �� (����: �и���)
	 retn : TRUE or FALSE
	*/
	BOOL Lock(UINT32 time=INFINITE)
	{
		UINT32 u32Ret	= WAIT_OBJECT_0;

		if (!m_hMutex)
		{
#ifdef _DEBUG
			AfxMessageBox(_T("The mutex is not initialized"), MB_ICONSTOP|MB_TOPMOST);
#endif // _DEBUG
			return FALSE;
		}

		// Mutex Locked ?
		u32Ret	= WaitForSingleObject(m_hMutex, time);
		switch (u32Ret)
		{
		// Fail. GetLastError()�� ������ Ȯ�� (Logic ���� ������ ��)
		case WAIT_FAILED	:	return FALSE;
		// Event Object�� Reset�ϰ�, �ٽ� WaitForSingleObject()�� ȣ���ؾ� ��
		case WAIT_ABANDONED	:	ReleaseMutex(m_hMutex);	return FALSE;
		// Time-out�� �߻���
		case WAIT_TIMEOUT	:	return FALSE;
		}

		// ��ٸ��� Event�� Signal�� ���
		return TRUE;	// WAIT_OBJECT_O
	}

	/*
	 desc : Unlock
	 parm : None
	 retn : TRUE or FALSE
	*/
	BOOL Unlock()
	{
		if (!m_hMutex)
		{
#ifdef _DEBUG
			AfxMessageBox(_T("The mutex is not initialized"), MB_ICONSTOP|MB_TOPMOST);
#endif // _DEBUG
			return FALSE;
		}

		ReleaseMutex(m_hMutex);

		return TRUE;
	}

	/*
	 desc : ���� ������ �߻��� ���� �� ��ȯ
	 parm : None
	 retn : Error �� ��ȯ
	*/
	UINT32 GetError()
	{
		return GetLastError();
	}
};
