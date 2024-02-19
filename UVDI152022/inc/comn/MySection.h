
#pragma once

class CMySection
{
public:

	CMySection()
	{
		__try
		{
			m_i32Sync	= 0;
			::InitializeCriticalSection(&m_csSync);
		}
		__except(STATUS_NO_MEMORY == GetExceptionCode())
		{
			ASSERT(_T("Out of Memory"));
		}
	}
	virtual ~CMySection()
	{
		::DeleteCriticalSection(&m_csSync);
	}
	inline BOOL Enter()
	{
		__try
		{
			::EnterCriticalSection(&m_csSync);
			m_i32Sync++;
		}
		__except(STATUS_NO_MEMORY == GetExceptionCode())
		{
			ASSERT(_T("Out of Memory"));
			return FALSE;
		}
		return TRUE;
	}
	inline VOID Leave()
	{
		m_i32Sync--;
		::LeaveCriticalSection(&m_csSync);
	}

	// ���� Lock (Sync)�� 1���� �Ǿ� �ִ��� Ȯ��
	inline BOOL IsLockCount()
	{
		return m_i32Sync > 0 ? TRUE : FALSE;
	}

protected:

	CRITICAL_SECTION	m_csSync;

	INT32				m_i32Sync;
};