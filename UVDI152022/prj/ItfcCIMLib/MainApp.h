
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.


class CMainApp : public CWinApp
{
public:
	CMainApp();

/* 가상 함수 재정의 */
public:
	virtual BOOL		InitInstance();
	virtual INT32		ExitInstance();

/* 로컬 변수 */
protected:

/* 로컬 함수 */
protected:

/* 공용 함수 */
public:


/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
};
