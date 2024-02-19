/*
Module : TSafe.H
Purpose: Defines the interface for a collection of MFC classes to provide thread safe access 
         to objects. The classes are based on the code presented in Chapter Ten of the book
         "Programming Applications for Microsoft Windows" by Jeffrey Richter. 
Created: PJN / 26-11-2002

All rights reserved.

Copyright (c) 1999 - 2009 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 
*/


/////////////////////////////// Macros / Defines //////////////////////////////

#pragma once

#ifndef __TSAFE_H__
#define __TSAFE_H__

#ifndef __AFXMT_H__
#include <afxmt.h>
#pragma message("To avoid this message, please put afxmt.h in your pre compiled header (normally stdafx.h)")
#endif

#ifndef CCRITICALSECTIONEX_EXT_CLASS
#define CCRITICALSECTIONEX_EXT_CLASS
#endif


/////////////////////////////// Classes ///////////////////////////////////////

//Extended version of MFC's CCriticalSection class. This version provides 
//debug information, timeouts and spin counts

class CCRITICALSECTIONEX_EXT_CLASS CCriticalSectionEx : public CSyncObject
{
public:
//Constructor / Destructors
	CCriticalSectionEx();
  CCriticalSectionEx(DWORD dwSpinCount);
	virtual ~CCriticalSectionEx();

//Methods
	operator CRITICAL_SECTION*();
	BOOL Unlock();
	BOOL Lock();
	BOOL Lock(DWORD dwTimeout);
  bool IsLocked() const { return m_bLocked; };
  DWORD SetSpinCount(DWORD dwSpinCount);

//Static Methods
  static BOOL SpinCountsAvailable();
  static BOOL TimeoutsAvailable();
  static DWORD NumberOfProcessors();

//Member variables
	CRITICAL_SECTION m_sect;

protected:
	DECLARE_DYNAMIC(CCriticalSectionEx)

//Methods
  BOOL Init(DWORD dwSpinCount);

//Member variables
  bool m_bLocked; //true if we have acquired access to the critical section, otherwise false
};


//Class which provides a thread safe wrapper for any C++ object

template <class TYPE>
class CThreadSafe : public CCriticalSectionEx
{
public:
//Constructors / Destructors
  CThreadSafe() {};
  CThreadSafe(const TYPE& value) : CCriticalSectionEx(),
                                   m_Value(value) 
  {
  }

//Methods
  operator TYPE()
  {
    CSingleLock sl(this, TRUE);
    return m_Value;
  }
  
  TYPE* operator&()
  {
    //Its debatable whether or not we should check that the critical section
    //has been locked when you take the address of the object. Here, we do
    //ASSERT. If you do not want this, then take the address of the "m_Value"
    //member variable explicitly
    ASSERT(IsLocked()); //You forgot to call Lock before calling this function!
    return &m_Value;
  }

  const TYPE& GetValue() const
  {
    ASSERT(IsLocked()); //You forgot to call Lock before calling this function!
    return m_Value;
  }

  void SetValue(const TYPE& NewValue)
  {
    ASSERT(IsLocked());  //You forgot to call Lock before calling this function!
    const TYPE& Value = GetValue();
    if (NewValue != Value) //As an optimization only change if different from the previous value
    {
      const TYPE& OldValue = Value;
      m_Value = NewValue;
      OnChanged(NewValue, OldValue); 
    }
  }

//Member variables
  TYPE m_Value; //The encapsulated data

protected:
//Methods
  virtual void OnChanged(const TYPE& /*NewValue*/, const TYPE& /*OldValue*/) {};
};


// A derived version of CThreadSafe which encapsulates a C++ scalar variable (which is thread safe)

template <class TYPE>
class CThreadSafeScalar : public CThreadSafe<TYPE>
{
public:
//Constructors / Destructors
  CThreadSafeScalar(const TYPE& data = 0) : CThreadSafe<TYPE>(data) 
  {
  }
  
  CThreadSafeScalar(CThreadSafeScalar<TYPE>& data)
  {
    operator=(data);
  }

//Methods
  operator TYPE()
  {
    return CThreadSafe<TYPE>::operator TYPE();
  }

  int operator==(const TYPE& value)
  {
    CSingleLock sl(this, TRUE);
    return m_Value == value;
  }

  int operator!=(const TYPE& value)
  {
    CSingleLock sl(this, TRUE);
    return m_Value != value;
  }

  CThreadSafeScalar<TYPE>& operator=(const TYPE& value)
  {
    CSingleLock sl(this, TRUE);
    SetValue(value);
    return *this;
  }
  
  CThreadSafeScalar<TYPE>& operator=(CThreadSafeScalar<TYPE>& value)
  {
    CSingleLock sl(this, TRUE);
    SetValue(value.operator TYPE());
    return *this;
  }

  CThreadSafeScalar<TYPE>& operator++() //Prefix
  {
    CSingleLock sl(this, TRUE);
    SetValue(GetValue() + 1);
    return *this;
  }
  
  CThreadSafeScalar<TYPE> operator++(int) //Postfix
  {
    CSingleLock sl(this, TRUE);
    CThreadSafeScalar<TYPE> OldValue(GetValue());
    SetValue(OldValue + 1);
    return OldValue;
  }

  CThreadSafeScalar<TYPE> operator+(const TYPE& value)
  {
    CSingleLock sl(this, TRUE);
    return CThreadSafeScalar<TYPE>(GetValue() + value);
  }

  CThreadSafeScalar<TYPE>& operator+=(TYPE value)
  {
    CSingleLock sl(this, TRUE);
    SetValue(GetValue() + value);
    return *this;
  }

  CThreadSafeScalar<TYPE>& operator--() //Prefix
  {
    CSingleLock sl(this, TRUE);
    SetValue(GetValue() - 1);
    return *this;
  }

  CThreadSafeScalar<TYPE> operator--(int) //Postfix
  {
    CSingleLock sl(this, TRUE);
    CThreadSafeScalar<TYPE> OldValue(GetValue());
    SetValue(OldValue - 1);
    return OldValue;
  }

  CThreadSafeScalar<TYPE> operator-(const TYPE& value)
  {
    CSingleLock sl(this, TRUE);
    return CThreadSafeScalar<TYPE>(GetValue() - value);
  }

  CThreadSafeScalar<TYPE>& operator-=(const TYPE& value)
  {
    CSingleLock sl(this, TRUE);
    SetValue(GetValue() - value);
    return *this;
  }

  CThreadSafeScalar<TYPE> operator*(const TYPE& value)
  {
    CSingleLock sl(this, TRUE);
    return CThreadSafeScalar<TYPE>(GetValue() * value);
  }

  CThreadSafeScalar<TYPE>& operator*=(const TYPE& value)
  {
    CSingleLock sl(this, TRUE);
    SetValue(GetValue() * value);
    return *this;
  }

  CThreadSafeScalar<TYPE> operator/(const TYPE& value)
  {
    CSingleLock sl(this, TRUE);
    return GetValue() / value;
  }

  CThreadSafeScalar<TYPE>& operator/=(const TYPE& value)
  {
    CSingleLock sl(this, TRUE);
    SetValue(GetValue() / value);
    return *this;
  }

  CThreadSafeScalar<TYPE> operator%(const TYPE& value)
  {
    CSingleLock sl(this, TRUE);
    return CThreadSafeScalar<TYPE>(GetValue() % value);
  }

  CThreadSafeScalar<TYPE>& operator%=(const TYPE& value)
  {
    CSingleLock sl(this, TRUE);
    SetValue(GetValue() % value);
    return *this;
  }

  CThreadSafeScalar<TYPE> operator^(const TYPE& value)
  {
    CSingleLock sl(this, TRUE);
    return CThreadSafeScalar<TYPE>(GetValue() ^ value);
  }

  CThreadSafeScalar<TYPE>& operator^=(const TYPE& value)
  {
    CSingleLock sl(this, TRUE);
    SetValue(GetValue() ^ value);
    return *this;
  }

  CThreadSafeScalar<TYPE> operator&(const TYPE& value)
  {
    CSingleLock sl(this, TRUE);
    return CThreadSafeScalar<TYPE>(GetValue() & value);
  }

  CThreadSafeScalar<TYPE>& operator&=(const TYPE& value)
  {
    CSingleLock sl(this, TRUE);
    SetValue(GetValue() & value);
    return *this;
  }

  CThreadSafeScalar<TYPE> operator|(const TYPE& value)
  {
    CSingleLock sl(this, TRUE);
    return CThreadSafeScalar<TYPE>(GetValue() | value);
  }

  CThreadSafeScalar<TYPE>& operator|=(const TYPE& value)
  {
    CSingleLock sl(this, TRUE);
    SetValue(GetValue() | value);
    return *this;
  }

  CThreadSafeScalar<TYPE> operator<<(const TYPE& value)
  {
    CSingleLock sl(this, TRUE);
    return GetValue() << value;
  }

  CThreadSafeScalar<TYPE>& operator<<=(const TYPE& value)
  {
    CSingleLock sl(this, TRUE);
    SetValue(GetValue() << value);
    return *this;
  }

  CThreadSafeScalar<TYPE> operator>>(const TYPE& value)
  {
    CSingleLock sl(this, TRUE);
    return CThreadSafeScalar<TYPE>(GetValue() >> value);
  }

  CThreadSafeScalar<TYPE>& operator>>=(const TYPE& value)
  {
    CSingleLock sl(this, TRUE);
    SetValue(GetValue() >> value);
    return *this;
  }
};

#endif //__TSAFE_H__
