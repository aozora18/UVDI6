#pragma once
#include <Windows.h>

// AXHS가 "#define HWND HANDLE" 못 하게 막는다.
#ifndef __AFX_H__
#define __AJIN_TMP_DEFINE_AFX_H__
#define __AFX_H__ 1
#endif

// ★ 치환 전에 AJIN_HANDLE을 '먼저' 선언
typedef void* AJIN_HANDLE;          // using 말고 typedef로 해도 OK

// ★ AXHS가 DWORD/WORD/BOOL/HANDLE 재정의 못 하게(충돌 예방)
#ifndef __DATA_TYPES__
#define __DATA_TYPES__ 1
#endif

// Ajin 헤더 안에서만 HANDLE → AJIN_HANDLE로 치환
#pragma push_macro("HANDLE")
#define HANDLE AJIN_HANDLE

#include "AXHS.h"
#include "AXM.h"    // (AXM.h가 AXHS.h를 내부에서 include해도 무방)

#pragma pop_macro("HANDLE")

#ifdef __AJIN_TMP_DEFINE_AFX_H__
#undef __AFX_H__
#undef __AJIN_TMP_DEFINE_AFX_H__
#endif