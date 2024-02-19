/*
Module : TSafe.cpp
Purpose: Defines the implementation for a collection of MFC classes to provide thread safe access 
         to objects. The classes are based on the code presented in Chapter Ten in the book
         "Programming Applications for Microsoft Windows" by Jeffrey Richter. 
Created: PJN / 26-11-2002
History: 22-12-2006 1. Updated copyright details
                    2. Optimized CThreadSafe constructor code
                    3. Code now uses newer C++ style casts instead of C style casts.
                    4. Updated the code to clean compile on VC 2005
                    5. Made CCriticalSectionEx::Lock and Unlock non virtual
                    6. Optimized _CRITICAL_SECTION_DATA constructor code
                    7. CCriticalSectionEx constructors now throws an exception using AfxThrowMemoryException
                    instead of AfxThrowResourceException
                    8. Removed unnecessary CThreadSafe destructor
                    9. Removed unnecessary CThreadSafeScalar destructor
                    10. Added postfix operator support to CThreadSafeScalar
                    11. Addition of a operator TYPE*() in CThreadSafe
                    12. m_Value member variable of CThreadSafe is now public
                    13. Addition of a CCRITICALSECTIONEX_EXT_CLASS preprocessor macro to allow the classes 
                    to be more easily added to an extension dll
         11-01-2009 1. Updated copyright details
                    2. Removed VC 6 style AppWizard comments from the code
                    3. The code has now been updated to support VC 2005 or later only. It seems that even 
                    though ATL's version of CCriticalSection now supports spin locks the MFC version of
                    CCriticalSection still does not support them, hence still the need for CCriticalSectionEx.
                    4. Fixed a bug in CCriticalSectionEx::Lock where it handled the wrong
                    SEH type being thrown. The code now only handles the STATUS_NO_MEMORY exception
                    5. Code now compiles cleanly using Code Analysis (/analyze)
                    6. Removed the bPreallocateEvent parameter from the CCriticalSectionEx::Init and 
                    CCriticalSectionEx's overridden constructor.
                    7. Removed the call to Unlock in the CCriticalSectionEx destructor. This is to be
                    consistent with MFC's CCriticalSection class.
                    8. CCriticalSectionEx::Lock now throws a CMemoryException* exception. This is to be
                    consistent with MFC's CCriticalSection class.
                    9. The m_bLocked member variable of CCriticalSectionEx is now a bool instead of a BOOL.
                    The IsLocked method has also been updated to return a bool.
                    
Copyright (c) 1999 - 2009 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 
*/


/////////////////////////////// Includes //////////////////////////////////////

#include "pch.h"
#include "TSafe.h"


/////////////////////////////// Macros / Defines //////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/////////////////////////////// Implementation //////////////////////////////////////

//Class which handles dynamically calling functions which must be constructed at run time
//since we do not want to depend on the NT / Win2k specific Critical Section functions. 
//To avoid the loader bringing up a message such as "Failed to load due to missing 
//export...", the functions are constructed using GetProcAddress. The CCriticalSectionEx 
//code then checks to see if these function pointers valid at runtime. Note that generally 
//it is a bad idea to put code which calls LoadLibrary in the constructor of a global object,
//namely _CriticalSectionData in this case, because it could be operating under so called 
//"Loader Lock", if the code is compiled into a DLL. In this particular case we are ok, 
//because we are calling GetModuleHandle and not LoadLibrary and we are only getting 
//function pointers in Kernel32. Doing anything more non-trivial than this would potentially 
//be bad and we would then have to implement this code at a later stage such as in the 
//CCriticalSectionEx constructor

class _CRITICAL_SECTION_DATA
{
public:
//Constructors /Destructors
  _CRITICAL_SECTION_DATA();

//typedefs
  typedef BOOL (__stdcall INITIALIZECRITICALSECTIONANDSPINCOUNT)(LPCRITICAL_SECTION, DWORD);
  typedef INITIALIZECRITICALSECTIONANDSPINCOUNT* LPINITIALIZECRITICALSECTIONANDSPINCOUNT;
  typedef DWORD (__stdcall SETCRITICALSECTIONSPINCOUNT)(LPCRITICAL_SECTION, DWORD);
  typedef SETCRITICALSECTIONSPINCOUNT* LPSETCRITICALSECTIONSPINCOUNT;
  typedef BOOL (__stdcall TRYENTERCRITICALSECTION)(LPCRITICAL_SECTION);
  typedef TRYENTERCRITICALSECTION* LPTRYENTERCRITICALSECTION;

//Data
  LPINITIALIZECRITICALSECTIONANDSPINCOUNT m_lpfnInitializeCriticalSectionAndSpinCount;
  LPSETCRITICALSECTIONSPINCOUNT           m_lpfnSetCriticalSectionSpinCount;
  LPTRYENTERCRITICALSECTION               m_lpfnTryEnterCriticalSection;
  DWORD                                   m_dwNumberOfProcessors;
};


_CRITICAL_SECTION_DATA::_CRITICAL_SECTION_DATA() : m_lpfnInitializeCriticalSectionAndSpinCount(NULL),
                                                   m_lpfnSetCriticalSectionSpinCount(NULL),
                                                   m_lpfnTryEnterCriticalSection(NULL),
                                                   m_dwNumberOfProcessors(1)
{

  //Get the function pointers
  HMODULE hKernel32 = GetModuleHandle(_T("KERNEL32.DLL"));
  if (hKernel32)
  {
    m_lpfnInitializeCriticalSectionAndSpinCount = reinterpret_cast<LPINITIALIZECRITICALSECTIONANDSPINCOUNT>(GetProcAddress(hKernel32, "InitializeCriticalSectionAndSpinCount"));
    m_lpfnSetCriticalSectionSpinCount = reinterpret_cast<LPSETCRITICALSECTIONSPINCOUNT>(GetProcAddress(hKernel32, "SetCriticalSectionSpinCount"));
    m_lpfnTryEnterCriticalSection = reinterpret_cast<LPTRYENTERCRITICALSECTION>(GetProcAddress(hKernel32, "TryEnterCriticalSection"));
  }

  //Also get the number of processors
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  m_dwNumberOfProcessors = si.dwNumberOfProcessors;
}


//the one and only _CRITICAL_SECTION_DATA
_CRITICAL_SECTION_DATA _CriticalSectionData;


IMPLEMENT_DYNAMIC(CCriticalSectionEx, CSyncObject)

CCriticalSectionEx::CCriticalSectionEx() : CSyncObject(NULL), m_bLocked(false)
{
  //Always try to initialize the critical section with a spin count.

  //Note that the MSDN recommends a value of 4000 for a critical section which 
  //is very busy such as the critical section which the heap manager uses to 
  //serializes access to a process's heap. As a good compromise / default value 
  //we will use a quarter of that, namely 1000. Also remember on a Uniprocessor
  //machine for obvious reasons the spin count value is completely ignored 
  //anyway!
  if (!Init(4000))
    AfxThrowMemoryException();
}

CCriticalSectionEx::CCriticalSectionEx(DWORD dwSpinCount) : CSyncObject(NULL), m_bLocked(false)
{
  if (!Init(dwSpinCount))
    AfxThrowMemoryException();
}

#pragma warning(push)
#pragma warning(disable:6322) //To ensure that the code compiles cleanly using code analysis
BOOL CCriticalSectionEx::Init(DWORD dwSpinCount)
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  if (_CriticalSectionData.m_lpfnInitializeCriticalSectionAndSpinCount)
  {
    //Call the "InitializeCriticalSectionAndSpinCount" SDK API
    bSuccess = _CriticalSectionData.m_lpfnInitializeCriticalSectionAndSpinCount(&m_sect, dwSpinCount);
  }
  else
  {
    __try  
    {
       //Call the SDK
      ::InitializeCriticalSection(&m_sect);
      bSuccess = TRUE;
    }
    __except(GetExceptionCode() == STATUS_NO_MEMORY)
    {
    }
  }

  return bSuccess;
}
#pragma warning(pop)

CCriticalSectionEx::~CCriticalSectionEx()
{ 
  //Free up the critical section
  ::DeleteCriticalSection(&m_sect); 
}

CCriticalSectionEx::operator CRITICAL_SECTION*()
{ 
  return &m_sect; 
}

BOOL CCriticalSectionEx::Lock()
{
  __try  
  {
    ::EnterCriticalSection(&m_sect);
    m_bLocked = true;
  }
  __except(GetExceptionCode() == STATUS_NO_MEMORY)
  {
    AfxThrowMemoryException();
  }
  return TRUE;
}

BOOL CCriticalSectionEx::Lock(DWORD dwTimeout)
{
  //If any timeout is specified (other than INFINITE) then try to acquire 
  //the critical section using "TryEnterCriticalSection". Note that calling this 
  //function is effectively like waiting 0 milliseconds for the critical section. 
  //In other words currently the only usable values for dwTimeout are 0 and INFINITE. 
  //Maybe in a future Windows, MS will provide real timeout support for a
  //critical section. Maybe something like "WaitEnterCriticalSection"!
  if (_CriticalSectionData.m_lpfnTryEnterCriticalSection && dwTimeout != INFINITE)
  {
    BOOL bSuccess = _CriticalSectionData.m_lpfnTryEnterCriticalSection(&m_sect);

    //Only mark as locked if we obtained the lock
    if (bSuccess)
      m_bLocked = true;

    return bSuccess;
  }
  else
  {
    //if TryEnterCriticalSection is not available or we specified a timeout of
    //INFINITE, then just fall back to "EnterCriticalSection" and ignore the 
    //dwTimeout value send into us
    return Lock(); 
  }
}

BOOL CCriticalSectionEx::Unlock()
{ 
  ::LeaveCriticalSection(&m_sect); 
  m_bLocked = false;
  return TRUE; 
}

DWORD CCriticalSectionEx::SetSpinCount(DWORD dwSpinCount)
{
  if (_CriticalSectionData.m_lpfnSetCriticalSectionSpinCount)
    return _CriticalSectionData.m_lpfnSetCriticalSectionSpinCount(&m_sect, dwSpinCount);
  else
  {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
  }
}

BOOL CCriticalSectionEx::SpinCountsAvailable()
{
  return (_CriticalSectionData.m_lpfnSetCriticalSectionSpinCount != NULL) &&
         (_CriticalSectionData.m_lpfnInitializeCriticalSectionAndSpinCount != NULL);
}

BOOL CCriticalSectionEx::TimeoutsAvailable()
{
  return (_CriticalSectionData.m_lpfnTryEnterCriticalSection != NULL);
}

DWORD CCriticalSectionEx::NumberOfProcessors()
{
  return _CriticalSectionData.m_dwNumberOfProcessors;
}
