//
// FILE: Mcl4Mfc.h
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////
#ifndef __MCL4MFC_H__
#define __MCL4MFC_H__

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include "CMcl.h"           // Mcl wrapper classes

// forward declaration of classes...
class CMcl4MfcThread;

// derived version of CWinThread for use with Mcl library...
class CMclWinThread : public CWinThread {
    // allow CMcl4MfcThread to call the protected functions
    // and directly access the internal data of CMclWinThread...
    friend class CMcl4MfcThread;

protected:
    CMcl4MfcThread *m_pOwner;

public:
    // constructor...
    CMclWinThread(CMcl4MfcThread *pOwner);

    // overridables, default behavior is to 
    // call the same function in the pOwner object...
    virtual BOOL InitInstance(void);
    virtual int ExitInstance(void);
    virtual BOOL OnIdle( LONG lCount);
    virtual BOOL PreTranslateMessage( MSG *pMsg);
    virtual BOOL IsIdleMessage( MSG *pMsg);
    virtual LRESULT ProcessWndProcException( CException *e, const MSG *pMsg);
    virtual BOOL ProcessMessageFilter( int code, LPMSG lpMsg);
    virtual int Run(void);

protected:
    // these functions call the original MFC base 
    // class implementations...
    virtual BOOL MfcInitInstance(void);
    virtual int MfcExitInstance(void);
    virtual BOOL MfcOnIdle( LONG lCount);
    virtual BOOL MfcPreTranslateMessage( MSG *pMsg);
    virtual BOOL MfcIsIdleMessage( MSG *pMsg);
    virtual LRESULT MfcProcessWndProcException( CException *e, const MSG *pMsg);
    virtual BOOL MfcProcessMessageFilter( int code, LPMSG lpMsg);
    virtual int MfcRun(void);
};

// CMcl4MfcThread base class...
class CMcl4MfcThread : public CMclKernel {
protected:
    // pointer to internal CWinThread derived object...
    // this base class will use aggregation and delegation
    // techniques to provide the functionality of both
    // CMclThread and CWinThread...
    CMclWinThread *m_pWinThread;

public:
    // base class constructor...
    CMcl4MfcThread();

    // destructor to clean up CWinThread object...
    // declared pure virtual to prevent
    // creating an instance of this class...
    virtual ~CMcl4MfcThread() = 0;

    // overrideable virtual functions...
    // the default implmentation calls the member function
    // in the CMclWinThread object which calls the original
    // CWinThread implementation...
    // derived classes which override these functions should
    // usually also call the base class implementations...
    virtual BOOL InitInstance(void);
    virtual int ExitInstance(void);
    virtual BOOL OnIdle( LONG lCount);
    virtual BOOL PreTranslateMessage( MSG *pMsg);
    virtual BOOL IsIdleMessage( MSG *pMsg);
    virtual LRESULT ProcessWndProcException( CException *e, const MSG *pMsg);
    virtual BOOL ProcessMessageFilter( int code, LPMSG lpMsg);
    virtual int Run(void);

    // functions which implement CMclThread functionality...

    // suspend the thread...
    DWORD Suspend(void);

    // resume the thread...
    DWORD Resume(void);

    // terminate the thread...
    BOOL Terminate( DWORD dwExitCode);

    // read a thread's exit code...
    BOOL GetExitCode( DWORD *pdwExitCode);

    // set a thread's priority...
    BOOL SetPriority( int nPriority);

    // read a thread's priority...
    int GetPriority(void);

    // get the internal thread id...
    DWORD GetThreadId(void);
};

// CMcl4MfcWorkerThread class is used like CMclThread...
class CMcl4MfcWorkerThread : public CMcl4MfcThread {
protected:
    CMclThreadHandler *m_pcThreadHandler;

public:
    // class constructor...
    CMcl4MfcWorkerThread(   CMclThreadHandler *pcThreadHandler, 
                            unsigned uInitFlag = 0, 
                            LPSECURITY_ATTRIBUTES lpSecurity = NULL, 
                            unsigned uStackSize = 0);


protected:
    // this function overriden to call the CMclThreadHandler object...
    virtual int Run(void);
};

// CMcl4MfcGUIThread class is used like CWinThread...
class CMcl4MfcGUIThread : public CMcl4MfcThread {
public:
    // class constructor...
    CMcl4MfcGUIThread(  unsigned uInitFlag = 0, 
                        LPSECURITY_ATTRIBUTES lpSecurity = NULL, 
                        unsigned uStackSize = 0);

    // this function starts the CWinThread message pump...
    virtual int Run(void);

    // functions which implement CMclWinThread functionality
    // which only make sense for GUI threads...

    // get the main window for this thread...
    CWnd *GetMainWnd(void);

    // set the main window for this thread...
    void SetMainWnd(CWnd *pWnd);

    // post a message to this thread...
    BOOL PostThreadMessage( UINT message, WPARAM wParam, LPARAM lParam);
};

// typedef for the autopointers we need for 
// CMcl4MfcThread derived threads...
typedef CMclDerivedAutoPtr<CMcl4MfcThread> CMcl4MfcThreadAutoPtr;
typedef CMclDerivedAutoPtr<CMcl4MfcWorkerThread> CMcl4MfcWorkerThreadAutoPtr;
typedef CMclDerivedAutoPtr<CMcl4MfcGUIThread> CMcl4MfcGUIThreadAutoPtr;

#endif


