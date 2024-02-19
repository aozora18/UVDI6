
#pragma once

#include <afxwin.h>

class CMyMutex
{

// 생성자 & 파괴자
public:

	/*
	 desc : 생성자
	 parm : share_name	- [in]  공유하고자 하는 고유의 명칭
	 retn : None
	*/
	CMyMutex()
	{
		m_hMutex	= NULL;
		wmemset(m_tzName, 0x00, 1024);
	}
	CMyMutex(TCHAR *share_name)
	{
		// 기존 Mutex 열어보고 열리지 않으면 새로운 Mutex 생성?
		m_hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, share_name);
		if (!m_hMutex)
		{
			m_hMutex= CreateMutex(NULL, FALSE, share_name);
		}
		if (m_hMutex)	swprintf_s(m_tzName, 1024, _T("%s"), share_name);
	}
	/*
	 desc : 파괴자
	 parm : None
	 retn : None
	*/
	virtual ~CMyMutex()
	{
		if (m_hMutex)	CloseHandle(m_hMutex);
		m_hMutex	= NULL;
	}

// 로컬 변수
protected:

	TCHAR				m_tzName[1024];
	HANDLE				m_hMutex;


// 로컬 함수
protected:


// 공용 함수
public:

	/*
	 desc : 뮤텍스 핸들 반환
	 parm : None
	 retn : 핸들
	*/
	HANDLE GetHandle()
	{
		return m_hMutex;
	}

	/*
	 desc : 뮤텍스 열기
	 parm : share_name	- [in]  공유하고자 하는 고유의 명칭
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
	 desc : 뮤텍스 닫기
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
	 parm : time	- [in]  Lock이 풀릴 때까지 최대 대기 시간 값 (단위: 밀리초)
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
		// Fail. GetLastError()로 원인을 확인 (Logic 빠져 나가야 함)
		case WAIT_FAILED	:	return FALSE;
		// Event Object를 Reset하고, 다시 WaitForSingleObject()를 호출해야 함
		case WAIT_ABANDONED	:	ReleaseMutex(m_hMutex);	return FALSE;
		// Time-out이 발생됨
		case WAIT_TIMEOUT	:	return FALSE;
		}

		// 기다리던 Event가 Signal된 경우
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
	 desc : 가장 마지막 발생된 에러 값 반환
	 parm : None
	 retn : Error 값 반환
	*/
	UINT32 GetError()
	{
		return GetLastError();
	}
};
