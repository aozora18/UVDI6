//
// FILE: CMclCritSec.cpp
//
// Copyright (c) 1997 by Aaron Michael Cohen
//
/////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "CMclCritSec.h"

// constructor creates a CRITICAL_SECTION inside
// the C++ object...
CMclCritSec::CMclCritSec(void) {
   // ::InitializeCriticalSection( &m_CritSec);
	if ( !::InitializeCriticalSectionAndSpinCount( &m_CritSec, 0x80000400) ){
		ASSERT(FALSE);		
	}

}

// destructor...
CMclCritSec::~CMclCritSec() {
    ::DeleteCriticalSection( &m_CritSec);
	
}

// enter the critical section...
void CMclCritSec::Enter(void) {	
    ::EnterCriticalSection( &m_CritSec);	
}

// leave the critical section...
void CMclCritSec::Leave(void) {
    ::LeaveCriticalSection( &m_CritSec);	
}

CRITICAL_SECTION *CMclCritSec::GetCritSec(void) {
    return &m_CritSec;
}

