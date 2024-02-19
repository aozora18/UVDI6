
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

	// 현재 Lock (Sync)이 1개라도 되어 있는지 확인
	inline BOOL IsLockCount()
	{
		return m_i32Sync > 0 ? TRUE : FALSE;
	}

protected:

	CRITICAL_SECTION	m_csSync;

	INT32				m_i32Sync;
};