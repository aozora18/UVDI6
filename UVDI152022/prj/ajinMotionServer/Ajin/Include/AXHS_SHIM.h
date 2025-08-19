#pragma once
#include <Windows.h>

// AXHS�� "#define HWND HANDLE" �� �ϰ� ���´�.
#ifndef __AFX_H__
#define __AJIN_TMP_DEFINE_AFX_H__
#define __AFX_H__ 1
#endif

// �� ġȯ ���� AJIN_HANDLE�� '����' ����
typedef void* AJIN_HANDLE;          // using ���� typedef�� �ص� OK

// �� AXHS�� DWORD/WORD/BOOL/HANDLE ������ �� �ϰ�(�浹 ����)
#ifndef __DATA_TYPES__
#define __DATA_TYPES__ 1
#endif

// Ajin ��� �ȿ����� HANDLE �� AJIN_HANDLE�� ġȯ
#pragma push_macro("HANDLE")
#define HANDLE AJIN_HANDLE

#include "AXHS.h"
#include "AXM.h"    // (AXM.h�� AXHS.h�� ���ο��� include�ص� ����)

#pragma pop_macro("HANDLE")

#ifdef __AJIN_TMP_DEFINE_AFX_H__
#undef __AFX_H__
#undef __AJIN_TMP_DEFINE_AFX_H__
#endif