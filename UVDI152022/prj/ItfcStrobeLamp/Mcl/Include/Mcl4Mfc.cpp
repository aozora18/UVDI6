//
// FILE: Mcl4Mfc.cpp
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "Mcl4Mfc.h"

/////////////////////////////////////////////////////////////////////////
// CMclWinThread base class functions...
/////////////////////////////////////////////////////////////////////////

// constructor...
CMclWinThread::CMclWinThread(CMcl4MfcThread *pOwner) {
    // initialize the main window handle and turn off
    // the CWinThread auto-deletion...
    m_bAutoDelete = FALSE;
    m_pMainWnd = NULL;

    // initialize the owner...
    m_pOwner = pOwner;
}

// overridables, default behavior is to 
// call the same function in the pOwner object...
BOOL CMclWinThread::InitInstance(void) {
    ASSERT(m_pOwner);
    return m_pOwner->InitInstance();
}

int CMclWinThread::ExitInstance(void) {
    ASSERT(m_pOwner);
    return m_pOwner->ExitInstance();
}

BOOL CMclWinThread::OnIdle( LONG lCount) {
    ASSERT(m_pOwner);
    return m_pOwner->OnIdle(lCount);
}

BOOL CMclWinThread::PreTranslateMessage( MSG *pMsg) {
    ASSERT(m_pOwner);
    return m_pOwner->PreTranslateMessage(pMsg);
}

BOOL CMclWinThread::IsIdleMessage( MSG *pMsg) {
    ASSERT(m_pOwner);
    return m_pOwner->IsIdleMessage(pMsg);
}

LRESULT CMclWinThread::ProcessWndProcException( CException *e, const MSG *pMsg) {
    ASSERT(m_pOwner);
    return m_pOwner->ProcessWndProcException( e, pMsg);
}

BOOL CMclWinThread::ProcessMessageFilter( int code, LPMSG lpMsg) {
    ASSERT(m_pOwner);
    return m_pOwner->ProcessMessageFilter( code, lpMsg);
}

int CMclWinThread::Run(void) {
    ASSERT(m_pOwner);
    return m_pOwner->Run();
}

// these functions call the original MFC base 
// class implementations...
BOOL CMclWinThread::MfcInitInstance(void) {
    // we MUST return TRUE here or MFC will shutdown
    // the newly created thread...
    return TRUE;
}

int CMclWinThread::MfcExitInstance(void) {
    return CWinThread::ExitInstance();
}

BOOL CMclWinThread::MfcOnIdle( LONG lCount) {
    return CWinThread::OnIdle(lCount);
}

BOOL CMclWinThread::MfcPreTranslateMessage( MSG *pMsg) {
    return CWinThread::PreTranslateMessage(pMsg);
}

BOOL CMclWinThread::MfcIsIdleMessage( MSG *pMsg) {
    return CWinThread::IsIdleMessage(pMsg);
}

LRESULT CMclWinThread::MfcProcessWndProcException( CException *e, const MSG *pMsg) {
    return CWinThread::ProcessWndProcException( e, pMsg);
}

BOOL CMclWinThread::MfcProcessMessageFilter( int code, LPMSG lpMsg) {
    return CWinThread::ProcessMessageFilter( code, lpMsg);
}

int CMclWinThread::MfcRun(void) {
    return CWinThread::Run();
}

/////////////////////////////////////////////////////////////////////////
// CMcl4MfcThread base class functions...
/////////////////////////////////////////////////////////////////////////

// constructor...
CMcl4MfcThread::CMcl4MfcThread() {
    // create the internal CWinThread derived object...
    m_pWinThread = new CMclWinThread(this);

    // detect errors and throw exceptions...
    if (m_pWinThread) {
        m_dwStatus = NO_ERROR;
    }
    else {
        m_dwStatus = ::GetLastError();
        ThrowError(m_dwStatus);
    }

    // notice that the OS thread is not created here,
    // only the C++ objects.
    // the derived classes should call m_pWinThread->CreateThread()
    // in their constructors as appropriate and save a
    // handle to the created thread in m_hHandle...
    m_hHandle = NULL;
}

// destructor...
CMcl4MfcThread::~CMcl4MfcThread() {
    // the internal CMclWinThread does NOT delete itself
    // like CWinThread objects can, so we delete it here...
    delete m_pWinThread;
    m_pWinThread = NULL;
}

// overrideable virtual functions...
// the default implmentation calls the member function
// in the CMclWinThread object which calls the original
// CWinThread implementation...
// derived classes which override these functions should
// usually also call the base class implementations...
/////////////////////////////////////////////////////////////////////////
BOOL CMcl4MfcThread::InitInstance(void) {
    ASSERT(m_pWinThread);
    return m_pWinThread->MfcInitInstance();
}

int CMcl4MfcThread::ExitInstance(void) {
    ASSERT(m_pWinThread);
    return m_pWinThread->MfcExitInstance();
}

BOOL CMcl4MfcThread::OnIdle( LONG lCount) {
    ASSERT(m_pWinThread);
    return m_pWinThread->MfcOnIdle(lCount);
}

BOOL CMcl4MfcThread::PreTranslateMessage( MSG *pMsg) {
    ASSERT(m_pWinThread);
    return m_pWinThread->MfcPreTranslateMessage(pMsg);
}

BOOL CMcl4MfcThread::IsIdleMessage( MSG *pMsg) {
    ASSERT(m_pWinThread);
    return m_pWinThread->MfcIsIdleMessage(pMsg);
}

LRESULT CMcl4MfcThread::ProcessWndProcException( CException *e, const MSG *pMsg) {
    ASSERT(m_pWinThread);
    return m_pWinThread->MfcProcessWndProcException( e, pMsg);
}

BOOL CMcl4MfcThread::ProcessMessageFilter( int code, LPMSG lpMsg) {
    ASSERT(m_pWinThread);
    return m_pWinThread->MfcProcessMessageFilter( code, lpMsg);
}

int CMcl4MfcThread::Run(void) {
    ASSERT(m_pWinThread);
    return m_pWinThread->MfcRun();
}

// functions which implement CMclThread functionality...
/////////////////////////////////////////////////////////////////////////

// suspend the thread...
DWORD CMcl4MfcThread::Suspend(void) {
    ASSERT(m_hHandle);
    return ::SuspendThread(m_hHandle);
}

// resume the thread...
DWORD CMcl4MfcThread::Resume(void) {
    ASSERT(m_hHandle);
    return ::ResumeThread(m_hHandle);
}

// terminate the thread...
BOOL CMcl4MfcThread::Terminate( DWORD dwExitCode) {
    ASSERT(m_hHandle);
    return ::TerminateThread( m_hHandle, dwExitCode);
}

// read a thread's exit code...
BOOL CMcl4MfcThread::GetExitCode( DWORD *pdwExitCode) {
    ASSERT(m_hHandle);
    return ::GetExitCodeThread( m_hHandle, pdwExitCode);
}

// set a thread's priority...
BOOL CMcl4MfcThread::SetPriority( int nPriority) {
    ASSERT(m_hHandle);
    return ::SetThreadPriority( m_hHandle, nPriority);
}    

// read a thread's priority...
int CMcl4MfcThread::GetPriority(void) {
    ASSERT(m_hHandle);
    return ::GetThreadPriority( m_hHandle);
}

// get the internal thread id...
DWORD CMcl4MfcThread::GetThreadId(void) {
    ASSERT(m_pWinThread);
    return m_pWinThread->m_nThreadID;
}

/////////////////////////////////////////////////////////////////////////
// CMcl4MfcWorkerThread functions...
/////////////////////////////////////////////////////////////////////////

// constructor...
// starts the worker thread executing the thread handler...
CMcl4MfcWorkerThread::CMcl4MfcWorkerThread( CMclThreadHandler *pcThreadHandler, 
                                            unsigned uInitFlag, 
                                            LPSECURITY_ATTRIBUTES lpSecurity, 
                                            unsigned uStackSize) {
    // set the thread handler...
    m_pcThreadHandler = pcThreadHandler;

    // create the OS thread...
    BOOL bStatus = m_pWinThread->CreateThread( uInitFlag, uStackSize, lpSecurity);

    // verify that OS thread was created...
    if (!bStatus) {
        m_dwStatus = ::GetLastError();
        ThrowError(m_dwStatus);
    }

    // save a copy of the handle to support use as
    // a CMclKernel object...
    DuplicateHandle( GetCurrentProcess(), m_pWinThread->m_hThread,
                     GetCurrentProcess(), &m_hHandle, 0, FALSE,
                     DUPLICATE_SAME_ACCESS );
}

int CMcl4MfcWorkerThread::Run(void) {
    // for worker threads, just call the ThreadHandlerProc,
    // no need to call the base class implementation, which
    // would result in running a message pump...
    ASSERT(m_pcThreadHandler);
    return m_pcThreadHandler->ThreadHandlerProc();
}

/////////////////////////////////////////////////////////////////////////
// CMcl4MfcGUIThread functions...
/////////////////////////////////////////////////////////////////////////

// constructor...
CMcl4MfcGUIThread::CMcl4MfcGUIThread(   unsigned uInitFlag, 
                                        LPSECURITY_ATTRIBUTES lpSecurity, 
                                        unsigned uStackSize) {
    // create the OS thread...
    BOOL bStatus = m_pWinThread->CreateThread( uInitFlag, uStackSize, lpSecurity);

    // verify that OS thread was created...
    if (!bStatus) {
        m_dwStatus = ::GetLastError();
        ThrowError(m_dwStatus);
    }

    // save a copy of the handle to support use as
    // a CMclKernel object...
    DuplicateHandle( GetCurrentProcess(), m_pWinThread->m_hThread,
                     GetCurrentProcess(), &m_hHandle, 0, FALSE,
                     DUPLICATE_SAME_ACCESS );
}

int CMcl4MfcGUIThread::Run(void) {
    // this function isn't really necessary since all it
    // does is call the base class implementation,
    // however it does make the code a little clearer and
    // serves to highlight the differences between the 
    // CMcl4MfcGUIThread and CMcl4MfcWorkerThread classes...
    return CMcl4MfcThread::Run();
}

// functions which implement CMclWinThread functionality
// which only make sense for GUI threads...

// get the main window for this thread...
CWnd *CMcl4MfcGUIThread::GetMainWnd(void) {
    ASSERT(m_pWinThread);
    return m_pWinThread->m_pMainWnd;
}

// set the main window for this thread...
void CMcl4MfcGUIThread::SetMainWnd(CWnd *pWnd) {
    ASSERT(m_pWinThread);
    m_pWinThread->m_pMainWnd = pWnd;
}

// post a message to this thread...
BOOL CMcl4MfcGUIThread::PostThreadMessage( UINT message, WPARAM wParam, LPARAM lParam) {
    ASSERT(m_pWinThread);

    // Versions of MFC included with VC++ before 4.2 do not define
    // the PostThreadMessage function on CWinThread, so we use the
    // Win32 API to make this work for all versions...
    //
    // ONLY FOR VC++ >= 4.2:
    // return m_pWinThread->PostThreadMessage( message, wParam, lParam);

    return ::PostThreadMessage( GetThreadId(), message, wParam, lParam);
}



